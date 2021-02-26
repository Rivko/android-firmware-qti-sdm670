/*
 * Copyright (c) 2018, ams AG
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

#include <string.h>
#include "pb_encode.h"
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_service_manager.h"
#include "sns_types.h"
#include "sns_printf.h"

#include "sns_tmd2725_sensor.h"

/**
 * Publish ALS Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void tmd2725_als_publish_attributes(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;
	{
		char const name[] = "tmd2725";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg)
			{.buf = name, .buf_len = sizeof(name)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_NAME,
			&value, 1, false);
	}
	{
		char const vendor[] = "ams AG";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg)
			{.buf = vendor, .buf_len = sizeof(vendor)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VENDOR,
			&value, 1, false);
	}
	{
		char const type[] = "ambient_light";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg)
			{.buf = type, .buf_len = sizeof(type)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = TMD2725_DRIVER_VERSION;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VERSION,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		char const proto1[] = "sns_ambient_light.proto";
		values[0].str.funcs.encode = pb_encode_string_cb;
		values[0].str.arg = &((pb_buffer_arg)
			{.buf = proto1, .buf_len = sizeof(proto1)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_flt = true;
		values[0].flt = TMD2725_ALS_RESOLUTION;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = TMD2725_ALS_ACTIVE_CURRENT;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = TMD2725_ALS_SLEEP_CURRENT;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
		range1[0].has_flt = true;
		range1[0].flt = TMD2725_ALS_RANGE_MIN;
		range1[1].has_flt = true;
		range1[1].flt = TMD2725_ALS_RANGE_MAX;
		values[0].has_subtype = true;
		values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
		values[0].subtype.values.arg = &((pb_buffer_arg)
			{.buf = range1, .buf_len = ARR_SIZE(range1)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
			values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
		char const op_mode0[] = TMD2725_LPM;
		char const op_mode1[] = TMD2725_NORMAL;
		values[0].str.funcs.encode = pb_encode_string_cb;
		values[0].str.arg = &((pb_buffer_arg)
			{.buf = op_mode0, .buf_len = sizeof(op_mode0)});
		values[1].str.funcs.encode = pb_encode_string_cb;
		values[1].str.arg = &((pb_buffer_arg)
			{.buf = op_mode1, .buf_len = sizeof(op_mode1)});

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
			values, ARR_SIZE(values), false);
	}
	{
		float data[ALS_EVENT_SIZE] = {0};
		state->als_encoded_event_len =
			pb_get_encoded_size_sensor_stream_event(data, ALS_EVENT_SIZE);
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = state->als_encoded_event_len;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = false;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DYNAMIC,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = true;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR,
			&value, 1, false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
		values[1].has_sint = true;
		values[1].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;

		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS,
			values, ARR_SIZE(values), true);
	}
}

sns_rc tmd2725_als_init(sns_sensor *const this)
{
	tmd2725_state *state = (tmd2725_state*)this->state->state;

	state->sensor = TMD2725_ALS;
	sns_memscpy(&state->my_suid,
			sizeof(state->my_suid),
			&((sns_sensor_uid)TMD2725_ALS_SUID),
			sizeof(sns_sensor_uid));
	tmd2725_common_init(this);
	tmd2725_als_publish_attributes(this);

	return SNS_RC_SUCCESS;
}

sns_rc tmd2725_als_deinit(sns_sensor *const this)
{
	UNUSED_VAR(this);
	
	return SNS_RC_SUCCESS;
}
