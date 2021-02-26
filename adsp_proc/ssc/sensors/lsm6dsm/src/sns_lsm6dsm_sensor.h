#pragma once
/**
 * @file sns_lsm6dsm_sensor.h
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
#include "sns_lsm6dsm_hal.h"
#include "sns_math_util.h"
#include "sns_pwr_rail_service.h"
#include "sns_registry_util.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_sync_com_port_service.h"
#include "sns_lsm6dsm_ver.h"

#define ACCEL_SUID_0 \
  {  \
    .sensor_uid =  \
      {  \
        0x38, 0x2b, 0xca, 0x0a, 0x0e, 0xd4, 0x47, 0x58,  \
        0xb6, 0xb5, 0x2a, 0x81, 0x83, 0xa2, 0x82, 0x2a  \
      }  \
  }

#define GYRO_SUID_0 \
  {  \
    .sensor_uid =  \
      {  \
        0x17, 0xe7, 0x13, 0x11, 0x88, 0xfe, 0x4b, 0xbe,  \
        0x9c, 0x7f, 0x2d, 0x28, 0x06, 0xa0, 0x1e, 0x01  \
      }  \
  }

#define MOTION_DETECT_SUID_0 \
  {  \
    .sensor_uid =  \
      {  \
        0xaf, 0x6f, 0xe9, 0x88, 0xe3, 0x23, 0x4c, 0x30,  \
        0xa7, 0x3f, 0x4d, 0xa6, 0x36, 0x2b, 0x0a, 0x32  \
      }  \
  }

#define SENSOR_TEMPERATURE_SUID_0 \
  {  \
    .sensor_uid =  \
      {  \
        0x43, 0x04, 0x48, 0x2d, 0x45, 0xc1, 0x40, 0xbf,  \
        0xb5, 0x82, 0x72, 0x8c, 0x19, 0xbe, 0x01, 0xc0   \
      }  \
  }

#if DUAL_SENSOR_SUPPORT
#define ACCEL_SUID_1 \
  {  \
    .sensor_uid =  \
      {  \
        0x70, 0x30, 0x96, 0xcc, 0xce, 0x71, 0x49, 0xfa,  \
        0x83, 0x8b, 0xf8, 0xd5, 0x98, 0x02, 0xaf, 0x88  \
      }  \
  }

#define GYRO_SUID_1 \
  {  \
    .sensor_uid =  \
      {  \
        0xd4, 0x5a, 0x44, 0x98, 0x77, 0xbf, 0x43, 0x6d,  \
        0xad, 0xb0, 0xb5, 0xeb, 0x62, 0xf9, 0xf5, 0x20  \
      }  \
  }

#define MOTION_DETECT_SUID_1 \
  {  \
    .sensor_uid =  \
      {  \
        0x03, 0x55, 0x1b, 0xc1, 0x2c, 0x04, 0x47, 0xec, \
        0x89, 0xbc, 0x0f, 0x7c, 0x4a, 0xb0, 0x46, 0x5a  \
      }  \
  }
#define SENSOR_TEMPERATURE_SUID_1 \
  {  \
    .sensor_uid =  \
      {  \
        0x1b, 0x81, 0x48, 0x2e, 0x05, 0x1f, 0x4d, 0xf9, \
        0x9d, 0xcb, 0x66, 0x0d, 0x6a, 0x25, 0x5b, 0x92  \
      }  \
  }
#endif

#ifndef SUID_IS_NULL
#define SUID_IS_NULL(suid_ptr) ( sns_memcmp( (suid_ptr),                \
                                             &(sns_sensor_uid){{0}},    \
                                             sizeof(sns_sensor_uid) ) == 0 )
#endif

#define NAME   "lsm6dsm"
#if BUILD_DB
#define VENDOR "template"
#else
#define VENDOR "STMicro"
#endif


/**
 * LSM6DSM ODR definitions
 */
#define LSM6DSM_ODR_0                 0.0
#define LSM6DSM_ODR_13                13.0
#define LSM6DSM_ODR_26                26.0
#define LSM6DSM_ODR_52                52.0
#define LSM6DSM_ODR_104               104.0
#define LSM6DSM_ODR_208               208.0
#define LSM6DSM_ODR_416               416.0
#define LSM6DSM_ODR_833               833.0
#define LSM6DSM_ODR_1660              1660.0
#define LSM6DSM_ODR_3330              3330.0
#define LSM6DSM_ODR_6660              6660.0
/**
 * Accelerometer ranges in +/-g unit
 */
#define LSM6DSM_ACCEL_RANGE_2G_MIN    (-2)
#define LSM6DSM_ACCEL_RANGE_2G_MAX    (2)
#define LSM6DSM_ACCEL_RANGE_4G_MIN    (-4)
#define LSM6DSM_ACCEL_RANGE_4G_MAX    (4)
#define LSM6DSM_ACCEL_RANGE_8G_MIN    (-8)
#define LSM6DSM_ACCEL_RANGE_8G_MAX    (8)
#define LSM6DSM_ACCEL_RANGE_16G_MIN   (-16)
#define LSM6DSM_ACCEL_RANGE_16G_MAX   (16)

/**
 * Accelerometer resolutions in mg/LSB
 */
#define LSM6DSM_ACCEL_RESOLUTION_2G    (0.061)
#define LSM6DSM_ACCEL_RESOLUTION_4G    (0.122)
#define LSM6DSM_ACCEL_RESOLUTION_8G    (0.244)
#define LSM6DSM_ACCEL_RESOLUTION_16G   (0.488)

/**
 * Gyroscope ranges in dps
 */
#define LSM6DSM_GYRO_RANGE_125_MIN    (-125)
#define LSM6DSM_GYRO_RANGE_125_MAX    (125)
#define LSM6DSM_GYRO_RANGE_245_MIN    (-245)
#define LSM6DSM_GYRO_RANGE_245_MAX    (245)
#define LSM6DSM_GYRO_RANGE_500_MIN    (-500)
#define LSM6DSM_GYRO_RANGE_500_MAX    (500)
#define LSM6DSM_GYRO_RANGE_1000_MIN   (-1000)
#define LSM6DSM_GYRO_RANGE_1000_MAX   (1000)
#define LSM6DSM_GYRO_RANGE_2000_MIN   (-2000)
#define LSM6DSM_GYRO_RANGE_2000_MAX   (2000)

/**
 * LSM6DSM sensitivity for gyro in dps/LSB
 */
#define LSM6DSM_GYRO_SSTVT_125DPS   (0.004375)
#define LSM6DSM_GYRO_SSTVT_245DPS   (0.008750)
#define LSM6DSM_GYRO_SSTVT_500DPS   (0.017500)
#define LSM6DSM_GYRO_SSTVT_1000DPS  (0.035000)
#define LSM6DSM_GYRO_SSTVT_2000DPS  (0.070000)

/**
 * LSM6DSM Sensor Temperature ODR (Hz) definitions
 */
#define LSM6DSM_SENSOR_TEMP_ODR_1        1.0
#define LSM6DSM_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temprature resolution in deg Celsius/LSB
 * 1/16 deg Celsius per LSB
 */
#define LSM6DSM_SENSOR_TEMPERATURE_RESOLUTION  (0.0625)

/**
 * Sensor Temperature range in deg Celsius
 */
#define LSM6DSM_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define LSM6DSM_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)


/** Supported opertating modes */
#define LSM6DSM_LPM          "LPM"
#define LSM6DSM_HIGH_PERF    "HIGH_PERF"
#define LSM6DSM_NORMAL       "NORMAL"

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
}lsm6dsm_attrib;

/** Power rail timeout States for the LSM6DSM Sensors.*/
#define LSM6DSM_POWER_RAIL_OFF_TIMEOUT_NS 1000000000ULL /* 1 second */
typedef enum
{
  LSM6DSM_POWER_RAIL_PENDING_NONE,
  LSM6DSM_POWER_RAIL_PENDING_INIT,
  LSM6DSM_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  LSM6DSM_POWER_RAIL_PENDING_OFF,
} lsm6dsm_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct lsm6dsm_state
{
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_sensor_uid          irq_suid;
  sns_sensor_uid          timer_suid;
  sns_sensor_uid          acp_suid; // Asynchronous COM Port
  sns_sensor_uid          reg_suid;
  sns_sensor_uid          dae_suid;
  lsm6dsm_sensor_type     sensor;
  sns_sensor_uid          my_suid;
  lsm6dsm_com_port_info   com_port_info;
  sns_interrupt_req       irq_config;
  uint32_t                hw_idx;

  sns_pwr_rail_service    *pwr_rail_service;
  sns_rail_config         rail_config;
  sns_power_rail_state    registry_rail_on_state;
  lsm6dsm_power_rail_pending_state    power_rail_pend_state;

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

} lsm6dsm_state;
#define MAX_SUPPORTED_SENSORS 4
typedef struct {
  lsm6dsm_sensor_type sensor;
  sns_sensor_api* sensor_api;
  sns_sensor_instance_api* instance_api;
} lsm6dsm_sensors;


/** Global const tables */
extern const float lsm6dsm_accel_resolutions[];
extern const float lsm6dsm_gyro_resolutions[];
extern lsm6dsm_accel_range lsm6dsm_accel_ranges[];
extern lsm6dsm_gyro_range lsm6dsm_gyro_ranges[];
extern const float lsm6dsm_accel_range_min[];
extern const float lsm6dsm_accel_range_max[];
extern const float lsm6dsm_gyro_range_min[];
extern const float lsm6dsm_gyro_range_max[];
extern const lsm6dsm_sensors lsm6dsm_supported_sensors[MAX_SUPPORTED_SENSORS];


/** Functions shared by all LSM6DSM Sensors */
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
void lsm6dsm_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lsm6dsm_sensor_type sensor_type);

/**
 * set_client_request() Sensor API common between all LSM6DSM
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* lsm6dsm_set_client_request(sns_sensor *const this,
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
void lsm6dsm_publish_def_attributes(sns_sensor *const this);
void lsm6dsm_init_sensor_info(sns_sensor *const this,
                              sns_sensor_uid const *suid,
                              lsm6dsm_sensor_type sensor_type);
void lsm6dsm_process_suid_events(sns_sensor *const this);
sns_rc lsm6dsm_process_registry_events(sns_sensor *const this);
void lsm6dsm_discover_hw(sns_sensor *const this);
void lsm6dsm_update_siblings(sns_sensor *const this);
sns_sensor_uid const* lsm6dsm_get_sensor_uid(sns_sensor const *const this);
bool lsm6dsm_sensor_write_output_to_registry(sns_sensor *const this,
                                             sns_sensor_instance *const instance);

sns_rc lsm6dsm_accel_init(sns_sensor *const this);
sns_rc lsm6dsm_gyro_init(sns_sensor *const this);
sns_rc lsm6dsm_motion_detect_init(sns_sensor *const this);
sns_rc lsm6dsm_sensor_temp_init(sns_sensor *const this);
sns_rc lsm6dsm_accel_deinit(sns_sensor *const this);
sns_rc lsm6dsm_gyro_deinit(sns_sensor *const this);
sns_rc lsm6dsm_motion_detect_deinit(sns_sensor *const this);
sns_rc lsm6dsm_sensor_temp_deinit(sns_sensor *const this);

