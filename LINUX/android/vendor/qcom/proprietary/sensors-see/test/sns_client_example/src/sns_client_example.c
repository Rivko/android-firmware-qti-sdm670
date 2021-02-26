/*=============================================================================
  @file sns_client_example.c

  Example client application.  This application will open a QMI connection to
  the Sensors Service (which resides on the SSC).  It will send a SUID Lookup
  Request for the data type "accel".  For each found SUID, this application
  will send an Attributes Request, and subsequently an enable request with
  the sampling period set at 10Hz.  The connection will be terminated after
  10 seconds of streaming.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <inttypes.h>

#include "qmi_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * Utility structure used to encode a string or already encoded message
 * into another Protocol Buffer message.
 */
typedef struct sns_buffer_arg
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} sns_buffer_arg;

/**
 * Structure to be used with pb_decode_float_arr_cb
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

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* QMI Client handle created within create_client_conn(), and used to send
 * a request to the Sensor service */
static qmi_client_type qmi_sensors_handle = NULL;

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* SUID for the accel Sensor, as received from the SUID Lookup Sensor */
static sns_std_suid accel_suid = (sns_std_suid){ .suid_low = 0, .suid_high = 0 };

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
static bool
encode_payload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_buffer_arg *info = (sns_buffer_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, info->buf, info->buf_len))
    return false;

  return true;
}

/**
 * Decode a string of bytes from a field. This function is intended to be
 * used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
static bool
decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_buffer_arg *data = (sns_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

/**
 * Decode an array of float values, such as the array within the sensor
 * sample data.
 */
bool
decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  pb_float_arr_arg const *float_arr = (pb_float_arr_arg const*)*arg;

  if(!pb_decode_fixed32(stream, &(float_arr->arr[*float_arr->arr_index])))
      return false;

  (*float_arr->arr_index)++;

  return true;
}

/**
 * Handle an incoming QMI response message from the Sensors Service.
 */
static void
resp_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *resp_c_struct, unsigned int resp_c_struct_len,
    void *resp_cb_data, qmi_client_error_type transp_err)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(resp_cb_data);

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Received response; error %i, msg_id %i, size %i",
      transp_err, msg_id, resp_c_struct_len);

  if(QMI_NO_ERR == transp_err) {
    sns_client_resp_msg_v01 *resp = (sns_client_resp_msg_v01*)resp_c_struct;

    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Received client ID %" PRId64 "result %i",
      resp->client_id, resp->result);
  }

  free(resp_c_struct);
}

/**
 * Send a QMI request message.
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] req_msg Completed request message to be sent
 *
 * @return true on success, false otherwise
 */
static bool
send_qmi_req(qmi_client_type qmi_handle, sns_client_req_msg_v01 const *req_msg)
{
  bool rv = false;
  size_t resp_len = sizeof(sns_client_resp_msg_v01);
  void *resp = malloc(resp_len);

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Sending request payload len %i", req_msg->payload_len);

  if(NULL == resp) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Unable to allocate sns_client_resp_msg_v01");
  } else {
    qmi_txn_handle txn_handle;

    qmi_client_error_type qmi_err = qmi_client_send_msg_async(
        qmi_handle, SNS_CLIENT_REQ_V01, (void*)req_msg, sizeof(*req_msg),
        resp, resp_len, resp_msg_cb, resp, &txn_handle);

    if(QMI_NO_ERR != qmi_err) {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "qmi_client_send_msg_async error %i", qmi_err);
      free(resp);
    } else {
      rv = true;
    }
  }

  return rv;
}

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 */
static bool
send_basic_req(qmi_client_type qmi_handle, sns_buffer_arg const *payload, sns_std_suid suid, uint32_t msg_id)
{
  sns_client_request_msg pb_req_msg = sns_client_request_msg_init_default;
  sns_client_req_msg_v01 req_msg;
  pb_ostream_t stream = pb_ostream_from_buffer(req_msg.payload, SNS_CLIENT_REQ_LEN_MAX_V01);

  pb_req_msg.suid = suid;
  pb_req_msg.msg_id = msg_id;
  pb_req_msg.request.has_batching = false;
  pb_req_msg.request.payload.funcs.encode = &encode_payload;
  pb_req_msg.request.payload.arg = (void*)payload;

  if(!pb_encode(&stream, sns_client_request_msg_fields, &pb_req_msg)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error Encoding request: %s", PB_GET_ERROR(&stream));
    return false;
  } else {
    req_msg.payload_len = stream.bytes_written;
    send_qmi_req(qmi_handle, &req_msg);
    return true;
  }
}

/**
 * Create an encoded Attribute request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_attr_req(void **encoded_req)
{
  size_t encoded_req_size;
  sns_std_attr_req attr_req = sns_std_attr_req_init_default;

  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_attr_req_fields, &attr_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_attr_req_fields, &attr_req)) {
       __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send an attribute request for the specified SUID.
 */
static bool
send_attr_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_attr_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid, SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    free(encoded_req);
  }

  return rv;
}

/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] data_type Sensor type to search for
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_suid_req(char const *data_type, void **encoded_req)
{
  size_t encoded_req_size;
  sns_suid_req suid_req = sns_suid_req_init_default;

  *encoded_req = NULL;

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &((sns_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;

  if(!pb_get_encoded_size(&encoded_req_size, sns_suid_req_fields, &suid_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_suid_req_fields, &suid_req)) {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send a request to the SUID Lookup Sensor for the complete list of SUIDs
 *
 * @param[i] qmi_handle QMI connecton created in create_client_conn
 * @param[i] data_type Sensor type to search for
 *
 * @return true if request was sent successfully, false otherwise
 */
static bool
send_suid_req(qmi_client_type qmi_handle, char const *data_type)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_suid_req(data_type, &encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ);
    free(encoded_req);
  }

  return rv;
}

/**
 * Create an encoded Accel enable request message.
 *
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static size_t
get_encoded_enable_req(void **encoded_req)
{
  size_t encoded_req_size;
  sns_std_sensor_config config_req = sns_std_sensor_config_init_default;
  config_req.sample_rate = 10;  // Stream at 10Hz

  *encoded_req = NULL;

  if(!pb_get_encoded_size(&encoded_req_size, sns_std_sensor_config_fields, &config_req)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE", "pb_get_encoded_size error");
  } else {
    void *encoded_buffer = malloc(encoded_req_size);
    pb_ostream_t stream = pb_ostream_from_buffer(encoded_buffer, encoded_req_size);

    if(!pb_encode(&stream, sns_std_sensor_config_fields, &config_req)) {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "Error Encoding attribute request: %s", PB_GET_ERROR(&stream));
    } else {
      *encoded_req = encoded_buffer;
    }
  }

  return NULL == *encoded_req ? 0 : encoded_req_size;
}

/**
 * Send an Accel enable request for the specified SUID.
 */
static bool
send_enable_req(qmi_client_type qmi_handle, sns_std_suid *suid)
{
  bool rv = false;
  void *encoded_req = NULL;
  size_t encoded_req_len;

  encoded_req_len = get_encoded_enable_req(&encoded_req);
  if(NULL != encoded_req) {
    sns_buffer_arg payload = (sns_buffer_arg)
      { .buf = encoded_req, .buf_len = encoded_req_len };
    rv = send_basic_req(qmi_handle, &payload, *suid, SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG);
    free(encoded_req);
  }

  return rv;
}

/**
 * QMI Error callback.  This would be our notification if the QMI connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See qmi_client_error_cb. */
static void
error_cb(qmi_client_type user_handle,
    qmi_client_error_type error, void *err_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(err_cb_data);

  __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "QMI error callback %i", error);
}

/**
 * Decode an attribute value.
 *
 * Once we find the attribute value we have been looking for (for example,
 * to differentiate two sensors with data type "accel"), we send an enable
 * request.
 */
static bool
decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  sns_buffer_arg str_data = (sns_buffer_arg){ .buf = NULL, .buf_len = 0 };

  value.str.funcs.decode = &decode_payload;
  value.str.arg = &str_data;

  value.subtype.values.funcs.decode = &decode_attr_value;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(value.has_flt)
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Attribute float: %f", value.flt);
  else if(value.has_sint)
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Attribute int: %"PRIi64, value.sint);
  else if(value.has_boolean)
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Attribute boolean: %i", value.boolean);
  else if(NULL != str_data.buf)
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Attribute string: %s", (char*)str_data.buf);
  else if(value.has_subtype)
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE", "Attribute nested");
  else
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE", "Unknown attribute type");

  return true;
}

/**
 * Decode a single attribute from the attributes array.
 */
static bool
decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr attribute = sns_std_attr_init_default;
  attribute.value.values.funcs.decode = &decode_attr_value;
  attribute.value.values.arg = NULL;

  if(!pb_decode(stream, sns_std_attr_fields, &attribute)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding attribute: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Attribute ID %i", attribute.attr_id);

  if(SNS_STD_SENSOR_ATTRID_NAME == attribute.attr_id) {
    // Based on some logic, choose this Accel Sensor and send enable request
    send_enable_req(qmi_sensors_handle, &accel_suid);
  }

  return true;
}

/**
 * Decode the payload of the event message, i.e. the Attribute Event
 */
static bool
decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_attr_event attr_event = sns_std_attr_event_init_default;

  attr_event.attributes.funcs.decode = &decode_attr;
  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Received Attribute Event length %zu", stream->bytes_left);

  if(!pb_decode(stream, sns_std_attr_event_fields, &attr_event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

/**
 * Decode the physical sensor config event.
 */
static bool
decode_physical_config_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_physical_config_event event = sns_std_sensor_physical_config_event_init_default;

  if(!pb_decode(stream, sns_std_sensor_physical_config_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "decode_physical_config_event: Error decoding event: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_INFO, "SNS_CLIENT_EXAMPLE",
      "Received config event.  Sample rate: %f", event.sample_rate);

  return true;
}

/**
 * Decode the sensor event (aka a sensor sample).
 */
static bool
decode_sensor_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_sensor_event event = sns_std_sensor_event_init_default;
  float sensor_data[3];
  uint8_t arr_index = 0;
  pb_float_arr_arg float_arg = (pb_float_arr_arg)
    { .arr = sensor_data, .arr_len = 3, .arr_index = &arr_index };

  event.data.funcs.decode = &decode_float_arr_cb;
  event.data.arg = &float_arg;

  if(!pb_decode(stream, sns_std_sensor_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "decode_sensor_event: Error decoding Attr Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_VERBOSE, "SNS_CLIENT_EXAMPLE",
      "Received sensor event.  (%f, %f, %f)",
      sensor_data[0], sensor_data[1], sensor_data[2]);

  return true;
}

/**
 * Decode an event message received from the accel Sensor.
 */
static bool
decode_accel_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  uint32_t *msg_id = (uint32_t*)*arg;

  __android_log_print(ANDROID_LOG_VERBOSE, "SNS_CLIENT_EXAMPLE",
      "Begin decoding Accel event");

  if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG == *msg_id) {
    pb_byte_t temp[stream->bytes_left];
    pb_read(stream, temp, stream->bytes_left);
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT == *msg_id) {
    return decode_physical_config_event(stream, field, arg);
  } else if(SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT == *msg_id) {
    return decode_sensor_event(stream, field, arg);
  } else if(SNS_STD_MSGID_SNS_STD_ATTR_EVENT == *msg_id) {
    return decode_attr_event(stream, field, arg);
  } else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == *msg_id) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Received Error event");
    return false;
  } else {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
       "Received unknown event %i", *msg_id);
    return false;
  }

  __android_log_print(ANDROID_LOG_VERBOSE, "SNS_CLIENT_EXAMPLE",
      "Done decoding Accel event");

  return true;
}

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 *
 * At this time we will send an attributes request to the SUID.
 */
static bool
decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_std_suid uid;

  if(!pb_decode(stream, sns_std_suid_fields, &uid)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Received SUID Event with SUID %"PRIx64" %"PRIx64,
      uid.suid_low, uid.suid_high);

  send_attr_req(qmi_sensors_handle, &uid);

  return true;
}

/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
static bool
decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_suid_event suid_event;
  sns_buffer_arg data;

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding SUID Event: %s", PB_GET_ERROR(stream));
    return false;
  }

  {
    char data_type[data.buf_len + 1];
    strncpy(data_type, (char*)data.buf, data.buf_len);
    data_type[data.buf_len] = '\0';

    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
        "Received SUID Event with data type '%s'", data_type);
  }

  return true;
}

/**
 * Get the message ID of the encoded event residing on stream.
 *
 * @return Message ID
 */
static uint32_t
get_msg_id(pb_istream_t *stream)
{
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "event: %s", PB_GET_ERROR(stream));
  } else {
    __android_log_print(ANDROID_LOG_INFO, "SNS_CLIENT_EXAMPLE",
      "Decoding event with message ID %i, timestamp %"PRIu64,
        event.msg_id, event.timestamp);
    return event.msg_id;
  }

  return 0;
}

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * @param[i] arg Sensor-specific decode function
 */
static bool
decode_events(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  pb_istream_t stream_cpy = *stream;

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Begin decoding event");
  uint32_t msg_id = get_msg_id(&stream_cpy);

  event.payload.funcs.decode = *arg;
  event.payload.arg = &msg_id;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Event decoding complete");
  return rv;
}

/**
 * Handle a received event message from the Sensor Service.
 *
 * This example code only ever sends requests to the SUID Lookup Sensor
 * and an Accel Sensor.  Therefore we only need to check against those two
 * SUIDs below.  If this client were to use more Sensors, either:
 *  - The client would open separate QMI connections for each Sensor
 *    (and then the SUID would be known implicitly because of the indication
 *    callback function the event was received from).
 *  - Save the SUIDs in some sort of global state, so that they can be used
 *    for comparison here.
 *
 * @param[i] suid Source Sensor of this event
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 */
static void
handle_event_msg(sns_std_suid const *suid, void const *event_msg, size_t event_msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;
  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Processing events from SUID %"PRIx64" %"PRIx64,
      suid->suid_low, suid->suid_high);

  if(suid->suid_high == suid_lookup.suid_high && suid->suid_low == suid_lookup.suid_low) {
    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_suid_event;
  } else {
    accel_suid = *suid;

    event.events.funcs.decode = &decode_events;
    event.events.arg = &decode_accel_event;
  }

  stream = pb_istream_from_buffer(event_msg, event_msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event)) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Error decoding event list: %s", PB_GET_ERROR(&stream));
  }
}
/* QMI indication callback.  See qmi_client_ind_cb. */
static void
ind_msg_cb(qmi_client_type user_handle, unsigned int msg_id,
    void *ind_buf, unsigned int ind_buf_len, void *ind_cb_data)
{
  UNUSED_VAR(user_handle);
  UNUSED_VAR(ind_cb_data);
  size_t ind_len = sizeof(sns_client_report_ind_msg_v01);
  sns_client_report_ind_msg_v01 *ind = malloc(ind_len);
  int32_t qmi_err;

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Received Indication; len %i", ind_buf_len);

  // Extract the Protocol Buffer encoded message from the outer QMI/IDL  message
  qmi_err = qmi_idl_message_decode(SNS_CLIENT_SVC_get_service_object_v01(),
      QMI_IDL_INDICATION, msg_id, ind_buf, ind_buf_len, ind, ind_len);
  if(QMI_IDL_LIB_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "QMI decode error %i", qmi_err);
  } else {
    sns_client_event_msg event = sns_client_event_msg_init_default;
    __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Indication from client ID %"PRIu64, ind->client_id);

    // Decode just the sns_client_event_msg in order to get the SUID
    pb_istream_t stream = pb_istream_from_buffer(ind->payload, ind->payload_len);
    if(pb_decode(&stream, sns_client_event_msg_fields, &event)) {
      handle_event_msg(&event.suid, ind->payload, ind->payload_len);
    } else {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "Error decoding Event Message: %s", PB_GET_ERROR(&stream));
    }
  }

  free(ind);
  __android_log_print(ANDROID_LOG_INFO, "SNS_CLIENT_EXAMPLE",
      "Indication processing completed");
}

/**
 * Create a QMI connection to Sensors
 *
 * @param[i] qmi_handle QMI Handle created in wait_for_service
 * @param[i] timeout Time-out in milliseconds. 0 = no timeout
 *
 * @return True on success; false otherwise
 */
static bool
create_client_conn(qmi_client_type *qmi_handle, uint32_t timeout)
{
  bool rv = false;
  qmi_idl_service_object_type service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_service_instance instance_id = 0;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Creating client connection");

  qmi_err = qmi_client_init_instance(service_obj,
      instance_id, ind_msg_cb, NULL, &os_params, timeout, qmi_handle);

  if(QMI_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "qmi_client_init_instance error %i", qmi_err);
  } else {
    qmi_err = qmi_client_register_error_cb(*qmi_handle, error_cb, NULL);

    if(QMI_NO_ERR != qmi_err) {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
        "qmi_client_register_error_cb error %i", qmi_err);
    } else {
      rv = true;
    }
  }

  if(!rv) {
    if(NULL != *qmi_handle) {
      qmi_client_release(*qmi_handle);
      *qmi_handle = NULL;
    }
  }

  return rv;
}

/**
 * Check whether the Sensor service is available.
 *
 * @param[i] timeout Timeout period in ms; 0 infinite
 *
 * @return true if the service is available, false upon timeout
 */
static bool
wait_for_service(uint32_t timeout)
{
  qmi_idl_service_object_type service_obj = SNS_CLIENT_SVC_get_service_object_v01();
  qmi_client_type notifier_handle;
  qmi_client_error_type qmi_err;
  qmi_cci_os_signal_type os_params;
  bool rv = true;

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Waiting for service");

  qmi_err = qmi_client_notifier_init(service_obj, &os_params, &notifier_handle);
  if(QMI_NO_ERR != qmi_err) {
    __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "qmi_client_notifier_init error %i", qmi_err);
    rv = false;
  } else {
    QMI_CCI_OS_SIGNAL_WAIT(&os_params, timeout);
    if(os_params.timed_out) {
      __android_log_print(ANDROID_LOG_ERROR, "SNS_CLIENT_EXAMPLE",
      "Service is not available after %i timeout", timeout);
      rv = false;
    }
  }

  qmi_client_release(notifier_handle);
  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Waiting complete");
  return rv;
}

int
main(int argc, char *argv[])
{
  UNUSED_VAR(argc);
  UNUSED_VAR(argv);

  sleep(2);
  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Begin CM Test");

  if(!wait_for_service(200))
    return 0;

  if(!create_client_conn(&qmi_sensors_handle, 200))
    return 0;

  if(!send_suid_req(qmi_sensors_handle, "accel"))
    return 0;

  sleep(10);  // Stream accel for 10 seconds
  qmi_client_release(qmi_sensors_handle);

  __android_log_print(ANDROID_LOG_WARN, "SNS_CLIENT_EXAMPLE",
      "Sensors Client Example completed - Exiting");

  return 0;
}
