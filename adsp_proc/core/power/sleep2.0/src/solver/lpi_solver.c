/*==============================================================================
  FILE:         lpi_solver.c
 
  OVERVIEW:     This file provides an extention to the sleep solver.  This is
                used to determine if an LPI should be entered.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/solver/lpi_solver.c#2 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#ifdef USLEEP_ISLAND_MODE_ENABLE

#include "comdef.h"
#include "CoreVerify.h"
#include "CoreTime.h"
#include "sleep_log.h"
#include "SleepLPI_synth_modes.h"
#include "lpi_solver.h"
#include "synthLPI.h"
#include "synthRegistry.h"

/*==============================================================================
                               INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepLPISolver_findElement
 *
 * @brief This function finds the lookup table element that 
 *        contains a duration range that satisfies the sleep
 *        duration passed in, and returns a pointer to that
 *        element.  This function can be optimized/modified
 *        independently of the main solver function.
 *
 * @param modeDuration:    Normal mode sleep duration to find the lookup 
 *                         table element for.
 *  
 * @param freqLutIdx:      Frequency table index based on
 *                         current core frequency 
 *  
 * @return Pointer to the lookup table element whose duration 
 *         range contains the sleep duration passed in.
 */
static sleep_lpiLut_elem *sleepLPISolver_findElement(uint64 modeDuration,
                                                     uint32 freqLutIdx)
{
  sleep_lpiLut        *modeLut;
  sleep_lpiLut_elem   *lpiLutElement = NULL;

  /* Get correct LUT for the given frequency */
  modeLut = SleepLPI_power_lookup_table[freqLutIdx];

  /* Current assumption is only 1 LPI exists */
  CORE_VERIFY(1 == modeLut->num_elements);

  if(modeDuration >= modeLut->element[0].duration_thresh)
  {
    lpiLutElement = &modeLut->element[0];
  }

  /* Return the only possible element */
  return(lpiLutElement);

}

/** 
 * sleepLPRSolver_setThresholdDeadline 
 * 
 * @brief This function outputs threshold deadline when the mode selected by
 *        lookup table solver may no longer be optimal. 
 *
 * @param durationThresh : Lookup table element duration threshold value 
 * @param modeEnterLat   : Chosen modes enter latency
 * @param normalDeadline : Normal mode deadline (hard or soft)
 * @param islandDeadline : Island mode hard deadline
 * @param refDeadline    : Outputed reference wakeup deadline
 *
 * @note This is meant to be an internal function and assumes valid input
 *       parameters.
 *  
 * @return Deadline when the chosen mode is no longer optimal 
 */
static uint64 sleepLPISolver_setThresholdDeadline(uint64 durationThresh,
                                                  uint32 modeEnterLat,
                                                  uint64 normalDeadline,
                                                  uint64 islandDeadline,
                                                  uint64 *refDeadline)
{
  uint64 islandNonOptimalDeadline;
  uint64 normalNonOptimalDeadline;

  /* The island deadline will not be optimial once we get within the duration 
   * of the modes entry latency */ 
  islandNonOptimalDeadline = islandDeadline - modeEnterLat;

  /* The normal mode duration is guaranteed to be greater than the mode duration threshold and will
   * not be optimal when we get within the modes duration threshold. */
  normalNonOptimalDeadline = normalDeadline - durationThresh;

  /* Use the worst case of the two non-optimal deadlines */
  if(islandNonOptimalDeadline < normalNonOptimalDeadline)
  {
    *refDeadline = islandDeadline;
    return(islandNonOptimalDeadline);
  }
  else
  {
    *refDeadline = normalDeadline;
    return(normalNonOptimalDeadline);
  }
}

/*
 * sleepLPISolver_initialize 
 */
uint32 sleepLPISolver_initialize(void)
{
  synthRegistry *registry;
  uint32        freqIdx, durIdx, nMode;
  uint32        modeIndex;

  registry = synthRegistry_getRegistry();

  /* There will always be at least 1 table */
  for(freqIdx = 0; freqIdx < SLEEP_NUM_STATIC_FREQ; freqIdx++)
  {
    sleep_lpiLut *pTable = SleepLPI_power_lookup_table[freqIdx]; 

    for(durIdx = 0; durIdx < pTable->num_elements; durIdx++)
    {
      /* Reset table stats */
      pTable->element[durIdx].element_stats.min = UINT64_MAX;

      for( nMode = 0; nMode < SLEEP_LPI_NUM_SYNTHESIZED_MODES; nMode++)
      {
        if(-1 == pTable->element[durIdx].mode[nMode].mode_idx) 
        {
          pTable->element[durIdx].mode[nMode].mode_ptr = NULL; 
        }
        else
        {
          modeIndex = pTable->element[durIdx].mode[nMode].mode_idx;
          pTable->element[durIdx].mode[nMode].mode_ptr = &registry->LPI->modes[modeIndex];
        }
      }
    }
  }

  return 0;
}

/**
 * sleepLPISolver_mainFunction
 *
 * @brief This function finds the right low power mode to enter, 
 *        based on a pre-determined lookup table.  Given
 *        duration and latency restrictions, the low power mode
 *        is chosen from the table.
 *
 * @param input:  List of constraints that are used by the solver 
 *                to select modes.
 * @param output: List of output data from the solver -- 
 *                contains the list of modes to enter, as well as
 *                the backoff time that should be used.
 */
solver_status sleepLPISolver_mainFunction(sleep_solver_input  *input,
                                          sleep_solver_output *output)
{
  int32             i;
  uint64            normalSleepDuration;
  uint64            normalSoftDeadline;
  uint64            normalHardDuration;
  uint64            islandHardDuration;
  uint32            modeEnterExitLat;
  uint32            modeEnterLat;
  sleep_lpiLut_elem *lookupElement    = NULL;
  sleep_synth_lpim  *modeChosen       = NULL;
  sleep_fLUT_node   *fLUT             = input->fLUT;

  if(SLEEP_ENABLED == synthRegistry_getStatus(REGISTRY_TYPE_LPI))
  {
    if(input->hard_deadlines.overall_min <= CoreTimetick_Get64())
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_INFO, (1 * 2), 
                      " LPI Solver bypass - Deadline in past "
                      "(Hard Deadline 0x%llx)",
                      ULOG64_DATA(input->hard_deadlines.overall_min));

      return SOLVER_DEADLINE_IN_PAST;
    }

    /* Compute the soft deadline from the duration */
    normalSoftDeadline = CoreTimetick_Get64() + input->soft_duration;
    
    /* Calculate hard duration from first hard deadline */
    normalHardDuration  = input->hard_deadlines.normal_min - CoreTimetick_Get64();
    islandHardDuration  = input->hard_deadlines.island_timer.value - CoreTimetick_Get64();

    /* Use the most restrictive of the normal hard or soft durations for overall sleep duration */
    normalSleepDuration = MIN(normalHardDuration, input->soft_duration);
    
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 0 + (2 * 2), 
                    " LPI Solver (Normal Duration: 0x%llx) "
                    "(Island Duration: 0x%llx) ",
                    ULOG64_DATA(normalHardDuration),
                    ULOG64_DATA(islandHardDuration));

    /* Find the lookup table entry for this duration which uses the normal mode duration to
     * keep things simple for now */
    lookupElement = sleepLPISolver_findElement(normalSleepDuration, fLUT->mLUT_idx);

    if(lookupElement != NULL)
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1 + (1 * 2),
                      " LPI table (mLUT: %d) (Duration: %lld)",
                      fLUT->mLUT_idx,
                      ULOG64_DATA(lookupElement->duration_thresh));

      /* Traverse the modes for the duration range found, and find the one
       * that fits the current criteria. */
      for(i = 0; i < SLEEP_LPI_NUM_SYNTHESIZED_MODES; i++)
      {
        modeChosen = lookupElement->mode[i].mode_ptr;

        /* If we hit a NULL mode, then we've reached the end of the mode list,
         * and didn't find a mode to enter. */
        if(NULL == modeChosen)
        {
          break;
        }

        /* Skip if mode is disabled */
        if(SLEEP_DISABLED == modeChosen->mode_status) 
        {
          continue;
        }

        /* Get total latency times for determining if normal mode restricts the mode */
        modeEnterExitLat = synthLPRM_getLatency(modeChosen, fLUT->mLUT_idx);
        
        /* Verify if normal mode would restrict choosing an LPI if the 
         * total mode latency > normal latency node restriction.
         * This is because if an normal mode interrupt fires just after starting entry,
         * then the interrupt has to wait enter+exit amount of time before being handled. */
        if(modeEnterExitLat > input->normal_latency_budget)
        {
          sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 3,
                          " Mode skipped (name: \"%s\") "
                          "(reason: \"Normal Latency Restricted\") "
                          "(Total mode latency: 0x%x) "
                          "(Normal latency budget 0x%x)",
                          modeChosen->name, 
                          modeEnterExitLat,
                          input->normal_latency_budget);

          /* Can not choose LPI due to normal mode restrictions */
          continue;
        }

        /* Get the mode entry latency to determine if there are any island restrictions */
        modeEnterLat = synthLPRM_getEnterLatency(modeChosen, fLUT->mLUT_idx);

        /* Verify island restrictions to LPI entry latency */
        if((modeEnterLat > islandHardDuration) ||
           (modeEnterLat > input->island_latency_budget))
        {
          sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 3 + (1 * 2),
                          " Mode skipped (name: \"%s\") "
                          "(reason: \"Island Mode Restricted\") "
                          "(Enter latency: 0x%x) "
                          "(Sleep Duration 0x%llx) "
                          "(Island latency budget 0x%x)",
                          modeChosen->name,
                          modeEnterLat,
                          ULOG64_DATA(islandHardDuration),
                          input->island_latency_budget);

          /* Can not choose LPI due to island restrictions */
          continue;
        }

        /* If we made it this far, then we found the right mode.
         * Record its info, fill output data and return. */
        output->selected_synthLPRM = modeChosen;
        output->threshold_deadline = 
          sleepLPISolver_setThresholdDeadline(lookupElement->duration_thresh, 
                                              modeEnterLat,
                                              MIN(input->hard_deadlines.normal_min, normalSoftDeadline),
                                              input->hard_deadlines.island_timer.value,
                                              &output->ref_deadline);

        sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_ATS, 
                            SLEEP_SOLVER_MODE_CHOSEN_NUM_ARGS,
                            SLEEP_SOLVER_MODE_CHOSEN_STR, 
                            SLEEP_SOLVER_MODE_CHOSEN,
                            modeChosen->name,
                            ULOG64_DATA(output->ref_deadline),
                            ULOG64_DATA(output->threshold_deadline));

        /* Prepare the data to return. */
        output->backoff_ptr     = NULL;
        output->enter_exit_time = modeEnterExitLat;
        return SOLVER_SUCCESS;
      }
    }
  }
  else
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, " LPI Registry Disabled");
    return SOLVER_REGISTRY_DISABLED;
  }

  return SOLVER_NO_MODE_CHOSEN;
}

#endif /* USLEEP_ISLAND_MODE_ENABLE */
