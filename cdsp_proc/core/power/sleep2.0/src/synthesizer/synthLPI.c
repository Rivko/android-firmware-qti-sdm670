/*==============================================================================
  FILE:         synthLPI.c
 
  OVERVIEW:     Provides the synth LPI mode functions.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/synthesizer/synthLPI.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "synthRegistry.h"
#include "synthTypes.h"
#include "synthLPI.h"
#include "synthLPRM.h"
#include "lookup_table_types.h"
#include "SleepLPI_synth_modes.h"

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
                            FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * synthLPI_getStatus
 */
sleep_status synthLPI_getStatus(sleep_synth_lpi *synthLPI)
{
  if((NULL != synthLPI) && (synthLPI->enabled_mode_count > 0))
  {
    return SLEEP_ENABLED;
  }
  
  return SLEEP_DISABLED;
}

/*
 * synthLPI_define
 */
void synthLPI_define(sleep_synth_lpi *synthLPI)
{
  synthRegistry_registerSynthLPI(synthLPI);
  return;
}

/*
 * synthLPI_init
 */
void synthLPI_init(sleep_synth_lpi *synthLPI)
{
  uint32            i;
  sleep_synth_lprm  *synthLPRM;
  
  CORE_VERIFY(synthLPI->mode_count == SLEEP_LPI_NUM_SYNTHESIZED_MODES);

  synthLPI->enabled_mode_count = 0;

  for(i = 0; i < SLEEP_LPI_NUM_SYNTHESIZED_MODES; i++)
  {
    synthLPRM = &(synthLPI->modes[i]);
    synthLPRM_initSynth(synthLPRM);
  }

  return;
}

/*
 * synthLPI_modeUpdated
 */
void synthLPI_modeUpdated(sleep_synth_lprm* synthLPRM)
{
  sleep_synth_lpr *parent;

  CORE_VERIFY_PTR( synthLPRM );

  parent = synthLPRM->synth_LPR_parent;

  CORE_VERIFY_PTR( parent );

  if(SLEEP_ENABLED == synthLPRM->mode_status)
  {
    parent->enabled_mode_count++;
  }
  else
  {
    parent->enabled_mode_count--;
  }

  /* Confirm that the number of enabled modes did not go negative, or
   * exceed the total number of modes. */
  CORE_VERIFY( parent->enabled_mode_count <= SLEEP_LPI_NUM_SYNTHESIZED_MODES );
  
  synthRegistry_LPRUpdated(parent);

  return;
}

/*
 * synthLPI_updateLatency
 */
#if 0
void synthLPI_updateLatency(int32 enterAdjustment, int32 exitAdjustment)
{
  uint32              nFreq;
  uint32              exitLat;
  int32               adjustedLat;
  sleep_lpiLut_elem   *lutElement;
  LUT_mode_elem_u     *mode;
  synth_seed_latency  *seedLatency;

  /* Current assumption is only 1 LPI exists */
  CORE_VERIFY(1 == SLEEP_NUM_TOTAL_LPIS);

  /* All frequencies are updated with the same offset */
  for(nFreq=0; nFreq < SLEEP_NUM_STATIC_FREQ; nFreq++)
  {
    lutElement = SleepLPI_power_lookup_table[nFreq]->element;
    mode       = &lutElement->mode[0];

    CORE_VERIFY_PTR(mode->mode_ptr);

    /* Update the seed latencies */
    seedLatency = mode->mode_ptr->seed_latency;

    /* Ensure the adjustments are within reason:
     * 1. Latency is not negative
     * 2. Adjustment is not more than x times current value */

    /* Get the modes exit latency and verify the adjustment */
    exitLat     = seedLatency->enter_exit[nFreq] - seedLatency->enter[nFreq];
    adjustedLat = (int32)exitLat + exitAdjustment;
    CORE_LOG_VERIFY((adjustedLat > 0) && ((exitLat * 8) > adjustedLat),
                    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 2,
                                    "LPI Adjust fail: Exit adjustment too large "
                                    "(adjustedLat: %d) (exitLat: %d)",
                                    adjustedLat, exitLat));
    
    /* Verify the enter latency adjustment */
    adjustedLat = (int32)seedLatency->enter[nFreq] + enterAdjustment;
    CORE_LOG_VERIFY((adjustedLat > 0) && ((seedLatency->enter[nFreq] * 8) > adjustedLat),
                    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 2,
                                    "LPI Adjust fail: Enter adjustment too large "
                                    "(adjustedLat: %d) (enterLat: %d)",
                                    adjustedLat, seedLatency->enter[nFreq]));
                    

    /* Initial verification passed, update the values */
    seedLatency->enter[nFreq]       += enterAdjustment;
    seedLatency->enter_exit[nFreq]  += enterAdjustment + exitAdjustment;

    /* Final verification check */
    CORE_LOG_VERIFY(seedLatency->enter[nFreq] < seedLatency->enter_exit[nFreq],
                    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0, 
                                    "LPI Adjust fail: Enter exceeds total"));
                    
    
    /* Update the LUT duration threshold */
    lutElement->duration_thresh += enterAdjustment + exitAdjustment;
    CORE_VERIFY(lutElement->duration_thresh >= seedLatency->enter_exit[nFreq]);
  }

  return;
}
#endif

/*
 * synthLPI_getCurrentLatency
 */
void synthLPI_getCurrentLatency(uint32 *enterLat, uint32 *exitLat, uint32 freqIdx)
{
  sleep_lpiLut_elem *lutElement;
  LUT_mode_elem_u   *mode;

  /* Current assumption is only 1 LPI exists */
  CORE_VERIFY(1 == SLEEP_LPI_NUM_SYNTHESIZED_MODES);
  CORE_VERIFY(freqIdx < SLEEP_NUM_STATIC_FREQ);

  /* Get the only LPI mode
   * Frequency index doesn't matter here and 1 table will always exist */
  lutElement = SleepLPI_power_lookup_table[0]->element;
  mode       = &lutElement->mode[0];

  CORE_VERIFY_PTR(mode->mode_ptr);

  /* Get the seed latencies */
  *enterLat = mode->mode_ptr->seed_latency->enter[freqIdx];
  *exitLat  = mode->mode_ptr->seed_latency->enter_exit[freqIdx] - *enterLat;

  return;
}

