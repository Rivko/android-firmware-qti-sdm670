/*==============================================================================
  FILE:         rsc_cmn.c

  OVERVIEW:     This file implements some of the APIs defined in rsc public 
                header file to interact with RSC SEQUENCER HW block(s) in
                island code sections

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/rsc/common/rsc_cmn.c#2 $
$DateTime: 2017/11/16 08:24:49 $
==============================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"
#include "rsc_internal.h"

/*=======================================================================
 *                            INTERNAL FUNCTIONS
 *======================================================================*/
/** 
 * rsc_tigger_sw_seq
 * 
 * @brief Software triggers the RSC sequence that has already been setup
 * 
 * @param type: RSC type to trigger
 */ 
static void rsc_tigger_sw_seq(rsc_type type)
{
  uint32 base;

  base = g_rscs[type].addr;

  /* Follow the steps to trigger the sequence:
   *  - Write 0x0
   *  - Write 0x1 */
  rsc_hal_set_trigger_seq(base, 0x0);
  rsc_hal_set_trigger_seq(base, 0x1);
}

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/*
 * rsc_trigger_seq
 */
rsc_result_t rsc_trigger_seq(rsc_type type, rsc_sw_trig_mode mode_id)
{
  uint32        modeIndex;
  uint32        base;
  rsc_result_t  result =  RSC_INVALID_PARAM;
  
  if(!RSC_VALID_TYPE(type))
  {
    return result;
  }

  base = g_rscs[type].addr;

  /* Since this is a SW trigger, add the trigger type to the mode given */
  mode_id += RSC_SW_TRIGGER_FLAG_LPM;

  RSC_SEQ_LOCK(type);

  /* Iterate through the list and find the mode */
  result = rsc_mode_index(type, mode_id, &modeIndex);

  if(RSC_SUCCESS == result)
  {
    /* Write the trigger register with start address of the seqeunce */
    rsc_hal_set_trigger_start_address(base, g_rscs[type].modes[modeIndex].start_addr);

    /* Trigger sequencer */
    rsc_tigger_sw_seq(type);

    /* Wait until sequencer is idle
     * 1 = Seqeuncer busy
     * 0 = Idle */
    while (0 != rsc_hal_seq_busy(base)) {};
  }

  RSC_SEQ_UNLOCK(type);

  return result;
}

/*
 * rsc_mode_index
 */
rsc_result_t rsc_mode_index(rsc_type type, uint8 lpm_id, uint32 *index)
{
  rsc_t *mode = &g_rscs[type];

  /* Iterate through static modes */
  for(*index = 0; *index < mode->mode_count; (*index)++)
  {
    if(mode->modes[*index].mode_id == lpm_id)
    {
      return RSC_SUCCESS;
    }
  }

  return RSC_INVALID_MODE_INDEX;
}

