/*==============================================================================
  FILE:         sleep_statsi.c
  
  OVERVIEW:     Contains internal variables and functions for the implementation 
                of sleep mode statistics and dynamic latency adjustments

  DEPENDENCIES: None

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/statistics/sleep_statsi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdint.h>
#include <stdlib.h>
#include "comdef.h"
#include "sleep.h"
#include "CoreMutex.h"
#include "synthLPRM.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "sleep_stats_types.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "synthRegistry.h"
#include "sleep_utils.h"
#include "lpr_solver.h"
#include "pdcProfile.h"
#include "rsc.h"
#include "sleepActive.h"
#include "sleepActivei.h"

/*==============================================================================
                             GLOBAL VARIABLES
 ==============================================================================*/
/* Contains certain shutdown / wakeup data points used in processing timelines and latency */
sleepStats_postedData   g_sleepStatsPostedData;

/* Last cycle wakeup times */
sleepStats_timestamps   g_wakeupTimestamps = {0};

/* Debug data strcture for tracking of occurances of QCTDD04481022 */
struct
{
  uint32                  count;
  sleepStats_timestamps   timestamps[4];
  uint64                  rpmh_wakeup_req[4];
  uint64                  qtimer_wakeup_req[4];
}g_wakeupWorkaroundData = {0, {0}, {0}, {0}};

/* =============================================================================
                          GLOBAL EXTERNAL VARIABLES
 =============================================================================*/
extern sleepStats_power_collapse_time g_sleepTotalPowerCollapseTime;

/*==============================================================================
                      INTERNAL VARIABLE DECLARATIONS
 ==============================================================================*/
/* Head pointer for sleep frequency lookup tables */
static sleep_fLUT_node  *g_sleepFLUT = NULL;

/* Used to controll access to the mail fLUT list */
static CoreMutexType    *g_sleepfLUTMutex = NULL;

/* Static FLUT array to use for adding elements first before using dynamic
 * memory in the heap */
#if defined(SLEEP_NUM_STATIC_FLUTS) && (SLEEP_NUM_STATIC_FLUTS > 0)
static sleep_fLUT_node  g_staticFlutArray[SLEEP_NUM_STATIC_FLUTS];
#endif

/* Counter to recored the no. of times RSC child and parent timestamps have been corrupted  */
static volatile uint32 g_sleepTimestampCorruptionCount = 0;

/*==============================================================================
                        INTERNAL FORWARD DECLARATIONS
 ==============================================================================*/
static void sleepStats_initSynth(synth_dyn_data *synth);

/*==============================================================================
                       INTERNAL FUNCTION DEFINITIONS
 ==============================================================================*/
/*
 * sleepStats_logWakeupTimes
 */
static void sleepStats_logWakeupTimes()
{
  sleepLog_printf(SLEEP_LOG_LEVEL_WAKEUPS, 4 + (2 * 2),
                  "  Last PDC wakeup: (Start: 0x%llx  V/O: %u %u) (End: 0x%llx  V/O: %u %u)",
                  ULOG64_DATA(g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_START].timeStamp),
                  g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_START].valid,
                  g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_START].overflow,
                  ULOG64_DATA(g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_END].timeStamp),
                  g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_END].valid,
                  g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_END].overflow);

  sleepLog_printf(SLEEP_LOG_LEVEL_WAKEUPS, 4 + (2 * 2),
                  " Last RSCp wakeup: (Start: 0x%llx  V/O: %u %u) (End: 0x%llx  V/O: %u %u)",
                  ULOG64_DATA(g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_START].timeStamp),
                  g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_START].valid,
                  g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_START].overflow,
                  ULOG64_DATA(g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_END].timeStamp),
                  g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_END].valid,
                  g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_END].overflow);

  sleepLog_printf(SLEEP_LOG_LEVEL_WAKEUPS, 4 + (2 * 2),
                  " Last RSCc wakeup: (Start: 0x%llx  V/O: %u %u) (End: 0x%llx  V/O: %u %u) ",
                  ULOG64_DATA(g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_START].timeStamp),
                  g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_START].valid,
                  g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_START].overflow,
                  ULOG64_DATA(g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp),
                  g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].valid,
                  g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].overflow);

  return;
}

/*
 * sleepStats_init
 */
void sleepStats_init(void)
{
#if defined(SLEEP_NUM_STATIC_FLUTS) && (SLEEP_NUM_STATIC_FLUTS > 0)
  /* Clear static array memory */
  memset(g_staticFlutArray, 0, sizeof(g_staticFlutArray));
#endif

  /* Setup the fLUT data lock */
  CORE_VERIFY_PTR(g_sleepfLUTMutex = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));

  return;
}

/*
 * sleepStats_initDynamicData
 */
void sleepStats_initDynamicData(sleepStats_dynamic_data *data)
{
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  /* New data has already been set to '0' so only non-zero values need to be
   * set */
  sleepStats_initSynth(&data->synth);
#endif

  return;
}

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
/*
 * sleepStats_initSynth
 */
static void sleepStats_initSynth(synth_dyn_data *synth)
{
  uint32              nMode;
  synth_LPRM_dyn_data *synthData = synth->LPRM;

  for(nMode = 0; nMode < SLEEP_LPR_NUM_SYNTHESIZED_MODES; nMode++)
  {
    synthData[nMode].adjust_cnt       = SLEEPSTATS_INITIAL_CYCLE_COUNT;
    synthData[nMode].wakeup_stats.min = INT64_MAX;
    synthData[nMode].wakeup_stats.max = INT64_MIN;
  }

  /* Nothing to do for synth LPR */

  return;
}

/*
 * sleepStats_resetSynth
 */
void sleepStats_resetSynth(synth_LPRM_dyn_data *synth)
{
  synth->wakeup_stats.min    = INT64_MAX;
  synth->wakeup_stats.max    = INT64_MIN;
  synth->wakeup_stats.total  = 0;
  synth->wakeup_stats.count  = 0;

  return;
}

/*
 * sleepStats_verifyBackoffRange
 */
void sleepStats_verifyBackoffRange(uint32 currentBackoff,
                                   int32  newBackoff, 
                                   uint32 enterExitLatency)
{
  /* Minimum and maximum allowable backoff based on the current value */
  uint32 maxBackoff = currentBackoff * SLEEPSTATS_VALID_BACKOFF_MAX_FACTOR;
  uint32 minBackoff = currentBackoff / SLEEPSTATS_VALID_BACKOFF_MIN_FACTOR;

  /* Make sure new latency value is something reasonable:
   *  - Between min & max allowed (either error or print warning)
   *  - Less than than enter + exit latency */ 
  if(newBackoff < enterExitLatency)
  {
    if((newBackoff < minBackoff) || (newBackoff > maxBackoff))
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 5,
                      " WARNING: New backoff out of range "
                      "(Current: %u) (New: %u) "
                      "(Min: %d) "
                      "(Enter+Exit: %u) "
                      "(Max %d)",
                      currentBackoff,
                      newBackoff,
                      minBackoff,
                      enterExitLatency, 
                      maxBackoff);

#if (0 != SLEEP_BACKOFF_ADJUST_CRASH_OUT_OF_RANGE)
      CORE_VERIFY(0);
#endif
    }
  }
  else
  {
    CORE_LOG_VERIFY(0, 
                    sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 4,
                                    " ERROR: New backoff exceeds enter + exit latency " 
                                    "(Current: %u) (New: %u) "
                                    "(Min: %d) "
                                    "(Enter+Exit: %u) ",
                                    currentBackoff,
                                    newBackoff,
                                    minBackoff,
                                    enterExitLatency));
  }

  return;
}

/*
 * sleepStats_adjustSynthmode
 */
void sleepStats_adjustSynthmode(int64 wakeupDelta,
                                int64 wakeupAdjust)
{
  synth_LPRM_dyn_data       *dynamicLPRM;
  uint32                    *adjustCnt;
  sleepStats_generic_signed *synthLPRMStats;
  uint32                    *currentBackoff;
  sleep_fLUT_node           *fLUT;
  uint32                    dynNum;
  sleep_solver_output       *solverOutput = sleepActive_GetSolverOutput();
  sleep_solver_input        *solverInput  = sleepActive_GetSolverInput();

  fLUT            = solverInput->fLUT;
  dynNum          = solverOutput->selected_synthLPRM->dynamic_num;
  dynamicLPRM     = &fLUT->dynamic_data.synth.LPRM[dynNum];
  adjustCnt       = &dynamicLPRM->adjust_cnt;
  synthLPRMStats  = &dynamicLPRM->wakeup_stats;
  currentBackoff  = &dynamicLPRM->backoff;

  /* Remove any wakeup latency errors so only the masters local latency is adjusted */
  wakeupDelta -= wakeupAdjust;

  /* Ensure a single delta adjustment would not create a negative backoff.
   * If it would, log a message and force a crash here to catch this situation. */
  CORE_LOG_VERIFY((((int64)(*currentBackoff) + wakeupDelta) > 0) &&
                  (wakeupDelta < US_TO_TICKS(0.35 * 1000 * 1000)),
                  sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 1 + (2 * 2),
                                  " ERROR: Bad wakeup delta "
                                  "(Wakeup Delta: %lld) "
                                  "(Wakeup Error: %lld) "
                                  "(Current Backoff: %d) ",
                                  ULOG64_DATA(wakeupDelta),
                                  ULOG64_DATA(wakeupAdjust),
                                  *currentBackoff));
  
  /* Record wakeup statistics */
  sleepStats_updateSignedValue(synthLPRMStats, wakeupDelta);
  
  /* Adjust synth mode latency if it's time to do so */
  if(synthLPRMStats->count >= *adjustCnt)
  {
    int64   actualAvg;
    int64   adjustedTotal   = synthLPRMStats->total;
    int64   avgCount        = (int64)(synthLPRMStats->count);
    
    if(avgCount > 2)
    {
      /* Remove worst to cases from average*/
      adjustedTotal = adjustedTotal - synthLPRMStats->min - synthLPRMStats->max;
      avgCount -= 2;
    }

    /* Compute average synth mode exit time */
    actualAvg = adjustedTotal / avgCount;

    sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 
                        SLEEP_BKOFF_STATS_NUM_ARGS,
                        SLEEP_BKOFF_STATS_STR, 
                        SLEEP_BKOFF_STATS,
                        solverOutput->selected_synthLPRM->name,
                        fLUT->frequency,
                        safe_signed_truncate(synthLPRMStats->min),
                        safe_signed_truncate(synthLPRMStats->max),
                        ULOG64_DATA(synthLPRMStats->total),
                        synthLPRMStats->count,
                        safe_signed_truncate(actualAvg));

    /* Sanity checking */
    if((actualAvg > INT32_MIN) && (actualAvg < INT32_MAX))
    {
      /* Want to keep synth mode slightly early to provide a little buffer */ 
      if((actualAvg < -250) || (actualAvg > -50)) 
      {
        int32   adjustment;
        int32   newBackoff;
        uint32  seedEnterExitLatency;

#ifdef SLEEP_ENABLE_FREQUENCY_SCALING
        seedEnterExitLatency = solverOutput->selected_synthLPRM->seed_latency->enter_exit[statInput->fLUT->mLUT_idx];
#else
        seedEnterExitLatency = solverOutput->selected_synthLPRM->seed_latency->enter_exit[0];
#endif

        adjustment = (int32)(actualAvg + 150);
        newBackoff = (int32)(*currentBackoff) + adjustment;
        
        /* Validate the new backoff and take the appropriate action */
        sleepStats_verifyBackoffRange(*currentBackoff, (uint32)newBackoff, seedEnterExitLatency);

        sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 3 + (1 * 2),
                        " Adjusting (Wakeup Error: %lld) "
                        "(Current Backoff: %d) (Adjustment: %d) "
                        "(New Backoff: %d)",
                        ULOG64_DATA(wakeupAdjust),
                        *currentBackoff,
                        adjustment,
                        newBackoff);

        /* Set new backoff value */
        *currentBackoff = (uint32)newBackoff;

        /* Ensure ATS picks up the new backoff value */
        sleepActive_SetSignal(SLEEP_SIGNAL_WAKE_LATENCY_CHANGED);
      }
      else
      {
        sleepLog_printf(SLEEP_LOG_LEVEL_AUTO_LATENCY, 2,
                        " No adjustment needed (Current: %d) (avg: %d)",
                        *currentBackoff,
                        safe_signed_truncate(actualAvg));
      }

      /* Increase next adjust time but limit to maximum number of cycles  */
      if(*adjustCnt < SLEEPSTATS_FINAL_CYCLE_COUNT)
      {
        /* Shift the adjustment counter for the next group average */
        *adjustCnt = ((*adjustCnt) << 1);
      }
    }
    else
    {
      sleepLog_printf( SLEEP_LOG_LEVEL_AUTO_LATENCY, 0,
                       " Adjustment failed sanity check");
    }

    /* reset min/max stats for next run */
    sleepStats_resetSynth(dynamicLPRM);
  }

  return;
}
#endif /* SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT */

/*
 * sleepStats_readWakeupTimes
 */
void sleepStats_readWakeupTimes(sleepStats_timestamps *times)
{
  sleepTarget_getRscTimestamp(RSC_CHILD, times->rsc_child);
  sleepTarget_getRscTimestamp(RSC_PARENT, times->rsc_parent);

  CORE_VERIFY(STATS_PDC_PROFILE_TOTAL == pdcProfile_getUnitData(times->pdc, 
                                                                STATS_PDC_PROFILE_TOTAL, 
                                                                (PDC_PROFILE_POWER_DOWN_START | 
                                                                 PDC_PROFILE_POWER_UP_END     |
                                                                 PDC_PROFILE_POWER_DOWN_END   |
                                                                 PDC_PROFILE_POWER_UP_START)));
  return;
}


/*
 * sleepStats_logWakeup
 */
void sleepStats_logWakeup(void)
{
  int64               wakeupDelta;
  int64               wakeupAdjust;
  uint64              expectedWakeup;
  uint64              sleepResume;
  uint64              stmExitTime;
  uint64              parentEnd, childStart, pdcStart;
  sleep_solver_output *solverOutput = sleepActive_GetSolverOutput();
  sleep_solver_input  *solverInput  = sleepActive_GetSolverInput();

  static sleepStats_os_overhead osOverhead    = {0};
  static uint32                 WBCount       = 0;
  static uint64                 actualWakeup  = 0;

  /* Get all the profiling timestamp units */
  sleepStats_readWakeupTimes(&g_wakeupTimestamps);

  /* Log useful data from the wakeup */
  sleepStats_logWakeupTimes();
  
  /* Log kernel time if we performed warmboot */
  osOverhead.count = qurt_power_shutdown_get_hw_ticks(&osOverhead.sleep_time, &osOverhead.awake_time);

  if(WBCount != osOverhead.count)
  {
    WBCount     = osOverhead.count;
    sleepResume = sleepStats_getLprTimeData(SLEEP_STATS_TIME_MSTR_RETURN_TYPE);

    /* Log OS bringup times */
    sleepLog_printf(SLEEP_LOG_LEVEL_PROFILING, 2 + (3*2),
                    "Kernel stats (Count: %u) "
                    "(PC timestamp: 0x%llx) (Resume timestamp: 0x%llx) "
                    "(Sleep resume: 0x%llx) "
                    "(WB time: %uuS)" ,
                    WBCount,
                    ULOG64_DATA(osOverhead.sleep_time),
                    ULOG64_DATA(osOverhead.awake_time),
                    ULOG64_DATA(sleepResume),
                    TICKS_TO_US(sleepResume - 
                                g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp));
  }

  /* Get expected & actual wakeup times */
  expectedWakeup  = sleepStats_getLprTimeData(SLEEP_STATS_TIME_REQ_LOCAL_WAKE_TYPE);

  parentEnd   = g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_END].timeStamp;
  childStart  = g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_START].timeStamp;
  pdcStart    = g_wakeupTimestamps.pdc[STATS_PDC_PROFILE_UP_START].timeStamp;

  /* Verify HWsolver didn't select clockgate, or parent was bypassed in which 
   * case we want to use the child start */ 
  if(g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_END].valid && 
     g_wakeupTimestamps.rsc_parent[STATS_RSC_PROFILE_UP_START].valid)
  {
    /* In RPMh assisted modes, RSCc will handshake with the RSCp, and will not proceed until
     * RSCp has completed it's sequence.  Since RSCp latency is given via RPMh driver, mark
     * the actual wakeup time as the end of the RSCp up sequence */
    actualWakeup = parentEnd;

    /* Subsystem was already up (should only be in sensors case, where SDC woke it up before Q6)
     * In this case, use the child start as the starting time */
    if(childStart > parentEnd)
    {
      actualWakeup = childStart;
    }

    /* !! PDC timer wakeup HW bug workaround (QCTDD04481022)
     * !! Try to detect error in match value by looking for various timings */
    if(((pdcStart & 0x007fffff) < 0x20) &&                              //Lower bits mis-match
       (pdcStart < sleepStats_getLprTimeData(SLEEP_STATS_RPMH_WAKEUP))  //wakeup eariler than expected
      )
    {
      uint32 idx = g_wakeupWorkaroundData.count % 4;

      sleepLog_printf(SLEEP_LOG_LEVEL_WAKEUPS, 1,
                      "WARNING: Detected very early PDC wakeup bug "
                      "(Count: %u)", g_wakeupWorkaroundData.count);

      memcpy(&(g_wakeupWorkaroundData.timestamps[idx]), &g_wakeupTimestamps, sizeof(g_wakeupTimestamps));
      g_wakeupWorkaroundData.qtimer_wakeup_req[idx] = sleepStats_getLprTimeData(SLEEP_STATS_QTIMER_WAKEUP);
      g_wakeupWorkaroundData.rpmh_wakeup_req[idx]   = sleepStats_getLprTimeData(SLEEP_STATS_RPMH_WAKEUP);
      g_wakeupWorkaroundData.count++;

      if(childStart > parentEnd)
      {
        /* With the AOP interrupt assertion, we should never hit this.
         * The child start should always be less than the parent end */
        sleepLog_printf(SLEEP_LOG_LEVEL_WAKEUPS, 0,
                        "WARNING: !! Using child start as wakeup, workaround failure !!");
      }
    }
	
	 /* Workaround for RSC timestamps corruption issue: 
     *  Try to check if timestamps value has been corrupted and return from here to avoid further calculations  
	 *  preventing the device from crashing*/

    if ((childStart == 0x7ffffffff) || (childStart == 0x7ffffe000))
    {
	  /*Increment the timestamp corruption count*/	
	  g_sleepTimestampCorruptionCount++;
	  
	  /*Print the warning that timestamp corruption has happened along with wakeup time and corruption count*/
	  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1 + (1 * 2),
                      "WARNING: Detected RSC time stamp corruption"
                      "(Total counts till this point: %u)"
                      "Master wakeup stats (RSC resume: 0x%llx)",
                      g_sleepTimestampCorruptionCount, 
                      ULOG64_DATA(actualWakeup));	   
      return;
    }
  }
  else
  {
    /* In non-RPMh assisted modes (standalone), or cases as noted above, use the RSCc up
     * sequence start as wakeup time */
    actualWakeup = childStart;
  }

  /* Get inaccuracy in the master wakeup so only the local latency is adjusted later */
  if(actualWakeup >= expectedWakeup)
  {
    /* RPMh or idle timer woke up the master late */
    wakeupAdjust = (int64)(actualWakeup - expectedWakeup); 
  }
  else
  {
    /* RPMh or idle timer woke up the master early */
    wakeupAdjust = -((int64)(expectedWakeup - actualWakeup));
  }

  /* Log master wakeup */
  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_INFO, SLEEP_WAKEUP_NUM_ARGS,
                      SLEEP_WAKEUP_STR, SLEEP_WAKEUP, 
                      ULOG64_DATA(actualWakeup),
                      ULOG64_DATA(expectedWakeup),
                      ULOG64_DATA(wakeupAdjust)); 

  /* Run any profilng functions */
  synthLPRM_profile(solverOutput->selected_synthLPRM, &g_wakeupTimestamps, solverInput->fLUT);

#if (SLEEP_PROFILING_ID == SLEEP_PMI_ID)
  /* If using the PMI interrupt for profiling, we can always get the STM exit time
   * as put by software */
  stmExitTime = sleepStats_getLprTimeData(SLEEP_STATS_TIME_STM_EXIT_TYPE);
#else

  /* HW Workaround: Bus de-iso sequence overwriting timestamp work around requires use of
   * PMI interrupt as profiling interrupt so no need to change anything here. */

  /* If using dedicated profiling interrupt, we can use the software exit time in uncacheable
   * mode cases */
  if(0 != solverOutput->selected_synthLPRM->uncacheable.num_component_modes)
  {
    stmExitTime = sleepStats_getLprTimeData(SLEEP_STATS_TIME_STM_EXIT_TYPE);
  }
  else
  {
    /* For cacheable modes, assume STM exit is the same as RSCc sequence end time */
    stmExitTime = g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp;
  }
#endif

  /* Log late sleep exits */
  if(stmExitTime > solverOutput->ref_deadline)
  {
    /* Get difference of sleep exit time to expected exit */
    wakeupDelta = stmExitTime - solverOutput->ref_deadline;
    
    sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 4*2,
                    "WARNING (message: \"Late sleep exit\") "
                    "(Actual: 0x%llx) (Expected: 0x%llx) "
                    "(diff ticks: %lld) (diff us: %lld)",
                    ULOG64_DATA(stmExitTime), 
                    ULOG64_DATA(solverOutput->ref_deadline),
                    ULOG64_DATA(wakeupDelta),
                    ULOG64_DATA(TICKS_TO_US(wakeupDelta)));
  }
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  else
  {
    /* Early wake ups are negative values */
    wakeupDelta = -(solverOutput->ref_deadline - stmExitTime);
  }

  /* Record stats and make any adjustments */
  sleepStats_adjustSynthmode(wakeupDelta, wakeupAdjust);
#endif /* SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT */

  return;
}

/*
 * sleepStats_getLastLprLatency
 */
uint32 sleepStats_getLastLprLatency(sleepStats_latency_type type)
{
  uint32 tempVal;

  CORE_VERIFY(type < SLEEP_STATS_NUM_LATENCY_TYPE);

  tempVal = g_sleepStatsPostedData.latency[type];

  /* Verify an LPR put a valid value */
  CORE_VERIFY(tempVal != 0);
  g_sleepStatsPostedData.latency[type] = 0;

  return(tempVal);
}

/*
 * sleepStats_getLprTimeData
 */
uint64 sleepStats_getLprTimeData(sleepStats_timedata_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_TIMEDATA_TYPE);
  return(g_sleepStatsPostedData.time[type]);
}

/*
 * sleepStats_getMiscData
 */
uint32 sleepStats_getMiscData(sleepStats_misc_type type)
{
  CORE_VERIFY(type < SLEEP_STATS_NUM_MISC_TYPE);
  return(g_sleepStatsPostedData.misc_32bit[type]);
}

/*
 * sleepStats_getTableEntry
 */
boolean sleepStats_getTableEntry(uint32           freq,
                                 sleep_fLUT_node  **fLUT)
{
  static sleep_fLUT_node *cachedEntry = NULL;
  
  sleep_fLUT_node *freqTable;
  sleep_fLUT_node *workingCachePtr;
  sleep_fLUT_node *closest;
  boolean         rtnCode   = TRUE;
  
  freqTable       = g_sleepFLUT;
  workingCachePtr = cachedEntry;
  closest         = freqTable;

  /* Set initial returned LUT to cached LUT */
  *fLUT = workingCachePtr;

  /* Only scan tables if given frequency is different from cached value */
  if(workingCachePtr == 0 || (freq != workingCachePtr->frequency))
  {
    while(freqTable != NULL)
    {
      /* Check if we already have a frequency entry in the table */
      if(freq == freqTable->frequency)
      {
        /* update cache pointer */
        *fLUT       = freqTable;
        cachedEntry = freqTable;
        return (TRUE);
      }

      /* Remember closest (but less than) entry */
      if(freqTable->frequency < freq)
      {
        closest = freqTable;
      }

      /* Check next entry */
      freqTable = freqTable->next;
    };

    /* At this point, a new entry needs to be added
     * Set the frequency LUT to the closest entry and return that new elements
     * need to be added.
     */
    *fLUT   = closest;
    rtnCode = FALSE;
  }

  return(rtnCode);
}

/**
 * sleepStats_initializeBackoffValues
 *
 * @brief Initializes a new synth backoff value using the given seed values or 
 *        the value of the closest frequency 
 *
 * @param newTable:    Pointer to newly created fLUT 
 * @param closestFlut: Pointer to closes fLUT, can be NULL if none exist
 */
static void sleepStats_initializeBackoffValues(sleep_fLUT_node *newTable,
                                               sleep_fLUT_node *closestFlut)
{
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  uint32              nMode;
  synth_LPRM_dyn_data *newDynSynth;
  synthRegistry       *synthRegistry = synthRegistry_getRegistry();

  CORE_VERIFY_PTR(synthRegistry);

  newDynSynth = newTable->dynamic_data.synth.LPRM;

  for(nMode = 0; nMode < SLEEP_LPR_NUM_SYNTHESIZED_MODES; nMode++)
  {
    /* Use the closest frequency
     *  - If the closest fLUT is NULL, this is the first entry, so use
     *    initial (only) seed value generated from SleepSynth
     *  - If the closest fLUT is value, use it's data 
     */
    if(NULL == closestFlut)
    {
      newDynSynth[nMode].backoff = 
        synthRegistry->LPR->modes[nMode].seed_latency->backoff[0];
    }
    else
    {
      newDynSynth[nMode].backoff = 
        closestFlut->dynamic_data.synth.LPRM[nMode].backoff;
    }

  }
#endif /*SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT*/

  return;
}

/*
 * sleepStats_addFrequencyTable
 */
sleep_fLUT_node* sleepStats_addFrequencyTable(sleep_fLUT_node *closestFlut,
                                              uint32          curFreq)
{
  sleep_fLUT_node *newTable = NULL;

#if defined(SLEEP_NUM_STATIC_FLUTS) && (SLEEP_NUM_STATIC_FLUTS > 0)
  static uint32   lastStaticIdx = 0;
#endif

  /* Find or create a free element to add the new frequency to */
#if defined(SLEEP_NUM_STATIC_FLUTS) && (SLEEP_NUM_STATIC_FLUTS > 0)
  if(lastStaticIdx < SLEEP_NUM_STATIC_FLUTS)
  {
    /* Still room in the static array, point to the next free element */
    newTable = &g_staticFlutArray[lastStaticIdx];
    lastStaticIdx++;
  }
  else
#endif
  {
    /* Allocate new fLUT element */
    CORE_VERIFY_PTR(newTable = calloc(1, sizeof(sleep_fLUT_node)));
  }

  CORE_VERIFY(curFreq > 0);
  newTable->frequency = curFreq;

#ifdef SLEEP_ENABLE_FREQUENCY_SCALING
  /* Find closest mLUT
   * If scaling feature is disabled, there is only on 1 mLUT so no need to search
   * Note that SLEEP_NUM_STATIC_FREQ will always be at least 1 */
  for(newTable->mLUT_idx = 1; 
       newTable->mLUT_idx < SLEEP_NUM_STATIC_FREQ; 
       newTable->mLUT_idx++)
  {
    if(curFreq < SleepLPR_static_freq_list[newTable->mLUT_idx])
    {
      break;
    }
  }

  /* Subtract one due to logic of finding closest, but less than frequency */
  newTable->mLUT_idx--;
#endif

  /* Initialize data & cache arrays for new frequency entry before adding to 
   * master list. */
  sleepStats_initDynamicData(&newTable->dynamic_data);
  sleepStats_initializeBackoffValues(newTable, closestFlut);

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 2,
                  "Inserting new fLUT "
                  "(freq: %d) (mode_LUT_idx: %d)",
                  curFreq, newTable->mLUT_idx);

  /* Lock the master list while the new element is added */
  sleepStats_lockfLUTAccess();

  /* Verify that a closest frequency found, if not, initialize the head
   * fLUT pointer */
  if(closestFlut != NULL)
  {
    /* The closest frequency will be less than the current in all but one case
     * where it is the only (head) element.  In this case, the new element will
     * become the head. 
     */
    if(closestFlut->frequency > curFreq)
    {
      newTable->next  = closestFlut;
      g_sleepFLUT     = newTable;
    }
    else
    {
      /* Insert new element in frequency order which is after closest */
      newTable->next    = closestFlut->next;
      closestFlut->next = newTable;
    }
  }
  else
  {
    /* Set very first fLUT element */
    g_sleepFLUT = newTable;
  }

  /* New element was added, unlock access */
  sleepStats_unlockfLUTAccess();

  return newTable;
}

/*
 * sleepStats_lockfLUTAccess
 */
void sleepStats_lockfLUTAccess(void)
{
  Core_MutexLock(g_sleepfLUTMutex);
  return;
}

/*
 * sleepStats_unlockfLUTAccess
 */
void sleepStats_unlockfLUTAccess(void)
{
  Core_MutexUnlock(g_sleepfLUTMutex);
  return;
}

/*
 * sleepStats_updatePCTime
 */
void sleepStats_updatePCTime(uint64 time)
{
  if(TRUE == g_sleepTotalPowerCollapseTime.tracking_enabled)
  {
    /* Update stats */
    g_sleepTotalPowerCollapseTime.stats.last_mode_time  = time;
    g_sleepTotalPowerCollapseTime.stats.in_mode_time    += time;
    g_sleepTotalPowerCollapseTime.stats.mode_run_cycles++;
  }

  return;
}

/*
 * sleepStats_resetGeneric32
 */
void sleepStats_resetGeneric32(sleepStats_generic32 *stats)
{
  stats->min    = UINT32_MAX;
  stats->max    = 0;
  stats->total  = 0;
  stats->count  = 0;
  return;
}

