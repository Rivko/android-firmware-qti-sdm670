/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/target/SDM670/sdi/ddrss_init_sdi.c#4 $
$DateTime: 2018/02/15 02:48:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/04/14   arindamm     First edit history header. Add new entries at top.
================================================================================*/

#include "ddrss_init_sdi.h"
#include "ddrss.h"





/*********************************************************************************
**********************************************************************************
Code from ddrss.c
**********************************************************************************
*********************************************************************************/

//================================================================================================//
// Pass 1 and Pass 2 DDR Initialization
//================================================================================================//
boolean HAL_DDR_SDI_Recover (SHRM_DDR_STRUCT *shrm_ddr_ptr,
                             DDR_CHANNEL channel, 
                             DDR_CHIPSELECT chip_select,
                             uint32 clk_freq_khz)
{
   uint32 m = 0;
   uint32 num_ch = shrm_ddr_ptr->misc.num_channel;
   uint32 old_clk_index = 0;
   uint32 old_ddr_period = 0;
   uint32 clk_idx = 0;
   
   // As described by the watchdog reset documentation at go/ddrss -> Napali -> Watchdog the Napali watchdog reset process
   //    is much simpler than previous chips.

   // For Pass 1 warm boot, we need to send a SHRM command via the RSI that brings the DDRSS from Powerstate D3 (collapsed)
   //    to Powerstate 0 (active), at the 200 MHz boot frequency.  We can then take the DDR out of (watchdog) self-refresh,
   //    and run recovery traffic.
   
   if ((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (shrm_ddr_ptr->misc.chip_version >= 0x0200)
      && ((shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect & DDR_CS1) == 0))
   {
       // Workaround for CSPD stuck on inactive rank issue (NAPALI-1079):
       // Enable activity based CSPD for non-existing rank after wdog reset, before power collapse exit frequency switch.
       // For each QOS level, set CABO_MPE_APM_OPT_CFG_CS[IDLE_POWER_DOWN_EN] & [ACTIVE_POWER_DOWN_EN] 
       //    for non-existing rank, and set MPE_LOAD_CONFIG=1.
       // Then disable the feature above again after the frequency switch.
       for (m = 0; m < 4; m++)
       {
           HWIO_OUTXFI2 (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CFG_m_CS_n, m, 1, ACTIVE_POWER_DOWN_EN, 1);
           HWIO_OUTXFI2 (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CFG_m_CS_n, m, 1, IDLE_POWER_DOWN_EN, 1);
           HWIO_OUTXFI (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CTRL_m, m, CLOCK_STOP_EN, 1);
       }
       
       HWIO_OUTXF (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_LOAD_CONFIG, LOAD_CONFIG, 1);
       while (HWIO_INXF (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_LOAD_CONFIG, LOAD_CONFIG) == 1);
   }
   
   
   // Figure out what clock index we were at before the watchdog reset
   old_ddr_period = HWIO_INX(SEQ_DDR_SS_MCCC_MCCC_MSTR_OFFSET, MCCC_MSTR_CLK_PERIOD);
   if (old_ddr_period == 0) 
   { 
      old_clk_index = 0;
   }
   else 
   {
      for (clk_idx = 0; clk_idx < MAX_NUM_CLOCK_PLAN; clk_idx++) {
      if (old_ddr_period >= shrm_ddr_ptr->ddr_runtime.clock_plan[clk_idx].clk_period_in_psec)
          break;
      }
      // Make sure we don't use an index past the end of the table
      old_clk_index = MIN(clk_idx, (MAX_NUM_CLOCK_PLAN-1));
   }
   
   // Do the Phy GFM mux workaround for Napali V2 if we are coming from a DDRCC frequency before watchdog reset
   if ((!((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (shrm_ddr_ptr->misc.chip_version == 0x0100)))
       && (shrm_ddr_ptr->ddr_runtime.clock_plan[old_clk_index].clk_mode == DDRCC)) 
 

   {
       // Workaround for Phy GFM mux not reset properly in SDI Pass 1 issue (also on Jira NAPALI-1079):
       // We must reset each the Glitch Free Mux selecting GCC/DDRCC clock mux in each Phy because it can come up
       //    in the wrong state during watchdog reset.
       
       for (m = 0; m < num_ch; m++) 
       {
           // For each phy on each channel, stop the clock from the MC
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 1);
           
           // For each phy, set the GFM to use the GCC clock
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_1_CFG, GCC_MODE, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_1_CFG, GCC_MODE, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_1_CFG, GCC_MODE, 1);
           
           // For each channel, toggle the GFM reset high
           HWIO_OUTXF(REG_OFFSET_MCCC_SLV(m), MCCC_SLV_CLK_RST_CTL, MC_SLV1_ARES, 1);
       }
       // Wait a while with the GRM reset high
       wait_for_xo_clocks_sdi(SDI_RESET_WAIT_IN_XO_CLKS);
       
       // For each channel, set the GFM reset back low
       for (m = 0; m < num_ch; m++) 
       {
           HWIO_OUTXF(REG_OFFSET_MCCC_SLV(m), MCCC_SLV_CLK_RST_CTL, MC_SLV1_ARES, 0);
       }
       wait_for_xo_clocks_sdi(SDI_RESET_WAIT_IN_XO_CLKS);
       
       // Trigger the GFM latches again (high/wait/low/wait)
       HWIO_OUTXF(SEQ_DDR_SS_DPC_REG_DPCC_OFFSET, DPCC_REG_PHY_XO_CBCR, CLK_ARES, 1);
       wait_for_xo_clocks_sdi(SDI_RESET_WAIT_IN_XO_CLKS);
       HWIO_OUTXF(SEQ_DDR_SS_DPC_REG_DPCC_OFFSET, DPCC_REG_PHY_XO_CBCR, CLK_ARES, 0);
       wait_for_xo_clocks_sdi(SDI_RESET_WAIT_IN_XO_CLKS);
       
       for (m = 0; m < num_ch; m++) 
       {
           // For each phy on each channel, run XO clock to reset the GFM latch trigger
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, XO_CLK_ON, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, XO_CLK_ON, 1);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, XO_CLK_ON, 1);
           
           wait_for_xo_clocks_sdi(SDI_RESET_WAIT_IN_XO_CLKS);
           
           // For each phy on each channel, restart the MC clock
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 0);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 0);
           HWIO_OUTXF(REG_OFFSET_DDR_PHY_CH(m) + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, MC_QFI_CLK_SW_CLK_OFF, 0);
       }
   }
   
   
   // Disable downtime requests to proceed with frequency switch included in powerstate change
   DDRSS_Downtime_Request_Ctrl_sdi(0);

   
   // Request that the SHRM move the DDRSS to Powerstate D0
   /* Enabling RPMH port override control*/
   SHRM_rpmh_port_override_control_sdi(1);
 
   /* Powerstate D0 (active) Work request */
   SHRM_rsi_cmd_inject_sdi(RSI_SDI_POWERSTATE_D0_WORK_CMD);
   
   /* LB Powerup work request */
   SHRM_rsi_cmd_inject_sdi(RSI_SDI_POWERSTATE_LB0_WORK_CMD);
 
   /* Disabling RPMH port override control*/
   SHRM_rpmh_port_override_control_sdi(0);
       
   if ((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (shrm_ddr_ptr->misc.chip_version == 0x0200)
      && ((shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect & DDR_CS1) == 0))
   {
       // Workaround for CSPD stuck on inactive rank issue (NAPALI-1079):
       // Disable the CSPD feature after the frequency switch.
	   for (m = 0; m < 4; m++)
	   {
           HWIO_OUTXFI2 (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CFG_m_CS_n, m, 1, ACTIVE_POWER_DOWN_EN, 1);
           HWIO_OUTXFI2 (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CFG_m_CS_n, m, 1, IDLE_POWER_DOWN_EN, 1);
           HWIO_OUTXFI (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_APM_OPT_CTRL_m, m, CLOCK_STOP_EN, 1);
	   }

       
       HWIO_OUTXF (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_LOAD_CONFIG, LOAD_CONFIG, 1);
       while (HWIO_INXF (REG_OFFSET_MC_MPE(BROADCAST_CH_NUM), MPE_LOAD_CONFIG, LOAD_CONFIG) == 1);
   }

   

   return TRUE;
}


//================================================================================================//
// Wait for the specified number of XO clocks
//================================================================================================//
void wait_for_xo_clocks_sdi(uint32 xo_clocks)
{
   uint64 timer_end = 0;
   uint64 timer_read = 0;
   uint64 timer_last = 0;
   uint64 overflow = 0;


   // Get the current timer value and figure out what the ending timer value is
   timer_last = (uint64) HWIO_INX(SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DDRSS_GLOBALCNTR_CURRENT);
   timer_end = timer_last + (uint64) xo_clocks;
   overflow = 0;
   
   do
   {
	   // Check the current timer, and correct it if it has overflowed.
	   timer_read = (uint64) HWIO_INX(SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DDRSS_GLOBALCNTR_CURRENT);
	   if (timer_read < timer_last)
	   {
		   overflow = 0x100000000;
	   }
	   timer_last = timer_read;
	   timer_read += overflow;
	   // End if we have reached the final timer value.
   } while (timer_read < timer_end);
   
   return;
}


//================================================================================================//
// Enable or disable downtime requests in DDRSS DT Aggregator
//================================================================================================//
void DDRSS_Downtime_Request_Ctrl_sdi (uint8 enable)
{ 
   // enable = 1 -> DT_REQ_DISABLE = 0, enable DT aggregator
   // enable = 0 -> DT_REQ_DISABLE = 1, disable DT aggregator
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                0, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                1, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                2, DT_REQ_DISABLE, ~enable);
   HWIO_OUTXFI (SEQ_DDR_SS_DDR_REG_DDR_SS_REGS_OFFSET, DDR_SS_REGS_DT_AGG_REQ_CFG_n,\
                3, DT_REQ_DISABLE, ~enable);				
   
   // Current workaround in MC block to enable/disable Task 5 (Downtime Request) for boot time freq switch
   // TODO: may be removed once downtime request could be disabled correctly in DDRSS level
   // Enable/disable MC freq switch task 5 (Downtime Request)
   HWIO_OUTXFI (REG_OFFSET_MC_SHKE(BROADCAST_CH_NUM), SHKE_FSW_TASK_CFG_n, 5, TASK_EN, enable);
}






/*********************************************************************************
**********************************************************************************
Code from ddrss_shrm.c
**********************************************************************************
*********************************************************************************/

//================================================================================================//
// usage: 
// 1. call SHRM_rpmh_port_override_control_sdi(uint8 en) with en = 1
// 2. call SHRM_rsi_cmd_inject with task_sdi(cmd_payload), like ddr_clk_switch.
//    work function call be called multiple times here
// 3. call SHRM_rpmh_port_override_control_sdi(uint8 en) with en = 0
//================================================================================================//
void SHRM_rpmh_port_override_control_sdi(uint8 en)
{
	// change Override RPMh option in MUX sel to CSR, SHRM_RSI_PATH[SEL] PoR value = 0(enable)
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RSI_PATH, SEL, !en);

	// Override RPMh in MUX sel
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_EN, EN, en);
}

void SHRM_rsi_cmd_inject_sdi(uint32 cmd_payload)
{
	// specify payload before issuing trigger cmd
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_CMD, CMD_PAYLOAD, cmd_payload);

	// issuing cmd trigger
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER, TRIGGER, 1);
	
	// Poll SHRM_CSR_SHRM_SPROC_STATUS to ensure task completed and processor in WAITI state
	while (HWIO_INXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_SPROC_STATUS, STATE) != 1);
	
	// clearing cmd trigger
	HWIO_OUTXF (SEQ_DDR_SS_SHRM_CSR_SHRM_CSR_OFFSET, SHRM_CSR_SHRM_RPM_PORT_OVERRIDE_TRIGGER, TRIGGER, 0);
}

