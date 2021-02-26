/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriverUtils.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     afe_slimbus_driver_utils.cpp

DESCRIPTION: AFE Slimbus driver utility functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright (c) 2011-2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Technologies, Inc. (QTI) Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriverUtils.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/
#include "AFESlimbusDriverUtils_i.h"
#include "ddislimbus.h"
#include "AFEInternal.h"
#include "AFEDevService.h"
#include "qurt_elite.h"
#include "adsp_afe_service_commands.h"
#include "AFESlimbusTimerUtils_i.h"
#include "ddislimbus.h"
#include "AFEDeviceDriver.h"
#include "AFESlimbusSlaveCfg_i.h"
#include "AFEInternal.h"
#include "dsdUtil.h"

extern afe_slimbus_device_config_t afe_sb_dev_cfg[MAX_SLIMBUS_HW_MODULES];

/*=====================================================================
 Functions
 ======================================================================*/
void*  afe_dal_bam_callback (void *cbCtxt, uint32 uSize1, void *pBuf2, uint32 uSize2)
{
  afe_dev_port_t* afe_port_ptr = (afe_dev_port_t *)cbCtxt;

  /* Skip the first interrupt processing (needed for multi-channel case),
	   This is mitigating the HW issue with generating the interrupt when first channel copy is done. */
  /* For 8Khz and mono case, first interrupt will come before port state changes to RUN. So, skipping
	   first interrupt should be independent of port state */
  if (afe_port_ptr->is_first_interrupt)
  {
    afe_port_ptr->is_first_interrupt = FALSE;
    return NULL;
  }

  if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
  {
    /*if software latched read the progress counter and av-timer*/
    afe_avt_drift_params_t *avt_drift_params_ptr = (afe_avt_drift_params_t *)&afe_port_ptr->avt_drift_params;
    if(TIMESTAMP_LATCH_TYPE_SOFTWARE == avt_drift_params_ptr->avtimer_latch_type)
    {
      afe_read_avt_and_sb_counters_atomically(afe_port_ptr, &avt_drift_params_ptr->sw_latched_dev_time,
                                              &afe_port_ptr->port_av_timestamp);
    }

    // signal to the port worker loop about the finishing of the data transfer
    afe_port_ptr->dynamic_thread_ptr->isr_context_counter++;
    qurt_elite_signal_send(&afe_port_ptr->dma_signal);
  }

  return NULL;
}

void*  afe_dal_port_callback (void *cbCtxt, uint32 uSize1, void *pBuf2, uint32 uSize2)
{
  ADSPResult result=ADSP_EOK;
  afe_dev_port_t* afe_port_ptr = (afe_dev_port_t *)cbCtxt;

  if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
  {
    afe_slimbus_state_struct_t* p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
    SlimBusResourceHandle h_master_port = p_dev_state->a_master_port_info[p_dev_state->afe_sb_cfg.num_master_ports - 1].h_master_port;
    SlimBusEventNotifyType portEvent;

    // get the port event information
    result = DalSlimBus_GetPortEvent(p_dev_state->p_sb_driver->p_core_driver, h_master_port, &portEvent);

    // if it port disconnect event, then signal to the sw thread which will be waiting for device closure
    if((portEvent.eEvent & (SLIMBUS_EVENT_O_FIFO_TRANSMIT_UNDERRUN|SLIMBUS_EVENT_O_FIFO_RECEIVE_OVERRUN)) || (result != DAL_SUCCESS))
    {
      // increment the counter in case of overflow or underflow events or any errors
      p_dev_state->port_error_cnt++;
    }
  }
  return NULL;
}

ADSPResult afe_sb_open_stream_ports(afe_slimbus_state_struct_t *p_dev_state)
{
  ADSPResult result = ADSP_EOK;
  DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
  afe_sb_master_port_info_t *p_stream_info = p_dev_state->a_master_port_info;
  uint16_t port_idx, desc_idx;
  SlimBusPortConfigType master_cfg;
  SlimBusResourceHandle h_master_port[AFE_PORT_MAX_AUDIO_CHAN_CNT];
  uint32_t virt_addr;
  uint32_t phy_addr = 0;
  uint16_t pipe_size;
  SlimBusPipeConfigType *pipe_cfg_info_ptr;
  uint32_t aligned_buf_size, aligned_buf_size_per_ch;
  uint32_t total_aligned_buf_size;

  memset(&master_cfg, 0, sizeof(SlimBusPortConfigType));

  //SLIMbus port will assert block_end after uBlockSize bytes,
  // only valid for producer data pipe in BAM-to-BAM mode.
  master_cfg.uBlockSize = 0;
  // SLIMbus port will assert trans_end after uTransSize bytes,
  // closing the current FIFO descriptor early; only valid for producer data pipe.
  master_cfg.uTransSize = 0;
  master_cfg.eOptions = p_dev_state->pack_mode;

  // choose the descriptors for the given device
  p_dev_state->desc_mem_type = AFE_DESC_MEM_LPM;
  p_dev_state->num_descptrs = MAX_DESCRIPTORS_PER_STREAM;
  master_cfg.uWaterMark = AFE_SB_PORT_CFG_WATERMARK_SIZE_2;
  if((AFE_DESC_MEM_LPM == p_dev_state->desc_mem_type)
      && (p_dev_state->afe_sb_cfg.sample_rate <= AFE_PORT_SAMPLE_RATE_48K))
  {
    master_cfg.uWaterMark = AFE_SB_PORT_CFG_WATERMARK_SIZE_1;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "num_descptrs: 0x%x, watermark: %lx", p_dev_state->num_descptrs, master_cfg.uWaterMark);

  // configure the BAM with default settings
  SlimBusBamRegisterEventType *bam_cfg_ptr = &p_dev_state->bam_cfg;
  bam_cfg_ptr->eOptions = SLIMBUS_BAM_O_DESC_DONE;
  bam_cfg_ptr->pUser = NULL;

  // create the BAM event. Single BAM event is used for all streams.
  // Only last stream gets registered for the BAM interrupt after data transfer
  bam_cfg_ptr->hEvent = NULL;
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &bam_cfg_ptr->hEvent, NULL);
  if(DAL_SUCCESS != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create BAM event, 0x%x", result);
    return ADSP_EFAILED;
  }

  // register the call back function for the events
  result = DALSYS_SetupCallbackEvent(bam_cfg_ptr->hEvent, afe_dal_bam_callback,
                                     (void*)p_dev_state->afe_port_ptr);
  if(DAL_SUCCESS != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register call back function to BAM events, 0x%x", result);
    return ADSP_EFAILED;
  }

  // create the master port event for capturing over-flow, underflow, port disconnect events
  p_dev_state->master_port_event = NULL;
  result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT, &p_dev_state->master_port_event,
                              NULL);
  if(DAL_SUCCESS != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create master port event, 0x%x", result);
    return ADSP_EFAILED;
  }

  // register the call back function for the events
  result = DALSYS_SetupCallbackEvent(p_dev_state->master_port_event, afe_dal_port_callback,
                                     (void*)p_dev_state->afe_port_ptr);
  if(DAL_SUCCESS != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register master port callback func, 0x%x", result);
    return ADSP_EFAILED;
  }

  if(p_dev_state->p_sb_driver->is_it_multi_end_point)
  {
    /* allocate master port(s) with multiple end points
     *
     * In the case of multi-end point, there would be many to one mapping between channels and master port.
     * Channels assigned to a particular master port would be updated in alloc master ports, which will be
     * useful
     *        in deciding BAM transfer buffer sizes and
     *        in connecting particular channel to corresponding port.*/

    uint8_t ep_index = 0, port_idx = 0, request_num_ch, rem_num_ch = p_dev_state->afe_sb_cfg.num_channels;

    uint32_t *a_max_eps_per_master = &(afe_sb_dev_cfg[p_dev_state->afe_sb_cfg.slimbus_dev_id].max_eps_per_master[0]);

    /* keep on requesting,  till all of the
     * channels were assigned to master ports */
    while( (rem_num_ch > 0) && (ep_index < NUM_SB_EPS_COMB) )
    {
      /* Request only upto max end points supported by master port.
       * If requesting number of channels is more than supported max end
       * points, try requesting in chunks*/
      if(rem_num_ch >= a_max_eps_per_master[ep_index])
      {
        request_num_ch = a_max_eps_per_master[ep_index];
      }
      else
      {
        request_num_ch = rem_num_ch;
      }

      //allocate master port with multiple end points
      if(DAL_SUCCESS == (result = afe_sb_multi_ep_alloc_master_ports(p_core_driver, MIN_FIFO_SIZE_IN_BYTES, &h_master_port[port_idx], request_num_ch)) )
      {
        /* assign requested number of channels to this master port*/
        p_dev_state->a_num_ch_per_port[port_idx] = request_num_ch;

        /* update the remaining channels*/
        rem_num_ch-= request_num_ch;

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Successfully allocated master port %d and assigned %d channels to it",port_idx, request_num_ch);

        /* update the master port index*/
        port_idx++;
      }
      else
      {
        /* Master port allocation failed, try with next available max end point*/
        ep_index++;

        if( NUM_SB_EPS_COMB == ep_index )
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate master port, tried all combinations of max end points", result);
          return ADSP_EFAILED;
        }

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Master port with request_num_ch:%d is not available, trying with next max end point:%d",\
              request_num_ch, a_max_eps_per_master[ep_index]);
      }
    }

    /* port index points to number of master ports allocated*/
    p_dev_state->afe_sb_cfg.num_master_ports = port_idx;
  }
  else
  {
    // Legacy case : allocate master ports for the whole group at once

    p_dev_state->afe_sb_cfg.num_master_ports = p_dev_state->afe_sb_cfg.num_channels;

    result = DalSlimBus_AllocMasterPorts(p_core_driver, SLIMBUS_PORT_REQ_O_MULTI_CHAN_GROUP,
                                         MIN_FIFO_SIZE_IN_BYTES, h_master_port,
                                         p_dev_state->afe_sb_cfg.num_master_ports * sizeof(h_master_port[0]));
    if(DAL_SUCCESS != result)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate master port, 0x%x", result);
      return ADSP_EFAILED;
    }

    /*For legacy case, there is 1:1 mapping between master port and channel */
    for(port_idx = 0; port_idx < p_dev_state->afe_sb_cfg.num_master_ports; port_idx++)
    {
      p_dev_state->a_num_ch_per_port[port_idx] = 1;
    }
  }

  uint32_t num_master_ports = p_dev_state->afe_sb_cfg.num_master_ports;

  // init the pipe cfg info, size of pipe, etc
  // Due to cicular nature of the FIFO, the additional +1 is used.
  // It is used to distinguish empty and full status ..

  /*for multi end points, there is HW bug for which we need to add extra pipe memory for holding dummy 
    decriptor. This dummy descriptor will be submitted by SLIMBus driver*/ 
  /*this will be removed once it is fixed in HW from Hana */ 
  if (p_dev_state->p_sb_driver->is_it_multi_end_point) 
  {
      pipe_size = (p_dev_state->num_descptrs +1 +1) * sizeof(SlimBusBamIOVecType); 
  }
  else
  {
  pipe_size = (p_dev_state->num_descptrs+1)*sizeof(SlimBusBamIOVecType);
  }
  
  aligned_buf_size = SLIMBUS_ALIGN(pipe_size, SLIMBUS_ALIGN_32_BYTES_MASK);
  total_aligned_buf_size = aligned_buf_size * num_master_ports;

  pipe_cfg_info_ptr = p_dev_state->pipe_cfg_info;
  memset(pipe_cfg_info_ptr, 0, num_master_ports*sizeof(SlimBusPipeConfigType));

  // Pipe memory allocation pass
  virt_addr = (uint32_t) afe_dev_lpa_aligned_malloc(total_aligned_buf_size, 32);
  if(0 == virt_addr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to get LPM memory for pipe cfg");
    return ADSP_ENOMEMORY;
  }
  // Convert to physical start address
  phy_addr =(uint32_t) qurt_elite_memorymap_get_physical_addr((uint32_t)virt_addr);
  if(0 == phy_addr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to get physical address for virtual address0x%x", virt_addr);
    afe_dev_lpa_aligned_free( (void *) virt_addr);
    return ADSP_EFAILED;
  }

  // Assign the allocated memory for individual pipe cfg
  pipe_cfg_info_ptr = p_dev_state->pipe_cfg_info;
  for(port_idx = 0; port_idx < num_master_ports; port_idx++)
  {
    pipe_cfg_info_ptr->DescFifo.uMinSize = pipe_size;
    pipe_cfg_info_ptr->eBamDev = SLIMBUS_BAM_DEV_HANDLE_MEM;
    pipe_cfg_info_ptr->DescFifo.pBase    = (void *) virt_addr;
    pipe_cfg_info_ptr->DescFifo.uPhysBase = phy_addr;
    pipe_cfg_info_ptr->DescFifo.uSize = pipe_cfg_info_ptr->DescFifo.uMinSize;
    MSG_3(MSG_SSID_QDSP6,DBG_HIGH_PRIO, " virt_addr :0x%x, phy_addr:0x%x, pipe size: %d,", virt_addr,phy_addr,pipe_cfg_info_ptr->DescFifo.uMinSize);
    virt_addr += aligned_buf_size;
    phy_addr += aligned_buf_size;
    pipe_cfg_info_ptr++;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LPM Pipe memory allocted size %ld for num stream %d",
        total_aligned_buf_size, num_master_ports);

  // Transfer buffer allocation phase
  aligned_buf_size_per_ch = SLIMBUS_ALIGN(p_dev_state->bam_trfr_size_per_ch, SLIMBUS_ALIGN_32_BYTES_MASK);

  total_aligned_buf_size = aligned_buf_size_per_ch * p_dev_state->afe_sb_cfg.num_channels* p_dev_state->num_descptrs ;

  if(AFE_DESC_MEM_LPM == p_dev_state->desc_mem_type)
  {
    virt_addr = (uint32_t)afe_dev_lpa_aligned_malloc(total_aligned_buf_size, 32);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LPM Des memory allocted size %ld",
          total_aligned_buf_size);
  }
  else
  {
    virt_addr = (uint32_t)qurt_elite_memory_aligned_malloc(total_aligned_buf_size, 32, QURT_ELITE_HEAP_DEFAULT);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Default Des memory allocted size %ld",
          total_aligned_buf_size);
  }

  if(0 == virt_addr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for descriptors");
    return ADSP_ENOMEMORY;
  }

  phy_addr =(uint32_t) qurt_lookup_physaddr((uint32_t)virt_addr);
  if(0 == phy_addr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to get physical address for virtual address0x%x", virt_addr);
    if (AFE_DESC_MEM_LPM == p_dev_state->desc_mem_type)
    {
      afe_dev_lpa_aligned_free((void *)virt_addr);
    }
    else
    {
      qurt_elite_memory_aligned_free((void *)virt_addr);
    }
    return ADSP_EFAILED;
  }
  // initialize the BAM buffers with all zeros
  if(p_dev_state->afe_port_ptr->dev_data_type == AFE_DSD_DOP_COMPRESSED_DATA_PORT)
  {
    //For DSD, we are inserting 0x69 as pause word per hardware req.
    memset((char *)virt_addr, DSD_PAUSE_WORD_BYTE, total_aligned_buf_size);
  }
  else
  {
    memset((char *)virt_addr, 0, total_aligned_buf_size);
  }

  pipe_cfg_info_ptr = p_dev_state->pipe_cfg_info;
  // initialize stream specific parameters (DMA buffer allocation and interrupt mode etc)
  for(port_idx = 0; port_idx < num_master_ports; port_idx++, p_stream_info++, pipe_cfg_info_ptr++)
  {
    // allocate & configure the descriptors for every stream
    for(desc_idx = 0; desc_idx < (p_dev_state->num_descptrs); desc_idx++)
    {
      uint8_t num_ch_per_port = p_dev_state->a_num_ch_per_port[port_idx];
      p_stream_info->cust_desc_info[desc_idx].virt_addr = virt_addr;
      p_stream_info->IOVec[desc_idx].uAddr = phy_addr;
      p_stream_info->IOVec[desc_idx].uSize = p_dev_state->bam_trfr_size_per_ch*num_ch_per_port;
      p_stream_info->IOVec[desc_idx].uFlags = SLIMBUS_BAM_IOVEC_FLAG_INT;
      phy_addr += aligned_buf_size_per_ch*num_ch_per_port;
      virt_addr += aligned_buf_size_per_ch*num_ch_per_port;
    }

    // assign the master port
    p_stream_info->h_master_port = h_master_port[port_idx];

    // register with port event handler to get under-run\over-flow\port-disconnect events
    // register for last stream in multi channels
    if(port_idx == (num_master_ports - 1))
    {
      result = DalSlimBus_RegisterPortEvent(p_core_driver, p_stream_info->h_master_port,
                                            p_dev_state->master_port_event, NULL);
      if(DAL_SUCCESS != result)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register master port event, 0x%x", result);
        return ADSP_EFAILED;
      }

      // initialize over-flow or under-flow count to 0
      p_dev_state->port_error_cnt = 0;
    }

    // configure the masterport
    if(DAL_SUCCESS
        != (result = DalSlimBus_ConfigMasterPort(p_core_driver, p_stream_info->h_master_port,
                                                 &master_cfg)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure master port, 0x%x", result);
      return ADSP_EFAILED;
    }

    if(port_idx == (num_master_ports - 1))
    {
      pipe_cfg_info_ptr->eOptions = (SlimBusBamOptionType)(
          SLIMBUS_BAM_O_DESC_DONE | SLIMBUS_BAM_O_ERROR | SLIMBUS_BAM_O_OUT_OF_DESC);
    }
    else
    {
      pipe_cfg_info_ptr->eOptions = (SlimBusBamOptionType)(
          SLIMBUS_BAM_O_ERROR | SLIMBUS_BAM_O_OUT_OF_DESC);
    }

    // configure master pipe which will be used to get data from memory to FIFO
    if(DAL_SUCCESS
        != (result = DalSlimBus_ConfigMasterPipe(p_core_driver, p_stream_info->h_master_port,
                                                 (SlimBusPortFlowType)p_dev_state->direction,
                                                 pipe_cfg_info_ptr)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to configure master pipe, 0x%x", result);
      return ADSP_EFAILED;
    }

    // register with BAM event handler to get data transfer events
    if(port_idx == (num_master_ports - 1))
    {
      result = DalSlimBus_RegisterBamEvent(p_core_driver, p_stream_info->h_master_port,
                                           SLIMBUS_BAM_DEFAULT, bam_cfg_ptr);
      if(DAL_SUCCESS != result)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register BAM event, 0x%x", result);
        return ADSP_EFAILED;
      }
    }
  }

  // initialize progress counter handle to NULL by default
  // and then allocate one progress counter per device
  // TBD: do we need to allocate progress counters based
  //      on need instead of default per device?
  p_dev_state->h_progress_ctr = 0;
  return (afe_sb_alloc_progress_counter(p_dev_state));
}

ADSPResult afe_sb_close_stream_ports(afe_slimbus_state_struct_t *p_dev_state)
{
  ADSPResult result=ADSP_EOK, curr_result;
  DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
  afe_sb_master_port_info_t *p_stream_info = p_dev_state->a_master_port_info;
  uint16_t port_idx, desc_idx;
  SlimBusPipeConfigType *pipe_cfg_info_ptr = p_dev_state->pipe_cfg_info;

  if (0 != p_dev_state->h_progress_ctr)
  {
    if(ADSP_EOK != (curr_result = afe_sb_dealloc_progress_counter(p_dev_state)) )
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Dealloc progress counter, result: 0x%x", curr_result);
      result |= curr_result;
    }
  }

  // Delllocate pipe cfg mem, first location point to all memory
  // and reset the pip cfg info
  if (pipe_cfg_info_ptr->DescFifo.pBase != NULL)
  {
    afe_dev_lpa_aligned_free((void *)pipe_cfg_info_ptr->DescFifo.pBase );
    for(port_idx = 0; port_idx < p_dev_state->afe_sb_cfg.num_master_ports; port_idx++)
    {
      pipe_cfg_info_ptr->DescFifo.pBase = NULL;
      pipe_cfg_info_ptr->DescFifo.uPhysBase = NULL;
      pipe_cfg_info_ptr++;
    }
  }

  // Deallocate mem for all desc
  // all mem is at the first location, i.e. first stream, first des
  if(0 != p_stream_info->cust_desc_info[0].virt_addr)
  {
    if (AFE_DESC_MEM_LPM == p_dev_state->desc_mem_type)
    {
      afe_dev_lpa_aligned_free((void *)p_stream_info->cust_desc_info[0].virt_addr);
    }
    else
    {
      qurt_elite_memory_aligned_free((void *)p_stream_info->cust_desc_info[0].virt_addr);
    }
  }

  // in case of valid stream, de-allocate the allocated memory for descriptors
  for(port_idx = 0; port_idx < p_dev_state->afe_sb_cfg.num_master_ports; port_idx++, p_stream_info++)
  {
    // de-allocate the master ports associated with this device
    if (p_stream_info->h_master_port)
    {
      if (DAL_SUCCESS != (curr_result = DalSlimBus_DeallocMasterPorts(p_core_driver, &(p_stream_info->h_master_port), sizeof(p_stream_info->h_master_port))))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Dealloc master port, 0x%x", curr_result);
        result |= curr_result;
      }
      p_stream_info->h_master_port = 0;
      p_dev_state->a_num_ch_per_port[port_idx] = 0;
    }

    // reset the descriptors for every stream
    for (desc_idx = 0; desc_idx < (p_dev_state->num_descptrs); desc_idx++)
    {
      p_stream_info->cust_desc_info[desc_idx].virt_addr = 0;
      p_stream_info->IOVec[desc_idx].uAddr = 0;
    }
  }

  if (NULL != p_dev_state->master_port_event)
  {
    DALSYS_DestroyObject(p_dev_state->master_port_event);
    p_dev_state->master_port_event = NULL;
  }

  if (NULL != p_dev_state->bam_cfg.hEvent)
  {
    DALSYS_DestroyObject(p_dev_state->bam_cfg.hEvent);
    p_dev_state->bam_cfg.hEvent = NULL;
  }

  return result;
}

ADSPResult afe_sb_open_data_channels(afe_slimbus_state_struct_t *p_dev_state)
{
  ADSPResult result;
  DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
  uint32_t master_port_idx = 0, ch_idx =0;

  // create resource group(s) for the requested device\interface
  if(p_dev_state->p_sb_driver->is_it_multi_end_point)
  {
    /* In the case of multi end point, each master port should have
     * unique channel group. Channels assigned to a particular master port, should
     * be added to this group and use this channel group as handle to call connect port to channels
     * */
    for(master_port_idx=0; master_port_idx < p_dev_state->afe_sb_cfg.num_master_ports; master_port_idx++)
    {
      if (DAL_SUCCESS != (result = DalSlimBus_AllocResourceGroup(p_core_driver, &p_dev_state->a_h_multi_ep_chan_group[master_port_idx])))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate resource group for mutli end point master port idx: %d, 0x%x", master_port_idx, result);
        return ADSP_EFAILED;
      }
    }
  }
  else
  {
    /* In legacy case, a single channel group is created and all of the channels need to be added
     * to this channel group. This channel group is used only to control all the channels
     *
     * During connect port to channel, data channel handle is used instead of channel group.
     * */
    if (DAL_SUCCESS != (result = DalSlimBus_AllocResourceGroup(p_core_driver, &p_dev_state->h_chan_group)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate resource group, 0x%x", result);
      return ADSP_EFAILED;
    }
  }

  /* allocate data channels and add them to their corresponding
   * channel group(s) and connect master port using associated channel group */
  for(master_port_idx=0; master_port_idx < p_dev_state->afe_sb_cfg.num_master_ports; master_port_idx++)
  {
    /*add all of the channels assigned to a particular master port to its associated channel group*/
    for(uint32_t idx=0; idx < p_dev_state->a_num_ch_per_port[master_port_idx]; idx++)
    {
      if (FALSE == p_dev_state->afe_sb_cfg.is_it_share_chan)
      {
        // allocate the data channel
        if (DAL_SUCCESS != (result = DalSlimBus_AllocDataChannel(p_core_driver, &(p_dev_state->h_data_channel[ch_idx]))))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate the channel, 0x%x", result);
          return ADSP_EFAILED;
        }
      }
      else
      {
        // allocate the data channel between master-port and slave-port
        if (DAL_SUCCESS != (result = DalSlimBus_AllocSharedDataChannel(p_core_driver,
                                                                       p_dev_state->afe_sb_cfg.shared_channel_mapping[ch_idx],
                                                                       &(p_dev_state->h_data_channel[ch_idx]))))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate the channel, 0x%x", result);
          return ADSP_EFAILED;
        }
      }

      if(p_dev_state->p_sb_driver->is_it_multi_end_point)
      {
        /* add the data channel to channel group associated for each master port*/
        if (DAL_SUCCESS != (result = DalSlimBus_AddResourceToGroup(p_core_driver,
                                                                   p_dev_state->a_h_multi_ep_chan_group[master_port_idx],
                                                                   p_dev_state->h_data_channel[ch_idx])))
        {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add channel %d to multi end point resource group, port idx %d, 0x%x",\
                ch_idx, master_port_idx, result);
          return ADSP_EFAILED;
        }
      }
      else
      { /* Legacy case, add all of the channels to a single channel group*/

        if (DAL_SUCCESS != (result = DalSlimBus_AddResourceToGroup(p_core_driver,
                                                                   p_dev_state->h_chan_group,
                                                                   p_dev_state->h_data_channel[ch_idx])))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add channel to resource group, 0x%x", result);
          return ADSP_EFAILED;
        }
      }

      /* update the channel index*/
      ch_idx++;
    }//idx for loop

    /* Connect channels to port*/
    if(p_dev_state->p_sb_driver->is_it_multi_end_point)
    {
      if(DAL_SUCCESS != (result = DalSlimBus_ConnectPortToChannel(p_core_driver, p_dev_state->a_h_multi_ep_chan_group[master_port_idx],
                                                                  (SlimBusPortFlowType)p_dev_state->direction,
                                                                  p_dev_state->a_master_port_info[master_port_idx].h_master_port)))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to connect multi end point master port to the channel group, master idx:%d, 0x%x", master_port_idx, result);
        return ADSP_EFAILED;
      }
    }
    else
    {
      /* Legacy case*/

      if (DAL_SUCCESS != (result = DalSlimBus_ConnectPortToChannel(p_core_driver, p_dev_state->h_data_channel[master_port_idx],
                                                                   (SlimBusPortFlowType)p_dev_state->direction,
                                                                   p_dev_state->a_master_port_info[master_port_idx].h_master_port)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to connect master port to the channel, 0x%x", result);
        return ADSP_EFAILED;
      }
    }

  }//master_port_idx for loop

  return ADSP_EOK;
}

ADSPResult afe_sb_close_data_channels(afe_slimbus_state_struct_t *p_dev_state)
{
  ADSPResult result = ADSP_EOK, curr_result = ADSP_EOK;
  DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
  afe_sb_master_port_info_t *p_stream_info =  p_dev_state->a_master_port_info;
  uint32_t port_idx = 0;

  if(p_dev_state->p_sb_driver->is_it_multi_end_point)
  {
    for(port_idx=0; port_idx < p_dev_state->afe_sb_cfg.num_master_ports; port_idx++)
    {
      // de-activate the data channel first
      if(DAL_SUCCESS != (curr_result = DalSlimBus_NextDataChannelControl(p_core_driver, p_dev_state->a_h_multi_ep_chan_group[port_idx], SLIMBUS_CHAN_CTRL_REMOVE)) )
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to remove the channel, 0x%x", curr_result);
        result |= curr_result;
      }
    }
  }
  else
  {
    /* Legacy case*/
    // de-activate the data channel first
    if(DAL_SUCCESS != (curr_result = DalSlimBus_NextDataChannelControl(p_core_driver, p_dev_state->h_chan_group, SLIMBUS_CHAN_CTRL_REMOVE)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to remove the channel, 0x%x", curr_result);
      // return; -> continue  with closing sequence and core driver should take care of the needed dependencies.
      result |= curr_result;
    }
  }

  if(DAL_SUCCESS != (curr_result = DalSlimBus_DoReconfigureNow(p_core_driver)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to do ReConfigNow(), 0x%x", curr_result);
    // return; -> continue  with closing sequence and core driver should take care of the needed dependencies.
    result |= curr_result;
  }

  // de-alloc all the data channels
  for(uint32_t i = 0; i < p_dev_state->afe_sb_cfg.num_channels; i++, p_stream_info++)
  {
    // de-allocate data channels associated with this device
    if (p_dev_state->h_data_channel[i])
    {
      curr_result = DalSlimBus_DeallocDataChannel(p_core_driver, p_dev_state->h_data_channel[i]);
      if (DAL_SUCCESS != curr_result)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-allocate the data channel, 0x%x", curr_result);
        result |= curr_result;
      }

      /* clean up the values*/
      p_dev_state->h_data_channel[i]   = 0;
    }
  }

  if(p_dev_state->p_sb_driver->is_it_multi_end_point)
  {
    // de-alloc resource groups of all master ports
    for(uint32_t port_idx=0; port_idx < p_dev_state->afe_sb_cfg.num_master_ports; port_idx++)
    {
      if (DAL_SUCCESS != (curr_result = DalSlimBus_DeallocResourceGroup(p_core_driver, p_dev_state->a_h_multi_ep_chan_group[port_idx])))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-allocate resource group for master port idx: %d, 0x%x", port_idx, curr_result);
        result |= curr_result;
      }
      p_dev_state->a_h_multi_ep_chan_group[port_idx] = 0;
    }
  }
  else
  {
    // Legacy case:de-alloc the resource group
    curr_result = DalSlimBus_DeallocResourceGroup(p_core_driver, p_dev_state->h_chan_group);
    if (DAL_SUCCESS != curr_result)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to de-allocate data channel resource group, 0x%x", curr_result);
      result |= curr_result;
    }

    p_dev_state->h_chan_group = 0;
  }

  return result;
}
