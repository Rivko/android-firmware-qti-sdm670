/*******************************************************************************
 * Copyright (c) 2017, Bosch Sensortec GmbH
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
 * @file sns_bmi160.c
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#pragma once

#include "sns_sensor.h"
#include "sns_data_stream.h"
#include "sns_sensor_uid.h"
#include "sns_pwr_rail_service.h"
#include "sns_island_service.h"

#include "sns_math_util.h"
#include "sns_diag_service.h"
#include "sns_registry_util.h"
#include "sns_suid_util.h"

#include "sns_bmi160_config.h"
#include "sns_bmi160_hal.h"
#include "sns_bmi160_sensor_instance.h"

#define ACCEL_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x6e, 0x93, 0x2d, 0xa2, 0x52, 0x52, 0x4D, 0x21,  \
        0x90, 0x0A, 0x87, 0x78, 0x42, 0x8B, 0x7A, 0xED \
    }  \
}

#define GYRO_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x57, 0xE5, 0xF0, 0xFC, 0x3C, 0x5A, 0x47, 0x2D,  \
        0xAA, 0x24, 0x1A, 0x6D, 0x65, 0x43, 0x4C, 0x5F \
    }  \
}

#define MOTION_DETECT_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x6e, 0x93, 0x34, 0xaa, 0x33, 0x4A, 0x4E, 0x24,  \
        0xAD, 0xFE, 0x5B, 0xED, 0xD0, 0x60, 0xAA, 0x66\
    }  \
}


#define SENSOR_TEMPERATURE_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x96, 0x0D, 0x20, 0xB4, 0xC8, 0xAD, 0x45, 0x77,  \
        0xA0, 0x36, 0x7E, 0x9E, 0x69, 0x4E, 0x8E, 0x22 \
    }  \
}


/** Forward Declaration of Accel Sensor API */
extern sns_sensor_api bmi160_accel_sensor_api;

/** Forward Declaration of Gyro Sensor API */
extern sns_sensor_api bmi160_gyro_sensor_api;

/** Forward Declaration of Motion Accel Sensor API */
extern sns_sensor_api bmi160_motion_detect_sensor_api;

/** Forward Declaration of Sensor Temperature Sensor API */
extern sns_sensor_api bmi160_sensor_temp_sensor_api;

/**
 * BMI160 Power consumption definition
 */
#define BMI160_ACC_ACTIVE_CURRENT     180
#define BMI160_ACC_LOWPOWER_CURRENT   3

#define BMI160_GYRO_ACTIVE_CURRENT    900
#define BMI160_GYRO_LOWPOWER_CURRENT  3

#define BMI160_MOTION_DETECTION_ACTIVE_CURRENT  12
#define BMI160_MOTION_DETECTION_LOWPOER_CURRENT 6

#define BMI160_TEMP_ACTIVE_CURRENT    180
#define BMI160_TEMP_LOWPOWER_CURRENT  3

/**
 * BMI160 ODR (Hz) definitions
 */
#define BMI160_ODR_0                 0.0
#define BMI160_ODR_12P5              12.5
#define BMI160_ODR_25                25.0
#define BMI160_ODR_50                50
#define BMI160_ODR_100               100
#define BMI160_ODR_200               200
#define BMI160_ODR_400               400
#define BMI160_ODR_800               800
#define BMI160_ODR_1600              1600
#define BMI160_ODR_3200              3200.0
#define BMI160_ODR_6400              6400.0

/**
 * Accelerometer ranges
 */
#define BMI160_ACCEL_RANGE_2G_MIN    (-2)
#define BMI160_ACCEL_RANGE_2G_MAX    (2)
#define BMI160_ACCEL_RANGE_4G_MIN    (-4)
#define BMI160_ACCEL_RANGE_4G_MAX    (4)
#define BMI160_ACCEL_RANGE_8G_MIN    (-8)
#define BMI160_ACCEL_RANGE_8G_MAX    (8)
#define BMI160_ACCEL_RANGE_16G_MIN   (-16)
#define BMI160_ACCEL_RANGE_16G_MAX   (16)

/**
 * Accelerometer resolutions
 */
#define BMI160_ACCEL_RESOLUTION_2G    (0.061)
#define BMI160_ACCEL_RESOLUTION_4G    (0.122)
#define BMI160_ACCEL_RESOLUTION_8G    (0.244)
#define BMI160_ACCEL_RESOLUTION_16G   (0.488)


/* in the unit of micro-g/lsb */
#define BMI160_ACCEL_SSTVT_2G    (1e6 * 4 / 65536)
#define BMI160_ACCEL_SSTVT_4G    (1e6 * 8 / 65536)
#define BMI160_ACCEL_SSTVT_8G    (1e6 * 16 / 65536)
#define BMI160_ACCEL_SSTVT_16G    (1e6 * 32 / 65536)


#define BMI160_SCALE_FACTOR_DATA_ACCEL     1e7  //can be 1e6 if we use +/- 8g or +/-16g range is used
#define BMI160_SCALE_FACTOR_DATA_DFT       1e6
#define BMI160_SCALE_FACTOR_DATA_GYRO      BMI160_SCALE_FACTOR_DATA_DFT
#define BMI160_SCALE_FACTOR_DATA_TEMP      BMI160_SCALE_FACTOR_DATA_DFT

/**
 * Gyroscope ranges
 */
#define BMI160_GYRO_RANGE_125_MIN    (-125)
#define BMI160_GYRO_RANGE_125_MAX    (125)
#define BMI160_GYRO_RANGE_250_MIN    (-250)
#define BMI160_GYRO_RANGE_250_MAX    (250)
#define BMI160_GYRO_RANGE_500_MIN    (-500)
#define BMI160_GYRO_RANGE_500_MAX    (500)
#define BMI160_GYRO_RANGE_1000_MIN   (-1000)
#define BMI160_GYRO_RANGE_1000_MAX   (1000)
#define BMI160_GYRO_RANGE_2000_MIN   (-2000)
#define BMI160_GYRO_RANGE_2000_MAX   (2000)



#define BMI160_GYRO_RESOLUTION_125DPS    (250 * 1.0 / 65536)
#define BMI160_GYRO_RESOLUTION_250DPS    (500 * 1.0 / 65536)
#define BMI160_GYRO_RESOLUTION_500DPS    (1000 * 1.0 / 65536)
#define BMI160_GYRO_RESOLUTION_1000DPS    (2000 * 1.0 / 65536)
#define BMI160_GYRO_RESOLUTION_2000DPS    (4000 * 1.0 / 65536)

/**
 * BMI160 sensitivity for gyro in (urad/sec)/LSB
 */
#define BMI160_GYRO_SSTVT_125DPS    ((250 *  PI / 180 * 1e6) / 65536)
#define BMI160_GYRO_SSTVT_250DPS    ((500 *  PI / 180 * 1e6) / 65536)
#define BMI160_GYRO_SSTVT_500DPS    ((1000 * PI / 180 * 1e6) / 65536)
#define BMI160_GYRO_SSTVT_1000DPS   ((2000 * PI / 180 * 1e6) / 65536)
#define BMI160_GYRO_SSTVT_2000DPS   ((4000 * PI / 180 * 1e6) / 65536)

/**
 * BMI160 Sensor Temperature ODR (Hz) definitions
 */
#define BMI160_SENSOR_TEMP_ODR_1        1.0
#define BMI160_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temperature resolution in deg Celsius/LSB
 * 1/16 deg Celsius per LSB
 */
#define BMI160_SENSOR_TEMPERATURE_RESOLUTION  (0.002)

/**
 * Sensor Temperature range in deg Celsius
 */
#define BMI160_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define BMI160_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)

/** Supported operating modes */
#define BMI160_LPM          "LPM"
#define BMI160_HIGH_PERF    "HIGH_PERF"
#define BMI160_NORMAL       "NORMAL"

/** Power rail timeout States for the BMI160 Sensors.*/
typedef enum
{
    BMI160_POWER_RAIL_PENDING_NONE,          //!< BMI160_POWER_RAIL_PENDING_NONE
    BMI160_POWER_RAIL_PENDING_INIT,          //!< BMI160_POWER_RAIL_PENDING_INIT
    BMI160_POWER_RAIL_PENDING_SET_CLIENT_REQ,//!< BMI160_POWER_RAIL_PENDING_SET_CLIENT_REQ
} bmi160_power_rail_pending_state;

/** Interrupt Sensor State. */

/**
 * Sensor common state structure
 */
typedef struct bmi160_common_state
{
    bmi160_com_port_info    com_port_info;
    sns_interrupt_req       irq_config;

    sns_rail_config         rail_config;
    sns_power_rail_state    registry_rail_on_state;


    SNS_SUID_LOOKUP_DATA(5) suid_lookup_data;

    uint32_t                who_am_i:16;
    uint32_t                hw_is_present:1;
    // registry sensor config
    // registry sensor config
    uint32_t                registry_pf_cfg_received:1;
    uint32_t                registry_placement_received:1;
    uint32_t                registry_orient_received:1;

    // test
    uint32_t                init_flags:8;

    sns_registry_phy_sensor_cfg     registry_cfg;

    // registry sensor platform config
    sns_registry_phy_sensor_pf_cfg  registry_pf_cfg;

    // axis conversion
    triaxis_conversion axis_map[TRIAXIS_NUM];

    // placement
    float                   placement[12];

} bmi160_common_state;


/**
 * Sensor state structure
 */

typedef struct bmi160_state
{
    bmi160_common_state     common;
#if BMI160_CONFIG_ENABLE_REGISTRY
    sns_data_stream         *reg_data_stream;
#endif
    sns_data_stream         *fw_stream;
    sns_data_stream         *timer_stream;
    sns_pwr_rail_service    *pwr_rail_service;
#if BMI160_CONFIG_ENABLE_ISLAND_MODE
    sns_island_service      *island_service;
#endif
    sns_diag_service        *diag_service;
    sns_sync_com_port_service *scp_service;

    sns_sensor_uid          my_suid;

    uint32_t scale_factor               :24;    //up to 10^7
    uint32_t sensor                     :8; //of bmi160_sensor_type
    //bmi160_power_rail_pending_state power_rail_pend_state;
    uint32_t power_rail_pend_state      :3;

    // sensor enable flag
    uint32_t sensor_client_present :1;
    // sensor configuration
    uint32_t is_dri :1;
    uint32_t supports_sync_stream :1;
    uint32_t registry_md_config_received :1;

    uint32_t registry_cfg_received:1;
    uint32_t registry_fac_cal_received  :1;
    uint32_t registry_attr_published:1;
    uint32_t resolution_idx             :4;


    int64_t hardware_id;

    //TOPMZ: use union of fac_cal and md_config to save space
    // factory calibration
    matrix3 fac_cal_corr_mat;
    int32_t fac_cal_bias[TRIAXIS_NUM];
    float fac_cal_scale[TRIAXIS_NUM];
    uint32_t fac_cal_version;

    // md config
    sns_registry_md_cfg md_config;

    size_t encoded_event_len;

    sns_sensor *owner;

} bmi160_state;


/**
 * notify_event() Sensor API common between all BMI160 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc bmi160_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all BMI160
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* bmi160_set_client_request(sns_sensor *const            this,
        struct sns_request const    *exist_request,
        struct sns_request const    *new_request,
        bool                        remove);
sns_rc bmi160_accel_init(sns_sensor *const this);
sns_rc bmi160_gyro_init(sns_sensor *const this);
sns_rc bmi160_motion_detect_init(sns_sensor *const this);
sns_rc bmi160_sensor_temp_init(sns_sensor *const this);
sns_rc bmi160_accel_deinit(sns_sensor *const this);
sns_rc bmi160_gyro_deinit(sns_sensor *const this);
sns_rc bmi160_motion_detect_deinit(sns_sensor *const this);
sns_rc bmi160_sensor_temp_deinit(sns_sensor *const this);


void bmi160_common_init(sns_sensor *const this);


void bmi160_start_power_rail_timer(sns_sensor *const this,
        sns_time timeout_ticks,
        bmi160_power_rail_pending_state pwr_rail_pend_state);

/**
 * Publish the sensor availability
 * @param this   sensor handler
 *
 * @return none
 */
void bmi160_publish_available(sns_sensor *const this);


/**
 * Update IMU the sibling sensors
 * @param this   sensor handler
 */
void bmi160_update_sibling_sensors(sns_sensor *const this);

/**
 * BMI160 HW detection function
 * @param this   sensor handler
 */
void bmi160_start_hw_detect_sequence(sns_sensor *this);

bool bmi160_discover_hw(sns_sensor *const this);

// <registry>
#if BMI160_CONFIG_ENABLE_REGISTRY
void bmi160_sensor_process_registry_event(sns_sensor *const this,
        sns_sensor_event *event);

void bmi160_publish_registry_attributes(sns_sensor *const this);
void bmi160_sensor_check_registry_col_progress(sns_sensor *const this);

void bmi160_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, bmi160_sensor_type sensor);
void bmi160_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance);
void bmi160_request_registry(sns_sensor *const this);
#else

void sns_bmi160_registry_def_config(sns_sensor *const this);
#endif
// </registry>
