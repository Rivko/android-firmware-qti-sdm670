/*==============================================================================
@file npa_scheduled_config.h

NPA Scheduled Request Configuration Structure Declarations

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/src/scheduler/npa_scheduler_config.h#1 $
============================================================================*/

#ifndef NPA_SCHEDULER_CONFIG_H
#define NPA_SCHEDULER_CONFIG_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "npa.h"
#include "npa_config.h"
#include "DALStdDef.h"

/* Configuration data for the NPA Scheduler
 * WARNING: If you change this structure, you must also update
 * npa_scheduler_config_data_property_struct[] in npa_scheduler.c
 */ 
typedef struct npa_scheduler_config
{
  unsigned int  log_buffer_size; /* Size in bytes */

  /* Allocations */
  npa_pool_config scheduled_queue_allocation;
  npa_pool_config scheduled_request_allocation;

  /* Thread config data */
  npa_workloop_config scheduled_request_workloop;

  /* minimum time period until the next timer s.t. a request will
   * be scheduled by an issue_request.  If the time is shorter, then
   * the request is handled "immediately" in the scheduler thread.
   * Count is in QTICKS
   */
  uint32 min_issue_sched_timeout;

  /* scheduler latency - Time it takes to handle the scheduler
   * code in QTICKS.
   */
  uint32 sched_latency;

  /* min_rescheduler_delta - Time that a request will be rescheduled
   * for npa_run_scheduler_requests() because it is too far into the future
   * to be handled by the current timeout.
   * This defined what "too far". 
   * In QTICKS. 
   */
  uint32 min_reschedule_delta;

  /* fork_lookahead_delta - Timeto add to the sleep wakeup time
   * in determining what is eligible to be forked.  It is to compensate
   * for the work sleep has to do in waking up.  This is added in addition to
   * the min_scheudle_delta. 
   * In QTICKS. 
   */
  uint32 fork_lookahead_delta;

  /* request_latency - default Scheduled request latency used if a scheduled
   * request is issued on a resource but it does not support the
   * NPA_QUERY_RESOURCE_REQUEST_LATENCY query. 
   * In QTICKS.
   * Does not include notify_latency.
   */
  uint32 request_latency;

  /* fork latency - default Scheduled fork latency used if a scheduled
   * request is issued on a resource but it does not support the
   * NPA_QUERY_FORK_REQUEST_LATENCY query. 
   * In QTICKS. 
   */
  uint32 fork_latency;

  /* notify latency - default Scheduled notify latency used if a scheduled
   * request is issued on a resource but it does not support the
   * NPA_QUERY_NOTIFY_REQUEST_LATENCY query. 
   * In QTICKS. 
   */
  uint32 notify_latency;

  /* sleep wake transition latence - time it takes sleep to come out
   * (LPR fires), timer to fire, and start of joins start firing.
   * Only subtracted from the target when telling sleep the must
   * wake time. 
   * In QTICKS. 
   */
  uint32 sleep_wake_transition_latency;

  /* timer queue latency - Time it takes for the timer to call the
   * NPA timer function.  Based upon work that may be ahead of the
   * NPA function.  This is subtracted from the current_timeout
   * before telling the timer when setting up the intial timeout
   * or in lpr_exit() when the request is not happening right
   * after sleep.
   * NOTE must be less than min_reschedule_delta. 
   * In QTICKS. 
   */
  uint32 timer_queue_latency;

  /* lpr_now_delta - Default time for LPR enter() to reschedule timer
   * to fire for "now" since 0 is possibly dangerous. 
   * In QTICKS. 
   */
  uint32 lpr_now_delta;

  /* auto complete delta - minimum time period where a forked request will 
   * will not be automatically complete if sleep wakes up early.  Used
   * by join code to determine if should call implied complete. 
   * In QTICKS. 
   */
  uint32 auto_complete_delta;

  /* clk_baseline - minimum value that NPA Scheduler will needs the clock to
   * be at for the scheduler latency calculations to be short.
   * Used to set the clock speed at the beginning of the Scheduler loop and in 
   * the LPR exit() if needed.
   */
  npa_resource_state clk_baseline;
  
  /* implied_completes_on_wakeup - Boolean - If deadlines of requests are
   * deemed far enough in the future that implied_completes need to be executed,
   * do so at wakeup, rather than from within npa_run_scheduler_forks, if this
   * value is 1
   */
  unsigned int implied_completes_on_wakeup;
  
  /* planB_implied_completes - Boolean - If set to 1, deferred implied
   * completes will be executed by the scheduler when the state of the
   * /core/cpu/busy resource changes to 1. If 0, the feature is disabled.
   */
  unsigned int planB_implied_completes;
  
  /* last entry in the structure.  Should be set to the address of the 
   * npa_scheduler_config table (i.e. &npa_scheduler_config_data)
   */
  const struct npa_scheduler_config* table_end;

} npa_scheduler_config;

#endif /* NPA_SCHEDULER_CONFIG_H */
