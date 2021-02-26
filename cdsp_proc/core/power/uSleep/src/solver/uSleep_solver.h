#ifndef _USLEEP_SOVER_H_
#define _USLEEP_SOVER_H_
/*==============================================================================
  FILE:         uSleep_solver.h

  OVERVIEW:     Functions for the uSleep solver

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/solver/uSleep_solver.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_lpri.h"
#include "utimer.h"

/*==============================================================================
                             TYPES & DEFINITIONS
 =============================================================================*/
/* Solver input parameters */
typedef struct uSleep_solver_input_s
{
  uint64                wakeup_deadline;
  utimer_non_defer_data timer_owner;
#ifdef UNPA_SUPPORT
  uint32                latency_budget;
#endif
}uSleep_solver_input;

/* Solver output parameters */
typedef struct uSleep_solver_output_s
{
  uSleep_LPR  *mode;
  uint64      threshold_deadline;
}uSleep_solver_output;

/* Valid return values of the solver functions */
typedef enum 
{
  USOLVER_SUCCESS              = 0,  /* Valid mode was chosen */
  USOLVER_DEADLINE_IN_PAST,          /* No mode chosen due to wakeup in past */
}uSolver_status;

/* Dynamic solver function type */
typedef uSolver_status (*usleep_solver_fcn)(uSleep_solver_input   *input, 
                                            uSleep_solver_output  *output);

/*==============================================================================
                              EXTERNAL VARIABLES
 =============================================================================*/
extern volatile boolean g_uSleepAllowLowPowerModes;

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * uSleepSolver_solve
 * 
 * @brief Internally sets and returns the un-cacheable mode that should be 
 *        entered with the allowed amount of sleep time.  Once called, only
 *        uSleepLPR_runLPRFunctions(...) is required to enter the chosen mode.
 *   
 * @param wakeupDeadline: Value (in absolute ticks) of the hard deadline wakeup 
 *                        time.
 * 
 * @return Status of solver
 */
uSolver_status uSleepSolver_solve(uSleep_solver_input   *input, 
                                  uSleep_solver_output  *output);

#endif /* _USLEEP_SOVER_H_ */

