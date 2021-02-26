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
#include "sns_mem_util.h"
#include "sns_oem1_sensor_instance.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#ifndef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
static sns_rc sns_oem1_process_resampler_event(sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_oem1_inst_state *state = (sns_oem1_inst_state*)this->state->state;

  sns_sensor_event *resampler_event_in = NULL;
  SNS_INST_PRINTF(LOW, this, "sns_oem1_inst_process_resampler_event");

  resampler_event_in = state->resampler_stream->api->peek_input(state->resampler_stream);

  while( NULL != resampler_event_in )
  {
    if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == resampler_event_in->message_id)
    {
      float data[3] = { 0 };
      float oem1_payload[OEM1_NUM_ELEMS];

      uint8_t arr_index = 0;
      pb_float_arr_arg arg = {
          .arr = data,
          .arr_len = ARR_SIZE(data),
          .arr_index = &arr_index
      };

      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)resampler_event_in->event,resampler_event_in->event_len);
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
        if (SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == resampler_data.status)
        {
          SNS_INST_PRINTF(LOW, this, " OEM1 received unreliable accel data"
              " from resampler ignoring..");
          resampler_event_in = state->resampler_stream->api->get_next_input(state->resampler_stream);
          continue;
        }
        //This is dummy logic for OEM1 demonstration purposes
        //OEMs can replace with their algo logic
        oem1_facing_state current_state = OEM1_UNKNOWN;

        SNS_INST_PRINTF(LOW, this,
            "resampler accel data: x %d/1000, y %d/1000, z %d/1000",
           (int) (1000*data[0]),(int)( 1000*data[1]),(int)(1000*data[2]));

        //sns_oem1_procces(this, &input);
        state->inst_config.previous_state = state->inst_config.current_state;

        if(0 < data[2])
        {
          current_state = OEM1_FACING_UP;
          oem1_payload[0]=100;
          oem1_payload[1]=data[1];
          oem1_payload[2]=data[2];
        }
        else
        {
          current_state = OEM1_FACING_DOWN;
          oem1_payload[0]= state->down_value;
          oem1_payload[1]=data[1];
          oem1_payload[2]=data[2];
        }

        state->inst_config.current_state = current_state;
        SNS_INST_PRINTF(LOW, this, "OEM1 output:  x %d/1000, y %d/1000, z %d/1000",
            (int) (1000*oem1_payload[0]),(int)( 1000*oem1_payload[1]),(int)(1000*oem1_payload[2]));

#ifdef OEM1_SUPPORT_EVENT_TYPE
        if(state->inst_config.current_state != state->inst_config.previous_state)
        {
#endif
          SNS_INST_PRINTF(MED, this,
              "OEM1 last %d current %d",
              state->inst_config.current_state,
              state->inst_config.previous_state);
          rc = pb_send_sensor_stream_event(this,
              NULL,
              resampler_event_in->timestamp,
              SNS_OEM1_MSGID_SNS_OEM1_DATA,
              SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
              oem1_payload,
              OEM1_NUM_ELEMS,
              state->config.encoded_data_event_len);
          if(SNS_RC_SUCCESS != rc)
          {
            SNS_INST_PRINTF(ERROR, this, "sns_oem1_inst_notify_event - Error in sending event");
          }
#ifdef OEM1_SUPPORT_EVENT_TYPE
        }
#endif
      }
    }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == resampler_event_in->message_id)
    {
      SNS_INST_PRINTF(LOW, this, "Received SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT");
    }
    else if(SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT == resampler_event_in->message_id)
    {
      SNS_INST_PRINTF(LOW, this, "Received SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG_EVENT");
    }
    // check next sample
    resampler_event_in = state->resampler_stream->api->get_next_input(state->resampler_stream);
  }
  return rc;
}
#endif
/**
 * Process incoming events to the OEM1 Sensor Instance.
 */
static sns_rc
sns_oem1_inst_notify_event(sns_sensor_instance *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;

#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_oem1_inst_state *state = (sns_oem1_inst_state*)this->state->state;

  sns_sensor_event *accel_event_in = NULL;
  SNS_INST_PRINTF(LOW, this, "sns_oem1_inst_notify_event");

  accel_event_in = state->accel_stream->api->peek_input(state->accel_stream);
  while(NULL != accel_event_in)
  {
    if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == accel_event_in->message_id)
    {
      float temp[OEM1_NUM_ELEMS];
      float accel_payload[OEM1_NUM_ELEMS];
      pb_istream_t stream =
          pb_istream_from_buffer((pb_byte_t*)accel_event_in->event, accel_event_in->event_len);
      sns_std_sensor_event stream_event = sns_std_sensor_event_init_default;
      uint8_t arr_index = 0;
      pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = OEM1_NUM_ELEMS, .arr_index = &arr_index};

      stream_event.data = (struct pb_callback_s)
            {.funcs.decode = &pb_decode_float_arr_cb, .arg = &arg};

      if(!pb_decode(&stream, sns_std_sensor_event_fields, &stream_event))
      {
        SNS_INST_PRINTF(ERROR, this, "sns_oem1_inst_notify_event - Error in decoding event");
      }
      else
      {
        if (SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE == stream_event.status)
        {
          SNS_INST_PRINTF(LOW, this, " OEM1 received unreliable accel data"
              " from accel ignoring..");
          accel_event_in = state->accel_stream->api->get_next_input(state->accel_stream);
          continue;
        }
        //This is dummy logic for OEM1 demonstration purposes
        //OEMs can replace with their algo logic
        oem1_facing_state current_state = OEM1_UNKNOWN;

        SNS_INST_PRINTF(LOW, this,
            "accel data: x %d/1000, y %d/1000, z %d/1000",
           (int) (1000*temp[0]),(int)( 1000*temp[1]),(int)(1000*temp[2]));

        state->inst_config.previous_state = state->inst_config.current_state;

        if(0 < temp[2])
        {
          current_state = OEM1_FACING_UP;
          accel_payload[0]=100;
          accel_payload[1]=temp[1];
          accel_payload[2]=temp[2];
        }
        else
        {
          current_state = OEM1_FACING_DOWN;
          accel_payload[0]=state->down_value;
          accel_payload[1]=temp[1];
          accel_payload[2]=temp[2];
        }
        state->inst_config.current_state = current_state;

        SNS_INST_PRINTF(LOW, this, "OEM1 output:x %d/1000, y %d/1000, z %d/1000",
           (int) (1000*accel_payload[0]),(int)( 1000*accel_payload[1]),
           (int)(1000*accel_payload[2]));

#ifdef OEM1_SUPPORT_EVENT_TYPE
        if(state->inst_config.current_state != state->inst_config.previous_state)
        {
#endif
        rc = pb_send_sensor_stream_event(this,
            NULL,
            accel_event_in->timestamp,
            SNS_OEM1_MSGID_SNS_OEM1_DATA,
            SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
            accel_payload,
            OEM1_NUM_ELEMS,
            state->config.encoded_data_event_len);
        if(SNS_RC_SUCCESS != rc)
        {
          SNS_INST_PRINTF(ERROR, this, "sns_oem1_inst_notify_event - Error in sending event");
        }
#ifdef OEM1_SUPPORT_EVENT_TYPE
        }
#endif
      }
      }
    else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == accel_event_in->message_id)
    {
      SNS_INST_PRINTF(LOW, this, "Received SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT");
    }
    accel_event_in = state->accel_stream->api->get_next_input(state->accel_stream);
  }
#else
  sns_oem1_process_resampler_event(this);
#endif
  return rc;

}


/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_instance_api sns_oem1_sensor_instance_api =
{
    .struct_len = sizeof(sns_sensor_instance_api),
    .init = &sns_oem1_inst_init,
    .deinit = &sns_oem1_inst_deinit,
    .set_client_config = &sns_oem1_inst_set_client_config,
    .notify_event = &sns_oem1_inst_notify_event
};
