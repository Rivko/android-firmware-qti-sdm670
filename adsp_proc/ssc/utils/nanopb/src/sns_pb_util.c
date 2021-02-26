/*=============================================================================
  @file sns_pb_util.c

  Utility functions provided by Qualcomm for use by Sensors in their use of
  nanoPB.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdbool.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_event_service.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_request.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_suid.pb.h"
#include "sns_types.h"

typedef struct pb_suid_event_decode_cb_arg
{
  /* Array of SUIDs */
  sns_sensor_uid *suids;
  /* Length of the suids array */
  int suids_len;
  /* Current write index into the array */
  int count;
} pb_suid_event_decode_cb_arg;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static bool
pb_encode_request_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  pb_simple_cb_arg *cb_arg = (pb_simple_cb_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_submessage(stream, cb_arg->fields, cb_arg->decoded_struct))
    return false;

  return true;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

bool
pb_encode_float_arr_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  pb_float_arr_arg *arr = (pb_float_arr_arg*)*arg;
  uint8_t i;

  if (PB_LTYPE(field->type) == PB_LTYPE_FIXED32)
  {
    float *f_data = (float *)arr->arr;
    for(i = 0; i < arr->arr_len; i++)
    {
      float data = f_data[i];

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_fixed32(stream, &data))
        return false;
    }

  }
  if (PB_LTYPE(field->type) == PB_LTYPE_UVARINT)
  {
    uint32_t *u_data = (uint32_t *)arr->arr;

    for(i = 0; i < arr->arr_len; i++)
    {
      uint32_t data = u_data[i];

      if(!pb_encode_tag_for_field(stream, field))
        return false;

      if(!pb_encode_fixed32(stream, &data))
        return false;
    }
  }
  return true;
}

bool
pb_decode_simple_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_simple_cb_arg *cb_arg = (pb_simple_cb_arg*)*arg;

  if(!pb_decode(stream, cb_arg->fields, cb_arg->decoded_struct))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "pb_decode_simple_cb failed: %s",
        PB_GET_ERROR(stream));
    return false;
  }

  return true;
}

bool
pb_decode_float_arr_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg)
{
  pb_float_arr_arg const *arr = (pb_float_arr_arg const*)*arg;
  if(*arr->arr_index >= arr->arr_len)
  {
    SNS_PRINTF(MED, sns_fw_printf, "Redundant element not decoded");
    pb_skip_field(stream, PB_WT_32BIT);
  }

  if (PB_LTYPE(field->type) == PB_LTYPE_FIXED32)
  {
    float *f_arr = arr->arr;
    if(!pb_decode_fixed32(stream, &(f_arr[*arr->arr_index])))
      return false;

    (*arr->arr_index)++;
  }
  if (PB_LTYPE(field->type) == PB_LTYPE_UVARINT)
  {
    uint32_t *u_arr = (uint32_t *)arr->arr;

    if(!pb_decode_fixed32(stream, &(u_arr[*arr->arr_index])))
      return false;

    (*arr->arr_index)++;
  }

  return true;
}

bool
pb_encode_string_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  pb_buffer_arg *data = (pb_buffer_arg*)*arg;
  if(pb_encode_tag_for_field(stream, field) &&
     pb_encode_string(stream, data->buf, data->buf_len))
    return true;

  SNS_SPRINTF(ERROR, sns_fw_printf, "Encode failed: %s", PB_GET_ERROR(stream));
  return false;
}

bool
pb_decode_string_cb(pb_istream_t *stream, const pb_field_t *field,
    void **arg)
{
  UNUSED_VAR(field);
  pb_buffer_arg *data = (pb_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

size_t
pb_encode_request(void *buffer, size_t buffer_len, void const *request_payload,
    pb_field_t const *payload_fields, sns_std_request *request)
{
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_len);
  sns_std_request temp = sns_std_request_init_default;
  pb_simple_cb_arg arg = (pb_simple_cb_arg)
    { .decoded_struct = (void*)request_payload, .fields = payload_fields };

  if(NULL == request)
    request = &temp;

  if(NULL != payload_fields)
  {
    request->payload.funcs.encode = &pb_encode_request_cb;
    request->payload.arg = &arg;
  }

  if(!pb_encode(&stream, sns_std_request_fields, request))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error Encoding request: %s", PB_GET_ERROR(&stream));
  }

  request->payload.arg = NULL;
  return stream.bytes_written;
}

bool
pb_decode_request(sns_request const *request, pb_field_t const *payload_fields,
    sns_std_request *std_request, void *decoded_req)
{
  pb_istream_t istream =
    pb_istream_from_buffer(request->request, request->request_len);
  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_req, .fields = payload_fields };
  sns_std_request temp =
      { .payload.funcs.decode = &pb_decode_simple_cb, .payload.arg = &arg };

  if(NULL == std_request)
    std_request = &temp;

  return pb_decode(&istream, sns_std_request_fields, std_request);
}

bool
pb_send_event(sns_sensor_instance *instance, pb_field_t const *fields,
    void const *c_struct, sns_time timestamp, uint32_t message_id,
    struct sns_sensor_uid const *sensor_uid)
{
  bool rv = false;
  size_t encoded_len;
  sns_service_manager *manager =
    instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

  if(NULL != fields && NULL != c_struct && pb_get_encoded_size(&encoded_len, fields, c_struct))
  {
    sns_sensor_event *event =
      event_service->api->alloc_event(event_service, instance, encoded_len);

    event->event_len = encoded_len;
    event->message_id = message_id;
    event->timestamp = timestamp;

    pb_ostream_t stream =
      pb_ostream_from_buffer((pb_byte_t*)event->event, event->event_len);

    if(pb_encode(&stream, fields, c_struct))
    {
      event_service->api->publish_event(event_service,
            instance, event, sensor_uid);
      rv = true;
    }
    else
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding SUID event: %s",
          PB_GET_ERROR(&stream));
    }
  }
  else
  {
    sns_sensor_event *event =
      event_service->api->alloc_event(event_service, instance, 0);

    event->event_len = 0;
    event->message_id = message_id;
    event->timestamp = timestamp;

    event_service->api->publish_event(event_service,
      instance, event, sensor_uid);
    rv = true;
  }

  return rv;
}

sns_rc
pb_send_sensor_stream_event(struct sns_sensor_instance *instance,
    struct sns_sensor_uid const *sensor_uid, sns_time timestamp,
    uint32_t message_id, sns_std_sensor_sample_status sample_status, float const *data,
    uint8_t data_len, size_t encoded_len)
{
  sns_rc rv = SNS_RC_FAILED;
  sns_service_manager *manager =
      instance->cb->get_service_manager(instance);
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
  sns_std_sensor_event stream_event = sns_std_sensor_event_init_default;
  uint8_t arr_index = 0;
  pb_float_arr_arg arg =
    {.arr = (float*)data, .arr_len = data_len, .arr_index = &arr_index};

  stream_event.status = sample_status;

  stream_event.data.funcs.encode = &pb_encode_float_arr_cb;
  stream_event.data.arg = &arg;

  if(encoded_len > 0)
  {
    sns_sensor_event *event =
        event_service->api->alloc_event(event_service, instance, encoded_len);

    pb_ostream_t stream =
      pb_ostream_from_buffer((pb_byte_t*)event->event, encoded_len);

    if(!pb_encode(&stream, sns_std_sensor_event_fields, &stream_event))
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error encoding sensor stream event");
    }

    event->event_len = encoded_len;
    event->message_id = message_id;
    event->timestamp = timestamp;

    rv = event_service->api->publish_event(event_service,
          instance, event, sensor_uid);
    }

  return rv;
}

size_t
pb_get_encoded_size_sensor_stream_event(float const *data, uint8_t data_len)
{
  size_t encoded_len;
  sns_std_sensor_event stream_event = sns_std_sensor_event_init_default;
  uint8_t arr_index = 0;
  pb_float_arr_arg arg =
    {.arr = (float*)data, .arr_len = data_len, .arr_index = &arr_index};

  stream_event.data.funcs.encode = &pb_encode_float_arr_cb;
  stream_event.data.arg = &arg;

  pb_get_encoded_size(&encoded_len, sns_std_sensor_event_fields, &stream_event);

  return encoded_len;
}

bool
pb_decode_suid_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_suid_search *suid_search = (sns_suid_search*)*arg;
  sns_std_suid uid;

  if(!pb_decode(stream, sns_std_suid_fields, &uid)) {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(0 != sns_memcmp(&uid,
                     &((sns_std_suid){0,0}),
                     sizeof(sns_std_suid)))
  {
    sns_memscpy(&suid_search->suid[suid_search->num_of_suids],
                sizeof(suid_search->suid[suid_search->num_of_suids]),
                &uid, sizeof(uid));
    suid_search->num_of_suids++;
  }

  return true;
}
