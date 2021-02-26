//===========================================================================
//  Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
//  QUALCOMM Proprietary and Confidential. 
//===========================================================================
////////////////////////////////////////////////////////////////////////////////////////////////
#include "ddrss.h"
#include "ddr_phy.h"
#include "target_config.h"
#include "mc_common.h"


// ***********************************************************************
/// Calculate CDC mask
// ***********************************************************************
uint32 DDR_PHY_hal_cfg_cdc_mask( uint16 coarse_fine, uint8 rank, uint16 hp_mode )
{
    uint32 mask = 0;

    if (rank == 0) {
        if (hp_mode == 1) {
            if (coarse_fine == 1) { //[4:0]
                mask = 0xFFFFFFE0; 
            }
            else { // fine delay [9:5]
                mask  = 0xFFFFFC1F; 
            }
        }
        else {
            mask = 0xFE0FFFFF; // lpcdc [24:20]
        }
    }
    else {
        if (hp_mode == 1) {
            if (coarse_fine == 1) { // [14:10]
                mask  = 0xFFFF83FF; 
            } 
            else { // Fine delay [19:15]
                mask  = 0xFFF07FFF; 
            }
        }
        else {
            mask = 0xC1FFFFFF; // lpcdc [29:25]
        }
    }    

    return mask;

} 

// ***********************************************************************
/// Calculate CDC shift
// ***********************************************************************

uint32 DDR_PHY_hal_cfg_cdc_shift( uint16 coarse_fine, uint8 rank, uint16 hp_mode )
{
    uint32 shift = 0;

    if (rank == 0) {
        if (hp_mode == 1) {
            if (coarse_fine == 1) { //[4:0]
                shift = 0;
            }
            else { // fine delay [9:5]
                shift  = 5; 
            }
        }
        else {
            shift = 20;
        }
    }
    else {
        if (hp_mode == 1) {    
            if (coarse_fine == 1) { // [14:10]
                shift  = 10; 
            } 
            else { // Fine delay [19:15]
                shift  = 15; 
            }
        }
        else {
            shift = 25;
        }
    }

    return shift;
}

// ***********************************************************************
/// ----------------------------------------------------------
///  HAL : CONFIG WRLV SLAVE
/// ----------------------------------------------------------
// ***********************************************************************
void DDR_PHY_hal_cfg_cdc_slave_wrlvl( uint32 _inst_, uint32 delay, uint16 coarse_fine, uint16 hp_mode, uint8 rank )
{
    if (rank == 0) {
            if (coarse_fine == 1) { //[4:0]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_0, delay); 
            }
            else { // fine delay [9:5]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_FINE_DELAY_RANK_0, delay); 
            }
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0X0);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0X1);
   }
    else {
            if (coarse_fine == 1) { // [14:10]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_1, delay); 
            } 
            else { // Fine delay [19:15]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_FINE_DELAY_RANK_1, delay);  
            }
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0X0);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0X1);
    }   


}

// CK CDC update workaround for Napali V1
void DDR_PHY_hal_cfg_cdc_slave_wrlvl_CA( uint32 _inst_,uint16 coarse_delay, uint8 rank)
{
    
    rank = coarse_delay & 1;
 if (rank == 0) {
        //Update CA write leveling CDC delay value
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_0, coarse_delay); 
            
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X0);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X0);
  
       //Send CA rank0 write opcode with DEC command to trigger rank update sequence. 
       //Write leveling CDC rank0 delay value will be applied.
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_0, 0x0);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_1, 0x1);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_2, 0x1);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_3, 0x1);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_0, 0x1);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_1, 0x1);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_2, 0x1);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_3, 0x1);        
       // seq_wait(1000,MS);
       
   }
    else 
    {
        //Update CA write leveling CDC delay value
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_CTL_CFG, HP_COARSE_DELAY_RANK_1, coarse_delay); 
            
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X0);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWRLVL_TOP_CFG, LOAD, 0X0);
        
        //Send CA rank1 write opcode with DEC command to trigger rank update sequence. 
        //Write leveling CDC rank0 delay value will be applied.
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_0, 0x0);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_1, 0x3);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_2, 0x3);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG,OPCODE_3, 0x3);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_0, 0x3);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_1, 0x3);
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_2, 0x3);    
        HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG,OPCODE_3, 0x3); 
    }   
    
    HWIO_OUTXF2 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE, BIST_DATA_DLY_CNT_LOAD_VAL, 0x1,0x4); 
    HWIO_OUTXF2 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL, BIST_LOOP_CNT_LOAD_VAL, 0xF,0x1);  

   // seq_wait(1000,MS);    

    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x0);    
}


void DDR_PHY_hal_wrlvl_DQ_burst_BIST ( uint32 _inst_, uint32 loop_count, uint32 clk_freq_khz)
{
   uint32 inst_offset_lut               = 0;
   uint32 lut                            =0;  

    // HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF, 0x1);
    //HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, 0x3A);
    
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, 0x0);
    HWIO_OUTX (_inst_,DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, 0x0);
    
    //BIST USR data
    for (lut=0;lut<16;lut++) 
    {      
      // Add LUT offset to the base address
      inst_offset_lut = _inst_ + (lut * 0x4);
      HWIO_OUTX (inst_offset_lut,DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, 0x0);
    } 

    HWIO_OUTXF2 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_MODE, BIST_DATA_DLY_CNT_LOAD_VAL, 0x1,0x4); 
    HWIO_OUTXF3 (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_CFG, BIST_OPCODE_INT_CNT_LOAD_VAL, BIST_LOOP_CNT_LOAD_VAL,BIST_CHK_TIMER_LOAD_VAL, 0xF,loop_count,0x3);    
    //Turn on the BIST clk at all time 
    HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_SND_SW_CLK_ON,0x1);

    //  //TODO::we may need freq switch to load SND  
    //  ddr_external_set_clk_speed(clk_freq_khz); 
    //  ddr_external_set_clk_speed(clk_freq_khz);   
    //
    //  
    //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x1);
    //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_ERR_CLR, 0x0);    
    //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, FSM_BIST_STATE_EN, 0x0);
    //  //HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG, BIST_TOP_CGC_CHK_SW_CLK_ON, 0x1);   
    //  HWIO_OUTXF (_inst_,DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG, BIST_START,0x1);  
}

uint32 DDR_PHY_hal_sta_wrlvl_training( uint32 _inst_ )
{

    //return ((HWIO_INXF (_inst_, DDR_PHY_DDRPHY_BISC_TRAINING_STA, TRAINING_STATUS) & 0xFF));
    return ((HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMHUB_STAT_1_STA, CBT_DQ_CAPTURE_STAT) & 0xFF));
  }


void DDR_PHY_hal_cfg_cdcext_wrlvl_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 fine_delay, uint32 coarse_delay )
{
  if (rank == 0) 
  {
    HWIO_OUTXF2 (_inst_+(period_index*4), 
                 DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                 HP_COARSE_DELAY_RANK_0,
                 HP_FINE_DELAY_RANK_0,
                 coarse_delay,
                 fine_delay
                 );    
  }
  else
  {
    HWIO_OUTXF2 (_inst_+(period_index*4), 
                 DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                 HP_COARSE_DELAY_RANK_1,
                 HP_FINE_DELAY_RANK_1,
                 coarse_delay,
                 fine_delay
                 );    
    }
    ///  write wrlvl delay value back to the corresponding cdc extension cfg
}

void DDR_PHY_hal_cfg_cdcext_wrlvl_ca_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 fine_delay, uint32 coarse_delay )
{
  uint32 RANK0_CK_WRLVL_COARSE0;
  if (rank == 0) 
  {
    HWIO_OUTXF2 (_inst_+(period_index*4), 
                 DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                 HP_COARSE_DELAY_RANK_0,
                 HP_FINE_DELAY_RANK_0,
                 coarse_delay,
                 fine_delay
                 );    
  }
  else
  {
    RANK0_CK_WRLVL_COARSE0 = HWIO_INXF (_inst_+(period_index*4), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, HP_COARSE_DELAY_RANK_0);
    if (RANK0_CK_WRLVL_COARSE0 > coarse_delay)
    {
        HWIO_OUTXF2 (_inst_+(period_index*4), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                 HP_COARSE_DELAY_RANK_1, HP_FINE_DELAY_RANK_1,
                 RANK0_CK_WRLVL_COARSE0, fine_delay
                 );   
    }
    else
    {
        HWIO_OUTXF2 (_inst_+(period_index*4), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                     HP_COARSE_DELAY_RANK_1, HP_FINE_DELAY_RANK_1,
                     coarse_delay, fine_delay
                     );   
        HWIO_OUTXF2 (_inst_+(period_index*4), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 
                     HP_COARSE_DELAY_RANK_0, HP_FINE_DELAY_RANK_0,
                     coarse_delay, fine_delay
                     );                        
    }
    }
    ///  write wrlvl delay value back to the corresponding cdc extension cfg
}

////  *********************************************************************
////  ----------------------------------------------------------------------
////  HAL           : RCW
////  Pretraining INIT
//// ----------------------------------------------------------------------
////  *********************************************************************

void DDR_PHY_RCW_pre_training_init (uint8 ch,  PHY_BUS dq)
{
    uint8 byte_lane              =  0;
    uint8 prfs_no                =  0;
    uint8 read_ie_ext_pre        =  0;
    uint8 opcode_read_lead_time  =  0;
    uint32 dq0_ddr_phy_base      =  0;
    uint32 read_oe_des_val       =  0;




    dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;	
	
    for (byte_lane = 0; byte_lane < dq; byte_lane++) 
	{
		for(prfs_no = 0; prfs_no <= MAX_TRAINING_FREQ_INDEX; prfs_no++)
		{
			read_oe_des_val = HWIO_INXF (dq0_ddr_phy_base +( byte_lane * DDR_PHY_OFFSET)+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, READ_OE_DES_VAL);
			read_ie_ext_pre = 0;
			opcode_read_lead_time = read_oe_des_val + 1;
			HWIO_OUTXF2 (dq0_ddr_phy_base +( byte_lane * DDR_PHY_OFFSET)+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, OPCODE_READ_LEAD_TIME_VAL, READ_IE_EXT_PRE_VAL,opcode_read_lead_time, read_ie_ext_pre);
		}
	}
}

//// ***********************************************************************
///// ----------------------------------------------------------
/////  HAL           : RCW
/////  status        : 1: enable rcw training, 0: disable
///// ----------------------------------------------------------
//// ***********************************************************************  
void DDR_PHY_hal_cfg_rcw (uint32 _inst_, RCW_EN_DIS rcw_setting)
{
	switch(rcw_setting)
	{
		//Enable PHY for RCW training
		case RCW_ENA :{
			//Enable PHY for RCW training
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_2_CFG, RCW_EN, 1);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, RCW_TRAINING_EN, 1);
			//Open the FPM latches for RCW CDC and QFI
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_2_CFG, RCW_COARSE_CTL, 0);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 1);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 0);
		break;
		}
		//Exit  RCW training
		case RCW_DIS :{
			//HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, RCW_TYPE_SEL, 0);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_2_CFG, RCW_EN, 1);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, RCW_TRAINING_EN, 0);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_2_CFG, RCW_COARSE_CTL, 0);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 1);
			HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 0);		
		break;
		}
	}
}

//************************************************************************************
//------------------------------------------------------------------------------------
//Sets the half/ full cycles 
//------------------------------------------------------------------------------------
//************************************************************************************
void DDR_PHY_hal_cfg_full_rcw(uint32 _inst_, uint8 full_cycle)
{
	/////***************************Half Cycle setting*********************************
	//////**************************Full cycle setting***********************************
	HWIO_OUTXF(_inst_, DTC_RDAW_CFG, RDAW_EXTEND, full_cycle); 
}

// ***********************************************************************
/// ----------------------------------------------------------
///  Read valude of rcw fine delay
/// ----------------------------------------------------------
// ***********************************************************************
uint8 DDR_PHY_hal_sta_rcw_fine( uint32 _inst_, uint8 rank )
{

    switch (rank) 
	{
    	case 0 : return (uint8)((HWIO_INX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG ) & 0x3E0) >> 5); //rank0 9:5
      break;
      case 1 : return (uint8)((HWIO_INX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG ) & 0xF8000) >> 15); //0x7C00) >> 15));// rank1 19:15
      break;
      default: return 0;
    }
}

// ***********************************************************************
/// ----------------------------------------------------------
///  Read value of rcw coarse delay
/// ----------------------------------------------------------
// ***********************************************************************
uint8 DDR_PHY_hal_sta_rcw_coarse( uint32 _inst_, uint8 rank )
{

    switch (rank) {
		case 0 : return (uint8)((HWIO_INX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG ) & 0x1F) >> 0);//rank0 4:0
    break;
    case 1 : return (uint8)((HWIO_INX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG ) & 0x7C00) >> 10); // rank1 14 : 10
		break;        
    default: return 0;
    }
}

// ***********************************************************************
/// ----------------------------------------------------------
///  Read value of number of half cycle
/// ----------------------------------------------------------
// ***********************************************************************

uint8 DDR_PHY_hal_sta_rcw_num_cycle( uint32 _inst_ )
{
    //uint8 half;
    uint8 full;
     
    // half = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_RD_RANK_EN_CFG, RD_EN_HALF_CYCLE_R0);
    // full = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_RD_RANK_EN_CFG, RD_EN_FULL_CYCLE_R0);
    	/////***************************Half Cycle setting*********************************
	//////**************************Full cycle setting***********************************
	full=HWIO_INXF(_inst_, DTC_RDAW_CFG, RDAW_EXTEND); 
	return (full);
}


  
// ***********************************************************************
/// ----------------------------------------------------------
///  HAL : CONFIG RCW RD SLAVE
/// ----------------------------------------------------------
// ***********************************************************************
void DDR_PHY_hal_cfg_cdc_slave_rcw( uint32 _inst_, uint32 delay, uint16 coarse_fine_delay, uint16 mode )
{
    uint32 cdc;
    uint32 mask;
    uint32 shift;
    // Read the CDC delay register
    cdc = HWIO_INX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG);
    mask  = DDR_PHY_hal_cfg_cdc_mask(coarse_fine_delay, 0, mode); // At 1017MHz rank ID is not enabled so training is done only rank0 bit fields.
    shift = DDR_PHY_hal_cfg_cdc_shift(coarse_fine_delay, 0, mode);
    // Clear space for delay in CDC
    cdc = cdc & mask;
    // Shift delay to insert in CDC
    delay = delay<<shift;
    // Insert delay into CDC
    cdc = cdc | delay;
    // Write CDC delay register 
    HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_CTL_CFG, cdc); 
    // Disable load 
        // Gate the clock
        // Disable the register
    HWIO_OUTXF3 (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_TOP_CFG, EN, LOAD, GATE, 0, 0, 1);
        // Pulse load
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_TOP_CFG, LOAD, 1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_TOP_CFG, LOAD, 0);

// 	HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 1);
//  HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 0);
        // Enable the register
        // Un-Gate the clock
    HWIO_OUTXF2 (_inst_, DDR_PHY_DDRPHY_CMCDCRCW_TOP_CFG, GATE, EN, 0, 1);
}

 // ***********************************************************************
 /// ----------------------------------------------------------
 ///  HAL			: RCW
 ///  status		: 1: RCW done, 0: RCW not done
 /// ----------------------------------------------------------
 // ***********************************************************************
 void DDR_PHY_rd_mon_status( uint32 _inst_, uint8 *pass )
 {

 	 *pass = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMHUB_STAT_1_STA, RCW_MON );
	  
 }

 //***************************************************************************
 /// ------------------------------------------------------------
 ///  HAL           : RCW
 ///  Update RCW EXT registers
 /// ------------------------------------------------------------
 //***************************************************************************
 void DDR_PHY_hal_cfg_rcw_QFI_ext( uint32 _inst_, uint32 tDQSCK_in_full_cycle, uint16 prfs_no )
{
  //uint32 reg;
  //uint32 shift;
  //uint32 mask;
	uint32 opcode_read_lead_time=0;
	uint32 read_ie_ext_pre=0;
	uint32 read_oe_des_val=0;


	read_oe_des_val = HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, READ_OE_DES_VAL);
	if ((read_oe_des_val+tDQSCK_in_full_cycle+1)<8)
	{
		read_ie_ext_pre = tDQSCK_in_full_cycle;
		opcode_read_lead_time = read_oe_des_val + tDQSCK_in_full_cycle +1;
	}
	else
	{
		//opcode_read_lead_time=7;
		//read_ie_ext_pre=6;
		
        ddr_printf(DDR_ERROR,"\nDDR ABORT: RCW FULL CYCLE SATURATED\n");
        ddr_abort();

	}
	HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, OPCODE_READ_LEAD_TIME_VAL, READ_IE_EXT_PRE_VAL,opcode_read_lead_time, read_ie_ext_pre);
}  
 //***************************************************************************
 /// ------------------------------------------------------------
 ///  HAL           : RCW
 ///  Update RCW EXT registers
 /// ------------------------------------------------------------
 //***************************************************************************
 void DDR_PHY_hal_cfg_rcw_QFI_ext_V2( uint32 _inst_, DDR_CHIPSELECT populated_chipselect,uint8 tDQSCK_perCS_full_cycle[NUM_CS] ,uint16 prfs_no )
{
	uint32 opcode_read_lead_time=0;
	uint32 read_ie_ext_pre=0;
	uint32 read_oe_des_val;
    uint8  best_tDQSCK_in_full_cycle = 0;
    uint8  full_cycle_diff = 0;
    uint8  rank_adj = 0;
    best_tDQSCK_in_full_cycle = (populated_chipselect == DDR_CS_BOTH )? ((tDQSCK_perCS_full_cycle[0] < tDQSCK_perCS_full_cycle[1]) ? 
        					                                                                       tDQSCK_perCS_full_cycle[0] : tDQSCK_perCS_full_cycle[1]) 
        					                                      : tDQSCK_perCS_full_cycle[0] ;
	read_oe_des_val = HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, READ_OE_DES_VAL);
	if ((read_oe_des_val+best_tDQSCK_in_full_cycle+1)<8)
	{
		read_ie_ext_pre = best_tDQSCK_in_full_cycle;
		opcode_read_lead_time = read_oe_des_val + best_tDQSCK_in_full_cycle +1;
	}
	else
	{
        ddr_printf(DDR_ERROR,"\nDDR ABORT: RCW FULL CYCLE SATURATED\n");
        ddr_abort();
	}
    HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, OPCODE_READ_LEAD_TIME_VAL, READ_IE_EXT_PRE_VAL,opcode_read_lead_time, read_ie_ext_pre);
    if(populated_chipselect == DDR_CS_BOTH)
    {
        //find the difference in the full_cycle between the 2 ranks.
        full_cycle_diff = (tDQSCK_perCS_full_cycle[1]  > tDQSCK_perCS_full_cycle[0]) ? 
                          (tDQSCK_perCS_full_cycle[1] - tDQSCK_perCS_full_cycle[0]) : (tDQSCK_perCS_full_cycle[0] - tDQSCK_perCS_full_cycle[1]);
        if(full_cycle_diff > 7)
        {
             ddr_printf(DDR_ERROR,"\nDDR ABORT: RCW RANK FULL CYCLE DELTA OVERFLOW\n");
             ddr_abort();
        }
        //find the rank which is delay than the other.
        rank_adj = (tDQSCK_perCS_full_cycle[1] > tDQSCK_perCS_full_cycle[0]) ? 1: 0;
    
        if (rank_adj == 0)// if the rank 0 needs adjmnt
        {
        HWIO_OUTXF(_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, READ_EN_EXT_PRE_RANK0_VAL, full_cycle_diff);

        }
        else //// if the rank 1 needs adjmnt
        {
        HWIO_OUTXF(_inst_+(prfs_no*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_2_CFG, READ_EN_EXT_PRE_RANK1_VAL, full_cycle_diff); 
        }  
    }
}  

void DDR_PHY_hal_cfg_wrlvl_half( uint32 _inst_, uint32 rank, uint32 dq_half_cycle )
{

  if (rank == 1)
  {
      HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_1_CFG, DQ_HALF_CYCLE_R1, dq_half_cycle);
  }
  else
  {
      HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_1_CFG, DQ_HALF_CYCLE_R0, dq_half_cycle);
  }
  HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 0x1);
  HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG, TRIG_WRLVL_LATCH, 0x0);

}

void DDR_PHY_hal_cfg_wrlvl_full( uint32 _inst_, uint32 rank, uint32 dq_full_cycle )
{

  if (rank == 1) {
      HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK1_VAL, dq_full_cycle);
  } else {
      HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK0_VAL, dq_full_cycle);
  }
  //DQS
  //DDRPHY_QFI2_CNTL_0_CFG[WRITE_EN_CYCLE_DLY_RANK0_VAL]
  //DDRPHY_QFI2_CNTL_0_CFG[WRITE_EN_CYCLE_DLY_RANK1_VAL]  
  
}

void DDR_PHY_hal_cfg_cdc_slave_wr( uint32 _inst_, uint32 delay, uint16 coarse_fine, uint16 hp_mode, uint8 rank )
{

    if (rank == 0) {
            if (coarse_fine == 1) { //[4:0]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_0, delay); 
            }
            else { // fine delay [9:5]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_0, delay); 
            }
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0x1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0x0);
    }
    else {
            if (coarse_fine == 1) { // [14:10]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_1, delay); 
            } 
            else { // Fine delay [19:15]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_1, delay);  
            }
        //Load CDC
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0x1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0x0);
    } 
}

void DDR_PHY_hal_cfg_cdc_slave_wr_b( uint32 _inst_,  uint8 rank, uint32 coarse,uint32 fine )
{
    if (rank == 0)
    {
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_0, coarse);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_0  , fine  );
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0x1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R0_RDY, 0x0);
    }
    else
    {
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_1, coarse);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_1  , fine  );
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0x1);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, R1_RDY, 0x0);
    }
}

void DDR_PHY_hal_get_cdc_slave_wr_b( uint32 _inst_,  uint8 rank, uint32 *coarse,uint32 *fine )
{
    if (rank == 0)
    {
        *coarse = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_0);
        *fine   = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_0  );
    }
    else
    {
        *coarse = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_COARSE_DELAY_RANK_1);
        *fine   = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCWR_CTL_CFG, HP_FINE_DELAY_RANK_1  );
    }
}
//// ***********************************************************************
///// ----------------------------------------------------------
/////  HAL : CONFIG RD SLAVE
///// ----------------------------------------------------------
//// ***********************************************************************
void DDR_PHY_hal_cfg_cdc_slave_rd( uint32 _inst_, uint32 delay, uint16 coarse_fine, uint16 hp_mode, uint8 rank )
{

    if (rank == 0) {
            if (coarse_fine == 1) { //[4:0]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_0, delay); 
            }
            else { // fine delay [9:5]
               HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_0, delay); 
            }
        //Load CDC
    }
    else {
            if (coarse_fine == 1) { // [14:10]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_1, delay); 
            } 
            else { // Fine delay [19:15]
                HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_1, delay);  
            }
    }
        //Load CDC
    HWIO_OUTXF3 (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, EN, LOAD, GATE, 0, 0, 1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 0);
    HWIO_OUTXF2 (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, GATE, EN, 0, 1);
    } 
void DDR_PHY_hal_cfg_cdc_slave_rd_b( uint32 _inst_, uint8 rank, uint32 coarse,  uint32 fine )
{
    if (rank == 0)
    {
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_0, coarse);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_0  , fine  );
    }
    else
    {
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_1, coarse);
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_1  , fine  );
    }
    HWIO_OUTXF3 (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, EN, LOAD, GATE, 0, 0, 1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, LOAD, 0);
    HWIO_OUTXF2 (_inst_, DDR_PHY_DDRPHY_CMCDCRD_TOP_CFG, GATE, EN, 0, 1);
}
void DDR_PHY_hal_get_cdc_slave_rd_b( uint32 _inst_, uint8 rank, uint32 *coarse,  uint32 *fine )
{
    if (rank == 0)
    {
        *coarse = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_0 );
        *fine   = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_0   );
    }
    else
    {
        *coarse = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_COARSE_DELAY_RANK_1);
        *fine   = HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMCDCRD_CTL_CFG, HP_FINE_DELAY_RANK_1  );
    }
}

void DDR_PHY_hal_cfg_pbit_dq_delay( uint32 _inst_, uint16 dq_num, uint16 txrx, uint16 sel, uint16 delay )
{
    uint32 temp = 0;
    uint32 shift = 0;
    uint32 mask = 0;
    
    shift = (0x5 * sel);
    mask = (~(0x1F << shift));
  
    /// select source of rank select b/w memory controller and csr default is from memory controller
    /// DDRPHY_CMHUB_TOP_0_CFG.BIT_RANK_SRC_SEL = rank_src_sel;
 
    switch(txrx) 
    {  
      case 1  : {          
                  temp = HWIO_INXF(_inst_+(dq_num*4), DDR_PHY_DDRPHY_CMSLICE0_PBIT_CTL_DQ_0_RXTX_CFG, TX0_CTL);
                  temp = ((delay << shift) | (mask & temp));
                  HWIO_OUTXF (_inst_+(dq_num*4), DDR_PHY_DDRPHY_CMSLICE0_PBIT_CTL_DQ_0_RXTX_CFG, TX0_CTL, temp);
                  break;
                }
      case 0  :  
                {
                  temp = HWIO_INXF(_inst_+(dq_num*4), DDR_PHY_DDRPHY_CMSLICE0_PBIT_CTL_DQ_0_RXTX_CFG, RX0_CTL);
                  temp = ((delay << shift) | (mask & temp));
                  HWIO_OUTXF (_inst_+(dq_num*4), DDR_PHY_DDRPHY_CMSLICE0_PBIT_CTL_DQ_0_RXTX_CFG, RX0_CTL, temp);
                  break;
                }
    }
}

/*  ************************************************************
    DDRPHY Write DQS IO DCC training
    ------------------------------------------------------------
    This API update the Write DQS IO DCC manual adj values
    ************************************************************
*/
void DDR_PHY_hal_cfg_wr_dqs_dcc_adj( uint32 _inst_, uint8 rank, uint16 wr_dqs_dcc_adj)
{
    if (rank == 0)
    {
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_DCC_TOP_0_CFG, IO_MODE, 0x0);// enable manual IO_DCC adjustment 
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_DCC_TOP_1_CFG, IO_MAN_ADJ, wr_dqs_dcc_adj); // program IO_DCC adjustment value
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_DCC_TOP_0_CFG, IO_MODE, 0x1);// disable manual IO_DCC adjustment 
    }
    else 
    {
        HWIO_OUTXF2 (_inst_, DDR_PHY_DDRPHY_DCC_TOP_6_CFG, RANK_SEL_BYP, RANK_SEL,  0x1 , 0x1 ); // enable SW rank_sel overide and select rank 1
        HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_DCC_TOP_6_CFG, DCC_ADJ_IODQS_R1, wr_dqs_dcc_adj); // program IO_DCC adjustment value
        HWIO_OUTXF2 (_inst_, DDR_PHY_DDRPHY_DCC_TOP_6_CFG, RANK_SEL_BYP, RANK_SEL,  0x0 , 0x0 ); // disable SW rank_sel overide and select rank 1
        
    }
}

void DDR_PHY_hal_cfg_cdcext_slave_rd( uint32 _inst_, uint8 rank, uint8 coarse_delay, uint8 fine_delay, uint8 prfs_no )
{ 
  if (rank == 0)
  {
    HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,
                 HP_COARSE_DELAY_RANK_0,
                 HP_FINE_DELAY_RANK_0,
                 coarse_delay,
                 fine_delay);
  }
  else
  {
    HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,
                 HP_COARSE_DELAY_RANK_1,
                 HP_FINE_DELAY_RANK_1,
                 coarse_delay,
                 fine_delay);    
  }
    
}
void DDR_PHY_hal_get_cdcext_slave_rd( uint32 _inst_, uint8 rank, uint8 *coarse_delay, uint8 *fine_delay, uint8 prfs_no )
{ 
  if (rank == 0)
  {
    *coarse_delay=HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,HP_COARSE_DELAY_RANK_0);
    *fine_delay  =HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,HP_FINE_DELAY_RANK_0  );
  }
  else
  {
    *coarse_delay=HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,HP_COARSE_DELAY_RANK_1);
    *fine_delay  =HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_RD_0_CTL_CFG,HP_FINE_DELAY_RANK_1  );
  }
}


void DDR_PHY_hal_cfg_cdcext_slave_wr( uint32 _inst_, uint8 rank, uint8 coarse_delay, uint8 fine_delay, uint8 prfs_no )
{         
  if (rank == 0)
  {
    HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,
                 HP_COARSE_DELAY_RANK_0,
                 HP_FINE_DELAY_RANK_0,
                 coarse_delay,
                 fine_delay);
  }
  else
  {
    HWIO_OUTXF2 (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,
                 HP_COARSE_DELAY_RANK_1,
                 HP_FINE_DELAY_RANK_1,
                 coarse_delay,
                 fine_delay);    
  }          
}

void DDR_PHY_hal_get_cdcext_slave_wr( uint32 _inst_, uint8 rank, uint8 *coarse_delay, uint8 *fine_delay, uint8 prfs_no )
{         
  if (rank == 0)
  {
    *coarse_delay=HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,HP_COARSE_DELAY_RANK_0);
    *fine_delay  =HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,HP_FINE_DELAY_RANK_0  );
  }
  else
  {
    *coarse_delay=HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,HP_COARSE_DELAY_RANK_1);
    *fine_delay  =HWIO_INXF (_inst_+(prfs_no*4), DDR_PHY_DDRPHY_CDCEXT_WR_0_CTL_CFG,HP_FINE_DELAY_RANK_1  );
  }
}

void DDR_PHY_hal_cfg_wrext_ctl_update( uint32 _inst_, uint8 period_index, uint8 rank, uint32 half_cycle )
{

    ///  wrlvl extension retmr and delay values
    switch(rank)
    {
        case 1  :
        {
            HWIO_OUTXF (_inst_+(period_index*4), DDR_PHY_DDRPHY_WRLVLEXT_CTL_0_CFG, DQ_HALF_CYCLE_R1, half_cycle);
            break;
        }
        case 0 :
        {
            HWIO_OUTXF (_inst_+(period_index*4), DDR_PHY_DDRPHY_WRLVLEXT_CTL_0_CFG, DQ_HALF_CYCLE_R0, half_cycle);
            break;
        }
    }
}

uint8 DDR_PHY_hal_get_wrext_ctl_update( uint32 _inst_, uint8 period_index, uint8 rank )
{
    uint8 half_cycle = 0;
        switch(rank)
        {
            case 1  :
            {
                half_cycle = HWIO_INXF (_inst_+(period_index*4), DDR_PHY_DDRPHY_WRLVLEXT_CTL_0_CFG, DQ_HALF_CYCLE_R1);
                break;
            }
            case 0 :
            {
                half_cycle = HWIO_INXF (_inst_+(period_index*4), DDR_PHY_DDRPHY_WRLVLEXT_CTL_0_CFG, DQ_HALF_CYCLE_R0);
                break;
            }
        }
		return(half_cycle);
}







void DDR_PHY_hal_cfg_wr_full_cycle( uint32 _inst_, uint8 prfs_index, uint8 rank, uint32 full_cycle )
{

          switch(rank) {
          case 1  : {
                HWIO_OUTXF (_inst_+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK0_VAL, full_cycle);
                break;
            }
         case 0 : {
                HWIO_OUTXF (_inst_+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK1_VAL, full_cycle);
                break;
            }
       }
}


uint8 DDR_PHY_hal_get_wr_full_cycle( uint32 _inst_, uint8 rank, uint8 prfs_index )
{
    uint8 full_cycle = 0;
          switch(rank) {
          case 1  : {
                full_cycle = HWIO_INXF (_inst_+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK0_VAL);
                break;
            }
         case 0 : {
                full_cycle = HWIO_INXF (_inst_+(prfs_index*4), DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_0_CFG, WRITE_DATA_CYCLE_DLY_RANK1_VAL);
                break;
            }
       }
       return(full_cycle);
}
void  DDR_PHY_hal_wr_io_dqsdcc_CSR (uint32 _inst_, uint8 rank,uint8 prfs_x, uint16 dcc_adj)
{
    if(rank==0)    

    {
        switch  (prfs_x)
        {
            case 0 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_0_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 1 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_1_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 2 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_2_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 3 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_3_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 4 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_4_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 5 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_5_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 6 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_6_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
            case 7 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_7_CTL_CFG, PADDCC_ADJ_DQS, dcc_adj); break;}
        }
    }
    else
    {  
        switch  (prfs_x)
        {
            case 0 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_0_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 1 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_1_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 2 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_2_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 3 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_3_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 4 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_4_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 5 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_5_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 6 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_6_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
            case 7 :{HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_PADDCC_7_CTL_CFG, PADDCC_ADJ_DQS_R1, dcc_adj); break;}
        }
    }
}  
void  DDR_PHY_hal_wr_t90_dq_dcc_CSR (uint32 _inst_, uint8 prfs_x, uint16 dcc_adj)
{
    switch  (prfs_x)
    {
        case 0 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_0_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 1 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_1_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 2 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_2_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 3 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_3_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 4 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_4_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 5 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_5_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 6 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_6_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
        case 7 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_7_CTL_CFG, CMDCC_ADJ_WR, dcc_adj); break;
    }
}  


void  DDR_PHY_hal_wr_wrlvl_dcc_CSR (uint32 _inst_, uint8 prfs_x, uint16 dcc_adj)
{
    switch  (prfs_x)
    {
        case 0 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_0_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 1 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_1_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 2 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_2_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 3 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_3_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 4 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_4_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 5 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_5_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 6 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_6_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
        case 7 :   HWIO_OUTXF(_inst_, DDR_PHY_DDRPHY_DCCEXT_CMDCC_7_CTL_CFG, CMDCC_ADJ_WRLVL, dcc_adj); break;
    }
}  
uint8  DDR_PHY_hal_get_wr_rank_update_en(uint32 _inst_, uint8 prfs_x)
{
    uint8 tmp=0xFF;

	switch (prfs_x)
	{
        case 0 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 1 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_1_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 2 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_2_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 3 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 4 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 5 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 6 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
        case 7 : {  tmp=HWIO_INXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,WR_RANK_UPDATE_EN);break;}
	}
   return(tmp);
}

void   DDR_PHY_hal_cfg_wr_rank_update_en(uint32 _inst_, uint8 prfs_x, uint8 wr_rank_update_en)
{
	switch (prfs_x)
	{
        case 0 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_0_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 1 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_1_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 2 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_2_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 3 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 4 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 5 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 6 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
        case 7 : {  HWIO_OUTXF( _inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG,WR_RANK_UPDATE_EN,wr_rank_update_en);break;}
	}
}

void Get_LUT_band_DDRCC (uint32 _inst_, uint32 freq, uint32 *DDRCC_prfs_indx_fmax)
{
    //uint8 prfs_indx = 6;
    uint32 PRFSLUT0 =0;
    uint32 PRFSLUT1 =0;
    uint32 PRFSLUT0_HI =0;
    uint32 PRFSLUT1_HI =0;
   
    PRFSLUT0 = HWIO_INX( _inst_, DDR_CC_DDRCC_FPM_PRFS_LUT0_CFG);
    PRFSLUT1 = HWIO_INX( _inst_, DDR_CC_DDRCC_FPM_PRFS_LUT1_CFG);
    
    PRFSLUT0_HI = HWIO_INX( _inst_, DDR_CC_DDRCC_FPM_PRFS_HI_LUT0_CFG);
    PRFSLUT1_HI = HWIO_INX( _inst_, DDR_CC_DDRCC_FPM_PRFS_HI_LUT1_CFG);
    
    
    DDRCC_prfs_indx_fmax[0]=1000000000/((((((PRFSLUT0_HI)&(0xFF))>>0)<<8)|(((PRFSLUT0)&(0xFF))>>0))*2);
    DDRCC_prfs_indx_fmax[1]=1000000000/((((((PRFSLUT0_HI)&(0xFF00))>>8)<<8)|(((PRFSLUT0)&(0xFF00))>>8))*2);
    DDRCC_prfs_indx_fmax[2]=1000000000/((((((PRFSLUT0_HI)&(0xFF0000))>>16)<<8)|(((PRFSLUT0)&(0xFF0000))>>16))*2);
    DDRCC_prfs_indx_fmax[3]=1000000000/((((((PRFSLUT0_HI)&(0xFF000000))>>24)<<8)|(((PRFSLUT0)&(0xFF000000))>>24))*2);
                                   
    DDRCC_prfs_indx_fmax[4]=1000000000/((((((PRFSLUT1_HI)&(0xFF))>>0)<<8)|(((PRFSLUT1)&(0xFF))>>0))*2);
    DDRCC_prfs_indx_fmax[5]=1000000000/((((((PRFSLUT1_HI)&(0xFF00))>>8)<<8)|(((PRFSLUT1)&(0xFF00))>>8))*2);
    DDRCC_prfs_indx_fmax[6]=1000000000/((((((PRFSLUT1_HI)&(0xFF0000))>>16)<<8)|(((PRFSLUT1)&(0xFF0000))>>16))*2);
    DDRCC_prfs_indx_fmax[7]=1000000000/((((((PRFSLUT1_HI)&(0xFF000000))>>24)<<8)|(((PRFSLUT1)&(0xFF000000))>>24))*2);
    
    //for (int i=0; i < 8; i++)
    //{
    //    ddr_printf(DDR_NORMAL,"++EXPECTED:   PRFS_FMAX: %d Mhz\n", DDRCC_prfs_indx_fmax[i]); 
    //}
}

