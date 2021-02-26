/*******************************************************************************
 * Copyright (c) 2017-18, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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

/**
 * @file sns_bmi160_sensor.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <string.h>
#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_service.h"
#include "sns_sensor_util.h"
#include "sns_types.h"
#include "sns_attribute_util.h"

#include "sns_bmi160_sensor.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_motion_detect.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"
#include "sns_cal_util.h"
#include "sns_registry.pb.h"


#define BMI160_REG_NN_ACCEL                     "bmi160_0.accel.config"
#define BMI160_REG_NN_GYRO                      "bmi160_0.gyro.config"
#define BMI160_REG_NN_TEMP                      "bmi160_0.temp.config"
#define BMI160_REG_NN_MD                        "bmi160_0.md.config"
#define BMI160_REG_NN_PLATFORM_CONFIG           "bmi160_0_platform.config"
#define BMI160_REG_NN_PLATFORM_PLACEMENT        "bmi160_0_platform.placement"
#define BMI160_REG_NN_PLATFORM_ORIENT           "bmi160_0_platform.orient"
#define BMI160_REG_NN_PLATFORM_FAC_CAL_ACCEL    "bmi160_0_platform.accel.fac_cal"
#define BMI160_REG_NN_PLATFORM_FAC_CAL_GYRO     "bmi160_0_platform.gyro.fac_cal"
#define BMI160_REG_NN_PLATFORM_FAC_CAL_TEMP     "bmi160_0_platform.temp.fac_cal"
#define BMI160_REG_NN_PLATFORM_MD_CONFIG        "bmi160_0_platform.md.config"

typedef struct pb_arg_reg_group_arg
{
    sns_sensor_instance* instance;
    const char*         name;
    bmi160_sensor_type  sensor;
    uint32_t            version;
} pb_arg_reg_group_arg;


extern const range_attr bmi160_accel_ranges[];
extern const float bmi160_accel_resolutions[];
extern const range_attr bmi160_gyro_ranges[];
extern const float bmi160_gyro_resolutions[];


// <power rail>
#if BMI160_CONFIG_POWER_RAIL
static sns_rc bmi160_register_power_rail(sns_sensor *const this)
{
    bmi160_state *sstate = (bmi160_state*)this->state->state;
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_rc rv = SNS_RC_SUCCESS;

    sstate->common.rail_config.rail_vote = SNS_RAIL_OFF;

    if(NULL == sstate->pwr_rail_service)
    {
        sstate->pwr_rail_service =
            (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);

        sstate->pwr_rail_service->api->sns_register_power_rails(sstate->pwr_rail_service, &sstate->common.rail_config);
    }

    if(NULL == sstate->pwr_rail_service)
    {
        rv = SNS_RC_FAILED;
    }
    return rv;
}
#endif



// <registry>
#if BMI160_CONFIG_ENABLE_REGISTRY

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
void bmi160_publish_registry_attributes(sns_sensor *const this)
{
#if !BMI160_CONFIG_ENABLE_SEE_LITE
    bmi160_state *sstate = (bmi160_state*)this->state->state;
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = sstate->is_dri;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_boolean = true;
        value.boolean = sstate->supports_sync_stream;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = sstate->hardware_id;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
    }
    {
        sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
            SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
        for(uint8_t i = 0; i < 12; i++)
        {
            values[i].has_flt = true;
            values[i].flt = sstate->common.placement[i];
        }
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
                values, ARR_SIZE(values), false);
    }
    {
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_sint = true;
        value.sint = sstate->common.registry_pf_cfg.rigid_body_type;
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
    }

    if(sstate->sensor == BMI160_ACCEL ||
            sstate->sensor == BMI160_GYRO)
    {
        /** Only accel and gyro use registry information for selected resolution. */
        sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
        value.has_flt = true;
        value.flt = (sstate->sensor == BMI160_ACCEL) ?
            bmi160_accel_resolutions[sstate->resolution_idx] :
            bmi160_gyro_resolutions[sstate->resolution_idx];
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1, false);
    }

    /** Only accel and gyro use registry information for selected range. */
    if (sstate->sensor == BMI160_ACCEL ||
            sstate->sensor == BMI160_GYRO)
    {
        sns_std_attr_value_data values[] = {SNS_ATTR};
        sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
        rangeMinMax[0].has_flt = true;
        rangeMinMax[0].flt = (sstate->sensor == BMI160_ACCEL) ?
            bmi160_accel_ranges[sstate->resolution_idx].min :
            bmi160_gyro_ranges[sstate->resolution_idx].min;
        rangeMinMax[1].has_flt = true;
        rangeMinMax[1].flt = (sstate->sensor == BMI160_ACCEL) ?
            bmi160_accel_ranges[sstate->resolution_idx].max :
            bmi160_gyro_ranges[sstate->resolution_idx].max;
        values[0].has_subtype = true;
        values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
        values[0].subtype.values.arg =
            &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
        sns_publish_attribute(
                this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
    }
#else
    UNUSED_VAR(this);
#endif
}


void bmi160_sensor_check_registry_col_progress(sns_sensor *const this)

{
    bmi160_state *sstate = (bmi160_state*)this->state->state;

    if (sstate->common.hw_is_present) {
        bool registry_collection_done;

        if (sstate->registry_cfg_received && !sstate->registry_attr_published) {
            sstate->registry_attr_published = 1;
            bmi160_publish_registry_attributes(this);
            BMI160_SENSOR_LOG(LOW, this, "registry attr published: %d", sstate->sensor);
        }

        registry_collection_done =
            sstate->registry_cfg_received &&
            sstate->common.registry_pf_cfg_received &&
            sstate->common.registry_orient_received && sstate->common.registry_placement_received &&
            sstate->registry_md_config_received;

        /** More clean up. */
        switch (sstate->sensor) {
            case BMI160_ACCEL:
            case BMI160_GYRO:
                //TODOMAG
                //case BMI160_SENSOR_TEMP:
                registry_collection_done = registry_collection_done && sstate->registry_fac_cal_received;
                break;
            default:
                break;
        }

        if (registry_collection_done) {
            sns_sensor_util_remove_sensor_stream(this, &sstate->reg_data_stream);
            BMI160_SENSOR_LOG(LOW, this, "registry_collection_done: %d", sstate->sensor);
        }
    }
}

void bmi160_sensor_process_registry_event(sns_sensor *const this,
        sns_sensor_event *event)
{
    bool rv = true;
    bmi160_state *sstate = (bmi160_state*)this->state->state;

    pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
            event->event_len);


    BMI160_SENSOR_LOG(LOW, this, "process_registry_event snr: %d", sstate->sensor);

    if (SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
        sns_registry_read_event read_event = sns_registry_read_event_init_default;
        pb_buffer_arg group_name = {0,0};
        read_event.name.arg = &group_name;
        read_event.name.funcs.decode = pb_decode_string_cb;

        if (!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
        {
            BMI160_SENSOR_LOG(ERROR, this, "Error decoding registry event");
        }
        else
        {
            stream = pb_istream_from_buffer((void*)event->event, event->event_len);

            if (0 == strncmp((char*)group_name.buf, BMI160_REG_NN_ACCEL,
                        group_name.buf_len) ||
                    0 == strncmp((char*)group_name.buf, BMI160_REG_NN_GYRO,
                        group_name.buf_len) ||
                    0 == strncmp((char*)group_name.buf, BMI160_REG_NN_TEMP,
                        group_name.buf_len) ||
                    0 == strncmp((char*)group_name.buf, BMI160_REG_NN_MD,
                        group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg = {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "config",
                            .parse_func = sns_registry_parse_phy_sensor_cfg,
                            .parsed_buffer = &sstate->common.registry_cfg
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv)
                {
                    sstate->registry_cfg_received = true;
                    sstate->is_dri = sstate->common.registry_cfg.is_dri;
                    sstate->hardware_id = sstate->common.registry_cfg.hw_id;
                    sstate->resolution_idx = sstate->common.registry_cfg.res_idx;
                    sstate->supports_sync_stream = sstate->common.registry_cfg.sync_stream;

                    if (BMI160_ACCEL == sstate->sensor) {
                        if (sstate->resolution_idx > BMI160_RANGE_ACC_PM16G) {
                            sstate->resolution_idx = BMI160_RANGE_ACC_PM16G;
                        }
                    } else if (BMI160_GYRO == sstate->sensor) {
                        if (sstate->resolution_idx > BMI160_RANGE_GYR_PM2000DPS) {
                            sstate->resolution_idx = BMI160_RANGE_GYR_PM2000DPS;
                        }
                    }

                    BMI160_SENSOR_LOG(LOW, this, "sensor: %d is_dri:%d, hardware_id:%d ",
                            sstate->sensor,
                            sstate->is_dri,
                            (int)sstate->hardware_id);
                    BMI160_SENSOR_LOG(LOW, this, "sensor: %d resolution_idx:%d, supports_sync_stream:%d ",
                            sstate->sensor,
                            sstate->common.registry_cfg.res_idx,
                            sstate->supports_sync_stream);
                }
            }
            else if (0 == strncmp((char*)group_name.buf, BMI160_REG_NN_PLATFORM_CONFIG,
                        group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg = {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "config",
                            .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
                            .parsed_buffer = &sstate->common.registry_pf_cfg
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv)
                {
                    sstate->common.registry_pf_cfg_received = true;

                    sstate->common.com_port_info.com_config.bus_type = (sns_bus_type)sstate->common.registry_pf_cfg.bus_type;
                    sstate->common.com_port_info.com_config.bus_instance = sstate->common.registry_pf_cfg.bus_instance;
                    sstate->common.com_port_info.com_config.slave_control = sstate->common.registry_pf_cfg.slave_config;
                    sstate->common.com_port_info.com_config.min_bus_speed_KHz = sstate->common.registry_pf_cfg.min_bus_speed_khz;
                    sstate->common.com_port_info.com_config.max_bus_speed_KHz = sstate->common.registry_pf_cfg.max_bus_speed_khz;
                    sstate->common.com_port_info.com_config.reg_addr_type = (sns_reg_addr_type)sstate->common.registry_pf_cfg.reg_addr_type;
                    sstate->common.irq_config.interrupt_num = sstate->common.registry_pf_cfg.dri_irq_num;
                    sstate->common.irq_config.interrupt_pull_type = (sns_interrupt_pull_type)sstate->common.registry_pf_cfg.irq_pull_type;
                    sstate->common.irq_config.is_chip_pin = sstate->common.registry_pf_cfg.irq_is_chip_pin;
                    sstate->common.irq_config.interrupt_drive_strength = (sns_interrupt_drive_strength)sstate->common.registry_pf_cfg.irq_drive_strength;
                    sstate->common.irq_config.interrupt_trigger_type = (sns_interrupt_trigger_type)sstate->common.registry_pf_cfg.irq_trigger_type;
                    sstate->common.rail_config.num_of_rails = sstate->common.registry_pf_cfg.num_rail;
                    sstate->common.registry_rail_on_state = (sns_power_rail_state)sstate->common.registry_pf_cfg.rail_on_state;
                    sns_strlcpy(sstate->common.rail_config.rails[0].name,
                            sstate->common.registry_pf_cfg.vddio_rail,
                            sizeof(sstate->common.rail_config.rails[0].name));
                    sns_strlcpy(sstate->common.rail_config.rails[1].name,
                            sstate->common.registry_pf_cfg.vdd_rail,
                            sizeof(sstate->common.rail_config.rails[1].name));

                    BMI160_SENSOR_LOG(LOW, this, "bus_type:%d bus_instance:%d slave_control:%d",
                            sstate->common.com_port_info.com_config.bus_type,
                            sstate->common.com_port_info.com_config.bus_instance,
                            sstate->common.com_port_info.com_config.slave_control);

                    BMI160_SENSOR_LOG(LOW, this, "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
                            sstate->common.com_port_info.com_config.min_bus_speed_KHz,
                            sstate->common.com_port_info.com_config.max_bus_speed_KHz,
                            sstate->common.com_port_info.com_config.reg_addr_type);

                    BMI160_SENSOR_LOG(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d",
                            sstate->common.irq_config.interrupt_num,
                            sstate->common.irq_config.interrupt_pull_type,
                            sstate->common.irq_config.is_chip_pin);

                    BMI160_SENSOR_LOG(LOW, this, "interrupt_drive_strength:%d interrupt_trigger_type:%d"
                            " rigid body type:%d",
                            sstate->common.irq_config.interrupt_drive_strength,
                            sstate->common.irq_config.interrupt_trigger_type,
                            sstate->common.registry_pf_cfg.rigid_body_type);

                }
            }

            //OPTIM3
            else if (0 == strncmp((char*)group_name.buf, BMI160_REG_NN_PLATFORM_PLACEMENT,
                        group_name.buf_len))
            {
                {
                    uint8_t arr_index = 0;
                    pb_float_arr_arg arr_arg = {
                        .arr = sstate->common.placement,
                        .arr_index = &arr_index,
                        .arr_len = 12
                    };

                    sns_registry_decode_arg arg = {
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

                if (rv)
                {
                    sstate->common.registry_placement_received = true;
                }
            }

            //OPTIM3
            else if (0 == strncmp((char*)group_name.buf, BMI160_REG_NN_PLATFORM_ORIENT,
                        group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg = {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = "orient",
                            .parse_func = sns_registry_parse_axis_orientation,
                            .parsed_buffer = sstate->common.axis_map
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv)
                {
                    sstate->common.registry_orient_received = true;
                    //SENSOR_PRINTF_LOW_FULL(this, "Registry read event for group %s received ", (char*)group_name.buf);

                    BMI160_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                            sstate->common.axis_map[0].ipaxis,
                            sstate->common.axis_map[0].opaxis, sstate->common.axis_map[0].invert);

                    BMI160_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                            sstate->common.axis_map[1].ipaxis, sstate->common.axis_map[1].opaxis,
                            sstate->common.axis_map[1].invert);

                    BMI160_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                            sstate->common.axis_map[2].ipaxis, sstate->common.axis_map[2].opaxis,
                            sstate->common.axis_map[2].invert);
                }
            }
            else if (0 == strncmp((char*)group_name.buf,
                        BMI160_REG_NN_PLATFORM_FAC_CAL_ACCEL,
                        group_name.buf_len) ||
                    0 == strncmp((char*)group_name.buf,
                        BMI160_REG_NN_PLATFORM_FAC_CAL_GYRO,
                        group_name.buf_len) ||
                    0 == strncmp((char*)group_name.buf,
                        BMI160_REG_NN_PLATFORM_FAC_CAL_TEMP,
                        group_name.buf_len))
            {
                uint32_t fac_cal_version = 0;
                float fac_cal_bias[TRIAXIS_NUM];
                {
                    uint8_t bias_arr_index = 0, scale_arr_index = 0;
                    pb_float_arr_arg bias_arr_arg = {
                        .arr = fac_cal_bias,
                        .arr_index = &bias_arr_index,
                        .arr_len = TRIAXIS_NUM
                    };

                    pb_float_arr_arg scale_arr_arg = {
                        .arr = sstate->fac_cal_scale,
                        .arr_index = &scale_arr_index,
                        .arr_len = TRIAXIS_NUM
                    };

                    sns_registry_decode_arg arg = {
                        .item_group_name = &group_name,
                        .parse_info_len = 3,
                        .parse_info[0] = {
                            .group_name = "bias",
                            .parse_func = sns_registry_parse_float_arr,
                            .parsed_buffer = &bias_arr_arg
                        },
                        .parse_info[1] = {
                            .group_name = "scale",
                            .parse_func = sns_registry_parse_float_arr,
                            .parsed_buffer = &scale_arr_arg
                        },
                        .parse_info[2] = {
                            .group_name = "corr_mat",
                            .parse_func = sns_registry_parse_corr_matrix_3,
                            .parsed_buffer = &sstate->fac_cal_corr_mat
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);

                    fac_cal_version = arg.version;
                }

                if (rv)
                {
                    sstate->registry_fac_cal_received = true;
                    sstate->fac_cal_version = fac_cal_version;
                    BMI160_SENSOR_LOG(LOW, this, "fac_cal received: %d", sstate->sensor);

                    uint8_t i;
                    for (i = 0; i < TRIAXIS_NUM; i++) {
                        sstate->fac_cal_bias[i] = roundf(fac_cal_bias[i] * sstate->scale_factor);
                    }

                    if (sstate->fac_cal_scale[0] != 0.0)
                    {
                        sstate->fac_cal_corr_mat.e00 = sstate->fac_cal_scale[0];
                        sstate->fac_cal_corr_mat.e11 = sstate->fac_cal_scale[1];
                        sstate->fac_cal_corr_mat.e22 = sstate->fac_cal_scale[2];
                    }
                } else {
                    BMI160_SENSOR_LOG(ERROR, this, "fac_cal error: %d", sstate->sensor);
                }
            }
            else if (0 == strncmp((char*)group_name.buf,
                        BMI160_REG_NN_PLATFORM_MD_CONFIG,
                        group_name.buf_len))
            {
                {
                    sns_registry_decode_arg arg = {
                        .item_group_name = &group_name,
                        .parse_info_len = 1,
                        .parse_info[0] = {
                            .group_name = BMI160_REG_NN_PLATFORM_MD_CONFIG,
                            .parse_func = sns_registry_parse_md_cfg,
                            .parsed_buffer = &sstate->md_config
                        }
                    };

                    read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
                    read_event.data.items.arg = &arg;

                    rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
                }

                if (rv)
                {
                    BMI160_SENSOR_LOG(LOW, this, "md_config received: %d %d %d %d",
                            sstate->sensor, (int)(sstate->md_config.thresh * 1e6), (int)(sstate->md_config.win * 1e6), sstate->md_config.disable);
                    sstate->registry_md_config_received = true;
                }
            }
            else
            {
                rv = false;
            }

            if (!rv)
            {
                BMI160_SENSOR_LOG(ERROR, this, "Error decoding registry");
            }
        }
    }
    else
    {
        BMI160_SENSOR_LOG(ERROR, this, "Received unsupported registry event msg id %u",
                event->message_id);
    }
}


/**
 * Registry request send function
 * sensor send the getting registry configuration request to the Framework
 *
 * @param this             the sensor handler
 * @param reg_group_name   the registry group name
 *
 * @return none
 */
static void bmi160_sensor_send_registry_request(sns_sensor *const this,
        char *reg_group_name)
{
    bmi160_state *sstate = (bmi160_state*)this->state->state;
    uint8_t buffer[100];
    int32_t encoded_len;
    sns_memset(buffer, 0, sizeof(buffer));
    sns_rc rc = SNS_RC_SUCCESS;

    sns_registry_read_req read_request;
    pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
        .buf_len = (strlen(reg_group_name) + 1) };

    read_request.name.arg = &data;
    read_request.name.funcs.encode = pb_encode_string_cb;

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
            &read_request, sns_registry_read_req_fields, NULL);
    if (0 < encoded_len)
    {
        sns_request request = (sns_request) {
            .request_len = encoded_len,
                .request = buffer,
                .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ
        };
        rc = sstate->reg_data_stream->api->send_request(sstate->reg_data_stream, &request);

        BMI160_SENSOR_LOG(LOW, this, "req_registry sent %d %d", sstate->sensor, rc);
    }

    //SENSOR_PRINTF_LOW_FULL(this, "Sending registry request for group name:%s", reg_group_name);
}


void bmi160_request_registry(sns_sensor *const this)
{
    bmi160_state *sstate = (bmi160_state*)this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

    BMI160_SENSOR_LOG(LOW, this, "req_registry: %d 0x%x %d", sstate->sensor, sstate->reg_data_stream, sstate->common.who_am_i);

    if ((NULL == sstate->reg_data_stream)
            && (0 == sstate->common.who_am_i))
    {
        sns_sensor_uid suid;

        sns_suid_lookup_get(&sstate->common.suid_lookup_data, "registry", &suid);


        stream_svc->api->create_sensor_stream(stream_svc,
                this, suid, &sstate->reg_data_stream);

        bmi160_sensor_send_registry_request(this, BMI160_REG_NN_PLATFORM_CONFIG);
        bmi160_sensor_send_registry_request(this, BMI160_REG_NN_PLATFORM_PLACEMENT);
        bmi160_sensor_send_registry_request(this, BMI160_REG_NN_PLATFORM_ORIENT);
        bmi160_sensor_send_registry_request(this, BMI160_REG_NN_PLATFORM_MD_CONFIG);

        if (BMI160_ACCEL == sstate->sensor)
        {
            bmi160_sensor_send_registry_request(this, BMI160_REG_NN_ACCEL);
            bmi160_sensor_send_registry_request(
                    this, BMI160_REG_NN_PLATFORM_FAC_CAL_ACCEL);
        }
        else if (BMI160_GYRO == sstate->sensor)
        {
            bmi160_sensor_send_registry_request(this, BMI160_REG_NN_GYRO);
            bmi160_sensor_send_registry_request(
                    this, BMI160_REG_NN_PLATFORM_FAC_CAL_GYRO);
        }
        else if (BMI160_SENSOR_TEMP == sstate->sensor)
        {
            bmi160_sensor_send_registry_request(
                    this, BMI160_REG_NN_TEMP);
            //todo: BMI160_REG_NN_PLATFORM_FAC_CAL_TEMP
        }
        else if (BMI160_MOTION_DETECT == sstate->sensor)
        {
            bmi160_sensor_send_registry_request(
                    this, BMI160_REG_NN_MD);
        }
    }
}


    static bool
bmi160_encode_registry_group_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
        void *const *arg)
{
    pb_arg_reg_group_arg* pb_arg = (pb_arg_reg_group_arg*)*arg;
    bmi160_instance_state *state =
        (bmi160_instance_state*)pb_arg->instance->state->state;

    if (0 == strncmp(pb_arg->name,"bias",strlen("bias")))
    {
        char const *names[] = {"x", "y", "z"};

        for (int i = 0; i < ARR_SIZE(names); i++)
        {
            pb_buffer_arg name_data = (pb_buffer_arg)
            { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg = &name_data;
            pb_item.has_flt = true;
            pb_item.has_version = true;

            if (pb_arg->sensor == BMI160_ACCEL)
            {
                pb_item.flt = state->accel_info.sstate->fac_cal_bias[i] / BMI160_SCALE_FACTOR_DATA_ACCEL;
                pb_item.version = state->accel_info.sstate->fac_cal_version;
            }
            else
            {
                pb_item.flt = state->gyro_info.sstate->fac_cal_bias[i] / BMI160_SCALE_FACTOR_DATA_DFT;
                pb_item.version = state->gyro_info.sstate->fac_cal_version;
            }

            if (!pb_encode_tag_for_field(stream, field))
                return false;

            if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
            {
                BMI160_INST_LOG(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    }
    else if (0 == strncmp(pb_arg->name,"corr_mat",strlen("corr_mat")))
    {
        char const *names[] = {"0_0", "0_1", "0_2",
            "1_0", "1_1", "1_2",
            "2_0", "2_1", "2_2",};

        for (int i = 0; i < ARR_SIZE(names); i++)
        {
            pb_buffer_arg name_data = (pb_buffer_arg)
            { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
            sns_registry_data_item pb_item = sns_registry_data_item_init_default;

            pb_item.name.funcs.encode = &pb_encode_string_cb;
            pb_item.name.arg = &name_data;
            pb_item.has_flt = true;
            pb_item.has_version = true;
            if (pb_arg->sensor == BMI160_ACCEL)
            {
                pb_item.flt = state->accel_info.sstate->fac_cal_corr_mat.data[i];
                pb_item.version = state->accel_info.sstate->fac_cal_version;
            }
            else
            {
                pb_item.flt = state->gyro_info.sstate->fac_cal_corr_mat.data[i];
                pb_item.version = state->gyro_info.sstate->fac_cal_version;
            }

            if (!pb_encode_tag_for_field(stream, field))
                return false;

            if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
            {
                BMI160_INST_LOG(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
                return false;
            }
        }
    }
    return true;
}

    static bool
bmi160_encode_registry_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
        void *const *arg)
{
    pb_arg_reg_group_arg *reg_arg = (pb_arg_reg_group_arg*)*arg;
    sns_sensor_instance *instance = reg_arg->instance;
    char const *names[] = {"bias", "corr_mat"};

    for(int i = 0; i < ARR_SIZE(names); i++)
    {
        pb_buffer_arg name_data = (pb_buffer_arg)
        { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
        sns_registry_data_item pb_item = sns_registry_data_item_init_default;
        pb_arg_reg_group_arg pb_arg= (pb_arg_reg_group_arg){
            .name = NULL,.instance = instance, .sensor = reg_arg->sensor
        };

        pb_item.has_version = true;
        pb_item.version = reg_arg->version;
        pb_item.name.arg = &name_data;
        pb_item.name.funcs.encode = &pb_encode_string_cb;

        if (0==strncmp(names[i],"bias",name_data.buf_len))
        {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &bmi160_encode_registry_group_cb;
            pb_item.subgroup.items.arg = &pb_arg;

        }
        else if (0==strncmp(names[i],"corr_mat",name_data.buf_len))
        {
            pb_arg.name = names[i];
            pb_item.has_subgroup = true;
            pb_item.subgroup.items.funcs.encode = &bmi160_encode_registry_group_cb;
            pb_item.subgroup.items.arg = &pb_arg;
        }
        if (!pb_encode_tag_for_field(stream, field))
        {
            BMI160_INST_LOG(ERROR, instance,"Error encoding corr_mat");

            return false;
        }

        if (!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
        {
            BMI160_INST_LOG(ERROR, instance,"Error encoding sns_registry_data_item_fields");
            return false;
        }
    }

    return true;
}



void bmi160_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, bmi160_sensor_type sensor)
{
    bmi160_state *state = (bmi160_state*)this->state->state;
    pb_arg_reg_group_arg arg = {.instance = instance };

    uint8_t buffer[350];
    int32_t encoded_len;
    char accel_name[] = BMI160_REG_NN_PLATFORM_FAC_CAL_ACCEL;
    char gyro_name[] = BMI160_REG_NN_PLATFORM_FAC_CAL_GYRO;

    bmi160_instance_state *istate = (bmi160_instance_state *)instance->state->state;
    pb_buffer_arg name_data;
    sns_registry_write_req write_req = sns_registry_write_req_init_default;

    if (sensor == BMI160_ACCEL)
    {
        name_data = (pb_buffer_arg)
        { .buf = accel_name, .buf_len = strlen(accel_name) + 1 };
        arg.sensor = BMI160_ACCEL;

        arg.version = istate->accel_info.sstate->fac_cal_version;
    }
    else if (sensor == BMI160_GYRO)
    {
        name_data = (pb_buffer_arg)
        { .buf = gyro_name, .buf_len = strlen(gyro_name) + 1 };
        arg.sensor = BMI160_GYRO;

        arg.version = istate->gyro_info.sstate->fac_cal_version;
    }
    else
    {
        BMI160_SENSOR_LOG(ERROR, this, "Unsupported sensor %d", sensor);
        return;
    }

    write_req.name.funcs.encode = &pb_encode_string_cb;
    write_req.name.arg = &name_data;
    write_req.data.items.funcs.encode = &bmi160_encode_registry_cb;
    write_req.data.items.arg = &arg;

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
            &write_req, sns_registry_write_req_fields, NULL);
    if (0 < encoded_len)
    {
        if (NULL == state->reg_data_stream)
        {
            sns_service_manager *smgr = this->cb->get_service_manager(this);
            sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);

            sns_sensor_uid suid;
            sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);


            stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->reg_data_stream);
        }


        if (NULL != state->reg_data_stream) {
            sns_request request = (sns_request){
                .request_len = encoded_len, .request = buffer,
                    .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
            state->reg_data_stream->api->send_request(state->reg_data_stream, &request);

            BMI160_SENSOR_LOG(MED, this, "req_update_registry for %d len:%d version: %d", sensor, encoded_len, arg.version);
        }
    }

}
#endif

bool bmi160_discover_hw(sns_sensor *const this)
{
    uint8_t buffer[1] = {0};
    bool hw_is_present = false;
    bmi160_state *state = (bmi160_state*)this->state->state;
    sns_rc rv = SNS_RC_SUCCESS;

    state->common.init_flags |= state->sensor;

    BMI160_SENSOR_LOG(MED, this, "dhw comport handle:%p, init flags:0x%x",
            state->common.com_port_info.port_handle,
            state->common.init_flags);
    // CHECK prepare for SPI
    if (state->common.com_port_info.com_config.bus_type == SNS_BUS_SPI)
    {
        bmi160_hal_sensor_prepare_spi_if(this);
    }
    /**-------------------Read and Confirm WHO-AM-I------------------------*/
    buffer[0] = 0x0;
    rv = bmi160_hal_get_who_am_i(state->scp_service, state->common.com_port_info.port_handle, &buffer[0]);
    BMI160_SENSOR_LOG(MED, this, "@sensor:%d get chip id:0x%x", state->sensor, buffer[0]);
    if (rv == SNS_RC_SUCCESS
            &&
            (BMI160_REGV_CHIP_ID_MAJOR == (buffer[0] & BMI160_REGV_CHIP_ID_MAJOR)))
    {
        // Reset Sensor only if an instance is not already running
        if (NULL == sns_sensor_util_get_shared_instance(this))
        {
            /*            rv = bmi160_hal_reset_device(state->scp_service,state->common.com_port_info.port_handle,
                          BMI160_ACCEL | BMI160_GYRO | BMI160_MOTION_DETECT | BMI160_SENSOR_TEMP);*/
        }

        if (rv == SNS_RC_SUCCESS)
        {
            hw_is_present = true;
        }
    }
    state->common.who_am_i = buffer[0];


    /**------------------Power Down and Close COM Port--------------------*/
    state->scp_service->api->sns_scp_update_bus_power(
            state->common.com_port_info.port_handle,
            false);

    state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

#if BMI160_CONFIG_POWER_RAIL
    /**----------------------Turn Power Rail OFF--------------------------*/
    state->common.rail_config.rail_vote = SNS_RAIL_OFF;
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
            this,
            &state->common.rail_config,
            NULL);
#endif

    return hw_is_present;
}





void bmi160_publish_available(sns_sensor * const this)
{
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}




void bmi160_start_hw_detect_sequence(sns_sensor *this)
{
    bmi160_state *sstate = (bmi160_state *) this->state->state;
    sns_rc rv = SNS_RC_SUCCESS;

    //sstate->common.registry_pf_cfg_received = false; copied from LSM6DS3 example code

    /**-----------------Register and Open COM Port-------------------------*/
    if (NULL == sstate->common.com_port_info.port_handle) {
        rv = sstate->scp_service->api->sns_scp_register_com_port(
                &sstate->common.com_port_info.com_config,
                &sstate->common.com_port_info.port_handle);

        if ((SNS_RC_SUCCESS == rv) && (NULL != sstate->common.com_port_info.port_handle)) {
            rv = sstate->scp_service->api->sns_scp_open(sstate->common.com_port_info.port_handle);
        }

        if (NULL != sstate->common.com_port_info.port_handle) {
            if (SNS_BUS_SPI == sstate->common.com_port_info.com_config.bus_type) {
                rv = bmi160_hal_switch_2_spi(sstate->scp_service, sstate->common.com_port_info.port_handle);
            }
        }
    }

#if BMI160_CONFIG_POWER_RAIL
    /**---------------------Register Power Rails --------------------------*/
    if (sns_suid_lookup_get(&sstate->common.suid_lookup_data, "timer", NULL)
            && NULL == sstate->pwr_rail_service
            && rv == SNS_RC_SUCCESS)
    {
        rv = bmi160_register_power_rail(this);

        /**---------------------Turn Power Rails ON----------------------------*/

        sstate->common.rail_config.rail_vote = sstate->common.registry_rail_on_state;

        if (rv == SNS_RC_SUCCESS) {
            sstate->pwr_rail_service->api->sns_vote_power_rail_update(sstate->pwr_rail_service,
                    this,
                    &sstate->common.rail_config,
                    NULL);
        }

        /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
        if (rv == SNS_RC_SUCCESS) {
            bmi160_start_power_rail_timer(this,
                    sns_convert_ns_to_ticks(BMI160_OFF_TO_IDLE_MS * 1000 * 1000),
                    BMI160_POWER_RAIL_PENDING_INIT);
        }
    }
#else
    sstate->common.hw_is_present = bmi160_discover_hw(this);

    if (sstate->common.hw_is_present)
    {
        BMI160_SENSOR_LOG(MED, this, "sensor:%d init finished", sstate->sensor);
        bmi160_publish_available(this);
        bmi160_update_sibling_sensors(this);
    }
    else
    {
        rv = SNS_RC_INVALID_LIBRARY_STATE;
        BMI160_SENSOR_LOG(ERROR, this, "BMI160 HW absent");
    }
#endif



}

void bmi160_update_sibling_sensors(sns_sensor * const this)
{
    sns_sensor *sensor = NULL;
    bmi160_state *sstate;


    bmi160_state *sstate_this = (bmi160_state*) this->state->state;

    for (sensor = this->cb->get_library_sensor(this, true);
            sensor != NULL;
            sensor = this->cb->get_library_sensor(this, false))
    {
        sstate = (bmi160_state*) sensor->state->state;


        if (sstate->sensor != sstate_this->sensor)
        {
            sns_memscpy(&sstate->common, sizeof(sstate->common),
                    &sstate_this->common, sizeof(sstate_this->common));
#if BMI160_CONFIG_POWER_RAIL
            bmi160_register_power_rail(sensor);
#endif
            bmi160_publish_available(sensor);
        }

#if BMI160_CONFIG_ENABLE_REGISTRY
        bmi160_sensor_check_registry_col_progress(sensor);

        BMI160_SENSOR_LOG(LOW, this, "update_siblings: %d %d", sstate->sensor, sstate->registry_cfg_received);
#else
        if (sstate->sensor != sstate_this->sensor) {
            sns_bmi160_registry_def_config(sensor);
        }
#endif
    }
}

/**

 * Sensor common initialization functions
 * @param this   sensor handler
 */
void bmi160_common_init(sns_sensor * const this)
{
    bmi160_state *sstate = (bmi160_state*) this->state->state;
    uint8_t i;

    struct sns_service_manager *smgr = this->cb->get_service_manager(this);
    sstate->diag_service = (sns_diag_service *) smgr->get_service(smgr, SNS_DIAG_SERVICE);
    sstate->scp_service = (sns_sync_com_port_service *) smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
    sstate->island_service = (sns_island_service *) smgr->get_service(smgr, SNS_ISLAND_SERVICE);
#endif

    sstate->sensor_client_present = false;
    sstate->owner = this;

    if ((sstate->sensor == BMI160_ACCEL) ||
            (sstate->sensor == BMI160_GYRO))
    {
        // initialize axis conversion settings
        for (i = 0; i < TRIAXIS_NUM; i++)
        {
            sstate->common.axis_map[i].opaxis = (triaxis) i;
            sstate->common.axis_map[i].ipaxis = (triaxis) i;
            sstate->common.axis_map[i].invert = false;
        }
    }

    // initialize fac cal correction matrix to identity
    sstate->fac_cal_corr_mat.e00 = 1.0;
    sstate->fac_cal_corr_mat.e11 = 1.0;
    sstate->fac_cal_corr_mat.e22 = 1.0;

    SNS_SUID_LOOKUP_INIT(sstate->common.suid_lookup_data, NULL);
    if (BMI160_ACCEL == sstate->sensor) {
#if BMI160_CONFIG_ENABLE_DAE
        sns_suid_lookup_add(this, &sstate->common.suid_lookup_data, "data_acquisition_engine");
#endif
        sns_suid_lookup_add(this, &sstate->common.suid_lookup_data, "interrupt");
        sns_suid_lookup_add(this, &sstate->common.suid_lookup_data, "async_com_port");
        sns_suid_lookup_add(this, &sstate->common.suid_lookup_data, "timer");
    }

#if BMI160_CONFIG_ENABLE_REGISTRY
    sns_suid_lookup_add(this, &sstate->common.suid_lookup_data, "registry");
#endif
}

