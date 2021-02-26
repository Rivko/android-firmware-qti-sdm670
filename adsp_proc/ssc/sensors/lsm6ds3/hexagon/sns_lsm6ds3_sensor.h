#pragma once
/**
 * @file sns_lsm6ds3_sensor.h
 *
 * LSM6DS3 Sensor implementation.
 *
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include "sns_data_stream.h"
#include "sns_lsm6ds3_hal.h"
#include "sns_island_service.h"
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

#include "sns_diag_service.h"
#include "sns_lsm6ds3_sensor_instance.h"
#include "sns_math_util.h"

#include "sns_registry_util.h"

#define ACCEL_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xee, 0x14, 0x9a, 0x2a, 0xac, 0x2e, 0x46, 0x2a,  \
        0x96, 0x6a, 0x5f, 0x14, 0xea, 0xec, 0x95, 0x47  \
      }  \
  }

#define GYRO_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x50, 0x07, 0xb9, 0xb2, 0x51, 0xa5, 0x45, 0x59,  \
        0xaf, 0x78, 0xca, 0x6a, 0xc3, 0x80, 0x1e, 0xa8  \
      }  \
  }

#define MOTION_DETECT_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xa7, 0xef, 0x92, 0x74, 0xf4, 0x0d, 0x45, 0xe9,  \
        0xa8, 0x11, 0xe6, 0xea, 0x7e, 0x5d, 0xa6, 0xb5  \
      }  \
  }

#define SENSOR_TEMPERATURE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xea, 0xc4, 0xcc, 0x53, 0x0f, 0x71, 0x40, 0xed,  \
        0x92, 0xc3, 0x99, 0x3f, 0x1a, 0xf4, 0x07, 0xaa   \
      }  \
  }

/** Forward Declaration of Accel Sensor API */
sns_sensor_api lsm6ds3_accel_sensor_api;

/** Forward Declaration of Gyro Sensor API */
sns_sensor_api lsm6ds3_gyro_sensor_api;

/** Forward Declaration of Motion Accel Sensor API */
sns_sensor_api lsm6ds3_motion_detect_sensor_api;

/** Forward Declaration of Sensor Temperature Sensor API */
sns_sensor_api lsm6ds3_sensor_temp_sensor_api;

/**
 * LSM6DS3 ODR (Hz) definitions
 */
#define LSM6DS3_ODR_0                 0.0
#define LSM6DS3_ODR_13                13.0
#define LSM6DS3_ODR_26                26.0
#define LSM6DS3_ODR_52                52.0
#define LSM6DS3_ODR_104               104.0
#define LSM6DS3_ODR_208               208.0
#define LSM6DS3_ODR_416               416.0
#define LSM6DS3_ODR_833               833.0
#define LSM6DS3_ODR_1660              1660.0
#define LSM6DS3_ODR_3330              3330.0
#define LSM6DS3_ODR_6660              6660.0

/**
 * Accelerometer ranges
 */
#define LSM6DS3_ACCEL_RANGE_2G_MIN    (-2)
#define LSM6DS3_ACCEL_RANGE_2G_MAX    (2)
#define LSM6DS3_ACCEL_RANGE_4G_MIN    (-4)
#define LSM6DS3_ACCEL_RANGE_4G_MAX    (4)
#define LSM6DS3_ACCEL_RANGE_8G_MIN    (-8)
#define LSM6DS3_ACCEL_RANGE_8G_MAX    (8)
#define LSM6DS3_ACCEL_RANGE_16G_MIN   (-16)
#define LSM6DS3_ACCEL_RANGE_16G_MAX   (16)

/**
 * Accelerometer resolutions
 */
#define LSM6DS3_ACCEL_RESOLUTION_2G    (0.061)
#define LSM6DS3_ACCEL_RESOLUTION_4G    (0.122)
#define LSM6DS3_ACCEL_RESOLUTION_8G    (0.244)
#define LSM6DS3_ACCEL_RESOLUTION_16G   (0.488)

/**
 * Gyroscope ranges
 */
#define LSM6DS3_GYRO_RANGE_245_MIN    (-245)
#define LSM6DS3_GYRO_RANGE_245_MAX    (245)
#define LSM6DS3_GYRO_RANGE_500_MIN    (-500)
#define LSM6DS3_GYRO_RANGE_500_MAX    (500)
#define LSM6DS3_GYRO_RANGE_1000_MIN   (-1000)
#define LSM6DS3_GYRO_RANGE_1000_MAX   (1000)
#define LSM6DS3_GYRO_RANGE_2000_MIN   (-2000)
#define LSM6DS3_GYRO_RANGE_2000_MAX   (2000)

/**
 * LSM6DS3 sensitivity for gyro in dps/LSB
 */
#define LSM6DS3_GYRO_SSTVT_245DPS   (0.008750)
#define LSM6DS3_GYRO_SSTVT_500DPS   (0.017500)
#define LSM6DS3_GYRO_SSTVT_1000DPS  (0.035000)
#define LSM6DS3_GYRO_SSTVT_2000DPS  (0.070000)

/**
 * LSM6DS3 Sensor Temperature ODR (Hz) definitions
 */
#define LSM6DS3_SENSOR_TEMP_ODR_1        1.0
#define LSM6DS3_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temprature resolution in deg Celsius/LSB
 * 1/16 deg Celsius per LSB
 */
#define LSM6DS3_SENSOR_TEMPERATURE_RESOLUTION  (0.0625)

/**
 * Sensor Temperature range in deg Celsius
 */
#define LSM6DS3_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define LSM6DS3_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)

/** Supported opertating modes */
#define LSM6DS3_LPM          "LPM"
#define LSM6DS3_HIGH_PERF    "HIGH_PERF"
#define LSM6DS3_NORMAL       "NORMAL"

#define LSM6DS3_POWER_RAIL_OFF_TIMEOUT_NS 1000000000ULL /* 1 second */

/** Power rail timeout States for the LSM6DS3 Sensors.*/
typedef enum
{
  LSM6DS3_POWER_RAIL_PENDING_NONE,
  LSM6DS3_POWER_RAIL_PENDING_INIT,
  LSM6DS3_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  LSM6DS3_POWER_RAIL_PENDING_OFF
} lsm6ds3_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct lsm6ds3_common_state
{
  lsm6ds3_com_port_info   com_port_info;
  sns_interrupt_req       irq_config;

  sns_rail_config         rail_config;
  sns_power_rail_state    registry_rail_on_state;

  bool                    hw_is_present;
  uint16_t                who_am_i;

  // Registry, IRQ, Timer, ASCP, DAE
  SNS_SUID_LOOKUP_DATA(5) suid_lookup_data;

  // registry sensor config
  bool registry_cfg_received;
  sns_registry_phy_sensor_cfg registry_cfg;

  // registry sensor platform config
  bool registry_pf_cfg_received;
  sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

  // axis conversion
  bool registry_orient_received;
  triaxis_conversion axis_map[TRIAXIS_NUM];

  // placement
  bool                    registry_placement_received;
  float                   placement[12];

} lsm6ds3_common_state;

typedef struct lsm6ds3_state
{
  lsm6ds3_common_state    common;
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;
  sns_island_service      *island_service;
  sns_sync_com_port_service *scp_service;

  lsm6ds3_sensor_type     sensor;
  sns_sensor_uid          my_suid;

  bool                    sensor_client_present;

  lsm6ds3_power_rail_pending_state    power_rail_pend_state;

  // sensor configuration
  bool is_dri;
  int64_t hardware_id;
  bool supports_sync_stream;
  uint8_t resolution_idx;

  // factory calibration
  bool                    registry_fac_cal_received;
  matrix3                 fac_cal_corr_mat;
  float                   fac_cal_bias[TRIAXIS_NUM];
  float                   fac_cal_scale[TRIAXIS_NUM];
  uint32_t                fac_cal_version;

  // md config
  bool                    registry_md_config_received;
  sns_registry_md_cfg     md_config;

  size_t                  encoded_event_len;

} lsm6ds3_state;

/** Functions shared by all LSM6DS3 Sensors */
/**
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 *
 * @param[i] this          Sensor reference
 * @param[i] instance      Sensor Instance to config
 * @param[i] sensor_type   Sensor type
 *
 * @return none
 */
void lsm6ds3_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6ds3_sensor_type sensor_type);

/**
 * notify_event() Sensor API common between all LSM6DS3 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc lsm6ds3_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all LSM6DS3
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* lsm6ds3_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);
sns_rc lsm6ds3_accel_init(sns_sensor *const this);
sns_rc lsm6ds3_gyro_init(sns_sensor *const this);
sns_rc lsm6ds3_motion_detect_init(sns_sensor *const this);
sns_rc lsm6ds3_sensor_temp_init(sns_sensor *const this);
sns_rc lsm6ds3_accel_deinit(sns_sensor *const this);
sns_rc lsm6ds3_gyro_deinit(sns_sensor *const this);
sns_rc lsm6ds3_motion_detect_deinit(sns_sensor *const this);
sns_rc lsm6ds3_sensor_temp_deinit(sns_sensor *const this);
void lsm6ds3_sensor_process_registry_event(sns_sensor *const this,
                                           sns_sensor_event *event);
void lsm6ds3_request_registry(sns_sensor *const this);
void lsm6ds3_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, lsm6ds3_sensor_type sensor);
void lsm6ds3_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance);
bool lsm6ds3_discover_hw(sns_sensor *const this);
void lsm6ds3_publish_available(sns_sensor *const this);
void lsm6ds3_update_sibling_sensors(sns_sensor *const this);
void lsm6ds3_start_hw_detect_sequence(sns_sensor *const this);
void lsm6ds3_start_power_rail_timer(sns_sensor *const this,
                    sns_time timeout_ticks,
                    lsm6ds3_power_rail_pending_state pwr_rail_pend_state);
void lsm6ds3_common_init(sns_sensor *const this);

