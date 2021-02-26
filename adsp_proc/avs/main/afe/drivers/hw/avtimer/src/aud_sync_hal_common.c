/** @file aud_sync_hal.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History


when       who     what, where, why
------     ---    -------------------------------------------------------
05/31/11    rb     created the document

========================================================================== */
#include "aud_sync_hal.h"
#include "aud_sync_hal_v2.h"

#include "qurt_elite.h"

static uint32   avsync_hw_version = 0;

avsync_hal_func_def_t* avsync_hal_func_def_ptr_g = NULL;

/** 
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 * 
 * @param[in]  base_ptr Base virtual address of the audio hardware registes.
 * @param[out] version Version of the implementation of Audio HAL.
 */
void hal_aud_sync_init(avtimer_hw_cfg_t *avt_hw_cfg)
{
   avsync_hw_version = avt_hw_cfg->hw_revision;
   avsync_hal_func_def_ptr_g = (avsync_hal_func_def_t*)avt_hw_cfg->avsync_hal_func_def_ptr;

   switch(avsync_hw_version)
   {
      case AVTIMER_HW_VERSION_2:
      default:
      {
         hal_aud_sync_init_v2(avt_hw_cfg);

         avsync_hal_func_def_ptr_g->hal_aud_sync_reset_all_f = hal_aud_sync_reset_all_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_enable_clocks_f = hal_aud_sync_enable_clocks_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_disable_clocks_f = hal_aud_sync_disable_clocks_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_set_vfr_source_f = hal_aud_sync_set_vfr_source_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_get_vfr_source_f = hal_aud_sync_get_vfr_source_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_adj_phase_f = hal_aud_sync_adj_phase_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_enable_tracking_f = hal_aud_sync_enable_tracking_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_disable_tracking_f = hal_aud_sync_disable_tracking_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_reset_tracking_f = hal_aud_sync_reset_tracking_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_unreset_tracking_f = hal_aud_sync_unreset_tracking_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_get_ovf_status_f = hal_aud_sync_get_ovf_status_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_clear_status_f = hal_aud_sync_clear_status_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_set_ref_cnt_inc_f = hal_aud_sync_set_ref_cnt_inc_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_set_xo_div_val_f = hal_aud_sync_set_xo_div_val_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_get_smp_cnt_f = hal_aud_sync_get_smp_cnt_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_get_ref_cnt_f = hal_aud_sync_get_ref_cnt_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_get_vfr_cnt_f = hal_aud_sync_get_vfr_cnt_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_set_smp_cnt_f = hal_aud_sync_set_smp_cnt_v2;
         avsync_hal_func_def_ptr_g->hal_aud_sync_set_ref_cnt_f = hal_aud_sync_set_ref_cnt_v2;
      }
      break;
   }
}

sampling_rate_t sample_rate_2_index(uint32_t sample_rate)
{
   switch(sample_rate)
   {
      case 8000:
         return EIGHT_K;
      case 16000:
         return SIXTEEN_K;
      case 48000:
         return FOURTY_EIGHT_K;
      default:
         //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sample rate index is INVALID SR: %d",(int) sample_rate );
         AUD_SYNC_ASSERT(0);
         return INVALID;
   }
}
