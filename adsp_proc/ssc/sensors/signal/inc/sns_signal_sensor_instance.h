#pragma once
/*=============================================================================
  @file sns_signal_sensor_instance.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_signal_sensor.h"
#include "sns_signal_sensor.pb.h"

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct
{
  sns_signal_sensor_req config;
} sns_signal_instance_state;
