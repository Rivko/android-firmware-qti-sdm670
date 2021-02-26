/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_wr_tdqs2dq.c#3 $
$DateTime: 2018/02/15 02:48:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
-------   ---          --------------------------------------------------------

================================================================================*/

#include "ddrss.h"
#include "mc_common.h"
#include <string.h>

void Training_DDRSS_WR_TDQS2DQ(BOOT_DDR_STRUCT *ddr, 
                               DDR_CHANNEL channel, 
                               DDR_CHIPSELECT chip_select, 
                               uint32 clk_freq_khz, 
                               uint8 prfs_index, 
                               training_params_t *training_params_ptr,
                               uint8             *local_vars)
{
  uint8  ch                                      = 0;
  uint8  cs                                      = 0;
  uint8  byte_lane                               = 0;
  uint32 dq0_ddr_phy_base                        = 0;
  uint32 low_freq_clk_khz                        = 200000;
  uint32 temp_rd_cdc_value[NUM_DQ_PCH]           = {0};
  uint32 dit_count[NUM_CH][NUM_CS][NUM_DIE_PCH]  = {{{0}}};
  PHY_BUS dq                                     = DQ;

  uint8 num_ch           = ddr->shrm_ddr_ptr->misc.num_channel;
  channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
  //================================================================================================//
  // WR DQDQS Training for band0/1.
  //================================================================================================//
  //Check if the current clk frequency is equal to the max training clk frequency.
  if (clk_freq_khz == ddr->shrm_ddr_ptr->ddr_runtime.max_ddr_frequency)
  {

    // switch to low freq 
    ddr_external_set_clk_speed (low_freq_clk_khz);
    for (ch = 0; ch < num_ch; ch++)
    {
      dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
      
      if((channel >> ch) & 0x1)
      {
        for(cs = 0; cs < NUM_CS; cs++)
        {
          if((chip_select >> cs) & 0x1)
          {
            // Ensure more setup margin.
            for(byte_lane = 0; byte_lane < dq; byte_lane++)
            {
              // Store the original cdc value.
              temp_rd_cdc_value[byte_lane] = HWIO_INX((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG);
              
              DDR_PHY_hal_cfg_cdc_slave_rd((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), 
                                          LOW_SPEED_RD_COARSE_CDC_VALUE, 
                                          0x1,  /*coarse_cdc */ 
                                          0x1,  /*hp_mode */
                                          cs);   
            }
            
            if(cs == 0)
            {                            
              ddr_printf(DDR_NORMAL, "START: DIT runtime calculation on Ch: %u Rank: %u\n", ch, cs);
              DDRSS_DIT_Runtime(ddr, ch, cs, training_params_ptr, dit_count, low_freq_clk_khz);
              ddr_printf(DDR_NORMAL, "END: DIT runtime calculation  Ch: %u Rank: %u\n", ch, cs);
            
              ddr_printf(DDR_NORMAL, "START: WR training for band0/1 on Ch: %u Rank: %u\n", ch, cs);
              DDRSS_wr_dqdqs_lpddr4_tdqs2dq_cal (ddr, 
                                              ch, 
                                              cs, 
                                              dq,
                                              training_params_ptr, 
                                              low_freq_clk_khz);
              ddr_printf(DDR_NORMAL, "END: WR training for band0/1 on Ch: %u Rank: %u\n", ch, cs);
            }
            
            // Restore the original CDC value.
            for(byte_lane = 0; byte_lane < dq; byte_lane++)
            {
              HWIO_OUTX((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, temp_rd_cdc_value[byte_lane]);
            }
          }
        } // cs
      }
    } // ch

    //freq_switch back to max training freq
    ddr_external_set_clk_speed (clk_freq_khz);
  }
}

// -------------------------------------------------------------------------
// Low-Speed WR Training for band0/1.
// -------------------------------------------------------------------------
boolean DDRSS_wr_dqdqs_lpddr4_tdqs2dq_cal (BOOT_DDR_STRUCT *ddr, 
                                           uint8 ch, 
                                           uint8 cs, 
                                           PHY_BUS dq,
                                           training_params_t *training_params_ptr, 
                                           uint32 clk_freq_khz)
{
    uint16                       period = 0;
   // uint16               scaling_period = 0;
    uint8                     byte_lane = 0;
    uint32             dq0_ddr_phy_base = 0;
    uint32                  tdqs2dq_min = 0;
    uint8                      freq_inx = 0;
    uint8              coarse_cdc_value = 0;
    uint8                fine_cdc_value = 0;	
    uint8                 dq_half_cycle = 0;
    uint8                  device_width = 0;
    uint16            dit_runtime_count = 0; 
    
    uint16                    tdqs2dq[NUM_DQ_PCH] = {0}; 
    uint32  dit_count[NUM_CH][NUM_CS][NUM_DIE_PCH] = {{{0}}};
    
    // Training data structure pointer
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    
    // Base pointer to DDR_PHY
    dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch)+ DQ0_DDR_PHY_OFFSET;
    
    // Calculate T/4
    period = CONVERT_CYC_TO_PS / clk_freq_khz;
    //io_width read from DDR_STRUCT
    // 2 dies per ch/cs for x 8 mode and 1 die / ch/cs for x16 mode
    device_width = ddr->shrm_ddr_ptr->detected_ddr_device[ch].device_io_width[cs]; // find the device width per rank

    dit_runtime_count = training_data_ptr->results.dit.dit_runtime_count;

    DDRSS_DIT_Read (ddr, ch, cs, training_params_ptr, 0, dit_count, clk_freq_khz);

    if (dit_count[ch][cs][0] != 0)
    {
      tdqs2dq[0] = ((period * dit_runtime_count) / (2 * dit_count[ch][cs][0]));
      training_data_ptr->results.dit.tdqs2dq_in_ps[ch][cs][0] = tdqs2dq[0];
      if(device_width == DEVICE_WIDTH_IS_x8)
      {
        // TODO : Correct for x8 device
        tdqs2dq[1] = ((period * dit_runtime_count) / (2 * dit_count[ch][cs][1]));
        training_data_ptr->results.dit.tdqs2dq_in_ps[ch][cs][1] = tdqs2dq[1];

      }
      else
      {
        training_data_ptr->results.dit.tdqs2dq_in_ps[ch][cs][1] = tdqs2dq[0]; // if the device_width_is_0x16, copy tdqs2dq of byte0 to byte1
      }
    }
    #if DSF_WR_DQDQS_TRAINING_TDQS2DQ_CAL_LOG
    ddr_printf(DDR_NORMAL, "tDQSDQ for CH = %d, CS = %d, die0 = %d \n",ch,cs,tdqs2dq[0]);
    if(device_width == DEVICE_WIDTH_IS_x8)
    {
        ddr_printf(DDR_NORMAL, "tDQSDQ for CH = %d, CS = %d, die1 = %d \n",ch,cs,tdqs2dq[1]);
    }
    #endif   
    // Choose the min value of the two dies.
    tdqs2dq_min = (device_width == DEVICE_WIDTH_IS_x8) ? 
                  ((tdqs2dq[0] < tdqs2dq[1])? tdqs2dq[0] : tdqs2dq[1]) : 
                   tdqs2dq[0] ;
    
    
    if(tdqs2dq_min > LOW_SPEED_HALF_CYC_CONV_THRESHOLD)
    {
        dq_half_cycle = 1;
        coarse_cdc_value = 0;
        fine_cdc_value = 0;		
        #if DSF_WR_DQDQS_TRAINING_TDQS2DQ_CAL_LOG
        ddr_printf(DDR_NORMAL, "  WR TDQS2DQ Channel %d Rank %d = HALF Cycle\n",ch,cs);
        #endif
    }
    else 
    {
        dq_half_cycle = 0;
        coarse_cdc_value = LOW_SPEED_NO_HALF_CYC_COARSE_CDC_VALUE;
        fine_cdc_value = LOW_SPEED_NO_HALF_CYC_FINE_CDC_VALUE;
        #if DSF_WR_DQDQS_TRAINING_TDQS2DQ_CAL_LOG
        ddr_printf(DDR_NORMAL, "  WR TDQS2DQ Channel %d Rank %d Coarse = %d Fine = %d\n",ch,cs,coarse_cdc_value, fine_cdc_value);
        #endif        
    }
        
    for (byte_lane = 0; byte_lane < dq; byte_lane++) 
    {        
        for(freq_inx = 0; freq_inx <= 0; freq_inx++) // For band0 only
        {
    
            // Update Registers.
            DDR_PHY_hal_cfg_cdcext_slave_wr ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                cs,
                                                coarse_cdc_value,
                                                fine_cdc_value,
                                                freq_inx);
        
            DDR_PHY_hal_cfg_wrext_ctl_update ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                freq_inx,
                                                cs,
                                                dq_half_cycle);
                                                
            if(ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect == DDR_CS_BOTH)
            {
                // Write the rank1 values into the CSRs.
                DDR_PHY_hal_cfg_cdcext_slave_wr ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                    1,
                                                    coarse_cdc_value,
                                                    fine_cdc_value,
                                                    freq_inx);
            
                DDR_PHY_hal_cfg_wrext_ctl_update ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)),
                                                    freq_inx,
                                                    1,
                                                    dq_half_cycle);  
                
            }
        } //freq_inx.
    } //byte_lane.   
    return TRUE;    
}
