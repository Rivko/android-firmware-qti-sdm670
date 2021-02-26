/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_wrlvl.c#5 $
$DateTime: 2018/12/17 04:30:22 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------

================================================================================*/

/***************************************************************************************************
   Description: The DDRSS Write Leveling Routine performs the LPDDR4 write leveling function.
   Write Leveling is used to determine the skew at the DRAM device between the CK and DQS strobe.
   The hardware does not support training for cs (rank) 1.  The rank 0 results are populated to 
   rank 1 after rank 0 is trained.   
 
***************************************************************************************************/
#include "ddrss.h"
#include "mc.h"

void Training_DDRSS_WRLVL(BOOT_DDR_STRUCT *ddr, 
                          DDR_CHANNEL channel, 
                          DDR_CHIPSELECT chip_select, 
                          uint32 freq,
                          uint8 prfs_index, 
                          training_params_t *training_params_ptr,
                          uint8             *local_vars)               
                  
{  
  //============================================================
  // WRLVL Main Code Thread
  //============================================================
  extern uint8 WRLVL_Training_Switchboard[8][1];
  uint8 freq_eq_max_training    = 0;
  uint32 period_pass1		= 0;
  uint32 freq_pass1		= 0;

  uint8 prfs_index_pass1	= 0;

   freq_eq_max_training 	 = WRLVL_Training_Switchboard[prfs_index][_WRLVL_CF];
   //DDR_CHANNEL channel = ddr->shrm_ddr_ptr->misc.total_channels;
   DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;
	
    if (freq_eq_max_training)
    {
	   //for (i=0; i<=20; i++)  //--> Loop for times
	   //{
			freq_pass1 = WRLVL_PASS1_FREQ;
			period_pass1 = CONVERT_CYC_TO_PS/WRLVL_PASS1_FREQ;
			prfs_index_pass1 = WRLVL_PASS1_PRFS_INDEX;
			DDRSS_wrlvl_seq(ddr,  channel, qualified_cs, freq_pass1, training_params_ptr, prfs_index_pass1);

			DDRSS_wrlvl_seq(ddr, channel, qualified_cs, freq, training_params_ptr, prfs_index);
	   //}	
    } 
        
    ddr_printf(DDR_NORMAL,"\n-----------------------------------------------\n");
} // DDR_PHY_wrlvl


void DDRSS_wrlvl_seq ( BOOT_DDR_STRUCT *ddr, uint8 channel, uint8 cs, uint32 freq, training_params_t *training_params_ptr, uint8 prfs_index)
{
	//uint32 channels               = (DDR_CHANNEL) (ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
	uint32 period                 = CONVERT_CYC_TO_PS/freq;
	uint8 latency_index           = 0;	
	uint8 MR2_opcode              = 0;
	//uint8 cs                      = 0;
	uint8 ch                      = 0;
    DDR_CHIPSELECT qualified_cs = (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[0].populated_chipselect;	
	
	// MC continuous clk
    DDRSS_MC_cnt_clk_en(ddr,channel, qualified_cs);

	// Enable write leveling in the DRAM using MR2 write
	latency_index = MC_DRAM_Latency_Index(ddr->shrm_ddr_ptr, period);
	MR2_opcode = ddr->shrm_ddr_ptr->ddr_runtime.dram_latency[latency_index].MR2;

	ddr_printf(DDR_NORMAL,"-----------------------------------------------\n");
	ddr_printf(DDR_NORMAL,"WRLVL TRAINING @ %d MHz all Channels\n", freq );
   
	for(ch = 0; ch < NUM_CH; ch++)
	{
		// Skip channels which are not enabled
		if(((channel >> ch) & 0x1) == 0x0) continue;
			ddr_printf(DDR_NORMAL, "\n");    
			for (cs = 0; cs < NUM_CS; cs++)
			{
				if (((qualified_cs >> cs) & 0x1) && (freq == WRLVL_PASS1_FREQ) )
				{
					DDRSS_wrlvl_ca(ddr,ch,cs,freq,training_params_ptr, prfs_index, MR2_opcode);
					#if DSF_WRLVL_TRAINING_LOG  
						if (cs > 0)
						{
							ddr_printf(DDR_NORMAL, "\n Choosing worst case data for CA across both ranks");
						}
					#endif 
				}
			}
			
			for (cs = 0; cs < NUM_CS; cs++)
			{
				if ((qualified_cs >> cs) & 0x1)
				{
					DDRSS_wrlvl_dqs(ddr,ch,cs,freq,training_params_ptr, prfs_index, MR2_opcode);
				}
			}

	}//ch
	
    // Disable MC continuous clk
    DDRSS_MC_cnt_clk_dis(ddr,channel,qualified_cs);
        
	// Disable WRLVL training mode in MR2
	ddr->shrm_ddr_ptr->ddr_runtime.dram_latency[latency_index].MR2 = MR2_opcode;
	
	// Switch to target frequency
	ddr_external_set_clk_speed (freq);
}

void DDRSS_wrlvl_ca(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint32 freq, training_params_t *training_params_ptr, uint8 prfs_index, uint8 MR2_opcode)
{
  //-------------------------------------------------------------------------------------
  // Check for DQS->CLK skew. If the CLK is early, add delay to the CA PHY WRLVL CDC
  // There are two DQ PHYs for each CA PHY.  Both DQ PHYs for each CA PHY must start with
  // a zero (fail) Write Level value.  
  //-------------------------------------------------------------------------------------

    uint8 wrlvl_ca_done                 = 0;  // CA wrlvl routine status
    uint8 bisc_status                   = 0;  // Device WRLVL result
    uint8 feedback_percent              = 0;  // Local feedback percent
    uint8 ca                            = 0;  // CA Loop counter
    uint8 loopcnt                       = 0;  // CA Loop counter
    uint8 dq                            = 0;  // DQ Loop counter
    uint8 done                          = 0;  // Loop done counter
    uint8 wrlvl_max_coarse_cdc          = 0;  // Coarse CDC Saturation Value
    uint8 wrlvl_max_loopcnt             = 0;  
    uint8 index                         = 0;  

    uint8 byte_one_cnt[NUM_DQ_PCH]      = {0};
    uint8 byte_zero_cnt[NUM_DQ_PCH]     = {0};
    uint8 byte_ca_done[NUM_DQ_PCH]      = {0};   // CA status of the dq bytes
    uint8 ca_delay[NUM_CA_PCH]          = {0};   // CA Write level coarse delay
	uint8 ca_delay_r0[NUM_CA_PCH]       = {0};   // CA Write level coarse delay

    uint32 dq0_ddr_phy_base             = 0;     // DDR PHY DQ0 Base Address
    uint32 ca0_ddr_phy_base             = 0;     // DDR PHY CA0 Base Address
    uint32 reg_offset_ddrcc             = 0;     //DDRCC Base address  

    uint8  wrlvl_max_fine_cdc           = 0;     // Fine CDC saturation limit
    uint8  wrlvl_feedback_percent       = 0;     // Feedback histogram pass percent
    uint8  wrlvl_coarse_cdc_step        = 0;     // Coarse CDC step
    uint8  wrlvl_fine_cdc_step          = 0;     // Fine CDC step
    uint32 CA_QFI2EXT_PRFS_CNTL_1_CFG   = 0;
    uint32 CA_PERIODIC_TRAINING_CLK     = 0;
    uint32 CA_TIMESTAMP_COUNTER_OFFSET  = 0;
    uint32 DQ_QFI2EXT_PRFS_CNTL_1_CFG   = 0;    
    uint32 DQ_PERIODIC_TRAINING_CLK     = 0;
    uint32 DQ_TIMESTAMP_COUNTER_OFFSET  = 0;
    
    // Training data structure pointer
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);

    // Set DQ0 PHY BASE for the channel and cs
    dq0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    ca0_ddr_phy_base = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    reg_offset_ddrcc = REG_OFFSET_DDR_PHY_CH(ch) + DDR_CC_OFFSET ; 

    // Set CDC Saturation Limits from the training params data structure
    wrlvl_max_coarse_cdc   = training_params_ptr->wrlvl.max_coarse_cdc;
    wrlvl_max_loopcnt      = training_params_ptr->wrlvl.max_loopcnt;
    wrlvl_max_fine_cdc     = training_params_ptr->wrlvl.max_fine_cdc;
    wrlvl_feedback_percent = training_params_ptr->wrlvl.feedback_percent;
    wrlvl_coarse_cdc_step  = training_params_ptr->wrlvl.coarse_cdc_step;
    wrlvl_fine_cdc_step    = training_params_ptr->wrlvl.fine_cdc_step;  


    for (ca=0;ca<NUM_CA_PCH;ca++) 
    {
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, TMIN_SW_CLK_ON, 0x1);   
        
        CA_PERIODIC_TRAINING_CLK = HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON);
        CA_QFI2EXT_PRFS_CNTL_1_CFG = HWIO_INX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG);
        CA_TIMESTAMP_COUNTER_OFFSET = HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET);
        
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON,0x1);
        HWIO_OUTX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, 0x141);  
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);

        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x1);
    } 
 
    // Enable software override to DQS/DQ pad OEs.
    for (dq = 0; dq < NUM_DQ_PCH; dq++) 
    {
        //Turn on continuous DDRCC clock
        HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_SEL, 0x1);
        HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_VAL, 0x1);  

        DQ_PERIODIC_TRAINING_CLK = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON);
        DQ_QFI2EXT_PRFS_CNTL_1_CFG = HWIO_INX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG);
        DQ_TIMESTAMP_COUNTER_OFFSET = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET);
        
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON,0x1);
        HWIO_OUTX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, 0x141);  /// ADDED for exp.
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);

        //Initialize BIST
        //DDR_PHY_hal_wrlvl_DQ_burst_BIST (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 1,clk_freq_khz); 
        DDRSS_BIST_wrlvl (ddr, ch, (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)), 1,freq);
        
        //Switch to target frequency
         ddr_external_set_clk_speed (freq);
         
        // Enable PAD SW override 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQ, 0x3FF);

        // Enable PAD OE 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQ, 0);

        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_CA_PAD_OE_OVR_MODE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQS, 1);

        // Enable DQ PAD IE
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_MODE_IE_DQ, 0x3FF);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_ENABLE_IE_DQ, 0x3FF);

        // Set DQPHY in WRLVL mode
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_BIST_WRLVL_CTRL_0_CFG,WRLVL_TRAINING_MODE,0x1);//
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, CBT_EN, 1);//
    }
    // Enable write leveling mode in memory device
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_2, (1 << 7) | MR2_opcode);
    
	//Set WRLVL Fine/Coarse delay to "0"
    if (freq == WRLVL_PASS1_FREQ)
	{
		for (dq = 0; dq < NUM_DQ_PCH; dq++) 
		{    
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 0, 1, 0 ); // Set WRLVL Fine delay to "0", for Rank0
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 1, 1, 0 ); // Set WRLVL Coarse delay to "0", , for Rank0
			
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 0, 1, 1 ); // Set WRLVL Fine delay to "0", for Rank1
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 1, 1, 1 ); // Set WRLVL Coarse delay to "0", , for Rank1
		}
	}
    
    //set coarse and fine to zero
    for (ca=0;ca<NUM_CA_PCH;ca++) 
    { 
		if ( cs == 1 )
		{
			ca_delay_r0[ca]	=  HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_0);
		}
		if (freq == WRLVL_PASS1_FREQ)
		{
			DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), 
												0, //setting coarse to zero
												0 ); //CS0
			DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), 
												0, //setting coarse to zero
												1 ); //CS1												
		}
		//Store initial value of CA Coarse WRLVL (either trainied or init value which should be 0)
		ca_delay[ca]    =  HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_0);
			
		// Loop until all DQs are done or error
        while (wrlvl_ca_done < NUM_DQ_PCH) 
        {
            // Read the bisc_status (Write Level result) from each DQ PHY pair
            for (dq=0;dq<NUM_DQ_PCH;dq++) 
            {  
                HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
                HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
                if (ca_delay[ca] >= wrlvl_max_coarse_cdc) 
                {
                    byte_ca_done[dq] = 1;
                }
                else 
                {
                    if (byte_ca_done[dq] == 0) 
                    {
                        byte_one_cnt[dq]  = 0;
                        byte_zero_cnt[dq] = 0;

                        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
                        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);    

                        for (loopcnt = wrlvl_max_loopcnt;loopcnt > 0; loopcnt-- ) ///*wrlvl_max_loopcnt;loopcnt > 0*/
                        {   
                            HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);

                            //DDRSS_mem_write_PXI_8 (ch, cs);
                            bisc_status =  DDR_PHY_hal_sta_wrlvl_training(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET));

                            // Collapse the status to a single bit
                            bisc_status = (bisc_status & 0x00000001);

                            //  Check the dq status and increment the one or zero count
                            if (bisc_status == 1) 
                            {
                                byte_one_cnt[dq]++;
                            } 
                            else 
                            {
                                byte_zero_cnt[dq]++;
                            }
                            //BIST stop
                            HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
                        }

                        // Calculate the threshold in percent for Write Level pass (histogram)
                        if ((byte_one_cnt[dq] + byte_zero_cnt[dq]) != 0) 
                        {
                            feedback_percent =  ((byte_one_cnt[dq] * 100) / (byte_one_cnt[dq] + byte_zero_cnt[dq]));
                        }
                        else 
                        {
                            feedback_percent = 0;
                        }

                        // Check dq status.  If equal to one (pass), increment the CA WRLVL delay (CA CLK and Data)
                        if (feedback_percent != 0) 
                        {
                            ca_delay[ca]++;

                            DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), 
                                                               ca_delay[ca], 
                                                               cs );
                        }
                        else 
                        {
                            byte_ca_done[dq] = 1;
                        }

                    } // if byte_ca_done
                } // else byte_ca_done
            } // for dq

            // Aggregate all of the byte status to determine the ca done state
            wrlvl_ca_done = 0;
            for (done = 0; done < NUM_DQ_PCH; done++) 
            {
              wrlvl_ca_done = wrlvl_ca_done + byte_ca_done[done];
            }
      
        } // while wrlvl_ca_done
    } //ca 
    
    for (ca=0;ca<NUM_CA_PCH;ca++) 
    { 
     if (ca_delay[ca] < ca_delay_r0[ca])
	 {
		ca_delay[ca] = ca_delay_r0[ca];
	 }
	 // DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET), ca_delay[ca], cs );
    if (prfs_index < LOWEST_WRLVL_COPY_PRFS)
    { 
       // Write the EXT registers including and below the trained frequency
		for (index = prfs_index+1; index > 0; )
        {
			index--;
			DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update(ca0_ddr_phy_base  + (ca * DDR_PHY_OFFSET),
											  index, 
											  0, //cs
											  0, //fine
											  ca_delay[ca]); //coarse
			
			DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update(ca0_ddr_phy_base  + (ca * DDR_PHY_OFFSET),
											  index, 
											  1, //cs
											  0, //fine
											  ca_delay[ca]); //coarse
        }


	}
	else
	{	 
		// Write the EXT registers including and below the trained frequency
        for (index = MAX_TRAINING_FREQ_INDEX; index >= LOWEST_WRLVL_COPY_PRFS; index--)
        {
            // Write the EXT DCC register 
            DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update(ca0_ddr_phy_base  + (ca * DDR_PHY_OFFSET),
                                              index, 
                                              0, //cs
                                              0, //fine
                                              ca_delay[ca]); //coarse
											  
											              // Write the EXT DCC register 
            DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update(ca0_ddr_phy_base  + (ca * DDR_PHY_OFFSET),
                                              index, 
                                              1, //cs
                                              0, //fine
                                              ca_delay[ca]); //coarse
        }
	}
        //disable registers                                                                              
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_TOP_CTRL_0_CFG, TMIN_SW_CLK_ON, 0x0); 
        
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, CA_PERIODIC_TRAINING_CLK);
        HWIO_OUTX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, CA_QFI2EXT_PRFS_CNTL_1_CFG);
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, CA_TIMESTAMP_COUNTER_OFFSET);
        
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x0);

        //BIST exit sequence
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0); 
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x0); //Set back to default value 0
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE,0x0);
        HWIO_OUTXF (ca0_ddr_phy_base + (ca* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_DATA_DLY_CNT_LOAD_VAL,0x0);
        HWIO_OUTXF3 (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL,
                                                           BIST_LOOP_CNT_LOAD_VAL, 
                                                           BIST_CHK_TIMER_LOAD_VAL, 
                                                           0x0,0x0,0x7);
        HWIO_OUTXF4 (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_0,
                                                           OPCODE_1, 
                                                           OPCODE_2,
                                                           OPCODE_3,															   
                                                           0x0,0x0,0x0,0x0);
        HWIO_OUTXF4 (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_0,
                                                           OPCODE_1, 
                                                           OPCODE_2,
                                                           OPCODE_3,															   
                                                           0x0,0x0,0x0,0x0);
                  
        #if DSF_WRLVL_TRAINING_LOG    
        ddr_printf(DDR_NORMAL,"\n    WRLVL ch %d rank %d CA %d coarse = %d  Total = %d ps",
                    ch,
                    cs,
                    ca,
                    ca_delay[ca],
                    ca_delay[ca] * COARSE_STEP_IN_PS
              );
        #endif  

        if((ca_delay[ca] >= wrlvl_max_coarse_cdc) || (ca_delay[ca] >= (wrlvl_max_coarse_cdc-1)))
        {
            ddr_printf (DDR_NORMAL, "\nDDR ABORT: WRLVL CA CDC SATURATED\n ");
            ddr_abort();
        }

    }

    for (dq = 0; dq < NUM_DQ_PCH; dq++) 
    {  
        // Disable DQ registers       
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, DQ_PERIODIC_TRAINING_CLK);
        HWIO_OUTX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, DQ_QFI2EXT_PRFS_CNTL_1_CFG);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, DQ_TIMESTAMP_COUNTER_OFFSET);

        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_BIST_WRLVL_CTRL_0_CFG,WRLVL_TRAINING_MODE,0x0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, CBT_EN, 0);

        //BIST exit sequence
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0); 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x0); //Set back to default value 0
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE,0x0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq* DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_DATA_DLY_CNT_LOAD_VAL,0x0);
        HWIO_OUTXF3 (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL,
                                                               BIST_LOOP_CNT_LOAD_VAL, 
                                                               BIST_CHK_TIMER_LOAD_VAL, 
                                                               0x0,0x0,0x7);
        // Disable PAD SW override 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQS, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQ, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQS, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQ, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_CA_PAD_OE_OVR_MODE_DQS, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQS, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_MODE_IE_DQ, 0);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_ENABLE_IE_DQ, 0);                                                           
    }
	
	// Disable write leveling mode in memory device
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_2, MR2_opcode);

}// DDRSS_wrlvl_ca

//  //-----------------------------------------------------------------------------
//  //  For the selected frequency, measure dqs->clk delay (wrlvl_max_loopcnt) 
//  //  times for each wrlvl delay value. if the one count
//  //  is greater than the zero count, the wrlvl training is finished.
//  //  If not, the coarse CDC register is incremented. When the coarse CDC passes, 
//  //  it is decremented and the loop repeats and increments the fine CDC delay.
//  //  This loop is kept continued until either the one count is bigger than the
//  //  zero count or when the wrlvl delay value reaches its maximum limit (MAX_CDC).
//  //--------------------------------------------------------------------------------------
//
void DDRSS_wrlvl_dqs(BOOT_DDR_STRUCT *ddr, uint8 ch, uint8 cs, uint32 freq, training_params_t *training_params_ptr, uint8 prfs_index, uint8 MR2_opcode)

{
    uint8  dq_status                    = 0;  // Device WRLVL result
    uint8  feedback_percent             = 0;  // Local feedback percent
    uint8  ca                           = 0;  // CA Loop counter
    uint8  loopcnt                      = 0;  // CA Loop counter
    uint8  dq                           = 0;  // DQ Loop counter
    uint8  done                         = 0;  // Loop done counter
    uint8  wrlvl_max_coarse_cdc         = 0;  // Coarse CDC Saturation Value
    uint8  wrlvl_max_loopcnt            = 0;  
    uint8  index                        = 0;  
    
    uint8 byte_one_cnt[NUM_DQ_PCH]      = {0};
    uint8 byte_zero_cnt[NUM_DQ_PCH]     = {0};
    
    uint32 dq0_ddr_phy_base             = 0;     // DDR PHY DQ0 Base Address
    uint32 ca0_ddr_phy_base             = 0;     // DDR PHY CA0 Base Address
    uint32 reg_offset_ddrcc             = 0;     //DDRCC Base address  
    
    uint8 fine_dqs_started[NUM_DQ_PCH]  = {0};   // Status of the dq bytes
    uint8 byte_dqs_done[NUM_DQ_PCH]     = {0};   // Status of the dq bytes
    uint8 fine_dqs_delay[NUM_DQ_PCH]    = {0};   // Write Level fine delay
    uint8 coarse_dqs_delay[NUM_DQ_PCH]  = {0};   // Write level coarse delay
    uint8 coarse_dqs_done[NUM_DQ_PCH]   = {0};   // Write level coarse delay status
    uint8 wrlvl_max_fine_cdc            = 0;     // Fine CDC saturation limit
    uint8 wrlvl_feedback_percent        = 0;     // Feedback histogram pass percent
    uint8 wrlvl_coarse_cdc_step         = 0;     // Coarse CDC step
    uint8 wrlvl_fine_cdc_step           = 0;     // Fine CDC step
    uint8 wrlvl_dqs_done                = 0; 
    uint32 CA_QFI2EXT_PRFS_CNTL_1_CFG   = 0;
    uint32 CA_PERIODIC_TRAINING_CLK     = 0;
    uint32 CA_TIMESTAMP_COUNTER_OFFSET  = 0;
    uint32 DQ_QFI2EXT_PRFS_CNTL_1_CFG   = 0;    
    uint32 DQ_PERIODIC_TRAINING_CLK     = 0;
    uint32 DQ_TIMESTAMP_COUNTER_OFFSET  = 0;
    
    // Training data structure pointer
    training_data *training_data_ptr;
    training_data_ptr = (training_data *)(&ddr->training_results.training_data);
    
    // Set DQ0 PHY BASE for the channel and cs
    dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
    ca0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;
    reg_offset_ddrcc = REG_OFFSET_DDR_PHY_CH(ch) + DDR_CC_OFFSET ; 
    
    // Set CDC Saturation Limits from the training params data structure
    wrlvl_max_coarse_cdc   = training_params_ptr->wrlvl.max_coarse_cdc;
    wrlvl_max_loopcnt      = training_params_ptr->wrlvl.max_loopcnt;
    wrlvl_max_fine_cdc     = training_params_ptr->wrlvl.max_fine_cdc;
    wrlvl_feedback_percent = training_params_ptr->wrlvl.feedback_percent;
    wrlvl_coarse_cdc_step  = training_params_ptr->wrlvl.coarse_cdc_step;
    wrlvl_fine_cdc_step    = training_params_ptr->wrlvl.fine_cdc_step;  
  
    for (ca=0;ca<NUM_CA_PCH;ca++) 
    {
        CA_PERIODIC_TRAINING_CLK = HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON);
        CA_QFI2EXT_PRFS_CNTL_1_CFG = HWIO_INX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG);
        CA_TIMESTAMP_COUNTER_OFFSET = HWIO_INXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET);
        
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
        HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON,0x1);  //ADDED for EXP
        HWIO_OUTX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, 0x141);  //ADDED for EXP
    }   

    // Enable software override to DQS/DQ pad OEs.
    for (dq = 0; dq < NUM_DQ_PCH; dq++) 
    {
        //Turn on continuous DDRCC clock
        HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_SEL, 0x1);
        HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_VAL, 0x1);  
    
        DQ_PERIODIC_TRAINING_CLK = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON);
        DQ_QFI2EXT_PRFS_CNTL_1_CFG = HWIO_INX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG);
        DQ_TIMESTAMP_COUNTER_OFFSET = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET);
        
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON,0x1);
        HWIO_OUTX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, 0x141); //ADDED for EXP
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
    
        //Initialize BIST
        //DDR_PHY_hal_wrlvl_DQ_burst_BIST (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 1,clk_freq_khz);
        DDRSS_BIST_wrlvl (ddr, ch, (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)), 1,freq);    
                
        //Switch to target frequency
        ddr_external_set_clk_speed (freq);
        
        // Enable PAD SW override 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQ, 0x3FF);
    
        // Enable PAD OE 
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQ, 0);
        
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_CA_PAD_OE_OVR_MODE_DQS, 1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQS, 1);
    
    
        // Enable DQ PAD IE
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_MODE_IE_DQ, 0x3FF);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_ENABLE_IE_DQ, 0x3FF);
    
        // Set DQPHY in WRLVL mode
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_BIST_WRLVL_CTRL_0_CFG,WRLVL_TRAINING_MODE,0x1);
        HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, CBT_EN, 1);        
    }
    
    // Enable write leveling mode in memory device
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_2, (1 << 7) | MR2_opcode);
    
	//TODO: remove later 
	if (freq == WRLVL_PASS1_FREQ)
	{
		//Set WRLVL Fine/Coarse delay to "0"
		for (dq = 0; dq < NUM_DQ_PCH; dq++) 
		{    
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 0, 1, 0 ); // Set WRLVL Fine delay to "0", for Rank0
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 1, 1, 0 ); // Set WRLVL Coarse delay to "0", , for Rank0
			
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 0, 1, 1 ); // Set WRLVL Fine delay to "0", for Rank1
			DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), 0 , 1, 1, 1 ); // Set WRLVL Coarse delay to "0", , for Rank1
		}
	}
	
	for (dq = 0; dq < NUM_DQ_PCH; dq++) 
    {
		if (cs == 0)
		{
			fine_dqs_delay[dq] = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_FINE_DELAY_RANK_0);
			coarse_dqs_delay[dq] = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_0);
		}
		if (cs == 1)
		{
			fine_dqs_delay[dq] = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_FINE_DELAY_RANK_1);
			coarse_dqs_delay[dq] = HWIO_INXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_1);
		}
	}

    // Loop through each DQ until Done or Error
    while (wrlvl_dqs_done < NUM_DQ_PCH) 
    {         
        // Setup all of the DQ PHYs
        for (dq = 0; dq < NUM_DQ_PCH; dq++) 
        {
            // Update WRLVL CDC
            if (byte_dqs_done[dq] == 0) 
            {
                DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),
                                            fine_dqs_delay[dq], 
                                            0, // fine 
                                            1, // hp_mode
                                            0 ); //Hard Coding to Rank 0 only during trainig due to Bist OP CODE
                
                DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),
                                                coarse_dqs_delay[dq], 
                                                1, // coarse 
                                                1, // hp_mode
                                                0 ); //Hard Coding to Rank 0 only during trainig due to Bist OP CODE
            }
            HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
            HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);    
        
            //  Repeat the WRLVL test for max_loopcount to make a 1 dimensional histogram
            for (loopcnt = wrlvl_max_loopcnt;loopcnt > 0;loopcnt-- ) 
            {
                // Update the status of bytes that are not done
                if (byte_dqs_done[dq] == 0) {
                HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);
            
                // DDRSS_mem_write_PXI_8 (ch, cs);
                dq_status = DDR_PHY_hal_sta_wrlvl_training(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET));
                // Collapse the status to a single bit
                dq_status = (dq_status & 0x00000001);
    
                //  Check the dq status and increment the one or zero count
                if (dq_status == 1) 
                {
                    byte_one_cnt[dq]++;
                } 
                else 
                {
                    byte_zero_cnt[dq]++;
                }
            
                //BIST exit sequence
                HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
            } // if (!byte_dqs_done
        } // for (loopcnt
    } //  End of loop for a given wrlvl delay value

    
    //=========================================================================
    // Process the Write Level Loop Result
    //=========================================================================
    for (dq = 0; dq < NUM_DQ_PCH; dq++) 
    {
        //-------------------------------------------------------------------------
        // Write Level Result Process Flow Control
        //-------------------------------------------------------------------------

        // Process the DQ if it is not done
        if (byte_dqs_done[dq] == 0) 
        {
            // Calculate the threshold in percent for Write Level pass (histogram)
            if ((byte_one_cnt[dq] + byte_zero_cnt[dq]) != 0) 
            {
                feedback_percent =  ((byte_one_cnt[dq] * 100) / (byte_one_cnt[dq] + byte_zero_cnt[dq]));
            }
            else 
            {
                feedback_percent = 0;
            }
            
            // Generate an Error and terminate if CDC overflows
            if ((coarse_dqs_delay[dq] >= (wrlvl_max_coarse_cdc - 1)) && (fine_dqs_delay[dq] >= wrlvl_max_fine_cdc)) 
            {
                byte_dqs_done[dq]   = 1;
            }
            // Coarse completed and the fine one count is greater than the zero count: wrlvl is complete
            else if ((coarse_dqs_done[dq] == 1) && (fine_dqs_started[dq] == 1) && (feedback_percent >= wrlvl_feedback_percent)) 
            {
                byte_dqs_done[dq] = 1;
            }
            // Course is complete and fine has not started : subtract one from coarse and start fine
            else if ((fine_dqs_started[dq] == 0) && ((feedback_percent >= wrlvl_feedback_percent) || (coarse_dqs_delay[dq] == wrlvl_max_coarse_cdc))) 
            {
                coarse_dqs_done[dq] = 1;
                fine_dqs_started[dq] = 1;
                if (coarse_dqs_delay[dq] != 0) 
                {
                    coarse_dqs_delay[dq] -= wrlvl_coarse_cdc_step;
                
                    // Update decremented coarse CDC
                    DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),
                                                coarse_dqs_delay[dq], 
                                                1, // coarse 
                                                1, // hp_mode
                                                0 ); //Hard Coding to Rank 0 only during trainig due to Bist OP CODE
                }
            }
            // Coarse is complete and the one count is NOT greater than the zero count : increment fine
            else if ((coarse_dqs_done[dq] == 1) && (feedback_percent < wrlvl_feedback_percent)) 
            {
                if (fine_dqs_delay[dq] < wrlvl_max_fine_cdc)
                {
                    fine_dqs_delay[dq] += wrlvl_fine_cdc_step;
                }
                else
                {
                    // If Fine delay has reached the MAX limit, Set the fine delay:0 and CDC delay +1 and continue the search for fine delay
                    fine_dqs_delay[dq] = 0;
                    coarse_dqs_delay[dq] += wrlvl_coarse_cdc_step;
                }
            }
            else 
            {
                // Increment coarse delay 
                coarse_dqs_delay[dq] += wrlvl_coarse_cdc_step;
            } 
        }// if !byte_dqs_done

        // Reset byte counters for the next iteration
        byte_one_cnt[dq]  = 0;
        byte_zero_cnt[dq] = 0;

    } // for (dq

    // Aggregate all of the byte status to determine the done state
    wrlvl_dqs_done = 0;
    
    for (done = 0; done < NUM_DQ_PCH; done++) 
    {
        wrlvl_dqs_done = wrlvl_dqs_done + byte_dqs_done[done];
    }

  } // while (wrlvl_dqs_done<)

  for (dq = 0; dq < NUM_DQ_PCH; dq++) 
  {
   
   if (prfs_index < LOWEST_WRLVL_COPY_PRFS)
   { 
		// Write the EXT registers including and below the trained frequency
		//for (index = LOWEST_WRLVL_COPY_PRFS; index <=7 ; index++)
		for (index = prfs_index+1; index > 0; )
		{
			index--;
			DDR_PHY_hal_cfg_cdcext_wrlvl_update(dq0_ddr_phy_base  + (dq * DDR_PHY_OFFSET),
											   index,
											   cs,
											   fine_dqs_delay[dq],
											   coarse_dqs_delay[dq]);
		} 	
  }
  else
  {
    // Write the EXT registers including and below the trained frequency
    for (index = MAX_TRAINING_FREQ_INDEX; index >= LOWEST_DCC_COPY_PRFS_DRRCC; index--)
    {
      // Write the EXT DCC register
      DDR_PHY_hal_cfg_cdcext_wrlvl_update(dq0_ddr_phy_base  + (dq * DDR_PHY_OFFSET),
                                          index,
                                          cs,
                                          fine_dqs_delay[dq],
                                          coarse_dqs_delay[dq]);
                                          
    } 
 
  }
    // Write the trained WRLVL values 
    DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),
                                    fine_dqs_delay[dq], 
                                    0, // fine 
                                    1, // hp_mode
                                    cs ); 

    DDR_PHY_hal_cfg_cdc_slave_wrlvl(dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),
                                    coarse_dqs_delay[dq], 
                                    1, // coarse 
                                    1, // hp_mode
                                    cs ); 

    // Disable WRLVL pulse control
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_BIST_WRLVL_CTRL_0_CFG, WRLVL_TRAINING_MODE, 0);
   
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, DQ_PERIODIC_TRAINING_CLK);
    HWIO_OUTX  (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, DQ_QFI2EXT_PRFS_CNTL_1_CFG);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, DQ_TIMESTAMP_COUNTER_OFFSET);
    
	//BIST exit sequence
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0); 
    //HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, FSM_BIST_STATE_EN, 0x0); 
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x0); //Set back to default value 0
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_CHK_SW_CLK_ON, 0x0); //Set back to default value 0 
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE,0x0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF, 0x0); //Set back to default value 0  
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_DATA_DLY_CNT_LOAD_VAL,0x0);
    HWIO_OUTXF3 (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL,
                                                           BIST_LOOP_CNT_LOAD_VAL, 
                                                           BIST_CHK_TIMER_LOAD_VAL, 
                                                           0x0,0,0x7);
    
    // Disable PAD SW override 
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQS, 0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQ, 0);

    // Disable PAD OE 
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQS, 0);
      HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQ, 0);
    
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_CA_PAD_OE_OVR_MODE_DQS, 0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_CA_PAD_OE_OVR_MODE_DQ, 0);
    
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQS, 0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQ, 0);

    // Disable DQ PAD IE
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_MODE_IE_DQ, 0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG, SW_PAD_ENABLE_IE_DQ, 0); 
    
    //Turn off continuous DDRCC clock
    HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_SEL, 0x0);
    HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_TOP_CTRL_CFG, DDRCC_LVDS_CLK_ENABLE_OVRD_VAL, 0x0);  


    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_BIST_WRLVL_CTRL_0_CFG,WRLVL_TRAINING_MODE,0x0);
    HWIO_OUTXF (dq0_ddr_phy_base + (dq * DDR_PHY_OFFSET), DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, CBT_EN, 0); 
    
    #if DSF_WRLVL_TRAINING_LOG    
    ddr_printf(DDR_NORMAL,"\n    WRLVL ch %d rank %d DQ %d coarse = %d  fine = %d  Total = %d ps",
                ch,
                cs,
                dq,
                coarse_dqs_delay[dq],
                fine_dqs_delay[dq],
                (coarse_dqs_delay[dq] * COARSE_STEP_IN_PS) + (fine_dqs_delay[dq] * FINE_STEP_IN_PS)
          );
    #endif    

    if((coarse_dqs_delay[dq] >=  wrlvl_max_coarse_cdc) || (coarse_dqs_delay[dq] >= (wrlvl_max_coarse_cdc-1)))
    {
        ddr_printf (DDR_NORMAL, "\nDDR ABORT: WRLVL DQ CDC SATURATED\n",coarse_dqs_delay[dq]);
        ddr_abort();
    }
 
    
  } // for (dq)

 for (ca=0;ca<NUM_CA_PCH;ca++) 
 {    
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, CA_PERIODIC_TRAINING_CLK);
    HWIO_OUTX  (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG, CA_QFI2EXT_PRFS_CNTL_1_CFG);
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET)+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, CA_TIMESTAMP_COUNTER_OFFSET);
    
    //BIST exit sequence
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET) ,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0); 
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x0); //Set back to default value 0
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE,0x0);
    HWIO_OUTXF (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_DATA_DLY_CNT_LOAD_VAL,0x0);
    HWIO_OUTXF3 (ca0_ddr_phy_base + (ca * DDR_PHY_OFFSET),DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL,
                                                           BIST_LOOP_CNT_LOAD_VAL, 
                                                           BIST_CHK_TIMER_LOAD_VAL, 
                                                           0x0,0,0x7);
 }
	
	// Disable write leveling mode in memory device
	MC_MR_Write (ddr, CH_1HOT(ch), CS_1HOT(cs), JEDEC_MR_2, MR2_opcode);
} // DDRSS_wrlvl_dqs

// EOF
