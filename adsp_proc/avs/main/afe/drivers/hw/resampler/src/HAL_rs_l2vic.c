/*
============================================================================

FILE:          HAL_rs.c

DESCRIPTION:   Resampler HW driver HAL file.

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

#include "HAL_rs_l2vic.h"


/************************************************
 *            HAL    Functions                  *
 ************************************************/

/**
 * Set L2VIC for HW resampler
 */
void HAL_rs_l2vic_init(uint32 base_virt_addr)
{
   uint32_t l2vic_reg_val;

   //disable int
   reg_wr(LPASS_QDSP6SS_L2VIC_INT_ENABLE_CLEAR1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, (1<<Q6SS_INT_SLICE_RESAMPLER_L2_INT_NUMBER));

   reg_rd(LPASS_QDSP6SS_L2VIC_INT_TYPE1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, &l2vic_reg_val);
   //set interrupts to level sensitive
   l2vic_reg_val = ((l2vic_reg_val) & (~(1<<Q6SS_INT_SLICE_RESAMPLER_L2_INT_NUMBER)));

   reg_wr(LPASS_QDSP6SS_L2VIC_INT_TYPE1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, l2vic_reg_val);
   reg_wr(LPASS_QDSP6SS_L2VIC_INT_CLEAR1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, (1<<Q6SS_INT_SLICE_RESAMPLER_L2_INT_NUMBER)); 
   reg_rd(LPASS_QDSP6SS_L2VIC_INT_STATUS1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, &l2vic_reg_val);
   reg_wr(LPASS_QDSP6SS_L2VIC_INT_ENABLE_SET1 + base_virt_addr - LPASS_QDSP6SS_L2VIC_VID, (1 << Q6SS_INT_SLICE_RESAMPLER_L2_INT_NUMBER)); 
}
 



