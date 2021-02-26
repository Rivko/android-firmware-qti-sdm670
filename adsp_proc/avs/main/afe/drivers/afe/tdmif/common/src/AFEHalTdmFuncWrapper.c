/** @file AFEHalTdmFuncWrapper.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2016-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/tdmif/common/src/AFEHalTdmFuncWrapper.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */

#include "qurt_elite.h"
#include "AFETdmDriver_i.h"
#include "AFEHalTdm.h"
#include "AFEHalTdmFuncDefs.h" 

/* AFE TDM HAL  function table */
static afe_tdm_hal_func_def_t  afe_tdm_hal_table;

/**
  @brief Perform TDM HAL Initialization

  @param[in] version Hardware version of AudioIF
             phy_base physical address of AUDIOIF base address
             virt_base virtual address of AUDIOIF base address
             tdm_offset tdm reg base addr LPASS_LPAIF_PCM_CTLa
  @return  void

*/
void hal_tdm_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t tdm_offset)
{

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " hal_tdm_init() is called version 0x%x", version);

  switch (version)
  {
     case LPASS_HW_VER_4_0_0:
     case LPASS_HW_VER_4_0_2:
     case LPASS_HW_VER_4_0_3:
     case LPASS_HW_VER_4_1_0:
     case LPASS_HW_VER_4_2_0:
     {
        hal_tdm_get_func_table_v1(&afe_tdm_hal_table);
        break;
     }
     case LPASS_HW_VER_3_10_0:
     {
        hal_tdm_get_func_table_v2(&afe_tdm_hal_table);
        break;
     }
     case LPASS_HW_VER_3_3_0:
     {
       hal_tdm_get_func_table_v3(&afe_tdm_hal_table);
       break;
     }
     default:
     {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unknown TDM HAL version: 0x%x", version);
        break;
     }
  }

  afe_tdm_hal_table.hal_tdm_init_fptr(version, phy_base, virt_base, tdm_offset);
}

/**
 * resets the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_reset(uint32_t tdm_index, uint32_t dir)
{
  afe_tdm_hal_table.hal_tdm_reset_fptr(tdm_index, dir);
}

/**
 * releases the reset for the specified TDM instances.
 *
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa
 */
void hal_tdm_reset_release(uint32_t tdm_index, uint32_t dir)
{
  afe_tdm_hal_table.hal_tdm_reset_release_fptr(tdm_index, dir);
}


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
void hal_tdm_config(uint32_t tdm_index, uint32_t dir, tdm_config_t *configPtr)
{
  afe_tdm_hal_table.hal_tdm_config_fptr(tdm_index, dir, configPtr);
}


/** 
 * Enable the specified TDM instances.
 * 
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir    TDM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_tdm_enable(uint32_t tdm_index, uint32_t dir)
{
  afe_tdm_hal_table.hal_tdm_enable_fptr(tdm_index, dir);
}


/** 
 * Disable the specified TDM instances.
 * 
 * @param[in] tdm_index  TDM Instance to be enabled.
 * @param[in] dir   TDM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_tdm_disable(uint32_t tdm_index, uint32_t dir)
{
  afe_tdm_hal_table.hal_tdm_disable_fptr(tdm_index, dir);
}


/** 
 * get the register value for the specified TDM index  
 * 
 * @param[in] tdm_index   TDM index
 *
 * @return   the register value for the specified TDM index  
 */
uint32 hal_tdm_get_intf(uint32_t tdm_index, uint32_t dir)
{
  return afe_tdm_hal_table.hal_tdm_get_intf_fptr(tdm_index, dir);
}

