/*==============================================================================
  FILE:         rsc_hal.c
  
  OVERVIEW:     This file implements HAL APIs exposed to rsc driver and performs
                actual HW register read and write.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/rsc/hal/src/rsc_hal.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "rsc_hwio.h"
#include "rsc_seq_hal.h"
#include "rsc_HALhwio.h"

/*===========================================================================
 *                            EXTERNAL FUNCTIONS
 *==========================================================================*/
/*
 * rsc_hal_get_num_seq_cmd_words
 */
uint32 rsc_hal_get_num_seq_cmd_words(uint32 base)
{
  return RSC_HWIO_INXFI(base, RSC_PARAM_RSC_CONFIG_DRVd, 0, NUM_SEQ_CMD_WORDS);
}

/*
 * rsc_hal_enable_ts_unit
 */
void rsc_hal_enable_ts_unit(uint32 base, uint8 unit_num, boolean enable)
{
  RSC_HWIO_OUTXFI2(base, RSC_TIMESTAMP_UNITm_EN_DRVd, 0, unit_num, ENABLE, enable);
}

/* 
 * rsc_hal_get_ts_unit_l
 */
uint32 rsc_hal_get_ts_unit_l(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd, 0, unit_num, TS_DATA_L);
}  

/* 
 * rsc_hal_get_ts_unit_h
 */
uint32 rsc_hal_get_ts_unit_h(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd, 0, unit_num, TS_DATA_H);
}

/* 
 * rsc_hal_get_ts_overflow
 */
uint32 rsc_hal_get_ts_overflow(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_OVERFLOW);
}

/*
 * rsc_hal_get_ts_valid
 */
uint32 rsc_hal_get_ts_valid(uint32 base, uint8 unit_num)
{
  return RSC_HWIO_INXFI2(base, RSC_TIMESTAMP_UNITm_OUTPUT_DRVd, 0, unit_num, TS_VALID);
}

/*
 * rsc_hal_set_seq_override_start_addr
 */
void rsc_hal_set_seq_override_start_addr(uint32 base, uint8 addr)
{
  RSC_HWIO_OUTXF(base, RSC_SEQ_OVERRIDE_START_ADDR_DRV0, ADDR, addr); 
}

/*
 * rsc_hal_set_seq_override_start_addr_valid
 */
void rsc_hal_set_seq_override_start_addr_valid(uint32 base, uint8 valid)
{
  HWIO_OUTXF(base, RSC_SEQ_OVERRIDE_START_ADDR_DRV0, VALID, valid); 
}

/*
 * rsc_hal_config_br_addr
 */
void rsc_hal_config_br_addr(uint32 base, uint32 idx, uint16 val)
{
  RSC_HWIO_OUTXI(base, RSC_SEQ_CFG_BR_ADDR_i_DRV0, idx, val);
}  

/*
 * rsc_hal_copy_cmd_seq
 */
uint16 rsc_hal_copy_cmd_seq(uint32 rscBase, uint32 *cmds, uint16 size)
{
  uint32 nCmd;

  if(size > rsc_hal_get_num_seq_cmd_words(rscBase))
  {
    return 0;
  }

  for(nCmd = 0; nCmd < size; nCmd++)
  {
    RSC_HWIO_OUTXI(rscBase, SEQ_MEM_m_DRV0, nCmd, cmds[nCmd]);
  }

  return size;
}

/*
 * rsc_hal_set_br_event
 */
void rsc_hal_set_br_event(uint32 base, uint16 val)
{
#ifdef RSC_BR_EVENT_PREFIX
  HWIO_OUT(HWIO_CONCAT_NAME(RSC_BR_EVENT_PREFIX, WRAPPER_RSCC_BR_EVENT), val);
#endif

  return;
}  

