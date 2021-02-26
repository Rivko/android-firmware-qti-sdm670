/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/HalLpassHdmiOutputFuncDefs.h#1 $
 ====================================================================== */
#ifndef _HAL_LPASS_HDMI_OUTPUT_FUNC_DEFS_H
#define _HAL_LPASS_HDMI_OUTPUT_FUNC_DEFS_H

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEHalLpassHdmiOutput.h"


#if defined(__cplusplus)
extern "C" {
#endif

/*==========================================================================
  Macro Defines
  ========================================================================== */


/*==========================================================================
  Function Declerations/Inlines
  ========================================================================== */

//common functions for both single stream and multiple stream cases

typedef void (*hal_lpass_hdmi_output_enable_dma_interrupt_func_t)(uint16 dma_idx, uint16 enable);   //1 - enable, 0 - disable
typedef void (*hal_lpass_hdmi_output_enable_packet_update_interrupt_func_t)(uint16 enable);   //1 - enable, 0 - disable
typedef void (*hal_lpass_hdmi_output_enable_packet_update_b_interrupt_func_t)(uint16 enable);   //1 - enable, 0 - disable
typedef void (*hal_lpass_hdmi_outputt_read_interrupt_status_func_t)(uint32_t *int_status);
typedef void (*hal_lpass_hdmi_outputt_clear_interrupts_func_t)(uint32_t clear_mask);
typedef uint32 (*hal_lpass_hdmi_output_check_dma_intr_func_t)(uint16 dma_idx, uint32 int_status);
typedef void (*hal_lpass_hdmi_output_get_dma_int_stc_func_t)(uint16 dma_idx, uint64 *stc_ptr);
typedef void (*hal_lpass_hdmi_output_get_dma_fifo_func_t)(uint16 dma_idx, uint32_t *fifo_count_ptr);
typedef void (*hal_lpass_hdmi_output_get_samples_in_dword_after_intr_func_t)(uint16 dma_idx, uint32_t *sample_in_dwrod_ptr);

typedef void (*hal_lpass_hdmi_output_enable_legacy_mode_func_t)(uint16 enable);   //0 - diable, 1 - enable

typedef void (*hal_lpass_hdmi_output_enable_parity_hw_mode_func_t)(uint16 enable);   //0 - diable, 1 - enable

typedef void (*hal_lpass_hdmi_output_cfg_ch_status_buf_func_t)(hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg);
typedef void (*hal_lpass_hdmi_output_cfg_user_bits_buf_func_t)(hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg);

typedef void (*hal_lpass_hdmi_output_enable_vbit_hw_mode_func_t)(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx, uint16 vbit);
typedef void (*hal_lpass_hdmi_output_disable_vbit_hw_mode_func_t)(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx);

//Single stream ONLY functions
typedef void (*hal_lpass_hdmi_output_ss_cfg_pack_trans_func_t)(hal_lpass_hdmi_output_ss_pack_trans_cfg_t ss_cfg);
typedef void (*hal_lpass_hdmi_output_ss_enable_pack_trans_func_t)(uint16 enable);   //1 - enable, 0 - disable


//multiple stream ONLY functions

typedef void (*hal_lpass_hdmi_output_ms_cfg_pack_trans_func_t)(hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg);

typedef void (*hal_lpass_hdmi_output_ms_enable_pack_trans_func_t)(uint16 enable);   //1 - enable, 0 - disable

typedef void (*hal_lpass_hdmi_output_ms_mute_stream_func_t)(uint16 stream_id, uint16 mute);   //1 - mute, 0 - unmute


typedef void (*hal_lpass_hdmi_output_ms_wait_bbit_func_t)(uint16 stream_id, uint16 wait);    //1 - wait, 0 - unwait

//display port ONLY functions
typedef void (*hal_lpass_hdmi_output_dp_mute_stream_func_t)(uint16 stream_id, uint16 mute);   //1 - mute, 0 - unmute
typedef void (*hal_lpass_hdmi_output_dp_cfg_audio_info_func_t)(hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg);
typedef void (*hal_lpass_hdmi_output_dp_engine_enable_func_t)(uint16 enable);


typedef struct hal_hdmi_output_func_def
{
   hal_lpass_hdmi_output_enable_dma_interrupt_func_t               hal_lpass_hdmi_output_enable_dma_interrupt_fptr;
   hal_lpass_hdmi_output_enable_packet_update_interrupt_func_t     hal_lpass_hdmi_output_enable_packet_update_interrupt_fptr;
   hal_lpass_hdmi_output_enable_packet_update_b_interrupt_func_t   hal_lpass_hdmi_output_enable_packet_update_b_interrupt_fptr;
   hal_lpass_hdmi_outputt_read_interrupt_status_func_t             hal_lpass_hdmi_outputt_read_interrupt_status_fptr;
   hal_lpass_hdmi_outputt_clear_interrupts_func_t                  hal_lpass_hdmi_outputt_clear_interrupts_fptr;
   hal_lpass_hdmi_output_check_dma_intr_func_t                     hal_lpass_hdmi_output_check_dma_intr_fptr;
   hal_lpass_hdmi_output_get_dma_int_stc_func_t                    hal_lpass_hdmi_output_get_dma_int_stc_fptr;
   hal_lpass_hdmi_output_get_dma_fifo_func_t                       hal_lpass_hdmi_output_get_dma_fifo_fptr;
   hal_lpass_hdmi_output_get_samples_in_dword_after_intr_func_t    hal_lpass_hdmi_output_get_samples_in_dword_after_intr_fptr;
   hal_lpass_hdmi_output_enable_legacy_mode_func_t                 hal_lpass_hdmi_output_enable_legacy_mode_fptr;   
   hal_lpass_hdmi_output_enable_parity_hw_mode_func_t              hal_lpass_hdmi_output_enable_parity_hw_mode_fptr;   
   hal_lpass_hdmi_output_cfg_ch_status_buf_func_t                  hal_lpass_hdmi_output_cfg_ch_status_buf_fptr;
   hal_lpass_hdmi_output_cfg_user_bits_buf_func_t                  hal_lpass_hdmi_output_cfg_user_bits_buf_fptr;
   hal_lpass_hdmi_output_enable_vbit_hw_mode_func_t                hal_lpass_hdmi_output_enable_vbit_hw_mode_fptr;
   hal_lpass_hdmi_output_disable_vbit_hw_mode_func_t               hal_lpass_hdmi_output_disable_vbit_hw_mode_fptr;

   //Single stream ONLY functions
   hal_lpass_hdmi_output_ss_cfg_pack_trans_func_t                  hal_lpass_hdmi_output_ss_cfg_pack_trans_fptr;
   hal_lpass_hdmi_output_ss_enable_pack_trans_func_t               hal_lpass_hdmi_output_ss_enable_pack_trans_fptr;   

   //multiple stream ONLY functions
   hal_lpass_hdmi_output_ms_cfg_pack_trans_func_t                  hal_lpass_hdmi_output_ms_cfg_pack_trans_fptr;
   hal_lpass_hdmi_output_ms_enable_pack_trans_func_t               hal_lpass_hdmi_output_ms_enable_pack_trans_fptr;   
   hal_lpass_hdmi_output_ms_mute_stream_func_t                     hal_lpass_hdmi_output_ms_mute_stream_fptr;  
   hal_lpass_hdmi_output_ms_wait_bbit_func_t                       hal_lpass_hdmi_output_ms_wait_bbit_fptr;    

   //display port ONLY functions
   hal_lpass_hdmi_output_dp_mute_stream_func_t                     hal_lpass_hdmi_output_dp_mute_stream_fptr;
   hal_lpass_hdmi_output_dp_cfg_audio_info_func_t                  hal_lpass_hdmi_output_dp_cfg_audio_info_fptr;
   hal_lpass_hdmi_output_dp_engine_enable_func_t                   hal_lpass_hdmi_output_dp_engine_enable_fptr;

} hal_hdmi_output_func_def_t; 


void hal_lpass_hdmi_output_init_v1(hal_hdmi_output_func_def_t* func_table_ptr, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base);

void hal_lpass_hdmi_output_init_v2(hal_hdmi_output_func_def_t* func_table_ptr, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif  //_HAL_LPASS_HDMI_OUTPUT_FUNC_DEFS_H

