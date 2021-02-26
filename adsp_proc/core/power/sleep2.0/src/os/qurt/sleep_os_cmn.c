/*==============================================================================
  FILE:         sleep_os_cmn.c

  OVERVIEW:     This file provides the sleep task and OS-specific
                island functionality for the processors with Qurt running on them.

  DEPENDENCIES: sleepOS_mainTask() is the entry point to the lowest-priority
                thread processing.

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/os/qurt/sleep_os_cmn.c#2 $
$DateTime: 2017/09/13 08:52:52 $ 
==============================================================================*/
#include <stdlib.h>
#include "qurt.h"
#include "CoreVerify.h"
#include "sleep_os.h"
#include "sleep_osi.h"
#include "sleep_log.h"

/*==============================================================================
                   INTERNAL VARIABLES IN ISLAND SECTION
 =============================================================================*/
/* Function pointer table for different Sleep behavior based on various
 * restrictions
 *
 *  DEFAULT   : Execute Idle-time Solver
 *  OVERRIDE  : qurt_override_wait_for_idle is used to perform lpm
 *  LPI       : Low power island function is used
 *  HOLD_OFF  : No Low-power Mode selection is performed
 */
sleep_idle_entry_ptr_type g_sleepIdleEntryTable[SLEEP_OS_IDLE_MODE_NUM_STATES] = 
{
  sleepOS_performLPM,      /* SLEEP_OS_IDLE_MODE_DEFAULT */
  sleepOS_performLPM,      /* SLEEP_OS_IDLE_MODE_OVERRIDE 
                            * Reuse same function since override is only a kernel setting */
  NULL,                    /* SLEEP_OS_IDLE_MODE_LPI */
  sleepOS_holdoffNoMode    /* SLEEP_OS_IDLE_MODE_HOLD_OFF */
};

/* Pointer to use as the entry point when the system is idle. */
volatile sleep_idle_entry_ptr_type * volatile   g_idleEntryPtr = &g_sleepIdleEntryTable[SLEEP_OS_IDLE_MODE_HOLD_OFF];

/* Function pointer to use for PMI exit handling. The pointer is set based on
 * if the system is in normal operational or island mode */
volatile sleep_PMI_handler_ptr_type             g_PMIFncPtr = NULL;

#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
/* Function pointer to use for profile handling. The pointer is set based on
 * if the system is in normal operational or island mode */
volatile sleep_profile_handler_ptr_type         g_profileFncPtr = NULL;
#endif

/* Main sleep task stack must be in TCM since task is also located in TCM. To
 * do that we have to statically declare an array in island section */
uint8                                           g_sleepTaskStack[SLEEP_STACK_SIZE];

/* PMI handler task stack must be in TCM since task is also located in TCM. To
 * do that we have to statically declare an array in island section */
uint8                                           g_sleepPMITaskStack[SLEEP_PMI_STACK_SIZE];

/* Signal that the PMI task waits on from the OS to begin the APCR exit process */ 
qurt_anysignal_t                                g_pmiSignal;

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepOS_setLPIEntryFunction
 */
void sleepOS_setLPIEntryFunction(sleep_idle_entry_ptr_type lpiFunction)
{
  g_sleepIdleEntryTable[SLEEP_OS_IDLE_MODE_LPI] = lpiFunction;
  return;
}

/*
 * sleepOS_getLPIEntryFunction
 */
sleep_idle_entry_ptr_type sleepOS_getLPIEntryFunction(void)
{
  return g_sleepIdleEntryTable[SLEEP_OS_IDLE_MODE_LPI];
}

/*
 * sleepOS_profilingHandler
 */
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
void sleepOS_profilingHandler(uint32 param)
{
  /* If there is a valid profiling function to call, call it when the interrupt has fired */
  if(NULL != g_profileFncPtr)
  {
    g_profileFncPtr();
  }

  return;
}
#endif

/*
 * sleepOS_pmiHandler
 */
void sleepOS_pmiHandler(void *context)
{
  /* Registering for PMI and associated signal */
  qurt_anysignal_init(&g_pmiSignal);

  /* Interrupt registration must be done in the context of the handler. */
  CORE_LOG_VERIFY(QURT_EOK == qurt_interrupt_register(SLEEP_PMI_ID, &g_pmiSignal, SLEEP_PMI_MASK),
                  sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0, 
                                  "Power Management Interrupt Registration failed"));

  /* Ensure interrupt is configured for rising edge */
  qurt_interrupt_set_config(SLEEP_PMI_ID, 1, 0);

  /* PMI interrupt monitoring and handling task */
  while(1)
  {
    qurt_anysignal_wait(&g_pmiSignal, SLEEP_PMI_MASK);
   
    /* Must always have a valid PMI function to call */
    CORE_VERIFY_PTR(g_PMIFncPtr);

    /* Call correct handler */
    g_PMIFncPtr();

    /* Signal is handled - Clear it, which must be done before ack */
    qurt_anysignal_clear(&g_pmiSignal, SLEEP_PMI_MASK);

    /* Interrupt has been handled, inform QuRT and clear interrupt */
    qurt_interrupt_acknowledge(SLEEP_PMI_ID);
  }
}

/**
 * sleepOS_mainTask
 * 
 * @brief Main sleep task that will call various low power modes
 *        when the system goes idle
 */
void sleepOS_mainTask
(
  /* Parameter received from Main Control task - ignored */
  void *ignored    /*lint -esym(715,ignored) */
)
{
  sleep_idle_entry_ptr_type sleepFunctionPtr;

  /* Read NV items, etc. here to check for modes that are disabled. */
  sleepOS_readEFSConfig();

  /* Main sleep task processing - Enter as many low power modes as possible. */
  while(1)
  {
    sleepFunctionPtr = *g_idleEntryPtr;

    if(sleepFunctionPtr != NULL)
    {
      /* Call the idle entry point.  This function pointer may be modified
       * by the /sleep/idle/plugin resource. */
      sleepFunctionPtr();
    }
  }

  CORE_VERIFY(0);
}

