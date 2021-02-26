//==============================================================================
// MODULE        : sns_rpr0521rs_sensor.h
// FUNCTION      : RPR0521RS Sensor implementation.
// AUTHOR        : Masafumi Seike
// MAKING DATE   : Nov/16/2017
// MODIFIED      : Ryosuke Yamamoto
// MODIFIED DATE : Dec/4/2018
// REMARKS       :
// COPYRIGHT     : Copyright (C) 2017 - 2018 - ROHM CO.,LTD.
//               : Redistribution and use in source and binary forms, with or
//               : without modification, are permitted provided that the
//               : following conditions are met:
//               : 1. Redistributions of source code must retain the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer.
//               : 2. Redistributions in binary form must reproduce the above
//               : copyright notice, this list of conditions and the following
//               : disclaimer in the documentation and/or other materials
//               : provided with the distribution.
//               : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//               : contributors may be used to endorse or promote products
//               : derived from this software without specific prior written
//               : permission.
//               : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//               : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//               : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//               : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//               : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//               : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//               : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//               : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//               : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//               : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//               : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//               : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//               : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//               : SUCH DAMAGE.
//==============================================================================
#ifndef SNS_RPR0521RS_SENSOR_H_
#define SNS_RPR0521RS_SENSOR_H_ (1)

#include <math.h>
#include "sns_interrupt.pb.h"
#include "sns_diag_service.h"
#include "sns_registry_util.h"
#include "sns_pwr_rail_service.h"
#include "sns_time.h"
#include "sns_sensor_uid.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sync_com_port_service.h"
#include "sns_island_service.h"
#include "sns_data_stream.h"
#include "sns_suid_util.h"
#include "sns_rpr0521rs_sensor.h"
#include "sns_rpr0521rs_hal.h"

typedef enum {
    RPR0521RS_ALS = 0x1,
    RPR0521RS_PS  = 0x2,
} RPR0521RS_SENSOR_TYPE;

//==============================================================================
// config file definition
//==============================================================================
#define RPR0521RS_CONFIG_ALS                 ("rpr0521rs_0.ambient_light.config")
#define RPR0521RS_CONFIG_PS                  ("rpr0521rs_0.proximity.config")
#define RPR0521RS_PLATFORM_CONFIG            ("rpr0521rs_0_platform.config")
#define RPR0521RS_PLATFORM_PLACEMENT         ("rpr0521rs_0_platform.placement")
#define RPR0521RS_PLATFORM_FAC_CAL_ALS       ("rpr0521rs_0_platform.ambient_light.fac_cal")
#define RPR0521RS_PLATFORM_FAC_CAL_INIT_ALS  ("rpr0521rs_0_platform.ambient_light.fac_cal_init")
#define RPR0521RS_PLATFORM_FAC_CAL_PS        ("rpr0521rs_0_platform.proximity.fac_cal")
#define RPR0521RS_PLATFORM_FAC_CAL_INIT_PS   ("rpr0521rs_0_platform.proximity.fac_cal_init")

//{13563903-A5D4-4AFD-D2EA-0E5B6F0CC420}
#define ALS_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x13, 0x56, 0x39, 0x03, 0xa5, 0xd4, 0x4a, 0xfd,  \
        0xd2, 0xea, 0x0e, 0x5b, 0x6f, 0x0c, 0xc4, 0x20  \
    }  \
}
//4b3ff652-6dd5-11e7-907b-a6006ad3dba0
#define PS_SUID \
{  \
    .sensor_uid =  \
    {  \
        0x4b, 0x3f, 0xf6, 0x52, 0x6d, 0xd5, 0x11, 0xe7,  \
        0x90, 0x7b, 0xa6, 0x00, 0x6a, 0xd3, 0xdb, 0xa0  \
    }  \
}

// Forward Declaration of ALS Sensor API
extern sns_sensor_api rpr0521rs_als_sensor_api;

// Forward Declaration of PS Sensor API
extern sns_sensor_api rpr0521rs_ps_sensor_api;

// ALS ODR (Hz)
#define RPR0521RS_SENSOR_ALS_ODR_0    ( 0.0F)
#define RPR0521RS_SENSOR_ALS_ODR_2P5  ( 2.5F)
#define RPR0521RS_SENSOR_ALS_ODR_10   (10.0F)

// ALS range
#define RPR0521RS_LUX_RANGE_X1_MIN    (    0.0F)
#define RPR0521RS_LUX_RANGE_X1_MAX    (43000.0F)
#define RPR0521RS_LUX_RANGE_X2_MIN    (    0.0F)
#define RPR0521RS_LUX_RANGE_X2_MAX    (30000.0F)
#define RPR0521RS_LUX_RANGE_X64_MIN   (    0.0F)
#define RPR0521RS_LUX_RANGE_X64_MAX   ( 1000.0F)
#define RPR0521RS_LUX_RANGE_X128_MIN  (    0.0F)
#define RPR0521RS_LUX_RANGE_X128_MAX  (  500.0F)

// ALS resolution
#define RPR0521RS_ALS_RESOLUTION_X1   (2.0F)
#define RPR0521RS_ALS_RESOLUTION_X2   (1.0F)
#define RPR0521RS_ALS_RESOLUTION_X64  (0.04F)
#define RPR0521RS_ALS_RESOLUTION_X128 (0.02F)

// ALS active current
#define RPR0521RS_ALS_ACTIVE_CURRENT  (90)
#define RPR0521RS_ALS_SLEEP_CURRENT   ( 1)

// PS active current
#define RPR0521RS_PS_ACTIVE_CURRENT   (90)
#define RPR0521RS_PS_SLEEP_CURRENT    ( 1)

// PS ODR (Hz)
#define RPR0521RS_SENSOR_PS_ODR_0   ( 0.0F)
#define RPR0521RS_SENSOR_PS_ODR_10  (10.0F)
#define RPR0521RS_SENSOR_PS_ODR_20  (20.0F)

#define RPR0521RS_ALS_CASE_PATTERN    (4U)

// Supported opertating modes
#define RPR0521RS_HIGH_PERF           ("HIGH_PERF")

#define RPR0521RS_OFF_TO_IDLE_NS            ( 100000000UL)  // 100 msec
#define RPR0521RS_POWER_RAIL_OFF_TIMEOUT_NS (1000000000ULL) //   1 sec

#define DRI_MODE     (1)
#define POLLING_MODE (0)

// Power rail timeout states
typedef enum {
    RPR0521RS_POWER_RAIL_PENDING_NONE,
    RPR0521RS_POWER_RAIL_PENDING_INIT,
    RPR0521RS_POWER_RAIL_PENDING_SET_CLIENT_REQ,
    RPR0521RS_POWER_RAIL_PENDING_OFF
} RPR0521RS_POWER_RAIL_PENDING_STATE;

// Interrupt Sensor State.
typedef struct {
  sns_com_port_config      com_config;
  sns_sync_com_port_handle *port_handle;

} RPR0521RS_COM_PORT_INFO;

// Interrupt Sensor State.
typedef struct {
    float_t d0[RPR0521RS_ALS_CASE_PATTERN];
    float_t d1[RPR0521RS_ALS_CASE_PATTERN];
    float_t juge[RPR0521RS_ALS_CASE_PATTERN];
} RPR0521RS_COEFF_UNIT;

typedef struct {
    sns_sensor_instance*  instance;
    const char*           name;
    RPR0521RS_SENSOR_TYPE sensor;
    uint32_t              version;
} pb_arg_reg_group_arg;

// Sensor common state
typedef struct {
    uint8_t                        who_am_i;
    
    // registry sensor platform config
    bool                           registry_pf_cfg_received;
    
    //cal flag
    bool                           registry_fac_cal_als_received;
    bool                           registry_fac_cal_init_als_received;
    bool                           registry_fac_cal_ps_received;
    bool                           registry_fac_cal_init_ps_received;
    
    bool                           hw_is_present;
    
    // registry sensor config
    bool                           registry_cfg_received;
    
    // placement
    bool                           registry_placement_received;
    float_t                        placement[12];
    
    // Registry, Timer, Interrupt
    SNS_SUID_LOOKUP_DATA(3)        suid_lookup_data;
    RPR0521RS_COM_PORT_INFO        com_port_info;
    sns_interrupt_req              irq_config;
    sns_rail_config                rail_config;
    sns_power_rail_state           registry_rail_on_state;
    sns_registry_phy_sensor_pf_cfg registry_pf_cfg;
    
    // registry sensor unit config
    sns_registry_phy_sensor_cfg    registry_cfg;
} RPR0521RS_COMMON_STATE;

typedef struct {
    RPR0521RS_COMMON_STATE    common;
    sns_sync_com_port_service *scp_service;
    sns_island_service        *island_service;
    bool                      sensor_client_present;
    RPR0521RS_SENSOR_TYPE     sensor;
    sns_sensor_uid            my_suid;
    size_t                    encoded_event_len;
    sns_data_stream           *reg_data_stream;
    sns_data_stream           *timer_stream;
    sns_pwr_rail_service      *pwr_rail_service;
    
    RPR0521RS_POWER_RAIL_PENDING_STATE power_rail_pend_state;

    // sensor configuration 
    bool    is_dri;
    int64_t hardware_id;
    uint8_t resolution_idx;
    bool    supports_sync_stream;
    
    // registry sensor config
    bool registry_cfg_received;
    sns_registry_phy_sensor_cfg registry_cfg;

    // placement
    bool    registry_placement_received;
    float_t placement[12];
    
    // registry sensor platform config
    sns_registry_phy_sensor_pf_cfg registry_pf_cfg;
    
    //sensor configration
    RPR0521RS_COEFF_UNIT tab_coef;
    
    float_t  near_threshold;
    float_t  far_threshold;
    float_t  als_fac_cal_scale_factor;
    float_t  als_fac_cal_bias;
    float_t  als_fac_cal_init_scale_factor;
    float_t  als_fac_cal_init_bias;
    uint32_t als_fac_cal_version;
    float_t  ps_fac_cal_bias;
    float_t  ps_fac_cal_init_bias;
    uint32_t ps_fac_cal_version;
} RPR0521RS_STATE;

//==============================================================================
// function prototype
//==============================================================================
// sensor api funciton 
sns_rc rpr0521rs_als_init(sns_sensor *const this);
sns_rc rpr0521rs_als_deinit(sns_sensor *const this);
sns_rc rpr0521rs_ps_init(sns_sensor *const this);
sns_rc rpr0521rs_ps_deinit(sns_sensor *const this);

// funciton to be able to access from other files
void rpr0521rs_common_init(sns_sensor *const this);
void rpr0521rs_request_registry(sns_sensor *const this);
void rpr0521rs_sensor_process_registry_event(sns_sensor *const this, sns_sensor_event *event);
void rpr0521rs_update_sibling_sensors(sns_sensor *const this);
void rpr0521rs_start_hw_detect_sequence(sns_sensor *const this);
bool rpr0521rs_discover_hw(sns_sensor *const this);
void rpr0521rs_start_power_rail_timer(sns_sensor *const this, sns_time timeout_ticks, RPR0521RS_POWER_RAIL_PENDING_STATE pwr_rail_pend_state);
void rpr0521rs_update_registry(sns_sensor *const this, sns_sensor_instance *const instance, RPR0521RS_SENSOR_TYPE sensor_type);
bool rpr0521rs_encode_registry_cb(struct pb_ostream_s *stream, struct pb_field_s const *field, void *const *arg);
bool rpr0521rs_encode_registry_group_cb(struct pb_ostream_s *stream, struct pb_field_s const *field, void *const *arg);

#endif // SNS_RPR0521RSRS_SENSOR_H_
