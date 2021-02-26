/** @file HALaudioif.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/hal_v1/src/AFEHalPcmV1.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "LPASS_ADDRESS_FILE.h"

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEPcmDriver_i.h"
#include "AFEHalPcm.h"
#include "AFEPcmHalFuncDefs.h"

#define PCM_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPAIF_PCM_CTL0)  


typedef enum hal_pcm_state_v1
{
   hal_pcm_stop      =     0,
   hal_pcm_start     =     1,
} hal_pcm_state_v1_t;

// primary and secondary interface [2] sink/source [2]
hal_pcm_state_v1_t pcm_intf_state[2][2];

#ifdef AUDIOIF_REG_DUMP  
typedef struct hal_pcm_dump_v1
{
   uint32_t LPASS_LPAIF_PCM_CTLa_DUMP[MAX_PCM_TYPE_NUM_V1][LPA_IF_SEQ_MAX];
   uint32_t LPASS_LPAIF_PCM_SLOT_CTLa_DUMP[MAX_PCM_TYPE_NUM_V1][LPA_IF_SEQ_MAX];
} hal_pcm_dump_v1_t;

static hal_pcm_dump_v1_t afe_hal_pcm_dump_v1;
static void hal_pcm_dump_v1(uint32_t intf_index, uint32_t dir, uint32_t dump_seq);
#endif

void hal_pcm_init_v1(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset);
void hal_pcm_reset_v1(uint32_t pcm_index, uint32_t dir);
void hal_pcm_reset_release_v1(uint32_t pcm_index, uint32_t dir);
void hal_pcm_config_v1(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr);
void hal_pcm_enable_v1(uint32_t pcm_index, uint32_t dir);
void hal_pcm_disable_v1(uint32_t pcm_index, uint32_t dir);
uint32_t hal_pcm_get_intf_v1(uint32_t pcm_index, uint32_t dir);



void hal_pcm_get_func_table_v1(afe_pcm_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_pcm_get_func_table_v1 func_table is NULL"); 
     return;
   }
  
   func_table->hal_pcm_init_fptr                = hal_pcm_init_v1;
   func_table->hal_pcm_reset_fptr               = hal_pcm_reset_v1; 
   func_table->hal_pcm_reset_release_fptr       = hal_pcm_reset_release_v1;
   func_table->hal_pcm_config_fptr              = hal_pcm_config_v1;
   func_table->hal_pcm_enable_fptr              = hal_pcm_enable_v1;
   func_table->hal_pcm_disable_fptr             = hal_pcm_disable_v1;
   func_table->hal_pcm_get_intf_fptr            = hal_pcm_get_intf_v1;
#ifdef AUDIOIF_REG_DUMP  
   func_table->hal_pcm_dump_fptr                = hal_pcm_dump_v1;
#endif
}



//global data
/**
 * Address related definitions
 */
static uint32_t                  pcm_phy_base; 
static int32_t                   pcm_virt_offset;
static uint32_t                  pcm_version;

void hal_pcm_init_v1(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset)
{
   pcm_version  = version;   
   pcm_phy_base = phy_base;
   
   ///< Calculate offset of virtual address in words (4 bytes)
   pcm_virt_offset = reg_virt_offset(virt_base, phy_base, pcm_offset);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "pcm version:[0x%x], pcm phy base:[0x%x], pcm virt base:[0x%x],pcm offset:[0x%x], pcm virt offset:[0x%x]", \
      pcm_version, pcm_phy_base, virt_base, pcm_offset, pcm_virt_offset );

}

void hal_pcm_reset_v1(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;
   uint32_t opposite_dir =  (dir & PCM_SOURCE) ? PCM_SINK: PCM_SOURCE;

   /* reset only once either for RX or TX per each PCM interface. */   
   if(hal_pcm_stop == pcm_intf_state[pcm_index][opposite_dir])
   {
      //enable path
      field_value = LPAIF_PCM_CTLa__RESET__ENABLE << LPAIF_PCM_CTLa__RESET___S;
      mask = LPAIF_PCM_CTLa__RESET___M;

      UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)),pcm_virt_offset,mask,field_value,0); ///< update register
   }
   
}

void hal_pcm_reset_release_v1(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   uint32_t opposite_dir =  (dir & PCM_SOURCE) ? PCM_SINK: PCM_SOURCE;

   /* reset only once either for RX or TX per each PCM interface. */   
   if(hal_pcm_stop == pcm_intf_state[pcm_index][opposite_dir])
   {
      field_value = LPAIF_PCM_CTLa__RESET__DISABLE << LPAIF_PCM_CTLa__RESET___S;
      mask = LPAIF_PCM_CTLa__RESET___M;

      UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
   }
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
void hal_pcm_config_v1(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr)
{
   uint32_t mask,field_value = 0;
   
   /**
    * To update the PCM control register, first build the mask for the fields
    * that needs to be updated. Here we will be udpating the fields - SYNC_SRC,
    * AUX_MODE, RATE, TPCM_WIDTH & TPCM_SLOT and CTRL_DATA_OE.
    * This is for the speaker path.
    */ 
   mask = LPAIF_PCM_CTLa__RATE___M |
          LPAIF_PCM_CTLa__SYNC_SRC___M |
          LPAIF_PCM_CTLa__AUX_MODE___M | 
          LPAIF_PCM_CTLa__CTRL_DATA_OE___M;


   if (PCM_SINK == dir)
   {
      mask |= ( LPAIF_PCM_CTLa__TPCM_WIDTH___M |
                LPAIF_PCM_CTLa__TPCM_SLOT___M |
                LPAIF_PCM_CTLa__TPCM_SLOT_NUM___M);
   }
   else
   {
      mask |= ( LPAIF_PCM_CTLa__RPCM_WIDTH___M |
                LPAIF_PCM_CTLa__RPCM_SLOT___M |
                LPAIF_PCM_CTLa__RPCM_SLOT_NUM___M );
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
            field_value = (LPAIF_PCM_CTLa__TPCM_WIDTH__ENUM_8_BIT << 
                                             LPAIF_PCM_CTLa__TPCM_WIDTH___S);
         }
         else
         {
            field_value = (LPAIF_PCM_CTLa__RPCM_WIDTH__ENUM_8_BIT << 
                                             LPAIF_PCM_CTLa__RPCM_WIDTH___S);
         }
         break;
      case LINEAR_WITHOUT_PADDING:
      case LINEAR_WITH_PADDING:
      case A_LAW_WITH_PADDING:
      case MU_LAW_WITH_PADDING:
         if (PCM_SINK == dir)
         {
            field_value = (LPAIF_PCM_CTLa__TPCM_WIDTH__ENUM_16_BIT << 
                                                LPAIF_PCM_CTLa__TPCM_WIDTH___S);
         }
         else
         {
            field_value = (LPAIF_PCM_CTLa__RPCM_WIDTH__ENUM_16_BIT << 
                                                LPAIF_PCM_CTLa__RPCM_WIDTH___S);
         }
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->sync_src)
   {
      case PCM_MSM_MODE_SLAVE:
         field_value |= (LPAIF_PCM_CTLa__SYNC_SRC__EXTERNAL <<
                                       LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      case PCM_MSM_MODE_MASTER:
         field_value |= (LPAIF_PCM_CTLa__SYNC_SRC__INTERNAL <<
                                       LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->sync)
   {
      case PCM_MODE:
         field_value |= (LPAIF_PCM_CTLa__AUX_MODE__PCM <<
                                       LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      case AUX_MODE:
         field_value |= (LPAIF_PCM_CTLa__AUX_MODE__AUX <<
                                       LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   switch (config_ptr->ctrl_data_oe)
   {
      case CTRL_DATA_OE_DISABLE:
         field_value |= (CTRL_DATA_OE_DISABLE << LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      case CTRL_DATA_OE_ENABLE:
         field_value |= (CTRL_DATA_OE_ENABLE << LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }
   
   
   switch (config_ptr->frame)
   {
      case BITS_PER_FRAME_8:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_8 << LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_16:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_16 << LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_32:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_32 << LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_64:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_64 << LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_128:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_128 << LPAIF_PCM_CTLa__RATE___S);
         break;
      case BITS_PER_FRAME_256:
         field_value |= (LPAIF_PCM_CTLa__RATE__ENUM_256 << LPAIF_PCM_CTLa__RATE___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   /* config num of slots and and slot number */
   if (PCM_SINK == dir)
   {  
      field_value |= ((config_ptr->slot_num - 1) << LPAIF_PCM_CTLa__TPCM_SLOT_NUM___S) &
                                         LPAIF_PCM_CTLa__TPCM_SLOT_NUM___M;     
      field_value |= ((config_ptr->slot[0] << LPAIF_PCM_CTLa__TPCM_SLOT___S) & 
                                        LPAIF_PCM_CTLa__TPCM_SLOT___M) ;
   }
   else
   {  
      field_value |= ((config_ptr->slot_num - 1) << LPAIF_PCM_CTLa__RPCM_SLOT_NUM___S) &
                                         LPAIF_PCM_CTLa__RPCM_SLOT_NUM___M;  
      field_value |= ((config_ptr->slot[0] << LPAIF_PCM_CTLa__RPCM_SLOT___S) & 
                                        LPAIF_PCM_CTLa__RPCM_SLOT___M) ;
   }




   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register

   /* Number of active slot */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Num active slots = %d" , config_ptr->slot_num);
   /* All four slots */
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 0 = %d", config_ptr->slot[0]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 1 = %d", config_ptr->slot[1]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 2 = %d", config_ptr->slot[2]);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== Slot 3 = %d", config_ptr->slot[3]);

   if (PCM_SINK == dir)
   {
      mask = ( LPAIF_PCM_SLOT_CTLa__TPCM_SLOT1___M |
                LPAIF_PCM_SLOT_CTLa__TPCM_SLOT2___M |
                LPAIF_PCM_SLOT_CTLa__TPCM_SLOT3___M);
   }
   else
   {
      mask = ( LPAIF_PCM_SLOT_CTLa__RPCM_SLOT1___M |
                LPAIF_PCM_SLOT_CTLa__RPCM_SLOT2___M |
                LPAIF_PCM_SLOT_CTLa__RPCM_SLOT3___M );
   }

   
   /* config num of slots and and slot number */
   if (PCM_SINK == dir)
   {  
      field_value = ((config_ptr->slot[1]) << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT1___S) &
                                         LPAIF_PCM_SLOT_CTLa__TPCM_SLOT1___M;     
      field_value |= ((config_ptr->slot[2] << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT2___S) & 
                                        LPAIF_PCM_SLOT_CTLa__TPCM_SLOT2___M) ;
      field_value |= ((config_ptr->slot[3] << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT3___S) & 
                                        LPAIF_PCM_SLOT_CTLa__TPCM_SLOT3___M) ;
   }
   else
   {  
      field_value = ((config_ptr->slot[1]) << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT1___S) &
                                         LPAIF_PCM_SLOT_CTLa__RPCM_SLOT1___M;     
      field_value |= ((config_ptr->slot[2] << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT2___S) & 
                                        LPAIF_PCM_SLOT_CTLa__RPCM_SLOT2___M) ;
      field_value |= ((config_ptr->slot[3] << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT3___S) & 
                                        LPAIF_PCM_SLOT_CTLa__RPCM_SLOT3___M) ;
   }

   UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_SLOT_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * Enable the specified PCM instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir    PCM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_pcm_enable_v1(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   uint32_t opposite_dir =  (dir & PCM_SOURCE) ? PCM_SINK: PCM_SOURCE;

   /* enable only once for the last */
   if(hal_pcm_start == pcm_intf_state[pcm_index][opposite_dir])
   {

      field_value = LPAIF_PCM_CTLa__ENABLE__ENABLE << LPAIF_PCM_CTLa__ENABLE___S;
      mask = LPAIF_PCM_CTLa__ENABLE___M;

      UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
      pcm_intf_state[pcm_index][dir] = hal_pcm_start;
   }
   else
   {
      pcm_intf_state[pcm_index][dir] = hal_pcm_start;
   }
}


/** 
 * Disable the specified I2S instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir   PCM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_pcm_disable_v1(uint32_t pcm_index, uint32_t dir)
{
   uint32_t mask,field_value;
   uint32_t opposite_dir =  (dir & PCM_SOURCE) ? PCM_SINK: PCM_SOURCE;

   //only disable for the first stop for TX or RX.
   if(hal_pcm_start == pcm_intf_state[pcm_index][opposite_dir])
   {
      field_value = LPAIF_PCM_CTLa__ENABLE__DISABLE << LPAIF_PCM_CTLa__ENABLE___S;
      mask = LPAIF_PCM_CTLa__ENABLE___M;

      UpdateRegister1(pcm_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(pcm_index)), pcm_virt_offset,mask,field_value,0); ///< update register
      pcm_intf_state[pcm_index][dir] = hal_pcm_stop;
   }
   else
   {
      pcm_intf_state[pcm_index][dir] = hal_pcm_stop;
   }
}


/** 
 * get the register value for the specified PCM index  
 * 
 * @param[in] pcm_index   PCM index
 *
 * @return   the register value for the specified PCM index  
 */
uint32 hal_pcm_get_intf_v1(uint32_t pcm_index, uint32_t dir)
{
   uint32_t field_value = 0;
   
   switch (pcm_index)
   {
      case PCM_PRIMARY:
        if(PCM_SINK == dir)
        {
          field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__PCM;
        }
        else
        {
          field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__PCM;
        }
         break;
      case PCM_SECONDARY:
        if(PCM_SINK == dir)
        {
          field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__SEC_PCM;
        }
        else
        {
          field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__SEC_PCM;
        }
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
          field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__NONE;
         break;
   }
  
   return field_value;
}

#ifdef AUDIOIF_REG_DUMP  
static void hal_pcm_dump_v1(uint32_t intf_index, uint32_t dump_seq)
{
   if(MAX_PCM_TYPE_NUM_V1 > intf_index)
   {         
      ReadRegister1(audioif_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(intf_index)),pcm_virt_offset, &afe_hal_pcm_dump_v1.LPASS_LPAIF_PCM_CTLa_DUMP[intf_index][dump_seq]);
      ReadRegister1(audioif_phy_base + PCM_REG_OFFSET(LPASS_LPAIF_PCM_SLOT_CTLa(intf_index)), pcm_virt_offset,&afe_hal_pcm_dump_v1.LPASS_LPAIF_PCM_SLOT_CTLa_DUMP[intf_index][dump_seq]);      
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_I2S_CTLa(addr: 0x%x) = 0x%x", LPASS_LPAIF_PCM_CTLa(intf_index), afe_hal_pcm_dump_v1.LPASS_LPAIF_PCM_CTLa_DUMP[intf_index][dump_seq]);  
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "==== LPASS_LPAIF_PCM_SLOT_CTLa(addr: 0x%x) = 0x%x",LPASS_LPAIF_PCM_SLOT_CTLa(intf_index), afe_hal_pcm_dump_v1.LPASS_LPAIF_PCM_SLOT_CTLa_DUMP[intf_index][dump_seq]);         
   }
}
               
#endif
               

