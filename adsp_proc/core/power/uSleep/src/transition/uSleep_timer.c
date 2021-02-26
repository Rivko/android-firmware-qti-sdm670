/*==============================================================================
  FILE:         uSleep_timer.c

  OVERVIEW:     This file provides the functions located in the island section
                to handle exit requirements for a DDR timer

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/transition/uSleep_timer.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep.h"
#include "uSleepi.h"
#include "uSleep_log.h"
#include "utimer.h"
#include "uSleep_timer.h"
#include "sleepActive.h"
#include "uSleep_util.h"
#include "sleep_os.h"
#include "uSleep_ddr_log.h"
#include "uATS.h"

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* Statically allocated stack memory for the task */
uint8 g_uSleepTimerTaskStack[DDR_TIMER_HANDLER_STACK_SIZE];

/* Signal that indicates to begin the uImage exit process due to our expired 
 * utimer */
qurt_anysignal_t g_uSleepTimerSignal;

/* Client handle of the registered utimer */
utimer_type g_uSleepTimerClient; 

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepTimer_timerSignalTask
 */
void uSleepTimer_timerSignalTask
(
  /* Parameter received from Main Control task - ignored */
  void *ignored
)
{
  uint32 signals;

  /* Begin task loop */
  while(1)
  {
    /* Wait for any work signals */
    signals = qurt_anysignal_wait(&g_uSleepTimerSignal, (USLEEP_TIMER_EXIT_EXPIRED));

    /* Reset the signal for next time */
    qurt_anysignal_clear(&g_uSleepTimerSignal, (USLEEP_TIMER_EXIT_EXPIRED));

    /* Check if DDR based uTimer expired */
    if(signals & USLEEP_TIMER_EXIT_EXPIRED)
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 0, "Timer exit");

      /* Begin the exit process so that we can handle the DDR timer */
      uSleep_exit();
    }
  }

  CORE_VERIFY(0);
}

/*
 * uSleepTimer_clearTimer
 */
void uSleepTimer_clearTimer(void)
{
  uint64 timeLeft;

  utimer_stop(&g_uSleepTimerClient, UT_TICK, &timeLeft);

  uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, (1 * 2), 
                   "Timer stop (Remain: 0x%llx)",
                   MICRO_ULOG64_DATA(timeLeft));
  return;
}

