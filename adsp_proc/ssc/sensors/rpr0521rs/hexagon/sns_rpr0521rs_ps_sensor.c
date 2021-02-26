//==============================================================================
// MODULE        : sns_rpr0521rs_ps_sensor.c
// FUNCTION      : RPR0521RS Proximity Sensor
// AUTHOR        : Masafumi Seike
// MAKING DATE   : NOV/17/2017
// MODIFIED      : Ryosuke Yamamoto
// MODIFIED DATE : Dec/4/2018
// REMARKS       :
// COPYRIGHT     : Copyright (C) 2017 - 2018 - ROHM CO.,LTD.
//               : Redistribution and use in source and binary forms, with or
//               : without modification, are permitted provided that the
//               : following conditions are met:
//               : 1. Redistributions of source code must retain the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer.
//               : 2. Redistributions in binary form must reproduce the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer in the documentation and/or other materials
//               : provided with the distribution.
//               : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//               : contributors may be used to endorse or promote products
//               : derived from this software without specific prior written
//               : permission.
//               : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//               : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//               : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//               : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//               : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//               : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//               : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//               : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//               : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//               : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//               : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//               : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//               : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//               : SUCH DAMAGE.
//==============================================================================

//==============================================================================
// INCLUDE FILES
//==============================================================================
#include <math.h>
#include "sns_rpr0521rs_sensor.h"
#include "sns_std_type.pb.h"
#include "sns_printf.h"
#include "sns_attribute_util.h"
#include "sns_pb_util.h"
#include "sns_sensor_util.h"
#include "sns_rpr0521rs_ver.h"

//==============================================================================
// function prototype
//==============================================================================
// internal use
static void rpr0521rs_publish_attributes(sns_sensor *const this);

//==============================================================================
// NAME      : rpr0521rs_ps_init
// FUNCTION  : initialize sensor function
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_ps_init(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    state->sensor = RPR0521RS_PS;
    
    (void)sns_memscpy(&state->my_suid, sizeof(state->my_suid), &((sns_sensor_uid)PS_SUID), sizeof(sns_sensor_uid));
    rpr0521rs_common_init(this);
    rpr0521rs_publish_attributes(this);
    
    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : rpr0521rs_ps_deinit
// FUNCTION  : close sensor function
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_ps_deinit(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    if (state->reg_data_stream != NULL) {
        (void)sns_sensor_util_remove_sensor_stream(this, &state->reg_data_stream);
    }

    return SNS_RC_SUCCESS;
}

//==============================================================================
// NAME      : rpr0521rs_publish_attributes
// FUNCTION  : publish sensor attributes
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_publish_attributes(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;

    {
        sns_std_attr_value_data values[] = {SNS_ATTR};

        sns_std_attr_value_data range1[] = {SNS_ATTR, SNS_ATTR};
        range1[0].has_flt                     = true;
        range1[0].flt                         = 0.0F; // cm
        range1[1].has_flt                     = true;
        range1[1].flt                         = 5.0F; // cm
        values[0].has_subtype                 = true;
        values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
        values[0].subtype.values.arg          = &((pb_buffer_arg){ .buf = range1, .buf_len = ARR_SIZE(range1) });
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RANGES, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_sint = true;
        values[0].sint     = RPR0521RS_PS_ACTIVE_CURRENT;  //uA
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_ACTIVE_CURRENT, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt     = RPR0521RS_SENSOR_PS_ODR_0;
        values[1].has_flt = true;
        values[1].flt     = RPR0521RS_SENSOR_PS_ODR_10;
        values[2].has_flt = true;
        values[2].flt     = RPR0521RS_SENSOR_PS_ODR_20;
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RATES, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        values[0].has_flt = true;
        values[0].flt     = 1.0F;
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RESOLUTIONS, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        char const op_mode1[] = RPR0521RS_HIGH_PERF;
        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg          = &((pb_buffer_arg) { .buf = op_mode1, .buf_len = sizeof(op_mode1) });
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_OP_MODES, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        char const proto1[] = "sns_proximity.proto";
        values[0].str.funcs.encode = pb_encode_string_cb;
        values[0].str.arg          = &((pb_buffer_arg) { .buf = proto1, .buf_len = sizeof(proto1) });
        
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_API, values, ARR_SIZE(values), false);
    }
    {
        char const name[] = "rpr0521rs";
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg) { .buf = name, .buf_len = sizeof(name) });
        
        sns_publish_attribute( this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1U, false);
    }
    {
        char const type[] = "proximity";
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg) { .buf = type, .buf_len = sizeof(type) });
        sns_publish_attribute( this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1U, false);
    }
    {
        char const vendor[] = "ROHM";
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.str.funcs.encode = pb_encode_string_cb;
        value.str.arg          = &((pb_buffer_arg) { .buf = vendor, .buf_len = sizeof(vendor) });
        sns_publish_attribute( this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = false;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DYNAMIC, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = RPR0521RS_VERSION;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = 0; // samples
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_FIFO_SIZE, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = RPR0521RS_PS_SLEEP_CURRENT; //uA
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SLEEP_CURRENT, &value, 1U, false);
    }
    {
        float_t data[2] = {0,};
        state->encoded_event_len = pb_get_encoded_size_sensor_stream_event(data, 2U);
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = (int64_t)state->encoded_event_len;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_EVENT_SIZE, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        
        value.has_boolean = true;
        value.boolean     = true;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR, &value, 1U, false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR};
        
        values[0].has_sint = true;
        values[0].sint     = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
        values[1].has_sint = true;
        values[1].sint     = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS, values, ARR_SIZE(values), true);
    }
    
    return;
}

