/*==============================================================================
  FILE:         sleep_stats.c
  
  OVERVIEW:     Contains variables and internal functions implementation of
                sleep mode statistics and dynamic latency adjustments
 
  DEPENDENCIES: None
 
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/statistics/sleep_stats.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include <stdlib.h>
#include <string.h>
#include "CoreVerify.h"
#include "sleep_lpr.h"
#include "sleep_lpri.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "SleepLPR_lookup_table.h"
#include "sleep_utils.h"

/*==============================================================================
                               GLOBAL VARIABLES
 =============================================================================*/
/* Tracks the amount of time spent in power collapse */
sleepStats_power_collapse_time g_sleepTotalPowerCollapseTime = {0};

/*==============================================================================
                           EXTERNAL GLOBAL VARIABLES
 =============================================================================*/
extern sleepStats_postedData g_sleepStatsPostedData;

/*==============================================================================
                          GLOBAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepStats_putLprLatency
 */
void sleepStats_putLprLatency(uint32 value, sleepStats_latency_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_LATENCY_TYPE);
  
  /* Verify synthLPRM enter & exit functions have retreived last value before
   * putting a new one */
  CORE_VERIFY(g_sleepStatsPostedData.latency[type] == 0);
  g_sleepStatsPostedData.latency[type] = value;
  return;
}

/*
 * sleepStats_putLprTimeData
 */
void sleepStats_putLprTimeData(uint64 value, sleepStats_timedata_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_TIMEDATA_TYPE);
  g_sleepStatsPostedData.time[type] = value;
  return;
}

/*
 * sleepStats_putMiscData
 */
void sleepStats_putMiscData(uint32 value, sleepStats_misc_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_MISC_TYPE);
  g_sleepStatsPostedData.misc_32bit[type] = value;
  return;
}

/*
 * sleepStats_updateValue32
 */
void sleepStats_updateValue32(sleepStats_generic32 *stats, uint32 value)
{
  stats->count++;
  stats->total = safe_unsigned_addition(stats->total, value);

  if(value > stats->max)
    stats->max = value;

  if(value < stats->min)
    stats->min = value;

  return;
}

/*
 * sleepStats_updateSignedValue 
 */
void sleepStats_updateSignedValue(sleepStats_generic_signed *stats,
                                  int64                      value)
{
  stats->count++;
  stats->total = safe_signed_addition(stats->total, value);

  if(value > stats->max)
    stats->max = value;

  if(value < stats->min)
    stats->min = value;

  return;
}

/*
 * sleepStats_getLPRHandle
 */
sleepStats_lpr_handle sleepStats_getLPRHandle(const char *LPRName)
{
  uint32                nLPR;
  sleepStats_lpr_handle result = 0;
  uint32                LPRCnt = sizeof(SleepLPR_LookupTable) / sizeof(SleepLPR_LookupTable[0]);

  CORE_VERIFY_PTR(LPRName);

  /* Loop through table entries for LPR name match */
  for(nLPR = 0; nLPR < LPRCnt; nLPR++)
  {
    /* Check for LPR name match */
    if(strcmp(LPRName, SleepLPR_LookupTable[nLPR]->resource_name) == 0)
    {
      sleep_lpr *pLPR = SleepLPR_LookupTable[nLPR];
      result          = (sleepStats_lpr_handle)pLPR;
      break;
    }
  }

  return result;
}

/*
 * sleepStats_getLPRStats
 */
void sleepStats_getLPRStats(sleepStats_lpr_handle LPRHandle,
                            sleepStats_mode_stats *modeStats)
{
  sleep_lpr *sleepLPR = (sleep_lpr *)LPRHandle;

  CORE_VERIFY_PTR(sleepLPR);

  /* lprHandle is currenlty a pointer to the LPR data structure which does not
   * currently contain information about the mode times. 
   * Set unused stats to 0 */ 
  modeStats->in_mode_time     = 0;
  modeStats->last_mode_time   = 0;

  /* Set LPR cycle count */
  modeStats->mode_run_cycles  = sleepLPR->run_count + sleepLPR->cacheable_run_count;

  return;
}

/*
 * sleepStats_trackPowerCollapseTime
 */
void sleepStats_trackPowerCollapseTime(uint8 enable)
{
  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 1,
                  "Tracking of power collapse times %s",
                  TRUE == enable ? "ENABLED" : "DISABLED");

  if(TRUE == enable)
  {
    /* Reset the stats on every enable */
    memset(&g_sleepTotalPowerCollapseTime.stats, 0, 
           sizeof(g_sleepTotalPowerCollapseTime.stats));

    g_sleepTotalPowerCollapseTime.tracking_enabled = TRUE;
  }
  else
  {
    g_sleepTotalPowerCollapseTime.tracking_enabled = FALSE;
  }

  return;
}

/*
 * sleepStats_getPowerCollapseTime
 */
const sleepStats_mode_stats *sleepStats_getPowerCollapseTime(void)
{
  return (&g_sleepTotalPowerCollapseTime.stats);
}

