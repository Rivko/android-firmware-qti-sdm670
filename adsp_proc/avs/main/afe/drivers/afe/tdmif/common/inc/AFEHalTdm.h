#ifndef __AFE_HAL_TDM_H__
#define __AFE_HAL_TDM_H__

/** @file AFEHalTdm.h
    This module contains Audio Hardware interface Abstraction layer function 
    declarations. This file publicizes enums defined by the HAL. 

    Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/inc/AFEHalTdm.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * The following structures is used for I2S interface
 */
typedef struct tdm_config
{
   // TODO: Need to update the variable format, like, slot_num
   /** Master/slave mode*/
   uint32_t sync_src;
   /** short/long sync type */
   uint32_t sync_type;
   /** num of active channel: max is 8 */
   uint32_t num_channels;
   /** num of bits in a sample */
   uint32_t bit_width;
   /** num of bits in a slot */
   uint32_t slot_width;
   /** num of slots in a frame */
   uint32_t nslots_per_frame;
   /** Invert sync: Only for long sync mode */
   uint32_t sync_invert;
   /** data delay w.r.t. sync */
   uint32_t sync_data_delay;
   /** OE setup */
   uint32_t ctrl_data_oe;
   /** which channel to enable */
   uint32_t slot_mask;

} __attribute__((aligned(4))) tdm_config_t;

/**
  @brief Perform TDM HAL Initialization

  @param[in] version Hardware version of AudioIF
             phy_base physical address of AUDIOIF base address
             virt_base virtual address of AUDIOIF base address
             tdm_offset tdm reg base addr LPASS_LPAIF_PCM_CTLa
  @return  void

*/
void hal_tdm_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t tdm_offset);

/**
 * resets the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_reset(uint32_t tdm_index, uint32_t dir);

/**
 * releases the reset for the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_reset_release(uint32_t tdm_index, uint32_t dir);

/**
 * config the instance of the TDM  specified .
 *
 * @param[in] tdm_index     TDM instance that needs to be reset.
 * @param[in] dir       0 : Sink, 1:  Source path
 * @param[in] configPtr Pointer to the configuration parameters.
 * Registers Affected:
 *                LPAIF_PCM_CTLa
 *                LPAIF_PCM_TDM_CTL_a
 *                LPAIF_PCM_TPCM_SLOT_NUM_a
 *                LPAIF_PCM_RPCM_SLOT_NUM_a
 *
 */
void hal_tdm_config(uint32_t tdm_index, uint32_t dir, tdm_config_t *config_ptr);

/**
 * Enable the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_enable(uint32_t tdm_index, uint32_t dir);

/**
 * Disable the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir   TDM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_disable(uint32_t tdm_index, uint32_t dir);

/**
 * get the register value for the specified TDM index
 *
 * @param[in] tdm_index   TDM index
 *
 * @return   the register value for the specified TDM index
 */
uint32_t hal_tdm_get_intf(uint32_t tdm_index, uint32_t dir);


#if defined(__cplusplus)
}
#endif

#endif  // !defined(__AFE_HAL_TDM_H__)
