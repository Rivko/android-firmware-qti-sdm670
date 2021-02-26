#pragma once
/*=============================================================================
  @file oem1_sensor.h

  The OEM1 virtual Sensor

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_data_stream.h"
#include "sns_diag_service.h"
#include "sns_resampler.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_suid_util.h"

//#define SUPPORT_DIRECT_SENSOR_REQUEST
//#define SUPPORT_EVENT_TYPE // enable in conjunction with resampler
//#define SUPPORT_REGISTRY

//By default OEM1 output is streaming type 
//To change it to event type - please enable below flag
//#define OEM1_SUPPORT_EVENT_TYPE

//Enable this flag to make use of values in registry file (sns_oem1.json) 
//This can be customized to add more i/p params
#define OEM1_SUPPORT_REGISTRY

//Minumum sample rate for oem1
#define OEM1_MINUMUM_SAMPLE_RATE 1.0
/*=============================================================================
  Type Definitions
  ===========================================================================*/
#define OEM1_SUID \
{  \
  .sensor_uid =  \
  {  \
    0xee, 0xf4, 0x35, 0xf8, 0x28, 0x75, 0x16, 0xa7,  \
    0xa1, 0x96, 0x4b, 0x50, 0x7b, 0xf7, 0xe3, 0xe0  \
  }  \
}

typedef struct
{
  size_t encoded_data_event_len;
  float sample_rate;
  float down_value;
} sns_oem1_config;

typedef struct sns_oem1_sensor_state {
  // Requests to the registry sensor
  sns_data_stream *registry_stream;


#ifndef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  // resampler, accel, registry
  SNS_SUID_LOOKUP_DATA(3) suid_lookup_data; 
#else
  // Accel, registry
  SNS_SUID_LOOKUP_DATA(2) suid_lookup_data;
#endif

  bool first_pass; 
  sns_oem1_config config;
  sns_diag_service *diag_service;
} sns_oem1_sensor_state;

//These are defined in sns_oem1_sensor.c
// and referred in vtable in sns_oem1_sensor_island.c
sns_rc sns_oem1_init(sns_sensor *const this);
sns_rc sns_oem1_deinit(sns_sensor *const this);
sns_sensor_instance* sns_oem1_set_client_request(
    sns_sensor *const this,
    struct sns_request const *exist_request,
    struct sns_request const *new_request,
    bool remove);

sns_rc sns_oem1_notify_event(sns_sensor *const this);

