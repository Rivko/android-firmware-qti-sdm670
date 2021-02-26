#ifndef _USLEEP_OS_H_
#define _USLEEP_OS_H_
/*==============================================================================
  FILE:         uSleep_os.h

  OVERVIEW:     Provides main OS API intface functions and types for uSleep

  DEPENDENCIES: None
 
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/inc/uSleep_os.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"

/*==============================================================================
                            DEFINITIONS & TYPES
 =============================================================================*/
/* Enumeration to indicate the Idle Mode Type */
typedef enum
{
  USLEEP_OS_IDLE_MODE_DEFAULT   = 0, /* Configure for single threaded sleep */
  USLEEP_OS_IDLE_MODE_OVERRIDE,      /* Configure for low overhead sleep */

  USLEEP_OS_IDLE_MODE_NUM_STATES
} uSleepOS_idleModeType;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * uSleepOS_configIdleMode
 * 
 * @brief Configures the behavior of the sleep thread for cacheable or uncacheable modes 
 *  
 * @param newIdleMode:  Configured power mode 
 */
void uSleepOS_configIdleMode(uSleepOS_idleModeType  newIdleMode);

/** 
 * uSleepOS_PMIPerformer
 * 
 * @brief Island mode version of the PMI interrupt handler routine.  
 */
void uSleepOS_PMIPerformer(void);

/** 
 * uSleepOS_enterLPMAbort
 * 
 * @brief Aborts the LRPM entry.  Could be used in the case where wakeup time 
 *        happens to be in the past or any other reason. 
 */
void uSleepOS_enterLPMAbort(void);

/** 
 * uSleepOS_performLPM
 * 
 * @brief Main uSleep function in island mode that will enter low power modes.
 */
uint32 uSleepOS_performLPM(void);

/** 
 * uSleepOS_performLPMExit
 * 
 * @brief Handles exiting LPR modes 
 */
void uSleepOS_performLPMExit(void);

/** 
 * uSleep_initialize
 * 
 * @brief Initialize the uSleep subsystem
 */
void uSleepOS_initialize(void);

/**
 * uSleepOS_perform
 * 
 * @brief Entry function for performing low power modes in uSleep.
 *
 * @return Potential error code (unused as of now)
 */
uint32 uSleepOS_perform(void);

#endif /* _USLEEP_OS_H_ */

