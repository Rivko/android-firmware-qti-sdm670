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
#include "sns_data_stream.h"
#include "sns_bma2x2_hal.h"
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

#include "sns_diag_service.h"
#include "sns_bma2x2_sensor_instance.h"
#include "sns_math_util.h"

#include "sns_registry_util.h"
#include "sns_bma2x2_cfg.h"

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
#include "sns_island_service.h"
#endif

#define BMA2X2_ACCEL_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xD7, 0x23, 0xBC, 0x29, 0x52, 0x52, 0x4D, 0x21,  \
        0x90, 0x0A, 0x87, 0x78, 0x42, 0x8B, 0x7A, 0xED \
      }  \
  }

#define BMA2X2_MOTION_DETECT_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x69, 0x89, 0x7F, 0x37, 0x33, 0x4A, 0x4E, 0x24,  \
        0xAD, 0xFE, 0x5B, 0xED, 0xD0, 0x60, 0xAA, 0x66\
      }  \
  }


#define BMA2X2_SENSOR_TEMPERATURE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xea, 0xc4, 0xcc, 0x53, 0x0f, 0x71, 0x40, 0xed,  \
        0x92, 0xc3, 0x99, 0x3f, 0x1a, 0xf4, 0x07, 0xaa   \
      }  \
  }

/** Forward Declaration of Accel Sensor API */
sns_sensor_api bma2x2_accel_sensor_api;

/** Forward Declaration of Motion Accel Sensor API */
sns_sensor_api bma2x2_motion_detect_sensor_api;

/** Forward Declaration of Sensor Temperature Sensor API */
sns_sensor_api bma2x2_sensor_temp_sensor_api;




#define BST_RV_ASSERT_RT(TARGET_VAL, EXP_VAL) \
  { \
    if (EXP_VAL != TARGET_VAL) \
    return TARGET_VAL;\
  }

#define BST_ARRARY_SIZE(array) (sizeof(array)/sizeof(array[0]))


/**
 * BMA2X2 ODR (Hz) definitions
 */
#define BMA2X2_ODR_0                 0.1
#define BMA2X2_ODR_16                15.62
#define BMA2X2_ODR_32                31.26
#define BMA2X2_ODR_63                62.5
#define BMA2X2_ODR_125               125.0
#define BMA2X2_ODR_250               250.0
#define BMA2X2_ODR_500               500.0
#define BMA2X2_ODR_1000              1000.0
#define BMA2X2_ODR_2000              2000.0

#define BMA2X2_MAX_ODR               (BMA2X2_ODR_1000)
#define BMA2X2_MAX_SF_FIFO_ODR       (125.0f)
#define BMA2X2_MIN_ODR               (15.62f)


/**
 * Accelerometer ranges
 */
#define BMA2X2_ACCEL_RANGE_2G_MIN    (-2*G)
#define BMA2X2_ACCEL_RANGE_2G_MAX    (2*G)
#define BMA2X2_ACCEL_RANGE_4G_MIN    (-4*G)
#define BMA2X2_ACCEL_RANGE_4G_MAX    (4*G)
#define BMA2X2_ACCEL_RANGE_8G_MIN    (-8*G)
#define BMA2X2_ACCEL_RANGE_8G_MAX    (8*G)
#define BMA2X2_ACCEL_RANGE_16G_MIN   (-16*G)
#define BMA2X2_ACCEL_RANGE_16G_MAX   (16*G)

#define BMA2X2_ACCEL_DEFAULT_RESOLUTION   2
#define BMA2X2_ACCEL_RESOLUTION_MAX_IDX   3  /* +-16g */
#define BMA2X2_TEMP_RESOLUTION_MAX_IDX    0 /* ONLY 1 resolution */
/**
 * Accelerometer resolutions
 */
#define BMA2X2_ACCEL_RESOLUTION_2G    (0.061)
#define BMA2X2_ACCEL_RESOLUTION_4G    (0.122)
#define BMA2X2_ACCEL_RESOLUTION_8G    (0.244)
#define BMA2X2_ACCEL_RESOLUTION_16G   (0.488)

/**
 * BMA2X2 Sensor Temperature ODR (Hz) definitions
 */
#define BMA2X2_SENSOR_TEMP_ODR_ZERO     0.01
#define BMA2X2_SENSOR_TEMP_ODR_1        1.0
#define BMA2X2_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temperature resolution in deg Celsius/LSB
 */
#define BMA2X2_SENSOR_TEMPERATURE_RESOLUTION  (0.5)

/**
 * Sensor Temperature range in deg Celsius
 */
#define BMA2X2_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define BMA2X2_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)

/** Supported operating modes */
#define BMA2X2_LPM          "LPM"
#define BMA2X2_HIGH_PERF    "HIGH_PERF"
#define BMA2X2_NORMAL       "NORMAL"

/** Power rail timeout States for the BMA2X2 Sensors.*/
typedef enum
{
  BMA2X2_POWER_RAIL_PENDING_NONE,
  BMA2X2_POWER_RAIL_PENDING_INIT,
  BMA2X2_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} bma2x2_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct bma2x2_common_state
{

  bma2x2_com_port_info    com_port_info;
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

  // sensor version
  bma2x2_sensor_hw_version hw_ver;

} bma2x2_common_state;


typedef struct bma2x2_state
{
  bma2x2_common_state     common;
#if BMA2X2_CONFIG_ENABLE_REGISTRY
  sns_data_stream         *reg_data_stream;
#endif
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;

#if BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
  sns_island_service      *island_service;
#endif

  sns_sync_com_port_service *scp_service;

  bma2x2_sensor_type      sensor;
  sns_sensor_uid          my_suid;

  bool                    sensor_client_present;

  bma2x2_power_rail_pending_state    power_rail_pend_state;

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

} bma2x2_state;

/** Functions shared by all BMA2X2 Sensors */
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
void bma2x2_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   bma2x2_sensor_type sensor_type);

/**
 * notify_event() Sensor API common between all LSM6DS3 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc bma2x2_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all BMA2X2
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* bma2x2_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);
sns_rc bma2x2_accel_init(sns_sensor *const this);
sns_rc bma2x2_motion_detect_init(sns_sensor *const this);
sns_rc bma2x2_sensor_temp_init(sns_sensor *const this);
sns_rc bma2x2_accel_deinit(sns_sensor *const this);
sns_rc bma2x2_motion_detect_deinit(sns_sensor *const this);
sns_rc bma2x2_sensor_temp_deinit(sns_sensor *const this);

void bma2x2_common_init(sns_sensor * const this);



/**
 * discover the HW
 * @param this    sensor handler
 * @return        SNS_RC_SUCCESS  on success
 *                others value    the others cases
 */
bool bma2x2_discover_hw(sns_sensor *const this);

/**
 * update the sibling sensor
 * @param this    sensor handler
 */
void bma2x2_update_sibling_sensors(sns_sensor *const this);
void bma2x2_start_hw_detect_sequence(sns_sensor *const this);

/**
 * Validate the sensor resolution index
 * @param this  sensor handler
 */
void bma2x2_validate_resolution(sns_sensor const * this);

void bma2x2_publish_available(sns_sensor *const this);

// <power rail>
/**
 * start power rail timer
 * @param this
 * @param timeout_ticks
 * @param pwr_rail_pend_state
 */
void bma2x2_start_power_rail_timer(sns_sensor *const this,
                                       sns_time timeout_ticks,
                                       bma2x2_power_rail_pending_state pwr_rail_pend_state);
// </power rail>

// <registry>
#if BMA2X2_CONFIG_ENABLE_REGISTRY
void bma2x2_sensor_process_registry_event(sns_sensor *const this,
                                              sns_sensor_event *event);

/**
 *
 * @param this
 * @param instance
 * @param sensor
 */
void bma2x2_update_registry(sns_sensor *const this,
                              sns_sensor_instance *const instance,
                              bma2x2_sensor_type sensor);

/**
 * update the sensor state
 * @param this            sensor state handler
 * @param instance        instance handler
 */
void bma2x2_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance);
/**
 * request the registry request to parse the json file
 * @param this   sensor handler
 */
void bma2x2_request_registry(sns_sensor *const this);

#else
extern void sns_bma2x2_registry_def_config(sns_sensor *const this);
#endif

// </registry>
