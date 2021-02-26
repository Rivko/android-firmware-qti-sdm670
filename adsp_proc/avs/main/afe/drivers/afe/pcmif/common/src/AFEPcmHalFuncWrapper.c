/** @file HALaudioif_wrapper.c
    This module implements the interface defined by the Hardware abstraction
    layer.

    Copyright (c) 2013-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/src/AFEPcmHalFuncWrapper.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "qurt_elite.h"
#include "AFEPcmHalFuncDefs.h"
#include "AFEHalPcm.h"
#include "lpasshwio_devcfg.h"

//global data
/* AFE PCM HAL  function table */
static afe_pcm_hal_func_def_t  afe_pcm_hal_table;


inline void hal_pcm_init(uint32_t version, uint32_t phy_base, uint32_t virt_base,  uint32_t pcm_offset)
{

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " hal_pcm_init() is called version 0x%x", version);

  switch(version)
  {
    case LPASS_HW_VER_3_8_0:
    case LPASS_HW_VER_3_3_0:
    {
      hal_pcm_get_func_table_v1(&afe_pcm_hal_table);
    }
    break;
    case LPASS_HW_VER_3_10_0:
    {
      hal_pcm_get_func_table_v3(&afe_pcm_hal_table);
    }
    break;
    case LPASS_HW_VER_4_0_0:
    case LPASS_HW_VER_4_0_2:
    case LPASS_HW_VER_4_0_3:
    case LPASS_HW_VER_4_1_0:
    case LPASS_HW_VER_4_2_0:
    {
      hal_pcm_get_func_table_v2(&afe_pcm_hal_table);
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " unknown pcm hal version 0x%x", version);
    }
    break;
  }

  afe_pcm_hal_table.hal_pcm_init_fptr(version, phy_base, virt_base, pcm_offset);

}

inline void hal_pcm_reset(uint32_t pcm_index, uint32_t dir)
{
  afe_pcm_hal_table.hal_pcm_reset_fptr(pcm_index, dir);

}

inline void hal_pcm_reset_release(uint32_t pcm_index, uint32_t dir)
{
  afe_pcm_hal_table.hal_pcm_reset_release_fptr(pcm_index, dir);
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
void hal_pcm_config(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr)
{
  afe_pcm_hal_table.hal_pcm_config_fptr(pcm_index, dir, config_ptr);
}


/** 
 * Enable the specified PCM instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir    PCM instance direction (source/sink).
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
void hal_pcm_enable(uint32_t pcm_index, uint32_t dir)
{
  afe_pcm_hal_table.hal_pcm_enable_fptr(pcm_index, dir);
}

/** 
 * Disable the specified I2S instances.
 * 
 * @param[in] pcm_index  PCM Instance to be enabled.
 * @param[in] dir   PCM instance direction (source/sink)
 * Registers Affected:
 *               1) LPAIF_PCM_CTLa 
 */
inline void hal_pcm_disable(uint32_t pcm_index, uint32_t dir)
{
  afe_pcm_hal_table.hal_pcm_disable_fptr(pcm_index, dir);
}


/** 
 * get the register value for the specified PCM index  
 * 
 * @param[in] pcm_index   PCM index
 *
 * @return   the register value for the specified PCM index  
 */
inline uint32 hal_pcm_get_intf(uint32_t pcm_index, uint32_t dir)
{

  return afe_pcm_hal_table.hal_pcm_get_intf_fptr(pcm_index, dir);
}

#ifdef AUDIOIF_REG_DUMP  
void hal_pcm_dump(uint32_t pcm_index, uint32_t dir, uint32_t dump_seq)
{
  return afe_pcm_hal_table.hal_pcm_dump_fptr(pcm_index, dir, dump_seq);
}
#endif


