//==============================================================================
// MODULE      : sns_bu52053nvx_sensor.h
// FUNCTION    : header for sensor function file
// AUTHOR      : Masafumi Seike
// PROGRAMMED  : Sensor Application Group, ROHM CO.,LTD.
// MAKING DATE : May/15/2017
// MODIFY DATE : AUG/10/2017
// REMARKS     :
// COPYRIGHT   : Copyright (C) 2017 - ROHM CO.,LTD.
//             : Redistribution and use in source and binary forms, with or
//             : without modification, are permitted provided that the
//             : following conditions are met:
//             : 1. Redistributions of source code must retain the above
//             : copyright notice, this list of conditions and the following
//             : disclaimer.
//             : 2. Redistributions in binary form must reproduce the above
//             : copyright notice, this list of conditions and the following
//             : disclaimer in the documentation and/or other materials
//             : provided with the distribution.
//             : 3. Neither the name of ROHM CO.,LTD. nor the names of its
//             : contributors may be used to endorse or promote products
//             : derived from this software without specific prior written
//             : permission.
//             : THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//             : CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//             : INCLUDING , BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//             : MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//             : DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//             : BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//             : CONTRIBUTORS EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//             : BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//             : SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             : INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//             : WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//             : NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//             : OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//             : SUCH DAMAGE.
//==============================================================================
#ifndef _SNS_BU52053NVX_SENSOR_H_
#define _SNS_BU52053NVX_SENSOR_H_ (1)

#include "sns_pwr_rail_service.h"
#include "sns_data_stream.h"
#include "sns_dae.pb.h"
#include "sns_diag_service.h"
#include "sns_bu52053nvx_sensor_instance.h"

#ifndef BU52053NVX_USE_DEFAULTS
#define BU52053NVX_USE_DEFAULTS   (1)
#endif

#define HALL_SUID \
    {  \
        .sensor_uid =  \
        {  \
            0x52, 0x4F, 0x48, 0x4D, 0x20, 0x48, 0x41, 0x4C,  \
            0x4C, 0x20, 0x35, 0x32, 0x30, 0x35, 0x33, 0x00  \
        }  \
    }

#define DEVICE_ACTIVE_CURRENT     (5)
#ifdef BU52053NVX_USE_DEFAULTS
#define RAIL_1                    ("/pmic/client/sensor_vddio")
#define HALL_IRQ_NUM              (124)//(119)
#define NUM_OF_RAILS              (1)
#endif  // BU52053NVX_USE_DEFAULTS

//Supported opertating modes
#define BU52053NVX_HIGH_PERF      "HIGH_PERF"

// Off to idle time
#define BU52053NVX_OFF_TO_IDLE_MS (100)  //ms

// Power rail timeout States for the BU52053NVX Sensors.
typedef enum
{
    POWER_RAIL_PENDING_NONE,
    POWER_RAIL_PENDING_INIT,
    POWER_RAIL_PENDING_SET_CLIENT_REQ,
} BU52053NVX_POWER_RAIL_PENDING_STATE;

// Interrupt Sensor State.
typedef struct
{
    sns_data_stream                     *fw_stream;
    sns_data_stream                     *timer_stream;
    sns_sensor_uid                      irq_suid;
    sns_sensor_uid                      timer_suid;
    BU52053NVX_SENSOR_TYPE              sensor;
    sns_sensor_uid                      my_suid;
    sns_interrupt_req                   irq_config;
    sns_pwr_rail_service                *pwr_rail_service;
    sns_rail_config                     rail_config;
    bool                                hw_is_present;
    bool                                sensor_client_present;
    BU52053NVX_POWER_RAIL_PENDING_STATE power_rail_pend_state;
    sns_diag_service                    *diag_service;
    size_t                              encoded_event_len;

} BU52053NVX_STATE;

// Forward Declaration of hall Sensor API
extern sns_sensor_api bu52053nvx_sensor_api;

#endif //_SNS_BU52053NVX_SENSOR_H_
