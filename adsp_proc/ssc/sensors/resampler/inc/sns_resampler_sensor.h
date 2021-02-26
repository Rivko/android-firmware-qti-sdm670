#pragma once
/*=============================================================================
  @file resampler_sensor.h

  The Quaternion virtual Sensor.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_data_stream.h"
#include "sns_resampler.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_std.pb.h"
#include "sns_time.h"

#define AXIS_CNT_DEFAULT 6

#define AXIS_CNT_MAX 25

#define VARIABLE_SIZE_ARRAY_COUNT 3

#define RESAMPLER_SENSOR_DIMENSION 6

#define SNS_RESAMPLER_DEBUG_ENABLE 1

#define RESAMPLER_SUID 0xc1,0x83,0x4b,0x97,0x9e,0x50,0x43,0x56,\
                       0xb4,0xe7,0x97,0xb0,0x11,0xde,0x33,0xd2

/*=============================================================================
  Type Definitions
  ===========================================================================*/
typedef struct
{
  sns_std_request std_request;
  sns_resampler_config config;
} resampler_request;

// PEND: Can be optimized to use messages defined in nano pb api
typedef struct
{
  // Pointer to dynamic array
  float*             data;
  //number of bytes in the allocated array
  uint32_t			 axis_cnt_bytes;
  // Timestamp of the sample
  sns_time           ts;
  //stores status of the current sample
  sns_std_sensor_sample_status status;
} resampler_sample;

typedef struct resampler_state {
  sns_sensor_uid reg_suid;

  // Requests to the registry sensor
  sns_data_stream *registry_stream;
  // Stream used to acquire the sensor and registry data SUIDs and attributes
  sns_data_stream *fw_stream;
} resampler_state;
