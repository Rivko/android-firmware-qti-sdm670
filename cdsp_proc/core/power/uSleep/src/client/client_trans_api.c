/*==============================================================================
  FILE:         client_trans_api.c

  OVERVIEW:     This file provides the public uSleep API functions related to 
                island transitions 

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/client/client_trans_api.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uSleep.h"
#include "uSleepi.h"
#include "sleep_stats_types.h"
#include "uSleep_log.h"
#include "uSleep_util.h"
#include "uSleep_timer.h"
#include "uSleep_transi.h"
#include "uSleep_os.h"
#include "uSleep_os_trans.h"
#include "qurt.h"
#include "uATS.h"

/*==============================================================================
                         EXTERNAL FUNCTION REFERENCES
 =============================================================================*/
void uSleep_fatalErrorHandler(uSleep_internal_state state);

/*==============================================================================
                           TYPE & MACRO DEFINITIONS
 =============================================================================*/
/* Macros for locking/unlocking the mutex around uSleep API's that will change
 * internal states */ 
#define USLEEP_LOCK()  qurt_mutex_lock(&g_uSleepMutex);
#define USLEEP_FREE()  qurt_mutex_unlock(&g_uSleepMutex);

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* API lock */
qurt_mutex_t g_uSleepMutex;

/*==============================================================================
                       EXTERNAL GUEST SECTION FUNCTIONS
 =============================================================================*/
/*
 * uSleep_exit
 */
uSleep_status uSleep_exit(void)
{
  uSleep_internal_state       state;
  uint32                      pendingInt    = qurt_system_vid_get();
  uSleep_transition_profiling *profileData  = uSleep_getProfilingDataPtr();
  qurt_thread_t               taskID        = qurt_thread_get_id();

  /* Since we are modifying the uSleep state to exit, only allow one client to
   * do that at a time */
  uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 2,
                   " Get exit mutex (Task: 0x%x) (VID: %u)",
                   taskID, pendingInt);

  USLEEP_LOCK();

  state = uSleep_getStateInternal();

  /* Check if we are in the middle of trying to enter island mode and someone
   * called exit */
  if(state & (uSLEEP_INTERNAL_STATE_STAGE1_ENTRY |
              uSLEEP_INTERNAL_STATE_STAGE2_ENTRY |
              uSLEEP_INTERNAL_STATE_FULL_ENTRY))
  {
    uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1,
                     " Waiting for entry completion (Task: 0x%x)", taskID);

    /* Wait for entry to complete before exiting */
    uSleepTrans_enterSignalControl(USLEEP_TRANS_SIGNAL_WAIT);

    /* Update the internal state after waiting */
    state = uSleep_getStateInternal();
  }

  /* If we are currently active, turn on DDR and wait for full exit to happen */
  if(state & uSLEEP_INTERNAL_STATE_ACTIVE)
  {
    /* Set exit requested state */
    uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_REQ_EXIT);

    /* Record the start of the exit sequence */
    profileData->exit_start_time = uCoreTimetick_Get64();

    uSleepLog_QDSSPrintf(USLEEP_LOG_LEVEL_PROFILING, 
                         USLEEP_EXIT_START_NUM_ARGS,
                         USLEEP_EXIT_START_STR, 
                         USLEEP_EXIT_START,
                         pendingInt);

    /* Clear the existing uTimer that was created based on the first normal
     * mode non-deferrable timer. */
    uSleepTimer_clearTimer();

    /* Reset the exit signal so we can block the caller until uSleep has fully 
     * exited */
    uSleepTrans_exitSignalControl(USLEEP_TRANS_SIGNAL_CLEAR);

    /* Exit island mode */
    uSleepTrans_islandControl(UIMAGE_EXIT);

    /* Block until the system has fully transitioned back to normal operational
     * mode */
    uSleepTrans_exitSignalControl(USLEEP_TRANS_SIGNAL_WAIT);
  }
  else if(0 == (state & uSLEEP_INTERNAL_STATE_IN_DDR))
  {
    /* Must be in DDR state if we reached this point, otherwise something went wrong */
    uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1,
                     " ERROR: bad state on uSleep exit (State: 0x%x)",
                     state);

    uSleepOS_haltOnError();
  }

  uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1, 
                   " Give exit mutex (Task: 0x%x)",
                   taskID);

  USLEEP_FREE();

  return USLEEP_SUCCESS;
}

/*
 * uSleep_exitOnError
 */
void uSleep_exitOnError(void)
{
  uSleep_internal_state state = uSleep_getStateInternal();

  /* Call the target specific error handler */
  uSleep_fatalErrorHandler(state);

  return;
}

