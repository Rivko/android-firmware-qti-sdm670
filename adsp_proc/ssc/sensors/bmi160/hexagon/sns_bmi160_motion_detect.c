/*******************************************************************************
 * Copyright (c) 2017-18, Bosch Sensortec GmbH
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
 * @file sns_bmi160_motion_detect.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_math_util.h"
#include "sns_types.h"
#include "sns_bmi160_sensor.h"
#include "pb_encode.h"
#include "sns_service_manager.h"
#include "sns_attribute_util.h"
#include "sns_pb_util.h"
#include "sns_bmi160_ver.h"

/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
    static void
bmi160_motion_detect_publish_attributes(sns_sensor *const this)
{
#if !BMI160_CONFIG_ENABLE_SEE_LITE
    bmi160_state *sstate = (bmi160_state*)this->state->state;
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
                values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint = BMI160_MOTION_DETECTION_ACTIVE_CURRENT;    //50hz, avg=1, downsampling
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
                values, ARR_SIZE(values), false);
    }

    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        char const op_mode1[] = BMI160_HIGH_PERF;

        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg = &((pb_buffer_arg)
                { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
                values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        char const proto1[] = "sns_motion_detect.proto";

        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg = &((pb_buffer_arg)
                { .buf = proto1, .buf_len = sizeof(proto1) });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
                values, ARR_SIZE(values), false);
    }
    {
        char const name[] = "BMI160_MOTION_DETECTOR";
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg = &((pb_buffer_arg)
                { .buf = name, .buf_len = sizeof(name) });
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
    }
#endif
    {
        char const type[] = "motion_detect";
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

#if !BMI160_CONFIG_ENABLE_SEE_LITE
    {
        char const vendor[] = "BOSCH";
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
        value.sint = BMI160_SEE_DD_ATTRIB_VERSION;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = 0;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = BMI160_MOTION_DETECTION_LOWPOER_CURRENT; //uA
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        sns_motion_detect_event stream_event =
            sns_motion_detect_event_init_default;
        pb_get_encoded_size(&sstate->encoded_event_len,
                sns_motion_detect_event_fields,
                &stream_event);
        value.has_sint = true;
        value.sint = sstate->encoded_event_len;
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
      sns_std_attr_value_data values[] = {SNS_ATTR};
      values[0].has_sint = true;
      values[0].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
      sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS,
          values, ARR_SIZE(values), true);
    }
#endif
}

/* See sns_sensor::init */
sns_rc bmi160_motion_detect_init(sns_sensor * const this)
{
  bmi160_state *sstate = (bmi160_state *) this->state->state;

  sstate->sensor = BMI160_MOTION_DETECT;
  sns_memscpy(&sstate->my_suid,
              sizeof(sstate->my_suid),
              &((sns_sensor_uid) MOTION_DETECT_SUID ),
              sizeof(sns_sensor_uid));

  bmi160_common_init(this);
  bmi160_motion_detect_publish_attributes(this);

  BMI160_SENSOR_LOG(MED, this, "<bmi160_if_ motion_detection_init>");

  return SNS_RC_SUCCESS;
}

sns_rc bmi160_motion_detect_deinit(sns_sensor * const this)
{
  UNUSED_VAR(this);
#if !BMI160_CONFIG_ENABLE_SEE_LITE
  bmi160_state *sstate = (bmi160_state*)this->state->state;
  UNUSED_VAR(sstate);
#endif
  BMI160_SENSOR_LOG(MED, this, "<bmi160_if_ md_deinit>");
  return SNS_RC_SUCCESS;
}

