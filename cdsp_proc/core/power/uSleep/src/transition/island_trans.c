/*==============================================================================
  FILE:         island_trans.c

  OVERVIEW:     This file provides island transition hardware routines located in island mode

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/transition/island_trans.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "DDIInterruptController.h"
#include "island_trans.h"
#include "uSleep_log.h"
#include "uSleep_hwio.h"
#include "rsc.h"
#include "busywait.h"

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * islandTrans_triggerTransition
 */ 
void islandTrans_triggerTransition(island_soft_trigger triggerType)
{
  static uint32 override;
  
  CORE_VERIFY(triggerType < ISLAND_SW_TRIGGER_LAST);

  uSleepLog_printf(USLEEP_LOG_LEVEL_TRANSITION, 1, 
                   "Begin bus transition (Mode: %s)",
                   ISLAND_SW_TRIGGER_ENTER == triggerType ? "Isolation" : "Deisolation");
  
  if(ISLAND_SW_TRIGGER_ENTER == triggerType)
  {
    /* Workaround for QCTDD03833969 */
    override = USLEEP_HWIO_INF(CGC_OVERRIDE, PUB_AHBS_CLK_EN);

    USLEEP_HWIO_OUTF(CGC_OVERRIDE, PUB_AHBS_CLK_EN, 1);
    CORE_VERIFY(RSC_SUCCESS == rsc_trigger_seq(RSC_CHILD, RSC_SW_TRIG_BUS_ISOLATION));
    USLEEP_HWIO_OUTF(CGC_OVERRIDE, PUB_AHBS_CLK_EN, override);

    /* Shutoff GCC to SSC clocks */
    HWIO_OUTF(USLEEP_HWIO_ISLD_GCC_CLK, DISABLE_GCC_TO_SSC_CLKS, 1);
  }
  else
  {
    /* Enable GCC to SSC clocks */
    HWIO_OUTF(USLEEP_HWIO_ISLD_GCC_CLK, DISABLE_GCC_TO_SSC_CLKS, 0);
    busywait(1);

    USLEEP_HWIO_OUTF(CGC_OVERRIDE, PUB_AHBS_CLK_EN, 1);
    CORE_VERIFY(RSC_SUCCESS == rsc_trigger_seq(RSC_CHILD, RSC_SW_TRIG_BUS_DEISOLATION));
    USLEEP_HWIO_OUTF(CGC_OVERRIDE, PUB_AHBS_CLK_EN, override);
  }

  return;
}

