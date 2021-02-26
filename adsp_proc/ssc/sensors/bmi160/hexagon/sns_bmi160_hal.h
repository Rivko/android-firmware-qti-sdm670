/*******************************************************************************
 * Copyright (c) 2017-18, Bosch Sensortec GmbH
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

/**
 * @file sns_bmi160_hal.h
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#pragma once

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_gpio_service.h"

#include "sns_bmi160_sensor_instance.h"
#include "sns_bmi160_trace.h"
#include "sns_bmi160_config.h"




extern bmi160_instance_state *bmi160_inst_singleton;




#define BMI160_SPEC_STARTUP_TIME_SBUS_US        3800


#define BMI160_SPEC_STARTUP_TIME_MS             10
#define BMI160_SPEC_IF_IDLE_TIME_NORMAL_US      2
#define BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US     450
#define BMI160_SPEC_IF_SPI_SWITCH_TIME_US       200



#define BMI160_SPEC_SOFT_RESET_DELAY_TIME_MS    10

#define BMI160_SPEC_ACC_STARTUP_TIME_US         3800  //2.5ms in datasheet
#define BMI160_SPEC_GYR_STARTUP_TIME_US         80000
#define BMI160_SPEC_MAG_STARTUP_TIME_US         500

#define BMI160_SPEC_ACC_SELF_TEST_WAIT_TIME_MS  50
#define BMI160_SPEC_GYR_SELF_TEST_WAIT_TIME_MS  50

#define BMI160_SPEC_SENSORTIME_RES_US           39.0
#define BMI160_SPEC_TS_LSB_OVERFLOW_VAL         0x1000000



#define BMI160_REGA_USR_CHIP_ID         0x00
#define BMI160_REGA_USR_ERR_REG         0x02
#define BMI160_REGA_USR_PMU_STATUS      0x03
#define BMI160_REGA_USR_DATA_0          0x04
#define BMI160_REGA_USR_DATA_7          0x0b
#define BMI160_REGA_USR_DATA_8          0x0c
#define BMI160_REGA_USR_DATA_13         0x11
#define BMI160_REGA_USR_DATA_14         0x12
#define BMI160_REGA_USR_DATA_19         0x17
#define BMI160_REGA_USR_SENSORTIME_0    0x18
#define BMI160_REGA_USR_SENSORTIME_2    0x1a

#define BMI160_REGA_USR_STATUS          0x1b

#define BMI160_REGA_USR_INT_STATUS_0    0x1c
#define BMI160_REGA_USR_INT_STATUS_1    0x1d
#define BMI160_REGA_USR_INT_STATUS_2    0x1e
#define BMI160_REGA_USR_INT_STATUS_3    0x1f

#define BMI160_REGA_USR_TEMPERATURE_0   0x20
#define BMI160_REGA_USR_TEMPERATURE_1   0x21

#define BMI160_REGA_USR_FIFO_LENGTH_0   0x22
#define BMI160_REGA_USR_FIFO_LENGTH_1   0x23

#define BMI160_REGA_USR_FIFO_DATA       0x24

#define BMI160_REGA_USR_ACC_CONF        0x40
#define BMI160_REGA_USR_ACC_RANGE       0x41
#define BMI160_REGA_USR_GYR_CONF        0x42
#define BMI160_REGA_USR_GYR_RANGE       0x43
#define BMI160_REGA_USR_MAG_CONF        0x44

#define BMI160_REGA_USR_FIFO_CONFIG_0   0x46
#define BMI160_REGA_USR_FIFO_CONFIG_1   0x47

#define BMI160_REGA_USR_MAG_IF_0        0x4b
#define BMI160_REGA_USR_MAG_IF_1        0x4c
#define BMI160_REGA_USR_MAG_IF_2        0x4d
#define BMI160_REGA_USR_MAG_IF_3        0x4e
#define BMI160_REGA_USR_MAG_IF_4        0x4f


#define BMI160_REGA_USR_INT_EN_0        0x50
#define BMI160_REGA_USR_INT_EN_1        0x51
#define BMI160_REGA_USR_INT_EN_2        0x52

#define BMI160_REGA_USR_INT_OUT_CTRL    0x53
#define BMI160_REGA_USR_INT_LATCH       0x54

#define BMI160_REGA_USR_INT_MAP_0       0x55
#define BMI160_REGA_USR_INT_MAP_1       0x56
#define BMI160_REGA_USR_INT_MAP_2       0x57


#define BMI160_REGA_USR_INT_MOTION_0    0x5f
#define BMI160_REGA_USR_INT_MOTION_1    0x60


#define BMI160_REGA_USR_IF_CONF         0x6b
#define BMI160_REGA_USR_PMU_TRIGGER     0x6c

#define BMI160_REGA_USR_SELF_TEST       0x6d

#define BMI160_REGA_USR_INT_TAP_0       0x63
#define BMI160_REGA_USR_INT_TAP_1       0x64

#define BMI160_REGA_USR_PEDO_STEP_0     0x78
#define BMI160_REGA_USR_STEP_CONF_0     0x7a
#define BMI160_REGA_USR_STEP_CONF_1     0x7b


#define BMI160_REGA_CMD_CMD             0x7e
#define BMI160_REGA_CMD_EXT_MODE        0x7f

#define BMI160_INT_STATUS_0_ANYM_INT    (1<<2)
#define BMI160_INT_STATUS_0_D_TAP_INT   (1<<4)

#define BMI160_REGV_CHIP_ID_MAJOR       0xd0

#define BMI160_REGV_CMD_NOOP            0x00
#define BMI160_REGV_CMD_ACC_MODE_SUSP   0x10
#define BMI160_REGV_CMD_ACC_MODE_NORMAL 0x11
#define BMI160_REGV_CMD_ACC_MODE_LP     0x12

#define BMI160_REGV_CMD_GYR_MODE_SUSP   0x14
#define BMI160_REGV_CMD_GYR_MODE_NORMAL 0x15
#define BMI160_REGV_CMD_GYR_MODE_FSUP   0x17

#define BMI160_REGV_CMD_MAG_MODE_SUSP   0x18
#define BMI160_REGV_CMD_MAG_MODE_NORMAL 0x19
#define BMI160_REGV_CMD_MAG_MODE_LP2    0x1b

#define BMI160_REGV_CMD_FIFO_FLUSH      0xb0
#define BMI160_REGV_CMD_INT_RESET       0xb1
#define BMI160_REGV_CMD_SOFT_RESET      0xb6

#define BMI160_REGV_CMD_TGT_PAGE_USR   0
#define BMI160_REGV_CMD_TGT_PAGE_COM   1
#define BMI160_REGV_CMD_TGT_PAGE_ACC   2
#define BMI160_REGV_CMD_TGT_PAGE_GYR   3


#define BMI160_REGV_CMD_EXT_MODE_EN_B0  0x37
#define BMI160_REGV_CMD_EXT_MODE_EN_B1  0x9a
#define BMI160_REGV_CMD_EXT_MODE_EN_B2  0xc0



#define BMI160_REGV_ODR_RESERVED       0x00
#define BMI160_REGV_ODR_0_78HZ         0x01
#define BMI160_REGV_ODR_1_56HZ         0x02
#define BMI160_REGV_ODR_3_12HZ         0x03
#define BMI160_REGV_ODR_6_25HZ         0x04
#define BMI160_REGV_ODR_12_5HZ         0x05
#define BMI160_REGV_ODR_25HZ           0x06
#define BMI160_REGV_ODR_50HZ           0x07
#define BMI160_REGV_ODR_100HZ          0x08
#define BMI160_REGV_ODR_200HZ          0x09
#define BMI160_REGV_ODR_400HZ          0x0A
#define BMI160_REGV_ODR_800HZ          0x0B
#define BMI160_REGV_ODR_1600HZ         0x0C
#define BMI160_REGV_ODR_RESERVED0      0x0D
#define BMI160_REGV_ODR_RESERVED1      0x0E
#define BMI160_REGV_ODR_RESERVED2      0x0F

#define BMI160_REGV_ACC_RANGE_PM2G       3
#define BMI160_REGV_ACC_RANGE_PM4G       5
#define BMI160_REGV_ACC_RANGE_PM8G       8
#define BMI160_REGV_ACC_RANGE_PM16G      12


#define BMI160_REGV_GYR_RANGE_PM2000DPS         0
#define BMI160_REGV_GYR_RANGE_PM1000DPS         1
#define BMI160_REGV_GYR_RANGE_PM500DPS          2
#define BMI160_REGV_GYR_RANGE_PM250DPS          3
#define BMI160_REGV_GYR_RANGE_PM125DPS          4

#define BMI160_REGV_PMU_STAT_ACC_SUSPEND        0
#define BMI160_REGV_PMU_STAT_ACC_NORMAL         1
#define BMI160_REGV_PMU_STAT_ACC_LP1            2
#define BMI160_REGV_PMU_STAT_ACC_LP2            3

#define BMI160_REGV_PMU_STAT_GYR_SUSPEND        0
#define BMI160_REGV_PMU_STAT_GYR_NORMAL         1
#define BMI160_REGV_PMU_STAT_GYR_RESERVED       2
#define BMI160_REGV_PMU_STAT_GYR_FAST_STARTUP   3

#define BMI160_REGV_PMU_STAT_MAG_SUSPEND        0
#define BMI160_REGV_PMU_STAT_MAG_NORMAL         1
#define BMI160_REGV_PMU_STAT_MAG_LP1            2
#define BMI160_REGV_PMU_STAT_MAG_LP2            3

#define BMI160_REGV_STEP_CONF_0_NORMAL          0x15
#define BMI160_REGV_STEP_CONF_1_NORMAL          0x03

#define BMI160_REGV_STEP_CONF_0_SENSITIVE       0x2d
#define BMI160_REGV_STEP_CONF_1_SENSITIVE       0x00

#define BMI160_REGV_STEP_CONF_0_ROBUST          0x1d
#define BMI160_REGV_STEP_CONF_1_ROBUST          0x07


#define BMI160_REGV_INT_LATCH_NONE              0
#define BMI160_REGV_INT_LATCH_313US             1
#define BMI160_REGV_INT_LATCH_625US             2
#define BMI160_REGV_INT_LATCH_1P25MS            3
#define BMI160_REGV_INT_LATCH_2P5MS             4
#define BMI160_REGV_INT_LATCH_5MS               5
#define BMI160_REGV_INT_LATCH_10MS              6
#define BMI160_REGV_INT_LATCH_20MS              7
#define BMI160_REGV_INT_LATCH_40MS              8
#define BMI160_REGV_INT_LATCH_80MS              9
#define BMI160_REGV_INT_LATCH_160MS             10
#define BMI160_REGV_INT_LATCH_320MS             11
#define BMI160_REGV_INT_LATCH_640MS             12
#define BMI160_REGV_INT_LATCH_1280S             13
#define BMI160_REGV_INT_LATCH_2560S             14
#define BMI160_REGV_INT_LATCH_PERM              15


#define B0_SET  1
#define B1_SET  (1 << 1)
#define B2_SET  (1 << 2)
#define B3_SET  (1 << 3)
#define B4_SET  (1 << 4)
#define B5_SET  (1 << 5)
#define B6_SET  (1 << 6)
#define B7_SET  (1 << 7)


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

#define BST_CEIL_P(f) ((f + 0.5f))
#define BST_CEIL_P_BUF(f, buf) ((f + buf))
//#define BST_IS_FLOAT_ZERO(f)    (((f) >= 0) && ((f) <= 0))
#define BST_IS_FLOAT_ZERO(f)    (0 == (int)(1e3 * (f)))

#define BMS_SCALE_S2US          1000000

#define BST_ARRAY_SIZE(array)   (sizeof(array)/sizeof(array[0]))
#define BST_MIN(a, b)           (((a) < (b)) ? (a) : (b))
#define BST_MAX(a, b)           (((a) > (b)) ? (a) : (b))
#define BST_MAX3(a, b, c)       (BST_MAX(BST_MAX((a), (b)), (c)))

#define BST_ABS(x)              (((x) > 0) ? (x) : (0-(x)))

#define BST_IGNORE_WARNING_UNUSED_VAR(v)    ((void)v)

#define BST_ASSUME_TS_IS_64BIT

#ifndef likely
#define likely(x) x
#endif

#ifndef BMI160_DD_CHECK_RETVAL
#define BMI160_DD_CHECK_RETVAL(val, val_success)\
    if (likely(val_success == val)) {       \
    } else {                                \
        val = val;                      \
        return val;                     \
    }
#endif


struct bst_sbus_spec {
    uint32_t clk_rate;

    uint32_t type       :2;
    uint32_t start_delay_us :30;
};


#define BMI160_FF_DEPTH_BYTES      1024


#define BMI160_FF_FRAME_LEN_TS  4
#define BMI160_FF_DATA_LEN_ACC  6
#define BMI160_FF_DATA_LEN_GYR  6
#define BMI160_FF_DATA_LEN_IMU  6
#define BMI160_FF_DATA_LEN_MAG  8

#define BMI160_FF_FH_MODE_CTRL                  0x01
#define BMI160_FF_FH_MODE_REGULAR               0x02
#define BMI160_FF_FH_EMPTY                      0x80


#define BMI160_FF_FH_MODE(frame_header_byte)    BST_GET_VAL_BITBLOCK(frame_header_byte, 6, 7)
#define BMI160_FF_FH_PARAM(frame_header_byte)   BST_GET_VAL_BITBLOCK(frame_header_byte, 2, 5)
#define BMI160_FF_FH_EXT(frame_header_byte)     BST_GET_VAL_BITBLOCK(frame_header_byte, 0, 1)



struct bmi160_odr_regv_map {
    float       odr;
    uint8_t     regv;
};





/** fifo paramters */
#define BMI160_FF_MAX_FRAMES_IMU            (BMI160_FF_DEPTH_BYTES / (BMI160_FF_DATA_LEN_IMU + 1))
#define BMI160_FF_MAX_FRAMES_MAG            (BMI160_FF_DEPTH_BYTES / (BMI160_FF_DATA_LEN_MAG + 1))

/** Off to idle time */
#define BMI160_OFF_TO_IDLE_MS      80  //ms



typedef union bmi160_status_reg {
    struct {
        uint8_t por_detected            :1;
        uint8_t gyr_self_test_ok        :1;
        uint8_t mag_man_op              :1;
        uint8_t foc_rdy                 :1;
        uint8_t nvm_rdy                 :1;
        uint8_t drdy_mag                :1;
        uint8_t drdy_gyr                :1;
        uint8_t drdy_acc                :1;
    } bits;
    uint8_t reg;
} bmi160_status_reg_t;

struct bmi160_sensor_data_raw_buf {
    uint8_t buf_a[6];

    uint8_t buf_g[6];

    uint8_t buf_m[8];

    uint8_t buf_ts[3];

    uint8_t buf_temp[2];

    uint8_t _reserved[2];


    uint8_t avail_ts   :1;
};

typedef union bmi160_sensor_data_req {
    struct {
        uint32_t m:  1;
        uint32_t g:  1;
        uint32_t a:  1;
        uint32_t ts: 1;
        uint32_t t:  1;
        uint32_t p:  1;

    } bits;

    uint32_t req;
} bmi160_sensor_data_req_t;

union bmi160_fifo_config_1 {
    struct {
        uint8_t fifo_stop_on_full:      1;
        uint8_t fifo_time_en:           1;
        uint8_t fifo_tag_int2_en:      1;
        uint8_t fifo_tag_int1_en:      1;
        uint8_t fifo_header_en:         1;
        uint8_t fifo_mag_en:            1;
        uint8_t fifo_acc_en:            1;
        uint8_t fifo_gyr_en:            1;
    } bits;

    uint8_t reg;
};


enum {
    BMI160_FF_FRAME_ERR_NONE = 0,
    BMI160_FF_FRAME_ERR_UNKNOWN,
    BMI160_FF_FRAME_ERR_PARTIAL_READ,
};

typedef enum BMI160_CMD_TYPE {
    BMI160_CMD_TYPE_ACC,
    BMI160_CMD_TYPE_GYR,
    BMI160_CMD_TYPE_MAG,
    BMI160_CMD_TYPE_MISC,
    BMI160_CMD_TYPE_UNKNOWN,
} bmi160_cmd_type_t;

typedef union bmi160_fifo_parse_result {
    struct {
        //NU
        uint32_t ff_err                 :1;
        //NU
        uint32_t ff_err_code            :3;

        uint32_t ff_overflow            :1;

        uint32_t ff_cache_full_agm      :3;


        uint32_t ff_avail_ts            :1;
        uint32_t ff_avail_ts_header     :1;
        uint32_t ff_avail_end_frame     :1;

        uint32_t offset_ts_header       :10;    //bmi26x
        uint32_t ff_batch_empty         :1; //empty fifo batch

    } bits;

    uint32_t rslt;
} bmi160_fifo_parse_result_t;


typedef struct bmi160_fifo_parse_out_desc {
    bmi160_fifo_parse_result_t  ff_parse_rslt;

    uint32_t                    ts_dev_ff               :24;

    uint32_t                    ff_frm_header_at_irq    :3;
    uint32_t                    ff_frm_header_before_ts :3;

    //in this batch only
    uint32_t                    fc_masters_this_batch   :8;

    uint32_t                    fc_at_irq_acc           :8;
    uint32_t                    fc_at_irq_gyr           :8;
    uint32_t                    fc_at_irq_mag           :8;
    uint32_t                    fc_at_irq_masters       :8;

    uint32_t                    fc_this_batch_acc       :8;
    uint32_t                    fc_this_batch_gyr       :8;
    uint32_t                    fc_this_batch_mag       :8;

    uint32_t                    fc_masters_mark_acc     :8;
    uint32_t                    fc_masters_mark_gyr     :8;
    uint32_t                    fc_masters_mark_mag     :8;



} bmi160_fifo_parse_out_desc_t;

typedef struct bmi160_fifo_parse_ctx {
    const uint8_t               *ff_buf;
    uint16_t                    ff_buf_len;

    //offset of member @ff_buf relative to the whole buffer read out in this batch
    //uint16_t                    offset_buffer;

    uint32_t                    ff_parse_flag;

    uint32_t                    ff_proc_len_left;


    void                        *priv_data;
} bmi160_fifo_parse_ctx_t;

#define BMI160_FIFO_PARSE_FLAG_DRYRUN   (1)

typedef union bmi160_fifo_pkg_stat {
    struct {
        //NU
        uint32_t ff_err         :1;
        //NU
        uint32_t ff_err_code    :3;

        uint32_t ff_overflow    :1;

        uint32_t ff_rdy_acc     :1;
        uint32_t ff_rdy_gyr     :1;
        uint32_t ff_rdy_mag     :1;
    } bits;

    uint32_t stat;
} bmi160_fifo_pkg_stat_t;

typedef union bmi160_hw_err_stat {
    /*! bmi160 sensor error status */
    struct {
        uint8_t fatal_err:      1;
        uint8_t err_code:       4;
        uint8_t i2c_fail_err:   1;
        uint8_t drop_cmd_err:   1;
        uint8_t mag_drdy_err:   1;
    } bits;

    uint8_t         regv;
} bmi160_hw_err_stat_t;




/******************* Function Declarations ***********************************/

/**
 * Resets the Sensor HW. Also calls
 * bmi160_device_set_default_state()
 *
 * @param[i] scp_service   handle to synch COM port service
 * @param[i] port_handle   handle to synch COM port
 * @param[i] sensor        bit mask for sensors to reset
 *
 * @return sns_rc
 * SNS_RC_FAILED - COM port failure
 * SNS_RC_SUCCESS
 */
sns_rc bmi160_hal_reset_device(
        sns_sensor_instance     *this,
        bmi160_sensor_type      sensor);

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
sns_rc bmi160_device_set_default_state(sns_sync_com_port_service *scp_service,
        sns_sync_com_port_handle *port_handle,
        bmi160_sensor_type sensor);

/**
 * Enables interrupt for FIFO sensors.
 *
 * @param[i] state         Instance state
 * @param[i] sensors       sensor bit mask to enable
 *
 * @return none
 */
void bmi160_hal_enable_fifo_intr(bmi160_instance_state *state);

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
sns_rc bmi160_hal_get_who_am_i(sns_sync_com_port_service *scp_service,
        sns_sync_com_port_handle  *port_handle,
        uint8_t                   *buffer);

sns_rc bmi160_hal_switch_2_spi(sns_sync_com_port_service *scp_service,
        sns_sync_com_port_handle  *port_handle);

/**
 * Reads status registers in Instance State.
 * This function is for debug only.
 *
 * @param[i] state                 Instance state
 * @param[i] sensor                bit mask of Sensors to enabl
 *
 * @return none
 */
void bmi160_hal_dump_reg(sns_sensor_instance *this);

/**
 * Sets Motion Detect config.
 *
 * @param[i] state        Instance state
 * @param[i] enable       true to enable Motion Accel else false
 *
 * @return none
 */
sns_rc bmi160_hal_set_md_config(sns_sensor_instance *const instance, bool enable);

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


sns_rc bmi160_hal_config_int_output(
        sns_sensor_instance     *const  instance,
        bool                    enable);

sns_rc bmi160_hal_config_int_md(sns_sensor_instance *const instance,
        bool enable,
        bool md_not_armed_event);



sns_rc bmi160_hal_config_int_fifo(
        sns_sensor_instance     *const  instance,
        bool                    enable);


sns_rc bmi160_hal_disable_int_fifo(
        sns_sensor_instance     *const  instance,
        bool                    fast);

sns_rc bmi160_hal_restore_int_fifo(
        sns_sensor_instance     *const  instance);

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
void bmi160_hal_handle_interrupt_md(
        bmi160_instance_state   *istate,
        sns_time                 irq_timestamp);



/**
 * Processes a fifo buffer and extracts accel and gyro samples from the buffer
 * and generates events for each sample.
 *
 * @param[i] instance     Sensor instance
 * @param[i] fifo_buf     Buffer containing samples read from HW FIFO
 * @param[i] fifo_len     Number of bytes in fifo buffer
 */
void bmi160_hal_process_fifo_data_buffer(
        sns_sensor_instance     *instance,
        const uint8_t           *fifo_buf,
        uint32_t                fifo_len);

/**
 * Handle an interrupt by reading the Fifo status register and sending out
 * appropriate requests to the asynchronous com port sensor to read the fifo.
 *
 * @param instance       Sensor Instance
 */
void bmi160_hal_handle_interrupt_fifo(sns_sensor_instance *const instance);


sns_rc bmi160_hal_handle_interrupt(
        sns_sensor_instance     *const instance,
        bmi160_int_check_ctx    *ctx);

/**
 * Sends config update event for the chosen sample_rate
 *
 * @param[i] instance    reference to this Instance
 */
void bmi160_hal_send_config_event(sns_sensor_instance *const instance);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void bmi160_dev_convert_and_send_temp_sample(
        sns_sensor_instance *const instance,
        sns_time            timestamp,
        const uint8_t       temp_data[2]);

/**
 * Sends sensor temperature event.
 *
 * @param[i] instance   Sensor Instance
 */
void bmi160_hal_handle_sensor_temp_sample(sns_sensor_instance *const instance);


/**
 * Starts/restarts polling timer
 *
 * @param instance   Instance reference
 */
void bmi160_hal_start_sensor_temp_polling_timer(sns_sensor_instance *this);

/**
 * Configures sensor with new/recomputed settings
 *
 * @param instance   Instance reference
 */
sns_rc bmi160_hal_reconfig_hw(sns_sensor_instance  *this);



uint8_t bmi160_hal_fifo_get_sensors_to_invalidate(bmi160_instance_state   *istate);
void bmi160_hal_fifo_invalidate_sensors(
        bmi160_instance_state   *istate,
        uint32_t                sensors_to_invalidate);


void bmi160_hal_fifo_update_master_odr_req(bmi160_instance_state *state);
void bmi160_hal_fifo_calc_wml_req_ldt(bmi160_instance_state *istate);

/**
 * Sends a FIFO complete event.
 *
 * @param instance   Instance reference
 * @param uid        uid of sensor to send event to
 */
void bmi160_send_fifo_flush_done(
        sns_sensor_instance     *const instance,
        sns_sensor_uid          *uid,
        sns_time                ts,
        uint8_t                 context);

sns_rc bmi160_sbus_read_wrapper(
        void        *sbus_obj,
        uint32_t    rega,
        uint8_t     *buf,
        uint32_t    len);


sns_rc bmi160_sbus_write_wrapper(
        void        *sbus_obj,
        uint32_t    rega,
        uint8_t     *buf,
        uint32_t    len);


sns_rc bmi160_dev_get_reg_hw_err_stat(
        void        *sbus_obj,
        union bmi160_hw_err_stat        *hw_err_st);



void bmi160_hal_start_timer(
        sns_sensor_instance     *this,
        sns_data_stream         *timer_stream,
        sns_time                time_out);


sns_rc bmi160_hal_send_cmd(
        bmi160_instance_state   *istate,
        uint8_t                 regv_cmd);


sns_rc bmi160_hal_cmd_deque(bmi160_instance_state   *istate);
sns_rc bmi160_hal_update_pmu_stat(bmi160_instance_state   *istate, bool check_err);
sns_rc bmi160_hal_handle_timer_cmd(bmi160_instance_state   *istate);


void bmi160_hal_fifo_drain(
        bmi160_instance_state   *istate,
        bool                    sync_read,
        bmi160_fifo_flush_trigger_t trigger);

sns_rc bmi160_dev_reg_read_modify_write(
        void        *sbus_obj,
        uint8_t     rega,
        uint8_t     bit_start,
        uint8_t     bit_end,
        uint8_t     bit_block_val);

void bmi160_hal_match_odr(
        bmi160_sensor_type  sensor_type,
        float               odr_req,
        float               *odr_matched,
        bmi160_regv_odr_t   *regv);

/**
 * Reads value of a GPIO pin.
 * Function has been written to demonstrate use of the GPIO
 * Service to read gpio.
 *
 * @param[i] instance     instance reference
 * @param[i] gpio_cfg     gpio config
 * @param[i] level        gpio logic level
 *
 * @return none
 */

sns_rc bmi160_read_gpio(
        bmi160_instance_state   *istate,
        sns_interrupt_req       *gpio_cfg,
        sns_gpio_state          *level);

/**
 * Writes to a GPIO pin.
 * Function has been written to demonstrate use of the GPIO
 * Service to write to gpio.
 *
 * @param[i] instance        instance reference
 * @param[i] gpio            gpio pin to write to
 * @param[i] is_chip_pin     true if this is a chip level TLMM
 *  	 pin
 * @param[i] drive_strength  gpio pin drive strength
 * @param[i] pull            pull type config
 * @param[i] state           output state to write
 *
 * @return none
 */
#ifndef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
void bmi160_write_gpio(
        sns_sensor_instance     *instance,
        uint32_t                gpio,
        bool                    is_chip_pin,
        sns_gpio_drive_strength drive_strength,
        sns_gpio_pull_type      pull,
        sns_gpio_state          state);
#endif


static inline sns_time bmi160_get_sys_tick()
{
    return sns_get_system_time();
}

static inline uint32_t bmi160_convert_tick_2_us(sns_time ticks)
{
    uint32_t us;

    uint64_t ticks_in_1ms = sns_convert_ns_to_ticks(1000 * 1000);

    us = ticks * 1e3 / ticks_in_1ms;

    return us;
}

static inline float bmi160_convert_us2ticks(float us)
{
    float ticks;

    uint32_t ticks_in_1ms = sns_convert_ns_to_ticks(1000 * 1000);

    ticks = (us * ticks_in_1ms) * 1e-3;

    return ticks;
}


static inline void bmi160_delay_us(uint32_t us)
{
    sns_time    ticks;
    sns_rc      rc;

    ticks = sns_convert_ns_to_ticks(us * 1000);

    rc = sns_busy_wait(ticks);
}


//ASSUME_TS_IS_64BIT: sizeof(sns_time) == sizeof(uint64_t)
    BST_ASSUME_TS_IS_64BIT
static inline sns_time bmi160_get_time_elapse_sys(sns_time before, sns_time after)
{

    return (after - before);
}


BST_ASSUME_TS_IS_64BIT
static inline
//we don't know which of t1 and t2 is eariler
sns_time bmi160_get_time_interval(sns_time t1, sns_time t2)
{
    if (t2 >= t1) {
        return t2 - t1;
    } else {
        return t1 - t2;
    }
}

BST_ASSUME_TS_IS_64BIT
    static inline
sns_time bmi160_get_time_mid_sys(sns_time a, sns_time b)
{
    sns_time mid;

    mid = (a + b) >> 1;

    return mid;
}

//time a is later than b
BST_ASSUME_TS_IS_64BIT
#define BMI160_TIME_LT(a, b) ((a) > (b))

//time b is earier than a
BST_ASSUME_TS_IS_64BIT
#define BMI160_TIME_ET(b, a) ((a) > (b))



BST_ASSUME_TS_IS_64BIT
#define BMI160_SYS_TIME_LH(t)  ((uint32_t)(t & 0xffffffff))
#define BMI160_SYS_TIME_HH(t)  ((uint32_t)((t >> 32) & 0xffffffff))

void
bmi160_hal_stop_tempetature_timer(sns_sensor_instance  *const this);


void bmi160_hal_register_interrupt(sns_sensor_instance *this);


/**
 * quick prepare SPI if without write operation validation
 * @param this   sensor frame handler
 * @return  SNS_RC_SUCCESS     on success
 */
sns_rc bmi160_hal_sensor_prepare_spi_if(sns_sensor * const this);
