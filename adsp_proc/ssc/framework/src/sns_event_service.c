/*=============================================================================
  @file sns_event_service.c

  Provides Sensor Instances the ability to publish events, which will be
  temporarily stored by this service, and ultimately delivered to all
  applicable clients.

  PEND: Use circular buffer in non-Island mode, and transition gracefully
  between the two.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdlib.h>
#include "pb_encode.h"
#include "sns_atomic.h"
#include "sns_assert.h"
#include "sns_diag.pb.h"
#include "sns_diag_print.h"
#include "sns_fw.pb.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_log.h"
#include "sns_fw_request.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_event.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_stream_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_thread_manager.h"
#include "sns_time.h"
#include "sns_types.h"

/*============================================================================
  Macro Definitions
  ===========================================================================*/

#define SNS_EVENT_SERVICE_FREE_PATTERN 0xD0
#define SNS_EVENT_SERVICE_DEAD_PATTERN 0xA5

// Maximum number of events that can be pending on a stream after which
// event will be dropped
#define SNS_DATA_STREAM_MAX_PENDING_EVENTS 1000

// Number of most recent events to retain when the event buffers are low on memory.
#define SNS_DATA_STREAM_NUMBER_OF_EVENTS_TO_RETAIN 10

// Enable detailed messages
// #define SNS_EVENT_SERVICE_VERBOSE_LOG
/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

static sns_event_service_api event_service_api SNS_SECTION(".data.sns");
static sns_fw_event_service event_service SNS_SECTION(".data.sns");

static const uint16_t version SNS_SECTION(".rodata.sns") = 1;
static const uint32_t max_buffer_size SNS_SECTION(".rodata.sns") = 512;

uint64_t sns_event_service_island_buffer[SNS_FW_EVENT_SERVICE_BUFFER_ISLAND  / sizeof(uint64_t)]
  SNS_SECTION(".data.sns");

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_event_service_api::get_max_event_size */
SNS_SECTION(".text.sns") static uint32_t
get_max_event_size(sns_event_service const *this)
{
  UNUSED_VAR(this);
  return max_buffer_size;
}

/* Disables island mode */
SNS_SECTION(".text.sns") static void
disable_island(sns_fw_event_service *this)
{
  if(!this->disable_island)
  {
    this->disable_island = true;
    sns_island_block(this->island_client);
    //SNS_PRINTF(HIGH, sns_fw_printf, "Disabled Island Mode");
  }
}

/* Attempt to reenable island mode */
SNS_SECTION(".text.sns") static void
attempt_reenable_island(sns_fw_event_service *this)
{
  if(this->disable_island &&
     0 == sns_memmgr_get_heap_usage(SNS_HEAP_EVENT) &&
     SNS_FW_EVENT_SERVICE_BUFFER_ISLAND / 2 >= this->bytes_allocated)
  {
    this->disable_island = false;
    //SNS_PRINTF(HIGH, sns_fw_printf, "Enabled Island Mode");
    sns_island_unblock(this->island_client);
  }
}

/**
 * Drop all events from the tail of all streams, so as to make available in the
 * global circular buffer.
 *
 * @param[i] event_service The Event Service
 * @param[i] streams Data Streams of a Sensor or Sensor Instance
 * @param[i] drop_all Drop all events, regardless of their position
 */
static void
drop_tail_from_stream(sns_fw_event_service *this, sns_isafe_list_iter *streams,
    bool drop_all)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_DROP_TAIL_START, 0);
  for(; NULL != sns_isafe_list_iter_curr(streams);
      sns_isafe_list_iter_advance(streams))
  {
    uint32_t drop_cnt = 0;
    sns_fw_data_stream *stream = (sns_fw_data_stream*)
      sns_isafe_list_iter_get_curr_data(streams);
    sns_fw_sensor_event *event = sns_data_stream_peek_input(stream);

    while(NULL != event &&
          stream->event_cnt > SNS_DATA_STREAM_NUMBER_OF_EVENTS_TO_RETAIN)
    {
      if(!drop_all)
      {
        uintptr_t ptr = (uintptr_t)event;
        uintptr_t drop_until;

        sns_osa_lock_acquire(this->buffer_lock);
        drop_until = this->buffer_head + SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_DROP_TILL_THRESH;
        if(drop_until >= this->active_buffer_end)
          drop_until = this->active_buffer_start + (drop_until - this->active_buffer_end);

        // PEND: We only want to drop the oldest samples, not all samples
        if( 0 == sns_memmgr_get_heap_usage(SNS_HEAP_EVENT) &&
           ((this->buffer_head > this->buffer_tail && ptr < this->buffer_head && ptr > drop_until) ||
           (this->buffer_head < this->buffer_tail && ptr < this->buffer_head && ptr > drop_until) ||
           (this->buffer_head < this->buffer_tail && ptr > this->buffer_head && ptr > drop_until && this->buffer_head < drop_until) ||
           (this->buffer_head < this->buffer_tail && ptr < this->buffer_head && this->buffer_head < drop_until)))
        {
          sns_osa_lock_release(this->buffer_lock);
          break;
        }

        sns_osa_lock_release(this->buffer_lock);
      }
      drop_cnt++;
      event = sns_data_stream_get_next_input(stream);
    }
    SNS_PRINTF(ERROR, sns_fw_printf, "Dropped %i events from " SNS_DIAG_PTR ", %i events remain",
        drop_cnt, stream, sns_data_stream_get_input_cnt(stream));
  }
  SNS_PROFILE(SNS_EVENT_SERVICE_DROP_TAIL_END, 0);
}

/**
 * Drop all samples on an incoming data stream up to the specified end-point.
 *
 * No-op unless sns_fw_event_service::low_mem_active is true.
 *
 * @param[i] sensor Sensor from which we should drop all data
 *
 * @return Always true
 */
SNS_SECTION(".text.sns") static bool
sns_event_service_drop_tail(sns_fw_sensor *sensor, void *unused)
{
  UNUSED_VAR(unused);
  sns_fw_event_service *this = &event_service;

  sns_isafe_list_iter streams;
  sns_sensor_instance *temp;

  sns_isafe_list_iter_init(&streams, &sensor->data_streams, true);
  // PEND: Limit # of events processed per notify_event;
  // Then update third argument to false here and below
  drop_tail_from_stream(this, &streams, true);

  for(temp = sensor->sensor.cb->get_sensor_instance(&sensor->sensor, true);
      NULL != temp;
      temp = sensor->sensor.cb->get_sensor_instance(&sensor->sensor, false))
  {
    sns_fw_sensor_instance *instance = (sns_fw_sensor_instance*)temp;
    sns_isafe_list_iter_init(&streams, &instance->data_streams, true);
    drop_tail_from_stream(this, &streams, true);
  }
  return true;
}

/**
 * Upon notification of low memory, drop the oldest unprocessed events from
 * all data streams.
 */
static void
handle_low_mem(intptr_t arg)
{
  UNUSED_VAR(arg);
  SNS_PRINTF(ERROR, sns_fw_printf, "Main buffer low memory, dropping data");
  sns_thread_manager_disable(true);
  atomic_store(&event_service.low_mem_active, 1);
  sns_sensor_foreach(&sns_event_service_drop_tail, NULL);
  atomic_store(&event_service.low_mem_active, 0);
  sns_thread_manager_disable(false);
  SNS_PRINTF(ERROR, sns_fw_printf, "low_mem_thread_func complete");
}

/**
 * Allocate an event from the global buffer.
 *
 * See sns_event_service_api::alloc_event
 */
SNS_SECTION(".text.sns") static sns_fw_sensor_event*
alloc_event(sns_fw_event_service *this, uint32_t event_len)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_ALLOC_EVENT_START, 0);
  sns_fw_sensor_event *fw_event;
  size_t struct_len = sizeof(*fw_event) - sizeof(fw_event->event.event) + event_len;
  bool heap_alloc = false;

  sns_osa_lock_acquire(this->buffer_lock);

  // Align to 8-byte boundary
  struct_len = ALIGN_8(struct_len);

  heap_alloc =
    (this->buffer_head + struct_len >= this->active_buffer_end &&
      this->active_buffer_start + struct_len >= this->buffer_tail) ||
     (this->buffer_head < this->buffer_tail &&
      this->buffer_head + struct_len >= this->buffer_tail);
  if(heap_alloc)
  {
    disable_island(this);
    sns_osa_lock_release(this->buffer_lock);
    fw_event = sns_malloc(SNS_HEAP_EVENT, struct_len);
  }
  else
  {
    // If we have gotten to the end of the buffer, roll-over
    if(this->buffer_head + struct_len >= this->active_buffer_end)
    {
      if(this->buffer_head == this->buffer_tail)
      {
        this->buffer_tail = this->active_buffer_start;
      }
      else
      {
        sns_memset((void*)this->buffer_head, SNS_EVENT_SERVICE_DEAD_PATTERN,
                   (this->active_buffer_end - this->buffer_head));
        this->rollover_here = this->buffer_head;
      }
      this->buffer_head = this->active_buffer_start;
    }

    fw_event = (sns_fw_sensor_event*)this->buffer_head;
    this->num_alloc++;
    this->max_alloc = SNS_MAX(this->max_alloc, this->num_alloc);

    this->bytes_allocated += struct_len;
    this->max_bytes_allocated = SNS_MAX(this->max_bytes_allocated, this->bytes_allocated);
    this->buffer_head += struct_len;
  }
  SNS_ASSERT(NULL != fw_event);
  fw_event->event.event_len = event_len;
  fw_event->header.client_cnt = INT32_MAX;
  fw_event->header.fw_event_len = struct_len;

#ifdef SNS_DEBUG_EVENT_SERVICE
  fw_event->header.headcode = 0xABCDABCD;
  fw_event->header.caller = ((intptr_t)__builtin_return_address(2));
#endif

  if(!heap_alloc)
    sns_osa_lock_release(this->buffer_lock);

  SNS_PROFILE(SNS_EVENT_SERVICE_ALLOC_EVENT_END, 0);
  return fw_event;
}

/* See sns_event_service_api::alloc_event */
SNS_SECTION(".text.sns") static sns_sensor_event*
alloc_event_for_instance(sns_event_service *event_service, sns_sensor_instance *instance,
            uint32_t event_len)
{
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_fw_sensor_event *fw_event;
  sns_fw_event_service *this = (sns_fw_event_service*)event_service;

  if(NULL != fw_instance->unpublished_event)
  {
    sns_osa_lock_acquire(this->buffer_lock);
    fw_instance->unpublished_event->header.client_cnt = 1;
    sns_osa_lock_release(this->buffer_lock);

    sns_event_service_consume(fw_instance->unpublished_event);
  }
  fw_event = alloc_event((sns_fw_event_service*)event_service, event_len);
  fw_instance->unpublished_event = fw_event;

  return &fw_event->event;
}

/**
 * Add new event to a stream's event array.  This will allow this
 * specific client to find and ultimately process the event.
 *
 * @note The stream->events_lock must be held while caling this function.
 */
SNS_SECTION(".text.sns") static void
add_event_to_stream(sns_fw_data_stream *stream, sns_fw_sensor_event *fw_event)
{
  sns_isafe_list_iter iter;
  sns_events_bin *events_bin_header = NULL;

  sns_isafe_list_iter_init(&iter, &stream->event_bins, false);

  SNS_ASSERT(stream->write_index <= SNS_DATA_STREAM_EVENT_CNT);
  if(SNS_DATA_STREAM_EVENT_CNT == stream->write_index ||
     NULL == sns_isafe_list_iter_curr(&iter))
  {
    if(SNS_ISLAND_STATE_NOT_IN_ISLAND != stream->island_operation)
    {
      events_bin_header = sns_malloc(SNS_HEAP_ISLAND, sizeof(*events_bin_header));
      if(NULL == events_bin_header)
      {
        SNS_ISLAND_EXIT();
        stream->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;
      }
    }

    if(NULL == events_bin_header)
      events_bin_header = sns_malloc(SNS_HEAP_MAIN, sizeof(*events_bin_header));
    SNS_ASSERT(NULL != events_bin_header);

    sns_isafe_list_item_init(&events_bin_header->list_entry, events_bin_header);
    sns_isafe_list_iter_insert(&iter, &events_bin_header->list_entry, true);
    stream->write_index = 0;
    sns_isafe_list_iter_init(&iter, &stream->event_bins, false);
  }

  events_bin_header = (sns_events_bin*)
    sns_isafe_list_iter_get_curr_data(&iter);

  events_bin_header->events[stream->write_index] = fw_event;
  ++stream->write_index;
}

/**
 * Publish an event to a particular data stream.
 *
 * @param[i] this Event Service reference
 * @param[i] stream Stream on which to publish the event
 * @param[i] fw_event Fully initialized event to publish
 *
 * @return
 *  SNS_RC_NOT_AVAILABLE - Stream is unavailable for further events
 *  SNS_RC_SUCCESS
 */
SNS_SECTION(".text.sns") static sns_rc
publish_client_event(sns_fw_event_service *this, sns_fw_data_stream *stream,
    sns_fw_sensor_event *fw_event)
{
  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  UNUSED_VAR(this);

  if(sns_data_stream_validate(stream))
  {
    sns_osa_lock_acquire(stream->events_lock);
    if(SNS_DATA_STREAM_AVAILABLE == stream->removing ||
       SNS_FW_MSGID_SNS_DESTROY_COMPLETE_EVENT == fw_event->event.message_id)
    {
      if(SNS_DATA_STREAM_MAX_PENDING_EVENTS > stream->event_cnt ||
         SNS_FW_MSGID_SNS_DESTROY_COMPLETE_EVENT == fw_event->event.message_id)
      {
        ++stream->event_cnt;
        add_event_to_stream(stream, fw_event);
        rv = SNS_RC_SUCCESS;
      }
      sns_event_service_add_pending_event(stream);
    }
    sns_osa_lock_release(stream->events_lock);
  }

  return rv;
}

/**
 * Publish an event to all applicable client data streams.
 *
 * @param[i] this Event Service
 * @param[i] instance Sensor Instance which produced the event
 * @param[i] sensor_uid To which clients to publish the event
 * @param[i] fw_sensor Sensor associated with the Instance
 * @param[i] event Event to be published
 *
 * @return The number of clients who received this event
 */
SNS_SECTION(".text.sns") static int32_t
publish_event_for_clients(sns_fw_event_service *this,
    sns_fw_sensor_instance *instance, sns_sensor_uid const *sensor_uid,
    sns_fw_sensor *fw_sensor, sns_fw_sensor_event *event)
{
  sns_isafe_list_iter req_list_iter;
  sns_isafe_list_item *curr_req_list = NULL;
  int32_t published_cnt = 0;
  bool publish_to_all = (NULL == sensor_uid);
  sensor_uid= (NULL == sensor_uid) ?
    fw_sensor->sensor.sensor_api->get_sensor_uid(&(fw_sensor->sensor)) : sensor_uid;

  for(curr_req_list = sns_isafe_list_iter_init(&req_list_iter, &instance->client_req_lists, true);
      NULL != curr_req_list;
      curr_req_list = sns_isafe_list_iter_advance(&req_list_iter))
  {
    sns_client_req_list *req_list = (sns_client_req_list*)
      sns_isafe_list_item_get_data(curr_req_list);

    if(publish_to_all || sns_sensor_uid_compare(req_list->suid, sensor_uid))
    {
      sns_isafe_list_iter client_iter;
      sns_isafe_list_item *curr_client_req = NULL;

      for(curr_client_req = sns_isafe_list_iter_init(&client_iter, &req_list->client_requests, true);
          NULL != curr_client_req;
          curr_client_req = sns_isafe_list_iter_advance(&client_iter))
      {
        sns_rc rc;
        sns_fw_request *req = (sns_fw_request*)
          sns_isafe_list_item_get_data(curr_client_req);

        if(!sns_island_is_island_ptr((intptr_t)req->stream) ||
           SNS_ISLAND_STATE_IN_ISLAND != req->stream->island_operation)
        {
          SNS_ISLAND_EXIT();
        }

        SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_TO_CLIENT_START, 2, req->stream,
            (NULL == req->stream->dst_sensor) ?
            (void*)req->stream->dst_instance : (void*)req->stream->dst_sensor);

        rc = publish_client_event(this, req->stream, event);
        if(SNS_RC_SUCCESS == rc)
        {
          published_cnt += 1;
#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
          SNS_PRINTF(LOW, sns_fw_printf, "Event by Instance " SNS_DIAG_PTR
            " for Stream " SNS_DIAG_PTR " (%i)",
            instance, req->stream, event->event.event_len);
#endif
        }
        SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_TO_CLIENT_END, 0);
      }
      if(!publish_to_all)
      {
        break;
      }
    }
  }
  return published_cnt;
}

/* See sns_event_service_api::publish_event_suid */
SNS_SECTION(".text.sns") static sns_rc
publish_event(sns_event_service *event_service, sns_sensor_instance *instance,
              sns_sensor_event *event, sns_sensor_uid const *sensor_uid)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_START, 0);
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_fw_sensor_event *fw_event = sns_event_service_get_fw_event(event);

  if(fw_instance->unpublished_event != fw_event)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Attempt to re-publish event");
  }
  else
  {
    sns_fw_event_service *this = (sns_fw_event_service*)event_service;
    sns_fw_sensor *fw_sensor = (sns_fw_sensor*)fw_instance->sensor;
    int32_t client_cnt, published_cnt;

    sns_pwr_sleep_mgr_check_config_event(instance, event,
        sns_attr_info_get_is_physical_sensor(fw_sensor->attr_info));

    client_cnt = sns_instance_client_cnt(fw_instance, sensor_uid);

    // fw_event is already on the buffer -> we must acquire the lock to modify it
    sns_osa_lock_acquire(this->buffer_lock);
    // If client_cnt is zero, just consume and don't publish
    fw_event->header.client_cnt = (0 == client_cnt) ? 1 : client_cnt;

    if(0 == client_cnt)
    {
      sns_event_service_consume(fw_event);
      sns_osa_lock_release(this->buffer_lock);
      SNS_PRINTF(HIGH, sns_fw_printf, "Consumed event 0x" SNS_DIAG_PTR
          " because instance 0x" SNS_DIAG_PTR " has no clients",
          fw_event, instance);
    }
    else
    {
      sns_osa_lock_release(this->buffer_lock);
      published_cnt = publish_event_for_clients(this, fw_instance, sensor_uid, fw_sensor, fw_event);

      // If the number of clients receiving the new event
      // is less than the client_cnt, then reduce the
      // fw_event->header.client_cnt appropriately.
      while(client_cnt > published_cnt)
      {
        sns_event_service_consume(fw_event);
        published_cnt += 1;
      }
    }
    sns_fw_log_sensor_inst_api_event(instance, sensor_uid, event);
    fw_instance->unpublished_event = NULL;
  }

  SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_END, 0);
  return SNS_RC_SUCCESS;
}

/* See sns_event_service_api::publish_error */
SNS_SECTION(".text.sns") static sns_rc
publish_error(sns_event_service *this, sns_sensor_instance *instance,
    sns_rc reason)
{
  if(0 != sns_instance_client_cnt((sns_fw_sensor_instance*)instance, NULL))
  {
    pb_ostream_t stream;
    sns_sensor_event *event;
    sns_std_error_event error_event = sns_std_error_event_init_default;

    SNS_PRINTF(MED, sns_fw_printf, "Publishing error %i for instance " SNS_DIAG_PTR,
        reason, instance);

    error_event.error = (sns_std_error)reason;
    event = alloc_event_for_instance(this, instance, sns_std_error_event_size);
    event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
    event->timestamp = sns_get_system_time();

    stream = pb_ostream_from_buffer((pb_byte_t*)event->event, event->event_len);
    if(!pb_encode(&stream, sns_std_error_event_fields, &error_event))
      SNS_ASSERT(false);

    publish_event(this, instance, event, NULL);
  }

  return SNS_RC_SUCCESS;
}

/**
 * Process a pending event for a Sensor that was just taken from the queue.
 *
 * @param[i] this Reference to the Event Service
 * @param[i] stream Data Stream with event(s) to process
 */
SNS_SECTION(".text.sns") static void
process_pending_event_sensor(sns_fw_event_service *this,
    sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_SENSOR, 0);
  sns_rc rc;
  sns_fw_sensor *sensor = stream->dst_sensor;

#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
  SNS_PRINTF(LOW, sns_fw_printf,
      "Notifying Sensor "SNS_DIAG_PTR" of event on "SNS_DIAG_PTR,
      sensor, stream);
#endif

  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_START, 1, sensor);
  if(!sns_island_is_island_ptr((intptr_t)sensor->sensor.sensor_api->notify_event) ||
     (NULL != sensor->sensor.instance_api &&
      !sns_island_is_island_ptr((intptr_t)sensor->sensor.instance_api->set_client_config)))
  {
    SNS_ISLAND_EXIT();
  }
  rc = sensor->sensor.sensor_api->notify_event((sns_sensor*)sensor);
  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_END, 0);

  if(SNS_RC_NOT_AVAILABLE == rc)
  {
    sns_isafe_list_iter streams;
    sns_isafe_list_iter_init(&streams, &sensor->data_streams, true);
    SNS_ISLAND_EXIT();
    drop_tail_from_stream(this, &streams, true);
  }
  else if(SNS_RC_INVALID_LIBRARY_STATE == rc)
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(MED, sns_fw_printf, "deinit_lib "
               SNS_DIAG_PTR " ", sensor->library);
    sns_sensor_library_deinit(sensor->library);
  }
  else if(SNS_RC_SUCCESS != rc)
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(MED, sns_fw_printf,
        "Recieved error %i from sensor " SNS_DIAG_PTR, rc, sensor);
    sns_sensor_deinit(sensor);
  }
}

/**
 * Process a pending event for a Sensor Instance that was just taken from the
 * queue.
 *
 * @param[i] this Reference to the Event Service
 * @param[i] stream Data Stream with event(s) to process
 */
SNS_SECTION(".text.sns") static void
process_pending_event_instance(sns_fw_event_service *this,
    sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_SENSOR_INSTANCE, 0);
  sns_rc rc;
  sns_fw_sensor_instance *instance = stream->dst_instance;

#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
  // PEND: This is goofy, we need another macro to control which to print
  SNS_PRINTF(LOW, sns_fw_printf,
      "Notifying Instance "SNS_DIAG_PTR" of event on "SNS_DIAG_PTR,
      instance, stream);
#endif

  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_START, 1, instance);
  rc = instance->sensor->sensor.instance_api->notify_event(
      (sns_sensor_instance*)instance);
  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_END, 0);

  if(SNS_RC_NOT_AVAILABLE == rc)
  {
    SNS_ISLAND_EXIT();
    sns_isafe_list_iter streams;
    sns_isafe_list_iter_init(&streams, &instance->data_streams, true);
    drop_tail_from_stream(this, &streams, true);
    sns_event_service_publish_error(instance, SNS_RC_NOT_AVAILABLE);
  }
  else if(SNS_RC_INVALID_STATE == rc)
  {
    SNS_ISLAND_EXIT();
    SNS_PRINTF(ERROR, sns_fw_printf,
        "Error %i from instance " SNS_DIAG_PTR, rc, instance);

    sns_sensor_instance_deinit(&instance->instance);
  }
  else if(SNS_RC_SUCCESS != rc)
  {
    sns_fw_sensor *sensor = instance->sensor;

    SNS_PRINTF(ERROR, sns_fw_printf,
        "Instance " SNS_DIAG_PTR " invalid return (%i)", instance, rc);
    sns_sensor_deinit(sensor);
  }
}

/* See sns_diag_encode_log_cb */
SNS_SECTION(".text.sns") static sns_rc
sns_event_service_log_buffer_usage_encode_cb(void *log, size_t log_size,
    size_t encoded_log_size, void *encoded_log, size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == log || 0 == log_size || NULL == encoded_log ||
     0 == encoded_log_size || NULL == bytes_written)
    return SNS_RC_FAILED;

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_mem_utilization_log_fields, log))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf,
        "Error encoding log: %s", PB_GET_ERROR(&stream));
    rc = SNS_RC_FAILED;
  }

  if(SNS_RC_SUCCESS == rc)
    *bytes_written = stream.bytes_written;

  return rc;
}

/**
 * Process a single event stream.
 *
 * See sns_thread_mgr_task_func().
 */
SNS_SECTION(".text.sns") static sns_rc
sns_event_service_process(void *func_arg)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_EVENTS_START, 0);
  sns_fw_event_service *this = (sns_fw_event_service*)&event_service;
  sns_fw_data_stream *stream = func_arg;

  if(!sns_island_is_island_ptr((intptr_t)stream) ||
     SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation ||
     ((NULL != stream->dst_sensor &&
        SNS_ISLAND_STATE_IN_ISLAND != stream->dst_sensor->island_operation) ||
      (NULL != stream->dst_instance &&
        SNS_ISLAND_STATE_IN_ISLAND != stream->dst_instance->island_operation)))
    SNS_ISLAND_EXIT();

  if(sns_data_stream_validate(stream))
  {
    int removing;

    sns_osa_lock_acquire(stream->events_lock);
    removing = stream->removing;
    sns_osa_lock_release(stream->events_lock);

    atomic_store(&stream->unprocessed_events, 0);
    if(SNS_DATA_STREAM_AVAILABLE != removing)
    {
      SNS_PROFILE(SNS_EVENT_SERVICE_REMOVE_STREAM_START, 0);
      sns_fw_sensor_event *event;

      for(event = sns_data_stream_peek_input(stream);
          NULL != event;
          event = sns_data_stream_get_next_input(stream))
      {
        if(SNS_FW_MSGID_SNS_DESTROY_COMPLETE_EVENT == event->event.message_id)
        {
          sns_data_stream_destroy_complete(stream);
          break;
        }
      }
      SNS_PROFILE(SNS_EVENT_SERVICE_REMOVE_STREAM_END, 0);
    }
    else
    {
      if(NULL != stream->dst_sensor)
        process_pending_event_sensor(this, stream);
      else
        process_pending_event_instance(this, stream);
    }
  }

  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_EVENTS_END, 0);
  return SNS_RC_SUCCESS;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_fw_event_service*
sns_event_service_init(void)
{
  sns_osa_lock_attr attr;
  sns_rc rc;
  event_service_api = (sns_event_service_api)
  {
    .struct_len = sizeof(event_service_api),
    .get_max_event_size = &get_max_event_size,
    .alloc_event = &alloc_event_for_instance,
    .publish_event = &publish_event,
    .publish_error = &publish_error
  };

  event_service.service.service.version = version;
  event_service.service.service.type = SNS_EVENT_SERVICE;
  event_service.service.api = &event_service_api;
  event_service.island_buffer = sns_event_service_island_buffer;

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &event_service.buffer_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_mem_heap_register_lowmem_cb(SNS_HEAP_EVENT, SNS_FW_EVENT_SERVICE_BUFFER_LOW_MEM_CB_THRESH,
    handle_low_mem, (intptr_t)&event_service);

  event_service.buffer_head = (uintptr_t)event_service.island_buffer;
  event_service.buffer_tail = (uintptr_t)event_service.island_buffer;

  event_service.active_buffer_start = (uintptr_t)event_service.island_buffer;
  event_service.active_buffer_end =
    (uintptr_t)&event_service.island_buffer[SNS_FW_EVENT_SERVICE_BUFFER_ISLAND / sizeof(uint64_t)];
  event_service.rollover_here = event_service.active_buffer_end;

  sns_memset(sns_event_service_island_buffer, SNS_EVENT_SERVICE_FREE_PATTERN,
             sizeof(sns_event_service_island_buffer));

  atomic_init(&event_service.low_mem_active, 0);
  event_service.disable_island = false;
  event_service.island_client = sns_island_aggregator_register_client("event service");

  return &event_service;
}

SNS_SECTION(".text.sns") sns_rc
sns_event_service_publish_error(struct sns_fw_sensor_instance *instance,
    sns_rc reason)
{
  return publish_error((sns_event_service*)&event_service,
      (sns_sensor_instance*)instance, reason);
}

SNS_SECTION(".text.sns") void
sns_event_service_consume(sns_fw_sensor_event *fw_event)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_CONSUME_EVENT_START, 0);
  sns_fw_event_service *this = &event_service;
  sns_osa_lock_acquire(this->buffer_lock);

  --fw_event->header.client_cnt;
  //Alloc event sets the client_cnt to INT32_MAX
  //If there are clients and the event is published to all clients, the cnt will be 0.
  //An allocated event not published , but consumed the cnt will be set to 1 then decrement to 0
  //client_cnt should NEVER be negative number
#ifdef SNS_DEBUG_EVENT_SERVICE
  SNS_ASSERT(0xABCDABCD == fw_event->header.headcode);
#endif

  // PEND: Debug why this assertion doesn't hold true
  //SNS_ASSERT(0 <= fw_event->header.client_cnt);
  if(0 > fw_event->header.client_cnt)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Event 0x"SNS_DIAG_PTR" has negative client_cnt: %d",
      fw_event, fw_event->header.client_cnt);
  }

  if(this->active_buffer_start <= (uintptr_t)fw_event &&
     (uintptr_t)fw_event <= this->active_buffer_end )
  {
    // PEND: 0 == fw_event->header.client_cnt - needs assert above to be enabled
    while(0 >= fw_event->header.client_cnt &&
          (uintptr_t)fw_event == (uintptr_t)this->buffer_tail &&
          this->buffer_tail != this->buffer_head)
    {
      int32_t total_len = fw_event->header.fw_event_len;
      SNS_ASSERT(total_len >= sizeof(sns_fw_sensor_event) - sizeof(fw_event->event.event));
      this->num_alloc--;
      this->bytes_allocated -= total_len;
      this->buffer_tail = (uintptr_t)fw_event + total_len;

      if(this->rollover_here == this->buffer_tail)
      {
        this->buffer_tail = this->active_buffer_start;
        this->rollover_here = this->active_buffer_end;
      }

      fw_event = (sns_fw_sensor_event*)this->buffer_tail;
    }
  }
  else if(0 >= fw_event->header.client_cnt)
  {
    sns_free(fw_event);
  }

  attempt_reenable_island(this);
  sns_osa_lock_release(this->buffer_lock);
  SNS_PROFILE(SNS_EVENT_SERVICE_CONSUME_EVENT_END, 0);
}

SNS_SECTION(".text.sns") sns_rc
sns_event_service_send_event(sns_fw_data_stream *stream, uint32_t message_id,
  uint32_t buffer_len, uint8_t *buffer)
{
  sns_rc rv;
  sns_fw_sensor_event *fw_event = alloc_event(&event_service, buffer_len);
  int removing = 0;

  sns_osa_lock_acquire(stream->events_lock);
  removing = stream->removing;
  sns_osa_lock_release(stream->events_lock);

  fw_event->event.message_id = message_id;
  fw_event->event.timestamp = sns_get_system_time();
  sns_memscpy(fw_event->event.event, buffer_len, buffer, buffer_len);
  fw_event->event.event_len = buffer_len;
  fw_event->header.client_cnt = 1;

  rv = publish_client_event(&event_service, stream, fw_event);
  if(SNS_RC_SUCCESS == rv)
  {
    if(SNS_DATA_STREAM_AVAILABLE == removing)
      sns_fw_log_sensor_api_event(stream, &fw_event->event);
  }
  else
  {
    sns_event_service_consume(fw_event);
  }

  return rv;
}

SNS_SECTION(".text.sns") sns_rc
sns_event_service_send_error_event(sns_fw_data_stream *stream, sns_rc reason)
{
  sns_rc rv = SNS_RC_FAILED;
  sns_std_error_event event = sns_std_error_event_init_default;
  uint8_t buffer[sns_std_error_event_size];

  event.error = (sns_std_error)reason;
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sns_std_error_event_size);

  if(pb_encode(&ostream, sns_std_error_event_fields, &event))
  {
    rv = sns_event_service_send_event(stream, SNS_STD_MSGID_SNS_STD_ERROR_EVENT,
        sns_std_error_event_size, buffer);
  }
  else
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error in encoding event: %s",
        PB_GET_ERROR(&ostream));
  }
  return rv;
}

SNS_SECTION(".text.sns") sns_fw_sensor_event*
sns_event_service_get_fw_event(struct sns_sensor_event *event)
{
  sns_fw_sensor_event *rv = (sns_fw_sensor_event*)
    ((uintptr_t)event - sizeof(rv->header));

  return rv;
}

SNS_SECTION(".text.sns") void
sns_event_service_log_buffer_use(uint64_t cookie)
{
  sns_fw_event_service *this = (sns_fw_event_service*)&event_service;
  sns_diag_mem_utilization_log *log = sns_diag_log_alloc(
      sizeof(sns_diag_mem_utilization_log), SNS_LOG_MEMORY_UTILIZATION);
  if(NULL != log)
  {
    log->heap_id = SNS_DIAG_HEAP_ID_EVENT_BUFFER;
    log->timestamp = sns_get_system_time();

    sns_osa_lock_acquire(this->buffer_lock);
    log->total_memory = this->active_buffer_end - this->active_buffer_start;
    log->used_memory = this->bytes_allocated;
    log->has_cookie = true;
    log->cookie = cookie;
    sns_osa_lock_release(this->buffer_lock);

    sns_diag_publish_fw_log(SNS_LOG_MEMORY_UTILIZATION,
        sizeof(sns_diag_mem_utilization_log), log,
        sns_diag_mem_utilization_log_size,
        sns_event_service_log_buffer_usage_encode_cb);
  }
}

SNS_SECTION(".text.sns") void
sns_event_service_deinit_instance(struct sns_fw_sensor_instance *instance)
{
  sns_fw_event_service *this = &event_service;

  if(NULL != instance->unpublished_event)
  {
    sns_osa_lock_acquire(this->buffer_lock);
    instance->unpublished_event->header.client_cnt = 1;
    sns_osa_lock_release(this->buffer_lock);
    sns_event_service_consume(instance->unpublished_event);
    instance->unpublished_event = NULL;
  }
}

SNS_SECTION(".text.sns") bool
sns_event_service_low_mem_active(void)
{
  return atomic_load(&event_service.low_mem_active);
}

SNS_SECTION(".text.sns") void
sns_event_service_add_pending_event(sns_fw_data_stream *stream)
{
  unsigned int disabled = 0;

  if(atomic_compare_exchange_strong(&stream->unprocessed_events, &disabled, 1))
  {
    sns_sensor_library *dest = NULL != stream->dst_sensor
      ? stream->dst_sensor->library : stream->dst_instance->sensor->library;
    SNS_ASSERT(NULL != dest);

    sns_thread_manager_add(dest, sns_event_service_process,
        stream, stream->event_priority);
  }
}
