/** @file aud_sync_hal.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History


when       who     what, where, why
------     ---    -------------------------------------------------------
05/31/11    rb     created the document

========================================================================== */
#include "qurt_elite.h"
#include "aud_sync_hal_v2.h"
#include "LPASS_ADDRESS_AVTIMER_V2.h"

#define AUDSYNC_OFFSET(_reg_) ((_reg_)- LPASS_AUD_SYNC_CTL) 

/**
 * Note that since AV timer is used in AFE while reading from/writing to circular buffer & to rate-match by adjusting AV timer period,
 * the clocks used to trigger sample_counter in aud_sync hardware block also should be triggered by the same clock
 * that triggers AV timer. Hence #if are used.
 *
 * L,M values below divide the crystal clock to match the sample rate of the port.
 *
 * Sampling rate to L,M depends on clock (known at compile time).
 * The below values are obtained as follows : sample_rate * (L+M/4096) = clock. where L,M are ints.
 * L,M values depend on clock and sampling rate. Sampling rate is index by sampling_rate_t enum:
 *                    8k       16k      48k
 */
//#elif (AVTIMER_CLOCK_SOURCE == LCC_AVT_NS__AVT_GFM_SRC_SEL__CXO)  //CXO : 19.2 MHz 
//                                          8k       16k           48k 
static   uint16_t L_ARRAY[] = {2400,   1200,    400 }; 
static   uint16_t M_ARRAY[] = {0,      0,       0}; 


/**
 * table for aud_sync mask values. indexed by aud_sync_mask_table[hal_aud_sync_intf_t][hal_aud_sync_phase_adj_t]
 * i.e rows = inf. column = phase adj val
 */
static uint32_t aud_sync_mask_table[3][3] = {
      //-1                                      0                                                                       +11
      {AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___M, AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___M | AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___M, AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___M}, //BT_1
      {AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___M, AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___M | AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___M, AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___M}, //BT_2
      {AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___M,  AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___M  | AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___M,  AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___M}   //FM
};
/**
 * table for aud_sync field values. indexed by aud_sync_field_value_table[hal_aud_sync_intf_t][hal_aud_sync_phase_adj_t]
 */
static uint32_t aud_sync_field_value_table[3][3] = {
      //-1                                        0               +1
      {1L << AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___S, 0, 1L << AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___S}, //BT_1
      {1L << AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___S, 0, 1L << AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___S}, //BT_2
      {1L << AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___S,  0, 1L << AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___S}   //FM
};

/**
 * Address related definitions
 */
static volatile uint32   audsync_base_phys_addr = 0; 
static volatile int32_t  audsync_virt_offset = 0;
static uint16_t *ptr_l_array;
static uint16_t *ptr_m_array;


/**
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 *
 * @param[in]  base_ptr Base virtual address of the audio hardware registes.
 * @param[out] version Version of the implementation of Audio HAL.
 */
void hal_aud_sync_init_v2(avtimer_hw_cfg_t *avt_hw_cfg)
{
   if(0 != avt_hw_cfg->audsync_offset)
   {
      audsync_base_phys_addr = avt_hw_cfg->avtimer_base_phys_addr;
      
      ///< Calculate offset of virtual address in words (4 bytes)
      audsync_virt_offset = reg_virt_offset(avt_hw_cfg->avtimer_base_virt_addr, avt_hw_cfg->avtimer_base_phys_addr, avt_hw_cfg->audsync_offset);

      ///< Setup L and M array (now only 19.2MHz, can extend using xo_val in dev config)
      ptr_l_array = &L_ARRAY[0];
      ptr_m_array = &M_ARRAY[0];
   }
}

void hal_aud_sync_reset_all_v2(void)
{
   //reset all registers
   // control
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), 
      audsync_virt_offset, 0xFFFFFFFF, 0, 0);   
   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_BT_1);
   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_BT_2);
   hal_aud_sync_reset_tracking(HAL_AUD_SYNC_INTF_FM);
   //status
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_STATUS), 
      audsync_virt_offset, 0xFFFFFFFF, 1, 0);
   //ref count increment for BT and FM
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_REF_INC),
      audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_FM_REF_INC),
      audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   //XO DIV number
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_XO_DIV_MOD),
      audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_FM_XO_DIV_MOD),
      audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   //load value for sample counter for BT and FM
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_COUNT_LOAD_XO1),  audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_COUNT_LOAD_XO2),  audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_FM_COUNT_LOAD_XO),   audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   //load value for reference counter for BT and FM
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_COUNT_LOAD_REF1), audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_BT_COUNT_LOAD_REF2), audsync_virt_offset, 0xFFFFFFFF, 0, 0);
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_FM_COUNT_LOAD_REF),  audsync_virt_offset, 0xFFFFFFFF, 0, 0);

   /**
    * remaining registers are read only
    * * sample counters
    * AUD_SYNC_BT_PHASE_CHK_XO1
    * AUD_SYNC_BT_PHASE_CHK_XO2
    * AUD_SYNC_FM_PHASE_CHK_XO
    * * reference counters
    * AUD_SYNC_BT_PHASE_CHK_REF1
    * AUD_SYNC_BT_PHASE_CHK_REF2
    * AUD_SYNC_FM_PHASE_CHK_REF
    * * VFR counters
    * AUD_SYNC_BT_VFR_TIMESTAMP
    * AUD_SYNC_FM_VFR_TIMESTAMP
    */
}

/**
 * enables the clocks using LCC_AVT_NS register
 * The clock source used for AV timer of 8960: PXO
 * The same XO should be used for audio synchronizer.
 *
 * @param[in] intf audio sync interface
 * @param[out] none
 */
void hal_aud_sync_enable_clocks_v2(hal_aud_sync_intf_t intf)
{
}

/**
 * disables the clocks using LCC_AVT_NS register
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_disable_clocks_v2(hal_aud_sync_intf_t intf)
{
}

/**
 * Set the VFR source as specified (external/internal).
 *
 * @param[in] vfr_src VFR source
 */
void hal_aud_sync_set_vfr_source_v2(hal_aud_sync_vfr_source_t vfr_src)
{
   uint32_t mask, field_value;
   mask        = AUD_SYNC_CTL__VFR_INT_SEL___M;             // Setup the mask
   field_value = ((uint32_t)vfr_src << AUD_SYNC_CTL__VFR_INT_SEL___S) ;

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, mask, field_value, 0);
}

/**
 * Get the VFR source
 *
 * @param[out] vfr_src_ptr Pointer to VFR source
 */
void hal_aud_sync_get_vfr_source_v2(hal_aud_sync_vfr_source_t *vfr_src_ptr)
{
   uint32_t value;
   
   ReadRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, &value);
   
   value &= AUD_SYNC_CTL__VFR_INT_SEL___M;
   value >>= AUD_SYNC_CTL__VFR_INT_SEL___S;

   *vfr_src_ptr = (hal_aud_sync_vfr_source_t) value;
}

/**
 * Adjusts the phase between sample counter and reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the phase adjustment
 *
 * @param[out] none
 */

void hal_aud_sync_adj_phase_v2(hal_aud_sync_intf_t intf, hal_aud_sync_phase_adj_t val, uint32_t *mask, uint32_t *field_value)
{
   //uint32_t mask=0, field_value=0;

   *mask        = aud_sync_mask_table[(int16_t)intf][(int16_t)(val-HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE)];
   *field_value = aud_sync_field_value_table[(int16_t)intf][(int16_t)(val-HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE)];

#if 0
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         switch(val)
         {
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___S;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_ZERO:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE1___M | AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___M;
               *field_value = 0;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_PLUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__BT_PH_ADJ_POS_WE1___S;
               break;
            }
            default:
            {
               AUD_SYNC_ASSERT(0);
            }
         }
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         switch(val)
         {
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___S;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_ZERO:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_NEG_WE2___M | AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___M;
               *field_value = 0;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_PLUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__BT_PH_ADJ_POS_WE2___S;
               break;
            }
            default:
            {
               AUD_SYNC_ASSERT(0);
            }
         }
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         switch(val)
         {
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_MINUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___S;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_ZERO:
            {
               *mask        = AUD_SYNC_CTL__FM_PH_ADJ_NEG_WE___M | AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___M;
               *field_value = 0;
               break;
            }
            case HAL_AUD_SYNC_PHASE_ADJ_VAL_PLUS_ONE:
            {
               *mask        = AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___M;
               *field_value = (uint32_t)1 << AUD_SYNC_CTL__FM_PH_ADJ_POS_WE___S;
               break;
            }
            default:
            {
               AUD_SYNC_ASSERT(0);
            }
         }
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }
#endif
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, *mask, *field_value, 0);
}

/**
 * enable tracking for the interface & also enables the XO div
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_enable_tracking_v2(hal_aud_sync_intf_t intf)
{
   uint32_t mask=0, field_value=0;
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         mask        = AUD_SYNC_CTL__BT_PH_TRK_EN___M | AUD_SYNC_CTL__BT_XO_DIV_EN___M;
         field_value = ( (uint32_t)AUD_SYNC_CTL__BT_PH_TRK_EN__ENABLE << AUD_SYNC_CTL__BT_PH_TRK_EN___S ) |
                       ( (uint32_t)AUD_SYNC_CTL__BT_XO_DIV_EN__ENABLE << AUD_SYNC_CTL__BT_XO_DIV_EN___S );
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         mask        = AUD_SYNC_CTL__FM_PH_TRK_EN___M | AUD_SYNC_CTL__FM_XO_DIV_EN___M;
         field_value = ( (uint32_t)AUD_SYNC_CTL__FM_PH_TRK_EN__ENABLE << AUD_SYNC_CTL__FM_PH_TRK_EN___S ) |
                       ( (uint32_t)AUD_SYNC_CTL__FM_XO_DIV_EN__ENABLE << AUD_SYNC_CTL__FM_XO_DIV_EN___S );
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, mask, field_value, 0);
}

/**
 * disable tracking for the interface & also disables the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_disable_tracking_v2(hal_aud_sync_intf_t intf)
{
   uint32_t mask=0, field_value=0;
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         mask        = AUD_SYNC_CTL__BT_PH_TRK_EN___M | AUD_SYNC_CTL__BT_XO_DIV_EN___M;
         field_value = 0;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         mask        = AUD_SYNC_CTL__FM_PH_TRK_EN___M | AUD_SYNC_CTL__FM_XO_DIV_EN___M;
         field_value = 0;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, mask, field_value, 0);
}

/**
 * resets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_reset_tracking_v2(hal_aud_sync_intf_t intf)
{
   uint32_t mask=0, field_value=0;
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         mask        = AUD_SYNC_CTL__BT_PH_TRK_RESET___M | AUD_SYNC_CTL__BT_XO_DIV_RESET___M;
         field_value = ((uint32_t)AUD_SYNC_CTL__BT_PH_TRK_RESET__RESET << AUD_SYNC_CTL__BT_PH_TRK_RESET___S) | \
                       ((uint32_t)AUD_SYNC_CTL__BT_XO_DIV_RESET__RESET << AUD_SYNC_CTL__BT_XO_DIV_RESET___S);
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         mask        = AUD_SYNC_CTL__FM_PH_TRK_RESET___M | AUD_SYNC_CTL__FM_XO_DIV_RESET___M;
         field_value = ((uint32_t)AUD_SYNC_CTL__FM_PH_TRK_RESET__RESET << AUD_SYNC_CTL__FM_PH_TRK_RESET___S) | \
                       ((uint32_t)AUD_SYNC_CTL__FM_XO_DIV_RESET__RESET << AUD_SYNC_CTL__FM_XO_DIV_RESET___S);
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, mask, field_value, 0);
}
/**
 * unresets tracking for the interface & also reset the XO div
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] none
 */
void hal_aud_sync_unreset_tracking_v2(hal_aud_sync_intf_t intf)
{
   uint32_t mask=0, field_value=0;
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         mask        = AUD_SYNC_CTL__BT_PH_TRK_RESET___M | AUD_SYNC_CTL__BT_XO_DIV_RESET___M;
         field_value = 0;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         mask        = AUD_SYNC_CTL__FM_PH_TRK_RESET___M | AUD_SYNC_CTL__FM_XO_DIV_RESET___M;
         field_value = 0;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_CTL), audsync_virt_offset, mask, field_value, 0);
}

/**
 * gets the overflow status of the reference counter & sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] ref_ovf_status overflow status of the reference counter
 * @param[out] smp_ovf_status overflow status of the sample counter
 */
void hal_aud_sync_get_ovf_status_v2(hal_aud_sync_intf_t intf, uint16_t *ref_ovf_status, uint16_t *smp_ovf_status)
{
   uint32_t ref_value=0, smp_value=0, value=0;

   ReadRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_STATUS), audsync_virt_offset, &value);
   
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         ref_value = value & AUD_SYNC_STATUS__BT_COUNT_OVF_REF1___M;
         ref_value >>= AUD_SYNC_STATUS__BT_COUNT_OVF_REF1___S;

         smp_value = value & AUD_SYNC_STATUS__BT_COUNT_OVF_XO1___M;
         smp_value >>= AUD_SYNC_STATUS__BT_COUNT_OVF_XO1___S;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         ref_value = value & AUD_SYNC_STATUS__BT_COUNT_OVF_REF2___M;
         ref_value >>= AUD_SYNC_STATUS__BT_COUNT_OVF_REF2___S;

         smp_value = value & AUD_SYNC_STATUS__BT_COUNT_OVF_XO2___M;
         smp_value >>= AUD_SYNC_STATUS__BT_COUNT_OVF_XO2___S;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         ref_value = value & AUD_SYNC_STATUS__FM_COUNT_OVF_REF___M;
         ref_value >>= AUD_SYNC_STATUS__FM_COUNT_OVF_REF___S;

         smp_value = value & AUD_SYNC_STATUS__FM_COUNT_OVF_XO___M;
         smp_value >>= AUD_SYNC_STATUS__FM_COUNT_OVF_XO___S;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   *ref_ovf_status = (uint16_t) ref_value;
   *smp_ovf_status = (uint16_t) smp_value;
}


/**
 * clears the aud_sync status register by writing 1
 *
 * @param[in] none
 *
 * @param[out] none
 */
void hal_aud_sync_clear_status_v2(void)
{
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(LPASS_AUD_SYNC_STATUS), audsync_virt_offset, AUD_SYNC_STATUS___M, 0x1, 0);
}

/**
 * sets the value by which reference count increments
 *
 * @param[in] intf audio sync interface
 * @param[in] ref_cnt_inc  value of ref count increment
 *
 * @param[out] none
 */
void hal_aud_sync_set_ref_cnt_inc_v2(hal_aud_sync_intf_t intf, uint16_t ref_cnt_inc)
{
   uint32_t mask=0, field_value=0, register_addr=0;
   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_REF_INC;
         mask           = AUD_SYNC_BT_REF_INC__REF_INC1___M;
         field_value    = ((uint32_t)ref_cnt_inc) << AUD_SYNC_BT_REF_INC__REF_INC1___S;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         register_addr  = LPASS_AUD_SYNC_BT_REF_INC;
         mask           = AUD_SYNC_BT_REF_INC__REF_INC2___M;
         field_value    = ((uint32_t)ref_cnt_inc) << AUD_SYNC_BT_REF_INC__REF_INC2___S;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_REF_INC;
         mask           = AUD_SYNC_FM_REF_INC__REF_INC1___M;
         field_value    = ((uint32_t)ref_cnt_inc) << AUD_SYNC_FM_REF_INC__REF_INC1___S;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, mask, field_value, 0);
}

/**
 * sets the value by which crystal oscillator is divided
 *
 * @param[in] intf audio sync interface
 * @param[in] L  cystal is divided by value = L+M/N (where N is 4096). L is the int portion.
 * @param[in] M  cystal is divided by value = L+M/N (where N is 4096). M is the fractional portion.
 *
 * @param[out] none
 */
void hal_aud_sync_set_xo_div_val_v2(hal_aud_sync_intf_t intf,uint32_t sample_rate)
{
   uint32_t mask=0, field_value=0, register_addr=0;
   sampling_rate_t sample_rate_index = sample_rate_2_index(sample_rate);

   if((FOURTY_EIGHT_K >= sample_rate_index) && (0 <= sample_rate_index))
   {
      switch(intf)
      {
        case HAL_AUD_SYNC_INTF_BT_1:
        {
          register_addr  = LPASS_AUD_SYNC_BT_XO_DIV_MOD;
          mask           = AUD_SYNC_BT_XO_DIV_MOD__L___M | AUD_SYNC_BT_XO_DIV_MOD__M___M;
          field_value    = ( (((uint32_t)ptr_l_array[sample_rate_index]) << AUD_SYNC_BT_XO_DIV_MOD__L___S) & AUD_SYNC_BT_XO_DIV_MOD__L___M ) |
                 ( (((uint32_t)ptr_m_array[sample_rate_index]) << AUD_SYNC_BT_XO_DIV_MOD__M___S) & AUD_SYNC_BT_XO_DIV_MOD__M___M ) ;
          break;
        }
        case HAL_AUD_SYNC_INTF_BT_2:
        {
          AUD_SYNC_ASSERT(0);
          break;
        }
        case HAL_AUD_SYNC_INTF_FM:
        {
          register_addr  = LPASS_AUD_SYNC_FM_XO_DIV_MOD;
          mask           = AUD_SYNC_FM_XO_DIV_MOD__L___M | AUD_SYNC_FM_XO_DIV_MOD__M___M;
          field_value    = ( (((uint32_t)ptr_l_array[sample_rate_index]) << AUD_SYNC_FM_XO_DIV_MOD__L___S) & AUD_SYNC_FM_XO_DIV_MOD__L___M ) |
                 ( (((uint32_t)ptr_m_array[sample_rate_index]) << AUD_SYNC_FM_XO_DIV_MOD__M___S) & AUD_SYNC_FM_XO_DIV_MOD__M___M ) ;
          break;
        }
        default:
        {
          AUD_SYNC_ASSERT(0);
        }
      }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, mask, field_value, 0);

   }
   else
   {
      AUD_SYNC_ASSERT(0);
   }
}

/**
 * gets the value of the sample counter
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the sample counter
 *
 */
void hal_aud_sync_get_smp_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *sample_cnt)
{
   uint32_t register_addr=0;
   *sample_cnt = 0;

   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_PHASE_CHK_XO1;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         register_addr  = LPASS_AUD_SYNC_BT_PHASE_CHK_XO2;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_PHASE_CHK_XO;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   ReadRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, sample_cnt);
}

/**
 * gets the value of the reference counter in samples
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] sample_cnt  value of the reference counter
 */
void hal_aud_sync_get_ref_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *reference_cnt)
{
   uint32_t register_addr=0;
   *reference_cnt = 0;

   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_PHASE_CHK_REF1;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         register_addr  = LPASS_AUD_SYNC_BT_PHASE_CHK_REF2;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_PHASE_CHK_REF;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   ReadRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, reference_cnt);
}

/**
 * gets the value of the VFR timestamp in samples
 *
 * @param[in] intf audio sync interface
 *
 * @param[out] vfr_cnt  value of the VFR timestamp.
 */
void hal_aud_sync_get_vfr_cnt_v2(hal_aud_sync_intf_t intf, uint32_t *vfr_cnt)
{
   uint32_t register_addr=0;
   *vfr_cnt = 0;

   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_VFR_TIMESTAMP;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         AUD_SYNC_ASSERT(0);
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_VFR_TIMESTAMP;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   ReadRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, vfr_cnt);
}

/**
 * sets the sample counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the sample counter
 *
 * @param[out] none
 */
void hal_aud_sync_set_smp_cnt_v2(hal_aud_sync_intf_t intf, uint32_t sample_cnt)
{
   uint32_t register_addr=0;

   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_COUNT_LOAD_XO1;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         register_addr  = LPASS_AUD_SYNC_BT_COUNT_LOAD_XO2;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_COUNT_LOAD_XO;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }
   
   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, 0xFFFFFFFF, sample_cnt, 0);
}

/**
 * sets the reference counter
 *
 * @param[in] intf audio sync interface
 * @param[in] val  value of the reference counter
 *
 * @param[out] none
 */
void hal_aud_sync_set_ref_cnt_v2(hal_aud_sync_intf_t intf, uint32_t reference_cnt)
{
   uint32_t register_addr=0;

   switch(intf)
   {
      case HAL_AUD_SYNC_INTF_BT_1:
      {
         register_addr  = LPASS_AUD_SYNC_BT_COUNT_LOAD_REF1;
         break;
      }
      case HAL_AUD_SYNC_INTF_BT_2:
      {
         register_addr  = LPASS_AUD_SYNC_BT_COUNT_LOAD_REF2;
         break;
      }
      case HAL_AUD_SYNC_INTF_FM:
      {
         register_addr  = LPASS_AUD_SYNC_FM_COUNT_LOAD_REF;
         break;
      }
      default:
      {
         AUD_SYNC_ASSERT(0);
      }
   }

   UpdateRegister1(audsync_base_phys_addr + AUDSYNC_OFFSET(register_addr), audsync_virt_offset, 0xFFFFFFFF, reference_cnt, 0);
}
