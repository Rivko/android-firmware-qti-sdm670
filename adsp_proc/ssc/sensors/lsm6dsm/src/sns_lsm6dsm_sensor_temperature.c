/**
 * @file sns_lsm6dsm_sensor_temperature.c
 *
 * LSM6DSM Sensor temperature implementation.
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **/
#include <string.h>
#include "sns_mem_util.h"
#include "sns_types.h"
#include "sns_service_manager.h"
#include "sns_lsm6dsm_sensor.h"
#include "sns_pb_util.h"
#include "pb_encode.h"
#include "sns_attribute_util.h"
#include "sns_pb_util.h"

/**
 * Publish all sensor-specific attributes.
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
void lsm6dsm_temp_publish_attributes(sns_sensor *const this)
{
  //set non-default attributes
  lsm6dsm_state *state = (lsm6dsm_state*)this->state->state;

  const char type[] = "sensor_temperature";
  const float resolutions[] =
  {
    LSM6DSM_SENSOR_TEMPERATURE_RESOLUTION,
  };
  const uint32_t active_current[3] = {24, 70, 240}; //uA
  const uint32_t sleep_current = 6; //uA

  lsm6dsm_publish_def_attributes(this);
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = LSM6DSM_SENSOR_TEMP_ODR_1;
    values[1].has_flt = true;
    values[1].flt = LSM6DSM_SENSOR_TEMP_ODR_5;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    int i;
    for(i = 0; i < ARR_SIZE(resolutions); i++)
    {
      values[i].has_flt = true;
      values[i].flt = resolutions[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
        values, i, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR};
    int i;
    for(i = 0; i < ARR_SIZE(active_current); i++)
    {
      values[i].has_sint = true;
      values[i].sint = active_current[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
        values, i, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = sleep_current; //uA
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
  }
  {
    float data[3] = {0};
    state->encoded_event_len =
        pb_get_encoded_size_sensor_stream_event(data, 1);
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->encoded_event_len;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 0;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    char const proto1[] = "sns_sensor_temperature.proto";
    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = proto1, .buf_len = sizeof(proto1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};

    sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
    range1[0].has_flt = true;
    range1[0].flt = LSM6DSM_SENSOR_TEMPERATURE_RANGE_MIN;
    range1[1].has_flt = true;
    range1[1].flt = LSM6DSM_SENSOR_TEMPERATURE_RANGE_MAX;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
        values, ARR_SIZE(values), true);
  }
}

/* See sns_sensor::init */
sns_rc lsm6dsm_sensor_temp_init(sns_sensor *const this)
{
  lsm6dsm_state *state = (lsm6dsm_state*)this->state->state;
  sns_sensor_uid* suid = &((sns_sensor_uid)SENSOR_TEMPERATURE_SUID_0);
#if DUAL_SENSOR_SUPPORT
  state->hw_idx = this->cb->get_registration_index(this);
  if(state->hw_idx)
    suid = &((sns_sensor_uid)SENSOR_TEMPERATURE_SUID_1);
#endif

  lsm6dsm_temp_publish_attributes(this);
  lsm6dsm_init_sensor_info(this, suid,
                           LSM6DSM_SENSOR_TEMP);

  // initialize fac cal correction matrix to identity
  // sensor temperature only uses
  // state->fac_cal_corr_mat.e00 for scaling and
  // state->fac_cal_bias[0] for bias correction
  // of it's single axis output
  state->fac_cal_corr_mat.e00 = 1.0;
  state->fac_cal_corr_mat.e11 = 1.0;
  state->fac_cal_corr_mat.e22 = 1.0;

  DBG_PRINT_EX(state->diag_service, this, LOW, __FILENAME__, __LINE__, "temp init");
  return SNS_RC_SUCCESS;
}

sns_rc lsm6dsm_sensor_temp_deinit(sns_sensor *const this)
{
  lsm6dsm_state *state = (lsm6dsm_state*)this->state->state;
  // Turn Sensor OFF.
  // Close COM port.
  // Turn Power Rails OFF.
  // No need to clear lsm6dsm_state because it will get freed anyway.

  DBG_PRINT_EX(state->diag_service, this, LOW, __FILENAME__, __LINE__, "temp deinit");
  return SNS_RC_SUCCESS;
}

