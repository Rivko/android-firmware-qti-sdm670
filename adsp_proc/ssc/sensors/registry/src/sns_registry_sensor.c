/*=============================================================================
  @file sns_registry_sensor.c

  @note Registry write requests are considered atomic actions.  That is to
  say, either all items from a registry request are written successfully, or
  none of them are.

  Copyright (c) 2017 - 2018 Qualcomm Technologies, Inc.
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
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_registry.pb.h"
#include "sns_registry_parser.h"
#include "sns_registry_sensor.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_thread_prio.h"
#include "sns_types.h"
#include "sns_pwr_scpm_mgr.h"

/*=============================================================================
 * Extern Data Definitions
 ============================================================================*/
extern sns_sensor_instance sns_instance_no_error;

/*=============================================================================
 * Static Data Definitions
 ============================================================================*/
#define REGISTRY_SUID 0x75,0x22,0x1e,0x70,0xb4,0x41,0x25,0x5e,\
                      0x59,0x27,0x7f,0x00,0xa7,0x54,0x27,0xe1

/* Registry Initialization thread */
static sns_osa_thread *reg_init_thread;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_sensor::event_notify */
static sns_rc
notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  // This Sensor receives no events
  return SNS_RC_SUCCESS;
}

/**
 * Registry initialization thread.
 *
 * File access function calls block until the sdsprpc Daemon is available on
 * the HLOS.  Until this Daemon is started automatically, we want to ensure
 * the rest of the SSC initialization can still be completed.
 */
static void
sns_reg_init_task(void *arg)
{
  sns_sensor *const this = (sns_sensor*)arg;

  /*Bumps up the clock till registry init complete to avoid the boottime 
  regression*/
  sns_scpm_mgr_client *scpm_client = sns_pwr_scpm_mgr_create_client();
  sns_pwr_scpm_mgr_mcps_bump_up(scpm_client);

  sns_osa_thread_active();
  if(SNS_RC_SUCCESS != sns_registry_init())
  {
    SNS_PRINTF(ERROR, this, "Error initializing registry");
  }
  else
  {
    sns_osa_lock_acquire(((sns_fw_sensor*)this)->library->library_lock);
    {
      char const name[] = "registry";
      sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
      value.str.funcs.encode = pb_encode_string_cb;
      value.str.arg = &((pb_buffer_arg)
          { .buf = name, .buf_len = sizeof(name) });
      sns_publish_attribute(
          this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
    }
    {
      char const type[] = "registry";
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
      char const proto_files[] = "sns_registry.proto";
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
    sns_osa_lock_release(((sns_fw_sensor*)this)->library->library_lock);
  }
  sns_pwr_scpm_mgr_mcps_bump_down(scpm_client);
  sns_osa_thread_idle();
  sns_osa_thread_delete(reg_init_thread);
}

/* See sns_sensor::init */
static sns_rc
init(sns_sensor *const this)
{
  sns_osa_thread_attr attr;
  sns_rc rc = SNS_RC_SUCCESS;

  sns_osa_thread_attr_init(&attr);
  rc |= sns_osa_thread_attr_set_name(&attr, "SNS_REG_INIT");
  rc |= sns_osa_thread_attr_set_stack(&attr, (uintptr_t)NULL, 4096);
  rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_NORMAL);
  rc |= sns_osa_thread_attr_set_priority(&attr, SNS_REGISTRY_THREAD_PRIO);

  if(SNS_RC_SUCCESS == rc)
    rc = sns_osa_thread_create(&sns_reg_init_task, this, &attr, &reg_init_thread);

  return SNS_RC_SUCCESS == rc ? SNS_RC_SUCCESS : SNS_RC_POLICY;
}

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid registry_suid = {.sensor_uid = {REGISTRY_SUID}};
  return &registry_suid;
}

/**
 * Parse a read request message to get the group name.
 *
 * @param[i] req Incoming request message
 *
 * @return Parsed name (pointer into the request message)
 */
static char const*
get_read_req_name(pb_buffer_arg const *payload)
{
  sns_registry_read_req read_req = sns_registry_read_req_init_default;
  pb_buffer_arg data;
  pb_istream_t stream =
    pb_istream_from_buffer(payload->buf, payload->buf_len);

  read_req.name.funcs.decode = &pb_decode_string_cb;
  read_req.name.arg = &data;

  if(pb_decode(&stream, sns_registry_read_req_fields, &read_req))
    return (char*)data.buf;

  SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding read req: %s", PB_GET_ERROR(&stream));
  return NULL;
}

/**
 * Encode all registry items for the given registry group.
 */
static bool
encode_registry_items_cb(struct pb_ostream_s *stream,
    struct pb_field_s const *field, void *const *arg)
{
  sns_registry_group *group = (sns_registry_group*)*arg;
  sns_isafe_list_iter iter;

  for(sns_isafe_list_iter_init(&iter, &group->items, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_registry_item *item = (sns_registry_item*)
      sns_isafe_list_iter_get_curr_data(&iter);
    pb_buffer_arg str_data;
    pb_buffer_arg name_data = (pb_buffer_arg)
      { .buf = item->name, .buf_len = strlen(item->name) + 1 };
    sns_registry_data_item pb_item = sns_registry_data_item_init_default;

    pb_item.name.funcs.encode = &pb_encode_string_cb;
    pb_item.name.arg = &name_data;

    pb_item.has_version = true;
    pb_item.version = item->version;

    if(SNS_REGISTRY_TYPE_GROUP == item->type)
    {
      pb_item.has_subgroup = true;
      pb_item.subgroup.items.funcs.encode = &encode_registry_items_cb;
      pb_item.subgroup.items.arg = (void*)item->data.group;
    }
    else if(SNS_REGISTRY_TYPE_INTEGER == item->type)
    {
      pb_item.has_sint = true;
      pb_item.sint = item->data.sint;
    }
    else if(SNS_REGISTRY_TYPE_FLOAT == item->type)
    {
      pb_item.has_flt = true;
      pb_item.flt = item->data.flt;
    }
    else if(SNS_REGISTRY_TYPE_STRING == item->type)
    {
      str_data = (pb_buffer_arg)
        { .buf = item->data.str, .buf_len = strlen(item->data.str) + 1 };
      pb_item.str.funcs.encode = &pb_encode_string_cb;
      pb_item.str.arg = &str_data;
    }

    if(!pb_encode_tag_for_field(stream, field))
      return false;

    if(!pb_encode_submessage(stream, sns_registry_data_item_fields, &pb_item))
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding item: %s", PB_GET_ERROR(stream));
      return false;
    }
  }

  return true;
}

/**
 * Create and send a registry read event.
 *
 * @param[i] instance Client-specific instance who made the request
 * @param[i] name Full registry group name to read
 */
static void
handle_read_req(sns_sensor_instance *instance, char const *name)
{
  pb_buffer_arg name_data = (pb_buffer_arg)
    { .buf = name, .buf_len = strlen(name) + 1 };
  sns_registry_read_event read_event = sns_registry_read_event_init_default;
  sns_registry_group const *group = sns_registry_group_find(name);

  if(NULL == group)
  {
    char group_name[strlen(name) + 1];
    char *item_name;

    sns_strlcpy(group_name, name, sizeof(group_name));
    item_name = strrchr(group_name, '.');
    if(NULL != item_name)
      item_name[0] = '\0';

    group = sns_registry_group_find(group_name);
    // PEND: This code is intended to support finding a requested item rather
    //       than a group.  However we need to confirm that this group contains
    //       the originally requested item.
  }

  read_event.name.funcs.encode = &pb_encode_string_cb;
  read_event.name.arg = &name_data;

  if(NULL == group)
  {
    SNS_SPRINTF(HIGH, sns_fw_printf, "Group not found: %s", name);
  }
  else
  {
    read_event.data.items.funcs.encode = &encode_registry_items_cb;
    read_event.data.items.arg = (void*)group;
  }

  pb_send_event(instance, sns_registry_read_event_fields, &read_event,
      sns_get_system_time(), SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT,
      NULL);
}

/**
 * Parse a write request message to get the group name.
 *
 * @param[i] req Incoming request message
 *
 * @return Parsed name (pointer into the request message)
 */
static char const*
get_write_req_name(pb_buffer_arg const *payload)
{
  sns_registry_write_req write_req = sns_registry_write_req_init_default;
  pb_buffer_arg data;
  pb_istream_t stream =
    pb_istream_from_buffer(payload->buf, payload->buf_len);

  write_req.name.funcs.decode = &pb_decode_string_cb;
  write_req.name.arg = &data;

  if(pb_decode(&stream, sns_registry_write_req_fields, &write_req))
    return (char*)data.buf;

  SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding write req: %s", PB_GET_ERROR(&stream));
  return NULL;
}

/**
 * Decode callback for a single registry item from sns_registry_data.
 */
static bool
registry_item_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = false;
  pb_istream_t stream_cpy = *stream;
  pb_buffer_arg str_data = {0, 0}, name_data = {0, 0};
  sns_registry_group *group = (sns_registry_group*)*arg;
  sns_registry_data_item pb_item = sns_registry_data_item_init_default;

  pb_item.name.funcs.decode = &pb_decode_string_cb;
  pb_item.name.arg = &name_data;
  pb_item.str.funcs.decode = &pb_decode_string_cb;
  pb_item.str.arg = &str_data;

  if(!pb_decode(stream, sns_registry_data_item_fields, &pb_item))
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding item: %s", PB_GET_ERROR(stream));
  else if(0 == name_data.buf_len)
    SNS_PRINTF(ERROR, sns_fw_printf, "Missing item name");
  else
  {
    sns_registry_item item;
    bool failed = false;

    item.name = (char*)name_data.buf;
    item.version = pb_item.has_version ? pb_item.version : 0;

    if(pb_item.has_flt)
    {
      item.data.flt = pb_item.flt;
      item.type = SNS_REGISTRY_TYPE_FLOAT;
    }
    else if(pb_item.has_sint)
    {
      item.data.sint = pb_item.sint;
      item.type = SNS_REGISTRY_TYPE_INTEGER;
    }
    else if(0 != str_data.buf_len)
    {
      item.data.str = (char*)str_data.buf;
      item.type = SNS_REGISTRY_TYPE_STRING;
    }
    else if(0 != pb_item.has_subgroup)
    {
      sns_registry_group *subgroup;
      char full_name[strlen(item.name) + strlen(group->name) + 2];

      sns_strlcpy(full_name, group->name, sizeof(full_name));
      sns_strlcat(full_name, ".", sizeof(full_name));
      sns_strlcat(full_name, item.name, sizeof(full_name));

      subgroup = sns_registry_group_create(full_name);
      if(NULL == subgroup)
      {
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error creating subgroup: %s", full_name);
        failed = true;
      }
      else
      {
        sns_registry_data_item temp_item = sns_registry_data_item_init_default;
        temp_item.subgroup.items.funcs.decode = &registry_item_decode_cb;
        temp_item.subgroup.items.arg = subgroup;

        if(!pb_decode(&stream_cpy, sns_registry_data_item_fields, &temp_item))
        {
          SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding item: %s", PB_GET_ERROR(stream));
          failed = true;
          sns_registry_group_free(subgroup);
          subgroup = NULL;
        }

        item.data.group = subgroup;
        item.type = SNS_REGISTRY_TYPE_GROUP;
      }
    }
    else
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Unknown item type");
      failed = true;
    }

    if(!failed)
    {
      sns_registry_item const *new_item =
          sns_registry_item_create(group, &item);
      if(NULL == new_item)
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error creating new item: %s", item.name);
      else
        rv = true;
    }
  }

  return rv;
}

/**
 * Handle a registry write request
 *
 * @param[i] Parsed null-terminated registry group name
 * @param[i] Incoming request message
 *
 * @return
 *  SNS_RC_SUCCESS
 *  SNS_RC_INVALID_VALUE Unable to create registry group
 */
static sns_rc
handle_write_req(char const *name, pb_buffer_arg const *payload)
{
  sns_rc rv = SNS_RC_SUCCESS;
  sns_registry_group *group = sns_registry_group_create(name);

  if(NULL == group)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failure creating registry group");
    rv = SNS_RC_INVALID_VALUE;
  }
  else
  {
    sns_registry_write_req write_req = sns_registry_write_req_init_default;
    pb_istream_t stream =
      pb_istream_from_buffer(payload->buf, payload->buf_len);

    write_req.data.items.funcs.decode = &registry_item_decode_cb;
    write_req.data.items.arg = group;

    if(!pb_decode(&stream, sns_registry_write_req_fields, &write_req))
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding write req: %s", PB_GET_ERROR(&stream));
      rv = SNS_RC_FAILED;
    }
    else
    {
      sns_registry_group_merge(group);
    }
    sns_registry_group_free(group);
  }

  return rv;
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
  UNUSED_VAR(curr_req);
  sns_sensor_instance *inst =
    this->cb->get_sensor_instance(this, true);

  if(NULL == inst)
    inst = this->cb->create_instance(this, 0);

  if(!remove)
  {
    pb_istream_t stream = pb_istream_from_buffer(new_req->request, new_req->request_len);
    pb_buffer_arg payload = {0, 0};
    sns_std_request request = sns_std_request_init_default;

    request.payload.funcs.decode = &pb_decode_string_cb;
    request.payload.arg = &payload;

    if(pb_decode(&stream, sns_std_request_fields, &request))
    {
      inst->cb->add_client_request(inst, new_req);
      if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ == new_req->message_id)
      {
        char const *name = get_read_req_name(&payload);
        if(NULL == name)
          SNS_PRINTF(ERROR, sns_fw_printf, "Error parsing registry read request");
        else
          handle_read_req(inst, name);
      }
      else if(SNS_REGISTRY_MSGID_SNS_REGISTRY_WRITE_REQ == new_req->message_id)
      {
        char const *name = get_write_req_name(&payload);
        if(NULL == name)
          SNS_PRINTF(ERROR, sns_fw_printf, "Error parsing registry write request");
        else
          handle_write_req(name, &payload);
      }
    }
    else
      SNS_SPRINTF(ERROR, sns_fw_printf, "Error decoding std req: %s", PB_GET_ERROR(&stream));

    inst->cb->remove_client_request(inst, new_req);
  }

  if(NULL != inst &&
     NULL == inst->cb->get_client_request(inst, get_sensor_uid(this), true))
  {
    this->cb->remove_instance(inst);
  }

  return &sns_instance_no_error;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_registry_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &init,
  .deinit = NULL,
  .get_sensor_uid = &get_sensor_uid,
  .set_client_request = &set_client_request,
  .notify_event = &notify_event,
};
