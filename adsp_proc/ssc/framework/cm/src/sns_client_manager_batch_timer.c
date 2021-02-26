/*=============================================================================
  @file sns_client_manager_batch_timer.c

  This file integrates all the timer based functionality in CM.

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_types.h"

#include "sns_client_manager.h"
#include "sns_client_manager_batch_timer.h"
#include "sns_client_manager_flush_manager.h"
#include "sns_client_manager_instance.h"
#include "sns_island.h"
#include "sns_math_util.h"
#include "sns_pb_util.h"
#include "sns_profiler.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_std.pb.h"
#include "sns_time.h"

#if defined(SSC_TARGET_HEXAGON)
  #include "qurt.h"
  #include "qurt_anysignal.h"
#endif

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Obtain the minimum wakeup or non wakeup batch period for
 * an instance. If the wakeup, non wakeup or both batch periods
 * are not available for an instance, set the unavailable value
 * to UINT64_MAX.
 *
 * @param[i] cm_inst_state  instance state structure
 * @param[i] proc_type      The processor type
 * @param[i] cm_sensor      The client manager sensor.
 * @param[i] time_left      The time left in ticks from the prev timer
 *                          stop operation.
 *
 * @param[o] wakeup         Wakeup batch period. UINT64_MAX if
 *                          no wakeup batch period is available.
 * @param[o] non_wakeup     Non wakeup batch period. UINT64_MAX
 *                          if no non wakeup batch period is
 *                          available.
 *
 */
static void cm_get_wakeup_nonwkup_periods(
    sns_cm_inst_state *cm_inst_state,
    sns_time *wakeup,
    sns_time *non_wakeup,
    sns_std_client_processor proc_type,
    sns_sensor *cm_sensor,
    sns_time time_elp,
    sns_cm_request* cm_in_req)
{
  UNUSED_VAR(cm_sensor);
  sns_isafe_list_iter req_iter;
  sns_time inst_wakeup = UINT64_MAX;
  sns_time inst_non_wakeup = UINT64_MAX;

  for(sns_isafe_list_iter_init(&req_iter, &cm_inst_state->req_list, true);
      NULL != sns_isafe_list_iter_curr(&req_iter);
      sns_isafe_list_iter_advance(&req_iter))
  {
    sns_cm_request *cm_request =
      (sns_cm_request*)sns_isafe_list_iter_get_curr_data(&req_iter);

    if(cm_request->req_msg.request.is_passive)
    {
      continue;
    }
    if(0 == cm_request->timer.batch_period ||
       proc_type != cm_request->req_msg.susp_config.client_proc_type ||
       !cm_request->req_msg.request.has_batching ||
       (cm_request->req_msg.request.has_batching && 0 == cm_request->req_msg.request.batching.batch_period))
    {
#if ENABLE_DEBUG_PRINT
      SNS_PRINTF(LOW, cm_sensor, "Skipping request");
#endif
    }
    else
    {
      sns_time rem_time = 0;
      if(cm_in_req != cm_request)
      {
        rem_time  = (cm_request->timer.batch_period - time_elp);
      }
      else
      {
        rem_time = cm_request->timer.batch_period;
      }
#if ENABLE_DEBUG_PRINT
      SNS_PRINTF(LOW, cm_sensor, "Rem time Low %u Hi %u",
                 SNS_CM_FETCH_LOW_32(rem_time),
                 SNS_CM_FETCH_HI_32(rem_time));
#endif

#if ENABLE_DEBUG_PRINT
      SNS_PRINTF(LOW,cm_sensor,"Has batching %d bp usecs %u fp usecs %u",
                  cm_request->req_msg.request.has_batching,
                  cm_request->req_msg.request.batching.batch_period,
                  cm_request->req_msg.request.batching.flush_period);
#endif
      if(SNS_CLIENT_DELIVERY_WAKEUP == cm_request->req_msg.susp_config.delivery_type &&
         rem_time < inst_wakeup)
      {
        inst_wakeup = rem_time;
      }
      else if(SNS_CLIENT_DELIVERY_NO_WAKEUP == cm_request->req_msg.susp_config.delivery_type &&
              rem_time < inst_non_wakeup )
      {
        inst_non_wakeup = rem_time;
      }
    }
  }
  *wakeup = inst_wakeup;
  *non_wakeup = inst_non_wakeup;
}

#if defined(SSC_TARGET_HEXAGON)
/*
 * This functions is the big image timer call back function registered with
 * the timer thread
 *
 * @param[i] args     The registered timer details.
 */
static void sns_cm_timer_bigimage_callback(void *args)
{
  sns_time now = sns_get_system_time();
  SNS_PROFILE(SNS_CM_BATCH_TIMER_SIG, 1, 1);

  cm_proc_info* proc_info = (cm_proc_info *)args;
  sns_sensor *sensor = (sns_sensor *)proc_info->time.sensor;
  sns_cm_state *cm_state = (sns_cm_state *)sensor->state->state;

  sns_osa_lock_acquire(cm_state->expired_timer_events_lock);
  cm_timer_event *timer_event = &cm_state->expired_timer_events[proc_info->proc_type];
  if(!timer_event->active)
  {
    timer_event->active = true;
    timer_event->delivery_type = proc_info->delivery_type;
    timer_event->proc_type = proc_info->proc_type;
    timer_event->time_now = now;
    timer_event->time_reg = proc_info->time.reg_time;
  }
  sns_osa_lock_release(cm_state->expired_timer_events_lock);

#if ENABLE_DEBUG_PRINT
  SNS_PRINTF(LOW, sensor, "Got timer call back for proc %d", proc_info->proc_type);
#endif
  qurt_anysignal_set(cm_state->os_params.signal, SNS_CM_THREAD_TIMER_SIGNAL);
}
#endif

/*=============================================================================
  Public  Function Definitions
  ===========================================================================*/

void sns_cm_configure_batch_timer(
    sns_fw_sensor *this,
    cm_proc_info *proc_info,
    sns_time time_elp,
    sns_cm_request* cm_req)
{
  sns_time wakeup = UINT64_MAX, non_wakeup = UINT64_MAX;
  sns_sensor_instance *instance = NULL;
  sns_isafe_list_iter instance_iter;
  sns_isafe_list_iter_init(&instance_iter, &this->sensor_instances, true);

  while(NULL != sns_isafe_list_iter_curr(&instance_iter))
  {
    instance = (sns_sensor_instance*)sns_isafe_list_iter_get_curr_data(&instance_iter);
    if(NULL != instance)
    {
      sns_cm_inst_state *cm_inst_state = (sns_cm_inst_state*)instance->state->state;
      sns_time req_wakeup = UINT64_MAX;
      sns_time req_nonwakeup = UINT64_MAX;
      #if ENABLE_DEBUG_PRINT
      SNS_PRINTF(LOW, (sns_sensor*)this, "Processing request for proc \'%d\'", proc_info->proc_type);
      #endif
      cm_get_wakeup_nonwkup_periods(cm_inst_state, &req_wakeup,
                                   &req_nonwakeup, proc_info->proc_type,
                                   (sns_sensor *)this, time_elp, cm_req);
      wakeup = (req_wakeup < wakeup)? req_wakeup : wakeup;
      non_wakeup = (req_nonwakeup < non_wakeup)? req_nonwakeup : non_wakeup;
    }
    sns_isafe_list_iter_advance(&instance_iter);
  }
  //No batch requests for a proc type
  if(UINT64_MAX == wakeup &&
     UINT64_MAX == non_wakeup)
  {
    #if ENABLE_DEBUG_PRINT
    SNS_PRINTF(LOW, (sns_sensor*)this, "There are no batch requests for proc_type %d" ,proc_info->proc_type);
    #endif
    proc_info->time.time_out = 0;
    return;
  }

  //Processor is awake
  if(!proc_info->is_suspended)
  {
    proc_info->time.time_out = SNS_MIN(wakeup, non_wakeup);
    proc_info->delivery_type = SNS_CLIENT_DELIVERY_WAKEUP;
    #if ENABLE_DEBUG_PRINT
    SNS_PRINTF(LOW, (sns_sensor*)this, "Setting time out to low %u hi %u",
               SNS_CM_FETCH_LOW_32(proc_info->time.time_out),
               SNS_CM_FETCH_HI_32(proc_info->time.time_out));
    #endif
  }
  //Processor in suspend
  else
  {
    if(UINT64_MAX != wakeup)
    {
      proc_info->delivery_type = SNS_CLIENT_DELIVERY_WAKEUP;
      proc_info->time.time_out = wakeup;
      #if ENABLE_DEBUG_PRINT
      SNS_PRINTF(LOW, (sns_sensor*)this, "Setting time out to low %u hi %u",
                 SNS_CM_FETCH_LOW_32(wakeup),
                 SNS_CM_FETCH_HI_32(wakeup));
     #endif
    }
    //No time out for non-wakeup sensors.
    else
    {
      proc_info->delivery_type = SNS_CLIENT_DELIVERY_NO_WAKEUP;
      proc_info->time.time_out = 0;
    }
  }

  proc_info->reval_batch_period = (0 != time_elp);

  #if ENABLE_DEBUG_PRINT
  SNS_PRINTF(LOW, (sns_sensor*)this, "Timer delivery type %d timeout %u proc_type %d'",
      proc_info->delivery_type, proc_info->time.time_out, proc_info->proc_type);
  #endif
}

void sns_cm_flush_instance_reqs(
   sns_sensor_instance *instance,
   sns_client_delivery delivery_type,
   cm_proc_info *proc_info)
{
  sns_isafe_list_iter iter;
  sns_cm_inst_state *client = (sns_cm_inst_state*)instance->state->state;
  bool is_flush_initiated = false;
  for(sns_isafe_list_iter_init(&iter, &client->req_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_cm_request *cm_request = (sns_cm_request*)
      sns_isafe_list_iter_get_curr_data(&iter);

    if(proc_info->proc_type != cm_request->req_msg.susp_config.client_proc_type)
      return;
    if(0 == cm_request->timer.batch_period)
      continue;

    //Get the instance and flush
    if(SNS_CM_NO_FLUSH == cm_request->flush_type && NULL != cm_request->data_stream)
    {
      // Only flush if delivery is wakeup or (non-wakeup and processor is awake)
      if((SNS_CLIENT_DELIVERY_WAKEUP == delivery_type) ||
         (SNS_CLIENT_DELIVERY_NO_WAKEUP == delivery_type &&
          !proc_info->is_suspended))
      {
        sns_rc rc = SNS_RC_FAILED;
        if(SNS_CM_IS_FLUSH_REQ(cm_request))
        {
          rc = cm_request->data_stream->api->initiate_flush(cm_request->data_stream);
        }
        if(SNS_RC_SUCCESS == rc)
        {
          cm_request->flush_type = SNS_CM_BATCH_FLUSH;
          is_flush_initiated =  true;
        }
        else
          SNS_INST_PRINTF(ERROR, instance, "Flush initiate failed for %d with error %d", client->client_id, rc);
      }
    }
  }
  if(is_flush_initiated)
  {
    sns_cm_bump_up_on_flush_initiate(instance);
  }
}

void sns_cm_flush_all_clients_for_proc(
    sns_sensor *const sensor,
    sns_client_delivery delivery_type,
    cm_proc_info *proc_info)
{
  SNS_PROFILE(SNS_CM_UPDATE_ALL_TIMERS_FUNC, 1, 1);
  sns_sensor_instance *instance;
  for(instance = sensor->cb->get_sensor_instance(sensor, true);
      NULL != instance;
      instance = sensor->cb->get_sensor_instance(sensor, false))
  {
    sns_cm_flush_instance_reqs(instance, delivery_type, proc_info);
  }
  SNS_PROFILE(SNS_CM_UPDATE_ALL_TIMERS_FUNC, 1, 2);
}

void sns_cm_handle_timer_event(
    sns_fw_sensor *sensor,
    cm_timer_event *timer_event)
{
  SNS_PROFILE(SNS_CM_HNDL_TIMER_EVENT_FUNC, 1, 1);
  sns_sensor *this = (sns_sensor*)sensor;
  cm_proc_info *proc_info = sns_cm_get_proc_info((sns_sensor*)sensor, timer_event->proc_type);

  if(NULL == proc_info)
  {
    SNS_PRINTF(ERROR, this, "Could not find proc info for proc %d", timer_event->proc_type);
    return;
  }

  if(proc_info->reval_batch_period)
  {
    sns_cm_state *cm_state = (sns_cm_state *)this->state->state;
    sns_cm_stop_batch_timer(cm_state, proc_info, NULL);
    sns_cm_configure_batch_timer((sns_fw_sensor*)this, proc_info, 0, NULL);
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
  }
  sns_cm_flush_all_clients_for_proc(this, timer_event->delivery_type, proc_info);

  SNS_PROFILE(SNS_CM_HNDL_TIMER_EVENT_FUNC, 1, 2);
}

void sns_cm_stop_batch_timer(
    sns_cm_state *cm_state,
    cm_proc_info *proc_info,
    sns_time * time_elp)
{
  UNUSED_VAR(cm_state);
  sns_time rem_time =0;
  if(0 != proc_info->current_timeout)
  {
    sns_timer_stop(proc_info->system_timer, &rem_time);
  }
  if(NULL != time_elp)
  {
    *time_elp =  proc_info->time.time_out - rem_time;
  }
  proc_info->time.reg_time = 0;
  proc_info->time.time_out = 0;
  proc_info->current_timeout = 0;
}

bool sns_cm_start_timer(
   sns_cm_state *cm_state,
   cm_proc_info *proc_info,
   sns_sensor *this)
{
  UNUSED_VAR(cm_state);
  bool rc = false;
  bool arm_timer = true;
  sns_time now = sns_get_system_time();
  if(0 == proc_info->time.time_out)
  {
    rc = false;
    arm_timer = false;
  }
  else if(proc_info->current_timeout == proc_info->time.time_out)
  {
    // No changes needed to already registered batch timer
    rc = true;
    arm_timer = false;
  }
#if defined(SSC_TARGET_HEXAGON)
  else if(proc_info->time.time_out < sns_convert_ns_to_ticks(SNS_CM_SHORTEST_BATCH_PERIOD_NS))
  {
    // The remaining timeout period is shorter than the shortest allowed
    // time. Pretend the timer has expired.
    rc = true;
    sns_cm_timer_bigimage_callback((void *)proc_info);

    // Now recalculate the batch period with 0 elapsed time
    sns_cm_configure_batch_timer((sns_fw_sensor *)this, proc_info, 0, NULL);
  }
#endif

  if(arm_timer)
  {
    //Start timer with updated timeout
    if(SNS_RC_SUCCESS == sns_timer_start_relative(proc_info->system_timer,
                         proc_info->time.time_out))
    {
      proc_info->current_timeout = proc_info->time.time_out;
      proc_info->time.reg_time = now;
      rc = true;
    }
    else
    {
      rc = false;
      SNS_PRINTF(ERROR, this, "Time start failed for proc %d", proc_info->proc_type);
    }
  }
  return rc;
}

bool sns_cm_register_batch_timer (
   sns_cm_state *cm_state,
   cm_proc_info *proc_info,
   sns_sensor *this)
{
  bool rc = false;

  if(0 != proc_info->time.time_out &&
     ((SNS_CLIENT_DELIVERY_WAKEUP == proc_info->delivery_type) ||
     (!proc_info->is_suspended &&
     (SNS_CLIENT_DELIVERY_NO_WAKEUP == proc_info->delivery_type))))
  {
     rc = sns_cm_start_timer(cm_state,proc_info,this);
  }

  if(!rc)
  {
    sns_time time_left = 0;
    // Stop the timer.
    sns_cm_stop_batch_timer(cm_state, proc_info, &time_left);
  }
  return rc;
}

void sns_cm_create_batch_timer(
   cm_proc_info *proc_info)
{
#if defined(SSC_TARGET_HEXAGON)
  sns_timer_attr timer_attr;
  sns_timer_attr_init(&timer_attr);
  sns_timer_attr_set_memory_partition(&timer_attr, SNS_OSA_MEM_TYPE_NORMAL);
  sns_timer_attr_set_periodic(&timer_attr, true);
  sns_timer_attr_set_deferrable(&timer_attr, false);

  sns_timer_create(sns_cm_timer_bigimage_callback,
    proc_info,
    &timer_attr,
    &proc_info->system_timer);
#else
  UNUSED_VAR(proc_info);
#endif

}
