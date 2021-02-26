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
*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*******************************************************************************/
#pragma once


#include "sns_data_stream.h"
#include "sns_bmp285_hal.h"
#if BMP_CONFIG_ENABLE_ISLAND_MODE
#include "sns_island_service.h"
#endif
#include "sns_pwr_rail_service.h"
#include "sns_sensor.h"
#include "sns_timer.pb.h"
#include "sns_bmp285_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_suid_util.h"
#include "sns_diag_service.h"
#include "sns_sync_com_port_service.h"

#include "sns_math_util.h"
#include "sns_printf.h"
#include "sns_registry_util.h"
#include "sns_bmp285_config.h"
#define PRESSURE_SUID \
{  \
    .sensor_uid =  \
    {  \
        0xb1, 0x0c, 0xd5, 0xe4, 0x0d, 0xe4, 0x4e, 0x5c,  \
        0xa3, 0x64, 0xba, 0xbb, 0x95, 0x29, 0x57, 0x00  \
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

#define BME_TYPE_SIZE_TEMPERATURE   12
#define BME_TYPE_SIZE_HUMIDITY      9



#define BMP_CONFIG_DFT_LOG_LEVEL SNS_LOW

#if BMP_CONFIG_ENABLE_DEBUG
#define BMP_SENSOR_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMP_CONFIG_DFT_LOG_LEVEL) { \
            SNS_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#define BMP_INST_LOG(LOG_LEVEL, this, arg...) { \
    if (NULL != this) { \
        if (SNS_##LOG_LEVEL >= BMP_CONFIG_DFT_LOG_LEVEL) { \
            SNS_INST_PRINTF(LOG_LEVEL, this, ##arg); \
        } \
    } \
}

#else
#define BMP_SENSOR_LOG(LOG_LEVEL, this, arg...)
#define BMP_INST_LOG(LOG_LEVEL, this, arg...)
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







#define BMP285_SENSOR_PRESSURE_RESOLUTION        (0.0016)
#define BMP285_SENSOR_PRESSURE_RANGE_MIN         (300.0f)
#define BMP285_SENSOR_PRESSURE_RANGE_MAX         (1100.0f)
#define BMP285_SENSOR_PRESSURE_LOW_POWER_CURRENT  (3)
#define BMP285_SENSOR_PRESSURE_NORMAL_POWER_CURRENT  (720)
#define BMP285_SENSOR_PRESSURE_SLEEP_CURRENT  (0)
#define BMP285_SENSOR_TEMPERATURE_RESOLUTION  (0.01)
#define BMP285_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0f)
#define BMP285_SENSOR_TEMPERATURE_RANGE_MAX  (85.0f)
#define BMP285_SENSOR_TEMPERATURE_LOW_POWER_CURRENT  (3)
#define BMP285_SENSOR_TEMPERATURE_NORMAL_POWER_CURRENT  (325)
#define BMP285_SENSOR_TEMPERATURE_SLEEP_CURRENT  (0)

/** Forward Declaration of Pressure Sensor API */
extern sns_sensor_api bmp285_pressure_sensor_api;

/** Forward Declaration of temperature Sensor API */
extern sns_sensor_api bmp285_temperature_sensor_api;
/**
 * BME ODR definitions
 */
#define BME_ODR_0                 (0.0)
#define BME_ODR_1                 (1.0)
#define BME_ODR_5                 (5.0)
#define BME_ODR_10                (10.0)
#define BME_ODR_25                (25.0)

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
} bmp285_power_rail_pending_state;

/** Interrupt Sensor State. */

typedef struct bmp285_common_state
{
  bmp285_com_port_info   com_port_info;
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

  // placement
  bool                    registry_placement_received;
  float                   placement[12];
  struct bmp280_calib_param_t  calib_param;/**<calibration data*/

} bmp285_common_state;

typedef struct bmp285_state
{
  bmp285_common_state    common;
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_pwr_rail_service    *pwr_rail_service;
  sns_diag_service        *diag_service;
  #if BMP_CONFIG_ENABLE_ISLAND_MODE
  sns_island_service      *island_service;
  #endif
  sns_sync_com_port_service *scp_service;

  bmp285_sensor_type     sensor;
  sns_sensor_uid          my_suid;

  bool                    sensor_client_present;

  bmp285_power_rail_pending_state power_rail_pend_state;

  // sensor configuration
  bool is_dri;
  int64_t hardware_id;
  bool supports_sync_stream;
  uint8_t resolution_idx;
  float chosen_sample_rate;
  float chosen_sample_rate_old;
  // factory calibration
  bool                    registry_fac_cal_received;
  float                   fac_cal_bias[1];
  float                   fac_cal_scale[1];
  uint32_t                fac_cal_version;


  size_t                  encoded_event_len;

} bmp285_state;

/** Functions shared by all BMP285 Sensors */
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
void bmp285_reval_instance_config(sns_sensor *this,
        sns_sensor_instance *instance, bmp285_sensor_type sensor_type);
sns_rc bmp285_com_read_wrapper(sns_sync_com_port_service *scp_service,sns_sync_com_port_handle *port_handle,
        uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
        uint32_t *xfer_bytes);
sns_rc bmp285_com_write_wrapper(sns_sync_com_port_service *scp_service,sns_sync_com_port_handle *port_handle,
        uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
        uint32_t *xfer_bytes, bool save_write_time);
s8 bmp280_set_power_mode(bmp285_instance_state *state, u8 v_power_mode_u8);
sns_sensor_instance* bmp285_sensor_set_client_request(sns_sensor * const this,
        struct sns_request const *exist_request,
        struct sns_request const *new_request,
        bool remove);
sns_rc bmp285_sensor_notify_event(sns_sensor * const this);
sns_rc bmp285_pressure_init(sns_sensor * const this);
sns_rc bmp285_pressure_deinit(sns_sensor * const this);
sns_rc bmp285_temperature_init(sns_sensor * const this);
sns_rc bmp285_temperature_deinit(sns_sensor * const this);
sns_rc bmp285_get_who_am_i(
  sns_sync_com_port_service *scp_service,
  sns_sync_com_port_handle *port_handle,
  uint8_t *buffer);
void bmp285_common_init(sns_sensor *const this);
// <registry>
#if !BMP_CONFIG_ENABLE_REGISTRY
void sns_bmp_registry_def_config(sns_sensor *const this);
#endif
// </registry>

