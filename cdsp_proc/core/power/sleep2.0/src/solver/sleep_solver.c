/*==============================================================================
  FILE:         sleep_solver.c
  
  OVERVIEW:     This file implements the sleep module interface for configuring
                the "solver" that is invoked at sleep time to decide which low
                power resources should be put into what modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/solver/sleep_solver.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep_log.h"
#include "sleep_solveri.h"
#include "lpr_solver.h"

#ifdef USLEEP_ISLAND_MODE_ENABLE
#include "lpi_solver.h"
#endif

/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/
extern volatile uint8 g_sleepAllowLowPowerModes;

/*==============================================================================
                               EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * sleepSolver_init
 */
void sleepSolver_init(void)
{
  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 0, "Init Solver");

  sleepLPRSolver_initialize();

#ifdef USLEEP_ISLAND_MODE_ENABLE
  sleepLPISolver_initialize();
#endif

  return;
}

/*
 * sleepSolver_solveMode
 */
solver_status sleepSolver_solveMode(sleep_solver_input  *input,
                                    sleep_solver_output *output)
{
  solver_status status = SOLVER_NO_MODE_CHOSEN;

  CORE_VERIFY_PTR(input);
  CORE_VERIFY_PTR(output);

  /* Set default values for solver output */
  memset(output, 0, sizeof(sleep_solver_output));

  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_ATS, SLEEP_ENTER_SOLVER_NUM_ARGS,
                      SLEEP_ENTER_SOLVER_STR, SLEEP_ENTER_SOLVER,
                      input->fLUT->frequency,
                      ULOG64_DATA(input->soft_duration),
                      input->normal_latency_budget,
                      input->island_latency_budget);

  /* Check globally enabled sleep flag. If not enable, do not run actual solver functions and
   * simply return no mode */
  if(g_sleepAllowLowPowerModes == 1)
  {
#ifdef USLEEP_ISLAND_MODE_ENABLE
    /* LPI is highest priority, so check if one can be entered first.
     * The LPI solver may or may not return a valid mode */
    status = sleepLPISolver_mainFunction(input, output);
#endif

    if(SOLVER_NO_MODE_CHOSEN      == status     ||
       SOLVER_REGISTRY_DISABLED   == status     ||
       SOLVER_LATENCY_RESTRICTED  == status)
    {
      /* Solve for normal mode if no other mode was previously chosen */
      status = sleepLPRSolver_mainFunction(input, output);
    }
  }

  return status;
}

