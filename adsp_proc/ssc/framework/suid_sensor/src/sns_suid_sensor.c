/*=============================================================================
  @file sns_suid_sensor.c

  The Framework Sensor manages all SUID and attribute requests.  It is
  technically part of the Framework, but it modelled separately in order
  to avoid special cases within the event-passing code.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <string.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_registry.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_suid_sensor.h"
#include "sns_types.h"

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#define MAX_DATATYPE_LEN 32

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Callback structure used when encoding the list of SUIDs in response
 * to an SUID lookup request.
 */
typedef struct sns_encode_suid_arg
{
  struct pb_ostream_s *stream;
  struct pb_field_s const *field;
  /* If an empty string, encode all sensors */
  char const *data_type;
  uint32_t *data_type_len;
  /* Client request only default SUID to be encoded */
  bool default_only;
  /* If the default sensor has been found and encoded */
  bool default_encoded;
  /* No default sensor found; instead use "first" matching SUID */
  bool use_first;
} sns_encode_suid_arg;

/**
 * Information regarding a default sensor of a given datatype; this is
 * populated from registry config.
 */
typedef struct sns_def_sensor_info
{
  /* entry for the def_sensor_list */
  sns_isafe_list_item list_entry;
  /* sensor datatype */
  char datatype[MAX_DATATYPE_LEN];
  /* attribute id to match */
  int attr_id;
  /* attribute value to match */
  int64_t attr_val;
  bool match_found;
} sns_def_sensor_info;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/
static sns_fw_sensor *suid_sensor = NULL;

static const sns_sensor_uid registry_suid = {
  .sensor_uid = {
    0x75,0x22,0x1e,0x70,0xb4,0x41,0x25,0x5e,
    0x59,0x27,0x7f,0x00,0xa7,0x54,0x27,0xe1
  }
};

static char const *reg_group_name = "default_sensors";

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* Decodes a single attribute value as an integer */
static bool
decode_attr_value_int(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  int rc = false;
  int64_t *attr_val = (int64_t*)*arg;

  if(NULL != attr_val)
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;

    if(!pb_decode(stream, sns_std_attr_value_data_fields, &value))
      SNS_PRINTF(ERROR, sns_fw_printf, "sns_suid_decode_attr_value, decode failed");

    if(value.has_sint)
    {
      *attr_val = value.sint;
      rc = true;
    }
  }
  return rc;
}

/* Callback function used with sns_attr_info_process_attribute */
static bool
default_sensor_process_attribute_cb(void *buf, uint32_t buf_len, void *arg)
{
  SNS_ASSERT(arg != NULL);
  bool rc = true;
  int64_t *attr_val = (int64_t*)arg;

  pb_istream_t stream = pb_istream_from_buffer(buf, buf_len);

  sns_std_attr attribute = sns_std_attr_init_default;
  attribute.value.values.funcs.decode = decode_attr_value_int;
  attribute.value.values.arg = (void*)attr_val;

  rc = pb_decode(&stream, sns_std_attr_fields, &attribute);
  if(!rc)
    SNS_PRINTF(ERROR, sns_fw_printf, "default_sensor_match_attr_cb, pb_decode() failed");

  return rc;
}

static bool
default_sensor_match_attr_cb(struct sns_attribute_info *attr_info, void *arg)
{
  SNS_ASSERT(arg != NULL);
  sns_def_sensor_info *item = (sns_def_sensor_info*)arg;
  bool rc = true;

  if(!item->match_found && sns_attr_info_get_available(attr_info))
  {
    char datatype[MAX_DATATYPE_LEN];
    sns_attr_info_get_data_type(attr_info, datatype, sizeof(datatype));

    if(0 == strcmp(datatype, item->datatype))
    {
      int64_t attr_val;

      rc = sns_attr_info_process_attribute(attr_info, item->attr_id,
          &default_sensor_process_attribute_cb, &attr_val);
      if(!rc)
      {
        SNS_PRINTF(ERROR, sns_fw_printf,
            "sns_attr_info_process_attribute() failed");
      }
      else
      {
        if(item->attr_val == attr_val)
        {
          SNS_SPRINTF(MED, sns_fw_printf,
              "default_sensor_match_attr_cb: match found %s", item->datatype);
          item->match_found = true;
          sns_attr_info_set_default(attr_info, true);
        }
      }
    }
  }
  return true;
}

/* Updates the def_sensor_list with default attribute info */
static void
default_sensor_match_attributes(char const *data_type)
{
  sns_suid_sensor_state *state =
    (sns_suid_sensor_state*)suid_sensor->sensor.state->state;
  sns_isafe_list_iter iter;
  bool rc;

  for(sns_isafe_list_iter_init(&iter, &state->def_sensor_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_def_sensor_info* item = (sns_def_sensor_info*)
        sns_isafe_list_iter_get_curr_data(&iter);

    if(!item->match_found && 0 == strcmp(item->datatype, data_type))
    {
      rc = sns_attr_svc_sensor_foreach(&default_sensor_match_attr_cb, (void*)item);
      if(!rc)
        SNS_PRINTF(ERROR, sns_fw_printf, "error while matching attributes");
    }
  }
}

/* callback function to decode one item from the default_sensor config */
static bool
default_sensor_item_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_def_sensor_info *sensor_info = (sns_def_sensor_info*)*arg;
  sns_registry_data_item reg_item = sns_registry_data_item_init_default;
  pb_buffer_arg item_name;
  bool rc, rv = true;

  reg_item.name.funcs.decode = &pb_decode_string_cb;
  reg_item.name.arg = &item_name;

  rc = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(!rc || 0 == item_name.buf_len)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "pb_decode() failed");
    rv = false;
  }
  else if(0 == strncmp((char*)item_name.buf, "id", item_name.buf_len))
  {
    if(!reg_item.has_sint)
      rv = false;
    else
      sensor_info->attr_id = reg_item.sint;
  }
  else if(0 == strncmp((char*)item_name.buf, "val", item_name.buf_len))
  {
    if (!reg_item.has_sint)
      rv = false;
    else
      sensor_info->attr_val = reg_item.sint;
  }

  return rv;
}

static bool
default_sensor_attr_group_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_def_sensor_info *sensor_info = (sns_def_sensor_info*)*arg;
  sns_registry_data_item reg_item = sns_registry_data_item_init_default;
  pb_buffer_arg group_name;
  bool rc, rv = true;

  reg_item.name.funcs.decode = &pb_decode_string_cb;
  reg_item.name.arg = &group_name;
  reg_item.subgroup.items.funcs.decode = default_sensor_item_decode_cb;
  reg_item.subgroup.items.arg = sensor_info;

  rc = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(!rc || 0 == group_name.buf_len)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "pb_decode() failed, buf_len=%i", group_name.buf_len);
    rv = false;
  }
  else if(!reg_item.has_subgroup)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "subgroup not found, decode failed");
    rv = false;
  }
  return rv;
}

/**
 * Decode a registry item; expected contents are a subgroup containing the
 * attributes which the default sensor must hold.
 */
static bool
default_sensor_group_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  UNUSED_VAR(arg);
  sns_suid_sensor_state *state =
    (sns_suid_sensor_state*)suid_sensor->sensor.state->state;
  sns_registry_data_item reg_item = sns_registry_data_item_init_default;
  pb_buffer_arg group_name;
  sns_def_sensor_info *sensor_info;
  bool rc, rv = true;

  sensor_info = sns_malloc(SNS_HEAP_MAIN, sizeof(*sensor_info));
  SNS_ASSERT(NULL != sensor_info);
  sensor_info->match_found = false;

  reg_item.name.funcs.decode = &pb_decode_string_cb;
  reg_item.name.arg = &group_name;
  reg_item.subgroup.items.funcs.decode = default_sensor_attr_group_decode_cb;
  reg_item.subgroup.items.arg = sensor_info;

  rc = pb_decode(stream, sns_registry_data_item_fields, &reg_item);
  if(!rc || 0 == group_name.buf_len || !reg_item.has_subgroup)
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "pb_decode() failed, buf_len=%i has_subgroup=%i",
        group_name.buf_len, reg_item.has_subgroup);
    sns_free(sensor_info);
    rv = false;
  }
  else
  {
    bool info_exists = false;
    sns_isafe_list_iter iter;

    sns_strlcpy(sensor_info->datatype, group_name.buf, MAX_DATATYPE_LEN - 1);
    sensor_info->datatype[MAX_DATATYPE_LEN-1] = '\0';

    SNS_SPRINTF(MED, sns_fw_printf,
        "default_sensor_group_decode_cb, dt=%s, attr_id=%i, attr_val=%i",
        sensor_info->datatype, sensor_info->attr_id, sensor_info->attr_val);

    /* check if the datatype exists in the def_sensor_list */
    for(sns_isafe_list_iter_init(&iter, &state->def_sensor_list, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_def_sensor_info *temp = (sns_def_sensor_info*)
          sns_isafe_list_iter_get_curr_data(&iter);

      if(0 == strcmp(temp->datatype, sensor_info->datatype))
      {
        info_exists = true;
        break;
      }
    }

    if(!info_exists)
    {
      sns_isafe_list_iter_init(&iter, &state->def_sensor_list, true);
      sns_isafe_list_item_init(&sensor_info->list_entry, sensor_info);
      sns_isafe_list_iter_insert(&iter, &sensor_info->list_entry, true);
      /* match this default sensor config to a sensor attribute_info */
      default_sensor_match_attributes(sensor_info->datatype);
    }
    else
    {
      sns_free(sensor_info);
    }
  }
  return rv;
}

/**
 * Decode all pending events from the Registry Sensor.
 */
static void
suid_process_registry_events(sns_sensor *this)
{
  sns_suid_sensor_state *state = (sns_suid_sensor_state*)this->state->state;
  sns_data_stream *reg_stream = state->reg_stream;

  for(sns_sensor_event *event = reg_stream->api->peek_input(reg_stream);
      NULL != event;
      event = reg_stream->api->get_next_input(reg_stream))
  {
    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
      pb_istream_t stream =
        pb_istream_from_buffer((void*)event->event, event->event_len);
      sns_registry_read_event read_event = sns_registry_read_event_init_default;
      pb_buffer_arg group_name;

      read_event.name.arg = &group_name;
      read_event.name.funcs.decode = pb_decode_string_cb;

      read_event.data.items.funcs.decode = &default_sensor_group_decode_cb;
      read_event.data.items.arg = NULL;

      if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
        SNS_PRINTF(ERROR, this, "Error decoding registry event");
    }
  }
}

/* See sns_sensor::event_notify */
static sns_rc
notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  suid_process_registry_events(this);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::init */
static sns_rc
init(sns_sensor *const this)
{
  sns_suid_sensor_state *state = (sns_suid_sensor_state*)this->state->state;
  suid_sensor = (sns_fw_sensor*)this;
  {
    char const name[] = "suid";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "suid";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "qualcomm";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = "sns_suid.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = true;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_STD_SENSOR_STREAM_TYPE_SINGLE_OUTPUT;
    sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }

  sns_isafe_list_init(&state->def_sensor_list);
  state->reg_stream = NULL;

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static sns_sensor_uid suid;
  suid = sns_get_suid_lookup();
  return &suid;
}

/**
 * Decode callback for the SUID request message.
 */
static bool
pb_suid_event_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_suid_lookup_request **lookup_request = (sns_suid_lookup_request**)*arg;
  pb_buffer_arg data;
  sns_suid_req suid_req = sns_suid_req_init_default;

  suid_req.data_type.funcs.decode = &pb_decode_string_cb;
  suid_req.data_type.arg = &data;

  if(pb_decode(stream, sns_suid_req_fields, &suid_req))
  {
    sns_suid_lookup_request *lookup;

    lookup = sns_malloc(SNS_HEAP_MAIN, sizeof(*lookup) + data.buf_len + 1);
    SNS_ASSERT(NULL != lookup);

    sns_isafe_list_item_init(&lookup->list_entry, lookup);
    lookup->register_updates = suid_req.has_register_updates && suid_req.register_updates;
    lookup->default_only = suid_req.has_default_only ? suid_req.default_only : true;

    sns_memscpy(lookup->data_type, data.buf_len, data.buf, data.buf_len);
    lookup->data_type_len = data.buf_len + 1;
    // Ensure data_type is always null-terminated (may be redundant)
    lookup->data_type[data.buf_len] = '\0';
    *lookup_request = lookup;

    SNS_SPRINTF(LOW, &suid_sensor->sensor,
        "Received request for data_type \'%s\' (register %i), default_only=%i",
        lookup->data_type, lookup->register_updates, lookup->default_only);
    return true;
  }
  else
  {
    SNS_SPRINTF(ERROR, &suid_sensor->sensor, "Error decoding SUID Request: %s",
        PB_GET_ERROR(stream));
    return false;
  }
}

/**
 * Parse an incoming lookup request, and store the results in a dynamicallly
 * allocated object.
 *
 * @param[i] request Incoming request from client
 * @param[o] lookup_request Parsed version of the request message
 */
static bool
parse_suid_request(sns_request const *request,
    sns_suid_lookup_request **lookup_request)
{
  sns_std_request req = sns_std_request_init_default;
  pb_istream_t stream =
    pb_istream_from_buffer(request->request, request->request_len);

  req.payload.funcs.decode = &pb_suid_event_decode_cb;
  req.payload.arg = lookup_request;

  if(!pb_decode(&stream, sns_std_request_fields, &req))
  {
    SNS_SPRINTF(ERROR, &suid_sensor->sensor, "Error decoding request: %s",
        PB_GET_ERROR(&stream));

    if(NULL != *lookup_request)
    {
      sns_free(*lookup_request);
      *lookup_request = NULL;
    }
    return false;
  }

  return true;
}

/**
 * Encode a single SUID within a SUID Event message.
 *
 * @param[i] stream Active stream within an encode function
 * @param[i] field The SUID field within sns_suid_event
 * @param[i] suid SUID to encode
 *
 * @return false if encoding was unsuccessful
 */
static bool
encode_suid_in_event(struct pb_ostream_s *stream,
    struct pb_field_s const *field, sns_sensor_uid const *suid)
{
  sns_std_suid uid;
  sns_memscpy(&uid, sizeof(uid), suid, sizeof(uid));

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_submessage(stream, sns_std_suid_fields, &uid))
    return false;

  return true;
}

/**
 * Given a Sensor, determine whether it meets the search criteria, and if so,
 * encode its SUID on the stream.
 *
 * @param[i] sensor
 * @param[i] arg Of type sns_encode_suid_arg, set below in encode_suid_cb
 *
 * @return True if encoding was successful, false otherwise
 */
static bool
encode_sensor_suid(struct sns_attribute_info *attr_info, void *arg)
{
  bool rv = true;
  sns_encode_suid_arg *suid_arg = (sns_encode_suid_arg*)arg;

  if(!suid_arg->default_only || !suid_arg->default_encoded)
  {
    if(sns_attr_info_get_available(attr_info))
    {
      bool encode = false;
      sns_sensor_uid suid;

      if('\0' == suid_arg->data_type[0])
      {
        char platform_sensor_vendor[] = "qti_qualcomm";
        char vendor[sizeof(platform_sensor_vendor) + 1];
        sns_attr_info_get_vendor(attr_info, vendor, sizeof(vendor));
        if(0 != strcmp(platform_sensor_vendor, vendor))
          encode = true;
      }
      else
      {
        char data_type[MAX_DATATYPE_LEN];

        sns_attr_info_get_data_type(attr_info, data_type, sizeof(data_type));
        if(0 == strcmp(data_type, suid_arg->data_type))
          encode = true;
      }

      if(suid_arg->default_only)
      {
        if(!suid_arg->use_first && !sns_attr_info_get_default(attr_info))
          encode = false;
      }

      if(encode)
      {
        suid = sns_attr_info_get_suid(attr_info);
        rv = encode_suid_in_event(suid_arg->stream, suid_arg->field, &suid);

        if(rv && suid_arg->default_only)
          suid_arg->default_encoded = true;
      }
    }
  }

  return true;
}

/**
 * Encode all SUIDs which match the client-specified query.
 *
 * If the default sensor was requested, but not found, instead encode the
 * first matching SUID.
 *
 * @param[i] arg Data Type to search for (sns_encode_suid_arg)
 *
 * @return True if encoding was successful, false otherwise
 */
static bool
encode_suid_cb(struct pb_ostream_s *stream, struct pb_field_s const *field,
    void *const *arg)
{
  sns_encode_suid_arg *suid_arg = (sns_encode_suid_arg*)*arg;
  suid_arg->stream = stream;
  suid_arg->field = field;
  suid_arg->default_encoded = false;

  if(suid_arg->default_only)
  {
    suid_arg->use_first = false;
    sns_attr_svc_sensor_foreach(&encode_sensor_suid, suid_arg);

    if(!suid_arg->default_encoded)
    {
      suid_arg->use_first = true;
      sns_attr_svc_sensor_foreach(&encode_sensor_suid, suid_arg);
    }
  }
  else
  {
    /* Encode all suids for a datatype */
    suid_arg->use_first = false;
    sns_attr_svc_sensor_foreach(&encode_sensor_suid, suid_arg);
  }
  // PEND: Consider returning false if encode failed (SNS_RC_SUCCESS != rc)
  return true;
}

/**
 * Form and send an SUID event based on the given request.
 *
 * data_type string will be an exact copy of what was sent by the client.  The
 * client may or may not have null-terminated their request string (Protocol
 * Buffer's C++ library for example, does not null-terminate strings).
 *
 * @param[i] sensor SUID Lookup Sensor
 * @param[i] instance Instance on which this request was assigned
 * @param[i] request Perform the request search and return publish results
 */
static void
send_suid_event(sns_sensor_instance *instance,
    sns_suid_lookup_request const *request)
{
  sns_suid_event suid_event = sns_suid_event_init_default;
  sns_encode_suid_arg arg = (sns_encode_suid_arg)
  {
    .data_type = request->data_type,
    .default_only = request->default_only,
    .default_encoded = false,
    .use_first = false
  };

  SNS_SPRINTF(LOW, sns_fw_printf, "send_suid_event dt=%s, default=%i",
              request->data_type, request->default_only);

  pb_buffer_arg data = (pb_buffer_arg)
    { .buf = request->data_type, .buf_len = request->data_type_len - 1 };

  suid_event.data_type.funcs.encode = &pb_encode_string_cb;
  suid_event.data_type.arg = &data;
  suid_event.suid.funcs.encode = &encode_suid_cb;
  suid_event.suid.arg = &arg;

  pb_send_event(instance, sns_suid_event_fields, &suid_event,
      sns_get_system_time(), SNS_SUID_MSGID_SNS_SUID_EVENT, NULL);
}

/**
 * Add a new lookup request to the state of an existing SUID Sensor Instance.
 *
 * @param[i] state Sensor Instance state
 * @param[i] new_request Parsed request from a client
 *
 * @return True if active requests remain for this client
 */
static bool
add_req_to_client(sns_suid_sensor_instance_config *state,
    sns_suid_lookup_request *new_request)
{
  sns_isafe_list_iter lookups;

  for(sns_isafe_list_iter_init(&lookups, &state->requests, true);
      NULL != sns_isafe_list_iter_curr(&lookups); )
  {
    sns_suid_lookup_request *request = (sns_suid_lookup_request*)
      sns_isafe_list_iter_get_curr_data(&lookups);

    if(0 == sns_memcmp(request->data_type, new_request->data_type,
                       request->data_type_len))
    {
      sns_isafe_list_iter_remove(&lookups);
      sns_free(request);
    }
    else
    {
      sns_isafe_list_iter_advance(&lookups);
    }
  }

  if(new_request->register_updates)
    sns_isafe_list_iter_insert(&lookups, &new_request->list_entry, false);
  else
    sns_free(new_request);

  return 0 != sns_isafe_list_iter_len(&lookups);
}

/*
 * See sns_sensor::set_client_request
 *
 * This Sensor combines all requests from the same client to a single Instance,
 * regardless of the request.  Instances are never shared amongst clients.
 */
static sns_sensor_instance*
set_client_request(sns_sensor *const this, sns_request const *curr_req,
  sns_request const *new_req, bool remove)
{
  sns_sensor_instance *curr_inst =
    sns_sensor_util_find_instance(this, curr_req, get_sensor_uid(this));
  sns_sensor_instance *rv_inst = NULL;

  // Remove is called when a client has closed the data stream.  Since an
  // instance only serves a single client, we indicate to the Framework that it
  // should be removed, by removing all associated client requests (which
  // should be exactly one).
  if(remove)
  {
    if(NULL != curr_inst)
    {
      curr_inst->cb->remove_client_request(curr_inst, curr_req);
      this->cb->remove_instance(curr_inst);
    }
  }
  else if(SNS_SUID_MSGID_SNS_SUID_REQ == new_req->message_id)
  {
    sns_suid_lookup_request *lookup_request = NULL;

    if(parse_suid_request(new_req, &lookup_request))
    {
      // For this Sensor, Instances are created effectively only to hold state
      // Hence, we don't bother calling set_client_config
      rv_inst = (NULL != curr_inst) ? curr_inst :
        this->cb->create_instance(this, sizeof(sns_suid_sensor_instance_config));

      // The Framework does book-keeping based on client-requests, so we still
      // need to ensure that we remove the "replaced" client request, and add
      // the new one
      if(NULL != curr_inst && NULL != curr_req)
        curr_inst->cb->remove_client_request(curr_inst, curr_req);

      // Sending an event requires that the client be added to the queue first
      rv_inst->cb->add_client_request(rv_inst, new_req);
      send_suid_event(rv_inst, lookup_request);

      if(!add_req_to_client((sns_suid_sensor_instance_config*)
            rv_inst->state->state, lookup_request))
      {
        rv_inst->cb->remove_client_request(rv_inst, new_req);
        this->cb->remove_instance(rv_inst);
        rv_inst = &sns_instance_no_error;
      }
    }
  }
  else
  {
    rv_inst = NULL;
  }

  return rv_inst;
}

/**
 * Send requests to the registry sensor, for the list of default
 * sensor attributes.
 *
 * @param[i] this The SUID Lookup Sensor
 */
static void
suid_send_registry_request(sns_sensor *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_suid_sensor_state *state = (sns_suid_sensor_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);

  pb_buffer_arg data = (pb_buffer_arg) {
      .buf = reg_group_name,
      .buf_len = sizeof(reg_group_name)
  };

  rc = stream_svc->api->create_sensor_stream(stream_svc, this,
      registry_suid, &state->reg_stream);
  if(SNS_RC_SUCCESS == rc)
  {
    uint8_t buffer[100];
    int32_t encoded_len;
    sns_registry_read_req read_request;

    read_request.name.arg = &data;
    read_request.name.funcs.encode = pb_encode_string_cb;

    encoded_len = pb_encode_request(buffer, sizeof(buffer),
        &read_request, sns_registry_read_req_fields, NULL);
    if(encoded_len <= 0)
    {
      SNS_PRINTF(ERROR, this, "encoding failed");
    }
    else
    {
      sns_request request = (sns_request) {
        .request_len = encoded_len,
        .request = buffer,
        .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ
      };

      state->reg_stream->api->send_request(state->reg_stream, &request);
    }
  }
}

/*===========================================================================
  Public Function Definitions
  ===========================================================================*/

/**
 * Note: Normally acquiring the library lock in this fashion would be
 * dangerous.  We'll rely on the fact that the SUID Lookup Sensor is never
 * removed, nor will it ever try to create a stream to another Sensor, in
 * order to keep it safe.
 */
void
sns_suid_sensor_apprise(char const *data_type)
{
  /* when registry sensor become available, send registry requests */
  if(0 == strcmp(data_type, "registry"))
    suid_send_registry_request((sns_sensor*)suid_sensor);

  sns_osa_lock_acquire(suid_sensor->library->library_lock);

  default_sensor_match_attributes(data_type);

  for(sns_sensor_instance *instance = suid_sensor->sensor.cb->get_sensor_instance(&suid_sensor->sensor, true);
      NULL != instance;
      instance = suid_sensor->sensor.cb->get_sensor_instance(&suid_sensor->sensor, false))
  {
    sns_isafe_list_iter iter;
    sns_suid_sensor_instance_config *state = (sns_suid_sensor_instance_config*)
      instance->state->state;

    for(sns_isafe_list_iter_init(&iter, &state->requests, true);
        NULL != sns_isafe_list_iter_curr(&iter);
        sns_isafe_list_iter_advance(&iter))
    {
      sns_suid_lookup_request *lookup_req = (sns_suid_lookup_request*)
        sns_isafe_list_iter_get_curr_data(&iter);

      if(0 == strcmp(lookup_req->data_type, data_type))
        send_suid_event(instance, lookup_req);
    }
  }
  sns_osa_lock_release(suid_sensor->library->library_lock);
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api suid_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &init,
  .get_sensor_uid = &get_sensor_uid,
  .set_client_request = &set_client_request,
  .notify_event = &notify_event
};
