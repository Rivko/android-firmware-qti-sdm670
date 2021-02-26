/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/inc/AFEDmaManager_i.h#1 $
 ====================================================================== */
#ifndef _DMA_MANAGER_I_H_
#define _DMA_MANAGER_I_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEDmaManager.h"
#include "lpasshwio_devcfg.h"

#if defined(__cplusplus)
extern "C" {
#endif


/*==========================================================================
  Macro Defines
  ========================================================================== */
static const uint32_t DMA_BUFFER_MGR_32_BYTE_ALIGNMENT     =  0x1F;
static const uint32_t DMA_BUFFER_MGR_16_BYTE_ALIGNMENT     =  0xF;
static const uint32_t DMA_BUFFER_MGR_8_BYTE_ALIGNMENT      =  0x7;
static const uint32_t DMA_BUFFER_MGR_4_BYTE_ALIGNMENT      =  0x3;
#define DMA_BUFFER_MGR_CACHE_LINE_SIZE  32
static const uint32_t DMA_BUFFER_MGR_CACHE_LINE_ALIGNMENT  =  (DMA_BUFFER_MGR_CACHE_LINE_SIZE - 1);

/**
  @brief dma alloaction state
*/
enum
{
   AFE_DMA_FREE   =  0,    
   AFE_DMA_USED   =  1,    
};

/** 
   @brief dma resource state per dma
 */
typedef struct dma_mgr_resource_state
{
   uint32_t                  dma_alloc_state;    ///< dma allocation state 
   dma_manager_state_t       *h_dma_ptr;           ///< dma handler pointer
} dma_mgr_resource_state_t;

/** 
   @brief dma mansger driver state

   This is used for managing for dma resource allocation and free.
   And also it will be used for dma interrupt handling for callback functions from client.
 */

typedef struct afe_dma_mgr_driver_state
{
   lpasshwio_prop_afe_dma_struct_t  dma_hw_info[LPASS_HW_DMA_MAX_TYPES];     // dma hw config data.
   dma_mgr_resource_state_t    *sink_dma_state_ptr[LPASS_HW_DMA_MAX_TYPES];       // dma sync dma resource state
   dma_mgr_resource_state_t    *source_dma_state_ptr[LPASS_HW_DMA_MAX_TYPES];     // dma source dma resource state
   qurt_elite_mutex_t          dma_lock[LPASS_HW_DMA_MAX_TYPES];
   qurt_elite_interrupt_ist_t  dma_ist_int_info[LPASS_HW_DMA_MAX_TYPES];
   uint32_t                    dma_virt_addr[LPASS_HW_DMA_MAX_TYPES];            /* dma main register virtual address mapping */
   qurt_mem_region_t           dma_qmem_region[LPASS_HW_DMA_MAX_TYPES];         /* dma register mapping region */
   
}afe_dma_mgr_driver_state_t;


/*==========================================================================
  Function Declerations
  ========================================================================== */

ADSPResult afe_dma_mgr_resource_state_init(void);

ADSPResult afe_dma_mgr_resource_state_deinit(void);

ADSPResult afe_dma_mgr_alloc_dma(dma_manager_state_t *dma_handle);

ADSPResult afe_dma_mgr_free_dma(dma_manager_state_t *dma_handle);


ADSPResult afe_dma_manager_get_dma_buffer_size(dma_manager_state_t *dma_mgr_state_ptr);

ADSPResult afe_dma_manager_alloc_dma_buffer(dma_manager_state_t *dma_mgr_state_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif /*_DMA_MANAGER_I_H_*/

