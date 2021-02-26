/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalMdssHdmiOutput.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHalMdssHdmiOutput.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "AFEHalMdssHdmiOutput.h"
#include "common.h"

/*=====================================================================
 Globals/Macros
 ======================================================================*/
//static uint32_t hdmi_input_phy_base;
//static int32_t  hdmi_input_virt_offset;

//#define HDMI_INPUT_OFFSET(_reg_) ((_reg_)-LPASS_HDMIRX_VERSION)


#define MMSS_HDMI_BASE_ADDR           (0x009A0000)   //Istari HDMI CORE base address

//MMSS_HDMI_CTRL
#define HDMI_CTRL_OFFSET 0x0

//MMSS_HDMI_AUDIO_PKT_CTRL
#define HDMI_AUDIO_PKT_CTRL_OFFSET   0x20
#define HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND__ENABLE    1
#define HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND__DISABLE    0
#define HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___M        0x1
#define HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___S        0

#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE__ON     1
#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE__OFF    0
#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___M 0x04000000
#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___S 26

#define HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE__DEFAULT    0
#define HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE__LPASS      1
#define HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___M 0x08000000
#define HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___S 27


//MMSS_HDMI_INFOFRAME_CTRL0
#define HDMI_INFOFRAME_CTRL0_OFFSET 0x2C
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND__ENABLE    1
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND__DISABLE    0
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___M        0x10
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___S        4

#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__ENABLE     1
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__DISABLE    0
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___M        0x80
#define HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___S        7


#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND__ENABLE    1
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND__DISABLE    0
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___M        0x00040000
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___S        18

#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__ENABLE     1
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__DISABLE    0
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___M        0x00100000
#define HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S        20



//MMSS_HDMI_AUDIO_CFG
#define HDMI_AUDIO_CFG_OFFSET   0x1D0
#define HDMI_AUDIO_CFG__ENABLE__AUDIO_ENGINE__ON     1
#define HDMI_AUDIO_CFG__ENABLE__AUDIO_ENGINE__OFF    0
#define HDMI_AUDIO_CFG__ENABLE___M     0x1
#define HDMI_AUDIO_CFG__ENABLE___S     0

#define HDMI_AUDIO_CFG__FIFO_WATERMARK__FIFO_WATERMARK_VALUE         4
#define HDMI_AUDIO_CFG__FIFO_WATERMARK___M     0xF0
#define HDMI_AUDIO_CFG__FIFO_WATERMARK___S     0x4

#define HDMI_AUDIO_CFG__PAYLOAD_PACK_EN__ON     1
#define HDMI_AUDIO_CFG__PAYLOAD_PACK_EN__OFF    0
#define HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___M    0x200
#define HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___S    0x9

#define HDMI_AUDIO_CFG_PAYLOAD_HBR_EN_ON     1
#define HDMI_AUDIO_CFG_PAYLOAD_HBR_EN_OFF    0
#define HDMI_AUDIO_CFG_PAYLOAD_HBR_EN___M    0x400
#define HDMI_AUDIO_CFG_PAYLOAD_HBR_EN___S    0xA

#define HDMI_AUDIO_CFG__AUDIO_MODE__DEFAULT          0
#define HDMI_AUDIO_CFG__AUDIO_MODE__3D_AUDIO         0x1
#define HDMI_AUDIO_CFG__AUDIO_MODE__MULTI_CHANNEL    0x2
#define HDMI_AUDIO_CFG__AUDIO_MODE___M    0x70000
#define HDMI_AUDIO_CFG__AUDIO_MODE___S    16  

#define HDMI_AUDIO_CFG__FLAT_SW_PACK__LPASS      1
#define HDMI_AUDIO_CFG__FLAT_SW_PACK__DEFAULT    0
#define HDMI_AUDIO_CFG__FLAT_SW_PACK___M    0x00800000
#define HDMI_AUDIO_CFG__FLAT_SW_PACK___S    23 

#define HDMI_AUDIO_CFG__SP_SW_PACK__LPASS      1
#define HDMI_AUDIO_CFG__SP_SW_PACK__DEFAULT    0
#define HDMI_AUDIO_CFG__SP_SW_PACK___M    0x01000000
#define HDMI_AUDIO_CFG__SP_SW_PACK___S    24     



//MMSS_HDMI_DEBUG
#define HDMI_DEBUG_OFFSET   0x204
#define HDMI_DEBUG__SAMPLE_PRESENT__ENABLE           1
#define HDMI_DEBUG__SAMPLE_PRESENT__DISABLE          0
#define HDMI_DEBUG__SAMPLE_PRESENT___M          0x10
#define HDMI_DEBUG__SAMPLE_PRESENT___S          4

#define HDMI_DEBUG__SAMPLE_PRESENT_LAYOUT1___M          0xF
#define HDMI_DEBUG__SAMPLE_PRESENT_LAYOUT1___S          0


static uint32                  hdmi_aud_phy_base;
static int32                   hdmi_aud_virt_offset;

/*=====================================================================
 Function definitions
 ======================================================================*/

void hal_mdss_hdmi_aud_init(uint32 version, uint32 phy_base, uint32 virt_base)
{   
   hdmi_aud_phy_base = phy_base;

   ///< Calculate offset of virtual address in words (4 bytes)
   hdmi_aud_virt_offset = (virt_base - phy_base)>>2; 

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE HDMI HAL:hdmiAudPhyBase =0x%x hdmiAudVirtOffset = 0x%x \n", hdmi_aud_phy_base, hdmi_aud_virt_offset);
}

#ifdef AFE_REG_DUMP 

void HAL_hdmi_RegDump()
{
   uint32 regVal = 0;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== start HAL_hdmi_RegDump ====\n");  

   ReadRegister1(hdmiAudPhyBase + HDMI_CTRL_OFFSET, hdmiAudVirtOffset, &regVal);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_CTRL = 0x%x \n", regVal);   

   ReadRegister1(hdmiAudPhyBase + HDMI_AUDIO_CFG_OFFSET, hdmiAudVirtOffset, &regVal);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_AUDIO_CFG = 0x%x \n", regVal); 

   ReadRegister1(hdmiAudPhyBase + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmiAudVirtOffset, &regVal);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_AUDIO_PKT_CTRL = 0x%x \n", regVal); 

   ReadRegister1(hdmiAudPhyBase + HDMI_DEBUG_OFFSET, hdmiAudVirtOffset, &regVal);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_DEBUG = 0x%x \n", regVal); 

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== end HAL_hdmi_RegDump ====\n"); 
}

#endif

void hal_mdss_hdmi_disable_core(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_CFG__ENABLE___M;
   fieldValue = HDMI_AUDIO_CFG__ENABLE__AUDIO_ENGINE__OFF << HDMI_AUDIO_CFG__ENABLE___S;

   //make sure HDMI core audio is dsiable before enabling DMA engine. This handles SSR case
   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_enable_core(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_CFG__ENABLE___M | HDMI_AUDIO_CFG__FIFO_WATERMARK___M;
   fieldValue = (HDMI_AUDIO_CFG__ENABLE__AUDIO_ENGINE__ON << HDMI_AUDIO_CFG__ENABLE___S) |
                (HDMI_AUDIO_CFG__FIFO_WATERMARK__FIFO_WATERMARK_VALUE << HDMI_AUDIO_CFG__FIFO_WATERMARK___S);

   ///< Enable HDMI Audio engine
   ///< MUST ENABLE after Audio DMA is enabled
   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_enable_hw_pack(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___M;
   fieldValue = HDMI_AUDIO_CFG__PAYLOAD_PACK_EN__ON << HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___S;
   
   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_disable_hw_pack(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___M;
   fieldValue = HDMI_AUDIO_CFG__PAYLOAD_PACK_EN__OFF << HDMI_AUDIO_CFG__PAYLOAD_PACK_EN___S;
   
   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

/**
 * Configures High bit rate (HBR) enable bit, to enable/disable
 * high bit rate for compressed pass-through use case.
 *
 * @param[in] enable    flag that indicates to enable or disable HBR
 *
 * @return  None
 *
 * @dependencies
 * None.
 */
void hal_mdss_hdmi_enable_disable_hbr(hdmi_mod_en_state_t enable)
{
  uint32_t mask,field_value;

  mask = HDMI_AUDIO_CFG_PAYLOAD_HBR_EN___M;

  if(ENABLE == enable)
  {
    field_value = HDMI_AUDIO_CFG_PAYLOAD_HBR_EN_ON << HDMI_AUDIO_CFG_PAYLOAD_HBR_EN___S;
  }
  else
  {
    field_value = HDMI_AUDIO_CFG_PAYLOAD_HBR_EN_OFF << HDMI_AUDIO_CFG_PAYLOAD_HBR_EN___S;
  }

  UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask,field_value,0);
}

void hal_mdss_hdmi_reset_sample_present_bit(void)
{
   //for 2 channel case, reset to zero
   uint32 mask,fieldValue;

   mask = HDMI_DEBUG__SAMPLE_PRESENT___M | HDMI_DEBUG__SAMPLE_PRESENT_LAYOUT1___M;
   fieldValue = (HDMI_DEBUG__SAMPLE_PRESENT__DISABLE << HDMI_DEBUG__SAMPLE_PRESENT___S) | (0 << HDMI_DEBUG__SAMPLE_PRESENT_LAYOUT1___S);

   UpdateRegister1(hdmi_aud_phy_base + HDMI_DEBUG_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_disable_packet_ctl(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___M;
   fieldValue = (HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND__DISABLE << HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___S);

   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_enable_packet_ctl(void)
{
   uint32 mask,fieldValue;

   mask = HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___M;
   fieldValue = (HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND__ENABLE << HDMI_AUDIO_PKT_CTRL__AUDIO_SAMPLE_SEND___S);

   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}


void hal_mdss_hdmi_cfg_audio_mode(mdss_hdmi_audio_mode mode)
{
   uint32 mask,fieldValue;
   uint32 mask1,fieldValue1;

   mask = HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___M;
   mask1 = HDMI_AUDIO_CFG__AUDIO_MODE___M | HDMI_AUDIO_CFG__FLAT_SW_PACK___M | HDMI_AUDIO_CFG__SP_SW_PACK___M;

   switch(mode)
   {
   case MODE_0:    //single stream via legacy hw, lpass uses LPAIF DMA
   default:
      fieldValue = HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE__DEFAULT << HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___S;

      fieldValue1 = (HDMI_AUDIO_CFG__AUDIO_MODE__DEFAULT << HDMI_AUDIO_CFG__AUDIO_MODE___S)     | 
                    (HDMI_AUDIO_CFG__FLAT_SW_PACK__DEFAULT << HDMI_AUDIO_CFG__FLAT_SW_PACK___S) |
                    (HDMI_AUDIO_CFG__SP_SW_PACK__DEFAULT << HDMI_AUDIO_CFG__SP_SW_PACK___S);
      break;

   case MODE_1:   //single stream via new hw
      fieldValue = HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE__DEFAULT << HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___S;

      fieldValue1 = (HDMI_AUDIO_CFG__AUDIO_MODE__DEFAULT << HDMI_AUDIO_CFG__AUDIO_MODE___S)     | 
                    (HDMI_AUDIO_CFG__FLAT_SW_PACK__LPASS << HDMI_AUDIO_CFG__FLAT_SW_PACK___S) |
                    (HDMI_AUDIO_CFG__SP_SW_PACK__LPASS << HDMI_AUDIO_CFG__SP_SW_PACK___S);
      break;

   case MODE_2:   //multiple streams via new hw
      fieldValue = HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE__LPASS << HDMI_AUDIO_PKT_CTRL__AUDIO_PKT_PULSE___S;

      fieldValue1 = (HDMI_AUDIO_CFG__AUDIO_MODE__MULTI_CHANNEL << HDMI_AUDIO_CFG__AUDIO_MODE___S)     | 
                    (HDMI_AUDIO_CFG__FLAT_SW_PACK__LPASS << HDMI_AUDIO_CFG__FLAT_SW_PACK___S) |
                    (HDMI_AUDIO_CFG__SP_SW_PACK__LPASS << HDMI_AUDIO_CFG__SP_SW_PACK___S);
      break;
   }

   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   UpdateRegister1(hdmi_aud_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi_aud_virt_offset, mask1,fieldValue1,0);
}

void hal_mdss_hdmi_start_audio_info_frame(void)
{
   //Use two separated register write:
   //AUDIO_INFO_UPDATE@HDMI_INFOFRAME_CTRL0 = 1
   //AUDIO_INFO_SEND@HDMI_INFOFRAME_CTRL0 = 1
   //AUDIO_INFO_UPDATE@HDMI_INFOFRAME_CTRL0 = 0
   //AudioInfoFrame will be out in next VSYNC.
   uint32 mask,fieldValue;

   mask = HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___M | HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___M;

   fieldValue = (HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___S) |
                (HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___S);
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_stop_audio_info_frame(void)
{
   //Two separated register writes:
   //AUDIO_INFO_UPDATE & AUDIO_INFO_SEND@HDMI_INFOFRAME_CTRL0 = 0
   uint32 mask,fieldValue;

   mask = HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___M | HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___M;

   fieldValue = (HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_SEND___S) |
                (HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___S);
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_INFO_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_start_audio_metadata(void)
{
   //Use two separated register write:
   //AUDIO_METADATA_UPDATE@HDMI_INFOFRAME_CTRL0 = 1
   //AUDIO_METADATA_SEND@HDMI_INFOFRAME_CTRL0 = 1
   //AUDIO_METADATA_UPDATE@HDMI_INFOFRAME_CTRL0 = 0
   //AudioMetaData will be out in next VSYNC.
   uint32 mask,fieldValue;

   mask = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___M | HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___M;

   fieldValue = (HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___S) |
                (HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S);
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_update_audio_metadata(void)     //for add and remove sub-stream
{
   //Use two separated register write:
   //AUDIO_METADATA_UPDATE@HDMI_INFOFRAME_CTRL0 = 1
   //AUDIO_METADATA_UPDATE@HDMI_INFOFRAME_CTRL0 = 0
   uint32 mask,fieldValue;

   mask = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___M;

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}

void hal_mdss_hdmi_stop_audio_metadata(void)
{
   //Two separated register writes:
   //AUDIO_METADATA_UPDATE & AUDIO_METADATA_SEND@HDMI_INFOFRAME_CTRL0 = 0

   uint32 mask,fieldValue;

   mask = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___M | HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___M;

   fieldValue = (HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_SEND___S) |
                (HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__ENABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S);
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);

   fieldValue = HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE__DISABLE << HDMI_INFOFRAME_CTRL0__AUDIO_METADATA_UPDATE___S;
   UpdateRegister1(hdmi_aud_phy_base + HDMI_INFOFRAME_CTRL0_OFFSET, hdmi_aud_virt_offset, mask,fieldValue,0);
}
