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

#include "AFESsrDrv.h"
#include "AFESsrDrv_i.h"
#include "AFEDmaManager.h"

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
 
afe_ssr_info_t      afe_ssr_info_global;

#define AFE_DMA_POWER_ON    1

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
 
ADSPResult afe_ssr_handler_init()
{
   ADSPResult               result = ADSP_EFAILED;

#if !defined(SIM)
   afe_ssr_lpaif_info_t    *p_lpaif_info;
   afe_ssr_lpm_info_t      *p_lpm_info;
#endif

   /* Clear the global structure */
   memset(&afe_ssr_info_global, 0, sizeof(afe_ssr_info_t));


   /* Below ifdef can be removed once back end core BSP is updated to include below
      error service callback registration API */
#if !defined(SIM)
   /* Register callback with error service */
   if (TRUE == err_crash_cb_reg_next_to_STM((afe_ssr_handler_fp)afe_ssr_handler_cb))
   {
      p_lpaif_info = &afe_ssr_info_global.lpaif_info;

      p_lpm_info = &afe_ssr_info_global.lpm_info;

      /* Init the guard head and tail words for SSR info */
      afe_ssr_info_global.ssr_info_guard_head = AFE_SSR_GUARD_WORD_HEAD;
      afe_ssr_info_global.ssr_info_guard_tail = AFE_SSR_GUARD_WORD_TAIL;

      /* Init the guard head and tail words for LPM Buf info */
      afe_ssr_info_global.lpm_buf_guard_head = AFE_SSR_GUARD_WORD_HEAD;
      afe_ssr_info_global.lpm_buf_guard_tail = AFE_SSR_GUARD_WORD_TAIL;

      /* Set the LPAIF core status to power OFF */
      p_lpaif_info->core_pwr_status = AFE_SSR_LPASS_CORE_OFF;
      

      /* Set the LPM status to power OFF */
      p_lpm_info->lpm_pwr_status = AFE_SSR_LPASS_CORE_OFF;

      /* Store the LPM size and virt adddress in SSR info struct */
      p_lpm_info->lpm_size = qurt_elite_globalstate.lpm_size;
      p_lpm_info->lpm_virt_addr = qurt_elite_globalstate.lpm_virt_addr;
         
   /**< Base virtual address of LPM. */
#ifdef AFE_SSR_DUMP_LPM_MEM
      //After this mpss/lpass Q6 reboot sequence will execute including the re-init of ddr/lpm heap allocations. 
      //We don't need to free it explicitly
      // And this is also the reason that we dont' need the deinit code.
      afe_ssr_info_global.lpm_buf_ptr = (uint8_t *) qurt_elite_memory_malloc(p_lpm_info->lpm_size, QURT_ELITE_HEAP_DEFAULT);
#endif      

   }
   else /* Failed to register callback function */
   {
      result = ADSP_EFAILED;
   }
#endif /* !defined(SIM) */
   
      
   return result;
}

// This is called by DMA manager whenever dma is enabled or disabled...
void afe_ssr_update_lpass_dma_status(uint32_t dma_type, uint32_t dir, uint32_t dma_idx, bool_t pwr_status)
{

   if(LPASS_HW_DMA_TYPE0 == dma_type)
   {
      // use bit map: each status of dma can represent max 32 dma status for each direction.
      if(dir)
      {
         // if it is on, then set the bit of dma position to 1... if not, turn off that bit for write dma (TX)
         if(pwr_status)
         {
            afe_ssr_info_global.lpaif_info.wrdma_status |= (AFE_DMA_POWER_ON << dma_idx);
         }
         else
         {
            afe_ssr_info_global.lpaif_info.wrdma_status &= ~(AFE_DMA_POWER_ON << dma_idx);
         }
      }      
      else
      {
      
         // if it is on, then set the bit of dma position to 1... if not, turn off that bit for write dma (RX)
         if(pwr_status)
         {
            afe_ssr_info_global.lpaif_info.rddma_status |= (AFE_DMA_POWER_ON << dma_idx);
         }
         else
         {
            afe_ssr_info_global.lpaif_info.rddma_status &= ~(AFE_DMA_POWER_ON << dma_idx);
         }

      }

      //If any of dma is on, then the lpass core is on so that we mark it as lpass core on for core power status.
      if((0 != afe_ssr_info_global.lpaif_info.rddma_status) ||
         (0 != afe_ssr_info_global.lpaif_info.wrdma_status) )
      {
         afe_ssr_info_global.lpaif_info.core_pwr_status = AFE_SSR_LPASS_CORE_ON;
         afe_ssr_info_global.lpm_info.lpm_pwr_status = AFE_SSR_LPASS_CORE_ON;
         
      }
      else
      {
         afe_ssr_info_global.lpaif_info.core_pwr_status = AFE_SSR_LPASS_CORE_OFF;
         afe_ssr_info_global.lpm_info.lpm_pwr_status = AFE_SSR_LPASS_CORE_OFF;
      }
   }
   return;
}



