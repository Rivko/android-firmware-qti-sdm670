/*=============================================================================
  @file sns_reg_util.c

  A simple utility to parse incoming registry data.  Intended for use by
  Sensors; is a replacement for/wrapper around decoding registry events
  manually by the Sensor directly.

  The Sensor developer should create a registry data stream normally.
  Within notify_event, sns_reg_event_handle should be called each time.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "sns_data_stream.h"
#include "sns_mem_util.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_reg_util.h"
#include "sns_registry.pb.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* Decode a registry item */
static bool
registry_item_decode_cb(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_reg_util_table *table = (sns_reg_util_table*)*arg;
  pb_buffer_arg name = { 0, 0 }, str = { 0, 0 };
  sns_registry_data_item pb_item = sns_registry_data_item_init_default;

  pb_item.name.funcs.decode = &pb_decode_string_cb;
  pb_item.name.arg = &name;
  pb_item.str.funcs.decode = &pb_decode_string_cb;
  pb_item.str.arg = &str;

  if(pb_decode(stream, sns_registry_data_item_fields, &pb_item))
  {
    for(int i = 0; i < table->items_len; i++)
    {
      if(0 == strcmp((char*)name.buf, table->items[i].name))
      {
        if(pb_item.has_sint && SNS_REG_TYPE_INT == table->items[i].type)
          sns_memscpy(table->items[i].data, sizeof(pb_item.sint), &pb_item.sint, sizeof(pb_item.sint));
        else if(pb_item.has_flt && SNS_REG_TYPE_FLT == table->items[i].type)
          sns_memscpy(table->items[i].data, sizeof(pb_item.sint), &pb_item.flt, sizeof(pb_item.flt));
        else if(table->items[i].type < 0)
          SNS_PRINTF(ERROR, sns_fw_printf, "Mismatch item type %s", table->items[i].name);
        else if(NULL != str.buf)
          sns_memscpy(table->items[i].data, table->items[i].type, str.buf, str.buf_len);

        break;
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Error decoding registry item");
  }

  return true;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc
sns_reg_util_handle(struct sns_data_stream *stream,
    sns_reg_util_table *tables, uint32_t tables_len)
{
  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  sns_sensor_event *event = stream->api->peek_input(stream);

  while(NULL != event)
  {
    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
      pb_istream_t stream =
        pb_istream_from_buffer((void*)event->event, event->event_len);
      sns_registry_read_event read_event = sns_registry_read_event_init_default;
      pb_buffer_arg group_name = {0,0};
      bool table_found = false;

      read_event.name.arg = &group_name;
      read_event.name.funcs.decode = pb_decode_string_cb;

      if(pb_decode(&stream, sns_registry_read_event_fields, &read_event))
      {
        for(int i = 0; i < tables_len; i++)
        {
          if(0 == strcmp(tables[i].name, group_name.buf))
          {
            stream = pb_istream_from_buffer((void*)event->event, event->event_len);
            read_event.data.items.arg = &tables[i];
            read_event.data.items.funcs.decode = registry_item_decode_cb;

            table_found = true;
            tables[i].group_recv = true;

            if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
              SNS_PRINTF(ERROR, sns_fw_printf, "Error decoding registry event");
          }
        }
      }
      else
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Error decoding registry event");
      }

      if(!table_found)
        SNS_SPRINTF(MED, sns_fw_printf, "Unknown group %s", group_name.buf);

      rv = SNS_RC_SUCCESS;
    }
    else if(SNS_STD_MSGID_SNS_STD_ERROR_EVENT == event->message_id)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Error event received %d", event->message_id);
      while(NULL != stream->api->get_next_input(stream));
      rv = SNS_RC_INVALID_STATE;
    }
    else
    {
      SNS_PRINTF(MED, sns_fw_printf, "Unknown event received %d", event->message_id);
    }
    event = stream->api->get_next_input(stream);
  }
  return rv;
}
