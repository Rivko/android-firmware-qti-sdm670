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

void MC_Config(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch     = 0;
   uint8  num_ch = 0;   
   uint32 offset = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   /* Enable MC broadcast */
   //ch = DDRSS_Broadcast_MC_Config(channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {   
         offset = REG_OFFSET_MC(ch);
         /* One-time settings */
        DDRSS_set_config(ddr, offset,
            (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[MC_CFG_IDX][0]));

         /* Set LOAD_CONFIG = 1 for each module for above settings to take effect */
         /* No LOAD_CONFIG for DTC since timing parameters will be re-calculated
            in frequency switch */
         offset = REG_OFFSET_MC_GLOBAL(ch);
         HWIO_OUTX (offset, GLOBAL_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, GLOBAL_LOAD_CONFIG) != 0x0);

         offset = REG_OFFSET_MC_ISU(ch);
         HWIO_OUTX (offset, ISU_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, ISU_LOAD_CONFIG) != 0x0);
         
         offset = REG_OFFSET_MC_CBU_TOP(ch);
         HWIO_OUTX (offset, CBU_TOP_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, CBU_TOP_LOAD_CONFIG) != 0x0);
 
         offset = REG_OFFSET_MC_MPE(ch);
         HWIO_OUTX (offset, MPE_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, MPE_LOAD_CONFIG) != 0x0);

         offset = REG_OFFSET_MC_SHKE(ch);
         HWIO_OUTX (offset, SHKE_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, SHKE_LOAD_CONFIG) != 0x0);

         offset = REG_OFFSET_MC_OSU(ch);
         HWIO_OUTX (offset, OSU_LOAD_CONFIG, 0x1); 
         while (HWIO_INX(offset, OSU_LOAD_CONFIG) != 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   
   /* Disable MC broadcast */
   //DDRSS_Broadcast_MC_Config( DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_Pre_Init_Setup_1 (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint8  clk_idx         = 0;
   uint8  params_idx      = 0;
   uint8  odt_en          = 0;   
   uint8  rd_dbi_en       = 0;
   uint8  wr_dbi_en       = 0;
   uint32 clk_in_ps       = 0;   
   uint32 reg_offset_dtc  = 0;
   uint32 reg_offset_shke = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   
   /* Enable MC broadcast */
   //ch = DDRSS_Broadcast_MC_Config( channel, NO_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_dtc  = REG_OFFSET_MC_DTC(ch);
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);

         /* The following steps are from Pre DRAM Initialization section in MC4_1.0_HPG. */
         /* 1. Set up the following timing registers in ps 
            as per the DRAM timing settings from the device datasheet. */
         
         // Timing parameters moved to one-time settings

         // Timing parameter re-calculation will be done during frequency switch
 
         // Initialize DBI and ODT based on eCDT table
         clk_idx = DDRSS_Find_Clk_Index(ddr, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz);
         clk_in_ps = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[clk_idx].clk_period_in_psec;
         params_idx = MC_Freq_Switch_Params_Index (ddr->shrm_ddr_ptr, clk_in_ps);
   
         odt_en     = ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].odt;
         rd_dbi_en  = ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].rd_dbi;
         wr_dbi_en  = (ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].MR3 & 0x80) >> 7;

         HWIO_OUTX (reg_offset_dtc, DTC_ODT_CFG, odt_en);
         HWIO_OUTX (reg_offset_dtc, DTC_DBI_CFG, (rd_dbi_en) | (wr_dbi_en << 8));

         // 2. Set the below config register field for the freq switch to proceed to completion.
         // SHKE_CONFIG[RANK_EN] = 2'b11 (SHKE_CONFIG[DEVICE_CFG] will be updated in MC_Post_Init_Setup)
         HWIO_OUTX (reg_offset_shke, SHKE_CONFIG, (DDR_CS_BOTH << 4));
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   
   /* Disable MC broadcast */
   //DDRSS_Broadcast_MC_Config( DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_Pre_Init_Setup_2 (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;  
   uint32 reg_offset_shke = 0;
   uint32 reg_offset_osu  = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   /* Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0) */
   //ch = DDRSS_Broadcast_MC_Config( channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
         reg_offset_osu  = REG_OFFSET_MC_OSU(ch);

         /* The following steps are from Pre DRAM Initialization section in MC4_1.0_HPG.
            Step 1-2 are covered in MC_Pre_Init_Setup_1 and step 3 is frequency switch */	 
         // 4. Configure the HW to put CKE low that was asserted high as part of freq switch
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 0, CMD_POWER_DOWN_PDE, 0x0, 0x0);
         MC_Cmd_Stagger_Cfg(ch, 0, 0x0, 0x0, 0x0);
   
         // 5. Configure the HW to turn CK OFF that got turned ON as part of freq switch
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 1, CMD_INIT_CK_OFF, 0x0, 0x0);
         MC_Cmd_Stagger_Cfg(ch, 1, 0x0, 0x0, 0x0);
   
         // 6. Set the trigger for HW to execute the commands programmed above
         // 7. Poll for the below register and wait for the read value to become zero.
         MC_Cmd_Trigger(ch, 0x3); // Selecting commands 0-1
            
         // 8. Put back the below SHKE rank_en register field to 0.
         // SHKE_CONFIG[RANK_EN] = 2'b0 (SHKE_CONFIG[DEVICE_CFG] will be updated in MC_Post_Init_Setup)
         HWIO_OUTX (reg_offset_shke, SHKE_CONFIG, (DDR_CS_NONE << 4));

         // 9. Program SSP override CSR to 0 to allow SSP to capture and propagate all the commands from CABO.
         // (the reset value of this CSR is 1, so after reset, it is required to set to 0)
         HWIO_OUTX (reg_offset_osu, OSU_SPARE_CFG, 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   
   /* Disable MC broadcast */
   //DDRSS_Broadcast_MC_Config( DDR_CH_NONE/* disable broadcast */, NO_RDATA);
}

void MC_Memory_Device_Init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch          = 0;
   uint8  num_ch      = 0;
   uint8  clk_idx     = 0;
   uint8  latency_idx = 0;
   uint8  params_idx  = 0;
   uint32 clk_in_ps   = 0;
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   clk_idx = DDRSS_Find_Clk_Index(ddr, ddr->shrm_ddr_ptr->misc.current_clk_in_kHz);
   clk_in_ps = ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[clk_idx].clk_period_in_psec;
   latency_idx = MC_DRAM_Latency_Index(ddr->shrm_ddr_ptr, clk_in_ps);
   params_idx = MC_Freq_Switch_Params_Index (ddr->shrm_ddr_ptr, clk_in_ps);
     
   /* Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0) */
   //ch = DDRSS_Broadcast_MC_Config( channel, OR_RDATA);
   ch = BROADCAST_CH_NUM;

   do { 
      if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
         /* The following steps are from DRAM Initialization section in MC4_1.0_HPG.
            Steps 1-6 are skipped since pre DRAM initialization is done */
         /* 7. Configure the HW to wait for tINIT3 (2ms) time after DRAM is taken out of RESET 
            before executing the command in step 9 */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 0, CMD_NOP, 0, 0);
         // clk_sel = 0x1 -> sleep clk (32KHz), interval = 0x42 -> 64*(1/32KHz) > 2ms
         MC_Cmd_Stagger_Cfg(ch, 0, 0x0, WAIT_SLEEP_CLOCK, 0x42);
      
         /* 8. Program for the memory clock to be turned ON and configure the HW 
            to wait for tINIT4 (5tCK) time before executing event in step 9 */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 1, CMD_INIT_CK_ON, 0, 0); 
         // clk_sel = 0x2 -> core clk, interval = 0x5 -> 5tCK
         MC_Cmd_Stagger_Cfg(ch, 1, 0x0, WAIT_CORE_CLOCK, 0x5); 
      
         /* 9. Program the CKE to be asserted high and configure the HW to wait for 
            tINIT5 (2us) before executing event in step 10 */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 2, CMD_POWER_DOWN_PDX, 0, 0); 
         // clk_sel = 0x0 -> XO clk (19.2MHz), interval = 0x27 -> 39*(1/19.2MHz) = 2us 
         MC_Cmd_Stagger_Cfg(ch, 2, 0x0, WAIT_XO_CLOCK, 0x27); 
      
         /* 10. Program the MRW execution to update nWR, RL/WL and DBI in the DRAM 
            as per the DDR frequency */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 4, CMD_MRW, JEDEC_MR_2,\
                        ddr->shrm_ddr_ptr->ddr_runtime.dram_latency[latency_idx].MR2); 
         MC_Cmd_Stagger_Cfg(ch, 4, 0x0, 0x0, 0x0);
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 5, CMD_MRW, JEDEC_MR_1,\
                        MC_MR1_Value(ddr->shrm_ddr_ptr, ch, latency_idx, params_idx)); 
         MC_Cmd_Stagger_Cfg(ch, 5, 0x0, 0x0, 0x0);
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 6, CMD_MRW, JEDEC_MR_3,\
                        ddr->shrm_ddr_ptr->ddr_runtime.mc_freq_switch[params_idx].MR3); 
         MC_Cmd_Stagger_Cfg(ch, 6, 0x0, 0x0, 0x0);
         
         /* 11. Program for the ZQ Calibration Start command and 
            configure HW to wait for tZQCAL (1us) time before executing step 12 */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 13, CMD_MPC_ZQCAL_START, 0, 0); 
         // stagger_ranks_en = 0x1 -> for shared ZQ resistor 
         // clk_sel = 0x0 -> XO clk (19.2MHz), interval = 0x14 -> 20*(1/19.2MHz) = 1us = tZQCAL_in_ps
         MC_Cmd_Stagger_Cfg(ch, 13, 0x1/* for shared ZQ resistor */, WAIT_XO_CLOCK, 0x14); 
         
         /* 12. Program for the ZQ Calibration Latch command */
         MC_Cmd_Set_Cfg(ch, DDR_CS_BOTH, 14, CMD_MPC_ZQCAL_LATCH, 0, 0); 
         MC_Cmd_Stagger_Cfg(ch, 14, 0x0, 0x0, 0x0);
      }
      // Increase channel index to loop all channels selected in non-broadcast case
      ch++;
   } while (ch < num_ch);
   
   /* Disable MC broadcast */
   //DDRSS_Broadcast_MC_Config( DDR_CH_NONE/* disable broadcast */, NO_RDATA);

   // CMD triggering will not support broadcast, need to use for loop to trigger each channel in series   
   for (ch = 0; ch < num_ch; ch++) {
      if (((channel >> ch) & 0x1) == 0x1) {
         /* 13. Set the trigger for HW to execute the commands programmed in Steps 7-12. */
         /* 14. Poll for the below register and wait for the read value to become zero. */
         // CMD_SELECT = 0x6077 -> Selecting commands 0-2, 4-6, 13-14 
         MC_Cmd_Trigger(ch, 0x6077);
      }
   }

   /* 15. Program the MRW execution to update SoC ODT value (MR22) */
   /* 16. Set the trigger for HW to execute the commands programmed in Step 15. */
   /* 17. Poll for the below register and wait for the read value to become zero. */
   /* Set MR22 to 0 in memory device initialization, requested by PHY team */
   MC_MR_Write(ddr, channel, DDR_CS_BOTH, JEDEC_MR_22, 0x0);   
}
 
boolean MC_DDR_Geometry_Detection(BOOT_DDR_STRUCT *ddr, uint8 ch,\
                                  uint8 cs, uint32 mr8_value)
{
   uint8 device_density  = 0;
   uint8 device_io_width = 0;
   uint8 nrows           = 0;
   uint8 ncols           = 0;
   uint8 nbanks          = 8;

   device_density  = (mr8_value & 0x3C) >> 2;
   device_io_width = (mr8_value & 0xC0) >> 6;

   if (device_io_width == DEVICE_WIDTH_IS_x16) { /* x16 mode */
      nrows = lpddr_geometry_table[device_density][0][0];
      ncols = lpddr_geometry_table[device_density][0][1];
   }
   else if (device_io_width == DEVICE_WIDTH_IS_x8) { /* x8 mode */
      nrows = lpddr_geometry_table[device_density][1][0];
      ncols = lpddr_geometry_table[device_density][1][1];
   }
   
   ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_rows[cs]        = nrows;
   ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_cols[cs]        = ncols;
   ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_banks[cs]       = nbanks;
   ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs] = device_io_width;
   ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_density[cs]  = device_density;
   
   return TRUE;
}

void MC_DDR_Topology_Detection(BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch              = 0;
   uint8  num_ch          = 0;
   uint8  cs              = 0; 
   uint8  rank_timeout    = 0;
   uint8  i               = 0;
   uint32 ch_1hot         = 0;
   uint32 cs_1hot         = 0;
   uint32 mr8_value       = 0;
   uint32 reg_offset_shke = 0;
   uint32 reg_offset_mpe  = 0;
   uint32 reg_offset_osu  = 0;   
   DDR_CHIPSELECT populated_chipselect = DDR_CS_NONE;
   DDR_CHIPSELECT populated_chipselects_all_ch = DDR_CS_NONE;

   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   for (ch = 0; ch < num_ch; ch++) {
      if (((channel >> ch) & 0x1) == 0x1) {
         populated_chipselect = DDR_CS_NONE;
         reg_offset_mpe  = REG_OFFSET_MC_MPE(ch);
         reg_offset_osu  = REG_OFFSET_MC_OSU(ch);
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
         ch_1hot         = CH_1HOT(ch);
         for (cs = 0; cs < NUM_CS; cs++) {
            cs_1hot = CS_1HOT(cs);

            /* 1. Configure the MRR execution to MR8 for each rank */
            MC_Cmd_Set_Cfg(ch, cs_1hot, 0, CMD_MRR, JEDEC_MR_8, 0);
            MC_Cmd_Stagger_Cfg(ch, 0, 0, 0, 0);   
            /* 2. Set the trigger for HW to execute the commands configured in previous step */
            /* 3. Poll and wait for the read value to become zero */
            MC_Cmd_Trigger(ch, 0x1);
            /* 4. Read the below register values for MRR timeout status */
            rank_timeout = HWIO_INX (reg_offset_shke, SHKE_MR_RDATA_TIMEOUT_STATUS);
            
            /* 5. If timeout status for a rank is 1, that means the corresponding rank is not populated.
			      Or, if we shouldn't be initializing that rank, mark it as unpopulated. */
            if (((rank_timeout & (0x1 << (cs * 4))) != 0)
			    || ((cs_1hot & chip_select) == 0)) {
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_rows[cs]        = 0;
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_cols[cs]        = 0;
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].num_banks[cs]       = 0;
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs] = 0;
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_density[cs]  = 0;
            
               if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
                  (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)) {
                  /* a. Below steps are SW workaround for per-rank pending queue empty status not resetted by sw_ares for Napali V1 */                
                  /* i. Disable PRQ Read backpressure enable */
                  HWIO_OUTXF (reg_offset_mpe, MPE_RD_BKPR_CFG, PRQ_BKPR_EN, 0);
                  /* Set the below register for the above settings to take effect */
                  HWIO_OUTX (reg_offset_mpe, MPE_LOAD_CONFIG, 0x1); 
                  /* Poll for LOAD_CONFIG to become zero */
                  while (HWIO_INX (reg_offset_mpe, MPE_LOAD_CONFIG) != 0x0);
                  
                  /* ii. Perform SW MRR sequence below for 63 times to wrap PRQ counter to 0 */
                  for (i = 0; i < 63; i++) {
                     MC_Cmd_Set_Cfg(ch, cs_1hot, 0, CMD_MRR, JEDEC_MR_8, 0); 
                     MC_Cmd_Stagger_Cfg(ch, 0, 0x1, 0x0, 0x0);
                     MC_Cmd_Trigger(ch, 0x1);       
                  }
                        
                  /* iii. Enable PRQ Read backpressure enable */
                  HWIO_OUTXF (reg_offset_mpe, MPE_RD_BKPR_CFG, PRQ_BKPR_EN, 1);
                  /* Set the below register for the above settings to take effect. */
                  HWIO_OUTX (reg_offset_mpe, MPE_LOAD_CONFIG, 0x1); 
                  /* Poll for LOAD_CONFIG to become zero */
                  while (HWIO_INX (reg_offset_mpe, MPE_LOAD_CONFIG) != 0x0);

                  /* b. Set the SW self-refresh vote for unavailable rank */
                  HWIO_OUTX (reg_offset_shke, SHKE_SW_SELF_REFRESH_VOTE, cs_1hot);
               }
               /* Other than Napali V1 */
               else {
                  /* Disable CKE to the unpopulated rank */
                  MC_Cmd_Set_Cfg(ch, cs_1hot, 0, CMD_POWER_DOWN_PDE, 0, 0); 
                  MC_Cmd_Stagger_Cfg(ch, 0, 0x0, 0x0, 0x0);
                  MC_Cmd_Trigger(ch, 0x1);
               }
            }
            else {
               /* The rank is populated and the rank can be enabled as part of post-initialization */
               if (cs == 0) {
                  mr8_value = HWIO_INX (reg_offset_shke, SHKE_MR_RDATA_RANK0);
                  // Read manufacturer ID (MR5) from Rank 0 only
                  ddr->shrm_ddr_ptr->detected_ddr_device[ch].manufacturer_id = \
                                  (MC_MR_Read(ch_1hot, DDR_CS0, JEDEC_MR_5) & 0xFF);
               }
               else if (cs == 1) {
                  mr8_value = HWIO_INX (reg_offset_shke, SHKE_MR_RDATA_RANK1);
               }
               
               // Read revision ID-1 (MR6)
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].revision_id_1[cs] = \
                               (MC_MR_Read(ch_1hot, cs_1hot, JEDEC_MR_6) & 0xFF);
               // Read revision ID-2 (MR7)
               ddr->shrm_ddr_ptr->detected_ddr_device[ch].revision_id_2[cs] = \
                               (MC_MR_Read(ch_1hot, cs_1hot, JEDEC_MR_7) & 0xFF);
                              
               MC_DDR_Geometry_Detection(ddr, ch, cs, mr8_value);
               populated_chipselect |= cs_1hot;
            }

            /* 6. Toggle sw_ares to reset pending read command and DQS read FIFO pointers */
            HWIO_OUTXF (reg_offset_mpe, MPE_RD_BEAT_CFG, PRQ_SW_ARES, 1);
            HWIO_OUTXF (reg_offset_osu, OSU_DQS_FIFO_CFG, RESET, 1);
            /* Wait for 100ns */
            seq_wait(100, NS);
            HWIO_OUTXF (reg_offset_mpe, MPE_RD_BEAT_CFG, PRQ_SW_ARES, 0);
            HWIO_OUTXF (reg_offset_osu, OSU_DQS_FIFO_CFG, RESET, 0);
         } // end of for(cs)
         ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect = populated_chipselect;
         
         // Check if memory device was not detected
         if(populated_chipselect == DDR_CS_NONE) {
            ddr_printf(DDR_ERROR, "\nDDR ABORT: NO MEMORY DEVICE DETECTED, Channel = %d, Populated CS = %d\n", ch, populated_chipselect);
            ddr_abort();
         }
         
         // Check if only rank 1 was detected
         if(populated_chipselect == DDR_CS1) {
            ddr_printf(DDR_ERROR, "\nDDR ABORT: ONLY RANK 1 OF MEMORY DEVICE DETECTED, Channel = %d, Populated CS = %d\n", ch, populated_chipselect);
            ddr_abort();
         }
         
         // Keep track of all detected ranks
         populated_chipselects_all_ch |= populated_chipselect;
      } 
   } // end of for(ch)
       
   // Check if populated chip selects is not the same for all channels
   for (ch = 0; ch < num_ch; ch++) {
      if (((channel >> ch) & 0x1) == 0x1) {
         if(ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect != populated_chipselects_all_ch) {
            ddr_printf(DDR_ERROR, "\nDDR ABORT: RANKS DETECTED NOT THE SAME FOR ALL CHANNELS, Channel = %d, Populated CS = %d\n", ch, ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect);
            ddr_abort();
         }
      }
   }
}

void MC_DDR_Addr_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel, DDR_CHIPSELECT chip_select)
{
   uint8  ch                    = 0;
   uint8  num_ch                = 0;
   uint8  cs                    = 0;
   uint8  device_density        = 0;
   uint8  device_io_width       = 0;
   uint8  interleave_shift      = 0;
   uint32 num_rows              = 0;
   uint32 num_cols              = 0;
   uint32 num_banks             = 0;
   uint32 size_in_mb            = 0;
   uint32 addr_base_9_2         = 0;
   uint32 addr_mask             = 0;
   uint32 timing_in_ps          = 0;
   uint32 timing_in_ck          = 0;
   uint64 base_addr             = 0xFFFFFFFFFFFFFFFF;
   uint64 addr_offset           = 0;
   uint64 ch_cs0_size           = 0;
   uint32 reg_offset_dtc        = 0; 
   uint32 reg_offset_global_sec = 0;
   uint64 total_package_size    = 0;
  
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

   for (cs = 0; cs < NUM_CS; cs++) {
      if ((chip_select & CS_1HOT(cs)) != 0) { // Rank exists   
         // Assume the same density for Rank 0 in all channels
         num_rows        = ddr->shrm_ddr_ptr->detected_ddr_device[0].num_rows[cs];
         num_cols        = ddr->shrm_ddr_ptr->detected_ddr_device[0].num_cols[cs];
         num_banks       = ddr->shrm_ddr_ptr->detected_ddr_device[0].num_banks[cs];
         device_density  = ddr->shrm_ddr_ptr->detected_ddr_device[0].device_density[cs];
         device_io_width = ddr->shrm_ddr_ptr->detected_ddr_device[0].device_io_width[cs];
   
         if (device_io_width == DEVICE_WIDTH_IS_x16) {
            size_in_mb  = lpddr_size_table[device_density][0][0];
            addr_offset = (((uint64) 1) << lpddr_size_table[device_density][0][1]);
         } 
         else if (device_io_width == DEVICE_WIDTH_IS_x8) {
            size_in_mb  = lpddr_size_table[device_density][1][0];
            addr_offset = (((uint64) 1) << lpddr_size_table[device_density][1][1]);
         } 
    
         switch(size_in_mb) {
            case 8192 : addr_mask = 0x80;
               break;
            case 4096 : addr_mask = 0xC0;
               break;
            case 2048 : addr_mask = 0xE0;
               break;
            case 1536 : addr_mask = 0xE0;
               break;  
            case 1024 : addr_mask = 0xF0;
               break;
            case 768  : addr_mask = 0xF0;
               break;
            case 512  : addr_mask = 0xF8;
               break;
            case 384  : addr_mask = 0xF8;
               break;
            case 256  : addr_mask = 0xFC;
               break;
            default   : addr_mask = 0x00;
               break;
         }
   
         // Use the same base address for all channels in interleaved case
         // Non-interleaved case is not supported
         if (cs == 0) {
            base_addr = MC_BASE_ADDR;
            ch_cs0_size = addr_offset;
   
            //Updated ddr_size_info struct for Rank0 
            for (ch = 0; ch < num_ch; ch++) {
               if (((channel >> ch) & 0x1) == 0x1) {
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_mb[ch]       = size_in_mb; 
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_io_width[ch] = device_io_width; 
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_addr[ch]     = base_addr;
               }
            }
         }
         else if (cs == 1) {
            /* CH0 CS1 base address depends on interleave vs non-interleave
             * interleaved base will be [base of CH0 CS0] + [ramsize of CH0 CS0] * NUM_CH
             * assume the same density for all channles in the interleaved case.
             * non-interleaved case is not supported */
            // Use the same base address for all channels if interleave is enabled
            base_addr = MC_BASE_ADDR + ch_cs0_size * num_ch;
   
            // Total package size = (size of CS0 + size of CS1) * (number of channels)
            // Assumption: Rank configuration is equivalent across channels
            total_package_size = (ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_mb[0] /*cs0 in CH0*/ + size_in_mb /*cs1*/) * num_ch ;

            // For 6GB configuration:
            // Place CS1 starting at 0x1_0000_0000
            // Double the base_addr for CS1
            if((total_package_size > 0xC00/*3GB*/) && (total_package_size <= 0x1800/*6GB*/) && (device_io_width == 1)) {
                base_addr = 2 * base_addr;
            }

            //Updated ddr_size_info struct for Rank1
            for (ch = 0; ch < num_ch; ch++) {
               if (((channel >> ch) & 0x1) == 0x1) {
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_mb[ch]       = size_in_mb; 
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_io_width[ch] = device_io_width; 
                  ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_addr[ch]     = base_addr;
               }
            }
         }
   
         // Convert base addr to [9:0] for ADDR_BASE CSR. 
         /* Right shift base address by log2[NUM_CH] first since the remapped address will be dropped
          * [2 bits for 4 channels; 1 bit for 2 channels; 0 bit for 1 channel] by LLCC for channel interleaving 
          * before providing to MC, then right shift by 26 to program [33:26] of base address to GLOBAL_SEC_ADDR_BASE_CS_n[9:2] */
         switch(num_ch) {
            case 4 : interleave_shift = 2;
               break;
            case 2 : interleave_shift = 1;
               break;
            case 1 : interleave_shift = 0;
               break;
            default: interleave_shift = 0;
               break;
         }
         addr_base_9_2 = (base_addr >> interleave_shift) >> 26;
            
         /* Enable MC broadcast and OR each bit of broadcast read data from each channel (polling for 0) */
         //ch = DDRSS_Broadcast_MC_Config( channel, OR_RDATA);
         ch = BROADCAST_CH_NUM;

         do { 
            if ((ch == BROADCAST_CH_NUM) || (((channel >> ch) & 0x1) == 0x1)) {
               reg_offset_dtc        = REG_OFFSET_MC_DTC(ch);
               reg_offset_global_sec = REG_OFFSET_MC_GLOBAL_SEC(ch);
              
               // Address setup
               HWIO_OUTXI (reg_offset_global_sec, GLOBAL_SEC_ADDR_BASE_CS_n, cs,\
                           addr_base_9_2 << 2);        /* ADDR_BASE -> [9:2] */
                                 
               HWIO_OUTXI (reg_offset_global_sec, GLOBAL_SEC_ADDR_MASK_CS_n, cs,\
                           addr_mask << 2);            /* ADDR_MASK -> [9:2] */
               if (num_banks != 0) {
               HWIO_OUTXMI (reg_offset_global_sec, GLOBAL_SEC_ADDR_MAP_CS_n, cs,\
                              (HWIO_GLOBAL_SEC_ADDR_MAP_CS_n_RMSK & ~(HWIO_GLOBAL_SEC_ADDR_MAP_CS_n_ADDR_MODE_BMSK))/*Mask to not change ADDR_MODE from DCB*/,\
                              ((1 << 16) & 0x00010000) /* RANK_EN -> [16] */ |\
                              (((num_banks >> 3) << 8) & 0x00000300) |\
                                                       /* BANK_SIZE -> [9:8] */
                                                       /* 0x0:BANKS_4 */ 
                                                       /* 0x1:BANKS_8 */
                              (((num_rows - 14) << 4) & 0x00000030) |\
                                                       /* ROW_SIZE -> [5:4] */
                                                       /* 0x0:ROWS_14 */ 
                                                       /* 0x1:ROWS_15 */
                                                       /* 0x2:ROWS_16 */
                                                       /* 0x3:ROWS_17 */
                              ((num_cols - 8) & 0x00000003));
                                                       /* COL_SIZE -> [1:0] */
                                                       /* 0x0:COLS_8  */
                                                       /* 0x1:COLS_9  */
                                                       /* 0x2:COLS_10 */
                                                       /* 0x3:COLS_11 */
               }
      
               // Update tRFC based on chip density, per rank.
               HWIO_OUTXI (reg_offset_dtc, DTC_RANK_TIMING_CFG_TRFC_AB_n, cs,\
                           (1 << 20) | lpddr_timing_table[device_density][0]);
               
               HWIO_OUTXI (reg_offset_dtc, DTC_RANK_TIMING_CFG_TRFC_PB_n, cs,\
                           (1 << 20) | lpddr_timing_table[device_density][1]);

               // Update tWTR/tWR based on device_io_width x16 or x8 device
               if (cs == 0) {   
                  HWIO_OUTX (reg_offset_dtc, DTC_BANK_TIMING_CFG_TWR,\
                             (lpddr_byte_mode_timing_table[1][device_io_width] << 20) | lpddr_byte_mode_timing_table[0][device_io_width]);

                  HWIO_OUTX (reg_offset_dtc, DTC_COL_TIMING_CFG_TWTR,\
                             (lpddr_byte_mode_timing_table[3][device_io_width] << 20) | lpddr_byte_mode_timing_table[2][device_io_width]);
               }
               else if (cs == 1) {
                  // Set the maximum timing values to tWR and tWTR if device_io_width is in hybrid mode
                  timing_in_ps = HWIO_INXF (reg_offset_dtc, DTC_BANK_TIMING_CFG_TWR, IN_PS);
                  timing_in_ck = HWIO_INXF (reg_offset_dtc, DTC_BANK_TIMING_CFG_TWR, MIN_CK);
                  if (lpddr_byte_mode_timing_table[0][device_io_width] > timing_in_ps) {
                     HWIO_OUTXF (reg_offset_dtc, DTC_BANK_TIMING_CFG_TWR, IN_PS, lpddr_byte_mode_timing_table[0][device_io_width]);
                  }
                  if (lpddr_byte_mode_timing_table[1][device_io_width] > timing_in_ck) {
                     HWIO_OUTXF (reg_offset_dtc, DTC_BANK_TIMING_CFG_TWR, MIN_CK, lpddr_byte_mode_timing_table[1][device_io_width]);
                  }

                  timing_in_ps = HWIO_INXF (reg_offset_dtc, DTC_COL_TIMING_CFG_TWTR, IN_PS);
                  timing_in_ck = HWIO_INXF (reg_offset_dtc, DTC_COL_TIMING_CFG_TWTR, MIN_CK);
                  if (lpddr_byte_mode_timing_table[2][device_io_width] > timing_in_ps) {
                     HWIO_OUTXF (reg_offset_dtc, DTC_COL_TIMING_CFG_TWTR, IN_PS, lpddr_byte_mode_timing_table[2][device_io_width]);
                  }
                  if (lpddr_byte_mode_timing_table[3][device_io_width] > timing_in_ck) {
                     HWIO_OUTXF (reg_offset_dtc, DTC_COL_TIMING_CFG_TWTR, MIN_CK, lpddr_byte_mode_timing_table[3][device_io_width]);
                  }
               }

               // Kick off timing parameter calculation and wait until update is done
               MC_Timing_Recalc_Update(ch);
            }
            // Increase channel index to loop all channels selected in non-broadcast case
            ch++;
         } while (ch < num_ch);
         // Disable MC broadcast
         //DDRSS_Broadcast_MC_Config( DDR_CH_NONE/* disable broadcast */, NO_RDATA);
      } // end of if(rank exists)
   } // end of for(cs)
}

void MC_Post_Init_Setup (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8  ch                   = 0;
   uint8  cs                   = 0;
   uint8  num_ch               = 0;
   uint8  device_density       = 0;
   uint8  device_io_width      = 0;
   uint8  sr_abort_en          = 0;
   uint8  vendor               = 0;
   DDR_CHIPSELECT chip_select  = 0;
   uint32 reg_offset_shke      = 0;
   uint32 reg_offset_mpe       = 0;
   uint32 mr0_value            = 0;
   
#if DSF_LPDDR4Y_SUPPORTED
   uint32 mr7_value            = 0;
#endif // #if DSF_LPDDR4Y_SUPPORTED
   
   num_ch  = ddr->shrm_ddr_ptr->misc.num_channel;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
   // Assume the same rank configuration for all channels
   chip_select = ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
   
   // Address setup
   MC_DDR_Addr_Setup(ddr, channel, chip_select);
   
   // Check if vendor-specific LPDDR4Y low power features are supported by the DRAM
   vendor = ddr->shrm_ddr_ptr->detected_ddr_device[0].manufacturer_id;
   // Check mode register 0 to see if the device supports LPDDR4Y
   mr0_value = MC_MR_Read (channel, DDR_CS0, JEDEC_MR_0);
   
#if DSF_LPDDR4Y_SUPPORTED
   if (vendor == SAMSUNG)
   {
      if ((mr0_value & 0x20) == 0x20)
      {
         ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support &= LPDDR4Y_LOW_SPEED_CA_BUFFER | LPDDR4Y_SINGLE_ENDED_CLOCK;
      }
      else
      {
         // Some Samsung devices will support a subset of LPDDR4Y.  Check MR7 to see.
		 mr7_value = MC_MR_Read (channel, DDR_CS0, JEDEC_MR_7);
         if ((mr7_value & 0x01) == 0x01)
         {
             ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support &= LPDDR4Y_SINGLE_ENDED_CLOCK;
         }
         else
         {
            ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support = 0;
         }
      }
   }
   else if ((vendor == HYNIX) && ((mr0_value & 0x20) == 0x20))
   {
      ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support &= LPDDR4Y_LOW_SPEED_CA_BUFFER | LPDDR4Y_SINGLE_ENDED_CLOCK;
   }
   else if ((vendor == MICRON) && ((mr0_value & 0x20) == 0x20))
   {
      ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support &= LPDDR4Y_SINGLE_ENDED_CLOCK;
   }
   else
   {
      ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support = 0;
   }
#else
   ddr->shrm_ddr_ptr->ddr_runtime.lpddr4y_support = 0;
#endif // #if DSF_LPDDR4Y_SUPPORTED
	  

   // Setup global timer registers for periodic ZQCAL
   // Different settings for different channels, not handled by SWC
   for (ch = 0; ch < num_ch; ch++) {
      if (((channel >> ch) & 0x1) == 0x1) {
         reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
         switch (ch) {
            case 0:
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_MATCH, 0x0);
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_HP_MATCH, 0x20);
               break;
            case 1:
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_MATCH, 0x40);
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_HP_MATCH, 0x60);
               break;
            case 2:
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_MATCH, 0x80);
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_HP_MATCH, 0xA0);
               break;
            case 3:
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_MATCH, 0xC0);
               HWIO_OUTX (reg_offset_shke, SHKE_ARB_GLOBAL_TIMER_HP_MATCH, 0xE0);
               break;
         }   
      }
   }
   
   ch = BROADCAST_CH_NUM;
   reg_offset_shke = REG_OFFSET_MC_SHKE(ch);
   reg_offset_mpe  = REG_OFFSET_MC_MPE(ch);
   
   for (cs = 0; cs < NUM_CS; cs++) {
      if ((chip_select & CS_1HOT(cs)) != 0) { // Rank exists   
         // Assume the same density and io width for in all channels
         device_density  = ddr->shrm_ddr_ptr->detected_ddr_device[0].device_density[cs];
         device_io_width = ddr->shrm_ddr_ptr->detected_ddr_device[0].device_io_width[cs];               

         /* 1. Set the below register depending on the device configuration */
         if (device_io_width == DEVICE_WIDTH_IS_x16) {
            // SHKE_CONFIG[DEVICE_CFG] = 0x0->x8_x8, 0x1->x16, 0x2->x8_x8_x8_x8, 0x3->x16_x16, 0x4->x32 
            if (cs == 0) {
               HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, DEVICE_CFG, 0x1);
            }
            else if ((cs == 1) && (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
               (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)))) {
               // Other than Napali V1
               HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, DEVICE_CFG_RANK1, 0x1); 
            } 
         }
         else if (device_io_width == DEVICE_WIDTH_IS_x8) {
            // SHKE_CONFIG[DEVICE_CFG] = 0x0->x8_x8, 0x1->x16, 0x2->x8_x8_x8_x8, 0x3->x16_x16, 0x4->x32 
            if (cs == 0) {
               HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, DEVICE_CFG, 0x0);
            }
            else if ((cs == 1) && (!((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
               (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)))) {
               // Other than Napali V1
               HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, DEVICE_CFG_RANK1, 0x0); 
            }            
         }

         /* 2. IF the DRAM device density is greater than 12Gb per die, self-refresh abort can be enabled as below */
         if (device_density >= 0x3) {
            // Enable Self Refresh Abort in memory controller
            sr_abort_en = HWIO_INXF (reg_offset_shke, SHKE_HW_SELF_REFRESH_CTRL, SR_ABORT_EN);
            // SHKE_HW_SELF_REFRESH_CTRL[5:4] = SR_ABORT_EN
            // Enable bit 4 for Rank0 (cs = 0) and enable bit 5 for Rank1 (cs = 1)
            sr_abort_en |= 1 << (cs + 4);
            HWIO_OUTXF (reg_offset_shke, SHKE_HW_SELF_REFRESH_CTRL, SR_ABORT_EN, sr_abort_en);
            // Enable Self Refresh Abort in DRAM
            // MR4_OP[3] = SR_Abort = 1b, MR4_OP[2:0] = Refresh_Rate = 011b
            MC_MR_Write(ddr, channel, CS_1HOT(cs), JEDEC_MR_4, 0xB);
         }
      } // end of if(rank exists)
   } // end of for(cs)

   /* 3. Program device MR23 to set DQS OSC interval timer with auto-stop */
   /* Included in periodic training function */
 
   /* 4. Set the following register to specify that the rank to enable for ZQ task during frequency switch */
   HWIO_OUTXF2 (reg_offset_shke, SHKE_FSW_ZQCAL_CFG, ZQSTART_RANK_EN, ZQLATCH_RANK_EN, chip_select, chip_select); 

   /* 5. Set the following register to specify that the rank is ready for regular traffic, activity based power management. */
   if ((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && \
      (ddr->shrm_ddr_ptr->misc.chip_version == 0x0100)) {
      HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, RANK_EN, DDR_CS_BOTH);
   }
   else {
      // Other than Napali V1
      HWIO_OUTX (reg_offset_mpe, MPE_RANK_STATE_CFG, DDR_CS_BOTH); 
      HWIO_OUTXF (reg_offset_shke, SHKE_CONFIG, RANK_EN, chip_select); 
   } 

   /* 6. The following is used in ISU to mark incoming traffic as Error as DRAM is not ready to accept any traffic. */
   HWIO_OUTX (reg_offset_shke, SHKE_INIT_CONFIG, chip_select);
   // Set the below register for the above settings to take effect.
   HWIO_OUTX (reg_offset_shke, SHKE_LOAD_CONFIG, 0x1); 
   // Poll for LOAD_CONFIG to become zero
   while (HWIO_INX (reg_offset_shke, SHKE_LOAD_CONFIG) != 0x0){
	}

   /* Enable periodic SHKE events */
   MC_All_Periodic_Ctrl (ddr, channel, chip_select, 1/* 1 for enable, 0 for disable */);
   /* Enable power saving features */ 
   MC_Power_Saving_Ctrl (ddr, channel, chip_select, 1/* 1 for enable, 0 for disable */);
   /* Configure the registers for watchdog sequence */
   MC_Watchdog_Reset_Setup (ddr, channel, chip_select);
   
   /* Set boot init completion flag to enable power state config workaround in run-time frequency switch */
   ddr->shrm_ddr_ptr->misc.boot_init_completed = 1;
}

