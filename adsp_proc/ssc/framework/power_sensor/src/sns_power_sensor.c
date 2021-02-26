/*=============================================================================
  @file sns_power_sensor.c

  Copyright (c) 2017 Qualcomm Technologies, Inc.
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
#include "sns_fw_gpio_service.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_power_sensor.h"
#include "sns_printf_int.h"
#include "sns_registry.pb.h"
#include "sns_registry_util.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_types.h"

/*=============================================================================
 * Static Data Definitions
 ============================================================================*/
#define POWER_SENSOR_SUID 0xd7,0x58,0xbe,0xef,0x23,0xb8,0x47,0x73,\
    0xad,0x4c,0xd3,0x24,0x28,0x56,0x08,0xe7

/* Registry group names of power configurations */
char* power_sensor_registry_groups[] = {"power.gpio.gpio_0",
                                       "power.gpio.gpio_1",
                                       "power.gpio.gpio_2",
                                       "power.gpio.gpio_3",
                                       "power.gpio.gpio_4",
                                       "power.gpio.gpio_5",
                                       "power.gpio.gpio_6",
                                       "power.gpio.gpio_7",
                                       "power.gpio.gpio_8",
                                       "power.gpio.gpio_9",
                                       "power.gpio.gpio_10",
                                       "power.gpio.gpio_11",
                                       "power.gpio.gpio_12",
                                       "power.gpio.gpio_13",
                                       "power.gpio.gpio_14",
                                       "power.gpio.gpio_15",
                                       "power.gpio.gpio_16",
                                       "power.gpio.gpio_17",
                                       "power.gpio.gpio_18",
                                       "power.gpio.gpio_19",
                                       "power.island"
                                      };

/*=============================================================================
 * Static Function Definitions
 ============================================================================*/

/**
 * Publish all Sensor attributes
 *
 * @param[i] this    reference to this Sensor
 */
static void
power_sensor_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "power_sensor";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "power_sensor";
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

static void
sns_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = false;
  sns_publish_attribute(this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/* See sns_sensor::init */
static sns_rc
power_sensor_init(sns_sensor *const this)
{
  sns_power_sensor_state *state = (sns_power_sensor_state*)this->state->state;

  power_sensor_publish_attributes(this);

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");

  return SNS_RC_SUCCESS;
}

/**
 * Parse the GPIO config values from the registry group for power
 *
 * @param[i] reg_item      registry item
 * @param[i] item_name     name of the item
 * @param[i] item_str_val  item string value
 * @param[o] parsed_buffer parsed output buffer
 *
 * @return true  if item found and parsed successfully
 *         false otherwise
 */
static bool
power_sensor_parse_gpio_config(sns_registry_data_item *reg_item,
    pb_buffer_arg *item_name, pb_buffer_arg *item_str_val,
    void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_sint)
  {
    sns_gpio_config *cfg = (sns_gpio_config*)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf,
                     "gpio_num",
                     item_name->buf_len))
    {
      cfg->gpio_num = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "active_fs",
                         item_name->buf_len))
    {
      cfg->active_fs = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "active_dir",
                         item_name->buf_len))
    {
      cfg->active_dir = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "active_pull",
                         item_name->buf_len))
    {
      cfg->active_pull = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "active_drive",
                         item_name->buf_len))
    {
      cfg->active_drive = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sleep_fs",
                         item_name->buf_len))
    {
      cfg->sleep_fs = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sleep_dir",
                         item_name->buf_len))
    {
      cfg->sleep_dir = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sleep_pull",
                         item_name->buf_len))
    {
      cfg->sleep_pull = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf,
                         "sleep_drive",
                         item_name->buf_len))
    {
      cfg->sleep_drive = reg_item->sint;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**
 * Parse the island config value from the registry group for power
 *
 * @param[i] reg_item      registry item
 * @param[i] item_name     name of the item
 * @param[i] item_str_val  item string value
 * @param[o] parsed_buffer parsed output buffer
 *
 * @return true  if item found and parsed successfully
 *         false otherwise
 */
static bool
power_sensor_parse_enable_island(sns_registry_data_item *reg_item,
    pb_buffer_arg *item_name, pb_buffer_arg *item_str_val,
    void *parsed_buffer)
{
  bool rv = true;

  if(NULL == reg_item || NULL == item_name || NULL == item_str_val ||
     NULL == parsed_buffer)
  {
    rv = false;
  }
  else if(reg_item->has_sint)
  {
    bool *enable_island = (bool *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf,
                     "enable_island",
                     item_name->buf_len))
    {
      *enable_island = (reg_item->sint == 1) ? true : false;
    }
  }
  else
  {
    rv = false;
  }

  return rv;
}

/**
 * Process events from the registry sensor with registry info of the power sensor groups
 *
 * @param[i] this    reference to this Sensor
 * @param[i] event   event pointer received from the registry
 */
static void
power_sensor_process_registry_event(sns_sensor *const this, sns_sensor_event *event)
{
  sns_gpio_config default_gpio_config, current_gpio_config;
  sns_memzero(&default_gpio_config, sizeof(default_gpio_config));
  sns_memzero(&default_gpio_config, sizeof(current_gpio_config));
  bool enable_island = true;
  bool rv = false;

  pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
      event->event_len);

  if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
  {
    sns_registry_read_event read_event = sns_registry_read_event_init_default;
    pb_buffer_arg group_name = {0,0};
    read_event.name.arg = &group_name;
    read_event.name.funcs.decode = pb_decode_string_cb;

    if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
    {
      SNS_SPRINTF(ERROR, this, "Error decoding registry event: %s", PB_GET_ERROR(&stream));
    }
    else
    {
      stream = pb_istream_from_buffer((void*)event->event, event->event_len);
      sns_memzero(&default_gpio_config, sizeof(default_gpio_config));
      sns_memzero(&current_gpio_config, sizeof(current_gpio_config));

      for(uint32_t i = 0; i < ARR_SIZE(power_sensor_registry_groups); i++)
      {
        if(0 == sns_memcmp((char*)group_name.buf, power_sensor_registry_groups[i],
                          group_name.buf_len))
        {
          if(0 == sns_memcmp((char*)group_name.buf, "power.island", group_name.buf_len))
          {
            sns_registry_decode_arg arg = {
              .item_group_name = &group_name,
              .parse_info_len = 1,
              .parse_info[0] = {
                .parse_func = power_sensor_parse_enable_island,
                .parsed_buffer = &enable_island
              }
            };
            sns_strlcpy(arg.parse_info[0].group_name, group_name.buf, group_name.buf_len);
            read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
            read_event.data.items.arg = &arg;
          }
          else
          {
            sns_registry_decode_arg arg = {
              .item_group_name = &group_name,
              .parse_info_len = 1,
              .parse_info[0] = {
                .parse_func = power_sensor_parse_gpio_config,
                .parsed_buffer = &current_gpio_config
              }
            };
            sns_strlcpy(arg.parse_info[0].group_name, group_name.buf, group_name.buf_len);
            read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
            read_event.data.items.arg = &arg;
          }
          rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
        }
      }
      if(rv && 0 == sns_memcmp((char*)group_name.buf, "power.island", group_name.buf_len))
      {
        SNS_SPRINTF(HIGH, this, "Parsed Group: %s Island Mode Config:%d",
            (char*)group_name.buf, enable_island);
        if(!enable_island)
        {
          sns_island_disable();
          sns_island_configure_island_enable(enable_island);
        }
      }
      else if(rv && 0 != sns_memcmp(&current_gpio_config, &default_gpio_config, sizeof(current_gpio_config)))
      {
        SNS_SPRINTF(HIGH, this, "Parsed Group: %s", (char*)group_name.buf);
        sns_gpio_service_store_gpio_config(&current_gpio_config);
      }
      else
      {
        SNS_SPRINTF(HIGH, this, "Received unsupported registry group %s",
                                 (char*)group_name.buf);
      }
    }
    if(!rv)
    {
      SNS_SPRINTF(ERROR, this, "Error decoding registry group %s due to %s",
          (char*)group_name.buf, PB_GET_ERROR(&stream));
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Received unsupported registry event msg id %u",
        event->message_id);
  }
}

/**
 * Send a request to the registry sensor for the provided power registry group
 *
 * @param[i] this             reference to this Sensor
 * @param[i] reg_group_name   registry power group name being requested
 */
static void
power_sensor_send_registry_request(sns_sensor *const this,
    char *reg_group_name)
{
  sns_power_sensor_state *state = (sns_power_sensor_state*)this->state->state;
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_memset(buffer, 0, sizeof(buffer));
  sns_rc rc = SNS_RC_SUCCESS;

  sns_registry_read_req read_request;
  pb_buffer_arg data = (pb_buffer_arg){ .buf = reg_group_name,
    .buf_len = (strlen(reg_group_name) + 1) };

  read_request.name.arg = &data;
  read_request.name.funcs.encode = pb_encode_string_cb;

  encoded_len = pb_encode_request(buffer, sizeof(buffer),
      &read_request, sns_registry_read_req_fields, NULL);
  if(0 < encoded_len)
  {
    sns_request request = (sns_request){
      .request_len = encoded_len, .request = buffer,
      .message_id = SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_REQ };
    rc = state->reg_stream->api->send_request(state->reg_stream, &request);
  }
  if(SNS_RC_SUCCESS == rc)
  {
    SNS_SPRINTF(LOW, this, "Sending registry request for group name:%s", reg_group_name);
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Error sending registry request:%d", rc);
  }
}

/* See sns_sensor::notify_event */
sns_rc power_sensor_notify_event(sns_sensor *const this)
{
  sns_power_sensor_state *state = (sns_power_sensor_state*)this->state->state;
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

    sns_publish_available(this);
    if(NULL == state->reg_stream)
    {
      stream_service->api->create_sensor_stream(stream_service,
          this, suid, &state->reg_stream);

      for(int i = 0; i < ARR_SIZE(power_sensor_registry_groups); i++)
      {
        power_sensor_send_registry_request(this, power_sensor_registry_groups[i]);
      }
    }
  }

  if(NULL != state->reg_stream)
  {
    sns_sensor_event *event = state->reg_stream->api->peek_input(state->reg_stream);

    while(NULL != event)
    {
      power_sensor_process_registry_event(this, event);
      event = state->reg_stream->api->get_next_input(state->reg_stream);
    }
  }

  return SNS_RC_SUCCESS;
}

/* See sns_sensor::get_sensor_uid */
static sns_sensor_uid const*
power_sensor_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid power_sensor_suid = {.sensor_uid = {POWER_SENSOR_SUID}};
  return &power_sensor_suid;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api sns_power_sensor_api =
{
    .struct_len = sizeof(sns_sensor_api),
    .init = &power_sensor_init,
    .deinit = NULL,
    .get_sensor_uid = &power_sensor_get_sensor_uid,
    .set_client_request = NULL,
    .notify_event = &power_sensor_notify_event,
};
