#ifndef _LPR_SOLVER_H_
#define _LPR_SOLVER_H_
/*==============================================================================
  FILE:         lpr_solver.h
  
  OVERVIEW:     Determines the LPR mode to enter, based on a lookup
                table provided by SleepSynth.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/solver/lpr_solver.h#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "lookup_table_types.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_solveri.h"

/*==============================================================================
                             EXTERNAL VARIABLES
 =============================================================================*/
/* Master lookup table that contains data to select the correct normal low power modes
 * to enter based on allowable sleep time. */
extern sleep_mLUT *SleepLPR_power_lookup_table[SLEEP_NUM_STATIC_FREQ];

/* Static frequency list from the XML inputs */
extern uint32 SleepLPR_static_freq_list[SLEEP_NUM_STATIC_FREQ];

/*==============================================================================
                              EXTERNAL FUNCTIONS
 =============================================================================*/
/** 
 * sleepLPRSolver_initialize
 * 
 * @brief Solver init function
 */ 
uint32 sleepLPRSolver_initialize(void);

/** 
 * sleepLPRSolver_mainFunction
 * 
 * @brief Main solver function to choose an LPR to enter based on the input
 *        constraints
 *
 * @param input:  Various input restrictions used to determine the power mode to enter 
 * @param output: Selected output mode and other data associated with the selected mode 
 *  
 * @return status of the solver 
 */ 
solver_status sleepLPRSolver_mainFunction(sleep_solver_input  *input,
                                          sleep_solver_output *output);

#endif /* _LPR_SOLVER_H_ */

