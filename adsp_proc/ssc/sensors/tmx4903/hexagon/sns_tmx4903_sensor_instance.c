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
#include "sns_tmx4903_sensor_instance.h"
#include "sns_tmx4903_hal.h"

/**
 * Publishes a calibration event
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 * @param[i] scale_result
 * @param[i] scale_size
 * @param[i] bias_result
 * @param[i] bias_size
 *
 * @return none
 */
void _ams_send_cal_event(sns_sensor_instance *instance,
                                sns_sensor_uid *uid,
                                float *scale_result, uint8_t scale_size,
                                float *bias_result, uint8_t bias_size);

/**
 * Wrap up an ALS sample and generate an event.
 *
 * @param[i] state              The state of the sensor instance
 * @param[i] diag               The instance's diag_service
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current sensor instance
 * @param[i] log_als_state_raw_info  Info on the log packet
 */
void ams_generateAlsResponse(ams_instance_state *state,
                             sns_diag_service* diag,
                             sns_time timestamp,
                             sns_sensor_instance *const instance,
                             ams_reportType_t report_type);
/**
 * Wrap up an RGB sample and generate an event.
 *
 * @param[i] state              The state of the sensor instance
 * @param[i] diag               The instance's diag_service
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current sensor instance
 *
 */
void ams_generateRgbResponse(ams_instance_state *state,
                             sns_diag_service* diag,
                             sns_time timestamp,
                             sns_sensor_instance *const instance,
                             ams_reportType_t report_type);
/**
 * Wrap up a Proximity sample and generate an event.
 *
 * @param[i] state              The state of the sensor instance
 * @param[i] diag               The instance's diag_service
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current sensor instance
 * @param[i] log_prox_state_raw_info  Info on the log packet
 */
void ams_generateProxResponse(ams_instance_state *state,
                              sns_diag_service* diag,
                              sns_time timestamp,
                              sns_sensor_instance *const instance,
                              log_sensor_state_raw_info *log_prox_state_raw_info,
                              ams_reportType_t report_type);
/**
 * Runs a factory test
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
void _ams_send_factory_test_event(sns_sensor_instance *instance,
                                  sns_sensor_uid *uid, bool test_result);


static void inst_cleanup(ams_instance_state *state, sns_stream_service *stream_mgr)
{
  if(NULL != state->interrupt_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->interrupt_data_stream);
    state->interrupt_data_stream = NULL;
  }
  if(NULL != state->timer_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
    state->timer_data_stream = NULL;
  }
  if(NULL != state->scp_service)
  {
    state->scp_service->api->sns_scp_close(state->com_port_info.port_handle);
    state->scp_service->api->sns_scp_deregister_com_port(&state->com_port_info.port_handle);
    state->scp_service = NULL;
  }
}

static void ams_init_diag_log_size(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

  float als_temp[ALS_PROTO_STD_ARRAY_LEN] = {0.0};
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  float rgb_temp[RGB_PROTO_STD_ARRAY_LEN] = {0.0};
#endif
  uint8_t arr_index = 0;
  pb_float_arr_arg als_arg = {.arr = (float*)als_temp, .arr_len = ALS_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  pb_float_arr_arg rgb_arg = {.arr = (float*)rgb_temp, .arr_len = RGB_PROTO_STD_ARRAY_LEN,
    .arr_index = &arr_index};
#endif
  uint64_t buffer[50];
  pb_ostream_t stream = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &als_arg;
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                           &batch_sample))
    {
      state->als_enc_raw_log_size = stream.bytes_written;
    }
  }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  batch_sample.sample.arg = &rgb_arg;
  if(pb_encode_tag(&stream, PB_WT_STRING,
                    sns_diag_sensor_state_raw_sample_tag))
  {
    if(pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                           &batch_sample))
    {
      state->rgb_enc_raw_log_size = stream.bytes_written;
    }
  }
#endif
}

/** See sns_sensor_instance_api::init */
sns_rc ams_inst_init(sns_sensor_instance *const this,
    sns_sensor_state const *sstate)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  ams_state *sensor_state = (ams_state*)sstate->state;
  float data[20];
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  AMS_PORT_log_Msg_3(AMS_DEBUG, "AMS: ams_inst_init: enter: this %p, sensor_state %p, sensor_type=%d"
                    , this
                    , sensor_state
                    , sensor_state->sensor_type
                    );

  state->priv = sensor_state->priv; // local copy of our deviceCtx ptr
  state->sensor_type = sensor_state->sensor_type;
  state->publish_sensors = 0;
  state->publish_sensors_prev = 0;

  state->scp_service = (sns_sync_com_port_service*)
              service_mgr->get_service(service_mgr, SNS_SYNC_COM_PORT_SERVICE);

  state->diag_service =  (sns_diag_service*)
      service_mgr->get_service(service_mgr, SNS_DIAG_SERVICE);


  sns_memscpy (&state->irq_suid, sizeof(sns_sensor_uid),
        &sensor_state->irq_suid, sizeof(sns_sensor_uid));

  sns_memscpy (&state->timer_suid, sizeof(sns_sensor_uid),
        &sensor_state->timer_suid, sizeof(sns_sensor_uid));

  state->irq_info.irq_config = sensor_state->irq_config;


  /** Initialize COM port to be used by the Instance */
  sns_memscpy(&state->com_port_info.com_config,
              sizeof(state->com_port_info.com_config),
              &sensor_state->com_port_info.com_config,
              sizeof(sensor_state->com_port_info.com_config));

  state->scp_service->api->sns_scp_register_com_port(&state->com_port_info.com_config,
                                              &state->com_port_info.port_handle);

  AMS_PORT_log_Msg_1(AMS_DEBUG, "ams_inst_init:  data_stream_:  scp handle %p"
                    , state->com_port_info.port_handle
                    );
  if(NULL == state->com_port_info.port_handle)
  {
    inst_cleanup(state, stream_mgr);
    return SNS_RC_FAILED;
  }

  state->scp_service->api->sns_scp_open(state->com_port_info.port_handle);

  /* Fill out the instance's portHndl */
  state->port_handle.scp_service = state->scp_service;
  state->port_handle.scp_handle = state->com_port_info.port_handle;
  state->port_handle.diag = state->diag_service;
  state->port_handle_ptr = &state->port_handle;

  /**----------- Copy all Sensor UIDs in instance state -------------*/
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  sns_memscpy(&state->als_info.suid[AMS_OC_IDX],
              sizeof(state->als_info.suid[AMS_OC_IDX]),
              &((sns_sensor_uid)ALS_SUID),
              sizeof(state->als_info.suid[AMS_OC_IDX]));
  sns_memscpy(&state->als_info.suid[AMS_STRM_IDX],
              sizeof(state->als_info.suid[AMS_STRM_IDX]),
              &((sns_sensor_uid)ALS_STREAMING_SUID),
              sizeof(state->als_info.suid[AMS_STRM_IDX]));
  state->encoded_als_event_len = pb_get_encoded_size_sensor_stream_event(data, ALS_PROTO_STD_ARRAY_LEN);
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  sns_memscpy(&state->rgb_info.suid[AMS_OC_IDX],
              sizeof(state->rgb_info.suid[AMS_OC_IDX]),
              &((sns_sensor_uid)RGB_SUID),
              sizeof(state->rgb_info.suid[AMS_OC_IDX]));
  sns_memscpy(&state->rgb_info.suid[AMS_STRM_IDX],
              sizeof(state->rgb_info.suid[AMS_STRM_IDX]),
              &((sns_sensor_uid)RGB_STREAMING_SUID),
              sizeof(state->rgb_info.suid[AMS_STRM_IDX]));
  state->encoded_rgb_event_len = pb_get_encoded_size_sensor_stream_event(data, RGB_PROTO_STD_ARRAY_LEN);
#endif
#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  sns_memscpy(&state->prox_info.suid[AMS_OC_IDX],
              sizeof(state->prox_info.suid[AMS_OC_IDX]),
              &(sns_sensor_uid)PROX_SUID,
              sizeof(state->prox_info.suid[AMS_OC_IDX]));
  sns_memscpy(&state->prox_info.suid[AMS_STRM_IDX],
              sizeof(state->prox_info.suid[AMS_STRM_IDX]),
              &(sns_sensor_uid)PROX_STREAMING_SUID,
              sizeof(state->prox_info.suid[AMS_STRM_IDX]));
#endif

  /* reset entire chip */
  state->calibration_data = sensor_state->calibration_data;
  sns_memscpy(state->cal_version,
              sizeof(state->cal_version),
              sensor_state->cal_version,
              sizeof(sensor_state->cal_version));
  ams_deviceInit(state->priv, state->port_handle_ptr, &sensor_state->calibration_data);

  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);


  ams_init_diag_log_size(this);
  return SNS_RC_SUCCESS;
}

bool ams_validate_device_odr(sns_sensor_instance *const instance, float *sample_rate)
{
  ams_instance_state *state = (ams_instance_state*)instance->state->state;
  bool change_in_sr = false;

  if(*sample_rate > 0.0
     && *sample_rate <= AMS_DEVICE_ODR_1)
  {
    *sample_rate = AMS_DEVICE_ODR_1;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_1
          && *sample_rate <= AMS_DEVICE_ODR_2)
  {
    *sample_rate = AMS_DEVICE_ODR_2;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_2
          && *sample_rate <= AMS_DEVICE_ODR_5)
  {
    *sample_rate = AMS_DEVICE_ODR_5;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_5
          && *sample_rate <= AMS_DEVICE_ODR_10)
  {
    *sample_rate = AMS_DEVICE_ODR_10;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_10
          && *sample_rate <= AMS_DEVICE_ODR_15)
  {
    *sample_rate = AMS_DEVICE_ODR_15;
  }
  else if(*sample_rate > AMS_DEVICE_ODR_15)
  {
    *sample_rate = AMS_DEVICE_ODR_20;
  }
  else
  {
    state->sampling_rate_hz = 0;
    state->sampling_intvl = 0;
  }

  if(state->sampling_rate_hz != *sample_rate)
  {
    change_in_sr = true;
    union {
        uint32_t  dwds[2];
        uint64_t qwd;
    } ticks;

    if(*sample_rate > 0)
    {
      state->sampling_intvl =
        sns_convert_ns_to_ticks((uint64_t)(1000000000.0 / *sample_rate));
      state->sampling_rate_hz = *sample_rate;
    }
    else
    {
      state->sampling_intvl = 0;
      state->sampling_rate_hz = 0.0f;
    }

    ticks.qwd = state->sampling_intvl;
    SNS_INST_PRINTF(LOW, instance
                    , "AMS: ams_validate_device_odr: sampling_intvl=(%08x:%08x)"
                    , ticks.dwds[1]
                    , ticks.dwds[0]
                    );
  }

  return change_in_sr;
}

/**
 * Runs a communication test - verfies WHO_AM_I, publishes self
 * test event.
 *
 * @param[i] instance  Instance reference
 * @param[i] uid       Sensor UID
 *
 * @return none
 */
static void _ams_send_com_test_event(sns_sensor_instance *instance,
                                     sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;

  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_COM;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);


}

static void _ams_setup_clean_irq_stream (sns_sensor_instance *this, bool on_of)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  if (on_of) {
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                   this,
                                                   state->irq_suid,
                                                   &state->interrupt_data_stream);

    /** Initialize IRQ info to be used by the Instance */
    sns_data_stream* data_stream = state->interrupt_data_stream;
    uint8_t buffer[20];
    sns_request irq_req =
    {
      .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
      .request    = buffer
    };

    sns_memset(buffer, 0, sizeof(buffer));
    irq_req.request_len = pb_encode_request(buffer,
                                            sizeof(buffer),
                                            &state->irq_info.irq_config,
                                            sns_interrupt_req_fields,
                                            NULL);
    if(irq_req.request_len > 0)
    {
      data_stream->api->send_request(data_stream, &irq_req);
    }

    AMS_PORT_log_Msg(AMS_DEBUG, "AMS: ams_inst_init: sent IRQ rqst");

  } else {
    if(NULL != state->interrupt_data_stream)
    {
      ams_deviceSetConfig(state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 0);
      stream_mgr->api->remove_stream(stream_mgr, state->interrupt_data_stream);
      state->interrupt_data_stream = NULL;
    }
  }
}

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
static void _ams_start_polling_timer(sns_sensor_instance *this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
              service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50];
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };

  if (NULL == state->timer_data_stream)
  {
    stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                   this,
                                                   state->timer_suid,
                                                   &state->timer_data_stream);
  }

  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->sampling_intvl;

  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
    state->timer_is_active = true;
  }
  else
  {
    //diag->api->sensor_printf(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}

static void _ams_run_self_test(sns_sensor_instance *instance)
{
  ams_instance_state *state = (ams_instance_state*)instance->state->state;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  bool who_am_i_success;
  bool bret;
  bool handlerIsReady = (devCtx && devCtx->portHndl);

  if(handlerIsReady)
  {
    ams_deviceIdentifier_e devId;
    devId = ams_validateDevice(devCtx->portHndl);
    who_am_i_success = (devId != AMS_UNKNOWN_DEVICE) && (devId != AMS_LAST_DEVICE) ? true : false;
  }
  else
  {
    who_am_i_success = false;
  }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->als_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->als_info.suid[AMS_OC_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->als_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    else if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateLux");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateLux(devCtx, NULL);
      }
      if (!bret)
      {
        _ams_send_factory_test_event(instance, &state->als_info.suid[AMS_OC_IDX], false);
      }
    }
  }
  if(state->als_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->als_info.suid[AMS_STRM_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->als_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    else if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateLux");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateLux(devCtx, NULL);
      }
      if (bret)
      {
        _ams_start_polling_timer(instance);
      }
      else
      {
        _ams_send_factory_test_event(instance, &state->als_info.suid[AMS_STRM_IDX], false);
      }
    }
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->prox_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->prox_info.suid[AMS_OC_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->prox_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    else if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateProx");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateProx(devCtx, NULL);
      }
      if(!bret)
      {
        _ams_send_factory_test_event(instance, &state->prox_info.suid[AMS_OC_IDX], false);
      }
    }
  }
  if(state->prox_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->prox_info.suid[AMS_STRM_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->prox_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    else if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateProx");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateProx(devCtx, NULL);
      }
      if (bret)
      {
        _ams_start_polling_timer(instance);
      }
      else
      {
        _ams_send_factory_test_event(instance, &state->prox_info.suid[AMS_STRM_IDX], false);
      }
    }
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  if(state->rgb_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->rgb_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->rgb_info.suid[AMS_OC_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->rgb_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    else if(state->rgb_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateRgb");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateRgb(devCtx, NULL);
      }
      if (!bret)
      {
        _ams_send_factory_test_event(instance, &state->rgb_info.suid[AMS_OC_IDX], false);
      }
    }
  }
  if(state->rgb_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->rgb_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_COM)
    {
      _ams_send_com_test_event(instance, &state->rgb_info.suid[AMS_STRM_IDX], who_am_i_success);
      state->new_self_test_request = false;
      state->rgb_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    else if(state->rgb_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      SNS_INST_PRINTF(HIGH, instance,
                     "AMS: _ams_run_self_test: calling ams_deviceCalibrateRgb");
      bret = false;
      if(handlerIsReady)
      {
        bret = ams_deviceCalibrateRgb(devCtx, NULL);
      }
      if (bret)
      {
        _ams_start_polling_timer(instance);
      }
      else
      {
        _ams_send_factory_test_event(instance, &state->rgb_info.suid[AMS_STRM_IDX], false);
      }
    }
  }
#endif
}

/**
 * Updates polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
static void _ams_set_polling_config(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

  {
    union {
        uint32_t dwds[2];
        uint64_t qwd;
    } ticks;
    ticks.qwd = state->sampling_intvl;
    SNS_INST_PRINTF(LOW, this
                    , "AMS: ams_set_polling_config: sampling_intvl=(%08x:%08x), timer_is_active=%d"
                    , ticks.dwds[1], ticks.dwds[0]
                    , state->timer_is_active
                    );
  }

  if(state->sampling_intvl > 0
     &&
     !state->new_self_test_request) // don't start the timer if self_test_request
  {
    _ams_start_polling_timer(this);
  }
  else if(state->timer_is_active)
  {
    state->timer_is_active = false;

    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
                service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    if(NULL != state->timer_data_stream)
    {
      stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
      state->timer_data_stream = NULL;
    }
  }
}

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
static void _ams_reconfig_hw(sns_sensor_instance *this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;

  SNS_INST_PRINTF(LOW, this
                  , "AMS: _ams_reconfig_hw: publish_sensors=0x%04x"
                  , state->publish_sensors
                  );

  if(state->publish_sensors & AMS_STREAMERS)
  {
    _ams_set_polling_config(this);
  }
  else
  {
    /* QC - the periodic timer would still be running at this point; must close timer stream to turn off this timer */
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
                service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    if(NULL != state->timer_data_stream)
    {
      stream_mgr->api->remove_stream(stream_mgr, state->timer_data_stream);
      state->timer_data_stream = NULL;
    }
    state->timer_is_active = false;
  }
}

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
static void _ams_send_config_event(sns_sensor_instance *const instance)
{
  ams_instance_state *state =
     (ams_instance_state*)instance->state->state;
  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;
  // TODO: Use appropriate op_mode selected by driver.
  char operating_mode[] = "NORMAL";
  pb_buffer_arg op_mode_args;

  SNS_INST_PRINTF(LOW, instance
                  , "AMS: _ams_send_config_event: enter: inst_this %p, .publish_sensors=0x%04x"
                  , instance
                  , state->publish_sensors
                  );
  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  ams_deviceInfo_t info;
  ams_getDeviceInfo(&info);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->publish_sensors & (AMS_ALS_OC | AMS_RGB_OC))
  {
    phy_sensor_config.has_sample_rate = false;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = info.alsSensor.activeCurrent_uA;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = (1 / info.alsSensor.adcBits);
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = info.alsSensor.rangeMin;
    phy_sensor_config.range[1] = info.alsSensor.rangeMax;
    phy_sensor_config.has_stream_is_synchronous = false;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = true;
    phy_sensor_config.has_DAE_watermark = false;

    if(state->publish_sensors & AMS_ALS_OC)
    {
      pb_send_event(instance,
                    sns_std_sensor_physical_config_event_fields,
                    &phy_sensor_config,
                    sns_get_system_time(),
                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                    &state->als_info.suid[AMS_OC_IDX]);

      if (state->als_info.client_cfg.oc_send_last_event)
      {
        state->als_info.client_cfg.oc_send_last_event = false;
        pb_send_sensor_stream_event(instance,
                                    &state->als_info.suid[AMS_OC_IDX],
                                    sns_get_system_time(),
                                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                    state->als_info.status,
                                    state->als_info.prev_OC_report,
                                    ARR_SIZE(state->als_info.prev_OC_report),
                                    state->encoded_als_event_len);
      }
    }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    if(state->publish_sensors & AMS_RGB_OC)
    {
      pb_send_event(instance,
                   sns_std_sensor_physical_config_event_fields,
                   &phy_sensor_config,
                   sns_get_system_time(),
                   SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                   &state->rgb_info.suid[AMS_OC_IDX]);

      if (state->rgb_info.client_cfg.oc_send_last_event)
      {
        state->rgb_info.client_cfg.oc_send_last_event = false;
        pb_send_sensor_stream_event(instance,
                                    &state->rgb_info.suid[AMS_OC_IDX],
                                    sns_get_system_time(),
                                    SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                    state->rgb_info.status,
                                    state->rgb_info.prev_OC_report,
                                    ARR_SIZE(state->rgb_info.prev_OC_report),
                                    state->encoded_rgb_event_len);
      }
    }
#endif
  }

  if(state->publish_sensors & (AMS_ALS_STRM | AMS_RGB_STRM))
  {
    phy_sensor_config.has_sample_rate = true;
    phy_sensor_config.sample_rate = state->sampling_rate_hz;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;

    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = info.alsSensor.activeCurrent_uA;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = (1 / info.alsSensor.adcBits);
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = info.alsSensor.rangeMin;
    phy_sensor_config.range[1] = info.alsSensor.rangeMax;
    phy_sensor_config.has_stream_is_synchronous = false;
    phy_sensor_config.has_DAE_watermark = false;


    if(state->publish_sensors & AMS_ALS_STRM)
    {
      pb_send_event(instance,
                   sns_std_sensor_physical_config_event_fields,
                   &phy_sensor_config,
                   sns_get_system_time(),
                   SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                    &state->als_info.suid[AMS_STRM_IDX]);
    }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    if(state->publish_sensors & AMS_RGB_STRM)
    {
      pb_send_event(instance,
                   sns_std_sensor_physical_config_event_fields,
                   &phy_sensor_config,
                   sns_get_system_time(),
                   SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                   &state->rgb_info.suid[AMS_STRM_IDX]);
    }
#endif
  }

#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->publish_sensors & AMS_PROX_OC)
  {
    phy_sensor_config.has_sample_rate = false;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = true;

    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.active_current = info.proxSensor.activeCurrent_uA;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = (1 / info.proxSensor.adcBits);
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = info.proxSensor.rangeMin;
    phy_sensor_config.range[1] = info.proxSensor.rangeMax;
    phy_sensor_config.has_stream_is_synchronous = false;
    phy_sensor_config.has_DAE_watermark = false;

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->prox_info.suid[AMS_OC_IDX]);
    if (state->prox_info.client_cfg.oc_send_last_event)
    {
      state->prox_info.client_cfg.oc_send_last_event = false;
      pb_send_event(instance,
                    sns_proximity_event_fields,
                    &state->prox_info.prox_state,
                    sns_get_system_time(),
                    SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
                    &state->prox_info.suid[AMS_OC_IDX]);
    }
  }
  if(state->publish_sensors & AMS_PROX_STRM)
  {
    phy_sensor_config.has_sample_rate = true;
    phy_sensor_config.sample_rate = state->sampling_rate_hz;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;

    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.active_current = info.proxSensor.activeCurrent_uA;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = (1 / info.proxSensor.adcBits);
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = info.proxSensor.rangeMin;
    phy_sensor_config.range[1] = info.proxSensor.rangeMax;
    phy_sensor_config.has_DAE_watermark = false;

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->prox_info.suid[AMS_STRM_IDX]);
  }
#endif
}

/** See sns_sensor_instance_api::set_client_config */
sns_rc ams_inst_set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  float desired_sample_rate = 0.0;
  float desired_report_rate = 0.0;
  float ams_chosen_sample_rate = 0.0;
  sns_rc rv = SNS_RC_SUCCESS;
  sns_std_req *payload = (sns_std_req*)client_request->request;

  SNS_INST_PRINTF(LOW, this
                  , "AMS: ams_inst_set_client_config: enter: inst_this 0x%X, message_id %d"
                  , this, client_request->message_id);

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);

  if(client_request->message_id == SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
  {
    SNS_INST_PRINTF(LOW, this
                    , "AMS: ams_inst_set_client_config: _STD_SENSOR_CONFIG, SR=%d, RR=%d"
                    , (int32_t)payload->desired_sample_rate
                    , (int32_t)payload->desired_report_rate
                    );

    // 1. Extract sample, report rates from client_request.
    // 2. Configure sensor HW.
    // 3. sendRequest() for Timer to start/stop in case of polling using timer_data_stream.
    // 4. sendRequest() for Intrerupt register/de-register in case of DRI using interrupt_data_stream.
    // 5. Save the current config information like type, sample_rate, report_rate, etc.
    desired_sample_rate = payload->desired_sample_rate;
    desired_report_rate = payload->desired_report_rate;

    if(desired_report_rate > desired_sample_rate)
    {
      // bad request. Return error or default report_rate to sample_rate
      desired_report_rate = desired_sample_rate;
    }

    ams_chosen_sample_rate = desired_sample_rate; //we only put this into effect in polling mode
    SNS_INST_PRINTF(LOW, this
                    , "AMS: ams_inst_set_client_config: ams_chosen_sample_rate=%d"
                    , ams_chosen_sample_rate
                    );

    {
      ams_configureFeature_t feature;
      uint16_t EITHER;
      bool     both_off, both_were_off;
      uint16_t newly_on, newly_off;
      bool     option_changed;
      uint32_t option_new_state;
      uint16_t on = state->publish_sensors;
      uint16_t p = state->publish_sensors_prev;

      newly_on = ~p & on;
      newly_off = p & ~on;


#if 0 // _PRINTF makes mistakes on this!
      AMS_PORT_log_Msg_4(AMS_DEBUG
                      , "ams_inst_set_client_config: on=%04x, p=%04x, newly_on=%04x, newly_off=%04x"
                      , on
                      , p
                      , newly_on
                      , newly_off
                      );
#else
      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_set_client_config: on=%04x, p=%04x, newly_on=%04x, newly_off=%04x"
                      , (uint32_t)on
                      , (uint32_t)p
                      , (uint32_t)newly_on
                      , (uint32_t)newly_off
                      );
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
      feature = AMS_CONFIG_ALS_LUX;
      EITHER = AMS_ALS_OC | AMS_ALS_STRM;
      both_were_off = !(p & EITHER);
      both_off = !(on & EITHER);

      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_set_client_config: ALS: EITHER=%04x, both_were_off=%d, both_off=%d"
                      , EITHER
                      , (uint32_t)both_were_off
                      , (uint32_t)both_off
                      );

      option_changed = false;
      if (newly_off & AMS_ALS_STRM)
      {
        option_changed = true;
        option_new_state = 0;
      }
      else if (newly_on & AMS_ALS_STRM)
      {
        option_changed = true;
        option_new_state = 1;
      }
      if(option_changed && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: ALS: CHANGE ALWAYS_READ STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ALWAYS_READ, option_new_state);
        SNS_INST_PRINTF(ERROR, this, "AMS: ERROR: ALS: changing ALWAYS_READ state failed");
      }

      option_changed = false;
      if ((newly_on & EITHER) && both_were_off)
      {
        option_changed = true;
        option_new_state = 1;
      }
      else if((newly_off & EITHER) && both_off)
      {
        option_changed = true;
        option_new_state = 0;
      }
      if (newly_on & AMS_ALS_OC)
      {
        state->als_info.prev_OC_report[0] = NEGATIVE_INFINITY;
        state->als_info.prev_OC_report[1] = NEGATIVE_INFINITY;
        /* if we're not going to _CONFIG_ENABLE it, make the 1st OC report now */
        if(!option_changed)
        {
          ams_generateAlsResponse(state, state->diag_service, sns_get_system_time(), this, AMS_REPORT_ON_CHANGE);
        }
      }
      if(option_changed && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: ALS: CHANGE ENABLE STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ENABLE, option_new_state);
      }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
      feature = AMS_CONFIG_ALS_RGB;
      EITHER = AMS_RGB_OC | AMS_RGB_STRM;
      both_were_off = !(p & EITHER);
      both_off = !(on & EITHER);

      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_set_client_config: RGB: EITHER=%04x, both_were_off=%d, both_off=%d"
                      , (uint32_t)EITHER
                      , (uint32_t)both_were_off
                      , (uint32_t)both_off
                      );

      option_changed = false;
      if (newly_off & AMS_RGB_STRM)
      {
        option_changed = true;
        option_new_state = 0;
      }
      else if (newly_on & AMS_RGB_STRM)
      {
        option_changed = true;
        option_new_state = 1;
      }
      if(option_changed && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: RGB: CHANGE ALWAYS_READ STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ALWAYS_READ, option_new_state);
      }

      option_changed = false;
      if ((newly_on & EITHER) && both_were_off)
      {
        option_changed = true;
        option_new_state = 1;
      }
      else if((newly_off & EITHER) && both_off)
      {
        option_changed = true;
        option_new_state = 0;
      }
      if (newly_on & AMS_RGB_OC)
      {
        state->rgb_info.prev_OC_report[0] = NEGATIVE_INFINITY;
        state->rgb_info.prev_OC_report[1] = NEGATIVE_INFINITY;
        state->rgb_info.prev_OC_report[2] = NEGATIVE_INFINITY;
        state->rgb_info.prev_OC_report[3] = NEGATIVE_INFINITY;
        state->rgb_info.prev_OC_report[4] = NEGATIVE_INFINITY;
        state->rgb_info.prev_OC_report[5] = NEGATIVE_INFINITY;
        /* if we're not going to _CONFIG_ENABLE it, make the 1st OC report now */
        if(!option_changed)
        {
          ams_generateRgbResponse(state, state->diag_service, sns_get_system_time(), this, AMS_REPORT_ON_CHANGE);
        }
      }
      if(option_changed  && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: RGB: CHANGE ENABLE STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ENABLE, option_new_state);
      }
#endif
#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
      feature = AMS_CONFIG_PROX;
      EITHER = AMS_PROX_OC | AMS_PROX_STRM;
      both_were_off = !(p & EITHER);
      both_off = !(on & EITHER);

      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_set_client_config: PROX: EITHER=%04x, both_were_off=%d, both_off=%d"
                      , (uint32_t)EITHER
                      , (uint32_t)both_were_off
                      , (uint32_t)both_off
                      );

      option_changed = false;
      if (newly_off & AMS_PROX_STRM)
      {
        option_changed = true;
        option_new_state = 0;
      }
      else if (newly_on & AMS_PROX_STRM)
      {
        option_changed = true;
        option_new_state = 1;
      }
      if(option_changed && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: PROX: CHANGE ALWAYS_READ STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ALWAYS_READ, option_new_state);
      }

      option_changed = false;
      if ((newly_on & EITHER) && both_were_off)
      {
        option_changed = true;
        option_new_state = 1;
      }
      else if((newly_off & EITHER) && both_off)
      {
        option_changed = true;
        option_new_state = 0;
      }
      if (newly_on & AMS_PROX_OC)
      {
        state->prox_info.prev_OC_report[0] = AMS_PROXIMITY_EVENT_TYPE_INVALID;
        /* if we're not going to _CONFIG_ENABLE it, make the 1st OC report now */
        if(!option_changed)
        {
          log_sensor_state_raw_info log_prox_state_raw_info;
          // Allocate Sensor State Raw log packets
          sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));

          ams_generateProxResponse(state, state->diag_service, sns_get_system_time(),
                                 this, &log_prox_state_raw_info, AMS_REPORT_ON_CHANGE);
        }
      }
      if(option_changed && !state->new_self_test_request)
      {
        SNS_INST_PRINTF(LOW, this
                        , "AMS: PROX: CHANGE ENABLE STATE TO:  %d"
                        , option_new_state
                        );
        ams_deviceSetConfig(state->priv, feature, AMS_CONFIG_ENABLE, option_new_state);
      }
#endif
      {
        uint16_t p_streamers = p & AMS_STREAMERS;
        uint16_t on_streamers = on & AMS_STREAMERS;

        /* Update timer if any change in current/previous streaming sensors */
        if(on_streamers || p_streamers)
        {
          if(ams_validate_device_odr(this, &ams_chosen_sample_rate))
          {
            _ams_reconfig_hw(this);
          }
        }
      }

      {
        uint16_t p_on_changesrs = p & AMS_ON_CHANGERS;
        uint16_t on_changesrs = on & AMS_ON_CHANGERS;
        if (!p_on_changesrs && on_changesrs)
        {
          // setup the irq stream
          _ams_setup_clean_irq_stream (this, true);
        }
        if (p_on_changesrs && !on_changesrs)
        {
          // remove the irq stream
          _ams_setup_clean_irq_stream (this, false);
        }
      }
	  
      if (state->new_or_chng_req)
      {
		/* send cal_event only for a new AMS_CONFIG_ALS_LUX client*/
        if ((((state->publish_sensors & AMS_ALS_OC) ^ AMS_ALS_OC) == 0) 
             ||
            (((state->publish_sensors & AMS_ALS_STRM) ^ AMS_ALS_STRM) == 0))
        {
          float result;
          result = (float)state->calibration_data.alsCalibrationFactor;
          _ams_send_cal_event (this, &state->als_info.suid[(newly_on ^ AMS_ALS_OC)? AMS_STRM_IDX : AMS_OC_IDX],
                               &result, 1, NULL, 0);
        }
		/* send cal_event only for a new AMS_CONFIG_ALS_RGB client*/
        if ((((state->publish_sensors & AMS_RGB_OC) ^ AMS_RGB_OC) == 0) 
             ||
            (((state->publish_sensors & AMS_RGB_STRM) ^ AMS_RGB_STRM) == 0))
        {
          float result[4];
          result[0] = (float)state->calibration_data.alsCoefR;
          result[1] = (float)state->calibration_data.alsCoefG;
          result[2] = (float)state->calibration_data.alsCoefB;
          result[3] = 0;
          _ams_send_cal_event (this, &state->rgb_info.suid[(newly_on ^ AMS_RGB_OC)? AMS_STRM_IDX : AMS_OC_IDX],
                               result, 4, NULL, 0);
        }
		/* send cal_event only for a new AMS_CONFIG_PROX client*/
        if ((((state->publish_sensors & AMS_PROX_OC) ^ AMS_PROX_OC) == 0) 
             ||
            (((state->publish_sensors & AMS_PROX_STRM) ^ AMS_PROX_STRM) == 0))
        {
          float result;
          result = (float)state->calibration_data.proxHardThreshold;
          _ams_send_cal_event (this, &state->prox_info.suid[(newly_on ^ AMS_PROX_OC)? AMS_STRM_IDX : AMS_OC_IDX],
                               &result, 1, NULL, 0);
        }
        state->new_or_chng_req = false;
      }
    }

    state->publish_sensors_prev = state->publish_sensors;
    if(!state->new_self_test_request)
    {
      _ams_send_config_event(this);
    }
  }
  else if(client_request->message_id == SNS_STD_MSGID_SNS_STD_FLUSH_REQ)
  {
    SNS_INST_PRINTF(ERROR, this, "AMS: ams_inst_set_client_config: _STD_FLUSH_REQ");
  }
  else if(client_request->message_id ==
          SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_CONFIG)
  {
    SNS_INST_PRINTF(MED, this
                    , "AMS: ams_inst_set_client_config: _PHYSICAL_SENSOR_TEST_CONFIG"
                    );

     // 1. Extract test type from client_request.
     // 2. Configure sensor HW for test type.
     // 3. send_request() for Timer Sensor in case test needs polling/waits.
    _ams_run_self_test(this);

  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);

  SNS_INST_PRINTF(LOW, this, "AMS: ams_inst_set_client_config: end");

  return rv;
}

sns_rc ams_inst_deinit(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);

  AMS_PORT_log_Msg(AMS_DEBUG, "AMS: ams_inst_deinit");

  inst_cleanup(state, stream_mgr);

  return SNS_RC_SUCCESS;
}

