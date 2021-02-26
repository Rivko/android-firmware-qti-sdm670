#pragma once
/**
 * @file sns_lsm6ds3_hal.h
 *
 * Hardware Access Layer functions.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_gpio_service.h"

#include "sns_lsm6ds3_sensor_instance.h"

/**
 *  Address registers
 */
#define STM_LSM6DS3_REG_FIFO_CTRL1        (0x06)
#define STM_LSM6DS3_REG_FIFO_CTRL2        (0x07)
#define STM_LSM6DS3_REG_FIFO_CTRL3        (0x08)
#define STM_LSM6DS3_REG_FIFO_CTRL4        (0x09)
#define STM_LSM6DS3_REG_FIFO_CTRL5        (0x0A)
#define STM_LSM6DS3_REG_INT1_CTRL         (0x0D)
#define STM_LSM6DS3_REG_INT2_CTRL         (0x0E)
#define STM_LSM6DS3_REG_WHO_AM_I          (0x0F)
#define STM_LSM6DS3_REG_CTRL1_A           (0x10)
#define STM_LSM6DS3_REG_CTRL2_G           (0x11)
#define STM_LSM6DS3_REG_CTRL3             (0x12)
#define STM_LSM6DS3_REG_CTRL4             (0x13)
#define STM_LSM6DS3_REG_CTRL5             (0x14)
#define STM_LSM6DS3_REG_CTRL6_G           (0x15)
#define STM_LSM6DS3_REG_CTRL7_G           (0x16)
#define STM_LSM6DS3_REG_CTRL8_XL          (0x17)
#define STM_LSM6DS3_REG_CTRL9_A           (0x18)
#define STM_LSM6DS3_REG_CTRL10            (0x19)
#define STM_LSM6DS3_REG_WAKE_SRC          (0x1B)
#define STM_LSM6DS3_REG_TAP_SRC           (0x1C)
#define STM_LSM6DS3_REG_D6D_SRC           (0x1D)
#define STM_LSM6DS3_REG_STATUS            (0x1E)

#define STM_LSM6DS3_REG_OUT_TEMP_L        (0x20)
#define STM_LSM6DS3_REG_OUT_TEMP_H        (0x21)
#define STM_LSM6DS3_REG_OUT_X_L_G         (0x22)
#define STM_LSM6DS3_REG_OUT_X_H_G         (0x23)
#define STM_LSM6DS3_REG_OUT_Y_L_G         (0x24)
#define STM_LSM6DS3_REG_OUT_Y_H_G         (0x25)
#define STM_LSM6DS3_REG_OUT_Z_L_G         (0x26)
#define STM_LSM6DS3_REG_OUT_Z_H_G         (0x27)
#define STM_LSM6DS3_REG_OUT_X_L_XL        (0x28)
#define STM_LSM6DS3_REG_OUT_X_H_XL        (0x29)
#define STM_LSM6DS3_REG_OUT_Y_L_XL        (0x2A)
#define STM_LSM6DS3_REG_OUT_Y_H_XL        (0x2B)
#define STM_LSM6DS3_REG_OUT_Z_L_XL        (0x2C)
#define STM_LSM6DS3_REG_OUT_Z_H_XL        (0x2D)
#define STM_LSM6DS3_REG_SENSOR_HUB10      (0x37)
#define STM_LSM6DS3_REG_FIFO_STATUS1      (0x3A)
#define STM_LSM6DS3_REG_FIFO_STATUS2      (0x3B)
#define STM_LSM6DS3_REG_FIFO_STATUS3      (0x3C)
#define STM_LSM6DS3_REG_FIFO_STATUS4      (0x3D)
#define STM_LSM6DS3_REG_FIFO_DATA_OUT_L   (0x3E)
#define STM_LSM6DS3_REG_FIFO_DATA_OUT_H   (0x3F)
#define STM_LSM6DS3_REG_TIMESTAMP0_REG    (0x40)
#define STM_LSM6DS3_REG_TIMESTAMP1_REG    (0x41)
#define STM_LSM6DS3_REG_TIMESTAMP2_REG    (0x42)
#define STM_LSM6DS3_REG_STEP_COUNTER_L    (0x4B)
#define STM_LSM6DS3_REG_STEP_COUNTER_H    (0x4C)
#define STM_LSM6DS3_REG_FUNC_SRC          (0x53)
#define STM_LSM6DS3_REG_TAP_CFG           (0x58)
#define STM_LSM6DS3_REG_TAP_THS_6D        (0x59)
#define STM_LSM6DS3_REG_TAP_DUR           (0x5A)
#define STM_LSM6DS3_REG_WAKE_THS          (0x5B)
#define STM_LSM6DS3_REG_WAKE_DUR          (0x5C)
#define STM_LSM6DS3_REG_MD1_CFG           (0x5E)
#define STM_LSM6DS3_REG_MD2_CFG           (0x5F)

#define STM_LSM6DS3_FIFO_WTM_STATUS_MASK   (0x80)
#define STM_LSM6DS3_FIFO_OVR_STATUS_MASK   (0x40)
#define STM_LSM6DS3_FIFO_FULL_STATUS_MASK  (0x20)
#define STM_LSM6DS3_FIFO_EMPTY_STATUS_MASK (0x10)
#define STM_LSM6DS3_FIFO_PATTERN_STATUS3   (0xFF)
#define STM_LSM6DS3_FIFO_PATTERN_STATUS4   (0x03)
#define STM_LSM6DS3_FIFO_WTM_CTRL1_MASK    (0xFF)
#define STM_LSM6DS3_FIFO_WTM_CTRL2_MASK    (0x0F)
#define STM_LSM6DS3_FIFO_DEC_XL_MASK       (0x07)
#define STM_LSM6DS3_FIFO_DEC_G_MASK        (0x38)
#define STM_LSM6DS3_FIFO_MODE_MASK         (0x07)
#define STM_LSM6DS3_FIFO_ODR_MASK          (0x78)
#define STM_LSM6DS3_INT1_FTH_MASK          (0x08)
#define STM_LSM6DS3_INT1_OVR_MASK          (0x10)
#define STM_LSM6DS3_FIFO_INT_MASK          (0x38)
#define STM_LSM6DS3_FS_XL_MASK             (0x0C)
#define STM_LSM6DS3_ODR_XL_MASK            (0xF0)
#define STM_LSM6DS3_ODR_BW_XL_MASK         (0xF3)
#define STM_LSM6DS3_FS_125_MASK            (0x02)
#define STM_LSM6DS3_FS_G_MASK              (0x0C)
#define STM_LSM6DS3_ODR_G_MASK             (0xF0)


/** Default values loaded in probe function */
#define LSM6DS3_WHOAMI_VALUE              (0x69)  /** Who Am I default value */

// Actual H/w supported max FIFO is 1365
// Temporily reduced to fix CTS failures

/** fifo paramters */
#define LSM6DS3_MAX_FIFO            208 // Max fifo samples 8K bytes to samples

/** Off to idle time */
#define LSM6DS3_OFF_TO_IDLE_MS      100  //ms

#define LSM6DS3_HEART_BEAT_ODR_COUNT 3


/** Motion detect configuration */
#define LSM6DS3_MD_THRESH          (0.6132f)             // m/s2
#define LSM6DS3_MD_DUR             (0.0)                 // sec
#define LSM6DS3_MD_ODR_VAL         (26.0)                // 26 Hz
#define LSM6DS3_MD_ODR             LSM6DS3_ACCEL_ODR26   // 26 Hz
// resolution of MD Threshold Register
#define LSM6DS3_REG_WAKE_THS_RES   (0.6132f) // m/s2 1LSB = FS/2^6 FS = 4 1LSB=0.6132m/s2
#define LSM6DS3_MD_THRESH_MAX      (LSM6DS3_REG_WAKE_THS_RES * ((uint16_t)pow(2,6) - 1))

#define LSM6DS3_NUM_AXES           3

#define LSM6DS3_SAMPLES_LIMIT      5
/******************* Function Declarations ***********************************/

/**
 * Resets the Sensor HW. Also calls
 * lsm6ds3_device_set_default_state()
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to reset
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6ds3_reset_device(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            lsm6ds3_sensor_type sensor);

/**
 * Loads default values in config registers.
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to handle
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6ds3_device_set_default_state(sns_sync_com_port_service *scp_service,
                                        sns_sync_com_port_handle *port_handle,
                                        lsm6ds3_sensor_type sensor);

/**
 * Disables FIFO ODR. Also disables ODR for sensors with
 * non-zero ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6ds3_stop_fifo_streaming(lsm6ds3_instance_state *state);

/**
 * Sets FIFO WM and decimation config registers.
 *
 * @param[i] instance         Instance
 *
 * @return none
 */
void lsm6ds3_set_fifo_wmk(sns_sensor_instance *const instance);

/**
 * Enable FIFO streaming. Also enables FIFO sensors with
 * non-zero desired ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6ds3_start_fifo_streaming(lsm6ds3_instance_state *state);

/**
 * Enables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 * @param[i] sensors       sensor bit mask to enable
 *
 * @return none
 */
void lsm6ds3_enable_fifo_intr(lsm6ds3_instance_state *state,
                              lsm6ds3_sensor_type sensor);
/**
 * Disables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void lsm6ds3_disable_fifo_intr(lsm6ds3_instance_state *state);

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
sns_rc lsm6ds3_get_who_am_i(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle  *port_handle,
                            uint8_t                   *buffer);

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
sns_rc lsm6ds3_set_accel_config(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                lsm6ds3_accel_odr      curr_odr,
                                lsm6ds3_accel_sstvt    sstvt,
                                lsm6ds3_accel_range    range,
                                lsm6ds3_accel_bw       bw);

/**
 * Sets Gyro ODR, range and sensitivity.
 *
 * @param[i] scp_service     handle to synch COM port service
 * @param[i] port_handle     handle to synch COM port
 * @param[i] curr_odr        Gyro ODR
 * @param[i] sstvt           Gyro sensitivity
 * @param[i] range           Gyro range
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc lsm6ds3_set_gyro_config(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle,
                               lsm6ds3_gyro_odr      curr_odr,
                               lsm6ds3_gyro_sstvt    sstvt,
                               lsm6ds3_gyro_range    range);

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
void lsm6ds3_set_fifo_config(lsm6ds3_instance_state *state,
                             uint16_t desired_wmk,
                             lsm6ds3_accel_odr a_chosen_sample_rate,
                             lsm6ds3_gyro_odr g_chosen_sample_rate,
                             lsm6ds3_sensor_type sensor);

/**
 * Reads status registers in Instance State.
 * This function is for debug only.
 *
 * @param[i] state                 Instance state
 * @param[i] sensor                bit mask of Sensors to enabl
 *
 * @return none
 */
void lsm6ds3_dump_reg(sns_sensor_instance *this, lsm6ds3_sensor_type sensor);

/**
 * Sets Motion Detect config.
 *
 * @param[i] state        Instance state
 * @param[i] enable       true to enable Motion Accel else false
 *
 * @return none
 */
void lsm6ds3_set_md_config(lsm6ds3_instance_state *state, bool enable);

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
void lsm6ds3_update_md_intr(sns_sensor_instance *const instance,
                            bool enable,
                            bool md_not_armed_event);

/**
 * Handles MD interrupt:
 *   1. Sends MD fired event.
 *   2. Starts Motion Accel Stream.
 *
 * @param[i] instance        Instance reference
 * @param[i] irq_timestamp   MD interrupt timestamp
 *
 * @return none
 */
void lsm6ds3_handle_md_interrupt(sns_sensor_instance *const instance,
                                   sns_time irq_timestamp);

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
void lsm6ds3_set_gated_accel_config(lsm6ds3_instance_state *state,
                           uint16_t desired_wmk,
                           lsm6ds3_accel_odr a_chosen_sample_rate,
                           lsm6ds3_sensor_type sensor);

/**
 * Gets current Accel ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float lsm6ds3_get_accel_odr(lsm6ds3_accel_odr curr_odr);

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time lsm6ds3_get_sample_interval(lsm6ds3_accel_odr curr_odr);

/**
 * Processes a fifo buffer and extracts accel and gyro samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance     Sensor instance
 * @param[i] gyro_enabled Whether gyro is enabled
 * @param[i] first_ts     Timestamp of first sample in fifo
 * @param[i] interval     Sampling interval in time ticks
 * @param[i] fifo         Buffer containing samples read from HW FIFO
 * @param[i] num_bytes    Number of bytes in fifo buffer
 */
void lsm6ds3_process_fifo_data_buffer(sns_sensor_instance *instance,
                                      bool                gyro_enabled,
                                      sns_time            first_ts,
                                      sns_time            interval,
                                      const uint8_t       *fifo,
                                      size_t              num_bytes)
;

/**
 * Handle an interrupt/FLush by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous/syncromous com port sensor to read the fifo.
 *
 * @param[i]        Sensor Instance
 * @param[i]        irq_time
 */
void lsm6ds3_handle_fifo_data(sns_sensor_instance *const instance,
  sns_time irq_time);

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void lsm6ds3_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void lsm6ds3_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const uint8_t       temp_data[2]);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 * @param[i] timeout_time timestamp in ticks
 */
void lsm6ds3_handle_sensor_temp_sample(sns_sensor_instance *const instance,
                                       uint64_t timeout_ticks);

/**
 * Sends a FIFO complete event.
 *
 * @param[i] instance   Instance reference 
 * @param[i] suid       sensor uid 
 */
void lsm6ds3_send_fifo_flush_done(sns_sensor_instance *const instance,
                                  sns_sensor_uid *suid);

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void lsm6ds3_start_sensor_temp_polling_timer(sns_sensor_instance *this);

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
void lsm6ds3_reconfig_hw(sns_sensor_instance *this);

/**
 * Reads value of a GPIO pin.
 * Function has been written to demonstrate use of the GPIO
 * Service to read gpio.
 *
 * @param[i] instance     instance reference
 * @param[i] gpio         gpio pin to read
 * @param[i] is_chip_pin  true if this is a chip level TLMM pin
 *       else false.
 *
 * @return none
 */
void lsm6ds3_read_gpio(sns_sensor_instance *instance, uint32_t gpio, bool is_chip_pin);

/**
 * Writes to a GPIO pin.
 * Function has been written to demonstrate use of the GPIO
 * Service to write to gpio.
 *
 * @param[i] instance        instance reference
 * @param[i] gpio            gpio pin to write to
 * @param[i] is_chip_pin     true if this is a chip level TLMM
 *       pin
 * @param[i] drive_strength  gpio pin drive strength
 * @param[i] pull            pull type config
 * @param[i] state           output state to write
 *
 * @return none
 */
#ifndef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
void lsm6ds3_write_gpio(sns_sensor_instance *instance, uint32_t gpio,
                        bool is_chip_pin,
                        sns_gpio_drive_strength drive_strength,
                        sns_gpio_pull_type pull,
                        sns_gpio_state state);
#endif
/**
 * Send Calibration event to client
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void lsm6ds3_send_cal_event(sns_sensor_instance * const instance, lsm6ds3_sensor_type sensor_type);
/**
 * Reset Calibration values
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */

void lsm6ds3_reset_cal_data(sns_sensor_instance *const instance,lsm6ds3_sensor_type sensor_type);

void lsm6ds3_register_interrupt(sns_sensor_instance *this);

void lsm6ds3_set_polling_config(sns_sensor_instance *const this);
void lsm6ds3_process_com_port_vector(sns_port_vector *vector, void *user_arg);
void lsm6ds3_inst_create_heart_beat_timer(sns_sensor_instance *this, sns_time timeout_ticks);
