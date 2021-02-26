#pragma once
/*=============================================================================
  @file sns_signal_sensor.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_isafe_list.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_service_manager.h"
#include "sns_signal_sensor_api.h"
#include "sns_time.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/
#define SNS_SIGNAL_SENSOR_MAX_SIGNALS 10

/*=============================================================================
  Type definitions
  ===========================================================================*/
typedef struct
{
  uint8_t bitshift;
  bool enabled;
  struct sns_sensor_instance *instance;
  sns_time timestamp;
}sns_signal_assigned;

typedef struct
{
  sns_signal_assigned registered_signals[SNS_SIGNAL_SENSOR_MAX_SIGNALS];
  sns_osa_thread *thread_id;
  sns_service_manager *manager;
} sns_signal_state;

