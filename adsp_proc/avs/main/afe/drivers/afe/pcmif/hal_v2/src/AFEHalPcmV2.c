/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/hal_v2/src/AFEHalPcmV2.c#1 $

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
#include "AFEPcmDriver_i.h"
#include "AFEHalPcm.h"
#include "AFEPcmHalFuncDefs.h"

#define PCM_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPAIF_PCM_CTL0)  

#ifdef AUDIOIF_REG_DUMP  
typedef struct hal_pcm_dump_v2
{
   uint32_t LPASS_LPAIF_PCM_CTLa_DUMP[MAX_PCM_TYPE_NUM][LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a_DUMP[MAX_PCM_TYPE_NUM][LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a_DUMP[MAX_PCM_TYPE_NUM][LPA_IF_SEQ_MAX];
} hal_pcm_dump_v2_t;

static hal_pcm_dump_v2_t afe_hal_pcm_dump_v2;

static void hal_pcm_dump_v2(uint32_t intf_index, uint32_t dir, uint32_t dump_seq);
#endif

void hal_pcm_init_v2(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset);
void hal_pcm_reset_v2(uint32_t pcm_index, uint32_t dir);
void hal_pcm_reset_release_v2(uint32_t pcm_index, uint32_t dir);
void hal_pcm_config_v2(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr);
void hal_pcm_enable_v2(uint32_t pcm_index, uint32_t dir);
void hal_pcm_disable_v2(uint32_t pcm_index, uint32_t dir);
uint32_t hal_pcm_get_intf_v2(uint32_t pcm_index, uint32_t dir);



void hal_pcm_get_func_table_v2(afe_pcm_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_pcm_get_func_table_v2 func_table is NULL"); 
     return;
   }
  
   func_table->hal_pcm_init_fptr                           = hal_pcm_init_v2;
   func_table->hal_pcm_reset_fptr               = hal_pcm_reset_v2; 
   func_table->hal_pcm_reset_release_fptr              = hal_pcm_reset_release_v2;
   func_table->hal_pcm_config_fptr               = hal_pcm_config_v2;
   func_table->hal_pcm_enable_fptr                   = hal_pcm_enable_v2;
   func_table->hal_pcm_disable_fptr                 = hal_pcm_disable_v2;
   func_table->hal_pcm_get_intf_fptr               = hal_pcm_get_intf_v2;
#ifdef AUDIOIF_REG_DUMP  
   func_table->hal_pcm_dump_fptr                   = hal_pcm_dump_v2;
#endif
   
}



//global data
/**
 * Address related definitions
 */
static uint32_t                  pcm_phy_base; 
static int32_t                   pcm_virt_offset;
static uint32_t                  pcm_version;

void hal_pcm_init_v2(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset)
{
   pcm_version  = version;   
   pcm_phy_base = phy_base;
   
   ///< Calculate offset of virtual address in words (4 bytes)
   pcm_virt_offset = reg_virt_offset(virt_base, phy_base, pcm_offset);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "pcm version:[0x%x], pcm phy base:[0x%x], pcm virt base:[0x%x],pcm offset:[0x%x], pcm virt offset:[0x%x]", \
      pcm_version, pcm_phy_base, virt_base, pcm_offset, pcm_virt_offset );

}

void hal_pcm_reset_v2(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(PCM_SINK == dir)
   {
     //Reset transmit path
      field_value = LPASS_LPAIF_PCM_CTLa__RESET_TX__ENABLE << LPASS_LPAIF_PCM_CTLa__RESET_TX___S;
      mask = LPASS_LPAIF_PCM_CTLa__RESET_TX___M;
   }
   else
   {
     //field_value receive path
     field_value = LPASS_LPAIF_PCM_CTLa__RESET_RX__ENABLE << LPASS_LPAIF_PCM_CTLa__RESET_RX___S;
     mask = LPASS_LPAIF_PCM_CTLa__RESET_RX___M;
   }
   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
}

void hal_pcm_reset_release_v2(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(PCM_SINK == dir)
   {
     //reset release trasmit path
     field_value = LPASS_LPAIF_PCM_CTLa__RESET_TX__DISABLE << LPASS_LPAIF_PCM_CTLa__RESET_TX___S;
     mask = LPASS_LPAIF_PCM_CTLa__RESET_TX___M;
   }
   else
   {
     //reset release receive path
     field_value = LPASS_LPAIF_PCM_CTLa__RESET_RX__DISABLE << LPASS_LPAIF_PCM_CTLa__RESET_RX___S;
     mask = LPASS_LPAIF_PCM_CTLa__RESET_RX___M;
   }

   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register

}


/** 
 * config the instance of the PCM  specified .     
 * 
 * @param[in] pcm_index     PCM instance that needs to be reset.
 * @param[in] dir       0 : Sink, 1:  Source path
 * @param[in] configPtr Pointer to the configuration parameters. 
 * Registers Affeceted:
 *                LPAIF_PCM_CTLa
 */
void hal_pcm_config_v2(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr)
{
   uint32_t mask,field_value = 0;
   uint32_t slot_count;

   /**
    * To update the PCM control register, first build the mask for the fields
    * that needs to be updated. Here we will be udpating the fields - SYNC_SRC,
    * AUX_MODE, RATE, TPCM_WIDTH & TPCM_SLOT and CTRL_DATA_OE.
    * This is for the speaker path.
    */ 
   mask = LPASS_LPAIF_PCM_CTLa__RATE___M |
          LPASS_LPAIF_PCM_CTLa__SYNC_SRC___M |
          LPASS_LPAIF_PCM_CTLa__AUX_MODE___M | 
          LPASS_LPAIF_PCM_CTLa__CTRL_DATA_OE___M;

   /*
    *   Originally, PCM have (non-TDM setup)
    *      1) LPAIF_PCM_CTL.TPCM_SLOT_NUM to govern the totoal number of slots..
    *      2) LPAIF_PCM_CTL.TPCM_SLOT, LPAIF_PCM_CTL.TPCM_SLOT1, SLOT2 .. SLOT7 for slot location
    *
    *   Now,
    *      1) LPAIF_PCM_I2S_SELa.SEL
    *      2) LPAIF_PCM_TPCM_SLOT_NUM_x ( A 32-bit vector for slot loction )
    *      3) LPAIF_PCM_CTLx.TPCM_WIDTH: actual bits transmitted in a slot.
    *
    *   NOTE: There is no total number of slots config.
    */

    /* Programming the slot number later as in different reg */
   if (PCM_SINK == dir)
   {
      mask |= LPASS_LPAIF_PCM_CTLa__TPCM_WIDTH___M;
   }
   else
   {
      mask |= LPASS_LPAIF_PCM_CTLa__RPCM_WIDTH___M;
   }

   /**
    * The cases specified in the if clause imply that the data width is 16
    * bits. The else clause handles the cases where the data width is 8 bits.
    */ 
   switch (config_ptr->quant)
   {
      case MU_LAW_WITHOUT_PADDING:
      case A_LAW_WITHOUT_PADDING:
         if (PCM_SINK == dir)
         {
            field_value = (LPASS_LPAIF_PCM_CTLa__TPCM_WIDTH__ENUM_8_BIT << 
                                             LPASS_LPAIF_PCM_CTLa__TPCM_WIDTH___S);
         }
         else
         {
            field_value = (LPASS_LPAIF_PCM_CTLa__RPCM_WIDTH__ENUM_8_BIT << 
                                             LPASS_LPAIF_PCM_CTLa__RPCM_WIDTH___S);
         }
         break;
      case LINEAR_WITHOUT_PADDING:
      case LINEAR_WITH_PADDING:
      case A_LAW_WITH_PADDING:
      case MU_LAW_WITH_PADDING:
         if (PCM_SINK == dir)
         {
            field_value = (LPASS_LPAIF_PCM_CTLa__TPCM_WIDTH__ENUM_16_BIT << 
                                                LPASS_LPAIF_PCM_CTLa__TPCM_WIDTH___S);
         }
         else
         {
            field_value = (LPASS_LPAIF_PCM_CTLa__RPCM_WIDTH__ENUM_16_BIT << 
                                                LPASS_LPAIF_PCM_CTLa__RPCM_WIDTH___S);
         }
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->sync_src)
   {
      case PCM_MSM_MODE_SLAVE:
         field_value |= (LPASS_LPAIF_PCM_CTLa__SYNC_SRC__EXTERNAL <<
                                       LPASS_LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      case PCM_MSM_MODE_MASTER:
         field_value |= (LPASS_LPAIF_PCM_CTLa__SYNC_SRC__INTERNAL <<
                                       LPASS_LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->sync)
   {
      case PCM_MODE:
         field_value |= (LPASS_LPAIF_PCM_CTLa__AUX_MODE__PCM <<
                                       LPASS_LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      case AUX_MODE:
         field_value |= (LPASS_LPAIF_PCM_CTLa__AUX_MODE__AUX <<
                                       LPASS_LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->ctrl_data_oe)
   {
      case CTRL_DATA_OE_DISABLE:
         field_value |= (CTRL_DATA_OE_DISABLE << LPASS_LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      case CTRL_DATA_OE_ENABLE:
         field_value |= (CTRL_DATA_OE_ENABLE << LPASS_LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }
   
   
   switch (config_ptr->frame)
   {
      case BITS_PER_FRAME_8:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_8 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_16:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_16 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_32:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_32 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_64:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_64 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_128:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_128 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_256:
         field_value |= (LPASS_LPAIF_PCM_CTLa__RATE__ENUM_256 << LPASS_LPAIF_PCM_CTLa__RATE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register

   /* Number of active slot */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Num active slots = %d" , config_ptr->slot_num);
   /* All four slots */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 0 = %d", config_ptr->slot[0]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 1 = %d", config_ptr->slot[1]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 2 = %d", config_ptr->slot[2]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 3 = %d", config_ptr->slot[3]);
   
   slot_count = config_ptr->slot_num;
   
   /* Depends on number of slots, build the 32-bit vectors */
   /* Program the location from configPrt->slot0, slot1, slot2, slot3 */
   if(slot_count)
   {
     field_value  = (1L << config_ptr->slot[0]);
     slot_count--;
   }
   if(slot_count)
   {
     field_value |= (1L << config_ptr->slot[1]);
     slot_count--;
   }
   if(slot_count)
   {
     field_value |= (1L << config_ptr->slot[2]);
     slot_count--;
   }
   if(slot_count)
   {
     field_value |= (1L << config_ptr->slot[3]);
     slot_count--;
   }
   /* Setup the mask again for slot location */
   if(PCM_SINK == dir)
   {
     mask = LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a___M;
     field_value <<= LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a___S;
     UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
   }
   else
   {
     mask = LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a___M;
     field_value <<= LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a___S;
     UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
   }
}


/** 
 * Enable the specified PCM instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir    PCM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_pcm_enable_v2(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(PCM_SINK == dir)
   {
     //enable transmit path
     field_value = LPASS_LPAIF_PCM_CTLa__ENABLE_TX__ENABLE << LPASS_LPAIF_PCM_CTLa__ENABLE_TX___S;
     mask = LPASS_LPAIF_PCM_CTLa__ENABLE_TX___M;
   }
   else
   {
     //enable receive path
     field_value = LPASS_LPAIF_PCM_CTLa__ENABLE_RX__ENABLE << LPASS_LPAIF_PCM_CTLa__ENABLE_RX___S;
     mask = LPASS_LPAIF_PCM_CTLa__ENABLE_RX___M;
   }
 
   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * Disable the specified I2S instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir   PCM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_pcm_disable_v2(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(PCM_SINK == dir)
   {
      //disable transmit path
     field_value = LPASS_LPAIF_PCM_CTLa__ENABLE_TX__DISABLE << LPASS_LPAIF_PCM_CTLa__ENABLE_TX___S;
     mask = LPASS_LPAIF_PCM_CTLa__ENABLE_TX___M;
   }
   else
   {
     //disable receive path
     field_value = LPASS_LPAIF_PCM_CTLa__ENABLE_RX__DISABLE << LPASS_LPAIF_PCM_CTLa__ENABLE_RX___S;
     mask = LPASS_LPAIF_PCM_CTLa__ENABLE_RX___M;
   }
   
   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * get the register value for the specified PCM index  
 * 
 * @param[in] pcm_index   PCM index
 *
 * @return   the register value for the specified PCM index  
 */
uint32 hal_pcm_get_intf_v2(uint32_t pcm_index, uint32_t dir)
{
   uint32_t field_value = 0;
   
   switch (pcm_index)
   {
      case PCM_PRIMARY:
         if(PCM_SINK == dir)
         {
           field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__PRI_SRC;
         }
         else
         {
           field_value = LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__PRI_SRC;
         }
         break;
      case PCM_SECONDARY:
         if(PCM_SINK == dir)
         {
           field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__SEC_SRC;
         }
         else
         {
           field_value = LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__SEC_SRC;
         }
         break;
      case PCM_TERTIARY:
         if(PCM_SINK == dir)
         {
           field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__TER_SRC;
         }
         else
         {
           field_value = LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__TER_SRC;
         }
         break;
      case PCM_QUATERNARY:
         if(PCM_SINK == dir)
         {
           field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__QUA_SRC;
         }
         else
         {
           field_value = LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__QUA_SRC;
         }
         break;

      case PCM_QUINARY:
        if(PCM_SINK == dir)
        {
          field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__QUI_SRC;
        }
        else
        {
          field_value = LPASS_LPAIF_WRDMA_CTLa__AUDIO_INTF__QUI_SRC;
        }
        break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
          field_value = LPASS_LPAIF_RDDMA_CTLa__AUDIO_INTF__NONE;
         break;
   }
  
   return field_value;
}

#ifdef AUDIOIF_REG_DUMP  
static void hal_pcm_dump_v2(uint32_t intf_index, uint32_t dir, uint32_t dump_seq)
{
   if(MAX_PCM_TYPE_NUM > intf_index)
   {         
      ReadRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(intf_index)),pcm_virt_offset, &afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_CTLa_DUMP[intf_index][dump_seq]);
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_I2S_CTLa(addr: 0x%x) = 0x%x", LPASS_LPAIF_PCM_CTLa(intf_index), afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_CTLa_DUMP[intf_index][dump_seq]);  

      if(LPA_IF_SINK == dir)
      {
        ReadRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a(intf_index)), pcm_virt_offset,&afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a_DUMP[intf_index][dump_seq]);
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a(addr: 0x%x) = 0x%x", LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a(intf_index), afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_TPCM_SLOT_NUM_a_DUMP[intf_index][dump_seq]);  
      }
      else
      {
        ReadRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a(intf_index)), pcm_virt_offset,&afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a_DUMP[intf_index][dump_seq]); 
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a(addr: 0x%x) = 0x%x", LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a(intf_index), afe_hal_pcm_dump_v2.LPASS_LPAIF_PCM_RPCM_SLOT_NUM_a_DUMP[intf_index][dump_seq]);
      }         
   }
}               
#endif
               


