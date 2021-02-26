#pragma once
/*=============================================================================
  @file sns_sensor_instance.h

  Represents a specific instance of a Sensor.  Each Sensor may have 0 or more
  instances active at any given time.  The Sensor determines when multiple
  client requests may be shared; it is expected to share aggressively.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_request;
struct sns_sensor;
struct sns_service_manager;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_sensor_instance
{
  /* Functions which call back into the framework; provided by the Framework */
  struct sns_sensor_instance_cb const *cb;

  /* State space allocated by the Framework for the sole use of the Sensor
   * Instance developer. */
  struct sns_sensor_instance_state *state;
} sns_sensor_instance;

/**
 * Accessor functions for Sensor Instance state managed by the Framework.
 */
typedef struct sns_sensor_instance_cb
{
  uint32_t struct_len;

  /**
   * Get a reference to the Service Manager.  With this object, a reference
   * to any other utility service can be obtained.
   *
   * @param[i] this Sensor Instance reference
   *
   * @return Service Manager reference
   */
  struct sns_service_manager* (*get_service_manager)(
    sns_sensor_instance *this);

  /**
   * Return the next client request associated with this Sensor Instance and
   * SUID.
   *
   * Each Sensor Instance has a list of client requests per SUID which it is
   * servicing.  Entries are added via calls to add_client_request; removed
   * via remove_client_request.
   *
   * Each call to this function iterates over the list, and returns the next
   * entry.  NULL is returned at the end of the list, or if the list is empty.
   *
   * @note An Instance may be handling client requests for multiple
   * (related) Sensors; must use SUID parameter to filter.
   *
   * @param[i] this Sensor Instance reference
   * @param[i] suid Sensor associated with this Instance
   * @param[i] first Return the first request; reset the internal iterator
   *                 Must be called first to initialize iteration
   *
   * SNS_RC_NOT_AVAILABLE - The Framework is not aware of SUID
   * SNS_RC_SUCCESS
   */
   struct sns_request const* (*get_client_request)(
    sns_sensor_instance *this,
    sns_sensor_uid const *suid,
    bool first);

   /**
    * Remove a client request from this Sensor Instance.
    *
    * @param[i] this Sensor Instance reference
    * @param[i] request Client request to be removed
    */
   void (*remove_client_request)(
    sns_sensor_instance *this,
    struct sns_request const *request);

  /**
   * Assign this Sensor Instance to service the client request.
   *
   * @note This function may only be given sns_request objects received
   * from sns_sensor_api::set_client_request.
   *
   * @note The SUID of the recepient Sensor will be noted upon addition;
   * this SUID must be used within get_client_request.
   *
   * @param[i] this Sensor Instance reference
   * @param[i] request Client request to be added
   */
   void (*add_client_request)(
    sns_sensor_instance *this,
    struct sns_request const *request);
} sns_sensor_instance_cb;

/**
 * Sensor Instance API; to be implemented by the Sensor developer
 */
typedef struct sns_sensor_instance_api
{
  uint32_t struct_len;

  /**
   * Initialize a Sensor Instance to its default state.  A call to
   * sns_sensor_instance_api::deinit will precede any subsequent calls
   * to this function.
   *
   * @note Persistent configuration can be made available using the
   * sensor_state.
   *
   * @param[i] this Sensor Instance reference
   * @param[i] sensor_state State of the Sensor which created this Instance
   *
   * @return
   * SNS_RC_NOT_AVAILABLE - Sensor state does not allow for this operation
   * SNS_RC_SUCCESS
   */
  sns_rc (*init)(
    sns_sensor_instance *const this,
    sns_sensor_state const *sensor_state);

  /**
   * Release all hardware and software resources associated with this Sensor
   * Instance.
   *
   * @param[i] this Sensor Instance reference
   *
   * @return
   * SNS_RC_INVALID_STATE - Error occurred: some resource could not be released
   * SNS_RC_SUCCESS
   */
  sns_rc (*deinit)(
    sns_sensor_instance *const this);

  /**
   * Update a Sensor Instance configuration to this sensorRequest.
   *
   * The Sensor Instance is expected to start all dependent streams, timers, etc..
   *
   * @note
   * A Sensor may define any number of unique request types they support.
   * However, a client may only have a single active stream; an enable
   * request can inherently serve as a "reconfiguration" request.
   *
   * @param[i] this Sensor Instance reference
   *
   * @return
   * SNS_RC_INVALID_VALUE - Invalid client request
   * SNS_RC_SUCCESS
   */
  sns_rc (*set_client_config)(
    sns_sensor_instance *const this,
    struct sns_request const *client_request);

  /**
   * Notification to the client that some data has been received.
   *
   * The client must use the sns_manager_event to obtain this data
   * for processing.
   *
   * @return
   * SNS_RC_INVALID_STATE - A client error occurred; Framework shall destroy client
   * SNS_RC_NOT_AVAILABLE - A transitory error occurred; Framework shall remove
   *    all outstanding input
   * SNS_RC_SUCCESS
   */
  sns_rc (*notify_event)(
    sns_sensor_instance *const this);
} sns_sensor_instance_api;

/**
 * State specifically allocated for the Sensor Instance, to be used by the
 * Sensor developer as needed.  May be relocated; no pointers into this buffer
 * may be saved.
 */
typedef struct sns_sensor_instance_state
{
  uint32_t state_len;
  uint64_t state[1];
} sns_sensor_instance_state;
