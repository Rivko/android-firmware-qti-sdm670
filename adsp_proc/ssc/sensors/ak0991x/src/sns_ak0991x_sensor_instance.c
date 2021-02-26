/**
 * @file sns_ak0991x_sensor_instance.c
 *
 * AK0991X Mag virtual Sensor Instance implementation.
 *
 * Copyright (c) 2016-2018 Asahi Kasei Microdevices
 * All Rights Reserved.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 **/

#include "sns_mem_util.h"
#include "sns_sensor_instance.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_types.h"

#include "sns_ak0991x_hal.h"
#include "sns_ak0991x_sensor.h"
#include "sns_ak0991x_sensor_instance.h"
#include "sns_ak0991x_s4s.h"


#include "sns_interrupt.pb.h"
#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#ifdef AK0991X_ENABLE_DIAG_LOGGING
#include "sns_diag_service.h"
#include "sns_diag.pb.h"
#endif
#include "sns_sync_com_port_service.h"
#include "sns_sensor_util.h"

/** See sns_sensor_instance_api::init */
sns_rc ak0991x_inst_init(sns_sensor_instance *const this,
                                sns_sensor_state const *sstate)
{
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;
  ak0991x_state *sensor_state =
    (ak0991x_state *)sstate->state;
  float               data[3];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service  *stream_mgr = (sns_stream_service *)
    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
#ifdef AK0991X_ENABLE_DIAG_LOGGING
  uint64_t buffer[10];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[AK0991X_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = AK0991X_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
#endif
#ifdef AK0991X_ENABLE_DUAL_SENSOR
  sns_sensor_uid mag_suid = (sensor_state->registration_idx == 0)? (sns_sensor_uid)MAG_SUID1 :
                                                              (sns_sensor_uid)MAG_SUID2;
  AK0991X_INST_PRINT(LOW, this, "hardware_id=%d registration_idx=%d", sensor_state->hardware_id, sensor_state->registration_idx);
#else
  sns_sensor_uid mag_suid = (sns_sensor_uid)MAG_SUID1;
#endif

  state->diag_service = (sns_diag_service *)
    service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);
  state->scp_service = (sns_sync_com_port_service *)
    service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  /**----------------Copy Sensor UID in instance state---------------*/
  sns_memscpy(&state->mag_info.suid,
              sizeof(state->mag_info.suid),
              &mag_suid,
              sizeof(state->mag_info.suid));

  SNS_INST_PRINTF(HIGH, this, "ak0991x inst init" );

  /**-------------------------Init Mag State-------------------------*/
  state->mag_info.desired_odr = AK0991X_MAG_ODR_OFF;
  state->mag_info.curr_odr = AK0991X_MAG_ODR_OFF;
  sns_memscpy(&state->mag_info.sstvt_adj,
              sizeof(state->mag_info.sstvt_adj),
              &sensor_state->sstvt_adj,
              sizeof(sensor_state->sstvt_adj));
  sns_memscpy(&state->mag_info.device_select,
              sizeof(state->mag_info.device_select),
              &sensor_state->device_select,
              sizeof(sensor_state->device_select));
  state->mag_info.cur_wmk = 0;
  // Init for s4s
  ak0991x_s4s_inst_init(this, sstate);

  switch (state->mag_info.device_select)
  {
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09911)
  case AK09911:
    state->mag_info.resolution = AK09911_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09911_FIFO_SIZE;
    state->mag_info.use_dri = false;
    state->mag_info.nsf = 0;
    state->mag_info.sdr = 0;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09912)
  case AK09912:
    state->mag_info.resolution = AK09912_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09912_FIFO_SIZE;
    state->mag_info.use_dri = sensor_state->is_dri;
    state->mag_info.nsf = sensor_state->nsf;
    state->mag_info.sdr = 0;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09913)
  case AK09913:
    state->mag_info.resolution = AK09913_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09913_FIFO_SIZE;
    state->mag_info.use_dri = false;
    state->mag_info.nsf = 0;
    state->mag_info.sdr = 0;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09915C) || defined(AK0991X_TARGET_AK09915D)
  case AK09915C:
  case AK09915D:
    state->mag_info.resolution = AK09915_RESOLUTION;
    state->mag_info.use_fifo = sensor_state->use_fifo;
    state->mag_info.max_fifo_size = AK09915_FIFO_SIZE;
    state->mag_info.use_dri = sensor_state->is_dri;
    state->mag_info.nsf = sensor_state->nsf;
    state->mag_info.sdr = sensor_state->sdr;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09916C)
  case AK09916C:
    state->mag_info.resolution = AK09916_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09916_FIFO_SIZE;
    state->mag_info.use_dri = false;
    state->mag_info.nsf = 0;
    state->mag_info.sdr = 0;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09916D)
  case AK09916D:
    state->mag_info.resolution = AK09916_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09916_FIFO_SIZE;
    state->mag_info.use_dri = sensor_state->is_dri;
    state->mag_info.nsf = 0;
    state->mag_info.sdr = 0;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09917)
  case AK09917:
    state->mag_info.resolution = AK09917_RESOLUTION;
    state->mag_info.use_fifo = sensor_state->use_fifo;
    state->mag_info.max_fifo_size = AK09917_FIFO_SIZE;
    state->mag_info.use_dri = sensor_state->is_dri;
    state->mag_info.nsf = sensor_state->nsf;
    state->mag_info.sdr = sensor_state->sdr;
    break;
#endif
#if defined(AK0991X_ENABLE_ALL_DEVICES) || defined(AK0991X_TARGET_AK09918)
  case AK09918:
    state->mag_info.resolution = AK09918_RESOLUTION;
    state->mag_info.use_fifo = false;
    state->mag_info.max_fifo_size = AK09918_FIFO_SIZE;
    state->mag_info.use_dri = false;
    state->mag_info.nsf = 0;
    state->mag_info.sdr = 0;
    break;
#endif
  default:
    return SNS_RC_FAILED;
  }

  state->pre_timestamp = sns_get_system_time();
  state->this_is_first_data = true;
  state->mag_info.data_count = 0;
  state->heart_beat_attempt_count = 0;
  state->flush_sample_count = 0;
  state->in_clock_error_procedure = false;

  state->mag_info.clock_error_meas_count = 0;
  state->internal_clock_error = 0x01 << AK0991X_CALC_BIT_RESOLUTION;

  state->encoded_mag_event_len = pb_get_encoded_size_sensor_stream_event(data, AK0991X_NUM_AXES);

#ifdef AK0991X_ENABLE_DRI
  sns_sensor_uid irq_suid;
  sns_sensor_uid acp_suid;
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "interrupt", &irq_suid);
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "async_com_port", &acp_suid);

  sns_rc rv;
  rv = stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                      this,
                                                      irq_suid,
                                                      &state->interrupt_data_stream);

  if (rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  rv = stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                      this,
                                                      acp_suid,
                                                      &state->async_com_port_data_stream);

  if (rv != SNS_RC_SUCCESS)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->interrupt_data_stream);
    return rv;
  }
#endif //AK0991X_ENABLE_DRI
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
  sns_sensor_uid device_mode_suid;
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "device_mode", &device_mode_suid);
  rv = stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                      this, 
                                                      device_mode_suid,
                                                      &state->device_mode_stream);
  if(rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  uint8_t i;
  for(i = 0; i < MAX_DEVICE_MODE_SUPPORTED; i++)
  {
    sns_memscpy(&state->cal_parameter[i],
                sizeof(state->cal_parameter[i]),
                &sensor_state->cal_parameter[i],
                sizeof(sensor_state->cal_parameter[i]));
    AK0991X_INST_PRINT(LOW, this, "| %4d %4d %4d |",
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e00*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e01*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e02*100));
     AK0991X_INST_PRINT(LOW, this, "| %4d %4d %4d |",
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e10*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e11*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e12*100));
     AK0991X_INST_PRINT(LOW, this, "| %4d %4d %4d |",
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e20*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e21*100),
       (int)(state->cal_parameter[i].fac_cal_corr_mat.e22*100));
      AK0991X_INST_PRINT(LOW, this, "Fac Cal bias %d %d %d",
       (int)(state->cal_parameter[i].fac_cal_bias[0]),
       (int)(state->cal_parameter[i].fac_cal_bias[1]),
       (int)(state->cal_parameter[i].fac_cal_bias[2]));
 }
#endif

  /** Initialize Timer info to be used by the Instance */
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "timer", &state->timer_suid);
  state->timer_data_stream = NULL;

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->com_port_info.com_config,
              sizeof(sensor_state->com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                                     &state->com_port_info.port_handle );

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  state->scp_service->api->sns_scp_update_bus_power(
    state->com_port_info.port_handle,
    false);

#ifdef AK0991X_ENABLE_DRI
  /** Initialize IRQ info to be used by the Instance */
  sns_memscpy(&state->irq_info,
              sizeof(state->irq_info),
              &sensor_state->irq_config,
              sizeof(sensor_state->irq_config));

  state->irq_info.is_registered = false;
  state->irq_info.detect_irq_event = false;
  state->irq_info.is_ready = false;

  /** Configure the Async Com Port */
  uint8_t                   pb_encode_buffer[100];
  uint32_t                  enc_len;

  state->ascp_config.bus_instance = sensor_state->com_port_info.com_config.bus_instance;
  if (sensor_state->com_port_info.com_config.bus_type == SNS_BUS_I2C)
  {
    state->ascp_config.bus_type = SNS_ASYNC_COM_PORT_BUS_TYPE_I2C;
  }
  else
  {
    state->ascp_config.bus_type = SNS_ASYNC_COM_PORT_BUS_TYPE_SPI;
  }
  state->ascp_config.max_bus_speed_kHz =
    sensor_state->com_port_info.com_config.max_bus_speed_KHz;
  state->ascp_config.min_bus_speed_kHz =
    sensor_state->com_port_info.com_config.min_bus_speed_KHz;
  state->ascp_config.reg_addr_type = SNS_ASYNC_COM_PORT_REG_ADDR_TYPE_8_BIT;
  state->ascp_config.slave_control = sensor_state->com_port_info.com_config.slave_control;
  enc_len = pb_encode_request(pb_encode_buffer, 100, &state->ascp_config,
                              sns_async_com_port_config_fields, NULL);

  sns_request async_com_port_request =
    (sns_request)
  {
    .message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG,
    .request_len = enc_len,
    .request = &pb_encode_buffer
  };
  state->async_com_port_data_stream->api->send_request(
    state->async_com_port_data_stream, &async_com_port_request);
#endif //AK0991X_ENABLE_DRI

 /** Copy down axis conversion settings */
  sns_memscpy(state->axis_map,  sizeof(sensor_state->axis_map),
              sensor_state->axis_map, sizeof(sensor_state->axis_map));

  /** Initialize factory calibration */
  state->mag_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->mag_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->mag_registry_cfg.fac_cal_corr_mat.e22 = 1.0;

#ifdef AK0991X_ENABLE_DIAG_LOGGING
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

  sns_sensor_uid dae_suid;
  sns_suid_lookup_get(&sensor_state->suid_lookup_data, "data_acquisition_engine", &dae_suid);
  ak0991x_dae_if_init(this, stream_mgr, &dae_suid, &mag_suid);

return SNS_RC_SUCCESS;
}

sns_rc ak0991x_inst_deinit(sns_sensor_instance *const this)
{
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;

  SNS_INST_PRINTF(HIGH, this, "deinit:: #samples=%u", state->total_samples);

  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
    ak0991x_reconfig_hw(this);
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  }

  ak0991x_dae_if_deinit(this);

  sns_sensor_util_remove_sensor_instance_stream(this,&state->timer_data_stream);
  //Deinit for S4S
  ak0991x_s4s_inst_deinit(this);
#ifdef AK0991X_ENABLE_DRI
  sns_sensor_util_remove_sensor_instance_stream(this,&state->interrupt_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this,&state->async_com_port_data_stream);
#endif //AK0991X_ENABLE_DRI
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
  sns_sensor_util_remove_sensor_instance_stream(this, &state->device_mode_stream);
#endif
  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
  }
  return SNS_RC_SUCCESS;
}

static uint16_t ak0991x_set_wmk(sns_sensor_instance *const this,
                                float desired_report_rate)
{
  uint16_t desired_wmk = 0;
#ifdef AK0991X_ENABLE_FIFO
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;

  if (state->mag_info.use_fifo)
  {
    if (desired_report_rate != 0)
    {
      /* Water mark level : 0x00 -> 1step, 0x01F ->32step*/
      desired_wmk = state->mag_info.req_wmk - 1;
    }

    switch (state->mag_info.device_select)
    {
      case AK09915C:
      case AK09915D:
        if (AK09915_FIFO_SIZE <= desired_wmk)
        {
          desired_wmk = AK09915_FIFO_SIZE - 1;
        }
        break;

      case AK09917:
        if (AK09917_FIFO_SIZE <= desired_wmk)
        {
          desired_wmk = AK09917_FIFO_SIZE - 1;
        }
        break;

      default:
        desired_wmk = 0;
        break;
    }
  }
#else
  UNUSED_VAR(this);
  UNUSED_VAR(desired_report_rate);
#endif // AK0991X_ENABLE_FIFO
  return desired_wmk;
}

static void ak0991x_care_fifo_buffer(sns_sensor_instance *const this,
                                     float mag_chosen_sample_rate,
                                     ak0991x_mag_odr mag_chosen_sample_rate_reg_value)
{
#ifdef AK0991X_ENABLE_FIFO
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;

    state->this_is_the_last_flush = true;
    AK0991X_INST_PRINT(LOW, this, "last flush before changing ODR");
    ak0991x_read_mag_samples(this);
    state->this_is_the_last_flush = false;

  // stop timer
  if (state->timer_data_stream != NULL)
  {
    state->mag_req.sample_rate = AK0991X_ODR_0;
    state->mag_info.desired_odr = AK0991X_MAG_ODR_OFF;
    ak0991x_reconfig_hw(this);
    ak0991x_register_timer(this);
    AK0991X_INST_PRINT(LOW, this, "stop register_timer");

    // reset
    state->mag_req.sample_rate = mag_chosen_sample_rate;
    state->mag_info.desired_odr = mag_chosen_sample_rate_reg_value;
  }
#else
  UNUSED_VAR(this);
  UNUSED_VAR(mag_chosen_sample_rate);
  UNUSED_VAR(mag_chosen_sample_rate_reg_value);
#endif // AK0991X_ENABLE_FIFO
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc ak0991x_inst_set_client_config(sns_sensor_instance *const this,
                                      sns_request const *client_request)
{
  ak0991x_instance_state *state =
    (ak0991x_instance_state *)this->state->state;
  state->client_req_id = client_request->message_id;
  float           desired_sample_rate = 0;
  float           desired_report_rate = 0;
  float           mag_chosen_sample_rate = 0;
  ak0991x_mag_odr mag_chosen_sample_rate_reg_value;
  uint16_t        desired_wmk;
  sns_rc          rv = SNS_RC_SUCCESS;
  float *fac_cal_bias = NULL;
  matrix3 *fac_cal_corr_mat = NULL;

  AK0991X_INST_PRINT(MED, this, "inst_set_client_config msg_id %d", client_request->message_id);
  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(
    state->com_port_info.port_handle,
    true);

  AK0991X_INST_PRINT(LOW, this, "dae_if_available %d",(int)ak0991x_dae_if_available(this));

  if (client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    // In case of DRI,
    //   1. Extract sample, report rates from client_request.
    //   2. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    //   3. Configure sensor HW.
    //   4. Save the current config information like type, sample_rate, report_rate, etc.
    // In case of polling,
    //   1. Extract sample, report rates from client_request.
    //   2. Configure sensor HW.
    //   3. Save the current config information like type, sample_rate, report_rate, etc.
    //   4. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    sns_ak0991x_mag_req *payload =
      (sns_ak0991x_mag_req *)client_request->request;
    desired_sample_rate = payload->sample_rate;
    desired_report_rate = payload->report_rate;
    state->mag_info.flush_only = payload->is_flush_only;

    // If already streaming, send out existing config before the next data events are sent
    if (!state->this_is_first_data && desired_sample_rate > 0)
    {
      ak0991x_send_config_event(this);
    }
#ifdef AK0991X_ENABLE_DEVICE_MODE_SENSOR
    if(NULL != state->device_mode_stream )
    {
      sns_request request = (sns_request){
       .message_id = SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG,
       .request_len = 0, .request = NULL };
      state->device_mode_stream->api->send_request(state->device_mode_stream, &request);
    }

#endif

    // Register for interrupt
    if(state->mag_info.use_dri && !ak0991x_dae_if_available(this))
    {
      ak0991x_register_interrupt(this);
    }

    if (desired_report_rate > desired_sample_rate)
    {
      // bad request. Return error or default report_rate to sample_rate
      desired_report_rate = desired_sample_rate;
    }

    AK0991X_INST_PRINT(LOW, this, "desired_sample_rate=%d desired_report_rate=%d",
        (int)desired_sample_rate, (int)desired_report_rate);

    state->mag_info.flush_period = payload->flush_period;
    rv = ak0991x_mag_match_odr(desired_sample_rate,
                               &mag_chosen_sample_rate,
                               &mag_chosen_sample_rate_reg_value,
                               state->mag_info.device_select);
    if (rv != SNS_RC_SUCCESS)
    {
      // TODO Unsupported rate. Report error using sns_std_error_event.
      // return rv;
    }

    if (desired_report_rate != 0)
    {
      // possibly larger than max HW FIFO for DAE
      state->mag_info.req_wmk = (uint16_t)(mag_chosen_sample_rate / desired_report_rate);
    }
    else
    {
      state->mag_info.req_wmk = 0;
    }

    desired_wmk = ak0991x_set_wmk(this, desired_report_rate);
    AK0991X_INST_PRINT(LOW, this, "desired_wmk=%d",desired_wmk);

    if( state->mag_info.cur_wmk == desired_wmk &&
        state->mag_info.curr_odr == mag_chosen_sample_rate_reg_value )
    {
      // No change needed -- return success
      AK0991X_INST_PRINT(LOW, this, "Config not changed.");
      ak0991x_send_config_event(this);
      // Turn COM port OFF
      state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

      return SNS_RC_SUCCESS;
    }

    state->mag_info.cur_wmk = desired_wmk;
    state->mag_req.sample_rate = mag_chosen_sample_rate;
    state->mag_info.desired_odr = mag_chosen_sample_rate_reg_value;

    AK0991X_INST_PRINT(LOW, this, "sample_rate=%d, reg_value=%d, config_step=%d",
                       (int)mag_chosen_sample_rate,
                       (int)mag_chosen_sample_rate_reg_value,
                       (int)state->config_step);

#ifdef AK0991X_ENABLE_DRI
    if(state->in_clock_error_procedure)
    {
      // Save request, but not set HW config -- return success
      AK0991X_INST_PRINT(LOW, this, "100Hz dummy measurement is still running. save request.");
      ak0991x_send_config_event(this);
      // Turn COM port OFF
      state->scp_service->api->sns_scp_update_bus_power(
                                                        state->com_port_info.port_handle,
                                                        false);

      return SNS_RC_SUCCESS;
    }
#endif

    if (AK0991X_CONFIG_IDLE == state->config_step &&
        ak0991x_dae_if_stop_streaming(this))
    {
      AK0991X_INST_PRINT(LOW, this, "done dae_if_stop_streaming");
      state->config_step = AK0991X_CONFIG_STOPPING_STREAM;
    }

    if (state->config_step == AK0991X_CONFIG_IDLE)
    {
      state->system_time = sns_get_system_time();

      // care the FIFO buffer if enabled FIFO and already streaming
      if ( (!state->this_is_first_data) && (state->mag_info.use_fifo) )
      {
        ak0991x_care_fifo_buffer(this,
                                 mag_chosen_sample_rate,
                                 mag_chosen_sample_rate_reg_value);
      }

      // Reset device
      rv = ak0991x_device_sw_reset(this,
                                   state->scp_service,
                                   state->com_port_info.port_handle);
      if(rv == SNS_RC_SUCCESS){
        AK0991X_INST_PRINT(LOW, this, "soft reset called.");
      }else{
        SNS_INST_PRINTF(ERROR, this, "soft reset failed.");
      }

      // hardware setting for measurement mode
      if (state->mag_info.use_dri && !ak0991x_dae_if_start_streaming(this))
      {
        ak0991x_reconfig_hw(this);
      }

      // Register for timer
      if (!state->mag_info.use_dri && !ak0991x_dae_if_available(this))
      {
        ak0991x_reconfig_hw(this);
        ak0991x_register_timer(this);
        // Register for s4s timer
        if (state->mag_info.use_sync_stream)
        {
          ak0991x_s4s_register_timer(this);
        }
      }

      if (state->mag_info.desired_odr != AK0991X_MAG_ODR_OFF)
      {
        ak0991x_send_config_event(this);
      }
    }

    // update registry configuration
    if(payload->registry_cfg.version >= state->mag_registry_cfg.version)
    {
      fac_cal_bias = state->mag_registry_cfg.fac_cal_bias;
      fac_cal_corr_mat = &state->mag_registry_cfg.fac_cal_corr_mat;
      state->mag_registry_cfg.version = payload->registry_cfg.version;
    }

    if(NULL != fac_cal_bias && NULL != fac_cal_corr_mat)
    {
      sns_memscpy(fac_cal_bias, sizeof(payload->registry_cfg.fac_cal_bias),
                  payload->registry_cfg.fac_cal_bias,
                  sizeof(payload->registry_cfg.fac_cal_bias));

      sns_memscpy(fac_cal_corr_mat, sizeof(payload->registry_cfg.fac_cal_corr_mat),
                  &payload->registry_cfg.fac_cal_corr_mat,
                  sizeof(payload->registry_cfg.fac_cal_corr_mat));
    }
  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {
    state->system_time = sns_get_system_time();
    state->fifo_flush_in_progress = true;
    AK0991X_INST_PRINT(LOW, this, "Flush requested at %u", (uint32_t)state->system_time);

    if(!ak0991x_dae_if_flush_samples(this))
    {
      if(state->mag_info.use_fifo)
      {
#ifdef AK0991X_ENABLE_DRI
        if (NULL != state->interrupt_data_stream)
        {
          sns_sensor_event *event =
            state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
          if(NULL == event || SNS_INTERRUPT_MSGID_SNS_INTERRUPT_EVENT != event->message_id)
          {
            ak0991x_read_mag_samples(this);
          }
        }
        else
        {
          ak0991x_read_mag_samples(this);
        }
#else
        ak0991x_read_mag_samples(this);
#endif
      }
      else
      {
        ak0991x_send_fifo_flush_done(this);
      }
    }
  }
  else if (state->client_req_id == SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    // If the sensor is streaming and there is a client_request to run self-test,
    // the existing stream can be temporarily paused.
    // When the self-test completes, the paused stream shall be restarted.
    if (state->mag_info.curr_odr != AK0991X_MAG_ODR_OFF)
    {
      AK0991X_INST_PRINT(LOW, this, "pause the stream for self-test.");
      mag_chosen_sample_rate = state->mag_req.sample_rate;
      mag_chosen_sample_rate_reg_value = state->mag_info.desired_odr;
      state->mag_req.sample_rate = 0;
      state->mag_info.desired_odr = AK0991X_MAG_ODR_OFF;

      if (AK0991X_CONFIG_IDLE == state->config_step &&
          ak0991x_dae_if_stop_streaming(this))
      {
        AK0991X_INST_PRINT(LOW, this, "done dae_if_stop_streaming");
        state->config_step = AK0991X_CONFIG_STOPPING_STREAM;
      }

      if (state->config_step == AK0991X_CONFIG_IDLE)
      {
        // care the FIFO buffer if enabled FIFO and already streaming
        if ((!state->this_is_first_data) && (state->mag_info.use_fifo))
        {
          ak0991x_care_fifo_buffer(this,
                                   mag_chosen_sample_rate,
                                   mag_chosen_sample_rate_reg_value);
        }

        // hardware setting for measurement mode
        if (state->mag_info.use_dri && !ak0991x_dae_if_start_streaming(this))
       	{
          ak0991x_reconfig_hw(this);
        }

        // Register for timer
        if (!state->mag_info.use_dri && !ak0991x_dae_if_available(this))
        {
          ak0991x_reconfig_hw(this);
          ak0991x_register_timer(this);
        }
        ak0991x_send_config_event(this);
      }

      // DAE handles PAUSE_SAMPLING request
      ak0991x_dae_if_process_events(this);

      // DAE handles FLUSH_HW request
      ak0991x_dae_if_process_events(this);

      AK0991X_INST_PRINT(LOW, this, "Execute the self-test.");
      ak0991x_run_self_test(this);
      state->new_self_test_request = false;

      AK0991X_INST_PRINT(LOW, this, "Resume the stream.");
      state->mag_req.sample_rate = mag_chosen_sample_rate;
      state->mag_info.desired_odr = mag_chosen_sample_rate_reg_value;

      // hardware setting for measurement mode
      if (state->mag_info.use_dri && !ak0991x_dae_if_available(this))
      {
        ak0991x_reconfig_hw(this);
      }

      // Register for timer
      if (!state->mag_info.use_dri && !ak0991x_dae_if_available(this))
      {
        ak0991x_reconfig_hw(this);
        ak0991x_register_timer(this);
      }

      ak0991x_send_config_event(this);

      // DAE handles SET_STREAMING_CONFIG request
      ak0991x_dae_if_process_events(this);
    }
    else
    {
      AK0991X_INST_PRINT(LOW, this, "SENSOR_TEST_CONFIG for selftest" );
      ak0991x_run_self_test(this);
      state->new_self_test_request = false;
    }
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(
    state->com_port_info.port_handle,
    false);

  return SNS_RC_SUCCESS;
}
