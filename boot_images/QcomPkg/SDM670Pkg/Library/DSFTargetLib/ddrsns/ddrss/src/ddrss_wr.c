/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_wr.c#7 $
$DateTime: 2019/08/01 06:25:20 $
$Author: pwbldsvc $
================================================================================================================================================================
when       who          what, where, why
-------   ---          --------------------------------------------------------

================================================================================*/

#include "ddrss.h"
#include "mc.h"
#include <string.h>
   
// -------------------------------------------------------------------------
// DDR PHY WR DQ-DQS Training.
// -------------------------------------------------------------------------
void Training_DDRSS_WR_DQDQS_LPDDR4 (BOOT_DDR_STRUCT *ddr, 
                                     DDR_CHANNEL channel, 
                                     DDR_CHIPSELECT chip_select, 
                                     uint32 clk_freq_khz, 
                                     uint8 prfs_index, 
                                     training_params_t *training_params_ptr,
                                     uint8             *local_vars)
{
    uint8 ch;
    uint8 cs;
    uint8 fsp_op;
    uint8 fsp_wr;
    uint8 high_current_mode;
    uint8 MR13_value;
    PHY_BUS  dq     = DQ;
    uint8 num_ch    = ddr->shrm_ddr_ptr->misc.num_channel;
    extern uint8 WR_Training_Switchboard[8][5];
    extern uint8 Triainig_pivot_plots_Switchboard[8][2];
    uint8 Str_array[8][NUM_DQ_PCH] ;

    fsp_op = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
    fsp_wr = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
  
    //Enable MC_Auto_refresh
    MC_Periodic_Auto_Refresh_Ctrl(ddr, channel, chip_select, 1);
  
  for (ch = 0; ch < num_ch; ch++)
  {
      if((channel >> ch) & 0x1)
      {
          
          for(cs = 0; cs < NUM_CS; cs++)
          {
              
               //=================================================================
               //=================================================================
               //=================================================================
               if( ( (                                     cs==1  ) &&
                     (                  chip_select >> cs) & 0x1  ) && 
                    (DDRSS_get_wr_rank_update_en(prfs_index,ch )==0  ) )
               {                               
                   ddr_printf(DDR_NORMAL,"copy WRITE CDC from rank 0 to rank 1 in prfs %d \n", prfs_index );
                   DDRSS_cpy_cdcext_slave_wr_r0tor1(prfs_index,ch );
                 if((WR_Training_Switchboard[prfs_index][_V_FSP0]==1) || 
                      (WR_Training_Switchboard[prfs_index][_V_FSP1]==1))
                 {
                      //=================================================================
                    //copy dram vref from rank 0 to rank 1
					if (WR_Training_Switchboard[prfs_index][_V_FSP0]==1){
						DDRSS_cpy_dram_wr_vref_r0tor1(ddr, ch, 0);
					}
					else if (WR_Training_Switchboard[prfs_index][_V_FSP1]==1){
						DDRSS_cpy_dram_wr_vref_r0tor1(ddr, ch, 1);
					}
                      //=================================================================
                 }
               }
               else 
               //=================================================================
               //=================================================================
              //=================================================================
                      
              if((chip_select >> cs) & 0x1)
              {
                ddr_printf(DDR_NORMAL,"\n\n\n>================    WRITE TRAINING START   ================<\n" );
                ddr_printf(DDR_NORMAL,"Frequency, %d, Channel, %d, Rank, %d,\n\n",clk_freq_khz, ch , cs);

                  //ddr_printf(DDR_NORMAL, "START: Write training on Ch: %u Rank: %u\n", ch, cs);
                  // Enter WR_Training. MR13, VRCG (bit 3) set to high-current mode for WR training.
                  high_current_mode = 1; 
                  MR13_value = (fsp_op << 7) | (fsp_wr << 6) | (0 << 5) | (1 << 4) | (high_current_mode << 3) | (0 << 2) | (0 << 0);
                  
                  // TODO : BIMC wait timer
                  // Satisfy tVRCG_ENABLE=200ns. ROUNDUP(200ns/52ns)=0x4
                  //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, 0x04);
                  seq_wait(200,NS);
                  
                  MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (MR13_value | MR13_BASE)); //disable Data Mask, Enable RRO
                  
                  // Reset manual_1 timer
                  //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, 0x00);
                  seq_wait(200,NS);
                    
                  //tArgs.currentChannel = ch;
                  //tArgs.currentChipSelect = cs;
                  //tArgs.trainingType = DDR_TRAINING_WRITE;
                  //tArgs.currentFreqIndex = training_ddr_freq_indx+TRAINING_START_PRFS;
                  //tArgs.FreqInKhz = clk_freq_khz;
          DDRSS_store_all_wr_rank_update_en( ch,  Str_array );
          DDRSS_set2one_all_wr_rank_update_en( ch );
                  
                  DDRSS_wr_dqdqs_lpddr4 (ddr, 
                                         ch, 
                                         cs, 
                                         dq,
                                         training_params_ptr, 
                                         (ddrss_rdwr_dqdqs_local_vars *)local_vars, 
                                         clk_freq_khz, 
                                         prfs_index);                                    
          DDRSS_restore_all_wr_rank_update_en( ch, Str_array );
                  
                  // Exit WR Training. MR13. (VRCG, bit 3 = 0, normal current mode).
                  high_current_mode = 0;
                  MR13_value = (fsp_op << 7) | (fsp_wr << 6) | (0 << 5) | (1 << 4)  | (high_current_mode << 3) | (0 << 2) | (0 << 0);
                  
                  // TODO : BIMC wait timer
                  // Satisfy tVRCG_DISABLE=100ns. ROUNDUP(100ns/52ns)=0x2
                  //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, div_ceil( ( ddr->cdt_params.lpddr.tVRCG_DISABLE * 100), XO_PERIOD_IN_PS));
                  seq_wait(200,NS);
                  MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (MR13_value | MR13_BASE));//MR13_BASE to disable DM and enable RRO
                  
                  // Reset manual_1 timer
                  //BIMC_Wait_Timer_Setup ((DDR_CHANNEL)CH_1HOT(ch), WAIT_XO_CLOCK, 0x00);
                  
                  //ddr_printf(DDR_NORMAL,"\nFrequency, %d, Channel, %d, Rank, %d,\n",clk_freq_khz, ch , cs);
                  ddr_printf(DDR_NORMAL,">================    WRITE TRAINING END     ================<\n" );
                  //ddr_printf(DDR_NORMAL, "END: Write training on Ch: %u Rank: %u\n\n", ch, cs);
                  //ddr_send_data_to_ddi(local_vars, LOCAL_VARS_AREA_SIZE, &tArgs, (void*)training_params_ptr);
                  
                  // Store local vars in flash and zero-initialize for use by the next training function.
                  //ddr_page_to_storage(local_vars, LOCAL_VARS_AREA_SIZE);
              }//if cs
          }//cs
      }//if ch
  }//ch

    #if DSF_WR_DQDQS_COARSE_FINE_PLOT_PIVOT

        if (Triainig_pivot_plots_Switchboard[prfs_index][_WR_P]==1)
        {
            uint64 tmp = 0;
      		DDRSS_wr_dqdqs_coarse_fine_plot_pivot (ddr, 
                                training_params_ptr, 
                                clk_freq_khz,
                                &tmp);       
        }
      
#endif    
    
    //Disable MC_Auto_refresh
    MC_Periodic_Auto_Refresh_Ctrl(ddr, channel, chip_select, 0);
}

// -------------------------------------------------------------------------
// DDR PHY WR DQ-DQS Training
// -------------------------------------------------------------------------
//extern uint32 write_in_pattern[] __attribute__((aligned(8)));

// ============================================================================================================================
// ======================================      W R I T E   DQDQS  T R A I N I N G                =================================
// ============================================================================================================================
void DDRSS_wr_dqdqs_lpddr4 (BOOT_DDR_STRUCT *ddr,
                            uint8 ch,
                            uint8 cs,
                            PHY_BUS dq,
                            training_params_t *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars *local_vars,
                            uint32 clk_freq_khz,
                            uint8 prfs_index)
{
    extern uint8 WR_Training_Switchboard[8][5];
  //uint8 freq_eq_max_training = (ddr->shrm_ddr_ptr->extended_cdt_runtime.max_ddr_frequency == clk_freq_khz);
    uint8 freq_eq_max_training = (WR_Training_Switchboard[prfs_index][_C_PB]==1);

    uint32    dq0_ddr_phy_base  = 0;
    uint8     byte              = 0;
    boolean pbcf_pass;   
    
    dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
	
    if(
	    (WR_Training_Switchboard[prfs_index][_C_PB  ]==1)||
	    (WR_Training_Switchboard[prfs_index][_C_CF  ]==1)||
	    (WR_Training_Switchboard[prfs_index][_DCC   ]==1)||
	    (WR_Training_Switchboard[prfs_index][_V_FSP1]==1)||
	    (WR_Training_Switchboard[prfs_index][_V_FSP0]==1)
		)
    {
       
        for (byte=0;byte<NUM_DQ_PCH;byte++)
        {      
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
          
         // HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,0x00041);
         // HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,0x00041);
         // HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,0x000261);
         // HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,0x000271);
         // HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,0x000381);
        }
        
        // Switch to the training frequency
        ddr_external_set_clk_speed (clk_freq_khz);
        
        for (byte=0;byte<NUM_DQ_PCH;byte++)
        { 
          // Clear Half Cycle settings
          DDR_PHY_hal_cfg_wrlvl_half(dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),
                                     0,//cs
                                     0x0);
                                      
          DDR_PHY_hal_cfg_wrext_ctl_update(dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), 
                                           prfs_index, 
                                           0, //cs
                                           0x0);
          DDR_PHY_hal_cfg_wrlvl_half(dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),
                                     1,//cs
                                     0x0);
                                      
          DDR_PHY_hal_cfg_wrext_ctl_update(dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), 
                                           prfs_index, 
                                           1, //cs
                                           0x0);      
	    								   
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_TOP_CTRL_1_CFG,PERIODIC_TRAINING_CLK_ON,0x1);
         // HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_IOCTLR_CTRL_CFG,IO_CAL_AUTO,   0x0);
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R0_MSK, 0x1);             
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R1_MSK, 0x1);             
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,MSTR_PHY,      0x0); 
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,PERIODIC_TRAINING_ACK_SEL,0x1); 
          HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,STATUS_CGC_ENABLE, 0x1); 
        }
        
        // ---------------------------------------------------------------------------------
        // Half and full cycle search
        // ---------------------------------------------------------------------------------   
        //ddr_printf(DDR_NORMAL,"DDRSS_WR_half_full_cycle_search\n");
        
        //DDRSS_WR_half_full_cycle_search (ddr,
        //                                 ch,
        //                                 cs,
        //                                 dq,
        //                                 training_params_ptr,
        //                                 local_vars,
        //                                 clk_freq_khz,
        //                                 prfs_index);
        //
	}
	
	
    if(WR_Training_Switchboard[prfs_index][_V_FSP1]==1)
    {
      ddr_printf(DDR_NORMAL,"DDRSS_wr_dqdqs_2D_vcf 1st run\n");
        DDRSS_wr_dqdqs_2D_vcf(ddr,
                        ch,
                        cs,
                        dq,
                        training_params_ptr,
                        (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                        clk_freq_khz,
                        prfs_index,
                        freq_eq_max_training,
			0/*sed_enable*/);
    }
	
    if(WR_Training_Switchboard[prfs_index][_C_PB]==1)//highest frequency
    {
            #if DSF_WR_DQDQS_TRAINING_LOG_BEFORE
            DDRSS_wr_dqdqs_log(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index);
            #endif
            ddr_printf(DDR_NORMAL,"DDRSS_wr_dqdqs_1D_pbcf\n");
			
            pbcf_pass = DDRSS_wr_dqdqs_1D_pbcf(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index,
                            freq_eq_max_training,
							8);
            if (!pbcf_pass)
			{
                 pbcf_pass = DDRSS_wr_dqdqs_1D_pbcf(ddr,
                                                  ch,
                                                  cs,
                                                  dq,
                                                  training_params_ptr,
                                                  (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                                                  clk_freq_khz,
                                                  prfs_index,
                                                  freq_eq_max_training,
							                      4);
			       if (!pbcf_pass)
			       {
                     ddr_abort();
			       }
			
			 }				 
            #if DSF_WR_DQDQS_TRAINING_LOG_AFTER
			
            ddr_external_set_clk_speed (clk_freq_khz); 
            ddr_printf(DDR_NORMAL,"DDRSS_wr_dqdqs_1D_pbcf after log after freq switch\n");
            DDRSS_wr_dqdqs_log(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index);
            #endif


    }

    if(WR_Training_Switchboard[prfs_index][_C_CF]==1)//high frequency (other than highest)
    {

            #if DSF_WR_DQDQS_TRAINING_LOG_BEFORE
            ddr_printf(DDR_NORMAL,"BEFORE_DDRSS_wr_dqdqs_1D_cf\n");
            DDRSS_wr_dqdqs_log(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index);
            #endif
            ddr_printf(DDR_NORMAL,"DDRSS_wr_dqdqs_1D_cf\n");
		    DDRSS_wr_dqdqs_1D_cf(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index,
                            freq_eq_max_training);

            #if DSF_WR_DQDQS_TRAINING_LOG_AFTER
            ddr_printf(DDR_NORMAL,"AFTER_DDRSS_wr_dqdqs_1D_cf\n");
            DDRSS_wr_dqdqs_log(ddr,
                            ch,
                            cs,
                            dq,
                            training_params_ptr,
                            (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                            clk_freq_khz,
                            prfs_index);
            #endif
    }

    if((WR_Training_Switchboard[prfs_index][_V_FSP0]==1) || 
       (WR_Training_Switchboard[prfs_index][_V_FSP1]==1))
    {
      ddr_printf(DDR_NORMAL,"DDRSS_wr_dqdqs_2D_vcf_max_rec\n");
        DDRSS_wr_dqdqs_2D_vcf(ddr,
                        ch,
                        cs,
                        dq,
                        training_params_ptr,
                        (ddrss_rdwr_dqdqs_local_vars *)local_vars,
                        clk_freq_khz,
                        prfs_index,
                        freq_eq_max_training,
                        1//sed_enable
						);

    }
    else
    {
        //scaling
        ddr_printf(DDR_NORMAL,"SCALING\n");
    }

    // Disable WR training settings
    for (byte=0;byte<NUM_DQ_PCH;byte++)
    {
      
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
      
      //HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,0x000041);
      //HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,0x000041);
      //HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,0x000041);
      //HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,0x000141);
      //HWIO_OUTX (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,0x000141);
    }
    
    // Switch to the training frequency
    ddr_external_set_clk_speed (clk_freq_khz);
    
   
    for (byte=0;byte<NUM_DQ_PCH;byte++)
    { 
    //  HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_IOCTLR_CTRL_CFG,IO_CAL_AUTO,              0x1);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_TOP_CTRL_1_CFG,PERIODIC_TRAINING_CLK_ON,  0x0);
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R0_MSK,            0x0);             
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,UPDATE_R1_MSK,            0x0);             
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,MSTR_PHY,                 0x0); 
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,PERIODIC_TRAINING_ACK_SEL,0x0); 
      HWIO_OUTXF (dq0_ddr_phy_base + (byte * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_UPDATE_INTF_CFG,STATUS_CGC_ENABLE,        0x0);       
    }  
}

void DDRSS_WR_half_full_cycle_search  (BOOT_DDR_STRUCT *ddr,
                                       uint8 ch,
                                       uint8 cs,
                                       PHY_BUS dq,
                                       training_params_t *training_params_ptr,
                                       ddrss_rdwr_dqdqs_local_vars *local_vars,
                                       uint32 clk_freq_khz,
                                       uint8 prfs_index)
{
    FAILCELL  fail_cell;

    uint32    dq0_ddr_phy_base  = 0;
    uint8            byte_lane  = 0;
    uint8           loop_count  = 0;
    uint8           MR14_value  = 0;
    uint8     coarse_cdc_value  = 0;
    uint8     min_coarse_value  = 0;
    uint8     max_coarse_value  = 0;

    uint8      half_cycle_steps = 0;
    uint8      full_cycle_steps = 0;

    uint8      half_cycle_count = 0;
    uint8  max_half_cycle_count = 4;

    uint16              full_cycle_in_ps = 0;
    uint16              half_cycle_in_ps = 0;

    uint8         start_full_cycle_count = 0;
    uint8         start_half_cycle_count = 0;

    uint8      dq_half_cycle[NUM_DQ_PCH] = {0};
    uint8      dq_full_cycle[NUM_DQ_PCH] = {0};

  //uint8                *compare_result;
    uint8 compare_result_acc[NUM_DQ_PCH] = {0};
    uint8                 byte_lane_done = 0;
    uint8    byte_lane_found[NUM_DQ_PCH] = {0};

    // Initialize local vars
    memset(local_vars->coarse_schmoo.coarse_dq_full_half_cdc,
    training_params_ptr->wr_dqdqs.max_loopcnt + 1,
    (NUM_DQ_PCH * NUM_FULL * NUM_HALF * COARSE_CDC));

    // Set DQ0 base for addressing
    dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

    // Set the Vref value to use if maximum terminated or unterminated frequency
    if((prfs_index == MAX_TRAINING_FREQ_INDEX) || (clk_freq_khz <= LOWEST_TRAINED_FPRS_FREQ))
    {
        MR14_value = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? WRITE_TERMINATED_MR14_VREF: WRITE_UNTERMINATED_MR14_VREF;
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, MR14_value);
    }

    // Calculate coarse sweep range to stay between T and T/2
    full_cycle_in_ps = CONVERT_CYC_TO_PS / clk_freq_khz;
    half_cycle_in_ps = full_cycle_in_ps / 2;
    full_cycle_steps = (full_cycle_in_ps / COARSE_STEP_IN_PS) + 1;
    half_cycle_steps = (half_cycle_in_ps / COARSE_STEP_IN_PS) + 1;
    max_coarse_value = (full_cycle_steps > COARSE_CDC_MAX_VALUE) ? COARSE_CDC_MAX_VALUE : full_cycle_steps;
    min_coarse_value = (half_cycle_steps > COARSE_CDC_MAX_VALUE) ? COARSE_CDC_MAX_VALUE : half_cycle_steps;

    // Calculate initial half and full cycle delays
    //max_half_cycle_count = (((CONVERT_CYC_TO_PS / clk_freq_khz) + MAX_TDQS2DQ_IN_PS) / ((CONVERT_CYC_TO_PS / clk_freq_khz) / 2));
    max_half_cycle_count = (MAX_TDQS2DQ_IN_PS / half_cycle_in_ps) + 1;

    start_full_cycle_count = max_half_cycle_count / 2;
    start_half_cycle_count = max_half_cycle_count % 2;

    if(start_full_cycle_count > (NUM_FULL - 1))
    {
       start_full_cycle_count = (NUM_FULL - 1);
    }

    for(byte_lane = 0; byte_lane < dq; byte_lane++)
    {
        dq_full_cycle[byte_lane] = start_full_cycle_count;
        dq_half_cycle[byte_lane] = start_half_cycle_count;
    }

    // Sweep half and full cycle delay
    for(half_cycle_count = (max_half_cycle_count + 1); half_cycle_count > 0;)
    {
        half_cycle_count--;

        // Set full and half cycle delays
        for(byte_lane = 0; byte_lane < dq; byte_lane++)
        {
            DDR_PHY_hal_cfg_wrlvl_half((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                        cs,
                                        dq_half_cycle[byte_lane]);

            local_vars->dq_half_cycle[byte_lane] = dq_half_cycle[byte_lane];

            DDR_PHY_hal_cfg_wrlvl_full((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                        cs,
                                        dq_full_cycle[byte_lane]);

            local_vars->dq_full_cycle[byte_lane] = dq_full_cycle[byte_lane];
        }

        // Sweep coarse delay from T to T/2 looking for fail to pass transition
        for(coarse_cdc_value = max_coarse_value; coarse_cdc_value >= min_coarse_value; coarse_cdc_value--)
        {
            for(byte_lane = 0; byte_lane < dq; byte_lane++)
            {
                // Set coarse delay
                DDR_PHY_hal_cfg_cdc_slave_wr((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                             coarse_cdc_value, 1, 1, cs); // 1 for coarse delay, 1 for HPCDC
            }
            fail_cell.w = 0;
            // Write and verify data pattern
            for(loop_count = 0; loop_count < training_params_ptr->wr_dqdqs.max_loopcnt; loop_count++)
            {
#if DSF_PXI_TRAINING_EN
                Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                DDRSS_mem_write_PXI( ch, cs); // 0 for dbi, 1 for prbs
#if DSF_PXI_TRAINING_EN
                Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);

                // Accumulate the compare results per byte
                for(byte_lane = 0; byte_lane < dq; byte_lane++)
                {
                  compare_result_acc[byte_lane] |= (fail_cell.b[0][byte_lane] | fail_cell.b[1][byte_lane]);
                }
            }

            // Check for right edge
            for(byte_lane = 0; byte_lane < dq; byte_lane++)
            {
                local_vars->coarse_schmoo.coarse_dq_full_half_cdc[byte_lane][dq_full_cycle[byte_lane]][dq_half_cycle[byte_lane]][coarse_cdc_value]
                    = compare_result_acc[byte_lane];

                if(compare_result_acc[byte_lane] == 0)
                {
                    if(byte_lane_found[byte_lane] == 0)
                    {
                        byte_lane_done++;
                        byte_lane_found[byte_lane]++;
                    }
                }

                compare_result_acc[byte_lane] = 0;
            }

            // Exit when all bytes are done
            if(byte_lane_done == dq)
            {
                break;
            }
        }  // coarse sweep

        // Exit when all bytes are done
        if(byte_lane_done == dq)
        {
            break;
        }

        // Increase half and full cycle for bytes which are not done
        for(byte_lane = 0; byte_lane < dq; byte_lane++)
        {
            if(byte_lane_found[byte_lane] == 0)
            {
                if(dq_half_cycle[byte_lane] == 0)
                {
                    dq_half_cycle[byte_lane] = 1;
                    dq_full_cycle[byte_lane]--;
                }
                else
                {
                    dq_half_cycle[byte_lane]--;
                }
            }
        }
    } // half cycle sweep

    // Update the WRLVL EXT register
    for(byte_lane = 0; byte_lane < dq; byte_lane++)
    {
      local_vars->dq_half_cycle[byte_lane] = 1;
      //local_vars->dq_full_cycle[byte_lane] = 1;
      
      DDR_PHY_hal_cfg_wrext_ctl_update (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),
                                        prfs_index,
                                        cs,
                                        local_vars->dq_half_cycle[byte_lane]
                                        );
                                        
      DDR_PHY_hal_cfg_wrlvl_half((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                   cs,
                                   dq_half_cycle[byte_lane]);
                                   
      DDR_PHY_hal_cfg_wrlvl_full((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                  cs,
                                  dq_full_cycle[byte_lane]);
                                  
#if DSF_WR_DQDQS_TRAINING_LOG_IN
      ddr_printf(DDR_NORMAL,"WR cs = %d byte = %d DQ_HALF = %d DQ_FULL = %d\n",
                             cs,
                             byte_lane,
                             local_vars->dq_half_cycle[byte_lane],
                             local_vars->dq_full_cycle[byte_lane]);
#endif
    }//byte_lane
}//DDRSS_WR_half_full_cycle_search



boolean DDRSS_wr_dqdqs_1D_pbcf(BOOT_DDR_STRUCT             *ddr,
                            uint8                        ch,
                            uint8                        cs,
                            PHY_BUS                      dq,
                            training_params_t            *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars  *local_vars,
                            uint32                       clk_freq_khz,
                            uint8                        prfs_indx,
                            uint8 training_ddr_freq_indx_eq_training_table_index,
                            uint8       dd_offset
                            )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
  //int                             cdc_edge = 0 ;
    int                           dq_dcc_adj = 0 ;
    int                           dqsdcc_adj = 0 ;
    uint32     perbit[NUM_DQ_PCH][PINS_PER_PHY] = {{0}} ;
  //uint32                   cdc_midpoint_ps = 0 ;
  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
  extern     uint8 dq_dbi_bit   ;
  extern uint8 WR_Training_Switchboard[8][5];
    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32        pr_max_midp[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_eyew[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_min_strt[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_midp[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32         pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
   uint32         pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        midp[NUM_DQ_PCH]  = {0};
    uint32        fail_pat = 0;
    uint8       pbit_offset ;
    uint8       dbi__offset ;
    uint8       first__edge ;
    uint8       second_edge ;
    uint8       dbi_npbit_offset;
    //uint8       dbi_x;
	uint16 		dcccsr=0;
   uint32   pr_bus_eyew[NUM_DQ_PCH]  = {0,0};
   uint32   pr_bus_skwr[NUM_DQ_PCH]  = {0,0};

#if DSF_TRAINING_LOG_EN
   uint32   pr_x                     =  0   ;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };

#endif
    //uint32    read_back_pattern[DQ_TRAINING_PATTERN_SIZE] = {0};
    uint32  allbit         = ALL_DQ_PASS_PAT ;
    
// ============================================================================================================================
// ======================================      I N I T I A L I Z A T I O N                    =================================
// ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_dq[0] = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1] = REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
    DDRSS_cdcref_init_wr(local_vars->cdc_ref);    //cdc ruler init
    // ============================================================================================================================
    // ======================================     DQ WR     C D C           S W E E P I N G       =================================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size(clk_freq_khz);

// ---------------------------------------------------------------------------------
// first edge for perbit second edge for dbi
// ---------------------------------------------------------------------------------
	ddr_printf(DDR_NORMAL, "\n DBI offset:  %d",dd_offset);
	
	for (dbi_npbit_offset = 0; dbi_npbit_offset < 2; dbi_npbit_offset++)
    {
	
       if(dbi_npbit_offset)
        {
            pbit_offset = 0;
            dbi__offset = dd_offset;
           //first__edge = bit;
            //second_edge = dq_dbi_bit;
        }
        else
        {
           pbit_offset = dd_offset ;
            dbi__offset = 0 ;
            //first__edge = dq_dbi_bit;
            //second_edge = bit;
        }
// ---------------------------------------------------------------------------------
// --------------------------offseting to train dbi pbit ---------------------------
// ---------------------------------------------------------------------------------
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                remapped_bit  = bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
                DDR_PHY_hal_cfg_pbit_dq_delay   (reg_offset_ddr_dq[phy_x],
                                                remapped_bit,
                                                1, // 1 for TX
                                                cs, //cs
                                                pbit_offset);
            }//bit

            //dbi
            DDR_PHY_hal_cfg_pbit_dq_delay   (reg_offset_ddr_dq[phy_x],
                                                dq_dbi_bit,
                                                1, // 1 for TX
                                                cs, //cs
                                                dbi__offset);
												
        }//phy_x
// ---------------------------------------------------------------------------------
//clearing the histogram initializing to Zero
// ---------------------------------------------------------------------------------

    memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);

// ---------------------------------------------------------------------------------
// --------------------------one knob sweeping CDC      ---------------------------
// ---------------------------------------------------------------------------------
        for (knob = 0;  knob < sweep_size;  knob++)
        {
            for (phy_x = 0; phy_x < dq ; phy_x++)
            {
                DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                                local_vars->cdc_ref[knob][COARSE_L],
                                                0x1, // 0 for fine_delay 1 for coarse
                                                HP_MODE,
                                                cs);

                DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                                local_vars->cdc_ref[knob][FINE_L],
                                                0x0, // 0 for fine_delay 1 for coarse
                                                HP_MODE,
                                                cs);
            }//phy_x
			
			
            for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
            {
#if DSF_PXI_TRAINING_EN 
                Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                DDRSS_mem_write_PXI (ch,cs);
#if DSF_PXI_TRAINING_EN 
                Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                fail_pat    =   DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
                allbit      =   DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, knob);
                if(allbit == ALL_DQ_FAIL_PAT) break;
            }//loop count
        }  //End of knob loop
    // ============================================================================================================================
    // ======================================      processing histogram for edge detection (pbit / dbi)============================
    // ============================================================================================================================
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                for (rf_x = 0; rf_x < 3; rf_x++)
                {
                    if(dbi_npbit_offset)
                    {
                        first__edge = bit ;
                        second_edge = BYTE_SIZE;//dq_dbi_bit ;
                    }
                    else
                    {
                        first__edge = BYTE_SIZE;//dq_dbi_bit;
                        second_edge = bit ;
                    }
                    DDRSS_passing_region_scan(&(local_vars->pass_reg[rf_x][phy_x][first__edge][STRT]),
                                              &(local_vars->pass_reg[rf_x][phy_x][second_edge][STOP]),
                                              local_vars->failcount_histogram [rf_x][phy_x][bit],
                                              0,sweep_size);

                }//rf_x
            }//bit
        }//phy_x
#if DSF_WR_DQDQS_TRAINING_LOG_HIST_IN
    ddr_printf(DDR_NORMAL, "\n\n\n");
    ddr_printf(DDR_NORMAL,"\nFrequency, %d, Channel, %d, Rank, %d,\n",clk_freq_khz, ch , cs);
    for (knob=0; knob < sweep_size-1; knob++)
    {
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
            for (rf_x = 2; rf_x >= 0; rf_x--)
            {
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                        ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][knob]);
                }
                ddr_printf ( DDR_NORMAL, "       ,");
            }
            ddr_printf(DDR_NORMAL,"%3d,%2d,%2d,%4d,%3X,",knob,
                                     local_vars->cdc_ref[knob][COARSE_L],
                                     local_vars->cdc_ref[knob][FINE_L  ],
                                     local_vars->cdc_ref[knob][DELAY_L ],
                                     local_vars->cdc_ref[knob][CDC_L   ]);
        }//phy_x
        ddr_printf(DDR_NORMAL, "\n");
    }//knob
 #endif
 }//dbi_npbit_offset

    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        for (bit = 0; bit < BYTE_SIZE+1/*8pbit+dbi*/; bit++)
        {
            for (rf_x = 0; rf_x < 3; rf_x++)
            {
        // ---------------------------------------------------------------------------------
        //decompose the offset out of passing range
        local_vars->pass_reg[rf_x][phy_x][bit][STOP]+= dd_offset;
        // ---------------------------------------------------------------------------------
                local_vars->pass_reg[rf_x][phy_x][bit][MIDP]  = ((local_vars->pass_reg[rf_x][phy_x][bit][STOP]  + local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) >>1 ); // midpoint equal to average
                local_vars->pass_reg[rf_x][phy_x][bit][EYEW]  =  (local_vars->pass_reg[rf_x][phy_x][bit][STOP]  - local_vars->pass_reg[rf_x][phy_x][bit][STRT] )  ; // eywidth  equal to stop - start
                if ( pr_max_midp[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_max_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_max_eyew[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_max_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_min_stop[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_min_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
                if ( pr_max_strt[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_max_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_min_midp[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_min_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_min_strt[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_min_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_min_eyew[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_min_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_max_stop[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_max_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
                if ( local_vars->pass_reg[rf_x][phy_x][bit][STRT] > local_vars->pass_reg[rf_x][phy_x][bit][STOP]) {
                     ddr_printf(DDR_NORMAL, "\n Try next DBI offset");
                     return(FALSE);
                }
            }//rf_x
        }//bit
        // ============================================================================================================================
        // ==========================================             S P E C   C h e c k            ======================================
        // ============================================================================================================================
        pr_bus_eyew[phy_x]  =local_vars->cdc_ref[pr_min_stop[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[2][phy_x]][DELAY_L ];
        pr_bus_skwr[phy_x]  =local_vars->cdc_ref[pr_max_midp[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[2][phy_x]][DELAY_L ];
        //ddr_printf (DDR_NORMAL, "\n pr_max_eyew[2][dq%d]=, %4d,"  ,phy_x,local_vars->cdc_ref[pr_max_eyew[2][phy_x]][DELAY_L]);
        //ddr_printf (DDR_NORMAL, "\n pr_bus_skwr[dq%d]   =, %4d,\n",phy_x,pr_bus_skwr[phy_x]);
		
        // ============================================================================================================================
        // ========================================== D U T Y   C Y C L E   D I S T O R T I O N  ======================================
        // ============================================================================================================================
        if(WR_Training_Switchboard[prfs_indx][_DCC]==1)
        {
            dq_dcc_adj                                               =  (pr_max_eyew[1/*ODD*/][phy_x]-pr_max_eyew[0/*even*/][phy_x]);
            dqsdcc_adj                                               =   pr_max_midp[1/*ODD*/][phy_x]-pr_max_midp[0/*even*/][phy_x];
            dcccsr = DDRSS_wr_dcc2csr(dqsdcc_adj);
 			DDR_PHY_hal_wr_io_dqsdcc_CSR ( reg_offset_ddr_dq[phy_x], cs, 7, dcccsr);
			DDR_PHY_hal_wr_io_dqsdcc_CSR ( reg_offset_ddr_dq[phy_x], cs, 6, dcccsr);
			DDR_PHY_hal_wr_io_dqsdcc_CSR ( reg_offset_ddr_dq[phy_x], cs, 5, dcccsr);
			DDR_PHY_hal_wr_io_dqsdcc_CSR ( reg_offset_ddr_dq[phy_x], cs, 4, dcccsr);
			DDR_PHY_hal_wr_io_dqsdcc_CSR ( reg_offset_ddr_dq[phy_x], cs, 3, dcccsr);
#if DSF_WR_DQDQS_TRAINING_LOG_IN
            ddr_printf (DDR_NORMAL, "\n dq_dcc_adj[dq%d]   =, %4d,\n",phy_x,dq_dcc_adj);
            ddr_printf (DDR_NORMAL, "\n dqsdcc_adj[dq%d]   =, %4d,cdc=, %4d\n",phy_x,dqsdcc_adj,dcccsr);
#endif
        }

			 
			 
        midp[phy_x] = pr_min_midp[2][phy_x] ;
		
		
		
        // ============================================================================================================================
        // Write the trained coarse and fine center to the active CDC register
        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[midp[phy_x] ][COARSE_L],
                                    0x1,  // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[midp[phy_x] ][FINE_L  ],
                                    0x0, // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        // ============================================================================================================================
        // Write the trained coarse and fine to the EXT registers
        DDR_PHY_hal_cfg_cdcext_slave_wr(reg_offset_ddr_dq[phy_x] ,
                                        cs,//cs
                                        local_vars->cdc_ref[ midp[phy_x] ][COARSE_L],
                                        local_vars->cdc_ref[ midp[phy_x] ][FINE_L  ],
                                        prfs_indx
                                        );
                                        
       // ddr_printf (DDR_NORMAL,"  WR CDC ch = %d cs = %d byte = %d coarse = %d fine = %d\n",
       //                   ch,cs,phy_x,local_vars->cdc_ref[mid_tmp ][COARSE_L],
       //                                local_vars->cdc_ref[mid_tmp ][FINE_L  ]);
        // ============================================================================================================================

        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            perbit[phy_x][bit]  = local_vars->pass_reg[2][phy_x][bit][MIDP]  -    pr_min_midp[2][phy_x];
            if (perbit[phy_x][bit] > 0xf){ perbit[phy_x][bit]=0xf;}
            remapped_bit  =        bit_remapping_bimc2phy_DQ[ch][phy_x][bit];
            DDR_PHY_hal_cfg_pbit_dq_delay(reg_offset_ddr_dq[phy_x],
                                            remapped_bit,
                                            1, // 1 for Tx
                                            cs,//cs
                                            perbit[phy_x][bit]);
        }
		bit= BYTE_SIZE;
        perbit[phy_x][bit] = local_vars->pass_reg[2][phy_x][bit][MIDP]  -    pr_min_midp[2][phy_x];
        if (perbit[phy_x][bit] > 0xf){ perbit[phy_x][bit]=0xf ;}
        DDR_PHY_hal_cfg_pbit_dq_delay(reg_offset_ddr_dq[phy_x],
                                        dq_dbi_bit,
                                        1, // 1 for Tx
                                        cs,//cs
                                        perbit[phy_x][bit]) ;
    }//phy_x
    
#if DSF_WR_DQDQS_TRAINING_LOG_IN
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
    ddr_printf(DDR_NORMAL, "\n");
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        ddr_printf (DDR_NORMAL, "DQ%d,knob, %d, coarse, %d, fine, %d, delay, %d,\n"
                                , phy_x
                                , midp[phy_x]
                                , local_vars->cdc_ref[ midp[phy_x] ][COARSE_L]
                                , local_vars->cdc_ref[ midp[phy_x] ][  FINE_L]
                                , local_vars->cdc_ref[ midp[phy_x] ][ DELAY_L]);        
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            ddr_printf (DDR_NORMAL, "%3d,",bit);
        }
        ddr_printf (DDR_NORMAL, "dbi,");
        ddr_printf (DDR_NORMAL, " memory bit,\n");
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
            ddr_printf (DDR_NORMAL, "%3d,",remapped_bit);
        }
        ddr_printf (DDR_NORMAL, "%3d,",dq_dbi_bit);
        ddr_printf (DDR_NORMAL, " PHY_bit,\n");
        // ============================================================================================================================
        for (pr_x = 0; pr_x < 4; pr_x++)
        {
            for (bit = 0; bit < BYTE_SIZE+1/*+dbi*/; bit++)
            {
                ddr_printf (DDR_NORMAL, "%3d,",local_vars->pass_reg[2][phy_x][bit][pr_x]);
            }
                ddr_printf (DDR_NORMAL, "%s,\n",pr_string[pr_x]);
        }
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE+1/*+dbi*/; bit++)
        {
            ddr_printf (DDR_NORMAL, "%3d,",perbit[phy_x][bit]);
        }
         ddr_printf (DDR_NORMAL, "%s,\n"," perbit  delay");
       // ============================================================================================================================
        ddr_printf (DDR_NORMAL, "pr_bus_eyew[%d]=, %d , ps, %d,\n"
		                      , phy_x
							  , pr_bus_eyew[phy_x]
							  , pr_bus_eyew[phy_x] );
        ddr_printf (DDR_NORMAL, "pr_bus_skwr[%d]=, %d , ps, %d,\n"
		                      , phy_x
							  , pr_bus_skwr[phy_x]
		                      , pr_bus_skwr[phy_x] );
    }//phy_x

#endif
    return(TRUE);
}// END OF FUNCTION
//
// ============================================================================================================================
// ======================================      W R I T E   DQDQS  T R A I N I N G                =================================
// ============================================================================================================================
boolean DDRSS_wr_dqdqs_1D_cf(BOOT_DDR_STRUCT                   *ddr,
                                   uint8                        ch,
                                   uint8                        cs,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8 training_ddr_freq_indx_eq_training_table_index
                                   )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
    extern  uint8   bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
    extern  uint8   bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    uint8         loop_cnt = 0 ;
    uint16            knob = 0 ;
    uint16      sweep_size = 0 ;
    uint8            phy_x = 0 ;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
     uint8     fail_histogram[3][NUM_DQ_PCH][HISTOGRAM_SIZE]={{{0}}};
  FAILCELL                          fail_cell;
    uint8     pr[NUM_DQ_PCH][NUM_PR_PARAMS];

    // ============================================================================================================================
    // ======================================      I N I T I A L I Z A T I O N                    =================================
    // ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ1_DDR_PHY_OFFSET;


    //clearing the histogram initializing to Zero
    memset(fail_histogram, 0,3 * NUM_DQ_PCH *HISTOGRAM_SIZE);
    DDRSS_cdcref_init_wr(local_vars->cdc_ref);//cdc ruler init

    // ============================================================================================================================
    // ============================================================================================================================
    // ======================================     DQ WR     C D C           S W E E P I N G       =================================
    // ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size(clk_freq_khz);
    for (knob = 0;  knob < sweep_size;  knob++)
    {
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][COARSE_L],
                                            0x1, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            0); //cs
            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L],
                                            0x0, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            0); //cs
            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][COARSE_L],
                                            0x1, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            1); //cs
            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L],
                                            0x0, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            1); //cs
        }//phy_x
        fail_cell.w=0;
        for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            DDRSS_mem_write_PXI (ch,cs);
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
        }//loop count
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            fail_histogram [0][phy_x][knob]=fail_cell.b[0][phy_x] ;
            fail_histogram [1][phy_x][knob]=fail_cell.b[1][phy_x] ;
            fail_histogram [2][phy_x][knob]=fail_cell.b[0][phy_x] | fail_cell.b[1][phy_x];
        }//phy_x
    }//knob
    // ============================================================================================================================
    // ======================================      calculation                   =================================
    // ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        DDRSS_passing_region_scan(  &pr[phy_x][STRT],
                                    &pr[phy_x][STOP],
                                    fail_histogram[2][phy_x],
                                    0,sweep_size);
        pr[phy_x][MIDP] = (pr[phy_x][STOP]+pr[phy_x][STRT])>>1;
        pr[phy_x][EYEW] =  pr[phy_x][STOP]-pr[phy_x][STRT];
        // ============================================================================================================================
        // ==========================================             S P E C   C h e c k            ======================================
        // ============================================================================================================================ 
       
        if(pr[phy_x][EYEW] == 0)
          {
             ddr_printf(DDR_ERROR, "\nDDR ABORT: WR 1D CF ZERO EYE WIDTH, Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", clk_freq_khz, ch, cs, phy_x );
             ddr_abort();
          }
       // This should not happen at 1804 (or Max freq.) before Vref training
        #if DSF_SMALL_EYE_DETECTION_LOG
        if((prfs_indx != 3) && (prfs_indx != 7))
        {
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 1: WR MINIMUM EYE WIDTH = %d (ps)= %d (f steps), Vref check point = %d (step) = - (mV), Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", pr[phy_x][EYEW] * FINE_STEP_IN_PS, pr[phy_x][EYEW], DDRSS_get_wr_vref(ddr, ch, cs, phy_x, clk_freq_khz), clk_freq_khz, ch, cs, phy_x);
        }
        #endif
        
        if((pr[phy_x][EYEW] < training_params_ptr->wr_dqdqs.min_eye_width) && (training_params_ptr->wr_dqdqs.min_eye_width_enable==1) && (prfs_indx != 7) )
          {
             ddr_printf(DDR_ERROR, "\n SCREEN 1: WR MINIMUM EYE WIDTH Abort  (EYEW = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", pr[phy_x][EYEW], clk_freq_khz, ch, cs, phy_x);
             training_params_ptr->small_eye_abort = 1;
          }
		
        // ============================================================================================================================
    // Write the trained coarse and fine center to the active CDC register
    DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                local_vars->cdc_ref[ pr[phy_x][MIDP] ][COARSE_L],
                                0x1,  // 0 for fine_delay 1 for coarse
                                HP_MODE,
                                cs);
    DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                local_vars->cdc_ref[ pr[phy_x][MIDP] ][FINE_L  ],
                                0x0, // 0 for fine_delay 1 for coarse
                                HP_MODE,
                                cs);
    // Write the trained coarse and fine to the EXT registers
    DDR_PHY_hal_cfg_cdcext_slave_wr(reg_offset_ddr_dq[phy_x] ,
                                    cs,//cs
                                    local_vars->cdc_ref[ pr[phy_x][MIDP] ][COARSE_L],
                                    local_vars->cdc_ref[ pr[phy_x][MIDP] ][FINE_L  ],
                                    prfs_indx
                                    );
    }//phy_x

#if DSF_WR_DQDQS_TRAINING_LOG_IN
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        ddr_printf(DDR_NORMAL, "    DQ%d,strt= %d, stop= %d , eyew= %d (%d ps) ,\n"
			                 , phy_x
			                 , pr[phy_x][STRT]
			                 , pr[phy_x][STOP]
			                 , pr[phy_x][EYEW]
			                 , local_vars->cdc_ref[   pr[phy_x][EYEW]   ][DELAY_L]);
        ddr_printf (DDR_NORMAL  , "    DQ%d,midp, %d, coarse, %d, fine, %d, Delay(ps), %d,  \n"
                                , phy_x
                                , pr[phy_x][MIDP]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][COARSE_L]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][  FINE_L]
                                , local_vars->cdc_ref[ pr[phy_x][MIDP] ][ DELAY_L]);
        // ============================================================================================================================
    }//phy_x
#endif
    return(1);
}// END OF FUNCTION
// ============================================================================================================================
// ======================================      W R I T E   DQDQS  T R A I N I N G                =================================
// ============================================================================================================================
boolean DDRSS_wr_dqdqs_2D_vcf(BOOT_DDR_STRUCT                   *ddr,
                                   uint8                        ch,
                                   uint8                        cs,
                                   PHY_BUS                      dq,
                                   training_params_t            *training_params_ptr,
                                   ddrss_rdwr_dqdqs_local_vars  *local_vars,
                                   uint32                       clk_freq_khz,
                                   uint8                        prfs_indx,
                                   uint8 training_ddr_freq_indx_eq_training_table_index,
                                   uint8                        sed_enable
                                   )
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================

  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
  extern  uint8 dq_dbi_bit   ;
  uint8 fail_histogram[NUM_DQ_PCH][HISTOGRAM_SIZE];
  uint8                           loop_cnt = 0 ;
  uint16                              knob = 0 ;
  uint8                             vref_x = 0 ;
  uint8                          vref_data = 0 ;
  uint16                        histg_size = 0 ;
  uint16                        sweep_size = 0 ;
  uint16                        sweep_step = 0 ;
  uint16                            v_step = 0 ;
  uint16                          s_vref_x = 0 ;
  uint8                              phy_x = 0 ;
  uint8                             fsp_op = 0 ;
  uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
  uint32        mid_cdc[NUM_DQ_PCH] = {0};
  FAILCELL  fail_cell;
  uint8 v_pr[MAXVREF][NUM_DQ_PCH][NUM_PR_PARAMS]={{{0}}};
  uint8 max_v_eyew[NUM_DQ_PCH]={0};
  uint8 v_th[NUM_DQ_PCH]={0};
  uint8 v_hist[3][MAXVREF]={{0}};
  uint8  min_eye_width_mask_top_Vref;
  uint8  min_eye_width_mask_bottom_Vref;
  
  uint8 vmin[3]={0};
  uint8 vmax[3]={0};
  uint8 mid_v[NUM_DQ_PCH]={0};
#if DSF_SMALL_EYE_DETECTION_LOG
  uint8  mid_v_p60    = 0;
  uint8  mid_v_n60    = 0;
  uint32 mid_v_p60_mv = 0;
  uint32 mid_v_n60_mv = 0;
  uint32 min_eye_width_mask_top_Vref_mv;
  uint32 min_eye_width_mask_bottom_Vref_mv;
#endif
  
  uint8 scaled_prfs_indx         = 0;
  uint8 scale                    = 0;
  uint32 scaled_cdc_delay_in_ps  = 0; 
  uint32 target_freq             = 0; 

#if DSF_WR_VREF_BYTE_MODE_EN
  uint8 MR31_value               = 0;
  Device_Io_Width_sel wr_io_width = DEVICE_WIDTH_IS_x16;
#endif
  
  training_data *training_data_ptr;
  training_data_ptr = (training_data *)(&ddr->training_results.training_data);
#if DSF_WR_VREF_BYTE_MODE_EN
	wr_io_width = (cs == 0 )? 
				(Device_Io_Width_sel)ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs0_io_width[ch] :
				(Device_Io_Width_sel)ddr->shrm_ddr_ptr->ddr_size_info.ddr_cs1_io_width[ch];
#endif

// ============================================================================================================================
// ======================================      I N I T I A L I Z A T I O N                    =================================
// ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_dq[0]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]  =  REG_OFFSET_DDR_PHY_CH(ch)+ DQ1_DDR_PHY_OFFSET;
    DDRSS_cdcref_init_wr(local_vars->cdc_ref);//cdc ruler init
    // ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size(clk_freq_khz);
    sweep_step = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 4;
    if(sed_enable == 0)
    {
    	sweep_step=2;
    }
    histg_size = sweep_size/sweep_step;
    // ---------------------------------------------------------------------------------
    //              clearing the histogram initializing to Zero
        memset(fail_histogram, 0,NUM_DQ_PCH *HISTOGRAM_SIZE);


// ============================================================================================================================
// ======================================     DQ WR     C D C           S W E E P I N G       =================================
// ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
// ============================================================================================================================
     v_step=training_params_ptr->wr_dqdqs.vref_step;
    for (vref_x  = training_params_ptr->wr_dqdqs.vref_start_value;  
         vref_x  < training_params_ptr->wr_dqdqs.vref_max_value;  
         vref_x += training_params_ptr->wr_dqdqs.vref_step)
    {
        vref_data=vknob2vrange(vref_x);
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);

        for (knob = 0;  knob < sweep_size;  knob+=sweep_step)
        {
            //  =================================Set knob start
            for (phy_x = 0; phy_x < dq ; phy_x++)
            {
                DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                                local_vars->cdc_ref[knob][COARSE_L],
                                                0x1, // 0 for fine_delay 1 for coarse
                                                HP_MODE,
                                                cs);
                DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                                local_vars->cdc_ref[knob][FINE_L],
                                                0x0, // 0 for fine_delay 1 for coarse
                                                HP_MODE,
                                                cs);
            }//phy_x
            //  =================================Set knob end
            fail_cell.w=0;
            for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
            {
#if DSF_PXI_TRAINING_EN 
                Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                DDRSS_mem_write_PXI (ch,cs);
#if DSF_PXI_TRAINING_EN 
                Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
                fail_cell.w |=  DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
            }//loop count

            for (phy_x = 0; phy_x < dq ; phy_x++)
            {
                fail_histogram[phy_x][knob/sweep_step]=fail_cell.b[0][phy_x] | fail_cell.b[1][phy_x];
             }//phy_x
            //ddr_printf (DDR_NORMAL,"%d ,",fail_histogram[phy_x][knob/sweep_step]);
                            
        }  //End of knob loop
        //ddr_printf (DDR_NORMAL,"\n");
		s_vref_x=vref_x/v_step;
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            DDRSS_passing_region_scan(  &v_pr[s_vref_x][phy_x][STRT],
                                        &v_pr[s_vref_x][phy_x][STOP],
                                        fail_histogram[phy_x],
                                        0,histg_size);
            v_pr[s_vref_x][phy_x][STRT] = v_pr[s_vref_x][phy_x][STRT]* sweep_step;
            v_pr[s_vref_x][phy_x][STOP] = v_pr[s_vref_x][phy_x][STOP]* sweep_step;
            v_pr[s_vref_x][phy_x][EYEW] =  v_pr[s_vref_x][phy_x][STOP]-v_pr[s_vref_x][phy_x][STRT];
            v_pr[s_vref_x][phy_x][MIDP] = (v_pr[s_vref_x][phy_x][STOP]+v_pr[s_vref_x][phy_x][STRT])>>1;

            if (max_v_eyew[phy_x] < v_pr[s_vref_x][phy_x][EYEW] )
            {
                max_v_eyew[phy_x]=   v_pr[s_vref_x][phy_x][EYEW] ;
                v_th[phy_x]      =   (max_v_eyew[phy_x]*8)/10;
                if (v_th[phy_x] < training_params_ptr->wr_dqdqs.min_eye_width)//This will guarantee all WR EYEs meet the MIN EYE requirments
                {
                    v_th[phy_x] = training_params_ptr->wr_dqdqs.min_eye_width;
                }
            }
		//ddr_printf (DDR_NORMAL, "v_th[%d]= %d,max_v_eyew[%d], %d,\n",phy_x,v_th[phy_x],phy_x,max_v_eyew[phy_x]);
        }//phy_x
        
        // Print v_pr per Byte 
        //ddr_printf (DDR_NORMAL,"vref_x= %d, phy_x= 0, strt= %d, stop= %d ,eye = %d ,wmidp= %d ,", vref_x,v_pr[s_vref_x][0][STRT],v_pr[s_vref_x][0][STOP],v_pr[s_vref_x][0][EYEW],v_pr[s_vref_x][0][MIDP]);
        //  
        //ddr_printf (DDR_NORMAL,"vref_x= %d, phy_x= 1, strt= %d, stop= %d ,eye = %d ,wmidp= %d ,\n",vref_x,v_pr[s_vref_x][1][STRT],v_pr[s_vref_x][1][STOP],v_pr[s_vref_x][1][EYEW],v_pr[s_vref_x][1][MIDP]);  
    }//vref_x
#if DSF_WR_VREF_BYTE_MODE_EN
    if (wr_io_width == DEVICE_WIDTH_IS_x16)
    {
    //for (phy_x = 0; phy_x < dq ; phy_x++)
    //{
        for(vref_x  = training_params_ptr->wr_dqdqs.vref_start_value;
            vref_x  < training_params_ptr->wr_dqdqs.vref_max_value;
            vref_x += training_params_ptr->wr_dqdqs.vref_step)
        {
 		s_vref_x=vref_x/v_step;
            if ((v_th[0/*phy_x*/]<=v_pr[s_vref_x][0/*phy_x*/][EYEW] )&& 
			    (v_th[1/*phy_x*/]<=v_pr[s_vref_x][1/*phy_x*/][EYEW] ))
            {
                v_hist[0][s_vref_x]=0;
            }
            else
            {
                v_hist[0][s_vref_x]=1;
            }
            //ddr_printf (DDR_NORMAL, "vref_x, %d,v_hist, %d,\n",vref_x,v_hist[s_vref_x]);
			
			if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ ))
            {
                v_hist[1][s_vref_x]=0;
            }
            else
            {
                v_hist[1][s_vref_x]=1;
            }
			
            if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  ))
            {
                v_hist[2][s_vref_x]=0;
            }
            else
            {
                v_hist[2][s_vref_x]=1;
            }
        }//vref_x
        /*DDRSS_passing_region_scan(  &vmin,
                                    &vmax,
                                    v_hist,
                                    training_params_ptr->wr_dqdqs.coarse_vref_start_value,
                                    (training_params_ptr->wr_dqdqs.coarse_vref_max_value)/v_step);  */
		DDRSS_passing_region_scan(   &vmin[0],
                                     &vmax[0],
                                    v_hist[0],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[1],
                                     &vmax[1],
                                    v_hist[1],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[2],
                                     &vmax[2],
                                    v_hist[2],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        mid_v[0]       = ((vmin[0]+vmax[0])>>1);//div2
        vref_data   = vknob2vrange(mid_v[0]*v_step);
        fsp_op = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);
        training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][0][fsp_op] = vref_data;
        mid_v[1] = mid_v[0];
        
    }
    else // if X8 device
    {
      for (phy_x = 0; phy_x < dq ; phy_x++)
      {
        for(vref_x  = training_params_ptr->wr_dqdqs.vref_start_value;
            vref_x  < training_params_ptr->wr_dqdqs.vref_max_value;
            vref_x += training_params_ptr->wr_dqdqs.vref_step)
        {
 		s_vref_x=vref_x/v_step;
            if ((v_th[phy_x]<=v_pr[s_vref_x][phy_x][EYEW] ) )
            {
                v_hist[0][s_vref_x]=0;
            }
            else
            {
                v_hist[0][s_vref_x]=1;
            }
            //ddr_printf (DDR_NORMAL, "vref_x, %d,v_hist, %d,\n",vref_x,v_hist[s_vref_x]);
			if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ ))
            {
                v_hist[1][s_vref_x]=0;
            }
            else
            {
                v_hist[1][s_vref_x]=1;
            }
			
            if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  ))
            {
                v_hist[2][s_vref_x]=0;
            }
            else
            {
                v_hist[2][s_vref_x]=1;
            }
        }//vref_x
      /*  DDRSS_passing_region_scan(  &vmin,
                                    &vmax,
                                    v_hist,
                                    training_params_ptr->wr_dqdqs.coarse_vref_start_value,
                                    (training_params_ptr->wr_dqdqs.coarse_vref_max_value)/v_step);   */

		DDRSS_passing_region_scan(   &vmin[0],
                                     &vmax[0],
                                    v_hist[0],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[1],
                                     &vmax[1],
                                    v_hist[1],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[2],
                                     &vmax[2],
                                    v_hist[2],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        mid_v[phy_x]       = ((vmin[0]+vmax[0])>>1);//div2
        vref_data   = vknob2vrange(mid_v[phy_x]*v_step);
        fsp_op = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
                //Enable byte mode for individual bytes
        MR31_value = 1 << (7 - phy_x) | 0 << (6 + phy_x); // Enable the byte to receive MR14 values and disable the other Byte
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_31, MR31_value);
        //Program the Vref value
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);                
        // Disable the Byte mode
        MR31_value = 0x0;
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_31, MR31_value);

        training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][phy_x][fsp_op] = vref_data;


     }
	 mid_v[0]       = ((mid_v[0]+mid_v[1])>>1);//div2
   }
#else


        for(vref_x  = training_params_ptr->wr_dqdqs.vref_start_value;
            vref_x  < training_params_ptr->wr_dqdqs.vref_max_value;
            vref_x += training_params_ptr->wr_dqdqs.vref_step)
        {
 		s_vref_x=vref_x/v_step;
            if ((v_th[0/*phy_x*/]<=v_pr[s_vref_x][0/*phy_x*/][EYEW] )&& 
			    (v_th[1/*phy_x*/]<=v_pr[s_vref_x][1/*phy_x*/][EYEW] ))
            {
                v_hist[0][s_vref_x]=0;
            }
            else
            {
                v_hist[0][s_vref_x]=1;
            }
            //ddr_printf (DDR_NORMAL, "vref_x, %d,v_hist, %d,\n",vref_x,v_hist[s_vref_x]);
            if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= training_params_ptr->wr_dqdqs.min_eye_width /*MIN_EYE_WIDTH*/ ))
            {
                v_hist[1][s_vref_x]=0;
            }
            else
            {
                v_hist[1][s_vref_x]=1;
            }
			
            if ((v_pr[s_vref_x][0/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  )&& 
			    (v_pr[s_vref_x][1/*phy_x*/][EYEW] >= FINE_STEPS_PER_COARSE  ))
            {
                v_hist[2][s_vref_x]=0;
            }
            else
            {
                v_hist[2][s_vref_x]=1;
            }
			
        }//vref_x
    
        DDRSS_passing_region_scan(   &vmin[0],
                                     &vmax[0],
                                    v_hist[0],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[1],
                                     &vmax[1],
                                    v_hist[1],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);
        DDRSS_passing_region_scan(   &vmin[2],
                                     &vmax[2],
                                    v_hist[2],
                                    training_params_ptr->wr_dqdqs.vref_start_value,
                                   (training_params_ptr->wr_dqdqs.vref_max_value)/v_step);									
									
        mid_v[0]       = ((vmin[0]+vmax[0])>>1);//div2
        vref_data   = vknob2vrange(mid_v[0]*v_step);
        fsp_op = (clk_freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
        MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14, vref_data);
        training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][0][fsp_op] = vref_data;
        mid_v[1] = mid_v[0];
#endif

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------START: Screen Logic copied from NapaliV2 DSFv242p2----------------------------------------------------
---------------------------------------------------------START: Screen Logic copied from NapaliV2 DSFv242p2----------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------------------------- */
		if( (vmax[0] == 0) || (mid_v[0] >= MAXVREF))
        {
             ddr_printf(DDR_ERROR, "\nDDR ABORT: WR 2D VCF ZERO EYE HEIGHT, Frequency = %d, Channel = %d, Rank = %d, Byte = %d\n", clk_freq_khz, ch, cs, phy_x );
             ddr_abort();
        }  

		if(sed_enable)
		{
        
        #if DSF_SMALL_EYE_DETECTION_LOG 
            
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 1: WR MINIMUM EYE WIDTH = %d (ps)= %d (f steps), Frequency = %d, Channel = %d, Rank = %d, Byte = 0\n"
            , v_pr[mid_v[0]][0/*phy_x*/][EYEW] * FINE_STEP_IN_PS, v_pr[mid_v[0]][0/*phy_x*/][EYEW]
            , clk_freq_khz, ch, cs );

            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 1: WR MINIMUM EYE WIDTH = %d (ps)= %d (f steps), Frequency = %d, Channel = %d, Rank = %d, Byte = 1\n"
            , v_pr[mid_v[0]][1/*phy_x*/][EYEW] * FINE_STEP_IN_PS, v_pr[mid_v[0]][1/*phy_x*/][EYEW]
            , clk_freq_khz, ch, cs );
            
            min_eye_width_mask_top_Vref =((mid_v[0] + (training_params_ptr->wr_dqdqs.min_eye_width_mask_top)/v_step) < 78/v_step ) ?  (mid_v[0] + (training_params_ptr->wr_dqdqs.min_eye_width_mask_top)/v_step) : 78/v_step;
            min_eye_width_mask_top_Vref_mv = min_eye_width_mask_top_Vref*v_step*3.6+90;
            
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 4: WR min_eye_width_mask_top, Vref check point = %d (step) = %d (mV), Frequency = %d, Channel = %d, Rank = %d, Byte= 0, EYE width = %d (ps) = %d (f steps), Byte= 1, EYE width = %d (ps) = %d (f steps)"
            ,min_eye_width_mask_top_Vref*v_step, min_eye_width_mask_top_Vref_mv 
            , clk_freq_khz, ch, cs 
            ,v_pr[min_eye_width_mask_top_Vref][0/*phy_x*/][EYEW]* FINE_STEP_IN_PS, v_pr[min_eye_width_mask_top_Vref][0/*phy_x*/][EYEW]
            ,v_pr[min_eye_width_mask_top_Vref][1/*phy_x*/][EYEW]* FINE_STEP_IN_PS, v_pr[min_eye_width_mask_top_Vref][1/*phy_x*/][EYEW]
            );
            
        #endif
        
        if( ((vmax[1] < mid_v[0]) || (((vmax[1] - mid_v[0])*v_step) <   training_params_ptr->wr_dqdqs.min_eye_width_mask_top/*MIN_EYE_WIDTH*/))&& (training_params_ptr->wr_dqdqs.min_eye_width_mask_top_enable==1) )
          {
            min_eye_width_mask_top_Vref =((mid_v[0] + (training_params_ptr->wr_dqdqs.min_eye_width_mask_top)/v_step) < 78/v_step ) ?  (mid_v[0] + (training_params_ptr->wr_dqdqs.min_eye_width_mask_top)/v_step) : 78/v_step;
            
            ddr_printf(DDR_ERROR, "\n SCREEN 4: WR min_eye_width_mask_top Abort  (eye_height_mask_top = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n"
            , (v_pr[min_eye_width_mask_top_Vref][0/*phy_x*/][EYEW] < v_pr[min_eye_width_mask_top_Vref][1/*phy_x*/][EYEW])? v_pr[min_eye_width_mask_top_Vref][0/*phy_x*/][EYEW] : v_pr[min_eye_width_mask_top_Vref][1/*phy_x*/][EYEW]
            , clk_freq_khz, ch, cs );
            
            training_params_ptr->small_eye_abort = 1;
          }
			
        #if DSF_SMALL_EYE_DETECTION_LOG

            min_eye_width_mask_bottom_Vref = ((mid_v[0] - (training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom)/v_step) < 0 ) ?  0 : mid_v[0] - (training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom)/v_step;
            min_eye_width_mask_bottom_Vref_mv = min_eye_width_mask_bottom_Vref*v_step*3.6+90; 
            
            ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 4: WR min_eye_width_mask_bottom, Vref check point = %d (step) = %d (mV), Frequency = %d, Channel = %d, Rank = %d, Byte= 0, EYE width = %d (ps) = %d (f steps), Byte= 1, EYE width = %d (ps) = %d (f steps)"
            ,min_eye_width_mask_bottom_Vref*v_step, min_eye_width_mask_bottom_Vref_mv
            , clk_freq_khz, ch, cs 
            ,v_pr[min_eye_width_mask_bottom_Vref][0/*phy_x*/][EYEW]* FINE_STEP_IN_PS, v_pr[min_eye_width_mask_bottom_Vref][0/*phy_x*/][EYEW]
            ,v_pr[min_eye_width_mask_bottom_Vref][1/*phy_x*/][EYEW]* FINE_STEP_IN_PS, v_pr[min_eye_width_mask_bottom_Vref][1/*phy_x*/][EYEW]
            );
            
        #endif
        
        if( ((mid_v[0] < vmin[1]) || ((( mid_v[0]-vmin[1] )*v_step) <  training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom)) && (training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom_enable==1))
          {
            min_eye_width_mask_bottom_Vref= ((mid_v[0] - (training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom)/v_step) < 0 ) ?  0 : mid_v[0] - (training_params_ptr->wr_dqdqs.min_eye_width_mask_bottom)/v_step;
            ddr_printf(DDR_ERROR, "\n SCREEN 4: WR min_eye_width_mask_bottom Abort  (eye_height_mask_bottom = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n"
            , ( v_pr[min_eye_width_mask_bottom_Vref][0/*phy_x*/][EYEW] < v_pr[min_eye_width_mask_bottom_Vref][1/*phy_x*/][EYEW]? v_pr[min_eye_width_mask_bottom_Vref][0/*phy_x*/][EYEW] : v_pr[min_eye_width_mask_bottom_Vref][1/*phy_x*/][EYEW] ) 
            , clk_freq_khz, ch, cs );      
            
            training_params_ptr->small_eye_abort = 1;
          }

			#if DSF_SMALL_EYE_DETECTION_LOG
				ddr_printf(DDR_NORMAL, "\nSmall Eye SCREEN 2: WR MINIMUM EYE HEIGHT = %d , Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", (vmax[2]<vmin[2]) ? 0 : (vmax[2] - vmin[2]) * v_step, clk_freq_khz, ch, cs);
			#endif
			
			if( ((vmax[2] < vmin[2]) || (((vmax[2]-vmin[2])*v_step ) < training_params_ptr->wr_dqdqs.min_eye_height)) && (training_params_ptr->wr_dqdqs.min_eye_height_enable==1))
			  {
				 ddr_printf(DDR_ERROR, "\n SCREEN 2: WR MINIMUM EYE HEIGHT Abort  (eye_height = %d), Frequency = %d, Channel = %d, Rank = %d, Byte = 0&1\n", (vmax[2]<vmin[2]) ? 0 : (vmax[2] - vmin[2]) * v_step, clk_freq_khz, ch, cs);
				 training_params_ptr->small_eye_abort = 1;
			  }
		}
	
    for (phy_x = 0; phy_x < dq ; phy_x++)
    {		
        if(mid_v[phy_x]<MAXVREF)
        {
            mid_cdc[phy_x] = v_pr[mid_v[phy_x]][phy_x][MIDP];
        }
        else
        {
           ddr_printf (DDR_NORMAL, "NOVREF Accepted");
           mid_cdc[phy_x]=0;
        }
#if DSF_WR_DQDQS_TRAINING_LOG_IN
        ddr_printf (DDR_NORMAL, "DQ, %d,   \n",phy_x);
        //ddr_printf (DDR_NORMAL, "vmin, %d,vmax, %d  mid_v(Dec): %d, vref_data(Hex): %x   \n",vmin,vmax,mid_v,vref_data );
        ddr_printf (DDR_NORMAL, "vmin, %d,vmax, %d  mid_v(Dec): %d, vref_data(Hex): %x   \n",vmin[0]*v_step,vmax[0]*v_step,mid_v[0]*v_step,vref_data );
		ddr_printf (DDR_NORMAL, "mid_cdc, %d, coarse, %d, fine, %d, delay, %d, \n"
                ,mid_cdc[phy_x]
                ,local_vars->cdc_ref[mid_cdc[phy_x] ][COARSE_L]
                ,local_vars->cdc_ref[mid_cdc[phy_x] ][FINE_L]
                ,local_vars->cdc_ref[mid_cdc[phy_x] ][DELAY_L]
                );
#endif

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------START: Screen Logic copied from NapaliV2 DSFv242p2----------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------------------------- */
	#if DSF_SMALL_EYE_DETECTION_LOG
    if(sed_enable)
	{

		mid_v_p60 = mid_v[0] + (60/(v_step*3.6));
        mid_v_n60 = (mid_v[0] < (60/(v_step*3.6))) ? 0 : mid_v[0] - (60/(v_step*3.6));
        mid_v_p60_mv = (mid_v_p60 * v_step * 3.6) + 90;
        mid_v_n60_mv = (mid_v_n60 * v_step * 3.6) + 90;
			
            ddr_printf (DDR_NORMAL, "\nSmall Eye SCREEN x: WR SIPI Aperture width +60mv Vref check point = %d (step) = %d (mv), Frequency = %d, Channel = %d, Rank = %d, Byte = %d, EYE width %d (ps) = %d (f steps)\n",
                mid_v_p60 * v_step, mid_v_p60_mv, clk_freq_khz, ch, cs, phy_x, v_pr[mid_v_p60][phy_x][EYEW] * FINE_STEP_IN_PS, v_pr[mid_v_p60][phy_x][EYEW]);
            ddr_printf (DDR_NORMAL, "\nSmall Eye SCREEN x: WR SIPI Aperture width -60mv Vref check point = %d (step) = %d (mv), Frequency = %d, Channel = %d, Rank = %d, Byte = %d, EYE width %d (ps) = %d (f steps)\n",
                mid_v_n60 * v_step, mid_v_n60_mv, clk_freq_khz, ch, cs, phy_x, v_pr[mid_v_n60][phy_x][EYEW] * FINE_STEP_IN_PS, v_pr[mid_v_n60][phy_x][EYEW]);
	}
	#endif
	
/*-----------------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------END: Screen Logic copied from NapaliV2 DSFv242p2----------------------------------------------------
---------------------------------------------------------END: Screen Logic copied from NapaliV2 DSFv242p2----------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------------------------------- */

        // Write the trained coarse and fine center to the active CDC register
        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[mid_cdc[phy_x] ][COARSE_L],
                                    0x1,  // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                    local_vars->cdc_ref[mid_cdc[phy_x] ][FINE_L  ],
                                    0x0, // 0 for fine_delay 1 for coarse
                                    HP_MODE,
                                    cs);
        // ============================================================================================================================
        // Write the trained coarse and fine to the EXT registers
        DDR_PHY_hal_cfg_cdcext_slave_wr(reg_offset_ddr_dq[phy_x] ,
                                        cs,//cs
                                        local_vars->cdc_ref[mid_cdc[phy_x] ][COARSE_L],
                                        local_vars->cdc_ref[mid_cdc[phy_x] ][FINE_L  ],
                                        prfs_indx
                                        );
        
        // ============================================================================================================================
    }//phy_x
    // ============================================================================================================================
    // ======================================              scaling                     =================================
    // ============================================================================================================================
    if(clk_freq_khz < LOWEST_TRAINED_FPRS_FREQ)
    {
      for (scale = 1; scale <= NUM_SCALE_BANDS; scale++)
      {
        // Calculate the scaled PRFS index
        scaled_prfs_indx = prfs_indx - scale ;
        target_freq      = SCALE_FREQ(scaled_prfs_indx);
                                             
        for (phy_x = 0; phy_x < dq; phy_x++)
        {   
          scaled_cdc_delay_in_ps = (clk_freq_khz * local_vars->cdc_ref[mid_cdc[phy_x] ][DELAY_L])/SCALE_FREQ(scale-1); 
#if DSF_WR_DQDQS_TRAINING_LOG_IN          
          ddr_printf(DDR_NORMAL,"SCALE : Source delay %d ps  Target delay %d ps\n",
                                 local_vars->cdc_ref[mid_cdc[phy_x] ][DELAY_L],scaled_cdc_delay_in_ps); 
#endif                                 
                                 
          DDRSS_WR_midpoint_to_CDC (ddr,
                                    ch,
                                    cs,
                                    scaled_cdc_delay_in_ps,
                                    0, //half
                                    phy_x,
                                    target_freq,
                                    scaled_prfs_indx);

        } //phy_x
      }
    }
    
    return(1);
}// END OF FUNCTION

// ===============================================================================================================================
// ======================================      W R I T E     T R A I N I N G    LOG    ====================================
// ===============================================================================================================================
boolean DDRSS_wr_dqdqs_log (BOOT_DDR_STRUCT            *ddr,
                            uint8                        ch,
                            uint8                        cs,
                            PHY_BUS                      dq,
                            training_params_t            *training_params_ptr,
                            ddrss_rdwr_dqdqs_local_vars  *local_vars,
                            uint32                       clk_freq_khz,
                            uint8                        prfs_indx)
{
// ============================================================================================================================
// ======================================      D E F I N I T I O N S                          =================================
// ============================================================================================================================
    int               dq_dcc_adj[NUM_DQ_PCH] = {0,0};
    int               dqsdcc_adj[NUM_DQ_PCH] = {0,0};

  extern  uint8  bit_remapping_phy2bimc_DQ[NUM_CH][NUM_DQ_PCH][PINS_PER_PHY];
  extern  uint8  bit_remapping_bimc2phy_DQ[NUM_CH][NUM_DQ_PCH][BYTE_SIZE];
    int                                 rf_x = 0 ;
    uint8                           loop_cnt = 0 ;
    uint8                                bit = 0 ;
    uint16                              knob = 0 ;
    uint16                        sweep_size = 0 ;
    uint8                              phy_x = 0 ;
    uint8                       remapped_bit = 0 ;
    uint32 fail_pat=0;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint32        pr_min_midp[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_max_midp[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_eyew[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32     safe_cdc_value[NUM_DQ_PCH] = {0,0} ;
    uint32        safe_coarse[NUM_DQ_PCH] = {0,0} ;
    uint32          safe_fine[NUM_DQ_PCH] = {0,0} ;
    uint32        pr_min_strt[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_stop[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_min_eyew[3][NUM_DQ_PCH] = {{199,199},{199,199},{199,199}};
    uint32        pr_max_strt[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
    uint32        pr_max_stop[3][NUM_DQ_PCH] = {{0,0},{0,0},{0,0}};
   uint32   safe_delay [NUM_DQ_PCH]  = {0,0};
   uint32   pr_bus_eyew[3][NUM_DQ_PCH]  = {{0,0},{0,0},{0,0}};
   uint32   pr_bus_skwr[3][NUM_DQ_PCH]  = {{0,0},{0,0},{0,0}};
   uint32   pr_x                     =  0   ;
   char     pr_string[4][25]  ={
                                   " passing region start",
                                   " passing region stop",
                                   " passing region midp",
                                   " passing region eyew"
                                    };

  uint32  allbit         = ALL_DQ_PASS_PAT ;
// ============================================================================================================================
// ======================================      I N I T I A L I Z A T I O N                    =================================
// ============================================================================================================================
    // Set DDR Base address registers
    reg_offset_ddr_dq[0]       =  REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]       =  REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
  for (phy_x = 0; phy_x < dq; phy_x++)
  {
     // Get the safe CDC value from the Shadow (ext) register
      safe_cdc_value[phy_x] = HWIO_INX ( reg_offset_ddr_dq[phy_x] , DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG ) ;
      if(cs==1)
      {
          safe_cdc_value[phy_x]= safe_cdc_value[phy_x]>>10;
      }
        // Parse the DQ CDC for the safe mode
      safe_coarse [phy_x]=  safe_cdc_value[phy_x]     & 0x1F;
      safe_fine   [phy_x]= (safe_cdc_value[phy_x]>>5) & 0x1F;
      safe_coarse [phy_x]= safe_coarse[phy_x];
      safe_fine   [phy_x]= safe_fine  [phy_x];
  }
  //clearing the histogram initializing to Zero
  memset(local_vars->failcount_histogram, 0,3 * NUM_DQ_PCH * PINS_PER_PHY * HISTOGRAM_SIZE);
// ======================================      cdc ref                   =================================
    DDRSS_cdcref_init_wr(local_vars->cdc_ref);
// ============================================================================================================================
// ============================================================================================================================
// ======================================     C A       C D C           S W E E P I N G       =================================
// ======================================     TO   C A P T U R E      H I S T O G R A M       =================================
// ============================================================================================================================
    sweep_size= DDRSS_get_sweep_size(clk_freq_khz);
   for (knob = 0;  knob < sweep_size;  knob++)
    {
        //  =================================Set knob start
        for (phy_x = 0; phy_x < dq ; phy_x++)
        {
            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][COARSE_L],
                                            0x1, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);

            DDR_PHY_hal_cfg_cdc_slave_wr(   reg_offset_ddr_dq[phy_x],
                                            local_vars->cdc_ref[knob][FINE_L],
                                            0x0, // 0 for fine_delay 1 for coarse
                                            HP_MODE,
                                            cs);
        }//phy_x
        //  =================================Set knob end
        for (loop_cnt = training_params_ptr->wr_dqdqs.max_loopcnt; loop_cnt > 0; loop_cnt --)
        {
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 0);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            DDRSS_mem_write_PXI (ch,cs);
#if DSF_PXI_TRAINING_EN 
            Update_PXI_Tables(ddr, ch, cs, 1);//wr_rd = 0 for write, 1 for read, 2 for both
#endif
            fail_pat    =   DDRSS_mem_read_Get_Fail_pat_PXI(ch,cs);
            allbit      =   DDRSS_fail2hist( fail_pat , local_vars->failcount_histogram, knob);
            if(allbit == ALL_DQ_FAIL_PAT) break;
        }//loop count
    }  //End of knob loop
   // ============================================================================================================================
   // ======================================      calculation                   =================================
   // ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            for (rf_x = 0; rf_x < 3; rf_x++)
            {
                //remapped_bit  =  bit_remapping_bimc2phy_DQ  [ch][phy_x][bit];
                DDRSS_passing_region_scan( &local_vars->pass_reg[rf_x][phy_x][bit][STRT],
                                           &local_vars->pass_reg[rf_x][phy_x][bit][STOP],
                                           local_vars->failcount_histogram [rf_x][phy_x][bit],
                                                        0,sweep_size);
                local_vars->pass_reg[rf_x][phy_x][bit][MIDP]  =(local_vars->pass_reg[rf_x][phy_x][bit][STOP]   +
                                                                local_vars->pass_reg[rf_x][phy_x][bit][STRT]) >>1 ; // midpoin equal to average
                local_vars->pass_reg[rf_x][phy_x][bit][EYEW]  =(local_vars->pass_reg[rf_x][phy_x][bit][STOP]   -
                                                                local_vars->pass_reg[rf_x][phy_x][bit][STRT] )  ; // eywidth  equal to stop - start
                if ( pr_max_midp[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_max_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_max_eyew[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_max_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_min_strt[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_min_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_min_stop[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_min_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
                if ( pr_min_midp[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ) { pr_min_midp[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ; }
                if ( pr_min_eyew[rf_x][phy_x] > local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ) { pr_min_eyew[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][EYEW] ; }
                if ( pr_max_strt[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STRT] ) { pr_max_strt[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STRT] ; }
                if ( pr_max_stop[rf_x][phy_x] < local_vars->pass_reg[rf_x][phy_x][bit][STOP] ) { pr_max_stop[rf_x][phy_x]  = local_vars->pass_reg[rf_x][phy_x][bit][STOP] ; }
            }
        }
        // ============================================================================================================================
        // ========================================== D U T Y   C Y C L E   D I S T O R T I O N  ======================================
        // ============================================================================================================================
        dq_dcc_adj[phy_x]=  (pr_max_eyew[0/*even*/][phy_x]-pr_max_eyew[1/*ODD*/][phy_x]);
        dqsdcc_adj[phy_x]=   pr_max_midp[0/*even*/][phy_x]-pr_max_midp[1/*ODD*/][phy_x];
    }

// ============================================================================================================================
// ==========================================Setting back coarse and fine values =====================================================
// ============================================================================================================================
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                      safe_coarse[phy_x],
                                      0x1, // 0 for fine_delay 1 for coarse
                                      HP_MODE,
                                      cs);

        DDR_PHY_hal_cfg_cdc_slave_wr(reg_offset_ddr_dq[phy_x],
                                       safe_fine[phy_x] ,
                                      0x0, // 0 for fine_delay 1 for coarse
                                      HP_MODE,
                                      cs);   // 0 for fine_delay_mode.
        safe_delay [phy_x]  = safe_coarse[phy_x]* COARSE_STEP_IN_PS + safe_fine[phy_x] * FINE_STEP_IN_PS;
    }
// ============================================================================================================================
// ======================================              L O G  P R I N T                       =================================
// ============================================================================================================================
    for (knob=0; knob < sweep_size-1; knob++)
    {
        for (phy_x = 0; phy_x < dq; phy_x++)
        {
            ddr_printf (DDR_NORMAL, "DQ%d,",phy_x);
            for (rf_x = 2; rf_x >= 0; rf_x--)
            {
                for (bit = 0; bit < BYTE_SIZE; bit++)
                {
                if(knob==local_vars->pass_reg[rf_x][phy_x][bit][MIDP] ){ddr_printf (DDR_NORMAL, " X,");}
                else{ddr_printf (DDR_NORMAL, "%2d,",local_vars->failcount_histogram[rf_x][phy_x][bit][knob]);}
                }
                if ((safe_delay[phy_x] >= local_vars->cdc_ref[knob  ][DELAY_L ]) &&
                    (safe_delay[phy_x] <  local_vars->cdc_ref[knob+1][DELAY_L])) {ddr_printf ( DDR_NORMAL, "<<<<<<<,");}
                else if             (         knob == pr_min_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_S,");}
                else if             (         knob == pr_max_strt[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_S,");}
                else if             (         knob == pr_min_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_M,");}
                else if             (         knob == pr_max_midp[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_M,");}
                else if             (         knob == pr_min_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<min_E,");}
                else if             (         knob == pr_max_stop[rf_x][phy_x] ) {ddr_printf ( DDR_NORMAL, "<<max_E,");}
                else if             (local_vars->cdc_ref[knob  ][FINE_L] == 0 )  {ddr_printf ( DDR_NORMAL, "      0,");}
                else                                                             {ddr_printf ( DDR_NORMAL, "       ,");}
            }
            ddr_printf(DDR_NORMAL,"%3d,%2d,%2d,%4d,%3X,",knob,
                                     local_vars->cdc_ref[knob][COARSE_L],
                                     local_vars->cdc_ref[knob][FINE_L  ],
                                     local_vars->cdc_ref[knob][DELAY_L ],
                                     local_vars->cdc_ref[knob][CDC_L   ]);
        }//phy_x
        ddr_printf(DDR_NORMAL, "\n");
    }//knob


    ddr_printf(DDR_NORMAL, "\n");
    for (phy_x = 0; phy_x < dq; phy_x++)
    {
        pr_bus_eyew[0][phy_x]  =local_vars->cdc_ref[pr_min_stop[0][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[0][phy_x]][DELAY_L ];
        pr_bus_skwr[0][phy_x]  =local_vars->cdc_ref[pr_max_midp[0][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[0][phy_x]][DELAY_L ];
        pr_bus_eyew[1][phy_x]  =local_vars->cdc_ref[pr_min_stop[1][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[1][phy_x]][DELAY_L ];
        pr_bus_skwr[1][phy_x]  =local_vars->cdc_ref[pr_max_midp[1][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[1][phy_x]][DELAY_L ];
        pr_bus_eyew[2][phy_x]  =local_vars->cdc_ref[pr_min_stop[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_max_strt[2][phy_x]][DELAY_L ];
        pr_bus_skwr[2][phy_x]  =local_vars->cdc_ref[pr_max_midp[2][phy_x]][DELAY_L ]  - local_vars->cdc_ref[pr_min_midp[2][phy_x]][DELAY_L ];
        ddr_printf (DDR_NORMAL, "=================DQ_%d,\n",phy_x);
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
            ddr_printf (DDR_NORMAL, "%4d,",remapped_bit);
        }
        ddr_printf (DDR_NORMAL, "PHY_bit,");
        ddr_printf (DDR_NORMAL, "bit\n");
        // ============================================================================================================================
        for (pr_x = 0; pr_x < 4; pr_x++)
        {
            for (bit = 0; bit < BYTE_SIZE; bit++)
            {
                ddr_printf (DDR_NORMAL, "%4d,",local_vars->pass_reg[2][phy_x][bit][pr_x]);
            }
                ddr_printf (DDR_NORMAL, "%s\n",pr_string[pr_x]);
        }
        // ============================================================================================================================
        for (bit = 0; bit < BYTE_SIZE; bit++)
        {
            remapped_bit  =  bit_remapping_bimc2phy_DQ[ch][phy_x][bit] ;
        }
        // ============================================================================================================================
        ddr_printf (DDR_NORMAL, "%s\n","perbit Delay");
        ddr_printf (DDR_NORMAL, "   wr_dq_dcc[%d][%d]=, %d ,\n"  ,cs ,phy_x,     dq_dcc_adj[phy_x]);
        ddr_printf (DDR_NORMAL, "   wr_dqsdcc[%d][%d]=, %d ,\n"  ,cs ,phy_x,     dqsdcc_adj[phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_eyew[phase0][%d]=, %d ,\n",phy_x, pr_bus_eyew[0][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_skwr[phase0][%d]=, %d ,\n",phy_x, pr_bus_skwr[0][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_eyew[phase1][%d]=, %d ,\n",phy_x, pr_bus_eyew[1][phy_x]);
        ddr_printf (DDR_NORMAL, "   pr_bus_skwr[phase1][%d]=, %d ,\n",phy_x, pr_bus_skwr[1][phy_x]);
        ddr_printf (DDR_NORMAL, "pr_bus_eyew[bothphase][%d]=, %d ,\n",phy_x, pr_bus_eyew[2][phy_x]);
        ddr_printf (DDR_NORMAL, "pr_bus_skwr[bothphase][%d]=, %d ,\n",phy_x, pr_bus_skwr[2][phy_x]);
    }//phy_x
    return(1);
}// END OF FUNCTION
uint16 DDRSS_wr_dcc2csr ( int dcc_adj)
{
    int dcc_edge = 0;
    //dcc_adj= even_anchor-odd_anchor;
    dcc_edge=0;
    if (dcc_adj< 0)
    {         
        dcc_adj= -dcc_adj;
        dcc_edge=1;
    }
    if (9 < dcc_adj)
    {
        dcc_adj  = 9;
    }
    return (dcc_adj<<(dcc_edge * 5));
}      

int DDRSS_get_wr_cdc_phy_offset(uint8 ch,uint8 cs, uint8 dq, uint32* const out_reg_offset)
{
	uint32 reg_offset_ddr_wr_cdc[NUM_DQ_PCH] = {0};

	//NUM_CS is static, not all may be populated
	if ( (ch >= NUM_CH) || (cs >= NUM_CS) || (dq >= NUM_DQ_PCH) )
	{
		return -1;
	}
	
	reg_offset_ddr_wr_cdc[0] = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
	reg_offset_ddr_wr_cdc[1] = REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;

        *out_reg_offset = reg_offset_ddr_wr_cdc[dq];

	return 0;
}

int DDRSS_get_cdcext_slave_wr(uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8* const coarse_delay, uint8* const fine_delay)
{
	uint8   prfs_x = 0;
	uint32 reg_offset_wr_cdc = 0;

	// bounds check inside DDRSS_get_wr_cdc_phy_offset
	if (DDRSS_get_wr_cdc_phy_offset(ch, cs, dq, &reg_offset_wr_cdc) < 0)
	{
		return -1;
	}

	prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);
	DDR_PHY_hal_get_cdcext_slave_wr( reg_offset_wr_cdc, cs, coarse_delay, fine_delay, prfs_x );

	return 0;
}


int DDRSS_set_cdcext_slave_wr(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 coarse_delay, uint8 fine_delay)
{
	uint8   prfs_x = 0;
	training_data *training_data_ptr;
	uint32 reg_offset_wr_cdc = 0;
	// bounds check inside DDRSS_get_wr_cdc_phy_offset
	if (DDRSS_get_wr_cdc_phy_offset(ch, cs, dq, &reg_offset_wr_cdc) < 0)
	{
		return -1;
	}
	
	prfs_x = DDRSS_Get_Freq_Index(NULL, freq_khz);

	DDR_PHY_hal_cfg_cdcext_slave_wr( reg_offset_wr_cdc, cs, coarse_delay, fine_delay, prfs_x );

	//FIXME: DDRSS_Periodic_Training_load_dit_struct_to_shrm updates the CDC values for periodic training as it uses them from a different place
	training_data_ptr = (training_data *)(&ddr->training_results.training_data);
	if (prfs_x == training_data_ptr->results.dit.dit_trac_prfs_ndx)
	{
		DDRSS_Periodic_Training_load_dit_struct_to_shrm(ddr, ch, 1<<cs);// takes a bit mask for CS
	}
	
	return 0;
}

int DDRSS_get_active_cdc_slave_wr(uint8 ch, uint8 cs, uint8 dq, uint8* const coarse_delay, uint8* const fine_delay) 
{
	uint32 coarse,fine;
	uint32 reg_offset_wr_cdc = 0;
	// bounds check inside DDRSS_get_wr_cdc_phy_offset
	if (DDRSS_get_wr_cdc_phy_offset(ch, cs, dq, &reg_offset_wr_cdc) < 0)
	{
		return -1;
	}
	
	DDR_PHY_hal_get_cdc_slave_wr_b( reg_offset_wr_cdc, cs, &coarse, &fine);
	
	//DDR_PHY_hal_get_cdc_slave_wr_b defined corse and fine as uint32* (pointers)
	*coarse_delay = coarse;
	*fine_delay = fine;

	return 0;
}

void DDRSS_cpy_cdcext_slave_wr_r0tor1(uint8 prfs_x, uint8 ch )
{
    uint8 coarse_delay;
    uint8 fine_delay;
    uint8 phy_x=0;
    uint32     reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    
    reg_offset_ddr_dq[0] = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1] = REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET;
    for (phy_x = 0; phy_x < NUM_DQ_PCH; phy_x++)
    {
        DDR_PHY_hal_get_cdcext_slave_wr( reg_offset_ddr_dq[phy_x],0, &coarse_delay,&fine_delay, prfs_x );
        DDR_PHY_hal_cfg_cdcext_slave_wr( reg_offset_ddr_dq[phy_x],1,  coarse_delay, fine_delay, prfs_x );
    }
}       

uint8 DDRSS_get_active_wr_vref(uint8 cs, uint8 ch, uint8 dq)
{
	dq = 0; // no single byte mode and not implemented in MC_MR_Read
	return vrange2vknob(MC_MR_Read(CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_14));
}

uint8 DDRSS_get_wr_vref(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz)
{
	uint8 fsp_op;
	dq = 0; // same Vref for both bytes
	training_data *training_data_ptr = (training_data *)(ddr->training_results.training_data);
	if ( (ch >= NUM_CH) || (cs >= NUM_CS) || (dq >= NUM_DQ_PCH) )
	{
		return -1;
	}

	fsp_op = (freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0;
	return vrange2vknob(training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][dq/*die for x8 */][fsp_op]);
}

int DDRSS_set_wr_vref(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint8 dq, uint32 freq_khz, uint8 vref_steps)
{
	uint8 fsp_op, vref_lp4;
	int ret = 0;
	training_data *training_data_ptr = (training_data *)(ddr->training_results.training_data);

	if ( (ch >= NUM_CH) || (cs >= NUM_CS) || (dq >= NUM_DQ_PCH) )
	{
		return -1;
	}
	
	vref_lp4 = vknob2vrange(vref_steps);
	if(vrange2vknob(vref_lp4) < vref_steps)
	{
		ret = -1;
	}
	
	fsp_op = (freq_khz > ODT_FSP_EN_FREQ_THRESHOLD) ? 1 : 0 ;
	dq = 0; // same Vref for both bytes
	training_data_ptr->results.wr_dram.wr_dram_vref[ch][cs][dq][fsp_op] = vref_lp4;	
	//TODO: could put internals of DDRSS_DRAM_Vref_Restore here and get rid of DDRSS_DRAM_Vref_Restore - to have every other client go through this generic set API
	DDRSS_DRAM_Vref_Restore(ddr, (1<<ch), (1<<cs) ); // apply modified Vref SW values //uses masks as arguments
	
	return ret;
}

void DDRSS_cpy_dram_wr_vref_r0tor1(BOOT_DDR_STRUCT *ddr,uint8 ch,uint8 fsp )
{  
    uint8  MR14_value  = 0;
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    MR14_value = MC_MR_Read  (     CH_1HOT(ch), DDR_CS0, JEDEC_MR_14);
    MC_MR_Write (ddr, CH_1HOT(ch), DDR_CS1, JEDEC_MR_14, MR14_value);
    training_data_ptr->results.wr_dram.wr_dram_vref[ch][1/*CS1*/][0/*Die0*/][fsp/*FSP0*/]=MR14_value;
}
uint8 DDRSS_get_wr_rank_update_en(uint8 prfs_x , uint8 ch)
{
    uint8 tmp;
    tmp=DDR_PHY_hal_get_wr_rank_update_en(REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET, prfs_x );
    return (tmp);
}

void DDRSS_store_all_wr_rank_update_en(uint8 ch, uint8 Str_array[8][NUM_DQ_PCH] )
{
	uint32 base_dq[NUM_DQ_PCH]={REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET,
	                            REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET};
    uint8  phy_x =0;
    uint8  prfs_x=0;
	for(prfs_x=0;prfs_x<8;prfs_x++)
	{
	    for(phy_x=0;phy_x<NUM_DQ_PCH;phy_x++)
	    {
	    	Str_array[prfs_x][phy_x]= DDR_PHY_hal_get_wr_rank_update_en(base_dq[phy_x], prfs_x);
	    }
	}
}

void  DDRSS_restore_all_wr_rank_update_en(uint8 ch, uint8 Str_array[8][NUM_DQ_PCH] )
{
	uint32 base_dq[NUM_DQ_PCH]={REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET,
	                            REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET};
    uint8  phy_x =0;
    uint8  prfs_x=0;
	for(prfs_x=0;prfs_x<8;prfs_x++)
	{
	    for(phy_x=0;phy_x<NUM_DQ_PCH;phy_x++)
	    {
	    	 DDR_PHY_hal_cfg_wr_rank_update_en(base_dq[phy_x], prfs_x,Str_array[prfs_x][phy_x]);
	    }
	}
}
void  DDRSS_set2one_all_wr_rank_update_en(uint8 ch )
{
	uint32 base_dq[NUM_DQ_PCH]={REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET,
	                            REG_OFFSET_DDR_PHY_CH(ch) + DQ1_DDR_PHY_OFFSET};
    uint8  phy_x =0;
    uint8  prfs_x=0;
	for(prfs_x=0;prfs_x<8;prfs_x++)
	{
	    for(phy_x=0;phy_x<NUM_DQ_PCH;phy_x++)
	    {
	    	 DDR_PHY_hal_cfg_wr_rank_update_en(base_dq[phy_x], prfs_x,1);
	    }
	}
}


