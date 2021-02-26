#pragma once
/*=============================================================================
  @file sns_timer_sensor_instance.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_timer_sensor.h"

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct
{
  /* List of all active timers, sorted by next expiration
   * Ie: head of the list expires next */
  sns_isafe_list_item list_entry;

  /* List of all periodic timers, sorted by shortest to longest period */
  sns_isafe_list_item periodic_entry;

  /* Absolute timeout time in ticks */
  sns_time trigger_time;

  /* Exact timer configuration used by this instance / timer */
  sns_timer_sensor_reg_event client_config;
} sns_timer_instance_state;
