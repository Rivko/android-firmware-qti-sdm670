/**
 * @file sns_lsm6dsm_hal.c
 *
 * Copyright (c) 2018, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
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
 **/

#include <math.h>
#include "sns_cal_util.h"
#include "sns_com_port_types.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_service_manager.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"
#include "sns_sensor_util.h"
#include "sns_lsm6dsm_hal.h"
#include "sns_lsm6dsm_sensor.h"
#include "sns_lsm6dsm_sensor_instance.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_printf.h"
#include "sns_async_com_port.pb.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_diag.pb.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_timer.pb.h"
#include "sns_cal.pb.h"

#define ASYNC_MIN_SAMPLES 5 //use ascp only if wmk > 5
#define LSM6DSM_HEART_BEAT_ODR_COUNT 5

/** Need to use ODR table. */
extern const odr_reg_map lsm6dsm_odr_map[LSM6DSM_REG_MAP_TABLE_SIZE];

/** LSM6DSM Accel Range min */
const float lsm6dsm_accel_range_min[] =
{
  LSM6DSM_ACCEL_RANGE_2G_MIN,
  LSM6DSM_ACCEL_RANGE_4G_MIN,
  LSM6DSM_ACCEL_RANGE_8G_MIN,
  LSM6DSM_ACCEL_RANGE_16G_MIN
};  //mg/LSB

/** LSM6DSM Accel Range max */
const float lsm6dsm_accel_range_max[] =
{
  LSM6DSM_ACCEL_RANGE_2G_MAX,
  LSM6DSM_ACCEL_RANGE_4G_MAX,
  LSM6DSM_ACCEL_RANGE_8G_MAX,
  LSM6DSM_ACCEL_RANGE_16G_MAX
};  //mg/LSB

/** LSM6DSM Gyro Range min */
const float lsm6dsm_gyro_range_min[] =
{
  LSM6DSM_GYRO_RANGE_125_MIN,
  LSM6DSM_GYRO_RANGE_245_MIN,
  LSM6DSM_GYRO_RANGE_500_MIN,
  LSM6DSM_GYRO_RANGE_1000_MIN,
  LSM6DSM_GYRO_RANGE_2000_MIN
};  //mdps/LSB

/** LSM6DSM Gyro Range max */
const float lsm6dsm_gyro_range_max[] =
{
  LSM6DSM_GYRO_RANGE_125_MAX,
  LSM6DSM_GYRO_RANGE_245_MAX,
  LSM6DSM_GYRO_RANGE_500_MAX,
  LSM6DSM_GYRO_RANGE_1000_MAX,
  LSM6DSM_GYRO_RANGE_2000_MAX
};  //mdps/LSB

#define SLOPE_SETTLING_TIME_NS (200000000) // 2 sample time with 13Hz in nano sec + tolerence = ~200ms
#define HPF_SETTLING_TIME_NS (4000000000) //4 sec in nano secs
#define MAX_INTERRUPT_CNT  3
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
  float sample[LSM6DSM_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} lsm6dsm_batch_sample;

// Unencoded batch sample
typedef struct
{
  /* Batch Sample type as defined in sns_diag.pb.h */
  sns_diag_batch_sample_type sample_type;
  /* Timestamp of the sensor state data sample */
  sns_time timestamp;
  /*Raw sensor state data sample*/
  float sample[1];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} lsm6dsm_batch_sample_oneaxis;

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
sns_rc lsm6dsm_encode_sensor_state_log_interrupt(
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
 * Encode log sensor state raw packet for sensor with 1 axis data
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
sns_rc lsm6dsm_encode_log_sensor_state_raw_oneaxis(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[1];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = 1,
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
  lsm6dsm_batch_sample_oneaxis *raw_sample = (lsm6dsm_batch_sample_oneaxis *)log;

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

    parsed_log_size += sizeof(lsm6dsm_batch_sample_oneaxis);
    i++;
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
sns_rc lsm6dsm_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[LSM6DSM_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = LSM6DSM_NUM_AXES,
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
  lsm6dsm_batch_sample *raw_sample = (lsm6dsm_batch_sample *)log;

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

    parsed_log_size += sizeof(lsm6dsm_batch_sample);
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
void lsm6dsm_log_sensor_state_raw_alloc(
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
void lsm6dsm_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  uint8_t sample_size,
  bool batch_complete)
{
  lsm6dsm_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (lsm6dsm_batch_sample *)log_raw_info->log;

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
  if(sample_size == 1)
  {
  log_raw_info->diag->api->submit_log(
        log_raw_info->diag,
        log_raw_info->instance,
        log_raw_info->sensor_uid,
        log_raw_info->bytes_written,
        log_raw_info->log,
        SNS_DIAG_SENSOR_STATE_LOG_RAW,
        log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
        lsm6dsm_encode_log_sensor_state_raw_oneaxis);
  }
  else
  {
    log_raw_info->diag->api->submit_log(
          log_raw_info->diag,
          log_raw_info->instance,
          log_raw_info->sensor_uid,
          log_raw_info->bytes_written,
          log_raw_info->log,
          SNS_DIAG_SENSOR_STATE_LOG_RAW,
          log_raw_info->log_sample_cnt * log_raw_info->encoded_sample_size,
        lsm6dsm_encode_log_sensor_state_raw);
  }

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
sns_rc lsm6dsm_log_sensor_state_raw_add(
  log_sensor_state_raw_info *log_raw_info,
  float *raw_data,
  uint8_t  sample_size,
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

  uint8_t batch_sample_size = sizeof(lsm6dsm_batch_sample);
  if(sample_size == 1)
    batch_sample_size = sizeof(lsm6dsm_batch_sample_oneaxis);

  if( (log_raw_info->bytes_written + batch_sample_size) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    lsm6dsm_log_sensor_state_raw_submit(log_raw_info, sample_size, false);
    lsm6dsm_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    if(sample_size == 1) {
      lsm6dsm_batch_sample_oneaxis *sample_oneaxis =
        (lsm6dsm_batch_sample_oneaxis *)log_raw_info->log;
      if(0 == log_raw_info->batch_sample_cnt)
      {
        sample_oneaxis[log_raw_info->log_sample_cnt].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_FIRST;
      }
      else
      {
        sample_oneaxis[log_raw_info->log_sample_cnt].sample_type =
          SNS_DIAG_BATCH_SAMPLE_TYPE_INTERMEDIATE;
      }

      sample_oneaxis[log_raw_info->log_sample_cnt].timestamp = timestamp;

      sns_memscpy(sample_oneaxis[log_raw_info->log_sample_cnt].sample,
          sizeof(sample_oneaxis[log_raw_info->log_sample_cnt].sample),
          raw_data,
          sizeof(sample_oneaxis[log_raw_info->log_sample_cnt].sample));

      sample_oneaxis[log_raw_info->log_sample_cnt].status = status;
    }
    else {
    lsm6dsm_batch_sample *sample =
        (lsm6dsm_batch_sample *)log_raw_info->log;
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
    }

    log_raw_info->bytes_written += batch_sample_size;

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }

  return rc;
}

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
static sns_rc lsm6dsm_com_read_wrapper(
  sns_sync_com_port_service* scp_service,
  sns_sync_com_port_handle*  port_handle,
   uint32_t reg_addr,
   uint8_t *buffer,
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
static sns_rc lsm6dsm_com_write_wrapper(
   sns_sensor_instance *const instance,
   uint32_t reg_addr,
   uint8_t *buffer,
   uint32_t bytes,
   uint32_t *xfer_bytes,
   bool save_write_time)
{
  sns_port_vector port_vec;
  port_vec.buffer = buffer;
  port_vec.bytes = bytes;
  port_vec.is_write = true;
  port_vec.reg_addr = reg_addr;
  sns_rc rc;
  lsm6dsm_instance_state *inst_state =
    (lsm6dsm_instance_state*)instance->state->state;
  rc = inst_state->scp_service->api->sns_scp_register_rw(inst_state->com_port_info.port_handle,
                                                        &port_vec,
                                                        1,
                                                        save_write_time,
                                                        xfer_bytes);
  if(rc != SNS_RC_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, instance, "write_wrapper %d", rc);
  }
#if LSM6DSM_DUMP_REG
  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "reg write 0x%x = %x",reg_addr, buffer[0]);
#endif
  return rc;
}

/**
 * If mask = 0x0 or 0xFF, or if size > 1, write reg_value
 * directly to reg_addr. Else, read value at reg_addr and only
 * modify bits defined by mask.
 *
 * @param[i] port_handle      handle to synch COM port
 * @param[i] reg_addr         reg addr to modify
 * @param[i] reg_value        value to write to register
 * @param[i] size             number of bytes to write
 * @param[o]  xfer_bytes      number of bytes transfered
 * @param[i] save_write_time  save write time input
 * @param[i] mask             bit mask to update
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dsm_read_modify_write(
    sns_sensor_instance *const instance,
    uint32_t reg_addr,
    uint8_t *reg_value,
    uint32_t size,
    uint32_t *xfer_bytes,
    bool save_write_time,
    uint8_t mask)
{
  uint8_t rw_buffer = 0;
  uint32_t rw_bytes = 0;

  sns_rc rc = SNS_RC_FAILED;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  sns_sync_com_port_handle* port_handle = state->com_port_info.port_handle;

  if((size > 1) || (mask == 0xFF) || (mask == 0x00))
  {
    rc = lsm6dsm_com_write_wrapper(instance,
                          reg_addr,
                          &reg_value[0],
                          size,
                          xfer_bytes,
                          save_write_time);

  }
  else
  {
    // read current value from this register
    rc = lsm6dsm_com_read_wrapper(state->scp_service,
                                  port_handle,
                                  reg_addr,
                                  &rw_buffer,
                                  1,
                                  &rw_bytes);
    if (SNS_RC_SUCCESS == rc)
    {
      // generate new value
      rw_buffer = (rw_buffer & (~mask)) | (*reg_value & mask);

      // write new value to this register
      rc = lsm6dsm_com_write_wrapper(instance,
                            reg_addr,
                            &rw_buffer,
                            1,
                            xfer_bytes,
                            save_write_time);
    }

  }

  if(rc != SNS_RC_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, instance, "read_modify_write %d", rc);
  }
#if LSM6DSM_DUMP_REG
  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "reg write 0x%x=0x%x mask=0x%x",reg_addr, *reg_value, mask);
#endif
  return rc;
}

void lsm6dsm_context_save(
    sns_sensor_instance *const this,
    uint8_t context_buffer[],
    uint8_t reg_map[],
    uint8_t reg_num)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)this->state->state;
  uint8_t i = 0;
  uint32_t xfer_bytes = 0;

  sns_rc rc = SNS_RC_FAILED;
  for(i=0; i < reg_num; i++) {

    // QC: Use one vectored transfer rather than multiple transfers.
    rc = lsm6dsm_com_read_wrapper(state->scp_service,
        state->com_port_info.port_handle,
        reg_map[i],
        &context_buffer[i],
        1,
        &xfer_bytes);

    if (SNS_RC_SUCCESS != rc)
    {
      SNS_INST_PRINTF(MED, this,"register reading register (%d), error (%d)", reg_map[i], rc);
      return;
    }
    if(xfer_bytes != 1)
    {
      SNS_INST_PRINTF(MED, this,"register reading bytes doesn't match (%d)", i);
      return;
    }
  }
}

void lsm6dsm_context_restore(
    sns_sensor_instance *const this,
    uint8_t context_buffer[],
    uint8_t reg_map[],
    uint8_t reg_num)
{
  uint8_t i = 0;
  uint32_t xfer_bytes = 0;
  for(i=0; i < reg_num; i++) {
    //SNS_INST_PRINTF(MED, "lsm6dsm_context_restore: %d-index:0x%x value:0x%x", i, reg_map[i], context_buffer[i]);
   // QC: Use one vectored transfer rather than multiple transfers.
    lsm6dsm_read_modify_write(this,
        reg_map[i],
        &context_buffer[i],
        1,
        &xfer_bytes,
        false,
        0xFF);
  }
}

/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_device_sw_reset(
    sns_sensor_instance *const instance,
    lsm6dsm_sensor_type sensor)
{
  UNUSED_VAR(sensor);
  uint8_t buffer[1];
  sns_rc rv = SNS_RC_SUCCESS;
  sns_time cur_time;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  sns_sync_com_port_handle* port_handle = state->com_port_info.port_handle;
  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "sw_rst");
  buffer[0] = 0x01;
  rv = lsm6dsm_com_write_wrapper(instance,
                             STM_LSM6DSM_REG_CTRL3,
                             &buffer[0],
                             1,
                             &xfer_bytes,
                             false);
  if(rv != SNS_RC_SUCCESS
     ||
     xfer_bytes != 1)
  {
    return rv;
  }

  cur_time = sns_get_system_time();
  do
  {
    if(sns_get_system_time() > (cur_time + sns_convert_ns_to_ticks(10*1000*1000)))
    {
      // Sensor HW has not recovered from SW reset.
      return SNS_RC_FAILED;
    }
    else
    {
      //1ms wait
      sns_busy_wait(sns_convert_ns_to_ticks(1*1000*1000));

      rv = lsm6dsm_com_read_wrapper(state->scp_service,
                                port_handle,
                                STM_LSM6DSM_REG_CTRL3,
                                &buffer[0],
                                1,
                                &xfer_bytes);

      if(rv != SNS_RC_SUCCESS)
      {
        // HW not ready. Keep going.
        DBG_INST_PRINT_EX(instance, ERROR, __FILENAME__, __LINE__,
                       "HW not ready %d", rv);
      }
      if(xfer_bytes != 1)
      {
        DBG_INST_PRINT_EX(instance, ERROR, __FILENAME__, __LINE__,
                       "read_wrapper %d, %d", rv, xfer_bytes);
        return SNS_RC_FAILED;
      }
    }

  } while((buffer[0] & 0x01));

  return SNS_RC_SUCCESS;
}

/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_device_set_default_state(
    sns_sensor_instance *const instance,
    lsm6dsm_sensor_type sensor)
{
  uint8_t buffer[1];
  sns_rc rv = SNS_RC_SUCCESS;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  UNUSED_VAR(state);
  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__, "set_default_state");

  if(sensor == LSM6DSM_ACCEL)
  {
    // reset Accel state only
  }
  else if(sensor == LSM6DSM_GYRO)
  {
    // reset Gyro state only
  }
  else if(sensor == (LSM6DSM_ACCEL | LSM6DSM_GYRO | LSM6DSM_MOTION_DETECT | LSM6DSM_SENSOR_TEMP))
  {

     // Configure control register 3
     buffer[0] = 0x0
       | (0<<7)           // BOOT bit
       | (0<<6)           // BDU bit
       | (0<<5)           // H_LACTIVE bit
       | (0<<4)           // PP_OD bit
       | (0<<3)           // SIM bit
       | (1<<2)           // IF_INC
       | (0<<1)           // BLE
       | 0;               // SW_RESET

     rv = lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_CTRL3,
                            &buffer[0],
                            1,
                            &xfer_bytes,
                            false,
                            0xFF);

     if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
     {
        return SNS_RC_FAILED;
     }

     //workaround enable HPF for XL here
     //initialize with high threshold
     buffer[0] = 0x3F;
     rv = lsm6dsm_read_modify_write(instance,
                         STM_LSM6DSM_REG_WAKE_THS,
                         &buffer[0],
                         1,
                         &xfer_bytes,
                         false,
                         0x3F);

     if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
     {
        return SNS_RC_FAILED;
     }

     buffer[0] = 0
       | (0<<7)            // LPF2_XL_EN: refer to figure 5.
       | (0<<5)            // HPCF_XL[1:0]: 0-SlopeFilter=ODR/50; 1-HP Filter=ODR/100; 2-HP Filter=ODR/9; 3-HP Filter=ODR/400.
       | (0<<2)            // HP_SLOPE_XL_EN:
       | 0;                // LOW_PASS_ON_6D:
     rv = lsm6dsm_read_modify_write(instance,
                         STM_LSM6DSM_REG_CTRL8_XL,
                         &buffer[0],
                         1,
                         &xfer_bytes,
                         false,
                         0xFF);

     if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
     {
        return SNS_RC_FAILED;
     }

     buffer[0] = 0
       | (1<<7)            // INTERRUPTS:
       | (0<<6)            // PEDO_EN:
       | (0<<5)            // TILT_EN:
       | (0<<4)            // SLOPE_FDS: refer to figure 5.
       | (0<<3)            // TAP_X_EN:
       | (0<<2)            // TAP_Y_EN:
       | (0<<1)            // TAP_Z_EN:
       | 0;                   // LIR: 0-interrupt not latched; 1-latched.

     rv = lsm6dsm_read_modify_write(instance,
                         STM_LSM6DSM_REG_TAP_CFG,
                         &buffer[0],
                         1,
                         &xfer_bytes,
                         false,
                         0x80);

     if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
     {
        return SNS_RC_FAILED;
     }

     buffer[0] = 0x0;
     rv = lsm6dsm_com_write_wrapper(instance,
                                STM_LSM6DSM_REG_CTRL9_A,
                                &buffer[0],
                                1,
                                &xfer_bytes,
                                false);

     if(rv != SNS_RC_SUCCESS || xfer_bytes != 1)
     {
        return SNS_RC_FAILED;
     }

     lsm6dsm_set_accel_config(instance,
                            LSM6DSM_ACCEL_ODR_OFF,
                            state->accel_info.sstvt,
                            state->accel_info.range,
                            LSM6DSM_ACCEL_BW50);

     lsm6dsm_set_gyro_config(instance,
                             LSM6DSM_GYRO_ODR_OFF,
                             state->gyro_info.sstvt,
                             state->gyro_info.range);

  }

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "odr a/g 0x%x/0x%x",
                 state->common_info.accel_curr_odr,state->common_info.gyro_curr_odr);

  return SNS_RC_SUCCESS;
}

/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_reset_device(
    sns_sensor_instance *const instance,
    lsm6dsm_sensor_type sensor)
{
  sns_rc rv = SNS_RC_SUCCESS;

  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__, "reset_device");
  /** HW reset only when both Accel and Gyro are requested for
   *  reset. */
  if( sensor == (LSM6DSM_ACCEL | LSM6DSM_GYRO | LSM6DSM_MOTION_DETECT | LSM6DSM_SENSOR_TEMP))
  {
     rv = lsm6dsm_device_sw_reset(instance, sensor);
  }

  if(rv == SNS_RC_SUCCESS)
  {
    rv = lsm6dsm_device_set_default_state(instance, sensor);
  }

  return rv;
}


/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_set_gyro_config(
    sns_sensor_instance *const instance,
    lsm6dsm_gyro_odr         curr_odr,
    lsm6dsm_gyro_sstvt       sstvt,
    lsm6dsm_gyro_range       range)
{
  UNUSED_VAR(sstvt);
  uint8_t buffer = (uint8_t)curr_odr | (uint8_t )range;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  state->common_info.gyro_curr_odr = curr_odr;
  if(!state->self_test_info.test_alive)
  {
    state->gyro_info.sstvt = sstvt;
  }
  state->common_info.gyro_odr_settime = sns_get_system_time();
  state->gyro_info.num_samples_to_discard = (lsm6dsm_odr_map[state->common_info.desired_odr_idx].odr * LSM6DSM_GYRO_ON_TIME_MS )/1000 +
  lsm6dsm_odr_map[state->common_info.desired_odr_idx].gyro_discard_samples;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "set_gyro_config: odr_settime=%u",
                 (uint32_t)state->common_info.gyro_odr_settime);

  return lsm6dsm_com_write_wrapper(instance,
                            STM_LSM6DSM_REG_CTRL2_G,
                            &buffer,
                            1,
                            &xfer_bytes,
                            false);
}

/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_set_accel_config(
   sns_sensor_instance *const instance,
                              lsm6dsm_accel_odr       curr_odr,
                              lsm6dsm_accel_sstvt     sstvt,
                              lsm6dsm_accel_range     range,
                              lsm6dsm_accel_bw        bw)
{
  UNUSED_VAR(sstvt);
  uint8_t buffer = (uint8_t)curr_odr | (uint8_t )range | (uint8_t)bw;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  state->common_info.accel_curr_odr = curr_odr;
  state->current_conf.odr = lsm6dsm_odr_map[state->common_info.desired_odr_idx].odr;
  if(!state->self_test_info.test_alive)
  {
    state->accel_info.sstvt = sstvt;
  }
  state->common_info.accel_odr_settime = sns_get_system_time();
  state->accel_info.num_samples_to_discard = lsm6dsm_odr_map[state->common_info.desired_odr_idx].accel_discard_samples;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "set_XL_config: c_odr=0x%x CTRL1_A=0x%x settime=%u(ticks)",
                 curr_odr, buffer, (uint32_t)state->common_info.accel_odr_settime);

  return lsm6dsm_com_write_wrapper(instance,
                            STM_LSM6DSM_REG_CTRL1_A,
                            &buffer,
                            1,
                            &xfer_bytes,
                            false);
}

/**
 * Updates GYRO_SLEEP bit.
 *
 * @param[i] port_handle   synch COM port handle
 * @param[i] set_sleep     true to enable Gyro sleep else false
 *
 * @return sns_rc
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dsm_set_gyro_sleep(
    sns_sensor_instance *const instance,
    bool set_sleep)
{
  uint8_t buffer;
  uint32_t xfer_bytes;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "set_gyro_sleep");
  if(set_sleep)
  {
    buffer = 0x40;
  }
  else
  {
    buffer = 0x0;
  }
  // Update Gyro Sleep state
  lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_CTRL4,
                            &buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x40);
  return SNS_RC_SUCCESS;
}

/**
 * Updates ACC Power mode  bit.
 *
 * @param[i] port_handle   synch COM port handle
 * @param[i] set_lp     true to enable acc low power else false high performance
 *
 * @return sns_rc
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dsm_set_acc_lpmode(
    sns_sensor_instance *const instance,
    bool set_lp)
{
  uint8_t buffer = (set_lp) ? 0x10 : 0x0;
  uint32_t xfer_bytes;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "set_lp=%d", set_lp);
  // Update Gyro Sleep state
  lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_CTRL6_G,
                            &buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x10);
  return SNS_RC_SUCCESS;
}

void lsm6dsm_powerdown_gyro(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;

  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__, "power down gyro");
  if(state->common_info.gyro_curr_odr) {
    lsm6dsm_set_gyro_config(this,
        state->gyro_info.desired_odr,
        state->gyro_info.sstvt,
        state->gyro_info.range);
  }

  if(state->gyro_info.is_in_sleep) {
    lsm6dsm_set_gyro_sleep(this, false);
    state->gyro_info.is_in_sleep = false;
  }
}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_stop_fifo_streaming(sns_sensor_instance *const instance)
{
  uint8_t rw_buffer = 0x00;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  if(state->fifo_info.fifo_rate > LSM6DSM_ACCEL_ODR_OFF) {
    lsm6dsm_com_write_wrapper(instance,
        STM_LSM6DSM_REG_FIFO_CTRL5,
        &rw_buffer,
        1,
        &xfer_bytes,
        false);
    state->fifo_info.fifo_rate = LSM6DSM_ACCEL_ODR_OFF;
    state->current_conf.fifo_odr = 0;
  }

  if(state->common_info.accel_curr_odr > LSM6DSM_ACCEL_ODR_OFF)
  {
    lsm6dsm_set_accel_config(instance,
        LSM6DSM_ACCEL_ODR_OFF,
        state->accel_info.sstvt,
        state->accel_info.range,
        state->accel_info.bw);
  }

  lsm6dsm_powerdown_gyro(instance);
  if(state->common_info.gyro_curr_odr > LSM6DSM_GYRO_ODR_OFF)
  {
    lsm6dsm_set_gyro_sleep(instance, true);
    state->gyro_info.is_in_sleep = true;
  }
  lsm6dsm_set_acc_lpmode(instance, true);

  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
                 "stop_fifo_streaming: odr a/g 0x%x/0x%x",
                 state->common_info.accel_curr_odr,state->common_info.gyro_curr_odr);
}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_set_fifo_config(sns_sensor_instance *const instance,
                             uint16_t desired_wmk,
                             lsm6dsm_accel_odr a_chosen_sample_rate,
                             lsm6dsm_gyro_odr g_chosen_sample_rate,
                             lsm6dsm_sensor_type sensor)
{
  UNUSED_VAR(g_chosen_sample_rate);
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
                 "set_fifo_config: sensor=0x%x sr=0x%x wm=%u",
                 sensor, a_chosen_sample_rate, desired_wmk);

  state->fifo_info.desired_wmk = desired_wmk;
  state->fifo_info.desired_fifo_rate = a_chosen_sample_rate;
  state->current_conf.fifo_odr = lsm6dsm_odr_map[state->common_info.desired_odr_idx].odr;
  if(sensor & LSM6DSM_ACCEL
     ||
     sensor & LSM6DSM_MOTION_DETECT)
  {
    state->accel_info.desired_odr = a_chosen_sample_rate;
//    state->accel_info.sstvt = LSM6DSM_ACCEL_SSTVT_8G;
//    state->accel_info.range = LSM6DSM_ACCEL_RANGE_8G;
    if(state->accel_info.desired_odr < LSM6DSM_ODR_1660)
    {
      state->accel_info.bw = LSM6DSM_ACCEL_BW50; // BW configuration bit to be unset for all ODRs till 833Hz
    }
    else
    {
      state->accel_info.bw = LSM6DSM_ACCEL_BW1600; //BW configuration bit to be set for 1.6Khz and greater ODRs
    }
  } else
    state->accel_info.desired_odr = LSM6DSM_ACCEL_ODR_OFF;

  if(sensor & LSM6DSM_GYRO)
  {
    //if gyro is enabled set acc desired rate as gyro
    state->accel_info.desired_odr = a_chosen_sample_rate;
    state->gyro_info.desired_odr = g_chosen_sample_rate;
//    state->gyro_info.range = STM_LSM6DSM_GYRO_RANGE_2000DPS;
//    state->gyro_info.sstvt = LSM6DSM_GYRO_SSTVT_2000DPS;
  } else
    state->gyro_info.desired_odr = LSM6DSM_GYRO_ODR_OFF;
}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_set_fifo_wmk(sns_sensor_instance *const instance)
{
  uint16_t wmk_words = 0;
  uint8_t wmkL = 0;
  uint8_t wmkH = 0;
  uint32_t xfer_bytes;
  uint8_t buffer;
  uint8_t decimation = 0;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "set_fifo_wmk: cur=%u des=%u",
                 state->fifo_info.cur_wmk, state->fifo_info.desired_wmk);

  state->fifo_info.cur_wmk = state->fifo_info.desired_wmk;
  // QC: How many things are current at the same time?
  //There's a fifo_info current watermark, and a current config watermark. Which one is more current?
  state->current_conf.wmk = state->fifo_info.desired_wmk;

  //convert samples to words
  if(((state->accel_info.desired_odr > LSM6DSM_ACCEL_ODR_OFF) && (state->fifo_info.fifo_enabled & LSM6DSM_ACCEL))
    || ((state->self_test_info.sensor == LSM6DSM_ACCEL)&&(state->self_test_info.test_alive)))
  {
    wmk_words = state->fifo_info.cur_wmk * 3;
    decimation |= 0x1;
  }

  if(((state->gyro_info.desired_odr > LSM6DSM_GYRO_ODR_OFF) && (state->fifo_info.fifo_enabled & LSM6DSM_GYRO))
    || ((state->self_test_info.sensor == LSM6DSM_GYRO)&&(state->self_test_info.test_alive)))
  {
    wmk_words += state->fifo_info.cur_wmk * 3;
    if((state->accel_info.desired_odr) && (!state->gyro_info.desired_odr) && (state->self_test_info.self_test_stage == LSM6DSM_SELF_TEST_STAGE_1))
    {
      wmk_words += state->fifo_info.cur_wmk * 2 * 3;
    }
    decimation |= 0x8;
  }

  state->current_conf.enabled_sensors = state->fifo_info.fifo_enabled & (LSM6DSM_ACCEL | LSM6DSM_GYRO);
  // Set Accel decimation to no decimation
  buffer = decimation;
  lsm6dsm_read_modify_write(instance,
                        STM_LSM6DSM_REG_FIFO_CTRL3,
                        &buffer,
                        1,
                        &xfer_bytes,
                        false,
                        0xFF);
  // Configure FIFO WM
  wmkL = wmk_words & 0xFF;
  lsm6dsm_read_modify_write(instance,
                        STM_LSM6DSM_REG_FIFO_CTRL1,
                        &wmkL,
                        1,
                        &xfer_bytes,
                        false,
                        0x0);

  wmkH = (wmk_words >> 8) & 0x0F;
  lsm6dsm_read_modify_write(instance,
                        STM_LSM6DSM_REG_FIFO_CTRL2,
                        &wmkH,
                        1,
                        &xfer_bytes,
                        false,
                        0xF);
  //report interval  * LSM6DSM_HEART_BEAT_ODR_COUNT
  state->heart_beat_timeout = sns_convert_ns_to_ticks(1000000000.0 /  state->desired_conf.odr) * LSM6DSM_HEART_BEAT_ODR_COUNT * (state->desired_conf.wmk==0 ? 1 : state->desired_conf.wmk);
  if((state->fifo_info.publish_sensors & (LSM6DSM_ACCEL | LSM6DSM_GYRO)) ||
       (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
  {
    lsm6dsm_inst_create_timer(instance, &state->timer_heart_beat_data_stream, state->heart_beat_timeout, true);
  }
  else
  {
    // Disable timer
    sns_sensor_util_remove_sensor_instance_stream(instance, &state->timer_heart_beat_data_stream);
  }
}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_start_fifo_streaming(sns_sensor_instance *const instance)
{
  // Enable FIFO Streaming
  // Enable Accel Streaming
  // Enable GYRO Streaming

  uint8_t rw_buffer = 0x00;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  sns_time now = sns_get_system_time();
  bool is_md_int_req = (state->md_info.enable_md_int && !(state->fifo_info.publish_sensors & LSM6DSM_ACCEL));

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "start_fifo_streaming: rate=0x%x last_ts=%u now=%u",
                 state->fifo_info.desired_fifo_rate,
                 (uint32_t)state->fifo_info.last_timestamp, (uint32_t)now);

  if(state->fifo_info.fifo_rate == LSM6DSM_ACCEL_ODR_OFF)
    state->fifo_info.last_timestamp = now;
  state->fifo_info.fifo_rate = state->fifo_info.desired_fifo_rate;

  if(state->current_conf.odr != state->desired_conf.odr) {
    //reset only if odr is changed
    //if odr is same as before, no need to recalculate sampling interval
    state->fifo_info.interrupt_cnt = 0;
    state->fifo_info.avg_sampling_intvl = (state->desired_conf.odr != 0) ?
      sns_convert_ns_to_ticks(1000000000.0f / state->desired_conf.odr) :
      0;
    state->fifo_info.avg_interrupt_intvl = state->fifo_info.avg_sampling_intvl * state->desired_conf.wmk;
  }

  //start streaming,stream mode
  rw_buffer = 0x06 | (uint8_t)(state->fifo_info.fifo_rate >> 1);
  lsm6dsm_read_modify_write(instance,
      STM_LSM6DSM_REG_FIFO_CTRL5,
      &rw_buffer,
      1,
      &xfer_bytes,
      false,
      0xFF);

  lsm6dsm_set_accel_config(instance,
      state->accel_info.desired_odr,
      state->accel_info.sstvt,
      state->accel_info.range,
      state->accel_info.bw);

  lsm6dsm_set_gyro_config(instance,
      state->gyro_info.desired_odr,
      state->gyro_info.sstvt,
      state->gyro_info.range);

  if(is_md_int_req && state->current_conf.md_enabled && state->gyro_info.desired_odr) {
    DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
                   "start_fifo_streaming EX:: re-enable MD interrupt to allow for settling time");
    lsm6dsm_set_md_intr(instance, false);
    lsm6dsm_set_md_config(instance, true);
    lsm6dsm_update_md_intr(instance, true, false);
  }

  if(state->gyro_info.is_in_sleep)
  {
    lsm6dsm_set_gyro_sleep(instance, false);
    state->gyro_info.is_in_sleep = false;
  }

  DBG_INST_PRINT(instance, MED, __FILENAME__, __LINE__,
                 "start_fifo_streaming EX:: a_odr = 0x%x g_odr = 0x%x",
                 state->common_info.accel_curr_odr,state->common_info.gyro_curr_odr);

}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_enable_fifo_intr(sns_sensor_instance *const instance,
                              lsm6dsm_sensor_type sensor)
{
  UNUSED_VAR(sensor);
  uint8_t rw_buffer = 0;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
                 "enable_fifo_intr: sensor=0x%x INT1_CTRL=0x%x",
                 sensor, state->fifo_info.int1_ctrl_reg);
  // TODO: With these changes following combos work with DAE enabled:
  //       1. accel only
  //       2. accel followed by gyro
  // Gyro standalone is not working in DAE case and needs further debug
  if(state->fifo_info.fifo_enabled && state->fifo_info.int1_ctrl_reg == 0)// && state->irq_info.irq_ready)
  {
    if(state->fifo_info.cur_wmk > 0)
    {
      if(state->fifo_info.fifo_enabled & LSM6DSM_ACCEL
         ||
         state->fifo_info.fifo_enabled & LSM6DSM_GYRO)
      {
        // Configure lsm6dsm FIFO control registers
        rw_buffer = 0x0
          | (0<<7)       // INT1 pedometer
          | (0<<6)       // INT1 sign_motion
          | (0<<5)       // INT1 FIFO_FULL flag
          | (1<<4)       // INT1 FIFO_OVR flag
          | (1<<3)       // INT1 FIFO_FTH flag
          | (0<<2)       // INT1 BOOT flag
          | (0<<1)       // INT1 DRDY_G flag
          | 0;           // INT1 DRDY_XL flag
        lsm6dsm_read_modify_write(instance,
                                  STM_LSM6DSM_REG_INT1_CTRL,
                                  &rw_buffer,
                                  1,
                                  &xfer_bytes,
                                  false,
                                  0x38);
        lsm6dsm_com_read_wrapper(state->scp_service,
                                 state->com_port_info.port_handle,
                                 STM_LSM6DSM_REG_INT1_CTRL,
                                 &state->fifo_info.int1_ctrl_reg,
                                 1,
                                 &xfer_bytes);
        DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
                 "INT1_CTRL=0x%x", state->fifo_info.int1_ctrl_reg);
      }
    }
    else
    {
      DBG_INST_PRINT(instance, ERROR, __FILENAME__, __LINE__,
                     "Not enable. WMK=0");
    }
  }
}

/**
 * see sns_lsm6dsm_hal.h
 */
/**
 * Read regs using sync com port
 *
 * @param[i] state              Instance state
 * @param[i] addr              address to read
 * @param[i] num_of_bytes       num of bytes to read
 * @param[o] buffer             status registers
 *
 * @return SNS_RC_SUCCESS if successful else SNS_RC_FAILED
 */
sns_rc lsm6dsm_read_regs_scp(sns_sensor_instance *const instance,
                                     uint8_t addr, uint16_t num_of_bytes, uint8_t *buffer)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  rc = lsm6dsm_com_read_wrapper(state->scp_service,
                       state->com_port_info.port_handle,
                       addr,
                       buffer,
                       num_of_bytes,
                       &xfer_bytes);

  if(rc != SNS_RC_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, instance,
                             "read_regs_scp fail %d", rc);
  }

  return rc;
}

void lsm6dsm_disable_fifo_intr(sns_sensor_instance *const instance)
{
  uint8_t rw_buffer = 0;
  uint32_t xfer_bytes;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "disable_fifo_intr");
  state->fifo_info.int1_ctrl_reg = 0;
  lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_INT1_CTRL,
                            &rw_buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x38);
}
//return no of bytes in fifo
sns_rc lsm6dsm_get_fifo_status(sns_sensor_instance *const instance, uint16_t* bytes_in_fifo, uint8_t* status_reg)
{
  sns_rc rc = SNS_RC_SUCCESS;
  //read fifo regs
  rc = lsm6dsm_read_regs_scp(instance, STM_LSM6DSM_REG_FIFO_STATUS1, 4, status_reg);

  if(rc != SNS_RC_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, instance,
        "read_fifo_status fail %d", rc);
    return rc;
  }
  // Calculate the number of bytes to be read
  uint16_t countH = status_reg[1] & 0x0F;
  uint16_t countL = status_reg[0] & 0xFF;
  *bytes_in_fifo =  (((countH << 8) & 0xFF00) | countL) * 2;
  if((!*bytes_in_fifo) && (status_reg[1] & 0x40)) {
    *bytes_in_fifo = 4096;
  }
  LSM6DSM_INST_DEBUG_TS(LOW, instance,
    "status_reg 0x%x 0x%x 0x%x 0x%x",status_reg[0], status_reg[1], status_reg[2], status_reg[3]);
  LSM6DSM_INST_DEBUG_TS(LOW, instance,
    "count 0x%x num_of_bytes %d pattern %d",(countH << 8) | countL, *bytes_in_fifo, ((status_reg[3] & 0x03) << 8) | (status_reg[2] & 0xFF));
  return rc;
}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_flush_fifo(sns_sensor_instance *const instance)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  if(state->fifo_info.interrupt_cnt < MAX_INTERRUPT_CNT) {
    LSM6DSM_INST_DEBUG_TS(HIGH, instance,
        "resetting int_cnt");
    state->fifo_info.interrupt_cnt = 0;
  }
  state->fifo_info.th_info.flush_req = true;
  if (!lsm6dsm_dae_if_flush_hw(instance)) {
    lsm6dsm_read_fifo_data(instance, sns_get_system_time(), true);
  }
}

/**
 * see sns_lsm6dsm_hal.h
 */
sns_rc lsm6dsm_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer)
{
  sns_rc rv = SNS_RC_SUCCESS;
  uint32_t xfer_bytes;

  rv = lsm6dsm_com_read_wrapper(scp_service,
                            port_handle,
                            STM_LSM6DSM_REG_WHO_AM_I,
                            buffer,
                            1,
                            &xfer_bytes);

  if(rv != SNS_RC_SUCCESS
     ||
     xfer_bytes != 1)
  {
    rv = SNS_RC_FAILED;
  }

  return rv;
}


/**
 * Updates temp sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
void lsm6dsm_set_polling_config(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;

  DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__,
      "set_polling_config: s_intvl=%u sr(*1000)=%d",
      (uint32_t)state->sensor_temp_info.sampling_intvl, (int)(state->sensor_temp_info.desired_sampling_rate_hz*1000));
  if(state->sensor_temp_info.sampling_intvl > 0)
  {
    lsm6dsm_start_sensor_temp_polling_timer(this);
  }
  else
  {
    state->sensor_temp_info.timer_is_active = false;
    state->sensor_temp_info.cur_sampling_rate_hz = 0;
  }
}

sns_rc lsm6dsm_recover_device(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  sns_rc rv = SNS_RC_SUCCESS;
  uint16_t reg_num = 0;
  uint8_t context_buffer[32] = {0};
  uint8_t reg_map[] = {
    STM_LSM6DSM_REG_FIFO_CTRL1,        //(0x06)
    STM_LSM6DSM_REG_FIFO_CTRL2,        //(0x07)
    STM_LSM6DSM_REG_FIFO_CTRL3,        //(0x08)
    STM_LSM6DSM_REG_FIFO_CTRL4,        //(0x09)
    STM_LSM6DSM_REG_FIFO_CTRL5,        //(0x0A)
    STM_LSM6DSM_REG_INT1_CTRL,         //(0x0D)
    STM_LSM6DSM_REG_INT2_CTRL,         //(0x0E)
    STM_LSM6DSM_REG_CTRL1_A,           //(0x10)
    STM_LSM6DSM_REG_CTRL2_G,           //(0x11)
    STM_LSM6DSM_REG_CTRL3,             //(0x12)
    STM_LSM6DSM_REG_CTRL4,             //(0x13)
    STM_LSM6DSM_REG_CTRL5,             //(0x14)
    STM_LSM6DSM_REG_CTRL6_G,           //(0x15)
    STM_LSM6DSM_REG_CTRL7_G,           //(0x16)
    STM_LSM6DSM_REG_CTRL8_XL,          //(0x17)
    STM_LSM6DSM_REG_CTRL9_A,           //(0x18)
    STM_LSM6DSM_REG_CTRL10,          //(0x19)
  };

  //Save Context
  reg_num = sizeof(reg_map)/sizeof(reg_map[0]);
  // QC: How are we making sure the context_buffer will not be written over the boundary?
  lsm6dsm_context_save(this, context_buffer, reg_map, reg_num);
  DBG_INST_PRINT_EX(this, MED, __FILENAME__, __LINE__, "Context saved: %d", reg_num);

  //Gyro set power down mode
  lsm6dsm_set_gyro_config(this,
                          LSM6DSM_GYRO_ODR_OFF,
                          state->gyro_info.sstvt,
                          state->gyro_info.range);

  //Set Accel in High Performance mode
  lsm6dsm_set_acc_lpmode(this, false);

  // Reset Sensor
  rv = lsm6dsm_reset_device(this,
      LSM6DSM_ACCEL | LSM6DSM_GYRO | LSM6DSM_MOTION_DETECT | LSM6DSM_SENSOR_TEMP);

  //Power up Accel if needed. It was powered down during reset_device
  lsm6dsm_set_accel_config(this,
      state->accel_info.desired_odr,
      state->accel_info.sstvt,
      state->accel_info.range,
      state->accel_info.bw);

  //Power up gyro if needed
  lsm6dsm_set_gyro_config(this,
      state->gyro_info.desired_odr,
      state->gyro_info.sstvt,
      state->gyro_info.range);

  //Restore context
  lsm6dsm_context_restore(this, context_buffer, reg_map, reg_num);

  //Resrt flags
  state->ascp_req_count = 0;

  if(state->fifo_info.reconfig_req)
    lsm6dsm_reconfig_fifo(this, false);

  state->fifo_info.last_timestamp = sns_get_system_time();

  return rv;
}


/**
 * Gets current Accel ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float lsm6dsm_get_accel_odr(lsm6dsm_accel_odr curr_odr)
{
  float odr = 0.0;
  int8_t idx;

  for(idx = 0; idx < ARR_SIZE(lsm6dsm_odr_map); idx++)
  {
    if(curr_odr == lsm6dsm_odr_map[idx].accel_odr_reg_value
       &&
       curr_odr != LSM6DSM_ACCEL_ODR_OFF)
    {
      odr = lsm6dsm_odr_map[idx].odr;
      break;
    }
  }

  return odr;
}

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time lsm6dsm_get_sample_interval(lsm6dsm_accel_odr curr_odr)
{
  sns_time  sample_interval = 0;
  float odr = lsm6dsm_get_accel_odr(curr_odr);

  if(odr > 0.0)
  {
    sample_interval = sns_convert_ns_to_ticks(1000000000.0f / odr);
  }

  return sample_interval;
}

/**
 * Extract a gyro sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_sample        The segment of fifo buffer that has the gyro sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] filter_delay       Sample filter delay in ticks.
 * @param[i] instance           The current lsm6dsm sensor instance
 * @param[i] event_service      Event service for sending out the gyro sample
 * @param[i] state              The state of the lsm6dsm sensor instance
 */
static void lsm6dsm_handle_gyro_sample(const uint8_t fifo_sample[6],
                                sns_time timestamp,
                                sns_time filter_delay,
                                sns_sensor_instance *const instance,
                                sns_event_service *event_service,
                                lsm6dsm_instance_state *state,
                                log_sensor_state_raw_info *log_gyro_state_raw_info)
{
  UNUSED_VAR(event_service);
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  const uint8_t* ip_raw = &fifo_sample[0];

  LSM6DSM_INST_DEBUG_TS(HIGH, instance, "status %d #_discard %d config_sensors %d",
      status, state->gyro_info.num_samples_to_discard, state->config_sensors);
  if(state->gyro_info.num_samples_to_discard != 0)
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
    state->gyro_info.num_samples_to_discard--;
  }
  /* protect config_sensors from being overwrited by HW INT
     during reconfig */
  else if(state->fifo_info.reconfig_req == false)
  {
    //data stabilized reset config sensors bit
    state->config_sensors &= ~LSM6DSM_GYRO;
  }

  if((status == SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH) || (state->config_sensors & LSM6DSM_GYRO)) {
      ip_raw = &fifo_sample[0];
      state->gyro_info.opdata_status = status;
    } else {
      ip_raw = &state->gyro_info.opdata_raw[0];
      status = state->gyro_info.opdata_status;
    }


  if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
  {

    vector3 opdata_cal;

    float ipdata[TRIAXIS_NUM] = {0};
    float opdata_raw[TRIAXIS_NUM] = {0};
    uint8_t i;

    for(i = 0; i < ARR_SIZE(ipdata); i++)
    {
      uint8_t j = i << 1;
      ipdata[i] =
        (int16_t)(((ip_raw[j+1] << 8) & 0xFF00) | ip_raw[j]) *
        ((state->gyro_info.sstvt *  PI) / 180.0f);
      state->gyro_info.opdata_raw[j] = ip_raw[j];
      state->gyro_info.opdata_raw[j+1] = ip_raw[j+1];
    }

    for(i = 0; i < ARR_SIZE(opdata_raw); i++)
    {
      opdata_raw[state->axis_map[i].opaxis] =
        (state->axis_map[i].invert ? -1.0 : 1.0) * ipdata[state->axis_map[i].ipaxis];
    }

    // factory calibration
    opdata_cal = sns_apply_calibration_correction_3(
        make_vector3_from_array(opdata_raw),
        make_vector3_from_array(state->gyro_registry_cfg.fac_cal_bias),
        state->gyro_registry_cfg.fac_cal_corr_mat);

#if LSM6DSM_AUTO_DEBUG
    if(status != SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE){
      SNS_INST_PRINTF(HIGH, instance, "ori gyro sample(*1000): %d, %d, %d",
                                     (int32_t)(opdata_cal.data[0]*1000),
                                     (int32_t)(opdata_cal.data[1]*1000),
                                     (int32_t)(opdata_cal.data[2]*1000));

    SNS_INST_PRINTF(HIGH, instance, "last_ts=%u count=%u",(uint32_t)timestamp, state->gyro_sample_counter);
               }
#endif

    pb_send_sensor_stream_event(instance,
        &state->gyro_info.suid,
        timestamp - filter_delay,
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
        status,
        opdata_cal.data,
        ARR_SIZE(opdata_cal.data),
        state->encoded_imu_event_len);
    LSM6DSM_INST_DEBUG_TS(HIGH, instance, "g_sample time=%u", (uint32_t)timestamp);

#if LSM6DSM_AUTO_DEBUG
    SNS_INST_PRINTF(HIGH, instance, "g_sample time=%u",(uint32_t)timestamp);
#endif
    state->gyro_sample_counter++;
    if(!(state->gyro_sample_counter % 100))
    {
      SNS_INST_PRINTF(HIGH, instance, "counter=%u", state->gyro_sample_counter);
    }
    // Log raw uncalibrated sensor data
    lsm6dsm_log_sensor_state_raw_add(
        log_gyro_state_raw_info,
        opdata_raw,
        LSM6DSM_NUM_AXES,
        timestamp - filter_delay,
        status);
  }
}

/**
 * Extract a accel sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_sample        The segment of fifo buffer that has the accel sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] filter_delay       Sample filter delay in ticks
 * @param[i] instance           The current lsm6dsm sensor instance
 * @param[i] event_service      Event service for sending out the gyro sample
 * @param[i] state              The state of the lsm6dsm sensor instance
 */
static void lsm6dsm_handle_accel_sample(const uint8_t fifo_sample[6],
                                sns_time timestamp,
                                sns_time filter_delay,
                                sns_sensor_instance *const instance,
                                sns_event_service *event_service,
                                lsm6dsm_instance_state *state,
                                log_sensor_state_raw_info *log_accel_state_raw_info)
{
  UNUSED_VAR(event_service);
  const uint8_t* ip_raw = &fifo_sample[0];
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  if(state->accel_info.num_samples_to_discard != 0)
  {
    status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
    state->accel_info.num_samples_to_discard--;
  }
  /* protect config_sensors from being overwrited by HW INT
     during reconfig */
  else if(state->fifo_info.reconfig_req == false)
  {
    //data stabilized reset config sensors bit
    state->config_sensors &= ~LSM6DSM_ACCEL;
  }

  if((status == SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH) || (state->config_sensors & LSM6DSM_ACCEL)) {
    ip_raw = &fifo_sample[0];
    state->accel_info.opdata_status = status;
  } else {
    ip_raw = &state->accel_info.opdata_raw[0];
    status = state->accel_info.opdata_status;
  }

  if(((state->fifo_info.reconfig_req == false) && (state->fifo_info.publish_sensors & LSM6DSM_ACCEL)) ||
     ((state->fifo_info.reconfig_req == true) && (state->fifo_info.inst_publish_sensors & LSM6DSM_ACCEL)) ||
     (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
  {

    vector3 opdata_cal;
    float ipdata[TRIAXIS_NUM] = {0};
    float opdata_raw[TRIAXIS_NUM] = {0};
    uint8_t i;

    for(i = 0; i < ARR_SIZE(ipdata); i++)
    {
      uint8_t j = i << 1;
      ipdata[i] =
        (int16_t)(((ip_raw[j+1] << 8) & 0xFF00) | ip_raw[j]) *
        state->accel_info.sstvt * G / 1000000;
      state->accel_info.opdata_raw[j] = ip_raw[j];
      state->accel_info.opdata_raw[j+1] = ip_raw[j+1];
    }

    for(i = 0; i < ARR_SIZE(opdata_raw); i++)
    {
      opdata_raw[state->axis_map[i].opaxis] =
        (state->axis_map[i].invert ? -1.0 : 1.0) * ipdata[state->axis_map[i].ipaxis];
    }

    // factory calibration
    opdata_cal = sns_apply_calibration_correction_3(
        make_vector3_from_array(opdata_raw),
        make_vector3_from_array(state->accel_registry_cfg.fac_cal_bias),
        state->accel_registry_cfg.fac_cal_corr_mat);

    LSM6DSM_INST_DEBUG_TS(HIGH, instance, "a_sample time=%u", (uint32_t)timestamp);
#if LSM6DSM_AUTO_DEBUG
    SNS_INST_PRINTF(HIGH, instance, "a_sample time=%u",(uint32_t)timestamp);
#endif
    state->accel_sample_counter++;
    if(!(state->accel_sample_counter % 100))
    {
      SNS_INST_PRINTF(HIGH, instance, "counter=%u", state->accel_sample_counter);
    }
#if LSM6DSM_DEBUG
    DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "ori accel sample(*1000): %d, %d, %d",
                                 (int32_t)(opdata_cal.data[0]*1000),
                                 (int32_t)(opdata_cal.data[1]*1000),
                                 (int32_t)(opdata_cal.data[2]*1000));
#endif

#if LSM6DSM_AUTO_DEBUG
    if(status != SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE)  {
      SNS_INST_PRINTF(HIGH, instance, "ori accel sample(*1000): %d, %d, %d",
                               (int32_t)(opdata_cal.data[0]*1000),
                               (int32_t)(opdata_cal.data[1]*1000),
                               (int32_t)(opdata_cal.data[2]*1000));
    SNS_INST_PRINTF(HIGH, instance, "last_ts=%u ",(uint32_t)timestamp, state->gyro_sample_counter);
    }
#endif
    pb_send_sensor_stream_event(instance,
        &state->accel_info.suid,
        timestamp - filter_delay,
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
        status,
        opdata_cal.data,
        ARR_SIZE(opdata_cal.data),
        state->encoded_imu_event_len);

    // Log raw uncalibrated sensor data
    lsm6dsm_log_sensor_state_raw_add(
        log_accel_state_raw_info,
        opdata_raw,
        LSM6DSM_NUM_AXES,
        timestamp - filter_delay,
        status);
  }
}

int64_t lsm6dsm_timestamps_correction(sns_sensor_instance *instance,
    uint16_t sample_sets,
    sns_time irq_time,
    sns_time last_ts,
    sns_time sample_interval_ticks)
{
  UNUSED_VAR(instance);
  int64_t ts_drift = irq_time - (last_ts + sample_sets * sample_interval_ticks);
  if(ts_drift == 0)
    return 0;
  //take 2% of sample_interval_ticks for all samples ts_drift
  //0.02 * sample_interval_ticks * sample_sets
  sns_time ts_catchup_min = 0.02f * sample_interval_ticks * sample_sets;
  int64_t ts_correction_drift;
  int64_t ts_correction = ts_catchup_min;

  if(ts_drift > 0) {
    ts_correction_drift = ts_drift - ts_catchup_min;
    //if > 0 use ts_catchup_min or use ts_drift as correction value
    if(ts_correction_drift < 0)
      ts_correction = ts_drift;
  }
  else if(ts_drift < 0) {
    ts_correction_drift = ts_drift + ts_catchup_min;
    // if < 0 use ts_catchup_min or use ts_drift as correction value
    if(ts_correction_drift > 0)
      ts_correction = ts_drift;
  }
  return ts_correction/sample_sets;
}
void lsm6dsm_process_fifo_data_buffer(
  sns_sensor_instance *instance,
  bool                gyro_enabled,
  sns_time            first_timestamp,
  sns_time            sample_interval_ticks,
  const uint8_t       *fifo_start,
  size_t              num_bytes
)
{
  uint16_t num_sample_sets = 0;
  uint32_t i;
  uint8_t offset = 0;
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)instance->state->state;
  sns_service_manager *service_manager = instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
    (sns_event_service*)service_manager->get_service(service_manager, SNS_EVENT_SERVICE);
  sns_diag_service* diag = state->diag_service;
  log_sensor_state_raw_info log_accel_state_raw_info, log_gyro_state_raw_info;
  sns_time timestamp;
  int64_t sample_time_correction = 0;
  if(fifo_start == NULL)
  {
    SNS_INST_PRINTF(ERROR,instance,
        "fifo_start NULL!");
    state->num_ascp_null_events++;
    return;
  }

  //Temporary debug variable. To be removed
  state->fifo_start_address = (uint8_t *)fifo_start;

  bool is_streaming = (!state->self_test_info.test_alive);
  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
      "test_alive=%d ascp_req_cnt=%d is_stream=%d",
      state->self_test_info.test_alive, state->ascp_req_count, is_streaming);

  LSM6DSM_INST_DEBUG_TS(HIGH, instance,
      "first_timestamp %u sample_interval_ticks %u",
      (uint32_t)first_timestamp, (uint32_t)sample_interval_ticks);

  // Allocate Sensor State Raw log packets for accel and gyro
  sns_memzero(&log_accel_state_raw_info, sizeof(log_accel_state_raw_info));
  log_accel_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
  log_accel_state_raw_info.diag = diag;
  log_accel_state_raw_info.instance = instance;
  log_accel_state_raw_info.sensor_uid = &state->accel_info.suid;
  log_accel_state_raw_info.log = NULL;
  offset = 5;

  if(gyro_enabled)
  {
    sns_memzero(&log_gyro_state_raw_info, sizeof(log_gyro_state_raw_info));
    log_gyro_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
    log_gyro_state_raw_info.diag = diag;
    log_gyro_state_raw_info.instance = instance;
    log_gyro_state_raw_info.sensor_uid = &state->gyro_info.suid;
    log_gyro_state_raw_info.log = NULL;
    offset = 11;
  }

  if(0 == state->fifo_info.last_timestamp)
  {
    state->fifo_info.last_timestamp = first_timestamp - sample_interval_ticks;
  }

  LSM6DSM_INST_DEBUG_TS(HIGH, instance,
      "validate timestamps start = %u sample_ticks = %u",
      (uint32_t)first_timestamp, (uint32_t)sample_interval_ticks);

  if((state->fifo_info.bh_info.interrupt_fired) &&
    (state->fifo_info.interrupt_cnt >= MAX_INTERRUPT_CNT)) {
    uint16_t total_sample_sets = num_bytes/(gyro_enabled ? 12 : 6);
    sample_time_correction = lsm6dsm_timestamps_correction(
    instance, total_sample_sets, state->fifo_info.bh_info.use_time, state->fifo_info.last_timestamp, sample_interval_ticks);
    sample_interval_ticks += sample_time_correction;
    first_timestamp += sample_time_correction;
  }
  if(is_streaming) {
    for(i = 0; i+offset < num_bytes; i += 6)
    {
      timestamp = first_timestamp + (num_sample_sets * sample_interval_ticks);

      if(timestamp <= state->fifo_info.bh_info.use_time) {

        if(gyro_enabled) {
          if(!num_sample_sets)
            lsm6dsm_log_sensor_state_raw_alloc(&log_gyro_state_raw_info, 0);

          // First sample belongs to Gyro
          lsm6dsm_handle_gyro_sample(&fifo_start[i],
              timestamp,
              sample_interval_ticks,
              instance,
              event_service,
              state,
              &log_gyro_state_raw_info);
          i += 6;
        }
        if((!num_sample_sets) &&
            ((state->fifo_info.publish_sensors & LSM6DSM_ACCEL) ||
             (state->accel_info.gated_client_present && !state->md_info.enable_md_int)))
          lsm6dsm_log_sensor_state_raw_alloc(&log_accel_state_raw_info, 0);

        lsm6dsm_handle_accel_sample(&fifo_start[i],
            timestamp,
            sample_interval_ticks / 2,
            instance,
            event_service,
            state,
            &log_accel_state_raw_info);
        state->fifo_info.last_timestamp = timestamp;
      }
      num_sample_sets++;
    }
  } else /* selftest */ {
    DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "Skip handle fifosample.");
    state->fifo_info.last_timestamp = sns_get_system_time();
  }

  // Skip logging data when self test is running or driver has not initiated any async read
  if(is_streaming)
  {
    if((state->fifo_info.publish_sensors & LSM6DSM_ACCEL) ||
        (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
    {
      lsm6dsm_log_sensor_state_raw_submit(&log_accel_state_raw_info, LSM6DSM_NUM_AXES, true);
    }
    if(gyro_enabled)
    {
      lsm6dsm_log_sensor_state_raw_submit(&log_gyro_state_raw_info, LSM6DSM_NUM_AXES, true);
    }
  }
  else
  {
    if(log_accel_state_raw_info.log != NULL)
    {
      DBG_INST_PRINT_EX(instance, ERROR, __FILENAME__, __LINE__,
          "Leaking accel log 0x%x", log_accel_state_raw_info.log);
    }
    if(gyro_enabled && log_gyro_state_raw_info.log != NULL)
    {
      DBG_INST_PRINT_EX(instance, ERROR, __FILENAME__, __LINE__,
          "Leaking gyro  log 0x%x", log_gyro_state_raw_info.log);
    }
  }
  LSM6DSM_INST_DEBUG_TS(HIGH, instance,
      "[use_time_drift]last_ts=%u use_time=%u drift=%d",
      (uint32_t)state->fifo_info.last_timestamp, (uint32_t)state->fifo_info.bh_info.use_time,
      (int32_t)(state->fifo_info.bh_info.use_time - state->fifo_info.last_timestamp));

  LSM6DSM_INST_DEBUG_TS(HIGH, instance,
      "[cur_time_drift]last_ts=%u cur_time=%u drift=%d",
      (uint32_t)state->fifo_info.last_timestamp, (uint32_t)sns_get_system_time(),
      (int32_t)(sns_get_system_time() - state->fifo_info.last_timestamp));
}

void lsm6dsm_process_com_port_vector(sns_port_vector *vector, void *user_arg)
{
  sns_sensor_instance *instance = (sns_sensor_instance *)user_arg;

  if(STM_LSM6DSM_REG_FIFO_DATA_OUT_L == vector->reg_addr)
  {
    //Vector contains a FIFO buffer read
    lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)instance->state->state;
    bool gyro_enabled = (state->common_info.gyro_curr_odr > 0);
    uint16_t num_sample_sets = gyro_enabled ? (vector->bytes / 12) : (vector->bytes / 6);
    //sns_time sampling_intvl = lsm6dsm_get_sample_interval(state->common_info.accel_curr_odr);
    sns_time sampling_intvl = state->fifo_info.bh_info.sampling_intvl;

#if LSM6DSM_AUTO_DEBUG
    SNS_INST_PRINTF(HIGH, instance, "c_wmk=%u vec->bytes=%u",state->fifo_info.cur_wmk, vector->bytes);
#endif
    LSM6DSM_INST_DEBUG_TS(LOW, instance,
        "c_wmk=%u vec->bytes=%u",
        state->fifo_info.cur_wmk, vector->bytes);
    if(num_sample_sets >= 1 && sampling_intvl > 0)
    {
      //sns_time first_timestamp =
      // state->fifo_info.interrupt_timestamp - sampling_intvl * (num_sample_sets - 1);
      sns_time first_timestamp =
        state->fifo_info.last_timestamp + sampling_intvl;
      LSM6DSM_INST_DEBUG_TS(LOW, instance,
          "sampling_intv=%u last_ts=%u first_ts=%u",
          (uint32_t)sampling_intvl, (uint32_t)state->fifo_info.last_timestamp, (uint32_t)first_timestamp);
#if LSM6DSM_AUTO_DEBUG
      SNS_INST_PRINTF(HIGH, instance,  "sampling_intv=%u last_ts=%u first_ts=%u",
          (uint32_t)sampling_intvl, (uint32_t)state->fifo_info.last_timestamp, (uint32_t)first_timestamp);
#endif
      // QC: In addition to taking the 1st timestamp, this should probably take an "n" and the "nth sample timestamp".
      // In normal cases (including late reads) N would be the watermark, and the nth sample timestamp would be the interrupt timestamp.
      // This will also prevent having to caluclate drifts and invent completely incorrect samples below.
      lsm6dsm_process_fifo_data_buffer(instance,
          gyro_enabled,
          first_timestamp,
          sampling_intvl,
          vector->buffer,
          vector->bytes);

      //check the drift w.r.t interrupt ts
      //in flush case fifo_info.interrupt_ts stores the time when flush started
      //in interrupt case fifo_info.interrupt_ts stores the time when int received
      //try end timestamp to be closer to this
      float mul = 0.95;
      int64_t ts_drift = state->fifo_info.bh_info.use_time - state->fifo_info.last_timestamp;

      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "last_ts=%u use_time=%u ts_drift=%d",
          (uint32_t)state->fifo_info.last_timestamp, (uint32_t)state->fifo_info.bh_info.use_time, (int32_t)ts_drift);

      // QC: This looks like a serious kludge. Please assign timestamps correctly rather than fabricating new samples.
      if((ts_drift > 0) && (ts_drift > mul * sampling_intvl)) {
        uint8_t buffer[12] = {0};
        if(gyro_enabled) {
          memcpy(&buffer[0], state->gyro_info.opdata_raw, 6);
          memcpy(&buffer[6], state->accel_info.opdata_raw, 6);
        } else
          memcpy(&buffer[0], state->accel_info.opdata_raw, 6);

        first_timestamp =
          state->fifo_info.last_timestamp + sampling_intvl;

        LSM6DSM_INST_DEBUG_TS(LOW, instance,
            "sampling_intv=%u last_ts=%u first_ts=%u",
            (uint32_t)sampling_intvl, (uint32_t)state->fifo_info.last_timestamp, (uint32_t)first_timestamp);
        lsm6dsm_process_fifo_data_buffer(instance,
            gyro_enabled,
            first_timestamp,
            sampling_intvl,
            buffer,
            (gyro_enabled) ? 12 : 6);
      }
    }
    else
    {
      LSM6DSM_INST_DEBUG_TS(ERROR, instance,
          "vector:: #bytes=%u intvl=%u", vector->bytes, sampling_intvl);
    }
  }
}


/** See lsm6dsm_hal.h */
void lsm6dsm_send_fifo_flush_done(sns_sensor_instance *const instance, sns_sensor_uid *suid)
{
  sns_service_manager *mgr = instance->cb->get_service_manager(instance);
  sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
  sns_time ts = sns_get_system_time();

  DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
      "flush");
  sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);
  event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
  event->event_len = 0;
  event->timestamp = ts;
  e_service->api->publish_event(e_service, instance, event, suid);

}
void lsm6dsm_align_fifo_data(sns_sensor_instance *const instance, uint16_t pattern_pos, uint16_t* num_of_bytes, bool is_over_run)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  UNUSED_VAR(is_over_run);
  bool gyro_enabled = (state->common_info.gyro_curr_odr > 0);
  uint8_t sample_size = (gyro_enabled) ? 12 : 6;
  //Limit pattern to between 0-5
  uint8_t pattern_pos_check = pattern_pos % (sample_size /2);
  uint8_t bytes_to_discard = sample_size - pattern_pos_check*2;
  uint8_t buffer_size = SNS_MAX(bytes_to_discard, 2);
  uint8_t buffer[buffer_size];
  sns_memset(buffer, 0, sizeof(buffer));

  if(*num_of_bytes > bytes_to_discard) {
    // Read the the  samples to discard
    lsm6dsm_read_regs_scp(instance, STM_LSM6DSM_REG_FIFO_DATA_OUT_L, bytes_to_discard, buffer);
    *num_of_bytes -= bytes_to_discard;
#if LSM6DSM_DEBUG
    //check pattern again after reading data
    if(*num_of_bytes >= sample_size) {
      lsm6dsm_read_regs_scp(instance, STM_LSM6DSM_REG_FIFO_STATUS3, 2, buffer);
      uint16_t pattern_pos = (((buffer[1] & 0x03) << 8) | (buffer[0] & 0xFF));
      DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                   "updated pattern:: %d num_of_bytes %d",pattern_pos, *num_of_bytes);
    }
#endif
  }
}

//estimate average sample time
void lsm6dsm_estimate_avg_st(sns_sensor_instance *const instance, sns_time irq_timestamp)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  sns_time sampling_intvl = lsm6dsm_get_sample_interval(state->common_info.accel_curr_odr);
  if(state->fifo_info.interrupt_cnt < MAX_INTERRUPT_CNT) {
    //calculate avg interrupt intervl
    if(state->fifo_info.interrupt_cnt < (MAX_INTERRUPT_CNT - 1)) {
      state->fifo_info.avg_interrupt_intvl = (irq_timestamp - state->fifo_info.last_timestamp);
      if((sampling_intvl < 0.9f * state->fifo_info.avg_interrupt_intvl/state->fifo_info.cur_wmk) ||
        (sampling_intvl > 1.1f * state->fifo_info.avg_interrupt_intvl/state->fifo_info.cur_wmk))
      {
        state->fifo_info.avg_interrupt_intvl = sampling_intvl * state->fifo_info.cur_wmk;
      }

      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "updated::avg_int_intvl=%d last_ts=%u irq_ts=%u",
          (uint32_t)state->fifo_info.avg_interrupt_intvl, (uint32_t)state->fifo_info.last_timestamp, (uint32_t)irq_timestamp);
    } else {

      uint32_t cur_int_delta = (uint32_t)(irq_timestamp - state->fifo_info.interrupt_timestamp);
      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "prev_int_ts=%u irq_ts=%u cur_int_delta = %d",
          (uint32_t)state->fifo_info.interrupt_timestamp,(uint32_t)irq_timestamp, cur_int_delta);
      if(state->fifo_info.interrupt_cnt  == (MAX_INTERRUPT_CNT - 1)) {
        state->fifo_info.avg_interrupt_intvl = cur_int_delta;
      } else {

#if 0 //moving avg not required if using H/W timestamps
        //use moving average
        int32_t avg_int_diff = (uint32_t)cur_int_delta - state->fifo_info.avg_interrupt_intvl;
        int32_t offset =  avg_int_diff/(state->fifo_info.interrupt_cnt+1);

        LSM6DSM_INST_DEBUG_TS(HIGH, instance,
            "avg_int_intl = %u avg_int_diff = %d offset = %d",
            (uint32_t)state->fifo_info.avg_interrupt_intvl, avg_int_diff, offset );

        state->fifo_info.avg_interrupt_intvl += offset ;
#endif
      }
    }
    state->fifo_info.avg_sampling_intvl = state->fifo_info.avg_interrupt_intvl/state->fifo_info.cur_wmk;
    state->fifo_info.interrupt_cnt++;

    LSM6DSM_INST_DEBUG_TS(HIGH, instance,
        "updated::avg_int_intvl=%u avg_sampling_intl=%u int_cnt=%u",
        state->fifo_info.avg_interrupt_intvl, (uint32_t)state->fifo_info.avg_sampling_intvl, state->fifo_info.interrupt_cnt);

  } else {
    //validate avg_sampling intvl
    //+- 20% of nominal sampling intvl
    if((state->fifo_info.avg_sampling_intvl < sampling_intvl * 0.8 ) || (state->fifo_info.avg_sampling_intvl > sampling_intvl * 1.2 )) {
      //how about starting 10% lower than nominal?
      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "resetting interrupt_cnt");
      state->fifo_info.avg_sampling_intvl = sampling_intvl;
      state->fifo_info.interrupt_cnt = 0;
    }
  }
  //state->fifo_info.prev_interrupt_timestamp = state->fifo_info.interrupt_timestamp;
  state->fifo_info.interrupt_timestamp = irq_timestamp;

  LSM6DSM_INST_DEBUG_TS(LOW, instance, "irq_ts=%u prev_irq=%u delta=%d",
      (uint32_t)irq_timestamp, (uint32_t)state->fifo_info.interrupt_timestamp,
      (int32_t)(irq_timestamp - state->fifo_info.interrupt_timestamp));
}

// QC: This function has grown so big. Maybe it's time to refactor it.

/** read fifo data after checking fifo int and use sync com port or async com port */
//if DAE is using interrupts, send interrupt timestamp
//if DAE is not using sensor interrupts, send the current time and set flush
void lsm6dsm_read_fifo_data(sns_sensor_instance *const instance, sns_time irq_timestamp, bool flush)
{

  uint8_t fifo_status[4] = {0, 0, 0, 0};
  uint8_t buffer[100];
  uint32_t enc_len;
  sns_rc rc = SNS_RC_SUCCESS;
  uint16_t num_of_bytes = 0;
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  sns_port_vector async_read_msg;
  bool gyro_enabled = (state->common_info.gyro_curr_odr > 0);
  uint8_t sample_size = (gyro_enabled) ? 12 : 6;
  sns_time cur_time = sns_get_system_time();

  if(flush)
    state->fifo_info.th_info.flush_req = true;


  // Read the FIFO Status register
  rc = lsm6dsm_get_fifo_status(instance, &num_of_bytes, fifo_status);
  // If samples size is more than MAX fifo wmk, read samples in multiples of MAX fifo
  if((num_of_bytes/sample_size) > LSM6DSM_MAX_FIFO) 
  {
    num_of_bytes = LSM6DSM_MAX_FIFO*sample_size;
  }  
  
  if(rc != SNS_RC_SUCCESS)
  {
    SNS_INST_PRINTF(ERROR, instance,
        "read_fifo_status FAIL");
    return;
  }
  //flush = true, its flush request use cur_time
  //flush = false, req could be interrupt or rechecking interrupt
  //-->check recheck_int flag to know whether rechecking or not
  //-->if rechecking -- use cur_time
  //-->if interrrupt -- use irq time

  if(num_of_bytes < sample_size) {
    DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
        "#bytes %u < 1pattern %u", num_of_bytes, sample_size);

    if(state->fifo_info.th_info.interrupt_fired) {
      if(state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_HIGH
          || state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_LOW)
      {
        lsm6dsm_send_interrupt_is_cleared_msg(instance);
      }
    }

    //reset th parameters
    state->fifo_info.th_info.interrupt_fired = false;
    //reset recheck_int flag
    state->fifo_info.recheck_int = false;

    if(state->ascp_req_count <= 0) {
      if(state->fifo_info.th_info.flush_client_present) {
        if((state->fifo_info.publish_sensors & LSM6DSM_ACCEL)||
          (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
        {
          lsm6dsm_send_fifo_flush_done(instance, &state->accel_info.suid);
        }
        if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
        {
          lsm6dsm_send_fifo_flush_done(instance, &state->gyro_info.suid);
        }
        state->fifo_info.th_info.flush_client_present = false;
      }
      state->fifo_info.th_info.flush_req = false;
      //if cur wmk = 1, we try to reconfig at interrupt context, so ignore if wmk=1
      if(state->fifo_info.reconfig_req) {
        //if only wmk is changing, do not follow normal sequence
        if((state->current_conf.odr == state->desired_conf.odr) &&
            (state->current_conf.enabled_sensors == state->desired_conf.enabled_sensors) &&
            (state->current_conf.wmk != state->desired_conf.wmk)) {
          lsm6dsm_set_fifo_wmk(instance);
          //enable interrupt
          lsm6dsm_enable_fifo_intr(instance, state->fifo_info.fifo_enabled);
          state->fifo_info.inst_publish_sensors = state->fifo_info.publish_sensors;
          state->fifo_info.reconfig_req = false;
          lsm6dsm_send_config_event(instance);
          return;
        } else if((state->current_conf.wmk != 1)||(state->fifo_info.bh_info.interrupt_fired)) {
          lsm6dsm_reconfig_fifo(instance, false);
        }
      }
      DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
          "read_fifo_data: reconf_req %d ", state->fifo_info.reconfig_req);
      return;
    }
  }

  if(state->fifo_info.recheck_int) {
    if((fifo_status[1] & 0x80) == 0) {
      state->fifo_info.recheck_int = false;
    }
  }
  if((state->fifo_info.th_info.interrupt_fired) && ((fifo_status[1] & 0x80)== 0)) {
    state->fifo_info.th_info.interrupt_fired = false;
  }
  if(state->fifo_info.th_info.interrupt_fired) {
    //ascp is already working
    if(state->ascp_req_count) {
      //if bh working on recheck interrupt
      if(state->fifo_info.bh_info.use_time > state->fifo_info.th_info.interrupt_ts) {
      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "interrupt is skipping as the same is handling: bh_use_time = %u cur_int_ts = %u",
          (uint32_t)state->fifo_info.bh_info.use_time, (uint32_t)state->fifo_info.th_info.interrupt_ts);
        state->fifo_info.th_info.interrupt_fired = false;
        if(!state->fifo_info.bh_info.flush_req)
          state->fifo_info.bh_info.use_time = state->fifo_info.th_info.interrupt_ts;
      }
    }
  }
  if((!state->fifo_info.recheck_int) && (!state->fifo_info.th_info.interrupt_fired) &&
      (!state->fifo_info.th_info.flush_req)) {
    //return nothing to be done here
    //useful for active_high/active_low interrupt handling
    return;
  }

  uint16_t num_sample_sets = num_of_bytes / sample_size;
  if((!state->fifo_info.th_info.interrupt_fired) ||
      (num_sample_sets > state->fifo_info.cur_wmk)) {
    if(state->fifo_info.interrupt_cnt < MAX_INTERRUPT_CNT) {
      LSM6DSM_INST_DEBUG_TS(HIGH, instance,
          "resetting interrupt_cnt");
      state->fifo_info.interrupt_cnt = 0;
      state->fifo_info.avg_interrupt_intvl = lsm6dsm_get_sample_interval(state->common_info.accel_curr_odr);
    }
  } else if(!state->fifo_info.recheck_int) {
    lsm6dsm_estimate_avg_st(instance, irq_timestamp);
  }

    //do not need this as we can use the cur time when sending async req
    //state->fifo_info.th_info.flush_ts = cur_time;
  if(state->fifo_info.recheck_int) {
    //if comes here means interrupt is fired
    //enters here only no request pending at ascp
    if(!state->fifo_info.th_info.interrupt_fired) {
      state->fifo_info.th_info.interrupt_fired = true;
      state->fifo_info.th_info.interrupt_ts = cur_time;
    } else if(num_sample_sets > state->fifo_info.cur_wmk) {
      state->fifo_info.th_info.interrupt_ts = cur_time;
    }
  } else {
    //enters here allways for active high/log interrupts, if set recheck_int = false
    if(fifo_status[1] & 0x80) {
      if(num_sample_sets > state->fifo_info.cur_wmk)
        state->fifo_info.th_info.interrupt_ts = cur_time;
    }
  }
  uint16_t pattern_pos = (((fifo_status[3] & 0x03) << 8) | (fifo_status[2] & 0xFF));
  pattern_pos  %= (sample_size /2);

  if(pattern_pos && num_of_bytes) {
    lsm6dsm_align_fifo_data(instance, pattern_pos, &num_of_bytes, (fifo_status[1] & 0x40)) ;
  }

  //sns_time sampling_intvl = lsm6dsm_get_sample_interval(state->common_info.accel_curr_odr);
  sns_time sampling_intvl = state->fifo_info.avg_sampling_intvl;

  DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
      "#bytes %d size %d", num_of_bytes, sample_size);

  num_of_bytes = (num_of_bytes / sample_size ) * sample_size;
  num_sample_sets = num_of_bytes / sample_size;

  DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
      "#bytes %d num_sample_set %d", num_of_bytes, num_sample_sets);

  if(state->ascp_req_count) {
    DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
        "ascpreq pending. reqcount %d", state->ascp_req_count);
    return;
  }
  if((num_sample_sets == state->fifo_info.cur_wmk) && (state->fifo_info.th_info.interrupt_fired))
    state->fifo_info.bh_info.use_time = state->fifo_info.th_info.interrupt_ts;
  else if((num_sample_sets > state->fifo_info.cur_wmk) && (state->fifo_info.cur_wmk == 1))
      state->fifo_info.bh_info.use_time = state->fifo_info.last_timestamp + sampling_intvl * num_sample_sets;
  else
    //TODO: Use something like this:
    // if( num_sample_sets > cur_wmk )
    // {
    //   use_time = interrupt_ts +
    //    (samp_intvl * (num_sample_sets - cur_wmk);
    // }
    state->fifo_info.bh_info.use_time = cur_time;

  if((num_sample_sets <= state->fifo_info.cur_wmk) && (state->fifo_info.bh_info.use_time  > cur_time))
    state->fifo_info.bh_info.use_time = cur_time;

  state->fifo_info.bh_info.sampling_intvl = sampling_intvl;

  state->fifo_info.bh_info.interrupt_fired = state->fifo_info.th_info.interrupt_fired ;
  state->fifo_info.bh_info.flush_req = state->fifo_info.th_info.flush_req;
  state->fifo_info.bh_info.flush_client_present = state->fifo_info.th_info.flush_client_present;
  //reset th parameters
  state->fifo_info.th_info.interrupt_fired = false;
  state->fifo_info.th_info.flush_req = false;
  state->fifo_info.th_info.flush_client_present = false;
  state->fifo_info.recheck_int = false;

  LSM6DSM_INST_DEBUG_TS(LOW, instance,
    "bh_info int_fired = %d flush_req = %d sampling_intvl = %u",
    state->fifo_info.bh_info.interrupt_fired, state->fifo_info.bh_info.flush_req, (uint32_t)(state->fifo_info.bh_info.sampling_intvl));
  LSM6DSM_INST_DEBUG_TS(LOW, instance,
    "cur_time = %u irq_ts = %u use_time = %u",
    (uint32_t)sns_get_system_time(), (uint32_t)state->fifo_info.bh_info.interrupt_ts, (uint32_t)state->fifo_info.bh_info.use_time);


#if LSM6DSM_AUTO_DEBUG
  SNS_INST_PRINTF(HIGH, instance, "bh_info int_fired = %d flush_req = %d sampling_intvl = %u",state->fifo_info.bh_info.interrupt_fired, state->fifo_info.bh_info.flush_req, (uint32_t)(state->fifo_info.bh_info.sampling_intvl));
#endif
 // Compose the async com port message
  async_read_msg.bytes = num_of_bytes;
  async_read_msg.reg_addr = STM_LSM6DSM_REG_FIFO_DATA_OUT_L;
  async_read_msg.is_write = false;
  async_read_msg.buffer = NULL;

  //if samples > ASYNC_MIN_SAMPLES use async com
  //else use sync com

  // QC: One set is 2 samples when Gyro is active, so we're OK using Sync com port for 60 bytes?
  if(num_sample_sets  <= ASYNC_MIN_SAMPLES) {
    uint8_t fifo_data[async_read_msg.bytes];
    // QC: is memset necessary?
    sns_memset(fifo_data, 0, sizeof(fifo_data));
    rc = lsm6dsm_read_regs_scp(instance, async_read_msg.reg_addr, async_read_msg.bytes, fifo_data);
    if(rc == SNS_RC_SUCCESS) {
      sns_port_vector vector;
      vector.reg_addr = async_read_msg.reg_addr;
      vector.bytes = async_read_msg.bytes;
      vector.buffer = fifo_data;
      lsm6dsm_process_com_port_vector(&vector, instance);

      if(state->fifo_info.bh_info.interrupt_fired) {
        if(state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_HIGH
            || state->irq_info.irq_config.interrupt_trigger_type == SNS_INTERRUPT_TRIGGER_TYPE_LOW)
        {
          lsm6dsm_send_interrupt_is_cleared_msg(instance);
        }
      }

      if(state->fifo_info.bh_info.flush_client_present) {
        if((state->fifo_info.publish_sensors & LSM6DSM_ACCEL) ||
         (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
        {
          lsm6dsm_send_fifo_flush_done(instance, &state->accel_info.suid);
        }
        if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
        {
          lsm6dsm_send_fifo_flush_done(instance, &state->gyro_info.suid);
        }
        state->fifo_info.bh_info.flush_client_present = false;
      }
      if(state->fifo_info.reconfig_req) {
        //if only wmk is changing, do not follow normal sequence
        if((state->current_conf.odr == state->desired_conf.odr) &&
            (state->current_conf.enabled_sensors == state->desired_conf.enabled_sensors) &&
            (state->current_conf.wmk != state->desired_conf.wmk)) {
          lsm6dsm_set_fifo_wmk(instance);
          lsm6dsm_enable_fifo_intr(instance, state->fifo_info.fifo_enabled);
          state->fifo_info.inst_publish_sensors = state->fifo_info.publish_sensors;
          state->fifo_info.reconfig_req = false;
          lsm6dsm_send_config_event(instance);
          return;
        } else if(state->fifo_info.bh_info.interrupt_fired)  {
          //reconfigures at interrupt boundary, do we need to check wmk too?
          lsm6dsm_reconfig_fifo(instance, false);
        }
      }
    }
  }

  else if(sns_ascp_create_encoded_vectors_buffer(&async_read_msg, 1, true, buffer, sizeof(buffer), &enc_len)) {

    // Send message to Async Com Port
    sns_request async_com_port_request =
      (sns_request)
      {
        .message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW,
        .request_len = enc_len,
        .request = buffer
      };
    rc = state->async_com_port_data_stream->api->send_request(
        state->async_com_port_data_stream, &async_com_port_request);
    if(rc != SNS_RC_SUCCESS) {
      DBG_INST_PRINT(instance, ERROR, __FILENAME__, __LINE__,
                   "ascp sendreq fail %d", rc);
    }
    DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
        "ascp_req_cnt %d", state->ascp_req_count);
    state->ascp_req_count++;
  } else {
    DBG_INST_PRINT(instance, ERROR, __FILENAME__, __LINE__,
                   "sns_ascp_create_encoded_vectors_buffer fail");
  }
}

/**
 * Extract a accel sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] instance           The current lsm6dsm sensor instance
 * @param[i] sensors[]          Array of sensors for which data is requested
 * @param[i] num_sensors        Number of sensor for which data is requested
 * @param[i] raw_data           Uncalibrated sensor data to be logged
 */
void lsm6dsm_get_data(sns_sensor_instance *const instance,
                                lsm6dsm_sensor_type sensors[],
                                uint8_t num_sensors,
                                float *raw_data)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)instance->state->state;

// Timestap is not needed for this implementation as we are not sending anything ot framework
  uint8_t reg_sample[6*num_sensors];
  uint8_t read_addr;
  uint32_t xfer_bytes;
  float data_conv;
  float gyro_conv = state->gyro_info.sstvt;
  float accel_conv = state->accel_info.sstvt * G/1000000;

  if(state->self_test_info.test_alive)
  {
    // Self tests run at hardcoaded values 2000DPS(gyro) and 2G(accel)
    gyro_conv = LSM6DSM_GYRO_SSTVT_2000DPS; // Value from ddf driver
    accel_conv = LSM6DSM_ACCEL_SSTVT_2G * G/1000000;
  }

  sns_memset(reg_sample, 0, sizeof(reg_sample));
  if((num_sensors == 2)||(sensors[0] == LSM6DSM_GYRO))
    read_addr = STM_LSM6DSM_REG_OUT_X_L_G;
  else
    read_addr = STM_LSM6DSM_REG_OUT_X_L_XL;

  lsm6dsm_com_read_wrapper(state->scp_service,
      state->com_port_info.port_handle,
      read_addr,
      reg_sample,
      6*num_sensors,
      &xfer_bytes);

  if(sensors[0] == LSM6DSM_GYRO) { //Check which is the 1st sensor
    data_conv = gyro_conv;
  } else {
    data_conv = accel_conv;
  }
  raw_data[0] =
     (int16_t)(((reg_sample[1] << 8) & 0xFF00) | reg_sample[0]) * data_conv ;
  raw_data[1] =
     (int16_t)(((reg_sample[3] << 8) & 0xFF00) | reg_sample[2]) * data_conv ;
  raw_data[2] =
     (int16_t)(((reg_sample[5] << 8) & 0xFF00) | reg_sample[4]) * data_conv ;
  LSM6DSM_INST_DEBUG_TS(HIGH, instance, "DATA timestamp(LSB) = %u sensor = %d",
                                 sns_get_system_time(), sensors[0]);
#if LSM6DSM_DEBUG
  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "Data - x0 = %d x1 = %d x2 = %d",
                                 (int32_t)(raw_data[0]*1000),
                                 (int32_t)(raw_data[1]*1000),
                                 (int32_t)(raw_data[2]*1000));
#endif

  if(num_sensors == 2) { //both accel and gyro data requested
    if(sensors[1] == LSM6DSM_GYRO) { //Check which is the 2nd sensor
      data_conv = gyro_conv;
    } else {
      data_conv = accel_conv;
    }
    raw_data[3] =
       (int16_t)(((reg_sample[7] << 8) & 0xFF00) | reg_sample[6]) * data_conv;
    raw_data[4] =
       (int16_t)(((reg_sample[9] << 8) & 0xFF00) | reg_sample[8]) * data_conv;
    raw_data[5] =
       (int16_t)(((reg_sample[11] << 8) & 0xFF00) | reg_sample[10]) * data_conv;

    LSM6DSM_INST_DEBUG_TS(HIGH, instance, "DATA timestamp = %llu sensor = %d",
                                    sns_get_system_time(), sensors[1]);
#if LSM6DSM_DEBUG
    DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__, "Data - x3 = %d x4 = %d x5 = %d",
                                    (int32_t)(raw_data[3]*1000), (int32_t)(raw_data[4]*1000), (int32_t)(raw_data[5]*1000));
#endif
  }

}

/**
 * see sns_lsm6dsm_hal.h
 */
void lsm6dsm_dump_reg(sns_sensor_instance *const instance,
                      lsm6dsm_sensor_type sensor)
{
  UNUSED_VAR(sensor);
#if LSM6DSM_DUMP_REG
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)instance->state->state;
  struct group_read {
    uint32_t first_reg;
    uint8_t  num_regs;
  } groups[] = { /* must fit within state->reg_status[] */
    { STM_LSM6DSM_REG_FIFO_CTRL1, 5 },
    { STM_LSM6DSM_REG_INT1_CTRL, 15 },
    { STM_LSM6DSM_REG_TAP_CFG,    8 }
  };
  uint32_t xfer_bytes;
  uint8_t *reg_val = state->reg_status;

  for(uint32_t i=0; i<ARR_SIZE(groups); i++)
  {
    lsm6dsm_com_read_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             groups[i].first_reg,
                             reg_val,
                             groups[i].num_regs,
                             &xfer_bytes);
    for(uint32_t j=0; j<groups[i].num_regs; j++)
    {
      DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
                   "dump:: 0x%02x = 0x%02x", groups[i].first_reg+j, reg_val[j]);
    }
    reg_val += groups[i].num_regs;
  }
#else
  UNUSED_VAR(instance);
#endif
}

void lsm6dsm_set_gated_accel_config(lsm6dsm_instance_state *state,
                      uint16_t desired_wmk,
                      lsm6dsm_accel_odr a_chosen_sample_rate,
                      lsm6dsm_sensor_type sensor)
{
  if(!state->accel_info.gated_client_present)
  {
    desired_wmk = 0;
    a_chosen_sample_rate = LSM6DSM_ACCEL_ODR_OFF;
  }

  if(sensor & LSM6DSM_ACCEL)
  {
    state->md_info.desired_wmk = desired_wmk;
    state->md_info.desired_odr = a_chosen_sample_rate;
    state->md_info.sstvt = state->accel_info.sstvt;
    state->md_info.range = state->accel_info.range;
    state->md_info.bw = LSM6DSM_ACCEL_BW50;
  }
}
void lsm6dsm_set_md_filter(sns_sensor_instance *const instance,
                            lsm6dsm_md_filter_type filter)
{
  lsm6dsm_instance_state *inst_state =
    (lsm6dsm_instance_state*)instance->state->state;
  uint32_t xfer_bytes;
  lsm6dsm_md_filter_type  md_filter = (filter ==  SLOPE_FILTER) ? 0x0 : 0x10;
  if (inst_state->md_info.filter != filter) {
    lsm6dsm_read_modify_write(instance,
        STM_LSM6DSM_REG_TAP_CFG,
        &md_filter,
        1,
        &xfer_bytes,
        false,
        0x10);
    inst_state->md_info.filter = filter;
  }
}

void lsm6dsm_set_md_intr(sns_sensor_instance *const instance,
                            bool enable)
{
  //update the reg only if necessary
  //save some cycles
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;

  uint8_t rw_buffer = 0;
  uint32_t xfer_bytes;
  bool update_reg = false;
  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "set_md_intr: %u %u %u",
                 enable, state->md_info.is_filter_settled,
                 state->md_info.cur_md_state.motion_detect_event_type);
  if((enable) &&
      (state->md_info.cur_md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED))
  {
    rw_buffer = 0
      | (0<<7)            // INT1_INACT_STATE
      | (0<<6)            // INT1_SINGLE_TAP
      | (1<<5)            // INT1_WU
      | (0<<4)            // INT1_FF
      | (0<<3)            // INT1_DOUBLE_TAP
      | (0<<2)            // INT1_6D
      | (0<<1)            // INT1_TILT
      | 0;                // INT1_TIMER
    update_reg = true;
    // QC: Pull out of if statement, since both the if and else cases have the same code.
    state->current_conf.md_enabled = enable;
  } else if((!enable) &&
      (state->md_info.is_filter_settled)) {
    update_reg = true;
    state->current_conf.md_enabled = enable;
  }

  if(update_reg)
    lsm6dsm_read_modify_write(instance,
        STM_LSM6DSM_REG_MD1_CFG,
        &rw_buffer,
        1,
        &xfer_bytes,
        false,
        0x20);
}



bool is_lsm6dsm_mdf_settled(sns_sensor_instance *this, lsm6dsm_md_filter_type filter)
{
  lsm6dsm_instance_state *inst_state =
    (lsm6dsm_instance_state*)this->state->state;
  sns_time cur_time, odr_time_elapsed, filter_settling_time;
  uint64_t settling_time_ns = 0;
  int64_t req_timeout = 0;

  if(inst_state->common_info.accel_curr_odr == LSM6DSM_ACCEL_ODR_OFF) {
    DBG_INST_PRINT_EX(this, LOW, __FILENAME__, __LINE__,
                   "accel on");
  }
  if(filter == SLOPE_FILTER) {
    settling_time_ns = SLOPE_SETTLING_TIME_NS ;
  } else if(filter == HP_FILTER) {
    settling_time_ns = HPF_SETTLING_TIME_NS;
  }
  //filter_settling_time = (samples_to_settle*1000000)/inst_state->common_info.accel_curr_odr;
  //get sample interval -- not possible at sensor level, use minimum 13Hz
  filter_settling_time = sns_convert_ns_to_ticks(settling_time_ns); // in ticks
  cur_time = sns_get_system_time();
  odr_time_elapsed = cur_time - inst_state->common_info.accel_odr_settime;
  req_timeout = (int64_t) (filter_settling_time - odr_time_elapsed);

  DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
                 "filter %d, cur_t %u, set_t %u/%u (ns/ticks)",
                 filter, (uint32_t)cur_time, (uint32_t)settling_time_ns, (uint32_t)filter_settling_time);
  DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
                 "a_odr_set_t %u elapsed %u req_t/o %u",
                 (uint32_t)inst_state->common_info.accel_odr_settime, (uint32_t)odr_time_elapsed,
                 (uint32_t)req_timeout);

  //dump registers
  //lsm6dsm_dump_reg(this, inst_state->fifo_info.fifo_enabled);
  if(req_timeout > 0) {
    lsm6dsm_inst_create_timer(this, &inst_state->timer_md_data_stream, req_timeout, false); //Settling time for ODR
    return false;
  } else
    return true;
}
// to set the correct md filter and enable md interrupt
void lsm6dsm_update_md_filter(sns_sensor_instance *const this)
{
  lsm6dsm_instance_state *inst_state = (lsm6dsm_instance_state*)this->state->state;

  lsm6dsm_md_filter_type filter;
  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
                 "mdf request: %u %u",
                 inst_state->md_info.md_client_present,
                 inst_state->md_info.cur_md_state.motion_detect_event_type);
  if(inst_state->md_info.cur_md_state.motion_detect_event_type == SNS_MOTION_DETECT_EVENT_TYPE_ENABLED) {
    //default is slope filter
    filter = SLOPE_FILTER;
    inst_state->md_info.is_filter_settled = false;
    if(is_lsm6dsm_mdf_settled(this, SLOPE_FILTER)) {
      inst_state->md_info.is_filter_settled = true;
      if(is_lsm6dsm_mdf_settled(this, HP_FILTER)) {
        filter = HP_FILTER;
      }
    }
    if(inst_state->md_info.is_filter_settled) {
      lsm6dsm_set_md_intr(this, true);
    }
    lsm6dsm_set_md_filter(this, filter);
  } else {
    // ignore the request

  }
}


void lsm6dsm_set_md_config(sns_sensor_instance *const instance, bool enable)
{
  lsm6dsm_instance_state *state =
    (lsm6dsm_instance_state*)instance->state->state;
  uint8_t rw_buffer = 0;
  uint8_t dur_set = 0x60; // wake up duration bits 5,6 set to 1
  uint8_t thresh_set = 0x3F; // thresh bits 0 to 5 set to 1
  uint32_t xfer_bytes;
  lsm6dsm_accel_odr accel_odr = state->accel_info.desired_odr;
  if(enable && (accel_odr == LSM6DSM_ACCEL_ODR_OFF))
    accel_odr = LSM6DSM_MD_ODR;

  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
                 "set_md_config: en=%d des_odr(md/a)=0x%x/0x%x",
                 enable, state->md_info.desired_odr, state->accel_info.desired_odr);
  DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
                 "md_thd(*1000)=%d win(*1000)=%d",
                 (int)(state->md_info.md_config.thresh *1000) , (int)(state->md_info.md_config.win*1000));

  if(enable && state->md_info.md_config.thresh <= LSM6DSM_MD_THRESH_MAX &&
     state->md_info.md_config.thresh >= 0.0)
  {
    // Threshold - bits 0 to 5 of STM_LSM6DSM_REG_WAKE_THS
    thresh_set &= ((uint8_t)((roundf)(state->md_info.md_config.thresh /
                                    LSM6DSM_REG_WAKE_THS_RES)) | 0xC0);
  }

  if(state->md_info.md_config.win <= 0x3 / LSM6DSM_MD_ODR_VAL
     && state->md_info.md_config.win >= 0.0)
  {
    // Wake Up Duration - bits 5,6 of STM_LSM6DSM_REG_WAKE_DUR
    dur_set &= (((uint8_t)((roundf)(state->md_info.md_config.win *
                                    LSM6DSM_MD_ODR_VAL)) << 5) | 0x9F);
  }

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "th_set=0x%x dur_set=0x%x",
                 thresh_set, dur_set);
  rw_buffer = !enable ? 0x3F : thresh_set;
  lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_WAKE_THS,
                            &rw_buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x3F);

  rw_buffer = dur_set;
  lsm6dsm_read_modify_write(instance,
                            STM_LSM6DSM_REG_WAKE_DUR,
                            &rw_buffer,
                            1,
                            &xfer_bytes,
                            false,
                            0x60);
  if(accel_odr != state->common_info.accel_curr_odr) {
    lsm6dsm_set_accel_config(instance,
        accel_odr,
        state->accel_info.sstvt,
        state->accel_info.range,
        LSM6DSM_ACCEL_BW50);
  }

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "a_odr=0x%x cur_odr(a/g)=0x%x/0x%x",
                 accel_odr, state->common_info.accel_curr_odr, state->common_info.gyro_curr_odr);
}


void lsm6dsm_update_md_intr(sns_sensor_instance *const instance,
                            bool enable,
                            bool md_not_armed_event)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
                 "update_md_intr: %d %d",
                 enable, md_not_armed_event);

  state->md_info.prev_md_state.motion_detect_event_type = state->md_info.cur_md_state.motion_detect_event_type;
  if(enable)
  {
    if (state->irq_info.irq_ready || lsm6dsm_dae_if_available(instance)) {
      state->md_info.cur_md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_ENABLED;
      lsm6dsm_update_md_filter(instance);
    }
  }
  else
  {
    state->md_info.cur_md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
    lsm6dsm_set_md_intr(instance, enable);
    lsm6dsm_set_md_filter(instance, SLOPE_FILTER);
  }

  if(enable || md_not_armed_event)
  {
    DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
                   "md_is_armed=%d",
                   state->md_info.cur_md_state.motion_detect_event_type);

    if(state->md_info.cur_md_state.motion_detect_event_type !=  state->md_info.prev_md_state.motion_detect_event_type) {
      DBG_INST_PRINT(instance, LOW, __FILENAME__, __LINE__,
                     "send MD_event =%d",
                     state->md_info.cur_md_state.motion_detect_event_type);
      pb_send_event(instance,
          sns_motion_detect_event_fields,
          &state->md_info.cur_md_state,
          sns_get_system_time(),
          SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
          &state->md_info.suid);
    }
  }
}
/**
 * Changes all gated accel requests to non-gated accel requests.
 *
 * @param instance   Reference to the instance
 *
 * @return None
 */
static void lsm6dsm_convert_accel_gated_req_to_non_gated(
   sns_sensor_instance *const instance)
{
  sns_request *request;
  bool req_converted_to_non_gated = false;
  sns_sensor_uid* suid = &((sns_sensor_uid)ACCEL_SUID_0);
#if DUAL_SENSOR_SUPPORT
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)instance->state->state;
  if(state->hw_idx)
    suid = &((sns_sensor_uid)ACCEL_SUID_1);
#endif

  /** Parse through existing requests and change gated accel
   *  requests to non-gated accel requests. */
  for(request = (sns_request *)instance->cb->get_client_request(instance, suid, true);
      NULL != request;
      request = (sns_request *)instance->cb->get_client_request(instance, suid, false))
  {
    if(request->message_id == SNS_STD_EVENT_GATED_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG)
    {
      request->message_id = SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG;
      req_converted_to_non_gated = true;
    }
  }

  /** Send an event to gated stream clients that the request is
   *  now treated as non-gated */
  if(req_converted_to_non_gated)
  {
    sns_service_manager *mgr = instance->cb->get_service_manager(instance);
    sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
    sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);
    lsm6dsm_instance_state *state = (lsm6dsm_instance_state *)instance->state->state;

    if(NULL != event)
    {
      event->message_id = SNS_STD_EVENT_GATED_SENSOR_MSGID_GATED_REQ_CONVERTED_TO_NON_GATED;
      event->event_len = 0;
      event->timestamp = sns_get_system_time();
      e_service->api->publish_event(e_service, instance, event, &state->accel_info.suid);
    }
  }
}

static void lsm6dsm_turn_off_md(sns_sensor_instance *const instance,
                                lsm6dsm_instance_state *state)
{
  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                 "turn_off_md: gated_client=%u",
                 state->accel_info.gated_client_present);

  state->md_info.enable_md_int = false;
  lsm6dsm_update_md_intr(instance, false, false);
  lsm6dsm_set_md_config(instance, false);

  //set reconfiguration only if no non-gated clients
  if((state->accel_info.gated_client_present) &&
    (state->fifo_info.fifo_rate == LSM6DSM_ACCEL_ODR_OFF)) {
    lsm6dsm_set_fifo_config(
      instance,
      state->md_info.desired_wmk,
      state->md_info.desired_odr,
      state->common_info.gyro_curr_odr,
      state->fifo_info.fifo_enabled);
    lsm6dsm_set_fifo_wmk(instance);
    lsm6dsm_start_fifo_streaming(instance);
    lsm6dsm_set_acc_lpmode(instance, false);
    lsm6dsm_enable_fifo_intr(instance, state->fifo_info.fifo_enabled);
    lsm6dsm_send_config_event(instance);
  }
  lsm6dsm_dump_reg(instance, state->fifo_info.fifo_enabled);
}

static bool lsm6dsm_check_md_interrupt(lsm6dsm_instance_state const *state,
                                       uint8_t const *wake_src)
{
  uint8_t rw_buffer = 0;
  if(wake_src == NULL)
  {
    uint32_t xfer_bytes;
    lsm6dsm_com_read_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             STM_LSM6DSM_REG_WAKE_SRC,
                             &rw_buffer,
                             1,
                             &xfer_bytes);
  }
  else
  {
    rw_buffer = *wake_src;
  }

  return (rw_buffer & 0x08) ? true : false;
}

void lsm6dsm_handle_md_interrupt(sns_sensor_instance *const instance,
                                 sns_time irq_timestamp,
                                 uint8_t const *wake_src)
{
  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;
  sns_diag_service* diag = state->diag_service;

  /**
   * 1. Handle MD interrupt: Send MD fired event to client.
   * 2. Disable MD.
   * 3. Start Motion Accel FIFO stream with desired config.
   */
  if(lsm6dsm_check_md_interrupt(state, wake_src))
  {
    sns_motion_detect_event md_state;
    md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
    pb_send_event(instance,
                  sns_motion_detect_event_fields,
                  &md_state,
                  irq_timestamp,
                  SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &state->md_info.suid);

    lsm6dsm_convert_accel_gated_req_to_non_gated(instance);

    DBG_INST_PRINT_EX(instance, LOW, __FILENAME__, __LINE__,
                 "MD fired");

    // Sensor State HW Interrupt Log
    sns_diag_sensor_state_interrupt *log =
      (sns_diag_sensor_state_interrupt *)diag->api->alloc_log(
        diag,
        instance,
        &state->md_info.suid,
        sizeof(sns_diag_sensor_state_interrupt),
        SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT);

    if(NULL != log)
    {
      log->interrupt = SNS_DIAG_INTERRUPT_MOTION;
      log->timestamp = irq_timestamp;

      diag->api->submit_log(diag,
                            instance,
                            &state->md_info.suid,
                            sizeof(sns_diag_sensor_state_interrupt),
                            log,
                            SNS_DIAG_SENSOR_STATE_LOG_INTERRUPT,
                            state->log_interrupt_encoded_size,
                            lsm6dsm_encode_sensor_state_log_interrupt);
    }
    lsm6dsm_turn_off_md(instance, state);
    state->num_md_ints++;
  }
}


/** See sns_lsm6dsm_hal.h */
void lsm6dsm_send_config_event(sns_sensor_instance *const instance)
{
  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;
  char operating_mode_normal[] = {LSM6DSM_NORMAL};
  char operating_mode_lpm[] = {LSM6DSM_LPM};
  char operating_mode_hpf[] = {LSM6DSM_HIGH_PERF};

  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;

  pb_buffer_arg op_mode_args;

  if(state->accel_info.lp_mode)
  {
    if(state->common_info.accel_curr_odr > LSM6DSM_ACCEL_ODR52)
    {
      //Normal mode
      op_mode_args.buf = &operating_mode_normal[0];
      op_mode_args.buf_len = sizeof(operating_mode_normal);
    }
    else
    {
      //Low power mode
      op_mode_args.buf = &operating_mode_lpm[0];
      op_mode_args.buf_len = sizeof(operating_mode_lpm);
    }
  }
  else
  {
    //High Performance mode
    op_mode_args.buf = &operating_mode_hpf[0];
    op_mode_args.buf_len = sizeof(operating_mode_hpf);
  }

  phy_sensor_config.has_sample_rate = true;
  phy_sensor_config.sample_rate =
     (state->fifo_info.fifo_rate > 0) ?
     (float)(13 << ((state->fifo_info.fifo_rate >> 4) - 1)) : 0.0;

  phy_sensor_config.has_water_mark = true;
  phy_sensor_config.water_mark = state->fifo_info.cur_wmk;
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = 240;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.resolution = (float)(state->accel_info.sstvt)/1000;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.range[0] = lsm6dsm_accel_range_min[state->accel_info.range_idx];
  phy_sensor_config.range[1] = lsm6dsm_accel_range_max[state->accel_info.range_idx];
  phy_sensor_config.has_stream_is_synchronous = true;
  phy_sensor_config.stream_is_synchronous = false;

  DBG_INST_PRINT(instance, HIGH, __FILENAME__, __LINE__,
                  "send_config_event: pub=0x%x SR*1000=%u WM=%u",
                  state->fifo_info.publish_sensors,
                  (uint32_t)(phy_sensor_config.sample_rate*1000),
                  phy_sensor_config.water_mark);

#if LSM6DSM_AUTO_DEBUG
  SNS_INST_PRINTF(HIGH, instance, "send_config_event: pub=0x%x SR*1000=%u WM=%u",state->fifo_info.publish_sensors,
                  (uint32_t)(phy_sensor_config.sample_rate*1000),
                  phy_sensor_config.water_mark);
#endif

  if(state->fifo_info.publish_sensors & LSM6DSM_ACCEL
     ||
     (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
  {
    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->accel_info.suid);

    lsm6dsm_send_cal_event(instance, LSM6DSM_ACCEL);
  }

  if(state->fifo_info.publish_sensors & LSM6DSM_GYRO)
  {
    // Override above values with gyro info
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 1250;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = state->gyro_info.sstvt;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = lsm6dsm_gyro_range_min[state->gyro_info.range_idx];
    phy_sensor_config.range[1] = lsm6dsm_gyro_range_max[state->gyro_info.range_idx];

    pb_send_event(instance,
        sns_std_sensor_physical_config_event_fields,
        &phy_sensor_config,
        sns_get_system_time(),
        SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
        &state->gyro_info.suid);

    lsm6dsm_send_cal_event(instance, LSM6DSM_GYRO);
  }
  if(state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP)
  {
    // Override above values with sensor temperature info
    phy_sensor_config.sample_rate = state->sensor_temp_info.desired_sampling_rate_hz;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 240;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = LSM6DSM_SENSOR_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = LSM6DSM_SENSOR_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = LSM6DSM_SENSOR_TEMPERATURE_RANGE_MAX;

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->sensor_temp_info.suid);
  }

  if(state->md_info.enable_md_int)
  {
    // Override above values with Motion Detect info
    phy_sensor_config.has_sample_rate = false;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = 24;
    phy_sensor_config.has_resolution = false;
    phy_sensor_config.range_count = 0;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = true;
    phy_sensor_config.has_stream_is_synchronous = true;
    phy_sensor_config.stream_is_synchronous = false;

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->md_info.suid);
  }

}

void lsm6dsm_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       temp_data[2])
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)instance->state->state;

  if(state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP)
  {
    sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
    sns_diag_service* diag = state->diag_service;
    int16_t temp_val = (int16_t)(temp_data[1] << 8 | temp_data[0]);
    float float_temp_val = (temp_val / 256.0) + 25.0;

    log_sensor_state_raw_info log_temp_state_raw_info;

    // Allocate Sensor State Raw log packets for accel and gyro
    sns_memzero(&log_temp_state_raw_info, sizeof(log_temp_state_raw_info));
    log_temp_state_raw_info.encoded_sample_size = state->log_temp_raw_encoded_size;
    log_temp_state_raw_info.diag = diag;
    log_temp_state_raw_info.instance = instance;
    log_temp_state_raw_info.sensor_uid = &state->sensor_temp_info.suid;
    lsm6dsm_log_sensor_state_raw_alloc(&log_temp_state_raw_info, 0);

    // factory calibration
    // Sc = C * (Sr - B)
    // Where,
    // *Sc = Calibrated sample
    // *Sr = Raw sample
    // *C = Scale
    // *B = Bias
    float_temp_val = state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 * (float_temp_val -
      state->sensor_temp_registry_cfg.fac_cal_bias[0]);
    if(state->self_test_info.test_alive)  // Skip process ms interrupt even if detected
    {
      DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
                     "test_alive");
    }
    else
    {
      pb_send_sensor_stream_event(instance,
                                  &state->sensor_temp_info.suid,
                                  timestamp,
                                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                  status,
                                  &float_temp_val,
                                  1,
                                  state->encoded_sensor_temp_event_len);
    }
    DBG_INST_PRINT_EX(instance, HIGH, __FILENAME__, __LINE__,
        "Temp data:  = %d timestamp = %u", (int)float_temp_val, (uint32_t)timestamp);
#if LSM6DSM_AUTO_DEBUG
    SNS_INST_PRINTF(HIGH, instance, "Temp data:  = %d timestamp = %u", (int)float_temp_val, (uint32_t)timestamp);
#endif
    // Log raw uncalibrated sensor data
    lsm6dsm_log_sensor_state_raw_add(
        &log_temp_state_raw_info,
        &float_temp_val,
        1,
        timestamp,
        status);
    lsm6dsm_log_sensor_state_raw_submit(&log_temp_state_raw_info, 1, true);
  }
  state->num_temp_samples++;
}

/** See sns_lsm6dsm_hal.h */
void lsm6dsm_handle_sensor_temp_sample(sns_sensor_instance *const instance)
{
  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;
  uint8_t temp_data[2] = {0};
  uint8_t buffer;
  uint32_t xfer_bytes = 0;

  lsm6dsm_com_read_wrapper(state->scp_service,
                   state->com_port_info.port_handle,
                   STM_LSM6DSM_REG_STATUS,
                   &buffer,
                   1,
                   &xfer_bytes);

  lsm6dsm_com_read_wrapper(
                   state->scp_service,
                   state->com_port_info.port_handle,
                   STM_LSM6DSM_REG_OUT_TEMP_L,
                   &temp_data[0],
                   2,
                   &xfer_bytes);

  lsm6dsm_convert_and_send_temp_sample(instance, sns_get_system_time(), temp_data);

}

/** See sns_lsm6dsm_hal.h */
void lsm6dsm_send_cal_event(sns_sensor_instance *const instance, lsm6dsm_sensor_type sensor_type)
{

  lsm6dsm_instance_state *state =
     (lsm6dsm_instance_state*)instance->state->state;

  DBG_INST_PRINT_EX(instance, MED, __FILENAME__, __LINE__,
        "send_cal_event");

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

  if(sensor_type == LSM6DSM_ACCEL)
  {
    bias_data[0] = state->accel_registry_cfg.fac_cal_bias[0];
    bias_data[1] = state->accel_registry_cfg.fac_cal_bias[1];
    bias_data[2] = state->accel_registry_cfg.fac_cal_bias[2];
    comp_matrix_data[0] = state->accel_registry_cfg.fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = state->accel_registry_cfg.fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = state->accel_registry_cfg.fac_cal_corr_mat.data[2];
    comp_matrix_data[3] = state->accel_registry_cfg.fac_cal_corr_mat.data[3];
    comp_matrix_data[4] = state->accel_registry_cfg.fac_cal_corr_mat.data[4];
    comp_matrix_data[5] = state->accel_registry_cfg.fac_cal_corr_mat.data[5];
    comp_matrix_data[6] = state->accel_registry_cfg.fac_cal_corr_mat.data[6];
    comp_matrix_data[7] = state->accel_registry_cfg.fac_cal_corr_mat.data[7];
    comp_matrix_data[8] = state->accel_registry_cfg.fac_cal_corr_mat.data[8];
  }
  else
  {
    bias_data[0] = state->gyro_registry_cfg.fac_cal_bias[0];
    bias_data[1] = state->gyro_registry_cfg.fac_cal_bias[1];
    bias_data[2] = state->gyro_registry_cfg.fac_cal_bias[2];
    comp_matrix_data[0] = state->gyro_registry_cfg.fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = state->gyro_registry_cfg.fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = state->gyro_registry_cfg.fac_cal_corr_mat.data[2];
    comp_matrix_data[4] = state->gyro_registry_cfg.fac_cal_corr_mat.data[4];
    comp_matrix_data[5] = state->gyro_registry_cfg.fac_cal_corr_mat.data[5];
    comp_matrix_data[6] = state->gyro_registry_cfg.fac_cal_corr_mat.data[6];
    comp_matrix_data[7] = state->gyro_registry_cfg.fac_cal_corr_mat.data[7];
    comp_matrix_data[8] = state->gyro_registry_cfg.fac_cal_corr_mat.data[8];
  }

  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
  pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
      { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(sensor_type == LSM6DSM_ACCEL)
  {
    suid_current = &state->accel_info.suid;
  }
  else
  {
    suid_current = &state->gyro_info.suid;
  }

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
                suid_current);
}

void lsm6dsm_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50];
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };

  sns_memset(buffer, 0, sizeof(buffer));
  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->sensor_temp_info.sampling_intvl;

  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    if(state->timer_sensor_temp_data_stream == NULL)
    {
      sns_service_manager *service_mgr = this->cb->get_service_manager(this);
      sns_stream_service *stream_mgr = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
      stream_mgr->api->create_sensor_instance_stream(stream_mgr,
          this,
          state->timer_suid,
          &state->timer_sensor_temp_data_stream);
      if(state->timer_sensor_temp_data_stream == NULL)
      {
        DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
            "timer_sensor_temp NULL");
        return;
      }
    }
    if(state->sensor_temp_info.desired_sampling_rate_hz !=  state->sensor_temp_info.cur_sampling_rate_hz) {
      DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
          "setting timer: rate(*1000)=%d period=%u",
          (int)(state->sensor_temp_info.desired_sampling_rate_hz*1000), (uint32_t)req_payload.timeout_period);
      state->timer_sensor_temp_data_stream->api->send_request(state->timer_sensor_temp_data_stream, &timer_req);
      state->sensor_temp_info.cur_sampling_rate_hz = state->sensor_temp_info.desired_sampling_rate_hz;
    }
    state->sensor_temp_info.timer_is_active = true;
  }
  else
  {
    //DBG_INST_PRINT(this, LOW, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}

void lsm6dsm_turn_off_fifo(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
      "turn_off_fifo: rate=0x%x", state->fifo_info.fifo_rate);
  if(state->fifo_info.fifo_rate > LSM6DSM_ACCEL_ODR_OFF) {
    lsm6dsm_set_fifo_config(this,
        0,
        LSM6DSM_ACCEL_ODR_OFF,
        LSM6DSM_GYRO_ODR_OFF,
        state->fifo_info.fifo_enabled);
    lsm6dsm_stop_fifo_streaming(this);
    lsm6dsm_powerdown_gyro(this);
    lsm6dsm_set_fifo_wmk(this);
    lsm6dsm_disable_fifo_intr(this);
    // Disable timer
    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_heart_beat_data_stream);
    state->health.heart_attack = false;
   }
}
void lsm6dsm_powerdown_hw(sns_sensor_instance *this)
{
  DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
      "power down sensor");
  lsm6dsm_turn_off_fifo(this);
  lsm6dsm_update_md_intr(this, false, false);
  lsm6dsm_set_md_config(this, false);
  lsm6dsm_set_polling_config(this);
}
/**sets fifo odr = 0
 * this retains old data and
 * new data doesn't store */
#if 0
void lsm6dsm_disable_fifo(sns_sensor_instance *this)
{
  uint8_t rw_buffer = 0x06 ;
  uint32_t xfer_bytes = 0;
  lsm6dsm_read_modify_write(this,
      STM_LSM6DSM_REG_FIFO_CTRL5,
      &rw_buffer,
      1,
      &xfer_bytes,
      false,
      0xFF);
}
#endif
// reconfig fifo after flush or without flush
void lsm6dsm_reconfig_fifo(sns_sensor_instance *this, bool flush)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  uint8_t rw_buffer = 0x0;
  uint32_t xfer_bytes;

  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
                 "reconfig_fifo EN:: flush=%u cur=0x%x des=0x%x",
                 flush, state->fifo_info.fifo_rate, state->fifo_info.desired_fifo_rate);


  lsm6dsm_disable_fifo_intr(this);

  if(state->fifo_info.fifo_rate > LSM6DSM_ACCEL_ODR_OFF) {
    if(flush)
      lsm6dsm_flush_fifo(this);
    if((state->ascp_req_count <= 0) &&
      state->fifo_info.reconfig_req &&
      !lsm6dsm_dae_if_available(this)) {
      // set fifo bypass mode
      lsm6dsm_read_modify_write(this,
          STM_LSM6DSM_REG_FIFO_CTRL5,
          &rw_buffer,
          1,
          &xfer_bytes,
          false,
          0x07);
    }
  }

  if((state->ascp_req_count <= 0) && state->fifo_info.reconfig_req) {
    lsm6dsm_set_fifo_wmk(this);
    lsm6dsm_start_fifo_streaming(this);
    lsm6dsm_send_config_event(this);
    //lsm6dsm_enable_fifo_intr(this, state->fifo_info.fifo_enabled);
    state->config_step = LSM6DSM_CONFIG_IDLE; /* done with reconfig */
    state->fifo_info.inst_publish_sensors = state->fifo_info.publish_sensors;
    state->fifo_info.reconfig_req = false;
    if(state->irq_info.irq_ready && !lsm6dsm_dae_if_available(this))
      lsm6dsm_enable_fifo_intr(this, state->fifo_info.fifo_enabled);
  }
  DBG_INST_PRINT(this, HIGH, __FILENAME__, __LINE__,
      "reconf_fifo EX:: flush=%u cur=0x%x",
      flush, state->fifo_info.fifo_rate);
}

void lsm6dsm_reconfig_hw(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  //tells whether fifo int is req or not
  bool is_fifo_int_req = ((state->fifo_info.publish_sensors & (LSM6DSM_ACCEL | LSM6DSM_GYRO)) ||
      (state->accel_info.gated_client_present && !state->md_info.enable_md_int));
  //tells whether md int is req or not
  bool is_md_int_req = (state->md_info.enable_md_int && !(state->fifo_info.publish_sensors & LSM6DSM_ACCEL));
  //tells whether sensor is req or not
  bool is_sensor_active = (is_md_int_req || state->fifo_info.publish_sensors || state->accel_info.gated_client_present);
  DBG_INST_PRINT_EX(this, HIGH, __FILENAME__, __LINE__,
                 "reconfig_hw: %d %d %d %d",
                 is_fifo_int_req, is_md_int_req, state->fifo_info.reconfig_req, is_sensor_active);
  if(!is_sensor_active) {
    lsm6dsm_powerdown_hw(this);
  } else {
    if(state->fifo_info.reconfig_req && is_fifo_int_req) {
      //need to flush fifo
      //set fifo rate to zero
      //retains old data
      //lsm6dsm_disable_fifo(this);

      if(lsm6dsm_dae_if_available(this))
      {
        lsm6dsm_reconfig_fifo(this, false);
      }
      else
      {
        lsm6dsm_reconfig_fifo(this, true);
      }
    } else if(!is_fifo_int_req && (state->fifo_info.fifo_rate > LSM6DSM_ACCEL_ODR_OFF)) {
      lsm6dsm_turn_off_fifo(this);
    } else if (is_fifo_int_req) {
      // Case when reconfig is not required
      lsm6dsm_send_config_event(this);
    }

    if(is_md_int_req && !state->current_conf.md_enabled) {
      lsm6dsm_set_md_config(this, true);
      lsm6dsm_update_md_intr(this, true, false);
    } else if(!is_md_int_req && state->current_conf.md_enabled) {
      lsm6dsm_update_md_intr(this, false, true);
      lsm6dsm_set_md_config(this, false);
    }
    //if accel is still off, turn on some one need this
    if(state->common_info.accel_curr_odr == LSM6DSM_ACCEL_ODR_OFF) {
      lsm6dsm_accel_odr accel_odr = state->accel_info.desired_odr;
      //Set minimum ODR
      if(state->accel_info.desired_odr == LSM6DSM_ACCEL_ODR_OFF)
      {
        accel_odr = LSM6DSM_ACCEL_ODR13;
      }
      lsm6dsm_set_accel_config(this,
          accel_odr,
          state->accel_info.sstvt,
          state->accel_info.range,
          state->accel_info.bw);
    }
    if(state->fifo_info.reconfig_req && !is_fifo_int_req) {
      state->fifo_info.reconfig_req = false;
    }

    DBG_INST_PRINT(this, MED, __FILENAME__, __LINE__, "publish=0x%x md=%u",
        state->fifo_info.publish_sensors, state->md_info.md_client_present);

    if((state->fifo_info.publish_sensors & LSM6DSM_SENSOR_TEMP) ||
        (state->sensor_temp_info.timer_is_active))
    {
      lsm6dsm_set_polling_config(this);
    }
  }

  DBG_INST_PRINT(this, LOW, __FILENAME__, __LINE__,
                 "step=%u", state->config_step);
}

void lsm6dsm_register_interrupt(sns_sensor_instance *this)
{
  lsm6dsm_instance_state *state = (lsm6dsm_instance_state*)this->state->state;
  if(!state->irq_info.irq_registered)
  {
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
      state->irq_info.irq_registered = true;
    }
  }
}

