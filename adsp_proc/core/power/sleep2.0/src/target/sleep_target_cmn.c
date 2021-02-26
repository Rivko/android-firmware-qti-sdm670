/*==============================================================================
  FILE:         sleep_target_cmn.c

  OVERVIEW:     This file provides common target-specific functionality for the
                modem Subsystem.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/target/sleep_target_cmn.c#2 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                             FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * sleepTarget_wakeupHandler
 * 
 * @brief Null handler for the registered idle timer interrupts
 */
void sleepTarget_wakeupHandler(uint32 param)
{
  return;
}

/** 
 * sleepTarget_aopWakeupHandler
 * 
 * @brief Null handler for the registered idle timer interrupts
 */
void sleepTarget_aopWakeupHandler(uint32 param)
{
  return;
}
