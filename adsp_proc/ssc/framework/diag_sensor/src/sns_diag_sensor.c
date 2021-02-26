/*=============================================================================
  @file sns_diag_sensor.c

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <string.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_attribute_service.h"
#include "sns_attribute_util.h"
#include "sns_diag_sensor.h"
#include "sns_diag_sensor.pb.h"
#include "sns_event_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_printf_int.h"
#include "sns_registry.pb.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_types.h"

/*=============================================================================
 * Macros and Constants
 ============================================================================*/

#define DIAG_SENSOR_SUID 0x85,0x90,0x68,0x12,0xa9,0x6f,0x43,0x94,\
    0xb5,0x87,0x39,0x20,0x18,0xe6,0x92,0x12
/*=============================================================================
 * Extern Data Definitions
 ============================================================================*/
extern sns_sensor_instance sns_instance_no_error;

/*=============================================================================
 * Data
 ============================================================================*/
bool sns_enable_qdss SNS_SECTION(".data.sns") = false;

/*=============================================================================
 * Static Function Definitions
 ============================================================================*/

/**
 * Publish all Sensor attributes
 *
 * @param[i] this reference to this Sensor
 */
static void
diag_sensor_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "diag_sensor";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "diag_sensor";
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
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
    sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }
  {
    char const proto_files[] = "sns_diag_sensor.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, true);
  }
}

static void
publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/* See sns_sensor::init */
static sns_rc
init(sns_sensor *const this)
{
  sns_diag_sensor_state *state = (sns_diag_sensor_state*)this->state->state;
  struct sns_service_manager *smgr= this->cb->get_service_manager(this);
  state->diag_service = (sns_diag_service*)
        smgr->get_service(smgr, SNS_DIAG_SERVICE);
  state->sensor_datatype_reg_read = false;

  diag_sensor_publish_attributes(this);
  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");

  return SNS_RC_SUCCESS;
}

/*
 * See sns_sensor::set_client_request
 */
static sns_sensor_instance*
set_client_request(sns_sensor *const this, const sns_request *curr_req,
    const sns_request *new_req, bool remove)
{
  UNUSED_VAR(curr_req);
  UNUSED_VAR(this);
  UNUSED_VAR(remove);
  if(NULL != new_req)
  {
    pb_istream_t stream =
      pb_istream_from_buffer(new_req->request, new_req->request_len);
    sns_std_request sr = sns_std_request_init_default;
    sns_diag_log_trigger_req dec_req = sns_diag_log_trigger_req_init_default;
    pb_simple_cb_arg arg =
      { .decoded_struct = &dec_req, .fields = sns_diag_log_trigger_req_fields };
    sr.payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

    if(SNS_DIAG_SENSOR_MSGID_SNS_DIAG_LOG_TRIGGER_REQ == new_req->message_id &&
       pb_decode(&stream, sns_std_request_fields, &sr))
    {
      uint64_t cookie = dec_req.has_cookie ? dec_req.cookie : 0;
      //SNS_PRINTF(HIGH, sns_fw_printf, "Log type: %x", dec_req.log_type);
      switch(dec_req.log_type)
      {
        case SNS_DIAG_TRIGGERED_LOG_TYPE_ISLAND_LOG:
          sns_island_generate_and_commit_state_log( cookie );
          break;
        case SNS_DIAG_TRIGGERED_LOG_TYPE_MEMORY_USAGE_LOG:
          sns_memmgr_log_heap_usage( cookie );
          sns_event_service_log_buffer_use( cookie );
          break;
        default:
          break;
      }
    }
  }
  return &sns_instance_no_error;
}

static void
send_registry_request(sns_sensor *const this)
{
  sns_diag_sensor_state *state = (sns_diag_sensor_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_registry_read_req read_request;
  char diag_sensor_group_name[] = "sns_diag_sensor_datatype";
  pb_buffer_arg data = (pb_buffer_arg){ .buf = diag_sensor_group_name,
    .buf_len = sizeof(diag_sensor_group_name) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len,
          .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    state->reg_stream->api->send_request(state->reg_stream, &request);
  }

  char diag_sensor_config_group_name[] = "sns_diag_config";
  pb_buffer_arg config_data = (pb_buffer_arg){ .buf = diag_sensor_config_group_name,
    .buf_len = sizeof(diag_sensor_config_group_name) };

  read_request.name.arg = &config_data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len,
          .request = buffer,
          .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    state->reg_stream->api->send_request(state->reg_stream, &request);
  }
}

static bool
diag_sensor_registry_item_decode_cb(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(arg);
  UNUSED_VAR(field);
  bool rv = true;
  pb_buffer_arg  name_data;
  sns_registry_data_item pb_item = sns_registry_data_item_init_default;
  sns_sensor* this = (sns_sensor*)*arg;

  pb_item.name.funcs.decode = &pb_decode_string_cb;
  pb_item.name.arg = &name_data;

  if(!pb_decode(stream, sns_registry_data_item_fields, &pb_item))
  {
    SNS_PRINTF(ERROR, this, "Error in decoding");

    rv = false;
  }
  else if(0 == name_data.buf_len)
  {
    SNS_PRINTF(ERROR, this, "Missing data_type name");
    rv = false;
  }
  else
  {
    if(pb_item.has_sint)
    {
      // calling diag_service to apply filter
      SNS_SPRINTF(LOW, sns_fw_printf, "value received for %s is %d",
          (char*)name_data.buf, (int)pb_item.sint);
       sns_diag_set_filter((char*)name_data.buf, (bool)pb_item.sint);
    }
    else
    {
      SNS_PRINTF(ERROR, this, "Diag filter Unknown item value");
      rv = false;
    }
  }
  return rv;
}

static bool
diag_sensor_registry_config_decode_cb(pb_istream_t *stream,
    const pb_field_t *field, void **arg)
{
  UNUSED_VAR(arg);
  UNUSED_VAR(field);
  bool rv = true;
  pb_buffer_arg  name_data;
  sns_registry_data_item pb_item = sns_registry_data_item_init_default;
  sns_sensor* this = (sns_sensor*)*arg;

  pb_item.name.funcs.decode = &pb_decode_string_cb;
  pb_item.name.arg = &name_data;

  if(!pb_decode(stream, sns_registry_data_item_fields, &pb_item))
  {
    SNS_PRINTF(ERROR, this, "Error in decoding");

    rv = false;
  }
  else if(0 == name_data.buf_len)
  {
    SNS_PRINTF(ERROR, this, "Missing data_type name");
    rv = false;
  }
  else if(0 == strncmp((char*)name_data.buf, "qdss", name_data.buf_len) && (pb_item.has_sint))
  {
    sns_enable_qdss = (0 == pb_item.sint)? false : true;
    SNS_PRINTF(LOW, this, "sns_enable_qdss set to %d\n", sns_enable_qdss);
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Diag filter Unknown item value");
    rv = false;
  }
  return rv;
}

static void
diag_sensor_process_registry_events(sns_sensor *const this)
{
  sns_diag_sensor_state *state = (sns_diag_sensor_state*)this->state->state;

  for(sns_sensor_event *event = state->reg_stream->api->peek_input(state->reg_stream);
      NULL != event;
      event = state->reg_stream->api->get_next_input(state->reg_stream))
  {
    SNS_PRINTF(LOW, this, "notification event pointer is %p message is %d",
        event, event->message_id);

    pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
        event->event_len);
    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
      sns_registry_read_event read_event = sns_registry_read_event_init_default;
      pb_buffer_arg data = {0,0};
      read_event.name.arg = &data;
      read_event.name.funcs.decode = pb_decode_string_cb;

      if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
      {
        SNS_PRINTF(ERROR, this, "Error decoding registry group name");
        return;
      }

      if((0 == strncmp(data.buf, "sns_diag_sensor_datatype", data.buf_len)))
      {
        SNS_PRINTF(LOW, this, "Decoding datatypes");
        read_event.data.items.funcs.decode = &diag_sensor_registry_item_decode_cb;
        read_event.data.items.arg = this;

        if(!state->sensor_datatype_reg_read)
        {
          // first time sns_diag_sensor_dataype group is being read
          // clear all data type filters before applying settings read from registry
          sns_diag_set_filter_all(false);
          state->sensor_datatype_reg_read = true;
        }
      }
      else if((0 == strncmp(data.buf, "sns_diag_config", data.buf_len)))
      {
        SNS_PRINTF(LOW, this, "Decoding config");
        read_event.data.items.funcs.decode = &diag_sensor_registry_config_decode_cb;
        read_event.data.items.arg = this;
      }
      else
      {
        SNS_SPRINTF(LOW, this, "Decoding registry group: %s", data.buf);
        return;
      }

      stream = pb_istream_from_buffer((void*)event->event,
        event->event_len);

      if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
      {
        SNS_PRINTF(ERROR, this, "Error decoding registry items");
        return;
      }

      SNS_SPRINTF(LOW, sns_fw_printf, "Registry Event decoding complete (%s)",
          (char*)data.buf);
    }
  }
}

sns_rc notify_event(sns_sensor *const this)
{
  sns_diag_sensor_state *state = (sns_diag_sensor_state*)this->state->state;
  sns_service_manager *manager = this->cb->get_service_manager(this);
  sns_stream_service *stream_service =
      (sns_stream_service*)manager->get_service(manager, SNS_STREAM_SERVICE);
  bool completed = sns_suid_lookup_complete(&state->suid_lookup_data);

  sns_suid_lookup_handle(this, &state->suid_lookup_data);

  SNS_PRINTF(LOW, this, "diag_sensor notified");

  if(completed != sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    sns_sensor_uid suid;

    publish_available(this);
    sns_suid_lookup_get(&state->suid_lookup_data, "registry", &suid);
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);

    if(NULL == state->reg_stream)
    {
      stream_service->api->create_sensor_stream(stream_service,
          this, suid, &state->reg_stream);
      send_registry_request(this);
    }
  }
  if(NULL != state->reg_stream)
  {
    diag_sensor_process_registry_events(this);
  }
  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid diag_sensor_suid = {.sensor_uid = {DIAG_SENSOR_SUID}};
  return &diag_sensor_suid;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_diag_sensor_api =
{
    .struct_len = sizeof(sns_sensor_api),
    .init = &init,
    .deinit = NULL,
    .get_sensor_uid = &get_sensor_uid,
    .set_client_request = &set_client_request,
    .notify_event = &notify_event,
};
