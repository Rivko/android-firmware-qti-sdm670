 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Vishay Capella Microsystems nor the
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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_cm3526_sensor.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"

#include "sns_diag_service.h"
#include "sns_printf.h"

/** See sns_cm3526_sensor.h */
void cm3526_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len)
{
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  cm3526_state *state = (cm3526_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
      (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  size_t encoded_len;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = data_type, .buf_len = data_type_len };

  sns_suid_req suid_req = sns_suid_req_init_default;
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;
  suid_req.data_type.funcs.encode = &pb_encode_string_cb;
  suid_req.data_type.arg = &data;

  if (state->fw_stream == NULL)
  {
    stream_service->api->create_sensor_stream(stream_service, this,
                                              sns_get_suid_lookup(), &state->fw_stream);
  }

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
                                  &suid_req, sns_suid_req_fields, NULL);
  if (0 < encoded_len)
  {
    sns_request request = (sns_request){ .request_len = encoded_len,
                                         .request = buffer,
                                         .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
    state->fw_stream->api->send_request(state->fw_stream, &request);
  }

}
static void cm3526_turn_rails_off(sns_sensor *this)
{
    sns_sensor *sensor;

    for (sensor = this->cb->get_library_sensor(this, true);
         NULL != sensor;
         sensor = this->cb->get_library_sensor(this, false))
    {
        cm3526_state *sensor_state = (cm3526_state*)sensor->state->state;
        if (sensor_state->common.rail_config.rail_vote != SNS_RAIL_OFF)
        {
            sensor_state->common.rail_config.rail_vote = SNS_RAIL_OFF;
            sensor_state->pwr_rail_service->api->sns_vote_power_rail_update(sensor_state->pwr_rail_service,
                                                                            sensor,
                                                                            &sensor_state->common.rail_config,
                                                                            NULL);
        }
    }
}

/** See sns_cm3526_sensor.h */
void cm3526_process_suid_events(sns_sensor *const this)
{
  cm3526_state *state = (cm3526_state*)this->state->state;

  for (;
       0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
       state->fw_stream->api->get_next_input(state->fw_stream))
  {
    sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);
    if (SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
    {
      pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);
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

      if (!pb_decode(&stream, sns_suid_event_fields, &suid_event))
      {
        SNS_PRINTF(ERROR, this, "SUID decode failed");
        continue;
      }

      /* if no suids found, ignore the event */
      if (suid_search.num_of_suids == 0)
      {
        continue;
      }

      /* save suid based on incoming data type name */
      if (0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
      {
        state->common.timer_suid = uid_list;
      }
      else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
      {
        state->common.reg_suid = uid_list;
      }
      else
      {
        SNS_PRINTF(ERROR, this, "unexpected datatype_name");
      }
    }
  }
}

static void cm3526_sensor_send_registry_request(sns_sensor *const this, char *reg_group_name)
{
  cm3526_state *state = (cm3526_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_rc rc = SNS_RC_SUCCESS;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name, .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),   &read_request, sns_registry_read_req_fields, NULL);
  if (0 < encoded_len)
  {
    sns_request request = { .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ,
                            .request_len = encoded_len,
                            .request = buffer };
    rc = state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
  }
  //SENSOR_PRINTF_LOW_FULL(this, "Sending registry request for group name:%s", reg_group_name);
}

static void cm3526_sensor_process_registry_event(sns_sensor *const this,
                                                  sns_sensor_event *event)
{
  bool rv = true;
  cm3526_state *state = (cm3526_state*)this->state->state;

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event, event->event_len);

  if (SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if (!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_PRINTF(ERROR, this, "Error decoding registry event");
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if (0 == strncmp((char*)group_name.buf, "cm3526_0.uv.config", group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg =
          {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] =
            {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_cfg,
              .parsed_buffer = &state->common.registry_cfg
            }
          };
          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if (rv)
        {
          state->common.registry_cfg_received = true;
          state->is_dri = state->common.registry_cfg.is_dri;
          state->hardware_id = state->common.registry_cfg.hw_id;
          state->resolution_idx = state->common.registry_cfg.res_idx;
          state->supports_sync_stream = state->common.registry_cfg.sync_stream;
          SNS_PRINTF(LOW, this, "Registry read event for group cm3526_0.uv.config received "
                                 "is_dri:%d, hardware_id:%d ",
                                 state->is_dri,(uint32_t)state->hardware_id);
          SNS_PRINTF(LOW, this, "resolution_idx:%d, supports_sync_stream:%d ",
                                state->resolution_idx,
                                state->supports_sync_stream);
        }
      }
      else if (0 == strncmp((char*)group_name.buf, "cm3526_0_platform.config", group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg =
          {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] =
            {
              .group_name = "config",
              .parse_func = sns_registry_parse_phy_sensor_pf_cfg,
              .parsed_buffer = &state->common.registry_pf_cfg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if (rv)
        {
          state->common.registry_pf_cfg_received = true;
          state->common.com_port_info.com_config.bus_type = state->common.registry_pf_cfg.bus_type;
          state->common.com_port_info.com_config.bus_instance = state->common.registry_pf_cfg.bus_instance;
          state->common.com_port_info.com_config.slave_control = state->common.registry_pf_cfg.slave_config;
          state->common.com_port_info.com_config.min_bus_speed_KHz = state->common.registry_pf_cfg.min_bus_speed_khz;
          state->common.com_port_info.com_config.max_bus_speed_KHz = state->common.registry_pf_cfg.max_bus_speed_khz;
          state->common.com_port_info.com_config.reg_addr_type = state->common.registry_pf_cfg.reg_addr_type;
          state->common.rail_config.num_of_rails = state->common.registry_pf_cfg.num_rail;
          state->common.registry_rail_on_state = state->common.registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->common.rail_config.rails[0].name,
                      state->common.registry_pf_cfg.vddio_rail,
                      sizeof(state->common.rail_config.rails[0].name));
          sns_strlcpy(state->common.rail_config.rails[1].name,
                      state->common.registry_pf_cfg.vdd_rail,
                      sizeof(state->common.rail_config.rails[1].name));

          SNS_PRINTF(LOW, this, "Registry read event for cm3526_0_platform.config gorup received "
             "bus_type:%d bus_instance:%d slave_control:%d",
                               state->common.com_port_info.com_config.bus_type,
                               state->common.com_port_info.com_config.bus_instance,
                               state->common.com_port_info.com_config.slave_control);

          SNS_PRINTF(LOW, this, "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
                               state->common.com_port_info.com_config.min_bus_speed_KHz,
                               state->common.com_port_info.com_config.max_bus_speed_KHz,
                               state->common.com_port_info.com_config.reg_addr_type);

          SNS_PRINTF(LOW, this, " rigid body type:%d", state->common.registry_pf_cfg.rigid_body_type);
        }
      }
      else if (0 == strncmp((char*)group_name.buf, "cm3526_0_platform.placement", group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg =
          {
            .arr = state->common.placement,
            .arr_index = &arr_index,
            .arr_len = 12
          };

          sns_registry_decode_arg arg =
          {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] =
            {
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
          for(uint8_t tmp=0;tmp<12;tmp++)
          {
            SNS_PRINTF(LOW, this,"Placement index:%d,value:[%d/1000]",tmp,(int32_t)(state->common.placement[tmp]*1000) );
          }
        }
      }
      else if (0 == strncmp((char*)group_name.buf, "cm3526_0_platform.uv.fac_cal", group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg =
          {
            .arr = (float *)&state->uv_fac_cal,
            .arr_index = &arr_index,
            .arr_len = 6
          };

          sns_registry_decode_arg arg =
          {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] =
            {
              .group_name = "fac_cal",
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
          state->registry_fac_cal_received = true;
          SNS_PRINTF(LOW, this, "Registry read event for fac cal group received "
            "a_scale:[%d/1000] a_bias:[%d/1000] b_scale:[%d/1000] b_bias:[%d/1000]",
            (int32_t)(state->uv_fac_cal.a_scale*1000), (int32_t)(state->uv_fac_cal.a_bias*1000),
            (int32_t)(state->uv_fac_cal.b_scale*1000), (int32_t)(state->uv_fac_cal.b_bias*1000));
        }
      }
      else
      {
        rv = false;
      }

      if (!rv)
      {
        //SENSOR_PRINTF_ERROR_FULL(this, "Error decoding registry group %s due to %s", (char*)group_name.buf,
        //                         PB_GET_ERROR(&stream));
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u", event->message_id);
  }
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void cm3526_publish_registry_attributes(sns_sensor *const this)
{
  cm3526_state *state = (cm3526_state*)this->state->state;
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->is_dri;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_DRI, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = state->supports_sync_stream;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_STREAM_SYNC, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->hardware_id;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_HW_ID, &value, 1, false);
  }
  {
    sns_std_attr_value_data values[] = {SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR,
      SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR, SNS_ATTR};
        for (uint8_t i = 0; i < 12; i++)
    {
      values[i].has_flt = true;
      values[i].flt = state->common.placement[i];
    }
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT, values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->common.registry_pf_cfg.rigid_body_type;
        sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
}

void cm3526_start_power_rail_timer(sns_sensor *const this,
                                   sns_time timeout_ticks,
                                   cm3526_power_rail_pending_state pwr_rail_pend_state)
{
  cm3526_state *state = (cm3526_state*)this->state->state;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  req_len = pb_encode_request(buffer,
                              sizeof(buffer),
                              &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if (req_len > 0)
  {
    sns_request timer_req = { .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
                              .request_len = req_len,
                              .request = buffer };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "LSM timer req encode error");
  }
}

static void cm3526_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void cm3526_set_self_test_inst_config(sns_sensor *this, sns_sensor_instance *instance)
{
    sns_request config;

    config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    config.request_len = 0;
    config.request = NULL;

    this->instance_api->set_client_config(instance, &config);
}

/** See sns_cm3526_sensor.h*/
sns_rc cm3526_sensor_notify_event(sns_sensor *const this)
{
    cm3526_state *state = (cm3526_state*)this->state->state;
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service *)service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    sns_time on_timestamp;
    uint16_t reg_read;
    sns_rc rv = SNS_RC_SUCCESS;
    sns_sensor_event *event;

    SNS_PRINTF(MED, this, "cm3526_sensor_notify_event: Enter");

    if(state->fw_stream)
    {
      if((0 == sns_memcmp(&state->common.timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->common.timer_suid))) ||
          (0 == sns_memcmp(&state->common.reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->common.reg_suid))))
      {
        cm3526_process_suid_events(this);

        // place a request to registry sensor
        if ((0 != sns_memcmp(&state->common.reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->common.reg_suid))))
        {
          if (state->reg_data_stream == NULL)
          {
            stream_svc->api->create_sensor_stream(stream_svc, this, state->common.reg_suid , &state->reg_data_stream);
            cm3526_sensor_send_registry_request(this, "cm3526_0_platform.config");
            cm3526_sensor_send_registry_request(this, "cm3526_0_platform.placement");
            cm3526_sensor_send_registry_request(this, "cm3526_0.uv.config");
            cm3526_sensor_send_registry_request(this, "cm3526_0_platform.uv.fac_cal");
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
            pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
            sns_timer_sensor_event timer_event;
            if (event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
            {
                if (pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
                {
                    if (state->power_rail_pend_state == CM3526_POWER_RAIL_PENDING_INIT)
                    {
                        /**-------------------Read and Confirm WHO-AM-I------------------------*/
                        reg_read = 0x0;
                        rv = cm3526_get_who_am_i(state->scp_service,state->common.com_port_info.port_handle, (uint8_t *)&reg_read);
                        if ((rv == SNS_RC_SUCCESS) && ((reg_read & 0xFF) == CM3526_WHOAMI_VALUE))
                        {
                            // Reset Sensor
                            rv = cm3526_reset_device(state->scp_service,state->common.com_port_info.port_handle, CM3526_UV);

                            if (rv == SNS_RC_SUCCESS)
                            {
                                SNS_PRINTF(LOW, this, "cm3526_sensor HW present");
                                state->common.hw_is_present = true;
                            }
                        }
                        state->common.who_am_i = (reg_read & 0xFF);

                        /**------------------Power Down and Close COM Port--------------------*/
                        state->scp_service->api->sns_scp_update_bus_power(state->common.com_port_info.port_handle, false);

                        state->scp_service->api->sns_scp_close(state->common.com_port_info.port_handle);
                        state->scp_service->api->sns_scp_deregister_com_port(&state->common.com_port_info.port_handle);

                        /**----------------------Turn Power Rail OFF--------------------------*/
                        state->common.rail_config.rail_vote = SNS_RAIL_OFF;
                        state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                                 this,
                                                                                 &state->common.rail_config,
                                                                                 NULL);
                        if (state->common.hw_is_present)
                        {
                            cm3526_publish_available(this);
                        }
                        else
                        {
                            rv = SNS_RC_INVALID_STATE;
                            cm3526_turn_rails_off(this);
                            SNS_PRINTF(LOW, this, "CM3526 HW absent");
                        }
                        state->power_rail_pend_state = CM3526_POWER_RAIL_PENDING_NONE;
                    }
                    else if (state->power_rail_pend_state == CM3526_POWER_RAIL_PENDING_SET_CLIENT_REQ)
                    {
                        sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);                        
                        if (NULL != instance)
                        {
						  cm3526_instance_state *inst_state = (cm3526_instance_state*) instance->state->state;
                          inst_state->instance_is_ready_to_configure = true;
                          cm3526_reval_instance_config(this, instance, state->sensor);

                          if (inst_state->new_self_test_request)
                          {
                            cm3526_set_self_test_inst_config(this, instance);
                          }
                        }
                        state->power_rail_pend_state = CM3526_POWER_RAIL_PENDING_NONE;
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

    if (NULL != state->reg_data_stream)
    {
        event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
        while (NULL != event)
        {
            cm3526_sensor_process_registry_event(this, event);

            if (state->common.registry_cfg_received && state->common.registry_placement_received)
            {
              cm3526_publish_registry_attributes(this);
            }

            event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
        }
    }

    if (0 != sns_memcmp(&state->common.timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->common.timer_suid)) &&
        state->common.registry_pf_cfg_received && state->common.registry_cfg_received && state->common.registry_placement_received)
    {
        state->common.registry_pf_cfg_received = false;

        /**-----------------Register and Open COM Port-------------------------*/
        if (NULL == state->common.com_port_info.port_handle)
        {
            rv = state->scp_service->api->sns_scp_register_com_port(&state->common.com_port_info.com_config,
                                                                    &state->common.com_port_info.port_handle);

            if (rv == SNS_RC_SUCCESS)
            {
                state->scp_service->api->sns_scp_open(state->common.com_port_info.port_handle);
            }
        }

      /**---------------------Register Power Rails --------------------------*/
        if (0 != sns_memcmp(&state->common.timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->common.timer_suid)) &&
            NULL == state->pwr_rail_service &&
            rv == SNS_RC_SUCCESS)
        {
            state->common.rail_config.rail_vote = SNS_RAIL_OFF;

            state->pwr_rail_service =
              (sns_pwr_rail_service*)service_mgr->get_service(service_mgr, SNS_POWER_RAIL_SERVICE);

            state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service, &state->common.rail_config);

            /**---------------------Turn Power Rails ON----------------------------*/
            state->common.rail_config.rail_vote = state->common.registry_rail_on_state;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                         this,
                                         &state->common.rail_config,
                                         &on_timestamp);

            /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
            if (NULL == state->timer_stream)
            {
                stream_svc->api->create_sensor_stream(stream_svc, this, state->common.timer_suid, &state->timer_stream);
                if (NULL != state->timer_stream)
                {
                  cm3526_start_power_rail_timer(this,
                                                sns_convert_ns_to_ticks(CM3526_OFF_TO_IDLE_MS * 1000 * 1000),
                                                CM3526_POWER_RAIL_PENDING_INIT);
                }
            }
        }
    }

    SNS_PRINTF(MED, this, "cm3526_sensor_notify_event: Exit");

    return rv;
}

/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool cm3526_get_decoded_uv_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{
    pb_istream_t stream;
    pb_simple_cb_arg arg = { .decoded_struct = decoded_payload, .fields = sns_std_sensor_config_fields };
    decoded_request->payload = (struct pb_callback_s){ .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(in_request->request, in_request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
    {
      SNS_PRINTF(ERROR, this, "LSM decode error");
      return false;
    }
    return true;
}

static void cm3526_get_uv_config(sns_sensor *this,
                                 sns_sensor_instance *instance,
                                 float *chosen_sample_rate,
                                 float *chosen_report_rate,
                                 bool *sensor_uv_client_present)
{
    //UNUSED_VAR(this);
    cm3526_instance_state *inst_state = (cm3526_instance_state*)instance->state->state;
    sns_sensor_uid suid = UV_SUID;
    sns_request const *request;

    sns_memscpy(&inst_state->uv_info.suid,
                sizeof(inst_state->uv_info.suid),
                &suid,
                sizeof(suid));

    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
    *sensor_uv_client_present = false;

    /** Parse through existing requests and get fastest sample
    *  rate and report rate requests. */
    for (request = instance->cb->get_client_request(instance, &suid, true);
         NULL != request;
         request = instance->cb->get_client_request(instance, &suid, false))
    {
        sns_std_request decoded_request;
        sns_std_sensor_config decoded_payload = sns_std_sensor_config_init_default;

        if (request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
            if (cm3526_get_decoded_uv_request(this, request, &decoded_request, &decoded_payload))
            {
              float report_rate;
              *chosen_sample_rate = SNS_MAX(*chosen_sample_rate, decoded_payload.sample_rate);
			  // Sensor going into Infinite loop, 
			  // If there are two clients present in client list with below function  
              //bool rc = sns_sensor_util_decide_max_batch(instance,&suid);
              if (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch)
              {
                report_rate = (1000000.0 / (float)UINT32_MAX);
              }
              else
              {
                //There is request with max batch not set .
                // do normal calculation
                if (decoded_request.has_batching && decoded_request.batching.batch_period > 0)
                {
                  report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
                }
                else
                {
                  report_rate = *chosen_sample_rate;
                }				
              }
              *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
              *sensor_uv_client_present = true;
            }
        }
    }
    inst_state->uv_info.report_rate_hz = *chosen_report_rate;
    inst_state->uv_info.sampling_rate_hz = *chosen_sample_rate;
}

static void cm3526_set_inst_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   float chosen_report_rate,
                                   float chosen_sample_rate,
                                   uint32_t message_id)
{
    SNS_PRINTF(MED, this, "cm3526_set_uv_inst_config: Enter");

    sns_cm3526_uv_req new_client_config;
    cm3526_state *state = (cm3526_state*)this->state->state;
    sns_request config;

    new_client_config.report_rate = chosen_report_rate;
    new_client_config.sample_rate = chosen_sample_rate;
    new_client_config.uv_fac_cal = state->uv_fac_cal;

    config.message_id = message_id;
    config.request_len = sizeof(sns_cm3526_uv_req);
    config.request = &new_client_config;

    this->instance_api->set_client_config(instance, &config);

    SNS_PRINTF(MED, this, "cm3526_set_uv_inst_config: Exit");
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
static bool cm3526_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
                                                 sns_std_request *decoded_request,
                                                 sns_physical_sensor_test_config *test_config)
{
    pb_istream_t stream;
    pb_simple_cb_arg arg = { .decoded_struct = test_config, .fields = sns_physical_sensor_test_config_fields };
    decoded_request->payload = (struct pb_callback_s){ .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(request->request, request->request_len);
    if (!pb_decode(&stream, sns_std_request_fields, decoded_request))
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
static bool cm3526_extract_self_test_info(sns_sensor *this,
                                          sns_sensor_instance *instance,
                                          struct sns_request const *new_request)
{
    sns_std_request decoded_request;
    sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
    cm3526_state *state = (cm3526_state*)this->state->state;
    cm3526_instance_state *inst_state = (cm3526_instance_state*)instance->state->state;
    cm3526_self_test_info *self_test_info;

    if (state->sensor == CM3526_UV)
    {
        self_test_info = &inst_state->uv_info.test_info;
    }
    else
    {
        return false;
    }

    if (cm3526_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
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

void cm3526_reval_instance_config(sns_sensor *this,
                                  sns_sensor_instance *instance,
                                  cm3526_sensor_type sensor_type)
{
    UNUSED_VAR(sensor_type);

    float sample_rate = 0;
    float report_rate = 0;
    bool sensor_uv_client_present;
    cm3526_instance_state *inst_state = (cm3526_instance_state*)instance->state->state;

    cm3526_get_uv_config(this,
                         instance,
                         &sample_rate,
                         &report_rate,
                         &sensor_uv_client_present);

    cm3526_set_inst_config(this,
                           instance,
                           report_rate,
                           sample_rate,
                           SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);

    if (!sensor_uv_client_present)
    {
        cm3526_turn_rails_off(this);
        inst_state->instance_is_ready_to_configure = false;
    }
}

/** See sns_cm3526_sensor.h */
sns_sensor_instance* cm3526_set_client_request(sns_sensor *const this,
                                               struct sns_request const *exist_request,
                                               struct sns_request const *new_request,
                                               bool remove)
{
    sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
    cm3526_state *state = (cm3526_state*)this->state->state;
    sns_time on_timestamp;
    sns_time delta;
    bool reval_config = false;

    SNS_PRINTF(MED, this, "cm3526_set_client_request: Enter");

    if (remove)
    {
        if (NULL != instance)
        {
            instance->cb->remove_client_request(instance, exist_request);
            /* Assumption: The FW will call deinit() on the instance before destroying it.
                         Putting all HW resources (sensor HW, COM port, power rail) in
                         low power state happens in Instance deinit().*/

            cm3526_reval_instance_config(this, instance, state->sensor);
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

        if (NULL == instance)
        {
            state->common.rail_config.rail_vote = state->common.registry_rail_on_state;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->common.rail_config,
                                                                     &on_timestamp);

            delta = sns_get_system_time() - on_timestamp;

            // Use on_timestamp to determine correct Timer value.
            if (delta < sns_convert_ns_to_ticks(CM3526_OFF_TO_IDLE_MS * 1000 * 1000))
            {
                cm3526_start_power_rail_timer(this,
                                              sns_convert_ns_to_ticks(CM3526_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
                                              CM3526_POWER_RAIL_PENDING_SET_CLIENT_REQ);
            }
            else
            {
                // rail is already ON
                reval_config = true;
            }

            /** create_instance() calls init() for the Sensor Instance */
            instance = this->cb->create_instance(this, sizeof(cm3526_instance_state));
            /* If rail is already ON then flag instance OK to configure */
            if (reval_config)
            {
                cm3526_instance_state *inst_state = (cm3526_instance_state*)instance->state->state;

                inst_state->instance_is_ready_to_configure = true;
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
                if ((NULL != exist_request) && (NULL != new_request))
                {
                    instance->cb->remove_client_request(instance, exist_request);
                }
                /** A new client sent new_request*/
                else if (NULL != new_request)
                {
                    // No-op. new_request will be added to requests list below.
                }
            }
        }

        /** Add the new request to list of client_requests.*/
        if (NULL != instance)
        {
            cm3526_instance_state *inst_state = (cm3526_instance_state*)instance->state->state;
            if (NULL != new_request)
            {
                instance->cb->add_client_request(instance, new_request);

                if (new_request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
                {
                    if (cm3526_extract_self_test_info(this, instance, new_request))
                    {
                        inst_state->new_self_test_request = true;
                    }
                }
            }
            if (reval_config && inst_state->instance_is_ready_to_configure)
            {
                cm3526_reval_instance_config(this, instance, state->sensor);

                if (inst_state->new_self_test_request)
                {
                    cm3526_set_self_test_inst_config(this, instance);
                }
            }
        }
    }

    if (NULL != instance &&
        NULL == instance->cb->get_client_request(instance, &(sns_sensor_uid)UV_SUID, true))
    {
        this->cb->remove_instance(instance);
        cm3526_turn_rails_off(this);
    }

    SNS_PRINTF(MED, this, "cm3526_set_client_request: Exit");

    return instance;
}

