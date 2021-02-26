/*==============================================================================
  FILE:         uSleep_os_trans.c

  OVERVIEW:     uSleep OS functions related specifically to transitions

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/os/qurt/ddr/uSleep_os_trans.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "timer.h"
#include "sleepActive.h"
#include "sleep_os.h"
#include "uSleep_util.h"
#include "uSleep_target.h"
#include "uSleep_lpr.h"
#include "uSleep_transi.h"
#include "uSleep_ddr_log.h"
#include "uSleep_os.h"
#include "uSleep_os_trans.h"

/*==============================================================================
                           INTERNAL TRANSITION FUNCTIONS
 =============================================================================*/
/** 
 * uSleepOS_beginEntryTransition
 * 
 * @brief Entry function to uSleep called from STM context. 
 *        uSleep entry is now committed.
 *  
 * @note Assumes function is called from STM via main sleep function 
 */
static void uSleepOS_beginEntryTransition(uint64 startTime)
{
  uSleep_internal_state         state         = uSleep_getStateInternal();
  uSleep_transition_profiling   *profileData  = uSleep_getProfilingDataPtr();
  const sleep_solver_deadlines  *deadlines    = sleepActive_GetSolverHardDeadlines();

  CORE_VERIFY_PTR(profileData);

  uSleepDDRLog_QDSSPrintf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 
                          USLEEP_ENTER_NUM_ARGS,
                          USLEEP_ENTER_STR, 
                          USLEEP_ENTER);

  /* Set timestamp of the start of the uSleep entry point */
  profileData->entry_start_time = startTime;

  /* Verify we are currently in normal mode before beginning transition
   * to island mode */
  CORE_VERIFY(uSLEEP_INTERNAL_STATE_IN_DDR == state);

  /* Only defer normal operational timers since we don't want to exit island
   * for a deferrable normal timer. */
  timer_defer_match_interrupt_64();

  /* Set full entry state */
  uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_FULL_ENTRY);
  
  /* Update the interrupt handler functions to the uImage version */
  uSleepTarget_setInterruptHandlers();

  /* Call LPR init functions */
  uSleepLPR_init();

  /* Transfer normal operational wakeup time to island system */
  uSleepTrans_setDDRWakeupTimeInternal(deadlines->normal_min);

  /* Call notification CB's while still in normal operational mode */
  uSleepTrans_transitionNotify(USLEEP_STATE_ENTER);

  /* Update the idle function pointer to the main transition function.
   * This is done because we must switch to an island function pointer. */
  uSleepOS_setIdleFunctionPtr(uSleepTrans_performIslandModeTransition);

  return;
}

/** 
 * uSleepOS_prepareForIslandEntry
 * 
 * @brief Prepares uSleep by ensuring various sleep options & 
 *        settings allow entry. If the function returns, it will have
 *        already placed the system in STM.         
 *    
 * @return Time of STM entry (in ticks) or 0 if unable to enter 
 *         any sleep (i.e. early exit cases)
 */
static uint64 uSleepOS_prepareForIslandEntry(void)
{
  /* Use the main sleep prepare function which performs all the
   * required actions by waiting for idle and verifying sleep modes */
  return(sleepOS_prepareForSleep(SLEEP_OS_IDLE_MODE_LPI));
}

/** 
 * uSleepOS_completeIslandExit
 * 
 * @brief Cleans up any actions on island exit that were done in the entry 
 *        preperation call 
 */
static void uSleepOS_completeIslandExit(void)
{
  /* Use the main sleep completion function which performs all the
   * required cleanup actions for normal sleep including STM exit */
  sleepOS_completeSleep(FALSE);
  return;
}

/*==============================================================================
                             EXTERNAL TRANSITION FUNCTIONS
 =============================================================================*/
/*
 * uSleepOS_mainEntry
 */ 
uint32 uSleepOS_mainEntry(void)
{
  uint64                      sleepStart;
  uSleep_idle_entry_ptr_type  uSleepFcnPtr;
  
  /* Update transition times if necessary */
  uSleepTrans_updateTimes();

  /* Allow island debug if option is enabled */
  uSleepTarget_enableIslandDebug(TRUE);

  /* Execute common sleep entry checks */
  sleepStart    = uSleepOS_prepareForIslandEntry();
  uSleepFcnPtr  = uSleepOS_getIdleFunctionPtr();

  if((0 == sleepStart) || (uSleepFcnPtr != uSleepOS_mainEntry))
  {
    uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 1,
                        "uSleep early exit (ptr: 0x%x)",
                        (uint32)uSleepFcnPtr);
    qurt_power_exit();

    /* Disallow island debug if option is enabled */
    uSleepTarget_enableIslandDebug(FALSE);

    return 0;
  }

  /***** Begin transition to uImage *****/
  uSleepOS_beginEntryTransition(sleepStart);

  return 0;
}

/*==============================================================================
                               EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * uSleepOS_cleanupIslandExit
 */
void uSleepOS_cleanupIslandExit(void)
{
  /* Complete and actions there were done at island entry by the perform call */
  uSleepOS_completeIslandExit();

  /* Disable island debug */
  uSleepTarget_enableIslandDebug(FALSE);

  return;
}

/*
 * uSleepOS_finalIslandExit
 */
void uSleepOS_finalIslandExit(void)
{
  /* Undefer normal operational timers that were deferred on the way into uSleep */
  timer_undefer_match_interrupt();
  return;
}

