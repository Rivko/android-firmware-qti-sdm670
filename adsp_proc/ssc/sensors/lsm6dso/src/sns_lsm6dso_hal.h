#pragma once
/**
 * @file sns_lsm6dso_hal.h
 *
 * Hardware Access Layer functions.
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

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"

#include "sns_lsm6dso_sensor_instance.h"

// Enable for test code
#ifndef LSM6DSO_ENABLE_TEST_CODE
#define LSM6DSO_ENABLE_TEST_CODE      1
#endif

/**
 *  Address registers
 */
#define STM_LSM6DSO_REG_FIFO_CTRL1        (0x07)
#define STM_LSM6DSO_REG_FIFO_CTRL2        (0x08)
#define STM_LSM6DSO_REG_FIFO_CTRL3        (0x09)
#define STM_LSM6DSO_REG_FIFO_CTRL4        (0x0A)
#define STM_LSM6DSO_REG_CNT_BDG1          (0x0B)
#define STM_LSM6DSO_REG_CNT_BDG2          (0x0C)
#define STM_LSM6DSO_REG_INT1_CTRL         (0x0D)
#define STM_LSM6DSO_REG_INT2_CTRL         (0x0E)
#define STM_LSM6DSO_REG_WHO_AM_I          (0x0F)
#define STM_LSM6DSO_REG_CTRL1_XL          (0x10)
#define STM_LSM6DSO_REG_CTRL2_G           (0x11)
#define STM_LSM6DSO_REG_CTRL3             (0x12)
#define STM_LSM6DSO_REG_CTRL4             (0x13)
#define STM_LSM6DSO_REG_CTRL5             (0x14)
#define STM_LSM6DSO_REG_CTRL6_G           (0x15)
#define STM_LSM6DSO_REG_CTRL7_G           (0x16)
#define STM_LSM6DSO_REG_CTRL8_XL          (0x17)
#define STM_LSM6DSO_REG_CTRL9_XL          (0x18)
#define STM_LSM6DSO_REG_CTRL10            (0x19)
#define STM_LSM6DSO_REG_CNT_BDG1          (0x0B)
#define STM_LSM6DSO_REG_ALL_INT_SRC       (0x1A)
#define STM_LSM6DSO_REG_WAKE_SRC          (0x1B)
#define STM_LSM6DSO_REG_TAP_SRC           (0x1C)
#define STM_LSM6DSO_REG_D6D_SRC           (0x1D)
#define STM_LSM6DSO_REG_STATUS            (0x1E)

#define STM_LSM6DSO_REG_OUT_TEMP_L        (0x20)
#define STM_LSM6DSO_REG_OUT_TEMP_H        (0x21)
#define STM_LSM6DSO_REG_OUT_X_L_G         (0x22)
#define STM_LSM6DSO_REG_OUT_X_H_G         (0x23)
#define STM_LSM6DSO_REG_OUT_Y_L_G         (0x24)
#define STM_LSM6DSO_REG_OUT_Y_H_G         (0x25)
#define STM_LSM6DSO_REG_OUT_Z_L_G         (0x26)
#define STM_LSM6DSO_REG_OUT_Z_H_G         (0x27)
#define STM_LSM6DSO_REG_OUT_X_L_XL        (0x28)
#define STM_LSM6DSO_REG_OUT_X_H_XL        (0x29)
#define STM_LSM6DSO_REG_OUT_Y_L_XL        (0x2A)
#define STM_LSM6DSO_REG_OUT_Y_H_XL        (0x2B)
#define STM_LSM6DSO_REG_OUT_Z_L_XL        (0x2C)
#define STM_LSM6DSO_REG_OUT_Z_H_XL        (0x2D)
#define STM_LSM6DSO_REG_FIFO_STATUS1      (0x3A)
#define STM_LSM6DSO_REG_FIFO_STATUS2      (0x3B)
#define STM_LSM6DSO_REG_FIFO_CNT_BDR_L    (0x3C)
#define STM_LSM6DSO_REG_FIFO_CNT_BDR_H    (0x3D)
#define STM_LSM6DSO_REG_TIMESTAMP0        (0x40)
#define STM_LSM6DSO_REG_TIMESTAMP1        (0x41)
#define STM_LSM6DSO_REG_TIMESTAMP2        (0x42)
#define STM_LSM6DSO_REG_TIMESTAMP3        (0x43)
#define STM_LSM6DSO_REG_TAP_CFG0          (0x56)
#define STM_LSM6DSO_REG_TAP_CFG1          (0x57)
#define STM_LSM6DSO_REG_TAP_CFG2          (0x58)
#define STM_LSM6DSO_REG_TAP_THS_6D        (0x59)
#define STM_LSM6DSO_REG_TAP_DUR           (0x5A)
#define STM_LSM6DSO_REG_WAKE_THS          (0x5B)
#define STM_LSM6DSO_REG_WAKE_DUR          (0x5C)
#define STM_LSM6DSO_REG_FREE_FALL         (0x5D)
#define STM_LSM6DSO_REG_MD1_CFG           (0x5E)
#define STM_LSM6DSO_REG_MD2_CFG           (0x5F)
#define STM_LSM6DSO_REG_FIFO_OUT_TAG      (0x78)
#define STM_LSM6DSO_REG_FIFO_OUT_X_L      (0x79)
#define STM_LSM6DSO_REG_FIFO_OUT_X_H      (0x7A)

#define STM_LSM6DSO_FIFO_WTM_STATUS_MASK   (0x80)
#define STM_LSM6DSO_FIFO_OVR_STATUS_MASK   (0x40)
#define STM_LSM6DSO_FIFO_FULL_STATUS_MASK  (0x20)
#define STM_LSM6DSO_FIFO_WTM_CTRL1_MASK    (0xFF)
#define STM_LSM6DSO_FIFO_WTM_CTRL2_MASK    (0x01)
#define STM_LSM6DSO_FIFO_MODE_MASK         (0x07)
#define STM_LSM6DSO_FS_XL_MASK             (0x0C)
#define STM_LSM6DSO_ODR_XL_MASK            (0xF0)
#define STM_LSM6DSO_ODR_BW_XL_MASK         (0xF3)
#define STM_LSM6DSO_FS_G_MASK              (0x0E)
#define STM_LSM6DSO_ODR_G_MASK             (0xF0)
#define STM_LSM6DSO_FIFO_WTM_INT_MASK      (0x08)
#define STM_LSM6DSO_FIFO_OVR_INT_MASK      (0x30)
#define STM_LSM6DSO_FIFO_DIFF1_MASK        (0xFF)
#define STM_LSM6DSO_FIFO_DIFF2_MASK        (0x07)
#define LSM6DSO_FIFO_STREAM_MODE    0x06 // fifo stream continuous mode
#define STM_LSM6DSO_GYRO_TAG		0x01
#define STM_LSM6DSO_ACC_TAG			0x02
#define STM_LSM6DS0_EXT_TAG			0x0f
#define STM_LSM6DSO_SAMPLE_SIZE		          6
#define STM_LSM6DSO_TAG_SIZE		            1
#define STM_LSM6DSO_FIFO_SAMPLE_SIZE	(STM_LSM6DSO_SAMPLE_SIZE + \
					 STM_LSM6DSO_TAG_SIZE)
#define STM_LSM6DSO_MAX_FIFO_SIZE	3584
#define STM_LSM6DSO_MAX_FIFO_DEPTH	(STM_LSM6DSO_MAX_FIFO_SIZE / \
					 STM_LSM6DSO_FIFO_SAMPLE_SIZE)


/** Default values loaded in probe function */
#define LSM6DSO_WHOAMI_VALUE              (0x6C)  /** Who Am I default value */

/** fifo paramters */
//#define LSM6DSO_MAX_FIFO            682 // Max fifo samples 8K bytes to samples

//QC Temp Fix till batching is fixed.
#define LSM6DSO_MAX_FIFO            208

/** Off to idle time */
#define LSM6DSO_OFF_TO_IDLE_MS      100  //ms

/** Gyro turn on/off time */
#define LSM6DSO_GYRO_ON_TIME_MS      70  //ms

/** Motion detect configuration */
#define LSM6DSO_MD_THRESH          (0.6132f)             // m/s2
#define LSM6DSO_MD_DUR             (0.0)                 // sec
#define LSM6DSO_MD_ODR_VAL         (26.0)                // 26 Hz
#define LSM6DSO_MD_ODR             LSM6DSO_ACCEL_ODR26   // 26 Hz
// resolution of MD Threshold Register
#define LSM6DSO_REG_WAKE_THS_RES   (0.6132f) // m/s2 1LSB = FS/2^6 FS = 4 1LSB=0.6132m/s2
#define LSM6DSO_MD_THRESH_MAX      (LSM6DSO_REG_WAKE_THS_RES * (63)) //(uint16_t)pow(2,6) - 1)

#define LSM6DSO_NUM_AXES           3

/******************* Function Declarations ***********************************/

/**
 * Resets the Sensor HW. Also calls
 * lsm6dso_device_set_default_state()
 *
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to reset
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dso_reset_device(
    sns_sensor_instance *const instance,
    lsm6dso_sensor_type sensor);

/**
 * Loads default values in config registers.
 *
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to handle
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dso_device_set_default_state(
    sns_sensor_instance *const instance,
    lsm6dso_sensor_type sensor);

/**
 * Disables FIFO ODR. Also disables ODR for sensors with
 * non-zero ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6dso_stop_fifo_streaming(sns_sensor_instance *const instance);

/**
 * Sets FIFO WM and decimation config registers.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6dso_set_fifo_wmk(sns_sensor_instance *const instance);

/**
 * Enable FIFO streaming. Also enables FIFO sensors with
 * non-zero desired ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6dso_start_fifo_streaming(sns_sensor_instance *const instance);

/**
 * Enables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 * @param[i] sensors       sensor bit mask to enable
 *
 * @return none
 */
void lsm6dso_enable_fifo_intr(sns_sensor_instance *const instance,
                              lsm6dso_sensor_type sensor);
/**
 * Disables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6dso_disable_fifo_intr(sns_sensor_instance *const instance);


/**
 * Gets Who-Am-I register for the sensor.
 *
 * @param[i] state         Instance state
 * @param[o] buffer        who am I value read from HW
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dso_get_who_am_i(sns_sync_com_port_service *scp_service,
		                    sns_sync_com_port_handle  *port_handle,
                            uint8_t *buffer);

/**
 * Sets Accel ODR, range, BW and sensitivity.
 *
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
sns_rc lsm6dso_set_accel_config(
   sns_sensor_instance *const instance,
                                lsm6dso_accel_odr      curr_odr,
                                lsm6dso_accel_sstvt    sstvt,
                                lsm6dso_accel_range    range,
                                lsm6dso_accel_bw       bw);

/**
 * Sets Gyro ODR, range and sensitivity.
 *
 * @param[i] port_handle     handle to synch COM port
 * @param[i] curr_odr        Gyro ODR
 * @param[i] sstvt           Gyro sensitivity
 * @param[i] range           Gyro range
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6dso_set_gyro_config(
    sns_sensor_instance *const instance,
                               lsm6dso_gyro_odr      curr_odr,
                               lsm6dso_gyro_sstvt    sstvt,
                               lsm6dso_gyro_range    range);

/**
 * Populates Instance state with desired FIFO configuration.
 *
 * @param[i] state                 Instance state
 * @param[i] desired_wmk           desired FIFO WM
 * @param[i] a_chosen_sample_rate  desired Accel ODR
 * @param[i] g_chosen_sample_rate  desired GYRO ODR
 * @param[i] sensor                bit mask of Sensors to enable
 *
 * @return none
 */
void lsm6dso_set_fifo_config(sns_sensor_instance *const instance,
                             uint16_t desired_wmk,
                             lsm6dso_accel_odr a_chosen_sample_rate,
                             lsm6dso_gyro_odr g_chosen_sample_rate,
                             lsm6dso_sensor_type sensor);

/**
 * Reads status registers in Instance State.
 * This function is for debug only.
 *
 * @param[i] state                 Instance state
 * @param[i] sensor                bit mask of Sensors to enabl
 *
 * @return none
 */
void lsm6dso_dump_reg(sns_sensor_instance *const instance, lsm6dso_sensor_type sensor);

/**
 * Sets Motion Detect config.
 *
 * @param[i] state        Instance state
 * @param[i] enable       true to enable Motion Accel else false
 *
 * @return none
 */
void lsm6dso_set_md_config(sns_sensor_instance *const instance, bool enable);

/**
 * Updates Motion detect interrupt state.
 *
 * @param[i] state               Instance state
 * @param[i] enable              true to enable Motion Accel
 *                               else false
 * @param[i] md_not_armed_event  true if MD not armed event
 *                               must be sent
 *
 * @return none
 */
void lsm6dso_update_md_intr(sns_sensor_instance *const instance,
                            bool enable,
                            bool md_not_armed_event);

/**
 * Handles MD interrupt:
 *   1. Sends MD fired event.
 *   2. Starts Motion Accel Stream.
 *
 * @param[i] instance        Instance reference
 * @param[i] irq_timestamp   MD interrupt timestamp
 * @param[i] wake_src        WAKE_SRC register contents
 *
 * @return none
 */
void lsm6dso_handle_md_interrupt(sns_sensor_instance *const instance,
                                 sns_time irq_timestamp,
                                 uint8_t const *wake_src);

/**
 * Sets Gated Accel config.
 *
 * @param[i] desired_wmk           desired FIFO WM
 * @param[i] a_chosen_sample_rate  desired Accel ODR
 * @param[i] g_chosen_sample_rate  desired GYRO ODR
 * @param[i] sensor                bit mask of Sensors to enable
 *
 * @return none
 */
void lsm6dso_set_gated_accel_config(lsm6dso_instance_state *state,
                           uint16_t desired_wmk,
                           lsm6dso_accel_odr a_chosen_sample_rate,
                           lsm6dso_sensor_type sensor);

/**
 * Gets current Accel ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float lsm6dso_get_accel_odr(lsm6dso_accel_odr curr_odr);

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time lsm6dso_get_sample_interval(lsm6dso_accel_odr curr_odr);

/**
 * Processes a fifo buffer and extracts accel and gyro samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance               Sensor instance
 * @param[i] gyro_enabled           Whether gyro is enabled
 * @param[i] first_timestamp        Timestamp of first sample in fifo
 * @param[i] sample_interval_ticks  Sampling interval in time ticks
 * @param[i] fifo_start             Buffer containing samples read from HW FIFO
 * @param[i] num_bytes              Number of bytes in fifo buffer
 */
void lsm6dso_process_fifo_data_buffer(
  sns_sensor_instance *instance,
  bool                gyro_enabled,
  sns_time            first_timestamp,
  sns_time            sample_interval_ticks,
  const uint8_t       *fifo_start,
  size_t              num_bytes
);

/**
 * read fifo data by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance                 Sensor Instance
 */
void lsm6dso_read_fifo_data(sns_sensor_instance *const instance, sns_time irq_time, bool flush);

/**
 * flush fifo by reading the fifo data and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance                 Sensor Instance
 * @param scp_read                 flush using sync/async read
 */

void lsm6dso_flush_fifo(sns_sensor_instance *const instance);
/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void lsm6dso_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void lsm6dso_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       temp_data[2]);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void lsm6dso_handle_sensor_temp_sample(sns_sensor_instance *const instance);

/**
 * Sends a FIFO complete event.
 *
 * @param instance   Instance reference
 */
void lsm6dso_send_fifo_flush_done(
  sns_sensor_instance *const instance,
  sns_sensor_uid *suid);

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void lsm6dso_start_sensor_temp_polling_timer(sns_sensor_instance *this);

/**
 * Updates temp sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
void lsm6dso_set_polling_config(sns_sensor_instance *const this);

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void lsm6dso_reconfig_hw(sns_sensor_instance *this);

/**
 * Configures sensor with new/recomputed fifo settings
 *
 * @param instance   Instance reference
 */

void lsm6dso_reconfig_fifo(sns_sensor_instance *this, bool flush);

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
sns_rc lsm6dso_read_modify_write(
    sns_sensor_instance *const instance,
    uint32_t reg_addr,
    uint8_t *reg_value,
    uint32_t size,
    uint32_t *xfer_bytes,
    bool save_write_time,
    uint8_t mask);


/**
 * Extract a accel sample from a segment of the fifo buffer and generate an
 * event.
 *
 * @param[i] instance           The current lsm6dso sensor instance
 * @param[i] sensors[]          Array of sensors for which data is requested
 * @param[i] num_sensors        Number of sensor for which data is requested
 * @param[i] raw_data           Uncalibrated sensor data to be logged
 */
void lsm6dso_get_data(sns_sensor_instance *const instance,
                                lsm6dso_sensor_type sensors[],
                                uint8_t num_sensors,
                                float *raw_data);

void lsm6dso_process_com_port_vector(sns_port_vector *vector, void *user_arg);

void lsm6dso_send_cal_event(sns_sensor_instance * const instance, lsm6dso_sensor_type sensor_type);

void lsm6dso_set_md_intr(sns_sensor_instance *const instance, bool enable);

void lsm6dso_update_md_filter(sns_sensor_instance * const instance);

void lsm6dso_register_interrupt(sns_sensor_instance *this);

sns_rc lsm6dso_read_fifo_status(sns_sensor_instance *const instance,
                                     uint8_t *buffer);
sns_rc lsm6dso_recover_device(sns_sensor_instance *const this);

void lsm6dso_inst_create_timer(sns_sensor_instance *this,
    sns_data_stream** timer_data_stream,
    sns_time timeout_ticks,
    bool is_periodic);
