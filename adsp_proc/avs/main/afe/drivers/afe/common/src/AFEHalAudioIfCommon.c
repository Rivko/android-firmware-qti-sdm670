/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEHalAudioIfCommon.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
/* Istari address file */
#include "LPASS_ADDRESS_FILE_LPAIF_HAL_V2.h"

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEAudioIfCommon.h"
#include "AFEHalAudioIfCommon.h"
#include "hwd_devcfg.h"

#define MUX_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPAIF_PCM_I2S_SEL0)  

#ifdef AUDIOIF_REG_DUMP    
typedef struct hal_audioif_mux_dump
{
   uint32_t LPASS_LPAIF_PCM_I2S_SELa_DUMP[MAX_LPAIF_MUX_NUM][LPA_IF_SEQ_MAX];
} hal_audioif_mux_dump_t;

static hal_audioif_mux_dump_t afe_hal_audioif_mux_dump;
#endif

//global data
/**
 * Address related definitions
 */
static uint32_t                  mux_phy_base; 
static int32_t                   mux_virt_offset;
static uint32_t                  mux_version;


void hal_audioif_common_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t mux_offset)
{
   mux_version  = version;   
   mux_phy_base = phy_base;
   
   ///< Calculate offset of virtual address in words (4 bytes)
   mux_virt_offset = reg_virt_offset(virt_base, phy_base, mux_offset);


   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "mux version:[0x%x], mux phy base:[0x%x], mux virt base:[0x%x], mux offset:[0x%x], mux virt offset:[0x%x]", \
      mux_version, mux_phy_base, virt_base, mux_offset, mux_virt_offset );

}


/** 
 * config mux for the specified PCM or I2S  index.
 * 
 * @param[in] intf_type       inteface type (i2s or pcm) to configure mux value.
 * @param[in] mux_index   mux index to be configured.
 * Registers Affected:
 *               1) LPASS_LPAIF_PCM_I2S_SELa 
 */
void hal_audioif_common_config_mux(audioif_hw_t intf_type, uint32_t mux_index)
{
   uint32_t mask, field_value;

   /* Set the TCSR MUX is configured in the clock manager */

   /* MI2S corresponds to MUXSEL to 0 */
   /* PCM corresponds to MUXSEL to 1 */

   if(AUDIOIF_I2S == intf_type)
   {
      field_value = LPASS_LPAIF_PCM_I2S_SELa__SEL__I2S_SRC;
   }
   else
   {
      field_value = LPASS_LPAIF_PCM_I2S_SELa__SEL__PCM_SRC;
   }
   mask = LPASS_LPAIF_PCM_I2S_SELa__SEL___M;
   
   UpdateRegister1(mux_phy_base + MUX_REG_OFFSET(LPASS_LPAIF_PCM_I2S_SELa(mux_index)), mux_virt_offset, mask,field_value,0);
}   


#ifdef AUDIOIF_REG_DUMP    
void hal_afe_audioif_mux_dump(uint32_t intf_index, uint32_t dump_seq)
{
   // if the mux is existed
   if(mux_version)
   {
      if(intf_index < MAX_LPAIF_MUX_NUM)
      {
         ReadRegister1(mux_phy_base + MUX_REG_OFFSET(LPASS_LPAIF_PCM_I2S_SELa(intf_index)),mux_virt_offset, &afe_hal_audioif_mux_dump.LPASS_LPAIF_PCM_I2S_SELa_DUMP[intf_index][dump_seq]);
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM_I2S_SELa (addr: 0x%x) = 0x%x",LPASS_LPAIF_PCM_I2S_SELa(intf_index),  afe_hal_audioif_mux_dump.LPASS_LPAIF_PCM_I2S_SELa_DUMP[intf_index][dump_seq]);
      }
   }
}
#endif


