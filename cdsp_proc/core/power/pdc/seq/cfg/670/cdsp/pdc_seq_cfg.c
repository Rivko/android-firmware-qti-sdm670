/*==============================================================================
  FILE:         pdc_seq_cfg.c
  
  OVERVIEW:     This file provides configuration data like sequences, delays, etc for 
  various PDC sequencers handled on a given subsystem.
 
  DEPENDENCIES: None

                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/cfg/670/cdsp/pdc_seq_cfg.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"
#include "pdc_hwio.h"
#include "pdc_config.h"

/*==========================================================================
 *                          PDC CONFIGURATIONS
 *=========================================================================*/
static uint32 g_pdcSequence[CSS_PDC_NUMSEQWORDS] = 
{
//CX_MIN_AOSS:
  0xbfa0a1e1 ,  //0x00: 0xe1      WAIT1     rx_fifo_empty                 
                //0x01: 0xa1      SET1      pdc_mode_control              
                //0x02: 0xa0      SET1      timer_match_valid             
                //0x03: 0xbf      SET1      seq_tcs_0_start               
  0xa5a4a2ff ,  //0x04: 0xff      WAIT1     tcs_0_seq_cmp                 
                //0x05: 0xa2      SET1      soc_sleep_req                 
                //0x06: 0xa4      SET1      dbg_resource_rel_req          
                //0x07: 0xa5      SET1      wfi_profiling_unit_3          
  0x8482a6e0 ,  //0x08: 0xe0      WAIT1     sequence_resume               
                //0x09: 0xa6      SET1      generic_profiling_unit_5      
                //0x0a: 0x82      SET0      soc_sleep_req                 
                //0x0b: 0x84      SET0      dbg_resource_rel_req          
  0xa9fdbde2 ,  //0x0c: 0xe2      WAIT1     soc_sleep_unlock              
                //0x0d: 0xbd      SET1      seq_tcs_2_start               
                //0x0e: 0xfd      WAIT1     tcs_2_seq_cmp                 
                //0x0f: 0xa9      SET1      pdc_aop_irq_1                 
  0xa7818089 ,  //0x10: 0x89      SET0      pdc_aop_irq_1                 
                //0x11: 0x80      SET0      timer_match_valid             
                //0x12: 0x81      SET0      pdc_mode_control              
                //0x13: 0xa7      SET1      wakeup_req                    
  0xe120e787 ,  //0x14: 0x87      SET0      wakeup_req                    
                //0x15: 0xe7      WAIT1     wakeup_ack                    
                //0x16: 0x20      RETURN    
//CX_COL_AOSS:
                //0x17: 0xe1      WAIT1     rx_fifo_empty                 
  0xfebea0a1 ,  //0x18: 0xa1      SET1      pdc_mode_control              
                //0x19: 0xa0      SET1      timer_match_valid             
                //0x1a: 0xbe      SET1      seq_tcs_1_start               
                //0x1b: 0xfe      WAIT1     tcs_1_seq_cmp                 
  0xa5a4a2a3 ,  //0x1c: 0xa3      SET1      cx_collapse_req               
                //0x1d: 0xa2      SET1      soc_sleep_req                 
                //0x1e: 0xa4      SET1      dbg_resource_rel_req          
                //0x1f: 0xa5      SET1      wfi_profiling_unit_3          
  0x8382a6e0 ,  //0x20: 0xe0      WAIT1     sequence_resume               
                //0x21: 0xa6      SET1      generic_profiling_unit_5      
                //0x22: 0x82      SET0      soc_sleep_req                 
                //0x23: 0x83      SET0      cx_collapse_req               
  0xbde3e284 ,  //0x24: 0x84      SET0      dbg_resource_rel_req          
                //0x25: 0xe2      WAIT1     soc_sleep_unlock              
                //0x26: 0xe3      WAIT1     cx_collapse_unlock            
                //0x27: 0xbd      SET1      seq_tcs_2_start               
  0x8089a9fd ,  //0x28: 0xfd      WAIT1     tcs_2_seq_cmp                 
                //0x29: 0xa9      SET1      pdc_aop_irq_1                 
                //0x2a: 0x89      SET0      pdc_aop_irq_1                 
                //0x2b: 0x80      SET0      timer_match_valid             
  0xe787a781 ,  //0x2c: 0x81      SET0      pdc_mode_control              
                //0x2d: 0xa7      SET1      wakeup_req                    
                //0x2e: 0x87      SET0      wakeup_req                    
                //0x2f: 0xe7      WAIT1     wakeup_ack                    
  0x00000020 ,  //0x30: 0x20      RETURN
};

/* Branch addresses - dummy for testing */
static uint8 g_PDCBranches[PDC_SEQ_HAL_BR_ADDR_REG_COUNT] = { 0x0 };

/* Delay values - dummy for testing */
static uint32 g_pdcDelays[PDC_SEQ_HAL_DELAY_REG_COUNT] =  { 0x0 };

static pdc_seq_hal_bsp_cfg_t g_pdcCfg = 
{
  PDC_SEQ_ARRAY (g_PDCBranches),
  PDC_SEQ_ARRAY (g_pdcDelays)
};

/* PDC supported low power mode */
static pdc_seq_hal_bsp_mode_t g_pdcModes[] =
{
  {
    PDC_MODE_CX_MIN_AOSS,   /* Mode id */
    CSS_PDC_CX_MIN_AOSS     /* Start Address */
  },
  {
    PDC_MODE_CX_COL_AOSS,
    CSS_PDC_CX_COL_AOSS
  },
};

pdc_seq_t g_pdcSeqInstance = 
{
  &g_pdcCfg,
  PDC_SEQ_ARRAY(g_pdcModes),
  PDC_SEQ_ARRAY(g_pdcSequence),
  PDC_HWIO_BASE_ADDR,           /* Subsystem PDC base */
  RSC_HWIO_BASE_ADDR            /* Subsystem RSCp base */
};

pdc_seq_t g_pdcSeqInstanceV2 =
{
  &g_pdcCfg,
  PDC_SEQ_ARRAY(g_pdcModes),
  PDC_SEQ_ARRAY(g_pdcSequence),
  PDC_HWIO_BASE_ADDR - 0x20000,  /* PDC base - V2 differs from V1 */
  RSC_HWIO_BASE_ADDR             /* RSCp base */
};
