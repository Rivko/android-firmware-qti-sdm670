/*==============================================================================
  FILE:         uSleep_util.c

  OVERVIEW:     This file provides uSleep helper functions that are located in
                island memory space

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/util/uSleep_util.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "simple_solver.h"
#include "uSleep_util.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "qurt.h"
#include "uSleepi.h"
#include "uSleep_timer.h"
#include "uSleep_lpr.h"
#endif

#ifdef UNPA_SUPPORT
#include "uSleep_npa.h"
#endif

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/* Main uSleep data structure that stores state & other internal data */
uSleep_global_data g_uSleepData = {0};

#ifdef USLEEP_SUPPORT_TRANSITIONS
/* State change locking mutext */
qurt_mutex_t g_uSleepStateMutex;
#endif

/*==============================================================================
                           TYPE & MACRO DEFINITIONS
 =============================================================================*/
/* Macros for locking/unlocking the mutex around uSleep API's that will change
 * internal states */ 
#define USLEEP_STATE_LOCK()  qurt_mutex_lock(&g_uSleepStateMutex);
#define USLEEP_STATE_FREE()  qurt_mutex_unlock(&g_uSleepStateMutex);

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
#ifdef USLEEP_SUPPORT_TRANSITIONS
/*
 * uSleep_setStateInternal
 */
void uSleep_setStateInternal(uSleep_internal_state state)
{
  /* Only one state can be set, so ensure only 1 bit is set */
  CORE_VERIFY(state == (state & -state));

  USLEEP_STATE_LOCK();
  g_uSleepData.state = state;
  USLEEP_STATE_FREE();

  return;
}

/*
 * uSleep_getStateInternal
 */
uSleep_internal_state uSleep_getStateInternal(void)
{
  uSleep_internal_state state;

  USLEEP_STATE_LOCK();
  state = g_uSleepData.state;
  USLEEP_STATE_FREE();

  return (state);
}

/*
 * uSleep_getDDRWakeupTimeInternal
 */
uint64 uSleep_getDDRWakeupTimeInternal(void)
{
  return(g_uSleepData.ddr_wakeup_timer);
}

/*
 * uSleep_getProfilingDataPtr
 */
uSleep_transition_profiling* uSleep_getProfilingDataPtr(void)
{
  return &g_uSleepData.transition_data;
}

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/*
 * uSleep_setLastSleepWakeupInternal
 */
void uSleep_setLastSleepWakeupInternal(uint64 wakeupTime)
{
  g_uSleepData.last_wakeup_timestamp = wakeupTime;
}

/*
 * uSleep_getLastSleepWakeupInternal
 */
uint64 uSleep_getLastSleepWakeupInternal(void)
{
  return(g_uSleepData.last_wakeup_timestamp);
}

#ifdef UNPA_SUPPORT
/*
 * uSleep_setLowPowerModeInternal
 */
void uSleep_setLowPowerModeInternal(uint32 request)
{
  boolean enable  = TRUE;

  CORE_VERIFY(request <= USLEEP_CPUVDD_LPR_MAX_VALUE);

  /* Set the mode */
  g_uSleepData.power_mode_state = request;

  if(USLEEP_CPUVDD_LPR_DISABLE_LPM_PLUS == request)
  {
    enable = FALSE;
  }

  /* Enable or disable advanced power collapse modes accordingly */
  if(USLEEP_NUMBER_LOW_POWER_MODES)
  {
    /* Mode control in island is very simple for now - disable/enable the highest supported mode */
    uSleepLPR_setModeStatus((USLEEP_NUMBER_LOW_POWER_MODES - 1), enable);
  }

  return;
}

#endif /* UNPA_SUPPORT */

