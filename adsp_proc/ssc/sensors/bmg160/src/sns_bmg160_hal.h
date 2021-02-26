#pragma once
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

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_gpio_service.h"

#include "sns_bmg160_sensor_instance.h"
#include "sns_bmg160_cfg.h"
#include "float.h"


#define BMG160_DELAY_US_AFTER_WRITTING_REGISTER      450    //450us
#define BMG160_DELAY_MS_AFTER_SOFT_RESET             3      //3ms

#define CONTINUOUS_BAD_SAMPLE_CNT_THRESHOLD          0x10
#define BMG160_U64_MAX         (0xFFFFFFFF)
#define BST_IS_FLOAT_ZERO(f)    (((f) >= -FLT_MIN) && ((f) <= FLT_MIN))


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

// Enable for test code
#ifndef BMG160_ENABLE_TEST_CODE
#define BMG160_ENABLE_TEST_CODE      1
#endif

#define BST_ABS(x)              (((x) > 0) ? (x) : (0-(x)))
#define BST_MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define BST_MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define BST_MAX3(a, b, c)       (BST_MAX(BST_MAX((a), (b)), (c))

/**
 *  Address registers
 */
#define BMG160_REGA_CHIP_ID                     0x00  /**<        Address of Chip ID Register               */
#define BMG160_REGA_REVISION_ID                 0x01  /**<        Address of Revision ID Register           */

/* Data Register */
#define BMG160_REGA_DATAX_LSB                   0x02  /**<        Address of X axis Rate LSB Register       */
#define BMG160_REGA_DATAX_MSB                   0x03  /**<        Address of X axis Rate MSB Register       */
#define BMG160_REGA_DATAY_LSB                   0x04  /**<        Address of Y axis Rate LSB Register       */
#define BMG160_REGA_DATAY_MSB                   0x05  /**<        Address of Y axis Rate MSB Register       */
#define BMG160_REGA_DATAZ_LSB                   0x06  /**<        Address of Z axis Rate LSB Register       */
#define BMG160_REGA_DATAZ_MSB                   0x07  /**<        Address of Z axis Rate MSB Register       */
#define BMG160_REGA_TEMP                        0x08  /**<        Address of Temperature Data LSB Register  */

/* Status Register */
#define BMG160_REGA_INT_STATUS0                 0x09  /**<        Address of Interrupt status Register 0    */
#define BMG160_REGA_INT_STATUS1                 0x0A  /**<        Address of Interrupt status Register 1    */
#define BMG160_REGA_INT_STATUS2                 0x0B  /**<        Address of Interrupt status Register 2    */
#define BMG160_REGA_INT_STATUS3                 0x0C  /**<        Address of Interrupt status Register 3    */
#define BMG160_REGA_FIFO_STATUS                 0x0E  /**<        Address of FIFO status Register           */

/* Control Register */
#define BMG160_REGA_DSP_RANGE_ADDR              0x0F  /**<        Address of DSP Range address Register     */
#define BMG160_REGA_DSP_BW_ADDR                 0x10  /**<        Address of DSP Bandwidth Register         */
#define BMG160_REGA_PMU_LPW0_ADDR               0x11  /**<        Address of PMU LPW0 Register              */
#define BMG160_REGA_PMU_LPW1_ADDR               0x12  /**<        Address of PMU LPW1  Register             */
#define BMG160_REGA_RATED_HBW_ADDR              0x13  /**<        Address of DSP HBW Register               */
#define BMG160_REGA_BA_SOFTRESET_ADDR           0x14  /**<        Address of BA Softreset Register          */
#define BMG160_REGA_INT_ENABLE0                 0x15  /**<        Address of Interrupt Enable 0             */
#define BMG160_REGA_INT_ENABLE1                 0x16  /**<        Address of Interrupt Enable 1             */
#define BMG160_REGA_INT_MAP_0                   0x17  /**<        Address of Interrupt MAP 0                */
#define BMG160_REGA_INT_MAP_1                   0x18  /**<        Address of Interrupt MAP 1                */
#define BMG160_REGA_INT_MAP_2                   0x19  /**<        Address of Interrupt MAP 2                */
#define BMG160_REGA_INT_0_ADDR                  0x1A  /**<        Address of Interrupt 0 Address register   */
#define BMG160_REGA_INT_1_ADDR                  0x1B  /**<        Address of Interrupt 1 Address register   */
#define BMG160_REGA_INT_2_ADDR                  0x1C  /**<        Address of Interrupt 2 Address register   */
#define BMG160_REGA_INT_3_ADDR                  0x1D  /**<        Address of Interrupt 3 Address register   */
#define BMG160_REGA_INT_4_ADDR                  0x1E  /**<        Address of Interrupt 4 Address register   */
#define BMG160_REGA_INT_5_ADDR                  0x1F  /**<        Address of Interrupt 5 Address register   */
#define BMG160_REGA_INT_6_ADDR                  0x20  /**<        Address of Interrupt 6 Address register   */
#define BMG160_REGA_RST_LATCH_ADDR              0x21  /**<        Address of Reset Latch Register           */
#define BMG160_REGA_INT_7_ADDR                  0x22  /**<        Address of Interrupt 7 Address register   */
#define BMG160_REGA_INT_8_ADDR                  0x23  /**<        Address of Interrupt 8 Address register   */
#define BMG160_REGA_INT_9_ADDR                  0x24  /**<        Address of Interrupt 9 Address register   */
#define BMG160_REGA_INT_10_ADDR                 0x25  /**<        Address of Interrupt 10 Address register  */
#define BMG160_REGA_INT_11_ADDR                 0x26  /**<        Address of Interrupt 11 Address register  */
#define BMG160_REGA_INT_12_ADDR                 0x27  /**<        Address of Interrupt 12 Address register  */
#define BMG160_REGA_INT_13_ADDR                 0x28  /**<        Address of Interrupt 13 Address register  */
#define BMG160_REGA_INT_14_ADDR                 0x29  /**<        Address of Interrupt 14 Address register  */
#define BMG160_REGA_INT_15_ADDR                 0x2A  /**<        Address of Interrupt 15 Address register  */
#define BMG160_REGA_INT_16_ADDR                 0x2B  /**<        Address of Interrupt 16 Address register  */
#define BMG160_REGA_INT_17_ADDR                 0x2C  /**<        Address of Interrupt 17 Address register  */
#define BMG160_REGA_INT_19_ADDR                 0x2D  /**<        Address of Interrupt 19 Address register  */
#define BMG160_REGA_INT_20_ADDR                 0x2E  /**<        Address of Interrupt 20 Address register  */
#define BMG160_REGA_INT_21_ADDR                 0x2F  /**<        Address of Interrupt 21 Address register  */
#define BMG160_REGA_INT_22_ADDR                 0x30  /**<        Address of Interrupt 22 Address register  */
#define BMG160_REGA_INT_OFF0_ADDR               0x31  /**<        Address of Interrupt OFF0 register        */
#define BMG160_REGA_INT_OFF1_ADDR               0x32  /**<        Address of Interrupt OFF1 Register        */
#define BMG160_REGA_TRIM_NVM_CTRL_ADDR          0x33  /**<        Address of Trim NVM control register      */
#define BMG160_REGA_BA_SPI3_WDT_ADDR            0x34  /**<        Address of BA SPI3,WDT Register           */
#define BMG160_REGA_INL_OFFSET_MSB_ADDR         0x36  /**<        Address of INL Offset MSB Register        */

/* Trim Register */
#define BMG160_REGA_INL_OFFSET_X                0x37  /**<        Address of INL Offset X Register          */
#define BMG160_REGA_INL_OFFSET_Y                0x38  /**<        Address of INL Offset Y Register          */
#define BMG160_REGA_INL_OFFSET_Z                0x39  /**<        Address of INL Offset Z Register          */
#define BMG160_REGA_TRIM_GP0_ADDR               0x3A  /**<        Address of Trim GP0 Register              */
#define BMG160_REGA_TRIM_GP1_ADDR               0x3B  /**<        Address of Trim GP1 Register              */
#define BMG160_REGA_FPGA_REV_ADDR               0x3C  /**<        Address of FPGA Rev Register              */

/* Control Register */
#define BMG160_REGA_FIFO_CGF1_ADDR              0x3D  /**<        Address of FIFO CGF0 Register             */
#define BMG160_REGA_FIFO_CGF0_ADDR              0x3E  /**<        Address of FIFO CGF1 Register             */

/* Data Register */
#define BMG160_REGA_FIFO_DATA_ADDR              0x3F  /**<        Address of FIFO Data Register             */

/** regv
*/
#define BMG160_WHOAMI_VALUE                     (0x0F)
#define BMG160_REGV_SOFT_RESET_VALUE            (0xB6)

/*motion*/
#define BMG160_REGA_INT_MOTION_DETECT           BMG160_REGA_INT_ENABLE1_REG
#define BMG160_REGV_GYRO_ENABLE_SLOPE_XYZ_VALUE 0x07

/** fifo parameters */
#define BMG160_CONFIG_TS_ITVL_EST_COEF          8
#define BMG160_ODR_BASE_FREQUENCY               (15.625f)
/** Off to idle time */
#define BMG160_OFF_TO_IDLE_MS                   (60)  //ms

/** Motion detect configuration */
#define BMG160_NUM_AXES                         3
#define BMG160_ODR_ZERO                         (0.1f)

/**
 * register 0x3e
 */
/** fifo mode */
#define BMG160_FIFO_MODE_BYPASS          0
#define BMG160_FIFO_MODE_FIFO            1
#define BMG160_FIFO_MODE_STREAM          2
/** fifo data */
#define BMG160_FIFO_DATA_SELECT_XYZ      0
#define BMG160_FIFO_DATA_SELECT_X        1
#define BMG160_FIFO_DATA_SELECT_Y        2
#define BMG160_FIFO_DATA_SELECT_Z        3

#define BMG160_FIFO_FRAME_SIZE           6
#define BMG160_FIFO_ASYNC_READ_HEADROOM  0
#define BMG160_FRAME_SIZE                6

/*!
 * ACC resolution
 */
#define LSB_RANGE_14BITS              16384
#define LSB_RANGE_12BITS              4096
#define LSB_RANGE_10BITS              1024
#define LSB_RANGE_8BITS               256

#define BMG160_GYRO_SENSOR_TEMP_OFFSET_DEG (24.0f)

#define BMG160_ACTIVE_CURRENT         250 /* 500uA*/
#define BMG160_LOW_POWER_CURRENT      5    /*uA*/
#define BMG160_HEART_BEAT_ODR_COUNT   3



#define BMG160_GYRO_RANGE_2000_MAX      (2000)
#define BMG160_GYRO_RANGE_2000_MIN      (-2000)
#define BMG160_GYRO_RANGE_1000_MAX      (1000)
#define BMG160_GYRO_RANGE_1000_MIN      (-1000)
#define BMG160_GYRO_RANGE_500_MAX       (500)
#define BMG160_GYRO_RANGE_500_MIN       (-500)
#define BMG160_GYRO_RANGE_250_MAX       (250)
#define BMG160_GYRO_RANGE_250_MIN       (-250)
#define BMG160_GYRO_RANGE_125_MAX       (125)
#define BMG160_GYRO_RANGE_125_MIN       (-125)

#define BMG160_RANGE_BASE_MIN           (-125)
#define BMG160_RANGE_BASE_MAX           (125)



#define BMG160_GYRO_ODR_100HZ                 (100.0f)
#define BMG160_GYRO_ODR_200HZ                 (200.0f)
#define BMG160_GYRO_ODR_400HZ                 (400.0f)
#define BMG160_GYRO_ODR_1000HZ                (1000.0f)
#define BMG160_GYRO_ODR_2000HZ                (2000.0f)

#define BMG160_ODR_100HZ                      (100)
#define BMG160_ODR_200HZ                      (200)
#define BMG160_ODR_400HZ                      (400)
#define BMG160_ODR_1000HZ                     (1000)
#define BMG160_ODR_2000HZ                     (2000)

#define BMG160_MIN_ODR                        BMG160_GYRO_ODR_100HZ
#if  BMG160_ENABLE_HIGH_ODR
#define BMG160_MAX_ODR                        BMG160_GYRO_ODR_2000HZ
#else
#define BMG160_MAX_ODR                        BMG160_GYRO_ODR_1000HZ
#endif

#define BMG160_ODR_2000HZ_UNFILTERED_REGV     0
#define BMG160_ODR_2000HZ_REGV                1
#define BMG160_ODR_1000HZ_REGV                2
#define BMG160_ODR_400HZ_REGV                 3
#define BMG160_ODR_200HZ_DECF10_FB23_REGV     4
#define BMG160_ODR_100HZ_DECF20_FB12_REGV     5
#define BMG160_ODR_200HZ_DECF10_FB64_REGV     6
#define BMG160_ODR_100HZ_DECF20_FB32_REGV     7

#define BMG160_REGV_RANGE_2000                0
#define BMG160_REGV_RANGE_1000                1
#define BMG160_REGV_RANGE_500                 2
#define BMG160_REGV_RANGE_250                 3
#define BMG160_REGV_RANGE_125                 4
#define BMG160_REGV_RANGE_MAX                 4

/**
 * BMG160 sensitivity for gyro in (mrad/sec)/LSB restricted by range
 */
#define BMG160_SSTVT_125DPS    (3.814755f * PI / 180)
#define BMG160_SSTVT_250DPS    (7.62951f  * PI / 180)
#define BMG160_SSTVT_500DPS    (15.25902f * PI / 180)
#define BMG160_SSTVT_1000DPS   (30.51804f * PI / 180)
#define BMG160_SSTVT_2000DPS   (61.03609f * PI / 180)

#define BMG160_RESOLUTION_BASE BMG160_SSTVT_125DPS

#define BMG160_MAX_FIFO_LEVEL    60  /* the physical max fifo level is 100, now we reserve some space to avoid overrun happen */
#define FIFO_INTERRUPT           0x10

// FIFO algo
#define BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_OVERLAP    15
#define BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_SYNC_ERROR 15
#define BMG160_FIFO_TS_DEVIATE_COUNTER_FIX_THSHOLD    4
#define BMG160_FIFO_TS_SYNC_ERROR_THRESHOLD           2
#define BMG160_FIFO_TS_OVERLAP_ERROR_THRESHOLD        2

#define BMG160_FIFO_MAX_LEN_READ_SYNC                4
#define BMG160_FIFO_TS_ISR_CNT_PREPRECISE            5
#define BMG160_FIFO_TS_ISR_PRECISE_MOST_WEIGHT       95
#define BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC           17




/******************* Function Declarations ***********************************/

typedef struct bst_odr_conf_pair {
  uint16_t  odr;
  uint8_t   regv;
  float     f_odr;
} bst_odr_conf_pair;
/******************* Function Declarations ***********************************/

typedef enum
{
  BMG160_PS_NORMAL,
  BMG160_PS_SUSPEND,
  BMG160_PS_DEEP_SUSPEND,
  BMG160_PS_INVALID
} bmg160_pmu_state;

/**
 * Disables FIFO ODR. Also disables ODR for sensors with
 * non-zero ODR.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
void bmg160_stop_data_streaming(bmg160_instance_state *state);

/**
 * Sets FIFO WM and decimation config registers.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
sns_rc bmg160_set_fifo_wml(bmg160_instance_state *state);

/**
 * Enable FIFO streaming. Also enables FIFO sensors with
 * non-zero desired ODR.
 *
 * @param[i] state         Instance state
 *
 * @return  SNS_RC_SUCCESS success
 *          others value   failure
 */
sns_rc bmg160_start_fifo_streaming(bmg160_instance_state *state);

/**
 * Enables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 * @param[i] sensors       sensor bit mask to enable
 *
 * @return none
 */
void bmg160_enable_data_intr(bmg160_instance_state *state, uint8_t sensor_enable_flags);

/**
 * Disables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 *
 * @return none
 */
sns_rc bmg160_disable_fifo_intr(bmg160_instance_state *state);

/**
 * Sets Gyro ODR, range, BW and sensitivity.
 *
 * @param[i] scp_service     handle to synch COM port service
 * @param[i] port_handle     handle to synch COM port
 * @param[i] curr_odr        Gyro  ODR
 * @param[i] resolution      Gyro  resolution
 * @param[i] range           Gyro range
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc bmg160_set_gyro_odr_config(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                bmg160_gyro_odr_regv      curr_odr,
                                bmg160_gyro_resolution    resolution ,
                                bmg160_gyro_range         range);

/**
 * Populates Instance state with desired FIFO configuration.
 *
 * @param[i] state                 Instance state
 * @param[i] desired_wmk           desired FIFO WM
 * @param[i] a_chosen_sample_rate  desired Gyro ODR
 * @param[i] sensor                bit mask of Sensors to enable
 *
 * @return none
 */
void bmg160_set_fifo_config(bmg160_instance_state *state,
                             uint16_t desired_wmk,
                             bmg160_gyro_odr_regv a_chosen_sample_rate,
                             uint8_t sensor_flags);

/**
 * Reads status registers in Instance State.
 * This function is for debug only.
 *
 * @param[i] state                 Instance state
 *
 * @return none
 */
void bmg160_dump_reg(sns_sensor_instance * const this);

/**
 * Gets current Gyro ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 */
float bmg160_get_gyro_odr(bmg160_gyro_odr_regv curr_odr);

/**
 * Provides sample interval based on current ODR.
 *
 * @param[i] curr_odr              Current FIFO ODR.
 *
 * @return sampling interval time in ticks
 */
sns_time bmg160_get_sample_interval(bmg160_gyro_odr_regv curr_odr);

/**
 * Processes a fifo buffer and extracts Gyro samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance     Sensor instance
 * @param[i] ts_ff_start  Timestamp of first sample in fifo
 * @param[i] ts_ff_itvl   Sampling interval in time ticks
 * @param[i] fifo         Buffer containing samples read from HW FIFO
 * @param[i] num_bytes    Number of bytes in fifo buffer
 */
void bmg160_process_fifo_data_buffer(sns_sensor_instance *instance,
                                     sns_time            ts_ff_start,
                                     sns_time            ts_ff_itvl,
                                      const uint8_t       *fifo,
                                      size_t              num_bytes);

/**
 * Handle an interrupt by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance       Sensor Instance
 * @param irq_event
 * @param ctx_type
 */
void bmg160_handle_interrupt_event(sns_sensor_instance *const instance,
      sns_interrupt_event *irq_event,
      bmg160_fifo_read_ctx_type_t ctx_type
      );

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void bmg160_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void bmg160_convert_and_send_temp_sample(
  sns_sensor_instance *const instance,
  sns_time            timestamp,
  const int8_t       temp_data);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 * @param[i] timeout_time timestamp in ticks
 */
void bmg160_handle_sensor_temp_sample(sns_sensor_instance *const instance,
                                      uint64_t timeout_ticks);

/**
 * Sends a FIFO complete event.
 *
 * @param[i] instance   Instance reference 
 * @param[i] suid       sensor uid 
 */
void bmg160_send_fifo_flush_done(sns_sensor_instance *const instance, sns_sensor_uid *suid);

/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void bmg160_start_sensor_temp_polling_timer(sns_sensor_instance *this);

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
void bmg160_hal_reconfig_hw(sns_sensor_instance *this);

/**
 * read data from sensor
 * @param scp_service
 * @param port_handle
 * @param rega
 * @param regv_buffer
 * @param bytes
 * @return
 */
sns_rc sns_see_bmg160_read_data(sns_sync_com_port_service *scp_service,
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
sns_rc sns_see_bmg160_write_data(sns_sync_com_port_service *scp_service,
                                sns_sync_com_port_handle *port_handle,
                                uint8_t rega, uint8_t* regv_buffer, uint32_t bytes);

sns_rc bmg160_handle_regv_cfg_timeout_event(bmg160_instance_state *state);

sns_rc bmg160_reset_device(sns_sync_com_port_service *scp_service,
                            sns_sync_com_port_handle *port_handle,
                            bmg160_sensor_type sensor);

void bmg160_hal_register_interrupt(sns_sensor_instance * const this);

// request configure
/**
 * Send Calibration event to client
 *
 * @param[i] instance        instance reference
 * @param[i] sensor          bit mask for sensors to handle
 * @return none
 */
void bmg160_send_cal_event(sns_sensor_instance *const inst,
                            bmg160_sensor_type sensor_type);
//


// fifo
void bmg160_hal_fifo_drain(
    sns_sensor_instance *const instance,
        bool                    sync_read,
        bmg160_fifo_flush_trigger_t trigger);

#if BMG160_CONFIG_ENABLE_FIFO
bool bmg160_hal_update_sample_ts(
   sns_sensor_instance * const this,
   uint16_t        ff_count,
   bool            in_irq,
   bool            ff_overrun,
   sns_time ts_curr);
#endif

static inline float bmg160_convert_us2ticks(uint32_t us)
{
  float ticks;
  uint32_t ticks_in_1ms = sns_convert_ns_to_ticks(1000 * 1000);
  ticks = (us * ticks_in_1ms) * 1e-3;

  return ticks;
}
