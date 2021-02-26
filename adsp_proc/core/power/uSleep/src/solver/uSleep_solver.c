/*==============================================================================
  FILE:         uSleep_solver.c

  OVERVIEW:     This file provides the support for the simple sover in island mode

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/uSleep/src/solver/uSleep_solver.c#3 $
$DateTime: 2017/11/11 10:49:47 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_solver.h"
#include "uSleep_log.h"
#include "uSleep_lpr.h"
#include "simple_solver.h"

/*==============================================================================
                              INTERNAL VARIABLES
  =============================================================================*/
/* Boolean that indicates whether we should enter any low power modes or not.
 *
 * This variable provides a simpler mechanism to disable low power modes
 * for developing/testing features that are not quite dependent on low 
 * power modes. Note that in the production code, it will be set so that
 * low power modes are always allowed. */
#ifdef FEATURE_DISABLE_USLEEP_MODES
volatile boolean      g_uSleepAllowLowPowerModes = FALSE;
#else
volatile boolean      g_uSleepAllowLowPowerModes = TRUE;
#endif

/*==============================================================================
                              EXTERNAL FUNCTION
 =============================================================================*/
/*
 * uSleepSolver_solve
 */
uSolver_status uSleepSolver_solve(uSleep_solver_input   *input, 
                                  uSleep_solver_output  *output)
{
  /* Call actual solver function */
  return(simpleSolver_solve(input, output));
}

