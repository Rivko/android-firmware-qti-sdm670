/*==============================================================================
  FILE:         client_API.c

  OVERVIEW:     This file provides uSleep API functions for user PD that are
                located in the normal operational memory section.

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/client_user/ddr/client_API.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleepi.h"
#include "uSleep_qdi.h"
#include "uSleep_util.h"
#include "qurt.h"
#include "client_user.h"

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* Callback function list pointer */
uSleep_callback_list *g_uSleepClientCallbacks = NULL;

/*==============================================================================
                           TYPE & MACRO DEFINITIONS
 =============================================================================*/
/* Macros for locking/unlocking the mutex around uSleep callback data access */
#define USLEEP_QDI_CB_LOCK()  qurt_mutex_lock(&g_uSleepQDICallbackMutex);
#define USLEEP_QDI_CB_FREE()  qurt_mutex_unlock(&g_uSleepQDICallbackMutex);

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/* Lock for callback registration */
qurt_mutex_t g_uSleepQDICallbackMutex;

/*==============================================================================
                      INTERNAL API HELPER FUNCTIONS
 =============================================================================*/
/**
 * uSleep_insertCallback
 * 
 * @brief Helper function that inserts a callback into the guest PD that will
 *        invoke the user PD callback task
 * 
 * @note  This function should not be called directly, it will be called
 *        appropriately via the registration API.
 * 
 * @param signal:   Signal structure to use for handshaking with guest PD
 * 
 * @return USLEEP_SUCCESS or USLEEP_FAILURE 
 */
static uint32 uSleep_insertCallback(uSleep_signals *signal)
{
  return qurt_qdi_handle_invoke(g_uSleepQDIClientHandle, 
                                USLEEP_QDI_INSERT_CALLBACK, signal); 
}

/**
 * uSleep_addUserCallbackLatency 
 *  
 * @brief Helper function that adds the enter/exit latencies for a callback 
 *        function
 *  
 * @note This function should not be called directly, it will be called 
 *       appropriately via the registration API.
 * 
 * @param enterLat: Enter latency (in ticks) of the callback being registered 
 * @param exitLat:  Exit latency (in ticks) of the callback being registered 
 *  
 * @return USLEEP_SUCCESS or USLEEP_FAILURE 
 */
static uint32 uSleep_addUserCallbackLatency(int32 enterLat, int32 exitLat)
{
  return qurt_qdi_handle_invoke(g_uSleepQDIClientHandle,
                                USLEEP_QDI_ADD_CALLBACK_LATENCY, 
                                enterLat, exitLat);
}

/*==============================================================================
                          EXTERNAL API FUNCTIONS
 =============================================================================*/
/*
 * uSleep_deregisterNotificationCallback
 * 
 * Note that the user version of this function does not remove the root->user PD
 * signal, only the actual callback.  This means that the user callback thread will still
 * be signaled, however it will not run any removed callback.
 */
uSleep_status uSleep_deregisterNotificationCallback(uSleep_notification_cb_handle handle)
{
  uSleep_callback_list  *current;
  uSleep_callback_list  *previous = NULL;
  uSleep_status         status    = USLEEP_FAILURE;

  /* Get data lock while we remove the callback. */
  USLEEP_QDI_CB_LOCK();

  current = g_uSleepClientCallbacks;

  /* Find given handle */
  while(NULL != current)
  {
    if(handle == (uSleep_notification_cb_handle)current)
    {
      if(NULL != previous)
      {
        /* Remove & free given callback */
        previous->next = current->next;
      }
      else
      {
        g_uSleepClientCallbacks = current->next;
      }

      /* Remove the original latency of this callback.
       * Any errors will be simply re-adjusted naturally. */
      uSleep_addUserCallbackLatency((0 - (int32)current->seedEnterLat),
                                    (0 - (int32)current->seedEnterLat));

      free(current);
      status = USLEEP_SUCCESS;
      break;
    }

    previous  = current;
    current   = current->next;
  }

  USLEEP_QDI_CB_FREE();

  return status;
}

/*
 * uSleep_registerNotificationCallback
 */
uSleep_notification_cb_handle uSleep_registerNotificationCallback(
                                uint32                      enterLat,
                                uint32                      exitLat,
                                uSleep_notification_cb_type callback)
{
  uSleep_callback_list **workingList;

  /* Nothing to do if callback wasn't provided. */
  if(NULL == callback)
  {
    return 0;
  }

  /* Get data lock while we insert the callback. */
  USLEEP_QDI_CB_LOCK();

  workingList = &g_uSleepClientCallbacks;

  /* If this is the first callback being added we need to add a signal 
   * callback to the guest os */
  if(NULL == *workingList)
  {
    CORE_VERIFY(USLEEP_SUCCESS == 
                uSleep_insertCallback(&g_uSleepCallbackSignal)); 
  }
  else
  {
    /* Move to last one in the list */
    do
    {
      /* Verify callback is not already registered */
      CORE_VERIFY((*workingList)->callback != callback);

      /* Move to next entry */
      workingList = &((*workingList)->next);
    }while(NULL != *workingList);
  }

  /* Create new callback element */
  CORE_VERIFY_PTR(*workingList = calloc(1, sizeof(uSleep_callback_list)));
  
  USLEEP_QDI_CB_FREE();

  (*workingList)->callback      = callback;
  (*workingList)->seedEnterLat  = enterLat;
  (*workingList)->seedExitLat   = exitLat;

  /* Add the latency of this callback. */
  uSleep_addUserCallbackLatency(enterLat, exitLat);

  return (uSleep_notification_cb_handle)(*workingList);
}

