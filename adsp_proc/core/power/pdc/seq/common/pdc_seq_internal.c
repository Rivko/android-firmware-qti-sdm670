/*==============================================================================
  FILE:         pdc_seq_internal.c

  OVERVIEW:     This file implements some common, internal and utility functions for 
                PDC sequencer driver.

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/seq/common/pdc_seq_internal.c#2 $
$DateTime: 2017/10/12 22:35:09 $
==============================================================================*/
#include "comdef.h"
#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"

/*=============================================================================
                                GLOBAL FUNCTIONS
 =============================================================================*/
/*
 * pdc_seq_handle_internal_cfg
 */
pdc_seq_result_t pdc_seq_handle_internal_cfg(void)
{
  uint32                      i;
  int                         ret_val;
  const pdc_seq_hal_bsp_cfg_t *cfg = g_pdcSeq->cfg;

  /* PDC branch configuration */
  for (i = 0; i < cfg->br_count; i++)
  {
    ret_val = pdc_seq_hal_cfg_br_addr(g_pdcSeq->addr, i, cfg->br_addr[i]);
    if (ret_val)
    {
      return PDC_SEQ_INVALID_PARAM;
    }
  }

  /* PDC delay configuration */
  for (i = 0; i < cfg->delay_count; i++)
  {
    ret_val = pdc_seq_hal_cfg_delay(g_pdcSeq->addr, i, cfg->delay[i]);
    if (ret_val)
    {
      return PDC_SEQ_INVALID_PARAM;
    }
  }

  return PDC_SEQ_SUCCESS;
}

/* 
 * pdc_seq_copy_cmd_seq
 */
pdc_seq_result_t pdc_seq_copy_cmd_seq(void)
{
  uint32            copied_cmds;
  pdc_seq_result_t  result = PDC_SEQ_SUCCESS;
  
  copied_cmds = pdc_seq_hal_copy_cmd_seq (g_pdcSeq->addr, g_pdcSeq->cmds, g_pdcSeq->cmd_length);
  if (copied_cmds == 0)
  {
    result = PDC_SEQ_NO_MEM;
  }

  return result;
}

/*
 * pdc_seq_mode_index
 */
int pdc_seq_mode_index(pdc_low_power_mode_type mode_id)
{
  uint32                  i;
  pdc_seq_hal_bsp_mode_t *mode;

  for (i = 0; i < g_pdcSeq->mode_count; i++)
  {
    mode = &g_pdcSeq->modes[i];
    if (mode->mode_id == mode_id)
    {
      return i;
    }
  }

  return PDC_SEQ_UNSUPPORTED_MODE;
}

