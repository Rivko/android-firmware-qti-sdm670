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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/


#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_bmp285_hal.h"
#include "sns_bmp285_sensor.h"
#include "sns_bmp285_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_cal.pb.h"
#include "sns_async_com_port_pb_utils.h"
#if(BMP_CONFIG_ENABLE_DIAG_LOG == 1)
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#endif
#include "sns_sensor_util.h"
#include "sns_sync_com_port_service.h"

void bmp285_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50] = {0};
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };
  sns_rc rc = SNS_RC_SUCCESS;

  BMP_INST_LOG(LOW, this, "bmp285_start_sensor_temp_polling_timer");

  if(NULL == state->temperature_timer_data_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *srtm_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);

    rc = srtm_svc->api->create_sensor_instance_stream(srtm_svc,
          this, state->timer_suid, &state->temperature_timer_data_stream);
  }

  if(SNS_RC_SUCCESS != rc
     || NULL == state->temperature_timer_data_stream)
  {
    SNS_INST_PRINTF(ERROR, this, "failed timer stream create rc = %d", rc);
    return;
  }

  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->temperature_info.sampling_intvl;
  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    state->temperature_timer_data_stream->api->send_request(state->temperature_timer_data_stream, &timer_req);
    state->temperature_info.timer_is_active = true;
  }
  else
  {
    //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}

void bmp285_start_sensor_pressure_polling_timer(sns_sensor_instance *this)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50] = {0};
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };
  sns_rc rc = SNS_RC_SUCCESS;
  BMP_INST_LOG(LOW, this, "bmp285_start_sensor_pressure_polling_timer");

  if(NULL == state->pressure_timer_data_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *srtm_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);

    rc = srtm_svc->api->create_sensor_instance_stream(srtm_svc,
          this, state->timer_suid, &state->pressure_timer_data_stream);
  }

  if(SNS_RC_SUCCESS != rc
     || NULL == state->pressure_timer_data_stream)
  {
    SNS_INST_PRINTF(ERROR, this, "failed timer stream create rc = %d", rc);
    return;
  }

  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->pressure_info.sampling_intvl;
  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    state->pressure_timer_data_stream->api->send_request(state->pressure_timer_data_stream, &timer_req);
    state->pressure_info.timer_is_active = true;
  }
  else
  {
    //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}
void bmp285_set_pressure_polling_config(sns_sensor_instance *const this)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  int err = 0;
  BMP_INST_LOG(LOW, this,
  "pressure_info.timer_is_active:%d state->pressure_info.sampling_intvl:%u",
  state->pressure_info.timer_is_active,
  state->pressure_info.sampling_intvl);
  BMP_INST_LOG(LOW, this, "state->sample_rate_no_need_change = %d",
  state->sample_rate_no_need_change);
  if(state->pressure_info.sampling_intvl > 0)
  {
    if((state->sample_rate_no_need_change != 1))
    {
      err = bmp280_set_power_mode(state,BMP280_FORCED_MODE);
      if(err)
      {
        SNS_INST_PRINTF(ERROR, this,"config power mode failed err = %d", err);
      }

      bmp285_start_sensor_pressure_polling_timer(this);
    }
  }
  else if(state->pressure_info.timer_is_active)
  {
    state->pressure_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this,
                                                  &state->pressure_timer_data_stream);
    bmp280_set_power_mode(state,BMP280_SLEEP_MODE);
  }
}

void bmp285_set_temperature_polling_config(sns_sensor_instance *const this)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  int err = 0;
  BMP_INST_LOG(LOW, this,
  "temperature_info.timer_is_active:%d state->temperature_info.sampling_intvl:%u",
  state->temperature_info.timer_is_active,
  state->temperature_info.sampling_intvl);

  if(state->temperature_info.sampling_intvl > 0)
  {
    err = bmp280_set_power_mode(state,BMP280_FORCED_MODE);
    if(err)
    {
      SNS_INST_PRINTF(ERROR, this,"config power mode failed err = %d", err);
    }
    else
    {
      bmp285_start_sensor_temp_polling_timer(this);
    }
  }
  else if(state->temperature_info.timer_is_active)
  {
    state->temperature_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this,
                                                  &state->temperature_timer_data_stream);
    bmp280_set_power_mode(state,BMP280_SLEEP_MODE);
  }
}

/*!
 *  @brief This API is used to write
 *   the working mode of the sensor
 *  @param v_work_mode_u8 : The value of work mode
 *   value      |  mode
 * -------------|-------------
 *    0         | BMP280_ULTRA_LOW_POWER_MODE
 *    1         | BMP280_LOW_POWER_MODE
 *    2         | BMP280_STANDARD_RESOLUTION_MODE
 *    3         | BMP280_HIGH_RESOLUTION_MODE
 *    4         | BMP280_ULTRA_HIGH_RESOLUTION_MODE
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 */
BMP280_RETURN_FUNCTION_TYPE bmp280_set_work_mode(
  bmp285_instance_state *state,
  u8 v_work_mode_u8)
{
  /* variable used to return communication result*/
  BMP280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
  u8 v_data_u8 = BMP280_INIT_VALUE;
  u32 xfer_bytes;
  /* check the p_bmp280 structure pointer as NULL*/
  if (state == BMP280_NULL) {
    com_rslt = E_BMP280_NULL_PTR;
  } else {
  if (v_work_mode_u8 <= BMP280_ULTRA_HIGH_RESOLUTION_MODE) {
    com_rslt = state->com_read(
        state->scp_service,
        state->com_port_info.port_handle,
        BMP280_CTRL_MEAS_REG, &v_data_u8,
        BMP280_GEN_READ_WRITE_DATA_LENGTH,
        &xfer_bytes);
    if (com_rslt == SUCCESS) {
      switch (v_work_mode_u8) {
      /* write work mode*/
      case BMP280_ULTRA_LOW_POWER_MODE:
        state->oversamp_temperature =
        BMP280_ULTRALOWPOWER_OVERSAMP_TEMPERATURE;
        state->oversamp_pressure =
          BMP280_ULTRALOWPOWER_OVERSAMP_PRESSURE;
        break;
      case BMP280_LOW_POWER_MODE:
        state->oversamp_temperature =
          BMP280_LOWPOWER_OVERSAMP_TEMPERATURE;
        state->oversamp_pressure =
          BMP280_LOWPOWER_OVERSAMP_PRESSURE;
        break;
      case BMP280_STANDARD_RESOLUTION_MODE:
        state->oversamp_temperature =
        BMP280_STANDARDRESOLUTION_OVERSAMP_TEMPERATURE;
        state->oversamp_pressure =
        BMP280_STANDARDRESOLUTION_OVERSAMP_PRESSURE;
        break;
      case BMP280_HIGH_RESOLUTION_MODE:
        state->oversamp_temperature =
        BMP280_HIGHRESOLUTION_OVERSAMP_TEMPERATURE;
        state->oversamp_pressure =
        BMP280_HIGHRESOLUTION_OVERSAMP_PRESSURE;
        break;
      case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
        state->oversamp_temperature =
        BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_TEMPERATURE;
        state->oversamp_pressure =
        BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_PRESSURE;
        break;
      }
      v_data_u8 = BMP280_SET_BITSLICE(v_data_u8,
        BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE,
        state->oversamp_temperature);
      v_data_u8 = BMP280_SET_BITSLICE(v_data_u8,
        BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE,
        state->oversamp_pressure);
      com_rslt = state->com_write(
        state->scp_service,
        state->com_port_info.port_handle,
        BMP280_CTRL_MEAS_REG,
        &v_data_u8, BMP280_GEN_READ_WRITE_DATA_LENGTH,
        &xfer_bytes,
        false);
    }
  } else {
    com_rslt = E_BMP280_OUT_OF_RANGE;
  }
  }
  return com_rslt;
}

void bmp285_reconfig_hw(sns_sensor_instance *this,
  bmp285_sensor_type sensor_type)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  int8_t err = 0;
  BMP_INST_LOG(LOW, this, "bmp285_reconfig_hw state->config_step = %d",state->config_step);
  BMP_INST_LOG(LOW, this,
      "enable sensor flag:0x%x publish sensor flag:0x%x",
      state->deploy_info.enable,
      state->deploy_info.publish_sensors);
  // Enable timer in case of sensor pressure clients
  if (sensor_type == BMP285_PRESSURE)
  //&&
      //!bmp285_dae_if_available(this))
  {
    err = bmp280_set_work_mode(state, BMP280_ULTRA_HIGH_RESOLUTION_MODE);
    if (err)
      SNS_INST_PRINTF(ERROR, this, "set oversample failed error = %d", err);
    bmp285_set_pressure_polling_config(this);
  }
  // Enable timer in case of sensor pressure clients
  if (sensor_type == BMP285_TEMPERATURE)
  //&&
     // !bmp285_dae_if_available(this))
  {
    err = bmp280_set_work_mode(state, BMP280_ULTRA_HIGH_RESOLUTION_MODE);
    if (err)
      SNS_INST_PRINTF(ERROR, this, "set oversample failed error = %d", err);
    bmp285_set_temperature_polling_config(this);
  }
  //if(state->deploy_info.publish_sensors != 0)
  //{
  //   bmp285_dae_if_start_streaming(this);
  // }
  state->config_step = BMP285_CONFIG_IDLE; /* done with reconfig */
  BMP_INST_LOG(LOW, this, "bmp285_reconfig_hw finished");
}

// TODO: Driver must use bias/scale factor as read from registry. Current values are
// placeholders.
void bmp285_send_cal_event(sns_sensor_instance *const instance, bmp285_sensor_type sensor_type)
{
  bmp285_instance_state *state =
     (bmp285_instance_state*)instance->state->state;

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[1] = {0};

  if(sensor_type == BMP285_PRESSURE)
  {
    bias_data[0] = state->sensor_pressure_registry_cfg.fac_cal_bias[0];
  }
  else
  {
    bias_data[0] = state->sensor_temp_registry_cfg.fac_cal_bias[0];
  }

  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(sensor_type == BMP285_PRESSURE)
  {
    suid_current = &state->pressure_info.suid;
  }
  else
  {
    suid_current = &state->temperature_info.suid;
  }

  new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  BMP_INST_LOG(LOW, instance, "Calibration event");
  pb_send_event(instance,
                sns_cal_event_fields,
                &new_calibration_event,
                sns_get_system_time(),
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                suid_current);
}

void bmp285_send_config_event(sns_sensor_instance *const instance)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  sns_std_sensor_physical_config_event phy_sensor_config =
  sns_std_sensor_physical_config_event_init_default;
  BMP_INST_LOG(LOW, instance, "state->deploy_info.publish_sensors %d", state->deploy_info.publish_sensors);

  char operating_mode[] = "NORMAL";

  pb_buffer_arg op_mode_args;

  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  phy_sensor_config.has_sample_rate = true;
  phy_sensor_config.has_water_mark = false;
  phy_sensor_config.water_mark = 1;
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.has_stream_is_synchronous = true;
  phy_sensor_config.stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = true;
  phy_sensor_config.dri_enabled = true;
  if (state->deploy_info.publish_sensors & BMP285_PRESSURE)
  {
    phy_sensor_config.sample_rate = state->pressure_info.sampling_rate_hz;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 720;
    phy_sensor_config.resolution = BMP285_SENSOR_PRESSURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMP285_SENSOR_PRESSURE_RANGE_MIN;
    phy_sensor_config.range[1] = BMP285_SENSOR_PRESSURE_RANGE_MAX;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;
    pb_send_event(
        instance,
        sns_std_sensor_physical_config_event_fields,
        &phy_sensor_config,
        sns_get_system_time(),
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
        &state->pressure_info.suid);
    bmp285_send_cal_event(instance, BMP285_PRESSURE);
  }
  if(state->deploy_info.publish_sensors & BMP285_TEMPERATURE)
  {
    phy_sensor_config.sample_rate = state->temperature_info.sampling_rate_hz;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 325;
    phy_sensor_config.resolution = BMP285_SENSOR_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMP285_SENSOR_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = BMP285_SENSOR_TEMPERATURE_RANGE_MAX;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;
    pb_send_event(
        instance,
        sns_std_sensor_physical_config_event_fields,
        &phy_sensor_config,
        sns_get_system_time(),
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
        &state->temperature_info.suid);
    bmp285_send_cal_event(instance, BMP285_TEMPERATURE);
  }
}

static sns_rc bmp285_validate_sensor_pressure_odr(sns_sensor_instance *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  BMP_INST_LOG(LOW, this, "pressure odr = %d", (int8_t)state->pressure_info.sampling_rate_hz);
  if(state->pressure_info.sampling_rate_hz > BME_ODR_0
     &&
     state->pressure_info.sampling_rate_hz <= BME_ODR_1)
  {
    state->pressure_info.sampling_rate_hz = BME_ODR_1;
  }
  else if(state->pressure_info.sampling_rate_hz > BME_ODR_1
          &&
          state->pressure_info.sampling_rate_hz <= BME_ODR_5)
  {
    state->pressure_info.sampling_rate_hz = BME_ODR_5;
  }
  else if(state->pressure_info.sampling_rate_hz > BME_ODR_5
          &&
          state->pressure_info.sampling_rate_hz <= BME_ODR_10)
  {
    state->pressure_info.sampling_rate_hz = BME_ODR_10;
  }
  else if(state->pressure_info.sampling_rate_hz > BME_ODR_10)
  {
    state->pressure_info.sampling_rate_hz = BME_ODR_25;
  }
  else
  {
    state->pressure_info.sampling_intvl = 0;
    state->pressure_info.timer_is_active = 0;
    BMP_INST_LOG(LOW, this, "close pressure sensor = %d, timer_is_active =%d",
              (uint32_t)state->pressure_info.sampling_rate_hz, state->pressure_info.timer_is_active);
    rc = SNS_RC_NOT_SUPPORTED;
  }

  if (rc == SNS_RC_SUCCESS)
  {
    state->pressure_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->pressure_info.sampling_rate_hz);
    BMP_INST_LOG(LOW, this, "pressure timer_value = %u", (uint32_t)state->pressure_info.sampling_intvl);
  }

  return rc;
}

static sns_rc bmp285_validate_sensor_temp_odr(sns_sensor_instance *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  BMP_INST_LOG(LOW, this, "temperature odr = %d", (int8_t)state->temperature_info.sampling_rate_hz);
  if(state->temperature_info.sampling_rate_hz > BME_ODR_0
     &&
     state->temperature_info.sampling_rate_hz <= BME_ODR_1)
  {
    state->temperature_info.sampling_rate_hz = BME_ODR_1;
  }
  else if(state->temperature_info.sampling_rate_hz > BME_ODR_1)
  {
    state->temperature_info.sampling_rate_hz = BME_ODR_5;
  }
  else
  {
    state->temperature_info.sampling_intvl = 0;
    state->temperature_info.timer_is_active = 0;
    BMP_INST_LOG(LOW, this, "close temperature sensor = %d, timer_is_active =%d",
           (uint32_t)state->temperature_info.sampling_rate_hz, state->temperature_info.timer_is_active);
    rc = SNS_RC_NOT_SUPPORTED;
  }
  if (rc == SNS_RC_SUCCESS)
  {
    state->temperature_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->temperature_info.sampling_rate_hz);
    BMP_INST_LOG(LOW, this, "temperature timer_value = %u", (uint32_t)state->temperature_info.sampling_intvl);
  }

  return rc;
}

static void inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr)
{
  bmp285_instance_state *state = (bmp285_instance_state*)this->state->state;
  #if BMP_CONFIG_ENABLE_DAE
  bmp285_dae_if_deinit(state, stream_mgr);
  #else
  UNUSED_VAR(stream_mgr);
  #endif
  sns_sensor_util_remove_sensor_instance_stream(this, &state->async_com_port_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->pressure_timer_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->temperature_timer_data_stream);

  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
}
sns_rc bmp285_inst_init(sns_sensor_instance * const this,
    sns_sensor_state const *sstate)
{
  bmp285_instance_state *state =
              (bmp285_instance_state*) this->state->state;
  bmp285_state *sensor_state =
              (bmp285_state*) sstate->state;
  float stream_data[1] = {0};
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
#if(BMP_CONFIG_ENABLE_DIAG_LOG == 1)
  uint64_t buffer[10];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[BMP285_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = BMP285_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
#endif
  sns_sensor_uid ascp_suid;
#if BMP_CONFIG_ENABLE_DAE
  sns_sensor_uid dae_suid;
#endif
  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
  #if BMP_CONFIG_ENABLE_ISLAND_MODE
  state->island_service = (sns_island_service*)
              service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);
  #endif
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "async_com_port", &ascp_suid);
  BMP_INST_LOG(LOW, this, "<sns_see_if__  init> from sensor:0x%x", sensor_state->sensor);
  /**---------Setup stream connections with dependent Sensors---------*/

  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 ascp_suid,
                                                 &state->async_com_port_data_stream);

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->common.com_port_info.com_config,
              sizeof(sensor_state->common.com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);

  if(NULL == state->async_com_port_data_stream ||
     NULL == state->com_port_info.port_handle)
  {
    inst_cleanup(this, stream_mgr);
    return SNS_RC_FAILED;
  }

  /**----------- Copy all Sensor UIDs in instance state -------------*/
  sns_memscpy(&state->pressure_info.suid,
              sizeof(state->pressure_info.suid),
              &((sns_sensor_uid)PRESSURE_SUID),
              sizeof(state->pressure_info.suid));
  sns_memscpy(&state->temperature_info.suid,
              sizeof(state->temperature_info.suid),
              &((sns_sensor_uid)TEMPERATURE_SUID),
              sizeof(state->temperature_info.suid));
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer", &state->timer_suid);


  /** Copy calibration data*/
  sns_memscpy(&state->calib_param,
              sizeof(state->calib_param),
              &sensor_state->common.calib_param,
              sizeof(sensor_state->common.calib_param));
  state->interface = sensor_state->common.com_port_info.com_config.bus_instance;
  state->op_mode = FORCED_MODE;/*default working mode*/
  state->com_read = bmp285_com_read_wrapper;/* com read function*/
  state->com_write = bmp285_com_write_wrapper;/*com write function*/
  /* set the data report length to the framework */
  state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(stream_data, 1);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  state->scp_service =  (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);


  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);


  /** Configure the Async Com Port */
  {
    sns_data_stream* data_stream = state->async_com_port_data_stream;
    sns_com_port_config* com_config = &sensor_state->common.com_port_info.com_config;
    uint8_t pb_encode_buffer[100];
    sns_request async_com_port_request =
    {
      .message_id  = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
      .request     = &pb_encode_buffer
    };

    state->ascp_config.bus_type          = (com_config->bus_type == SNS_BUS_I2C) ?
      SNS_ASYNC_COM_PORT_BUS_TYPE_I2C : SNS_ASYNC_COM_PORT_BUS_TYPE_SPI;
    state->ascp_config.slave_control     = com_config->slave_control;
    state->ascp_config.reg_addr_type     = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
    state->ascp_config.min_bus_speed_kHz = com_config->min_bus_speed_KHz;
    state->ascp_config.max_bus_speed_kHz = com_config->max_bus_speed_KHz;
    state->ascp_config.bus_instance      = com_config->bus_instance;

    async_com_port_request.request_len =
      pb_encode_request(pb_encode_buffer,
                        sizeof(pb_encode_buffer),
                        &state->ascp_config,
                        sns_async_com_port_config_fields,
                        NULL);
    data_stream->api->send_request(data_stream, &async_com_port_request);
  }
  /** Initialize factory calibration */
  state->sensor_pressure_registry_cfg.fac_cal_scale[0] = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_scale[0] = 1.0;
#if(BMP_CONFIG_ENABLE_DIAG_LOG == 1)
  /** Determine size of sns_diag_sensor_state_raw as defined in
   *  sns_diag.proto
   *  sns_diag_sensor_state_raw is a repeated array of samples of
   *  type sns_diag_batch sample. The following determines the
   *  size of sns_diag_sensor_state_raw with a single batch
   *  sample */
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                               &batch_sample))
    {
      state->log_raw_encoded_size = stream.bytes_written;
    }
  }
#endif
#if BMP_CONFIG_ENABLE_DAE
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "data_acquisition_engine", &dae_suid);
  bmp285_dae_if_init(this, stream_mgr, &dae_suid);
#endif
  BMP_INST_LOG(LOW, this, "<sns_see_if__ init> success");
  return SNS_RC_SUCCESS;
}

sns_rc bmp285_inst_deinit(sns_sensor_instance *const this)
{
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  inst_cleanup(this, stream_mgr);

  return SNS_RC_SUCCESS;
}

/**
 * Sends a self-test completion event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID 
 * @param[i] result    pass/fail result 
 * @param[i] type      test type 
 * @param[i] err_code  driver specific error code 
 *
 * @return none
 */
static void bmp285_send_self_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result,
                                        sns_physical_sensor_test_type type,
                                        bmp285_test_err_code err_code)
{
  uint8_t data[1] = {(uint8_t)err_code};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = type;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

#if BMP_CONFIG_ENABLE_SELF_TEST_FAC
void bmp285_start_factory_test(sns_sensor_instance* instance, bmp285_sensor_type sensor)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;

  state->fac_test_info.num_samples = 0;
  state->fac_test_info.sample_square_sum[0] = 0;
  state->fac_test_info.sample_square_sum[1] = 0;
  state->fac_test_info.sample_square_sum[2] = 0;
  state->fac_test_info.sample_sum[0] = 0;
  state->fac_test_info.sample_sum[1] = 0;
  state->fac_test_info.sample_sum[2] = 0;
  state->fac_test_in_progress = true;
  state->fac_test_sensor = sensor;
  state->fac_test_info.at_rest = true;

  if(sensor == BMP285_PRESSURE)
  {
    /*
      state->fac_test_info.bias_thresholds[0] = 180 * G / 1000; // 180 m/s2
      state->fac_test_info.bias_thresholds[1] = 180 * G / 1000; // 180 m/s2
      state->fac_test_info.bias_thresholds[2] = 210 * G / 1000; // 210 m/s2
      state->fac_test_info.variance_threshold = 383.0; // 383(m/s2)2
      */
    state->pressure_info.sampling_rate_hz = BME_ODR_5;
    state->pressure_info.sampling_intvl = 
      sns_convert_ns_to_ticks(1000000000.0 / state->pressure_info.sampling_rate_hz);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, instance, "Unknown sensor %d", sensor);
  }
  BMP_INST_LOG(HIGH, instance, "bmp285_start_factory_test() sensor %d", sensor);
  bmp285_reconfig_hw(instance, sensor);
}
#endif

void bmp285_run_self_test(sns_sensor_instance *instance)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;
  BMP_INST_LOG(LOW, instance, "bmp285_run_self_test");
  rv = bmp285_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);
  if(rv == SNS_RC_SUCCESS
     &&
     buffer == BMP285_WHOAMI_VALUE)
  {
    who_am_i_success = true;
  }
  BMP_INST_LOG(HIGH, instance, "bmp285_run_self_test who am i = %d", who_am_i_success);

  if(state->pressure_info.test_info.test_client_present)
  {
    BMP_INST_LOG(HIGH, instance, "test_client_present  test_type = %d", state->pressure_info.test_info.test_type);
    if(state->pressure_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmp285_send_self_test_event(instance, &state->pressure_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMP285_TEST_NO_ERROR);
    }
    else if(state->pressure_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      #if BMP_CONFIG_ENABLE_SELF_TEST_FAC
      bmp285_start_factory_test(instance, BMP285_PRESSURE);
      #endif
    }
    state->pressure_info.test_info.test_client_present = false;
  }
  if(state->temperature_info.test_info.test_client_present)
  {
    if(state->temperature_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmp285_send_self_test_event(instance, &state->temperature_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMP285_TEST_NO_ERROR);
    }
    else if(state->temperature_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->temperature_info.test_info.test_client_present = false;
  }
}

sns_rc bmp285_inst_set_client_config(
    sns_sensor_instance * const this,
    sns_request const *client_request)
{
  bmp285_instance_state *state = (bmp285_instance_state*) this->state->state;
  state->client_req_id = client_request->message_id;
  float desired_sample_rate = 0;
  float desired_report_rate = 0;
  bmp285_sensor_type sensor_type = BMP_SENSOR_INVALID;
  bmp285_power_mode op_mode = INVALID_WORK_MODE;
  sns_bmp285_cfg_req *payload = (sns_bmp285_cfg_req*)client_request->request;
#if BMP_CONFIG_ENABLE_REGISTRY
  float *fac_cal_bias = NULL;
  float *fac_cal_scale = NULL;
#endif
  sns_rc rv = SNS_RC_SUCCESS;

  BMP_INST_LOG(LOW, this, "<sns_see_if__  set_client_config>");

  /* Turn COM port ON, *physical* */
  state->scp_service->api->sns_scp_update_bus_power(
      state->com_port_info.port_handle, true);

  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.

    desired_sample_rate = payload->sample_rate;
    desired_report_rate = payload->report_rate;
    state->sample_rate_no_need_change = payload->sample_rate_no_not_change;
    
    BMP_INST_LOG(LOW, this, "state->sample_rate_no_need_change %d",
      state->sample_rate_no_need_change);
    sensor_type = payload->sensor_type;
    op_mode = payload->op_mode;

    if(desired_report_rate > desired_sample_rate)
    {
    /* bad request. Return error or default report_rate to sample_rate */
      desired_report_rate = desired_sample_rate;
    }
    if(sensor_type == BMP285_TEMPERATURE)
    {
      rv = bmp285_validate_sensor_temp_odr(this);
      if(rv != SNS_RC_SUCCESS
         && desired_sample_rate != 0)
      {
        // TODO Unsupported rate. Report error using sns_std_error_event.
        SNS_INST_PRINTF(ERROR, this, "sensor_temp ODR match error %d", rv);
        //return rv;
      }
    }
    if(sensor_type == BMP285_PRESSURE)
    {
      rv = bmp285_validate_sensor_pressure_odr(this);
      if(rv != SNS_RC_SUCCESS
         && desired_sample_rate != 0)
      {
        // TODO Unsupported rate. Report error using sns_std_error_event.
        SNS_INST_PRINTF(ERROR, this, "sensor_pressure ODR match error %d", rv);
        //return rv;
      }
    }
    #if BMP_CONFIG_ENABLE_DAE
    if(BMP285_CONFIG_IDLE == state->config_step && bmp285_dae_if_stop_streaming(this))
    {
      state->config_step = BMP285_CONFIG_STOPPING_STREAM;
    }
    #endif
    if(state->config_step == BMP285_CONFIG_IDLE)
    {
      bmp285_reconfig_hw(this, sensor_type);
    }

    bmp285_send_config_event(this);
  #if BMP_CONFIG_ENABLE_REGISTRY
    // update registry configuration
    if(BMP285_PRESSURE == payload->registry_cfg.sensor_type
       && payload->registry_cfg.version >= state->sensor_pressure_registry_cfg.version)
    {
      fac_cal_bias = state->sensor_pressure_registry_cfg.fac_cal_bias;
      fac_cal_scale = state->sensor_pressure_registry_cfg.fac_cal_scale;
      state->sensor_pressure_registry_cfg.version = payload->registry_cfg.version;
    }
    else if(BMP285_TEMPERATURE== payload->registry_cfg.sensor_type
            && payload->registry_cfg.version >= state->sensor_temp_registry_cfg.version)
    {
      fac_cal_bias = state->sensor_temp_registry_cfg.fac_cal_bias;
      fac_cal_scale = state->sensor_temp_registry_cfg.fac_cal_scale;
      state->sensor_temp_registry_cfg.version = payload->registry_cfg.version;
    }
    if(NULL!= fac_cal_bias && NULL != fac_cal_scale)
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));

      sns_memscpy(fac_cal_scale, sizeof(payload->registry_cfg.fac_cal_scale),
                  &payload->registry_cfg.fac_cal_scale,
                  sizeof(payload->registry_cfg.fac_cal_scale));
    }
    BMP_INST_LOG(LOW, this, "pressure fac_cal_bias[0]:%u temp fac_cal_bias[0]:%u",
    (uint32_t)state->sensor_pressure_registry_cfg.fac_cal_bias[0],
    (uint32_t)state->sensor_temp_registry_cfg.fac_cal_bias[0]);
    BMP_INST_LOG(LOW, this, "pressure fac_cal_cal[0]:%u temp fac_cal_cal[0]:%u",
    (uint32_t)state->sensor_pressure_registry_cfg.fac_cal_scale[0],
    (uint32_t)state->sensor_temp_registry_cfg.fac_cal_scale[0]);
    BMP_INST_LOG(LOW, this, "pressure version:%u temp version:%u",
    (uint32_t)state->sensor_pressure_registry_cfg.version,
    (uint32_t)state->sensor_temp_registry_cfg.version);
  #endif
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    /** All self-tests can be handled in normal mode. */
    #if BMP_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
    #endif
    bmp285_run_self_test(this);
    state->new_self_test_request = false;
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);
  BMP_INST_LOG(LOW, this, "<sns_see_if__  set_client_config> exit");
  return SNS_RC_SUCCESS;
}
