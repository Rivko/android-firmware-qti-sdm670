/*=============================================================================
  @file sns_sensor_util.c

  Utility functions for use by Sensor Developers.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "pb_decode.h"
#include "sns_assert.h"
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_isafe_list.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"

#define SNS_SECTION(name)  \
  __attribute__((section(name)))

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/
/**
 * Remove a sensor stream
 */
SNS_SECTION(".text.sns") sns_rc
sns_sensor_util_remove_sensor_stream(
    sns_sensor *this,
    sns_data_stream **stream)
{
  sns_rc rc = SNS_RC_SUCCESS;
  if (NULL != this && NULL != *stream)
  {
    sns_service_manager* service_mgr = this->cb->get_service_manager(this);
    sns_stream_service* stream_service =
    (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                  SNS_STREAM_SERVICE);
    rc = stream_service->api->remove_stream(stream_service,
                                            *stream);
    *stream = NULL;
  }
  return rc;
}

/**
 * Remove a sensor instance stream
 */
SNS_SECTION(".text.sns") sns_rc
sns_sensor_util_remove_sensor_instance_stream(
    sns_sensor_instance *this,
    sns_data_stream **stream)
{
  sns_rc rc = SNS_RC_SUCCESS;
  if (NULL != this && NULL != *stream)
  {
    sns_service_manager* service_mgr = this->cb->get_service_manager(this);
    sns_stream_service* stream_service =
    (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                  SNS_STREAM_SERVICE);
    rc = stream_service->api->remove_stream(stream_service,
                                            *stream);
    *stream = NULL;
  }
  return rc;
}

SNS_SECTION(".text.sns") sns_sensor_instance*
sns_sensor_util_find_instance(sns_sensor const *sensor,
    sns_request const *request, sns_sensor_uid const *suid)
{
  sns_sensor_instance *instance;

  for(instance = sensor->cb->get_sensor_instance(sensor, true);
      NULL != instance;
      instance = sensor->cb->get_sensor_instance(sensor, false))
  {
    sns_request const *req;

    for(req = instance->cb->get_client_request(instance, suid, true);
        NULL != req;
        req = instance->cb->get_client_request(instance, suid, false))
    {
      if(request == req)
        return instance;
    }
  }

  return NULL;
}

SNS_SECTION(".text.sns") sns_sensor_instance*
sns_sensor_util_get_shared_instance(sns_sensor *sensor)
{
  sns_sensor *curr;

  for(curr = sensor->cb->get_library_sensor(sensor, true);
      NULL != curr;
      curr = sensor->cb->get_library_sensor(sensor, false))
  {
    sns_sensor_instance *instance =
      curr->cb->get_sensor_instance(curr, true);

    if(NULL != instance)
      return instance;
  }

  return NULL;
}

SNS_SECTION(".text.sns") sns_sensor_instance*
sns_sensor_util_find_instance_match(sns_sensor const *sensor,
    sns_request const *request, sns_sensor_util_instance_match_cb cb)
{
  sns_sensor_instance *instance;

  for(instance = sensor->cb->get_sensor_instance(sensor, true);
      NULL != instance;
      instance = sensor->cb->get_sensor_instance(sensor, false))
  {
    if(cb(request, instance))
      return instance;
  }

  return NULL;
}

SNS_SECTION(".text.sns") sns_sensor_uid
sns_get_suid_lookup(void)
{
  sns_sensor_uid suid;
  sns_suid_sensor temp = sns_suid_sensor_init_default;
  sns_memscpy(&suid, sizeof(suid), &temp, sizeof(sns_suid_sensor));
  return suid;
}

SNS_SECTION(".text.sns") sns_rc
sns_process_streams_in_timesync(sns_data_stream **streams,
                                sns_timesync_event_info *info, int num_streams,
                                process_stream_event process_event_cb,
                                void *userdata)
{
  /* validate arguments */
  if (process_event_cb == NULL || info == NULL || streams == NULL)
  {
    return SNS_RC_INVALID_VALUE;
  }

  for (int i=0; i<num_streams; i++)
  {
    info[i].pending = false;
  }
  /* keep processing the data streams, until done */
  while (true)
  {
    int data_on_each_stream = true;
    /* iterate over all streams until we find a data event for each */
    for (int i=0; i<num_streams; i++)
    {
      sns_data_stream *stream = streams[i];;
      int data_available = false;
      if (stream)
      {
        /* if a previously retrieved event is pending, no need to peek() to
           get it again, continue to next stream */
        if (info[i].pending == true)
        {
          continue;
        }

        /* as a fail-safe, if stream has backlog of more than
           1000 samples, drop old samples
           TODO: remove this check when event buffer low-memory error handling
           is in place */
        while (stream->api->get_input_cnt(stream) > 1000)
        {
          stream->api->get_next_input(stream);
        }

        /* iterate till we find one data event for this stream */
        for (sns_sensor_event *event = stream->api->peek_input(stream);
             NULL != event;
             event = stream->api->get_next_input(stream))
        {
          /* if event is sensor recurring data event,
             save it for processing later. see sns_client.proto for message
             id ranges*/
          if (event->message_id >= 1024 && event->message_id <= 1536)
          {
            data_available = true;
            info[i].event = event;
            info[i].stream_index = i;
            info[i].pending = true;
            break;
          }
          else
          {
            /* process the non-data event immediately */
            process_event_cb(event, i, userdata);
          }
        }
      }
      /* if a data event not found for a stream, set flag */
      if (!data_available)
      {
        data_on_each_stream = false;
      }
    }

    /* if no data on one of the streams, stop processing */
    if (!data_on_each_stream)
    {
      break;
    }

    /* find earliest event from the set and process it */
    uint64_t min_ts = UINT64_MAX;
    int min_idx = 0;
    for (int i=0; i<num_streams; i++)
    {
      if (info[i].event->timestamp < min_ts)
      {
        min_ts = info[i].event->timestamp;
        min_idx = i;
      }
    }
    sns_data_stream *stream = streams[info[min_idx].stream_index];
    process_event_cb(info[min_idx].event, info[min_idx].stream_index, userdata);
    stream->api->get_next_input(stream);
    info[min_idx].pending = false;
  }
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc
sns_sensor_util_find_min_batch_period(sns_sensor_instance *instance,
                                      sns_sensor_uid const *suid,
                                      uint32_t *min_batch_period,
                                      uint32_t *max_flush_period,
                                      bool     *max_batch)
{
  sns_request const *req;
  sns_rc ret_val = SNS_RC_INVALID_STATE;
  *min_batch_period = UINT32_MAX;
  *max_flush_period = 0;
  *max_batch = true;
  for(req = instance->cb->get_client_request(instance, suid, true);
      NULL != req;
      req = instance->cb->get_client_request(instance, suid, false))
  {
    sns_std_request decoded_req = sns_std_request_init_default;
    pb_istream_t stream = pb_istream_from_buffer(req->request, req->request_len);
    if (pb_decode(&stream, sns_std_request_fields, &decoded_req))
    {
      if (decoded_req.has_batching)
      {
        *min_batch_period = decoded_req.batching.batch_period < *min_batch_period ?
          decoded_req.batching.batch_period : *min_batch_period;

        if (decoded_req.batching.has_flush_period)
        {
          *max_flush_period = decoded_req.batching.flush_period > *max_flush_period ?
            decoded_req.batching.flush_period : *max_flush_period;
        }
        if (decoded_req.batching.has_max_batch)
        {
          *max_batch = decoded_req.batching.max_batch == true ?
            *max_batch : false;
        }
        else
        {
          *max_batch = false;
        }
      }
      else
      {
        // TODO: Decide if (decoded_req.has_batching == false) should be interpreted as
        // "no batching" or as "don't care". Current implementation is for interpreting
        // it as "no batching".
        *min_batch_period = 0;
        *max_batch = false;
      }
      ret_val = SNS_RC_SUCCESS;
    }
    else
    {
      SNS_INST_PRINTF(ERROR, instance, "Error decoding client request. Assuming Batch period of 0");
      *min_batch_period = 0;
      *max_flush_period = 0;
      *max_batch = false;
      ret_val = SNS_RC_FAILED;
      break;
    }
  }
  return ret_val;
}

SNS_SECTION(".text.sns") bool
sns_sensor_util_decide_max_batch(
    sns_sensor_instance  *instance,
    sns_sensor_uid const *suid)
{
  sns_request const *req;
  bool ret_val = true;

  for(req = instance->cb->get_client_request(instance, suid, true);
      NULL != req;
      req = instance->cb->get_client_request(instance, suid, false))
  {
    sns_std_request decoded_req = sns_std_request_init_default;
    pb_istream_t stream = pb_istream_from_buffer(req->request, req->request_len);
    if (pb_decode(&stream, sns_std_request_fields, &decoded_req))
    {
      if (decoded_req.has_batching)
      {
        if(decoded_req.batching.has_max_batch &&
           decoded_req.batching.max_batch)
        {
            continue;
        }
        else
        {
          SNS_INST_PRINTF(LOW, instance ,"There is a request with no max batch");
          ret_val = false;
          break;
        }
      }
      else
      {
        SNS_INST_PRINTF(LOW, instance,
          "There is a request with no batching spec");
        ret_val = false;
        break;
      }
    }
    else
    {
      SNS_INST_PRINTF(LOW, instance,
        "Error decoding client request. Assuming Batch period of 0");
      ret_val = false;
      break;
    }
  }
  return ret_val;
}

/** See sns_sensor_util.h */
SNS_SECTION(".text.sns") void
sns_sensor_util_send_flush_event(sns_sensor_uid const *uid,
                                 sns_sensor_instance *instance)
{
  sns_service_manager *smgr = instance->cb->get_service_manager(instance);
  sns_event_service *e_service =
     (sns_event_service*)smgr->get_service(smgr, SNS_EVENT_SERVICE);
  sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

  if(NULL != event)
  {
    event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
    event->event_len = 0;
    event->timestamp = sns_get_system_time();
    e_service->api->publish_event(e_service, instance, event, uid);
  }
}

