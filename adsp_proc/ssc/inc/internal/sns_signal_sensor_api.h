#pragma once
/*=============================================================================
  @file sns_signal_sensor_api.h

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include "sns_time.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/
//TODO: Remove when SUID Query support is working
#define SIGNAL_SENSOR_UID   { \
    .sensor_uid = \
    { \
      0x5f, 0xb4, 0x65, 0xb3, 0x57, 0xb1, 0x07, 0x45, \
      0x75, 0x1a, 0xbe, 0x82, 0xd7, 0xea, 0xb2, 0xc3 \
    } \
  }

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef enum
{
  SIGNAL_REGISTERED,
  SIGNAL_ARRIVED,
} sns_signal_sensor_event;

typedef struct
{
  bool enable_new_signal;
} signal_sensor_config;

typedef struct
{
  void *thread_handle;
  uint32_t signal_flag;
} signal_register_event;

typedef struct
{
  sns_time signal_time;
  uint32_t signal_flag;
} signal_arrived_event;

typedef struct
{
  sns_signal_sensor_event event_id;
  union
  {
    signal_register_event details;
    signal_arrived_event flags;
  }payload;
} sns_signal_event;

