/*========================================================================
Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
=====================================---================================= */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEHalDumpFuncDefs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/15/14   arthuryu   Created file.
========================================================================== */
/**
@file AFEPcmHalFuncDefs.h

@brief This file contains afe hardware abstraction layer.
*/
#ifndef __AFE_HAL_DUMP_FUNC_DEFS_H__
#define __AFE_HAL_DUMP_FUNC_DEFS_H__

#include "AFEHalDump.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** 
 * Initialize register to default values and return the version number of the
 * audio HAL. It also sets the base virtual address of the audio hardware
 * registers.
 * 
 */
typedef void (*hal_afe_dump_init_func_t)(uint32_t version);
typedef void (*hal_afe_dump_deinit_func_t)(void);
typedef void (*hal_afe_clock_dump_func_t)(uint32_t intf_type, uint32_t intf_index, uint32_t dump_seq);
typedef void (*hal_afe_audioif_dump_func_t)(uint32_t intf_type, uint32_t intf_index, uint32_t dir, uint32 channel, uint32_t dump_seq);


typedef struct
{
   hal_afe_dump_init_func_t                hal_afe_dump_init_fptr;
   hal_afe_dump_deinit_func_t              hal_afe_dump_deinit_fptr;
   hal_afe_clock_dump_func_t               hal_afe_clock_dump_fptr;
} afe_hal_dump_func_def_t; 


void hal_afe_dump_get_func_table_v1(afe_hal_dump_func_def_t *func_table);
void hal_afe_dump_get_func_table_v2(afe_hal_dump_func_def_t *func_table);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__AFE_HAL_DUMP_FUNC_DEFS_H__
