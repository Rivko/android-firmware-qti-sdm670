/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/HalLpassHdmiOutputFuncWrapper.cpp#2 $
$DateTime: 2017/11/23 06:25:06 $
$Author: pwbldsvc $
$Change: 14912369 $
$Revision: #2 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/HalLpassHdmiOutputFuncWrapper.cpp#2 $ $DateTime: 2017/11/23 06:25:06 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2014   FZ   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "AFEHalLpassHdmiOutput.h"
#include "common.h"
#include "HalLpassHdmiOutputFuncDefs.h"

/*=====================================================================
 Globals/Macros
 ======================================================================*/

static hal_hdmi_output_func_def_t  hal_hdmi_output_func_def;

/*=====================================================================
 Function definitions
 ======================================================================*/

//common functions for both single stream and multiple stream cases

void hal_lpass_hdmi_output_init(uint32 version, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base)
{
   memset(&hal_hdmi_output_func_def, 0, sizeof(hal_hdmi_output_func_def_t));

   switch (version)
   {
      case LPASS_HW_VER_4_0_0:
      {
         hal_lpass_hdmi_output_init_v1(&hal_hdmi_output_func_def,
                                       phy_base, virt_base,
                                       reset_phy_base, reset_virt_base);
         break;
      }
      case LPASS_HW_VER_4_0_2:
      case LPASS_HW_VER_4_1_0:
      case LPASS_HW_VER_4_2_0:
      {
         hal_lpass_hdmi_output_init_v2(&hal_hdmi_output_func_def,
                                       phy_base, virt_base,
                                       reset_phy_base, reset_virt_base);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unknown LPASS h/w version: 0x%x", version);
         break;
      }
   }

   return;
}


void hal_lpass_hdmi_output_enable_dma_interrupt(uint16 dma_idx, uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_dma_interrupt_fptr(dma_idx, enable);
}

void hal_lpass_hdmi_output_enable_packet_update_interrupt(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_packet_update_interrupt_fptr(enable);
}

void hal_lpass_hdmi_output_enable_packet_update_b_interrupt(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_packet_update_b_interrupt_fptr(enable);
}

void hal_lpass_hdmi_outputt_read_interrupt_status(uint32_t *int_status)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_outputt_read_interrupt_status_fptr(int_status);
}
void hal_lpass_hdmi_outputt_clear_interrupts(uint32_t clear_mask)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_outputt_clear_interrupts_fptr(clear_mask);
}
uint32 hal_lpass_hdmi_output_check_dma_intr(uint16 dma_idx, uint32 int_status)
{
   return hal_hdmi_output_func_def.hal_lpass_hdmi_output_check_dma_intr_fptr(dma_idx, int_status);
}
void hal_lpass_hdmi_output_get_dma_int_stc(uint16 dma_idx, uint64 *stc_ptr)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_get_dma_int_stc_fptr(dma_idx, stc_ptr);
}
void hal_lpass_hdmi_output_get_dma_fifo(uint16 dma_idx, uint32_t *fifo_count_ptr)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_get_dma_fifo_fptr(dma_idx, fifo_count_ptr);
}
void hal_lpass_hdmi_output_get_samples_in_dword_after_intr(uint16 dma_idx, uint32_t *sample_in_dwrod_ptr)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_get_samples_in_dword_after_intr_fptr(dma_idx, sample_in_dwrod_ptr);
}

void hal_lpass_hdmi_output_enable_legacy_mode(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_legacy_mode_fptr(enable);
}

void hal_lpass_hdmi_output_enable_parity_hw_mode(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_parity_hw_mode_fptr(enable);
}

void hal_lpass_hdmi_output_cfg_ch_status_buf(hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_cfg_ch_status_buf_fptr(chs_cfg);
}

void hal_lpass_hdmi_output_cfg_user_bits_buf(hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_cfg_user_bits_buf_fptr(ubs_cfg);
}

void hal_lpass_hdmi_output_enable_vbit_hw_mode(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx, uint16 vbit)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_enable_vbit_hw_mode_fptr(s_mode, dma_idx, vbit);
}
void hal_lpass_hdmi_output_disable_vbit_hw_mode(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_disable_vbit_hw_mode_fptr(s_mode, dma_idx);
}

//Single stream ONLY functions
void hal_lpass_hdmi_output_ss_cfg_pack_trans(hal_lpass_hdmi_output_ss_pack_trans_cfg_t ss_cfg)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ss_cfg_pack_trans_fptr(ss_cfg);
}
void hal_lpass_hdmi_output_ss_enable_pack_trans(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ss_enable_pack_trans_fptr(enable);
}


//multiple stream ONLY functions

void hal_lpass_hdmi_output_ms_cfg_pack_trans(hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ms_cfg_pack_trans_fptr(ms_cfg);
}

void hal_lpass_hdmi_output_ms_enable_pack_trans(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ms_enable_pack_trans_fptr(enable);
}

void hal_lpass_hdmi_output_ms_mute_stream(uint16 stream_id, uint16 mute)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ms_mute_stream_fptr(stream_id, mute);
}


void hal_lpass_hdmi_output_ms_wait_bbit(uint16 stream_id, uint16 wait)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_ms_wait_bbit_fptr(stream_id, wait);
}

//display port ONLY functions
void hal_lpass_hdmi_output_dp_mute_stream(uint16 stream_id, uint16 mute)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_dp_mute_stream_fptr(stream_id, mute);
}

void hal_lpass_hdmi_output_dp_cfg_audio_info(hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_dp_cfg_audio_info_fptr(audioinfo_cfg);
}
void hal_lpass_hdmi_output_dp_engine_enable(uint16 enable)
{
   hal_hdmi_output_func_def.hal_lpass_hdmi_output_dp_engine_enable_fptr(enable);
}
