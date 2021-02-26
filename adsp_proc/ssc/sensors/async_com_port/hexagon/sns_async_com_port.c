/*=============================================================================
  @file sns_async_com_port.c

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <string.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port.h"
#include "sns_async_com_port.pb.h"
#include "sns_async_com_port_instance.h"
#include "sns_async_uart.h"
#include "sns_async_uart_instance.h"
#include "sns_attribute_util.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_signal.h"
#include "sns_suid.pb.h"
#include "sns_thread_prio.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define SNS_ASYNC_COM_PORT_ENABLE_DEBUG 0
#define ASYNC_COM_PORT_SUID \
  {  \
    .sensor_uid =  \
    {  \
      0xa9, 0xb0, 0x15, 0x29, 0x15, 0x0b, 0x67, 0x46,  \
      0xc5, 0x27, 0xd1, 0x0f, 0x91, 0x8c, 0x8a, 0x73  \
    }  \
  }

static const sns_sensor_uid sensor_uid SNS_SECTION(".rodata.sns")= ASYNC_COM_PORT_SUID;


/*=============================================================================
  Extern functions
  ===========================================================================*/

extern sns_rc
sns_async_com_port_inst_generate_event(sns_sensor_instance *const async_com_port_inst);

extern sns_rc
sns_async_uart_inst_generate_event(sns_sensor_instance *const this);

/*=============================================================================
  Static functions
  ===========================================================================*/

SNS_SECTION(".text.sns") static void sns_async_com_port_send_error(sns_sensor_instance* const this, sns_rc error)
{
  sns_async_com_port_error async_com_port_event = sns_async_com_port_error_init_default;

  async_com_port_event.error_code = (sns_std_error)error;
  pb_send_event( this, sns_async_com_port_error_fields, &async_com_port_event,
                 sns_get_system_time(), SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_ERROR,
                 NULL );
}

// see sns_handle_signal_event
SNS_SECTION(".text.sns") static void sns_async_com_port_process_signal_events(sns_sensor *const this, uint32_t signal, void const * args)
{
  SNS_PROFILE(SNS_ASCP_HANDLE_SIGNAL_EVENT_START, 0);
  sns_sensor_instance *inst;
  sns_async_com_port_state* state = (sns_async_com_port_state*) this->state->state;

  // Check the signal to see if it's for async_com_port or async_uart
  if (signal == state->signal)
  {
    // Handle the async_com_port signal
    for(inst = this->cb->get_sensor_instance(this, true);
        NULL != inst;
        inst = this->cb->get_sensor_instance(this, false))
    {
      sns_async_com_port_instance_state *inst_state =
        (sns_async_com_port_instance_state*)inst->state->state;
      if(inst_state->last_rw_timestamp != inst_state->last_rw_event_timestamp)
      {
        sns_async_com_port_inst_generate_event(inst);
      }
    }
  }
  // Must be a signal for async_uart
  else
  {
    // Handle the async_uart signal
    sns_sensor * async_uart_sensor = (sns_sensor *) args;
    for (inst = async_uart_sensor->cb->get_sensor_instance(async_uart_sensor, true);
        NULL != inst;
        inst = async_uart_sensor->cb->get_sensor_instance(async_uart_sensor, false))
    {
      // Generate the instance's event
      sns_async_uart_inst_generate_event(inst);
    }
  }
  SNS_PROFILE(SNS_ASCP_HANDLE_SIGNAL_EVENT_END, 0);
}

static void
sns_async_com_port_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "async_com_port";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "async_com_port";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = "qti_qualcomm";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = "async_com_port.proto";
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
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

static sns_rc
sns_async_com_port_init(sns_sensor *const this)
{
  sns_rc rc = SNS_RC_FAILED;
  sns_async_com_port_state *state = (sns_async_com_port_state *)this->state->state;
  state->sig_thread = sns_signal_thread_create(this, "sns_sig_ascp", SNS_ASCP_THREAD_PRIO,
    sns_async_com_port_process_signal_events, NULL, true);
  rc = sns_signal_register_signal(state->sig_thread, NULL, &state->signal);
  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, this, "sns_async_com_port_init - sns_signal_register_signal() failed");
  }
  else
  {
    sns_async_com_port_publish_attributes(this);
  }
  return rc;
}

static sns_rc
sns_async_com_port_deinit(sns_sensor *const this)
{
  sns_async_com_port_state *state = (sns_async_com_port_state *)this->state->state;
  // PEND: Make sure async UART isn't trying to use this thread before destroying it
  sns_signal_thread_destroy(state->sig_thread);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_sensor_uid const*
sns_async_com_port_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

SNS_SECTION(".text.sns") static sns_sensor_instance*
sns_async_com_port_set_client_request(sns_sensor *const this,
                                 sns_request const *curr_req,
                                 sns_request const *new_req,
                                 bool remove)
{
  if(remove)
  {
    sns_sensor_instance *inst = sns_sensor_util_find_instance(this, curr_req,
      sns_async_com_port_get_sensor_uid(this));
    if(NULL != inst)
    {
      inst->cb->remove_client_request(inst, curr_req);
      if(NULL == inst->cb->get_client_request(inst,
            sns_async_com_port_get_sensor_uid(this), true))
      {
        this->cb->remove_instance(inst);
      }
    }
    return inst;
  }
  else
  {
    sns_sensor_instance *existing_inst = sns_sensor_util_find_instance(this,
      curr_req, sns_async_com_port_get_sensor_uid(this));
    if(NULL == existing_inst)
    {
      sns_sensor_instance *new_async_com_port_inst =
        this->cb->create_instance(this, sizeof(sns_async_com_port_instance_state));
      // New request -- must be a COM_PORT_CONFIG
      if(NULL != new_async_com_port_inst)
      {
        new_async_com_port_inst->cb->add_client_request(new_async_com_port_inst, new_req);
        if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG == new_req->message_id &&
           SNS_RC_SUCCESS == this->instance_api->set_client_config(new_async_com_port_inst, new_req))
        {
          if(!pb_send_event(new_async_com_port_inst, NULL, NULL,sns_get_system_time(),
                            SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_REG, NULL))
          {
            SNS_PRINTF(ERROR, this, "Failed to send ASCP registered event");
          }
        }
        else
        {
          SNS_PRINTF(ERROR, this, "set client config failed. msg_id %u", new_req->message_id);
          sns_async_com_port_send_error(new_async_com_port_inst, SNS_RC_INVALID_VALUE);
          new_async_com_port_inst->cb->remove_client_request(new_async_com_port_inst, new_req);
          this->cb->remove_instance(new_async_com_port_inst);
          new_async_com_port_inst = NULL;
        }
      }
      return new_async_com_port_inst;
    }
    else
    {
      if(SNS_ASYNC_COM_PORT_MSGID_SNS_ASYNC_COM_PORT_CONFIG != new_req->message_id)
      {
        this->instance_api->set_client_config(existing_inst, new_req);
      }
      else
      {
        SNS_PRINTF(ERROR, this, "Trying to send a COM_PORT_CONFIG on an already configured stream.");
        sns_async_com_port_send_error(existing_inst, SNS_RC_INVALID_VALUE);
      }
      return existing_inst;
    }
  }
}

SNS_SECTION(".text.sns") static sns_rc
sns_async_com_port_inst_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

sns_sensor_api async_com_port_api SNS_SECTION(".data.sns")  =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_async_com_port_init,
  .deinit = &sns_async_com_port_deinit,
  .get_sensor_uid = &sns_async_com_port_get_sensor_uid,
  .set_client_request = &sns_async_com_port_set_client_request,
  .notify_event = &sns_async_com_port_inst_notify_event,
};

