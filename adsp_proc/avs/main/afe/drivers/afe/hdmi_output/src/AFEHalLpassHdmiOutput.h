/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalLpassHdmiOutput.h#1 $
 ====================================================================== */
#ifndef _HAL_LPASS_HDMI_OUTPUT_I_H
#define _HAL_LPASS_HDMI_OUTPUT_I_H

/*==========================================================================
  Include files
  ========================================================================== */
#include "typedef_drivers.h" 
#include "lpasshwio_devcfg.h"

/*==========================================================================
  Macro Defines
  ========================================================================== */

static const uint32_t HAL_LPASS_HDMI_OUTPUT_WATERMARK             		 =  16;
static const uint32_t HAL_LPASS_HDMI_OUTPUT_BYTES_PER_SAMPLES     		 =  4;


/** Possible group device state. */
typedef enum
{
   HAL_LPASS_HDMI_OUTPUT_SINGLE_STREAM_MODE = 0,   
   HAL_LPASS_HDMI_OUTPUT_MULTIPLE_STREAM_MODE       
} hal_lpass_hdmi_output_stream_mode;


typedef struct
{
   uint16   dma_idx;        //0, 1, 2, 3
   uint16   chs_buf_idx;    //0, 1, 2, 3, 4, 5, 6, 7
   uint16   hw_mode;        //0 - SW mode, 1 - HW mode
   uint32   ch_sts_data0;
   uint32   ch_sts_data1;
} hal_lpass_hdmi_output_ch_status_cfg_t;

typedef struct
{
   uint16   dma_idx;         //0, 1, 2, 3
   uint16   user_buf_idx;    //0, 1, 2, 3
   uint16   hw_mode;        //0 - SW mode, 1 - HW mode
} hal_lpass_hdmi_output_user_bits_cfg_t;


typedef struct
{
   uint16   sp_en;                //not used now
   uint16   force_mute;           //not used now
   uint16   sp_bits_layout1;      //sample present bit for layout 1
   uint16   dma_idx;              //0, 1, 2, 3
} hal_lpass_hdmi_output_ss_pack_trans_cfg_t;

typedef struct
{
   uint16   dma_idx;             //0, 1, 2, 3
   uint16   stream_id;           //1, 2, 3, 4
} hal_lpass_hdmi_output_ms_pack_trans_cfg_t;

typedef struct
{
   uint16   dma_idx;        //0, 1, 2, 3 - not used now
   uint16   freq_idx;    
   uint16   cc_idx;        //0 - SW mode, 1 - HW mode
   uint16   ct_idx;
   uint16   ca_idx;
} hal_lpass_hdmi_output_dp_audioinfo_cfg_t;

/*==========================================================================
  Function Declerations/Inlines
  ========================================================================== */

//common functions for both single stream and multiple stream cases
void hal_lpass_hdmi_output_init(uint32 version, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base);

void hal_lpass_hdmi_output_enable_dma_interrupt(uint16 dma_idx, uint16 enable);   //1 - enable, 0 - disable
void hal_lpass_hdmi_output_enable_packet_update_interrupt(uint16 enable);   //1 - enable, 0 - disable
void hal_lpass_hdmi_output_enable_packet_update_b_interrupt(uint16 enable);   //1 - enable, 0 - disable
void hal_lpass_hdmi_outputt_read_interrupt_status(uint32_t *int_status);
void hal_lpass_hdmi_outputt_clear_interrupts(uint32_t clear_mask);
uint32 hal_lpass_hdmi_output_check_dma_intr(uint16 dma_idx, uint32 int_status);
void hal_lpass_hdmi_output_get_dma_int_stc(uint16 dma_idx, uint64 *stc_ptr);
void hal_lpass_hdmi_output_get_dma_fifo(uint16 dma_idx, uint32_t *fifo_count_ptr);
void hal_lpass_hdmi_output_get_samples_in_dword_after_intr(uint16 dma_idx, uint32_t *sample_in_dwrod_ptr);



void hal_lpass_hdmi_output_enable_legacy_mode(uint16 enable);   //0 - diable, 1 - enable

void hal_lpass_hdmi_output_enable_parity_hw_mode(uint16 enable);   //0 - diable, 1 - enable

void hal_lpass_hdmi_output_cfg_ch_status_buf(hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg);
void hal_lpass_hdmi_output_cfg_user_bits_buf(hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg);

void hal_lpass_hdmi_output_enable_vbit_hw_mode(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx, uint16 vbit);
void hal_lpass_hdmi_output_disable_vbit_hw_mode(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx);

//Single stream ONLY functions
void hal_lpass_hdmi_output_ss_cfg_pack_trans(hal_lpass_hdmi_output_ss_pack_trans_cfg_t ss_cfg);
void hal_lpass_hdmi_output_ss_enable_pack_trans(uint16 enable);   //1 - enable, 0 - disable


//multiple stream ONLY functions

void hal_lpass_hdmi_output_ms_cfg_pack_trans(hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg);

void hal_lpass_hdmi_output_ms_enable_pack_trans(uint16 enable);   //1 - enable, 0 - disable

void hal_lpass_hdmi_output_ms_mute_stream(uint16 stream_id, uint16 mute);   //1 - mute, 0 - unmute


void hal_lpass_hdmi_output_ms_wait_bbit(uint16 stream_id, uint16 wait);    //1 - wait, 0 - unwait

//display port ONLY functions
void hal_lpass_hdmi_output_dp_mute_stream(uint16 stream_id, uint16 mute);   //1 - mute, 0 - unmute

void hal_lpass_hdmi_output_dp_cfg_audio_info(hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg);
void hal_lpass_hdmi_output_dp_engine_enable(uint16 enable);   //1 - enable, 0 - disable

#endif  //_HAL_LPASS_HDMI_OUTPUT_I_H

