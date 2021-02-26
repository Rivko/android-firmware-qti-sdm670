/*==============================================================================
  FILE:         wakeup_lpr.c

  OVERVIEW:     This file provides the LPR definition for programming 
                the wakeup time for low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/lpr/wakeup_lpr.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "synthLPRM.h"
#include "rpmh_client.h"

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * cpuWakeupLPR_standaloneEnter
 *
 * @brief Enter function for standalone wakeup time programming
 */
void cpuWakeupLPR_standaloneEnter(uint64 wakeupTick)
{
  /* Program the wakeup time in the local timer.
   * Different timer resolutions may result in a slightly updated
   * programmed match value, so save the actual value for later use. */
  sleepStats_putLprTimeData(sleepTarget_enableSleepTimer(wakeupTick),
                            SLEEP_STATS_TIME_REQ_LOCAL_WAKE_TYPE);

  return;
}

/**
 * cpuWakeupLPR_standaloneSleepSetEnter
 *
 * @brief Enter function for standalone with sleep set wakeup time programming 
 *  
 * @note this is only used in cases where there is a HW solver (i.e. SLPI) 
 */
void cpuWakeupLPR_standaloneSleepSetEnter(uint64 wakeupTick)
{
  uint64 wakeTime;
  uint32 wakeSetLatency = rpmh_get_wake_latency(SLEEP_RPMH_DRV_MAP);

  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1,
                  " RPMh wake latency (value: 0x%x)",
                  wakeSetLatency);

  /* Factor in the wake set transition time to the programmed wakeup time */
  wakeTime = wakeupTick - wakeSetLatency;

  /* Since RPMh is giving it's estimate for the set transitions, do not include
   * that in our backoff calculations */
  sleepStats_putLprTimeData(sleepTarget_enableSleepTimer(wakeTime) + wakeSetLatency,
                            SLEEP_STATS_TIME_REQ_LOCAL_WAKE_TYPE);

  return;
}

/**
 * cpuWakeupLPR_RPMhEnter
 *
 * @brief Enter function for RPMh assisted wakeup time programming
 */
void cpuWakeupLPR_RPMhEnter(uint64 wakeupTick)
{
  uint64 wakeSetLatency = 0; 
  uint64 localWakeup;
  uint64 rpmhWakeup;
  uint64 programedWakeup;

  /* Include the wake set transition time in the local timer wakeup value */
  wakeSetLatency  = rpmh_get_wake_latency(SLEEP_RPMH_DRV_MAP);
  localWakeup     = wakeupTick - wakeSetLatency;

  /* Program the local wakeup timer */
  programedWakeup = sleepTarget_enableSleepTimer(localWakeup);

  /* Since RPMh is giving it's estimate for the set transitions, do not include
   * that in our backoff calculations */
  sleepStats_putLprTimeData(programedWakeup + wakeSetLatency, 
                            SLEEP_STATS_TIME_REQ_LOCAL_WAKE_TYPE);

  /* If local timer failed, no sense in spending cycles to program the RPMh time as
   * solver needs to run again which is signaled by the enableSleepTimer API above */
  if(0 != programedWakeup)
  {
    /* Backoff the PDC wake transition time and program PDC wake time.
     * This value is the worst case hard coded value for now */
    rpmhWakeup = programedWakeup - US_TO_TICKS(200);

    /* !! PDC timer wakeup HW bug workaround (QCTDD04481022)
     * !! Force a minimum value in the lower order bits so we can detect the
     * !! match value error */
    rpmhWakeup |= 0x20;
    sleepStats_putLprTimeData(programedWakeup, SLEEP_STATS_QTIMER_WAKEUP);
    sleepStats_putLprTimeData(rpmhWakeup, SLEEP_STATS_RPMH_WAKEUP);

    sleepTarget_enableRpmhTimer(rpmhWakeup);
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, (2*2),
                    " Program RPMh "
                    "(Match Tick: 0x%llx) "
                    "(Wake Set Latency: 0x%llx)",
                    ULOG64_DATA(rpmhWakeup),
                    ULOG64_DATA(wakeSetLatency));
  }

  return;
}

