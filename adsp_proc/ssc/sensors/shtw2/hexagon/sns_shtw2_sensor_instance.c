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
 *******************************************************************************/
#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_shtw2_hal.h"
#include "sns_shtw2_sensor.h"
#include "sns_shtw2_sensor_instance.h"
#include "sns_stream_service.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_timer.pb.h"
#include "sns_types.h"

static void shtw2_cleanup_instance(shtw2_instance_state *state,
                                   sns_stream_service *stream_mgr)
{
  if(NULL != state->one_shot_timer_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->one_shot_timer_data_stream);
    state->one_shot_timer_data_stream = NULL;
  }
  if(NULL != state->periodic_timer_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->periodic_timer_data_stream);
    state->periodic_timer_data_stream = NULL;
  }
  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
}

/**
 * Initialize a Sensor Instance to its default state.  A call to
 * sns_sensor_instance_api::deinit will precede any subsequent calls
 * to this function.
 *
 * @note Persistent configuration can be made available using the
 * sensor_state; this state will not be available again until deinit.
 *
 * @param[io] this Sensor Instance reference
 * @param[i] sensor_state State of the Sensor which created this Instance
 *
 * @return
 * SNS_RC_NOT_AVAILABLE - Sensor state does not allow for this operation
 * SNS_RC_SUCCESS
 *
 * See sns_sensor_instance.h.
 */
sns_rc shtw2_inst_init(sns_sensor_instance *const this,
                       sns_sensor_state const *sstate)
{
  shtw2_instance_state *state =
      (shtw2_instance_state*)this->state->state;
  shtw2_state *sensor_state =
      (shtw2_state*)sstate->state;
  sns_rc rc = SNS_RC_SUCCESS;
  uint64_t buffer[10];
  float data[1];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  state->diag_service = (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
  state->scp_service = (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  sns_diag_service* diag = state->diag_service;
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[SHTW2_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = SHTW2_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  /** Determine size of sns_diag_sensor_state_raw as defined in
   *  sns_diag.proto
   *  sns_diag_sensor_state_raw is a repeated array of samples of
   *  type sns_diag_batch sample. The following determines the
   *  size of sns_diag_sensor_state_raw with a single batch
   *  sample
   */
  if(pb_encode_tag(&stream, PB_WT_STRING,
                   sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                           &batch_sample))
    {
      state->log_raw_encoded_size = stream.bytes_written;
    }
  }

  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                         "SHTW2 instance init");

  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 sensor_state->timer_suid,
                                                 &state->one_shot_timer_data_stream);
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 sensor_state->timer_suid,
                                                 &state->periodic_timer_data_stream);
  state->timer_suid = sensor_state->timer_suid;

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->com_port_info.com_config,
              sizeof(sensor_state->com_port_info.com_config));
  /** QC Check the com port register return value */
  rc = state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                          &state->com_port_info.port_handle);

  if(NULL == state->one_shot_timer_data_stream ||
     NULL == state->periodic_timer_data_stream ||
     NULL == state->com_port_info.port_handle ||
     (rc != SNS_RC_SUCCESS))
  {
    shtw2_cleanup_instance(state, stream_mgr);
    return SNS_RC_FAILED;
  }

  /**----------- Copy all Sensor UIDs in instance state -------------*/
  sns_memscpy(&state->temperature_info.suid,
              sizeof(state->temperature_info.suid),
              &((sns_sensor_uid)TEMPERATURE_SUID),
              sizeof(state->temperature_info.suid));
  sns_memscpy(&state->humidity_info.suid,
              sizeof(state->humidity_info.suid),
              &((sns_sensor_uid)HUMIDITY_SUID),
              sizeof(state->humidity_info.suid));


  state->current_odr = SHTW2_ODR_0;
  state->timer_state = IDLE;
  state->encoded_sensor_event_len = pb_get_encoded_size_sensor_stream_event(data, 1);
  state->temperature_info.enabled = false;
  state->humidity_info.enabled = false;
  state->new_self_test_request = false;

  /* Initialize factory calibration */
  state->temperature_registry_cfg.fac_cal_scale[0] = 1.0;
  state->temperature_registry_cfg.fac_cal_bias[0] = 0.0;
  state->temperature_registry_cfg.threshold = SHTW2_TEMP_CHANGE;
  state->humidity_registry_cfg.fac_cal_scale[0] = 1.0;
  state->humidity_registry_cfg.fac_cal_bias[0] = 0.0;
  state->humidity_registry_cfg.threshold = SHTW2_HUMIDITY_CHANGE;

  state->diag_service = (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  state->scp_service = (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  return SNS_RC_SUCCESS;
}

/**
 * Release all hardware and software resources associated with this Sensor
 * Instance.
 *
 * @param[io] this Sensor Instance reference
 * @param[i] sensor_state State of the Sensor which created this Instance
 *
 * @return
 * SNS_RC_INVALID_STATE - Error occurred: some resource could not be released
 * SNS_RC_SUCCESS
 *
 * See sns_sensor_instance.h.
 */
sns_rc shtw2_inst_deinit(sns_sensor_instance *const this)
{
  shtw2_instance_state *state = (shtw2_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  shtw2_cleanup_instance(state, stream_mgr);

  return SNS_RC_SUCCESS;
}

static sns_rc shtw2_send_config_event(sns_sensor_instance *const this)
{
  shtw2_instance_state *state = (shtw2_instance_state*) this->state->state;

  sns_std_sensor_physical_config_event phy_sensor_config =
      sns_std_sensor_physical_config_event_init_default;
  char operating_mode[] = SHTW2_NORMAL;
  pb_buffer_arg op_mode_args;

  op_mode_args.buf                     = &operating_mode[0];
  op_mode_args.buf_len                 = sizeof(operating_mode);

  phy_sensor_config.has_sample_rate = true;
  phy_sensor_config.sample_rate = state->current_odr;
  phy_sensor_config.has_water_mark = false;
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = SHTW2_ACTIVE_CURRENT_UA;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.has_stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = false;
  phy_sensor_config.has_DAE_watermark = false;

  if(state->humidity_info.enabled)
  {
    phy_sensor_config.resolution = SHTW2_HUMIDITY_RESOLUTION;
    phy_sensor_config.range[0] = SHTW2_HUMIDITY_RANGE_MIN;
    phy_sensor_config.range[1] = SHTW2_HUMIDITY_RANGE_MAX;

    pb_send_event(this,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->humidity_info.suid);
  }

  if(state->temperature_info.enabled)
  {
    phy_sensor_config.resolution = SHTW2_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range[0] = SHTW2_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = SHTW2_TEMPERATURE_RANGE_MAX;

    pb_send_event(this,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->temperature_info.suid);
  }
  return SNS_RC_SUCCESS;
}

static void shtw2_match_odr(float desired_sample_rate,
                            float *chosen_sample_rate)
{
  if(desired_sample_rate <= SHTW2_ODR_0)
  {
    *chosen_sample_rate = SHTW2_ODR_0;
  }
  else if(desired_sample_rate <= SHTW2_ODR_1)
  {
    *chosen_sample_rate = SHTW2_ODR_1;
  }
  else if(desired_sample_rate <= SHTW2_ODR_5)
  {
    *chosen_sample_rate = SHTW2_ODR_5;
  }
  else
  {
    *chosen_sample_rate = SHTW2_ODR_10;
  }
}

/**
 * Update a Sensor Instance configuration to this sensorRequest.
 *
 * The Sensor Instance is expected to start all dependent streams, timers, etc..
 *
 * @note
 * A Sensor may define any number of unique request types they support.
 * However, a client may only have a single active stream; an enable
 * request can inherently serve as a "reconfiguration" request.
 *
 * @param[io] this Sensor Instance reference
 *
 * @return
 * SNS_RC_INVALID_VALUE - Invalid client request
 * SNS_RC_SUCCESS
 *
 * See sns_sensor_instance.h.
 */
sns_rc shtw2_inst_set_client_config(sns_sensor_instance *const this,
                                    sns_request const *client_request)
{
  shtw2_instance_state *state = (shtw2_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  sns_diag_service* diag = state->diag_service;
  sns_rc rv;
  float desired_odr;
  float chosen_odr;
  float *fac_cal_bias = NULL;
  float *fac_cal_scale = NULL;
  sns_time timeout_ticks;

  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                         "SHTW2 instance set client config");

  /* Turn COM-port on. */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    true);

  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG)
  {
    sns_shtw2_request *payload = (sns_shtw2_request*)client_request->request;
    desired_odr = payload->sample_rate;

    SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                           "Check ODR");
    shtw2_match_odr(desired_odr, &chosen_odr);

    if(desired_odr != chosen_odr)
    {
      SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                             "Desired ODR of %d milli-Hz can not be set, instead using ODR of %d milli-Hz",
                             (int)(desired_odr * 1000), (int)(chosen_odr * 1000));
    }

    if(state->current_odr != chosen_odr)
    {
      SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                             "Change ODR of SHTW2 from %d milli-Hz to %d milli-Hz",
                             (int)(state->current_odr * 1000), (int)(chosen_odr * 1000));

      /* Cancel periodic timer if measurements are already running and only ODR is changed */
      if(state->current_odr != SHTW2_ODR_0)
      {
        shtw2_stop_all_timers(this);
      }

      state->current_odr = chosen_odr;

      if(state->current_odr != SHTW2_ODR_0)
      {
        SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                               "Start measurements");

        /* Start periodic timer */
        if(state->periodic_timer_data_stream == NULL)
        {
          rv = stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                              this,
                                                              state->timer_suid,
                                                              &state->periodic_timer_data_stream);
          if(rv != SNS_RC_SUCCESS)
          {
            SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                                   "Failed to create periodic timer stream");
            return rv;
          }
        }

        timeout_ticks = sns_convert_ns_to_ticks(SHTW2_ODR_TO_INTERVAL_NS(state->current_odr));
        rv = shtw2_measurement_timer_set(timeout_ticks,
                                         state->periodic_timer_data_stream,
                                         true);
        if(rv != SNS_RC_SUCCESS)
        {
          SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                 "Failed to start SHTW2 measurement timer");
          state->timer_state = IDLE;
          return rv;
        }

        /* Return if measurement already running. */
        if(state->timer_state != IDLE) {
          SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                                 "Ignore, a measurement is already in progress");
          state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                            false);
          return SNS_RC_SUCCESS;
        }

        /* Start new measurement. */
        rv = shtw2_init_measurement(this);
        if(rv != SNS_RC_SUCCESS)
        {
          SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                 "Failed to init SHTW2 measurement");
        }
        rv = shtw2_send_config_event(this);
        if(rv != SNS_RC_SUCCESS)
        {
          SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                 "Failed to send config event");
        }
      }
      else
      {
        state->timer_state = IDLE;
        SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                               "Stop measurements");
      }
    }
    else
    {
      rv = shtw2_send_config_event(this);
      SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                             "Current and chosen ODR are identical with %d milli-Hz",
                             (int)(state->current_odr * 1000));
    }

    /* Update registry configuration */
    if(SHTW2_TEMPERATURE == payload->registry_cfg.sensor_type)
    {
      fac_cal_bias = state->temperature_registry_cfg.fac_cal_bias;
      fac_cal_scale = state->temperature_registry_cfg.fac_cal_scale;
      state->temperature_registry_cfg.threshold = payload->registry_cfg.threshold;
    }
    else if(SHTW2_HUMIDITY == payload->registry_cfg.sensor_type)
    {
      fac_cal_bias = state->humidity_registry_cfg.fac_cal_bias;
      fac_cal_scale = state->humidity_registry_cfg.fac_cal_scale;
      state->humidity_registry_cfg.threshold = payload->registry_cfg.threshold;
    }

    if(NULL != fac_cal_bias && NULL != fac_cal_scale)
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));
      sns_memscpy(fac_cal_scale, sizeof(payload->registry_cfg.fac_cal_scale),
                  payload->registry_cfg.fac_cal_scale,
                  sizeof(payload->registry_cfg.fac_cal_scale));
    }
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    shtw2_run_self_test(this);
    state->new_self_test_request = false;
  }
  else
  {
    SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                           "Unknown message id %d", client_request->message_id);
  }

  /* Turn COM port OFF. */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  return SNS_RC_SUCCESS;
}
