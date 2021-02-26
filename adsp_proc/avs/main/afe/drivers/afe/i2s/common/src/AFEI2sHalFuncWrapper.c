/** @file HALaudioif_wrapper.c
   This module implements the interface defined by the Hardware abstraction
   layer.

   Copyright (c) 2013-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
   QUALCOMM Proprietary.  Export of this technology or software is regulated
   by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/ /*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sHalFuncWrapper.c#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------

========================================================================== */
#include "qurt_elite.h"
#include "AFEI2sHalFuncDefs.h"
#include "AFEHalI2s.h"
#include "lpasshwio_devcfg.h"

//global data
/* AFE PCM HAL  function table */
static afe_i2s_hal_func_def_t  afe_i2s_hal_table;

inline void hal_i2s_init(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t i2s_offset, uint32_t int_i2s_offset)
{

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " hal_i2s_init() is called version 0x%x", version);

   switch (version)
   {
      case LPASS_HW_VER_3_8_0:
      case LPASS_HW_VER_3_10_0:
      case LPASS_HW_VER_3_3_0:
      {
         hal_i2s_get_func_table_v1(&afe_i2s_hal_table);
         break;
      }
      case LPASS_HW_VER_4_0_0:
      case LPASS_HW_VER_4_0_2:
      case LPASS_HW_VER_4_0_3:
      case LPASS_HW_VER_4_1_0:
      {
         hal_i2s_get_func_table_v2(&afe_i2s_hal_table);
         break;
      }
      /*Warlock*/
      case LPASS_HW_VER_4_2_0:
      {
         hal_i2s_get_func_table_v3(&afe_i2s_hal_table);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Unknown I2S HAL version: 0x%x", version);
         break;
      }
   }

   afe_i2s_hal_table.hal_i2s_init_fptr(version, phy_base, virt_base, i2s_offset, int_i2s_offset);

}


void hal_i2s_config(uint32_t i2s_index, uint32_t dir, i2s_config_t *config_ptr)
{
   afe_i2s_hal_table.hal_i2s_config_fptr(i2s_index, dir, config_ptr);
}

void hal_i2s_enable(uint32_t i2s_index, uint32_t dir)
{
   afe_i2s_hal_table.hal_i2s_enable_fptr(i2s_index, dir);

}

void hal_i2s_disable(uint32_t i2s_index, uint32_t dir)
{
   afe_i2s_hal_table.hal_i2s_disable_fptr(i2s_index, dir);
}

uint32_t hal_i2s_get_intf(uint32_t i2s_index, uint32_t dir)
{
   return afe_i2s_hal_table.hal_i2s_get_intf_fptr(i2s_index, dir);
}

#ifdef AUDIOIF_REG_DUMP
void hal_i2s_dump(uint32_t intf_index, uint32_t dir, uint32_t dump_seq)
{
   return afe_i2s_hal_table.hal_i2s_dump_fptr(intf_index, dir, dump_seq);
}
#endif

