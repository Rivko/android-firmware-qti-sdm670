#ifndef _SLEEP_SOLVERI_H_
#define _SLEEP_SOLVERI_H_
/*==============================================================================
  FILE:         sleep_solveri.h
  
  OVERVIEW:     This file declares the private interface used to declare and configure
                "solvers" that implement the decision logic for the sleep task.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/solver/sleep_solveri.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep_solver.h"
#include "synthLPRM.h"
#include "synthTypes.h"
#include "sleep_lpr.h"
#include "lookup_table_types.h"

/*==============================================================================
                          GLOBAL TYPE DEFINITIONS
 =============================================================================*/
/* This structure contains data that is needed by the sleep solver,
 * in order to make a decision about which low power modes should be
 * entered. */
typedef struct sleep_solver_input_s
{
  uint32                  normal_latency_budget;  /* Normal mode latency restriction in ticks */
  uint32                  island_latency_budget;  /* Island latency restriction in ticks */
  sleep_solver_deadlines  hard_deadlines;         /* Various hard deadlines */
  uint64                  soft_duration;          /* Soft wake-up duration in ticks */
  sleep_fLUT_node         *fLUT;                  /* Pointer to the freq LUT */
  uint32                  last_cpu_freq_khz;      /* Value of last CPU frequency (kHz) change */
} sleep_solver_input;

/* This structure contains information that is needed by the 
 * caller of the solver, in order to enter the selected modes, 
 * and wake up on time. 
 *
 * With active time solving, mode's backoff can change after it is selected
 * due to auto-backoff kicking in but that may not get propagated properly
 * everywhere if ATS is not triggered subsequently before another sleep 
 * cycle. We use pointer to that value so whenever it is referenced, value 
 * is always most recent at that point. */
typedef struct sleep_solver_output_s
{
  sleep_synth_lprm  *selected_synthLPRM;    /* Solution array as output */
  const uint32      *backoff_ptr;           /* Pointer to total backoff latency
                                               of selected modes */
  uint32            enter_exit_time;        /* Total latency of selected modes */
  uint64            ref_deadline;           /* Deadline used for duration */
  uint64            threshold_deadline;     /* Threshold deadline - chosen mode
                                               may not be valid after this */  
} sleep_solver_output;

/* Valid return values of the solver functions */
typedef enum 
{
  SOLVER_SUCCESS              = 0,  /* Valid mode was chosen */
  SOLVER_DEADLINE_IN_PAST,          /* No mode chosen due to wakeup in past */
  SOLVER_LATENCY_RESTRICTED,        /* Input is valid, but latency budget restricted */
  SOLVER_REGISTRY_DISABLED,         /* LPR/LPI registry disabled */
  SOLVER_NO_MODE_CHOSEN,            /* Input is valid, but no mode was able to be chosen */
}solver_status;

/*==============================================================================
                          GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepSolver_init
 *
 * @brief Initializes the solver function
 */
void sleepSolver_init(void);

/**
 * @brief Main sleep solver function which on execution will
 *        select low power modes that should be entered during CPU idle, based
 *        on the current dynamic conditions.
 *
 * @param input:  List of constraints that are used by the solver 
 *                to select modes.
 * @param output: List of output data from the solver -- 
 *                contains the list of modes to enter, as well as
 *                the backoff time that should be used.
 *  
 * @return status of the solver 
 */
solver_status sleepSolver_solveMode(sleep_solver_input  *input,
                                    sleep_solver_output *output);

#endif /* _SLEEP_SOLVERI_H_ */

