/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/src/AFEPcmHalFuncDefs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/15/14   arthuryu   Created file.
========================================================================== */
/**
@file AFEPcmHalFuncDefs.h

@brief This file contains afe hardware abstraction layer.
*/

#ifndef __AFE_PCM_HAL_FUNC_DEFS_H__
#define __AFE_PCM_HAL_FUNC_DEFS_H__

#include "AFEHalPcm.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** 
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 * 
 */

typedef void (*hal_pcm_init_func_t)(uint32_t version, uint32_t phy_base, uint32_t virt_base, uint32_t pcm_offset);
typedef void (*hal_pcm_reset_func_t)(uint32_t pcm_index, uint32_t dir);
typedef void (*hal_pcm_reset_release_func_t)(uint32_t pcm_index, uint32_t dir);
typedef void (*hal_pcm_config_func_t)(uint32_t pcm_index, uint32_t dir, pcm_config_t *config_ptr);
typedef void (*hal_pcm_enable_func_t)(uint32_t pcm_index, uint32_t dir);
typedef void (*hal_pcm_disable_func_t)(uint32_t pcm_index, uint32_t dir);
typedef uint32_t (*hal_pcm_get_intf_func_t)(uint32_t pcm_index, uint32_t dir);
#ifdef AUDIOIF_REG_DUMP
typedef void (*hal_pcm_dump_func_t)(uint32_t intf_index, uint32_t dir, uint32_t dump_seq);
#endif



typedef struct
{
   hal_pcm_init_func_t                   hal_pcm_init_fptr;
   hal_pcm_reset_func_t                  hal_pcm_reset_fptr;
   hal_pcm_reset_release_func_t          hal_pcm_reset_release_fptr;
   hal_pcm_config_func_t                 hal_pcm_config_fptr;
   hal_pcm_enable_func_t                 hal_pcm_enable_fptr;
   hal_pcm_disable_func_t                hal_pcm_disable_fptr;
   hal_pcm_get_intf_func_t               hal_pcm_get_intf_fptr;
#ifdef AUDIOIF_REG_DUMP
   hal_pcm_dump_func_t                   hal_pcm_dump_fptr;
#endif
} afe_pcm_hal_func_def_t; 


void hal_pcm_get_func_table_v1(afe_pcm_hal_func_def_t *func_table);
void hal_pcm_get_func_table_v2(afe_pcm_hal_func_def_t *func_table);
void hal_pcm_get_func_table_v3(afe_pcm_hal_func_def_t *func_table);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__AFE_PCM_HAL_FUNC_DEFS_H__
