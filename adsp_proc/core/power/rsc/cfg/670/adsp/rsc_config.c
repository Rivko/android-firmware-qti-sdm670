/*==============================================================================
  FILE:         rsc_config.c

  OVERVIEW:     This file contains target specific data/configuration for rsc
                instances.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "rsc.h"
#include "rsc_config.h"
#include "rsc_common.h"
#include "rsc_hwio.h"

/*==========================================================================
 *                        QDSP6 RSC CONFIGURATIONS
 *=========================================================================*/
uint32 g_rscChildSequence[Q6SS_RSCC_SSC_LPASS_NUMSEQWORDS] = 
{
//BUS_ISO_SEQ:
  0x70085016 ,  //0x00: 0x5016    SET1      rsc_intf_haltreq              
                //0x02: 0x7008    WAIT1     rsc_intf_haltack_event        
  0x7003501c ,  //0x04: 0x501C    SET1      rsc_intf_clk_halt             
                //0x06: 0x7003    WAIT1     rsc_core_clk_off_event        
  0x50185017 ,  //0x08: 0x5017    SET1      rsc_intf_clamp_l_set          
                //0x0a: 0x5018    SET1      rsc_intf_clamp_e_set          
  0x4016401c ,  //0x0c: 0x401C    SET0      rsc_intf_clk_halt             
                //0x0e: 0x4016    SET0      rsc_intf_haltreq              
  0x501c1000 ,  //0x10: 0x1000    RETURN    
//BUS_DEISO_SEQ:
                //0x12: 0x501C    SET1      rsc_intf_clk_halt             
  0x501a7003 ,  //0x14: 0x7003    WAIT1     rsc_core_clk_off_event        
                //0x16: 0x501A    SET1      rsc_intf_clamp_e_clear        
  0x401b501b ,  //0x18: 0x501B    SET1      rsc_intf_fifo_areset          
                //0x1a: 0x401B    SET0      rsc_intf_fifo_areset          
  0x401c5019 ,  //0x1c: 0x5019    SET1      rsc_intf_clamp_l_clear        
                //0x1e: 0x401C    SET0      rsc_intf_clk_halt             
  0x50041000 ,  //0x20: 0x1000    RETURN    
//SHORT_APCR_SEQ_LPASS_SSC:
                //0x22: 0x5004    SET1      rsc_clk_halt                  
  0x00015005 ,  //0x24: 0x5005    SET1      rsc_clamp_io                  
                //0x26: 0x0001    DELI      52ns                          
  0x40065006 ,  //0x28: 0x5006    SET1      rsc_save                      
                //0x2a: 0x4006    SET0      rsc_save                      
  0x50095008 ,  //0x2c: 0x5008    SET1      rsc_clamp_wl                  
                //0x2e: 0x5009    SET1      rsc_mem_periph_slp            
  0x500b7009 ,  //0x30: 0x7009    WAIT1     rsc_mem_periph_pd_event       
                //0x32: 0x500B    SET1      rsc_clamp_qmc_mem             
  0x6006500c ,  //0x34: 0x500C    SET1      rsc_bhs_off                   
                //0x36: 0x6006    WAIT0     rsc_bhs_event                 
  0x70005000 ,  //0x38: 0x5000    SET1      rsc_seq_wait_wakeup           
                //0x3a: 0x7000    WAIT1     rsc_core_rsc_bringup_req      
  0x400c5003 ,  //0x3c: 0x5003    SET1      rsc_ares                      
                //0x3e: 0x400C    SET0      rsc_bhs_off                   
  0x400b7006 ,  //0x40: 0x7006    WAIT1     rsc_bhs_event                 
                //0x42: 0x400B    SET0      rsc_clamp_qmc_mem             
  0x70044009 ,  //0x44: 0x4009    SET0      rsc_mem_periph_slp            
                //0x46: 0x7004    WAIT1     rsc_mem_periph_ready_event    
  0x50074008 ,  //0x48: 0x4008    SET0      rsc_clamp_wl                  
                //0x4a: 0x5007    SET1      rsc_restore                   
  0x40054007 ,  //0x4c: 0x4007    SET0      rsc_restore                   
                //0x4e: 0x4005    SET0      rsc_clamp_io                  
  0x50154003 ,  //0x50: 0x4003    SET0      rsc_ares                      
                //0x52: 0x5015    SET1      rsc_pm_irq                    
  0x40044015 ,  //0x54: 0x4015    SET0      rsc_pm_irq                    
                //0x56: 0x4004    SET0      rsc_clk_halt                  
  0x50221000 ,  //0x58: 0x1000    RETURN    
//LONG_APCR_SEQ_TCS:
                //0x5a: 0x5022    SET1      rsc_parent_tcs_trigger_2      
  0x40227022 ,  //0x5c: 0x7022    WAIT1     rsc_parent_tcs_event_2        
                //0x5e: 0x4022    SET0      rsc_parent_tcs_trigger_2      
  0x70035004 ,  //0x60: 0x5004    SET1      rsc_clk_halt                  
                //0x62: 0x7003    WAIT1     rsc_core_clk_off_event        
  0x00015005 ,  //0x64: 0x5005    SET1      rsc_clamp_io                  
                //0x66: 0x0001    DELI      52ns                          
  0x40065006 ,  //0x68: 0x5006    SET1      rsc_save                      
                //0x6a: 0x4006    SET0      rsc_save                      
  0x50095008 ,  //0x6c: 0x5008    SET1      rsc_clamp_wl                  
                //0x6e: 0x5009    SET1      rsc_mem_periph_slp            
  0x500a7009 ,  //0x70: 0x7009    WAIT1     rsc_mem_periph_pd_event       
                //0x72: 0x500A    SET1      rsc_mem_core_slp              
  0x500b700a ,  //0x74: 0x700A    WAIT1     rsc_mem_core_pd_event         
                //0x76: 0x500B    SET1      rsc_clamp_qmc_mem             
  0x6006500c ,  //0x78: 0x500C    SET1      rsc_bhs_off                   
                //0x7a: 0x6006    WAIT0     rsc_pwr_on_event              
  0x5010501e ,  //0x7c: 0x501E    SET1      rsc_ext_xo_enable             
                //0x7e: 0x5010    SET1      rsc_pll_outdis                
  0x50135011 ,  //0x80: 0x5011    SET1      rsc_pll_opmode0_stby          
                //0x82: 0x5013    SET1      rsc_pll_reset                 
  0x50015014 ,  //0x84: 0x5014    SET1      rsc_clamp_mss2cx              
                //0x86: 0x5001    SET1      rsc_child_parent_shutdown_req 
  0x40017001 ,  //0x88: 0x7001    WAIT1     rsc_parent_child_shutdown_ack 
                //0x8a: 0x4001    SET0      rsc_child_parent_shutdown_req 
  0x70005000 ,  //0x8c: 0x5000    SET1      rsc_seq_wait_wakeup           
                //0x8e: 0x7000    WAIT1     rsc_core_rsc_bringup_req      
  0x70025002 ,  //0x90: 0x5002    SET1      rsc_child_parent_bringup_req  
                //0x92: 0x7002    WAIT1     rsc_parent_child_bringup_ack  
  0x50234002 ,  //0x94: 0x4002    SET0      rsc_child_parent_bringup_req  
                //0x96: 0x5023    SET1      rsc_parent_tcs_trigger_3      
  0x40237023 ,  //0x98: 0x7023    WAIT1     rsc_parent_tcs_event_3        
                //0x9a: 0x4023    SET0      rsc_parent_tcs_trigger_3      
  0x40145003 ,  //0x9c: 0x5003    SET1      rsc_ares                      
                //0x9e: 0x4014    SET0      rsc_clamp_mss2cx              
  0x00054013 ,  //0xa0: 0x4013    SET0      rsc_pll_reset                 
                //0xa2: 0x0005    DELI      1040ns                        
  0x40110005 ,  //0xa4: 0x0005    DELI      1040ns                        
                //0xa6: 0x4011    SET0      rsc_pll_opmode0_stby          
  0x7006400c ,  //0xa8: 0x400C    SET0      rsc_bhs_off                   
                //0xaa: 0x7006    WAIT1     rsc_pwr_on_event              
  0x400b700b ,  //0xac: 0x700B    WAIT1     rsc_mem_refresh_event         
                //0xae: 0x400B    SET0      rsc_clamp_qmc_mem             
  0x7005400a ,  //0xb0: 0x400A    SET0      rsc_mem_core_slp              
                //0xb2: 0x7005    WAIT1     rsc_mem_core_ready_event      
  0x70044009 ,  //0xb4: 0x4009    SET0      rsc_mem_periph_slp            
                //0xb6: 0x7004    WAIT1     rsc_mem_periph_ready_event    
  0x50074008 ,  //0xb8: 0x4008    SET0      rsc_clamp_wl                  
                //0xba: 0x5007    SET1      rsc_restore                   
  0x40054007 ,  //0xbc: 0x4007    SET0      rsc_restore                   
                //0xbe: 0x4005    SET0      rsc_clamp_io                  
  0x50154003 ,  //0xc0: 0x4003    SET0      rsc_ares                      
                //0xc2: 0x5015    SET1      rsc_pm_irq                    
  0x70074015 ,  //0xc4: 0x4015    SET0      rsc_pm_irq                    
                //0xc6: 0x7007    WAIT1     rsc_pll_ready_event           
  0x401e4010 ,  //0xc8: 0x4010    SET0      rsc_pll_outdis                
                //0xca: 0x401E    SET0      rsc_ext_xo_enable             
  0x10004004 ,  //0xcc: 0x4004    SET0      rsc_clk_halt                  
                //0xce: 0x1000    RETURN    
//LONG_APCR_SEQ_NOTCS:
  0x70035004 ,  //0xd0: 0x5004    SET1      rsc_clk_halt                  
                //0xd2: 0x7003    WAIT1     rsc_core_clk_off_event        
  0x00015005 ,  //0xd4: 0x5005    SET1      rsc_clamp_io                  
                //0xd6: 0x0001    DELI      52ns                          
  0x40065006 ,  //0xd8: 0x5006    SET1      rsc_save                      
                //0xda: 0x4006    SET0      rsc_save                      
  0x50095008 ,  //0xdc: 0x5008    SET1      rsc_clamp_wl                  
                //0xde: 0x5009    SET1      rsc_mem_periph_slp            
  0x500a7009 ,  //0xe0: 0x7009    WAIT1     rsc_mem_periph_pd_event       
                //0xe2: 0x500A    SET1      rsc_mem_core_slp              
  0x500b700a ,  //0xe4: 0x700A    WAIT1     rsc_mem_core_pd_event         
                //0xe6: 0x500B    SET1      rsc_clamp_qmc_mem             
  0x6006500c ,  //0xe8: 0x500C    SET1      rsc_bhs_off                   
                //0xea: 0x6006    WAIT0     rsc_pwr_on_event              
  0x5010501e ,  //0xec: 0x501E    SET1      rsc_ext_xo_enable             
                //0xee: 0x5010    SET1      rsc_pll_outdis                
  0x50135011 ,  //0xf0: 0x5011    SET1      rsc_pll_opmode0_stby          
                //0xf2: 0x5013    SET1      rsc_pll_reset                 
  0x50015014 ,  //0xf4: 0x5014    SET1      rsc_clamp_mss2cx              
                //0xf6: 0x5001    SET1      rsc_child_parent_shutdown_req 
  0x40017001 ,  //0xf8: 0x7001    WAIT1     rsc_parent_child_shutdown_ack 
                //0xfa: 0x4001    SET0      rsc_child_parent_shutdown_req 
  0x70005000 ,  //0xfc: 0x5000    SET1      rsc_seq_wait_wakeup           
                //0xfe: 0x7000    WAIT1     rsc_core_rsc_bringup_req      
  0x70025002 ,  //0x100: 0x5002    SET1      rsc_child_parent_bringup_req  
                //0x102: 0x7002    WAIT1     rsc_parent_child_bringup_ack  
  0x50034002 ,  //0x104: 0x4002    SET0      rsc_child_parent_bringup_req  
                //0x106: 0x5003    SET1      rsc_ares                      
  0x40134014 ,  //0x108: 0x4014    SET0      rsc_clamp_mss2cx              
                //0x10a: 0x4013    SET0      rsc_pll_reset                 
  0x00050005 ,  //0x10c: 0x0005    DELI      1040ns                        
                //0x10e: 0x0005    DELI      1040ns                        
  0x400c4011 ,  //0x110: 0x4011    SET0      rsc_pll_opmode0_stby          
                //0x112: 0x400C    SET0      rsc_bhs_off                   
  0x700b7006 ,  //0x114: 0x7006    WAIT1     rsc_pwr_on_event              
                //0x116: 0x700B    WAIT1     rsc_mem_refresh_event         
  0x400a400b ,  //0x118: 0x400B    SET0      rsc_clamp_qmc_mem             
                //0x11a: 0x400A    SET0      rsc_mem_core_slp              
  0x40097005 ,  //0x11c: 0x7005    WAIT1     rsc_mem_core_ready_event      
                //0x11e: 0x4009    SET0      rsc_mem_periph_slp            
  0x40087004 ,  //0x120: 0x7004    WAIT1     rsc_mem_periph_ready_event    
                //0x122: 0x4008    SET0      rsc_clamp_wl                  
  0x40075007 ,  //0x124: 0x5007    SET1      rsc_restore                   
                //0x126: 0x4007    SET0      rsc_restore                   
  0x40034005 ,  //0x128: 0x4005    SET0      rsc_clamp_io                  
                //0x12a: 0x4003    SET0      rsc_ares                      
  0x40155015 ,  //0x12c: 0x5015    SET1      rsc_pm_irq                    
                //0x12e: 0x4015    SET0      rsc_pm_irq                    
  0x40107007 ,  //0x130: 0x7007    WAIT1     rsc_pll_ready_event           
                //0x132: 0x4010    SET0      rsc_pll_outdis                
  0x4004401e ,  //0x134: 0x401E    SET0      rsc_ext_xo_enable             
                //0x136: 0x4004    SET0      rsc_clk_halt                  
  0x00001000 ,  //0x138: 0x1000    RETURN
};

/*==========================================================================
 *                          SSC RSCp CONFIGURATIONS
 *=========================================================================*/
uint32 g_rscParentSequence[SSC_LPASS_RSCP_NUMSEQWORDS] = 
{
//LONG_SEQ:
  0x51e3a350 ,  //0x00: 0x50      BEQ       child_bringup_req_assert=1    EXIT_SLEEP                    
                //0x01: 0xa3      SET1      rsc_pdc_start_monitoring_out  
                //0x02: 0xe3      WAIT1     rsc_pdc_start_monitoring_started
                //0x03: 0x51      BEQ       child_bringup_req_assert=1    EXIT_SLEEP_REPLAY             
  0xeaaae5a0 ,  //0x04: 0xa0      SET1      rsc_reserved_for_hidden_tcs   
                //0x05: 0xe5      WAIT1     hidden_tcs_comp               
                //0x06: 0xaa      SET1      tcs_hw_trigger_out_4          
                //0x07: 0xea      WAIT1     tcs_hw_trigger_cmp_4          
  0xceb0838a ,  //0x08: 0x8a      SET0      tcs_hw_trigger_out_4          
                //0x09: 0x83      SET0      rsc_pdc_start_monitoring_out  
                //0x0a: 0xb0      SET1      clk_en                        
                //0x0b: 0xce      WAIT0     pscbc_clk_on                  
  0xb2acb1b3 ,  //0x0c: 0xb3      SET1      ssc_gcc_ret_or_pc             
                //0x0d: 0xb1      SET1      opmode_ctrl                   
                //0x0e: 0xac      SET1      pll_mode                      
                //0x0f: 0xb2      SET1      pll_oe                        
  0xa581e1a1 ,  //0x10: 0xa1      SET1      rsc_pdc_shutdown_req          
                //0x11: 0xe1      WAIT1     rsc_pdc_shutdown_ack          
                //0x12: 0x81      SET0      rsc_pdc_shutdown_req          
                //0x13: 0xa5      SET1      rsc_internal_seq_wait_wakeup  
  0xe2a285e0 ,  //0x14: 0xe0      WAIT1     rsc_pdc_wakeup_event          
                //0x15: 0x85      SET0      rsc_internal_seq_wait_wakeup  
                //0x16: 0xa2      SET1      rsc_pdc_bringup_req           
                //0x17: 0xe2      WAIT1     rsc_pdc_bringup_ack           
  0x8bebab82 ,  //0x18: 0x82      SET0      rsc_pdc_bringup_req           
                //0x19: 0xab      SET1      tcs_hw_trigger_out_5          
                //0x1a: 0xeb      WAIT1     tcs_hw_trigger_cmp_5          
                //0x1b: 0x8b      SET0      tcs_hw_trigger_out_5          
  0x8d91ec8c ,  //0x1c: 0x8c      SET0      pll_mode                      
                //0x1d: 0xec      WAIT1     pll_locked_OR_pll_lockdly_exp 
                //0x1e: 0x91      SET0      opmode_ctrl                   
                //0x1f: 0x8d      SET0      pll_tmr_rst                   
  0x93069092 ,  //0x20: 0x92      SET0      pll_oe                        
                //0x21: 0x90      SET0      clk_en                        
                //0x22: 0x6       DELI      5044ns                        
                //0x23: 0x93      SET0      ssc_gcc_ret_or_pc             
//EXIT_SLEEP_REPLAY:
  0x2084e4a4 ,  //0x24: 0xa4      SET1      rsc_pdc_start_replay_out      
                //0x25: 0xe4      WAIT1     rsc_pdc_start_replay_done     
                //0x26: 0x84      SET0      rsc_pdc_start_replay_out      
                //0x27: 0x20      RETURN    
//EXIT_SLEEP:
  0x00000020 ,  //0x28: 0x20      RETURN     


};

branch_info g_rscParentBranch[SSC_LPASS_RSCP_NUMBRANCHES] = {
  {0, 0x28 },  // EXIT_SLEEP = 39 + BRANCH_ADDR: 0x0
  {1, 0x24 },  // EXIT_SLEEP_REPLAY = 35 + BRANCH_ADDR: 0x0
};
