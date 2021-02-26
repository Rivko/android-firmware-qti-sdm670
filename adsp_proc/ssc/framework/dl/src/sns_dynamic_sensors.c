/*=============================================================================
  @file sns_dynamic_sensors.c

  Query the Registry for the list of shared objects, then load and initialize
  all Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*============================================================================
  Include Files
  ===========================================================================*/

#include <dlfcn.h>
#include <stdbool.h>
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_registry_util.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_service.h"
#include "sns_stream_service.h"
#include "sns_suid_util.h"

/*============================================================================
  Macro Definitions
  ===========================================================================*/

#define REG_GROUP_NAME "sns_dynamic_sensors"

/*============================================================================
  Type Definitions
  ===========================================================================*/

/** Sensor state structure */
typedef struct sns_dynamic_sensor_state
{
  /* Registry SUID Lookup */
  SNS_SUID_LOOKUP_DATA(1) suid_lookup_data;
  /* Registry data stream */
  sns_data_stream *reg_stream;
} sns_dynamic_sensor_state;

/*============================================================================
  Static Data
  ===========================================================================*/

#define DYNAMIC_SENSOR_SUID 0x5d, 0x91, 0xe3, 0x8d, 0x23, 0xc9, 0x46, 0x6a,\
                            0x27, 0xbd, 0x30, 0x85, 0x67, 0xc6, 0xa9, 0x5e

/*============================================================================
  Static Functions
  ===========================================================================*/

/**
 * Publish all Sensor attributes
 *
 * @param[i] this Reference to this Sensor
 */
static void
publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "dl";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "dl";
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
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_boolean = true;
    value.boolean = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

/**
 * Send the Registry request for the list of shared objects to load.
 *
 * @param[i] stream Data stream already opened to the Registry
 *
 * @return true upon success; false otherwise
 */
static bool
send_registry_request(sns_data_stream *stream)
{
  bool rv = false;
  uint32_t encoded_len;
  uint8_t buffer[100];
  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = REG_GROUP_NAME,
    .buf_len = (strlen(REG_GROUP_NAME) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
          &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rv = (SNS_RC_SUCCESS == stream->api->send_request(stream, &request));
  }

  return rv;
}

/**
 * Handle a shared object entry from the registry group
 *
 * @param[i] reg_item registry item
 * @param[i] register_func_name Register function name
 * @param[i] so_path Shared object path
 * @param[o] parsed_buffer Unused
 *
 * @return true
 */
static bool
load_shared_object(sns_registry_data_item *reg_item,
    pb_buffer_arg *register_func_name, pb_buffer_arg *so_path,
    void *parsed_buffer)
{
  UNUSED_VAR(reg_item);
  sns_sensor *this = (sns_sensor*)parsed_buffer;

  if(NULL != register_func_name && NULL != so_path)
  {
    void *handle;

    sns_register_cb reg_cb = (sns_register_cb)
    {
      .struct_len = sizeof(reg_cb),
      .init_sensor = &sns_sensor_init
    };

    SNS_SPRINTF(MED, this, "Load Dynamic Sensor '%s'", so_path->buf);

    handle = dlopen(so_path->buf, RTLD_NOW);
    if(0 == handle)
    {
      SNS_PRINTF(ERROR, this, "dlopen failed");
    }
    else
    {
      sns_register_sensors register_func = (sns_register_sensors)
        dlsym(handle, register_func_name->buf);
      if(0 == register_func)
      {
        SNS_PRINTF(ERROR, this, "dlsym failed");
        dlclose(handle);
      }
      else
      {
        sns_rc rc;
        sns_sensor_library *library;
        sns_isafe_list_iter iter;

        SNS_SPRINTF(MED, this, "Register Dynamic Sensor '%s'",
            register_func_name->buf);

        library = sns_sensor_library_init(register_func, 0);
        sns_osa_lock_acquire(library->library_lock);
        rc = register_func(&reg_cb);
        library->removing = SNS_LIBRARY_ACTIVE;
        sns_sensor_library_start(library);

        sns_isafe_list_iter_init(&iter, &library->sensors, true);
        if(0 == sns_isafe_list_iter_len(&iter))
        {
          sns_sensor_library_deinit(library);
          sns_osa_lock_release(library->library_lock);
          sns_sensor_library_delete(library);
        }
        else
          sns_osa_lock_release(library->library_lock);

        SNS_SPRINTF(MED, this, "Register Dynamic Sensor '%s' rc %d",
            register_func_name->buf,rc);

        if(SNS_RC_SUCCESS != rc)
        {
          SNS_PRINTF(ERROR, this, "Register failed");
          sns_sensor_library_deinit(library);
          sns_sensor_library_delete(library);
          dlclose(handle);
        }
      }
    }
  }

  return true;
}

/**
 * Process an event from the Registry Sensor.
 *
 * @param[i] this Dynamic Sensor reference
 * @param[i] event Registry event
 */
static void
process_registry_event(sns_sensor *this, sns_sensor_event const *event)
{
  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0, 0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_SPRINTF(ERROR, this, "Error decoding event: %s", PB_GET_ERROR(&stream));
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);

      if(0 == sns_memcmp((char*)group_name.buf, REG_GROUP_NAME, group_name.buf_len))
      {
        sns_registry_decode_arg arg = {
          .item_group_name = &group_name,
          .parse_info_len = 1,
          .parse_info[0] = {
            .parse_func = load_shared_object,
            .parsed_buffer = (void*)this
          }
        };
        read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
        read_event.data.items.arg = &arg;

        if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
        {
          SNS_SPRINTF(ERROR, this, "Error decoding registry event: %s",
              PB_GET_ERROR(&stream));
        }
      }
      else
      {
        SNS_SPRINTF(HIGH, this, "Unknown registry group %s",
            (char*)group_name.buf);
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Unknown event msg id %i", event->message_id);
  }
}

/**
 * Handle events received on the registry stream.
 */
static void
registry_handle(sns_sensor *this, sns_data_stream *stream)
{
  if(NULL != stream)
  {
    for(sns_sensor_event *event = stream->api->peek_input(stream);
        NULL != event;
        event = stream->api->get_next_input(stream))
    {
      process_registry_event(this, event);
    }
  }
}

/*===========================================================================
  Sensor API Definitions
  ===========================================================================*/

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid power_sensor_suid =
    {.sensor_uid = {DYNAMIC_SENSOR_SUID}};
  return &power_sensor_suid;
}

/* See sns_sensor::init */
static sns_rc
sensor_init(sns_sensor *const this)
{
  sns_dynamic_sensor_state *state = (sns_dynamic_sensor_state*)this->state->state;

  publish_attributes(this);

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::notify_event */
static sns_rc
notify_event(sns_sensor *const this)
{
  sns_dynamic_sensor_state *state = (sns_dynamic_sensor_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
      (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  bool completed = sns_suid_lookup_complete(&state->suid_lookup_data);

  sns_suid_lookup_handle(this, &state->suid_lookup_data);
  if(completed != sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    sns_sensor_uid suid;

    sns_suid_lookup_get(&state->suid_lookup_data, "registry", &suid);
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);

    stream_service->api->create_sensor_stream(stream_service,
        this, suid, &state->reg_stream);

    SNS_PRINTF(LOW, this, "Sending registry request");
    if(!send_registry_request(state->reg_stream))
    {
      SNS_PRINTF(ERROR, this, "Error sending registry request");
    }
  }

  registry_handle(this, state->reg_stream);

  return SNS_RC_SUCCESS;
}

sns_sensor_api sns_dynamic_sensor_api =
{
    .struct_len = sizeof(sns_sensor_api),
    .init = &sensor_init,
    .deinit = NULL,
    .get_sensor_uid = &get_sensor_uid,
    .set_client_request = NULL,
    .notify_event = &notify_event,
};

sns_rc
sns_register_dynamic_sensor(sns_register_cb const *register_api)
{
  UNUSED_VAR(register_api);
  register_api->init_sensor(sizeof(sns_dynamic_sensor_state),
      &sns_dynamic_sensor_api, NULL);

  return SNS_RC_SUCCESS;
}
