/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "sns_tmx4903_sensor.h"

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS // entire file

#if !defined(AMS_DRIVER_VERSION) && !defined(AMS_DEFAULT_DRIVER_VERSION)
#define AMS_DEFAULT_DRIVER_VERSION  0x0100
#define AMS_DRIVER_VERSION          AMS_DEFAULT_DRIVER_VERSION
#endif

extern const ams_calibrationData_t default_common_cal_data;

/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
ams_publish_attributes(sns_sensor *const this, sns_std_sensor_stream_type stream_type)
{
  ams_deviceInfo_t info;
  ams_getDeviceInfo(&info);

  ams_state *state = (ams_state*)this->state->state;
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};

    sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
    range1[0].has_flt = true;
    range1[0].flt = info.alsSensor.rangeMin;
    range1[1].has_flt = true;
    range1[1].flt = info.alsSensor.rangeMax;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};

    sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
    range1[0].has_flt = true;
    range1[0].flt = info.alsSensor.rangeMin;
    range1[1].has_flt = true;
    range1[1].flt = info.alsSensor.rangeMax;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE,
      values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = stream_type;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
        values, ARR_SIZE(values), false);
  }
  {
   sns_std_attr_value_data values[] = {SNS_ATTR};
   values[0].has_sint = true;
   values[0].sint = info.alsSensor.activeCurrent_uA;
   sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
       values, ARR_SIZE(values), false);
  }
  {
   sns_std_attr_value_data values[] = {SNS_ATTR};
   values[0].has_sint = true;
   values[0].sint = info.alsSensor.standbyCurrent_uA;
   sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT,
       values, ARR_SIZE(values), false);
  }

  if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_STREAMING)
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = AMS_DEVICE_ODR_1;
    values[1].has_flt = true;
    values[1].flt = AMS_DEVICE_ODR_2;
    values[2].has_flt = true;
    values[2].flt = AMS_DEVICE_ODR_5;
    values[3].has_flt = true;
    values[3].flt = AMS_DEVICE_ODR_10;

    state->odr_for_selftest = AMS_DEVICE_ODR_10;

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
        values, ARR_SIZE(values), false);
  } else {
    /* _ATTRID_RATES not supported; this is an on-change sensor */
    state->odr_for_selftest = 0.0;
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = 0.001;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    char const op_mode1[] = AMS_OPMODE_NORMAL;

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = op_mode1, .buf_len = sizeof(op_mode1) });

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    char const proto1[] = "sns_ambient_light.proto";

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = proto1, .buf_len = sizeof(proto1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
        values, ARR_SIZE(values), false);
  }
  {
    char const name[] = STRINGIFY(CONFIG_AMS_PART_NAME);
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "ambient_light";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "AMS";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = (stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE) ? true : false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = AMS_DRIVER_VERSION;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 0;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    float data[ALS_PROTO_STD_ARRAY_LEN] = {0};
    state->encoded_event_len =
        pb_get_encoded_size_sensor_stream_event(data, ARR_SIZE(data));
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->encoded_event_len;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, true);
  }
}

/* See sns_sensor::init */
static sns_rc _common_ams_als_init(sns_sensor *const this, sns_std_sensor_stream_type stream_type)
{
  ams_state *state = (ams_state*)this->state->state;
  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service *)smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service =
     (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
  sns_diag_service* diag = state->diag_service;

  state->calibration_data = default_common_cal_data;
  /* our private wrapped-up copies of the com port's handles etc. */
  state->port_handle.scp_service = state->scp_service;
  /* state->port_handle.scp_handle not available until we call
         scp_service->api->sns_scp_register_com_port() */
  state->port_handle_ptr = &state->port_handle;
  // for debug use - allows called fn to call sensor_printf
  state->port_handle.diag = diag;

  state->sensor_client_present = false;

  if (stream_type == SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE)
  {
    state->sensor_type = AMS_ALS_OC;
    sns_memscpy(&state->my_suid,
                sizeof(state->my_suid),
                &((sns_sensor_uid)ALS_SUID),
                sizeof(sns_sensor_uid));
  }
  else
  {
    state->sensor_type = AMS_ALS_STRM;
    sns_memscpy(&state->my_suid,
                sizeof(state->my_suid),
                &((sns_sensor_uid)ALS_STREAMING_SUID),
                sizeof(sns_sensor_uid));
  }

  ams_publish_attributes(this, stream_type);

  SNS_PRINTF(LOW, this
             , " AMS: _common_ams_als_init: this %p, stream_type %d: state %p, scp_service %p"
             , this
             , stream_type
             , state
             , state->scp_service
             );

  ams_send_suid_req(this, "interrupt", sizeof("interrupt"));
  ams_send_suid_req(this, "timer", sizeof("timer"));
  ams_send_suid_req(this, "registry", sizeof("registry"));

  SNS_PRINTF(LOW, this, " AMS: ams_als_init: end");
  return SNS_RC_SUCCESS;
}
sns_rc ams_als_init(sns_sensor *const this)
{
  return _common_ams_als_init(this, SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE);
}
sns_rc ams_als_streaming_init(sns_sensor *const this)
{
  return _common_ams_als_init(this, SNS_STD_SENSOR_STREAM_TYPE_STREAMING);
}

sns_rc ams_als_deinit(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS
