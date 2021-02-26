/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEDmaManagerUtils.cpp#3 $
$DateTime: 2017/12/07 12:24:07 $
$Author: pwbldsvc $
$Change: 15009245 $
$Revision: #3 $

FILE:     hal_dma_manager.cpp

DESCRIPTION: Common DMA Manager

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEDmaManagerUtils.cpp#3 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/25/13    RP      created

============================================================================*/

#include "qurt.h"
#include "qube.h"
#include "qurt_elite.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "AFEDmaManager.h"
#include "AFEDmaManager_i.h"
#include "AFEHalDmaManager_i.h"
#include "AFEDeviceDriver.h"
#include "hwd_devcfg.h"
#include "afe_lpm.h"

/*=====================================================================
 Global Declarations
 ======================================================================*/
afe_dma_mgr_driver_state_t afe_dma_mgr_driver_state_info;

#ifdef __cplusplus
extern "C"
{
#endif
  void afe_dma_type0_interrupt_handler(void *arg);
  void afe_dma_type1_interrupt_handler(void *arg);
#ifdef __cplusplus
}
#endif

#define DMA_INTR_IST_STACK_SIZE_BYTES 1024

#define BITWISE_CHECK_FOR_2_BYTE_ALIGNMENT 1
#define BITWISE_CHECK_FOR_4_BYTE_ALIGNMENT 3
#define BITWISE_CHECK_FOR_8_BYTE_ALIGNMENT 7

#define divide_by_2(x) (x >> 1)
#define divide_by_4(x) (x >> 2)
#define divide_by_8(x) (x >> 3)

/*=====================================================================
 Static Function Headers
 ======================================================================*/
static ADSPResult afe_dma_mgr_device_cfg_init(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr);
static ADSPResult afe_dma_mgr_dma_type_init(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr);
static ADSPResult afe_dma_mgr_dma_type_deinit(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr);
static void afe_dma_interrupt_handler(uint32_t dma_type);
static void afe_handle_dma_isr(uint32_t dma_type, uint32_t int_status, uint32_t int_status2);
static void handle_sink_dma_per_int(uint32_t dma_type, uint32_t dma_index, uint32_t event_mask);
static void handle_source_dma_per_int(uint32_t dma_type, uint32_t dma_index, uint32_t event_mask);


static uint32_t afe_dma_manager_align_size(uint32_t size, uint32_t alignment)
{
  uint32_t temp = alignment - 1;
  return ((size + temp) & ~temp);
}


/*=====================================================================
 Function Definitions
 ======================================================================*/

/**
  initialize dma resource state

  @param[in]
  None

  @return
  aDSP error code

  @dependencies
  None.
 */

ADSPResult afe_dma_mgr_resource_state_init(void)
{
  ADSPResult   result = ADSP_EOK;

  memset(&afe_dma_mgr_driver_state_info, 0, sizeof(afe_dma_mgr_driver_state_t));

  // get the dma dev config data. At this point, the dma type2 only has the device config data.
  if(ADSP_EOK != (result = afe_dma_mgr_device_cfg_init(&afe_dma_mgr_driver_state_info)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_device_cfg_init is failed");
    return result;
  }

  /* allocate global dma state structure for all dma channels and init */
  if(ADSP_EOK != (result = afe_dma_mgr_dma_type_init(&afe_dma_mgr_driver_state_info)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_dma_type2_init is failed");
  }

  if(ADSP_EOK != (result = afe_hal_dma_init(&afe_dma_mgr_driver_state_info)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_hal_dma_init is failed");
  }

  return result;

}

/**
  deinitialize dma resource state 

  @param[in]
  None

  @return
  aDSP error code

  @dependencies
  None.
 */

ADSPResult afe_dma_mgr_resource_state_deinit(void)
{
  ADSPResult   result = ADSP_EOK;

  /* deallocate global dma state structure for all dma channels*/

  if(ADSP_EOK != (result = afe_dma_mgr_dma_type_deinit(&afe_dma_mgr_driver_state_info)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_dma_type_deinit is failed");
  }

  return result;
}

/**
  reset dmas 

  @param[in] dma_type   dma type
  None

  @return
  aDSP error code

  @dependencies
  None.
 */

ADSPResult afe_dma_mgr_reset_dma(uint32_t dma_type)
{
  ADSPResult   result = ADSP_EOK;
  uint32_t dma_index = AFE_INVALID_DMA_INDEX;
  lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[dma_type];

  /* Capture register status and stop all the available read DMA's */
  for(dma_index = 0; dma_index < dma_hw_info_ptr->sink_dma_cnt; dma_index++)
  {
    /* Reset the DMA ctl to POR value */
    afe_hal_dma_reset(dma_type, LPASS_HW_DMA_SINK, dma_index);

  }

  /* Capture register status and stop all the available write DMA's */
  for(dma_index = 0; dma_index < dma_hw_info_ptr->source_dma_cnt; dma_index++)
  {
    /* Reset the DMA ctl to POR value */
    afe_hal_dma_reset(dma_type, LPASS_HW_DMA_SOURCE, dma_index);

  }

  return result;
}



/**
  allocate free DMA to dma handle.
  This is only for dma type 2.
  if the dma resource is not managed by driver, dma manager is expecting the dma_index is passed as invalid index.

  @param[in]  dma_handle           dma handle

  @return
  aDSP error code

  @dependencies
  None.
 */

ADSPResult afe_dma_mgr_alloc_dma(dma_manager_state_t *dma_handle)
{
  ADSPResult   result = ADSP_EOK;
  uint32_t max_dma_cnt;
  uint32_t dma_index = AFE_INVALID_DMA_INDEX;
  uint32_t i;
  lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[dma_handle->dma_type];
  dma_mgr_resource_state_t    *sink_dma_state_ptr = afe_dma_mgr_driver_state_info.sink_dma_state_ptr[dma_handle->dma_type];
  dma_mgr_resource_state_t    *source_dma_state_ptr = afe_dma_mgr_driver_state_info.source_dma_state_ptr[dma_handle->dma_type];


  qurt_elite_mutex_lock(&afe_dma_mgr_driver_state_info.dma_lock[dma_handle->dma_type]);

  if(LPASS_HW_DMA_SINK == dma_handle->dma_dir)
  {
    if(NULL != sink_dma_state_ptr)
    {
      max_dma_cnt =  dma_hw_info_ptr->sink_dma_cnt;

      ///< Loop over DMA channel list.
      for(i = 0; i < max_dma_cnt; i++)
      {
        if(AFE_DMA_FREE == sink_dma_state_ptr[i].dma_alloc_state)
        {
          sink_dma_state_ptr[i].h_dma_ptr = dma_handle;
          ///< Found a DMA channel.
          sink_dma_state_ptr[i].dma_alloc_state = AFE_DMA_USED;
          dma_index = i;
          break;
        }
      }

      if(AFE_INVALID_DMA_INDEX == dma_index)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No free dma for sink dma type[%d]", dma_handle->dma_type);
        result = ADSP_ENORESOURCE;
      }
      else
      {
        dma_handle->dma_idx = dma_index;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No dma for sink dma type[%d]", dma_handle->dma_type);
    }
  }
  else
  {
    if(NULL != source_dma_state_ptr)
    {
      max_dma_cnt =  dma_hw_info_ptr->source_dma_cnt;

      ///< Loop over DMA channel list.
      for(i = 0; i < max_dma_cnt; i++)
      {
        if(AFE_DMA_FREE == source_dma_state_ptr[i].dma_alloc_state)
        {
          source_dma_state_ptr[i].h_dma_ptr = dma_handle;
          ///< Found a DMA channel.
          source_dma_state_ptr[i].dma_alloc_state = AFE_DMA_USED;
          dma_index = i;
          break;
        }
      }

      if(AFE_INVALID_DMA_INDEX == dma_index)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No free dma for source dma type[%d]", dma_handle->dma_type);
        result = ADSP_ENORESOURCE;
      }
      else
      {
        dma_handle->dma_idx = dma_index;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No dma for source dma type[%d]", dma_handle->dma_type);
    }
  }

  qurt_elite_mutex_unlock(&afe_dma_mgr_driver_state_info.dma_lock[dma_handle->dma_type]);

  return result;
}

/**
  free DMA 
  This is only for dma type 2.

  @param[in]  dma_handle           dma handle

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_free_dma(dma_manager_state_t *dma_handle)
{
  ADSPResult   result = ADSP_EOK;

  lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[dma_handle->dma_type];
  dma_mgr_resource_state_t    *sink_dma_state_ptr = afe_dma_mgr_driver_state_info.sink_dma_state_ptr[dma_handle->dma_type];
  dma_mgr_resource_state_t    *source_dma_state_ptr = afe_dma_mgr_driver_state_info.source_dma_state_ptr[dma_handle->dma_type];

  qurt_elite_mutex_lock(&afe_dma_mgr_driver_state_info.dma_lock[dma_handle->dma_type]);

  if(LPASS_HW_DMA_SINK == dma_handle->dma_dir)
  {
    if(NULL != sink_dma_state_ptr)
    {
      if(dma_handle->dma_idx < dma_hw_info_ptr->sink_dma_cnt)
      {
        sink_dma_state_ptr[dma_handle->dma_idx].h_dma_ptr = NULL;
        sink_dma_state_ptr[dma_handle->dma_idx].dma_alloc_state = AFE_DMA_FREE;
      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "invalid sync dma type[%d] index[%lu] to be freed", dma_handle->dma_type, dma_handle->dma_idx);
        result = ADSP_EFAILED;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No dma for sink dma type[%d]", dma_handle->dma_type);
    }
  }
  else
  {
    if(NULL != source_dma_state_ptr)
    {
      if(dma_handle->dma_idx < dma_hw_info_ptr->source_dma_cnt)
      {
        source_dma_state_ptr[dma_handle->dma_idx].h_dma_ptr = NULL;
        source_dma_state_ptr[dma_handle->dma_idx].dma_alloc_state = AFE_DMA_FREE;
      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "invalid source dma type[%d] index[%lu] to be freed", dma_handle->dma_type, dma_handle->dma_idx);
        result = ADSP_EFAILED;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No dma for source dma type[%d]", dma_handle->dma_type);
    }
  }

  qurt_elite_mutex_unlock(&afe_dma_mgr_driver_state_info.dma_lock[dma_handle->dma_type]);

  return result;
}

/**
  Calculated the DMA buffer size based on the DMA stat information

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[out] buffer_size               size of the DMA buffer

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_manager_get_dma_buffer_size(dma_manager_state_t *dma_mgr_state_ptr)
{
  if(0 == dma_mgr_state_ptr->num_buffers)
  {
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Get buffer size called with 0 buffers");
    return ADSP_EFAILED;
  }
  
  uint32_t buffer_size;

  //Currently only even number of channels is allowed
  QURT_ELITE_ASSERT((dma_mgr_state_ptr->num_channels & 0x1) == 0);

  buffer_size =  dma_mgr_state_ptr->int_samples_per_period * dma_mgr_state_ptr->num_channels
      * dma_mgr_state_ptr->input_bytes_per_sample;

  //protective check to ensure that a zero sized buffer won't be allocated    
  if(0 == buffer_size)
  {
  	//set buffer size as 0 if it is calculated as 0
   MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Calculated DMA buffer size = 0");
   dma_mgr_state_ptr->dma_buffer_size = 0;

  	return ADSP_EFAILED;
  }
  
  //set the period count in double words
  dma_mgr_state_ptr->period_count_in_word32 = buffer_size / sizeof(uint32_t);

  //total buffer size for all DMA buffers
  dma_mgr_state_ptr->dma_buffer_size = buffer_size * dma_mgr_state_ptr->num_buffers;

  
  return ADSP_EOK;
}

/**
  Allocate the DMA buffer

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_manager_alloc_dma_buffer(dma_manager_state_t *dma_mgr_state_ptr)
{

  uint32_t buffer_size = dma_mgr_state_ptr->dma_buffer_size;
  bool_t is_buffer_allocated = FALSE;

  //align total buffer size to cache line size (32 byte boundary)
  if(buffer_size & DMA_BUFFER_MGR_CACHE_LINE_ALIGNMENT)
  {
    buffer_size = afe_dma_manager_align_size(buffer_size, DMA_BUFFER_MGR_CACHE_LINE_SIZE);
  }

   /* If LPM memory is exhausted, only for HDMI interface, try
   * to allocate in DDR, for other interfaces, simply error out.*/
  if(DMA_MGR_LPM_BUFFER == dma_mgr_state_ptr->buff_mem_type ||
      (DMA_MGR_LPM_DDR_BUFFER) == dma_mgr_state_ptr->buff_mem_type)
  {
    //perform cache line aligned malloc
    dma_mgr_state_ptr->dma_buffer_virt_addr_ptr = (uint32_t *)afe_dev_lpa_aligned_malloc(buffer_size,
                                                                                         DMA_BUFFER_MGR_CACHE_LINE_SIZE);
    if(NULL == dma_mgr_state_ptr->dma_buffer_virt_addr_ptr)
    {
       if(DMA_MGR_LPM_DDR_BUFFER == dma_mgr_state_ptr->buff_mem_type)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Failed to allocate DMA buffer of size %lu in LPM buffer. Will try to allocate in DDR", buffer_size);
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate DMA buffer of size %lu in LPM", buffer_size);
        return ADSP_ENORESOURCE;
      }
    }
    else
    {
      is_buffer_allocated = TRUE;
      
      /* DMA buffer allocated in LPM, so updating buff_mem_type
       * as DMA_MGR_LPM_BUFFER. This information will be used
       * while freeing the buffer */
      dma_mgr_state_ptr->buff_mem_type = DMA_MGR_LPM_BUFFER;
    }
  }

  if(!is_buffer_allocated)
  {
    //perform cache line aligned malloc
    dma_mgr_state_ptr->dma_buffer_virt_addr_ptr = (uint32_t *)qurt_elite_memory_aligned_malloc(buffer_size,
                                                                                               DMA_BUFFER_MGR_CACHE_LINE_SIZE, QURT_ELITE_HEAP_DEFAULT);

    if(NULL == dma_mgr_state_ptr->dma_buffer_virt_addr_ptr)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate DMA buffer of size %lu", buffer_size);
      return ADSP_ENORESOURCE;
    }
    else
    {
      dma_mgr_state_ptr->buff_mem_type = DMA_MGR_DDR_BUFFER;
    }
  }

  //store the updated buffer size
  dma_mgr_state_ptr->total_buffer_size = buffer_size;

  //query the physical address for the given virtual address
  dma_mgr_state_ptr->dma_buffer_phy_addr_ptr = (uint32_t *)qurt_lookup_physaddr((uint32_t)dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
  if(NULL == dma_mgr_state_ptr->dma_buffer_phy_addr_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to look up physical address for given virtual address %x", (int)dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
    return ADSP_EFAILED;
  }

  return ADSP_EOK;
}




/**
  initialize dma type2  resource state including interrupt registeration.

  @param[in] : afe_dma_mgr_ptr points to the global dma manager state

  @return
  aDSP error code

  @dependencies
  None.
 */
static ADSPResult afe_dma_mgr_dma_type_init(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr)
{
  uint32_t size = 0;
  ADSPResult result = ADSP_EOK;
  lpasshwio_prop_afe_dma_struct_t  *dma_hw_info_ptr = NULL;
  char_t dma_mgr_int_thread_name[16];

  for(uint32_t i = 0; i < LPASS_HW_DMA_MAX_TYPES; i++)
  {
    dma_hw_info_ptr = &afe_dma_mgr_ptr->dma_hw_info[i];

    if(0 != dma_hw_info_ptr->hw_revision)
    {
      // create memory region for lpass dma hw
      // This information is need to be saved to  dma global state
      if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&(afe_dma_mgr_ptr->dma_qmem_region[i]),
                                                       dma_hw_info_ptr->dma_reg_base, dma_hw_info_ptr->dma_reg_size, &(afe_dma_mgr_ptr->dma_virt_addr[i]))))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for dma register base");
        return result;
      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "dma virtual addr 0x%x phy addr 0x%x",
              (unsigned int)afe_dma_mgr_ptr->dma_virt_addr[i], (unsigned int)dma_hw_info_ptr->dma_reg_base);
      }


      if(0 < dma_hw_info_ptr->sink_dma_cnt)
      {
        // allocate init dma sink dma states
        size = sizeof(dma_mgr_resource_state_t) * (dma_hw_info_ptr->sink_dma_cnt);

        afe_dma_mgr_ptr->sink_dma_state_ptr[i] = (dma_mgr_resource_state_t *)qurt_elite_memory_malloc(size, QURT_ELITE_HEAP_DEFAULT);

        if(NULL == afe_dma_mgr_ptr->sink_dma_state_ptr[i])
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in allocating memory for dma_mgr_resource_state_t structure");
          return ADSP_ENOMEMORY;
        }
        else
        {  // this dma state to  the default init state
          memset(afe_dma_mgr_ptr->sink_dma_state_ptr[i], 0, size);
        }
      }

      if(0 < dma_hw_info_ptr->source_dma_cnt)
      {
        // allocate init dma source dma states
        size = sizeof(dma_mgr_resource_state_t) * (dma_hw_info_ptr->source_dma_cnt);

        afe_dma_mgr_ptr->source_dma_state_ptr[i] = (dma_mgr_resource_state_t *)qurt_elite_memory_malloc(size, QURT_ELITE_HEAP_DEFAULT);

        if(NULL == afe_dma_mgr_ptr->source_dma_state_ptr[i])
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in allocating memory for dma_mgr_resource_state_t structure");
          return ADSP_ENOMEMORY;
        }
        else
        {  // this dma state to  the default init state
          memset(afe_dma_mgr_ptr->source_dma_state_ptr[i], 0, size);
        }
      }

      switch(i)
      {
        case LPASS_HW_DMA_TYPE0:
          /* register interrupt handler */
          qurt_elite_strl_cpy(dma_mgr_int_thread_name, "dma_type0_isr", sizeof(dma_mgr_int_thread_name));
          if(ADSP_FAILED(result = qurt_elite_interrupt_register(&afe_dma_mgr_ptr->dma_ist_int_info[i], (uint16_t)dma_hw_info_ptr->int_irq_no,
                                                                afe_dma_type0_interrupt_handler, (void *)NULL, dma_mgr_int_thread_name, DMA_INTR_IST_STACK_SIZE_BYTES)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to register dma type 0 interrupt with status %d", result);
          }
          break;
        case LPASS_HW_DMA_TYPE1:

          /* register interrupt handler */
          qurt_elite_strl_cpy(dma_mgr_int_thread_name, "dma_type1_isr", sizeof(dma_mgr_int_thread_name));
          if(ADSP_FAILED(result = qurt_elite_interrupt_register(&afe_dma_mgr_ptr->dma_ist_int_info[i], (uint16_t)dma_hw_info_ptr->int_irq_no,
                                                                afe_dma_type1_interrupt_handler, (void *)NULL, dma_mgr_int_thread_name, DMA_INTR_IST_STACK_SIZE_BYTES)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to register dma type 1 interrupt with status %d", result);
          }
          break;
        default:
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error qurt interrupt register unknown dma type %lu", i);
          break;
      }

      qurt_elite_mutex_init(&afe_dma_mgr_driver_state_info.dma_lock[i]);
    }
  }

  return ADSP_EOK;
}

/**
  deinitialize dma type2  resource state including interrupt registeration.

  @param[in] : afe_dma_mgr_ptr points to the global dma manager state

  @return
  aDSP error code

  @dependencies
  None.
 */
static ADSPResult afe_dma_mgr_dma_type_deinit(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr)
{
  ADSPResult   result = ADSP_EOK;
  lpasshwio_prop_afe_dma_struct_t  *dma_hw_info_ptr = NULL;

  for(uint32_t i = 0; i < LPASS_HW_DMA_MAX_TYPES; i++)
  {
    dma_hw_info_ptr = &afe_dma_mgr_ptr->dma_hw_info[i];

    if(0 != dma_hw_info_ptr->hw_revision)
    {
      /* deregister interrupt with callback function. */
      if(ADSP_FAILED(qurt_elite_interrupt_unregister(&afe_dma_mgr_ptr->dma_ist_int_info[i])))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DMA MGR: failed to unregister resampler interrupt %d", i);
      }

      if(NULL != afe_dma_mgr_ptr->sink_dma_state_ptr[i])
      {
        qurt_elite_memory_free(afe_dma_mgr_ptr->sink_dma_state_ptr[i]);
        afe_dma_mgr_ptr->sink_dma_state_ptr[i] = NULL;
      }

      if(NULL != afe_dma_mgr_ptr->source_dma_state_ptr[i])
      {
        qurt_elite_memory_free(afe_dma_mgr_ptr->source_dma_state_ptr[i]);
        afe_dma_mgr_ptr->source_dma_state_ptr[i] = NULL;
      }

      qurt_elite_mutex_destroy(&afe_dma_mgr_driver_state_info.dma_lock[i]);
    }
  }

  return result;
}


/**
  dma interrupt handler
  This is will be called when the interrupt is triggered..

  This interrupt handler will be common for 3 dma types.

  @dependencies
  None.
 */
static void afe_dma_interrupt_handler(uint32_t dma_type)
{
  /* int_status  contains status of LPASS_LPAIF_IRQ_STATa
      int_status2 contains status of LPASS_LPAIF_IRQ2_STATa*/
  uint32_t int_status = 0, int_status2 = 0;

  if(dma_type < LPASS_HW_DMA_MAX_TYPES)
  {
    // read interrupt status
    afe_hal_dma_read_interrupt_status(dma_type, &int_status, &int_status2 );

    while((0 != int_status)|| (0 != int_status2))
    {
      // handle the triggerred interrupts
      afe_handle_dma_isr(dma_type, int_status,int_status2 );
      // clear the triggerred interrupts
      afe_hal_dma_clear_interrupt(dma_type, int_status, int_status2);
      // check if there are new interrupts triggerred during previous interrupt handling.
      afe_hal_dma_read_interrupt_status(dma_type, &int_status, &int_status2);
    }
  }
}

/**
  dma type0 interrupt handler
  This is will be called for the first time when the interrupt is triggered..

  Each type of dma need to have separate interrupt handler.
  This interrupt handler may be used for other driver types

  For example, hdmi tx does the same.
                      hdmi rx can be modified like below.

  @dependencies
  None.
 */
void afe_dma_type0_interrupt_handler(void *arg)
{
  afe_dma_interrupt_handler(LPASS_HW_DMA_TYPE0);
}

/**
  dma type1 interrupt handler
  This is will be called for the first time when the interrupt is triggered..

  Each type of dma need to have separate interrupt handler.
  This interrupt handler may be used for other driver types

  For example, hdmi tx does the same.
                      hdmi rx can be modified like below.

  @dependencies
  None.
 */
void afe_dma_type1_interrupt_handler(void *arg)
{
  afe_dma_interrupt_handler(LPASS_HW_DMA_TYPE1);

}


/**
  handle dma interrupts
  This identifies which dma interrupt is triggerred and call the right handler for the dma interrupt.

  @param[in] int_status : each bit represents which dma itnerrupt and which type of interrupt is triggerred. 

  @return 
  None

  @dependencies
  None.
 */
static void afe_handle_dma_isr(uint32_t dma_type, uint32_t int_status, uint32_t int_status2 )
{
  lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[dma_type];
  uint32_t  dma_index;
  uint32_t event_mask = 0;

  uint32_t  max_dma_cnt;
  uint16_t  dp_event_flag = 1;   //check DP specific interrupt only once

  // check if there is any interrupt triggerred for sync dma type
  // if it is, call the sync dma type interrupt handler
  if(0 != dma_hw_info_ptr->sink_dma_cnt)
  {
    max_dma_cnt =  dma_hw_info_ptr->sink_dma_cnt;
    for(dma_index = 0; dma_index < max_dma_cnt; dma_index++)
    {
      event_mask =  afe_hal_dma_check_dma_interrupt(dma_type, LPASS_HW_DMA_SINK, dma_index, \
                                                    int_status,int_status2 );
      /*  dma_check_hdmi_interrupt function is set only for type 2 dmas. If HDMI runs in Display port mode
          this function will help to check the metadata status. For regular HDMI mode, it just capture metadata status
          but action will not taken as metadata will be NULL. In case of any spurious metadata for HDMI, subsequent layers
          are ignoring the same. */
      if(dp_event_flag)
      {
        event_mask = event_mask | afe_hal_dma_check_hdmi_interrupt(dma_type, int_status);
        dp_event_flag = 0;
      }

      if(event_mask)
      {
        handle_sink_dma_per_int(dma_type, dma_index, event_mask);
      }
    }
  }
  // check if there is any interrupt triggerred for source dma
  // if it is, call the source dma interrupt handler
  if(0 != dma_hw_info_ptr->source_dma_cnt)
  {
    max_dma_cnt =  dma_hw_info_ptr->source_dma_cnt;
    for(dma_index = 0; dma_index < max_dma_cnt; dma_index++)
    {
      event_mask =  afe_hal_dma_check_dma_interrupt(dma_type, LPASS_HW_DMA_SOURCE, dma_index, int_status,int_status2);
      if(event_mask)
      {
        handle_source_dma_per_int(dma_type, dma_index, event_mask);
      }
    }
  }
}

/**
  call the registered callback function for sync dma  interrupt

  @param[in] dma_index is index for the allocated dma for this interrupt.
  @param[in] event_mask is the what kind of interrupts happened to the dma.

  @return 
  None

  @dependencies
  None.
 */
static void handle_sink_dma_per_int(uint32_t dma_type, uint32_t dma_index, uint32_t event_mask)
{
  dma_manager_state_t *dma_mgr_state_ptr  = afe_dma_mgr_driver_state_info.sink_dma_state_ptr[dma_type][dma_index].h_dma_ptr;

  if(dma_mgr_state_ptr->dma_int_cb)
  {
    dma_mgr_state_ptr->dma_int_cb(dma_mgr_state_ptr->afe_driver_context, event_mask);
  }
}

/**
  call the registered callback function for sync dma  interrupt

  @param[in] dma_index is index for the allocated dma for this interrupt.
  @param[in] event_mask is the what kind of interrupts happened to the dma.

  @return 
  None

  @dependencies
  None.
 */
static void handle_source_dma_per_int(uint32_t dma_type, uint32_t dma_index, uint32_t event_mask)
{
  dma_manager_state_t *dma_mgr_state_ptr  = afe_dma_mgr_driver_state_info.source_dma_state_ptr[dma_type][dma_index].h_dma_ptr;

  if(dma_mgr_state_ptr->dma_int_cb)
  {
    dma_mgr_state_ptr->dma_int_cb(dma_mgr_state_ptr->afe_driver_context, event_mask);
  }
}


/**
  initialize dma device config.

  read device config data.
  dma type 2 only has this config data now.

  @param[in] : afe_dma_mgr_ptr points to the global dma manager state

  @return
  aDSP error code

  @dependencies
  None.
 */
static ADSPResult afe_dma_mgr_device_cfg_init(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr)
{
  ADSPResult                       result_type0 = ADSP_EOK, result_type1 = ADSP_EOK, result;
  DALSYSPropertyVar                prop_var;
  lpasshwio_prop_afe_dma_struct_t  *dma_prop_dev_cfg_ptr;
  lpasshwio_prop_afe_dma_struct_t  *dma_hw_info_ptr;

  dma_prop_dev_cfg_ptr  = NULL;
  dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[LPASS_HW_DMA_TYPE0];
  const char *dev_id_dma_type0 = "AUDIOIFDMA";

  memset(dma_hw_info_ptr, 0, sizeof(lpasshwio_prop_afe_dma_struct_t));

  DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl_dma_type0);

  /* Get device handle */
  if(DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr(dev_id_dma_type0, dal_hdl_dma_type0))
  {
    /* Read property ptr */
    if(DAL_SUCCESS == DALSYS_GetPropertyValue(dal_hdl_dma_type0, "AudioIfDmaPropStructPtr", 0, &prop_var))
    {
      dma_prop_dev_cfg_ptr = (lpasshwio_prop_afe_dma_struct_t *)prop_var.Val.pStruct;

      dma_hw_info_ptr->hw_revision = dma_prop_dev_cfg_ptr->hw_revision;

      /* hw revision */
      if(0 != dma_hw_info_ptr->hw_revision)
      {
        dma_hw_info_ptr->dma_reg_base = dma_prop_dev_cfg_ptr->dma_reg_base;
        dma_hw_info_ptr->dma_reg_size = dma_prop_dev_cfg_ptr->dma_reg_size;
        dma_hw_info_ptr->dma_int_reg_offset_addr = dma_prop_dev_cfg_ptr->dma_int_reg_offset_addr;
        dma_hw_info_ptr->rddma_reg_offset_addr = dma_prop_dev_cfg_ptr->rddma_reg_offset_addr;
        dma_hw_info_ptr->wrdma_reg_offset_addr = dma_prop_dev_cfg_ptr->wrdma_reg_offset_addr;
        dma_hw_info_ptr->stc_rddma_reg_offset_addr = dma_prop_dev_cfg_ptr->stc_rddma_reg_offset_addr;
        dma_hw_info_ptr->stc_wrdma_reg_offset_addr = dma_prop_dev_cfg_ptr->stc_wrdma_reg_offset_addr;
        dma_hw_info_ptr->wrdma5_reg_offset_addr = dma_prop_dev_cfg_ptr->wrdma5_reg_offset_addr;
        dma_hw_info_ptr->stc_wrdma5_reg_offset_addr = dma_prop_dev_cfg_ptr->stc_wrdma5_reg_offset_addr;
        dma_hw_info_ptr->sink_dma_cnt = dma_prop_dev_cfg_ptr->sink_dma_cnt;
        dma_hw_info_ptr->source_dma_cnt = dma_prop_dev_cfg_ptr->source_dma_cnt;
        dma_hw_info_ptr->int_irq_no = dma_prop_dev_cfg_ptr->int_irq_no;
        dma_hw_info_ptr->int_output_index = dma_prop_dev_cfg_ptr->int_output_index;
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Audioif DMA is not supported");
        result_type0 = ADSP_EUNSUPPORTED;
      }
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting audioif DMA Property Struct Ptr");
      result_type0 = ADSP_EFAILED;
    }
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for AUDIOIFDMA");
    result_type0 = ADSP_EFAILED;
  }

  dma_prop_dev_cfg_ptr  = NULL;
  dma_hw_info_ptr = &afe_dma_mgr_driver_state_info.dma_hw_info[LPASS_HW_DMA_TYPE1];
  const char *dev_id_dma_type1 = "HDMIOUTDMA";

  memset(dma_hw_info_ptr, 0, sizeof(lpasshwio_prop_afe_dma_struct_t));

  DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl_dma_type1);

  /* Get device handle */
  if(DAL_SUCCESS == DALSYS_GetDALPropertyHandleStr(dev_id_dma_type1, dal_hdl_dma_type1))
  {
    /* Read property ptr */
    if(DAL_SUCCESS == DALSYS_GetPropertyValue(dal_hdl_dma_type1, "HdmiOutDmaPropStructPtr", 0, &prop_var))
    {
      dma_prop_dev_cfg_ptr = (lpasshwio_prop_afe_dma_struct_t *)prop_var.Val.pStruct;

      dma_hw_info_ptr->hw_revision = dma_prop_dev_cfg_ptr->hw_revision;

      /* hw revision */
      if(0 != dma_hw_info_ptr->hw_revision)
      {
        dma_hw_info_ptr->dma_reg_base = dma_prop_dev_cfg_ptr->dma_reg_base;
        dma_hw_info_ptr->dma_reg_size = dma_prop_dev_cfg_ptr->dma_reg_size;
        dma_hw_info_ptr->dma_int_reg_offset_addr = dma_prop_dev_cfg_ptr->dma_int_reg_offset_addr;
        dma_hw_info_ptr->rddma_reg_offset_addr = dma_prop_dev_cfg_ptr->rddma_reg_offset_addr;
        dma_hw_info_ptr->wrdma_reg_offset_addr = dma_prop_dev_cfg_ptr->wrdma_reg_offset_addr;
        dma_hw_info_ptr->stc_rddma_reg_offset_addr = dma_prop_dev_cfg_ptr->stc_rddma_reg_offset_addr;
        dma_hw_info_ptr->stc_wrdma_reg_offset_addr = dma_prop_dev_cfg_ptr->stc_wrdma_reg_offset_addr;
        dma_hw_info_ptr->sink_dma_cnt = dma_prop_dev_cfg_ptr->sink_dma_cnt;
        dma_hw_info_ptr->source_dma_cnt = dma_prop_dev_cfg_ptr->source_dma_cnt;
        dma_hw_info_ptr->int_irq_no = dma_prop_dev_cfg_ptr->int_irq_no;
        dma_hw_info_ptr->int_output_index = dma_prop_dev_cfg_ptr->int_output_index;
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HDMIOUT DMA is not supported");
        result_type1 = ADSP_EUNSUPPORTED;
      }
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting HDMIOUT DMA Property Struct Ptr");
      result_type1 = ADSP_EFAILED;
    }
  }
  else
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for HDMIOUTDMA");
    result_type1 = ADSP_EFAILED;
  }

  /* If both are un-supported */
  if((ADSP_EUNSUPPORTED == result_type0) &&
      (ADSP_EUNSUPPORTED == result_type1))
  {
    result = ADSP_EUNSUPPORTED;
  }
  /* If either of the dev cfg init fails */
  else if((ADSP_EFAILED == result_type0) ||
      (ADSP_EFAILED == result_type1))
  {
    result = ADSP_EFAILED;
  }
  else /* At least one EOK and no EFAILED */
  {
    result = ADSP_EOK;
  }

  return result;
}

/**
  SW workaround for HW bug in v60, where cached write is taking huge latency. So for specific chips,
  we would keep LPM as uncached, until it is fixed.
  For LPM read operations, keeping it uncached would lead to degradation in performance but since most of our
  processing is done in Rx path(LPM write), we want the latency impact to be minimum in that path.

  This function is for copying the data without using memscpy which has dczero instruction causing latency hit.
  Now with Sample based fwk, the number of samples would always be even and min bit rate is 2 bytes, so 32 bit would
  be the minimum for for_loop iterations.

  @param[in]  dst_ptr: Destination ptr
              dst_len: dest length in bytes
              src_ptr: Source ptr
              src_len: src length in bytes

  @return
  Void

  @dependencies
  None.
 */
void afe_dma_buff_copy(int8_t *dst_ptr, uint32_t dst_len, int8_t *src_ptr, uint32_t src_len)
{

  if((NULL == dst_ptr) || (NULL == src_ptr) || (0 == dst_len) || (0 == src_len))
  {
    MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Invalid dma copy params: dst: 0x%lx, dst_len: %lu, src: 0x%lx, dst_len: %lu ",
          dst_ptr, dst_len, src_ptr, src_len);
    return;
  }

  if(lpm_is_it_cached())
  {
    memscpy(dst_ptr, dst_len, src_ptr, src_len);
  }
  else
  {
    uint32_t iter = 0;
    uint32_t copy_len = (dst_len <= src_len) ?
        dst_len : src_len;
    uint32_t remain_len = copy_len;

    /* If both src and dst are 8 byte aligned */
    if((0 == ((int32_t)src_ptr & BITWISE_CHECK_FOR_8_BYTE_ALIGNMENT))
        && (0 == ((int32_t)dst_ptr & BITWISE_CHECK_FOR_8_BYTE_ALIGNMENT)))
    {
      int64_t *src_64_ptr = (int64_t *)src_ptr;
      int64_t *dst_64_ptr = (int64_t *)dst_ptr;

      for(iter = 0; iter < divide_by_8(copy_len); iter++)
      {
        (*dst_64_ptr++) = (*src_64_ptr++);
      }

      remain_len = copy_len & BITWISE_CHECK_FOR_8_BYTE_ALIGNMENT;
      dst_ptr = (int8_t *)dst_64_ptr;
      src_ptr = (int8_t *)src_64_ptr;
    }
    /* If both src and dst are 4 byte aligned */
    else if((0 == ((int32_t)src_ptr & BITWISE_CHECK_FOR_4_BYTE_ALIGNMENT))
        && (0 == ((int32_t)dst_ptr & BITWISE_CHECK_FOR_4_BYTE_ALIGNMENT)))
    {
      int32_t *src_32_ptr = (int32_t *)src_ptr;
      int32_t *dst_32_ptr = (int32_t *)dst_ptr;

      for(iter = 0; iter < divide_by_4(copy_len); iter++)
      {
        (*dst_32_ptr++) = (*src_32_ptr++);
      }

      remain_len = copy_len & BITWISE_CHECK_FOR_4_BYTE_ALIGNMENT;
      dst_ptr = (int8_t *)dst_32_ptr;
      src_ptr = (int8_t *)src_32_ptr;
    }
    /* If both src and dst are 2 byte aligned */
    else if((0 == ((int32_t)src_ptr & BITWISE_CHECK_FOR_2_BYTE_ALIGNMENT))
        && (0 == ((int32_t)dst_ptr & BITWISE_CHECK_FOR_2_BYTE_ALIGNMENT)))
    {
      int16_t *src_16_ptr = (int16_t *)src_ptr;
      int16_t *dst_16_ptr = (int16_t *)dst_ptr;

      for(iter = 0; iter < divide_by_2(copy_len); iter++)
      {
        (*dst_16_ptr++) = (*src_16_ptr++);
      }

      remain_len = copy_len & BITWISE_CHECK_FOR_2_BYTE_ALIGNMENT;
      dst_ptr = (int8_t *)dst_16_ptr;
      src_ptr = (int8_t *)src_16_ptr;
    }

    /* Copy 1 byte at a time for remaining len */
    for(iter = 0; iter < remain_len; iter++)
    {
      (*dst_ptr++) = (*src_ptr++);
    }
  }

  return;
}

/**
  Cache invalidate operation for DMA memory

  @param[in]  virt_addr: Starting virtual address to be invalidated.
              mem_size: size of the region(bytes) to be invalidated.

  @return
  Void

  @dependencies
  None.
 */
ADSPResult afe_dma_buff_cache_invalidate(uint32_t virt_addr, uint32_t mem_size)
{
  ADSPResult result = ADSP_EOK;

  if(lpm_is_it_cached())
  {
    result = qurt_elite_memorymap_cache_invalidate(virt_addr, mem_size);
  }

  return result;
}

/**
  Cache flush operation for DMA memory

  @param[in]  virt_addr: Starting virtual address to be flushed.
              mem_size: size of the region(bytes) to be flushed.

  @return
  Void

  @dependencies
  None.
 */
ADSPResult afe_dma_buff_cache_flush(uint32_t virt_addr, uint32_t mem_size)
{
  ADSPResult result = ADSP_EOK;

  if(lpm_is_it_cached())
  {
    result = qurt_elite_memorymap_cache_flush(virt_addr, mem_size);
  }

  return result;
}

