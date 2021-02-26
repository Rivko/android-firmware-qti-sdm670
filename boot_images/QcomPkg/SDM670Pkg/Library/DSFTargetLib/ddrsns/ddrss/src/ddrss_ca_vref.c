/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_ca_vref.c#3 $
$DateTime: 2018/02/15 02:48:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------

================================================================================*/

#include "ddrss.h"
#include <string.h>




uint8 CBT_active;

void Training_DDRSS_CA_VREF_LPDDR4(BOOT_DDR_STRUCT *ddr, 
                                   DDR_CHANNEL channel, 
                                   DDR_CHIPSELECT chip_select, 
                                   uint32 clk_freq_khz, 
                                   uint8 prfs_index, 
                                   training_params_t *training_params_ptr,
                                   uint8             *local_vars
                                   ) 
{
    uint8  ch        = 0;
    uint8  cs        = 0;
    //uint8 freq_eq_max_training = ddr->shrm_ddr_ptr->misc.max_ddr_frequency == clk_freq_khz;
    uint8 num_ch               = ddr->shrm_ddr_ptr->misc.num_channel;
    channel                    = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);    
    boolean ca_vref_fail_flag  = FALSE;

    for (ch = 0; ch < num_ch; ch++)
    {
      if ((channel >> ch) & 0x1)
      {
          for (cs = 0; cs < NUM_CS; cs++)
          {
            if ((chip_select >> cs) & 0x1)
            {
              ddr_printf(DDR_NORMAL, "START: CA training on Ch: %u Rank: %u\n", ch, cs);

              ca_vref_fail_flag = DDRSS_Vrefonly_lpddr4(ddr, 
                                                        ch, 
                                                        cs, 
                                                        training_params_ptr,
                                                        (ddrss_ca_vref_local_vars *)local_vars,
                                                        clk_freq_khz
                                                        );
            }
          }
      }
    }
}  
   
//================================================================================================//
// DDR PHY CA VREF training
//================================================================================================//
boolean DDRSS_Vrefonly_lpddr4_mode1(BOOT_DDR_STRUCT *ddr,
                                    uint8            ch,  
                                    uint8            cs, 
                                    PHY_BUS          ca,
                                    uint8            prfs_index,
                                    training_params_t *training_params_ptr,
                                    ddrss_ca_vref_local_vars *local_vars,
                                    uint32           clk_freq_khz
                                   )
{
    uint8               loop_cnt = 0;
    uint8                phy_inx = 0;
    uint8          pattern_index = 0;
    uint8             vref_value = 0;
    uint8              vref_data = 0;
    uint8             cs_pattern = 0;
    uint16        expect_pattern = 0;   
  //uint32        reg_offset_dpe = 0;
    uint8               min_vref = 0;
    uint8               max_vref = 0;
    uint8               mid_vref = 0;
    uint32 reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
  //  uint32 reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint16     feedback_data[NUM_CA_PCH] = {0};
    uint8      range_vref[NUM_CS][NUM_CA_PCH] = {{0}};
  
  //reg_offset_dpe         = REG_OFFSET_DPE(ch);
    reg_offset_ddr_dq[0]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ1_DDR_PHY_OFFSET;

    
     CBT_active=1;
    //Vref Sweeping loop
    for (vref_value  = training_params_ptr->ca_vref.vref_start_value; //30
		 vref_value < training_params_ptr->ca_vref.vref_max_value; //80
		 vref_value ++) 
    {
        // Add MR12_vref_range_bit as the bit 6
		vref_data =vknob2vrange(vref_value);
        // failures counters reset to 0
        local_vars->vref_fail_count_table[0][vref_value]=0; 
        local_vars->vref_fail_count_table[1][vref_value]=0;         


        // Low speed
        ddr_external_set_clk_speed (200000);

        //CA Entry
        //BIMC_CA_Training_Entry_with_Vref( ch, CS_1HOT(cs),  vref_data);
        
        // PHY CA0 training entry
        DDR_PHY_hal_cfg_ca_vref_dq_in_entry(reg_offset_ddr_dq[0]);
        DDR_PHY_hal_cfg_ca_vref_dq_in_entry(reg_offset_ddr_dq[1]);

        // High speed

        ddr_external_set_clk_speed (clk_freq_khz);

        // Different CA patterns are sent out to enchance training result
        for (pattern_index = 0; pattern_index < CA_PATTERN_NUM; pattern_index++) 
        { 
            //BIMC_write_CA_Pattern (reg_offset_dpe,  ca_training_pattern_lpddr4[pattern_index]);
            cs_pattern     = ca_training_pattern_lpddr4[pattern_index][1];
            expect_pattern = DDRSS_dq_remapping(cs_pattern & 0x3F);     // Only lower 6-bit is valid;
            // For each pattern, each point of histogram will be tested for 16 times,
            for (loop_cnt = training_params_ptr->ca_vref.max_loopcnt; loop_cnt > 0; loop_cnt --) 
            {
                // The MC sends the VREF setting through DQ PHY0 to DRAM on DQ[6:0]
                //BIMC_Send_CA_Pattern (ch, cs);
                feedback_data[0] = DDR_PHY_hal_sta_ca_vref (reg_offset_ddr_dq[0]); 
                //feedback_data[1] = DDR_PHY_hal_sta_ca_vref (reg_offset_ddr_dq[1]); 
                
                ddr_printf(DDR_NORMAL, "vref=%2d vref_data=%2d patn=%2d, lp=%2d,exp= %2x,fdbk0 =%2x  fdbk1=%2x\n",
                vref_value,vref_data,pattern_index, loop_cnt,
                expect_pattern, feedback_data[0] /*, feedback_data[1]*/ );
          
                if (feedback_data[0] != expect_pattern)  { local_vars->vref_fail_count_table[0][vref_value]++; }
                //if (feedback_data[1] != expect_pattern)  { local_vars->vref_fail_count_table[1][vref_value]++; }
                // If CA passes, ca_pattern is required to invert
                if (feedback_data[0] == expect_pattern /*|| feedback_data[1] == expect_pattern*/) 
                {
                    cs_pattern = ~cs_pattern;   //Invert CS_Pattern if passes  
                    expect_pattern = DDRSS_dq_remapping (cs_pattern & 0x3F);   
                    //HWIO_OUTX (reg_offset_dpe, DPE_CA_TRAIN_CS, cs_pattern);
                }
            }
        }
               
        //BIMC_CA_Training_Exit_CKE_ON (ch, CS_1HOT(cs));
        //BIMC_CA_Training_Exit_MR     (ch, CS_1HOT(cs), 1/*FSP_OP*/);
        //DDR_PHY_exit();
        //Exit Ca Training 
        DDR_PHY_hal_cfg_ca_vref_dq_exit (reg_offset_ddr_dq[0]);
        DDR_PHY_hal_cfg_ca_vref_dq_exit (reg_offset_ddr_dq[1]);
    }//End of Vref loop
    
    CBT_active=0;
 
    ddr_external_set_clk_speed (clk_freq_khz);

    // Process the histogram of both CA0 and CA1 and save to data structure
    for (phy_inx = 0; phy_inx < ca; phy_inx ++)     
    {
        DDRSS_passing_region_scan( &min_vref, &max_vref,  local_vars->vref_fail_count_table[phy_inx],
								   training_params_ptr->ca_vref.vref_start_value, training_params_ptr->ca_vref.vref_max_value);
        mid_vref = ( min_vref + max_vref ) / 2; 
        if (min_vref==max_vref)
        {
            return(FALSE);
        }
		range_vref[cs][phy_inx]=vknob2vrange(mid_vref);
        
    }
    //copy rank01 to rank 1   
    range_vref[1][0] = range_vref[0][0];

    DDRSS_MR_Write_on_die (ddr, ch, 0, 12, 
                            range_vref[0][0],0);

    DDRSS_MR_Write_on_die (ddr, ch, 1, 12, 
                           range_vref[1][0],0);
  return(TRUE);
}

boolean DDRSS_Vrefonly_lpddr4(BOOT_DDR_STRUCT *ddr,
                           uint8            ch,  
                           uint8            cs, 
                           training_params_t        *training_params_ptr,
                           ddrss_ca_vref_local_vars *local_vars,
                           uint32           clk_freq_khz
                           )
{
    uint8               loop_cnt = 0;
    //uint8                phy_inx = 0;
    uint8          pattern_index = 0;
    uint8             vref_value = 0;
    uint8              vref_data = 0;
    uint8             cs_pattern = 0;
    uint16        expect_pattern = 0;   
    uint32   reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint32   reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    uint8               min_vref = 0;
    uint8               max_vref = 0;
    uint8               mid_vref = 0;
    uint16 feedback_data[NUM_CA_PCH] = {0};
    uint8  range_vref[NUM_CS][NUM_CA_PCH] = {{0}};
    
  //reg_offset_dpe        = REG_OFFSET_DPE(ch);
    reg_offset_ddr_dq[0]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ1_DDR_PHY_OFFSET;
    reg_offset_ddr_ca[0]   = REG_OFFSET_DDR_PHY_CH (ch) + CA0_DDR_PHY_OFFSET;

    // PHY CA0 training entry
    DDR_PHY_hal_cfg_ca_vref_dq_out_entry (reg_offset_ddr_dq[0]);
    DDR_PHY_hal_cfg_ca_vref_dq_in_entry  (reg_offset_ddr_dq[1]);
    //DDRSS_BIST_CA_Training_entry (ch);
  
    //BIMC CA training Entry
    //BIMC_CA_Training_Entry (ch, CS_1HOT(cs), 1/*FSP_OP*/);
    
    for (vref_value  = training_params_ptr->ca_vref.vref_start_value; //30
		 vref_value < training_params_ptr->ca_vref.vref_max_value; //80
		 vref_value ++) 
    {
        // Add MR12_vref_range_bit as the bit 6
		vref_data =vknob2vrange(vref_value);

        // Send Vref to dq0
        DDR_PHY_hal_BIST_cfg_Vref  ( reg_offset_ddr_dq[0] , vref_data );
        DDR_PHY_hal_Send_BIST_START( reg_offset_ddr_dq[0] );

        // failures counters reset to 0
        local_vars->vref_fail_count_table[0][vref_value]=0; 
        // Different CA patterns are sent out to enchance training result
        for (pattern_index = 0; pattern_index < CA_PATTERN_NUM; pattern_index++) 
        { 
		  //config BIST to send the pattern throu CA
		  //DDRSS_BIST_cfg_CA_Pattern(ch);
		  DDR_PHY_hal_BIST_cfg_CA_Pattern_ca(reg_offset_ddr_ca[0],ca_training_pattern_lpddr4[pattern_index]);

          cs_pattern     = ca_training_pattern_lpddr4[pattern_index][1];
          expect_pattern = DDRSS_dq_remapping(cs_pattern & 0x3F);     // Only lower 6-bit is valid;
          // For each pattern, each point of histogram will be tested for 16 times,
          for (loop_cnt = training_params_ptr->ca_vref.max_loopcnt; loop_cnt > 0; loop_cnt --) 
          {
            // The MC sends the VREF setting through DQ PHY0 to DRAM on DQ[6:0]
            DDR_PHY_hal_Send_BIST_START( reg_offset_ddr_ca[0] );
            feedback_data[0] = DDR_PHY_hal_sta_ca_vref (reg_offset_ddr_dq[1]); 
            if (feedback_data[0] != expect_pattern) { local_vars->vref_fail_count_table[0][vref_value]++; }
            if (feedback_data[0] == expect_pattern)
            {
              cs_pattern = ~cs_pattern;   //Invert CS_Pattern if passes  
              expect_pattern = DDRSS_dq_remapping (cs_pattern & 0x3F);   
              //HWIO_OUTX (reg_offset_dpe, DPE_CA_TRAIN_CS, cs_pattern);
            }
          }//loopcount
        }//pattern
    }//End of Vref loop

        DDRSS_passing_region_scan( &min_vref, &max_vref,  local_vars->vref_fail_count_table[0],
								   training_params_ptr->ca_vref.vref_start_value, training_params_ptr->ca_vref.vref_max_value);
        mid_vref = ( min_vref + max_vref ) >>1; //divide by two
		range_vref[cs][0] =vknob2vrange(mid_vref);
    
    //copy rank01 to rank 1   
    range_vref[1][0] = range_vref[0][0];
    DDR_PHY_hal_BIST_cfg_Vref  ( reg_offset_ddr_dq[0] , vref_data );
    DDR_PHY_hal_Send_BIST_START( reg_offset_ddr_dq[0] );
    // DDRSS_MR_Write_on_die (ddr, ch, 0, 12, range_vref[0][0],0);
    // DDRSS_MR_Write_on_die (ddr, ch, 1, 12, range_vref[1][0],0);
    return(TRUE);
}


void DDRSS_BIST_CA_Training_entry (uint8 ch)
{
    uint32   reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint32   reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    
    reg_offset_ddr_dq[0]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ1_DDR_PHY_OFFSET;
    reg_offset_ddr_ca[0]   = REG_OFFSET_DDR_PHY_CH (ch) + CA0_DDR_PHY_OFFSET;
	
    //DDR_PHY_hal_BIST_cfg_CA_Pattern_ca (reg_offset_ddr_ca[0] ,  ca_training_pattern);
    DDR_PHY_hal_BIST_cfg_CA_Pattern_b0 (reg_offset_ddr_dq[0] );
    DDR_PHY_hal_BIST_cfg_CA_Pattern_b1 (reg_offset_ddr_dq[1] );
}



void  DDRSS_BIST_cfg_CA_Pattern (uint8 ch, uint8  ca_training_pattern[3])
{
    uint32   reg_offset_ddr_ca[NUM_CA_PCH] = {0};
    uint32   reg_offset_ddr_dq[NUM_DQ_PCH] = {0};
    
    reg_offset_ddr_dq[0]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ0_DDR_PHY_OFFSET;
    reg_offset_ddr_dq[1]   = REG_OFFSET_DDR_PHY_CH (ch) + DQ1_DDR_PHY_OFFSET;
    reg_offset_ddr_ca[0]   = REG_OFFSET_DDR_PHY_CH (ch) + CA0_DDR_PHY_OFFSET;
	
    DDR_PHY_hal_BIST_cfg_CA_Pattern_ca (reg_offset_ddr_ca[0] ,  ca_training_pattern);
    DDR_PHY_hal_BIST_cfg_CA_Pattern_b0 (reg_offset_ddr_dq[0] );
    DDR_PHY_hal_BIST_cfg_CA_Pattern_b1 (reg_offset_ddr_dq[1] );
}


//void DDRSS_Send_CA_Pattern(uint8 ch, uint8 cs)
//{
//    uint32 ca0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;       
//
//	HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
//	HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
//
//}