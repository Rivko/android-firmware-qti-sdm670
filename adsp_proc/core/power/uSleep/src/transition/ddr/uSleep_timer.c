/*==============================================================================
  FILE:         uSleep_timer.c

  OVERVIEW:     This file provides the functions located in the normal section
                to handle exit requirements for a DDR timer

  DEPENDENCIES: None
 
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/transition/ddr/uSleep_timer.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "CoreTime.h"
#include "uSleep.h"
#include "uSleepi.h"
#include "uSleep_ddr_log.h"
#include "utimer.h"
#include "uSleep_timer.h"

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
extern uint8            g_uSleepTimerTaskStack[DDR_TIMER_HANDLER_STACK_SIZE];
extern qurt_anysignal_t g_uSleepTimerSignal;
extern utimer_type      g_uSleepTimerClient; 

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepTimer_init
 */
void uSleepTimer_init(void)
{
  int                 status;
  qurt_thread_t       tid;
  qurt_thread_attr_t  attr;
  
  /* Initilize the DDR timer thread attributes */ 
  qurt_thread_attr_init(&attr);
  
  /* Initilize the thread signal */  
  qurt_anysignal_init(&g_uSleepTimerSignal);

  /* Initilize the timer client */
  CORE_VERIFY(UTE_SUCCESS == utimer_def_osal(&g_uSleepTimerClient, 
                                             UTIMER_NATIVE_OS_SIGNAL_TYPE, 
                                             &g_uSleepTimerSignal, USLEEP_TIMER_EXIT_EXPIRED));

  /* Stack memory has to be statically allocated due to island section
   * location requirement */
  qurt_thread_attr_set_stack_size(&attr, DDR_TIMER_HANDLER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&attr, (void *)g_uSleepTimerTaskStack); 

  /* Fill in the stack with a marker to try and find overflows */
  memset(g_uSleepTimerTaskStack, 0xF8, sizeof(g_uSleepTimerTaskStack));

  qurt_thread_attr_set_priority(&attr, DDR_TIMER_HANDLER_PRIORITY);
  qurt_thread_attr_set_tcb_partition(&attr, 1); /* task is located in uImage */
  qurt_thread_attr_set_name(&attr, "uSleep_TmrHnd");

  /* Create the DDR timer handler task in uImage section memory */
  status = qurt_thread_create(&tid, &attr, uSleepTimer_timerSignalTask, NULL);

  CORE_LOG_VERIFY(status == QURT_EOK,
                  uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 0,
                                      "uSleepTimer_init: "
                                      "creation failed"));

  return;
}

/*
 * uSleepTimer_setValue
 */
void uSleepTimer_setValue(uint64 deadline)
{
  uint64 now = CoreTimetick_Get64();

  CORE_VERIFY(deadline > now);

  /* Create a uTimer from the given DDR timer that will fire early enough
   * so that uImage has exited by the time the DDR timer fires */
  CORE_VERIFY(UTE_SUCCESS == utimer_set_absolute(&g_uSleepTimerClient, deadline)); 
  return;
}

