/*==============================================================================
  FILE:         synthLPR.c
 
  OVERVIEW:     Provides the synth mode functions.
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/synthesizer/synthLPR.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "sleepi.h"
#include "synthRegistry.h"
#include "synthTypes.h"
#include "synthLPR.h"
#include "synthLPRM.h"
#include "CoreVerify.h"
#include "sleep_log.h"

/*==============================================================================
                            FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * synthLPR_init
 */
void synthLPR_init(sleep_synth_lpr *synthLPR)
{
  uint32            i;
  sleep_synth_lprm  *synthLPRM;
  
  CORE_VERIFY(synthLPR->mode_count == 
              SLEEP_LPR_NUM_SYNTHESIZED_MODES );

  synthLPR->enabled_mode_count = 0;

  for(i = 0; i < SLEEP_LPR_NUM_SYNTHESIZED_MODES; i++)
  {
    synthLPRM = &(synthLPR->modes[i]);

    /* Ensure that a synthLPR does not have the LPI attribute */
    CORE_LOG_VERIFY(0 == (synthLPRM->attr & SYNTH_MODE_ATTR_LPI),
                    sleepLog_printf(SLEEP_LOG_LEVEL_ERROR,
                                    0, "ERROR (message: \"LPR has bad attribute\")"));

    synthLPRM_initSynth(synthLPRM);
  }

  return;
}

/*
 * synthLPR_getStatus
 */
sleep_status synthLPR_getStatus(sleep_synth_lpr *synthLPR)
{
  if((NULL != synthLPR) && (synthLPR->enabled_mode_count > 0))
  {
    return SLEEP_ENABLED;
  }

  return SLEEP_DISABLED;
}

/*
 * synthLPR_modeUpdated
 */
void synthLPR_modeUpdated(sleep_synth_lprm* synthLPRM)
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
  CORE_VERIFY( parent->enabled_mode_count <= SLEEP_LPR_NUM_SYNTHESIZED_MODES );
  
  synthRegistry_LPRUpdated(parent);

  return;
}

/*
 * synthLPR_define
 */
void synthLPR_define(sleep_synth_lpr *synthLPR)
{
  synthRegistry_registerSynthLPR(synthLPR);
  return;
}
