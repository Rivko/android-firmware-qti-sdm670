/*******************************************************************************
* Copyright (c) 2019, Bosch Sensortec GmbH
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
#include "sns_printf.h"

#include "sns_bmm150_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_cal.pb.h"
#include "sns_suid.pb.h"
#include "sns_registry.pb.h"
#define BMM150_CONFIG_MAG         "bmm150_0.mag.config"
#define BMM150_CONFIG_TEMP             "bmm150_0.temp.config"
#define BMM150_PLATFORM_CONFIG         "bmm150_0_platform.config"
#define BMM150_PLATFORM_PLACEMENT      "bmm150_0_platform.placement"
#define BMM150_PLATFORM_ORIENT         "bmm150_0_platform.orient"
#define BMM150_PLATFORM_FAC_CAL_MAG  "bmm150_0_platform.mag.fac_cal"

typedef struct pb_arg_reg_group_arg
{
  sns_sensor_instance* instance;
  const char*          name;
  bmm150_sensor_type sensor;
  uint32_t version;
}pb_arg_reg_group_arg;


sns_rc bmm150_get_who_am_i(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint8_t *buffer)
  
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint32_t xfer_bytes;
  uint8_t reg_data = 0;
  uint8_t power_ctrl_addr = BMM150_POWER_CONTROL_ADDR;
  uint8_t chip_id_addr = BMM150_CHIP_ID_ADDR;
  #if BMM_CONFIG_DFT_BUS_SPI
    power_ctrl_addr = (BMM150_POWER_CONTROL_ADDR | 0x80);
    chip_id_addr = (BMM150_CHIP_ID_ADDR | 0x80);
  #endif
  /* Power control register 0x4B is read */
  rv = bmm150_com_read_wrapper(scp_service, port_handle, power_ctrl_addr, &reg_data, 1, &xfer_bytes);
  /* Proceed if everything is fine until now */
  if (rv ==  BMM150_OK)
  {
    /* Sets the value of power control bit */
    reg_data = BMM150_SET_BITS_POS_0(reg_data, BMM150_PWR_CNTRL, BMM150_POWER_CNTRL_ENABLE);
    rv = bmm150_com_write_wrapper(scp_service,port_handle, power_ctrl_addr,
      &reg_data, 1, &xfer_bytes, false);
  }
  /*3ms wait*/
  sns_busy_wait(sns_convert_ns_to_ticks(3*1000*1000));
  rv = bmm150_com_read_wrapper(scp_service,
                        port_handle,
                          chip_id_addr,
                          buffer,
                          (uint32_t) 1,
                          &xfer_bytes);
  if (rv != SNS_RC_SUCCESS || xfer_bytes != 1) {
    rv = SNS_RC_FAILED;
  }
  return rv;
}

/*!
 *  @brief This API is the entry point, Call this API before using other APIs.
 *  This API reads the chip-id of the sensor which is the first step to
 *  verify the sensor and updates the trim parameters of the sensor.
 */
int8_t bmm150_read_trim(sns_sensor * const this)
{
  bmm150_state *state = (bmm150_state *)this->state->state;
  int8_t rslt = 0;
  uint8_t trim_x1y1[2] = {0};
  uint8_t trim_xyz_data[4] = {0};
  uint8_t trim_xy1xy2[10] = {0};
  uint16_t temp_msb = 0;
  uint32_t xfer_bytes = 0;
  uint8_t dig_x1_addr = BMM150_DIG_X1;
  uint8_t dig_z4_addr = BMM150_DIG_Z4_LSB;
  uint8_t dig_z2_addr = BMM150_DIG_Z2_LSB;
#if BMM_CONFIG_DFT_BUS_SPI
  dig_x1_addr = (BMM150_DIG_X1 | 0x80);
  dig_z4_addr = (BMM150_DIG_Z4_LSB | 0x80);
  dig_z2_addr = (BMM150_DIG_Z2_LSB | 0x80);
#endif
  /* Trim register value is read */
  rslt = bmm150_com_read_wrapper(state->scp_service, state->common.com_port_info.port_handle,
    dig_x1_addr, trim_x1y1, 2, &xfer_bytes);
  if (rslt ==  BMM150_OK) {
      rslt = bmm150_com_read_wrapper(state->scp_service, state->common.com_port_info.port_handle,
        dig_z4_addr, trim_xyz_data, 4, &xfer_bytes);
      if (rslt ==  BMM150_OK) {
          rslt = bmm150_com_read_wrapper(state->scp_service, state->common.com_port_info.port_handle,
            dig_z2_addr, trim_xy1xy2, 10, &xfer_bytes);
          if (rslt ==  BMM150_OK) {
              /* Trim data which is read is updated
              in the device structure */
              state->common.trim_data.dig_x1 = (int8_t)trim_x1y1[0];
              state->common.trim_data.dig_y1 = (int8_t)trim_x1y1[1];
              state->common.trim_data.dig_x2 = (int8_t)trim_xyz_data[2];
              state->common.trim_data.dig_y2 = (int8_t)trim_xyz_data[3];
              temp_msb = ((uint16_t)trim_xy1xy2[3]) << 8;
              state->common.trim_data.dig_z1 = (uint16_t)(temp_msb | trim_xy1xy2[2]);
              temp_msb = ((uint16_t)trim_xy1xy2[1]) << 8;
              state->common.trim_data.dig_z2 = (int16_t)(temp_msb | trim_xy1xy2[0]);
              temp_msb = ((uint16_t)trim_xy1xy2[7]) << 8;
              state->common.trim_data.dig_z3 = (int16_t)(temp_msb | trim_xy1xy2[6]);
              temp_msb = ((uint16_t)trim_xyz_data[1]) << 8;
              state->common.trim_data.dig_z4 = (int16_t)(temp_msb | trim_xyz_data[0]);
              state->common.trim_data.dig_xy1 = trim_xy1xy2[9];
              state->common.trim_data.dig_xy2 = (int8_t)trim_xy1xy2[8];
              temp_msb = ((uint16_t)(trim_xy1xy2[5] & 0x7F)) << 8;
              state->common.trim_data.dig_xyz1 = (uint16_t)(temp_msb | trim_xy1xy2[4]);
          }
      }
  }
  BMM_SENSOR_LOG(LOW, this, "trim_data x1 = 0x%x, y1 = 0x%x",
  state->common.trim_data.dig_x1, state->common.trim_data.dig_y1);
  BMM_SENSOR_LOG(LOW, this, "trim_data x2 = 0x%x, y2 = 0x%x",
  state->common.trim_data.dig_x2, state->common.trim_data.dig_y2);
  BMM_SENSOR_LOG(LOW, this, "trim_data Z1 = 0x%x, Z2 = 0x%x Z3 = 0x%x, Z4 = 0x%x",
  state->common.trim_data.dig_z1, state->common.trim_data.dig_z2, state->common.trim_data.dig_z3, state->common.trim_data.dig_z4);
  BMM_SENSOR_LOG(LOW, this, "trim_data xy1 = 0x%x, xy2 = 0x%x xyz1 = 0x%x",
  state->common.trim_data.dig_xy1, state->common.trim_data.dig_xy2, state->common.trim_data.dig_xyz1);
  return rslt;
}

#if BMM_CONFIG_ENABLE_REGISTRY
static void bmm150_sensor_process_registry_event(sns_sensor *const this,
                                           sns_sensor_event *event)
{
  bool rv = true;
  bmm150_state *state = (bmm150_state*)this->state->state;

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);
  BMM_SENSOR_LOG(LOW, this, ">>bmm150_sensor_process_registry_event = %d<<", event->message_id);

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

      if(0 == strncmp((char*)group_name.buf, BMM150_CONFIG_MAG,
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, BMM150_CONFIG_TEMP,
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

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
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

          BMM_SENSOR_LOG(LOW, this, "Registry read event for group registry_cfg received "
                     "is_dri:%d, hardware_id:%d resolution_idx:%d, supports_sync_stream:%d",
                     state->is_dri, state->hardware_id, state->resolution_idx,
                     state->supports_sync_stream);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMM150_PLATFORM_CONFIG,
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

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_pf_cfg_received = true;

          state->common.com_port_info.com_config.bus_type = (sns_bus_type)state->common.registry_pf_cfg.bus_type;
          state->common.com_port_info.com_config.bus_instance = state->common.registry_pf_cfg.bus_instance;
          state->common.com_port_info.com_config.slave_control = state->common.registry_pf_cfg.slave_config;
          state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
          state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
          state->common.com_port_info.com_config.reg_addr_type = (sns_reg_addr_type)state->common.registry_pf_cfg.reg_addr_type;
          state->common.rail_config.num_of_rails = state->common.registry_pf_cfg.num_rail;
          state->common.registry_rail_on_state = (sns_power_rail_state)state->common.registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->common.rail_config.rails[0].name,
                      state->common.registry_pf_cfg.vddio_rail,
                      sizeof(state->common.rail_config.rails[0].name));
          sns_strlcpy(state->common.rail_config.rails[1].name,
                      state->common.registry_pf_cfg.vdd_rail,
                      sizeof(state->common.rail_config.rails[1].name));

          BMM_SENSOR_LOG(LOW, this, "bus_type:%d bus_instance:%d slave_control:%d min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d ",
                     (uint8_t)state->common.com_port_info.com_config.bus_type,
                     state->common.com_port_info.com_config.bus_instance,
                     state->common.com_port_info.com_config.slave_control,
                     state->common.com_port_info.com_config.min_bus_speed_KHz,
                     state->common.com_port_info.com_config.max_bus_speed_KHz,
                     (uint8_t)state->common.com_port_info.com_config.reg_addr_type);
          BMM_SENSOR_LOG(LOW, this, "rigid body type:%d",
                     state->common.registry_pf_cfg.rigid_body_type);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMM150_PLATFORM_PLACEMENT,
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

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_placement_received = true;
          BMM_SENSOR_LOG(LOW, this, "Registry read event for group registry_placement received "
                     "p[0]:%u p[6]:%u p[11]:%u", (uint32_t)state->common.placement[0],
                     (uint32_t)state->common.placement[6], (uint32_t)state->common.placement[11]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, BMM150_PLATFORM_ORIENT,
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

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->common.registry_orient_received = true;

          BMM_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[0].ipaxis,
                 state->common.axis_map[0].opaxis, state->common.axis_map[0].invert);

          BMM_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[1].ipaxis, state->common.axis_map[1].opaxis,
                 state->common.axis_map[1].invert);

          BMM_SENSOR_LOG(LOW, this, "Input Axis:%d maps to Output Axis:%d with inversion %d",
                 state->common.axis_map[2].ipaxis, state->common.axis_map[2].opaxis,
                 state->common.axis_map[2].invert);
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           BMM150_PLATFORM_FAC_CAL_MAG,
                           group_name.buf_len))
      {
        uint32_t fac_cal_version;
        {
          uint8_t bias_arr_index = 0, scale_arr_index = 0;
          pb_float_arr_arg bias_arr_arg = {
            .arr = state->fac_cal_bias,
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

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
          fac_cal_version = arg.version;
        }

        if(rv)
        {
          state->registry_fac_cal_received = true;
          state->fac_cal_version = fac_cal_version;
          if(state->fac_cal_scale[0] != 0.0f)
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
#endif

#if BMM_CONFIG_ENABLE_REGISTRY
static void bmm150_sensor_send_registry_request(sns_sensor *const this,
                                                 char *reg_group_name)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
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
    if (rc != SNS_RC_SUCCESS)
    {
      SNS_PRINTF(ERROR, this, "bmm150 send registry request failed");
    }
  }
}

void bmm150_request_registry(sns_sensor *const this)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
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

    if(BMM150_MAG == state->sensor)
    {
      bmm150_sensor_send_registry_request(this, BMM150_PLATFORM_CONFIG);
      bmm150_sensor_send_registry_request(this, BMM150_PLATFORM_PLACEMENT);
      bmm150_sensor_send_registry_request(this, BMM150_PLATFORM_ORIENT);
      bmm150_sensor_send_registry_request(this, BMM150_CONFIG_MAG);
      bmm150_sensor_send_registry_request(
        this, BMM150_PLATFORM_FAC_CAL_MAG);
    }
    else if(BMM150_TEMPERATURE == state->sensor)
    {
      bmm150_sensor_send_registry_request(this, BMM150_CONFIG_TEMP);
    }
  }
}

static bool
bmm150_encode_registry_group_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  pb_arg_reg_group_arg* pb_arg = (pb_arg_reg_group_arg*)*arg;
  bmm150_instance_state *state =
     (bmm150_instance_state*)pb_arg->instance->state->state;

  if(0 == strncmp(pb_arg->name,"bias",strlen("bias")))
  {
    char const *names[] = {"x", "y", "z"};

    for (uint8_t i = 0; i < ARR_SIZE(names); i++)
    {
      pb_buffer_arg name_data = (pb_buffer_arg)
                      { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
      sns_registry_data_item pb_item = sns_registry_data_item_init_default;

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.has_version = true;

      if(pb_arg->sensor == BMM150_MAG)
      {
        pb_item.flt = state->mag_registry_cfg.fac_cal_bias[i];
        pb_item.version = state->mag_registry_cfg.version;
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

      pb_item.name.funcs.encode = &pb_encode_string_cb;
      pb_item.name.arg = &name_data;
      pb_item.has_flt = true;
      pb_item.has_version = true;
      if(pb_arg->sensor == BMM150_MAG)
      {
        pb_item.flt = state->mag_registry_cfg.fac_cal_corr_mat.data[i];
        pb_item.version = state->mag_registry_cfg.version;
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
bmm150_encode_registry_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
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

    pb_item.has_version = true;
    pb_item.version = reg_arg->version;
    pb_item.name.arg = &name_data;
    pb_item.name.funcs.encode = &pb_encode_string_cb;

    if(0==strncmp(names[i],"bias",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &bmm150_encode_registry_group_cb;
      pb_item.subgroup.items.arg = &pb_arg;

    }
    else if(0==strncmp(names[i],"corr_mat",name_data.buf_len))
    {
      pb_arg.name = names[i];
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &bmm150_encode_registry_group_cb;
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

void bmm150_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, bmm150_sensor_type sensor)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
  pb_arg_reg_group_arg arg = {.instance = instance };

  bmm150_instance_state *inst_state =
            (bmm150_instance_state*)instance->state->state;
  uint8_t buffer[1000];
  int32_t encoded_len;
  char mag_name[] = BMM150_PLATFORM_FAC_CAL_MAG;

  pb_buffer_arg name_data;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  if(sensor == BMM150_MAG)
  {
    name_data = (pb_buffer_arg)
          { .buf = mag_name, .buf_len = strlen(mag_name) + 1 };
    arg.sensor = BMM150_MAG;
    arg.version = inst_state->mag_registry_cfg.version;
  }

  else
  {
    SNS_PRINTF(ERROR, this, "Unsupported sensor %d", sensor);
    return;
  }

  write_req.name.funcs.encode = &pb_encode_string_cb;
  write_req.name.arg = &name_data;
  write_req.data.items.funcs.encode = &bmm150_encode_registry_cb;
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

void bmm150_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance)
{
  bmm150_state *sensor_state;
  bmm150_instance_state *inst_state = (bmm150_instance_state*)instance->state->state;
  sns_sensor *sensor = NULL;

  for(sensor = this->cb->get_library_sensor(this, true);
      sensor != NULL;
      sensor = this->cb->get_library_sensor(this, false))
  {
    sensor_state = (bmm150_state*)sensor->state->state;

    if(sensor_state->sensor == BMM150_MAG
       && sensor_state->fac_cal_version < inst_state->mag_registry_cfg.version)
    {
      sns_memscpy(&sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias),
                  &inst_state->mag_registry_cfg.fac_cal_bias[0],
                  sizeof(inst_state->mag_registry_cfg.fac_cal_bias));

      sns_memscpy(&sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat),
                  &inst_state->mag_registry_cfg.fac_cal_corr_mat,
                  sizeof(inst_state->mag_registry_cfg.fac_cal_corr_mat));
      sensor_state->fac_cal_version = inst_state->mag_registry_cfg.version;
    }

  }
}
#endif
bool bmm150_discover_hw(sns_sensor *const this)
{
  uint8_t buffer[1] = {0};
  bool hw_is_present = false;
  bmm150_state *state = (bmm150_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;

  /**-------------------Read and Confirm WHO-AM-I------------------------*/
  rv = bmm150_get_who_am_i(state->scp_service,state->common.com_port_info.port_handle,
                            &buffer[0]);
  BMM_SENSOR_LOG(LOW, this, "bmm150 who am i is 0x%x",buffer[0]);
  if(rv == SNS_RC_SUCCESS
     &&
     buffer[0] == BMM150_WHOAMI_VALUE)
  {
    // Reset Sensor only if an inatance is not alreadly running
    if(NULL == sns_sensor_util_get_shared_instance(this))
    {
      rv = (sns_rc)bmm150_read_trim(this);
      if (rv != SNS_RC_SUCCESS)
      {
        SNS_PRINTF(ERROR, this, "read trim failed");
      }
    }
    if(rv == SNS_RC_SUCCESS)
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
#if BMM_CONFIG_POWER_RAIL

  /**----------------------Turn Power Rail OFF--------------------------*/
  state->common.rail_config.rail_vote = SNS_RAIL_OFF;
  state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                           this,
                                                           &state->common.rail_config,
                                                           NULL);
#endif

  return hw_is_present;
}

void bmm150_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}
#if BMM_CONFIG_POWER_RAIL
static sns_rc bmm150_register_power_rail(sns_sensor *const this)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_rc rv = SNS_RC_SUCCESS;

  state->common.rail_config.rail_vote = SNS_RAIL_OFF;

  if(NULL == state->pwr_rail_service)
  {
    state->pwr_rail_service =
     (sns_pwr_rail_service*)smgr->get_service(smgr, SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->common.rail_config);
  }

  if(NULL == state->pwr_rail_service)
  {
    rv = SNS_RC_FAILED;
  }
  return rv;
}
#endif
/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
bmm150_publish_registry_attributes(sns_sensor *const this)
{
#if !BMM_CONFIG_ENABLE_SEE_LITE
  bmm150_state *state = (bmm150_state*)this->state->state;
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
  #else
  UNUSED_VAR(this);
  #endif
}
void bmm150_update_sibling_sensors(sns_sensor *const this)
{
  sns_sensor *sensor = NULL;
  bmm150_state *state;
  bmm150_state *acc_state = (bmm150_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;

  for(sensor = this->cb->get_library_sensor(this, true);
       sensor != NULL;
       sensor = this->cb->get_library_sensor(this, false))
  {
    state = (bmm150_state*)sensor->state->state;

    if(state->sensor != BMM150_MAG)
    {
      sns_memscpy(&state->common, sizeof(state->common),
                  &acc_state->common, sizeof(acc_state->common));
      #if BMM_CONFIG_POWER_RAIL
      rv = bmm150_register_power_rail(sensor);
      if (rv != SNS_RC_SUCCESS)
      {
        SNS_PRINTF(ERROR, this, "bmm150_register_power_rail failed");
      }
      #endif
      bmm150_publish_available(sensor);
    }

    /** Moving registry based attribute publish here. */
    bmm150_publish_registry_attributes(sensor);
#if BMM_CONFIG_ENABLE_REGISTRY
    /** More clean up. */
    sns_sensor_util_remove_sensor_stream(sensor, &state->reg_data_stream);
#endif
  }
}

#if BMM_CONFIG_POWER_RAIL
static void bmm150_start_power_rail_timer(sns_sensor * const this,
    sns_time timeout_ticks,
    bmm150_power_rail_pending_state pwr_rail_pend_state)
{
  bmm150_state *state = (bmm150_state*)this->state->state;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];
  BMM_SENSOR_LOG(LOW, this, "start power rail timer");
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  if(NULL == state->timer_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
    sns_sensor_uid suid;

    sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", &suid);
    stream_svc->api->create_sensor_stream(stream_svc, this, suid, &state->timer_stream);
  }

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if(req_len > 0 && NULL != state->timer_stream)
  {
    sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
         .request = buffer, .request_len = req_len  };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "LSM timer req encode error");
  }
  BMM_SENSOR_LOG(LOW, this, "start power rail timer request sent");
}
#endif
void bmm150_start_hw_detect_sequence(sns_sensor *const this)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
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
#if BMM_CONFIG_POWER_RAIL
  /**---------------------Register Power Rails --------------------------*/
  if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL)
     && NULL == state->pwr_rail_service
     && rv == SNS_RC_SUCCESS)
  {
    rv = bmm150_register_power_rail(this);

    /**---------------------Turn Power Rails ON----------------------------*/
    state->common.rail_config.rail_vote = state->common.registry_rail_on_state;

    if(rv == SNS_RC_SUCCESS
       && NULL != state->pwr_rail_service)
    {
      rv = state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                               this,
                                                               &state->common.rail_config,
                                                               NULL);
    }

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    if(rv == SNS_RC_SUCCESS)
    {
      bmm150_start_power_rail_timer(this,
                                     sns_convert_ns_to_ticks(BME_OFF_TO_IDLE_MS * 1000 * 1000),
                                     BME_POWER_RAIL_PENDING_INIT);
    }
  }
#else

  state->common.hw_is_present = bmm150_discover_hw(this);

  if (state->common.hw_is_present)
  {
    BMM_SENSOR_LOG(MED, this, "sensor:%d init finished in disable pwr_rail_service", state->sensor);
    bmm150_publish_available(this);
    bmm150_update_sibling_sensors(this);
  }
  else
  {
    rv = SNS_RC_INVALID_LIBRARY_STATE;
    SNS_PRINTF(ERROR, this, "BMM150 HW absent");
  }
#endif
}

void bmm150_common_init(sns_sensor *const this)
{
  bmm150_state *state = (bmm150_state*)this->state->state;
  uint8_t i = 0;

  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service *)
    smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->scp_service =
     (sns_sync_com_port_service *)smgr->get_service(smgr, SNS_SYNC_COM_PORT_SERVICE);
  #if BMM_CONFIG_ENABLE_ISLAND_MODE
  state->island_service =
     (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
  #endif
  state->sensor_client_present = false;
  if(state->sensor == BMM150_MAG)
  {
    // initialize axis conversion settings
    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      state->common.axis_map[i].opaxis = (triaxis)i;
      state->common.axis_map[i].ipaxis = (triaxis)i;
      state->common.axis_map[i].invert = false;
    }
  }

  // initialize fac cal correction matrix to identity
  state->fac_cal_corr_mat.e00 = 1.0f;
  state->fac_cal_corr_mat.e11 = 1.0f;
  state->fac_cal_corr_mat.e22 = 1.0f;

  SNS_SUID_LOOKUP_INIT(state->common.suid_lookup_data, NULL);
  /** Accel sensor fetches all common dependent sensor SUIDs. */
  if(state->sensor == BMM150_MAG)
  {
  #if BMM_CONFIG_ENABLE_DAE
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "data_acquisition_engine");
  #endif
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "async_com_port");
    sns_suid_lookup_add(this, &state->common.suid_lookup_data, "timer");
  }
  #if BMM_CONFIG_ENABLE_REGISTRY
  sns_suid_lookup_add(this, &state->common.suid_lookup_data, "registry");
  #endif
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void bmm150_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{

  sns_request config;
  BMM_SENSOR_LOG(LOW, this, "bmm150_set_self_test_inst_config");
  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

/** See sns_bmm150_sensor.h*/
sns_rc bmm150_sensor_notify_event(sns_sensor * const this)
{
  bmm150_state *state = (bmm150_state*) this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;
  BMM_SENSOR_LOG(LOW, this, "<sns_see_if__ notify_event> from sensor:%d ", state->sensor);
  if(!sns_suid_lookup_complete(&state->common.suid_lookup_data))
  {
  #if BMM_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_island_exit(state->island_service, this);
  #endif
    sns_suid_lookup_handle(this, &state->common.suid_lookup_data);

    #if BMM_CONFIG_ENABLE_REGISTRY
    if(sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", NULL))
    {
      bmm150_request_registry(this);
    }

    #else
      sns_bmm_registry_def_config(this);
    #endif

    if(sns_suid_lookup_complete(&state->common.suid_lookup_data))
    {
      sns_suid_lookup_deinit(this, &state->common.suid_lookup_data);
    }
  }

  /**----------------------Handle a Timer Sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    event = state->timer_stream->api->peek_input(state->timer_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;
      BMM_SENSOR_LOG(LOW, this, ">>timer event<< message_id = %d", event->message_id);
      if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == BME_POWER_RAIL_PENDING_INIT)
          {
            /** Initial HW discovery is OK to run in normal mode. */
            #if BMM_CONFIG_ENABLE_ISLAND_MODE
            state->island_service->api->sensor_island_exit(state->island_service, this);
            #endif
            state->common.hw_is_present = bmm150_discover_hw(this);

            if(state->common.hw_is_present)
            {
              bmm150_publish_available(this);
              bmm150_update_sibling_sensors(this);
              BMM_SENSOR_LOG(LOW, this, "sensor:%d initialize finished", state->sensor);
            }
            else
            {
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              SNS_PRINTF(ERROR, this, "BMM150 HW absent");
            }
            state->power_rail_pend_state = BME_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == BME_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            if(NULL != instance)
            {
              bmm150_instance_state *inst_state =
                 (bmm150_instance_state *) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              bmm150_reval_instance_config(this, instance, state->sensor);
              if(inst_state->new_self_test_request)
              {
                bmm150_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = BME_POWER_RAIL_PENDING_NONE;
          }
        }
        else
        {
          SNS_PRINTF(ERROR, this, "pb_decode error");
        }
      }
      event = state->timer_stream->api->get_next_input(state->timer_stream);
    }
    /** Free up timer stream if not needed anymore */
    if(state->power_rail_pend_state == BME_POWER_RAIL_PENDING_NONE)
    {
      sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
    }
  }
#if BMM_CONFIG_ENABLE_REGISTRY
  if(NULL != state->reg_data_stream)
  {
    BMM_SENSOR_LOG(LOW, this, ">>reg_data_stream<<");
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
      /** All registry events can be handled in normal mode. */
      #if BMM_CONFIG_ENABLE_ISLAND_MODE
      state->island_service->api->sensor_island_exit(state->island_service, this);
      #endif
      bmm150_sensor_process_registry_event(this, event);

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }
#endif
  if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL) &&
     state->common.registry_pf_cfg_received && state->common.registry_cfg_received &&
     state->common.registry_orient_received && state->common.registry_placement_received)
  {
    /** Initial HW detection sequence is OK to run in normal mode. */
    #if BMM_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_island_exit(state->island_service, this);
    #endif
    bmm150_start_hw_detect_sequence(this);
  }
  return rv;
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool bmm150_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
                                                  sns_std_request *decoded_request,
                                                  sns_physical_sensor_test_config *test_config)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = test_config,
        .fields = sns_physical_sensor_test_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(request->request,
                                  request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "LSM decode error");
    return false;
  }
  return true;
}

static void bmm150_set_sensor_inst_config(sns_sensor *this,
    sns_sensor_instance *instance, float chosen_report_rate,
    float chosen_sample_rate,
    bmm150_sensor_type  sensor_type,
    sns_bmm150_registry_cfg registry_cfg,
    bool sample_rate_no_need_change)
{
  UNUSED_VAR(instance);
  sns_bmm150_cfg_req new_client_config;
  sns_request config;

  new_client_config.report_rate = chosen_report_rate;
  new_client_config.sample_rate = chosen_sample_rate;
  new_client_config.sensor_type = sensor_type;
  new_client_config.op_mode = FORCED_MODE;
  new_client_config.sample_rate_no_not_change = sample_rate_no_need_change;
  new_client_config.registry_cfg = registry_cfg;
  BMM_SENSOR_LOG(LOW, this, "sensor type:%d, sample rate %d no_change %d",
  sensor_type, (uint8_t)chosen_sample_rate, sample_rate_no_need_change);

  config.message_id  = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
  config.request_len = sizeof(sns_bmm150_cfg_req);
  config.request     = &new_client_config;
  this->instance_api->set_client_config(instance, &config);
}
#if BMM_CONFIG_POWER_RAIL
/**
 * Turns power rails off
 *
 * @paramp[i] this   Sensor reference
 *
 * @return none
 */
static void bmm150_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    bmm150_state *sensor_state = (bmm150_state*)sensor->state->state;
    if(sensor_state->common.rail_config.rail_vote != SNS_RAIL_OFF)
    {
      sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
      sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                                                                      sensor,
                                                                      &sensor_state->common.rail_config,
                                                                      NULL);
    }
  }
}
#endif
static bool bmm150_get_decoded_sensor_request(
  sns_sensor const *this,
  sns_request const *in_request,
  sns_std_request *decoded_request,
  sns_std_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  /* decode argument */
  pb_simple_cb_arg arg = {
      .decoded_struct = decoded_payload,
      .fields = sns_std_sensor_config_event_fields
  };
  /* decode functions.decode */
  decoded_request->payload = (struct pb_callback_s ) {
      .funcs.decode = &pb_decode_simple_cb,
      .arg = &arg
  };
  stream = pb_istream_from_buffer(in_request->request, in_request->request_len);
  if (!pb_decode(&stream, sns_std_request_fields, decoded_request)) {
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
static bool bmm150_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  bmm150_state *state = (bmm150_state*)this->state->state;
  bmm150_instance_state *inst_state = (bmm150_instance_state*)instance->state->state;
  bmm150_self_test_info *self_test_info;

  if(state->sensor == BMM150_MAG)
  {
    self_test_info = &inst_state->mag_info.test_info;
  }
  else if(state->sensor == BMM150_TEMPERATURE)
  {
    self_test_info = &inst_state->temperature_info.test_info;
  }
  else
  {
    return false;
  }

  if(bmm150_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
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

/* get the configuration item for tempture sensor */
static void bmm150_get_sensor_tempture_config(sns_sensor *this,
  sns_sensor_instance *instance,
  bmm150_sensor_type sensor_type,
  float *chosen_sample_rate,
  float *chosen_report_rate,
  bool *sensor_tempture_client_present)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_type);
  bmm150_instance_state *inst_state =
     (bmm150_instance_state*)instance->state->state;
  sns_sensor_uid suid = TEMPERATURE_SUID;
  sns_request const *request;

  *chosen_report_rate = 0;
  *chosen_sample_rate = 0;
  *sensor_tempture_client_present = false;

  /** Parse through existing requests and get fastest sample
   *    rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
    NULL != request;
    request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};
    BMM_SENSOR_LOG(LOW, this, "temperature message id %d", request->message_id);
    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(bmm150_get_decoded_sensor_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                       decoded_payload.sample_rate);
        if(decoded_request.has_batching
         &&
         decoded_request.batching.batch_period > 0)
        {
          report_rate = (1000000.0f / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = *chosen_sample_rate;
        }
        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
        *sensor_tempture_client_present = true;
      }
    }
  }
  inst_state->temperature_info.report_timer_hz  = *chosen_report_rate;
  inst_state->temperature_info.sampling_rate_hz= *chosen_sample_rate;
  BMM_SENSOR_LOG(LOW, this, "temperature sample rete %d temperature present %d",
  (uint8_t)inst_state->temperature_info.sampling_rate_hz,
  *sensor_tempture_client_present);
}

static void bmm150_get_sensor_mag_config(
  sns_sensor *this,
  sns_sensor_instance *instance,
  bmm150_sensor_type sensor_type,
  float *chosen_sample_rate,
  float *chosen_report_rate,
  bool *sensor_mag_client_present)
{
  UNUSED_VAR(this);
  UNUSED_VAR(sensor_type);
  bmm150_instance_state *inst_state =
   (bmm150_instance_state*)instance->state->state;
  sns_sensor_uid suid = MAG_SUID;
  sns_request const *request;

  *chosen_report_rate = 0;
  *chosen_sample_rate = 0;
  *sensor_mag_client_present = false;

    /** Parse through existing requests and get fastest sample
     *    rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
     NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(bmm150_get_decoded_sensor_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                decoded_payload.sample_rate);
        if(decoded_request.has_batching
         &&
         decoded_request.batching.batch_period > 0)
        {
          report_rate = (1000000.0f / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = *chosen_sample_rate;
        }
        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
        *sensor_mag_client_present = true;
      }
    }
  }
  inst_state->mag_info.report_timer_hz  = *chosen_report_rate;
  inst_state->mag_info.sampling_rate_hz= *chosen_sample_rate;
  BMM_SENSOR_LOG(LOW, this, "mag sample rete %d mag present %d",
  (uint8_t)inst_state->mag_info.sampling_rate_hz,
  *sensor_mag_client_present);
}


static void  bmm150_mark_sensor_enable_state (
    sns_sensor_instance *this,
    bmm150_sensor_type sensor_type,
    bool enable)
{
  bmm150_instance_state *inst_state = (bmm150_instance_state *) this->state->state;
    /* mark the corresponding sensor as fifo info field *now only the sw fifo* */
  switch (sensor_type) {
  case BMM150_MAG:
    if(enable)
    {
      inst_state->deploy_info.publish_sensors |= BMM150_MAG;
      inst_state->deploy_info.enable |= BMM150_MAG;
    }
    else
    {
      inst_state->deploy_info.publish_sensors &= ~BMM150_MAG;
      inst_state->deploy_info.enable &= ~BMM150_MAG;
    }
    break;
  case BMM150_TEMPERATURE:
    if(enable)
    {
      inst_state->deploy_info.publish_sensors |= BMM150_TEMPERATURE;
      inst_state->deploy_info.enable |= BMM150_TEMPERATURE;
    }
    else
    {
      inst_state->deploy_info.publish_sensors &= ~BMM150_TEMPERATURE;
      inst_state->deploy_info.enable &= ~BMM150_TEMPERATURE;
    }
    break;
  default:
    break;
  }
}

/**
 * re-evaluate the instance configuration
 */
void bmm150_reval_instance_config(sns_sensor *this,
    sns_sensor_instance *instance,
    bmm150_sensor_type sensor_type)
{
  bmm150_instance_state *inst_state = (bmm150_instance_state*)instance->state->state;
  bmm150_state *sensor_state = (bmm150_state*)this->state->state;
  sns_bmm150_registry_cfg registry_cfg = {.sensor_type = sensor_type};
  /**
   * 1. Get best mag Config.
   * 2. Get best temperature Config.
   * 5. Decide best Instance Config based on above outputs.
   */
  float p_sample_rate = 0;
  float p_report_rate = 0;
  float t_sample_rate = 0;
  float t_report_rate = 0;

  float chosen_sample_rate = 0;
  float chosen_report_rate = 0;

  bool p_sensor_client_present = false;
  bool t_sensor_client_present = false;
  bool sample_rate_no_need_change = false;
  BMM_SENSOR_LOG(LOW, this, "sensor type: %d %d", sensor_state->sensor, sensor_type);

  if(sensor_type == BMM150_MAG)
  {
    bmm150_get_sensor_mag_config(this, instance, sensor_state->sensor,
      &p_sample_rate, &p_report_rate, &p_sensor_client_present);
    chosen_sample_rate = p_sample_rate;
    chosen_report_rate = p_report_rate;
  }
  if(sensor_type == BMM150_TEMPERATURE)
  {
    bmm150_get_sensor_tempture_config(this, instance, sensor_state->sensor,
    &t_sample_rate, &t_report_rate, &t_sensor_client_present);
    chosen_sample_rate = t_sample_rate;
    chosen_report_rate = t_report_rate;
  }

  if (sensor_type == BMM150_MAG || sensor_type == BMM150_TEMPERATURE)
  {
    if (sensor_type == BMM150_MAG)
      bmm150_mark_sensor_enable_state(
      instance, BMM150_MAG, p_sensor_client_present);
    if (sensor_type == BMM150_TEMPERATURE)
      bmm150_mark_sensor_enable_state(
      instance, BMM150_TEMPERATURE, t_sensor_client_present);
    BMM_SENSOR_LOG(LOW, this,
      "sensor type:%d, enable sensor flag:0x%x publish sensor flag:0x%x",
      sensor_type, inst_state->deploy_info.enable,
      inst_state->deploy_info.publish_sensors);
    /* set the sensor instance configuration*/
    sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
      sensor_state->fac_cal_bias, sizeof(sensor_state->fac_cal_bias));
    sns_memscpy(&registry_cfg.fac_cal_corr_mat, sizeof(registry_cfg.fac_cal_corr_mat),
      &sensor_state->fac_cal_corr_mat, sizeof(sensor_state->fac_cal_corr_mat));
    registry_cfg.version = sensor_state->fac_cal_version;
    if (((chosen_sample_rate == 10.0f) && (sensor_state->chosen_sample_rate_old == 5.0f)) ||
    (chosen_sample_rate == sensor_state->chosen_sample_rate_old))
    {
      sample_rate_no_need_change = true;
      BMM_SENSOR_LOG(LOW, this,
      "sensor type:%d, sample_rate_no_need_change %d",
      sensor_type, sample_rate_no_need_change);
    }
    
    BMM_SENSOR_LOG(LOW, this,
    "sensor type:%d, sensor_state->chosen_sample_rate_old %d chosen_sample_rate:%d",
    sensor_type, (uint32_t)sensor_state->chosen_sample_rate_old,
      (uint32_t)chosen_sample_rate);
    sensor_state->chosen_sample_rate_old = chosen_sample_rate;
    BMM_SENSOR_LOG(LOW, this,
    "sensor type:%d, sensor_state->chosen_sample_rate_old %d",
    sensor_type, (uint32_t)sensor_state->chosen_sample_rate_old);

    bmm150_set_sensor_inst_config(this, instance, chosen_report_rate, chosen_sample_rate,
    sensor_type, registry_cfg, sample_rate_no_need_change);
  }
  /*
  if (!inst_state->deploy_info.enable)
  {
    sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
  #if BMM_CONFIG_POWER_RAIL
    sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(
        sensor_state->pwr_rail_service, this, &sensor_state->common.rail_config, NULL);
  #endif
  }
  */
}
void bmm150_reset_cal_data(sns_sensor_instance *const instance,bmm150_sensor_type sensor_type)
{
  bmm150_instance_state *state =
     (bmm150_instance_state*)instance->state->state;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {1,0,0,0,1,0,0,0,1};
  if(sensor_type == BMM150_MAG)
  {
    for (uint8_t i = 0; i < ARR_SIZE(bias_data); i++)
    {
      state->mag_registry_cfg.fac_cal_bias[i] = bias_data[i];
    }
    for (uint8_t i = 0; i < ARR_SIZE(comp_matrix_data); i++)
    {
      state->mag_registry_cfg.fac_cal_corr_mat.data[i] = comp_matrix_data[i];
    }
    state->mag_registry_cfg.version++;
  }
}
/** See sns_bmm150_sensor.h */
sns_sensor_instance* bmm150_sensor_set_client_request(
    sns_sensor *const this,
    struct sns_request const *exist_request,
    struct sns_request const *new_request,
    bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  bmm150_state *state = (bmm150_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  BMM_SENSOR_LOG(LOW, this, "<sns_see_if__ set_client_request> for %d", state->sensor);

  if(remove)
  {
    if(NULL != instance)
    {
      BMM_SENSOR_LOG(LOW, this, "<sns_see_if__ removing request", exist_request->message_id);
      instance->cb->remove_client_request(instance, exist_request);
      bmm150_reval_instance_config(this, instance, state->sensor);
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
#if BMM_CONFIG_POWER_RAIL
      if(state->sensor == BMM150_MAG || state->sensor == BMM150_TEMPERATURE)
      {
        state->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
      }
      else
      {
        state->common.rail_config.rail_vote = SNS_RAIL_OFF;
      }

      state->pwr_rail_service->api->sns_vote_power_rail_update(
                                          state->pwr_rail_service,
                                          this,
                                          &state->common.rail_config,
                                          &on_timestamp);

      delta = sns_get_system_time() - on_timestamp;

     // Use on_timestamp to determine correct Timer value.
      if(delta < sns_convert_ns_to_ticks(BME_OFF_TO_IDLE_MS * 1000 * 1000))
      {
        bmm150_start_power_rail_timer(this, sns_convert_ns_to_ticks(
                BME_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
                BME_POWER_RAIL_PENDING_SET_CLIENT_REQ);
      }
      else
      {
        // rail is already ON
        reval_config = true;
      }
#else
      UNUSED_VAR(on_timestamp);
      UNUSED_VAR(delta);
      reval_config = true;
#endif

      /** create_instance() calls init() for the Sensor Instance */
      BMM_SENSOR_LOG(LOW, this, "instace is NULL, now create a sigaltone instance");
      instance = this->cb->create_instance(this,
                                           sizeof(bmm150_instance_state));
      BMM_SENSOR_LOG(LOW, this, "instace is %p",instance);
     /* If rail is already ON then flag instance OK to configure */
      if(reval_config)
      {
        bmm150_instance_state *inst_state =
         (bmm150_instance_state *)instance->state->state;

        inst_state->instance_is_ready_to_configure = true;
        BMM_SENSOR_LOG(LOW, this, "reval configure with instance is NULL case");
      }
    }
    else
    {
      if(NULL != exist_request
         && NULL != new_request
         && new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
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
      bmm150_instance_state *inst_state =
         (bmm150_instance_state*)instance->state->state;
      if(new_request != NULL)
      {
        BMM_SENSOR_LOG(LOW, this, "new request from %d message id:%d add to the client request list",
          state->sensor,
          new_request->message_id);
        BMM_SENSOR_LOG(LOW, this, "instace2 is %p reval_config %d",instance, reval_config);
        BMM_SENSOR_LOG(LOW, this, "instance_is_ready_to_configure %d",inst_state->instance_is_ready_to_configure);
        instance->cb->add_client_request(instance, new_request);
        if(new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET)
        {
          BMM_SENSOR_LOG(LOW,this,"Request for resetting cal data");
          bmm150_reset_cal_data(instance,state->sensor);
      #if BMM_CONFIG_ENABLE_REGISTRY
          bmm150_update_sensor_state(this, instance);
          bmm150_update_registry(this, instance, BMM150_MAG);
          bmm150_update_registry(this, instance, BMM150_TEMPERATURE);
      #endif
          bmm150_send_cal_event(instance, state->sensor);
        }

        if(new_request->message_id ==
           SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
        {
          if(bmm150_extract_self_test_info(this, instance, new_request))
          {
            inst_state->new_self_test_request = true;
          }
        }
      }
      if(reval_config && inst_state->instance_is_ready_to_configure)
      {
        BMM_SENSOR_LOG(LOW, this, "try to configure");
        bmm150_reval_instance_config(this, instance, state->sensor);
        if(inst_state->new_self_test_request)
        {
          bmm150_set_self_test_inst_config(this, instance);
        }
      }
    }
  }
  if(NULL != instance &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)TEMPERATURE_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)MAG_SUID, true))
  {
  #if BMM_CONFIG_ENABLE_REGISTRY
    bmm150_instance_state *inst_state =
           (bmm150_instance_state*)instance->state->state;
    if(inst_state->update_fac_cal_in_registry)
    {
      /** Registry write can be handled in normal mode. */
      #if BMM_CONFIG_ENABLE_ISLAND_MODE
      state->island_service->api->sensor_island_exit(state->island_service, this);
      #endif
      /** Update factory cal data in the registry. */
      bmm150_update_registry(this, instance, BMM150_MAG);
      bmm150_update_sensor_state(this, instance);
    }
#endif
    this->cb->remove_instance(instance);
#if BMM_CONFIG_POWER_RAIL
    bmm150_turn_rails_off(this);
#endif
  }
  return instance;
}
