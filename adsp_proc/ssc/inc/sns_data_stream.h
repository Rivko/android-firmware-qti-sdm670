#pragma once
/*=============================================================================
  @file sns_data_stream.h

  Represents a Client Sensor <-> Service Sensor connection.

  Each data stream logically contains series of unprocessed input events.  Each
  of these events is from the same source Sensor Instance, but may contain
  heterogeneous data, differentiated by their message ID.

  The oldest sample not yet removed from this data queue can be read/accessed
  by 'peek_input', let us call it "event X".  Once event X has been consumed
  by the Sensor, 'get_next_input' should be called; the Framework will free
  the memory associated with X and perform all appropriate book-keeping.  In
  addition to removing event X, 'get_next_input' will return the next event
  on the event queue (if applicable), event Y.  An immediate call to
  'peek_input' will now result in event Y being returned.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include "sns_request.h"
#include "sns_sensor_event.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_data_stream
{
  struct sns_data_stream_api *api;
} sns_data_stream;

typedef struct sns_data_stream_api
{
  uint32_t struct_len;

  /**
   * Send a request to some other service/Sensor.  This request may
   * update or replace the existing stream, depending on the Sensor
   * specification.
   *
   * @param[io] data_stream Data stream on which to send the request
   * @param[i] Request to be sent; Framework will copy request
   *
   * @return
   * SNS_RC_INVALID_TYPE - Request ID not valid
   * SNS_RC_INVALID_STATE - Stream is no longer available; create again
   * SNS_RC_SUCCESS
   */
  sns_rc (*send_request)(
    sns_data_stream *data_stream,
    sns_request *request);

  /**
   * Initiate a flush on the connection associated with sensorUID.
   *
   * @note This is a helper function; clients may also initiate a flush
   * by generating a flush request message, and sending it via send_request.
   *
   * @param[io] data_stream Data stream on which to initiate the flush
   *
   * @return
   * SNS_RC_INVALID_STATE - Stream is no longer available; create again
   * SNS_RC_SUCCESS
   */
  sns_rc (*initiate_flush)(
    sns_data_stream *data_stream);

  /**
   * Retrieve a pointer to the oldest unprocessed input sample associated with
   * this data stream from the event queue.  This event is a single, logical
   * sample, as produced and published by the source Sensor.
   *
   * @note Multiple sequential calls to this function will return the same
   * pointer.
   *
   * @param[io] data_stream Data stream from which to get an event
   *
   * @return Next unprocessed event on the queue; NULL if no events remain
   */
  sns_sensor_event* (*peek_input)(
    sns_data_stream *data_stream);

  /**
   * Remove the current event from the input queue (the event that would
   * be returned via peek_input).  Return the next unprocessed event from the
   * event queue.
   *
   * Once this function returns, there is no means to retrieve the removed
   * Event again; the data has been freed, and its memory should not be
   * accessed.
   *
   * @param[io] data_stream Data stream from which to get an event
   *
   * @return The next unprocessed event on the queue (after the removal occurs)
   *         NULL if no further events remain
   */
  sns_sensor_event* (*get_next_input)(
    sns_data_stream *data_stream);

  /**
   * Lookup the current number of input pending on this data stream.  This
   * value may change at any time, and should not be treated as precise.
   *
   * @note Do no rely on this value to assume valid input from peek_input.
   *
   * @param[io] data_stream Data stream from which to get the input count
   *
   * @return Number of input events (aka samples) available for processing
   */
  uint32_t (*get_input_cnt)(
    sns_data_stream *data_stream);
} sns_data_stream_api;
