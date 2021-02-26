/*==============================================================================
  FILE:         uSleep_os.c

  OVERVIEW:     This file provides uSleep OS API and functions that reside in
                normal mode memory space

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/os/qurt/ddr/uSleep_os.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_log.h"
#include "uSleep_target.h"
#include "uSleep_lpr.h"
#include "uATS.h"

#if defined(USLEEP_SUPPORTED_USER_PDS)
#include "uSleep_qdi.h"
#endif

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleep_os.h"
#include "uSleep_os_trans.h"
#include "uSleep_timer.h"
#include "uSleep_transi.h"
#include "uSleep_util.h"
#endif

/*==============================================================================
                            EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepOS_initialize
 */
void uSleepOS_initialize(void)
{
  /* Setup uSleep logging system */
  uSleepLog_initialize();

  /* Target specific initialization */
  uSleepTarget_initialize();

#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Setup default uSleep entry point */
  uSleepOS_setIdleFunctionPtr(uSleepOS_mainEntry);

  /* Transition initialization */
  uSleepTrans_initialize();

  /* Set uSleep initial internal state */
  uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_IN_DDR);

  /* Setup the handler for the DDR timer while in uImage */
  uSleepTimer_init();

#else

  /* Init the sleep task for standalone operation */
  uSleepOS_initializeTask();

  /* Setup the low power modes. This should be done after any target init, and if
   * transitions are supported, it will be called on every island entry. */
  uSleepLPR_init();

#endif

#if defined(USLEEP_SUPPORTED_USER_PDS)
  /* Setup uSleep QDI interface */
  uSleep_QDIInit();
#endif

  /* Setup uATS which must be done after all nodes are created */
  uATS_init();

  return;
}

