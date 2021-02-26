#pragma once
/**
 *  sns_island_service.h
 *
 *  Provides a synchronous API to request island mode exit.
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/** Include Files */

#include <stdbool.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_service.h"

struct sns_sensor;
struct sns_sensor_instance;

/**
 * The island service. Can be obtained from
 * sns_service_managere::getService.
 */
typedef struct sns_island_service
{
  /** Service information */
  sns_service service;

  /** Public API provided by the Service to be used by the Sensor. */
  struct sns_island_service_api *api;
} sns_island_service;

/**
 * API made available to Sensors.
 */
typedef struct sns_island_service_api
{
  uint32_t struct_len;

  /**
   * Request exit from island mode available to a Sensor.
   *
   * Note: Sensors typically use this service API to request island mode
   * exit from an island mode function before calling a normal mode function.
   * The normal mode function should be called only if this API returns
   * SNS_RC_SUCCESS.
   *
   * Typical uses are:
   *  1. Physical sensor self-test:
   *     - handling timer/interrupt events for self-test execution.
   *  2. Handling registry sensor events.
   *
   * @param[i] this      island service reference
   * @param[i] sensor    sensor reference that is requesting island exit.
   *                     NULL if an instance is requesting island exit.
   *
   * @return sns_rc
   * SNS_RC_SUCCESS if island mode exit was successful
   * SNS_RC_FAILED  otherwise
   */
  sns_rc (*sensor_island_exit)(sns_island_service *this,
                               struct sns_sensor const* sensor);

  /**
   * Request exit from island mode available to a Sensor Instance.
   *
   * Note: Sensor Instances typically use this service API to request
   * island mode exit from an island mode function before calling a normal
   * mode function. The normal mode function should be called only if this API
   * returns SNS_RC_SUCCESS.
   *
   * Typical uses are:
   *  1. Physical sensor self-test:
   *     - handling timer/interrupt events for self-test execution.
   *  2. Handling registry sensor events.
   *
   * @param[i] this      island service reference
   * @param[i] instance  sensor instance reference that is requesting island exit.
   *                     NULL if a sensor is requesting island exit.
   *
   * @return sns_rc
   * SNS_RC_SUCCESS if island mode exit was successful
   * SNS_RC_FAILED  otherwise
   */
  sns_rc (*sensor_instance_island_exit)(sns_island_service *this,
                                        struct sns_sensor_instance const* instance);

  /**
   * Generates a log packet of the sns_island_trace_log type and commits it.
   *
   * Note: This API must be used for debug only. Typical use of this API is to
   * confirm if a line of code is executing in island mode.
   * All calls to this debug API should be disabled by default and only enabled
   * using registry config or compile switches when necessary for debug.
   *
   * @param[i] this              island service reference
   * @param[i] user_defined_id   The value to be added to the cookie field in
   *                             the log packet
   *
   * @return sns_rc
   * SNS_RC_SUCCESS              log packet operation was successful
   * SNS_RC_NOT_SUPPORTED        log packet not supported
   */
  sns_rc (*island_log_trace)(sns_island_service *this,
                             uint64_t user_defined_id);

} sns_island_service_api;

