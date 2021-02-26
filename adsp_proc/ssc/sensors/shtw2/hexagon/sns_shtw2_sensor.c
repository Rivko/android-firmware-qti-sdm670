/*******************************************************************************
 * Copyright (c) 2017, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/
#include <string.h>

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_service.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_shtw2_hal.h"
#include "sns_shtw2_sensor.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_timer.pb.h"
#include "sns_types.h"
#include "sns_printf.h"

static void shtw2_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void shtw2_set_self_test_inst_config(sns_sensor *this,
                                     sns_sensor_instance *instance)
{
  sns_request config;

  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

static void shtw2_start_power_rail_timer(sns_sensor *const this,
                                         sns_time timeout_ticks,
                                         shtw2_power_rail_pending_state pwr_rail_pend_state)
{
  shtw2_state *state = (shtw2_state*)this->state->state;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
         .request = buffer, .request_len = req_len };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "SHTW2 timer request encode error");
  }
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void shtw2_publish_registry_attributes(sns_sensor *const this)
{
  shtw2_state *state = (shtw2_state*)this->state->state;
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
      values[i].flt = state->placement[i];
    }
    sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_PLACEMENT,
        values, ARR_SIZE(values), false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = state->registry_pf_cfg.rigid_body_type;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, false);
  }
}

static void shtw2_sensor_process_registry_event(sns_sensor *const this,
                                                sns_sensor_event *event)
{
  bool rv = true;
  shtw2_state *state = (shtw2_state*)this->state->state;

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
      SNS_PRINTF(MED, this, "Registry event decoded");
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == strncmp((char*)group_name.buf,
                      "shtw2_0.ambient_temperature.config",
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf,
                      "shtw2_0.humidity.config",
                      group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
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

          SNS_PRINTF(LOW, this, "Registry read event received with "
                     "is_dri:%d, hardware_id:%lld",
                     state->is_dri, state->hardware_id);
          SNS_PRINTF(LOW, this, "resolution_idx:%d, supports_sync_stream:%d",
                     state->resolution_idx, state->supports_sync_stream);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, "shtw2_0_platform.config",
                           group_name.buf_len))
      {
        {
          sns_registry_decode_arg arg = {
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
          state->rail_config.num_of_rails = state->registry_pf_cfg.num_rail;
          state->registry_rail_on_state = state->registry_pf_cfg.rail_on_state;
          sns_strlcpy(state->rail_config.rails[0].name,
                      state->registry_pf_cfg.vddio_rail,
                      sizeof(state->rail_config.rails[0].name));

          SNS_PRINTF(LOW, this, "Registry read event received with "
                     "bus_type:%d bus_instance:%d slave_control:%d",
                     state->com_port_info.com_config.bus_type,
                     state->com_port_info.com_config.bus_instance,
                     state->com_port_info.com_config.slave_control);
          SNS_PRINTF(LOW, this, "min_bus_speed_KHz:%d max_bus_speed_KHz:%d",
                     state->com_port_info.com_config.min_bus_speed_KHz,
                     state->com_port_info.com_config.max_bus_speed_KHz);
          SNS_PRINTF(LOW, this, "reg_addr_type:%d rigid body type:%d",
                     state->com_port_info.com_config.reg_addr_type,
                     state->registry_pf_cfg.rigid_body_type);
          SNS_PRINTF(LOW, this, "num_rail:%d, rail_on_state: %d",
                     state->rail_config.num_of_rails,
                     state->registry_rail_on_state);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, "shtw2_0_platform.placement",
                           group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->placement,
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
          state->registry_placement_received = true;
          SNS_PRINTF(LOW, this, "Registry read event for placement received");
        }
      }
      else if(0 == strncmp((char*)group_name.buf, "shtw2_0_platform.threshold",
                           group_name.buf_len))
      {
        {
          uint8_t arr_index = 0;
          pb_float_arr_arg arr_arg = {
            .arr = state->threshold,
            .arr_index = &arr_index,
            .arr_len = 2
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = "threshold",
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
          state->registry_threshold_received = true;
          SNS_PRINTF(LOW, this, "Registry read event for thresholds humidity: [%d/100] ambient_temp:[%d/100]received",
		            (int32_t)(state->threshold[0]*100),(int32_t)(state->threshold[1]*100));
        }
      }
      else if(0 == strncmp((char*)group_name.buf,
                           "shtw2_0_platform.ambient_temperature.fac_cal",
                           group_name.buf_len) ||
              0 == strncmp((char*)group_name.buf,
                           "shtw2_0_platform.humidity.fac_cal",
                           group_name.buf_len))
      {
        SNS_PRINTF(LOW, this, "Read factory calibration");

        {
          uint8_t bias_arr_index = 0, scale_arr_index = 0;
          pb_float_arr_arg bias_arr_arg = {
            .arr = state->fac_cal_bias,
            .arr_index = &bias_arr_index,
            .arr_len = 1
          };

          pb_float_arr_arg scale_arr_arg = {
            .arr = state->fac_cal_scale,
            .arr_index = &scale_arr_index,
            .arr_len = 1
          };

          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 2,
            .parse_info[0] = {
              .group_name = "bias",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &bias_arr_arg
            },
            .parse_info[1] = {
              .group_name = "scale",
              .parse_func = sns_registry_parse_float_arr,
              .parsed_buffer = &scale_arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }

        if(rv)
        {
          state->registry_fac_cal_received = true;

          SNS_PRINTF(MED, this, "Registry read event received with "
                     "bias:%d scale:%d", (int)(state->fac_cal_bias[0]),
                     (int)(state->fac_cal_scale[0]));
        }
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u",
                             event->message_id);
  }
}

static void shtw2_sensor_send_registry_request(sns_sensor *const this,
                                               char *reg_group_name)
{
  shtw2_state *state = (shtw2_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_rc rc = SNS_RC_SUCCESS;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
      .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer), &read_request,
                                  sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
        .request_len = encoded_len, .request = buffer,
        .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rc = state->reg_data_stream->api->send_request(state->reg_data_stream,
                                                   &request);
  }

  SNS_PRINTF(LOW, this, "Sending registry request");
}

/**
 * Notification to the client that some data has been received.
 *
 * The client must use the sns_manager_event to obtain this data
 * for processing.
 *
 * @return
 * SNS_RC_INVALID_STATE - A client error occurred; Framework shall destroy client
 * SNS_RC_NOT_AVAILABLE - A transitory error occurred; Framework shall remove all outstanding input
 * SNS_RC_SUCCESS
 *
 * See sns_sensor.h.
 */
sns_rc shtw2_sensor_notify_event(sns_sensor *const this)
{
  shtw2_state *state = (shtw2_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  sns_time on_timestamp;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;

  SNS_PRINTF(MED, this, "Notify event");

  /*-------------------------Handle SUID event.--------------------------*/
  if(state->fw_stream)
  {
    if((0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}),
                        sizeof(state->timer_suid)))
       || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}),
                           sizeof(state->reg_suid))))
    {
      shtw2_process_suid_events(this);

      /* Place a request to registry sensor */
      if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
      {
        if(state->reg_data_stream == NULL)
        {
          state->registry_pf_cfg_received = false;
          state->registry_cfg_received = false;
          state->registry_placement_received = false;
          state->registry_fac_cal_received = false;
          state->registry_threshold_received = false;

          stream_svc->api->create_sensor_stream(stream_svc, this,
                                                state->reg_suid,
                                                &state->reg_data_stream);

          SNS_PRINTF(MED, this, "Request platform config, placement and"
                                "threshold registry");
          shtw2_sensor_send_registry_request(this, "shtw2_0_platform.config");
          shtw2_sensor_send_registry_request(this, "shtw2_0_platform.placement");
          shtw2_sensor_send_registry_request(this, "shtw2_0_platform.threshold");

          if(state->sensor == SHTW2_TEMPERATURE)
          {
            SNS_PRINTF(MED, this, "Request config and factory calibration for the "
                       "temperature sensor");
            shtw2_sensor_send_registry_request(this, "shtw2_0.ambient_temperature.config");
            shtw2_sensor_send_registry_request(this,
                                               "shtw2_0_platform.ambient_temperature.fac_cal");
          }
          else if(state->sensor == SHTW2_HUMIDITY)
          {
            SNS_PRINTF(MED, this, "Request config and factory calibration for the "
                       "humidity sensor");
            shtw2_sensor_send_registry_request(this, "shtw2_0.humidity.config");
            shtw2_sensor_send_registry_request(this,
                                               "shtw2_0_platform.humidity.fac_cal");
          }
        }
      }
    }
  }

  /*----------------------Handle a timer sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    event = state->timer_stream->api->peek_input(state->timer_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == SHTW2_POWER_RAIL_PENDING_INIT)
          {
            rv = shtw2_check_ID_register(state->scp_service,
                                         state->com_port_info.port_handle);
            if(rv == SNS_RC_SUCCESS)
            {
              SNS_PRINTF(HIGH, this, "Found SHTW2 sensor");
              state->hw_is_present = true;
            }
            else
            {
              SNS_PRINTF(HIGH, this, "SHTW2 sensor not found");
            }

            /*------------------Power Down and Close COM Port--------------------*/
            //QC Check for port handle validity
            if(NULL != state->com_port_info.port_handle)
            {
              state->scp_service->api->sns_scp_update_bus_power(
                  state->com_port_info.port_handle,
                  false);
              state->scp_service->api->sns_scp_close(
                  state->com_port_info.port_handle);
              state->scp_service->api->sns_scp_deregister_com_port(
                  &state->com_port_info.port_handle);
            }

            /*----------------------Turn Power Rail OFF--------------------------*/
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     NULL);
            if(state->hw_is_present)
            {
              SNS_PRINTF(MED, this, "SHTW2 HW is available");
              shtw2_publish_available(this);
            }
            else
            {
              rv = SNS_RC_INVALID_STATE;
              SNS_PRINTF(MED, this, "SHTW2 HW absent");
            }
            state->power_rail_pend_state = SHTW2_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == SHTW2_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            if(NULL != instance)
            {
              shtw2_instance_state *inst_state =
                  (shtw2_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              SNS_PRINTF(MED, this, "SHTW2 instance is ready to be configured");
              shtw2_reval_instance_config(this, instance, state->sensor);
              if(inst_state->new_self_test_request)
              {
                shtw2_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = SHTW2_POWER_RAIL_PENDING_NONE;
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
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
      shtw2_sensor_process_registry_event(this, event);

      if(state->registry_cfg_received && state->registry_placement_received)
      {
        SNS_PRINTF(MED, this, "Publish registry attributes");
        shtw2_publish_registry_attributes(this);
      }

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)) &&
     state->registry_pf_cfg_received && state->registry_cfg_received &&
     state->registry_placement_received && state->registry_fac_cal_received &&
     state->registry_threshold_received)
  {
    state->registry_pf_cfg_received = false;

    /*-----------------Register and Open COM Port-------------------------*/
    if(NULL == state->com_port_info.port_handle)
    {
      //QC check com port register return value
      rv = state->scp_service->api->sns_scp_register_com_port(
           &state->com_port_info.com_config,
           &state->com_port_info.port_handle);
      if(rv == SNS_RC_SUCCESS)
      {
         rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
      }
    }

    /*---------------------Register Power Rails --------------------------*/
    if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))
       && NULL == state->pwr_rail_service
       && rv == SNS_RC_SUCCESS)
    {
      SNS_PRINTF(MED, this, "Registering power rails");

      state->rail_config.rail_vote = SNS_RAIL_OFF;
      state->pwr_rail_service =
          (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                          SNS_POWER_RAIL_SERVICE);

      state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                             &state->rail_config);

      /*---------------------Turn Power Rails ON----------------------------*/
      state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
      state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                               this,
                                                               &state->rail_config,
                                                               &on_timestamp);

      /*-------------Create a Timer stream for Power Rail ON timeout.---------*/
      if(NULL == state->timer_stream)
      {
        SNS_PRINTF(MED, this, "Create timer sensor stream");

        stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                              &state->timer_stream);
        if(NULL != state->timer_stream)
        {
          SNS_PRINTF(MED, this, "Power rail turned on, wait for %d ms to let sensor initialize",
                     SHTW2_OFF_TO_IDLE_MS);
          shtw2_start_power_rail_timer(this,
                                       sns_convert_ns_to_ticks(SHTW2_OFF_TO_IDLE_MS * 1000 * 1000),
                                       SHTW2_POWER_RAIL_PENDING_INIT);
        }
      }
    }
  }

  return rv;
}

void shtw2_process_suid_events(sns_sensor *const this)
{
  shtw2_state *state = (shtw2_state*)this->state->state;

  for(sns_sensor_event *event = state->fw_stream->api->peek_input(state->fw_stream);
      NULL != event;
      event = state->fw_stream->api->get_next_input(state->fw_stream))
  {
    if(SNS_SUID_MSGID_SNS_SUID_EVENT == event->message_id)
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

      if(!pb_decode(&stream, sns_suid_event_fields, &suid_event))
      {
        SNS_PRINTF(ERROR, this, "pb_decode() failed");
        continue;
      }

      /* If no suids found, ignore the event */
      if(suid_search.num_of_suids == 0)
      {
        SNS_PRINTF(LOW, this, "Ignoring SUID event, no SUIDs found for SHTW2");
        continue;
      }

      /* Save suid based on incoming data type name */
      if(0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
      {
        SNS_PRINTF(MED, this, "Timer SUID for SHTW2");
        state->timer_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
      {
        SNS_PRINTF(MED, this, "Registry SUID for SHTW2");
        state->reg_suid = uid_list;
      }
      else
      {
        SNS_PRINTF(ERROR, this, "unexpected datatype_name");
      }
    }
  }
}

void shtw2_send_suid_req(sns_sensor *this, char *const data_type,
                         uint32_t data_type_len)
{
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  shtw2_state *state = (shtw2_state*)this->state->state;
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

  if(state->fw_stream == NULL)
  {
    stream_service->api->create_sensor_stream(stream_service, this,
                                              sns_get_suid_lookup(),
                                              &state->fw_stream);
  }

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &suid_req, sns_suid_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
        .request_len = encoded_len,
        .request = buffer,
        .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
    state->fw_stream->api->send_request(state->fw_stream, &request);
  }
}

static void shtw2_get_config(sns_sensor *this,
                             sns_sensor_instance *instance,
                             shtw2_sensor_type sensor_type,
                             float *chosen_sample_rate,
                             bool *sensor_client_present)
{
  sns_sensor_uid suid;
  sns_request const *request;

  if(sensor_type == SHTW2_TEMPERATURE)
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)TEMPERATURE_SUID), sizeof(sns_sensor_uid));
  }
  else if (sensor_type == SHTW2_HUMIDITY)
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)HUMIDITY_SUID), sizeof(sns_sensor_uid));
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Unknown sensor type");
  }

  *chosen_sample_rate = SHTW2_ODR_0;
  *sensor_client_present = false;

  /*
   * Parse through existing requests and get fastest sample rate and report
   * rate requests.
   */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    /*
     * Since humidity and ambient_temperature are on-change sensors, the
     * incoming request is an empty message with ID
     * SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
     */
    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
      *sensor_client_present = true;
      *chosen_sample_rate = SHTW2_ODR_1;
    }
    else  // TODO handle self-test request
    {
    }
  }
  SNS_PRINTF(HIGH, this, "Sensor client present: %d", *sensor_client_present);
  SNS_PRINTF(HIGH, this, "Chosen sample rate (milli-Hz): %d", (int)(*chosen_sample_rate * 1000));
}

static void shtw2_set_inst_config(sns_sensor *this,
                                  sns_sensor_instance *instance,
                                  float chosen_sample_rate,
                                  sns_shtw2_registry_cfg registry_cfg)
{
  sns_shtw2_request new_client_config;
  sns_request config;

  SNS_PRINTF(MED, this, "Set SHT instance client config");

  new_client_config.sample_rate = chosen_sample_rate;
  new_client_config.registry_cfg = registry_cfg;

  config.message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
  config.request_len = sizeof(sns_shtw2_request);
  config.request = &new_client_config;

  this->instance_api->set_client_config(instance, &config);
}

static void shtw2_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    shtw2_state *sensor_state = (shtw2_state*)sensor->state->state;
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

void shtw2_reval_instance_config(sns_sensor *this,
                                 sns_sensor_instance *instance,
                                 shtw2_sensor_type sensor_type)
{
  shtw2_instance_state *inst_state =
      (shtw2_instance_state*)instance->state->state;
  shtw2_state *state = (shtw2_state*)this->state->state;
  float chosen_sample_rate = 0;
  float sample_rate = 0;
  bool temperature_client_present = false;
  bool humidity_client_present = false;
  sns_shtw2_registry_cfg registry_cfg = {.sensor_type = sensor_type};

  if(sensor_type == SHTW2_HUMIDITY)
  {
    registry_cfg.threshold = state->threshold[0];
  }
  else if(sensor_type == SHTW2_TEMPERATURE)
  {
    registry_cfg.threshold = state->threshold[1];
  }

  shtw2_get_config(this,
                   instance,
                   SHTW2_TEMPERATURE,
                   &chosen_sample_rate,
                   &temperature_client_present);
  shtw2_get_config(this,
                   instance,
                   SHTW2_HUMIDITY,
                   &sample_rate,
                   &humidity_client_present);
  chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);

  SNS_PRINTF(HIGH, this, "Temperature sensor client present: %d", temperature_client_present);
  SNS_PRINTF(HIGH, this, "Humidity sensor client present: %d", humidity_client_present);

  if(inst_state != NULL)
  {
    inst_state->temperature_info.enabled = temperature_client_present;
    inst_state->humidity_info.enabled = humidity_client_present;
  }

  sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
              state->fac_cal_bias, sizeof(state->fac_cal_bias));
  sns_memscpy(registry_cfg.fac_cal_scale, sizeof(registry_cfg.fac_cal_scale),
              state->fac_cal_scale, sizeof(state->fac_cal_scale));

  shtw2_set_inst_config(this, instance, chosen_sample_rate, registry_cfg);
}

/**
 * Decodes self test requests.
 *
 * @param[i] this              Sensor reference
 * @param[i] request           Encoded input request
 * @param[o] decoded_request   Decoded standard request
 * @param[o] test_config       Decoded self test request
 *
 * @return bool True if decoding is successfull else false.
 */
static bool shtw2_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
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
    SNS_PRINTF(ERROR, this, "SHTW2 test decode error");
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
static bool shtw2_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  shtw2_state *state = (shtw2_state*)this->state->state;
  shtw2_instance_state *inst_state = (shtw2_instance_state*)instance->state->state;
  shtw2_self_test_info *self_test_info;

  if(state->sensor == SHTW2_TEMPERATURE)
  {
    self_test_info = &inst_state->temperature_info.test_info;
  }
  else if(state->sensor == SHTW2_HUMIDITY)
  {
    self_test_info = &inst_state->humidity_info.test_info;
  }
  else
  {
    return false;
  }

  if(shtw2_get_decoded_self_test_request(this, new_request, &decoded_request,
                                         &test_config))
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

/**
 * Add, remove, or update a client's request to this Sensor.
 *
 * If 'remove' is false:
 * A client has sent a new request to this Sensor.  Determine if any
 * active Sensor Instance in sns_sensor_cb::get_sensor_instances()
 * will handle this request.
 * If not, either instantiate and initialize a new Sensor Instance
 * with the appropriate configuration; or update an existing Instance
 * to additionally support this request.  In all cases, update the
 * list returned from sns_sensor_instance::get_client_requests.
 *
 * If 'remove' is true:
 * Remove this client request from sns_sensor_instance::get_client_requests;
 * re-arrange any remaining client requests/sensor instances.
 *
 * After this function returns, the Framework will forcibly remove this
 * request if not done so by the Sensor.
 *
 * @param[io] this Sensor reference
 * @param[i] exist_request If this request comes-in over an existing stream,
 *                       this is the existing request.
 * @param[i] new_request New request just received
 * @param[i] remove If the client no longer requires this data
 *
 * @return
 * The Sensor Instance chosen to handle this new client.  Will be used
 * to send back the configuration response, and initial state event.
 *
 * See sns_sensor.h.
 */
sns_sensor_instance* shtw2_set_client_request(sns_sensor *const this,
                                              struct sns_request const *exist_request,
                                              struct sns_request const *new_request,
                                              bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  shtw2_state *state = (shtw2_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  if(remove)
  {
    if(NULL != instance)
    {
      SNS_PRINTF(HIGH, this, "Remove SHTW2 client request");

      instance->cb->remove_client_request(instance, exist_request);
      shtw2_reval_instance_config(this, instance, state->sensor);
    }
  }
  else if(NULL != new_request)
  { 


  if ((NULL == instance) && (new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
  {
	  // no op
  }
  else
  {
    SNS_PRINTF(HIGH, this, "Add/update SHTW2 client request");

    /*
     * 1. If new request then:
     *     a. Power ON rails.
     *     b. Power ON COM port - Instance must handle COM port power.
     *     c. Create new instance.
     *     d. Re-evaluate existing requests and choose appropriate instance config.
     *     e. set_client_config for this instance.
     *     f. Add new_request to list of requests handled by the Instance.
     *     g. Power OFF COM port if not needed- Instance must handle COM port power.
     *     h. Return the Instance.
     * 2. If there is an Instance already present:
     *     a. Add new_request to list of requests handled by the Instance.
     *     b. Remove exist_request from list of requests handled by the Instance.
     *     c. Re-evaluate existing requests and choose appropriate Instance config.
     *     d. set_client_config for the Instance if not the same as current config.
     *     e. publish the updated config.
     *     f. Return the Instance.
     * 3.  If "flush" request:
     *     a. Perform flush on the instance.
     *     b. Return NULL.
     */

    if(NULL == instance)
    {
      SNS_PRINTF(HIGH, this, "Create new SHTW2 instance");
      state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
      state->pwr_rail_service->api->sns_vote_power_rail_update(
                                           state->pwr_rail_service,
                                           this,
                                           &state->rail_config,
                                           &on_timestamp);

      delta = sns_get_system_time() - on_timestamp;

      if(delta < sns_convert_ns_to_ticks(SHTW2_OFF_TO_IDLE_MS * 1000 * 1000))
      {
        SNS_PRINTF(MED, this, "Power rail turned on, wait for %d ms to let sensor initialize",
                   SHTW2_OFF_TO_IDLE_MS);
        shtw2_start_power_rail_timer(this,
                                     sns_convert_ns_to_ticks(SHTW2_OFF_TO_IDLE_MS * 1000 * 1000) - delta,
                                     SHTW2_POWER_RAIL_PENDING_SET_CLIENT_REQ);
      }
      else
      {
        /* Rail is already ON. */
        reval_config = true;
      }

      /* create_instance() calls init() for the sensor instance. */
      instance = this->cb->create_instance(this,
                                           sizeof(shtw2_instance_state));
      /* If rail is already ON then flag instance OK to configure. */
      if(reval_config)
      {
        shtw2_instance_state *inst_state =
            (shtw2_instance_state*)instance->state->state;
        inst_state->instance_is_ready_to_configure = true;
        SNS_PRINTF(HIGH, this, "SHTW2 instance is ready to be configured");
      }
      else
      {
        SNS_PRINTF(HIGH, this, "SHTW2 instance NOT yet ready to be configured");
      }
    }
    else
    {
      if(new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
      {
        sns_sensor_util_send_flush_event(&state->my_suid, instance);
        return instance;
      }
      else
      {
        reval_config = true;

        /* An existing client is changing request. */
        if((NULL != exist_request) && (NULL != new_request))
        {
          instance->cb->remove_client_request(instance, exist_request);
        }
        /* A new client sent new_request. */
        else if(NULL != new_request)
        {
         /* No-op. new_request will be added to requests list below. */
        }
      }
    }

    /* Add the new request to list of client_requests. */
    if(NULL != instance)
    {
      shtw2_instance_state *inst_state =
          (shtw2_instance_state*)instance->state->state;
      if((NULL != new_request) && (new_request->message_id != SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
      {
       instance->cb->add_client_request(instance, new_request);

       if(new_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
       {
         if(shtw2_extract_self_test_info(this, instance, new_request))
         {
           inst_state->new_self_test_request = true;
         }
       }
       else if(new_request->message_id ==
               SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
       {
         /* Force a first sample out if there is a new client. */
         if(state->sensor == SHTW2_HUMIDITY)
         {
           inst_state->humidity_info.force_first_sample = true;
         }
         else
         {
           inst_state->temperature_info.force_first_sample = true;
         }
       }
      }
      if(reval_config && inst_state->instance_is_ready_to_configure)
      {
        shtw2_reval_instance_config(this, instance, state->sensor);

        if(inst_state->new_self_test_request)
        {
          shtw2_set_self_test_inst_config(this, instance);
        }
      }
    }
  }
  }
  else // bad request
  {
    instance = NULL;
  }

  /* QC: Sensors are required to call remove_instance when clientless. */
  if(NULL != instance &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)HUMIDITY_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)TEMPERATURE_SUID, true))
  {
    SNS_PRINTF(HIGH, this, "Remove clientless instance");
    this->cb->remove_instance(instance);
    shtw2_turn_rails_off(this);
  }

  return instance;
}
