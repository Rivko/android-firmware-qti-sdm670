#ifndef _SLEEP_STATSI_H_
#define _SLEEP_STATSI_H_
/*==============================================================================
  FILE:         sleep_statsi.h
  
  OVERVIEW:     Internal functions & types to gather and adjust
                sleep statistics
 
  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/statistics/sleep_statsi.h#2 $
$DateTime: 2017/09/13 08:52:52 $
==============================================================================*/
#include "comdef.h"
#include "synthTypes.h"
#include "sleep.h"
#include "sleep_stats.h"
#include "sleep_lpr.h"
#include "lookup_table_types.h"
#include "sleep_stats_dynamic.h"
#include "rsc.h"
#include "pdcProfile.h"

/*==============================================================================
                           INTERNAL TYPE DEFINITIONS
 =============================================================================*/
/** 
 * SLEEPSTATS_INITIAL_CYCLE_COUNT
 * 
 * @brief Initial number of sleep cycle statistics to gather before making
 *        any latency adjustments
 *        Note that this value must be less than the final cycle count
 */
#define SLEEPSTATS_INITIAL_CYCLE_COUNT  0x00000020

/** 
 * SLEEPSTATS_FINAL_CYCLE_COUNT
 * 
 * @brief Maximum number of statistics to gather. Adjustments will be made 
 *        after every x number of cycles once this value is reached.
 *        Note that this value must be greater than the initial cycle count   
 */
#define SLEEPSTATS_FINAL_CYCLE_COUNT    0x00001000

/** 
 * SLEEPSTATS_VALID_BACKOFF_MIN_FACTOR & SLEEPSTATS_VALID_BACKOFF_MAX_FACTOR
 * 
 * @brief Scale factors used to compute the minimum and maximum backoff values 
 *        allowed when a Synthmode is adjusted
 */ 
#define SLEEPSTATS_VALID_BACKOFF_MIN_FACTOR  8 /* Value as a division 
                                                * factor of the current value */
#define SLEEPSTATS_VALID_BACKOFF_MAX_FACTOR  6 /* Value as a multiplication 
                                                * factor of the current value */
/**
 * SLEEP_BACKOFF_ADJUST_CRASH_OUT_OF_RANGE 
 * 
 * If defined as non-zero, the master will force a crash if the new backoff is outside of 
 * the allowable backoff range based on the factors above.  If not defined,
 * a warning message will be output instead.
 */ 
#define SLEEP_BACKOFF_ADJUST_CRASH_OUT_OF_RANGE 0

/** 
 * sleepStats_os_overhead
 * 
 * @brief This struct contains pc enter and exit overhead for previous
 *        PC operation
 */
typedef struct sleepStats_os_overhead_s
{
  uint32  count;        /* Count of power collapse cycles */
  uint64  sleep_time;   /* Time of entering last power collapse */
  uint64  awake_time;   /* Time of exiting last power collapse */
}sleepStats_os_overhead;

/**
 * sleepStats_power_collapse_time
 *
 * @brief Data strcuture for tracking of total time in PC
 */
typedef struct sleepStats_power_collapse_time_s
{
  uint8                 tracking_enabled;
  sleepStats_mode_stats stats;
}sleepStats_power_collapse_time;

/* Data for a frequency table entry */
typedef struct sleep_fLUT_node_s
{
  /* The frequency of this table entry */
  uint32 frequency;

  /* mode LUT index to use for synth mode selection */
  uint32 mLUT_idx;

  /* Statistic & latency data arrays for given frequency */
  sleepStats_dynamic_data dynamic_data;

  /* Linked list pointer */
  struct sleep_fLUT_node_s *next;
} sleep_fLUT_node;

/* Index values for RSC profiling timestamps
 * Note that the ordering here matches the mask bit order in the RSC driver */
typedef enum 
{
  STATS_RSC_PROFILE_DN_START = 0, /* Power down sequence start */
  STATS_RSC_PROFILE_DN_END,       /* Power down sequence done */
  STATS_RSC_PROFILE_UP_START,     /* Power up sequence start */
  STATS_RSC_PROFILE_UP_END,       /* Power up sequence end */

  STATS_RSC_PROFILE_TOTAL
}sleepStats_rsc_profile_units;

/* Index values for PDC profiling timestamps
 * Note that the ordering here matches the mask bit order in the PDC driver */
typedef enum 
{
  STATS_PDC_PROFILE_DN_START = 0, /* Power down sequence start */
  STATS_PDC_PROFILE_UP_END,       /* Power up sequence end */
  STATS_PDC_PROFILE_DN_END,       /* Power down sequence done */
  STATS_PDC_PROFILE_UP_START,     /* Power up sequence start */
  
  STATS_PDC_PROFILE_TOTAL
}sleepStats_pdc_profile_units;

/* Profiling timestamp units for last sleep cycle */
typedef struct sleepStats_timestamps_s
{
  rsc_profile_unit  rsc_child[STATS_RSC_PROFILE_TOTAL];
  rsc_profile_unit  rsc_parent[STATS_RSC_PROFILE_TOTAL];
  profile_unit      pdc[STATS_PDC_PROFILE_TOTAL];
}sleepStats_timestamps;

/**
 * sleepStats_postedData
 *
 * @brief Various time and data values that are stored at certain
 *        points and later retrieved by the statistic functions
 */
typedef struct sleepStats_postedData_s
{
  /* Variable for the last latency value of the last LPR */
  uint32 latency[SLEEP_STATS_NUM_LATENCY_TYPE];

  /* Variable for the timestamp values of certain LPR events */
  uint64 time[SLEEP_STATS_NUM_TIMEDATA_TYPE];

  /* Variable for miscellaneous values of certain LPR events */
  uint32 misc_32bit[SLEEP_STATS_NUM_MISC_TYPE];
}sleepStats_postedData;

/*==============================================================================
                            FUCNTION DECLARATIONS
 =============================================================================*/
/**
 * sleepStats_readWakeupTimes
 *
 * @brief Reads all the HW wakeup times for the PDC & RSCs
 * 
 * @param times: Pointer to time datastructure
 */
void sleepStats_readWakeupTimes(sleepStats_timestamps *times);

/**
 * sleepStats_init
 *
 * @brief Initializes statistic subsystem
 */
void sleepStats_init(void);

/**
 * sleepStats_logWakeup
 *
 * @brief Main sleep statistics funtion that will log wakeup data and adjust 
 *        the latency of synth modes if necessary.
 */
void sleepStats_logWakeup(void);

/** 
 * sleepStats_getLprTimeData 
 *  
 * @brief Gets special LPRM time stamp values that were previously put.
 *
 * @param type: ENUM time data type
 * 
 * @return Time value requested
 */
uint64 sleepStats_getLprTimeData(sleepStats_timedata_type type);

/** 
 * sleepStats_getMiscData 
 *  
 * @brief Gets special values that were previously put.
 *
 * @param type: ENUM time data type
 * 
 * @return Data value requested
 */
uint32 sleepStats_getMiscData(sleepStats_misc_type type);

/** 
 * sleepStats_getLastLprLatency 
 *  
 * @brief Gets special LPRM latency values that were previously put.
 *        
 * @param type: ENUM latency type 
 *  
 * @return Latency value requested
 */
uint32 sleepStats_getLastLprLatency(sleepStats_latency_type type);

/** 
 * sleepStats_initDynamicData 
 *  
 * @brief Initialize frequency dependant data arrays
 *        
 * @param data: Pointer to dynamic data array
 */
void sleepStats_initDynamicData(sleepStats_dynamic_data *data);

/**
 * sleepStats_getTableEntry
 *
 * @brief Finds the lookup table entry index that 
 *        corresponds to the current core frequency.  It will
 *        select the closest entry that is less than or equal
 *        to the core frequency.
 *  
 * @param freq: Current frequency (in Khz) of the core
 * @param fLUT: Frequency table pointer for the current core
 *              frequency or closest match
 *                  Return value:
 *                    TRUE  -> Existing fLUT match
 *                    FALSE -> Closest fLUT (or NULL if no entries) 
 *                 
 * @return TRUE   if found existing entry
 *         FALSE  if no match was found and new cache/data entries need to be
 *                added.
 */ 
boolean sleepStats_getTableEntry(uint32           freq,
                                 sleep_fLUT_node  **fLUT);

/**
 * sleepStats_addFrequencyTable
 *
 * @brief Adds a latency cache data element when a new frequency is encountered
 * 
 * @param closestFlut:  The closest frequency element
 *                       (as returned from sleepStats_getTableEntry)                 
 * @param curFreq:      Current core frequency in Khz
  *
 * @return New frequency element pointer
 */
sleep_fLUT_node* sleepStats_addFrequencyTable(sleep_fLUT_node *closestFlut,
                                              uint32          curFreq);

/**
 * sleepStats_lockfLUTAccess
 *
 * @brief Mutex locking function for fLUT data access
 */
void sleepStats_lockfLUTAccess(void);

/**
 * sleepStats_unlockfLUTAccess
 *
 * @brief Mutex unlocking function for fLUT data access
 */
void sleepStats_unlockfLUTAccess(void);

/**
 * @brief Clears and resets the statistical data
 *
 * @param synth: Data structure to reset
 */
void sleepStats_resetSynth(synth_LPRM_dyn_data *synth);

/**
 * @brief Range checks the new synthmode backoff to verify it is a valid 
 *        adjustment.  If the new backoff fails the range check, the appropriate
 *        message and action will take place in this function. 
 *
 * @param currentBackoff:   Current backoff of the synthmode 
 * @param newBackoff:       New (adjusted) backoff of the synthmode 
 * @param enterExitLatency: Synthmode combined enter and exit latency 
 *   
 */
void sleepStats_verifyBackoffRange(uint32 currentBackoff,
                                   int32  newBackoff, 
                                   uint32 enterExitLatency);

/** 
 * sleepStats_updatePCTime 
 *  
 * @brief Called after sleep code resumes from power collapse to 
 *        record the amount of time spent in power collapse 
 */
void sleepStats_updatePCTime(uint64 time);

#endif /*_SLEEP_STATSI_H_*/

