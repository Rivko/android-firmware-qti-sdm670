#pragma once
/*=============================================================================
  @file sns_fw_data_stream.h

  Represents a Client Sensor <-> Service Sensor connection.

  @note All Data Stream functions may only be called while holding the data
  stream owner's library_lock.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include "sns_data_stream.h"
#include "sns_fw_attribute_service.h"
#include "sns_isafe_list.h"
#include "sns_island_util.h"
#include "sns_osa_lock.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_fw_request;
struct sns_fw_sensor_event;
struct sns_fw_sensor_instance;
struct sns_fw_sensor;
struct sns_sensor_library;
struct sns_sensor_uid;

/*=============================================================================
  Preprocessor Definitions
  ===========================================================================*/

#define SNS_DATA_STREAM_EVENT_CNT 20

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* Pointers to data in global circular buffer. */
typedef struct sns_events_bin
{
  sns_isafe_list_item list_entry;
  struct sns_fw_sensor_event *events[SNS_DATA_STREAM_EVENT_CNT];
} sns_events_bin;

/**
 * @note Sensor Instances cannot be moved to/from Island while it has
 * outstanding events pending processing by its clients.
 */
typedef struct sns_fw_data_stream
{
  sns_data_stream stream;

  /* List entry for the dst_instance::streams or dst_sensor:streams list.
   * This is the creator/owner of the stream, and receives all events. */
  sns_isafe_list_item list_entry_client;

  /* Sensor for the source of this data. */
  struct sns_fw_sensor *data_source;
  /* Client-generated configuration request;
   * Protected by data_source->library->state_lock */
  struct sns_fw_request *client_request;

  /* How many unprocessed events are pending on the buffer */
  uint16_t event_cnt;
  uint8_t read_index;  /* Read index into the first events entry*/
  uint8_t write_index;  /* Write index into the last events entry*/

  /* All pending events for this stream; is empty when no events are pending;
   * sns_events_bin */
  sns_isafe_list event_bins;

  /* Protects events, event_cnt, read_index, and write_index.
   * client_request protected by the dst_instance or dst_sensor's lock.
   * All other data is guaranteed constant and valid as long as it exists on
   * the pending_events list, or on a data_streams list. */
  sns_osa_lock *events_lock;

  /* The destination of this data stream;
   * Will not be modified until this structure is destroyed
   * (always done while holding data_source->library->state_lock) */
  struct sns_fw_sensor_instance *dst_instance;
  struct sns_fw_sensor *dst_sensor;

  /* Protected by events_lock */
  enum {
    /* Data Stream has been created and is available for use */
    SNS_DATA_STREAM_AVAILABLE = 0,
    /* The client has requested that this data stream be removed;
     * waiting on the data source to acknowledge receipt */
    SNS_DATA_STREAM_WAIT_SRC = 1,
    /* The "Destroy Complete" event has been sent, and once received,
     * this data stream is ready to be freed and cleared. */
    SNS_DATA_STREAM_WAIT_RECEIPT = 2,
  } removing;

  /* Data Stream can be accessed in Island mode if
   * 1. Source Sensor is available in Island Mode
   * 2. Destination Sensor or Sensor Instance is available in Island Mode
   * 3. All event_bins are available in island mode
   * 4. client_request is available in island mode
   * 5. This data stream is available in island mode
   */
  sns_island_state island_operation;

  /* Events/Requests sent on this stream should be treated with priority */
  bool event_priority;
  bool req_priority;

  /* Whether the data stream has been processed since last added to task list;
   * Must be atomically read/written. */
  unsigned int unprocessed_events;

  /* Stream Integrity checksum; must be at end of structure
   * Purpose: gracefully catch and handle Sensor's errors */
  uint32_t stream_integrity;
} sns_fw_data_stream;

/* Callback structure used to find a Sensor with the specified SUID;
 * Specifies the type of data stream to be created. */
typedef struct sns_find_sensor_arg
{
  sns_sensor_uid *suid;
  struct sns_fw_sensor *sensor;
  sns_attr_priority priority;
  bool available;
} sns_find_sensor_arg;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/**
 * Allocate and initialize a new data stream object.
 *
 * Exactly one of dst_sensor or dst_instance may be non-NULL.
 *
 * @note src_sensor->sensor cannot be safely dereferenced within this function.
 *
 * @return
 * SNS_RC_NOT_AVAILABLE - Sensor has not yet been marked as available
 * SNS_RC_SUCCESS;
 */
sns_rc data_stream_init( struct sns_fw_sensor *dst_sensor,
    struct sns_fw_sensor_instance *dst_instance,
    sns_find_sensor_arg *src_sensor, sns_fw_data_stream **data_stream);

/**
 * Function called by a client to destroy a data stream.  The stream must
 * be independently removed from the client's list.
 *
 * @note This request will be handled ansynchronously; the client is
 * guaranteed to receive no further events after this function call returns.
 */
void sns_data_stream_deinit(struct sns_fw_data_stream *stream);

/**
 * Handle receipt of the "destroy complete" event.  Receipt of this event
 * indicates that no further events will be received on this connection,
 * and the framework may safely free the stream and its remaining state.
 *
 * @note if the client instance is in the process of being removed, check
 * if this is the final stream in its list.  if so, also remove and free that
 * instance.
 *
 * @return
 * SNS_RC_SUCCESS
 */
sns_rc sns_data_stream_destroy_complete(struct sns_fw_data_stream *stream);

/**
 * Validate a data stream
 *
 * @param[i] stream Data stream to be validated
 *
 * @return true if stream is a valid data stream; false otherwise
 */
bool sns_data_stream_validate(sns_fw_data_stream *stream);

/* See sns_data_stream_api::peek_input */
struct sns_fw_sensor_event* sns_data_stream_peek_input(
    sns_fw_data_stream *stream);

/* See sns_data_stream_api::get_next_input */
struct sns_fw_sensor_event* sns_data_stream_get_next_input(
    sns_fw_data_stream *stream);

/* See sns_data_stream_api::initiate_flush */
sns_rc sns_data_stream_initiate_flush(sns_fw_data_stream *stream);

/* See sns_data_stream_api::get_input_cnt */
uint32_t sns_data_stream_get_input_cnt(sns_fw_data_stream *stream);
