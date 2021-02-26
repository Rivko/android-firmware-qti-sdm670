#ifndef _USLEEP_TARGET_H_
#define _USLEEP_TARGET_H_
/*==============================================================================
  FILE:         uSleep_target.h

  OVERVIEW:     Types & definitions target specific requirements for uSleep

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/target/uSleep_target.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_lpri.h"
#include "rsc.h"

/*==============================================================================
                                    TYPES
 =============================================================================*/
typedef enum target_power_mode
{
  TARGET_POWER_MODE_APCR          = 0,
  TARGET_POWER_MODE_APCR_PLL_LPM,
  TARGET_POWER_MODE_CLOCKGATE
}target_power_mode;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepTarget_getCurrentModeLatency
 * 
 * @brief Gets the current modes latency values
 * 
 * @return Currently configuraiton modes latency values
 */
const uSleep_lpr_latency *uSleepTarget_getCurrentModeLatency(void);

/**
 * uSleepTarget_configBasePowerMode
 * 
 * @brief Configures the base power mode once at init (or at every island entry if transitions 
 *        are supported) 
 * 
 * @param mode: Configures for the desired default power mode
 */
void uSleepTarget_configBasePowerMode(target_power_mode mode);

/**
 * uSleepTarget_restoreBasePowerMode
 * 
 * @brief Undoes anything previously done by uSleepTarget_configBasePowerMode and should be called 
 *        on island exit to restore any settings that might have been changed in island. 
 */
void uSleepTarget_restoreBasePowerMode(void);

/**
 * uSleepTarget_configPowerMode
 * 
 * @brief Configures the power mode to enter when the system goes idle
 * 
 * @note This function assume the based setup has already been performed for APCR
 *       modes and will only update settings specific to island power modes
 * 
 * @param mode:       Configures for the desired power mode
 * @param enablePMI:  If TRUE, enables the PMI interrupt and runs the SW
 *                    LPRM exit sequence
 */
void uSleepTarget_configPowerMode(target_power_mode mode, boolean enablePMI);

/**
 * uSleepTarget_enterIdle
 * 
 * @brief Waits for the system to become active and performs the 
 *        pre-configured power mode.
 *  
 * @note  Clock gating should be configured before calling this function 
 */
void uSleepTarget_enterIdle(void);

/**
 * uSleepTarget_initialize
 *  
 * @brief Performs any target specific initializations 
 */
void uSleepTarget_initialize(void);

/**
 * uSleepTarget_programWakeupTimer
 *  
 * @brief Programs the wakeup time 
 *  
 * @param wakeupTime: Absolute value of the desired wakeup time 
 *  
 * @return Time programmed was successful 
 */
boolean uSleepTarget_programWakeupTimer(uint64 wakeupTime);

/**
 * uSleepTarget_disableWakeupTimer
 *  
 * @brief Disables the previously programed wakeup time 
 */
void uSleepTarget_disableWakeupTimer(void);

/**
 * uSleepTarget_enterLowPowerMode
 *  
 * @brief Enters a low power mode 
 *  
 * @param mode:       Low Power mode to enter 
 * @param wakeupTime: Absolute value of the desired wakeup time 
 */
void uSleepTarget_enterLowPowerMode(uSleep_LPR *mode, uint64 wakeupTime, boolean reEntrant);

/**
 * uSleepTarget_exitLowPowerMode
 *  
 * @brief Exits the given low power mode 
 */
void uSleepTarget_exitLowPowerMode(uSleep_LPR *mode);

/**
 * uSleepTarget_setInterruptHandlers
 *  
 * @brief This should be called on the uImage entry transtion, while still in normal mode, 
 *        to update the interrupt handlers 
 */
void uSleepTarget_setInterruptHandlers(void);

/**
 * uSleepTarget_restoreInterruptHandlers
 *  
 * @brief This should be called on the uImage exit transtion, when in normal mode 
 *        to restore the original the interrupt handlers 
 */
void uSleepTarget_restoreInterruptHandlers(void);

/** 
 * uSleepTarget_enableIslandDebug
 * 
 * @brief Allows T32 to remain connected while in island mode
 *  
 * @note This function must be called outside of single threaded mode.
 *        
 * @param enable: Flag to inidate if island should be setup to allow 
 *                debug. 
 */
void uSleepTarget_enableIslandDebug(boolean enable);

/**
 * uSleepTarget_setRpmHandshake
 *
 * @brief Controls the RSC-RPM handshakeing 
 *  
 * @param performHandshake: Boolean value to enable or disable the RPM handshake 
 *                          at power collapse 
 *  
 * @note Assumes the RSC driver has already been initialized 
 *  
 * @return RSC results
 */
rsc_result_t uSleepTarget_setRpmHandshake(boolean performHandshake);

#endif /* _USLEEP_TARGET_H_ */

