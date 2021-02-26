/*=============================================================================
  @file sns_accel_cal_sensor.c

  The Accel_Cal virtual Sensor implementation

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
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
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_registry.pb.h"
#include "sns_sensor_util.h"
#include "sns_sensor_util_internal.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_types.h"
#include <stdio.h>

// temp structure for pb arg
typedef struct pb_arg_accel_cal
{
  sns_sensor *sensor;
  const char *name;
}pb_arg_accel_cal;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
/**
 * Publish all attributes for accel cal.
 */
static void
sns_accel_cal_publish_attributes(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state *)this->state->state;
  {
    char const name[] = "accel_cal";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "accel_cal";
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
    char const proto_files[] = "sns_accel_cal.proto";
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
    value.boolean = false;         // made available later
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->persist_state.config.registration_index;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RIGID_BODY,
        &value, 1, true);
  }
}

static void
sns_accel_cal_send_registry_request(sns_sensor *const this,char* reg_group_name)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
    .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    state->registry_stream->api->send_request(state->registry_stream, &request);
  }
}

static bool
sns_accel_cal_parse_registry_persist(pb_istream_t *stream,
                      const pb_field_t *field,
                      void **arg)
{
  sns_sensor *this = (sns_sensor*)*arg;
  accel_cal_state *state = (accel_cal_state*)this->state->state;

  UNUSED_VAR(field);
  sns_registry_data_item reg_item = sns_registry_data_item_init_default;
  pb_istream_t stream_cpy = *stream;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;

  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);

  if(reg_item.has_subgroup)
  {
    sns_registry_data_item temp_item = sns_registry_data_item_init_default;
    temp_item.subgroup.items.funcs.decode = &sns_accel_cal_parse_registry_persist;
    temp_item.subgroup.items.arg = *arg;

    if(!pb_decode(&stream_cpy, sns_registry_data_item_fields, &temp_item))
    {
      SNS_PRINTF(ERROR, this,"Error in item decoding");
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "timestamp", item_name.buf_len))
  {
    if(reg_item.has_sint)
    {
      state->persist_state.output.timestamp= reg_item.sint;
      SNS_PRINTF(LOW, this, "Accel cal sensor received timestamp %d from registry",reg_item.sint);
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "accuracy", item_name.buf_len))
  {
    if(reg_item.has_sint)
    {
      state->persist_state.output.accuracy= reg_item.sint;
      SNS_PRINTF(LOW, this, "Accel cal sensor received accuracy %d from registry",reg_item.sint);
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "bias_0", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.bias[0]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received bias_0 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));

      // reading version of only one item is sufficient because all items would share the
      // same version
      if (reg_item.has_version)
      {
        state->registry_persist_version = reg_item.version;
        SNS_PRINTF(LOW, this, "Accel cal persist state version is %d",reg_item.version);
      }
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "bias_1", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.bias[1]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received bias_1 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "bias_2", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.bias[2]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received bias_2 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "sf_0", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.scale_factor[0]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received sf_0 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "sf_1", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.scale_factor[1]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received sf_1 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "sf_2", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.scale_factor[2]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received sf_1 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_00", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[0]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_00 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_01", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[1]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_01 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_02", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[2]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_02 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_10", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[3]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_10 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_11", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[4]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_11 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_12", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[5]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_12 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_20", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[6]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_20 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_21", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[7]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_21 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "cm_22", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.output.compensation_matrix[8]= reg_item.flt;
      SNS_PRINTF(LOW, this, "Accel cal sensor received cm_22 [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }

  return rv;
}

static bool
sns_accel_cal_parse_registry_config(pb_istream_t *stream,
                      const pb_field_t *field,
                      void **arg)
{
  UNUSED_VAR(field);
  sns_registry_data_item reg_item;
  pb_buffer_arg item_name = {0,0};
  reg_item.name.arg = &item_name;
  reg_item.name.funcs.decode = pb_decode_string_cb;
  bool rv = pb_decode(stream, sns_registry_data_item_fields, &reg_item);

  sns_sensor *this = (sns_sensor*)*arg;
  accel_cal_state *state =
     (accel_cal_state*)this->state->state;

  if(0 == strncmp((char*)item_name.buf, "sample_rate", item_name.buf_len))
  {
    if(reg_item.has_flt)
    {
      state->persist_state.config.sampleRate = reg_item.flt;
      SNS_PRINTF(LOW, this,
          "Accel cal sensor received sample rate [%d/1000] from registry",
          (int32_t)(reg_item.flt*1000));
    }
    else
    {
      rv = false;
    }
  }
  else if(0 == strncmp((char*)item_name.buf, "calibration_period", item_name.buf_len))
  {
    if(reg_item.has_sint)
    {
      state->persist_state.config.calibration_period = (uint16_t)reg_item.sint;
      SNS_PRINTF(LOW, this,
          "Accel cal sensor received calibration period %d from registry",
          state->persist_state.config.calibration_period );
    }
    else
    {
      rv = false;
    }
  }
  return rv;
}

static void
sns_accel_cal_handle_registry_event(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;

  if(NULL != state->registry_stream)
  {
    sns_sensor_event *event;
    event = state->registry_stream->api->peek_input(state->registry_stream);
    while(NULL != event)
    {
      if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
      {
        pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
                                                            event->event_len);
        sns_registry_read_event read_event = sns_registry_read_event_init_default;
        pb_buffer_arg group_name = {0,0};
        read_event.name.arg = &group_name;
        read_event.name.funcs.decode = pb_decode_string_cb;

        if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
        {
          SNS_PRINTF(ERROR, this, "Error in decoding registry event");
        }
        else
        {
          stream = pb_istream_from_buffer((void*)event->event, event->event_len);

          if(0 == strncmp((char*)group_name.buf, "sns_accel_cal_config",group_name.buf_len))
          {
            SNS_PRINTF(LOW, this, "DBG:Accel cal received sns_accel_cal_config");

            sns_registry_read_event read_event = sns_registry_read_event_init_default;
            pb_buffer_arg group_name = {0,0};
            read_event.name.arg = &group_name;
            read_event.name.funcs.decode = pb_decode_string_cb;
            read_event.data.items.arg = this;
            read_event.data.items.funcs.decode = sns_accel_cal_parse_registry_config;

            if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
            {
              SNS_PRINTF(ERROR, this, "Error in decoding registry event");
            }
            else
            {
              state->registry_configuration_received = true;
            }
          }
          else
          {
            //sns_accel_cal_persist_sX, X used for dual sensor
            char buffer[sizeof("sns_accel_cal_persist_s") + 1];
            snprintf(buffer, sizeof(buffer), "sns_accel_cal_persist_s%d", 
              state->persist_state.config.registration_index);
            if(0 == strncmp((char*)group_name.buf, buffer, group_name.buf_len))
            {
              SNS_PRINTF(LOW, this, "DBG:Accel cal received sns_accel_cal_persist_s%u",
                state->persist_state.config.registration_index);
              sns_registry_read_event read_event = sns_registry_read_event_init_default;
              pb_buffer_arg group_name = {0,0};
              read_event.name.arg = &group_name;
              read_event.name.funcs.decode = pb_decode_string_cb;
              read_event.data.items.arg = this;
              read_event.data.items.funcs.decode = sns_accel_cal_parse_registry_persist;
  
              if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
              {
                SNS_PRINTF(ERROR, this, "Error in decoding registry event");
              }
              else
              {
                state->registry_presist_received = true;
              }
            }
          }
        }
      }
      else
      {
        SNS_PRINTF(HIGH, this, "Unrecognized registry event: %u", event->message_id);
      }
      event = state->registry_stream->api->get_next_input(state->registry_stream);
    }
    if(
        state->registry_presist_received &&                   // should it depend of persist data ?
        state->registry_configuration_received
        )
    {
      // publish available as true when the algorithm is implemented
      /*
      sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.has_boolean = true;
      value.boolean     = true;
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
      SNS_PRINTF(HIGH, this, "Accel cal sensor available for use");
      */
    }
  }
}

/* See sns_sensor::notify_event */
static sns_rc
sns_accel_cal_notify_event(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;
  bool completed = sns_suid_lookup_complete(&state->suid_lookup_data);

  sns_suid_lookup_handle(this, &state->suid_lookup_data);
  sns_accel_cal_handle_registry_event(this);

  if(completed != sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    sns_sensor_uid suid;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_service = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    sns_suid_lookup_deinit(this, &state->suid_lookup_data);
    sns_suid_lookup_get(&state->suid_lookup_data, "registry", &suid);

    stream_service->api->create_sensor_stream(stream_service, this,
        suid, &state->registry_stream);
    if(NULL != state->registry_stream)
    {
      sns_accel_cal_send_registry_request(this, "sns_accel_cal_config");
      //sns_accel_cal_persist_sX, X used for dual sensor
      char buffer[sizeof("sns_accel_cal_persist_s") + 1];
      snprintf(buffer, sizeof(buffer), "sns_accel_cal_persist_s%d", 
        state->persist_state.config.registration_index);
      sns_accel_cal_send_registry_request(this, buffer);
    }
  }

  return SNS_RC_SUCCESS;
}

static bool suid_lookup_cb(struct sns_sensor *const sensor,
    char const *data_type, struct sns_sensor_event *event)
{
  accel_cal_state *state =
    (accel_cal_state*)sensor->state->state;
  pb_istream_t stream = pb_istream_from_buffer(
      (void*)event->event, event->event_len);
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;
  sns_sensor_util_attrib attrib = (sns_sensor_util_attrib)
    { .sensor = sensor };
  attrib.attr_id = SNS_STD_SENSOR_ATTRID_RIGID_BODY;
  bool ret = false;

  attr_event.attributes.funcs.decode = &sns_sensor_util_decode_attr;
  attr_event.attributes.arg = (void*)&attrib;

  if(0 == strcmp(data_type, "registry") || 0 == strcmp(data_type, "amd") || 
     0 == strcmp(data_type, "resampler"))
  {
    ret = true;  // Always accept first resampler and registry Sensors
  }
  else
  {
    if(!pb_decode(&stream, sns_std_attr_event_fields, &attr_event))
    {
      SNS_PRINTF(ERROR, sensor, "Error decoding attr event");
    }
    else if(SNS_STD_SENSOR_ATTRID_RIGID_BODY == attrib.attr_id)
    {
      return (state->persist_state.config.registration_index == attrib.attr_value);
    }
  }

  return ret;
}

/* See sns_sensor::init */
static sns_rc
sns_accel_cal_init(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;

  state->self_suid = this->sensor_api->get_sensor_uid(this);
  state->persist_state.config.registration_index = this->cb->get_registration_index(this);
  sns_accel_cal_publish_attributes(this);

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, &suid_lookup_cb);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "amd");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "accel");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "resampler");

  state->persist_state.config.calibration_period =
    SNS_ACCEL_CAL_CALIBRATION_PERIOD;
  state->persist_state.config.sampleRate =
    SNS_ACCEL_CAL_SAMPLE_RATE;

  // initialize bias and accuracy
  sns_memset(state->persist_state.output.bias,0,
              sizeof(state->persist_state.output.bias));
  state->persist_state.output.accuracy =
    SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;

  SNS_PRINTF(HIGH, this, "Accel cal sensor initialized");
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
sns_accel_cal_get_sensor_uid(sns_sensor const *this)
{
  accel_cal_state *state = (accel_cal_state *)this->state->state;
  static const sns_sensor_uid accel_cal_suid_0 = 
    {.sensor_uid = {SNS_ACCEL_CAL_SUID_BASE}};
  static const sns_sensor_uid accel_cal_suid_1 = 
    {.sensor_uid = {SNS_ACCEL_CAL_SUID_BASE + 1}};
  if(0 == state->persist_state.config.registration_index)
  {
    return &accel_cal_suid_0;
  }
  else
  {
    return &accel_cal_suid_1;
  }
}

static bool
encode_accel_cal_group_for_registry(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  pb_arg_accel_cal* pb_arg = (pb_arg_accel_cal*)*arg;
  accel_cal_state *state =
      (accel_cal_state*)pb_arg->sensor->state->state;

  if (0 == strncmp(pb_arg->name,"bias",strlen("bias")))
  {
    char const *names[] = {"bias_0", "bias_1", "bias_2"};

    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                      { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.flt = state->persist_state.output.bias[i];
      pb_item.has_version = true;
      pb_item.version = state->registry_persist_version;

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_PRINTF(ERROR,pb_arg->sensor,"Error in encoding item");
        return false;
      }
    }

  }
  else if (0 == strncmp(pb_arg->name,"scale_factor",sizeof("scale_factor")))
  {
    char const *names[] = {"sf_0", "sf_1", "sf_2"};
    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.flt = state->persist_state.output.scale_factor[i];
      pb_item.has_version = true;
      pb_item.version = state->registry_persist_version;

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_PRINTF(ERROR,pb_arg->sensor,"Error in encoding item");
        return false;
      }
    }
  }
  else if (0 == strncmp(pb_arg->name,"compensation_matrix",strlen("compensation_matrix")))
  {
    char const *names[] = {"cm_00", "cm_01", "cm_02",
                           "cm_10", "cm_11", "cm_12",
                           "cm_20", "cm_21", "cm_22",};
    for (int i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                             { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.flt = state->persist_state.output.compensation_matrix[i];
      pb_item.has_version = true;
      pb_item.version = state->registry_persist_version;

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_PRINTF(ERROR,pb_arg->sensor,"Error in encoding item");
        return false;
      }
    }
  }

  return true;
}


static bool
sns_send_to_registry_persist_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  sns_sensor *this = (sns_sensor*)*arg;
  accel_cal_state *state =
      (accel_cal_state*)this->state->state;


  SNS_PRINTF(HIGH, this,"DBG:Accel_cal updating version %d to for persist data ", state->registry_persist_version);

  char const *names[] = {"timestamp", "accuracy", "bias", "scale_factor","compensation_matrix"};

  for(int i = 0; i < ARR_SIZE(names); i++)
  {

    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_arg_accel_cal pb_arg= (pb_arg_accel_cal){
      .name = NULL,.sensor= this
    };

    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;
    if(0==strncmp(names[i],"timestamp",name_data.buf_len))
    {
      pb_item.has_sint = true;
      pb_item.sint = state->persist_state.output.timestamp;
      pb_item.has_version = true;
      pb_item.version = state->registry_persist_version;
    }
    else if (0==strncmp(names[i],"accuracy",name_data.buf_len))
    {
      pb_item.has_sint = true;
      pb_item.sint = state->persist_state.output.accuracy;
      pb_item.has_version = true;
      pb_item.version = state->registry_persist_version;
    }
    else if (0==strncmp(names[i],"bias",name_data.buf_len))
    {
      pb_arg.name = names[i];

      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_accel_cal_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;
    }
    else if (0==strncmp(names[i],"scale_factor",name_data.buf_len))
    {

      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_accel_cal_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;

    }
    else if (0==strncmp(names[i],"compensation_matrix",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_accel_cal_group_for_registry;
      pb_item.subgroup.items.arg = &pb_arg;

    }


    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error in encoding tag item");

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error in encoding item");
      return false;
    }
  }

  return true;


}
static sns_rc
sns_write_output_to_registry(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;


  uint8_t buffer[1000];
  int32_t encoded_len;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  //sns_accel_cal_persist_sX, X used for dual sensor
  char name[sizeof("sns_accel_cal_persist_s") + 1];
  snprintf(name, sizeof(name), "sns_accel_cal_persist_s%d", 
    state->persist_state.config.registration_index);
  pb_buffer_arg name_data = (pb_buffer_arg)
        { .buf = name, .buf_len = strlen(name) + 1 };

  write_req.name.funcs.encode = &pb_encode_string_cb;
  write_req.name.arg = &name_data;
  write_req.data.items.funcs.encode = &sns_send_to_registry_persist_cb;
  write_req.data.items.arg = this;

  // updating version number for persitent data
  state->registry_persist_version++;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &write_req, sns_registry_write_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
    state->registry_stream->api->send_request(state->registry_stream, &request);
  }
  return true;
}
/* See sns_sensor::set_client_request */
static sns_sensor_instance*
sns_accel_cal_set_client_request(sns_sensor *const this,
                                 sns_request const *exist_request,
                                 sns_request const *new_request,
                                 bool remove)
{
  SNS_PRINTF(HIGH, this,"Accel cal sensor set_client_request "
      "current (%p) new (%p) remove (%d)",exist_request,new_request,remove);
  accel_cal_state *state = (accel_cal_state*)this->state->state;
  sns_sensor_instance *ret_inst = sns_sensor_util_get_shared_instance(this);

  if (remove)
  {
    if (ret_inst != NULL)
    {
      ret_inst->cb->remove_client_request(ret_inst, exist_request);
    }
  }
  else
  {
    if (ret_inst == NULL)
    {
      ret_inst = this->cb->create_instance(this,sizeof(sns_accel_cal_inst_state));
    }
    else
    {
      if (exist_request != NULL && new_request != NULL)
      {
        ret_inst->cb->remove_client_request(ret_inst, exist_request);
      }
    }
    if (new_request != NULL && ret_inst != NULL)
    {
      ret_inst->cb->add_client_request(ret_inst, new_request);
      if(SNS_RC_SUCCESS != this->instance_api->set_client_config(ret_inst, new_request))
      {
        ret_inst->cb->remove_client_request(ret_inst, new_request);
      }

    }
  }

  if (ret_inst != NULL &&
      ret_inst->cb->get_client_request(ret_inst,state->self_suid, true) == NULL)
  {
    //copy the persistent state
    sns_accel_cal_inst_state *inst_state =
        (sns_accel_cal_inst_state*)ret_inst->state->state;

    sns_memscpy(&state->persist_state.output, sizeof(state->persist_state.output),
        &inst_state->persist_state.output, sizeof(inst_state->persist_state.output));
    SNS_PRINTF(LOW, this,"DBG: Accel cal instance removed, saving the persistent state");
    sns_write_output_to_registry(this);
    this->cb->remove_instance(ret_inst);
  }

  return ret_inst;
}

/* See sns_sensor::deinit */
static sns_rc
sns_accel_cal_deinit(sns_sensor *const this)
{
  accel_cal_state *state = (accel_cal_state*)this->state->state;

  sns_suid_lookup_deinit(this, &state->suid_lookup_data);
  sns_sensor_util_remove_sensor_stream(this,&state->registry_stream);
  return SNS_RC_SUCCESS;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_accel_cal_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_accel_cal_init,
  .deinit = &sns_accel_cal_deinit,
  .get_sensor_uid = &sns_accel_cal_get_sensor_uid,
  .set_client_request = &sns_accel_cal_set_client_request,
  .notify_event = &sns_accel_cal_notify_event,
};
