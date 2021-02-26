#pragma once
/*=============================================================================
  @file sns_sensor_util.h

  Utility functions for use by Sensor Developers.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_data_stream.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_data_stream;
struct sns_sensor;
struct sns_sensor_instance;
struct sns_request;
struct sns_sensor_instance_state;

typedef struct
{
  char *data_type_str;   /* input to SUID sensor */
  sns_sensor_uid *suid;  /* list of SUIDs returned from SUID sensor */
  uint32_t num_of_suids; /* number of items in suid list*/
} sns_suid_search;

typedef struct sns_timesync_event_info
{
  sns_sensor_event *event;
  int stream_index;
  bool pending;
} sns_timesync_event_info;

/**
 * callback function used with sns_process_streams_in_timesync
 *
 *  @param event: a single event from one of the data streams
 *  @param stream_index index of the data stream associated with
 *                      this event
 *  @param userdata opaque userdata supplied by the caller
 */
typedef sns_rc (*process_stream_event) (sns_sensor_event *event,
                                        int stream_index, void *userdata);

/**
 * Callback function used in sns_sensor_util_find_instance_match()
 *
 * @param[i] request The new incoming request which is looking for a home
 * @param[i] an active Sensor Instance
 *
 * @return true If request can be serviced by this Instance; false otherwise
 */
typedef bool (*sns_sensor_util_instance_match_cb)
  (struct sns_request const *request,
   sns_sensor_instance const *instance);

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/
/**
 * Removes a particular stream for a sensor and marks the stream as a NULL.
 *
 * @param[i] sensor The Sensor for which the stream is to be removed
 * @param[i] stream The stream to be removed
 *
 * @return SNS_RC_SUCCESS If stream removed successfully.
 */

sns_rc
sns_sensor_util_remove_sensor_stream(
    sns_sensor *this,
    sns_data_stream **stream);

/**
 * Removes a particular stream for a sensor instance and marks the stream as NULL.
 *
 * @param[i] sensor The sensor instance for which the stream is to be removed
 * @param[i] stream The stream to be removed
 *
 * @return SNS_RC_SUCCESS If stream removed successfully.
 */

sns_rc
sns_sensor_util_remove_sensor_instance_stream(
    sns_sensor_instance *this,
    sns_data_stream **stream);

/**
 * Find the instance (if any) which is presently servicing this request.
 *
 * @param[i] sensor The Sensor from which to search through
 * @param[i] request The request to search for
 *
 * @return Found Sensor Instance currently servicing this request; or NULL
 */
struct sns_sensor_instance *sns_sensor_util_find_instance(
  struct sns_sensor const *sensor,
  struct sns_request const *request,
  sns_sensor_uid const *suid);

/**
 * Find the shared instance for a Sensor.
 *
 * Many physical drivers share a single Instance across all associated
 * Sensors.  This function finds and returns that single Instance if it exists.
 *
 * @param[i] sensor A Sensor which shares a single Instance
 *
 * @return Shared Instance, or NULL
 */
struct sns_sensor_instance* sns_sensor_util_get_shared_instance(
    struct sns_sensor *sensor);

/**
 * Find an existing instance that can handle this new request.
 *
 * @param[i] sensor Sensor which received the request
 * @param[i] request New incoming request from sns_sensor_api::set_client_request
 * @param[i] cb Callback function for each existing Instance
 *
 * @return Instance reference if found, otherwise NULL
 */
struct sns_sensor_instance* sns_sensor_util_find_instance_match(
    struct sns_sensor const *sensor, struct sns_request const *request,
    sns_sensor_util_instance_match_cb cb);

/**
 * Return the SUID of the SUID Lookup Sensor.
 */
sns_sensor_uid sns_get_suid_lookup(void);

/**
 * Process data from the given streams in synchronized
 * chronological order.
 *
 * - Non-data events will be processed as soon as they are
 *   available
 * - Data events on the stream will be processed only when there
 *   is at least one data event available on all streams
 * - Data events across streams will be processed in
 *   chronological order
 *
 * @param streams pointer to array of data streams
 * @param info TODO
 * @param num_streams number of data streams
 * @param process_event_cb callback function for processing one
 *                         event from a data stream
 *
 * @param userdata userdata to be supplied to each callback
 *
 * @return sns_rc SNS_RC_SUCCESS if everything went fine, error
 *         status otherwise
 */
sns_rc sns_process_streams_in_timesync(sns_data_stream **streams,
                                       sns_timesync_event_info *info, int num_streams,
                                       process_stream_event process_event_cb,
                                       void *userdata);


/**
 * Find the minimum batch period from among the clients of an
 * instance
 *
 * @param[i] instance The Sensor instance being evaluated
 * @param[i] suid The specific suid for which clients are being
 *       evaluated
 * @param[i] min_batch_period The min period of batching
 * @param[i] max_flush_period The max period of flushing
 * @param[i] max_batch        The max batch flag
 *
 * @return sns_rc SNS_RC_SUCCESS if a minimum batch period and
 *                               minimum flush period was
 *                               successfully determined
 *                SNS_RC_FAILED  if there was an error in
 *                               determining the batch period or
 *                               flush period
 *                SNS_RC_INVALID_STATE if there are no clients
 *                               for the instance for this suid
 */
sns_rc
sns_sensor_util_find_min_batch_period(sns_sensor_instance *instance,
                                      sns_sensor_uid const *suid,
                                      uint32_t *min_batch_period,
                                      uint32_t *max_flush_period,
                                      bool     *max_batch);

/**
 * Decide if all requests have max batch set to true.
 *
 * @param[i] instance The Sensor instance being evaluated
 * @param[i] suid The specific suid for which clients are being
 *       evaluated
 *
 * @return bool   True           If all requests have the max
 *                               batch set to true.
 *
 *                False          If there is a request which does
 *                               not have max batch set to true.
 */
bool
sns_sensor_util_decide_max_batch(sns_sensor_instance *instance,
                                 sns_sensor_uid const *suid);

/**
 * Send a flush event.
 *
 * @param[i] uid     UID of the Sensor.
 * @param instance   reference to the instance of the Sensor.
 *
 * @return none
 */
void sns_sensor_util_send_flush_event(sns_sensor_uid const *uid,
                                    sns_sensor_instance *instance);
