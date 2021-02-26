/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/inc/AFEDmaManager.h#2 $
 ====================================================================== */

#ifndef _DMA_MANAGER_H_
#define _DMA_MANAGER_H_

/*==========================================================================
  Include Files
========================================================================== */
#include "AFEInternal.h"
#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

  /*==========================================================================
  Macro\enum definitions
========================================================================== */
  typedef enum
  {
    LPASS_HW_HDMI_INPUT,
    LPASS_HW_HDMI_OUTPUT,
    LPASS_HW_I2S,
    LPASS_HW_PCM,
    LPASS_HW_TDM,
    LPASS_HW_HDMI14,
    LPASS_HW_MAX_HARDWARE_BLOCKS
  }lpass_hw_t;

  enum
  {
    LPASS_HW_DMA_TYPE0 = 0,    //lpass  audioif dma
    LPASS_HW_DMA_TYPE1,        //lpass hdmi tx 2.0
    LPASS_HW_DMA_MAX_TYPES
  };

  enum
  {
    LPASS_HW_DMA_SINK,
    LPASS_HW_DMA_SOURCE,
  };

  enum
  {
    DMA_MGR_DATA_TYPE_UNCOMPRESSED,
    DMA_MGR_DATA_TYPE_COMPRESSED,
  };

  /* DMA manager buffer types.
   */
  enum
  {
    DMA_MGR_LPM_BUFFER =0,
    /* Allocate DMA buffer in LPM memory */

    DMA_MGR_DDR_BUFFER,
    /* Allocate DMA buffer in DDR memory */

    DMA_MGR_LPM_DDR_BUFFER
    /* First try to allocate in LPM memory, if LPM
     * is exhausted, then allocate in DDR memory. */
  };

  /**
   * dma type2 hw interrupt event type
   used for I2S, PCM, HDMI 1.4 drivers
   */
  enum
  {
    DMA_TYPE2_EVENT_DATA                  = 1,
    DMA_TYPE2_EVENT_FIFOERROR             = 2,
    DMA_TYPE2_EVENT_BUSERROR              = 4,
  };

  /**
   * dma type0 hw interrupt event type
   used for HDMI out/display port 2.0 driver
   */
  enum
  {
    DMA_TYPE0_EVENT_DATA                  = 1,
    DMA_TYPE0_EVENT_UNDEERRUN             = 2,
    DMA_TYPE0_EVENT_ERROR                 = 4,

    DMA_TYPE0_EVENT_DP_METADATA_DONE      = 8,
    DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR      = 16,   //If SW enables METADATA while DP core engine is diabled,
    //QDSP will never get DMA_TYPE0_EVENT_DP_METADATA_DONE interrupt
    //instead lpass hw will issue DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR interrupt
    //so SW can handle this accordingly
  };

/* This enum provides the information who is owning particualr dma index*/
/* By default LPASS_HW_DMA_DEFAULT_LOCAL_CLIENT is owner. */
/* only for remote request resources set to  LPASS_HW_DMA_REMOTE_CLIENT*/

  typedef enum
  {
    LPASS_HW_DMA_DEFAULT_LOCAL_CLIENT =0,
    LPASS_HW_DMA_REMOTE_CLIENT,
  }dma_ch_master_t;


#define AFE_INVALID_DMA_INDEX   0xFFFF

  typedef void* dma_device_handle;

  typedef void (*dma_mgr_read_func) (void * , uint32_t * /*src_ptr*/, uint8_t */*dst_ptr*/);   //for AFE SOURCE port
  typedef void (*dma_mgr_write_func) (void * , uint8_t * /*src_ptr*/, uint32_t */*dst_ptr*/);   //for AFE SINK port

  /*==========================================================================
  Struct definitions
========================================================================== */

  /** callback function pointer */

  typedef void (*dma_mgr_cb_func) (
      void                *cb_context,    /** A void pointer to user defined context */
      uint32_t               event_mask     /** Callback event type mask*/
  ) ;


  typedef struct dma_mgr_open_params
  {
    uint32_t sample_rate;
    uint32_t num_channels;
    uint32_t input_bytes_per_sample;
    uint32_t output_bytes_per_sample;
    uint32_t int_samples_per_period;
    uint32_t watermark;
    uint32_t buff_mem_type;
    uint32_t num_buffers;
    lpass_hw_t hardware_type;
    uint16_t data_type;
    uint32_t dma_type;
    uint32_t dma_dir;
    uint32_t dma_idx;                // this is only for audio if and hdmi out which has more than one dma.
    uint32_t ifconfig_dma_control;   // This is only for audio if which has more than one audio interfaces to be associated with dma.
    uint32_t ifconfig_dma_control_int; /* This is only for  internal i2s which has more than one audio interfaces to be associated with dma.
                                        Either iconfig_dma_control or iconfig_dma_control  only will be non zero at a time per dma.
                                        This wont be used for other interaces like PCM IF,, TDM, HDMI etc*/
    dma_mgr_read_func dma_read_func_ptr;
    dma_mgr_write_func dma_write_func_ptr;
    void * afe_driver_context;    /** callback context */
    dma_mgr_cb_func dma_int_cb;  // This is only for audio if because the interrupt handling is handled in dma manger.
  }dma_mgr_open_params_t;

  typedef struct dma_manager_state
  {
    uint32_t sampling_rate;
    uint32_t num_channels;
    uint32_t input_bytes_per_sample;
    uint32_t output_bytes_per_sample;
    uint32_t dma_buffer_size;
    uint32_t num_buffers;
    uint32_t total_buffer_size;
    uint32_t period_count_in_word32;
    uint32_t *dma_buffer_virt_addr_ptr;
    uint32_t *dma_buffer_phy_addr_ptr;
    uint32_t *dma_buffer_next_avail_addr;
    uint32_t int_samples_per_period;
    uint16_t data_type;
    uint32_t buff_mem_type;
    uint32_t watermark;
    lpass_hw_t hardware_type;      // only for debugging purpose
    uint32_t dma_type;
    uint32_t dma_dir;
    uint32_t dma_idx;
    uint32_t ifconfig_dma_control; // This is only for audioif which has more than one audio interfaces to be associated with dma.
    uint32_t ifconfig_dma_control_int; /* This is only for  internal i2s which has more than one audio interfaces to be associated with dma.
                                        either iconfig_dma_control or iconfig_dma_control  only will be non zero at a time per dma
                                        This wont be used for other interaces like PCM IF,, TDM, HDMI etc*/
    dma_ch_master_t dma_client_id;  
    dma_mgr_read_func dma_read_func_ptr;
    dma_mgr_write_func dma_write_func_ptr;
    void * afe_driver_context;   /** callback context */
    dma_mgr_cb_func dma_int_cb;  // This is only for audio if because the interrupt handling is handled in dma manger.
  }dma_manager_state_t;

  /*==========================================================================
  Function declarations
========================================================================== */

  /**
  initialize afe dma manager
      initialize the dma state data including the dma resource state and interrupt registeration

  @return
  aDSP error code

  @dependencies
  None.
   */
  ADSPResult afe_dma_mgr_init(void);

  /**
  deinitialize afe dma manager
      free all the allocated resources.

  @return
  aDSP error code

  @dependencies
  None.
   */

  ADSPResult afe_dma_mgr_deinit(void);

  /**
  Sets up the DMA hardware

  @param[in]  h_dma_ptr                Pointer to the DMA manger handle
  @param[in]  dma_mgr_open_params_t    Parameters to configure the DMA hardware  

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_open(dma_device_handle *h_dma_ptr, dma_mgr_open_params_t *open_params);


  /**
  Deinitializes DMA hardware

  @param[in]  h_dma_ptr                DMA manger handle  

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_close(dma_device_handle h_dma_ptr);


  /**
  @brief Starts the DMA Engine

  @param[in] h_dma_ptr pointer to DMA manager handle
  @param[in] start_param_ptr pointer to the parameter list for setting up DMA

  @return  ADSP_EOK on success, an error code on error

   */
  ADSPResult afe_dma_mgr_start(dma_device_handle h_dma_ptr);


  /**
  De-initializes DMA engine

  @param[in]  h_dma_ptr                DMA manger handle

  @return
  aDSP error code

  @dependencies
  None.
   */
  ADSPResult afe_dma_mgr_stop(dma_device_handle h_dma_ptr);

  /**
  disable and clear DMA interrupt

  @param[in]  h_dma_ptr                DMA manger handle

  @return
  aDSP error code

  @dependencies
  None.
   */
  ADSPResult afe_dma_mgr_disable_clear_dma_int(dma_device_handle h_dma_ptr);


  /**
  Read from the DMA buffers

  @param[in]  h_dma_ptr                DMA manger handle
  @param[in]  buffer_ptr               Destination buffer pointer   

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_read(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr);

  /**
  Write to the DMA buffers

  @param[in]  h_dma_ptr                DMA manger handle
  @param[in]  buffer_ptr               Source buffer pointer   

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_write(dma_device_handle h_dma_ptr, uint8_t *buffer_ptr);

  /**
  get the allocated DMA index

   @param[in]  h_dma_ptr                DMA manger handle

   @return 
   dma index

   @dependencies 
   None.
   */
  uint32_t afe_dma_mgr_get_dma_index(dma_device_handle h_dma_ptr);


  /**
  Read dma time stamp and fifo count

  @param[in]  h_dma_ptr                DMA manger handle
  @param[in]  dma_int_timestamp_ptr            timestamp to be read   
  @param[in]  fifo_count_ptr                          fifo count  to be read   

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_read_timestamp_fifo_count(dma_device_handle h_dma_ptr, uint64_t *dma_int_timestamp_ptr, uint32_t *fifo_count_ptr);


  /**
  Reset all dmas

  @param[in]  dma_type                 DMA type to be reset

  @return 
  aDSP error code

  @dependencies 
  None.
   */
  ADSPResult afe_dma_mgr_reset_dma(uint32_t dma_type);

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
  void afe_dma_buff_copy(int8_t *dst_ptr, uint32_t dst_len, int8_t *src_ptr, uint32_t src_len);

  /**
  Cache invalidate operation for DMA memory

  @param[in]  virt_addr: Starting virtual address to be invalidated.
              mem_size: size of the region(bytes) to be invalidated.

  @return
  Void

  @dependencies
  None.
   */
  ADSPResult afe_dma_buff_cache_invalidate(uint32_t virt_addr, uint32_t mem_size);

  /**
  Cache flush operation for DMA memory

  @param[in]  virt_addr: Starting virtual address to be flushed.
              mem_size: size of the region(bytes) to be flushed.

  @return
  Void

  @dependencies
  None.
   */
  ADSPResult afe_dma_buff_cache_flush(uint32_t virt_addr, uint32_t mem_size);



/**
  @brief Function which allocates  LPAIF DMA resources to remote
         client(HLOS).
  
  @param[in] ptr, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_request_lpass_dma_resources_by_remote_client_handler(void *ptr);


/**
  @brief Function  releases  LPAIF DMA resources which are 
         assigned to remote client(HLOS).
   
  
  @param[in] ptr, pointer to APR packet

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_release_lpass_dma_resources_by_remote_client_handler(void *ptr);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _DMA_MANAGER_H_
