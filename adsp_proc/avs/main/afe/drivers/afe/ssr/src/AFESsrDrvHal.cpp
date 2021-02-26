/*========================================================================
   This file contains AFE SSR handler implementation

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: 
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */

#include "AFESsrDrv_i.h"
#include "afe_lpm.h"
#include "AFEDmaManager.h"


/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

static inline void afe_ssr_reset_lpaif_dma()
{
   afe_ssr_lpaif_info_t    *p_lpaif_info;

   p_lpaif_info = &afe_ssr_info_global.lpaif_info;

   /* Check the LPAIF core power status */
   if(AFE_SSR_LPASS_CORE_ON == p_lpaif_info->core_pwr_status)
   {
      /* Set the status to OFF, as if any error happens inside error handler,
         don't do these steps again */
      p_lpaif_info->core_pwr_status = AFE_SSR_LPASS_CORE_OFF;

      /* First reset the DMA to avoid beep noise during SSR */
      afe_dma_mgr_reset_dma(LPASS_HW_DMA_TYPE0);
   }

   return;
}


#ifdef AFE_SSR_DUMP_LPM_MEM
static inline void afe_ssr_dump_lpm_mem()
{
   afe_ssr_lpm_info_t      *p_lpm_info;

   p_lpm_info = &afe_ssr_info_global.lpm_info;

   /* Check the LPM core power status */
   if(AFE_SSR_LPASS_CORE_ON == p_lpm_info->lpm_pwr_status)
   {
      /* Set the status to OFF, as if any error happens inside
         error handler, don't do these steps again */
      p_lpm_info->lpm_pwr_status = AFE_SSR_LPASS_CORE_OFF;

      afe_ssr_lpm_dump(afe_ssr_info_global.lpm_buf_ptr, p_lpm_info->lpm_virt_addr, p_lpm_info->lpm_size);

   }
   return;
}
#endif //AFE_SSR_DUMP_LPM_MEM

void afe_ssr_handler_cb(void)
{
   /* Check the guard words for SSR info and LPM buffer */
   if((AFE_SSR_GUARD_WORD_HEAD != afe_ssr_info_global.ssr_info_guard_head) ||
      (AFE_SSR_GUARD_WORD_TAIL != afe_ssr_info_global.ssr_info_guard_tail) ||
      (AFE_SSR_GUARD_WORD_HEAD != afe_ssr_info_global.lpm_buf_guard_head) ||
      (AFE_SSR_GUARD_WORD_TAIL != afe_ssr_info_global.lpm_buf_guard_tail))
   {
      return;
   }

   /* Reset the LPAIF DMA */
   afe_ssr_reset_lpaif_dma();

   /* Capture LPM content in global buffer */
#ifdef AFE_SSR_DUMP_LPM_MEM
   afe_ssr_dump_lpm_mem();
#endif //AFE_SSR_DUMP_LPM_MEM

   return;
}
 
