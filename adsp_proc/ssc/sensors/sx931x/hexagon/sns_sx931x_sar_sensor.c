/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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


#include <string.h>
#include "sns_mem_util.h"
#include "sns_types.h"
#include "sns_service_manager.h"
#include "sns_sx931x_sensor.h"
#include "sns_sx931x_ver.h"
#include "sns_pb_util.h"
#include "sns_attribute_util.h"

/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void sx931x_publish_attributes(sns_sensor *const this)
{
    sx931x_state *state = (sx931x_state *)this->state->state;
    {
        sns_std_attr_value_data values[] = { SNS_ATTR };
        sns_std_attr_value_data range1[] = { SNS_ATTR, SNS_ATTR };
        range1[0].has_flt = true;
        range1[0].flt = SX931x_SENSOR_SAR_RANGE_MIN;
        range1[1].has_flt = true;
        range1[1].flt = SX931x_SENSOR_SAR_RANGE_MAX;
        values[0].has_subtype = true;
        values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
        values[0].subtype.values.arg =
            &((pb_buffer_arg )
        {
            .buf = range1, .buf_len = ARR_SIZE(range1)
        } );
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;//SNS_STD_SENSOR_STREAM_TYPE_STREAMING;   //SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint = SX931x_SENSOR_SAR_LOW_POWER_CURRENT;
        values[1].has_sint = true;
        values[1].sint = SX931x_SENSOR_SAR_NORMAL_POWER_CURRENT;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt = SMT_ODR_1;
        values[1].has_flt = true;
        values[1].flt = SMT_ODR_5;
        values[2].has_flt = true;
        values[2].flt = SMT_ODR_10;
        values[3].has_flt = true;
        values[3].flt = SMT_ODR_25;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt = SX931x_SENSOR_SAR_RESOLUTION;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
        char const op_mode1[] = SMT_LPM;
        char const op_mode2[] = SMT_NORMAL;

        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg = &((pb_buffer_arg)
        {
            .buf = op_mode1, .buf_len = sizeof(op_mode1)
        });
        values[1].str.funcs.encode = pb_encode_string_cb;
        values[1].str.arg = &((pb_buffer_arg)
        {
            .buf = op_mode2, .buf_len = sizeof(op_mode2)
        });

        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        char const proto1[] = "sns_sar.proto";//"sns_accel.proto";//"sns_proximity.proto";//"sns_pressure.proto";"sns_accel.proto"
        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg = &((pb_buffer_arg)
        {
            .buf = proto1, .buf_len = sizeof(proto1)
        });
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
#if DUAL_SENSOR_SUPPORT
        char const name_1[] = "semtech_sx931x_1";
        char const name_0[] = "semtech_sx931x_0";
        if (state->hw_idx)
        {
            value.str.arg = &((pb_buffer_arg)
            {
                .buf = name_1, .buf_len = sizeof(name_1)
            });
        }
        else
        {
            value.str.arg = &((pb_buffer_arg)
            {
                .buf = name_0, .buf_len = sizeof(name_0)
            });
        }
#else
        char const name_0[] = "semtech_sx931x_0";
        value.str.arg = &((pb_buffer_arg)
        {
            .buf = name_0, .buf_len = sizeof(name_0)
        });
#endif
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
    }
    {
        char const type[] = "SAR";//"accel"; // "SAR";//"accel";//"proximity";////pressure
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg = &((pb_buffer_arg)
        {
            .buf = type, .buf_len = sizeof(type)
        });
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
    }
    {
        char const vendor[] = "Semtech";
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg = &((pb_buffer_arg)
        {
            .buf = vendor, .buf_len = sizeof(vendor)
        });
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
        value.has_sint = true;
        value.sint = SX931x_SEE_DD_VER;
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
        value.sint = SX931x_SENSOR_SAR_SLEEP_CURRENT;
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1, false);
    }
    {
        float data[1] = {0};
        state->encoded_event_len =
            pb_get_encoded_size_sensor_stream_event(data, 1);
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = state->encoded_event_len;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = true;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1, true);
    }
}

sns_rc sns_see_sx931x_sar_init(sns_sensor *const this)
{
    sx931x_state *state = (sx931x_state *) this->state->state;
    struct sns_service_manager *smgr = this->cb->get_service_manager(this);
    state->diag_service = (sns_diag_service *) smgr->get_service(smgr, SNS_DIAG_SERVICE);

    state->scp_service = (sns_sync_com_port_service *)
                         smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
    state->sensor = SX9XXX_SAR;
    state->sensor_client_present = false;

    sns_memscpy(&state->my_suid, sizeof(state->my_suid),
                &((sns_sensor_uid)SAR_SUID_0),
                sizeof(sns_sensor_uid));
    state->hw_idx = 0;
#if DUAL_SENSOR_SUPPORT
    state->hw_idx = this->cb->get_registration_index(this);
    if(state->hw_idx)
    {
        sns_memscpy(&state->my_suid, sizeof(state->my_suid),
                    &((sns_sensor_uid)SAR_SUID_1),
                    sizeof(sns_sensor_uid));
    }
#endif
    sx931x_publish_attributes(this);
    SX931x_PRINTF(LOW, this, "<sns_see_if__ init>sar sensor init");
    //sns_see_sx931x_send_suid_req(this, "data_acquisition_engine", sizeof("data_acquisition_engine"));
    sns_see_sx931x_send_suid_req(this, "async_com_port", 15);
    sns_see_sx931x_send_suid_req(this, "timer", 6);
    sns_see_sx931x_send_suid_req(this, "interrupt", 10);
    sns_see_sx931x_send_suid_req(this, "registry", 9);
    return SNS_RC_SUCCESS;
}

sns_rc sns_see_sx931x_sar_deinit(sns_sensor *const this)
{
    UNUSED_VAR(this);
    return SNS_RC_SUCCESS;
}
