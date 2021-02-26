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

#include "sns_bmm150_hal.h"
#include "sns_bmm150_sensor.h"
#include "sns_bmm150_sensor_instance.h"

#include "sns_async_com_port.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "sns_pb_util.h"
#include "sns_async_com_port_pb_utils.h"

#include "sns_std_sensor.pb.h"
#include "sns_diag_service.h"
#include "sns_std.pb.h"
#include "sns_diag.pb.h"
#include "sns_cal_util.h"
#if BMM_CONFIG_ENABLE_DIAG_LOG
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
  float sample[BMM150_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} bmm150_batch_sample;

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
sns_rc bmm150_encode_sensor_state_log_interrupt(
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
sns_rc bmm150_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[BMM150_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = BMM150_NUM_AXES,
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
  bmm150_batch_sample *raw_sample = (bmm150_batch_sample *)log;

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

    parsed_log_size += sizeof(bmm150_batch_sample);
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
void bmm150_log_sensor_state_raw_alloc(
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
void bmm150_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  bmm150_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (bmm150_batch_sample *)log_raw_info->log;

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
        bmm150_encode_log_sensor_state_raw);

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
sns_rc bmm150_log_sensor_state_raw_add(
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

  if( (log_raw_info->bytes_written + sizeof(bmm150_batch_sample)) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    bmm150_log_sensor_state_raw_submit(log_raw_info, false);
    bmm150_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    bmm150_batch_sample *sample =
        (bmm150_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(bmm150_batch_sample);

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
sns_rc bmm150_com_read_wrapper(
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
sns_rc bmm150_com_write_wrapper(
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

 /*! @file bmm150.c
     @brief Sensor driver for BMM150 sensor */

/************************** Internal macros *******************************/
/* Sensor ODR, Repetition and axes enable/disable settings */
#define MODE_SETTING_SEL        UINT16_C(0x000F)
/* Interrupt pin settings like polarity,latch and int_pin enable */
#define INTERRUPT_PIN_SETTING_SEL   UINT16_C(0x01F0)
/* Settings to enable/disable interrupts */
#define INTERRUPT_CONFIG_SEL        UINT16_C(0x1E00)
/* Interrupt settings for configuring threshold values */
#define INTERRUPT_THRESHOLD_CONFIG_SEL  UINT16_C(0x6000)

/********************** Static function declarations ************************/
/*!
 * @brief This internal API is used to validate the device pointer for
 * null conditions.
 *
 * @param[in] dev : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t null_ptr_check(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API sets/resets the power control bit of 0x4B register.
 *
 * @param[in] pwrcntrl_bit : Variable used to select/deselect the suspend mode.
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 * pwrcntrl_bit     |  power mode
 * -----------------|-------------------------
 * 0x00             |    Suspend mode
 * 0x01             |    Sleep/Active modes
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t set_power_control_bit(uint8_t pwrcntrl_bit, struct bmm150_instance_state *dev);

/*!
 * @brief This internal API reads the trim registers of the sensor and stores
 * the trim values in the "trim_data" of device structure.
 *
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t read_trim_registers(struct bmm150_instance_state *dev);

/*!
 * @brief This internal API writes the op_mode value in the Opmode bits
 * (bits 1 and 2) of 0x4C register.
 *
 *  op_mode    |  Power mode
 * ------------|-----------------------
 *   0x00      |   BMM150_NORMAL_MODE
 *   0x01      |   BMM150_FORCED_MODE
 *   0x03      |   BMM150_SLEEP_MODE
 *
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t write_op_mode(uint8_t op_mode, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API sets the device from suspend to sleep mode
 * by setting the power control bit to '1' of 0x4B register
 *
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t suspend_to_sleep_mode(struct bmm150_instance_state *dev);

/*!
 * @brief This internal API sets the xy repetition value in the 0x51 register.
 *
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 *  dev->settings.xy_rep    |  nXY(XY Repetitions)
 * -------------------------|-----------------------
 *   0x00                   |   1
 *   0x01                   |   3
 *   0x02                   |   5
 *    .                     |   .
 *    .                     |   .
 *   0xFF                   |   511
 *
 * @note number of XY Repetitions nXY  = 1+2(dev->settings.xy_rep)
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t set_xy_rep(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API sets the z repetition value in the 0x52 register.
 *
 * @param[in,out] dev      : Structure instance of bmm150_dev
 *
 *  dev->settings.z_rep     |  nZ(Z Repetitions)
 * -------------------------|-----------------------
 *   0x00                   |   1
 *   0x01                   |   2
 *   0x02                   |   3
 *    .                     |   .
 *    .                     |   .
 *   0xFF                   |   256
 *
 * @note number of Z Repetitions nZ  = 1+(dev->settings.z_rep)
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t set_z_rep(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to set the output data rate of the sensor
 *
 * @param[in] dev       : Structure instance of bmm150_dev.
 *
 *  dev->settings.data_rate |   Data rate (ODR)
 * -------------------------|-----------------------
 *   0x00                   |  BMM150_DATA_RATE_10HZ
 *   0x01                   |  BMM150_DATA_RATE_02HZ
 *   0x02                   |  BMM150_DATA_RATE_06HZ
 *   0x03                   |  BMM150_DATA_RATE_08HZ
 *   0x04                   |  BMM150_DATA_RATE_15HZ
 *   0x05                   |  BMM150_DATA_RATE_20HZ
 *   0x06                   |  BMM150_DATA_RATE_25HZ
 *   0x07                   |  BMM150_DATA_RATE_30HZ
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_odr(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API sets the preset mode ODR and repetition settings.
 * @param[in] dev      : Structure instance of bmm150_dev
 *
 *  API settings            |  Representation
 * -------------------------|------------------------------
 * dev->settings.data_rate  | Output Data Rate (ODR)
 * dev->settings.xy_rep     | XY repetition value
 * dev->settings.z_rep      | Z-repetition value
 *
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error.
 */
static int8_t set_odr_xyz_rep(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to enable or disable the magnetic
 * measurement of x,y,z axes based on the value of xyz_axes_control.
 *
 * @param[in] dev        : Structure instance of bmm150_dev.
 *
 *  dev->settings.xyz_axes_control | Measurement axes/channel
 *  -------------------------------|--------------------------
 *  Bit 0                      | X - Channel
 *  Bit 1                      | Y - Channel
 *  Bit 2                      | Z - Channel
 *
 * @note Setting 1 - Disables Channel measurement
 * @note Setting 0 - Enables Channel measurement
 *
 * dev->settings.xyz_axes_control | Measurement axes Enabled/disabled
 * -------------------------------|------------------------------------
 *  0x01                          | Disables X axis (Y,Z axes enabled)
 *  0x02                          | Disables Y axis (X,Z axes enabled)
 *  0x04                          | Disables Z axis (X,Y axes enabled)
 *  0x07                          | Disables all X,Y,Z axes measurement
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_control_measurement_xyz(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to identify the settings which the user
 * wants to modify in the sensor.
 *
 * @param[in] sub_settings : Contains the settings subset to identify particular
 * group of settings which the user is interested to change.
 * @param[in] settings : Contains the user specified settings.
 *
 * @return Indicates whether user is interested to modify the settings which
 * are related to sub_settings.
 * @retval True -> User wants to modify this group of settings
 * @retval False -> User does not want to modify this group of settings
 */
static uint8_t are_settings_changed(uint16_t sub_settings, uint16_t settings);

/*!
 * @brief This API sets the ODR , measurement axes control ,
 * repetition values of xy,z.
 *
 * @param[in] desired_settings : Contains the settings which user wants to
 * change.
 * @param[in] dev : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t mode_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev);
/*!
 * @brief This internal API is used to parse and store the sensor
 * settings in the device structure
 *
 * @param[in] reg_data   : Pointer of an array consisting all sensor
 *                         setting data from 0x4B to 0x52 registers.
 * @param[in] dev        : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static void parse_setting(const uint8_t *reg_data, struct bmm150_instance_state *dev);

/*!
 * @brief This API is used to enable the interrupts and map them to the
 * corresponding interrupt pins and specify the pin characteristics like the
 * polarity , latch settings for the interrupt pins.
 *
 * @note The other interrupts can be latched or non-latched but,
 * Data ready interrupt is always cleared after reading out the data
 *
 * @param[in] desired_settings : Contains the settings which user wants to
 *                               change.
 * @param[in] dev              : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t interrupt_pin_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev);

/*!
 * @brief This API is used to enable data overrun , overflow interrupts and
 * enable/disable high/low threshold interrupts for x,y,z axis based on the
 * threshold values set by the user in the High threshold (0x50) and
 * Low threshold (0x4F) registers.
 *
 * @param[in] desired_settings : Contains the settings which user wants to
 *                               change.
 * @param[in] dev              : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t interrupt_config(uint16_t desired_settings, const struct bmm150_instance_state *dev);

/*!
 * @brief This API is used to write the user specified High/Low threshold value
 * as a reference to generate the high/low threshold interrupt.
 *
 * @param[in] desired_settings : Contains the settings which user wants to
 *                               change.
 * @param[in] dev              : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t interrupt_threshold_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev);

#ifdef BMM150_USE_FLOATING_POINT
/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer X axis data in float.
 *
 * @param[in] mag_data_x     : The value of raw X data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated X data value in float
 */
static float compensate_x(int16_t mag_data_x, uint16_t data_rhall, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Y axis data in float.
 *
 * @param[in] mag_data_y     : The value of raw Y data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated Y data value in float
 */
static float compensate_y(int16_t mag_data_y, uint16_t data_rhall, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Z axis data in float.
 *
 * @param[in] mag_data_z     : The value of raw Z data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated Z data value in float
 */
static float compensate_z(int16_t mag_data_z, uint16_t data_rhall, const struct bmm150_instance_state *dev);

#else

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer X axis data in int16_t.
 *
 * @param[in] mag_data_x     : The value of raw X data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated X data value in int16_t format
 */
static int16_t compensate_x(int16_t mag_data_x, uint16_t data_rhall, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Y axis data in int16_t.
 *
 * @param[in] mag_data_y     : The value of raw Y data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated Y data value in int16_t format
 */
static int16_t compensate_y(int16_t mag_data_y, uint16_t data_rhall, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Z axis data in int16_t.
 *
 * @param[in] mag_data_z     : The value of raw Z data
 * @param[in] data_rhall     : The value of raw RHALL data
 * @param[in] dev            : Structure instance of bmm150_dev.
 *
 * @return Result of compensated Z data value in int16_t format
 */
static int16_t compensate_z(int16_t mag_data_z, uint16_t data_rhall, const struct bmm150_instance_state *dev);

#endif

/*!
 * @brief This internal API is used to perform the normal self test
 * of the sensor and return the self test result as return value
 *
 * @param[in] dev          : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t perform_normal_self_test(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to enable the normal self test by setting
 * the Self Test bit (bit0) of the 0x4C register,
 * which triggers the start of self test
 *
 * @param[out] self_test_enable : The value of self test bit0 in 0x4C register
 * @param[in] dev               : Structure instance of bmm150_dev.
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t enable_normal_self_test(uint8_t *self_test_enable, const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to validate the results of normal self test
 * by using the self test status available in the bit0 of registers 0x42,0x44
 * and 0x46.
 *
 * @param[in] dev       : Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t validate_normal_self_test(const struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to perform advanced self test for Z axis
 *
 * @param[in] dev     : Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 *
 * Return value         | Status of self-test
 *----------------------|---------------------------
 *      0               | BMM150_OK
 *      8               | BMM150_W_ADV_SELF_TEST_FAIL
 */
static int8_t perform_adv_self_test(struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to set the desired power mode ,
 * axes control and repetition settings for advanced self test
 *
 * @param[in] dev   :  Structure instance of bmm150_dev
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t adv_self_test_settings(struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to set the positive or negative value of
 * self-test current and obtain the corresponding magnetometer z axis data
 *
 * @param[in] self_test_current  : Self test current either positive/negative
 * @param[out] data_z            : Z-axis Magnetometer data
 * @param[in] dev                : Structure instance of bmm150_dev
 *
 * self_test_current       | Self-test current Direction
 *-------------------------|------------------------------
 *     0x03                | BMM150_ENABLE_POSITIVE_CURRENT
 *     0x02                | BMM150_ENABLE_NEGATIVE_CURRENT
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t adv_self_test_measurement(uint8_t self_test_current, int16_t *data_z, struct bmm150_instance_state *dev);

/*!
 * @brief This internal API is used to get the difference between the
 * Z axis mag data obtained by positive and negative self-test current
 * and validate whether the advanced self test is done successfully or not.
 *
 * @param[in] positive_data_z   : Z-axis Mag data by positive self-test current
 * @param[in] negative_data_z  : Z-axis Mag data by negative self-test current
 *
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 *
 *  Return value        | Status of self-test
 *----------------------|---------------------------
 *      0               | BMM150_OK
 *      8               | BMM150_W_ADV_SELF_TEST_FAIL
 */
static int8_t validate_adv_self_test(int16_t positive_data_z, int16_t negative_data_z);

/*!
 * @brief This internal API is used to set the self test current value in
 * the Adv. ST bits (bit6 and bit7) of 0x4C register
 *
 * @param[in] self_test_current  : Self test current value (+ve/-ve)
 * @param[in] dev                : Structure instance of bmm150_dev
 *
 * self_test_current       | Self-test current Direction
 *-------------------------|------------------------------
 *     0x00                | BMM150_DISABLE_SELF_TEST_CURRENT
 *     0x02                | BMM150_ENABLE_NEGATIVE_CURRENT
 *     0x03                | BMM150_ENABLE_POSITIVE_CURRENT
 *
 * @return Result of API execution status
 * @retval zero -> Success / +ve value -> Warning / -ve value -> Error
 */
static int8_t set_adv_self_test_current(uint8_t self_test_current, const struct bmm150_instance_state *dev);

/********************** Global function definitions ************************/
/*!
 *  @brief This API is the entry point, Call this API before using other APIs.
 *  This API reads the chip-id of the sensor which is the first step to
 *  verify the sensor and updates the trim parameters of the sensor.
 */
int8_t bmm150_init(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  //uint8_t chip_id = 0;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
    /* Power up the sensor from suspend to sleep mode */
    rslt = set_power_control_bit(BMM150_POWER_CNTRL_ENABLE, dev);
    /* Start-up time delay of 3ms*/
    dev->delay_ms(BMM150_START_UP_TIME);
    if (rslt ==  BMM150_OK) {
      /* Chip ID of the sensor is read */
      //rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_CHIP_ID_ADDR, &chip_id, 1, dev);
      /* Proceed if everything is fine until now */
      //if (rslt ==  BMM150_OK) {
        /* Check for chip id validity */
        //if (chip_id == BMM150_CHIP_ID) {
          /* Function to update trim values */
          rslt = read_trim_registers(dev);
        } else {
          rslt = BMM150_E_DEV_NOT_FOUND;
        }
      }

  return rslt;
}

/*!
 * @brief This API writes the given data to the register address
 * of the sensor.
 */
int8_t bmm150_set_regs(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint8_t reg_addr, uint8_t *reg_data, uint8_t len,
  const struct bmm150_instance_state *dev)
{
  int8_t rslt = BMM150_OK;
  uint32_t xfer_bytes = 0;
  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if ((rslt ==  BMM150_OK) && (reg_data != NULL) && (len != 0)) {
    /* Write the data to the reg_addr */
    /* SPI write requires to set The MSB of reg_addr as 0
  	but in default the MSB is always 0 */
    rslt = bmm150_com_write_wrapper(scp_service, port_handle, reg_addr, reg_data, len, &xfer_bytes, false);
  } else {
    rslt = BMM150_E_NULL_PTR;
  }

  return rslt;
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t bmm150_get_regs(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint8_t reg_addr, uint8_t *reg_data, uint8_t len,
  const struct bmm150_instance_state *dev)
{
  int8_t rslt = BMM150_OK;
  uint32_t xfer_bytes = 0;
  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if ((rslt ==  BMM150_OK) && (reg_data != NULL)) {
      if (dev->interface != BMM150_I2C_INTF) {
          /* If interface selected is SPI */
          reg_addr = reg_addr | 0x80;
      }
      /* Read the data from the reg_addr */
      rslt = bmm150_com_read_wrapper(scp_service, port_handle,reg_addr, reg_data, len,&xfer_bytes);
  } else {
      rslt = BMM150_E_NULL_PTR;
  }

  return rslt;
}

/*!
 * @brief This API is used to perform soft-reset of the sensor
 * where all the registers are reset to their default values except 0x4B.
 */
int8_t bmm150_soft_reset(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_POWER_CONTROL_ADDR, &reg_data, 1, dev);
      if (rslt ==  BMM150_OK) {
          reg_data = reg_data | BMM150_SET_SOFT_RESET;
          rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_POWER_CONTROL_ADDR, &reg_data, 1, dev);
          dev->delay_ms(BMM150_SOFT_RESET_DELAY);
      }
  }

  return rslt;
}

/*!
 * @brief This API is used to set the power mode of the sensor.
 */
int8_t bmm150_set_op_mode(struct bmm150_instance_state *dev)
{
  int8_t rslt;

  uint8_t pwr_mode = dev->settings.pwr_mode;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* Select the power mode to set */
      switch (pwr_mode) {
      case BMM150_NORMAL_MODE:
          /* If the sensor is in suspend mode
          put the device to sleep mode */
          rslt = suspend_to_sleep_mode(dev);
          if (rslt == BMM150_OK) {
              /* write the op mode */
              rslt = write_op_mode(pwr_mode, dev);
          }
          break;
      case BMM150_FORCED_MODE:
          /* If the sensor is in suspend mode
          put the device to sleep mode */
          rslt = suspend_to_sleep_mode(dev);
          if (rslt == BMM150_OK) {
              /* write the op mode */
              rslt = write_op_mode(pwr_mode, dev);
          }
          break;
      case BMM150_SLEEP_MODE:
          /* If the sensor is in suspend mode
          put the device to sleep mode */
          rslt = suspend_to_sleep_mode(dev);
          if (rslt == BMM150_OK) {
              /* write the op mode */
              rslt = write_op_mode(pwr_mode, dev);
          }
          break;
      case BMM150_SUSPEND_MODE:
          /* Set the power control bit to zero */
          rslt = set_power_control_bit(BMM150_POWER_CNTRL_DISABLE, dev);
          break;
      default:
          rslt = BMM150_E_INVALID_CONFIG;
          break;
      }
  }

  return rslt;
}

/*!
 * @brief This API is used to get the power mode of the sensor.
 */
int8_t bmm150_get_op_mode(uint8_t *op_mode, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);

  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      if (dev->settings.pwr_cntrl_bit == BMM150_POWER_CNTRL_DISABLE) {
          /* Power mode set is suspend mode*/
          *op_mode = BMM150_SUSPEND_MODE;
      } else {
          /*Power mode set is stored in the op_mode */
          rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
          *op_mode = BMM150_GET_BITS(reg_data, BMM150_OP_MODE);
      }
  }

  return rslt;
}

/*!
 * @brief This API is used to set the preset mode of the sensor.
 */
int8_t bmm150_set_presetmode(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t preset_mode;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      preset_mode = dev->settings.preset_mode;
      switch (preset_mode) {
      case BMM150_PRESETMODE_LOWPOWER:
          /* Set the data rate x,y,z repetition
          for Low Power mode */
          dev->settings.data_rate = BMM150_DATA_RATE_10HZ;
          dev->settings.xy_rep = BMM150_LOWPOWER_REPXY;
          dev->settings.z_rep = BMM150_LOWPOWER_REPZ;
          rslt = set_odr_xyz_rep(dev);
          break;
      case BMM150_PRESETMODE_REGULAR:
          /* Set the data rate x,y,z repetition
          for Regular mode */
          dev->settings.data_rate = BMM150_DATA_RATE_10HZ;
          //dev->settings.xy_rep = BMM150_REGULAR_REPXY;
          dev->settings.xy_rep = BMM150_ENHANCED_REPXY;
          dev->settings.z_rep = BMM150_REGULAR_REPZ;
          rslt = set_odr_xyz_rep(dev);
          break;
      case BMM150_PRESETMODE_HIGHACCURACY:
          /* Set the data rate x,y,z repetition
          for High Accuracy mode */
          dev->settings.data_rate = BMM150_DATA_RATE_20HZ;
          dev->settings.xy_rep = BMM150_HIGHACCURACY_REPXY;
          dev->settings.z_rep = BMM150_HIGHACCURACY_REPZ;
          rslt = set_odr_xyz_rep(dev);
          break;
      case BMM150_PRESETMODE_ENHANCED:
          /* Set the data rate x,y,z repetition
          for Enhanced Accuracy mode */
          dev->settings.data_rate = BMM150_DATA_RATE_10HZ;
          dev->settings.xy_rep = BMM150_ENHANCED_REPXY;
          dev->settings.z_rep = BMM150_ENHANCED_REPZ;
          rslt = set_odr_xyz_rep(dev);
          break;
      default:
          rslt = BMM150_E_INVALID_CONFIG;
          break;
      }
  }

  return rslt;
}

/*!
 * @brief This API sets the sensor settings based on the desired_settings
 * and the dev structure configuration
 */
int8_t bmm150_set_sensor_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev)
{
  int8_t rslt;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      if (are_settings_changed(MODE_SETTING_SEL, desired_settings)) {
          /* ODR, Control measurement, XY,Z repetition values*/
          rslt = mode_settings(desired_settings, dev);
      }
      if ((!rslt) && are_settings_changed(INTERRUPT_PIN_SETTING_SEL, desired_settings)) {
          /* Interrupt pin settings */
          rslt = interrupt_pin_settings(desired_settings, dev);
      }
      if ((!rslt) && are_settings_changed(INTERRUPT_CONFIG_SEL, desired_settings)) {
          /* Interrupt configuration settings */
          rslt = interrupt_config(desired_settings, dev);
      }
      if ((!rslt) && are_settings_changed(INTERRUPT_THRESHOLD_CONFIG_SEL, desired_settings)) {
          /* Interrupt threshold settings */
          rslt = interrupt_threshold_settings(desired_settings, dev);
      }

  }

  return rslt;
}

/*!
 * @brief This API gets the sensor settings and updates the dev structure
 */
int8_t bmm150_get_sensor_settings(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t setting[BMM150_SETTING_DATA_LEN] = {0};

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /*Read the entire sensor settings */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_POWER_CONTROL_ADDR, setting, BMM150_SETTING_DATA_LEN, dev);
      if (rslt ==  BMM150_OK) {
          /*Parse and store the settings */
          parse_setting(setting, dev);
      }
  }

  return rslt;
}

/*!
 * @brief This API is used to read the magnetometer data from registers
 * 0x42 to 0x49 and update the dev structure with the
 * compensated mag data in micro-tesla.
 */
int8_t bmm150_read_mag_data(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  int16_t msb_data;
  uint8_t reg_data[BMM150_XYZR_DATA_LEN] = {0};
  struct bmm150_raw_mag_data raw_mag_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /*Read the mag data registers */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DATA_X_LSB, reg_data, BMM150_XYZR_DATA_LEN, dev);
      if (rslt ==  BMM150_OK) {
          /* Mag X axis data */
          reg_data[0] = BMM150_GET_BITS(reg_data[0], BMM150_DATA_X);
          /* Shift the MSB data to left by 5 bits */
          /* Multiply by 32 to get the shift left by 5 value */
          msb_data = ((int16_t)((int8_t)reg_data[1])) * 32;
          /* Raw mag X axis data */
          raw_mag_data.raw_datax = (int16_t)(msb_data | reg_data[0]);
          /* Mag Y axis data */
          reg_data[2] = BMM150_GET_BITS(reg_data[2], BMM150_DATA_Y);
          /* Shift the MSB data to left by 5 bits */
          /* Multiply by 32 to get the shift left by 5 value */
          msb_data = ((int16_t)((int8_t)reg_data[3])) * 32;
          /* Raw mag Y axis data */
          raw_mag_data.raw_datay = (int16_t)(msb_data | reg_data[2]);
          /* Mag Z axis data */
          reg_data[4] = BMM150_GET_BITS(reg_data[4], BMM150_DATA_Z);
          /* Shift the MSB data to left by 7 bits */
          /* Multiply by 128 to get the shift left by 7 value */
          msb_data = ((int16_t)((int8_t)reg_data[5])) * 128;
          /* Raw mag Z axis data */
          raw_mag_data.raw_dataz = (int16_t)(msb_data | reg_data[4]);
          /* Mag R-HALL data */
          reg_data[6] = BMM150_GET_BITS(reg_data[6], BMM150_DATA_RHALL);
          raw_mag_data.raw_data_r = (uint16_t)(((uint16_t)reg_data[7] << 6) | reg_data[6]);
          /* Compensated Mag X data in int16_t format */
          dev->data.x = compensate_x(raw_mag_data.raw_datax, raw_mag_data.raw_data_r, dev);
          /* Compensated Mag Y data in int16_t format */
          dev->data.y = compensate_y(raw_mag_data.raw_datay, raw_mag_data.raw_data_r, dev);
          /* Compensated Mag Z data in int16_t format */
          dev->data.z = compensate_z(raw_mag_data.raw_dataz, raw_mag_data.raw_data_r, dev);
      }
  }

  return rslt;
}
/*!
 * @brief This API is used to perform the complete self test
 * (both normal and advanced) for the BMM150 sensor
 */
int8_t bmm150_perform_self_test(uint8_t self_test_mode, struct bmm150_instance_state *dev)
{
  int8_t rslt;
  int8_t self_test_rslt = 0;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      switch (self_test_mode) {
      case BMM150_NORMAL_SELF_TEST:
          /* Set the sensor in sleep mode */
          dev->settings.pwr_mode = BMM150_SLEEP_MODE;
          rslt = bmm150_set_op_mode(dev);
          if (rslt == BMM150_OK) {
              /* Perform the normal self test */
              rslt = perform_normal_self_test(dev);
          }
          break;

      case BMM150_ADVANCED_SELF_TEST:
          /* Perform the advanced self test */
          rslt = perform_adv_self_test(dev);
          /* Check to ensure bus error does not occur */
          if (rslt >=  BMM150_OK) {
              /* Store the status of self test result */
              self_test_rslt = rslt;
              /* Perform soft reset */
              rslt = bmm150_soft_reset(dev);
          }
          /* Check to ensure bus operations are success */
          if (rslt == BMM150_OK) {
              /* Restore self_test_rslt as return value */
              rslt = self_test_rslt;
          }
          break;
      default:
          rslt = BMM150_E_INVALID_CONFIG;
          break;
      }
  }

  return rslt;
}

/*!
 * @brief This API is used to get the status flags of all interrupt
 * which is used to check for the assertion of interrupts
 */
int8_t bmm150_get_interrupt_status(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t interrupt_status;
  uint8_t data_ready_status;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* Read the data ready status from the register 0x48 */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DATA_READY_STATUS, &data_ready_status, 1, dev);
      if (rslt == BMM150_OK) {
          /* Read the interrupt status from the register 0x50 */
          rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_INTERRUPT_STATUS, &interrupt_status, 1, dev);
          if (rslt == BMM150_OK) {
              /* Mask and store the data ready status bit*/
              data_ready_status = BMM150_GET_BITS_POS_0(data_ready_status, BMM150_DRDY_STATUS);
              /* store the entire interrupt status in dev */
              dev->int_status = (data_ready_status << 8) | interrupt_status;
          }
      }
  }

  return rslt;
}
/****************************************************************************/
/**\name    INTERNAL APIs                                               */

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct bmm150_instance_state *dev)
{
  int8_t rslt;

  if ((dev == NULL) || (dev->delay_ms == NULL)) {
      /* Device structure pointer is not valid */
      rslt = BMM150_E_NULL_PTR;
  } else {
      /* Device structure is fine */
      rslt = BMM150_OK;
  }

  return rslt;
}

/*!
 * @brief This internal API sets/resets the power control bit of 0x4B register.
 */
static int8_t set_power_control_bit(uint8_t pwrcntrl_bit, struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data = 0;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* Power control register 0x4B is read */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_POWER_CONTROL_ADDR, &reg_data, 1, dev);
      /* Proceed if everything is fine until now */
      if (rslt ==  BMM150_OK) {
          /* Sets the value of power control bit */
          reg_data = BMM150_SET_BITS_POS_0(reg_data, BMM150_PWR_CNTRL, pwrcntrl_bit);
          rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_POWER_CONTROL_ADDR, &reg_data, 1, dev);
          if (rslt ==  BMM150_OK) {
              /*Store the power control bit
              value in dev structure*/
              dev->settings.pwr_cntrl_bit = pwrcntrl_bit;
          }
      }
  }

  return rslt;
}

/*!
 * @brief This internal API reads the trim registers of the sensor and stores
 * the trim values in the "trim_data" of device structure.
 */
static int8_t read_trim_registers(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t trim_x1y1[2] = {0};
  uint8_t trim_xyz_data[4] = {0};
  uint8_t trim_xy1xy2[10] = {0};
  uint16_t temp_msb = 0;

  /* Trim register value is read */
  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DIG_X1, trim_x1y1, 2, dev);
  if (rslt ==  BMM150_OK) {
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DIG_Z4_LSB, trim_xyz_data, 4, dev);
      if (rslt ==  BMM150_OK) {
          rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DIG_Z2_LSB, trim_xy1xy2, 10, dev);
          if (rslt ==  BMM150_OK) {
              /* Trim data which is read is updated
              in the device structure */
              dev->trim_data.dig_x1 = (int8_t)trim_x1y1[0];
              dev->trim_data.dig_y1 = (int8_t)trim_x1y1[1];
              dev->trim_data.dig_x2 = (int8_t)trim_xyz_data[2];
              dev->trim_data.dig_y2 = (int8_t)trim_xyz_data[3];
              temp_msb = ((uint16_t)trim_xy1xy2[3]) << 8;
              dev->trim_data.dig_z1 = (uint16_t)(temp_msb | trim_xy1xy2[2]);
              temp_msb = ((uint16_t)trim_xy1xy2[1]) << 8;
              dev->trim_data.dig_z2 = (int16_t)(temp_msb | trim_xy1xy2[0]);
              temp_msb = ((uint16_t)trim_xy1xy2[7]) << 8;
              dev->trim_data.dig_z3 = (int16_t)(temp_msb | trim_xy1xy2[6]);
              temp_msb = ((uint16_t)trim_xyz_data[1]) << 8;
              dev->trim_data.dig_z4 = (int16_t)(temp_msb | trim_xyz_data[0]);
              dev->trim_data.dig_xy1 = trim_xy1xy2[9];
              dev->trim_data.dig_xy2 = (int8_t)trim_xy1xy2[8];
              temp_msb = ((uint16_t)(trim_xy1xy2[5] & 0x7F)) << 8;
              dev->trim_data.dig_xyz1 = (uint16_t)(temp_msb | trim_xy1xy2[4]);
          }
      }
  }


  return rslt;
}

/*!
 * @brief This internal API writes the op_mode value in the Opmode bits
 * (bits 1 and 2) of 0x4C register.
 */
static int8_t write_op_mode(uint8_t op_mode, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* Read the 0x4C register */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
      if (rslt ==  BMM150_OK) {
          /* Set the op_mode value in Opmode bits of 0x4C */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_OP_MODE, op_mode);
          rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API sets the device from suspend to sleep mode
 * by setting the power control bit to '1' of 0x4B register
 */
static int8_t suspend_to_sleep_mode(struct bmm150_instance_state *dev)
{
  int8_t rslt;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      if (dev->settings.pwr_cntrl_bit == BMM150_POWER_CNTRL_DISABLE) {
          rslt = set_power_control_bit(BMM150_POWER_CNTRL_ENABLE, dev);
          /* Start-up time delay of 3ms*/
          dev->delay_ms(BMM150_START_UP_TIME);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API sets the xy repetition value in the 0x51 register.
 */
static int8_t set_xy_rep(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t rep_xy;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* set the xy repetition */
      rep_xy = dev->settings.xy_rep;
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_REP_XY_ADDR, &rep_xy, 1, dev);
  }

  return rslt;
}

/*!
 * @brief This internal API sets the z repetition value in the 0x52 register.
 */
static int8_t set_z_rep(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t rep_z;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /* set the z repetition */
      rep_z = dev->settings.z_rep;
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_REP_Z_ADDR, &rep_z, 1, dev);
  }

  return rslt;
}

/*!
 * @brief This internal API is used to set the output data rate of the sensor.
*/
static int8_t set_odr(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      /*Read the 0x4C register */
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
      if (rslt ==  BMM150_OK) {
          /*Set the ODR value */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_ODR, dev->settings.data_rate);
          rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API sets the preset mode ODR and repetition settings.
 */
static int8_t set_odr_xyz_rep(const struct bmm150_instance_state *dev)
{
  int8_t rslt;

  /* Set the ODR */
  rslt = set_odr(dev);
  if (rslt ==  BMM150_OK) {
      /* Set the XY-repetitions number */
      rslt = set_xy_rep(dev);
      if (rslt ==  BMM150_OK) {
          /* Set the Z-repetitions number */
          rslt = set_z_rep(dev);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to enable or disable the magnetic
 * measurement of x,y,z axes based on the value of xyz_axes_control.
 */
static int8_t set_control_measurement_xyz(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Check for null pointer in the device structure*/
  rslt = null_ptr_check(dev);
  /* Proceed if null check is fine */
  if (rslt ==  BMM150_OK) {
      rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_AXES_ENABLE_ADDR, &reg_data, 1, dev);
      if (rslt ==  BMM150_OK) {
          /* Set the axes to be enabled/disabled*/
          reg_data = BMM150_SET_BITS(reg_data, BMM150_CONTROL_MEASURE, dev->settings.xyz_axes_control);
          rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_AXES_ENABLE_ADDR, &reg_data, 1, dev);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to identify the settings which the user
 * wants to modify in the sensor.
 */
static uint8_t are_settings_changed(uint16_t sub_settings, uint16_t desired_settings)
{
  uint8_t settings_changed = FALSE;

  if (sub_settings & desired_settings) {
      /* User wants to modify this particular settings */
      settings_changed = TRUE;
  } else {
      /* User don't want to modify this particular settings */
      settings_changed = FALSE;
  }

  return settings_changed;
}

/*!
 * @brief This API sets the ODR , measurement axes control ,
 * repetition values of xy,z.
 */
static int8_t mode_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev)
{
  int8_t rslt = BMM150_E_INVALID_CONFIG;

  if (desired_settings & BMM150_DATA_RATE_SEL) {
      /* Sets the ODR */
      rslt = set_odr(dev);
  }
  if (desired_settings & BMM150_CONTROL_MEASURE_SEL) {
      /* Enables/Disables the control measurement axes */
      rslt = set_control_measurement_xyz(dev);
  }
  if (desired_settings & BMM150_XY_REP_SEL) {
      /* Sets the XY repetition */
      rslt = set_xy_rep(dev);
  }
  if (desired_settings & BMM150_Z_REP_SEL) {
      /* Sets the Z repetition */
      rslt = set_z_rep(dev);
  }

  return rslt;
}

/*!
 * @brief This internal API is used to parse and store the sensor
 * settings in the device structure
 */
static void parse_setting(const uint8_t *reg_data, struct bmm150_instance_state *dev)
{
  /* Parse all the w/r registers and update the
  current sensor settings in the dev structure*/
  dev->settings.z_rep = reg_data[7];
  dev->settings.xy_rep = reg_data[6];
  dev->settings.int_settings.high_threshold = reg_data[5];
  dev->settings.int_settings.low_threshold = reg_data[4];
  dev->settings.xyz_axes_control = BMM150_GET_BITS(reg_data[3], BMM150_CONTROL_MEASURE);
  dev->settings.int_settings.drdy_pin_en = BMM150_GET_BITS(reg_data[3], BMM150_DRDY_EN);
  dev->settings.int_settings.int_pin_en = BMM150_GET_BITS(reg_data[3], BMM150_INT_PIN_EN);
  dev->settings.int_settings.drdy_polarity = BMM150_GET_BITS(reg_data[3], BMM150_DRDY_POLARITY);
  dev->settings.int_settings.int_latch = BMM150_GET_BITS(reg_data[3], BMM150_INT_LATCH);
  dev->settings.int_settings.int_polarity = BMM150_GET_BITS_POS_0(reg_data[3], BMM150_INT_POLARITY);
  dev->settings.int_settings.data_overrun_en = BMM150_GET_BITS(reg_data[2], BMM150_DATA_OVERRUN_INT);
  dev->settings.int_settings.overflow_int_en = BMM150_GET_BITS(reg_data[2], BMM150_OVERFLOW_INT);
  dev->settings.int_settings.high_int_en = BMM150_GET_BITS(reg_data[2], BMM150_HIGH_THRESHOLD_INT);
  dev->settings.int_settings.low_int_en = BMM150_GET_BITS_POS_0(reg_data[2], BMM150_LOW_THRESHOLD_INT);
  dev->settings.data_rate = BMM150_GET_BITS(reg_data[1], BMM150_ODR);
}

/*!
 * @brief This API is used to enable the interrupts and map them to the
 * corresponding interrupt pins and specify the pin characteristics like the
 * polarity , latch settings for the interrupt pins.
 */
static int8_t interrupt_pin_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;
  struct bmm150_int_ctrl_settings int_settings;

  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_AXES_ENABLE_ADDR, &reg_data, 1, dev);

  if (rslt == BMM150_OK) {
      int_settings = dev->settings.int_settings;

      if (desired_settings & BMM150_DRDY_PIN_EN_SEL) {
          /* Enables the Data ready interrupt and
          maps it to the DRDY pin of the sensor  */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_DRDY_EN, int_settings.drdy_pin_en);
      }
      if (desired_settings & BMM150_INT_PIN_EN_SEL) {
          /* Sets interrupt pin enable */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_INT_PIN_EN, int_settings.int_pin_en);
      }
      if (desired_settings & BMM150_DRDY_POLARITY_SEL) {
          /* Sets Data ready pin's polarity */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_DRDY_POLARITY, int_settings.drdy_polarity);
      }
      if (desired_settings & BMM150_INT_LATCH_SEL) {
          /* Sets Interrupt in latched or non-latched mode */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_INT_LATCH, int_settings.int_latch);
      }
      if (desired_settings & BMM150_INT_POLARITY_SEL) {
          /* Sets Interrupt pin's polarity */
          reg_data = BMM150_SET_BITS_POS_0(reg_data, BMM150_INT_POLARITY, int_settings.int_polarity);
      }

      /* Set the interrupt configurations in the 0x4E register */
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_AXES_ENABLE_ADDR, &reg_data, 1, dev);
  }

  return rslt;
}

/*!
 * @brief This API is used to enable data overrun , overflow interrupts and
 * enable/disable high/low threshold interrupts for x,y,z axis based on the
 * threshold values set by the user in the High threshold (0x50) and
 * Low threshold (0x4F) registers.
 */
static int8_t interrupt_config(uint16_t desired_settings, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;
  struct bmm150_int_ctrl_settings int_settings;

  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_INT_CONFIG_ADDR, &reg_data, 1, dev);

  if (rslt == BMM150_OK) {
      int_settings = dev->settings.int_settings;

      if (desired_settings & BMM150_DATA_OVERRUN_INT_SEL) {
          /* Sets Data overrun interrupt */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_DATA_OVERRUN_INT, int_settings.data_overrun_en);
      }
      if (desired_settings & BMM150_OVERFLOW_INT_SEL) {
          /* Sets Data overflow interrupt */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_OVERFLOW_INT, int_settings.overflow_int_en);
      }
      if (desired_settings & BMM150_HIGH_THRESHOLD_INT_SEL) {
          /* Sets high threshold interrupt */
          reg_data = BMM150_SET_BITS(reg_data, BMM150_HIGH_THRESHOLD_INT, int_settings.high_int_en);
      }
      if (desired_settings & BMM150_LOW_THRESHOLD_INT_SEL) {
          /* Sets low threshold interrupt */
          reg_data = BMM150_SET_BITS_POS_0(reg_data, BMM150_LOW_THRESHOLD_INT, int_settings.low_int_en);
      }

      /* Set the interrupt configurations in the 0x4D register */
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_INT_CONFIG_ADDR, &reg_data, 1, dev);
  }

  return rslt;
}

/*!
 * @brief This API is used to write the user specified High/Low threshold value
 * as a reference to generate the high/low threshold interrupt.
 */
static int8_t interrupt_threshold_settings(uint16_t desired_settings, const struct bmm150_instance_state *dev)
{
  int8_t rslt = BMM150_E_INVALID_CONFIG;
  uint8_t reg_data;

  if (desired_settings & BMM150_LOW_THRESHOLD_SETTING_SEL) {
      /* Sets the Low threshold value to trigger interrupt */
      reg_data = dev->settings.int_settings.low_threshold;
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_LOW_THRESHOLD_ADDR, &reg_data, 1, dev);
  }

  if (desired_settings & BMM150_HIGH_THRESHOLD_SETTING_SEL) {
      /* Sets the High threshold value to trigger interrupt */
      reg_data = dev->settings.int_settings.high_threshold;
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_HIGH_THRESHOLD_ADDR, &reg_data, 1, dev);
  }

  return rslt;

}

#ifdef BMM150_USE_FLOATING_POINT
/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer x axis data(micro-tesla) in float.
 */
static float compensate_x(int16_t mag_data_x, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  float retval = 0;
  float process_comp_x0;
  float process_comp_x1;
  float process_comp_x2;
  float process_comp_x3;
  float process_comp_x4;

  /* Overflow condition check */
  if ((mag_data_x != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL) &&
      (data_rhall != 0) && (dev->trim_data.dig_xyz1 != 0)) {
          /*Processing compensation equations*/
          process_comp_x0 = (((float)dev->trim_data.dig_xyz1) * 16384.0f / data_rhall);
          retval = (process_comp_x0 - 16384.0f);
          process_comp_x1 = ((float)dev->trim_data.dig_xy2) * (retval * retval / 268435456.0f);
          process_comp_x2 = process_comp_x1 + retval * ((float)dev->trim_data.dig_xy1) / 16384.0f;
          process_comp_x3 = ((float)dev->trim_data.dig_x2) + 160.0f;
          process_comp_x4 = mag_data_x * ((process_comp_x2 + 256.0f) * process_comp_x3);
          retval = ((process_comp_x4 / 8192.0f) + (((float)dev->trim_data.dig_x1) * 8.0f)) / 16.0f;
  } else {
      /* overflow, set output to 0.0f */
      retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer y axis data(micro-tesla) in float.
 */
static float compensate_y(int16_t mag_data_y, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  float retval = 0;
  float process_comp_y0;
  float process_comp_y1;
  float process_comp_y2;
  float process_comp_y3;
  float process_comp_y4;

  /* Overflow condition check */
  if ((mag_data_y != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL)
      && (data_rhall != 0) && (dev->trim_data.dig_xyz1 != 0)) {
          /*Processing compensation equations*/
          process_comp_y0 = ((float)dev->trim_data.dig_xyz1) * 16384.0f / data_rhall;
          retval = process_comp_y0 - 16384.0f;
          process_comp_y1 = ((float)dev->trim_data.dig_xy2) * (retval * retval / 268435456.0f);
          process_comp_y2 = process_comp_y1 + retval * ((float)dev->trim_data.dig_xy1) / 16384.0f;
          process_comp_y3 = ((float)dev->trim_data.dig_y2) + 160.0f;
          process_comp_y4 = mag_data_y * (((process_comp_y2) + 256.0f) * process_comp_y3);
          retval = ((process_comp_y4 / 8192.0f) + (((float)dev->trim_data.dig_y1) * 8.0f)) / 16.0f;
  } else {
      /* overflow, set output to 0.0f */
      retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer z axis data(micro-tesla) in float.
 */
static float compensate_z(int16_t mag_data_z, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  float retval = 0;
  float process_comp_z0;
  float process_comp_z1;
  float process_comp_z2;
  float process_comp_z3;
  float process_comp_z4;
  float process_comp_z5;

   /* Overflow condition check */
  if ((mag_data_z != BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL) &&
      (dev->trim_data.dig_z2 != 0) && (dev->trim_data.dig_z1 != 0)
      && (dev->trim_data.dig_xyz1 != 0) && (data_rhall != 0)) {
          /* Processing compensation equations */
          process_comp_z0 = ((float)mag_data_z) - ((float)dev->trim_data.dig_z4);
          process_comp_z1 = ((float)data_rhall) - ((float)dev->trim_data.dig_xyz1);
          process_comp_z2 = (((float)dev->trim_data.dig_z3) * process_comp_z1);
          process_comp_z3 = ((float)dev->trim_data.dig_z1) * ((float)data_rhall) / 32768.0f;
          process_comp_z4 = ((float)dev->trim_data.dig_z2) + process_comp_z3;
          process_comp_z5 = (process_comp_z0 * 131072.0f) - process_comp_z2;
          retval = (process_comp_z5 / ((process_comp_z4) * 4.0f)) / 16.0f;
  } else {
      /* overflow, set output to 0.0f */
      retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
  }

  return retval;
}

#else

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer X axis data(micro-tesla) in int16_t.
 */
static int16_t compensate_x(int16_t mag_data_x, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  int16_t retval;
  uint16_t process_comp_x0 = 0;
  int32_t process_comp_x1;
  uint16_t process_comp_x2;
  int32_t process_comp_x3;
  int32_t process_comp_x4;
  int32_t process_comp_x5;
  int32_t process_comp_x6;
  int32_t process_comp_x7;
  int32_t process_comp_x8;
  int32_t process_comp_x9;
  int32_t process_comp_x10;

  /* Overflow condition check */
  if (mag_data_x != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL) {
      if (data_rhall != 0) {
          /* Availability of valid data*/
          process_comp_x0 = data_rhall;
      } else if (dev->trim_data.dig_xyz1 != 0) {
          process_comp_x0 = dev->trim_data.dig_xyz1;
      } else {
          process_comp_x0 = 0;
      }
      if (process_comp_x0 != 0) {
          /* Processing compensation equations*/
          process_comp_x1 = ((int32_t)dev->trim_data.dig_xyz1) * 16384;
          process_comp_x2 = ((uint16_t)(process_comp_x1 / process_comp_x0)) - ((uint16_t)0x4000);
          retval = ((int16_t)process_comp_x2);
          process_comp_x3 = (((int32_t)retval) * ((int32_t)retval));
          process_comp_x4 = (((int32_t)dev->trim_data.dig_xy2) * (process_comp_x3 / 128));
          process_comp_x5 = (int32_t)(((int16_t)dev->trim_data.dig_xy1) * 128);
          process_comp_x6 = ((int32_t)retval) * process_comp_x5;
          process_comp_x7 = (((process_comp_x4 + process_comp_x6) / 512) + ((int32_t)0x100000));
          process_comp_x8 = ((int32_t)(((int16_t)dev->trim_data.dig_x2) + ((int16_t)0xA0)));
          process_comp_x9 = ((process_comp_x7 * process_comp_x8) / 4096);
          process_comp_x10 = ((int32_t)mag_data_x) * process_comp_x9;
          retval = ((int16_t)(process_comp_x10 / 8192));
          retval = (retval + (((int16_t)dev->trim_data.dig_x1) * 8)) / 16;
      } else {
          retval = BMM150_OVERFLOW_OUTPUT;
      }
  } else {
      /* Overflow condition */
      retval = BMM150_OVERFLOW_OUTPUT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Y axis data(micro-tesla) in int16_t.
 */
static int16_t compensate_y(int16_t mag_data_y, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  int16_t retval;
  uint16_t process_comp_y0 = 0;
  int32_t process_comp_y1;
  uint16_t process_comp_y2;
  int32_t process_comp_y3;
  int32_t process_comp_y4;
  int32_t process_comp_y5;
  int32_t process_comp_y6;
  int32_t process_comp_y7;
  int32_t process_comp_y8;
  int32_t process_comp_y9;

  /* Overflow condition check */
  if (mag_data_y != BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL) {
      if (data_rhall != 0) {
          /* Availability of valid data*/
          process_comp_y0 = data_rhall;
      } else if (dev->trim_data.dig_xyz1 != 0) {
          process_comp_y0 = dev->trim_data.dig_xyz1;
      } else {
          process_comp_y0 = 0;
      }
      if (process_comp_y0 != 0) {
          /*Processing compensation equations*/
          process_comp_y1 = (((int32_t)dev->trim_data.dig_xyz1) * 16384) / process_comp_y0;
          process_comp_y2 = ((uint16_t)process_comp_y1) - ((uint16_t)0x4000);
          retval = ((int16_t)process_comp_y2);
          process_comp_y3 = ((int32_t) retval) * ((int32_t)retval);
          process_comp_y4 = ((int32_t)dev->trim_data.dig_xy2) * (process_comp_y3 / 128);
          process_comp_y5 = ((int32_t)(((int16_t)dev->trim_data.dig_xy1) * 128));
          process_comp_y6 = ((process_comp_y4 + (((int32_t)retval) * process_comp_y5)) / 512);
          process_comp_y7 = ((int32_t)(((int16_t)dev->trim_data.dig_y2) + ((int16_t)0xA0)));
          process_comp_y8 = (((process_comp_y6 + ((int32_t)0x100000)) * process_comp_y7) / 4096);
          process_comp_y9 = (((int32_t)mag_data_y) * process_comp_y8);
          retval = (int16_t)(process_comp_y9 / 8192);
          retval = (retval + (((int16_t)dev->trim_data.dig_y1) * 8)) / 16;
      } else {
          retval = BMM150_OVERFLOW_OUTPUT;
      }
  } else {
      /* Overflow condition*/
      retval = BMM150_OVERFLOW_OUTPUT;
  }

  return retval;
}

/*!
 * @brief This internal API is used to obtain the compensated
 * magnetometer Z axis data(micro-tesla) in int16_t.
 */
static int16_t compensate_z(int16_t mag_data_z, uint16_t data_rhall, const struct bmm150_instance_state *dev)
{
  int32_t retval;
  int16_t process_comp_z0;
  int32_t process_comp_z1;
  int32_t process_comp_z2;
  int32_t process_comp_z3;
  int16_t process_comp_z4;

  if (mag_data_z != BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL) {
      if ((dev->trim_data.dig_z2 != 0) && (dev->trim_data.dig_z1 != 0)
      && (data_rhall != 0) && (dev->trim_data.dig_xyz1 != 0)) {
          /*Processing compensation equations*/
          process_comp_z0 = ((int16_t)data_rhall) - ((int16_t) dev->trim_data.dig_xyz1);
          process_comp_z1 = (((int32_t)dev->trim_data.dig_z3) * ((int32_t)(process_comp_z0))) / 4;
          process_comp_z2 = (((int32_t)(mag_data_z - dev->trim_data.dig_z4)) * 32768);
          process_comp_z3 = ((int32_t)dev->trim_data.dig_z1) * (((int16_t)data_rhall) * 2);
          process_comp_z4 = (int16_t)((process_comp_z3 + (32768)) / 65536);
          retval = ((process_comp_z2 - process_comp_z1) / (dev->trim_data.dig_z2 + process_comp_z4));

          /* saturate result to +/- 2 micro-tesla */
          if (retval > BMM150_POSITIVE_SATURATION_Z) {
              retval =  BMM150_POSITIVE_SATURATION_Z;
          } else {
              if (retval < BMM150_NEGATIVE_SATURATION_Z)
                  retval = BMM150_NEGATIVE_SATURATION_Z;
          }
          /* Conversion of LSB to micro-tesla*/
          retval = retval / 16;
      } else {
          retval = BMM150_OVERFLOW_OUTPUT;

      }
  } else {
      /* Overflow condition*/
      retval = BMM150_OVERFLOW_OUTPUT;
  }

  return (int16_t)retval;
}

#endif

/*!
 * @brief This internal API is used to perform the normal self test
 * of the sensor and return the self test result as return value
 */
static int8_t perform_normal_self_test(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t self_test_bit;

  /* Triggers the start of normal self test */
  rslt = enable_normal_self_test(&self_test_bit, dev);
      /* Check for self test completion status */
      if ((rslt == BMM150_OK) && (self_test_bit == 0)) {
          /* Validates the self test results for all 3 axes */
          rslt = validate_normal_self_test(dev);
      }

  return rslt;
}

/*!
 * @brief This internal API is used to enable the normal self test by setting
 * the Self Test bit (bit0) of the 0x4C register,
 * which triggers the start of self test
 */
static int8_t enable_normal_self_test(uint8_t *self_test_enable, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;
  uint8_t self_test_val;

  /* Read the data from register 0x4C */
  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
  if (rslt == BMM150_OK) {
      /* Set the Self Test bit(bit0) of the 0x4C register */
      self_test_val = 1;
      reg_data = BMM150_SET_BITS_POS_0(reg_data, BMM150_SELF_TEST, self_test_val);
      /* Write the data to 0x4C register to trigger self test */
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
      dev->delay_ms(BMM150_NORMAL_SELF_TEST_DELAY);
      if (rslt == BMM150_OK) {
          /* Read the data from register 0x4C */
          rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
          /* Self Test bit(bit0) is stored in self_test_enable,
          It will be reset to zero after the self test is over */
          *self_test_enable = BMM150_GET_BITS_POS_0(reg_data, BMM150_SELF_TEST);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to validate the results of normal self test
 * by using the self test status available in the bit0 of registers 0x42,0x44
 * and 0x46.
 */
static int8_t validate_normal_self_test(const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t status;
  uint8_t self_test_rslt[5];

  /* Read the data from register 0x42 to 0x46 */
  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_DATA_X_LSB, self_test_rslt, BMM150_SELF_TEST_LEN, dev);
  if (rslt == BMM150_OK) {
      /* Parse and get the self test status bits */
      /* X-Self-Test (bit0) of 0x42 register is stored*/
      self_test_rslt[0] = BMM150_GET_BITS_POS_0(self_test_rslt[0], BMM150_SELF_TEST);
      /* Y-Self-Test (bit0) of 0x44 register is stored */
      self_test_rslt[2] = BMM150_GET_BITS_POS_0(self_test_rslt[2], BMM150_SELF_TEST);
      /* Z-Self-Test (bit0) of 0x46 register is stored */
      self_test_rslt[4] = BMM150_GET_BITS_POS_0(self_test_rslt[4], BMM150_SELF_TEST);
      /* Combine the self test status and store it in the first
      3 bits of the status variable for processing*/
      status = (uint8_t)((self_test_rslt[4] << 2) | (self_test_rslt[2] << 1) | self_test_rslt[0]);
      /* Validate status and store Self test result in "rslt" */
      if (status == BMM150_SELF_TEST_STATUS_SUCCESS) {
          /* Self test is success when all status bits are set */
          rslt = BMM150_OK;
      } else {
          if (status == BMM150_SELF_TEST_STATUS_XYZ_FAIL) {
              /* Self test - all axis fail condition */
              rslt = BMM150_W_NORMAL_SELF_TEST_XYZ_FAIL;
          } else {
              /* Self test - some axis fail condition */
              rslt = (int8_t)status;
          }
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to perform advanced self test for Z axis
 */
static int8_t perform_adv_self_test(struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t self_test_current;
  int16_t positive_data_z;
  int16_t negative_data_z;

  /* Set the desired power mode ,axes control and repetition settings */
  rslt = adv_self_test_settings(dev);
  if (rslt == BMM150_OK) {
      /* Measure the Z axes data with positive self-test current */
      self_test_current = BMM150_ENABLE_POSITIVE_CURRENT;
      rslt = adv_self_test_measurement(self_test_current, &positive_data_z, dev);
      if (rslt == BMM150_OK) {
          /* Measure the Z axes data with
          negative self-test current */
          self_test_current = BMM150_ENABLE_NEGATIVE_CURRENT;
          rslt = adv_self_test_measurement(self_test_current, &negative_data_z, dev);
          if (rslt == BMM150_OK) {
              /* Disable self-test current */
              self_test_current = BMM150_DISABLE_SELF_TEST_CURRENT;
              rslt = set_adv_self_test_current(self_test_current, dev);
              if (rslt == BMM150_OK) {
                  /* Validate the advanced self test */
                  rslt = validate_adv_self_test(positive_data_z, negative_data_z);
              }
          }
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to set the desired power mode ,
 * axes control and repetition settings for advanced self test
 */
static int8_t adv_self_test_settings(struct bmm150_instance_state *dev)
{
  int8_t rslt;

  /* Set the power mode as sleep mode */
  dev->settings.pwr_mode = BMM150_SLEEP_MODE;
  rslt = bmm150_set_op_mode(dev);
  if (rslt == BMM150_OK) {
      /* Disable XY-axis measurement */
      dev->settings.xyz_axes_control = BMM150_DISABLE_XY_AXIS;
      rslt = set_control_measurement_xyz(dev);
      if (rslt == BMM150_OK) {
          /* Repetition value is set as 0x04 */
          dev->settings.z_rep = BMM150_SELF_TEST_REP_Z;
          rslt = set_z_rep(dev);
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to set the positive or negative value of
 * self-test current and obtain the corresponding magnetometer z axis data
 */
static int8_t adv_self_test_measurement(uint8_t self_test_current, int16_t *data_z, struct bmm150_instance_state *dev)
{
  int8_t rslt;

  /* Set the advanced self test current as positive or
  negative based on the value of parameter "self_test_current" */
  rslt = set_adv_self_test_current(self_test_current, dev);
  if (rslt == BMM150_OK) {
      /* Set the device in forced mode*/
      dev->settings.pwr_mode = BMM150_FORCED_MODE;
      rslt = bmm150_set_op_mode(dev);
      /* Delay to ensure measurement is complete */
      dev->delay_ms(BMM150_ADV_SELF_TEST_DELAY);
      if (rslt == BMM150_OK) {
          /* Read Mag data and store the value of Z axis data */
          rslt = bmm150_read_mag_data(dev);
          if (rslt == BMM150_OK) {
              /* Mag Z axis data is stored */
              *data_z = dev->data.z;
          }
      }
  }

  return rslt;
}

/*!
 * @brief This internal API is used to get the difference between the
 * Z axis mag data obtained by positive and negative self-test current
 * and validate whether the advanced self test is done successfully or not.
 */
static int8_t validate_adv_self_test(int16_t positive_data_z, int16_t negative_data_z)
{
  int32_t adv_self_test_rslt;
  int8_t rslt;

  /* Advanced self test difference between the Z axis mag data
     obtained by the positive and negative self-test current */
  adv_self_test_rslt = positive_data_z - negative_data_z;
  /* Advanced self test validation */
  /*Value of adv_self_test_rslt should be in between 180-240 micro-tesla*/
  if ((adv_self_test_rslt > 180) && (adv_self_test_rslt < 240)) {
      /* Advanced self test success */
      rslt = BMM150_OK;
  } else {
      /* Advanced self test fail */
      rslt = BMM150_W_ADV_SELF_TEST_FAIL;
  }

  return rslt;
}

/*!
 * @brief This internal API is used to set the self test current value in
 * the Adv. ST bits (bit6 and bit7) of 0x4C register
 */
static int8_t set_adv_self_test_current(uint8_t self_test_current, const struct bmm150_instance_state *dev)
{
  int8_t rslt;
  uint8_t reg_data;

  /* Read the 0x4C register */
  rslt = bmm150_get_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
  if (rslt == BMM150_OK) {
      /* Set the self test current value in the Adv. ST bits
      (bit6 and bit7) of 0x4c register */
      reg_data = BMM150_SET_BITS(reg_data, BMM150_ADV_SELF_TEST, self_test_current);
      rslt = bmm150_set_regs(dev->scp_service, dev->com_port_info.port_handle, BMM150_OP_MODE_ADDR, &reg_data, 1, dev);
  }

  return rslt;
}

void bmm150_delay (uint32_t period)
{
  UNUSED_VAR(period);
}

void bmm150_handle_temperature_data_sample(
    sns_sensor_instance *const instance,
    float sample_data,
    sns_time timestamp)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  sns_sensor_uid suid;

  suid = state->temperature_info.suid;

  pb_send_sensor_stream_event(instance,
      &suid,
      timestamp,
      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
      status,
      &sample_data,
      1,
      state->encoded_temp_event_len);

}

void bmm150_handle_mag_data_sample(
    sns_sensor_instance *const instance,
    const struct bmm150_mag_data data,
    sns_time timestamp)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;
  sns_sensor_uid suid;

  suid = state->mag_info.suid;
  float ipdata[TRIAXIS_NUM] = {0},opdata_raw[TRIAXIS_NUM] = {0};
  uint8_t i = 0;
  ipdata[TRIAXIS_X] = data.x;
  ipdata[TRIAXIS_Y] = data.y;
  ipdata[TRIAXIS_Z] = data.z;
  BMM_INST_LOG(LOW, instance, "data *1000 x %d y %d z %d timestamp = %u", 
    (int32_t)(data.x * 1000), (int32_t)(data.y * 1000), (int32_t)(data.z * 1000), (uint32_t)timestamp);
  // axis conversion
  for(i = 0; i < TRIAXIS_NUM; i++)
  {
    opdata_raw[state->axis_map[i].opaxis] = (state->axis_map[i].invert ? -1.0 : 1.0) *
      ipdata[state->axis_map[i].ipaxis];
  }
#if BMM_CONFIG_ENABLE_SELF_TEST_FAC
  if(state->fac_test_in_progress && state->fac_test_sensor == BMM150_MAG)
  {
    state->fac_test_info.num_samples++;
    for(i = 0; i < TRIAXIS_NUM; i++)
    {
      state->fac_test_info.sample_sum[i] += (opdata_raw[i]);
      state->fac_test_info.sample_square_sum[i] +=
          ((int64_t)(opdata_raw[i]) * (int64_t)(opdata_raw[i]));
    }
  }
#endif
  // factory calibration
  vector3 opdata_cal = sns_apply_calibration_correction_3(
      make_vector3_from_array(opdata_raw),
      make_vector3_from_array(state->mag_registry_cfg.fac_cal_bias),
      state->mag_registry_cfg.fac_cal_corr_mat);
if (state->sample_cnt_to_skip_odrc)
{
  state->sample_cnt_to_skip_odrc--;
  status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
}
BMM_INST_LOG(LOW, instance, "after data *1000 x %d y %d z %d status = %u", 
  (int32_t)(data.x * 1000), (int32_t)(data.y * 1000), (int32_t)(data.z * 1000), status);

#if BMM_CONFIG_ENABLE_DIAG_LOG
  sns_diag_service* diag       = state->diag_service;
  log_sensor_state_raw_info log_sensor_state_raw_info;
  sns_memzero(&log_sensor_state_raw_info, sizeof(log_sensor_state_raw_info));
  log_sensor_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
  log_sensor_state_raw_info.diag = diag;
  log_sensor_state_raw_info.instance = instance;
  log_sensor_state_raw_info.sensor_uid = &suid;
  bmm150_log_sensor_state_raw_alloc(&log_sensor_state_raw_info, 0);
#endif

  pb_send_sensor_stream_event(instance,
      &suid,
      timestamp,
      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
      status,
      opdata_cal.data,
      ARR_SIZE(opdata_cal.data),
      state->encoded_imu_event_len);
#if BMM_CONFIG_ENABLE_DIAG_LOG
  bmm150_log_sensor_state_raw_add(
        &log_sensor_state_raw_info,
        opdata_raw,
        timestamp,
        status);
  bmm150_log_sensor_state_raw_submit(&log_sensor_state_raw_info, true);
#endif
}
#if BMM_CONFIG_ENABLE_DAE
void bmm150_convert_and_send_mag_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       data[6])
{
  UNUSED_VAR(instance);
  UNUSED_VAR(timestamp);
  UNUSED_VAR(data[6]);

/*
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  s32 v_uncomp_mag_s32 = 0;
  float temperature = 0;
  float bmm150_comp_data_buffer = 0;

  v_uncomp_mag_s32 = (s32)((((u32)(
          data[0]))
          << bmm150_SHIFT_BIT_POSITION_BY_12_BITS)
          | (((u32)(data[1]))
          << bmm150_SHIFT_BIT_POSITION_BY_04_BITS)
          | ((u32)data[2]
          >> bmm150_SHIFT_BIT_POSITION_BY_04_BITS));
  v_uncomp_temperature_s32 = (s32)((((u32)(
        data[3]))
        << bmm150_SHIFT_BIT_POSITION_BY_12_BITS)
        | (((u32)(
        data[4]))
        << bmm150_SHIFT_BIT_POSITION_BY_04_BITS)
        | ((u32)data[5]
        >> bmm150_SHIFT_BIT_POSITION_BY_04_BITS));
  temperature = bmm150_compensate_temperature_double(state,v_uncomp_temperature_s32);
  bmm150_comp_data_buffer = bmm150_compensate_mag_double(state, v_uncomp_mag_s32) / 100;
  bmm150_handle_mag_data_sample(instance, bmm150_comp_data_buffer, timestamp);
  */
}

void bmm150_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       data[3])
{
  UNUSED_VAR(instance);
  UNUSED_VAR(timestamp);
  UNUSED_VAR(data[3]);

/*
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  float bmm150_comp_data_buffer = 0;

  v_uncomp_temperature_s32 = (s32)((((u32)(
          data[0]))
          << bmm150_SHIFT_BIT_POSITION_BY_12_BITS)
          | (((u32)(data[1]))
          << bmm150_SHIFT_BIT_POSITION_BY_04_BITS)
          | ((u32)data[2]
          >> bmm150_SHIFT_BIT_POSITION_BY_04_BITS));

  bmm150_comp_data_buffer = bmm150_compensate_temperature_double(state,v_uncomp_temperature_s32);
  bmm150_handle_temperature_data_sample(instance, bmm150_comp_data_buffer, timestamp);
  */
}
#endif
#define	ABS(a)		((a) > 0 ? (a) : -(a)) /*!< Absolute value */
void bmm150_process_mag_timer_stream_data_buffer(sns_sensor_instance *instance, sns_timer_sensor_event * const timer_event)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  int err = 0;
  sns_time report_timestamp;
  int64_t delet_time = 0;
  err = bmm150_read_mag_data(state);
  if (err)
    SNS_INST_PRINTF(ERROR, instance,"read data failed failed err = %d", err);
  if (state->old_timestamp == 0)
  {
    report_timestamp = timer_event->timeout_time;
  }
  else
  {
    if (((timer_event->timeout_time - state->old_timestamp) > (0.98 * state->mag_info.sampling_intvl))
      &&(((timer_event->timeout_time - state->old_timestamp) < (1.02 * state->mag_info.sampling_intvl))))
    {
      report_timestamp = timer_event->timeout_time;
    }
    else
    {
      report_timestamp = state->old_timestamp + state->mag_info.sampling_intvl;
    }
  }
  uint8_t data[2] = {0};
  /*Read the mag data registers */
  err  = bmm150_get_regs(state->scp_service, state->com_port_info.port_handle, BMM150_REP_XY_ADDR,
  data, 2, state);
  if (err)
    SNS_INST_PRINTF(ERROR, instance,"read data failed failed err = %d", err);
  bmm150_handle_mag_data_sample(instance, state->data, report_timestamp);
  state->old_timestamp = report_timestamp;
  delet_time = timer_event->timeout_time - report_timestamp;
  if(ABS(delet_time) > (0.98 * state->mag_info.sampling_intvl))
  {
    BMM_INST_LOG(LOW, instance, "rebase timestamp = %u",(uint32_t)(timer_event->timeout_time - report_timestamp));
    state->old_timestamp = timer_event->timeout_time;
    report_timestamp = timer_event->timeout_time;
  }
  BMM_INST_LOG(LOW, instance, "deltet timestamp = %u reg51_data[0] = 0x%x reg52_data[1] = 0x%x",
    (uint32_t)(timer_event->timeout_time - report_timestamp), data[0], data[1]);
}

void bmm150_process_temperature_timer_stream_data_buffer(sns_sensor_instance *instance, sns_timer_sensor_event * const timer_event)
{
  UNUSED_VAR(instance);
  UNUSED_VAR(timer_event);
/*
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  s32 v_uncomp_temperature_s32 = 0;
  float bmm150_comp_data_buffer = 0;

  bmm150_read_uncomp_temperature(state, &v_uncomp_temperature_s32);
  bmm150_comp_data_buffer = bmm150_compensate_temperature_double(state,v_uncomp_temperature_s32);
  sns_time timestamp = state->interrupt_timestamp;
  bmm150_handle_temperature_data_sample(instance, bmm150_comp_data_buffer, timestamp);
  */
}

void bmm150_handle_mag_data_stream_timer_event(sns_sensor_instance *const instance, sns_timer_sensor_event * const timer_event)
{
  bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  int err = 0;
  bmm150_process_mag_timer_stream_data_buffer(instance, timer_event);
  state->settings.pwr_mode = BMM150_FORCED_MODE;
  err = bmm150_set_op_mode(state);
  if (err)
     SNS_INST_PRINTF(ERROR, instance,"config power mode failed err = %d", err);
}

void bmm150_handle_temperature_data_stream_timer_event(sns_sensor_instance *const instance, sns_timer_sensor_event * const timer_event)
{
  //bmm150_instance_state *state = (bmm150_instance_state*)instance->state->state;
  int err = 0;
  SNS_INST_PRINTF(LOW, instance, "bmm150_handle_temperature_data_stream_timer_event");
  bmm150_process_temperature_timer_stream_data_buffer(instance, timer_event);
  //err = bmm150_set_power_mode(state,bmm150_FORCED_MODE);
  if (err)
    SNS_INST_PRINTF(ERROR, instance,"config power mode failed err = %d", err);
}
