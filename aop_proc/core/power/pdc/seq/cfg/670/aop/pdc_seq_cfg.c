/*===========================================================================
                              pdc_seq_cfg.c

DESCRIPTION:
  This file provides configuration data like sequences, delays, etc for 
  various PDC sequencers handled on a given subsystem.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*History
   Desc		            commit ID 		date
Initial warlok seq      75d56be	       9/25/17
*/
//===========================================================================
//                   Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include "pdc_seq.h"
#include "pdc_seq_internal.h"
#include "pdc_seq_hal.h"
#include "pdc_seq_hal_bsp.h"

#define PDC_SEQ_ARRAY(arr) arr, (sizeof(arr)/sizeof(arr[0]))

//---------------------------------------------------------------------------
//  Global Variables
//---------------------------------------------------------------------------

/* Cx collapse sequence - dummy for testing */
pdc_seq_cmd_t  aop_collapse[] =
{
   0xe1,     // WAIT1   rx_fifo_empty           # Make sure all transactions from RSC have exited the PDC
   0xa1,     // SET1    pdc_mode_control        # PDC exists the pass through mode and controls TX and RX transactions on IPCB
   0xa0,     // SET1    timer_match_valid       # Enable timer based wakeup (assumes the timer_match_value was written by RSC)
   0xb3,     // SET1    addl_seq_pwr_ctrl_5     # [DEBUG] SWAO going to Retention
   0xf3,     // WAIT1   addl_seq_wait_event_5   # Wait for SWAO going to Retention
   0xb7,     // SET1    addl_seq_pwr_ctrl_9     # [DEBUG] Clock OFF 
   0xf6,     // WAIT1   addl_seq_wait_event_8   # Wait for DEBUG ETB Clock OFF
   0xf7,     // WAIT1   addl_seq_wait_event_9   # Wait for Message RAM Clock OFF
   0x07,     // DELI    10036ns                 # Delay before asserting memory clamp  
   0xb5,     // SET1    addl_seq_pwr_ctrl_7     # [DEBUG] Inform DEBUG to use FIFO and not ETB (clamp_mem) 
   0xb1,     // SET1    addl_seq_pwr_ctrl_3     # [AOSS_CC] Turn on Pwr Mux Ctrl Clock
   0xd1,     // WAIT0   addl_seq_wait_event_3   # Wait for acknowledgement from AOSS_CC
   0x8e,     // SET0    addl_seq_pwr_ctrl_0     # [AOSS_PwrMuxCtrl] Values indicate Rails you want to switch. out2apm_tile_active_br[0].
   0xaf,     // SET1    addl_seq_pwr_ctrl_1     # [AOSS_PwrMuxCtrl] Values indicate Rails you want to switch. out2apm_tile_active_br[1]
   0xb0,     // SET1    addl_seq_pwr_ctrl_2     # [AOSS_PwrMuxCtrl] Switch Mux to indicated Rail (2'b10 = Aux should be active branch).out2apm_start_apm_switch
   0x90,     // SET0    addl_seq_pwr_ctrl_2     # [AOSS_PwrMuxCtrl] 1 followed by 0 on out2apm_start_apm_switch
   0xf0,     // WAIT1   addl_seq_wait_event_2   # Wait for apm2out_tile_trans_done
   0x91,     // SET0    addl_seq_pwr_ctrl_3     # [AOSS_CC] Turn OFF Pwr Mux Ctrl Clock
   0xbf,     // SET1    seq_tcs_0_start      
   0xff,     // WAIT1   tcs_0_seq_cmp       
   0xbe,     // SET1    seq_tcs_1_start      
   0xfe,     // WAIT1   tcs_1_seq_cmp
   0xf5,     // WAIT1   addl_seq_wait_event_7   # Wait for VRM in idle
   0xb6,     // SET1    addl_seq_pwr_ctrl_8     # Disable AOP watchdog
   0xb2,     // SET1    addl_seq_pwr_ctrl_4     # Turn off RO (etb/message RAM clk off) 
   0xa5,     // SET1    wfi_profiling_unit_3    # Profiling Unit to indicate end of Power Down Sequence
   0xe0,     // WAIT1   sequence_resume    
   0xa6,     // SET1    generic_profiling_unit_5  # HW workaround for QCTDD03447497
   0x92,     // SET0    addl_seq_pwr_ctrl_4     # Turn ON RO (etb/message RAM clk off) 
   0x96,     // SET0    addl_seq_pwr_ctrl_8     # Enable AOP watchdog
   0xbd,     // SET1    seq_tcs_2_start      
   0xfd,     // WAIT1   tcs_2_seq_cmp       
   0xbc,     // SET1    seq_tcs_3_start      
   0xfc,     // WAIT1   tcs_3_seq_cmp      
   0xb1,     // SET1    addl_seq_pwr_ctrl_3     # [AOSS_CC] Turn ON Pwr Mux Ctrl Clock
   0xd1,     // WAIT0   addl_seq_wait_event_3   # Wait for acknowledgement from AOSS_CC
   0xae,     // SET1    addl_seq_pwr_ctrl_0     # [AOSS_PwrMuxCtrl] Values indicate Rails you want to switch. out2apm_tile_active_br[0].
   0x8f,     // SET0    addl_seq_pwr_ctrl_1     # [AOSS_PwrMuxCtrl] Values indicate Rails you want to switch. out2apm_tile_active_br[1]
   0xb0,     // SET1    addl_seq_pwr_ctrl_2     # [AOSS_PwrMuxCtrl] Switch Mux to indicated Rail (2'b10 = Aux should be active branch).out2apm_start_apm_switch
   0x90,     // SET0    addl_seq_pwr_ctrl_2     # [AOSS_PwrMuxCtrl] 1 followed by 0 on out2apm_start_apm_switch
   0xf0,     // WAIT1   addl_seq_wait_event_2   # Wait for apm2out_tile_trans_done
   0x91,     // SET0    addl_seq_pwr_ctrl_3     # Disable apm_controller_wrap_clk
   0x95,     // SET0    addl_seq_pwr_ctrl_7     # [DEBUG] Inform DEBUG to use ETB and not FIFO (clamp_mem) 
   0x97,     // SET0    addl_seq_pwr_ctrl_9     # [DEBUG] Clock ON 
   0xd6,     // WAIT0   addl_seq_wait_event_8   # Wait for [DEBUG] Clock ON
   0xb4,     // SET1    addl_seq_pwr_ctrl_6     # [DEBUG] SWAO out of Retention
   0xf4,     // WAIT1   addl_seq_wait_event_6   # Wait for SWAO out of Retention
   0x93,     // SET0    addl_seq_pwr_ctrl_5     # [DEBUG] SWAO going to Retention
   0x04,     // DELI    520ns
   0x94,     // SET0    addl_seq_pwr_ctrl_6     # [DEBUG] SWAO out of Retention
   0x81,     // SET0      pdc_mode_control        # Restore "pas-through mode".
   0xa7,     // SET1    wakeup_req              # Hand over control to AOP RSC 
   0x87,     // SET0    wakeup_req  
   0xe7,     // WAIT1   wakeup_ack              # RSC should ack back immediately
   0x20,     // RETURN  
};

pdc_seq_cmd_t debug_seq[] =
{
// Sequence Memory Commands
  0x50,   	//  CX_COL_AOSS:            BEQ rst_ctrl_resin_in=1    BRANCH0_ABNORMAL_RESET 
  0xa1,   	//  SET1        pdc_mode_control        # PDC exists the pass through mode and controls TX and RX transactions on IPCB
  0xa2,   	//	SET1        soc_sleep_req           # To indicate to AOP to exit the SOC infrastructure Sleep
  0xa3,   	//	SET1        cx_collapse_req         # To indicate to AOP to exit cx_collapse 
  0xe2,   	//	WAIT1       soc_sleep_unlock        # Wait for AOP to indicate it is out of SOC infrastructure sleep
  0xe3,   	//	WAIT1       cx_collapse_unlock      # Wait for AOP to indicate it is out of cx_collapse
  0xbc,   	//	SET1        seq_tcs_3_start         # Response Required Votes for XO ON, CX @ MOL and MX @MOL
  0xfc,   	//	WAIT1       tcs_3_seq_cmp
  0xe0,   	//	WAIT1       sequence_resume
  0x50,   	//	BEQ         rst_ctrl_resin_in=1    BRANCH0_ABNORMAL_RESET 
  0xbe,   	//	SET1        seq_tcs_1_start         # Fire& Forget Power Down Votes - XO Shutdown, CX OFF, MX RET
  0xfe,   	//	WAIT1       tcs_1_seq_cmp           
  0x83,   	//	SET0        cx_collapse_req         # To Enable AOP to enter cx_collapse
  0x82,   	//	SET0        soc_sleep_req           # To Enable AOP to enable the SOC infrastructure to Sleep
  0x20,   	//	RETURN
  0xb5,   	//BRANCH0_ABNORMAL_RESET: SET1        addl_seq_pwr_ctrl_7_save_ff_bar         # override the functional save_ff to 0
  0xae,   	//	SET1        addl_seq_pwr_ctrl_0_act_br_switch_sel   # select the hard coded value 
  0xb1,   	//	SET1        addl_seq_pwr_ctrl_3_apm_ctrl_clk_en     # turn on power mux controller clk
  0xd1,   	//	WAIT0       addl_seq_wait_event_3_apm_ctrl_clk_on   # wait for power mux controller clk to be ON
  0xb0,   	//	SET1        addl_seq_pwr_ctrl_2_start_apm_switch    # start switching the power mux
  0x06,  	    //  DELI        5044ns
  0x91,   	//	SET0        addl_seq_pwr_ctrl_3_apm_ctrl_clk_en     # turn off power mux controller clk
  0xb7,   	//	SET1        addl_seq_pwr_ctrl_9_pwr_up_rst_mux_sel
  0xb9,   	//	SET1        addl_seq_pwr_ctrl_11_restore_ff_mux_sel
  0xb8,   	//	SET1        addl_seq_pwr_ctrl_10_restore_ff
  0x04,  		//	DELI        520ns
  0x98,   	//	SET0        addl_seq_pwr_ctrl_10_restore_ff
  0xba,   	//	SET1        addl_seq_pwr_ctrl_12_clamp_mem_iso_out_freeze_io_ret_0pin_mem_svs_mem_turbo_bar
  0xb4,   	//	SET1        addl_seq_pwr_ctrl_6_iso_input_bar           # override the functional iso_input to 0
  0xb2,   	//	SET1        addl_seq_pwr_ctrl_4_func_clk_dis_bar        # override the functional func_clk_dis to 0
  0xb3,   	//	SET1        addl_seq_pwr_ctrl_5_xo_pad_enable
  0xbb,   	//	SET1        addl_seq_pwr_ctrl_13_status_to_rst_ctrl  
  0xf2, 		//	WAIT1       addl_seq_wait_event_4_tied_low          # will always wait here and not end sequence so as 
  0x20,   	//	RETURN
};

/* Branch addresses - unused */
uint8 aop_branches[] = { 0x0, 0x0, 0x0, 0x0 };
uint8 dbg_branches[] = { 0xf, 0x0, 0x0, 0x0 };

/* Delay values - unused */
uint32 delays[] = 
{ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

pdc_seq_hal_bsp_cfg_t aop_cfg = 
{
  PDC_SEQ_ARRAY (aop_branches),
  PDC_SEQ_ARRAY (delays)
};

pdc_seq_hal_bsp_cfg_t dbg_cfg = 
{
  PDC_SEQ_ARRAY (dbg_branches),
  PDC_SEQ_ARRAY (delays)
};

/* PDC supported low power mode */
pdc_seq_hal_bsp_mode_t aop_modes[] =
{
  {
    PDC_SEQ_ARRAY (aop_collapse),
    1,
    NULL
  },
};


/* PDC supporting debug on reset */
pdc_seq_hal_bsp_mode_t debug_modes[] =
{
  {
    PDC_SEQ_ARRAY (debug_seq),
    1,
    NULL
  },
};

/* PDC sequencers handled on this Subsystem */
pdc_seq_t g_pdc_seq_instances[] = 
{
  {
    "aop",
    &aop_cfg,
    PDC_SEQ_ARRAY (aop_modes),
    0x260000,
    0xD0000
  },
  {
    "debug",
    &dbg_cfg,
    PDC_SEQ_ARRAY (debug_modes),
    0x270000,
    0x0            /* No attached RSC */
  },
};

uint32 g_pdc_seq_instance_count = 
  sizeof (g_pdc_seq_instances) / sizeof (g_pdc_seq_instances[0]);
