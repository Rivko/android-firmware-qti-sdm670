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
static uint32 g_rscChildSequence[Q6SS_RSCC_CSS_NUMSEQWORDS] = 
{
//SHORT_APCR_SEQ_CSS:
  0x50055004 ,  //0x00: 0x5004    SET1      rsc_clk_halt                  
                //0x02: 0x5005    SET1      rsc_clamp_io                  
  0x50060001 ,  //0x04: 0x0001    DELI      52ns                          
                //0x06: 0x5006    SET1      rsc_save                      
  0x50084006 ,  //0x08: 0x4006    SET0      rsc_save                      
                //0x0a: 0x5008    SET1      rsc_clamp_wl                  
  0x70095009 ,  //0x0c: 0x5009    SET1      rsc_mem_periph_slp            
                //0x0e: 0x7009    WAIT1     rsc_mem_periph_pd_event       
  0x500d500b ,  //0x10: 0x500B    SET1      rsc_clamp_qmc_mem             
                //0x12: 0x500D    SET1      rsc_ldo_bypass_open           
  0x500e500c ,  //0x14: 0x500C    SET1      rsc_bhs_off                   
                //0x16: 0x500E    SET1      rsc_ldo_pd                    
  0x50006006 ,  //0x18: 0x6006    WAIT0     rsc_pwr_on_event              
                //0x1a: 0x5000    SET1      rsc_seq_wait_wakeup           
  0x50037000 ,  //0x1c: 0x7000    WAIT1     rsc_core_rsc_bringup_req      
                //0x1e: 0x5003    SET1      rsc_ares                      
  0x400c400e ,  //0x20: 0x400E    SET0      rsc_ldo_pd                    
                //0x22: 0x400C    SET0      rsc_bhs_off                   
  0x400d7006 ,  //0x24: 0x7006    WAIT1     rsc_pwr_on_event              
                //0x26: 0x400D    SET0      rsc_ldo_bypass_open           
  0x4009400b ,  //0x28: 0x400B    SET0      rsc_clamp_qmc_mem             
                //0x2a: 0x4009    SET0      rsc_mem_periph_slp            
  0x40087004 ,  //0x2c: 0x7004    WAIT1     rsc_mem_periph_ready_event    
                //0x2e: 0x4008    SET0      rsc_clamp_wl                  
  0x40075007 ,  //0x30: 0x5007    SET1      rsc_restore                   
                //0x32: 0x4007    SET0      rsc_restore                   
  0x40034005 ,  //0x34: 0x4005    SET0      rsc_clamp_io                  
                //0x36: 0x4003    SET0      rsc_ares                      
  0x40155015 ,  //0x38: 0x5015    SET1      rsc_pm_irq                    
                //0x3a: 0x4015    SET0      rsc_pm_irq                    
  0x10004004 ,  //0x3c: 0x4004    SET0      rsc_clk_halt                  
                //0x3e: 0x1000    RETURN    
//LONG_APCR_SEQ_NOTCS:
  0x70035004 ,  //0x40: 0x5004    SET1      rsc_clk_halt                  
                //0x42: 0x7003    WAIT1     rsc_core_clk_off_event        
  0x00015005 ,  //0x44: 0x5005    SET1      rsc_clamp_io                  
                //0x46: 0x0001    DELI      52ns                          
  0x40065006 ,  //0x48: 0x5006    SET1      rsc_save                      
                //0x4a: 0x4006    SET0      rsc_save                      
  0x50095008 ,  //0x4c: 0x5008    SET1      rsc_clamp_wl                  
                //0x4e: 0x5009    SET1      rsc_mem_periph_slp            
  0x500a7009 ,  //0x50: 0x7009    WAIT1     rsc_mem_periph_pd_event       
                //0x52: 0x500A    SET1      rsc_mem_core_slp              
  0x500b700a ,  //0x54: 0x700A    WAIT1     rsc_mem_core_pd_event         
                //0x56: 0x500B    SET1      rsc_clamp_qmc_mem             
  0x500c500d ,  //0x58: 0x500D    SET1      rsc_ldo_bypass_open           
                //0x5a: 0x500C    SET1      rsc_bhs_off                   
  0x500f500e ,  //0x5c: 0x500E    SET1      rsc_ldo_pd                    
                //0x5e: 0x500F    SET1      rsc_ldo_bg_pd                 
  0x501e6006 ,  //0x60: 0x6006    WAIT0     rsc_pwr_on_event              
                //0x62: 0x501E    SET1      rsc_ext_xo_enable             
  0x50115010 ,  //0x64: 0x5010    SET1      rsc_pll_outdis                
                //0x66: 0x5011    SET1      rsc_pll_opmode0_stby          
  0x50145013 ,  //0x68: 0x5013    SET1      rsc_pll_reset                 
                //0x6a: 0x5014    SET1      rsc_clamp_mss2cx              
  0x70015001 ,  //0x6c: 0x5001    SET1      rsc_child_parent_shutdown_req 
                //0x6e: 0x7001    WAIT1     rsc_parent_child_shutdown_ack 
  0x50004001 ,  //0x70: 0x4001    SET0      rsc_child_parent_shutdown_req 
                //0x72: 0x5000    SET1      rsc_seq_wait_wakeup           
  0x50027000 ,  //0x74: 0x7000    WAIT1     rsc_core_rsc_bringup_req      
                //0x76: 0x5002    SET1      rsc_child_parent_bringup_req  
  0x40027002 ,  //0x78: 0x7002    WAIT1     rsc_parent_child_bringup_ack  
                //0x7a: 0x4002    SET0      rsc_child_parent_bringup_req  
  0x40145003 ,  //0x7c: 0x5003    SET1      rsc_ares                      
                //0x7e: 0x4014    SET0      rsc_clamp_mss2cx              
  0x00054013 ,  //0x80: 0x4013    SET0      rsc_pll_reset                 
                //0x82: 0x0005    DELI      1040ns                        
  0x40110005 ,  //0x84: 0x0005    DELI      1040ns                        
                //0x86: 0x4011    SET0      rsc_pll_opmode0_stby          
  0x0006400f ,  //0x88: 0x400F    SET0      rsc_ldo_bg_pd                 
                //0x8a: 0x0006    DELI      5044ns                        
  0x401d501d ,  //0x8c: 0x501D    SET1      rsc_ldo_update                
                //0x8e: 0x401D    SET0      rsc_ldo_update                
  0x400c400e ,  //0x90: 0x400E    SET0      rsc_ldo_pd                    
                //0x92: 0x400C    SET0      rsc_bhs_off                   
  0x400d7006 ,  //0x94: 0x7006    WAIT1     rsc_pwr_on_event              
                //0x96: 0x400D    SET0      rsc_ldo_bypass_open           
  0x400b700b ,  //0x98: 0x700B    WAIT1     rsc_mem_refresh_event         
                //0x9a: 0x400B    SET0      rsc_clamp_qmc_mem             
  0x7005400a ,  //0x9c: 0x400A    SET0      rsc_mem_core_slp              
                //0x9e: 0x7005    WAIT1     rsc_mem_core_ready_event      
  0x70044009 ,  //0xa0: 0x4009    SET0      rsc_mem_periph_slp            
                //0xa2: 0x7004    WAIT1     rsc_mem_periph_ready_event    
  0x50074008 ,  //0xa4: 0x4008    SET0      rsc_clamp_wl                  
                //0xa6: 0x5007    SET1      rsc_restore                   
  0x40054007 ,  //0xa8: 0x4007    SET0      rsc_restore                   
                //0xaa: 0x4005    SET0      rsc_clamp_io                  
  0x50154003 ,  //0xac: 0x4003    SET0      rsc_ares                      
                //0xae: 0x5015    SET1      rsc_pm_irq                    
  0x70074015 ,  //0xb0: 0x4015    SET0      rsc_pm_irq                    
                //0xb2: 0x7007    WAIT1     rsc_pll_ready_event           
  0x401e4010 ,  //0xb4: 0x4010    SET0      rsc_pll_outdis                
                //0xb6: 0x401E    SET0      rsc_ext_xo_enable             
  0x10004004 ,  //0xb8: 0x4004    SET0      rsc_clk_halt                  
                //0xba: 0x1000    RETURN
};

/* QDSP6 mode configurations. */
static rsc_modes_t g_rscChildModes[] =
{
  {
    RSC_MODE_APCR_PLL_ON,                 // mode_id
    Q6SS_RSCC_CSS_SHORT_APCR_SEQ_CSS,     // start_address
  },
  {
    RSC_MODE_PWRC_BHS,
    Q6SS_RSCC_CSS_LONG_APCR_SEQ_NOTCS,
  },
};

/*==========================================================================
 *                          SSC RSCp CONFIGURATIONS
 *=========================================================================*/
static uint32 g_rscParentSequence[TURING_RSCP_NUMSEQWORDS] = 
{
  0xe8a8e5a0 ,  //0x00: 0xa0      SET1      hidden_tcs_trig_from_seq      
                //0x01: 0xe5      WAIT1     hidden_tcs_comp               
                //0x02: 0xa8      SET1      local_tcs_hw_trigger2         
                //0x03: 0xe8      WAIT1     local_tcs_hw_cmp2             
  0xa3cdac88 ,  //0x04: 0x88      SET0      local_tcs_hw_trigger2         
                //0x05: 0xac      SET1      pll_standby                   
                //0x06: 0xcd      WAIT0     pll_lock                      
                //0x07: 0xa3      SET1      pdc_start_monitoring          
  0xe1a183e3 ,  //0x08: 0xe3      WAIT1     pdc_monitoring_started        
                //0x09: 0x83      SET0      pdc_start_monitoring          
                //0x0a: 0xa1      SET1      pdc_shutdown_req              
                //0x0b: 0xe1      WAIT1     pdc_shutdown_ack              
  0x85e0a581 ,  //0x0c: 0x81      SET0      pdc_shutdown_req              
                //0x0d: 0xa5      SET1      seq_wait_wakeup               
                //0x0e: 0xe0      WAIT1     pdc_wakeup_req_or_child_bringup_req
                //0x0f: 0x85      SET0      seq_wait_wakeup               
  0xa482e2a2 ,  //0x10: 0xa2      SET1      pdc_bringup_req               
                //0x11: 0xe2      WAIT1     pdc_bringup_ack               
                //0x12: 0x82      SET0      pdc_bringup_req               
                //0x13: 0xa4      SET1      pdc_start_replay              
  0xe9a984e4 ,  //0x14: 0xe4      WAIT1     pdc_reply_done                
                //0x15: 0x84      SET0      pdc_start_replay              
                //0x16: 0xa9      SET1      local_tcs_hw_trigger3         
                //0x17: 0xe9      WAIT1     local_tcs_hw_cmp3             
  0xeeed8c89 ,  //0x18: 0x89      SET0      local_tcs_hw_trigger3         
                //0x19: 0x8c      SET0      pll_standby                   
                //0x1a: 0xed      WAIT1     pll_lock                      
                //0x1b: 0xee      WAIT1     pll_outctrl                   
  0x00002003 ,  //0x1c: 0x3       DELI      208ns                         
                //0x1d: 0x20      RETURN     
};

static rsc_modes_t g_rscParentModes[] =
{
  {
    RSC_PARENT_MODE_CHIP_LPM,     // mode_id
    TURING_RSCP_PDC_HS_PWR_COL,   // start_address
  },
};

rsc_t g_rscInstances[] =
{
  {
    RSC_ARRAY(g_rscChildModes),     //Instance modes
    g_rscChildSequence,             //Instance sequence
    Q6SS_RSCC_CSS_NUMSEQWORDS,
    NULL,                           //branch info (static)
    Q6SS_RSCC_CSS_NUMBRANCHES,
    RSC_SS_BASE + 0x003b0000,
  },
  
  {
    RSC_ARRAY(g_rscParentModes),
    g_rscParentSequence,
    TURING_RSCP_NUMSEQWORDS,
    NULL,
    TURING_RSCP_NUMBRANCHES,
    RSC_SS_BASE + 0x000a4000,
  }
};

/* Variable to indicate rsc instance counts */
uint32 g_rscInstCount = sizeof(g_rscInstances) / sizeof(g_rscInstances[0]);