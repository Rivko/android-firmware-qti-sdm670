#ifndef _LPI_SOLVER_H_
#define _LPI_SOLVER_H_
/*==============================================================================
  FILE:         lpi_solver.h
  
  OVERVIEW:     Determines the LPI mode to enter, based on a lookup
                table provided by SleepSynth.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/solver/lpi_solver.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "lookup_table_types.h"
#include "SleepLPR_synth_modes.h"
#include "sleep_solveri.h"

/*==============================================================================
                             EXTERNAL VARIABLES
 =============================================================================*/
/**
 * @brief SleepLPI_power_lookup_table
 *
 * Master lookup table that contains data to select the correct island low power modes 
 * to enter based on allowable sleep time.
 */
extern sleep_lpiLut *SleepLPI_power_lookup_table[SLEEP_NUM_STATIC_FREQ];

/*==============================================================================
                              EXTERNAL FUNCTIONS
 =============================================================================*/
/** 
 * sleepLPISolver_initialize
 * 
 * @brief Solver init function
 */ 
#ifdef USLEEP_ISLAND_MODE_ENABLE
uint32 sleepLPISolver_initialize(void);
#else
#define sleepLPISolver_initialize() 0
#endif

/** 
 * sleepLPISolver_mainFunction
 * 
 * @brief Main solver function to select an island to enter. 
 *  
 * @note  It is assumed that an island has higher priority than any normal 
 *        low power mode.  As such, this function should be called and checked
 *        for valid output before solving for any normal mode. 
 *  
 * @return status of the solver
 */
#ifdef USLEEP_ISLAND_MODE_ENABLE
solver_status sleepLPISolver_mainFunction(sleep_solver_input  *input,
                                          sleep_solver_output *output);
#else
#define sleepLPISolver_mainFunction(input, output) SOLVER_NO_MODE_CHOSEN
#endif

#endif /* _LPI_SOLVER_H_ */

