/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
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

#include "sns_bmg160_hal.h"
#include "sns_bmg160_sensor.h"
#include "sns_bmg160_sensor_instance.h"

#include "sns_async_com_port.pb.h"
#include "sns_interrupt.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_pb_util.h"
#include "sns_sync_com_port_service.h"

#if BMG160_CONFIG_ENABLE_SELF_TEST_FAC
#include "sns_cal.pb.h"
#endif

static void inst_cleanup(sns_sensor_instance *const this, sns_stream_service *stream_mgr)
{
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  sns_rc rc = SNS_RC_SUCCESS;

  UNUSED_VAR(stream_mgr);
  // TODO: Is this OK to do here? Introduced by DAE sensor, since we could be
  // waiting for response from DAE before writing to HW -- but we don't have that chance.
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, true);
  }
  //bmg160_set_fifo_config(state, 0, 0, 0);
  //bmg160_hal_reconfig_hw(this);

  rc = bmg160_reset_device(state->scp_service,state->com_port_info.port_handle,
                         BMG160_GYRO | BMG160_SENSOR_TEMP);
  if(NULL != state->com_port_info.port_handle)
  {
    state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle, false);
  }

  rc |= sns_sensor_util_remove_sensor_instance_stream(this, &state->interrupt_data_stream);
  rc |= sns_sensor_util_remove_sensor_instance_stream(this, &state->async_com_port_data_stream);
  rc |= sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);

  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    rc |= state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, this);
}

static void bmg160_inst_collect_sensors(
        bmg160_instance_state *istate,
        bmg160_state           *sstate)
{
    sns_sensor                  *sensor_this = sstate->owner;
    sns_sensor                  *sensor;
    bmg160_state                *sensor_state;

    for (sensor = sensor_this->cb->get_library_sensor(sensor_this, true);
            sensor != NULL;
            sensor = sensor_this->cb->get_library_sensor(sensor_this, false)) {
        sensor_state = (bmg160_state*)sensor->state->state;
        if (BMG160_GYRO == sensor_state->sensor) {
            istate->gyro_info.sstate = sensor_state;
        } else if (BMG160_SENSOR_TEMP == sensor_state->sensor) {
            istate->sensor_temp_info.sstate = sensor_state;
        }
        istate->sensors_col |= sensor_state->sensor;
    }

    BMG160_INST_PRINTF(LOW, istate->owner, "sensors_col: 0x%x", istate->sensors_col);
}


/** See sns_sensor_instance_api::init */
sns_rc bmg160_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate)
{
  bmg160_instance_state *state =
              (bmg160_instance_state*)this->state->state;
  bmg160_state *sensor_state =
              (bmg160_state*)sstate->state;
  float data[3] = {0.0f};
  float temp_data[1] = {0.0f};
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  uint64_t buffer[10] = {0};
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float diag_temp[BMG160_NUM_AXES] = {0.0f};
  pb_float_arr_arg arg = {.arr = (float*)diag_temp, .arr_len = BMG160_NUM_AXES,
    .arr_index = &arr_index};
  batch_sample.sample.funcs.encode = pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;
  sns_sensor_uid irq_suid, ascp_suid;

  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);
#if BMG160_CONFIG_ENABLE_ISLAND_MODE
  state->island_service = (sns_island_service*)
              service_mgr->get_service(service_mgr, SNS_ISLAND_SERVICE);
#endif

  /**---------Setup stream connections with dependent Sensors---------*/
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "interrupt", &irq_suid);
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "async_com_port", &ascp_suid);
  stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                 this,
                                                 irq_suid,
                                                 &state->interrupt_data_stream);

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

  if(NULL == state->interrupt_data_stream ||
     NULL == state->async_com_port_data_stream ||
     NULL == state->com_port_info.port_handle)
  {
    inst_cleanup(this, stream_mgr);
    return SNS_RC_FAILED;
  }

  /**----------- Copy all Sensor UIDs in instance state -------------*/
  sns_memscpy(&state->gyro_info.suid,
              sizeof(state->gyro_info.suid),
              &((sns_sensor_uid)GYRO_SUID),
              sizeof(state->gyro_info.suid));
  sns_memscpy(&state->sensor_temp_info.suid,
              sizeof(state->sensor_temp_info.suid),
              &((sns_sensor_uid)SENSOR_TEMPERATURE_SUID),
              sizeof(state->sensor_temp_info.suid));
  /**-----------------------------------------------------------------*/
  state->regv_cfg_ctrl.cfg_state = BMG160_REGV_CONFIG_IDLE;
  state->regv_cfg_ctrl.reg_config_timer = NULL;
  sns_suid_lookup_get(&sensor_state->common.suid_lookup_data, "timer", &state->timer_suid);
  /**-------------------------Init FIFO State-------------------------*/
  state->fifo_info.fifo_enabled = 0;
  state->fifo_info.fifo_rate = BMG160_GYRO_ODR_OFF;
  state->fifo_info.cur_wml = 0;
  state->fifo_info.max_requested_wmk = 0;
  state->in_low_power_mode = 1;

  /**-------------------------Init Gyro State-------------------------*/
  state->encoded_imu_event_len = pb_get_encoded_size_sensor_stream_event(data, 3);
  state->encoded_sensor_temp_event_len = pb_get_encoded_size_sensor_stream_event(temp_data, 1);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);

  state->scp_service =  (sns_sync_com_port_service*)
      service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  //reset device
  /*bmg160_reset_device(state->scp_service,state->com_port_info.port_handle,
                         BMG160_GYRO | BMG160_SENSOR_TEMP);*/

  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
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

  /** Initialize factory calibration */
  state->gyro_registry_cfg.fac_cal_corr_mat.e00 = 1.0f;
  state->gyro_registry_cfg.fac_cal_corr_mat.e11 = 1.0f;
  state->gyro_registry_cfg.fac_cal_corr_mat.e22 = 1.0f;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 = 1.0f;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e11 = 1.0f;
  state->sensor_temp_registry_cfg.fac_cal_corr_mat.e22 = 1.0f;

  /** Determine sizes of encoded logs */
  sns_diag_sensor_state_interrupt sensor_state_interrupt =
        sns_diag_sensor_state_interrupt_init_default;
  pb_get_encoded_size(&state->log_interrupt_encoded_size,
                      sns_diag_sensor_state_interrupt_fields,
                      &sensor_state_interrupt);

  state->owner = this;
  // collect all the sensors information
  bmg160_inst_collect_sensors(state, sensor_state);
  state->gyro_info.curr_odr_regv = BMG160_GYRO_ODR_OFF;
  {
    if (state->gyro_info.sstate->resolution_idx > BMG160_REGV_RANGE_MAX)
    {
      state->gyro_info.resolution    = BMG160_SSTVT_2000DPS;
      state->gyro_info.range_regv    = BMG160_GYRO_RANGE_2000DPS;
    }
    else
    {
      state->gyro_info.resolution    = BMG160_RESOLUTION_BASE *
          (1 << state->gyro_info.sstate->resolution_idx);
      state->gyro_info.range_regv    = BMG160_REGV_RANGE_MAX -
          state->gyro_info.sstate->resolution_idx;
    }
  }

  state->gyro_info.lp_mode       = false;



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


  BMG160_INST_PRINTF (MED, this, "<bmg160_iif inst_init> 0x%x %d",
                      this, sensor_state->sensor);

  return SNS_RC_SUCCESS;
}

sns_rc bmg160_inst_deinit(sns_sensor_instance *const this)
{
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  BMG160_INST_PRINTF (MED, this, "<bmg160_iif inst_deinit> 0x%x", this);
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
static void bmg160_send_self_test_event(sns_sensor_instance *instance,
                                         sns_sensor_uid *uid, bool test_result,
                                         sns_physical_sensor_test_type type,
                                         bmg160_test_err_code err_code)
{
  uint8_t data[1] = {(uint8_t)err_code};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = type;
  test_event.test_data.funcs.encode = pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}


#if BMG160_CONFIG_ENABLE_SELF_TEST_FAC
static
void bmg160_test_fac_rls_hw(sns_sensor_instance     *instance)
{
  bmg160_instance_state *istate =
      (bmg160_instance_state*) instance->state->state;

  BMG160_INST_PRINTF(MED, instance, "fac_test rls_hw");

  istate->fifo_info.publish_sensors = 0;
  istate->fifo_info.fifo_enabled = 0;

  istate->fifo_info.cur_wml = 0;
  istate->gyro_info.sample_rate_req = 0;
  istate->gyro_info.report_rate_req = 0;

  bmg160_hal_reconfig_hw (instance);
}

static
void bmg160_test_fac_req_hw(sns_sensor_instance * const instance)
{
  bmg160_instance_state *istate =
      (bmg160_instance_state*) instance->state->state;

  BMG160_INST_PRINTF(MED, instance, "fac_test req_hw");

  istate->fifo_info.publish_sensors = 0;

  if (BMG160_GYRO == istate->fac_test_info.fac_test_sensor)
  {
    istate->fifo_info.publish_sensors |= BMG160_GYRO;

    istate->gyro_info.curr_odr_regv = BMG160_GYRO_ODR_OFF;
    istate->gyro_info.range_regv    = BMG160_GYRO_RANGE_2000DPS;
    istate->gyro_info.resolution    = BMG160_SSTVT_2000DPS;

    // force to apply the current configuration
    istate->gyro_info.sample_rate_req = 100;
    istate->gyro_info.report_rate_req = istate->gyro_info.sample_rate_req;
  }

  bmg160_hal_register_interrupt (instance);
  istate->irq_info.irq_ready = true;

  bmg160_inst_assess_overall_req (instance);
  bmg160_hal_reconfig_hw (instance);
}


void bmg160_start_factory_test(sns_sensor_instance* instance,
                               bmg160_sensor_type sensor)
{
  bmg160_instance_state *state = (bmg160_instance_state*)instance->state->state;

  state->fac_test_info.num_samples = 0;
  state->fac_test_info.sample_square_sum[0] = 0;
  state->fac_test_info.sample_square_sum[1] = 0;
  state->fac_test_info.sample_square_sum[2] = 0;
  state->fac_test_info.sample_sum[0] = 0;
  state->fac_test_info.sample_sum[1] = 0;
  state->fac_test_info.sample_sum[2] = 0;
  state->fac_test_in_progress = true;
  state->fac_test_info.fac_test_sensor = sensor;
  state->fac_test_info.at_rest = true;
  state->fifo_info.fifo_enabled |= sensor | BMG160_GYRO;

  if(sensor == BMG160_GYRO)
  {
    state->fac_test_info.bias_thresholds[0] = 40 * PI / 180; //40 rad/sec
    state->fac_test_info.bias_thresholds[1] = 40 * PI / 180; //40 rad/sec
    state->fac_test_info.bias_thresholds[2] = 40 * PI / 180; //40 rad/sec
    state->fac_test_info.variance_threshold = 100.0f; // 100(rad/sec)2
  }
  else
  {
    SNS_INST_PRINTF(ERROR, instance, "Unknown sensor %d", sensor);
  }

  BMG160_INST_PRINTF(HIGH, instance, "bmg160_start_factory_test() sensor %d", sensor);
  bmg160_test_fac_req_hw(instance);
}



void bmg160_send_fac_cal_event(
        sns_sensor_instance *const  instance,
        bmg160_state *sstate)
{
  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0, 0, 0};
  float comp_matrix_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  bmg160_instance_state *istate = (bmg160_instance_state*)instance->state->state;
  const sns_sensor_uid            *suid = &(sstate->my_suid);

  UNUSED_VAR(istate);

  bias_data[0] = sstate->fac_cal_bias[0]; // values in istate are usually from registry or from factory test routine
  bias_data[1] = sstate->fac_cal_bias[1];
  bias_data[2] = sstate->fac_cal_bias[2];

  comp_matrix_data[0] = sstate->fac_cal_corr_mat.data[0];
  comp_matrix_data[1] = sstate->fac_cal_corr_mat.data[1];
  comp_matrix_data[2] = sstate->fac_cal_corr_mat.data[2];
  comp_matrix_data[3] = sstate->fac_cal_corr_mat.data[3];
  comp_matrix_data[4] = sstate->fac_cal_corr_mat.data[4];
  comp_matrix_data[5] = sstate->fac_cal_corr_mat.data[5];
  comp_matrix_data[6] = sstate->fac_cal_corr_mat.data[6];
  comp_matrix_data[7] = sstate->fac_cal_corr_mat.data[7];
  comp_matrix_data[8] = sstate->fac_cal_corr_mat.data[8];

  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      {.buf = &bias_data, .buf_len = ARR_SIZE(
          bias_data)};
  pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
      {.buf = &comp_matrix_data, .buf_len = ARR_SIZE(
          comp_matrix_data)};

  new_calibration_event.bias.funcs.encode = pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.comp_matrix.funcs.encode = pb_encode_float_arr_cb;
  new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  BMG160_INST_PRINTF(HIGH, instance, "bmg cal evt: %d <%d %d %d> ver: %d", sstate->sensor,
          (int)(bias_data[0] * 10000), (int)(bias_data[1] * 10000), (int)(bias_data[2] * 10000),
          sstate->fac_cal_version);

  pb_send_event (instance, sns_cal_event_fields, &new_calibration_event,
                 sns_get_system_time (), SNS_CAL_MSGID_SNS_CAL_EVENT, suid);
}

void bmg160_process_fac_test(sns_sensor_instance *instance)
{
  if (NULL == instance) {
    return ;
  }
  int i;
  bool test_pass = true;
  bmg160_instance_state *state = (bmg160_instance_state*)instance->state->state;
  sns_sensor_uid *uid = &state->gyro_info.suid;
  sns_bmg160_registry_cfg *registry_cfg = &state->gyro_registry_cfg;
  bmg160_test_err_code err_code = BMG160_TEST_NO_ERROR;
  bmg160_state *sstate = NULL;

  BMG160_INST_PRINTF(HIGH, instance, "Sensor %d",
      state->fac_test_info.fac_test_sensor);

  if(state->fac_test_info.fac_test_sensor == BMG160_GYRO)
  {
    registry_cfg = &state->gyro_registry_cfg;
    uid = &state->gyro_info.suid;
    sstate = state->gyro_info.sstate;
  }

  if(state->fac_test_info.num_samples >= 64)
  {
    BMG160_INST_PRINTF(HIGH, instance, "sample sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_sum[0],
                    (int32_t)state->fac_test_info.sample_sum[1],
                    (int32_t)state->fac_test_info.sample_sum[2]);
    BMG160_INST_PRINTF(HIGH, instance, "sample square sums: %d %d %d",
                    (int32_t)state->fac_test_info.sample_square_sum[0],
                    (int32_t)state->fac_test_info.sample_square_sum[1],
                    (int32_t)state->fac_test_info.sample_square_sum[2]);

    state->fac_test_in_progress = false;
    bmg160_test_fac_rls_hw(instance);

    registry_cfg->fac_cal_bias[0] =
       state->fac_test_info.sample_sum[0] / state->fac_test_info.num_samples;
    registry_cfg->fac_cal_bias[1] =
       state->fac_test_info.sample_sum[1] / state->fac_test_info.num_samples;
    registry_cfg->fac_cal_bias[2] =
       state->fac_test_info.sample_sum[2] / state->fac_test_info.num_samples;
    registry_cfg->version ++;

    BMG160_INST_PRINTF(MED, instance, "fac.t #bias# x(10000):%d, y(10000):%d, z(10000):%d",
        (int32_t) (registry_cfg->fac_cal_bias[0] * 10000),
        (int32_t) (registry_cfg->fac_cal_bias[1] * 10000),
        (int32_t) (registry_cfg->fac_cal_bias[2] * 10000));

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
        err_code = BMG160_FAC_TEST_DEV_NOT_STATIONARY;
        BMG160_INST_PRINTF(ERROR, instance, "FAILED device not stationary var[%u]=%u  %u",
                        i, (uint32_t)state->fac_test_info.variance[i],
                        (uint32_t)state->fac_test_info.variance_threshold);
        break;
      }

      // Check biases are in defined limits
      if(fabsf(registry_cfg->fac_cal_bias[i]) > state->fac_test_info.bias_thresholds[i])
      {
        test_pass = false;
        err_code = BMG160_FAC_TEST_HIGH_BIAS;
        BMG160_INST_PRINTF(ERROR, instance, "FAILED bias very large");
        break;
      }

      // Check for zero variance
      if(state->fac_test_info.variance[i] == 0.0)
      {
         test_pass = false;
         err_code = BMG160_FAC_TEST_ZERO_VARIANCE;
         BMG160_INST_PRINTF(ERROR, instance, "FAILED zero variance");
         break;
      }
    }

    /** update_fac_cal_in_registry if test is successful.*/
    state->update_fac_cal_in_registry = test_pass;

    if (test_pass) {
      BMG160_INST_PRINTF(MED, instance, "CONGRATULATIONS!!! fac test passed");
    }

    bmg160_send_cal_event(instance, state->fac_test_info.fac_test_sensor);
    bmg160_send_self_test_event(instance, uid, test_pass,
                                SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY, err_code);
    if (test_pass) {
      if (sstate == NULL) {
        BMG160_INST_PRINTF(ERROR, instance, "can't find sensor for this fac");
        return ;
      }
      for (i = 0; i < TRIAXIS_NUM; i++) {
          sstate->fac_cal_bias[i] = (registry_cfg->fac_cal_bias[i]);
      }
      sstate->fac_cal_version ++;
      bmg160_send_fac_cal_event(instance, sstate);
    }
  }
}
#endif

/** See sns_bmg160_hal.h */
void bmg160_run_self_test(sns_sensor_instance *instance)
{
  if (NULL == instance) { return ; }
  bmg160_instance_state *state = (bmg160_instance_state*)instance->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint8_t buffer = 0;
  bool who_am_i_success = false;

  rv = bmg160_get_who_am_i(state->scp_service,
                            state->com_port_info.port_handle,
                            &buffer);
  if(rv == SNS_RC_SUCCESS
     &&
     buffer == BMG160_WHOAMI_VALUE)
  {
    who_am_i_success = true;
  }

  BMG160_INST_PRINTF(HIGH, instance, "bmg160_run_self_test()");

  if(state->gyro_info.test_info.test_client_present)
  {
    if(state->gyro_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmg160_send_self_test_event(instance, &state->gyro_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMG160_TEST_NO_ERROR);
    }
    else if(state->gyro_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
       bmg160_start_factory_test(instance, BMG160_GYRO);
    }
    state->gyro_info.test_info.test_client_present = false;
  }

  if(state->sensor_temp_info.test_info.test_client_present)
  {
    if(state->sensor_temp_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      bmg160_send_self_test_event(instance, &state->sensor_temp_info.suid,
                                   who_am_i_success, SNS_PHYSICAL_SENSOR_TEST_TYPE_COM, BMG160_TEST_NO_ERROR);
    }
    else if(state->sensor_temp_info.test_info.test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      // Handle factory test. The driver may choose to reject any new
      // streaming/self-test requests when factory test is in progress.
    }
    state->sensor_temp_info.test_info.test_client_present = false;
  }
}


void bmg160_reset_fac_cal_data(
        sns_sensor_instance *const  instance,
        struct bmg160_state         *sstate,
        bool                        update_version)
{
    UNUSED_VAR(instance);

    memset(sstate->fac_cal_bias, 0, sizeof(sstate->fac_cal_bias));
    memset(&sstate->fac_cal_corr_mat, 0, sizeof(sstate->fac_cal_corr_mat));

    sstate->fac_cal_corr_mat.e00 = 1.0f;
    sstate->fac_cal_corr_mat.e11 = 1.0f;
    sstate->fac_cal_corr_mat.e22 = 1.0f;

    if (update_version) {
        sstate->fac_cal_version++;
    }
}
