/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEDmaManager.cpp#2 $
$DateTime: 2017/11/23 06:25:06 $
$Author: pwbldsvc $
$Change: 14912369 $
$Revision: #2 $

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEDmaManager.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/25/13    RP      created

============================================================================*/

#include "qurt_elite.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "AFEDmaManager.h"
#include "AFEDmaManager_i.h"
#include "AFEHalDmaManager_i.h"
#include "AFEDeviceDriver.h"
#include "AFEHalDmaManagerCommon.h"
#include "AFESsrDrv.h"


// Get four byte aligned size
#define GET_4_BYTE_ALIGN(size) (((size)+3) & (~(0x3)))

extern afe_dma_mgr_driver_state_t afe_dma_mgr_driver_state_info;


/**
  Function which allocates read dma/write dma channels 

  @param[in]  num_dma_chs_req: number of dma channels requested 
  @param[in/out]  dma_idx_payload_ptr: array which stores the 
        dma channel index
  @param[in] dir indicates it is SINK or SOURCE 
  @param[in] dma_type  indicates dma type 0 or dma type 1

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_alloc_dma_resources_for_remote_client(uint8_t num_dma_chs_req, uint8_t *dma_idx_payload_ptr, uint8_t dir, uint8_t dma_type);

/**
  Function which frees read dma/write dma channels 

  @param[in]  num_dma_chs_req: number of dma channels requested 
  @param[in/out]  dma_idx_payload_ptr: array which stores the 
        dma channel index
  @param[in] dir indicates it is SINK or SOURCE 
  @param[in] dma_type  indicates dma type 0 or dma type 1

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_free_dma_resources_for_remote_client(uint8_t num_dma_chs_req, uint8_t *dma_idx_payload_ptr, uint8_t dir, uint8_t dma_type);




/**
  initialize afe dma manager
      initialize the dma state data including the dma resource state and interrupt registeration

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_init(void)
{
  ADSPResult   result = ADSP_EOK;

  if(ADSP_EOK != (result = afe_dma_mgr_resource_state_init()))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_init is failed");
  }

  return result;
}

/**
  deinitialize afe dma manager
      free all the allocated resources.

  @return
  aDSP error code

  @dependencies
  None.
 */

ADSPResult afe_dma_mgr_deinit(void)
{
  ADSPResult   result = ADSP_EOK;

  if(ADSP_EOK != (result = afe_dma_mgr_resource_state_deinit()))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_deinit is failed");
  }

  return result;
}

/**
  get the allocated DMA index

  @param[in]  h_dma_ptr                Pointer to the DMA manger handle
  @param[in]  uint32_t *dma_index_ptr   Parameters to get  the DMA index  

  @return 
  aDSP error code

  @dependencies 
  None.
 */
uint32_t afe_dma_mgr_get_dma_index(dma_device_handle h_dma_ptr)
{
  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  if((NULL == dma_mgr_state_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  return dma_mgr_state_ptr->dma_idx;
}


/**
  @brief Register with DMA manager

  @param[in] h_dma_ptr pointer to DMA manager handle
  @param[in] open_params_ptr pointer to the parameter list DMA Manager registration

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_dma_mgr_open(dma_device_handle *h_dma_ptr, dma_mgr_open_params_t *open_params_ptr)
{

  ADSPResult result = ADSP_EOK;

  if((NULL == h_dma_ptr) || (NULL == open_params_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle / Open Params Null");
    return ADSP_EFAILED;
  }

  switch(open_params_ptr->hardware_type)
  {
    case LPASS_HW_HDMI_INPUT:
    case LPASS_HW_HDMI_OUTPUT:
    case LPASS_HW_I2S:
    case LPASS_HW_PCM:
    case LPASS_HW_TDM:
    case LPASS_HW_HDMI14:
      break;
    default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Incorrect hardware type");
      return ADSP_EUNSUPPORTED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)qurt_elite_memory_malloc(sizeof(dma_manager_state_t),
                                                                                           QURT_ELITE_HEAP_DEFAULT);
  if(NULL == dma_mgr_state_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in allocating memory for DMA Manager state structure");
    return ADSP_ENORESOURCE;
  }
  memset(dma_mgr_state_ptr, 0, sizeof(dma_manager_state_t));

  //copy over the parameters to DMA state structure
  dma_mgr_state_ptr->hardware_type = open_params_ptr->hardware_type;
  dma_mgr_state_ptr->num_channels = open_params_ptr->num_channels;
  dma_mgr_state_ptr->sampling_rate = open_params_ptr->sample_rate;
  dma_mgr_state_ptr->int_samples_per_period = open_params_ptr->int_samples_per_period;
  dma_mgr_state_ptr->input_bytes_per_sample = open_params_ptr->input_bytes_per_sample;
  dma_mgr_state_ptr->output_bytes_per_sample = open_params_ptr->output_bytes_per_sample;
  dma_mgr_state_ptr->dma_read_func_ptr = open_params_ptr->dma_read_func_ptr;
  dma_mgr_state_ptr->dma_write_func_ptr = open_params_ptr->dma_write_func_ptr;
  dma_mgr_state_ptr->buff_mem_type = open_params_ptr->buff_mem_type;
  dma_mgr_state_ptr->watermark = open_params_ptr->watermark;
  dma_mgr_state_ptr->num_buffers = open_params_ptr->num_buffers;
  dma_mgr_state_ptr->dma_type = open_params_ptr->dma_type;
  dma_mgr_state_ptr->dma_dir = open_params_ptr->dma_dir;
  dma_mgr_state_ptr->data_type = open_params_ptr->data_type;

  result = afe_dma_mgr_alloc_dma(dma_mgr_state_ptr);
  dma_mgr_state_ptr->ifconfig_dma_control = open_params_ptr->ifconfig_dma_control;
  dma_mgr_state_ptr->ifconfig_dma_control_int = open_params_ptr->ifconfig_dma_control_int;
  dma_mgr_state_ptr->dma_int_cb = open_params_ptr->dma_int_cb;
  dma_mgr_state_ptr->afe_driver_context = open_params_ptr->afe_driver_context;

  if(ADSP_EOK != result)
  {
    goto __bailout;
  }
  if(ADSP_EOK != (result = afe_dma_manager_get_dma_buffer_size(dma_mgr_state_ptr)))
  {
    goto __bailout;
  }

  //allocate the DMA buffer and zero initialize it
  result = afe_dma_manager_alloc_dma_buffer(dma_mgr_state_ptr);
  if(ADSP_EOK != result)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Failed to allocate DMA buffer");
    goto __bailout;
  }
  if(open_params_ptr->data_type == AFE_DSD_DOP_DATA)
  {
    memset(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr, 0x69, dma_mgr_state_ptr->dma_buffer_size);

  }
  else
  {
    memset(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr, 0, dma_mgr_state_ptr->dma_buffer_size);
  }

  //flush the cache associated with the DMA memory
  result = qurt_elite_memorymap_cache_flush((uint32_t)dma_mgr_state_ptr->dma_buffer_virt_addr_ptr,
                                            dma_mgr_state_ptr->dma_buffer_size);
  if(ADSP_EOK != result)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to flush cache associated with DMA buffer");
    goto __bailout;
  }

  //SW starts reading from the beginning of DMA buffer
  dma_mgr_state_ptr->dma_buffer_next_avail_addr = dma_mgr_state_ptr->dma_buffer_virt_addr_ptr;

  //store the handle
  *h_dma_ptr = (dma_device_handle *)dma_mgr_state_ptr;

  return ADSP_EOK;

  __bailout:
  //If allocated free the DMA buffer
  if(NULL != dma_mgr_state_ptr->dma_buffer_virt_addr_ptr)
  {
    if(DMA_MGR_DDR_BUFFER == dma_mgr_state_ptr->buff_mem_type)
    {
      qurt_elite_memory_aligned_free(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
    }
    else
    {
      afe_dev_lpa_aligned_free(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
    }
    dma_mgr_state_ptr->dma_buffer_virt_addr_ptr = NULL;
  }

  if(NULL != dma_mgr_state_ptr)
  {
    //free the DMA manager state
    qurt_elite_memory_free(dma_mgr_state_ptr);
    dma_mgr_state_ptr = NULL;
  }

  return result;
}


/**
  Deregisters DMA manager client

  @param[in]  h_dma_ptr                DMA manger handle

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_close(dma_device_handle h_dma_ptr)
{
  ADSPResult result = ADSP_EOK;
  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  if((NULL == dma_mgr_state_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  result = afe_dma_mgr_free_dma(dma_mgr_state_ptr);

  //If allocated free the DMA buffer
  if(NULL != dma_mgr_state_ptr->dma_buffer_virt_addr_ptr)
  {
    if(DMA_MGR_DDR_BUFFER == dma_mgr_state_ptr->buff_mem_type)
    {
      qurt_elite_memory_aligned_free(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
    }
    else
    {
      afe_dev_lpa_aligned_free(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
    }
    dma_mgr_state_ptr->dma_buffer_virt_addr_ptr = NULL;
  }

  //free the DMA manager state
  qurt_elite_memory_free(dma_mgr_state_ptr);
  dma_mgr_state_ptr = NULL;

  return result;
}


/**
  @brief Starts the DMA Engine

  @param[in] h_dma_ptr pointer to DMA manager handle
  @param[in] start_param_ptr pointer to the parameter list for setting up DMA

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_dma_mgr_start(dma_device_handle h_dma_ptr)
{
  uint32_t wps_count;
  hal_dma_config_t dma_config;
  ADSPResult   result = ADSP_EOK;

  if(NULL == h_dma_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DMA Manager handle / Open Params Null");
    return ADSP_EFAILED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  //make sure DMA is disabled before enabling it
  if(ADSP_EOK != (result = afe_hal_dma_disable_dma_channel(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to disable dma channel [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
    return result;
  }

  //wps count is the number of 32 bit words per sample element (sample element = bytes per sample * number of channels)
  // if the number of 32 bits words per sample element is less than 1 Dword, then set it to 1.
  wps_count = (dma_mgr_state_ptr->num_channels * dma_mgr_state_ptr->input_bytes_per_sample) >> 2;
  if(0 == wps_count)
  {
    wps_count = 1;
  }

  //init and fill the dma config args.
  memset(&dma_config, 0, sizeof(hal_dma_config_t));

  //calculate burst size depending on the period length.
  //Period count should be a multiple of burst size
  if(0 == (dma_mgr_state_ptr->period_count_in_word32 & DMA_BUFFER_MGR_16_BYTE_ALIGNMENT))
  {
    dma_config.burst_size = 16;
  }
  else if(0 == (dma_mgr_state_ptr->period_count_in_word32 & DMA_BUFFER_MGR_8_BYTE_ALIGNMENT))
  {
    dma_config.burst_size = 8;
  }
  else if(0 == (dma_mgr_state_ptr->period_count_in_word32 & DMA_BUFFER_MGR_4_BYTE_ALIGNMENT))
  {
    dma_config.burst_size = 4;
  }
  else
  {
    dma_config.burst_size = 1;
  }

  //configure the DMA channel
  dma_config.buffer_len = dma_mgr_state_ptr->dma_buffer_size/sizeof(uint32_t); //buffer length in terms of 32 bit words
  dma_config.buffer_start_addr = dma_mgr_state_ptr->dma_buffer_phy_addr_ptr;
  dma_config.dma_int_per_cnt = dma_mgr_state_ptr->period_count_in_word32;
  dma_config.wps_count = wps_count;
  dma_config.watermark = dma_mgr_state_ptr->watermark;
  dma_config.ifconfig_dma_control = dma_mgr_state_ptr->ifconfig_dma_control;
  dma_config.ifconfig_dma_control_int = dma_mgr_state_ptr->ifconfig_dma_control_int;

  //NOTE: if burst size is 1 or dma_int_per_cnt is smaller than dma fifo size, then the burst is supposed to be disabled.

  //config the DMA channel
  if(ADSP_EOK != (result = afe_hal_dma_config_dma_channel(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, &dma_config)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to config dma channel [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  } //clear only the DMA interrupt status
  else if(ADSP_EOK != (result = afe_hal_dma_clear_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to cliar dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  } //enable the interrupt
  else if(ADSP_EOK != (result = afe_hal_dma_enable_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to enable dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }//enable the DMA channel
  else if(ADSP_EOK != (result = afe_hal_dma_enable_dma_channel(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to enable dma channel [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }

  afe_ssr_update_lpass_dma_status(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, TRUE);

  return result;
}

/**
  De-initializes DMA engine

  @param[in]  h_dma_ptr                DMA manger handle

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_stop(dma_device_handle h_dma_ptr)
{
  ADSPResult   result = ADSP_EOK;

  if((NULL == h_dma_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  //disable the interrupt
  if(ADSP_EOK != (result = afe_hal_dma_disable_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to disable dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  } //clear the interrupt
  else if(ADSP_EOK != (result = afe_hal_dma_clear_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to clear dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);

  }//disable the DMA channel
  else if(ADSP_EOK != (result = afe_hal_dma_disable_dma_channel(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to disable dma channel [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }

  afe_ssr_update_lpass_dma_status(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, FALSE);

  return result;
}

/**
  disable and clear DMA interrupt

  @param[in]  h_dma_ptr                DMA manger handle

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_disable_clear_dma_int(dma_device_handle h_dma_ptr)
{
  ADSPResult   result = ADSP_EOK;

  if((NULL == h_dma_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  //disable the interrupt
  if(ADSP_EOK != (result = afe_hal_dma_disable_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to disable dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  } //clear the interrupt
  else if(ADSP_EOK != (result = afe_hal_dma_clear_dma_interrupt(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to clear dma interrupt [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }
  else if(LPASS_HW_DMA_SINK == dma_mgr_state_ptr->dma_dir)
  {
    /**<Below change has been done as a s/w workaround for h/w (digital codec) limitation
     * If i2s/DMA is stopped first, and codec Rx is still running, codec may read
     * the last non-zero sample in its buffer continuously, resulting in DC pulse
     * generation at codec output, which is perceived as pop noise when high->zero->high
     * transition happens in this DC pulse.
     *
     * Below workaround zero-fills the DMA FIFO, i2s data lines before i2s/DMA disable. This
     * ensures that codec is not stuck at reading non-zero value after DMA disable
     * */

    if(NULL != dma_mgr_state_ptr->dma_buffer_virt_addr_ptr)
    {
      /* Zero out the DMA buffer */
      memset(dma_mgr_state_ptr->dma_buffer_virt_addr_ptr, (dma_mgr_state_ptr->data_type == AFE_DSD_DOP_DATA) ? 0x69 : 0, dma_mgr_state_ptr->dma_buffer_size);

      /**<flush the cache associated with the DMA memory.*/
      if(ADSP_EOK != (result = qurt_elite_memorymap_cache_flush((uint32_t)dma_mgr_state_ptr->dma_buffer_virt_addr_ptr,
                                                                dma_mgr_state_ptr->dma_buffer_size)) )
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to flush cache associated with DMA buffer");
      }
      else
      {
        /**< Sleep for 625 us to let the DMA FIFO and i2s buffers drained out
         * For 8 KHz, FIFO watermark is set to 1 DWORD + 3 samples in i2s h/w
         * buffer.
         * Total worst case delay = 2+3 = 5 samples @ 16-bit 8 KHz = 625 us*/
        avtimer_drv_sleep(625);
      }
    }
  }
  return result;
}


/**
  Read from the DMA buffers

  @param[in]  h_dma_ptr                DMA manger handle
  @param[in]  buffer_ptr               Destination buffer pointer

  @return
  aDSP error code

  @dependencies
  None.
 */
ADSPResult afe_dma_mgr_read(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr)
{
  ADSPResult result = ADSP_EOK;

  if((NULL == h_dma_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  uint32_t *curr_addr;

  // This is also need to move to driver specific functions.
  //read the current address that DMA Is writing to
  (void) afe_hal_dma_get_dma_curr_addr(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, (uint32_t *)&curr_addr);
  uint32_t *next_read_addr = dma_mgr_state_ptr->dma_buffer_next_avail_addr;

  //check to see if there is enough data in DMA buffer to read
  uint32_t hw_offset = (uint32_t)(curr_addr - dma_mgr_state_ptr->dma_buffer_phy_addr_ptr);
  uint32_t sw_offset = (uint32_t)(next_read_addr - dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
  int32_t diff = hw_offset - sw_offset;
  uint32_t dma_buff_size_in_word32 = dma_mgr_state_ptr->dma_buffer_size >> 2;

  if(diff < 0)
  {
    diff += dma_buff_size_in_word32;
  }

  if((uint32_t)diff < dma_mgr_state_ptr->period_count_in_word32)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Warning: Not enough 32 bit words available to read from DMA buffer, Available words = %lu",
          (uint32_t)diff);
    //set back the next available address to read
    next_read_addr += dma_mgr_state_ptr->period_count_in_word32;
    if(next_read_addr >= dma_mgr_state_ptr->dma_buffer_virt_addr_ptr + dma_buff_size_in_word32)
    {
      //wrap around
      next_read_addr = dma_mgr_state_ptr->dma_buffer_virt_addr_ptr;
    }
  }

  //invalidate cache before reading, function internally takes care of alignment to cache line boundary
  result = afe_dma_buff_cache_invalidate((uint32_t)next_read_addr,
                                         dma_mgr_state_ptr->period_count_in_word32 * sizeof(int32));

  //copy from DMA buffer to client buffer
  dma_mgr_state_ptr->dma_read_func_ptr(dma_mgr_state_ptr->afe_driver_context, next_read_addr, buffer_ptr);

  //move ahead the read pointer
  next_read_addr += dma_mgr_state_ptr->period_count_in_word32;
  if(next_read_addr >= dma_mgr_state_ptr->dma_buffer_virt_addr_ptr + dma_buff_size_in_word32)
  {
    //wrap around
    next_read_addr = dma_mgr_state_ptr->dma_buffer_virt_addr_ptr;
  }
  dma_mgr_state_ptr->dma_buffer_next_avail_addr = next_read_addr;

  return result;
}

ADSPResult afe_dma_mgr_write(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr)
{
  ADSPResult result = ADSP_EOK;

  if((NULL == h_dma_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to DMA Manager handle Null");
    return ADSP_EFAILED;
  }

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  
  uint32_t *next_write_addr = dma_mgr_state_ptr->dma_buffer_next_avail_addr;
  uint32_t *curr_addr;  
  (void) afe_hal_dma_get_dma_curr_addr(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx,
                                         (uint32_t *)&curr_addr);

  //check to see if we are not writing into the same buffer that hardware is
  //reading from
  uint32_t hw_offset = (uint32_t)(curr_addr - dma_mgr_state_ptr->dma_buffer_phy_addr_ptr);
  uint32_t sw_offset = (uint32_t)(next_write_addr - dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);
  int32_t diff = hw_offset - sw_offset;
  uint32_t dma_buff_size_in_word32 = dma_mgr_state_ptr->dma_buffer_size >> 2;

  if(diff < 0)
  {
    diff += dma_buff_size_in_word32;
  }

  if((uint32_t)diff < dma_mgr_state_ptr->period_count_in_word32)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Warning: Not enough 32 bit words available to write into DMA buffer, Available words = %lu",
          (uint32_t)diff);
    //set back the next available address to read
    next_write_addr += dma_mgr_state_ptr->period_count_in_word32;
    if(next_write_addr >= dma_mgr_state_ptr->dma_buffer_virt_addr_ptr + dma_buff_size_in_word32)
    {
      //wrap around
      next_write_addr = dma_mgr_state_ptr->dma_buffer_virt_addr_ptr;
    }
  }

  uint32_t tail_length = dma_buff_size_in_word32 - (next_write_addr - dma_mgr_state_ptr->dma_buffer_virt_addr_ptr);

  if(tail_length >= dma_mgr_state_ptr->period_count_in_word32)
  {
    //copy from client buffer to DMA buffer
    if(dma_mgr_state_ptr->dma_write_func_ptr)
    {
      dma_mgr_state_ptr->dma_write_func_ptr(dma_mgr_state_ptr->afe_driver_context, buffer_ptr, next_write_addr);
    }

    //cache flush
    result = afe_dma_buff_cache_flush((uint32_t)next_write_addr,
                                      dma_mgr_state_ptr->period_count_in_word32 * sizeof(int32));

    ///< update the address for software processing next time.
    if (tail_length > dma_mgr_state_ptr->period_count_in_word32)
    {
      next_write_addr += dma_mgr_state_ptr->period_count_in_word32;
    }
    else
    {
      next_write_addr = dma_mgr_state_ptr->dma_buffer_virt_addr_ptr;
    }   

    ///< Store next available addr for processing the subsequent iteration.
    dma_mgr_state_ptr->dma_buffer_next_avail_addr = next_write_addr;
  }

  return result;
}

ADSPResult afe_dma_mgr_read_timestamp_fifo_count(dma_device_handle h_dma_ptr, uint64_t *dma_int_timestamp_ptr, uint32_t *fifo_count_ptr)
{
  ADSPResult   result = ADSP_EOK;

  dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)h_dma_ptr;

  if(ADSP_EOK != (result = afe_hal_dma_get_dma_int_stc(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, \
                                                       dma_int_timestamp_ptr)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to read STC [dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }
  else if(ADSP_EOK != (result = afe_hal_dma_get_dma_fifo_count(dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx, \
                                                               fifo_count_ptr)))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Manager failed to read fifo count[dma type:%d, dir:%d, idx:%d]",
          dma_mgr_state_ptr->dma_type, dma_mgr_state_ptr->dma_dir, dma_mgr_state_ptr->dma_idx);
  }

  return result;
}


ADSPResult afe_alloc_dma_resources_for_remote_client(uint8_t num_dma_chs_req, uint8_t *dma_idx_store_ptr, uint8_t dir, uint8_t dma_type)
{
  ADSPResult   result = ADSP_EOK;
  uint16_t i;

  for (i=0; i<num_dma_chs_req; i++) 
  {
    dma_manager_state_t *dma_mgr_state_ptr = (dma_manager_state_t *)qurt_elite_memory_malloc(sizeof(dma_manager_state_t),
                                                                                           QURT_ELITE_HEAP_DEFAULT);

    if (dma_mgr_state_ptr == NULL) 
    {
        MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"memory alloc failed");
        return ADSP_ENOMEMORY;
    }
    memset(dma_mgr_state_ptr,0,sizeof(dma_manager_state_t));
    dma_mgr_state_ptr->dma_type  = dma_type;
    dma_mgr_state_ptr->dma_dir   = dir;
    dma_mgr_state_ptr->dma_client_id = LPASS_HW_DMA_REMOTE_CLIENT;

    /* if any resource alloc is failing, return the error*/
    /*As part of error clean up, will free the allocated dma channels */
    if (ADSP_FAILED(result = afe_dma_mgr_alloc_dma(dma_mgr_state_ptr)))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"DMA channels allocation failed, dir:0x%x", dir);
      qurt_elite_memory_free(dma_mgr_state_ptr);
      return ADSP_EFAILED;
    }
    else
    {
      //if resource is allocating succesfully, storing in result payload ptr 
      dma_idx_store_ptr[i] = dma_mgr_state_ptr->dma_idx;
    }
  }
  return ADSP_EOK;
}


ADSPResult afe_free_dma_resources_for_remote_client(uint8_t num_dma_chs_req, uint8_t *dma_idx_payload_ptr, uint8_t dir, uint8_t dma_type)
{
    ADSPResult result = ADSP_EOK;
    dma_mgr_resource_state_t  *dma_resource_state_ptr;
    uint8_t  dma_ch_idx;
    uint16_t max_dma_channels, i;
    dma_manager_state_t  *dma_mgr_state_ptr;

    if(LPASS_HW_DMA_SINK == dir)
    {  
      dma_resource_state_ptr = afe_dma_mgr_driver_state_info.sink_dma_state_ptr[dma_type];
      max_dma_channels       = afe_dma_mgr_driver_state_info.dma_hw_info[dma_type].sink_dma_cnt;
    }
    else
    {
      dma_resource_state_ptr = afe_dma_mgr_driver_state_info.source_dma_state_ptr[dma_type];
      max_dma_channels       = afe_dma_mgr_driver_state_info.dma_hw_info[dma_type].source_dma_cnt;
    }
    for (i=0; i< num_dma_chs_req; i++) 
    { 
       dma_ch_idx =  *dma_idx_payload_ptr++;
       /*dma index should be in the range of 0 to max_dma_channels-1 */
       if (dma_ch_idx >= max_dma_channels ) 
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Invalid channel index 0x%x",dma_ch_idx);
          return ADSP_EFAILED; 
       }
       dma_mgr_state_ptr = dma_resource_state_ptr[dma_ch_idx].h_dma_ptr;

      if (dma_mgr_state_ptr != NULL) 
      {
          if(ADSP_FAILED(result = afe_dma_mgr_free_dma(dma_mgr_state_ptr)))
          {
              MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Error in frreing dma idx: 0x%x dir:0x%x", dma_ch_idx,dir);
          }
          qurt_elite_memory_free(dma_mgr_state_ptr);
      }
      else
      {
          MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Null pointer of DMA state structure");
      }
    }
    return result;
}



ADSPResult afe_validate_dma_resources_send_by_remote_client(uint8_t num_dma_chs_req, uint8_t *dma_idx_payload_ptr, uint8_t dir, uint8_t dma_type)
{
    dma_mgr_resource_state_t  *dma_resource_state_ptr;
    uint8_t  dma_ch_idx = 0;
    dma_manager_state_t  *dma_mgr_state_ptr;
    uint16_t max_dma_channels, i;

    if(LPASS_HW_DMA_SINK == dir)
    {   max_dma_channels       = afe_dma_mgr_driver_state_info.dma_hw_info[dma_type].sink_dma_cnt;
        dma_resource_state_ptr = afe_dma_mgr_driver_state_info.sink_dma_state_ptr[dma_type];
    }
    else
    {
        max_dma_channels       = afe_dma_mgr_driver_state_info.dma_hw_info[dma_type].source_dma_cnt;
        dma_resource_state_ptr = afe_dma_mgr_driver_state_info.source_dma_state_ptr[dma_type];
    }
    for (i=0; i< num_dma_chs_req; i++) 
    { 
       dma_ch_idx =  *dma_idx_payload_ptr ;
       /*dma index should be in the range of 0 to max_dma_channels-1 */
       if (dma_ch_idx >= max_dma_channels ) 
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Invalid channel index 0x%x",dma_ch_idx);
          return ADSP_EFAILED; 
       }
       dma_mgr_state_ptr = dma_resource_state_ptr[dma_ch_idx].h_dma_ptr;

      if (dma_mgr_state_ptr != NULL) 
      {
          if ((dma_mgr_state_ptr->dma_idx != dma_ch_idx) ||
               dma_mgr_state_ptr->dma_client_id != LPASS_HW_DMA_REMOTE_CLIENT) 
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " Invalid channel index received 0x%x",dma_ch_idx);
            return ADSP_EFAILED;
          }
      }
      else
      {
          MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Null pointer of DMA state structure");
          return ADSP_EFAILED;
      }
      dma_idx_payload_ptr++;
    }

    return ADSP_EOK;
}


/**
  @brief Function handler to request LPASS DMA resources by 
         remote client .

  @param[in] ptr, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_request_lpass_dma_resources_by_remote_client_handler(void *ptr)
{
  elite_apr_packet_t *rsp_apr_pkt_ptr = NULL;
  int32_t rc = APR_EOK;
  ADSPResult result= ADSP_EOK;
  uint32_t  rsp_pkt_size=0;
  uint8_t *rd_dma_ch_rsp_payload_ptr = NULL, *wr_dma_ch_rsp_payload_ptr = NULL;
  void *apr_rsp_payload_ptr=NULL;
  uint8_t *payload_ptr = NULL;
  uint16_t variable_payload_size;
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t *)ptr;

  if (elite_apr_if_get_payload_size(apr_pkt_ptr) < \
      (sizeof(afe_cmd_request_lpass_dma_resources))+(sizeof(afe_cmd_request_lpass_resources)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for LPASS DMA resources");
    rc = ADSP_EUNEXPECTED;
    goto afe_request_lpass_dma_resources_error_1;
  }

  /* parse command payload */
  int8_t *apr_payload_ptr;
  afe_cmd_request_lpass_dma_resources *cmd_payload_ptr;
  apr_payload_ptr = (int8_t *)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));
  /* Get the actual payload for DMA request*/
  cmd_payload_ptr = (afe_cmd_request_lpass_dma_resources *)(apr_payload_ptr+sizeof(afe_cmd_request_lpass_resources));

  if (NULL ==cmd_payload_ptr) 
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "apr payloadptr is NULL");
    rc = ADSP_EUNEXPECTED;
    goto afe_request_lpass_dma_resources_error_1;
  }

  if (cmd_payload_ptr->dma_type != AFE_LPAIF_DEFAULT_DMA_TYPE)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "apr payloadptr is NULL");
    rc = ADSP_EFAILED;
    goto afe_request_lpass_dma_resources_error_1;
  }
  
  variable_payload_size = GET_4_BYTE_ALIGN((cmd_payload_ptr->num_read_dma_channels*sizeof(uint8_t))+ \
                   (cmd_payload_ptr->num_write_dma_channels*sizeof(uint8_t)));

  /* calculate response apr packet size*/
  /* variable payload is sumof read dma channels and write dma channels bytes*/
  rsp_pkt_size = sizeof(afe_cmdrsp_request_lpass_resources)  + \
                 sizeof(afe_cmdrsp_request_lpass_dma_resources) +  \
                 variable_payload_size ;  


  /* Allocate the return packet */
  if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(afe_svc_global_ptr->apr_handle,
                                                  elite_apr_if_get_dst_addr(apr_pkt_ptr),
                                                  elite_apr_if_get_dst_port(apr_pkt_ptr),
                                                  elite_apr_if_get_src_addr(apr_pkt_ptr),
                                                  elite_apr_if_get_src_port(apr_pkt_ptr),
                                                  elite_apr_if_get_client_token(apr_pkt_ptr),
                                                  AFE_CMDRSP_REQUEST_LPASS_RESOURCES,
                                                  rsp_pkt_size, &rsp_apr_pkt_ptr)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, \
          "AFE_SVC_CMD_REQUEST_LPASS_RESOURCES Response packet allocation failed  = %ld!", rc);
    rc = ADSP_EFAILED;
    goto afe_request_lpass_dma_resources_error_1;
  }
  if (NULL == rsp_apr_pkt_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, \
        "afe_remote_lpass_core_hw_vote_reqeust Response packet allocation failed. pPacket is NULL");
    rc = ADSP_ENOMEMORY;
    goto afe_request_lpass_dma_resources_error_1;
  }

  /* Get the response payload */
  elite_apr_if_get_payload((void **)(&apr_rsp_payload_ptr), rsp_apr_pkt_ptr);
  afe_cmdrsp_request_lpass_dma_resources *dma_rsp_payload_ptr;
  afe_cmdrsp_request_lpass_resources    *lpass_resource_rsp_payload_ptr;
  lpass_resource_rsp_payload_ptr = (afe_cmdrsp_request_lpass_resources *)apr_rsp_payload_ptr;
  dma_rsp_payload_ptr = (afe_cmdrsp_request_lpass_dma_resources *)(lpass_resource_rsp_payload_ptr+1);

  /*Logic of the function is, even if any resource allocation is failed, free up the allocated 
     resources which are done as part of this command. Either allcoate all request resources
     or none of the resources. Not doing partial allocation*/

  /*response will be in this fashion: 
    resource_id 
    dma_type
    num_rd_dma_channels
    num_wr_dma_channels
    reserved
    <Variable payload>
    <Number of read dma_channel bytes will follow>
    <Number of write dma_channel bytes will follow> */

  /* Fill the response payload ptr with success values*/
  lpass_resource_rsp_payload_ptr->status       =  ADSP_EOK; 
  lpass_resource_rsp_payload_ptr->resource_id  =  AFE_LPAIF_DMA_RESOURCE_ID;
  dma_rsp_payload_ptr->dma_type                =  cmd_payload_ptr->dma_type;
  dma_rsp_payload_ptr->num_read_dma_channels   =  cmd_payload_ptr->num_read_dma_channels;
  dma_rsp_payload_ptr->num_write_dma_channels  =  cmd_payload_ptr->num_write_dma_channels;
  dma_rsp_payload_ptr->reserved                =  0;

  /*Move  the ptr to variable payload*/
  payload_ptr = (uint8_t *)(dma_rsp_payload_ptr+1);
  /*Fill the variable payload with 0xFF, as "0" is valid dma index" */
  memset(payload_ptr, 0xFF, variable_payload_size);

  /* Assign payload ptr to rd_dma_ch_rsp_payload_ptr */
  rd_dma_ch_rsp_payload_ptr = payload_ptr;
  /*Move the ptr by  num_read_dma_channels bytes to get the wr_dma_ch_payload ptr*/ 
  wr_dma_ch_rsp_payload_ptr =  rd_dma_ch_rsp_payload_ptr+ (cmd_payload_ptr->num_read_dma_channels*sizeof(uint8_t));

  if (cmd_payload_ptr->num_read_dma_channels > 0) 
  {

    if(ADSP_FAILED(result = afe_alloc_dma_resources_for_remote_client(cmd_payload_ptr->num_read_dma_channels, rd_dma_ch_rsp_payload_ptr,\
                                                                       LPASS_HW_DMA_SINK ,cmd_payload_ptr->dma_type)))
    {
       MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "allocation of Read dma failed 0x%x", result);
       /*As read dma allocation is failed, just send the response without proceeding further*/ 
       goto lpass_dma_resources_response;
    }
  }
  if (cmd_payload_ptr->num_write_dma_channels > 0) 
  {
    if(ADSP_FAILED(result |= afe_alloc_dma_resources_for_remote_client(cmd_payload_ptr->num_write_dma_channels,wr_dma_ch_rsp_payload_ptr,\
                                                                        LPASS_HW_DMA_SOURCE, cmd_payload_ptr->dma_type )))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "allocation of write dma failed 0x%x", result);
    }
  }

  lpass_dma_resources_response:
  /* if any allcoation is failed, set status= ADSP_EFAILED and free the resources which are allocated above */
  if (ADSP_FAILED(result) ) 
  {
    lpass_resource_rsp_payload_ptr->status =  ADSP_EFAILED; 
    dma_rsp_payload_ptr->num_read_dma_channels  = 0;
    dma_rsp_payload_ptr->num_write_dma_channels  = 0;

    /* No need to validate before freeing as we are passing same indices allocated by DSP*/
    /* Also this errors returning by this fucntions are ignored. It will return failure only for invalid indices*/
    if(ADSP_FAILED(result = afe_free_dma_resources_for_remote_client(cmd_payload_ptr->num_read_dma_channels, rd_dma_ch_rsp_payload_ptr,\
                                                                       LPASS_HW_DMA_SINK ,cmd_payload_ptr->dma_type)))
    {
        MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, " Error in freeing dma resources 0x%x",result);
    }
   
    if(ADSP_FAILED(result = afe_free_dma_resources_for_remote_client(cmd_payload_ptr->num_write_dma_channels, wr_dma_ch_rsp_payload_ptr,\
                                                                       LPASS_HW_DMA_SOURCE ,cmd_payload_ptr->dma_type)))
    {
        MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, " Error in freeing rma resources 0x%x",result);
    }
    
  }

  /* Send response packet out */
  if (APR_EOK != (rc = elite_apr_if_async_send(afe_svc_global_ptr->apr_handle, rsp_apr_pkt_ptr)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_remote_lpass_dma_resources_reqeust Response send failed  = %ld!", rc);
    rc = ADSP_EFAILED;
    goto afe_request_lpass_dma_resources_error_2;
  }
  /* free the original packet */
  (void)elite_apr_if_free(afe_svc_global_ptr->apr_handle, apr_pkt_ptr);

  return ADSP_EOK;

  afe_request_lpass_dma_resources_error_2:
  //free response packet
  (void)elite_apr_if_free(afe_svc_global_ptr->apr_handle, rsp_apr_pkt_ptr);
  afe_request_lpass_dma_resources_error_1:
  //ack back the apr packet
  (void)elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, apr_pkt_ptr, rc);
  return rc;
}

ADSPResult afe_release_lpass_dma_resources_by_remote_client_handler(void *ptr)
{
  ADSPResult result = APR_EOK;
  uint8_t num_read_dma_ch, num_write_dma_ch,dma_type;
  uint8_t *rd_dma_ch_idx_ptr, *wr_dma_ch_idx_ptr;
  uint32_t expected_apr_pkt_size=0;
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t *)ptr;

  if (!apr_pkt_ptr)
  {
    return ADSP_EBADPARAM;
  }
  /* parse command payload */
  /* This ptr is already validated before calling this function */
  afe_cmd_release_lpass_resources *apr_payload_ptr;
  apr_payload_ptr = (afe_cmd_release_lpass_resources *)(elite_apr_if_get_payload_ptr(apr_pkt_ptr));

  afe_cmd_release_lpass_dma_resources *cmd_payload_ptr;
  cmd_payload_ptr = (afe_cmd_release_lpass_dma_resources *)(apr_payload_ptr + 1);

  if (cmd_payload_ptr == NULL) 
  {
    result = ADSP_EFAILED;
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO," Null poniter for APR payload");
    goto afe_release_lpass_dma_resources_error;
  }
  dma_type = cmd_payload_ptr->dma_type;

  if ( dma_type != AFE_LPAIF_DEFAULT_DMA_TYPE) 
  {
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO," Invalid DMA type");
    goto afe_release_lpass_dma_resources_error;
  }
  /*apr pkt size should be  sizeof(afe_cmd_release_lpass_dma_resources)+ 
     num_read_dma_ch bytes+num-wr-dma_ch-bytes */
  expected_apr_pkt_size = sizeof(afe_cmd_release_lpass_resources)+\
                          sizeof(afe_cmd_release_lpass_dma_resources)+\
                          (cmd_payload_ptr->num_read_dma_channels*sizeof(uint8_t)) + \
                          (cmd_payload_ptr->num_write_dma_channels*sizeof(uint8_t))   ; 

  if (elite_apr_if_get_payload_size(apr_pkt_ptr) < expected_apr_pkt_size)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for AFE_SVC_CMD_RELEASE_LPASS_DMA_RESOURCES");
    result = ADSP_EUNEXPECTED;
    goto afe_release_lpass_dma_resources_error;
  }

  /*Do the actual processing for releasing resources*/
  {
    /* get num_read_dma_ch*/
    num_read_dma_ch =  cmd_payload_ptr->num_read_dma_channels;
    num_write_dma_ch  = cmd_payload_ptr->num_write_dma_channels;
    /*rd_dma_ch_idx_ptr holds the start address of variable payload*/
    rd_dma_ch_idx_ptr =  (uint8_t *)(cmd_payload_ptr + 1);
    /* now get  wr_dma_ch_idx location by moving the rd_dma_ch_idx  by
         num_read_dma_ch bytes*/
    wr_dma_ch_idx_ptr=  (rd_dma_ch_idx_ptr+ num_read_dma_ch*sizeof(uint8_t));

    /* first validate the given payload.If validation is failed, just return the error code*/
    /* We are not freeing any resources if validation is failed*/
    /* this validation checks whether HLOS sending the same resources which are allocated in request command*/    
    if(ADSP_FAILED( afe_validate_dma_resources_send_by_remote_client(num_read_dma_ch, rd_dma_ch_idx_ptr, LPASS_HW_DMA_SINK,dma_type) || \
                    afe_validate_dma_resources_send_by_remote_client(num_write_dma_ch,wr_dma_ch_idx_ptr,LPASS_HW_DMA_SOURCE,dma_type)))
    {
      MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Rcvd dma indexes which are not allocated to HLOS, erroring out");
      result = ADSP_EFAILED;
      goto afe_release_lpass_dma_resources_error;
    }
    /* At this point all validations are succesful, so we can release the resources*/    
    if(ADSP_FAILED( afe_free_dma_resources_for_remote_client(num_read_dma_ch, rd_dma_ch_idx_ptr,LPASS_HW_DMA_SINK,dma_type) || \
                    afe_free_dma_resources_for_remote_client(num_write_dma_ch,wr_dma_ch_idx_ptr,LPASS_HW_DMA_SOURCE,dma_type)))
    {
     result = ADSP_EFAILED; 
      MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Error in freeing resources");
      goto afe_release_lpass_dma_resources_error;
    }
  }

  afe_release_lpass_dma_resources_error:
  /* end command */
  (void)elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, apr_pkt_ptr, result);
  return result;
}
