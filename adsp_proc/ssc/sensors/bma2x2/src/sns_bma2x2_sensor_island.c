/*******************************************************************************
 * Copyright (c) 2018, Bosch Sensortec GmbH
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
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "sns_bma2x2_hal.h"
#include "sns_bma2x2_sensor.h"
#include "sns_bma2x2_sensor_instance.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_motion_detect.pb.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_registry.pb.h"


// <sensor API>
static sns_sensor_uid const* bma2x2_accel_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = BMA2X2_ACCEL_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* bma2x2_motion_detect_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid =  BMA2X2_MOTION_DETECT_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* bma2x2_sensor_temp_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = BMA2X2_SENSOR_TEMPERATURE_SUID;

  return &sensor_uid;
}
// </sensor API>


// <power rail>
#if BMA2X2_CONFIG_ENABLE_POWER_RAIL
void bma2x2_start_power_rail_timer(sns_sensor *const this,
                                           sns_time timeout_ticks,
                                           bma2x2_power_rail_pending_state pwr_rail_pend_state)
{
  bma2x2_state *state = (bma2x2_state*)this->state->state;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[20];
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
}
#endif
// </power rail>

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */

void bma2x2_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{

  sns_request config;

  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

/** See sns_bma2x2_sensor.h*/
sns_rc bma2x2_sensor_notify_event(sns_sensor *const this)
{
  bma2x2_state *state = (bma2x2_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;

  SENSOR_DEBUG(MED, this, "<bma2x2_sif__> sensor notify event @%d", state->sensor);

  if(!sns_suid_lookup_complete(&state->common.suid_lookup_data))
  {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
    state->island_service->api->sensor_island_exit(state->island_service, this);
#endif
    sns_suid_lookup_handle(this, &state->common.suid_lookup_data);

#if BMA2X2_CONFIG_ENABLE_REGISTRY
    if(sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", NULL))
    {
      bma2x2_request_registry(this);
    }
#else
    sns_bma2x2_registry_def_config(this);
    SENSOR_DEBUG(MED, this, "bus_type:%d, reg addr type:%d, sc:0x%x",
                  state->common.com_port_info.com_config.bus_type,
                  state->common.com_port_info.com_config.reg_addr_type,
                  state->common.com_port_info.com_config.slave_control);
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
      SENSOR_DEBUG(MED, this, "timer event msg:%d", event->message_id);
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;

      if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == BMA2X2_POWER_RAIL_PENDING_INIT)
          {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
            /** Initial HW discovery is OK to run in normal mode. */
            state->island_service->api->sensor_island_exit(state->island_service, this);
#endif

            state->common.hw_is_present = bma2x2_discover_hw(this);

            if(state->common.hw_is_present)
            {
              bma2x2_publish_available(this);
              bma2x2_update_sibling_sensors(this);
            }
            else
            {
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              SENSOR_DEBUG(ERROR, this, "BMA2X2 HW absent");
            }
            state->power_rail_pend_state = BMA2X2_POWER_RAIL_PENDING_NONE;
            SENSOR_DEBUG(MED, this, "s:%d initialize finish", state->sensor);
          }
          else if(state->power_rail_pend_state == BMA2X2_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            if(NULL != instance)
            {
              bma2x2_instance_state *inst_state =
                 (bma2x2_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              bma2x2_reval_instance_config(this, instance, state->sensor);
              if(inst_state->new_self_test_request)
              {
                bma2x2_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = BMA2X2_POWER_RAIL_PENDING_NONE;
          }
        }
        else
        {
          SENSOR_DEBUG(ERROR, this, "pb_decode error");
        }
      }
      event = state->timer_stream->api->get_next_input(state->timer_stream);
    }
    /** Free up timer stream if not needed anymore */
    if(state->power_rail_pend_state == BMA2X2_POWER_RAIL_PENDING_NONE)
    {
      sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
    }
  }

#if BMA2X2_CONFIG_ENABLE_REGISTRY
  if(NULL != state->reg_data_stream)
  {
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
      /** All registry events can be handled in normal mode. */
      state->island_service->api->sensor_island_exit(state->island_service, this);
#endif
      bma2x2_sensor_process_registry_event(this, event);

      // resolution validation
      bma2x2_validate_resolution(this);

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }
#endif

  if(state->common.registry_pf_cfg_received && state->common.registry_cfg_received &&
     state->common.registry_orient_received && state->common.registry_placement_received)
  {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
    /** Initial HW detection sequence is OK to run in normal mode. */
    state->island_service->api->sensor_island_exit(state->island_service, this);
#endif

    if (sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL))
    {
      bma2x2_start_hw_detect_sequence(this);
    }
  }
  return rv;
}


static void bma2x2_send_flush_config(sns_sensor *const this,
                                       sns_sensor_instance *instance)
{
  sns_request config;

  config.message_id = SNS_STD_MSGID_SNS_STD_FLUSH_REQ;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}



/**
 * Returns decoded request message for type
 * sns_std_sensor_config.
 *
 * @param[in] in_request   Request as sorted in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool bma2x2_get_decoded_imu_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_std_sensor_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, this, "LSM decode error");
    return false;
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
 * @return bool True if decoding is successful else false.
 */
static bool bma2x2_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
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


/**
 * Parses through all standard streaming requests and deduces
 * best HW config for the internal sensor type.
 *
 * @param[i] this             Sensor reference
 * @param[i] instance         Instance reference
 * @param[i] sensor_type      sensor type
 * @param[o] chosen_sample_rate   chosen sample rate in Hz
 * @param[o] chosen_report_rate   chosen report rate in Hz
 * @param[o] chosen_flush_period_ticks   chosen flush period in ticks
 * @param[o] non_gated_sensor_client_present  True if non-gated
 *       client is present.
 * @param[o] gated_sensor_client_present  True if gated client is
 *       present.
 */
static void bma2x2_get_acc_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   float *chosen_sample_rate,
                                   float *chosen_report_rate,
                                   sns_time *chosen_flush_period_ticks,
                                   bool *non_gated_sensor_client_present,
                                   bool *gated_sensor_client_present)
{
  UNUSED_VAR(this);
  sns_sensor_uid suid;
  sns_request const *request;
  bool is_max_batch = true;
  bool is_flush_only = true;

  sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)BMA2X2_ACCEL_SUID), sizeof(sns_sensor_uid));

  *chosen_report_rate = 0.0f;
  *chosen_sample_rate = 0.0f;
  *non_gated_sensor_client_present = false;
  *chosen_flush_period_ticks = 0;
  if(gated_sensor_client_present)
  {
    *gated_sensor_client_present = false;
  }

  /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
       ||
       request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(bma2x2_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate = 0.0f;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);

        if(decoded_request.has_batching)
        {
          is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);
          is_flush_only &= decoded_request.batching.flush_only;

          if(decoded_request.batching.has_flush_period)
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks(decoded_request.batching.flush_period*1000));
          }
          else
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks(decoded_request.batching.batch_period*1000));
          }
        }
        else
        {
          *chosen_flush_period_ticks = UINT64_MAX;
          is_max_batch = false;
          is_flush_only = false;
        }

        if(decoded_request.has_batching
           &&
           decoded_request.batching.batch_period > 0)
        {
          report_rate = (1000000.0f / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = decoded_payload.sample_rate;
        }

        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);

        if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
        {
          *non_gated_sensor_client_present = true;
        }
        else
        {
          if(gated_sensor_client_present)
          {
            *gated_sensor_client_present = true;
          }
        }
      }
    }
  }

  /** If there is a client for the sensor and
   *  if max_batch is set in all requests then choose the largest WM. */
  if(is_max_batch)
  {
    if(*non_gated_sensor_client_present ||
       (gated_sensor_client_present && *gated_sensor_client_present))
    {
    *chosen_report_rate = (1.0f / UINT32_MAX);
    }
  }

  SENSOR_DEBUG(LOW, this, "is_flush_only %u is_max_batch %d",
             is_flush_only, is_max_batch);
}


/**
 * Determines motion detect config.
 *
 * @param[i] this   Sensor reference
 * @param[i] instance  Sensor Instance reference
 * @param[o] chosen_md_enable  True if MD should be enabled
 * @param[o] md_client_present True if there is an MD client
 *       present.
 *
 * @return none
 */
static void bma2x2_get_motion_detect_config(sns_sensor *this,
                                             sns_sensor_instance *instance,
                                             bool *chosen_md_enable,
                                             bool *md_client_present)
{
  sns_sensor_uid suid = BMA2X2_MOTION_DETECT_SUID;
  bma2x2_instance_state *inst_state =
     (bma2x2_instance_state*)instance->state->state;
  sns_request const *request;
  UNUSED_VAR(this);

  *chosen_md_enable = false;
  *md_client_present = false;

  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
      // Enable MD interrupt when:
      // 1. There is a new request and MD is in MDF/MDD state OR
      // 2. There is an existing request and MD is in MDE/MDD state
      // Introduced for power measurement testing,
      // Disable md interrupt using registry setting and send Disable event to md client,
      // if disable flag is true and client is present
      if(!inst_state->md_info.md_config.disable)
      {
        *chosen_md_enable = true;
      }

      *md_client_present = true;
      // Consumed new request
      inst_state->md_info.md_new_req = false;
      return;
    }
  }
}




/**
 * Parses through standard streaming requests for the sensor
 * temperature dataype and deduces best HW config.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Intance reference
 * @param[o] chosen_sample_rate  chosen sample rate in Hz
 * @param[o] chosen_report_rate  chosen report rate in Hz
 * @param[o] sensor_temp_client_present  True if there is a
 *       sensor temp client present
 *
 * @return none
 */
static void bma2x2_get_sensor_temp_config(sns_sensor *this,
                                           sns_sensor_instance *instance,
                                           float *chosen_sample_rate,
                                           float *chosen_report_rate,
                                           sns_time *chosen_flush_period_ticks,
                                           bool *sensor_temp_client_present)
{
  UNUSED_VAR(this);
  bma2x2_instance_state *inst_state =
     (bma2x2_instance_state*)instance->state->state;
  sns_sensor_uid suid = BMA2X2_SENSOR_TEMPERATURE_SUID;
  sns_request const *request;
  bool is_max_batch = true;
  bool is_flush_only = true;

  *chosen_report_rate = 0.0f;
  *chosen_sample_rate = 0.0f;
  *sensor_temp_client_present = false;
  *chosen_flush_period_ticks = 0;

    /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(bma2x2_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate = 0.0f;
        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
                                       decoded_payload.sample_rate);

        if(decoded_request.has_batching
           &&
           decoded_request.batching.batch_period > 0)
        {
          report_rate = (1000000.0 / (float)decoded_request.batching.batch_period);
        }
        else
        {
          report_rate = decoded_payload.sample_rate;
        }

        if(decoded_request.has_batching)
        {
          is_max_batch &= (decoded_request.batching.has_max_batch && decoded_request.batching.max_batch);
          is_flush_only &= (decoded_request.batching.flush_only);

          if(decoded_request.batching.has_flush_period)
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks(decoded_request.batching.flush_period*1000));
          }
          else
          {
            *chosen_flush_period_ticks =
              SNS_MAX(*chosen_flush_period_ticks,
                      sns_convert_ns_to_ticks(decoded_request.batching.batch_period*1000));
          }
        }
        else
        {
          *chosen_flush_period_ticks = UINT64_MAX;
          is_max_batch = false;
          is_flush_only = false;
        }

        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);
        *sensor_temp_client_present = true;
      }
    }
  }

  if(*sensor_temp_client_present &&
     (is_max_batch))
    {
      *chosen_report_rate = (1.0f / UINT32_MAX);
    }

  inst_state->sensor_temp_info.report_rate_hz  = *chosen_report_rate;
  inst_state->sensor_temp_info.sampling_rate_hz = *chosen_sample_rate;
}

/**
 * Set standard streaming config for the instance.
 *
 * @param[i] this        Sensor reference
 * @param[i] instance    Sensor Instance reference
 * @param[i] chosen_report_rate   chosen report rate in Hz
 * @param[i] chosen_sample_rate   chosen sample rate in Hz
 * @param[i] chosen_flush_period_ticks chosen flush period in tick
 * @param[i] registry_cfg Sensor specific registry configuration
 * @param[i] message_id           input message ID
 *
 * @return none
 */
static void bma2x2_set_inst_config(sns_sensor *this,
                                    sns_sensor_instance *instance,
                                    float chosen_report_rate,
                                    float chosen_sample_rate,
                                    sns_time chosen_flush_period_ticks,
                                    sns_bma2x2_registry_cfg registry_cfg,
                                    uint32_t message_id)
{
  sns_bma2x2_req new_client_config;
  sns_request config;
  bma2x2_state *sstate = (bma2x2_state *)this->state->state;

  new_client_config.desired_report_rate = chosen_report_rate;
  new_client_config.desired_sample_rate = chosen_sample_rate;
  new_client_config.registry_cfg        = registry_cfg;
  new_client_config.hw_ver              = sstate->common.hw_ver;
  new_client_config.desired_flush_ticks = chosen_flush_period_ticks;


  config.message_id = message_id;
  config.request_len = sizeof(sns_bma2x2_req);
  config.request = &new_client_config;

  this->instance_api->set_client_config(instance, &config);
}

/**
 * Copy sensor specfic registry info into instance state (Ex: md_config)
 *
 * @param[i] this        Sensor reference
 * @param[i] inst_state  Sensor Instance state reference
 *
 * @return None
 */
static void bma2x2_save_sensor_specific_registry_info_into_instance_state(
  sns_sensor *const this,bma2x2_instance_state *inst_state)
{

  sns_sensor *sensor = NULL;
  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    bma2x2_state *sensor_state = (bma2x2_state*)sensor->state->state;
    /*Save md sensor registry config into instance state */
    if(BMA2X2_MOTION_DETECT == sensor_state->sensor)
    {
      sns_memscpy(&inst_state->md_info.md_config, sizeof(inst_state->md_info.md_config),
      &sensor_state->md_config, sizeof(sensor_state->md_config));
    break;
    }
  }
}


#if BMA2X2_CONFIG_ENABLE_POWER_RAIL
/**
 * Turns power rails off
 *
 * @paramp[i] this   Sensor reference
 *
 * @return none
 */
static void bma2x2_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    bma2x2_state *sensor_state = (bma2x2_state*)sensor->state->state;
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
// </power rail>

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
static bool bma2x2_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  bma2x2_state *state = (bma2x2_state*)this->state->state;
  bma2x2_instance_state *inst_state = (bma2x2_instance_state*)instance->state->state;
  bma2x2_self_test_info *self_test_info;

  if(state->sensor == BMA2X2_ACCEL)
  {
    self_test_info = &inst_state->accel_info.test_info;
  }
  else if(state->sensor == BMA2X2_MOTION_DETECT)
  {
    self_test_info = &inst_state->md_info.test_info;
  }
  else if(state->sensor == BMA2X2_SENSOR_TEMP)
  {
    self_test_info = &inst_state->sensor_temp_info.test_info;
  }
  else
  {
    return false;
  }

  if(bma2x2_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
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

/** See sns_bma2x2_sensor.h */
void bma2x2_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   bma2x2_sensor_type sensor_type)
{
  /**
   * 1. Get best non-gated and gated Accel Config.
   * 3. Get Motion Detect Config.
   * 4. Get best Sensor Temperature Config.
   * 5. Decide best Instance Config based on above outputs.
   */
  float chosen_sample_rate = 0.0f;
  float chosen_report_rate = 0.0f;
  sns_time chosen_flush_period_ticks = 0;
  float sample_rate = 0.0f;
  float report_rate = 0.0f;
  sns_time flush_period_ticks = 0;
  bool a_sensor_client_present_non_gated;
  bool a_sensor_client_present_gated;
  bool md_sensor_client_present;
  bool sensor_temp_client_present;
  bma2x2_instance_state *inst_state =
     (bma2x2_instance_state*)instance->state->state;
  bma2x2_state *state = (bma2x2_state*)this->state->state;
  sns_bma2x2_registry_cfg registry_cfg = {.sensor_type = sensor_type};

  // accel
  bma2x2_get_acc_config(this, instance, &chosen_sample_rate,
            &chosen_report_rate, &chosen_flush_period_ticks,
            &a_sensor_client_present_non_gated, &a_sensor_client_present_gated);

  inst_state->accel_info.gated_client_present = a_sensor_client_present_gated;

  INST_DEBUG(MED, instance, "@ss acc rr(100): %d sr: %d scl_p: %u gated_present %u",
             (uint32_t)(chosen_report_rate * 100), (uint32_t)chosen_sample_rate,
             (uint32_t)chosen_flush_period_ticks, a_sensor_client_present_gated);

  chosen_flush_period_ticks = SNS_MAX(chosen_flush_period_ticks, flush_period_ticks);
  inst_state->accel_info.sample_rate_req = chosen_sample_rate;
  inst_state->accel_info.report_rate_req = chosen_report_rate;

  // motion detection
  bma2x2_get_motion_detect_config(this, instance,
            &inst_state->md_info.enable_md_int, &md_sensor_client_present);

  inst_state->md_info.md_client_present = md_sensor_client_present;

  if(a_sensor_client_present_non_gated)
  {
    inst_state->md_info.enable_md_int = false;
  }

  if(chosen_report_rate > 0.0 && chosen_sample_rate > 0.0)
  {
    uint32_t wm = (uint32_t)(chosen_sample_rate / chosen_report_rate);
    inst_state->fifo_info.max_requested_wmk = SNS_MAX(1, wm);
  }

  bma2x2_get_sensor_temp_config(this, instance, &sample_rate, &report_rate,
             &flush_period_ticks, &sensor_temp_client_present);
  inst_state->sensor_temp_info.max_requested_flush_ticks = flush_period_ticks;

  INST_DEBUG(MED, instance, "@ss temp rr:%d sr: %d f_p %u",
             (uint32_t)report_rate, (uint32_t)sample_rate,
             (uint32_t)flush_period_ticks);

  chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
  chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);

  /** Start with a clean slate */
  inst_state->fifo_info.fifo_enabled = 0;
  inst_state->fifo_info.publish_sensors = 0;

  if(a_sensor_client_present_non_gated)
  {
    inst_state->fifo_info.fifo_enabled |= BMA2X2_ACCEL;
    inst_state->fifo_info.publish_sensors |= BMA2X2_ACCEL;
  }

  if(a_sensor_client_present_gated)
  {
    inst_state->fifo_info.fifo_enabled |= BMA2X2_ACCEL;
    if(!inst_state->md_info.enable_md_int)
    {
      inst_state->fifo_info.publish_sensors |= BMA2X2_ACCEL;
    }
  }


  if(md_sensor_client_present)
  {
    inst_state->fifo_info.fifo_enabled |= BMA2X2_ACCEL;
  }

  if(sensor_temp_client_present)
  {
    inst_state->fifo_info.fifo_enabled |= BMA2X2_ACCEL;
    inst_state->fifo_info.publish_sensors |= BMA2X2_SENSOR_TEMP;
  }

  if(BMA2X2_ACCEL == sensor_type ||
     BMA2X2_SENSOR_TEMP == sensor_type)
  {
    sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
                state->fac_cal_bias, sizeof(state->fac_cal_bias));

    sns_memscpy(&registry_cfg.fac_cal_corr_mat, sizeof(registry_cfg.fac_cal_corr_mat),
                &state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat));

    if (BMA2X2_ACCEL == sensor_type)
    {
      registry_cfg.acc_res_idx = state->resolution_idx;
      inst_state->accel_info.acc_res_idx  = state->resolution_idx;
      inst_state->accel_info.hw_ver       = state->common.hw_ver;
      registry_cfg.version = state->fac_cal_version;
    }
    else
    { /**/ }
  }

  INST_DEBUG(MED, instance, "fifo ef: 0x%x, fifo pf:0x%x",
                inst_state->fifo_info.fifo_enabled, inst_state->fifo_info.publish_sensors);
  bma2x2_set_inst_config(this,
                          instance,
                          chosen_report_rate,
                          chosen_sample_rate,
                          chosen_flush_period_ticks,
                          registry_cfg,
                          SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
}




/** See sns_bma2x2_sensor.h */
sns_sensor_instance* bma2x2_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  bma2x2_state *sstate = (bma2x2_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  SENSOR_DEBUG(LOW, this, "<bma2x2_sif_> set_client_req @sensor:%d", sstate->sensor);
  SENSOR_DEBUG(LOW, this, "this:%p, ereq:%p nreq:%p rm:%d inst:%p",
              this, exist_request, new_request, remove, instance);
  if (NULL != new_request)
  {
    SENSOR_DEBUG(MED, this, "req msg_id: %d", new_request->message_id);
  }

  if(remove)
  {
    if(NULL != instance)
    {
      instance->cb->remove_client_request(instance, exist_request);
      bma2x2_reval_instance_config(this, instance, sstate->sensor);
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
#if BMA2X2_CONFIG_ENABLE_POWER_RAIL
       if(sstate->sensor == BMA2X2_ACCEL ||
               sstate->sensor == BMA2X2_MOTION_DETECT ||
               sstate->sensor == BMA2X2_SENSOR_TEMP)
       {
         sstate->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
       }

       sstate->pwr_rail_service->api->sns_vote_power_rail_update(
                                            sstate->pwr_rail_service,
                                            this,
                                            &sstate->common.rail_config,
                                            &on_timestamp);

       delta = sns_get_system_time() - on_timestamp;

       // Use on_timestamp to determine correct Timer value.
       if(delta < sns_convert_ns_to_ticks(BMA2X2_OFF_TO_IDLE_MS*1000*1000))
       {
         bma2x2_start_power_rail_timer(this,
                                        sns_convert_ns_to_ticks(BMA2X2_OFF_TO_IDLE_MS*1000*1000) - delta,
                                        BMA2X2_POWER_RAIL_PENDING_SET_CLIENT_REQ);
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
       instance = this->cb->create_instance(this, sizeof(bma2x2_instance_state));

       bma2x2_instance_state *inst_state =
             (bma2x2_instance_state*)instance->state->state;

        /** save sensor specific info into instance state*/
        bma2x2_save_sensor_specific_registry_info_into_instance_state(this,inst_state);

       /* If rail is already ON then flag instance OK to configure */
       if(reval_config)
       {
         bma2x2_instance_state *inst_state =
          (bma2x2_instance_state*)instance->state->state;

         inst_state->instance_is_ready_to_configure = true;
       }
     }
     else
     {
       if(NULL != exist_request
          &&
          NULL != new_request
          &&
          new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
       {
         bma2x2_instance_state *inst_state = (bma2x2_instance_state*)instance->state->state;

         if(inst_state->fifo_info.fifo_enabled)
         {
           bma2x2_send_flush_config(this, instance);
           /** Do not update instance client request list at this point
           because FIFO flush is a transitory request for an on-going
           stream request. */
           return instance;
         }
         else
         {
           /** There aren't any FIFO sensors enabled to support flush.
            *  Send flush complete event anyway. */
           bma2x2_hal_send_fifo_flush_done(instance, &sstate->my_suid);
           return instance;
         }
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
        bma2x2_instance_state *inst_state =
           (bma2x2_instance_state*)instance->state->state;
        if(NULL != new_request)
        {
          instance->cb->add_client_request(instance, new_request);

          if(new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
             &&
             sstate->sensor == BMA2X2_MOTION_DETECT)
          {
            if (inst_state->fifo_info.publish_sensors & BMA2X2_ACCEL)
            {
              //send event as MD disabled since non-gated client is active
              //no need of this as we alreay set md_info state
              sns_motion_detect_event md_state;
              md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
              pb_send_event(instance,
                sns_motion_detect_event_fields,
                &md_state,
                sns_get_system_time(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &inst_state->md_info.suid);
              reval_config = false;
            }
            else if (inst_state->md_info.enable_md_int)
            {
              reval_config = false;
              //there is exsisting md client already present, just send event
              pb_send_event(instance,
                  sns_motion_detect_event_fields,
                  &inst_state->md_info.md_state,
                  sns_get_system_time(),
                  SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &inst_state->md_info.suid);
              INST_DEBUG(LOW, instance,"there is exsisting md client already present");
            }
            else
            {
              inst_state->md_info.md_new_req = true;
            }
          }
          if(new_request->message_id ==
             SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
          {
            if(bma2x2_extract_self_test_info(this, instance, new_request))
            {
              inst_state->new_self_test_request = true;
            }
          }
          // XXX
          if ((new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) &&
              (exist_request == NULL))
          {
            inst_state->accel_info.hw_acc_need_recfg |= 0x08;
          }
        }
        if(reval_config && inst_state->instance_is_ready_to_configure)
        {
          bma2x2_reval_instance_config(this, instance, sstate->sensor);

          if(inst_state->new_self_test_request)
          {
            bma2x2_set_self_test_inst_config(this, instance);
          }
        }
     }
  }

  if(NULL != instance &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid) BMA2X2_MOTION_DETECT_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)BMA2X2_ACCEL_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid) BMA2X2_SENSOR_TEMPERATURE_SUID, true))
  {
#if BMA2X2_CONFIG_ENABLE_REGISTRY
    bma2x2_instance_state *inst_state =
             (bma2x2_instance_state*)instance->state->state;
    if(inst_state->update_fac_cal_in_registry)
    {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
      /** Registry write can be handled in normal mode. */
      sstate->island_service->api->sensor_island_exit(sstate->island_service, this);
#endif
      /** Example code for registry write operation. */
      bma2x2_update_registry(this, instance, BMA2X2_ACCEL);
      bma2x2_update_sensor_state(this, instance);
    }
#endif
    this->cb->remove_instance(instance);
#if BMA2X2_CONFIG_ENABLE_POWER_RAIL
    bma2x2_turn_rails_off(this);
#endif
  }

  return instance;
}




// <API>
sns_sensor_api bma2x2_accel_sensor_api =
{
  .struct_len             = sizeof(sns_sensor_api),
  .init                   = &bma2x2_accel_init,
  .deinit                 = &bma2x2_accel_deinit,
  .get_sensor_uid         = &bma2x2_accel_get_sensor_uid,
  .set_client_request     = &bma2x2_set_client_request,
  .notify_event           = &bma2x2_sensor_notify_event,
};

sns_sensor_api bma2x2_motion_detect_sensor_api =
{
  .struct_len             = sizeof(sns_sensor_api),
  .init                   = &bma2x2_motion_detect_init,
  .deinit                 = &bma2x2_motion_detect_deinit,
  .get_sensor_uid         = &bma2x2_motion_detect_get_sensor_uid,
  .set_client_request     = &bma2x2_set_client_request,
  .notify_event           = &bma2x2_sensor_notify_event,
};

sns_sensor_api bma2x2_sensor_temp_sensor_api =
{
  .struct_len             = sizeof(sns_sensor_api),
  .init                   = &bma2x2_sensor_temp_init,
  .deinit                 = &bma2x2_sensor_temp_deinit,
  .get_sensor_uid         = &bma2x2_sensor_temp_get_sensor_uid,
  .set_client_request     = &bma2x2_set_client_request,
  .notify_event           = &bma2x2_sensor_notify_event,
};
// </API>

