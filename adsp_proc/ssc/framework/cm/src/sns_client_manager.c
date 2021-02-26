/*=============================================================================
  @file sns_client_manager.c

  Incoming requests to the Framework will arrive via the QMI Client Manager.
  From the Framework perspective, this module functions just as any other
  Sensor would, implementing and using the same Sensors API.  This module
  will translate incoming QMI request messages into Client Request objects,
  and similarly will translate Sensor Events into QMI indications.  This
  translation will be trivial, and this module will only concern itself with
  the requested batch period and the client processor.  The configuration
  request will be passed along to the appropriate Sensor, and this module
  will batch all data received (as appropriate).

  @note All QMI callbacks will be received within the same thread, and hence
  some thread protection is not necessary.  The CM Library mutex must be
  acquired prior to the internal QMI mutex(s).

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_data_stream.h"
#include "sns_fw_attribute_service.h"
#include "sns_register.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"

#include "sns_assert.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_instance.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_timer.h"
#include "sns_types.h"

#include "sns_attribute_util.h"
#include "sns_client.pb.h"
#include "sns_client_api_v01.h"
#include "sns_client_manager.h"
#include "sns_client_manager_batch_timer.h"
#include "sns_client_manager_flush_manager.h"
#include "sns_client_manager_instance.h"
#include "sns_client_manager_qmi.h"
#include "sns_fw_event_service.h"
#include "sns_fw_log.h"
#include "sns_memmgr.h"
#include "sns_profiler.h"
#include "sns_remote_proc_state.pb.h"
#include "sns_sensor_util.h"
#include "sns_suid.pb.h"
#include "sns_suid_util.h"
#include "sns_thread_prio.h"

#if defined(SSC_TARGET_HEXAGON)
#include "qurt.h"
#include "qurt_anysignal.h"
#else
#include <stdio.h>
#endif
#include "sns_pwr_sleep_mgr.h"
/*=============================================================================
  Macros & Definitions
  ===========================================================================*/


/*=============================================================================
  Static Data
  ===========================================================================*/
#if defined(SSC_TARGET_HEXAGON)

static uint8_t cm_task_heap[ 4096 ];
static const uint32_t cm_thread_sig = ( SNS_CM_THREAD_QURT_SIGNAL |
    SNS_CM_THREAD_TIMER_SIGNAL|
    SNS_CM_THREAD_FLUSH_TO_DDR_SIGNAL|
    SNS_CM_THREAD_LOW_MEM_THRSH1_SIGNAL|
    SNS_CM_THREAD_LOW_MEM_THRSH2_SIGNAL);
#endif

extern sns_sensor_instance_api cm_sensor_instance_api;

/* Registry group names of power configurations */
char* cm_registry_groups[] SNS_SECTION(".rodata.island") = {"sns_cm"};

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

void sns_cm_set_latency_vote(
    sns_sensor *this,
    sns_std_client_processor proc_type)
{
  if(sns_cm_is_island_blocked(this, proc_type))
  {
    /* Always vote for latency of 5 ms */
    SNS_PRINTF(LOW, this, "Set latency vote for proc type %d",proc_type);
    sns_pwr_sleep_mgr_set_latency(SNS_CM_APPS_AWAKE_LATENCY_USEC);
  }
  else
  {
    /* Release the latency on apps suspend */
    SNS_PRINTF(LOW, this, "Remove latency vote for proc type %d",proc_type);
    sns_pwr_sleep_mgr_release_latency();
  }
}

/*
 * In the event of low batch memory, this function sends out all
 * accumulated data.
 *
 * @param[i] cm_sensor The framework sensor pointer for CM sensor.
 */
void cm_handle_low_batch_memory(
    sns_fw_sensor *cm_sensor,
    bool drop_events)
{
  SNS_PROFILE(SNS_CM_HNDL_LOW_BATHC_MEM_FUNC, 2, 1, drop_events);
  sns_sensor *this = (sns_sensor*)cm_sensor;
  sns_cm_state *state = (sns_cm_state*)this->state->state;
  sns_list_iter iter;

  SNS_PRINTF(MED, this, "Handle low batch memory is triggered");

  for(sns_list_iter_init(&iter, &state->proc_type_list, true);
      NULL != sns_list_iter_curr(&iter);
      sns_list_iter_advance(&iter))
  {
    cm_proc_info *proc_info = (cm_proc_info*)
      sns_list_iter_get_curr_data(&iter);
    SNS_ISLAND_EXIT();
    sns_cm_send_all_data_for_proc(cm_sensor, proc_info->proc_type, drop_events);
    if(sns_event_service_low_mem_active())
    {
      if(!drop_events)
      {
#if defined(SSC_TARGET_HEXAGON)
        qurt_anysignal_set((void *)state->os_params.signal, SNS_CM_THREAD_LOW_MEM_THRSH1_SIGNAL);
#endif
      }
      else
      {
#if defined(SSC_TARGET_HEXAGON)
        qurt_anysignal_set((void *)state->os_params.signal, SNS_CM_THREAD_LOW_MEM_THRSH2_SIGNAL);
#endif
      }
      SNS_PROFILE(SNS_CM_HNDL_LOW_BATHC_MEM_FUNC, 2, 2, drop_events);
      break;
    }
  }
  SNS_PROFILE(SNS_CM_HNDL_LOW_BATHC_MEM_FUNC, 2, 2, drop_events);
}

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/
#if defined(SSC_TARGET_HEXAGON)
/**
 * Callback used by mem heap  to indicate the first low memory
 * threshold hit on the batch heap.
 * This function tries to send events without dropping events.
 * param[i] arg  The CM sensor pointer
 */
  static void
sns_client_manager_batch_heap_send(intptr_t arg)
{
  SNS_PROFILE(SNS_CM_LOW_MEM_THRSH1_SIG, 1, 1);
  SNS_PRINTF(MED, sns_fw_printf, "Handle low batch heap memory send");
  qurt_anysignal_set((void *)arg, SNS_CM_THREAD_LOW_MEM_THRSH1_SIGNAL);
}
/**
 * Callback used by mem heap  to indicate the second high memory
 * threshold hit on the batch heap.
 * This function tries to send if it can , else drops events.
 * param[i] arg  The CM sensor pointer
 */
  static void
sns_client_manager_batch_heap_drop(intptr_t arg)
{
  SNS_PROFILE(SNS_CM_LOW_MEM_THRSH2_SIG, 1, 1);
  SNS_PRINTF(HIGH, sns_fw_printf, "Handle high batch heap memory try send or drop");
  qurt_anysignal_set((void *)arg, SNS_CM_THREAD_LOW_MEM_THRSH2_SIGNAL);
}

/**
 * Processes expired timer events from the timer thread.
 *
 * @param[i] sensor The CM sensor pointer.
 */
static void cm_handle_timer_signal(
    sns_fw_sensor *sensor)
{
  sns_cm_state *cm_state = (sns_cm_state*)sensor->sensor.state->state;

  sns_osa_lock_acquire(cm_state->expired_timer_events_lock);
  for(int i = 0; i < ARR_SIZE(cm_state->expired_timer_events); i++)
  {
    if(cm_state->expired_timer_events[i].active)
    {
      cm_state->expired_timer_events[i].active = false;
      cm_timer_event timer_event = cm_state->expired_timer_events[i];
      sns_osa_lock_release(cm_state->expired_timer_events_lock);
      sns_cm_handle_timer_event(sensor, &timer_event);
      sns_osa_lock_acquire(cm_state->expired_timer_events_lock);
    }
  }
  sns_osa_lock_release(cm_state->expired_timer_events_lock);
}

/**
 * Main task loop for the QMI Client Manager.
 */
static void sns_cm_task(
    void *arg)
{
  sns_fw_sensor *sensor = (sns_fw_sensor*)arg;
  sns_sensor *this=(sns_sensor*)sensor;
  sns_cm_state *state = (sns_cm_state*)sensor->sensor.state->state;
  state->island_client = sns_island_aggregator_register_client("SNS_CM");
  state->scpm_client = sns_pwr_scpm_mgr_create_client();
  sns_osa_thread_active();

  SNS_PRINTF(HIGH, this, "SNS_CM Thread started");
  sns_time start_time = 0, prev_start_time = 0, total_time = 0, active_time = 0;
  sns_time start_active_time = sns_get_system_time();
  sns_time thread_util_sampling_period =
    sns_convert_ns_to_ticks(SNS_THREAD_UTIL_SAMPLING_PERIOD_NS);

  for(;;)
  {
    qmi_csi_error qmi_err;
    uint32_t sigs_rcvd = 0;

    prev_start_time = start_time;
    start_time = sns_get_system_time();
    if(start_active_time != 0)
    {
      total_time += (start_time - prev_start_time);
      active_time += (start_time - start_active_time);
      if(total_time > thread_util_sampling_period)
      {
        uint32_t thread_utilization = (uint16_t)((active_time * 100) /
            total_time);
        sns_pwr_scpm_mgr_update_thread_utilization(state->thread, thread_utilization);
        total_time = 0;
        active_time = 0;
      }
    }

    sns_island_unblock(state->island_client);
    sns_osa_thread_idle();
    qurt_anysignal_wait(state->os_params.signal, cm_thread_sig);
    sigs_rcvd = qurt_anysignal_get(state->os_params.signal);
    qurt_anysignal_clear(state->os_params.signal, sigs_rcvd);
    sns_osa_thread_active();
    start_active_time = sns_get_system_time();

    sns_island_block(state->island_client);
#if ENABLE_DEBUG_PRINT
    SNS_PRINTF(LOW, this, "Sigs received %i", sigs_rcvd);
#endif
    if(sigs_rcvd & SNS_CM_THREAD_QURT_SIGNAL)
    {
      SNS_PROFILE(SNS_CM_QURT_SIG, 1, 1);
      sns_osa_lock_acquire(sensor->library->library_lock);
      qmi_err = qmi_csi_handle_event(state->service_provider, &state->os_params);
      sns_osa_lock_release(sensor->library->library_lock);
      if(QMI_CSI_NO_ERR != qmi_err)
      {
        SNS_PRINTF(ERROR, this, "qmi_csi_handle_event error %i", qmi_err);
      }
      SNS_PROFILE(SNS_CM_QURT_SIG, 1, 2);
    }
    if(sigs_rcvd & SNS_CM_THREAD_TIMER_SIGNAL)
    {
      SNS_PROFILE(SNS_CM_BATCH_TIMER_SIG, 1, 2);
      sns_osa_lock_acquire(sensor->library->library_lock);
      cm_handle_timer_signal(sensor);
      sns_osa_lock_release(sensor->library->library_lock);
    }
    if (sigs_rcvd & SNS_CM_THREAD_LOW_MEM_THRSH1_SIGNAL)
    {
      SNS_PROFILE(SNS_CM_LOW_MEM_THRSH1_SIG, 1, 2);
      sns_osa_lock_acquire(sensor->library->library_lock);
      SNS_ISLAND_EXIT();
      sns_pwr_scpm_mgr_mcps_bump_up(state->scpm_client);
      cm_handle_low_batch_memory(sensor,false);
      sns_pwr_scpm_mgr_mcps_bump_down(state->scpm_client);
      sns_osa_lock_release(sensor->library->library_lock);
    }
    if (sigs_rcvd & SNS_CM_THREAD_LOW_MEM_THRSH2_SIGNAL)
    {
      SNS_PROFILE(SNS_CM_LOW_MEM_THRSH2_SIG, 1, 2);
      sns_osa_lock_acquire(sensor->library->library_lock);
      SNS_ISLAND_EXIT();
      sns_pwr_scpm_mgr_mcps_bump_up(state->scpm_client);
      cm_handle_low_batch_memory(sensor,true);
      sns_pwr_scpm_mgr_mcps_bump_down(state->scpm_client);
      sns_osa_lock_release(sensor->library->library_lock);
    }
  }
}
/**
 * Set the QMI Operating System Parameters.
 *
 * PEND: Pull these functions out into separate files.
 */
  static void
cm_set_os_params(sns_cm_state *state)
{
  qurt_signal_t *signal = sns_malloc(SNS_HEAP_MAIN, sizeof(*signal));
  SNS_ASSERT(NULL != signal);

  qurt_anysignal_init(signal);
  state->os_params.signal = signal;
  state->os_params.sig = (cm_thread_sig & SNS_CM_THREAD_QURT_SIGNAL);
}
#else
static void
cm_set_os_params(sns_cm_state *state){ UNUSED_VAR(state); }
#endif

bool sns_cm_is_island_blocked(
    sns_sensor *this,
    sns_std_client_processor proc_type)
{
  SNS_PROFILE(SNS_CM_DECIDE_ISLAND_FUNC, 1, 1);
  sns_sensor_instance *cm_sensor_inst = NULL;
  sns_fw_sensor *sensor = (sns_fw_sensor *)this;
  bool island_block_flag = false;
  cm_proc_info *proc_info = sns_cm_get_proc_info(this, proc_type);
  sns_isafe_list_iter instance_iter;


  SNS_ASSERT(NULL != proc_info);
  sns_isafe_list_iter_init(&instance_iter, &sensor->sensor_instances, true);
  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
	  cm_sensor_inst = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
	  if(NULL != cm_sensor_inst)
    {
      sns_cm_inst_state *cm_inst = (sns_cm_inst_state*)cm_sensor_inst->state->state;
      sns_isafe_list_iter req_iter;

      for(sns_isafe_list_iter_init(&req_iter,&cm_inst->req_list,true);
          NULL != sns_isafe_list_iter_curr(&req_iter);
          sns_isafe_list_iter_advance(&req_iter))
      {
        sns_cm_request *cm_req = (sns_cm_request*)
          sns_isafe_list_iter_get_curr_data(&req_iter);
        if((cm_req->is_streaming_sensor) &&
            (cm_req->req_msg.susp_config.client_proc_type == proc_type) &&
            (cm_req->req_msg.msg_id > SNS_CM_MAX_FRAMEWORK_RESERVED_REQ_ID))
        {
          //If any streaming request is present and wakeup client
          //we always want to block island.
          if(cm_req->req_msg.susp_config.delivery_type == SNS_CLIENT_DELIVERY_WAKEUP)
          {
            island_block_flag = true;
            break;
          }
          //If any streaming request is present and non-wakeup client but processor is
          //awake we want island block.
          if(cm_req->req_msg.susp_config.delivery_type == SNS_CLIENT_DELIVERY_NO_WAKEUP)
          {
            //Awake
            if(!proc_info->is_suspended)
            {
              island_block_flag = true;
              break;
            }
          }
        }
      }
	  }
    if(island_block_flag)
      break;
    sns_isafe_list_iter_advance(&instance_iter);
  }
  if(island_block_flag)
  {
    sns_island_block(proc_info->island_client);
#if defined(SSC_TARGET_HEXAGON)
    sns_pwr_sleep_mgr_set_island_mode_enabled(false);
#endif
    SNS_PRINTF(HIGH,this,"Blocking island because island block flag %d, proc type %d proc_suspend %d",
        island_block_flag, proc_type, proc_info->is_suspended);
  }
  // Unblock island mode if we're not already in island mode
  else if (SNS_ISLAND_STATE_IN_ISLAND != sns_island_get_island_state())
  {
    SNS_PRINTF(HIGH,this,"UN-Blocking island because island block flag %d, proc type %d proc_suspend %d",
        island_block_flag, proc_type, proc_info->is_suspended);
    sns_island_unblock(proc_info->island_client);
#if defined(SSC_TARGET_HEXAGON)
    sns_pwr_sleep_mgr_set_island_mode_enabled(true);
#endif
  }
  SNS_PROFILE(SNS_CM_DECIDE_ISLAND_FUNC, 1, 2);
  return island_block_flag;
}

/*=============================================================================
  Static Sensor API Function Definitions
  ===========================================================================*/
  static void
cm_switch_non_wkup_streaming_req_to_batching_req(
    sns_data_stream *data_stream,
    sns_cm_request * cm_req,
    bool enable_max_batch)
{
  //Only change non-wakeup streaming requests.
  //Non-wakeup batching requests always operate in max batch mode
  if(SNS_CLIENT_DELIVERY_WAKEUP != cm_req->req_msg.susp_config.delivery_type &&
      (!cm_req->req_msg.request.has_batching ||
       (cm_req->req_msg.request.has_batching &&
        0 == cm_req->req_msg.request.batching.batch_period)))
  {
    uint8_t buffer[200];

    if(NULL != cm_req)
    {

      sns_std_request decoded_req = cm_req->req_msg.request;

      //Always add the batching spec
      if(!decoded_req.has_batching)
      {
        decoded_req.has_batching = true;
      }
      //Decide max batch based on the max batch flag
      decoded_req.batching.has_max_batch = true;
      decoded_req.batching.max_batch = enable_max_batch;

      pb_buffer_arg arg ;
      if(0 != cm_req->sensor_payload_len)
      {
        arg.buf = cm_req->sensor_payload;
        arg.buf_len =  cm_req->sensor_payload_len;
      }
      else
      {
        arg.buf = NULL;
        arg.buf_len =  0;
      }

      decoded_req.payload.funcs.encode = &pb_encode_string_cb;
      decoded_req.payload.arg = &arg;
      pb_ostream_t stream_o = pb_ostream_from_buffer((pb_byte_t *)buffer, sizeof(buffer));

      //If payload was empty then reset the payload encode to prevent encoding null bytes.
      if(0 == arg.buf_len)
      {
        arg.buf = NULL;
        arg.buf_len = 0;
      }
      if(!pb_encode(&stream_o, sns_std_request_fields, &decoded_req))
      {
        SNS_SPRINTF(ERROR, sns_fw_printf, "Error Encoding request: %s", PB_GET_ERROR(&stream_o));
      }
      else
      {
        size_t encoded_len = stream_o.bytes_written;
        //Send the modified request
        if(0 < encoded_len)
        {
          sns_request request = (sns_request){
            .request_len = encoded_len,
              .request = buffer,
              .message_id = cm_req->req_msg.msg_id };
          data_stream->api->send_request(data_stream, &request);
        }
      }
    }
  }
}

/**
 * Changes the requests on all sensor streams for
 * max batching or end max batching.
 *
 * @param[i] this                The sensor pointer.
 * @param[i] proc_in_suspend     The current proccessor state.
 * @param[i] enable_max_batch    The max batch flag to set.
 * @param[i] proc_type           The client processor type.
 *
 */
static void cm_req_max_batch(
    sns_sensor *this,
    bool proc_in_suspend,
    bool enable_max_batch,
    sns_std_client_processor proc_type)
{
  sns_sensor_instance *cm_sensor_inst = NULL;
  sns_fw_sensor *sensor = (sns_fw_sensor*)this;
  sns_cm_state *state = (sns_cm_state *)this->state->state;
  sns_isafe_list_iter instance_iter;

  cm_proc_info *apss_proc_info =
    sns_cm_get_proc_info((sns_sensor*)sensor, proc_type);
  SNS_ASSERT(NULL != apss_proc_info);

  sns_isafe_list_iter_init(&instance_iter, &sensor->sensor_instances, true);
  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
    cm_sensor_inst = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
    if(NULL != cm_sensor_inst)
    {

      sns_cm_inst_state *cm_inst_state = (sns_cm_inst_state*)cm_sensor_inst->state->state;
      //Switch out of max batching
      sns_isafe_list_iter req_iter;

      for(sns_isafe_list_iter_init(&req_iter, &cm_inst_state->req_list, true);
          NULL!=sns_isafe_list_iter_curr(&req_iter);
          sns_isafe_list_iter_advance(&req_iter))
      {
        sns_cm_request *cm_request = (sns_cm_request*)sns_isafe_list_iter_get_curr_data(&req_iter);
        if(proc_type == cm_request->req_msg.susp_config.client_proc_type &&
           NULL != cm_request->data_stream)
        {
          //We always update the processor suspend state here.
          cm_request->proc_susp_state = proc_in_suspend;
          if(!(state->registry_config.max_batch_disabled))
          {
            cm_switch_non_wkup_streaming_req_to_batching_req(cm_request->data_stream,cm_request,enable_max_batch);
          }
        }
      }

      //Flush all the requests for the sensor instance
      //if APPS is awake and we have at least 1 non wakeup client
      if(!enable_max_batch && 0 < apss_proc_info->non_wk_cnt)
      {
        sns_cm_send_flush(cm_sensor_inst, SNS_STD_CLIENT_PROCESSOR_APSS);
      }
    }
    sns_isafe_list_iter_advance(&instance_iter);
  }
}

  SNS_SECTION(".text.island")
cm_proc_info* sns_cm_get_proc_info(
    sns_sensor *this,
    sns_std_client_processor proc_type)
{
  sns_cm_state *cm_state = (sns_cm_state*)this->state->state;
  sns_list_iter iter;
  cm_proc_info * proc_info = NULL;
  for(sns_list_iter_init(&iter, &cm_state->proc_type_list, true);
      NULL != sns_list_iter_curr(&iter);
      sns_list_iter_advance(&iter))
  {
    cm_proc_info * proc_info_temp = (cm_proc_info*)
      sns_list_iter_get_curr_data(&iter);
    if(proc_info_temp->proc_type == proc_type)
    {
      proc_info = proc_info_temp;
#if ENABLE_DEBUG_PRINT
      SNS_PRINTF(HIGH,this,"Found a proc info struct for proc type %d",
          proc_info->proc_type);
#endif
      break;
    }
  }
  return proc_info;
}

/**
 * Process APPS transition into and out of suspend state.
 * Also updates the processor suspend state for each request
 *
 * @param[i] prev_in_suspend The previous APPS state.
 * @param[i] curr_in_suspend The previous APPS state.
 * @param[i] this            The CM sensor pointer.
 *
 */
SNS_SECTION(".text.island")
static void __attribute__ ((noinline)) cm_process_proc_transition(
    bool prev_in_suspend,
    bool curr_in_suspend,
    sns_sensor *this,
    sns_std_client_processor proc_type)
{
  SNS_PROFILE(SNS_CM_HANDLE_AP_TRANSITION, 3, 1, prev_in_suspend, curr_in_suspend);
  sns_cm_state *cm_state = (sns_cm_state*)this->state->state;
  cm_proc_info *proc_info = sns_cm_get_proc_info(this, proc_type);

  if(NULL == proc_info)
  {
    SNS_PRINTF(ERROR, this,
        "Processor info structure for %d is NULL",proc_type);
    return;
  }
  //Suspend -> Apps awake
  if(prev_in_suspend && !curr_in_suspend)
  {
    SNS_PRINTF(LOW, this, "Suspend to awake transition processing");
    //Decide Island State and set latency vote
    sns_cm_set_latency_vote(this, proc_type);

    sns_cm_send_all_data_for_proc((sns_fw_sensor *)this, proc_type, false);

    cm_req_max_batch(this,curr_in_suspend, false, proc_type);
  }
  //Awake->Suspend
  else if(!prev_in_suspend && curr_in_suspend)
  {

    //Decide Island State
    if(!sns_cm_is_island_blocked(
          this, proc_type))
    {
      /* Release the latency on apps suspend */
      sns_pwr_sleep_mgr_release_latency();
    }
    cm_req_max_batch(this,curr_in_suspend,true, proc_type);
  }
  else
  {
    SNS_PRINTF(ERROR, this,
        "No PM state transition.Should have never happened.");
    return;
  }

  //1. Cancel current timer
  //2. clear timer event list
  //3. and re-register timers after evaluating.

  sns_osa_lock_acquire(cm_state->expired_timer_events_lock);
  cm_state->expired_timer_events[proc_type].active = false;
  sns_osa_lock_release(cm_state->expired_timer_events_lock);
  
  if(NULL != proc_info->system_timer)
  {
    sns_time time_elp = 0;
    sns_cm_stop_batch_timer(cm_state,proc_info,&time_elp);
    sns_cm_configure_batch_timer((sns_fw_sensor *)this,proc_info, time_elp, NULL);
    if(sns_cm_register_batch_timer(cm_state, proc_info, this))
    {
      SNS_PRINTF(LOW, this, "Registered a timer successfully for proc \'%d\'", proc_info->proc_type);
    }
    else
    {
      //SNS_PRINTF(LOW, this, "Did not register timer for proc \'%d\'", proc_info->proc_type);
    }
  }
  SNS_PROFILE(SNS_CM_HANDLE_AP_TRANSITION, 3, 2, prev_in_suspend, curr_in_suspend);
}

  static void __attribute__ ((noinline))
cm_handle_single_remote_proc_event(sns_sensor *this, sns_sensor_event *event_in)
{
  bool prev_proc_susp_state = false;
  sns_std_client_processor proc_type = SNS_STD_CLIENT_PROCESSOR_APSS;
  cm_proc_info *proc_info = NULL;

  if(SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_EVENT == event_in->message_id)
  {
    pb_istream_t stream =
      pb_istream_from_buffer((pb_byte_t*)event_in->event, event_in->event_len);

    sns_remote_proc_state_event remote_proc_state_data =
      sns_remote_proc_state_event_init_default;

    if(!pb_decode(&stream, sns_remote_proc_state_event_fields,
          &remote_proc_state_data))
    {
      SNS_PRINTF(ERROR, this, "Error decoding Remote proc state sensor event");
    }
    else
    {
      proc_type = remote_proc_state_data.proc_type;
      sns_remote_proc_state_event_type event_type =
        remote_proc_state_data.event_type;
      proc_info = sns_cm_get_proc_info(this, proc_type);

      if(NULL == proc_info)
      {
        SNS_PRINTF(ERROR,this,"Proc info for proc %d is NULL",proc_type);
        return;
      }
      //Only store the previous state for apps processor
      SNS_PRINTF(LOW, this, "Got an event for proc %d event %d", proc_type);
      prev_proc_susp_state = proc_info->is_suspended;
      if(SNS_REMOTE_PROC_STATE_SUSPEND == event_type)
      {
        proc_info->is_suspended = true;
      }
      else
      {
        proc_info->is_suspended = false;
      }
    }
  }
  else
  {
    SNS_PRINTF(ERROR, this, "CM Sensor received an event "
        "from remote processor which it does not understand");
  }

  //Do any calculation only if valid remote proc event.
  if((NULL != proc_info) &&
      (prev_proc_susp_state != proc_info->is_suspended))
  {
    cm_process_proc_transition(prev_proc_susp_state,
        proc_info->is_suspended,
        this, proc_type);
  }
  else
  {
    SNS_PRINTF(ERROR, this, "Got an invalid proc type %d",proc_type);
  }
}
/**
 * Handles the events from the remote proc stream.
 *
 * @param[i] this The CM sensor pointer.
 */
static void __attribute__ ((noinline)) cm_handle_remote_proc_event (
    sns_sensor *this)
{
  sns_cm_state *state = (sns_cm_state*)this->state->state;
  sns_sensor_event *last_event_in = NULL ;
   sns_sensor_event *event_in =
       state->remote_proc_stream->api->peek_input(state->remote_proc_stream);
  
  while(NULL != event_in)
  {
    last_event_in = event_in;
    event_in = state->remote_proc_stream->api->get_next_input(state->remote_proc_stream);
  }
  if(NULL != last_event_in)
  {
    cm_handle_single_remote_proc_event(this, last_event_in);
  }
}

/**
 * Handles the events from the registry stream.
 *
 * @param[i] this The CM sensor pointer.
 */
  static void
cm_handle_registry_event(sns_sensor *const this)
{
  sns_cm_state *state = (sns_cm_state*)this->state->state;
  bool rv = false;
  sns_sensor_event *event =
    state->registry_stream->api->peek_input(state->registry_stream);
  while(NULL != event)
  {
    pb_istream_t stream = pb_istream_from_buffer((void*)event->event,
        event->event_len);

    if(SNS_REGISTRY_MSGID_SNS_REGISTRY_READ_EVENT == event->message_id)
    {
      SNS_PRINTF(ERROR,this, "Got a registry read event");
      sns_registry_read_event read_event = sns_registry_read_event_init_default;
      pb_buffer_arg group_name = {0,0};
      read_event.name.arg = &group_name;
      read_event.name.funcs.decode = pb_decode_string_cb;

      if(!pb_decode(&stream, sns_registry_read_event_fields, &read_event))
      {
        SNS_SPRINTF(ERROR,this, "Error decoding registry event: %s",
            PB_GET_ERROR(&stream));
      }
      else
      {
        stream = pb_istream_from_buffer((void*)event->event, event->event_len);

        for(uint32_t i = 0; i < ARR_SIZE(cm_registry_groups); i++)
        {
          if(0 == sns_memcmp((char*)group_name.buf, cm_registry_groups[i],
                group_name.buf_len))
          {
            sns_registry_decode_arg arg = {
              .item_group_name = &group_name,
              .parse_info_len = 1,
              .parse_info[0] = {
                .parse_func = cm_parse_reg_config,
                .parsed_buffer = &state->registry_config
              }
            };
            read_event.data.items.funcs.decode = &sns_registry_item_decode_cb;
            read_event.data.items.arg = &arg;

            rv = pb_decode(&stream, sns_registry_read_event_fields, &read_event);
          }
        }
        if(rv)
        {
          SNS_SPRINTF(HIGH,this, "Parsed Group: %s, DDR Heap Size: %d",
              (char*)group_name.buf,
              state->registry_config.ddr_heap_size);

          SNS_SPRINTF(HIGH,this, "Parsed Group: %s, Max batch disabled: %d",
              (char*)group_name.buf,
              state->registry_config.max_batch_disabled);
          SNS_SPRINTF(HIGH,this, "Parsed Group: %s, Min batch period threshold: %d",
              (char*)group_name.buf,
              state->registry_config.min_batch_period_thrshld_ms);

          sns_sensor_util_remove_sensor_stream(this,&state->registry_stream);
          break;

        }
        else
        {
          SNS_SPRINTF(ERROR,this, "Received unsupported registry group %s",
              (char*)group_name.buf);
        }
      }
      if(false == rv)
      {
        SNS_SPRINTF(ERROR,this, "Error decoding registry group %s due to %s", (char*)group_name.buf,
            PB_GET_ERROR(&stream));
      }
    }
    else
    {
      SNS_PRINTF(ERROR,this, "Received unsupported registry event msg id %u",
          event->message_id);
    }
    event = state->registry_stream->api->get_next_input(state->registry_stream);
  }
}
/**
 * Sends a registry read request.
 *
 * @param[i] this            The CM sensor pointer.
 * @param[i] reg_group_name  The registry group to be read.
 */
static void cm_send_registry_request(
    sns_sensor *const this,
    char *reg_group_name)
{
  uint8_t buffer[100];
  int32_t encoded_len;
  sns_cm_state *state = (sns_cm_state*)this->state->state;

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
    state->registry_stream->api->send_request(state->registry_stream, &request);
  }
  SNS_SPRINTF(HIGH,this, "Sending registry request for group name:%s", reg_group_name);

}

/**
 * Sends a request to the remote processor sensor.
 *
 * @param[i] state          The CM sensor state structure.
 * @param[i] this           The CM sensor pointer.
 *
 */
static void cm_send_remote_proc_request(
    sns_cm_state *state,
    sns_sensor *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  if(!state->remote_proc_state_configure && NULL != state->remote_proc_stream)
  {
    const pb_field_t *fields = sns_remote_proc_state_config_fields;
    sns_std_request std_req = sns_std_request_init_default;
    uint64_t buffer[80];
    sns_remote_proc_state_config remote_proc_state_config;

    remote_proc_state_config.proc_type = SNS_STD_CLIENT_PROCESSOR_APSS;

    size_t encoded_len = pb_encode_request(
        buffer, sizeof(buffer),
        &remote_proc_state_config, fields, &std_req);

    if(( 0 < encoded_len ) && state->remote_proc_stream)
    {
      sns_request request = (sns_request)
      { .message_id = SNS_REMOTE_PROC_STATE_MSGID_SNS_REMOTE_PROC_STATE_CONFIG,
        .request_len = encoded_len,
        .request = buffer };
      rc = state->remote_proc_stream->api->send_request(state->remote_proc_stream, &request);
      if(SNS_RC_SUCCESS == rc)
      {
        SNS_PRINTF(LOW, this, "Successfully sent config request to remote proc sensor");
        state->remote_proc_state_configure = true;
      }
      else
        SNS_PRINTF(ERROR, this, "Failed to send config request");
    }
    else
    {
      SNS_PRINTF(ERROR, this, "Failed to send sensor request, stream=%p", state->remote_proc_stream);
    }
  }
}

static sns_rc cm_sensor_notify_event(
    sns_sensor *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_cm_state *state = (sns_cm_state*)this->state->state;
  sns_service_manager *service_mgr = this->cb->get_service_manager(this);
  sns_stream_service *stream_svc = (sns_stream_service*)
    service_mgr->get_service(service_mgr, SNS_STREAM_SERVICE);
  bool completed = sns_suid_lookup_complete(&state->suid_lookup_data);

  if(NULL != state->registry_stream)
  {
    cm_handle_registry_event(this);
  }

  if(NULL != state->remote_proc_stream)
  {
    cm_handle_remote_proc_event(this);
  }

  sns_suid_lookup_handle(this, &state->suid_lookup_data);

  if(completed != sns_suid_lookup_complete(&state->suid_lookup_data))
  {
    sns_suid_lookup_deinit(this, &state->suid_lookup_data);

    if(NULL == state->remote_proc_stream)
    {
      sns_sensor_uid suid;

      sns_suid_lookup_get(&state->suid_lookup_data,
          "remote_proc_state", &suid);
      stream_svc->api->create_sensor_stream(stream_svc, this,
          suid, &state->remote_proc_stream);
      cm_send_remote_proc_request(state,this);
    }

    if(NULL == state->registry_stream)
    {
      sns_sensor_uid suid;

      sns_suid_lookup_get(&state->suid_lookup_data, "registry", &suid);
      stream_svc->api->create_sensor_stream(stream_svc,
          this, suid, &state->registry_stream);
      for(uint32_t i = 0; i < ARR_SIZE(cm_registry_groups); i++)
      {
        cm_send_registry_request(this, cm_registry_groups[i]);
      }
    }
  }

  return rc;
}

/* Publish this sensor attributes to SEE.
*/
static void cm_publish_attributes(sns_sensor *const this)
{
  //sns_cm_state *state = (sns_cm_state *)this->state->state;
  {
    char const name[] = SNS_CM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name, .buf_len = sizeof(name) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
  }
  {
    char const type[] = SNS_CM_SENSOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = type, .buf_len = sizeof(type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_TYPE, &value, 1, false);
  }
  {
    char const vendor[] = SNS_CM_VENDOR_NAME;
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = vendor, .buf_len = sizeof(vendor) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VENDOR, &value, 1, false);
  }
  {
    char const proto_files[] = SNS_CM_PROTO_NAME;
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
    value.boolean     = false;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_AVAILABLE, &value, 1, false);
  }
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    value.has_sint = true;
    value.sint     = 1;
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
}

/**
 * Perform all initialization for the QMI Client Manager.
 * Register with QMI as a service.
 *
 * @see sns_sensor::init
 */
static sns_rc cm_sensor_init(
    sns_sensor *const this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_osa_lock_attr attr;
  sns_cm_state *state = (sns_cm_state*)this->state->state;

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_NORMAL);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  cm_set_os_params(state);

  rc = sns_osa_lock_attr_init(&attr);
  rc |= sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  rc |= sns_osa_lock_create(&attr, &state->expired_timer_events_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_list_init(&state->proc_type_list);

  SNS_SUID_LOOKUP_INIT(state->suid_lookup_data, NULL);
  sns_suid_lookup_add(this, &state->suid_lookup_data, "remote_proc_state");
  sns_suid_lookup_add(this, &state->suid_lookup_data, "registry");

  //Create the processor infos here to reserve island memory
  sns_cm_create_proc_info(this, SNS_STD_CLIENT_PROCESSOR_SSC);
  sns_cm_create_proc_info(this, SNS_STD_CLIENT_PROCESSOR_APSS);
  sns_cm_create_proc_info(this, SNS_STD_CLIENT_PROCESSOR_ADSP);
  sns_cm_create_proc_info(this, SNS_STD_CLIENT_PROCESSOR_MDSP);
  sns_cm_create_proc_info(this, SNS_STD_CLIENT_PROCESSOR_CDSP);

  state->remote_proc_state_configure = false;
  //Set default to 1KB till registry becomes available.
  state->registry_config.ddr_heap_size =  1024 *1024;
  //Power test mode is disabled by default
  state->registry_config.max_batch_disabled = false;
  state->registry_config.min_batch_period_thrshld_ms =
    SNS_CM_DEFAULT_MIN_BATCH_PERIOD_THRESHOLD;
  state->bump_up = false;
  state->can_flush_to_ddr = false;

  {
#if defined(SSC_TARGET_HEXAGON)
    sns_cm_state *state = (sns_cm_state*)this->state->state;
    sns_osa_thread_attr attr;


    sns_mem_heap_register_lowmem_cb(SNS_HEAP_BATCH, SNS_CM_MAX_BATCH_HEAP_USAGE_HIGH,
        sns_client_manager_batch_heap_drop, (intptr_t)state->os_params.signal);

    sns_mem_heap_register_lowmem_cb(SNS_HEAP_BATCH, SNS_CM_MAX_BATCH_HEAP_USAGE_LOW,
        sns_client_manager_batch_heap_send, (intptr_t)state->os_params.signal);

    sns_osa_thread_attr_init(&attr);
    rc |= sns_osa_thread_attr_set_name(&attr, "SNS_CM");
    rc |= sns_osa_thread_attr_set_stack(&attr, (uintptr_t)cm_task_heap, 4096);
    rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_NORMAL);
    rc |= sns_osa_thread_attr_set_priority(&attr, SNS_CM_THREAD_PRIO);

    if(SNS_RC_SUCCESS == rc)
      rc = sns_osa_thread_create(&sns_cm_task, this, &attr, &state->thread);
    else
      SNS_PRINTF(ERROR, this, "Error setting thread attributes");
#endif
  }
  cm_publish_attributes(this);
  rc = sns_cm_service_register(this);

  return rc;
}

/* See sns_sensor::set_client_request */
static sns_sensor_instance* cm_sensor_set_client_request(
    sns_sensor *const this,
    sns_request const *curr_req,
    sns_request const *new_req, bool remove)
{
  UNUSED_VAR(this);
  UNUSED_VAR(curr_req);
  UNUSED_VAR(new_req);
  UNUSED_VAR(remove);

  return NULL;
}

/*===========================================================================
  Public Data Definitions
  ===========================================================================*/

sns_sensor_api cm_sensor_api SNS_SECTION(".rodata.island") =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &cm_sensor_init,
  .get_sensor_uid = &cm_get_sensor_uid,
  .set_client_request = &cm_sensor_set_client_request,
  .notify_event = &cm_sensor_notify_event,
};

/*===========================================================================
  Public Function Definitions
  ===========================================================================*/
/* See sns_sensor::get_sensor_uid */
  SNS_SECTION(".text.island")
sns_sensor_uid const *cm_get_sensor_uid(
    sns_sensor const *const this)
{
  UNUSED_VAR(this);
  static const sns_sensor_uid sensor_uid =
  {
    .sensor_uid =
    { 0xcd, 0x5e, 0x48, 0x7d, 0x69, 0xe8, 0x28, 0x16,
      0x2b, 0x9b, 0xef, 0x82, 0xda, 0x6d, 0x4e, 0xb4
    }
  };

  return &sensor_uid;
}

  bool
cm_parse_reg_config(
    sns_registry_data_item *reg_item,
    pb_buffer_arg *item_name,
    pb_buffer_arg *item_str_val,
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
    cm_reg_config *cfg = (cm_reg_config *)parsed_buffer;

    if(0 == strncmp((char*)item_name->buf, "heap_size", item_name->buf_len))
    {
      cfg->ddr_heap_size = reg_item->sint;
    }
    else if(0 == strncmp((char*)item_name->buf, "max_batch_disabled", item_name->buf_len))
    {
      cfg->max_batch_disabled = (reg_item->sint == 1) ? true : false;
    }
    else if(0 == strncmp((char*)item_name->buf, "min_batch_period_thrshld_ms", item_name->buf_len))
    {
      cfg->min_batch_period_thrshld_ms = reg_item->sint;
    }
  }
  else
  {
    rv = false;
  }
  return rv;
}

cm_proc_info *sns_cm_create_proc_info(
    sns_sensor *this,
    sns_std_client_processor client_proc_type)
{
  cm_proc_info *proc_info = sns_cm_get_proc_info(this, client_proc_type);
  if(NULL == proc_info)
  {
    proc_info =  (cm_proc_info*)sns_malloc(SNS_HEAP_ISLAND, sizeof(cm_proc_info));
  }

  SNS_ASSERT(NULL != proc_info);

  sns_cm_state *state = (sns_cm_state*)this->state->state;
  char name[20];

  SNS_ASSERT(NULL != proc_info);

  if(NULL != proc_info)
  {
    sns_memset(&proc_info->time,0,sizeof(cm_eval_timer));
#if ENABLE_CM_SUSPEND_SIM
    if(client_proc_type == SNS_STD_CLIENT_PROCESSOR_APSS)
      proc_info->is_suspended = false;
    else
      proc_info->is_suspended = true;
#else
    proc_info->is_suspended = false;
#endif
    proc_info->system_timer = NULL;
    proc_info->time.reg_time = 0;
    proc_info->time.time_out = 0;
    proc_info->time.sensor = this;
    proc_info->proc_type = client_proc_type;
    proc_info->non_wk_cnt = 0;
    proc_info->current_timeout = 0;
    proc_info->reval_batch_period = false;
    snprintf(name, sizeof(name), "SNS_CM_PROC_%d", proc_info->proc_type);
    proc_info->island_client = sns_island_aggregator_register_client(name);

    sns_cm_create_batch_timer(proc_info);

    sns_list_iter iter;
    sns_list_item_init(&proc_info->item,proc_info);
    sns_list_iter_init(&iter, &state->proc_type_list, false);
    sns_list_iter_insert(&iter, &proc_info->item, true);
  }
  return proc_info;

}

void sns_cm_reval_and_register_timers(
    sns_sensor *const this,
	sns_std_client_processor proc_type,
	sns_cm_request* cm_req)
{
  sns_cm_state *cm_state = (sns_cm_state*)this->state->state;
  sns_list_iter iter;
  bool proc_fnd = false;
  sns_time time_elp = 0;

  for(sns_list_iter_init(&iter, &cm_state->proc_type_list, true);
      NULL != sns_list_iter_curr(&iter);
      sns_list_iter_advance(&iter))
  {
    cm_proc_info *proc_info = (cm_proc_info*)
      sns_list_iter_get_curr_data(&iter);

    if(proc_type == proc_info->proc_type)
      proc_fnd = true;
    if( proc_fnd &&
    	 (0 != proc_info->time.reg_time))
    {
      SNS_PRINTF(LOW, this, "Reval timer for proc \'%d\'",
          proc_info->proc_type);
      sns_time elp_time = ABS(sns_get_system_time() - proc_info->time.reg_time);
      sns_cm_stop_batch_timer(cm_state, proc_info, &time_elp);
      sns_cm_flush_all_clients_for_proc(this,
    		  proc_info->delivery_type, proc_info);
      SNS_PRINTF(LOW, this, "Elapsed time low %u hi %u",
          SNS_CM_FETCH_LOW_32(elp_time), SNS_CM_FETCH_HI_32(elp_time));
    }
    if(proc_fnd)
    {
      sns_cm_configure_batch_timer((sns_fw_sensor*)this, proc_info, time_elp, cm_req);
      if(sns_cm_register_batch_timer(cm_state, proc_info, this))
      {
        SNS_PRINTF(LOW, this, "Registered a timer successfully for proc \'%d\'",
            proc_info->proc_type);
      }
      else
      {
        SNS_PRINTF(LOW, this, "Did not register timer for proc \'%d\'",
            proc_info->proc_type);
      }
      break;
    }
  }
}

sns_rc sns_register_cm(
    sns_register_cb const *register_api)
{
  register_api->init_sensor(sizeof(sns_cm_state), &cm_sensor_api,
      &cm_sensor_instance_api);

  return SNS_RC_SUCCESS;
}
