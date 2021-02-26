/*==============================================================================
  FILE:         island.c

  OVERVIEW:     This file provides island (hardware based) transition functions

  DEPENDENCIES: Functions are located in normal mode memory section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/transition/ddr/island.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_transi.h"
#include "island_trans.h"
#include "pdcTcs.h"

/*==============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleep_transitionInitialize
 */
void uSleep_transitionInitialize(void)
{
  /* Setup required hardware transition data */
  islandTrans_initialize();
  return;
}

/*==============================================================================
                              GLOBAL EXIT FUNCTIONS
 =============================================================================*/
/*
 * uSleep_completeExit
 * 
 * Performed after kernel stage 1 exit call
 */
void uSleep_completeExit(uSleep_transition_mode mode)
{
  /* Re-enable the CX wake resource */
  pdcTcs_enableCommand("cx.lvl", TCS_NUM_SLEEP0, TRUE);
  pdcTcs_enableCommand("cx.lvl", TCS_NUM_WAKE0, TRUE);
  return;
}

/*==============================================================================
                              GLOBAL ENTER FUNCTIONS
 =============================================================================*/
/*
 * uSleep_prepareForEntry
 */
void uSleep_prepareForEntry(void)
{
  /* Disable the CX wake resource */
  pdcTcs_enableCommand("cx.lvl", TCS_NUM_SLEEP0, FALSE);
  pdcTcs_enableCommand("cx.lvl", TCS_NUM_WAKE0, FALSE);
  return;
}

