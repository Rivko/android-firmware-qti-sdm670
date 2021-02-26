/*******************************************************************************
* Copyright (c) 2019, Bosch Sensortec GmbH
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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*******************************************************************************/
#pragma once


#include "sns_data_stream.h"
#include "sns_bmm150_hal.h"
#if BMM_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_timer.pb.h"
#include "sns_bmm150_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"
#include "sns_diag_service.h"
#include "sns_sync_com_port_service.h"
#include "bmm150_defs.h"
#include "sns_math_util.h"
#include "sns_printf.h"
#include "sns_registry_util.h"
#include "sns_bmm150_config.h"
#define MAG_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x92, 0x33, 0x64, 0x24, 0xe0, 0xa0, 0x47, 0x66,  \
        0xa9, 0x3f, 0x89, 0x12, 0xfd, 0x95, 0x0d, 0xc0  \
    }  \
}

#define TEMPERATURE_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x02, 0x68, 0x2a, 0x93, 0x17, 0xbc, 0x4d, 0xad, \
        0xbb, 0xe8, 0xfe, 0x1d, 0x2e, 0xe1, 0xd2, 0x2b  \
    }  \
}



#define BMM_CONFIG_DFT_LOG_LEVEL SNS_LOW

#if BMM_CONFIG_ENABLE_DEBUG
#define BMM_SENSOR_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMM_CONFIG_DFT_LOG_LEVEL) { \
            SNS_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#define BMM_INST_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMM_CONFIG_DFT_LOG_LEVEL) { \
            SNS_INST_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#else
#define BMM_SENSOR_LOG(LOG_LEVEL, this, arg...)
#define BMM_INST_LOG(LOG_LEVEL, this, arg...)
#endif

/**
 * delay time with ms
 */
#define BME_BUSY_DELAY(ms) sns_busy_wait((sns_convert_ns_to_ticks(ms * 1000 * 1000)))

/* bit operation */
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






#define BMM150_SENSOR_MAG_RESOLUTION        (0.3)
#define BMM150_SENSOR_MAG_RANGE_MIN         (-2500.0f)
#define BMM150_SENSOR_MAG_RANGE_MAX         (2500.0f)
#define BMM150_SENSOR_MAG_LOW_POWER_CURRENT  (3)
#define BMM150_SENSOR_MAG_NORMAL_POWER_CURRENT  (500)
#define BMM150_SENSOR_MAG_SLEEP_CURRENT  (0)
#define BMM150_SENSOR_TEMPERATURE_RESOLUTION  (0.01)
#define BMM150_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0f)
#define BMM150_SENSOR_TEMPERATURE_RANGE_MAX  (85.0f)
#define BMM150_SENSOR_TEMPERATURE_LOW_POWER_CURRENT  (3)
#define BMM150_SENSOR_TEMPERATURE_NORMAL_POWER_CURRENT  (500)
#define BMM150_SENSOR_TEMPERATURE_SLEEP_CURRENT  (0)

/** Forward Declaration of Pressure Sensor API */
extern sns_sensor_api bmm150_mag_sensor_api;

/** Forward Declaration of temperature Sensor API */
extern sns_sensor_api bmm150_temperature_sensor_api;
/**
 * BME ODR definitions
 */
#define BME_ODR_0                 (0.0)
#define BME_ODR_1                 (1.0)
#define BME_ODR_5                 (5.0)
#define BME_ODR_8                 (8.0)
#define BME_ODR_10                (10.0)
#define BME_ODR_15                (15.0)
#define BME_ODR_25                (25.0)
#define BME_ODR_50                (50.0)
#define BME_ODR_100                (100.0)

#define BME_LPM              "LPM"
#define BME_HIGH_PERF        "HIGH_PERF"
#define BME_NORMAL           "NORMAL"

/**
 * definition for the bmp285 API
 */
#define BME_SPI_INTERFACE                1
#define BME_I2C_INTERFACE                2

/** Power rail timeout States for the BME Sensors.*/
typedef enum
{
  BME_POWER_RAIL_PENDING_NONE,
  BME_POWER_RAIL_PENDING_INIT,
  BME_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  BME_POWER_RAIL_PENDING_CREATE_DEPENDENCY,
} bmm150_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct bmm150_common_state
{
  bmm150_com_port_info   com_port_info;

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
  struct bmm150_trim_registers trim_data;/**<calibration data*/

} bmm150_common_state;

typedef struct bmm150_state
{
  bmm150_common_state    common;
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;
#if BMM_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service      *island_service;
#endif
  sns_sync_com_port_service *scp_service;

  bmm150_sensor_type     sensor;
  sns_sensor_uid          my_suid;

  bool                    sensor_client_present;

  bmm150_power_rail_pending_state power_rail_pend_state;

  // sensor configuration
  bool is_dri;
  int64_t hardware_id;
  bool supports_sync_stream;
  uint8_t resolution_idx;
  float chosen_sample_rate;
  float chosen_sample_rate_old;
  // factory calibration
  bool                    registry_fac_cal_received;
  matrix3                 fac_cal_corr_mat;
  float                   fac_cal_bias[TRIAXIS_NUM];
  float                   fac_cal_scale[TRIAXIS_NUM];
  uint32_t                fac_cal_version;


  size_t                  encoded_event_len;

} bmm150_state;

/** Functions shared by all BMM150 Sensors */
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
void bmm150_reval_instance_config(sns_sensor *this,
        sns_sensor_instance *instance, bmm150_sensor_type sensor_type);
sns_rc bmm150_com_read_wrapper(sns_sync_com_port_service *scp_service,sns_sync_com_port_handle *port_handle,
        uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
        uint32_t *xfer_bytes);
sns_rc bmm150_com_write_wrapper(sns_sync_com_port_service *scp_service,sns_sync_com_port_handle *port_handle,
        uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
        uint32_t *xfer_bytes, bool save_write_time);
sns_sensor_instance* bmm150_sensor_set_client_request(sns_sensor * const this,
        struct sns_request const *exist_request,
        struct sns_request const *new_request,
        bool remove);
sns_rc bmm150_sensor_notify_event(sns_sensor * const this);
sns_rc bmm150_mag_init(sns_sensor * const this);
sns_rc bmm150_mag_deinit(sns_sensor * const this);
sns_rc bmm150_temperature_init(sns_sensor * const this);
sns_rc bmm150_temperature_deinit(sns_sensor * const this);
sns_rc bmm150_get_who_am_i(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint8_t *buffer);
void bmm150_common_init(sns_sensor *const this);
// <registry>
#if !BMM_CONFIG_ENABLE_REGISTRY
void sns_bmm_registry_def_config(sns_sensor *const this);
#endif
// </registry>

