/**
 * @file sns_lsm6dso_sensor.c
 *
 * Common implementation for LSM6DSO Sensors.
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
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
 **/

#include <string.h>
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_diag_service.h"
#include "sns_island_service.h"
#include "sns_lsm6dso_sensor.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_motion_detect.pb.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_registry.pb.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_timer.pb.h"
#include "sns_cal.pb.h"

/** Forward Declaration of Accel Sensor API */
sns_sensor_api lsm6dso_accel_sensor_api;

/** Forward Declaration of Gyro Sensor API */
sns_sensor_api lsm6dso_gyro_sensor_api;

/** Forward Declaration of motion detect Sensor API */
sns_sensor_api lsm6dso_motion_detect_sensor_api;

/** Forward Declaration of Sensor temp Sensor API */
sns_sensor_api lsm6dso_sensor_temp_sensor_api;

/** LSM6DSO Accel Resolution */
const float lsm6dso_accel_resolutions[] =
{
  LSM6DSO_ACCEL_RESOLUTION_2G,
  LSM6DSO_ACCEL_RESOLUTION_4G,
  LSM6DSO_ACCEL_RESOLUTION_8G,
  LSM6DSO_ACCEL_RESOLUTION_16G
}; //mg/LSB

/** LSM6DSO Gyro Resolution */
const float lsm6dso_gyro_resolutions[] =
{
  LSM6DSO_GYRO_SSTVT_125DPS,
  LSM6DSO_GYRO_SSTVT_245DPS,
  LSM6DSO_GYRO_SSTVT_500DPS,
  LSM6DSO_GYRO_SSTVT_1000DPS,
  LSM6DSO_GYRO_SSTVT_2000DPS
};  //mdps/LSB

lsm6dso_gyro_range lsm6dso_gyro_ranges[] =
{
  STM_LSM6DSO_GYRO_RANGE_125DPS,
  STM_LSM6DSO_GYRO_RANGE_245DPS,  /*corresponding value in register setting*/
  STM_LSM6DSO_GYRO_RANGE_500DPS,
  STM_LSM6DSO_GYRO_RANGE_1000DPS,
  STM_LSM6DSO_GYRO_RANGE_2000DPS
};

lsm6dso_accel_range lsm6dso_accel_ranges[] =
{
  LSM6DSO_ACCEL_RANGE_2G,
  LSM6DSO_ACCEL_RANGE_4G,
  LSM6DSO_ACCEL_RANGE_8G,
  LSM6DSO_ACCEL_RANGE_16G
};

//define non-dependent sensors at the begining
//then dependent sensors ex: MA depends on accel
//so MA shoule be defined below the accel
//Not following the order fails sensor configuration
const lsm6dso_sensors lsm6dso_supported_sensors[ MAX_SUPPORTED_SENSORS ] = {
  {LSM6DSO_ACCEL, &lsm6dso_accel_sensor_api, &lsm6dso_sensor_instance_api},
  {LSM6DSO_GYRO, &lsm6dso_gyro_sensor_api, &lsm6dso_sensor_instance_api},
  {LSM6DSO_MOTION_DETECT , &lsm6dso_motion_detect_sensor_api, &lsm6dso_sensor_instance_api},
  {LSM6DSO_SENSOR_TEMP, &lsm6dso_sensor_temp_sensor_api, &lsm6dso_sensor_instance_api}
};
//static char reg_dependency[][MAX_DEP_LENGTH] =  {"registry" };
//static char add_dependency[][MAX_DEP_LENGTH]= {""};

static void lsm6dso_exit_island(sns_sensor *const this)
{
  sns_service_manager *smgr = this->cb->get_service_manager(this);
  sns_island_service  *island_svc  =
    (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);
  island_svc->api->sensor_island_exit(island_svc, this);
}

static sns_rc lsm6dso_process_timer_events(sns_sensor *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_data_stream *stream = state->timer_stream;

  if(NULL == stream || 0 == stream->api->get_input_cnt(stream))
  {
    return rv;
  }

  for(sns_sensor_event *event = stream->api->peek_input(stream);
      NULL != event;
      event = stream->api->get_next_input(stream))
  {
    sns_diag_service *diag = state->diag_service;
    sns_timer_sensor_event timer_event;
    pb_istream_t pbstream;

    if(SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT != event->message_id)
    {
      continue; /* not interested in other events */
    }

    pbstream = pb_istream_from_buffer((pb_byte_t*)event->event, event->event_len);
    if(!pb_decode(&pbstream, sns_timer_sensor_event_fields, &timer_event))
    {
      DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__, "pb_decode error");
      continue;
    }

    DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
              "Timer fired:: sensor %d req_timeout %u timeout %u ",
              state->sensor, timer_event.requested_timeout_time, timer_event.timeout_time);
    DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
              "Timer fired:: sensor %d power_rail %d ",
              state->sensor, state->power_rail_pend_state);

    if(state->power_rail_pend_state == LSM6DSO_POWER_RAIL_PENDING_INIT)
    {
      /** Initial HW discovery is OK to run in normal mode. */
      lsm6dso_exit_island(this);
      lsm6dso_discover_hw(this);
      if(!state->hw_is_present)
      {
        rv = SNS_RC_INVALID_LIBRARY_STATE;
        DBG_PRINT(diag, this, LOW, __FILENAME__, __LINE__, "LSM6DSO HW absent");
      }
    }
    else if(state->power_rail_pend_state == LSM6DSO_POWER_RAIL_PENDING_SET_CLIENT_REQ)
    {
      sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
      if(NULL != instance)
      {
        lsm6dso_instance_state *inst_state =  (lsm6dso_instance_state*) instance->state->state;
        DBG_PRINT(diag, this, LOW, __FILENAME__, __LINE__,
            "instance available :: reval configuration");
        inst_state->instance_is_ready_to_configure = true;
        lsm6dso_reval_instance_config(this, instance, state->sensor);
      } else {
        DBG_PRINT(diag, this, LOW, __FILENAME__, __LINE__,
            "instance is not available");
      }
    }
    else if(state->power_rail_pend_state == LSM6DSO_POWER_RAIL_PENDING_OFF)
    {
      sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);

      state->rail_config.rail_vote = SNS_RAIL_OFF;
      state->pwr_rail_service->api->
        sns_vote_power_rail_update(state->pwr_rail_service, this,
                                   &state->rail_config, NULL);
      if(NULL != instance)
      {
        lsm6dso_instance_state *inst_state =  (lsm6dso_instance_state*) instance->state->state;
        inst_state->instance_is_ready_to_configure = false;
      }
    }

    //Remove registry data stream
    sns_sensor_util_remove_sensor_stream(this, &state->reg_data_stream);

    state->power_rail_pend_state = LSM6DSO_POWER_RAIL_PENDING_NONE;
  }
  if(state->power_rail_pend_state == LSM6DSO_POWER_RAIL_PENDING_NONE)
  {
    sns_sensor_util_remove_sensor_stream(this, &state->timer_stream);
  }
  return rv;
}

/* See sns_sensor::get_sensor_uid */
sns_sensor_uid const* lsm6dso_get_sensor_uid(sns_sensor const *const this)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  return &state->my_suid;
}

static void lsm6dso_start_power_rail_timer(sns_sensor *const this,
                                           sns_time timeout_ticks,
                                           lsm6dso_power_rail_pending_state pwr_rail_pend_state)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_diag_service* diag = state->diag_service;

  if(NULL == state->timer_stream)
  {
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_svc = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    stream_svc->api->create_sensor_stream(stream_svc, this, state->timer_suid,
                                          &state->timer_stream);
  }

  if(NULL != state->timer_stream)
  {
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
           .request = buffer, .request_len = req_len};
      state->timer_stream->api->send_request(state->timer_stream, &timer_req);
      state->power_rail_pend_state = pwr_rail_pend_state;
    }
    else
    {
      DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                               "LSM timer req encode error");
    }
  }
  else
  {
    DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
              "Failed to create timer stream");
  }
}

/** See sns_lsm6dso_sensor.h*/
static sns_rc lsm6dso_sensor_notify_event(sns_sensor *const this)
{
  sns_rc rv = SNS_RC_SUCCESS;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;

  if((NULL != state->fw_stream &&
      0 != state->fw_stream->api->get_input_cnt(state->fw_stream)) ||
     (NULL != state->reg_data_stream &&
      0 != state->reg_data_stream->api->get_input_cnt(state->reg_data_stream)))
  {
    lsm6dso_exit_island(this);
    lsm6dso_process_suid_events(this);
    rv = lsm6dso_process_registry_events(this);
  }

  if(rv == SNS_RC_SUCCESS)
  {
    rv = lsm6dso_process_timer_events(this);
  }

  if(rv == SNS_RC_SUCCESS && LSM6DSO_ACCEL == state->sensor)
  {
    if(!state->hw_is_present &&
       NULL != state->pwr_rail_service &&
       NULL != state->timer_stream &&
       state->power_rail_pend_state == LSM6DSO_POWER_RAIL_PENDING_NONE)
    {
      sns_time timeticks;

      state->rail_config.rail_vote = SNS_RAIL_ON_LPM;
      state->pwr_rail_service->api->sns_vote_power_rail_update(state->pwr_rail_service,
                                                               this,
                                                               &state->rail_config,
                                                               &timeticks); /* ignored */
      timeticks = sns_convert_ns_to_ticks(LSM6DSO_OFF_TO_IDLE_MS * 1000 * 1000);
      lsm6dso_start_power_rail_timer(this, timeticks, LSM6DSO_POWER_RAIL_PENDING_INIT);
    }

    if(!state->available && state->hw_is_present && state->outstanding_reg_requests == 0)
    {
      lsm6dso_exit_island(this);
      lsm6dso_update_siblings(this);
    }
  }

  return rv;
}

static void  lsm6dso_send_flush_config(sns_sensor *const this,
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
static bool lsm6dso_get_decoded_imu_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_std_sensor_config *decoded_payload)
{

  lsm6dso_state *state = (lsm6dso_state *) this->state->state;
  sns_diag_service* diag = state->diag_service;
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
    DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                             "LSM decode error");
    return false;
  }
  return true;
}

/**
 * Returns decoded request message for type
 * sns_physical_sensor_test_config_fields.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
static bool lsm6dso_get_decoded_self_test_request(sns_sensor const *this, sns_request const *in_request,
                                            sns_std_request *decoded_request,
                                            sns_physical_sensor_test_config *decoded_payload)
{

  lsm6dso_state *state = (lsm6dso_state *) this->state->state;
  sns_diag_service* diag = state->diag_service;
  pb_istream_t stream;

  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_physical_sensor_test_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
                             "LSM decode error - self test");
    return false;
  }
  return true;
}

static void lsm6dso_get_imu_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6dso_sensor_type sensor_type,
                                   float *chosen_sample_rate,
                                   float *chosen_report_rate,
                                   uint64_t *chosen_flush_period_ticks,
                                   bool *non_gated_sensor_client_present,
                                   bool *gated_sensor_client_present)
{
  lsm6dso_instance_state *inst_state =
    (lsm6dso_instance_state*)instance->state->state;
  sns_sensor_uid suid;
  sns_request const *request;
  lsm6dso_state *state = (lsm6dso_state *) this->state->state;
  sns_diag_service* diag = state->diag_service;
  bool is_flush_only = true;

  if(sensor_type == LSM6DSO_ACCEL)
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)ACCEL_SUID), sizeof(sns_sensor_uid));
  }
  else if(sensor_type == LSM6DSO_GYRO)
  {
    sns_memscpy(&suid, sizeof(suid), &((sns_sensor_uid)GYRO_SUID), sizeof(sns_sensor_uid));
  }

  *chosen_report_rate = 0;
  *chosen_sample_rate = 0;
  *chosen_flush_period_ticks = 0;
  *non_gated_sensor_client_present = false;
  *gated_sensor_client_present = false;

  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};
    decoded_payload.sample_rate = 0;
    sns_physical_sensor_test_config decoded_payload_phy = sns_physical_sensor_test_config_init_default;

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
        ||
        request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(lsm6dso_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate = 0;
        bool is_max_batch = true;

        *chosen_sample_rate = SNS_MAX(*chosen_sample_rate,
            decoded_payload.sample_rate);

        DBG_PRINT(state->diag_service, this, MED, __FILENAME__, __LINE__,
                  "decoded req sp(*1000) %d rp(*1000) %d",
                 (int)(decoded_payload.sample_rate * 1000), (int)(decoded_request.batching.batch_period*1000));
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
        else if(!is_max_batch)
        {
          report_rate = UINT32_MAX;
        }

        if(is_max_batch) {
          /** if max batch then always use max FIFO */
          report_rate = (1.0f / UINT32_MAX);
        }

        *chosen_report_rate = SNS_MAX(*chosen_report_rate,
            report_rate);

        if(*chosen_sample_rate) {
          if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
          {
            *non_gated_sensor_client_present = true;
          }
          else
          {
            *gated_sensor_client_present = true;
          }
        }

        DBG_PRINT(state->diag_service, this, MED, __FILENAME__, __LINE__,
                  "is_flush_only %u is_max_batch %d",
                  is_flush_only, is_max_batch);
      }
    }
    else if(lsm6dso_get_decoded_self_test_request(this, request, &decoded_request, &decoded_payload_phy)) //check for self test
    {
      if(request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
          "lsm6dso_get_imu_config: Physical sensor test: %d", decoded_payload_phy.test_type);

        // Only SW test is not supported.
        if(decoded_payload_phy.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_SW)
          return;
        inst_state->common_info.mode |= LSM6DSO_MODE_SELF_TEST;
        inst_state->self_test_info.test_type = decoded_payload_phy.test_type;
        if(sensor_type == LSM6DSO_ACCEL)
        {
          inst_state->accel_info.self_test_is_successful = true;
        }
        else
        {
          inst_state->gyro_info.self_test_is_successful = true;
        }
      }
    }
  }
}

static void lsm6dso_get_motion_detect_config(sns_sensor *this,
                                             sns_sensor_instance *instance,
                                             bool *md_client_present)
{
  sns_request const *request;
  sns_sensor_uid suid = MOTION_DETECT_SUID;
  lsm6dso_instance_state *inst_state =
     (lsm6dso_instance_state*)instance->state->state;
  sns_diag_service* diag = inst_state->diag_service;
  sns_std_request decoded_request;
  sns_physical_sensor_test_config decoded_payload_phy = sns_physical_sensor_test_config_init_default;
  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                             "get md config");

  inst_state->md_info.enable_md_int = false;

  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
    {
    // Enable MD interrupt when:
    // 1. There is a new request and MD is in MDF state OR
    // 2. There is an existing request and MD is in MDE/MDD state

      inst_state->md_info.enable_md_int = true;

      //client for md (=non_gated)
      *md_client_present = true;
      // Consumed new request
      inst_state->md_info.md_new_req = false;
      return;
    }
    else if(lsm6dso_get_decoded_self_test_request(this, request, &decoded_request, &decoded_payload_phy)) //check for self test
    {

      if(request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
          "lsm6dso_get_imu_config: Physical sensor test: %d", decoded_payload_phy.test_type);

        // Only COM test is supported.
        if(decoded_payload_phy.test_type != SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
          return;
        inst_state->common_info.mode |= LSM6DSO_MODE_SELF_TEST;
        inst_state->self_test_info.test_type = decoded_payload_phy.test_type;
        inst_state->md_info.self_test_is_successful = true;
        return; //Return without enabling the MD
      }
    }
  }

}


static void lsm6dso_get_sensor_temp_config(sns_sensor *this,
                                           sns_sensor_instance *instance,
                                           float *chosen_sample_rate,
                                           float *chosen_report_rate,
                                           uint64_t *chosen_flush_period_ticks,
                                           bool *sensor_temp_client_present)
{
  UNUSED_VAR(this);
  lsm6dso_instance_state *inst_state =
    (lsm6dso_instance_state*)instance->state->state;
  sns_sensor_uid suid = SENSOR_TEMPERATURE_SUID;
  sns_request const *request;
  sns_diag_service* diag = inst_state->diag_service;

  *chosen_report_rate = 0;
  *chosen_sample_rate = 0;
  *chosen_flush_period_ticks = 0;
  *sensor_temp_client_present = false;
  bool is_flush_only = true;

  /** Parse through existing requests and get fastest sample
   *  rate and report rate requests. */
  for(request = instance->cb->get_client_request(instance, &suid, true);
      NULL != request;
      request = instance->cb->get_client_request(instance, &suid, false))
  {
    sns_std_request decoded_request;
    sns_std_sensor_config decoded_payload = {0};
    sns_physical_sensor_test_config decoded_payload_phy = sns_physical_sensor_test_config_init_default;

    if(request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      if(lsm6dso_get_decoded_imu_request(this, request, &decoded_request, &decoded_payload))
      {
        float report_rate;
        bool is_max_batch = true;

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
          report_rate = UINT32_MAX;
        }
        *sensor_temp_client_present = true;

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

        if(is_max_batch)
        {
          report_rate = (1.0f / UINT32_MAX);
        }
        *chosen_report_rate = SNS_MAX(*chosen_report_rate, report_rate);

      }
    }
    else if(lsm6dso_get_decoded_self_test_request(this, request, &decoded_request, &decoded_payload_phy)) //check for self test
    {
      if(request->message_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
      {
        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
          "lsm6dso_get_imu_config: Physical sensor test: %d", decoded_payload_phy.test_type);

        // Only COM test is supported.
        if(decoded_payload_phy.test_type != SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
          return;
        inst_state->common_info.mode |= LSM6DSO_MODE_SELF_TEST;
        inst_state->self_test_info.test_type = decoded_payload_phy.test_type;
        inst_state->sensor_temp_info.self_test_is_successful = true;
      }
    }
  }
  inst_state->sensor_temp_info.report_rate_hz  = *chosen_report_rate;
  inst_state->sensor_temp_info.desired_sampling_rate_hz = *chosen_sample_rate;
  //this would be useful for terminating temp sensor
  if(*chosen_sample_rate == 0)
    inst_state->sensor_temp_info.sampling_intvl = 0;

}

static void lsm6dso_set_inst_config(sns_sensor *this,
    sns_sensor_instance *instance,
    lsm6dso_sensor_type sensor,
    float chosen_report_rate,
    float chosen_sample_rate,
    uint64_t chosen_flush_ticks,
    sns_lsm6dso_registry_cfg registry_cfg,
    uint32_t message_id)
{
  lsm6dso_instance_state *inst_state =
    (lsm6dso_instance_state*)instance->state->state;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_request req_config;
  sns_diag_service* diag = state->diag_service;
  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__, "set_inst_config:: sensor=%u", sensor);
  if(((sensor == LSM6DSO_ACCEL) ||
      (sensor == LSM6DSO_GYRO) ||
      (sensor == LSM6DSO_MOTION_DETECT) ||
      (sensor == LSM6DSO_SENSOR_TEMP)) &&
      (inst_state->common_info.mode & LSM6DSO_MODE_SELF_TEST) &&
      (!inst_state->self_test_info.test_alive)) {
    sns_lsm6dso_self_test_req client_config;
    client_config.test_type = inst_state->self_test_info.test_type;
    req_config.message_id = SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG;
    req_config.request_len = sizeof(sns_lsm6dso_self_test_req);
    req_config.request = &client_config;
    DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
                             "setting self test configuration::  ");
    this->instance_api->set_client_config(instance, &req_config);

  } else if((sensor == LSM6DSO_ACCEL) ||
      (sensor == LSM6DSO_GYRO) ||
      (sensor == LSM6DSO_MOTION_DETECT) ||
      (sensor == LSM6DSO_SENSOR_TEMP)) {
    sns_lsm6dso_req client_config;
    client_config.desired_report_rate = chosen_report_rate;
    client_config.desired_sample_rate = chosen_sample_rate;
    client_config.desired_flush_ticks = chosen_flush_ticks;
    client_config.registry_cfg = registry_cfg;
    req_config.message_id = message_id;
    req_config.request_len = sizeof(sns_lsm6dso_req);
    req_config.request = &client_config;

    DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
              "set_inst_config:: SR*1000=%d RR*1000=%d",
              (uint32_t)(client_config.desired_sample_rate*1000),
              (uint32_t)(client_config.desired_report_rate*1000));
    this->instance_api->set_client_config(instance, &req_config);
  }
  else
    return;

}
static void lsm6dso_turn_rails_off(sns_sensor *this)
{
  sns_sensor *sensor;

  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    lsm6dso_state *sensor_state = (lsm6dso_state*)sensor->state->state;
    if(sensor_state->rail_config.rail_vote != SNS_RAIL_OFF)
    {
      sns_time timeout = sns_convert_ns_to_ticks(LSM6DSO_POWER_RAIL_OFF_TIMEOUT_NS);
      lsm6dso_start_power_rail_timer( sensor, timeout, LSM6DSO_POWER_RAIL_PENDING_OFF );
    }
  }
}
void lsm6dso_update_sensor_config(sns_sensor *this,
    sns_sensor_instance *instance,
    lsm6dso_sensor_type sensor,
    bool client_present)
{
  lsm6dso_instance_state *inst_state =
    (lsm6dso_instance_state*)instance->state->state;

  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_diag_service* diag = state->diag_service;

  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
      "update_sensor_config:: client present %d sensor %d", client_present, sensor);
  if(client_present) {
    if(sensor == LSM6DSO_GYRO) {
      inst_state->fifo_info.fifo_enabled |= LSM6DSO_ACCEL;
    }

    if(sensor != LSM6DSO_MOTION_DETECT) {
      if(sensor != LSM6DSO_SENSOR_TEMP)
        inst_state->fifo_info.fifo_enabled |= sensor;
      inst_state->fifo_info.publish_sensors |= sensor;
    } else {
      inst_state->md_info.md_client_present = true;
    }
  }
}
void lsm6dso_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              lsm6dso_sensor_type sensor_type)
{
  /**
   * 1. Get best Accel Config.
   * 2. Get best Gyro Config.
   * 3. Get best Motion Accel Config.
   * 4. Get best Sensor Temperature Config.
   * 5. Decide best Instance Config based on above outputs.
   */

  float chosen_sample_rate = 0.0f;
  float chosen_report_rate = 0.0f;
  uint64_t chosen_flush_ticks = 0;
  uint32_t message_id = 0;
  lsm6dso_instance_state *inst_state =
    (lsm6dso_instance_state*)instance->state->state;
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_diag_service* diag = state->diag_service;
  sns_lsm6dso_registry_cfg registry_cfg = {.sensor_type = sensor_type};

  uint8_t sensor_count = ARR_SIZE(lsm6dso_supported_sensors);
  struct {
    lsm6dso_sensor_type sensor;
    float sample_rate;
    float report_rate;
    uint64_t flush_period_ticks;
    bool ngated_client_present; //= client_present
    bool gated_client_present; //incase of accel and md
  } sensor_info[sensor_count];
  int i = 0;
  /** Start with a clean slate */
  inst_state->fifo_info.fifo_enabled = 0;
  inst_state->fifo_info.publish_sensors = 0;

  DBG_PRINT(diag, this, LOW, __FILENAME__, __LINE__,
      "lsm6dso_reval_instance_config sensor %d state->sensor %d", sensor_type, state->sensor);
  for(; i< sensor_count ; i++) {
    sensor_info[i].gated_client_present = false;
    sensor_info[i].ngated_client_present = false;
    sensor_info[i].sensor = lsm6dso_supported_sensors[i].sensor;
    if((sensor_info[i].sensor == LSM6DSO_ACCEL) ||
        (sensor_info[i].sensor == LSM6DSO_GYRO)) {
      lsm6dso_get_imu_config(this, instance, sensor_info[i].sensor,
          &sensor_info[i].sample_rate,
          &sensor_info[i].report_rate,
          &sensor_info[i].flush_period_ticks,
          &sensor_info[i].ngated_client_present,
          &sensor_info[i].gated_client_present);
      message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
      //ignore gyro gated request if any
      if(sensor_info[i].sensor == LSM6DSO_ACCEL) {
        inst_state->accel_info.gated_client_present = sensor_info[i].gated_client_present;
      }
    } else if(sensor_info[i].sensor == LSM6DSO_MOTION_DETECT) {
      lsm6dso_get_motion_detect_config(this,
          instance,
          &sensor_info[i].ngated_client_present);
      //QC change to handle pure MD clients
      if(sensor_info[i].ngated_client_present) {
        message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG;
        sensor_info[i].report_rate = 1/LSM6DSO_MD_REPORT_PERIOD;
        sensor_info[i].sample_rate = LSM6DSO_MD_SAMPLE_RATE;
        sensor_info[i].flush_period_ticks = 0;
      } else {
        sensor_info[i].report_rate = 0;
        sensor_info[i].sample_rate = 0;
        sensor_info[i].flush_period_ticks = 0;
      }
    } else if(sensor_info[i].sensor == LSM6DSO_SENSOR_TEMP) {
      lsm6dso_get_sensor_temp_config(this, instance,
          &sensor_info[i].sample_rate,
          &sensor_info[i].report_rate,
          &sensor_info[i].flush_period_ticks,
          &sensor_info[i].ngated_client_present);
      message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
    } else {
      DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
          "config:Unknown sensor %d", sensor_info[i].sensor);
    }

    DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
        "sensor %d gclient %d non_gclient %d",  sensor_info[i].sensor, sensor_info[i].gated_client_present, sensor_info[i].ngated_client_present);
    chosen_sample_rate = SNS_MAX(chosen_sample_rate, sensor_info[i].sample_rate);
    chosen_report_rate = SNS_MAX(chosen_report_rate, sensor_info[i].report_rate);
    chosen_flush_ticks = SNS_MAX(chosen_flush_ticks, sensor_info[i].flush_period_ticks);
    DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
        "config: %d %d %d", sensor_info[i].sensor, (int)(sensor_info[i].sample_rate*1000), (int)(sensor_info[i].report_rate*1000));
  }
  for(i = 0; i< sensor_count ; i++) {
    lsm6dso_update_sensor_config(this, instance, sensor_info[i].sensor, sensor_info[i].ngated_client_present);
  }
  //special case if gated client is present
  //do not set publish sensors if gated, it is handled while sending data
  if(inst_state->accel_info.gated_client_present)
    inst_state->fifo_info.fifo_enabled |= LSM6DSO_ACCEL;
  if(inst_state->fifo_info.publish_sensors & LSM6DSO_ACCEL)
    inst_state->md_info.enable_md_int = false;


  DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
        "fifo_enabled 0x%x fifo_enabled 0x%x publish sensors 0x%x ",
        inst_state->fifo_info.fifo_enabled, inst_state->fifo_info.fifo_enabled,inst_state->fifo_info.publish_sensors);

  if(chosen_report_rate > 0.0 && chosen_sample_rate > 0.0)
  {
    // TODO: This is for DAE watermark. Set max WM to UINT32_MAX if all clients
    // are SNS_STD_DELIVERY_FLUSH.
    uint32_t wm = (uint32_t)(chosen_sample_rate / chosen_report_rate);
    inst_state->fifo_info.max_requested_wmk = SNS_MAX(1, wm);
  }

  if(LSM6DSO_ACCEL == sensor_type || LSM6DSO_GYRO == sensor_type ||
     LSM6DSO_SENSOR_TEMP == sensor_type)
  {
    sns_memscpy(registry_cfg.fac_cal_bias, sizeof(registry_cfg.fac_cal_bias),
                state->fac_cal_bias, sizeof(state->fac_cal_bias));

    sns_memscpy(&registry_cfg.fac_cal_corr_mat, sizeof(registry_cfg.fac_cal_corr_mat),
                &state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat));
  }

  lsm6dso_set_inst_config(this,
      instance,
      sensor_type,
      chosen_report_rate,
      chosen_sample_rate,
      chosen_flush_ticks,
      registry_cfg,
      message_id);

  // Check if any clients are present before turning off the rails
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
    lsm6dso_turn_rails_off(this);
    inst_state->instance_is_ready_to_configure = false;
  }
}

/** See sns_lsm6dso_sensor.h */
sns_sensor_instance* lsm6dso_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  sns_sensor_instance *instance = sns_sensor_util_get_shared_instance(this);
  sns_diag_service* diag = state->diag_service;  sns_time on_timestamp;
  sns_time delta;
  bool reval_config = false;

  DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
            "lsm6dso_set_client_request:: exist=%x new=%x msg=%u remove=%u",
            exist_request, new_request,
            (new_request != NULL) ? new_request->message_id :
            ((exist_request != NULL) ? exist_request->message_id : 0), remove);

  if(remove)
  {
    if(NULL == instance) {
      DBG_PRINT(diag, this, ERROR, __FILENAME__, __LINE__,
          "lsm6dso_set_client_request:: Instance not available! Returning!");
      return instance;
    }

    DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
        "lsm6dso_set_client_request:: remove request");

    lsm6dso_instance_state *inst_state =
      (lsm6dso_instance_state*)instance->state->state;
    inst_state->config_sensors |= state->sensor;

    instance->cb->remove_client_request(instance, exist_request);
    /* Assumption: The FW will call deinit() on the instance before destroying it.
       Putting all HW resources (sensor HW, COM port, power rail)in
       low power state happens in Instance deinit().*/
    if(exist_request->message_id != SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
    {
      lsm6dso_reval_instance_config(this, instance, state->sensor);
    }
    else
    {
      lsm6dso_instance_state *inst_state =
        (lsm6dso_instance_state*)instance->state->state;

      //if reconfigure hw has been postponed due to a remove request during self test. Do it now
      if(inst_state->self_test_info.reconfig_postpone)
      {
        DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                        "Reconfiguring HW for request recieved during self-test");
        lsm6dso_reval_instance_config(this, instance, state->sensor);
        inst_state->fifo_info.interrupt_cnt = 0;
        inst_state->self_test_info.reconfig_postpone = false;
      }

      //If a factory self test was run, update the registry & sensor->state
      if(inst_state->self_test_info.update_registry)
      {
        DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                "Updating registry and sensor state with new calibration values");

        //copy to sensor state
        if(LSM6DSO_ACCEL == inst_state->self_test_info.sensor)
        {
          sns_memscpy(state->fac_cal_bias, sizeof(state->fac_cal_bias),
                      inst_state->accel_registry_cfg.fac_cal_bias, sizeof(inst_state->accel_registry_cfg.fac_cal_bias));
        }
        else if(LSM6DSO_GYRO == inst_state->self_test_info.sensor)
        {
          sns_memscpy(state->fac_cal_bias, sizeof(state->fac_cal_bias),
                      inst_state->gyro_registry_cfg.fac_cal_bias, sizeof(inst_state->gyro_registry_cfg.fac_cal_bias));
        }

        //Update flag in sensor_state to indicate which registry is to be updated

        //write registry
        inst_state->registry_reset.request = false;
        lsm6dso_sensor_write_output_to_registry(this, instance);

        // Set the flag to false indicating that the registry is updated
        inst_state->self_test_info.update_registry = false;
      }
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

    DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
        "lsm6dso_set_client_request:: add request ");
    if(NULL != instance)
    {
      lsm6dso_instance_state *inst_state =
        (lsm6dso_instance_state*)instance->state->state;
      if(inst_state->self_test_info.test_alive)
      {
        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
            "Self test is running. Ignoring new request! ");
        return NULL;  // Return without honouring any request for any sensor streaming
      }
    }
    if(NULL == instance)
    {
      if(state->sensor == LSM6DSO_GYRO)
      {
        state->rail_config.rail_vote = SNS_RAIL_ON_NPM;
      }
      else
      {
        state->rail_config.rail_vote = SNS_RAIL_ON_LPM;
      }
      state->pwr_rail_service->api->sns_vote_power_rail_update(
          state->pwr_rail_service,
          this,
          &state->rail_config,
          &on_timestamp);

      delta = sns_get_system_time() - on_timestamp;

      // Use on_timestamp to determine correct Timer value.
      if(delta < sns_convert_ns_to_ticks(LSM6DSO_OFF_TO_IDLE_MS*1000*1000))
      {
        DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "lsm6dso_set_client_request:: start power rail timer");
        lsm6dso_start_power_rail_timer(this,
            sns_convert_ns_to_ticks(LSM6DSO_OFF_TO_IDLE_MS*1000*1000) - delta,
            LSM6DSO_POWER_RAIL_PENDING_SET_CLIENT_REQ);
      } else {
        // rail is already ON
        DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "lsm6dso_set_client_request::  power rail already ON");
        state->power_rail_pend_state = LSM6DSO_POWER_RAIL_PENDING_NONE;
        reval_config = true;

      }
      DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
          "lsm6dso_set_client_request::  creating instance");
      /** create_instance() calls init() for the Sensor Instance */
      instance = this->cb->create_instance(this,
          sizeof(lsm6dso_instance_state));


      /* If rail is already ON then flag instance OK to configure */
      if(reval_config)
      {
        lsm6dso_instance_state *inst_state =
          (lsm6dso_instance_state*)instance->state->state;

        inst_state->instance_is_ready_to_configure = true;
      }

    }
    else
    {
      DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
          "lsm6dso_set_client_request::  instance already available");
      if(NULL != new_request)
        DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
            "sensor %d new_req msg_id %d",state->sensor, new_request->message_id);
      if(NULL != exist_request
          &&
          NULL != new_request
          &&
          new_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
      {
        lsm6dso_instance_state *inst_state =
          (lsm6dso_instance_state*)instance->state->state;

        if(inst_state->fifo_info.fifo_enabled &&
          ((state->sensor == LSM6DSO_ACCEL) || (state->sensor == LSM6DSO_GYRO)))
        {
          lsm6dso_send_flush_config(this, instance);
          /** Do not update instance client request list at this point
            because FIFO flush is a transitory request for an on-going
            stream request. */
          return instance;
        }
        else
        {
          /** There aren't any FIFO sensors enabled to support flush.
           *  Send flush complete event anyway. */
          lsm6dso_send_fifo_flush_done(instance, &state->my_suid);
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
      lsm6dso_instance_state *inst_state =
        (lsm6dso_instance_state*)instance->state->state;
      if(NULL != new_request)
      {
        instance->cb->add_client_request(instance, new_request);

        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
            "lsm6dso_set_client_request::  adding new client request reval_config = %d inst_ready_to_config = %d", reval_config, inst_state->instance_is_ready_to_configure);
        DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
            "lsm6dso_set_client_request::  adding new client request sensor %d msg_id %d", state->sensor, new_request->message_id);
        if(LSM6DSO_MOTION_DETECT == state->sensor) {
          sns_memscpy(&inst_state->md_info.md_config, sizeof(inst_state->md_info.md_config),
              &state->md_config, sizeof(state->md_config));
          DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
              "lsm6dso_set_client_request:: copying md config");
        }

        if(new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG
            ||
            new_request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG) {
          inst_state->config_sensors |= state->sensor;
          //copy range/resolution to inst state
          if(LSM6DSO_ACCEL == state->sensor)
          {
            inst_state->accel_info.sstvt = lsm6dso_accel_resolutions[state->accel_resolution_idx]*1000;  //convert to micro-g/LSB
            inst_state->accel_info.range = lsm6dso_accel_ranges[state->accel_resolution_idx];
            inst_state->accel_info.range_idx = state->accel_resolution_idx;
          }
          else if(LSM6DSO_GYRO == state->sensor)
          {
            inst_state->gyro_info.sstvt = lsm6dso_gyro_resolutions[state->gyro_resolution_idx];
            inst_state->gyro_info.range = lsm6dso_gyro_ranges[state->gyro_resolution_idx];
            inst_state->gyro_info.range_idx = state->gyro_resolution_idx;
          }
        }
        if(new_request->message_id == SNS_CAL_MSGID_SNS_CAL_RESET) {
          DBG_PRINT(diag, this, HIGH, __FILENAME__, __LINE__,
              "Received event: SNS_CAL_MSGID_SNS_CAL_RESET");
          inst_state->registry_reset.request = true;
          inst_state->registry_reset.sensor_type = state->sensor;
          lsm6dso_sensor_write_output_to_registry(this, instance);

          //copy to sensor state
          if(LSM6DSO_ACCEL == state->sensor)
          {
            sns_memscpy(state->fac_cal_bias, sizeof(state->fac_cal_bias),
                        inst_state->accel_registry_cfg.fac_cal_bias, sizeof(inst_state->accel_registry_cfg.fac_cal_bias));
            sns_memscpy(&state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat),
                        &inst_state->accel_registry_cfg.fac_cal_corr_mat, sizeof(inst_state->accel_registry_cfg.fac_cal_corr_mat));
          }
          else if(LSM6DSO_GYRO == state->sensor)
          {
            sns_memscpy(state->fac_cal_bias, sizeof(state->fac_cal_bias),
                        inst_state->gyro_registry_cfg.fac_cal_bias, sizeof(inst_state->gyro_registry_cfg.fac_cal_bias));
            sns_memscpy(&state->fac_cal_corr_mat, sizeof(state->fac_cal_corr_mat),
                        &inst_state->gyro_registry_cfg.fac_cal_corr_mat, sizeof(inst_state->gyro_registry_cfg.fac_cal_corr_mat));
          }

          lsm6dso_send_cal_event(instance, state->sensor);
        }

        if(new_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG
           &&
           state->sensor == LSM6DSO_MOTION_DETECT)
        {
          if(inst_state->fifo_info.publish_sensors & LSM6DSO_ACCEL) {
            //send event as MD disabled since non-gated client is active
            //no need of this as we alreay set md_info state
            sns_motion_detect_event md_state;
            md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;

            DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                "send MD_event =%d",
                md_state.motion_detect_event_type);
            pb_send_event(instance,
                sns_motion_detect_event_fields,
                &md_state,
                sns_get_system_time(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &inst_state->md_info.suid);
            reval_config = false;
          } else if (inst_state->md_info.enable_md_int) {
            //there is exsisting md client already present, just send event
            DBG_PRINT(diag, this, MED, __FILENAME__, __LINE__,
                "send MD_event =%d",
                inst_state->md_info.cur_md_state.motion_detect_event_type);
            pb_send_event(instance,
                sns_motion_detect_event_fields,
                &inst_state->md_info.cur_md_state,
                sns_get_system_time(),
                SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                &inst_state->md_info.suid);
            reval_config = false;
          } else
            inst_state->md_info.md_new_req = true;
        }
      }
      if(reval_config && inst_state->instance_is_ready_to_configure)
      {
        lsm6dso_reval_instance_config(this, instance, state->sensor);
      }
    }
  }
  if(NULL != instance) {
    lsm6dso_instance_state *inst_state =
      (lsm6dso_instance_state*)instance->state->state;
    //reset config sensor bit if sensor is not present if fifo enabled
    if(!(inst_state->fifo_info.fifo_enabled & state->sensor))
      inst_state->config_sensors &= ~state->sensor;
  }

  // Sensors are required to call remove_instance when clientless
  if(NULL != instance &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)ACCEL_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)MOTION_DETECT_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)GYRO_SUID, true) &&
     NULL == instance->cb->get_client_request(instance,
       &(sns_sensor_uid)SENSOR_TEMPERATURE_SUID, true))
  {
    this->cb->remove_instance(instance);
  }
  return instance;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/
sns_sensor_api lsm6dso_accel_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6dso_accel_init,
  .deinit             = &lsm6dso_accel_deinit,
  .get_sensor_uid     = &lsm6dso_get_sensor_uid,
  .set_client_request = &lsm6dso_set_client_request,
  .notify_event       = &lsm6dso_sensor_notify_event,
};
sns_sensor_api lsm6dso_gyro_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6dso_gyro_init,
  .deinit             = &lsm6dso_gyro_deinit,
  .get_sensor_uid     = &lsm6dso_get_sensor_uid,
  .set_client_request = &lsm6dso_set_client_request,
  .notify_event       = &lsm6dso_sensor_notify_event,
};
sns_sensor_api lsm6dso_motion_detect_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6dso_motion_detect_init,
  .deinit             = &lsm6dso_motion_detect_deinit,
  .get_sensor_uid     = &lsm6dso_get_sensor_uid,
  .set_client_request = &lsm6dso_set_client_request,
  .notify_event       = &lsm6dso_sensor_notify_event,
};

sns_sensor_api lsm6dso_sensor_temp_sensor_api =
{
  .struct_len         = sizeof(sns_sensor_api),
  .init               = &lsm6dso_sensor_temp_init,
  .deinit             = &lsm6dso_sensor_temp_deinit,
  .get_sensor_uid     = &lsm6dso_get_sensor_uid,
  .set_client_request = &lsm6dso_set_client_request,
  .notify_event       = &lsm6dso_sensor_notify_event,
};

