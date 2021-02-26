#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 ******************************************************************************/
#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_math_util.h"
#include "sns_pwr_rail_service.h"
#include "sns_registry_util.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_shtw2_hal.h"
#include "sns_shtw2_sensor_instance.h"

#define SENSOR_PRINTF(diag, sensor, prio, filename, line, fmt, ...) \
  do { \
    if (diag != NULL && diag->api != NULL) { \
      diag->api->sensor_printf(diag, sensor, prio, filename, line, fmt, ##__VA_ARGS__); \
    } \
  } while (0)

#define TEMPERATURE_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xec, 0x0e, 0x17, 0x70, 0x6d, 0x31, 0x43, 0xa8,  \
        0xbc, 0xcc, 0xd2, 0x8e, 0x38, 0xc2, 0x3f, 0x92  \
      }  \
  }

#define HUMIDITY_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x57, 0x00, 0x20, 0x6d, 0x3f, 0x9d, 0x42, 0x66,  \
        0x94, 0x56, 0xae, 0x7a, 0x8a, 0x99, 0xaf, 0xf2  \
      }  \
  }

sns_sensor_api shtw2_temperature_sensor_api;
sns_sensor_api shtw2_humidity_sensor_api;

/**
 * SHTW2 ODR definitions
 */
#define SHTW2_ODR_0                    0.0f
#define SHTW2_ODR_1                    1.0f
#define SHTW2_ODR_5                    5.0f
#define SHTW2_ODR_10                   10.0f

/**
 * Temperature and humidity ranges
 */
#define SHTW2_TEMPERATURE_RANGE_MIN    -30    /* in degree celsius */
#define SHTW2_TEMPERATURE_RANGE_MAX    100    /* in degree celsius */
#define SHTW2_HUMIDITY_RANGE_MIN       0      /* in % relative humidty */
#define SHTW2_HUMIDITY_RANGE_MAX       100    /* in % relative humidty */

/**
 * Temperature and humidity resolutions
 */
#define SHTW2_TEMPERATURE_RESOLUTION   0.01
#define SHTW2_HUMIDITY_RESOLUTION      0.01

/**
 * Supported operating modes
 */
#define SHTW2_NORMAL                   "NORMAL"

/**
 * Power rail timeout states for the SHTW2 sensor.
 */
typedef enum
{
  SHTW2_POWER_RAIL_PENDING_NONE,
  SHTW2_POWER_RAIL_PENDING_INIT,
  SHTW2_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} shtw2_power_rail_pending_state;

/**
 * Sensor state.
 */
typedef struct shtw2_state
{
  sns_data_stream                *reg_data_stream;
  sns_data_stream                *fw_stream;
  sns_data_stream                *timer_stream;
  sns_sensor_uid                 timer_suid;
  sns_sensor_uid                 reg_suid;
  shtw2_sensor_type              sensor;
  sns_sensor_uid                 my_suid;
  shtw2_com_port_info            com_port_info;

  sns_pwr_rail_service           *pwr_rail_service;
  sns_rail_config                rail_config;
  sns_power_rail_state           registry_rail_on_state;

  bool                           hw_is_present;

  shtw2_power_rail_pending_state power_rail_pend_state;

  sns_diag_service               *diag_service;
  sns_sync_com_port_service      *scp_service;
  size_t                         encoded_event_len;

  /* Registry sensor platform config */
  bool                           registry_pf_cfg_received;
  sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

  /* Registry sensor config */
  bool                           registry_cfg_received;
  sns_registry_phy_sensor_cfg    registry_cfg;

  /* Sensor configuration read from registry */
  bool                           is_dri;
  int64_t                        hardware_id;
  bool                           supports_sync_stream;
  uint8_t                        resolution_idx;

  /* Placement */
  bool                           registry_placement_received;
  float                          placement[12];

  /* Factory calibration */
  bool                    registry_fac_cal_received;
  float                   fac_cal_bias[1];
  float                   fac_cal_scale[1];

  /* Threshold [humidity, temperature] */
  bool                           registry_threshold_received;
  float                          threshold[2];

} shtw2_state;

/* Functions shared by all SHTW2 sensors. */
/**
 * This function parses the client_request list per sensor and
 * determines final config for the sensor instance.
 *
 * @param[i] this          sensor reference
 * @param[i] instance      sensor instance to config
 * @param[i] sensor_type   sensor type
 *
 * @return none
 */
void shtw2_reval_instance_config(sns_sensor *this,
                                 sns_sensor_instance *instance,
                                 shtw2_sensor_type sensor_type);

/**
 * Sends a request to the SUID sensor to get SUID of a dependent
 * sensor.
 *
 * @param[i] this          sensor reference
 * @param[i] data_type     data_type of dependent sensor
 * @param[i] data_type_len length of the data_type string
 */
void shtw2_send_suid_req(sns_sensor *this, char *const data_type,
                         uint32_t data_type_len);

/**
 * Processes events from SUID sensor.
 *
 * @param[i] this   sensor reference
 *
 * @return none
 */
void shtw2_process_suid_events(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all SHTW2 sensors.
 *
 * @param this    sensor reference
 *
 * @return sns_rc
 */
sns_rc shtw2_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all SHTW2
 * sensors.
 *
 * @param this            sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* shtw2_set_client_request(sns_sensor *const this,
                                              struct sns_request const *exist_request,
                                              struct sns_request const *new_request,
                                              bool remove);

sns_rc shtw2_temperature_init(sns_sensor *const this);
sns_rc shtw2_humidity_init(sns_sensor *const this);
sns_rc shtw2_temperature_deinit(sns_sensor *const this);
sns_rc shtw2_humidity_deinit(sns_sensor *const this);
