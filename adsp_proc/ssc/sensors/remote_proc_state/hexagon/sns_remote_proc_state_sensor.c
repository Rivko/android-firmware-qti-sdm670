/*=============================================================================
  @file sns_remote_proc_state_sensor.c

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "smem_type.h"
#include "smp2p.h"
#include "sns_attribute_util.h"
#include "sns_fw_sensor.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_remote_proc_state.pb.h"
#include "sns_remote_proc_state_sensor.h"
#include "sns_remote_proc_state_sensor_instance.h"
#include "sns_std.pb.h"
#include "sns_suid.pb.h"
#include "sns_thread_prio.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/
/*=============================================================================
  Type definitions
  ===========================================================================*/

/*=============================================================================
  Globals
  ===========================================================================*/
static const sns_sensor_uid sensor_uid = REMOTE_PROC_STATE_SUID;

/*=============================================================================
  Static functions
  ===========================================================================*/

/**
 * Find remote processor state instance that has the same processor type
 * as the provided processor type of the new request
 *
 * @param[i] sensor      remote processor state sensor
 * @param[i] proc_type   remote processor type
 */
static sns_sensor_instance*
sns_remote_proc_state_match_instance(sns_sensor *const sensor, sns_std_client_processor proc_type)
{
  sns_sensor_instance *instance;

  for(instance = sensor->cb->get_sensor_instance(sensor, true);
      NULL != instance;
      instance = sensor->cb->get_sensor_instance(sensor, false))
  {
    sns_remote_proc_state_inst_state *inst_state =
      (sns_remote_proc_state_inst_state *)instance->state->state;
    if(inst_state->inst_proc_type == proc_type)
    {
      return instance;
    }
  }
  return NULL;
}

/**
 * Let SEE know that this sensor is now available for clients.
 *
 * @param[i] this  remote processor state sensor
 */
static void
sns_remote_proc_state_publish_available(sns_sensor *const this)
{
  sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
  value.has_boolean = true;
  value.boolean = true;
  sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, true);
}

/**
 * Main function for the SMP2P task. Function registers with the SMP2P service
 * and waits for SMP2P notifications. Signals an event when an SMP2P notification
 * is received.
 *
 * @param[i] args      signal thread pointer
 */
void
sns_remote_proc_state_task(void *args)
{
  sns_osa_thread_active();
  sns_signal_thread *sig_thread = (sns_signal_thread *)args;
  smem_host_type source_id = SMEM_APPS;
  char *entry_name = "sleepstate";
  smp2p_context_type *smp2p_context = NULL;
  uint32 smp2p_data = 0;
  sns_remote_proc_state_state *state = (sns_remote_proc_state_state *)(sig_thread->sensor->state->state);
  sns_rc rc = SNS_RC_FAILED;
  sns_std_client_processor proc_type = SNS_STD_CLIENT_PROCESSOR_APSS;
  sns_island_client_handle island_client = sns_island_aggregator_register_client("Remote_proc_state");
  rc = smp2p_register(&smp2p_context, source_id, entry_name);

  if(SMP2P_STATUS_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, sig_thread->sensor, "Error: SMP2P registration failed with error:%d", rc);
  }
  else
  {
    sns_remote_proc_state_publish_available(sig_thread->sensor);
    while(sig_thread->active)
    {
      sns_island_unblock(island_client);
      sns_osa_thread_idle();
      rc = smp2p_wait(smp2p_context);
      sns_osa_thread_active();
      sns_island_block(island_client);

      if(SMP2P_STATUS_SUCCESS != rc)
      {
        SNS_PRINTF(ERROR, sig_thread->sensor, "Error: Wait for SMP2P event failed with error:%d", rc);
        continue;
      }

      rc = smp2p_read(smp2p_context, &smp2p_data);

      if(SMP2P_STATUS_SUCCESS == rc)
      {
         sns_osa_lock_acquire(((sns_fw_sensor *)sig_thread->sensor)->library->library_lock);

         SNS_PROFILE(SNS_REMOTE_PROC_STATE_SMP2P_DATA_RECEIVED, 1, smp2p_data);
         SNS_PRINTF(MED, sig_thread->sensor, "SMP2P event received with data: %d", smp2p_data);
         if(smp2p_data & SMP2P_AWAKE)
         {
           state->ap_proc_state = SNS_REMOTE_PROC_STATE_AWAKE;
         }
         else
         {
           state->ap_proc_state = SNS_REMOTE_PROC_STATE_SUSPEND;
         }

         sig_thread->cb(sig_thread->sensor, 0, &proc_type);

         sns_osa_lock_release(((sns_fw_sensor *)sig_thread->sensor)->library->library_lock);
      }
      else
      {
        SNS_PRINTF(ERROR, sig_thread->sensor, "Error: SMP2P read failed with error:%d", rc);
      }
    }
  }
  sns_osa_thread_idle();
}

/**
 * Generate event to notify the clients of a remote processor state change
 *
 * @param[i] instance      remote processor state sensor instance
 * @param[i] sensor        remote processor state sensor
 *
 * @return
 * SNS_RC_SUCCESS         if event sent successfully
 * SNS_RC_FAILED          otherwise
 */
static sns_rc
sns_remote_proc_state_inst_generate_event(sns_sensor_instance *const instance,
                                          sns_sensor *const sensor)
{
  sns_remote_proc_state_state *state = (sns_remote_proc_state_state *)sensor->state->state;
  sns_remote_proc_state_event remote_proc_state_event = sns_remote_proc_state_event_init_default;
  sns_remote_proc_state_inst_state *inst_state =
    (sns_remote_proc_state_inst_state *)instance->state->state;
  sns_rc rc = SNS_RC_SUCCESS;

  remote_proc_state_event.proc_type = inst_state->inst_proc_type;

  if(SNS_STD_CLIENT_PROCESSOR_APSS == remote_proc_state_event.proc_type)
  {
    remote_proc_state_event.event_type = state->ap_proc_state;
  }

  SNS_PROFILE(SNS_REMOTE_PROC_STATE_GENERATE_EVENT, 1, remote_proc_state_event.event_type);

  if(!pb_send_event(instance, sns_remote_proc_state_event_fields, &remote_proc_state_event,
                sns_get_system_time(), SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_EVENT,
                NULL))
  {
    SNS_PRINTF(ERROR, sensor, "Failed to send Remote Proc State event for instance %p", instance);
    rc = SNS_RC_FAILED;
  }
  return rc;
}

/**
 * Handles signal event from remote processor state task on
 * remote processor state change
 *
 * @param[i] this          remote processor state sensor
 * @param[i] args          remote processor type
 */
static void
sns_remote_proc_state_process_signal_events(sns_sensor *this, uint32_t signal, void const *args)
{
  UNUSED_VAR(signal);
  sns_std_client_processor *proc_type = (sns_std_client_processor *)args;
  sns_sensor_instance *inst = NULL;

  SNS_PROFILE(SNS_REMOTE_PROC_STATE_PROCESS_SIGNAL_EVENT, 1, *proc_type);
  SNS_PRINTF(LOW, this, "Received signal event");

  inst = sns_remote_proc_state_match_instance(this, *proc_type);

  if(NULL != inst)
  {
    sns_remote_proc_state_inst_generate_event(inst, this);
  }
}

/**
 * Publish all saved attributes for SMP2P.
 */
static void
sns_remote_proc_state_publish_attributes(sns_sensor *const this)
{
  {
    char const name[] = "Remote Processor State";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = "remote_proc_state";
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
    char const proto_files[] = "sns_remote_proc_state.proto";
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = proto_files, .buf_len = sizeof(proto_files) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_API, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = 1;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint = SNS_STD_SENSOR_STREAM_TYPE_ON_CHANGE;
    sns_publish_attribute(
      this, SNS_STD_SENSOR_ATTRID_STREAM_TYPE, &value, 1, true);
  }
}

/* See sns_sensor_api::init */
static sns_rc
sns_remote_proc_state_init(sns_sensor *const this)
{
  sns_rc rc = SNS_RC_FAILED;
  sns_remote_proc_state_state *state = (sns_remote_proc_state_state *)this->state->state;

  sns_memzero(state, sizeof(sns_remote_proc_state_state));
  state->ap_proc_state = SNS_REMOTE_PROC_STATE_AWAKE;

  sns_remote_proc_state_publish_attributes(this);

  state->ro_buffer.sig_thread = sns_signal_thread_create(this, "sns_sig_remote_proc_state",
                                                         SNS_REMOTE_PROC_STATE_THREAD_PRIO,
                                                         sns_remote_proc_state_process_signal_events,
                                                         sns_remote_proc_state_task, false);
  rc = sns_signal_register_signal(state->ro_buffer.sig_thread, NULL, &state->ro_buffer.signal);

  return rc;
}

/* See sns_sensor_api::deinit */
static sns_rc
sns_remote_proc_state_deinit(sns_sensor *const this)
{
  sns_remote_proc_state_state *state = (sns_remote_proc_state_state *)this->state->state;
  sns_signal_thread_destroy(state->ro_buffer.sig_thread);
  return SNS_RC_SUCCESS;
}

/* See sns_sensor_api::get_sensor_uid */
sns_sensor_uid const*
sns_remote_proc_state_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

/* See sns_sensor_api::set_client_request */
static sns_sensor_instance*
sns_remote_proc_state_set_client_request(sns_sensor *const this,
                             sns_request const *curr_req,
                             sns_request const *new_req,
                             bool remove)

{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_sensor_instance *match_inst = NULL, *rv_inst = NULL;
  sns_sensor_instance *curr_inst = sns_sensor_util_find_instance( this, curr_req,
                                     this->sensor_api->get_sensor_uid(this));

  if(remove)
  {
    if(NULL != curr_inst)
    {
      curr_inst->cb->remove_client_request(curr_inst, curr_req);

      if(NULL == curr_inst->cb->get_client_request(
          curr_inst, sns_remote_proc_state_get_sensor_uid(this), true))
      {
        this->cb->remove_instance(curr_inst);
      }
    }
  }
  else
  {
    pb_istream_t stream;
    sns_remote_proc_state_config config = {0};
    sns_std_request request = sns_std_request_init_default;

    pb_simple_cb_arg arg =
      { .decoded_struct = &config, .fields = sns_remote_proc_state_config_fields };
    request.payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
    stream = pb_istream_from_buffer(new_req->request, new_req->request_len);

    if(pb_decode(&stream, sns_std_request_fields, &request))
    {
      sns_request decoded_req = {.message_id = new_req->message_id,
        .request_len = sizeof(config), .request = &config };
      match_inst = sns_remote_proc_state_match_instance(this, config.proc_type);

      if(NULL == curr_inst)
      {
        // If this is a request from a new client
        if(NULL == match_inst)
        {
          rv_inst = this->cb->create_instance( this,
            sizeof(sns_remote_proc_state_inst_state));

          SNS_PRINTF(LOW, this, "Created remote processor state sensor instance");

          if(NULL != rv_inst)
          {
            rc = this->instance_api->set_client_config(rv_inst, &decoded_req);
            if(SNS_RC_SUCCESS != rc)
            {
              SNS_PRINTF(ERROR, this, "Set client config returned error code:%d", rc);
              this->cb->remove_instance(rv_inst);
              rv_inst = NULL;
            }
          }
        }
        else
        {
          rv_inst = match_inst;
        }
      }
      else
      {
        curr_inst->cb->remove_client_request(curr_inst, curr_req);
        rv_inst = curr_inst;
        this->instance_api->set_client_config(rv_inst, &decoded_req);
      }

      if(NULL != rv_inst)
      {
        rv_inst->cb->add_client_request(rv_inst, new_req);
        sns_remote_proc_state_inst_generate_event(rv_inst, this);
      }
    }
  }
  return rv_inst;
}

/* See sns_sensor_api::notify_event */
static sns_rc
sns_remote_proc_state_inst_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

sns_sensor_api remote_proc_state_sensor_api =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_remote_proc_state_init,
  .deinit = &sns_remote_proc_state_deinit,
  .get_sensor_uid = &sns_remote_proc_state_get_sensor_uid,
  .set_client_request = &sns_remote_proc_state_set_client_request,
  .notify_event = &sns_remote_proc_state_inst_notify_event,
};
