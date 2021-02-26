/*==============================================================================
  FILE:         uATS.c
  
  OVERVIEW:     This file implements the micro-Active Time Solver DDR functions
  
  DEPENDENCIES: None

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/uATS/ddr/uATS.c#1 $
$DateTime: 2018/07/30 01:21:36 $
===============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "uATS.h"
#include "qurt.h"
#include "qurt_mutex.h"
#include "uSleep_ddr_log.h"
#include "utimer.h"
#include "uSleep_solver.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleep_node.h"
#endif

#ifdef UNPA_SUPPORT
#include "uSleep_npa.h"
#include "uSleep_npai.h"
#include "unpa.h"
#include "unpa_event.h"
#endif

/*==============================================================================
                              EXTERANL REFERENCES
  =============================================================================*/
/* Signal Object used for communicating with Active Time solver */
extern qurt_signal_t        g_uActiveSignal;
extern uSleep_solver_input  g_uSolverInput;
extern utimer_type          g_uATSThresholdTimer;
extern qurt_mutex_t         g_uATSSignalMutex;

#ifdef UNPA_SUPPORT
extern unpa_query_handle    g_latencyQueryHandle;
extern unpa_event           g_uSleepCpuVddEvent;
extern unpa_event           g_uSleepLatencyEvent;

void uATS_nodeChangeCB(unpa_event_type  eventType,
                       void             *eventData, 
                       void             *cbdata);
#endif /* UNPA_SUPPORT */

void uSleepNPA_initialize(void);

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
  =============================================================================*/
/*
 * uATS_init 
 */
void uATS_init(void)
{
#ifdef UNPA_SUPPORT
  unpa_event_handle latencyEventHandle;
  unpa_event_handle cpuVddEventHandle;

  /* Setup Solver Input Defaults */
  g_uSolverInput.latency_budget  = UINT32_MAX;
#endif

  g_uSolverInput.wakeup_deadline = UINT64_MAX;

  /* Initialize signal type */
  qurt_signal_init(&g_uActiveSignal);

  /* Init the signal mutex */
  qurt_mutex_init(&g_uATSSignalMutex);

  /* Defining threshold timer and a non-deferrable timer change event
   * Function type is used because uATS is not it's own thread and is invoked in
   * sleep context which needs special handling. */
  CORE_VERIFY(UTE_SUCCESS == 
               utimer_def_osal(&g_uATSThresholdTimer,       /* Timer obj */
                               UTIMER_FUNC1_CB_TYPE,        /* Notify type */
                               uATS_timerThreshHandlerCB,   /* Callback fcn */
                               0));                         /* User Context */

  /* Make the threshold timer deferrable so it doesn't effect any sleep decisions */
  utimer_set_deferrable(&g_uATSThresholdTimer, TRUE);

  CORE_VERIFY(UTE_SUCCESS == utimer_register_for_non_defer_expiry_change(
                              UTIMER_FUNC1_CB_TYPE,
                              uATS_timerEventHandlerCB,
                              0));

#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Setup uSleep nodes.  This needs to be done after the ATS signal init, and before any
   * query handles are created */
  uSleepNode_initialize();
#endif

#ifdef UNPA_SUPPORT
  uSleepNPA_initialize();

  /* This assumes that NPA initilization is done, and the node is already created */
  CORE_VERIFY_PTR(g_latencyQueryHandle = unpa_getQueryHandle(USLEEP_LATENCY_NODE_NAME));

  /* Register for a change event on the various nodes */
  CORE_VERIFY_PTR(latencyEventHandle = unpaEvent_newCallback(USLEEP_LATENCY_NODE_NAME,
                                                             UNPA_POST_CHANGE_EVENT,
                                                             uATS_nodeChangeCB,
                                                             (void*)USLEEP_SIGNAL_LATENCY,
                                                              &g_uSleepLatencyEvent));

  CORE_VERIFY_PTR(cpuVddEventHandle = unpaEvent_newCallback(USLEEP_CPUVDD_LPR_NODE_NAME,
                                                            UNPA_POST_CHANGE_EVENT,
                                                            uATS_nodeChangeCB,
                                                            (void*)USLEEP_SIGNAL_REGISTRY,
                                                            &g_uSleepCpuVddEvent));
#endif /* UNPA_SUPPORT */

  uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 0, "uATS Boot complete");

  return;
}

