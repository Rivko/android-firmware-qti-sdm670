/*==============================================================================
  FILE:         uSleep_os.c

  OVERVIEW:     This file provides the main uSleep functions that reside in
                island memory space

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/os/qurt/uSleep_os.c#6 $
$DateTime: 2018/01/03 05:43:53 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "qurt.h"
#include "utimer.h"
#include "uATS.h"
#include "uSleep_log.h"
#include "uSleep_os.h"
#include "uSleep_util.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleep_os_trans.h"
#include "dog_hal.h"
#include "uSleep_transi.h"
#include "sleep_os.h"
#endif

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Flag to indicate if exit routines should be run */
boolean g_uSleepRunExit = FALSE;

/*==============================================================================
                            INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepOS_performLPMExit
 */
void uSleepOS_performLPMExit(void)
{
  uint64                STMExit;
  unsigned int          interrupt;
  uSleep_solver_output  *solverOutput;

  interrupt    = qurt_system_vid_get();
  solverOutput = uATS_getSolverOutput();

  CORE_VERIFY(TRUE == g_uSleepRunExit);

  /* Verify that an exit funtion needs to be called */
  if(NULL != solverOutput->mode)
  {
    /* Run exit function */
    uSleepLPR_runLPRFunctions(FALSE, 0, FALSE, solverOutput->mode);
  }
  
  /* Clear the need to run the exit function */
  g_uSleepRunExit = FALSE;
    
  /* Record STM exit time */
  STMExit = uCoreTimetick_Get64();

  /* Exit STM */
  qurt_power_exit();

  uSleepLog_QDSSPrintf(USLEEP_LOG_LEVEL_PROFILING, 
                       USLEEP_MASTER_WAKEUP_NUM_ARGS,
                       USLEEP_MASTER_WAKEUP_STR, 
                       USLEEP_MASTER_WAKEUP,
                       interrupt,
                       MICRO_ULOG64_DATA(uSleep_getLastSleepWakeupInternal()),
                       MICRO_ULOG64_DATA(STMExit));

  return;
}

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepOS_performLPM
 */
uint32 uSleepOS_performLPM(void)
{
  uSleep_solver_output  *solverOutput;
  uSleep_solver_input   *solverInput;
  uint32                printCnt        = 0;

#ifdef USLEEP_SUPPORT_TRANSITIONS
  uSleep_internal_state state;
#endif

  /* Wait for all other HW threads to go idle.
   * 1. If override is set, we will enter all-wait when all threads go idle and
   *    this function does not return.
   * 2. If override is not set, this call will return when all threads go
   *    idle and we will be in STM 
   * 3. If the call is forced to return when the HW is not idle, the override bit
   *    setting is ignored, the function will return, but we will not be in STM.  This will
   *    be the case when uATS needs to run */ 
  if(0 != qurt_power_wait_for_idle())
  {
#ifdef USLEEP_SUPPORT_TRANSITIONS
    state = uSleep_getStateInternal();
    
    if(uSLEEP_INTERNAL_STATE_ACTIVE == state)
    {
      uATS_performSolving();
    }
    else
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1, 
                       "uSleepOS_performLPM forced idle exit (State: 0x%x)", 
                       state); 
    }
#else
    uATS_performSolving();
#endif

    /* Return value is currently meaningless, however just return a unique value different from 
     * other exit cases */
    return 2;
  }
 
#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Refresh current state of uSleep to verify that an exit request has not been
   * made. */
  state = uSleep_getStateInternal();
  
  /* Check if the function pointer has changed or if an island exit has been requested and
   * simply return from this function.
   * The exit request state check should not be true, as the exit routines should have
   * forced an exit from the wait_for_idle call above, but the logic is here just in case. */
  if((uSleepOS_getIdleFunctionPtr() != uSleepOS_performLPM) ||
     (state & (uSLEEP_INTERNAL_STATE_STAGE1_EXIT | 
               uSLEEP_INTERNAL_STATE_STAGE2_EXIT)))
  {
    uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 2,
                     "uSleep perform exit "
                     "(Reason: Function change or exit request) "
                     "(Fcn: 0x%x) "
                     "(state: 0x%x)",
                     (uint32)uSleepOS_getIdleFunctionPtr(),
                     state);

    /* Exit STM */
    qurt_power_exit();

    return 1;
  }
#endif

  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0, "uSleep cycle start");

  /* Get the last solver data from the active time solver */
  solverInput   = uATS_getSolverInput();
  solverOutput  = uATS_getSolverOutput();

  if ((NULL != solverOutput) && (NULL != solverOutput->mode))
  {
    /* Should not be here if mode was cacheable */
    CORE_VERIFY(FALSE == solverOutput->mode->cached);
  
    if(printCnt)
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1,
                       "Skipping LPM - no mode chosen "
                       "(Final count %d)",
                       printCnt);
    }

    /* Clear the mode skipped print counter when a mode is chosen */
    printCnt = 0;

    /* Last check to make sure we can enter the mode still. */
    if(solverInput->wakeup_deadline <= uCoreTimetick_Get64() ||
       ((solverOutput->threshold_deadline - uCoreTimetick_Get64()) < USLEEP_THRESHOLD_LIMIT_CHECK)
      )
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0,
                       "Skipping LPM - wakeup in past or threshold expired");

      /* Exit STM */
      qurt_power_exit();
      return 0;
    }

    /* Need to run the exit routine */
    g_uSleepRunExit = TRUE;

    /* Run the enter functions for the selected LPR that was previously chosen. */
    uSleepLPR_runLPRFunctions(TRUE, (solverInput->wakeup_deadline - solverOutput->mode->backoff), 
                              FALSE, solverOutput->mode);
  }
  else
  {
    /* Limit the message printing if we are continiously unable to choose a mode */
    if(0 == (printCnt++ % 24))
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1,
                       "Skipping LPM - no mode chosen "
                       "(Count %d)",
                       printCnt);
    }

    /* Exit STM */
    qurt_power_exit();
  }

  return 0;
}

#ifdef USLEEP_SUPPORT_TRANSITIONS
/*
 * uSleepOS_performError
 */ 
uint32 uSleepOS_performError(void)
{
  /* Set via fatal error exit from uImage transition. This function should
   * NEVER be allowed to run from OS scheduler, but core verify just in case. */
  uSleepOS_haltOnError();

  return 1; /* Just here for compliance with prototype. */
}

/*
 * uSleepOS_setIdleFunctionPtr
 */
void uSleepOS_setIdleFunctionPtr(uSleep_idle_entry_ptr_type newIdleEntryPtr)
{
  /* Use main sleeps function pointer setting function since uSleep is
   * running in the sleep thread */
  sleepOS_setLPIEntryFunction((sleep_idle_entry_ptr_type)newIdleEntryPtr);
}

/*
 * uSleepOS_getIdleFunctionPtr
 */
uSleep_idle_entry_ptr_type uSleepOS_getIdleFunctionPtr(void)
{
  /* Use main sleeps API to retrieve the current LPI function */
  return ((uSleep_idle_entry_ptr_type)sleepOS_getLPIEntryFunction());
}

/*
 * uSleepOS_haltOnError
 */
void uSleepOS_haltOnError(void)
{
  HAL_dogForceBite();
  while(1);
}

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/* 
 * uSleepOS_waitForTimerSync
 */
void uSleepOS_waitForTimerSync(void)
{
  /* wait for minimal timer difference to ensure a "few XO" cycles have passed */
  uint64        syncVal = uCoreTimetick_Get64() + 4;
  static uint32 cnt = 0;
  
  while(uCoreTimetick_Get64() < syncVal)
  {
    cnt++;
  }
  
  return;
}

/* 
 * uSleepOS_PMIPerformer
 */
void uSleepOS_PMIPerformer(void)
{
  /* HW workaround to allow the local timer to sync to the global timer */
  uSleepOS_waitForTimerSync();
  
#if defined(USLEEP_SUPPORT_UNCACHED_APCR)
  /* set the current time as the wakeup */
  uSleep_setLastSleepWakeupInternal(uCoreTimetick_Get64());

  /* Call exit function */
  uSleepOS_performLPMExit();
#endif

  return;
}

/* 
 * uSleepOS_enterLPMAbort
 */
void uSleepOS_enterLPMAbort(void)
{
  uSleep_solver_output *solverOutput;

  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0, "!! LRPM entry abort !!");

  /* LPRM entry was aborted, run the exit function and cleanup */
  if(TRUE == g_uSleepRunExit)
  {
    solverOutput = uATS_getSolverOutput();

    /* Verify that an exit funtion needs to be called */
    if(NULL != solverOutput->mode)
    {
      /* Run exit function */
      uSleepLPR_runLPRFunctions(FALSE, 0, FALSE, solverOutput->mode);
    }
    
    /* Clear the need to run the exit function */
    g_uSleepRunExit = FALSE;

    /* Exit STM */
    qurt_power_exit();
  }

  /* Force a re-solve */
  uATS_SetSignal(USLEEP_SIGNAL_HARD_DURATION);

  return;
}

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepOS_configIdleMode
 */
void uSleepOS_configIdleMode(uSleepOS_idleModeType  newIdleMode)
{
  CORE_VERIFY(newIdleMode < USLEEP_OS_IDLE_MODE_NUM_STATES);
  
  /* Mode specific action - currently it is just for one mode but if this 
   * grows, preferably use switch case. */
  if(USLEEP_OS_IDLE_MODE_OVERRIDE == newIdleMode)
  {
    /* No Sleep task intervention - QuRT performs all wait directly. */
    qurt_power_override_wait_for_idle(TRUE);
  }
  else
  {
    /* Sleep task needs to be scheduled. */
    qurt_power_override_wait_for_idle(FALSE);
  }

  return;
}

