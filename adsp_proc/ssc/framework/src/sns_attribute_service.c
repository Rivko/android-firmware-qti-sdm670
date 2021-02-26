/*=============================================================================
  @file sns_attribute_service.c

  Private Framework state for the Attribute Service.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdbool.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_attribute_service.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_request.h"
#include "sns_fw_sensor.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_sensor.h"
#include "sns_suid_sensor.h"
#include "sns_types.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_pb_util.h"
#include "sns_std.pb.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Private Framework state for the Attribute Manager.
 */
typedef struct sns_fw_attribute_service
{
  sns_attribute_service service;

  /* List of type sns_attribute_info */
  sns_isafe_list sensor_attributes;
  sns_osa_lock *sensor_attributes_lock;
} sns_fw_attribute_service;

/**
 * A single Sensor attribute, as published by the Sensor.
 */
typedef struct sns_attribute
{
  /* Kept on sns_attribute_info::attributes */
  sns_isafe_list_item list_entry;
  /* ID  of this attribute entry; only one entry on list per ID */
  sns_attribute_id id;
  /* Length of the value buffer */
  uint32_t value_len;
  /* PB encoded sns_std_attr */
  uint64_t value[];
} sns_attribute;

/**
 * Attributes that will reside in DDR memory
 */
typedef struct sns_attribute_info_big
{
  /* Kept on sns_fw_attribute_service::sensor_attributes */
  sns_isafe_list_item list_entry;

  /* List of clients registered for attribute updates.
   * Whenever a Sensor publishes attributes, generate an Event using the
   * EventManager, which will then notify clients of the change. */
  sns_isafe_list clients;

  /* All published attributes for a Sensor; sns_attribute */
  sns_isafe_list attributes;
  /* Sensor UID associated with this Sensor */
  sns_sensor_uid suid;
  /* Pointer to the associated Sensor */
  sns_fw_sensor *sensor;

  /* The following field values are determined strictly from Sensor-published
   * attributes, and are available to optimization purposes only. */
  bool is_default;

  /* Data type of this sensor; points into attributes */
  char const *data_type;
  /* Vendor of this sensor; points into attributes */
  char const *vendor;

}sns_attribute_info_big;

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

static sns_fw_attribute_service attribute_service SNS_SECTION(".data.sns");
static sns_attribute_service_api attribute_service_api SNS_SECTION(".data.sns");

static const uint16_t version = 1;

// PEND: The following list is better suited in the registry
static const char *event_priority_datatypes[] =
{
  "timer",
  "interrupt",
  "data_acquisition_engine",
};
static const char *req_priority_datatypes[] =
{
  "timer",
  "interrupt",
  "data_acquisition_engine",
  "async_com_port",
};

/*=============================================================================
  Attribute Information Function Definitions
  ===========================================================================*/

sns_rc
sns_attr_svc_sensor_init(sns_fw_sensor *sensor)
{
  sns_attribute_info_big *attr_info = NULL;
  sns_isafe_list_iter iter;

  attr_info = (sns_attribute_info_big *)sns_malloc(SNS_HEAP_MAIN, sizeof(sns_attribute_info_big));
  SNS_ASSERT(NULL != attr_info);

  sns_isafe_list_item_init(&attr_info->list_entry, attr_info);
  sns_isafe_list_init(&attr_info->clients);
  sns_isafe_list_init(&attr_info->attributes);
  attr_info->sensor = sensor;
  attr_info->vendor = NULL;
  attr_info->data_type = NULL;

  sensor->attr_info->attr_info = attr_info;
  sensor->attr_info->priority = (sns_attr_priority){ false, false };
  sensor->attr_info->available = false;
  sensor->attr_info->is_physical_sensor = false;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  sns_isafe_list_iter_init(&iter, &attribute_service.sensor_attributes,
      (SNS_ISLAND_STATE_NOT_IN_ISLAND != sensor->island_operation));
  sns_isafe_list_iter_insert(&iter, &attr_info->list_entry, true);
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);

  return SNS_RC_SUCCESS;
}

void
sns_attr_svc_sensor_deinit(sns_fw_sensor *sensor)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  sns_isafe_list_item_remove(&sensor->attr_info->attr_info->list_entry);
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  sns_free(sensor->attr_info->attr_info);
}

void
sns_attr_info_get_vendor(sns_attribute_info const *attr_info,
    char *vendor, uint32_t vendor_len)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  if(NULL != attr_info->attr_info->vendor)
    sns_strlcpy(vendor, attr_info->attr_info->vendor, vendor_len);
  else if(vendor_len >= 1)
    vendor[0] = '\0';
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

void
sns_attr_info_set_vendor(sns_attribute_info *attr_info,
    char const *vendor)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->attr_info->vendor = vendor;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

void
sns_attr_info_get_data_type(sns_attribute_info const *attr_info,
    char *data_type, uint32_t data_type_len)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  if(NULL != attr_info->attr_info->data_type)
    sns_strlcpy(data_type, attr_info->attr_info->data_type, data_type_len);
  else if(data_type_len >= 1)
    data_type[0] = '\0';
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

void
sns_attr_info_set_data_type(sns_attribute_info *attr_info,
    char const *data_type)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->attr_info->data_type = data_type;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

bool
sns_attr_info_get_default(struct sns_attribute_info const *attr_info)
{
  bool val;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  val = attr_info->attr_info->is_default;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return val;
}

void
sns_attr_info_set_default(struct sns_attribute_info *attr_info, bool val)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->attr_info->is_default = val;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

SNS_SECTION(".text.sns") sns_attr_priority
sns_attr_info_get_priority(sns_attribute_info const *attr_info)
{
  sns_attr_priority priority;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  priority = attr_info->priority;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return priority;
}

SNS_SECTION(".text.sns") static void
sns_attr_info_set_priority(sns_attribute_info *attr_info,
    sns_attr_priority priority)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->priority = priority;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

SNS_SECTION(".text.sns") bool
sns_attr_info_get_available(sns_attribute_info const *attr_info)
{
  bool available = false;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  available = attr_info->available;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return available;
}

SNS_SECTION(".text.sns") static void
sns_attr_info_set_available(sns_attribute_info *attr_info,
    bool available)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->available = available;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

SNS_SECTION(".text.sns") bool
sns_attr_info_get_is_physical_sensor(sns_attribute_info const *attr_info)
{
  bool is_physical_sensor;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  is_physical_sensor = attr_info->is_physical_sensor;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return is_physical_sensor;
}

SNS_SECTION(".text.sns") static void
sns_attr_info_set_is_physical_sensor(sns_attribute_info *attr_info,
    bool is_physical_sensor)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->is_physical_sensor = is_physical_sensor;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

sns_sensor_uid
sns_attr_info_get_suid(sns_attribute_info const *attr_info)
{
  sns_sensor_uid suid;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  suid = attr_info->attr_info->suid;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return suid;
}

void
sns_attr_info_set_suid(sns_attribute_info *attr_info,
    sns_sensor_uid const *suid)
{
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  attr_info->attr_info->suid = *suid;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
}

sns_fw_sensor*
sns_attr_info_get_sensor(sns_attribute_info const *attr_info)
{
  sns_fw_sensor *sensor;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  sensor = attr_info->attr_info->sensor;
  if(!sns_island_is_island_ptr((intptr_t)sensor) ||
     SNS_ISLAND_STATE_IN_ISLAND != sensor->island_operation)
  {
    SNS_ISLAND_EXIT();
  }
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return sensor;
}

sns_sensor_library*
sns_attr_info_get_library(struct sns_attribute_info const *attr_info)
{
  sns_sensor_library *library;
  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  library = attr_info->attr_info->sensor->library;
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return library;
}

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* @see pb_callback_s::decode */
bool
pb_decode_attr_value_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  pb_buffer_arg *buf = (pb_buffer_arg*)*arg;
  pb_buffer_arg str_data = (pb_buffer_arg){ .buf = NULL, .buf_len = 0 };
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.str.funcs.decode = &pb_decode_string_cb;
  value.str.arg = &str_data;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "pb_decode_attr_value_cb failed: %s",
        PB_GET_ERROR(stream));
    return false;
  }

  if(value.has_boolean)
    buf->buf = (void*)value.boolean;
  else if(NULL != str_data.buf)
  {
    buf->buf = str_data.buf;
    buf->buf_len = str_data.buf_len;
  }

  return true;
}

bool
pb_decode_attr_check_value_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  static const char std_sensor_proto[] = "sns_std_sensor.proto";
  pb_buffer_arg *buf = (pb_buffer_arg*)*arg;
  pb_buffer_arg str_data = (pb_buffer_arg){ .buf = NULL, .buf_len = 0 };
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.str.funcs.decode = &pb_decode_string_cb;
  value.str.arg = &str_data;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &value))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "pb_decode_attr_value_cb failed: %s",
        PB_GET_ERROR(stream));
    return false;
  }
  if(NULL != str_data.buf && NULL == buf->buf)
  {
    if(0 == strncmp(str_data.buf, std_sensor_proto, sizeof(std_sensor_proto)))
    {
      buf->buf = str_data.buf;
      buf->buf_len = str_data.buf_len;
    }
  }
  return true;
}

/**
 * Decode a simple/single attribute value.
 * (Only supports strings and boolean, defaults to '0').
 *
 * @param[i] attr Attribute to decode
 *
 * @return Contained value
 */
static void const*
decode_attribute(sns_attribute *attribute,
  bool (*decode_func_cb)(pb_istream_t *stream, const pb_field_t *field, void **arg))
{
  pb_buffer_arg value = (pb_buffer_arg){ .buf = NULL, .buf_len = 0 };
  sns_std_attr attr = sns_std_attr_init_default;
  pb_istream_t stream = pb_istream_from_buffer(
      (pb_byte_t const*)attribute->value, attribute->value_len);

  attr.value.values.funcs.decode = decode_func_cb;
  attr.value.values.arg = &value;

  if(!pb_decode(&stream, sns_std_attr_fields, &attr))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "pb_decode failed: %s", PB_GET_ERROR(&stream));
    return NULL;
  }

  return value.buf;
}

/**
 * Process and parse attributes of note to the FW.  These will be saved
 * as part of the FW Sensor's state.
 *
 * @param[i] new_attr New Attribute just published by the Sensor
 * @param[i] fw_sensor Sensor which published the attribute
 */
SNS_SECTION(".text.sns") static void
process_special_attributes(sns_attribute *new_attr, sns_fw_sensor *fw_sensor)
{
  if(SNS_STD_SENSOR_ATTRID_AVAILABLE == new_attr->id)
  {
    SNS_ISLAND_EXIT();
    bool available = (bool)decode_attribute(new_attr, &pb_decode_attr_value_cb);
    if(available != sns_attr_info_get_available(fw_sensor->attr_info))
    {
      char data_type[32];
      sns_attr_info_get_data_type(fw_sensor->attr_info, data_type, sizeof(data_type));
      sns_attr_info_set_available(fw_sensor->attr_info, available);
      sns_suid_sensor_apprise(data_type);
    }
    sns_diag_register_sensor(fw_sensor);
  }
  else if(SNS_STD_SENSOR_ATTRID_TYPE == new_attr->id)
  {
    sns_attr_priority priority = { false, false };
    char const *data_type =
      (char*)decode_attribute(new_attr, &pb_decode_attr_value_cb);
    SNS_ASSERT(NULL != data_type);
    sns_attr_info_set_data_type(fw_sensor->attr_info, data_type);

    for(uint8_t i = 0; i < ARR_SIZE(event_priority_datatypes); i++)
    {
      if(0 == strcmp(data_type, event_priority_datatypes[i]))
      {
        priority.event_priority = true;
        break;
      }
    }
    for(uint8_t i = 0; i < ARR_SIZE(req_priority_datatypes); i++)
    {
      if(0 == strcmp(data_type, req_priority_datatypes[i]))
      {
        priority.req_priority = true;
        break;
      }
    }

    sns_attr_info_set_priority(fw_sensor->attr_info, priority);
  }
  else if(SNS_STD_SENSOR_ATTRID_VENDOR == new_attr->id)
  {
    char const *vendor = (char*)decode_attribute(new_attr, &pb_decode_attr_value_cb);
    SNS_ISLAND_EXIT();
    sns_attr_info_set_vendor(fw_sensor->attr_info, vendor);
  }
  else if(SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR == new_attr->id &&
          !sns_attr_info_get_is_physical_sensor(fw_sensor->attr_info))
  {
    bool physical_sensor = (bool)decode_attribute(new_attr, &pb_decode_attr_value_cb);
    sns_attr_info_set_is_physical_sensor(fw_sensor->attr_info, physical_sensor);
  }
}

/* See sns_attribute_service_api::publish_attribute */
SNS_SECTION(".text.sns") static sns_rc
publish_attribute(sns_attribute_service *this, struct sns_sensor *sensor,
    void const *attribute, uint32_t attribute_len, sns_attribute_id attribute_id,
    bool completed)
{
  UNUSED_VAR(this);
  UNUSED_VAR(completed);
  sns_isafe_list_iter iter;
  sns_fw_sensor *fw_sensor = (sns_fw_sensor*)sensor;
  sns_attribute *new_attr;
  sns_mem_heap_id heap = SNS_HEAP_MAIN;
  sns_isafe_list_item *curr_attr = NULL;

  SNS_ISLAND_EXIT();
  new_attr = sns_malloc(heap, sizeof(*new_attr) + attribute_len);

  if(SNS_HEAP_ISLAND == heap && NULL == new_attr)
  {
    new_attr = sns_malloc(SNS_HEAP_MAIN, sizeof(*new_attr) + attribute_len);
    fw_sensor->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;
  }
  SNS_ASSERT(NULL != new_attr);

  sns_isafe_list_item_init(&new_attr->list_entry, new_attr);
  new_attr->id = attribute_id;
  new_attr->value_len = attribute_len;
  sns_memscpy(&new_attr->value, attribute_len, attribute, attribute_len);

  for(curr_attr = sns_isafe_list_iter_init(&iter, &fw_sensor->attr_info->attr_info->attributes, true);
      NULL != curr_attr;
      curr_attr = sns_isafe_list_iter_advance(&iter))
  {
    sns_attribute *attr =
       (sns_attribute*)sns_isafe_list_item_get_data(curr_attr);
    if(attr->id == attribute_id)
    {
      sns_isafe_list_iter_remove(&iter);
      sns_free(attr);
      break;
    }
  }

  sns_isafe_list_iter_insert(&iter, &new_attr->list_entry, false);
  process_special_attributes(new_attr, fw_sensor);

  return SNS_RC_SUCCESS;
}

/**
 * Encode the array of attributes into an attribute event.
 */
static bool
attr_event_encode_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_fw_sensor const *sensor = (sns_fw_sensor const*)*arg;
  sns_isafe_list_iter iter;
  sns_isafe_list_item *curr_attr = NULL;

  for(curr_attr = sns_isafe_list_iter_init(&iter, &sensor->attr_info->attr_info->attributes, true);
      NULL != curr_attr;
      curr_attr = sns_isafe_list_iter_advance(&iter))
  {
    sns_attribute *attr =
       (sns_attribute*)sns_isafe_list_item_get_data(curr_attr);

    if(!pb_encode_tag_for_field(stream, field))
      return false;

    if(!pb_encode_string(stream, (pb_byte_t*)attr->value, attr->value_len))
      return false;
  }

  return true;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") sns_rc
sns_attr_svc_handle_req(sns_sensor *sensor, sns_fw_request *req)
{
  pb_istream_t istream;
  sns_std_attr_req request;

  SNS_ISLAND_EXIT();

  sns_std_request pb_req = sns_std_request_init_default;
  pb_simple_cb_arg arg =
    { .decoded_struct = &request, .fields = sns_std_attr_req_fields };
  pb_req.payload = (struct pb_callback_s)
    { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

  istream = pb_istream_from_buffer(req->request.request, req->request.request_len);
  if(pb_decode(&istream, sns_std_request_fields, &pb_req))
  {
    size_t encoded_len;
    sns_std_attr_event event = sns_std_attr_event_init_default;
    event.attributes.funcs.encode = &attr_event_encode_cb;
    event.attributes.arg = sensor;

    if(pb_get_encoded_size(&encoded_len, sns_std_attr_event_fields, &event))
    {
      uint8_t *buffer = sns_malloc(SNS_HEAP_MAIN, encoded_len);
      SNS_ASSERT(NULL != buffer);
      pb_ostream_t ostream = pb_ostream_from_buffer(buffer, encoded_len);

      if(pb_encode(&ostream, sns_std_attr_event_fields, &event))
      {
        sns_event_service_send_event(req->stream, SNS_STD_MSGID_SNS_STD_ATTR_EVENT,
            encoded_len, buffer);
      }
      else
      {
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding attribute event: %s",
            PB_GET_ERROR(&ostream));
      }
      sns_free(buffer);
    }
    else
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "pb_get_encoded_size Error (attr event)");
    }
  }
  else
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding SUID request: %s",
        PB_GET_ERROR(&istream));
  }

  return SNS_RC_SUCCESS;
}

sns_fw_attribute_service*
sns_attr_svc_init(void)
{
  sns_osa_lock_attr attr;
  sns_rc rc;

  attribute_service_api = (sns_attribute_service_api)
  {
    .struct_len = sizeof(attribute_service_api),
    .publish_attribute = &publish_attribute
  };

  sns_isafe_list_init(&attribute_service.sensor_attributes);

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &attribute_service.sensor_attributes_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  attribute_service.service.service.version = version;
  attribute_service.service.service.type = SNS_ATTRIBUTE_SERVICE;
  attribute_service.service.api = &attribute_service_api;

  return &attribute_service;
}

SNS_SECTION(".text.sns") bool
sns_attr_svc_sensor_foreach(sns_attr_svc_sensor_foreach_func func, void *arg)
{
  bool rv = true;
  sns_isafe_list_iter iter;

  if(!sns_island_is_island_ptr((intptr_t)func) ||
     !sns_island_is_island_ptr((intptr_t)arg))
    SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);

  for(sns_isafe_list_iter_init(&iter, &attribute_service.sensor_attributes, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_attribute_info_big *attr_info =
      sns_isafe_list_iter_get_curr_data(&iter);

    if(!func(attr_info->sensor->attr_info, arg))
    {
      rv = false;
      break;
    }
  }

  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return rv;
}

bool
sns_attr_info_process_attribute(struct sns_attribute_info *attr_info,
    uint32_t attribute_id, process_attribute_cb cb, void *cb_arg)
{
  bool rc = true;
  sns_isafe_list_iter iter;
  sns_attribute *attr;

  sns_osa_lock_acquire(attribute_service.sensor_attributes_lock);
  for(sns_isafe_list_iter_init(&iter, &attr_info->attr_info->attributes, true);
      NULL != (attr = (sns_attribute*)sns_isafe_list_iter_get_curr_data(&iter));
      sns_isafe_list_iter_advance(&iter))
  {
    if(attr->id == attribute_id)
    {
      rc = cb(attr->value, attr->value_len, cb_arg);
      break;
    }
  }
  sns_osa_lock_release(attribute_service.sensor_attributes_lock);
  return rc;
}
