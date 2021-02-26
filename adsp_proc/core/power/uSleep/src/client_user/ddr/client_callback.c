/*==============================================================================
  FILE:         client_callback.c

  OVERVIEW:     This file provides the initialization routines for uSleep in
                user PD's

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/client_user/ddr/client_callback.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep.h"
#include "uSleepi.h"
#include "uSleep_util.h"
#include "uSleep_qdi.h"
#include "qurt.h"
#include "client_user.h"

/*==============================================================================
                           TYPE & MACRO DEFINITIONS
 =============================================================================*/
/* Callback task priority */
#define USLEEP_PRI                0x00  /* Highest priority so it can be scheduled 
                                         * during kernel stage 1 exit which automatically
                                         * increases the sleep thread to highest */

/* Callback task stack size */
#define USLEEP_CLIENT_STACK_SIZE  2048

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
extern uSleep_callback_list *g_uSleepClientCallbacks;
extern qurt_mutex_t         g_uSleepQDICallbackMutex;

/*==============================================================================
                           FORWARD DECLARATIONS
 =============================================================================*/
void uSleep_callbackHandler(void *parameter);

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* Callback signals used to coordinate the handling of user & guest PD
 * notification callbacks */
uSleep_signals  g_uSleepCallbackSignal;

/*==============================================================================
                            INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleep_createClientThread
 * 
 * @brief Creates the client thread that will be used to call the registered
 *        transition callbacks in the same PD
 * 
 * @param Process ID of the current PD
 */ 
static void uSleep_createClientThread(unsigned int processId)
{
  qurt_thread_attr_t  tattr;
  unsigned int        stackbase;
  char                threadName[QURT_THREAD_ATTR_NAME_MAXLEN];
  qurt_thread_t       uSleepWorkerThreadID;
  int                 status;

  qurt_anysignal_init(&g_uSleepCallbackSignal.start);
  qurt_signal_init(&g_uSleepCallbackSignal.done);

  snprintf(threadName, sizeof(threadName), "uSlpClnt_%u", processId);
  stackbase = (unsigned int)malloc(USLEEP_CLIENT_STACK_SIZE);

  CORE_VERIFY_PTR(stackbase);
  
  qurt_thread_attr_init(&tattr);
  qurt_thread_attr_set_stack_size(&tattr, USLEEP_CLIENT_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&tattr, (void*)stackbase);
  qurt_thread_attr_set_priority(&tattr, USLEEP_PRI);
  qurt_thread_attr_set_name(&tattr, threadName);

  status = qurt_thread_create(&uSleepWorkerThreadID, &tattr,
                              uSleep_callbackHandler, NULL);

  CORE_VERIFY(QURT_EOK == status);

  return;
}

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * uSleep_callbackHandler 
 *  
 * @brief User mode callback task handler. This function will call the 
 *        user registered callback notifications and signal the guest OS when
 *        it is done.
 * 
 * @param parameter: unused
 */
void uSleep_callbackHandler(void *parameter)
{
  unsigned int              startSignal;
  unsigned int              doneSignal;
  uSleep_state_notification state;
  uSleep_callback_list      *workingList;

  while(1)
  {
    /* Wait for the signal */
    qurt_anysignal_wait(&g_uSleepCallbackSignal.start, 
                        (USLEEP_SIGNAL_START_ENTER | USLEEP_SIGNAL_START_EXIT));

    startSignal = qurt_anysignal_get(&g_uSleepCallbackSignal.start);
    doneSignal  = qurt_signal_get(&g_uSleepCallbackSignal.done);
   
    /* Ensure we have an expected signal */
    CORE_VERIFY(startSignal & (USLEEP_SIGNAL_START_ENTER | USLEEP_SIGNAL_START_EXIT));
  
    /* If guest OS didn't clear our done signal, something went wrong */
    CORE_VERIFY(0 == doneSignal);

    if(startSignal & USLEEP_SIGNAL_START_ENTER)
    {
      qurt_anysignal_clear(&g_uSleepCallbackSignal.start, 
                           USLEEP_SIGNAL_START_ENTER);

      state = USLEEP_STATE_ENTER;
    }
    else
    {
      qurt_anysignal_clear(&g_uSleepCallbackSignal.start, 
                           USLEEP_SIGNAL_START_EXIT);

      state = USLEEP_STATE_EXIT;
    }

    /* Loop through all the registered callbacks */
    workingList = g_uSleepClientCallbacks;
    while(NULL != workingList)
    {
      CORE_VERIFY_PTR(workingList->callback);

      /* Notify clients we are switching modes - return status is ignored for
       * now */
      workingList->callback(state);

      /* Move to next registered callback */
      workingList = workingList->next;
    }

    /* Tell guest OS we are done with the callbacks */
    qurt_signal_set(&g_uSleepCallbackSignal.done, USLEEP_SIGNAL_DONE_FINISHED);
  }/* End while (1) */

  CORE_VERIFY(0); /* loop should never break */
}

/**
 * uSleep_ClientQDIInit
 * 
 * @brief Client init function that will setup the uSleep system in the 
 *        current user PD 
 */
void uSleep_ClientQDIInit(void) 
{
  int uSleepClientPID = qurt_getpid();

  /* This function should not be called from guest OS PD.  A 0 value indicates
   * that is was. */
  CORE_VERIFY(0 != uSleepClientPID);

  /* Ensure this function is only called once per PD init */
  CORE_VERIFY(g_uSleepQDIClientHandle < 0);

  qurt_mutex_init(&g_uSleepQDICallbackMutex);

  /* Open the QDI handler for uSleep so API's may be called from user PD */
  g_uSleepQDIClientHandle = qurt_qdi_open(USLEEP_QDI_DRIVER_NAME);
  
  CORE_VERIFY(g_uSleepQDIClientHandle >= 0);
  
  /* Create user callback task */
  uSleep_createClientThread(uSleepClientPID);

  return;
}

