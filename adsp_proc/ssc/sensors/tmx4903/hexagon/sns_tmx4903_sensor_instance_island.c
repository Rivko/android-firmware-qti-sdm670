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

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[ALS_PROTO_STD_ARRAY_LEN];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} als_batch_sample;

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[RGB_PROTO_STD_ARRAY_LEN];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} rgb_batch_sample;

/** See sns_sensor_instance_api::set_client_config */
sns_rc ams_inst_set_client_config(sns_sensor_instance *const this,
    sns_request const *client_request);

static void _ams_wrap_up_factory_test(sns_sensor_instance *instance,
                                         bool *test_result);
static void _ams_check_and_remove_stream(sns_sensor_instance *this,
    ams_mode_index oc_strm, sns_data_stream *stream);

static void ams_als_send_config_update(sns_sensor_instance *const this, ams_mode_index mode)
{
  ams_instance_state *state =
     (ams_instance_state*)this->state->state;
  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  // TODO: Use appropriate op_mode selected by driver.
  char operating_mode[] = "NORMAL";
  pb_buffer_arg op_mode_args;

  SNS_INST_PRINTF(LOW, this
                  , "AMS: ams_als_send_config_update: enter: inst_this %p, .publish_sensors=0x%04x"
                  , this
                  , state->publish_sensors
                  );
  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  ams_deviceInfo_t info;
  ams_getDeviceInfo(&info);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  SNS_INST_PRINTF(LOW, this
              , "AMS: ams_als_send_config_update: new mMaxLux = %d"
              , devCtx->ccbAlsCtx.ctxAlgAls.results.mMaxLux
              );
  if(((state->publish_sensors & AMS_ALS_STRM) ^ AMS_ALS_STRM) == 0)
  {
    phy_sensor_config.has_sample_rate = true; 
    phy_sensor_config.sample_rate = state->sampling_rate_hz;
  }
  else {
    phy_sensor_config.has_sample_rate = false;
  }
  phy_sensor_config.has_water_mark = false;
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = info.alsSensor.activeCurrent_uA;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.resolution = (1 / devCtx->ccbAlsCtx.ctxAlgAls.saturation);
  phy_sensor_config.range_count = 2;
  phy_sensor_config.range[0] = 0;
  phy_sensor_config.range[1] = devCtx->ccbAlsCtx.ctxAlgAls.results.mMaxLux;
  phy_sensor_config.has_stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = true;
  phy_sensor_config.dri_enabled = (mode==AMS_STRM_IDX ? false : true);
  phy_sensor_config.has_DAE_watermark = false;

  pb_send_event(this,
                sns_std_sensor_physical_config_event_fields,
                &phy_sensor_config,
                sns_get_system_time(),
                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                &state->als_info.suid[mode]);
#endif
}

/**
 * Allocate Sensor State Raw Log Packet
 *
 * @param[i] diag       Pointer to diag service
 * @param[i] log_size   Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
static void _ams_log_sensor_state_raw_alloc(
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

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
/**
 * Encode als log sensor state raw packet
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
sns_rc ams_encode_als_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[ALS_PROTO_STD_ARRAY_LEN];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = ALS_PROTO_STD_ARRAY_LEN,
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
  als_batch_sample *raw_sample = (als_batch_sample *)log;

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

    parsed_log_size += sizeof(als_batch_sample);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
/**
 * Encode rgb log sensor state raw packet
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
sns_rc ams_encode_rgb_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[RGB_PROTO_STD_ARRAY_LEN];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = RGB_PROTO_STD_ARRAY_LEN,
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
  rgb_batch_sample *raw_sample = (rgb_batch_sample *)log;

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

    parsed_log_size += sizeof(rgb_batch_sample);
    i++;
  }

  if (SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }

  return rc;
}
#endif

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
static sns_rc _ams_log_sensor_state_raw_add(
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

  if(0)
  { ; }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if(log_raw_info->sensor_type == AMS_ALS_OC
     ||
     log_raw_info->sensor_type == AMS_ALS_STRM)
  {
    als_batch_sample *sample =
        (als_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(als_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  else if(log_raw_info->sensor_type == AMS_RGB_OC
          ||
          log_raw_info->sensor_type == AMS_RGB_STRM)
  {
    rgb_batch_sample *sample =
        (rgb_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(rgb_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }
#endif

  return rc;
}

/**
 * Submit the Sensor State Raw Log Packet
 *
 * @param[i] log_raw_info   Pointer to logging information
 *       pertaining to the sensor
 * @param[i] batch_complete true if submit request is for end
 *       of batch
 *  */
static void _ams_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  sns_diag_encode_log_cb cb;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  if(0)
  { ; }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  else if(log_raw_info->sensor_type == AMS_ALS_OC
     ||
     log_raw_info->sensor_type == AMS_ALS_STRM)
  {
    als_batch_sample *sample = (als_batch_sample *)log_raw_info->log;

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
    cb = ams_encode_als_log_sensor_state_raw;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  else if(log_raw_info->sensor_type == AMS_RGB_OC ||
          log_raw_info->sensor_type == AMS_RGB_STRM)
  {
    rgb_batch_sample *sample = (rgb_batch_sample *)log_raw_info->log;

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
    cb = ams_encode_rgb_log_sensor_state_raw;
  }
#endif
  else
  {
    return;
  }

  log_raw_info->diag->api->submit_log(
        log_raw_info->diag,
        log_raw_info->instance,
        log_raw_info->sensor_uid,
        log_raw_info->bytes_written,
        log_raw_info->log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
        cb);

  log_raw_info->log = NULL;
}

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
                             ams_reportType_t report_type)
{
  UNUSED_VAR(diag);
  ams_apiAls_t outData;
  float data[ALS_PROTO_STD_ARRAY_LEN];
  sns_std_sensor_sample_status status;

  log_sensor_state_raw_info log_als_state_raw_info;

  sns_memzero(&log_als_state_raw_info, sizeof(log_als_state_raw_info));
  log_als_state_raw_info.encoded_sample_size = state->als_enc_raw_log_size;
  log_als_state_raw_info.diag = diag;
  log_als_state_raw_info.instance = instance;

  ams_deviceGetAls(state->priv, &outData);
  if((outData.quality & ALS_QUALITY_LUX_MASK) == ALS_QUALITY_LUX_GOOD)
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  }
  else
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
  }
  state->als_info.status = status;
  data[0] = (float)(outData.mLux)/1000.0;
  data[1] = outData.rawClear;

  SNS_INST_PRINTF(LOW, instance
                 , "AMS: ams_generateAlsResponse: mLux %d, rawClear %d, quality %d"
                 , 1000 * (int32_t)data[0], (int32_t)data[1]
                 , status
                 );

  if((state->publish_sensors & AMS_ALS_OC) &&
     (report_type & AMS_REPORT_ON_CHANGE))
  {
    if (data[0] != state->als_info.prev_OC_report[0])
    {
      pb_send_sensor_stream_event(instance,
                                 &state->als_info.suid[AMS_OC_IDX],
                                 timestamp,
                                 SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                 status,
                                 data,
                                 ARR_SIZE(data),
                                 state->encoded_als_event_len);

      log_als_state_raw_info.sensor_uid = &state->als_info.suid[AMS_OC_IDX];
      log_als_state_raw_info.sensor_type = AMS_ALS_OC;
      _ams_log_sensor_state_raw_alloc(&log_als_state_raw_info, sizeof(als_batch_sample));
      _ams_log_sensor_state_raw_add(
          &log_als_state_raw_info,
          data,
          timestamp,
          status);
      _ams_log_sensor_state_raw_submit(&log_als_state_raw_info, true);
      if (status == SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH) {
        state->als_info.prev_OC_report[0] = data[0];
        state->als_info.prev_OC_report[1] = data[1];
      }
    }
  }

  if((state->publish_sensors & AMS_ALS_STRM) &&
     (report_type & AMS_REPORT_STREAMING))
  {
    pb_send_sensor_stream_event(instance,
                               &state->als_info.suid[AMS_STRM_IDX],
                               timestamp,
                               SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                               status,
                               data,
                               ARR_SIZE(data),
                               state->encoded_als_event_len);

    log_als_state_raw_info.sensor_uid = &state->als_info.suid[AMS_STRM_IDX];
    log_als_state_raw_info.sensor_type = AMS_ALS_STRM;
    _ams_log_sensor_state_raw_alloc(&log_als_state_raw_info, sizeof(als_batch_sample));
    _ams_log_sensor_state_raw_add(
        &log_als_state_raw_info,
        data,
        timestamp,
        status);
    _ams_log_sensor_state_raw_submit(&log_als_state_raw_info, true);
  }
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB

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
                             ams_reportType_t report_type)
{
  UNUSED_VAR(diag);
  ams_apiAls_t outData;
  float data[RGB_PROTO_STD_ARRAY_LEN];
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  log_sensor_state_raw_info log_rgb_state_raw_info;

  sns_memzero(&log_rgb_state_raw_info, sizeof(log_rgb_state_raw_info));
  log_rgb_state_raw_info.encoded_sample_size = state->rgb_enc_raw_log_size;
  log_rgb_state_raw_info.diag = diag;
  log_rgb_state_raw_info.instance = instance;

  ams_deviceGetAls(state->priv, &outData);
  if((outData.quality & ALS_QUALITY_CCT_MASK) == ALS_QUALITY_CCT_GOOD)
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  }
  else
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
  }
  state->rgb_info.status = status;
  data[0] = (float)outData.red;
  data[1] = (float)outData.green;
  data[2] = (float)outData.blue;
  data[3] = (float)outData.clear;
  data[4] = (float)outData.colorTemp;
  data[5] = (float)outData.rawClear;

#if 1 //DBG
  AMS_PORT_log_Msg_5(AMS_DEBUG
                  , "AMS: ams_generateRgbResponse: quality %d; data (milliunits): R %d, G %d, B %d, C %d"
                  , status
                  , 1000 * (int32_t)data[0]
                  , 1000 * (int32_t)data[1]
                  , 1000 * (int32_t)data[2]
                  , 1000 * (int32_t)data[3]
                  );
  AMS_PORT_log_Msg_2(AMS_DEBUG
                  , "AMS: ams_generateRgbResponse: CT %d, rawClear %d"
                  , (int32_t)data[4]
                  , (int32_t)data[5]
                  );
#else //DBG
  SNS_INST_PRINTF(LOW, instance
                  , "AMS: ams_generateRgbResponse: quality %d; data (milliunits): R %d, G %d, B %d, C %d"
                  , status
                  , 1000 * (int32_t)data[0]
                  , 1000 * (int32_t)data[1]
                  , 1000 * (int32_t)data[2]
                  , 1000 * (int32_t)data[3]
                  );
  SNS_INST_PRINTF(LOW, instance
                  , "AMS: ams_generateRgbResponse: CT %d, rawClear %d"
                  , (int32_t)data[4]
                  , (int32_t)data[5]
                  );
#endif //DBG

 if((state->publish_sensors & AMS_RGB_OC) &&
     (report_type & AMS_REPORT_ON_CHANGE))
  {
    if (data[0] != state->rgb_info.prev_OC_report[0])
    {
      pb_send_sensor_stream_event(instance,
                                 &state->rgb_info.suid[AMS_OC_IDX],
                                 timestamp,
                                 SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                 status,
                                 data,
                                 ARR_SIZE(data),
                                 state->encoded_rgb_event_len);

      log_rgb_state_raw_info.sensor_uid = &state->rgb_info.suid[AMS_OC_IDX];
      log_rgb_state_raw_info.sensor_type = AMS_RGB_OC;
      _ams_log_sensor_state_raw_alloc(&log_rgb_state_raw_info, sizeof(rgb_batch_sample));
      _ams_log_sensor_state_raw_add(
         &log_rgb_state_raw_info,
         data,
         timestamp,
         status);
      _ams_log_sensor_state_raw_submit(&log_rgb_state_raw_info, true);
      state->rgb_info.prev_OC_report[0] = data[0];
      state->rgb_info.prev_OC_report[1] = data[1];
      state->rgb_info.prev_OC_report[2] = data[2];
      state->rgb_info.prev_OC_report[3] = data[3];
      state->rgb_info.prev_OC_report[4] = data[4];
      state->rgb_info.prev_OC_report[5] = data[5];
    }
  }

  if((state->publish_sensors & AMS_RGB_STRM) &&
     (report_type & AMS_REPORT_STREAMING))
  {
    pb_send_sensor_stream_event(instance,
                               &state->rgb_info.suid[AMS_STRM_IDX],
                               timestamp,
                               SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                               status,
                               data,
                               ARR_SIZE(data),
                               state->encoded_rgb_event_len);

    log_rgb_state_raw_info.sensor_uid = &state->rgb_info.suid[AMS_STRM_IDX];
    log_rgb_state_raw_info.sensor_type = AMS_RGB_STRM;
    _ams_log_sensor_state_raw_alloc(&log_rgb_state_raw_info, sizeof(rgb_batch_sample));
    _ams_log_sensor_state_raw_add(
       &log_rgb_state_raw_info,
       data,
       timestamp,
       status);
    _ams_log_sensor_state_raw_submit(&log_rgb_state_raw_info, true);
  }

}
#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
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
                              ams_reportType_t report_type)
{
  UNUSED_VAR(diag);
  UNUSED_VAR(log_prox_state_raw_info);
  ams_apiPrx_t outData;
  ams_proximity_event_type ams_newNearBy;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  ams_deviceGetPrx(state->priv, &outData);

  state->prox_info.prox_state.raw_adc = outData.proximity;
  state->prox_info.prox_state.status = status;
  /* The event type code conveys the binary nearBy status */
  state->prox_info.prox_state.proximity_event_type =
         (outData.nearBy ? SNS_PROXIMITY_EVENT_TYPE_NEAR : SNS_PROXIMITY_EVENT_TYPE_FAR);
  ams_newNearBy = outData.nearBy ? AMS_PROXIMITY_EVENT_TYPE_NEAR : AMS_PROXIMITY_EVENT_TYPE_FAR;

#if 1 //DBG
  AMS_PORT_log_Msg_4(AMS_DEBUG
                  , "AMS: ams_generateProxResponse: event_type %d, publ=%04x, rept_typ=%02x;  proximity %u"
                  , state->prox_info.prox_state.proximity_event_type
                  , state->publish_sensors
                  , report_type
                  , state->prox_info.prox_state.raw_adc
                  );
#else
  SNS_INST_PRINTF(LOW, instance
                  , "AMS: ams_generateProxResponse: event_type %d, proximity %u"
                  , state->prox_info.prox_state.proximity_event_type
                  , state->prox_info.prox_state.raw_adc
                  );
#endif //DBG

  if((state->publish_sensors & AMS_PROX_OC) &&
     ((report_type == AMS_REPORT_ON_CHANGE) || (report_type == AMS_REPORT_OC_AND_STRM)))
  {
    if (ams_newNearBy != state->prox_info.prev_OC_report[0])
    {
      pb_send_event(instance,
                    sns_proximity_event_fields,
                    &state->prox_info.prox_state,
                    timestamp,
                    SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
                    &state->prox_info.suid[AMS_OC_IDX]);
      state->prox_info.prev_OC_report[0] = ams_newNearBy;
    }
  }
  if((state->publish_sensors & AMS_PROX_STRM) &&
     ((report_type == AMS_REPORT_STREAMING) || (report_type == AMS_REPORT_OC_AND_STRM)))
  {
    pb_send_event(instance,
                  sns_proximity_event_fields,
                  &state->prox_info.prox_state,
                  timestamp,
                  SNS_PROXIMITY_MSGID_SNS_PROXIMITY_EVENT,
                  &state->prox_info.suid[AMS_STRM_IDX]);
  }
}
#endif //CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB

/** See sns_sensor_instance_api::notify_event */
static sns_rc ams_inst_notify_event(sns_sensor_instance *const this)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  sns_interrupt_event irq_event = sns_interrupt_event_init_zero;
  sns_sensor_event *event;
  sns_diag_service* diag = state->diag_service;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  int32_t updateAvailableFlags;
  bool handlerIsReady = (devCtx && devCtx->portHndl && devCtx->portHndl->scp_handle);

  SNS_INST_PRINTF(LOW, this
                  , "AMS: ams_inst_notify_event: enter, data_streams: int %p, timer %p"
                  , state->interrupt_data_stream
                  , state->timer_data_stream
                  );

  // Turn COM port ON
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           true);

  // Handle interrupts
  updateAvailableFlags = 0;
  if(NULL != state->interrupt_data_stream)
  {
    event = state->interrupt_data_stream->api->peek_input(state->interrupt_data_stream);
    while(NULL != event)
    {
#if 1
      AMS_PORT_log_Msg_1(AMS_DEBUG
                      , "ams_inst_notify_event: next interrupt event:  id=%d"
                      , event->message_id
                      );
#else
      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_notify_event: next interrupt event:  id=%d"
                      , event->message_id
                      );
#endif
      if (event->message_id == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT)
      {
        if (state->priv)
        {
          SNS_INST_PRINTF(LOW, this, "AMS: Received INT registration event");
          ams_deviceSetConfig(state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 1);
        }
      }
      else
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
        if(state->publish_sensors & AMS_ON_CHANGERS)
        {
          SNS_INST_PRINTF(LOW, this, "AMS: Received valid INT event");
          if(pb_decode(&stream, sns_interrupt_event_fields, &irq_event))
          {
            state->interrupt_timestamp = irq_event.timestamp;
            if (handlerIsReady){
              ams_deviceEventHandler(devCtx);
              updateAvailableFlags = ams_getResult(state->priv);
              if ((updateAvailableFlags == AMS_CALIBRATION_DONE)
                   || (updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED))
              {
                bool test_result = false;

                if (updateAvailableFlags == AMS_CALIBRATION_DONE){
                  test_result = true;
                }
                _ams_wrap_up_factory_test(this, &test_result);
              }
              else if(!state->new_self_test_request)
              {
                /* no regular reports if selftesting (including if CAL just completed) */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
                if (updateAvailableFlags & (1 << AMS_AMBIENT_SENSOR)){
                  SNS_INST_PRINTF(LOW, this, "AMS: updateAvailable => AMS_AMBIENT_SENSOR");
                  if(state->publish_sensors & AMS_ALS_OC){
                    ams_generateAlsResponse(state, diag, state->interrupt_timestamp, this, AMS_REPORT_ON_CHANGE);
                    if (updateAvailableFlags & AMS_ALS_RGB_GAIN_CHANGED) {
                      ams_als_send_config_update(this, AMS_OC_IDX);
                    }
                  }
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
                  if (state->publish_sensors & AMS_RGB_OC){
                    ams_generateRgbResponse(state, diag, state->interrupt_timestamp, this, AMS_REPORT_ON_CHANGE);
                  }
#endif
                } //AMS_AMBIENT_SENSOR update available
#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
                if (updateAvailableFlags & (1 << AMS_PROXIMITY_SENSOR)){
                  SNS_INST_PRINTF(LOW, this, "AMS: updateAvailable => AMS_PROXIMITY_SENSOR");
                  if(state->publish_sensors & AMS_PROX_OC){
                    log_sensor_state_raw_info log_prox_state_raw_info;

                    // Allocate Sensor State Raw log packets
                    sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));

                    ams_generateProxResponse(state, diag, state->interrupt_timestamp,
                                           this, &log_prox_state_raw_info, AMS_REPORT_ON_CHANGE);
                  }
                }
#endif
              }
            }
          }
          else
          {
            SNS_INST_PRINTF(ERROR, this, "AMS: pb_decode error");
          }
        }
      }
      event = state->interrupt_data_stream->api->get_next_input(state->interrupt_data_stream);
    }
    if ((updateAvailableFlags == AMS_CALIBRATION_DONE)
        ||
        (updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED))
    {
      _ams_check_and_remove_stream(this, AMS_OC_IDX, state->interrupt_data_stream);
    }

    SNS_INST_PRINTF(LOW, this, "AMS: ams_inst_notify_event: no more interrupt events");
  }

  // Handle Timer events
  updateAvailableFlags = 0;
  if(NULL != state->timer_data_stream)
  {
    event = state->timer_data_stream->api->peek_input(state->timer_data_stream);
    while(NULL != event)
    {
      pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
      sns_timer_sensor_event timer_event;

      SNS_INST_PRINTF(LOW, this
                      , "AMS: ams_inst_notify_event: next timer event:  id=%d; timer_is_active=%d"
                      , event->message_id
                      , state->timer_is_active
                      );

      if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
      {
        if((event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
            &&
            state->timer_is_active
            &&
            state->sampling_intvl > 0)
        {
          SNS_INST_PRINTF(LOW, this, "AMS: ams_inst_notify_event: poll timer: publish_sensors 0x%04x"
                                       , state->publish_sensors
                                       );
          if (handlerIsReady)
          {
            ams_deviceEventHandler(devCtx);
            updateAvailableFlags = ams_getResult(state->priv);
            SNS_INST_PRINTF(LOW, this, "AMS: ams_inst_notify_event: updateAvailableFlags=%08x", updateAvailableFlags);
            if ((updateAvailableFlags == AMS_CALIBRATION_DONE)
                 || (updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED))
            {
              bool test_result = false;

              if (updateAvailableFlags == AMS_CALIBRATION_DONE){
                test_result = true;
              }
              _ams_wrap_up_factory_test(this, &test_result);
            }
            else if(!state->new_self_test_request)
            {
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
              if(state->publish_sensors & AMS_ALS_STRM)
              {
                ams_generateAlsResponse(state, diag, timer_event.timeout_time, this, AMS_REPORT_STREAMING);
                if (updateAvailableFlags & AMS_ALS_RGB_GAIN_CHANGED) {
                  ams_als_send_config_update(this, AMS_STRM_IDX);
                }
              }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
              if(state->publish_sensors & AMS_RGB_STRM)
              {
                ams_generateRgbResponse(state, diag, timer_event.timeout_time, this, AMS_REPORT_STREAMING);
              }
#endif
#endif //CONFIG_AMS_OPTICAL_SENSOR_ALS

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
              if(state->publish_sensors & AMS_PROX_STRM)
              {
                log_sensor_state_raw_info log_prox_state_raw_info;
                sns_memzero(&log_prox_state_raw_info, sizeof(log_prox_state_raw_info));

                ams_generateProxResponse(state, diag, timer_event.timeout_time,
                                                       this, &log_prox_state_raw_info, AMS_REPORT_STREAMING);
              }
#endif
            }
          }
        }
        else
        {
          AMS_PORT_log_Msg_3(AMS_DEBUG
                           , "ams_inst_notify_event: ignoring timer evt; msgid=%d, timer_is_active=%d, S.I. > 0? _%d_"
                           , event->message_id
                           , state->timer_is_active
                           , (state->sampling_intvl > 0)
                           );
        }
      }
      else
      {
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_inst_notify_event: ERROR decoding timer_data_stream event");
      }
      event = state->timer_data_stream->api->get_next_input(state->timer_data_stream);
    }
    if ((updateAvailableFlags == AMS_CALIBRATION_DONE)
        ||
        (updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED))
    {
      _ams_check_and_remove_stream(this, AMS_STRM_IDX, state->timer_data_stream);
    }
    SNS_INST_PRINTF(LOW, this, "AMS: ams_inst_notify_event: no more timer_data_stream events");
  }

  // Turn COM port OFF
  state->scp_service->api->sns_scp_update_bus_power(state->com_port_info.port_handle,
                                                                           false);

  return SNS_RC_SUCCESS;
}

static void _ams_check_and_remove_stream(sns_sensor_instance *this,
    ams_mode_index oc_strm, sns_data_stream *stream)
{
  ams_instance_state *state;
  bool test_client_present = false;

  if (NULL == stream) {
    return;
  }
  state = (ams_instance_state *) this->state->state;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if (state->als_info.test_info[oc_strm].test_client_present)
  {
    state->als_info.test_info[oc_strm].test_client_present = false;
    test_client_present = true;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if (state->prox_info.test_info[oc_strm].test_client_present)
  {
    state->prox_info.test_info[oc_strm].test_client_present = false;
    test_client_present = true;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  if (state->rgb_info.test_info[oc_strm].test_client_present)
  {
    state->rgb_info.test_info[oc_strm].test_client_present = false;
    test_client_present = true;
  }
#endif

  if (test_client_present && state->client_present == 0)
  {
    sns_service_manager *service_mgr = this->cb->get_service_manager(this);
    sns_stream_service *stream_mgr = (sns_stream_service*)
                service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
    stream_mgr->api->remove_stream(stream_mgr, stream);
    if (oc_strm == AMS_STRM_IDX) {
      state->timer_is_active = false;
      state->timer_data_stream = NULL;
    } else {
      state->interrupt_data_stream = NULL;
    }
  }
}

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
                                float *bias_result, uint8_t bias_size)
{
  uint8_t arr_index = 0;
  float scale_arr[20];
  float bias_arr[20];
  pb_float_arr_arg scale_arg;
  pb_float_arr_arg bias_arg;
  sns_cal_event cal_event = sns_cal_event_init_default;
  cal_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  if (0 == scale_size && 0 == bias_size) {
    return;
  }

  if (scale_size > 0)
  {
    sns_memscpy (scale_arr, sizeof(float) * scale_size, scale_result, sizeof(float) * scale_size);
    scale_arg.arr = scale_arr;
    scale_arg.arr_len = scale_size;
    scale_arg.arr_index = &arr_index;

    cal_event.scale_factor.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.scale_factor.arg = &scale_arg;
  }
  if (bias_size > 0)
  {
    sns_memscpy (bias_arr, sizeof(float) * bias_size, bias_result, sizeof(float) * bias_size);
    bias_arg.arr = bias_arr;
    bias_arg.arr_len = bias_size;
    bias_arg.arr_index = &arr_index;

    cal_event.bias.funcs.encode = &pb_encode_float_arr_cb;
    cal_event.bias.arg = &bias_arg;
  }

  pb_send_event(instance,
                sns_cal_event_fields,
                &cal_event,
                sns_get_system_time(),
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                uid);
}

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
                                  sns_sensor_uid *uid, bool test_result)
{
  uint8_t data[1] = {0};
  pb_buffer_arg buff_arg = (pb_buffer_arg)
      { .buf = &data, .buf_len = sizeof(data) };
  sns_physical_sensor_test_event test_event =
     sns_physical_sensor_test_event_init_default;
  test_event.test_passed = test_result;
  test_event.test_type = SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY;
  test_event.test_data.funcs.encode = &pb_encode_string_cb;
  test_event.test_data.arg = &buff_arg;

  pb_send_event(instance,
                sns_physical_sensor_test_event_fields,
                &test_event,
                sns_get_system_time(),
                SNS_PHYSICAL_SENSOR_TEST_MSGID_SNS_PHYSICAL_SENSOR_TEST_EVENT,
                uid);
}

/**
 * Collects factory test results and submits for replying and/or updating
 * the cal data in the registry.
 *
 * @param[i] instance     Instance reference
 * @param[io] test_result  Test pass/fail result if already known, else NULL
 *
 * @return none
 */
static void _ams_wrap_up_factory_test(sns_sensor_instance *this,
                                         bool *test_result)
{
  ams_instance_state *state = (ams_instance_state*)this->state->state;
  ams_deviceCtx_t *devCtx = (ams_deviceCtx_t *)(state->priv);
  int32_t updateAvailableFlags;
  bool lcl_test_result;

  if(test_result)
  {
    SNS_INST_PRINTF(MED, this
                    , "AMS: _ams_wrap_up_factory_test: enter: instance %p, *test_result=%d"
                    , this
                    , *test_result
                    );
  } else {
    SNS_INST_PRINTF(ERROR, this
                    , "AMS: _ams_wrap_up_factory_test: enter: instance %p, no incoming test_result"
                    , this
                    );
  }

  if(!test_result)
  {
    test_result = &lcl_test_result;
    updateAvailableFlags = ams_getResult(state->priv);
    SNS_INST_PRINTF(ERROR, this
                    , "AMS: _ams_wrap_up_factory_test: got cal status=%d"
                    , updateAvailableFlags
                    );
    if((state->sensor_type_under_test == AMS_ALS_OC)
        || (state->sensor_type_under_test == AMS_ALS_STRM)
        || (state->sensor_type_under_test == AMS_PROX_OC)
        || (state->sensor_type_under_test == AMS_PROX_STRM))
    {
      if (updateAvailableFlags == AMS_CALIBRATION_DONE){
        *test_result = true;
      } else if(updateAvailableFlags == AMS_CALIBRATION_DONE_BUT_FAILED){
        *test_result = false;
      }
    }
    else
    {
      /* RGB has no simple cal procedure so we succeed by decree. */
      *test_result = true;
    }
  }

  /* Assume no more than 1 sensor can be self-tested at a time */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
  if(state->als_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      SNS_INST_PRINTF(MED, this
                      , "AMS: _ams_wrap_up_factory_test: ALS_OC, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.alsCalibrationFactor = devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      state->cal_version[ALSCALIBRATIONFACTOR_IDX]++;
      result = (float) devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      _ams_send_cal_event (this, &state->als_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0);
      _ams_send_factory_test_event(this, &state->als_info.suid[AMS_OC_IDX], *test_result);
      state->new_self_test_request = false;
    }
    return;
  }

  if(state->als_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->als_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      SNS_INST_PRINTF(MED, this
                      , "AMS: _ams_wrap_up_factory_test: ALS_STRM, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.alsCalibrationFactor = devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      state->cal_version[ALSCALIBRATIONFACTOR_IDX]++;
      result = (float) devCtx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
      _ams_send_cal_event (this, &state->als_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0);
      _ams_send_factory_test_event(this, &state->als_info.suid[AMS_STRM_IDX], *test_result);
      state->new_self_test_request = false;
    }
    return;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
  if(state->prox_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      SNS_INST_PRINTF(MED, this
                      , "AMS: _ams_wrap_up_factory_test: PROX_OC, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.proxHardThreshold = devCtx->systemCalibrationData->proxHardThreshold;
      state->cal_version[PROXHARDTHRESHOLD_IDX]++;
      result = (float) devCtx->systemCalibrationData->proxHardThreshold;
      _ams_send_cal_event (this, &state->prox_info.suid[AMS_OC_IDX],
         &result, 1, NULL, 0);
      _ams_send_factory_test_event(this, &state->prox_info.suid[AMS_OC_IDX], *test_result);
      state->new_self_test_request = false;
    }
    return;
  }

  if(state->prox_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->prox_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result;
      SNS_INST_PRINTF(MED, this
                      , "AMS: _ams_wrap_up_factory_test: PROX_STRM, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.proxHardThreshold = devCtx->systemCalibrationData->proxHardThreshold;
      state->cal_version[PROXHARDTHRESHOLD_IDX]++;
      result = (float) devCtx->systemCalibrationData->proxHardThreshold;
      _ams_send_cal_event (this, &state->prox_info.suid[AMS_STRM_IDX],
         &result, 1, NULL, 0);
      _ams_send_factory_test_event(this, &state->prox_info.suid[AMS_STRM_IDX], *test_result);
      state->new_self_test_request = false;
    }
    return;
  }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
  if(state->rgb_info.test_info[AMS_OC_IDX].test_client_present)
  {
    if(state->rgb_info.test_info[AMS_OC_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result[4];
      SNS_INST_PRINTF(ERROR, this
                      , "AMS: _ams_wrap_up_factory_test: RGB_OC, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.alsCoefR = devCtx->systemCalibrationData->alsCoefR;
      state->calibration_data.alsCoefG = devCtx->systemCalibrationData->alsCoefG;
      state->calibration_data.alsCoefB = devCtx->systemCalibrationData->alsCoefB;
      result[0] = (float)devCtx->systemCalibrationData->alsCoefR;
      result[1] = (float)devCtx->systemCalibrationData->alsCoefG;
      result[2] = (float)devCtx->systemCalibrationData->alsCoefB;
      result[3] = 0;
      state->cal_version[ALSCOEFR_IDX]++;
      state->cal_version[ALSCOEFG_IDX]++;
      state->cal_version[ALSCOEFB_IDX]++;
      _ams_send_cal_event (this, &state->rgb_info.suid[AMS_OC_IDX],
         result, 4, NULL, 0);
      _ams_send_factory_test_event(this, &state->rgb_info.suid[AMS_OC_IDX], *test_result);
      state->new_self_test_request = false;
      state->rgb_info.test_info[AMS_OC_IDX].test_client_present = false;
    }
    return;
  }

  if(state->rgb_info.test_info[AMS_STRM_IDX].test_client_present)
  {
    if(state->rgb_info.test_info[AMS_STRM_IDX].test_type == SNS_PHYSICAL_SENSOR_TEST_TYPE_FACTORY)
    {
      float result[4];
      SNS_INST_PRINTF(ERROR, this
                      , "AMS: _ams_wrap_up_factory_test: RGB_STRM, *test_result=%d"
                      , *test_result
                      );
      state->calibration_data.alsCoefR = devCtx->systemCalibrationData->alsCoefR;
      state->calibration_data.alsCoefG = devCtx->systemCalibrationData->alsCoefG;
      state->calibration_data.alsCoefB = devCtx->systemCalibrationData->alsCoefB;
      result[0] = (float)devCtx->systemCalibrationData->alsCoefR;
      result[1] = (float)devCtx->systemCalibrationData->alsCoefG;
      result[2] = (float)devCtx->systemCalibrationData->alsCoefB;
      result[3] = 0;
      state->cal_version[ALSCOEFR_IDX]++;
      state->cal_version[ALSCOEFG_IDX]++;
      state->cal_version[ALSCOEFB_IDX]++;
      _ams_send_cal_event (this, &state->rgb_info.suid[AMS_STRM_IDX],
         result, 4, NULL, 0);
      _ams_send_factory_test_event(this, &state->rgb_info.suid[AMS_STRM_IDX], *test_result);
      state->new_self_test_request = false;
      state->rgb_info.test_info[AMS_STRM_IDX].test_client_present = false;
    }
    return;
  }
#endif

}

/** Public Data Definitions. */

sns_sensor_instance_api ams_sensor_instance_api =
{
  .struct_len        = sizeof(sns_sensor_instance_api),
  .init              = &ams_inst_init,
  .deinit            = &ams_inst_deinit,
  .set_client_config = &ams_inst_set_client_config,
  .notify_event      = &ams_inst_notify_event
};
