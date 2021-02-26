/*============================================================================
@file npa_scheduler.c

NPA Scheduled Requests

Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/scheduler/npa_scheduler.c#1 $
============================================================================*/
#include "npa_scheduler_internal.h"
#include "npa_resource.h"
#include "npa_remote_rpm_protocol.h"
#include "limits.h"
#include "CoreThread.h"
#include "CoreProperty.h"
#include "CoreString.h"
#include "timer.h"
#include "rcinit.h"
#include "sleep_npa_scheduler.h"

#ifndef WINSIM
#include "ClockDefs.h"
#include "VCSDefs.h"
#endif

#ifdef NPA_SCHEDULER_USES_SLEEP_OS_H
#define ADJUST_DEADLINE( x )  sleepNPAScheduler_adjustDeadline( x )
#define SET_DEADLINE( x )     sleepNPAScheduler_setDeadline( x )
#define CANCEL_DEADLINE       sleepNPAScheduler_cancelDeadline
#else
#define ADJUST_DEADLINE( x ) sleep_npa_scheduler_adjust_deadline( x )
#define SET_DEADLINE( x )    sleep_npa_scheduler_set_deadline( x )
#define CANCEL_DEADLINE      sleep_npa_scheduler_cancel_deadline
#endif

#ifdef WINSIM
#include "npa_offline.h"
#endif /* WINSIM */

/* NPA_SCHEDULER_SLEEP_LPR_SUPPORTED is defined in SConscript */
#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
int npa_sleep_lpr_supported = 1;
#else
int npa_sleep_lpr_supported = 0;
#endif

#define NPA_UPDATE_START_TIMES 1
#define NPA_DONT_UPDATE_START_TIMES 0
#define NPA_LOCK_LIST 1
#define NPA_DONT_LOCK_LIST 0
#define NPA_DO_COMPLETE_LIST 1
#define NPA_DONT_DO_COMPLETE_LIST 0

/**
 * Max total adjustment allowed.
 * Note - These should be much less than min_issue_sched_timeout and
 * min_reschedule_delta else system can get into a constant poking mode.
 */
uint32 NPA_SCHEDULER_MAX_TIMER_ADJUSTMENT = 9600;    /* 500us */
uint32 NPA_SCHEDULER_MAX_LATENCY_ADJUSTMENT = 9600;  /* 500us */
uint32 NPA_SCHEDULER_MAX_LPR_ADJUSTMENT = 9600;      /* 500us */

/**
 * @brief 
 * Limit adjustments to various backoffs by 1920 ticks at each instance
 * to prevent unusually large bad values from corrupting the adjustment.  
 */ 
#define NPA_SCHEDULER_CALCULATE_ADJUST(delta) ( MIN((delta), 1920) )

/** the core scheduler data structure */
npa_scheduler_control npa_scheduler;

#ifndef NPA_USES_CORE_PROPERTY
/* DEVCFG not defined so need to access config data directly */
extern const npa_scheduler_config npa_scheduler_config_data;
extern const char *npa_lpr_issuable_resource_names_data[];
extern const npa_resource_config npa_resource_overrides_data[];

#else
/* Define DEVCFG structure info for npa_config_data */
CorePropertyStructure npa_scheduler_config_data_property_struct[] =
{
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, log_buffer_size),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_queue_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_queue_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_request_allocation.initial),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_request_allocation.subsequent),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_request_workloop.num_events),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, scheduled_request_workloop.priority),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, min_issue_sched_timeout),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, sched_latency),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, min_reschedule_delta),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, fork_lookahead_delta),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, request_latency),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, fork_latency),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, notify_latency),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, sleep_wake_transition_latency),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, lpr_now_delta),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, auto_complete_delta),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, clk_baseline),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, implied_completes_on_wakeup),
  CORE_PROPERTY_STRUCTURE_ELEMENT(npa_scheduler_config, planB_implied_completes),
  {NULL, 0, 0} /* last entry */
};
#endif /* !NPA_USES_CORE_PROPERTY */

#ifndef WINSIM
/* adjustment for the delay to process NAS requests that could not fit
   in the RPM smd buffer and have to be processed on wakeup */
#define NPA_RPM_DEFERRED_NAS_DELAY_PER_REQUEST 8000

/* variable to determine how many requests could not be sent to the rpm */
extern unsigned npa_rpm_num_deferred_nas_updates;
#endif

static npa_client_handle 
npa_scheduler_remove( npa_client_handle client, uint32 update_start_times,
                      uint32 lock_list );

static void
npa_scheduler_sort_insert( npa_client_handle client,
                           npa_scheduler_queue_struct *list_ptr,
                           uint32 update_start_times );

static void npa_run_scheduler_notifies( void );

static void npa_setup_next_timeout( void );

const char* npa_scheduled_client_state_name( npa_scheduled_client_state state );

/**
 * <!-- npa_scheduler_mark_resource_lpr_issuable -->
 *
 * @brief Mark a resource as lpr issuable
 *
 * This is a callback that is registered for every resource in the 
 * lpr_issuable list.  When the resource is created this callback 
 * is called to set the NPA_RESOURCE_LPR_ISSUBALE attribute.
 *
 * @param context Unused
 * @param event_type Unused
 * @param data Resource name
 * @param data_size Size of resource name
 */
void npa_scheduler_mark_resource_lpr_issuable( void *context,
                                               unsigned int event_type,
                                               void *data,
                                               unsigned int data_size )
{
  npa_resource *resource;
  CORE_VERIFY_PTR( data );
  resource = npa_find_resource( (char *)data );
  CORE_VERIFY_PTR( resource );
  resource->definition->attributes |= NPA_RESOURCE_LPR_ISSUABLE;
}


/**
 * <!-- npa_scheduler_target_config -->
 *
 * @brief Get the per-target configuration data used by the NPA Scheduler
 *
 * If NPA_USES_CORE_PROPERTY is defined, then use Core's
 * GetProperty routines to get the data from the DEVCFG interface.
 *
 * If NPA_USES_CORE_PROPERTY is not defined, then the Sconscript
 * will have linked in the data directly into the NPA image so return
 * the data directly.
 *
 * @return Pointer to an npa_scheduler_config table
 */
static const npa_scheduler_config* npa_scheduler_target_config( void )
{
#ifdef NPA_USES_CORE_PROPERTY
  const npa_scheduler_config *config_data = NULL;
  static npa_scheduler_config final_data;
  int count;

  /* check to see if there is Target Specific config data */
  if ( CORE_PROPERTY_SUCCESS ==
         CoreProperty_GetPtr( "/dev/power/npa/target/config",
                              "scheduler_config_data",
                              (const void **)&config_data ) )
  { 
    CORE_VERIFY_PTR( config_data );
    /* have target specific data; this trumps all else */
    
    /* last entry should point to its self */
    CORE_VERIFY_PTR( config_data == config_data->table_end );

    ULOG_RT_PRINTF_1( npa.log,
          "npa_scheduler_config: "
          "Target Specific NPA Target Config structure found: 0x%08x",
          config_data );

    /* can pass back the original */
    return config_data;
  }
  else
  { /* Don't have Target specific config so get general config data */
    CoreProperty_GetPtr( "/dev/power/npa/config",
                         "scheduler_config_data",
                         (const void **) &config_data );

    /* make sure we have a valid pointer */
    CORE_VERIFY_PTR( config_data );

    /* last entry should point to its self */
    CORE_VERIFY_PTR( config_data == config_data->table_end );

    ULOG_RT_PRINTF_1( npa.log,
          "npa_scheduler_config: "
          "General NPA Target Config structure found: 0x%08x",
          config_data );

    /* copy across to a writable location */
    core_memcpy( &final_data, sizeof(final_data),
                 config_data, sizeof(final_data) );

    /* Are there any overrides to this data dictated by target code? */
    count 
      = CoreProperty_UpdateStructure( "/dev/power/npa/target/scheduler_config",
                                      npa_scheduler_config_data_property_struct,
                                      (void *) &final_data );

    if ( count > 0 && count <= CORE_PROPERTY_NO_ERROR )
    {
      ULOG_RT_PRINTF_1( npa_scheduler.log,
                        "npa_target_config: %d Individual NPA Target Config found",
                        count );
    }
  }

  final_data.table_end = &final_data; /* proper table termination */

  return &final_data;

#else /* DEVCFG not defined; return data pointer */
  /* last entry should point to its self */
  if ( &npa_scheduler_config_data != npa_scheduler_config_data.table_end )
  {
    ULOG_RT_PRINTF_0( npa_scheduler.log,
                      "npa_scheduler_config: Input does not match structure" );
    CORE_VERIFY( 0 );
  }

  return &npa_scheduler_config_data;
#endif
}


/**
 * <!-- npa_scheduler_setup_lpr_issuable_resources -->
 *
 * @brief Register resources to be marked as LPR issuable
 * 
 * @param issuable_names : Null terminated array of char pointers
 */
static void
npa_scheduler_setup_lpr_issuable_resources( const char **issuable_names )
{
  while ( *issuable_names )
  {
    ULOG_RT_PRINTF_1( npa_scheduler.log, "Marking \"%s\" LPR issuable",
                      *issuable_names );
    /* Add callback to set the LPR_ISSUABLE attribute on resource when
       it is available */
    npa_resource_available_cb( *issuable_names, 
                               npa_scheduler_mark_resource_lpr_issuable,
                               NULL );
    ++issuable_names;
  }
}


/**
 * <!-- npa_scheduler_lpr_issuable_config -->
 *
 * @brief Get the per-target configuration of LPR issuable resources and
 * mark them up as such.
 * 
 * If NPA_USES_CORE_PROPERTY is defined, then use Core's
 * GetProperty routines to get the data from the DEVCFG interface searching
 * for the regular config data and then appending in the target specific values
 * if they exist.
 *
 * If NPA_USES_CORE_PROPERTY is not defined, then the Sconscript
 * will have linked in the data directly into the NPA image so return
 * the data directly.
 */
static void npa_scheduler_lpr_issuable_config( void )
{
#ifdef NPA_USES_CORE_PROPERTY
  const char **issuable_names = NULL; /* Array of char *s */

  /* Get lpr_issuable_resource_names run time config data if exists */
  if ( CORE_PROPERTY_SUCCESS ==
         CoreProperty_GetPtr( "/dev/power/npa/config",
                              "lpr_issuable_resource_names",
                              (const void **)&issuable_names ) )
  { 
    ULOG_RT_PRINTF_0( npa_scheduler.log,
        "npa_init_scheduler: NPA LPR Issuable Resource Names found" );

    /* add unique names to the list */
    npa_scheduler_setup_lpr_issuable_resources( issuable_names );
  }

  /* Append the target specific config data if it exists */
  if ( CORE_PROPERTY_SUCCESS ==
         CoreProperty_GetPtr( "/dev/power/npa/target/config",
                              "lpr_issuable_resource_names",
                              (const void **)&issuable_names ) )
  {
    ULOG_RT_PRINTF_0( npa_scheduler.log,
        "npa_init_scheduler: Additional NPA LPR Issuable Resource Names found" );

    /* add unique names to the list */
    npa_scheduler_setup_lpr_issuable_resources( issuable_names );
  }

#else
  npa_scheduler_setup_lpr_issuable_resources(
                                   npa_lpr_issuable_resource_names_data );
#endif
}


/**
 * <!-- npa_scheduler_action_type_name -->
 *
 * @brief returns text for action type
 *
 * @param type : npa_scheduler_action_type to decode
 *
 * @return pointer to string of name of the action type.
 */
static const char*
npa_scheduler_action_type_name( npa_scheduler_action_type type )
{
  switch ( type )
  {
    case NPA_SCHEDULED_ACTION_TIMEOUT:  /* Timeout event has occured */
      return "NPA_SCHEDULED_ACTION_TIMEOUT";
  
    case NPA_SCHEDULED_ACTION_TRIGGER:  /* Run scheduled events now */
      return "NPA_SCHEDULED_ACTION_TRIGGER";

    case NPA_SCHEDULER_ACTION_LPR_WAKEUP:  /* Run scheduled events now */
      return "NPA_SCHEDULER_ACTION_LPR_WAKEUP";
  
    default: return "NPA_SCHEDULED_ACTION_UNKNOWN";
  }
}


/**
 * <!-- npa_update_start_time -->
 *
 * @brief Update the start time of each client in a linked list, starting
 * at the given client and working back towards the head. If do_complete_list
 * is not set, stop when we reach a client whose start_time doesn't change.
 *
 * @note Caller must lock list before calling this routine
 *
 * @param client : Client with which to start
 * @param do_complete_list : 0 means stop when we reach a client whoe
 *                           start time doesn't change
 *                           1 means stop when reach head of list.
 */
static void npa_update_start_time( npa_client_handle client,
                                   uint32 do_complete_list )
{
  npa_scheduler_data *request_ptr;
  uint64 end_time;
  uint64 start_time;

  /* update this client and all preceeding clients start times */
  while ( client )
  {
    CORE_VERIFY_PTR( request_ptr = client->request_ptr );

    end_time = request_ptr->request_time;

    if ( request_ptr->next &&
         end_time > request_ptr->next->request_ptr->start_time )
    { /* Next's end time is earlier */
      end_time = request_ptr->next->request_ptr->start_time;
    }

    /* calculate new start time */
    start_time = end_time - request_ptr->active_latency.wake;

    if ( start_time != request_ptr->start_time )
    { /* new start time so keep updating */
      request_ptr->start_time = start_time;
    }
    else if ( 0 == do_complete_list )
    { /* no change, so skip rest of list */
      break;
    }
        
    client = request_ptr->prev;
  }
}


/**
 * <!-- npa_issue_implied_complete -->
 *
 * @brief Issue an implied complete
 *
 * @note The caller must ensure this function is protected from
 * competing threads issuing new requests or otherwise operating on
 * this client. This can be done by running it from within the LPR,
 * from the scheduler workloop thread or by explicitly acquiring the
 * npa_scheduler.scheduler_work_lock beforehand.
 *
 * @param client : Client to issue the implied complete on/for
 */
static void npa_issue_implied_complete( npa_client_handle client )
{ 
  npa_scheduler_data *request_ptr;
  npa_work_request scheduled_request;

  CORE_VERIFY_PTR( request_ptr = client->request_ptr );

  if ( request_ptr->implied_state != NPA_IMPLIED_COMPLETE_PENDING )
  {
    return;
  }

  ULOG_RT_PRINTF_4( npa.log, "npa_scheduler_implied_complete "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(start_time: 0x%016llx)",
                    client, client->name,
                    ULOG64_LOWWORD( request_ptr->start_time ),
                    ULOG64_HIGHWORD( request_ptr->start_time ) );

  request_ptr->implied_state = NPA_IMPLIED_COMPLETE_IN_PROGRESS;
    
  /* save the scheduled request because we are about to issue another
     (the 0 or complete) on top of it */
  core_memcpy( &scheduled_request,
               sizeof(scheduled_request), 
               &(NPA_INCOMING_REQUEST(client)),
               sizeof(scheduled_request) );

  npa_terminate_request( client );

  /* restore the scheduled request */
  core_memcpy( &(NPA_INCOMING_REQUEST(client)),
               sizeof(scheduled_request), 
               &scheduled_request,
               sizeof(scheduled_request) );
  
  /* Since we issued the complete, anything that follows is assumed to be
     a new request */
  request_ptr->new_request = 1;

  /* Set the implied and notify states appropriately */
  request_ptr->implied_state = NPA_IMPLIED_COMPLETE_EXECUTED;
  request_ptr->notify_state = NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED;
 
  /* wake work is now the entire issue request again */
  request_ptr->active_latency.wake = 
               request_ptr->active_latency.request
               + request_ptr->active_latency.request_adjustment
               + request_ptr->active_latency.notify;

  /* Need to adjust the timeline from this client to first gap */
  npa_update_start_time( client, NPA_DONT_DO_COMPLETE_LIST );
}


/**
 * <!-- npa_run_scheduled_request -->
 *
 * @brief Executes the scheduled request for a specific client
 *
 * @param client : Handle of the client for the request.
 *
 * @param dur_ptr : Pointer to memory to fill with duration information
 */
static void npa_run_scheduled_request( npa_client_handle client,
                                       npa_duration *dur_ptr )
{
  npa_scheduler_data *request_ptr = NULL;
  uint64 issue_dur = 0;
  uint64 start_time;
  uint64 expected_duration = 0;

  request_ptr = client->request_ptr;

  request_ptr->state = NPA_SCHEDULED_CLIENT_IN_PROGRESS;

  if ( NPA_IMPLIED_COMPLETE_PENDING == request_ptr->implied_state)
  { /* Remove from the implied_completes_now list if present */
    npa_list_Remove( &npa_scheduler.implied_completes_now_list, client );
    /* Remove from the implied_completes_planB list, if present */
    npa_list_Remove( &npa_scheduler.implied_completes_planB_list, client );
  }

  /* Reset implied state as we are now executing the actual request */
  request_ptr->implied_state = NPA_NO_IMPLIED_COMPLETE;

  /* Restore the issue_request function pointer */
  client->issue_request = request_ptr->issue_request;
  request_ptr->issue_request = NULL; /* no danglers */

  if ( request_ptr->new_request )
  { 
    request_ptr->stats.last_issue_request_time
      = start_time = CoreTimetick_Get64();

    /* run the request */
    client->issue_request( client, request_ptr->new_request );

    issue_dur = npa_calc_duration( start_time, 
                                   CoreTimetick_Get64(),
                                   dur_ptr );

    /* expected duration is request latency plus last adjustment */
    expected_duration = request_ptr->active_latency.request
                        + request_ptr->active_latency.request_adjustment;

    /* Check to see if the duration was bigger than expected */
    if ( issue_dur > expected_duration )
    {
      /* Calculate a new adjustment */
      request_ptr->active_latency.request_adjustment 
        = MIN( request_ptr->active_latency.request_adjustment 
               + (uint32) NPA_SCHEDULER_CALCULATE_ADJUST(issue_dur-expected_duration),
               NPA_SCHEDULER_MAX_LATENCY_ADJUSTMENT );
    }
  }

  ULOG_RT_PRINTF_5( npa_scheduler.log,
                    "npa_run_scheduled_request (client: 0x%08x) "
                    "(issue_dur: %u) (expected_dur: %u) "
                    "(adjust: %u) (new_request: %d)",
                    client, (uint32)issue_dur, (uint32)expected_duration,
                    (uint32)request_ptr->active_latency.request_adjustment,
                    request_ptr->new_request
                  );
}


/**
 * <!-- npa_scheduler_notify -->
 *
 * @brief Handle the notify callback
 *
 * Clean up the client request data structures and then 
 * notify the client.  Must be called after the requests
 * has been handled.  If a lock is passed, it must be
 * "locked".
 *
 * @param client : Client to use for notification
 * @param lockptr : If not NULL, Lock to toggle off and then on.
 */
static void npa_scheduler_notify(npa_client_handle client,
                                 CoreMutex* lockptr)
{
  npa_scheduler_data *request_ptr;
  npa_scheduler_callback notify_ptr;
  npa_scheduled_notification_state notify_state;
  void *context;
  uint64 current_time;
  int64 delta;
  uint64 request_time;

  request_ptr = client->request_ptr;

  /* remember whom we need to notify */
  notify_ptr = request_ptr->notification;

  /* remember context */
  context = request_ptr->context;

  /* no dangling pointers but haven't called notification yet. */
  request_ptr->issue_request = NULL;
  request_ptr->notification = NULL;

  /* we are done with this request */
  request_ptr->state = NPA_SCHEDULED_CLIENT_INACTIVE;
  request_ptr->implied_state = NPA_NO_IMPLIED_COMPLETE;

  /* remember the request time because it could change before
   * the logging is printed out at the bottom.
   */
  request_time = request_ptr->request_time;

  /* update if request is done in time */
  request_ptr->stats.last_notification_time
    = current_time = CoreTimetick_Get64();
  delta = npa_compare_times(request_ptr->request_time,current_time);
  if (0 <= delta
      || (0 == NPA_ACTIVE_REQUEST( client ).state))
  { /* current time is < request time or it
     * is a cancel/complete that is suppose to happen
     * after the request time so "on time"
     */
    notify_state = NPA_SCHEDULED_NOTIFICATION_ON_TIME;
    ++request_ptr->stats.on_time;
    ++npa_scheduler.stats.on_time;
  }
  else
  { /* current time is > requested time so late */
    notify_state = NPA_SCHEDULED_NOTIFICATION_LATE;
    delta = -delta; /* invert sign */
    ++request_ptr->stats.late;
    ++npa_scheduler.stats.late;
    request_ptr->stats.late_by_sum
      += current_time - request_ptr->request_time;
  }

  request_ptr->notify_state = notify_state;

  if (NULL != notify_ptr)
  { /* valid notification function so call it */
    /* remember the time for duration */
    current_time = CoreTimetick_Get64();
    request_ptr->stats.last_notification_time = current_time;
    if (NULL != lockptr)
    { /* Have a lock we need to toggle */
      Core_MutexUnlock(lockptr);
      notify_ptr(client, request_ptr->notify_state, context);
      Core_MutexLock(lockptr);
    }
    else
    { /* No lock to toggle */
      notify_ptr(client, request_ptr->notify_state, context);
    }
    /* remember duration */
    npa_calc_duration(current_time, 
                      CoreTimetick_Get64(),
                      &request_ptr->stats.notify_issue_request_duration);

    /* remember the time we did a notification */
    request_ptr->stats.last_notification_time = current_time;

    ULOG_RT_PRINTF_9( npa_scheduler.log, "\tDONE npa_scheduler_notify: "
                      "(client: 0x%08x) (client_name: \"%s\") "
                      "(notify: 0x%08x) "
                      "(request_time: 0x%016llx) "
                      "(active request: %d) "
                      "(\"%s\") "
                      "(delta: %lld)",
                      client, client->name,
                      notify_ptr,
                      ULOG64_LOWWORD(request_time),
                      ULOG64_HIGHWORD(request_time),
                      NPA_ACTIVE_REQUEST( client ).state,
                      npa_scheduler_status_name(notify_state),
                      ULOG64_LOWWORD(delta),
                      ULOG64_HIGHWORD(delta));
  }
}

/**
 * <!-- npa_listfn_issue_implied_complete -->
 *
 * @brief A npa_list_Apply wrapper around issue_implied_complete
 *
 * @param p: Cast into the client handle to issue the implied_complete
 */
static void npa_listfn_issue_implied_complete( void* p )
{
  npa_issue_implied_complete( (npa_client_handle)p );
}

/**
 * <!-- npa_dispatch_next_awake_callback -->
 *
 * @brief Dispatches (invokes) a registered next_awake callback
 */
static void npa_dispatch_next_awake_callback( void *p )
{
  npa_next_awake_callback *nac = (npa_next_awake_callback *)p;
  CORE_VERIFY_PTR( nac->func );

  nac->func( nac->data );

  Core_PoolFree( &npa.next_awake_callback_pool, nac );
}

/**
 * <!-- npa_run_scheduler_requests -->
 *
 * @brief Run pending requests
 *
 * Takes the current pending sorted scheduled request list and runs
 * the requests quest starting at the head.  It will run until the list
 * is either empty or a request is npa_max_reschedule_delta in the future.
 *
 * It will calculate a new timeout and setup another timer
 * when it has reached either point.
 *
 * @param action : Information about signal causing the function to be called
 */
void npa_run_scheduler_requests( npa_scheduler_action *action )
{
  uint64 start_time = CoreTimetick_Get64();
  uint64 func_duration;
  npa_client_handle client = NULL;
  npa_scheduler_data *request_ptr = NULL;
  int64 late_by;
  const char *adjective;

  uint64 max_handle_time;
  uint64 last_request_time;

  /* prevent pokes or timeout setup since scheduler is active */
  npa_scheduler.next_timeout_state = NPA_SCHEDULER_NTS_NO_SETUP;

  /* remember time started */
  npa_scheduler.stats.last_run_scheduler_request_time = start_time;

  if ( NPA_SCHEDULED_ACTION_TIMEOUT == action->type
       || NPA_SCHEDULER_ACTION_LPR_WAKEUP == action->type )
  { /* Running because of timeout or LPR wakeup */

    /* time lag between now and when the timer fired */
    npa_calc_duration(npa_scheduler.timer.timedout_at,
                      start_time,
                      &npa_scheduler.stats.timeout_to_scheduler_duration);
  
    /* how "on time" we are? */
    late_by = npa_compare_times(start_time,npa_scheduler.next_scheduler_time);

    if (late_by > 0)
    { /* we are late so count it */
      uint64 adjust;
      npa_calc_duration(npa_scheduler.next_scheduler_time,
                        start_time,
                        &npa_scheduler.stats.run_requests_late_duration);
      adjective = "LATE";
  
      /* Want to calculate an adjustment for the next timeout/wakeup
       * event but want to limit it in case there is one bad large
       * value 
       */ 
      adjust = NPA_SCHEDULER_CALCULATE_ADJUST(late_by);

      if (NPA_SCHEDULED_ACTION_TIMEOUT == action->type)
      { /* Timer notification was late */
        npa_scheduler.adjust.timer
          = MIN(npa_scheduler.adjust.timer + adjust,
                NPA_SCHEDULER_MAX_TIMER_ADJUSTMENT);
      }
      else if (NPA_SCHEDULER_ACTION_LPR_WAKEUP == action->type)
      { /* LPR notification was late */
        npa_scheduler.adjust.lpr
          = MIN(npa_scheduler.adjust.lpr + adjust,
                NPA_SCHEDULER_MAX_LPR_ADJUSTMENT);
      }
    }
    else 
    { /* event is early */

      /* get absolute value for printout */
      late_by = ABS(late_by);

      if (late_by > npa_scheduler.config_ptr->auto_complete_delta)
      { /* running very early */
        npa_calc_duration(start_time,
                          npa_scheduler.next_scheduler_time,
                          &npa_scheduler.stats.run_requests_very_early_duration);
        adjective = "VERY EARLY";
      }
      else
      { /* just ahead of on time */
        npa_calc_duration(start_time,
                          npa_scheduler.next_scheduler_time,
                          &npa_scheduler.stats.run_requests_early_duration);
        adjective = "EARLY";
      }
    }
  }
  else
  { /* probably a poke so not late */
    late_by = 0;
    adjective = "";
  }

  ULOG_RT_PRINTF_9( npa_scheduler.log, "START npa_run_scheduler_requests %d "
                    "(action type: 0x%08x - \"%s\") "
                    "(start vs. desired: \"%s\" %d) "
                    "(timedout_at: 0x%016llx) "
                    "(req head: 0x%08x) "
                    "(fork head: 0x%08x)", 
                    npa_scheduler.stats.num_run_scheduler_requests,
                    action->type,
                    npa_scheduler_action_type_name(action->type),
                    adjective,
                    (uint32) late_by,
                    ULOG64_LOWWORD(npa_scheduler.timer.timedout_at),
                    ULOG64_HIGHWORD(npa_scheduler.timer.timedout_at),
                    npa_scheduler.request_queue.head,
                    npa_scheduler.forked_queue.head);

#ifndef WINSIM /* WINSIM plays with this bit too much to trust it in sim */
  /* must not be in sleep */
  CORE_VERIFY(npa_scheduler.sleep.enter == 0);
#endif

#ifndef WINSIM
  if ( NPA_SCHEDULER_ACTION_LPR_WAKEUP == action->type )
  {
    /* Dispatch NEXT_AWAKE callbacks */
    npa_list_ApplyAndClear( &npa_scheduler.next_awake_callbacks, 
                            npa_dispatch_next_awake_callback );
  }
#endif

  /* Request CPU clock at the floor specified in the config */
  if ( npa_scheduler.clk_cpu_client )
  {
    npa_issue_suppressible_request( npa_scheduler.clk_cpu_client,
                                    npa_scheduler.clk_cpu_floor );
  }

  /* Any notifies pending */
  npa_run_scheduler_notifies();

  /* There is a narrow window that is open below if a client is destroyed
   * in the middle of removing it off the list and locking it.  Since
   * we don't want to block other clients from doing list work while
   * a request is being processed, we might want a specific lock for
   * destroying any client, but since this should be a very rare condition,
   * it is ok to have it locked for a long time.  
   */
  Core_MutexLock(npa_scheduler.scheduler_work_lock);

  /* Mark the last request time as the current time so we have something in
     a reasonable range */
  last_request_time = CoreTimetick_Get64();
  
  /* The maximum handle time is the current time plus the deltas */ 
  max_handle_time = last_request_time 
                    + npa_scheduler.config_ptr->min_reschedule_delta
                    + npa_scheduler.config_ptr->sched_latency
                    + npa_scheduler.adjust.timer;  

  /* If the scheduler is not running at wakeup OR
     (running at wakeup AND there is something on the request queue AND
      start_time of the client at the head of the request queue is within
      max_handle_time, process the request queue */
  if (action->type != NPA_SCHEDULER_ACTION_LPR_WAKEUP ||
      (action->type == NPA_SCHEDULER_ACTION_LPR_WAKEUP &&
       npa_scheduler.request_queue.head &&
       npa_scheduler.request_queue.head->request_ptr->start_time <= 
         max_handle_time))
  {
    for ((client = npa_scheduler_remove(npa_scheduler.request_queue.head,
                                        NPA_DONT_UPDATE_START_TIMES,
                                        NPA_LOCK_LIST));
          NULL != client;
          (client = npa_scheduler_remove(npa_scheduler.request_queue.head,
                                         NPA_DONT_UPDATE_START_TIMES,
                                         NPA_LOCK_LIST)))
    {
      CORE_VERIFY_PTR( request_ptr = client->request_ptr );

      /* TODO: Not sure why we check this at all; must be always true? */
      if (NPA_SCHEDULED_CLIENT_UNLINKED == request_ptr->state)
      { /* it is ours to use */

       /* If too early for request, put back in list, else execute */
        if (request_ptr->start_time > max_handle_time
            && request_ptr->request_time != last_request_time)
        {
          npa_scheduler_sort_insert(client, 
                                    &npa_scheduler.request_queue,
                                    NPA_UPDATE_START_TIMES);
          break; /* done as everything else on the list needs to run later */
        }
        else
        { /* handle request */
          ++npa_scheduler.stats.sched_requests_handled;

          /* remember this one's time for next compare */
          last_request_time = request_ptr->request_time;

          /* run the request */
          npa_run_scheduled_request(client,
                                    &client->request_ptr->stats.full_issue_request_duration);

          /* issue notification inside the lock */
          npa_scheduler_notify(client, NULL);
        }
        
        /* The maximum handle time is the current time plus the deltas */ 
        max_handle_time = CoreTimetick_Get64() 
                          + npa_scheduler.config_ptr->min_reschedule_delta
                          + npa_scheduler.config_ptr->sched_latency
                          + npa_scheduler.adjust.timer;
      }
    } /* for (client...) */
  }

  /* If blocking timer, release it */
  if ( npa_scheduler.timer.deferred_match_count )
  {
    CORE_VERIFY( 0 == (--npa_scheduler.timer.deferred_match_count) );
    timer_undefer_match_interrupt();
  }

  /* Execute implied_completes that were deferred from last sleep to wakeup.
   * The list may be empty if there were no implied_completes to execute or
   * if the implied_completes_on_wakeup flag in the config is 0
   */
  if (action->type == NPA_SCHEDULER_ACTION_LPR_WAKEUP)
  {
    npa_list_ApplyAndClear(&npa_scheduler.implied_completes_on_wakeup_list,
                           npa_listfn_issue_implied_complete);
  }

  if (npa_scheduler.complete_queue.head)
  {
    ULOG_RT_PRINTF_3( npa_scheduler.log,
                      "\tnpa_run_scheduler_requests Check Complete Queue "
                      "(client: 0x%08x) "
                      "(start_time: 0x%016llx)",
                      npa_scheduler.complete_queue.head,
                      ULOG64_LOWWORD(npa_scheduler.complete_queue.head->request_ptr->start_time),
                      ULOG64_HIGHWORD(npa_scheduler.complete_queue.head->request_ptr->start_time));

    /* Check Complete Queue 
     * TODO - If there are many completes to handle, it may start eating
     * into the period needed for the request_queue.  Need some way to 
     * recheck if a request queue request needs to fire.
     */
    for ((client = npa_scheduler_remove(npa_scheduler.complete_queue.head,
                                          NPA_DONT_UPDATE_START_TIMES,
                                          NPA_LOCK_LIST));
          NULL != client;
          (client = npa_scheduler_remove(npa_scheduler.complete_queue.head,
                                          NPA_DONT_UPDATE_START_TIMES,
                                          NPA_LOCK_LIST))
         )
    {
      request_ptr = client->request_ptr;
      if (CoreTimetick_Get64() >= request_ptr->request_time)
      { /* Current time is after the start time so OK to run */
        /* run the request. */
        npa_run_scheduled_request(client,
                                  &request_ptr->stats.full_issue_request_duration);

        /* issue notification inside the lock. */
        npa_scheduler_notify(client, NULL);
      }
      else
      { /* Too early so put back on list
         * Don't need to update start times, just need it to be
         * ordered.
         */
        npa_scheduler_sort_insert(client, 
                                  &npa_scheduler.complete_queue,
                                  NPA_DONT_UPDATE_START_TIMES);
        /* Insert overwrites the start time so put back to legal value */
        request_ptr->start_time = request_ptr->request_time;
        break;  /* done with the list */
      }
    }
  }
  
  /* scheduler can be poked again and timeouts setup, 
   * including by the following npa_setup_next_timeout()
   * so no race condition exists.  Worst case is one
   * unneeded poke occurs for a very small race window compared
   * to blocking a lot of outer tasks until this loop is done.
   */
  npa_scheduler.next_timeout_state = NPA_SCHEDULER_NTS_OPEN;

  /* Setup timer that may have changed */
  npa_setup_next_timeout();
            
  /* done with main workloop.  The rest can be interrupted */
  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  /* calculate function duration and record max */
  func_duration = npa_calc_duration(start_time,
                                    CoreTimetick_Get64(),
                                    &npa_scheduler.stats.run_requests_duration);

  ULOG_RT_PRINTF_4( npa_scheduler.log, "END npa_run_scheduler_requests %d "
                    "(function_duration 0x%08x) "
                    "(req head: 0x%08x) "
                    "(fork head: 0x%08x)", 
                    npa_scheduler.stats.num_run_scheduler_requests++,
                    (uint32)func_duration,
                    npa_scheduler.request_queue.head,
                    npa_scheduler.forked_queue.head);

  /* done with the clock */
  if (npa_scheduler.clk_cpu_client)
  { 
    npa_complete_request(npa_scheduler.clk_cpu_client);
  }
}


/**
 * <!-- npa_run_scheduler_forks -->
 *
 * @brief
 *
 * @param wakeup_time: information about signal causing the function to be called
 */
void npa_run_scheduler_forks(uint64 wakeup_time)
{
  npa_client_handle client;
  npa_client_handle next_client;
  npa_scheduler_data *request_ptr;
  uint64 absolute_time = 0;
  uint64 max_handle_time;
  uint64 current_time;
  uint32 tell_sleep = 0;
  uint32 num_forked = 0;
  uint32 num_fork_short_circuit = 0;
  uint32 num_implied_completes = 0;
  uint32 max_time_adjusted = 0;
  int64 due_time_delta = 0;
  int64 timer_adjust = 0;

  /* must be in sleep and forked queue must be empty.
   * This could be caused by being called by something other than
   * npa_scheduler_lpr_enter() or npa_scheduler_lpr_exit(), was either
   * not run, or it ran but the notify didn't happen before sleep
   * called the enter function again.
  */
  CORE_VERIFY( npa_scheduler.sleep.enter && 
               npa_scheduler.forked_queue.head == NULL );
  
  /* get first client of request list */
  client = npa_scheduler.request_queue.head;
  
  /* Request queue should not be empty. The LPR should have been turned
     off and Sleep should not have called enter function */
  CORE_VERIFY_PTR( client );

  /* End time for finding requests to run in the LPR is sleep
   * wake time fork lookahead delta.  This will find the
   * first request that is due close to the exit time.
   * If one is found in the loop, then use the completion time
   * of the first one found plus reschedule time.
   * This will find all the requests that are grouped together
   * and run them all in the LPR.
   *
   * Need to also take into account the timer queue latency as sleep
   * has been told to account for this latency too and if it is bigger
   * than fork lookahead delta, sleep will be waking up earlier for us
   * but we won't fork since it is too early.
   */
  max_handle_time = wakeup_time
                    + npa_scheduler.config_ptr->fork_lookahead_delta
                    + npa_scheduler.config_ptr->timer_queue_latency;

  /* Issue request to the "/protocol/rpm/polling_mode" resource to
     switch the NPA remoting driver into polling mode */
  npa_issue_required_request( npa_scheduler.rpm_polling_mode_client, 1 );

  do
  { /* for all clients check if should fork or issue implied complete */
    request_ptr = client->request_ptr;
    next_client = request_ptr->next; /* remember next one because this one
                                      * may come off the queue.
                                      */

    due_time_delta = npa_compare_times(request_ptr->start_time, 
                                       max_handle_time);

    if (due_time_delta <= 0
        && _npa_resource_has_attribute(client->resource, 
                                       NPA_RESOURCE_LPR_ISSUABLE))
    { /* It is due at wakeup and forkable, so handle it */
      npa_client_handle c;

      ULOG_RT_PRINTF_3( npa.log, "\tNPA Scheduled Request LPR "
                        "(client: 0x%08x) (client_name: \"%s\") "
                        "(new_request: %d)",
                        client, client->name,
                        request_ptr->new_request);

      if (!max_time_adjusted)
      { /* Adjust the max_handle_time once to be based upon the completion
         * time of this first Forkable request. This is so all request that
         * end around this time get done together.
         */
        uint64 adjusted_handle_time
          = request_ptr->request_time 
            + npa_scheduler.config_ptr->min_reschedule_delta;

        uint64 delta_handle_time;

        delta_handle_time = npa_compare_times(adjusted_handle_time,
                                              max_handle_time);

        if (delta_handle_time > 0)
        { /* adjusted_handle_time is farther out so use it. */
          max_handle_time = adjusted_handle_time;
        }

        /* do this only once */
        max_time_adjusted = 1;
      }

      /* Peek ahead into the request queue for requests that we will fork
       * (i.e. whose start_time is <= max_handle_time) and determine if
       * one of them is to the same resource as this client. If yes, set
       * attributes on both this client and the "match" to inform the
       * resource that multiple requests are scheduled for next wakeup.
       */
      c = next_client;
      while ( c && c->request_ptr->start_time <= max_handle_time )
      {
        if ( c->resource == client->resource )
        {
          npa_set_request_attribute( client, NPA_REQUEST_MULTIPLE_NEXT_AWAKE );
          npa_set_request_attribute( c, NPA_REQUEST_MULTIPLE_NEXT_AWAKE );
          break;
        }
        c = c->request_ptr->next;
      }

      /* take it off the request list.  We will put it the
       * forked list below.
       */
      npa_scheduler_remove(client,
                           NPA_DONT_UPDATE_START_TIMES,
                           NPA_LOCK_LIST);

      /* put back the original request so no dangling pointer to
       * scheduler routines and in case other routines use 
       * this field.
       */
      client->issue_request = request_ptr->issue_request;
        
      if (request_ptr->new_request)
      { /* issue only if it is a new request */
        npa_work_request pending_request;

        /* mark that we are forking.  npa_scheduler_sort_insert()
         * will put back on the list with the proper state.
         */
        request_ptr->state = NPA_SCHEDULED_CLIENT_FORKING;

        ++npa_scheduler.stats.forked;
        ++request_ptr->stats.forked;
        ++num_forked;

        /* Save the pending request and after issue restore as pending
         * request. This is to allow the implied complete logic to correctly
         * schedule the pending request later, if it needed to apply the
         * complete
         */
        core_memcpy(&pending_request, 
                    sizeof(pending_request),
                    &(NPA_PENDING_REQUEST(client)),
                    sizeof(pending_request));

        /* Indicate to the resource that this request applies at next wakeup */
        npa_set_request_attribute(client, NPA_REQUEST_NEXT_AWAKE);

        /* time duration of the issue request */
        request_ptr->stats.last_issue_fork_time
          = current_time = CoreTimetick_Get64();

        /* run the request */
        client->issue_request(client, 1);

        npa_calc_duration(current_time,
                          CoreTimetick_Get64(),
                          &request_ptr->stats.fork_issue_request_duration);

        /* Restore the pending request structure; this is only useful when
         * implied completes apply
         */
        core_memcpy(&(NPA_PENDING_REQUEST(client)),
                    sizeof(pending_request),
                    &pending_request,
                    sizeof(pending_request));

        /* If it had an executed implied complete, set it back to pending
         * in case there is another early wakeup.
         */
        if (NPA_IMPLIED_COMPLETE_EXECUTED == request_ptr->implied_state)
        { /* move back to pending */
          request_ptr->implied_state = NPA_IMPLIED_COMPLETE_PENDING;
        }
      }
      else
      { /* short circuiting since not a new request */     
        ++npa_scheduler.stats.fork_short_circuits;
        ++request_ptr->stats.fork_short_circuits;
        ++num_fork_short_circuit;
      }

      /* wake work is now just notify work
       * Must update before putting on the list so the
       * list's work time is correct.
       */ 
      request_ptr->active_latency.wake = request_ptr->active_latency.notify;

      /* put in forked list */
      npa_scheduler_sort_insert(client, 
                                &npa_scheduler.forked_queue, 
                                NPA_DONT_UPDATE_START_TIMES);
      
      /* npa_scheduler_sort_insert sets state to LINKED, change
       * to show it is expecting to notified
       */
      request_ptr->state = NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED;
    }
    else if ((due_time_delta > 0)
             && NPA_IMPLIED_COMPLETE_PENDING == request_ptr->implied_state)
    { /* implied complete is needed as request won't fire when we wake */

      /* remove from implied complete list */
      npa_list_Remove(&npa_scheduler.implied_completes_now_list, client);
      
      if (npa_scheduler.config_ptr->implied_completes_on_wakeup == 1)
      {
        npa_list_PushFront(&npa_scheduler.implied_completes_on_wakeup_list,
                           client );
      }
      else
      {
        /* Indicate to the resource that this request applies at next wakeup */
        npa_set_request_attribute(client, NPA_REQUEST_NEXT_AWAKE);

        /* it is still on request queue so just issue implied complete.
         * The amount of work should stay the same as the request queue
         * always has the worst case amount of work to do. 
         */ 
        npa_issue_implied_complete(client);

        ++num_implied_completes;
      }
    }
    client = next_client; /* next item in original list */
  } while(client != NULL);

  /* Complete request to the "/protocol/rpm/polling_mode" resource to
     switch the NPA remoting driver back into barrier mode */
  npa_complete_request( npa_scheduler.rpm_polling_mode_client );
  
  if (npa_scheduler.forked_queue.num_linked_clients)
  { /* put something on the list so things changed.
     * Note that it may have went on the lists but it was not sent
     * to the RPM since it was not new so we need to sort even though
     * the request_started is false
     */
    /* Because things may have come out in-between items, we
     * need to scan the whole list.
     */
    npa_update_start_time(npa_scheduler.request_queue.tail, 
                          NPA_DO_COMPLETE_LIST);

    /* make sure start times for forked requests are correct too. 
     * Since this is first time to touch them in this list, do
     * the complete list
     */
    npa_update_start_time(npa_scheduler.forked_queue.tail,
                          NPA_DO_COMPLETE_LIST);

  
    /* Need to check if the next timeout should be driven by the 
     * request queue or the forked queue as the timeout has to occur
     * early enough to meet the request times of the clients in the
     * forked queue too.
     */
    if (NULL != npa_scheduler.request_queue.head)
    { /* something in request queue so base time on it */
      /* absolute start time is start time from head of queue
       * minus the notify work in the forked queue
       */
      absolute_time 
        = npa_scheduler.request_queue.head->request_ptr->start_time
          - npa_scheduler.forked_queue.sum_wake_work;
  
      if (NULL != npa_scheduler.forked_queue.head /* Klocworks */
          && NULL != npa_scheduler.forked_queue.head->request_ptr
          && (absolute_time > npa_scheduler.forked_queue.head->request_ptr->start_time))
      { /* have forked requests too and they start earlier than the request's
         * earliest start minus the work to do for the notify work so use
         * this time instead.
         */
        absolute_time 
          = npa_scheduler.forked_queue.head->request_ptr->start_time;
      }
  
      /* tell sleep but not timer */
      tell_sleep = 1;
    }
    else if (NULL != npa_scheduler.forked_queue.head->request_ptr)
    { /* just work in the forked queue */
  
      absolute_time 
        = npa_scheduler.forked_queue.head->request_ptr->start_time;
  
      tell_sleep = 1;
    }
    /* else not sure why nothing is in the list, but just ignore.
     * timer will fire when we wake up and solve the issue.
     */

    /* tell sleep */
    if (npa_scheduler.sleep.lpr_client
        && npa_sleep_lpr_supported
        && tell_sleep)
    { /* have a request in NAS so need to tell sleep about the time change */
      int64 request_delta = 0;
      time_timetick_type timer_expire_delta = 0;

      /* remember when need to start run_scheduler_requests by */
      npa_scheduler.next_scheduler_time = absolute_time;

      /* account for scheduler latency, sleep wake time, and 
       * runtime lpr adjustment
       */
      absolute_time 
        -= npa_scheduler.config_ptr->sched_latency
           + npa_scheduler.config_ptr->sleep_wake_transition_latency
           + npa_scheduler.adjust.lpr;
#ifndef WINSIM
      absolute_time 
        -= npa_rpm_num_deferred_nas_updates * NPA_RPM_DEFERRED_NAS_DELAY_PER_REQUEST;
#endif

      /* test for timer needing to fire before the NPA Scheduler
       * task is done upon wakeup
       */
      timer_expire_delta = timer_get_time_till_first_non_deferrable_timer_64(NULL);

      if (0 != timer_expire_delta)
      { /* timer has an expire value in the future */
        current_time = CoreTimetick_Get64();
        /* find time until tail's request time */
        request_delta
          = npa_compare_times(npa_scheduler.forked_queue.tail->request_ptr->request_time,
                              current_time);

        /* difference between tail's request time and timer needing to fire */
        timer_adjust = npa_compare_times(request_delta,
                                         timer_expire_delta);

        if (timer_adjust > 0)
        { /* timer is due to fire before finished with requests
           * so adjustment is needed.
           */
          absolute_time -= timer_adjust;
          npa_scheduler.next_scheduler_time -= timer_adjust;

          /* stats on how much we change */
          npa_calc_duration(timer_adjust, 0,
                            &npa_scheduler.stats.nas_timer_adjustment);
        }
      }

      /* Tell sleep new wake time */
      ADJUST_DEADLINE(absolute_time);

      /* This is the time we expect to wake up by */
      npa_scheduler.timer.current_timeout = absolute_time;
    }
  }

  ULOG_RT_PRINTF_8( npa_scheduler.log, "DONE npa_run_scheduler_forks "
                    "(current_timeout:0x%016llx) "
                    "(max_handle_time: 0x%016llx) "
                    "(sleep_wakeup_time: 0x%016llx) "
                    "(timer_adjust: 0x%08x) "
                    "(tell_sleep: %d)",
                    ULOG64_LOWWORD(npa_scheduler.timer.current_timeout),
                    ULOG64_HIGHWORD(npa_scheduler.timer.current_timeout),
                    ULOG64_LOWWORD(max_handle_time),
                    ULOG64_HIGHWORD(max_handle_time),
                    ULOG64_LOWWORD(wakeup_time),
                    ULOG64_HIGHWORD(wakeup_time),
                    (int32) timer_adjust,
                    tell_sleep);
  ULOG_RT_PRINTF_3( npa_scheduler.log,
                    "       (num_forked: %d)"
                    " (num_fork_short_circuit: %d)"
                    " (num_implied_completes: %d)",
                    num_forked,
                    num_fork_short_circuit,
                    num_implied_completes);
}

/**
 * <!-- npa_notify_next_awake -->
 *
 * @brief Register a callback with the scheduler that will be invoked after
 * wakeup, before scheduled client notifications. This interface is intended
 * for resource authors who special-process requests with the NEXT_AWAKE
 * attribute and require notifications at wakeup to complete said processing.
 * The callbacks will be invoked in LIFO order (reflecting assumed resource
 * dependencies) and there will be one callback for each invocation of the
 * below API.
 */
void npa_notify_next_awake( npa_simple_callback func, void *data )
{
  npa_next_awake_callback *nac = Core_PoolAlloc( &npa.next_awake_callback_pool );
  CORE_VERIFY_PTR( nac );

  nac->func = func;
  nac->data = data;

  npa_list_PushFront( &npa_scheduler.next_awake_callbacks, nac );
}

/**
 * <!-- npa_is_next_wakeup_after_nas -->
 *
 * @brief Returns TRUE if npa_scheduler.sleep.enter == 0. This global is set to 1
 * before the scheduler issues NAS requests and reset to 0 at the end of the
 * LPR exit routine on next wakeup. Since the scheduler is the only entity
 * that issues NAS requests, resource authors may be able to use this API
 * to distingush whether a non-NAS (or non-NEXT_AWAKE) request received
 * after a NAS request (but before NEXT_AWAKE callbacks) is after next
 * wakeup (legal) or not.
 */
unsigned int npa_is_next_wakeup_after_nas()
{
  return ( npa_scheduler.sleep.enter == 0 );
}

/**
 * <!-- npa_run_scheduler_notifies -->
 * 
 * @brief Handle all pending scheduler notifies.
 *
 */
static void npa_run_scheduler_notifies(void)
{
  npa_client_handle client;
  npa_scheduler_data *request_ptr = NULL;
  uint64 latest_handle_time = 0;
  int did_implied_complete = 0;
  int64 delta = 0;

  if (NULL == npa_scheduler.forked_queue.head)
  {
    return;
  }

  /* Notifies can be called by the scheduler and client threads
   * so we need to protect it.  Note that since only things 
   * that hold this lock also only touch the fork queue, 
   * we don't need to lock the queue. 
   */
  Core_MutexLock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_2( npa_scheduler.log, "\tnpa_run_scheduler_notifies "
                    "(forked head: 0x%0x) "
                    "(forked count: %d)",
                    npa_scheduler.forked_queue.head,
                    npa_scheduler.forked_queue.num_linked_clients);
  
  /* there is a race condition where we didn't get
   * the lock until after another task already did the work
   * so check to see if anything still needs to be done.
   */
  if (npa_scheduler.forked_queue.num_linked_clients)
  { /* have something to do */

    /* calculate a latest handle time that are valid for
     * this wakeup time. If start times are later than that, we need to 
     * handle the implied completes to turn off the last NAS as it is not
     * needed.
     */
    latest_handle_time = CoreTimetick_Get64()
                         + npa_scheduler.config_ptr->auto_complete_delta;

    /* See how soon the first forked request is.  Need to do this only once
     * as if it is too early for the first, it is too early for all the
     * rest
     */
    delta 
      = npa_compare_times(latest_handle_time, 
                  npa_scheduler.forked_queue.head->request_ptr->request_time);

    do
    { /* loop and decide if need to do implied complete (backout) request
       * and put the request back on request queue 
       * or just issue the notify.
       */
      client = npa_scheduler_remove(npa_scheduler.forked_queue.head,
                                    NPA_DONT_UPDATE_START_TIMES,
                                    NPA_DONT_LOCK_LIST);

      request_ptr = client->request_ptr;

      if(( delta < 0)
         && (NPA_IMPLIED_COMPLETE_PENDING == request_ptr->implied_state))
      { /* woke up early so complete the request and put back on request queue */
        ULOG_RT_PRINTF_6( npa_scheduler.log, "\tNeed Implied Complete "
                          "(client: 0x%08x) "
                          "(delta: 0x%x) "
                          "(latest_handle_time: 0x%016llx) "
                          "(start_time:0x%016llx)",
                          client,
                          (uint32) delta,
                          ULOG64_LOWWORD(latest_handle_time),
                          ULOG64_HIGHWORD(latest_handle_time),
                          ULOG64_LOWWORD(request_ptr->start_time),
                          ULOG64_HIGHWORD(request_ptr->start_time));

        /* Remove from the implied complete list if it is on it */
        npa_list_Remove(&npa_scheduler.implied_completes_now_list, client);

        /* put back on request queue */
        npa_scheduler_sort_insert(client, 
                                  &npa_scheduler.request_queue,
                                  NPA_DONT_UPDATE_START_TIMES);

        /* issue the complete and update start time */
        npa_issue_implied_complete(client);

        /* we did the implied complete and need to update timer later */
        did_implied_complete = 1;
      }
      else
      { /* notify the clients with their callbacks */
        npa_scheduler_notify(client, NULL);
      }
    } while (0 != npa_scheduler.forked_queue.num_linked_clients);

    if (did_implied_complete)
    { /* Changed the request queue so sort it then
       * setup timer that may have changed.
       * This will short circuit if called within the NPA Scheduler thread
       */
      npa_update_start_time(npa_scheduler.request_queue.tail, 
                            NPA_DO_COMPLETE_LIST);
      npa_setup_next_timeout();
    }
  }

  /* done with critical section */
  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_2( npa_scheduler.log, "\tDONE - npa_run_scheduler_notifies "
                    "(latest_handle_time: 0x%016llx)",
                    ULOG64_LOWWORD(latest_handle_time),
                    ULOG64_HIGHWORD(latest_handle_time));
}


/**
 * <!-- npa_scheduler_sort_insert --> 
 * @brief Insert client into schedule link list.
 *
 * Takes a client and inserts it into the scheduled request link list
 * in sorted order going from closest to furthest request completion time.  
 *
 * @param client : Handle of the client for the request.  
 *                 Client handles are the same as the address
 *                 of the npa_client structure.
 * @param list_ptr : List for the insert
 * @param update_start_times : Update the clients (and all previous
 *                             clients in the list when needed) if true.
 *                             Ignored if the list is not sorted.
 *
 * Client's scheduled request state will be changed to NPA_SCHEDULED_CLIENT_LINKED
 */
static void npa_scheduler_sort_insert(npa_client_handle client,
                                      npa_scheduler_queue_struct *list_ptr,
                                      uint32 update_start_times)
{
  npa_client_handle cptr = NULL;
  npa_scheduler_data *request_ptr;

  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(request_ptr = client->request_ptr);
  CORE_VERIFY(NULL == request_ptr->my_queue); /* not already on a queue*/

  /* make sure element is not linked to anything */
  request_ptr->next = request_ptr->prev = NULL;

  /* remember list this client is in */
  request_ptr->my_queue = list_ptr;

  /* Lock scheduled list */
  Core_MutexLock(list_ptr->lock);

  if (!list_ptr->not_sorted)
  { /* so search for insert point */
    /* setup an invalid start time so it doesn't hit by coincedence */
    request_ptr->start_time = request_ptr->request_time + 1;

    /* sort entry w.r.t. request time.
     * Look for the entry that has a time greater than the new one.
     */
    for (cptr = list_ptr->head;
           NULL != cptr 
           && (request_ptr->request_time >= cptr->request_ptr->request_time);
             cptr = cptr->request_ptr->next)
    { /* client's time is less than the list element's time.
       * Do nothing, just look for next insert location in list */
      CORE_VERIFY(cptr != client); /* test code */
    }
  }
  else
  { /* just throw on at tail */
    cptr = NULL;
  }

  /* at this point, cptr point's to the spot to insert before.
     NULL means at end of list and possibly list is empty.
  */

  /* point to the next element or NULL as the case may be */
  request_ptr->next = cptr;

  if (NULL != cptr)
  { /* have a link after so grab its prev pointer */
    request_ptr->prev = cptr->request_ptr->prev;
  }
  else
  { /* at tail */
    request_ptr->prev = list_ptr->tail;
    list_ptr->tail = client;
  }

  if (cptr == list_ptr->head)
  { /* insert new item at head */
    list_ptr->head = client;
  }
  if (NULL != request_ptr->prev)
  { /* there is an element before so update its next */
    request_ptr->prev->request_ptr->next = client;
  }
  if (NULL != request_ptr->next)
  { /* there is an element after so update its prev */
    request_ptr->next->request_ptr->prev = client;
  }

  /* Now in Linked state */
  request_ptr->state = NPA_SCHEDULED_CLIENT_LINKED;

  ++list_ptr->num_linked_clients;

  if (list_ptr->num_linked_clients > list_ptr->high_watermark)
  { /* keep track of the max elements in the list */
    list_ptr->high_watermark = list_ptr->num_linked_clients;
  }

  /* remember how much work this queue has in it */
  list_ptr->sum_wake_work += request_ptr->active_latency.wake;

  /* update the start time if directed and a sorted list */
  if (update_start_times && !list_ptr->not_sorted)
  {
    npa_update_start_time(client,NPA_DONT_DO_COMPLETE_LIST);
  }
  Core_MutexUnlock(list_ptr->lock);
}


/**
 * <!-- npa_scheduler_remove -->
 *
 * @brief Remove client from schedule link list.
 *
 * Takes a client and removes it from the link list
 * that it is in, else it does nothing.
 * 
 * @param client : Handle of the client for the request.  
 *                 Client handles are the same as the address
 *                 of the npa_client structure.
 * @param update_start_times : Update the clients (and all previous
 *                             clients in the list when needed) if true.
 * @param lock_list : True if list needs to be locked for the action.
 *
 * @return client handle
 * 
 * @sa This might be a blocking call on the scheduler list.
 *
 * Client's scheduled request state will be changed to
 * NPA_SCHEDULED_CLIENT_UNLINKED if remove was successful.
 */
static npa_client_handle npa_scheduler_remove (npa_client_handle client,
                                               uint32 update_start_times,
                                               uint32 lock_list)
{
  npa_scheduler_data *request_ptr;
  npa_scheduler_queue_struct *list_ptr;
   
  if (NULL == client)
  { /* not real client so just ignore */
    return(client);
  }

  /* Should only be called on schedulable clients
   * in a queue
   */
  CORE_VERIFY(NULL != client->request_ptr);

  request_ptr = client->request_ptr;

  list_ptr = request_ptr->my_queue;

  if (list_ptr)
  { /* client in link list so we can remove it */

    if (NPA_LOCK_LIST == lock_list)
    { /* caller wants the list locked */
      Core_MutexLock(list_ptr->lock);
    }
    /* no longer in this list */
    request_ptr->my_queue = NULL;

    if (list_ptr->head == client)
    { /* at list head */
      list_ptr->head = request_ptr->next;
    }
    if (list_ptr->tail == client)
    { /* at list tail */
      list_ptr->tail = request_ptr->prev;
    }
    if (request_ptr->next)
    { /* update item following */
      request_ptr->next->request_ptr->prev 
        = request_ptr->prev;
    }
    if (request_ptr->prev)
    { /* update item preceeding */
      request_ptr->prev->request_ptr->next 
        = request_ptr->next;
    }

    if (NPA_UPDATE_START_TIMES == update_start_times)
    { /* asked to update start times, request is now out of
       * list so update before loose prev pointer
       */
      npa_update_start_time(request_ptr->prev, NPA_DONT_DO_COMPLETE_LIST);
    }
    /*no dangling pointers */
    request_ptr->prev = request_ptr->next = NULL;

    request_ptr->state = NPA_SCHEDULED_CLIENT_UNLINKED;
    --list_ptr->num_linked_clients;
    
    /* work no longer counted but do protect from wrap in case 
     * something got off.  This will clear itself up when the list
     * goes empty.
     */   
    list_ptr->sum_wake_work -= MIN(request_ptr->active_latency.wake,
                                   list_ptr->sum_wake_work);


    if (NPA_LOCK_LIST == lock_list)
    { /* have a list so unlock it */
      Core_MutexUnlock(list_ptr->lock);
    }
  }
  /* else ignore */

  return(client);
}


#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
/**
 * <!-- is_lpr_work_soon -->
 *
 *  @brief see if an RPM dependent request is going to fire on the next timeout.
 *
 * @return 1 if there is one that will fire, 0 if not
 */
NPA_INLINE int is_lpr_work_soon(void)
{
  npa_client_handle client;

  if (NULL != npa_scheduler.forked_queue.head)
  { /* something in fork queue so it is soon */
    return(1);
  }

  /* Check if any LPR issuable client in request queue */
  for (client = npa_scheduler.request_queue.head;
        client;
        client = client->request_ptr->next)
  {
    if (_npa_resource_has_attribute(client->resource, NPA_RESOURCE_LPR_ISSUABLE))
    { /* something to run in LPR */
      return(1);
    }
  }

  /* nothing in the queue is LPR issuable so ignore */
  return (0);
}


/**
 * <!-- npa_scheduler_enable_lpr -->
 *
 *  @brief If lpr is disabled, enable it.
 *
 */
NPA_INLINE void npa_scheduler_enable_lpr(void)
{
  if (1 != npa_scheduler.sleep.lpr_state_on)
  { /* LPR is off.  Enable the lpr. */
    /* There is only one mode so bit 0 is used */
    npa_issue_required_request( npa_scheduler.sleep.lpr_client, SLEEP_LPRM_NUM(0) );
    npa_scheduler.sleep.lpr_state_on = 1;
  }
}


/**
 * <!-- npa_scheduler_disable_lpr -->
 *
 *  @brief If lpr is enabled, disable it.
 *
 */
NPA_INLINE void npa_scheduler_disable_lpr(void)
{
  if (1 == npa_scheduler.sleep.lpr_state_on)
  { /* no request but LPR is on, so turn off */
    npa_scheduler.sleep.lpr_state_on = 0;
    npa_complete_request(npa_scheduler.sleep.lpr_client);
  }
}
#endif /* NPA_SCHEDULER_SLEEP_LPR_SUPPORTED */


/**
 * <!-- npa_scheduler_enable_timer -->
 *
 * @brief Update the Scheduler timer and notify sleep lpr of the change
 *
 * Note - Should always be called in a thread safe environment.
 *
 * @param start_ttick : TTICK absolute time for next timer event
 * @param sleep_deadline : Deadline information for sleep
 * @param timer_latency : TTICKS to account for timer triggering latency
 *
 */
static void npa_scheduler_enable_timer(uint64 start_ttick,
                                       uint64 sleep_deadline,
                                       uint64 timer_latency)
{
  uint64 delta_ttick;
  uint64 current_time = CoreTimetick_Get64();
  uint64 timer_start = start_ttick - timer_latency; /* adjust for latency */

  /* remember time need to run by */
  npa_scheduler.next_scheduler_time = start_ttick;

  /* setup next timeout.
   * this takes into account the non-sleep timer queue latency
   */
  npa_scheduler.timer.current_timeout = timer_start;

  if (timer_start > current_time)
  { /* positive delta so use it */
    delta_ttick = timer_start - current_time;
  }
  else
  { /* time_start is in the past so set delta to zero */
    delta_ttick = 0;
  }

  timer_set_64( &npa_scheduler.timer.timer, delta_ttick, 0, T_TICK );

  /* Tell sleep the time we must be awake (may be UINT64_MAX, implying no
   * deadline). The deadline will get adjusted out if the NPA Scheduler LPR
   * mode is chosen.
   */
  SET_DEADLINE(sleep_deadline);
  npa_scheduler.sleep.last_sleep_deadline = sleep_deadline;

  ULOG_RT_PRINTF_8( npa_scheduler.log, "npa_scheduler_enable_timer "
                    "(start_ttick: 0x%016llx) "
                    "(timer_latency: 0x%016llx) "
                    "(delta: 0x%016llx) "
                    "(sleep_deadline: 0x%016llx)",
                    ULOG64_LOWWORD(start_ttick),
                    ULOG64_HIGHWORD(start_ttick),
                    ULOG64_LOWWORD(timer_latency),
                    ULOG64_HIGHWORD(timer_latency),
                    ULOG64_LOWWORD(delta_ttick),
                    ULOG64_HIGHWORD(delta_ttick),
                    ULOG64_LOWWORD(sleep_deadline),
                    ULOG64_HIGHWORD(sleep_deadline));
}


/**
 * <!-- npa_scheduler_disable_timer -->
 *
 * @brief Cancel the Scheduler timer and notify sleep lpr of the change
 *
 * @note Should always be called in a thread safe environment.
 */
void npa_scheduler_disable_timer(void)
{
  uint64 remaining;

  /* clear the timer and log the number of ticks timer thinks it has
   * until its next expiry.
   */
  remaining = timer_clr_64(&npa_scheduler.timer.timer, T_TICK);

  /* Tell sleep the deadline info it may have is no longer valid */ 
  CANCEL_DEADLINE();

#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
  if (npa_sleep_lpr_supported)
  {
    npa_scheduler_disable_lpr();
  }
#endif

  ULOG_RT_PRINTF_2( npa_scheduler.log, "\tnpa_scheduler_disable_timer "
                    "(timer_expiry_ticks: 0x%016llx)",
                    ULOG64_LOWWORD(remaining),
                    ULOG64_HIGHWORD(remaining));
}


/**
 * <!-- npa_scheduler_signal_task -->
 *
 * @brief Send signal to the NPA Scheduler task
 *
 * @param sig: Signal to send to the NPA Scheduler task.
 */
void npa_scheduler_signal_task(npa_scheduler_action_type sig)
{
  /* valid range for sig is between 0 and UNDEFINED */
  CORE_VERIFY( sig < NPA_SCHEDULED_ACTION_UNDEFINED );
  /* send the event */
  CORE_DAL_VERIFY( DALSYS_EventCtrl( npa_scheduler.events_array[sig], 
                                     DALSYS_EVENT_CTRL_TRIGGER ) );
}


/**
 * <!-- npa_setup_next_timeout -->
 *
 * @brief  Calculates and setup Scheduler Time Out
 * 
 * Calculates the next desired timeout w.r.t. the Scheduled Request list.
 * If no requests are pending, it will make sure the timer is turned off
 * (although there still could be race condition and it still fires.
 *
 * If the request is more than half the wrap time, then assume it is actually
 * late and declare it as 0 and poke the scheduler to handle it.
 *
 * It is not ran in this context since it would be another client's request
 * and we don't want to muddy up the current task with other task's work.
 *
 *  @sa This can block on npa_scheduler.request_queue.lock.
 */
static void npa_setup_next_timeout()
{
  npa_client_handle requestQ_client_ptr;
  npa_client_handle completeQ_client_ptr;
  uint64 start_time = 0;
  uint64 reschedule_time;
  uint64 complete_start_time = UINT64_MAX; /* MAXINT means not set */
  uint64 request_start_time  = UINT64_MAX;
  uint64 timer_latency = 0; /* assume complete queue */
  uint64 sleep_deadline = UINT64_MAX;

  if (NPA_SCHEDULER_NTS_NO_SETUP == npa_scheduler.next_timeout_state)
  { /* scheduler is running so don't need to setup timeout or pokes.
     * No race condition though since the callers request should already
     * be in the queue so scheduler task will see it.
     * If scheduler sneaks in right after the test, then we will do nothing
     * since the timeout will already be correct below.
     */
    return;
  }

  Core_MutexLock(npa_scheduler.request_queue.lock);
  Core_MutexLock(npa_scheduler.complete_queue.lock);

  requestQ_client_ptr = npa_scheduler.request_queue.head;
  completeQ_client_ptr = npa_scheduler.complete_queue.head;

  if (NULL == requestQ_client_ptr && NULL == completeQ_client_ptr)
  { /* Disable timer, cancel sleep deadline and disable LPRM */
    npa_scheduler_disable_timer();

    Core_MutexUnlock(npa_scheduler.complete_queue.lock);
    Core_MutexUnlock(npa_scheduler.request_queue.lock);

    return; /* nothing to schedule */
  }

  if (requestQ_client_ptr)
  { /* have a request queue item so setup desired start time */
    /* Calculate start time for request queue 
    * as it must finish before request time.
    */
    request_start_time = requestQ_client_ptr->request_ptr->start_time
                         - npa_scheduler.config_ptr->sched_latency;

    /* New deadline to sleep */
    sleep_deadline = request_start_time 
                     - npa_scheduler.config_ptr->sleep_wake_transition_latency;
  }

  if (completeQ_client_ptr)
  {
    complete_start_time = completeQ_client_ptr->request_ptr->start_time;
  }

  /* else have a timeout value, make sure next event is correct */
  
  reschedule_time = CoreTimetick_Get64(); /* Set for complete queue s.t.
                                           * only things in the past will
                                           * fire.
                                           */

  if (request_start_time > complete_start_time)
  { /* request queue's start time is after complete's start time so
     * complete should drive timer.
     */
    start_time = complete_start_time;

    /* don't want the timer to fire early for complete queue */
    timer_latency = 0;
  }
  else
  { /* setup request queues start and reschedule time */
    /* need to take into account the timer queue latency time */
    start_time = request_start_time;

    /* need to make sure we wake up soon enough for the timer
     * overhead/latency
     */
    timer_latency = npa_scheduler.config_ptr->timer_queue_latency
                    + npa_scheduler.adjust.timer;

    /* need to handle all requests out past min_reschedule_delta time 
     * and timer latency
     */
    reschedule_time += npa_scheduler.config_ptr->min_reschedule_delta;
  }

  if (start_time >= reschedule_time)
  { /* far enough in the future so set timer */
    if (npa_scheduler.timer.current_timeout != start_time
        || !TIMER_IS_ACTIVE(&npa_scheduler.timer.timer) )
    { /* new time or timer is not active so schedule timeout */
      npa_scheduler_enable_timer(start_time, sleep_deadline, timer_latency);
    } /* else the timeout is already scheduled so nothing to do */

    /* If there are requests in the fork/request turn on lpr */
#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
    if (npa_scheduler.sleep.lpr_client && npa_sleep_lpr_supported)
    { /* have a Sleep LPR and we want to talk to it. */
      if (is_lpr_work_soon())
      { /* lpr issuable requests in the request queue*/
        npa_scheduler_enable_lpr();
        }
      else 
      {
        npa_scheduler_disable_lpr();
      }
    }
#endif
  }
  else if ( npa_scheduler.next_timeout_state == NPA_SCHEDULER_NTS_OPEN)
  { /* wants to happen now so trigger the scheduler and
     * the scheduler is not already poked. so poke it. */

    /* say that we are poking the scheduler.
     * We don't need to worry about this being cleared immediately
     * since the request is already in the scheduled list and it is
     * ok to poke again even if there is nothing to do.  Just trying to
     * cut down on the extra pokes.
     */
    npa_scheduler.next_timeout_state = NPA_SCHDEDULER_NTS_NO_POKE;

    ULOG_RT_PRINTF_10( npa_scheduler.log,
                      "  poke scheduler"
                      " (start_time: 0x%016llx)"
                      " (reschedule_time: 0x%016llx)"
                      " (latency : 0x%016llx)"
                      " (complete_start_time: 0x%016llx)"
                      " (request_start_time: 0x%016llx)",
                      ULOG64_LOWWORD(start_time),
                      ULOG64_HIGHWORD(start_time),
                      ULOG64_LOWWORD(reschedule_time),
                      ULOG64_HIGHWORD(reschedule_time),
                      ULOG64_LOWWORD(timer_latency),
                      ULOG64_HIGHWORD(timer_latency),
                      ULOG64_LOWWORD(complete_start_time),
                      ULOG64_HIGHWORD(complete_start_time),
                      ULOG64_LOWWORD(request_start_time),
                      ULOG64_HIGHWORD(request_start_time));

    /* Turn the timer/lpr off since we are poking.  It will be setup again
     * when the scheduler runs
     */
    npa_scheduler_disable_timer();

    /* Trigger scheduler that it has work to do if needed now */
    npa_scheduler_signal_task(NPA_SCHEDULED_ACTION_TRIGGER);
  }

  Core_MutexUnlock(npa_scheduler.complete_queue.lock);
  Core_MutexUnlock(npa_scheduler.request_queue.lock);
}


/**
 * <!-- npa_scheduler_handle_implied_complete -->
 *
 * @brief Issue the Implied Complete if needed
 *
 * Checks to see if an implied complete is pending on the client
 * and issues it if the system is busy
 *
 * @note This should only be run in the NPA Scheduler tread.
 *  
 * @param action : handle to action describing. (Not used)
 */
void npa_scheduler_handle_implied_complete(npa_scheduler_action *action)
{
  npa_client_handle client;
  npa_query_type query_result;
  npa_query_status qstatus;

  /* See if system is "busy" and thus should do the complete */
  qstatus = npa_query( npa_scheduler.core_cpu_busy_query_handle,
                      NPA_QUERY_CURRENT_STATE,
                      &query_result );

  if ((NPA_QUERY_UNKNOWN_RESOURCE == qstatus)
      || ((NPA_QUERY_SUCCESS == qstatus)
          && (0 != query_result.data.state)))
  { /* system is busy or not defined so need to issue the implied completes */
    Core_MutexLock(npa_scheduler.scheduler_work_lock);
    /* complete one request on the queue. */
    client = (npa_client_handle) 
                npa_list_PopFront(&npa_scheduler.implied_completes_now_list);
    if (client)
    { /* popped a client off of the list */
      CORE_VERIFY_PTR(client->request_ptr); /* Klocworks */
      /* Check if there is an implied complete and if it needs to run */
      if (NPA_IMPLIED_COMPLETE_PENDING == client->request_ptr->implied_state
          && NPA_SCHEDULED_CLIENT_LINKED == client->request_ptr->state)
      { /* Implied complete is pending and it is still linked in queue 
         * so OK to run the impled complete
         */
        npa_issue_implied_complete(client);

        /* Setup timer that may have changed */
        npa_setup_next_timeout();
      }
    }
    Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

    if (npa_list_PeekFront(&npa_scheduler.implied_completes_now_list))
    { /* We need to allow the scheduler a chance to run so resignal
       * this event if there is more to do
       */
      npa_scheduler_signal_task(NPA_SCHEDULED_ACTION_IMPLIED_COMPLETE);
    }
  }
}


/**
 * <!-- npa_issue_scheduled_request -->
 *
 * @brief Set as the client->issue_request function to make the following 
 * request a scheduled client work request.
 *
 * Request will be either scheduled to run by the time setup by the
 * npa_create_scheduled_request() or immediately depending up
 * the time remaining until the calculated start time (based on
 * deadline, latency, et al). 
 *  
 * @param client : handle to client being used
 * @param new_request : requested resource state
 */
static void npa_issue_scheduled_request( npa_client_handle client, 
                                         int new_request )
{
  npa_scheduler_data *request_ptr = client->request_ptr;
  npa_query_type query_result;
  npa_query_status qstatus;
  npa_resource_latency *resource_latency;
  uint32 req_time, fork_time, notify_time;
  npa_resource *resource = client->resource;

  /* Must be in EXPECTING state else it is a hard error */
  if (NPA_SCHEDULED_CLIENT_EXPECTING != request_ptr->state)
  { /* Not in the expected state for issuing the request.
     * This could be due to another issue_request being
     * given on an already scheduled request.
     */
    ULOG_RT_PRINTF_3( npa.log,
                      "npa_issue_scheduled_request - unexpected state - "
                      " (client: 0x%08x)"
                      " (client_name: \"%s\")"
                      " (state: \"%s\")",
                      client,
                      client->name,
                      npa_scheduled_client_state_name(request_ptr->state));
    CORE_VERIFY(0); /* fatal error */
  }
  NPA_PENDING_REQUEST(client) = NPA_INCOMING_REQUEST(client);

  /* Only user thread can touch client's data structure
   * until it is in the list. So no lock is needed until
   * the insert occurs.
   */

  /* save the new_request parameter to use later */
  request_ptr->new_request = new_request;

  /* setup the client's values though in case the resource
   * wants to take over on the request.  This means the new_request
   * and the issue_request values have to be set to desired values.
   */
  client->issue_request = request_ptr->issue_request;
  /* TODO - This now opens up a window where we may/may not have to handle
   * this request but cannot detect an illegal additional issue_request.
   * One solution is to not do the above and make it the responsiblity
   * of the resource to clean up.  Maybe we provide a function call
   * to do the cleanup?
   */

  /* set default latency */
  req_time = client->resource->latency->request;
  fork_time = client->resource->latency->fork;
  notify_time = client->resource->latency->notify;

  /* Find what work can be done pre and post sleep.  If the
   * request is LPR Issuable, then the request can be handled 
   * in the NPA LPR routine when entering sleep.
   * If the routine is not LPR Issuable, then the request will be
   * called post wakeup.
   */
  /* request resource's latencies */
  /* Pass in the late_probability */
  query_result.data.value = request_ptr->late_probability;
  query_result.type = NPA_QUERY_TYPE_VALUE;
  qstatus = npa_query_resource( client->resource, 
                                NPA_QUERY_RESOURCE_LATENCY, 
                                &query_result );
  if (NPA_QUERY_UNSUPPORTED_QUERY_ID != qstatus)
  { /* resource gave latencies so use them */
    /* see if resource wants to handle the timeout itself */
    if (NPA_QUERY_TYPE_VALUE == query_result.type
        && 0 == query_result.data.value)
    { /* resource will handle so we are done */
      /** @todo How does resource know it needs to handle 
        * THIS client's request?  Should we pass the client handle?
        * -- Also, how does this resource know the deadline info?
        */
      ULOG_RT_PRINTF_4( npa.log, 
                        "npa_issue_scheduled_request (client: 0x%08x) "
                        "(client_name: \"%s\" ) "
                        "(resource_name: \"%s\") (Latency: %d) "
                        "- Resource will handle scheduling",
                        client,
                        client->name,
                        client->resource_name,
                        query_result.data.state);

      /* count it */
      ++request_ptr->stats.resource_handle_scheduling;

      /* restore original issue request */
      client->issue_request = request_ptr->issue_request;

      /* No implied complete */
      request_ptr->implied_state = NPA_NO_IMPLIED_COMPLETE;
      request_ptr->issue_request = NULL; /* no dangling pointers */

      /* Request is no longer active */
      request_ptr->state = NPA_SCHEDULED_CLIENT_INACTIVE;

      return;
    }

    /* resource must have returned a reference at this point if using the api
     * correctly */
    CORE_VERIFY(resource_latency = 
        (npa_resource_latency *)query_result.data.reference);
    if (resource_latency->request != 0xffffffff)
    {
      req_time = resource_latency->request;
    }
    if (resource_latency->notify != 0xffffffff)
    {
      /* set the notify latency */
      notify_time = resource_latency->notify;
    }
    /* if resource is lpr issuable, set the fork latency */
    if (resource_latency->fork != 0xffffffff)
    {
      fork_time = resource_latency->fork;
    }
  }

  /* set the found values in the client's structure */
  request_ptr->active_latency.request = req_time;
  request_ptr->active_latency.fork = fork_time;
  /* we always have notify work to do */
  request_ptr->active_latency.notify 
    = request_ptr->active_latency.wake = notify_time;
  
  /* determine amount of extra work to do */
  if (request_ptr->new_request
     || NPA_NO_IMPLIED_COMPLETE != request_ptr->implied_state )
  { /* request will not short circuit or there is a possibility 
     * of an implied complte so wake latency 
     * is the additional request and any runtime adjustments
     */
    request_ptr->active_latency.wake 
      += req_time + request_ptr->active_latency.request_adjustment;
  }

  Core_MutexLock(npa_scheduler.scheduler_work_lock);

  /* If the request is not a complete/cancel, then it goes on the
   * regular request list and will be ran s.t. it is done by
   * the request time.
   */
  if (NPA_PENDING_REQUEST(client).state)
  { /* Put in queue and update start times since things can
     * only shift to the left.
     */
    npa_scheduler_sort_insert(client, 
                              &npa_scheduler.request_queue, 
                              NPA_UPDATE_START_TIMES);
  }
  else
  { /* request is for a complete or cancel so put on the
     * complete list.  It will happen no sooner than request time.
     * Don't need to update start times since things can shift to
     * the right.
     */
    npa_scheduler_sort_insert(client, 
                              &npa_scheduler.complete_queue, 
                              NPA_DONT_UPDATE_START_TIMES);
    /* since we don't update the start time ever, we need to set
     * the start_time as npa_scheduler_sort_insert() puts in an
     * illegal value.
     */
    request_ptr->start_time = request_ptr->request_time;
  }

  /* Setup timer that may have changed, this will poke the scheduler
   * if this needs to run soon.
   */
  npa_setup_next_timeout();

  /* Check if there is an implied complete and if it needs to run */
  if (NPA_IMPLIED_COMPLETE_PENDING == request_ptr->implied_state)
  { /* have an implied complete */
    /* See if system is "busy" and thus should do the complete */
    qstatus = npa_query( npa_scheduler.core_cpu_busy_query_handle,
                         NPA_QUERY_CURRENT_STATE,
                         &query_result );

    if (NPA_QUERY_UNKNOWN_RESOURCE == qstatus ||
        (NPA_QUERY_SUCCESS == qstatus && 0 != query_result.data.state) ||
        !_npa_resource_has_attribute(resource, NPA_RESOURCE_LPR_ISSUABLE))
    { /* system is busy or busy is not defined; issue the implied complete */
      /* Add to the implied complete stack */
      npa_list_PushFront(&npa_scheduler.implied_completes_now_list, client);
      /* Trigger scheduler thread to handle the implied complete */
      npa_scheduler_signal_task(NPA_SCHEDULED_ACTION_IMPLIED_COMPLETE);
    }
    else
    { /* /core/cpu/busy is 0; defer the complete, but set things up such that
        we can execute the complete if the state of this resource changes */
      /* Add client to implied_completes_planB list */
      npa_list_PushBack(&npa_scheduler.implied_completes_planB_list, client);
    }
  }

  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_9( npa_scheduler.log,
                    "\tnpa_issue_scheduled_request"
                    " (client: 0x%08x)"
                    " (client_name: \"%s\")"
                    " (request time: 0x%016llx)"
                    " (start time: 0x%016llx)"
                    " (full latency: 0x%08x)"
                    " (fork latency: 0x%08x)"
                    " (notify latency: 0x%08x)",
                    client,
                    client->name,
                    ULOG64_LOWWORD(request_ptr->request_time),
                    ULOG64_HIGHWORD(request_ptr->request_time),
                    ULOG64_LOWWORD(request_ptr->start_time),
                    ULOG64_HIGHWORD(request_ptr->start_time),
                    req_time,
                    fork_time,
                    notify_time);
}


/**
 * <!-- npa_scheduler_core_cpu_busy_event_callback -->
 *
 * @brief Performs implied completes if the state of /core/cpu/busy changes
 * to 1, after the scheduler has deferred one/more implied_completes
 * 
 */
void npa_scheduler_core_cpu_busy_event_callback( void         *context,
                                                 unsigned int  event_type, 
                                                 void         *data,       
                                                 unsigned int  data_size )
{
  npa_query_status qstatus;
  npa_query_type query_result;

  qstatus = npa_query( npa_scheduler.core_cpu_busy_query_handle,
                       NPA_QUERY_CURRENT_STATE, &query_result );

  ULOG_RT_PRINTF_1( npa_scheduler.log, 
                    "npa_scheduler_core_cpu_busy_event (state: %d)", 
                    query_result.data.state );

  if (qstatus == NPA_QUERY_SUCCESS && query_result.data.state != 0)
  { /* System reading busy */
    Core_MutexLock( npa_scheduler.scheduler_work_lock );
    /* If we have deferred implied completes, execute them... */
    if (!npa_list_IsEmpty( &npa_scheduler.implied_completes_planB_list ))
    {
      npa_list_ApplyAndClear( &npa_scheduler.implied_completes_planB_list,
                              npa_listfn_issue_implied_complete );
      /* Setup timer may have changed. TODO: Really? */
      npa_setup_next_timeout();
    }
    Core_MutexUnlock( npa_scheduler.scheduler_work_lock );
  }
}

#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
/**
 * <!-- npa_init_scheduler_lpr_callback -->
 *
 * @brief All dependencies are now ready so initialize the npa_scheduler_lpr.
 *
 * During the scheduler initialization this is set as the resource available
 * callback for when the "npa_scheduler_lpr_init_resources" become available. 
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_init_scheduler_lpr_callback( void *context,
                                             unsigned int event_type,
                                             void *data,
                                             unsigned int data_size)
{
  if (npa_sleep_lpr_supported)
  {/* Init and register the Scheduler Fork LPR */
    ULOG_RT_PRINTF_0( npa_scheduler.log,
                      "npa_init_scheduler_lpr_callback");

    npa_scheduler_lpr_init();

    /* we are working with sleep so timer needs to be deferrable
     * so sleep knows it is not a hard wakeup time.
     */
    timer_group_set_deferrable(&npa_scheduler.timer.group, 1);
  }
}
#endif /* NPA_SCHEDULER_SLEEP_LPR_SUPPORTED */


/**
 * <!-- npa_scheduler_timer_def -->
 *
 * @brief Setup NPA Scheduler timer
 * 
 */
static void npa_scheduler_timer_def(void)
{  /* Timer subsytem should now be up so setup timer interface 
    * Have callback send signal to workloop
    */
  timer_def( &npa_scheduler.timer.timer, 
             &npa_scheduler.timer.group, 
             NULL, 
             0,
             npa_scheduler_timeout_callback,
             0 );
}


/**
 * <!-- npa_scheduler_rpm_polling_mode_avail_cb -->
 *
 * @brief Create a client handle to the "/protocol/rpm/polling_mode" resource
 *
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "/protocol/rpm/polling_mode" becomes available.
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_scheduler_create_rpm_polling_mode_client( void *context,
                                                          unsigned int event_type,
                                                          void *data,       
                                                          unsigned int data_size)
{
  npa_scheduler.rpm_polling_mode_client = 
    npa_create_sync_client( "/protocol/rpm/polling_mode",
                            "npa_scheduler_rpm_polling_mode_client",
                            NPA_CLIENT_REQUIRED );
  CORE_VERIFY_PTR( npa_scheduler.rpm_polling_mode_client );
}


/**
 * <!-- npa_init_scheduler_timer_callback -->
 *
 * @brief All dependencies are now ready for the scheduler timer so initialize.
 *
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "/init/timer" becomes available.
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_init_scheduler_timer_callback( void *context,
                                        unsigned int event_type, 
                                        void *data,       
                                        unsigned int data_size)
{
  ULOG_RT_PRINTF_0( npa_scheduler.log,
                    "npa_init_scheduler_timer_callback");

  /* Timer subsytem should now be up so setup timer interface */
  npa_scheduler_timer_def();

  npa_scheduler.timer_up = 1;

#ifdef NPA_SCHEDULER_SLEEP_LPR_SUPPORTED
  if ( npa_sleep_lpr_supported )
  { /* Set up a callback to initialize the scheduler LPR after "/sleep/lpr"
       and "/core/cpu/wakeup" are available */
    static const char *lpr_deps[] = { "/sleep/lpr", "/core/cpu/wakeup" };
    npa_resources_available_cb( NPA_ARRAY( lpr_deps ),
                                npa_init_scheduler_lpr_callback,
                                NULL );
  }
#endif
}


/**
 * <!-- npa_init_scheduler_clk_cpu_callback -->
 *
 * @brief Creates client linked to "clk/cpu"
 *
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "/clk/cpu" becomes available.
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_init_scheduler_clk_cpu_callback( void *context,
                                                 unsigned int event_type,
                                                 void *data,
                                                 unsigned int data_size)
{
  /* Error if client is already created */
  CORE_VERIFY(NULL == npa_scheduler.clk_cpu_client);

  /* create the clk client */
  npa_scheduler.clk_cpu_client 
    = npa_create_sync_client( "/clk/cpu", 
                              "npa_scheduler_clk_cpu_client",
                              NPA_CLIENT_SUPPRESSIBLE );
  CORE_VERIFY_PTR(npa_scheduler.clk_cpu_client);
  
  npa_scheduler.clk_cpu_floor = npa_scheduler.config_ptr->clk_baseline;
  
#ifndef WINSIM
  if ( npa_scheduler.clk_cpu_floor == 0 )
  {
    npa_query_type     queryResult;
    npa_query_status   status;

    status = npa_query_by_name( "/clk/cpu",
                                CLOCK_NPA_QUERY_CPU_MAX_KHZ_AT_CORNER + VCS_CORNER_LOW,
                                &queryResult );
    if ( status == NPA_QUERY_SUCCESS )
    {
      npa_scheduler.clk_cpu_floor = queryResult.data.value;
    }
  }
#endif
}


/**
 * <!-- npa_init_scheduler_core_cpu_busy_callback -->
 *
 * @brief Creates query handle to "core/cpu_busy"
 * 
 * In npa_init_scheduler, this function is set as the resource available
 * callback for when "/core/cpu/busy" becomes available.
 * All parameters are ignored but defined to meet calling convention.
 *
 * @param context : Not used
 * @param event_type: Not used
 * @param data : Not used
 * @param data_size: Not used
 */
static void npa_init_scheduler_core_cpu_busy_callback( void *context,
                                                       unsigned int event_type,
                                                       void *data,
                                                       unsigned int data_size)
{
  /* Error if query handle is already created */
  CORE_VERIFY(NULL == npa_scheduler.core_cpu_busy_query_handle);

  /* create a query handle to /core/cpu/busy */
  CORE_VERIFY_PTR( 
    npa_scheduler.core_cpu_busy_query_handle
      = npa_create_query_handle( "/core/cpu/busy" ) );

  /* create a state change event handle to /core/cpu/busy */
  CORE_VERIFY_PTR(
    npa_scheduler.core_cpu_busy_event_handle
      = npa_create_change_event_cb( "/core/cpu/busy",
                                    "npa_scheduler_core_cpu_busy_state",
                                    npa_scheduler_core_cpu_busy_event_callback,
                                    NULL ) );
}

/**
 * <!-- npa_mcpm_clkquery_node_available -->
 *
 * @brief Creates a query handle to the MCPM "/mcpm/clk/query" resource
 * and queries it for an optimal Q6 floor/baseline vote.
 *
 * @param Unused
 */
static void npa_mcpm_clkquery_node_available( void *context,
                                              unsigned int event_type,
                                              void *data,
                                              unsigned int data_size )
{
  npa_query_status status;
  npa_query_type result;

  status = npa_query_by_name( "/mcpm/clk/query",
                              NPA_QUERY_CURRENT_STATE, &result );

  if ( status == NPA_QUERY_SUCCESS && result.data.state != 0 )
  {
    npa_scheduler.clk_cpu_floor = result.data.state;
  }
}

/**
 * <!-- npa_resource_overrides_list_add_uniquely -->
 *
 * @brief Walk the list. Replace or add the npa_resource_config.  
 *
 * @param list_ptr : Pointer to a npa_list_header to work on. 
 * @param add_ptr : An npa_resource_config to add
 */
static void npa_resource_overrides_list_add_uniquely(npa_list_header *list_ptr,
                                             const npa_resource_config *add_ptr)
{
  npa_list_elem *elem_ptr = NULL;

  CORE_VERIFY_PTR(list_ptr);
  CORE_VERIFY_PTR(add_ptr);

  while (add_ptr->name)
  { /* find if current name is unique in the list */
    for (elem_ptr = list_ptr->head; elem_ptr != NULL; elem_ptr = elem_ptr->next)
    {
      if (strncmp(((npa_resource_config *)(elem_ptr->data))->name, 
                  add_ptr->name, NPA_MAX_STR_LEN+1) == 0)
      { /* names match */
        break;
      }
    }

    if (elem_ptr)
    { /* found a match so overwrite */
      elem_ptr->data = (void *)add_ptr;
    }
    else
    { /* unique so push to beginning */
      npa_list_PushFront(list_ptr, (void *) add_ptr);
    }

    ++add_ptr;
  }
}


/**
 * <!-- npa_resource_overrides_config -->
 *
 * @brief Update resource configs if updates are available. 
 */
static void npa_resource_overrides_config( npa_list_header *list_ptr )
{
#ifdef NPA_USES_CORE_PROPERTY
  npa_resource_config *overrides_ptr = NULL;
  int property_status;

  /* Get npa_resource_overrides run time config data if exists.
   * overrides_ptr will not change if it doesn't exist
   */
  property_status = CoreProperty_GetPtr("/dev/power/npa/config",
                                        "resource_overrides",
                                        (const void **) &overrides_ptr);

  if ((CORE_PROPERTY_SUCCESS == property_status) && overrides_ptr )
  { /* updated version */
    ULOG_RT_PRINTF_0( npa.log,
        "npa_resource_overrides_config: Runtime NPA Resource Config found" );
    npa_resource_overrides_list_add_uniquely(list_ptr, overrides_ptr);
  }
    
  /* Get any possible target specific updates and new values. 
   * overrides_ptr will not change if it doesn't exist.
   */
  property_status = CoreProperty_GetPtr("/dev/power/npa/target/config",
                                        "resource_overrides",
                                        (const void **) &overrides_ptr);

  if ((CORE_PROPERTY_SUCCESS == property_status) && overrides_ptr )
  { /* update to current state version or prepend if unique */
    ULOG_RT_PRINTF_0( npa.log,
        "npa_resource_overrides_config: Target NPA Resource Config found" );
    npa_resource_overrides_list_add_uniquely(list_ptr, overrides_ptr);
  }

#else /* !NPA_USES_CORE_PROPERTY */
  /* create the list */
  npa_resource_overrides_list_add_uniquely(list_ptr, 
                                           npa_resource_overrides_data);
#endif /* NPA_USES_CORE_PROPERTY */
}


/**
 * <!-- npa_get_resource_override_latency -->
 * 
 * @brief Search for a resource name in npa_scheduler.overrides
 *
 * @param name : Name of resource to search for
 *
 * @return pointer to latencies if they exist else returns NULL
 */
npa_resource_latency_data 
  *npa_get_resource_override_latency(npa_list_header *list,
                                     const char *name)
{
  npa_list_elem *elem_ptr;
  npa_resource_config *data_ptr;

  CORE_VERIFY_PTR(name);

  for (elem_ptr = list->head; elem_ptr != NULL; elem_ptr = elem_ptr->next)
  { /* scan the list */
    data_ptr = (npa_resource_config *)(elem_ptr->data);
    if(strncmp(data_ptr->name, name, NPA_MAX_STR_LEN+1) == 0)
    { /* found it so return latency */
      return(&((npa_resource_config *)(elem_ptr->data))->latency);
    }
  }
  return((npa_resource_latency_data *) NULL);
}


/**
* <!-- npa_schedule_client_internal -->
*
* @brief Make the client schedule the next request.
*
* client and client->request_ptr must be valid on entry.
*/
static npa_client_handle 
npa_schedule_client_internal( npa_client_handle client, 
                              uint64 request_time, 
                              uint32 late_probability,
                              npa_scheduler_callback notification,
                              void * context )
{
  npa_scheduler_data *request_ptr = client->request_ptr;

  /* Make sure there is no previous request on client */
  CORE_VERIFY( request_ptr->state == NPA_SCHEDULED_CLIENT_INACTIVE );

  /* Save the request in the client structure */
  request_ptr->request_time = request_time;
  request_ptr->late_probability = late_probability;
  request_ptr->notification = notification;
  request_ptr->context = context;

  /* Swap out issue_request() function pointer with scheduled_issue_request
     but save original for scheduler to call */
  request_ptr->issue_request = client->issue_request;
  client->issue_request = npa_issue_scheduled_request;

  /* state is waiting for on "issue_request" call to happen */
  request_ptr->state = NPA_SCHEDULED_CLIENT_EXPECTING;

  /* notification state is that nothing for this request has run yet */
  request_ptr->notify_state = NPA_SCHEDULED_NOTIFICATION_NO_ACTION;

  return client;
}


/**
 * <!-- npa_scheduler_init_task -->
 *
 * @brief Start up the NPA Scheduler tsk
 * 
 * To be ran at initialization time.  Will Core Fatal if the task
 * could not be started.
 *
 * @param config_data - Pointer to scheduler configuration data
 */
void npa_scheduler_init_task(const npa_scheduler_config *config_data)
{
  int i;
   /* create events to send to this task */
  for (i=0; i < NPA_SCHEDULED_ACTION_UNDEFINED; ++i)
  {
    CORE_DAL_VERIFY( 
      DALSYS_EventCreate( DALSYS_EVENT_ATTR_CLIENT_DEFAULT, 
                          &npa_scheduler.events_array[i],
                          NULL ) );
    CORE_DAL_VERIFY( 
      DALSYS_EventCtrl( npa_scheduler.events_array[i], 
                        DALSYS_EVENT_CTRL_RESET ) );
  }
}


/**
 * <!-- npa_schedule_timeout_callback -->
 *
 * @brief Handles Timer Callbacks
 * 
 * Triggers the Scheduler workloop with an action to process the 
 * schedule requests from the timer thread.
 *
 * @param data : Data passed by timer.  Defined when calling timer_def().
 */
void npa_scheduler_timeout_callback(timer_cb_data_type data)
{
  uint64 cur_time;
  int64 delta_time;
  
  cur_time = npa_scheduler.timer.timedout_at = CoreTimetick_Get64();

  /* Measure desired timeout vs current */
  delta_time = npa_compare_times(npa_scheduler.timer.current_timeout, 
                                 cur_time);
  if (delta_time < 0)
  { /* Callback is late */
      npa_calc_duration(npa_scheduler.timer.current_timeout,
                        cur_time,
                        &npa_scheduler.stats.timeout_callback_late);
  }
  else
  { /* ontime*/
    npa_calc_duration(cur_time,
                      npa_scheduler.timer.current_timeout,
                      &npa_scheduler.stats.timeout_callback_early);
  }

  npa_scheduler_signal_task( NPA_SCHEDULED_ACTION_TIMEOUT );
}

/*======================================================================
  Public Functions
  ======================================================================*/
/** Updated npa configuration data */
npa_scheduler_config npa_scheduler_live_config;


/**
 * <!-- npa_init_scheduler -->
 * 
 * @brief Init scheduled request data structures, but not timer 
 */
void npa_init_scheduler(void)
{
  ULogResult ulog_result;
  const npa_scheduler_config *config_data = npa_scheduler_target_config();

  if ( NULL != npa_scheduler.log )
  { /* already initialized */
    return;
  }

  ulog_result = ULogFront_RealTimeInit( &npa_scheduler.log,
                                        NPA_SCHEDULER_DEFAULT_LOG_NAME,
                                        config_data->log_buffer_size,
                                        ULOG_MEMORY_LOCAL,
                                        ULOG_LOCK_OS );

  CORE_VERIFY( DAL_SUCCESS == ulog_result
               || ( ULOG_ERR_INITINCOMPLETE == ulog_result
                    && 0 == config_data->log_buffer_size ) );

  ULOG_RT_PRINTF_1( npa_scheduler.log, "npa_init_scheduler (config_data: 0x%08x)",
                    config_data );

  if ( 0 == config_data->scheduled_request_workloop.num_events )
  { /* no events allowed so feature is disabled */
    return;
  }
  /* else we should init scheduled requests */

  /* record config info */
  npa_scheduler.config_ptr = config_data;

  /* create locks for accessing npa_scheduler queues */
  CORE_VERIFY_PTR( 
    npa_scheduler.request_queue.lock 
      = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  /* Fork queue does not need to be sorted as it is always
   * cleared at one time and it is filled off of a already
   * sorted list
   */
  CORE_VERIFY_PTR( 
    npa_scheduler.forked_queue.lock 
      = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));
  npa_scheduler.forked_queue.not_sorted = 1;

  CORE_VERIFY_PTR( 
    npa_scheduler.complete_queue.lock 
      = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  /* create scheduler thread lock */
  CORE_VERIFY_PTR( 
    npa_scheduler.scheduler_work_lock 
      = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  /* Create request data pool */
  npa_init_pool( &npa_scheduler.data_pool,
                 &config_data->scheduled_request_allocation,
                 sizeof(npa_scheduler_data) );

  /* initialize data structures of the NPA Scheduler thread */
  npa_scheduler_init_task(config_data);

  /* create a client to the rpm polling mode resource when it is available */
  npa_resource_available_cb( "/protocol/rpm/polling_mode",
                             npa_scheduler_create_rpm_polling_mode_client,
                             NULL );

  /* need to wait until /init/timer is available before
   * the scheduler can setup the timer.
   */
  npa_resource_available_cb( "/init/timer",
                             npa_init_scheduler_timer_callback,
                             NULL );

  /* need to wait until /clk/cpu is available before
   * the scheduler can create the client.
   */
  npa_resource_available_cb( "/clk/cpu",
                             npa_init_scheduler_clk_cpu_callback,
                             NULL );

  /* Need /core/cpu/busy for Implied Complete processing */
  npa_resource_available_cb( "/core/cpu/busy",
                             npa_init_scheduler_core_cpu_busy_callback,
                             NULL );

  /* If the MCPM node with "optimal floor vote" information is
     available, we'll use it */
  npa_resource_available_cb( "/mcpm/clk/query",
                             npa_mcpm_clkquery_node_available,
                             NULL );

  npa_scheduler_lpr_issuable_config();

  /* parse the resource latency override information and put it into
   * a list to be parsed at client creation to determine default latencies.
   */
  npa_resource_overrides_config(&npa_scheduler.overrides_list);

  /* let world know that Scheduled Request is up */
  npa_define_marker("/init/npa_scheduled_requests");
}


/**
 * <!-- npa_create_schedulable_client_ex -->
 *
 *
 */
npa_client_handle npa_create_schedulable_client_ex(const char *resource_name,
                                                   const char *client_name,
                                                   npa_client_type client_type,
                                                   unsigned int client_value,
                                                   void *client_ref)
{
  npa_client    *client;
  npa_scheduler_data *request_ptr;
  uint32 req_time, fork_time, notify_time;
  npa_resource_latency_data *internal_data_ptr;
  npa_resource_latency *resource_latency;

  if (NULL == npa_scheduler.log)
  {  /* if no workloop, then feature not enabled so
      * say that the client could not be created
      */
     ULOG_RT_PRINTF_3( npa.log,
                       "FAILED npa_create_schedulable_client (resource: \"%s\") "
                       "(client_name: \"%s\") (client_type: \"%s\"):"
                       "NPA target configuration does not support scheduled clients",
                       resource_name,
                       client_name,
                       npa_client_type_name(client_type) );
    return (NULL);
  }

  ULOG_RT_PRINTF_3( npa.log, "npa_create_schedulable_client_ex "
                    "(resource_name: \"%s\") (client_name: \"%s\") "
                    "(client_type: \"%s\")",
                    resource_name, client_name,
                    npa_client_type_name( client_type ) );

  /* try to create a new client */
  client = npa_create_sync_client_ex(resource_name,
                                     client_name, 
                                     client_type,
                                     client_value,
                                     client_ref);

  if (NULL != client)
  { /* create schedule request data structure */
    
    /* Klocworks */
    CORE_VERIFY_PTR(client->resource);
    CORE_VERIFY_PTR(client->resource->_internal);

    CORE_VERIFY_PTR(request_ptr = Core_PoolAlloc( &npa_scheduler.data_pool ));

    memset(request_ptr,0,sizeof(npa_scheduler_data));
    
    client->request_ptr = request_ptr;

    if (client->resource->latency == NULL)
    {
      /* allocate and set default latency */
      CORE_VERIFY_PTR( resource_latency = Core_PoolAlloc( &npa.latency_pool ));

      memset(resource_latency,0,sizeof(npa_resource_latency));

      req_time = npa_scheduler.config_ptr->request_latency;
      fork_time = npa_scheduler.config_ptr->fork_latency;
      notify_time = npa_scheduler.config_ptr->notify_latency;

      /* Check to see if there is any resource specific latency values */
      internal_data_ptr 
          = npa_get_resource_override_latency(&npa_scheduler.overrides_list,
                                              resource_name);
      if (internal_data_ptr)
      { /* non-NULL means resource has override data */
        /* see if it overrides default */
        if (internal_data_ptr->req_latency != 0xffffffff)
        { /* valid request time */
          req_time = internal_data_ptr->req_latency;
        }
        if (internal_data_ptr->fork_latency != 0xffffffff)
        { /* valid fork time */
          fork_time = internal_data_ptr->fork_latency;
        }
        if (internal_data_ptr->notify_latency != 0xffffffff)
        {/* valid latency time */
          notify_time = internal_data_ptr->notify_latency;
        }
      }

      /* set the desired default value */
      resource_latency->request = req_time;
      resource_latency->fork = fork_time;
      resource_latency->notify = notify_time;
      client->resource->latency = resource_latency;
    }
  }

  return(client);
}


/**
 * <!-- npa_destroy_schedulable_client -->
 *
 */
void npa_destroy_schedulable_client(npa_client_handle client)
{
  npa_scheduler_data *request_ptr;

  if (NULL == client->request_ptr)
  { /* not schedulable so just ignore the call then */
    return;
  }
  
  /* close race condition with scheduler */
  Core_MutexLock(npa_scheduler.scheduler_work_lock);
  
  request_ptr = client->request_ptr;

  /* client may have a request pending so remove it.
   * If the request is in progress, this will then block
   * until it is done.
   */
  npa_unschedule_request(client);

  client->request_ptr = NULL; /* no dangling pointers */

  /* ok now to remove memory */
  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  /* remove memory */
  Core_PoolFree( &npa_scheduler.data_pool, request_ptr );
  /* request_ptr = NULL; Uncomment if ever put more code below this */
}


/**
 * <!-- npa_schedule_client_qclk -->
 *
 * @brief Places client into scheduled request mode.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time in QTimer ticks.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and will be invoked
 * when the request is complete. Note that the callback will be ran in the
 * Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is 
 * invoked or npa_unschedule_request is invoked and the client is successfully
 * moved out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request.  Client handles are
 * the same as the address of the npa_client structure.
 * @param request_time : Target absolute time for the request to be fininshed.
 * Units are T_TICKS for QTimer.
 * @param late_probablity : Percentage of time (in 2^-31) that a late 
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when
 * request is completed. This may occur in a differnt thread.
 * @param context : data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
npa_client_handle npa_schedule_client_qclk( npa_client_handle client, 
                                            uint64 request_time, 
                                            uint32 late_probability,
                                            npa_scheduler_callback notification,
                                            void * context )
{
  /* Make sure this is a schedulable client */
  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(client->request_ptr);

  ULOG_RT_PRINTF_7( npa.log,
                    "npa_schedule_client "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(time: 0x%016llx) (late_prob: %d)"
                    "(notification: 0x%08x) (context: 0x%x) ",
                    client, client->name, 
                    ULOG64_LOWWORD(request_time),
                    ULOG64_HIGHWORD(request_time),
                    late_probability,
                    notification, context );

  /* schedule the client */
  return npa_schedule_client_internal( client,
                                       request_time,
                                       late_probability,
                                       notification,
                                       context );
}


/**
 * <!-- npa_release_and_schedule_client_qclk -->
 *
 * @brief Places client into scheduled request mode and completes any current
 * request on the client.
 *
 * Creates a scheduled request that the system will attempt to honor
 * and complete by the requested deadline time.  It will also complete
 * an current requests in the most determined optimal method.  This can be
 * either issueing the "npa_complete()" immediately to just waiting for the
 * system to go to sleep and letting the Next Active Set and Sleep set handle
 * the cancel.
 *
 * The deadline is the time the scheduled request should be completed by.
 *
 * The notification callback is required (cannot be null) and will be invoked
 * when the request is complete. Note that the callback will be ran in the
 * Schedule Request scheduler thread.
 * This client cannot be reused until either the notification callback is
 * invoked or npa_unschedule_request is invoked and the client is successfully
 * moved out of Scheduled mode.
 *
 * It is expected that not all scheduled requests are of equal importance.
 * Late probability is the percentage of time (in 2^-31) that a late completion
 * can be tolerated.  0 (no lateness is acceptable) can be passed.
 * The scheduler will use this in determining how worst-case of latency should
 * be accounted for. Even with a value of zero, it is not possible to guarantee
 * completion by the given deadline, but the scheduler will make a best effort
 * to complete the request by the given time.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 * @param deadline_time : Target absolute time for the request to be fininshed.
 * Units are T_TICKS for qclock.
 * @param late_probablity : percentage of time (in 2^-31) that a late 
 * completion can be tolerated. 0 lateness is acceptable.
 * @param notification : Callback function to be called by scheduler when
 * request is completed. This may occur in a differnt thread.
 * @param context : data to be passed to call_back as parameters.
 *
 * @return Return value is the client handle passed in.
 */
npa_client_handle 
    npa_release_and_schedule_client_qclk( npa_client_handle client, 
                                          uint64 request_time, 
                                          uint32 late_probability,
                                          npa_scheduler_callback notification,
                                          void * context )
{
  /* Make sure this is a schedulable client */
  CORE_VERIFY_PTR(client);
  CORE_VERIFY_PTR(client->request_ptr);

  ULOG_RT_PRINTF_7( npa.log,
                    "npa_release_and_schedule_client_qtime "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(time: 0x%016llx) (late_prob: %d)"
                    "(notification: 0x%08x) (context: 0x%x) ",
                    client,
                    client->name, 
                    ULOG64_LOWWORD(request_time),
                    ULOG64_HIGHWORD(request_time),
                    late_probability,
                    notification,
                    context );

  /* Mark that there is a release and schedule in progress */
  client->request_ptr->implied_state = NPA_IMPLIED_COMPLETE_PENDING;

  /* schedule the client */
  return(npa_schedule_client_internal( client,
                                       request_time,
                                       late_probability,
                                       notification,
                                       context));
}


/**
 * <!-- npa_unschedule_request -->
 *
 * @brief Remove client from scheduled request mode.
 *
 * Takes a client and if it is in Scheduled Resource mode, moves it back
 * into normal operation mode (i.e. no scheduled).
 *
 * The notification callback setup by npa_schedule_client may still be invoked
 * by the scheduler thread if the request is currently being processed.
 *
 * This function is safe to call even if the client does not currently have an
 * active request scheduled against it but will error if it is not a 
 * schedulable client.
 *
 * @note If the scheduled request was started with a call to
 * npa_release_and_schedule_client(), the request has not been handled
 * by the NPA Scheduler and the implied complete has  not happened,
 * the implied complete will be issued in the caller's thread before
 * returning from this function.  This is to force the client request
 * into a known state - either the complete or the request has been
 * handled. 
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 *
 * @return Return value is the notification state of this request.
 */
npa_scheduled_notification_state 
    npa_unschedule_request(npa_client_handle client)
{
  npa_scheduler_data *request_ptr;
  uint64 start_time;

  /* assume that the request has not started so will be cleanly unscheduled */
  npa_scheduled_notification_state 
    notify_state = NPA_SCHEDULED_NOTIFICATION_UNSCHEDULED;

  /* time for measuring duration */
  start_time = CoreTimetick_Get64();

  /* Verify that inputs are valid */
  CORE_VERIFY_PTR(client);

  request_ptr = client->request_ptr;

  if (NULL == request_ptr)
  { /* not a schedulable client so ignore the call */

    ULOG_RT_PRINTF_2( npa.log, "npa_unschedule_request "
                      "(client: 0x%08x) (client_name: \"%s\") "
                      "Not a schedulable client",
                      client, client->name);
    return notify_state;
  }
  
  ULOG_RT_PRINTF_4( npa.log, "npa_unschedule_request "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\")",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state));

  /* Lock the scheduler work loop.
   * Note that if the request is being processed, then
   * this lock will block until it is completed.
   */
  Core_MutexLock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_4( npa_scheduler.log, "\tnpa_unschedule_request Locked "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\")",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state));

  switch (request_ptr->state)
  {

    case NPA_SCHEDULED_CLIENT_INACTIVE:
      /* Nothing to do. */
      notify_state = request_ptr->notify_state;
      /* told someone about it so state needs cleaned up */
      request_ptr->notify_state = NPA_SCHEDULED_NOTIFICATION_NO_ACTION;
      break;

    case NPA_SCHEDULED_CLIENT_LINKED:
      /* client still in link list so we can remove it */
      npa_scheduler_remove(client,
                           NPA_UPDATE_START_TIMES,
                           NPA_LOCK_LIST);

      ULOG_RT_PRINTF_3( npa_scheduler.log, 
                        "\tnpa_unschedule_request Removed from List "
                        "(client: 0x%08x) (client_name: \"%s\") "
                        "(state: \"%s\")",
                        client, client->name,
                        npa_scheduled_client_state_name(request_ptr->state));
    /* Fall into NPA_SCHEDULED_CLIENT_EXPECTING state */

    case NPA_SCHEDULED_CLIENT_UNLINKED: /* may have hit the window in scheduler
                                         * task where it was removed from the
                                         * queue but preempted before locking
                                         * so not processed yet.
                                         */
      /* If an implied complete is expected, then complete the previous 
       * request so the client is in a known state.  We are protected
       * by the scheduler lock up above.  Don't do this for EXPECTING
       * as no issue_request has occured yet so the handshake is not
       * complete.
       */
      if (NPA_IMPLIED_COMPLETE_PENDING == request_ptr->implied_state)
      { /* Complete is expected */
        /* Remove from the implied complete list */
        npa_list_Remove(&npa_scheduler.implied_completes_now_list, client);
        /* Remove from the implied_completes_planB list, if present */
        npa_list_Remove(&npa_scheduler.implied_completes_planB_list, client);
        npa_issue_implied_complete(client);
      }
      if (NPA_IMPLIED_COMPLETE_EXECUTED == request_ptr->implied_state)
      { /* something happened so update state */
        notify_state = NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED;
      }
      
    /* Fall into NPA_SCHEDULED_CLIENT_EXPECTING state */

    case NPA_SCHEDULED_CLIENT_EXPECTING:
      /* No issue request has happened yet so backout the scheduled request */
      /* restore original issue request */
      client->issue_request = request_ptr->issue_request;
      /* No implied complete */
      request_ptr->implied_state = NPA_NO_IMPLIED_COMPLETE;
      request_ptr->issue_request = NULL; /* no dangling pointers */

      /* Request is no longer active */
      request_ptr->state = NPA_SCHEDULED_CLIENT_INACTIVE;
      break;

    case NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED:
      /* Client was forked and is awaiting a notify */
      ULOG_RT_PRINTF_2( npa_scheduler.log, 
                        "\tnpa_unschedule_request Notify Expected"
                        "(client: 0x%08x) (client_name: \"%s\")",
                        client, client->name);

      ++npa_scheduler.stats.forced_notifies;

      npa_scheduler_remove(client, NPA_DONT_UPDATE_START_TIMES, 
                                   NPA_DONT_LOCK_LIST);
      
      if ( request_ptr->implied_state == NPA_IMPLIED_COMPLETE_PENDING &&
           npa_compare_times( CoreTimetick_Get64() + npa_scheduler.config_ptr->auto_complete_delta,
                              client->request_ptr->request_time ) < 0 )
      {
        /* If an implied_complete was pending on the client and the unschedule
         * request was received much before request_time, execute the complete
         * before notifying the client of a successful unschedule.
         */

        ULOG_RT_PRINTF_3( npa_scheduler.log, "\tNeed Implied Complete "
                          "(client: 0x%08x) (start_time: 0x%016llx)",
                          client, 
                          ULOG64_LOWWORD(request_ptr->start_time),
                          ULOG64_HIGHWORD(request_ptr->start_time));

        /* Remove client from implied_completes_now list if on it */
        npa_list_Remove(&npa_scheduler.implied_completes_now_list, client);
         
        npa_terminate_request(client);
         
        request_ptr->issue_request = NULL;

        request_ptr->state = NPA_SCHEDULED_CLIENT_INACTIVE;
        request_ptr->implied_state = NPA_IMPLIED_COMPLETE_EXECUTED;
        request_ptr->notify_state = NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED;
      }
      else
      {
        npa_scheduler_notify(client, NULL);
      }

      /* something happened so update state */
      notify_state = request_ptr->notify_state;

      /* request is now in a known state - either "completed" or "applied" */
      break;

    case NPA_SCHEDULED_CLIENT_IN_PROGRESS:
    case NPA_SCHEDULED_CLIENT_FORKING:
    case NPA_SCHEDULED_CLIENT_UNDEFINED:
    default:
      /* Can't be in these states when requests are unscheduled */
      CORE_VERIFY( 0 );
      break;
  }

  /* the "last" request is now being reported when this function returns,
   *  so any other unschedules will not do any work until something else
   * is scheduled.
   */
  request_ptr->notify_state = NPA_SCHEDULED_NOTIFICATION_NO_ACTION;

  /* Setup timer that may have changed */
  npa_setup_next_timeout();

  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_6( npa_scheduler.log, "\tDONE npa_unschedule_request "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\") "
                    "(notify_state: \"%s\") "
                    "(dur: %d ticks)",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state),
                    npa_scheduler_status_name(notify_state),
                    (int32) npa_compare_times(CoreTimetick_Get64(), 
                                              start_time));

  return (notify_state);
}


/**
 * <!-- npa_sync_scheduled_request -->
 *
 * @brief Force immediate exection of any pending scheduled request on a client
 *
 * Takes a client and if it is in Scheduled Resource mode with a pending
 * issued request, forces it to execute immediately, else it will remove any
 * client waiting for the issue. 
 *
 * The notification callback setup by npa_schedule_client will be invoked
 * if the scheduled issued request is pending or currently being processed.
 *
 * This function is safe to call even if the client does not currently have an
 * active request scheduled against it but will error if it is not a 
 * schedulable client.
 *
 * @param client : Handle of the client for the request. Client handles are
 * the same as the address of the npa_client structure.
 *
 * @return Return value is the notification state of this request.  Return
 * NPA_SCHEDULED_NOTIFICATION_NO_ACTION if there was no issued request pending.
 * Else will return NPA_SCHEDULED_NOTIFICATION_LATE
 */
npa_scheduled_notification_state 
     npa_sync_scheduled_request(npa_client_handle client)
{
  npa_scheduler_data *request_ptr;
  uint64 start_time;

  /* Assume no action state */
  npa_scheduled_notification_state 
    notify_state = NPA_SCHEDULED_NOTIFICATION_NO_ACTION;

  /* time for measuring duration */
  start_time = CoreTimetick_Get64();

  /* Verify that inputs are valid */
  CORE_VERIFY_PTR(client);

  request_ptr = client->request_ptr;

  if (NULL == request_ptr)
  { /* not a schedulable client so ignore the call */

    ULOG_RT_PRINTF_2( npa.log, "npa_sync_scheduled_request "
                      "(client: 0x%08x) (client_name: \"%s\") "
                      "Not a schedulable client",
                      client, client->name);
    return notify_state;
  }
  
  ULOG_RT_PRINTF_4( npa.log, "npa_sync_scheduled_request "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\")",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state));

  /* Lock the scheduler work loop.
   * Note that if the request is being processed, then
   * this lock will block until it is completed.
   */
  Core_MutexLock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_4( npa_scheduler.log, "\tnpa_sync_scheduled_request Locked "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\")",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state));

  switch (request_ptr->state)
  {

    case NPA_SCHEDULED_CLIENT_INACTIVE:
      /* Nothing to do. */
      break;

    case NPA_SCHEDULED_CLIENT_EXPECTING:
      /* scheduled but no issue_request has happened */
      /* so restore original issue request */
      client->issue_request = request_ptr->issue_request;
      request_ptr->issue_request = NULL; /* no dangling pointers */

      /* Request is no longer active */
      request_ptr->state = NPA_SCHEDULED_CLIENT_INACTIVE;

      notify_state = request_ptr->notify_state;
      break;

    case NPA_SCHEDULED_CLIENT_LINKED:
      /* Request pending and client still in link list so we can remove it */
      npa_scheduler_remove(client,
                           NPA_UPDATE_START_TIMES,
                           NPA_LOCK_LIST);

      ULOG_RT_PRINTF_3( npa_scheduler.log, 
                        "\tnpa_sync_scheduled_request Removed from List "
                        "(client: 0x%08x) (client_name: \"%s\") "
                        "(state: \"%s\")",
                        client, client->name,
                        npa_scheduled_client_state_name(request_ptr->state));
    /* Fall into NPA_SCHEDULED_CLIENT_UNLINKED state */

    case NPA_SCHEDULED_CLIENT_UNLINKED: 
      /* may have hit the window in scheduler task where it was 
       * removed from the queue but preempted before locking
       * so not processed yet.
       */
      /* Run the request */
      ++npa_scheduler.stats.sched_requests_handled;

      /* run the request.
       * this call will clear out request_ptr->notification.
       */
      npa_run_scheduled_request(client,
                                &client->request_ptr->stats.full_issue_request_duration);
                                
      /* issue notification inside the lock */
      npa_scheduler_notify(client, NULL);

      /* something happened so update state */
      notify_state = request_ptr->notify_state;

      break;

    case NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED:
      /* Client was forked and is awaiting a notify */
      ULOG_RT_PRINTF_2( npa_scheduler.log, 
                        "\tnpa_sync_scheduled_request Notify "
                        "(client: 0x%08x) (client_name: \"%s\")",
                        client, client->name);

      /* Remove client from the forked list */
      npa_scheduler_remove( client, NPA_DONT_UPDATE_START_TIMES,
                                    NPA_DONT_LOCK_LIST );
      
      /* Remove from the implied_completes_now list if client is on it */
      if ( request_ptr->implied_state == NPA_IMPLIED_COMPLETE_PENDING )
      {
        npa_list_Remove(&npa_scheduler.implied_completes_now_list, client);
      }
      
      /* Notify client */
      npa_scheduler_notify(client, NULL);

      /* something happened so update state */
      notify_state = request_ptr->notify_state;

      /* Request is now applied */
      break;

    case NPA_SCHEDULED_CLIENT_IN_PROGRESS:
    case NPA_SCHEDULED_CLIENT_FORKING:
    case NPA_SCHEDULED_CLIENT_UNDEFINED:
    default:
      /* Can't be in these states when requests are sync-ed */
      CORE_VERIFY( 0 );
      break; 
  }
  
  /* Setup timer that may have changed */
  npa_setup_next_timeout();

  Core_MutexUnlock(npa_scheduler.scheduler_work_lock);

  ULOG_RT_PRINTF_6( npa_scheduler.log, "\tDONE npa_sync_scheduled_request "
                    "(client: 0x%08x) (client_name: \"%s\") "
                    "(state: %d - \"%s\") "
                    "(notify_state: \"%s\") "
                    "(dur: %d ticks)",
                    client, client->name,
                    request_ptr->state,
                    npa_scheduled_client_state_name(request_ptr->state),
                    npa_scheduler_status_name(notify_state),
                    (int32) npa_compare_times(CoreTimetick_Get64(), 
                                              start_time));

  return (notify_state);
}


/**
 * <!-- npa_update_scheduled_request_time -->
 *
 * @brief Update the request_time or deadline of a previously scheduled
 * request
 *
 * @param client : Handle of the client with the scheduled request
 * @param new_request_time : The new request_time or deadline
 *
 * @return Returns 0 if the deadline was successfully updated, -1 if not.
 * The call may fail if the client does not have a scheduled request set up
 * or if the scheduled request has already been picked up for processing.
 */
int npa_update_scheduled_request_time( npa_client_handle client,
                                       uint64 new_request_time )
{
  int ret = -1;
  CORE_VERIFY_PTR( client );
  CORE_VERIFY_PTR( client->request_ptr );

  ULOG_RT_PRINTF_5( npa.log, "npa_update_scheduled_request_time "
                    "(client: 0x%08x) (cur_deadline: 0x%016llx) (new_deadline: 0x%016llx)",
                    client,
                    ULOG64_LOWWORD(client->request_ptr->request_time),
                    ULOG64_HIGHWORD(client->request_ptr->request_time),
                    ULOG64_LOWWORD(new_request_time),
                    ULOG64_HIGHWORD(new_request_time) );

  Core_MutexLock( npa_scheduler.scheduler_work_lock );

  /* If state is LINKED, the client is either on the request queue or
     the complete queue */
  if ( client->request_ptr->state == NPA_SCHEDULED_CLIENT_LINKED )
  {
    npa_scheduler_queue_struct *queue = client->request_ptr->my_queue;

    npa_scheduler_remove( client, NPA_UPDATE_START_TIMES, NPA_LOCK_LIST );

    client->request_ptr->request_time = new_request_time;

    npa_scheduler_sort_insert( client, queue, NPA_UPDATE_START_TIMES );

    /* Update timer to the new start_time of the queue */
    npa_setup_next_timeout();

    ret = 0;
  }

  Core_MutexUnlock( npa_scheduler.scheduler_work_lock );

  ULOG_RT_PRINTF_2( npa.log, "\tDONE npa_update_scheduled_request_time "
                    "(client: 0x%08x) (ret: %d)", client, ret );
  return ret;
}

/**
 * <!-- npa_scheduler_status_name -->
 * 
 * @brief Status to a string request
 *
 * Returns a pointer to a constant string of the text for the
 * npa_scheduled_notification_state.
 *  
 * @param state : npa_scheduled_notification_state value
 * @param state : requested resource state
 *
 * @return Returns string ptr of the name.
 */
const char *npa_scheduler_status_name(
                   npa_scheduled_notification_state state)
{
  switch(state)
  {
    case NPA_SCHEDULED_NOTIFICATION_LATE:
      return("NPA_SCHEDULED_NOTIFICATION_LATE");

    case NPA_SCHEDULED_NOTIFICATION_ON_TIME:
      return("NPA_SCHEDULED_NOTIFICATION_ON_TIME");

    case NPA_SCHEDULED_NOTIFICATION_UNSCHEDULED:
      return("NPA_SCHEDULED_NOTIFICATION_UNSCHEDULED");

    case NPA_SCHEDULED_NOTIFICATION_NO_ACTION:
      return("NPA_SCHEDULED_NOTIFICATION_NO_ACTION");

    case NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED:
      return("NPA_SCHEDULED_NOTIFICATION_IMPLIED_COMPLETE_ISSUED");

    default:
      return("Unknown npa_scheduled_notification_state");
  }
}


/**
 * <!-- npa_scheduler_client_state_name -->
 *
 * @brief status to a string request
 *
 * Returns a pointer to a constant string of the text for the
 * npa_scheduled_notification_state.
 *
 * @param state: npa_scheduled_client_state value
 *
 * @return string ptr of the name.
 */
const char *npa_scheduled_client_state_name(npa_scheduled_client_state state)
{
  switch(state)
  {
    case NPA_SCHEDULED_CLIENT_INACTIVE:
      return "NPA_SCHEDULED_CLIENT_INACTIVE";
    
    case NPA_SCHEDULED_CLIENT_EXPECTING:
      return "NPA_SCHEDULED_CLIENT_EXPECTING";
    
    case NPA_SCHEDULED_CLIENT_LINKED:
      return "NPA_SCHEDULED_CLIENT_LINKED";
    
    case NPA_SCHEDULED_CLIENT_UNLINKED:
      return "NPA_SCHEDULED_CLIENT_UNLINKED";
    
    case NPA_SCHEDULED_CLIENT_FORKING:
      return "NPA_SCHEDULED_CLIENT_FORKING";
      
    case NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED:
      return "NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED";
    
    case NPA_SCHEDULED_CLIENT_IN_PROGRESS:
      return "NPA_SCHEDULED_CLIENT_IN_PROGRESS";
    
    case NPA_SCHEDULED_CLIENT_UNDEFINED:
      return "NPA_SCHEDULED_CLIENT_UNDEFINED";

    default:
      return "Unknown npa_scheduled_client_state";
  }
}


/**
 * <!-- npa_scheduler_clear_client_stats -->
 * 
 * @brief Clears a clients statistics area.
 *
 * @param client : Client handle for the clear
 */
void npa_scheduler_clear_client_stats(npa_client_handle client)
{
  memset(&client->request_ptr->stats,0,sizeof(client->request_ptr->stats));
}


/**
 * <!-- npa_scheduler_clear_control_stats-->
 * 
 * @brief Clears the request scheduler's statistics area.
 */
void npa_scheduler_clear_control_stats(void)
{
  memset(&npa_scheduler.stats,0,sizeof(npa_scheduler.stats));
}


/**
 * <!-- npa_scheduler_walk_list -->
 * 
 * @brief Debug check for a valid scheduled client list. 
 *
 * Walks through the scheduled client lists to verify that
 * it is consistent.  It makes sure all are in the state
 * of NPA_SCHEDULED_CLIENT_LINKED and that the count 
 * matches the links in the list.
 *
 * This is a blocking routine on each list.
 *  
 * @return Returns number of detected errors in all lists.
 */
uint32 npa_scheduler_walk_list(void)
{
  npa_client_handle client = NULL;
  uint32 count = 0;
  uint32 error = 0;
  npa_scheduler_queue_struct *list_ptr;
  int i;
  /* do this for multiple queues */
  npa_scheduler_queue_struct *lists[] =
        { &npa_scheduler.request_queue,
          &npa_scheduler.forked_queue,
          &npa_scheduler.complete_queue,
          NULL
        };

  for (i = 0, list_ptr = lists[0];
       NULL != list_ptr;
       list_ptr = lists[++i], count = 0)
  {
    Core_MutexLock(list_ptr->lock);

    /* check that head and tail make sense */
    if ((NULL == list_ptr->head
         && NULL != list_ptr->tail)
         || (NULL != list_ptr->head
             && NULL == list_ptr->tail))
    { /* head and tail don't agree on list being empty or not */
      ULOG_RT_PRINTF_3( npa.log,
        "npa_scheduler_walk_list out of sync"
        " - head and tail not agree with eachother "
        "(list: 0x%08x)"
        "(head: 0x%08x)"
        "(tail: 0x%08x)",
        list_ptr,
        list_ptr->head,
        list_ptr->tail);
      ++error;
      /* cannot trust list so go to next list */
      Core_MutexUnlock(list_ptr->lock);
      continue;
    }

    for (client = list_ptr->head;
         client != NULL;
         client = client->request_ptr->next)
    { /* for each client in the list */

      if (++count > list_ptr->num_linked_clients)
      { /* have more elements than list says should have */
        ULOG_RT_PRINTF_2( npa.log,
                          "npa_scheduler_walk_list out of sync - counts off"
                          " (count:%d) (num_linked: %d)",
                          count, list_ptr->num_linked_clients);
        ++error;
        /* can't trust list so don't go any farther */
        break;
      }

      if (NULL == client->request_ptr)
      {/* all things in the list should have a request_ptr */
        ULOG_RT_PRINTF_1( npa.log,
          "npa_scheduler_walk_list out of sync - no request_ptr "
          "(client: 0x%08x)",
          client);
        ++error;
      }

      if (!(client->request_ptr->state == NPA_SCHEDULED_CLIENT_LINKED
            || client->request_ptr->state 
                 == NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED)
          )
      { /* client in the list so must be CLIENT_LINKED state and isn't */
        ULOG_RT_PRINTF_2( npa.log,
          "npa_scheduler_walk_list out of sync - client in wrong state "
          "(client: 0x%08x) (state: \"%s\")",
          client, client->request_ptr->state);
        ++error;
      }

      if (!list_ptr->not_sorted && client->request_ptr->next)
      { /* sorted list and has a next so see if it is in order */
        if (client->request_ptr->request_time > client->request_ptr->next->request_ptr->request_time)
        { /* next's request time is before current's */
          ULOG_RT_PRINTF_7( npa.log,
            "npa_scheduler_walk_list out of sync - request_time out of time order "
            "(list: 0x%08x) "
            "(client: 0x%08x) "
            "(next: 0x%08x) "
            "(current's start: 0x%016llx) "
            "(next's start: 0x%016llx)",
            list_ptr,
            client,
            client->request_ptr->next,
            ULOG64_LOWWORD(client->request_ptr->request_time),
            ULOG64_HIGHWORD(client->request_ptr->request_time),
            ULOG64_LOWWORD(client->request_ptr->next->request_ptr->request_time),
            ULOG64_HIGHWORD(client->request_ptr->next->request_ptr->request_time));

          ++error;
        }
        if (client->request_ptr->start_time > client->request_ptr->next->request_ptr->start_time)
        { /* next's start time is before current's */
          ULOG_RT_PRINTF_7( npa.log,
            "npa_scheduler_walk_list out of sync - start_time out of time order "
            "(list: 0x%08x) "
            "(client: 0x%08x) "
            "(next: 0x%08x) "
            "(current's start: 0x%016llx) "
            "(next's start: 0x%016llx)",
            list_ptr,
            client,
            client->request_ptr->next,
            ULOG64_LOWWORD(client->request_ptr->start_time),
            ULOG64_HIGHWORD(client->request_ptr->start_time),
            ULOG64_LOWWORD(client->request_ptr->next->request_ptr->start_time),
            ULOG64_HIGHWORD(client->request_ptr->next->request_ptr->start_time));

          ++error;
        }
        if (client->request_ptr->start_time > client->request_ptr->request_time)
        { /* client's start time is after request time */
          ULOG_RT_PRINTF_7( npa.log,
            "npa_scheduler_walk_list out of sync - start_time > request_time "
            "(list: 0x%08x) "
            "(client: 0x%08x) "
            "(next: 0x%08x) "
            "(current's start: 0x%016llx) "
            "(current's request: 0x%016llx)",
            list_ptr,
            client,
            client->request_ptr->next,
            ULOG64_LOWWORD(client->request_ptr->start_time),
            ULOG64_HIGHWORD(client->request_ptr->start_time),
            ULOG64_LOWWORD(client->request_ptr->request_time),
            ULOG64_HIGHWORD(client->request_ptr->request_time));

          ++error;
        }
      }
    } /* for (client...) */

    if (count != list_ptr->num_linked_clients)
    {
      ULOG_RT_PRINTF_2( npa.log,
                        "npa_scheduler_walk_list out of sync - counts off "
                        "(count: %d) (num_linked_clients: %d)",
                        count, list_ptr->num_linked_clients);
      ++error;
    }

    Core_MutexUnlock(list_ptr->lock);
  } /* for (i...) */

  return(error);
}

/**
 * <!-- npaScheduler task -->
 *
 * @brief Wait on the npa_scheduler.events_array and process npa_scheduler_action_types
 *
 * @param unused_param: Parameter received from Main Control task - ignored
 */
void npaScheduler_task(unsigned long int unused_param)
{
  uint32 current_index;
  npa_scheduler_action action;

  /* Initialize the NPS Scheduler subsystem */
  npa_init_scheduler();

  /* Start the task */
  rcinit_handshake_startup();

  /* get thread id */
  npa_scheduler.thread_id = Core_ThreadSelf();

  /* Task processing */
  while( 1 )
  {
    /* Block until requests are complete */
    CORE_DAL_VERIFY( 
      DALSYS_EventMultipleWait(npa_scheduler.events_array, 
                               NPA_ARRAY_SIZE( npa_scheduler.events_array ),
                               DALSYS_EVENT_TIMEOUT_INFINITE,
                               &current_index));

    /* setup for common fields */
    action.type = (npa_scheduler_action_type)current_index;

    switch (current_index)
    {
  
      case NPA_SCHEDULER_ACTION_LPR_WAKEUP: /* LPR signaling task */      
        /* Make sure priority is now at highest value */
        /* Make sure priority is at desired value */
        Core_SetThreadPriority(npa_scheduler.thread_id,
                               npa_scheduler.config_ptr->scheduled_request_workloop.priority);
        /* Fall into next case */
      case NPA_SCHEDULED_ACTION_TIMEOUT:    /* Timeout event has occurred */
      case NPA_SCHEDULED_ACTION_TRIGGER:    /* Run the scheduled events */
        action.handler_fcn = npa_run_scheduler_requests;
        action.data = 0;
        action.handler_fcn(&action);
        break;

      case NPA_SCHEDULED_ACTION_IMPLIED_COMPLETE: /* Implied complete is needed */
        action.handler_fcn = npa_scheduler_handle_implied_complete;
        action.data = 0;
        action.handler_fcn(&action);
        break;

      default:
        /* If sig is out of valid range, err_fatal. Else, ignore */
        if ( current_index >= NPA_SCHEDULED_ACTION_UNDEFINED )
        {
          CORE_VERIFY( 0 );
        }
        break;
    }
  }
}
