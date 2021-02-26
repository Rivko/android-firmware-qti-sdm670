#ifndef _SYNTHLPRM_H_
#define _SYNTHLPRM_H_
/*==============================================================================
  FILE:         synthLPRM.h
  
  OVERVIEW:     Provides support functions for the synth LPRMs

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/synthesizer/synthLPRM.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "sleepi.h"
#include "synthTypes.h"
#include "sleep_statsi.h"

/**
 * synthLPRM_isAttributeSet
 * 
 * @brief Function tests if the given attributes are set for the 
 *        synth LPRM 
 *  
 * @param synthLPRM:      Synth LPRM to check 
 * @param attributes:     One or more synth mode attributes to check 
 * @param requireAllSet:  If TRUE, ensures ALL attributes are set, 
 *                        if FALSE, check if one or more of the attributes are set 
 *  
 * @return Returns TRUE if the attributes are set as given, FALSE otherwise
 */
boolean synthLPRM_isAttributeSet(sleep_synth_lprm *synthLPRM,
                                 uint32           attributes,
                                 boolean          requireAllSet);

/**
 * synthLPRM_getParent
 *
 * @brief Returns the parent (or the owner) of the mode.
 *
 * The parent may be synthesized LPR.
 *
 * @return Pointer to parent node.
 */ 
static SLEEP_INLINE sleep_synth_lpr *synthLPRM_getParent
(
  sleep_synth_lprm *synthLPRM
)
{
  return synthLPRM->synth_LPR_parent; 
}

/**
 * synthLPRM_getLatency
 *
 * @brief Will be used to find out the time required to enter & exit this mode.
 *
 * If the mode is the presynthesized one, its enter latency will be sum of all
 * its component modes' enter latencies.
 *
 * @param mLUTidx: Mode table index to use to for latency value of lprm 
 
 * @return Returns total time required by this mode to enter.
 */
uint32 synthLPRM_getLatency(sleep_synth_lprm  *synthLPRM,
                            uint32            mLUTidx);

/**
 * synthLPRM_getEnterLatency
 *
 * @brief Will be used to find out the entry time required for the given mode.
 *
 * @param mLUTidx: Mode table index to use to for latency value of lprm 
 *
 * @return Returns total time required by this mode to enter.
 */
uint32 synthLPRM_getEnterLatency(sleep_synth_lprm  *synthLPRM,
                                 uint32            mLUTidx);

/**
 * synthLPRM_getBackOffTime
 *
 * @brief Returns a pointer to given synth mode's backoff latency for 
 *        specified core frequency.
 *
 * @note The return value is a pointer to const value even though the value
 *       itself is not constant. It is to avoid any unintended manipulation 
 *       of that value by caller outside of backoff adjustment module.
 *
 * @param fLUT: Frequency LUT pointer for current core frequency.
 *
 * @return Pointer to (const) backoff latency of calling synth mode at 
 *         input frequency.
 */
const uint32* synthLPRM_getBackOffTime( sleep_synth_lprm *synthLPRM,
                                        sleep_fLUT_node  *fLUT );

/**
 * synthLPRM_enter
 *
 * @brief Calls the component enter functions for the given synthmode 
 *        at idle time. 
 *
 * @param synthLPRM: Selected mode pointer from solver function
 * @param wakeup_tick: Absolute wakeup time in ticks 
 * @param fLUT: Frequency table pointer for the current core frequency
 */
void synthLPRM_enter( sleep_synth_lprm  *synthLPRM, 
                      uint64            wakeupTick, 
                      sleep_fLUT_node   *fLUT);

/**
 * synthLPRM_cachedModeEnter
 *
 * @brief Calls the cachable component enter functions for the given 
 *        synthmode at active time 
 *
 * @param synthLPRM:  Selected mode pointer from solver function 
 * @param wakeupTick: Absolute wakeup time
 * @param reenter:    Only enter modes whose reentrant attribute is set
 */
void synthLPRM_cachedModeEnter(sleep_synth_lprm  *synthLPRM,
                               uint64            wakeupTick,
                               boolean           reenter);

/**
 * synthLPRM_exit
 *
 * @brief Calls the component exit functions for the given synthmode
 *        at idle time.
 *  
 * @param synthLPRM: Selected mode pointer from solver function 
 * @param fLUT: Frequency table pointer for the current core frequency
 *  
 * It should be noted that the order of calling the exit functions 
 * will be reverse of that of enter functions.
 */
void synthLPRM_exit( sleep_synth_lprm *synthLPRM,
                     sleep_fLUT_node  *fLUT );

/**
 * synthLPRM_profile
 *
 * @brief Performs all the synthmode and component profiling
 *  
 * @param 
 */
void synthLPRM_profile(sleep_synth_lprm       *synthLPRM,
                       sleepStats_timestamps  *timeStamps,
                       sleep_fLUT_node        *fLUT);

/**
 * synthLPRM_cachedModeExit
 *
 * @brief Calls the cachable component exit functions for the given 
 *        synthmode at active time
 *  
 * @param synthLPRM: Selected mode pointer from solver function  
 */
void synthLPRM_cachedModeExit( sleep_synth_lprm *synthLPRM );

/**
 * synthLPRM_update
 *
 * @brief Checks the new status of this mode (ENABLED, DISABLED or IMPOSSIBLE)
 *        and if it differs from old status, it updates the corresponding LPR
 */
void synthLPRM_update( sleep_synth_lprm *synthLPRM, 
                       sleep_lprm       *changedSleepLPRM);

/**
 * synthLPRM_initSynth
 *
 * @brief Constructor to create sleep_synth_lprm from input presynthesized mode.
 *
 * @param lprm: Pointer to presynthesized mode.
 */
void synthLPRM_initSynth(sleep_synth_lprm *synthLPRM);

/**
 * synthLPRM_setComponentModeAttributes
 *
 * @brief Sets the attribute field of each component mode based on 
 *        environment (like number of cores and cores sharing this mode).
 *
 * This function is mainly useful for already existing modes. It sets the
 * attribute in a way so that minimum overhead is incurred.
 */
void synthLPRM_setComponentModeAttributes( sleep_lprm *sleepLPRM );

/**
 * @brief Determines if the synthmode that is in the process of being configured 
 *        has any uncacheable component modes. 
 *  
 * @note  This function is only intended to be called from the LPRM enter and/or exit 
 *        routines and can be used to determine if idle time sleep will run or not 
 *  
 * @return TRUE:  if current mode has no uncacheable component modes 
 *         FALSE: if current mode has 1 or more uncacheable component modes
 */
boolean synthLPRM_isConfiguringModeFullyCacheable(void);

#endif /* _SYNTHLPRM_H_ */

