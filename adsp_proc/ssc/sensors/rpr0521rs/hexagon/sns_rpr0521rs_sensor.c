//==============================================================================
// MODULE        : sns_rpr0521rs_sensor.c
// FUNCTION      : Common function for RPR0521RS Sensors.
// AUTHOR        : Masafumi Seike
// MAKING DATE   : NOV/13/2017
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
#include "sns_sensor.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_printf.h"
#include "sns_pb_util.h"
#include "sns_registry.pb.h"
#include "sns_sensor_util.h"
#include "sns_diag_service.h"
#include "sns_attribute_util.h"
#include "pb_encode.h"
#include "pb.h"
#include "sns_pb_util.h"

//==============================================================================
// struction define and global value
//==============================================================================
static const RPR0521RS_RANGE_ATTR rpr0521rs_als_ranges[] =
{
    {RPR0521RS_LUX_RANGE_X1_MIN,   RPR0521RS_LUX_RANGE_X1_MAX},
    {RPR0521RS_LUX_RANGE_X2_MIN,   RPR0521RS_LUX_RANGE_X2_MAX},
    {RPR0521RS_LUX_RANGE_X64_MIN,  RPR0521RS_LUX_RANGE_X64_MAX},
    {RPR0521RS_LUX_RANGE_X128_MIN, RPR0521RS_LUX_RANGE_X128_MAX}
};

static const float_t rpr0521rs_als_resolutions[] =
{
    RPR0521RS_ALS_RESOLUTION_X1,
    RPR0521RS_ALS_RESOLUTION_X2,
    RPR0521RS_ALS_RESOLUTION_X64,
    RPR0521RS_ALS_RESOLUTION_X128
};

//==============================================================================
// function prototype
//==============================================================================
// internal use
static void rpr0521rs_sensor_send_registry_request(sns_sensor *const this, char *reg_group_name);
static void rpr0521rs_publish_registry_attributes(sns_sensor *const this);
static void rpr0521rs_publish_available(sns_sensor *const this);
static sns_rc rpr0521rs_register_power_rail(sns_sensor *const this);

//==============================================================================
// NAME      : rpr0521rs_common_init
// FUNCTION  : initialize common state
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_common_init(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    struct sns_service_manager *smgr = this->cb->get_service_manager(this);

    state->scp_service           = (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
    state->island_service        = (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
    state->sensor_client_present = false;

    state->common.who_am_i = 0U;
    SNS_SUID_LOOKUP_INIT(state->common.suid_lookup_data, 0);

    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "interrupt");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "timer");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "registry");
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_request_registry
// FUNCTION  : create reg_data_stream and collate config for Framework
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_request_registry(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;

    sns_sensor_uid      suid;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service  *stream_svc  = (sns_stream_service*) service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    
    // place a request to registry sensor
    if ((state->reg_data_stream == NULL) && (state->common.who_am_i == 0U)) {
        sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
        stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->reg_data_stream);

        if (state->sensor == RPR0521RS_ALS) {
            rpr0521rs_sensor_send_registry_request(this, RPR0521RS_CONFIG_ALS);
        } else if (state->sensor == RPR0521RS_PS) {
            rpr0521rs_sensor_send_registry_request(this, RPR0521RS_CONFIG_PS);
        } else {
            SNS_PRINTF(ERROR, this, "No such a sensor !!!");
        }
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_CONFIG);
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_PLACEMENT);
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_FAC_CAL_ALS);
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_FAC_CAL_INIT_ALS);
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_FAC_CAL_PS);
        rpr0521rs_sensor_send_registry_request(this, RPR0521RS_PLATFORM_FAC_CAL_INIT_PS);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_sensor_send_registry_request
// FUNCTION  : send request to collate config for Framework
// param[i]  : this           : Sensor reference
// param[i]  : reg_group_name : config name
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_sensor_send_registry_request(sns_sensor *const this, char *reg_group_name)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    uint8_t  buffer[100];
    uint32_t encoded_len = 0U;
    
    sns_memset(buffer, 0, sizeof(buffer));
    
    sns_registry_read_req read_request;
    pb_buffer_arg data = (pb_buffer_arg){ .buf     = reg_group_name,
                                          .buf_len = (strlen(reg_group_name) + 1U) };

    read_request.name.arg          = &data;
    read_request.name.funcs.encode = pb_encode_string_cb;

    encoded_len = pb_encode_request(buffer, sizeof(buffer), &read_request, sns_registry_read_req_fields, NULL);
    if (0U < encoded_len) {
        sns_request request = (sns_request){ .request_len = encoded_len,
                                             .request     = buffer,
                                             .message_id  = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
        state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_sensor_process_registry_event
// FUNCTION  : extract register config from json file
// param[i]  : this : Sensor reference
// param[i]  : event
// REMARKS   : 
// RETURN    : none
//==============================================================================
void rpr0521rs_sensor_process_registry_event(sns_sensor *const this, sns_sensor_event *event)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    bool     rv = true;
    uint32_t fac_cal_version = 0U;
    
    pb_istream_t            stream     = pb_istream_from_buffer((void*)event->event, event->event_len);
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    
    if ((uint32_t)SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id) {
        pb_buffer_arg group_name = {0,0};
        read_event.name.arg = &group_name;
        read_event.name.funcs.decode = pb_decode_string_cb;

        rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        if (!rv) {
            SNS_PRINTF(ERROR, this, "Error decoding registry event");
        } else {
            stream = pb_istream_from_buffer((void*)event->event, event->event_len);
            
            if ((0 == strncmp((char*)group_name.buf, RPR0521RS_CONFIG_ALS, group_name.buf_len)) 
                || (0 == strncmp((char*)group_name.buf, RPR0521RS_CONFIG_PS, group_name.buf_len))) {
                {
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 1,
                                                    .parse_info[0]   = { .group_name    = "config",
                                                                         .parse_func    = sns_registry_parse_phy_sensor_cfg,
                                                                         .parsed_buffer = &state->common.registry_cfg } };
                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv) {
                    state->common.registry_cfg_received = true;
                    state->is_dri               = state->common.registry_cfg.is_dri;
                    state->hardware_id          = state->common.registry_cfg.hw_id;
                    state->resolution_idx       = state->common.registry_cfg.res_idx;
                    state->supports_sync_stream = state->common.registry_cfg.sync_stream;

                    SNS_PRINTF(LOW, this, "Registry read event for group registry_cfg received "
                                          "is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
                                          state->is_dri, state->hardware_id, state->resolution_idx,
                                          state->supports_sync_stream);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_CONFIG, group_name.buf_len)) {
                {
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 1,
                                                    .parse_info[0]   = { .group_name    = "config",
                                                                         .parse_func    = sns_registry_parse_phy_sensor_pf_cfg,
                                                                         .parsed_buffer = &state->common.registry_pf_cfg } };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv) {
                    state->common.registry_pf_cfg_received = true;

                    state->common.com_port_info.com_config.bus_type          = state->common.registry_pf_cfg.bus_type;
                    state->common.com_port_info.com_config.bus_instance      = state->common.registry_pf_cfg.bus_instance;
                    state->common.com_port_info.com_config.slave_control     = state->common.registry_pf_cfg.slave_config;
                    state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
                    state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
                    state->common.com_port_info.com_config.reg_addr_type     = state->common.registry_pf_cfg.reg_addr_type;
                    state->common.irq_config.interrupt_num                   = state->common.registry_pf_cfg.dri_irq_num;
                    state->common.irq_config.interrupt_pull_type             = state->common.registry_pf_cfg.irq_pull_type;
                    state->common.irq_config.is_chip_pin                     = state->common.registry_pf_cfg.irq_is_chip_pin;
                    state->common.irq_config.interrupt_drive_strength        = state->common.registry_pf_cfg.irq_drive_strength;
                    state->common.irq_config.interrupt_trigger_type          = state->common.registry_pf_cfg.irq_trigger_type;
                    state->common.rail_config.num_of_rails                   = state->common.registry_pf_cfg.num_rail;
                    state->common.registry_rail_on_state                     = state->common.registry_pf_cfg.rail_on_state;
                    (void)sns_strlcpy(state->common.rail_config.rails[0].name, state->common.registry_pf_cfg.vddio_rail, sizeof(state->common.rail_config.rails[0].name));
                    (void)sns_strlcpy(state->common.rail_config.rails[1].name, state->common.registry_pf_cfg.vdd_rail, sizeof(state->common.rail_config.rails[1].name));

                    SNS_PRINTF(LOW, this, "Registry read event for group registry_pf_cfg received "
                                          "bus_type:%d bus_instance:%d slave_control:%d "
                                          "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d ",
                                          state->common.com_port_info.com_config.bus_type,
                                          state->common.com_port_info.com_config.bus_instance,
                                          state->common.com_port_info.com_config.slave_control,
                                          state->common.com_port_info.com_config.min_bus_speed_KHz,
                                          state->common.com_port_info.com_config.max_bus_speed_KHz,
                                          state->common.com_port_info.com_config.reg_addr_type);

                    SNS_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d "
                                          "interrupt_drive_strength:%d interrupt_trigger_type:%d rigid body type:%d",
                                          state->common.irq_config.interrupt_num,
                                          state->common.irq_config.interrupt_pull_type,
                                          state->common.irq_config.is_chip_pin,
                                          state->common.irq_config.interrupt_drive_strength,
                                          state->common.irq_config.interrupt_trigger_type,
                                          state->common.registry_pf_cfg.rigid_body_type);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_PLACEMENT,group_name.buf_len)) {
                {
                    uint8_t arr_index = 0U;
                    pb_float_arr_arg arr_arg = { .arr       = state->common.placement,
                                                 .arr_index = &arr_index,
                                                 .arr_len   = 12 };

                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 1,
                                                    .parse_info[0]   = { .group_name    = "placement",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &arr_arg } };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv) {
                    state->common.registry_placement_received = true;
                    SNS_PRINTF(LOW, this, "Registry read event for group registry_placement received "
                                          "p[0]:%u p[6]:%u p[11]:%u", (uint32_t)state->common.placement[0],
                                          (uint32_t)state->common.placement[6], (uint32_t)state->common.placement[11]);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_FAC_CAL_ALS, group_name.buf_len)) {
                {
                    uint8_t coeff_arr_index = 0U;
                    uint8_t scale_arr_index = 0U;
                    uint8_t bias_arr_index  = 0U;
                    
                    pb_float_arr_arg coeff_arr_arg = { .arr       = (float_t*)&state->tab_coef,
                                                       .arr_index = &coeff_arr_index,
                                                       .arr_len   = sizeof(RPR0521RS_COEFF_UNIT) };
                    
                    pb_float_arr_arg scale_arr_arg = { .arr       = (float_t*)&state->als_fac_cal_scale_factor,
                                                       .arr_index = &scale_arr_index,
                                                       .arr_len   = sizeof(state->als_fac_cal_scale_factor) };
                    
                    pb_float_arr_arg bias_arr_arg  = { .arr       = (float_t*)&state->als_fac_cal_bias,
                                                       .arr_index = &bias_arr_index,
                                                       .arr_len   = sizeof(state->als_fac_cal_bias) };
                    
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 3,
                                                    .parse_info[0]   = { .group_name    = "coefficient",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &coeff_arr_arg },
                                                    .parse_info[1]   = { .group_name    = "scale",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &scale_arr_arg },
                                                    .parse_info[2]   = { .group_name    = "bias",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &bias_arr_arg  }};
                    
                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                    fac_cal_version = arg.version;
                }
                
                if (rv) {
                    state->common.registry_fac_cal_als_received = true;
                    state->als_fac_cal_version = fac_cal_version;
                    SNS_PRINTF(LOW, this, "scale = %d, bias = %d, als_fac_cal_version = %u", (unsigned int)(state->als_fac_cal_scale_factor * 1000), (unsigned int)state->als_fac_cal_bias, (uint32_t)state->als_fac_cal_version);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_FAC_CAL_INIT_ALS, group_name.buf_len)) {
                {
                    uint8_t init_scale_arr_index = 0U;
                    uint8_t init_bias_arr_index  = 0U;
                    
                    pb_float_arr_arg init_scale_arr_arg = { .arr       = (float_t*)&state->als_fac_cal_init_scale_factor,
                                                            .arr_index = &init_scale_arr_index,
                                                            .arr_len   = sizeof(state->als_fac_cal_init_scale_factor) };
                    
                    pb_float_arr_arg init_bias_arr_arg  = { .arr       = (float_t*)&state->als_fac_cal_init_bias,
                                                            .arr_index = &init_bias_arr_index,
                                                            .arr_len   = sizeof(state->als_fac_cal_init_bias) };
                    
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 2,
                                                    .parse_info[0]   = { .group_name    = "init_scale",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &init_scale_arr_arg },
                                                    .parse_info[1]   = { .group_name    = "init_bias",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &init_bias_arr_arg  }};
                    
                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }
                
                if (rv) {
                    state->common.registry_fac_cal_init_als_received = true;
                    SNS_PRINTF(LOW, this, "init_scale = %d, init_bias = %d", (unsigned int)(state->als_fac_cal_init_scale_factor * 1000), (unsigned int)state->als_fac_cal_init_bias);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_FAC_CAL_PS, group_name.buf_len)) {
                {
                    uint8_t  near_arr_index       = 0U;
                    uint8_t  far_arr_index        = 0U;
                    uint8_t  bias_arr_index       = 0U;
                    
                    pb_float_arr_arg near_arr_arg = { .arr       = (float_t*)&state->near_threshold,
                                                      .arr_index = &near_arr_index,
                                                      .arr_len   = sizeof(state->near_threshold) };

                    pb_float_arr_arg far_arr_arg  = { .arr       = (float_t*)&state->far_threshold,
                                                      .arr_index = &far_arr_index,
                                                      .arr_len   = sizeof(state->far_threshold) };

                    pb_float_arr_arg bias_arr_arg = { .arr       = (float_t*)&state->ps_fac_cal_bias,
                                                      .arr_index = &bias_arr_index,
                                                      .arr_len   = sizeof(state->ps_fac_cal_bias) };
                    
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 3,
                                                    .parse_info[0]   = { .group_name    = "near_threshold",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &near_arr_arg },
                                                    .parse_info[1]   = { .group_name    = "far_threshold",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &far_arr_arg  }, 
                                                    .parse_info[2]   = { .group_name    = "bias",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &bias_arr_arg }};

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                    fac_cal_version = arg.version;
                }
                
                if (rv) {
                    state->common.registry_fac_cal_ps_received = true;
                    state->ps_fac_cal_version = fac_cal_version;
                    SNS_PRINTF(LOW, this, "near_threshold = %u, far_threshold = %u, bias = %d, ps_fac_cal_version = %u", (uint32_t)state->near_threshold, (uint32_t)state->far_threshold, (unsigned int)state->ps_fac_cal_bias, (uint32_t)state->ps_fac_cal_version);
                }
            } else if (0 == strncmp((char*)group_name.buf, RPR0521RS_PLATFORM_FAC_CAL_INIT_PS, group_name.buf_len)) {
                {
                    uint8_t init_bias_arr_index = 0U;
                    
                    pb_float_arr_arg init_bias_arr_arg  = { .arr       = (float_t*)&state->ps_fac_cal_init_bias,
                                                            .arr_index = &init_bias_arr_index,
                                                            .arr_len   = sizeof(state->ps_fac_cal_init_bias) };
                    
                    sns_registry_decode_arg arg = { .item_group_name = &group_name,
                                                    .parse_info_len  = 1,
                                                    .parse_info[0]   = { .group_name    = "init_bias",
                                                                         .parse_func    = sns_registry_parse_float_arr,
                                                                         .parsed_buffer = &init_bias_arr_arg }};

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg          = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }
                
                if (rv) {
                    state->common.registry_fac_cal_init_ps_received = true;
                    SNS_PRINTF(LOW, this, "init_bias = %d", (unsigned int)state->ps_fac_cal_init_bias);
                }
            } else {
                rv = false;
            }

            if (!rv) {
                SNS_PRINTF(ERROR, this, "Error decoding registry group");
            }
        }
    } else {
        SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u", event->message_id);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_discover_hw
// FUNCTION  : discover hw by reading id register
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : true(hw discovered) or false(hw not discovered)
//==============================================================================
bool rpr0521rs_discover_hw(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    uint8_t who_am_i      = 0U;
    bool    hw_is_present = false;
    sns_rc  rv            = SNS_RC_SUCCESS;

    //-------------------Read and Confirm WHO-AM-I------------------------
    rv = rpr0521rs_check_who_am_i(state->scp_service, state->common.com_port_info.port_handle, &who_am_i);
    if (rv == SNS_RC_SUCCESS) {
        state->common.who_am_i = who_am_i;
        hw_is_present          = true;
        SNS_PRINTF(LOW, this, "who am i is 0x%02X", state->common.who_am_i);
    } else {
        SNS_PRINTF(ERROR, this, "Execution Faild in rpr0521rs_check_who_am_i!");
    }
    //------------------Power Down and Close COM Port--------------------
    state->scp_service->api->sns_scp_update_bus_power(state->common.com_port_info.port_handle, false);
    state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

    //----------------------Turn Power Rail OFF--------------------------
    state->common.rail_config.rail_vote = SNS_RAIL_OFF;
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service, this, &state->common.rail_config, 0);

    return (hw_is_present);
}

//==============================================================================
// NAME      : rpr0521rs_publish_available
// FUNCTION  : publish available data
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
static void rpr0521rs_publish_available(sns_sensor *const this)
{
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean     = true;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1U, true);
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_register_power_rail
// FUNCTION  : register power rail function
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : SNS_RC_SUCCESS or SNS_RC_FAILED
//==============================================================================
static sns_rc rpr0521rs_register_power_rail(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_rc              rv    = SNS_RC_SUCCESS;

    state->common.rail_config.rail_vote = SNS_RAIL_OFF;

    if (NULL == state->pwr_rail_service) {
        state->pwr_rail_service = (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);
        state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service, &state->common.rail_config);
    }

    if (NULL == state->pwr_rail_service) {
        rv = SNS_RC_FAILED;
    }
    return rv;
}

//==============================================================================
// NAME      : rpr0521rs_publish_registry_attributes
// FUNCTION  : publish attributes read from registry
// param[i]  : this : Sensor reference
// REMARKS   : 
// RETURN    : none
//==============================================================================
static void rpr0521rs_publish_registry_attributes(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    uint8_t i = 0U;
    
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = state->is_dri;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean     = state->supports_sync_stream;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1U, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = state->hardware_id;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1U, false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
            SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
        for (i=0U; i<12U; i++) {
            values[i].has_flt = true;
            values[i].flt     = state->common.placement[i];
        }
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT, values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint     = (int64_t)state->common.registry_pf_cfg.rigid_body_type;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1U, false);
    }
    if (state->sensor == RPR0521RS_ALS) {
        {
            if (state->is_dri == DRI_MODE) {
                sns_std_attr_value_data values[] = {SNS_ATTR};
                values[0].has_sint = true;
                values[0].sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
                sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, values, ARR_SIZE(values), false);
            }
        }
        {
            sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
            value.has_flt = true;
            value.flt     = rpr0521rs_als_resolutions[state->resolution_idx];
            sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1U, false);
        }
        {
            sns_std_attr_value_data values[]      = {SNS_ATTR};
            sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
            rangeMinMax[0].has_flt = true;
            rangeMinMax[0].flt     = rpr0521rs_als_ranges[state->resolution_idx].min;
            rangeMinMax[1].has_flt = true;
            rangeMinMax[1].flt     = rpr0521rs_als_ranges[state->resolution_idx].max;
            values[0].has_subtype = true;
            values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
            values[0].subtype.values.arg          = &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
            sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
        }
    } else if (state->sensor == RPR0521RS_PS) {
        {
            if (state->is_dri == POLLING_MODE) {
                sns_std_attr_value_data values[] = {SNS_ATTR};
                values[0].has_sint = true;
                values[0].sint     = SNS_STD_SENSOR_STREAM_TYPE_STREAMING;
                sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, values, ARR_SIZE(values), false);
            }
        }
        {
            sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
            value.has_flt = true;
            value.flt     = 1.0F;
            sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1U, false);
        }
        {
            sns_std_attr_value_data values[] = {SNS_ATTR};
            sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
            rangeMinMax[0].has_flt = true;
            rangeMinMax[0].flt     = 0.0F; // cm
            rangeMinMax[1].has_flt = true;
            rangeMinMax[1].flt     = 5.0F; // cm
            values[0].has_subtype = true;
            values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
            values[0].subtype.values.arg          = &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
            sns_publish_attribute( this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
        }
    } else {
        // no process
    }

    return;
}

//==============================================================================
// NAME      : rpr0521rs_update_sibling_sensors
// FUNCTION  : update sibling sensors
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : none
//==============================================================================
void rpr0521rs_update_sibling_sensors(sns_sensor *const this)
{
    RPR0521RS_STATE *state = NULL;
    
    sns_sensor *sensor = NULL;
    
    sensor = this->cb->get_library_sensor(this, true);
    while (sensor != NULL) {
        state = (RPR0521RS_STATE*)sensor->state->state;
        (void)rpr0521rs_register_power_rail(sensor);
        rpr0521rs_publish_available(sensor);
        rpr0521rs_publish_registry_attributes(sensor);
        (void)sns_sensor_util_remove_sensor_stream(sensor, &state->reg_data_stream);
        sensor = this->cb->get_library_sensor(this, false);
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_start_hw_detect_sequence
// FUNCTION  : start hardware detect sequence
// param[i]  : this : Sensor reference
// REMARKS   :
// RETURN    : 
//==============================================================================
void rpr0521rs_start_hw_detect_sequence(sns_sensor *const this)
{
    RPR0521RS_STATE *state = (RPR0521RS_STATE*)this->state->state;
    
    sns_rc rv = SNS_RC_SUCCESS;
    
    state->common.registry_pf_cfg_received = false;
    
    //-----------------Register and Open COM Port-------------------------
    if (NULL == state->common.com_port_info.port_handle) {
        rv = state->scp_service->api->sns_scp_register_com_port(&state->common.com_port_info.com_config, &state->common.com_port_info.port_handle );

        if (rv == SNS_RC_SUCCESS) {
            rv = state->scp_service->api->sns_scp_open(state->common.com_port_info.port_handle);
        }
    }

    //---------------------Register Power Rails --------------------------
    if ((sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL)) && (NULL == state->pwr_rail_service) && (rv == SNS_RC_SUCCESS)) {
        rv = rpr0521rs_register_power_rail(this);

        //---------------------Turn Power Rails ON----------------------------
        state->common.rail_config.rail_vote = state->common.registry_rail_on_state;

        if (rv == SNS_RC_SUCCESS) {
            rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service, this, &state->common.rail_config, 0);
        }

        //-------------Create a Timer stream for Power Rail ON timeout.---------
        if (rv == SNS_RC_SUCCESS) {
            rpr0521rs_start_power_rail_timer(this, sns_convert_ns_to_ticks(RPR0521RS_OFF_TO_IDLE_NS), RPR0521RS_POWER_RAIL_PENDING_INIT);
        }
    }
    
    return;
}

//==============================================================================
// NAME      : rpr0521rs_update_registry
// FUNCTION  : update registry
// param[i]  : this        : Sensor reference
// param[i]  : instance    : Sensor instance reference
// param[i]  : sensor_type : Sensor type
// REMARKS   : 
// RETURN    : none
//==============================================================================
void rpr0521rs_update_registry(sns_sensor *const this, sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type)
{
    RPR0521RS_STATE          *state      = (RPR0521RS_STATE*)this->state->state;
    RPR0521RS_INSTANCE_STATE *inst_state = (RPR0521RS_INSTANCE_STATE*)instance->state->state;
    
    uint8_t  buffer[1000];
    uint32_t encoded_len = 0U;
    char     als_name[] = RPR0521RS_PLATFORM_FAC_CAL_ALS;
    char     ps_name[]  = RPR0521RS_PLATFORM_FAC_CAL_PS;
    
    pb_arg_reg_group_arg arg = { .instance = instance,
                                 .name     = 0,
                                 .sensor   = 0,
                                 .version  = 0U };

    pb_buffer_arg          name_data;
    sns_registry_write_req write_req = sns_registry_write_req_init_default;

    if (sensor_type == RPR0521RS_ALS)  {
        name_data   = (pb_buffer_arg){ .buf = als_name, .buf_len = strlen(als_name) + 1U };
        arg.sensor  = RPR0521RS_ALS;
        arg.version = inst_state->als_info.common.registry_cfg.fac_cal_version;
    } else if (sensor_type == RPR0521RS_PS) {
        name_data   = (pb_buffer_arg){ .buf = ps_name, .buf_len = strlen(ps_name) + 1U };
        arg.sensor  = RPR0521RS_PS;
        arg.version = inst_state->ps_info.common.registry_cfg.fac_cal_version;
    } else {
        SNS_PRINTF(ERROR, this, "Unsupported sensor %d", sensor_type);
        return;
    }

    write_req.name.funcs.encode       = &pb_encode_string_cb;
    write_req.name.arg                = &name_data;
    write_req.data.items.funcs.encode = &rpr0521rs_encode_registry_cb;
    write_req.data.items.arg          = &arg;

    encoded_len = pb_encode_request(buffer, sizeof(buffer), &write_req, sns_registry_write_req_fields, NULL);
    if (0U < encoded_len) {
        if (NULL == state->reg_data_stream) {
            sns_service_manager *smgr = this->cb->get_service_manager(this);
            sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
            sns_sensor_uid suid;

            sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
            stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->reg_data_stream);
        }

        sns_request request = (sns_request){ .request_len = encoded_len, 
                                             .request = buffer,
                                             .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
        state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    }
}

//==============================================================================
// NAME      : rpr0521rs_encode_registry_cb
// FUNCTION  : 
// param[i]  : 
// REMARKS   : 
// RETURN    : true or false
//==============================================================================
bool rpr0521rs_encode_registry_cb(struct pb_ostream_s *stream, struct pb_field_s const *field, void *const *arg)
{
    UNUSED_VAR(field);
    
    pb_arg_reg_group_arg *reg_arg  = (pb_arg_reg_group_arg*)*arg;
    sns_sensor_instance  *instance = reg_arg->instance;
    char const *names[] = {"scale", "bias"};
    uint32_t   i = 0U;

    for (i=0; i < ARR_SIZE(names); i++) {
        pb_buffer_arg          name_data = (pb_buffer_arg){ .buf = names[i], .buf_len = strlen(names[i]) + 1U };
        sns_registry_data_item pb_item   = sns_registry_data_item_init_default;
        pb_arg_reg_group_arg   pb_arg    = (pb_arg_reg_group_arg){ .name     = 0,
                                                                   .instance = instance,
                                                                   .sensor   = reg_arg->sensor };

        pb_item.has_version       = true;
        pb_item.version           = reg_arg->version;
        pb_item.name.arg          = &name_data;
        pb_item.name.funcs.encode = &pb_encode_string_cb;

        if (0 == strncmp(names[i],"scale",name_data.buf_len))  {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &rpr0521rs_encode_registry_group_cb;
            pb_item.subgroup.items.arg          = &pb_arg;
        } else if (0 == strncmp(names[i],"bias",name_data.buf_len))  {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &rpr0521rs_encode_registry_group_cb;
            pb_item.subgroup.items.arg          = &pb_arg;
        } else {
            // non process
        }
        
        if (!pb_encode_tag_for_field(stream, field)) {
            SNS_INST_PRINTF(ERROR, instance,"Error encoding corr_mat");
            return (false);
        }
        
        if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item)) {
            SNS_INST_PRINTF(ERROR, instance,"Error encoding sns_registry_data_item_fields");
            return false;
        }
    }

    return true;
}

//==============================================================================
// NAME      : rpr0521rs_encode_registry_group_cb
// FUNCTION  : 
// param[i]  : 
// REMARKS   : 
// RETURN    : true or false
//==============================================================================
bool rpr0521rs_encode_registry_group_cb(struct pb_ostream_s *stream, struct pb_field_s const *field, void *const *arg)
{
    pb_arg_reg_group_arg*    pb_arg = (pb_arg_reg_group_arg*)*arg;
    RPR0521RS_INSTANCE_STATE *state = (RPR0521RS_INSTANCE_STATE*)pb_arg->instance->state->state;

    RPR0521RS_SENSOR_COMMON_INFO *sensor_info = NULL;
    
    uint32_t i = 0U;
    
    if (0 == strncmp(pb_arg->name,"scale",strlen("scale"))) {
        char const *names[] = {"x"};

        for (i=0U; i<ARR_SIZE(names); i++) {
            pb_buffer_arg name_data = (pb_buffer_arg){ .buf = names[i], .buf_len = strlen(names[i]) + 1U };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg          = &name_data;
            pb_item.has_flt           = true;
            pb_item.has_version       = true;

            if (pb_arg->sensor == RPR0521RS_ALS) {
                sensor_info = &state->als_info.common;
            } else  {
                sensor_info = &state->ps_info.common;
            }
            
            pb_item.flt     = sensor_info->registry_cfg.fac_cal_scale_factor;
            pb_item.version = sensor_info->registry_cfg.fac_cal_version;

            if (!pb_encode_tag_for_field(stream, field)) {
                return false;
            }

            if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item)) {
                SNS_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    } else if (0 == strncmp(pb_arg->name,"bias",strlen("bias"))) {
        char const *names[] = {"x"};

        for (i=0U; i<ARR_SIZE(names); i++) {
            pb_buffer_arg name_data = (pb_buffer_arg){ .buf = names[i], .buf_len = strlen(names[i]) + 1U };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg          = &name_data;
            pb_item.has_flt           = true;
            pb_item.has_version       = true;

            if (pb_arg->sensor == RPR0521RS_ALS) {
                sensor_info = &state->als_info.common;
            } else  {
                sensor_info = &state->ps_info.common;
            }
            
            pb_item.flt     = sensor_info->registry_cfg.fac_cal_bias;
            pb_item.version = sensor_info->registry_cfg.fac_cal_version;

            if (!pb_encode_tag_for_field(stream, field)) {
                return false;
            }

            if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item)) {
                SNS_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    } else {
        // non process
    }
    
    return true;
}

