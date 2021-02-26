/*==============================================================================
  FILE:         sleep_stats_cmn.c
  
  OVERVIEW:     Contains internal common variables and functions for the implementation 
                of sleep mode statistics and dynamic latency adjustments

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/statistics/sleep_stats_cmn.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep_stats_types.h"
#include "sleep_utils.h"

/*==============================================================================
                       INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepStats_updateValue
 */
void sleepStats_updateValue(sleepStats_generic *stats, uint64 value)
{
  stats->count++;
  stats->total = safe_unsigned_addition(stats->total, value);

  if(value > stats->max)
    stats->max = value;

  if(value < stats->min)
    stats->min = value;

  return;
}

