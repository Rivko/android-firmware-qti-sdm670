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
#include <stdint.h>
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_types.h"

#include "sns_bma2x2_hal.h"
#include "sns_bma2x2_sensor.h"
#include "sns_bma2x2_sensor_instance.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sync_com_port_service.h"

#if BMA2X2_CONFIG_ENABLE_SELF_TEST_FAC
#include "sns_cal.pb.h"
#endif

/**
 * clean up instance
 * @param this            instance handler
 * @param stream_mgr      stream manager
 */
static void inst_cleanup(sns_sensor_instance *this, sns_stream_service *stream_mgr)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *)this->state->state;
  // TODO: Is this OK to do here? Introduced by DAE sensor, since we could be
  // waiting for response from DAE before writing to HW -- but we don't have that chance.
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(
                                        state->com_port_info.port_handle,
                                        true);
  }
  bma2x2_hal_reconfig_hw(this);
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(
                                      state->com_port_info.port_handle,
                                      false);
  }
#if BMA2X2_CONFIG_ENABLE_DAE
  bma2x2_dae_if_deinit(state, stream_mgr);
#else
  UNUSED_VAR(stream_mgr);
#endif

  sns_sensor_util_remove_sensor_instance_stream(this,
                                            &state->interrupt_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this,
                                            &state->async_com_port_data_stream);
  sns_sensor_util_remove_sensor_instance_stream(this,
                                            &state->timer_data_stream);
#if !BMA2X2_ENABLE_FORCE_WAIT
  sns_sensor_util_remove_sensor_instance_stream(this,
                                            &state->accel_info.regv_cfg_manager.reg_config_timer);
#endif

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
  sns_sensor_util_remove_sensor_instance_stream(this,
                                            &state->timer_heart_beat_data_stream);
#endif

  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(
                                            &state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
}

/** See sns_sensor_instance_api::init */
sns_rc bma2x2_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate)
{
  bma2x2_instance_state *state =
              (bma2x2_instance_state*)this->state->state;
  bma2x2_state *sensor_state =
              (bma2x2_state*)sstate->state;
  float data[3];
  float temp_data[1];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  uint64_t buffer[10];
  pb_ostream_t stream =
                  pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[BMA2X2_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = BMA2X2_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
  sns_sensor_uid irq_suid, ascp_suid;

#if BMA2X2_CONFIG_ENABLE_DAE
  sns_sensor_uid dae_suid;
#endif

  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
  state->island_service = (sns_island_service*)
              service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);
#endif

  /**---------Setup stream connections with dependent Sensors---------*/
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data,
                      "interrupt", &irq_suid);
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data,
                      "async_com_port", &ascp_suid);
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 irq_suid,
                                                 &state->interrupt_data_stream);

  stream_mgr->api->create_sensor_instance_stream(
                                            stream_mgr,
                                            this,
                                            ascp_suid,
                                            &state->async_com_port_data_stream);

  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->common.com_port_info.com_config,
              sizeof(sensor_state->common.com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(
                                           &state->com_port_info.com_config,
                                           &state->com_port_info.port_handle);

  if(NULL == state->interrupt_data_stream ||
     NULL == state->async_com_port_data_stream ||
     NULL == state->com_port_info.port_handle)
  {
    inst_cleanup(this, stream_mgr);
    return SNS_RC_FAILED;
  }

  /**----------- Copy all Sensor UIDs in instance state -------------*/
  sns_memscpy(&state->accel_info.suid,
              sizeof(state->accel_info.suid),
              &((sns_sensor_uid) BMA2X2_ACCEL_SUID),
              sizeof(state->accel_info.suid));
  sns_memscpy(&state->md_info.suid,
              sizeof(state->md_info.suid),
              &((sns_sensor_uid) BMA2X2_MOTION_DETECT_SUID),
              sizeof(state->md_info.suid));
  sns_memscpy(&state->sensor_temp_info.suid,
              sizeof(state->sensor_temp_info.suid),
              &((sns_sensor_uid) BMA2X2_SENSOR_TEMPERATURE_SUID),
              sizeof(state->sensor_temp_info.suid));

  INST_DEBUG(MED, this, "<bma2x2_ iif> inst init @sensor:%d",
              sensor_state->sensor);
  /**-----------------------------------------------------------------*/
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer",
                      &state->timer_suid);

  /**-------------------------Init FIFO State-------------------------*/
  state->fifo_info.fifo_enabled = 0;
  state->fifo_info.fifo_rate = BMA2X2_ACCEL_ODR_OFF;
  state->fifo_info.cur_wml = 0;
  state->fifo_info.max_requested_wmk = 0;
  state->fifo_info.ff_flush_in_proc = false;

  /**-------------------------Init Accel State-------------------------*/
  state->accel_info.curr_odr_regv = BMA2X2_ACCEL_ODR_OFF;
  state->accel_info.sstvt = BMA2X2_ACCEL_SSTVT_8G;
  state->accel_info.range = BMA2X2_ACCEL_RANGE_8G;
  state->accel_info.bw = BMA2X2_ACCEL_BW50;
  state->accel_info.lp_mode = false;

  state->encoded_imu_event_len =
                          pb_get_encoded_size_sensor_stream_event(data, 3);
  state->encoded_sensor_temp_event_len =
                          pb_get_encoded_size_sensor_stream_event(temp_data, 1);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  state->scp_service =  (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  bma2x2_reset_device(state->scp_service,state->com_port_info.port_handle,
                      BMA2X2_ACCEL | BMA2X2_MOTION_DETECT | BMA2X2_SENSOR_TEMP);

  state->scp_service->api->sns_scp_update_bus_power(
                                              state->com_port_info.port_handle,
                                              false);

  /** Initialize IRQ info to be used by the Instance */
  state->irq_info.irq_config = sensor_state->common.irq_config;
  state->irq_info.irq_ready = false;

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

  /** Copy down axis conversion settings */
  sns_memscpy(state->axis_map,  sizeof(sensor_state->common.axis_map),
              sensor_state->common.axis_map, sizeof(sensor_state->common.axis_map));

  INST_DEBUG(MED, this, "axis_map[0] ipaxis:%d opaxis:%d invert:%d",
              state->axis_map[0].ipaxis,
              state->axis_map[0].opaxis,
              state->axis_map[0].invert);

  INST_DEBUG(MED, this, "axis_map[1] ipaxis:%d opaxis:%d invert:%d",
              state->axis_map[1].ipaxis,
              state->axis_map[1].opaxis,
              state->axis_map[1].invert);

  INST_DEBUG(MED, this, "axis_map[2] ipaxis:%d opaxis:%d invert:%d",
              state->axis_map[2].ipaxis,
              state->axis_map[2].opaxis,
              state->axis_map[2].invert);

  /** Initialize factory calibration */
  state->accel_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->accel_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->accel_registry_cfg.fac_cal_corr_mat.e22 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e11 = 1.0;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e22 = 1.0;

  /** Copy down MD configuration */
  sns_memscpy(&state->md_info.md_config, sizeof(state->md_info.md_config),
              &sensor_state->md_config, sizeof(sensor_state->md_config));

  /** Determine sizes of encoded logs */
  sns_diag_sensor_state_interrupt sensor_state_interrupt =
        sns_diag_sensor_state_interrupt_init_default;
  pb_get_encoded_size(&state->log_interrupt_encoded_size,
                      sns_diag_sensor_state_interrupt_fields,
                      &sensor_state_interrupt);

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

#if  BMA2X2_CONFIG_ENABLE_DAE
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data,
                      "data_acquisition_engine", &dae_suid);
  bma2x2_dae_if_init(this, stream_mgr, &dae_suid);
#endif

  state->accel_info.regv_cfg_manager.cfg_state = BMA2X2_REGV_CONFIG_IDLE;

  return SNS_RC_SUCCESS;
}


sns_rc bma2x2_inst_deinit(sns_sensor_instance *const this)
{
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  INST_DEBUG(MED, this, "<sns_bma2x2_iif_ inst_deinit> inst:0x%x", this);
  inst_cleanup(this, stream_mgr);

  return SNS_RC_SUCCESS;
}



// <self test>
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
static void bma2x2_send_self_test_event(sns_sensor_instance *instance,
                                        sns_sensor_uid *uid, bool test_result,
                                        sns_physical_sensor_test_type type,
                                        bma2x2_test_err_code err_code)
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




static
void bma2x2_test_fac_req_hw(sns_sensor_instance     *instance)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)instance->state->state;
  INST_DEBUG(HIGH, instance, "fac_test req_hw");

  istate->fifo_info.publish_sensors = 0;

  if (BMA2X2_ACCEL == istate->fac_test_sensor)
  {
    istate->fifo_info.publish_sensors |= BMA2X2_ACCEL;
    istate->accel_info.curr_odr_regv   = BMA2X2_ACCEL_ODR_OFF;
    istate->accel_info.range           = BMA2X2_ACCEL_RANGE_8G;

    istate->accel_info.acc_res_idx = BMA2X2_RANGE_ACC_8G_IDX;
    istate->accel_info.sample_rate_req = BMA2X2_ODR_125;
    istate->accel_info.report_rate_req = istate->accel_info.sample_rate_req;
  }

  bma2x2_hal_register_interrupt(instance);
  istate->irq_info.irq_ready = true;

  bma2x2_inst_assess_overall_req(instance);
  bma2x2_hal_reconfig_hw(instance);
}


int bma2x2_start_factory_test(sns_sensor_instance* instance,
                              bma2x2_sensor_type sensor)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)instance->state->state;

  if (state->accel_info.curr_odr_regv != BMA2X2_ACCEL_ODR_OFF)
  {
    INST_DEBUG(ERROR, instance, "WARNING!!! Data streaming still in process");
    return BMA2X2_FAC_TEST_DEV_NOT_STATIONARY;
  }

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
  state->fifo_info.fifo_enabled |= sensor | BMA2X2_ACCEL;

  if(sensor == BMA2X2_ACCEL)
  {
    state->fac_test_info.bias_thresholds[0] = 200 * G / 1000; // 180 m/s2
    state->fac_test_info.bias_thresholds[1] = 200 * G / 1000; // 180 m/s2
    state->fac_test_info.bias_thresholds[2] = 260 * G / 1000; // 210 m/s2
    state->fac_test_info.variance_threshold = 383.0; // 383(m/s2)2
  }
  else
  {
    SNS_INST_PRINTF(ERROR, instance, "Unknown sensor %d", sensor);
    return SNS_RC_FAILED;
  }

  bma2x2_test_fac_req_hw(instance);
  INST_DEBUG (HIGH, instance, "fac test sensor %d", sensor);
  return SNS_RC_SUCCESS;
}

/** See sns_bma2x2_hal.h */
void bma2x2_run_self_test(sns_sensor_instance *instance)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*) instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;

  rv = bma2x2_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);
  if (rv == SNS_RC_SUCCESS
      &&
	   ((buffer & BMA2X2_WHOAMI_VALUE_FLAGS) == BMA2X2_WHOAMI_VALUE_FLAGS))
  {
    who_am_i_success = true;
  }

  INST_DEBUG(HIGH, instance, "bma2x2_run_self_test()");

  if(state->accel_info.test_info.test_client_present)
  {
    INST_DEBUG (HIGH, instance, "test_client_present  test_type = %d",
                state->accel_info.test_info.test_type);
    if(state->accel_info.test_info.test_type ==
        SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bma2x2_send_self_test_event(instance, &state->accel_info.suid,
                                   who_am_i_success,
                                   SNS_PHYSICAL_SENSOR_TEST_TYPE_COM,
                                   BMA2X2_FAC_TEST_NO_ERROR);
    }
    else if(state->accel_info.test_info.test_type ==
            SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
      if ((who_am_i_success) &&
          (!state->fac_test_in_progress))
      {
        bma2x2_start_factory_test(instance, BMA2X2_ACCEL);
      }
    }
    state->accel_info.test_info.test_client_present = false;
  }

  if(state->md_info.test_info.test_client_present)
  {
    if(state->md_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bma2x2_send_self_test_event(instance, &state->md_info.suid,
                                     who_am_i_success,
                                     SNS_PHYSICAL_SENSOR_TEST_TYPE_COM,
                                     BMA2X2_FAC_TEST_NO_ERROR);
    }
    else if(state->md_info.test_info.test_type ==
            SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->md_info.test_info.test_client_present = false;
  }
  if(state->sensor_temp_info.test_info.test_client_present)
  {
    if(state->sensor_temp_info.test_info.test_type ==
        SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bma2x2_send_self_test_event(instance, &state->sensor_temp_info.suid,
                                     who_am_i_success,
                                     SNS_PHYSICAL_SENSOR_TEST_TYPE_COM,
                                     BMA2X2_FAC_TEST_NO_ERROR);
    }
    else if(state->sensor_temp_info.test_info.test_type ==
            SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->sensor_temp_info.test_info.test_client_present = false;
  }
}

static
void bma2x2_send_fac_cal_event(
        sns_sensor_instance *const  instance,
        sns_bma2x2_registry_cfg     *registry_cfg,
        sns_sensor_uid              *suid)
{
    bma2x2_instance_state *istate = (bma2x2_instance_state*)instance->state->state;

    sns_cal_event new_calibration_event = sns_cal_event_init_default;
    float bias_data[] = {0,0,0};
    float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

    UNUSED_VAR(istate);

    bias_data[0] = registry_cfg->fac_cal_bias[0]; // values in istate are usually from registry or from factory test routine
    bias_data[1] = registry_cfg->fac_cal_bias[1];
    bias_data[2] = registry_cfg->fac_cal_bias[2];
    comp_matrix_data[0] = registry_cfg->fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = registry_cfg->fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = registry_cfg->fac_cal_corr_mat.data[2];
    comp_matrix_data[3] = registry_cfg->fac_cal_corr_mat.data[3];
    comp_matrix_data[4] = registry_cfg->fac_cal_corr_mat.data[4];
    comp_matrix_data[5] = registry_cfg->fac_cal_corr_mat.data[5];
    comp_matrix_data[6] = registry_cfg->fac_cal_corr_mat.data[6];
    comp_matrix_data[7] = registry_cfg->fac_cal_corr_mat.data[7];
    comp_matrix_data[8] = registry_cfg->fac_cal_corr_mat.data[8];

    pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
    { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
    pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
    { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };

    new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
    new_calibration_event.bias.arg = &buff_arg_bias;
    new_calibration_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
    new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
    new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

    pb_send_event(instance,
            sns_cal_event_fields,
            &new_calibration_event,
            sns_get_system_time(),
            SNS_CAL_MSGID_SNS_CAL_EVENT,
            suid);
}

/**
 * release the H/W from factory testing
 * @param instance    instance handler
 */
static
void bma2x2_test_fac_rls_hw(sns_sensor_instance * const instance)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*) instance->state->state;

  INST_DEBUG (HIGH, instance, "fac_test rls_hw");

  istate->fifo_info.fifo_enabled    = 0;
  istate->fifo_info.publish_sensors = 0;
  istate->fifo_info.fifo_rate       = BMA2X2_ACCEL_ODR_OFF;
  istate->fifo_info.cur_wml         = 0;

  istate->accel_info.curr_odr_regv  = BMA2X2_ACCEL_ODR_OFF;
  istate->accel_info.ff_wml          = 0;
  istate->accel_info.sample_rate_req = 0.0;

  bma2x2_hal_reconfig_hw (instance);
}

void bma2x2_process_fac_test(sns_sensor_instance *inst)
{
  int i;
  bool test_pass = true;
  bma2x2_instance_state *state = (bma2x2_instance_state*)inst->state->state;
  sns_sensor_uid *uid = &state->accel_info.suid;
  sns_bma2x2_registry_cfg *registry_cfg = &state->accel_registry_cfg;
  bma2x2_test_err_code err_code = BMA2X2_FAC_TEST_NO_ERROR;

  INST_DEBUG(HIGH, inst, "@fac_test sensor: %d", state->fac_test_sensor);

  if(state->fac_test_info.num_samples >= 64)
  {
    INST_DEBUG(HIGH, inst, "sample sums: %d %d %d",
                    (int32_t) (state->fac_test_info.sample_sum[0] * 1000),
                    (int32_t) (state->fac_test_info.sample_sum[1] * 1000),
                    (int32_t) (state->fac_test_info.sample_sum[2] * 1000));
    INST_DEBUG(HIGH, inst, "sample square sums: %d %d %d",
                    (int32_t) (state->fac_test_info.sample_square_sum[0] * 1000),
                    (int32_t) (state->fac_test_info.sample_square_sum[1] * 1000),
                    (int32_t) (state->fac_test_info.sample_square_sum[2] * 1000));

    bma2x2_test_fac_rls_hw(inst);

    state->fac_test_in_progress = false;

    registry_cfg->fac_cal_bias[0] =
       state->fac_test_info.sample_sum[0] / (state->fac_test_info.num_samples - 4);
    registry_cfg->fac_cal_bias[1] =
       state->fac_test_info.sample_sum[1] / (state->fac_test_info.num_samples - 4);
    registry_cfg->fac_cal_bias[2] =
       state->fac_test_info.sample_sum[2] / (state->fac_test_info.num_samples - 4);
    registry_cfg->version ++;

    for(i = 0; i < TRIAXIS_NUM; i ++)
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
        err_code = BMA2X2_FAC_TEST_DEV_NOT_STATIONARY;
        SNS_INST_PRINTF(ERROR, inst,
                        "FAILED device not stationary var[%u]=%u  %u", i,
                        (uint32_t)state->fac_test_info.variance[i],
                        (uint32_t)state->fac_test_info.variance_threshold);
        break;
      }

      // Check biases are in defined limits
      if(fabsf(registry_cfg->fac_cal_bias[i]) > state->fac_test_info.bias_thresholds[i])
      {
        test_pass = false;
        err_code = BMA2X2_FAC_TEST_HIGH_BIAS;
        SNS_INST_PRINTF(ERROR, inst, "FAILED bias very large");
        break;
      }

      // Check for zero variance
      if(state->fac_test_info.variance[i] == 0.0)
      {
        test_pass = false;
        err_code = BMA2X2_FAC_TEST_ZERO_VARIANCE;
        SNS_INST_PRINTF(ERROR, inst, "FAILED zero variance");
        break;
      }
    }

    /** update_fac_cal_in_registry if test is successful.*/
    state->update_fac_cal_in_registry = test_pass;

    INST_DEBUG(MED, inst, "fac test passed:%d err code:%d",
                test_pass, err_code);

    bma2x2_send_cal_event(inst, state->fac_test_sensor);
    bma2x2_send_self_test_event(inst, uid,
                                 test_pass, SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);
    if (test_pass)
    {
      bma2x2_send_fac_cal_event(inst, registry_cfg, uid);
    }
  }
}
