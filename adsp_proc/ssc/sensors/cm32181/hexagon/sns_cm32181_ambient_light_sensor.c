 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   3. Neither the name of Vishay Capella Microsystems nor the
*     names of its contributors may be used to endorse or promote products
*     derived from this software without specific prior written permission.
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
 * @file sns_cm32181_ambient_light_sensor.c
 *
 * CM32181 Ambient Light Sensor Implementation.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <string.h>
#include "sns_pb_util.h"
#include "sns_attribute_util.h"
#include "sns_cm32181_sensor.h"
#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_types.h"
#include "sns_cm32181_ver.h"

#include "sns_printf.h"

/**
 * Publish all Sensor attributes
 *
 * @param[i] this  reference to this Sensor
 *
 * @return none
 */
static void cm32181_publish_attributes(sns_sensor *const this)
{
	cm32181_state *state = (cm32181_state*)this->state->state;

	{
		sns_std_attr_value_data values[] = {SNS_ATTR};

		sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
		range1[0].has_flt = true;
		range1[0].flt = CM32181_ALS_RANGE_MIN;
		range1[1].has_flt = true;
		range1[1].flt = CM32181_ALS_RANGE_MAX;
		values[0].has_subtype = true;
		values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
		values[0].subtype.values.arg = &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = 35; //uA
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_flt = true;
		values[0].flt = CM32181_ODR;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_flt = true;
		values[0].flt = CM32181_ALS_RESOLUTION; //Lux
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS, values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		char const op_mode1[] = CM32181_NORMAL;

		values[0].str.funcs.encode = pb_encode_string_cb;
		values[0].str.arg = &((pb_buffer_arg){ .buf = op_mode1, .buf_len = sizeof(op_mode1) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,	values, ARR_SIZE(values), false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		char const proto1[] = "sns_ambient_light.proto";
		values[0].str.funcs.encode = pb_encode_string_cb;
		values[0].str.arg = &((pb_buffer_arg){ .buf = proto1, .buf_len = sizeof(proto1) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API, values, ARR_SIZE(values), false);
	}
	{
		char const name[] = "cm32181";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg){ .buf = name, .buf_len = sizeof(name) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
		}
	{
		char const type[] = "ambient_light";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg){ .buf = type, .buf_len = sizeof(type) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
	}
	{
		char const vendor[] = "vishay_capella";
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.str.funcs.encode = pb_encode_string_cb;
		value.str.arg = &((pb_buffer_arg){ .buf = vendor, .buf_len = sizeof(vendor) });
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = false;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = CM32181_DRIVER_VERSION;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = CM32181_MAX_FIFO; // samples
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = 1; //uA
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
	}
	{
		float data[2] = {0};
		state->encoded_event_len =
		pb_get_encoded_size_sensor_stream_event(data, 2);
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_sint = true;
		value.sint = state->encoded_event_len;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
	}
	{
		sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
		value.has_boolean = true;
		value.boolean = true;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, false);
	}
	{
		sns_std_attr_value_data values[] = {SNS_ATTR};
		values[0].has_sint = true;
		values[0].sint = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
		sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS, values, ARR_SIZE(values), true);
	}	
}

/* See sns_sensor::init */
sns_rc cm32181_ambient_light_init(sns_sensor *const this)
{
	cm32181_state *state = (cm32181_state*)this->state->state;
	struct sns_service_manager *smgr = this->cb->get_service_manager(this);
	state->diag_service = (sns_diag_service *)smgr->get_service(smgr, SNS_DIAG_SERVICE);
	state->scp_service = (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);

	SNS_PRINTF(MED, this, "cm32181_ambient_light_init: Enter");

	state->sensor = CM32181_AMBIENT_LIGHT;
	
	// Initialize factory calibration data
	state->ambient_light_fac_cal.scale = 1.0;
	state->ambient_light_fac_cal.bias = 0.0;	

	sns_memscpy(&state->my_suid,
				sizeof(state->my_suid),
				&((sns_sensor_uid)AMBIENT_LIGHT_SUID),
				sizeof(sns_sensor_uid));

	cm32181_send_suid_req(this, "timer", 6);
	cm32181_send_suid_req(this, "registry", 9);

	cm32181_publish_attributes(this);	
	
	SNS_PRINTF(MED, this, "cm32181_ambient_light_init: Exit");

	return SNS_RC_SUCCESS;
}

sns_rc cm32181_ambient_light_deinit(sns_sensor *const this)
{
	//UNUSED_VAR(this);

	SNS_PRINTF(MED, this, "cm32181_ambient_light_deinit: Enter");

	// Turn Sensor OFF.
	// Close COM port.
	// Turn Power Rails OFF.
	// No need to clear cm32181_state because it will get freed anyway.

	SNS_PRINTF(MED, this, "cm32181_ambient_light_deinit: Exit");

	return SNS_RC_SUCCESS;
}
