/*==============================================================================
  FILE:         simple_solver.c

  OVERVIEW:     This file provides the support for the simple sover

  DEPENDENCIES: Object file generated from source is marked as island section
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/solver/simple_solver.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uCoreTime.h"
#include "uSleep_solver.h"
#include "uSleep_log.h"
#include "uSleep_lpr.h"
#include "qurt.h"
#include "uSleepi.h"

/*==============================================================================
                          EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * uSleepSolver_solve
 */
uSolver_status simpleSolver_solve(uSleep_solver_input   *input, 
                                  uSleep_solver_output  *output)
{
  uSleep_LPR  *mode;
  uint32      modeLatency;
  uint64      duration;
  int32       idx             = USLEEP_NUMBER_LOW_POWER_MODES - 1;
  uint64      wakeupDeadline  = input->wakeup_deadline;

  CORE_VERIFY_PTR(input);
  CORE_VERIFY_PTR(output);

  /* Set default values for solver output */
  memset(output, 0, sizeof(uSleep_solver_output));

  if(FALSE == g_uSleepAllowLowPowerModes)
  {
    /* Return the default low power mode which must always be valid and enabled at all times */
    uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0, " Power modes disabled, selecting default");
    output->mode                = &g_uSleepDefaultLPR;
    output->threshold_deadline  = 0;  /* No need to re-solve based on threshold timer */
    return USOLVER_SUCCESS;
  }
  
#ifdef UNPA_SUPPORT
  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1 + (1 * 2),
                   " Simple Solver (Hard Deadline: 0x%llx) "
                   "(Latency budget 0x%x)",
                   MICRO_ULOG64_DATA(wakeupDeadline),
                   input->latency_budget);
#else
  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, (1 * 2),
                   " Simple Solver (Hard Deadline: 0x%llx)",
                   MICRO_ULOG64_DATA(wakeupDeadline));
#endif

  /* Start at highest index (most power savings) and work backwards */
  while(idx >= 0)
  {
    mode        = &g_uSleepLPR[idx];
    modeLatency = mode->working_latency.enter + mode->working_latency.exit;

    if(wakeupDeadline <= uCoreTimetick_Get64())
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 0, " Solver bypass - Hard Deadline in past");
      return USOLVER_DEADLINE_IN_PAST;
    }

    duration = wakeupDeadline - uCoreTimetick_Get64();

    /* Both user and internal enablement flags must be set for LPR to truly be enabled.
     * Duration is compared against enter+exit+backoff to account for useful time actually in 
     * the mode since we are not using the traditional LUT as normal sleep has */
    if((USLEEP_LPR_ENABLED == mode->enabled_flags)  && 
       (duration > (modeLatency + mode->backoff))
#ifdef UNPA_SUPPORT
       && (input->latency_budget > modeLatency)
#endif
      )
    {
      output->threshold_deadline = wakeupDeadline - modeLatency;

      /* If threashold will expire 'relativly' soon, it's just not worth selecting this mode 
       * so try to find the next one. */
      if((uCoreTimetick_Get64() >= output->threshold_deadline) ||
         ((output->threshold_deadline - uCoreTimetick_Get64()) < USLEEP_THRESHOLD_LIMIT_CHECK)
        )
      {
        uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1,
                       " Mode skipped "
                       "(Name: %s) "
                       "(Reason: Close to threshold)",
                       mode->name);

        output->threshold_deadline = 0;
      }
      else
      {
        output->mode = mode;
        break;
      }
    }
    /* Only print if internally enabled from compile time */
    else if(USLEEP_LPR_INTERNAL_ENABLE_MASK == (mode->enabled_flags & USLEEP_LPR_INTERNAL_ENABLE_MASK)) 
    {
      uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 3,
                       " Mode skipped "
                       "(Name: %s) "
                       "(Enabled: %s) "
                       "(Latency: 0x%x)",
                       mode->name,
                       (mode->enabled_flags & USLEEP_LPR_ENABLE_MASK) ? "True" : "False",
                       modeLatency);
    }

    idx--;
  }
 
  if(NULL == output->mode)
  {
    output->mode                = &g_uSleepDefaultLPR;
    output->threshold_deadline  = 0;  /* No need to re-solve based on threshold timer */
  }
#if 0
  else if(TRUE == output->mode->cached)
  {
    /* Don't set a threashold for cacheable modes since they should be fast enough and it's not
     * worth resolving */
    output->threshold_deadline  = 0;
  }
#endif
  uSleepLog_printf(USLEEP_LOG_LEVEL_PROFILING, 1,
                     "Solver mode chosen (Mode: %s)",
                     output->mode->name);

  return USOLVER_SUCCESS;
}

