/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
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

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_bmg160_sensor.h"
#include "sns_bmg160_sensor_instance.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"
#include "sns_registry.pb.h"


#define BMG160_CONFIG_GYRO             "bmg160_0.gyro.config"
#define BMG160_CONFIG_TEMP             "bmg160_0.temp.config"
#define BMG160_PLATFORM_CONFIG         "bmg160_0_platform.config"
#define BMG160_PLATFORM_PLACEMENT      "bmg160_0_platform.placement"
#define BMG160_PLATFORM_ORIENT         "bmg160_0_platform.orient"
#define BMG160_PLATFORM_FAC_CAL_GYRO   "bmg160_0_platform.gyro.fac_cal"
#define BMG160_PLATFORM_FAC_CAL_TEMP   "bmg160_0_platform.temp.fac_cal"

typedef struct pb_arg_reg_group_arg
{
  const char*          name;
  sns_sensor_instance* instance;
  uint32_t            version;
  bmg160_sensor_type sensor;
}pb_arg_reg_group_arg;


#if BMG160_CONFIG_ENABLE_REGISTRY
static const range_attr bmg160_gyro_ranges[] =
{
  {BMG160_GYRO_RANGE_125_MIN, BMG160_GYRO_RANGE_125_MAX},
  {BMG160_GYRO_RANGE_250_MIN, BMG160_GYRO_RANGE_250_MAX},
  {BMG160_GYRO_RANGE_500_MIN, BMG160_GYRO_RANGE_500_MAX},
  {BMG160_GYRO_RANGE_1000_MIN, BMG160_GYRO_RANGE_1000_MAX},
  {BMG160_GYRO_RANGE_2000_MIN, BMG160_GYRO_RANGE_2000_MAX}
};

static const float bmg160_gyro_resolutions[] =
{
  BMG160_SSTVT_125DPS,
  BMG160_SSTVT_250DPS,
  BMG160_SSTVT_500DPS,
  BMG160_SSTVT_1000DPS,
  BMG160_SSTVT_2000DPS,
};
#endif


// <registry>
#if BMG160_CONFIG_ENABLE_REGISTRY
void bmg160_sensor_process_registry_event(sns_sensor *const this,
                                                  sns_sensor_event *event)
{
  bool rv = true;
  bmg160_state *state = (bmg160_state*)this->state->state;

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_PRINTF(ERROR, this, "Error decoding registry event");
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == strncmp((char*)group_name.buf, BMG160_CONFIG_GYRO,
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, BMG160_CONFIG_TEMP,
                      group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_cfg,
              .parsed_buffer = &state->common.registry_cfg
            }
          };

          read_event.data.items.funcs.decode = sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_cfg_received = true;
          state->is_dri = state->common.registry_cfg.is_dri;
          state->hardware_id = state->common.registry_cfg.hw_id;
          state->resolution_idx = state->common.registry_cfg.res_idx;
          state->supports_sync_stream = state->common.registry_cfg.sync_stream;

          BMG160_PRINTF(LOW, this, "Registry read event for group registry_cfg received "
                     "is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
                     state->is_dri, state->hardware_id, state->resolution_idx,
                     state->supports_sync_stream);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMG160_PLATFORM_CONFIG,
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
              .parsed_buffer = &state->common.registry_pf_cfg
            }
          };

          read_event.data.items.funcs.decode = sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_pf_cfg_received = true;

          state->common.com_port_info.com_config.bus_type = state->common.registry_pf_cfg.bus_type;
          state->common.com_port_info.com_config.bus_instance = state->common.registry_pf_cfg.bus_instance;
          state->common.com_port_info.com_config.slave_control = state->common.registry_pf_cfg.slave_config;
          state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
          state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
          state->common.com_port_info.com_config.reg_addr_type = state->common.registry_pf_cfg.reg_addr_type;
          state->common.irq_config.interrupt_num = state->common.registry_pf_cfg.dri_irq_num;
          state->common.irq_config.interrupt_pull_type = state->common.registry_pf_cfg.irq_pull_type;
          state->common.irq_config.is_chip_pin = state->common.registry_pf_cfg.irq_is_chip_pin;
          state->common.irq_config.interrupt_drive_strength = state->common.registry_pf_cfg.irq_drive_strength;
          state->common.irq_config.interrupt_trigger_type = state->common.registry_pf_cfg.irq_trigger_type;
          state->common.rail_config.num_of_rails = state->common.registry_pf_cfg.num_rail;
          state->common.registry_rail_on_state = state->common.registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->common.rail_config.rails[0].name,
                      state->common.registry_pf_cfg.vddio_rail,
                      sizeof(state->common.rail_config.rails[0].name));
          sns_strlcpy(state->common.rail_config.rails[1].name,
                      state->common.registry_pf_cfg.vdd_rail,
                      sizeof(state->common.rail_config.rails[1].name));

          BMG160_PRINTF(LOW, this, "Registry read event for group registry_pf_cfg received "
                     "bus_type:%d bus_instance:%d slave_control:%d "
                     "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d ",
                     state->common.com_port_info.com_config.bus_type,
                     state->common.com_port_info.com_config.bus_instance,
                     state->common.com_port_info.com_config.slave_control,
                     state->common.com_port_info.com_config.min_bus_speed_KHz,
                     state->common.com_port_info.com_config.max_bus_speed_KHz,
                     state->common.com_port_info.com_config.reg_addr_type);

          BMG160_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d "
                     "interrupt_drive_strength:%d interrupt_trigger_type:%d rigid body type:%d",
                     state->common.irq_config.interrupt_num,
                     state->common.irq_config.interrupt_pull_type,
                     state->common.irq_config.is_chip_pin,
                     state->common.irq_config.interrupt_drive_strength,
                     state->common.irq_config.interrupt_trigger_type,
                     state->common.registry_pf_cfg.rigid_body_type);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMG160_PLATFORM_PLACEMENT,
                           group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->common.placement,
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

          read_event.data.items.funcs.decode = sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_placement_received = true;
          BMG160_PRINTF(LOW, this, "Registry read event for group registry_placement received "
                     "p[0]:%u p[6]:%u p[11]:%u", (uint32_t)state->common.placement[0],
                     (uint32_t)state->common.placement[6], (uint32_t)state->common.placement[11]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMG160_PLATFORM_ORIENT,
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "orient",
              .parse_func = sns_registry_parse_axis_orientation,
              .parsed_buffer = state->common.axis_map
            }
          };

          read_event.data.items.funcs.decode = sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_orient_received = true;

          BMG160_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[0].ipaxis,
                 state->common.axis_map[0].opaxis, state->common.axis_map[0].invert);

          BMG160_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[1].ipaxis, state->common.axis_map[1].opaxis,
                 state->common.axis_map[1].invert);

          BMG160_PRINTF(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[2].ipaxis, state->common.axis_map[2].opaxis,
                 state->common.axis_map[2].invert);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                         BMG160_PLATFORM_FAC_CAL_GYRO,
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                         BMG160_PLATFORM_FAC_CAL_TEMP,
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
            .arr = state->fac_cal_scale,
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
              .parsed_buffer = &state->fac_cal_corr_mat
            }
          };

          read_event.data.items.funcs.decode = sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
          fac_cal_version = arg.version;
        }

        if(rv)
        {
          state->registry_fac_cal_received = true;
          state->fac_cal_version = fac_cal_version;
          BMG160_PRINTF(LOW, this, "fac_cal received: %d, version:%d",
                        state->sensor, state->fac_cal_version);

          uint8_t i;
          for (i = 0; i < TRIAXIS_NUM; i++) {
              state->fac_cal_bias[i] = roundf(fac_cal_bias[i]);
          }

          if(state->fac_cal_scale[0] != 0.0)
          {
            state->fac_cal_corr_mat.e00 = state->fac_cal_scale[0];
            state->fac_cal_corr_mat.e11 = state->fac_cal_scale[1];
            state->fac_cal_corr_mat.e22 = state->fac_cal_scale[2];
          }
        }
      }
      else
      {
        rv = false;
      }

      if(!rv)
      {
        SNS_PRINTF(ERROR, this, "Error decoding registry group");
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u",
               event->message_id);
  }
}

static void bmg160_sensor_send_registry_request(sns_sensor *const this,
                                                 char *reg_group_name)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
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
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    if (rc != SNS_RC_SUCCESS) {
        SNS_PRINTF(ERROR, this, "Request registry data failed.");
    }
  }
}

void bmg160_request_registry(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  // place a request to registry sensor

  if(state->reg_data_stream == NULL
     && state->common.who_am_i == 0)
  {
    sns_sensor_uid suid;

    sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
    stream_svc->api->create_sensor_stream(stream_svc,
        this, suid, &state->reg_data_stream);

    if(BMG160_GYRO == state->sensor)
    {
      bmg160_sensor_send_registry_request(this, BMG160_PLATFORM_CONFIG);
      bmg160_sensor_send_registry_request(this, BMG160_PLATFORM_PLACEMENT);
      bmg160_sensor_send_registry_request(this, BMG160_PLATFORM_ORIENT);
      bmg160_sensor_send_registry_request(this, BMG160_CONFIG_GYRO);
      bmg160_sensor_send_registry_request(
        this, BMG160_PLATFORM_FAC_CAL_GYRO);
    }
    else if(BMG160_SENSOR_TEMP == state->sensor)
    {
      bmg160_sensor_send_registry_request(this, BMG160_CONFIG_TEMP);
    }
  }
}

static bool
bmg160_encode_registry_group_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  pb_arg_reg_group_arg* pb_arg = (pb_arg_reg_group_arg*)*arg;
  bmg160_instance_state *istate =
     (bmg160_instance_state*)pb_arg->instance->state->state;

  if(0 == strncmp(pb_arg->name,"bias",strlen("bias")))
  {
    char const *names[] = {"x", "y", "z"};

    for (uint8_t i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                      { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.has_version = true;

      if(pb_arg->sensor == BMG160_GYRO)
      {
        pb_item.flt = istate->gyro_info.sstate->fac_cal_bias[i];
        pb_item.version = istate->gyro_info.sstate->fac_cal_version;
      }

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
        return false;
      }
    }
  }
  else if (0 == strncmp(pb_arg->name,"corr_mat",strlen("corr_mat")))
  {
    char const *names[] = {"0_0", "0_1", "0_2",
                           "1_0", "1_1", "1_2",
                           "2_0", "2_1", "2_2",};

    for (uint8_t i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                             { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.has_version = true;
      if(pb_arg->sensor == BMG160_GYRO)
      {
        pb_item.flt = istate->gyro_info.sstate->fac_cal_corr_mat.data[i];
        pb_item.version = istate->gyro_info.sstate->fac_cal_version;
      }

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
      {
        SNS_INST_PRINTF(ERROR, pb_arg->instance, "Error encoding sns_registry_data_item_fields");
        return false;
      }
    }
  }
  return true;
}

static bool
bmg160_encode_registry_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
   pb_arg_reg_group_arg *reg_arg = (pb_arg_reg_group_arg*)*arg;
  sns_sensor_instance *instance = reg_arg->instance;
  char const *names[] = {"bias", "corr_mat"};

  for(uint8_t i = 0; i < ARR_SIZE(names); i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;
    pb_arg_reg_group_arg pb_arg= (pb_arg_reg_group_arg){
      .name = NULL,.instance = instance, .sensor = reg_arg->sensor
    };

    pb_item.name.arg = &name_data;
    pb_item.name.funcs.encode = pb_encode_string_cb;

    if(0==strncmp(names[i],"bias",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = bmg160_encode_registry_group_cb;
      pb_item.subgroup.items.arg = &pb_arg;

    }
    else if(0==strncmp(names[i],"corr_mat",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = bmg160_encode_registry_group_cb;
      pb_item.subgroup.items.arg = &pb_arg;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_INST_PRINTF(ERROR, instance,"Error encoding corr_mat");

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_INST_PRINTF(ERROR, instance,"Error encoding sns_registry_data_item_fields");
      return false;
    }
  }

  return true;
}

void bmg160_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, bmg160_sensor_type sensor)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  bmg160_instance_state *istate = (bmg160_instance_state *) instance->state->state;
  pb_arg_reg_group_arg arg = {.instance = instance };

  uint8_t buffer[1000];
  int32_t encoded_len;
  char gyro_name[] = BMG160_PLATFORM_FAC_CAL_GYRO;
  pb_buffer_arg name_data;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  if(sensor == BMG160_GYRO)
  {
    name_data = (pb_buffer_arg)
          { .buf = gyro_name, .buf_len = strlen(gyro_name) + 1 };
    arg.sensor = BMG160_GYRO;
    arg.version = istate->gyro_info.sstate->fac_cal_version;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Unsupported sensor %d", sensor);
    return;
  }

  write_req.name.funcs.encode = pb_encode_string_cb;
  write_req.name.arg = &name_data;
  write_req.data.items.funcs.encode = bmg160_encode_registry_cb;
  write_req.data.items.arg = &arg;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &write_req, sns_registry_write_req_fields, NULL);
  if(0 < encoded_len)
  {
    if(NULL == state->reg_data_stream)
    {
      sns_service_manager *smgr = this->cb->get_service_manager(this);
      sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
      sns_sensor_uid suid;

      sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", &suid);
      stream_svc->api->create_sensor_stream(stream_svc, this, suid,
          &state->reg_data_stream);
    }

    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
    state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
}

void bmg160_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance)
{
  bmg160_state *sensor_state;
  bmg160_instance_state *inst_state = (bmg160_instance_state*)instance->state->state;
  sns_sensor *sensor = NULL;

  for(sensor = this->cb->get_library_sensor(this, true);
      sensor != NULL;
      sensor = this->cb->get_library_sensor(this, false))
  {
    sensor_state = (bmg160_state*)sensor->state->state;

    if(sensor_state->sensor == BMG160_GYRO
            && sensor_state->fac_cal_version < inst_state->gyro_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->gyro_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->gyro_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->gyro_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->gyro_registry_cfg.fac_cal_corr_mat));
    }
    else if(sensor_state->sensor == BMG160_SENSOR_TEMP
            && sensor_state->fac_cal_version < inst_state->sensor_temp_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->sensor_temp_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->sensor_temp_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->sensor_temp_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->sensor_temp_registry_cfg.fac_cal_corr_mat));
    }
  }
}
#endif
// </registry>



bool bmg160_discover_hw(sns_sensor *const this)
{
  uint8_t buffer[1] = {0};
  bool hw_is_present = false;
  bmg160_state *state = (bmg160_state*)this->state->state;
  sns_rc rc = SNS_RC_SUCCESS;

  /**-------------------Read and Confirm WHO-AM-I------------------------*/
  rc = bmg160_get_who_am_i(state->scp_service,state->common.com_port_info.port_handle,
                            &buffer[0]);

  BMG160_PRINTF(MED, this, "WHO AM I: 0x%x, rv:%d", buffer[0], rc);

  if(rc == SNS_RC_SUCCESS
     &&
     buffer[0] == BMG160_WHOAMI_VALUE)
  {
#if 0
    // Reset Sensor only if an inatance is not alreadly running
    if(NULL == sns_sensor_util_get_shared_instance(this))
    {
      rv = bmg160_reset_device(state->scp_service,state->common.com_port_info.port_handle,
          BMG160_GYRO | BMG160_SENSOR_TEMP);
    }
    if(rv == SNS_RC_SUCCESS)
#endif
    {
       hw_is_present = true;
    }
  }

  state->common.who_am_i = buffer[0];

  /**------------------Power Down and Close COM Port--------------------*/
  rc = state->scp_service->api->sns_scp_update_bus_power(
                                              state->common.com_port_info.port_handle,
                                              false);

  state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
  rc |= state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

  /**----------------------Turn Power Rail OFF--------------------------*/
#if BMG160_CONFIG_ENABLE_POWER_RAIL
  state->common.rail_config.rail_vote = SNS_RAIL_OFF;
  rc |= state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                           this,
                                                           &state->common.rail_config,
                                                           NULL);
#endif

  if(rc != SNS_RC_SUCCESS) {
      SNS_PRINTF(ERROR, this, "power down operation failed %d", rc);
  }

  return hw_is_present;
}

void bmg160_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

#if BMG160_CONFIG_ENABLE_POWER_RAIL
static sns_rc bmg160_register_power_rail(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_rc rc = SNS_RC_SUCCESS;

  state->common.rail_config.rail_vote = SNS_RAIL_OFF;

  if(NULL == state->pwr_rail_service)
  {
    state->pwr_rail_service =
     (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);

    rc = state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->common.rail_config);
    if (rc != SNS_RC_SUCCESS) {
        SNS_PRINTF(ERROR, this, "registry power rails fail %d", rc);
    }
  }

  if(NULL == state->pwr_rail_service)
  {
    rc = SNS_RC_FAILED;
  }
  return rc;
}
#endif

// <registry>
#if BMG160_CONFIG_ENABLE_REGISTRY
/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
bmg160_publish_registry_attributes(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  BMG160_PRINTF(MED, this, "registry publish attributes");

  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->is_dri;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->supports_sync_stream;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->hardware_id;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
    for(uint8_t i = 0; i < 12; i++)
    {
      values[i].has_flt = true;
      values[i].flt = state->common.placement[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->common.registry_pf_cfg.rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
  if(state->sensor == BMG160_GYRO)
  {
    /** Only accel and gyro use registry information for selected resolution. */
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_flt = true;
    value.flt = bmg160_gyro_resolutions[state->resolution_idx];
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SELECTED_RESOLUTION, &value, 1, false);
  }
  /** Only accel and gyro use registry information for selected range. */
  if(state->sensor == BMG160_GYRO)
  {
    sns_std_attr_value_data values[] = {SNS_ATTR};
    sns_std_attr_value_data rangeMinMax[] = {SNS_ATTR, SNS_ATTR};
    rangeMinMax[0].has_flt = true;
    rangeMinMax[0].flt = bmg160_gyro_ranges[state->resolution_idx].min;
    rangeMinMax[1].has_flt = true;
    rangeMinMax[1].flt = bmg160_gyro_ranges[state->resolution_idx].max;
    values[0].has_subtype = true;
    values[0].subtype.values.funcs.encode = sns_pb_encode_attr_cb;
    values[0].subtype.values.arg =
      &((pb_buffer_arg){ .buf = rangeMinMax, .buf_len = ARR_SIZE(rangeMinMax) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_SELECTED_RANGE, &values[0], ARR_SIZE(values), true);
  }
}
#endif
// </registry>


void bmg160_validate_resolution(sns_sensor *const this)
{
  bmg160_state *sstate = (bmg160_state *)this->state->state;

  if (sstate->common.registry_cfg_received && sstate->common.registry_placement_received)
  {
    bmg160_publish_registry_attributes(this);
  }

}

void bmg160_update_sibling_sensors(sns_sensor *const this)
{
  sns_sensor *sensor = NULL;
  bmg160_state *state;
  bmg160_state *gyr_state = (bmg160_state*)this->state->state;

  for(sensor = this->cb->get_library_sensor(this, true);
       sensor != NULL;
       sensor = this->cb->get_library_sensor(this, false))
  {
    state = (bmg160_state*)sensor->state->state;

    if(state->sensor != BMG160_GYRO)
    {
      sns_rc rc = SNS_RC_SUCCESS;
      sns_memscpy(&state->common, sizeof(state->common),
                  &gyr_state->common, sizeof(gyr_state->common));
#if BMG160_CONFIG_ENABLE_POWER_RAIL
      rc = bmg160_register_power_rail(sensor);
      if (rc != SNS_RC_SUCCESS) {
        BMG160_PRINTF(ERROR, this, "register power rail error:%d", rc);
      }
#endif
    }

#if BMG160_CONFIG_ENABLE_REGISTRY
    /** Moving registry based attribute publish here. */
    //bmg160_publish_registry_attributes(sensor);
    /** More clean up. */
    sns_sensor_util_remove_sensor_stream(sensor, &state->reg_data_stream);
#endif
  }
}

void bmg160_start_hw_detect_sequence(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;

  state->common.registry_pf_cfg_received = false;

  /**-----------------Register and Open COM Port-------------------------*/
  if(NULL == state->common.com_port_info.port_handle)
  {
    rv = state->scp_service->api->sns_scp_register_com_port(
       &state->common.com_port_info.com_config,
       &state->common.com_port_info.port_handle);

    if(rv == SNS_RC_SUCCESS)
    {
      rv = state->scp_service->api->sns_scp_open(state->common.com_port_info.port_handle);
    }
  }

#if BMG160_CONFIG_ENABLE_POWER_RAIL
  /**---------------------Register Power Rails --------------------------*/
  if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL)
     && NULL == state->pwr_rail_service
     && rv == SNS_RC_SUCCESS)
  {
    rv = bmg160_register_power_rail(this);

    /**---------------------Turn Power Rails ON----------------------------*/
    state->common.rail_config.rail_vote = state->common.registry_rail_on_state;

    if(rv == SNS_RC_SUCCESS)
    {
      rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                               this,
                                                               &state->common.rail_config,
                                                               NULL);
    }

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    if(rv == SNS_RC_SUCCESS)
    {
      bmg160_start_power_rail_timer(this,
                                     sns_convert_ns_to_ticks(BMG160_OFF_TO_IDLE_MS * 1000 * 1000),
                                     BMG160_POWER_RAIL_PENDING_INIT);
    }
  }
#else
  bmg160_state *sstate = (bmg160_state*)this->state->state;
  sstate->common.hw_is_present = bmg160_discover_hw(this);

  if (sstate->common.hw_is_present)
  {
      BMG160_PRINTF(MED, this, "sensor:%d init finished", sstate->sensor);
      bmg160_publish_available(this);
      bmg160_update_sibling_sensors(this);
  }
  else
  {
      rv = SNS_RC_INVALID_LIBRARY_STATE;
      BMG160_PRINTF(ERROR, this, "BMI160 HW absent");
  }
#endif
}

void bmg160_common_init(sns_sensor *const this)
{
  bmg160_state *state = (bmg160_state*)this->state->state;
  uint8_t i = 0;

  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service *)
    smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service =
     (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);

#if BMG160_CONFIG_ENABLE_ISLAND_MODE
  state->island_service =
     (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
#endif

  state->sensor_client_present = false;
  state->owner = this;

  if(state->sensor == BMG160_GYRO)
  {
    // initialize axis conversion settings
    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      state->common.axis_map[i].opaxis = i;
      state->common.axis_map[i].ipaxis = i;
      state->common.axis_map[i].invert = false;
    }
  }

  // initialize fac cal correction matrix to identity
  state->fac_cal_corr_mat.e00 = 1.0;
  state->fac_cal_corr_mat.e11 = 1.0;
  state->fac_cal_corr_mat.e22 = 1.0;

  SNS_SUID_LOOKUP_INIT(state->common.suid_lookup_data, NULL);

  if(state->sensor == BMG160_GYRO)
  {
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "interrupt");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "async_com_port");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "timer");
  }
#if BMG160_CONFIG_ENABLE_REGISTRY
  sns_suid_lookup_add(this, &state->common.suid_lookup_data, "registry");
#endif
}
