/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
================================================================================*/

#include "mc_common.h"


void MC_Cmd_Set_Cfg (uint8 ch, uint32 chip_select, uint8 reg_num,\
                     uint32 event_sel, uint32 mr_addr, uint32 mr_data)
{
   uint32 reg_value = 0;

   // Combine field values to create single register write
   reg_value = ((mr_data << 24)     & 0xFF000000) /* MR_DATA   [31:24] */ | \
               ((mr_addr << 16)     & 0x00FF0000) /* MR_ADDR   [23:16] */ | \
               ((chip_select << 12) & 0x00003000) /* RANK_SEL  [13:12] */ | \
               ((event_sel)         & 0x000003FF) /* EVENT_SEL [9:0]   */ ;

   HWIO_OUTXI (REG_OFFSET_MC_SHKE(ch), SHKE_CMD_SET_CFG_n, reg_num, reg_value);
}
    
void MC_Cmd_Stagger_Cfg (uint8 ch, uint8 reg_num, uint32 stagger_en,\
                         uint32 clk_sel, uint32 interval)
{
   uint32 reg_value = 0;

   // Combine field values to create single register write
   reg_value = ((stagger_en << 20) & 0x00100000) /* STAGGER_RANKS_EN [20]    */ | \
               ((clk_sel    << 16) & 0x00030000) /* CLK_SELECT       [17:16] */ | \
               ((interval)         & 0x0000FFFF) /* INTERVAL         [15:0]  */ ;
   
   HWIO_OUTXI (REG_OFFSET_MC_SHKE(ch), SHKE_CMD_SET_STAGGER_CFG_n, reg_num, reg_value);
}

void MC_Cmd_Trigger (uint8 ch, uint32 cmd_sel)
{
   uint32 reg_offset = 0;

   reg_offset = REG_OFFSET_MC_SHKE(ch);

   // Set the trigger for HW to execute the commands selected by CMD_SELECT
   HWIO_OUTX (reg_offset, SHKE_SW_CMD_SEQ, cmd_sel); // Assume PRIORITY field [29:28] is the same across the channels
   HWIO_OUTX (reg_offset, SHKE_SW_CMD_TRIGGER, 0x1); 
      
   // Poll for MC_SW_CMD_TRIGGER[SEQ_TRIGGER] and wait for the read value to become zero
   while (HWIO_INX (reg_offset, SHKE_SW_CMD_TRIGGER) != 0x0);
}
 
uint8 MC_MR1_Value (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint8 ch, uint8 RL_WL_idx, uint8 params_idx)
{
   uint8 mr1_value = 0;

   mr1_value = (HWIO_INX (REG_OFFSET_MC_MPE(ch), MPE_CCP_OPT_CFG) & 0x03)                           /* MR1[1:0] = Burst Length */ |\
               ((shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].wr_pre << 2) & 0x04) /* MR1[2]   = WR-PRE       */ |\
               ((shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].rd_pre << 3) & 0x08) /* MR1[3]   = RD-PRE       */ |\
               ((RL_WL_idx << 4) & 0x70) /* nWR located in RL_WL_lpddr_struct, use RL_WL_idx */     /* MR1[6:4] = nWR          */ |\
               ((shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].rpst << 7) & 0x80);  /* MR1[7]   = RPST         */

   return mr1_value;
}

uint8 MC_DRAM_Latency_Index (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint32 clk_in_ps)
{
   uint8 clk_idx = 0;

   /* Cover the case of clk_in_ps with initial value 0 */
   if (clk_in_ps == 0) { 
      return 0;
   }
   else {
      for (clk_idx = 0; clk_idx < NUM_DRAM_LATENCY_STRUCTS; clk_idx++) {
       if (clk_in_ps >= shrm_ddr_ptr->ddr_runtime.dram_latency[clk_idx].clk_in_ps)
          break;
      }
      // Make sure we don't return an index past the end of the table
      return MIN(clk_idx, (NUM_DRAM_LATENCY_STRUCTS-1));
   }
}

uint8 MC_Freq_Switch_Params_Index (SHRM_DDR_STRUCT *shrm_ddr_ptr, uint32 clk_in_ps)
{
   uint8 clk_idx = 0;

   /* Cover the case of clk_in_ps with initial value 0 */
   if (clk_in_ps == 0) { 
      return 0;
   }
   else {   
      for (clk_idx = 0; clk_idx < NUM_FREQ_SWITCH_STRUCTS; clk_idx++) {
          if (clk_in_ps >= shrm_ddr_ptr->ddr_runtime.mc_freq_switch[clk_idx].clk_in_ps)
             break;
      }
      // Make sure we don't return an index past the end of the table
      return MIN(clk_idx, (NUM_FREQ_SWITCH_STRUCTS-1));
   }
}

void MC_Enter_Self_Refresh (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch                = 0;
   uint8  num_ch            = 0;  
   uint32 reg_offset_shke   = 0;
   uint32 reg_offset_global = 0;
   uint32 reg_offset_osu    = 0;

   num_ch = shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL)(channel & shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_shke   = REG_OFFSET_MC_SHKE(ch);
         reg_offset_global = REG_OFFSET_MC_GLOBAL(ch);
         reg_offset_osu    = REG_OFFSET_MC_OSU(ch);

         /* 2. Configure the below register to error out on the new RD/WR traffic that is being sent to MC.
            SW has to ensure that no new traffic comes to MC before doing this step. */
         HWIO_OUTX (reg_offset_shke, SHKE_INIT_CONFIG, 0x0);

         /* 3. Disable the periodic ZQ calibration and periodic MR4 reads. */
         if ((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (shrm_ddr_ptr->misc.chip_version == 0x0100)) {
            shrm_ddr_ptr->sw_zqcal.event_enable = 0x0;
         }
         else {
#if 0 /* Disabled ZQ_CG_EN and ASYNC_CG_EN in one-time settings to avoid extra long clock switch latency */
            /* Disable ZQ_CG_EN and AYSNC_CG_EN before disabling periodic ZQ */
            HWIO_OUTXF2 (reg_offset_shke, SHKE_CLK_CTRL, ZQ_CG_EN, EVENT_ASYNC_CG_EN, 0x0, 0x0);
#endif

            HWIO_OUTXF  (reg_offset_shke, SHKE_ZQSTART_EVENT_CTRL, RANK_EN, 0x0);
            HWIO_OUTXF  (reg_offset_shke, SHKE_ZQLATCH_EVENT_CTRL, RANK_EN, 0x0);
         }
         HWIO_OUTXFI (reg_offset_shke, SHKE_INTERVAL_EVENT_CTRL_n, 0, RANK_EN, 0x0);
         HWIO_OUTXF  (reg_offset_shke, SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, 0x0);
         HWIO_OUTXF  (reg_offset_shke, SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, 0x0);

         /* 4. Set the below register for the above settings to take effect. */
         HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1);

         /* 5. Set the SW self refresh vote to put the DRAM into self refresh. */
         HWIO_OUTX (reg_offset_shke, SHKE_SW_SELF_REFRESH_VOTE, chip_select);
     }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   
   // Change to AND each bit of broadcast read data from each channel (polling for 1)
   DDRSS_Broadcast_MC_Config(channel, AND_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_global = REG_OFFSET_MC_GLOBAL(ch);
         reg_offset_osu    = REG_OFFSET_MC_OSU(ch);
        
         /* 6. Poll for SW self refresh status (RANKS_IN_SELFRFRSH (bits[9:8]))
            and CSPD status (RANKS_IN_CSPD (bits[5:4])) of each rank present to be 1 */
         if ((chip_select & DDR_CS0) != 0x0) {
            while ((HWIO_INX (reg_offset_global, GLOBAL_DEVICE_STATUS) & 0x110) != 0x110);
         }
         if ((chip_select & DDR_CS1) != 0x0) {
            while ((HWIO_INX (reg_offset_global, GLOBAL_DEVICE_STATUS) & 0x220) != 0x220);
         }

         /* 7. Poll for RCH FIFO to be empty ensuring that upstream has accepted all the read data */
         while ((HWIO_INX (reg_offset_global, GLOBAL_ISU_RCH_STATUS) & 0x1) != 0x1);

         /* Poll for idle registers to be set to ensure that the clocks are not needed anymore by CABO.
            Once set, it indicates that the power collapse entry can now be initiated */
         /* Not applied to Napali V1 */
         if (!((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (shrm_ddr_ptr->misc.chip_version == 0x0100))) {
            while ((HWIO_INXF (reg_offset_osu, OSU_IDLE_STATUS, PWRS_CTRL_IDLE)) != 0x1);
            while ((HWIO_INXF (reg_offset_global, GLOBAL_IDLE_STATUS, RANK_IDLE)) != 0x3);
         }
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);

   // Change to OR each bit of broadcast read data from each channel (polling for 0)
   DDRSS_Broadcast_MC_Config(channel, OR_RDATA);

   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);  
}
       
void MC_Exit_Self_Refresh (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch                          = 0;
   uint8  num_ch                      = 0;
   uint8  shke_interval_event_en      = 0;
   uint8  shke_ZQStart_event_en       = 0;
   uint8  shke_ZQLatch_event_en       = 0;
   uint8  shke_pdt_trac_event_en      = 0;
   uint32 periodic_events_enable_flag = 0; 
   uint32 reg_offset_shke             = 0;

   num_ch  = shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL)(channel & shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   /* periodic_event_enable_flag bit mapping as 
    * bit 0 =1 if SHKE_INTERVAL_EVENT_CTRL_0 is enable  
    * bit 1 =1 if SHKE_ZQSTART_EVENT_CTRL is enable
    * bit 2 =1 if SHKE_ZQLATCH_EVENT_CTRL is enable
    * bit 3 =1 if SHKE_PDT_TRAC_EVENT_CTRL is enable
    */
   periodic_events_enable_flag = shrm_ddr_ptr->periodic_events_enable_flag;
   shke_interval_event_en = (periodic_events_enable_flag >> SHKE_INTERVAL_EVENT_CTRL_0_EN )& 0x01 ;
   shke_ZQStart_event_en  = (periodic_events_enable_flag >> SHKE_ZQSTART_EVENT_CTRL_EN ) & 0x01;
   shke_ZQLatch_event_en  = (periodic_events_enable_flag >> SHKE_ZQLATCH_EVENT_CTRL_EN ) & 0x01;
   shke_pdt_trac_event_en = (periodic_events_enable_flag >> SHKE_PDT_TRAC_EVENT_CTRL_EN) & 0x01;

   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {        
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
        
         /* 9. Clear the SW self refresh vote */
         HWIO_OUTX (reg_offset_shke, SHKE_SW_SELF_REFRESH_VOTE, 0x0);

         /* 10. Program the MR4 read command for two ranks respectively. */
         /* a. Program the MR4 read command to Rank0. */
         MC_Cmd_Set_Cfg(ch, DDR_CS0, 10, CMD_MRR_MR4, 0x0, 0x0); 
         MC_Cmd_Stagger_Cfg(ch, 10, 0x0, 0x0, 0x0);
         /* b. Set the trigger for HW to execute the commands programmed above */
         /* c. Poll for the below register and wait for the read value to become zero */
         MC_Cmd_Trigger(ch, 0x400); // Selecting command 10
         if ((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (shrm_ddr_ptr->misc.chip_version == 0x0100)) {
            /* d. Trigger a SW timer of 2us and wait for expiration */
            seq_wait(2, US);
         }
         else { /* Not Napali V1 */
            /* d. Poll for the below register and wait for the read value to become zero */
            while ((HWIO_INXF (reg_offset_shke, SHKE_DTC_CTRL_STATUS, FSM_STATE)) != 0x0);
         }
         if (chip_select == DDR_CS_BOTH) {
            /* e. Program the MR4 read command to Rank0. */
            MC_Cmd_Set_Cfg(ch, DDR_CS1, 10, CMD_MRR_MR4, 0x0, 0x0); 
            MC_Cmd_Stagger_Cfg(ch, 10, 0x0, 0x0, 0x0);
            /* f. Set the trigger for HW to execute the commands programmed above */
            /* g. Poll for the below register and wait for the read value to become zero */
            MC_Cmd_Trigger(ch, 0x400); // Selecting command 10
         }

         /* 11. Program for the ZQ Calibration Start command and configure HW to wait for tZQCAL time 
            before executing ZQ Calibration Latch command */
         MC_Cmd_Set_Cfg (ch, chip_select, 11, CMD_MPC_ZQCAL_START, 0x0, 0x0);
         // clk_sel = 0x0 -> XO clk (19.2MHz), interval = 0x14 -> 20*(1/19.2MHz) = 1us = tZQCAL_in_ps
         MC_Cmd_Stagger_Cfg(ch, 11, 0x1/* for shared ZQ resistor */, WAIT_XO_CLOCK, 0x14); 

         /* 12. Program for the ZQ Calibration Latch command */
         /* ZQCAL_LATCH does not need wait timer. tZQLAT timing is already taken care while
            executing ZQCAL_LATCH. Hence set the wait timer to 0 */
         MC_Cmd_Set_Cfg (ch, chip_select, 12, CMD_MPC_ZQCAL_LATCH, 0x0, 0x0);
         MC_Cmd_Stagger_Cfg (ch, 12, 0, 0, 0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);

   // CMD triggering will not support broadcast, need to use for loop to trigger each channel in series   
   for (ch = 0; ch < num_ch; ch++) {
      if (((channel >> ch) & 0x1) == 0x1) {
         /* 13. Select the commands to be executed */
         /* 14. Set the trigger for HW to execute the commands programmed above */
         /* 15. Poll for CABO_SHKE_SW_CMD_TRIGGER[SEQ_TRIGGER] and wait for the read value to become zero */
         MC_Cmd_Trigger(ch, 0x1800); // selecting commands 11-12
      }
   }
   
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {      
         /* 16. Enable the periodic ZQ calibration and periodic MR4 reads */
         if ((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (shrm_ddr_ptr->misc.chip_version == 0x0100)) {     
            // Skip SW ZQCAl re-enable if it is in watchdog reset flow
            if (HWIO_INXF(REG_OFFSET_MC_SHKE(0), SHKE_SELF_REFRESH_STATUS, WDOG_SELF_REFRESH) != 0) {
               shrm_ddr_ptr->sw_zqcal.event_enable = shrm_ddr_ptr->sw_zqcal.event_status;
            }
         }
         else {
            if(shke_ZQStart_event_en) {
               HWIO_OUTXF (reg_offset_shke, SHKE_ZQSTART_EVENT_CTRL, RANK_EN, chip_select);
            } 
            if(shke_ZQLatch_event_en) {
               HWIO_OUTXF (reg_offset_shke, SHKE_ZQLATCH_EVENT_CTRL, RANK_EN, chip_select);
            }
         }

         if(shke_interval_event_en) {
            HWIO_OUTXFI (reg_offset_shke, SHKE_INTERVAL_EVENT_CTRL_n, 0, RANK_EN, chip_select);
         }

         if(shke_pdt_trac_event_en) {
            HWIO_OUTXF (reg_offset_shke, SHKE_PDT_TRAC_EVENT_CTRL, RANK_EN, chip_select);
            HWIO_OUTXF (reg_offset_shke, SHKE_PDT_ACQ_EVENT_CTRL, RANK_EN, chip_select);
         }
#if 0 /* Disabled ZQ_CG_EN and ASYNC_CG_EN in one-time settings to avoid extra long clock switch latency */
         if (!((shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (shrm_ddr_ptr->misc.chip_version == 0x0100))) {  
            /* Restore back the following two CG_EN */
            HWIO_OUTXF2 (reg_offset_shke, SHKE_CLK_CTRL, ZQ_CG_EN, EVENT_ASYNC_CG_EN, 0x1, 0x1);
         }
#endif
         /* 17. Enable MC to accept RD/WR traffic */
         HWIO_OUTX (reg_offset_shke, SHKE_INIT_CONFIG, chip_select);
       
         /* 18. Set the below register for the above settings to take effect */
         HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1); 
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_HW_Self_Refresh_Ctrl (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel,\
                              DDR_CHIPSELECT chip_select, uint8 enable)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint32 reg_offset_shke = 0;

   num_ch  = shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   
   for (ch = 0; ch < num_ch; ch++) {
      if (shrm_ddr_ptr->ddr_runtime.hw_self_refresh_enable[ch] == 1) {
         if (((channel >> ch) & 0x1) == 0x1) {
            reg_offset_shke = REG_OFFSET_MC_SHKE(ch);

            if (enable == 0x1) {
               HWIO_OUTXF (reg_offset_shke, SHKE_HW_SELF_REFRESH_CTRL, RANK_EN, chip_select);
            }
            else {
               HWIO_OUTXF (reg_offset_shke, SHKE_HW_SELF_REFRESH_CTRL, RANK_EN, 0);
            }
         
            // Set SHKE_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
            HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1);
         }
      }
   }
}

void MC_QFI_Clock_Gating_Ctrl (SHRM_DDR_STRUCT *shrm_ddr_ptr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   uint8  ch             = 0;
   uint8  num_ch         = 0;
   uint32 reg_offset_osu = 0;
     
   num_ch  = shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config( channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {   
         reg_offset_osu = REG_OFFSET_MC_OSU(ch);
         HWIO_OUTXF2 (reg_offset_osu, OSU_QFI_CLK_CTRL, QFI_CLK_SET, QFI_CLK_OVERRIDE_EN, (~enable & 0x1), (~enable & 0x1));
         HWIO_OUTX (reg_offset_osu, OSU_LOAD_CONFIG, 0x1);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);

   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

