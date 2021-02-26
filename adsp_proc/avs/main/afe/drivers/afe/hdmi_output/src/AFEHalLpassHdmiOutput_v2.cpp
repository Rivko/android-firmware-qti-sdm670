/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalLpassHdmiOutput_v2.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalLpassHdmiOutput_v2.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

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
#include "LPASS_ADDRESS_FILE_HDMI_OUTPUT_v2.h"
#include "HalLpassHdmiOutputFuncDefs.h"

/*=====================================================================
 Globals/Macros
 ======================================================================*/

#define REG_OFFSET(_reg_) ((_reg_) - LPASS_LPASS_HDMITX_BASE)  


static uint32                  hdmi_tx_phy_base;
static int32                   hdmi_tx_virt_offset;

static uint32                  hdmi_tx_reset_phy_base;
static int32                   hdmi_tx_reset_virt_offset;

/*=====================================================================
 Function definitions
 ======================================================================*/

void hal_lpass_hdmi_output_enable_legacy_mode_v2(uint16 enable)   //0 - diable, 1 - enable
{
   //lpass HDMI hw cfg
   //HDMITX_LEGACY. LEGACY_EN = 1   --> use LPA_IF DMA
   //                         = 0   --> use new LPASS HDMITx hw
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_LEGACY__LEGACY_EN___M;
   fieldValue = (enable << LPASS_HDMITX_LEGACY__LEGACY_EN___S);

   UpdateRegister1(hdmi_tx_reset_phy_base + (LPASS_HDMITX_LEGACY - LPASS_HDMITX_CTL), hdmi_tx_reset_virt_offset, mask,fieldValue,0);
}


void hal_lpass_hdmi_output_enable_parity_hw_mode_v2(uint16 enable)   //0 - diable, 1 - enable
{
   //7.	HDMITX_PARITY                   -> for both single stream and multi-stream cases
   //    HDMITX_PARITY. CALC_EN = 1   --- hw calculate P bit
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_PARITY__CALC_EN___M;
   fieldValue = (enable << LPASS_HDMITX_PARITY__CALC_EN___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_PARITY), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_cfg_ch_status_buf_v2(hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg)
{
  //HDMITX_DMA0_CTRL RESET_VALUE 0x00000000
   //     HW_USER_SEL BIT[6:5]         Map to 4 user bit buffer via [0, 1, 2, 3]
   //     HW_CHS_SEL BIT[4:2]      ->  Map to 8 channel status buffers via [0, 1, 2, 3, 4, 5,6, 7]
   //     USE_HW_USER BIT[1]     ->  1 --- hw way, 0  --- sw way
   //     USE_HW_CHS BIT[0]       ->  1 --- hw way, 0  --- sw way
   //                     Set default for hw pack way

   //                     For single stream case, use ch_s_buf_0 and 1 only. Same for use bits. Not support dual mono now.

   uint16   dma_idx = chs_cfg.dma_idx;
   uint16   buf_idx = chs_cfg.chs_buf_idx;
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_DMA0_CTRL__USE_HW_CHS___M | LPASS_HDMITX_DMA0_CTRL__HW_CHS_SEL___M;
   fieldValue = (chs_cfg.hw_mode << LPASS_HDMITX_DMA0_CTRL__USE_HW_CHS___S) | (chs_cfg.chs_buf_idx << LPASS_HDMITX_DMA0_CTRL__HW_CHS_SEL___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_DMA0_CTRL + dma_idx * 4), hdmi_tx_virt_offset, mask,fieldValue,0);

   //set channel status values
   mask = LPASS_HDMITX_CHS0_LSB__LSB_BITS___M;
   fieldValue = (chs_cfg.ch_sts_data0 << LPASS_HDMITX_CHS0_LSB__LSB_BITS___S);
   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_CHS0_LSB + buf_idx * 4), hdmi_tx_virt_offset, mask,fieldValue,0);

   mask = LPASS_HDMITX_CHS0_MSB__MSB_BITS___M;
   fieldValue = (chs_cfg.ch_sts_data1 << LPASS_HDMITX_CHS0_MSB__MSB_BITS___S);
   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_CHS0_MSB + buf_idx * 4), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_cfg_user_bits_buf_v2(hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg)
{
  //HDMITX_DMA0_CTRL RESET_VALUE 0x00000000
   //     HW_USER_SEL BIT[6:5]         Map to 4 user bit buffer via [0, 1, 2, 3]
   //     HW_CHS_SEL BIT[4:2]      ->  Map to 8 channel status buffers via [0, 1, 2, 3, 4, 5,6, 7]
   //     USE_HW_USER BIT[1]     ->  1 --- hw way, 0  --- sw way
   //     USE_HW_CHS BIT[0]       ->  1 --- hw way, 0  --- sw way
   //                     Set default for hw pack way

   //                     For single stream case, use ch_s_buf_0 and 1 only. Same for use bits. Not support dual mono now.

   uint16   dma_idx = ubs_cfg.dma_idx;
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_DMA0_CTRL__USE_HW_USER___M | LPASS_HDMITX_DMA0_CTRL__HW_USER_SEL___M;
   fieldValue = (ubs_cfg.hw_mode << LPASS_HDMITX_DMA0_CTRL__USE_HW_USER___S) | (ubs_cfg.user_buf_idx << LPASS_HDMITX_DMA0_CTRL__HW_USER_SEL___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_DMA0_CTRL + dma_idx * 4), hdmi_tx_virt_offset, mask,fieldValue,0);

   //TBD - support user buffer cfg
   //LPASS_HDMITX_USER0_LSB, LPASS_HDMITX_USER0_MID, LPASS_HDMITX_USER0_MSB   --- for ubs_cfg.user_buf_idx = 0
   //LPASS_HDMITX_USER1_LSB, LPASS_HDMITX_USER1_MID, LPASS_HDMITX_USER1_MSB   --- for ubs_cfg.user_buf_idx = 1
   //LPASS_HDMITX_USER2_LSB, LPASS_HDMITX_USER2_MID, LPASS_HDMITX_USER2_MSB   --- for ubs_cfg.user_buf_idx = 2
   //LPASS_HDMITX_USER3_LSB, LPASS_HDMITX_USER3_MID, LPASS_HDMITX_USER3_MSB   --- for ubs_cfg.user_buf_idx = 3
}

void hal_lpass_hdmi_output_enable_vbit_hw_mode_v2(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx, uint16 vbit)
{
   uint32 mask,fieldValue;

   if(HAL_LPASS_HDMI_OUTPUT_SINGLE_STREAM_MODE == s_mode)
   {
      mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___M | LPASS_HDMITX_VBIT_CTRL__VBIT_SSTREAM___M;
      fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___S) | (vbit << LPASS_HDMITX_VBIT_CTRL__VBIT_SSTREAM___S);
   }
   else
   {
      switch(dma_idx)
      {
      case 0:
      default:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA0___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___S) | (vbit << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA0___S);
         break;
      case 1:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA1___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___S) | (vbit << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA1___S);
         break;
      case 2:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA2___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___S) | (vbit << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA2___S);
         break;
      case 3:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA3___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___S) | (vbit << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA3___S);
         break;
      }
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_VBIT_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_disable_vbit_hw_mode_v2(hal_lpass_hdmi_output_stream_mode s_mode, uint16 dma_idx)
{
   uint32 mask,fieldValue;

   if(HAL_LPASS_HDMI_OUTPUT_SINGLE_STREAM_MODE == s_mode)
   {
      //set to default
      mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___M | LPASS_HDMITX_VBIT_CTRL__VBIT_SSTREAM___M;
      fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___S) | (0 << LPASS_HDMITX_VBIT_CTRL__VBIT_SSTREAM___S);

      UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_VBIT_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);

      //disable able
      mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___M;
      fieldValue = (0 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT___S) ;
   }
   else
   {
      //set to default
      switch(dma_idx)
      {
      case 0:
      default:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA0___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___S) | (0 << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA0___S);
         break;
      case 1:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA1___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___S) | (0 << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA1___S);
         break;
      case 2:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA2___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___S) | (0 << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA2___S);
         break;
      case 3:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___M | LPASS_HDMITX_VBIT_CTRL__VBIT_DMA3___M;
         fieldValue = (1 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___S) | (0 << LPASS_HDMITX_VBIT_CTRL__VBIT_DMA3___S);
         break;
      }

      UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_VBIT_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);

      switch(dma_idx)
      {
      case 0:
      default:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___M;
         fieldValue = (0 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA0___S);
         break;
      case 1:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___M;
         fieldValue = (0 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA1___S);
         break;
      case 2:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___M;
         fieldValue = (0 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA2___S);
         break;
      case 3:
         mask = LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___M;
         fieldValue = (0 << LPASS_HDMITX_VBIT_CTRL__REPLACE_VBIT_DMA3___S);
         break;
      }
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_VBIT_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);

}

//
//what are REQON_PRELOAD_DMAx bits useed for? 
//this is the case in which there is a request to the DMA and the FIFO is still empty
//For example the DMA might be just reprogrammed and preloading new data
//it might happen because AHB is temporally slow
//maybe this is a debug feature
void hal_lpass_hdmi_output_enable_dma_interrupt_v2(uint16 dma_idx, uint16 enable)   //1 - enable, 0 - disable
{
   uint32 mask,fieldValue;

   switch(dma_idx)
   {
   case 0:
   default:
         
      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___M     | 
             LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___M    | 
             LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___M     | 
             LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___M;

      fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___S)   |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___S);
      break;

   case 1:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___M     | 
             LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___M    | 
             LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___M     | 
             LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___M;

      fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___S)   |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___S);

      break;

   case 2:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___M     | 
             LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___M    | 
             LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___M     | 
             LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___M;

      fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___S)   |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___S);

      break;

   case 3:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___M     | 
             LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___M    | 
             LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___M     | 
             LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___M;

      fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___S)   |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___S)    |
                   (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___S);

      break;
   }

   // For enable case, clearing the interrupts first
   if(1 == enable)
   {
      UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_CLR), hdmi_tx_virt_offset, mask,fieldValue,0);
   }

   // Enabling the interrupts now
   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_EN), hdmi_tx_virt_offset, mask,fieldValue,0);
}



void hal_lpass_hdmi_output_enable_packet_update_interrupt_v2(uint16 enable)   //1 - enable, 0 - disable
{
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_INTERRUPT_EN__PACKET_UPDATE___M;

   fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PACKET_UPDATE___S);

   // Clearing the interrupts first
   if(1 == enable)
   {
      UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_CLR), hdmi_tx_virt_offset, mask,fieldValue,1);
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_EN), hdmi_tx_virt_offset, mask,fieldValue,0);
}


void hal_lpass_hdmi_output_enable_packet_update_b_interrupt_v2(uint16 enable)   //1 - enable, 0 - disable
{
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_INTERRUPT_EN__PACKET_UPDATE_B___M;

   fieldValue = (enable << LPASS_HDMITX_INTERRUPT_EN__PACKET_UPDATE_B___S);

   // Clearing the interrupts first
   if(1 == enable)
   {
      UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_CLR), hdmi_tx_virt_offset, mask,fieldValue,1);
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_EN), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_outputt_read_interrupt_status_v2(uint32_t *int_status)
{
   uint32_t int_enable;
   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_STATUS),
         hdmi_tx_virt_offset, (uint32 *)int_status);

   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_EN),
            hdmi_tx_virt_offset, (uint32 *)&int_enable);
   //we are only concerned with the status bits for which corresponding interrupt is enabled
   *int_status = *int_status & int_enable;
}

void hal_lpass_hdmi_outputt_clear_interrupts_v2(uint32_t clear_mask)
{
   uint32 mask;

   mask = clear_mask;

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_INTERRUPT_CLR), hdmi_tx_virt_offset, mask,clear_mask,1);
}


uint32 hal_lpass_hdmi_output_check_dma_intr_v2(uint16 dma_idx, uint32 int_status)
{
   uint32 dma_int_bit_mask = 0;
   uint32 dma_int=0;
   int32 dma_bit_shift;

   dma_bit_shift = dma_idx * 3;
   dma_int_bit_mask = 0x7 << dma_bit_shift;
   dma_int = int_status & dma_int_bit_mask;

   if(dma_int)
   {
      dma_int = dma_int >> dma_bit_shift;
   }
   else
   {
      dma_int = 0;
   }

   return dma_int;

}

void hal_lpass_hdmi_output_get_dma_int_stc_v2(uint16 dma_idx, uint64 *stc_ptr)
{
   uint32  lsb, msb1, msb2;
   //Whenever DMA interrupt happens, the DMA STC registers latch on to the STC value (from AV-timer)
   //at that instant.
   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_STC_MSBa(dma_idx)),hdmi_tx_virt_offset,&msb1);
   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_STC_LSBa(dma_idx)),hdmi_tx_virt_offset,&lsb);
   //Read the MSB twice to check if it has changed. If so read the LSB once again
   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_STC_MSBa(dma_idx)),hdmi_tx_virt_offset,&msb2);
   if(msb1 != msb2)
   {
      ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_STC_LSBa(dma_idx)),hdmi_tx_virt_offset,&lsb);
   }
   lsb = (lsb & LPASS_HDMITX_RDDMA_STC_LSBa___M) >> LPASS_HDMITX_RDDMA_STC_LSBa___S;
   msb2 = (msb2 & LPASS_HDMITX_RDDMA_STC_MSBa__STC_MSB___M) >> LPASS_HDMITX_RDDMA_STC_MSBa__STC_MSB___S;
   *stc_ptr = ((uint64)msb2 << 32) | lsb;

   return;
}

void hal_lpass_hdmi_output_get_dma_fifo_v2(uint16 dma_idx, uint32_t *fifo_count_ptr)
{
   uint32_t fifo_cnt;
   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_PERa(dma_idx)), hdmi_tx_virt_offset, &fifo_cnt);
   *fifo_count_ptr = (fifo_cnt & LPASS_HDMITX_RDDMA_PERa__PERIOD_FIFO___M) >> LPASS_HDMITX_RDDMA_PERa__PERIOD_FIFO___S;

   return;
}


void hal_lpass_hdmi_output_get_samples_in_dword_after_intr_v2(uint16 dma_idx, uint32_t *sample_in_dwrod_ptr)
{
   uint32  value;

   ReadRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_RDDMA_PER_CNTa(dma_idx)), hdmi_tx_virt_offset, &value);

   /**
       * Read DMAs connect to SPKR (SINK)
       * The number of samples transferred by the sink codecs, is lesser than
       * number of samples read from memory by FIFO Word count.
       */
   *sample_in_dwrod_ptr = 
      ((LPASS_HDMITX_RDDMA_PER_CNTa__PER_CNT___M & value ) >>
        LPASS_HDMITX_RDDMA_PER_CNTa__PER_CNT___S) -
      ((LPASS_HDMITX_RDDMA_PER_CNTa__FIFO_WORDCNT___M & value ) >>
        LPASS_HDMITX_RDDMA_PER_CNTa__FORMAT_ERR___S ) ;
}

////////////////////////////////////////////////
//Single stream ONLY functions
//Single stream ONLY functions
////////////////////////////////////////////////
void hal_lpass_hdmi_output_ss_cfg_pack_trans_v2(hal_lpass_hdmi_output_ss_pack_trans_cfg_t ss_cfg)
{
   //HDMITX_SSTREAM_CTRL RESET_VALUE 0x000009E8
   //     SET_SP_ON_EN BIT[11]     -- always on, default
   //     FORCE_UNMUTE BIT[10]     -- change to one bit
   //     FORCE_MUTE BIT[9]
   //     LAYOUT1_SP_MASK BIT[8:5]
   //     LAYOUT BIT[4]
   //     AUTO_BBIT_EN BIT[3]
   //     DMA_SEL BIT[2:1]
   //     SSTREAM_EN BIT[0]          -> data packet trans trigger bit, 1 for enable data trans, 0 for disable 

   uint32 mask,fieldValue;
   uint16 layout = 0;

   if(0 != ss_cfg.sp_bits_layout1)
   {
      //layout 1 case
      layout = 1;
   }

   mask = LPASS_HDMITX_SSTREAM_CTRL__DMA_SEL___M | LPASS_HDMITX_SSTREAM_CTRL__LAYOUT___M | LPASS_HDMITX_SSTREAM_CTRL__LAYOUT1_SP_MASK___M;

   fieldValue = (ss_cfg.dma_idx << LPASS_HDMITX_SSTREAM_CTRL__DMA_SEL___S) | 
                (layout << LPASS_HDMITX_SSTREAM_CTRL__LAYOUT___S)          | 
                (ss_cfg.sp_bits_layout1 << LPASS_HDMITX_SSTREAM_CTRL__LAYOUT1_SP_MASK___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_SSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_ss_enable_pack_trans_v2(uint16 enable)   //1 - enable, 0 - disable
{
   // SSTREAM_EN BIT@HDMITX_SSTREAM_CTRL
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_SSTREAM_CTRL__SSTREAM_EN___M;

   fieldValue = (enable << LPASS_HDMITX_SSTREAM_CTRL__SSTREAM_EN___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_SSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}


////////////////////////////////////////////////////////
//multiple stream ONLY functions
//multiple stream ONLY functions
////////////////////////////////////////////////////////

void hal_lpass_hdmi_output_ms_cfg_pack_trans_v2(hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg)
{
   //LPASS_HDMITX_MSTREAM_CTRL__DMA3_STREAM___POR 0x3
   //LPASS_HDMITX_MSTREAM_CTRL__DMA2_STREAM___POR 0x2
   //LPASS_HDMITX_MSTREAM_CTRL__DMA1_STREAM___POR 0x1
   //LPASS_HDMITX_MSTREAM_CTRL__DMA0_STREAM___POR 0x0

   uint32 mask,fieldValue;

   //stream_id SW and HW mapping:
   //SW: 1 , 2, 3, 4
   //HW: 0,  1, 2, 3
   uint16 hal_stream_id = ms_cfg.stream_id - 1;

   switch(ms_cfg.dma_idx)
   {
   case 0:
   default:
       mask = LPASS_HDMITX_MSTREAM_CTRL__DMA0_STREAM___M;
       fieldValue = (hal_stream_id << LPASS_HDMITX_MSTREAM_CTRL__DMA0_STREAM___S);
      break;
   case 1:
       mask = LPASS_HDMITX_MSTREAM_CTRL__DMA1_STREAM___M;
       fieldValue = (hal_stream_id << LPASS_HDMITX_MSTREAM_CTRL__DMA1_STREAM___S);
      break;
   case 2:
       mask = LPASS_HDMITX_MSTREAM_CTRL__DMA2_STREAM___M;
       fieldValue = (hal_stream_id << LPASS_HDMITX_MSTREAM_CTRL__DMA2_STREAM___S);
      break;
   case 3:
       mask = LPASS_HDMITX_MSTREAM_CTRL__DMA3_STREAM___M;
       fieldValue = (hal_stream_id << LPASS_HDMITX_MSTREAM_CTRL__DMA3_STREAM___S);
      break;
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_MSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_ms_enable_pack_trans_v2(uint16 enable)   //1 - enable, 0 - disable
{
   //LPASS_HDMITX_MSTREAM_CTRL__PACKER_EN
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_MSTREAM_CTRL__PACKER_EN___M;
   fieldValue = (enable << LPASS_HDMITX_MSTREAM_CTRL__PACKER_EN___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_MSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}

void hal_lpass_hdmi_output_ms_mute_stream_v2(uint16 stream_id, uint16 mute)   //1 - mute, 0 - unmute
{
   //LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM3___POR 0x0
   //LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM2___POR 0x0
   //LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM1___POR 0x0
   //LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM0___POR 0x0
   uint32 mask,fieldValue;

   //stream_id SW and HW mapping:
   //SW: 1 , 2, 3, 4
   //HW: 0,  1, 2, 3
   uint16 hal_stream_id = stream_id - 1;

   switch(hal_stream_id)
   {
   case 0:
   default:
       mask = LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM0___M;
       fieldValue = (mute << LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM0___S);
      break;
   case 1:
       mask = LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM1___M;
       fieldValue = (mute << LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM1___S);
      break;
   case 2:
        mask = LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM2___M;
       fieldValue = (mute << LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM2___S);
      break;
   case 3:
       mask = LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM3___M;
       fieldValue = (mute << LPASS_HDMITX_MSTREAM_CTRL__MUTE_STREAM3___S);
      break;
   }

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_MSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
}


void hal_lpass_hdmi_output_ms_wait_bbit_v2(uint16 stream_id, uint16 wait)    //1 - wait, 0 - unwait)
{
   //use default - always waiting for B frame

   //LPASS_HDMITX_MSTREAM_CTRL__WAIT_BBIT_STREAM3___POR 0x1
   //LPASS_HDMITX_MSTREAM_CTRL__WAIT_BBIT_STREAM2___POR 0x1
   //LPASS_HDMITX_MSTREAM_CTRL__WAIT_BBIT_STREAM1___POR 0x1
   //LPASS_HDMITX_MSTREAM_CTRL__WAIT_BBIT_STREAM0___POR 0x1
   return;
}

void hal_lpass_hdmi_output_dp_mute_stream_v2(uint16 stream_id, uint16 mute)   //1 - mute, 0 - unmute
{
   //LPASS_HDMITX_MSTREAM_CTRL__PACKER_EN
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_DP_METADATA_CTRL__MUTE___M;
   fieldValue = (mute << LPASS_HDMITX_DP_METADATA_CTRL__MUTE___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_DP_METADATA_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);

   return;
}

void hal_lpass_hdmi_output_dp_cfg_audio_info_v2(hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg)
{
   //LPASS_HDMITX_DP_METADATA_CTRL
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_DP_METADATA_CTRL__AS_SDP_HB3_CC___M | LPASS_HDMITX_DP_METADATA_CTRL__AS_SDP_HB3_CT___M | 
          LPASS_HDMITX_DP_METADATA_CTRL__AIF_DB4_CA___M | LPASS_HDMITX_DP_METADATA_CTRL__SAMPLING_FREQ___M;
   fieldValue = (audioinfo_cfg.cc_idx << LPASS_HDMITX_DP_METADATA_CTRL__AS_SDP_HB3_CC___S) | (audioinfo_cfg.ct_idx << LPASS_HDMITX_DP_METADATA_CTRL__AS_SDP_HB3_CT___S) |
                (audioinfo_cfg.ca_idx << LPASS_HDMITX_DP_METADATA_CTRL__AIF_DB4_CA___S) | (audioinfo_cfg.freq_idx << LPASS_HDMITX_DP_METADATA_CTRL__SAMPLING_FREQ___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_DP_METADATA_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);

   return;
}

void hal_lpass_hdmi_output_dp_engine_enable_v2(uint16 enable)
{
   //LPASS_HDMITX_MSTREAM_CTRL__PACKER_EN
   uint32 mask,fieldValue;

   mask = LPASS_HDMITX_SSTREAM_CTRL__DISPLAY_PORT_EN___M;
   fieldValue = (enable << LPASS_HDMITX_SSTREAM_CTRL__DISPLAY_PORT_EN___S);

   UpdateRegister1(hdmi_tx_phy_base + REG_OFFSET(LPASS_HDMITX_SSTREAM_CTRL), hdmi_tx_virt_offset, mask,fieldValue,0);
   return;
}

//common functions for both single stream and multiple stream cases

void hal_lpass_hdmi_output_init_v2(hal_hdmi_output_func_def_t* func_table_ptr, uint32 phy_base, uint32 virt_base, uint32 reset_phy_base, uint32 reset_virt_base)
{
   hdmi_tx_phy_base = phy_base;

   ///< Calculate offset of virtual address in words (4 bytes)
   hdmi_tx_virt_offset = (virt_base - phy_base)>>2; 

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE lpass HDMI HAL:hdmiAudPhyBase =0x%x hdmiAudVirtOffset = 0x%x \n", hdmi_tx_phy_base, hdmi_tx_virt_offset);

   hdmi_tx_reset_phy_base = reset_phy_base;

   ///< Calculate offset of virtual address in words (4 bytes)
   hdmi_tx_reset_virt_offset = (reset_virt_base - reset_phy_base)>>2; 

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE lpass reset HDMI HAL:hdmiAudPhyBase =0x%x hdmiAudVirtOffset = 0x%x \n", hdmi_tx_reset_phy_base, hdmi_tx_reset_virt_offset);

   if(func_table_ptr)
   {
      func_table_ptr->hal_lpass_hdmi_output_enable_dma_interrupt_fptr = hal_lpass_hdmi_output_enable_dma_interrupt_v2;
      func_table_ptr->hal_lpass_hdmi_output_enable_packet_update_interrupt_fptr = hal_lpass_hdmi_output_enable_packet_update_interrupt_v2;
      func_table_ptr->hal_lpass_hdmi_output_enable_packet_update_b_interrupt_fptr = hal_lpass_hdmi_output_enable_packet_update_b_interrupt_v2;
      func_table_ptr->hal_lpass_hdmi_outputt_read_interrupt_status_fptr = hal_lpass_hdmi_outputt_read_interrupt_status_v2;
      func_table_ptr->hal_lpass_hdmi_outputt_clear_interrupts_fptr = hal_lpass_hdmi_outputt_clear_interrupts_v2;
      func_table_ptr->hal_lpass_hdmi_output_check_dma_intr_fptr = hal_lpass_hdmi_output_check_dma_intr_v2;
      func_table_ptr->hal_lpass_hdmi_output_get_dma_int_stc_fptr = hal_lpass_hdmi_output_get_dma_int_stc_v2;
      func_table_ptr->hal_lpass_hdmi_output_get_dma_fifo_fptr = hal_lpass_hdmi_output_get_dma_fifo_v2;
      func_table_ptr->hal_lpass_hdmi_output_get_samples_in_dword_after_intr_fptr = hal_lpass_hdmi_output_get_samples_in_dword_after_intr_v2;
      func_table_ptr->hal_lpass_hdmi_output_enable_legacy_mode_fptr = hal_lpass_hdmi_output_enable_legacy_mode_v2;   
      func_table_ptr->hal_lpass_hdmi_output_enable_parity_hw_mode_fptr = hal_lpass_hdmi_output_enable_parity_hw_mode_v2;   
      func_table_ptr->hal_lpass_hdmi_output_cfg_ch_status_buf_fptr = hal_lpass_hdmi_output_cfg_ch_status_buf_v2;
      func_table_ptr->hal_lpass_hdmi_output_cfg_user_bits_buf_fptr = hal_lpass_hdmi_output_cfg_user_bits_buf_v2;
      func_table_ptr->hal_lpass_hdmi_output_enable_vbit_hw_mode_fptr = hal_lpass_hdmi_output_enable_vbit_hw_mode_v2;
      func_table_ptr->hal_lpass_hdmi_output_disable_vbit_hw_mode_fptr = hal_lpass_hdmi_output_disable_vbit_hw_mode_v2;

      func_table_ptr->hal_lpass_hdmi_output_ss_cfg_pack_trans_fptr = hal_lpass_hdmi_output_ss_cfg_pack_trans_v2;
      func_table_ptr->hal_lpass_hdmi_output_ss_enable_pack_trans_fptr = hal_lpass_hdmi_output_ss_enable_pack_trans_v2;   

      func_table_ptr->hal_lpass_hdmi_output_ms_cfg_pack_trans_fptr = hal_lpass_hdmi_output_ms_cfg_pack_trans_v2;
      func_table_ptr->hal_lpass_hdmi_output_ms_enable_pack_trans_fptr = hal_lpass_hdmi_output_ms_enable_pack_trans_v2;   
      func_table_ptr->hal_lpass_hdmi_output_ms_mute_stream_fptr = hal_lpass_hdmi_output_ms_mute_stream_v2;  
      func_table_ptr->hal_lpass_hdmi_output_ms_wait_bbit_fptr = hal_lpass_hdmi_output_ms_wait_bbit_v2;    

      func_table_ptr->hal_lpass_hdmi_output_dp_mute_stream_fptr = hal_lpass_hdmi_output_dp_mute_stream_v2;    
      func_table_ptr->hal_lpass_hdmi_output_dp_cfg_audio_info_fptr = hal_lpass_hdmi_output_dp_cfg_audio_info_v2;
      func_table_ptr->hal_lpass_hdmi_output_dp_engine_enable_fptr = hal_lpass_hdmi_output_dp_engine_enable_v2;
   }

   return;
}
