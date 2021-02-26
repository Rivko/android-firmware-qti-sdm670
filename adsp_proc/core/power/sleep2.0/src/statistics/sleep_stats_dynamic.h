#ifndef _SLEEP_STATSDYNAMIC_H_
#define _SLEEP_STATSDYNAMIC_H_
/*==============================================================================
  FILE:         sleep_stats_dynamic.h
  
  OVERVIEW:     Internal generic types to gather and adjust sleep statistics for 
                synth modes
 
  DEPENDENCIES: None
 
                Copyright (c) 2013-2015,2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/statistics/sleep_stats_dynamic.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "sleep_stats.h"
#include "sleep_stats_types.h"
#include "sleep_target.h"

/*==============================================================================
                           INTERNAL TYPE DEFINITIONS
 =============================================================================*/
/** 
 * synth_LPRM_dyn_data
 * 
 * @brief Synthmode component based statistic data
 */
typedef struct synth_LPRM_dyn_data_s
{
  uint32                    backoff;      /* Adjusted synth LPRM backoff time */
  sleepStats_generic_signed wakeup_stats; /* Wakeup statistics for synth LPRM */
  uint32                    adjust_cnt;   /* Backoff adjustment count */
}synth_LPRM_dyn_data;

/** 
 * synth_LPR_dyn_data
 * 
 * @brief Synthmode based statistic data
 */
typedef struct synth_LPR_dyn_data_s
{
  /* Noting to track on LPR basis yet */
}synth_LPR_dyn_data;

/** 
 * sleep_LPRM_dyn_data
 * 
 * @brief Sleep component mode based statistic data
 */
typedef struct sleep_LPRM_dyn_data_s
{
  /* sleep component LPRM time statistics
   * 
   * Exit stats are not represented here because there is currently not
   * enough information at runtime to determine proper exit latency.
   */
}sleep_LPRM_dyn_data;

/** 
 * synth_dyn_data
 * 
 * @brief Root data structure that holds synth based statistics
 */
typedef struct synth_dyn_data_s
{
//  synth_LPR_dyn_data  LPR;
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  synth_LPRM_dyn_data LPRM[SLEEP_LPR_NUM_SYNTHESIZED_MODES];
#endif
}synth_dyn_data;

/** 
 * sleepStats_dynamic_data
 * 
 * @brief Master dynamic statistic data structure
 */
typedef struct sleepStats_dynamic_data_s
{
#if defined (SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT)
  synth_dyn_data          synth;                             /* Synth LPR(M)'s */
//  sleep_LPRM_dyn_data     sleep_LPRM[SLEEP_NUM_TOTAL_LPRMS]; /* Component LPRM's */
#endif
}sleepStats_dynamic_data;

#endif /* _SLEEP_STATSDYNAMIC_H_ */

