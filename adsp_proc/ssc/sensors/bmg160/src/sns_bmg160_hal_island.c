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

#include "sns_bmg160_hal.h"
#include "sns_bmg160_sensor.h"
#include "sns_bmg160_sensor_instance.h"

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
#include "sns_bmg160_cfg.h"
#include "sns_cal.pb.h"
//#include "sns_vendor_profiler.h"
//#include "tracer_event_ids.h"

/** Need to use ODR table. */
extern const bst_odr_conf_pair bmg160_config_map[BMG160_REG_MAP_TABLE_SIZE];

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
  float sample[BMG160_NUM_AXES];
  /* Data status.*/
  sns_std_sensor_sample_status status;
} bmg160_batch_sample;

// <diag log>
#if BMG160_CONFIG_ENABLE_DIAG_LOG
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
sns_rc bmg160_encode_sensor_state_log_interrupt(
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
static sns_rc bmg160_encode_log_sensor_state_raw(
  void *log, size_t log_size, size_t encoded_log_size, void *encoded_log,
  size_t *bytes_written)
{
  sns_rc rc = SNS_RC_SUCCESS;
  uint32_t i = 0;
  size_t encoded_sample_size = 0;
  size_t parsed_log_size = 0;
  sns_diag_batch_sample batch_sample = sns_diag_batch_sample_init_default;
  uint8_t arr_index = 0;
  float temp[BMG160_NUM_AXES];
  pb_float_arr_arg arg = {.arr = (float *)temp, .arr_len = BMG160_NUM_AXES,
    .arr_index = &arr_index};

  if(NULL == encoded_log || NULL == log || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  batch_sample.sample.funcs.encode = pb_encode_float_arr_cb;
  batch_sample.sample.arg = &arg;

  if(!pb_get_encoded_size(&encoded_sample_size, sns_diag_batch_sample_fields,
                          &batch_sample))
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);
  bmg160_batch_sample *raw_sample = (bmg160_batch_sample *)log;

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

    parsed_log_size += sizeof(bmg160_batch_sample);
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
static void bmg160_log_sensor_state_raw_alloc(
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
static void bmg160_log_sensor_state_raw_submit(
  log_sensor_state_raw_info *log_raw_info,
  bool batch_complete)
{
  bmg160_batch_sample *sample = NULL;

  if(NULL == log_raw_info || NULL == log_raw_info->diag ||
     NULL == log_raw_info->instance || NULL == log_raw_info->sensor_uid ||
     NULL == log_raw_info->log)
  {
    return;
  }

  sample = (bmg160_batch_sample *)log_raw_info->log;

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
        bmg160_encode_log_sensor_state_raw);

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
 * SNS_RC_SUCCESS if encoding was successful
 * SNS_RC_FAILED otherwise
 */
static sns_rc bmg160_log_sensor_state_raw_add(
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

  if( (log_raw_info->bytes_written + sizeof(bmg160_batch_sample)) >
     log_raw_info->log_size)
  {
    // no more space in log packet
    // submit and allocate a new one
    bmg160_log_sensor_state_raw_submit(log_raw_info, false);
    bmg160_log_sensor_state_raw_alloc(log_raw_info, 0);
  }

  if(NULL == log_raw_info->log)
  {
    rc = SNS_RC_FAILED;
  }
  else
  {
    bmg160_batch_sample *sample =
        (bmg160_batch_sample *)log_raw_info->log;

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

    log_raw_info->bytes_written += sizeof(bmg160_batch_sample);

    log_raw_info->log_sample_cnt++;
    log_raw_info->batch_sample_cnt++;
  }

  return rc;
}
#endif
// </diag log>

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
static sns_rc bmg160_com_read_wrapper(
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
static sns_rc bmg160_com_write_wrapper(
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


/**
 * soft reset the sensor
 * @param scp_service    synchronized com port service
 * @param port_handle    com port handler
 * @param sensor         sensor id
 * @return
 */
static sns_rc bmg160_device_sw_reset(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle,
                               bmg160_sensor_type sensor)
{
  UNUSED_VAR(sensor);
  uint8_t buffer[1];
  sns_rc rv = SNS_RC_SUCCESS;

  buffer[0] = BMG160_REGV_SOFT_RESET_VALUE;
  rv = sns_see_bmg160_write_data(
                             scp_service,
                             port_handle,
                             BMG160_REGA_BA_SOFTRESET_ADDR,
                             &buffer[0],
                             1);
  if(rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  // delay after sensor soft-reset
  sns_busy_wait(sns_convert_ns_to_ticks(BMG160_DELAY_MS_AFTER_SOFT_RESET * 1000 * 1000));
  return SNS_RC_SUCCESS;
}


/**
 * see sns_bmg160_hal.h
 */
sns_rc bmg160_device_set_default_state(sns_sync_com_port_service * scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        bmg160_sensor_type sensor)
{
  UNUSED_VAR(scp_service);
  UNUSED_VAR(port_handle);
  UNUSED_VAR(sensor);
  return SNS_RC_SUCCESS;
}



/** see sns_bmg160_sensor_instance.h */
sns_rc bmg160_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            uint8_t *buffer)
{
  return sns_see_bmg160_read_data(scp_service,
                                port_handle,
                                BMG160_REGA_CHIP_ID,
                                buffer,
                                1);
}


/**
 * configure the gyro power state
 * @param state   instance state handler
 * @param pst     power state
 * @return  SNS_RC_SUCCESS  success
 *          others value    failure
 */
static sns_rc bmg160_configure_power_satate(
    sns_sensor_instance * instance,
    bmg160_pmu_state pst)
{
  uint32_t xfer_bytes;
  uint8_t regv = 0;
  uint8_t regv_bk = 0;
  bmg160_instance_state *state = (bmg160_instance_state*) instance->state->state;
  sns_rc rv  ;

  if (state->in_low_power_mode)
  {
    // XXX ds post: we should delay some time before the next operation
    sns_busy_wait(sns_convert_ns_to_ticks(BMG160_DELAY_US_AFTER_WRITTING_REGISTER * 1000)); //450us
  }

  rv = bmg160_com_read_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMG160_REGA_PMU_LPW0_ADDR,
                               &regv,
                               1,
                               &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rv, SNS_RC_SUCCESS, instance);
  if (rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  regv_bk = regv;

  BMG160_INST_PRINTF(MED, instance, "b> pwm:0x%x, target:%d",
                    regv, pst);

  if (pst == BMG160_PS_NORMAL)
  {
    regv = BST_CLR_VAL_BIT(regv, 5);
    regv = BST_CLR_VAL_BIT(regv, 7);
  }
  else if ((pst == BMG160_PS_DEEP_SUSPEND) && (!(regv & 0x20)))
  {
    regv = BST_SET_VAL_BIT(regv, 5);
  }
  else if ((pst == BMG160_PS_SUSPEND) && (!(regv & 0x80)))
  {
    regv = BST_SET_VAL_BIT(regv, 7);
  }

  if (regv == regv_bk)
  {
    BMG160_INST_PRINTF(LOW, instance, "no need to update<0x%x 0x%x>",
                       regv, regv_bk);
    return SNS_RC_SUCCESS;
  }

  if (bmg160_com_write_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_PMU_LPW0_ADDR,
                                  &regv,
                                  1,
                                  &xfer_bytes,
                                  false) == SNS_RC_SUCCESS)
  {
    // XXX ds post: we should delay some time before the next operation
    sns_busy_wait(sns_convert_ns_to_ticks(BMG160_DELAY_US_AFTER_WRITTING_REGISTER * 1000)); //450us
    BMG160_INST_PRINTF(MED, instance, "w regv:0x%x @ 0x%x",
                      regv, BMG160_REGA_PMU_LPW0_ADDR);
    regv = 0xFF;
    rv = bmg160_com_read_wrapper(state->scp_service,
                                      state->com_port_info.port_handle,
                                      BMG160_REGA_PMU_LPW0_ADDR,
                                      &regv,
                                      1,
                                      &xfer_bytes);
    BMG160_INST_PRINTF(MED, instance, "a> pwm:0x%x", regv);
    BMG160_INST_RV_ASSERT_LOG(rv, SNS_RC_SUCCESS, instance);
    if (rv != SNS_RC_SUCCESS)
    {
      return rv;
    }

    if (pst == BMG160_PS_NORMAL)
    {
      state->in_low_power_mode = 0;
    }
    else
    {
      state->in_low_power_mode = 1;
    }
    return SNS_RC_SUCCESS;
  }
  else
  {
    BMG160_INST_PRINTF(ERROR, instance, "write: 0x%x -> 0x%x failure",
                        regv, BMG160_REGA_PMU_LPW0_ADDR);
    regv = 0;
    rv = bmg160_com_read_wrapper(state->scp_service,
                                      state->com_port_info.port_handle,
                                      BMG160_REGA_PMU_LPW0_ADDR,
                                      &regv,
                                      1,
                                      &xfer_bytes);
    BMG160_INST_PRINTF(MED, instance, "rv:%d, now: 0x%x: 0x%x failure",
                       rv, BMG160_REGA_PMU_LPW0_ADDR, regv);
    return rv;
  }
}

/**
 * configure bmg160 range
 * @param state
 * @return
 */
static sns_rc bmg160_configure_range(bmg160_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  sns_rc rc = SNS_RC_SUCCESS;

  rc = bmg160_com_read_wrapper(state->scp_service,
                                   state->com_port_info.port_handle,
                                   BMG160_REGA_DSP_RANGE_ADDR,
                                   &regv,
                                   1,
                                   &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(SNS_RC_SUCCESS, rc, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv |= 0x80;
  regv = BST_SET_VAL_BITBLOCK(regv, 0, 2, state->gyro_info.range_regv);

  rc = bmg160_com_write_wrapper(state->scp_service,
                                   state->com_port_info.port_handle,
                                   BMG160_REGA_DSP_RANGE_ADDR,
                                   &regv,
                                   1,
                                   &xfer_bytes,
                                   false);
  BMG160_INST_RV_ASSERT_LOG(rc,SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  return SNS_RC_SUCCESS;
}

/**
 * reset the device interface
 * @param scp_service      comport service
 * @param port_handle      the port handle
 * @param sensor           sensor type id
 * @return    SNS_RC_SUCCESS      on success
 *            others value        failure
 */
sns_rc bmg160_reset_device(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            bmg160_sensor_type sensor)
{
  sns_rc rv = SNS_RC_SUCCESS;

  /** HW reset only when both Gyro is requested for
   *  reset. */
  if( sensor == (BMG160_GYRO| BMG160_SENSOR_TEMP))
  {
     rv = bmg160_device_sw_reset(scp_service, port_handle, sensor);
     if (rv != SNS_RC_SUCCESS)
     {
       return rv;
     }
  }

  if(rv == SNS_RC_SUCCESS)
  {
    rv = bmg160_device_set_default_state(scp_service, port_handle, sensor);
  }

  return rv;
}




/**
 * see sns_bmg160_hal.h
 */
sns_rc bmg160_set_gyro_odr_config(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle  *port_handle,
                                bmg160_gyro_odr_regv     curr_odr,
                                bmg160_gyro_resolution   resolution,
                                bmg160_gyro_range        range)
{
  UNUSED_VAR(resolution);
  UNUSED_VAR(range);
  uint8_t regv , regv_bk;
  uint32_t xfer_bytes;
  sns_rc rv;

  if (curr_odr == BMG160_GYRO_ODR_OFF)
  {
    /* no need reconfigure the sensor */
    return SNS_RC_SUCCESS;
  }

  // read firstly
  rv = bmg160_com_read_wrapper(scp_service,
                               port_handle,
                               BMG160_REGA_DSP_BW_ADDR,
                               &regv,
                               1,
                               &xfer_bytes);
  if (rv != SNS_RC_SUCCESS)
  {
    return rv;
  }

  regv_bk = regv;

  regv = 0;
  regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, (uint8_t) curr_odr);
  regv = BST_SET_VAL_BIT(regv, 7);

  // compare
  if ((regv_bk & 0x1F) == (regv & 0x1F))
  {
    return SNS_RC_SUCCESS;
  }

  return bmg160_com_write_wrapper(scp_service,
                                   port_handle,
                                   BMG160_REGA_DSP_BW_ADDR,
                                   &regv,
                                   1,
                                   &xfer_bytes,
                                   false);
}


/**
 * see sns_bmg160_hal.h
 */
void bmg160_set_fifo_config(bmg160_instance_state *state,
                             uint16_t desired_wml,
                             bmg160_gyro_odr_regv regv_chosen_sample_rate,
                             uint8_t sensor_flags)
{
  UNUSED_VAR(sensor_flags);
  state->fifo_info.fifo_rate = regv_chosen_sample_rate;

  if ((regv_chosen_sample_rate != state->gyro_info.curr_odr_regv) ||
      (desired_wml != state->fifo_info.cur_wml))
  {
    state->gyro_info.curr_odr_regv = state->fifo_info.fifo_rate;
    state->fifo_info.cur_wml = desired_wml;
  }
}

/**
 * see sns_bmg160_hal.h
 */
sns_rc bmg160_set_fifo_wml(bmg160_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;

  if(state->fifo_info.fifo_enabled & BMG160_GYRO)
  {
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 6, state->fifo_info.cur_wml);
  }

  return bmg160_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_FIFO_CGF1_ADDR,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);

}


/**
 * see sns_bmg160_hal.h
 */
sns_rc bmg160_start_fifo_streaming(bmg160_instance_state *state)
{
#if BMG160_CONFIG_ENABLE_FIFO
  uint8_t regv = 0x00;
  uint8_t regv_bk = 0x00;
  uint32_t xfer_bytes;
#endif
  sns_rc rc = SNS_RC_SUCCESS;

  /** range */
  rc = bmg160_configure_range(state);
  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS) {
    return rc;
  }

  /** ODR*/
  rc = bmg160_set_gyro_odr_config(state->scp_service,
                           state->com_port_info.port_handle,
                           state->gyro_info.curr_odr_regv,
                           state->gyro_info.resolution,
                           state->gyro_info.range_regv
                           );
  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  /** FIFO */
  state->fifo_info.sampling_intvl = bmg160_get_sample_interval(state->fifo_info.fifo_rate);
#if BMG160_CONFIG_ENABLE_FIFO
  if (state->low_latency_req)
  { /* data ready mode */ }
  else
  {
    rc = bmg160_set_fifo_wml(state);
    BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
    if (rc != SNS_RC_SUCCESS) {
      return rc;
    }
    //start streaming,stream mode

    rc = bmg160_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_FIFO_CGF0_ADDR,
                            &regv,
                            1,
                            &xfer_bytes);
    BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);
    if (rc != SNS_RC_SUCCESS)
    {
      return rc;
    }

    regv_bk = regv;

    regv = 0;
    regv = BST_SET_VAL_BITBLOCK(regv, 6, 7, BMG160_FIFO_MODE_STREAM);
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 0x00);

    if (regv == regv_bk)
    {
      return SNS_RC_SUCCESS;
    }

    rc = bmg160_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_FIFO_CGF0_ADDR,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
    BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
    if (rc != SNS_RC_SUCCESS)
    {
      return rc;
    }
  }
#endif
  return rc;
}

#if BMG160_CONFIG_ENABLE_FIFO
static sns_rc bmg160_hw_cfg_stop_fifo_streaming(bmg160_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  sns_rc rc = SNS_RC_SUCCESS;

  /** stop fifo INT */
  rc = bmg160_disable_fifo_intr(state);
  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);
  /** FIFO */
  state->fifo_info.sampling_intvl = BMG160_U64_MAX;
  /*fifo disable*/
  /* fifo stream mode */
  regv = BST_SET_VAL_BITBLOCK(regv, 6, 7, BMG160_FIFO_MODE_BYPASS);
  regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, 0x00);

  if(state->fifo_info.fifo_enabled)
  {
    rc = bmg160_com_write_wrapper(state->scp_service,
                              state->com_port_info.port_handle,
                              BMG160_REGA_FIFO_CGF0_ADDR,
                              &regv,
                              1,
                              &xfer_bytes,
                              false);
    BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
    if (rc != SNS_RC_SUCCESS)
    {
      return rc;
    }
  }
  return bmg160_set_fifo_wml(state);
}
#endif

static sns_rc bmg160_hw_cfg_stop_drdy_streaming(bmg160_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  sns_rc rc;

  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_ENABLE0,
                            &regv,
                            1,
                            &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_CLR_VAL_BIT(regv, 7);
  rc = bmg160_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_ENABLE0,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  /** INT map */
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_MAP_1,
                            &regv,
                            1,
                            &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_CLR_VAL_BIT(regv, 0);/* clear int_map1 */
  regv = BST_CLR_VAL_BIT(regv, 7);/* clear int_map2 */
  rc = bmg160_com_write_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_MAP_1,
                            &regv,
                            1,
                            &xfer_bytes,
                            false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }
  

  /** INT pin configuration */
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                          state->com_port_info.port_handle,
                          BMG160_REGA_INT_ENABLE1,
                          &regv,
                          1,
                          &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, 0x0a);
  rc = bmg160_com_write_wrapper(state->scp_service,
                          state->com_port_info.port_handle,
                          BMG160_REGA_INT_ENABLE1,
                          &regv,
                          1,
                          &xfer_bytes,
                          false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  return SNS_RC_SUCCESS;
}


/**
 * see sns_bmg160_hal.h
 */
void bmg160_stop_data_streaming(bmg160_instance_state *state)
{
  sns_rc rc = SNS_RC_SUCCESS;
#if BMG160_CONFIG_ENABLE_FIFO
  if (state->low_latency_req)
  {
    rc = bmg160_hw_cfg_stop_drdy_streaming(state);
  }
  else
  {
    rc = bmg160_hw_cfg_stop_fifo_streaming(state);
  }
#else
  bmg160_hw_cfg_stop_drdy_streaming(state);
#endif

  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);

  /** ODR */
  rc = bmg160_set_gyro_odr_config(
                           state->scp_service,
                           state->com_port_info.port_handle,
                           BMG160_GYRO_ODR_OFF,
                           state->gyro_info.resolution,
                           state->gyro_info.range_regv);
  BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS) {
    return ;
  }
}

#if BMG160_CONFIG_ENABLE_FIFO
static sns_rc bmg160_hal_hw_cfg_enable_fifo_init(bmg160_instance_state *state,
                                                 uint8_t sensor_enable_flags)
{
  UNUSED_VAR(sensor_enable_flags);
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  uint8_t map_2_int1 = 1;
  sns_rc rc;

  if(state->fifo_info.fifo_enabled)
  {
    if(state->fifo_info.cur_wml > 0)
    {
      if(state->fifo_info.fifo_enabled & BMG160_GYRO)
      {
        /** INT */
        rc = bmg160_com_read_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_ENABLE0,
                                  &regv,
                                  1,
                                  &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        regv = BST_SET_VAL_BIT(regv, 6);
        rc = bmg160_com_write_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_ENABLE0,
                                  &regv,
                                  1,
                                  &xfer_bytes,
                                  false);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }
        /** FIFO enable*/
        regv = 0;
        rc = bmg160_com_read_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_4_ADDR,
                                  &regv,
                                  1,
                                  &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        regv = BST_SET_VAL_BIT(regv, 7);
        rc = bmg160_com_write_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_4_ADDR,
                                  &regv,
                                  1,
                                  &xfer_bytes,
                                  false);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        /** INT map */
        regv = 0;
        rc = bmg160_com_read_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_MAP_1,
                                  &regv,
                                  1,
                                  &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        if (map_2_int1)
        {
            regv = BST_SET_VAL_BIT(regv, 2);
        }
        else
        {
            regv = BST_SET_VAL_BIT(regv, 5);
        }
        rc = bmg160_com_write_wrapper(state->scp_service,
                                  state->com_port_info.port_handle,
                                  BMG160_REGA_INT_MAP_1,
                                  &regv,
                                  1,
                                  &xfer_bytes,
                                  false);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        /** INT pin configuration */
        regv = 0;
        rc = bmg160_com_read_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_ENABLE1,
                                &regv,
                                1,
                                &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }

        regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, 0x05);
        rc = bmg160_com_write_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_ENABLE1,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
        if (rc != SNS_RC_SUCCESS)
        {
          return rc;
        }
      }
    }
    else
    {
      // TODO add error code - trying to enable FIFO interrupt when wmk=0
    }
  }
  return SNS_RC_SUCCESS;
}
#endif // BMG160_CONFIG_ENABLE_FIFO

static sns_rc bmg160_hal_hw_cfg_enable_drdy_init(bmg160_instance_state *state,  uint8_t sensor_enable_flags)
{
  UNUSED_VAR(sensor_enable_flags);
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  sns_rc rc;

  /** INT */
   regv = 0;
   rc = bmg160_com_read_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             BMG160_REGA_INT_ENABLE0,
                             &regv,
                             1,
                             &xfer_bytes);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }

   regv = BST_SET_VAL_BIT(regv, 7);
   rc = bmg160_com_write_wrapper(state->scp_service,
                                 state->com_port_info.port_handle,
                                 BMG160_REGA_INT_ENABLE0,
                                 &regv,
                                 1,
                                 &xfer_bytes,
                                 false);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }

   /** INT map */
   regv = 0;
   rc = bmg160_com_read_wrapper(state->scp_service,
                             state->com_port_info.port_handle,
                             BMG160_REGA_INT_MAP_1,
                             &regv,
                             1,
                             &xfer_bytes);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }
#ifdef BMG160_VR
   regv = BST_SET_VAL_BIT(regv, 0);/* map int1 for acc sync */
#endif
   regv = BST_SET_VAL_BIT(regv, 7);/* map int2 */
   rc = bmg160_com_write_wrapper(state->scp_service,
                                 state->com_port_info.port_handle,
                                 BMG160_REGA_INT_MAP_1,
                                 &regv,
                                 1,
                                 &xfer_bytes,
                                 false);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }

   /** INT pin configuration */
   regv = 0;
   rc = bmg160_com_read_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMG160_REGA_INT_ENABLE1,
                           &regv,
                           1,
                           &xfer_bytes);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }

   regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, 0x05);
   rc = bmg160_com_write_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMG160_REGA_INT_ENABLE1,
                           &regv,
                           1,
                           &xfer_bytes,
                           false);
   BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
   if (rc != SNS_RC_SUCCESS)
   {
     return rc;
   }
   return SNS_RC_SUCCESS;
}

/**
 * see sns_bmg160_hal.h
 */
void bmg160_enable_data_intr(bmg160_instance_state *istate,  uint8_t sensor_enable_flags)
{
#if BMG160_CONFIG_ENABLE_FIFO
  if (istate->low_latency_req)
  {
    sns_rc rc = SNS_RC_SUCCESS;
    // switch to data ready mode
    rc = bmg160_disable_fifo_intr(istate);
    rc |= bmg160_hal_hw_cfg_enable_drdy_init(istate, sensor_enable_flags);
    BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, istate->owner);
  }
  else
  {
    sns_rc rc = SNS_RC_SUCCESS;
    rc = bmg160_hal_hw_cfg_enable_fifo_init(istate, sensor_enable_flags);
    BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, istate->owner);
  }
#else
  bmg160_hal_hw_cfg_enable_drdy_init(istate, sensor_enable_flags);
#endif
}

/**
 * see sns_bmg160_hal.h
 */
sns_rc bmg160_disable_fifo_intr(bmg160_instance_state *state)
{
  uint8_t regv = 0;
  uint32_t xfer_bytes;
  sns_rc rc;

  /** fifo enable*/
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                               state->com_port_info.port_handle,
                               BMG160_REGA_INT_4_ADDR,
                               &regv,
                               1,
                               &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_CLR_VAL_BIT(regv, 7);
  rc = bmg160_com_write_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_4_ADDR,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  /**INT enable*/
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_ENABLE0,
                            &regv,
                            1,
                            &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_CLR_VAL_BIT(regv, 6);
  rc = bmg160_com_write_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_ENABLE0,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  /** fifo enable*/
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                            state->com_port_info.port_handle,
                            BMG160_REGA_INT_4_ADDR,
                            &regv,
                            1,
                            &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_CLR_VAL_BIT(regv, 7);
  rc = bmg160_com_write_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_4_ADDR,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }
  /** MAP*/
  regv = 0;
  rc = bmg160_com_read_wrapper(state->scp_service,
                          state->com_port_info.port_handle,
                          BMG160_REGA_INT_ENABLE1,
                          &regv,
                          1,
                          &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  regv = BST_SET_VAL_BITBLOCK(regv, 0, 3, 0x06);
  rc = bmg160_com_write_wrapper(state->scp_service,
                                state->com_port_info.port_handle,
                                BMG160_REGA_INT_ENABLE1,
                                &regv,
                                1,
                                &xfer_bytes,
                                false);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  return SNS_RC_SUCCESS;
}

#if BMG160_CONFIG_ENABLE_FIFO
/**
 * Reads four status registers starting from
 * STM_BMG160_REG_FIFO_STATUS1.
 *
 * @param[i] state              Instance state
 * @param[o] buffer             status registers
 *
 * @return none
 */
static sns_rc bmg160_read_fifo_status(bmg160_instance_state *state,
                                     uint8_t *buffer)
{
  uint32_t xfer_bytes;
  sns_rc rc;

  rc = bmg160_com_read_wrapper(state->scp_service,
                           state->com_port_info.port_handle,
                           BMG160_REGA_FIFO_STATUS,
                           buffer,
                           1,
                           &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, state->owner);
  if (rc != SNS_RC_SUCCESS)
  {
    return rc;
  }

  return SNS_RC_SUCCESS;
}
#endif

/**
 * Updates temp sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
static void bmg160_set_polling_config(sns_sensor_instance *const this)
{
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;

  if(state->sensor_temp_info.sampling_intvl > 0)
  {
    if(!state->sensor_temp_info.timer_is_active)
    {
      bmg160_start_sensor_temp_polling_timer(this);
    }
  }
  else if(state->sensor_temp_info.timer_is_active)
  {
    state->sensor_temp_info.timer_is_active = false;
    sns_sensor_util_remove_sensor_instance_stream(this, &state->timer_data_stream);
  }
}

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time bmg160_get_sample_interval(bmg160_gyro_odr_regv curr_odr)
{
  sns_time  sample_interval = 0;
  float odr = bmg160_get_gyro_odr(curr_odr);

  if(odr > 0.0)
  {
    sample_interval = sns_convert_ns_to_ticks(1000000000 / odr);
  }

  return sample_interval;
}

#if BMG160_CONFIG_ENABLE_FIFO

static void bmg160_hal_ts_fix_post(bmg160_instance_state *istate)
{
  if (istate->fifo_info.ts_fix_flag) {
    istate->fifo_info.ff_itvl_gap_need_fix_too_low  = 0;
    istate->fifo_info.ff_itvl_gap_need_fix_too_high = 0;

    if (istate->fifo_info.ts_fix_flag == BMG160_TS_FIX_OVERLAP) { //too high
      istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_est * 99 / 100;
    } else if (istate->fifo_info.ts_fix_flag == BMG160_TS_FIX_SYNC) { //too low
      istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_est * 101 / 100;
    } else if (istate->fifo_info.ts_fix_flag == BMG160_TS_FIX_LIGHT_OVERLAP) { //little high
      istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_est * 995 / 1000;
    } else if (istate->fifo_info.ts_fix_flag == BMG160_TS_FIX_LIGHT_SYNC) { //little low
      istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_est * 1005 / 1000;
    }
  }

  // boundary checking
  if (istate->fifo_info.ts_fff_intvl_est * 100 < istate->fifo_info.ts_fff_intvl_est * 98) {
    istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_ideal * 98 / 100;
  } else if (istate->fifo_info.ts_fff_intvl_est * 100 > istate->fifo_info.ts_fff_intvl_ideal * 102) {
    istate->fifo_info.ts_fff_intvl_est = istate->fifo_info.ts_fff_intvl_ideal * 102 / 100;
  }

  istate->fifo_info.ts_fix_ignore = false;
}

static void bmg160_hal_ts_drift_verification(sns_sensor_instance * const this,
                                             sns_time ts_last_end,
                                             sns_time est_start, sns_time est_end,
                                             sns_time ts_curr,  uint16_t ff_count, bool in_irq)
{
  bmg160_instance_state *state = (bmg160_instance_state *) this->state->state;
  uint32_t  devi;
  devi = (ts_curr > est_end) ? (ts_curr - est_end) : (est_end - ts_curr);
  bmg160_ts_fx_flag fix_flag = BMG160_TS_FIX_NONE;
  uint32_t ts_fix_take_care_threshold = 0;
  bool ts_fix_ignore = false;

  BMG160_INST_PRINTF(MED,this,
          "ts.last:0x%x%08x, start:0x%x%08x, end:0x%x%08x, ts_curr:0x%x%08x, diff: %d ff_count: %d, too.high.cnt:%d, too.low.cnt:%d",
          (uint32_t) (ts_last_end >> 32), (uint32_t) ts_last_end,
          (uint32_t) (est_start >> 32), (uint32_t) est_start,
          (uint32_t) (est_end >> 32), (uint32_t) est_end,
          (uint32_t) (ts_curr >> 32), (uint32_t) ts_curr,
          (uint32_t)devi, ff_count,
          state->fifo_info.ff_itvl_gap_need_fix_too_high,
          state->fifo_info.ff_itvl_gap_need_fix_too_low);

  if (est_start < ts_last_end) {
    //overlap
    BMG160_INST_PRINTF(ERROR, this, "ERROR!!! ts overlap <0x%x%08x : 0x%x%08x>",
                    (uint32_t) (ts_last_end >> 32), (uint32_t) (ts_last_end),
                    (uint32_t) (est_start >> 32), (uint32_t) (est_start));
    state->fifo_info.ff_itvl_gap_need_fix_too_high = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_OVERLAP;
    state->fifo_info.ts_fix_flag = BMG160_TS_FIX_OVERLAP;
    return ;
  }

  ts_fix_take_care_threshold = (state->fifo_info.ts_fff_intvl_ideal / 3);

  if (in_irq) {
    if (devi >= (ts_fix_take_care_threshold)) {
      if (ts_curr >= est_end) {
        // happy case but...
        if (devi >= (BMG160_FIFO_TS_SYNC_ERROR_THRESHOLD * state->fifo_info.ts_fff_intvl_ideal)) {
          fix_flag = BMG160_TS_FIX_SYNC;
        } else if (devi >= state->fifo_info.ts_fff_intvl_ideal) {
          fix_flag = BMG160_TS_FIX_LIGHT_SYNC;
        } else {
          if (state->fifo_info.ff_itvl_gap_need_fix_too_low < BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD) {
            state->fifo_info.ff_itvl_gap_need_fix_too_low  ++;
          }
        }

        if (fix_flag == BMG160_TS_FIX_NONE) {
          if (state->fifo_info.ff_itvl_gap_need_fix_too_low >= BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD) {
            state->fifo_info.ff_itvl_gap_need_fix_too_low = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD;
            fix_flag = BMG160_TS_FIX_LIGHT_SYNC;
          }
        }

        // reset the high counter
        if (state->fifo_info.ff_itvl_gap_need_fix_too_high) {
          state->fifo_info.ff_itvl_gap_need_fix_too_high  --;
        }

        // too mush on left, reset the high counter
        if (fix_flag) {
          state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;
        }
      } else { // dangerous ... current ts < ts.end
        if (devi > (BMG160_FIFO_TS_OVERLAP_ERROR_THRESHOLD * state->fifo_info.ts_fff_intvl_ideal)) {
          if (ff_count <= state->fifo_info.cur_wml) {
            fix_flag = BMG160_TS_FIX_OVERLAP;
            BMG160_INST_PRINTF(ERROR, this, "ERROR!!! overlap during irq mode:- %d", devi);
          } else {
            state->fifo_info.ff_itvl_gap_need_fix_too_high += 2;
            ts_fix_ignore = true;
          }
        } else if (devi > state->fifo_info.ts_fff_intvl_ideal){
          if (ff_count <= state->fifo_info.cur_wml) {
            state->fifo_info.ff_itvl_gap_need_fix_too_high += 2;
            ts_fix_ignore = true;
          }
        } else {
          state->fifo_info.ff_itvl_gap_need_fix_too_high ++;
        }

        if (fix_flag == BMG160_TS_FIX_NONE) {
          if (state->fifo_info.ff_itvl_gap_need_fix_too_high >= BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD) {
            state->fifo_info.ff_itvl_gap_need_fix_too_high = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD;
            fix_flag = BMG160_TS_FIX_LIGHT_OVERLAP;
          }
        }

        //reset low
        if (state->fifo_info.ff_itvl_gap_need_fix_too_low) {
          state->fifo_info.ff_itvl_gap_need_fix_too_low --;
        }
        if (fix_flag) {
          state->fifo_info.ff_itvl_gap_need_fix_too_low = 0;
        }
      }
    } else {
      if (state->fifo_info.ff_itvl_gap_need_fix_too_low) {
        state->fifo_info.ff_itvl_gap_need_fix_too_low--;
      }

      if (state->fifo_info.ff_itvl_gap_need_fix_too_high) {
        state->fifo_info.ff_itvl_gap_need_fix_too_high--;
      }
    }
  }
  else {   //NON-IRQ mode
    if (ts_curr < est_end) {
      // this case is very dangerous
      state->fifo_info.ff_itvl_gap_need_fix_too_low = 0;
      state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;
      fix_flag = BMG160_TS_FIX_OVERLAP;
      if (state->fifo_info.ff_itvl_gap_need_fix_too_high >= BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD) {
        state->fifo_info.ff_itvl_gap_need_fix_too_high = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD;
      } else {
        state->fifo_info.ff_itvl_gap_need_fix_too_high = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD;
      }

      BMG160_INST_PRINTF(ERROR, this, "ERROR!!! over lap happened in non-irq mode");
    } else {
      uint16_t ff_count_max = 2; //SNS_MAX(2, ff_count);

      // happy case but...
      if (devi >= (ff_count_max * state->fifo_info.ts_fff_intvl_ideal))
      {
        // rebase
        fix_flag = BMG160_TS_FIX_REBASE;
        BMG160_INST_PRINTF(ERROR, this, "ts sync error in non-irq context,rebase");

      } else if (devi >= state->fifo_info.ts_fff_intvl_ideal) {
        fix_flag = BMG160_TS_FIX_SYNC;
      } else if (devi > (state->fifo_info.ts_fff_intvl_ideal / 2)) {
        fix_flag = BMG160_TS_FIX_LIGHT_SYNC;
      } else if (devi > (state->fifo_info.ts_fff_intvl_ideal / 4)){
        state->fifo_info.ff_itvl_gap_need_fix_too_low ++;
      }

      if (fix_flag == BMG160_TS_FIX_NONE) {
        if (state->fifo_info.ff_itvl_gap_need_fix_too_low >= BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD) {
          state->fifo_info.ff_itvl_gap_need_fix_too_low = BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD;
          fix_flag = BMG160_TS_FIX_LIGHT_SYNC;
        }
      }

      // fix high counter
      if (state->fifo_info.ff_itvl_gap_need_fix_too_high) {
        state->fifo_info.ff_itvl_gap_need_fix_too_high --;
      }

      if (fix_flag) {
        state->fifo_info.ff_itvl_gap_need_fix_too_high  = 0;
      }

    }
  }

  if (((est_end + state->fifo_info.ts_fff_intvl_ideal) < ts_curr) ||
      (fix_flag == BMG160_TS_FIX_REBASE)) {   //SYNC error checking
    sns_time ts_batching = ff_count;
    ts_batching = ts_last_end + ts_batching * state->fifo_info.ts_fff_intvl_ideal;
    if (ts_batching < ts_curr) {
      fix_flag = BMG160_TS_FIX_REBASE;
      state->fifo_info.ff_itvl_gap_need_fix_too_high = 0;
      state->fifo_info.ff_itvl_gap_need_fix_too_low= 0;
    } else {
      if (fix_flag == BMG160_TS_FIX_REBASE) {
        fix_flag = BMG160_TS_FIX_SYNC;
      }
    }
  }

  state->fifo_info.ts_fix_flag = fix_flag;
  state->fifo_info.ts_fix_ignore = ts_fix_ignore;
}


//CHECK, we assume that the FIFO interrupt is regularly
bool bmg160_hal_update_sample_ts(
   sns_sensor_instance * const this,
   uint16_t        ff_count,
   bool            in_irq,
   bool            ff_overrun,
   sns_time ts_curr)
{
  bmg160_instance_state *state = (bmg160_instance_state *) this->state->state;
  sns_time  est_start;
  sns_time  est_end;

  uint32_t  devi;
  sns_time  itvl_irq, frame_itvl_this_batch = 0;
  uint32_t ts_itvl_fix = state->fifo_info.ts_fff_intvl_est;

  if (ff_overrun)
  {
    BMG160_INST_PRINTF(ERROR, this, "NOTICE!!! OVERFLOW");
  }

  BMG160_INST_PRINTF(LOW, this, "[ts] irq: %u, ff count:%d, %u,ff wml:%d",
                     (uint32_t) state->fifo_info.isr_ts,
                     ff_count,
                     (uint32_t)(state->fifo_info.ff_accum_cnt + ff_count),
                     state->fifo_info.cur_wml);

  if (state->fifo_info.avail_accurate_ts_last_sample)
  {
    if (in_irq)
    {
      // Deviation
      itvl_irq = bmg160_get_time_elaspse_sys(state->fifo_info.isr_last_ts,
                                           state->fifo_info.isr_ts);
      frame_itvl_this_batch = (itvl_irq / ff_count);

      devi = bmg160_get_time_deviation(frame_itvl_this_batch, state->fifo_info.ts_fff_intvl_est);

      BMG160_INST_PRINTF(LOW, this, "%u %u %u %u",
                         (uint32_t)state->fifo_info.end_timestamp,
                         (uint32_t)state->fifo_info.isr_last_ts,
                         (uint32_t)state->fifo_info.isr_ts,
                         (uint32_t)itvl_irq);

      BMG160_INST_PRINTF(LOW, this, "batch intvl 2 INT:%u, avg itvl:%u est intvl:%u, diff:%u",
              (uint32_t)(itvl_irq),
              (uint32_t) frame_itvl_this_batch,
              (uint32_t)state->fifo_info.ts_fff_intvl_est,
              (int32_t)devi);

      if (state->fifo_info.ff_isr_cnt < BMG160_FIFO_TS_ISR_CNT_PREPRECISE)
      {
        // fast calculate
        ts_itvl_fix = (uint32_t) ((((uint32_t)(ts_itvl_fix * (100 - BMG160_FIFO_TS_ISR_PRECISE_MOST_WEIGHT))) +
                          (uint32_t)(frame_itvl_this_batch * (BMG160_FIFO_TS_ISR_PRECISE_MOST_WEIGHT))) / 100);
      }
      else
      {
        //
        ts_itvl_fix = (uint32_t)(((uint32_t)(ts_itvl_fix * (BMG160_FIFO_TS_ISR_PRECISE_MOST_WEIGHT)) +
                (uint32_t)(frame_itvl_this_batch * (100 - BMG160_FIFO_TS_ISR_PRECISE_MOST_WEIGHT))) / 100);
      }
    }

    // Alignment
    if (bmg160_itvl_within_hifi_accuracy(state->fifo_info.ts_fff_intvl_ideal, ts_itvl_fix))
    {
      state->fifo_info.ts_fff_intvl_est = ts_itvl_fix;
    }
    else
    {
      ts_itvl_fix = state->fifo_info.ts_fff_intvl_est;
    }

    BMG160_INST_PRINTF(LOW, this, "ts.est:%u, ts.algo:%u", (uint32_t )ts_itvl_fix,
                (uint32_t ) state->fifo_info.ts_fff_intvl_est);

    // ts start from the latest ts
    uint32_t temp_ff_count = (uint32_t) ff_count;
    est_end   = (sns_time)(state->fifo_info.end_timestamp + ((sns_time)temp_ff_count * (sns_time)ts_itvl_fix));
    est_start = state->fifo_info.end_timestamp + (sns_time)ts_itvl_fix;

    bmg160_hal_ts_drift_verification(this, state->fifo_info.end_timestamp, est_start, est_end, ts_curr,
                                     ff_count, in_irq);
  }
  else
  {
    // the first frame
    // we assume that the ts end is just the ist fired or
    // reseted due to flush
    if (!bmg160_itvl_within_hifi_accuracy(state->fifo_info.ts_fff_intvl_ideal,
                                       state->fifo_info.ts_fff_intvl_est))
    {
      state->fifo_info.ts_fff_intvl_est = state->fifo_info.ts_fff_intvl_ideal;
    }
    else
    {
      if (state->fifo_info.ts_fff_intvl_est == state->fifo_info.ts_fff_intvl_ideal)
      {
        state->fifo_info.ts_fff_intvl_est = (state->fifo_info.ts_fff_intvl_ideal * 98) / 100;
      }
    }

    // always use the interrupt ts as the first frame end ts
    est_end   = state->fifo_info.isr_ts;
    est_start = est_end - (state->fifo_info.ts_fff_intvl_est * (ff_count - 1));

  }

  if (state->fifo_info.ts_fix_flag == BMG160_TS_FIX_OVERLAP ||
        state->fifo_info.ts_fix_flag == BMG160_TS_FIX_SYNC) {
      itvl_irq = bmg160_get_time_elaspse_sys (state->fifo_info.end_timestamp, state->fifo_info.isr_ts);
      frame_itvl_this_batch = (itvl_irq / ff_count);
      ts_itvl_fix = frame_itvl_this_batch;

      //est_end = state->fifo_info.isr_ts;
      //est_start = est_end - (ts_itvl_fix * (ff_count - 1));
      est_start = state->fifo_info.end_timestamp + ts_itvl_fix;
      est_end   = state->fifo_info.end_timestamp + ts_itvl_fix * ff_count;
  } else if (state->fifo_info.ts_fix_flag == BMG160_TS_FIX_REBASE) {
      if (!bmg160_itvl_within_hifi_accuracy(state->fifo_info.ts_fff_intvl_ideal, state->fifo_info.ts_fff_intvl_est)) {
        state->fifo_info.ts_fff_intvl_est = state->fifo_info.ts_fff_intvl_ideal;
      } else {
        if (state->fifo_info.ts_fff_intvl_ideal == state->fifo_info.ts_fff_intvl_est) {
          state->fifo_info.ts_fff_intvl_est = ((state->fifo_info.ts_fff_intvl_ideal * 98) / 100);
        }
      }

      ts_itvl_fix = state->fifo_info.ts_fff_intvl_est;
      // always use the interrupt ts as the first frame end ts
      est_end   = state->fifo_info.isr_ts;
      est_start = est_end - (ts_itvl_fix * (ff_count - 1));
  } else {
      if(state->fifo_info.ts_fix_flag == BMG160_TS_FIX_LIGHT_SYNC) {
        ts_itvl_fix = (state->fifo_info.ts_fff_intvl_ideal * 102) / 100;
      } else if (state->fifo_info.ts_fix_flag == BMG160_TS_FIX_LIGHT_OVERLAP) {
        ts_itvl_fix = (state->fifo_info.ts_fff_intvl_ideal * 98) / 100;
      }

      //update
      if (state->fifo_info.avail_accurate_ts_last_sample) {
        est_start = state->fifo_info.end_timestamp + ts_itvl_fix;
        est_end   = state->fifo_info.end_timestamp + ts_itvl_fix * ff_count;
      }
  }

  // overlap checking
  if ((est_start <= state->fifo_info.end_timestamp) &&
      ((state->fifo_info.ts_fix_flag != BMG160_TS_FIX_OVERLAP)) &&
      (state->fifo_info.ts_fix_ignore == false)) {
    //fix it
    itvl_irq = bmg160_get_time_elaspse_sys(state->fifo_info.end_timestamp, state->fifo_info.isr_ts);
    frame_itvl_this_batch = (itvl_irq / ff_count);
    ts_itvl_fix = frame_itvl_this_batch;

    //est_end = state->fifo_info.isr_ts;
    //est_start = est_end - (ts_itvl_fix * (ff_count - 1));

    est_start = state->fifo_info.end_timestamp + ts_itvl_fix;
    est_end   = state->fifo_info.end_timestamp + ts_itvl_fix * ff_count;

    //fast fix fag
    state->fifo_info.ts_fix_flag = BMG160_TS_FIX_OVERLAP;
    BMG160_INST_PRINTF(HIGH, this, "fix overlap:%u %u", (uint32_t)state->fifo_info.ts_fff_intvl_est, ts_itvl_fix);
  } else {
    int32_t ts_dlta = state->fifo_info.isr_ts - est_end;
    if(ts_dlta > 2 * state->fifo_info.ts_fff_intvl_ideal) {
      //need a fast fix to fix the sync issue
      BMG160_INST_PRINTF(MED, this, "WARNING!!! didn't get a sync issue, %d", state->fifo_info.ts_fix_flag);
    }
  }

  if (state->fifo_info.ts_fix_flag) {
    state->fifo_info.ts_itvl_fast_fixed = ts_itvl_fix;
  }

  state->fifo_info.timestamp = est_start;
  state->fifo_info.end_timestamp = est_end;

  state->fifo_info.isr_last_ts = state->fifo_info.isr_ts;

  if (state->fifo_info.avail_accurate_ts_last_sample == 0) {
    state->fifo_info.avail_accurate_ts_last_sample = 1;
  }

  // postage
  bmg160_hal_ts_fix_post(state);

  BMG160_INST_PRINTF(LOW, this, "isr.cnt:%u, update intvl ts est:%u, use:%u, fast.fix:%u, fix_flag:%d",
               (uint32_t )state->fifo_info.ff_isr_cnt,
               (uint32_t )ts_itvl_fix,
               (uint32_t ) state->fifo_info.ts_fff_intvl_est,
               (uint32_t ) state->fifo_info.ts_itvl_fast_fixed,
               state->fifo_info.ts_fix_flag);

  BMG160_INST_PRINTF(LOW, this, "dl_meta <%d %u %u %u %u %u> %d",
             ff_count, (uint32_t)est_start, (uint32_t)est_end,
             (uint32_t)ts_curr,
             (uint32_t) state->fifo_info.isr_ts,
             (uint32_t)state->fifo_info.ts_fff_intvl_est,
             (state->fifo_info.ff_accum_cnt + ff_count));
  return true;
}
#endif


/**
 * Extract a gyro sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] fifo_sample        The segment of fifo buffer that has the gyro sample
 * @param[i] timestamp          Timestamp to be used for this sample
 * @param[i] instance           The current bmg160 sensor instance
 * @param[i] event_service      Event service for sending out the
 * @param[i] state              The state of the bmg160 sensor instance
 */
static void bmg160_handle_gyro_sample(const uint8_t fifo_sample[6],
                                sns_time timestamp,
                                sns_sensor_instance *const instance,
                                sns_event_service *event_service,
                                log_sensor_state_raw_info *log_gyro_state_raw_info)
{
  bmg160_instance_state *state = (bmg160_instance_state *) instance->state->state;
  UNUSED_VAR(event_service);
  int16_t compose_data[3];
  float ipdata[TRIAXIS_NUM] = {0}, opdata_raw[TRIAXIS_NUM] = {0};
  uint8_t i = 0;
  triaxis_conversion          *axis_map;

  compose_data[0] = (fifo_sample[0] | (fifo_sample[1] << 8));
  compose_data[1] = (fifo_sample[2] | (fifo_sample[3] << 8));
  compose_data[2] = (fifo_sample[4] | (fifo_sample[5] << 8));

  ipdata[0] = compose_data[0] * state->gyro_info.resolution / 1000.0;
  ipdata[1] = compose_data[1] * state->gyro_info.resolution / 1000.0;
  ipdata[2] = compose_data[2] * state->gyro_info.resolution / 1000.0;


#if BMG160_ENABLE_LOG_LEVEL_2
  // res data
  BMG160_INST_PRINTF(LOW, instance, "phy_data:(1000)%d (1000)%d (1000)%d",
                    (int32_t)(ipdata[0] * 1000),
                    (int32_t)(ipdata[1] * 1000),
                    (int32_t)(ipdata[2] * 1000));
#endif
  // axis conversion
  axis_map = state->gyro_info.sstate->common.axis_map;
  for(i = 0; i < TRIAXIS_NUM; i ++)
  {
    opdata_raw[axis_map[i].opaxis] = (axis_map[i].invert ? -1.0 : 1.0) *
      ipdata[axis_map[i].ipaxis];
  }

#if BMG160_ENABLE_LOG_LEVEL_2
  // res data
  BMG160_INST_PRINTF(LOW, instance, "map_phy data:(1000)%d (1000)%d (1000)%d",
                    (int32_t)(opdata_raw[0] * 1000),
                    (int32_t)(opdata_raw[1] * 1000),
                    (int32_t)(opdata_raw[2] * 1000));
#endif

#if BMG160_CONFIG_ENABLE_SELF_TEST_FAC
  if (state->fac_test_in_progress && BMG160_GYRO
      == state->fac_test_info.fac_test_sensor)
  {
    state->fac_test_info.num_samples ++;

    for (i = 0; i < TRIAXIS_NUM; i++)
    {
      state->fac_test_info.sample_sum[i] += (opdata_raw[i]);
      state->fac_test_info.sample_square_sum[i] += ((int64_t) (opdata_raw[i])
          * (int64_t) (opdata_raw[i]));
    }
  }
#endif

  // factory calibration
  vector3 opdata_cal = sns_apply_calibration_correction_3(
      make_vector3_from_array(opdata_raw),
      make_vector3_from_array(state->gyro_registry_cfg.fac_cal_bias),
      state->gyro_registry_cfg.fac_cal_corr_mat);

#if BMG160_ENABLE_LOG_LEVEL_2
  // res data
  BMG160_INST_PRINTF(LOW, instance, "report_phy data:(1000)%d (1000)%d (1000)%d",
                    (int32_t)(opdata_cal.x * 1000),
                    (int32_t)(opdata_cal.y * 1000),
                    (int32_t)(opdata_cal.z * 1000));
#endif

  if((state->fifo_info.publish_sensors & BMG160_GYRO))
  {
    sns_std_sensor_sample_status status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

    if (state->sample_cnt_to_skip_odrc)
    {
      state->sample_cnt_to_skip_odrc --;
      status = SNS_STD_SENSOR_SAMPLE_STATUS_UNRELIABLE;
    }
    pb_send_sensor_stream_event(instance,
                                &state->gyro_info.sstate->my_suid,
                                timestamp,
                                SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT,
                                status,
                                opdata_cal.data,
                                ARR_SIZE(opdata_cal.data),
                                state->encoded_imu_event_len);

#if BMG160_CONFIG_ENABLE_DIAG_LOG
    // Log raw uncalibrated sensor data
    bmg160_log_sensor_state_raw_add(
      log_gyro_state_raw_info,
      opdata_raw,
      timestamp,
      status);
#else
    UNUSED_VAR(log_gyro_state_raw_info);
#endif

#if BMG160_ENABLE_LOG_LEVEL_2
  BMG160_INST_PRINTF(LOW, instance, "package send st:%d %d",
                     state->sample_cnt_to_skip_odrc, status);
#endif
  }

}

/**
 * Gets current Gyro ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float bmg160_get_gyro_odr(bmg160_gyro_odr_regv curr_odr)
{
  float odr = 0.0;
  uint8_t idx;

  for(idx = 0; idx < ARR_SIZE(bmg160_config_map); idx++)
  {
    if(curr_odr == bmg160_config_map[idx].regv
       &&
       curr_odr != BMG160_GYRO_ODR_OFF)
    {
      odr = bmg160_config_map[idx].f_odr;
      break;
    }
  }

  return odr;
}


sns_rc bmg160_handle_regv_cfg_timeout_event(bmg160_instance_state *state)
{
  sns_rc rv = SNS_RC_SUCCESS;
  UNUSED_VAR(state);
#if 0
  uint32_t xfer_bytes;

  if (state->regv_cfg_ctrl.cfg_state == BMG160_REGV_CONFIG_INPROGRESS)
  {
    rv = bmg160_com_write_wrapper(state->scp_service,
                                    state->com_port_info.port_handle,
                                    BMG160_REGA_FIFO_MODE_REG,
                                    &state->regv_cfg_ctrl.saved_regv,
                                    1,
                                    &xfer_bytes,
                                    false);
    BST_RV_ASSERT_RT(rv, SNS_RC_SUCCESS);
    state->regv_cfg_ctrl.cfg_state = BMG160_REGV_CONFIG_IDLE;
  }

  state->gyro_info.lp_mode = false;
#endif
  return rv;
}

void bmg160_process_fifo_data_buffer(
  sns_sensor_instance *instance,
  sns_time            ts_ff_start,
  sns_time            ts_ff_itvl,
  const uint8_t       *fifo_start,
  size_t              num_bytes)
{
  uint16_t num_sample_sets = 0;
  uint32_t i;
  bmg160_instance_state *state = (bmg160_instance_state *)instance->state->state;
  sns_service_manager *service_manager = instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
     (sns_event_service*)service_manager->get_service(service_manager, SNS_EVENT_SERVICE);
  log_sensor_state_raw_info log_gyro_state_raw_info;

#if BMG160_CONFIG_ENABLE_DIAG_LOG
  sns_diag_service* diag = state->diag_service;
  // Allocate Sensor State Raw log packets for Gyro
  sns_memzero(&log_gyro_state_raw_info, sizeof(log_gyro_state_raw_info));
  log_gyro_state_raw_info.encoded_sample_size = state->log_raw_encoded_size;
  log_gyro_state_raw_info.diag = diag;
  log_gyro_state_raw_info.instance = instance;
  log_gyro_state_raw_info.sensor_uid = &state->gyro_info.suid;
  bmg160_log_sensor_state_raw_alloc(&log_gyro_state_raw_info, 0);
#endif

  for(i = 0; i < num_bytes; i += 6)
  {
    //sns_time timestamp = state->fifo_info.timestamp + (num_sample_sets++ * ts_itvl);
    sns_time timestamp = ts_ff_start + (num_sample_sets++ * ts_ff_itvl);
    //SNS_VENDOR_PROFILE(SNS_LOWLATENCY_GYRO_DRIVER_TS, state->fifo_info.isr_ts, timestamp);
#if BMG160_ENABLE_LOG_LEVEL_1
    BMG160_INST_PRINTF(LOW, instance, "ts before fd:%u, isr cnt:%u, accum ff cnt:%u",
                       (uint32_t) timestamp,
                       (uint32_t) state->fifo_info.ff_isr_cnt,
                       (uint32_t) (state->fifo_info.ff_accum_cnt + 1));
#endif
    // group delay
    timestamp -= state->gyro_info.ticks_filter_delay;
    state->fifo_info.ff_accum_cnt ++;
    bmg160_handle_gyro_sample(&fifo_start[i],
                                timestamp,
                                instance,
                                event_service,
                                &log_gyro_state_raw_info);
#if BMG160_ENABLE_LOG_LEVEL_1
    BMG160_INST_PRINTF(LOW, instance, "ff ts: <%d %u %u %u %u %u> accum ff_cnt: <%u %u>",
        i / 6,
        (uint32_t)(timestamp>>32),
        (uint32_t)timestamp,
        (uint32_t) state->fifo_info.isr_ts,
        (uint32_t)(ts_ff_itvl), (uint32_t) sns_get_system_time(),
        (uint32_t)(state->fifo_info.ff_accum_cnt >> 32),
        (uint32_t)state->fifo_info.ff_accum_cnt);
#endif
  }

#if BMG160_CONFIG_ENABLE_DIAG_LOG
  bmg160_log_sensor_state_raw_submit(&log_gyro_state_raw_info, true);
#else
  UNUSED_VAR(log_gyro_state_raw_info);
#endif
}

/** See bmg160_hal.h */
void bmg160_send_fifo_flush_done(sns_sensor_instance *const instance,
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



static
void bmg160_hal_fifo_read_out(
    sns_sensor_instance *const inst,
    bmg160_fifo_read_ctx_t  *ctx,
    uint64_t timestamp)
{
  bmg160_instance_state   *istate = (bmg160_instance_state *) inst->state->state;
  uint32_t ff_len = 0;
  sns_sensor_uid *suid = &istate->gyro_info.suid;
  uint8_t fifo_overrun = 0;

#if BMG160_CONFIG_ENABLE_FIFO
  uint32_t enc_len;
  bool irq_trigger = false;
  uint8_t buffer[100];
  sns_port_vector async_read_msg;
  uint8_t fifo_status;
  sns_rc rc;
  sns_time ts_start = sns_get_system_time();
  sns_time ts_end;
#endif

#if BMG160_CONFIG_ENABLE_FIFO
  if (istate->low_latency_req)
  {
    // data ready mode
    ff_len = 1;
  }
  else
  {
    rc = bmg160_read_fifo_status(istate, &fifo_status);
    BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, inst);
    if (rc != SNS_RC_SUCCESS)
    {
      return ;
    }

    ts_end = sns_get_system_time();
    if ((ts_end - ts_start) < sns_convert_ns_to_ticks(1000000)) {
      istate->fifo_info.isr_ts = ts_end;
    } else {
      if (istate->gyro_info.curr_odr_regv > 0) {
        istate->fifo_info.isr_ts = ts_start + (istate->fifo_info.ts_fff_intvl_ideal >> istate->gyro_info.curr_odr_regv);
      } else {
        istate->fifo_info.isr_ts = ts_start + (sns_convert_ns_to_ticks(200000)); //200us
      }
    }

    ff_len          = (uint32_t)(fifo_status & 0x7F);
    fifo_overrun    = fifo_status & 0x80;
  }
#else
  ff_len = 1;
#endif

  BMG160_INST_PRINTF(MED, inst, "ctx.type:%d ctx.syc.read:%d ff flush read out ff count:%d, overrun:%d",
      ctx->ctx_type, ctx->sync_read, ff_len, fifo_overrun);


  if ((ff_len > 0) && (!istate->fifo_info.ascp_in_progress))
  {
#if BMG160_CONFIG_ENABLE_FIFO
    if (ff_len <= BMG160_FIFO_MAX_LEN_READ_SYNC)
    {
      ctx->sync_read = 1;
    }

    if (ctx->ctx_type == BMG160_FIFO_READ_CTX_TYPE_WMI)
    {
      istate->fifo_info.isr_ts  = timestamp;
      irq_trigger = true;
      istate->fifo_info.ff_isr_cnt ++;
    }

    // Compose the async com port message
    async_read_msg.bytes    = ff_len * BMG160_FIFO_FRAME_SIZE;
    async_read_msg.reg_addr = BMG160_REGA_FIFO_DATA_ADDR;
    async_read_msg.is_write = false;
    async_read_msg.buffer   = NULL;


    if (!istate->low_latency_req)
    {
      bool update_ts_success = false;
      // fifo mode
      // derive the ts base on the interrupt timestamp
      update_ts_success = bmg160_hal_update_sample_ts(inst, ff_len,
                                                      irq_trigger,
                                                      false, istate->fifo_info.isr_ts);
      if (!update_ts_success) {}
    }


    if (!ctx->sync_read) {
      sns_ascp_create_encoded_vectors_buffer (
          &async_read_msg, 1, true, buffer, sizeof(buffer), &enc_len);

      // Send message to Async Com Port
      sns_request async_com_port_request = (sns_request)
          {.message_id = SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_VECTOR_RW,
           .request_len = enc_len, .request = buffer};
      istate->async_com_port_data_stream->api->send_request (
          istate->async_com_port_data_stream, &async_com_port_request);

      // async in process
      istate->fifo_info.ascp_in_progress = true;
    } else {
      if (istate->low_latency_req) {
        //data ready
        // sync mode
        uint32_t xfer_bytes;
        uint8_t gyro_buffer[BMG160_FRAME_SIZE]= {0};

        istate->fifo_info.timestamp = timestamp;

        rc = bmg160_com_read_wrapper(istate->scp_service,
                              istate->com_port_info.port_handle,
                              BMG160_REGA_DATAX_LSB,
                              gyro_buffer,
                              BMG160_FRAME_SIZE,
                              &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, inst);
        if (rc != SNS_RC_SUCCESS)
        {
          return ;
        }

        bmg160_process_fifo_data_buffer(inst,
                                        istate->fifo_info.timestamp,
                                        istate->fifo_info.ts_fff_intvl_est,
                                        gyro_buffer,
                                        BMG160_FRAME_SIZE);
      }
      else
      {
        //TODO3: synchronous fifo read
        uint32_t xfer_bytes = 0;
        uint8_t fifo_data_buffer[async_read_msg.bytes];
        sns_time ts_itvl = istate->fifo_info.ts_fff_intvl_est;

        if (istate->fifo_info.ts_fix_flag)
        {
          ts_itvl = istate->fifo_info.ts_itvl_fast_fixed;
        }

        // read fifo data
        rc = bmg160_com_read_wrapper (istate->scp_service,
                                      istate->com_port_info.port_handle,
                                      async_read_msg.reg_addr,
                                      fifo_data_buffer,
                                      async_read_msg.bytes,
                                      &xfer_bytes);
        BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, inst);
        if (rc != SNS_RC_SUCCESS)
        {
          return ;
        }

        // process fifo data
        bmg160_process_fifo_data_buffer (
            inst,
            istate->fifo_info.timestamp,
            ts_itvl,
            fifo_data_buffer, async_read_msg.bytes);

        if (istate->fifo_info.ff_flush_in_proc)
        {
          istate->fifo_info.ff_flush_in_proc = false;
          bmg160_send_fifo_flush_done (inst, suid);
        }
      }
    }
#else
    //data ready
    // sync mode
    uint32_t xfer_bytes;
    uint8_t gyro_buffer[BMG160_FRAME_SIZE]= {0};

    istate->fifo_info.timestamp = timestamp;

    istate->fifo_info.ff_isr_cnt ++;
    bmg160_com_read_wrapper(istate->scp_service,
                            istate->com_port_info.port_handle,
                            BMG160_REGA_DATAX_LSB,
                            gyro_buffer, BMG160_FRAME_SIZE, &xfer_bytes);
    bmg160_process_fifo_data_buffer(inst, timestamp,
                                    istate->fifo_info.ts_fff_intvl_est,
                                    gyro_buffer, BMG160_FRAME_SIZE);
#endif
  }
  else
  {
    // reset FIFO istate if there are no samples to flush
    if (istate->fifo_info.ff_flush_in_proc)
    {
      istate->fifo_info.ff_flush_in_proc = false;
      bmg160_send_fifo_flush_done (inst, suid);
    }
  }
}



void bmg160_handle_interrupt_event(
    sns_sensor_instance *const inst,
    sns_interrupt_event *irq_event,
    bmg160_fifo_read_ctx_type_t ctx_type
    )
{
  bmg160_fifo_read_ctx_t ctx;
  uint64_t ts = sns_get_system_time();

  ctx.ctx_type = ctx_type;
#if BMG160_CONFIG_ENABLE_FIFO
  ctx.sync_read = false;
#else
  ctx.sync_read = true;
#endif

  if (ctx_type == BMG160_FIFO_READ_CTX_TYPE_WMI)
  {
    if (irq_event != NULL)
    {
      ts = irq_event->timestamp;
      BMG160_INST_PRINTF(MED, inst, "irq ts:%u", ts);
    }
  }

  // read out fifo data with ctx
  bmg160_hal_fifo_read_out(inst, &ctx, ts);
}



void bmg160_hal_fifo_drain(
    sns_sensor_instance *const instance,
        bool                    sync_read,
        bmg160_fifo_flush_trigger_t trigger)
{
  bmg160_instance_state   *istate = (bmg160_instance_state*)instance->state->state;
  bmg160_fifo_read_ctx_t ctx;

  ctx.ctx_type = BMG160_FIFO_READ_CTX_TYPE_FLUSH;
  ctx.sync_read = sync_read;

#if BMG160_CONFIG_ENABLE_FLUSH_SYNC
  if (trigger == BMG160_FIFO_FLUSH_TRIGGER_CLIENTS)
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

  BMG160_INST_PRINTF (MED, instance, "fifo_flush trigger: %d %d", trigger, sync_read);

  bmg160_hal_fifo_read_out (instance, &ctx, sns_get_system_time());
}


/**
 * see sns_bmg160_hal.h
 */
void bmg160_dump_reg(sns_sensor_instance * const this)
{
  if (this == NULL)
  {
    return ;
  }
#if BMG160_CONFIG_ENABLE_DUMP_CORE_REG
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  struct group_read {
    uint32_t first_reg;
    uint8_t  num_regs;
  } groups[] = {
      {BMG160_REGA_DSP_RANGE_ADDR, 5},
      {BMG160_REGA_INT_ENABLE0, 5},
      {BMG160_REGA_INT_4_ADDR, 1},
      {BMG160_REGA_FIFO_CGF1_ADDR, 2}
  };
  uint32_t xfer_bytes;
  uint8_t regv_buffer[32] = {0};
  uint8_t *p_regv = regv_buffer;
  sns_rc rv = SNS_RC_SUCCESS;

  for(uint32_t i=0; i<ARR_SIZE(groups); i++)
  {
    rv = bmg160_com_read_wrapper(state->scp_service,
                                 state->com_port_info.port_handle,
                                 groups[i].first_reg,
                                 p_regv,
                                 groups[i].num_regs,
                                 &xfer_bytes);
    for(uint32_t j=0; j<groups[i].num_regs; j++)
    {
      BMG160_INST_PRINTF(LOW, this, "rv:%d dump: 0x%02x = 0x%02x",
                         rv,
                         groups[i].first_reg+j, regv_buffer[j]);
    }
    p_regv += groups[i].num_regs;
  }

#else
  UNUSED_VAR(this);
#endif
}


sns_rc sns_see_bmg160_read_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes)
{
   uint32_t reg_addr = rega;
   uint32_t xfer_bytes;
   return bmg160_com_read_wrapper(scp_service,
                                  port_handle,
                                  reg_addr,
                                  regv_buffer,
                                  bytes,
                                  &xfer_bytes);
}

sns_rc sns_see_bmg160_write_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes)
{
   uint32_t reg_addr = rega;
   uint32_t xfer_bytes;
   return bmg160_com_write_wrapper(scp_service,
                                  port_handle,
                                  reg_addr,
                                  regv_buffer,
                                  bytes,
                                  &xfer_bytes,
                                  false);
}

/** See sns_bmg160_hal.h */
void bmg160_send_config_event(sns_sensor_instance *const instance)
{
  bmg160_instance_state *state =
     (bmg160_instance_state*)instance->state->state;

  sns_std_sensor_physical_config_event phy_sensor_config =
     sns_std_sensor_physical_config_event_init_default;

  // TODO: Use appropriate op_mode selected by driver.
  char operating_mode[] = "NORMAL";

  pb_buffer_arg op_mode_args;

  op_mode_args.buf = &operating_mode[0];
  op_mode_args.buf_len = sizeof(operating_mode);

  phy_sensor_config.has_sample_rate = true;
  if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR_OFF)
  {
    phy_sensor_config.sample_rate = 0.0f;
  }
  else
  {
    if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR_OFF)
    {
      phy_sensor_config.sample_rate = 0.0f;
    }
    else
    {
      if (state->gyro_info.curr_odr_regv <= BMG160_GYRO_ODR2000_FB230)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_2000HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR1000)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_1000HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR400)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_400HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR200_FB23)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_200HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR100_FB12)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_100HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR200_FB64)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_200HZ;
      }
      else if (state->gyro_info.curr_odr_regv == BMG160_GYRO_ODR100_FB32)
      {
        phy_sensor_config.sample_rate = BMG160_GYRO_ODR_100HZ;
      }
    }
  }

  BMG160_INST_PRINTF(MED, instance, "phy cfg:%d %d %u",
       state->gyro_info.curr_odr_regv,
                     state->fifo_info.fifo_rate, (uint32_t) (phy_sensor_config.sample_rate));

#if BMG160_CONFIG_ENABLE_FIFO
  if (state->low_latency_req)
  {
    // data ready mode
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
  phy_sensor_config.operation_mode.funcs.encode = pb_encode_string_cb;
  phy_sensor_config.operation_mode.arg = &op_mode_args;
  phy_sensor_config.has_active_current = true;
  phy_sensor_config.active_current = BMG160_ACTIVE_CURRENT;
  phy_sensor_config.has_resolution = true;
  phy_sensor_config.resolution = state->gyro_info.resolution;
  phy_sensor_config.range_count = 2;
  phy_sensor_config.range[0] = BMG160_RANGE_BASE_MIN * (1 << (BMG160_GYRO_RANGE_125DPS - state->gyro_info.range_regv));
  phy_sensor_config.range[1] = BMG160_RANGE_BASE_MAX * (1 << (BMG160_GYRO_RANGE_125DPS - state->gyro_info.range_regv));
  phy_sensor_config.has_stream_is_synchronous = true;
  phy_sensor_config.stream_is_synchronous = false;
  phy_sensor_config.has_dri_enabled = true;
  phy_sensor_config.dri_enabled = true;
  phy_sensor_config.has_DAE_watermark = false;
  phy_sensor_config.DAE_watermark = 0;

  if(state->fifo_info.publish_sensors & BMG160_GYRO)
  {
    pb_send_event(instance,
                  sns_std_sensor_physical_config_event_fields,
                  &phy_sensor_config,
                  sns_get_system_time(),
                  SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT,
                  &state->gyro_info.sstate->my_suid);
    bmg160_send_cal_event(instance, BMG160_GYRO);
  }

  if(state->fifo_info.publish_sensors & BMG160_SENSOR_TEMP)
  {
    // Override above values with sensor temperature info
    phy_sensor_config.sample_rate = 1;
    phy_sensor_config.has_water_mark = false;
    phy_sensor_config.operation_mode.funcs.encode = pb_encode_string_cb;
    phy_sensor_config.operation_mode.arg = &op_mode_args;
    phy_sensor_config.has_active_current = true;
    phy_sensor_config.active_current = BMG160_ACTIVE_CURRENT;
    phy_sensor_config.has_resolution = true;
    phy_sensor_config.resolution = BMG160_SENSOR_TEMPERATURE_RESOLUTION;
    phy_sensor_config.range_count = 2;
    phy_sensor_config.range[0] = BMG160_SENSOR_TEMPERATURE_RANGE_MIN;
    phy_sensor_config.range[1] = BMG160_SENSOR_TEMPERATURE_RANGE_MAX;
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
                  &state->sensor_temp_info.sstate->my_suid);
  }
}

void bmg160_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const int8_t       temp_data)
{
  bmg160_instance_state *state = (bmg160_instance_state*)instance->state->state;
  int16_t temp_val = (int16_t)(temp_data);
  float float_temp_val = (temp_val) / 2.0;
  float_temp_val += 24.0;

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

/** See sns_bmg160_hal.h */
void bmg160_handle_sensor_temp_sample(sns_sensor_instance *const instance,
                                      uint64_t timeout_ticks)
{
  bmg160_instance_state *state =
     (bmg160_instance_state*)instance->state->state;
  uint8_t regv = 0;
  int8_t temperature_raw_data;
  uint32_t xfer_bytes = 0;
  sns_rc rc;

  rc = bmg160_com_read_wrapper(state->scp_service,
                          state->com_port_info.port_handle,
                          BMG160_REGA_TEMP,
                          &regv,
                          1,
                          &xfer_bytes);
  BMG160_INST_RV_ASSERT_LOG(rc , SNS_RC_SUCCESS, instance);
  if (rc != SNS_RC_SUCCESS)
  {
    return ;
  }

  temperature_raw_data = (int8_t)regv;
  bmg160_convert_and_send_temp_sample(instance, timeout_ticks, temperature_raw_data);
}

void bmg160_start_sensor_temp_polling_timer(sns_sensor_instance *this)
{
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
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

void bmg160_hal_reconfig_hw(sns_sensor_instance * const this)
{
  bmg160_instance_state *state = (bmg160_instance_state*)this->state->state;
  bool enable_fifo_stream = (state->fifo_info.cur_wml > 0);

  BMG160_INST_PRINTF(MED, this, "inst# reconfig hw: wml:%d %d",
                     state->fifo_info.cur_wml, enable_fifo_stream);

  /* streaming */
  if(enable_fifo_stream)
  {
    if ((state->fifo_info.publish_sensors & BMG160_GYRO))
    {
      sns_rc rc = SNS_RC_SUCCESS;

      rc = bmg160_configure_power_satate(this, BMG160_PS_NORMAL);
      BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, this);
      /* resolution base on the HW sensor version*/
#if BMG160_CONFIG_ENABLE_FIFO
      if (!state->low_latency_req)
      {
        // FIFO mode
        bmg160_stop_data_streaming(state);
        rc = bmg160_set_fifo_wml(state);
        BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, this);
      }
#endif
      rc = bmg160_start_fifo_streaming(state);
      BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, this);

      /* Enable interrupt only for Gyro */
      if((state->fifo_info.publish_sensors & (BMG160_GYRO)) &&
          state->irq_info.irq_ready )
      {
        bmg160_enable_data_intr(state, state->fifo_info.fifo_enabled);
      }
    }
    /* Enable timer in case of sensor temperature clients*/
    if(state->fifo_info.publish_sensors & BMG160_SENSOR_TEMP)
    {
      bmg160_set_polling_config(this);
    }
  }
  else
  {
    sns_rc rc = SNS_RC_SUCCESS;
    bmg160_stop_data_streaming(state);
    rc = bmg160_configure_power_satate(this, BMG160_PS_DEEP_SUSPEND);
    BMG160_INST_RV_ASSERT_LOG(rc, SNS_RC_SUCCESS, this);
  }

  state->config_step = BMG160_CONFIG_IDLE; /* done with reconfigure */


  //dump
  bmg160_dump_reg(this);
}


void bmg160_hal_register_interrupt(sns_sensor_instance *  const this)
{
  bmg160_instance_state *state = (bmg160_instance_state *) this->state->state;
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


void bmg160_send_cal_event(sns_sensor_instance *const inst,
                            bmg160_sensor_type sensor_type)
{

  bmg160_instance_state *state =
     (bmg160_instance_state*)inst->state->state;

  sns_cal_event new_calibration_event = sns_cal_event_init_default;
  float bias_data[] = {0,0,0};
  float comp_matrix_data[] = {0,0,0,0,0,0,0,0,0};

  if(sensor_type == BMG160_GYRO)
  {
    bias_data[0] = state->gyro_registry_cfg.fac_cal_bias[0];
    bias_data[1] = state->gyro_registry_cfg.fac_cal_bias[1];
    bias_data[2] = state->gyro_registry_cfg.fac_cal_bias[2];
    comp_matrix_data[0] = state->gyro_registry_cfg.fac_cal_corr_mat.data[0];
    comp_matrix_data[1] = state->gyro_registry_cfg.fac_cal_corr_mat.data[1];
    comp_matrix_data[2] = state->gyro_registry_cfg.fac_cal_corr_mat.data[2];
    comp_matrix_data[3] = state->gyro_registry_cfg.fac_cal_corr_mat.data[3];
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
  if(sensor_type == BMG160_GYRO)
  {
    suid_current = &state->gyro_info.suid;
  }
  else
  {
    suid_current = NULL;
  }

  new_calibration_event.bias.funcs.encode = pb_encode_float_arr_cb;
  new_calibration_event.bias.arg = &buff_arg_bias;
  new_calibration_event.comp_matrix.funcs.encode = pb_encode_float_arr_cb;
  new_calibration_event.comp_matrix.arg = &buff_arg_comp_matrix;
  new_calibration_event.status = SNS_STD_SENSOR_SAMPLE_STATUS_ACCURACY_HIGH;

  BMG160_INST_PRINTF (MED, inst, "Calibration event");

  pb_send_event(inst,
                sns_cal_event_fields,
                &new_calibration_event,
                sns_get_system_time(),
                SNS_CAL_MSGID_SNS_CAL_EVENT,
                suid_current);
}
