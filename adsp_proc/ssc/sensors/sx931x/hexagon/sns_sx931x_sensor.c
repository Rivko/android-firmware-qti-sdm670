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
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_service.h"
#include "sns_sensor_util.h"
#include "sns_types.h"
#include "sns_attribute_util.h"

#include "sns_sx931x_sensor.h"
#include "sns_sx931x_sensor_instance.h"



#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"


#define CONFIG_SAR              ".sar.config"
#define PLATFORM_CONFIG         "_platform.config"
#define PLATFORM_PLACEMENT      "_platform.placement"

#define SX93XX_GEN_GROUP(x,group) NAME "_"#x group

#define MAX_DEP_LENGTH 30

#if DUAL_SENSOR_SUPPORT
#define SENSOR_CNT 2
#else
#define SENSOR_CNT 1
#endif

enum {
  REG_CONFIG_SAR,
  REG_PLATFORM_CONFIG,
  REG_PLATFORM_PLACEMENT,
  REG_MAX_CONFIGS,
};

static char sx93xx_reg_config[SENSOR_CNT][REG_MAX_CONFIGS][SNS_REGISTRY_MAX_NAME_LEN] = {
  {
    SX93XX_GEN_GROUP(0, CONFIG_SAR),
    SX93XX_GEN_GROUP(0, PLATFORM_CONFIG),
    SX93XX_GEN_GROUP(0, PLATFORM_PLACEMENT),
  },
#if DUAL_SENSOR_SUPPORT
  {
    SX93XX_GEN_GROUP(1, CONFIG_SAR),
    SX93XX_GEN_GROUP(1, PLATFORM_CONFIG),
    SX93XX_GEN_GROUP(1, PLATFORM_PLACEMENT),
  }
#endif
};


static void sx931x_set_sensor_inst_config(sns_sensor *this,
        sns_sensor_instance *instance,
        float chosen_report_rate,
        float chosen_sample_rate,
        sx931x_sensor_type  sensor_type)
{
    UNUSED_VAR(instance);
    sns_sx931x_cfg_req new_client_config;
    sns_request config;

    new_client_config.report_rate = chosen_report_rate;
    new_client_config.sample_rate = chosen_sample_rate;
    new_client_config.sensor_type = sensor_type;
    new_client_config.op_mode = NORMAL_MODE;
    SX931x_PRINTF(LOW, this, "sensor type:%d, sample rate %d", sensor_type, (uint8_t)chosen_sample_rate);


    config.message_id  = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
    config.request_len = sizeof(sns_sx931x_cfg_req);
    config.request     = &new_client_config;
    this->instance_api->set_client_config(instance, &config);
}

static bool sx931x_get_decoded_sensor_request(
    sns_sensor const *this,
    sns_request const *in_request,
    sns_std_request *decoded_request,
    sns_std_sensor_config *decoded_payload)
{
    pb_istream_t stream;
    /* decode argument */
    pb_simple_cb_arg arg =
    {
        .decoded_struct = decoded_payload,
        .fields = sns_std_sensor_config_event_fields
    };
    /* decode functions.decode */
    decoded_request->payload = (struct pb_callback_s )
    {
        .funcs.decode = &pb_decode_simple_cb,
               .arg = &arg
    };
    stream = pb_istream_from_buffer(in_request->request, in_request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        SNS_PRINTF(ERROR, this, "LSM decode error");
    }
    return true;
}



static void sx931x_get_sensor_sar_config(sns_sensor *this,
        sns_sensor_instance *instance,
        sx931x_sensor_type sensor_type,
        float *chosen_sample_rate,
        float *chosen_report_rate,
        bool *sensor_sar_client_present)
{
    UNUSED_VAR(this);
    UNUSED_VAR(sensor_type);
    sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
    sns_request const *request;
    sns_sensor_uid* suid = &((sns_sensor_uid)SAR_SUID_0);
    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *sensor_sar_client_present = false;

#if DUAL_SENSOR_SUPPORT
    if (inst_state->hw_idx)
    {
        suid = &((sns_sensor_uid)SAR_SUID_1);
    }
#endif

    /*
    * Parse through existing requests and get fastest sample
    * rate and report rate requests.
    */
    for(request = instance->cb->get_client_request(instance, suid, true);
        NULL != request;
        request = instance->cb->get_client_request(instance, suid, false))
    {
        sns_std_request decoded_request;
        sns_std_sensor_config decoded_payload = {0};

        if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if(sx931x_get_decoded_sensor_request(this, request, &decoded_request, &decoded_payload))
            {
                float report_rate;
                *chosen_sample_rate = SNS_MAX(*chosen_sample_rate, decoded_payload.sample_rate);

                if(decoded_request.has_batching && decoded_request.batching.batch_period > 0)
                {
                    report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
                }
                else
                {
                    report_rate = *chosen_sample_rate;
                }
                *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
                *sensor_sar_client_present = true;
            }
        }
        else if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
        {			
            SX931x_PRINTF(MED, this, "sx931x_get_sensor_sar_config: request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG");
            *sensor_sar_client_present = true;
        }

    }
    inst_state->sar_info.report_timer_hz  = *chosen_report_rate;
    inst_state->sar_info.sampling_rate_hz = *chosen_sample_rate;

    SX931x_PRINTF(LOW, this, "sar sample rate %d sar present %d",
               (uint8_t)inst_state->sar_info.sampling_rate_hz,
               *sensor_sar_client_present);
}


static void  sx931x_mark_sensor_enable_state (sns_sensor_instance *this,
        sx931x_sensor_type sensor_type,
        bool enable)
{
    sx931x_instance_state *inst_state = (sx931x_instance_state *) this->state->state;
    /* mark the corresponding sensor as fifo info field *now only the sw fifo* */
    switch (sensor_type)
    {
    case SX9XXX_SAR:
        if (enable)
        {
            inst_state->deploy_info.publish_sensors |= SX9XXX_SAR;
            inst_state->deploy_info.enable |= SX9XXX_SAR;
        }
        else
        {
            inst_state->deploy_info.publish_sensors &= ~SX9XXX_SAR;
            inst_state->deploy_info.enable &= ~SX9XXX_SAR;
        }
        break;

    default:
        break;
    }
}

/**********************************************************
 * re-evaluate the instance configuration
 **********************************************************/
void sx931x_reval_instance_config(sns_sensor *this,
                                  sns_sensor_instance *instance,
                                  sx931x_sensor_type sensor_type)
{
    sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
    sx931x_state *sensor_state = (sx931x_state *)this->state->state;

    /**
     * 1. Get best sar Config.
     * 2. Get best temperature Config.
     * 5. Decide best Instance Config based on above outputs.
     */
    float p_sample_rate = 0;
    float p_report_rate = 0;
  //float t_sample_rate = 0;
  //float t_report_rate = 0;

    float chosen_sample_rate;
    float chosen_report_rate;

    bool p_sensor_client_present = false;
   //ool t_sensor_client_present = false;

    SX931x_PRINTF(LOW, this, "sensor type: %d %d", sensor_state->sensor, sensor_type);

    if(sensor_type == SX9XXX_SAR)
    {
        sx931x_get_sensor_sar_config(this, instance, sensor_state->sensor,
                                     &p_sample_rate, &p_report_rate,
                                     &p_sensor_client_present);
        chosen_sample_rate = p_sample_rate;
        chosen_report_rate = p_report_rate;
    }
    if(sensor_type == SX9XXX_SAR)
    {
        if (sensor_type == SX9XXX_SAR)
        {
            sx931x_mark_sensor_enable_state(instance, SX9XXX_SAR, p_sensor_client_present);
        }

        SX931x_PRINTF(LOW, this,
                   "sensor type:%d, enable sensor flag:0x%x publish sensor flag:0x%x",
                   sensor_type, inst_state->deploy_info.enable,
                   inst_state->deploy_info.publish_sensors);
        
        /* set the sensor instance configuration*/
        sx931x_set_sensor_inst_config(this, instance,
                                      chosen_report_rate,
                                      chosen_sample_rate,
                                      sensor_type);
    }


    if (!inst_state->deploy_info.enable)
    {
        sensor_state->rail_config.rail_vote = SNS_RAIL_OFF;
        sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                this, &sensor_state->rail_config,
                NULL);
    }
}

void sx931x_sensor_process_suid_events(sns_sensor *const this)
{
    sx931x_state *state = (sx931x_state *)this->state->state;

    for(; 0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
        state->fw_stream->api->get_next_input(state->fw_stream))
    {
        sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);

        if(SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
        {
            pb_istream_t stream = pb_istream_from_buffer((void *)event->event, event->event_len);
            sns_suid_event suid_event = sns_suid_event_init_default;
            pb_buffer_arg data_type_arg = { .buf = NULL, .buf_len = 0 };
            sns_sensor_uid uid_list;
            sns_suid_search suid_search;
            suid_search.suid = &uid_list;
            suid_search.num_of_suids = 0;

            suid_event.data_type.funcs.decode = &pb_decode_string_cb;
            suid_event.data_type.arg = &data_type_arg;
            suid_event.suid.funcs.decode = &pb_decode_suid_event;
            suid_event.suid.arg = &suid_search;

            if(!pb_decode(&stream, sns_suid_event_fields, &suid_event))
            {
                SNS_PRINTF(ERROR, this, "SUID Decode failed");
                continue;
            }

            /* if no suids found, ignore the event */
            if(suid_search.num_of_suids == 0)
            {
                continue;
            }

            /* save suid based on incoming data type name */
            if(0 == strncmp(data_type_arg.buf, "data_acquisition_engine", data_type_arg.buf_len))
            {
                state->dae_suid = uid_list;
            }
            else if(0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len))
            {
                state->irq_suid = uid_list;
            }
            else if(0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
            {
                state->timer_suid = uid_list;
            }
            else if (0 == strncmp(data_type_arg.buf, "async_com_port", data_type_arg.buf_len))
            {
                state->acp_suid = uid_list;
            }
            else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
            {
                state->reg_suid = uid_list;
            }
            else
            {
                SNS_PRINTF(ERROR, this, "invalid datatype_name");
            }
        }
    }
}


/********************************************************
 *
 * Publish attributes read from registry
 *
 ********************************************************/
static void sx931x_publish_registry_attributes(sns_sensor *const this)
{
    sx931x_state *state = (sx931x_state *)this->state->state;
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = state->registry_cfg.is_dri;
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = state->registry_cfg.is_dri;
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = state->registry_cfg.hw_id;
        sns_publish_attribute(
            this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
                                            SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
                                            SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR
                                           };
        for(uint8_t i = 0; i < 12; i++)
        {
            values[i].has_flt = true;
            values[i].flt = state->placement[i];
        }
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
                              values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = state->registry_pf_cfg.rigid_body_type;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
    }
}

static void sx931x_sensor_start_power_rail_timer(sns_sensor *const this,
        sns_time timeout_ticks,
        sx931x_power_rail_pending_state pwr_rail_pend_state)
{
    sx931x_state *state = (sx931x_state *) this->state->state;
    sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
    size_t req_len;
    uint8_t buffer[20];
    SX931x_PRINTF(LOW, this, "start power rail timer");
    sns_memset(buffer, 0, sizeof(buffer));
    req_payload.is_periodic = false;
    req_payload.start_time = sns_get_system_time();
    req_payload.timeout_period = timeout_ticks;

    req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                sns_timer_sensor_config_fields, NULL);
    if(req_len > 0)
    {
        sns_request timer_req =
        {
            .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
            .request = buffer,
            .request_len = req_len
        };
        state->timer_stream->api->send_request(state->timer_stream, &timer_req);
        state->power_rail_pend_state = pwr_rail_pend_state;
    }
    else
    {
        SNS_PRINTF(ERROR, this, "LSM timer req encode error");
    }
    SX931x_PRINTF(LOW, this, "start power rail timer request sent");
}

static void sx931x_sensor_process_registry_event(sns_sensor *const this,
        sns_sensor_event *event)
{
    bool rv = true;
    sx931x_state *state = (sx931x_state *)this->state->state;

    pb_istream_t stream = pb_istream_from_buffer((void *)event->event,
                          event->event_len);
    uint32_t hw_id      = state->hw_idx;

    SX931x_PRINTF(LOW, this, "<sx931x_sensor_process_registry_event = %d>", event->message_id);

    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
        sns_registry_read_event read_event = sns_registry_read_event_init_default;
        pb_buffer_arg group_name = {0, 0};
        read_event.name.arg = &group_name;
        read_event.name.funcs.decode = pb_decode_string_cb;

        if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
        {
            SNS_PRINTF(ERROR, this, "Error decoding registry event");
        }
        else
        {
            stream = pb_istream_from_buffer((void *)event->event, event->event_len);
            if(0 == strncmp((char *)group_name.buf, sx93xx_reg_config[hw_id][REG_CONFIG_SAR],
                                 group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg =
                    {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "config",
                            .parse_func = sns_registry_parse_phy_sensor_cfg,
                            .parsed_buffer = &state->registry_cfg
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if(rv)
                {
                    state->registry_cfg_received = true;
                    state->is_dri = state->registry_cfg.is_dri;
                    state->hardware_id = state->registry_cfg.hw_id;
                    state->resolution_idx = state->registry_cfg.res_idx;
                    state->supports_sync_stream = state->registry_cfg.sync_stream;

                    SX931x_PRINTF(LOW, this, "Registry read event for sensor %d received "
                               "is_dri:%d, hardware_id:%lld ",
                               state->sensor,
                               state->is_dri,
                               state->hardware_id);
                    SX931x_PRINTF(LOW, this, "resolution_idx:%d, supports_sync_stream:%d ",
                               state->resolution_idx,
                               state->supports_sync_stream);
                }
            }
            else if(0 == strncmp((char *)group_name.buf, sx93xx_reg_config[hw_id][REG_PLATFORM_CONFIG],
                                 group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg =
                    {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "config",
                            .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
                            .parsed_buffer = &state->registry_pf_cfg
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if(rv)
                {
                    state->registry_pf_cfg_received = true;

                    state->com_port_info.com_config.bus_type = state->registry_pf_cfg.bus_type;
                    state->com_port_info.com_config.bus_instance = state->registry_pf_cfg.bus_instance;
                    state->com_port_info.com_config.slave_control = state->registry_pf_cfg.slave_config;
                    state->com_port_info.com_config.min_bus_speed_KHz = state->registry_pf_cfg.min_bus_speed_khz;
                    state->com_port_info.com_config.max_bus_speed_KHz = state->registry_pf_cfg.max_bus_speed_khz;
                    state->com_port_info.com_config.reg_addr_type = state->registry_pf_cfg.reg_addr_type;
                    state->irq_config.interrupt_num = state->registry_pf_cfg.dri_irq_num;
                    state->irq_config.interrupt_pull_type = state->registry_pf_cfg.irq_pull_type;
                    state->irq_config.is_chip_pin = state->registry_pf_cfg.irq_is_chip_pin;
                    state->irq_config.interrupt_drive_strength = state->registry_pf_cfg.irq_drive_strength;
                    state->irq_config.interrupt_trigger_type = state->registry_pf_cfg.irq_trigger_type;
                    state->rail_config.num_of_rails = state->registry_pf_cfg.num_rail;
                    state->registry_rail_on_state = state->registry_pf_cfg.rail_on_state;
                    sns_strlcpy(state->rail_config.rails[0].name,
                                state->registry_pf_cfg.vddio_rail,
                                sizeof(state->rail_config.rails[0].name));
                    sns_strlcpy(state->rail_config.rails[1].name,
                                state->registry_pf_cfg.vdd_rail,
                                sizeof(state->rail_config.rails[1].name));

                    SX931x_PRINTF(LOW, this, "Registry read event for group %d received "
                               "bus_type:%d bus_instance:%d slave_control:%d",
                               state->sensor,
                               state->com_port_info.com_config.bus_type,
                               state->com_port_info.com_config.bus_instance,
                               state->com_port_info.com_config.slave_control);

                    SX931x_PRINTF(LOW, this, "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
                               state->com_port_info.com_config.min_bus_speed_KHz,
                               state->com_port_info.com_config.max_bus_speed_KHz,
                               state->com_port_info.com_config.reg_addr_type);
                    SX931x_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d",
                               state->irq_config.interrupt_num,
                               state->irq_config.interrupt_pull_type,
                               state->irq_config.is_chip_pin);

                    SX931x_PRINTF(LOW, this, "interrupt_drive_strength:%d interrupt_trigger_type:%d"
                               " rigid body type:%d",
                               state->irq_config.interrupt_drive_strength,
                               state->irq_config.interrupt_trigger_type,
                               state->registry_pf_cfg.rigid_body_type);

                    SX931x_PRINTF(LOW, this, "num_rail:%d, rail_on_state:%d, vddio_rail:%s, vdd_rail:%s", state->rail_config.num_of_rails,
                               state->registry_rail_on_state,
                               state->rail_config.rails[0].name,
                               state->rail_config.rails[1].name);

                }
            }
            else if(0 == strncmp((char *)group_name.buf, sx93xx_reg_config[hw_id][REG_PLATFORM_PLACEMENT],
                                 group_name.buf_len))
            {
                {
                    uint8_t arr_index = 0;
                    pb_float_arr_arg arr_arg =
                    {
                        .arr = state->placement,
                        .arr_index = &arr_index,
                        .arr_len = 12
                    };

                    sns_registry_decode_arg arg =
                    {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "placement",
                            .parse_func = sns_registry_parse_float_arr,
                            .parsed_buffer = &arr_arg
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if(rv)
                {
                    state->registry_placement_received = true;
                    SX931x_PRINTF(LOW, this, "Registry read event for group %d received", state->sensor);
                }
            }
            else
            {
                SNS_PRINTF(ERROR, this, "no find the group_name");
                rv = false;
            }

            if(!rv)
            {
                SNS_PRINTF(ERROR, this, "Error decoding registry group %s due to %s", (char *)group_name.buf,
                           PB_GET_ERROR(&stream));
            }
        }
    }
    else
    {
        SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u", event->message_id);
    }
}

static void sx931x_sensor_send_registry_request(sns_sensor *const this, char *reg_group_name)
{
    sx931x_state *state = (sx931x_state *)this->state->state;
    uint8_t buffer[100];
    int32_t encoded_len;
    sns_memset(buffer, 0, sizeof(buffer));
    sns_rc rc = SNS_RC_SUCCESS;
    sns_registry_read_req read_request;
    pb_buffer_arg data = (pb_buffer_arg)
    {
        .buf = reg_group_name,
         .buf_len = (strlen(reg_group_name) + 1)
    };

    read_request.name.arg = &data;
    read_request.name.funcs.encode = pb_encode_string_cb;

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
                                    &read_request, sns_registry_read_req_fields, NULL);
    if(0 < encoded_len)
    {
        sns_request request = (sns_request)
        {
            .request_len = encoded_len, .request = buffer,
             .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ
        };
        rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    }
}


/*****************************************************************
 *  self test for instance config.
 *****************************************************************/
void sx931x_set_self_test_inst_config(sns_sensor *this, sns_sensor_instance *instance)
{
    sns_request config;
    config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    config.request_len = 0;
    config.request = NULL;

    SX931x_PRINTF(LOW, this, "sx931x_set_self_test_inst_config");
    this->instance_api->set_client_config(instance, &config);
}

static void sx931x_publish_available(sns_sensor *const this)
{
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;

    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

sns_rc sns_see_sx931x_sensor_notify_event(sns_sensor *const this)
{
    sx931x_state *state = (sx931x_state *) this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    sns_time on_timestamp;
    uint8_t buffer[1];
    sns_rc rv = SNS_RC_SUCCESS;
    sns_sensor_event *event;
    uint32_t hw_id = state->hw_idx;


    if (state->fw_stream)
    {
        SX931x_PRINTF(LOW, this, ">>fw_stream<<");
        if ((0 == sns_memcmp(&state->acp_suid, &((sns_sensor_uid)
    {
        {
            0
        }
    }), sizeof(state->acp_suid)))
    || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid)
    {
        {
            0
        }
    }), sizeof(state->timer_suid)))
    || (0 == sns_memcmp(&state->dae_suid, &((sns_sensor_uid)
    {
        {
            0
        }
    }), sizeof(state->dae_suid)))
    || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid)
    {
        {
            0
        }
    }), sizeof(state->reg_suid))))
        {
            sx931x_sensor_process_suid_events(this);
            /* request to registry sensor*/
            if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid)
        {
            {
                0
            }
        }), sizeof(state->reg_suid))))
            {
                if(state->reg_data_stream == NULL)
                {
                    stream_svc->api->create_sensor_stream(stream_svc, this, state->reg_suid, &state->reg_data_stream);
                    sx931x_sensor_send_registry_request(this, sx93xx_reg_config[hw_id][REG_PLATFORM_CONFIG]);
                    sx931x_sensor_send_registry_request(this, sx93xx_reg_config[hw_id][REG_PLATFORM_PLACEMENT]);
                    if(SX9XXX_SAR == state->sensor)
                    {
                        sx931x_sensor_send_registry_request(this, sx93xx_reg_config[hw_id][REG_CONFIG_SAR]);
                    }
                }
            }
        }
    }


    /**----------------------Handle a Timer Sensor event.-------------------*/
    if (NULL != state->timer_stream)
    {
        event = state->timer_stream->api->peek_input(state->timer_stream);
        while (NULL != event)
        {
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                  event->event_len);
            sns_timer_sensor_event timer_event;
            SX931x_PRINTF(LOW, this, ">>timer event<< message_id = %d", event->message_id);
            if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
            {
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    if (state->power_rail_pend_state == POWER_RAIL_PENDING_INIT)
                    {
                        /**-------------------Read and Confirm WHO-AM-I------------------------*/
                        buffer[0] = 0xFF;
                        rv = sx931x_get_who_am_i(state->scp_service, state->com_port_info.port_handle, &buffer[0]);
                        SX931x_PRINTF(LOW, this, "sx931x who am i is 0x%x", buffer[0]);

                        if (rv == SNS_RC_SUCCESS && buffer[0] == SX931x_WHOAMI_VALUE)
                        {
                            /* Reset Sensor */
                            rv = sx931x_device_sw_reset(state->scp_service, state->com_port_info.port_handle);

                            if (rv == SNS_RC_SUCCESS)
                            {
                                state->hw_is_present = true;
                            }
                        }
                        state->who_am_i = buffer[0];
                        /**------------------Power Down and Close COM Port--------------------*/
                        state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                false);

                        state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
                        state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);

                        /**----------------------Turn Power Rail OFF--------------------------*/
                        state->rail_config.rail_vote = SNS_RAIL_OFF;
                        state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                this,
                                &state->rail_config,
                                NULL);
                        if (state->hw_is_present)
                        {
                            sx931x_publish_available(this);
                            SNS_PRINTF(LOW, this, "sensor:%d initialize finished: %d", state->sensor, state->hw_idx);
                        }
                        else
                        {
                            rv = SNS_RC_INVALID_STATE;
                            SNS_PRINTF(ERROR, this, "sx931x HW absent");
                        }
                        state->power_rail_pend_state = POWER_RAIL_PENDING_NONE;
                    }
                    else if(state->power_rail_pend_state == POWER_RAIL_PENDING_SET_CLIENT_REQ)
                    {
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
                        if(NULL != instance)
                        {
                            sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
                            inst_state->instance_is_ready_to_configure = true;
                            sx931x_reval_instance_config(this, instance, state->sensor);
                            if(inst_state->new_self_test_request)
                            {
                                sx931x_set_self_test_inst_config(this, instance);
                            }
                        }
                        state->power_rail_pend_state  = POWER_RAIL_PENDING_NONE;
                    }
                }
                else
                {
                    SNS_PRINTF(ERROR, this, "pb_decode error");
                }
            }
            event = state->timer_stream->api->get_next_input(state->timer_stream);
        }
    }

    if(NULL != state->reg_data_stream)
    {
        SX931x_PRINTF(LOW, this, ">>reg_data_stream<<");
        event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
        while(NULL != event)
        {
            sx931x_sensor_process_registry_event(this, event);
            if(state->registry_cfg_received && state->registry_placement_received)
            {
                sx931x_publish_registry_attributes(this);
            }
            event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
        }
    }

    if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid)
{
    {
        0
    }
}), sizeof(state->timer_suid)) &&
state->registry_pf_cfg_received && state->registry_cfg_received && state->registry_placement_received)
    {
        state->registry_pf_cfg_received = false;

        /***************Register and Open COM Port************************************/
        if (NULL == state->com_port_info.port_handle)
        {
            rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                    &state->com_port_info.port_handle);
            SX931x_PRINTF(LOW, this, "register com port success? rv=%d", rv);
            /* open the COM port*/
            if(rv == SNS_RC_SUCCESS)
            {
                rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
                SX931x_PRINTF(LOW, this, "open com port success> rv=%d, handle=%p", rv,
                           state->com_port_info.port_handle);
            }
        }

        /****************************Register Power Rails*****************************/
        if (0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid)
    {
        {
            0
        }
    }), sizeof(state->timer_suid))
    && NULL == state->pwr_rail_service
    && rv == SNS_RC_SUCCESS)
        {
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service =
                (sns_pwr_rail_service *)service_mgr->get_service(service_mgr, SNS_POWER_RAIL_SERVICE);
            /* ************************register power rails ******************************/
            state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                    &state->rail_config);

            SX931x_PRINTF(LOW, this, "register power rails: rail vote:%d, num of rails:%d [1]%s [2]%s rv=%d",
                       state->rail_config.rail_vote,
                       state->rail_config.num_of_rails,
                       state->rail_config.rails[0].name,
                       state->rail_config.rails[1].name,
                       rv);

            /*************************Turn Power Rails ON*******************************/
            if (state->sensor == SX9XXX_SAR)
            {
                state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
            }
            else
            {
                state->rail_config.rail_vote = SNS_RAIL_OFF;
            }
            rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                    this,
                    &state->rail_config,
                    &on_timestamp);
            SX931x_PRINTF(LOW, this, "vote power rails value:%d rv=%d", state->rail_config.rail_vote, rv);

            /************************Create a Timer stream for Power Rail ON timeout.*********/
            if (NULL == state->timer_stream)
            {
                SX931x_PRINTF(LOW, this, "here create the timer stream");
                stream_svc->api->create_sensor_stream(stream_svc,
                                                      this,
                                                      state->timer_suid,
                                                      &state->timer_stream);
                if (NULL != state->timer_stream)
                {
                    sx931x_sensor_start_power_rail_timer(this,
                                                         sns_convert_ns_to_ticks(SX93xx_OFF_TO_IDLE_MS * 1000 * 1000),
                                                         POWER_RAIL_PENDING_INIT);
                }
            }
        }
    }
    return rv;
}

/**********************************************************************************
 * Decodes self test requests.
 **********************************************************************************/
static bool sx931x_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
        sns_std_request *decoded_request,
        sns_physical_sensor_test_config *test_config)
{
    pb_istream_t stream;
    pb_simple_cb_arg arg = {.decoded_struct = test_config,
                            .fields = sns_physical_sensor_test_config_fields
                           };
    decoded_request->payload = (struct pb_callback_s)
    {
        .funcs.decode = &pb_decode_simple_cb,
               .arg = &arg
    };

    stream = pb_istream_from_buffer(request->request, request->request_len);
    if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
        SNS_PRINTF(ERROR, this, "LSM decode error");
        return false;
    }
    return true;
}


/**
 * Decodes a physical sensor self test request and updates
 * instance state with request info.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[i] new_request Encoded request
 *
 * @return True if request is valid else false
 */
static bool sx931x_extract_self_test_info(sns_sensor *this,
        sns_sensor_instance *instance,
        struct sns_request const *new_request)
{
    sns_std_request decoded_request;
    sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
    sx931x_state *state = (sx931x_state *)this->state->state;
    sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
    sx931x_self_test_info *self_test_info;

    if(state->sensor == SX9XXX_SAR)
    {
        self_test_info = &inst_state->sar_info.test_info;
    }
    else
    {
        return false;
    }

    if(sx931x_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
    {
        self_test_info->test_type = test_config.test_type;
        self_test_info->test_client_present = true;
        return true;
    }
    else
    {
        return false;
    }
}


static void sx931x_turn_rails_off(sns_sensor *this)
{
    sns_sensor *sensor;

    for(sensor = this->cb->get_library_sensor(this, true);
        NULL != sensor;
        sensor = this->cb->get_library_sensor(this, false))
    {
        sx931x_state *sensor_state = (sx931x_state *)sensor->state->state;
        if(sensor_state->rail_config.rail_vote != SNS_RAIL_OFF)
        {
            sensor_state->rail_config.rail_vote = SNS_RAIL_OFF;
            sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                                                                            sensor,
                                                                            &sensor_state->rail_config,
                                                                            NULL);
        }
    }
}


/** See sns_sx931x_sensor.h */
sns_sensor_instance *sns_see_sx931x_sensor_set_client_request(sns_sensor *const this,
        struct sns_request const *exist_request,
        struct sns_request const *new_request,
        bool remove)
{
    sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
    sx931x_state *state = (sx931x_state *)this->state->state;
    sns_time on_timestamp;
    sns_time delta;
    bool reval_config = false;
    sns_sensor_uid* suid = &((sns_sensor_uid)SAR_SUID_0);

    SX931x_PRINTF(LOW, this, "<sns_see_if__ set_client_request> for %d", state->sensor);

#if DUAL_SENSOR_SUPPORT
    if(state->hw_idx)
    {
        suid = &((sns_sensor_uid)SAR_SUID_1);
    }
#endif

    if (remove)
    {
        if(NULL != instance)
        {
            SX931x_PRINTF(LOW, this, "<sns_see_if__ removing request", exist_request->message_id);
            instance->cb->remove_client_request(instance, exist_request);
            sx931x_reval_instance_config(this, instance, state->sensor);
        }
    }
    else
    {
        // 1. If new request then:
        //     a. Power ON rails.
        //     b. Power ON COM port - Instance must handle COM port power.
        //     c. Create new instance.
        //     d. Re-evaluate existing requests and choose appropriate instance config.
        //     e. set_client_config for this instance.
        //     f. Add new_request to list of requests handled by the Instance.
        //     g. Power OFF COM port if not needed- Instance must handle COM port power.
        //     h. Return the Instance.
        // 2. If there is an Instance already present:
        //     a. Add new_request to list of requests handled by the Instance.
        //     b. Remove exist_request from list of requests handled by the Instance.
        //     c. Re-evaluate existing requests and choose appropriate Instance config.
        //     d. set_client_config for the Instance if not the same as current config.
        //     e. publish the updated config.
        //     f. Return the Instance.
        // 3.  If "flush" request:
        //     a. Perform flush on the instance.
        //     b. Return NULL.

        if(NULL == instance)
        {
            if(state->sensor == SX9XXX_SAR)
            {
                state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
            }
            else
            {
                state->rail_config.rail_vote = SNS_RAIL_OFF;
            }

            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                    this,
                    &state->rail_config,
                    &on_timestamp);

            delta = sns_get_system_time() - on_timestamp;

            // Use on_timestamp to determine correct Timer value.
            if(delta < sns_convert_ns_to_ticks(SX93xx_OFF_TO_IDLE_MS * 1000 * 1000))
            {
                sx931x_sensor_start_power_rail_timer(this,
                                                     sns_convert_ns_to_ticks(SX93xx_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
                                                     POWER_RAIL_PENDING_SET_CLIENT_REQ);
            }
            else
            {
                reval_config = true; /*rail is already ON*/
            }

            /** create_instance() calls init() for the Sensor Instance */
            SX931x_PRINTF(LOW, this, "instace is NULL, now create a sigaltone instance");
            instance = this->cb->create_instance(this, sizeof(sx931x_instance_state));
            /* If rail is already ON then flag instance OK to configure */
            if(reval_config)
            {
                sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
                inst_state->instance_is_ready_to_configure = true;
                SX931x_PRINTF(LOW, this, "reval configure with instance is NULL case ???");
            }
        }
        else
        {
            if(NULL != exist_request && NULL != new_request
               && (new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
            {
                sns_sensor_util_send_flush_event(&state->my_suid, instance);
                return instance;
            }
            else
            {
                reval_config = true;

                /** An existing client is changing request*/
                if((NULL != exist_request) && (NULL != new_request))
                {
                    instance->cb->remove_client_request(instance, exist_request);
                }
                /** A new client sent new_request*/
                else if(NULL != new_request)
                {
                    // No-op. new_request will be added to requests list below.
                }
            }
        }
        /** Add the new request to list of client_requests.*/
        if(NULL != instance)
        {
            sx931x_instance_state *inst_state = (sx931x_instance_state *)instance->state->state;
            if(new_request != NULL)
            {
                SX931x_PRINTF(LOW, 
                           this, "new request from %d message id:%d add to the client request list",
                           state->sensor,
                           new_request->message_id);
                instance->cb->add_client_request(instance, new_request);
                if(new_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
                {
                    if(sx931x_extract_self_test_info(this, instance, new_request))
                    {
                        inst_state->new_self_test_request = true;
                    }
                }
            }
            if (reval_config && inst_state->instance_is_ready_to_configure)
            {
                SX931x_PRINTF(LOW, this, "try to configure");
                sx931x_reval_instance_config(this, instance, state->sensor);
                if(inst_state->new_self_test_request)
                {
                    sx931x_set_self_test_inst_config(this, instance);
                }
            }
        }
    }

    if(NULL != instance &&
       NULL == instance->cb->get_client_request(instance, suid, true))
    {
        this->cb->remove_instance(instance);
        sx931x_turn_rails_off(this);
    }

    return instance;
}

void sns_see_sx931x_send_suid_req(sns_sensor *this, char *const data_type,
                                  uint32_t data_type_len)
{
    uint8_t buffer[50];
    sns_memset(buffer, 0, sizeof(buffer));
    sx931x_state *state = (sx931x_state *)this->state->state;
    sns_service_manager *manager = this->cb->get_service_manager(this);
    sns_stream_service *stream_service =
        (sns_stream_service *)manager->get_service(manager, SNS_STREAM_SERVICE);
    size_t encoded_len;
    pb_buffer_arg data = (pb_buffer_arg)
    {
        .buf = data_type, .buf_len = data_type_len
    };

    sns_suid_req suid_req = sns_suid_req_init_default;
    suid_req.has_register_updates = true;
    suid_req.register_updates = true;
    suid_req.data_type.funcs.encode = &pb_encode_string_cb;
    suid_req.data_type.arg = &data;


    /* create the event/data stream for the platform resource */
    if (state->fw_stream == NULL)
    {
        stream_service->api->create_sensor_stream(stream_service, this,
                sns_get_suid_lookup(),
                &state->fw_stream);
    }

    encoded_len = pb_encode_request(buffer,
                                    sizeof(buffer),
                                    &suid_req,
                                    sns_suid_req_fields,
                                    NULL);
    if (encoded_len > 0)
    {
        sns_request request = (sns_request)
        {
            .request_len = encoded_len,
             .request = buffer,
              .message_id = SNS_SUID_MSGID_SNS_SUID_REQ
        };
        state->fw_stream->api->send_request(state->fw_stream, &request);
    }
}
