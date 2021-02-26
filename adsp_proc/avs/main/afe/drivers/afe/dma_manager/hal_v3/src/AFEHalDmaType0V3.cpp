/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/hal_v3/src/AFEHalDmaType0V3.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEHalDmaType0V3.cpp

DESCRIPTION: DMA Manager Type3 (Bear+) HAL layer

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2015 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/hal_v3/src/AFEHalDmaType0V3.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/06/2015  SJ      created
============================================================================*/

/*==========================================================================
  Include Files
========================================================================== */

#include "LPASS_ADDRESS_FILE_LPAIF_V3.h"
#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEHalDmaManager_i.h"
#include "AFEHalDmaManagerCommon.h"
#include "AFEHalDump.h"



/*==========================================================================
  global data
========================================================================== */
static uint32_t                  reg_base_type0 = 0; 
static int32_t                   int_reg_virtoffset_type0 = 0;
static int32_t                   rddma_reg_virtoffset_type0 = 0;
static int32_t                   wrdma_reg_virtoffset_type0 = 0;
static int32_t                   stc_rddma_reg_virtoffset_type0 = 0;
static int32_t                   stc_wrdma_reg_virtoffset_type0 = 0;
static uint32_t                  intr_index_type0 = 2;


#ifdef AUDIOIF_REG_DUMP   
#define LPAIF_RDDMA_MAX  LPAIF_RDDMA_CTLa_ELEM
#define LPAIF_WRDMA_MAX  LPAIF_WRDMA_CTLa_ELEM

typedef struct hal_audioif_dma_dump_v3
{
  uint32_t LPASS_LPAIF_RDDMA_CTLa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_BASEa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_PER_LENa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_CTLa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_BASEa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_PER_LENa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
} hal_audioif_dma_dump_v3_t;

static hal_audioif_dma_dump_v3_t afe_hal_audioif_dma_dump_v3;

static void hal_dma_dump_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq);
#endif


/*==========================================================================
  Function Definitions
========================================================================== */
static void hal_dma_enable_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_config_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);
static void hal_dma_get_dma_curr_addr_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr);
static void hal_dma_clear_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_enable_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_get_dma_int_stc_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint64_t *p_stc);
static void hal_dma_get_dma_fifo_count_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t *fifo_cnt_ptr);


static uint32_t hal_dma_check_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t int_status, uint32 int_status2);
static void hal_dma_read_interrupt_status_type0_v3(uint32 *int_status_ptr, uint32 *int_status2_ptr);
static void hal_dma_clear_interrupt_type0_v3(uint32_t int_status, uint32_t int_status2);

static void hal_dma_reset_type0_v3(uint32_t dma_dir, uint32_t dma_idx);


void hal_dma_init_type0_v3(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base, lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr)
{
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hal_dma_init_type0_v3 is called");
  if(NULL == func_table)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "func_table is NULL ====");
    return;
  }

  //func_table->afe_hal_dma_init_fptr;
  func_table->afe_hal_dma_enable_dma_channel_fptr = hal_dma_enable_dma_channel_type0_v3;
  func_table->afe_hal_dma_disable_dma_channel_fptr = hal_dma_disable_dma_channel_type0_v3;
  func_table->afe_hal_dma_config_dma_channel_fptr = hal_dma_config_dma_channel_type0_v3;
  func_table->afe_hal_dma_get_dma_curr_addr_fptr = hal_dma_get_dma_curr_addr_type0_v3;
  func_table->afe_hal_dma_disable_dma_interrupt_fptr = hal_dma_disable_dma_interrupt_type0_v3;
  func_table->afe_hal_dma_enable_dma_interrupt_fptr = hal_dma_enable_dma_interrupt_type0_v3;
  func_table->afe_hal_dma_clear_dma_interrupt_fptr = hal_dma_clear_dma_interrupt_type0_v3;
  func_table->afe_hal_dma_get_dma_interrupt_stc_fptr   = hal_dma_get_dma_int_stc_type0_v3;
  func_table->afe_hal_dma_get_dma_fifo_count_fptr = hal_dma_get_dma_fifo_count_type0_v3;

  // interrupt handling
  func_table->afe_hal_dma_check_dma_interrupt_fptr = hal_dma_check_dma_interrupt_type0_v3;
  func_table->afe_hal_dma_read_interrupt_status_fptr = hal_dma_read_interrupt_status_type0_v3;
  func_table->afe_hal_dma_clear_interrupt_fptr = hal_dma_clear_interrupt_type0_v3;

  //ssr handling
  func_table->afe_hal_dma_reset_fptr  = hal_dma_reset_type0_v3;

#ifdef AUDIOIF_REG_DUMP              
  func_table->afe_hal_dma_dump_fptr = hal_dma_dump_type0_v3;
#endif

  intr_index_type0 = intr_index;

  reg_base_type0 = dma_hw_info_ptr->dma_reg_base;
  int_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->dma_int_reg_offset_addr);
  rddma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->rddma_reg_offset_addr);
  wrdma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->wrdma_reg_offset_addr);
  stc_rddma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->stc_rddma_reg_offset_addr);
  stc_wrdma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->stc_wrdma_reg_offset_addr);

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dma Type0 [intr_index0:%d] [reg_base_type0:0x%x] [dma_reg_virtoffset_type0:0x%x]", \
        intr_index_type0, reg_base_type0, int_reg_virtoffset_type0);
  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[rddma_reg_virtoffset_type0:0x%x] [wrdma_reg_virtoffset_type0:0x%x] [stc_rddma_reg_virtoffset_type0:0x%x] [stc_wrdma_reg_virtoffset_type0:0x%x]", \
        rddma_reg_virtoffset_type0, wrdma_reg_virtoffset_type0, stc_rddma_reg_virtoffset_type0, stc_wrdma_reg_virtoffset_type0);

}

static int32_t hal_dma_int_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_IRQ_EN0);
}

static int32_t hal_rddma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_RDDMA_CTL0);
}

static int32_t hal_wrdma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_WRDMA_CTL0);
}

static int32_t hal_stc_rddma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_RDDMA_STC_LSB0);
}

static int32_t hal_stc_wrdma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_WRDMA_STC_LSB0);
}


static void hal_dma_enable_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    mask = LPAIF_RDDMA_CTLa__ENABLE___M;
    field_value = LPAIF_RDDMA_CTLa__ENABLE__ON << LPAIF_RDDMA_CTLa__ENABLE___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

    //enable dynamic clock control.
    mask = LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK__ON << LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    mask = LPAIF_WRDMA_CTLa__ENABLE___M;
    field_value = LPAIF_WRDMA_CTLa__ENABLE__ON << LPAIF_WRDMA_CTLa__ENABLE___S;
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);

    //enable dynamic clock control.
    mask = LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK__ON << LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }

}


static void hal_dma_disable_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    //disable dynamic clock enable.
    mask = LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK__OFF << LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

    mask = LPAIF_RDDMA_CTLa__ENABLE___M | LPAIF_RDDMA_CTL0__AUDIO_INTF___M;
    field_value = (LPAIF_RDDMA_CTLa__ENABLE__OFF << LPAIF_RDDMA_CTLa__ENABLE___S)
              | (LPAIF_RDDMA_CTLa__AUDIO_INTF__NONE << LPAIF_RDDMA_CTLa__AUDIO_INTF___S);

    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    //disable dynamic clock enable.
    mask = LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK__OFF << LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);

    mask = LPAIF_WRDMA_CTLa__ENABLE___M | LPAIF_WRDMA_CTL0__AUDIO_INTF___M;
    field_value = (LPAIF_WRDMA_CTLa__ENABLE__OFF << LPAIF_WRDMA_CTLa__ENABLE___S)
              | (LPAIF_WRDMA_CTLa__AUDIO_INTF__NONE << LPAIF_WRDMA_CTLa__AUDIO_INTF___S);
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);

  }

}

static void hal_dma_config_dma_channel_type0_v3(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{
  uint32_t mask,field_value;

  ///< Update the base address of the DMA buffer
  mask = LPAIF_RDDMA_BASEa__BASE_ADDR___M;
  field_value = ((uint32)dma_config_ptr->buffer_start_addr >> LPAIF_RDDMA_BASEa__BASE_ADDR___S)
                           << LPAIF_RDDMA_BASEa__BASE_ADDR___S;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    //READ DMA
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BASEa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    //WRITE DMA
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BASEa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }

  ///< Update the buffer length of the DMA buffer.
  mask = LPAIF_RDDMA_BUFF_LENa__LENGTH___M;
  field_value = (dma_config_ptr->buffer_len-1);

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    //RDDMA
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BUFF_LENa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    //WRDMA
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BUFF_LENa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }

  ///< Update the interrupt sampler period
  mask = LPAIF_RDDMA_PER_LENa___M;
  field_value=(dma_config_ptr->dma_int_per_cnt-1)<<LPAIF_RDDMA_PER_LENa___S;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PER_LENa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PER_LENa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }

  /**
   * Update the audio interface and fifo Watermark in AUDIO_DMA_CTLa(channel)
   * register. Obtain these values from the input configuration structure
   */
  mask = LPAIF_RDDMA_CTLa__BURST_EN___M |
      LPAIF_RDDMA_CTLa__AUDIO_INTF___M |
      LPAIF_RDDMA_CTLa__FIFO_WATERMRK___M |
      LPAIF_RDDMA_CTLa__WPSCNT___M;


  field_value = (dma_config_ptr->ifconfig_dma_control) <<LPAIF_RDDMA_CTLa__AUDIO_INTF___S;


  field_value |=((dma_config_ptr->watermark-1)<<LPAIF_RDDMA_CTLa__FIFO_WATERMRK___S);

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   * 8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   * The burst mode needs to be disabled for this case for latency and drift detection issue
   */
  if((LPAIF_RDDMA_CTLa__FIFO_WATERMRK__ENUM_8+1) < dma_config_ptr->dma_int_per_cnt)
  {
    //enable BURST basing on burst_size
    switch(dma_config_ptr->burst_size)
    {
      case 4:
      case 8:
      case 16:
        field_value |= (LPAIF_RDDMA_CTLa__BURST_EN__INCR4 << LPAIF_RDDMA_CTLa__BURST_EN___S);
        break;
      case 1:
        field_value |= (LPAIF_RDDMA_CTLa__BURST_EN__SINGLE<<LPAIF_RDDMA_CTLa__BURST_EN___S);
        break;
      default:
        break;
    }
  }
  else
  {
    field_value |= (LPAIF_RDDMA_CTLa__BURST_EN__SINGLE<<LPAIF_RDDMA_CTLa__BURST_EN___S);
  }

  field_value |=  ((dma_config_ptr->wps_count - 1) << LPAIF_RDDMA_CTLa__WPSCNT___S);

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }
}



static void hal_dma_get_dma_curr_addr_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr)
{
  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CURR_ADDRa(dma_idx)),
                  rddma_reg_virtoffset_type0, (uint32 *)curr_addr);
  }
  else
  {
    ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CURR_ADDRa(dma_idx)),
                  wrdma_reg_virtoffset_type0, (uint32 *)curr_addr);
  }
}

static void hal_dma_clear_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32 mask;
  uint32 clear_mask = 0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      case 0:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_RDDMA_CH0___M
            | LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH0___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_RDDMA_CH1___M
            | LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH1___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_RDDMA_CH2___M
            | LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH2___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_RDDMA_CH3___M
            | LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH3___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_RDDMA_CH4___M
            | LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH4___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH4___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      case 0:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_WRDMA_CH0___M
            | LPAIF_IRQ_CLEARa__OVR_WRDMA_CH0___M
            | LPAIF_IRQ_CLEARa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_WRDMA_CH1___M
            | LPAIF_IRQ_CLEARa__OVR_WRDMA_CH1___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_WRDMA_CH2___M
            | LPAIF_IRQ_CLEARa__OVR_WRDMA_CH2___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        clear_mask = (LPAIF_IRQ_CLEARa__PER_WRDMA_CH3___M
            | LPAIF_IRQ_CLEARa__OVR_WRDMA_CH3___M
            | LPAIF_IRQ_CLEARa__ERR_RDDMA_CH3___M);
        break;
      default:
        break;
    }
  }

  //UpdateRegister1 function: it can update whatever in the field value regardless of mask.
  //the mask only zero out value after reading the current value.
  //Therefore, if we use the mask as zero, we do not apply any value in the reg.
  //So that we will not clear any interrupt we don't want.
  mask = 0x0;
  ///< This register is write-only
  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_CLEARa(intr_index_type0)),
                  int_reg_virtoffset_type0,mask,clear_mask,1);
}


static void hal_dma_disable_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t disable_mask=0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      case 0:
        disable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH0___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH0___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        disable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH1___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH1___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        disable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH2___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH2___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        disable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH3___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH3___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        disable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH4___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH4___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH4___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      case 0:
        disable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH0___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH0___M
            | LPAIF_IRQ_ENa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        disable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH1___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH1___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        disable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH2___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH2___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        disable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH3___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH3___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      default:
        break;
    }
  }

  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_ENa(intr_index_type0)),
                  int_reg_virtoffset_type0,disable_mask,0x0,0);
}

static void hal_dma_enable_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t enable_mask = 0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      case 0:
        enable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH0___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH0___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        enable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH1___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH1___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        enable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH2___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH2___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        enable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH3___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH3___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        enable_mask = (LPAIF_IRQ_ENa__PER_RDDMA_CH4___M
            | LPAIF_IRQ_ENa__UNDR_RDDMA_CH4___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH4___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      case 0:
        enable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH0___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH0___M
            | LPAIF_IRQ_ENa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        enable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH1___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH1___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        enable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH2___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH2___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        enable_mask = (LPAIF_IRQ_ENa__PER_WRDMA_CH3___M
            | LPAIF_IRQ_ENa__OVR_WRDMA_CH3___M
            | LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      default:
        break;
    }
  }

  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_ENa(intr_index_type0)),
                  int_reg_virtoffset_type0, enable_mask,enable_mask,0);
}


static uint32_t hal_dma_check_dma_interrupt_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t int_status, uint32 int_status2)
{
  uint32_t dma_idx_int_bitmask = 0;
  uint32_t dma_int=0;
  uint32_t dma_bit_shift =0;

  //dma channel num should be converted to bit postion for that channel.
  // 8960 and 8974 is not matching this position for TX.
  // This also require information the direction of dma channel

  // 3 bits per channel
  // 5 channels RDDMA bits and 4 channels WRDMA bits
  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    dma_bit_shift = dma_idx*3;
  }
  else
  {
    dma_bit_shift = (dma_idx + 5)*3;
  }

  dma_idx_int_bitmask = 0x7 << dma_bit_shift; //0:0, 1:3, 2:6, 3:9, 4:12, 5:15, 6:18, 7:21, 8: 24.

  dma_int = int_status & dma_idx_int_bitmask;
  if(dma_int)
  {
    dma_int = dma_int >> dma_bit_shift;
    //TODO: convert it to the event AFEDalEventType

    return dma_int; //1, 2, 4 (0x1, 0x2, 0x4)
  }
  return FALSE;
}

static void hal_dma_read_interrupt_status_type0_v3(uint32 *int_status_ptr, uint32 *int_status2_ptr)
{
  /* Read the interrupt status only for enabled interrupts.*/
  ReadRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_STATa(intr_index_type0)),
                int_reg_virtoffset_type0,int_status_ptr);

  /*The valid interrupts are 0 to 26 (total 27) for dma*/
  *int_status_ptr = *int_status_ptr & 0x7FFFFFF;                                                                     

  /* this is not valid for this HW version*/
  *int_status2_ptr = 0;                                                                    
}

static void hal_dma_clear_interrupt_type0_v3(uint32_t int_status, uint32_t int_status2)
{
  uint32_t mask;

  mask = int_status;

  ///< This register is write-only
  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_CLEARa(intr_index_type0)),
                  int_reg_virtoffset_type0,mask,int_status,1);

}


static void hal_dma_get_dma_int_stc_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint64_t *p_stc)
{
  uint32  lsb, msb1, msb2;

  /**
   * Whenever DMA interrupt happens, the DMA STC registers latch on to the STC value (from AV-timer)
   * at that instant.
   */

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    ReadRegister1(reg_base_type0 + hal_stc_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_STC_MSBa(dma_idx)),stc_rddma_reg_virtoffset_type0,&msb1);
    ReadRegister1(reg_base_type0 + hal_stc_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_STC_LSBa(dma_idx)),stc_rddma_reg_virtoffset_type0,&lsb);
    ReadRegister1(reg_base_type0 + hal_stc_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_STC_MSBa(dma_idx)),stc_rddma_reg_virtoffset_type0,&msb2);
    if(msb1 != msb2)
    {
      ReadRegister1(reg_base_type0 + hal_stc_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_STC_LSBa(dma_idx)),stc_rddma_reg_virtoffset_type0,&lsb);
    }
    lsb = (lsb & LPAIF_RDDMA_STC_LSBa___M) >> LPAIF_RDDMA_STC_LSBa___S;
    msb2 = (msb2 & LPAIF_RDDMA_STC_MSBa___M) >> LPAIF_RDDMA_STC_MSBa___S;
    *p_stc = ((uint64)msb2 << 32) | lsb;
  }
  else
  {
    ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&msb1);
    ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&lsb);
    ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&msb2);
    if(msb1 != msb2)
    {
      ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&lsb);
    }
    lsb = (lsb & LPAIF_WRDMA_STC_LSBa___M) >> LPAIF_WRDMA_STC_LSBa___S;
    msb2 = (msb2 & LPAIF_WRDMA_STC_MSBa___M) >> LPAIF_WRDMA_STC_MSBa___S;
    *p_stc = ((uint64)msb2 << 32) | lsb;
  }
}


static void hal_dma_get_dma_fifo_count_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t *fifo_cnt_ptr)
{
  uint32_t fifo_count = 0;
  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PERa(dma_idx)),rddma_reg_virtoffset_type0,&fifo_count);
    *fifo_cnt_ptr = (fifo_count & LPAIF_RDDMA_PERa__PERIOD_FIFO___M) >> LPAIF_RDDMA_PERa__PERIOD_FIFO___S;
  }
  else
  {
    ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PERa(dma_idx)),wrdma_reg_virtoffset_type0,&fifo_count);
    *fifo_cnt_ptr = (fifo_count & LPAIF_WRDMA_PERa__PERIOD_FIFO___M) >> LPAIF_WRDMA_PERa__PERIOD_FIFO___S;
  }

}

static void hal_dma_reset_type0_v3(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    // reset to 0
    mask = LPAIF_RDDMA_CTLa___M;
    field_value = 0;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

  }
  else
  {
    //reset to 0
    mask = LPAIF_WRDMA_CTLa___M;
    field_value = 0;
    UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                    wrdma_reg_virtoffset_type0, mask,field_value,0);
  }
}

#ifdef AUDIOIF_REG_DUMP   
static void hal_dma_dump_type0_v3(uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq)
{
  if(LPA_IF_SINK == dma_dir)
  {  //READ DMA
    if(LPAIF_RDDMA_MAX > dma_idx)
    {
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_CTLa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BASEa(dma_idx)),rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_BASEa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BUFF_LENa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PER_LENa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_CTLa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_CTLa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_BASEa(%d) = 0x%x", dma_idx, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_BASEa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_BUFF_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_PER_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_RDDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
    }
  }
  else
  {  //WRITE DMA
    if(LPAIF_WRDMA_MAX > dma_idx)
    {
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_CTLa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BASEa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_BASEa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BUFF_LENa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PER_LENa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_CTLa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_CTLa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_BASEa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_BASEa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_BUFF_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_PER_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v3.LPASS_LPAIF_WRDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
    }
  }
}
#endif     

