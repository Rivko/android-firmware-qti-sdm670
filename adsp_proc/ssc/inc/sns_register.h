#pragma once
/*=============================================================================
  @file sns_register.h

  Entry point for all Sensors.  Each loaded Sensors library must implement
  this function.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stddef.h>
#include <stdint.h>
#include "sns_rc.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_uid;
struct sns_sensor_api;
struct sns_sensor_instance_api;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Callback functions into the Framework, available only within
 * sns_register_sensors.
 */
typedef struct sns_register_cb
{
  uint32_t struct_len;

  /**
   * Allocate and initialize a Sensor.  Allocate state_len bytes for private
   * use of the Sensor, which will be initialized in sns_sensor::init().
   *
   * @param[i] state_len Size to be allocated for sns_sensor::state.
   * @param[i] sensor_api Sensor API implemented by the Sensor developer
   * @param[i] instance_api Sensor Instance API; by the Sensor developer
   *
   * @return
   * SNS_RC_POLICY - state_len too large
   * SNS_RC_NOT_AVAILABLE - Sensor UID is already in-use
   * SNS_RC_FAILED - Sensor initialization failed
   * SNS_RC_SUCCESS
   */
  sns_rc (*init_sensor)(
    uint32_t state_len,
    struct sns_sensor_api const *sensor_api,
    struct sns_sensor_instance_api const *instance_api);

  // PEND: Add query for Framework version
} sns_register_cb;

/**
 * Call init_sensor for all provided Sensor types.
 *
 * This function will only be called once per loaded library.
 *
 * @param[i] register_api
 *
 * @return
 * SNS_RC_INVALID_STATE - Hardware and/or software requirements not met
 * SNS_RC_SUCCESS
 */
typedef sns_rc (*sns_register_sensors)(
  sns_register_cb const *register_api);

/**
 * Sensor registration entry.  Used in sns_static_sensors.c by SEE
 * initialization.  Subject to change: DO NOT USE DIRECTLY.
 */
typedef struct sns_register_entry {
  sns_register_sensors func;
  uint32_t cnt;
} sns_register_entry;
