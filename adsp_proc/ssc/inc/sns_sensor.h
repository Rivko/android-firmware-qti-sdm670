#pragma once
/*=============================================================================
  @file sns_sensor.h

  Represents a source of data within the Sensors Framework; Sensors
  may optionally and additionally be data consumers.

  Sensors must manage their list of Sensor Instances.  The Framework
  will not explicitly create or destroy any Sensor Instance; although it
  may relocate it in memory as needed, so pointers should not be saved.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stddef.h>
#include <stdint.h>
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor_uid.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_service_manager;
struct sns_sensor_instance;
struct sns_sensor_instance_api;

/*=============================================================================
  External Declarations
  ===========================================================================*/

/**
 * In some cases, a Sensor may successfully handle a request within
 * set_client_request, however the outcome of the operation may not be an
 * client request assigned to an instance.  For example if a Sensor Driver
 * receives a calibration reset request, the request may be handled
 * synchronously, and an Instance is not necessarily maintained after the
 * function call returns.  The API states that in the case of an error, NULL
 * is returned; in this case there is no error, but we similarly do not have
 * a valid instance to return.  Hence, this global variable.
 */
extern struct sns_sensor_instance sns_instance_no_error;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * A physical, virtual, or utility Sensor residing on the system.
 * Structures of this type are only created at boot-up.
 */
typedef struct sns_sensor
{
  /* Functions which call back into the framework; provided by the Framework */
  struct sns_sensor_cb const *cb;

  /* API implementation provided for and by this Sensor */
  struct sns_sensor_api const *sensor_api;

  /* The associated API for an Sensor Instances created for and by this
   * Sensor. */
  struct sns_sensor_instance_api const *instance_api;

  /* State space allocated by the Framework for the sole use of the Sensor
   * developer. */
  struct sns_sensor_state *state;
} sns_sensor;

/**
 * Sensor functions available for use by the Sensor developer.
 */
typedef struct sns_sensor_cb
{
  uint32_t struct_len;

  /**
   * Get a reference to the Service Manager.  With this object, a reference
   * to any other utility service can be obtained.
   *
   * @param[i] this Sensor reference
   *
   * @return Service Manager reference
   */
  struct sns_service_manager* (*get_service_manager)(
    sns_sensor const *this);

  /**
   * Return the next Sensor Instance associated with this Sensor.
   *
   * Each Sensor has a list of associated Sensor Instances; entries are added
   * to that list within calls to 'create_instance', and removed from the
   * list when it services no client requests.
   *
   * Each call to this function iterates over the list, and returns the next
   * entry.  NULL is returned at the end of the list, or if the list is empty.
   *
   * @param[i] this Sensor reference
   * @param[i] first Return the first instance; reset the internal iterator
   *                 Must be called first to initialize iteration
   *
   * @return Next Sensor Instance associated with this Sensor
   */
  struct sns_sensor_instance* (*get_sensor_instance)(
    sns_sensor const *this,
    bool first);

  /**
   * Allocate and initialize a new Sensor Instance to be associated with this
   * Sensor.  Will call sns_sensor_instance::init.
   *
   * @note Direct pointers to the returned value should not be saved.
   *
   * @param[i] this Sensor reference
   * @param[i] stateLen Allocation size for sns_sensor_instance::state
   *
   * @return Newly created Sensor Instance
   */
  struct sns_sensor_instance* (*create_instance)(
    sns_sensor *this,
    uint32_t state_len);

  /**
   * Remove and deallocate a Sensor Instance.  Will call
   * sns_sensor_instance::deinit.
   *
   * @param[i] instance Instance received within set_client_request
   */
  void (*remove_instance)(
    struct sns_sensor_instance *instance);

  /**
   * Return the next Sensor associated with this library.
   *
   * Each Sensor is a member of a library; each library may contain several
   * Sensors.  Sensors may be removed from a library upon errors, but no
   * entries are added after Framework initialization has completed.
   *
   * Each call to this function iterates over the list, and returns the next
   * entry.  NULL is returned at the end of the list, or if the list is empty.
   *
   * This function is intended to be used by Sensors which share physical
   * hardware with another sensor, and hence must share state/instances.
   *
   * @param[i] this Sensor reference
   * @param[i] first Return the first sensor; reset the internal iterator;
   *                 Must be called first to initialize iteration
   *
   * @return Next Sensor associated with this library.
   */
  struct sns_sensor* (*get_library_sensor)(
    sns_sensor const *this,
    bool first);

  /**
   * If multiple copies of this Sensor Library have been registered with SEE,
   * this returns the index (starting at '0') of this particular copy.  See
   * parameter registration_cnt of env.AddSSCSU.
   *
   * @param[i] this Sensor reference
   *
   * @return Library registration index
   */
  uint32_t (*get_registration_index)(
    sns_sensor const *this);
} sns_sensor_cb;

/**
 * Functions every Sensor developer must implement.
 *
 * These functions are exclusively used by the Sensor Framework.
 */
typedef struct sns_sensor_api
{
  uint32_t struct_len;

  /**
   * Initialize a Sensor to its hard-coded/default state.  Generate
   * requests for any other necessary data (e.g. Registry data).  A call to
   * sns_sensor_api::deinit will precede any subsequent calls to this function.
   *
   * @param[i] this Sensor reference
   *
   * @return
   * SNS_RC_INVALID_STATE - Requisite hardware not available
   * SNS_RC_POLICY - Required services not available
   * SNS_RC_SUCCESS
   */
  sns_rc (*init)(
    sns_sensor *const this);

  /**
   * Release all hardware and software resources associated with this Sensor
   *
   * @param[i] this Sensor reference
   *
   * @return
   * SNS_RC_INVALID_STATE - Error occurred: some resource could not be released
   * SNS_RC_SUCCESS
   */
  sns_rc (*deinit)(
    sns_sensor *const this);

  /**
   * Each Sensor must have a globally unique identifier; each algorithm
   * and driver will define their own. If a Sensor may be loaded twice on the
   * system, it is responsible for returning two unique values.  These must
   * not change across device reboots.
   *
   * @param[i] this Sensor reference
   *
   * @return The unique identifier for this Sensor
   */
  sns_sensor_uid const* (*get_sensor_uid)(
    sns_sensor const *const this);

  /**
   * Notification to the client that some data has been received.
   *
   * The client must use the sns_event_service to obtain this data
   * for processing.
   *
   * @return
   * SNS_RC_INVALID_STATE - A client error occurred; Framework shall destroy
   *                        client
   * SNS_RC_NOT_AVAILABLE - A transitory error occurred; Framework shall
   *                        remove all outstanding input
   * SNS_RC_INVALID_LIBRARY_STATE - A permanent error occurred; Framework shall
   *                        destroy all sensors present in the client library
   * SNS_RC_SUCCESS
   */
  sns_rc (*notify_event)(
    sns_sensor *const this);

  /**
   * Add, remove, or update a client's request to this Sensor.
   *
   * For each new request sent by a client, the Sensor (via this function)
   * will receive the new_request.  If the client has an active request
   * (which is to be replaced), it will be specified in exist_request.
   *
   * If 'remove' is false:
   * A client has sent a new request to this Sensor.  Determine if any
   * active Sensor Instance in sns_sensor_cb::get_sensor_instance()
   * will handle this request.  If yes, use add_client_request to associate
   * this new request with that existing Instance.
   *
   * If not, instantiate and initialize a new Sensor Instance with the
   * appropriate configuration, and similarly use add_client_request.
   *
   * In either case, if exist_request is provided and new_request provides
   * a superceding configuration, exist_request must be removed via
   * remove_client_request.
   *
   * If 'remove' is true:
   * Remove this client request by sns_sensor_instance_cb::remove_client_request;
   * re-arrange any remaining client requests/sensor instances.
   *
   * In all cases, if the result of the operation is a Sensor Instance with
   * zero clients, sns_sensor_cb::remove_instance must be called.
   *
   * @param[i] this Sensor reference
   * @param[i] exist_request If this request comes-in over an existing stream,
   *                       this is the existing request.
   * @param[i] new_request New request just received
   * @param[i] remove If the client no longer requires this data
   *
   * @return
   * The Sensor Instance chosen to handle this new client.  NULL if an error
   * occurred during processing; or if "remove" was true.
   * Or sns_instance_no_error (see above).
   */
  struct sns_sensor_instance* (*set_client_request)(
    sns_sensor *const this,
    struct sns_request const *exist_request,
    struct sns_request const *new_request,
    bool remove);
} sns_sensor_api;

/**
 * State specifically allocated for the Sensor, to be used by the Sensor
 * developer as needed.  May be moved; no pointers into this buffer may
 * be saved.
 */
typedef struct sns_sensor_state
{
  uint32_t state_len;
  uint64_t state[1];
} sns_sensor_state;
