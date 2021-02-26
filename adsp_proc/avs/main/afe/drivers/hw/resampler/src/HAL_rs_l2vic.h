/*
============================================================================

FILE:          HAL_rs_l2vic.h

DESCRIPTION:   Resampler HW driver HAL header file.

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary	
============================================================================

============================================================================
  EDIT HISTORY FOR MODULE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2011-03-21   qm   Initial version.
============================================================================
*/
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef HAL_RS_L2VIC_H
#define HAL_RS_L2VIC_H

#include "qurt_elite.h"
#include "common.h" //reg_wr reg_rd
#include "LPASS_ADDRESS_FILE.h"


/**
 * L2 interrupt number for resampler
*/
#define Q6SS_RESAMPLER_L2_INT_NUMBER (53)  //what number to use?

/**
 * Number of L2 interrupts per slice
*/
#define Q6SS_L2_INTS_PER_SLICE  (32)          

/**
 * Slice number of RESAMPLER interrupts
*/
#define Q6SS_RESAMPLER_SLICE_NUMBER  (1)          
#define Q6SS_INT_SLICE_RESAMPLER_L2_INT_NUMBER (Q6SS_RESAMPLER_L2_INT_NUMBER -(Q6SS_L2_INTS_PER_SLICE*Q6SS_RESAMPLER_SLICE_NUMBER))


void HAL_rs_l2vic_init(uint32 base_virt_addr);

#endif

#ifdef __cplusplus
}
#endif //__cplusplus


