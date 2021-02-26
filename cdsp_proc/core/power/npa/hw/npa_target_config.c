/*============================================================================
@file npa_target_config.c

NPA target configuration defaults. Copy this file to the specific target
directory and change as necessary.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1.c4/power/npa/hw/npa_target_config.c#1 $
============================================================================*/
#include "npa_config.h"
#include "npa_scheduler_config.h"

#define REQUEST_THREAD_PRIORITY 226
#define SCHEDULER_THREAD_PRIORITY 237

const npa_config npa_config_data =
{
  65536,                        /* Log Buffer Size */

  /* pool configuration */
  { 50, 10 },                   /* resources */
  { 130, 10 },                   /* clients */
  { 20, 10 },                   /* events */
  { 40, 10 },                   /* links */
  { 10, 10 },                   /* work queue elements */
  { 32, 16 },                   /* list elements */

  /* Workloop configuration */
  { REQUEST_THREAD_PRIORITY, 1 },  /* Async Request Workloop */
  { 0,  1 },                       /* Async Event Workloop */
  &npa_config_data /* table end - points to self */
};

/* Holder for Resource Override Data.  Uncomment and fill
 * in if there is a need to override defaults.
 */
const npa_resource_config npa_resource_overrides_data[] = 
{
  /* name, request, fork, join */
 { NULL, { 0, 0, 0 } }  /* Always last entry */
};

#ifdef NPA_SCHEDULED_REQUESTS_SUPPORTED
#include "npa_scheduler_internal.h"

/* default "constants" for npa_scheduler */
const npa_scheduler_config npa_scheduler_config_data = {
    32768,     /* Log Buffer Size */

  /* pool configuration */
  { 10, 10 },                   /* scheduled Workloop queue elements */
  { 10, 10 },                   /* scheduled request elements */

  /* Workloop configuration */
  { REQUEST_THREAD_PRIORITY, 1 }, /* Sched Request Workloop */

  /* default constants in QClks */
  200,       /* min_issue_sched_timeout - TTICKS */
  100,       /* sched_latency - TTICKS */
  20000,     /* min_reschedule_delta - TTICKS */
  20000,     /* fork_lookahead_delta - TTICKS */
  100,       /* resource request latency - TTICKS */
  100,       /* resource fork latency - TTICKS */
  100,       /* resource join latency - TTICKS */
  0,         /* sleep wake transition latency - TTICKS */
  100,       /* timer_queue_latency - TTICKS */
  165,       /* LPR overhead enter latency - TTICKS */
  2700,      /* LPR overhead exit latency - TTICKS */
  0,         /* lpr_now_delta - TTICKS */
  200000,    /* auto_complete_delta - TTICKS */
  245000,    /* clk_baseline - Units of clock request (KHz) */
  1,         /* implied_completes_on_wakeup (boolean) */  
  0,       /* Plan B Sleep Timer Duration - microseconds */
  (void *)(&npa_scheduler_config_data) /* always last entry */
};

/* RPM protocol dependant resource name - Must be NULL terminated */
const char *npa_lpr_issuable_resource_names_data[] 
  = {"/protocol/rpm/rpm",
     "/sched/test/request0",
      NULL};
#endif /* NPA_SCHEDULED_REQUESTS_SUPPORTED */
