#ifndef _SLEEPACTIVEI_H_
#define _SLEEPACTIVEI_H_
/*==============================================================================
  FILE:         sleepActivei.h

  OVERVIEW:     This file contains internal APIs exposed by the Sleep Active-time 
                Solver Thread.

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/active/sleepActivei.h#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include "comdef.h"
#include "sleep_solveri.h"
#include "synthTypes.h"

/*==============================================================================
                                  MACROS & TYPES
  =============================================================================*/
#define SLEEPSOLVER_STACK_SIZE 2048

/*==============================================================================
                              GLOBAL FUNCTION DECLARATIONS
  =============================================================================*/
/**
 * @brief Returns the input parameters used by Sleep Active for 
 *        input to Active time solver
 *  
 * @return A pointer to the solver input struct
 */
sleep_solver_input *sleepActive_GetSolverInput(void);

/**
 * @brief Returns the low-power mode selected by Active time 
 *        solver
 *  
 * @return A pointer to the last chosen solver output struct
 */
sleep_solver_output *sleepActive_GetSolverOutput(void);

/**
 * @brief Configures what the main sleep task will execute when the subsystem 
 *        enters the idle state.
 *  
 * @param synthLPRM:  Selected mode to be entered at idle time 
 */
void sleepActive_configureIdleMode(sleep_synth_lprm *synthLPRM);

#endif /* _SLEEPACTIVEI_H_ */

