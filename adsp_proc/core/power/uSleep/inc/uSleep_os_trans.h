#ifndef _USLEEP_OS_TRANS_H_
#define _USLEEP_OS_TRANS_H_
/*==============================================================================
  FILE:         uSleep_os_trans.h

  OVERVIEW:     Provides main OS API intface functions and types for uSleep related
                to transitions

  DEPENDENCIES: None
 
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/inc/uSleep_os_trans.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/**
 * uSleep_idle_entry_ptr_type
 *
 * @brief Function pointer type for different behaviors when processor goes
 *        idle during uSleep interaction.
 */
typedef uint32 (*uSleep_idle_entry_ptr_type)(void);

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * uSleepOS_mainEntry
 *  
 * @brief This function is the main entry into uSleep mode. It will switch to 
 *        uSleep when restrictions allow it.
 *  
 * @return 0
 */
uint32 uSleepOS_mainEntry(void);

/** 
 * uSleepOS_haltOnError
 * 
 * @brief Called when the system is unable to return to DDR mode.
 */
void uSleepOS_haltOnError(void);

/**
 * uSleepOS_setIdleFunctionPtr
 *
 * @brief This function sets the idle entry point used by uSleep during next
 *        idle cycle.
 *
 * @param newIdleEntryPtr: Pointer to the function that will be called when
 *                         system is idle next time.
 */
void uSleepOS_setIdleFunctionPtr(uSleep_idle_entry_ptr_type newIdleEntryPtr);

/**
 * uSleepOS_getIdleFunctionPtr
 *
 * @brief Returns pointer to the idle entry point used by uSleep in next
 *        idle cycle.
 *
 * @return see @brief
 */
uSleep_idle_entry_ptr_type uSleepOS_getIdleFunctionPtr(void);

/** 
 * uSleepOS_performError 
 *  
 * @brief This function is the fatal exit from uImage function. There is no
 *        execution or recovery from this function. The OS should never allow
 *        this function to run. 
 */
uint32 uSleepOS_performError(void);

/** 
 * uSleepOS_cleanupIslandExit
 * 
 * @brief Performs island exit requirements from sleep context while between kernel stage 1 & 2 
 *        exit calls.  Sleep thread is at highest priority here, and interrupts are still locked. 
 */
void uSleepOS_cleanupIslandExit(void);

/** 
 * uSleepOS_finalIslandExit
 * 
 * @brief Performs the final stages of exiting island from sleep context after kernel stage 2.
 *        System is completely released from STM and sleep thread is back to normal priority here. 
 */
void uSleepOS_finalIslandExit(void);

#endif /* _USLEEP_OS_TRANS_H_ */
