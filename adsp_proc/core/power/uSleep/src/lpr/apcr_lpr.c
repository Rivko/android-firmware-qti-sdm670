/*==============================================================================
  FILE:         apcr_lpr.c

  OVERVIEW:     This file provides uSleep APCR LPR functions

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/lpr/apcr_lpr.c#3 $
$DateTime: 2018/01/03 05:43:53 $
==============================================================================*/
#include "comdef.h"
#include "qurt.h"
#include "uSleep.h"
#include "uSleep_os.h"
#include "uSleep_target.h"
#include "qdss.h"

extern void hwsolver_island_lpr_enter(void);
extern void hwsolver_island_lpr_exit(void);

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
/* 
 * uSleepLPR_apcrPlusEnter 
 */
void uSleepLPR_apcrPlusEnter(uint64 wakeupTime, boolean reEntrant)
{
  /* APCR mode with PMI interrupt since this is uncached */
  uSleepTarget_configPowerMode(TARGET_POWER_MODE_APCR_PLL_LPM, TRUE);

  /* Enable handshaking with the RSCp / HWsolver */
  uSleepTarget_setRpmHandshake(TRUE);

  /* Disable/re-enable modes at the HWSolver based on current latency
   * restriction and enter+exit latency of uATS-solved mode */
  hwsolver_island_lpr_enter();

  /*QDSS low power mode enter*/
  qdss_lpm_enter();
  
  /* Program the local wakeup time */
  if(TRUE == uSleepTarget_programWakeupTimer(wakeupTime))
  {
  /* Enter all wait */
  qurt_power_apcr_enter();
  }
  else
  {
    uSleepOS_enterLPMAbort();
  }

  /*QDSS low power mode exit*/
  qdss_lpm_exit();
  
  /* Log last HWSolved timeslot and mode */
  hwsolver_island_lpr_exit();

  return;
}

#else

/* 
 * uSleepLPR_cachedApcrPlusEnter
 */
void uSleepLPR_cachedApcrPlusEnter(uint64 wakeupTime, boolean reEntrant)
{
  if(TRUE != reEntrant)
  {
    /* In APCR cached mode, still fire the PMI interrupt for profiling data capture */
    uSleepTarget_configPowerMode(TARGET_POWER_MODE_APCR_PLL_LPM, FALSE);

    /* Enable handshaking with the RSCp / HWsolver */
    uSleepTarget_setRpmHandshake(TRUE);
  }

  /* Disable/re-enable modes at the HWSolver based on current latency
   * restriction and enter+exit latency of uATS-solved mode */
  hwsolver_island_lpr_enter();

  /* Since timers are always deferred, always program the wakeup time */
  if(TRUE != uSleepTarget_programWakeupTimer(wakeupTime))
  {
    uSleepOS_enterLPMAbort();
  }

  return;
}

#endif /* USLEEP_SUPPORT_UNCACHED_APCR */

