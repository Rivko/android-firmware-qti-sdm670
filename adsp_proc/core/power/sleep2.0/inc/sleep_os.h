#ifndef _SLEEP_OS_H_
#define _SLEEP_OS_H_
/*==============================================================================
  FILE:           sleep_os.h

  OVERVIEW:       This file contains declarations of functions that are used
                  in the common main sleep subsystem

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/inc/sleep_os.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "sleep.h"

/*==============================================================================
                                   TYPES
 =============================================================================*/
/* PMI & profling ISR function pointer that is called durring PC exit. The 
 * pointer will be modified to point to the correct handler based on if
 * the system is in uImage or normal operational modes. */
typedef void (*sleep_PMI_handler_ptr_type)(void);
typedef void (*sleep_profile_handler_ptr_type)(void);

/* Type definition for function pointer to sleep behavior when processor goes idle. */
typedef uint32 (*sleep_idle_entry_ptr_type)(void);

/* Enumeration to indicate the Idle Mode Type */
typedef enum
{
  SLEEP_OS_IDLE_MODE_DEFAULT   = 0, /* Configure for idle time sleep */
  SLEEP_OS_IDLE_MODE_OVERRIDE,      /* Configure for low overhead sleep */
  SLEEP_OS_IDLE_MODE_LPI,           /* Configure for LPI based sleep */
  SLEEP_OS_IDLE_MODE_HOLD_OFF,      /* Configure for busy wait during idle */

  SLEEP_OS_IDLE_MODE_NUM_STATES     /* Number of valid idle mode states */
}sleepOS_IdleModeType;

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepOS_verifyIdleModeConfig
 *
 * @brief This function checks how processor idle is set to be handled with
 *        the input config.
 *
 * @param idleMode: Enum for how processor idle is expected to be handled.
 *
 * @return TRUE if processor idle is currently handled in a same way as 
 *         specified by idleMode else FALSE.
 */
boolean sleepOS_verifyIdleModeConfig(sleepOS_IdleModeType idleMode);

/**
 * sleepOS_getHardDeadline 
 *  
 * @brief Returns the hard deadline that the sleep task must be 
 *        awake by, in absolute ticks.  This is the time that
 *        sleep expects to exit by.
 *  
 *        NOTE:  It only makes sense to call this function from
 *               within the idle context (i.e., from within an
 *               LPR).
 *  
 * @return The hard deadline, in absolute ticks, that sleep 
 *         expects to exit by.
 */
uint64 sleepOS_getHardDeadline(void);

/**
 * sleepOS_PMIPerformer
 *
 * @brief PMI handler code for normal operational image.
 */
void sleepOS_PMIPerformer(void);

/**
 * sleepOS_profilePerformer
 *
 * @brief Profile handler code for normal operational image.
 */
void sleepOS_profilePerformer(void);

/**
 * sleepOS_prepareForSleep 
 *  
 * @brief Common logic that will verify it is safe to enter a low power mode. 
 *  
 *        If there is an entry failure status, the appropriate action will have
 *        already occured, the caller will not need to take further action.
 *  
 *        If the function returns success, it will have already entered STM
 *        and the caller should continue to enter a low power mode.
 *  
 * @param idleModeCfg: Enum to indicate how processor idle is expected to 
 *                     be handled currently.
 *  
 * @return Time of STM entry (in ticks) or 0 if failure 
 */
uint64 sleepOS_prepareForSleep(sleepOS_IdleModeType idleModeCfg);

/**
 * sleepOS_completeSleep 
 *  
 * @brief Common logic that will perform cleanup upon exiting idle time sleep. 
 *  
 * @note This should be paired and called after sleepOS_prepareForSleep. 
 * 
 * @param exitStm: Flag that indicates if STM should truly be exited by makeing the kernel call
 */
void sleepOS_completeSleep(boolean exitStm);

/**
 * sleepOS_setLPIEntryFunction
 * 
 * @brief This function configures the entry function for an LPI.
 *
 * @param lpiFunction: Function that will be called when an LPI is able to enter.
 */
void sleepOS_setLPIEntryFunction(sleep_idle_entry_ptr_type lpiFunction);

/**
 * sleepOS_getLPIEntryFunction
 * 
 * @brief This function gets the current function that will execute when an LPI 
 *        sleep mode is configured 
 *
 * @return LPI function
 */
sleep_idle_entry_ptr_type sleepOS_getLPIEntryFunction(void);

/**
 * sleepOS_setPMIFunctionPTR 
 * sleepOS_getPMIFunctionPTR 
 *
 * @brief These functions set/get the PMI handler that will be called from the ISR        
 *  
 * @param newPMIPtr:  Pointer to the function to call when the PMI ISR is 
 *                    invoked.
 */
void sleepOS_setPMIFunctionPTR(sleep_PMI_handler_ptr_type newPtr);
sleep_PMI_handler_ptr_type sleepOS_getPMIFunctionPTR(void);

/**
 * sleepOS_setProfileFunctionPTR 
 * sleepOS_getProfileFunctionPTR 
 *
 * @brief These functions set/get the profiling handler that will be called from the ISR        
 *  
 * @param newPMIPtr:  Pointer to the function to call when the ISR is 
 *                    invoked.
 */
void sleepOS_setProfileFunctionPTR(sleep_PMI_handler_ptr_type newPtr);
sleep_PMI_handler_ptr_type sleepOS_getProfileFunctionPTR(void);

#endif /* _SLEEP_OS_H_ */
