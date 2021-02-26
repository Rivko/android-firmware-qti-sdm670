/*==============================================================================
  FILE:         clockgate_lpr.c

  OVERVIEW:     This file provides uSleep clock gating LPR functions

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/lpr/clockgate_lpr.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_target.h"

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/* 
 * uSleepLPR_clockgateEnter 
 */
void uSleepLPR_clockgateEnter(uint64 wakeupTime, boolean reEntrant)
{
  if(TRUE != reEntrant)
  {
    /* Enable clock gating (with no PMI interrupt)*/
    uSleepTarget_configPowerMode(TARGET_POWER_MODE_CLOCKGATE, FALSE);
  }

  return;
}

