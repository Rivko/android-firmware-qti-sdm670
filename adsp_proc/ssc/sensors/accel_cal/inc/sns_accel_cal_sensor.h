#pragma once
/*=============================================================================
  @file sns_accel_cal_sensor.h

  Accel cal sensor.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_accel_cal.pb.h"
#include "sns_data_stream.h"
#include "sns_resampler.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_suid_util.h"

#define SNS_CAL_SENSOR_DIM                 (3)
#define SNS_CAL_COMP_MATRIX_DIM            (9)

#define SNS_ACCEL_CAL_CALIBRATION_PERIOD    (60ULL) //  seconds

#define SNS_ACCEL_CAL_SAMPLE_RATE (10.0) // Hz

#define SNS_ACCEL_CAL_SUID_BASE  0xb4, 0xa0, 0x1f, 0x4a, 0x81, 0xa6, 0x09, 0xd7, \
                                 0x3c, 0x91, 0x34, 0xda, 0xe8, 0xaa, 0xcd, 0x9a
                   

/*=============================================================================
  Type Definitions
  ===========================================================================*/
typedef struct
{
  sns_time            timestamp;
  float               bias[SNS_CAL_SENSOR_DIM];
  float               scale_factor[SNS_CAL_SENSOR_DIM];
  float               compensation_matrix[SNS_CAL_COMP_MATRIX_DIM];
  sns_std_sensor_sample_status   accuracy;
} sns_cal_event_data;

/*accel calibration configuration*/
typedef struct
{
  float    sampleRate;    // sample rate in Hz
  uint16_t calibration_period; // sec
  uint8_t  registration_index;
} sns_accel_cal_config;

/*accel calibration persistent state*/
typedef struct
{
  sns_accel_cal_config config;
  sns_cal_event_data output;
} sns_accel_cal_persist;

typedef struct accel_cal_state
{
  sns_sensor_uid const   *self_suid;
  sns_data_stream        *registry_stream; // Requests to the registry sensor
  SNS_SUID_LOOKUP_DATA(4) suid_lookup_data;  // Registry, accel, AMD, resampler

  sns_accel_cal_persist    persist_state;

  uint32_t                registry_persist_version;
  bool                    registry_presist_received;

  bool                    registry_configuration_received;
} accel_cal_state;

