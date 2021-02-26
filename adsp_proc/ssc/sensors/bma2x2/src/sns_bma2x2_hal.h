#pragma once
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

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_gpio_service.h"

#include "sns_bma2x2_sensor_instance.h"


#define BMA2X2_U64_MAX         (0xFFFFFFFF)

#define BST_ABS(x)              (((x) > 0) ? (x) : (0-(x)))
#define BST_MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define BST_MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define BST_MAX3(a, b, c)       (BST_MAX(BST_MAX((a), (b)), (c)))

#define BST_GET_VAL_BIT(val, bit) (((val)>>(bit)) & 0x01)
#define BST_GET_VAL_BITBLOCK(val, start, end) (((val)>>(start)) & ((1<<(end - start + 1))-1))

#define BST_SET_VAL_BIT(val, bit)      (val | (1 << bit))
#define BST_CLR_VAL_BIT(val, bit)      (val & (~(1 << bit)))

#define BST_SET_VAL_BITBLOCK(val, start, end, val_bb) \
    ( (val & \
       (~ (((1<<(end - start + 1))-1) << start) ) \
      )\
      |\
      ((val_bb &\
        ((1<<(end - start + 1))-1)) << start)\
    )

// fifo algo
#define BMA2X2_FIFO_ISR_ITVL_ACCUMULATE_NUM         64
#define BMA2X2_FIFO_TS_DEVIATE_THRESHOLD            5
#define BMA2X2_FIFO_TS_FIXED_HEAD_ROOM              2
#define BMA2X2_FIFO_TS_ISR_CNT_PREPRECISE           10
#define BMA2X2_FIFO_TS_ISR_PRECISE_MOST_WEIGHT      95
#define BMA2X2_HEART_BEAT_ODR_COUNT                 3
#define BMA2X2_FIFO_MAX_LEN_READ_SYNC               3
// bma2x2 has 24 max fifo len to report the
// HIGH HIFO boundary is [-2% * odr, 2% * odr],
// so one max batching ts gap is about ts_odr * 0.02 * fifo_len,
// let suppose the max FIFO frames maybe 30(24), so the max variance of one fifo batching
// is about 0.02 * 30 * odr = 0.6 * odr
//
// we make a the margin we allow the jitter but the interrupt interval is not regularly
// so we allow 2 / 0.6 = 3 jitter shift on one side, then the driver must
// fix this shift issue. maybe this should be fine tune depend
// on the max fifo water mark level and the platform
#define BMA2X2_FIFO_TS_NEED_FIXED_FF_NUM_TOO_LOW            2
#define BMA2X2_FIFO_TS_NEED_FIXED_FF_NUM_TOO_HIGH           1

#define BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC                  5



// Enable for test code
#ifndef BMA2X2_ENABLE_TEST_CODE
#define BMA2X2_ENABLE_TEST_CODE      1
#endif



#define BMA2X2_280_CHIP_ID          0xFB
#define BMA2X2_255_CHIP_ID          0xFA
#define BMA2X2_250E_CHIP_ID         0xF9
#define BMA2X2_222E_CHIP_ID         0xF8
#define BMA2X2_CHIPID_HIGH_BITS_VAL 0xF8
#define BMA2X2_CHIPID_MASK          BMA2X2_CHIPID_HIGH_BITS_VAL


/**
 *  Address registers
 */
#define BMA2X2_REGA_CHIP_ID_REG                      0x00
#define BMA2X2_REGA_VERSION_REG                      0x01
#define BMA2X2_REGA_X_AXIS_LSB_REG                   0x02
#define BMA2X2_REGA_X_AXIS_MSB_REG                   0x03
#define BMA2X2_REGA_Y_AXIS_LSB_REG                   0x04
#define BMA2X2_REGA_Y_AXIS_MSB_REG                   0x05
#define BMA2X2_REGA_Z_AXIS_LSB_REG                   0x06
#define BMA2X2_REGA_Z_AXIS_MSB_REG                   0x07
#define BMA2X2_REGA_TEMP_RD_REG                      0x08
#define BMA2X2_REGA_STATUS1_REG                      0x09
#define BMA2X2_REGA_STATUS2_REG                      0x0A
#define BMA2X2_REGA_STATUS_TAP_SLOPE_REG             0x0B
#define BMA2X2_REGA_STATUS_ORIENT_HIGH_REG           0x0C
#define BMA2X2_REGA_STATUS_FIFO_REG                  0x0E
#define BMA2X2_REGA_RANGE_SEL_REG                    0x0F
#define BMA2X2_REGA_BW_SEL_REG                       0x10
#define BMA2X2_REGA_MODE_CTRL_REG                    0x11
#define BMA2X2_REGA_LOW_NOISE_CTRL_REG               0x12
#define BMA2X2_REGA_DATA_CTRL_REG                    0x13
#define BMA2X2_REGA_RESET_REG                        0x14
#define BMA2X2_REGA_INT_ENABLE1_REG                  0x16  /*motion*/
#define BMA2X2_REGA_INT_ENABLE2_REG                  0x17
#define BMA2X2_REGA_INT_SLO_NO_MOT_REG               0x18
#define BMA2X2_REGA_INT1_PAD_SEL_REG                 0x19
#define BMA2X2_REGA_INT_DATA_SEL_REG                 0x1A
#define BMA2X2_REGA_INT2_PAD_SEL_REG                 0x1B
#define BMA2X2_REGA_INT_SRC_REG                      0x1E
#define BMA2X2_REGA_INT_SET_REG                      0x20
#define BMA2X2_REGA_INT_CTRL_REG                     0x21
#define BMA2X2_REGA_LOW_DURN_REG                     0x22
#define BMA2X2_REGA_LOW_THRES_REG                    0x23
#define BMA2X2_REGA_LOW_HIGH_HYST_REG                0x24
#define BMA2X2_REGA_HIGH_DURN_REG                    0x25
#define BMA2X2_REGA_HIGH_THRES_REG                   0x26
#define BMA2X2_REGA_SLOPE_DURN_REG                   0x27
#define BMA2X2_REGA_SLOPE_THRES_REG                  0x28
#define BMA2X2_REGA_SLO_NO_MOT_THRES_REG             0x29
#define BMA2X2_REGA_TAP_PARAM_REG                    0x2A
#define BMA2X2_REGA_TAP_THRES_REG                    0x2B
#define BMA2X2_REGA_ORIENT_PARAM_REG                 0x2C
#define BMA2X2_REGA_THETA_BLOCK_REG                  0x2D
#define BMA2X2_REGA_THETA_FLAT_REG                   0x2E
#define BMA2X2_REGA_FLAT_HOLD_TIME_REG               0x2F
#define BMA2X2_REGA_FIFO_WML_TRIG                    0x30
#define BMA2X2_REGA_STATUS_LOW_POWER_REG             0x31
#define BMA2X2_REGA_SELF_TEST_REG                    0x32
#define BMA2X2_REGA_EEPROM_CTRL_REG                  0x33
#define BMA2X2_REGA_SERIAL_CTRL_REG                  0x34
#define BMA2X2_REGA_CTRL_UNLOCK_REG                  0x35
#define BMA2X2_REGA_OFFSET_CTRL_REG                  0x36
#define BMA2X2_REGA_OFFSET_PARAMS_REG                0x37
#define BMA2X2_REGA_OFFSET_FILT_X_REG                0x38
#define BMA2X2_REGA_OFFSET_FILT_Y_REG                0x39
#define BMA2X2_REGA_OFFSET_FILT_Z_REG                0x3A
#define BMA2X2_REGA_OFFSET_UNFILT_X_REG              0x3B
#define BMA2X2_REGA_OFFSET_UNFILT_Y_REG              0x3C
#define BMA2X2_REGA_OFFSET_UNFILT_Z_REG              0x3D
#define BMA2X2_REGA_FIFO_MODE_REG                    0x3E
#define BMA2X2_REGA_FIFO_DATA_OUTPUT_REG             0x3F

/** regv
 * */
#define BMA2X2_REGV_SOFT_RESET_VALUE                 0xB6

/*motion*/
#define BMA2X2_REGA_INT_MOTION_DETECT                BMA2X2_REGA_INT_ENABLE1_REG
#define BMA2X2_REGV_ACCEL_ENABLE_SLOPE_XYZ_VALUE     0x07

/** Default values loaded in probe function */
#define BMA2X2_WHOAMI_VALUE_FLAGS                    (0xF8)  /** Who Am I default value */

/** fifo parameters */
/* Max fifo samples 32 frames sample, we limit it to ensure the fifo not overflow during the fifo data handling */
#define BMA2X2_MAX_FIFO            24
#define BMA2X2_FIFO_FRAME_SIZE     6

#define BMA2X2_ODR_BASE_REGV      BMA2X2_REGV_BW_7_81HZ
#define BMA2X2_ODR_BASE_FREQUENCY  15.625
/** Off to idle time */
#define BMA2X2_OFF_TO_IDLE_MS      80  //ms

/** Motion detect configuration */
#define BMA2X2_MD_THRESH          (0.6132f)             // m/s2
#define BMA2X2_MD_ODR_VAL         (26.0)                // 26 Hz
#define BMA2X2_MD_ODR             BMA2X2_REGV_BW_31_25HZ // 26 Hz
// resolution of MD Threshold Register

#define BMA2X2_NUM_AXES           3

#define BMA2X2_ODR_ZERO           0.1

#define BMA2X2_MD_DUR             2
#define BMA2X2_MD_THRESHOLD       16
#define BMA2X2_INT_STATUS_MD_TRIGGERED_MASK 0x04
/**
 * register 0x3e
 */
/** fifo mode */
#define BMA2X2_FIFO_MODE_BYPASS   0
#define BMA2X2_FIFO_MODE_FIFO     1
#define BMA2X2_FIFO_MODE_STREAM   2
/** fifo data */
#define BMA2X2_FIFO_DATA_SELECT_XYZ 0
#define BMA2X2_FIFO_DATA_SELECT_X   1
#define BMA2X2_FIFO_DATA_SELECT_Y   2
#define BMA2X2_FIFO_DATA_SELECT_Z   3

/*!
 * ACC resolution
 */
#define LSB_RANGE_14BITS              16384
#define LSB_RANGE_12BITS              4096
#define LSB_RANGE_10BITS              1024
#define LSB_RANGE_8BITS               256

#define BMA2X2_ACCEL_SENSOR_TEMP_OFFSET_DEG (24.0)

#define BMA2X2_ACTIVE_CURRENT         130 /*uA*/
#define BMA2X2_LOW_POWER_CURRENT      2 /*uA*/

typedef enum {
  BMA2X2_ACCEL_RANGE_IDX_2G,   /*!< +- 2G*/
  BMA2X2_ACCEL_RANGE_IDX_4G,   /*!< +- 4G*/
  BMA2X2_ACCEL_RANGE_IDX_8G,   /*!< +- 8G*/
  BMA2X2_ACCEL_RANGE_IDX_16G,  /*!< +- 16G */
  BMA2X2_ACCEL_RANGE_IDX_MAX,
  BMA2X2_ACCEL_RANGE_IDX_INVALID
} bma2x2_accel_range_idx;


typedef enum {
  BMA2X2_FIFO_TS_OK,
  BMA2X2_FIFO_TS_FIX_LOWER,
  BMA2X2_FIFO_TS_FIX_HIGHER,
} bma2x2_fifo_ts_fix_st;

/******************* Function Declarations ***********************************/

/**
 * Disables FIFO ODR. Also disables ODR for sensors with
 * non-zero ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void bma2x2_stop_fifo_streaming(bma2x2_instance_state *state);

/**
 * Enable FIFO streaming. Also enables FIFO sensors with
 * non-zero desired ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void bma2x2_start_fifo_streaming(bma2x2_instance_state *state);

/**
 * Enables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 * @param[i] sensors       sensor bit mask to enable
 *
 * @return none
 */
void bma2x2_enable_fifo_intr(bma2x2_instance_state *state, uint8_t sensor_enable_flags);

/**
 * Populates Instance state with desired FIFO configuration.
 *
 * @param[i] state                 Instance state
 * @param[i] desired_wmk           desired FIFO WM
 * @param[i] a_chosen_sample_rate  desired Accel ODR
 * @param[i] sensor                bit mask of Sensors to enable
 *
 * @return none
 */
void bma2x2_hal_set_fifo_config(bma2x2_instance_state *state,
                             uint16_t desired_wmk,
                             bma2x2_accel_odr_regv a_chosen_sample_rate,
                             uint8_t sensor_flags);

/**
 * Sets FIFO WM and decimation config registers.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void bma2x2_set_fifo_wml(bma2x2_instance_state *state);

/**
 * Reads status registers in Instance State.
 * This function is for debug only.
 *
 * @param[i] state                 Instance state
 * @param[i] sensor                bit mask of Sensors to enabl
 *
 * @return none
 */
void bma2x2_hal_dump_reg(sns_sensor_instance * const this);

/**
 * Sets Motion Detect config.
 *
 * @param[i] state        Instance state
 * @param[i] enable       true to enable Motion Accel else false
 *
 * @return none
 */
void bma2x2_hal_set_md_config(sns_sensor_instance * const instance, bool enable);

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
void bma2x2_hal_update_md_intr(sns_sensor_instance *const instance,
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
void bma2x2_hal_handle_md_interrupt(sns_sensor_instance *const instance,
                                   sns_time irq_timestamp);

/**
 * Sets Gated Accel config.
 *
 * @param[i] desired_wmk           desired FIFO WM
 * @param[i] a_chosen_sample_rate  desired Accel ODR
 * @param[i] sensor                bit mask of Sensors to enable
 *
 * @return none
 */
void bma2x2_hal_set_gated_accel_config(bma2x2_instance_state *state,
                           uint16_t desired_wmk,
                           bma2x2_accel_odr_regv a_chosen_sample_rate,
                           uint8_t sensor_flags);

/**
 * Gets current Accel ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float bma2x2_hal_get_accel_odr(bma2x2_accel_odr_regv curr_odr);

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time bma2x2_get_sample_interval(bma2x2_accel_odr_regv curr_odr);

/**
 * Processes a fifo buffer and extracts accel samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance     Sensor instance
 * @param[i] first_ts     Timestamp of first sample in fifo
 * @param[i] interval     Sampling interval in time ticks
 * @param[i] fifo         Buffer containing samples read from HW FIFO
 * @param[i] num_bytes    Number of bytes in fifo buffer
 */
void bma2x2_hal_process_fifo_data_buffer(sns_sensor_instance *instance,
                                      sns_time            ts_ff_start,
                                      sns_time            interval,
                                      const uint8_t       *fifo,
                                      size_t              num_bytes)
;

/**
 * Handle an interrupt by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance       Sensor Instance
 */
/**
 * Handle an interrupt by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance       Sensor Instance
 * @param irq_event      irq event pointer
 * @param ctx_type       context type
 */
void bma2x2_hal_handle_fifo_event(sns_sensor_instance *const instance,
    sns_interrupt_event *irq_event,
    bma2x2_fifo_read_ctx_type_t ctx_type);

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void bma2x2_hal_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void bma2x2_hal_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const int8_t       temp_data);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void bma2x2_hal_handle_sensor_temp_sample(sns_sensor_instance *const instance);


/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void bma2x2_start_sensor_temp_polling_timer(sns_sensor_instance *this);


/**
 * reset the sensor
 * @param scp_service    sync COM service
 * @param port_handle    COM port handle
 * @param sensor         sensor handler
 * @return       sns_rc
 * SNS_RC_SUCCESS        on success
 * other value           the others case
 */
sns_rc bma2x2_reset_device(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            bma2x2_sensor_type sensor);

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
void bma2x2_hal_reconfig_hw(sns_sensor_instance * const this);

/**
 * read data from sensor
 * @param scp_service
 * @param port_handle
 * @param rega            register address
 * @param regv_buffer     register value buffer
 * @param bytes           bytes to read
 * @return
 */
sns_rc bma2x2_hal_read_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes);

/**
 * write data to the sensor
 * @param scp_service
 * @param port_handle
 * @param rega
 * @param regv_buffer
 * @param bytes
 * @return
 */
sns_rc bma2x2_hal_write_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes);

sns_rc bma2x2_hal_handle_regv_cfg_timeout_event(bma2x2_instance_state *state);

/**
 * Sends a FIFO complete event.
 *
 * @param[i] instance   Instance reference
 * @param[i] suid       sensor uid
 */
void bma2x2_hal_send_fifo_flush_done(sns_sensor_instance *const instance,
                                  sns_sensor_uid *suid);

/**
 * Updates temperature sensor polling configuration
 *
 * @param[i] instance   Sensor instance
 *
 * @return sampling interval time in ticks
 */
void bma2x2_set_polling_config(sns_sensor_instance *const this);

/**
 * Register interrupt
 * @param this    instance handler
 */
void bma2x2_hal_register_interrupt(sns_sensor_instance *this);

// <fifo>
void bma2x2_hal_fifo_drain(
    sns_sensor_instance *inst,
    bool                    sync_read,
    bma2x2_fifo_flush_trigger_t trigger);
//

bool bma2x2_hal_update_sample_ts(
   sns_sensor_instance * const this,
   uint16_t        ff_count,
   bool            in_irq,
   bool            ff_overrun,
   sns_time ts_curr);

void bma2x2_reset_ts_base(bma2x2_instance_state *istate);

void bma2x2_inst_prepare_odr_changing(
    sns_sensor_instance * const inst,
    bma2x2_accel_odr_regv regv_desired_odr
    );

#if !BMA2X2_ENABLE_FORCE_WAIT
void bma2x2_hal_handle_reg_cfg_timer_event(sns_sensor_instance *const inst);
#endif

#if BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
/**
 * Create heart beat timer
 * @param inst              instance handler
 * @param timeout_ticks     timeout ticks
 */
void bma2x2_hal_create_heart_beat_timer(sns_sensor_instance *inst,
                                        sns_time timeout_ticks);
#endif

//<calibration/registry>
/**
 * Send Calibration event to client
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void bma2x2_send_cal_event(sns_sensor_instance *const inst,
                            bma2x2_sensor_type sensor_type);

/**
 * Reset Calibration values
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void bma2x2_reset_cal_data(sns_sensor_instance *const inst,
      bma2x2_sensor_type sensor_type);
//</calibration/registry>

static inline float bma2x2_convert_us2ticks(uint32_t us)
{
  float ticks;
  uint32_t ticks_in_1ms = sns_convert_ns_to_ticks(1000 * 1000);
  ticks = (us * ticks_in_1ms) * 1e-3;

  return ticks;
}
