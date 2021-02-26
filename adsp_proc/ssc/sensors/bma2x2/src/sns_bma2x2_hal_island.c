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
#include "sns_com_port_types.h"
#include "sns_event_service.h"
#include "sns_gpio_service.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service_manager.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_types.h"

#include "sns_bma2x2_hal.h"
#include "sns_bma2x2_sensor.h"
#include "sns_bma2x2_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_pb_util.h"

#include "sns_std_sensor.pb.h"

#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_timer.pb.h"

#include "sns_cal_util.h"
#include "sns_printf.h"

#if !BMA2X2_CONFIG_ENABLE_DAE
#include "sns_stream_service.h"
#endif
#include "sns_cal.pb.h"
#include "tracer_event_ids.h"

struct bms_accel_any_motion_setting {
    //power in unit of uA
    uint8_t         pwr;
    //sensitivity to user motion
    uint8_t         sensitivity;

    //regv
    uint8_t         sleep_dur;

    //regv
    uint8_t         dur;
    //regv
    uint8_t         threshold;

    uint16_t        odr;

    //regv
    uint16_t        bw;
};

const struct bms_accel_any_motion_setting BMA2X2_ANY_MOTION_PRESETS_LP1 [] = {
    {
        .pwr            = 5,
        .sensitivity    = 2,

        .odr            = 2000,
        .bw             = 0x0f,
        //50ms
        .sleep_dur      = 0x0c,
        .dur            = 2,
        .threshold      = 16,
    },

    {
        .pwr            = 9,
        .sensitivity    = 3,

        .odr            = 2000,
        .bw             = 0x0f,
        //25ms
        .sleep_dur      = 0x0b,
        .dur            = 2,
        .threshold      = 16,
    },

    {
        // power measured on QC platform was close to 57uA
        .pwr            = 57,
        .sensitivity    = 3,

        .odr            = 32,
        // BW = 16Hz
        .bw             = 0x09,
        // sleep duration in LP1 mode = 0ms
        .sleep_dur      = 0x00,
        // use at least 1 sample to determine MD
        .dur            = 2,
        // threshold for 4G range = (4 >> 1)*7.81mg = 15.62mg
        .threshold      = 2,
    },

};

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
  float sample[BMA2X2_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} bma2x2_batch_sample;


#if BMA2X2_CONFIG_ENABLE_DIAG_LOG
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
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
static sns_rc bma2x2_hal_encode_sensor_state_log_interrupt(
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
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
static sns_rc bma2x2_hal_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[BMA2X2_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = BMA2X2_NUM_AXES,
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
  bma2x2_batch_sample *raw_sample = (bma2x2_batch_sample *)log;

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

    parsed_log_size += sizeof(bma2x2_batch_sample);
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
static void bma2x2_hal_log_sensor_state_raw_alloc(
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
static void bma2x2_hal_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  bma2x2_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (bma2x2_batch_sample *)log_raw_info->log;

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
        bma2x2_hal_encode_log_sensor_state_raw);

  log_raw_info->log = NULL;
}

/**
 *
 * Add raw uncalibrated sensor data to Sensor State Raw log
 * packet
 *
 * @param[i] log_raw_info Pointer to logging information
 *                        pertaining to the sensor
 * @param[i] raw_data     uncalibrated sensor data to be logged
 * @param[i] timestamp    Timestamp of the sensor data
 * @param[i] status       Status of the sensor data
 *
 * * @return sns_rc,
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
static sns_rc bma2x2_hal_log_sensor_state_raw_add(
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

  if( (log_raw_info->bytes_written + sizeof(bma2x2_batch_sample)) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    bma2x2_hal_log_sensor_state_raw_submit(log_raw_info, false);
    bma2x2_hal_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    bma2x2_batch_sample *sample =
        (bma2x2_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(bma2x2_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }

  return rc;
}
#else
static sns_rc bma2x2_hal_encode_sensor_state_log_interrupt(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
    UNUSED_VAR(log);
    UNUSED_VAR(log_size);
    UNUSED_VAR(encoded_log_size);
    UNUSED_VAR(encoded_log);
    UNUSED_VAR(bytes_written);
}

static sns_rc bma2x2_hal_encode_log_sensor_state_raw(
    void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
    size_t *bytes_written)
{
    UNUSED_VAR(log);
    UNUSED_VAR(log_size);
    UNUSED_VAR(encoded_log_size);
    UNUSED_VAR(encoded_log);
    UNUSED_VAR(bytes_written);
}

static void bma2x2_hal_log_sensor_state_raw_alloc(
  log_sensor_state_raw_info *log_raw_info,
  uint32_t log_size)
{
    UNUSED_VAR(log_raw_info);
    UNUSED_VAR(log_size);
}

static sns_rc bma2x2_hal_log_sensor_state_raw_add(
  log_sensor_state_raw_info *log_raw_info,
  float *raw_data,
  sns_time timestamp,
  sns_std_sensor_sample_status status)
{
    UNUSED_VAR(log_raw_info);
    UNUSED_VAR(raw_data);
    UNUSED_VAR(timestamp);
    UNUSED_VAR(status);
}
  
static void bma2x2_hal_log_sensor_state_raw_submit(
    log_sensor_state_raw_info *log_raw_info,
    bool batch_complete)
{
      UNUSED_VAR(log_raw_info);
      UNUSED_VAR(batch_complete);
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
static sns_rc bma2x2_com_read_wrapper(
   sns_sync_com_port_service *scp_service,
   sns_sync_com_port_handle *port_handle,
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
static sns_rc bma2x2_com_write_wrapper(
   sns_sync_com_port_service *scp_service,
   sns_sync_com_port_handle *port_handle,
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

  return scp_service->api->sns_scp_register_rw(port_handle,
                                               &port_vec,
                                               1,
                                               save_write_time,
                                               xfer_bytes);
}

static sns_rc bma2x2_com_rw_wrapper(
   sns_sync_com_port_service *scp_service,
   sns_sync_com_port_handle *port_handle,
   sns_port_vector *vectors,
   int32_t num_vectors,
   uint32_t *xfer_bytes)
{
   return scp_service->api->sns_scp_register_rw(port_handle,
       vectors,
       num_vectors,
       false,
       xfer_bytes);
}

/**
 * soft reset the sensor
 * @param scp_service    synchronized com port service
 * @param port_handle    com port handler
 * @param sensor         sensor id
 * @return
 */
sns_rc bma2x2_device_sw_reset(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle,
                               bma2x2_sensor_type sensor)
{
  UNUSED_VAR(sensor);
  uint8_t buffer[1];
  sns_rc rv = SNS_RC_SUCCESS;

  buffer[0] = BMA2X2_REGV_SOFT_RESET_VALUE;
  rv = bma2x2_hal_write_data(
                             scp_service,
                             port_handle,
                             BMA2X2_REGA_RESET_REG,
                             &buffer[0],
                             1);
  if(rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  return SNS_RC_SUCCESS;
}

/**
 * set the device to the default state
 * @param scp_service
 * @param port_handle
 * @param sensor
 * @return sns_rc
 *  SNS_RC_SUCCESS on success
 *  other value    others cases
 */
static sns_rc bma2x2_device_set_default_state(sns_sync_com_port_service * scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        bma2x2_sensor_type sensor)
{
  UNUSED_VAR(scp_service);
  UNUSED_VAR(port_handle);
  UNUSED_VAR(sensor);
  return SNS_RC_SUCCESS;
}



/** see sns_bma2x2_sensor_instance.h */
sns_rc bma2x2_reset_device(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            bma2x2_sensor_type sensor)
{
  sns_rc rv = SNS_RC_SUCCESS;

  /** HW reset only when both Accel is requested for
   *  reset. */
  if( sensor == (BMA2X2_ACCEL | BMA2X2_MOTION_DETECT | BMA2X2_SENSOR_TEMP))
  {
     rv = bma2x2_device_sw_reset(scp_service, port_handle, sensor);
  }

  if(rv == SNS_RC_SUCCESS)
  {
    rv = bma2x2_device_set_default_state(scp_service, port_handle, sensor);
  }

  return rv;
}

#if BMA2X2_ENABLE_TEMP_PATCH
static void bma2x2_temperature_patch(sns_sensor_instance * const this)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *)this->state->state;
  uint32_t rega;
  uint8_t regv;
  uint32_t xfer_bytes;
  sns_rc  rc = SNS_RC_SUCCESS;

  if (this == NULL)
  {
    return ;
  }

  INST_DEBUG(MED, this, "temp patch start");
  // open
  rega = 0x35;
  regv = 0xCA;
  rc = bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            rega,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  if (rc != SNS_RC_SUCCESS)
  {
    INST_DEBUG(ERROR, this, "com wirte error:0x%x-> 0x%0x", regv, rega);
    return ;
  }
  rc = bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            rega,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  if (rc != SNS_RC_SUCCESS)
  {
    INST_DEBUG(ERROR, this, "com wirte error:0x%x-> 0x%0x", regv, rega);
    return ;
  }

  sns_busy_wait(sns_convert_ns_to_ticks(20));
  // disable temperature
  rega = 0x4F;
  regv = 0x00;
  rc = bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            rega,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  if (rc != SNS_RC_SUCCESS)
  {
    INST_DEBUG(ERROR, this, "com wirte error:0x%x-> 0x%0x", regv, rega);
    return ;
  }

  sns_busy_wait(sns_convert_ns_to_ticks(20));
  // close
  rega = 0x35;
  regv = 0x0A;
  rc = bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            rega,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  if (rc != SNS_RC_SUCCESS)
  {
    INST_DEBUG(ERROR, this, "com wirte error:0x%x-> 0x%0x", regv, rega);
    return ;
  }

  INST_DEBUG(MED, this, "temp patch apply success");
}
#endif


/**
 * Sets Accel ODR, range, BW and sensitivity.
 *
 * @param[i] scp_service     handle to synch COM port service
 * @param[i] port_handle     handle to synch COM port
 * @param[i] curr_odr        Accel ODR
 * @param[i] sstvt           Accel sensitivity
 * @param[i] range           Accel range
 * @param[i] bw              Accel BW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
static sns_rc bma2x2_set_accel_odr_config(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle  *port_handle,
                                bma2x2_accel_odr_regv     curr_odr,
                                bma2x2_accel_sstvt        sstvt,
                                bma2x2_accel_range        range,
                                bma2x2_accel_bw           bw)
{
  UNUSED_VAR(sstvt);
  UNUSED_VAR(range);
  UNUSED_VAR(bw);
  uint8_t buffer = (uint8_t)curr_odr;
  uint32_t xfer_bytes;

  if (curr_odr == BMA2X2_ACCEL_ODR_OFF)
  {
    /* no need reconfigure the sensor */
    return SNS_RC_SUCCESS;
  }
  return bma2x2_com_write_wrapper(scp_service,
                                   port_handle,
                                   BMA2X2_REGA_BW_SEL_REG,
                                   &buffer,
                                   1,
                                   &xfer_bytes,
                                   false);
}

void bma2x2_stop_fifo_with_bypass(bma2x2_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;

  /* fifo stream mode */
  regv = BST_SET_VAL_BITBLOCK(regv, 6, 7, BMA2X2_FIFO_MODE_BYPASS);
  regv = BST_SET_VAL_BITBLOCK(regv, 2, 3, 0x03);

  bma2x2_com_write_wrapper(state->scp_service,
                              state->com_port_info.port_handle,
                              BMA2X2_REGA_FIFO_MODE_REG,
                              &regv,
                              1,
                              &xfer_bytes,
                              false);
}

/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_stop_fifo_streaming(bma2x2_instance_state *state)
{
  if (state->low_latency_req)
  { }
  else
  {
    uint8_t fifo_mode_val = 0;
    uint8_t fifo_wml_trig_val = 0;
    uint32_t xfer_bytes;

    /** FIFO */
    state->fifo_info.sampling_intvl = BMA2X2_U64_MAX;
    /* fifo stream mode */
    fifo_mode_val = BST_SET_VAL_BITBLOCK(fifo_mode_val, 6, 7, BMA2X2_FIFO_MODE_BYPASS);
    fifo_mode_val = BST_SET_VAL_BITBLOCK(fifo_mode_val, 2, 3, 0x03);

    if (state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
    {
      fifo_wml_trig_val = BST_SET_VAL_BITBLOCK(fifo_wml_trig_val, 0, 5, state->fifo_info.cur_wml);
    }

    sns_port_vector fifo_mode_vec;
    fifo_mode_vec.is_write = true;
    fifo_mode_vec.buffer = &fifo_mode_val;
    fifo_mode_vec.reg_addr = BMA2X2_REGA_FIFO_MODE_REG;
    fifo_mode_vec.bytes = 1;
    sns_port_vector fifo_wml_trig_vec;
    fifo_wml_trig_vec.is_write = true;
    fifo_wml_trig_vec.buffer = &fifo_wml_trig_val;
    fifo_wml_trig_vec.reg_addr = BMA2X2_REGA_FIFO_WML_TRIG;
    fifo_wml_trig_vec.bytes = 1;

    if (state->fifo_info.fifo_enabled)
      {
      sns_port_vector vectors[] =
        { fifo_mode_vec, fifo_wml_trig_vec };
      bma2x2_com_rw_wrapper (state->scp_service, state->com_port_info.port_handle, vectors, ARRAY_SIZE(vectors), &xfer_bytes);
      }
    else
      {
      sns_port_vector vectors[] =
        { fifo_wml_trig_vec };
      bma2x2_com_rw_wrapper (state->scp_service, state->com_port_info.port_handle, vectors, ARRAY_SIZE(vectors), &xfer_bytes);
      }
  }
}


void bma2x2_reset_ts_base(bma2x2_instance_state *istate)
{
  istate->fifo_info.ff_ts_avail_last_sample = 0;
  istate->fifo_info.ff_isr_cnt = 0;
}

/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_hal_set_fifo_config(bma2x2_instance_state *istate,
                             uint16_t desired_wml,
                             bma2x2_accel_odr_regv regv_desired_odr,
                             uint8_t sensor_flags)
{
  UNUSED_VAR(sensor_flags);
  bma2x2_accel_sstvt acc_sstvt_map[] = {
     BMA2X2_ACCEL_SSTVT_2G,
     BMA2X2_ACCEL_SSTVT_4G,
     BMA2X2_ACCEL_SSTVT_8G,
     BMA2X2_ACCEL_SSTVT_16G
  };
  bma2x2_accel_range acc_range_map[] = {
     BMA2X2_ACCEL_RANGE_2G,
     BMA2X2_ACCEL_RANGE_4G,
     BMA2X2_ACCEL_RANGE_8G,
     BMA2X2_ACCEL_RANGE_16G
  };

  if (regv_desired_odr != istate->fifo_info.fifo_rate)
  {
    istate->fifo_info.fifo_rate = regv_desired_odr;
    //istate->fifo_info.sample_cnt_to_skip_odrc = BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC + 1;
    bma2x2_reset_ts_base(istate);
  }

  if (desired_wml != istate->fifo_info.cur_wml)
  {
    istate->fifo_info.cur_wml = desired_wml;
  }

  istate->accel_info.curr_odr_regv = istate->fifo_info.fifo_rate;
  istate->accel_info.sstvt = acc_sstvt_map[istate->accel_info.acc_res_idx];
  istate->accel_info.range = acc_range_map[istate->accel_info.acc_res_idx];
  istate->accel_info.bw = BMA2X2_ACCEL_BW50;
}



void bma2x2_set_fifo_wml(bma2x2_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;


  if(state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
  {
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 5, state->fifo_info.cur_wml);
  }

  bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMA2X2_REGA_FIFO_WML_TRIG,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);

}


/**
 * configure acc range
 * @param state   instance handler
 * @return
 */
static sns_rc bma2x2_configure_range(bma2x2_instance_state *state)
{
  uint8_t regv = state->accel_info.range;
  uint32_t xfer_bytes;
  return bma2x2_com_write_wrapper(state->scp_service,
                                   state->com_port_info.port_handle,
                                   BMA2X2_REGA_RANGE_SEL_REG,
                                   &regv,
                                   1,
                                   &xfer_bytes,
                                   false);
}

/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_start_fifo_streaming(bma2x2_instance_state *state)
{
  if (state->low_latency_req)
  {}
  else
  {
    uint8_t regv = 0x00;
    uint32_t xfer_bytes;

    /** ODR*/
    bma2x2_set_accel_odr_config (state->scp_service, state->com_port_info.port_handle, state->accel_info.curr_odr_regv, state->accel_info.sstvt, state->accel_info.range,
                                 state->accel_info.bw);

    /** FIFO */
    state->fifo_info.sampling_intvl = bma2x2_get_sample_interval (state->fifo_info.fifo_rate);
    bma2x2_set_fifo_wml (state);
    //start streaming,stream mode
    regv = BST_SET_VAL_BITBLOCK(regv, 6, 7, BMA2X2_FIFO_MODE_STREAM);
    regv = BST_SET_VAL_BITBLOCK(regv, 2, 3, 0x03);

    bma2x2_com_write_wrapper (state->scp_service, state->com_port_info.port_handle,
    BMA2X2_REGA_FIFO_MODE_REG,
                              &regv, 1, &xfer_bytes, false);
  }
}


static void bma2x2_hal_enable_fifo_int(bma2x2_instance_state *state,  uint8_t sensor_enable_flags)
{
  UNUSED_VAR(sensor_enable_flags);
  uint32_t xfer_bytes;

  if(state->fifo_info.fifo_enabled)
  {
    if(state->fifo_info.cur_wml > 0)
    {
      if(state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
      {
        sns_port_vector enable2_vec;
        uint8_t enalbe2_val = 0;
        enable2_vec.is_write = false;
        enable2_vec.buffer = &enalbe2_val;
        enable2_vec.reg_addr = BMA2X2_REGA_INT_ENABLE2_REG;
        enable2_vec.bytes = 1;
        sns_port_vector data_sel_vec;
        uint8_t data_sel_val = 0;
        data_sel_vec.is_write = false;
        data_sel_vec.buffer = &data_sel_val;
        data_sel_vec.reg_addr = BMA2X2_REGA_INT_DATA_SEL_REG;
        data_sel_vec.bytes = 1;
        sns_port_vector vectors[] =
          { enable2_vec, data_sel_vec };
        bma2x2_com_rw_wrapper (state->scp_service, state->com_port_info.port_handle, vectors, ARRAY_SIZE(vectors), &xfer_bytes);
        enalbe2_val = BST_SET_VAL_BIT(enalbe2_val, 6);
#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
        data_sel_val = BST_SET_VAL_BITBLOCK(data_sel_val, 5, 6, 3);
#else
        data_sel_val = BST_SET_VAL_BITBLOCK(data_sel_val, 1, 2, 3);
#endif
        enable2_vec.is_write = true;
        data_sel_vec.is_write = true;
        vectors[0] = enable2_vec;
        vectors[1] = data_sel_vec;
        bma2x2_com_rw_wrapper (state->scp_service, state->com_port_info.port_handle, vectors, ARRAY_SIZE(vectors), &xfer_bytes);
        }
    }
    else
    {
      // TODO add error code - trying to enable FIFO interrupt when wmk=0
    }

  }
}

static void bma2x2_hal_enable_drdy_int(bma2x2_instance_state *state,  uint8_t sensor_enable_flags)
{
  UNUSED_VAR(sensor_enable_flags);
  uint32_t xfer_bytes;

  if(state->fifo_info.fifo_enabled)
  {
    if(state->fifo_info.cur_wml > 0)
    {
      if(state->fifo_info.fifo_enabled & BMA2X2_ACCEL)
      {
        sns_port_vector enable2_vec;
        uint8_t enalbe2_val = 0;
        enable2_vec.is_write = false;
        enable2_vec.buffer = &enalbe2_val;
        enable2_vec.reg_addr = BMA2X2_REGA_INT_ENABLE2_REG;
        enable2_vec.bytes = 1;
        sns_port_vector data_sel_vec;
        uint8_t data_sel_val = 0;
        data_sel_vec.is_write = false;
        data_sel_vec.buffer = &data_sel_val;
        data_sel_vec.reg_addr = BMA2X2_REGA_INT_DATA_SEL_REG;
        data_sel_vec.bytes = 1;
        sns_port_vector vectors[] =
          {enable2_vec, data_sel_vec};
        bma2x2_com_rw_wrapper(state->scp_service,
            state->com_port_info.port_handle,
            vectors,
            ARRAY_SIZE(vectors),
            &xfer_bytes);

        enalbe2_val = BST_SET_VAL_BIT(enalbe2_val, 4);
#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
        data_sel_val = BST_SET_VAL_BIT(data_sel_val, 7);
#else
        data_sel_val = BST_SET_VAL_BIT(data_sel_val, 0);
#endif
        enable2_vec.is_write = true;
        data_sel_vec.is_write = true;
        vectors[0] = enable2_vec;
        vectors[1] = data_sel_vec;
        bma2x2_com_rw_wrapper(state->scp_service,
            state->com_port_info.port_handle,
            vectors,
            ARRAY_SIZE(vectors),
            &xfer_bytes);
      }

    }
    else
    {
      // TODO add error code - trying to enable FIFO interrupt when wmk=0
    }

  }
}


static void bma2x2_hal_hw_cfg_disable_fifo_int(bma2x2_instance_state *state)
{
  uint32_t xfer_bytes;

  sns_port_vector enable2_vec;
  uint8_t enalbe2_val = 0;
  enable2_vec.is_write = false;
  enable2_vec.buffer = &enalbe2_val;
  enable2_vec.reg_addr = BMA2X2_REGA_INT_ENABLE2_REG;
  enable2_vec.bytes = 1;
  sns_port_vector data_sel_vec;
  uint8_t data_sel_val = 0;
  data_sel_vec.is_write = false;
  data_sel_vec.buffer = &data_sel_val;
  data_sel_vec.reg_addr = BMA2X2_REGA_INT_DATA_SEL_REG;
  data_sel_vec.bytes = 1;
  sns_port_vector vectors[] = {enable2_vec, data_sel_vec};
  bma2x2_com_rw_wrapper(state->scp_service,
      state->com_port_info.port_handle,
      vectors,
      2,//ARRAY_SIZE(vectors),
      &xfer_bytes);

  enalbe2_val = BST_CLR_VAL_BIT(enalbe2_val, 6);
#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    data_sel_val = BST_SET_VAL_BITBLOCK(data_sel_val, 5, 6, 0);
#else
    data_sel_val = BST_SET_VAL_BITBLOCK(data_sel_val, 1, 2, 0);
#endif
  enable2_vec.is_write = true;
  enable2_vec.buffer = &enalbe2_val;
  data_sel_vec.is_write = true;
  data_sel_vec.buffer = &data_sel_val;
  vectors[0] = enable2_vec;
  vectors[1] = data_sel_vec;
  bma2x2_com_rw_wrapper(state->scp_service,
      state->com_port_info.port_handle,
      vectors,
      2,//ARRAY_SIZE(vectors),
      &xfer_bytes);
}



/**
 * Disables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
static void bma2x2_hal_hw_cfg_disable_drdy_int(bma2x2_instance_state *state)
{
  uint32_t xfer_bytes;

    sns_port_vector enable2_vec;
    uint8_t enalbe2_val = 0;
    enable2_vec.is_write = false;
    enable2_vec.buffer = &enalbe2_val;
    enable2_vec.reg_addr = BMA2X2_REGA_INT_ENABLE2_REG;
    enable2_vec.bytes = 1;
    sns_port_vector data_sel_vec;
    uint8_t data_sel_val = 0;
    data_sel_vec.is_write = false;
    data_sel_vec.buffer = &data_sel_val;
    data_sel_vec.reg_addr = BMA2X2_REGA_INT_DATA_SEL_REG;
    data_sel_vec.bytes = 1;
    sns_port_vector vectors[] = {enable2_vec, data_sel_vec};
    bma2x2_com_rw_wrapper(state->scp_service,
        state->com_port_info.port_handle,
        vectors,
        ARRAY_SIZE(vectors),
        &xfer_bytes);

    enalbe2_val = BST_CLR_VAL_BIT(enalbe2_val, 4);
#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
      data_sel_val = BST_CLR_VAL_BIT(data_sel_val, 7);
#else
      data_sel_val = BST_CLR_VAL_BIT(data_sel_val, 0);
#endif
    enable2_vec.is_write = true;
    data_sel_vec.is_write = true;
    vectors[0] = enable2_vec;
    vectors[1] = data_sel_vec;
    bma2x2_com_rw_wrapper(state->scp_service,
        state->com_port_info.port_handle,
        vectors,
        ARRAY_SIZE(vectors),
        &xfer_bytes);
}

/**
 * Disables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
static void bma2x2_disable_fifo_intr(bma2x2_instance_state *istate)
{
#if BMA2X2_CONFIG_ENABLE_FIFO
  if (istate->low_latency_req)
  {
    bma2x2_hal_hw_cfg_disable_drdy_int(istate);
  }
  else
  {
    bma2x2_hal_hw_cfg_disable_fifo_int(istate);
  }
#else
  bma2x2_hal_hw_cfg_disable_drdy_int(istate);
#endif
}

/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_enable_fifo_intr(bma2x2_instance_state *state,  uint8_t sensor_enable_flags)
{
#if BMA2X2_CONFIG_ENABLE_FIFO
  if (state->low_latency_req)
  {
    // stop fifo
    bma2x2_stop_fifo_streaming(state);
    bma2x2_hal_enable_drdy_int(state, sensor_enable_flags);
  }
  else
  {
    bma2x2_hal_enable_fifo_int(state, sensor_enable_flags);
  }
#else
    bma2x2_hal_enable_drdy_int(state, sensor_enable_flags);
#endif
}


/** see sns_bma2x2_sensor_instance.h */
sns_rc bma2x2_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer)
{
  return bma2x2_hal_read_data(scp_service,
                                port_handle,
                                BMA2X2_REGA_CHIP_ID_REG,
                                buffer,
                                1);
}

#if BMA2X2_CONFIG_ENABLE_FIFO
/**
 * Reads four status registers starting from
 * BMA2X2_REGA_STATUS_FIFO_REG.
 *
 * @param[i] state              Instance state
 * @param[o] buffer             status registers
 *
 * @return none
 */
static void bma2x2_read_fifo_status(bma2x2_instance_state *state,
                                     uint8_t *buffer)
{
  uint32_t xfer_bytes;

  bma2x2_com_read_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMA2X2_REGA_STATUS2_REG,
                           &state->accel_info.int_status,
                           1,
                           &xfer_bytes);

  /*   | data ready | fifo wml | fifo full | reserved |
   *   |               reserved                       |
   *
   **/
  if ((state->accel_info.int_status & 0x60) ||
      (state->fifo_info.ff_flush_in_proc))
  {
    bma2x2_com_read_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMA2X2_REGA_STATUS_FIFO_REG,
                           buffer,
                           1,
                           &xfer_bytes);
  }
}
#endif


void bma2x2_set_polling_config(sns_sensor_instance *const this)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;

  if(state->sensor_temp_info.sampling_intvl > 0)
  {
    if(!state->sensor_temp_info.timer_is_active)
    {
      bma2x2_start_sensor_temp_polling_timer(this);
    }
  }
  else if(state->sensor_temp_info.timer_is_active)
  {
    state->sensor_temp_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
  }
}

/**
 * Gets current Accel ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float bma2x2_hal_get_accel_odr(bma2x2_accel_odr_regv curr_odr)
{
  float odr = 0.0;
  int8_t idx;

  for(idx = 0; idx < ARR_SIZE(bma2x2_reg_map); idx++)
  {
    if(curr_odr == bma2x2_reg_map[idx].acc_bw_regv
       &&
       curr_odr != BMA2X2_ACCEL_ODR_OFF)
    {
      odr = bma2x2_reg_map[idx].odr;
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
sns_time bma2x2_get_sample_interval(bma2x2_accel_odr_regv curr_odr)
{
  sns_time  sample_interval = 0;
  float odr = bma2x2_hal_get_accel_odr(curr_odr);

  if(odr > 0.0)
  {
    sample_interval = sns_convert_ns_to_ticks(1000000000 / odr);
  }

  return sample_interval;
}


/**
 * Extract a accel sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_sample        The segment of fifo buffer that has the accel sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current bma2x2 sensor instance
 * @param[i] event_service      Event service for sending out the
 * @param[i] state              The state of the bma2x2 sensor instance
 */
static void bma2x2_handle_accel_sample(const uint8_t fifo_sample[6],
                                sns_time timestamp,
                                sns_sensor_instance *const instance,
                                sns_event_service *event_service,
                                bma2x2_instance_state *state,
                                log_sensor_state_raw_info *log_accel_state_raw_info)
{
  UNUSED_VAR(event_service);
  int16_t compose_data[3];
  float ipdata[TRIAXIS_NUM] = {0}, opdata_raw[TRIAXIS_NUM] = {0};
  uint8_t i = 0;

  compose_data[0] = (fifo_sample[0] | (fifo_sample[1] << 8));
  compose_data[1] = (fifo_sample[2] | (fifo_sample[3] << 8));
  compose_data[2] = (fifo_sample[4] | (fifo_sample[5] << 8));

  if (state->accel_info.hw_ver == BMA2X2_SENSOR_VERSION_280)
  {
    /* 14 bits */
    compose_data[0] = compose_data[0] >> 2;
    compose_data[1] = compose_data[1] >> 2;
    compose_data[2] = compose_data[2] >> 2;
  }
  else if (state->accel_info.hw_ver == BMA2X2_SENSOR_VERSION_255)
  {
    /* 12 bits */
    compose_data[0] = compose_data[0] >> 4;
    compose_data[1] = compose_data[1] >> 4;
    compose_data[2] = compose_data[2] >> 4;
  }
  else if (state->accel_info.hw_ver == BMA2X2_SENSOR_VERSION_250E)
  {
    /* 10 bits */
    compose_data[0] = compose_data[0] >> 6;
    compose_data[1] = compose_data[1] >> 6;
    compose_data[2] = compose_data[2] >> 6;
  }
  else if (state->accel_info.hw_ver == BMA2X2_SENSOR_VERSION_222E)
  {
    /* 8 bits */
    compose_data[0] = compose_data[0] >> 8;
    compose_data[1] = compose_data[1] >> 8;
    compose_data[2] = compose_data[2] >> 8;
  }
  else
  {}


  ipdata[0] = compose_data[0] * state->accel_info.current_range_resolution;
  ipdata[1] = compose_data[1] * state->accel_info.current_range_resolution;
  ipdata[2] = compose_data[2] * state->accel_info.current_range_resolution;


  // axis conversion
  for(i = 0; i < TRIAXIS_NUM; i ++)
  {
    opdata_raw[state->axis_map[i].opaxis] = (state->axis_map[i].invert ? -1.0 : 1.0) *
      ipdata[state->axis_map[i].ipaxis];
  }

#if BMA2X2_CONFIG_ENABLE_SELF_TEST_FAC
  INST_DEBUG(LOW, instance, "x(1000):%d, y(1000):%d, z(1000):%d",
        (int32_t)(opdata_raw[0] * 1000),
        (int32_t)(opdata_raw[1] * 1000),
        (int32_t)(opdata_raw[2] * 1000)
        );

  if(state->fac_test_in_progress && state->fac_test_sensor == BMA2X2_ACCEL)
  {
    state->fac_test_info.num_samples ++;

    if (state->fac_test_info.num_samples >= 5) 
    {
        for(i = 0; i < TRIAXIS_NUM; i ++)
        {
          if(i == (TRIAXIS_NUM - 1))
          {
            opdata_raw[i] -= G;
          }
          state->fac_test_info.sample_sum[i] += (opdata_raw[i]);
          state->fac_test_info.sample_square_sum[i] +=
              ((int64_t)(opdata_raw[i]) * (int64_t)(opdata_raw[i]));
        }
    }
  }
#endif

  // factory calibration
  vector3 opdata_cal = sns_apply_calibration_correction_3(
      make_vector3_from_array(opdata_raw),
      make_vector3_from_array(state->accel_registry_cfg.fac_cal_bias),
      state->accel_registry_cfg.fac_cal_corr_mat);

  if((state->fifo_info.publish_sensors & BMA2X2_ACCEL)
     || (state->accel_info.gated_client_present && !state->md_info.enable_md_int))
  {
    sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

    if (state->fifo_info.sample_cnt_to_skip_odrc)
    {
      state->fifo_info.sample_cnt_to_skip_odrc --;
      status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
    }
    pb_send_sensor_stream_event(instance,
                                &state->accel_info.suid,
                                timestamp,
                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                status,
                                opdata_cal.data,
                                ARR_SIZE(opdata_cal.data),
                                state->encoded_imu_event_len);
    // Log raw uncalibrated sensor data
    bma2x2_hal_log_sensor_state_raw_add(
      log_accel_state_raw_info,
      opdata_raw,
      timestamp,
      status);
  }
}

// TODO remove to right position


#if BMA2X2_CONFIG_ENABLE_FIFO

static void bma2x2_hal_ts_drift_verification(sns_sensor_instance * const this,
    sns_time ts_curr, sns_time est_end, uint16_t ff_count, bool in_irq)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *) this->state->state;
  uint32_t devi;
  devi = (ts_curr > est_end) ? (ts_curr - est_end) : (est_end - ts_curr);

  INST_DEBUG(LOW, this,
      "end:%u ts_curr: %u diff: %d ff_count: %d" " too.high.cnt:%d, too.low.cnt:%d",
      (uint32_t ) est_end, (uint32_t ) ts_curr, (int32_t ) (ts_curr - est_end),
      ff_count, state->fifo_info.ff_itvl_gap_need_fix_too_high,
      state->fifo_info.ff_itvl_gap_need_fix_too_low);

  if (in_irq)
  {
    // drift and ts gap variance checking
    if (devi >= state->fifo_info.ts_itvl_ideal * (1))
    {
      if (ts_curr > est_end)
      {
        if (devi >= (BMA2X2_FIFO_TS_NEED_FIXED_FF_NUM_TOO_LOW * state->fifo_info.ts_itvl_ideal))
        {
          state->fifo_info.ff_itvl_gap_need_fix_too_low = BMA2X2_FIFO_TS_DEVIATE_THRESHOLD;
        }
        else
        {
          if (state->fifo_info.ff_itvl_gap_need_fix_too_low < BMA2X2_FIFO_TS_DEVIATE_THRESHOLD)
          {
            // continuous drift on lower side
            state->fifo_info.ff_itvl_gap_need_fix_too_low++;
          }
        }

        //check the higher fix counter, this is not the higher rounds
        if (state->fifo_info.ff_itvl_gap_need_fix_too_high)
        {
          state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;
        }
      }
      else
      {
        // dangerous, current ts before than ts.end 1 frame ts
        state->fifo_info.ff_itvl_gap_need_fix_too_high = BMA2X2_FIFO_TS_DEVIATE_THRESHOLD;
        //check the higher fix counter, this is not the higher rounds
        if (state->fifo_info.ff_itvl_gap_need_fix_too_low)
        {
          state->fifo_info.ff_itvl_gap_need_fix_too_low = 0;
        }
      }
    }
    else
    {
      // allowance
      // within 1 frame interval difference
      if (state->fifo_info.ff_itvl_gap_need_fix_too_low)
      {
        state->fifo_info.ff_itvl_gap_need_fix_too_low--;
      }
      if (state->fifo_info.ff_itvl_gap_need_fix_too_high)
      {
        state->fifo_info.ff_itvl_gap_need_fix_too_high--;
      }
    }
  }
  else
  {
    // non-irq mode
    if (ts_curr < est_end)
    {
      // NO-IRQ mode, the ts current is the bigger than the frame time
      // if the est end is bigger than this time, means the estimation error
      // need to fix
      // dangerous
      state->fifo_info.ff_itvl_gap_need_fix_too_high =
          BMA2X2_FIFO_TS_DEVIATE_THRESHOLD;
    }
    else if (ts_curr > est_end)
    {
      uint16_t ff_count_max = 2;
      ff_count_max = SNS_MAX(ff_count_max, ff_count);
      ff_count_max = SNS_MAX(ff_count_max, state->fifo_info.cur_wml);
      // happy case but...
      if (devi > (ff_count_max * state->fifo_info.ts_itvl_ideal))
      {
        // rebase
        state->fifo_info.ff_ts_avail_last_sample = false;
        state->fifo_info.ff_itvl_gap_need_fix_too_low = 0;
        state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;

        INST_DEBUG (ERROR, this, "ts drift over 1 fifo batching time,"
            "need to rebase");
      }
      else if (devi >= (2 * state->fifo_info.ts_itvl_ideal))
      {
        state->fifo_info.ff_itvl_gap_need_fix_too_low = BMA2X2_FIFO_TS_DEVIATE_THRESHOLD;
      }
      else if (devi > (1 * state->fifo_info.ts_itvl_ideal))
      {
        state->fifo_info.ff_itvl_gap_need_fix_too_low ++;
      }
      else
      {
        // allowance
        if (state->fifo_info.ff_itvl_gap_need_fix_too_low)
        {
          state->fifo_info.ff_itvl_gap_need_fix_too_low --;
        }
      }

      // fix the opposite drift
      if (state->fifo_info.ff_itvl_gap_need_fix_too_high)
      {
        state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;
      }
    }
  }
}


bool bma2x2_hal_update_sample_ts(
   sns_sensor_instance * const this,
   uint16_t        ff_count,
   bool            in_irq,
   bool            ff_overrun,
   sns_time ts_curr)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *) this->state->state;
  sns_time  est_start;
  sns_time  est_end;

  uint32_t  devi;
  sns_time  itvl_irq, frame_itvl_this_batch = 0;
  uint32_t ts_itvl_fix = state->fifo_info.ts_itvl_est;

  UNUSED_VAR(in_irq);

  if (ff_overrun)
  {
    INST_DEBUG(ERROR, this, "NOTICE!!! OVERFLOW");
  }

  INST_DEBUG(LOW, this, "[ts] flush st:%d, 1st ff available:%d irq: %u, ff count:%d, ff wml:%d",
      state->fifo_info.ff_flush_in_proc,
      state->fifo_info.ff_ts_avail_last_sample,
      (uint32_t) state->fifo_info.ts_irq,
      ff_count,
      state->accel_info.ff_wml);

  if (state->fifo_info.ff_ts_avail_last_sample)
  {
    if (in_irq)
    {
      // ISR mode, do the algo update
      itvl_irq = bma2x2_get_time_elaspse_sys (state->fifo_info.ts_irq_last, state->fifo_info.ts_irq);
      frame_itvl_this_batch = (itvl_irq / ff_count);

      // Deviation
      devi = bma2x2_get_time_deviation (frame_itvl_this_batch, state->fifo_info.ts_itvl_est);

      INST_DEBUG(LOW, this,
        "batch intvl 2 INT:%u, avg itvl:%u, " "ideal_itvl:%d, est intvl:%u, diff:%u",
        (uint32_t ) itvl_irq, (uint32_t ) frame_itvl_this_batch,
        (uint32_t ) state->fifo_info.ts_itvl_ideal,
        (uint32_t ) state->fifo_info.ts_itvl_est, (int32_t ) devi);

      if (state->fifo_info.ff_isr_cnt > BMA2X2_FIFO_TS_ISR_CNT_PREPRECISE)
      {
        ts_itvl_fix = ((ts_itvl_fix * BMA2X2_FIFO_TS_ISR_PRECISE_MOST_WEIGHT) +
                      frame_itvl_this_batch * (100 - BMA2X2_FIFO_TS_ISR_PRECISE_MOST_WEIGHT)) / 100;
      }
      else
      {
        ts_itvl_fix = ((ts_itvl_fix * (100 - BMA2X2_FIFO_TS_ISR_PRECISE_MOST_WEIGHT)) +
                      frame_itvl_this_batch * BMA2X2_FIFO_TS_ISR_PRECISE_MOST_WEIGHT) / 100;
      }
    }

    // assert
    if (is_bma2x2_ts_within_hifi_accuracy (state->fifo_info.ts_itvl_ideal, ts_itvl_fix))
    {
      state->fifo_info.ts_itvl_est = ts_itvl_fix;
    }
    else
    {
      ts_itvl_fix = state->fifo_info.ts_itvl_est;
    }

    INST_DEBUG(
          LOW, this, "isr.cnt:%u, update intvl ts est:%u, use:%u",
          (uint32_t )state->fifo_info.ff_isr_cnt,
          (uint32_t )ts_itvl_fix,
          (uint32_t ) state->fifo_info.ts_itvl_est);

    // ts start from the latest ts
    est_end = state->fifo_info.ts_ff_end + ff_count * ts_itvl_fix;
    est_start = state->fifo_info.ts_ff_end + ts_itvl_fix;
  }
  else
  {
    // the first frame
    // we assume that the ts end is just the INT fired or
    // reseted due to flush
    if (!bma2x2_itvl_within_accuracy (state->fifo_info.ts_itvl_ideal, state->fifo_info.ts_itvl_est))
    {
      state->fifo_info.ts_itvl_est = state->fifo_info.ts_itvl_ideal;
    }
    else
    {
      if (state->fifo_info.ts_itvl_ideal == state->fifo_info.ts_itvl_est)
      {
        state->fifo_info.ts_itvl_est = ((state->fifo_info.ts_itvl_ideal * 98) / 100);

      }
    }

    // always use the interrupt ts as the first frame end ts
    est_end   = state->fifo_info.ts_irq;
    est_start = est_end - (state->fifo_info.ts_itvl_est * (ff_count - 1));
  }

  bma2x2_hal_ts_drift_verification(this, ts_curr, est_end, ff_count, in_irq);

  // check and handle the accumulate drift issue
  if ((state->fifo_info.ff_itvl_gap_need_fix_too_low >= BMA2X2_FIFO_TS_DEVIATE_THRESHOLD)
          || (state->fifo_info.ff_itvl_gap_need_fix_too_high >= BMA2X2_FIFO_TS_DEVIATE_THRESHOLD))
  {
    state->fifo_info.ff_itvl_need_fast_fix = 1;
    if (state->fifo_info.ff_itvl_gap_need_fix_too_low >= BMA2X2_FIFO_TS_DEVIATE_THRESHOLD)
    {
       ts_itvl_fix = state->fifo_info.ts_itvl_fast_fixed = (state->fifo_info.ts_itvl_ideal * 102) / 100;
    }
    else
    {
      ts_itvl_fix = state->fifo_info.ts_itvl_fast_fixed = (state->fifo_info.ts_itvl_ideal * 98) / 100;
    }
    state->fifo_info.ts_itvl_fast_fixed = ts_itvl_fix;
  }
  else
  {
    state->fifo_info.ff_itvl_need_fast_fix = 0;
  }


  if (!state->fifo_info.ff_ts_avail_last_sample)
  {
    state->fifo_info.ff_ts_avail_last_sample = 1;
  }
  else
  {
    // try to fix this loop
    if (state->fifo_info.ts_itvl_fast_fixed)
    {
      // ts start from the latest ts
      est_end = state->fifo_info.ts_ff_end + ff_count * ts_itvl_fix;
      est_start = state->fifo_info.ts_ff_end + ts_itvl_fix;
    }
  }

  state->fifo_info.ts_ff_start = est_start;
  state->fifo_info.ts_ff_end = est_end;
  // update the last 'irq' ts
  state->fifo_info.ts_irq_last = state->fifo_info.ts_irq;

  INST_DEBUG(LOW, this,
        "fifo_meta < %d %u %u %u %u %u>",
        ff_count,
        (uint32_t)est_start,
        (uint32_t)est_end,
        (uint32_t)state->fifo_info.ts_itvl_est,
        (uint32_t)ts_curr,
        (uint32_t) state->fifo_info.ts_irq);

  return true;
}
#endif


/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_hal_process_fifo_data_buffer(
  sns_sensor_instance *instance,
  sns_time            ts_ff_start,
  sns_time            ts_ff_itvl,
  const uint8_t       *fifo_start,
  size_t              num_bytes)
{
  bma2x2_instance_state *state = (bma2x2_instance_state *)instance->state->state;
  sns_service_manager *service_manager = instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
     (sns_event_service*)service_manager->get_service(service_manager, SNS_EVENT_SERVICE);
  log_sensor_state_raw_info log_accel_state_raw_info;

  // FIFO
  uint16_t num_sample_sets = 0;
  uint32_t i;

#if BMA2X2_CONFIG_ENABLE_DIAG_LOG
  sns_diag_service* diag = state->diag_service;
  // Allocate Sensor State Raw log packets for accel
  sns_memzero(&log_accel_state_raw_info, sizeof(log_accel_state_raw_info));
  log_accel_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
  log_accel_state_raw_info.diag = diag;
  log_accel_state_raw_info.instance = instance;
  log_accel_state_raw_info.sensor_uid = &state->accel_info.suid;
  bma2x2_hal_log_sensor_state_raw_alloc(&log_accel_state_raw_info, 0);
#endif

  {
    for(i = 0; i < num_bytes; i += 6)
    {
      sns_time timestamp = ts_ff_start + (num_sample_sets++ * ts_ff_itvl);
#if BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1
      INST_DEBUG(LOW, instance, "ts before fd:%u, isr cnt:%u, frame cnt:%u",
                 (uint32_t) timestamp,
                 (uint32_t) state->fifo_info.ff_isr_cnt,
                 (uint32_t)(state->fifo_info.ff_accum_cnt + 1));
#endif

      timestamp -= state->accel_info.ticks_filter_delay;

      state->fifo_info.ff_accum_cnt ++;
      bma2x2_handle_accel_sample(&fifo_start[i],
                                timestamp,
                                instance,
                                event_service,
                                state,
                                &log_accel_state_raw_info);
#if BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1
      INST_DEBUG(LOW, instance, ":%d dl_meta:<%u %u>, accum ff:<%u %u>",
          num_sample_sets,
          (uint32_t) (timestamp >> 32),
          (uint32_t) timestamp,
          (uint32_t) (state->fifo_info.ff_accum_cnt >> 32),
          (uint32_t)state->fifo_info.ff_accum_cnt);
#endif
    }

    bma2x2_hal_log_sensor_state_raw_submit(&log_accel_state_raw_info, true);
  }
}


// <fifo handler>
void bma2x2_hal_send_fifo_flush_done(sns_sensor_instance *const instance,
                                  sns_sensor_uid *suid)
{
  sns_service_manager *mgr = instance->cb->get_service_manager(instance);
  sns_event_service *e_service = (sns_event_service*)mgr->get_service(mgr, SNS_EVENT_SERVICE);
  sns_sensor_event *event = e_service->api->alloc_event(e_service, instance, 0);

  if(NULL != event)
  {
    event->message_id = SNS_STD_MSGID_SNS_STD_FLUSH_EVENT;
    event->event_len = 0;
    event->timestamp = sns_get_system_time();

    e_service->api->publish_event(e_service, instance, event, suid);
  }
}

#if BMA2X2_CONFIG_ENABLE_FIFO
static void bma2x2_hal_fifo_determine_ff_len(
    sns_sensor_instance *const inst,
    bma2x2_fifo_read_ctx_t *ctx,
    uint16_t *ff_len)
{
  uint8_t fifo_status = 0;
  uint8_t fifo_overrun;

  UNUSED_VAR(ctx);

  bma2x2_instance_state *state = (bma2x2_instance_state*)inst->state->state;

  // Read the FIFO Status register
  bma2x2_read_fifo_status(state, &fifo_status);

  // Calculate the number of bytes to be read
  *ff_len         = (uint16_t)(fifo_status & 0x7F);
  fifo_overrun    = fifo_status & 0x80;
}
#endif



static
void bma2x2_hal_fifo_read_out(
    sns_sensor_instance *inst,
    bma2x2_fifo_read_ctx_t  *ctx,
    uint64_t timestamp
    )
{
  bma2x2_instance_state   *istate = (bma2x2_instance_state *) inst->state->state;
  uint8_t                     buffer[100];
  uint32_t                    enc_len;
  sns_port_vector             async_read_msg;
  uint16_t                    ff_len = 0;
  bool                        irq_trigger = false;

#if BMA2X2_CONFIG_ENABLE_FIFO
  if (istate->low_latency_req)
  {
    // wml = 1 --> change to the data ready mode
    // use the data ready mode
    ff_len = 1;
  }
  else
  {
    bma2x2_hal_fifo_determine_ff_len(inst, ctx, &ff_len);
  }
#else
  ff_len = 1;
#endif

  INST_DEBUG(MED, inst, "get fifo frame len:%d, ctx:%d", ff_len, ctx->ctx_type);
  istate->fifo_info.ts_irq = sns_get_system_time();

  if ((ff_len > 0) && (!istate->fifo_info.ff_ascp_in_progress))
  {
    if (ff_len <= BMA2X2_FIFO_MAX_LEN_READ_SYNC)
    {
      // force to async read if there are lower than a margin frames
      ctx->sync_read = 1;
    }

    if (ctx->ctx_type == BMA2X2_FIFO_READ_CTX_TYPE_WMI)
    {
      istate->fifo_info.ts_irq = timestamp;
      irq_trigger = true;
      istate->fifo_info.ff_isr_cnt ++;
    }
    else
    {
      istate->fifo_info.ts_irq = sns_get_system_time();
    }

    async_read_msg.bytes    = (ff_len * BMA2X2_FIFO_FRAME_SIZE);
    async_read_msg.reg_addr = BMA2X2_REGA_FIFO_DATA_OUTPUT_REG;
    async_read_msg.is_write = false;
    async_read_msg.buffer   = NULL;

#if BMA2X2_CONFIG_ENABLE_FIFO
    if (istate->low_latency_req)
    { /* data ready */ }
    else
    {
      // derive the ts base on the interrupt timestamp
      bma2x2_hal_update_sample_ts(inst, ff_len,
        irq_trigger, false, istate->fifo_info.ts_irq);
    }
#else
    // only rely on the drdy interrupt
#endif

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
    // hear beat
    istate->heart_beat_timestamp = istate->fifo_info.ts_ff_end;
    bma2x2_hal_create_heart_beat_timer(inst, istate->heart_beat_timeout);
#endif

    if (!ctx->sync_read)
    {
      // Compose the async com port message
      sns_ascp_create_encoded_vectors_buffer (
          &async_read_msg, 1, true, buffer, sizeof(buffer), &enc_len);

      // Send message to Async Com Port
      sns_request async_com_port_request = (sns_request)
          {.message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW,
           .request_len = enc_len, .request = buffer};
      istate->async_com_port_data_stream->api->send_request (
          istate->async_com_port_data_stream, &async_com_port_request);

      // fifo async in process
      istate->fifo_info.ff_ascp_in_progress = true;
    }
    else
    {
      // sync read
#if !BMA2X2_CONFIG_ENABLE_FIFO
      // data ready mode
      uint8_t acc_data_buffer[BMA2X2_FIFO_FRAME_SIZE] = {0};
      uint32_t xfer_bytes = 0;
      istate->fifo_info.ts_irq = timestamp;
      bool new_data_flag = false;

      // read acc data
      bma2x2_com_read_wrapper(istate->scp_service,
                              istate->com_port_info.port_handle,
                              BMA2X2_REGA_X_AXIS_LSB_REG,
                              acc_data_buffer,
                              BMA2X2_FIFO_FRAME_SIZE,
                              &xfer_bytes);
      new_data_flag = true;
      // handle data
      istate->fifo_info.ts_ff_start = istate->fifo_info.ts_irq;
      bma2x2_hal_process_fifo_data_buffer(inst,
    		  	  	  	  	  	  	  istate->fifo_info.ts_irq,
                                       istate->fifo_info.sampling_intvl,
                                       acc_data_buffer,
                                       BMA2X2_FIFO_FRAME_SIZE);
#else
      if (istate->low_latency_req)
      {
        // data ready mode
        uint8_t acc_data_buffer[BMA2X2_FIFO_FRAME_SIZE] = {0};
        uint32_t xfer_bytes = 0;
        istate->fifo_info.ts_irq = timestamp;
        bool new_data_flag = false;

        // read acc data
        bma2x2_com_read_wrapper(istate->scp_service,
                                istate->com_port_info.port_handle,
                                BMA2X2_REGA_X_AXIS_LSB_REG,
                                acc_data_buffer,
                                BMA2X2_FIFO_FRAME_SIZE,
                                &xfer_bytes);
        new_data_flag = true;
        // handle data
        istate->fifo_info.ts_ff_start = istate->fifo_info.ts_irq;
        bma2x2_hal_process_fifo_data_buffer(inst,
                                    istate->fifo_info.ts_irq,
                                         istate->fifo_info.sampling_intvl,
                                         acc_data_buffer,
                                         BMA2X2_FIFO_FRAME_SIZE);
      }
      else
      {
        uint32_t xfer_bytes = 0;
        // prepare read buffer
        uint8_t fifo_data_buffer[async_read_msg.bytes];
        sns_time ts_itvl = istate->fifo_info.ts_itvl_est;
        if (istate->fifo_info.ff_itvl_need_fast_fix)
        {
          //fast fix triggered
          ts_itvl = istate->fifo_info.ts_itvl_fast_fixed;
        }

        // read fifo data
        bma2x2_com_read_wrapper(istate->scp_service,
                              istate->com_port_info.port_handle,
                              async_read_msg.reg_addr,
                              fifo_data_buffer,
                              async_read_msg.bytes,
                              &xfer_bytes);
        // handle fifo data
        bma2x2_hal_process_fifo_data_buffer(inst,
                                       istate->fifo_info.ts_ff_start,
                                       ts_itvl,
                                       fifo_data_buffer,
                                       async_read_msg.bytes);
        // reset FIFO istate if there are no samples to flush

        if (istate->fifo_info.ff_flush_in_proc)
        {
          istate->fifo_info.ff_flush_in_proc = false;
          bma2x2_hal_send_fifo_flush_done(inst, &istate->accel_info.suid);
        }
      }
        if(istate->fac_test_in_progress)
        {
#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
          /** All self-tests can be handled in normal mode. */
          istate->island_service->api->sensor_instance_island_exit(istate->island_service, inst);
#endif
          bma2x2_process_fac_test(inst);
        }
#endif
    }
  }
  else
  {
    // reset FIFO istate if there are no samples to flush
    if (istate->fifo_info.ff_flush_in_proc)
    {
      istate->fifo_info.ff_flush_in_proc = false;
      bma2x2_hal_send_fifo_flush_done(inst, &istate->accel_info.suid);
    }
  }
}


/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_hal_handle_fifo_event(sns_sensor_instance *const inst,
    sns_interrupt_event *irq_event,
    bma2x2_fifo_read_ctx_type_t ctx_type
    )
{
  bma2x2_fifo_read_ctx_t ctx;
  uint64_t ts = sns_get_system_time();
  ctx.ctx_type = ctx_type;

#if BMA2X2_CONFIG_ENABLE_FIFO
  ctx.sync_read = false;
#else
  ctx.sync_read = true;
#endif

  if (ctx_type == BMA2X2_FIFO_READ_CTX_TYPE_WMI)
  {
    if (irq_event != NULL)
    {
      ts = irq_event->timestamp;
    }
  }

  // read out fifo data with ctx
  bma2x2_hal_fifo_read_out(inst, &ctx, ts);
}


/** see sns_bma2x2_hal.h */
void bma2x2_hal_dump_reg(sns_sensor_instance * const this)
{
  if (this == NULL)
  {
    return ;
  }
#if BMA2X2_CONFIG_ENABLE_DUMP_CORE_REG
    bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;
  struct group_read {
    uint32_t first_reg;
    uint8_t  num_regs;
  } groups[] = { /* must fit within state->reg_status[] */

    { BMA2X2_REGA_STATUS1_REG, 2 },

    { BMA2X2_REGA_RANGE_SEL_REG, 5 },

    { BMA2X2_REGA_INT_ENABLE1_REG, 6 },
        
    { BMA2X2_REGA_INT_SET_REG, 1 },

    { BMA2X2_REGA_FIFO_WML_TRIG, 1 },

    { BMA2X2_REGA_FIFO_MODE_REG, 1 }
  };

  uint32_t xfer_bytes;
  uint8_t reg_status[32];
  uint8_t *reg_val = reg_status;
 
  for(uint32_t i=0; i<ARR_SIZE(groups); i++)
  {
    bma2x2_com_read_wrapper(state->scp_service,

                             state->com_port_info.port_handle,
                             groups[i].first_reg,
                             reg_val,
                             groups[i].num_regs,
                             &xfer_bytes);
    for(uint32_t j=0; j<groups[i].num_regs; j++)
    {
      INST_DEBUG(LOW, this, "dump:: 0x%02x = 0x%02x, hwfg:0x%02x",
                 groups[i].first_reg+j, reg_val[j], state->accel_info.hw_acc_need_recfg);
    }
    reg_val += groups[i].num_regs;
  }
#else
  UNUSED_VAR(this);
#endif
}


/** see sns_bma2x2_hal.h */
void bma2x2_hal_set_gated_accel_config(bma2x2_instance_state *state,
                      uint16_t desired_wmk,
                      bma2x2_accel_odr_regv a_chosen_sample_rate,
                      uint8_t sensor_flags)
{
  if(!state->accel_info.gated_client_present)
  {
    desired_wmk = 0;
    a_chosen_sample_rate = BMA2X2_ACCEL_ODR_OFF;
  }

  if(sensor_flags & BMA2X2_ACCEL)
  {
    state->md_info.desired_wmk = desired_wmk;
    state->md_info.desired_odr = a_chosen_sample_rate;
    state->md_info.sstvt = BMA2X2_ACCEL_SSTVT_8G;
    state->md_info.range = BMA2X2_ACCEL_RANGE_8G;
    state->md_info.bw = BMA2X2_ACCEL_BW50;
  }
}


#if BMA2X2_CONFIG_ENABLE_MD_LPM

static sns_rc sns_see_acc_set_pmu(sns_sensor_instance * const inst,
                                  bool enable)
{
#define BMA2X2_PMU_WAIT_US     (500 * 1000)
  bma2x2_instance_state *istate = (bma2x2_instance_state *) inst->state->state;
  uint8_t regv;
  uint8_t regv_fifo_cfg_1;
  uint32_t xfer_bytes;
  sns_rc rv;

  rv =  bma2x2_com_read_wrapper(istate->scp_service,
                                          istate->com_port_info.port_handle,
                                          BMA2X2_REGA_MODE_CTRL_REG,
                                          &regv,
                                          1,
                                          &xfer_bytes);
  if (rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  if (enable == false)
  {
    rv = bma2x2_com_read_wrapper(istate->scp_service,
                                      istate->com_port_info.port_handle,
                                      BMA2X2_REGA_FIFO_MODE_REG,
                                      &regv_fifo_cfg_1,
                                      1,
                                      &xfer_bytes);
  }

  //power configure
  if (enable == true)
  {
    regv = BST_CLR_VAL_BIT(regv, 7);
    regv = BST_CLR_VAL_BIT(regv, 6);
    regv = BST_CLR_VAL_BIT(regv, 5);
  }
  else
  {
    regv = BST_SET_VAL_BIT(regv, 7);
    regv = BST_CLR_VAL_BIT(regv, 6);
    regv = BST_CLR_VAL_BIT(regv, 5);

    regv = BST_SET_VAL_BITBLOCK(regv, 1, 4, 0);
  }
  //hw configure
  rv |= bma2x2_com_write_wrapper(istate->scp_service,
                                istate->com_port_info.port_handle,
                                BMA2X2_REGA_MODE_CTRL_REG,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
  if (rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  if (enable == false)
#if BMA2X2_ENABLE_FORCE_WAIT
  {
    sns_busy_wait(sns_convert_ns_to_ticks(BMA2X2_PMU_WAIT_US));
    rv = bma2x2_com_write_wrapper(istate->scp_service,
                              istate->com_port_info.port_handle,
                              BMA2X2_REGA_FIFO_MODE_REG,
                              &regv_fifo_cfg_1,
                              1,
                              &xfer_bytes,
                              false);
  }
#else
  {
    /*prepare the timer*/
    if (BMA2X2_REGV_CONFIG_IDLE == istate->accel_info.regv_cfg_manager.cfg_state)
    {
      istate->accel_info.regv_cfg_manager.saved_regv = regv_fifo_cfg_1;
      istate->accel_info.regv_cfg_manager.cfg_state = BMA2X2_REGV_CONFIG_INPROGRESS;
      bma2x2_hal_create_reg_cfg_timer(inst, sns_convert_ns_to_ticks(BMA2X2_PMU_WAIT_US));
      rv = SNS_RC_SUCCESS;
    }
    else
    {
      // last register configuration still in progress
      INST_DEBUG(ERROR, inst, "last reg cfg still in progress");
      rv = SNS_RC_NOT_AVAILABLE;
    }
  }
#endif
  return rv;
}

static sns_rc sns_see_acc_get_out_of_mdlpm(sns_sensor_instance *const inst)
{
  return sns_see_acc_set_pmu(inst, true);
}

static sns_rc bma2x2_hal_acc_configure_anymotion_lpml(bma2x2_instance_state *state)
{
  uint8_t i;
  const struct bms_accel_any_motion_setting *any_motion_setting;
  uint8_t score_curr;
  uint8_t regv;
  int8_t idx_score_highest = -1;
  uint16_t odr             = 32;
  uint8_t score_highest    = 0;
  uint8_t range_idx;
  uint32_t xfer_bytes;
  sns_rc rc = SNS_RC_SUCCESS;

  for (i = 0; i < BST_ARRARY_SIZE(BMA2X2_ANY_MOTION_PRESETS_LP1); i++)
  {
      any_motion_setting = BMA2X2_ANY_MOTION_PRESETS_LP1 + i;
      if (any_motion_setting->odr == odr)
      {
          score_curr = 1000 * any_motion_setting->sensitivity / any_motion_setting->pwr;

          if (score_curr > score_highest)
          {
              score_highest = score_curr;
              idx_score_highest = i;
          }
      }
  }
  if (idx_score_highest < 0)
  {
      return SNS_RC_FAILED;
  }
  any_motion_setting = BMA2X2_ANY_MOTION_PRESETS_LP1 + idx_score_highest;


  range_idx = state->accel_info.acc_res_idx;
  /*slop*/
  regv = any_motion_setting->threshold >> range_idx;
  if (0 == regv)
  {
      regv = 1;
  }
  rc = bma2x2_com_write_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMA2X2_REGA_SLOPE_THRES_REG,
                               &regv,
                               1,
                               &xfer_bytes,
                               false);
  BST_RV_ASSERT_RT(rc, SNS_RC_SUCCESS);
  /*duration*/
  regv = 0;
  regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, any_motion_setting->dur);
  rc = bma2x2_com_write_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMA2X2_REGA_SLOPE_DURN_REG,
                               &regv,
                               1,
                               &xfer_bytes,
                               false);
  BST_RV_ASSERT_RT(rc, SNS_RC_SUCCESS);
  /*band with*/
  regv = 0;
  regv = BST_SET_VAL_BITBLOCK(regv, 0, 4, any_motion_setting->bw);
  rc = bma2x2_com_write_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMA2X2_REGA_BW_SEL_REG,
                               &regv,
                               1,
                               &xfer_bytes,
                               false);
  BST_RV_ASSERT_RT(rc, SNS_RC_SUCCESS);
  /*power*/
  regv = 0;
  regv = BST_SET_VAL_BIT(regv, 6);
  regv = BST_SET_VAL_BITBLOCK(regv, 1, 4, any_motion_setting->sleep_dur);
  rc = bma2x2_com_write_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMA2X2_REGA_MODE_CTRL_REG,
                               &regv,
                               1,
                               &xfer_bytes,
                               false);
  BST_RV_ASSERT_RT(rc, SNS_RC_SUCCESS);
  /*mark as low power mode*/
  state->accel_info.lp_mode = true;

  return SNS_RC_SUCCESS;
}
#else

static sns_rc sns_see_bma2x2_config_motion_detect_thresh(
    bma2x2_instance_state *state)
{
    uint8_t regv;
    uint8_t range_idx = sns_see_bma2x2_get_range_idx(state);
    uint32_t xfer_bytes;

    regv = BMA2X2_MD_THRESHOLD >> range_idx;
    if (0 == regv) {
        regv = 1;
    }

    return bma2x2_com_write_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             BMA2X2_REGA_SLOPE_THRES_REG,
                             &regv,
                             1,
                             &xfer_bytes,
                             false);
}


static sns_rc sns_see_bma2x2_config_motion_detect_duration(
    bma2x2_instance_state *state)
{
    uint8_t regv;
    uint32_t xfer_bytes;

    regv = BMA2X2_MD_DUR;

    return bma2x2_com_write_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             BMA2X2_REGA_SLOPE_DURN_REG,
                             &regv,
                             1,
                             &xfer_bytes,
                             false);
}

#endif



/**
 * see sns_bma2x2_hal.h
 */
void bma2x2_hal_set_md_config(sns_sensor_instance * const this,  bool enable)
{
  uint8_t rega = 0;
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  bma2x2_instance_state *state = (bma2x2_instance_state *)this->state->state;
  bma2x2_accel_odr_regv accel_odr = enable ? BMA2X2_MD_ODR : BMA2X2_ACCEL_ODR_OFF;

  /* INT XYZ motion */
  bma2x2_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMA2X2_REGA_INT_MOTION_DETECT,
                            &regv,
                            1,
                            &xfer_bytes);

  if (enable)
  {
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 2, BMA2X2_REGV_ACCEL_ENABLE_SLOPE_XYZ_VALUE);
  }
  else
  {
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 2, 0);
  }

  bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMA2X2_REGA_INT_MOTION_DETECT,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);

#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    rega = BMA2X2_REGA_INT2_PAD_SEL_REG;
#else
    rega = BMA2X2_REGA_INT1_PAD_SEL_REG;
#endif

  /* map slop INT to PIN */
  bma2x2_com_read_wrapper(state->scp_service,
                              state->com_port_info.port_handle,
                              rega,
                              &regv,
                              1,
                              &xfer_bytes);
  if (enable)
  {
      regv = BST_SET_VAL_BIT(regv, 2);
  }
  else
  {
      regv = BST_CLR_VAL_BIT(regv, 2);
  }

  bma2x2_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            rega,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);


  /** odr*/
  bma2x2_set_accel_odr_config(
                           state->scp_service,
                           state->com_port_info.port_handle,
                           accel_odr,
                           state->accel_info.sstvt,
                           state->accel_info.range,
                           state->accel_info.bw);
  if (enable)
  {
#if BMA2X2_CONFIG_ENABLE_MD_LPM
    bma2x2_hal_acc_configure_anymotion_lpml(state);
#else
    sns_see_bma2x2_config_motion_detect_thresh(state);
    sns_see_bma2x2_config_motion_detect_duration(state);
#endif
  }
  else
  {
#if BMA2X2_CONFIG_ENABLE_MD_LPM
    sns_see_acc_get_out_of_mdlpm(this);
#else
#endif
  }
}



/** * see sns_bma2x2_hal.h */
void bma2x2_hal_update_md_intr(sns_sensor_instance *const instance,
                            bool enable,
                            bool md_not_armed_event)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)instance->state->state;

  if(enable)
  {
    state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_ENABLED;
  }
  else
  {
    state->md_info.md_state.motion_detect_event_type = SNS_MOTION_DETECT_EVENT_TYPE_DISABLED;
  }

  if(enable || md_not_armed_event)
  {
    INST_DEBUG(LOW, instance, "bma2x2_hal_update_md_intr md_is_armed=%d",
                                  state->md_info.md_state.motion_detect_event_type);
    pb_send_event(instance,
                  sns_motion_detect_event_fields,
                  &state->md_info.md_state,
                  sns_get_system_time(),
                  SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &state->md_info.suid);
  }
}


/**
 * Changes all gated accel requests to non-gated accel requests.
 *
 * @param instance   Reference to the instance
 *
 * @return None
 */
static void bma2x2_convert_accel_gated_req_to_non_gated(
   sns_sensor_instance *const instance)
{
  sns_request *request;
  bool req_converted_to_non_gated = false;

  /** Parse through existing requests and change gated accel
   *  requests to non-gated accel requests. */
  for(request = (sns_request *)instance->cb->get_client_request(instance, &((sns_sensor_uid) BMA2X2_ACCEL_SUID), true);
      NULL != request;
      request = (sns_request *)instance->cb->get_client_request(instance, &((sns_sensor_uid) BMA2X2_ACCEL_SUID), false))
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
    bma2x2_instance_state *state = (bma2x2_instance_state *)instance->state->state;

    if(NULL != event)
    {
      event->message_id = SNS_STD_EVENT_GATED_SENSOR_MSGID_GATED_REQ_CONVERTED_TO_NON_GATED;
      event->event_len = 0;
      event->timestamp = sns_get_system_time();
      e_service->api->publish_event(e_service, instance, event, &state->accel_info.suid);
    }
  }
}


void bma2x2_hal_handle_md_interrupt(sns_sensor_instance *const instance,
                                 sns_time irq_timestamp)
{
  bma2x2_instance_state *state =
     (bma2x2_instance_state*)instance->state->state;
  uint8_t rw_buffer = 0;
  uint32_t xfer_bytes;
#if BMA2X2_CONFIG_ENABLE_DIAG_LOG
  sns_diag_service* diag = state->diag_service;
#endif

  bma2x2_com_read_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMA2X2_REGA_STATUS1_REG,
                           &rw_buffer,
                           1,
                           &xfer_bytes);

  if(rw_buffer & BMA2X2_INT_STATUS_MD_TRIGGERED_MASK)
  {
    state->md_info.md_state.motion_detect_event_type =
       SNS_MOTION_DETECT_EVENT_TYPE_FIRED;
    pb_send_event(instance,
                  sns_motion_detect_event_fields,
                  &state->md_info.md_state,
                  irq_timestamp,
                  SNS_MOTION_DETECT_MSGID_SNS_MOTION_DETECT_EVENT,
                  &state->md_info.suid);

    bma2x2_convert_accel_gated_req_to_non_gated(instance);

    INST_DEBUG(LOW, instance, "MD fired");
#if BMA2X2_CONFIG_ENABLE_DIAG_LOG
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
                            bma2x2_hal_encode_sensor_state_log_interrupt);
    }
#endif
  }
}


/** See sns_bma2x2_hal.h */
void bma2x2_hal_send_config_event(sns_sensor_instance *const instance)
{
  bma2x2_instance_state *state =
     (bma2x2_instance_state*)instance->state->state;

  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;

  // TODO: Use appropriate op_mode selected by driver.
  char operating_mode[] = "NORMAL";

  pb_buffer_arg op_mode_args;

  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  phy_sensor_config.has_sample_rate = true;
  if (state->fifo_info.fifo_rate == BMA2X2_ACCEL_ODR_OFF)
  {
    phy_sensor_config.sample_rate = 0.0;
  }
  else
  {
    phy_sensor_config.sample_rate = BMA2X2_ODR_BASE_FREQUENCY * (1 << (state->fifo_info.fifo_rate - BMA2X2_ODR_BASE_REGV));
  }

#if BMA2X2_CONFIG_ENABLE_FIFO
  if (state->low_latency_req)
  {
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.water_mark = 1;
  }
  else
  {
    phy_sensor_config.has_water_mark = true;
    phy_sensor_config.water_mark = state->fifo_info.cur_wml;
  }
#else
  phy_sensor_config.has_water_mark = false;
  phy_sensor_config.water_mark = 1;
#endif
  phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = BMA2X2_ACTIVE_CURRENT;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.has_stream_is_synchronous = true;
  phy_sensor_config.stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = true;
  phy_sensor_config.dri_enabled = true;
  /* For sensors that route data through the SDC/DAE sensor, the DAE watermark
     should be set to max requested watermark. */
#if BMA2X2_CONFIG_ENABLE_DAE
  phy_sensor_config.has_DAE_watermark = true;
  phy_sensor_config.DAE_watermark =
     (state->fifo_info.max_requested_flush_ticks == 0) ?
       UINT32_MAX :
       state->fifo_info.max_requested_wmk;
#else
  phy_sensor_config.has_DAE_watermark = false;
  phy_sensor_config.DAE_watermark = 0;
#endif

  if((state->fifo_info.publish_sensors & BMA2X2_ACCEL)
     ||
     state->accel_info.gated_client_present)
  {
    float acc_res_map [] = {
       BMA2X2_ACCEL_RESOLUTION_2G,
       BMA2X2_ACCEL_RESOLUTION_4G,
       BMA2X2_ACCEL_RESOLUTION_8G,
       BMA2X2_ACCEL_RESOLUTION_16G
    };
    float acc_res_min_max_map [4][2] = {
      {BMA2X2_ACCEL_RANGE_2G_MIN, BMA2X2_ACCEL_RANGE_2G_MAX},
      {BMA2X2_ACCEL_RANGE_4G_MIN, BMA2X2_ACCEL_RANGE_4G_MAX},
      {BMA2X2_ACCEL_RANGE_8G_MIN, BMA2X2_ACCEL_RANGE_8G_MAX},
      {BMA2X2_ACCEL_RANGE_16G_MIN, BMA2X2_ACCEL_RANGE_16G_MAX},
    };

    phy_sensor_config.resolution = acc_res_map[state->accel_info.acc_res_idx];
    phy_sensor_config.range[0] = acc_res_min_max_map[state->accel_info.acc_res_idx][0];
    phy_sensor_config.range[1] = acc_res_min_max_map[state->accel_info.acc_res_idx][1];

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->accel_info.suid);
    bma2x2_send_cal_event(instance, BMA2X2_ACCEL);
  }

  if(state->fifo_info.publish_sensors & BMA2X2_SENSOR_TEMP)
  {
    // Override above values with sensor temperature info
    phy_sensor_config.sample_rate = 1;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = &pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = BMA2X2_ACTIVE_CURRENT;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = BMA2X2_SENSOR_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMA2X2_SENSOR_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = BMA2X2_SENSOR_TEMPERATURE_RANGE_MAX;
    phy_sensor_config.has_dri_enabled = true;
    phy_sensor_config.dri_enabled = false;
    phy_sensor_config.has_DAE_watermark = false;
    phy_sensor_config.DAE_watermark =
       (state->sensor_temp_info.max_requested_flush_ticks == 0) ?
       UINT32_MAX :
       (state->sensor_temp_info.sampling_rate_hz / state->sensor_temp_info.report_rate_hz);

    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->sensor_temp_info.suid);
  }
}


void bma2x2_hal_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const int8_t       temp_data)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)instance->state->state;
  int16_t temp_val = (int16_t)(temp_data);
  float float_temp_val = (temp_val) / 2.0;
  float_temp_val += BMA2X2_ACCEL_SENSOR_TEMP_OFFSET_DEG;

  float_temp_val = state->sensor_temp_registry_cfg.fac_cal_corr_mat.e00 * (float_temp_val -
    state->sensor_temp_registry_cfg.fac_cal_bias[0]);

  pb_send_sensor_stream_event(instance,
                              &state->sensor_temp_info.suid,
                              timestamp,
                              SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                              SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH,
                              &float_temp_val,
                              1,
                              state->encoded_sensor_temp_event_len);
}


/** See sns_bma2x2_hal.h */
void bma2x2_hal_handle_sensor_temp_sample(sns_sensor_instance *const instance)
{
  bma2x2_instance_state *state =
     (bma2x2_instance_state*)instance->state->state;
  uint8_t regv = 0;
  int8_t temperature_raw_data;
  uint32_t xfer_bytes = 0;

  bma2x2_com_read_wrapper(state->scp_service,
                          state->com_port_info.port_handle,
                          BMA2X2_REGA_TEMP_RD_REG,
                          &regv,
                          1,
                          &xfer_bytes);

  temperature_raw_data = (int8_t)regv;
  bma2x2_hal_convert_and_send_temp_sample(instance,
                   sns_get_system_time(),
                   temperature_raw_data);
}



void bma2x2_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  uint8_t buffer[50];
  sns_request timer_req = {
    .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
    .request    = buffer
  };
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == state->timer_data_stream)
  {
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_stream_service *strm_svc =
       (sns_stream_service*)smgr->get_service(smgr, SNS_STREAM_SERVICE);
    rc = strm_svc->api->create_sensor_instance_stream(strm_svc, this,
        state->timer_suid, &state->timer_data_stream);
  }

  if(rc != SNS_RC_SUCCESS
     || NULL == state->timer_data_stream)
  {
    SNS_INST_PRINTF(ERROR, this, "error creating stream %d", rc);
    return;
  }
  req_payload.is_periodic = true;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = state->sensor_temp_info.sampling_intvl;

  timer_req.request_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
                                            sns_timer_sensor_config_fields, NULL);
  if(timer_req.request_len > 0)
  {
    state->timer_data_stream->api->send_request(state->timer_data_stream, &timer_req);
    state->sensor_temp_info.timer_is_active = true;
  }
}


#if BMA2X2_CONFIG_HANDLE_HW_CONFIG
static void bma2x2_config_acc(sns_sensor_instance * const this)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)this->state->state;

  // range
  bma2x2_configure_range(istate);

#if BMA2X2_CONFIG_ENABLE_FIFO
  if (!istate->low_latency_req)
  {
    bma2x2_set_fifo_wml(istate);
    bma2x2_start_fifo_streaming(istate);
  }
  else
  {
    // data ready
    bma2x2_set_accel_odr_config(istate->scp_service,
                             istate->com_port_info.port_handle,
                             istate->accel_info.curr_odr_regv,
                             istate->accel_info.sstvt,
                             istate->accel_info.range,
                             istate->accel_info.bw);
  }
#else
  bma2x2_set_accel_odr_config(istate->scp_service,
                           istate->com_port_info.port_handle,
                           istate->accel_info.curr_odr_regv,
                           istate->accel_info.sstvt,
                           istate->accel_info.range,
                           istate->accel_info.bw);
#endif
}


static void bma2x2_configure_md(sns_sensor_instance * const this)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)this->state->state;
  if(istate->md_info.enable_md_int)
  {
    // TODO stop current streaming, flush data
    bma2x2_hal_set_md_config(this, true);
    if(istate->irq_info.irq_ready)
    {
      bma2x2_hal_update_md_intr(this, true, false);
    }
  }
  else
  {
    if(istate->md_info.md_client_present
       &&
       !istate->md_info.enable_md_int)
    {
      bma2x2_hal_update_md_intr(this, false, true);
      bma2x2_hal_set_md_config(this, false);
    }
  }
}

static void bma2x2_hal_configure_sensor_temperature(sns_sensor_instance * const this)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)this->state->state;
  /* Enable timer in case of sensor temperature clients*/
  if(istate->fifo_info.publish_sensors & BMA2X2_SENSOR_TEMP
#if BMA2X2_CONFIG_ENABLE_DAE
      &&
     !bma2x2_dae_if_available(this)
#endif
     )
  {
    bma2x2_set_polling_config(this);
  }
  else
  {
    if(NULL != istate->timer_data_stream)
    {
      istate->sensor_temp_info.timer_is_active = false;
      sns_sensor_util_remove_sensor_instance_stream(this, &istate->timer_data_stream);
    }
  }
}

static void bma2x2_hal_configure_sensor_power(sns_sensor_instance * const inst)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state *) inst->state->state;
  bool enable_hw = (istate->fifo_info.fifo_enabled & (BMA2X2_ACCEL | BMA2X2_SENSOR_TEMP | BMA2X2_MOTION_DETECT));
  if (!enable_hw)
  {
    // no HW request, suspend sensor
    sns_see_acc_set_pmu(inst, false);
  }
  else if (istate->fifo_info.publish_sensors & (BMA2X2_ACCEL))
  {
    // ACC data streaming, normal mode
    sns_see_acc_set_pmu(inst, true);
  }
}

#endif


static void bma2x2_do_config_hw(sns_sensor_instance * const this)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)this->state->state;
  bool enable_fifo_stream = (istate->fifo_info.publish_sensors & (BMA2X2_ACCEL | BMA2X2_SENSOR_TEMP | BMA2X2_MOTION_DETECT));

  INST_DEBUG(LOW, this, "istate:%p, e_f_s:0X%x, e_fifo_state=0x%x",
             istate, enable_fifo_stream, enable_fifo_stream);

#if !BMA2X2_ENABLE_FORCE_WAIT
  if (istate->accel_info.regv_cfg_manager.cfg_state ==
                  BMA2X2_REGV_CONFIG_INPROGRESS)
  {
    INST_DEBUG(ERROR, this, "reg cfg still in progress");
    istate->accel_info.acc_cfg_in_process = 1;
    return ;
  }
#endif

  if(istate->md_info.enable_md_int)
  {
    enable_fifo_stream = false;
  }


  // <power>
  bma2x2_hal_configure_sensor_power(this);

  if (istate->accel_info.regv_cfg_manager.cfg_state ==
                    BMA2X2_REGV_CONFIG_IDLE)
  {
    // <interrupt>
    if ((enable_fifo_stream & BMA2X2_ACCEL) ||
    (enable_fifo_stream & BMA2X2_MOTION_DETECT))
    {
      bma2x2_enable_fifo_intr(istate, istate->fifo_info.fifo_enabled);
    }
    else
    {
      bma2x2_disable_fifo_intr(istate);
    }

    // <configure accel>
    bma2x2_config_acc(this);

    // <conofigure MD>
    bma2x2_configure_md(this);

    // <configure Temperature>
    bma2x2_hal_configure_sensor_temperature(this);
  }


#if BMA2X2_ENABLE_DAE
  if(istate->fifo_info.publish_sensors != 0)
  {
    bma2x2_dae_if_start_streaming(this);
  }
#endif

  INST_DEBUG(MED, this, "do cfg hw finish");
  return ;
}


void bma2x2_hal_reconfig_hw(sns_sensor_instance * const this)
{
  bma2x2_instance_state *istate = (bma2x2_instance_state*)this->state->state;
#if BMA2X2_ENABLE_DEBUG
  bool enable_fifo_stream = (istate->fifo_info.fifo_enabled & (BMA2X2_ACCEL | BMA2X2_SENSOR_TEMP | BMA2X2_MOTION_DETECT));

  INST_DEBUG(LOW, this, "@rcfg_hw this:%p, ps:0x%x, efs:%d",
            this, istate->fifo_info.publish_sensors, enable_fifo_stream);
#endif

#if BMA2X2_ENABLE_TEMP_PATCH
  // temperature patch
  if (istate->do_temperature_patch == false)
  {
    bma2x2_temperature_patch(this);
    istate->do_temperature_patch = true;
  }
#endif

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
  sns_sensor_util_remove_sensor_instance_stream(this, &istate->timer_heart_beat_data_stream);
#endif
  // configure hw
  bma2x2_do_config_hw(this);
  istate->config_step = BMA2X2_CONFIG_IDLE; /* done with reconfigure */

  // dump register
  //bma2x2_hal_dump_reg(this);
}


sns_rc bma2x2_hal_read_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes)
{
   uint32_t reg_addr = rega;
   uint32_t xfer_bytes;
   return bma2x2_com_read_wrapper(scp_service,
                                  port_handle,
                                  reg_addr,
                                  regv_buffer,
                                  bytes,
                                  &xfer_bytes);
}


sns_rc bma2x2_hal_write_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes)
{
   uint32_t reg_addr = rega;
   uint32_t xfer_bytes;
   return bma2x2_com_write_wrapper(scp_service,
                                  port_handle,
                                  reg_addr,
                                  regv_buffer,
                                  bytes,
                                  &xfer_bytes,
                                  false);
}





void bma2x2_hal_register_interrupt(sns_sensor_instance *this)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;
  if(!state->irq_info.irq_registered)
  {
    sns_data_stream* data_stream = state->interrupt_data_stream;
    uint8_t buffer[20];
    sns_request irq_req =
    {
      .message_id = SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REQ,
      .request    = buffer
    };

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




void bma2x2_hal_fifo_drain(
    sns_sensor_instance *inst,
    bool                    sync_read,
    bma2x2_fifo_flush_trigger_t trigger)
{
  bma2x2_instance_state   *istate = (bma2x2_instance_state *) inst->state->state;
  bma2x2_fifo_read_ctx_t      ctx;

  ctx.ctx_type = BMA2X2_FIFO_READ_CTX_TYPE_FLUSH;
  ctx.sync_read = sync_read;

#if BMA2X2_CONFIG_ENABLE_FLUSH_SYNC_READ
  if (trigger == BMA2X2_FIFO_FLUSH_TRIGGER_CLIENTS)
  {
    istate->fifo_info.ff_flush_in_proc = true;
  }
#else
  if (!sync_read)
  {
    istate->fifo_info.ff_flush_in_proc = true;
  }
  else
  {
    istate->fifo_info.ff_flush_in_proc = false;
  }
#endif

  istate->fifo_info.ff_flush_trigger = trigger;

  INST_DEBUG(HIGH, inst, "fifo_flush trigger: %d %d", trigger, sync_read);

  bma2x2_hal_fifo_read_out(inst, &ctx, sns_get_system_time());
}


void bma2x2_inst_prepare_odr_changing(
    sns_sensor_instance * const inst,
    bma2x2_accel_odr_regv regv_desired_odr
    )
{
  bma2x2_instance_state *istate = (bma2x2_instance_state *) inst->state->state;
  if ((regv_desired_odr != istate->fifo_info.fifo_rate) &&
      (istate->fifo_info.fifo_enabled != BMA2X2_ACCEL_ODR_OFF) &&
      (regv_desired_odr != BMA2X2_ACCEL_ODR_OFF))
  {
    //should flush by driver self
    //flush
    bma2x2_hal_fifo_drain(inst, true, BMA2X2_FIFO_FLUSH_TRIGGER_HW_CHANGE);
  }
}

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
void bma2x2_hal_create_heart_beat_timer(sns_sensor_instance *this, sns_time timeout_ticks)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)this->state->state;
  //  sns_diag_service* diag = state->diag_service;

  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[50];
  req_payload.is_periodic = true;
  req_payload.start_time = state->heart_beat_timestamp;
  req_payload.timeout_period = timeout_ticks;

  INST_DEBUG(LOW,this,"creat HB timer-EN: cur_time = %u",(uint32_t)req_payload.start_time);
  INST_DEBUG(LOW,this,"heart beat time_out ms = %u",(uint32_t)(timeout_ticks/19200));
  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
      sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    if(state->timer_heart_beat_data_stream == NULL)
    {
      sns_service_manager *service_mgr = this->cb->get_service_manager(this);
      sns_stream_service *stream_mgr = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
      stream_mgr->api->create_sensor_instance_stream(stream_mgr,
          this,
          state->timer_suid,
          &state->timer_heart_beat_data_stream);
      INST_DEBUG(LOW,this,"creating Heart Beat timer stream!");
    }
    if(state->timer_heart_beat_data_stream == NULL)
    {
      SNS_INST_PRINTF(ERROR, this,
          "Error creating Heart Beat timer stream!");
      return;
    } else {
      sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request = buffer, .request_len = req_len  };
      state->timer_heart_beat_data_stream->api->send_request(state->timer_heart_beat_data_stream, &timer_req);
      INST_DEBUG(LOW,this,"set heart_beat timer start_time=%u timeout_period=%u",
      (uint32_t)req_payload.start_time, (uint32_t)req_payload.timeout_period);
    }
  }
  else
  {
    //DBG_INST_PRINT(diag, this, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}
#endif

#if !BMA2X2_ENABLE_FORCE_WAIT
void bma2x2_hal_create_reg_cfg_timer(sns_sensor_instance * const inst, sns_time timeout_ticks)
{
  bma2x2_instance_state *state = (bma2x2_instance_state*)inst->state->state;
  sns_timer_sensor_config req_payload = sns_timer_sensor_config_init_default;
  size_t req_len;
  uint8_t buffer[50];

  req_payload.is_periodic = false;
  req_payload.start_time = sns_get_system_time();
  req_payload.timeout_period = timeout_ticks;

  INST_DEBUG(LOW, inst, "reg cfg timer-EN: cur_time: %u",(uint32_t)req_payload.start_time);
  INST_DEBUG(LOW, inst, "reg cfg time_out us: %u",(uint32_t)(timeout_ticks/19));

  req_len = pb_encode_request(buffer, sizeof(buffer), &req_payload,
      sns_timer_sensor_config_fields, NULL);
  if(req_len > 0)
  {
    if(state->accel_info.regv_cfg_manager.reg_config_timer == NULL)
    {
      sns_service_manager *service_mgr = inst->cb->get_service_manager(inst);
      sns_stream_service *stream_mgr = (sns_stream_service*)
        service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
      stream_mgr->api->create_sensor_instance_stream(stream_mgr,
                                                     inst,
                                                     state->timer_suid,
                                                     &state->accel_info.regv_cfg_manager.reg_config_timer);
      INST_DEBUG(LOW, inst, "creating register configuration stream!");
    }
    if(state->accel_info.regv_cfg_manager.reg_config_timer == NULL)
    {
      SNS_INST_PRINTF(ERROR, inst,
          "Error creating reg cfg timer stream!");
      return;
    }
    else
    {
      sns_request timer_req =
      {  .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
        .request = buffer, .request_len = req_len
      };
      state->accel_info.regv_cfg_manager.reg_config_timer->api->send_request(
          state->accel_info.regv_cfg_manager.reg_config_timer, &timer_req);
      INST_DEBUG(LOW, inst,"set reg cfg timer st:%u tt:%u",
      (uint32_t)req_payload.start_time, (uint32_t)req_payload.timeout_period);
    }
  }
  else
  {
    //DBG_INST_PRINT(diag, inst, SNS_ERROR, __FILENAME__, __LINE__,
    //                         "LSM timer req encode error");
  }
}


void bma2x2_hal_handle_reg_cfg_timer_event(sns_sensor_instance *const inst)
{
  bma2x2_instance_state * istate = (bma2x2_instance_state *) inst->state->state;
  sns_sensor_event *event;
  uint8_t regv;
  sns_rc rv ;

  if(NULL != istate->accel_info.regv_cfg_manager.reg_config_timer)
    {
      event = istate->accel_info.regv_cfg_manager.reg_config_timer->api->peek_input(
          istate->accel_info.regv_cfg_manager.reg_config_timer);
      while(NULL != event)
      {
        pb_istream_t stream = pb_istream_from_buffer((pb_byte_t*)event->event,
                                                     event->event_len);
        sns_timer_sensor_event timer_event;
        if(pb_decode(&stream, sns_timer_sensor_event_fields, &timer_event))
        {
          if(event->message_id == SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT)
          {
            INST_DEBUG(LOW, inst, "reg cfg timer fired");
            if (istate->accel_info.regv_cfg_manager.cfg_state ==
                BMA2X2_REGV_CONFIG_INPROGRESS)
            {
              uint32_t xfer_bytes;
              // restore previous fifo mode configuration
              regv = istate->accel_info.regv_cfg_manager.saved_regv;
              rv = bma2x2_com_write_wrapper(istate->scp_service,
                                            istate->com_port_info.port_handle,
                                            BMA2X2_REGA_FIFO_MODE_REG,
                                            &regv,
                                            1,
                                            &xfer_bytes,
                                            false);
              if (rv == SNS_RC_SUCCESS)
              {
                istate->accel_info.regv_cfg_manager.cfg_state = BMA2X2_REGV_CONFIG_IDLE;
                INST_DEBUG(LOW, inst, "reg cfg timer.fifo cfg success");
              }
              else
              {
                INST_DEBUG(LOW, inst, "reg cfg timer.fifo cfg failure:%d", rv);
              }
              // reconfigure
              if (rv == SNS_RC_SUCCESS)
              {
                bool enable_fifo_stream = (istate->fifo_info.publish_sensors &
                    (BMA2X2_ACCEL | BMA2X2_SENSOR_TEMP | BMA2X2_MOTION_DETECT));
                // <interrupt>
                if ((enable_fifo_stream & BMA2X2_ACCEL) ||
                    (enable_fifo_stream & BMA2X2_MOTION_DETECT))
                {
                  bma2x2_enable_fifo_intr(istate, istate->fifo_info.fifo_enabled);
                }
                else
                {
                  bma2x2_disable_fifo_intr(istate);
                }

                // <configure accel>
                bma2x2_config_acc(inst);

                // <conofigure MD>
                bma2x2_configure_md(inst);

                // <configure Temperature>
                bma2x2_hal_configure_sensor_temperature(inst);

                // dump register
                bma2x2_hal_dump_reg(inst);
              }
            }
          }
          else
          {
            INST_DEBUG(LOW, inst, "reg cfg timer msg :%u", event->message_id);
          }
        }
      event = istate->accel_info.regv_cfg_manager.reg_config_timer->api->get_next_input(
          istate->accel_info.regv_cfg_manager.reg_config_timer);
      }
    }

  if (istate->accel_info.acc_cfg_in_process)
  {
    // resume the h/w configuration
    bma2x2_do_config_hw(inst);
    istate->accel_info.acc_cfg_in_process = 0;
  }

}

#endif


// <calibration/registry>
void bma2x2_send_cal_event(sns_sensor_instance *const inst,
                            bma2x2_sensor_type sensor_type)
{

  bma2x2_instance_state *state =
     (bma2x2_instance_state*)inst->state->state;

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

  if(sensor_type == BMA2X2_ACCEL)
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

  pb_buffer_arg buff_arg_bias = (pb_buffer_arg)
      { .buf = &bias_data, .buf_len = ARR_SIZE(bias_data) };
  pb_buffer_arg buff_arg_comp_matrix = (pb_buffer_arg)
      { .buf = &comp_matrix_data, .buf_len = ARR_SIZE(comp_matrix_data) };
  sns_sensor_uid *suid_current;

  //update suid
  if(sensor_type == BMA2X2_ACCEL)
  {
    suid_current = &state->accel_info.suid;
  }
  else
  {
    suid_current = NULL;
  }

  new_calibration_event.bias.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.comp_matrix.funcs.encode = &pb_encode_float_arr_cb;
  new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  INST_DEBUG (MED, inst, "Calibration event");

  pb_send_event(inst,
                sns_cal_event_fields,
                &new_calibration_event,
                sns_get_system_time(),
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                suid_current);
}

/** See sns_bma2x2_hal.h */
void bma2x2_reset_cal_data(sns_sensor_instance *const inst,
      bma2x2_sensor_type sensor_type)
{
  bma2x2_instance_state *state =
     (bma2x2_instance_state*) inst->state->state;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {1,0,0,0,1,0,0,0,1};

  if(sensor_type == BMA2X2_ACCEL)
  {
    for (int i = 0; i < ARR_SIZE(bias_data); i++)
    {
      state->accel_registry_cfg.fac_cal_bias[i] = bias_data[i];
    }

    for (int i = 0; i < ARR_SIZE(comp_matrix_data); i++)
    {
      state->accel_registry_cfg.fac_cal_corr_mat.data[i] = comp_matrix_data[i];
    }
  state->accel_registry_cfg.version ++;
  }
}

// </calibration/registry>

