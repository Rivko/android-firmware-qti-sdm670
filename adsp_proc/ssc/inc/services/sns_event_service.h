#pragma once
/*=============================================================================
  @file sns_event_service.h

  Handles the storage of all Sensor events.  The ThreadManager is notified
  when new events are available for processing.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "sns_rc.h"
#include "sns_service.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_instance;
struct sns_request;
struct sns_sensor_event;
struct sns_sensor_uid;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_event_service
{
  sns_service service;
  struct sns_event_service_api *api;
} sns_event_service;

typedef struct sns_event_service_api
{
  uint32_t struct_len;

  /**
   * Get the maximum event size supported by the Framework
   *
   * @param[i] this Event Service reference
   * @return The maximum event size supported by the Framework.
   */
  uint32_t (*get_max_event_size)(
    sns_event_service const *this);

  /**
   * Allocate an empty event buffer.
   *
   * Output will not be sent to clients until publishEvent is called.
   * allocBuffer may not be called again until publishEvent is caled.
   *
   * From whichever Sensor Instance API called allocEvent, within
   * that same function block publishEvent must be called. If the function
   * block returns without doing so, the Framework will free the buffer and
   * return it to the pool.
   *
   * @note
   * An output Event may consist of multiple logical samples.  These
   * samples will be consumed individually and sequentially by the
   * client.
   *
   * @param[i] this Event Service reference
   * @param[i] instance Sensor Instance which will form/publish the event
   * @param[i] event_len Buffer space to allocate for the Event
   *
   * @return All allocations are guaranteed to succeed unless event_len
   * exceeds get_max_buffer_size().
   */
  struct sns_sensor_event* (*alloc_event)(
    sns_event_service *this,
    struct sns_sensor_instance *instance,
    uint32_t event_len);

  /**
   * Publish an event, previously allocated by alloc_event, to all registered
   * clients.
   *
   * Client may specify the SUID of the data being sent.  This SUID must
   * match the SUID of the Sensor to which a client sent its enable request.
   *
   * @note Must be called within the same block as allocBuffer.
   *
   * @param[i] this Event Service reference
   * @param[i] instance Sensor Instance which formed the event
   * @param[i] event Event to publish to all registered clients
   * @param[i] sensor_uid SUID of this published data, if set to
   *       NULL, framework determines suid from instance parameter
   *
   * @return
   * SNS_RC_SUCCESS
   */
  sns_rc (*publish_event)(
    sns_event_service *this,
    struct sns_sensor_instance *instance,
    struct sns_sensor_event *event,
    struct sns_sensor_uid const *sensor_uid);

  /**
   * Publish a generic error event.  This event will be delivered to all
   * active clients.  Upon receipt, the Framework will destroy this Sensor
   * Instance, and all associated state.
   *
   * @note This is a helper function; an error event could also be
   * generated and published using the alloc_event/publish_event API.
   *
   * @param[io] this Event Service reference
   * @param[i] reason
   *  SNS_RC_INVALID_STATE - The Sensor Instance entered an invalid state
   *  SNS_RC_NOT_AVAILABLE - A software and/or hardware dependency has been lost
   *
   * @return
   * SNS_RC_SUCCESS
   */
  sns_rc (*publish_error)(
    sns_event_service *this,
    struct sns_sensor_instance *instance,
    sns_rc reason);
} sns_event_service_api;
