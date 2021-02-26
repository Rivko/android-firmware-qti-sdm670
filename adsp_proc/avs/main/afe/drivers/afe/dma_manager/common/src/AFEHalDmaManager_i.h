/** @file hal_dma_manager_i.h
   This module contains Audio Hardware interface Abstraction layer function
   declarations. This file publicizes enums defined by the HAL.

   Copyright (c) 2013-2017 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
   QUALCOMM Proprietary.  Export of this technology or software is regulated
   by the U.S. Government, Diversion contrary to U.S. law prohibited.
*/ /*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEHalDmaManager_i.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------
07/22/13   RP     Created


========================================================================== */
#ifndef _HAL_DMA_MANAGER_I_H_
#define _HAL_DMA_MANAGER_I_H_

#include "AFEHalDmaManagerCommon.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*==========================================================================
Structure definitions
========================================================================== */


//typedef void (*afe_hal_dma_init_func_t)(void);

typedef void (*afe_hal_dma_reg_addr_config_func_t)(uint32_t base_addr, int32_t virt_offset);

typedef void (*afe_hal_dma_enable_dma_channel_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef void (*afe_hal_dma_disable_dma_channel_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef void (*afe_hal_dma_config_dma_channel_func_t)(uint32_t dma_dir, uint32_t dma_index, hal_dma_config_t *dma_config_ptr);

typedef void (*afe_hal_dma_get_dma_curr_addr_func_t)(uint32_t dma_dir, uint32_t dma_index, uint32_t *curr_addr);

typedef void (*afe_hal_dma_disable_dma_interrupt_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef void (*afe_hal_dma_enable_dma_interrupt_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef uint32_t(*afe_hal_dma_check_dma_interrupt_func_t)(uint32_t dma_dir, uint32_t dma_index, uint32_t int_status, uint32_t int_status2);

typedef uint32_t(*afe_hal_dma_check_hdmi_interrupt_func_t)(uint32_t int_status);

typedef void (*afe_hal_dma_clear_dma_interrupt_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef void (*afe_hal_dma_read_interrupt_status_func_t)(uint32 *int_status_ptr, uint32 *int_status2_ptr);

typedef void (*afe_hal_dma_clear_interrupt_func_t)(uint32_t int_status, uint32_t int_status2);

typedef void (*afe_hal_dma_get_dma_interrupt_stc_func_t)(uint32_t dma_dir, uint32_t dma_index, uint64 *p_stc);

typedef void (*afe_hal_dma_get_dma_fifo_count_func_t)(uint32_t dma_dir, uint32_t dma_index, uint32_t *fifo_cnt_ptr);

typedef void (*afe_hal_dma_reset_func_t)(uint32_t dma_dir, uint32_t dma_index);

typedef void (*afe_hal_dma_dump_func_t)(uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq);

typedef struct afe_hal_dma_func_def
{
   afe_hal_dma_enable_dma_channel_func_t           afe_hal_dma_enable_dma_channel_fptr;
   afe_hal_dma_disable_dma_channel_func_t          afe_hal_dma_disable_dma_channel_fptr;
   afe_hal_dma_config_dma_channel_func_t           afe_hal_dma_config_dma_channel_fptr;
   afe_hal_dma_get_dma_curr_addr_func_t            afe_hal_dma_get_dma_curr_addr_fptr;
   afe_hal_dma_disable_dma_interrupt_func_t        afe_hal_dma_disable_dma_interrupt_fptr;
   afe_hal_dma_enable_dma_interrupt_func_t         afe_hal_dma_enable_dma_interrupt_fptr;
   afe_hal_dma_clear_dma_interrupt_func_t          afe_hal_dma_clear_dma_interrupt_fptr;
   afe_hal_dma_get_dma_interrupt_stc_func_t        afe_hal_dma_get_dma_interrupt_stc_fptr;
   afe_hal_dma_get_dma_fifo_count_func_t           afe_hal_dma_get_dma_fifo_count_fptr;

   // interrupt handling
   afe_hal_dma_check_dma_interrupt_func_t          afe_hal_dma_check_dma_interrupt_fptr;
   afe_hal_dma_check_hdmi_interrupt_func_t         afe_hal_dma_check_hdmi_interrupt_fptr;
   afe_hal_dma_read_interrupt_status_func_t        afe_hal_dma_read_interrupt_status_fptr;
   afe_hal_dma_clear_interrupt_func_t              afe_hal_dma_clear_interrupt_fptr;

   // ssr handling
   afe_hal_dma_reset_func_t                         afe_hal_dma_reset_fptr;

#ifdef AUDIOIF_REG_DUMP
   afe_hal_dma_dump_func_t                          afe_hal_dma_dump_fptr;
#endif
} afe_hal_dma_func_def_t;



void hal_dma_init_type1(afe_hal_dma_func_def_t *func_table, uint32_t base_addr, int32_t virt_offset);
void hal_dma_init_type1_v2(afe_hal_dma_func_def_t *func_table, uint32_t base_addr, int32_t virt_offset);

void hal_dma_init_type0_v1(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v2(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v3(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v4(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v5(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v6(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);

void hal_dma_init_type0_v7(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base,
                           lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus



#endif /*__HAL_DMA_MANAGER_I__*/
