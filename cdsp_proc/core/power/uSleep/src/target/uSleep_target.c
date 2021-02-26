/*==============================================================================
  FILE:         uSleep_target.c

  OVERVIEW:     This file provides uSleep target specfic functions while in
                island mode

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/target/uSleep_target.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uSleep_target.h"
#include "qurt.h"
#include "uSleep_hwio.h"
#include "uSleep_log.h"
#include "uSleep_util.h"
#include "rsc.h"
#include "uATS.h"

/*==============================================================================
                        INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef DYNAMIC_LPR_CALLBACKS
/* 
 * uSleepTarget_callRegisteredCB
 * 
 * @brief Internal helper function used to call the user registered LPR callbacks
 *        when entering/exiting the power mode
 */
static void uSleepTarget_callRegisteredCB(uSleep_lpr_callback *lpr,
                                          uint64              wakeupTime)
{
  while(NULL != lpr)
  {
    uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1,
                     " Calling registered LPR: %s",
                     lpr->name);

    lpr->func(wakeupTime, lpr->userData);
    lpr = lpr->next;
  }
  return;
}
#endif /* DYNAMIC_LPR_CALLBACKS */

/*==============================================================================
                        EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepTarget_getCurrentModeLatency 
 */
const uSleep_lpr_latency *uSleepTarget_getCurrentModeLatency(void)
{
  uSleep_solver_output  *solverOutput = uATS_getSolverOutput();
  return &solverOutput->mode->working_latency;
}

/* 
 * uSleepTarget_enterIdle 
 */
void uSleepTarget_enterIdle(void)
{
  /* Wait for wakeup interrupt */ 
  qurt_power_wait_for_active();
  return;
}

/* 
 * uSleepTarget_configPowerMode
 */
void uSleepTarget_configPowerMode(target_power_mode mode, boolean enablePMI)
{
  uint32 wakeIRQ;
  
  /* Since the only mode supported is APCR, write HWIO directly here rather than
   * go through the extra logic in q6LPMConfig_setupModeConfig to keep it fast and simple */
  if(TARGET_POWER_MODE_CLOCKGATE == mode)
  {
    /* Clock gating bit overrides all other settings, so just set this one bit.
     * This setting will be in effect until explicitly unset */
    USLEEP_HWIO_OUTF(SLPC_CFG, CLK_GATING_MODE, 1);
  }
  else
  {
    wakeIRQ = (TRUE == enablePMI) ? 0x1 : 0x0;

    USLEEP_HWIO_OUTF(SLPC_CFG, CLK_GATING_MODE, 0);
    USLEEP_HWIO_OUTF(RSCCTL_EN_STATERET, WAKE_IRQ, wakeIRQ);
	
    /* Isalnd only operates on sensors PLL only*/
    USLEEP_HWIO_OUTF( RSC_EVENT_PLL_OVR, PLL_OVRRD,  0x1 );
  }

  return;
}

/*
 * sleepTarget_setRpmHandshake
 */
rsc_result_t uSleepTarget_setRpmHandshake(boolean performHandshake)
{
  uint16 request  = (TRUE == performHandshake) ? 1 : 0;
  uint16 nRequest = (TRUE == performHandshake) ? 0 : 1;

  /* Override the child -> parent handshake singnals */
  USLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, BRINGUP_REQ,  request);
  USLEEP_HWIO_OUTF(RSCCTL_EN_PRSC, SHUTDOWN_REQ, request);

  /* If not performing the handshake, we also need to override the ACK signal
   * in the sequence since it is generic */
  USLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, BRINGUP_ACK_OVRRD,  nRequest);
  USLEEP_HWIO_OUTF(RSC_EVENT_PRSC_OVR, SHUTDOWN_ACK_OVRRD, nRequest);

  return RSC_SUCCESS;
}

/* 
 * uSleepTarget_programWakeupTimer 
 */
boolean uSleepTarget_programWakeupTimer(uint64 wakeupTime)
{
  uSleep_setLastSleepWakeupInternal(wakeupTime);

  /* Program the wakeup time */
  uTimetick_SetWakeUpTimerInterrupt(wakeupTime);
  uTimetick_EnableWakeUpTimerInterrupt(TRUE);

  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, (1*2),
                   " Set wakeup (match: 0x%llx)",
                   MICRO_ULOG64_DATA(wakeupTime));

  /* Verify wakeup time is valid as late as possible */
  if(uCoreTimetick_Get64() >= wakeupTime)
  {
    return FALSE;
  }

  return TRUE;
}

/* 
 * uSleepTarget_enterLowPowerMode 
 */
void uSleepTarget_enterLowPowerMode(uSleep_LPR *mode, uint64 wakeupTime, boolean reEntrant)
{
  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 2,
                   "%s LPM: %s",
                   TRUE == reEntrant ? "Re-entering" : "Entering",
                   mode->name);

#ifdef DYNAMIC_LPR_CALLBACKS
  if(NULL != mode->registered_lprs.enter)
  {
    /* Dynamic reentrant callbacks are not supported in cacheable modes */
    CORE_VERIFY((FALSE == reEntrant) && (FALSE == mode->cached));

    /* Call any user registered CB's */
    uSleepTarget_callRegisteredCB(mode->registered_lprs.enter, wakeupTime);
  }
#endif

  /* Call the main function to enter power collapse */
  if(NULL != mode->enter_func)
  {
    mode->enter_func(wakeupTime, reEntrant);
  }

  return;
}

/* 
 * uSleepTarget_exitLowPowerMode 
 */
void uSleepTarget_exitLowPowerMode(uSleep_LPR *mode)
{
  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0, "Exiting LPM");

  if(NULL != mode->exit_func)
  {
    mode->exit_func();
  }

#ifdef DYNAMIC_LPR_CALLBACKS
  if(NULL != mode->registered_lprs.exit)
  {
    /* Call any user registered CB's */
    uSleepTarget_callRegisteredCB(mode->registered_lprs.exit, 0);
  }
#endif

  return;
}

