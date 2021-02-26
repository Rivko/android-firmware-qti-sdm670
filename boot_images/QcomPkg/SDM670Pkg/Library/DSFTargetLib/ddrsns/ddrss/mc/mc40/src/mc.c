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

#include "mc.h"
#include "ddrss.h"

void MC_MR_Write (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data)
{
   uint8 ch     = BROADCAST_CH_NUM;
   uint8 num_ch = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   //Don't broadcast to all channels if not all selected	
   if (channel != ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask)
   {
    	ch=0;
   } 
    // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   //ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         // Single command triggerring, only use SHKE_CMD_SET_CFG_0
         MC_Cmd_Set_Cfg (ch, chip_select, 0, CMD_MRW, mr_addr, mr_data);
         // No wait time needed
         MC_Cmd_Stagger_Cfg (ch, 0, 0, 0, 0);
         // Trigger command and wait for completion
         MC_Cmd_Trigger(ch, 0x1); // Choose CMD_SET_CFG_0 in bit 0
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);

   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

uint32 MC_MR_Read (DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint32 mr_addr)
{
   uint8  ch              = 0;
   uint32 reg_data        = 0;
   uint32 reg_offset_shke = 0;

   ch = MC_CH_INX(channel);
   reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
   
   // Single command triggerring, only use SHKE_CMD_SET_CFG_0
   MC_Cmd_Set_Cfg (ch, chip_select, 0, CMD_MRR, mr_addr, 0x0);
   // No wait time needed
   MC_Cmd_Stagger_Cfg (ch, 0, 0, 0, 0);
   // Trigger command and wait for completion
   MC_Cmd_Trigger(ch, 0x1); // Choose CMD_SET_CFG_0 in bit 0

   if((chip_select & DDR_CS0) != 0) {
      reg_data = HWIO_INX (reg_offset_shke, SHKE_MR_RDATA_RANK0);
   }
   else if((chip_select & DDR_CS1) != 0) {
      reg_data = HWIO_INX (reg_offset_shke, SHKE_MR_RDATA_RANK1);
   }
   return (reg_data);
}

void MC_Timing_Recalc_Update (uint8 ch)
{
   uint32 reg_offset_dtc = 0;

   reg_offset_dtc = REG_OFFSET_MC_DTC(ch);

   // Kick off timing parameter calculation and wait until done
   // Set MC_DTC_SW_TRIGGER_CFG[RECALC_TIMERS] = 1
   HWIO_OUTX (reg_offset_dtc, DTC_SW_TRIGGER_CFG, 0x1);
   // Poll for MC_DTC_SW_TRIGGER_CFG[RECALC_TIMERS] and wait for the read value to become zero.
   while (HWIO_INX (reg_offset_dtc, DTC_SW_TRIGGER_CFG) != 0x0);

   // Update all the calculated DTC timing values and wait until done
   // Set MC_DTC_SW_TRIGGER_CORE[UPDATE_TIMERS] = 1
   HWIO_OUTX (reg_offset_dtc, DTC_SW_TRIGGER_CORE, 0x1);
   // Poll for MC_DTC_SW_TRIGGER_CORE[UPDATE_TIMERS] and wait for the read value to become zero.
   while (HWIO_INX (reg_offset_dtc, DTC_SW_TRIGGER_CORE) != 0x0);  
}

void MC_Periodic_Auto_Refresh_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint8  rank_en         = 0;
   uint32 reg_offset_shke = 0;
 
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);

         if (enable == 0x1) {
            // Set rank enable bit(s) based on chip_select
            rank_en = chip_select;
         }
         else {
            // Clear rank enable bit(s) to 0
            rank_en = 0;
         }

         if (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))) {
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 0, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 1, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 2, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 3, REFRESH_PULL_IN_LIMIT_EN, enable);
         }
         // Enable/disable periodic refresh
         HWIO_OUTXF2 (reg_offset_shke, SHKE_REFRESH_EVENT_CTRL, REFRESH_PB_RANK_EN, RANK_EN, rank_en, rank_en);

         // Set SHKE_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
         HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1);
         // Poll for LOAD_CONFIG to become zero
         while (HWIO_INX(reg_offset_shke, SHKE_LOAD_CONFIG) != 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_All_Periodic_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint8  rank_en         = 0;
   uint32 reg_offset_shke = 0;
 
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);

         if (enable == 0x1) {
            // Set rank enable bit(s) based on chip_select
            rank_en = chip_select;
         }
         else {
            // Clear rank enable bit(s) to 0
            rank_en = 0;
         }

         if (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))) {
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 0, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 1, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 2, REFRESH_PULL_IN_LIMIT_EN, enable);
            HWIO_OUTXFI (reg_offset_shke, SHKE_REFRESH_DEBIT_CTRL_n, 3, REFRESH_PULL_IN_LIMIT_EN, enable);
         }
         // 1. Enable/disable periodic refresh
         HWIO_OUTXF2 (reg_offset_shke, SHKE_REFRESH_EVENT_CTRL, REFRESH_PB_RANK_EN, RANK_EN, rank_en, rank_en);

         // 2. Enable/disable periodic MR4 read
         if (ddr->shrm_ddr_ptr->ddr_runtime.MR4_polling_enable == 1) {
         HWIO_OUTXFI (reg_offset_shke, SHKE_INTERVAL_EVENT_CTRL_n, 0, RANK_EN, rank_en);
         
         /*Store event enabling info in shrm_ddr_ptr->periodic_events_enable_flag    
          * required while power wakeup sequence in SHRM
          *  periodic_event_enable_flag bit mapping as 
          * bit 0 =1 if SHKE_INTERVAL_EVENT_CTRL_0 is enable  
          * bit 1 =1 if SHKE_ZQSTART_EVENT_CTRL is enable
          * bit 2 =1 if SHKE_ZQLATCH_EVENT_CTRL is enable
          * bit 3 =1 if SHKE_PDT_TRAC_EVENT_CTRL is enable
          */
         ddr->shrm_ddr_ptr->periodic_events_enable_flag |= ((enable & 0x1) << SHKE_INTERVAL_EVENT_CTRL_0_EN);
         ddr->shrm_ddr_ptr->periodic_events_enable_flag &= ~((~enable & 0x1) << SHKE_INTERVAL_EVENT_CTRL_0_EN);
         }
 
         // 3. Enable/disable periodic ZQ calibration
         if (ddr->shrm_ddr_ptr->ddr_runtime.periodic_zq_calibration_enable == 1) {
             // SW ZQCAL workaround only applies to Napali V1 Samsung DRAM
             if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
                (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100) && \
                (ddr->shrm_ddr_ptr->detected_ddr_device[0].manufacturer_id == SAMSUNG)) {
                if (enable == 1) {
                   ddr->shrm_ddr_ptr->sw_zqcal.event_enable = 0x1;
                   ddr->shrm_ddr_ptr->sw_zqcal.event_status = 0x1;

            }
            else if (enable == 0) {
               ddr->shrm_ddr_ptr->sw_zqcal.event_enable = 0x0;
               ddr->shrm_ddr_ptr->sw_zqcal.event_status = 0x0;
            }
            if (chip_select == DDR_CS_BOTH) {
               // 32ms for dual rank
               // interval = 32ms/(1/400MHz) = 32*1000*400 = 12800000 cycle if SHRM CLK = 400MHz
               ddr->shrm_ddr_ptr->sw_zqcal.interval = 32*1000*400; 
            }
            else {
               // 64ms for single rank
               // interval = 64ms/(1/400MHz) = 64*1000*400 = 25600000 cycle if SHRM CLK = 400MHz
               ddr->shrm_ddr_ptr->sw_zqcal.interval = 64*1000*400; 
            }
            if (chip_select == DDR_CS1) {
               ddr->shrm_ddr_ptr->sw_zqcal.rank_flag = 1;
            }
            else { /* both CS0 and dual rank case, initial rank_flag = 0 */
               ddr->shrm_ddr_ptr->sw_zqcal.rank_flag = 0;
            }               
         }
         else {   
            HWIO_OUTXF (reg_offset_shke, SHKE_ZQSTART_EVENT_CTRL, RANK_EN, rank_en);
           ddr->shrm_ddr_ptr->periodic_events_enable_flag |= ((enable & 0x1) << SHKE_ZQSTART_EVENT_CTRL_EN);
           ddr->shrm_ddr_ptr->periodic_events_enable_flag &= ~((~enable & 0x1) << SHKE_ZQSTART_EVENT_CTRL_EN);
            
                HWIO_OUTXF (reg_offset_shke, SHKE_ZQLATCH_EVENT_CTRL, RANK_EN, rank_en);
               ddr->shrm_ddr_ptr->periodic_events_enable_flag |= ((enable & 0x1) << SHKE_ZQLATCH_EVENT_CTRL_EN);
               ddr->shrm_ddr_ptr->periodic_events_enable_flag &= ~((~enable & 0x1) << SHKE_ZQLATCH_EVENT_CTRL_EN);
             }
         }

         // 4. Enable/disable periodic DIT (DQS Oscillator Interval Timer)
         // Moved to HAL_DDR_Periodic_Training API 
         
         // Set SHKE_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
         HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1);
         // Poll for LOAD_CONFIG to become zero
         while (HWIO_INX(reg_offset_shke, SHKE_LOAD_CONFIG) != 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_Power_Saving_Ctrl (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select, uint8 enable)
{
   uint8  ch                = 0;
   uint8  num_ch            = 0;
   uint8  index             = 0;
   uint8  idx_m             = 0;
   uint8  idx_n             = 0;
   uint8  clk_idx           = 0;
   uint32 reg_offset_mpe    = 0;
   uint32 reg_offset_osu    = 0;
   uint32 reg_offset_global = 0;
      
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   clk_idx = DDRSS_Find_Clk_Index(ddr, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz);
  
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config( channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {   
         reg_offset_mpe = REG_OFFSET_MC_MPE(ch);
         
         // 1. Enable/disable CSPD
         // TODO: set all QoS registers to the same value for now. May update to use different values if needed.
         for (index = 0; index < 4; index++) {
            if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
                (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)) {			 
               HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 0, IDLE_POWER_DOWN_EN, enable);
               HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 0, ACTIVE_POWER_DOWN_EN, enable);
               HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 1, IDLE_POWER_DOWN_EN, enable);
               HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 1, ACTIVE_POWER_DOWN_EN, enable);
            }
            else { /* Not Napali V1 */
               if ((chip_select & DDR_CS0) != 0) {
                  HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 0, IDLE_POWER_DOWN_EN, enable);
                  HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 0, ACTIVE_POWER_DOWN_EN, enable);
               }
               if ((chip_select & DDR_CS1) != 0) {
                  HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 1, IDLE_POWER_DOWN_EN, enable);
                  HWIO_OUTXFI2 (reg_offset_mpe, MPE_APM_OPT_CFG_m_CS_n, index, 1, ACTIVE_POWER_DOWN_EN, enable);
               }
            }
            HWIO_OUTXFI (reg_offset_mpe, MPE_APM_OPT_CTRL_m, index, CLOCK_STOP_EN, enable);
         }
         // Set MPE_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
         HWIO_OUTX (reg_offset_mpe, MPE_LOAD_CONFIG, 0x1);
         // Poll for LOAD_CONFIG to become zero
         while (HWIO_INX(reg_offset_mpe, MPE_LOAD_CONFIG) != 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
    
   // 2. Enable/disable HW activity based self-refresh
   MC_HW_Self_Refresh_Ctrl (ddr->shrm_ddr_ptr, channel, chip_select, enable);

   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {   
         reg_offset_osu = REG_OFFSET_MC_OSU(ch);
         reg_offset_global = REG_OFFSET_MC_GLOBAL(ch);

         // 3. Enable/disable root clock gating (except Napali V1)
         if (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100))) {
            HWIO_OUTXF (reg_offset_global, GLOBAL_RTCG_CFG, RTCG_EN, enable);
            // Set GLOBAL_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
            HWIO_OUTX (reg_offset_global, GLOBAL_LOAD_CONFIG, 0x1);
            // Poll for LOAD_CONFIG to become zero
            while (HWIO_INX(reg_offset_global, GLOBAL_LOAD_CONFIG) != 0x0);
         }

         // 4. Enable QFI power state transitions
         if (enable == 1) {
            for (idx_m = 0; idx_m < 4; idx_m++) {
               for (idx_n = 0; idx_n < 4; idx_n++) {
                  HWIO_OUTXI2 (reg_offset_osu, OSU_PWRS_m_n_CFG, idx_m, idx_n,\
                               ddr->shrm_ddr_ptr->ddr_runtime.mc_pwrs_cfg[clk_idx][idx_m][idx_n]);
               }
            }
         }
         else if (enable == 0) {
            for (idx_m = 0; idx_m < 4; idx_m++) {
               for (idx_n = 0; idx_n < 4; idx_n++) {
                  HWIO_OUTXI2 (reg_offset_osu, OSU_PWRS_m_n_CFG, idx_m, idx_n, 0x0);
               }
            }
         }	 
         // Set OSU_LOAD_CONFIG[LOAD_CONFIG] = 1 for the above settings to take effect 
         HWIO_OUTX (reg_offset_osu, OSU_LOAD_CONFIG, 0x1);
         // Poll for LOAD_CONFIG to become zero
         while (HWIO_INX(reg_offset_osu, OSU_LOAD_CONFIG) != 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);   
}

void MC_ZQ_Calibration (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch              = 0;
   uint8  cs              = 0;
   uint8  num_ch          = 0;
   uint32 reg_offset_shke = 0;

   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;
   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {        
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
         if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
            (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)) {
            for (cs = 0; cs < NUM_CS; cs++) {
               if ((chip_select & CS_1HOT(cs)) != 0) { // Rank exists
                  MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 0, CMD_MC_SW_STALL, 0, 0);
                  MC_Cmd_Stagger_Cfg(ch, 0, 0x0, 0x0, 0x0);
                  
                  // Set ZQCAL START
                  MC_Cmd_Set_Cfg(ch, CS_1HOT(cs), 1, CMD_MPC_ZQCAL_START, 0, 0);
                  MC_Cmd_Stagger_Cfg(ch, 1, 0x0, 0, 0); 
                  
                  MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 2, CMD_MC_SW_STALL_EXIT, 0, 0);
                  MC_Cmd_Stagger_Cfg(ch, 2, 0x0, 0x0, 0x0);
                  
                  // PRIORITY = 0x3
                  // CMD_SELECT = 0x7 -> Selecting commands 0-2
                  HWIO_OUTX (reg_offset_shke, SHKE_SW_CMD_SEQ, ((0x3 << 28) | 0x7));
                  HWIO_OUTX (reg_offset_shke, SHKE_SW_CMD_TRIGGER, 0x1); 
                  // Poll for MC_SW_CMD_TRIGGER[SEQ_TRIGGER] and wait for the read value to become zero
                  while (HWIO_INX (reg_offset_shke, SHKE_SW_CMD_TRIGGER) != 0x0);
                  
                  MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 3, CMD_NOP, 0, 0);
                  // 2us (0x28 * 52ns) between ZQSTART to ZQLATCH
                  MC_Cmd_Stagger_Cfg(ch, 3, 0x0, WAIT_XO_CLOCK, 0x28);
                  
                  // Set ZQCAL LATCH 
                  MC_Cmd_Set_Cfg(ch, CS_1HOT(cs), 4, CMD_MPC_ZQCAL_LATCH, 0, 0);
                  MC_Cmd_Stagger_Cfg(ch, 4, 0x0, 0x0, 0x0);
                  
                  // PRIORITY = 0x3
                  // CMD_SELECT = 0x18 -> Selecting commands 3-4
                  HWIO_OUTX (reg_offset_shke, SHKE_SW_CMD_SEQ, ((0x3 << 28) | 0x18));
                  HWIO_OUTX (reg_offset_shke, SHKE_SW_CMD_TRIGGER, 0x1); 
                  // Poll for MC_SW_CMD_TRIGGER[SEQ_TRIGGER] and wait for the read value to become zero
                  while (HWIO_INX (reg_offset_shke, SHKE_SW_CMD_TRIGGER) != 0x0);
               } // end of if(rank exits)
            } // end of for(cs)
         } // end of if(Napali V1) 
         // Other than Napali V1
         else {
            /* 1. Program for the ZQ Calibration Start command and configure HW to 
               wait for tZQCAL time before executing ZQ Calibration Latch command. */
            MC_Cmd_Set_Cfg (ch, chip_select, 0, CMD_MPC_ZQCAL_START, 0x0, 0x0);
            // tZQCAL has 1ps resolution, convert to XO_PERIOD. 
            // clk_sel = 0x0 -> XO clk (19.2MHz), interval = 0x14 -> 20*(1/19.2MHz) = 1us = tZQCAL_in_ps
            MC_Cmd_Stagger_Cfg(ch, 0, 0x1/* for shared ZQ resistor */, WAIT_XO_CLOCK, 0x14); 
            
            // ZQCAL_LATCH does not need wait timer. tZQLAT timing is already taken care while
            // executing ZQCAL_LATCH. Hence set the wait timer to 0.
            MC_Cmd_Set_Cfg (ch, chip_select, 1, CMD_MPC_ZQCAL_LATCH, 0x0, 0x0);
            MC_Cmd_Stagger_Cfg (ch, 1, 0, 0, 0);
            
            // Trigger command and wait for completion
            MC_Cmd_Trigger(ch, 0x3); /* selecting commands 0-1 */
         }
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_Watchdog_Reset_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint32 reg_offset_shke = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   
   // Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0)
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
         
         /* 1. Configure the MRW execution to switch the FSP-WR to update the settings of non-operating set. 
            For the MR_DATA, OP[7-6] will be overwritten by HW and the programmed value doesn't matter.
            OP[5-0] should be the DRAM defaults/correspond to low DDR frequency. */
         // RRO(OP[4]) = 0x1, all other OPs in MR13 are set as default value of 0x0
         MC_Cmd_Set_Cfg (ch, chip_select, 13, CMD_MRW, JEDEC_MR_13, (0x10 | MR13_BASE)); //enable RRO and disable DM
         MC_Cmd_Stagger_Cfg (ch, 13, 0, 0, 0);

         /* 2. Configure the MRW execution to update nWR, RL/WL in the DRAM as per the low DDR frequency. */
         // Choose lowest DDR frequency index 0x0 for extended_cdt_runtime.dram_latency and extended_cdt_runtime.mc_freq_switch
         MC_Cmd_Set_Cfg (ch, chip_select, 14, CMD_MRW, JEDEC_MR_2,\
                         ddr->shrm_ddr_ptr->ddr_runtime.dram_latency[0].MR2);
         MC_Cmd_Stagger_Cfg (ch, 14, 0, 0, 0);
         MC_Cmd_Set_Cfg (ch, chip_select, 15, CMD_MRW, JEDEC_MR_1,\
                         MC_MR1_Value(ddr->shrm_ddr_ptr, ch, 0, 0));
         MC_Cmd_Stagger_Cfg (ch, 15, 0, 0, 0);
         MC_Cmd_Set_Cfg (ch, chip_select, 16, CMD_MRW, JEDEC_MR_3,\
                         ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[0].MR3);
         MC_Cmd_Stagger_Cfg (ch, 16, 0, 0, 0);

         /* 3. Configure the MRW execution to setup ODT values as per the low DDR frequency */
         MC_Cmd_Set_Cfg (ch, chip_select, 17, CMD_MRW, JEDEC_MR_11,\
                         0 /* TODO: CA/DQ ODT values */);
         MC_Cmd_Stagger_Cfg (ch, 17, 0, 0, 0);
         MC_Cmd_Set_Cfg (ch, chip_select, 18, CMD_MRW, JEDEC_MR_22,\
                         0 /* TODO: SoC ODT etc */);
         MC_Cmd_Stagger_Cfg (ch, 18, 0, 0, 0);

         /* 4. Configure the MRW execution to setup CA/DQ Vref values as per the low DDR frequency */
         MC_Cmd_Set_Cfg (ch, chip_select, 19, CMD_MRW, JEDEC_MR_12,\
                         0x4D/* TODO: CA Vref value */);
         MC_Cmd_Stagger_Cfg (ch, 19, 0, 0, 0);
         MC_Cmd_Set_Cfg (ch, chip_select, 20, CMD_MRW, JEDEC_MR_14,\
                         0x4D/* TODO: DQ Verf value */);
         MC_Cmd_Stagger_Cfg (ch, 20, 0, 0, 0);

         /* 5. Configure to do FSP-OP switch. 
            For the MR_DATA, OP[7-6] will be overwritten by HW and the programmed value doesn't matter. 
            OP[5-0] should be the DRAM defaults/correspond to low DDR frequency with high current mode enabled. */
         MC_Cmd_Set_Cfg (ch, chip_select, 21, CMD_MRW_FSP_OP, JEDEC_MR_13, (0x18 | MR13_BASE)); //enable RRO and disable DM
         MC_Cmd_Stagger_Cfg (ch, 21, 0, 0, 0);

         /* Configure to disable VRCG */
         MC_Cmd_Set_Cfg (ch, chip_select, 22, CMD_MRW_VRCG_DIS, JEDEC_MR_13, (0x10 | MR13_BASE)); //enable RRO and disable DM
         MC_Cmd_Stagger_Cfg (ch, 22, 0, 0, 0);

         /* 6. Configure to enter CSPD. */
         MC_Cmd_Set_Cfg (ch, DDR_CS_BOTH, 23, CMD_POWER_DOWN_CSPDE, 0x0, 0x0);
         /* Stagger interval is equal to the cycles required for CK_OFF opcode propagated and processed in MC clock */
         /* Clock plan index 2 (mc_pwrs_extend_cycle[2]) is equal to boot frequency 200MHz */
         MC_Cmd_Stagger_Cfg (ch, 23, 0, WAIT_CORE_CLOCK, 
                             ddr->shrm_ddr_ptr->ddr_runtime.mc_pwrs_extend_cycle[2].ca_act_pwrs_extend);

         /* 7. Select the commands to be executed when watchdog reset occurs. */
         HWIO_OUTX (reg_offset_shke, SHKE_WDOG_CMD_SEQ, 0xFFE000); /* selecting command 13-23 */
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);

   // Disable MC broadcast
   //DDRSS_Broadcast_MC_Config(DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}


