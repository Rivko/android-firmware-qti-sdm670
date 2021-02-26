#ifndef _SLEEP_STATS_H_
#define _SLEEP_STATS_H_
/*=============================================================================
  FILE:            sleep_stats.h
  
  OVERVIEW:        Provides the functions & types to gather and adjust sleep
                   statistics
 
                   Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary.
                   Export of this technology or software is regulated by the U.S. Government.
                   Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/api/power/sleep2.0/sleep_stats.h#3 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"
#include "sleep_stats_types.h"

/*==============================================================================
                           GLOBAL TYPE DEFINITIONS
 =============================================================================*/
/* LPR handle for power stat API's */
typedef uint32 sleepStats_lpr_handle;

/**
 * sleepStats_mode_stats
 *
 * @brief Basic statistic data for the synth mode
 */
typedef struct sleepStats_synthmode_stats_s
{
  /* Timestamps of the last enter/exit times.  Timestamps are at the point
   * if power collapse (if cached) or idle timeline (if uncached) */
  uint64 last_enter_timestamp;
  uint64 last_exit_timestamp;

  /* Running total of ticks we were in this low power mode */
  uint64 in_mode_time;

  /* Total number of times this mode is entered @ any frequency */
  uint32 cycles;
}sleepStats_synthmode_stats;

/**
 * sleepStats_lprm_stats
 *
 * @brief Basic statistic data for component LPRM's 
 *  
 * @note At this time cacheable and uncacheable times are not tracked seperately 
 */
typedef struct sleepStats_lprm_stats_s
{
  sleepStats_generic32  enter_lat;            /* Stats for enter latency */
  sleepStats_generic32  exit_lat;             /* Stats for exit latency */
  uint64                last_enter_start;   /* Start of last entry function call */
  uint64                last_enter_end;     /* End of last entry function call */
  uint64                last_exit_start;    /* Start of last exit function call */
  uint64                last_exit_end;      /* End of last exit function call */
}sleepStats_lprm_stats;

/**
 * sleepStats_mode_stats
 *
 * @brief Basic statistic data for overall power tracking
 */
typedef struct sleepStats_mode_stats_s
{
  /* Number of ticks we were in this low power mode the last 
   * time it was entered */
  uint64 last_mode_time;

  /* Running total of ticks we were in this low power mode */
  uint64 in_mode_time;

  /* Total number of times this mode is entered @ any frequency */
  uint32 mode_run_cycles;
}sleepStats_mode_stats;

/**
 * sleep_latency_type
 *
 * @brief Enum for LPR latency values
 *  
 * Values will be used by latency put function to record LPR additional latency 
 * times  
 */ 
typedef enum sleepStats_latency_type
{
  SLEEP_STATS_ENTER_LATENCY_TYPE = 0,
  SLEEP_STATS_EXIT_LATENCY_TYPE,
  SLEEP_STATS_BACKOFF_LATENCY_TYPE,

  SLEEP_STATS_NUM_LATENCY_TYPE
}sleepStats_latency_type;

/**
 * sleep_timestamp_type
 *
 * @brief Enum for LPR timestamp values
 *  
 * Values will be used by sleepStat_getLastLprLatency to record certain LPR 
 * times 
 */ 
typedef enum sleepStats_timedata_type
{
  SLEEP_STATS_TIME_STM_EXIT_TYPE        = 0,  /* Time of STM exit */
  SLEEP_STATS_TIME_REQ_LOCAL_WAKE_TYPE,       /* Expected local wakeup time */
  SLEEP_STATS_TIME_SLEEP_IDLE_START_TYPE,     /* Sleep idle timeline start */
  SLEEP_STATS_TIME_MSTR_RETURN_TYPE,          /* First point in sleep code after PC */

  /* !! PDC timer wakeup HW bug workaround (QCTDD04481022) */
  SLEEP_STATS_RPMH_WAKEUP,
  SLEEP_STATS_QTIMER_WAKEUP,

  SLEEP_STATS_NUM_TIMEDATA_TYPE
}sleepStats_timedata_type;

/**
 * sleepStats_misc_type
 *
 * @brief Enum for miscellaneous statistics values
 *  
 * Values will be used by sleepStat_getLastMiscData to record certain LPR events
 */ 
typedef enum sleepStats_misc_type
{
  SLEEP_STATS_MISC_WAKEUP_REASON_TYPE = 0, /* Wake reason(RPM/SA, un/scheduled*/

  SLEEP_STATS_NUM_MISC_TYPE
}sleepStats_misc_type;

/*==============================================================================
                              FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * sleepStats_putLprTimeData
 * 
 * @brief Saves special LPRM time stamp values that will be retreived later.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR.
 * 
 * @param value: Time value in ticks 
 * @param type: ENUM time data type  
 */
void sleepStats_putLprTimeData(uint64                   value,
                               sleepStats_timedata_type type);

/** 
 * sleepStats_putLprLatency 
 *  
 * @brief Saves the local latency value of an LPR that will be 
 *        retreived & used later in latency calculations.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR and should be used if the SLEEP_MODE_ATTR_NO_AUTO_LATENCY_TRACK
 * bit is set for the LPRM.
 *  
 * Each type should be called only once (data saved) per sleep cycle.
 * 
 * @param value: Local latency value in ticks 
 * @param latType: ENUM latency type
 */
void sleepStats_putLprLatency(uint32                  value,
                              sleepStats_latency_type type);

/** 
 * sleepStats_putMiscData
 *  
 * @brief Saves special values that will be retreived later.
 *
 * This function is intended to be called only in the enter & exit functions
 * of the LPR.
 * 
 * @param value: Data
 * @param type: ENUM time data type  
 */
void sleepStats_putMiscData(uint32                value,
                            sleepStats_misc_type  type);

/** 
 * sleepStats_get_lpr_stats 
 *  
 * @brief Gets the LPRM cycle statistic data 
 *
 * @param lprHandle: LPR handle obtained from sleepStats_get_lpr_handle()
 * @param modeStats: If successful, requested data
 */
void sleepStats_getLPRStats(sleepStats_lpr_handle LPRHandle,
                            sleepStats_mode_stats *modeStats);

/** 
 * sleepStats_get_lpr_stats
 *  
 * @deprecated use sleepStats_getLPRStats
 */
#define sleepStats_get_lpr_stats sleepStats_getLPRStats

/** 
 * sleepStats_getLPRHandle
 *  
 * @brief Gets a LPR handle for use with sleep statistic functions
 *
 * @param lprName: LPR name 
 *                 This string can be obtained from looking in the LPR XML
 *                 configuration files or in the generated
 *                 "SleepLPR_LookupTable" data structure.  
 * @return Handle value, or 0 if error
 */
sleepStats_lpr_handle sleepStats_getLPRHandle(const char *LPRName);

/** 
 * sleepStats_get_lpr_handle
 *  
 * @deprecated use sleepStats_getLPRHandle
 */
#define sleepStats_get_lpr_handle sleepStats_getLPRHandle

/**
 * sleepStats_trackPowerCollapseTime
 *
 * @brief Starts (or stops) the tracking of the time while in any uncached
 *        type of power collapse. 
 *  
 * @param enable: TRUE or FALSE to enable or disable tracking. 
 *                Any call to enable will reset the statistics
 */
void sleepStats_trackPowerCollapseTime(uint8 enable);

/**
 * sleepStats_getPowerCollapseTime
 *
 * @brief Retrieves the current power collapse time statistic data structure. 
 *        A call to enable the statistic gathering must be performed first for
 *        any meaningful data to be returned. 
 *  
 * @note Only uncached, non-island power modes are reported in the statistics and both
 *       the last cycle and total times will include overhead.
 *  
 * @return Pointer to the current power collapse statistical data. 
 *         (See definition of sleepStats_mode_stats for specific data)
 */
const sleepStats_mode_stats *sleepStats_getPowerCollapseTime(void);

#endif /*_SLEEP_STATS_H_*/

