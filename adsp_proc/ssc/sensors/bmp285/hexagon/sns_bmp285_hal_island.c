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


#include "sns_rc.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_math_util.h"
#include "sns_service_manager.h"
#include "sns_com_port_types.h"
#include "sns_sync_com_port_service.h"
#include "sns_types.h"

#include "sns_bmp285_hal.h"
#include "sns_bmp285_sensor.h"
#include "sns_bmp285_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"

#include "sns_std_sensor.pb.h"
#include "sns_timer.pb.h"
#include "sns_std.pb.h"
#if(BMP_CONFIG_ENABLE_DIAG_LOG == 1)
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#endif

#if BMP_CONFIG_ENABLE_DIAG_LOG
typedef struct log_sensor_state_raw_info
{
  /* Pointer to diag service */
  sns_diag_service *diag;
  /* Pointer to sensor instance */
  sns_sensor_instance *instance;
  /* Pointer to sensor UID*/
  struct sns_sensor_uid *sensor_uid;
  /* Size of a single encoded sample */
  size_t encoded_sample_size;
  /* Pointer to log*/
  void *log;
  /* Size of allocated space for log*/
  uint32_t log_size;
  /* Number of actual bytes written*/
  uint32_t bytes_written;
  /* Number of batch samples written*/
  /* A batch may be composed of several logs*/
  uint32_t batch_sample_cnt;
  /* Number of log samples written*/
  uint32_t log_sample_cnt;
} log_sensor_state_raw_info;

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[BMP285_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} bmp285_batch_sample;

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
sns_rc bmp285_encode_sensor_state_log_interrupt(
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
sns_rc bmp285_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[BMP285_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = BMP285_NUM_AXES,
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
  bmp285_batch_sample *raw_sample = (bmp285_batch_sample *)log;

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

    parsed_log_size += sizeof(bmp285_batch_sample);
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
 * @param[i] log_raw_info Sensor state log info
 * @param[i] log_size     Optional size of log packet to
 *    be allocated. If not provided by setting to 0, will
 *    default to using maximum supported log packet size
 */
void bmp285_log_sensor_state_raw_alloc(
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
void bmp285_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  bmp285_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (bmp285_batch_sample *)log_raw_info->log;

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
        bmp285_encode_log_sensor_state_raw);

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
sns_rc bmp285_log_sensor_state_raw_add(
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

  if( (log_raw_info->bytes_written + sizeof(bmp285_batch_sample)) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    bmp285_log_sensor_state_raw_submit(log_raw_info, false);
    bmp285_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    bmp285_batch_sample *sample =
        (bmp285_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(bmp285_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }

  return rc;
}
#endif

/**
 * Read wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           read buffer
 * @param[i] bytes            bytes to read
 * @param[o] xfer_bytes       bytes read
 *
 * @return sns_rc
 */
sns_rc bmp285_com_read_wrapper(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint32_t reg_addr,
  uint8_t  *buffer,
  uint32_t bytes,
  uint32_t *xfer_bytes)
{
  sns_port_vector port_vec;
  port_vec.buffer = buffer;
  port_vec.bytes = bytes;
  port_vec.is_write = false;
  port_vec.reg_addr = reg_addr;

  return scp_service->api->sns_scp_register_rw(port_handle,
                              &port_vec,
                              1,
                              false,
                              xfer_bytes);
}

/**
 * Write wrapper for Synch Com Port Service.
 *
 * @param[i] port_handle      port handle
 * @param[i] reg_addr         register address
 * @param[i] buffer           write buffer
 * @param[i] bytes            bytes to write
 * @param[o] xfer_bytes       bytes written
 * @param[i] save_write_time  true to save write transfer time.
 *
 * @return sns_rc
 */
sns_rc bmp285_com_write_wrapper(
  sns_sync_com_port_service * scp_service,
  sns_sync_com_port_handle *port_handle,
  uint32_t reg_addr,
  uint8_t  *buffer,
  uint32_t bytes,
  uint32_t *xfer_bytes,
  bool     save_write_time)
{
  sns_port_vector port_vec;
  port_vec.buffer = buffer;
  port_vec.bytes = bytes;
  port_vec.is_write = true;
  port_vec.reg_addr = reg_addr;

  return  scp_service->api->sns_scp_register_rw(
      port_handle,
      &port_vec,
      1,
      save_write_time,
      xfer_bytes);
}

/*!
 *  @brief This API is used to read uncompensated temperature
 *  in the registers 0xFA, 0xFB and 0xFC
 *  @note 0xFA -> MSB -> bit from 0 to 7
 *  @note 0xFB -> LSB -> bit from 0 to 7
 *  @note 0xFC -> LSB -> bit from 4 to 7
 *  @param v_uncomp_temperature_s32 : The uncompensated temperature.
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
BMP280_RETURN_FUNCTION_TYPE bmp280_read_uncomp_temperature(
  bmp285_instance_state *state,
  s32 *v_uncomp_temperature_s32)
{
  /* variable used to return communication result*/
  BMP280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
  /* Array holding the MSB and LSb value
   a_data_u8r[0] - Temperature MSB
   a_data_u8r[1] - Temperature LSB
   a_data_u8r[2] - Temperature LSB
   */
  u8 a_data_u8r[BMP280_TEMPERATURE_DATA_SIZE] = {BMP280_INIT_VALUE,
      BMP280_INIT_VALUE, BMP280_INIT_VALUE};
  u8 data = 0;
  u32 xfer_bytes;
  /* check the p_bmp280 structure pointer as NULL*/
  if (state == BMP280_NULL) {
    com_rslt = E_BMP280_NULL_PTR;
  } else {
    /* read temperature data */
    com_rslt = state->com_read(
        state->scp_service,
        state->com_port_info.port_handle,
        BMP280_TEMPERATURE_MSB_REG, a_data_u8r,
        BMP280_TEMPERATURE_DATA_LENGTH,
        &xfer_bytes);

        /* read temperature data */
    com_rslt = state->com_read(
        state->scp_service,
        state->com_port_info.port_handle,
        0xF4, &data,
        1,
        &xfer_bytes);
    *v_uncomp_temperature_s32 = (s32)((((u32)(
        a_data_u8r[BMP280_TEMPERATURE_MSB_DATA]))
        << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
        | (((u32)(
        a_data_u8r[BMP280_TEMPERATURE_LSB_DATA]))
        << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
        | ((u32)a_data_u8r[BMP280_TEMPERATURE_XLSB_DATA]
        >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));
  }
  return com_rslt;
}

/*!
 *  @brief This API is used to read uncompensated pressure.
 *  in the registers 0xF7, 0xF8 and 0xF9
 *  @note 0xF7 -> MSB -> bit from 0 to 7
 *  @note 0xF8 -> LSB -> bit from 0 to 7
 *  @note 0xF9 -> LSB -> bit from 4 to 7
 *  @param v_uncomp_pressure_s32 : The value of uncompensated pressure
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 */
BMP280_RETURN_FUNCTION_TYPE bmp280_read_uncomp_pressure(
  bmp285_instance_state *state,
  s32 *v_uncomp_pressure_s32)
{
  /* variable used to return communication result*/
  BMP280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
  /* Array holding the MSB and LSb value
   a_data_u8[0] - Pressure MSB
   a_data_u8[1] - Pressure LSB
   a_data_u8[2] - Pressure LSB
   */
  u8 a_data_u8[BMP280_PRESSURE_DATA_SIZE] = {BMP280_INIT_VALUE,
      BMP280_INIT_VALUE, BMP280_INIT_VALUE};
  u32 xfer_bytes;
  /* check the p_bmp280 structure pointer as NULL*/
  if (state == BMP280_NULL) {
    com_rslt = E_BMP280_NULL_PTR;
  } else {
    com_rslt = state->com_read(
        state->scp_service,
        state->com_port_info.port_handle,
        BMP280_PRESSURE_MSB_REG, a_data_u8,
        BMP280_PRESSURE_DATA_LENGTH,
        &xfer_bytes);
    *v_uncomp_pressure_s32 = (s32)((((u32)(
        a_data_u8[BMP280_PRESSURE_MSB_DATA]))
        << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
        | (((u32)(a_data_u8[BMP280_PRESSURE_LSB_DATA]))
        << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
        | ((u32)a_data_u8[BMP280_PRESSURE_XLSB_DATA]
        >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));
  }
  return com_rslt;
}

/*!
 * @brief This API used to read
 * actual temperature from uncompensated temperature
 * @note Returns the value in Degree centigrade
 * @note Output value of "51.23" equals 51.23 DegC.
 *  @param v_uncomp_temperature_s32 : value of uncompensated temperature
 *  @return
 *  Actual temperature in floating point
 *
 */
float bmp280_compensate_temperature_double(
  bmp285_instance_state *state,
  s32 v_uncomp_temperature_s32)
{
  double v_x1_u32r = BMP280_INIT_VALUE;
  double v_x2_u32r = BMP280_INIT_VALUE;
  double temperature = BMP280_INIT_VALUE;
  /* calculate x1*/
  v_x1_u32r = (((double)v_uncomp_temperature_s32) / 16384.0 -
      ((double)state->calib_param.dig_T1) / 1024.0) *
  ((double)state->calib_param.dig_T2);
  /* calculate x2*/
  v_x2_u32r = ((((double)v_uncomp_temperature_s32) / 131072.0 -
      ((double)state->calib_param.dig_T1) / 8192.0) *
      (((double)v_uncomp_temperature_s32) / 131072.0 -
      ((double)state->calib_param.dig_T1) / 8192.0)) *
  ((double)state->calib_param.dig_T3);
  /* calculate t_fine*/
  state->calib_param.t_fine = (s32)(v_x1_u32r + v_x2_u32r);
  /* calculate true pressure*/
  temperature = (v_x1_u32r + v_x2_u32r) / 5120.0;

  return temperature;
}
/*!
 *  @brief Reads actual pressure from uncompensated pressure
 *  and returns pressure in Pa as double.
 *  @note Output value of "96386.2"
 *  equals 96386.2 Pa = 963.862 hPa.
 *  @param v_uncomp_pressure_s32 : value of uncompensated pressure
 *  @return
 *  Actual pressure in floating point
 *
 */
float bmp280_compensate_pressure_double(
  bmp285_instance_state *state,
  s32 v_uncomp_pressure_s32)
{
  double v_x1_u32r = BMP280_INIT_VALUE;
  double v_x2_u32r = BMP280_INIT_VALUE;
  double pressure = BMP280_INIT_VALUE;
  double v_pressure_diff_dbl = 0;
  double v_x3_u32r = 0;

  v_x1_u32r = ((double)state->calib_param.t_fine/2.0) - 64000.0;
  v_x2_u32r = v_x1_u32r * v_x1_u32r *
  ((double)state->calib_param.dig_P6) / 32768.0;
  v_x2_u32r = v_x2_u32r + v_x1_u32r *
  ((double)state->calib_param.dig_P5) * 2.0;
  v_x2_u32r = (v_x2_u32r / 4.0) +
  (((double)state->calib_param.dig_P4) * 65536.0);
  v_x1_u32r = (((double)state->calib_param.dig_P3) *
    v_x1_u32r * v_x1_u32r / 524288.0 +
    ((double)state->calib_param.dig_P2) * v_x1_u32r) / 524288.0;
  v_x1_u32r = (1.0 + v_x1_u32r / 32768.0) *
  ((double)state->calib_param.dig_P1);
  pressure = 1048576.0 - (double)v_uncomp_pressure_s32;
  /* Avoid exception caused by division by zero */
  if ((v_x1_u32r > 0) || (v_x1_u32r < 0))
    pressure = (pressure - (v_x2_u32r / 4096.0)) *
          6250.0 / v_x1_u32r;
  else
  return BMP280_INVALID_DATA;
  v_x1_u32r = ((double)state->calib_param.dig_P9) *
  pressure * pressure / 2147483648.0;
  v_x2_u32r = pressure * ((double)state->calib_param.dig_P8) / 32768.0;

  /*! added  for bmp285 support */
  /* calculate different pressure*/
  v_pressure_diff_dbl = pressure / 256.0;
  /* calculate x3*/       v_x3_u32r = ((v_pressure_diff_dbl *
  v_pressure_diff_dbl * v_pressure_diff_dbl
  * (double)state->calib_param.dig_P10)
  / 131072.0);
  /* calculate true pressure*/
  pressure = pressure + (v_x1_u32r + v_x2_u32r + v_x3_u32r +
      ((double)state->calib_param.dig_P7)) / 16.0;

  return pressure;
}
/*!
 *  @brief This API used to set the
 *  Operational Mode from the sensor in the register 0xF4 bit 0 and 1
 *  @param v_power_mode_u8 : The value of power mode value
 *  value            |   Power mode
 * ------------------|------------------
 *  0x00             | BMP280_SLEEP_MODE
 *  0x01 and 0x02    | BMP280_FORCED_MODE
 *  0x03             | BMP280_NORMAL_MODE
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 */
BMP280_RETURN_FUNCTION_TYPE bmp280_set_power_mode(
  bmp285_instance_state *state,
  u8 v_power_mode_u8)
{
  /* variable used to return communication result*/
  BMP280_RETURN_FUNCTION_TYPE com_rslt = ERROR;
  u8 v_mode_u8 = BMP280_INIT_VALUE;
  u32 xfer_bytes;
  /* check the p_bmp280 structure pointer as NULL*/
  if (state == BMP280_NULL) {
    com_rslt = E_BMP280_NULL_PTR;
  } else {
    if (v_power_mode_u8 <= BMP280_NORMAL_MODE) {
      /* write the power mode*/
      v_mode_u8 = (state->oversamp_temperature
          << BMP280_SHIFT_BIT_POSITION_BY_05_BITS)
          + (state->oversamp_pressure
          << BMP280_SHIFT_BIT_POSITION_BY_02_BITS)
          + v_power_mode_u8;
      com_rslt = state->com_write(
          state->scp_service,
          state->com_port_info.port_handle,
          BMP280_CTRL_MEAS_REG_POWER_MODE__REG,
          &v_mode_u8,
          BMP280_GEN_READ_WRITE_DATA_LENGTH,
          &xfer_bytes,
          false);
    } else {
      com_rslt = E_BMP280_OUT_OF_RANGE;
    }
  }
  return com_rslt;
}

void bmp285_handle_temperature_data_sample(
    sns_sensor_instance *const instance,
    float sample_data,
    sns_time timestamp)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  sns_sensor_uid suid;

  suid = state->temperature_info.suid;
//QC: This change is to use factory scale and bias to factory calibrate data?
//QC: Please check the math. See comments in sns_cal.proto:
//QC:  if only SF and B are available,
//QC:       Sc = ((S .* SF) - B)

  // factory calibration
  // Sc = C * (Sr - B)
  // Where,
  // *Sc = Calibrated sample
  // *Sr = Raw sample
  // *C = Scale
  // *B = Bias
  sample_data = (state->sensor_temp_registry_cfg.fac_cal_scale[0] * (sample_data)) -
    state->sensor_temp_registry_cfg.fac_cal_bias[0];
  pb_send_sensor_stream_event(instance,
      &suid,
      timestamp,
      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
      status,
      &sample_data,
      1,
      state->encoded_imu_event_len);
}

void bmp285_handle_pressure_data_sample(
    sns_sensor_instance *const instance,
    float sample_data,
    sns_time timestamp)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  sns_sensor_uid suid;

  suid = state->pressure_info.suid;
#if BMP_CONFIG_ENABLE_DIAG_LOG
  sns_diag_service* diag       = state->diag_service;
  log_sensor_state_raw_info log_sensor_state_raw_info;
  log_sensor_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
  log_sensor_state_raw_info.diag = diag;
  log_sensor_state_raw_info.instance = instance;
  log_sensor_state_raw_info.sensor_uid = &suid;
  bmp285_log_sensor_state_raw_alloc(&log_sensor_state_raw_info, 0);
#endif
//QC: This change is to use factory scale and bias to factory calibrate data?
//QC: Please check the math. See comments in sns_cal.proto:
//QC:  if only SF and B are available,
//QC:       Sc = ((S .* SF) - B)

  // factory calibration
  // Sc = C * (Sr - B)
  // Where,
  // *Sc = Calibrated sample
  // *Sr = Raw sample
  // *C = Scale
  // *B = Bias
  sample_data = (state->sensor_pressure_registry_cfg.fac_cal_scale[0] * sample_data) -
    state->sensor_pressure_registry_cfg.fac_cal_bias[0];
  pb_send_sensor_stream_event(instance,
      &suid,
      timestamp,
      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
      status,
      &sample_data,
      1,
      state->encoded_imu_event_len);
#if BMP_CONFIG_ENABLE_DIAG_LOG
  bmp285_log_sensor_state_raw_add(
        &log_sensor_state_raw_info,
        &sample_data,
        timestamp,
        status);
  bmp285_log_sensor_state_raw_submit(&log_sensor_state_raw_info, true);
#endif
}
#if BMP_CONFIG_ENABLE_DAE
void bmp285_convert_and_send_pressure_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       data[6])
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  s32 v_uncomp_pressure_s32 = 0;
  float temperature = 0;
  float bmp285_comp_data_buffer = 0;

  v_uncomp_pressure_s32 = (s32)((((u32)(
          data[0]))
          << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
          | (((u32)(data[1]))
          << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
          | ((u32)data[2]
          >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));
  v_uncomp_temperature_s32 = (s32)((((u32)(
        data[3]))
        << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
        | (((u32)(
        data[4]))
        << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
        | ((u32)data[5]
        >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));
  temperature = bmp280_compensate_temperature_double(state,v_uncomp_temperature_s32);
  bmp285_comp_data_buffer = bmp280_compensate_pressure_double(state, v_uncomp_pressure_s32) / 100;
  bmp285_handle_pressure_data_sample(instance, bmp285_comp_data_buffer, timestamp);
}

void bmp285_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       data[3])
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  float bmp285_comp_data_buffer = 0;

  v_uncomp_temperature_s32 = (s32)((((u32)(
          data[0]))
          << BMP280_SHIFT_BIT_POSITION_BY_12_BITS)
          | (((u32)(data[1]))
          << BMP280_SHIFT_BIT_POSITION_BY_04_BITS)
          | ((u32)data[2]
          >> BMP280_SHIFT_BIT_POSITION_BY_04_BITS));

  bmp285_comp_data_buffer = bmp280_compensate_temperature_double(state,v_uncomp_temperature_s32);
  bmp285_handle_temperature_data_sample(instance, bmp285_comp_data_buffer, timestamp);
}
#endif
void bmp285_process_pressure_timer_stream_data_buffer(sns_sensor_instance *instance, sns_timer_sensor_event * const timer_event)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  s32 v_uncomp_pressure_s32 = 0;
  float temperature = 0;
  float pressure = 0;
  float  bmp285_comp_data_buffer = 0;

  bmp280_read_uncomp_temperature(state, &v_uncomp_temperature_s32);
  bmp280_read_uncomp_pressure(state, &v_uncomp_pressure_s32);
  temperature = bmp280_compensate_temperature_double(state,v_uncomp_temperature_s32);
  pressure = bmp280_compensate_pressure_double(state, v_uncomp_pressure_s32);
  bmp285_comp_data_buffer = pressure / 100;
  BMP_INST_LOG(LOW, instance, "p timer_event p_data %d timestamp  0x%x%08x",
    (uint32_t)(pressure), (uint32_t)(timer_event->timeout_time>>32), (uint32_t)(timer_event->timeout_time));
  bmp285_handle_pressure_data_sample(instance, bmp285_comp_data_buffer, (sns_time)timer_event->timeout_time);
}

void bmp285_process_temperature_timer_stream_data_buffer(sns_sensor_instance *instance, sns_timer_sensor_event * const timer_event)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  float bmp285_comp_data_buffer = 0;

  bmp280_read_uncomp_temperature(state, &v_uncomp_temperature_s32);
  bmp285_comp_data_buffer = bmp280_compensate_temperature_double(state,v_uncomp_temperature_s32);
  BMP_INST_LOG(LOW, instance, "t timer_event t_data %d timestamp  0x%x%08x",
    (uint32_t)(bmp285_comp_data_buffer), (uint32_t)(timer_event->timeout_time>>32), (uint32_t)(timer_event->timeout_time));
  bmp285_handle_temperature_data_sample(instance, bmp285_comp_data_buffer, (sns_time)timer_event->timeout_time);
}

void bmp285_handle_pressure_data_stream_timer_event(sns_sensor_instance *const instance, sns_timer_sensor_event * const timer_event)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  int err = 0;
  bmp285_process_pressure_timer_stream_data_buffer(instance, timer_event);
  err = bmp280_set_power_mode(state,BMP280_FORCED_MODE);
  if (err)
     SNS_INST_PRINTF(ERROR, instance,"config power mode failed err = %d", err);
}

void bmp285_handle_temperature_data_stream_timer_event(sns_sensor_instance *const instance, sns_timer_sensor_event * const timer_event)
{
  bmp285_instance_state *state = (bmp285_instance_state*)instance->state->state;
  int err = 0;
  bmp285_process_temperature_timer_stream_data_buffer(instance, timer_event);
  err = bmp280_set_power_mode(state,BMP280_FORCED_MODE);
  if (err)
     SNS_INST_PRINTF(ERROR, instance,"config power mode failed err = %d", err);
}
