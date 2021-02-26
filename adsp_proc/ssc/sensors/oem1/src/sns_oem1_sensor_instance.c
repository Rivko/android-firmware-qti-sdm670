/*=============================================================================
  @file sns_oem1_sensor_instance.c

  The oem1 virtual Sensor Instance implementation

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_oem1_sensor_instance.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"

/* See sns_sensor_instance_api::init */
sns_rc
sns_oem1_inst_init(sns_sensor_instance *this,
    sns_sensor_state const *state)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_oem1_inst_state *inst_state =
      (sns_oem1_inst_state*)this->state->state;
  sns_oem1_sensor_state *sensor_state =
      (sns_oem1_sensor_state*)state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_stream_service *stream_service = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
#endif

#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "accel", &inst_state->accel_suid);
#else
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "resampler", &inst_state->resampler_suid);
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "accel", &inst_state->accel_suid);
#endif

  inst_state->diag_service = (sns_diag_service*)
          service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  inst_state->down_value = sensor_state->config.down_value;

#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  // start regular stream for accel
  stream_service->api->create_sensor_instance_stream(
      stream_service,
      this,
      inst_state->accel_suid,
      &inst_state->accel_stream);
#endif

  inst_state->inst_config.previous_state = OEM1_UNKNOWN;
  inst_state->inst_config.current_state = OEM1_UNKNOWN;

  // read platform specific configuration
  sns_memscpy(&inst_state->config,
      sizeof(inst_state->config),
      &sensor_state->config,
      sizeof(sensor_state->config) );
  return rc;
}

/* See sns_sensor_instance_api::deinit */
sns_rc
sns_oem1_inst_deinit(sns_sensor_instance *const this)
{
  sns_oem1_inst_state *state =
      (sns_oem1_inst_state*)this->state->state;

#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_sensor_util_remove_sensor_instance_stream(this, &state->accel_stream);
#else
  sns_sensor_util_remove_sensor_instance_stream(this, &state->resampler_stream);
#endif
  SNS_INST_PRINTF(MED, this, "OEM1 inst deinit");

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
sns_rc
sns_oem1_inst_set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_oem1_inst_state *state = (sns_oem1_inst_state*)this->state->state;

  SNS_INST_PRINTF(LOW, this, "sns_oem1_inst_set_client_config");

#ifdef OEM1_SUPPORT_EVENT_TYPE
  if(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == client_request->message_id)
#else
  if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == client_request->message_id)
#endif
  {

#ifdef OEM1_SUPPORT_EVENT_TYPE
    state->config.sample_rate = OEM1_MINUMUM_SAMPLE_RATE;
#else
    // initialize state here
    sns_std_sensor_config *client_req =
        (sns_std_sensor_config *)(client_request->request);
    sns_memscpy(&state->client_config,
        sizeof(state->client_config),
        client_request->request,
        client_request->request_len);

    state->config.sample_rate = client_req->sample_rate;
#endif

#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST // not using the resampler, request accel directly
    size_t encoded_len;
    uint8_t buffer[20];

    // enable accel here
    sns_memset(buffer, 0, sizeof(buffer));
    sns_std_sensor_config accel_config = {.sample_rate = state->config.sample_rate};
    SNS_INST_PRINTF(LOW, this, "Enable accel at rate %d", state->config.sample_rate);

    encoded_len = pb_encode_request(buffer,
        sizeof(buffer),
        &accel_config,
        sns_std_sensor_config_fields,
        NULL);
    if(0 < encoded_len && NULL != state->accel_stream )
    {
      sns_request request = (sns_request){
        .message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG,
            .request_len = encoded_len, .request = buffer };
      state->accel_stream->api->send_request(state->accel_stream, &request);
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Error in creating accel stream OR encoding failed");
      rc = SNS_RC_NOT_SUPPORTED;
    }

    SNS_INST_PRINTF(LOW, this, "Processed oem1 config request: enabled accel with"
        "sample rate %d, result: %u", state->config.sample_rate, rc);
#else
    // start streaming from resampler
    SNS_INST_PRINTF(LOW, this, "Enable resampler at rate %d", state->config.sample_rate);
    sns_service_manager *manager =
        this->cb->get_service_manager(this);
    sns_stream_service *stream_service =
        (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

    sns_resampler_config resampler_config = sns_resampler_config_init_default;
    size_t encoded_len;
    uint8_t buffer[100];

    sns_memscpy(&resampler_config.sensor_uid,
        sizeof(resampler_config.sensor_uid),
        &state->accel_suid,
        sizeof(state->accel_suid));
    resampler_config.resampled_rate = state->config.sample_rate;
    resampler_config.rate_type = SNS_RESAMPLER_RATE_MINIMUM;
    resampler_config.filter = false;

    SNS_INST_PRINTF(LOW, this, "Before create_sensor_instance_stream");
    // create connection with resampler sensor
    stream_service->api->create_sensor_instance_stream(
        stream_service,
        this,
        state->resampler_suid,
        &state->resampler_stream);
    SNS_INST_PRINTF(LOW, this, "After create_sensor_instance_stream");

    if(NULL != state->resampler_stream)
    {
      SNS_INST_PRINTF(LOW, this, "resampler_stream NOT NULL");
      encoded_len = pb_encode_request(buffer, sizeof(buffer),
          &resampler_config, sns_resampler_config_fields, NULL);
      if(0 < encoded_len)
      {
        sns_request request = (sns_request){
          .message_id = SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG,
              .request_len = encoded_len, .request = buffer };
        rc = state->resampler_stream->api->send_request(state->resampler_stream, &request);
        SNS_INST_PRINTF(MED, this, "oem1 sent accel request to resampler at rate %d", state->config.sample_rate);
      }
      else
      {
        SNS_INST_PRINTF(ERROR, this, "oem1 failed to encode resampler config");
      }
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "oem1 resampler stream creation failed");
    }
#endif
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Unsupported request message id %u",
        client_request->message_id);
    rc = SNS_RC_NOT_SUPPORTED;
  }

  return rc;
}

