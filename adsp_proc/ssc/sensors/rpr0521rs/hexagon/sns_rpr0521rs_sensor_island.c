//==============================================================================
// MODULE        : sns_rpr0521rs_sensor_island.c
// FUNCTION      : Sensor island for RPR0521RS.
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
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_std.pb.h"
#include "sns_com_port_types.h"
#include "sns_diag_service.h"
#include "sns_printf.h"
#include "sns_sensor_util.h"
#include "sns_rpr0521rs_sensor_instance.h"
#include "sns_std_sensor.pb.h"
#include "sns_pwr_rail_service.h"
#include "sns_math_util.h"
#include "sns_timer.pb.h"
#include "pb.h"
#include "sns_pb_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_service_manager.h"
#include "sns_event_service.h"
#include "sns_cal.pb.h"
#include "pb_encode.h"

//==============================================================================
// function prototype
//==============================================================================
// sensor api funciton
static sns_sensor_uid const* rpr0521rs_als_get_sensor_uid(sns_sensor const *const this);
static sns_sensor_uid const* rpr0521rs_ps_get_sensor_uid(sns_sensor const *const this);
static sns_sensor_instance* rpr0521rs_set_client_request(sns_sensor *const this, struct sns_request const *exist_request,  struct sns_request const *new_request, bool except);
static sns_rc rpr0521rs_sensor_notify_event(sns_sensor *const this);

// internal use
static void rpr0521rs_set_self_test_inst_config(sns_sensor *const this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_reval_instance_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type);
static void rpr0521rs_get_sensor_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type, float_t *sampling_rate, bool *sensor_client_present);
static void rpr0521rs_set_inst_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_REQ *instance_config_req, uint32_t message_id);
static void rpr0521rs_turn_rails_off(sns_sensor *this);
static void rpr0521rs_send_fifo_flush_done(sns_sensor_instance *const instance, sns_sensor_uid *suid);
static void rpr0521rs_update_sensor_state(sns_sensor *const this, sns_sensor_instance *const instance);
static void rpr0521rs_reset_cal_data(sns_sensor *const this, sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type);
static bool rpr0521rs_power_on_wait_check(sns_sensor *this, sns_time power_on_timestamp);
static bool rpr0521rs_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request, sns_std_request *decoded_request, sns_physical_sensor_test_config *test_config);
static bool rpr0521rs_get_decoded_request(sns_sensor const *this, sns_request const *in_request, sns_std_request *decoded_request, sns_std_sensor_config *decoded_payload);
static bool rpr0521rs_extract_self_test_info(sns_sensor *this, sns_sensor_instance *instance, struct sns_request const *new_request);

//==============================================================================
// struct define
//==============================================================================
sns_sensor_api rpr0521rs_als_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &rpr0521rs_als_init,
    .deinit             = &rpr0521rs_als_deinit,
    .get_sensor_uid     = &rpr0521rs_als_get_sensor_uid,
    .set_client_request = &rpr0521rs_set_client_request,
    .notify_event       = &rpr0521rs_sensor_notify_event,
};

sns_sensor_api rpr0521rs_ps_sensor_api =
{
    .struct_len         = sizeof(sns_sensor_api),
    .init               = &rpr0521rs_ps_init,
    .deinit             = &rpr0521rs_ps_deinit,
    .get_sensor_uid     = &rpr0521rs_ps_get_sensor_uid,
    .set_client_request = &rpr0521rs_set_client_request,
    .notify_event       = &rpr0521rs_sensor_notify_event,
};

//==============================================================================
// NAME      : rpr0521rs_als_get_sensor_uid
// FUNCTION  : get sensor uid for ALS
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : sensor uid number
//==============================================================================
static sns_sensor_uid const* rpr0521rs_als_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = ALS_SUID;
    
    return &sensor_uid;
}

//==============================================================================
// NAME      : rpr0521rs_ps_get_sensor_uid
// FUNCTION  : get sensor uid for PS
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : sensor uid number
//==============================================================================
static sns_sensor_uid const* rpr0521rs_ps_get_sensor_uid(sns_sensor const *const this)
{
    UNUSED_VAR(this);
    static const sns_sensor_uid sensor_uid = PS_SUID;
    
    return &sensor_uid;
}

//==============================================================================
// NAME      : rpr0521rs_set_client_request
// FUNCTION  : common sensor API
// param[i]  : this          : Sensor reference
// param[i]  : exist_request : existing request
// param[i]  : new_request   : new request
// param[i]  : except        : true to except request
// REMARKS   :
// RETURN    : pointer of sns_sensor_instance
//==============================================================================
static sns_sensor_instance* rpr0521rs_set_client_request(sns_sensor *const this, struct sns_request const *exist_request,  struct sns_request const *new_request, bool except)
{
    sns_sensor_instance      *instance   = sns_sensor_util_get_shared_instance(this);
    RPR0521RS_STATE          *state      = (RPR0521RS_STATE*)this->state->state;
    RPR0521RS_INSTANCE_STATE *inst_state = NULL;
    
    bool     reval_config = false;
    bool     wait_power_on_ok = false;
    sns_time on_timestamp = 0U;
    
    SNS_PRINTF(LOW, this, "set_client_req 0x%X  0x%X  %d", exist_request, new_request, except);
    
    if (except) {
        if (NULL != instance) {
            instance->cb->remove_client_request(instance, exist_request);
            rpr0521rs_reval_instance_config(this, instance, state->sensor);
        }
    } else {
        if (NULL == instance) {
            state->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
            state->power_rail_pend_state = RPR0521RS_POWER_RAIL_PENDING_NONE;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,this, &state->common.rail_config, &on_timestamp);
            
            wait_power_on_ok = rpr0521rs_power_on_wait_check(this, on_timestamp);
            // Use on_timestamp to determine correct Timer value.
            if (wait_power_on_ok) {
                // rail is already ON
                reval_config = true;
            }
            // create_instance() calls init() for the Sensor Instance
            instance = this->cb->create_instance(this, sizeof(RPR0521RS_INSTANCE_STATE));
            // If rail is already ON then flag instance OK to configure
            if (reval_config) {
                inst_state  = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
                inst_state->instance_is_ready_to_configure = true;
            }
        } else {
            inst_state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
            if ((NULL != exist_request) && (NULL != new_request) && (new_request->message_id == (uint32_t)SNS_STD_MSGID_SNS_STD_FLUSH_REQ)) {
                // There aren't any FIFO sensors enabled to support flush.
                //  Send flush complete event anyway.
                rpr0521rs_send_fifo_flush_done(instance, &state->my_suid);
                return (instance);
            } else {
                reval_config = true;
                if (!inst_state->instance_is_ready_to_configure) {
                    
                    wait_power_on_ok = rpr0521rs_power_on_wait_check(this, inst_state->power_on_timestamp);
                    if (wait_power_on_ok) {
                        inst_state->instance_is_ready_to_configure = true;
                    }
                }
                // An existing client is changing request
                if ((NULL != exist_request) && (NULL != new_request)) {
                    instance->cb->remove_client_request(instance, exist_request);
                } else if (NULL != new_request) { // A new client sent new_request
                    // No-op. new_request will be added to requests list below.
                } else {
                    // no process
                }
            }
        }
        
        // Add the new request to list of client_requests.
        if (NULL != instance) {
            inst_state  = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
            inst_state->power_on_timestamp = on_timestamp;
            if (NULL != new_request) {
                instance->cb->add_client_request(instance, new_request);
                switch (new_request->message_id) {
                    case (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG:
                        inst_state->reset_threshold = true;
                        break;
                    
                    case (uint32_t)SNS_CAL_MSGID_SNS_CAL_RESET:
                        rpr0521rs_reset_cal_data(this, instance, state->sensor);
                        rpr0521rs_update_sensor_state(this, instance);
                        // Registry write can be handled in normal mode.
                        state->island_service->api->sensor_island_exit(state->island_service, this);
                        rpr0521rs_update_registry(this, instance, RPR0521RS_ALS);
                        rpr0521rs_update_registry(this, instance, RPR0521RS_PS);
                        rpr0521rs_send_cal_event(instance, state->sensor);
                        inst_state->update_fac_cal_in_registry = false;
                        break;
                    
                    case (uint32_t)SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG:
                        if (rpr0521rs_extract_self_test_info(this, instance, new_request)) {
                            inst_state->new_self_test_request = true;
                        }
                        break;
                    
                    default :
                        // non process
                        break;
                }
            }
            if ((reval_config) && (inst_state->instance_is_ready_to_configure)) {
                rpr0521rs_reval_instance_config(this, instance, state->sensor);
                if (inst_state->new_self_test_request) {
                    rpr0521rs_set_self_test_inst_config(this, instance, state->sensor);
                }
            }
        }
    }
    
    if (NULL != instance) {
        if ((instance->cb->get_client_request(instance, &(sns_sensor_uid)ALS_SUID, true) == NULL) && (instance->cb->get_client_request(instance, &(sns_sensor_uid)PS_SUID, true) == NULL)) {
            inst_state  = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
            if (inst_state->update_fac_cal_in_registry) {
                // Registry write can be handled in normal mode.
                state->island_service->api->sensor_island_exit(state->island_service, this);
                // Update factory cal data in the registry.
                rpr0521rs_update_registry(this, instance, RPR0521RS_ALS);
                rpr0521rs_update_registry(this, instance, RPR0521RS_PS);
                rpr0521rs_update_sensor_state(this, instance);
            }
            
            this->cb->remove_instance(instance);
            rpr0521rs_turn_rails_off(this);
        }
    } else {
        rpr0521rs_turn_rails_off(this);
    }
    
    return (instance);
}

//==============================================================================
// NAME      : rpr0521rs_power_on_wait_check
// FUNCTION  : check whether system waits enough time after power rail on
// param[i]  : this               : Sensor reference
// param[i]  : power_on_timestamp : time stamp at power rail on
// REMARKS   : 
// RETURN    : true or false
//==============================================================================
static bool rpr0521rs_power_on_wait_check(sns_sensor *this, sns_time power_on_timestamp)
{
    bool     result = true;
    sns_time delta  = 0U;
    sns_time temp   = 0U;
    
    delta = sns_get_system_time() - power_on_timestamp;
    temp  = sns_convert_ns_to_ticks(RPR0521RS_OFF_TO_IDLE_NS);
    if (temp > delta) {
        rpr0521rs_start_power_rail_timer(this, temp, RPR0521RS_POWER_RAIL_PENDING_SET_CLIENT_REQ);
        result = false;
    } 
    
    return (result);
}


//==============================================================================
// NAME      : rpr0521rs_reval_instance_config
// FUNCTION  : this function parses the client_request list per Sensor and
//           : determines final config for the Sensor Instance.
// param[i]  : this        : Sensor reference
// param[i]  : instance    : Sensor Instance to config
// param[i]  : sensor_type : Sensor type
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_reval_instance_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_STATE          *state      = (RPR0521RS_STATE*)this->state->state;
    RPR0521RS_INSTANCE_STATE *inst_state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    RPR0521RS_REQ instance_config_req = { .sensor_type          = sensor_type,
                                          .sampling_rate_hz     = 0.0F,
                                          .fac_cal_scale_factor = 0.0F,
                                          .fac_cal_bias         = 0.0F,
                                          .fac_cal_version      = 0U };

    bool     sensor_client   = false;
    float_t  sampling_als    = 0.0F;
    float_t  sampling_ps     = 0.0F;
    uint32_t send_massage_id = 0U;
    
    inst_state->sensor_type_info.enabled         = 0U;
    inst_state->als_info.common.heart_beat_count = 0U;
    inst_state->ps_info.common.heart_beat_count  = 0U;
    
    rpr0521rs_get_sensor_config(this, instance, RPR0521RS_ALS, &sampling_als, &sensor_client);
    if (sensor_client) {
        inst_state->sensor_type_info.enabled |= (uint8_t)RPR0521RS_ALS;
    }
    
    rpr0521rs_get_sensor_config(this, instance, RPR0521RS_PS, &sampling_ps, &sensor_client);
    if (sensor_client) {
        inst_state->sensor_type_info.enabled |= (uint8_t)RPR0521RS_PS;
    }
    
    if (sensor_type == RPR0521RS_ALS) {
        instance_config_req.sampling_rate_hz     = sampling_als;
        instance_config_req.fac_cal_scale_factor = state->als_fac_cal_scale_factor;
        instance_config_req.fac_cal_bias         = state->als_fac_cal_bias;
        instance_config_req.fac_cal_version      = state->als_fac_cal_version;
    } else if (sensor_type == RPR0521RS_PS) {
        instance_config_req.sampling_rate_hz     = sampling_ps;
        instance_config_req.fac_cal_bias         = state->ps_fac_cal_bias;
        instance_config_req.fac_cal_version      = state->ps_fac_cal_version;
    } else {
        SNS_PRINTF(ERROR, this, "sensor type error!!!");
    }
    
    if (state->is_dri == POLLING_MODE) {
        send_massage_id = (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
    } else {
        send_massage_id = (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
    }
    
    rpr0521rs_set_inst_config(this, instance, &instance_config_req, send_massage_id);
    
    return;
}
        
//==============================================================================
// NAME      : rpr0521rs_set_inst_config
// FUNCTION  : set standard streaming config for the instance.
// param[i]  : this                : Sensor reference
// param[i]  : instance            : Sensor Instance reference
// param[i]  : instance_config_req : Sensor specific registry configuration
// param[i]  : message_id          : input message ID 
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_set_inst_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_REQ *instance_config_req, uint32_t message_id)
{
    sns_request config;
    
    config.message_id  = message_id;
    config.request_len = sizeof(RPR0521RS_REQ);
    config.request     = instance_config_req;
    
    this->instance_api->set_client_config(instance, &config);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_get_sensor_config
// FUNCTION  : set standard streaming config for the instance.
// param[i]  : this                  : Sensor reference
// param[i]  : instance              : Sensor Instance reference
// param[i]  : sensor_type           : Sensor type
// param[o]  : sampling_rate         : sampling rate from framework
// param[o]  : sensor_client_present : sensor present
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_get_sensor_config(sns_sensor *this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type, float_t *sampling_rate, bool *sensor_client_present)
{
    sns_request const     *request = NULL;
    sns_std_request       decoded_request;
    sns_std_sensor_config decoded_payload = {0};
    sns_sensor_uid        suid;
    
    *sensor_client_present = false;
    
    if (sensor_type == RPR0521RS_ALS) {
        (void)sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)ALS_SUID), sizeof(sns_sensor_uid));
    } else {
        (void)sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)PS_SUID), sizeof(sns_sensor_uid));
    } 
    
    // Parse through existing requests and get fastest sample rate and report rate requests.
    request = instance->cb->get_client_request(instance, &suid, true); 
    while (NULL != request) {
        switch (request->message_id) {
            case (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG:
                if (rpr0521rs_get_decoded_request(this, request, &decoded_request, &decoded_payload)) {
                    *sampling_rate = SNS_MAX(*sampling_rate, decoded_payload.sample_rate);
                    *sensor_client_present = true;
                }
                break;

            case (uint32_t)SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG:
                *sensor_client_present = true;
                break;

            default:
                *sensor_client_present = false;
                break;
        }
        request = instance->cb->get_client_request(instance, &suid, false);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_get_decoded_request
// FUNCTION  : return decoded request message for type sns_std_sensor_config.
// param[i]  : this            : Sensor reference
// param[i]  : in_request      : Request as sotred in client_requests list
// param[o]  : decoded_request : Standard decoded message.
// param[o]  : decoded_payload : Decoded stream request payload.
// REMARKS   :
// RETURN    : bool true if decode is successful else false
//==============================================================================
static bool rpr0521rs_get_decoded_request(sns_sensor const *this, sns_request const *in_request, sns_std_request *decoded_request, sns_std_sensor_config *decoded_payload)
{
    pb_istream_t stream;
    bool         result = true;
    
    pb_simple_cb_arg arg = { .decoded_struct = decoded_payload,
                             .fields         = sns_std_sensor_config_fields };
    
    decoded_request->payload = (struct pb_callback_s) { .funcs.decode = &pb_decode_simple_cb,
                                                        .arg = &arg };
    
    stream = pb_istream_from_buffer(in_request->request, in_request->request_len);
    result = pb_decode(&stream, sns_std_request_fields, decoded_request);
    if (!result) {
        SNS_PRINTF(ERROR, this, "RPR0521RS decode error");
    }
    
    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_turn_rails_off
// FUNCTION  : turns power rail off
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_turn_rails_off(sns_sensor *this)
{
    sns_sensor *sensor = NULL;
    sns_time   timeout = 0U;
    
    sensor = this->cb->get_library_sensor(this, true);
    while (NULL != sensor) {
        RPR0521RS_STATE *sensor_state = (RPR0521RS_STATE*)sensor->state->state;
        if (sensor_state->common.rail_config.rail_vote != SNS_RAIL_OFF) {
            timeout = sns_convert_ns_to_ticks(RPR0521RS_POWER_RAIL_OFF_TIMEOUT_NS);
            rpr0521rs_start_power_rail_timer(sensor, timeout, RPR0521RS_POWER_RAIL_PENDING_OFF);
        }
        sensor = this->cb->get_library_sensor(this, false);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_start_power_rail_timer
// FUNCTION  : Create timer_stream and start power rail timer
// param[i]  : this                : Sensor reference
// param[i]  : timeout_ticks       : timeout value
// param[i]  : pwr_rail_pend_state : power rail pending state
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_start_power_rail_timer(sns_sensor *const this, sns_time timeout_ticks, RPR0521RS_POWER_RAIL_PENDING_STATE pwr_rail_pend_state)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;

    sns_timer_sensor_config req_payload= sns_timer_sensor_config_init_default;
    sns_service_manager     *smgr       = NULL;
    sns_stream_service      *stream_svc = NULL;
    sns_sensor_uid          suid;
    sns_request             timer_req;
    uint8_t                 buffer[20];
    size_t                  req_len = 0U;
    
    req_payload.is_periodic    = false;
    req_payload.start_time     = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;
    
    if (NULL == state->timer_stream) {
        smgr       = this->cb->get_service_manager(this);
        stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
        (void)sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", &suid);
        (void)stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->timer_stream);
    }

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload, sns_timer_sensor_config_fields, NULL);
    if ((req_len > 0U) && (NULL != state->timer_stream)) {
        timer_req.message_id  = (uint32_t)SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG;
        timer_req.request     = buffer;
        timer_req.request_len = req_len;
        
        (void)state->timer_stream->api->send_request(state->timer_stream, &timer_req);
        state->power_rail_pend_state = pwr_rail_pend_state;
    } else {
        SNS_PRINTF(ERROR, this, "RPR0521RS timer req encode error");
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_sensor_notify_event
// FUNCTION  : common sensor API
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS
//==============================================================================
static sns_rc rpr0521rs_sensor_notify_event(sns_sensor *const this)
{
    RPR0521RS_STATE  *state = (RPR0521RS_STATE*)this->state->state;
    
    sns_rc           rv     = SNS_RC_SUCCESS;
    sns_sensor_event *event = NULL;

    if (!sns_suid_lookup_complete(&state->common.suid_lookup_data)) {
        state->island_service->api->sensor_island_exit(state->island_service, this);
        sns_suid_lookup_handle(this, &state->common.suid_lookup_data);

        if (sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", NULL)) {
            rpr0521rs_request_registry(this);
        }

        if (sns_suid_lookup_complete(&state->common.suid_lookup_data)) {
            sns_suid_lookup_deinit(this, &state->common.suid_lookup_data);
        }
    }

    //----------------------Handle a Timer Sensor event.-------------------//
    if (NULL != state->timer_stream) {
        event = state->timer_stream->api->peek_input(state->timer_stream);
        while (NULL != event) {
            pb_istream_t           stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
            sns_timer_sensor_event timer_event;

            if ((uint32_t)SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id) {
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event)) {
                    if (state->power_rail_pend_state == RPR0521RS_POWER_RAIL_PENDING_INIT) {
                        // Initial HW discovery is OK to run in normal mode.
                        state->island_service->api->sensor_island_exit(state->island_service, this);

                        state->common.hw_is_present = rpr0521rs_discover_hw(this);

                        if (state->common.hw_is_present) {
                            rpr0521rs_update_sibling_sensors(this);
                        } else {
                            rv = SNS_RC_INVALID_LIBRARY_STATE;
                            SNS_PRINTF(ERROR, this, "RPR0521 HW absent");
                        }
                        state->power_rail_pend_state = RPR0521RS_POWER_RAIL_PENDING_NONE;
                    } else if (state->power_rail_pend_state == RPR0521RS_POWER_RAIL_PENDING_SET_CLIENT_REQ) {
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
                        if (NULL != instance) {
                            RPR0521RS_INSTANCE_STATE *inst_state = (RPR0521RS_INSTANCE_STATE*) instance->state->state;
                            inst_state->instance_is_ready_to_configure = true;
                            rpr0521rs_reval_instance_config(this, instance, state->sensor);
                            if (inst_state->new_self_test_request) {
                                rpr0521rs_set_self_test_inst_config(this, instance, state->sensor);
                            }
                        }
                        state->power_rail_pend_state = RPR0521RS_POWER_RAIL_PENDING_NONE;
                    } else if (state->power_rail_pend_state == RPR0521RS_POWER_RAIL_PENDING_OFF) {
                        if (SNS_RAIL_OFF != state->common.rail_config.rail_vote) {
                            state->common.rail_config.rail_vote = SNS_RAIL_OFF;
                            (void)state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service, this, &state->common.rail_config, NULL);
                        }
                        state->power_rail_pend_state = RPR0521RS_POWER_RAIL_PENDING_NONE;
                    } else {
                        // no process
                    }
                } else {
                    SNS_PRINTF(ERROR, this, "pb_decode error");
                }
            }
            event = state->timer_stream->api->get_next_input(state->timer_stream);
        }
        // Free up timer stream if not needed anymore
        if (state->power_rail_pend_state == RPR0521RS_POWER_RAIL_PENDING_NONE) {
            (void)sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
        }
    }

    if (NULL != state->reg_data_stream) {
        event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
        while (NULL != event) {
            // All registry events can be handled in normal mode. //
            state->island_service->api->sensor_island_exit(state->island_service, this);
            rpr0521rs_sensor_process_registry_event(this, event);

            event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
        }
    }

    if ((state->common.registry_pf_cfg_received) && (state->common.registry_cfg_received)
        && (state->common.registry_fac_cal_als_received) && (state->common.registry_fac_cal_init_als_received) 
        && (state->common.registry_fac_cal_ps_received)  && (state->common.registry_fac_cal_init_ps_received)
        && (state->common.registry_placement_received)) {
        // Initial HW detection sequence is OK to run in normal mode.
        state->island_service->api->sensor_island_exit(state->island_service, this);
        if (sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL)) {
            rpr0521rs_start_hw_detect_sequence(this);
        }
    }
    
    return rv;
}

//==============================================================================
// NAME      : rpr0521rs_set_self_test_inst_config
// FUNCTION  : set instance config to run a self test
// param[i]  : this        : Sensor reference
// param[i]  : instance    : Sensor Instance reference
// param[i]  : sensor_type : Sensor type
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_set_self_test_inst_config(sns_sensor *const this, sns_sensor_instance *instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_REQ registry_req = { .sensor_type          = sensor_type,
                                   .sampling_rate_hz     = 0.0F,
                                   .fac_cal_scale_factor = 0.0F,
                                   .fac_cal_bias         = 0.0F,
                                   .fac_cal_version      = 0U };
    
    sns_request config;

    config.message_id  = (uint32_t)SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    config.request_len = 0U;
    config.request     = &registry_req;

    this->instance_api->set_client_config(instance, &config);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_get_decoded_self_test_request
// FUNCTION  : decode self test requests
// param[i]  : this            : Sensor reference
// param[i]  : request         : Encoded input request
// param[o]  : decoded_request : Decoded standard request
// param[o]  : test_config     : decoded self test request
// REMARKS   :
// RETURN    : bool True if decoding is successfull else false.
//==============================================================================
static bool rpr0521rs_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request, sns_std_request *decoded_request, sns_physical_sensor_test_config *test_config)
{
    bool         result = true;
    pb_istream_t stream;
    
    pb_simple_cb_arg arg = { .decoded_struct = test_config,
                             .fields         = sns_physical_sensor_test_config_fields };
    
    decoded_request->payload = (struct pb_callback_s){ .funcs.decode = &pb_decode_simple_cb, 
                                                       .arg          = &arg };

    stream = pb_istream_from_buffer(request->request, request->request_len);
    result = pb_decode(&stream, sns_std_request_fields, decoded_request);
    if (!result) {
        SNS_PRINTF(ERROR, this, "RPR0521RS decode error");
    }

    return (result);
}

//==============================================================================
// NAME      : rpr0521rs_extract_self_test_info
// FUNCTION  : decode a physical sensor self test request and updates
//             instance state with request info.
// param[i]  : this        : Sensor reference
// param[i]  : instance    : Sensor Instance reference
// param[i]  : new_request : Encoded request
// REMARKS   :
// RETURN    : True if request is valid else false
//==============================================================================
static bool rpr0521rs_extract_self_test_info(sns_sensor *this, sns_sensor_instance *instance, struct sns_request const *new_request)
{
    RPR0521RS_STATE                 *state      = (RPR0521RS_STATE*)this->state->state;
    RPR0521RS_INSTANCE_STATE        *inst_state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    RPR0521RS_SELF_TEST_INFO        *self_test_info = NULL;
    sns_std_request                 decoded_request;
    sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
    bool                            result      = true;
    
    if (state->sensor == RPR0521RS_ALS) {
        self_test_info = &inst_state->als_info.common.test_info;
    } else if (state->sensor == RPR0521RS_PS) {
        self_test_info = &inst_state->ps_info.common.test_info;
    } else {
        result = false;
    }
    
    if (self_test_info != NULL) {
        result = rpr0521rs_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config);
        if (result) {
            self_test_info->test_type           = test_config.test_type;
            self_test_info->test_client_present = true;
        }
    }
    
    return (result);
} 

//==============================================================================
// NAME      : rpr0521rs_send_fifo_flush_done
// FUNCTION  : send flush event
// param[i]  : instance : instance reference
// param[i]  : suid     : sensor uid
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_send_fifo_flush_done(sns_sensor_instance *const instance, sns_sensor_uid *suid)
{
    sns_service_manager *mgr = instance->cb->get_service_manager(instance);
    sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);
    
    if (NULL != event) {
        event->message_id = (uint32_t)SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
        event->event_len  = 0U;
        event->timestamp  = sns_get_system_time();

        e_service->api->publish_event(e_service, instance, event, suid);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_reset_cal_data
// FUNCTION  : reset factory calibration parameter
// param[i]  : this        : Sensor reference
// param[i]  : instance    : instance reference
// param[i]  : sensor_type : sensor type
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_reset_cal_data(sns_sensor *const this, sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_STATE          *sensor_state = (RPR0521RS_STATE*)this->state->state;
    RPR0521RS_INSTANCE_STATE *inst_state   = (RPR0521RS_INSTANCE_STATE *)instance->state->state;
    
    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    float_t bias_data  = 0.0F;
    float_t scale_data = 0.0F;
    
    if (sensor_type == RPR0521RS_ALS) {
        sensor_info = &inst_state->als_info.common;
        sensor_info->registry_cfg.fac_cal_version = sensor_state->als_fac_cal_version;
        bias_data   = sensor_state->als_fac_cal_init_bias;
        scale_data  = sensor_state->als_fac_cal_init_scale_factor;
    } else {
        sensor_info = &inst_state->ps_info.common;
        sensor_info->registry_cfg.fac_cal_version = sensor_state->ps_fac_cal_version;
        bias_data   = sensor_state->ps_fac_cal_init_bias;
    }
    
    sensor_info->registry_cfg.fac_cal_bias         = bias_data;
    sensor_info->registry_cfg.fac_cal_scale_factor = scale_data;
    sensor_info->registry_cfg.fac_cal_version++;
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_update_sensor_state
// FUNCTION  : if sensor instance version is higher than sensor's one,
//             sensor's factory calibration parameter is updated.
// param[i]  : this     : Sensor reference
// param[i]  : instance : Sensor instance reference
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_update_sensor_state(sns_sensor *const this, sns_sensor_instance *const instance)
{
    RPR0521RS_STATE          *sensor_state = NULL;
    RPR0521RS_INSTANCE_STATE *inst_state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;

    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    sns_sensor *sensor = NULL;

    for (sensor = this->cb->get_library_sensor(this, true); sensor != NULL; sensor = this->cb->get_library_sensor(this, false)) {
        sensor_state = (RPR0521RS_STATE*)sensor->state->state;
        
        if (sensor_state->sensor == RPR0521RS_ALS) {
            sensor_info = &inst_state->als_info.common;
            if (sensor_state->als_fac_cal_version < sensor_info->registry_cfg.fac_cal_version) {
                sensor_state->als_fac_cal_scale_factor = sensor_info->registry_cfg.fac_cal_scale_factor;
                sensor_state->als_fac_cal_bias         = sensor_info->registry_cfg.fac_cal_bias;
                sensor_state->als_fac_cal_version      = sensor_info->registry_cfg.fac_cal_version;
            }
        } else {
            sensor_info = &inst_state->ps_info.common;
            if (sensor_state->ps_fac_cal_version < sensor_info->registry_cfg.fac_cal_version) {
                sensor_state->ps_fac_cal_bias          = sensor_info->registry_cfg.fac_cal_bias;
                sensor_state->ps_fac_cal_version       = sensor_info->registry_cfg.fac_cal_version;
            }
        }
    }
    
    return;
}
