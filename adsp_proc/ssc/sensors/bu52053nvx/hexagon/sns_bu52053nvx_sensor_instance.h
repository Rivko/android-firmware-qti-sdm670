//==============================================================================
// MODULE      : sns_bu52053nvx_sensor_instance.h
// FUNCTION    : header for sensor instance function file
// AUTHOR      : Masafumi Seike
// PROGRAMMED  : Sensor Application Group, ROHM CO.,LTD.
// MAKING DATE : May/18/2017
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
#ifndef _SNS_BU52053NVX_SENSOR_INSTANCE_H_
#define _SNS_BU52053NVX_SENSOR_INSTANCE_H_ (1)

#include "sns_sensor_instance.h"
#include "sns_data_stream.h"
#include "sns_interrupt.pb.h"
#include "sns_diag_service.h"
#include "sns_sensor_uid.h"
#include "sns_hall.pb.h"

// Forward Declaration of Instance API
extern sns_sensor_instance_api bu52053nvx_sensor_instance_api;

typedef enum
{
    BU52053NVX_HALL = 0x1
} BU52053NVX_SENSOR_TYPE;

typedef struct
{
    sns_sensor_uid suid;
} BU52053NVX_HALL_INFO;

// Private state.
typedef struct
{
    // hall HW config details
    BU52053NVX_HALL_INFO hall_info;

    // Interrupt dependency info.
    sns_interrupt_req irq_info;
    bool irq_is_reqistered;

    sns_hall_event prev_event;
    bool first_event_sent;

    // Data streams from dependentcies.
    sns_data_stream *interrupt_data_stream;

    sns_diag_service *diag_service;
    bool instance_is_ready_to_configure;

} BU52053NVX_INSTANCE_STATE;


#endif //_SNS_BU52053NVX_SENSOR_INSTANCE_H_
