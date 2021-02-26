/*==============================================================================
  FILE:         uimage.c

  OVERVIEW:     This file provides uimage (software based) transition functions

  DEPENDENCIES: Functions are located in normal mode memory section

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/transition/island.c#3 $
$DateTime: 2018/01/02 22:13:27 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_trans.h"
#include "uSleep_transi.h"
#include "island_mgr.h"
#include "island_trans.h"
#include "uSleep_hwio.h"
#include "uSleep_util.h"
#include "uSleep_log.h"
#include "urpmh.h"
#include "uSleepi.h"
#include "qdss.h"

/*==============================================================================
                              GLOBAL VARIABLES
 =============================================================================*/
static volatile boolean g_uSleepKeepDDROn = FALSE;

/*==============================================================================
                              GLOBAL EXIT FUNCTIONS
 =============================================================================*/
/*
 * uSleep_prepareForExit
 *
 * Performed in uSleep_exit context before any kernel exit calls
 */
void uSleep_prepareForExit(uSleep_transition_mode mode)
{
  if(TRUE != g_uSleepKeepDDROn)
  {
    /* Trigger AMC's to exit island */
    urpmh_island_exit();

    /* Trigger AXI bus deisolation and begin HW island exit */
    islandTrans_triggerTransition(ISLAND_SW_TRIGGER_EXIT);

    /* Indicate out of HW island */
    HWIO_OUTF(USLEEP_HWIO_ISLD_GCC_CLK, ISLAND_MODE, 0);

    /*QDSS SSC island exit*/
    qdss_ssc_island_exit();
  }

  /* Continue the exit process */
  uSleepTrans_notifyComplete(FALSE);

  return;
}

/*==============================================================================
                              GLOBAL ENTER FUNCTIONS
 =============================================================================*/
/*
 * uSleep_completeEntry
 */
void uSleep_completeEntry(void)
{
  if(TRUE != g_uSleepKeepDDROn)
  {
    /* Trigger AMCs to enter island */
    urpmh_island_enter();

    /* Indicate island mode has been entered after sending AMC votes */
    HWIO_OUTF(USLEEP_HWIO_ISLD_GCC_CLK, ISLAND_MODE, 1);

    /*QDSS SSC island enter */
    qdss_ssc_island_enter();
  }

  return;
}

/*
 * uSleep_continueEntry
 */
void uSleep_continueEntry(void)
{
  if(TRUE != g_uSleepKeepDDROn)
  {
    /* Trigger AXI bus isolation and begin island entry */
    islandTrans_triggerTransition(ISLAND_SW_TRIGGER_ENTER);
  }

  /* Complete island entry by notifying it's complete */
  uSleepTrans_notifyComplete(TRUE);

  return;
}

/*
 * uSleep_fatalErrorHandler
 */
void uSleep_fatalErrorHandler(uSleep_internal_state state)
{
  uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1,
                   "USLEEP FATAL ERROR CALLED (state: %d)",
                   state);

  switch(state)
  {
    /* At beginning stages of entry -
     *
     * Nothing to do as the normal exit request was made before (or while in) call
     * to kernel entry */
    case uSLEEP_INTERNAL_STATE_FULL_ENTRY:
    {
      return;
    }
    /* In call to kernel entry -
     *
     * DDR/Bus isolation has not happened, so skip to calling the kernel exit routines
     * directly */
    case uSLEEP_INTERNAL_STATE_STAGE1_ENTRY:
    {
      uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_FATAL_EXIT);
      uSleepTrans_notifyComplete(FALSE);
      return;
    }

    /* Island entry transition is in progress -
     *
     * This stage is after kernel entry, but before final transition is complete.
     * Sleep is in the process of HW entry and must wait for the completion before allowing exit. */
    case uSLEEP_INTERNAL_STATE_STAGE2_ENTRY:
    {
      /* Trigger AXI bus deisolation and begin HW island exit */
      islandTrans_triggerTransition(ISLAND_SW_TRIGGER_EXIT);

      /* Indicate out of HW island */
      HWIO_OUTF(USLEEP_HWIO_ISLD_GCC_CLK, ISLAND_MODE, 0);

      /* Continue the exit process */
      uSleepTrans_notifyComplete(FALSE);
      break;
    }

    /* Island entry transition is finalizing -
     *
     * This stage is after hardware entry has completed and sleep is performing the final
     * stages of software entry */
    case uSLEEP_INTERNAL_STATE_STAGE3_ENTRY:
      uSleepTrans_islandControl(UIMAGE_EXIT_FAST);
    break;

    /* Standard exit has initially been requested -
     *
     * This stage is before kernel or HW exit has been triggered so just start the fatal
     * exit process */
    case uSLEEP_INTERNAL_STATE_REQ_EXIT:
      uSleepTrans_islandControl(UIMAGE_EXIT_FAST);
    break;

    /* Standard exit is in progress -
     *
     * This stage is before kernel exit call, but HW exit should be in progress which needs to
     * complete before continuing the exit */
    case uSLEEP_INTERNAL_STATE_STAGE1_EXIT:
    {
      /* Set the fatal exit state so when the transition notification is called it will perform
       * the correct behavior */
      uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_FATAL_EXIT);

      /* Perform the 'prepare' function again to ensure any HW requirements are done.
       * This should inturn continue the fatal exit case. */
      uSleep_prepareForExit(UIMAGE_EXIT_FAST);
      break;
    }

    /* Standard exit is in progress -
     *
     * Island has completed the HW exit transition so perform the rest of the SW fatal exit
     * routines */
    case uSLEEP_INTERNAL_STATE_STAGE2_EXIT:
    {
      uSleep_setStateInternal(uSLEEP_INTERNAL_STATE_FATAL_EXIT);
      uSleepTrans_notifyComplete(FALSE);
      break;
    }

    /* In island operational mode -
     *
     * Simply start the fatal error exit process */
    case uSLEEP_INTERNAL_STATE_ACTIVE:
    {
      /* In island operational mode, just call the island control API with the fatal flag */
      uSleepTrans_islandControl(UIMAGE_EXIT_FAST);
      break;
    }

    /* Invalid or recursive state - function should not have been called */
    default:
    {
      uSleepLog_QDSSPrintf(USLEEP_LOG_LEVEL_TRANSITION,
                           USLEEP_CRIT_ERR_NUM_ARGS,
                           USLEEP_CRIT_ERR_STR,
                           USLEEP_CRIT_ERR,
                           state);

    }
  }

  return;
}

