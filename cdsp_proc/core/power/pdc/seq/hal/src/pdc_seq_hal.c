/*==============================================================================
  FILE:         pdc_seq_hal.c

  OVERVIEW:     This file implements the HAL interface for PDC sequencer.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/hal/src/pdc_seq_hal.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "pdc_HALhwioBase.h"
#include "pdc_hwio.h"

/*==============================================================================
                                GLOBAL FUNCTIONS
 =============================================================================*/
/*
 * pdc_seq_hal_enable
 */
void pdc_seq_hal_enable(uint32 pdc_base, uint8 enable)
{
  enable = (enable != FALSE) ? 1 : 0;
  PDC_HWIO_OUTXF (pdc_base, ENABLE_PDC, ENABLE_PDC, enable);
}

/* 
 * pdc_seq_hal_clk_gate_enable
 */
void pdc_seq_hal_clk_gate_enable(uint32 pdc_base, uint8 enable)
{
  uint8 disable = enable ? 0 : 1;
  PDC_HWIO_OUTXF (pdc_base, ENABLE_PDC, DISABLE_CLK_GATE, disable);
}

/* 
 * pdc_seq_hal_cfg_wakeup_time
 */
void pdc_seq_hal_cfg_wakeup_time(uint32 rsc_base, uint64 wakeup_time)
{
  const uint32  mask        = (uint32)(-1);
  const uint32  valid_mask  = RSC_HWIO_FMSK (HIDDEN_TCS_CMD0_DATA_DRVd, PDC_MATCH_VALUE_VALID);
  uint32        low         = wakeup_time & mask;
  uint32        high        = (wakeup_time >> 32) & mask;

  high |= valid_mask;

  RSC_HWIO_OUTXI (rsc_base, HIDDEN_TCS_CMD1_DATA_DRVd, 0, low);
  RSC_HWIO_OUTXI (rsc_base, HIDDEN_TCS_CMD0_DATA_DRVd, 0, high);
}

/* 
 * pdc_seq_hal_cfg_wakeup_time_ex
 */
void pdc_seq_hal_cfg_wakeup_time_ex(uint32 pdc_base, uint64 wakeup_time)
{
  const uint32  mask  = (uint32)(-1);
  uint32        low   = wakeup_time & mask;
  uint32        high  = (wakeup_time >> 32) & mask;

  PDC_HWIO_OUTX (pdc_base, TIMER_MATCH_VALUE_LO, low);
  PDC_HWIO_OUTX (pdc_base, TIMER_MATCH_VALUE_HI, high);
}

/*
 * pdc_seq_hal_cfg_start_addr
 */
void pdc_seq_hal_cfg_start_addr(uint32 rsc_base, uint16 start_addr)
{
  uint32 start_addr_tcs = start_addr |
   RSC_HWIO_FMSK (HIDDEN_TCS_CMD2_DATA_DRV0, PDC_SEQ_START_ADDR_VALID);
   
  RSC_HWIO_OUTX (rsc_base, HIDDEN_TCS_CMD2_DATA_DRV0, start_addr_tcs);
}

/*
 * pdc_seq_hal_cfg_start_addr_ex
 */
void pdc_seq_hal_cfg_start_addr_ex(uint32 pdc_base, uint16 start_addr)
{
  PDC_HWIO_OUTXF (pdc_base, SEQ_START_ADDR, SEQ_START_ADDR, start_addr);
}

/*
 * pdc_seq_hal_cfg_bf_addr
 */
int pdc_seq_hal_cfg_br_addr(uint32 pdc_base, uint16 br_index, uint8 addr)
{
  if (br_index >= PDC_SEQ_HAL_BR_ADDR_REG_COUNT)
  {
    return PDC_SEQ_HAL_INVALID_INDEX;
  }

  PDC_HWIO_OUTXI (pdc_base, SEQ_CFG_BR_ADDR_b, br_index, addr);
  return 0;
}

/*
 * pdc_seq_hal_cfg_delay
 */
int pdc_seq_hal_cfg_delay(uint32 pdc_base, uint16 delay_index, uint32 delay)
{
  if (delay_index >= PDC_SEQ_HAL_DELAY_REG_COUNT)
  {
    return PDC_SEQ_HAL_INVALID_INDEX;
  }

  PDC_HWIO_OUTXI (pdc_base, SEQ_CFG_DELAY_VAL_v, delay_index, delay);
  return 0;
}

/*
 * pdc_seq_hal_copy_cmd_seq
 */
uint16 pdc_seq_hal_copy_cmd_seq(uint32 pdc_base, uint32 *cmds, uint16 length)
{
  uint32 nCmd;

  if(length > PDC_HWIO_INXFI(pdc_base, PDC_PARAM_SEQ_CONFIG_DRVd, 0, BLK_NUM_SEQ_CMD_WORDS))
  {
    return 0;
  }

  for(nCmd = 0; nCmd < length; nCmd++)
  {
    PDC_HWIO_OUTXI(pdc_base + PDC_SEQ_MEM_BASE_OFFSET, SEQ_MEM_m, nCmd, cmds[nCmd]);
  }

  return length;
}

