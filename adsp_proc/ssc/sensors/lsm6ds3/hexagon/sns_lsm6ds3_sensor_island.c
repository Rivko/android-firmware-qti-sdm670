/**
 * @file sns_lsm6ds3_sensor_island.c
 *
 * Common implementation for LSM6DS3 Sensors for island mode.
 *
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

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

#include "sns_lsm6ds3_hal.h"
#include "sns_lsm6ds3_sensor.h"
#include "sns_lsm6ds3_sensor_instance.h"

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
#include "sns_cal.pb.h"


static sns_sensor_uid const* lsm6ds3_accel_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = ACCEL_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* lsm6ds3_gyro_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = GYRO_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* lsm6ds3_motion_detect_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = MOTION_DETECT_SUID;

  return &sensor_uid;
}

static sns_sensor_uid const* lsm6ds3_sensor_temp_get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid = SENSOR_TEMPERATURE_SUID;

  return &sensor_uid;
}

void lsm6ds3_start_power_rail_timer(sns_sensor *const this,
                                           sns_time timeout_ticks,
                                           lsm6ds3_power_rail_pending_state pwr_rail_pend_state)
{
  lsm6ds3_state *state = (lsm6ds3_state*)this->state->state;

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

/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void lsm6ds3_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance)
{

  sns_request config;

  config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
  config.request_len = 0;
  config.request = NULL;

  this->instance_api->set_client_config(instance, &config);
}

/** See sns_lsm6ds3_sensor.h*/
sns_rc lsm6ds3_sensor_notify_event(sns_sensor *const this)
{
  lsm6ds3_state *state = (lsm6ds3_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_sensor_event *event;

  if(!sns_suid_lookup_complete(&state->common.suid_lookup_data))
  {
    state->island_service->api->sensor_island_exit(state->island_service, this);
    sns_suid_lookup_handle(this, &state->common.suid_lookup_data);

    if(sns_suid_lookup_get(&state->common.suid_lookup_data, "registry", NULL))
    {
      lsm6ds3_request_registry(this);
    }

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

      if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT == event->message_id)
      {
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(state->power_rail_pend_state == LSM6DS3_POWER_RAIL_PENDING_INIT)
          {
            /** Initial HW discovery is OK to run in normal mode. */
            state->island_service->api->sensor_island_exit(state->island_service, this);

            state->common.hw_is_present = lsm6ds3_discover_hw(this);

            if(state->common.hw_is_present)
            {
              lsm6ds3_publish_available(this);
              lsm6ds3_update_sibling_sensors(this);
            }
            else
            {
              rv = SNS_RC_INVALID_LIBRARY_STATE;
              LSM6DS3_PRINTF(LOW, this, "LSM6DS3 HW absent");
            }
            state->power_rail_pend_state = LSM6DS3_POWER_RAIL_PENDING_NONE;
          }
          else if(state->power_rail_pend_state == LSM6DS3_POWER_RAIL_PENDING_SET_CLIENT_REQ)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            if(NULL != instance)
            {
              lsm6ds3_instance_state *inst_state =
                 (lsm6ds3_instance_state*) instance->state->state;
              inst_state->instance_is_ready_to_configure = true;
              lsm6ds3_reval_instance_config(this, instance, state->sensor);
              if(inst_state->new_self_test_request)
              {
                lsm6ds3_set_self_test_inst_config(this, instance);
              }
            }
            state->power_rail_pend_state = LSM6DS3_POWER_RAIL_PENDING_NONE;
          }
          else if(LSM6DS3_POWER_RAIL_PENDING_OFF == state->power_rail_pend_state)
          {
            sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
            /*Not required to turn OFF power rail, If instance is present*/
            if(NULL == instance)
            {
              if(SNS_RAIL_OFF != state->common.rail_config.rail_vote)
              {
                state->common.rail_config.rail_vote = SNS_RAIL_OFF;
                state->pwr_rail_service->api->
                sns_vote_power_rail_update(state->pwr_rail_service, this,
                                           &state->common.rail_config, NULL);
              }
            }
            state->power_rail_pend_state = LSM6DS3_POWER_RAIL_PENDING_NONE;
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
    if(state->power_rail_pend_state == LSM6DS3_POWER_RAIL_PENDING_NONE)
    {
      sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
    }
  }

  if(NULL != state->reg_data_stream)
  {
    event = state->reg_data_stream->api->peek_input(state->reg_data_stream);
    while(NULL != event)
    {
      /** All registry events can be handled in normal mode. */
      state->island_service->api->sensor_island_exit(state->island_service, this);
      lsm6ds3_sensor_process_registry_event(this, event);

      event = state->reg_data_stream->api->get_next_input(state->reg_data_stream);
    }
  }

  if(state->common.registry_pf_cfg_received && state->common.registry_cfg_received &&
     state->common.registry_orient_received && state->common.registry_placement_received)
  {
    /** Initial HW detection sequence is OK to run in normal mode. */
    state->island_service->api->sensor_island_exit(state->island_service, this);
    if(sns_suid_lookup_get(&state->common.suid_lookup_data, "timer", NULL))
    {
      lsm6ds3_start_hw_detect_sequence(this);
    }
  }
  return rv;
}

static void lsm6ds3_send_flush_config(sns_sensor *const this,
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
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool lsm6ds3_get_decoded_imu_request(sns_sensor const *this, sns_request const *in_request,
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
 * @return bool True if decoding is successfull else false.
 */
static bool lsm6ds3_get_decoded_self_test_request(sns_sensor const *this, sns_request const *request,
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
 * Parses through all starndard streaming requests and deduces
 * best HW config for the inertial sensor type.
 *
 * @param[i] this             Sensor reference
 * @param[i] instance         Instance reference
 * @param[i] sensor_type      sensor type
 * @param[o] chosen_sample_rate   chosen sample rate in Hz
 * @param[o] chosen_report_rate   chosen report rate in Hz 
 * @param[o] chosen_flush_period_ticks   chosen flush period in ticks 
 * @param[o] non_gated_sensor_client_present  True if non-gated
 *       client is present.
 * @param[o] gated_sensor_client_present  Tur if gated client is
 *       present.
 */
static void lsm6ds3_get_imu_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6ds3_sensor_type sensor_type,
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

  if(sensor_type == LSM6DS3_ACCEL)
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)ACCEL_SUID), sizeof(sns_sensor_uid));
  }
  else
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)GYRO_SUID), sizeof(sns_sensor_uid));
  }

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
      if(lsm6ds3_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
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

        *chosen_report_rate = SNS_MAX(*chosen_report_rate,
                                      report_rate);

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

  LSM6DS3_PRINTF(LOW, this, "is_flush_only %u is_max_batch %d",
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
static void lsm6ds3_get_motion_detect_config(sns_sensor *this,
                                             sns_sensor_instance *instance,
                                             bool *chosen_md_enable,
                                             bool *md_client_present)
{
  UNUSED_VAR(this);
  sns_sensor_uid suid = MOTION_DETECT_SUID;
  lsm6ds3_instance_state *inst_state =
     (lsm6ds3_instance_state*)instance->state->state;
  sns_request const *request;

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
static void lsm6ds3_get_sensor_temp_config(sns_sensor *this,
                                           sns_sensor_instance *instance,
                                           float *chosen_sample_rate,
                                           float *chosen_report_rate,
                                           sns_time *chosen_flush_period_ticks,
                                           bool *sensor_temp_client_present)
{
  UNUSED_VAR(this);
  lsm6ds3_instance_state *inst_state =
     (lsm6ds3_instance_state*)instance->state->state;
  sns_sensor_uid suid = SENSOR_TEMPERATURE_SUID;
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
      if(lsm6ds3_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
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
static void lsm6ds3_set_inst_config(sns_sensor *this,
                                    sns_sensor_instance *instance,
                                    float chosen_report_rate,
                                    float chosen_sample_rate,
                                    sns_time chosen_flush_period_ticks,
                                    sns_lsm6ds3_registry_cfg registry_cfg,
                                    uint32_t message_id)
{
  sns_lsm6ds3_req new_client_config;
  sns_request config;

  new_client_config.desired_report_rate = chosen_report_rate;
  new_client_config.desired_sample_rate = chosen_sample_rate;
  new_client_config.desired_flush_ticks = chosen_flush_period_ticks;
  new_client_config.registry_cfg = registry_cfg;

  config.message_id = message_id;
  config.request_len = sizeof(sns_lsm6ds3_req);
  config.request = &new_client_config;

  this->instance_api->set_client_config(instance, &config);
}

/**
 * Turns power rails off
 *
 * @paramp[i] this   Sensor reference
 *
 * @return none
 */
static void lsm6ds3_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor = NULL;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    lsm6ds3_state *sensor_state = (lsm6ds3_state*)sensor->state->state;
    if(SNS_RAIL_OFF != sensor_state->common.rail_config.rail_vote)
    {
      sns_time timeout = sns_convert_ns_to_ticks(LSM6DS3_POWER_RAIL_OFF_TIMEOUT_NS);
      lsm6ds3_start_power_rail_timer( sensor, timeout, LSM6DS3_POWER_RAIL_PENDING_OFF );
    }
  }
}
/**
 * Copy sensor specfic registry info into instance state (Ex: md_config)
 *
 * @param[i] this        Sensor reference
 * @param[i] inst_state  Sensor Instance state reference
 *
 * @return None
 */
static void lsm6ds3_save_sensor_specific_registry_info_into_instance_state(
	sns_sensor *const this,lsm6ds3_instance_state *inst_state)
{
	   
  sns_sensor *sensor = NULL;
  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    lsm6ds3_state *sensor_state = (lsm6ds3_state*)sensor->state->state;
    /*Save md sensor registry config into instance state */
    if(LSM6DS3_MOTION_DETECT == sensor_state->sensor)
    {             
	  sns_memscpy(&inst_state->md_info.md_config, sizeof(inst_state->md_info.md_config),
	    &sensor_state->md_config, sizeof(sensor_state->md_config));
	  break;
    }
  }	
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
static bool lsm6ds3_extract_self_test_info(sns_sensor *this,
                              sns_sensor_instance *instance,
                              struct sns_request const *new_request)
{
  sns_std_request decoded_request;
  sns_physical_sensor_test_config test_config = sns_physical_sensor_test_config_init_default;
  lsm6ds3_state *state = (lsm6ds3_state*)this->state->state;
  lsm6ds3_instance_state *inst_state = (lsm6ds3_instance_state*)instance->state->state;
  lsm6ds3_self_test_info *self_test_info;

  if(state->sensor == LSM6DS3_ACCEL)
  {
    self_test_info = &inst_state->accel_info.test_info;
  }
  else if(state->sensor == LSM6DS3_GYRO)
  {
    self_test_info = &inst_state->gyro_info.test_info;
  }
  else if(state->sensor == LSM6DS3_MOTION_DETECT)
  {
    self_test_info = &inst_state->md_info.test_info;
  }
  else if(state->sensor == LSM6DS3_SENSOR_TEMP)
  {
    self_test_info = &inst_state->sensor_temp_info.test_info;
  }
  else
  {
    return false;
  }

  if(lsm6ds3_get_decoded_self_test_request(this, new_request, &decoded_request, &test_config))
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

/** See sns_lsm6ds3_sensor.h */
void lsm6ds3_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6ds3_sensor_type sensor_type)
{
  /**
   * 1. Get best non-gated and gated Accel Config.
   * 2. Get best Gyro Config.
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
  bool g_sensor_client_present;
  bool md_sensor_client_present;
  bool sensor_temp_client_present;
  lsm6ds3_instance_state *inst_state =
     (lsm6ds3_instance_state*)instance->state->state;
  lsm6ds3_state *state = (lsm6ds3_state*)this->state->state;
  sns_lsm6ds3_registry_cfg registry_cfg = {.sensor_type = sensor_type};

  lsm6ds3_get_imu_config(this, instance, LSM6DS3_ACCEL, &chosen_sample_rate,
            &chosen_report_rate, &chosen_flush_period_ticks,
            &a_sensor_client_present_non_gated, &a_sensor_client_present_gated);

  inst_state->accel_info.gated_client_present = a_sensor_client_present_gated;

  LSM6DS3_PRINTF(LOW, this, "Accel: rr %u sr %u f_p %u gated_present %u",
             (uint32_t)chosen_report_rate, (uint32_t)chosen_sample_rate,
             (uint32_t)chosen_flush_period_ticks, a_sensor_client_present_gated);

  lsm6ds3_get_imu_config(this, instance, LSM6DS3_GYRO, &sample_rate,
            &report_rate, &flush_period_ticks, &g_sensor_client_present, NULL);

  LSM6DS3_PRINTF(LOW, this, "Gyro: rr %u sr %u f_p %u",
             (uint32_t)report_rate, (uint32_t)sample_rate,
             (uint32_t)flush_period_ticks);

  chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
  chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);
  chosen_flush_period_ticks = SNS_MAX(chosen_flush_period_ticks, flush_period_ticks);

  lsm6ds3_get_motion_detect_config(this, instance,
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

  lsm6ds3_get_sensor_temp_config(this, instance, &sample_rate, &report_rate,
             &flush_period_ticks, &sensor_temp_client_present);
  inst_state->sensor_temp_info.max_requested_flush_ticks = flush_period_ticks;

  LSM6DS3_PRINTF(LOW, this, "Sensor temp: rr %u sr %u f_p %u",
             (uint32_t)report_rate, (uint32_t)sample_rate,
             (uint32_t)flush_period_ticks);

  chosen_sample_rate = SNS_MAX(chosen_sample_rate, sample_rate);
  chosen_report_rate = SNS_MAX(chosen_report_rate, report_rate);

  /** Start with a clean slate */
  inst_state->fifo_info.fifo_enabled = 0;
  inst_state->fifo_info.publish_sensors = 0;

  if(a_sensor_client_present_non_gated)
  {
    inst_state->fifo_info.fifo_enabled |= LSM6DS3_ACCEL;
    inst_state->fifo_info.publish_sensors |= LSM6DS3_ACCEL;
  }

  if(a_sensor_client_present_gated)
  {
    inst_state->fifo_info.fifo_enabled |= LSM6DS3_ACCEL;
    if(!inst_state->md_info.enable_md_int)
    {
      inst_state->fifo_info.publish_sensors |= LSM6DS3_ACCEL;
    }
  }

  if(g_sensor_client_present)
  {
    inst_state->fifo_info.fifo_enabled |= (LSM6DS3_ACCEL | LSM6DS3_GYRO);
    inst_state->fifo_info.publish_sensors |= LSM6DS3_GYRO;
  }

  if(md_sensor_client_present)
  {
    inst_state->fifo_info.fifo_enabled |= LSM6DS3_ACCEL;
  }

  if(sensor_temp_client_present)
  {
    inst_state->fifo_info.fifo_enabled |= LSM6DS3_ACCEL;
    inst_state->fifo_info.publish_sensors |= LSM6DS3_SENSOR_TEMP;
  }

  if(LSM6DS3_ACCEL == sensor_type || LSM6DS3_GYRO == sensor_type ||
     LSM6DS3_SENSOR_TEMP == sensor_type)
  {
    sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
                state->fac_cal_bias, sizeof(state->fac_cal_bias));

    sns_memscpy(&registry_cfg.fac_cal_corr_mat, sizeof(registry_cfg.fac_cal_corr_mat),
                &state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat));
    registry_cfg.version = state->fac_cal_version;
  }

  lsm6ds3_set_inst_config(this,
                          instance,
                          chosen_report_rate,
                          chosen_sample_rate,
                          chosen_flush_period_ticks,
                          registry_cfg,
                          SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
}

/** See sns_lsm6ds3_sensor.h */
sns_sensor_instance* lsm6ds3_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  lsm6ds3_state *state = (lsm6ds3_state*)this->state->state;
  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  LSM6DS3_PRINTF(LOW, this, "set_client_req 0x%X  0x%X  %d", exist_request, new_request, remove);

  if(remove)
  {
    if(NULL != instance)
    {
      instance->cb->remove_client_request(instance, exist_request);
      lsm6ds3_reval_instance_config(this, instance, state->sensor);
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
       if(state->sensor == LSM6DS3_GYRO)
       {
         state->common.rail_config.rail_vote = SNS_RAIL_ON_NPM;
       }
       else
       {
         state->common.rail_config.rail_vote = state->common.registry_rail_on_state;
       }

       state->pwr_rail_service->api->sns_vote_power_rail_update(
                                            state->pwr_rail_service,
                                            this,
                                            &state->common.rail_config,
                                            &on_timestamp);

       delta = sns_get_system_time() - on_timestamp;

       // Use on_timestamp to determine correct Timer value.
       if(delta < sns_convert_ns_to_ticks(LSM6DS3_OFF_TO_IDLE_MS*1000*1000))
       {
         lsm6ds3_start_power_rail_timer(this,
                                        sns_convert_ns_to_ticks(LSM6DS3_OFF_TO_IDLE_MS*1000*1000) - delta,
                                        LSM6DS3_POWER_RAIL_PENDING_SET_CLIENT_REQ);
       }
       else
       {
         // rail is already ON
         reval_config = true;
       }

       /** create_instance() calls init() for the Sensor Instance */
       instance = this->cb->create_instance(this,
                                            sizeof(lsm6ds3_instance_state));

	   lsm6ds3_instance_state *inst_state =
          (lsm6ds3_instance_state*)instance->state->state;

	   /** save sensor specific info into instance state*/
	   lsm6ds3_save_sensor_specific_registry_info_into_instance_state(this,inst_state);	   	   
       /* If rail is already ON then flag instance OK to configure */
       if(reval_config)
       {

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
         lsm6ds3_instance_state *inst_state =
          (lsm6ds3_instance_state*)instance->state->state;

         if(inst_state->fifo_info.fifo_enabled)
         {
           lsm6ds3_send_flush_config(this, instance);
           /** Do not update instance client request list at this point
           because FIFO flush is a transitory request for an on-going
           stream request. */
           return instance;
         }
         else
         {
           /** There aren't any FIFO sensors enabled to support flush.
            *  Send flush complete event anyway. */
           lsm6ds3_send_fifo_flush_done(instance, &state->my_suid);
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
        lsm6ds3_instance_state *inst_state =
           (lsm6ds3_instance_state*)instance->state->state;
        if(NULL != new_request)
        {
          instance->cb->add_client_request(instance, new_request);

          if(new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
             &&
             state->sensor == LSM6DS3_MOTION_DETECT)
          {                        
            if (inst_state->md_info.enable_md_int)
            {
              reval_config = false;
			  //there is exsisting md client already present, just send event
              pb_send_event(instance,
                sns_motion_detect_event_fields,
                &inst_state->md_info.md_state,
                sns_get_system_time(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &inst_state->md_info.suid);                
	          LSM6DS3_PRINTF(LOW,this,"there is exsisting md client already present");
            }else
            {
              inst_state->md_info.md_new_req = true;          
            }
          }
          if(new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET) {
            LSM6DS3_PRINTF(LOW,this,"Request for resetting cal data");
            lsm6ds3_reset_cal_data(instance,state->sensor);
            lsm6ds3_update_sensor_state(this, instance);
            lsm6ds3_update_registry(this, instance, LSM6DS3_ACCEL);
            lsm6ds3_update_registry(this, instance, LSM6DS3_GYRO);
            lsm6ds3_send_cal_event(instance, state->sensor);
          }
          if(new_request->message_id ==
             SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
          {
            if(lsm6ds3_extract_self_test_info(this, instance, new_request))
            {
              inst_state->new_self_test_request = true;
            }
          }
        }
        if(reval_config && inst_state->instance_is_ready_to_configure)
        {
          lsm6ds3_reval_instance_config(this, instance, state->sensor);

          if(inst_state->new_self_test_request)
          {
            lsm6ds3_set_self_test_inst_config(this, instance);
          }
        }
     }
  }

  if(NULL != instance &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)MOTION_DETECT_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)ACCEL_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)GYRO_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)SENSOR_TEMPERATURE_SUID, true))
  {
    lsm6ds3_instance_state *inst_state =
           (lsm6ds3_instance_state*)instance->state->state;
    if(inst_state->update_fac_cal_in_registry)
    {
      /** Registry write can be handled in normal mode. */
      state->island_service->api->sensor_island_exit(state->island_service, this);
      /** Update factory cal data in the registry. */
      lsm6ds3_update_registry(this, instance, LSM6DS3_ACCEL);
      lsm6ds3_update_registry(this, instance, LSM6DS3_GYRO);
      lsm6ds3_update_sensor_state(this, instance);
    }
    LSM6DS3_PRINTF(LOW,this,"Instance is in removal state");
    this->cb->remove_instance(instance);
    lsm6ds3_turn_rails_off(this);
  }

  return instance;
}

sns_sensor_api lsm6ds3_accel_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6ds3_accel_init,
  .deinit             = &lsm6ds3_accel_deinit,
  .get_sensor_uid     = &lsm6ds3_accel_get_sensor_uid,
  .set_client_request = &lsm6ds3_set_client_request,
  .notify_event       = &lsm6ds3_sensor_notify_event,
};

sns_sensor_api lsm6ds3_gyro_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6ds3_gyro_init,
  .deinit             = &lsm6ds3_gyro_deinit,
  .get_sensor_uid     = &lsm6ds3_gyro_get_sensor_uid,
  .set_client_request = &lsm6ds3_set_client_request,
  .notify_event       = &lsm6ds3_sensor_notify_event,
};

sns_sensor_api lsm6ds3_motion_detect_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6ds3_motion_detect_init,
  .deinit             = &lsm6ds3_motion_detect_deinit,
  .get_sensor_uid     = &lsm6ds3_motion_detect_get_sensor_uid,
  .set_client_request = &lsm6ds3_set_client_request,
  .notify_event       = &lsm6ds3_sensor_notify_event,
};

sns_sensor_api lsm6ds3_sensor_temp_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6ds3_sensor_temp_init,
  .deinit             = &lsm6ds3_sensor_temp_deinit,
  .get_sensor_uid     = &lsm6ds3_sensor_temp_get_sensor_uid,
  .set_client_request = &lsm6ds3_set_client_request,
  .notify_event       = &lsm6ds3_sensor_notify_event,
};

