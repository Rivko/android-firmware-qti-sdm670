/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_rcw.c#6 $
$DateTime: 2019/08/01 06:25:20 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------

================================================================================*/

/***************************************************************************************************
   Description: The DDRSS RCW routine performs training to determine the TDQS2CK delay. 
   The training generates a DQS gating signal in the PHY to filter out any unwanted DQS
   gliches that occur if ODT is enabled for LPDDR4. The training finds a proper 
   tDQS2CK (RCW) delay for each data byte.
***************************************************************************************************/
#include "ddrss.h"
#include "ddrss_training.h"
#include "mc.h"

/***************************************************************************************************
This is the main RCW function that calls the other helper functions. RCW training is performed before
any other training at 1017MHz. The RCW training is done only once when training frequency for all the 
trainings is max_frequency. This function checks if the frequency is max frequency, selects the channel
and the rank for which the RCW training is to be done
***************************************************************************************************/
void Training_DDRSS_RCW(BOOT_DDR_STRUCT *ddr, 
                        DDR_CHANNEL channel, 
                        DDR_CHIPSELECT chip_select, 
                        uint32 freq, 
                        uint8 prfs_index, 
                        training_params_t *training_params_ptr,
                        uint8             *local_vars
                        ) 
{
    uint8 ch				= 0;
	uint8 cs				= 0;
	PHY_BUS dq              = DQ;
	uint8 rcw_start_clk_idx	= 0;
    uint8 num_ch			= ddr->shrm_ddr_ptr->misc.num_channel;
//	uint8 num_cs			= ddr->shrm_ddr_ptr.detected_ddr_device.populated_chipselect;
	uint8 freq_index		= 0;
  
	channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);

	//freq = 200000;
	
	//Determine the rcw clock frequency index from the clock plan
    for(rcw_start_clk_idx = 0; rcw_start_clk_idx < MAX_NUM_CLOCK_PLAN-1; rcw_start_clk_idx++)
    {
      if (RCW_THRESHOLD_FREQ < ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[rcw_start_clk_idx].clk_freq_in_khz) 
	  {
	      rcw_start_clk_idx--;
		  break;
	  }
    }


    
    // Initialize RCW params before starting RCW training 
	for(freq_index = rcw_start_clk_idx ; freq_index < MAX_NUM_CLOCK_PLAN; freq_index++ )
	{
		ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][0] = 0x100000;
		ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][1] = 0x100000;
	}	

    // Set READ_IE_EXT_PRE_VAL and OPCODE_READ_LEAD_TIME_VAL
    for (ch = 0; ch < num_ch; ch++)
    {
      if ((channel >> ch) & 0x1)
      {
        DDR_PHY_RCW_pre_training_init (ch,  dq);
      }
    }	
    for (ch = 0; ch < num_ch; ch++)
    {
      if ((channel >> ch) & 0x1)
      {
          for(cs = 0; cs < NUM_CS; cs++)
          {
              if((chip_select >> cs) & 0x1)
              {  
               //   ddr_printf(DDR_NORMAL, "START: RCW training on Ch: %u Rank: %u\n", ch, cs);
                  DDRSS_RCW_slave (ddr, ch, cs, chip_select, dq, training_params_ptr, rcw_start_clk_idx, freq,(ddrss_rcw_local_vars*) local_vars);
               //   ddr_printf(DDR_NORMAL, "END: RCW training on Ch: %u Rank: %u\n", ch, cs);
              }
          }
      }
    }
	
	// Adding Four cycles margin for 2 + 2 (Read preamble) for palcment of RD_EN to RDDQS in DDRSS_struct_op code
	for(freq_index = rcw_start_clk_idx ; freq_index < MAX_NUM_CLOCK_PLAN; freq_index++ )
	{
		ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][0] = ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][0] + 0x400000;
		ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][1] = ddr->shrm_ddr_ptr->ddr_runtime.mc_rd_opcode_offset[freq_index][1] + 0x400000;
	}	

    //Frequency switch to 200MHz
    // Perform a dummy MR read to workaround read hang due to RCW training
	//ddr_external_set_clk_speed (ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[rcw_start_clk_idx].clk_freq_in_khz);
		
}
/*************************************************************************************************************
This function sets PHY and MC registers for RCW training mode.
*************************************************************************************************************/
void  DDRSS_RCW_slave (BOOT_DDR_STRUCT *ddr,
					uint8 ch,
					uint8 cs,
					DDR_CHIPSELECT populated_chipselect,
					PHY_BUS dq,
					training_params_t *training_params_ptr,
					uint8 rcw_start_clk_idx,
					uint32 current_freq,
          ddrss_rcw_local_vars *local_vars) 
{
	
    uint8   all_found        = 0;
    //uint8   num_half_cycle   = 0;
    uint8   num_cycle   = 0;
    uint8   loop_cnt         = 0;
    uint8   max_coarse_delay = 0;
    uint8   max_fine_delay   = 0;
    uint8   rcw_training_reset[NUM_DQ_PCH] = {1,1};
    uint8   byte_done[NUM_DQ_PCH] = {0,0};
    uint8   byte_not_done[NUM_DQ_PCH] = {1,1}; 
    uint8   byte_lane        = 0;
    uint32  dq0_ddr_phy_base = 0;
    uint32  pad_cntl_3_reg[NUM_DQ_PCH]   = {0};
    uint32  pad_cntl_4_reg[NUM_DQ_PCH]   = {0};
    uint8  bit               = 0;
	//   DDR_CHIPSELECT   populated_chipselect = DDR_CS_NONE;

	//    populated_chipselect = ddr->shrm_ddr_ptr.detected_ddr_device[ch].populated_chipselect;
    loop_cnt             = training_params_ptr->rcw.max_loopcnt;
    max_coarse_delay     = training_params_ptr->rcw.max_coarse_cdc;
    max_fine_delay       = training_params_ptr->rcw.max_fine_cdc;   

	// Set DDR Base address registers
     dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET; 

    // Frequency switch to RCW frequency.
	ddr_external_set_clk_speed (ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[rcw_start_clk_idx].clk_freq_in_khz);

    for(byte_lane = 0; byte_lane < dq; byte_lane++)
     {
        // Set Passivation based on one time settings to temp. params
        pad_cntl_3_reg[byte_lane] = HWIO_INX ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_3_CFG);
        pad_cntl_4_reg[byte_lane] = HWIO_INX ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_4_CFG);
        //Disable passivation overide PAD_CNTL3/4_CFG bit25 = 0
        HWIO_OUTXF  ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_3_CFG, LEFT_PAD_CNTL_BIT_25_RFU, 0);
        HWIO_OUTXF  ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_4_CFG, RIGHT_PAD_CNTL_BIT_25_RFU, 0); 

		// Set local Vrefs to mid-value.
        for(bit = 0; bit < PINS_PER_PHY; bit++)
        {
            DDR_PHY_hal_cfg_local_vref  ( (dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), cs, bit, LOCAL_VREF_MID_VALUE);
        }       
     }

	//Configure CABO_SHKE_CMD_SET_CFG_10 to issue CMD_MPC_RDDQCAL
	MC_Cmd_Set_Cfg(ch, CS_1HOT(cs), 0xA, CMD_MPC_RDDQCAL, 0x0, 0x0);
	MC_Cmd_Stagger_Cfg(ch, 0xA, 0x0, 0x0, 0x0);
		
	//Fehya//	// Select the rank to be trained 
//	for(byte_lane = 0; byte_lane < dq; byte_lane++)
//	{
//		HWIO_OUTXF ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_CMCDCRCW_TOP_CFG, DLY_SEL, cs);		
//	}
	
	//Enter Read Preamble Training mode.
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (0xC2 | MR13_BASE)); //enable RRO and disable DM
	
	//Enable PHY for RCW training ,and set register
	rcw_enable_disable (byte_not_done, ch, dq);

    for (num_cycle = 0; num_cycle <= training_params_ptr->rcw.max_num_cycle; num_cycle++)  //No half cycles and move only full cycle
    {
        if (all_found) 
			break;
		else
        {
            //Set half cycle/full cycle delays
             set_rcw_full_cycle (byte_not_done, num_cycle, ch, dq);            
            //Initialize coarse and fine delay to zero
            set_rcw_cdc_delay(0, RCW_COARSE_DELAY, HP_MODE, ch,  byte_not_done, dq);// As rank is not enabled at 1017MHz use only rank0 bitfield of the CDC for both the CDCs
            set_rcw_cdc_delay(0, RCW_FINE_DELAY, HP_MODE, ch,  byte_not_done, dq);   
            //STEP THROUGH ALL COARSE CDC DELAYS, coarse delay swept from zero. when a RCW rising edge is detected, we backup coarse cdc by 1 to perform fine training
           cdc_delay_search (loop_cnt, max_coarse_delay, RCW_COARSE_DELAY,  &all_found, ch, cs, byte_done, byte_not_done, dq, local_vars);
        }
    }
if(! all_found)
{
#if DSF_RCW_TRAINING_LOG
	ddr_printf (DDR_NORMAL,"Channel %d and rank %d is a fail\n", ch, cs); 
#endif
}

	// reset the byte_done[] values to zero and byte_not_done[] = 1 for fine training usage.
	for(byte_lane = 0 ; byte_lane < dq; byte_lane++)
	{
		byte_not_done[byte_lane] = 1;
		byte_done[byte_lane] = 0;
	}

	//Set CDC delay back to zero
	cdc_delay_search (loop_cnt, max_fine_delay, RCW_FINE_DELAY, &all_found, ch, cs,  byte_done, byte_not_done,   dq, local_vars) ;
	//read the trained value in to the local_vars
	//read_tDQSCK( ch, cs, dq, local_vars);
	
	if ((cs == 1) || (populated_chipselect == DDR_CS0))
	{
		//Calculate the tdqsck for both the ranks and choose the best tdqsck amongst both. Scale this tdqsck in number of cycles for all the above PRFS frequencies
		DDRSS_scale_rcw_delay_write_ext_reg(ddr, ch, populated_chipselect, dq, training_params_ptr, rcw_start_clk_idx, local_vars);
	}

	for(byte_lane = 0 ; byte_lane < dq; byte_lane++)
    {   
        //disable RCW training
		rcw_training_reset[byte_lane] = 0;
		byte_not_done[byte_lane] = 1;
        // Set Passivation based on one time settings 
        HWIO_OUTX  ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_3_CFG, pad_cntl_3_reg[byte_lane]);
        HWIO_OUTX  ((dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET)), DDR_PHY_DDRPHY_PAD_CNTL_4_CFG, pad_cntl_4_reg[byte_lane]); 
    }    

	//clear half/full cycle and coarse/fine cdc delays to zero 
	set_rcw_full_cycle (byte_not_done, 0, ch, dq); // set number of cycles to 0
	set_rcw_cdc_delay(0, RCW_COARSE_DELAY, HP_MODE, ch, byte_not_done, dq); // set coarse cdc delay to 0
    set_rcw_cdc_delay(0, RCW_FINE_DELAY, HP_MODE, ch,  byte_not_done, dq);  //Set fine cdc delay to 0

	//Disable RCW training
	rcw_enable_disable(rcw_training_reset, ch, dq); 
	
	//Exit Read Preamble training. MR13[1]:1 for FSP1
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_13, (0xC0 | MR13_BASE)); //enable RRO and disable DM
	
	// Perform a dummy MR read to workaround read hang due to RCW training
	ddr_external_set_clk_speed (current_freq);
	//  ddr_external_set_clk_speed (ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[rcw_start_clk_idx].clk_freq_in_khz);

}


void rcw_enable_disable (uint8 byte_not_done[NUM_DQ_PCH], uint8 ch, PHY_BUS dq)
{
	uint8  byte_lane         = 0;
	uint32 dq0_ddr_phy_base  = 0;
	RCW_EN_DIS rcw_setting;
	dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
	for (byte_lane = 0; byte_lane < dq; byte_lane++)
	{
		if (byte_not_done[byte_lane])
			//Enable RCW training
			rcw_setting = RCW_ENA;
		else
			//Disable or Exit RCW training
			rcw_setting = RCW_DIS;
			
      DDR_PHY_hal_cfg_rcw(dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), rcw_setting);
   }
	
	
}
//-------------------------------------------------------------------
//Set half/ full cycles
//-------------------------------------------------------------------
// byte_not_done : 1: Set half/full cycle delays, 0: dont set any half or full cycles
// num_cycle	 : 1: half cycle, 2: full cycle , 3: one and half cycle

void set_rcw_full_cycle (uint8 byte_not_done[NUM_DQ_PCH], uint8 num_cycle, uint8 ch,  PHY_BUS dq)
{	
	uint8  byte_lane   =  0;
	uint32 MC_DTC_base =  0;;   // MC CH base address	
	MC_DTC_base  = REG_OFFSET_MC_DTC(ch);
    
	for (byte_lane = 0; byte_lane < dq; byte_lane++) 
    {
		if (byte_not_done[byte_lane])		
		{
			DDR_PHY_hal_cfg_full_rcw(MC_DTC_base, num_cycle);		
			MC_Timing_Recalc_Update (ch); 
		}
	}
}

//-------------------------------------------------------------------
// SET RCW CDC delay
//-----------------------------------------------------------------

void set_rcw_cdc_delay ( uint32 rcw_delay,  uint16 coarse_fine, uint16 mode, uint8 ch,  uint8 byte_not_done[NUM_DQ_PCH], PHY_BUS dq)
{
   uint8 byte_lane = 0;
   uint32 dq0_ddr_phy_base;   // DDR PHY DQ0 base address

   dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
#if DSF_RCW_TRAINING_LOG
	//ddr_printf (DDR_NORMAL,"SET_RCW_DELAY: delay = %d, coarse_fine = %d\n",rcw_delay, coarse_fine);
#endif
   for (byte_lane = 0; byte_lane < dq; byte_lane++) {
      if (byte_not_done[byte_lane]) // if enable[byte_lane] = 1 then set the 
      DDR_PHY_hal_cfg_cdc_slave_rcw(dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET), rcw_delay, coarse_fine, mode);
   }
}


//-------------------------------------------------------------------
// RCW CDC COARSE AND FINE DELAY SEARCH
//-------------------------------------------------------------------
// loop_cnt         : number fo time to run read test
// coarse_fine_sel  : select to run coarse delay sweep or fine delay sweep
// all_found        : all_byte_done = 1, search finished, 0: not finish
// ch               : pass from top, 1 or 0

void cdc_delay_search (uint8 loop_cnt, uint8 max_delay, uint8 coarse_fine_sel,  uint8 *all_found, uint8 ch, uint8 cs, uint8 *byte_done, uint8 *byte_not_done, PHY_BUS dq, ddrss_rcw_local_vars *local_vars) 
{

   uint8    byte_zero_cnt[NUM_DQ_PCH]   = {0,0};
   uint8     byte_one_cnt[NUM_DQ_PCH]   = {0,0};
   uint8    byte_indx, lp, indx;
   uint8    delay ;

   for (delay = 0; delay < max_delay; delay++)  
   {

   //---------------------------------------------------------------------
   //  Set CDC coarse delay
   //---------------------------------------------------------------------
#if DSF_RCW_TRAINING_LOG
   //ddr_printf ("CDC_DELAY_SEARCH:  FORLOOP delay =   %d, max_delay = %d\n ", delay, max_delay);
#endif
  	 set_rcw_cdc_delay(delay, coarse_fine_sel, HP_MODE, ch, byte_not_done, dq);

   	//clear counters
		for (indx = 0; indx < dq ; indx++) 
	 	{
    		byte_one_cnt[indx] = 0;
    		byte_zero_cnt[indx] = 0;
   	 	}

   	 	for (lp = 0; lp < loop_cnt; lp++)  
	 	{

			//Set the trigger for HW to execute MPC command that has been pre-programmed and Poll to be done
			MC_Cmd_Trigger (ch, 0x0400); // Set the trigger on  CABO_SHKE_CMD_SET_CFG_9,10,11
			//***MC_MR_Read(CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_5);

     	    //Read RCW_MON status
    		read_rcw_status (byte_one_cnt, byte_zero_cnt, dq, ch);
   	 	}

        //Check read result
     	for (byte_indx = 0; byte_indx < dq; byte_indx++) 
		{
			if((byte_one_cnt[byte_indx] > byte_zero_cnt[byte_indx]) && (byte_not_done[byte_indx] == 1)) // should it be byte_one_cnt[byte_indx] == loop_cnt??
			{
				byte_done[byte_indx] = 1;
				byte_not_done[byte_indx] = 0;
				if((coarse_fine_sel == RCW_COARSE_DELAY) && (delay > 0))// if training type is coarse training 
				{
					set_rcw_cdc_delay(delay-1, coarse_fine_sel, HP_MODE, ch, byte_not_done, dq);// Set coarse cdc = delay -1. This will be the starting
																								 // point for fine training
				}
                read_tDQSCK( ch, cs, byte_indx, local_vars, coarse_fine_sel);
			}
   		 }
		 if ((byte_done[0] == 1 ) && (byte_done[1] == 1 ))
         {
            *all_found = 1;
            break;
         }

   }  //max coarse/fine delay for loop
}

//-------------------------------------------------------------------
// READ RCW STATUS
//-------------------------------------------------------------------
void  read_rcw_status (uint8 *byte_one_cnt, uint8 *byte_zero_cnt, PHY_BUS dq, uint8 ch)
{

   uint8 byte_indx = 0;
   uint8 pass[NUM_DQ_PCH] = {0,0};
   uint32 dq0_ddr_phy_base;   // DDR PHY DQ0 base address

   dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

 for (byte_indx =0; byte_indx < dq; byte_indx++) {

      DDR_PHY_rd_mon_status(dq0_ddr_phy_base + (byte_indx * DDR_PHY_OFFSET)  ,
                         &pass[byte_indx]);
	  (pass[byte_indx] == 1)?
		                  byte_one_cnt[byte_indx]++ :
		                  byte_zero_cnt[byte_indx]++ ; 
   }
}

void read_tDQSCK( uint8 ch, uint8 cs, uint8 byte_lane, ddrss_rcw_local_vars *local_vars, uint8 coarse_fine_sel)
{
	
	uint32 dq0_ddr_phy_base  =  0;   // DDR PHY DQ0 base address
	uint32 MC_DTC_base       =  0 ;   // MC CH base address
	
	MC_DTC_base  = REG_OFFSET_MC_DTC(ch);
	dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

    if  (coarse_fine_sel == RCW_COARSE_DELAY) 
    {
        // Read the DQ PHY RCW registers
        local_vars->rcw_coarse_cdc[ch][cs][byte_lane]  = DDR_PHY_hal_sta_rcw_coarse(dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),0);// Read from only Rank0 fields 
        local_vars->rcw_num_cycles[ch][cs][byte_lane] = DDR_PHY_hal_sta_rcw_num_cycle(MC_DTC_base);
    }
    else
    {
         local_vars->rcw_fine_cdc[ch][cs][byte_lane]    = DDR_PHY_hal_sta_rcw_fine(dq0_ddr_phy_base   + (byte_lane * DDR_PHY_OFFSET),0);// Read from only rank0 fields
    }
}

void DDRSS_scale_rcw_delay_write_ext_reg (BOOT_DDR_STRUCT *ddr, uint8 ch, DDR_CHIPSELECT populated_chipselect, PHY_BUS dq, training_params_t *training_params_ptr, uint8 rcw_start_clk_idx, ddrss_rcw_local_vars *local_vars)
{
	uint8  byte_lane             =  0;	// loop counter
	uint8  tDQSCK_in_full_cycle  =  0;
	uint8  prfs_idx              =  0;
	uint8  rank                  =  0;
	uint32 dq0_ddr_phy_base      =  0;	// DDR PHY DQ0 base address
    uint32 rcw_cal_period        =  0;
	uint32 rcw_period            =  0;
	uint32 best_tDQSCK_in_ps     =  0;
	uint32 tDQSCK_in_ps[NUM_CS][NUM_DQ_PCH] =  {{0}};
    uint8  tDQSCK_perCS_full_cycle[NUM_CS] = {0};
//uint8  freq_index            =  0;

	dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;

	rcw_period = CONVERT_CYC_TO_PS / ddr->shrm_ddr_ptr->ddr_runtime.clock_plan[rcw_start_clk_idx].clk_freq_in_khz;//Calculate the rcw period.
	
    for (byte_lane = 0; byte_lane < dq; byte_lane++) 
    {
    	// CALCULATE THE tDQSCK  FOR ALL THE RANKS AND THEIR BYTES
		for(rank = 0; rank < NUM_CS; rank++)
		{
		    if ((populated_chipselect >> rank) & 0x1)  // populated_chipset = b11 for cs = DDR_CS_BOTH,
            {                                          //                   = b1  for cs = DDR_CS0 
			    tDQSCK_in_ps[rank][byte_lane] = ((local_vars->rcw_num_cycles[ch][rank][byte_lane] * rcw_period) +
										    (local_vars->rcw_coarse_cdc[ch][rank][byte_lane] * training_params_ptr->rcw.coarse_cdc_step) +
										    (local_vars->rcw_fine_cdc[ch][rank][byte_lane] * training_params_ptr->rcw.fine_cdc_step));
                #if DSF_RCW_TRAINING_LOG
                  ddr_printf(DDR_NORMAL,"Ch = %d,Rank = %d, Byte = %d: \n", ch, rank, byte_lane);
                  ddr_printf(DDR_NORMAL, "Num_cycles: %d, Coarse_cdc:  %d, Fine_cdc: %d, search_value in ps = %d \n",
                                            local_vars->rcw_num_cycles[ch][rank][byte_lane], 
                                            local_vars->rcw_coarse_cdc[ch][rank][byte_lane],
                                            local_vars->rcw_fine_cdc[ch][rank][byte_lane],
                                            tDQSCK_in_ps[rank][byte_lane]) ;
                #endif
            }
		}
        // determine the best tDQSCK for both the ranks (least of the 2)
        best_tDQSCK_in_ps = (populated_chipselect == DDR_CS_BOTH )? ((tDQSCK_in_ps[0][byte_lane] < tDQSCK_in_ps[1][byte_lane]) ? 
        					                                                                       tDQSCK_in_ps[0][byte_lane] : tDQSCK_in_ps[1][byte_lane]) 
        					                                      : tDQSCK_in_ps[0][byte_lane] ;
							
    for (prfs_idx = RCW_TRAINING_TABLE_START_INDX; prfs_idx <= HIGHEST_TRAINING_TABLE_INDX; prfs_idx++)
		{			
			rcw_cal_period = CONVERT_CYC_TO_PS / ddr->shrm_ddr_ptr->misc.training_freq_in_khz [HIGHEST_TRAINING_TABLE_INDX - prfs_idx];

            if((ddr->shrm_ddr_ptr->misc.platform_id == MSM_VERSION_NAPALI) && (ddr->shrm_ddr_ptr->misc.chip_version == 0x100))
            {
			// Round the tDQSCK_in_full_cycle value  
			tDQSCK_in_full_cycle = (best_tDQSCK_in_ps + (rcw_cal_period/2) ) / rcw_cal_period;
			
			//Program the tDQSCK to the QFI2 register
			DDR_PHY_hal_cfg_rcw_QFI_ext( dq0_ddr_phy_base +( byte_lane * DDR_PHY_OFFSET), tDQSCK_in_full_cycle, prfs_idx); /// PRFS_NO needs to be calculated
            }
            //IF Napali V2
            else //if not Napali V1, applies to Napali V2 and onwards
            {
                //Calculate the per rank full cycle needed
                tDQSCK_perCS_full_cycle [0] = (tDQSCK_in_ps[0][byte_lane] + (rcw_cal_period/2))/ rcw_cal_period;
                if (populated_chipselect == DDR_CS_BOTH){
                    tDQSCK_perCS_full_cycle [1] = (tDQSCK_in_ps[1][byte_lane] + (rcw_cal_period/2))/ rcw_cal_period;
                }
 #if DSF_RCW_TRAINING_LOG
            for(rank = 0; rank< NUM_CS; rank++){
                if ((populated_chipselect >> rank) & 0x1)  // populated_chipset = b11 for cs = DDR_CS_BOTH,
                {
                   ddr_printf(DDR_NORMAL, "PRSF = %d  Channel %d  rank = %d  byte_lane = %d:\t\ttDQSCKin_full_cycle = %d\n",
                                            prfs_idx,
                                            ch, 
                                            rank,
                                            byte_lane,
                                            tDQSCK_perCS_full_cycle [rank] );      
                }
            }

 #endif               
                    //Program the tDQSCK to the QFI2 register
			        DDR_PHY_hal_cfg_rcw_QFI_ext_V2( dq0_ddr_phy_base +( byte_lane * DDR_PHY_OFFSET),populated_chipselect, tDQSCK_perCS_full_cycle,  prfs_idx);
            
            }

		}
			
    }
	
}
