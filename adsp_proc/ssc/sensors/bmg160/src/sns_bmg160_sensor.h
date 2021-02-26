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
#include "sns_data_stream.h"
#include "sns_bmg160_hal.h"
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"

#include "sns_diag_service.h"
#include "sns_bmg160_sensor_instance.h"
#include "sns_math_util.h"

#include "sns_registry_util.h"

#if BMG160_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif

#define GYRO_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xbe, 0x09, 0x01, 0xe5, 0xce, 0xe5, 0x40, 0xd5,  \
        0xbe, 0xb8, 0x3d, 0x3c, 0x16, 0xe1, 0xdc, 0xc7   \
      }  \
  }

#define SENSOR_TEMPERATURE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xcb, 0x42, 0x32, 0x9b, 0x56, 0x30, 0x41, 0x10,  \
        0xb8, 0xf8, 0x94, 0x11, 0x28, 0x66, 0xfb, 0xcd   \
      }  \
  }

/** Forward Declaration of Gyro Sensor API */
extern sns_sensor_api bmg160_gyro_sensor_api;

/** Forward Declaration of Sensor Temperature Sensor API */
extern sns_sensor_api bmg160_sensor_temp_sensor_api;


#define BST_RV_ASSERT_RT(TARGET_VAL, EXP_VAL) \
  { \
    if (EXP_VAL != TARGET_VAL) \
    return TARGET_VAL;\
  }

#define BST_ARRARY_SIZE(array) (sizeof(array)/sizeof(array[0]))

#define BST_ASSERT(expval, targetval) { \
  if (expval != targetval) { \
    return targetval; \
  } \
}

/**
 * BMA2X2 Sensor Temperature ODR (Hz) definitions
 */
#define BMG160_SENSOR_TEMP_ODR_ZERO     0.01
#define BMG160_SENSOR_TEMP_ODR_1        1.0
#define BMG160_SENSOR_TEMP_ODR_5        5.0

/**
 * Sensor Temperature resolution in deg Celsius/LSB
 */
#define BMG160_SENSOR_TEMPERATURE_RESOLUTION  (0.5)

/**
 * Sensor Temperature range in deg Celsius
 */
#define BMG160_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define BMG160_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)

/** Supported operating modes */
#define BMG160_LPM          "LPM"
#define BMG160_NORMAL       "NORMAL"

/** Power rail timeout States for the BMA2X2 Sensors.*/
typedef enum
{
  BMG160_POWER_RAIL_PENDING_NONE,
  BMG160_POWER_RAIL_PENDING_INIT,
  BMG160_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} bmg160_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct bmg160_common_state
{
  bmg160_com_port_info    com_port_info;
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

} bmg160_common_state;

typedef struct bmg160_state
{
  bmg160_common_state     common;
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;
  sns_sensor              *owner;

#if BMG160_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service      *island_service;
#endif

  sns_sync_com_port_service *scp_service;

  bmg160_sensor_type      sensor;

  bmg160_power_rail_pending_state    power_rail_pend_state;

  uint8_t                 resolution_idx;

  bool                    sensor_client_present;
  bool                    is_dri;
  bool                    supports_sync_stream;
  bool                    registry_fac_cal_received;
  uint32_t                fac_cal_version;
  size_t                  encoded_event_len;
  int64_t                 hardware_id;

  matrix3                 fac_cal_corr_mat;
  float                   fac_cal_bias[TRIAXIS_NUM];
  float                   fac_cal_scale[TRIAXIS_NUM];

  sns_sensor_uid          my_suid;
} bmg160_state;

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
void bmg160_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   bmg160_sensor_type sensor_type);

/**
 * notify_event() Sensor API common between all LSM6DS3 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc bmg160_sensor_notify_event(sns_sensor *const this);

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
sns_sensor_instance* bmg160_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);
sns_rc bmg160_gyro_init(sns_sensor *const this);
sns_rc bmg160_sensor_temp_init(sns_sensor *const this);
sns_rc bmg160_gyro_deinit(sns_sensor *const this);
sns_rc bmg160_sensor_temp_deinit(sns_sensor *const this);


bool bmg160_discover_hw(sns_sensor *const this);

void bmg160_validate_resolution(sns_sensor * const this);

void bmg160_publish_available(sns_sensor *const this);
void bmg160_update_sibling_sensors(sns_sensor *const this);
void bmg160_start_hw_detect_sequence(sns_sensor *const this);
void bmg160_start_power_rail_timer(sns_sensor *const this,
                    sns_time timeout_ticks,
                    bmg160_power_rail_pending_state pwr_rail_pend_state);
void bmg160_common_init(sns_sensor *const this);

#if BMG160_CONFIG_ENABLE_REGISTRY
void bmg160_sensor_process_registry_event(sns_sensor *const this,
                                           sns_sensor_event *event);
void bmg160_request_registry(sns_sensor *const this);
void bmg160_update_registry(sns_sensor *const this,
        sns_sensor_instance *const instance, bmg160_sensor_type sensor);
void bmg160_update_sensor_state(sns_sensor *const this,
        sns_sensor_instance *const instance);
#else
void sns_bmg160_registry_def_config(sns_sensor *const this);
#endif
