#ifndef _SIMPLE_SOVER_H_
#define _SIMPLE_SOVER_H_
/*==============================================================================
  FILE:         simple_solver.h

  OVERVIEW:     Prototypes & definitions for the simple solver

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/solver/simple_solver.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "uSleep_solver.h"

/*==============================================================================
                            FUNCTION DECLARATIONS
 =============================================================================*/
/** 
 * simpleSolver_solve
 * 
 * @brief Implementation of the simple solver that returns the power mode to enter
 *        given the wakeup deadline
 *   
 * @param input:  Various input parameters required to solver for a low power mode
 * @param output: Output paremters after running solver function
 * 
 * @return Status of solver
 */
uSolver_status simpleSolver_solve(uSleep_solver_input   *input,
                                  uSleep_solver_output  *output);

#endif /* _SIMPLE_SOVER_H_ */

