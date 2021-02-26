/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHalHdmi14.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
/* Istari address file */
#include "LPASS_ADDRESS_FILE_LPAIF_HAL_V2.h"

#ifdef AFE_REG_DUMP
/* Istari CC Address file */
#include "LPASS_ADDRESS_FILE_CC_HAL_V2.h"
#endif

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEHalHdmi14.h"
#include "AFEHalDump.h"
#include "lpasshwio_devcfg.h"


#define HDMI_CTRL_OFFSET 0x100

#define HDMI_TIMING_ENGINE_EN_OFFSET 0x3F8

#define HDMI_CS_60958_0_OFFSET   0x1EC
#define HDMI_CS_60958_0_SAMPLING_FREQUENCY___M   0xF000000
#define HDMI_CS_60958_0_B___M                    0x2   //required for non-PCM stream
#define HDMI_CS_60958_0__CLOCK_ACCURACY___M      0x30000000
#define HDMI_CS_60958_0__CLOCK_ACCURACY___S      28

#define HDMI_CS_60958_1_OFFSET   0x1F0
#define HDMI_CS_60958_1_CS_WORD_LENGTH___M   0xF
#define HDMI_CS_60958_1_VALID_LR_M           0x50000

#define HDMI_AUDIO_CFG_OFFSET   0x2D0
#define HDMI_AUDIO_ENGINE__ENABLE__ON     1
#define HDMI_AUDIO_ENGINE__ENABLE__OFF    0
#define HDMI_AUDIO_ENGINE__ENABLE___M     0x1
#define HDMI_AUDIO_ENGINE__ENABLE___S     0

#define HDMI_AUDIO_FIFO_WATERMARK         4
#define HDMI_AUDIO_FIFO_WATERMARK___M     0xF0
#define HDMI_AUDIO_FIFO_WATERMARK___S     0x4

#define HDMI_AUDIO_PAYLOAD_PACK_EN_ON     1
#define HDMI_AUDIO_PAYLOAD_PACK_EN_OFF    0
#define HDMI_AUDIO_PAYLOAD_PACK_EN___M    0x200
#define HDMI_AUDIO_PAYLOAD_PACK_EN___S    0x9        

#define HDMI_AUDIO_PAYLOAD_HBR_EN_ON     1
#define HDMI_AUDIO_PAYLOAD_HBR_EN_OFF    0
#define HDMI_AUDIO_PAYLOAD_HBR_EN___M    0x400
#define HDMI_AUDIO_PAYLOAD_HBR_EN___S    0xA

#define HDMI_AUDIO_PKT_CTRL_OFFSET   0x120
#define HDMI_AUDIO_SAMPLE_SEND___M        0x1
#define HDMI_AUDIO_SAMPLE_SEND___S        0
#define HDMI_AUDIO_SAMPLE_SEND_ENABLE   1
#define HDMI_AUDIO_SAMPLE_SEND_DISABLE  0
#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___M 0x04000000
#define HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___S 26


#define HDMI_AUDIO_PKT_CTRL2_OFFSET                0x144
#define HDMI_AUDIO_PKT_CTRL2__CHANNEL_ENABLE___M   0x00000F00
#define HDMI_AUDIO_PKT_CTRL2__CHANNEL_ENABLE___S   8
#define HDMI_AUDIO_PKT_CTRL2__CS_60958_SOURCE___M  0x00000010
#define HDMI_AUDIO_PKT_CTRL2__CS_60958_SOURCE___S  4

#define HDMI_DEBUG_OFFSET   0x304
#define HDMI_DEBUG_SAMPLE_PRESENT_ENABLE           1
#define HDMI_DEBUG_SAMPLE_PRESENT_DISABLE          0
#define HDMI_DEBUG_SAMPLE_PRESENT_ENABLE_SHIFT     4
#define HDMI_DEBUG_SAMPLE_PRESENT_BIT_MASK         0x0000001F


#define HDMI14_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPA_IF_BASE)  

#ifdef AUDIOIF_REG_DUMP
typedef struct hal_hdmi14_dump
{
   uint32_t HDMI_CTRL_OFFSET_DUMP[LPA_IF_SEQ_MAX];
   uint32_t HDMI_CS_60958_0_OFFSET_DUMP[LPA_IF_SEQ_MAX];
   uint32_t HDMI_CS_60958_1_OFFSET_DUMP[LPA_IF_SEQ_MAX];
   uint32_t HDMI_AUDIO_CFG_OFFSET_DUMP[LPA_IF_SEQ_MAX];
   uint32_t HDMI_AUDIO_PKT_CTRL_OFFSET_DUMP[LPA_IF_SEQ_MAX];
   uint32_t HDMI_DEBUG_OFFSET_DUMP[LPA_IF_SEQ_MAX];
} hal_hdmi14_dump_t;

static hal_hdmi14_dump_t afe_hal_hdmi14_dump;
#endif

//global data
/**
 * Address related definitions
 */
static uint32_t                  hdmi14_phy_base; 
static int32_t                   hdmi14_virt_offset;
static uint32_t                  hdmi14_version;

void hal_hdmi14_init(uint32_t version, uint32_t phy_base, uint32_t virt_base)
{
   hdmi14_version  = version;   

   /*SW workaround 
    * Since lpass hw version 3.8.0, the hdmi offset has been changed.
    * The offset has been shifted down by 0x100 which means HDMI_CTL_OFFSET(0x100) has been changed to 0x0.
    * So that it adjust the phy reg address base to accomodate this change.
    * Once it has been branched, there will be single offset only for the new hardware versions
    */
   if(version >= LPASS_HW_VER_3_8_0)
   {
      hdmi14_phy_base = phy_base - HDMI_CTRL_OFFSET;
   }
   
   ///< Calculate offset of virtual address in words (4 bytes)
   hdmi14_virt_offset = (virt_base - phy_base)>>2; 


   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hdmi14 version:[0x%x], hdmi14 phy base:[0x%x], hdmi14virt base:[0x%x], hdmi14 virt offset:[0x%x]", \
      hdmi14_version, hdmi14_phy_base, virt_base, hdmi14_virt_offset );

}

#ifdef AUDIOIF_REG_DUMP
void hal_hdmi14_reg_dump(uint32_t dump_seq)
{
   ReadRegister1(hdmi14_phy_base + HDMI_CTRL_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_CTRL_OFFSET_DUMP[dump_seq]);
   ReadRegister1(hdmi14_phy_base + HDMI_CS_60958_0_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_CS_60958_0_OFFSET_DUMP[dump_seq]);
   ReadRegister1(hdmi14_phy_base + HDMI_CS_60958_1_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_CS_60958_1_OFFSET_DUMP[dump_seq]);
   ReadRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_AUDIO_CFG_OFFSET_DUMP[dump_seq]);
   ReadRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_AUDIO_PKT_CTRL_OFFSET_DUMP[dump_seq]);
   ReadRegister1(hdmi14_phy_base + HDMI_DEBUG_OFFSET, hdmi14_virt_offset, &afe_hal_hdmi14_dump.HDMI_DEBUG_OFFSET_DUMP[dump_seq]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== start HAL_hdmi_RegDump dump_seq[%d] ====", dump_seq);  
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_CTRL = 0x%x", afe_hal_hdmi14_dump.HDMI_CTRL_OFFSET_DUMP[dump_seq]);   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_60958_0 = 0x%x", afe_hal_hdmi14_dump.HDMI_CS_60958_0_OFFSET_DUMP[dump_seq]);   
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_60958_1 = 0x%x", afe_hal_hdmi14_dump.HDMI_CS_60958_1_OFFSET_DUMP[dump_seq]);  
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_AUDIO_CFG = 0x%x", afe_hal_hdmi14_dump.HDMI_AUDIO_CFG_OFFSET_DUMP[dump_seq]); 
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_AUDIO_PKT_CTRL = 0x%x", afe_hal_hdmi14_dump.HDMI_AUDIO_PKT_CTRL_OFFSET_DUMP[dump_seq]); 
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== HDMI_DEBUG = 0x%x", afe_hal_hdmi14_dump.HDMI_DEBUG_OFFSET_DUMP[dump_seq]); 
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== end HAL_hdmi_RegDump ===="); 
}
#endif

void hal_hdmi14_disable_core(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_ENGINE__ENABLE___M;
   field_value = HDMI_AUDIO_ENGINE__ENABLE__OFF << HDMI_AUDIO_ENGINE__ENABLE___S;

   //make sure HDMI core audio is dsiable before enabling DMA engine. This handles SSR case
   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_enable_core(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_ENGINE__ENABLE___M | HDMI_AUDIO_FIFO_WATERMARK___M;
   field_value = (HDMI_AUDIO_ENGINE__ENABLE__ON << HDMI_AUDIO_ENGINE__ENABLE___S) |
                (HDMI_AUDIO_FIFO_WATERMARK << HDMI_AUDIO_FIFO_WATERMARK___S);

   ///< Enable HDMI Audio engine
   ///< MUST ENABLE after Audio DMA is enabled
   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_enable_hw_pack(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PAYLOAD_PACK_EN___M;
   field_value = HDMI_AUDIO_PAYLOAD_PACK_EN_ON << HDMI_AUDIO_PAYLOAD_PACK_EN___S;
   
   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_disable_hw_pack(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PAYLOAD_PACK_EN___M;
   field_value = HDMI_AUDIO_PAYLOAD_PACK_EN_OFF << HDMI_AUDIO_PAYLOAD_PACK_EN___S;
   
   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

/**
 * Configures High bit rate (HBR) enable bit, to enable/disable
 * high bit rate for compressed pass-through use case.
 *
 * @param[in] enable  flag that indicates to enable or disable HBR
 *
 * @return  None
 *
 * @dependencies
 * None.
 */
void hal_hdmi14_enable_disable_hbr(hdmi14_mod_en_state_t enable)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PAYLOAD_HBR_EN___M;

   if(ENABLE == enable)
   {
     field_value = HDMI_AUDIO_PAYLOAD_HBR_EN_ON << HDMI_AUDIO_PAYLOAD_HBR_EN___S;
   }
   else
   {
     field_value = HDMI_AUDIO_PAYLOAD_HBR_EN_OFF << HDMI_AUDIO_PAYLOAD_HBR_EN___S;
   }

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_CFG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_set_packet_info(void* ch_status_table_ptr, uint32_t validity_bit)
{
   uint32_t mask,field_value;
   uint32_t* pTable = ch_status_table_ptr;

   mask = HDMI_CS_60958_0_B___M | HDMI_CS_60958_0_SAMPLING_FREQUENCY___M;
   field_value =((pTable[1]>>29)|(pTable[24]>>6)|(pTable[25]>>5)|(pTable[26]>>4)|(pTable[27]>>3));

   UpdateRegister1(hdmi14_phy_base + HDMI_CS_60958_0_OFFSET, hdmi14_virt_offset, mask,field_value,0);
   
   mask = HDMI_CS_60958_1_CS_WORD_LENGTH___M | HDMI_CS_60958_1_VALID_LR_M;
   field_value =((pTable[32]>>30)|(pTable[33]>>29)|(pTable[34]>>28)|(pTable[35]>>27));
   field_value |= ((validity_bit<<16) | (validity_bit<<18));

   UpdateRegister1(hdmi14_phy_base + HDMI_CS_60958_1_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_set_pkt_ctl_cs_60958_source_hwd(void)
{
   //source 0: sw pack, 1: hw pack
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PKT_CTRL2__CS_60958_SOURCE___M;
   field_value = (1 << HDMI_AUDIO_PKT_CTRL2__CS_60958_SOURCE___S) & HDMI_AUDIO_PKT_CTRL2__CS_60958_SOURCE___M;

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL2_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_set_pkt_ctl_channel_enable_layout1(void)
{
   //layout1 which frame will contain the channul status info. it shoudl be 4 (0xF)
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PKT_CTRL2__CHANNEL_ENABLE___M;
   field_value = (0xF << HDMI_AUDIO_PKT_CTRL2__CHANNEL_ENABLE___S) & HDMI_AUDIO_PKT_CTRL2__CHANNEL_ENABLE___M;

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL2_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}   

void hal_hdmi14_set_pkt_ctl_cs_60958_update(void)
{
   //   HWIO_OUTF(MMSS_HDMI_AUDIO_PKT_CTRL, CS_60958_UPDATE, 0x1);
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___M;
   field_value = (1 << HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___S) & HDMI_AUDIO_PKT_CTRL__CS_60958_UPDATE___M;

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_sample_present_bit_layout1(uint32_t present_bits)
{
   //DEBUG[4] when enabled, will make Sample present for LAYOUT 1 programable. SAMPLE_PRESENT will be the vallue programmed in DEBUG[3:0]

   uint32_t mask,field_value;

   mask = HDMI_DEBUG_SAMPLE_PRESENT_BIT_MASK;
   field_value =( (present_bits&0x0000000F) | (HDMI_DEBUG_SAMPLE_PRESENT_ENABLE << HDMI_DEBUG_SAMPLE_PRESENT_ENABLE_SHIFT) );

   UpdateRegister1(hdmi14_phy_base + HDMI_DEBUG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_reset_sample_present_bit(void)
{
   //for 2 channel case, reset to zero
   uint32_t mask,field_value;

   mask = HDMI_DEBUG_SAMPLE_PRESENT_BIT_MASK;
   field_value = HDMI_DEBUG_SAMPLE_PRESENT_DISABLE << HDMI_DEBUG_SAMPLE_PRESENT_ENABLE_SHIFT;

   UpdateRegister1(hdmi14_phy_base + HDMI_DEBUG_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_disable_packet_ctl(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_SAMPLE_SEND___M;
   field_value = (HDMI_AUDIO_SAMPLE_SEND_DISABLE << HDMI_AUDIO_SAMPLE_SEND___S);

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}

void hal_hdmi14_enable_packet_ctl(void)
{
   uint32_t mask,field_value;

   mask = HDMI_AUDIO_SAMPLE_SEND___M;
   field_value = (HDMI_AUDIO_SAMPLE_SEND_ENABLE << HDMI_AUDIO_SAMPLE_SEND___S);

   UpdateRegister1(hdmi14_phy_base + HDMI_AUDIO_PKT_CTRL_OFFSET, hdmi14_virt_offset, mask,field_value,0);
}



/** 
 * get the register value for the hdmi14 
 * 
 *
 * @return   the register value for hdmi14
 */
uint32 hal_hdmi14_get_intf(void)
{  
   return LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__HDMI;
}

