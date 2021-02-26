/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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
 *******************************************************************************/

/**
 * @file sns_bmi160.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_types.h"
#include "sns_service_manager.h"
#include "sns_bmg160_sensor.h"
#include "pb_encode.h"
#include "sns_attribute_util.h"
#include "sns_pb_util.h"
#include "sns_bmg160_ver.h"

#include "sns_bmg160_cfg.h"
/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
bmg160_gyro_publish_attributes(sns_sensor *const this)
{
#if !BMG160_CONFIG_ENABLE_SEE_LITE
  bmg160_state *state = (bmg160_state*)this->state->state;
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};

     sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
     range1[0].has_flt = true;
     range1[0].flt = BMG160_GYRO_RANGE_125_MIN;
     range1[1].has_flt = true;
     range1[1].flt = BMG160_GYRO_RANGE_125_MAX;
     values[0].has_subtype = true;
     values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[0].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

     sns_std_attr_value_data range2[] = {SNS_ATTR, SNS_ATTR};
     range2[0].has_flt = true;
     range2[0].flt = BMG160_GYRO_RANGE_250_MIN;
     range2[1].has_flt = true;
     range2[1].flt = BMG160_GYRO_RANGE_250_MAX;
     values[1].has_subtype = true;
     values[1].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[1].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range2, .buf_len = ARR_SIZE(range2) });

     sns_std_attr_value_data range3[] = {SNS_ATTR, SNS_ATTR};
     range3[0].has_flt = true;
     range3[0].flt = BMG160_GYRO_RANGE_500_MIN;
     range3[1].has_flt = true;
     range3[1].flt = BMG160_GYRO_RANGE_500_MAX;
     values[2].has_subtype = true;
     values[2].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[2].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range3, .buf_len = ARR_SIZE(range3) });

     sns_std_attr_value_data range4[] = {SNS_ATTR, SNS_ATTR};
     range4[0].has_flt = true;
     range4[0].flt = BMG160_GYRO_RANGE_1000_MIN;
     range4[1].has_flt = true;
     range4[1].flt = BMG160_GYRO_RANGE_1000_MAX;
     values[3].has_subtype = true;
     values[3].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[3].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range4, .buf_len = ARR_SIZE(range4) });

     sns_std_attr_value_data range5[] = {SNS_ATTR, SNS_ATTR};
     range5[0].has_flt = true;
     range5[0].flt = BMG160_GYRO_RANGE_2000_MIN;
     range5[1].has_flt = true;
     range5[1].flt = BMG160_GYRO_RANGE_2000_MAX;
     values[4].has_subtype = true;
     values[4].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[4].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range5, .buf_len = ARR_SIZE(range5) });

     sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
         values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};

     sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
     range1[0].has_flt = true;
     range1[0].flt = BMG160_GYRO_RANGE_125_MIN;
     range1[1].has_flt = true;
     range1[1].flt = BMG160_GYRO_RANGE_125_MAX;
     values[0].has_subtype = true;
     values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[0].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });

     sns_std_attr_value_data range2[] = {SNS_ATTR, SNS_ATTR};
     range2[0].has_flt = true;
     range2[0].flt = BMG160_GYRO_RANGE_250_MIN;
     range2[1].has_flt = true;
     range2[1].flt = BMG160_GYRO_RANGE_250_MAX;
     values[1].has_subtype = true;
     values[1].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[1].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range2, .buf_len = ARR_SIZE(range2) });

     sns_std_attr_value_data range3[] = {SNS_ATTR, SNS_ATTR};
     range3[0].has_flt = true;
     range3[0].flt = BMG160_GYRO_RANGE_500_MIN;
     range3[1].has_flt = true;
     range3[1].flt = BMG160_GYRO_RANGE_500_MAX;
     values[2].has_subtype = true;
     values[2].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[2].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range3, .buf_len = ARR_SIZE(range3) });

     sns_std_attr_value_data range4[] = {SNS_ATTR, SNS_ATTR};
     range4[0].has_flt = true;
     range4[0].flt = BMG160_GYRO_RANGE_1000_MIN;
     range4[1].has_flt = true;
     range4[1].flt = BMG160_GYRO_RANGE_1000_MAX;
     values[3].has_subtype = true;
     values[3].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[3].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range4, .buf_len = ARR_SIZE(range4) });

     sns_std_attr_value_data range5[] = {SNS_ATTR, SNS_ATTR};
     range5[0].has_flt = true;
     range5[0].flt = BMG160_GYRO_RANGE_2000_MIN;
     range5[1].has_flt = true;
     range5[1].flt = BMG160_GYRO_RANGE_2000_MAX;
     values[4].has_subtype = true;
     values[4].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
     values[4].subtype.values.arg =
       &((pb_buffer_arg){ .buf = range5, .buf_len = ARR_SIZE(range5) });

     sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE,
         values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
        values, ARR_SIZE(values), false);
  }


  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR
#if BMG160_HIGHEST_ODR
        , SNS_ATTR
#endif
    };
    values[0].has_flt = true;
    values[0].flt = BMG160_GYRO_ODR_100HZ;
    values[1].has_flt = true;
    values[1].flt = BMG160_GYRO_ODR_200HZ;
#if BMG160_HIGHEST_ODR
    values[2].has_flt = true;
    values[2].flt = BMG160_HIGHEST_ODR;
#endif
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
        values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = BMG160_GYRO_ODR_1000HZ;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES,
        values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    values[0].has_flt = true;
    values[0].flt = BMG160_SSTVT_125DPS;
    values[1].has_flt = true;
    values[1].flt = BMG160_SSTVT_250DPS;
    values[2].has_flt = true;
    values[2].flt = BMG160_SSTVT_500DPS;
    values[3].has_flt = true;
    values[3].flt = BMG160_SSTVT_1000DPS;
    values[4].has_flt = true;
    values[4].flt = BMG160_SSTVT_2000DPS;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
        values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
    static char const op_mode1[] = BMG160_LPM;
    static char const op_mode2[] = BMG160_NORMAL;

    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
    values[1].str.funcs.encode = pb_encode_string_cb;
    values[1].str.arg = &((pb_buffer_arg)
        { .buf = op_mode2, .buf_len = sizeof(op_mode2) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
        values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = BMG160_LOW_POWER_CURRENT;
    values[1].has_sint = true;
    values[1].sint = BMG160_ACTIVE_CURRENT;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
        values, ARR_SIZE(values), false);
  }

  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    static char const proto1[] = "sns_gyro.proto";
    values[0].str.funcs.encode = pb_encode_string_cb;
    values[0].str.arg = &((pb_buffer_arg)
        { .buf = proto1, .buf_len = sizeof(proto1) });
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
        values, ARR_SIZE(values), false);
  }

  {
    static char const name[] = "BMG160";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
#endif
  {
    static char const type[] = "gyro";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }

  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }

#if !BMG160_CONFIG_ENABLE_SEE_LITE
  {
    static char const vendor[] = "BOSCH";
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
        this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = BMG160_SNS_SEE_ATTRIB_VERSION;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = false;
#if BMG160_CONFIG_ENABLE_FIFO
    value.sint = BMG160_MAX_FIFO_LEVEL;
#else
    value.sint = 0; // samples
#endif
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
  }

  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = BMG160_LOW_POWER_CURRENT; //uA
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
  }
  {
    float data[3] = {0};
    state->encoded_event_len =
        pb_get_encoded_size_sensor_stream_event(data, 3);
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
        this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
    values[0].has_sint = true;
    values[0].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
    values[1].has_sint = true;
    values[1].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS,
        values, ARR_SIZE(values), true);
  }
#endif
}

/* See sns_sensor::init */
sns_rc bmg160_gyro_init(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;

  state->sensor = BMG160_GYRO;
  sns_memscpy(&state->my_suid,
              sizeof(state->my_suid),
              &((sns_sensor_uid)GYRO_SUID),
              sizeof(sns_sensor_uid));

  bmg160_common_init(this);
  
  bmg160_gyro_publish_attributes(this);

  BMG160_PRINTF(MED, this, "<bmg160_sif_ > gyr init");
  return SNS_RC_SUCCESS;
}

/** See sns_sensor.h */
sns_rc bmg160_gyro_deinit(sns_sensor *const this)
{
#if !BMG160_CONFIG_ENABLE_SEE_LITE
#if BMG160_CONFIG_ENABLE_DEBUG
  BMG160_PRINTF(MED, this, "<bmg160_sif_ > gyr deinit");
#else
  UNUSED_VAR(this);
#endif

#else
  UNUSED_VAR(this);
#endif
  return SNS_RC_SUCCESS;
}

