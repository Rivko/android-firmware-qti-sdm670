/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/hal_v5/src/AFEHalDmaType0V5.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEHalDmaType0V5.cpp

DESCRIPTION: DMA Manager Type5 (SL/SH families) HAL layer

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/hal_v5/src/AFEHalDmaType0V5.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
8/29/2016  PSR      created
============================================================================*/

/*==========================================================================
  Include Files
========================================================================== */
#include "LPASS_ADDRESS_FILE_LPAIF_v5.h"
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
static int32_t                   wrdma5_reg_virtoffset_type0 = 0;
static int32_t                   stc_rddma_reg_virtoffset_type0 = 0;
static int32_t                   stc_wrdma_reg_virtoffset_type0 = 0;
static int32_t                   stc_wrdma5_reg_virtoffset_type0 = 0;
static uint32_t                  intr_index_type0 = 2;

#ifdef AUDIOIF_REG_DUMP 
#define LPAIF_RDDMA_MAX  LPASS_LPAIF_RDDMA_CTLa_ELEM
#define LPAIF_WRDMA_MAX  LPASS_LPAIF_WRDMA_CTLa_ELEM

typedef struct hal_audioif_dma_dump_v5
{
  uint32_t LPASS_LPAIF_RDDMA_CTLa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_BASEa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_RDDMA_PER_LENa_DUMP[LPAIF_RDDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_CTLa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_BASEa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
  uint32_t LPASS_LPAIF_WRDMA_PER_LENa_DUMP[LPAIF_WRDMA_MAX][LPA_IF_SEQ_MAX];
} hal_audioif_dma_dump_v5_t;

static hal_audioif_dma_dump_v5_t afe_hal_audioif_dma_dump_v5;

static void hal_dma_dump_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq);
#endif

/*==========================================================================
  Function Definitions
========================================================================== */
static void hal_dma_enable_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_config_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);
static void hal_dma_get_dma_curr_addr_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr);
static void hal_dma_clear_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_disable_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_enable_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx);
static void hal_dma_get_dma_int_stc_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint64_t *p_stc);
static void hal_dma_get_dma_fifo_count_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t *fifo_cnt_ptr);


static uint32_t hal_dma_check_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t int_status1, uint32_t int_status2);
static void hal_dma_read_interrupt_status_type0_v5(uint32 *int_status1_ptr, uint32 *int_status2_ptr);
static void hal_dma_clear_interrupt_type0_v5(uint32_t int_status1,uint32_t int_status2 );


static void hal_dma_config_dma_channel_type0_sub_sink_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);
static void hal_dma_config_dma_channel_type0_sub_source_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);
static void hal_dma_config_dma5_channel_type0_sub_source_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr);

static void hal_dma_reset_type0_v5(uint32_t dma_dir, uint32_t dma_idx);

void hal_dma_init_type0_v5(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base, lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr)
{
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hal_dma_init_type0_v5 is called");
  if(NULL == func_table)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "func_table is NULL ====");
    return;
  }

  func_table->afe_hal_dma_enable_dma_channel_fptr = hal_dma_enable_dma_channel_type0_v5;
  func_table->afe_hal_dma_disable_dma_channel_fptr = hal_dma_disable_dma_channel_type0_v5;
  func_table->afe_hal_dma_config_dma_channel_fptr = hal_dma_config_dma_channel_type0_v5;
  func_table->afe_hal_dma_get_dma_curr_addr_fptr = hal_dma_get_dma_curr_addr_type0_v5;
  func_table->afe_hal_dma_disable_dma_interrupt_fptr = hal_dma_disable_dma_interrupt_type0_v5;
  func_table->afe_hal_dma_enable_dma_interrupt_fptr = hal_dma_enable_dma_interrupt_type0_v5;
  func_table->afe_hal_dma_clear_dma_interrupt_fptr = hal_dma_clear_dma_interrupt_type0_v5;
  func_table->afe_hal_dma_get_dma_interrupt_stc_fptr   = hal_dma_get_dma_int_stc_type0_v5;
  func_table->afe_hal_dma_get_dma_fifo_count_fptr = hal_dma_get_dma_fifo_count_type0_v5;

  // interrupt handling
  func_table->afe_hal_dma_check_dma_interrupt_fptr = hal_dma_check_dma_interrupt_type0_v5;
  func_table->afe_hal_dma_read_interrupt_status_fptr = hal_dma_read_interrupt_status_type0_v5;
  func_table->afe_hal_dma_clear_interrupt_fptr = hal_dma_clear_interrupt_type0_v5;

  //ssr handling
  func_table->afe_hal_dma_reset_fptr  = hal_dma_reset_type0_v5;

#ifdef AUDIOIF_REG_DUMP              
  func_table->afe_hal_dma_dump_fptr = hal_dma_dump_type0_v5;
#endif


  intr_index_type0 = intr_index;


  reg_base_type0 = dma_hw_info_ptr->dma_reg_base;
  int_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->dma_int_reg_offset_addr);
  rddma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->rddma_reg_offset_addr);
  wrdma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->wrdma_reg_offset_addr);
  stc_rddma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->stc_rddma_reg_offset_addr);
  stc_wrdma_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->stc_wrdma_reg_offset_addr);
  wrdma5_reg_virtoffset_type0 =  reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->wrdma5_reg_offset_addr);
  stc_wrdma5_reg_virtoffset_type0 = reg_virt_offset(virt_base, dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->stc_wrdma5_reg_offset_addr);


  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dma Type0 [intr_index0:%d] [reg_base_type0:0x%x] [dma_reg_virtoffset_type0:0x%x]", \
        intr_index_type0, reg_base_type0, int_reg_virtoffset_type0);
  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[rddma_reg_virtoffset_type0:0x%x] [wrdma_reg_virtoffset_type0:0x%x] [stc_rddma_reg_virtoffset_type0:0x%x] [stc_wrdma_reg_virtoffset_type0:0x%x]", \
        rddma_reg_virtoffset_type0, wrdma_reg_virtoffset_type0, stc_rddma_reg_virtoffset_type0, stc_wrdma_reg_virtoffset_type0);
  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "[wrdma5_reg_virtoffset_type0:0x%x] [stc_wrdma5_reg_virtoffset_type0:0x%x] ", \
        wrdma5_reg_virtoffset_type0, stc_wrdma5_reg_virtoffset_type0);

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


static int32_t hal_wrdma5_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_WRDMA_CTL5);
}

static int32_t hal_stc_rddma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_RDDMA_STC_LSB0);
}

static int32_t hal_stc_wrdma_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_WRDMA_STC_LSB0);
}

static int32_t hal_stc_wrdma5_register_offset_type0(uint32_t register_addr)
{
  return (register_addr - LPASS_LPAIF_WRDMA_STC_LSB5);
}


static void hal_dma_enable_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    mask = LPASS_LPAIF_RDDMA_CTLa__ENABLE___M;
    field_value = LPASS_LPAIF_RDDMA_CTLa__ENABLE__ON << LPASS_LPAIF_RDDMA_CTLa__ENABLE___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

    //enable dynamic clock control.
    mask = LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK__ON << LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    /* for WRDMA idx 0-4 update LPASS_LPAIF_WRDMA_CTLa
       for WRDMA idx  >4  update  LPASS_LPAIF_WRDMA_CTL5*/
    if (dma_idx <= 4)
    {

      mask = LPASS_LPAIF_WRDMA_CTLa__ENABLE___M;
      field_value = LPASS_LPAIF_WRDMA_CTLa__ENABLE__ON << LPASS_LPAIF_WRDMA_CTLa__ENABLE___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                      wrdma_reg_virtoffset_type0, mask,field_value,0);

      //enable dynamic clock control.
      mask = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___M;
      field_value = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK__ON << LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                      wrdma_reg_virtoffset_type0, mask,field_value,0);
    }
    else
    {
      mask = LPASS_LPAIF_WRDMA_CTL5__ENABLE___M;
      field_value = LPASS_LPAIF_WRDMA_CTL5__ENABLE__ON << LPASS_LPAIF_WRDMA_CTL5__ENABLE___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                      wrdma5_reg_virtoffset_type0, mask,field_value,0);

      //enable dynamic clock control.
      mask = LPASS_LPAIF_WRDMA_CTL5__DYNAMIC_CLOCK___M;
      field_value = LPASS_LPAIF_WRDMA_CTL5__DYNAMIC_CLOCK__ON << LPASS_LPAIF_WRDMA_CTL5__DYNAMIC_CLOCK___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                      wrdma5_reg_virtoffset_type0, mask,field_value,0);

    }
  }
}


static void hal_dma_disable_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    //disable dynamic clock enable.
    mask = LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___M;
    field_value = LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK__OFF << LPASS_LPAIF_RDDMA_CTLa__DYNAMIC_CLOCK___S;
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

    mask = LPASS_LPAIF_RDDMA_CTLa__ENABLE___M | LPASS_LPAIF_RDDMA_CTL0__AUDIO_INTF___M;
    field_value = (LPASS_LPAIF_RDDMA_CTLa__ENABLE__OFF << LPASS_LPAIF_RDDMA_CTLa__ENABLE___S)
                  | (LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__NONE << LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF___S);

    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);
  }
  else
  {
    /* for WRDMA idx 0-4  update  LPASS_LPAIF_WRDMA_CTLa
       for WRDMA idx  >4  update  LPASS_LPAIF_WRDMA_CTL5*/
    if (dma_idx <=4)
    {
      //disable dynamic clock enable.
      mask = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___M;
      field_value = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK__OFF << LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                      wrdma_reg_virtoffset_type0, mask,field_value,0);
      //Add comments that why INTF_INT_M is added
      mask = LPASS_LPAIF_WRDMA_CTLa__ENABLE___M | LPASS_LPAIF_WRDMA_CTL0__AUDIO_INTF___M|LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___M;
      field_value = (LPASS_LPAIF_WRDMA_CTLa__ENABLE__OFF << LPASS_LPAIF_WRDMA_CTLa__ENABLE___S)
                      | (LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__NONE << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___S)
                      |  (LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT__NONE<<LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___S);
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                      wrdma_reg_virtoffset_type0, mask,field_value,0);
    }
    else  //for dma_idx >= 5
    {
      //disable dynamic clock enable.
      mask = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___M;
      field_value = LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK__OFF << LPASS_LPAIF_WRDMA_CTLa__DYNAMIC_CLOCK___S;
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                      wrdma5_reg_virtoffset_type0, mask,field_value,0);

      mask = LPASS_LPAIF_WRDMA_CTLa__ENABLE___M | LPASS_LPAIF_WRDMA_CTL0__AUDIO_INTF___M|LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___M;
      field_value = (LPASS_LPAIF_WRDMA_CTLa__ENABLE__OFF << LPASS_LPAIF_WRDMA_CTLa__ENABLE___S)
                                |(LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__NONE << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___S)
                                |(LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT__NONE<<LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___S);
      UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                      wrdma5_reg_virtoffset_type0, mask,field_value,0);
    }
  }
}

static void hal_dma_config_dma_channel_type0_v5(uint32_t dma_dir, uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    hal_dma_config_dma_channel_type0_sub_sink_v5(dma_idx, dma_config_ptr);
  }
  else
  {
    /* for WRDMA idx 0-4 update LPASS_LPAIF_WRDMA_CTLa for dma configuration
       for WRDMA idx  >4  update  LPASS_LPAIF_WRDMA_CTL5 for dma configuration*/
    if (dma_idx<=4)
    {
      hal_dma_config_dma_channel_type0_sub_source_v5(dma_idx, dma_config_ptr);
    }
    else
    {
      hal_dma_config_dma5_channel_type0_sub_source_v5(dma_idx, dma_config_ptr);
    }


  }
}

static void hal_dma_get_dma_curr_addr_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t *curr_addr)
{
  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CURR_ADDRa(dma_idx)),
                  rddma_reg_virtoffset_type0, (uint32 *)curr_addr);
  }
  else
  {
    /* for WRDMA idx 0-4 update/read/write LPASS_LPAIF_WRDMA_CURR_ADDRa
       for WRDMA idx  >4  update/read/write  LPASS_LPAIF_WRDMA_CURR_ADDR5*/
    if (dma_idx <=4)
    {
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CURR_ADDRa(dma_idx)),
                    wrdma_reg_virtoffset_type0, (uint32 *)curr_addr);
    }
    else
    {
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CURR_ADDR5),
                    wrdma5_reg_virtoffset_type0, (uint32 *)curr_addr);
    }
  }
}



static void hal_dma_clear_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32 mask;
  uint32 clear_mask = 0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      /* For RDDMA <=4, use IRQ_CLEAR macros*/
      case 0:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_CLEARa__UNDR_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH4___M);
        break;
        /* For RDDMA >4, use IRQ2_CLEAR macros*/
      case 5:
        clear_mask = (LPASS_LPAIF_IRQ2_CLEARa__PER_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_CLEARa__UNDR_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_CLEARa__ERR_RDDMA_CH5___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      /* For WRDMA <= 3, use IRQ_CLEAR macros*/
      case 0:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_CLEARa__OVR_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_CLEARa__OVR_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_WRDMA_CH1___M);
        break;
      case 2:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_CLEARa__OVR_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_WRDMA_CH2___M);
        break;
      case 3:
        clear_mask = (LPASS_LPAIF_IRQ_CLEARa__PER_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_CLEARa__OVR_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_CLEARa__ERR_RDDMA_CH3___M);
        break;
        /* For WRDMA > 3, use IRQ2_CLEAR macros*/
      case 4:
        clear_mask = (LPASS_LPAIF_IRQ2_CLEARa__PER_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_CLEARa__OVR_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_CLEARa__ERR_WRDMA_CH4___M);
        break;
      case 5:
        clear_mask = (LPASS_LPAIF_IRQ2_CLEARa__PER_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_CLEARa__OVR_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_CLEARa__ERR_WRDMA_CH5___M);
        break;
      default:
        break;
    }
  }

  //UpdateRegister1 function: it can update whatever in the field value regardless of mask.
  //the mask only zero out value after reading the current value.
  //Therefore, if we use the mask as zero, we do not apply any value in the reg.
  //So that we will not clear any interrupt we don't want.

  /*LPASS_LPAIF_IRQ_CLEARa address controls RDDMA idx from 0 to 4 and WRDDMA IDX from o to 3*/
  /*LPASS_LPAIF_IRQ2_CLEARa address controls RDDMA idx greater than 4 and WRDDMA IDX  greater than 3*/
  mask = 0x0;
  if (((4>=dma_idx)&&(LPASS_HW_DMA_SINK == dma_dir)) ||
      ((3>=dma_idx)&&(LPASS_HW_DMA_SOURCE == dma_dir)))
  {
    ///< This register is write-only
    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_CLEARa(intr_index_type0)),
                    int_reg_virtoffset_type0,mask,clear_mask,1);
  }
  else
  {
    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ2_CLEARa(intr_index_type0)),
                    int_reg_virtoffset_type0,mask,clear_mask,1);
  }

}


static void hal_dma_disable_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t disable_mask=0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      /* For RDDMA <=4, use IRQ_EN macros*/
      case 0:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH4___M);
        break;
        /* For RDDMA >4, use IRQ2_EN macros*/
      case 5:
        disable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__UNDR_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_RDDMA_CH5___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      /* For WRDMA <= 3, use IRQ_EN macros*/
      case 0:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        disable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
        /* For WRDMA > 3, use IRQ2_EN macros*/
      case 4:
        disable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_ENa__OVR_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_WRDMA_CH4___M);
        break;
      case 5:
        disable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__OVR_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_WRDMA_CH5___M);
        break;
      default:
        break;
    }
  }

  /*LPASS_LPAIF_IRQ_ENa can handle 0-4 RDDMA idx, 0-3 WRDMA idx
  //LPASS_LPAIF_IRQ_ENa can handle >4 RDDMA idx, 4-5(>3)  WRDMA idx */
  if (((4>=dma_idx)&&(LPASS_HW_DMA_SINK   ==  dma_dir)) ||
      ((3>=dma_idx)&&(LPASS_HW_DMA_SOURCE == dma_dir)))
  {

    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_ENa(intr_index_type0)),
                    int_reg_virtoffset_type0,disable_mask,0x0,0);
  }
  else
  {
    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ2_ENa(intr_index_type0)),
                    int_reg_virtoffset_type0,disable_mask,0x0,0);
  }

}

static void hal_dma_enable_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t enable_mask = 0;


  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    switch (dma_idx)
    {
      /* For RDDMA <=4, use IRQ_EN macros*/
      case 0:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH0___M);
        break;
      case 1:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH1___M);
        break;
      case 2:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH2___M);
        break;
      case 3:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH3___M);
        break;
      case 4:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_ENa__UNDR_RDDMA_CH4___M
            | LPASS_LPAIF_IRQ_ENa__ERR_RDDMA_CH4___M);
        break;
        /* For RDDMA >4, use IRQ2_EN macros*/
      case 5:
        enable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__UNDR_RDDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_RDDMA_CH5___M);
        break;
      default:
        break;
    }
  }
  else
  {
    switch (dma_idx)
    {
      /* For WRDMA <= 3, use IRQ_EN macros*/
      case 0:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH0___M
            | LPASS_LPAIF_IRQ_ENa__ERR_WRDMA_CH0___M);
        break;
      case 1:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH1___M
            | LPASS_LPAIF_IRQ_ENa__ERR_WRDMA_CH1___M);
        break;
      case 2:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH2___M
            | LPASS_LPAIF_IRQ_ENa__ERR_WRDMA_CH2___M);
        break;
      case 3:
        enable_mask = (LPASS_LPAIF_IRQ_ENa__PER_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__OVR_WRDMA_CH3___M
            | LPASS_LPAIF_IRQ_ENa__ERR_WRDMA_CH3___M);
        break;
        /* For WRDMA > 3, use IRQ2_EN macros*/
      case 4:
        enable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_ENa__OVR_WRDMA_CH4___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_WRDMA_CH4___M);
        break;
      case 5:
        enable_mask = (LPASS_LPAIF_IRQ2_ENa__PER_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__OVR_WRDMA_CH5___M
            | LPASS_LPAIF_IRQ2_ENa__ERR_WRDMA_CH5___M);
        break;
      default:
        break;
    }
  }

  //LPASS_LPAIF_IRQ_ENa can handle 0-4 RDDMA Channels, 0-3 WRDMA channels
  //LPASS_LPAIF_IRQ_ENa can handle >4 RDDMA Channels, 4-5(>3)  WRDMA channels

  if (((4>=dma_idx)&&(LPASS_HW_DMA_SINK   ==  dma_dir)) ||
      ((3>=dma_idx)&&(LPASS_HW_DMA_SOURCE == dma_dir)))
  {
    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_ENa(intr_index_type0)),
                    int_reg_virtoffset_type0, enable_mask,enable_mask,0);
  }
  else
  {
    UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ2_ENa(intr_index_type0)),
                    int_reg_virtoffset_type0, enable_mask,enable_mask,0);
  }

}


static uint32_t hal_dma_check_dma_interrupt_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t int_status1,uint32_t int_status2 )
{
  uint32_t dma_idx_int_bitmask = 0;
  uint32_t dma_int=0;
  uint32_t dma_bit_shift =0;
  uint32_t int_status=0;

  /* for RDDMA channels (0 -4) read interrupt status IRD_STATa[] register
      for RDDMA channels >4, read int status IRQ2_STATa[]*/

  /* for WRDDMA channels (0 -3) read interrupt status IRD_STATa[] register
      for WRDDMA channels >3, read int status IRQ2_STATa[]*/

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    if(dma_idx <=4)
    {
      dma_bit_shift = dma_idx*3;
      int_status  = int_status1;
    }
    else
    {
      /* this condition will hit when RDDMA channels are > 5*/
      /* when DMA channels are >5, DMA status we need to read from IRQ2_STAT register */
      /* +1 is added, because Status starts from bit 1*/
      dma_bit_shift = (dma_idx-4);
      int_status  = int_status2;
    }
  }
  else
  {
    if (dma_idx<=3)
    {
      dma_bit_shift = (dma_idx + 5) * 3;
      int_status  = int_status1;
    }
    else
    {
      /* this condition will hit when WRDMA idx is > 3*/
      /* when WRDMA idx is >3, DMA status we need to read from IRQ2_STAT register */
      dma_bit_shift = ((dma_idx - 3 ) * 3) + 1;
      int_status  = int_status2;
    }

  }

  dma_idx_int_bitmask = 0x7 << dma_bit_shift; //0:0, 1:3, 2:6, 3:9, 4:12, 5:15, 6:18, 7:21, 8: 24.

  dma_int = int_status & dma_idx_int_bitmask;
  if(dma_int)
  {
    dma_int = dma_int >> dma_bit_shift;

    return dma_int; //1, 2, 4 (0x1, 0x2, 0x4)
  }
  return FALSE;
}

static void hal_dma_read_interrupt_status_type0_v5(uint32 *int_status1_ptr, uint32 *int_status2_ptr)
{
  /* Read the interrupt status only for enabled interrupts.*/
  ReadRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_STATa(intr_index_type0)),
                int_reg_virtoffset_type0,int_status1_ptr);

  /*The valid interrupts are 0 to 26 (total 27) for dma*/
  *int_status1_ptr = *int_status1_ptr & 0x7FFFFFF; 

  /* Read the interrupt status only for enabled interrupts.*/ 
  ReadRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ2_STATa(intr_index_type0)),
                int_reg_virtoffset_type0,int_status2_ptr);

  /*The valid interrupts are bit 1 to bit 9  for dma*/
  *int_status2_ptr = *int_status2_ptr & 0x000003FE;                                                                      
}

static void hal_dma_clear_interrupt_type0_v5(uint32_t int_status1,uint32_t int_status2 )
{
  uint32_t mask;

  mask = int_status1;

  ///< This register is write-only
  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ_CLEARa(intr_index_type0)),
                  int_reg_virtoffset_type0,mask,int_status1,1);


  mask = int_status2;

  ///< This register is write-only
  UpdateRegister1(reg_base_type0 + hal_dma_int_register_offset_type0(LPASS_LPAIF_IRQ2_CLEARa(intr_index_type0)),
                  int_reg_virtoffset_type0,mask,int_status2,1);

}


static void hal_dma_get_dma_int_stc_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint64_t *p_stc)
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
    lsb = (lsb & LPASS_LPAIF_RDDMA_STC_LSBa___M) >> LPASS_LPAIF_RDDMA_STC_LSBa___S;
    msb2 = (msb2 & LPASS_LPAIF_RDDMA_STC_MSBa___M) >> LPASS_LPAIF_RDDMA_STC_MSBa___S;
    *p_stc = ((uint64)msb2 << 32) | lsb;
  }
  else
  {
    /* If WRDMA idx <=4 read  LPASS_LPAIF_WRDMA_STC_MSBa, LPASS_LPAIF_WRDMA_STC_LSBa registers*/
    /* If WRDMA idx > 4 read  LPASS_LPAIF_WRDMA_STC_MSB5, LPASS_LPAIF_WRDMA_STC_LSB5 registers*/
    if (dma_idx<=4)
    {
      ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSBa(dma_idx)), stc_wrdma_reg_virtoffset_type0, &msb1);
      ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&lsb);
      ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&msb2);
      if(msb1 != msb2)
      {
        ReadRegister1(reg_base_type0 + hal_stc_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSBa(dma_idx)),stc_wrdma_reg_virtoffset_type0,&lsb);
      }
      lsb = (lsb & LPASS_LPAIF_WRDMA_STC_LSBa___M) >> LPASS_LPAIF_WRDMA_STC_LSBa___S;
      msb2 = (msb2 & LPASS_LPAIF_WRDMA_STC_MSBa___M) >> LPASS_LPAIF_WRDMA_STC_MSBa___S;
      *p_stc = ((uint64)msb2 << 32) | lsb;
    }
    else
    {
      ReadRegister1(reg_base_type0 + hal_stc_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSB5), stc_wrdma5_reg_virtoffset_type0, &msb1);
      ReadRegister1(reg_base_type0 + hal_stc_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSB5),stc_wrdma5_reg_virtoffset_type0,&lsb);
      ReadRegister1(reg_base_type0 + hal_stc_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_STC_MSB5),stc_wrdma5_reg_virtoffset_type0,&msb2);
      if(msb1 != msb2)
      {
        ReadRegister1(reg_base_type0 + hal_stc_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_STC_LSB5),stc_wrdma5_reg_virtoffset_type0,&lsb);
      }
      lsb = (lsb & LPASS_LPAIF_WRDMA_STC_LSB5___M) >> LPASS_LPAIF_WRDMA_STC_LSB5___S;
      msb2 = (msb2 & LPASS_LPAIF_WRDMA_STC_MSB5___M) >> LPASS_LPAIF_WRDMA_STC_MSB5___S;
      *p_stc = ((uint64)msb2 << 32) | lsb;
    }
  }
}


static void hal_dma_get_dma_fifo_count_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t *fifo_cnt_ptr)
{
  uint32_t fifo_count = 0;
  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PERa(dma_idx)),rddma_reg_virtoffset_type0,&fifo_count);
    *fifo_cnt_ptr = (fifo_count & LPASS_LPAIF_RDDMA_PERa__PERIOD_FIFO___M) >> LPASS_LPAIF_RDDMA_PERa__PERIOD_FIFO___S;
  }
  else
  {
    /*If WRDMA idx <=4 read  LPASS_LPAIF_WRDMA_PERa  registers*/
    /* If WRDMA idx > 4 read  LPASS_LPAIF_WRDMA_PER5  registers*/
    if (dma_idx<=4)
    {
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PERa(dma_idx)), wrdma_reg_virtoffset_type0, &fifo_count);
      *fifo_cnt_ptr = (fifo_count & LPASS_LPAIF_WRDMA_PERa__PERIOD_FIFO___M) >> LPASS_LPAIF_WRDMA_PERa__PERIOD_FIFO___S;
    }
    else
    {
      ReadRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_PER5), wrdma5_reg_virtoffset_type0, &fifo_count);
      *fifo_cnt_ptr = (fifo_count & LPASS_LPAIF_WRDMA_PER5__PERIOD_FIFO___M) >> LPASS_LPAIF_WRDMA_PER5__PERIOD_FIFO___S;
    }
  }

}


static void hal_dma_config_dma_channel_type0_sub_sink_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{
  uint32_t mask,field_value;

  ///< Update the base address of the DMA buffer
  mask = LPASS_LPAIF_RDDMA_BASEa__BASE_ADDR___M;
  field_value = ((uint32)dma_config_ptr->buffer_start_addr >> LPASS_LPAIF_RDDMA_BASEa__BASE_ADDR___S)
                               << LPASS_LPAIF_RDDMA_BASEa__BASE_ADDR___S;

  //READ DMA
  UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BASEa(dma_idx)),
                  rddma_reg_virtoffset_type0, mask,field_value,0);


  ///< Update the buffer length of the DMA buffer.
  mask = LPASS_LPAIF_RDDMA_BUFF_LENa__LENGTH___M;
  field_value = (dma_config_ptr->buffer_len-1);


  UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BUFF_LENa(dma_idx)),
                  rddma_reg_virtoffset_type0, mask,field_value,0);

  ///< Update the interrupt sampler period
  mask = LPASS_LPAIF_RDDMA_PER_LENa___M;
  field_value=(dma_config_ptr->dma_int_per_cnt-1)<<LPASS_LPAIF_RDDMA_PER_LENa___S;


  UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PER_LENa(dma_idx)),
                  rddma_reg_virtoffset_type0, mask,field_value,0);

  /**
   * Update the audio interface and fifo Watermark in AUDIO_DMA_CTLa(channel)
   * register. Obtain these values from the input configuration structure
   */
  mask = LPASS_LPAIF_RDDMA_CTLa__BURST_EN___M |
      LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF___M |
      LPASS_LPAIF_RDDMA_CTLa__FIFO_WATERMRK___M |
      LPASS_LPAIF_RDDMA_CTLa__WPSCNT___M;


  if (dma_config_ptr->ifconfig_dma_control) 
  {
    field_value = (dma_config_ptr->ifconfig_dma_control) << LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF___S;
  }
  else
  {
    field_value = (dma_config_ptr->ifconfig_dma_control_int) << LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF___S;
  }

  field_value |=((dma_config_ptr->watermark-1)<<LPASS_LPAIF_RDDMA_CTLa__FIFO_WATERMRK___S);

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   * 8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   * The burst mode needs to be disabled for this case for latency and drift detection issue
   */
  if((LPASS_LPAIF_RDDMA_CTLa__FIFO_WATERMRK__ENUM_8+1) < dma_config_ptr->dma_int_per_cnt)
  {
    //enable BURST basing on burst_size
    switch(dma_config_ptr->burst_size)
    {
      case 4:
      case 8:
      case 16:
        field_value |= (LPASS_LPAIF_RDDMA_CTLa__BURST_EN__INCR4 << LPASS_LPAIF_RDDMA_CTLa__BURST_EN___S);
        break;
      case 1:
        field_value |= (LPASS_LPAIF_RDDMA_CTLa__BURST_EN__SINGLE<<LPASS_LPAIF_RDDMA_CTLa__BURST_EN___S);
        break;
      default:
        break;
    }
  }
  else
  {
    field_value |= (LPASS_LPAIF_RDDMA_CTLa__BURST_EN__SINGLE<<LPASS_LPAIF_RDDMA_CTLa__BURST_EN___S);
  }

  field_value |=  ((dma_config_ptr->wps_count - 1) << LPASS_LPAIF_RDDMA_CTLa__WPSCNT___S);


  UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                  rddma_reg_virtoffset_type0, mask,field_value,0);

}

static void hal_dma_config_dma_channel_type0_sub_source_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{
  uint32_t mask,field_value;

  ///< Update the base address of the DMA buffer
  mask = LPASS_LPAIF_WRDMA_BASEa__BASE_ADDR___M;
  field_value = ((uint32)dma_config_ptr->buffer_start_addr >> LPASS_LPAIF_WRDMA_BASEa__BASE_ADDR___S)
                               << LPASS_LPAIF_WRDMA_BASEa__BASE_ADDR___S;


  //WRITE DMA
  UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BASEa(dma_idx)),
                  wrdma_reg_virtoffset_type0, mask,field_value,0);


  ///< Update the buffer length of the DMA buffer.
  mask = LPASS_LPAIF_WRDMA_BUFF_LENa__LENGTH___M;
  field_value = (dma_config_ptr->buffer_len-1);


  //WRDMA
  UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BUFF_LENa(dma_idx)),
                  wrdma_reg_virtoffset_type0, mask,field_value,0);


  ///< Update the interrupt sampler period
  mask = LPASS_LPAIF_WRDMA_PER_LENa___M;
  field_value=(dma_config_ptr->dma_int_per_cnt-1)<<LPASS_LPAIF_WRDMA_PER_LENa___S;


  UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PER_LENa(dma_idx)),
                  wrdma_reg_virtoffset_type0, mask,field_value,0);


  /**
   * Update the audio interface and fifo Watermark in AUDIO_DMA_CTLa(channel)
   * register. Obtain these values from the input configuration structure
   */
  mask = LPASS_LPAIF_WRDMA_CTLa__BURST_EN___M |
      LPASS_LPAIF_WRDMA_CTLa__FIFO_WATERMRK___M |
      LPASS_LPAIF_WRDMA_CTLa__WPSCNT___M;

  /* for WRDMA check,we need to update INT I2S bits or regular I2S bits*/
  /* This we are checking with ifconfig_dma_control. If it is zero, then internal i2S */
  /* only i2s driver will update  ifconfig_dma_control_int value */ 

  if (dma_config_ptr->ifconfig_dma_control)
  {
    mask |= LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___M;
    field_value = (dma_config_ptr->ifconfig_dma_control) << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___S;
  }
  else 
  {
    mask |= LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___M;
    field_value = (dma_config_ptr->ifconfig_dma_control_int) << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___S;
  }

  field_value |=((dma_config_ptr->watermark-1)<<LPASS_LPAIF_WRDMA_CTLa__FIFO_WATERMRK___S);

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   * 8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   * The burst mode needs to be disabled for this case for latency and drift detection issue
   */
  if((LPASS_LPAIF_WRDMA_CTLa__FIFO_WATERMRK__ENUM_8+1) < dma_config_ptr->dma_int_per_cnt)
  {
    //enable BURST basing on burst_size
    switch(dma_config_ptr->burst_size)
    {
      case 4:
      case 8:
      case 16:
        field_value |= (LPASS_LPAIF_WRDMA_CTLa__BURST_EN__INCR4 << LPASS_LPAIF_WRDMA_CTLa__BURST_EN___S);
        break;
      case 1:
        field_value |= (LPASS_LPAIF_WRDMA_CTLa__BURST_EN__SINGLE<<LPASS_LPAIF_WRDMA_CTLa__BURST_EN___S);
        break;
      default:
        break;
    }
  }
  else
  {
    field_value |= (LPASS_LPAIF_WRDMA_CTLa__BURST_EN__SINGLE<<LPASS_LPAIF_WRDMA_CTLa__BURST_EN___S);
  }

  field_value |=  ((dma_config_ptr->wps_count - 1) << LPASS_LPAIF_WRDMA_CTLa__WPSCNT___S);


  UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                  wrdma_reg_virtoffset_type0, mask,field_value,0);
}

/*This function is same as the above , excpept the offset need to be used different one and also the 
  final register addresses are different, which is not contiguous to WRDMA_CTL0 registers */

static void hal_dma_config_dma5_channel_type0_sub_source_v5(uint32_t dma_idx, hal_dma_config_t *dma_config_ptr)
{
  uint32_t mask,field_value;

  ///< Update the base address of the DMA buffer
  mask = LPASS_LPAIF_WRDMA_BASE5__BASE_ADDR___M;
  field_value = ((uint32)dma_config_ptr->buffer_start_addr >> LPASS_LPAIF_WRDMA_BASE5__BASE_ADDR___S)
                               << LPASS_LPAIF_WRDMA_BASE5__BASE_ADDR___S;


  //WRITE DMA
  UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_BASE5),
                  wrdma5_reg_virtoffset_type0, mask,field_value,0);


  ///< Update the buffer length of the DMA buffer.
  mask = LPASS_LPAIF_WRDMA_BUFF_LEN5__LENGTH___M;
  field_value = (dma_config_ptr->buffer_len-1);


  //WRDMA
  UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_BUFF_LEN5),
                  wrdma5_reg_virtoffset_type0, mask,field_value,0);


  ///< Update the interrupt sampler period
  mask = LPASS_LPAIF_WRDMA_PER_LEN5___M;
  field_value=(dma_config_ptr->dma_int_per_cnt-1)<<LPASS_LPAIF_WRDMA_PER_LEN5___S;


  UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_PER_LEN5),
                  wrdma5_reg_virtoffset_type0, mask,field_value,0);


  /**
   * Update the audio interface and fifo Watermark in AUDIO_DMA_CTLa(channel)
   * register. Obtain these values from the input configuration structure
   */
  mask = LPASS_LPAIF_WRDMA_CTL5__BURST_EN___M |
      LPASS_LPAIF_WRDMA_CTL5__FIFO_WATERMRK___M |
      LPASS_LPAIF_WRDMA_CTL5__WPSCNT___M;

  /* If int I2S interface, different bit positions(LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___S) need to update*/
  /* if  i2s/PCM mux combination interfaces used, LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___S postions need to update*/
  if (dma_config_ptr->ifconfig_dma_control )
  {
    mask |= LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___M;
    field_value = (dma_config_ptr->ifconfig_dma_control) << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF___S;
  }
  else 
  {
    mask |= LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___M;
    field_value = (dma_config_ptr->ifconfig_dma_control_int) << LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF_INT___S;
  }

  field_value |=((dma_config_ptr->watermark-1)<<LPASS_LPAIF_WRDMA_CTL5__FIFO_WATERMRK___S);

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   * 8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   * The burst mode needs to be disabled for this case for latency and drift detection issue
   */
  if((LPASS_LPAIF_WRDMA_CTL5__FIFO_WATERMRK__ENUM_8+1) < dma_config_ptr->dma_int_per_cnt)
  {
    //enable BURST basing on burst_size
    switch(dma_config_ptr->burst_size)
    {
      case 4:
      case 8:
      case 16:
        field_value |= (LPASS_LPAIF_WRDMA_CTL5__BURST_EN__INCR4 << LPASS_LPAIF_WRDMA_CTL5__BURST_EN___S);
        break;
      case 1:
        field_value |= (LPASS_LPAIF_WRDMA_CTL5__BURST_EN__SINGLE<<LPASS_LPAIF_WRDMA_CTL5__BURST_EN___S);
        break;
      default:
        break;
    }
  }
  else
  {
    field_value |= (LPASS_LPAIF_WRDMA_CTL5__BURST_EN__SINGLE<<LPASS_LPAIF_WRDMA_CTL5__BURST_EN___S);
  }

  field_value |=  ((dma_config_ptr->wps_count - 1) << LPASS_LPAIF_WRDMA_CTL5__WPSCNT___S);


  UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                  wrdma5_reg_virtoffset_type0, mask,field_value,0);
}

static void hal_dma_reset_type0_v5(uint32_t dma_dir, uint32_t dma_idx)
{
  uint32_t mask,field_value;

  if(LPASS_HW_DMA_SINK == dma_dir)
  {
    mask = LPASS_LPAIF_RDDMA_CTLa___M;
    field_value =  	LPASS_LPAIF_RDDMA_CTLa___POR; //reset value from IPCAT
    UpdateRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)),
                    rddma_reg_virtoffset_type0, mask,field_value,0);

  }
  else
  {

    mask = LPASS_LPAIF_WRDMA_CTLa___M;
    field_value = LPASS_LPAIF_WRDMA_CTLa___POR; //reset value from IPCAT
    if (dma_idx<=4)
    {
        UpdateRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)),
                        wrdma_reg_virtoffset_type0, mask,field_value,0);
    }
    else
    {
        UpdateRegister1(reg_base_type0 + hal_wrdma5_register_offset_type0(LPASS_LPAIF_WRDMA_CTL5),
                        wrdma5_reg_virtoffset_type0, mask,field_value,0);
    }
  }
}

#ifdef AUDIOIF_REG_DUMP   
static void hal_dma_dump_type0_v5(uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq)
{

  if(LPA_IF_SINK == dma_dir)
  {  //READ DMA
    if(LPAIF_RDDMA_MAX > dma_idx)
    {
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_CTLa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_CTLa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BASEa(dma_idx)),rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_BASEa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_BUFF_LENa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_rddma_register_offset_type0(LPASS_LPAIF_RDDMA_PER_LENa(dma_idx)), rddma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_CTLa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_CTLa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_BASEa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_BASEa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_BUFF_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_RDDMA_PER_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_RDDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
    }
  }
  else
  {  //WRITE DMA

    if(LPAIF_WRDMA_MAX > dma_idx)
    {
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_CTLa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_CTLa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BASEa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_BASEa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_BUFF_LENa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      ReadRegister1(reg_base_type0 + hal_wrdma_register_offset_type0(LPASS_LPAIF_WRDMA_PER_LENa(dma_idx)), wrdma_reg_virtoffset_type0, &afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_CTLa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_CTLa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_BASEa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_BASEa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_BUFF_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_BUFF_LENa_DUMP[dma_idx][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_WRDMA_PER_LENa(%d) = 0x%x", dma_idx, afe_hal_audioif_dma_dump_v5.LPASS_LPAIF_WRDMA_PER_LENa_DUMP[dma_idx][dump_seq]);
    }
  }
}

#endif


