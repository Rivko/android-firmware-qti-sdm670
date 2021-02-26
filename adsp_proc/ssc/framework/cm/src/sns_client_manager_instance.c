/*=============================================================================
  @file sns_client_manager_instance.c

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  This module
  will translate incoming QMI request messages into Client Request objects,
  and similarly will translate Sensor Events into QMI indications.  This
  translation will be trivial, and this module will only concern itself with
  the requested batch period and the client processor.  The configuration
  request will be passed along to the appropriate Sensor, and this module
  will batch all data received (as appropriate).

  @note All QMI callbacks will be received within the same thread, and hence
  some thread protection is not necessary.  The CM Library mutex must be
  acquired prior to the internal QMI mutex(s).

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_data_stream.h"
#include "sns_fw_attribute_service.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"

#include "sns_assert.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_mem_util.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_types.h"

#include "sns_attribute_util.h"
#include "sns_client.pb.h"
#include "sns_client_api_v01.h"
#include "sns_client_manager.h"
#include "sns_client_manager_batch_timer.h"
#include "sns_client_manager_flush_manager.h"
#include "sns_client_manager_instance.h"
#include "sns_client_manager_qmi.h"
#include "sns_std.pb.h"

#include "sns_std_sensor.pb.h"

#include "sns_fw_log.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_profiler.h"
#include "sns_suid.pb.h"
#include "sns_fw_event_service.h"


/*=============================================================================
  Macro definitions
  ===========================================================================*/

/* Maximum payload length of the jumbo_report that will be used; bytes.
 * PEND: Profiling to determine the optimal size of the message. */
#define SNS_CM_MAX_IND_LEN 10240
/* For passive batching requests, CM sends the data out if actual report latency
 * is 70 percent of the expected report latency .
 */
#define SNS_CM_PASSIVE_REQ_RPT_LAT_RATIO 0.7f

/* Max number of event processed per notification callback from fwk. */
#define MAX_NUM_OF_EVENTS_PROCESSED 50
/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct sns_cm_batch_bin sns_cm_batch_bin;

/*=============================================================================
  Static Sensor  Function Definitions
  ===========================================================================*/

//The indication message defined to prevent allocation on stack
static sns_client_report_ind_msg_v01 cm_ind;
static sns_client_jumbo_report_ind_msg_v01 cm_jumbo_ind;

/**
 * Returns true if the message id is a non wakeup message id.
 *
 * @param[i] event_id         The event message id.
 * @param[i] cm_request       The client request message.
 *
 * @return   true             The message is of non wake up type.
 *           false            The message is of wakeup type.
 */
SNS_SECTION(".text.island")
static bool cm_is_nowk_event (
    const uint32_t event_id,
    const sns_cm_request *cm_request)
{
  bool rv = false;
  uint32_t * msg_id_ptr = cm_request->nowk_msg_ids;

  if (!sns_island_is_island_ptr((intptr_t)msg_id_ptr))
	  SNS_ISLAND_EXIT();

  for(int i=0; i< cm_request->nowk_msg_id_cnt; i++)
  {
    if(event_id == *(msg_id_ptr + i))
    {
      rv = true;
      break;
    }
  }
  return rv;
}

/**
 * Validates the client request message
 *
 * @param[i] cm_request           The client request message.
 *
 */
static sns_rc cm_validate_request(
    sns_client_request_msg *cm_request)
{
  sns_rc ret_val = SNS_RC_SUCCESS;

  // Validate the cm_request
  if((SNS_CLIENT_DELIVERY_WAKEUP != cm_request->susp_config.delivery_type) &&
     (SNS_CLIENT_DELIVERY_NO_WAKEUP != cm_request->susp_config.delivery_type))
  {
    cm_request->susp_config.delivery_type = SNS_CLIENT_DELIVERY_WAKEUP;
  }

  // Validate the sns_std_request
  if(cm_request->request.has_batching)
  {
    if(cm_request->request.batching.batch_period < SNS_CM_SHORTEST_BATCH_PERIOD_NS/1000 &&
       0 != cm_request->request.batching.batch_period)
    {
      ret_val = SNS_RC_INVALID_VALUE;
    }
  }
  return ret_val;
}

/**
 * Encode a sensor event.
 */
static bool cm_encode_events(
  pb_ostream_t *stream,
  const pb_field_t *field,
  void *const *arg)
{
  sns_sensor_event *event = (sns_sensor_event*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, (pb_byte_t*)event->event, event->event_len))
    return false;

  return true;
}

/**
 * This function encodes and sends a QMI indication for a single event.
 *
 * PEND: If client is busy, data should not be dropped (per flush_period).
 *
 * @param[i] this          The client manager instance.
 * @param[i] sensor        The sensor which generated the event.
 * @param[i] use_jumbo_report Whether to use cm_ind or cm_jumbo_ind
 * @param[i] payload_len   The number of bytes in the payload.
 */
static qmi_csi_error cm_send_qmi_ind(
    sns_sensor_instance *this,
    sns_fw_sensor const *sensor,
    bool use_jumbo_report,
    uint32_t payload_len)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;
  uint32_t ind_len = sizeof(cm_ind);
  sns_client_jumbo_report_ind_msg_v01 *ind = (void*)&cm_ind;
  qmi_csi_error err;
  uint32_t msg_id = SNS_CLIENT_REPORT_IND_V01;

  if(use_jumbo_report)
  {
    ind = &cm_jumbo_ind;
    ind_len = sizeof(cm_jumbo_ind);
    msg_id = SNS_CLIENT_JUMBO_REPORT_IND_V01;
  }

  ind->client_id = client->client_id;
  ind->payload_len = payload_len;

  sns_fw_log_client_api_ind(this, &sensor->sensor,
                            cm_get_sensor_uid(NULL),
                            ind->client_id, ind->payload, ind->payload_len);

  err = qmi_csi_send_ind(client->qmi_handle, msg_id, ind, ind_len);
  if(QMI_CSI_NO_ERR != err)
  {
    //We can hit this error if the client disconnected while
    //we were sending data.So printing a high log here.
    SNS_INST_PRINTF(HIGH,this, "qmi_csi_send_ind error %i", err);
    if(QMI_CSI_CONN_BUSY == err)
    {
       client->busy = true;
    }
  }
  return err;
}

/**
 * This function creates a pb ostream buffer for the cm indication payload.
 *
 * PEND: When QMI busy -> dropped events bug is fixed, also attempt to use
 *       smaller indication when sending only a single event (and use jumbo)
 *       only as a fallback.
 * @param[o] stream The output pb stream.
 * @param[i] use_jumbo_report Whether to use cm_ind or cm_jumbo_ind
 * @param[i] sensor The CM sensor object.
 * @param[i] this The CM sensor instance.
 *
 * @return True if buffer is re-initialized; False Otherwise.
 */
static bool cm_ind_pb_buff_stream_reset(
    pb_ostream_t *stream,
    bool use_jumbo_report,
    sns_fw_sensor const *sensor,
    sns_sensor_instance *this)
{
  sns_sensor_uid suid = sns_attr_info_get_suid(sensor->attr_info);
  sns_std_suid uid;
  sns_memscpy(&uid, sizeof(uid), &suid, sizeof(suid));

  if(use_jumbo_report)
    *stream = pb_ostream_from_buffer(cm_jumbo_ind.payload, SNS_CM_MAX_IND_LEN);
  else
    *stream = pb_ostream_from_buffer(cm_ind.payload, sizeof(cm_ind.payload));
  if(!pb_encode_tag(stream, PB_WT_STRING, sns_client_event_msg_suid_tag) ||
     !pb_encode_submessage(stream, sns_std_suid_fields, &uid))
  {
    SNS_INST_PRINTF(ERROR, this, "Error encoding events");
    return false;
  }
  return true;
}

/**
 * Get the combined encoded tag and message size for a sub-message.
 */
static bool cm_get_encoded_header_size(
    size_t *size,
    pb_wire_type_t wiretype,
    uint32_t field_number,
    size_t msg_size)
{
  pb_ostream_t stream_tag = PB_OSTREAM_SIZING;
  pb_ostream_t stream_msg = PB_OSTREAM_SIZING;
  if(!pb_encode_tag(&stream_tag, wiretype, field_number) ||
     !pb_encode_varint(&stream_msg, (uint64_t)msg_size))
  {
    return false;
  } else
  {
    *size = stream_tag.bytes_written + stream_msg.bytes_written;
    return true;
  }
}

/**
 * This function encodes a single event into the cm indication pb payload.
 *
 * @param[i] stream        The output pb stream.
 * @param[i] event         The event to be encoded.
 * @param[i] this          The CM sensor instance.
 *
 * @return true if event successfully encoded
 *         false if the stream buffer is full
 */
static bool cm_ind_pb_encode_event(
    pb_ostream_t *stream,
    sns_sensor_event *event,
    sns_sensor_instance *this)
{
  sns_client_event_msg_sns_client_event sub_message;
  size_t encoded_len;
  size_t encoded_header_len;

  sub_message.msg_id = event->message_id;
  sub_message.timestamp = event->timestamp;
  sub_message.payload.funcs.encode = &cm_encode_events;
  sub_message.payload.arg = event;

  bool success = false;
  size_t initial_stream_len = stream->bytes_written;

  // PEND: Optimize the pb_get_encoded size
  if(pb_get_encoded_size(&encoded_len,
        sns_client_event_msg_sns_client_event_fields, &sub_message) &&
     cm_get_encoded_header_size(&encoded_header_len,
        PB_WT_STRING, sns_client_event_msg_events_tag, encoded_len))
  {
    if(stream->bytes_written + encoded_len + encoded_header_len > stream->max_size)
    {
      SNS_INST_PRINTF(HIGH, this, "CM Ind payload size reached %d",
          stream->bytes_written + encoded_len + encoded_header_len);
    }
    else if(!pb_encode_tag(stream, PB_WT_STRING, sns_client_event_msg_events_tag))
    {
      SNS_INST_PRINTF(ERROR, this, "Error encoding tag");
    }
    else if (!pb_encode_varint(stream, (uint64_t)encoded_len))
    {
      SNS_INST_PRINTF(ERROR, this, "Error encoding message size");
    }
    else if(!pb_encode(stream, sns_client_event_msg_sns_client_event_fields, &sub_message))
    {
      SNS_INST_PRINTF(ERROR, this, "Error encoding message len %d", event->event_len);
    }
    else
    {
      success = true;
    }
  }

  // Reset stream length to ignore possibly encoded tag and/or size.
  if(!success)
  {
    stream->bytes_written = initial_stream_len;
  }
  return success;
}

/**
 * This function encodes and sends a QMI indication for a single event.
 *
 * @param[i] this   The client manager instance.
 * @param[i] event  The actual event being sent
 * @param[i] sensor The sensor which generated the event.
 *
 * @return   qmi_csi_err code.
 */
static qmi_csi_error cm_encode_and_send_single_ind (
   sns_sensor_instance *this,
   sns_sensor_event *event,
   sns_fw_sensor const *sensor)
{
  pb_ostream_t stream;
  sns_std_suid uid;
  qmi_csi_error rv =  QMI_CSI_NO_ERR;
  sns_sensor_uid suid = sns_attr_info_get_suid(sensor->attr_info);
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;

  sns_memscpy(&uid, sizeof(uid), &suid, sizeof(suid));

  if(!cm_ind_pb_buff_stream_reset(&stream, client->use_jumbo_report, sensor, this))
  {
    SNS_INST_PRINTF(ERROR,this, "Error encoding cm tag");
  }
  else
  {
    if(cm_ind_pb_encode_event(&stream, event, this))
    {
       rv = cm_send_qmi_ind(this, sensor,
          client->use_jumbo_report, stream.bytes_written);
      //We can hit this error if the client disconnected while
      //we were sending data.So printing a high log here.
      if(QMI_CSI_NO_ERR != rv)
      {
        SNS_INST_PRINTF(HIGH, this, "cm_encode_and_send_single_ind"
            " send_qmi_ind error %i", rv);
      }
    }
    else
      SNS_INST_PRINTF(ERROR, this, "cm_encode_and_send_single_ind"
          " Encoding sub message failed");
  }
  return rv;
}

/**
 * Updates the first batch and last batch ts for a batching request.
 *
 * @param[i] cm_request The request in which the event is stored.
 */
SNS_SECTION(".text.island")
static void cm_update_batch_ts(
    sns_cm_request *cm_request)
{
  sns_isafe_list_iter iter;
  sns_cm_batch_bin *cm_bin = NULL;
  int data_count =0;
  sns_isafe_list_item *curr_batch_buff = NULL;

  //Find the first data event and update the first batch ts.
  curr_batch_buff = sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);
  while(NULL != curr_batch_buff)
  {
    cm_bin = (sns_cm_batch_bin*)
             sns_isafe_list_item_get_data(curr_batch_buff);
    //Drop a data event only
    if((cm_bin->event.message_id >=1024) &&
        (cm_bin->event.message_id <=1536))
    {
      data_count++;
      cm_request->first_batch_ts = cm_bin->event.timestamp;
      break;
    }
    else
      curr_batch_buff = sns_isafe_list_iter_advance(&iter);
  }
  //If no data events left after truncation.
  if(!data_count)
  {
    cm_request->first_batch_ts = 0;
    cm_request->last_batch_ts =  0;
  }
}

/**
 * This function trims the batch buffer associated with a request.
 * Drops the SNS_CM_BATCH_BIN_DROP_COUNT number of oldest events.
 *
 * @param[i] req_msg The client request message.
 */
SNS_SECTION(".text.island")
static void cm_trim_batch_buffer_list(
    sns_cm_request *cm_req_msg)
{
  sns_isafe_list_iter batch_bin_iter;
  uint32_t drop_count =0 ;
  for(sns_isafe_list_iter_init(&batch_bin_iter, &cm_req_msg->batch_buff, true);
      NULL != sns_isafe_list_iter_curr(&batch_bin_iter);)
  {
    sns_cm_batch_bin *cm_bin = (sns_cm_batch_bin*)
        sns_isafe_list_iter_get_curr_data(&batch_bin_iter);
    sns_isafe_list_iter_remove(&batch_bin_iter);
    sns_free(cm_bin);
    cm_bin = NULL;
    drop_count++;
    if(drop_count >= SNS_CM_BATCH_BIN_DROP_COUNT)
    {
      break;
    }
  }
  //Update the time stamps
  cm_update_batch_ts(cm_req_msg);
}

/**
 * This function drops the batch buffer associated with a request.
 *
 * @param[i] req_msg The client request message.
 */
static void cm_clear_batch_buffer_list(
    sns_cm_request *cm_req_msg)
{
  sns_isafe_list_iter batch_bin_iter;
  for(sns_isafe_list_iter_init(&batch_bin_iter, &cm_req_msg->batch_buff, true);
      NULL != sns_isafe_list_iter_curr(&batch_bin_iter);)
  {
    sns_cm_batch_bin *cm_bin = (sns_cm_batch_bin*)
        sns_isafe_list_iter_get_curr_data(&batch_bin_iter);
    sns_isafe_list_iter_remove(&batch_bin_iter);
    sns_free(cm_bin);
    cm_bin = NULL;
  }
  //Update the time stamps
  cm_update_batch_ts(cm_req_msg);
}

/**
 * This function encodes all events of a request,
 * and sends the data as indications on QMI.
 * If the last event sent is not a flush event, we
 * send a flush event to indicate end of batch.
 *
 * @param[i] this              The client manager instance.
 * @param[i] sensor            The sensor which generated the event.
 * @param[i] cm_request        The request message .
 * @param[o] last_sent_msg_id  The last event message id encoded in the QMI message.
 */
static void cm_send_events(
    sns_sensor_instance *this,
    sns_fw_sensor const *sensor,
    sns_cm_request *cm_request,
    uint32_t *last_sent_msg_id)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;
  sns_sensor_uid suid = sns_attr_info_get_suid(sensor->attr_info);
  sns_std_suid uid;
  pb_ostream_t stream;
  bool message_full = true;
  sns_isafe_list_iter iter;
  sns_time last_sent_ts = 0;
  uint32_t last_sent_msg = 0;
  uint32_t send_count = 0;
  sns_memscpy(&uid, sizeof(uid), &suid, sizeof(suid));

  sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);

#if ENABLE_DEBUG_PRINT
  SNS_SPRINTF(ERROR, sns_fw_printf, "Items before %d first_ts %llu last_ts %llu",
              sns_isafe_list_iter_len(&iter),cm_request->first_batch_ts,cm_request->last_batch_ts);
#endif
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_cm_batch_bin *batch_bin = (sns_cm_batch_bin*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if((last_sent_msg == batch_bin->event.message_id) &&
       (last_sent_ts >= batch_bin->event.timestamp))
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "Duplicate event id %d ts %llu req %0x",
	              batch_bin->event.message_id,batch_bin->event.timestamp,cm_request);
      SNS_SPRINTF(ERROR, sns_fw_printf,
          "Sensor suid suid_high[low] %x suid_high[high] %x suid_low[low] %x suid_low[high] %x",
          SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_high),
          SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_high),
          SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_low),
          SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_low));
    }
    if(message_full)
    {
      if(!cm_ind_pb_buff_stream_reset(&stream, client->use_jumbo_report, sensor, this))
      {
        SNS_INST_PRINTF(ERROR, this, "Error encoding cm tag req %0x",cm_request);
        break;
      }
    }

    message_full = !cm_ind_pb_encode_event(&stream, &batch_bin->event, this);
    if(!message_full)
    {
      last_sent_ts = batch_bin->event.timestamp;
      last_sent_msg =  batch_bin->event.message_id;
      send_count++;
      sns_isafe_list_iter_advance(&iter);
    }

    // If the message is full, or if there are no further events to encode
    if(message_full || NULL == sns_isafe_list_iter_curr(&iter))
    {
      qmi_csi_error err;
      *last_sent_msg_id = batch_bin->event.message_id;

      err = cm_send_qmi_ind(this, sensor, client->use_jumbo_report, stream.bytes_written);
      //Do not drop events if QMI busy, drop for all other QMI errors
      if(QMI_CSI_CONN_BUSY == err)
      {
        SNS_INST_PRINTF(ERROR, this, "send_qmi_ind error %i", err);
        break;
      }
      else
      {
        //Reset the iterator to head for removal
        sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);
        while (send_count != 0)
        {
          if(NULL != sns_isafe_list_iter_curr(&iter))
          {
            sns_cm_batch_bin *batch_bin_remove = (sns_cm_batch_bin*)
                  sns_isafe_list_iter_get_curr_data(&iter);
            sns_isafe_list_iter_remove(&iter);
            sns_free(batch_bin_remove);
          }
          else
          {
            break;
          }
          send_count--;
        }
        //Re-init iterator to head again after removal
        sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);
      }
    }
  }

  //Update the first and last timestamps based on the data left.
  sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);
  if(0 != sns_isafe_list_iter_len(&iter))
  {
    while(NULL != sns_isafe_list_iter_curr(&iter))
    {
      sns_cm_batch_bin *cm_bin = (sns_cm_batch_bin*)
              sns_isafe_list_iter_get_curr_data(&iter);
      if((cm_bin->event.message_id >=1024) &&
          (cm_bin->event.message_id <=1536))
      {
        cm_request->first_batch_ts = cm_bin->event.timestamp;
#if ENABLE_DEBUG_PRINT
        SNS_INST_PRINTF(ERROR, this, "Updating first batch timestamp in request %0x",cm_request);
#endif
         break;
      }
      else
        sns_isafe_list_iter_advance(&iter);
    }
  }
  else
  {
#if ENABLE_DEBUG_PRINT
    SNS_INST_PRINTF(ERROR, this, "Resetting both batch timestamps in request %0x",cm_request);
#endif
    cm_request->first_batch_ts = 0;
    cm_request->last_batch_ts = 0;
  }

#if ENABLE_DEBUG_PRINT
  sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);
  SNS_SPRINTF(ERROR, sns_fw_printf, "cm_req %0x Items after %d first_ts %llu last_ts %llu",
              cm_request,sns_isafe_list_iter_len(&iter),cm_request->first_batch_ts,cm_request->last_batch_ts);
#endif
}


/**
 * Find a CM request for an instance associated with a particular SUID.
 *
 * @note This makes the assumption that only a single data stream per
 * SUID is ever created.  This is a restriction we place on the Client API.
 *
 * @param[i] instance Current client
 * @param[i] suid
 *
 * @return Data stream found, or NULL
 */
static sns_cm_request* cm_find_cm_request(
  sns_sensor_instance *instance,
  sns_std_suid *suid)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;

  sns_isafe_list_iter iter;
  sns_cm_request *request = NULL;

  for(sns_isafe_list_iter_init(&iter, &client->req_list, true);
      NULL != (request = (sns_cm_request*)sns_isafe_list_iter_get_curr_data(&iter));
      sns_isafe_list_iter_advance(&iter))
  {
    if(sns_sensor_uid_compare((sns_sensor_uid*)suid,
                             (sns_sensor_uid*)&request->req_msg.suid))
    {
      return request;
    }
  }
  return NULL;
}
/**
 * This function clears the data event message id tracker list.
 *
 * @param[i] req_msg The CM request structure.
 */
static void cm_clear_data_evnt_msg_lst(
    sns_cm_request *cm_req_msg)
{
  sns_isafe_list_iter iter;
  for(sns_isafe_list_iter_init(&iter, &cm_req_msg->data_evnt_msg_lst, true);
      NULL != sns_isafe_list_iter_curr(&iter);)
  {
    sns_cm_data_evnt_msg *curr = (sns_cm_data_evnt_msg*)
        sns_isafe_list_iter_get_curr_data(&iter);
    sns_isafe_list_iter_remove(&iter);
    sns_free(curr);
    curr = NULL;
  }
}

/**
 * This function resests  the data event message id entries
 * in the tracker list.
 *
 * @param[i] req_msg The CM request structure.
 */
static void cm_rst_data_evnt_msg_lst(
    sns_cm_request *cm_req_msg)
{
  sns_isafe_list_iter iter;
  for(sns_isafe_list_iter_init(&iter, &cm_req_msg->data_evnt_msg_lst, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_cm_data_evnt_msg *curr = (sns_cm_data_evnt_msg*)
        sns_isafe_list_iter_get_curr_data(&iter);
    curr->msg_id = -1;
    curr->ts = 0;
  }
}

/*
 * Validates if a received event is allowed based on the timestamp.
 *
 * @param[i] req_msg          The CM request structure.
 * @param[i] event            The received sensor event.
 * @param[i] set_flag         True if the msg id is found n set.
 *                            False if the msg id is to be found only.
 *
 * @return   True        If message id is received for first time.
 *                       Or if the new received event for the message id
 *                       has newer timestamp.
 *
 *           False       Otherwise.
 */
SNS_SECTION(".text.island")
static bool cm_findnset_data_evnt_msg_lst(
    sns_cm_request *req_msg,
    sns_sensor_event *event,
    bool set_flag)
{
  sns_isafe_list_iter iter;
  bool rv = false;
  int index = -1,found_at_index = -1;
  sns_cm_data_evnt_msg *cm_data_msg = NULL;
  for(sns_isafe_list_iter_init(&iter, &req_msg->data_evnt_msg_lst, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    cm_data_msg = (sns_cm_data_evnt_msg*)
        sns_isafe_list_iter_get_curr_data(&iter);
    index++;

    if(event->message_id == cm_data_msg->msg_id)
    {
       found_at_index = index;
       //Only find operation
       if(!set_flag)
       {
         rv = true;
         break;
       }
       //Only if new event
       else if(event->timestamp > cm_data_msg->ts)
       {
         cm_data_msg->ts = event->timestamp;
         rv = true;
       }
       break;
    }
    //First time the message id is detected.
    else if (-1 == cm_data_msg->msg_id)
    {
      cm_data_msg->ts = event->timestamp;
      cm_data_msg->msg_id = event->message_id;
      rv = true;
      found_at_index = index;
      break;
    }
  }

  //If the message was found but it was not at head.
  if(rv && (0 < found_at_index))
  {
    //Optimization to put the latest received message id at the head of the list.
    sns_isafe_list_iter_remove(&iter);
    sns_isafe_list_iter_init(&iter, &req_msg->data_evnt_msg_lst, true);
    sns_isafe_list_item_init(&cm_data_msg->item, cm_data_msg);
    sns_isafe_list_iter_insert(&iter, &cm_data_msg->item, false);
  }
  //If we cannot accommodate a new message id in the list
  //We add it to the list and then allow the message to go through.
  else if(-1 == found_at_index)
  {
    cm_data_msg = (sns_cm_data_evnt_msg*)
          sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_cm_data_evnt_msg));
    if(cm_data_msg == NULL)
    {
      SNS_ISLAND_EXIT();
      cm_data_msg = (sns_cm_data_evnt_msg*)
             sns_malloc(SNS_HEAP_MAIN, sizeof(sns_cm_data_evnt_msg));
      SNS_ASSERT(NULL != cm_data_msg);
    }
    cm_data_msg->ts = event->timestamp;
    cm_data_msg->msg_id = event->message_id;
    sns_isafe_list_iter_init(&iter, &req_msg->data_evnt_msg_lst, true);
    sns_isafe_list_item_init(&cm_data_msg->item, cm_data_msg);
    sns_isafe_list_iter_insert(&iter, &cm_data_msg->item, false);
    rv = true;
  }
  return rv;
}


/**
 * Inserts the event into the cm_request batch buffer list
 *
 * @param[i] cm_batch_bin The item to be inserted into the list.
 * @param[i] cm_request   The request into which the event is to be stored in.
 */
SNS_SECTION(".text.island")
static void cm_insert_event(
   sns_cm_batch_bin *cm_batch_bin,
   sns_cm_request *cm_request,
   sns_fw_sensor *cm_sensor)
{
  SNS_PROFILE(SNS_CM_INSERT_EVENT_FUNC, 1, 1);
  sns_isafe_list_iter iter;
  sns_isafe_list_item_init(&cm_batch_bin->item, cm_batch_bin);
  sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, false);
  if( !cm_is_nowk_event(cm_batch_bin->event.message_id, cm_request) )
  {
    //If timestamp check fails
    if(!cm_findnset_data_evnt_msg_lst(cm_request, &cm_batch_bin->event, false))
    {
      SNS_PRINTF(MED, sns_fw_printf, "Invalid event - Message id %d Ts 0x%x%08x",
           cm_batch_bin->event.message_id,
           (uint32_t)(cm_batch_bin->event.timestamp >>32),
           (uint32_t)(cm_batch_bin->event.timestamp) );
      SNS_PRINTF(MED, sns_fw_printf,
           "suid_high[low] %x suid_high[high] %x suid_low[low] %x suid_low[high] %x",
           SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_high),
           SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_high),
           SNS_CM_FETCH_LOW_32(cm_request->req_msg.suid.suid_low),
           SNS_CM_FETCH_HI_32(cm_request->req_msg.suid.suid_low));
      sns_free(cm_batch_bin);
    }
    else
    {
      sns_isafe_list_iter_insert(&iter, &cm_batch_bin->item, true);
      cm_request->last_batch_ts = cm_batch_bin->event.timestamp;
    }
    if(0 == cm_request->first_batch_ts)
      cm_request->first_batch_ts = cm_request->last_batch_ts;
  }
  else
  {
    sns_isafe_list_iter_insert(&iter, &cm_batch_bin->item, true);
  }
  if((cm_request->last_batch_ts >= (cm_request->first_batch_ts+ 
      cm_request->compare_period)) &&
      !cm_request->req_msg.request.batching.flush_only)
  {
	if( SNS_CLIENT_DELIVERY_WAKEUP ==
			cm_request->req_msg.susp_config.delivery_type)
	{
      SNS_PRINTF(HIGH, cm_sensor,
    		  "Sending data for wakeup req %0x", cm_request);
      sns_cm_send_all_data_for_proc(cm_sensor,
          cm_request->req_msg.susp_config.client_proc_type, false);
	}
	else if(!cm_request->proc_susp_state)
	{
      SNS_PRINTF(HIGH, cm_sensor,
    		 "Sending data for non-wakeup req %0x", cm_request);
      sns_cm_send_all_data_for_proc(cm_sensor,
          cm_request->req_msg.susp_config.client_proc_type, false);
	}
  }

  SNS_PROFILE(SNS_CM_INSERT_EVENT_FUNC, 1, 2);
}

/*
 * This function sends the data accumulated on a request and is a non island
 * function.
 *
 * @param[i] cm_sensor_inst    The CM sensor instance.
 * @param[i] cm_req            The CM maintained request structure, whose data needs to be sent.
 * @param[i] drop_events      Flag to indicate low memory condition.
 */
static void cm_send_cm_req_data(
    sns_sensor_instance *cm_sensor_inst,
    sns_cm_request *cm_req,
    bool drop_events)
{
  sns_cm_inst_state *cm_inst = (sns_cm_inst_state*)cm_sensor_inst->state->state;
  sns_fw_data_stream *fw_data_stream = (sns_fw_data_stream*)cm_req->data_stream;
  sns_fw_sensor *fw_src_sensor = (sns_fw_sensor*)fw_data_stream->data_source;

  //If client is not busy, data can be sent
  if(!cm_inst->busy)
  {
    uint32_t last_sent_msg_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
    //Send data if not flush only or a flush is requested for a flush only
    //client
    if(( cm_req->req_msg.request.batching.flush_only  &&
         (SNS_CM_CLIENT_FLUSH == cm_req->flush_type)))
    {
      cm_send_events(cm_sensor_inst, fw_src_sensor, cm_req, &last_sent_msg_id);
    }
    else if (!cm_req->req_msg.request.batching.flush_only)
    {
      //If client flush, send all data irrespective of what is accumulated.
      if(SNS_CM_CLIENT_FLUSH == cm_req->flush_type)
      {
        cm_send_events(cm_sensor_inst, fw_src_sensor, cm_req, &last_sent_msg_id);
      }
      else
      {
         if( 0 != cm_req->first_batch_ts)
         {
           cm_send_events(cm_sensor_inst, fw_src_sensor, cm_req, &last_sent_msg_id);
         }
      }
    }
  }
  //If low memory is already hit,drop some data.
  if(drop_events)
  {
     SNS_INST_PRINTF(ERROR,cm_sensor_inst,
         "Client busy but low memory condition. Dropping samples");
     cm_trim_batch_buffer_list(cm_req);
  }
}

/**
 * Removes the oldest data event in the batch buffer list for a request.
 *
 * Do not remove configuration events.
 *
 * @param[i] cm_request The request in which the event is stored.
 */
SNS_SECTION(".text.island")
static void cm_remove_oldest_events(
    sns_cm_request *cm_request)
{
  SNS_PROFILE(SNS_CM_REMOVE_OLDEST_EVENT_FUNC, 1, 1);
  sns_cm_batch_bin *cm_bin = NULL;
  sns_isafe_list_iter iter;
  sns_isafe_list_iter_init(&iter, &cm_request->batch_buff, true);

  //Drop events till we fall below compare period or
  //we run out of events.
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    cm_bin = (sns_cm_batch_bin*)
             sns_isafe_list_iter_get_curr_data(&iter);
    //Drop a data event only
    if((cm_bin->event.message_id >=1024) &&
       (cm_bin->event.message_id <=1536))
    {
       if((cm_request->last_batch_ts - cm_bin->event.timestamp) >=
        cm_request->compare_period)
       {
         sns_isafe_list_iter_remove(&iter);
         sns_free(cm_bin);
       }
       else
       {
         break;
       }
    }
    else
      sns_isafe_list_iter_advance(&iter);
  }
  cm_update_batch_ts(cm_request);
  SNS_PROFILE(SNS_CM_REMOVE_OLDEST_EVENT_FUNC, 1, 2);
}

SNS_SECTION(".text.island")
void sns_cm_bump_down_on_flush_compl (
   sns_sensor_instance *instance)
{
  sns_fw_sensor_instance *cm_sensor_inst = (sns_fw_sensor_instance *)instance;
  sns_sensor *cm_sensor =(sns_sensor *)cm_sensor_inst->sensor;
  sns_cm_state *state = (sns_cm_state*)cm_sensor->state->state;

  //Bump down only if it was bumped up
  if(state->bump_up)
  {
    state->bump_up = false;
    SNS_INST_PRINTF(HIGH, instance, "MCPS bump down bump state %d",state->bump_up);
    sns_pwr_scpm_mgr_mcps_bump_down(state->scpm_client);
  }
}

SNS_SECTION(".text.island")
void sns_cm_bump_up_on_flush_initiate (
   sns_sensor_instance *instance)
{
  sns_fw_sensor_instance *cm_sensor_inst = (sns_fw_sensor_instance *)instance;
  sns_sensor *cm_sensor =(sns_sensor *)cm_sensor_inst->sensor;
  sns_cm_state *state = (sns_cm_state*)cm_sensor->state->state;
  //Bump up only if not done so already
  if(!state->bump_up &&
     sns_cm_is_island_blocked(cm_sensor, SNS_STD_CLIENT_PROCESSOR_APSS) )
  {
    state->bump_up = true;
    SNS_INST_PRINTF(HIGH, instance,
          "MCPS bump up bump state %d",state->bump_up);
    sns_pwr_scpm_mgr_mcps_bump_up(state->scpm_client);
  }
}

/**
 * Removes the oldest data event in the batch buffer list for all
 * batching requests.
 * Does not remove configuration events.
 *
 * @param[i] cm_sensor Framework sensor pointer for CM sensor.
 */
SNS_SECTION(".text.island")
static void sns_cm_drop_oldest_batch_data(sns_fw_sensor *cm_sensor)
{
  SNS_PROFILE(SNS_CM_DROP_OLDEST_BATCH_DATA_FUNC, 1, 1);

  sns_sensor_instance *cm_sensor_inst = NULL;
  sns_isafe_list_iter instance_iter;
  sns_isafe_list_iter_init(&instance_iter, &cm_sensor->sensor_instances, true);
  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
    cm_sensor_inst = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
    if(NULL != cm_sensor_inst)
    {
      sns_isafe_list_iter req_iter;
      sns_cm_inst_state *cm_inst =
        (sns_cm_inst_state*)cm_sensor_inst->state->state;

      for(sns_isafe_list_iter_init(&req_iter,&cm_inst->req_list, true);
          NULL != sns_isafe_list_iter_curr(&req_iter);
          sns_isafe_list_iter_advance(&req_iter))
      {
        sns_cm_request *cm_request = (sns_cm_request*)
          sns_isafe_list_iter_get_curr_data(&req_iter);
        cm_proc_info *proc_info = sns_cm_get_proc_info((sns_sensor*)cm_sensor,
            cm_request->req_msg.susp_config.client_proc_type);

        SNS_ASSERT(NULL != proc_info);

        if( (cm_request->req_msg.request.has_batching &&
            !cm_inst->busy &&
            (SNS_CLIENT_DELIVERY_WAKEUP !=
            cm_request->req_msg.susp_config.delivery_type)
            && proc_info->is_suspended) ||
            cm_request->req_msg.request.batching.flush_only)
        {
          if(cm_request->last_batch_ts - cm_request->first_batch_ts >=
             cm_request->compare_period)
          {
            cm_remove_oldest_events(cm_request);
          }
        }
      }
    }
    sns_isafe_list_iter_advance(&instance_iter);
  }

  SNS_PROFILE(SNS_CM_DROP_OLDEST_BATCH_DATA_FUNC, 1, 2);
}

/**
 * This functions stores the event in the batch queue belonging to the request.
 *
 * @param[i] event      The sensor event that needs to be stored.
 * @param[i] cm_request The CM request structure associated with the event.
 * @param[i] this       The CM sensor instance pointer.
 *
 * @return
 * SNS_RC_FAILED - Storage failure
 * SNS_RC_SUCCESS
 */
SNS_SECTION(".text.island")
static sns_rc sns_cm_store_event(
   sns_sensor_event *event,
   sns_cm_request *cm_request,
   sns_sensor_instance *this)
{
  SNS_PROFILE(SNS_CM_STORE_EVENT_FUNC, 1, 1);
  sns_cm_batch_bin *cm_batch_bin = NULL;
  sns_rc rc = SNS_RC_SUCCESS;

  size_t struct_len = sizeof(sns_cm_batch_bin)- sizeof(event->event) +event->event_len;

  if(NULL != cm_request)
  {
    if((SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == event->message_id) &&
        (SNS_CM_BATCH_FLUSH == cm_request->flush_type) &&
        (0 == cm_request->first_batch_ts))
    {
      SNS_INST_PRINTF(LOW, this, "Got a flush event before any data. Drop it");
      return rc;
    }
    sns_data_stream *data_stream = cm_request->data_stream;
    sns_fw_data_stream *fw_stream = (sns_fw_data_stream*)data_stream;
    sns_fw_sensor_instance *cm_sensor_inst = fw_stream->dst_instance;
    sns_fw_sensor *cm_sensor = cm_sensor_inst->sensor;
    sns_cm_state *cm_state = (sns_cm_state*)cm_sensor->sensor.state->state;
    cm_batch_bin = (sns_cm_batch_bin*)sns_malloc(
        SNS_HEAP_ISLAND, struct_len);

    if(NULL == cm_batch_bin)
    {
      sns_cm_drop_oldest_batch_data(cm_sensor);
      if(!(cm_state->registry_config.max_batch_disabled))
      {
        SNS_ISLAND_EXIT();
        if (true == cm_state->can_flush_to_ddr)
        {
          sns_cm_handle_flush_to_ddr_memory(cm_sensor);
        }
        cm_state->can_flush_to_ddr = false;
      }
      // Try really hard to get a batch_bin in island mode.
      cm_batch_bin = (sns_cm_batch_bin*)sns_malloc(
        SNS_HEAP_ISLAND,struct_len);
    }
    if(NULL == cm_batch_bin)
    {
      SNS_ISLAND_EXIT();
      cm_batch_bin = (sns_cm_batch_bin*)sns_malloc(
          SNS_HEAP_BATCH, struct_len);

      //If no memory left, send out all data and free the memory here.
      if(NULL == cm_batch_bin)
      {
         cm_handle_low_batch_memory(cm_sensor,true);

        // try in island first as some island memory might
        // have got freed
        cm_batch_bin = (sns_cm_batch_bin*)sns_malloc(
            SNS_HEAP_ISLAND,struct_len);

        if(NULL == cm_batch_bin)
        {
         cm_batch_bin = (sns_cm_batch_bin*)sns_malloc(
              SNS_HEAP_BATCH,struct_len);
        }
         SNS_ASSERT(cm_batch_bin != NULL);
      }
    }

    if(sns_island_is_island_ptr((intptr_t)cm_batch_bin)) 
    {
      cm_state->can_flush_to_ddr = true;
    }

    cm_batch_bin->event.event_len = event->event_len;
    cm_batch_bin->event.message_id = event->message_id;
    cm_batch_bin->event.timestamp = event->timestamp;

    if(event->event_len)
    {
      sns_memscpy((pb_byte_t*)cm_batch_bin->event.event, event->event_len,
          (pb_byte_t*)event->event, event->event_len);
    }
    if(0 == cm_request->req_msg.request.batching.flush_period)
    {
      sns_free(cm_batch_bin);
  }
  else
  {
      //Keep adding, removal or truncation will happen in big image
      cm_insert_event(cm_batch_bin, cm_request, cm_sensor);
      if(cm_state->registry_config.max_batch_disabled)
      {
        sns_cm_drop_oldest_batch_data(cm_sensor);
      }
    }
    //If passive request but the report period has elapsed,
    //we should send all the data thats available.
    if (cm_request->req_msg.request.is_passive)
    {

      sns_time compare_period =
          cm_request->last_batch_ts - cm_request->first_batch_ts;
      sns_time report_latency = sns_convert_ns_to_ticks(
        cm_request->req_msg.request.batching.batch_period * 1000ULL);

      //Passive requests are always treated as wake-up requests, so send
      //data.
      if( compare_period >=
          ( SNS_CM_PASSIVE_REQ_RPT_LAT_RATIO*report_latency ))
      {
        sns_cm_send_all_data_for_proc(cm_sensor,
            cm_request->req_msg.susp_config.client_proc_type, false);
      }
    }
  }
  else
  {
    SNS_INST_PRINTF(ERROR,this,"cm_request is NULL");
  }
  SNS_PROFILE(SNS_CM_STORE_EVENT_FUNC, 1, 2);
  return rc;
}

/**
 * This functions decides whether to send the data for a request event.
 *
 * @param[i] this       The CM sensor instance.
 * @param[i] event      Event received to be sent
 * @param[i] cm_req_msg The CM request which needs
 *                      to be evaluated for sending indications on.
 *
 * @return
 * True -  Indication can be sent.
 * False - Indication cannot be sent.
 */
SNS_SECTION(".text.island")
static bool sns_cm_can_send(
   sns_sensor_instance *this,
   sns_sensor_event    *event,
   sns_cm_request *cm_req_msg )
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;
  bool rv = true;

  //Flush only takes precendence always
  if(( cm_req_msg->req_msg.request.batching.flush_only ) &&
     ( cm_req_msg->flush_type != SNS_CM_CLIENT_FLUSH ) &&
     ( SNS_STD_MSGID_SNS_STD_FLUSH_EVENT != event->message_id ))
  {
    rv = false;
  }
  else if(SNS_CLIENT_DELIVERY_WAKEUP == cm_req_msg->req_msg.susp_config.delivery_type)
  {
    //If processor is suspended and non wakeup message id
    if (cm_req_msg->proc_susp_state &&
        cm_is_nowk_event(event->message_id, cm_req_msg))
    {
      rv = false;
    }
    //Depend on the client busy and valid ts for all other use cases.
    else
    {
      if(!cm_is_nowk_event(event->message_id, cm_req_msg))
      {
        rv = cm_findnset_data_evnt_msg_lst(cm_req_msg, event, true) &&
             !client->busy;
      }
      else
        rv = !client->busy;
    }
  }
  else if(SNS_CLIENT_DELIVERY_NO_WAKEUP == cm_req_msg->req_msg.susp_config.delivery_type)
  {
    if(cm_req_msg->proc_susp_state )
    {
      rv = false;
    }
    else
    {
      if(!cm_is_nowk_event(event->message_id, cm_req_msg))
      {
        rv = cm_findnset_data_evnt_msg_lst(cm_req_msg, event, true) &&
             !client->busy;
      }
      else
        rv = !client->busy;
    }
    //Send if client not busy and precessor is not in suspend.
  }

  return rv;
}

/**
 * This functions sends all the batched data for a processor on a sensor instance,
 * across multiple requests.
 *
 * @param[i] this       The CM sensor instance.
 * @param[i] proc_type  The processor type for which the data should be sent.
 * @param[i] drop_events The low memory flag.
 *
 * @return
 * SNS_RC_FAILED - Data send failure.
 * SNS_RC_SUCCESS - Data sent.
 */
SNS_SECTION(".text.island")
static sns_rc sns_cm_send_inst_data_for_proc(
    sns_sensor_instance *this,
    sns_std_client_processor proc_type,
    bool drop_events)
{
  sns_sensor_instance *cm_sensor_inst = this;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_cm_inst_state *cm_inst = (sns_cm_inst_state*)cm_sensor_inst->state->state;
  sns_isafe_list_iter req_iter;
  sns_isafe_list_item *curr_req_list_item = NULL;

  for(curr_req_list_item = sns_isafe_list_iter_init(&req_iter,&cm_inst->req_list,true);
      NULL != curr_req_list_item;
      curr_req_list_item = sns_isafe_list_iter_advance(&req_iter))
  {
    sns_cm_request *cm_req = (sns_cm_request*)
      sns_isafe_list_item_get_data(curr_req_list_item);

    if(drop_events)
    {
      SNS_ISLAND_EXIT();
      cm_send_cm_req_data(this,cm_req,drop_events);
      cm_req->last_sent_time = sns_get_system_time();
    }
    else if (cm_req->req_msg.susp_config.client_proc_type == proc_type)
    {
      if(!cm_req->req_msg.request.batching.flush_only)
      {
         SNS_ISLAND_EXIT();
         cm_send_cm_req_data(this,cm_req,drop_events);
         cm_req->last_sent_time = sns_get_system_time();
      }
      //If flush only client but a client flush is in progress on the request
      else if (cm_req->req_msg.request.batching.flush_only &&
               (SNS_CM_CLIENT_FLUSH == cm_req->flush_type ))
      {
        SNS_ISLAND_EXIT();
        cm_send_cm_req_data(this,cm_req,drop_events);
        cm_req->last_sent_time = sns_get_system_time();
      }

    }
  }

  return rv;
}
/*===========================================================================
  Public Function Definitions
  ===========================================================================*/
void sns_cm_handle_flush_to_ddr_memory(
    sns_fw_sensor *cm_sensor)
{
  sns_sensor_instance *cm_sensor_inst = NULL;
  SNS_PROFILE(SNS_CM_FLUSH_DDR_INST_LOOP, 1, 1);
  sns_cm_state *state = (sns_cm_state*)cm_sensor->sensor.state->state;
  sns_pwr_scpm_mgr_mcps_bump_up(state->scpm_client);
  SNS_PRINTF(HIGH, cm_sensor, "MCPS bump up on flush to ddr");
  sns_isafe_list_iter instance_iter;
  sns_isafe_list_iter_init(&instance_iter, &cm_sensor->sensor_instances, true);

  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
    cm_sensor_inst = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
    if(NULL != cm_sensor_inst)
    {
      SNS_PROFILE(SNS_CM_FLUSH_DDR_REQ_LOOP, 1, 1);
      sns_isafe_list_iter req_iter;
      sns_cm_inst_state *cm_inst = (sns_cm_inst_state*)cm_sensor_inst->state->state;
      sns_isafe_list_item *curr_req_list = NULL;

      for(curr_req_list = sns_isafe_list_iter_init(&req_iter,&cm_inst->req_list, true);
          NULL != curr_req_list;
          curr_req_list = sns_isafe_list_iter_advance(&req_iter))
      {
        SNS_PROFILE(SNS_CM_FLUSH_DDR_BATCH_BIN_LOOP, 1, 1);
        sns_cm_request *cm_req = (sns_cm_request*)
          sns_isafe_list_item_get_data(curr_req_list);
        sns_isafe_list_iter batch_list_iter;
        sns_isafe_list_item *curr_batch_buff = NULL;
        for(curr_batch_buff = sns_isafe_list_iter_init(&batch_list_iter, &cm_req->batch_buff, true);
            NULL != curr_batch_buff;)
        {
          sns_cm_batch_bin *batch_bin = (sns_cm_batch_bin*)
            sns_isafe_list_item_get_data(curr_batch_buff);
          curr_batch_buff = sns_isafe_list_iter_advance(&batch_list_iter);
          sns_cm_batch_bin *batch_bin_dst = NULL;
          size_t struct_len = sizeof(sns_cm_batch_bin) - sizeof(batch_bin->event.event)
              + batch_bin->event.event_len;

          //If island item but is not last item in list, then replace
          if(sns_island_is_island_ptr((intptr_t)batch_bin) &&
             NULL != curr_batch_buff)
          {
            batch_bin_dst = (sns_cm_batch_bin*)sns_malloc(SNS_HEAP_BATCH,struct_len);

            if(NULL == batch_bin_dst)
            {
              cm_handle_low_batch_memory(cm_sensor,true);
              return;
            }
          }
          // Ensure last item is in island mode
          else if (!sns_island_is_island_ptr((intptr_t)batch_bin) &&
             NULL == curr_batch_buff)
          {
            batch_bin_dst = (sns_cm_batch_bin*)sns_malloc(SNS_HEAP_ISLAND,struct_len);

            if(NULL == batch_bin_dst)
            {
                // No island memory available. Give up
                return;
            }
          }

          // Move if required
          if (NULL != batch_bin_dst)
          {
            batch_bin_dst->event.event_len = batch_bin->event.event_len;
            batch_bin_dst->event.message_id = batch_bin->event.message_id;
            batch_bin_dst->event.timestamp = batch_bin->event.timestamp;

            if(batch_bin_dst->event.event_len > 0)
            {
               sns_memscpy((pb_byte_t*)batch_bin_dst->event.event,
                          batch_bin->event.event_len,
                          (pb_byte_t*)batch_bin->event.event,
                          batch_bin->event.event_len);
            }

            sns_isafe_list_item_init(&batch_bin_dst->item, batch_bin_dst);
            sns_isafe_list_iter_return(&batch_list_iter);
            sns_isafe_list_iter_remove(&batch_list_iter);
            sns_isafe_list_iter_insert(&batch_list_iter, &batch_bin_dst->item, false);
            sns_free(batch_bin);
            curr_batch_buff = sns_isafe_list_iter_curr(&batch_list_iter);
          }
        }
        SNS_PROFILE(SNS_CM_FLUSH_DDR_BATCH_BIN_LOOP, 1, 2);
      }
       SNS_PROFILE(SNS_CM_FLUSH_DDR_REQ_LOOP, 1, 2);
    }
    sns_isafe_list_iter_advance(&instance_iter);
  }

  SNS_PRINTF(HIGH, cm_sensor, "MCPS bump down on cm flush to ddr");
  sns_pwr_scpm_mgr_mcps_bump_down(state->scpm_client);
  state->can_flush_to_ddr = false;
  SNS_PROFILE(SNS_CM_FLUSH_DDR_INST_LOOP, 1, 2);
}

SNS_SECTION(".text.island")
sns_rc sns_cm_send_all_data_for_proc(
   sns_fw_sensor *sensor,
   sns_std_client_processor proc_type,
   bool drop_events)
{
  SNS_PROFILE(SNS_CM_SEND_ALL_DATA_FUNC, 1, 1);
  sns_sensor_instance *cm_sensor_inst = NULL;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_isafe_list_iter instance_iter;
  sns_isafe_list_iter_init(&instance_iter, &sensor->sensor_instances, true);

  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
    cm_sensor_inst = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
    if(NULL != cm_sensor_inst)
    {
      //Send all data for the instance whose proc type matches
      sns_cm_send_inst_data_for_proc(cm_sensor_inst, proc_type, drop_events);
    }
    sns_isafe_list_iter_advance(&instance_iter);
  }
  SNS_PROFILE(SNS_CM_SEND_ALL_DATA_FUNC, 1, 2);
  return rv;
}

SNS_SECTION(".text.island")
void sns_cm_handle_client_resume(
    sns_fw_sensor_instance *this)
{
  SNS_PROFILE(SNS_CM_CLNT_RESUME_FUNC, 1, 1);
  sns_cm_inst_state *cm_inst = (sns_cm_inst_state*)this->instance.state->state;
  cm_inst->busy = false;
  sns_fw_sensor *sensor = this->sensor;
  sns_isafe_list_iter req_iter;

  for(sns_isafe_list_iter_init(&req_iter,&cm_inst->req_list,true);
      NULL != sns_isafe_list_iter_curr(&req_iter);
      sns_isafe_list_iter_advance(&req_iter))
  {
    sns_cm_request *cm_req = (sns_cm_request*)
      sns_isafe_list_iter_get_curr_data(&req_iter);
    //Try to send all data for a processor only once.
    cm_proc_info *proc_info = sns_cm_get_proc_info((sns_sensor*)sensor,
        cm_req->req_msg.susp_config.client_proc_type);
    if((NULL != proc_info) && !proc_info->is_suspended)
    {
#if ENABLE_DEBUG_PRINT
       SNS_PRINTF(ERROR, sns_fw_printf, "Sending all data for proc %d",
           cm_req->req_msg.susp_config.client_proc_type);
#endif
       sns_cm_send_all_data_for_proc(this->sensor, proc_info->proc_type, false);
    }
  }
  SNS_PROFILE(SNS_CM_CLNT_RESUME_FUNC, 1, 2);
}

/*=============================================================================
  Static Sensor Function Definitions
  ===========================================================================*/
/*
 * Sends an attribute query request on an SUID.
 *
 * @param[i] cm_request The CM request structure.
 * @param[i] instance   The CM sensor instance.
 *
 */
static void cm_send_attribute_request(
    sns_cm_request *cm_request,
    sns_sensor_instance *instance)
{
  size_t encoded_len;
  uint8_t buffer[100];
  sns_std_request std_req = sns_std_request_init_default;
  sns_std_attr_req attr_req = sns_std_attr_req_init_default;

  sns_std_suid suid = {0xabababababababab, 0xabababababababab};

  attr_req.has_register_updates = false;

  if(sns_sensor_uid_compare((sns_sensor_uid*)&cm_request->req_msg.suid,
                            (sns_sensor_uid*)&suid))
  {
    SNS_INST_PRINTF(HIGH, instance, "Do not send attribute request for suid sensor");
    return;
  }

  if(cm_request->req_msg.msg_id == SNS_STD_MSGID_SNS_STD_ATTR_REQ)
  {
    SNS_INST_PRINTF(HIGH, instance, "Do not send attribute request if CM request was for an attribute");
    return;
  }

  encoded_len = pb_encode_request(buffer, sizeof(buffer), &attr_req ,sns_std_attr_req_fields , &std_req);
  if(encoded_len > 0)
  {
    sns_request request = (sns_request){
      .message_id = SNS_STD_MSGID_SNS_STD_ATTR_REQ,
      .request_len = encoded_len,
      .request = buffer };
    cm_request->data_stream->api->send_request(cm_request->data_stream, &request);
    SNS_INST_PRINTF(HIGH, instance, "Sending attribute request for sensor %x", cm_request->req_msg.suid);
  }
}

/*
 * Configures or Reconfigures the cm request structure with the new
 * client request.
 *
 * @param[i] instance   The CM sensor instance
 * @param[i] cm_request The existing client request message.
 * @param[i] req_msg    The client request message.
 * @param[i] msg_id_arr  The array of special message ids.
 * @param[i] msg_id_cnt  The msd id array size.
 *
 */
static void cm_configure_cm_request(
    sns_sensor_instance *instance,
    sns_cm_request *cm_request,
    sns_client_request_msg const *req_msg,
    uint32_t const *msg_id_arr,
    const int msg_id_cnt)
{
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_sensor *sensor = (sns_sensor*)fw_instance->sensor;
  sns_cm_state *cm_state = (sns_cm_state*)sensor->state->state;

  sns_time thrshld_bp_usec = (cm_state->registry_config.min_batch_period_thrshld_ms)*1000ULL;
  sns_time req_bp_usec = req_msg->request.batching.batch_period;

  cm_request->req_msg.suid.suid_high = req_msg->suid.suid_high;
  cm_request->req_msg.suid.suid_low = req_msg->suid.suid_low;
  cm_request->req_msg.msg_id = req_msg->msg_id;
  cm_request->req_msg.request.has_batching = req_msg->request.has_batching;
  cm_request->req_msg.request.has_is_passive = req_msg->request.has_is_passive;

  if(cm_request->nowk_msg_id_cnt != msg_id_cnt)
  {
    if(cm_request->nowk_msg_ids)
    {
      sns_free(cm_request->nowk_msg_ids);
      cm_request->nowk_msg_ids = NULL;
    }
  }
  cm_request->nowk_msg_id_cnt = msg_id_cnt;
  //We should get memory if nonwk msg id cnt is non zero
  if(cm_request->nowk_msg_id_cnt  && (NULL == cm_request->nowk_msg_ids) )
  {
    cm_request->nowk_msg_ids = (uint32_t *)sns_malloc(SNS_HEAP_ISLAND,sizeof(uint32_t)*msg_id_cnt);
    if(NULL == cm_request->nowk_msg_ids)
      cm_request->nowk_msg_ids = (uint32_t *)sns_malloc(SNS_HEAP_MAIN,sizeof(uint32_t)*msg_id_cnt);
    SNS_ASSERT(cm_request->nowk_msg_ids != NULL);
  }

  sns_memscpy(cm_request->nowk_msg_ids, sizeof(uint32_t) *msg_id_cnt,
              msg_id_arr, sizeof(uint32_t) *msg_id_cnt);


  if(cm_request->req_msg.request.has_is_passive)
  {
    cm_request->req_msg.request.is_passive = req_msg->request.is_passive;
  }
  else
    cm_request->req_msg.request.is_passive = false;

  if(req_msg->request.has_batching)
  {
    SNS_INST_PRINTF(LOW, instance, "This request has a batching spec");
    cm_request->req_msg.request.batching = req_msg->request.batching;
    if(req_bp_usec < thrshld_bp_usec)
    {
      SNS_INST_PRINTF(HIGH, instance, "Switching bp to streaming");
      cm_request->req_msg.request.batching.batch_period = 0;
    }
    if(0 == cm_request->req_msg.request.batching.flush_period)
    {
      cm_request->req_msg.request.batching.has_flush_period = true;
      if( (UINT64_MAX - cm_request->req_msg.request.batching.batch_period) >
          SNS_CM_FLUSHP_PERIOD_PADDING_TIME_USEC)
      {
        cm_request->req_msg.request.batching.flush_period =
            cm_request->req_msg.request.batching.batch_period +
		    SNS_CM_FLUSHP_PERIOD_PADDING_TIME_USEC;
      }
      else
      {
      cm_request->req_msg.request.batching.flush_period =
          cm_request->req_msg.request.batching.batch_period;
      }
    }
  }
  else
  {
    cm_request->req_msg.request.batching.batch_period = 0;
    cm_request->req_msg.request.batching.flush_period = 0;
    cm_request->req_msg.request.batching.has_flush_period = false;
    cm_request->req_msg.request.batching.has_flush_only = false;
    cm_request->req_msg.request.batching.flush_only = false;
  }
  cm_request->req_msg.susp_config = req_msg->susp_config;

  //If batch period is 0 or a flush only client then set the batch period to 0
  if(!cm_request->req_msg.request.has_batching ||
     0 == cm_request->req_msg.request.batching.batch_period ||
     (cm_request->req_msg.request.batching.has_flush_only &&
      cm_request->req_msg.request.batching.flush_only))
  {
    cm_request->timer.batch_period = 0;
  }
  else
  {
    cm_request->timer.batch_period = sns_convert_ns_to_ticks(
        cm_request->req_msg.request.batching.batch_period * 1000ULL);
  }

  cm_request->compare_period =
      cm_request->req_msg.request.batching.flush_period;

  cm_request->compare_period =
      sns_convert_ns_to_ticks(cm_request->compare_period * 1000ULL);
}
/*
 * Reconfigure the existing CM request with new client request parameters.
 *
 * @param[i] instance   The CM sensor instance
 * @param[i] req_msg    The client request message.
 * @param[i] cm_request The existing client request message.
 * @param[i] msg_id_arr  The array of special message ids.
 * @param[i] msg_id_cnt  The msd id array size.
 *
 * @returns The created request structure associated with the client request.
 */
static void cm_reconfig_request(
   sns_sensor_instance *instance,
   sns_client_request_msg const *req_msg,
   sns_cm_request *cm_request,
   uint32_t const *msg_id_arr,
   const int msg_id_cnt)
{
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_sensor *sensor = (sns_sensor*)fw_instance->sensor;

  cm_proc_info *proc_info = sns_cm_get_proc_info(sensor, req_msg->susp_config.client_proc_type);

  SNS_ASSERT(NULL != proc_info);

  cm_configure_cm_request(instance, cm_request, req_msg, msg_id_arr, msg_id_cnt);

  cm_rst_data_evnt_msg_lst(cm_request);

  if(cm_request->is_streaming_sensor)
  {
     if((SNS_STD_SENSOR_STREAM_TYPE_STREAMING == cm_request->stream_attr_value) &&
        cm_request->req_msg.request.has_batching &&
        (cm_request->req_msg.request.batching.batch_period != 0))
     {
       cm_request->is_streaming_sensor = false;
     }
  }
  else
  {
    if(SNS_STD_SENSOR_STREAM_TYPE_STREAMING == cm_request->stream_attr_value &&
       (!cm_request->req_msg.request.has_batching ||
        (cm_request->req_msg.request.has_batching &&
        (cm_request->req_msg.request.batching.batch_period == 0))))
    {
      cm_request->is_streaming_sensor = true;
    }

  }

  if(NULL != proc_info)
  {
    //Decide island state and set latency vote
    sns_cm_set_latency_vote(sensor, proc_info->proc_type);
  }
}

/*
 * Create a request and add it to the cm sensor instance request list.
 *
 * @param[i] instance    The CM sensor instance
 * @param[i] req_msg     The client request message.
 * @param[i] msg_id_arr  The array of special message ids.
 * @param[i] msg_id_cnt  The msd id array size.
 *
 * @returns The created request structure associated with the client request.
 */
static sns_cm_request *cm_add_request(
   sns_sensor_instance *instance,
   sns_client_request_msg const *req_msg,
   uint32_t const *msg_id_arr,
   const int msg_id_cnt)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  sns_service_manager *manager =
    instance->cb->get_service_manager(instance);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_sensor *sensor = (sns_sensor*)fw_instance->sensor;
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)fw_instance->sensor;

  cm_proc_info *proc_info =
      sns_cm_get_proc_info(sensor, req_msg->susp_config.client_proc_type);
  SNS_ASSERT(NULL != proc_info);

  sns_isafe_list_iter iter;
  size_t struct_len = sizeof(sns_cm_request);
  sns_cm_request *cm_request;


  SNS_INST_PRINTF(LOW, instance,
             "Creating a batch config for proct_type %d request %d client_id %d",
             req_msg->susp_config.client_proc_type, req_msg->msg_id, client->client_id);

  cm_request = (sns_cm_request*)sns_malloc(SNS_HEAP_ISLAND, struct_len);
  if(NULL == cm_request)
  {
    sns_cm_drop_oldest_batch_data(fw_sensor);
    SNS_ISLAND_EXIT();
    sns_cm_handle_flush_to_ddr_memory(fw_sensor);
    cm_request = (sns_cm_request*)sns_malloc(SNS_HEAP_MAIN, struct_len);
    SNS_ASSERT(cm_request != NULL);
    sns_isafe_list_init(&cm_request->data_evnt_msg_lst);
  }
  else
  {
    sns_isafe_list_init(&cm_request->data_evnt_msg_lst);
  }
  sns_isafe_list_init(&cm_request->batch_buff);
  //Proc is awake till we get a notification saying its suspended.
  cm_request->proc_susp_state = false;
  // The request is not for a streaming sensor till we get attribute response
  cm_request->is_streaming_sensor = false;
  cm_request->first_batch_ts = 0;
  cm_request->last_batch_ts = 0;
  cm_request->flush_type = SNS_CM_NO_FLUSH;
  cm_request->active_flush_req_count = 0;
  cm_request->last_sent_time = 0;
  cm_request->sensor_payload = NULL;
  cm_request->sensor_payload_len = 0;
  cm_request->stream_attr_value = SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT;

  cm_configure_cm_request(instance, cm_request, req_msg,
                          msg_id_arr, msg_id_cnt);

  if(SNS_CLIENT_DELIVERY_NO_WAKEUP == cm_request->req_msg.susp_config.delivery_type)
  {
    proc_info->non_wk_cnt++;
  }
  //Create the data stream for the request
  stream_service->api->create_sensor_instance_stream(stream_service,
       instance, *((sns_sensor_uid*)&req_msg->suid), &cm_request->data_stream);

  if(NULL == cm_request->data_stream)
  {
    SNS_INST_PRINTF(ERROR, instance, "Stream creation failed");
    sns_free(cm_request);
    cm_request = NULL;
  }
  else
  {
    cm_send_attribute_request(cm_request, instance);
    sns_isafe_list_iter_init(&iter, &client->req_list, false);
    sns_isafe_list_item_init(&cm_request->item, cm_request);
    sns_isafe_list_iter_insert(&iter, &cm_request->item, true);
  }

  return cm_request;
}

/**
 * This function evaluates the batching configuration for a request.
 *
 * @param[i] cm_req_msg  The client manager request structure associated
 *                       with the original request.
 *
 * @return True - if event needs to be batched
 *         False - If request does not have a batch spec.
 */
SNS_SECTION(".text.island")
static bool cm_need_batching (
   sns_cm_request *cm_req_msg)
{
  bool rv = false;

  if(NULL != cm_req_msg)
  {
    if(cm_req_msg->req_msg.request.has_batching )
    {
      //Non-zero batch period
      if(cm_req_msg->req_msg.request.batching.batch_period != 0)
        rv = true;
      //zero batch period and flush only request
      else if(cm_req_msg->req_msg.request.batching.has_flush_only &&
              cm_req_msg->req_msg.request.batching.flush_only &&
              cm_req_msg->req_msg.request.batching.has_flush_period &&
              cm_req_msg->req_msg.request.batching.flush_period != 0)
        rv = true;
      //If zero batch period and not flush only.
    }
  }
  return rv;
}

/**
 * Sends an indication containing the specified event to the client of this
 * instance.
 *
 * @param[i] this       The current client instance
 * @param[i] event      Event received which needs to be sent
 * @param[i] src_sensor Sensor for the source of the data.
 * @param[i] cm_request The CM request structure.
 */
SNS_SECTION(".text.island")
static void cm_send_ind (
   sns_sensor_instance *this,
   sns_sensor_event *event,
   sns_fw_sensor *src_sensor,
   sns_cm_request *cm_request)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;

  if(!client->busy)
  {
    SNS_ISLAND_EXIT();
    qmi_csi_error err = cm_encode_and_send_single_ind(this, event, src_sensor);
    //Store the event if connection busy is hit
    if(QMI_CSI_CONN_BUSY == err)
    {
      sns_cm_store_event(event, cm_request, this);
    }
    cm_request->last_sent_time = sns_get_system_time();
  }
  else
  {
    sns_cm_store_event(event, cm_request, this);
  }
}

SNS_SECTION(".text.island")
static bool sns_cm_decode_attr_value(
    pb_istream_t *stream,
    const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  sns_cm_attrib *cm_attrib = *arg;

  if(cm_attrib != NULL)
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    pb_buffer_arg str_data = (pb_buffer_arg){.buf = NULL, .buf_len = 0 };
    value.str.funcs.decode = &pb_decode_string_cb;
    value.str.arg = &str_data;
    sns_cm_request * cm_req = cm_attrib->cm_request;

    value.subtype.values.funcs.decode = &sns_cm_decode_attr_value;
    value.subtype.values.arg = (void *)cm_attrib;

    if (!pb_decode(stream, sns_std_attr_value_data_fields, &value))
    {
      SNS_INST_PRINTF(ERROR, cm_attrib->instance, "Error decoding attribute");
      return false;
    }

    if(SNS_STD_SENSOR_ATTRID_STREAM_TYPE == cm_attrib->attr_id)
    {
      if (value.has_sint)
      {
        SNS_INST_PRINTF(HIGH, cm_attrib->instance,"Decoding %d with value: %d",
            cm_attrib->attr_id, value.sint);
        SNS_ASSERT(NULL != cm_req);
        cm_attrib->attr_value = value.sint;
        cm_req->stream_attr_value = cm_attrib->attr_value;
        if(SNS_STD_SENSOR_STREAM_TYPE_STREAMING == cm_attrib->attr_value)
        {
          if(!cm_req->req_msg.request.has_batching ||
             (cm_req->req_msg.request.has_batching &&
              (cm_req->req_msg.request.batching.batch_period == 0)))
          {
             cm_req->is_streaming_sensor = true;
          }
          //If batching client is present
          else
          {
            cm_req->is_streaming_sensor = false;
          }
          SNS_INST_PRINTF(HIGH, cm_attrib->instance, "Request %0x streaming %d",
          cm_attrib->cm_request,cm_attrib->cm_request->is_streaming_sensor);
        }
        else
        {
           cm_req->is_streaming_sensor = false;
        }
      }
    }
  }
  return true;
}
SNS_SECTION(".text.island")
static bool sns_cm_decode_attr(
    pb_istream_t *stream,
    const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  sns_cm_attrib *cm_attrib = *arg;
  pb_istream_t stream_cpy = *stream;

  sns_std_attr attribute = sns_std_attr_init_default;
  attribute.value.values.funcs.decode = NULL;
  attribute.value.values.arg = arg;


  if(!pb_decode(stream, sns_std_attr_fields, &attribute))
  {
    SNS_INST_PRINTF(ERROR, cm_attrib->instance, "Error decoding attribute");
    return false;
  }
  cm_attrib->attr_id = attribute.attr_id;
  attribute.attr_id = -1;
  attribute.value.values.funcs.decode = &sns_cm_decode_attr_value;
  attribute.value.values.arg = (void *)cm_attrib;

  if(!pb_decode(&stream_cpy, sns_std_attr_fields, &attribute))
  {
    SNS_INST_PRINTF(ERROR, cm_attrib->instance, "Error decoding attribute");
    return false;
  }
  if (SNS_STD_SENSOR_ATTRID_STREAM_TYPE == cm_attrib->attr_id)
  {
     SNS_INST_PRINTF(HIGH, cm_attrib->instance, "Decoded attribute value: %d", cm_attrib->attr_value);
  }
  return true;
}

/**
 * Handles the attribute response for the specified client
 *
 * @param[i] this       The current client instance
 * @param[i] event      Event received to be sent
 * @param[i] src_sensor Pointer to source sensor
 * @param[i] cm_sensor  CM Sensor .
 * @param[i] cm_request The CM request structure.
 *
 * @return
 * SNS_RC_INVALID_STATE - Catastrophic error occurred, restart
 * SNS_RC_SUCCESS
 */
static sns_rc cm_handle_attr (
   sns_sensor_instance *this,
   sns_sensor_event    *event,
   sns_fw_sensor       *cm_sensor,
   sns_cm_request      *cm_request)
{
  sns_rc rc = SNS_RC_SUCCESS;
  cm_proc_info* cm_apps_proc_info =
  sns_cm_get_proc_info((sns_sensor *)cm_sensor,
       cm_request->req_msg.susp_config.client_proc_type);

  if(NULL == cm_apps_proc_info)
  {
    SNS_INST_PRINTF(ERROR, this, "Proc info is null for %d",
        cm_request->req_msg.susp_config.client_proc_type);
    return SNS_RC_FAILED;
  }
  sns_cm_attrib cm_attrib;
  pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                                         event->event_len);

  cm_attrib.instance = this;
  cm_attrib.cm_request = cm_request;
  cm_attrib.attr_value = false;
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;
  attr_event.attributes.funcs.decode = &sns_cm_decode_attr;
  attr_event.attributes.arg = (void *)&cm_attrib;

  if (!pb_decode(&stream, sns_std_attr_event_fields, &attr_event))
  {
    SNS_INST_PRINTF(ERROR, this, "Error decoding Attr Event");
    rc = SNS_RC_INVALID_STATE;
  }
  else
  {
    SNS_INST_PRINTF(HIGH, this, "Stream Type is:%d", cm_attrib.attr_value);
    /* Walk through the list to find the client for this sensor */
  }

  //Decide island state and set latency vote
  sns_cm_set_latency_vote((sns_sensor *)cm_sensor, cm_apps_proc_info->proc_type);

  return rc;
}

/**
 * Handles an indication containing the specified event to the client of this
 * instance.
 *
 * @param[i] this       The current client instance
 * @param[i] event      Event received to be sent
 * @param[i] src_sensor Pointer to source sensor
 * @param[i] cm_sensor  CM Sensor .
 * @param[i] cm_request The CM request structure.
 *
 * @return
 * SNS_RC_INVALID_STATE - Catastrophic error occurred, restart
 * SNS_RC_SUCCESS
 */
SNS_SECTION(".text.island")
static sns_rc cm_handle_ind (
   sns_sensor_instance *this,
   sns_sensor_event    *event,
   sns_fw_sensor       *src_sensor,
   sns_fw_sensor       *cm_sensor,
   sns_cm_request      *cm_request)
{
  SNS_PROFILE(SNS_CM_HANDLE_IND_FUNC, 1, 1);
  sns_rc rv = SNS_RC_SUCCESS;
  sns_std_client_processor proc_type;
  bool send_flag =  sns_cm_can_send(this, event, cm_request);
  bool batch_flag = cm_need_batching (cm_request);

#if ENABLE_DEBUG_PRINT
  SNS_INST_PRINTF(LOW, this, "Event id %d send_flag %d batch_flag %d req %0x",
      event->message_id, send_flag, batch_flag, cm_request);
#endif
  if(send_flag)
  {
    proc_type = cm_request->req_msg.susp_config.client_proc_type;
    //If a batch request with non zero flush period
    if(batch_flag && (0 != cm_request->req_msg.request.batching.flush_period))
    {
      if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT != event->message_id)
      {
        rv = sns_cm_store_event(event,cm_request,this);
      }
      //If either a flush from client or a batch timer flush,
      //data can be delivered if awake.
      if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == event->message_id)
      {
        if(SNS_CM_CLIENT_FLUSH == cm_request->flush_type)
        {
          for(; (cm_request->active_flush_req_count > 0) ;
                cm_request->active_flush_req_count--)
          {
              rv = sns_cm_store_event(event,cm_request,this);
          }
#if ENABLE_DEBUG_PRINT
		  SNS_INST_PRINTF(HIGH, this, "Client flush complete for req %0x active_flush_req_count %d",
		                  cm_request, cm_request->active_flush_req_count);
#endif
          sns_cm_send_all_data_for_proc(cm_sensor, proc_type, false);
        }
        else if(SNS_CM_BATCH_FLUSH == cm_request->flush_type)
        {
          rv = sns_cm_store_event(event,cm_request,this);
          sns_cm_send_all_data_for_proc(cm_sensor, proc_type, false);
        }
      }
    }
    // Pure streaming
    else if(!batch_flag)
    {
      sns_cm_send_all_data_for_proc(cm_sensor, proc_type, false);
      //This check is for non-wakeup streaming clients being flushed on apps wakeup
      if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == event->message_id)
      {
        if(SNS_CM_CLIENT_FLUSH == cm_request->flush_type)
        {
          for(; (cm_request->active_flush_req_count > 0) ;
                cm_request->active_flush_req_count--)
          {
            cm_send_ind(this, event, src_sensor, cm_request);
          }
        }
#if ENABLE_DEBUG_PRINT
	    SNS_INST_PRINTF(HIGH, this, "Client flush complete for req %0x active_flush_req_count %d",
		                cm_request, cm_request->active_flush_req_count);
#endif
      }
      else
      {
        cm_send_ind(this, event, src_sensor, cm_request);
      }
    }
  }
  //Hi-fi requirement
  else
  {
    if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT != event->message_id)
    {
      rv = sns_cm_store_event(event,cm_request,this);
    }
    //If either a flush from client or a batch timer flush,
    if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == event->message_id)
    {
      if(SNS_CM_CLIENT_FLUSH == cm_request->flush_type)
      {
        for(; (cm_request->active_flush_req_count > 0) ;
              cm_request->active_flush_req_count--)
        {
          rv = sns_cm_store_event(event,cm_request,this);
        }
#if ENABLE_DEBUG_PRINT
	    SNS_INST_PRINTF(HIGH, this, "Client flush complete for req %0x active_flush_req_count %d",
		               cm_request, cm_request->active_flush_req_count);
#endif
      }
      else if(SNS_CM_BATCH_FLUSH == cm_request->flush_type)
      {
        rv = sns_cm_store_event(event,cm_request,this);
      }
    }
  }
  SNS_PROFILE(SNS_CM_HANDLE_IND_FUNC, 1, 2);
  return rv;
}

/**
 * This function add the max batch flag for a request which contains a batching spec
 * and forwards the request to the sensor.
 * Else it just forwards the request to the sensor as is.
 *
 * @param[i] data_stream   The data stream associated with a sensor from CM.
 * @param[i] max_batch     Flag which tells whether to switch into or out of max batch.
 * @param[i] cm_sensor     The CM sensor pointer.
 */
static void cm_modify_batch_req(
    sns_cm_request *cm_request,
    sns_request *req,
    sns_data_stream *data_stream,
    sns_sensor *cm_sensor)
{
  static uint8_t buffer[SNS_CLIENT_REQ_LEN_MAX_V01 + sns_std_request_batch_spec_size];
  sns_cm_state *cm_state = (sns_cm_state*)cm_sensor->state->state;

  if(NULL != req)
  {
    sns_std_request decoded_req = sns_std_request_init_default;

    pb_buffer_arg arg = { NULL, 0 };
    decoded_req.payload = (struct pb_callback_s)
        { .funcs.decode = &pb_decode_string_cb, .arg = &arg };

    pb_istream_t stream = pb_istream_from_buffer(req->request, req->request_len);

    if(pb_decode(&stream, sns_std_request_fields, &decoded_req))
    {
      if(decoded_req.has_batching &&
         (0 != decoded_req.batching.batch_period) &&
         (cm_state->registry_config.max_batch_disabled == false) &&
         !cm_request->req_msg.request.is_passive &&
         (SNS_CLIENT_DELIVERY_NO_WAKEUP ==
             cm_request->req_msg.susp_config.delivery_type))
      {
        decoded_req.batching.has_max_batch = true;
        decoded_req.batching.max_batch = true;
      }
      decoded_req.payload.funcs.encode = &pb_encode_string_cb;
      decoded_req.payload.arg = &arg;
      pb_ostream_t stream_o = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));

      //If payload was empty then reset the payload encode to prevent encoding null bytes.
      if(0 == arg.buf_len)
      {
        arg.buf = NULL;
        arg.buf_len = 0;
      }
      else
      {
        //Re-modification of the request, prevents leaks
        if(NULL != cm_request->sensor_payload)
        {
          sns_free(cm_request->sensor_payload);
          cm_request->sensor_payload_len = 0;
        }
        //Store the payload in decoded string bytes form
        cm_request->sensor_payload=(void *)sns_malloc(SNS_HEAP_MAIN,arg.buf_len);
        SNS_ASSERT(NULL != cm_request->sensor_payload);
        sns_memscpy (cm_request->sensor_payload, arg.buf_len, arg.buf, arg.buf_len);
        cm_request->sensor_payload_len = arg.buf_len;
      }

      if(!pb_encode(&stream_o, sns_std_request_fields, &decoded_req))
      {
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error Encoding request: %s", PB_GET_ERROR(&stream_o));
      }
      else
      {
        size_t encoded_len = stream_o.bytes_written;
        //Send the modified request
        if(0 < encoded_len)
        {
          sns_request request = (sns_request){
                                .request_len = encoded_len,
                                .request = buffer,
                                .message_id = req->message_id };
          data_stream->api->send_request(data_stream, &request);
        }
      }
    }
    else
    {
      SNS_PRINTF(ERROR, cm_sensor, "This request does not have batch period, hence ignoring");
    }
  }
}

/**
 * Repackage and forward a request along to its Sensor destination.
 *
 * @param[i] instance   The current client instance
 * @param[i] client_id  The client id associated with the instance.
 * @param[i] qmi_req    Incoming QMI Request
 * @param[i] msg_id     Sensor-specific message ID
 * @param[i] stream     Stream on which to forward the request
 * @param[i] cm_request The CM request structure.
 *
 * @return
 * SNS_RC_FAILED - If forwarding the request failed.
 * SNS_RC_SUCCESS
 */
static sns_rc cm_forward_request(
  sns_sensor_instance *instance,
  uint64_t client_id,
  sns_client_req_msg_v01 *qmi_req,
  uint32_t msg_id,
  sns_data_stream *stream,
  sns_cm_request *cm_request)
{
  pb_istream_t istream;
  sns_rc rc = SNS_RC_SUCCESS;
  bool eof = false;
  uint32_t tag;
  pb_wire_type_t wire_type;
  sns_sensor *cm_sensor = (sns_sensor*)((sns_fw_sensor_instance*)instance)->sensor;
  sns_sensor *data_src_sensor = (sns_sensor*)&((sns_fw_data_stream*)
            cm_request->data_stream)->data_source->sensor;

  istream = pb_istream_from_buffer(qmi_req->payload, qmi_req->payload_len);

  sns_fw_log_client_api_req(instance, data_src_sensor, cm_get_sensor_uid(NULL),
                            client_id, qmi_req->payload, qmi_req->payload_len);
  //track the external flush_request
  if(SNS_STD_MSGID_SNS_STD_FLUSH_REQ == msg_id)
  {
    SNS_INST_PRINTF(LOW, instance, "Got a flush request , id %d", msg_id);
    pb_istream_t istream;
    sns_client_request_msg req_msg = sns_client_request_msg_init_default;

    istream = pb_istream_from_buffer(qmi_req->payload, qmi_req->payload_len);
    if(!pb_decode(&istream, sns_client_request_msg_fields, &req_msg))
    {
      SNS_INST_PRINTF(ERROR, instance, "Decoding failed %s", PB_GET_ERROR(&istream));
    }
    else
    {
      rc = sns_cm_handle_client_flush_request(instance, cm_request, stream);
    }
  }
  else
  {
    while(pb_decode_tag(&istream, &wire_type, &tag, &eof) && !eof)
    {
      if(PB_WT_STRING == wire_type && sns_client_request_msg_request_tag == tag)
      {
        pb_istream_t sub_stream;
        if(pb_make_string_substream(&istream, &sub_stream))
        {
          sns_request request = (sns_request)
            { .message_id = msg_id, .request = sub_stream.state,
              .request_len = sub_stream.bytes_left };

          cm_modify_batch_req(cm_request, &request, stream, cm_sensor);
        }
        break;
      }
      pb_skip_field(&istream, wire_type);
    }
  }
  return rc;
}

/**
 * Handles the client requested disable request by cleaning the batch buffer and
 * and the data stream associated with the request.
 *
 * @param[i] instance   The current client instance
 * @param[i] req_msg    The client request message.
 */
static void cm_handle_client_disable_request (
   sns_sensor_instance *instance,
   sns_client_request_msg *req_msg,
   sns_client_req_msg_v01 *qmi_req)
{
  SNS_PROFILE(SNS_CM_HANDLE_CLNT_DISABLE_FUNC, 1, 1);
  sns_isafe_list_iter req_list_iter;
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  sns_fw_sensor_instance *fw_cm_inst = (sns_fw_sensor_instance *)instance;
  sns_fw_sensor *fw_cm_sensor = (sns_fw_sensor *)fw_cm_inst->sensor;

  sns_service_manager *manager =
   instance->cb->get_service_manager(instance);
  sns_stream_service *stream_service =
   (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

#if ENABLE_DEBUG_PRINT
  SNS_INST_PRINTF(ERROR, instance,
      "Got a disable request for client id %d proc_type \'%d\'",
      client->client_id, req_msg->susp_config.client_proc_type);
#endif

  for(sns_isafe_list_iter_init(&req_list_iter, &client->req_list, true);
      NULL != sns_isafe_list_iter_curr(&req_list_iter);
      sns_isafe_list_iter_advance(&req_list_iter))
  {
    sns_cm_request *cm_req_msg = (sns_cm_request*)
      sns_isafe_list_iter_get_curr_data(&req_list_iter);

    if(sns_sensor_uid_compare((sns_sensor_uid*)&cm_req_msg->req_msg.suid,
                              (sns_sensor_uid*)&req_msg->suid))
    {
      sns_cm_send_all_data_for_proc(fw_cm_sensor,
         cm_req_msg->req_msg.susp_config.client_proc_type, false);

      cm_proc_info *proc_info = sns_cm_get_proc_info((sns_sensor*)fw_cm_sensor,
          cm_req_msg->req_msg.susp_config.client_proc_type);

      SNS_ASSERT(NULL != proc_info);

      if(cm_req_msg->flush_type != SNS_CM_NO_FLUSH)
      {
        sns_cm_bump_down_on_flush_compl(instance);
      }

      if(SNS_CLIENT_DELIVERY_NO_WAKEUP == cm_req_msg->req_msg.susp_config.delivery_type)
      {
        if(0 < proc_info->non_wk_cnt)
        {
          proc_info->non_wk_cnt--;
        }
      }

      if(NULL != cm_req_msg->data_stream)
      {
        sns_fw_sensor *data_src = ((sns_fw_data_stream*)
          cm_req_msg->data_stream)->data_source;
        sns_sensor *data_src_sensor = &data_src->sensor;

        sns_fw_log_client_api_req(instance, data_src_sensor, cm_get_sensor_uid(NULL),
            client->client_id, qmi_req->payload, qmi_req->payload_len);
        sns_fw_log_client_api_resp(instance, data_src_sensor, cm_get_sensor_uid(NULL),
            client->client_id, SNS_RC_SUCCESS);
      }
      else
      {
        SNS_INST_PRINTF(ERROR, instance, "Data stream should not be null for req %0x",cm_req_msg);
      }
      cm_clear_batch_buffer_list(cm_req_msg);
      cm_clear_data_evnt_msg_lst(cm_req_msg);
      sns_isafe_list_iter_remove(&req_list_iter);

      if(NULL != cm_req_msg->data_stream)
      {
        stream_service->api->remove_stream(stream_service, cm_req_msg->data_stream);
      }
      if(NULL != cm_req_msg->sensor_payload)
      {
        sns_free(cm_req_msg->sensor_payload);
        cm_req_msg->sensor_payload = NULL;
      }
      sns_cm_set_latency_vote(&((sns_fw_sensor_instance*)instance)->sensor->sensor,
                             proc_info->proc_type);
      if(cm_req_msg->nowk_msg_ids)
      {
        sns_free(cm_req_msg->nowk_msg_ids);
        cm_req_msg->nowk_msg_ids = NULL;
      }
      sns_free(cm_req_msg);
      break;
    }
  }
  SNS_PROFILE(SNS_CM_HANDLE_CLNT_DISABLE_FUNC, 1, 2);
}

/**
 * Handles the client disconnect request.
 * Clears out all the requests on the client
 * manager instance and the associated batch buffers.
 *
 * @param[i] instance   The client instance which is being removed.
 */
static void cm_handle_client_disconnect (
   sns_sensor_instance *instance)
{
  SNS_PROFILE(SNS_CM_HANDLE_CLNT_DISCONNECT_FUNC, 1, 1);
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;

  sns_fw_sensor_instance *fw_cm_inst = (sns_fw_sensor_instance *)instance;
  sns_fw_sensor *fw_cm_sensor = (sns_fw_sensor *)fw_cm_inst->sensor;

  sns_list d_proc_t_list;
  sns_list_init(&d_proc_t_list);
  sns_list_iter d_list_iter;
  cm_proc_info *d_proc_info = NULL;
  uint32_t proc_t_map = 0;

  sns_isafe_list_iter req_list_iter;
#if ENABLE_DEBUG_PRINT
  SNS_PRINTF(ERROR, sns_fw_printf, "Got a disconnect");
#endif
  for(sns_isafe_list_iter_init(&req_list_iter, &client->req_list, true);
      NULL != sns_isafe_list_iter_curr(&req_list_iter);)
  {
    sns_cm_request *cm_req_msg = (sns_cm_request*)
        sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&req_list_iter));

    cm_proc_info *proc_info = sns_cm_get_proc_info((sns_sensor*)fw_cm_sensor,
        cm_req_msg->req_msg.susp_config.client_proc_type);


    SNS_ASSERT(NULL != proc_info);

    //Re-eval the latency vote if a new processor type is found among requests
    //being removed.
    //Only create 1 entry for a unique processor type
    if( !(proc_t_map & (1 << cm_req_msg->req_msg.susp_config.client_proc_type)))
    {
       d_proc_info =  (cm_proc_info*)sns_malloc(SNS_HEAP_MAIN, sizeof(cm_proc_info));
       SNS_ASSERT(NULL != d_proc_info);
       d_proc_info->proc_type = cm_req_msg->req_msg.susp_config.client_proc_type;
       sns_list_iter iter;
       sns_list_item_init(&d_proc_info->item,d_proc_info);
       sns_list_iter_init(&iter, &d_proc_t_list, false);
       sns_list_iter_insert(&iter, &d_proc_info->item, true);
       proc_t_map = proc_t_map | ( 1<< cm_req_msg->req_msg.susp_config.client_proc_type );
    }
    if(SNS_CLIENT_DELIVERY_NO_WAKEUP == cm_req_msg->req_msg.susp_config.delivery_type)
    {
      if(0 < proc_info->non_wk_cnt)
      {
        proc_info->non_wk_cnt--;
      }
    }
    cm_clear_batch_buffer_list(cm_req_msg);
    cm_clear_data_evnt_msg_lst(cm_req_msg);

    if(cm_req_msg->flush_type != SNS_CM_NO_FLUSH)
    {
      sns_cm_bump_down_on_flush_compl(instance);
    }
    if((NULL != cm_req_msg->data_stream))
    {
      sns_service_manager *manager =
        instance->cb->get_service_manager(instance);
      sns_stream_service *stream_service =
        (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  
      stream_service->api->remove_stream(stream_service, cm_req_msg->data_stream);
      cm_req_msg->data_stream = NULL;
    }

    if(NULL != cm_req_msg->sensor_payload)
    {
      sns_free(cm_req_msg->sensor_payload);
      cm_req_msg->sensor_payload = NULL;
    }
    if(cm_req_msg->nowk_msg_ids)
    {
      sns_free(cm_req_msg->nowk_msg_ids);
      cm_req_msg->nowk_msg_ids = NULL;
    }
    sns_free(cm_req_msg);
  }
  for(sns_list_iter_init(&d_list_iter, &d_proc_t_list, true);
      NULL != sns_list_iter_curr(&d_list_iter);)
  {
	d_proc_info = (cm_proc_info*)
	      sns_list_item_get_data(sns_list_iter_remove(&d_list_iter));
	sns_cm_set_latency_vote( (sns_sensor *)fw_cm_sensor, d_proc_info->proc_type);
	sns_free(d_proc_info);
	d_proc_info = NULL;
  }

  SNS_PROFILE(SNS_CM_HANDLE_CLNT_DISCONNECT_FUNC, 1, 2);
}

/**
 * Handles the client instance deinit.
 * Clears out all the data streams associated with all the requests.
 * Then calls the disconnect handler which clears all local requests and
 * batch buffers
 *
 * @param[i] instance   The client instance which is being removed.
 */
static sns_rc cm_process_deinit(
    sns_sensor_instance *instance)
{
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  sns_isafe_list_iter req_list_iter;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_fw_sensor_instance *cm_sensor_inst = (sns_fw_sensor_instance *)instance;
  sns_fw_sensor *cm_sensor = cm_sensor_inst->sensor;

  //Remove all the streams
  for(sns_isafe_list_iter_init(&req_list_iter, &client->req_list, true);
      NULL != sns_isafe_list_iter_curr(&req_list_iter);
      sns_isafe_list_iter_advance(&req_list_iter))
  {
   sns_cm_request *cm_req_msg = (sns_cm_request*)
     sns_isafe_list_iter_get_curr_data(&req_list_iter);
    if(NULL != cm_req_msg)
    {
      sns_cm_reval_and_register_timers((sns_sensor *)cm_sensor,
      		  cm_req_msg->req_msg.susp_config.client_proc_type, NULL);
    }
  }

  cm_handle_client_disconnect(instance);
  return rv;
}

/**
 * Process all pending events for this client.  Form and send indications,
 * and/or batch data as appropriate.
 *
 * @see sns_sensor_instance_api::event_notify
 */
SNS_SECTION(".text.island")
static sns_rc cm_instance_notify_event(
   sns_sensor_instance *const instance)
{
  SNS_PROFILE(SNS_CM_NOTIFY_FUNC, 1, 1);
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  sns_isafe_list_iter iter;
  sns_rc rc = SNS_RC_SUCCESS;
  sns_isafe_list_item *curr_req_list = NULL;
  uint32_t handled_event_cnt = 0;

  for(curr_req_list = sns_isafe_list_iter_init(&iter, &client->req_list, true);
      NULL != curr_req_list;
      curr_req_list = sns_isafe_list_iter_advance(&iter))
  {
    sns_cm_request *cm_request = (sns_cm_request*)
      sns_isafe_list_item_get_data(curr_req_list);

    sns_data_stream *data_stream = cm_request->data_stream;
    sns_fw_data_stream *fw_stream = (sns_fw_data_stream *)data_stream;
    sns_fw_sensor *src_sensor = fw_stream->data_source;
    sns_fw_sensor_instance *cm_sensor_inst = fw_stream->dst_instance;
    sns_fw_sensor *cm_sensor = cm_sensor_inst->sensor;
    sns_sensor_event *event = data_stream->api->peek_input(data_stream);

    while(NULL != event)
    {
      if(SNS_STD_MSGID_SNS_STD_ATTR_EVENT == event->message_id)
      {
        SNS_ISLAND_EXIT();
        rc = cm_handle_attr(instance, event, cm_sensor, cm_request);

        //Attribute requested by client as well
        //Send the indication out.
        if(SNS_STD_MSGID_SNS_STD_ATTR_REQ == cm_request->req_msg.msg_id)
        {
          rc = cm_handle_ind(instance, event, src_sensor, cm_sensor, cm_request);
        }
      }
      else
      {
        rc = cm_handle_ind(instance, event, src_sensor, cm_sensor, cm_request);
        if(SNS_STD_MSGID_SNS_STD_FLUSH_EVENT == event->message_id)
        {
          if(cm_request->flush_type != SNS_CM_NO_FLUSH)
          {
            sns_cm_bump_down_on_flush_compl(instance);
          }
          //Reset the flush_in_progress
          cm_request->flush_type = SNS_CM_NO_FLUSH;
        }
      }

      //Get the events out of stream irrespective of handle indication erroring out
      event = data_stream->api->get_next_input(data_stream);
      handled_event_cnt++;
      if (handled_event_cnt > MAX_NUM_OF_EVENTS_PROCESSED)
      {
        sns_event_service_add_pending_event(fw_stream);
        break;
      }
    }

    if (handled_event_cnt > MAX_NUM_OF_EVENTS_PROCESSED) break;
  }

  SNS_PROFILE(SNS_CM_NOTIFY_FUNC, 1, 2);
  return rc;
}

/* See sns_sensor_instance_api::init */
static sns_rc cm_instance_init(
   sns_sensor_instance *const this,
   sns_sensor_state const *sensor_state)
{
  UNUSED_VAR(sensor_state);
  sns_cm_inst_state *cm_inst_state = (sns_cm_inst_state*)this->state->state;

  cm_inst_state->busy = false;
  cm_inst_state->use_jumbo_report = false;
  sns_isafe_list_init(&cm_inst_state->req_list);

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc cm_instance_set_client_config(
   sns_sensor_instance *this,
   sns_request const *client_request)
{
  SNS_PROFILE(SNS_CM_INST_SET_CLNT_CFG_FUNC, 1, 1);
  sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;
  sns_client_req_msg_v01 *qmi_req = (sns_client_req_msg_v01*)client_request->request;
  sns_rc rc = SNS_RC_SUCCESS;
  sns_client_request_msg req_msg = sns_client_request_msg_init_default;
  pb_istream_t istream;
  uint32_t msg_ids[SNS_CM_REQ_CRIT_MSG_ID_MAX] = { 0 };
  uint8_t msg_id_index = 0;
  sns_fw_sensor_instance *cm_sensor_inst = (sns_fw_sensor_instance *)this;
  sns_fw_sensor *cm_sensor = cm_sensor_inst->sensor;
  sns_cm_request *cm_request = NULL;
  pb_float_arr_arg nowk_msg_ids = {
    .arr = (float *)msg_ids,
    .arr_len = ARR_SIZE(msg_ids),
    .arr_index = &msg_id_index
  };

  // Setup call backs to decode the array of floats
  req_msg.susp_config.nowakeup_msg_ids = (struct pb_callback_s){
    .funcs.decode = &pb_decode_float_arr_cb, .arg = &nowk_msg_ids
  };

    istream = pb_istream_from_buffer(qmi_req->payload, qmi_req->payload_len);
  if(!pb_decode(&istream, sns_client_request_msg_fields, &req_msg))
    {
    SNS_INST_PRINTF(ERROR, this, "Decoding failed");
    }
  else
  {
    SNS_INST_PRINTF(LOW, this, "Received request; struct len %i",
        qmi_req->payload_len);

    SNS_INST_PRINTF(LOW, this, "Received request with crit msg id count %i",
        msg_id_index);
    rc = cm_validate_request(&req_msg);
    if(SNS_RC_SUCCESS == rc)
    {
    if(SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ == req_msg.msg_id)
    {
        SNS_INST_PRINTF(LOW, this,
            "Disable request received for suid_high[low] %x suid_high[high] %x",
            SNS_CM_FETCH_LOW_32(req_msg.suid.suid_high),
            SNS_CM_FETCH_HI_32(req_msg.suid.suid_high));
      cm_handle_client_disable_request(this, &req_msg, qmi_req);
    }
      else
      {
    //Store the batch and suspend config
    sns_cm_inst_state *client = (sns_cm_inst_state*)this->state->state;
        cm_request = cm_find_cm_request(this, &req_msg.suid);
    sns_sensor *data_src_sensor;

        SNS_INST_PRINTF(LOW, this,
            "create_sensor_instance_stream for suid_high[low] %x suid_high[high] %x suid_low[low] %x suid_low[high] %x",
            SNS_CM_FETCH_LOW_32(req_msg.suid.suid_high),
            SNS_CM_FETCH_HI_32(req_msg.suid.suid_high),
            SNS_CM_FETCH_LOW_32(req_msg.suid.suid_low),
            SNS_CM_FETCH_HI_32(req_msg.suid.suid_low));

        client->use_jumbo_report = qmi_req->use_jumbo_report_valid ?
          qmi_req->use_jumbo_report : false;

    if(NULL == cm_request)
    {
      cm_request = cm_add_request(this, &req_msg , msg_ids, msg_id_index);
    }
    else
    {
      SNS_INST_PRINTF(LOW, this, "Found a CM request %p reconfiguring", cm_request);
      if(SNS_STD_MSGID_SNS_STD_FLUSH_REQ != req_msg.msg_id)
      {
        sns_sensor_event    event;
        event.event_len =0;
        event.message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
        event.timestamp = sns_get_system_time();
 
        //Stop the active flush so that subsequent flushes can be triggered.
        if(SNS_CM_CLIENT_FLUSH == cm_request->flush_type)
        {
          for(; (cm_request->active_flush_req_count > 0) ;
                cm_request->active_flush_req_count--)
          {
             sns_cm_store_event(&event,cm_request,this);
          }
#if ENABLE_DEBUG_PRINT
          SNS_INST_PRINTF( HIGH, this, "Client flush complete for req %0x active_flush_req_count %d",
                           cm_request, cm_request->active_flush_req_count);
#endif
              sns_cm_send_all_data_for_proc(cm_sensor,
              cm_request->req_msg.susp_config.client_proc_type, false);
              sns_cm_bump_down_on_flush_compl(this);
        }
        else if(SNS_CM_BATCH_FLUSH == cm_request->flush_type)
        {
          sns_cm_store_event(&event,cm_request,this);
              sns_cm_send_all_data_for_proc(cm_sensor,
              cm_request->req_msg.susp_config.client_proc_type, false);
              sns_cm_bump_down_on_flush_compl(this);
            }
            else
            {
              sns_cm_send_all_data_for_proc(cm_sensor,
                  cm_request->req_msg.susp_config.client_proc_type, false);
        }
        cm_request->flush_type = SNS_CM_NO_FLUSH;
        cm_request->active_flush_req_count = 0;
        cm_reconfig_request(this, &req_msg, cm_request, msg_ids, msg_id_index);
      }
    }
        // PEND: Determine which result values to return in the resp message
        if (NULL != cm_request)
        {
          cm_validate_request(&cm_request->req_msg);
    rc = cm_forward_request( this, client->client_id, qmi_req,
                             req_msg.msg_id, cm_request->data_stream, cm_request);
          data_src_sensor = (sns_sensor*)&((sns_fw_data_stream*)
              cm_request->data_stream)->data_source->sensor;
        }
        else
        {
          // If the client request could not be created, then client manager
          // adopts the request and becomes the source sensor for sending
          // the response.
          data_src_sensor = &(((sns_fw_sensor_instance *)this)->sensor->sensor);
          rc = SNS_RC_INVALID_VALUE;
        }

        sns_fw_log_client_api_resp(this, data_src_sensor,
            cm_get_sensor_uid(NULL),
                                client->client_id, rc);
  }
    }
    else
  {
      SNS_INST_PRINTF(ERROR, this, "Rejected invalid client request %d from client %d",
                      req_msg.msg_id, client->client_id);
    }
  }
    sns_cm_reval_and_register_timers( (sns_sensor*)cm_sensor,
                                      req_msg.susp_config.client_proc_type, cm_request);
  SNS_INST_PRINTF(LOW, this,
      "Instance created and configuration set for client id %d sns_rc %d",
                   client->client_id, rc);
  SNS_PROFILE(SNS_CM_INST_SET_CLNT_CFG_FUNC, 1, 2);
  return rc;
}

static sns_rc cm_instance_deinit(
    sns_sensor_instance *const this)
{

  sns_cm_inst_state *cm_inst_state = (sns_cm_inst_state*)this->state->state;

  cm_process_deinit(this);
  SNS_INST_PRINTF(LOW, this, "Instance deinited for client id %d",
      cm_inst_state->client_id);
  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/
sns_sensor_instance_api cm_sensor_instance_api SNS_SECTION(".rodata.island") =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &cm_instance_init,
  .set_client_config = &cm_instance_set_client_config,
  .notify_event = &cm_instance_notify_event,
  .deinit = &cm_instance_deinit
};
