/*=============================================================================
  @file sns_timer_sensor.c

  PEND: Use Timer Sensor as PRINTF argument

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
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
#include "sns_attribute_util.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_std.pb.h"
#include "sns_suid.pb.h"
#include "sns_thread_prio.h"
#include "sns_timer.pb.h"
#include "sns_timer_sensor.h"
#include "sns_timer_sensor_instance.h"
#include "sns_types.h"

/*=============================================================================
  Macros and Constants
  ===========================================================================*/

/* Enable for extensive & costly debugging messages */
//#define SNS_TIMER_ENABLE_DEBUG


/* Virtual timer will be created with this interval. Used so that other timers
   will sync to this one if possible */
#define TIMER_SYNC_INTERVAL_NS 1000000 /* 1ms */

/*=============================================================================
  Globals
  ===========================================================================*/
static const sns_std_request default_sns_std_request SNS_SECTION(".rodata.sns") = sns_std_request_init_default;
static const sns_timer_sensor_config default_sns_timer_sensor_config SNS_SECTION(".rodata.sns") = sns_timer_sensor_config_init_default;
static const sns_sensor_uid sensor_uid SNS_SECTION(".rodata.sns") = TIMER_SUID;

/*=============================================================================
  Internal functions
  ===========================================================================*/

/* ------------------------------------------------------------------------------------ */
/**
 * Utility function to get the head of a list
 *
 * @param[i] list   list to get the head of
 *
 * @return pointer to list head item
 */
SNS_SECTION(".text.sns")
static sns_isafe_list_item *
sns_timer_get_list_head( sns_isafe_list*list )
{
  sns_isafe_list_iter iter;
  sns_isafe_list_item *list_head;

  sns_isafe_list_iter_init(&iter, list, true);
  list_head = sns_isafe_list_iter_curr(&iter);
  return list_head;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Utility function to compute a synchronized start time for timers
 *
 * This will return a time close to target_start, such that the returned time
 * is synced_start + N * period_step, for some integer N.
 *
 * @param[i] target_start   The desired timer start time
 * @param[i] synced_start   A synchronized start time
 * @param[i] period_step    A period of the synchronized timer to sync to
 *
 * @return Absolute start time of a synchronized timer
 */
SNS_SECTION(".text.sns")
static sns_time
sns_timer_select_timer_start( int64_t target_start,
                              int64_t synced_start,
                              int64_t period_step )
{
  int32_t num_periods = (synced_start - target_start) / period_step;
  return (synced_start - (num_periods * period_step));
}

/* ------------------------------------------------------------------------------------ */
/**
 * Callback registered with the timer utility. Runs in timer utility context
 *
 * This saves information about the current time into the "read only" sensor buffer, and
 * signals the timer sensor via the OS signal.
 *
 * @param[i] args   Pointer to the timer sensor "read-only" buffer
 */
SNS_SECTION(".text.sns")
static void
sns_timer_sensor_callback(sns_timer_cb_func_arg args)
{
  SNS_PROFILE(SNS_TIMER_CALLBACK_START, 0);
  sns_timer_read_only_buffer* ro_buffer = (sns_timer_read_only_buffer *)args;
  sns_time now = sns_get_system_time();
  ro_buffer->timeout_id_counter += 1;
  ro_buffer->timeout_event.last_timeout_id = ro_buffer->timeout_id_counter;
  ro_buffer->timeout_event.timestamp = now;
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf, "Timer callback now %x",
             (uint32_t)now);
#endif
  SNS_PROFILE(SNS_TIMER_SET_SIGNAL, 1, ro_buffer->signal);
  sns_osa_thread_sigs_set(ro_buffer->sig_thread->thread, ro_buffer->signal);
  SNS_PROFILE(SNS_TIMER_CALLBACK_END, 0);
}

/* ------------------------------------------------------------------------------------ */
/**
 * Utility function arm the system timer
 *
 * Will start the system timer based on the first timer in the list of registered timers.
 * If the list is empty, it will stop the system timer.
 *
 * @param[i] state    Timer sensor state
 */
SNS_SECTION(".text.sns")
static void
sns_timer_sensor_arm_system_timer(sns_timer_state *state)
{
  sns_isafe_list_item *iter_item;

  iter_item = sns_timer_get_list_head( &state->registered_timers );
  if( NULL != iter_item )
  {
    sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
    sns_timer_instance_state *registered_entry =
      (sns_timer_instance_state *)(registered_inst->state->state);

    sns_timer_start_absolute(state->system_timer, registered_entry->trigger_time);
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf, "Set timer to %x", (uint32_t)registered_entry->trigger_time);
#endif
  }
  else
  {
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf, "stopping system timer");
#endif
    sns_timer_stop(state->system_timer, NULL);
  }
}

/* ------------------------------------------------------------------------------------ */
/**
 * Adds a new timer to the list of timers
 *
 * There are two lists:
 * Registered timers:
 *   This list is sorted first by next timeout, and then by priority. It does not contain
 *   "dry_run" (aka virtual) timers, whether periodic or not.
 *
 * Periodic timers:
 *   This list contains only periodic timers. It is sorted by period, and then by priority.
 *   It may contain dry_run (virtual) periodic timers.
 *
 * @param[i] state          Timer sensor state
 * @param[i] inst           Instance state (the new timer to add to the list)
 * @param[i] rearm_timer_if_needed  Will rearm the system timer if the list head changes
 *
 * @return SNS_RC_SUCCESS
 */
SNS_SECTION(".text.sns")
static sns_rc
sns_timer_sensor_record_entry(sns_timer_state *state,
                              sns_sensor_instance *inst,
                              bool rearm_timer_if_needed)
{
  sns_rc ret_val = SNS_RC_FAILED;
  sns_isafe_list_iter iter;
  sns_isafe_list_item *orig_list_head = NULL;
  sns_isafe_list_item *iter_item;
  sns_timer_instance_state *time_entry = (sns_timer_instance_state *)inst->state->state;

  /* Add to the list of active timers */
  if( !time_entry->client_config.is_dry_run )
  {
    sns_isafe_list_iter_init(&iter, &state->registered_timers, true);
    sns_isafe_list_item_init(&time_entry->list_entry, inst);
    iter_item = sns_isafe_list_iter_curr(&iter);
    orig_list_head = iter_item;
    while( NULL != iter_item )
    {
      sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
      sns_timer_instance_state *registered_entry =
        (sns_timer_instance_state *)(registered_inst->state->state);
      sns_time sorted_trigger_time = registered_entry->trigger_time;
      if( time_entry->trigger_time <= sorted_trigger_time )
      {
        while( time_entry->trigger_time == registered_entry->trigger_time &&
               time_entry->client_config.priority < registered_entry->client_config.priority &&
               NULL != (iter_item = sns_isafe_list_iter_advance(&iter)))
        {
          // Advance to the next item in the list if the trigger times are the
          // same, but the priority is higher
          registered_inst = sns_isafe_list_item_get_data(iter_item);
          registered_entry =
            (sns_timer_instance_state *)(registered_inst->state->state);
#ifdef SNS_TIMER_ENABLE_DEBUG
          SNS_PRINTF(LOW, sns_fw_printf, "recording entry skipping prio");
#endif
        }
        break;
      }
#ifdef SNS_TIMER_ENABLE_DEBUG
      SNS_PRINTF(LOW, sns_fw_printf, "recording entry advance");
#endif
      iter_item = sns_isafe_list_iter_advance(&iter);
    }

    sns_isafe_list_iter_insert(&iter, &time_entry->list_entry, false);
  }

  /* If periodic, add to the list of periodic timers */
  if( time_entry->client_config.is_periodic &&
      !sns_isafe_list_item_present( &time_entry->periodic_entry) )
  {
    sns_isafe_list_item_init(&time_entry->periodic_entry, inst);
    sns_isafe_list_iter_init(&iter, &state->periodic_timers, true);
    iter_item = sns_isafe_list_iter_curr(&iter);
    while( NULL != iter_item )
    {
      sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
      sns_timer_instance_state *registered_entry =
        (sns_timer_instance_state *)(registered_inst->state->state);
      sns_time sorted_timeout_period = registered_entry->client_config.timeout_period;
      if( time_entry->client_config.timeout_period <= sorted_timeout_period )
      {
        break;
      }
      iter_item = sns_isafe_list_iter_advance(&iter);
    }
    sns_isafe_list_iter_insert(&iter, &time_entry->periodic_entry, false);
  }

  if( rearm_timer_if_needed &&
      !time_entry->client_config.is_dry_run )
  {
    if( orig_list_head != sns_timer_get_list_head(&state->registered_timers) )
    {
      sns_timer_sensor_arm_system_timer(state);
    }
  }
  ret_val = SNS_RC_SUCCESS;

  return ret_val;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Deletes a timer from the timer lists.
 *
 * @param[i] state          Timer sensor state
 * @param[i] inst           Instance state (the new timer to add to the list)
 * @param[i] rearm_timer_if_needed  Will rearm the system timer if the list head changes
 * @param[i] delete_from_periodic_list  Whether to remove the timer from the list of
 *                                      periodic timers or not
 */
SNS_SECTION(".text.sns")
static void
sns_timer_sensor_delete_entry(sns_timer_state *state,
                              sns_sensor_instance *inst,
                              bool rearm_timer_if_needed,
                              bool delete_from_periodic_list)
{
  bool reset_timer = false;
  sns_sensor_instance *inst_head;
  sns_timer_instance_state *inst_state =
    (sns_timer_instance_state *)inst->state->state;

  if(rearm_timer_if_needed)
  {
    inst_head =
      sns_isafe_list_item_get_data(sns_timer_get_list_head(&state->registered_timers));
    reset_timer = (inst_head == inst)? true: false;
  }

  sns_isafe_list_item_remove( &inst_state->list_entry );
  if( delete_from_periodic_list )
  {
    sns_isafe_list_item_remove( &inst_state->periodic_entry );
  }

  if(reset_timer)
  {
    sns_timer_sensor_arm_system_timer(state);
  }
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf, "Deleted timer: %x",
    (intptr_t)inst);
#endif
}

/* ------------------------------------------------------------------------------------ */
/**
 * Updates the trigger_time for dry_run (virtual) timers
 *
 * Virtual timers do not expire, so their trigger_times are updated only when this
 * function is called.
 *
 * @param[i] state          Timer sensor state
 * @param[i] system_time    Current time
 */
SNS_SECTION(".text.sns")
static void
sns_timer_update_dry_run_timers(sns_timer_state *state, sns_time system_time)
{
  sns_isafe_list_iter iter;
  sns_isafe_list_item *iter_item;

  sns_isafe_list_iter_init(&iter, &state->periodic_timers, true);
  iter_item = sns_isafe_list_iter_curr(&iter);
  while( NULL != iter_item )
  {
    sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
    sns_timer_instance_state *registered_entry =
      (sns_timer_instance_state *)(registered_inst->state->state);
    if( registered_entry->client_config.is_dry_run )
    {
      registered_entry->trigger_time =
        ((system_time / registered_entry->client_config.timeout_period) + 1) *
        (registered_entry->client_config.timeout_period);
#ifdef SNS_TIMER_ENABLE_DEBUG
      SNS_PRINTF(LOW, sns_fw_printf, "updating dryrun timer to: %x. sys_time %x",
                 (uint32_t)registered_entry->trigger_time,
                 (uint32_t)system_time);
#endif
    }
    iter_item = sns_isafe_list_iter_advance(&iter);
  }
}

/* ------------------------------------------------------------------------------------ */
/**
 * Updates registered timer list to account for expired timers
 *
 * This will remove expired non-periodic timers from the registered timer list.
 * Periodic timers are advanced to the appropriate spot in the registered timer list.
 * The list of periodic timers is not changed.
 * This function assumes that all events associated with the expired timers have already
 * been sent.
 *
 * Rearms the system timer based on the new head of the registered timer list.
 *
 * @param[i] state          Timer sensor state
 * @param[i] last_timeout   Current time. Timers older than this have expired
 */
SNS_SECTION(".text.sns")
static sns_rc
sns_timer_sensor_handle_expired(sns_sensor *this,
                                sns_time last_timeout)
{
  sns_timer_state *state = (sns_timer_state*)this->state->state;
  sns_isafe_list_item *list_head;

  while(NULL != (list_head = sns_timer_get_list_head(&state->registered_timers)))
  {
    sns_sensor_instance *inst =
      (sns_sensor_instance*)sns_isafe_list_item_get_data(list_head);
    sns_timer_instance_state *entry = (sns_timer_instance_state*)inst->state->state;
    if(entry->trigger_time <= last_timeout)
    {
      sns_timer_sensor_delete_entry(state, inst, false, false);
      if(entry->client_config.is_periodic)
      {
        entry->trigger_time += entry->client_config.timeout_period;
        sns_timer_sensor_record_entry(state, inst, false);
      }
      else
      {
        this->cb->remove_instance(inst);
      }
    }
    else
    {
      break;
    }
  }
  sns_timer_sensor_arm_system_timer(state);

  return SNS_RC_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Generates an event for an expired timer.
 *
 * @param[i] timer_inst     Instance state (the expired timer)
 * @param[i] timer_sensor   Timer sensor state
 * @param[i] system_time    Current time
 *
 * @return SNS_RC_SUCCESS if the event is sent.
 */
SNS_SECTION(".text.sns")
static sns_rc
sns_timer_inst_generate_event(sns_sensor_instance *const timer_inst,
                              sns_sensor *const timer_sensor,
                              sns_time system_time)
{
  sns_timer_instance_state * state =
    (sns_timer_instance_state *)timer_inst->state->state;
  sns_timer_state *sensor_state = (sns_timer_state *)timer_sensor->state->state;
  sns_timer_sensor_event timer_event = sns_timer_sensor_event_init_default;

  timer_event.timeout_time = sensor_state->ro_buffer.timeout_event.timestamp;
  timer_event.requested_timeout_time = state->trigger_time;

  if(!pb_send_event(timer_inst, sns_timer_sensor_event_fields, &timer_event,
                    system_time, SNS_TIMER_MSGID_SNS_TIMER_SENSOR_EVENT,
                    NULL))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Timer event failure. instance %p",
               timer_inst);
    return SNS_RC_FAILED;
  }
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf, "Timer sent event for %x, tout %x, now %x",
             (uint32_t)timer_inst, (uint32_t)state->trigger_time,
             (uint32_t)system_time );
#endif
  return SNS_RC_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Registered callback with the signal thread. Handles the expired system timer
 *
 * Does all of the necessary work for handling expired timers
 *
 * @param[i] this  The timer sensor
 */
SNS_SECTION(".text.sns")
static void
sns_timer_process_signal_events(sns_sensor *this,
                                uint32_t signal,
                                void const *args)
{
  UNUSED_VAR(signal);
  UNUSED_VAR(args);
  sns_timer_state *state = (sns_timer_state*)this->state->state;
  sns_isafe_list_iter iter;
  sns_time min_timeout_ticks =
    sns_convert_ns_to_ticks(SNS_TIMER_SENSOR_TIMEOUT_MIN_TIMEOUT_NANOSEC);
  sns_time now = sns_get_system_time();

  sns_isafe_list_iter_init(&iter, &state->registered_timers, true);

#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf, "Timer process sig events last TO state:%x ro:%x now:%x",
             (uint32_t)state->last_timeout_id_processed,
             (uint32_t)state->ro_buffer.timeout_event.last_timeout_id,
             (uint32_t)now);
#endif
  if(state->last_timeout_id_processed != state->ro_buffer.timeout_event.last_timeout_id)
  {
    sns_time timeout_limit = now + min_timeout_ticks;
    sns_isafe_list_item *iter_item;

    state->last_timeout_id_processed = state->ro_buffer.timeout_event.last_timeout_id;

    iter_item = sns_isafe_list_iter_curr(&iter);
    while( NULL != iter_item )
    {
      sns_sensor_instance *inst =
        (sns_sensor_instance*)sns_isafe_list_item_get_data(iter_item);
      sns_timer_instance_state *entry = (sns_timer_instance_state*)inst->state->state;

      if(entry->trigger_time <= timeout_limit)
      {
        SNS_PROFILE(SNS_TIMER_SEND_EVENT_START, 0);
        sns_timer_inst_generate_event(inst, this, now);
        SNS_PROFILE(SNS_TIMER_SEND_EVENT_END, 0);
      }
      else
      {
        break;
      }
      iter_item = sns_isafe_list_iter_advance(&iter);
    }
    sns_timer_sensor_handle_expired(this, timeout_limit);
  }
}

/* ------------------------------------------------------------------------------------ */
/**
 * Publish all attributes for the timer sensor.
 */
static void
sns_timer_sensor_publish_attributes(sns_sensor *const this)
{
  {
    sns_std_attr_value_data value = sns_std_attr_value_data_init_default;
    char const name_type[] = "timer";
    value.str.funcs.encode = pb_encode_string_cb;
    value.str.arg = &((pb_buffer_arg)
        { .buf = name_type, .buf_len = sizeof(name_type) });
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_NAME, &value, 1, false);
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
    char const proto_files[] = "sns_timer.proto";
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
    value.sint = 2;
    sns_publish_attribute(
        this, SNS_STD_SENSOR_ATTRID_VERSION, &value, 1, true);
  }
}

/* ------------------------------------------------------------------------------------ */
/**
 * Returns decoded request message for type
 * sns_timer_sensor_config_fields.
 *
 * @param[in] in_request   Request as sotred in client_requests
 *                         list.
 * @param decoded_request  Standard decoded message.
 * @param decoded_payload  Decoded stream request payload.
 *
 * @return bool true if decode is successful else false
 */
SNS_SECTION(".text.sns")
static bool
sns_timer_get_decoded_req(sns_request const *in_request,
                          sns_std_request *decoded_request,
                          sns_timer_sensor_config *decoded_payload)
{
  pb_istream_t stream;
  pb_simple_cb_arg arg =
      { .decoded_struct = decoded_payload,
        .fields = sns_timer_sensor_config_fields };
  decoded_request->payload = (struct pb_callback_s)
      { .funcs.decode = &pb_decode_simple_cb, .arg = &arg };
  stream = pb_istream_from_buffer(in_request->request,
                                  in_request->request_len);
  if(!pb_decode(&stream, sns_std_request_fields, decoded_request))
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Timer decode error");
    return false;
  }
  return true;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Finds a configuration for start and timeout times for new non-periodic timers
 *
 * This will attempt to select a new configuration for a timer such that the new
 * timer will expire at the same time as an existing periodic timer.
 *
 * @param[i] state            Timer sensor state
 * @param[i] req_config       Requested timer configuration
 * @param[o] selected_config  Selected timer configuration
 */
SNS_SECTION(".text.sns")
static void
sns_timer_find_aperiodic_best_fit(sns_timer_state *state,
                                  sns_timer_sensor_config const *const req_config,
                                  sns_timer_sensor_reg_event *selected_config)
{
  sns_time target_expiry = req_config->start_time + req_config->timeout_period;
  sns_time earliest_expiry = target_expiry;
  sns_time latest_expiry = target_expiry;
  sns_isafe_list_iter iter;
  sns_isafe_list_item *iter_item;

  // Only search through periodic timers. There's not a lot of point in
  // trying to sync to the of all timers.
  // Moreover, the "virtual" timer is only stored on the periodic timer list,
  // and we do want to sync to the virtual timer.
  sns_isafe_list_iter_init(&iter, &state->periodic_timers, false);

  if( req_config->has_start_config )
  {
    earliest_expiry -= req_config->start_config.early_start_delta;
    latest_expiry   += req_config->start_config.late_start_delta;
  }
  if( req_config->has_timeout_config )
  {
    earliest_expiry -= req_config->timeout_config.low_timeout_delta;
    latest_expiry   += req_config->timeout_config.high_timeout_delta;
  }
  iter_item = sns_isafe_list_iter_curr(&iter);
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf, "finding best fit. earliest %x latest %x",
             (uint32_t)earliest_expiry, (uint32_t)latest_expiry);
#endif
  while( NULL != iter_item )
  {
    sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
    sns_timer_instance_state *registered_entry =
      (sns_timer_instance_state *)(registered_inst->state->state);
    sns_time expiry_under_test = registered_entry->trigger_time;
    sns_time period_under_test = registered_entry->client_config.timeout_period;

    int32_t ceiling;
    int32_t floor;

    ceiling =
      /* ceil((earliest - expiriy_under_test)/period_under_test) */
      1 + (((earliest_expiry-expiry_under_test)-1) / period_under_test);

    floor = (latest_expiry - expiry_under_test) / period_under_test;

#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf, "finding best fit expy %x, period %x",
               (uint32_t)expiry_under_test,
               (uint32_t)period_under_test );
    SNS_PRINTF(LOW, sns_fw_printf, "floor %x ceil %x",
               floor, ceiling);
#endif
    if( ceiling <= floor )
    {
      target_expiry = (target_expiry / period_under_test) * period_under_test
        - (expiry_under_test % period_under_test);
      if( target_expiry < earliest_expiry )
      {
        target_expiry += period_under_test;
      }
      break;
    }
    if( latest_expiry <= registered_entry->trigger_time )
    {
      break;
    }
    iter_item = sns_isafe_list_iter_return(&iter);
  }
  selected_config->timeout_period = target_expiry - selected_config->start_time;
}


/* ------------------------------------------------------------------------------------ */
/**
 * Finds a configuration for start and timeout times for new periodic timers
 *
 * This will attempt to select a new configuration for a timer such that the new
 * timer will expire at the same time as an existing periodic timer.
 *
 * @param[i] state            Timer sensor state
 * @param[i] req_config       Requested timer configuration
 * @param[o] selected_config  Selected timer configuration
 */
SNS_SECTION(".text.sns")
static void
sns_timer_find_periodic_best_fit(sns_timer_state *state,
                                 sns_timer_sensor_config const *const req_config,
                                 sns_timer_sensor_reg_event *selected_config)
{
  sns_isafe_list_iter iter;
  sns_isafe_list_item *iter_item;

  /* Purposefully not using sns_time, since signed values are needed */
  int64_t target_period   = req_config->timeout_period;
  int64_t shortest_period = target_period;
  int64_t longest_period  = target_period;

  int64_t target_start   = req_config->start_time;
  int64_t earliest_start = target_start;
  int64_t latest_start   = target_start;

  int64_t selected_period     = 0;
  int64_t selected_start_time = 0;

  if( req_config->has_start_config )
  {
    earliest_start -= req_config->start_config.early_start_delta;
    latest_start   += req_config->start_config.late_start_delta;
  }
  if( req_config->has_timeout_config )
  {
    shortest_period -= req_config->timeout_config.low_timeout_delta;
    longest_period  += req_config->timeout_config.high_timeout_delta;
  }

#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf,
             "Finding periodic best fit for: shortest_period %x longest_period %x req_period %x",
             (uint32_t)shortest_period,
             (uint32_t)longest_period,
             (uint32_t)target_period);
  SNS_PRINTF(LOW, sns_fw_printf,
             "Finding periodic best fit for: earliest_start %x latest_start %x target_start %x",
             (uint32_t)earliest_start,
             (uint32_t)latest_start,
             (uint32_t)target_start);
#endif

  // Step 1: Search for slower periodic timers, such that a factor of the
  // slower period is within the period range.
  // Choose a factor of the slowest periodic timer that is within the
  // requested range (and closest to the target value)
  sns_isafe_list_iter_init(&iter, &state->periodic_timers, false);
  iter_item = sns_isafe_list_iter_curr(&iter);
  while( NULL != iter_item && selected_period == 0 )
  {
    int_fast16_t divisor;
    sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
    sns_timer_instance_state *cur_inst_state =
      (sns_timer_instance_state *)(registered_inst->state->state);
    sns_timer_sensor_reg_event *cur_timer =
      (sns_timer_sensor_reg_event *)(&cur_inst_state->client_config);

#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf,
               "trying step 1: cur timeout %x, cur trigger time %x",
               (uint32_t)cur_timer->timeout_period,
               (uint32_t)cur_inst_state->trigger_time);
#endif

    if( shortest_period > (int64_t)cur_timer->timeout_period )
    {
      // Step 1 complete, since this period isn't slower
#ifdef SNS_TIMER_ENABLE_DEBUG
      SNS_PRINTF(LOW, sns_fw_printf, "No fit step 1. shortest > current timeout");
#endif
      break;
    }
    if( longest_period > (int64_t)cur_timer->timeout_period )
    {
      selected_period = cur_timer->timeout_period;
      selected_start_time = sns_timer_select_timer_start( target_start,
                                                          cur_inst_state->trigger_time,
                                                          selected_period );
#ifdef SNS_TIMER_ENABLE_DEBUG
      SNS_PRINTF(LOW, sns_fw_printf, "fit in step 1: selected_period %x cur trigger time %x",
                 (uint32_t)selected_period, (uint32_t)cur_inst_state->trigger_time );
#endif
    }
    else
    {
      divisor = 2;
      do {
        int64_t period_under_test =
          (int64_t)cur_timer->timeout_period / divisor;

#ifdef SNS_TIMER_ENABLE_DEBUG
        SNS_PRINTF(LOW, sns_fw_printf, "trying step 1: period_under_test %x",
                   (uint32_t)period_under_test);
#endif
        if( period_under_test * divisor == cur_timer->timeout_period &&
            period_under_test >= shortest_period &&
            period_under_test <= longest_period )
        {
          if( llabs(period_under_test - target_period) < llabs(selected_period - target_period) )
          {
            selected_period = period_under_test;
            selected_start_time = sns_timer_select_timer_start( target_start,
                                                                cur_inst_state->trigger_time,
                                                                selected_period );
#ifdef SNS_TIMER_ENABLE_DEBUG
            SNS_PRINTF(LOW, sns_fw_printf, "fit in step 1");
#endif
          }
        }
        divisor++;
      } while( selected_period == 0 &&
               ((int64_t)cur_timer->timeout_period / divisor) > shortest_period );
    }
    iter_item = sns_isafe_list_iter_return( &iter );
  }

  // Step 2: If no timer was found in step 1, find a faster period such that the
  // faster period * N is within range.
  while( NULL != iter_item && selected_period == 0 )
  {
    sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
    sns_timer_instance_state *cur_inst_state =
      (sns_timer_instance_state *)(registered_inst->state->state);
    sns_timer_sensor_reg_event *cur_timer =
      (sns_timer_sensor_reg_event *)(&cur_inst_state->client_config);
    int64_t period_under_test = (int64_t)cur_timer->timeout_period;
    int32_t ceiling_of_num_periods_before;
    int32_t floor_of_num_periods_after;

    ceiling_of_num_periods_before =
      /* ceil(shortest_period / period_under_test) */
      1 + ((shortest_period-1) / period_under_test);
    floor_of_num_periods_after =
      longest_period / period_under_test;

#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf,
               "trying step 2: Ceiling %x floor %x, period_under_test %x",
               ceiling_of_num_periods_before, floor_of_num_periods_after,
               (uint32_t)period_under_test);
#endif

    if( ceiling_of_num_periods_before <= floor_of_num_periods_after )
    {
      selected_period =
        (target_period / period_under_test) * period_under_test;
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf,
               "fit step 2. selected_period %x",
               (uint32_t) selected_period);
#endif
      if( selected_period < shortest_period )
      {
#ifdef SNS_TIMER_ENABLE_DEBUG
        SNS_PRINTF(LOW, sns_fw_printf,
                   "fit step 2. selected_period += %x",
                   (uint32_t)period_under_test);
#endif
        selected_period += period_under_test;
      }
      if( ( period_under_test % selected_period ) <=
          ( latest_start - earliest_start ) )
      {
        selected_start_time = sns_timer_select_timer_start( target_start,
                                                            cur_inst_state->trigger_time,
                                                            period_under_test );
#ifdef SNS_TIMER_ENABLE_DEBUG
        SNS_PRINTF(LOW, sns_fw_printf,
                   "fit step 2 start time %x. target_start %x",
                   (uint32_t)selected_start_time,
                   (uint32_t)cur_inst_state->trigger_time);
#endif
      }
      else
      {
#ifdef SNS_TIMER_ENABLE_DEBUG
        SNS_PRINTF(LOW, sns_fw_printf,
                   "fit step 2 start no fit %x > %x",
                   (uint32_t)( period_under_test % selected_period ),
                   (uint32_t)( latest_start - earliest_start ) );
#endif
        selected_start_time = target_start;
      }
      break;
    }

    iter_item = sns_isafe_list_iter_return( &iter );
  }

  // Step 3: No matching periodic timers. At least try to match up with some existing timer
  if( selected_period == 0 )
  {
#ifdef SNS_TIMER_ENABLE_DEBUG
    SNS_PRINTF(LOW, sns_fw_printf, "finding periodic fit step 3");
#endif
    selected_period = target_period;
    selected_start_time = target_start;
    sns_isafe_list_iter_init(&iter, &state->registered_timers, false);
    iter_item = sns_isafe_list_iter_curr(&iter);
    while( NULL != iter_item )
    {
      sns_sensor_instance *registered_inst = sns_isafe_list_item_get_data(iter_item);
      sns_timer_instance_state *cur_inst_state =
        (sns_timer_instance_state *)(registered_inst->state->state);
      int64_t start_under_test = (int64_t)cur_inst_state->trigger_time;

      if( ( start_under_test % selected_period ) <=
          ( latest_start - earliest_start ) )
      {
        selected_start_time = sns_timer_select_timer_start( target_start,
                                                            start_under_test,
                                                            selected_period );
        break;
      }
      iter_item = sns_isafe_list_iter_return( &iter );
    }
  }
  selected_config->timeout_period = selected_period;
  selected_config->start_time = selected_start_time;
}

/* ------------------------------------------------------------------------------------ */
/**
 * Attempt to synchronize new timers, if possible.
 *
 * @param[i] state            Timer sensor state
 * @param[i] req_config       Requested timer configuration
 * @param[o] selected_config  Selected timer configuration
 */
SNS_SECTION(".text.sns")
static void
sns_timer_find_best_fit(sns_timer_state *state,
                        sns_timer_sensor_config const *const req_config,
                        sns_timer_sensor_reg_event *selected_config)
{
#ifdef SNS_TIMER_ENABLE_DEBUG
  SNS_PRINTF(LOW, sns_fw_printf,
             "find best fit: has_start_config %d has_timeout_config %d periodic %d",
             req_config->has_start_config, req_config->has_timeout_config, req_config->is_periodic );
#endif

  *selected_config = (sns_timer_sensor_reg_event)sns_timer_sensor_reg_event_init_default;

  selected_config->start_time = req_config->start_time;
  selected_config->timeout_period = req_config->timeout_period;
  selected_config->is_periodic = req_config->is_periodic;
  if( req_config->has_is_dry_run )
  {
    selected_config->is_dry_run = req_config->is_dry_run;
  }
  else
  {
    selected_config->is_dry_run = sns_timer_sensor_config_is_dry_run_default;
  }
  if( req_config->has_priority )
  {
    selected_config->priority = req_config->priority;
  }
  else
  {
    selected_config->priority = sns_timer_sensor_config_priority_default;
  }

  // Synchronize timers with options
  if( req_config->has_start_config || req_config->has_timeout_config )
  {
    sns_timer_update_dry_run_timers(state, sns_get_system_time());

    // Requested config has either start or timeout options
    if( !req_config->is_periodic )
    {
      sns_timer_find_aperiodic_best_fit(state, req_config, selected_config);
    }
    else
    {
      sns_timer_find_periodic_best_fit(state, req_config, selected_config);
    }
  }
}


/*=============================================================================
  Sensor API functions
  ===========================================================================*/

/* ------------------------------------------------------------------------------------ */
static sns_rc
sns_timer_sensor_init(sns_sensor *const this)
{
  sns_rc rc = SNS_RC_FAILED;
  sns_timer_state *state = (sns_timer_state *)this->state->state;
  sns_timer_attr timer_attr;

  sns_memzero(state, sizeof(sns_timer_state));

  state->ro_buffer.sig_thread = sns_signal_thread_create(this, "sns_sig_timer",
    SNS_TIMER_THREAD_PRIO, sns_timer_process_signal_events, NULL, true);
  rc = sns_signal_register_signal(state->ro_buffer.sig_thread, NULL, &state->ro_buffer.signal);
  if(SNS_RC_SUCCESS == rc)
  {
    sns_isafe_list_init(&state->registered_timers);
    sns_isafe_list_init(&state->periodic_timers);
    sns_timer_attr_init(&timer_attr);
    sns_timer_attr_set_memory_partition(&timer_attr, SNS_OSA_MEM_TYPE_ISLAND);
    sns_timer_attr_set_periodic(&timer_attr, false);
    sns_timer_attr_set_deferrable(&timer_attr, false);
    sns_timer_create(sns_timer_sensor_callback, &state->ro_buffer, &timer_attr,
      &state->system_timer);

    // Create a virtual 1ms periodic timer for other timers to sync to.
    {
      sns_time now = sns_get_system_time();
      sns_isafe_list_iter iter;
      sns_sensor_instance *timer_inst =
        this->cb->create_instance(this, sizeof(sns_timer_instance_state));
      sns_timer_instance_state *virtual_timer =
        (sns_timer_instance_state *)timer_inst->state->state;
      virtual_timer->client_config =
        (sns_timer_sensor_reg_event)sns_timer_sensor_reg_event_init_default;
      virtual_timer->client_config.timeout_period =
        sns_convert_ns_to_ticks( TIMER_SYNC_INTERVAL_NS );
      virtual_timer->client_config.start_time =
        ((now / virtual_timer->client_config.timeout_period)+1) *
        (virtual_timer->client_config.timeout_period);
      virtual_timer->client_config.is_periodic = true;
      virtual_timer->client_config.is_dry_run = true;
      virtual_timer->trigger_time =
        virtual_timer->client_config.start_time +virtual_timer->client_config.timeout_period;
      sns_isafe_list_iter_init(&iter, &state->periodic_timers, true);
      sns_isafe_list_item_init(&virtual_timer->periodic_entry, timer_inst);
      sns_isafe_list_iter_insert(&iter, &virtual_timer->periodic_entry, false);
#ifdef SNS_TIMER_ENABLE_DEBUG
      SNS_PRINTF(LOW, sns_fw_printf, "Creating virtual timer timeout %x period %x",
                 (uint32_t)virtual_timer->trigger_time,
                 (uint32_t)virtual_timer->client_config.timeout_period);
#endif

      /*
      sns_isafe_list_iter_init(&iter, &state->registered_timers, true);
      sns_isafe_list_item_init(&virtual_timer->list_entry, timer_inst);
      sns_isafe_list_iter_insert(&iter, &virtual_timer->list_entry, false);
      */
    }

    sns_timer_sensor_publish_attributes(this);
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "signal registration fail");
  }

  return rc;
}

/* ------------------------------------------------------------------------------------ */
static sns_rc
sns_timer_sensor_deinit(sns_sensor *const this)
{
  sns_timer_state *state = (sns_timer_state *)this->state->state;
  sns_timer_stop(state->system_timer, NULL);
  sns_timer_delete(state->system_timer);
  sns_signal_thread_destroy(state->ro_buffer.sig_thread);
  return SNS_RC_SUCCESS;
}

/* ------------------------------------------------------------------------------------ */
SNS_SECTION(".text.sns")
static sns_sensor_uid const*
sns_timer_sensor_get_sensor_uid(sns_sensor const *this)
{
  UNUSED_VAR(this);
  return &sensor_uid;
}

/* ------------------------------------------------------------------------------------ */
SNS_SECTION(".text.sns")
static sns_sensor_instance*
sns_timer_set_client_request(sns_sensor *const this,
                             sns_request const *curr_req,
                             sns_request const *new_req,
                             bool remove)
{
  sns_timer_state *state =
    (sns_timer_state*)this->state->state;
  sns_std_request decoded_request = default_sns_std_request;
  sns_timer_sensor_config decoded_payload = default_sns_timer_sensor_config;

  if(remove)
  {
    sns_sensor_instance *inst =
      sns_sensor_util_find_instance(this, curr_req,
                                    this->sensor_api->get_sensor_uid(this));
    if(NULL != inst)
    {
      inst->cb->remove_client_request(inst, curr_req);
      if(NULL ==
         inst->cb->get_client_request(inst,
                                      sns_timer_sensor_get_sensor_uid(this),
                                      true))
      {
        sns_timer_sensor_delete_entry(state, inst, true, true);
        this->cb->remove_instance(inst);
      }
    }
    return inst;
  }
  else if( SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG == new_req->message_id &&
           sns_timer_get_decoded_req(new_req, &decoded_request, &decoded_payload) )
  {
    sns_rc rc;
    sns_sensor_instance *timer_inst = NULL;
    sns_sensor_instance *existing_inst =
      sns_sensor_util_find_instance(this, curr_req,
                                    this->sensor_api->get_sensor_uid(this));
    sns_timer_instance_state inst_state_to_match = {};
    sns_timer_instance_state *new_inst_state;

    if(decoded_payload.is_periodic && decoded_payload.timeout_period < 4800)
    {
      return NULL;
    }

    sns_timer_find_best_fit( state, &decoded_payload,
                             &inst_state_to_match.client_config );
    inst_state_to_match.trigger_time =
      inst_state_to_match.client_config.start_time +
      inst_state_to_match.client_config.timeout_period;

    if(NULL == existing_inst)
    {
      for( existing_inst = this->cb->get_sensor_instance( this, true );
           existing_inst != NULL;
           existing_inst = this->cb->get_sensor_instance( this, false ) )
      {
        sns_timer_instance_state *existing_inst_state =
          (sns_timer_instance_state *)(existing_inst->state->state);
        if( ( existing_inst_state->trigger_time ==
              inst_state_to_match.trigger_time ) &&
            ( existing_inst_state->client_config.timeout_period ==
              inst_state_to_match.client_config.timeout_period ) &&
            ( existing_inst_state->client_config.is_periodic ==
              inst_state_to_match.client_config.is_periodic ) &&
            ( existing_inst_state->client_config.is_dry_run ==
              inst_state_to_match.client_config.is_dry_run ) &&
            ( existing_inst_state->client_config.priority ==
              inst_state_to_match.client_config.priority ) )
        {
#ifdef SNS_TIMER_ENABLE_DEBUG
          SNS_PRINTF(LOW, sns_fw_printf, "Sharing existing instance");
#endif
          // This request can be handled by an existing instance.
          existing_inst->cb->add_client_request(existing_inst, new_req);
          return existing_inst;
        }
      }
      // No exisisting instance
      existing_inst =
        this->cb->create_instance(this, sizeof(sns_timer_instance_state));
    }
    else
    {
      existing_inst->cb->remove_client_request(existing_inst, curr_req);
      sns_timer_sensor_delete_entry(state, existing_inst, true, true);
    }
    timer_inst = existing_inst;

    if(NULL != timer_inst)
    {
      new_inst_state = (sns_timer_instance_state *)timer_inst->state->state;
      sns_memscpy( new_inst_state, sizeof(*new_inst_state),
                   &inst_state_to_match, sizeof(inst_state_to_match ) );

      timer_inst->cb->add_client_request(timer_inst, new_req);

      sns_request inst_req =
        { .message_id = SNS_TIMER_MSGID_SNS_TIMER_SENSOR_CONFIG,
          .request = &decoded_payload,
          .request_len = sizeof(sns_timer_sensor_config) };
      this->instance_api->set_client_config(timer_inst, &inst_req);
      sns_isafe_list_item_init(&new_inst_state->periodic_entry, timer_inst);
      rc = sns_timer_sensor_record_entry(state, timer_inst, true);
      if(SNS_RC_SUCCESS == rc)
      {
#ifdef SNS_TIMER_ENABLE_DEBUG
        SNS_PRINTF(LOW, sns_fw_printf, "Recorded timer: %x with result %d.",
                   (intptr_t)timer_inst, rc);
        SNS_PRINTF(LOW, sns_fw_printf, "req start %x actual start %x",
                   (uint32_t)decoded_payload.start_time,
                   (uint32_t)new_inst_state->client_config.start_time );
        SNS_PRINTF(LOW, sns_fw_printf, "req timeout %x actual timeout %x",
                   (uint32_t)decoded_payload.timeout_period,
                   (uint32_t)new_inst_state->client_config.timeout_period );
        SNS_PRINTF(LOW, sns_fw_printf, "nom req trigger %x actual trigger %x",
                   (uint32_t)(decoded_payload.start_time + decoded_payload.timeout_period),
                   (uint32_t)new_inst_state->trigger_time);

#endif
        // Generate timer registration event
        if(!pb_send_event(timer_inst, sns_timer_sensor_reg_event_fields,
                          &(new_inst_state->client_config), sns_get_system_time(),
                          SNS_TIMER_MSGID_SNS_TIMER_SENSOR_REG_EVENT, NULL))
        {
          SNS_PRINTF(ERROR, sns_fw_printf, "Timer registration event failure");
        }
      }
    }
    else
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Unsupported message_id %d", new_req->message_id);
    }
    return timer_inst;
  }

  return NULL;
}

/* ------------------------------------------------------------------------------------ */
SNS_SECTION(".text.sns")
static sns_rc
sns_timer_notify_event(sns_sensor *const this)
{
  UNUSED_VAR(this);
  return SNS_RC_SUCCESS;
}

sns_sensor_api timer_sensor_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_sensor_api),
  .init = &sns_timer_sensor_init,
  .deinit = &sns_timer_sensor_deinit,
  .get_sensor_uid = &sns_timer_sensor_get_sensor_uid,
  .set_client_request = &sns_timer_set_client_request,
  .notify_event = &sns_timer_notify_event,
};
