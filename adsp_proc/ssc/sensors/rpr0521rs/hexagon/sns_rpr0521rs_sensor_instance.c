//==============================================================================
// MODULE        : sns_rpr0521_sensor_instance.c
// FUNCTION      : RPR0521RS Sensor Instance implementation.
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Nov/09/2017
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
#include "sns_rpr0521rs_sensor_instance.h"
#include "sns_stream_service.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_service_manager.h"
#include "pb_encode.h"
#include "sns_diag.pb.h"
#include "sns_sensor_util.h"
#include "sns_printf.h"

//==============================================================================
// function prototype
//==============================================================================
// internal use
static void inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr);
static void rpr0521rs_send_self_test_event(sns_sensor_instance *instance, RPR0521RS_SENSOR_COMMON_INFO *sensor_info, bool test_result, sns_physical_sensor_test_type type);
static void rpr0521rs_start_factory_test(sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type);

//==============================================================================
// NAME      : rpr0521rs_inst_init
// FUNCTION  : initialize sensor instance
// param[i]  : this   : Sensor instance reference
// param[i]  : sstate : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
sns_rc rpr0521rs_inst_init(sns_sensor_instance *const this, sns_sensor_state const *sstate)
{
    RPR0521RS_INSTANCE_STATE *state        = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    RPR0521RS_STATE          *sensor_state = (RPR0521RS_STATE*)sstate->state;
    
    float_t  als_data_size[2] = {0.0F, 0.0F};
    float_t  ps_data_size[2]  = {0.0F, 0.0F};
    float_t  diag_temp        = 0.0F;
    uint8_t  arr_index        = 0U;
    uint64_t buffer[10];
   
    sns_sensor_uid        irq_suid;
    sns_service_manager   *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service    *stream_mgr  = (sns_stream_service*) service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    pb_ostream_t          stream       = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
    sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
    
    pb_float_arr_arg arg = { .arr = (float_t*)&diag_temp, 
                             .arr_len = 1,
                             .arr_index = &arr_index };
    batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
    batch_sample.sample.arg          = &arg;
    
    state->scp_service    = (sns_sync_com_port_service*) service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
    state->island_service = (sns_island_service*) service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);

    //---------Setup stream connections with dependent Sensors---------
    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "interrupt", &irq_suid);
    stream_mgr->api->create_sensor_instance_stream(stream_mgr, this, irq_suid, &state->interrupt_data_stream);

    
    // Initialize COM port to be used by the Instance 
    (void)sns_memscpy(&state->com_port_info.com_config, sizeof(state->com_port_info.com_config),
                      &sensor_state->common.com_port_info.com_config, sizeof(sensor_state->common.com_port_info.com_config));

    state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config, &state->com_port_info.port_handle);

    if ((NULL == state->interrupt_data_stream) || (NULL == state->com_port_info.port_handle)) {
        inst_cleanup(this, stream_mgr);
        return SNS_RC_FAILED;
    }

    // ----------- Copy all Sensor UIDs in instance state -------------
    (void)sns_memscpy(&state->als_info.common.suid, sizeof(state->als_info.common.suid), &((sns_sensor_uid)ALS_SUID), sizeof(state->als_info.common.suid));
    (void)sns_memscpy(&state->ps_info.common.suid, sizeof(state->ps_info.common.suid), &((sns_sensor_uid)PS_SUID), sizeof(state->ps_info.common.suid));

    sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer", &state->timer_suid);
    
    // -------------------------Init ALS State-------------------------
    state->als_info.common.curr_intvl_ms  = RPR0521RS_ALS_ODR_MS_100;
    state->als_info.common.sampling_intvl = 0U;
    state->als_info.gain0                 = RPR0521RS_ALS_GAIN_X1;
    state->als_info.gain1                 = RPR0521RS_ALS_GAIN_X1;
    state->als_info.coefficient           = sensor_state->tab_coef;
    state->als_info.thresh_high           = RPR0521RS_INIT_ALS_THH;
    state->als_info.thresh_low            = RPR0521RS_INIT_ALS_THL;
    
    // -------------------------Init PS State-------------------------
    state->ps_info.common.curr_intvl_ms   = RPR0521RS_PS_ODR_MS_100;
    state->ps_info.common.sampling_intvl  = 0U;
    state->ps_info.ori_thresh_near        = (uint16_t)sensor_state->near_threshold;
    state->ps_info.ori_thresh_far         = (uint16_t)sensor_state->far_threshold;
    
    // -------------------------Init common State-------------------------
    state->irq_info.irq_trig = 0U;
    
    // -------------------------Init Factory Test State-------------------------
    state->fac_test_in_progress       = false;
    state->update_fac_cal_in_registry = false;
    state->als_info.common.registry_cfg.fac_cal_version = 0U;
    state->als_info.common.fac_test_info.reference_val  = RPR0521RS_FAC_TEST_ALS_REF_VAL;
    state->ps_info.common.registry_cfg.fac_cal_version  = 0U;
    state->ps_info.common.fac_test_info.reference_val   = RPR0521RS_FAC_TEST_PS_REF_VAL;
    
    state->encoded_als_event_len = pb_get_encoded_size_sensor_stream_event(als_data_size, 2U);
    state->encoded_ps_event_len  = pb_get_encoded_size_sensor_stream_event(ps_data_size,  2U);

    state->diag_service = (sns_diag_service*) service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
    state->scp_service  = (sns_sync_com_port_service*) service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

    state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
    
    (void)rpr0521rs_device_init(state);
    
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    
    SNS_INST_PRINTF(LOW, this, "Set parameter als received d0[0]:%u d1[0]:%u juge[0]:%u. "
                               "Set parameter ps threshold received near:%u far:%u", 
                               (uint32_t)(state->als_info.coefficient.d0[0] * 1000),
                               (uint32_t)(state->als_info.coefficient.d1[0] * 1000), 
                               (uint32_t)(state->als_info.coefficient.juge[0] * 1000),
                               (uint32_t)state->ps_info.ori_thresh_near, 
                               (uint32_t)state->ps_info.ori_thresh_far);

    // Initialize IRQ info to be used by the Instance 
    state->irq_info.irq_config     = sensor_state->common.irq_config;
    state->irq_info.irq_ready      = false;
    state->irq_info.irq_registered = false;

    // Determine size of sns_diag_sensor_state_raw as defined in
    //  sns_diag.proto
    //  sns_diag_sensor_state_raw is a repeated array of samples of
    //  type sns_diag_batch sample. The following determines the
    //  size of sns_diag_sensor_state_raw with a single batch
    //  sample 
    if (pb_encode_tag(&stream, PB_WT_STRING, (uint32_t)sns_diag_sensor_state_raw_sample_tag)) {
        if (pb_encode_delimited(&stream, sns_diag_batch_sample_fields, &batch_sample)) {
            state->log_raw_encoded_size = stream.bytes_written;
        }
    }
    
    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : rpr0521rs_inst_deinit
// FUNCTION  : remove sensor instance state parameter
// param[i]  : this : Sensor instance reference
// REMARKS   :
// RETURN    : none
//==============================================================================
sns_rc rpr0521rs_inst_deinit(sns_sensor_instance *const this)
{
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service  *stream_mgr  = (sns_stream_service*)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    
    inst_cleanup(this, stream_mgr);

    return (SNS_RC_SUCCESS);
}

//==============================================================================
// NAME      : inst_cleanup
// FUNCTION  : cleanup sensor instance
// param[i]  : this       : Sensor instance reference
// param[i]  : stream_mgr : stream manager
// REMARKS   :
// RETURN    : none
//==============================================================================
static void inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr)
{
    UNUSED_VAR(stream_mgr);
    
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;
    
    if (NULL != state->com_port_info.port_handle) {
        state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
    }
    
    rpr0521rs_reconfig_hw(this);
    
    if (NULL != state->com_port_info.port_handle) {
        state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
    }
    
    (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->interrupt_data_stream);
    state->irq_info.irq_registered = false;
    
    (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->als_info.common.timer_data_stream);
    (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->als_info.common.timer_heart_beat_data_stream);
    (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->ps_info.common.timer_data_stream);
    (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->ps_info.common.timer_heart_beat_data_stream);
    
    if (NULL != state->scp_service) {
        state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
        state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
        state->scp_service = NULL;
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_run_self_test
// FUNCTION  : execute requested self-tests
// param[i]  : instance    : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_run_self_test(sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    sns_rc  rv               = SNS_RC_SUCCESS;
    uint8_t who_am_i         = 0U;
    bool    who_am_i_success = false;

    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }
    
    if (sensor_info->test_info.test_client_present) {
        switch (sensor_info->test_info.test_type) {
            case SNS_PHYSICAL_SENSOR_TEST_TYPE_COM :
                SNS_INST_PRINTF(HIGH, instance, "test_type is SNS_PHYSICAL_SENSOR_TEST_TYPE_COM");
                if (state->fac_test_in_progress) {
                    SNS_INST_PRINTF(ERROR, instance, "ERROR : FACTORY TEST is already running !!!");
                }
                
                rv = rpr0521rs_check_who_am_i(state->scp_service, state->com_port_info.port_handle, &who_am_i);
                if (rv == SNS_RC_SUCCESS) {
                    who_am_i_success = true;
                    SNS_INST_PRINTF(HIGH, instance, "SNS_PHYSICAL_SENSOR_TEST_TYPE_COM is success");
                }
                
                rpr0521rs_send_self_test_event(instance, sensor_info, who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM);
                break;
            
            case SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY :
                SNS_INST_PRINTF(HIGH, instance, "test_type is SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY");
                // Handle factory test. The driver may choose to reject any new
                // streaming/self-test requests when factory test is in progress.
                if (state->fac_test_in_progress) {
                    SNS_INST_PRINTF(ERROR, instance, "ERROR : FACTORY TEST is already running !!!");
                } else {
                    sns_memset(sensor_info->fac_test_info.store_samples, 0, sizeof(sensor_info->fac_test_info.store_samples));
                    sensor_info->fac_test_info.sample_sum = 0.0F;
                    state->fac_test_num_samples           = 0U;
                    rpr0521rs_start_factory_test(instance, sensor_type);
                }
                break;
            
            default :
                SNS_INST_PRINTF(ERROR, instance, "ERROR : Driver does not implement the test type");
                sensor_info->test_info.test_client_present = false;
                break;
        }
    } else {
        SNS_INST_PRINTF(ERROR, instance, "Non test present");
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_send_self_test_event
// FUNCTION  : send a self-test result
// param[i]  : instance    : Sensor instance reference
// param[i]  : sensor_info : Sensor instance common info
// param[i]  : test_result : test result
// param[i]  : type        : test type
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_send_self_test_event(sns_sensor_instance *instance, RPR0521RS_SENSOR_COMMON_INFO *sensor_info, bool test_result, sns_physical_sensor_test_type type)
{
    uint8_t data = 0U;
    
    pb_buffer_arg                  buff_arg   = (pb_buffer_arg){ .buf = &data, .buf_len = sizeof(data) };
    sns_physical_sensor_test_event test_event = sns_physical_sensor_test_event_init_default;
    
    test_event.test_passed            = test_result;
    test_event.test_type              = type;
    test_event.test_data.funcs.encode = &pb_encode_string_cb;
    test_event.test_data.arg          = &buff_arg;

    pb_send_event(instance,
                        sns_physical_sensor_test_event_fields,
                        &test_event,
                        sns_get_system_time(),
                        SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                        &sensor_info->suid);

    sensor_info->test_info.test_client_present = false;

    SNS_INST_PRINTF(LOW, instance, "test_client_present = %u test_result %d type %d", sensor_info->test_info.test_client_present, (int32_t)test_result, (int32_t)type);

    return;
}

//==============================================================================
// NAME      : rpr0521rs_start_factory_test
// FUNCTION  : run a factory test
// param[i]  : instance    : Sensor instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_start_factory_test(sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    uint16_t intvl_ms;
    uint16_t raw_intvl = 0U;
    
    state->fac_test_sensor      = sensor_type;
    state->fac_test_in_progress = true;
    
    if (state->fac_test_sensor == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
        intvl_ms    = RPR0521RS_ALS_ODR_MS_100;
        state->irq_info.irq_trig &= ~RPR0521RS_INT_TRIG_ALS;
        (void)rpr0521rs_als_setting_register(state, intvl_ms, &raw_intvl);
    } else {
        sensor_info = &state->ps_info.common;
        intvl_ms    = RPR0521RS_PS_ODR_MS_50;
        state->irq_info.irq_trig &= ~RPR0521RS_INT_TRIG_PS;
        (void)rpr0521rs_ps_setting_register(state, intvl_ms, &raw_intvl);
    }
    
    state->sensor_type_info.enabled |= state->fac_test_sensor;
    sensor_info->sampling_intvl      = (sns_time)sns_convert_ns_to_ticks((uint64_t)((raw_intvl + RPR0521RS_FAC_TEST_T_MEAS_MARGIN) * 1000000ULL));
    sensor_info->heart_beat_timeout  = sensor_info->sampling_intvl * RPR0521RS_HBT_POLLING;
    
    (void)rpr0521rs_device_set_interrupt(state);
    rpr0521rs_reconfig_hw(instance);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_process_factory_test
// FUNCTION  : test for validation of sensor
// param[i]  : 
// REMARKS   : 
// RETURN    : none
//==============================================================================
void rpr0521rs_process_factory_test(sns_sensor_instance *const this, float_t sample)
{
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)this->state->state;

    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    bool    fac_test_result  = false;
    uint8_t i                = 0U;
    float_t average_samples  = 0.0F;
    float_t fac_test_limit_l = 0.0F;
    float_t fac_test_limit_h = 0.0F;
    
    if (state->fac_test_sensor == RPR0521RS_ALS) {
        sensor_info = &state->als_info.common;
    } else {
        sensor_info = &state->ps_info.common;
    }
    
    rpr0521rs_inst_create_heart_beat_timer(this, sensor_info);
    
    sensor_info->fac_test_info.store_samples[state->fac_test_num_samples] = sample;
    sensor_info->fac_test_info.sample_sum += sample;
    
    if (state->fac_test_num_samples >= (RPR0521RS_FAC_TEST_SAMPLE_CNT - 1U)) {
        average_samples  = sensor_info->fac_test_info.sample_sum / (float_t)RPR0521RS_FAC_TEST_SAMPLE_CNT;
        fac_test_limit_l = average_samples * RPR0521RS_FAC_TEST_LIMIT_L_PARA;
        fac_test_limit_h = average_samples * RPR0521RS_FAC_TEST_LIMIT_H_PARA;
        for (i=0U; i<RPR0521RS_FAC_TEST_SAMPLE_CNT; i++) {
            SNS_INST_PRINTF(LOW, this, "store_samples[%u] = %u[lx](1000)", i, (uint32_t)(sensor_info->fac_test_info.store_samples[i]*1000));
            fac_test_result = false;
            if ((sensor_info->fac_test_info.store_samples[i] > fac_test_limit_l) && (sensor_info->fac_test_info.store_samples[i] < fac_test_limit_h)) {
                fac_test_result = true;
				break;
            }
        }
        state->update_fac_cal_in_registry = fac_test_result;
        state->fac_test_in_progress       = false;
    } else {
        state->fac_test_num_samples = (state->fac_test_num_samples + 1U) % RPR0521RS_FAC_TEST_SAMPLE_CNT;
    }
    
    if (state->update_fac_cal_in_registry) {
        SNS_INST_PRINTF(HIGH, this, "SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY is success");
        // make factory calibration parameter
        if (state->fac_test_sensor == RPR0521RS_ALS) {
            sensor_info->registry_cfg.fac_cal_scale_factor = sensor_info->fac_test_info.reference_val / average_samples;
        } else {
            sensor_info->registry_cfg.fac_cal_bias = average_samples - sensor_info->fac_test_info.reference_val;
        }
        sensor_info->registry_cfg.fac_cal_version++;
        rpr0521rs_send_cal_event(this, state->fac_test_sensor);
    }
    
    // After factory test, this function is run
    if (!state->fac_test_in_progress) {
        rpr0521rs_recovery_device_state(this);
        state->sensor_type_info.enabled = 0U;
        if (state->als_info.common.heart_beat_timer_is_active) {
            (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->als_info.common.timer_heart_beat_data_stream);
        }
        if (state->ps_info.common.heart_beat_timer_is_active) {
            (void)sns_sensor_util_remove_sensor_instance_stream(this, &state->ps_info.common.timer_heart_beat_data_stream);
        }
        rpr0521rs_send_self_test_event(this, sensor_info, fac_test_result, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY);
    }
    
    return;
}

