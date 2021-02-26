#pragma once
/*=============================================================================
  @file sns_pb_util.h

  Utility functions provided by Qualcomm for use by Sensors in association
  with nanoPB.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "pb_decode.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_time.h"

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

struct sns_sensor_instance;
struct sns_sensor_uid;
struct sns_sensor_event;
struct sns_request;

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Structure to be used with pb_decode_simple_cb.
 */
typedef struct pb_simple_cb_arg
{
  /* Where to place the decoded message */
  void *decoded_struct;
  /* Message definition associated with decoded_struct */
  const pb_field_t *fields;
} pb_simple_cb_arg;

/**
 * Structure to be used with pb_encode_float_arr_cb & pb_decode_float_arr_cb
 */
typedef struct pb_float_arr_arg
{
  /* Where to place the decoded float array */
  float *arr;
  /* Length of the float array */
  uint8_t arr_len;
  /* Array index used for decoding.
     Must be initialized to 0 prior to calling pb_decode_float_arr_cb*/
  uint8_t *arr_index;
} pb_float_arr_arg;

/**
 * Callback structure for decode_string and encode_string
 */
typedef struct pb_buffer_arg
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} pb_buffer_arg;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

/**
 * Decode a nested message.  This function is intended to be used as a
 * callback function during the decode process.
 *
 * @note This utility function can only be used for simple messages without
 * nested structures or arrays.
 *
 * @see pb_callback_s::decode
 */
bool pb_decode_simple_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg);

/**
 * Encode a nested array of floats  This function is intended to
 * be used as a callback function during the encode process.
 *
 * @see pb_callback_s::encode
 */
bool pb_encode_float_arr_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg);
/**
 * Decode a nested array of floats  This function is intended to
 * be used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
bool pb_decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg);

/**
 * Encode a string into a field. This function is intended to be used as a
 * callback function during the encode process.
 *
 * @see pb_callback_s::encode
 */
bool pb_encode_string_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg);

/**
 * Decode a string from a field. This function is intended to be used as a
 * callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
bool pb_decode_string_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg);

/**
 * Encode a sns_std_request and its Sensor-specific payload onto the given
 * buffer.
 *
 * @param[i] buffer Where the encoded sns_std_request shall be placed
 * @param[i] buffer_len Size of buffer
 * @param[i] request_payload The decoded structure to be encoded and placed
 *                           within sns_std_request::payload
 * @param[i] payload_fields Message definition associated with request_payload
 * @param[i] request sns_std_request configuration; NULL to use default values
 *
 * @return Size of encoded message
 */
size_t pb_encode_request(void *buffer, size_t buffer_len,
    void const *request_payload, pb_field_t const *payload_fields,
    sns_std_request *request);

/**
 * Decodes a request message.
 *
 * @param[i] req Request to decode
 * @param[i] payload_fields Message definition associated with request_payload
 * @param[i] request sns_std_request configuration; NULL to use default values
 * @param[o] decoded_req Destination for the decoded request
 *
 * @return True if the request is successfully decoded
 */
bool pb_decode_request(struct sns_request const *req, pb_field_t const *payload_fields,
    sns_std_request *request, void *decoded_req);

/**
 * Encode an outgoing Event message; then publish that Event.
 *
 * @note This utility function can only be used for simple events without
 * nested structures or arrays.
 *
 * @param[i] instance Source of the event
 * @param[i] fields Message definition
 * @param[i] c_struct Decoded Event to send
 * @param[i] timestamp Timestamp of the Event
 * @param[i] message_id Message ID of the Event
 * @param[i] sns_sensor_uid Sensor UID of the published event.
 *       This can be NULL if Instance is not shared between
 *       Sensors.
 *
 * @return True upon success, false otherwise
 */
bool pb_send_event(struct sns_sensor_instance *instance,
    pb_field_t const *fields, void const *c_struct, sns_time timestamp,
    uint32_t message_id, struct sns_sensor_uid const *sensor_uid);

/**
 * Determine encoded size of sns_std_sensor_event with variable length
 * floating point data payload. Construct, sns_std_sensor_event message
 * is as defined in sns_std_sensor.proto.
 *
 * @param[i] data Data array of floats to be used as the data
 *                payload as defined in sns_std_sensor.proto
 * @param[i] data_len Length of the data array
 *
 * @return Size of the encoded message
 */
size_t pb_get_encoded_size_sensor_stream_event(float const *data,
    uint8_t data_len);

/**
 * Construct, Encode and Publish an outgoing sns_std_sensor_event as
 * defined in sns_std_sensor.proto.
 *
 * @param[i] instance Source of the event
 * @param[i] sns_sensor_uid Sensor UID of the published event
 * @param[i] timestamp Timestamp of the published event
 * @param[i] message_id Message ID of the published event
 * @param[i] sample_status the sample status of the published
 *       event as defined in sns_std_sensor.proto
 * @param[i] data Data array of floats to be used as the data
 *       payload as defined in sns_std_sensor.proto
 * @param[i] data_len Length of the data array
 * @param[i] encoded_len Length of the encoded message as
 *       determined by pb_get_encoded_size_sensor_stream_event
 *
 * @return sns_rc
 */
sns_rc pb_send_sensor_stream_event(struct sns_sensor_instance *instance,
    struct sns_sensor_uid const *sensor_uid, sns_time timestamp,
    uint32_t message_id, sns_std_sensor_sample_status sample_status, float const *data,
    uint8_t data_len, size_t encoded_len);

/**
 * Decode an sns_suid_event with variable number of SUIDs
 * @see pb_callback_s::decode
 *
 * @param[i] stream nanopb input stream
 * @param[i] field  nanopb field for decoding
 * @param[i] arg    type sns_suid_search
 *
 * @return true on success, false on failure
 */
bool pb_decode_suid_event(
   pb_istream_t *stream, const pb_field_t *field, void **arg);
