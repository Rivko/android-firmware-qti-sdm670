#pragma once
/*=============================================================================
  @file oem1_sensor_instance.h

  The oem1 virtual Sensor Instance

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_diag_service.h"
#include "sns_oem1.pb.h"
#include "sns_oem1_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_std_sensor.pb.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
/* Number of elements in the computed oem1 vector */
#define OEM1_NUM_ELEMS      (3)

/*============================================================================
  Type Declarations
  ===========================================================================*/

sns_rc sns_oem1_inst_init(sns_sensor_instance *this,sns_sensor_state const *state);
sns_rc sns_oem1_inst_deinit(sns_sensor_instance *const this);
sns_rc sns_oem1_inst_set_client_config(sns_sensor_instance *const this,
                                      sns_request const *client_request);

typedef struct
{
  float accel_sample[OEM1_NUM_ELEMS];
  sns_time accel_sample_ts;
} sns_oem1_input;

  typedef enum
{
  OEM1_MIN_VALUE = -2147483647, // To force signed 32 bit value
  OEM1_UNKNOWN = 0,
  OEM1_FACING_UP  = 1,
  OEM1_FACING_DOWN  = 2,
  OEM1_MAX_VALUE = 2147483647 // To force signed 32 bit value
} oem1_facing_state;

typedef struct
{
  oem1_facing_state            previous_state;
  oem1_facing_state            current_state;
} sns_oem1_inst_config;

typedef struct sns_oem1_inst_state
{
#ifdef OEM1_SUPPORT_DIRECT_SENSOR_REQUEST
  sns_data_stream *accel_stream;
#else
  sns_data_stream *resampler_stream;
  sns_sensor_uid resampler_suid;
#endif
  sns_diag_service *diag_service;
  sns_std_sensor_config client_config;
  sns_oem1_config config;
  sns_oem1_inst_config inst_config;
  sns_std_sensor_sample_status accuracy;
  sns_sensor_uid accel_suid;
  float down_value;
#ifdef OEM1_SUPPORT_REGISTRY
  sns_sensor_uid registry_suid;
#endif
} sns_oem1_inst_state;
