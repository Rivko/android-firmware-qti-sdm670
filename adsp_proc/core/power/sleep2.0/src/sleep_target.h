#ifndef _SLEEP_TARGET_H_
#define _SLEEP_TARGET_H_
/*==============================================================================
  FILE:         sleep_target.h
  
  OVERVIEW:     This file provides the externs and declarations needed for
                target specific code

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/sleep_target.h#3 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"
#include "sleepi.h"
#include "rsc.h"
#include "DDIChipInfo.h"
#include "sleep_solver.h"
#include "pdc_seq.h"

/*==============================================================================
                             TYPE DEFINITIONS
 =============================================================================*/
/* Used to enumerate l2 lprm configuration */
typedef enum{
  L2_TCM_RET,       /* Config processor SS & L2 for L2 ret & TCM ret          */
  L2_TCM_NORET,     /* Config processor SS & L2 for L2 non-ret & TCM non-ret  */
  L2_NORET,         /* Config processor SS & L2 for L2 non-ret & TCM ret      */
  L2_NUM_RET_MODES  /* Total number of retention modes                        */
} l2_lprm;

/* Target config data returned by sleepTarget_initialize() */
typedef struct 
{
  DalChipInfoFamilyType   chip_family;    /* Chip family architecture*/
  DalChipInfoVersionType  chip_ver;       /* Chip architecture version */
  uint32                  l2_mask;        /* Mask for L2 Cache Power Ctl */
  uint32                  tcm_mask;       /* Mask for TCM Power Ctl */
} sleep_target_config;

/*==============================================================================
                             EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepTarget_getHardWakeupTime
 * 
 * @brief Returns the amount of time until the next timer expiration. This is used
 *        to further decide on which low-power mode(s) can be entered.
 *
 * @param deadlines: Data strcuture pointer that will be populated with the first
 *                   absolute time (in ticks) of the various hard deadlines
 */ 
void sleepTarget_getHardWakeupTime(sleep_solver_deadlines *deadlines);

/**
 * sleepTarget_initialize
 * 
 * @brief Perform target-specific initialization of the sleep subsystem.
 */
void sleepTarget_initialize(void);

/**
 * sleepTarget_initializeLPR 
 *  
 * @brief Do target-specific initialization for the /sleep/lpr node.
 *        This initialization will happen before the node is available
 *        to the outside world.
 */
void sleepTarget_initializeLPR(void);

/**
 * sleepTarget_initializeLPR 
 *  
 * @brief Set subsystem wakeup match value
 * 
 * @param wakeupTick: Absolute time in 19.2MHz ticks when wakeup should be programmed.
 */
void sleepTarget_enableRpmhTimer(uint64 wakeupTick);

/**
 * sleepTarget_enableSleepTimer
 *  
 * @brief Set the local wakeup timer match value
 * 
 * @param wakeupTick: Absolute time in 19.2MHz ticks when wakeup should be programmed. 
 *  
 * @note Assumes function is called from a cacheable LPR 
 *  
 * @return Actual local wakeup time programed
 */
uint64 sleepTarget_enableSleepTimer(uint64 wakeupTick);

/**
 * sleepTarget_disableSleepTimer
 *  
 * @brief Disables any Timer Hardware that was programmed to wake-up
 *        the processor.
 */
void sleepTarget_disableSleepTimer(void);

/** 
 * sleepTarget_enableWatchdog 
 *  
 * @brief sleep function to enable DOG AUTOKICK
 */
void sleepTarget_enableWatchdog(void);

/** 
 * sleepTarget_disableWatchdog 
 *  
 * @brief sleep function to disable DOG AUTOKICK
 */
void sleepTarget_disableWatchdog(void);


/** 
 * sleepTarget_getCPUArchitecture 
 *  
 * @brief Gets the current CPU family & version
 *  
 * @param chipFamily: Return value with family of chip
 * @param chipVer: Return value with version of chip
 */
void sleepTarget_getCPUArchitecture(DalChipInfoFamilyType  *chip_family, 
                                    DalChipInfoVersionType *chip_ver);

/**
 * sleepTarget_setRPMhAssistedMode
 *
 * @brief Sets an indicator if the RPM LPR enter/exit functions have been 
 *        executed as part of the current synth mode 
 *  
 *        This function should only be called from the RPM LPR functions. 
 *  
 * @param value: TRUE or FALSE to indicate if in RPM assisted mode 
 */
void sleepTarget_setRPMhAssistedMode(boolean value);

/**
 * sleepTarget_getRPMhAssistedMode
 *
 * @brief Retreives the value previously set by the "set" function that 
 *        indicates if an RPMh synth mode was selected for entry. 
 *
 *        This function is mainly useful to the CPU VDD LPR to determine if we are 
 *        in an RPM assised synth mode 
 *  
 * @return: TRUE or FALSE to indicate if we have entered RPMh assisted mode
 */
boolean sleepTarget_getRPMhAssistedMode(void);

/**
 * sleepTarget_performInitialLPRExit
 *
 * @brief Performs any target specfic exit requirements prior to executing 
 *        the LPRM exit routines.  It is expected to call this function as
 *        early as possible in the sleep exit from power collapse routines. 
 */
void sleepTarget_performInitialLPRExit(void);

/**
 * sleepTarget_setRpmHandshake
 *
 * @brief Controls the RSC-RPM handshakeing 
 *  
 * @param performHandshake: Boolean value to enable or disable the RPM handshake 
 *                          at power collapse 
 *  
 * @return RSC results
 */
rsc_result_t sleepTarget_setRpmHandshake(boolean performHandshake);

/**
 * sleepTarget_setPdcLowPowerMode
 *
 * @brief Controls the power mode that PDC will enter
 *  
 * @param mode: Power mode to setup
 *  
 * @return PDC results
 */
pdc_seq_result_t sleepTarget_setPdcLowPowerMode(pdc_low_power_mode_type mode);

/**
 * sleepTarget_setRscLowPowerMode
 *
 * @brief Controls the power mode that RSC will enter
 *  
 * @param type:     RSC type to configure (child or parent) 
 * @param coreMode: Power mode to setup
 *  
 * @return RSC results
 */
rsc_result_t sleepTarget_setRscLowPowerMode(rsc_type type, rsc_low_power_mode_type coreMode);

/**
 * sleepTarget_getRscTimestamp
 *
 * @brief Gets the timestamp data of the last RSC power mode execution
 *  
 * @param type:       RSC type to configure (child or parent) 
 * @param timeStamp:  Timestamp structure poitner to be populated with data 
 */
void sleepTarget_getRscTimestamp(rsc_type               type,
                                 rsc_profile_unit       *timeStamp);

/**
 * sleepTarget_isInterruptPending
 *
 * @brief Checks if a particular interrupt is pending 
 *  
 * @param intNumber: Interrupt vector to check 
 *  
 * @return: TRUE if interrupt is pending, else FALSE
 */
boolean sleepTarget_isInterruptPending(uint32 intNumber);

/**
 * sleepTarget_setDefaultPowerMode
 *
 * @brief Sets a default power mode to enter should the sub-system enter the idle state. 
 *        This mode should be the most basic and quickest to enter.  Typically it will be
 *        clock gating.
 *  
 * @note The configured mode MUST be fully cacheable 
 *  
 *       When un-setting the default mode, the function will take into account any required setting 
 *       changes for the currently selected actual power mode to enter.  The intention of this function
 *       is to only be called from the ATS logic.
 *  
 * @param enable: Boolean to enable or disable the default mode 
 */
void sleepTarget_setDefaultPowerMode(boolean enable);

#endif /*_SLEEP_TARGET_H_*/

