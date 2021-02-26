/*=============================================================================
  @file sns_qcm.c

  Incoming requests to the Framework will arrive via the QSocket Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  The module
  will translate incoming QSocket messages into Client Request objects,
  and similarly will translate Sensor Events into QSocket messages.

  PEND: Need to use two event fifos for each client stream.  One for recurring
  data which may be dropped, and another for non-recurrent (configuration)
  events which may not be dropped.  Not necessary for customers, as data
  will not be batched long enough to need to drop anything.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  include files
  ===========================================================================*/
#include <stdbool.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_atomic.h"
#include "sns_attribute_util.h"
#include "sns_data_stream.h"
#include "sns_fw_sensor.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_pb_util.h"
#include "sns_rc.h"
#include "sns_sensor_util.h"
#include "sns_std.pb.h"
#include "sns_std_type.pb.h"
#include "sns_stream_service.h"
#include "sns_suid_util.h"
#include "sns_types.h"

#include "sns_client.pb.h"
#include "sns_remote_proc_state.pb.h"
#include "sns_timer.pb.h"

#include "sns_qcm.h"
#include "sns_qcm_fifo.h"
#include "sns_qcm_service.h"

/*=============================================================================
  Macros
  ===========================================================================*/

/* Wait time between qsend retries (10ms) */
#define SNS_QCM_OOM_TIMER_NS 10000000

/*=============================================================================
  Static Data
  ===========================================================================*/

/* Should always be set to the processor on which QCM is running.  QCM assumes
 * that sending data to a co-located client is cheap/free, and therefore is
 * more liberal in sending batched data. */
static const sns_std_client_processor sns_qcm_proc_type SNS_SECTION(".rodata.sns")
    = SNS_STD_CLIENT_PROCESSOR_SSC;

/*=============================================================================
  Global Data
  ===========================================================================*/

const sns_sensor_uid qcm_sensor_suid SNS_SECTION(".rodata.sns")
    = {.sensor_uid = {QCM_SENSOR_SUID}};

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Create a new stream for an existing client.
 *
 * @param[o] stream_out Created stream
 *
 * @return
 * SNS_RC_INVALID_TYPE - SUID Not found
 * SNS_RC_SUCCESS
 */
SNS_SECTION(".text.sns") static sns_rc
create_client_stream(sns_qcm_state *state, sns_qcm_client *client,
    sns_sensor_uid *suid, sns_qcm_stream **stream_out)
{
  sns_rc rv;
  sns_isafe_list_iter iter;
  sns_qcm_stream *stream = NULL;

#ifdef SNS_ISLAND_INCLUDE_QCM
  stream = sns_malloc(SNS_HEAP_ISLAND, sizeof(*stream));
#endif
  if(NULL == stream)
  {
    SNS_ISLAND_EXIT();
    stream = sns_malloc(SNS_HEAP_MAIN, sizeof(*stream));
  }
  SNS_ASSERT(NULL != stream);

  stream->fifo = NULL;
  sns_memscpy(&stream->suid, sizeof(stream->suid), suid, sizeof(*suid));
  sns_qcm_fifo_init(&stream->fifo, UINT64_MAX);
  stream->client = client;

  rv = state->stream_service->api->create_sensor_stream(
      state->stream_service, state->sensor, stream->suid, &stream->data_stream);
  if(SNS_RC_SUCCESS != rv)
  {
    SNS_PRINTF(ERROR, state->sensor, "create_sensor_stream error %i", rv);
    sns_free(stream);
    stream = NULL;
    rv = SNS_RC_INVALID_TYPE;
  }
  else
  {
    sns_isafe_list_item_init(&stream->item, stream);
    sns_isafe_list_iter_init(&iter, &client->streams, false);
    sns_isafe_list_iter_insert(&iter, &stream->item, true);
  }

  *stream_out = stream;
  return rv;
}

/**
 * Close a client stream, and free all associated resources.
 */
SNS_SECTION(".text.sns") static void
close_client_stream(sns_qcm_state *state, sns_qcm_stream *stream)
{
  sns_isafe_list_item_remove(&stream->item);

  if(NULL != stream->data_stream)
  {
    state->stream_service->api->remove_stream(
        state->stream_service, stream->data_stream);
  }
  sns_qcm_fifo_deinit(stream->fifo);
  sns_free(stream);
}

/**
 * Find a client's stream object given a SUID.
 *
 * @return Found stream, or NULL
 */
SNS_SECTION(".text.sns") static sns_qcm_stream*
find_client_stream(sns_qcm_client *client, sns_sensor_uid *suid)
{
  sns_qcm_stream *stream = NULL;
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, &client->streams, true),
      stream = (sns_qcm_stream*)sns_isafe_list_iter_get_curr_data(&iter);
      NULL != stream;
      stream = (sns_qcm_stream*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
  {
    if(sns_sensor_uid_compare(&stream->suid, suid))
      break;
  }

  return stream;
}

/* See sns_qcm_service_connect */
SNS_SECTION(".text.sns") static sns_rc
service_connect(struct sns_qcm_conn *conn,
    void *service_cookie, void **connection_handle)
{
  sns_sensor *sensor = (sns_sensor*)service_cookie;
  sns_qcm_state *state = (sns_qcm_state*)sensor->state->state;
  sns_qcm_client *client = NULL;
  sns_isafe_list_iter iter;
  sns_osa_lock_attr lattr;
  sns_rc rc;

#ifdef SNS_ISLAND_INCLUDE_QCM
  client = sns_malloc(SNS_HEAP_ISLAND, sizeof(*client));
#endif

  if(NULL == client)
  {
    SNS_ISLAND_EXIT();
    client = sns_malloc(SNS_HEAP_MAIN, sizeof(*client));
  }
  SNS_ASSERT(NULL != client);

  SNS_PRINTF(LOW, sensor, "QSocket client connect %x (client %x)",
      conn, client);

  rc = sns_osa_lock_attr_init(&lattr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
#ifdef SNS_ISLAND_INCLUDE_QCM
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_ISLAND);
#else
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_NORMAL);
#endif
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_init(&lattr, &client->streams_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_isafe_list_init(&client->streams);
  client->conn = conn;
  client->busy = false;

  sns_isafe_list_item_init(&client->item, client);

  sns_osa_lock_acquire(&state->clients_lock);
  sns_isafe_list_iter_init(&iter, &state->clients, false);
  sns_isafe_list_iter_insert(&iter, &client->item, true);
  sns_osa_lock_release(&state->clients_lock);

  *connection_handle = client;

  return SNS_RC_SUCCESS;
}

/**
 * An existing client has disconnected.  Free the associated memory, and
 * close any open data streams.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] service_cookie As registered in sns_qcm_service_init
 */
SNS_SECTION(".text.sns") static void
service_disconnect(void *connection_handle, void *service_cookie)
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)service_cookie;
  sns_qcm_state *state = (sns_qcm_state*)sensor->sensor.state->state;
  sns_qcm_client *client = (sns_qcm_client*)connection_handle;
  sns_isafe_list_iter iter;

  if(!sns_island_is_island_ptr((intptr_t)client))
    SNS_ISLAND_EXIT();

  SNS_PRINTF(LOW, state->sensor, "QSocket client disconnect %x", client);

  sns_osa_lock_acquire(&state->clients_lock);
  sns_isafe_list_item_remove(&client->item);
  sns_osa_lock_release(&state->clients_lock);

  sns_isafe_list_iter_init(&iter, &client->streams, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_qcm_stream *stream =
      sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&iter));

    if(NULL != stream->data_stream)
    {
      sns_osa_lock_acquire(sensor->library->library_lock);
      state->stream_service->api->remove_stream(
          state->stream_service, stream->data_stream);
      stream->data_stream = NULL;
      sns_osa_lock_release(sensor->library->library_lock);
    }

    close_client_stream(state, stream);
  }

  sns_osa_lock_deinit(&client->streams_lock);
  sns_free(client);
}

/**
 * Gets the processor info structure for a processor type.  Creates a new entry
 * if not already present.
 *
 * @param[i] state QCM State
 * @param[i] proc_type The processor type
 *
 * @return Found Processor; never NULL
 */
SNS_SECTION(".text.sns") static sns_qcm_proc*
find_processor(sns_qcm_state *state, sns_std_client_processor proc_type)
{
  sns_isafe_list_iter iter;
  sns_qcm_proc *proc = NULL;

  sns_osa_lock_acquire(&state->processors_lock);
  for(sns_isafe_list_iter_init(&iter, &state->processors, true),
      proc = (sns_qcm_proc*)sns_isafe_list_iter_get_curr_data(&iter);
      NULL != proc;
      proc = (sns_qcm_proc*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
  {
    if(proc->proc_type == proc_type)
      break;
  }
  sns_osa_lock_release(&state->processors_lock);

  if(NULL == proc)
  {
#ifdef SNS_ISLAND_INCLUDE_QCM
    proc = sns_malloc(SNS_HEAP_ISLAND, sizeof(*proc));
#endif
    if(NULL == proc)
    {
      SNS_ISLAND_EXIT();
      proc = sns_malloc(SNS_HEAP_MAIN, sizeof(*proc));
    }
    SNS_ASSERT(NULL != proc);

    sns_isafe_list_item_init(&proc->item, proc);
    proc->proc_type = proc_type;
    proc->is_suspended = false;
    proc->next_batch_time = 0;
    proc->min_batch_period = 0;

    sns_osa_lock_acquire(&state->processors_lock);
    sns_isafe_list_iter_init(&iter, &state->processors, false);
    sns_isafe_list_iter_insert(&iter, &proc->item, true);
    sns_osa_lock_release(&state->processors_lock);
  }

  return proc;
}

/**
 * Notify a processor that it's list of clients has changed, and/or
 * a client's batch_period has been modified.  Perform all necessary
 * book-keeping, and update the registered timer accordingly.
 */
SNS_SECTION(".text.sns") static void
update_processor(sns_qcm_proc *proc)
{
  UNUSED_VAR(proc);
  // PEND: Inspect all streams associated with this processor; update
  //       next_batch_time & min_batch_period to reflect new client
  // PEND: Update batch timer request (if necessary)
}

/**
 * Initiate a flush operation.  Send a flush request to all client streams
 * associated with this processor.  Upon receipt of the flush event, send
 * the final batched events, concluding with a flush event.
 */
SNS_SECTION(".text.sns") static void
initiate_flush(sns_qcm_proc *proc)
{
  UNUSED_VAR(proc);
  // PEND: Send flush requests on all applicable streams
  // PEND: Send data for all streams associated with this processor
}

/**
 * Send a request to the Timer Sensor for 10ms.
 */
SNS_SECTION(".text.sns") static void
send_timer_request(sns_qcm_state *state)
{
  unsigned int oom_active = 0;

  if(atomic_compare_exchange_strong(&state->oom_active, &oom_active, 1))
  {
    sns_rc rc;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    size_t encoded_len;
    uint8_t buffer[sns_timer_sensor_config_size + 20];

    req_payload.is_periodic = false;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = sns_convert_ns_to_ticks(SNS_QCM_OOM_TIMER_NS);

    encoded_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
        sns_timer_sensor_config_fields, NULL);

    if(encoded_len > 0)
    {
      sns_request timer_req={ .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request_len = encoded_len, .request = buffer};

      rc = state->timer_stream->api->send_request(state->timer_stream, &timer_req);
      SNS_PRINTF(LOW, state->sensor, "Sent OOM timer request %i", rc);
    }
  }
}

/**
 * Encode a sensor event.
 */
SNS_SECTION(".text.sns") static bool
encode_event(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_sensor_event *event = (sns_sensor_event*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, (pb_byte_t*)event->event, event->event_len))
    return false;

  return true;
}

/**
 * Encode a single sns_client_event_msg_sns_client_event.
 */
SNS_SECTION(".text.sns") static bool
encode_client_event(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_sensor_event *event = (sns_sensor_event*)*arg;
  sns_client_event_msg_sns_client_event sub_message =
    sns_client_event_msg_sns_client_event_init_default;

  sub_message.msg_id = event->message_id;
  sub_message.timestamp = event->timestamp;
  sub_message.payload.funcs.encode = &encode_event;
  sub_message.payload.arg = event;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_submessage(stream,
        sns_client_event_msg_sns_client_event_fields, &sub_message))
    return false;

  return true;
}

/**
 * Encode as many buffered stream events as possible in the PB stream.
 * Each encoded entry has type sns_client_event_msg_sns_client_event.
 */
SNS_SECTION(".text.sns") static bool
encode_client_events(pb_ostream_t *ostream, const pb_field_t *field,
    void *const *arg)
{
  sns_qcm_stream *stream = (sns_qcm_stream*)*arg;
  uint32_t max_ind_len = sns_qcm_service_max_ind_len(stream->client->conn);

  for(sns_sensor_event *event = sns_qcm_fifo_remove(stream->fifo, false);
      NULL != event;
      event = sns_qcm_fifo_remove(stream->fifo, true))
  {
    sns_client_event_msg_sns_client_event sub_message =
      sns_client_event_msg_sns_client_event_init_default;

    sub_message.msg_id = event->message_id;
    sub_message.timestamp = event->timestamp;
    sub_message.payload.funcs.encode = &encode_event;
    sub_message.payload.arg = event;

    // PEND: Use fully encoded length (rather than assuming meta data is 10 bytes)
    if(max_ind_len < ostream->bytes_written + event->event_len + 10)
      break;
    else if(!pb_encode_tag_for_field(ostream, field))
      break;
    if(!pb_encode_submessage(ostream,
          sns_client_event_msg_sns_client_event_fields, &sub_message))
      break;
  }

  // NanoPB encodes messages twice, first to get the message size, and second
  // for real.  We only want to actually drop events in the latter case.
  // PEND: We need to optimize this code path so we only encode once
  if(NULL == ostream->callback)
    sns_qcm_fifo_commit(stream->fifo, SNS_QCM_COMMIT_UNDO);

  return true;
}

/**
 * Send all batched data for a client stream.  If no data is present,
 * send nothing.
 *
 * @return Whether the connection has been closed by the client
 */
SNS_SECTION(".text.sns") static bool
send_stream_data(sns_qcm_state *state, sns_qcm_stream *stream)
{
  bool exit_loop = false;
  bool rv = false;

  while(!exit_loop)
  {
    sns_rc rc = SNS_RC_SUCCESS;
    sns_client_event_msg msg = sns_client_event_msg_init_default;

    if(NULL == sns_qcm_fifo_remove(stream->fifo, false))
    {
      exit_loop = true;
    }
    else
    {
      sns_memscpy(&msg.suid, sizeof(msg.suid),
                  &stream->suid, sizeof(stream->suid));
      msg.events.funcs.encode = &encode_client_events;
      msg.events.arg = stream;

      rc = sns_qcm_service_send(stream->client->conn, &msg);
      if(SNS_RC_NOT_AVAILABLE == rc || SNS_RC_FAILED == rc)
      {
        stream->client->busy = true;
        exit_loop = true;

        if(SNS_RC_FAILED == rc)
          send_timer_request(state);
      }
      else if(SNS_RC_INVALID_STATE == rc)
      {
        rv = true;
        exit_loop = true;
      }
      else if(SNS_RC_INVALID_VALUE == rc)
      {
        SNS_PRINTF(ERROR, state->sensor,
          "Encode error; dropping all buffered data");
        while(NULL != sns_qcm_fifo_remove(stream->fifo, true))
          ;
        rc = SNS_RC_SUCCESS;
      }
    }

    sns_qcm_fifo_commit(stream->fifo,
        SNS_RC_SUCCESS == rc ? SNS_QCM_COMMIT_DONE : SNS_QCM_COMMIT_RELEASE);
  }

  return rv;
}

/**
 * Parse an incoming client request, and save the configuration in the stream
 * object.
 *
 * @param[i] sensor QCM Sensor (for printf)
 * @param[i] client Client which sent this request
 * @param[i] req Encoded request message of type sns_client_request_msg
 * @param[i] req_len Length (in bytes) of req
 * @param[o] stream_out The existing or newly created stream
 * @param[o] msg_id Decoded message ID
 *
 * @return
 * SNS_RC_SUCCESS
 * SNS_RC_FAILED -  Error decoding request
 * SNS_RC_INVALID_TYPE - Unable to create data stream
 */
SNS_SECTION(".text.sns") static sns_rc
parse_req(sns_fw_sensor *sensor, sns_qcm_client *client, void *req,
    uint32_t req_len, sns_qcm_stream **stream_out, uint32_t *msg_id)
{
  sns_qcm_state *state = (sns_qcm_state*)sensor->sensor.state->state;
  sns_client_request_msg req_msg = sns_client_request_msg_init_default;
  pb_istream_t istream;
  sns_qcm_stream *stream = NULL;
  sns_rc rv = SNS_RC_SUCCESS;

  istream = pb_istream_from_buffer(req, req_len);
  if(!pb_decode(&istream, sns_client_request_msg_fields, &req_msg))
  {
    SNS_PRINTF(ERROR, state->sensor, "Decoding failed");
    rv = SNS_RC_FAILED;
  }
  else
  {
    stream = find_client_stream(client, (sns_sensor_uid*)&req_msg.suid);
    if(NULL == stream)
      rv = create_client_stream(state, client,
          (sns_sensor_uid*)&req_msg.suid, &stream);

    *msg_id = req_msg.msg_id;

    if(NULL != stream)
    {
      send_stream_data(state, stream);
      stream->processor = find_processor(state,
          req_msg.susp_config.client_proc_type);
      stream->wakeup =
        SNS_CLIENT_DELIVERY_WAKEUP == req_msg.susp_config.delivery_type;

      if(req_msg.request.has_batching)
      {
        stream->batch_period = sns_convert_ns_to_ticks(
            req_msg.request.batching.batch_period * 1000ULL);
        stream->flush_period = sns_convert_ns_to_ticks(
            req_msg.request.batching.flush_period * 1000ULL);

        if(!req_msg.request.batching.has_flush_period)
        {
          if(0 != req_msg.request.batching.batch_period)
            stream->flush_period = stream->batch_period;
          else
            stream->flush_period = UINT64_MAX;
        }

        stream->flush_only = req_msg.request.batching.flush_only;

        sns_qcm_fifo_init(&stream->fifo, stream->flush_period);
        update_processor(stream->processor);
      }
    }
  }

  *stream_out = stream;

  return rv;
}

/**
 * A new request from an existing client has arrived.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] req_handle UNUSED
 * @param[i] req Encoded request message of type sns_client_request_msg
 * @param[i] req_len Length (in bytes) of req
 * @param[i] service_cookie As registered in sns_qcm_service_init
 *
 * @return See sns_qcm_service_req
 */
SNS_SECTION(".text.sns") static sns_rc
service_request(void *connection_handle, void *req_handle,
    void *req, uint32_t req_len, void *service_cookie)
{
  UNUSED_VAR(req_handle);
  sns_fw_sensor *sensor = (sns_fw_sensor*)service_cookie;
  sns_qcm_state *state = (sns_qcm_state*)sensor->sensor.state->state;
  sns_qcm_client *client = (sns_qcm_client*)connection_handle;
  sns_qcm_stream *stream = NULL;
  uint32_t msg_id_int = 0;
  sns_rc rv = SNS_RC_SUCCESS;

  if(!sns_island_is_island_ptr((intptr_t)client))
    SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(sensor->library->library_lock);
  sns_osa_lock_acquire(&client->streams_lock);

  rv = parse_req(sensor, client, req, req_len, &stream, &msg_id_int);
  if(SNS_RC_SUCCESS != rv)
  {
    SNS_PRINTF(ERROR, state->sensor, "parse_req error %i", rv);
  }
  else
  {
    if(SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ == msg_id_int)
    {
      close_client_stream(state, stream);
    }
    else
    {
      pb_wire_type_t wire_type;
      bool eof = false;
      uint32_t tag;
      pb_istream_t istream = pb_istream_from_buffer(req, req_len);

      while(pb_decode_tag(&istream, &wire_type, &tag, &eof) && !eof)
      {
        if(PB_WT_STRING == wire_type && sns_client_request_msg_request_tag == tag)
        {
          pb_istream_t sub_stream;
          if(pb_make_string_substream(&istream, &sub_stream))
          {
            sns_request request = (sns_request)
              { .message_id = msg_id_int, .request = sub_stream.state,
                .request_len = sub_stream.bytes_left };

            stream->data_stream->api->send_request(stream->data_stream, &request);
          }
          break;
        }
        pb_skip_field(&istream, wire_type);
      }
    }
  }

  sns_osa_lock_release(&client->streams_lock);
  sns_osa_lock_release(sensor->library->library_lock);

  return rv;
}

/**
 * A client connection is now available again to send events.  Will be called
 * at some point after SNS_RC_NOT_AVAILABLE is returned from
 * sns_qcm_service_send.
 *
 * @param[i] connection_handle As registered from sns_qcm_service_connect
 * @param[i] service_cookie As registered in sns_qcm_service_init
 */
SNS_SECTION(".text.sns") static void
service_resume(void *connection_handle, void *service_cookie)
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)service_cookie;
  sns_qcm_state *state = (sns_qcm_state*)sensor->sensor.state->state;
  sns_isafe_list_iter iter;
  sns_qcm_stream *stream;
  sns_qcm_client *client = (sns_qcm_client*)connection_handle;

  if(!sns_island_is_island_ptr((intptr_t)client))
    SNS_ISLAND_EXIT();

  SNS_PRINTF(MED, &sensor->sensor, "service_resume %x", client);

  sns_osa_lock_acquire(&client->streams_lock);
  client->busy = false;

  for(sns_isafe_list_iter_init(&iter, &client->streams, true),
      stream = (sns_qcm_stream*)sns_isafe_list_iter_get_curr_data(&iter);
      NULL != stream && !client->busy;
      stream = (sns_qcm_stream*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
  {
    if(send_stream_data(state, stream))
      break;
  }
  sns_osa_lock_release(&client->streams_lock);
}

/**
 * Publish all Sensor attributes
 *
 * @param[i] this reference to the QCM Sensor
 */
static void
publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = SNS_QCM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = SNS_QCM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = SNS_QCM_VENDOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

/* See sns_sensor::init */
static sns_rc
init(sns_sensor *const this)
{
  sns_qcm_state *state = (sns_qcm_state*)this->state->state;
  sns_osa_lock_attr lattr;
  sns_rc rc;

  state->sensor = (sns_sensor*)this;

  sns_service_manager *manager = this->cb->get_service_manager(this);
  state->stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

  sns_isafe_list_init(&state->processors);
  sns_isafe_list_init(&state->clients);

  rc = sns_osa_lock_attr_init(&lattr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
#ifdef SNS_ISLAND_INCLUDE_QCM
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_ISLAND);
#else
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_NORMAL);
#endif
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_init(&lattr, &state->processors_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_init(&lattr, &state->clients_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  publish_attributes(this);
  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "remote_proc_state");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "timer");

  sns_qcm_service_init(&state->service, this, &service_connect,
      &service_disconnect, &service_request, &service_resume, this);

  return SNS_RC_SUCCESS;
}

/**
 * Upon receipt of an event from SEE, re-encode and send the event.
 *
 * @return True if a connection is closing; false otherwise
 */
SNS_SECTION(".text.sns") static bool
handle_event(sns_qcm_state *state, sns_qcm_client *client,
    sns_qcm_stream *stream, sns_sensor_event *event)
{
  bool rv = false;

  if(client->busy || 0 != stream->batch_period ||
     (!stream->wakeup && stream->processor->is_suspended))
  {
    sns_qcm_fifo_insert(stream->fifo, event);
  }
  else
  {
    sns_rc rc;
    sns_client_event_msg msg = sns_client_event_msg_init_default;

    UNUSED_VAR(stream);
    sns_memscpy(&msg.suid, sizeof(msg.suid),
                &stream->suid, sizeof(stream->suid));
    msg.events.funcs.encode = &encode_client_event;
    msg.events.arg = event;

    rc = sns_qcm_service_send(client->conn, &msg);
    if(SNS_RC_SUCCESS != rc)
      SNS_PRINTF(MED, state->sensor, "Error sending event %i", rc);

    if(SNS_RC_NOT_AVAILABLE == rc || SNS_RC_FAILED == rc)
    {
      client->busy = true;
      sns_qcm_fifo_insert(stream->fifo, event);

      if(SNS_RC_FAILED == rc)
        send_timer_request(state);
    }
    else if(SNS_RC_INVALID_STATE == rc)
    {
      rv = true;
    }
  }

  return rv;
}

/**
 * Check all data streams for pending events.  Forward to client as
 * appropriate.
 *
 * PEND: The following may ultimately be found to be too inefficient.  A
 * natural solution may be to subdivide data streams across Instances (whose
 * sole purpose will be to cut-down on this searching time).
 */
SNS_SECTION(".text.sns") static void
handle_client_events(sns_qcm_state *state)
{
  sns_isafe_list_iter client_iter;
  sns_qcm_client *client;

  sns_osa_lock_acquire(&state->clients_lock);
  for(sns_isafe_list_iter_init(&client_iter, &state->clients, true),
      client = (sns_qcm_client*)sns_isafe_list_iter_get_curr_data(&client_iter);
      NULL != client;
      client = (sns_qcm_client*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&client_iter)))
  {
    sns_isafe_list_iter stream_iter;
    sns_qcm_stream *stream;
    bool closing = false;

    sns_osa_lock_acquire(&client->streams_lock);
    for(sns_isafe_list_iter_init(&stream_iter, &client->streams, true),
        stream = (sns_qcm_stream*)sns_isafe_list_iter_get_curr_data(&stream_iter);
        NULL != stream;
        stream = (sns_qcm_stream*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&stream_iter)))
    {
      sns_data_stream *data_stream = stream->data_stream;

      if(NULL != data_stream)
      {
        for(sns_sensor_event *event = data_stream->api->peek_input(data_stream);
            NULL != event;
            event = data_stream->api->get_next_input(data_stream))
        {
          closing = handle_event(state, client, stream, event);
        }

        if(sns_qcm_proc_type == stream->processor->proc_type &&
           0 != stream->batch_period && !closing && !client->busy)
          closing = send_stream_data(state, stream);

        if(closing)
        {
          state->stream_service->api->remove_stream(
            state->stream_service, stream->data_stream);
          stream->data_stream = NULL;
        }
      }
    }
    sns_osa_lock_release(&client->streams_lock);
  }
  sns_osa_lock_release(&state->clients_lock);
}

/**
 * Send an enable request to the remote processor state Sensor.  This data
 * stream will be persistent across the life of QCM.
 *
 * @param[i] data_stream Already created data stream to remote proc state
 */
static void
enable_remote_proc(sns_qcm_state *state, sns_data_stream *data_stream)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint64_t buffer[80];
  size_t encoded_len;
  sns_remote_proc_state_config remote_proc_state_config;

  remote_proc_state_config.proc_type = SNS_STD_CLIENT_PROCESSOR_APSS;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &remote_proc_state_config, sns_remote_proc_state_config_fields, NULL);

  if(0 < encoded_len)
  {
    sns_request request = (sns_request)
        { .message_id = SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_CONFIG,
          .request_len = encoded_len, .request = buffer };
    rc = data_stream->api->send_request(data_stream, &request);
  }

  if(SNS_RC_SUCCESS != rc || 0 >= encoded_len)
    SNS_PRINTF(ERROR, state->sensor, "Failed to encode/send request");
}

/**
 * Handle any unprocessed events from the remote processor state Sensor.
 *
 * If the AP exits suspend, we will take this opportunity to verify all client
 * connections are still present.
 */
static void
handle_remote_proc_events(sns_qcm_state *state)
{
  for(sns_sensor_event *event = state->rps_stream->api->peek_input(state->rps_stream);
      NULL != event;
      event = state->rps_stream->api->get_next_input(state->rps_stream))
  {
    if(SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_EVENT == event->message_id)
    {
      bool is_suspended;
      sns_qcm_proc *proc;
      pb_istream_t stream =
        pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

      sns_remote_proc_state_event rps_event =
          sns_remote_proc_state_event_init_default;

      if(!pb_decode(&stream, sns_remote_proc_state_event_fields, &rps_event))
      {
        SNS_PRINTF(ERROR, state->sensor, "Error decoding RPS");
      }
      else if(SNS_STD_CLIENT_PROCESSOR_APSS == rps_event.proc_type &&
              SNS_REMOTE_PROC_STATE_AWAKE == rps_event.event_type)
      {
        sns_qcm_service_check(state->service);
      }

      proc = find_processor(state, rps_event.proc_type);
      is_suspended = proc->is_suspended;
      proc->is_suspended = SNS_REMOTE_PROC_STATE_AWAKE != rps_event.event_type;

      if(is_suspended && !proc->is_suspended)
        initiate_flush(proc);
    }
  }
}

/**
 * Handle any unprocessed events from the Timer Sensor.
 *
 * These events indicate that the Service was recently OOM, and we should
 * attempt to resend any pending events.
 */
static void
handle_timer_events(sns_qcm_state *state)
{
  bool activate = false;

  for(sns_sensor_event *event = state->timer_stream->api->peek_input(state->timer_stream);
      NULL != event;
      event = state->timer_stream->api->get_next_input(state->timer_stream))
    if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
      activate = true;

  if(activate)
  {
    sns_isafe_list_iter client_iter;
    sns_qcm_client *client;

    atomic_store(&state->oom_active, 0);

    sns_osa_lock_acquire(&state->clients_lock);
    for(sns_isafe_list_iter_init(&client_iter, &state->clients, true),
        client = (sns_qcm_client*)sns_isafe_list_iter_get_curr_data(&client_iter);
        NULL != client;
        client = (sns_qcm_client*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&client_iter)))
    {
      service_resume(client, state->sensor);
    }
    sns_osa_lock_release(&state->clients_lock);
  }
}

/* See sns_sensor::notify_event */
SNS_SECTION(".text.sns") static sns_rc
notify_event(sns_sensor *const this)
{
  sns_qcm_state *state = (sns_qcm_state*)this->state->state;

  if(NULL == state->rps_stream)
  {
    sns_suid_lookup_handle(this, &state->suid_lookup_data);

    if(sns_suid_lookup_complete(&state->suid_lookup_data))
    {
      sns_sensor_uid suid;

      SNS_ISLAND_EXIT();
      sns_suid_lookup_deinit(this, &state->suid_lookup_data);

      sns_suid_lookup_get(&state->suid_lookup_data, "remote_proc_state", &suid);
      state->stream_service->api->create_sensor_stream(state->stream_service,
          this, suid, &state->rps_stream);

      sns_suid_lookup_get(&state->suid_lookup_data, "timer", &suid);
      state->stream_service->api->create_sensor_stream(state->stream_service,
        this, suid, &state->timer_stream);
      SNS_ASSERT(NULL != state->timer_stream);

      if(NULL != state->rps_stream)
        enable_remote_proc(state, state->rps_stream);
    }
  }
  else if(NULL != state->rps_stream->api->peek_input(state->rps_stream))
  {
    SNS_ISLAND_EXIT();
    handle_remote_proc_events(state);
  }
  else if(NULL != state->timer_stream->api->peek_input(state->timer_stream))
  {
    SNS_ISLAND_EXIT();
    handle_timer_events(state);
  }

  handle_client_events(state);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
SNS_SECTION(".text.sns") static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &qcm_sensor_suid;
}

/*===========================================================================
  Sensor API Data Definitions
  ===========================================================================*/

static const sns_sensor_api sns_qcm_sensor_api SNS_SECTION(".rodata.sns") =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &init,
  .deinit = NULL,
  .get_sensor_uid = &get_sensor_uid,
  .set_client_request = NULL,
  .notify_event = &notify_event,
};

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc
sns_register_qcm(sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_qcm_state), &sns_qcm_sensor_api, NULL);

  return SNS_RC_SUCCESS;
}
