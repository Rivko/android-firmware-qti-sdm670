#ifndef _SLEEP_SOLVER_H_
#define _SLEEP_SOLVER_H_
/*==============================================================================
  FILE:         sleep_solver.h
  
  OVERVIEW:     This file declares the public types and functions to access data
                from the main sleep solver.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/inc/sleep_solver.h#2 $
$DateTime: 2017/09/13 08:52:52 $
==============================================================================*/
#include "comdef.h"
#include "utimer.h"
#include "timer.h"

/*==============================================================================
                          GLOBAL TYPE DEFINITIONS
 =============================================================================*/
/* Timer ID structure data for the first non-deferrable */
typedef struct sleep_deadline_timers_s
{
  uint64                value;      /* Timer absolute expiry time */

  union
  {
    timer_non_defer_data  normal;   /* Owner info for the timer */
    utimer_non_defer_data island;   
  }owner;
}sleep_deadline_timers;

/* Data structure to hold the various hard deadlines that the solver will use to 
 * select a low power mode. */
typedef struct sleep_solver_deadlines_s
{
  sleep_deadline_timers island_timer; /* Next non-deferrable island timer expiry */
  sleep_deadline_timers normal_timer; /* Next non-deferrable normal-mode timer expiry */
  uint64                normal_min;   /* Minimum of all normal mode specific deadlines */
  uint64                overall_timer_min;  /* Minimum of all non-deferrable qtimer deadlines */
  uint64                overall_min;  /* Minimum of all island & normal mode deadlines */
} sleep_solver_deadlines;

/*==============================================================================
                                   FUNCTIONS
 =============================================================================*/
/**
 * sleepLPRSolver_allowCacheableModes
 * 
 * @brief Allows the solver to select cacheable synthmodes.  This can be used to
 *        force an idle timeline if cacheable modes are disabled.
 * 
 * @param allow:  Flag to allow or disallow cacheable synthmodes
 */
void sleepLPRSolver_allowCacheableModes(boolean allow);

#endif /* _SLEEP_SOLVER_H_ */

