#pragma once
/**
 * @file sns_lsm6dso_sensor.h
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

#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_lsm6dso_hal.h"
#include "sns_math_util.h"
#include "sns_pwr_rail_service.h"
#include "sns_registry_util.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_sync_com_port_service.h"
#include "sns_lsm6dso_ver.h"

#define ACCEL_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x44, 0xba, 0xd7, 0xf8, 0x19, 0x8d, 0x41, 0x9a,  \
        0xa6, 0x7e, 0x72, 0x01, 0xfc, 0x6a, 0xbb, 0x1f  \
      }  \
  }

#define GYRO_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xc1, 0x11, 0x15, 0x49, 0x10, 0x8a, 0x41, 0xb9,  \
        0xa0, 0x72, 0xf9, 0x69, 0x89, 0xc4, 0xd3, 0x97  \
      }  \
  }

#define MOTION_DETECT_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xcd, 0x4b, 0x1a, 0x6d, 0x68, 0xda, 0x4a, 0x77,  \
        0x85, 0xab, 0x46, 0xe2, 0xcd, 0xa5, 0x68, 0x0b  \
      }  \
  }

#define SENSOR_TEMPERATURE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x79, 0x32, 0xb8, 0x9c, 0xdc, 0x02, 0x40, 0xaf,  \
        0xa3, 0x8f, 0x80, 0xc0, 0xc0, 0x15, 0x36, 0x97   \
      }  \
  }

#ifndef SUID_IS_NULL
#define SUID_IS_NULL(suid_ptr) ( sns_memcmp( (suid_ptr),                \
                                             &(sns_sensor_uid){{0}},    \
                                             sizeof(sns_sensor_uid) ) == 0 )
#endif

#define NAME   "lsm6dso"
#if BUILD_DB
#define VENDOR "template"
#else
#define VENDOR "STMicro"
#endif


/**
 * LSM6DSO ODR definitions
 */
#define LSM6DSO_ODR_0                 0.0
#define LSM6DSO_ODR_13                13.0
#define LSM6DSO_ODR_26                26.0
#define LSM6DSO_ODR_52                52.0
#define LSM6DSO_ODR_104               104.0
#define LSM6DSO_ODR_208               208.0
#define LSM6DSO_ODR_416               416.0
#define LSM6DSO_ODR_833               833.0
#define LSM6DSO_ODR_1660              1660.0
#define LSM6DSO_ODR_3330              3330.0
#define LSM6DSO_ODR_6660              6660.0
/**
 * Accelerometer ranges in +/-g unit
 */
#define LSM6DSO_ACCEL_RANGE_2G_MIN    (-2)
#define LSM6DSO_ACCEL_RANGE_2G_MAX    (2)
#define LSM6DSO_ACCEL_RANGE_4G_MIN    (-4)
#define LSM6DSO_ACCEL_RANGE_4G_MAX    (4)
#define LSM6DSO_ACCEL_RANGE_8G_MIN    (-8)
#define LSM6DSO_ACCEL_RANGE_8G_MAX    (8)
#define LSM6DSO_ACCEL_RANGE_16G_MIN   (-16)
#define LSM6DSO_ACCEL_RANGE_16G_MAX   (16)

/**
 * Accelerometer resolutions in mg/LSB
 */
#define LSM6DSO_ACCEL_RESOLUTION_2G    (0.061)
#define LSM6DSO_ACCEL_RESOLUTION_4G    (0.122)
#define LSM6DSO_ACCEL_RESOLUTION_8G    (0.244)
#define LSM6DSO_ACCEL_RESOLUTION_16G   (0.488)

/**
 * Gyroscope ranges in dps
 */
#define LSM6DSO_GYRO_RANGE_125_MIN    (-125)
#define LSM6DSO_GYRO_RANGE_125_MAX    (125)
#define LSM6DSO_GYRO_RANGE_245_MIN    (-245)
#define LSM6DSO_GYRO_RANGE_245_MAX    (245)
#define LSM6DSO_GYRO_RANGE_500_MIN    (-500)
#define LSM6DSO_GYRO_RANGE_500_MAX    (500)
#define LSM6DSO_GYRO_RANGE_1000_MIN   (-1000)
#define LSM6DSO_GYRO_RANGE_1000_MAX   (1000)
#define LSM6DSO_GYRO_RANGE_2000_MIN   (-2000)
#define LSM6DSO_GYRO_RANGE_2000_MAX   (2000)

/**
 * LSM6DSO sensitivity for gyro in dps/LSB
 */
#define LSM6DSO_GYRO_SSTVT_125DPS   (0.004375)
#define LSM6DSO_GYRO_SSTVT_245DPS   (0.008750)
#define LSM6DSO_GYRO_SSTVT_500DPS   (0.017500)
#define LSM6DSO_GYRO_SSTVT_1000DPS  (0.035000)
#define LSM6DSO_GYRO_SSTVT_2000DPS  (0.070000)

/**
 * LSM6DSO Sensor Temperature ODR (Hz) definitions
 */
#define LSM6DSO_SENSOR_TEMP_ODR_1        1.0
#define LSM6DSO_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temprature resolution in deg Celsius/LSB
 * 1/16 deg Celsius per LSB
 */
#define LSM6DSO_SENSOR_TEMPERATURE_RESOLUTION  (0.0625)

/**
 * Sensor Temperature range in deg Celsius
 */
#define LSM6DSO_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define LSM6DSO_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)


/** Supported opertating modes */
#define LSM6DSO_LPM          "LPM"
#define LSM6DSO_HIGH_PERF    "HIGH_PERF"
#define LSM6DSO_NORMAL       "NORMAL"

typedef enum
{
  ATTR_AVAILABLE = 0,
  ATTR_NAME,
  ATTR_DATA_TYPE,
  ATTR_VENDOR,
  ATTR_VERSION,
  ATTR_RATES,
  ATTR_RESOLUTION,
  ATTR_FIFO_SIZE,
  ATTR_ACTIVE_CURRENT,
  ATTR_SLEEP_CURRENT,
  ATTR_RANGES,
  ATTR_OP_MODES,
  ATTR_API,
  ATTR_EVENT_SIZE,
  ATTR_STREAM_TYPE,
  ATTR_IS_DYNAMIC,
  ATTR_RIGID_BODY,
  ATTR_PLACEMENT,
  ATTR_HW_ID,
  ATTR_DRI_SUPPORT,
  ATTR_SYNC_STREAM_SUPPORT,
  ATTR__MAX,
}lsm6dso_attrib;

/** Power rail timeout States for the LSM6DSO Sensors.*/
#define LSM6DSO_POWER_RAIL_OFF_TIMEOUT_NS 1000000000ULL /* 1 second */
typedef enum
{
  LSM6DSO_POWER_RAIL_PENDING_NONE,
  LSM6DSO_POWER_RAIL_PENDING_INIT,
  LSM6DSO_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  LSM6DSO_POWER_RAIL_PENDING_OFF,
} lsm6dso_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct lsm6dso_state
{
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_sensor_uid          irq_suid;
  sns_sensor_uid          timer_suid;
  sns_sensor_uid          acp_suid; // Asynchronous COM Port
  sns_sensor_uid          reg_suid;
  sns_sensor_uid          dae_suid;
  lsm6dso_sensor_type     sensor;
  sns_sensor_uid          my_suid;
  lsm6dso_com_port_info   com_port_info;
  sns_interrupt_req       irq_config;

  sns_pwr_rail_service    *pwr_rail_service;
  sns_rail_config         rail_config;
  sns_power_rail_state    registry_rail_on_state;
  lsm6dso_power_rail_pending_state    power_rail_pend_state;

  bool                    hw_is_present:1;
  bool                    available:1;
  bool                    sensor_client_present:1;

  // sensor configuration
  bool                    is_dri:1;
  bool                    supports_sync_stream:1;
  int64_t                 hardware_id;
  uint8_t                 resolution_idx;
  uint8_t                 accel_resolution_idx;
  uint8_t                 gyro_resolution_idx;

  // registry sensor config
  int8_t                  outstanding_reg_requests;
  uint8_t                 rigid_body_type;
  uint32_t                registry_persist_version;

  // axis conversion
  triaxis_conversion      axis_map[TRIAXIS_NUM];

  // factory calibration
  matrix3                 fac_cal_corr_mat;
  float                   fac_cal_bias[TRIAXIS_NUM];
  float                   fac_cal_scale[TRIAXIS_NUM];

  // placement
  float                   placement[12];

  // md config
  sns_registry_md_cfg     md_config;

  // debug
  uint16_t                who_am_i;
  sns_diag_service *diag_service;
  sns_sync_com_port_service *scp_service;
  size_t                  encoded_event_len;

} lsm6dso_state;
#define MAX_SUPPORTED_SENSORS 4
typedef struct {
  lsm6dso_sensor_type sensor;
  sns_sensor_api* sensor_api;
  sns_sensor_instance_api* instance_api;
} lsm6dso_sensors;


/** Global const tables */
extern const float lsm6dso_accel_resolutions[];
extern const float lsm6dso_gyro_resolutions[];
extern lsm6dso_accel_range lsm6dso_accel_ranges[];
extern lsm6dso_gyro_range lsm6dso_gyro_ranges[];
extern const float lsm6dso_accel_range_min[];
extern const float lsm6dso_accel_range_max[];
extern const float lsm6dso_gyro_range_min[];
extern const float lsm6dso_gyro_range_max[];
extern const lsm6dso_sensors lsm6dso_supported_sensors[MAX_SUPPORTED_SENSORS];


/** Functions shared by all LSM6DSO Sensors */
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
void lsm6dso_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6dso_sensor_type sensor_type);

/**
 * set_client_request() Sensor API common between all LSM6DSO
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* lsm6dso_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);

/**
 * Publishes default Sensor attributes.
 *
 * @param this   Sensor reference
 *
 * @return none
 */
void lsm6dso_publish_def_attributes(sns_sensor *const this);
void lsm6dso_init_sensor_info(sns_sensor *const this,
                              sns_sensor_uid const *suid,
                              lsm6dso_sensor_type sensor_type);
void lsm6dso_process_suid_events(sns_sensor *const this);
sns_rc lsm6dso_process_registry_events(sns_sensor *const this);
void lsm6dso_discover_hw(sns_sensor *const this);
void lsm6dso_update_siblings(sns_sensor *const this);
sns_sensor_uid const* lsm6dso_get_sensor_uid(sns_sensor const *const this);
bool lsm6dso_sensor_write_output_to_registry(sns_sensor *const this,
                                             sns_sensor_instance *const instance);

sns_rc lsm6dso_accel_init(sns_sensor *const this);
sns_rc lsm6dso_gyro_init(sns_sensor *const this);
sns_rc lsm6dso_motion_detect_init(sns_sensor *const this);
sns_rc lsm6dso_sensor_temp_init(sns_sensor *const this);
sns_rc lsm6dso_accel_deinit(sns_sensor *const this);
sns_rc lsm6dso_gyro_deinit(sns_sensor *const this);
sns_rc lsm6dso_motion_detect_deinit(sns_sensor *const this);
sns_rc lsm6dso_sensor_temp_deinit(sns_sensor *const this);

