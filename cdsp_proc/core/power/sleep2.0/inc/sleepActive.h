#ifndef _SLEEPACTIVE_H_
#define _SLEEPACTIVE_H_
/*==============================================================================
  FILE:         sleepActive.h

  OVERVIEW:     This file contains public APIs exposed by the Sleep Active-time 
                Solver Thread.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/inc/sleepActive.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "sleep_solver.h"

/*==============================================================================
                                GLOBAL TYPE DECLARATIONS
  =============================================================================*/ 
/* List of Signals that cause the Active Time solver 
 * to re-evaluate its selection of low-power modes */
typedef enum
{
  /** Data signals **/
  SLEEP_SIGNAL_RESERVED             = 0x00000001, /* Unused */

  SLEEP_SIGNAL_NORMAL_LATENCY       = 0x00000002, /* Update in Latency Constraint */
  SLEEP_SIGNAL_SOFT_DURATION        = 0x00000004, /* Update Idle Soft Duration */
  SLEEP_SIGNAL_REGISTRY             = 0x00000008, /* Update List of Enabled Synth Modes */

  SLEEP_SIGNAL_CPU_FREQUENCY        = 0x00000010, /* Cpu Frequency Change */
  SLEEP_SIGNAL_HARD_DURATION        = 0x00000020, /* Update normal timer hard duration */
  SLEEP_SIGNAL_RPM_QUEUE_CHANGE     = 0x00000040, /* RPM message queue is empty */
  SLEEP_SIGNAL_ISLAND_LATENCY       = 0x00000080, /* Update in island mode latency constraint */

  SLEEP_SIGNAL_ISLAND_HARD_DURATION = 0x00000100, /* Update island timer hard duration */
  SLEEP_SIGNAL_THRESHOLD_EXPIRED    = 0x00000200, /* Threshold timer expired, need to resolve */
  SLEEP_SIGNAL_WAKE_LATENCY_CHANGED = 0x00000400, /* Wake set latency value changed */

  SLEEP_SIGNAL_LAST_DATA            = 0x00000800, /* MUST BE LAST DATA SIGNAL */

  /** Control signal to control main ATS operation **/
  SLEEP_SIGNAL_CTL_INIT_COMPLETE    = 0x00010000, /* ATS init complete */
  SLEEP_SIGNAL_CTL_RCINIT_DONE      = 0x00020000, /* RCinit complete */
  SLEEP_SIGNAL_CTL_PERFORM_PROFILE  = 0x00040000, /* Update profiling data for last entered mode */

  SLEEP_SIGNAL_LAST_CTL             = 0x00080000, /* MUST BE LAST CONTROL SIGNAL */

  /** Signal masks **/
  SLEEP_SIGNAL_MASK_ALL_DATA  = (SLEEP_SIGNAL_LAST_DATA - 1),   /* Mask for all data signals */
  SLEEP_SIGNAL_MASK_ALL_CTRL  = (~SLEEP_SIGNAL_MASK_ALL_DATA),  /* Mask for all control signals */

  SLEEP_SIGNAL_MASK_STANDARD  = (SLEEP_SIGNAL_MASK_ALL_DATA | SLEEP_SIGNAL_CTL_PERFORM_PROFILE)

}sleepActive_SignalType;

/*==============================================================================
                              GLOBAL FUNCTION DECLARATIONS
  =============================================================================*/
/** 
 * sleepActive_SetSignal
 * 
 * @brief Trigger signal(s) of type sig on Active Time solver thread
 *
 *        Signals set on the Active Time solver are used to determine 
 *        which execution conditions that have changed. 
 *        The Active Time solver thread executes the sleep solver to determine
 *        whether these modified inputs cause a change in the selection of 
 *        low-power mode.
 * 
 * @param signals: A single or bitwise OR of specific signals to be set
 *                 for Active Time solver to take action on
 */
void sleepActive_SetSignal(sleepActive_SignalType signals);

/** 
 * sleepActive_GetSolverHardDeadlines 
 *  
 * @brief Returns the hard deadline times (in absolute ticks) that 
 *        were used by the solver to select the low power mode 
 *  
 * @return A pointer to the deadline data structure
 */
const sleep_solver_deadlines *sleepActive_GetSolverHardDeadlines(void);

/**
 * sleepActive_setRPMQueueStatus
 * 
 * @brief This function is called to indicate the state of the RPM message queue
 *  
 * @param isEmpty:  TRUE/FALSE value to indicate if the queue is empty or not 
 */
void sleepActive_setRPMQueueStatus(boolean isEmpty);

/**
 * sleepActive_clearThreasholdTimer
 * 
 * @brief Clears the threshold timer
 */
void sleepActive_clearThreasholdTimer(void);

#endif /* _SLEEPACTIVE_H_ */

