//==============================================================================
// MODULE        : sns_rpr0521_hal.c
// FUNCTION      : hardware abstruction layer
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Feb/08/2018
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
#include "sns_rpr0521rs_sensor.h"
#include "sns_rpr0521rs_sensor_instance.h"
#include "sns_stream_service.h"
#include "sns_diag_service.h"
#include "sns_timer.pb.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_printf.h"
#include "sns_sensor_instance.h"
#include "sns_pb_util.h"
#include "sns_rpr0521rs_hal.h"

//==============================================================================
// function prototype
//==============================================================================
static uint8_t rpr0521rs_change_als_gain(uint8_t gain_rate);
static sns_rc  rpr0521rs_com_read_wrapper( RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes);
static sns_rc  rpr0521rs_com_write_wrapper(RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes);
static sns_rc  rpr0521rs_com_rw_common(RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes, bool read_write);
static sns_rc  rpr0521rs_device_write_power_ctl(RPR0521RS_INSTANCE_STATE *state, bool als, bool ps);
static sns_rc  rpr0521rs_device_rewrite_modectl(RPR0521RS_INSTANCE_STATE *state, RPR0521RS_SET_ODR set_intvl);
static sns_rc  rpr0521rs_device_reset(RPR0521RS_INSTANCE_STATE *state);

//==============================================================================
// NAME      : rpr0521rs_check_who_am_i
// FUNCTION  : get and check who-am-i value
// param[i]  : scp_service : handle to sync COM port service
// param[i]  : port_handle : handle to sync COM port
// param[o]  : read_data   : who am i value read from HW
// REMARKS   :
// RETURN    : sns_rc
//           :  SNS_RC_FAILED - COM port failure
//           :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_check_who_am_i(const sns_sync_com_port_service *scp_service, sns_sync_com_port_handle *port_handle, uint8_t *read_data)
{
    RPR0521RS_INSTANCE_STATE tmp_state; // This is temporary state for com. Not real state.
    
    sns_rc  result;
    
    tmp_state.scp_service               = (sns_sync_com_port_service *)scp_service;
    tmp_state.com_port_info.port_handle = port_handle;
    
    result = rpr0521rs_com_read_wrapper(&tmp_state, RPR0521RS_REG_SYSTEMCTL, read_data, 1U);
    if (result == SNS_RC_SUCCESS) {
        *read_data &= RPR0521RS_PARTID_MASK;
        if (*read_data == RPR0521RS_WHOAMI_VALUE) {
            result = SNS_RC_SUCCESS;
        } else {
            result = SNS_RC_FAILED;
        }
    }
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_register_interrupt
// FUNCTION  : register interrupt
// param[i]  : state : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_register_interrupt(RPR0521RS_INSTANCE_STATE* state)
{
    if (!state->irq_info.irq_registered) {
        sns_data_stream* data_stream = state->interrupt_data_stream;
        uint8_t buffer[20];
        sns_request irq_req = { .message_id  = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
                                .request_len = 0U,
                                .request     = buffer };

        irq_req.request_len = pb_encode_request(buffer, sizeof(buffer), &state->irq_info.irq_config, sns_interrupt_req_fields, NULL);
        if (irq_req.request_len > 0U) {
            data_stream->api->send_request(data_stream, &irq_req);
            state->irq_info.irq_registered = true;
        }
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_als_setting_register
// FUNCTION  : setting ODR to register
// param[i]  : state     : Sensor instance reference
// param[i]  : speed     : setting odr value (unit ms)
// param[o]  : raw_intvl : have odr setting in program
// REMARKS   :
// RETURN    : sns_rc
//           :  SNS_RC_FAILED - COM port failure
//           :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_als_setting_register(RPR0521RS_INSTANCE_STATE *state, uint16_t speed, uint16_t *raw_intvl)
{
    sns_rc  result = SNS_RC_SUCCESS;
    RPR0521RS_SET_ODR set_intvl;
    
    if (speed == 0U) {
        set_intvl = RPR0521RS_NON_SET;
    } else {
        switch (state->ps_info.common.curr_intvl_ms) {
            case RPR0521RS_PS_ODR_MS_50:
                if (speed < RPR0521RS_ALS_ODR_MS_100) {
                    // setting Error
                    result     = SNS_RC_NOT_SUPPORTED;
                } else if (speed < RPR0521RS_ALS_ODR_MS_400){
                    *raw_intvl = RPR0521RS_ALS_ODR_MS_100;
                    set_intvl  = RPR0521RS_ODR_ALS_100_PS_50;
                } else {
                    *raw_intvl = RPR0521RS_ALS_ODR_MS_400;
                    set_intvl  = RPR0521RS_ODR_ALS_400_PS_50;
                }
                break;
            
            case RPR0521RS_PS_ODR_MS_100:
                if (speed < RPR0521RS_ALS_ODR_MS_100) {
                    // setting Error
                    result     = SNS_RC_NOT_SUPPORTED;
                } else if (speed < RPR0521RS_ALS_ODR_MS_400){
                    *raw_intvl = RPR0521RS_ALS_ODR_MS_100;
                    set_intvl  = RPR0521RS_ODR_ALS_100_PS_100;
                } else {
                    *raw_intvl = RPR0521RS_ALS_ODR_MS_400;
                    set_intvl  = RPR0521RS_ODR_ALS_400_PS_100;
                }
                break;
            
            default:
                result = SNS_RC_FAILED;
                break;
        }
    }
    if (result == SNS_RC_SUCCESS) {
        result = rpr0521rs_device_rewrite_modectl(state, set_intvl);
    }
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_ps_setting_register
// FUNCTION  : setting ODR to register
// param[i]  : state     : Sensor instance reference
// param[i]  : speed     : setting odr value (unit ms)
// param[o]  : raw_intvl : have odr setting in program
// REMARKS   : 
// RETURN    : sns_rc
//           :  SNS_RC_FAILED - COM port failure
//           :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_ps_setting_register(RPR0521RS_INSTANCE_STATE *state, uint16_t speed, uint16_t *raw_intvl)
{
    sns_rc   result = SNS_RC_SUCCESS;
    RPR0521RS_SET_ODR set_intvl;
    
    if (speed == 0U) {
        set_intvl = RPR0521RS_NON_SET;
    } else {
        switch (state->als_info.common.curr_intvl_ms) {
            case RPR0521RS_ALS_ODR_MS_100:
                if (speed < RPR0521RS_PS_ODR_MS_50) {
                    // setting Error
                    result     = SNS_RC_NOT_SUPPORTED;
                } else if (speed < RPR0521RS_PS_ODR_MS_100) {
                    *raw_intvl = RPR0521RS_PS_ODR_MS_50;
                    set_intvl  = RPR0521RS_ODR_ALS_100_PS_50;
                } else {
                    *raw_intvl = RPR0521RS_PS_ODR_MS_100;
                    set_intvl  = RPR0521RS_ODR_ALS_100_PS_100;
                }
                break;
            
            case RPR0521RS_ALS_ODR_MS_400:
                if (speed < RPR0521RS_PS_ODR_MS_50) {
                    // setting Error
                    result     = SNS_RC_NOT_SUPPORTED;
                } else if (speed < RPR0521RS_PS_ODR_MS_100){
                    *raw_intvl = RPR0521RS_PS_ODR_MS_50;
                    set_intvl  = RPR0521RS_ODR_ALS_400_PS_50;
                } else {
                    *raw_intvl = RPR0521RS_PS_ODR_MS_100;
                    set_intvl  = RPR0521RS_ODR_ALS_400_PS_100;
                }
                break;
                    
            default :
                result = SNS_RC_FAILED;
                break;
        }
    }
    if(result == SNS_RC_SUCCESS) {
        result = rpr0521rs_device_rewrite_modectl(state, set_intvl);
    }
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_start_sensor_polling_timer
// FUNCTION  : make timer stream
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_info : Sensor instance common state
// REMARKS   : 
// RETURN    : sns_rc
//           :  SNS_RC_FAILED - COM port failure
//           :  SNS_RC_SUCCESS
//==============================================================================
void rpr0521rs_start_sensor_polling_timer(sns_sensor_instance *const this, RPR0521RS_SENSOR_COMMON_INFO *sensor_info)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    uint8_t buffer[70];
    
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    sns_request             timer_req   = { .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG, .request_len = 0U, .request = buffer };
    sns_rc                  result      = SNS_RC_SUCCESS;
    
    if (NULL == sensor_info->timer_data_stream) {
        sns_service_manager *smgr     = this->cb->get_service_manager(this);
        sns_stream_service  *strm_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
        result = strm_svc->api->create_sensor_instance_stream(strm_svc, this, state->timer_suid, &sensor_info->timer_data_stream);
    }
    
    if ((result == SNS_RC_SUCCESS) && (NULL != sensor_info->timer_data_stream)) {
        req_payload.is_periodic    = true;
        req_payload.start_time     = sns_get_system_time() - sensor_info->sampling_intvl;
        req_payload.timeout_period = sensor_info->sampling_intvl;
        req_payload.start_config.late_start_delta = sensor_info->sampling_intvl;
        req_payload.has_start_config = true;
        timer_req.request_len      = pb_encode_request(buffer, sizeof(buffer), &req_payload, sns_timer_sensor_config_fields, NULL);
        // start polling timer
        sensor_info->timer_data_stream->api->send_request(sensor_info->timer_data_stream, &timer_req);
        sensor_info->timer_is_active = true;
    } else {
        SNS_INST_PRINTF(ERROR, this, "error creating stream %d", result);
    }
    
    return;
}
//==============================================================================
// NAME      : rpr0521rs_inst_create_heart_beat_timer
// FUNCTION  : make heart beat timer stream
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_info : Sensor instance common state
// REMARKS   : 
// RETURN    : none
//==============================================================================
void rpr0521rs_inst_create_heart_beat_timer(sns_sensor_instance *this, RPR0521RS_SENSOR_COMMON_INFO *sensor_info)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    size_t  req_len = 0U;
    uint8_t buffer[50];
    
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    
    req_payload.is_periodic    = true;
    req_payload.start_time     = sns_get_system_time();
    req_payload.timeout_period = sensor_info->heart_beat_timeout;

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload, sns_timer_sensor_config_fields, NULL);
    if (req_len > 0U) {
        if (sensor_info->timer_heart_beat_data_stream == NULL) {
            sns_service_manager *service_mgr = this->cb->get_service_manager(this);
            sns_stream_service  *stream_mgr  = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
            stream_mgr->api->create_sensor_instance_stream(stream_mgr, this, state->timer_suid, &sensor_info->timer_heart_beat_data_stream);
            SNS_INST_PRINTF(LOW,this,"creating Heart Beat timer stream!");
        }
        if (sensor_info->timer_heart_beat_data_stream == NULL) {
            SNS_INST_PRINTF(ERROR, this, "Error creating Heart Beat timer stream!");
        } else {
            sns_request timer_req = { .message_id  = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
                                      .request     = buffer, 
                                      .request_len = req_len  };
            sensor_info->timer_heart_beat_data_stream->api->send_request(sensor_info->timer_heart_beat_data_stream, &timer_req);
            sensor_info->heart_beat_timer_is_active = true;
            SNS_INST_PRINTF(LOW,this,"set heart_beat timer start_time=%u timeout_period=%u", (uint32_t)(req_payload.start_time/19200), (uint32_t)(req_payload.timeout_period/19200));
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "RPR0521RS timer req encode error!");
    }

    return;
}

//==============================================================================
// NAME      : rpr0521rs_data_streaming_control
// FUNCTION  : control hardware enable
// param[i]  : state : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_data_streaming_control(RPR0521RS_INSTANCE_STATE *state)
{
    bool als = false;
    bool ps  = false;
    
    //start streaming mode
    if (state->sensor_type_info.enabled == (uint8_t)(RPR0521RS_ALS | RPR0521RS_PS)) {
        als = true;
        ps  = true;
    } else if (state->sensor_type_info.enabled == (uint8_t)RPR0521RS_ALS) {
        als = true;
        ps  = false;
    } else if (state->sensor_type_info.enabled == (uint8_t)RPR0521RS_PS) {
        als = false;
        ps  = true;
    } else {
        // no process
    }
    (void)rpr0521rs_device_write_power_ctl(state, als, ps);
    
    return;
}

//==============================================================================
// NAME     : rpr0521rs_device_init
// FUNCTION : initialize paramter for IC
// param[i] : state : Sensor instance reference
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_init(RPR0521RS_INSTANCE_STATE *state)
{
    sns_rc      result;
    uint8_t     gain0 = 0U;
    uint8_t     gain1 = 0U;
    ALL_REG_BUF reg_buf;
    
    rpr0521rs_device_reset(state);
    
    gain0 = rpr0521rs_change_als_gain(state->als_info.gain0);
    gain1 = rpr0521rs_change_als_gain(state->als_info.gain1);
    
    reg_buf.mode_ctl  = RPR0521RS_INIT_MODECTL;
    reg_buf.psals_ctl = RPR0521RS_INIT_ALSPSCTL | (gain1 << 2) | (gain0 << 4);
    reg_buf.ps_ctl    = RPR0521RS_INIT_PSCTL;
    reg_buf.reserved0 = 0U;
    reg_buf.reserved1 = 0U;
    reg_buf.reserved2 = 0U;
    reg_buf.reserved3 = 0U;
    reg_buf.reserved4 = 0U;
    reg_buf.reserved5 = 0U;
    reg_buf.intr      = RPR0521RS_INIT_INTRRUPT | state->irq_info.irq_trig;
    *(uint16_t*)(&reg_buf.psth_hl)  = RPR0521RS_INIT_PS_THH;
    *(uint16_t*)(&reg_buf.psth_ll)  = RPR0521RS_INIT_PS_THL;
    *(uint16_t*)(&reg_buf.alsth_hl) = RPR0521RS_INIT_ALS_THH;
    *(uint16_t*)(&reg_buf.alsth_ll) = RPR0521RS_INIT_ALS_THL;

    state->reg_buf = reg_buf;
    
    result = rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_MODECTL, (uint8_t*)&state->reg_buf, sizeof(state->reg_buf));
    
    return (result);

}

//==============================================================================
// NAME     : rpr0521rs_change_als_gain
// FUNCTION : change data from value rate to register value
// param[i] : gain_rate : gain rate
// REMARKS  :
// RETURN   : gain data of register value
//          : 
//==============================================================================
static uint8_t rpr0521rs_change_als_gain(uint8_t gain_rate)
{
    uint8_t temp = 0U;
    
    switch (gain_rate){
        case RPR0521RS_ALS_GAIN_X1 :
            temp = (uint8_t)RPR0521RS_ALS_REG_GAIN_1X;
            break;
        case RPR0521RS_ALS_GAIN_X2 :
            temp = (uint8_t)RPR0521RS_ALS_REG_GAIN_2X;
            break;
        case RPR0521RS_ALS_GAIN_X64 :
            temp = (uint8_t)RPR0521RS_ALS_REG_GAIN_64X;
            break;
        case RPR0521RS_ALS_GAIN_X128 :
            temp = (uint8_t)RPR0521RS_ALS_REG_GAIN_128X;
            break;
        default :
            temp = (uint8_t)RPR0521RS_ALS_REG_GAIN_1X;
            break;
    }
    
    return (temp);
}

//==============================================================================
// NAME     : rpr0521rs_device_reset
// FUNCTION : reset ic
// param[i] : state : Sensor instance reference
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_device_reset(RPR0521RS_INSTANCE_STATE *state)
{
    sns_rc  result = SNS_RC_FAILED;
    uint8_t data   = 0U;
    
    data = RPR0521RS_SW_RESET | RPR0521RS_INT_RESET;
    result = rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_SYSTEMCTL, &data, sizeof(data));
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_write_power_ctl
// FUNCTION : hardware power on
// param[i] : state : Sensor instance reference
// param[i] : als
// param[i] : ps
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_device_write_power_ctl(RPR0521RS_INSTANCE_STATE *state, bool als, bool ps)
{
    sns_rc  result;
    uint8_t power_state = state->reg_buf.mode_ctl;
    
    power_state &= RPR0521RS_POWER_CLR;
    
    if (als) {
        power_state |= RPR0521RS_ALS_EN;
    }
    if (ps) {
        power_state |= RPR0521RS_PS_EN;
    }
        
    result = rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_MODECTL, &power_state, sizeof(power_state));
    if (result == SNS_RC_SUCCESS) {
    	if ((state->als_info.common.test_info.test_client_present) || (state->ps_info.common.test_info.test_client_present)) {
            // no process;
        } else {
        	state->reg_buf.mode_ctl = power_state;
        }
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_ps_read_data
// FUNCTION : read proximity data
// param[i] : state    : Sensor instance reference
// param[o] : data0    : near or far data
// param[o] : raw_data : raw data from ic
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_ps_read_data(RPR0521RS_INSTANCE_STATE *state, uint8_t *data0, uint16_t *raw_data)
{
    sns_rc   result;
    uint16_t ps_data = 0U;
    
    result = rpr0521rs_com_read_wrapper(state, RPR0521RS_REG_PS_DATA, (uint8_t *)&ps_data, sizeof(ps_data));
    
    if (result == SNS_RC_SUCCESS) {
        if (ps_data <= state->ps_info.thresh_far) {
            *data0 = RPR0521RS_PS_FAR;
        } else if (ps_data >= state->ps_info.thresh_near){
            *data0 = RPR0521RS_PS_NEAR;
        } else {
            *data0 = RPR0521RS_PS_KEEP;
        }
        *raw_data = ps_data;
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_als_read_data
// FUNCTION : read ambient light data
// param[i] : state    : Sensor instance reference
// param[o] : raw_data : raw data from ic
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_als_read_data(RPR0521RS_INSTANCE_STATE *state, ALS_DATA *raw_data)
{
    sns_rc   result;
    ALS_DATA als;
    
    result = rpr0521rs_com_read_wrapper(state, RPR0521RS_REG_ALS_DATA, (uint8_t *)&als, sizeof(als));
    if (result == SNS_RC_SUCCESS) {
        *raw_data = als;
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_als_threshold_ctl
// FUNCTION : make data of als threshold
// param[i] : state  : Sensor instance reference
// param[i] : adata0 : adc data0
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_als_threshold_ctl(RPR0521RS_INSTANCE_STATE *state, uint16_t adata0)
{
    sns_rc   result;
    uint32_t delt     = 0U;
    uint32_t new_thh  = 0U;
    uint32_t new_thl  = 0U;
    uint32_t raw_data = 0U;

    raw_data = (uint32_t)adata0;
    delt     = (raw_data * RPR0521RS_ALS_TH_PERCENT) / 100U;
    new_thh  = raw_data + delt;
    new_thl  = raw_data - delt;
    
    if (new_thh > RPR0521RS_ALS_MAX_VALUE) {
        new_thh = RPR0521RS_ALS_MAX_VALUE;
    }
    
    result = rpr0521rs_device_set_threshold(state, (uint16_t)new_thh, (uint16_t)new_thl, RPR0521RS_REG_ALS_TH_L);
    if (result == SNS_RC_SUCCESS) {
        state->als_info.thresh_high = (uint16_t)new_thh;
        state->als_info.thresh_low  = (uint16_t)new_thl;
    }
        
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_set_threshold
// FUNCTION : set threshold
// param[i] : state    : Sensor instance reference
// param[i] : high     : high threshold paramter
// param[i] : low      : low threshold paramter
// param[i] : reg_addr : hardware register address
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_set_threshold(RPR0521RS_INSTANCE_STATE *state, uint16_t high, uint16_t low, uint32_t reg_addr)
{
    sns_rc    result;
    THRESHOLD set_threshold;
    
    set_threshold.high = high;
    set_threshold.low  = low;
    
    result = rpr0521rs_com_write_wrapper(state, reg_addr, (uint8_t *)&set_threshold, sizeof(set_threshold));
    if (result == SNS_RC_SUCCESS) {
        if (reg_addr == RPR0521RS_REG_ALS_TH_L) {
            if (state->als_info.common.test_info.test_client_present) {
            	// no process
            } else {
                *(uint16_t*)(&state->reg_buf.alsth_hl) = set_threshold.high;
                *(uint16_t*)(&state->reg_buf.alsth_ll) = set_threshold.low;
            }
        } else {
            if (state->ps_info.common.test_info.test_client_present) {
            	// no process
            } else {
                *(uint16_t*)(&state->reg_buf.psth_hl) = set_threshold.high;
                *(uint16_t*)(&state->reg_buf.psth_ll) = set_threshold.low;
            }
        }
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_interrupt_read
// FUNCTION : read interrupt status
// param[i] : state : Sensor instance reference
// param[o] : intr  : interrupt status
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_interrupt_read(RPR0521RS_INSTANCE_STATE *state, uint8_t *intr)
{
    sns_rc  result;
    uint8_t read_data = 0U;
    
    result = rpr0521rs_com_read_wrapper(state, RPR0521RS_REG_INTERRUPT, &read_data, sizeof(read_data));
    if (result == SNS_RC_SUCCESS) {
        *intr = read_data;
    }
    
    return result;
}

//==============================================================================
// NAME     : rpr0521rs_device_set_interrupt
// FUNCTION : set interrupt register
// param[i] : state : Sensor instance reference
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_set_interrupt(RPR0521RS_INSTANCE_STATE *state)
{
    sns_rc  result;
    uint8_t write_reg_data = 0U;
    
    write_reg_data = (state->reg_buf.intr & RPR0521RS_INT_TRG_CLR) | state->irq_info.irq_trig;
    result         = rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_INTERRUPT, &write_reg_data, 1U);
    if (result == SNS_RC_SUCCESS) {
        if ((state->als_info.common.test_info.test_client_present) || (state->ps_info.common.test_info.test_client_present)) {
        	// no process
        } else {
            state->reg_buf.intr = write_reg_data;
        }
    }
    
    return result;
}

//==============================================================================
// NAME     : rpr0521rs_device_rewrite_modectl
// FUNCTION : rewrite mode control register
// param[i] : state     : Sensor instance reference
// param[i] : set_intvl : set ODR to IC
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_device_rewrite_modectl(RPR0521RS_INSTANCE_STATE *state, RPR0521RS_SET_ODR set_intvl)
{
    sns_rc  result         = SNS_RC_SUCCESS;
    uint8_t write_reg_data = 0U;
    
    if (set_intvl == RPR0521RS_NON_SET) {
        // non process
    } else {
        write_reg_data = (state->reg_buf.mode_ctl & RPR0521RS_MEASER_ODR_CLR) | set_intvl;
        result = rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_MODECTL, &write_reg_data, 1U);
        if (result == SNS_RC_SUCCESS) {
            if ((state->als_info.common.test_info.test_client_present) || (state->ps_info.common.test_info.test_client_present)) {
                // no process
            } else {
                state->reg_buf.mode_ctl = write_reg_data;
            }
        }
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_device_psctl_read
// FUNCTION : read psctl data
// param[i] : state  : Sensor instance reference
// param[o] : ps_ctl : psctl data
// REMARKS  :
// RETURN   : sns_rc
//          :  SNS_RC_FAILED - COM port failure
//          :  SNS_RC_SUCCESS
//==============================================================================
sns_rc rpr0521rs_device_psctl_read(RPR0521RS_INSTANCE_STATE *state, uint8_t *ps_ctl)
{
    sns_rc  result;
    uint8_t read_data = 0U;
    
    result = rpr0521rs_com_read_wrapper(state, RPR0521RS_REG_PSCTL, &read_data, sizeof(read_data));
    if (result == SNS_RC_SUCCESS) {
        *ps_ctl = read_data;
    }
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_recovery_device_state
// FUNCTION : 
// param[i] : instance : Sensor instance reference
// REMARKS  :
// RETURN   : none
//==============================================================================
void rpr0521rs_recovery_device_state(sns_sensor_instance *instance)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    rpr0521rs_device_reset(state);
    rpr0521rs_com_write_wrapper(state, RPR0521RS_REG_MODECTL, (uint8_t*)&state->reg_buf, sizeof(state->reg_buf));
    
    rpr0521rs_dump_reg(instance);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_dump_reg
// FUNCTION  : read registers for debug
// param[i]  : this : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_dump_reg(sns_sensor_instance *this)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    struct group_read {
        uint32_t first_reg;
        uint8_t  num_regs;
    } groups[] = {{RPR0521RS_REG_SYSTEMCTL, 6},
                  {RPR0521RS_REG_INTERRUPT, 9},
                  {RPR0521RS_REG_ID,        1}};
    uint8_t  regv_buffer[32] = {0};
    uint32_t i;
    uint32_t j;

    for (i=0; i<ARR_SIZE(groups); i++) {
        (void)rpr0521rs_com_read_wrapper(state, groups[i].first_reg, regv_buffer, groups[i].num_regs);
        for (j=0; j<groups[i].num_regs; j++) {
            SNS_INST_PRINTF(MED, this, "reg[0x%02x] = 0x%02x", groups[i].first_reg+j, regv_buffer[j]);
            regv_buffer[j] = 0U;
        }
    }
    
    return;
}

//==============================================================================
// NAME     : rpr0521rs_com_read_wrapper
// FUNCTION : read wrapper for sync com port service
// param[i] : state    : Sensor instance reference
// param[i] : reg_addr : register address
// param[i] : buffer   : read buffer
// param[i] : bytes    : bytes to read
// REMARKS  :
// RETURN   : sns_rc
//          :   SNS_RC_FAILED - COM port failure
//          :   SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_com_read_wrapper(RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes)
{
    sns_rc result;
    
    result = rpr0521rs_com_rw_common(state, reg_addr, buffer, bytes, false);
    
    return (result);
}

//==============================================================================
// NAME     : rpr0521rs_com_write_wrapper
// FUNCTION : write wrapper for sync com port service
// param[i] : state    : Sensor instance reference
// param[i] : reg_addr : register address
// param[i] : buffer   : write buffer
// param[i] : bytes    : bytes to read
// REMARKS  :
// RETURN   : sns_rc
//          :   SNS_RC_FAILED - COM port failure
//          :   SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_com_write_wrapper(RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes)
{
    sns_rc result;
    
    result = rpr0521rs_com_rw_common(state, reg_addr, buffer, bytes, true);
    
    return (result);
    
}

//==============================================================================
// NAME     : rpr0521rs_com_rw_common
// FUNCTION : common wrapper for sync com port service
// param[i] : state      : Sensor instance reference
// param[i] : reg_addr   : register address
// param[i] : buffer     : write buffer
// param[i] : bytes      : bytes to write
// param[i] : read_write : read : false, write : true
// REMARKS  :
// RETURN   : sns_rc
//          :   SNS_RC_FAILED - COM port failure
//          :   SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_com_rw_common(RPR0521RS_INSTANCE_STATE *state, uint32_t reg_addr, uint8_t *buffer, uint32_t bytes, bool read_write)
{
    sns_sync_com_port_service *scp    = NULL;
    sns_sync_com_port_handle  *handle = NULL;
    
    sns_port_vector port_vec;
    sns_rc          result;
    uint32_t        xfer_bytes = 0U;
    
    scp    = state->scp_service;
    handle = state->com_port_info.port_handle;
    
    port_vec.buffer   = buffer;
    port_vec.bytes    = bytes;
    port_vec.is_write = read_write;
    port_vec.reg_addr = reg_addr;
    result = scp->api->sns_scp_register_rw(handle, &port_vec, 1, false, &xfer_bytes);
    if (xfer_bytes == bytes) {
        result = SNS_RC_SUCCESS;
    } else {
        result = SNS_RC_FAILED;
    }

    return (result);
}
