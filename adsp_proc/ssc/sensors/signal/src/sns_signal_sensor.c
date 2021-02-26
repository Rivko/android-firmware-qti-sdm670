/*=============================================================================
  @file sns_signal_sensor.h

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stddef.h>
#include <string.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_attribute_util.h"
#include "sns_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_signal_sensor.h"
#include "sns_signal_sensor.pb.h"
#include "sns_signal_sensor_api.h"
#include "sns_signal_sensor_instance.h"
#include "sns_stream_service.h"
#include "sns_thread_prio.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

#define SNS_SIG_SENSR_STACK_SIZE 2*1024

/*=============================================================================
  Static data
  ===========================================================================*/
  static const sns_sensor_uid sensor_uid SNS_SECTION(".rodata.sns")= SIGNAL_SENSOR_UID;
  static uint8_t sns_sig_sensor_stack[SNS_SIG_SENSR_STACK_SIZE] SNS_SECTION(".data.sns");

  /*=============================================================================
  Static functions
  ===========================================================================*/

/**
 * sns_signal_sensor_internal_task
 *
 * The task loop for the signal_sensor task.
 */
SNS_SECTION(".text.sns") static void sns_signal_sensor_internal_task(void * args)
{
  uint32_t sigs;
  sns_fw_sensor *signal_sensor = (sns_fw_sensor *)args;
  sns_signal_state* signal_sensor_state =
    (sns_signal_state *)signal_sensor->sensor.state->state;
  uint32_t current_signal;
  sns_osa_thread_active();

  sns_service_manager *manager =
    signal_sensor_state->manager;
  sns_event_service *event_service =
    (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);

  while(1)
  {
    sigs = 0;
    sns_osa_thread_idle();
    sns_osa_thread_sigs_wait(0xFFFFFFFF, &sigs);
    sns_osa_thread_active();
    sns_osa_lock_acquire(signal_sensor->library->library_lock);

    current_signal = 0x1;
    for( uint8_t i = 0; (i < SNS_SIGNAL_SENSOR_MAX_SIGNALS) && (0 != sigs);
      i++, current_signal = current_signal << 1)
    {
      SNS_PROFILE(SNS_SIGNAL_SENSOR_HANDLE_SIGNAL_START,1, current_signal);
      if((sigs & current_signal) && signal_sensor_state->registered_signals[i].enabled)
      {
        if (!sns_island_is_island_ptr((intptr_t)signal_sensor_state->registered_signals[i].instance))
        {
          SNS_ISLAND_EXIT();
        }
        sns_sensor_event *event_out =
          event_service->api->alloc_event( event_service,
          signal_sensor_state->registered_signals[i].instance, 0 );
        SNS_ASSERT(NULL != event_out);
        event_out->event_len = 0;
        event_out->message_id = SNS_SIGNAL_SENSOR_MSGID_SNS_SIGNAL_SENSOR_NOTIFY;
        event_out->timestamp = sns_get_system_time();
        SNS_PROFILE(SNS_SIGNAL_SENSOR_SEND_EVENT_START, 0);
        event_service->api->publish_event(event_service,
          signal_sensor_state->registered_signals[i].instance, event_out, NULL);
        sigs ^= current_signal;
        SNS_PROFILE(SNS_SIGNAL_SENSOR_SEND_EVENT_END, 0);
      }
      SNS_PROFILE(SNS_SIGNAL_SENSOR_HANDLE_SIGNAL_END, 0);
    }
    sns_osa_lock_release(signal_sensor->library->library_lock);
  }
}

/**
 * Publish all saved attributes for Signal.
 */
static void
sns_signal_sensor_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "Signal Sensor";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "internal_system_signal";
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
    char const proto_files[] = "sns_signal_sensor.proto";
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
sns_signal_sensor_init(sns_sensor *const this)
{
  uint8_t i;
  sns_signal_state *state = (sns_signal_state *)this->state->state;
  sns_osa_thread_attr attr;
  sns_memzero(state, sizeof(sns_signal_state));

  for(i = 0; i < SNS_SIGNAL_SENSOR_MAX_SIGNALS; i++)
  {
    state->registered_signals[i].bitshift = i;
    state->registered_signals[i].enabled = false;
    state->registered_signals[i].instance = NULL;
  }
  state->manager = this->cb->get_service_manager(this);

  sns_osa_thread_attr_init(&attr);
  sns_osa_thread_attr_set_name(&attr, "SIG_SENSR");
  sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  sns_osa_thread_attr_set_stack(&attr, (intptr_t)sns_sig_sensor_stack, SNS_SIG_SENSR_STACK_SIZE);
  sns_osa_thread_attr_set_priority(&attr, SNS_SIG_SENSR_THREAD_PRIO);

  //WARNING: Spawning a thread from a sensor is not supported. The signal sensor
  // is the only platform sensor that is allowed to have it's own thread.
  sns_osa_thread_create(sns_signal_sensor_internal_task, this, &attr, &state->thread_id);

  SNS_PRINTF(LOW, this, "Signal Sensor started");
  sns_signal_sensor_publish_attributes(this);

  return SNS_RC_SUCCESS;
}

static sns_rc
sns_signal_sensor_deinit(sns_sensor *const this)
{
  UNUSED_VAR(this);
  //NA
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_sensor_uid const*
sns_signal_sensor_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

SNS_SECTION(".text.sns") static sns_sensor_instance*
sns_signal_set_client_request(sns_sensor *const this,
                              sns_request const *encoded_curr_req,
                              sns_request const *encoded_new_req,
                              bool remove)
{
  uint8_t assigned_bitshift = 0xFF;
  sns_signal_state *state =
    (sns_signal_state*)this->state->state;
//  signal_sensor_config *new_config = (signal_sensor_config *)new_req->request;
  pb_istream_t stream;
  sns_signal_sensor_req new_request = sns_signal_sensor_req_init_default;
  sns_signal_sensor_req curr_req = sns_signal_sensor_req_init_default;
  sns_std_request sr = sns_std_request_init_default;
  pb_simple_cb_arg arg =
    { .decoded_struct = &new_request, .fields = sns_signal_sensor_req_fields };
  sr.payload = (struct pb_callback_s)
    { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };

  if(NULL != encoded_new_req)
  {
    stream = pb_istream_from_buffer(encoded_new_req->request, encoded_new_req->request_len);
    if(!pb_decode(&stream, sns_std_request_fields, &sr))
    {
      return NULL;
    }
  }

  if(NULL != encoded_curr_req)
  {
    arg.decoded_struct = &curr_req;
    stream = pb_istream_from_buffer(encoded_curr_req->request, encoded_curr_req->request_len);
    if(!pb_decode(&stream, sns_std_request_fields, &sr))
    {
      return NULL;
    }
  }

  if(remove)
  {
    sns_sensor_instance *inst = sns_sensor_util_find_instance(this, encoded_curr_req,
        this->sensor_api->get_sensor_uid(this));
    uint8_t i;
    for(i = 0; i < SNS_SIGNAL_SENSOR_MAX_SIGNALS; i++)
    {
      if(state->registered_signals[i].instance == inst)
      {
        state->registered_signals[i].enabled = false;
        state->registered_signals[i].instance = NULL;
        break;
      }
    }
    SNS_PRINTF(LOW, this, "Unset signal for flag %x", (0x1 << i));
    if(NULL != inst)
    {
      inst->cb->remove_client_request(inst, encoded_curr_req);
      if(NULL == inst->cb->get_client_request(
            inst, sns_signal_sensor_get_sensor_uid(this), true))
      {
        this->cb->remove_instance(inst);
      }
    }
    return NULL;
  }
  else
  {
    sns_sensor_instance *existing_inst =
      sns_sensor_util_find_instance(this, encoded_curr_req,
          this->sensor_api->get_sensor_uid(this));
    uint8_t i;

    if(NULL == existing_inst)
    {
      sns_sensor_instance *new_signal_inst =
        this->cb->create_instance(this, sizeof(sns_signal_instance_state));

      if(NULL != new_signal_inst)
      {
        sns_request temp_sensor_req = {.message_id = SNS_SIGNAL_SENSOR_MSGID_SNS_SIGNAL_SENSOR_REQ,
          .request = &new_request, .request_len = sizeof(new_request)};

        new_signal_inst->cb->add_client_request(new_signal_inst, encoded_new_req);
        bool sig_assigned = false;
        for(i = 0; i < SNS_SIGNAL_SENSOR_MAX_SIGNALS; i++)
        {
          if(NULL == state->registered_signals[i].instance)
          {
            state->registered_signals[i].enabled = new_request.enable;
            state->registered_signals[i].instance = new_signal_inst;
            sig_assigned = true;
            assigned_bitshift = i;
            break;
          }
        }
        SNS_ASSERT(true == sig_assigned);
        this->instance_api->set_client_config(new_signal_inst, &temp_sensor_req);

        SNS_PROFILE(SNS_SIGNAL_SENSOR_SEND_FLAG_START, 0);
        sns_service_manager *manager =
          this->cb->get_service_manager(this);
        sns_event_service *event_service =
          (sns_event_service*)manager->get_service(manager, SNS_EVENT_SERVICE);
        sns_sensor_event *event_out =
          event_service->api->alloc_event( event_service, new_signal_inst,
                                           sns_signal_sensor_token_size);

        if ( NULL != event_out )
        {
          pb_ostream_t stream =
            pb_ostream_from_buffer((pb_byte_t*)event_out->event, event_out->event_len);
          sns_signal_sensor_token token_event;
          token_event.flag = ((uint32_t)0x1 << assigned_bitshift);
          token_event.thread = (uintptr_t)state->thread_id;
          if(pb_encode(&stream, sns_signal_sensor_token_fields, &token_event))
          {
            event_out->event_len = stream.bytes_written;
            event_out->timestamp = sns_get_system_time();
            event_out->message_id = SNS_SIGNAL_SENSOR_MSGID_SNS_SIGNAL_SENSOR_TOKEN;
            event_service->api->publish_event(event_service, new_signal_inst, event_out, NULL);
          }
          else
          {
            SNS_PRINTF(FATAL, this, "Failed to encode token event for signal %x",
              ((uint32_t)0x1 << assigned_bitshift) );
          }
        }
        SNS_PROFILE(SNS_SIGNAL_SENSOR_SEND_FLAG_END, 0);
      }
      return new_signal_inst;
    }
    else
    {
      for(i = 0; i < SNS_SIGNAL_SENSOR_MAX_SIGNALS; i++)
      {
        if(existing_inst == state->registered_signals[i].instance)
        {
          state->registered_signals[i].enabled = new_request.enable;
          break;
        }
      }
      return existing_inst;
    }
  }
}

SNS_SECTION(".text.sns") static sns_rc
sns_signal_inst_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

sns_sensor_api signal_sensor_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_signal_sensor_init,
  .deinit = &sns_signal_sensor_deinit,
  .get_sensor_uid = &sns_signal_sensor_get_sensor_uid,
  .set_client_request = &sns_signal_set_client_request,
  .notify_event = &sns_signal_inst_notify_event
};
