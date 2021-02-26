#pragma once
/*=============================================================================
  @file accel_cal_sensor_instance.h

  Accel cal sensor instance header.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_accel_cal.pb.h"
#include "sns_accel_cal_sensor.h"
#include "sns_amd.pb.h"
#include "sns_diag_service.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

#define SNS_ACCEL_CAL_AXES (3)
#define SNS_ACCEL_CAL_COL SNS_ACCEL_CAL_AXES

typedef enum sns_accel_cal_event_type
{
   SNS_ACCEL_CAL_BIAS = 1,
   SNS_ACCEL_CAL_SCALE_FACTOR = 2,
   SNS_ACCEL_CAL_COMP_MATRIX = 4,
} sns_accel_cal_event_type;

typedef struct sns_accel_cal_inst_state
{
  sns_data_stream       *resampler_stream;
  sns_data_stream       *amd_stream;
  sns_sensor_uid const  *self_suid;
  sns_accel_cal_persist  persist_state;
  sns_amd_event         amd_state;

  sns_sensor_uid accel_suid;
  sns_sensor_uid resampler_suid;
  sns_sensor_uid amd_suid;

  sns_diag_service    *diag_service;
} sns_accel_cal_inst_state;
