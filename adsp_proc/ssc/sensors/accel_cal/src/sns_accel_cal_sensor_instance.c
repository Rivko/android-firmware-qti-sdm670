/*=============================================================================
  @file sns_accel_cal_sensor_instance.c

  The Accel_Cal virtual Sensor Instance implementation

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_accel_cal_sensor.h"
#include "sns_accel_cal_sensor_instance.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_time.h"
#include "sns_timer.pb.h"
#include "sns_types.h"

sns_rc
sns_accel_cal_disable_accel(sns_sensor_instance *const this)
{
  sns_accel_cal_inst_state *state =
     (sns_accel_cal_inst_state*)this->state->state;
  sns_rc rc = SNS_RC_SUCCESS;
  SNS_INST_PRINTF(LOW, this,"Accel cal removing stream %d from resampler ",state->resampler_stream);

  rc = sns_sensor_util_remove_sensor_instance_stream(this,&state->resampler_stream);

  return rc;
}

sns_rc
sns_accel_cal_enable_accel( sns_sensor_instance *const this,
                          sns_stream_service *stream_service)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_resampler_config resampler_config = sns_resampler_config_init_default;
  size_t encoded_len;
  uint8_t buffer[100];
  sns_accel_cal_inst_state *state =
     (sns_accel_cal_inst_state*)this->state->state;

  sns_memscpy(&resampler_config.sensor_uid,
              sizeof(resampler_config.sensor_uid),
              &state->accel_suid,
              sizeof(state->accel_suid));

  resampler_config.resampled_rate = state->persist_state.config.sampleRate;
  resampler_config.rate_type = SNS_RESAMPLER_RATE_MINIMUM;
  resampler_config.filter = false;

  // create connection with resampler sensor
  stream_service->api->create_sensor_instance_stream(
    stream_service,
    this,
    state->resampler_suid,
    &state->resampler_stream);

  if(NULL != state->resampler_stream)
  {
    encoded_len = pb_encode_request(buffer, sizeof(buffer),
        &resampler_config, sns_resampler_config_fields, NULL);
    if(0 < encoded_len)
    {
      sns_request request = (sns_request){
        .message_id = SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG,
        .request_len = encoded_len, .request = buffer };
      rc = state->resampler_stream->api->send_request(state->resampler_stream, &request);
      SNS_INST_PRINTF(MED, this, "Accel cal sent accel request to resampler");
    }
    else
    {
      SNS_INST_PRINTF(ERROR, this, "Accel cal Failed to encode resampler config");
    }
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this, "Accel cal resampler stream creation failed");
  }

  return rc;
}

bool
sns_accel_cal_algo(float *restrict accel_data, float *restrict bias,
                     sns_std_sensor_sample_status *accuracy)
{
  UNUSED_VAR(accel_data);
  UNUSED_VAR(bias);
  UNUSED_VAR(accuracy);

  //fill your calibration algo here

  return false;
}

sns_rc
sns_accel_cal_process_accel(sns_sensor_instance *this,
                           sns_time timestamp,
                           float *accel_data)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_accel_cal_inst_state *state =
      (sns_accel_cal_inst_state*)this->state->state;

    SNS_INST_PRINTF(LOW, this, "Accel cal : accel samples x,[%d/1000],y,[%d/1000],z,[%d/1000]",
        (int32_t)(accel_data[0]*1000),
        (int32_t)(accel_data[1]*1000),
        (int32_t)(accel_data[2]*1000));

    float new_bias[SNS_CAL_SENSOR_DIM];
    sns_std_sensor_sample_status   new_accuracy;
    if(sns_accel_cal_algo(accel_data, new_bias, &new_accuracy))
    {
      // Save in persist structure
      state->persist_state.output.bias[0] = new_bias[0];
      state->persist_state.output.bias[1] = new_bias[1];
      state->persist_state.output.bias[2] = new_bias[2];
      state->persist_state.output.timestamp = timestamp;
      state->persist_state.output.accuracy = new_accuracy;
    }

  return rc;
}


sns_rc
sns_accel_cal_process_amd(sns_sensor_instance *const this,
                          sns_stream_service *stream_service,
                          sns_amd_event amd_data)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_accel_cal_inst_state *state =
     (sns_accel_cal_inst_state*)this->state->state;
  state->amd_state = amd_data;

  SNS_INST_PRINTF(LOW, this," cal received %d state from AMD", amd_data.state);

  // Stop the  stream when motion is detected
  if((SNS_AMD_EVENT_TYPE_MOTION == state->amd_state.state) ||
      (SNS_AMD_EVENT_TYPE_UNKNOWN == state->amd_state.state) )
  {
    //Switch off  if not done already.
    if(state->resampler_stream)
    {
      rc = sns_accel_cal_disable_accel(this);
      if(SNS_RC_SUCCESS == rc)
      {
        state->resampler_stream = NULL;
      }
    }
  }
  else if(SNS_AMD_EVENT_TYPE_STATIONARY == state->amd_state.state )
  {
    //Switch on  stream if not done already.
    if(!state->resampler_stream)
    {
      rc = sns_accel_cal_enable_accel(this, stream_service);
    }
  }
  return rc;
}

/* Generic function to handle cal events to be published
*  Handles
*  a. Bias only.
*  b. Scale factor only.
*  c. Comp Matrix only.
*  d. Bias + Scale_factor
*  e. Bias + Comp Matrix.
*  f. Bias + Scale Factor + Comp Matrix
*  g. Scale Factor + Comp Matrix
*/
sns_rc
pb_send_accel_cal_sensor_event(
    struct sns_sensor_instance *instance,
    struct sns_sensor_uid const *sensor_uid,
    sns_time timestamp,
    uint32_t message_id,
    sns_std_sensor_sample_status sample_status,
    float const *data,
    uint8_t data_len,
    size_t encoded_len,
    sns_accel_cal_event_type event_type)
{
  sns_rc rv = SNS_RC_FAILED;

  sns_service_manager *manager =
      instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

  sns_cal_event cal_event = sns_cal_event_init_default;

  uint8_t arr_bias_index = 0;
  uint8_t arr_scale_factor_index = 0;
  uint8_t arr_comp_matrix_index = 0;
  uint8_t g_arr_index =0;

  pb_float_arr_arg arg_bias;
  pb_float_arr_arg arg_scale_factor;
  pb_float_arr_arg arg_comp_matrix;

  //Status of the event being published
  cal_event.status = sample_status;

  if(event_type & SNS_ACCEL_CAL_BIAS)
  {
    arg_bias.arr = (float *)data;
    arg_bias.arr_len = data_len;
    arg_bias.arr_index = &arr_bias_index;
    g_arr_index += data_len;
    cal_event.bias.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.bias.arg = &arg_bias;
  }
  else if(event_type & SNS_ACCEL_CAL_SCALE_FACTOR)
  {
    arg_scale_factor.arr = (float *)data;
    arg_scale_factor.arr_len = data_len;
    arr_scale_factor_index = g_arr_index;
    arg_scale_factor.arr_index = &arr_scale_factor_index;
    g_arr_index += data_len;

    cal_event.scale_factor.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.scale_factor.arg = &arg_scale_factor;

  }
  else if(event_type & SNS_ACCEL_CAL_COMP_MATRIX)
  {
    arg_comp_matrix.arr = (float *)data;
    //If data_len passed corresponds to axes.
    //This can happen when the data being passed has comp_matrix with bias.
    if(data_len == SNS_ACCEL_CAL_AXES)
       arg_comp_matrix.arr_len = data_len*data_len;
    else
     arg_comp_matrix.arr_len = data_len ;

    arr_comp_matrix_index = g_arr_index;
    arg_scale_factor.arr_index = &arr_comp_matrix_index;

    cal_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.comp_matrix.arg = &arg_comp_matrix;
  }
  else
  {
    return rv;
  }

  if(encoded_len > 0)
  {
    sns_sensor_event *event =
        event_service->api->alloc_event(event_service, instance, encoded_len);

    pb_ostream_t stream =
        pb_ostream_from_buffer((pb_byte_t*)event->event, encoded_len);

    if(!pb_encode(&stream, sns_cal_event_fields, &cal_event))
    {
      SNS_INST_PRINTF(ERROR,instance, "Error encoding sensor stream event");
    }

    event->event_len = encoded_len;
    event->message_id = message_id;
    event->timestamp = timestamp;

    rv = event_service->api->publish_event(event_service,
        instance, event, sensor_uid);
  }

  return rv;
}

/**
 * Process incoming events to the Accel_Cal Sensor Instance.
 */
static sns_rc
sns_accel_cal_inst_notify_event(sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_accel_cal_inst_state *state =
    (sns_accel_cal_inst_state*)this->state->state;
  sns_service_manager *manager =
    this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  sns_amd_event amd_data = sns_amd_event_init_default;
  sns_time last_timestamp = state->persist_state.output.timestamp;

  if(NULL != state->amd_stream)
  {
    for(sns_sensor_event *event = state->amd_stream->api->peek_input(state->amd_stream);
        NULL != event;
        event = state->amd_stream->api->get_next_input(state->amd_stream))
    {
      if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
      {
        SNS_INST_PRINTF(ERROR, this, "Error event received");
        if(SNS_RC_SUCCESS != rc)
        {
          SNS_INST_PRINTF(ERROR, this, "Unable to send error event");
        }

        //Consume all remaining events
        while(NULL != state->amd_stream->api->get_next_input(state->amd_stream));
        rc = SNS_RC_INVALID_STATE;
        break;
      }
      else if(SNS_AMD_MSGID_SNS_AMD_EVENT == event->message_id)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);

        if(!pb_decode(&stream, sns_amd_event_fields, &amd_data))
        {
           SNS_INST_PRINTF(ERROR, this, "Accel cal : Error in decoding AMD data event");
        }
        else
        {
          rc = sns_accel_cal_process_amd(this, stream_service, amd_data);
        }
      }
    }
  }

  if(NULL != state->resampler_stream)
  {
    sns_sensor_event *event;
    for(event = state->resampler_stream->api->peek_input(state->resampler_stream);
        NULL != event;
        event = (state->resampler_stream ?state->resampler_stream->api->get_next_input(state->resampler_stream):NULL))
    {
      if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == event->message_id)
      {
        float data[3] = { 0 };
        uint8_t arr_index = 0;
        pb_float_arr_arg arg = {
           .arr = data,
           .arr_len = ARR_SIZE(data),
           .arr_index = &arr_index
        };
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
        sns_std_sensor_event resampler_data = sns_std_sensor_event_init_default;

        resampler_data.data = (struct pb_callback_s) {
          .funcs.decode = &pb_decode_float_arr_cb, .arg = &arg
        };

        if(!pb_decode(&stream, sns_std_sensor_event_fields, &resampler_data))
        {
          SNS_INST_PRINTF(ERROR, this, "Error in decoding resampler event");
        }
        else
        {
          sns_accel_cal_process_accel( this,event->timestamp, data );
        }
      }
    }
  }

  if(state->persist_state.output.timestamp != last_timestamp)
  {
     rc = pb_send_accel_cal_sensor_event(this,
                                   state->self_suid,
                                   state->persist_state.output.timestamp,
                                   SNS_CAL_MSGID_SNS_CAL_EVENT,
                                   state->persist_state.output.accuracy,
                                   state->persist_state.output.bias,
                                   ARR_SIZE(state->persist_state.output.bias),
                                   pb_get_encoded_size_sensor_stream_event(
                                       state->persist_state.output.bias,
                                       ARR_SIZE(state->persist_state.output.bias)),
                                   SNS_ACCEL_CAL_BIAS);
  }
  return rc;
}

/* See sns_sensor_instance_api::init */
static sns_rc
sns_accel_cal_inst_init(sns_sensor_instance *this,
    sns_sensor_state const *state)
{
  sns_accel_cal_inst_state *inst_state =
     (sns_accel_cal_inst_state*)this->state->state;
  accel_cal_state *sensor_state =
     (accel_cal_state*)state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);

  // load latest persistent state from sensor state
  sns_memscpy(&inst_state->persist_state,
              sizeof(inst_state->persist_state),
              &sensor_state->persist_state,
              sizeof(sensor_state->persist_state));
  inst_state->self_suid = sensor_state->self_suid;

  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "accel",
      &inst_state->accel_suid);
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "resampler",
      &inst_state->resampler_suid);
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "amd",
      &inst_state->amd_suid);

  inst_state->diag_service =
    (sns_diag_service*)service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  return SNS_RC_SUCCESS;
}

/* See sns_sensor_instance_api::set_client_config */
static sns_rc
sns_accel_cal_inst_set_client_config(sns_sensor_instance *const this,
                                    sns_request const *client_request)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_accel_cal_inst_state *state =
     (sns_accel_cal_inst_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);

  //On change config is empty .
  if(SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG == client_request->message_id)
  {
    //push the event from persistent state
    pb_send_accel_cal_sensor_event(this,
        state->self_suid,
        state->persist_state.output.timestamp,
        SNS_CAL_MSGID_SNS_CAL_EVENT,
        state->persist_state.output.accuracy,
        state->persist_state.output.bias,
        ARR_SIZE(state->persist_state.output.bias),
        pb_get_encoded_size_sensor_stream_event(
            state->persist_state.output.bias,
            ARR_SIZE(state->persist_state.output.bias)),
            SNS_ACCEL_CAL_BIAS);

    // creating AMD stream is not already created
    if(!state->amd_stream)
    {
      stream_service->api->create_sensor_instance_stream(stream_service,
          this,state->amd_suid, &state->amd_stream);
      SNS_INST_PRINTF(LOW, this,"Accel cal creates AMD stream %d", state->amd_stream);

      if(NULL != state->amd_stream)
      {
        SNS_INST_PRINTF(LOW, this, "Accel cal Sending on-change reqest to AMD");
        sns_request sensor_req = (sns_request){
          .message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG,
          .request_len = 0, .request = NULL };

        state->amd_stream->api->send_request(state->amd_stream, &sensor_req);
      }
    }
  }
  else if(SNS_CAL_MSGID_SNS_CAL_RESET==client_request->message_id)
  {
    sns_memset(&state->persist_state.output, 0, sizeof(state->persist_state.output));
  }
  else
  {
    SNS_INST_PRINTF(ERROR, this,"Accel cal receives unexpected request msg_id %d",
        client_request->message_id);
    rc = SNS_RC_NOT_SUPPORTED;
  }
  return rc;
}

/* See sns_sensor_instance_api::deinit */
static sns_rc
sns_accel_cal_inst_deinit(sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_accel_cal_inst_state *inst_state =
     (sns_accel_cal_inst_state*)this->state->state;

  // tear down connections to open streams
  rc = sns_sensor_util_remove_sensor_instance_stream(this, &inst_state->amd_stream);
  rc = sns_sensor_util_remove_sensor_instance_stream(this, &inst_state->resampler_stream);
  return rc;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_accel_cal_sensor_instance_api =
{
  .struct_len = sizeof(sns_sensor_instance_api),
  .init = &sns_accel_cal_inst_init,
  .deinit = &sns_accel_cal_inst_deinit,
  .set_client_config = &sns_accel_cal_inst_set_client_config,
  .notify_event = &sns_accel_cal_inst_notify_event
};
