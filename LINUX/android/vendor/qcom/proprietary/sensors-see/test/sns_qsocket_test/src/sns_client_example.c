/*=============================================================================
  @file sns_client_example.c

  Example client application.  This application will open a QSocket or QMI
  connection to the Sensors Service (which resides on the SSC).  It will
  send a SUID Lookup request for the data type "accel".  For each found SUID,
  this application will send an Attributes Request, and subsequently an enable
  request with the sampling period set at 10Hz.  The connection will be
  terminated after 10 seconds of streaming.

  PEND: Accept sample rate/batch period/sensor name/stream length as
  command-line input

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
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
#include <inttypes.h>
#include "sns_osa.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_client.pb.h"
#include "sns_suid.pb.h"
#include "sns_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

// Number of concurrent clients to test/stream
#define SNS_CLIENT_TEST_CNT 1

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

/**
 * Event handlers for each <suid, msg_id> pair.
 */
typedef bool (*sns_event_func)(struct sns_client *client, pb_istream_t *stream);
typedef struct sns_event_handler
{
  sns_std_suid *suid;
  uint32_t msg_id;
  sns_event_func func;
} sns_event_handler;

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* SUID for the accel Sensor, as received from the SUID Lookup Sensor */
static sns_std_suid accel_suid = (sns_std_suid){ .suid_low = 0, .suid_high = 0 };

/* Single connected client to the QMI/QSocket service */
static struct sns_client *sensor_client[2] = {NULL, NULL};

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
static bool
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

/* See sns_client_resp */
static void
resp_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);
  SNS_LOG(WARN, "Received response rc %i", error);
}

/**
 * Send a request to the specified service.  Do not specify any batching
 * options.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 *
 * @return true upon success; false otherwise
 */
static bool
send_basic_req(struct sns_client *client, sns_std_suid *suid,
    uint32_t msg_id, void *encode_func, void *arg)
{
  sns_client_request_msg req_msg = sns_client_request_msg_init_default;
  int err;

  req_msg.suid = *suid;
  req_msg.msg_id = msg_id;
  req_msg.susp_config.client_proc_type = SNS_STD_CLIENT_PROCESSOR_SSC;
  req_msg.request.has_batching = false;
  req_msg.request.batching.batch_period = 1000000;  // 1 second
  req_msg.request.payload.funcs.encode = encode_func;
  req_msg.request.payload.arg = arg;

  SNS_LOG(WARN, "send_basic_req");

  err = sns_client_send(client, &req_msg, &resp_cb, NULL);
  if(0 != err)
  {
    SNS_LOG(ERROR, "sns_client_send error %i", err);
    return false;
  }
  return true;
}

/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] data_type Sensor type to search for
 * @param[o] encoded_req Generated encoded request
 *
 * @return Length of encoded_req
 */
static bool
encode_suid_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_suid_req suid_req = sns_suid_req_init_default;
  char const *data_type = (char const*)*arg;
  bool rv = true;

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &((sns_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;

  if(!pb_encode_tag_for_field(stream, field))
  {
    SNS_LOG(ERROR, "Error encoding SUID tag: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else if(!pb_encode_submessage(stream, sns_suid_req_fields, &suid_req))
  {
    SNS_LOG(ERROR, "Error encoding SUID request: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  return rv;
}

/**
 * Create an Accel enable request.
 */
static bool
encode_enable_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  UNUSED_VAR(arg);
  bool rv = true;
  sns_std_sensor_config config_req = sns_std_sensor_config_init_default;

  config_req.sample_rate = 10;  // Stream at 10Hz

  if(!pb_encode_tag_for_field(stream, field))
  {
    SNS_LOG(ERROR, "Error encoding enable tag: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else if(!pb_encode_submessage(stream, sns_std_sensor_config_fields, &config_req))
  {
    SNS_LOG(ERROR, "Error encoding enable request: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  return rv;
}

/**
 * QSocket/QMI Error callback.  This would be our notification if the connection
 * was lost.  A typical implementation would re-establish the connection, and
 * resend any active requests.
 *
 * See sns_client_error */
static void
error_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);

  SNS_LOG(ERROR, "Error callback %i", error);
}

/**
 * Decode the physical sensor config event.
 */
static bool
decode_physical_config_event(struct sns_client *client, pb_istream_t *stream)
{
  UNUSED_VAR(client);
  sns_std_sensor_physical_config_event event =
    sns_std_sensor_physical_config_event_init_default;

  if(!pb_decode(stream, sns_std_sensor_physical_config_event_fields, &event))
  {
    SNS_LOG(ERROR, "decode_physical_config_event: Error decoding event: %s",
        PB_GET_ERROR(stream));
    return false;
  }

  SNS_LOG(INFO, "Received config event.  Sample rate: %f", event.sample_rate);

  return true;
}

/**
 * Decode the sensor event (aka a sensor sample).
 */
static bool
decode_sensor_event(struct sns_client *client, pb_istream_t *stream)
{
  UNUSED_VAR(client);
  sns_std_sensor_event event = sns_std_sensor_event_init_default;
  float sensor_data[3];
  uint8_t arr_index = 0;
  pb_float_arr_arg float_arg = (pb_float_arr_arg)
    { .arr = sensor_data, .arr_len = 3, .arr_index = &arr_index };

  event.data.funcs.decode = &decode_float_arr_cb;
  event.data.arg = &float_arg;

  if(!pb_decode(stream, sns_std_sensor_event_fields, &event))
  {
    SNS_LOG(ERROR, "decode_sensor_event: Error decoding Attr Event: %s",
        PB_GET_ERROR(stream));
    return false;
  }

  SNS_LOG(VERBOSE, "Received sensor event.  (%f, %f, %f)",
      sensor_data[0], sensor_data[1], sensor_data[2]);

  return true;
}

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 */
static bool
decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_std_suid uid;
  struct sns_client *client = *arg;

  if(!pb_decode(stream, sns_std_suid_fields, &uid))
  {
    SNS_LOG(ERROR, "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  SNS_LOG(WARN, "Received SUID Event with SUID %"PRIx64" %"PRIx64,
      uid.suid_low, uid.suid_high);

  accel_suid = uid;

  send_basic_req(client, &accel_suid,
      SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG,
      &encode_enable_req, NULL);

  return true;
}

/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
static bool
decode_suid_event(struct sns_client *client, pb_istream_t *stream)
{
  sns_suid_event suid_event = sns_suid_event_init_default;
  sns_buffer_arg data;

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.suid.arg = client;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event))
  {
    SNS_LOG(ERROR, "Error decoding SUID event: %s", PB_GET_ERROR(stream));
    return false;
  }

  {
    char data_type[data.buf_len + 1];
    strncpy(data_type, (char*)data.buf, data.buf_len);
    data_type[data.buf_len] = '\0';

    SNS_LOG(WARN, "Received SUID event with data type '%s'", data_type);
  }

  return true;
}

/**
 * Decode the payload of the event message, i.e. the SUID Event
 *
 * @param[i] arg Message ID
 */
static bool
decode_error_event(struct sns_client *client, pb_istream_t *stream)
{
  UNUSED_VAR(client);
  sns_std_error_event error_event = sns_std_error_event_init_default;

  if(!pb_decode(stream, sns_std_error_event_fields, &error_event))
  {
    SNS_LOG(ERROR, "Error decoding error event: %s", PB_GET_ERROR(stream));
    return false;
  }
  else
  {
    SNS_LOG(WARN, "Received error event: %i", error_event.error);
  }

  return true;
}

static sns_event_handler event_handlers[] =
{
  { NULL, SNS_STD_MSGID_SNS_STD_ERROR_EVENT, &decode_error_event },
  { &suid_lookup, SNS_SUID_MSGID_SNS_SUID_EVENT, &decode_suid_event },
  { &accel_suid, SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_EVENT, &decode_sensor_event },
  { &accel_suid, SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_PHYSICAL_CONFIG_EVENT, &decode_physical_config_event },
};

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * @param[i] arg Sensor-specific decode function
 */
static bool
decode_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  sns_buffer_arg data;
  void **args = (void**)*arg;
  sns_std_suid *suid = args[0];
  struct sns_client *client = args[1];

  SNS_LOG(WARN, "Begin decoding event");

  event.payload.funcs.decode = &decode_payload;
  event.payload.arg = &data;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else
  {
    pb_istream_t sub_stream = pb_istream_from_buffer(data.buf, data.buf_len);

    for(size_t i = 0; i < ARR_SIZE(event_handlers); i++)
    {
      if((NULL == event_handlers[i].suid ||
          (suid->suid_high == event_handlers[i].suid->suid_high &&
           suid->suid_low == event_handlers[i].suid->suid_low)) &&
        event.msg_id == event_handlers[i].msg_id)
      {
        event_handlers[i].func(client, &sub_stream);
        break;
      }
      if(ARR_SIZE(event_handlers) - 1 == i)
      {
        SNS_LOG(ERROR, "No handler for <%lx %lx, %i>",
            suid->suid_high, suid->suid_low, event.msg_id);
      }
    }
  }

  SNS_LOG(ERROR, "Event decoding complete");
  return rv;
}

/**
 * Get the SUID of a client request indication message.
 *
 * In theory this is inefficient, and could be optimized by using manual
 * decoding.
 *
 * @param[i] msg Encoded message of type sns_client_event_msg
 * @param[i] msg_len Length of the msg buffer
 */
static sns_std_suid
get_suid(void *msg, uint32_t msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;

  stream = pb_istream_from_buffer(msg, msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
  {
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));
  }

  return event.suid;
}

/**
 * Handle a received indication message from the Sensor Service.
 *
 * @param[i] client Client connection on which this indication was received.
 * @param[i] event_msg PB-encoded message of type sns_client_event_msg
 * @param[i] event_msg_len Length (in bytes) of the encoded event_msg
 */
static void
ind_cb(struct sns_client *client, void *msg, uint32_t msg_len,
    void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;
  sns_std_suid suid = get_suid(msg, msg_len);
  void *arg[2] = {&suid, client};

  SNS_LOG(WARN, "Processing events from SUID %"PRIx64" %"PRIx64,
      suid.suid_low, suid.suid_high);

  event.events.funcs.decode = &decode_event;
  event.events.arg = arg;

  stream = pb_istream_from_buffer(msg, msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));
}

int
main(int argc, char *argv[])
{
  UNUSED_VAR(argc);
  UNUSED_VAR(argv);
  int err;

  for(int i = 0; i < SNS_CLIENT_TEST_CNT; i++)
  {
    sleep(2);
    SNS_LOG(WARN, "Begin CM Test %i", i);

    err = sns_client_init(&sensor_client[i], 2000, &ind_cb, NULL, &error_cb, NULL);
    if(0 != err)
    {
      SNS_LOG(ERROR, "sns_client_init error %i", err);
      return 0;
    }

    if(!send_basic_req(sensor_client[i], &suid_lookup, SNS_SUID_MSGID_SNS_SUID_REQ,
          &encode_suid_req, "accel"))
      return 0;
  }

  sleep(5);  // Stream accel for 5 seconds

  for(int i = 0; i < SNS_CLIENT_TEST_CNT; i++)
  {
    sns_client_deinit(sensor_client[i]);
    SNS_LOG(WARN, "Sensors Client completed");
    sleep(2);
  }

  return 0;
}
