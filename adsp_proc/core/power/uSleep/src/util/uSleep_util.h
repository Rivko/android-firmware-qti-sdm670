#ifndef _USLEEP_UTIL_H_
#define _USLEEP_UTIL_H_
/*==============================================================================
  FILE:         uSleep_util.h

  OVERVIEW:     Types & prototypes for internal utility uSleep functions

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/util/uSleep_util.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

#ifdef USLEEP_SUPPORT_TRANSITIONS
#include "uSleepi.h"
#include "uSleep_transi.h"
#include "sleep_stats_types.h"
#endif

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/** 
 * uSleep_global_data
 * 
 * @brief Global uSleep data structure that holds various states and data
 */
typedef struct uSleep_global_data_s
{
#ifdef USLEEP_SUPPORT_TRANSITIONS
  /* Current internal uSleep state */
  uSleep_internal_state       state;

  /* DDR wakeup timer deadline */
  uint64                      ddr_wakeup_timer;

  /* Transition profiling data structure */
  uSleep_transition_profiling transition_data;
#endif

  /* Power mode specific to uSleep mode */
  uint32                      power_mode_state;

  /* Time of the last wakeup in uImage */
  uint64                      last_wakeup_timestamp;
}uSleep_global_data;

/*==============================================================================
                          DDR FUNCTION DECLARATIONS
 =============================================================================*/
#ifdef USLEEP_SUPPORT_TRANSITIONS
/**
 * uSleep_createQueryHandles
 * 
 * @brief Creates the various handles required to turn on and off DDR
 * 
 * @note This function must be called before the first entry into island mode
 *       while still in normal operational mode.
 */
void uSleep_createQueryHandles(void);

/**
 * uSleepTrans_setDDRWakeupTimeInternal
 * 
 * @brief Sets the value of the first non-deferrable timer from normal mode so uSleep 
 *        can exit to service it.
 *  
 * @note This function does not actually program a uTimer, it only sets an internal value. 
 *       Be sure to call uSleepTrans_programDDRWakeupTimeInternal to actually create a uTimer 
 *  
 * @param wakeupTimer:  Absolute time (in ticks) of the next non-deferrable 
 *                      normal operational timer. 
 */
void uSleepTrans_setDDRWakeupTimeInternal(uint64 wakeupTimer);

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/*==============================================================================
                        ISLAND FUNCTION DECLARATIONS
 =============================================================================*/
#ifdef USLEEP_SUPPORT_TRANSITIONS
/**
 * uSleep_setStateInternal
 * 
 * @brief Sets the current uSleep internal state
 * 
 * @param state: uSleep internal state to set
 */
void uSleep_setStateInternal(uSleep_internal_state state);

/**
 * uSleep_getStateInternal
 * 
 * @brief Gets the current uSleep internal state
 */
uSleep_internal_state uSleep_getStateInternal(void);

/**
 * uSleep_getDDRWakeupTimeInternal
 * 
 * @brief Gets the currently set DDR wakeup time as given to uSleep during 
 *        the entry transition.
 */
uint64 uSleep_getDDRWakeupTimeInternal(void);

/**
 * uSleep_getProfilingDataPtr
 * 
 * @brief Gets a pointer to the uSleep profiling data structure which can be 
 *        used to access data directly 
 */
uSleep_transition_profiling* uSleep_getProfilingDataPtr(void);

#endif /* USLEEP_SUPPORT_TRANSITIONS */

/**
 * uSleep_setLastSleepWakeupInternal
 * 
 * @brief Sets the time of the wakeup from the last power cycle. 
 *  
 * @param wakeupTime: Absolute time of the last wakeup 
 */
void uSleep_setLastSleepWakeupInternal(uint64 wakeupTime);

/**
 * uSleep_getLastSleepWakeupInternal
 * 
 * @brief Gets the time of the wakeup from the last power cycle. 
 *  
 * @return Absolute time of the last wakeup. 
 */
uint64 uSleep_getLastSleepWakeupInternal(void);

/**
 * uSleep_setLowPowerModeInternal
 * 
 * @brief Indicates if we can enter retention mode based on the use case
 *  
 * @note This is not an aggregation as in normal operational mode.  The last 
 *       function call will set the current mode.
 *  
 * @param request: Node state value
 */
void uSleep_setLowPowerModeInternal(uint32 request);

#endif /* _USLEEP_UTIL_H_ */

