#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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

#include <stdint.h>
#include "sns_sensor.h"
#include "sns_attribute_service.h"
#include "sns_data_stream.h"
#include "sns_sensor_uid.h"
#include "sns_pwr_rail_service.h"
#include "sns_sx931x_hal.h"

#include "sns_sx931x_sensor_instance.h"
#include "sns_math_util.h"
#include "sns_diag_service.h"
#include "sns_sync_com_port_service.h"

#include "sns_timer.pb.h"
#include "sns_printf.h"
#include "sns_registry_util.h"



#define SAR_SUID_0 \
{  \
    .sensor_uid =  \
    {  \
        0x25, 0xb5, 0x43, 0x2a, 0xb3, 0x3a, 0x41, 0xdf,  \
        0x82, 0x33, 0x5d, 0xe9, 0x2d, 0x54, 0xeb, 0xd5  \
    }  \
}

#if DUAL_SENSOR_SUPPORT

#define SAR_SUID_1 \
{  \
    .sensor_uid =  \
    {  \
        0x78, 0x12, 0xb4, 0x95, 0x24, 0x78, 0x45, 0x9f, \
        0x81, 0x2a, 0xe7, 0xfd, 0x9b, 0x3f, 0x40, 0xeb  \
    }  \
}
#endif



#define NAME    "sx931x"

#define NUM_OF_RAILS                   1
#define RAIL_1                         "/pmic/client/sensor_vddio"



#define SX931x_SENSOR_SAR_RESOLUTION        (1)
#define SX931x_SENSOR_SAR_RANGE_MIN         (0)
#define SX931x_SENSOR_SAR_RANGE_MAX         (65536)
#define SX931x_SENSOR_SAR_LOW_POWER_CURRENT  (3)
#define SX931x_SENSOR_SAR_NORMAL_POWER_CURRENT  (300)
#define SX931x_SENSOR_SAR_SLEEP_CURRENT  (0)
#define SX931x_SENSOR_TEMPERATURE_RESOLUTION  (1)
#define SX931x_SENSOR_TEMPERATURE_RANGE_MIN  (0)
#define SX931x_SENSOR_TEMPERATURE_RANGE_MAX  (65536)
#define SX931x_SENSOR_TEMPERATURE_LOW_POWER_CURRENT  (3)
#define SX931x_SENSOR_TEMPERATURE_NORMAL_POWER_CURRENT  (200)
#define SX931x_SENSOR_TEMPERATURE_SLEEP_CURRENT  (0)

/** Forward Declaration of Pressure Sensor API */
sns_sensor_api sx931x_sar_sensor_api;

/**
 * BME ODR definitions
 */
#define SMT_ODR_0                 (0.0)
#define SMT_ODR_1                 (1.0)
#define SMT_ODR_5                 (5.0)
#define SMT_ODR_10                (10.0)
#define SMT_ODR_25                (25.0)

#define SMT_LPM              "LPM"
#define SMT_HIGH_PERF        "HIGH_PERF"
#define SMT_NORMAL           "NORMAL"



/** Power rail timeout States for the  Sensors.*/
typedef enum
{
    POWER_RAIL_PENDING_NONE,
    POWER_RAIL_PENDING_INIT,
    POWER_RAIL_PENDING_SET_CLIENT_REQ,
    POWER_RAIL_PENDING_CREATE_DEPENDENCY,
} sx931x_power_rail_pending_state;

/** Interrupt Sensor State. */
/*********************************************************
 * sensor state
 *********************************************************/
typedef struct sx931x_state
{
    /* data stream */
    sns_data_stream              *reg_data_stream;
    sns_data_stream              *fw_stream;
    sns_data_stream              *timer_stream;
    /* sensor suid */
    sns_sensor_uid               reg_suid;
    sns_sensor_uid               irq_suid;
    sns_sensor_uid               timer_suid;
    sns_sensor_uid               acp_suid; // Asynchronous COM Port for the stream data fetching
    sns_sensor_uid               my_suid;
    sns_sensor_uid               dae_suid; // Data Acquisition Engine
    sx931x_sensor_type           sensor;   // used to identify the sensor state involved with
    /* COM port information */
    sx931x_com_port_info         com_port_info;
    sns_interrupt_req            irq_config;
    /*  Power rail */
    sns_pwr_rail_service         *pwr_rail_service;
    sns_rail_config              rail_config;
    sns_power_rail_state         registry_rail_on_state;
    bool                         hw_is_present;
    bool                         sensor_client_present;
    /* state machine flag */
    sx931x_power_rail_pending_state power_rail_pend_state;
    // debug
    // sensor configuration
    bool 			 is_dri;
    bool 			 supports_sync_stream;
    uint8_t 			 resolution_idx;
    uint32_t                     hw_idx;
    int64_t 			 hardware_id;
    // registry sensor config
    bool 			 registry_cfg_received;
    sns_registry_phy_sensor_cfg  registry_cfg;
    // registry sensor platform config
    bool 			 registry_pf_cfg_received;
    sns_registry_phy_sensor_pf_cfg registry_pf_cfg;
    // placement
    bool                    	 registry_placement_received;
    float                   	 placement[12];

    uint16_t                     who_am_i;
    sns_diag_service             *diag_service;
    sns_sync_com_port_service    *scp_service;
    size_t                       encoded_event_len;    
} sx931x_state;


/*********************************************************
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 ********************************************************/
void sx931x_reval_instance_config(sns_sensor *this, sns_sensor_instance *instance, sx931x_sensor_type sensor_type);
void sns_see_sx931x_send_suid_req(sns_sensor *this, char *const data_type, uint32_t data_type_len);
void sx931x_sensor_process_suid_events(sns_sensor *const this);
sns_rc sx931x_com_read_wrapper(sns_sync_com_port_service *scp_service,
                               sns_sync_com_port_handle *port_handle,
                               uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
                               uint32_t *xfer_bytes);
sns_rc sx931x_com_write_wrapper(sns_sync_com_port_service *scp_service, sns_sync_com_port_handle *port_handle,
                                uint32_t reg_addr, uint8_t *buffer, uint32_t bytes,
                                uint32_t *xfer_bytes, bool save_write_time);
int8_t sx932x_set_power_mode(sx931x_instance_state *state, uint8_t v_power_mode_u8);
sns_sensor_instance *sns_see_sx931x_sensor_set_client_request(sns_sensor *const this,
        struct sns_request const *exist_request,
        struct sns_request const *new_request,
        bool remove);
sns_rc sns_see_sx931x_sar_init(sns_sensor *const this);
sns_rc sns_see_sx931x_sensor_notify_event(sns_sensor *const this);
sns_rc sns_see_sx931x_sar_deinit(sns_sensor *const this);
//sns_rc sns_see_sx931x_temperature_init(sns_sensor *const this);
//sns_rc sns_see_sx931x_temperature_deinit(sns_sensor *const this);
sns_rc sx931x_get_who_am_i(sns_sync_com_port_service *scp_service, sns_sync_com_port_handle *port_handle, uint8_t *buffer);
