#ifndef _SLEEP_OSI_H_
#define _SLEEP_OSI_H_
/*==============================================================================
  FILE:           sleep_osi.h

  OVERVIEW:       This file contains sleep internal declarations of functions 
                  that are used in the common main sleep subsystem

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/os/sleep_osi.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep.h"
#include "sleep_os.h"

/*==============================================================================
                                    TYPES
 =============================================================================*/
/* Internal return values used for the status of the call to the kernel wait for idle API */
typedef enum
{
  WAIT_FOR_IDLE_ENTERED_STM = 0, /* All HW threads went idle, and function returned in STM */
  WAIT_FOR_IDLE_FORCED_EXIT      /* Someone requested an exit from the API call before all HW 
                                  * threads are idle.  System is NOT in STM when function returns. */
}idle_status;

/*==============================================================================
                                  DEFINITIONS
 =============================================================================*/
/* Size of main sleep task stack */
#define SLEEP_STACK_SIZE      2560

/* Size of PMI handler task stack */
#define SLEEP_PMI_STACK_SIZE  2048

/* PMI signal mask used to register the OS signal
 * that will be triggered as part of the APCR bringup sequence. */
#define SLEEP_PMI_MASK        (1 << 0)

/* Max number of characters in the full path to the sleep ini file. */
#define MAX_INI_PATH_LEN      64

/* Macro to check if any interrupt is pending at L2Vic.
 *
 * On QDSP6 V4 and above, the L2 Interrupt Controller is attached to INT31
 * of QDSP6'es IPEND register. All User configurable interrupts in the system
 * are attached ot L2 VIC. This makes it sufficient to check for L2 MSB */
#define IPEND_IS_MSB_SET(ipend_status)  ((ipend_status) & (1 << 31))

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepOS_configIdleMode
 * 
 * @brief This function configures how processor idle is handled within sleep.
 * 
 * @param idleMode: Idle time behavior/configuration for next cycle.
 */
void sleepOS_configIdleMode(sleepOS_IdleModeType idleMode);

/**
 * sleepOS_getFrequency
 *
 * @brief A query function to return cpu clock frequency in KHz.
 *
 * @return CPU frequency in KHz.
 */
uint32 sleepOS_getFrequency(void);

/**
 * sleepOS_mainTask
 *
 * @brief This is the main sleep task function
 *  
 * @note Function is located in island memory section.
 */
void sleepOS_mainTask(void *ignored);

/**
 * sleepOS_pmiHandler
 *
 * @brief Function responsible for handling Power Manager Interrupt (PMI).
 *
 * @param context: Thread context or data. (used for prototype compliance only)
 */
void sleepOS_pmiHandler(void *context);

/**
 * sleepOS_readEFSConfig 
 *  
 * @brief Initialize the sleep LPR subsystem.  When this callback is 
 *        called, the EFS subsystem has been initialized.  We rely on
 *        that in order to read the config files.
 *
 * @note Not all masters have access to EFS and require this function.
 */
void sleepOS_readEFSConfig(void);

/**
 * sleepOS_performLPM
 *
 * @brief Put the system in to the lowest possible power state allowed by
 *        imposed restrictions at run time.
 *
 * This function usually solves for the low power modes that can be entered
 * in special sleep context.
 *
 * @return Always 0 (for prototype compliance)
 */ 
uint32 sleepOS_performLPM(void);

/**
 * sleepOS_holdoffNoMode
 *
 * @brief This function simply returns and does not perform any type of low 
 *        power mode. Usually this is during system initialization or in a
 *        situation where we don't want target to enter any low power modes
 *        at all (e.g. debug, bring up). 
 */
uint32 sleepOS_holdoffNoMode(void);

/** 
 * sleepOS_setFrequency 
 *  
 * @brief Sets max frequency at current voltage when executing sleep.
 *
 * This function sets the max frequency for current voltage level when 
 * executing the sleep solver and executing low power modes.
 *
 * @return Returns the new/updated frequency in KHz.
 */
uint32 sleepOS_setFrequency(void);

/**
 * sleepOS_unsetFrequency 
 *
 * @brief Releasing Sleep request on CPU clock
 */
void sleepOS_unsetFrequency(void);

#endif /* _SLEEP_OSI_H_ */

