/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "sns_tmx4903_sensor.h"


const ams_calibrationData_t default_common_cal_data = {
  .timeBase_us = 2780,
  .adcMaxCount = 1024,
  .alsThresholdHigh = 5,
  .alsThresholdLow = 5,
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
  .alsCalibrationLuxTargetHigh = 100,
  .alsCalibrationLuxTargetLow = 100,
  .alsCalibrationLuxTarget = 300,
  .alsCalibrationLuxTargetError = 15,
  .alsCalibrationFactor = 1000,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  .proxAdaptiveThresholdFactor = 100,
  .proxAdaptiveThreshold = 100,
  .proxHardThreshold = 29952,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  .Cc = 118,
  .Rc = 112,
  .Gc = 172,
  .Bc = 180,
  .alsCoefR = 195,
  .alsCoefG = 1000,
  .alsCoefB = -293,
  .alsDfg = 436,
  .alsCctOffset = 1053,
  .alsCctCoef = 4417,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
  .Wbc = 111,
  .Wideband_C_factor = 790,
  .Wideband_R_factor = 540,
  .Wideband_B_factor = 390
#endif
};

void ams_send_fifo_flush_done(sns_sensor_instance *const instance,
                              sns_sensor_uid *suid)
{
  sns_service_manager *mgr = instance->cb->get_service_manager(instance);
  sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
  sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

  if(NULL != event)
  {
    event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
    event->event_len = 0;
    event->timestamp = sns_get_system_time();

    e_service->api->publish_event(e_service, instance, event, suid);
  }
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
static bool ams_get_decoded_std_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_std_sensor_config_fields };
#ifndef ENABLE_DEBUG
  UNUSED_VAR(this)
#endif
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "AMS: ERROR: ams_get_decoded_std_request");
    return false;
  }
  return true;
}

static void ams_get_device_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   ams_sensor_type sensor_type,
                                   float *chosen_sample_rate,
                                   float *chosen_report_rate,
                                   bool *client_present,
                                   int32_t *num_clients)
{
  sns_sensor_uid suid;
  sns_request const *request;
  ams_state *state = (ams_state*)this->state->state;
  sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid){{0}}), sizeof(sns_sensor_uid));

  *num_clients = 0;
  *client_present = false;

  if(0)
  { ; }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if(sensor_type == AMS_ALS_OC)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)ALS_SUID), sizeof(sns_sensor_uid));
  }
  else if(sensor_type == AMS_ALS_STRM)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)ALS_STREAMING_SUID), sizeof(sns_sensor_uid));
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  else if(sensor_type == AMS_RGB_OC)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)RGB_SUID), sizeof(sns_sensor_uid));
  }
  else if(sensor_type == AMS_RGB_STRM)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)RGB_STREAMING_SUID), sizeof(sns_sensor_uid));
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  else if(sensor_type == AMS_PROX_OC)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)PROX_SUID), sizeof(sns_sensor_uid));
  }
  else if(sensor_type == AMS_PROX_STRM)
  {
    sns_memscpy(&suid, sizeof(suid),
        &((sns_sensor_uid)PROX_STREAMING_SUID), sizeof(sns_sensor_uid));
  }
#endif
  else
  {
    return;
  }

  if (instance->cb->get_client_request(instance, &suid, true) != NULL)
  {
    *chosen_report_rate = 0;
    *chosen_sample_rate = 0;
  }
  /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    AMS_PORT_log_Msg_4(AMS_MED, "ams_get_device_config: this %p, instance %p, sensor_type %d, msgId=%d"
                      , this
                      , instance
                      , sensor_type
                      , request->message_id
                      );

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(ams_get_decoded_std_request(this, request, &decoded_request, &decoded_payload))
      {
        SNS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d, curr SR %d, recvd SR %d",
                                sensor_type,
                                (int32_t)*chosen_sample_rate,
                                (int32_t)decoded_payload.sample_rate);
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);
        *chosen_report_rate = *chosen_sample_rate;
        *client_present = true;
        (*num_clients)++;
      }
    }
    else if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
      SNS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d (ON_CHANGE)",
                              sensor_type
                              );
      *chosen_sample_rate = 0;
      *chosen_report_rate = 0;
      *client_present = true;
      (*num_clients)++;
    }
    else if(request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
    {
      SNS_PRINTF(MED, this, "AMS: ams_get_device_config: sensor_type %d (SELF TEST)",
                              sensor_type
                              );
      *chosen_sample_rate = state->odr_for_selftest;
      *chosen_report_rate = state->odr_for_selftest;
      *client_present = true;
    }
  }
}

static void ams_set_inst_config(sns_sensor *this,
                                    sns_sensor_instance *instance,
                                    float chosen_report_rate,
                                    float chosen_sample_rate,
                                    uint32_t message_id)
{
  sns_std_req new_client_config;
  sns_request config;

  new_client_config.desired_report_rate = chosen_report_rate;
  new_client_config.desired_sample_rate = chosen_sample_rate;

  config.message_id = message_id;
  config.request_len = sizeof(sns_std_req);
  config.request = &new_client_config;

  this->instance_api->set_client_config(instance, &config);
}

static void ams_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    ams_state *sensor_state = (ams_state*)sensor->state->state;
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

void ams_start_power_rail_timer(sns_sensor *const this,
                                sns_time timeout_ticks,
                                ams_power_rail_pending_state pwr_rail_pend_state)
{
  ams_state *state = (ams_state*)this->state->state;

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
         .request = buffer, .request_len = req_len  };
    state->timer_stream->api->send_request(state->timer_stream, &timer_req);
    state->power_rail_pend_state = pwr_rail_pend_state;
  }
  else
  {
    SNS_PRINTF(ERROR, this, "AMS timer req encode error");
  }
}

/**
 * Publish attributes read from registry
 *
 * @param[i] this    reference to this Sensor
 *
 * @return none
 */
static void
ams_publish_registry_attributes(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;
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
        this, SNS_STD_SENSOR_ATTRID_RIGID_BODY, &value, 1, true);
  }
}

void ams_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              ams_sensor_type sensor_type)
{
  /**
   * 1. Get best ALS Config.
   * 2. Get best Proximity Config.
   * 3. Get best RGB Config.
   * 4. Get best Color Temperature Config.
   * 5. Decide best Instance Config based on above outputs.
   */
  UNUSED_VAR(sensor_type);
  float chosen_sample_rate = 0;
  float chosen_report_rate = 0;
  float sample_rate = 0;
  float report_rate = 0;
  bool client_present = false;
  int32_t num_clients;
  ams_state *state = (ams_state*)this->state->state;
  ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;

  SNS_PRINTF(MED, this, " AMS: ams_reval_instance_config: this %p, instance %p, sensor_type %d", this,
                          instance,
                          state->sensor_type);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
    ams_get_device_config(this,
                           instance,
                           AMS_ALS_STRM,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: ALS: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);
    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_ALS_STRM;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_ALS_STRM);
    }

    ams_get_device_config(this,
                           instance,
                           AMS_ALS_OC,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: ALS: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);
    if (num_clients > inst_state->als_info.client_cfg.num_oc_clients_prev
        &&
        num_clients > 1)
    {
      inst_state->als_info.client_cfg.oc_send_last_event = true;
    }
    else
    {
      inst_state->als_info.client_cfg.oc_send_last_event = false;
    }
    inst_state->als_info.client_cfg.num_oc_clients_prev = num_clients;

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_ALS_OC;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_ALS_OC);
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    ams_get_device_config(this,
                           instance,
                           AMS_RGB_STRM,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: RGB: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);
    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_RGB_STRM;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_RGB_STRM);
    }

    ams_get_device_config(this,
                           instance,
                           AMS_RGB_OC,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: RGB: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)sample_rate,
                            (int32_t)chosen_sample_rate);

    if (num_clients > inst_state->rgb_info.client_cfg.num_oc_clients_prev
        &&
        num_clients > 1)
    {
      inst_state->rgb_info.client_cfg.oc_send_last_event = true;
    }
    else
    {
      inst_state->rgb_info.client_cfg.oc_send_last_event = false;
    }
    inst_state->rgb_info.client_cfg.num_oc_clients_prev = num_clients;

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_RGB_OC;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_RGB_OC);
    }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_get_device_config(this,
                           instance,
                           AMS_PROX_STRM,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: PROX: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)chosen_sample_rate,
                            (int32_t)sample_rate);

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_PROX_STRM;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_PROX_STRM);
    }

    ams_get_device_config(this,
                           instance,
                           AMS_PROX_OC,
                           &sample_rate,
                           &report_rate,
                           &client_present,
                           &num_clients);
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: PROX: client_present %d, curr SR %d, recvd SR %d",
                            client_present,
                            (int32_t)chosen_sample_rate,
                            (int32_t)sample_rate);

    if (num_clients > inst_state->prox_info.client_cfg.num_oc_clients_prev
        &&
        num_clients > 1)
    {
      inst_state->prox_info.client_cfg.oc_send_last_event = true;
    }
    else
    {
      inst_state->prox_info.client_cfg.oc_send_last_event = false;
    }
    inst_state->prox_info.client_cfg.num_oc_clients_prev = num_clients;

    if(client_present)
    {
      chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
      chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
      inst_state->publish_sensors |= AMS_PROX_OC;
    }
    else
    {
      inst_state->publish_sensors &= ~(AMS_PROX_OC);
    }
    SNS_PRINTF(LOW, this, "AMS: ams_reval_instance_config: PROX: publish_sensors=0x%04x",
                            inst_state->publish_sensors);
#endif

  if(state->sensor_type & AMS_STREAMERS)
  {
    AMS_PORT_log_Msg_1(AMS_DEBUG, "ams_reval_instance_config: sensor_type %d is a STREAMER, validate the ODR", state->sensor_type);
    //ams_validate_device_odr(instance, &chosen_sample_rate);
  }

  ams_set_inst_config(this,
                      instance,
                      chosen_report_rate,
                      chosen_sample_rate,
                      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);

  if(!inst_state->publish_sensors)
  {
    AMS_PORT_log_Msg(AMS_DEBUG, "ams_reval_instance_config: .publish_sensors == 0, turn rails off");
    ams_turn_rails_off(this);
    inst_state->instance_is_ready_to_configure = false;
  }
}

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void ams_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{

  sns_request config;

  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

/**
 * It extracts each element's version as well as it's value.
 *
 * @return bool True if decoding is successfull else false.
 */
bool
sns_custom_parse_registry_float_array(sns_registry_data_item *reg_item,
                                      pb_buffer_arg  *item_name,
                                      pb_buffer_arg  *item_str_val,
                                      void *parsed_buffer)
{
  bool rv = true;
  pb_custom_float_parse_arg *arg = (pb_custom_float_parse_arg*) parsed_buffer;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_flt && arg->size > *arg->index)
  {
    {
      arg->data_array[*arg->index] = reg_item->flt;
      if (reg_item->has_version) {
        arg->version_array[*arg->index] = reg_item->version;
      }
      (*arg->index)++;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

static void ams_sensor_process_registry_event(sns_sensor *const this,
                                              sns_sensor_event *event)
{
  bool rv = true;
  ams_state *state = (ams_state*)this->state->state;

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

      if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME(".als.config"),
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME(".prox.config"),
                      group_name.buf_len) ||
         0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME(".rgb.config"),
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

          SNS_PRINTF(LOW, this, "Registry read event received "
                     "is_dri:%d, hardware_id:%d ",
                     state->is_dri, state->hardware_id);
          SNS_PRINTF(LOW, this, "resolution_idx:%d, supports_sync_stream:%d ",
                     state->resolution_idx, state->supports_sync_stream);
        }
      }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"),
                           group_name.buf_len))
      {
        float parsed_float_array[ALS_FAC_CAL_NUM] = {0};
        uint32_t version_array[ALS_FAC_CAL_NUM] = {0};
        {
          uint8_t arr_index = 0;

          pb_custom_float_parse_arg arr_arg = {
            .data_array = parsed_float_array,
            .version_array = version_array,
            .index = &arr_index,
            .size = ALS_FAC_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"),
              .parse_func = sns_custom_parse_registry_float_array,
              .parsed_buffer = &arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data.alsCalibrationFactor = (uint16_t)parsed_float_array[0];
          state->cal_version[ALSCALIBRATIONFACTOR_IDX] = version_array[0];
		  if(parsed_float_array[2] != 0)
              state->calibration_data.alsCalibrationLuxTarget = (uint16_t)parsed_float_array[2];

          SNS_PRINTF(LOW, this, "Registry read event for ALS fac_cal calibration received ");
          sns_memscpy(state->calibration_data.deviceName, sizeof(state->calibration_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data.deviceName));

          SNS_PRINTF(LOW, this, "Als Scale: %d, Als Bias: %d",
                               (int32_t)parsed_float_array[0],
                               (int32_t)parsed_float_array[1]);

        }
      }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"),
                           group_name.buf_len))
      {
        float parsed_float_array[PROX_FAC_CAL_NUM] = {0};
        uint32_t version_array[PROX_FAC_CAL_NUM] = {0};
        {
          uint8_t arr_index = 0;

          pb_custom_float_parse_arg arr_arg = {
            .data_array = parsed_float_array,
            .version_array = version_array,
            .index = &arr_index,
            .size = PROX_FAC_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"),
              .parse_func = sns_custom_parse_registry_float_array,
              .parsed_buffer = &arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data.proxHardThreshold = (uint16_t)parsed_float_array[0];
          state->cal_version[PROXHARDTHRESHOLD_IDX] = version_array[0];

          SNS_PRINTF(LOW, this, "Registry read event for PROX fac_cal calibration received ");
          sns_memscpy(state->calibration_data.deviceName, sizeof(state->calibration_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data.deviceName));

          SNS_PRINTF(LOW, this, "Prox near_threshold: %d, Prox far_threshold: %d",
                               (int32_t)parsed_float_array[0],
                               (int32_t)parsed_float_array[1]);

        }
      }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"),
                           group_name.buf_len))
      {
        float fac_cal_float_array[RGB_FAC_CAL_NUM] = {0};
        uint32_t version_array[RGB_FAC_CAL_NUM] = {0};
        {
          uint8_t fac_cal_arr_index = 0;

          pb_custom_float_parse_arg fac_cal_arr_arg = {
            .data_array = fac_cal_float_array,
            .version_array = version_array,
            .index = &fac_cal_arr_index,
            .size = RGB_FAC_CAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"),
              .parse_func = sns_custom_parse_registry_float_array,
              .parsed_buffer = &fac_cal_arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data.alsCoefR = (int32_t)fac_cal_float_array[0];
          state->calibration_data.alsCoefG = (int32_t)fac_cal_float_array[2];
          state->calibration_data.alsCoefB = (int32_t)fac_cal_float_array[4];
          state->cal_version[ALSCOEFR_IDX] = version_array[0];
          state->cal_version[ALSCOEFG_IDX] = version_array[2];
          state->cal_version[ALSCOEFB_IDX] = version_array[4];

          SNS_PRINTF(LOW, this, "Registry read event for RGB fac_cal calibration received ");
          sns_memscpy(state->calibration_data.deviceName, sizeof(state->calibration_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data.deviceName));

          SNS_PRINTF(LOW, this, "RGB r_channel_scale: %d, RGB color_temp_bias: %d",
                               (int32_t)fac_cal_float_array[0],
                               (int32_t)fac_cal_float_array[RGB_FAC_CAL_NUM - 1]);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.target_val"),
                           group_name.buf_len))
      {
        float target_val_float_array[RGB_TARGET_VAL_NUM] = {0};
        uint32_t version_array[RGB_TARGET_VAL_NUM] = {0};
        {
          uint8_t target_val_arr_index = 0;

          pb_custom_float_parse_arg target_val_arr_arg = {
            .data_array = target_val_float_array,
            .version_array = version_array,
            .index = &target_val_arr_index,
            .size = RGB_TARGET_VAL_NUM
          };
          sns_registry_decode_arg arg = {
            .item_group_name = &group_name,
            .parse_info_len = 1,
            .parse_info[0] = {
              .group_name = AMS_REGISTRY_ENTRY_NAME("_platform.rgb.target_val"),
              .parse_func = sns_custom_parse_registry_float_array,
              .parsed_buffer = &target_val_arr_arg
            }
          };

          read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
          read_event.data.items.arg = &arg;

          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
        if(rv)
        {
          state->calibration_data.targetR = (uint32_t)target_val_float_array[0];
          state->calibration_data.targetG = (uint32_t)target_val_float_array[1];
          state->calibration_data.targetB = (uint32_t)target_val_float_array[2];

          SNS_PRINTF(LOW, this, "Registry read event for RGB target_val calibration received ");
          sns_memscpy(state->calibration_data.deviceName, sizeof(state->calibration_data.deviceName),
              STRINGIFY(CONFIG_AMS_PART_NAME), sizeof(state->calibration_data.deviceName));

          SNS_PRINTF(LOW, this, "RGB target r: %d, target g: %d, target b: %d",
                               (int32_t)target_val_float_array[0],
                               (int32_t)target_val_float_array[1],
                               (int32_t)target_val_float_array[2]);
        }
      }
#endif
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.config"),
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

          SNS_PRINTF(LOW, this, "Registry read event received "
                     "bus_type:%d bus_instance:%d slave_control:%d",
                     state->com_port_info.com_config.bus_type,
                     state->com_port_info.com_config.bus_instance,
                     state->com_port_info.com_config.slave_control);

          SNS_PRINTF(LOW, this, "min_bus_speed_KHz :%d max_bus_speed_KHz:%d reg_addr_type:%d",
                               state->com_port_info.com_config.min_bus_speed_KHz,
                               state->com_port_info.com_config.max_bus_speed_KHz,
                               state->com_port_info.com_config.reg_addr_type);

          SNS_PRINTF(LOW, this, "interrupt_num:%d interrupt_pull_type:%d is_chip_pin:%d",
                               state->irq_config.interrupt_num,
                               state->irq_config.interrupt_pull_type,
                               state->irq_config.is_chip_pin);

          SNS_PRINTF(LOW, this, "interrupt_drive_strength:%d interrupt_trigger_type:%d"
             " rigid body type:%d",
             state->irq_config.interrupt_drive_strength,
             state->irq_config.interrupt_trigger_type,
             state->registry_pf_cfg.rigid_body_type);

          //SENSOR_PRINTF_LOW_FULL(this, "num_rail:%d, rail_on_state:%d, vddio_rail:%s, vdd_rail:%s", state->rail_config.num_of_rails,
          //   state->registry_rail_on_state,
          //   state->rail_config.rails[0].name,
          //   state->rail_config.rails[1].name);
        }
      }
      else if(0 == strncmp((char*)group_name.buf, AMS_REGISTRY_ENTRY_NAME("_platform.placement"),
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
          SNS_PRINTF(LOW, this, "Registry read event for placement received "
                 "p[0]:%d p[6]:%d p[11]:%d",
                 (uint32_t)state->placement[0], (uint32_t)state->placement[6],
                 (uint32_t)state->placement[11]);
        }
      }
      else
      {
        rv = false;
      }

      if(!rv)
      {
        //SENSOR_PRINTF_ERROR_FULL(this, "Error decoding registry group %s due to %s", (char*)group_name.buf,
        //                         PB_GET_ERROR(&stream));
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u",
                             event->message_id);
  }
}

static void
ams_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/** See sns_ams_sensor.h */
void ams_process_suid_events(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;

  for(;
      0 != state->fw_stream->api->get_input_cnt(state->fw_stream);
      state->fw_stream->api->get_next_input(state->fw_stream))
  {
    sns_sensor_event *event =
      state->fw_stream->api->peek_input(state->fw_stream);

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

      if(!pb_decode(&stream, sns_suid_event_fields, &suid_event)) {
         SNS_PRINTF(ERROR, this, "AMS: ams_process_suid_events: SUID Decode failed");
         continue;
       }

      /* if no suids found, ignore the event */
      if(suid_search.num_of_suids == 0)
      {
        continue;
      }

      /* save suid based on incoming data type name */
      if(0 == strncmp(data_type_arg.buf, "interrupt", data_type_arg.buf_len))
      {
        state->irq_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "timer", data_type_arg.buf_len))
      {
        state->timer_suid = uid_list;
      }
      else if (0 == strncmp(data_type_arg.buf, "registry", data_type_arg.buf_len))
      {
        state->reg_suid = uid_list;
      }
      else if(0 == strncmp(data_type_arg.buf, "async_com_port", data_type_arg.buf_len))
      {
        SNS_PRINTF(LOW, this, "AMS: ams_process_suid_events: received ASCP SUID");
      }
      else
      {
        SNS_PRINTF(ERROR, this, "AMS: ams_process_suid_events: invalid datatype_name");
      }
    }
  }
}

static void ams_sensor_send_registry_request(sns_sensor *const this,
                                             char *reg_group_name)
{
  ams_state *state = (ams_state*)this->state->state;
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
  }

  //SENSOR_PRINTF_LOW_FULL(this, "Sending registry request for group name:%s", reg_group_name);
}

/** See sns_ams_sensor.h*/
sns_rc ams_sensor_notify_event(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  sns_time on_timestamp;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;

  SNS_PRINTF(HIGH, this, " AMS: ams_sensor_notify_event: enter: this %p, state %p, fw_stream %p, timer_stream %p, pwr_rail_service %p", this, state, state->fw_stream, state->timer_stream, state->pwr_rail_service);

  if(state->fw_stream)
  {
    if((0 == sns_memcmp(&state->irq_suid, &((sns_sensor_uid){{0}}), sizeof(state->irq_suid)))
     || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)))
     || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid)))
    )
    {
      ams_process_suid_events(this);
      // place a request to registry sensor
      if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
      {
        if(state->reg_data_stream == NULL)
        {
          stream_svc->api->create_sensor_stream(stream_svc,
              this, state->reg_suid , &state->reg_data_stream);

          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.placement"));
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".als.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"));
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".prox.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"));
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".rgb.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.target_val"));
#endif
        }
      }
    }
  }

  /**----------------------Handle a Timer Sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: timer_stream");

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
          if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_INIT)
          {
            /**-------------------Read and Confirm WHO-AM-I------------------------*/
            SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: try amsLibrary_open...");
            state->priv = amsLibrary_open(state->port_handle_ptr, &state->calibration_data);
            if (state->priv != NULL){
              SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: amsLibrary_open SUCCESS");
              state->hw_is_present = true;
            } else {
              SNS_PRINTF(ERROR, this, " AMS: ams_sensor_notify_event: amsLibrary_open ERROR");
              /* TODO: we need to free some things here */
              rv = SNS_RC_INVALID_LIBRARY_STATE;
            }

            /**------------------Power Down and Close COM Port--------------------*/
            state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

            state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
            state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
            /* mustn't use the com port any more so make our handles etc. invalid */
            SNS_PRINTF(LOW, this, " AMS: clear AMS_PORT_portHndl");
            state->com_port_info.port_handle = NULL;
            state->port_handle.scp_handle = NULL;
            state->port_handle_ptr = NULL;

            /**----------------------Turn Power Rail OFF--------------------------*/
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     NULL);
            if(state->hw_is_present)
            {
              ams_publish_available(this);
            }
            else
            {
              rv = SNS_RC_INVALID_STATE;
              SNS_PRINTF(LOW, this, " AMS: HW absent");
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: _POWER_RAIL_PENDING_SET_CLIENT_REQ");

            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            SNS_PRINTF(LOW,this, " AMS: ams_sensor_notify_event: shared instance %p", instance);
            if(NULL != instance)
            {
              ams_instance_state *inst_state =
                 (ams_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              ams_reval_instance_config(this, instance, state->sensor_type);
              if(inst_state->new_self_test_request)
              {
                ams_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
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
      ams_sensor_process_registry_event(this, event);
      if(state->registry_cfg_received && state->registry_placement_received)
      {
        ams_publish_registry_attributes(this);
      }

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  /**---------------------Register Power Rails --------------------------*/
  if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))
     && state->registry_pf_cfg_received
     && state->registry_cfg_received
     && state->registry_placement_received)
  {
    state->registry_pf_cfg_received = false;
    /**-----------------Register and Open COM Port-------------------------*/
    if(NULL == state->com_port_info.port_handle)
    {
      SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: .port_handle is null");
      rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                        &state->com_port_info.port_handle);
      SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: .port_handle %p, set AMS_PORT_portHndl", state->com_port_info.port_handle);

      if(SNS_RC_SUCCESS == rv)
      {
        rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
        if(SNS_RC_SUCCESS == rv)
        {
          /* set our wrapped-up handle for this port */
          state->port_handle.scp_handle = state->com_port_info.port_handle;
          state->port_handle_ptr = &state->port_handle;
        }
      }
    }
    SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: register power rails");
    state->rail_config.rail_vote = SNS_RAIL_OFF;

    state->pwr_rail_service =
     (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                     SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->rail_config);

    /**---------------------Turn Power Rails ON----------------------------*/
    state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
    SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: .rail_vote %d", state->rail_config.rail_vote);
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                             this,
                                                             &state->rail_config,
                                                             &on_timestamp);

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    if(NULL == state->timer_stream)
    {
      SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: .timer_stream is null");
      stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                            &state->timer_stream);
      SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: .timer_stream %p", state->timer_stream);
      if(NULL != state->timer_stream)
      {
        SNS_PRINTF(LOW, this, " AMS: ams_sensor_notify_event: power rail timer start");
        ams_start_power_rail_timer(this,
                                  sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS * 1000 * 1000),
                                  AMS_POWER_RAIL_PENDING_INIT);
      }
    }
  }
  return rv;
}

sns_rc ams_streaming_sensor_notify_event(sns_sensor *const this)
{
  ams_state *state = (ams_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  sns_time on_timestamp;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;

  SNS_PRINTF(LOW, this, " AMS: ams_streaming_sensor_notify_event: enter: this %p, state %p, fw_stream %p, timer_stream %p, pwr_rail_service %p",
             this, state, state->fw_stream, state->timer_stream, state->pwr_rail_service);

  if(state->fw_stream)
  {
    if((0 == sns_memcmp(&state->irq_suid, &((sns_sensor_uid){{0}}), sizeof(state->irq_suid)))
     || (0 == sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid)))
     || (0 == sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid)))
    )
    {
      ams_process_suid_events(this);
            // place a request to registry sensor
      if((0 != sns_memcmp(&state->reg_suid, &((sns_sensor_uid){{0}}), sizeof(state->reg_suid))))
      {
        if(state->reg_data_stream == NULL)
        {
          stream_svc->api->create_sensor_stream(stream_svc,
              this, state->reg_suid , &state->reg_data_stream);

          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.placement"));
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".als.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"));
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".prox.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"));
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME(".rgb.config"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"));
          ams_sensor_send_registry_request(this, AMS_REGISTRY_ENTRY_NAME("_platform.rgb.target_val"));
#endif
        }
      }
    }
  }

  /**----------------------Handle a Timer Sensor event.-------------------*/
  if(NULL != state->timer_stream)
  {
    SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: timer_stream");

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
          if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_INIT)
          {
            /**-------------------Read and Confirm WHO-AM-I------------------------*/
            SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: try amsLibrary_open...");
            state->priv = amsLibrary_open(state->port_handle_ptr, &state->calibration_data);
            if (state->priv != NULL){
              SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: amsLibrary_open SUCCESS");
              state->hw_is_present = true;
            } else {
              SNS_PRINTF(ERROR, this, " AMS: ams_streaming_sensor_notify_event: amsLibrary_open ERROR");
              /* TODO: we need to free some things here */
              rv = SNS_RC_INVALID_LIBRARY_STATE;
            }

            /**------------------Power Down and Close COM Port--------------------*/
            state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

            state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
            state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
            /* mustn't use the com port any more so make our handles etc. invalid */
            SNS_PRINTF(LOW, this, " AMS: clear AMS_PORT_portHndl");
            state->com_port_info.port_handle = NULL;
            state->port_handle.scp_handle = NULL;
            state->port_handle_ptr = NULL;

            /**----------------------Turn Power Rail OFF--------------------------*/
            state->rail_config.rail_vote = SNS_RAIL_OFF;
            state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                                     this,
                                                                     &state->rail_config,
                                                                     NULL);
            if(state->hw_is_present)
            {
              ams_publish_available(this);
            }
            else
            {
              rv = SNS_RC_INVALID_STATE;
              SNS_PRINTF(LOW, this, " AMS: HW absent");
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: _POWER_RAIL_PENDING_SET_CLIENT_REQ");

            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            SNS_PRINTF(MED,this, " AMS: ams_streaming_sensor_notify_event: shared instance %p", instance);
            if(NULL != instance)
            {
              ams_instance_state *inst_state =
                 (ams_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              ams_reval_instance_config(this, instance, state->sensor_type);
              if(inst_state->new_self_test_request)
              {
                ams_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = AMS_POWER_RAIL_PENDING_NONE;
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
      ams_sensor_process_registry_event(this, event);
      if(state->registry_cfg_received && state->registry_placement_received)
      {
        ams_publish_registry_attributes(this);
      }

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  /**---------------------Register Power Rails --------------------------*/
  if(0 != sns_memcmp(&state->timer_suid, &((sns_sensor_uid){{0}}), sizeof(state->timer_suid))
     && state->registry_pf_cfg_received
     && state->registry_cfg_received
     && state->registry_placement_received)
  {
    state->registry_pf_cfg_received = false;
    /**-----------------Register and Open COM Port-------------------------*/
    if(NULL == state->com_port_info.port_handle)
    {
      SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .port_handle is null");
      rv = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                        &state->com_port_info.port_handle);
      SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .port_handle %p, set AMS_PORT_portHndl", state->com_port_info.port_handle);

      if(rv == SNS_RC_SUCCESS)
      {
        rv = state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
        /* set our wrapped-up handle for this port */
        state->port_handle.scp_handle = state->com_port_info.port_handle;
        state->port_handle_ptr = &state->port_handle;
      }
    }
    SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: register power rails");
    state->rail_config.rail_vote = SNS_RAIL_OFF;

    state->pwr_rail_service =
     (sns_pwr_rail_service*)service_mgr->get_service(service_mgr,
                                                     SNS_POWER_RAIL_SERVICE);

    state->pwr_rail_service->api->sns_register_power_rails(state->pwr_rail_service,
                                                           &state->rail_config);

    /**---------------------Turn Power Rails ON----------------------------*/
    state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
    SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .rail_vote %d", state->rail_config.rail_vote);
    state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                             this,
                                                             &state->rail_config,
                                                             &on_timestamp);

    /**-------------Create a Timer stream for Power Rail ON timeout.---------*/
    if(NULL == state->timer_stream)
    {
      SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .timer_stream is null");
      stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                            &state->timer_stream);
      SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: .timer_stream %p", state->timer_stream);
      if(NULL != state->timer_stream)
      {
        SNS_PRINTF(MED, this, " AMS: ams_streaming_sensor_notify_event: power rail timer start");
        ams_start_power_rail_timer(this,
                                  sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS * 1000 * 1000),
                                  AMS_POWER_RAIL_PENDING_INIT);
      }
    }
  }
  return rv;
}

static bool
write_to_rgb_fac_cal(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_sensor *this = (sns_sensor*) *arg;

  ams_state *state = (ams_state *)this->state->state;
  ams_calibrationData_t *cfg = &state->calibration_data;
  uint32_t *new_version = state->cal_version;

  char const *names[] = {"r_channel_scale",
                         "g_channel_scale",
                         "b_channel_scale"
  };
  //TODO: add suport for far_threshold

  for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1 };
         /* QC - consider using sizeof() instead of strlen() + 1. Comment applies to all such usage through driver.*/
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;

    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    /* QC - the if check can be converted into a macro */
    if (0==strncmp(names[i], "r_channel_scale", sizeof("r_channel_scale")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[ALSCOEFR_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->alsCoefR;
    }
    else if (0==strncmp(names[i], "g_channel_scale", sizeof("g_channel_scale")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[ALSCOEFG_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->alsCoefG;
    }
    else if (0==strncmp(names[i], "b_channel_scale", sizeof("b_channel_scale")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[ALSCOEFB_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->alsCoefB;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
      return false;
    }
  }
  SNS_PRINTF(LOW, this, "AMS: sns_ams_encode_calibration_registry_cb: done");
  return true;
}

static bool
write_to_prox_fac_cal(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_sensor *this = (sns_sensor*) *arg;

  ams_state *state = (ams_state *)this->state->state;
  ams_calibrationData_t *cfg = &state->calibration_data;
  uint32_t *new_version = state->cal_version;

  char const *names[] = {"near_threshold"};
  //TODO: add suport for far_threshold

  for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1};
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;

    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    /* QC - the if check can be converted into a macro */
    if (0==strncmp(names[i], "near_threshold", sizeof("near_threshold")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[PROXHARDTHRESHOLD_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->proxHardThreshold;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
      return false;
    }
  }
  SNS_PRINTF(LOW, this, "AMS: sns_ams_encode_calibration_registry_cb: done");
  return true;
}

static bool
write_to_als_fac_cal(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_sensor *this = (sns_sensor*) *arg;

  ams_state *state = (ams_state *)this->state->state;
  ams_calibrationData_t *cfg = &state->calibration_data;
  uint32_t *new_version = state->cal_version;

  char const *names[] = {"scale"};

  for(int i = 0; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
           { .buf = names[i], .buf_len = strlen(names[i]) + 1};
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;

    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    /* QC - the if check can be converted into a macro */
    if (0==strncmp(names[i], "scale", sizeof("scale")))
    {
      pb_item.has_version = true;
      pb_item.version = new_version[ALSCALIBRATIONFACTOR_IDX];
      pb_item.has_flt = true;
      pb_item.flt = (float)cfg->alsCalibrationFactor;
    }
    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, this,"Error encoding tag item: %d", i);

      return false;
    }

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_PRINTF(ERROR, this,"Error encoding item: %d", i);
      return false;
    }
  }
  SNS_PRINTF(LOW, this, "AMS: sns_ams_encode_calibration_registry_cb: done");
  return true;
}

bool
write_calibration_to_registry(sns_sensor *const this)
{
  //todo how much to define for this buffer
  uint8_t buffer[1000];
  int32_t encoded_len;
  ams_state *state =  (ams_state *) this->state->state;
  sns_registry_write_req write_req = sns_registry_write_req_init_default;

  /* names[] and encoder[] arrays must be kept in sync */
  const char *names[] = { AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"),
                          AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"),
                          AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal")
  };

  for (int i = 0 ; i < sizeof(names)/sizeof(names[0]) ; i++)
  {
    pb_buffer_arg name_data = (pb_buffer_arg)
          { .buf = names[i],
            .buf_len = strlen (names[i]) + 1 };

    write_req.name.funcs.encode = &pb_encode_string_cb;
    write_req.name.arg = &name_data;
    write_req.data.items.arg = this;

    if (0 == strncmp(names[i],
          AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"),
          sizeof(AMS_REGISTRY_ENTRY_NAME("_platform.als.fac_cal"))))
    {
      write_req.data.items.funcs.encode = write_to_als_fac_cal;
    }
    else if (0 == strncmp(names[i],
          AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"),
          sizeof(AMS_REGISTRY_ENTRY_NAME("_platform.prox.fac_cal"))))
    {
      write_req.data.items.funcs.encode = write_to_prox_fac_cal;
    }
    else if (0 == strncmp(names[i],
          AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"),
          sizeof(AMS_REGISTRY_ENTRY_NAME("_platform.rgb.fac_cal"))))
    {
      write_req.data.items.funcs.encode = write_to_rgb_fac_cal;
    }

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
        &write_req, sns_registry_write_req_fields, NULL);
    if(0 < encoded_len)
    {
      sns_request request = (sns_request){
        .request_len = encoded_len, .request = buffer,
            .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ };
      state->reg_data_stream->api->send_request(state->reg_data_stream, &request);
    } else {
      return false;
    }
  }

  return true;
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
static bool ams_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
                                                  sns_std_request *decoded_request,
                                                  sns_physical_sensor_test_config *test_config)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = test_config,
        .fields = sns_physical_sensor_test_config_fields };
#ifndef ENABLE_DEBUG
  UNUSED_VAR(this)
#endif
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(request->request,
                                  request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "ams decode error");
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
static bool ams_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request = sns_std_request_init_default;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  ams_state *state = (ams_state*)this->state->state;
  ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
  ams_self_test_info *self_test_info;

  if (0) { }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if (state->sensor_type == AMS_ALS_OC) {
    self_test_info = &inst_state->als_info.test_info[AMS_OC_IDX];
  } else if (state->sensor_type == AMS_ALS_STRM) {
    self_test_info = &inst_state->als_info.test_info[AMS_STRM_IDX];
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  else if (state->sensor_type == AMS_PROX_OC) {
    self_test_info = &inst_state->prox_info.test_info[AMS_OC_IDX];
  } else if (state->sensor_type == AMS_PROX_STRM) {
    self_test_info = &inst_state->prox_info.test_info[AMS_STRM_IDX];
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  else if (state->sensor_type == AMS_RGB_OC) {
    self_test_info = &inst_state->rgb_info.test_info[AMS_OC_IDX];
  } else if (state->sensor_type == AMS_RGB_STRM) {
    self_test_info = &inst_state->rgb_info.test_info[AMS_STRM_IDX];
  }
#endif
  else
  {
    SNS_PRINTF(LOW, this, "AMS: ams_extract_self_test_info: Error: unknown sensor type");
    return false;
  }

  if(ams_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
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

void ams_send_suid_req(sns_sensor *this, char *const data_type,
    uint32_t data_type_len)
{
  uint8_t buffer[50];
  sns_memset(buffer, 0, sizeof(buffer));
  ams_state *state = (ams_state*)this->state->state;
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
     stream_service->api->create_sensor_stream(stream_service,
         this, sns_get_suid_lookup(), &state->fw_stream);
  }

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &suid_req, sns_suid_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer, .message_id = SNS_SUID_MSGID_SNS_SUID_REQ };
    state->fw_stream->api->send_request(state->fw_stream, &request);
  }
}

/** See sns_ams_sensor.h */
sns_sensor_instance* ams_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  ams_state *state = (ams_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  SNS_PRINTF(LOW, this,
             "AMS: ams_set_client_request: enter: sensor_this 0x%X, exist_rq 0x%X, new_rq 0x%X, rmv=%d; inst 0x%X"
             , this, exist_request, new_request, remove, instance);

  if(remove)
  {
    SNS_PRINTF(LOW, this, "AMS: ams_set_client_request:  remove rqst; instance 0x%X", instance);
    if(NULL != instance)
    {
      ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
      instance->cb->remove_client_request(instance, exist_request);

      if (exist_request->message_id !=
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        inst_state->client_present--;
      }

      ams_reval_instance_config(this, instance, state->sensor_type);
      inst_state->new_or_chng_req = false;
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
       state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
       state->pwr_rail_service->api->sns_vote_power_rail_update(
                                            state->pwr_rail_service,
                                            this,
                                            &state->rail_config,
                                            &on_timestamp);

       delta = sns_get_system_time() - on_timestamp;

       // Use on_timestamp to determine correct Timer value.
       if(delta < sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS*1000*1000))
       {
         ams_start_power_rail_timer(this,
                                   sns_convert_ns_to_ticks(AMS_DEVICE_OFF_TO_IDLE_MS*1000*1000) - delta,
                                   AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ);
       }
       else
       {
         // rail is already ON
         reval_config = true;
       }

       SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: null instance, call create_instance");

       /** create_instance() calls init() for the Sensor Instance */
       instance = this->cb->create_instance(this,
                                            sizeof(ams_instance_state));
       ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
       inst_state->new_or_chng_req = true;
       /* If rail is already ON then flag instance OK to configure */
       if(reval_config)
       {
         inst_state->instance_is_ready_to_configure = true;
       }
     }
     else
     {
       if(new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
       {
          ams_send_fifo_flush_done (instance, &state->my_suid);
          return instance;
       }
       else
       {
         reval_config = true;

         SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: instance %p; new or chg rqst", instance);
		 
		 ams_instance_state *inst_state = (ams_instance_state *) instance->state->state;
         inst_state->new_or_chng_req = true;

         /** An existing client is changing request*/
         if((NULL != exist_request) && (NULL != new_request))
         {
           SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: chg rqst");
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
        ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
        if((NULL != new_request) && (new_request->message_id != SNS_STD_MSGID_SNS_STD_FLUSH_REQ))
        {
          SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: adding new request");
          instance->cb->add_client_request(instance, new_request);
          if(new_request->message_id ==
             SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
          {
            if(ams_extract_self_test_info(this, instance, new_request))
            {
              SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: is selftest rqst; reval_config=%d", reval_config);
              inst_state->new_self_test_request = true;
              inst_state->sensor_type_under_test = state->sensor_type;
            }
          } else {
            inst_state->client_present++;
          }
        }
        if(reval_config && inst_state->instance_is_ready_to_configure)
        {
          SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: call ams_reval_instance_config");
          ams_reval_instance_config(this, instance, state->sensor_type);
          if(inst_state->new_self_test_request)
          {
            ams_set_self_test_inst_config(this, instance);
          }
        }
     }
  }
  }
  else // bad request
  {
    instance = NULL;
  }

  /* Sensors are required to call remove_instance when clientless */
  if(NULL != instance
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
     &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)ALS_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)ALS_STREAMING_SUID, true)
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
     &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)RGB_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)RGB_STREAMING_SUID, true)
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
     &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)PROX_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)PROX_STREAMING_SUID, true)
#endif
    )
  {
    sns_sensor *iter;
    ams_instance_state *inst_state = (ams_instance_state*)instance->state->state;
    for(iter = this->cb->get_library_sensor(this, true);
        NULL != iter;
        iter = this->cb->get_library_sensor(this, false))
    {
      ams_state *sensor_state = (ams_state*)iter->state->state;

      sensor_state->calibration_data = inst_state->calibration_data;
      sns_memscpy(sensor_state->cal_version,
                  sizeof(sensor_state->cal_version),
                  inst_state->cal_version,
                  sizeof(inst_state->cal_version));
    }

    write_calibration_to_registry(this);

    SNS_PRINTF(LOW, this, "AMS: ams_set_client_request: no requests left, call cb->remove_instance");
    this->cb->remove_instance(instance);
  }

  return instance;
}

