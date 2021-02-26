/****************************************************************************
 QUALCOMM Proprietary Design Data
 Copyright (c) 2017, Qualcomm Technologies Incorporated. All rights reserved.
 ****************************************************************************/
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/DSFTargetLib/ddrsns/ddrss/src/ddrss_phy_cc_init.c#4 $
$DateTime: 2019/08/01 06:25:20 $
$Author: pwbldsvc $
================================================================================
when       who          what, where, why
--------   ---          --------------------------------------------------------
05/30/14   jeasley      Create separate CA and DQ PHY settings routines
05/20/14   jeasley      Correct SW handshake and Broadcast
05/08/14   jeasley      Move initialization of polling variables to the inside of the loop.
05/06/14   jeasley      Invert the polarity of sw_handshake_complete poll.
05/06/14   jeasley      Updated DDR_PHY_CC_init to use broadcast.
05/04/14   arindamm     First edit history header. Add new entries at top.
================================================================================*/

#include "ddrss.h"
#include "ddrss_mccc.h"

//================================================================================================//
// Pre training set-up
//================================================================================================//

uint32   reg_offset_ddr_phy;

//================================================================================================//
// DDR PHY and CC one-time settings
//================================================================================================//
//void DDR_PHY_CC_Config(BOOT_DDR_STRUCT *ddr, void *data)
//{   
//    uint32   dq0_ddr_phy_base; 
//    uint32   ca0_ddr_phy_base;
//    uint32   cc_ddr_phy_base;
//    uint8    ch;
//    uint8    byte_lane;
//    uint8    ca_lane;
//    PHY_BUS  dq     = DQ;
//    PHY_BUS  ca     = CA;
//    
//    for (ch = 0; ch < NUM_CH; ch++)
//    {
//        dq0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + DQ0_DDR_PHY_OFFSET;
//        ca0_ddr_phy_base  = REG_OFFSET_DDR_PHY_CH(ch) + CA0_DDR_PHY_OFFSET;        
//        
//        for (byte_lane=0;byte_lane<dq;byte_lane++)
//        {
//            DDRSS_set_config_base_delta(ddr,
//                dq0_ddr_phy_base + (byte_lane * DDR_PHY_OFFSET),
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_PHY_DQ_CFG_BASE_IDX]),
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_PHY_DQ_CFG_DELTA_IDX]));
//        }
//        
//        for (ca_lane=0;ca_lane<ca;ca_lane++)
//        {
//            DDRSS_set_config_base_delta(ddr, 
//                ca0_ddr_phy_base + (ca_lane * DDR_PHY_OFFSET),
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_PHY_CA_CFG_BASE_IDX]),
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_PHY_CA_CFG_DELTA_IDX]));
//        }
//        
//        cc_ddr_phy_base  =  REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;
//        
//        DDRSS_set_config_base_delta(ddr,
//                cc_ddr_phy_base,
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_CC_CFG_BASE_IDX]),
//                (uint32(*)[2])((size_t)data + ((uint16*)data)[DDR_CC_CFG_DELTA_IDX]));
//    }
//}

//================================================================================================//
// DDR PHY and CC one-time settings
//================================================================================================//
void DDR_PHY_CC_Config(BOOT_DDR_STRUCT *ddr)
{
    uint8 ch = 0, prfs_band = 0;
    uint32 reg_offset_ddr_phy;
    uint8 num_ch = ddr->shrm_ddr_ptr->misc.num_channel;
    uint8 ts_v1[NUM_CH] = {52, 53};  
    uint8 *ts_ptr = ts_v1;
    
    // Enable broadcast mode for all DQ PHYs on both channels
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_DQs);
   //Warlock does not contain PHY1 instance as it is dual channel
   //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
   //          AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_DQs);
    DDRSS_set_config(ddr, BROADCAST_BASE,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[DQ_PHY_CFG_IDX][0]));

    // Enable broadcast mode for all CA PHYs on both channels
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_CAs);
   //Warlock does not contain PHY1 instance as it is dual channel
   //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
   //          AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_CAs);
    DDRSS_set_config(ddr, BROADCAST_BASE,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[CA_PHY_CFG_IDX][0]));
        
    for (ch = 0; ch < num_ch; ch+=2)
    {
        reg_offset_ddr_phy = REG_OFFSET_DDR_PHY_CH(ch);
        HWIO_OUTX(reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, 0x1);
    }    

    // Enable broadcast mode for all CCs on both channels
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY0_CCs);
   //Warlock does not contain PHY1 instance as it is dual channel
   //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
   //          AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, ALL_PHY1_CCs);
    DDRSS_set_config(ddr, BROADCAST_BASE,
        (uint32(*)[2])((size_t)(ddr->dcb_ptr) + (*ddr->dcb_ptr)[DDR_CC_CFG_IDX][0]));

    // Disable broadcast mode 
    HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY0_AHB2PHY_SWMAN_OFFSET,
        AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, PHY_BC_DISABLE);
   //Warlock does not contain PHY1 instance as it is dual channel
   //HWIO_OUTX(SEQ_DDR_SS_DDRSS_AHB2PHY1_AHB2PHY_SWMAN_OFFSET,
   //          AHB2PHY_SWMAN_AHB2PHY_BROADCAST_EN_CFG_LOWER, PHY_BC_DISABLE);

    for (ch = 0; ch < NUM_CH; ch++)
    {
        reg_offset_ddr_phy = REG_OFFSET_DDR_PHY_CH(ch);
		HWIO_OUTXF(reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, ts_ptr[ch]);
		HWIO_OUTXF(reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, ts_ptr[ch]);
		HWIO_OUTXF(reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_QFI2EXT_PRFS_7_CNTL_0_CFG, TIMESTAMP_COUNTER_OFFSET, ts_ptr[ch]);

        for ( prfs_band = 0; prfs_band < LOWEST_WRLVL_COPY_PRFS; prfs_band++)
        {
            // WRLVL settings @GCC mode to offset QFI2.0 propagation delay
            HWIO_OUTX(reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET + (4 * prfs_band), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 0x1C07);
            HWIO_OUTX(reg_offset_ddr_phy + DQ0_DDR_PHY_OFFSET + (4 * prfs_band), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 0x0);
            HWIO_OUTX(reg_offset_ddr_phy + DQ1_DDR_PHY_OFFSET + (4 * prfs_band), DDR_PHY_DDRPHY_CDCEXT_WRLVL_0_CTL_CFG, 0x3C0F);
        }
    }   
}

//================================================================================================//
// DDR PHY and CC Initialization
//================================================================================================//
void DDR_PHY_CC_init (BOOT_DDR_STRUCT *ddr, DDR_CHANNEL channel)
{
   uint8 ch = 0;
   uint32 reg_offset_ddr_phy;
   uint8 num_ch           = ddr->shrm_ddr_ptr->misc.num_channel;
    uint32 reg_offset_ddrcc     = 0;
    DDR_CHIPSELECT qualified_cs = DDR_CS_NONE;
   channel = (DDR_CHANNEL) (channel & ddr->shrm_ddr_ptr->misc.broadcast_ch_en_mask);
      
  for (ch = 0; ch < num_ch; ch++)
   {
      reg_offset_ddr_phy = REG_OFFSET_DDR_PHY_CH(ch);
      
      if ((channel >> ch) & 0x1)
      {
            qualified_cs =  (DDR_CHIPSELECT)ddr->shrm_ddr_ptr->detected_ddr_device[ch].populated_chipselect;
            // Base pointer to DDR_CC
            reg_offset_ddrcc = REG_OFFSET_DDR_CC_CH(ch) + DDR_CC_OFFSET;
            if (ch%2 == 0)
            {
                //Even channles are assigned to Ch0 register
                HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_UPDATE_INTF_CFG0, UPDT_REQ_TYPE_CH0, qualified_cs);
            }
            else
            {
                //Odd channles are assigned to Ch1 register
                HWIO_OUTXF(reg_offset_ddrcc, DDR_CC_DDRCC_UPDATE_INTF_CFG0, UPDT_REQ_TYPE_CH1, qualified_cs);
            }
            
            if (ch%2 == 0)
            {
         // Set master CA PHY, enable automatic IO calibration, and perform manual IO calibration

           HWIO_OUTX(reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_UPDATE_INTF_CFG, 1);
           HWIO_OUTXF(reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET, DDR_PHY_DDRPHY_IOCTLR_CTRL_CFG, IO_CAL_AUTO, 1);
           DDR_PHY_hal_cfg_sw_iocal (reg_offset_ddr_phy + CA0_DDR_PHY_OFFSET);


      }
   }
}
}