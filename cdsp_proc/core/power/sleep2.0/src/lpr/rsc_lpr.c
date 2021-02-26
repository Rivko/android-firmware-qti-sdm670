/*==============================================================================
  FILE:         rsc_lpr.c

  OVERVIEW:     This file provides the LPR definition for programming 
                the parent RSC power modes

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/lpr/rsc_lpr.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep_target.h"
#include "sleep_statsi.h"

/*==============================================================================
                              EXTERNAL LPR FUNCTIONS
 =============================================================================*/
/**
 * RSCLPR_chipSleepEnter
 *
 * @brief Enter function for RSC parent long sequence that handshakes with PDC
 */
void RSCLPR_chipSleepEnter(uint64 wakeupTick)
{
  sleepTarget_setRscLowPowerMode(RSC_PARENT, RSC_PARENT_MODE_CHIP_LPM);
  return;
}

/*==============================================================================
                             RSC Profiling Functions
 =============================================================================*/
/**
 * RSCLPR_profileExit
 *
 * @brief Updates LPRM exit specific profiling data which measures the RSCp time.
 *        This time is the same regardless of cacheable or uncacheable setting.
 */
uint64 RSCLPR_profileExit(sleepStats_timestamps *timeStamps, 
                          sleep_lprm            *self,
                          boolean               runAsCacheable)
{
  rsc_profile_unit  *start = &timeStamps->rsc_parent[STATS_RSC_PROFILE_UP_START];
  rsc_profile_unit  *end   = &timeStamps->rsc_parent[STATS_RSC_PROFILE_UP_END];

  /* If they aren't valid, then most likely it has been bypassed due to a pending interrupt */
  if((1 == start->valid) && (1 == end->valid))
  {
    CORE_VERIFY(end->timeStamp > start->timeStamp);

    /* We are cheating a bit here since RPMh driver actually will be providing the latency time.
     * Since they are doing that, and this LRPM is cacheable, it will not contribute any of its
     * own backoff latency. However, for debug/tracking purposes, store the actual RSCp time in the
     * uncacheable stats. */
    sleepStats_updateValue32(&self->mode_statistics.exit_lat, end->timeStamp - start->timeStamp);
  }

  return (self->cacheable_mode_statistics.last_exit_end - 
          self->cacheable_mode_statistics.last_exit_start); 
}

/**
 * RSCLPR_profileEnter
 *
 * @brief Updates LPRM entry specific profiling data which measures the RSCp time.
 *        This time is the same regardless of cacheable or uncacheable setting.
 *  
 * @note At the moment, this only tracks the HW time and not the SW entry function time  
 */
uint64 RSCLPR_profileEnter(sleepStats_timestamps  *timeStamps,
                          sleep_lprm              *self,
                          boolean                 runAsCacheable)
{
  rsc_profile_unit  *start = &timeStamps->rsc_parent[STATS_RSC_PROFILE_DN_START];
  rsc_profile_unit  *end   = &timeStamps->rsc_parent[STATS_RSC_PROFILE_DN_END];

  if((0 == start->valid) || (0 == end->valid))
  {
    return 0;
  }

  CORE_VERIFY(end->timeStamp > start->timeStamp);

  sleepStats_updateValue32(&self->mode_statistics.enter_lat, 
                           self->cacheable_mode_statistics.last_exit_end - 
                           self->cacheable_mode_statistics.last_exit_start);

  return(end->timeStamp - start->timeStamp);
}

