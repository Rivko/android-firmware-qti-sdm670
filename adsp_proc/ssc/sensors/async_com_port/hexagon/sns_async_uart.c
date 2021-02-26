/*=============================================================================
  @file sns_async_uart.c

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
#include "sns_async_uart.pb.h"
#include "sns_std_sensor.pb.h"

#include "sns_async_com_port.h"
#include "sns_async_uart.h"
#include "sns_async_uart_instance.h"

#include "sns_assert.h"
#include "sns_attribute_util.h"
#include "sns_island.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_uid.h"
#include "sns_signal.h"
#include "sns_suid.pb.h"
#include "sns_types.h"
//#include "sns_diag_service.h"
//#include "sns_event_service.h"
//#include "sns_island_util.h"
//#include "sns_mem_util.h"
//#include "sns_profiler.h"
//#include "sns_request.h"
//#include "sns_sensor_instance.h"
//#include "sns_sensor_util.h"
//#include "sns_service.h"
//#include "sns_service_manager.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define ASYNC_UART_SUID \
  {  \
    .sensor_uid =  \
    {  \
      0x90, 0x41, 0x05, 0x65, 0xc9, 0x0c, 0x40, 0x77,  \
      0xbf, 0xc0, 0x29, 0xf6, 0x4a, 0x6b, 0x9c, 0x80  \
    }  \
  }

static const sns_sensor_uid sensor_uid SNS_SECTION(".rodata.sns")= ASYNC_UART_SUID;


/*=============================================================================
  Extern Variable Declarations
  ===========================================================================*/

extern sns_sensor_api async_com_port_api;


/*=============================================================================
  Static functions
  ===========================================================================*/
static void
sns_async_uart_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "async_uart";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "async_uart";
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
    char const proto_files[] = "async_uart.proto";
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
sns_async_uart_init(sns_sensor *const this)
{
  sns_rc rc = SNS_RC_FAILED;
  sns_async_uart_state *state = (sns_async_uart_state *)this->state->state;
  sns_sensor *sensor;

  // PEND: This logic should probably get abstracted out into its own method
  // Try to access the async com port's sensor state by searching for it.
  for(sensor = this->cb->get_library_sensor(this, true);
      NULL != sensor;
      sensor = this->cb->get_library_sensor(this, false))
  {
    if (sensor->sensor_api == &async_com_port_api)
    {
      // Piggyback off of async com port's signal thread
      sns_async_com_port_state *acp_state = (sns_async_com_port_state *)sensor->state->state;
      state->sig_thread = acp_state->sig_thread;
    }
  }

  // If the signal thread can't be found (or is uninitialized), crash the system
  // since this is unexpected behavior.
  SNS_ASSERT(state->sig_thread);

  rc = sns_signal_register_signal(state->sig_thread, this, &state->signal);
  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, this, "sns_async_uart_init - sns_signal_register_signal() failed");
  }
  else
  {
    sns_async_uart_publish_attributes(this);
  }
  SNS_PRINTF(HIGH, this, "sns_async_uart_init - finished init. signal 0x%x", state->signal);
  return rc;
}

static sns_rc
sns_async_uart_deinit(sns_sensor *const this)
{
  sns_async_uart_state *state = (sns_async_uart_state *)this->state->state;
  // PEND: This call will be problematic if the ACP is deinitialized before this
  // sensor.
  sns_signal_unregister_signal(state->sig_thread, state->signal);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_sensor_uid const*
sns_async_uart_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

SNS_SECTION(".text.sns") static sns_sensor_instance*
sns_async_uart_set_client_request(sns_sensor *const this,
                                 sns_request const *curr_req,
                                 sns_request const *new_req,
                                 bool remove)
{
  sns_rc rv;

  SNS_PRINTF(HIGH, this, "sns_async_uart_set_client_request %d", remove);

  // Find the sensor instance
  sns_sensor_instance *existing_inst = sns_sensor_util_find_instance(this,
    curr_req, sns_async_uart_get_sensor_uid(this));

  if(remove)
  {
    // Remove the client request from the sensor instance, if it exists
    if(NULL != existing_inst)
    {
      existing_inst->cb->remove_client_request(existing_inst, curr_req);
      // If there are no more associated client requests, then remove the sensor
      // instance.
      if(NULL == existing_inst->cb->get_client_request(existing_inst,
            sns_async_uart_get_sensor_uid(this), true))
      {
        this->cb->remove_instance(existing_inst);
      }
    }
    return existing_inst;
  }
  else
  {
    if(NULL == existing_inst)
    {
      // PEND: Reject config requests that attempt to reuse another client's
      // UART instance. In the future, we may be able to support the sharing of
      // async UART instances between clients, but this is not needed right now.
      sns_sensor_instance *new_async_uart_inst = this->cb->create_instance(this,
        sizeof(sns_async_uart_instance_state));
      if(NULL != new_async_uart_inst)
      {
        new_async_uart_inst->cb->add_client_request(new_async_uart_inst, new_req);
        rv = this->instance_api->set_client_config(new_async_uart_inst, new_req);
        if (rv != SNS_RC_SUCCESS)
        {
          SNS_PRINTF(ERROR, this, "Failed async UART set_client_config %d", rv);

          // Clean up
          this->cb->remove_instance(new_async_uart_inst);
          new_async_uart_inst = NULL;
        }
        else
        {
          SNS_PRINTF(HIGH, this, "Started a new async UART instance");
        }
      }
      else
      {
        SNS_PRINTF(ERROR, this, "Unable to create a new async UART instance");
      }
      return new_async_uart_inst;
    }
    else
    {
      // Peek at the message to see if it's a reconfiguration request
      if (new_req->message_id == SNS_ASYNC_UART_MSGID_SNS_ASYNC_UART_CONFIG)
      {
        // PEND: Support async UART instance reconfiguration. This likely will
        // require tearing down the instance and bringing it back up with the new
        // config.
        SNS_PRINTF(ERROR, this, "There's already an existing async UART config for this client.");
        return NULL;
      }
      existing_inst->cb->remove_client_request(existing_inst, curr_req);
      existing_inst->cb->add_client_request(existing_inst, new_req);
      this->instance_api->set_client_config(existing_inst, new_req);
      return existing_inst;
    }
  }
}

SNS_SECTION(".text.sns") static sns_rc
sns_async_uart_inst_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

sns_sensor_api async_uart_api SNS_SECTION(".data.sns")  =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_async_uart_init,
  .deinit = &sns_async_uart_deinit,
  .get_sensor_uid = &sns_async_uart_get_sensor_uid,
  .set_client_request = &sns_async_uart_set_client_request,
  .notify_event = &sns_async_uart_inst_notify_event,
};
