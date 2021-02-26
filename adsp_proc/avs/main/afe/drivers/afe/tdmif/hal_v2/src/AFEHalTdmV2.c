/** @file AFEHalTdm.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/hal_v2/src/AFEHalTdmV2.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
/* Istari address file */
#include "LPASS_ADDRESS_FILE_LPAIF_V3.h"

#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFETdmDriver_i.h"
#include "AFEHalTdmFuncDefs.h"
#include "AFEHalTdm.h"

#define TDM_REG_OFFSET(_reg_) ((_reg_)-LPASS_LPAIF_PCM_CTL0)
#define AFE_TDM_MAX_NUM_SLOTS 8

//global data
/**
 * Address related definitions
 */
static uint32_t                  tdm_phy_base; 
static int32_t                   tdm_virt_offset;
static uint32_t                  tdm_version;

void hal_tdm_init_v2(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t tdm_offset);
void hal_tdm_reset_v2(uint32_t tdm_index, uint32_t dir);
void hal_tdm_reset_release_v2(uint32_t tdm_index, uint32_t dir);
void hal_tdm_config_v2(uint32_t tdm_index, uint32_t dir, tdm_config_t *config_ptr);
void hal_tdm_enable_v2(uint32_t tdm_index, uint32_t dir);
void hal_tdm_disable_v2(uint32_t tdm_index, uint32_t dir);
uint32_t hal_tdm_get_intf_v2(uint32_t tdm_index, uint32_t dir);

void hal_tdm_get_func_table_v2(afe_tdm_hal_func_def_t *func_table)
{
   if(NULL == func_table)
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hal_tdm_get_func_table_v2 func_table is NULL");
     return;
   }

   func_table->hal_tdm_init_fptr                = hal_tdm_init_v2;
   func_table->hal_tdm_reset_fptr               = hal_tdm_reset_v2;
   func_table->hal_tdm_reset_release_fptr       = hal_tdm_reset_release_v2;
   func_table->hal_tdm_config_fptr              = hal_tdm_config_v2;
   func_table->hal_tdm_enable_fptr              = hal_tdm_enable_v2;
   func_table->hal_tdm_disable_fptr             = hal_tdm_disable_v2;
   func_table->hal_tdm_get_intf_fptr            = hal_tdm_get_intf_v2;
}

void hal_tdm_init_v2(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t tdm_offset)
{
   tdm_version  = version;  
   tdm_phy_base = phy_base;
   
   ///< Calculate offset of virtual address in words (4 bytes)
   tdm_virt_offset = reg_virt_offset(virt_base, phy_base, tdm_offset);


   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "tdm version:[0x%x], tdm phy base:[0x%x], tdm virt base:[0x%x], tdm virt offset:[0x%x]", \
      tdm_version, tdm_phy_base, virt_base, tdm_virt_offset );
}

void hal_tdm_reset_v2(uint32_t tdm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(TDM_SINK == dir)
   {
     //Reset transmit path
      field_value = LPAIF_PCM_CTLa__RESET_TX__ENABLE << LPAIF_PCM_CTLa__RESET_TX___S;
      mask = LPAIF_PCM_CTLa__RESET_TX___M;
   }
   else
   {
     //field_value receive path
     field_value = LPAIF_PCM_CTLa__RESET_RX__ENABLE << LPAIF_PCM_CTLa__RESET_RX___S;
     mask = LPAIF_PCM_CTLa__RESET_RX___M;
   }
   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(tdm_index)), tdm_virt_offset,
     mask,field_value,0); ///< update register
}

void hal_tdm_reset_release_v2(uint32_t tdm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(TDM_SINK == dir)
   {
     //reset release trasmit path
     field_value = LPAIF_PCM_CTLa__RESET_TX__DISABLE << LPAIF_PCM_CTLa__RESET_TX___S;
     mask = LPAIF_PCM_CTLa__RESET_TX___M;
   }
   else
   {
     //reset release receive path
     field_value = LPAIF_PCM_CTLa__RESET_RX__DISABLE << LPAIF_PCM_CTLa__RESET_RX___S;
     mask = LPAIF_PCM_CTLa__RESET_RX___M;
   }

   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(tdm_index)), 
     tdm_virt_offset,mask,field_value,0); ///< update register

}


/** 
 * config the instance of the TDM  specified .     
 * 
 * @param[in] tdm_index     TDM instance that needs to be reset.
 * @param[in] dir       0 : Sink, 1:  Source path
 * @param[in] configPtr Pointer to the configuration parameters. 
 * Registers Affeceted:
 *                LPAIF_PCM_CTLa
 *                LPAIF_PCM_TDM_CTL_a
 *                LPAIF_PCM_TPCM_SLOT_NUM_a
 *                LPAIF_PCM_RPCM_SLOT_NUM_a
 *                
 */
void hal_tdm_config_v2(uint32_t tdm_index, uint32_t dir, tdm_config_t *configPtr)
{
   uint32 mask, fieldValue;
   uint32 regOffset;
   uint32 nbitsPerFrame;

   // First step: Collect info for LPASS_LPAIF_PCM_CTLa
   fieldValue = 0;

   /* Parse slot mask and get the slot number which is going to active*/
   uint16_t active_slots_index =0;
   uint16_t num_active_slots = 0;
   uint32_t temp;
   uint32_t tdm_slot_map[AFE_TDM_MAX_NUM_SLOTS] = {0};
   for (uint8_t i=0; (i< configPtr->nslots_per_frame) && (num_active_slots<=AFE_TDM_MAX_NUM_SLOTS); i++ )
   {
     temp = (configPtr->slot_mask)& (0x00000001<<i);
     if (temp)
     {
       tdm_slot_map[active_slots_index] = i;
       active_slots_index = active_slots_index+1;
       num_active_slots++;
     }
   }
   /* Setup the mask for sync src, (short,long) sync type, oe mode */
   mask =  LPAIF_PCM_CTLa__SYNC_SRC___M |
     LPAIF_PCM_CTLa__AUX_MODE___M |
     LPAIF_PCM_CTLa__CTRL_DATA_OE___M;

   /* Setup the sync src: internal or external */
   switch (configPtr->sync_src)
   {
      case TDM_MSM_MODE_MASTER:
         fieldValue |= (LPAIF_PCM_CTLa__SYNC_SRC__INTERNAL <<
            LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      case TDM_MSM_MODE_SLAVE:
         fieldValue |= (LPAIF_PCM_CTLa__SYNC_SRC__EXTERNAL <<
            LPAIF_PCM_CTLa__SYNC_SRC___S);
         break;
      default:
         // TODO: Error case Should not happen. HAL cannot handle it gracefully.
         break;
   }

   /* Setup the sync type, short / long */
   switch(configPtr->sync_type)
   {
      case TDM_SHORT_SYNC_TYPE:
         fieldValue |= (LPAIF_PCM_CTLa__AUX_MODE__PCM <<
            LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      case TDM_LONG_SYNC_TYPE:
         fieldValue |= (LPAIF_PCM_CTLa__AUX_MODE__AUX <<
            LPAIF_PCM_CTLa__AUX_MODE___S);
         break;
      case TDM_SLOT_SYNC_TYPE:
          fieldValue |= (LPAIF_PCM_CTLa__AUX_MODE__PCM <<
             LPAIF_PCM_CTLa__AUX_MODE___S);
          fieldValue |= (LPAIF_PCM_CTLa__ONE_SLOT_SYNC_EN__ENABLE <<
             LPAIF_PCM_CTLa__ONE_SLOT_SYNC_EN___S);
          break;
      default:
         break;
   }

   /* Setup the OE */
   switch (configPtr->ctrl_data_oe)
   {
      case TDM_CTRL_DATA_OE_DISABLE:
         fieldValue |= (TDM_CTRL_DATA_OE_DISABLE << LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      case TDM_CTRL_DATA_OE_ENABLE:
         fieldValue |= (TDM_CTRL_DATA_OE_ENABLE << LPAIF_PCM_CTLa__CTRL_DATA_OE___S);
         break;
      default:
         break;
   }

   if (TDM_SINK == dir)
   {
     mask |=  LPAIF_PCM_CTLa__TPCM_SLOT___M;
     fieldValue |= ((tdm_slot_map[0] << LPAIF_PCM_CTLa__TPCM_SLOT___S) &
        LPAIF_PCM_CTLa__TPCM_SLOT___M) ;
   }
   else
   {
      mask |= LPAIF_PCM_CTLa__RPCM_SLOT___M;
      fieldValue |= ((tdm_slot_map[0] << LPAIF_PCM_CTLa__RPCM_SLOT___S) &
            LPAIF_PCM_CTLa__RPCM_SLOT___M) ;
   }

   regOffset = LPASS_LPAIF_PCM_CTLa(tdm_index);
   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(regOffset), tdm_virt_offset,mask,fieldValue,0);
   // End First step: Update LPASS_LPAIF_PCM_CTLa

   // Seoncd phase: LPAIF_PCM_TDM_CTL_a
   // Setup enable-tdm, sync-delay, rate, enable diff
   mask = LPAIF_PCM_TDM_CTL_a__ENABLE_TDM___M |
     LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY___M |
     LPAIF_PCM_TDM_CTL_a__TDM_RATE___M |
     LPAIF_PCM_TDM_CTL_a__ENABLE_DIFF_SAMPLE_WIDTH___M;

   // Enable TDM mode
   fieldValue  = LPAIF_PCM_TDM_CTL_a__ENABLE_TDM__ENABLE
     << LPAIF_PCM_TDM_CTL_a__ENABLE_TDM___S;

   // Figure out sync delay, delay 1 is the typical use case
   switch(configPtr->sync_data_delay)
   {
     case TDM_DATA_DELAY_0_CYCLE:
       fieldValue |= LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY__DELAY_0_CYCLE
         << LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY___S;
       break;
     case TDM_DATA_DELAY_1_CYCLE:
     default:
       fieldValue |= LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY__DELAY_1_CYCLE
          << LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY___S;
       break;
     case TDM_DATA_DELAY_2_CYCLE:
       fieldValue |= LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY__DELAY_2_CYCLE
          << LPAIF_PCM_TDM_CTL_a__TDM_SYNC_DELAY___S;
       break;
   }

   // Figure out the rate = slot x slot-width..  rate should be limited
   nbitsPerFrame = configPtr->nslots_per_frame * configPtr->slot_width;
   
   fieldValue |= (nbitsPerFrame - 1) << LPAIF_PCM_TDM_CTL_a__TDM_RATE___S;

   // Based on slot-width, enable or disable DIFF support
   if(configPtr->bit_width != configPtr->slot_width)
   {
     fieldValue |= LPAIF_PCM_TDM_CTL_a__ENABLE_DIFF_SAMPLE_WIDTH__ENABLE
       << LPAIF_PCM_TDM_CTL_a__ENABLE_DIFF_SAMPLE_WIDTH___S;
   }
   else
   {
     fieldValue |= LPAIF_PCM_TDM_CTL_a__ENABLE_DIFF_SAMPLE_WIDTH__DISABLE
       << LPAIF_PCM_TDM_CTL_a__ENABLE_DIFF_SAMPLE_WIDTH___S;
   }

   // Invert sync or not only for LONG SYNC mode ??
   // Setup the width regardless of DIFF mode.
   if (TDM_SINK == dir)
   {
      mask |= LPAIF_PCM_TDM_CTL_a__TDM_INV_TPCM_SYNC___M;
      fieldValue |= configPtr->sync_invert
        << LPAIF_PCM_TDM_CTL_a__TDM_INV_TPCM_SYNC___S;

      mask |= LPAIF_PCM_TDM_CTL_a__TDM_TPCM_WIDTH___M;
      fieldValue |= (configPtr->bit_width - 1)
        << LPAIF_PCM_TDM_CTL_a__TDM_TPCM_WIDTH___S;

      mask |= LPAIF_PCM_TDM_CTL_a__TDM_TPCM_SLOT_NUM___M;
      fieldValue |= (num_active_slots -1)
         << LPAIF_PCM_TDM_CTL_a__TDM_TPCM_SLOT_NUM___S;
   }
   else
   {
      mask |= LPAIF_PCM_TDM_CTL_a__TDM_INV_RPCM_SYNC___M;
      fieldValue |= configPtr->sync_invert
        << LPAIF_PCM_TDM_CTL_a__TDM_INV_RPCM_SYNC___S;

      mask |= LPAIF_PCM_TDM_CTL_a__TDM_RPCM_WIDTH___M;
      fieldValue |= (configPtr->bit_width - 1)
        << LPAIF_PCM_TDM_CTL_a__TDM_RPCM_WIDTH___S;

      mask |= LPAIF_PCM_TDM_CTL_a__TDM_RPCM_SLOT_NUM___M;
      fieldValue |= (num_active_slots -1)
         << LPAIF_PCM_TDM_CTL_a__TDM_RPCM_SLOT_NUM___S;
   }


   regOffset = LPASS_LPAIF_PCM_TDM_CTL_a(tdm_index);
   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(regOffset), tdm_virt_offset,mask,fieldValue,0);
   // End seoncd phase: LPAIF_PCM_TDM_CTL_a

   // Phase 3: Setup LPAIF_PCM_TDM_SAMPLE_WIDTH_a if DIFF mode
   if(configPtr->bit_width != configPtr->slot_width)
   {
     MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM DIFF mode (%d) (%d)", 
       configPtr->bit_width, configPtr->slot_width);
     if(TDM_SINK == dir)
     {
       mask = LPAIF_PCM_TDM_SAMPLE_WIDTH_a__TDM_TPCM_SAMPLE_WIDTH___M;
       fieldValue = (configPtr->slot_width - 1)
         << LPAIF_PCM_TDM_SAMPLE_WIDTH_a__TDM_TPCM_SAMPLE_WIDTH___S;
     }
     else
     {
       mask = LPAIF_PCM_TDM_SAMPLE_WIDTH_a__TDM_RPCM_SAMPLE_WIDTH___M;
       fieldValue = (configPtr->slot_width - 1)
         << LPAIF_PCM_TDM_SAMPLE_WIDTH_a__TDM_RPCM_SAMPLE_WIDTH___S;
      }
     regOffset = LPASS_LPAIF_PCM_TDM_SAMPLE_WIDTH_a(tdm_index);
     UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(regOffset), tdm_virt_offset,mask,fieldValue,0);
   }
   // End Phase 3: LPAIF_PCM_TDM_SAMPLE_WIDTH_a if DIFF mode

   // Phase 4: slot mask, LPASS_LPAIF_PCM_SLOT_CTL1a
   /* slot 0 is already updated above as part of first update in LPASS_LPAIF_PCM_CTLa
      register*/
   /* cofig LPASS_LPAIF_PCM_SLOT_CTL1a,  for slot 1 -3 */
   if(num_active_slots > 1)
   {
     if (TDM_SINK == dir)
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
     if (TDM_SINK == dir)
     {
       fieldValue = ((tdm_slot_map[1]) << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT1___S) &
           LPAIF_PCM_SLOT_CTLa__TPCM_SLOT1___M;
       fieldValue |= ((tdm_slot_map[2] << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT2___S) &
           LPAIF_PCM_SLOT_CTLa__TPCM_SLOT2___M) ;
       fieldValue |= ((tdm_slot_map[3] << LPAIF_PCM_SLOT_CTLa__TPCM_SLOT3___S) &
           LPAIF_PCM_SLOT_CTLa__TPCM_SLOT3___M) ;
     }
     else
     {
       fieldValue = ((tdm_slot_map[1]) << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT1___S) &
           LPAIF_PCM_SLOT_CTLa__RPCM_SLOT1___M;
       fieldValue |= ((tdm_slot_map[2] << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT2___S) &
           LPAIF_PCM_SLOT_CTLa__RPCM_SLOT2___M) ;
       fieldValue |= ((tdm_slot_map[3] << LPAIF_PCM_SLOT_CTLa__RPCM_SLOT3___S) &
           LPAIF_PCM_SLOT_CTLa__RPCM_SLOT3___M) ;
     }
     UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_SLOT_CTLa(tdm_index)),
                     tdm_virt_offset,mask,fieldValue,0); ///< update register
   }

   /* cofig LPASS_LPAIF_PCM_SLOT_CTL2a,  for slot 4 -6 */
   if(num_active_slots > 4)
   {
     if (TDM_SINK == dir)
     {
       mask = ( LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT4___M |
           LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT5___M |
           LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT6___M);
     }
     else
     {
       mask = ( LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT4___M |
           LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT5___M |
           LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT6___M );
     }

     if (TDM_SINK == dir)
     {
       fieldValue = ((tdm_slot_map[4]) << LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT4___S) &
           LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT4___M;
       fieldValue |= ((tdm_slot_map[5] << LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT5___S) &
           LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT5___M) ;
       fieldValue |= ((tdm_slot_map[6] << LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT6___S) &
           LPAIF_PCM_SLOT_CTL2_a__TPCM_SLOT6___M) ;
     }
     else
     {
       fieldValue = ((tdm_slot_map[4]) << LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT4___S) &
           LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT4___M;
       fieldValue |= ((tdm_slot_map[5] << LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT5___S) &
           LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT5___M) ;
       fieldValue |= ((tdm_slot_map[6] << LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT6___S) &
           LPAIF_PCM_SLOT_CTL2_a__RPCM_SLOT6___M) ;
     }
     UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_SLOT_CTL2_a(tdm_index)),
                     tdm_virt_offset,mask,fieldValue,0); ///< update register
   }

   if(num_active_slots > 7)
   {
     /* cofig LPASS_LPAIF_PCM_SLOT_CTL3a,  for slot7 */
     if (TDM_SINK == dir)
     {
       mask =  LPAIF_PCM_SLOT_CTL3_a__TPCM_SLOT7___M ;
     }
     else
     {
       mask =  LPAIF_PCM_SLOT_CTL3_a__RPCM_SLOT7___M ;
     }

     if (TDM_SINK == dir)
     {
       fieldValue = ((tdm_slot_map[7]) << LPAIF_PCM_SLOT_CTL3_a__TPCM_SLOT7___S) &
           LPAIF_PCM_SLOT_CTL3_a__TPCM_SLOT7___M;
     }
     else
     {
       fieldValue = ((tdm_slot_map[7]) << LPAIF_PCM_SLOT_CTL3_a__RPCM_SLOT7___S) &
           LPAIF_PCM_SLOT_CTL3_a__RPCM_SLOT7___M;
     }
     UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_SLOT_CTL3_a(tdm_index)),
                     tdm_virt_offset,mask,fieldValue,0); ///< update register
   }
   // End Phase 4: slot mask
}


/** 
 * Enable the specified TDM instances.
 * 
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_tdm_enable_v2(uint32_t tdm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(TDM_SINK == dir)
   {
     //enable transmit path
     field_value = LPAIF_PCM_CTLa__ENABLE_TX__ENABLE << LPAIF_PCM_CTLa__ENABLE_TX___S;
     mask = LPAIF_PCM_CTLa__ENABLE_TX___M;
   }
   else
   {
     //enable receive path
     field_value = LPAIF_PCM_CTLa__ENABLE_RX__ENABLE << LPAIF_PCM_CTLa__ENABLE_RX___S;
     mask = LPAIF_PCM_CTLa__ENABLE_RX___M;
   }
 
   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(tdm_index)), 
     tdm_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * Disable the specified TDM instances.
 * 
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir   TDM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_tdm_disable_v2(uint32_t tdm_index, uint32_t dir)
{
   uint32_t mask,field_value;

   if(TDM_SINK == dir)
   {
      //disable transmit path
     field_value = LPAIF_PCM_CTLa__ENABLE_TX__DISABLE << LPAIF_PCM_CTLa__ENABLE_TX___S;
     mask = LPAIF_PCM_CTLa__ENABLE_TX___M;
   }
   else
   {
     //disable receive path
     field_value = LPAIF_PCM_CTLa__ENABLE_RX__DISABLE << LPAIF_PCM_CTLa__ENABLE_RX___S;
     mask = LPAIF_PCM_CTLa__ENABLE_RX___M;
   }
   
   UpdateRegister1(tdm_phy_base + TDM_REG_OFFSET(LPASS_LPAIF_PCM_CTLa(tdm_index)), 
     tdm_virt_offset,mask,field_value,0); ///< update register
}


/** 
 * get the register value for the specified TDM index  
 * 
 * @param[in] tdm_index   TDM index
 *
 * @return   the register value for the specified TDM index  
 */
uint32 hal_tdm_get_intf_v2(uint32_t tdm_index, uint32_t dir)
{
  uint32_t field_value = 0;

  switch (tdm_index)
  {
    case TDM_PRIMARY:
      if(TDM_SINK == dir)
      {
        field_value = LPAIF_RDDMA_CTLa__AUDIO_INTF__PCM;
      }
      else
      {
        field_value = LPAIF_WRDMA_CTLa__AUDIO_INTF__PCM;
      }
      break;
    case TDM_SECONDARY:
      if(TDM_SINK == dir)
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

