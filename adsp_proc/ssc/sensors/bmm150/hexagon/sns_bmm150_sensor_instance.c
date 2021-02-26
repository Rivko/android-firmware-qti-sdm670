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


#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_bmm150_hal.h"
#include "sns_bmm150_sensor.h"
#include "sns_bmm150_sensor_instance.h"

#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"
#include "sns_timer.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_cal.pb.h"
#include "sns_async_com_port_pb_utils.h"
#if(BMM_CONFIG_ENABLE_DIAG_LOG == 1)
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#endif
#include "sns_sensor_util.h"
#include "sns_sync_com_port_service.h"
#include "bmm150_defs.h"

void bmm150_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50] = {0};
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };
  sns_rc rc = SNS_RC_SUCCESS;

  BMM_INST_LOG(LOW, this, "bmm150_start_sensor_temp_polling_timer");

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
  req_payload.start_time = sns_get_system_time() - state->temperature_info.sampling_intvl;
  req_payload.has_start_config = true;
  req_payload.start_config.early_start_delta = 0;
  req_payload.start_config.late_start_delta = state->temperature_info.sampling_intvl;
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

void bmm150_start_sensor_pressure_polling_timer(sns_sensor_instance *this)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50] = {0};
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };
  sns_rc rc = SNS_RC_SUCCESS;
  BMM_INST_LOG(LOW, this, "bmm150_start_sensor_mag_polling_timer");
  state->old_timestamp = 0;

  if(NULL == state->mag_timer_data_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *srtm_svc = (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);

    rc = srtm_svc->api->create_sensor_instance_stream(srtm_svc,
          this, state->timer_suid, &state->mag_timer_data_stream);
  }

  if(SNS_RC_SUCCESS != rc
     || NULL == state->mag_timer_data_stream)
  {
    SNS_INST_PRINTF(ERROR, this, "failed timer stream create rc = %d", rc);
    return;
  }

  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->mag_info.sampling_intvl;
  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    state->mag_timer_data_stream->api->send_request(state->mag_timer_data_stream, &timer_req);
    state->mag_info.timer_is_active = true;
  }
  else
  {
    //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}
void bmm150_set_mag_polling_config(sns_sensor_instance *const this)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  int err = 0;
  BMM_INST_LOG(LOW, this,
  "mag_info.timer_is_active:%d state->mag_info.sampling_intvl:%u",
  state->mag_info.timer_is_active,
  state->mag_info.sampling_intvl);
  BMM_INST_LOG(LOW, this, "power_state = %d", state->settings.pwr_cntrl_bit);
  BMM_INST_LOG(LOW, this, "state->sample_rate_no_need_change = %d",
  state->sample_rate_no_need_change);
  if(state->mag_info.sampling_intvl > 0)
  {
    if((state->sample_rate_no_need_change != 1))
    {
      state->sample_cnt_to_skip_odrc = 3;
      state->settings.pwr_mode = BMM150_FORCED_MODE;
      err = bmm150_set_op_mode(state);
      BME_BUSY_DELAY(3);
      state->settings.preset_mode = BMM150_PRESETMODE_REGULAR;
      err = bmm150_set_presetmode(state);
      if(err)
      {
        SNS_INST_PRINTF(ERROR, this,"config power mode failed err = %d", err);
      }

      bmm150_start_sensor_pressure_polling_timer(this);
    }
  }
  else if(state->mag_info.timer_is_active)
  {
    state->mag_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this,
                                                  &state->mag_timer_data_stream);
    state->old_timestamp = 0;
  }
}

void bmm150_set_temperature_polling_config(sns_sensor_instance *const this)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  int err = 0;
  SNS_INST_PRINTF(LOW, this,
  "temperature_info.timer_is_active:%d state->temperature_info.sampling_intvl:%u",
  state->temperature_info.timer_is_active,
  state->temperature_info.sampling_intvl);

  if(state->temperature_info.sampling_intvl > 0)
  {
    //err = bmm150_set_power_mode(state,BMP280_FORCED_MODE);
    if(err)
    {
      SNS_INST_PRINTF(ERROR, this,"config power mode failed err = %d", err);
    }
    else
    {
      //bmm150_start_sensor_temp_polling_timer(this);
    }
  }
  else if(state->temperature_info.timer_is_active)
  {
    state->temperature_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this,
                                                  &state->temperature_timer_data_stream);

  }
}

void bmm150_reconfig_hw(sns_sensor_instance *this,
  bmm150_sensor_type sensor_type)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  BMM_INST_LOG(LOW, this, "bmm150_reconfig_hw state->config_step = %d",state->config_step);
  BMM_INST_LOG(LOW, this,
      "enable sensor flag:0x%x publish sensor flag:0x%x",
      state->deploy_info.enable,
      state->deploy_info.publish_sensors);
  if (sensor_type == BMM150_MAG)
  //&&
      //!bmm150_dae_if_available(this))
  {
    bmm150_set_mag_polling_config(this);
  }
  if (sensor_type == BMM150_TEMPERATURE)
  //&&
     // !bmm150_dae_if_available(this))
  {
    bmm150_set_temperature_polling_config(this);
  }
  //if(state->deploy_info.publish_sensors != 0)
  //{
 //   bmm150_dae_if_start_streaming(this);
 // }
  state->config_step = BMM150_CONFIG_IDLE;
  BMM_INST_LOG(LOW, this, "bmm150_reconfig_hw finished");
}

void bmm150_send_cal_event(sns_sensor_instance *const instance, bmm150_sensor_type sensor_type)
{
  bmm150_instance_state *state =
     (bmm150_instance_state*)instance->state->state;

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

  if(sensor_type == BMM150_MAG)
  {
    bias_data[0] = state->mag_registry_cfg.fac_cal_bias[0];
    bias_data[1] = state->mag_registry_cfg.fac_cal_bias[1];
    bias_data[2] = state->mag_registry_cfg.fac_cal_bias[2];
    comp_matrix_data[0] = state->mag_registry_cfg.fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = state->mag_registry_cfg.fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = state->mag_registry_cfg.fac_cal_corr_mat.data[2];
    comp_matrix_data[3] = state->mag_registry_cfg.fac_cal_corr_mat.data[3];
    comp_matrix_data[4] = state->mag_registry_cfg.fac_cal_corr_mat.data[4];
    comp_matrix_data[5] = state->mag_registry_cfg.fac_cal_corr_mat.data[5];
    comp_matrix_data[6] = state->mag_registry_cfg.fac_cal_corr_mat.data[6];
    comp_matrix_data[7] = state->mag_registry_cfg.fac_cal_corr_mat.data[7];
    comp_matrix_data[8] = state->mag_registry_cfg.fac_cal_corr_mat.data[8];
  }
 
  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
  pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
      { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(sensor_type == BMM150_MAG)
  {
    suid_current = &state->mag_info.suid;
  }
  else
  {
    suid_current = &state->temperature_info.suid;
  }

  new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  SNS_INST_PRINTF(ERROR, instance, "Calibration event");
  pb_send_event(instance,
                sns_cal_event_fields,
                &new_calibration_event,
                sns_get_system_time(),
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                suid_current);

}

void bmm150_send_config_event(sns_sensor_instance *const instance)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  sns_std_sensor_physical_config_event phy_sensor_config =
  sns_std_sensor_physical_config_event_init_default;
  BMM_INST_LOG(LOW, instance, "state->deploy_info.publish_sensors %d", state->deploy_info.publish_sensors);

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
  if (state->deploy_info.publish_sensors & BMM150_MAG)
  {
    phy_sensor_config.sample_rate = state->mag_info.sampling_rate_hz;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 170;
    phy_sensor_config.resolution = BMM150_SENSOR_MAG_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMM150_SENSOR_MAG_RANGE_MIN;
    phy_sensor_config.range[1] = BMM150_SENSOR_MAG_RANGE_MAX;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;
    pb_send_event(
        instance,
        sns_std_sensor_physical_config_event_fields,
        &phy_sensor_config,
        sns_get_system_time(),
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
        &state->mag_info.suid);
    bmm150_send_cal_event(instance, BMM150_MAG);
  }
  if(state->deploy_info.publish_sensors & BMM150_TEMPERATURE)
  {
    phy_sensor_config.sample_rate = state->temperature_info.sampling_rate_hz;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 325;
    phy_sensor_config.resolution = BMM150_SENSOR_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMM150_SENSOR_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = BMM150_SENSOR_TEMPERATURE_RANGE_MAX;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;
    pb_send_event(
        instance,
        sns_std_sensor_physical_config_event_fields,
        &phy_sensor_config,
        sns_get_system_time(),
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
        &state->temperature_info.suid);
    bmm150_send_cal_event(instance, BMM150_TEMPERATURE);
  }
}

static sns_rc bmm150_validate_sensor_mag_odr(sns_sensor_instance *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  BMM_INST_LOG(LOW, this, "mag odr = %d", (int8_t)state->mag_info.sampling_rate_hz);

  if(state->mag_info.sampling_rate_hz >= BME_ODR_1
          &&
          state->mag_info.sampling_rate_hz <= BME_ODR_10)
  {
    state->mag_info.sampling_rate_hz = BME_ODR_10;
  }
  else if(state->mag_info.sampling_rate_hz > BME_ODR_10
          &&
          state->mag_info.sampling_rate_hz <= BME_ODR_15)
  {
    state->mag_info.sampling_rate_hz = BME_ODR_15;
  }
  else if(state->mag_info.sampling_rate_hz > BME_ODR_15
    &&
    state->mag_info.sampling_rate_hz <= BME_ODR_50)
  {
    state->mag_info.sampling_rate_hz = BME_ODR_50;
  }
  else if(state->mag_info.sampling_rate_hz > BME_ODR_50)
  {
  #if defined (BMM_CONFIG_SUPPORT_100HZ)
    state->mag_info.sampling_rate_hz = BME_ODR_100;
  #else
    state->mag_info.sampling_rate_hz = BME_ODR_50;
  #endif
  }
  else
  {
    state->mag_info.sampling_intvl = 0;
    state->mag_info.timer_is_active = 0;
    BMM_INST_LOG(LOW, this, "close mag sensor = %d, timer_is_active =%d",
              (uint32_t)state->mag_info.sampling_rate_hz, state->mag_info.timer_is_active);
    rc = SNS_RC_SUCCESS;
  }
  BMM_INST_LOG(LOW, this, "mag setting odr = %d", (int8_t)state->mag_info.sampling_rate_hz);
  if (rc == SNS_RC_SUCCESS)
  {
    state->mag_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->mag_info.sampling_rate_hz);
    BMM_INST_LOG(LOW, this, "mag timer_value = %u", state->mag_info.sampling_intvl);
  }
  return rc;
}

static sns_rc bmm150_validate_sensor_temp_odr(sns_sensor_instance *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  BMM_INST_LOG(LOW, this, "temperature odr = %d", (int8_t)state->temperature_info.sampling_rate_hz);
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
    BMM_INST_LOG(LOW, this, "close temperature sensor = %d, timer_is_active =%d",
           (uint32_t)state->temperature_info.sampling_rate_hz, state->temperature_info.timer_is_active);
    rc = SNS_RC_SUCCESS;
  }
  if (rc == SNS_RC_SUCCESS)
  {
    state->temperature_info.sampling_intvl =
      sns_convert_ns_to_ticks(1000000000.0 / state->temperature_info.sampling_rate_hz);
    BMM_INST_LOG(LOW, this, "temperature timer_value = %u", state->temperature_info.sampling_intvl);
  }

  return rc;
}

static void inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr)
{
  bmm150_instance_state *state = (bmm150_instance_state*)this->state->state;
  #if BMM_CONFIG_ENABLE_DAE
  bmm150_dae_if_deinit(state, stream_mgr);
  #else
  UNUSED_VAR(stream_mgr);
  #endif
  sns_sensor_util_remove_sensor_instance_stream(this, &state->async_com_port_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->mag_timer_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this, &state->temperature_timer_data_stream);

  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
}
sns_rc bmm150_inst_init(sns_sensor_instance * const this,
    sns_sensor_state const *sstate)
{
  bmm150_instance_state *state =
              (bmm150_instance_state*) this->state->state;
  bmm150_state *sensor_state =
              (bmm150_state*) sstate->state;
  float stream_data[1] = {0};
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
#if(BMM_CONFIG_ENABLE_DIAG_LOG == 1)
  uint64_t buffer[10];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[BMM150_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = BMM150_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
#endif
  sns_sensor_uid ascp_suid;
#if BMM_CONFIG_ENABLE_DAE
  sns_sensor_uid dae_suid;
#endif
  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
  #if BMM_CONFIG_ENABLE_ISLAND_MODE
  state->island_service = (sns_island_service*)
              service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);
  #endif
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "async_com_port", &ascp_suid);
  BMM_INST_LOG(LOW, this, "<sns_see_if__  init> from sensor:0x%x", sensor_state->sensor);
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
  sns_memscpy(&state->mag_info.suid,
              sizeof(state->mag_info.suid),
              &((sns_sensor_uid)MAG_SUID),
              sizeof(state->mag_info.suid));
  sns_memscpy(&state->temperature_info.suid,
              sizeof(state->temperature_info.suid),
              &((sns_sensor_uid)TEMPERATURE_SUID),
              sizeof(state->temperature_info.suid));
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer", &state->timer_suid);


  /** Copy calibration data*/
  sns_memscpy(&state->trim_data,
              sizeof(state->trim_data),
              &sensor_state->common.trim_data,
              sizeof(sensor_state->common.trim_data));
  state->interface = (bmm150_intf)sensor_state->common.com_port_info.com_config.bus_instance;
  state->op_mode = FORCED_MODE;/*default working mode*/
  state->com_read = bmm150_com_read_wrapper;/* com read function*/
  state->com_write = bmm150_com_write_wrapper;/*com write function*/
  /* set the data report length to the framework */
  state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(stream_data, 3);
  state->encoded_temp_event_len = pb_get_encoded_size_sensor_stream_event(stream_data, 1);

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
  /* api data initialization */
  {
    if (state->ascp_config.bus_type == SNS_ASYNC_COM_PORT_BUS_TYPE_I2C)
    {
      state->interface = BMM150_I2C_INTF;
    }
    else
    {
      state->interface = BMM150_SPI_INTF;
    }
    state->delay_ms = bmm150_delay;
  }

  /** Copy down axis conversion settings */
  sns_memscpy(state->axis_map,  sizeof(sensor_state->common.axis_map),
              sensor_state->common.axis_map, sizeof(sensor_state->common.axis_map));

  /** Initialize factory calibration */
  state->mag_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->mag_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->mag_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
#if(BMM_CONFIG_ENABLE_DIAG_LOG == 1)
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
#if BMM_CONFIG_ENABLE_DAE
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "data_acquisition_engine", &dae_suid);
  bmm150_dae_if_init(this, stream_mgr, &dae_suid);
#endif
  BMM_INST_LOG(LOW, this, "<sns_see_if__ init> success");
  return SNS_RC_SUCCESS;
}

sns_rc bmm150_inst_deinit(sns_sensor_instance *const this)
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
static void bmm150_send_self_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result,
                                        sns_physical_sensor_test_type type,
                                        bmm150_test_err_code err_code)
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

#if BMM_CONFIG_ENABLE_SELF_TEST_FAC
void bmm150_start_factory_test(sns_sensor_instance* instance, bmm150_sensor_type sensor)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;

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

  if(sensor == BMM150_MAG)
  {
    /*
      state->fac_test_info.bias_thresholds[0] = 180 * G / 1000; // 180 m/s2
      state->fac_test_info.bias_thresholds[1] = 180 * G / 1000; // 180 m/s2
      state->fac_test_info.bias_thresholds[2] = 210 * G / 1000; // 210 m/s2
      state->fac_test_info.variance_threshold = 383.0; // 383(m/s2)2
      */
    state->mag_info.sampling_rate_hz = BME_ODR_5;
    state->mag_info.sampling_intvl = 
      sns_convert_ns_to_ticks(1000000000.0 / state->mag_info.sampling_rate_hz);
  }
  else
  {
    SNS_INST_PRINTF(ERROR, instance, "Unknown sensor %d", sensor);
  }
  BMM_INST_LOG(HIGH, instance, "bmm150_start_factory_test() sensor %d", sensor);
  bmm150_reconfig_hw(instance, sensor);
}
#endif

void bmm150_run_self_test(sns_sensor_instance *instance)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;
  BMM_INST_LOG(LOW, instance, "bmm150_run_self_test");
  rv = bmm150_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);
  if(rv == SNS_RC_SUCCESS
     &&
     buffer == BMM150_WHOAMI_VALUE)
  {
    who_am_i_success = true;
  }
  BMM_INST_LOG(HIGH, instance, "bmm150_run_self_test who am i = %d", who_am_i_success);

  if(state->mag_info.test_info.test_client_present)
  {
    BMM_INST_LOG(HIGH, instance, "test_client_present  test_type = %d", state->mag_info.test_info.test_type);
    if(state->mag_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmm150_send_self_test_event(instance, &state->mag_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMM150_TEST_NO_ERROR);
    }
    else if(state->mag_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      #if BMM_CONFIG_ENABLE_SELF_TEST_FAC
      bmm150_start_factory_test(instance, BMM150_MAG);
      #endif
    }
    state->mag_info.test_info.test_client_present = false;
  }
  if(state->temperature_info.test_info.test_client_present)
  {
    if(state->temperature_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmm150_send_self_test_event(instance, &state->temperature_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMM150_TEST_NO_ERROR);
    }
    else if(state->temperature_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->temperature_info.test_info.test_client_present = false;
  }
}

void bmm150_process_fac_test(sns_sensor_instance *instance)
{
  int i;
  bool test_pass = true;
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  sns_sensor_uid *uid = &state->mag_info.suid;
  sns_bmm150_registry_cfg *registry_cfg = &state->mag_registry_cfg;
  bmm150_test_err_code err_code = BMM150_TEST_NO_ERROR;

  BMM_INST_LOG(HIGH, instance, "Sensor %d", state->fac_test_sensor);



  if(state->fac_test_info.num_samples == 64)
  {
    BMM_INST_LOG(HIGH, instance, "Sample Sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_sum[0],
                    (int32_t)state->fac_test_info.sample_sum[1],
                    (int32_t)state->fac_test_info.sample_sum[2]);
    BMM_INST_LOG(HIGH, instance, "Sample square sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_square_sum[0],
                    (int32_t)state->fac_test_info.sample_square_sum[1],
                    (int32_t)state->fac_test_info.sample_square_sum[2]);

    state->fac_test_in_progress = false;


    state->fac_test_info.num_samples -= 3; // subtract discarded number of samples

    registry_cfg->fac_cal_bias[0] =
       state->fac_test_info.sample_sum[0] / state->fac_test_info.num_samples;
    registry_cfg->fac_cal_bias[1] =
       state->fac_test_info.sample_sum[1] / state->fac_test_info.num_samples;
    registry_cfg->fac_cal_bias[2] =
       state->fac_test_info.sample_sum[2] / state->fac_test_info.num_samples;
    registry_cfg->version ++;

    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      float varT = (state->fac_test_info.sample_sum[i]) * (state->fac_test_info.sample_sum[i]);

      state->fac_test_info.variance[i] = (state->fac_test_info.sample_square_sum[i]
                                          - (varT / state->fac_test_info.num_samples)) / state->fac_test_info.num_samples;

      // Check variance to determine whether device is stationary
      if(state->fac_test_info.variance[i] > state->fac_test_info.variance_threshold)
      {
        // device is not stationary
        state->fac_test_info.at_rest = false;
        test_pass = false;
        err_code = BMM150_FAC_TEST_DEV_NOT_STATIONARY;
        if(instance != NULL)
          SNS_INST_PRINTF(ERROR, instance, "FAILED device not stationary var[%u]=%u  %u",
                        i, (uint32_t)state->fac_test_info.variance[i],
                        (uint32_t)state->fac_test_info.variance_threshold);
        break;
      }

      // Check biases are in defined limits
      if(fabsf(registry_cfg->fac_cal_bias[i]) > state->fac_test_info.bias_thresholds[i])
      {
        test_pass = false;
        err_code = BMM150_FAC_TEST_HIGH_BIAS;
        if(instance != NULL)
          SNS_INST_PRINTF(ERROR, instance, "FAILED bias very large");
        break;
      }

      // Check for zero variance
      if(state->fac_test_info.variance[i] == 0.0)
      {
         test_pass = false;
         err_code = BMM150_FAC_TEST_ZERO_VARIANCE;
         if(instance != NULL)
          SNS_INST_PRINTF(ERROR, instance, "FAILED zero variance");
         break;
      }
    }

    /** update_fac_cal_in_registry if test is successful.*/
    state->update_fac_cal_in_registry = test_pass;
    bmm150_send_cal_event(instance, state->fac_test_sensor);
    bmm150_send_self_test_event(instance, uid,
                                 test_pass, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);
  }
}
sns_rc bmm150_inst_set_client_config(
    sns_sensor_instance * const this,
    sns_request const *client_request)
{
  bmm150_instance_state *state = (bmm150_instance_state*) this->state->state;
  state->client_req_id = client_request->message_id;
  float desired_sample_rate = 0;
  float desired_report_rate = 0;
  bmm150_sensor_type sensor_type = BMP_SENSOR_INVALID;
  sns_bmm150_cfg_req *payload = (sns_bmm150_cfg_req*)client_request->request;
#if BMM_CONFIG_ENABLE_REGISTRY
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;
#endif
  sns_rc rv = SNS_RC_SUCCESS;

  BMM_INST_LOG(LOW, this, "<sns_see_if__  set_client_config>");

  // Turn COM port ON
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
    
    BMM_INST_LOG(LOW, this, "state->sample_rate_no_need_change %d",
      state->sample_rate_no_need_change);
    sensor_type = payload->sensor_type;

    if(desired_report_rate > desired_sample_rate)
    {
    /* bad request. Return error or default report_rate to sample_rate */
      desired_report_rate = desired_sample_rate;
      SNS_INST_PRINTF(ERROR, this, "desired_report_rate %u", (uint32_t)desired_report_rate);
    }
    if (sensor_type == BMM150_TEMPERATURE)
    {
      rv = bmm150_validate_sensor_temp_odr(this);
      if(rv != SNS_RC_SUCCESS
         && desired_sample_rate != 0)
      {
        // TODO Unsupported rate. Report error using sns_std_error_event.
        SNS_INST_PRINTF(ERROR, this, "sensor_temp ODR match error %d", rv);
        //return rv;
      }
    }
    if (sensor_type == BMM150_MAG)
    {
      rv = bmm150_validate_sensor_mag_odr(this);
      if(rv != SNS_RC_SUCCESS
         && desired_sample_rate != 0)
      {
        // TODO Unsupported rate. Report error using sns_std_error_event.
        SNS_INST_PRINTF(ERROR, this, "sensor_pressure ODR match error %d", rv);
        //return rv;
      }
    }
    #if BMM_CONFIG_ENABLE_DAE
    if (BMM150_CONFIG_IDLE == state->config_step && bmm150_dae_if_stop_streaming(this))
    {
      state->config_step = BMM150_CONFIG_STOPPING_STREAM;
    }
    #endif
    if (state->config_step == BMM150_CONFIG_IDLE)
    {
      bmm150_reconfig_hw(this, sensor_type);
    }

    bmm150_send_config_event(this);
  #if BMM_CONFIG_ENABLE_REGISTRY
    // update registry configuration
    if(BMM150_MAG == payload->registry_cfg.sensor_type
       && payload->registry_cfg.version >= state->mag_registry_cfg.version)
    {
      fac_cal_bias = state->mag_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->mag_registry_cfg.fac_cal_corr_mat;
      state->mag_registry_cfg.version = payload->registry_cfg.version;
    }


    if(NULL!= fac_cal_bias && NULL != fac_cal_corr_mat)
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));

      sns_memscpy(fac_cal_corr_mat, sizeof(payload->registry_cfg.fac_cal_corr_mat),
                  &payload->registry_cfg.fac_cal_corr_mat,
                  sizeof(payload->registry_cfg.fac_cal_corr_mat));
    }
  #endif
  }

  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    /** All self-tests can be handled in normal mode. */
    #if BMM_CONFIG_ENABLE_ISLAND_MODE
    state->island_service->api->sensor_instance_island_exit(state->island_service, this);
    #endif
    bmm150_run_self_test(this);
    state->new_self_test_request = false;
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);
  BMM_INST_LOG(LOW, this, "<sns_see_if__  set_client_config> exit");
  return SNS_RC_SUCCESS;
}
