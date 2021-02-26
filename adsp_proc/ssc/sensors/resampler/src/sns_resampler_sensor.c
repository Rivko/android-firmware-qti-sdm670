/*=============================================================================
  @file sns_resampler_sensor.c

  The Resampler virtual Sensor implementation

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_resampler_sensor.h"
#include "sns_resampler_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std_sensor.pb.h"
#include "sns_types.h"

/*=============================================================================
 * Static Data Definitions
 ============================================================================*/

#define SNS_RESAMPLER_VERSION 1

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Publish all saved attributes for Resampler.
 */
static void
publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "resampler";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "resampler";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "qualcomm";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = "sns_resampler.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_RESAMPLER_VERSION;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

/* See sns_sensor::notify_event */
sns_rc
sns_resampler_notify_event(sns_sensor *const this)
{
  resampler_state *state =
    (resampler_state*)this->state->state;
  /*sns_service_manager *manager =
    this->cb->get_service_manager(this);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);*/
  sns_sensor_event *event =
      state->fw_stream->api->peek_input(state->fw_stream);

  while(NULL != event)
  {
    /*if(true)
    {
      // PEND: If this is the registry SUID
      sns_request registry_req;
      stream_service->api->create_sensor_stream(stream_service,
          this, sns_fw_suid, &state->registry_stream);
      // PEND:  Form registry request
      state->registry_stream->api->send_request(state->registry_stream,
          &registry_req);

      state->reg_suid = (sns_sensor_uid){0};  // PEND: Save SUID
    }*/
    event = state->fw_stream->api->get_next_input(state->fw_stream);
  }

  while(NULL != state->registry_stream &&
        0 != state->registry_stream->api->get_input_cnt(
           state->registry_stream))
  {
    event = state->registry_stream->api->peek_input(state->registry_stream);
    // PEND: Save registry data
    state->registry_stream->api->get_next_input(state->registry_stream);
  }

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::init */
sns_rc
sns_resampler_init(sns_sensor *const this)
{
  /*resampler_state *state = (resampler_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
    (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  //sns_request fw_suid_req;

  // create connection with fw to discover SUIDs of sensors of interest.
  stream_service->api->create_sensor_stream(stream_service,
                                            this,
                                            sns_fw_suid,
                                            &state->fw_stream);*/

  // PEND:  Form SUID request for registry SUID
  // state->fw_stream->api->send_request(state->fw_stream, &fw_suid_req);

  publish_attributes(this);

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::deinit */
sns_rc
sns_resampler_deinit(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

