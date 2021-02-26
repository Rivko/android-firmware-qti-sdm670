/**
 * @file sns_test_sensor_island.c
 *
 * The test virtual Sensor implementation
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/3.2/sensors/test/src/sns_test_sensor_island.c#4 $
 * $DateTime: 2018/03/27 11:46:46 $
 * $Change: 15784639 $
 *
 **/

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_test_sensor.h"
#include "sns_test_sensor_instance.h"
#include "sns_types.h"

extern const sns_test_implementation test_sensor_impl;

void
sns_test_send_sensor_request(sns_sensor *const this,
                             sns_sensor_uid suid,
                             void* payload,
                             pb_field_t const* payload_fields,
                             uint32_t message_id,
                             sns_std_request std_req)
{
  sns_test_state* state = (sns_test_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  size_t encoded_len;
  uint8_t buffer[1000];
  sns_memset(buffer, 0, sizeof(buffer));

  stream_mgr->api->create_sensor_stream(stream_mgr,
      this, suid, &state->sensor_stream);

  if(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == message_id)
  {
    SNS_PRINTF(LOW, this, "Sending on-change reuest");
    sns_request request = (sns_request){ .message_id = message_id,
      .request_len = 0, .request = NULL };
    state->sensor_stream->api->send_request(state->sensor_stream, &request);
  }
  else
  {
    encoded_len = pb_encode_request(buffer, sizeof(buffer),
                                    payload, payload_fields, &std_req);

    if(0 < encoded_len && NULL != state->sensor_stream)
    {
      sns_request request = (sns_request){ .message_id = message_id,
        .request_len = encoded_len, .request = buffer };
      state->sensor_stream->api->send_request(state->sensor_stream, &request);
    }
    else
    {
      SNS_PRINTF(ERROR, this, "Failed to send sensor request, stream=%p", state->sensor_stream);
    }
  }
}

/**
 * @brief starts a stream for the tested sensor
 * @param this
 */
void
sns_test_start_sensor_stream(sns_sensor *const this)
{
  sns_test_state *state = (sns_test_state*)this->state->state;
  void *payload = state->sns_pb_req_payload;
  uint32_t message_id;
  const pb_field_t *payload_fields = NULL;
  sns_std_request std_req = sns_std_request_init_default;
#ifndef SNS_TEST_THRESHOLD
  sns_sensor_uid suid;
#endif
  state->test_sensor_create_request(this, payload, &payload_fields,
                                    &message_id, &std_req);

  /* PEND: create stream for all SUIDs found for sensor type under test */
#ifndef SNS_TEST_THRESHOLD
  sns_suid_lookup_get(&state->suid_lookup_data, test_sensor_impl.datatype, &suid);
  sns_test_send_sensor_request(this, suid, payload, payload_fields,
      message_id, std_req);
#endif
  state->test_in_progress = true;
}

/**
 * @brief stops the stream for the tested sensor
 * @param this
 */
void
sns_test_stop_sensor_stream(sns_sensor *const this)
{
  sns_test_state *state = (sns_test_state*)this->state->state;
  if (NULL != state->sensor_stream)
  {
    //SENSOR_PRINTF_HIGH_FULL(this, "removing stream for %s", state->suid_search[0].data_type_str);
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    stream_mgr->api->remove_stream(stream_mgr, state->sensor_stream);
    state->sensor_stream = NULL;
  }
}

sns_rc
sns_test_handle_sensor_event(sns_sensor *const this)
{
  sns_test_state* s = (sns_test_state*)this->state->state;

  for(sns_sensor_event *e = s->sensor_stream->api->peek_input(s->sensor_stream);
      NULL != e && s->remaining_events > 0;
      e = s->sensor_stream->api->get_next_input(s->sensor_stream))
  {
    s->test_sensor_process_event(this, e->event, e->event_len, s->test_data,
                                 e->message_id, e->timestamp);
    s->remaining_events--;
    s->num_events_received++;
  }

  /* if all events are processed, stop the stream */
  if (s->remaining_events <= 0)
  {
    s->remaining_iterations--;
    sns_test_stop_sensor_stream(this);
    SNS_PRINTF(HIGH, this, "test iteration finished, remaining=%d",
                             s->remaining_iterations);
    if (s->remaining_iterations > 0)
    {
      s->remaining_events = NUM_EVENTS_TO_PROCESS;
      sns_test_start_sensor_stream(this);
    }
    else
    {
      SNS_PRINTF(HIGH, this, "test finished!");
      if (s->num_events_received == NUM_EVENTS_TO_PROCESS * NUM_TEST_ITERATIONS)
      {
        SNS_PRINTF(HIGH, this, "result = PASS");
      }
      else
      {
        SNS_PRINTF(HIGH, this, "result = FAIL");
      }
    }
  }

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::notify_event */
sns_rc
sns_test_notify_event(sns_sensor *const this)
{
  sns_test_state *state = (sns_test_state*)this->state->state;
  bool lookup_complete = sns_suid_lookup_complete(&state->suid_lookup_data);

  sns_suid_lookup_handle(this, &state->suid_lookup_data);
  if(lookup_complete != sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    SNS_PRINTF(MED, this, "Sensor streaming starts");
    sns_test_start_sensor_stream(this);
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);
  }

  if(NULL != state->sensor_stream)
  {
    sns_test_handle_sensor_event(this);
  }

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
 sns_sensor_uid const*
sns_test_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
   static const sns_sensor_uid sensor_uid =
  {
    .sensor_uid = { TEST_SUID }
  };

  return &sensor_uid;
}

/* See sns_sensor::set_client_request */
 sns_sensor_instance*
sns_test_set_client_request(sns_sensor *const this,
                            struct sns_request const *exist_request,
                            struct sns_request const *new_request,
                            bool remove)
{
  UNUSED_VAR(this);
  UNUSED_VAR(exist_request);
  UNUSED_VAR(new_request);
  UNUSED_VAR(remove);
  return NULL;
}

sns_sensor_api sns_test_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &sns_test_init,
  .deinit             = &sns_test_deinit,
  .get_sensor_uid     = &sns_test_get_sensor_uid,
  .set_client_request = &sns_test_set_client_request,
  .notify_event       = &sns_test_notify_event,
};
