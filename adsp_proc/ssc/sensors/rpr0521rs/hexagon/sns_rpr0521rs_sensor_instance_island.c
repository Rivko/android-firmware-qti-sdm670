//==============================================================================
// MODULE        : sns_rpr0521_sensor_instance_island.c
// FUNCTION      : Sensor instance implementation for island mode
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Nov/16/2017
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
#include "sns_printf.h"
#include "sns_std_sensor.pb.h"
#include "sns_timer.pb.h"
#include "sns_proximity.pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_sensor_util.h"
#include "sns_cal.pb.h"
#include "sns_event_service.h"
#include "sns_rpr0521rs_hal.h"
#include "sns_rpr0521rs_sensor.h"
#include "sns_rpr0521rs_sensor_instance.h"

//==============================================================================
// function prototype
//==============================================================================
// sensor api funciton 
static sns_rc rpr0521rs_inst_set_client_config(sns_sensor_instance *const this,sns_request const *client_request);
static sns_rc rpr0521rs_inst_notify_event(sns_sensor_instance *const this);

// internal use
static sns_rc rpr0521rs_ps_send_data(sns_sensor_instance *const this, sns_time ts);
static sns_rc rpr0521rs_als_send_data(sns_sensor_instance *const this, sns_time ts);
static sns_rc rpr0521rs_process_heart_beat_timer_event(sns_sensor_instance *const this, sns_sensor_event *event, RPR0521RS_SENSOR_TYPE sensor_type);
static sns_rc rpr0521rs_inst_update(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type);
static sns_rc rpr0521rs_log_sensor_state_raw_alloc(sns_sensor_instance *const this, sns_sensor_uid *suid, LOG_SENSOR_STATE_RAW_INFO *log_raw_info);
static sns_rc rpr0521rs_log_sensor_state_raw_add(sns_sensor_instance *const this, LOG_SENSOR_STATE_RAW_INFO *log_raw_info, float_t *raw_data, sns_time timestamp, sns_std_sensor_sample_status status, RPR0521RS_SENSOR_TYPE sensor_type);
static sns_rc rpr0521rs_als_encode_log_sensor_state_raw(void *log_info, size_t log_size, size_t encoded_log_size, void *encoded_log, size_t *bytes_written);
static sns_rc rpr0521rs_ps_encode_log_sensor_state_raw(void *log_info, size_t log_size, size_t encoded_log_size, void *encoded_log, size_t *bytes_written);
static void rpr0521rs_handle_hbt_event(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_set_polling_config(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_handle_sensor_config(sns_sensor_instance *const this, RPR0521RS_REQ *payload);
static void rpr0521rs_handle_on_change_config(sns_sensor_instance *const this, RPR0521RS_REQ *payload);
static void rpr0521rs_handle_test_config(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_process_interrupt_event(sns_sensor_instance *const this, sns_sensor_event *event);
static void rpr0521rs_process_timer_event(sns_sensor_instance *const this, sns_sensor_event *event, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_log_sensor_state_raw_submit(LOG_SENSOR_STATE_RAW_INFO *log_raw_info, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_make_raw_log(sns_sensor_instance *const this, float_t *raw_data, sns_time ts, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_send_config_event(sns_sensor_instance *const instance);
static uint16_t rpr0521rs_change_hz_to_ms(sns_sensor_instance *const this, float_t speed);
static float_t rpr0521rs_calculate_lx(sns_sensor_instance *const this, ALS_DATA raw_data);

//==============================================================================
// struct define
//==============================================================================
sns_sensor_instance_api rpr0521rs_sensor_instance_api =
{
    .struct_len        = sizeof(sns_sensor_instance_api),
    .init              = &rpr0521rs_inst_init,
    .deinit            = &rpr0521rs_inst_deinit,
    .set_client_config = &rpr0521rs_inst_set_client_config,
    .notify_event      = &rpr0521rs_inst_notify_event
};

//==============================================================================
// NAME      : rpr0521rs_inst_set_client_config
// FUNCTION  : set cilent config (API)
// param[i]  : this           : Sensor instance reference
// param[i]  : client_request : sensor specific registry configuration
// REMARKS   :
// RETURN    : sns_rc
//             SNS_RC_FAILED - COM port failure
//             SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_inst_set_client_config(sns_sensor_instance *const this, sns_request const *client_request)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_REQ *payload = (RPR0521RS_REQ*)client_request->request;

    SNS_INST_PRINTF(LOW, this, "set_client_cfg msg_id %d", client_request->message_id);
    
    // Turn COM port ON
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
    
    if ((payload->sensor_type == RPR0521RS_ALS) || (payload->sensor_type == RPR0521RS_PS)) {
        switch (client_request->message_id){
            case (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG:
                rpr0521rs_handle_sensor_config(this, payload);
                break;

            case (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG:
                rpr0521rs_handle_on_change_config(this, payload);
                break;

            case (uint32_t)SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG:
                rpr0521rs_handle_test_config(this, payload->sensor_type);
                break;
            
            default :
                // no process
                break;
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "Non sensor type");
    }
    
    // Turn COM port OFF
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : rpr0521rs_handle_sensor_config
// FUNCTION  : handle sensor config at polling mode
// param[i]  : this    : Sensor instance reference
// param[i]  : payload : parameter from sensor
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_handle_sensor_config(sns_sensor_instance *const this, RPR0521RS_REQ *payload)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    sns_rc   rc                = SNS_RC_SUCCESS;
    uint16_t raw_intvl         = 0U;
    uint16_t desired_sample_ms = 0U;
    
    sns_service_manager *mgr           = this->cb->get_service_manager(this);
    sns_event_service   *event_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    
    desired_sample_ms = rpr0521rs_change_hz_to_ms(this, payload->sampling_rate_hz);
    if (payload->sensor_type == RPR0521RS_ALS) {
        rc          = rpr0521rs_als_setting_register(state, desired_sample_ms, &raw_intvl);
        sensor_info = &state->als_info.common;
    } else {
        rc          = rpr0521rs_ps_setting_register(state, desired_sample_ms, &raw_intvl);
        sensor_info = &state->ps_info.common;
    }
    
    sensor_info->is_dri = false;
    
    if (rc == SNS_RC_SUCCESS) {
        if (FLOAT_ZERO_CHECK > payload->sampling_rate_hz) {
            sensor_info->sampling_intvl = 0U;
        } else {
            sensor_info->sampling_intvl = (sns_time)sns_convert_ns_to_ticks((uint64_t)(1000000000.0F / payload->sampling_rate_hz));
        }
        if (!sensor_info->test_info.test_client_present) {
            sensor_info->sampling_rate_hz   = payload->sampling_rate_hz;
            sensor_info->curr_intvl_ms      = raw_intvl;
            sensor_info->heart_beat_timeout = sensor_info->sampling_intvl * RPR0521RS_HBT_POLLING;
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "ODR match error %d", rc);
        //Allocate and send immediately.
        sns_sensor_event *event = event_service->api->alloc_event(event_service, this, 0);
        if (NULL != event) {
            event->message_id = (uint32_t)SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
            event->event_len  = 0U;
            event->timestamp  = sns_get_system_time();
            event_service->api->publish_event(event_service, this, event, &sensor_info->suid);
        }
        SNS_INST_PRINTF(ERROR, this, "Execution Faild in rpr0521rs setting register!");
        return;
    }
    
    // set factory calibration parameter to sensor instance
    if (payload->fac_cal_version >= sensor_info->registry_cfg.fac_cal_version) {
        sensor_info->registry_cfg.fac_cal_scale_factor = payload->fac_cal_scale_factor;
        sensor_info->registry_cfg.fac_cal_bias         = payload->fac_cal_bias;
        sensor_info->registry_cfg.fac_cal_version      = payload->fac_cal_version;
    }
    
    SNS_INST_PRINTF(LOW, this, "sensor_type = %d, fac_cal_version = %d, fac_cal_scale_factor = %d, fac_cal_bias = %d", 
                               (int)payload->sensor_type, (uint32_t)sensor_info->registry_cfg.fac_cal_version, (unsigned int)(sensor_info->registry_cfg.fac_cal_scale_factor * 1000), (unsigned int)sensor_info->registry_cfg.fac_cal_bias);
    
    rpr0521rs_reconfig_hw(this);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_handle_on_change_config
// FUNCTION  : handle sensor config at on change mode
// param[i]  : this    : Sensor instance reference
// param[i]  : payload : parameter from sensor
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_handle_on_change_config(sns_sensor_instance *const this, RPR0521RS_REQ *payload)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    int32_t temp = 0;
    
    if (payload->sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
        if (state->sensor_type_info.enabled & (uint8_t)RPR0521RS_ALS) {
            state->irq_info.irq_trig |= RPR0521RS_INT_TRIG_ALS;
            sensor_info->heart_beat_timeout = (sns_time)sns_convert_ns_to_ticks(sensor_info->curr_intvl_ms * 1000000ULL) * RPR0521RS_HBT_DRI;
            if (state->reset_threshold) {
                rpr0521rs_device_set_threshold(state, RPR0521RS_INIT_ALS_THH, RPR0521RS_INIT_ALS_THL, RPR0521RS_REG_ALS_TH_L);
                state->reset_threshold = false;
            }
        } else {
            state->irq_info.irq_trig &= ~RPR0521RS_INT_TRIG_ALS;
            sensor_info->heart_beat_timeout = 0U;
        }
    } else {
        sensor_info = &state->ps_info.common;
        if (state->sensor_type_info.enabled & (uint8_t)RPR0521RS_PS) {
            state->irq_info.irq_trig |= RPR0521RS_INT_TRIG_PS;
            sensor_info->heart_beat_timeout = (sns_time)sns_convert_ns_to_ticks(sensor_info->curr_intvl_ms * 1000000ULL) * RPR0521RS_HBT_DRI;
            if (state->reset_threshold) {
                rpr0521rs_device_set_threshold(state, RPR0521RS_INIT_PS_THH, RPR0521RS_INIT_PS_THL, RPR0521RS_REG_PS_TH_L);
                state->reset_threshold = false;
            }
        } else {
            state->irq_info.irq_trig &= ~RPR0521RS_INT_TRIG_PS;
            sensor_info->heart_beat_timeout = 0U;
        }
    }
    
    // set factory calibration parameter to sensor instance
    if (payload->fac_cal_version >= sensor_info->registry_cfg.fac_cal_version) {
        sensor_info->registry_cfg.fac_cal_scale_factor = payload->fac_cal_scale_factor;
        sensor_info->registry_cfg.fac_cal_bias         = payload->fac_cal_bias;
        sensor_info->registry_cfg.fac_cal_version      = payload->fac_cal_version;
    }
    
    if (payload->sensor_type == RPR0521RS_PS) {
        temp = (int32_t)state->ps_info.ori_thresh_near + (int32_t)sensor_info->registry_cfg.fac_cal_bias;
        if (temp < 0) {
            state->ps_info.thresh_near = 0U;
        } else {
            state->ps_info.thresh_near = (uint16_t)temp;
        }
        temp = (int32_t)state->ps_info.ori_thresh_far + (int32_t)sensor_info->registry_cfg.fac_cal_bias;
        if (temp < 0) {
            state->ps_info.thresh_far = 0U;
        } else {
            state->ps_info.thresh_far = (uint16_t)temp;
        }
    }

    SNS_INST_PRINTF(LOW, this, "thresh_near = %d, thresh_far = %d", (unsigned int)state->ps_info.thresh_near, (unsigned int)state->ps_info.thresh_far);
    SNS_INST_PRINTF(LOW, this, "sensor_type = %d, fac_cal_version = %d, fac_cal_scale_factor = %d, fac_cal_bias = %d", 
                               (int)payload->sensor_type, (uint32_t)sensor_info->registry_cfg.fac_cal_version, (unsigned int)(sensor_info->registry_cfg.fac_cal_scale_factor * 1000), (unsigned int)sensor_info->registry_cfg.fac_cal_bias);
    
    if (sensor_info->sampling_intvl > 0U) {
        sensor_info->sampling_intvl= 0U;
    }
    
    sensor_info->is_dri = true;
    rpr0521rs_register_interrupt(state);
    rpr0521rs_device_set_interrupt(state);
    rpr0521rs_reconfig_hw(this);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_handle_test_config
// FUNCTION  : handle sensor config at self test mode
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_handle_test_config(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    // All self-tests can be handled in normal mode.
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
    rpr0521rs_run_self_test(this, sensor_type);
    state->new_self_test_request = false;
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_inst_notify_event
// FUNCTION  : instance notify event (API)
// param[i]  : this : Sensor instance reference
// REMARKS   :
// RETURN    : sns_rc
//             SNS_RC_FAILED - COM port failure
//             SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_inst_notify_event(sns_sensor_instance *const this)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    sns_sensor_event *event = NULL;
    
    sns_rc als_hbt_result = SNS_RC_SUCCESS;
    sns_rc ps_hbt_result  = SNS_RC_SUCCESS;
    sns_rc result         = SNS_RC_SUCCESS;
    
    // Turn COM port ON
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
    
    // Handle interrupts
    if (NULL != state->interrupt_data_stream) {
        event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
        while (NULL != event) {
            rpr0521rs_process_interrupt_event(this, event);
            event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
        }
    }

    // Handle ALS Timer events
    if (NULL != state->als_info.common.timer_data_stream) {
        event = state->als_info.common.timer_data_stream->api->peek_input(state->als_info.common.timer_data_stream);
        while (NULL != event) {
            rpr0521rs_process_timer_event(this, event, RPR0521RS_ALS);
            event = state->als_info.common.timer_data_stream->api->get_next_input(state->als_info.common.timer_data_stream);
        }
    }

    // Handle PS Timer events
    if (NULL != state->ps_info.common.timer_data_stream) {
        event = state->ps_info.common.timer_data_stream->api->peek_input(state->ps_info.common.timer_data_stream);
        while (NULL != event) {
            rpr0521rs_process_timer_event(this, event, RPR0521RS_PS);
            event = state->ps_info.common.timer_data_stream->api->get_next_input(state->ps_info.common.timer_data_stream);
        }
    }
    
    // Handle ALS Heart Beat Timer events
    if (NULL != state->als_info.common.timer_heart_beat_data_stream) {
        event = state->als_info.common.timer_heart_beat_data_stream->api->peek_input(state->als_info.common.timer_heart_beat_data_stream);
        while (NULL != event) {
            als_hbt_result = rpr0521rs_process_heart_beat_timer_event(this, event, RPR0521RS_ALS);
            event  = state->als_info.common.timer_heart_beat_data_stream->api->get_next_input(state->als_info.common.timer_heart_beat_data_stream);
        }
    } 
    
    // Handle PS Heart Beat Timer events
    if (NULL != state->ps_info.common.timer_heart_beat_data_stream) {
        event = state->ps_info.common.timer_heart_beat_data_stream->api->peek_input(state->ps_info.common.timer_heart_beat_data_stream);
        while (NULL != event) {
            ps_hbt_result = rpr0521rs_process_heart_beat_timer_event(this, event, RPR0521RS_PS);
            event  = state->ps_info.common.timer_heart_beat_data_stream->api->get_next_input(state->ps_info.common.timer_heart_beat_data_stream);
        }
    }

    if ((als_hbt_result == SNS_RC_INVALID_STATE) || (ps_hbt_result == SNS_RC_INVALID_STATE)) {
        result = SNS_RC_INVALID_STATE;
    } else if ((als_hbt_result == SNS_RC_NOT_AVAILABLE) || (ps_hbt_result == SNS_RC_NOT_AVAILABLE)) {
        result = SNS_RC_NOT_AVAILABLE;
    } else {
        result = SNS_RC_SUCCESS;
    }
    
    // Turn COM port OFF
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_process_interrupt_event
// FUNCTION  : process interrupt event
// param[i]  : this  : Sensor instance reference
// param[i]  : event : sensor event
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_process_interrupt_event(sns_sensor_instance *const this, sns_sensor_event *event)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
    uint8_t             intr_state = 0U;
    
    switch (event->message_id) {
        case (uint32_t)SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT:
            SNS_INST_PRINTF(LOW, this, "SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT");
            state->irq_info.irq_ready = true;
            break;
        
        case (uint32_t)SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT:
            SNS_INST_PRINTF(LOW, this, "SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT");
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
            if (pb_decode(&stream, sns_interrupt_event_fields, &irq_event)) {
                // Check which sensor's interrupt happens.
                (void)rpr0521rs_device_interrupt_read(state, (uint8_t *)&intr_state);
                if ((RPR0521RS_PS_INT_OCCUR & intr_state) && (state->ps_info.common.is_dri)) {
                    (void)rpr0521rs_ps_send_data(this, event->timestamp);
                }
                if ((RPR0521RS_ALS_INT_OCCUR & intr_state) && (state->als_info.common.is_dri)) {
                    (void)rpr0521rs_als_send_data(this, event->timestamp);
                }
            }
            break;
        
        default:
            SNS_INST_PRINTF(ERROR, this, "Received invalid event id=%d", event->message_id);
            break;
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_process_timer_event
// FUNCTION  : process timer event
// param[i]  : this        : Sensor instance reference
// param[i]  : event       : sensor event
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_process_timer_event(sns_sensor_instance *const this, sns_sensor_event *event, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    sns_timer_sensor_event timer_event;
    
    pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
    
    switch (event->message_id) {
        case (uint32_t)SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT:
            SNS_INST_PRINTF(LOW, this, "SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT");
            // non process
            break;
        
        case (uint32_t)SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT:
            SNS_INST_PRINTF(LOW, this, "SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT");
            if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                if ((sensor_type == RPR0521RS_ALS) && (state->sensor_type_info.enabled & RPR0521RS_ALS)) {
                    (void)rpr0521rs_als_send_data(this, event->timestamp);
                } else if ((sensor_type == RPR0521RS_PS) && (state->sensor_type_info.enabled & RPR0521RS_PS)) {
                    (void)rpr0521rs_ps_send_data(this, event->timestamp);
                } else {
                    // no process
                }
            }
            break;
            
        default:
            SNS_INST_PRINTF(MED, this, "unknown message_id %d", event->message_id);
            break;
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_process_heart_beat_timer_event
// FUNCTION  : process heart beat timer event
// param[i]  : this        : Sensor instance reference
// param[i]  : event       : sensor event
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : sns_rc
//             SNS_RC_SUCCESS
//             SNS_RC_NOT_AVAILABLE : try to revive sensor
//             SNS_RC_INVALID_STATE : after some revive attempts
//==============================================================================
static sns_rc rpr0521rs_process_heart_beat_timer_event(sns_sensor_instance *const this, sns_sensor_event *event, RPR0521RS_SENSOR_TYPE sensor_type)
{
    sns_rc result = SNS_RC_SUCCESS;
    
    pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
    sns_timer_sensor_event timer_event;
    if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
        if (event->message_id == (uint32_t)SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT) {
            SNS_INST_PRINTF(LOW, this, "Heart beat timer fired");
            if (sensor_type == RPR0521RS_ALS) {
                rpr0521rs_handle_hbt_event(this, RPR0521RS_ALS);
                result = rpr0521rs_inst_update(this, RPR0521RS_ALS);
            } else {
                rpr0521rs_handle_hbt_event(this, RPR0521RS_PS);
                result = rpr0521rs_inst_update(this, RPR0521RS_PS);
            }
        } else {
            SNS_INST_PRINTF(LOW, this, "Heart beat timer message_id : %u", event->message_id);
        }
    }

    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_handle_hbt_event
// FUNCTION  : judge whether need to revive sensor IC
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_handle_hbt_event(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    sns_rc  rc       = SNS_RC_SUCCESS;
    uint8_t who_am_i = 0U;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }
    
    if (sensor_info->is_dri == DRI_MODE) {
        rc = rpr0521rs_check_who_am_i(state->scp_service, state->com_port_info.port_handle, &who_am_i); // COM Check
        if (rc != SNS_RC_SUCCESS) {
            state->revive_sensor = true;
            sensor_info->heart_beat_count++;
        }
    } else {
        state->revive_sensor = true;
        sensor_info->heart_beat_count++;
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_inst_update
// FUNCTION  : if need, try to revive sensor
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : sns_rc
//             SNS_RC_SUCCESS
//             SNS_RC_NOT_AVAILABLE : try to revive sensor
//             SNS_RC_INVALID_STATE : after some revive attempts
//==============================================================================
static sns_rc rpr0521rs_inst_update(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    sns_rc result = SNS_RC_SUCCESS;
    sns_rc rc     = SNS_RC_SUCCESS;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }
    
    if (sensor_info->heart_beat_count < RPR0521RS_HBT_FIRED_COUNT) {
        if (state->revive_sensor) {
            rc = rpr0521rs_device_init(state);
            if (rc == SNS_RC_SUCCESS) {
                (void)rpr0521rs_device_set_interrupt(state);
                rpr0521rs_reconfig_hw(this);
                state->revive_sensor = false;
            }
            result = SNS_RC_NOT_AVAILABLE;
            SNS_INST_PRINTF(HIGH, this, "SNS_RC_NOT_AVAILABLE");
        }
    } else {
        result = SNS_RC_INVALID_STATE;
        SNS_INST_PRINTF(HIGH, this, "SNS_RC_INVALID_STATE");
    }
    
    return result;
}

//==============================================================================
// NAME      : rpr0521rs_ps_send_data
// FUNCTION  : send sensor data to application for proximity
// param[i]  : this : Sensor instance reference
// param[i]  : ts   : time stamp
// REMARKS   : 
// RETURN    : sns_rc
//==============================================================================
static sns_rc rpr0521rs_ps_send_data(sns_sensor_instance *const this, sns_time ts)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    sns_proximity_event ps_event = sns_proximity_event_init_default;
    sns_rc   result;
    uint16_t ps_raw  = 0U;
    uint8_t  ps_data = 0U;
    uint8_t  ps_ctl  = 0U;
    uint16_t set_h_threshold = 0U;
    uint16_t set_l_threshold = 0U;
    bool     event_send = false;
    
    result = rpr0521rs_device_psctl_read(state, &ps_ctl);
    if (result == SNS_RC_SUCCESS) {
        result = rpr0521rs_device_ps_read_data(state, &ps_data, &ps_raw);
    }
    
    if (result == SNS_RC_SUCCESS) {
        if (state->ps_info.common.test_info.test_client_present) {
            state->island_service->api->sensor_instance_island_exit(state->island_service, this);
            rpr0521rs_process_factory_test(this, (float_t)ps_raw);
        } else {
            if (ps_ctl & RPR0521RS_AMB_IR_MASK) {
                ps_data = RPR0521RS_PS_FAR;
            }
            //make parameter
            switch (ps_data) {
                case RPR0521RS_PS_KEEP:
                    SNS_INST_PRINTF(LOW, this, "RPR0521RS_PS_KEEP");
                    event_send = false;
                    break;
                case RPR0521RS_PS_NEAR:
                    SNS_INST_PRINTF(LOW, this, "RPR0521RS_PS_NEAR");
                    event_send = true;
                    ps_event.proximity_event_type = SNS_PROXIMITY_EVENT_TYPE_NEAR;
                    set_l_threshold = state->ps_info.thresh_far;
                    set_h_threshold = RPR0521RS_PS_MAX_VALUE;
                    break;
                case RPR0521RS_PS_FAR:
                    SNS_INST_PRINTF(LOW, this, "RPR0521RS_PS_FAR");
                    event_send = true;
                    ps_event.proximity_event_type = SNS_PROXIMITY_EVENT_TYPE_FAR;
                    set_l_threshold = 0U;
                    set_h_threshold = state->ps_info.thresh_near;
                    break;
                default:
                    SNS_INST_PRINTF(LOW, this, "SNS_RC_INVALID_STATE");
                    event_send = false;
                    result = SNS_RC_INVALID_STATE;
                    break;
            }
            if ((result == SNS_RC_SUCCESS) && (event_send)) {
                (void)rpr0521rs_device_set_threshold(state, set_h_threshold, set_l_threshold, RPR0521RS_REG_PS_TH_L);
                ps_event.raw_adc = (uint32_t)ps_raw;
                ps_event.status  = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
                pb_send_event(this, sns_proximity_event_fields, &ps_event, ts, SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT, &state->ps_info.common.suid);
            }
            
            rpr0521rs_make_raw_log(this, (float_t *)&ps_event.raw_adc, ts, RPR0521RS_PS);
            
            if (state->ps_info.common.heart_beat_timeout == 0U) {
                // non process
            } else {
                state->ps_info.common.heart_beat_count = 0U;
                rpr0521rs_inst_create_heart_beat_timer(this, &state->ps_info.common);
            }
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "Error: Can't get the data from GPIO.");
    }
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_als_send_data
// FUNCTION  : send sensor data to application for Proximity
// param[i]  : this : Sensor instance reference
// param[i]  : ts   : time stamp
// REMARKS   : 
// RETURN    : sns_rc
//==============================================================================
static sns_rc rpr0521rs_als_send_data(sns_sensor_instance *const this, sns_time ts)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    sns_rc   result;
    ALS_DATA raw_data;
    float_t  als_data_raw = 0.0F;
    float_t  als_data_cal = 0.0F;
    float_t  als_send_data[SEND_DATA_SIZE] = {0.0F, 0.0F};
    float_t  als_raw_data[SEND_DATA_SIZE]  = {0.0F, 0.0F};
    
    result = rpr0521rs_device_als_read_data(state, &raw_data);
    if (result == SNS_RC_SUCCESS) {
        als_data_raw = rpr0521rs_calculate_lx(this, raw_data);
        
        SNS_INST_PRINTF(HIGH, this, "als_data_raw %u", (uint32_t)(als_data_raw*1000));
        // execute factory calibration
        als_data_cal = state->als_info.common.registry_cfg.fac_cal_scale_factor * als_data_raw;
        
        if (state->als_info.common.test_info.test_client_present) {
            state->island_service->api->sensor_instance_island_exit(state->island_service, this);
            rpr0521rs_process_factory_test(this, als_data_raw);
        } else {
            als_send_data[0] = als_data_cal;
            als_send_data[1] = (float_t)raw_data.data0;
            
            (void)pb_send_sensor_stream_event(this,
                                              &state->als_info.common.suid,
                                              ts,
                                              SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                              SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
                                              als_send_data,
                                              ARR_SIZE(als_send_data),
                                              state->encoded_als_event_len);
            (void)rpr0521rs_als_threshold_ctl(state, raw_data.data0);
            
            als_raw_data[0] = als_data_raw;
            als_raw_data[1] = (float_t)raw_data.data0;
            rpr0521rs_make_raw_log(this, als_raw_data, ts, RPR0521RS_ALS);
            
            if (state->als_info.common.heart_beat_timeout == 0U) {
                // non process
            } else {
                state->als_info.common.heart_beat_count = 0U;
                rpr0521rs_inst_create_heart_beat_timer(this, &state->als_info.common);
            }
        }
    } else {
        SNS_INST_PRINTF(ERROR, this, "Error: Can't get the data.");
    }
    
    return (result);
}
//==============================================================================
// NAME      : rpr0521rs_calculate_lx
// FUNCTION  : calculate lx value
// param[i]  : this     : Sensor instance reference
// param[i]  : raw_data : adc value
// REMARKS   : 
// RETURN    : lx value
//==============================================================================
static float_t rpr0521rs_calculate_lx(sns_sensor_instance *const this, ALS_DATA raw_data)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    uint8_t cnt  = 0U;
    float_t lx   = 0.0F;
    
    do {
        if ((int64_t)(raw_data.data1 * DIGIT_COEFF) < ((int64_t)((state->als_info.coefficient.juge[cnt] * DIGIT_COEFF) * raw_data.data0))) {
            break;
        } else {
            cnt++;
        }
    } while (cnt < RPR0521RS_ALS_CASE_PATTERN);
    
    if (cnt < RPR0521RS_ALS_CASE_PATTERN) {
        lx = ((state->als_info.coefficient.d0[cnt] * raw_data.data0) / state->als_info.gain0) - ((state->als_info.coefficient.d1[cnt] * raw_data.data1) / state->als_info.gain1);
    } else {
        lx = 0.0F;
    }
    
    if (lx < 0) {
        lx = 0.0F;
    }
    
    return (lx);
}

//==============================================================================
// NAME      : rpr0521rs_make_raw_log
// FUNCTION  : make raw log
// param[i]  : this        : Sensor instance reference
// param[i]  : raw_data    : raw data
// param[i]  : ts          : time stamp
// param[i]  : sensor_type : Sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_make_raw_log(sns_sensor_instance *const this, float_t *raw_data, sns_time ts, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    LOG_SENSOR_STATE_RAW_INFO    log_state_raw_info;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }
    
    (void)sns_memzero(&log_state_raw_info, sizeof(log_state_raw_info));
    (void)rpr0521rs_log_sensor_state_raw_alloc(this, &sensor_info->suid, &log_state_raw_info);
    (void)rpr0521rs_log_sensor_state_raw_add(this, &log_state_raw_info, raw_data, ts, SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH, sensor_type);
    rpr0521rs_log_sensor_state_raw_submit(&log_state_raw_info, sensor_type);

    return;
}

//==============================================================================
// NAME      : rpr0521rs_set_polling_config
// FUNCTION  : update sensor polling configuration
// param[i]  : this        : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_set_polling_config(sns_sensor_instance *const this, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }

    if (sensor_info->sampling_intvl == 0U) {
        if (sensor_info->timer_is_active) {
            sensor_info->timer_is_active = false;
            (void)sns_sensor_util_remove_sensor_instance_stream(this, &sensor_info->timer_data_stream);
        }
    } else {
        rpr0521rs_start_sensor_polling_timer(this, sensor_info);
    }
    
    if (sensor_info->heart_beat_timeout == 0U) {
        if (sensor_info->heart_beat_timer_is_active) {
            (void)sns_sensor_util_remove_sensor_instance_stream(this, &sensor_info->timer_heart_beat_data_stream);
        }
    } else {
        rpr0521rs_inst_create_heart_beat_timer(this, sensor_info);
    }
    
    return;
}

///==============================================================================
// NAME      : rpr0521rs_change_hz_to_ms
// FUNCTION  : change hz to msec
// param[i]  : this  : Sensor instance reference
// speed[i]  : speed : sampling rate[Hz]
// REMARKS   : 
// RETURN    : change[msec]
//==============================================================================
static uint16_t rpr0521rs_change_hz_to_ms(sns_sensor_instance *const this, float_t speed)
{
    UNUSED_VAR(this);
    
    uint16_t change = 0U; 
    
    if (speed == 0.0F){
        change = 0U;
    } else {
        change = (uint16_t)(1000.0F / speed); // unit is ms
        if (change == 0U) {
            change = 1U;
        }
    }
    
    return change;
}

//==============================================================================
// NAME      : rpr0521rs_reconfig_hw
// FUNCTION  : start data streaming and timer
// param[i]  : this : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_reconfig_hw(sns_sensor_instance *this)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    rpr0521rs_data_streaming_control(state);
    rpr0521rs_set_polling_config(this, RPR0521RS_ALS);
    rpr0521rs_set_polling_config(this, RPR0521RS_PS);
    rpr0521rs_send_config_event(this);
    rpr0521rs_dump_reg(this);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_send_config_event
// FUNCTION  : send config update event
// param[i]  : instance : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_send_config_event(sns_sensor_instance *const instance)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;

    sns_std_sensor_physical_config_event phy_sensor_config = sns_std_sensor_physical_config_event_init_default;

    char operating_mode[] = "RPR0521RS_HIGH_PERF";

    pb_buffer_arg op_mode_args;
    if (state->sensor_type_info.enabled & (uint8_t)RPR0521RS_ALS) {
        op_mode_args.buf     = &operating_mode[0];
        op_mode_args.buf_len = sizeof(operating_mode);

        if (state->als_info.common.is_dri) {
            phy_sensor_config.has_sample_rate         = false;
        } else {
            phy_sensor_config.has_sample_rate         = true;
            phy_sensor_config.sample_rate             = state->als_info.common.sampling_rate_hz;
        }
        phy_sensor_config.has_water_mark              = false;
        phy_sensor_config.water_mark                  = 0U;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg          = &op_mode_args;
        phy_sensor_config.has_active_current          = true;
        phy_sensor_config.active_current              = (uint32_t)RPR0521RS_ALS_ACTIVE_CURRENT;
        phy_sensor_config.has_resolution              = true;
        phy_sensor_config.resolution                  = RPR0521RS_ALS_RESOLUTION_X1;
        phy_sensor_config.range_count                 = 2U;
        phy_sensor_config.range[0]                    = RPR0521RS_LUX_RANGE_X1_MIN;
        phy_sensor_config.range[1]                    = RPR0521RS_LUX_RANGE_X1_MAX;
        phy_sensor_config.has_stream_is_synchronous   = false;
        phy_sensor_config.stream_is_synchronous       = false;
        phy_sensor_config.has_dri_enabled             = state->als_info.common.is_dri;
        phy_sensor_config.dri_enabled                 = state->als_info.common.is_dri;
        // For sensors that route data through the SDC/DAE sensor, the DAE watermark
        // should be set to max requested watermark.
        phy_sensor_config.has_DAE_watermark = false;

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->als_info.common.suid);
    } 
    if (state->sensor_type_info.enabled & (uint8_t)RPR0521RS_PS) {
        op_mode_args.buf     = &operating_mode[0];
        op_mode_args.buf_len = sizeof(operating_mode);

        if (state->ps_info.common.is_dri) {
            phy_sensor_config.has_sample_rate         = false;
        } else {
            phy_sensor_config.has_sample_rate         = true;
            phy_sensor_config.sample_rate             = state->ps_info.common.sampling_rate_hz;
        }
        phy_sensor_config.has_water_mark              = false;
        phy_sensor_config.water_mark                  = 0U;
        phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
        phy_sensor_config.operation_mode.arg          = &op_mode_args;
        phy_sensor_config.has_active_current          = true;
        phy_sensor_config.active_current              = (uint32_t)RPR0521RS_PS_ACTIVE_CURRENT;
        phy_sensor_config.has_resolution              = true;
        phy_sensor_config.resolution                  = 1.0F;
        phy_sensor_config.range_count                 = 2U;
        phy_sensor_config.range[0]                    = 0.0F;
        phy_sensor_config.range[1]                    = 1.0F;
        phy_sensor_config.has_stream_is_synchronous   = false;
        phy_sensor_config.stream_is_synchronous       = false;
        phy_sensor_config.has_dri_enabled             = state->ps_info.common.is_dri;
        phy_sensor_config.dri_enabled                 = state->ps_info.common.is_dri;
        // For sensors that route data through the SDC/DAE sensor, the DAE watermark
        // should be set to max requested watermark.
        phy_sensor_config.has_DAE_watermark = false;

        pb_send_event(instance,
                      sns_std_sensor_physical_config_event_fields,
                      &phy_sensor_config,
                      sns_get_system_time(),
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                      &state->ps_info.common.suid);
    } 

    return;
}

//==============================================================================
// NAME      : rpr0521rs_log_sensor_state_raw_alloc
// FUNCTION  : allocate sensor state raw log packet
// param[i]  : this         : Sensor instance reference
// param[i]  : suid         : sensor uid
// param[o]  : log_raw_info : pointer to log raw buffer
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_log_sensor_state_raw_alloc(sns_sensor_instance *const this, sns_sensor_uid *suid, LOG_SENSOR_STATE_RAW_INFO *log_raw_info)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE *)this->state->state;
    
    sns_diag_service *diag = state->diag_service;

    log_raw_info->diag                = diag;
    log_raw_info->instance            = this;
    log_raw_info->sensor_uid          = suid;
    log_raw_info->log_size            = log_raw_info->diag->api->get_max_log_size(log_raw_info->diag);
    log_raw_info->log                 = log_raw_info->diag->api->alloc_log(log_raw_info->diag, log_raw_info->instance, log_raw_info->sensor_uid, log_raw_info->log_size, SNS_DIAG_SENSOR_STATE_LOG_RAW);
    log_raw_info->log_sample_cnt      = 0U;
    log_raw_info->bytes_written       = 0U;
    log_raw_info->encoded_sample_size = state->log_raw_encoded_size;

    return SNS_RC_SUCCESS;
}

//==============================================================================
// NAME      : rpr0521rs_log_sensor_state_raw_submit
// FUNCTION  : submit the sensor state raw log packet
// param[i]  : log_raw_info : pointer to log raw buffer
// param[i]  : sensor_type  : sensor type
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_log_sensor_state_raw_submit(LOG_SENSOR_STATE_RAW_INFO *log_raw_info, RPR0521RS_SENSOR_TYPE sensor_type)
{
    if (NULL == log_raw_info) {
        return;
    } else {
        if ((NULL == log_raw_info->diag) || (NULL == log_raw_info->instance) || (NULL == log_raw_info->sensor_uid) || (NULL == log_raw_info->log)) {
            return;
        }
    }
    
    if (sensor_type == RPR0521RS_ALS) {
        RPR0521RS_ALS_SAMPLE *sample = (RPR0521RS_ALS_SAMPLE *)log_raw_info->log;
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;

        log_raw_info->diag->api->submit_log( log_raw_info->diag,
                                             log_raw_info->instance,
                                             log_raw_info->sensor_uid,
                                             log_raw_info->bytes_written,
                                             log_raw_info->log,
                                             SNS_DIAG_SENSOR_STATE_LOG_RAW,
                                             log_raw_info->encoded_sample_size,
                                             rpr0521rs_als_encode_log_sensor_state_raw );
    } else {
        RPR0521RS_PS_SAMPLE *sample = (RPR0521RS_PS_SAMPLE *)log_raw_info->log;
        sample->sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;

        log_raw_info->diag->api->submit_log( log_raw_info->diag,
                                             log_raw_info->instance,
                                             log_raw_info->sensor_uid,
                                             log_raw_info->bytes_written,
                                             log_raw_info->log,
                                             SNS_DIAG_SENSOR_STATE_LOG_RAW,
                                             log_raw_info->encoded_sample_size,
                                             rpr0521rs_ps_encode_log_sensor_state_raw );
    }

    log_raw_info->log = NULL;

    return;
}

//==============================================================================
// NAME      : rpr0521rs_log_sensor_state_raw_add
// FUNCTION  : add raw uncalibrated sensor data to Sensor State Raw log packet
// param[i]  : log_raw_info : pointer to logging information pertaining to the sensor
// param[i]  : raw_data     : uncalibrated sensor data to be logged
// param[i]  : timestamp    : timestamp of the sensor data
// param[i]  : status       : status of the sensor data
// param[i]  : sensor_type  : sensor type
// REMARKS   :
// RETURN    : sns_rc
//           : SNS_RC_SUCCESS : if encoding was succesful
//           : SNS_RC_FAILED  : otherwise
//==============================================================================
static sns_rc rpr0521rs_log_sensor_state_raw_add(sns_sensor_instance *const this, LOG_SENSOR_STATE_RAW_INFO *log_raw_info, float_t *raw_data, sns_time timestamp, sns_std_sensor_sample_status status, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE *)this->state->state;
    
    sns_rc rc = SNS_RC_SUCCESS;

    if (NULL == log_raw_info) {
        return (SNS_RC_FAILED);
    } else {
        if ((NULL == log_raw_info->diag) || (NULL == log_raw_info->instance) || (NULL == log_raw_info->sensor_uid) || (NULL == log_raw_info->log)) {
            return (SNS_RC_FAILED);
        }
    }
    
    // depends on sensor_type
    if (sensor_type == RPR0521RS_ALS) {
        if ((log_raw_info->bytes_written + sizeof(RPR0521RS_ALS_SAMPLE)) > log_raw_info->log_size) {
            // no more space in log packet
            // submit and allocate a new one
            rpr0521rs_log_sensor_state_raw_submit(log_raw_info, RPR0521RS_ALS);
            (void)rpr0521rs_log_sensor_state_raw_alloc(this, &state->als_info.common.suid, log_raw_info);
        }
    } else {
        if ((log_raw_info->bytes_written + sizeof(RPR0521RS_PS_SAMPLE)) > log_raw_info->log_size) {
            // no more space in log packet
            // submit and allocate a new one
            rpr0521rs_log_sensor_state_raw_submit(log_raw_info, RPR0521RS_PS);
            (void)rpr0521rs_log_sensor_state_raw_alloc(this, &state->ps_info.common.suid, log_raw_info);
        }
    }
    
    if (NULL == log_raw_info->log) {
        rc = SNS_RC_FAILED;
    } else {
        // depend on sensor_type
        if (sensor_type == RPR0521RS_ALS) {
            RPR0521RS_ALS_SAMPLE *sample = (RPR0521RS_ALS_SAMPLE *)log_raw_info->log;
            
            if (0U == log_raw_info->batch_sample_cnt) {
                sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
            } else {
                sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
            }
            (void)sns_memscpy(sample[log_raw_info->log_sample_cnt].sample, sizeof(sample[log_raw_info->log_sample_cnt].sample), raw_data, sizeof(sample[log_raw_info->log_sample_cnt].sample));
            sample[log_raw_info->log_sample_cnt].timestamp = timestamp;
            sample[log_raw_info->log_sample_cnt].status    = status;
            
            log_raw_info->bytes_written += sizeof(RPR0521RS_ALS_SAMPLE);
            log_raw_info->log_sample_cnt++;
            log_raw_info->batch_sample_cnt++;
        } else {
            RPR0521RS_PS_SAMPLE *sample = (RPR0521RS_PS_SAMPLE *)log_raw_info->log;
            
            if (0U == log_raw_info->batch_sample_cnt) {
                sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
            } else {
                sample[log_raw_info->log_sample_cnt].sample_type = SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
            }
            sample[log_raw_info->log_sample_cnt].sample    = *raw_data;
            sample[log_raw_info->log_sample_cnt].timestamp = timestamp;
            sample[log_raw_info->log_sample_cnt].status    = status;
            
            log_raw_info->bytes_written += sizeof(RPR0521RS_PS_SAMPLE);
            log_raw_info->log_sample_cnt++;
            log_raw_info->batch_sample_cnt++;
        }
    }

    return rc;
}

//==============================================================================
// NAME      : rpr0521rs_als_encode_log_sensor_state_raw
// FUNCTION  : encode log sensor state raw packet
// param[i]  : log_info         : pointer to log packet information
//           : log_size         : size of log packet information
// param[i]  : encoded_log_size : maximum permitted encoded size of the log
// param[o]  : encoded_log      : pointer to location where encoded log should
//           :                  : be generated
// param[o]  : bytes_written    : pointer to actual bytes written during
//           :                  : encode
// REMARKS   :
// RETURN    : sns_rc
//           : SNS_RC_SUCCESS : if encoding was succesful
//           : SNS_RC_FAILED  : otherwise
//==============================================================================
static sns_rc rpr0521rs_als_encode_log_sensor_state_raw(void *log_info, size_t log_size, size_t encoded_log_size, void *encoded_log, size_t *bytes_written)
{
    sns_rc                rc                   = SNS_RC_SUCCESS;
    uint32_t              i                    = 0U;
    size_t                encoded_sample_size  = 0U;
    size_t                parsed_log_size      = 0U;
    sns_diag_batch_sample batch_sample         = sns_diag_batch_sample_init_default;
    uint8_t               arr_index            = 0U;
    float_t               temp[SEND_DATA_SIZE] = {0.0F,};
    pb_float_arr_arg      arg = {.arr = (float_t *)&temp, .arr_len = SEND_DATA_SIZE, .arr_index = &arr_index};

    if ((NULL == encoded_log) || (NULL == log_info) || (NULL == bytes_written)) {
        return (SNS_RC_FAILED);
    }

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg          = &arg;

    if (!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields, &batch_sample)) {
        return (SNS_RC_FAILED);
    }

    pb_ostream_t         stream      = pb_ostream_from_buffer(encoded_log, encoded_log_size);
    RPR0521RS_ALS_SAMPLE *raw_sample = (RPR0521RS_ALS_SAMPLE *)log_info;

    while ((parsed_log_size < log_size) && ((stream.bytes_written + encoded_sample_size) <= encoded_log_size) && (i < (uint32_t)(-1))) {
        arr_index = 0U;
        arg.arr   = (float_t *)&raw_sample[i].sample;

        batch_sample.sample_type = raw_sample[i].sample_type;
        batch_sample.status      = raw_sample[i].status;
        batch_sample.timestamp   = raw_sample[i].timestamp;

        if (!pb_encode_tag(&stream, PB_WT_STRING, (uint32_t)sns_diag_sensor_state_raw_sample_tag)) {
            rc = SNS_RC_FAILED;
            break;
        } else if (!pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample)) {
            rc = SNS_RC_FAILED;
            break;
        } else {
            // non process
        }

        parsed_log_size += sizeof(RPR0521RS_ALS_SAMPLE);
        i++;
    }

    if (SNS_RC_SUCCESS == rc) {
        *bytes_written = stream.bytes_written;
    }

    return (rc);
}

//==============================================================================
// NAME      : rpr0521rs_ps_encode_log_sensor_state_raw
// FUNCTION  : encode log sensor state raw packet
// param[i]  : log_info         : pointer to log packet information
//           : log_size         : size of log packet information
// param[i]  : encoded_log_size : maximum permitted encoded size of the log
// param[o]  : encoded_log      : pointer to location where encoded log should
//           :                  : be generated
// param[o]  : bytes_written    : pointer to actual bytes written during
//           :                  : encode
// REMARKS   :
// RETURN    : sns_rc
//           : SNS_RC_SUCCESS : if encoding was succesful
//           : SNS_RC_FAILED  : otherwise
//==============================================================================
static sns_rc rpr0521rs_ps_encode_log_sensor_state_raw(void *log_info, size_t log_size, size_t encoded_log_size, void *encoded_log, size_t *bytes_written)
{
    sns_rc                rc                  = SNS_RC_SUCCESS;
    uint32_t              i                   = 0U;
    size_t                encoded_sample_size = 0U;
    size_t                parsed_log_size     = 0U;
    sns_diag_batch_sample batch_sample        = sns_diag_batch_sample_init_default;
    uint8_t               arr_index           = 0U;
    float_t               temp                = 0.0F;
    pb_float_arr_arg      arg = {.arr = (float_t *)&temp, .arr_len = 1, .arr_index = &arr_index};

    if ((NULL == encoded_log) || (NULL == log_info) || (NULL == bytes_written)) {
        return (SNS_RC_FAILED);
    }

    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg          = &arg;

    if (!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields, &batch_sample)) {
        return (SNS_RC_FAILED);
    }

    pb_ostream_t         stream     = pb_ostream_from_buffer(encoded_log, encoded_log_size);
    RPR0521RS_PS_SAMPLE *raw_sample = (RPR0521RS_PS_SAMPLE *)log_info;

    while ((parsed_log_size < log_size) && ((stream.bytes_written + encoded_sample_size) <= encoded_log_size) && (i < (uint32_t)(-1))) {
        arr_index = 0U;
        arg.arr   = (float_t *)&raw_sample[i].sample;

        batch_sample.sample_type = raw_sample[i].sample_type;
        batch_sample.status      = raw_sample[i].status;
        batch_sample.timestamp   = raw_sample[i].timestamp;

        if (!pb_encode_tag(&stream, PB_WT_STRING, (uint32_t)sns_diag_sensor_state_raw_sample_tag)) {
            rc = SNS_RC_FAILED;
            break;
        } else if (!pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample)) {
            rc = SNS_RC_FAILED;
            break;
        } else {
            // non process
        }

        parsed_log_size += sizeof(RPR0521RS_PS_SAMPLE);
        i++;
    }

    if (SNS_RC_SUCCESS == rc) {
        *bytes_written = stream.bytes_written;
    }

    return (rc);
}

//==============================================================================
// NAME       : rpr0521rs_send_cal_event
// FUNCTION   : send factory calibration parameter to framework
// param[in]  : instance    : instance reference
// param[in]  : sensor_type : sensors to handle
// REMARKS    :
// RETURN     : none
//==============================================================================
void rpr0521rs_send_cal_event(sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE *)instance->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    sns_cal_event new_calibration_event = sns_cal_event_init_default;

    float_t bias_data  = 0.0F;
    float_t scale_data = 0.0F;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
        
    }
    
    bias_data  = sensor_info->registry_cfg.fac_cal_bias;
    scale_data = sensor_info->registry_cfg.fac_cal_scale_factor;
    
    pb_buffer_arg buff_arg_bias  = (pb_buffer_arg){ .buf = &bias_data,  .buf_len = sizeof(bias_data) };
    pb_buffer_arg buff_arg_scale = (pb_buffer_arg){ .buf = &scale_data, .buf_len = sizeof(scale_data) };
    
    new_calibration_event.bias.funcs.encode         = &pb_encode_float_arr_cb;
    new_calibration_event.bias.arg                  = &buff_arg_bias;
    new_calibration_event.scale_factor.funcs.encode = &pb_encode_float_arr_cb;
    new_calibration_event.scale_factor.arg          = &buff_arg_scale;
    new_calibration_event.status                    = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
    
    pb_send_event(instance, sns_cal_event_fields, &new_calibration_event, sns_get_system_time(), SNS_CAL_MSGID_SNS_CAL_EVENT, &sensor_info->suid);

    return;
}

