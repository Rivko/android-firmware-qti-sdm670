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
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/seq/cfg/670/adsp/pdc_seq_cfg.c#1 $
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
static uint32 g_pdcSequence[SSC_PDC_NUMSEQWORDS] = 
{
//SSC_GDSC_AOSS:
  0xf2b20d3a ,  //0x00: 0x3a      CALL      common_sleep_seq0             
                //0x01: 0xd       DELI      100048ns                      
                //0x02: 0xb2      SET1      addl_seq_pwr_ctrl_4_gdsc_collapse_en
                //0x03: 0xf2      WAIT1     addl_seq_wait_event_4_gdsc_collapse_done
  0xa238ffbf ,  //0x04: 0xbf      SET1      seq_tcs_0_start               
                //0x05: 0xff      WAIT1     tcs_0_seq_cmp                 
                //0x06: 0x38      CALL      pmux_switch_to_soc_mx         
                //0x07: 0xa2      SET1      soc_sleep_req                 
  0xa6e0a5a4 ,  //0x08: 0xa4      SET1      dbg_resource_rel_req          
                //0x09: 0xa5      SET1      wfi_profiling_unit_3          
                //0x0a: 0xe0      WAIT1     sequence_resume               
                //0x0b: 0xa6      SET1      generic_profiling_unit_5      
  0x02e28482 ,  //0x0c: 0x82      SET0      soc_sleep_req                 
                //0x0d: 0x84      SET0      dbg_resource_rel_req          
                //0x0e: 0xe2      WAIT1     soc_sleep_unlock              
                //0x0f: 0x2       DELI      104ns                         
  0x92fdbd39 ,  //0x10: 0x39      CALL      pmux_switch_to_vdd_mx         
                //0x11: 0xbd      SET1      seq_tcs_2_start               
                //0x12: 0xfd      WAIT1     tcs_2_seq_cmp                 
                //0x13: 0x92      SET0      addl_seq_pwr_ctrl_4_gdsc_collapse_en
  0x3a203bd2 ,  //0x14: 0xd2      WAIT0     addl_seq_wait_event_4_gdsc_collapse_done
                //0x15: 0x3b      CALL      common_wakeup_seq0            
                //0x16: 0x20      RETURN    
//SSC_GDSC_NOAOSS:
                //0x17: 0x3a      CALL      common_sleep_seq0             
  0xbff2b20d ,  //0x18: 0xd       DELI      100048ns                      
                //0x19: 0xb2      SET1      addl_seq_pwr_ctrl_4_gdsc_collapse_en
                //0x1a: 0xf2      WAIT1     addl_seq_wait_event_4_gdsc_collapse_done
                //0x1b: 0xbf      SET1      seq_tcs_0_start               
  0xa5a438ff ,  //0x1c: 0xff      WAIT1     tcs_0_seq_cmp                 
                //0x1d: 0x38      CALL      pmux_switch_to_soc_mx         
                //0x1e: 0xa4      SET1      dbg_resource_rel_req          
                //0x1f: 0xa5      SET1      wfi_profiling_unit_3          
  0x0284a6e0 ,  //0x20: 0xe0      WAIT1     sequence_resume               
                //0x21: 0xa6      SET1      generic_profiling_unit_5      
                //0x22: 0x84      SET0      dbg_resource_rel_req          
                //0x23: 0x2       DELI      104ns                         
  0x92fdbd39 ,  //0x24: 0x39      CALL      pmux_switch_to_vdd_mx         
                //0x25: 0xbd      SET1      seq_tcs_2_start               
                //0x26: 0xfd      WAIT1     tcs_2_seq_cmp                 
                //0x27: 0x92      SET0      addl_seq_pwr_ctrl_4_gdsc_collapse_en
  0xe1203bd2 ,  //0x28: 0xd2      WAIT0     addl_seq_wait_event_4_gdsc_collapse_done
                //0x29: 0x3b      CALL      common_wakeup_seq0            
                //0x2a: 0x20      RETURN    
//SSC_GDSC_AOSS_NOTIMER:
                //0x2b: 0xe1      WAIT1     rx_fifo_empty                 
  0xb20d80a1 ,  //0x2c: 0xa1      SET1      pdc_mode_control              
                //0x2d: 0x80      SET0      timer_match_valid             
                //0x2e: 0xd       DELI      100048ns                      
                //0x2f: 0xb2      SET1      addl_seq_pwr_ctrl_4_gdsc_collapse_en
  0x38ffbff2 ,  //0x30: 0xf2      WAIT1     addl_seq_wait_event_4_gdsc_collapse_done
                //0x31: 0xbf      SET1      seq_tcs_0_start               
                //0x32: 0xff      WAIT1     tcs_0_seq_cmp                 
                //0x33: 0x38      CALL      pmux_switch_to_soc_mx         
  0xe0a5a4a2 ,  //0x34: 0xa2      SET1      soc_sleep_req                 
                //0x35: 0xa4      SET1      dbg_resource_rel_req          
                //0x36: 0xa5      SET1      wfi_profiling_unit_3          
                //0x37: 0xe0      WAIT1     sequence_resume               
  0xe28482a6 ,  //0x38: 0xa6      SET1      generic_profiling_unit_5      
                //0x39: 0x82      SET0      soc_sleep_req                 
                //0x3a: 0x84      SET0      dbg_resource_rel_req          
                //0x3b: 0xe2      WAIT1     soc_sleep_unlock              
  0xfdbd3902 ,  //0x3c: 0x2       DELI      104ns                         
                //0x3d: 0x39      CALL      pmux_switch_to_vdd_mx         
                //0x3e: 0xbd      SET1      seq_tcs_2_start               
                //0x3f: 0xfd      WAIT1     tcs_2_seq_cmp                 
  0x203bd292 ,  //0x40: 0x92      SET0      addl_seq_pwr_ctrl_4_gdsc_collapse_en
                //0x41: 0xd2      WAIT0     addl_seq_wait_event_4_gdsc_collapse_done
                //0x42: 0x3b      CALL      common_wakeup_seq0            
                //0x43: 0x20      RETURN    
//pmux_switch_to_soc_mx:
  0x048eafb1 ,  //0x44: 0xb1      SET1      addl_seq_pwr_ctrl_3_mx_pmux_ctrl_clk_on
                //0x45: 0xaf      SET1      addl_seq_pwr_ctrl_1_mx_act_br1
                //0x46: 0x8e      SET0      addl_seq_pwr_ctrl_0_mx_act_br0
                //0x47: 0x4       DELI      520ns                         
  0x91f090b0 ,  //0x48: 0xb0      SET1      addl_seq_pwr_ctrl_2_mx_switch 
                //0x49: 0x90      SET0      addl_seq_pwr_ctrl_2_mx_switch 
                //0x4a: 0xf0      WAIT1     addl_seq_wait_event_2_mx_trans_done
                //0x4b: 0x91      SET0      addl_seq_pwr_ctrl_3_mx_pmux_ctrl_clk_on
  0xae8fb120 ,  //0x4c: 0x20      RETURN    
//pmux_switch_to_vdd_mx:
                //0x4d: 0xb1      SET1      addl_seq_pwr_ctrl_3_mx_pmux_ctrl_clk_on
                //0x4e: 0x8f      SET0      addl_seq_pwr_ctrl_1_mx_act_br1
                //0x4f: 0xae      SET1      addl_seq_pwr_ctrl_0_mx_act_br0
  0xf090b004 ,  //0x50: 0x4       DELI      520ns                         
                //0x51: 0xb0      SET1      addl_seq_pwr_ctrl_2_mx_switch 
                //0x52: 0x90      SET0      addl_seq_pwr_ctrl_2_mx_switch 
                //0x53: 0xf0      WAIT1     addl_seq_wait_event_2_mx_trans_done
  0xa1e12091 ,  //0x54: 0x91      SET0      addl_seq_pwr_ctrl_3_mx_pmux_ctrl_clk_on
                //0x55: 0x20      RETURN    
//common_sleep_seq0:
                //0x56: 0xe1      WAIT1     rx_fifo_empty                 
                //0x57: 0xa1      SET1      pdc_mode_control              
  0x818020a0 ,  //0x58: 0xa0      SET1      timer_match_valid             
                //0x59: 0x20      RETURN    
//common_wakeup_seq0:
                //0x5a: 0x80      SET0      timer_match_valid             
                //0x5b: 0x81      SET0      pdc_mode_control              
  0x20e787a7 ,  //0x5c: 0xa7      SET1      wakeup_req                    
                //0x5d: 0x87      SET0      wakeup_req                    
                //0x5e: 0xe7      WAIT1     wakeup_ack                    
                //0x5f: 0x20      RETURN    
};

static uint8 g_PDCBranches[PDC_SEQ_HAL_BR_ADDR_REG_COUNT] = 
{
 0x44 ,	// pmux_switch_to_soc_mx = 68 + BRANCH_ADDR: 0x0
 0x4d ,	// pmux_switch_to_vdd_mx = 77 + BRANCH_ADDR: 0x0
 0x56 ,	// common_sleep_seq0 = 86 + BRANCH_ADDR: 0x0
 0x5a ,	// common_wakeup_seq0 = 90 + BRANCH_ADDR: 0x0
};

/* Delay values - dummy for testing */
static uint32 g_pdcDelays[PDC_SEQ_HAL_DELAY_REG_COUNT] = { 0x0 };

static pdc_seq_hal_bsp_cfg_t g_pdcCfg = 
{
  PDC_SEQ_ARRAY (g_PDCBranches),
  PDC_SEQ_ARRAY (g_pdcDelays)
};

/* PDC supported low power mode */
static pdc_seq_hal_bsp_mode_t g_pdcModes[] =
{
  {
    PDC_MODE_CX_COL_AOSS,   /* Mode id */
    SSC_PDC_SSC_GDSC_AOSS   /* Start Address */
  },
{
    PDC_MODE_CX_COL_AOSS_NOTIMER,
    SSC_PDC_SSC_GDSC_AOSS_NOTIMER
  },
};

pdc_seq_t g_pdcSeqInstance = 
{
  &g_pdcCfg,
  PDC_SEQ_ARRAY(g_pdcModes),
  PDC_SEQ_ARRAY(g_pdcSequence),
  PDC_HWIO_BASE_ADDR,             //Target PDC base
  RSC_HWIO_BASE_ADDR              //Target subsystem RSCp base
};
