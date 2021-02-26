 /********************************************************************************
* Copyright (c) 2017, Vishay Capella Microsystems
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Vishay Capella Microsystems nor the
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
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*******************************************************************************/

#pragma once

#include "sns_data_stream.h"
#include "sns_cm3526_hal.h"
#include "sns_island_service.h"
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"

#include "sns_diag_service.h"
#include "sns_cm3526_sensor_instance.h"
#include "sns_math_util.h"

#include "sns_registry_util.h"

#define UV_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xea, 0x05, 0x04, 0x7d, 0x39, 0x1f, 0x4e, 0xf3,  \
        0xaa, 0x45, 0x3c, 0xf8, 0xa2, 0x43, 0x09, 0x04  \
      }  \
  }

/** Forward Declaration of UV Sensor API */
sns_sensor_api cm3526_uv_sensor_api;

/**
 * CM3526 ODR (Hz) definitions
 */
#define CM3526_ODR                  5.0

/**
 * UV sensor ranges
 */
#define CM3526_UV_RANGE_MIN         (0.0)
#define CM3526_UV_RANGE_MAX         (6553.0)

/**
 * UV sensor resolutions
 */
#define CM3526_UV_RESOLUTION        (0.1)

/** Supported opertating modes */
#define CM3526_NORMAL               "NORMAL"

/** Power rail timeout States for the CM3526 Sensors.*/
typedef enum
{
  CM3526_POWER_RAIL_PENDING_NONE,
  CM3526_POWER_RAIL_PENDING_INIT,
  CM3526_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} cm3526_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct cm3526_common_state
{
  sns_sensor_uid          reg_suid;
  sns_sensor_uid          timer_suid;

  cm3526_com_port_info    com_port_info;

  sns_rail_config         rail_config;
  sns_power_rail_state    registry_rail_on_state;

  bool                    hw_is_present;
  uint16_t                who_am_i;

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

} cm3526_common_state;

typedef struct cm3526_state
{
  cm3526_common_state     common;	
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;  
  sns_sync_com_port_service *scp_service;
  
  cm3526_sensor_type      sensor;
  sns_sensor_uid          my_suid;

  bool                    sensor_client_present;

  cm3526_power_rail_pending_state    power_rail_pend_state;

    // sensor configuration 
  bool is_dri;
  int64_t hardware_id;
  bool supports_sync_stream;
  uint8_t resolution_idx;

  // factory calibration
  bool registry_fac_cal_received;
  cm3526_uv_fac_cal uv_fac_cal;

  size_t                  encoded_event_len;
} cm3526_state;

/** Functions shared by all CM3526 Sensors */
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
void cm3526_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   cm3526_sensor_type sensor_type);

/**
 * Sends a request to the SUID Sensor to get SUID of a dependent
 * Sensor.
 *
 * @param[i] this          Sensor reference
 * @param[i] data_type     data_type of dependent Sensor
 * @param[i] data_type_len Length of the data_type string
 */
void cm3526_send_suid_req(sns_sensor *this, char *const data_type,
                           uint32_t data_type_len);

/**
 * Processes events from SUID Sensor.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void cm3526_process_suid_events(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all CM3526 Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc cm3526_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all CM3526
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* cm3526_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);

sns_rc cm3526_uv_init(sns_sensor *const this);
sns_rc cm3526_uv_deinit(sns_sensor *const this);

