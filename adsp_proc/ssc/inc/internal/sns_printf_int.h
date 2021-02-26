#pragma once
/*=============================================================================
 * @file sns_printf_int.h
 *
 * Internal Printf support.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_printf.h"

/*=============================================================================
  Dependencies
  ===========================================================================*/
extern void
sns_diag_sensor_sprintf(
    const sns_sensor *sensor, sns_message_priority prio,
    const char *file, uint32_t line, const char *format, ...);

extern sns_sensor *sns_fw_printf;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#define SNS_DIAG_PTR        "%x"

/**
  * Size of the Sensors diag F3 trace buffer. This buffer will be used to store
  * F3 messages on the Heap and can be extracted from the RAM dump in case of
  * a crash.
  */
#define SNS_F3_TRACE_SIZE 16*1024

/**
 * Print a log message from a Sensor.
 *
 * Full printf support; less efficient than SNS_PRINTF.
 *
 * Parameters:
 *   prio: LOW, MED, HIGH, ERROR, FATAL
 *   sensor: sns_sensor
 *   fmt, args: printf style format string and arguments
 */
#define SNS_SPRINTF(prio, sensor, fmt, ...) do { \
  sns_diag_sensor_sprintf(sensor, SNS_##prio, __FILENAME__, \
      __LINE__, fmt, ##__VA_ARGS__); \
} while(0)
