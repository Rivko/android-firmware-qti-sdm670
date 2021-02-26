/*==============================================================================
  FILE:         sleep_npa.c
  
  OVERVIEW:     This file provides the NPA node definitions for latency and
                wakeup nodes.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_nodes/sleep_npa.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "sleep_npai.h"

/*==============================================================================
                            EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * sleepNPA_initialize
 */
void sleepNPA_initialize(void)
{
  /* Define the various sleep nodes */
  sleepMaxDurNode_define();
  sleepLatencyNode_define();

  return;
}

