//===========================================================================
//  Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
//  QUALCOMM Proprietary and Confidential. 
//===========================================================================
////////////////////////////////////////////////////////////////////////////////////////////////

#include "ddr_phy.h"
#include "ddrss.h"

#define OCR_WR_P0_R0_BL0	    0x00	   // CA/DQ
#define OCR_WR_P0_R0_BL1	    0x01	   // CA/DQ
#define OCR_WR_P0_R1_BL0	    0x02	   // CA/DQ
#define OCR_WR_P0_R1_BL1	    0x03	   // CA/DQ
#define OCR_RD_P0_R0_BL0	    0x04	   // DQ only
#define OCR_RD_P0_R0_BL1	    0x05	   // DQ only
#define OCR_RD_P0_R1_BL0	    0x06	   // DQ only
#define OCR_RD_P0_R1_BL1	    0x07	   // DQ only
#define CMD_CKE_ALL_OFF	        0x08	   // CA only
#define CMD_CKE1_OFF_CKE0_ON	0x09	   // CA only
#define CMD_CKE1_ON_CKE0_OFF	0x0A	   // CA only
#define CMD_CKE_ALL_ON	        0x0B	   // CA only
#define CMD_CK_OFF	            0x0C	   // CA only
#define CMD_CK_ON	            0x0D	   // CA only

#define OCR_WR_P1_R0_BL0	    0x10    // CA/DQ
#define OCR_WR_P1_R0_BL1	    0x11    // CA/DQ
#define OCR_WR_P1_R1_BL0	    0x12    // CA/DQ
#define OCR_WR_P1_R1_BL1	    0x13    // CA/DQ
#define OCR_RD_P1_R0_BL0	    0x14    // DQ only
#define OCR_RD_P1_R0_BL1	    0x15    // DQ only
#define OCR_RD_P1_R1_BL0	    0x16    // DQ only
#define OCR_RD_P1_R1_BL1	    0x17    // DQ only





//   CH0/1_DQ0/1_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG[PERIODIC_TRAINING_CLK_ON]=1'b1
//   CH0/1_DQ0/1_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG[PERIODIC_TRAINING_CLK_ON]=1'b1
//   CH0/1_DQ0/1_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG[PERIODIC_TRAINING_CLK_ON]=1'b1
//   CH0/1_DQ0/1_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG[PERIODIC_TRAINING_CLK_ON]=1'b1
//   CH0/1_DQ0/1_DDRPHY_UPDATE_INTF_CFG[PERIODIC_TRAINING_ACK_SEL]=1'b1
//   CH0_DQ0/1_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG=h000151
//   CH0_DQ0/1_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG=h000161
//   CH0_DQ0/1_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG=h000261
//   CH0_DQ0/1_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG=h000271
//   CH0_DQ0/1_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG=h000381
//   CH0/1_DQ0/1_DDRPHY_QFI2_CNTL_5_CFG[WPRE_OFF] = 1
//   CH0_DQ1_DDRPHY_FPM_PRFS_4_PWRS_0_CFG[7:6] = 0
//   CH0_DQ1_DDRPHY_FPM_PRFS_5_PWRS_0_CFG[7:6] = 0
//   CH0_DQ1_DDRPHY_FPM_PRFS_6_PWRS_0_CFG[7:6] = 0
//   CH0_DQ1_DDRPHY_FPM_PRFS_7_PWRS_0_CFG[7:6] = 0
//   
//       for (phy_x=0;phy_x<NUM_DQ_PCH;phy_x++)
//       {      
//   		
//   		
//   		// HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		// HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_1_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		// HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_2_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		// HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		//Periodic training
//   		HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
//   		
//   		HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_UPDATE_INTF_CFG, PERIODIC_TRAINING_ACK_SEL, 0x1);//added from  hpg
//   
//   		HWIO_OUTX  (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG, 0x000151);
//   		HWIO_OUTX  (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG, 0x000161);
//   		HWIO_OUTX  (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG, 0x000261);
//   		HWIO_OUTX  (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG, 0x000271);
//   		HWIO_OUTX  (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG, 0x000381);
//   	  
//   	    HWIO_OUTXF (dq_ddr_phy_base[phy_x], DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF    , 0x1);
//   
//   	}
//   	//bits 6,7
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_4_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_4_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_5_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_5_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_6_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_6_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_7_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
//   	HWIO_OUTXF (dq_ddr_phy_base[1], DDRPHY_FPM_PRFS_7_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);


void DDR_PHY_hal_cfg_ca_vref_dq_prep( uint32 _inst_ )
{
   		// HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_0_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		// HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_1_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		// HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_2_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		// HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_3_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		//Periodic training
   		   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_4_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_5_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_6_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_TOPCTRL1EXT_PRFS_7_CFG, PERIODIC_TRAINING_CLK_ON, 0x1);
   		   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, PERIODIC_TRAINING_ACK_SEL, 0x1);//added from  hpg
   		   HWIO_OUTX  (_inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_3_CNTL_1_CFG, 0x000151);
   		   HWIO_OUTX  (_inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_4_CNTL_1_CFG, 0x000161);
   		   HWIO_OUTX  (_inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_5_CNTL_1_CFG, 0x000261);
   		   HWIO_OUTX  (_inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_6_CNTL_1_CFG, 0x000271);
   		   HWIO_OUTX  (_inst_, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_1_CFG, 0x000381);
   	       HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG, WPRE_OFF    , 0x1);

}
void DDR_PHY_hal_cfg_ca_vref_ca_prep( uint32 _inst_ )
{

    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_4_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_5_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_6_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG,  DQ_ODT_ENABLE, 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_FPM_PRFS_7_PWRS_0_CFG, DQS_ODT_ENABLE, 0x0);

}
void DDR_PHY_hal_cfg_ca_vref_dq_out_entry( uint32 _inst_ )
{
	

// DDRPHY_CMIO_PAD_MODE_CFG[SW_PAD_MODE_DQS] = 1
// DDRPHY_CMIO_PAD_MODE_CFG[SW_PAD_MODE_DQ] = 0x3FF
// DDRPHY_CMIO_PAD_MODE_CFG[SW_PAD_ENABLE_OE_DQS] = 1
// DDRPHY_CMIO_PAD_MODE_CFG[SW_CA_PAD_OE_OVR_DQS] = 1
// DDRPHY_CMIO_PAD_MODE_CFG[SW_PAD_ENABLE_OE_DQ] = 0x3FF 

//    // Put DQ0 in OE mode
//    csr_write(WB_DQ0, "DDRPHY_CMIO_PAD_MODE_CFG",       '{"SW_PAD_MODE_DQS","SW_PAD_MODE_DQ"},'{'h1,'h3FF},.CH("CH0"));
//    csr_write(WB_DQ0, "DDRPHY_CMIO_PAD_OE_CFG",         '{"SW_CA_PAD_OE_OVR_DQS","SW_PAD_ENABLE_OE_DQS"},'{'h1,'h1},.CH("CH0"));
//    csr_write(WB_DQ0, "DDRPHY_CMIO_PAD_OE_CFG",         '{"SW_PAD_ENABLE_OE_DQS"}        ,   '{'h1},   .RMW(1));
//    csr_write(WB_DQ0, "DDRPHY_CMIO_PAD_OE_CFG",         '{"SW_PAD_ENABLE_OE_DQ"}         ,   '{'h3FF}, .RMW(1));

	
	
	
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQS     , 0x1  );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG, SW_PAD_MODE_DQ      , 0x3FF);
	
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQS, 0x1  );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_CA_PAD_OE_OVR_DQS, 0x1  );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG, SW_PAD_ENABLE_OE_DQ , 0x3FF);
}

void DDR_PHY_hal_cfg_ca_vref_dq_in_entry( uint32 _inst_ )
{
	
    // Put DQ1 in IE mode
//    csr_write(WB_DQ1, "DDRPHY_CMHUB_TOP_0_CFG",         '{"CA_TRAINING_MODE"}            ,   '{'h1},   .RMW(1));
//    csr_write(WB_DQ1, "DDRPHY_CMIO_PAD_MODE_CFG",       '{"SW_PAD_MODE_DQS"}             ,   '{'h1},   .RMW(1));
//    csr_write(WB_DQ1, "DDRPHY_CMIO_PAD_MODE_CFG",       '{"SW_PAD_MODE_DQ"}              ,   '{'h3FF}, .RMW(1));
//    csr_write(WB_DQ1, "DDRPHY_CMIO_PAD_IE_CFG",         '{"SW_PAD_ENABLE_IE_DQS"}        ,   '{'h1},   .RMW(1));
//    csr_write(WB_DQ1, "DDRPHY_CMIO_PAD_IE_CFG",         '{"SW_PAD_MODE_IE_DQ","SW_PAD_ENABLE_IE_DQ"},'{'h3FF,'h3FF},.CH("CH0"));
//    csr_write(WB_DQ1, "DDRPHY_PAD_DQ_0_CFG",            '{"ODT_EN"}                      ,   '{'h0},   .RMW(1));
//    csr_write(WB_DQ1, "DDRPHY_PAD_DQS_0_CFG",           '{"ODT_EN"}                      ,   '{'h0},   .RMW(1));
	
	
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG   , CA_TRAINING_MODE     , 0x1   );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG , SW_PAD_MODE_DQS      , 0x1   );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG , SW_PAD_MODE_DQ       , 0x3ff );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG , SW_PAD_ENABLE_IE_DQS , 0x1   );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG , SW_PAD_ENABLE_IE_DQ  , 0x3FF );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_IE_CFG , SW_PAD_MODE_IE_DQ    , 0x3FF );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_PAD_DQ_0_CFG      , ODT_EN               , 0x0   );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_PAD_DQS_0_CFG     , ODT_EN               , 0x0   );
}


void DDR_PHY_hal_cfg_ca_vref_dq_exit( uint32 _inst_ )
{
//dq
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG         , TIMESTAMP_COUNTER_OFFSET   , 0x33);
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG , BIST_TOP_CGC_SND_SW_CLK_ON , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_3_CFG         , WRITE_BURST_LENGTH_0_VAL   , 0x7 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG         , FSM_BIST_STATE_EN          , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG         , DATA_BUS_MASK_EN           , 0x1 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG       , SW_PAD_MODE_DQS            , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_MODE_CFG       , SW_PAD_MODE_DQ             , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG       , SW_PAD_ENABLE_OE_DQS       , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMIO_PAD_OE_CFG       , SW_PAD_ENABLE_OE_DQ        , 0x0 );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG         , CA_TRAINING_MODE           , 0x0 );//in hpg was 0x1
}	  
void DDR_PHY_hal_cfg_ca_vref_ca_exit( uint32 _inst_ )
{	  
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_0_CFG         , TIMESTAMP_COUNTER_OFFSET   , 0x33  );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_TOP_CGC_ACTIVE_CFG , BIST_TOP_CGC_SND_SW_CLK_ON , 0x0   );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_3_CFG         , WRITE_BURST_LENGTH_0_VAL   , 0x1   );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG         , FSM_BIST_STATE_EN          , 0x0   );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_5_CFG         , DATA_BUS_MASK_EN           , 0x1   );
   HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_4_CFG         , DATA_SLICE_CKE_EN          , 0x300 );
  // HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_CMHUB_TOP_0_CFG         , CA_TRAINING_MODE           , 0x0   );//in DV was 0x1
}	  	  

void DDR_PHY_hal_BIST_CKE_ON( uint32 _inst_ )
{
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_0, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_1, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_2, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_3, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_0, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_1, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_2, CMD_CKE_ALL_ON);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_3, CMD_CKE_ALL_ON);
	
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
}

void DDR_PHY_hal_BIST_CKE_OFF( uint32 _inst_ )
{
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_0, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_1, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_2, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_3, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_0, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_1, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_2, CMD_CKE_ALL_OFF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_3, CMD_CKE_ALL_OFF);
	
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
}



void DDR_PHY_hal_BIST_cfg_Vref(uint32 _inst_ , uint32 vref )
{
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_0, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_1, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_2, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG, OPCODE_3, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_0, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_1, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_2, OCR_WR_P0_R0_BL0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG, OPCODE_3, OCR_WR_P0_R0_BL0);

    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_1_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_2_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_3_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_4_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_5_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_6_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_7_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_8_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_9_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_10_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_11_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_12_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_13_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_14_CFG, DATA, vref);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_15_CFG, DATA, vref);
	
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
	//HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
}


void   DDR_PHY_hal_BIST_cfg_CA_Pattern_ca (uint32 _inst_ ,uint8  ca_trn_pat[3])
{
//6 Program BIST 
                           

//   Program WRITE operation to CA:
//   CH0/1_CA_DDRPHY_BIST_OPCODE_LUT_x_CFG[OPCODE_x] = OCR_WR_P0_R0_BL0
//   CH0/1_CA_DDRPHY_BIST_USR_DATA_LUT_0_CFG[DATA] = 0x5
//   CH0/1_CA_DDRPHY_BIST_USR_DATA_LUT_1_CFG[DATA] = 0xA
//   CH0/1_CA_DDRPHY_BIST_USR_DATA_LUT_2_CFG[DATA] = 0x5
//   Configure BIST mode:
//   CH0/1_CA/DQ0/1_DDRPHY_BIST_CTRL_CFG[BIST_MODE] = 0x1
//   CH0/1_CA/DQ0/1_DDRPHY_BIST_CTRL_CFG[BIST_LOOP_CNT_LOAD_VAL] = 0x1
//   CH0/1_DQ0_DDRPHY_BIST_CTRL_CFG[BIST_DATA_DLY_CNT_LOAD_VAL] = 4
//   CH0/1_CA_DDRPHY_BIST_CTRL_CFG[BIST_DATA_DLY_CNT_LOAD_VAL] = 0
//   CH0/1_DQ0/CA_DDRPHY_BIST_CTRL_CFG[BIST_OPCODE_INT_CNT_LOAD_VAL] = 0xF
//   DDRPHY_QFI2_CNTL_4_CFG[DATA_SLICE_CKE_EN ] = 0

//   Program WRITE operation to CA:

    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG  , OPCODE_0,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG  , OPCODE_1,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG  , OPCODE_2,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_0_CFG  , OPCODE_3,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG  , OPCODE_0,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG  , OPCODE_1,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG  , OPCODE_2,  OCR_WR_P0_R0_BL0      );
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_OPCODE_LUT_1_CFG  , OPCODE_3,  OCR_WR_P0_R0_BL0      );
	
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_0_CFG       , DATA    ,  ca_trn_pat[0]);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_1_CFG       , DATA    ,  ca_trn_pat[1]);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_USR_DATA_LUT_2_CFG       , DATA    ,  ca_trn_pat[2]);
    // Configure BIST mode:
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_MODE                   , 0x1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_LOOP_CNT_LOAD_VAL      , 0x1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_DATA_DLY_CNT_LOAD_VAL  , 0x0);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_OPCODE_INT_CNT_LOAD_VAL, 0xF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_4_CFG, DATA_SLICE_CKE_EN           , 0x0);
}



void   DDR_PHY_hal_BIST_cfg_CA_Pattern_b0 (uint32 _inst_ )
{
    // Configure BIST mode:
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_MODE                   , 0x1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_LOOP_CNT_LOAD_VAL      , 0x1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_DATA_DLY_CNT_LOAD_VAL  , 0x4);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_OPCODE_INT_CNT_LOAD_VAL, 0xF);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_QFI2_CNTL_4_CFG, DATA_SLICE_CKE_EN           , 0x0);
}

void   DDR_PHY_hal_BIST_cfg_CA_Pattern_b1 (uint32 _inst_ )
{
    // Configure BIST mode:
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_MODE                   , 0x1);
    HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_CFG  , BIST_LOOP_CNT_LOAD_VAL      , 0x1);
}



// void DDR_PHY_hal_Send_CA_Pattern( uint32 _inst_ )
// {
// 	HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
// 	HWIO_OUTX (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
// }
void DDR_PHY_hal_Send_BIST_START( uint32 _inst_ )
{

	HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x1 );
	HWIO_OUTXF (_inst_, DDR_PHY_DDRPHY_BIST_CTRL_ACTIVE_CFG , BIST_START, 0x0 );
}

// ***********************************************************************
/// DDRPHY BISC STA READ
/// seq_msg(INFO, 0, "DDRPHY CA VREF STA READ");
/// -----------------------------------------------------------------
///     HALs for BISC STA READ
/// -----------------------------------------------------------------
// ***********************************************************************
uint32 DDR_PHY_hal_sta_ca_vref( uint32 _inst_ )
{

    ///  only the lower 6 bits are feedback data from DRAM for CA training
    ///  but due to bit remapping, all 10 bits need to be saved. Then the
    ///  correct remap needs to take place before the comparison can happen.
  return ((HWIO_INXF (_inst_, DDR_PHY_DDRPHY_CMHUB_STAT_1_STA, CBT_DQ_CAPTURE_STAT) & 0x3FF));
}





