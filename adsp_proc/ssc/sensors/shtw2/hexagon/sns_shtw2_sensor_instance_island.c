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
#include "sns_std.pb.h"
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

typedef struct log_sensor_state_raw_info
{
  /* Pointer to diag service */
  sns_diag_service *diag;
  /* Pointer to sensor instance */
  sns_sensor_instance *instance;
  /* Pointer to sensor UID */
  struct sns_sensor_uid *sensor_uid;
  /* Size of a single encoded sample */
  size_t encoded_sample_size;
  /* Pointer to log */
  void *log;
  /* Size of allocated space for log */
  uint32_t log_size;
  /* Number of actual bytes written */
  uint32_t bytes_written;
  /* Number of batch samples written */
  /* A batch may be composed of several logs */
  uint32_t batch_sample_cnt;
  /* Number of log samples written */
  uint32_t log_sample_cnt;
} log_sensor_state_raw_info;

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /* Raw sensor state data sample */
  float sample[SHTW2_NUM_AXES];
  /* Data status */
  sns_std_sensor_sample_status status;
} shtw2_batch_sample;

/**
 * Encode Sensor State Log.Interrupt
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc shtw2_encode_sensor_state_log_interrupt(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  sns_diag_sensor_state_interrupt *sensor_state_interrupt =
    (sns_diag_sensor_state_interrupt *)log;
  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_sensor_state_interrupt_fields,
                sensor_state_interrupt))
  {
    rc = SNS_RC_FAILED;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}

/**
 * Encode log sensor state raw packet
 *
 * @param[i] log Pointer to log packet information
 * @param[i] log_size Size of log packet information
 * @param[i] encoded_log_size Maximum permitted encoded size of
 *                            the log
 * @param[o] encoded_log Pointer to location where encoded
 *                       log should be generated
 * @param[o] bytes_written Pointer to actual bytes written
 *       during encode
 *
 * @return sns_rc
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc shtw2_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[SHTW2_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = SHTW2_NUM_AXES,
    .arr_index = &arr_index};

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  batch_sample.sample.funcs.encode = &pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                          &batch_sample))
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
  shtw2_batch_sample *raw_sample = (shtw2_batch_sample *)log;

  while(parsed_log_size < log_size &&
        (stream.bytes_written + encoded_sample_size)<= encoded_log_size &&
        i < (uint32_t)(-1))
  {
    arr_index = 0;
    arg.arr = (float *)raw_sample[i].sample;

    batch_sample.sample_type = raw_sample[i].sample_type;
    batch_sample.status = raw_sample[i].status;
    batch_sample.timestamp = raw_sample[i].timestamp;

    if(!pb_encode_tag(&stream, PB_WT_STRING,
                      sns_diag_sensor_state_raw_sample_tag))
    {
      rc = SNS_RC_FAILED;
      break;
    }
    else if(!pb_encode_delimited(&stream, sns_diag_batch_sample_fields,
                                 &batch_sample))
    {
      rc = SNS_RC_FAILED;
      break;
    }

    parsed_log_size += sizeof(shtw2_batch_sample);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
void shtw2_log_sensor_state_raw_alloc(
  log_sensor_state_raw_info *log_raw_info,
  uint32_t log_size)
{
  uint32_t max_log_size = 0;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid)
  {
    return;
  }

  // allocate memory for sensor state - raw sensor log packet
  max_log_size = log_raw_info->diag->api->get_max_log_size(
       log_raw_info->diag);

  if(0 == log_size)
  {
    // log size not specified.
    // Use max supported log packet size
    log_raw_info->log_size = max_log_size;
  }
  else if(log_size <= max_log_size)
  {
    log_raw_info->log_size = log_size;
  }
  else
  {
    return;
  }

  log_raw_info->log = log_raw_info->diag->api->alloc_log(
    log_raw_info->diag,
    log_raw_info->instance,
    log_raw_info->sensor_uid,
    log_raw_info->log_size,
    SNS_DIAG_SENSOR_STATE_LOG_RAW);

  log_raw_info->log_sample_cnt = 0;
  log_raw_info->bytes_written = 0;
}

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *       pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *       of batch
 *  */
void shtw2_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  shtw2_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (shtw2_batch_sample *)log_raw_info->log;

  if(batch_complete)
  {
    // overwriting previously sample_type for last sample
    if(1 == log_raw_info->batch_sample_cnt)
    {
      sample[0].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_ONLY;
    }
    else if(1 < log_raw_info->batch_sample_cnt)
    {
      sample[log_raw_info->log_sample_cnt - 1].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_LAST;
    }
  }

  log_raw_info->diag->api->submit_log(
        log_raw_info->diag,
        log_raw_info->instance,
        log_raw_info->sensor_uid,
        log_raw_info->bytes_written,
        log_raw_info->log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
        shtw2_encode_log_sensor_state_raw);

  log_raw_info->log = NULL;
}

/**
 *
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *                        pertaining to the sensor
 * @param[i] raw_data     Uncalibrated sensor data to be logged
 * @param[i] timestamp    Timestamp of the sensor data
 * @param[i] status       Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was succesful
 * SNS_RC_FAILED otherwise
 */
sns_rc shtw2_log_sensor_state_raw_add(
  log_sensor_state_raw_info *log_raw_info,
  float *raw_data,
  sns_time timestamp,
  sns_std_sensor_sample_status status)
{
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == raw_data || NULL == log_raw_info->log)
  {
    return SNS_RC_FAILED;
  }

  if( (log_raw_info->bytes_written + sizeof(shtw2_batch_sample)) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    shtw2_log_sensor_state_raw_submit(log_raw_info, false);
    shtw2_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    shtw2_batch_sample *sample =
        (shtw2_batch_sample *)log_raw_info->log;

    if(0 == log_raw_info->batch_sample_cnt)
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
    }
    else
    {
      sample[log_raw_info->log_sample_cnt].sample_type =
        SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
    }

    sample[log_raw_info->log_sample_cnt].timestamp = timestamp;

    sns_memscpy(sample[log_raw_info->log_sample_cnt].sample,
                sizeof(sample[log_raw_info->log_sample_cnt].sample),
                raw_data,
                sizeof(sample[log_raw_info->log_sample_cnt].sample));

    sample[log_raw_info->log_sample_cnt].status = status;

    log_raw_info->bytes_written += sizeof(shtw2_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }

  return rc;
}

static void shtw2_log_sensor_data(sns_diag_service *diag,
                                  sns_sensor_instance *const instance,
                                  shtw2_instance_state *state,
                                  struct sns_sensor_uid *sensor_uid,
                                  float *raw_data,
                                  sns_time timestamp,
                                  sns_std_sensor_sample_status status)
{
  log_sensor_state_raw_info raw_info;

  // Allocate Sensor State Raw log packets for accel and gyro
  sns_memzero(&raw_info, sizeof(raw_info));
  raw_info.encoded_sample_size = state->log_raw_encoded_size;
  raw_info.diag = diag;
  raw_info.instance = instance;
  raw_info.sensor_uid = sensor_uid;
  shtw2_log_sensor_state_raw_alloc(&raw_info, 0);

  shtw2_log_sensor_state_raw_add(
      &raw_info,
      raw_data,
      timestamp,
      status);

  shtw2_log_sensor_state_raw_submit(&raw_info, true);
}

sns_rc shtw2_measurement_timer_set(sns_time timeout_ticks,
                                   sns_data_stream *timer_data_stream,
                                   bool is_periodic)
{
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[SHTW2_REQUEST_PAYLOAD_SIZE];
  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = is_periodic;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                              sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
         .request = buffer, .request_len = req_len };
    timer_data_stream->api->send_request(timer_data_stream,
                                         &timer_req);
  }

  return SNS_RC_SUCCESS;
}

sns_rc shtw2_init_measurement(sns_sensor_instance *const this)
{
  shtw2_instance_state *state = (shtw2_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr =
      (sns_stream_service*)service_mgr->get_service(service_mgr,
                                                    SNS_STREAM_SERVICE);
  sns_diag_service* diag = state->diag_service;
  sns_time timeout_ticks;
  sns_rc rv;

  /* Ignore request if a measurement is already ongoing. */
  if(state->timer_state == MEASURING)
    return SNS_RC_SUCCESS;

  rv = shtw2_start_measurement(state->scp_service,
                               state->com_port_info.port_handle);

  if(rv != SNS_RC_SUCCESS)
  {
    SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                           "Failed to start SHTW2 measurement");
    return rv;
  }

  if(state->one_shot_timer_data_stream == NULL)
  {
    rv = stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                        this,
                                                        state->timer_suid,
                                                        &state->one_shot_timer_data_stream);
    if(rv != SNS_RC_SUCCESS)
    {
      SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                             "Failed to create one-shot timer stream");
      return rv;
    }
  }

  timeout_ticks = sns_convert_ns_to_ticks(SHTW2_MEAS_DURATION_US * 1000);
  rv = shtw2_measurement_timer_set(timeout_ticks,
                                   state->one_shot_timer_data_stream, false);
  if(rv != SNS_RC_SUCCESS)
  {
    SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                           "Failed to start SHTW2 measurement timer");
    state->timer_state = IDLE;
    return rv;
  }
  state->timer_state = MEASURING;

  return rv;
}

void shtw2_stop_all_timers(sns_sensor_instance *const this)
{
  shtw2_instance_state *state =
      (shtw2_instance_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_mgr = (sns_stream_service*)
      service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  if(NULL != state->periodic_timer_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->periodic_timer_data_stream);
    state->periodic_timer_data_stream = NULL;
  }
  if(NULL != state->one_shot_timer_data_stream)
  {
    stream_mgr->api->remove_stream(stream_mgr, state->one_shot_timer_data_stream);
    state->one_shot_timer_data_stream = NULL;
  }
}

/**
 * Notification to the client that some data has been received.
 *
 * The client must use the sns_manager_event to obtain this data
 * for processing.
 *
 * @return
 * SNS_RC_INVALID_STATE - A client error occurred; Framework shall destroy client
 * SNS_RC_NOT_AVAILABLE - A transitory error occurred; Framework shall remove all outstanding input
 * SNS_RC_SUCCESS
 *
 * See sns_sensor_instance.h.
 */
static sns_rc shtw2_inst_notify_event(sns_sensor_instance *const this)
{
  shtw2_instance_state *state =
    (shtw2_instance_state*)this->state->state;
  sns_diag_service* diag = state->diag_service;
  sns_sensor_event *event;
  sns_time timestamp;
  sns_rc rv;
  float data[1] = {0};

  /* Turn COM-port on. */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    true);

  if(NULL != state->one_shot_timer_data_stream)
  {
    event = state->one_shot_timer_data_stream->api->peek_input(state->one_shot_timer_data_stream);

    if(NULL!=event)
      {
        if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
        {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t *)event->event,
                                                     event->event_len);
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {        
          if((state->temperature_info.enabled || state->humidity_info.enabled)
             && state->current_odr > SHTW2_ODR_0)
          {
            SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                   "Process one-shot timer measurement event for the SHTW2, timer state is %d",
                                   state->timer_state);

            switch(state->timer_state)
            {
              case MEASURING:
                rv = shtw2_read_measurement(state->scp_service,
                                            state->com_port_info.port_handle,
                                            state->temperature_registry_cfg,
                                            state->humidity_registry_cfg,
                                            &state->temperature_info,
                                            &state->humidity_info);
                if(rv == SNS_RC_SUCCESS)
                {
                  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                         "SHTW2 temperature: %d milli-degC",
                                         (int)(state->temperature_info.latest_reading * 1000));
                  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                         "bias: %d, scale: %d, threshold: %d milli-degC",
                                         (int)(state->temperature_registry_cfg.fac_cal_bias[0]),
                                         (int)(state->temperature_registry_cfg.fac_cal_scale[0]),
                                         (int)(state->temperature_registry_cfg.threshold * 1000));
                  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                         "SHTW2 humidity: %d milli-RH",
                                         (int)(state->humidity_info.latest_reading * 1000));
                  SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                         "bias: %d, scale: %d, threshold: %d milli-RH",
                                         (int)(state->humidity_registry_cfg.fac_cal_bias[0]),
                                         (int)(state->humidity_registry_cfg.fac_cal_scale[0]),
                                         (int)(state->humidity_registry_cfg.threshold * 1000));

                  timestamp = sns_get_system_time();
                  if(state->temperature_info.enabled &&
                     (state->temperature_info.report_sample
                      || state->temperature_info.force_first_sample))
                  {
                    data[0] = state->temperature_info.latest_reading;
                    pb_send_sensor_stream_event(this,
                                                &state->temperature_info.suid,
                                                timestamp,
                                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                                SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
                                                data,
                                                ARR_SIZE(data),
                                                state->encoded_sensor_event_len);

                    shtw2_log_sensor_data(diag,
                                          this,
                                          state,
                                          &state->temperature_info.suid,
                                          data,
                                          timestamp,
                                          SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH);

                    if(state->temperature_info.force_first_sample)
                    {
                      state->temperature_info.force_first_sample = false;
                    }
                  }
                  if(state->humidity_info.enabled &&
                     (state->humidity_info.report_sample
                      || state->humidity_info.force_first_sample))
                  {
                    data[0] = state->humidity_info.latest_reading;
                    pb_send_sensor_stream_event(this,
                                                &state->humidity_info.suid,
                                                timestamp,
                                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                                SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
                                                data,
                                                ARR_SIZE(data),
                                                state->encoded_sensor_event_len);

                    shtw2_log_sensor_data(diag,
                                          this,
                                          state,
                                          &state->humidity_info.suid,
                                          data,
                                          timestamp,
                                          SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH);

                    if(state->humidity_info.force_first_sample)
                    {
                      state->humidity_info.force_first_sample = false;
                    }
                  }
                }
                else
                {
                  SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                         "Failed to read SHTW2 measurement");
                }

                if(state->current_odr > SHTW2_ODR_0)
                {
                  state->timer_state = WAITING;
                }
                else
                {
                  SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                                         "Stop periodic measurement timer");
                  shtw2_stop_all_timers(this);
                  state->timer_state = IDLE;
                }
                break;
              case WAITING:
              case IDLE:
              default:
                SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                       "Erroneous one-shot timer state %d",
                                       state->timer_state);
              break;
            }
          }
          else
          {
            SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                                   "Received one-shot timer measurement event but neither "
                                   "temperature nor humidity is enabled or ODR is 0");
          }
        }
        else
        {
          SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                 "Measurement request one-shot timer with unknown message ID");
        }
      }
      else
      {
        SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                               "Measurement request one-shot timer decode failed");
      }
    }

    /* We should never have more than one timer event. */
    while(NULL != state->one_shot_timer_data_stream->api->get_next_input(state->one_shot_timer_data_stream))
    {
      SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                             "More than one one-shot timer data stream event, ignoring it");
    }
  }

  if(NULL != state->periodic_timer_data_stream)
  {
    event = state->periodic_timer_data_stream->api->peek_input(state->periodic_timer_data_stream);

    if(NULL!=event) 
	{
      if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
      {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                   event->event_len);
      sns_timer_sensor_event timer_event;
      if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {        
          if((state->temperature_info.enabled || state->humidity_info.enabled)
             && state->current_odr > SHTW2_ODR_0)
          {
            SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                                   "Process periodic timer measurement event for the SHTW2, timer state is %d",
                                   state->timer_state);

            switch(state->timer_state)
            {
              case WAITING:
                rv = shtw2_init_measurement(this);
                if(rv != SNS_RC_SUCCESS)
                {
                  SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                         "Failed to init SHTW2 measurement");
                  return rv;
                }
                break;
              case MEASURING:
              case IDLE:
              default:
                SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                       "Erroneous periodic measurement timer state %d",
                                       state->timer_state);
              break;
            }
          }
          else
          {
            SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                                   "Received periodic timer measurement event but neither "
                                   "temperature nor humidity is enabled or ODR is 0");
          }
        }
        else
        {
          SHT_SENSOR_INST_PRINTF(diag, this, state, ERROR, __FILENAME__, __LINE__,
                                 "Measurement request periodic timer with unknown message ID");
        }
      }
      else
      {
        SHT_SENSOR_INST_PRINTF(diag, this, state, HIGH, __FILENAME__, __LINE__,
                               "Measurement request periodic timer decode failed");
      }
    }

    /* We should never have more than one timer event. */
    while(NULL != state->periodic_timer_data_stream->api->get_next_input(state->periodic_timer_data_stream))
    {
      SHT_SENSOR_INST_PRINTF(diag, this, state, MED, __FILENAME__, __LINE__,
                             "More than one SHTW2 timer data stream event, ignoring it");
    }
  }

  /* Turn COM-port off. */
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                    false);

  return SNS_RC_SUCCESS;
}

/** Public Data Definitions. */
sns_sensor_instance_api shtw2_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &shtw2_inst_init,
  .deinit            = &shtw2_inst_deinit,
  .set_client_config = &shtw2_inst_set_client_config,
  .notify_event      = &shtw2_inst_notify_event
};
