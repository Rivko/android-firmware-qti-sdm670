/*=============================================================================
  @file sns_async_com_port_pb_utils.c

  Utility functions provided by Qualcomm for use by Sensors in their use of
  nanoPB for Async Com Port.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_printf_int.h"
#include "sns_std.pb.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

/* Buffer size to be used for encoding async com port messages */
#define SNS_ASYNC_COM_PORT_PB_DEF_BUFFER_SIZE 50

/* Buffer size to be used for encoding sns_request messages */
#define SNS_ASYNC_COM_PORT_REQ_MSG_BUFFER_SIZE 50

/*=============================================================================
  Type definitions
  ===========================================================================*/

typedef struct sns_async_com_port_enc_cb_arg
{
  /* Vectors array */
  sns_port_vector *array;
  /* Vector array count */
  uint32_t array_count;
  /* Current index in array */
  uint32_t curr_index;
} sns_async_com_port_enc_cb_arg;

typedef struct sns_async_com_port_dec_cb_arg
{
  /* Current vector being decoded */
  sns_port_vector vector;
  /* Callback provided by user */
  sns_async_com_port_process_vector callback;
  /* Args provided by user */
  void *user_args;
  /* Overwrite the vector size */
  bool overwrite_num_bytes;
  /* Bytes in stream */
  uint32_t buffer_size;
}sns_async_com_port_dec_cb_arg;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* Encode
 * sns_async_com_port_vector_rw.sns_async_com_port_vector[].buffer
 */
SNS_SECTION(".text.sns") bool
sns_async_com_port_encode_buffer(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_async_com_port_enc_cb_arg *cb_arg = (sns_async_com_port_enc_cb_arg*)*arg;
  if(!pb_encode_tag_for_field(stream, field))
  {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error Encoding request");
      return false;
  }
  if(!pb_encode_string(stream, cb_arg->array[cb_arg->curr_index].buffer,
    cb_arg->array[cb_arg->curr_index].bytes))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Error Encoding request");
    return false;
  }
  return true;
}

/* Encode each sns_async_com_port_vector_rw.sns_async_com_port_vector[] */
SNS_SECTION(".text.sns") bool
sns_async_com_port_encode_each_vector(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_async_com_port_enc_cb_arg *cb_arg = (sns_async_com_port_enc_cb_arg*)*arg;

  for(cb_arg->curr_index = 0; cb_arg->curr_index < cb_arg->array_count; cb_arg->curr_index += 1)
  {
    sns_async_com_port_vector vector = sns_async_com_port_vector_init_default;
    vector.reg_addr = cb_arg->array[cb_arg->curr_index].reg_addr;
    vector.has_num_bytes = true;
    vector.num_bytes = cb_arg->array[cb_arg->curr_index].bytes;
    vector.operation = (cb_arg->array[cb_arg->curr_index].is_write)?
      SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE : SNS_ASYNC_COM_PORT_OPERATION_TYPE_READ;
    if(NULL != cb_arg->array[cb_arg->curr_index].buffer)
    {
      vector.buffer.funcs.encode = &sns_async_com_port_encode_buffer;
      vector.buffer.arg = *arg;
    }

    if(!pb_encode_tag_for_field(stream, field))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error Encoding request");
      return false;
    }
    if(!pb_encode_submessage(stream, sns_async_com_port_vector_fields, &vector))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error Encoding request");
      return false;
    }
  }

  return true;
}

/* Encode sns_async_com_port_vector_rw */
SNS_SECTION(".text.sns") bool
sns_async_com_port_encode_request_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_async_com_port_enc_cb_arg *cb_arg = (sns_async_com_port_enc_cb_arg*)*arg;
  sns_async_com_port_vector_rw vector_rw;

  vector_rw.num_vectors = cb_arg->array_count;
  vector_rw.vectors.funcs.encode = &sns_async_com_port_encode_each_vector;
  vector_rw.vectors.arg = *arg;


  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_submessage(stream, sns_async_com_port_vector_rw_fields, &vector_rw))
    return false;

  return true;
}

/* Decode
 * sns_async_com_port_vector_rw.sns_async_com_port_vector[].buffer
 */
SNS_SECTION(".text.sns") bool
sns_async_com_port_decode_buffer(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_async_com_port_dec_cb_arg *dec_args = (sns_async_com_port_dec_cb_arg *)*arg;
  if(0 != stream->bytes_left)
  {
      dec_args->vector.buffer = (uint8_t *)stream->state;
      dec_args->buffer_size = stream->bytes_left;
      dec_args->overwrite_num_bytes = true;
      stream->state = (pb_byte_t*)stream->state + stream->bytes_left;
      stream->bytes_left = 0;
  }
  return true;
}

/* Decode each
 * sns_async_com_port_vector_rw.sns_async_com_port_vector[]
 */

SNS_SECTION(".text.sns") bool
sns_async_com_port_decode_each_vector(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_async_com_port_dec_cb_arg *dec_args = (sns_async_com_port_dec_cb_arg *)*arg;
  sns_async_com_port_vector vector = sns_async_com_port_vector_init_default;
  vector.buffer.funcs.decode = &sns_async_com_port_decode_buffer;
  vector.buffer.arg = *arg;
  dec_args->vector.buffer = NULL;
  dec_args->buffer_size = 0;
  dec_args->overwrite_num_bytes = false;

  if(!pb_decode(stream,sns_async_com_port_vector_fields,&vector))
    return false;
   dec_args->vector.is_write = (SNS_ASYNC_COM_PORT_OPERATION_TYPE_WRITE == vector.operation)?
     true: false;
   dec_args->vector.reg_addr = vector.reg_addr;
   dec_args->vector.bytes = (vector.has_num_bytes)?vector.num_bytes:0;
   if(dec_args->overwrite_num_bytes)
   {
     dec_args->vector.bytes = dec_args->buffer_size;
   }
  dec_args->callback(&dec_args->vector, dec_args->user_args);

  return true;
}


/*=============================================================================
  Public Functions
  ===========================================================================*/
SNS_SECTION(".text.sns") size_t sns_ascp_estimate_pb_buffer_size( sns_port_vector vectors[],
                                         uint32_t        num_vectors,
                                         bool            wrap_inside_sns_pb_req)
{
  uint32_t buffer_size = 0;
  if( NULL != vectors && 0 != num_vectors )
  {
    // Step 1: Compute the working buffer size
    uint32_t i;
    for(i = 0; i < num_vectors; i++)
    {
      buffer_size += SNS_ASYNC_COM_PORT_PB_DEF_BUFFER_SIZE +
        ((vectors[i].is_write) ? 0 : vectors[i].bytes);
    }
    if(wrap_inside_sns_pb_req)
    {
      buffer_size += SNS_ASYNC_COM_PORT_REQ_MSG_BUFFER_SIZE;
    }
  }
  return buffer_size;
}

SNS_SECTION(".text.sns") bool sns_ascp_create_encoded_vectors_buffer(
  sns_port_vector vectors[],
  uint32_t        num_vectors,
  bool            wrap_inside_sns_pb_req,
  uint8_t         *buffer,
  uint32_t        buffer_size,
  uint32_t        *encoded_len)
{
  if( NULL != vectors && 0 != num_vectors &&
    NULL != buffer && NULL != encoded_len )
  {

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
    sns_std_request temp;
    sns_async_com_port_vector_rw vector_rw;
    sns_memzero(&temp, sizeof(sns_std_request));
    sns_memzero(&vector_rw, sizeof(sns_async_com_port_vector_rw));
    void *msg;
    pb_field_t const *fields;
    sns_async_com_port_enc_cb_arg arg = (sns_async_com_port_enc_cb_arg)
      {
        .array = vectors,
        .array_count = num_vectors,
        .curr_index = 0,
      };

    if(wrap_inside_sns_pb_req)
    {
      // Embed inside a sns_std_request message
      temp.payload.funcs.encode = &sns_async_com_port_encode_request_cb;
      temp.payload.arg = &arg;
      msg = &temp;
      fields = sns_std_request_fields;
    }
    else
    {
      // Buffer will contain only the sns_async_com_port_vector_rw message
      vector_rw.num_vectors = num_vectors;
      vector_rw.vectors.funcs.encode = &sns_async_com_port_encode_each_vector;
      vector_rw.vectors.arg = &arg;
      msg = &vector_rw;
      fields = sns_async_com_port_vector_rw_fields;
    }

    if(!pb_encode(&stream, fields, msg))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error Encoding request");
      return false;
    }

    *encoded_len = stream.bytes_written;

    return true;
  }
  return false;
}

SNS_SECTION(".text.sns") bool sns_ascp_for_each_vector_do(pb_istream_t *stream, sns_async_com_port_process_vector callback, void *user_args)
{
  sns_async_com_port_vector_rw vector_rw = sns_async_com_port_vector_rw_init_default;
  sns_async_com_port_dec_cb_arg arg = (sns_async_com_port_dec_cb_arg)
  {
    .vector = (sns_port_vector){ .is_write = false, .reg_addr = 0, .bytes = 0, .buffer = NULL },
    .overwrite_num_bytes = false,
    .buffer_size = 0,
    .user_args = user_args,
    .callback = callback,
  };
  vector_rw.vectors.funcs.decode = &sns_async_com_port_decode_each_vector;
  vector_rw.vectors.arg = &arg;

  if(!pb_decode(stream, sns_async_com_port_vector_rw_fields, &vector_rw))
    return false;
  return true;
}
