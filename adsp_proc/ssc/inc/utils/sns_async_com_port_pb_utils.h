/*=============================================================================
  @file sns_async_com_port_pb_utils.h

  Utility functions provided by Qualcomm for use by Sensors in their use of
  nanoPB for Async Com Port.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/
#pragma once

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_async_com_port.pb.h"
#include "sns_com_port_types.h"
#include "sns_sensor_instance.h"

/*=============================================================================
  Type definitions
  ===========================================================================*/
/* sns_async_com_port_process_vector
 *
 * Callback used to access decoded data
 *
 * @param[o] vector         The decoded vector
 * @param[o] user_args      User supplied args
 */
typedef void (*sns_async_com_port_process_vector)(
  sns_port_vector       *vector,
  void                  *user_args);

/* sns_ascp_estimate_pb_buffer_size
 *
 * Helper function to estimate the size of a buffer required to hold the
 * encoded nanoPB message. The estimate will never be smaller than the actual
 * required size
 *
 * @param[i] vectors                 Array of vectors to be encoded
 * @param[i] num_vectors             Number of vectors to be encoded
 * @param[i] wrap_inside_sns_pb_req  Set to true to embed the message in a sns_std_request message
 *
 * @return Estimated buffer size required to hold the encoded message.
 */

size_t sns_ascp_estimate_pb_buffer_size( sns_port_vector vectors[],
                                         uint32_t        num_vectors,
                                         bool            wrap_inside_sns_pb_req);


/* sns_ascp_create_encoded_vectors_buffer
 *
 * Helper function to encode an array of sns_port_vector into a single
 * sns_async_com_port_vector_rw message. Optionally, it can add the
 * sns_async_com_port_vector_rw message as a payload to the sns_std_request message
 *
 * @param[i] vector[]                An array of vectors that will get encoded into the message
 * @param[i] vector_size             Number of elements in the array
 * @param[i] wrap_inside_sns_pb_req  Set to true to embed the message in a sns_std_request message
 * @param[i] buffer                  Pointer to the buffer.
 * @param[i] buffer_size             Size of the buffer
 * @param[o] encoded_len             Number of bytes written into the buffer
 *
 * @return true if encode was successful
 *         false otherwise
 */
bool sns_ascp_create_encoded_vectors_buffer(
  sns_port_vector vector[],
  uint32_t        vector_size,
  bool            wrap_inside_sns_pb_req,
  uint8_t         *buffer,
  uint32_t        buffer_size,
  uint32_t        *encoded_len);

/* sns_ascp_for_each_vector_do
 *
 * Helper function to decode a sns_async_com_port_vector_rw. The provided
 * callback will be called once for each sns_port_vector that is extracted
 * from the encoded message. The decoded vector and the decoded buffer provided
 * to the callback are temporary. They must be copied over within the callback
 * to preserve them.
 *
 * @param[i] stream      PB stream.
 * @param[i] callback    Callback to be called for each vector
 * @param[i] user_args   User supplied args that will be provided in the callback
 *
 * @return true if successfully decoded
 *         false otherwise
 */
bool sns_ascp_for_each_vector_do(
  pb_istream_t                      *stream,
  sns_async_com_port_process_vector callback,
  void                              *user_args);

