/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEHalDmaType1_v2.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hal_dma_manager.cpp

DESCRIPTION: VFR Interrupt Registration / Handling functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2013 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEHalDmaType1_v2.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/24/2013  RP      created
============================================================================*/

/*==========================================================================
  Include Files
========================================================================== */
#include "LPASS_ADDRESS_FILE_HDMI_OUTPUT_v2.h"
#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEHalDmaManager_i.h"
#include "AFEHalDmaManagerCommon.h"

/*==========================================================================
  global data
========================================================================== */
static uint32_t                  reg_base_type1 = 0; 
static int32_t                   reg_virtoffset_type1 = 0;

/*==========================================================================
  Function Definitions
========================================================================== */
static void hal_dma_enable_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_config_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);
static void hal_dma_get_dma_curr_addr_type1_v2(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr);
static void hal_dma_clear_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_enable_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx);
static int32_t hal_dma_register_offset_type1_v2(uint32_t register_addr);

static uint32_t hal_dma_check_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx,uint32_t int_status,uint32 int_status2);
static uint32_t hal_dma_check_hdmi_interrupt_type1_v2(uint32_t int_status);
static void hal_dma_read_interrupt_status_type1_v2(uint32 *int_status_ptr, uint32 *int_status2_ptr);
static void hal_dma_clear_interrupt_type1_v2(uint32_t int_status, uint32_t int_status2);


void hal_dma_init_type1_v2(afe_hal_dma_func_def_t *func_table, uint32_t base_addr, int32_t virt_offset)
{
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hal_dma_init_type1 is called");
  if(NULL == func_table)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "func_table is NULL ====");
    return;
  }

  func_table->afe_hal_dma_enable_dma_channel_fptr = hal_dma_enable_dma_channel_type1_v2;
  func_table->afe_hal_dma_disable_dma_channel_fptr = hal_dma_disable_dma_channel_type1_v2;
  func_table->afe_hal_dma_config_dma_channel_fptr = hal_dma_config_dma_channel_type1_v2;
  func_table->afe_hal_dma_get_dma_curr_addr_fptr = hal_dma_get_dma_curr_addr_type1_v2;
  func_table->afe_hal_dma_disable_dma_interrupt_fptr = hal_dma_disable_dma_interrupt_type1_v2;
  func_table->afe_hal_dma_enable_dma_interrupt_fptr = hal_dma_enable_dma_interrupt_type1_v2;
  func_table->afe_hal_dma_clear_dma_interrupt_fptr = hal_dma_clear_dma_interrupt_type1_v2;

  func_table->afe_hal_dma_get_dma_interrupt_stc_fptr = NULL;;
  func_table->afe_hal_dma_get_dma_fifo_count_fptr = NULL;

  // interrupt handling
  func_table->afe_hal_dma_check_dma_interrupt_fptr = hal_dma_check_dma_interrupt_type1_v2;
  func_table->afe_hal_dma_check_hdmi_interrupt_fptr = hal_dma_check_hdmi_interrupt_type1_v2;
  func_table->afe_hal_dma_read_interrupt_status_fptr = hal_dma_read_interrupt_status_type1_v2;
  func_table->afe_hal_dma_clear_interrupt_fptr = hal_dma_clear_interrupt_type1_v2;

  reg_base_type1 = base_addr;
  reg_virtoffset_type1 = virt_offset;
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dma Type1 [reg_base_type1:0x%x] [reg_virtoffset_type1:0x%x]", reg_base_type1, reg_virtoffset_type1);

}

static int32_t hal_dma_register_offset_type1_v2(uint32_t register_addr)
{
  return (register_addr - LPASS_LPASS_HDMITX_BASE);
}

static void hal_dma_enable_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t field_value;

  //enable DMA channel
  field_value = LPASS_HDMITX_RDDMA_CTLa__ENABLE__ON << LPASS_HDMITX_RDDMA_CTLa__ENABLE___S;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CTLa(dma_idx)), reg_virtoffset_type1,
                  LPASS_HDMITX_RDDMA_CTLa__ENABLE___M, field_value, 0);

  //enable dynamic clock control.
  field_value = LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK__ON << LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK___S;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CTLa(dma_idx)), reg_virtoffset_type1,
                  LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK___M, field_value, 0);

}


static void hal_dma_disable_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t field_value;

  //disable dynamic clock enable.
  field_value = LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK__OFF << LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK___S;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CTLa(dma_idx)),
                  reg_virtoffset_type1, LPASS_HDMITX_RDDMA_CTLa__DYNAMIC_CLOCK___M, field_value, 0);

  //disable DMA channel
  field_value = LPASS_HDMITX_RDDMA_CTLa__ENABLE__OFF << LPASS_HDMITX_RDDMA_CTLa__ENABLE___S;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CTLa(dma_idx)),
                  reg_virtoffset_type1, LPASS_HDMITX_RDDMA_CTLa__ENABLE___M, field_value, 0);

}


static void hal_dma_config_dma_channel_type1_v2(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{
  uint32_t mask, field_value;

  //< Update the base address of the DMA buffer. Address should be 16 byte multiple
  field_value = (uint32)dma_config_ptr->buffer_start_addr & LPASS_HDMITX_RDDMA_BASEa__BASE_ADDR___M;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_BASEa(dma_idx)),
                  reg_virtoffset_type1, LPASS_HDMITX_RDDMA_BASEa__BASE_ADDR___M, field_value, 0);

  ///< Update the buffer length of the DMA buffer.
  field_value = (dma_config_ptr->buffer_len - 1);

  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_BUFF_LENa(dma_idx)),
                  reg_virtoffset_type1, LPASS_HDMITX_RDDMA_BUFF_LENa__LENGTH___M, field_value, 0);

  ///< Update the interrupt sampler period
  field_value = (dma_config_ptr->dma_int_per_cnt - 1) << LPASS_HDMITX_RDDMA_PER_LENa__LENGTH___S;
  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_PER_LENa(dma_idx)),
                  reg_virtoffset_type1, LPASS_HDMITX_RDDMA_PER_LENa__LENGTH___M,field_value, 0);

  ///< Update the FIFO watermark, WPS Count, Burst size
  mask = (LPASS_HDMITX_RDDMA_CTLa__FIFO_WATERMRK___M | LPASS_HDMITX_RDDMA_CTLa__WPSCNT___M);
  field_value = ((dma_config_ptr->watermark - 1) << LPASS_HDMITX_RDDMA_CTLa__FIFO_WATERMRK___S) |
      ((dma_config_ptr->wps_count - 1) << LPASS_HDMITX_RDDMA_CTLa__WPSCNT___S);

  //Enable Dynamic bursts - This means that hardware will pick the largest burst size
  //based on the the alignment
  field_value |= (LPASS_HDMITX_RDDMA_CTLa__DYN_BURST__ON << LPASS_HDMITX_RDDMA_CTLa__DYN_BURST___S);
  mask  |= LPASS_HDMITX_RDDMA_CTLa__DYN_BURST___M;
  //All burst sizes need to be enabled for dynamic burst to take effect
  field_value |= ((LPASS_HDMITX_RDDMA_CTLa__BURST_EN__INCR4 << LPASS_HDMITX_RDDMA_CTLa__BURST_EN___S) |
      (LPASS_HDMITX_RDDMA_CTLa__BURST8_EN__INCR8 << LPASS_HDMITX_RDDMA_CTLa__BURST8_EN___S) |
      (LPASS_HDMITX_RDDMA_CTLa__BURST16_EN__INCR16 << LPASS_HDMITX_RDDMA_CTLa__BURST16_EN___S));

  //set MASK for all BURST related bits
  mask |= (LPASS_HDMITX_RDDMA_CTLa__BURST_EN___M  | LPASS_HDMITX_RDDMA_CTLa__BURST8_EN___M |
      LPASS_HDMITX_RDDMA_CTLa__BURST16_EN___M);

  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CTLa(dma_idx)),
                  reg_virtoffset_type1, mask, field_value, 0);

}


static void hal_dma_get_dma_curr_addr_type1_v2(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr)
{
  ReadRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_RDDMA_CURR_ADDRa(dma_idx)),
                reg_virtoffset_type1, (uint32 *)curr_addr);

}


static void hal_dma_clear_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx)
{

  uint32_t mask,field_value;

  uint16_t enable = 1;

  switch(dma_idx)
  {
    case 0:
    default:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___S);
      break;

    case 1:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___S);

      break;

    case 2:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___S);

      break;

    case 3:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___S);

      break;
  }

  //CLEAR LPASS_HDMITX_INTERRUPT_STATUS__METADATA_DONE_STA
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___S);

  //CLEAR LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___S);

  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_CLR), reg_virtoffset_type1, mask,field_value,1);

}

static void hal_dma_disable_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  uint16_t enable = 0;

  switch(dma_idx)
  {
    case 0:
    default:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___S);
      break;

    case 1:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___S);

      break;

    case 2:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___S);

      break;

    case 3:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___S);

      break;
  }

  //DISABLE LPASS_HDMITX_INTERRUPT_STATUS__METADATA_DONE_STA
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___S);

  //Disable LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___S);


  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_EN), reg_virtoffset_type1, mask,field_value,0);

}

static void hal_dma_enable_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  uint16_t enable = 1;

  switch(dma_idx)
  {
    case 0:
    default:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH0___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH0___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA0___S);
      break;

    case 1:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH1___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH1___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA1___S);

      break;

    case 2:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH2___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH2___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA2___S);

      break;

    case 3:

      mask = LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___M     | 
      LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___M    |
      LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___M     |
      LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___M;

      field_value = (enable << LPASS_HDMITX_INTERRUPT_EN__PER_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__UNDR_RDDMA_CH3___S)   |
          (enable << LPASS_HDMITX_INTERRUPT_EN__ERR_RDDMA_CH3___S)    |
          (enable << LPASS_HDMITX_INTERRUPT_EN__REQON_PRELOAD_DMA3___S);

      break;
  }

  //ENABLE LPASS_HDMITX_INTERRUPT_STATUS__METADATA_DONE_STA
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__METADATA_DONE___S);

  //ENABLE LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS
  mask = mask | LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___M;
  field_value = field_value | (enable << LPASS_HDMITX_INTERRUPT_EN__SDEDP_AUD_DIS___S);

  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_EN), reg_virtoffset_type1, mask,field_value,0);

}


static uint32_t hal_dma_check_dma_interrupt_type1_v2(uint32_t dma_dir, uint32_t dma_idx,uint32_t int_status,uint32 int_status2)
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



/*
Note:

1. The below interrupt bit is for SW to check DMA shutdown DONE or not.
   The purpose of these interrupt bits are for nicely handling below case in loop:

   a) Enable this DMA
   b) Disable this DMA

   If HW does not finish step b) then SW issues step a), HW will be in un-known state.
   However as HW takes only nacro-sec to enable/disable DMA, it should be safe to avoid 
   SHUTDOWN_DMA check as the gap between b) SW disable DMA and a) SW re-enable this DMA should 
   be larger then 100 ms.

   LPASS_HDMITX_INTERRUPT_EN__SHUTDOWN_DMA0 
   LPASS_HDMITX_INTERRUPT_EN__SHUTDOWN_DMA1
   LPASS_HDMITX_INTERRUPT_EN__SHUTDOWN_DMA2
   LPASS_HDMITX_INTERRUPT_EN__SHUTDOWN_DMA3.. 

2. LPASS_HDMITX_INTERRUPT_EN__CA_SW_ERR is for channel allocation setting error check.
   We have SW check before setting up HW.

 */
static uint32_t hal_dma_check_hdmi_interrupt_type1_v2(uint32_t int_status)
{
  uint32 dma_int_bit_mask = 0;
  uint32 dma_int=0;

  dma_int_bit_mask = LPASS_HDMITX_INTERRUPT_STATUS__METADATA_DONE_STA___M;
  dma_int = int_status & dma_int_bit_mask;

  if(dma_int)
  {
    dma_int = DMA_TYPE0_EVENT_DP_METADATA_DONE;
  }
  else
  {
    //check error case
    dma_int_bit_mask = LPASS_HDMITX_INTERRUPT_STATUS__SDEDP_AUD_DIS_STA___M;
    dma_int = int_status & dma_int_bit_mask;

    if(dma_int)
    {
      dma_int = DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR;
    }
    else
    {
      dma_int = 0;
    }
  }

  return dma_int;
}



static void hal_dma_read_interrupt_status_type1_v2(uint32 *int_status_ptr, uint32 *int_status2_ptr)
{
  uint32_t int_enable;
  ReadRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_STATUS),
                reg_virtoffset_type1, (uint32 *)int_status_ptr);

  ReadRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_EN),
                reg_virtoffset_type1, (uint32 *)&int_enable);
  //we are only concerned with the status bits for which corresponding interrupt is enabled
  *int_status_ptr = *int_status_ptr & int_enable;
}


static void hal_dma_clear_interrupt_type1_v2(uint32_t int_status, uint32_t int_status2)
{
  uint32 mask;

  mask = int_status;

  UpdateRegister1(reg_base_type1 + hal_dma_register_offset_type1_v2(LPASS_HDMITX_INTERRUPT_CLR), reg_virtoffset_type1, mask,int_status,1);
}


