#pragma once
/*============================================================================
  @file qmd.h

  Absolute motion detector header file

  Copyright (c) 2010-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

============================================================================*/

#include <stdint.h>
#include "sns_sensor_uid.h"
#include "sns_time.h"

#define QMD_ACCEL_COLS (3)
#define QMD_OP_INTERVAL_REL    (0)                            // seconds
#define QMD_FILTER_ENABLED_REL (false)                        // boolean
#define QMD_FC_REL             (0)                            // Hz

typedef int32_t q16_t;

typedef enum
{
  QMD_MIN_VALUE = -2147483647, // To force signed 32 bit value
  QMD_UNKNOWN = 0,
  QMD_REST    = 1,
  QMD_MOTION  = 2,
  QMD_MAX_VALUE = 2147483647 // To force signed 32 bit value
} qmd_motion_state;

typedef enum
{
  QMD_STREAM_ACCEL_PERIODIC,
  QMD_STREAM_ACCEL_BUFFERED,
  QMD_STREAM_ACCEL_PAUSE
} qmd_accel_stream_type;

typedef enum
{
  QMD_MD_STATE_DISABLED,
  QMD_MD_STATE_ENABLED,
  QMD_MD_STATE_FIRED,
  QMD_MD_STATE_UNKNOWN
}qmd_motion_detect_state;

typedef enum
{
  QMD_INPUT_ACCEL,
  QMD_INPUT_MD
} qmd_input_data_type;

/**
* qmd_config
* Passed to the algorithm instance upon algo reset.
*
* These values were parsed from the enable request message, registry data, and
* internal defaults
*/
typedef struct
{
  int32_t sample_rate;                   // In Hz, Q16
  int32_t internal_config_param1;        // In radians, Q16
  int32_t internal_config_param2;        // In seconds, Q16
  int32_t internal_config_param3;        // In radians, Q16
  int32_t internal_config_param4;        // In seconds, Q16
  bool mdIntAvailable;
  uint32_t gpio_pin_num;     
  bool gpio_is_chip_pin;
} qmd_config;

/**
* qmd_input
* Contains input to the QMD algorithms.
*/
typedef struct
{
  qmd_input_data_type input_type;
  int32_t accel[QMD_ACCEL_COLS];  // accel, m/s/s, Q16
} qmd_input;

/**
* qmd_output
* Will be used to generate output indication messages
*/
typedef struct
{
  qmd_motion_state rest_state; // unknown=0, rest=1, motion=2
  sns_time timestamp;
} qmd_output;

/**
* amd_persist
* AMD algorithm information to be stored in persitant memory
*
* This structure does not need to be protected against concurrent access
*/
typedef struct {
  qmd_config     qmdConfig;         /**< Default algorithm configuration */
  q16_t          reportRate;    /**< Default reporting rate */
  bool           accelAvailable;        /**< Accel availibility */
  bool           regDataAvailable;     /**< Registry data available*/
  bool           mdIntAvailable;      /**< SMGR Motion detect interrupt available*/
  bool           gatedAccelAvailable;      /**< SMGR Motion detect interrupt available*/
  uint64_t       power;
  uint64_t       minSample;
  uint64_t       maxSample;
  sns_sensor_uid const *accelSUID;
  sns_sensor_uid const *mdIntSUID;
  sns_sensor_uid const *GatedAccelSUID;
} qmd_persist;

/* QMD Message API */


