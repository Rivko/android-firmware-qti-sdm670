/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHdmi14Driver.cpp#2 $
$DateTime: 2017/11/23 06:25:06 $
$Author: pwbldsvc $
$Change: 14912369 $
$Revision: #2 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHdmi14Driver.cpp#2 $ $DateTime: 2017/11/23 06:25:06 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "qurt_error.h"
#include "AFEDeviceDriver.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "adsp_afe_service_commands.h"
#include "AFEHdmi14Driver.h"
#include "AFEHdmi14Driver_i.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalHdmi14.h"
#include "AFEHdmi14Utils.h"
#include "AFEDeviceDriverInterface.h"
#include "hwd_devcfg.h"

/*=====================================================================
 Globals
 ======================================================================*/
//Note: if it is possible, decouple driver from service layer for better modulization.


//static declarations for afe ports per port
static afe_dev_port_t port_rx_hdmi14;

//static delcarations for hdmi14 dev port state per hdmi14 interfaces.
static hdmi14_port_state_t  hdmi14_rx;


//global variable for hdmi14 driver to share any variable common to all ports for example device config, etc.
static hdmi14_drv_state_t    hdmi14_driver; 


/*=====================================================================
 Static Functions
 ======================================================================*/
static ADSPResult afe_hdmi14_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size);
static ADSPResult afe_hdmi14_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);
static ADSPResult afe_hdmi14_driver_start(void *afe_drv, dev_start_params_t *hdmi14_start_params_ptr);
static ADSPResult afe_hdmi14_driver_stop(void *afe_drv);
static ADSPResult afe_hdmi14_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr);
static ADSPResult afe_hdmi14_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size);
static ADSPResult afe_hdmi14_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *hdmi14_port_cfg_data_ptr);
static void afe_hdmi14_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address);
static ADSPResult afe_hdmi14_dev_config_port (hdmi14_port_state_t* p_dev_state, afe_param_id_hdmi_multi_chan_audio_cfg_t *hdmi14_cfg_ptr);
static void afe_hdmi14_int_callback(void* afe_drv, uint32 event_mask);
static ADSPResult afe_hdmi14_driver_write(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static uint32 afe_hdmi14_get_watermark(hdmi14_port_state_t *p_dev_state);
static uint32 afe_hdmi14_get_num_dma_buffer(hdmi14_port_state_t *p_dev_state);
static ADSPResult afe_hdmi14_driver_device_cfg_init(hdmi14_dev_cfg_struct_t * hdmi14_hw_info_ptr);

static afe_driver_func_tbl_t sink_ftbl =
{

    afe_hdmi14_driver_write,
    afe_hdmi14_set_param,
    afe_hdmi14_get_param,
    afe_hdmi14_driver_start,
    afe_hdmi14_driver_stop,
    NULL,
    afe_hdmi14_get_properties,
};


/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform hdmi14 Driver Initialization
            The driver init function does
                  1. get hardware device config for hdmi14
                  2. get the virtual addres for hardware registers
                  3. get the avtimer tick frequency which will be used for converting the dma timestamp to usec.
                  4. initialize all the device driver structures 

  @param[in]
  None

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi14_driver_init(void)
{
  ADSPResult   result = ADSP_EOK;

  memset(&hdmi14_driver, 0, sizeof(hdmi14_drv_state_t));
  hdmi14_dev_cfg_struct_t *hdmi14_hw_info_ptr = &hdmi14_driver.hdmi14_hw_info;

  // 1. read hdmi14 hwd dev config and cache it to the hdmi14 driver global state
  if(ADSP_FAILED(result = afe_hdmi14_driver_device_cfg_init(hdmi14_hw_info_ptr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error afe_hdmi14_driver_device_cfg_init");
    return result;
  }

  // hdmi14 register interface
  if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&hdmi14_driver.hdmi14_qmem_region,
                                                   hdmi14_hw_info_ptr->hdmi14_reg_base, hdmi14_hw_info_ptr->hdmi14_reg_size, &hdmi14_driver.hdmi14_virt_addr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for hdmi14 register base");
    return result;
  }
  else
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hdmi14 virtual addr 0x%x phy addr 0x%x",
          (unsigned int)hdmi14_driver.hdmi14_virt_addr, (unsigned int)hdmi14_hw_info_ptr->hdmi14_reg_base);
  }


  // 3. do the hal init which save the physical address and virtual address offset.
  hal_hdmi14_init(hdmi14_hw_info_ptr->hw_revision, hdmi14_hw_info_ptr->hdmi14_reg_size, hdmi14_driver.hdmi14_virt_addr);

  //4. initialize all the device driver structures

  hdmi14_driver.driver_state = HDMI14_DRIVER_INIT;

  return ADSP_EOK;
}

/**
  @brief Perform hdmi14 Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi14_driver_deinit(void)
{

  if(HDMI14_DRIVER_INIT != hdmi14_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  /* release all resource allocated during init*/
  HwdDevCfg_DestoryVirtAddress(hdmi14_driver.hdmi14_qmem_region);


  /* release all resource allocated during init*/
  HwdDevCfg_DestoryVirtAddress(hdmi14_driver.hdmi14_dma_qmem_region);

  // change the driver state.
  hdmi14_driver.driver_state = HDMI14_DRIVER_DEINIT;

  return ADSP_EOK;
}

/**
  @brief Perform hdmi14 port initialization
               The driver port init function does these per port.
                   init the dev port state, afe service port structures, and service global state

                  1.  save the afe service port structure pointer to service global state aud_stat_afe_svc_t

                  2. service port structure
                       call afe_svc_init_port() per port which fill the basic information and default values.
                       set the avt drvier pointer which is provided from driver side.
                       set the data handling function pointer (afe_dev_process).
                       save the avtimer latch type
                       save the handle of these device state per port to the port structure                    

                  3.  save the interface enum and direction 

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi14_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
  ADSPResult      result;
  afe_dev_port    *dev_port_ptr = NULL;
  uint16_t        port_id;
  afe_dev_port_proc_func afe_dev_process = NULL;
  uint32_t dir;
  hdmi14_port_state_t *p_dev_state = NULL;
  afe_driver_func_tbl_t *ftable_ptr;

  if(HDMI14_DRIVER_INIT != hdmi14_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  for (port_id = AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START; port_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END; port_id++)
  {
    switch (port_id)
    {
      case AFE_PORT_ID_MULTICHAN_HDMI_RX:
      {
        dev_port_ptr = &port_rx_hdmi14;
        p_dev_state = &hdmi14_rx;
        ftable_ptr = &sink_ftbl;
        dir = HDMI14_SINK;
      }
      break;
      default:
      {
        // Audio Port ID is not hdmi14 and skipped
        dev_port_ptr = NULL;
        afe_dev_process = NULL;
      }
    }

    if (NULL != dev_port_ptr)
    {
      dir = port_id&AFE_PORT_DIR_MASK;
      if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create hdmi14 port = %d, aborting", port_id);
        return result;
      }
      memset(p_dev_state, 0, sizeof(hdmi14_port_state_t));

      // As the hdmi14 port id can not changed without HLOS approval, these port pointer arrary can not be changed at this moment.
      afe_svc_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)] = dev_port_ptr;

      dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = &p_dev_state->hdmi14_avt_drift;
      dev_port_ptr->afe_dev_process = afe_dev_process;
      dev_port_ptr->afe_drv_ftbl = ftable_ptr;
      dev_port_ptr->avt_drift_params.avtimer_latch_type = TIMESTAMP_LATCH_TYPE_HARDWARE;

      // This can be assinged during open once afe introduce the open and close command for AFE port.
      dev_port_ptr->afe_drv = (void *) p_dev_state;

      // afe device state per port...
      p_dev_state->direction = dir;

    }
  }

  return ADSP_EOK;
}



/**
  @brief Perform hdmi14 port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi14_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
  // check the driver state
  if(HDMI14_DRIVER_INIT != hdmi14_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  // free resources if there is anything is allocated during hdmi14 port init.
  afe_dev_port    *dev_port_ptr = NULL;

  //  The port id is not straitforward because the port id are mixed up for hdmi14,
  // initialize slimbus related ports
  for (uint16_t port_id = AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START; port_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END; port_id++)
  {
    switch (port_id)
    {
      case AFE_PORT_ID_MULTICHAN_HDMI_RX:
        dev_port_ptr = afe_svc_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)];
        break;
      default:
      {
        // Audio Port ID is not hdmi14 and skipped
        dev_port_ptr = NULL;
      }
    }

    if(NULL != dev_port_ptr)
    {
      afe_svc_deinit_port(dev_port_ptr);
    }
  }

  return ADSP_EOK;
}


/**
  @brief Set hdmi14 port config
               shoudn't pass any of afe_dev_port_t structure type from this point to make driver independent from service layer.

  @param[in] afe_drv pointer for AFE dev port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;

  // check the driver state
  if(HDMI14_DRIVER_INIT != hdmi14_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  if((NULL == afe_drv) || (NULL == cfg_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle / params pointer is null");
    return ADSP_EFAILED;
  }

  switch(param_id)
  {
    case AFE_PARAM_ID_HDMI_CONFIG:
    {
      // choose the appropriate config version
      uint32_t hdmi14_cfg_version = (uint32_t) *(cfg_ptr);
      if(hdmi14_cfg_version > AFE_API_VERSION_HDMI_CONFIG)
      {
        hdmi14_cfg_version = AFE_API_VERSION_HDMI_CONFIG;
      }

      switch (hdmi14_cfg_version)
      {
        case AFE_API_VERSION_HDMI_CONFIG:
        {
          afe_param_id_hdmi_multi_chan_audio_cfg_t *hdmi14_cfg_ptr =  (afe_param_id_hdmi_multi_chan_audio_cfg_t *)cfg_ptr;

          if (payload_size < sizeof(afe_param_id_hdmi_multi_chan_audio_cfg_t))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI14 Cfg fail: Invalid payload size: %ld, hdmi14", \
                  payload_size);
            result = ADSP_EBADPARAM;
          }
          else
          {
            result = afe_hdmi14_dev_config_port(p_dev_state, hdmi14_cfg_ptr);
          }
          break;
        }
        default:
          return ADSP_EUNSUPPORTED;
      }
      break;
    }
    default:
      return ADSP_EUNSUPPORTED;
  }

  return result;
}


/**
  @brief Get hdmi14 port config

  @param[in] afe_drv pointer for afe device port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
  ADSPResult result = ADSP_EOK;
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;

  if((NULL == afe_drv) || (NULL == param_buf_ptr) ||  (NULL == params_buffer_len_req_ptr) )
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / params pointers are null");
    return ADSP_EFAILED;
  }

  switch(param_id)
  {
    case AFE_PARAM_ID_HDMI_CONFIG:
    {
      switch (p_dev_state->dev_cfg_minor_ver)
      {
        case AFE_API_VERSION_HDMI_CONFIG:
        {
          *params_buffer_len_req_ptr = sizeof(afe_param_id_hdmi_multi_chan_audio_cfg_t);
          if(*params_buffer_len_req_ptr > payload_size )
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: hdmi14 cfg required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, payload_size);
            return ADSP_ENOMEMORY;
          }
          afe_param_id_hdmi_multi_chan_audio_cfg_t  *hdmi14_cfg_ptr = (afe_param_id_hdmi_multi_chan_audio_cfg_t *)param_buf_ptr;

          /* Populate the payload parameters */
          hdmi14_cfg_ptr->hdmi_cfg_minor_version = p_dev_state->dev_cfg_minor_ver;
          hdmi14_cfg_ptr->bit_width    = p_dev_state->bit_width;
          hdmi14_cfg_ptr->sample_rate  = p_dev_state->sample_rate;
          hdmi14_cfg_ptr->channel_allocation = p_dev_state->hdmi_ca;
          hdmi14_cfg_ptr->data_type = p_dev_state->data_format;
          hdmi14_cfg_ptr->reserved = 0;
        }
        break;
        default:
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 cfg Get Cmd Cfg fail: InCompatible cfg version: %ld",
                p_dev_state->dev_cfg_minor_ver);
          return ADSP_EBADPARAM;
        }
      }

      break;
    }
    default:
      return ADSP_EUNSUPPORTED;
  }

  return result;
}


/**
  @brief Set up the data transfer for hdmi14 port
               1. check if any shared resource is already is used. mux, data lines,...
               2. open dma manger with callback function
               3. config hdmi14 interface
               4. start the dma
               5. enable the hdmi14 interface

  @param[in] afe_drv pointer for afe device port structure
  @param[in]  hdmi14_open_params_ptr for open parameters

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_driver_start(void *afe_drv, dev_start_params_t *hdmi14_start_params_ptr)
{
  ADSPResult result = ADSP_EOK;
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;
  dma_mgr_open_params_t open_params;


  if((NULL == afe_drv) || (NULL == hdmi14_start_params_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle / Open Params Null");
    return ADSP_EFAILED;
  }

  /* Set the block transfer time */
  // this will be determined during the beginning of start cmd and after the config cmd
  p_dev_state->int_samples_per_period = hdmi14_start_params_ptr->int_samples_per_period;

  // callback info for interrupt handling
  p_dev_state->cb_info.afe_dev_int_cb = hdmi14_start_params_ptr->cb_info.afe_dev_int_cb;
  p_dev_state->cb_info.cb_arg = hdmi14_start_params_ptr->cb_info.cb_arg;

  // dma manager open
  memset(&open_params, 0, sizeof(dma_mgr_open_params_t));

  // hardware type and dma type
  open_params.hardware_type = LPASS_HW_HDMI14;
  open_params.dma_type = LPASS_HW_DMA_TYPE0;

  //  open dma manger with callback function for read or write
  open_params.dma_dir = LPASS_HW_DMA_SINK;
  open_params.dma_write_func_ptr = (dma_mgr_write_func)afe_hdmi14_write_dma;
  open_params.sample_rate = p_dev_state->sample_rate;
  open_params.int_samples_per_period = p_dev_state->int_samples_per_period;
  open_params.input_bytes_per_sample =  p_dev_state->dma_input_bytes_per_sample;
  //need to be revisted...
  open_params.output_bytes_per_sample =  p_dev_state->bytes_per_channel;
  open_params.num_channels = p_dev_state->num_channels;

  // if the number of channel is bigger than 2, the buffer should be allocated for 8 channels data
  // Anything greater than 2 channels uses HDMI Layout 1
  if(2 < open_params.num_channels)
  {
    open_params.num_channels = 8;
  }

  // dma buffer type
  /* If LPM is exhausted, then allocate DMA buffer in DDR */
  open_params.buff_mem_type = DMA_MGR_LPM_DDR_BUFFER;

  // 4 kinds of format is possible here. combination of linear/non-linear  packed/non-packed
  open_params.data_type = p_dev_state->data_format;

  // register the interrupt callback
  open_params.dma_int_cb = afe_hdmi14_int_callback;
  open_params.afe_driver_context = afe_drv;

  // this info will be passing down to dma hal and it is only for audioif cases including hdmi14, .
  // This value should be matching to audioif dma hal config function
  open_params.ifconfig_dma_control  = hal_hdmi14_get_intf();

  // get the watermark value. if watermark is 1, then dma burst mode is supposed to be off.
  open_params.watermark = afe_hdmi14_get_watermark(p_dev_state);

  // dma buffer size need to be checked carefully based on alignment requirement...
  open_params.num_buffers = afe_hdmi14_get_num_dma_buffer(p_dev_state);

  if(ADSP_FAILED(result = afe_dma_mgr_open(&p_dev_state->dma_dev_hdl, &open_params)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register with DMA Manager with status %d",result);
    return result;
  }

  //allocate and init memory  channel status table for 60958 format packing
  if(ADSP_FAILED(result = afe_hdmi14_init_IEC60958_ch_status_table(p_dev_state)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The hdmi14 ch status table init has failed");
    return result;
  }

  hal_hdmi14_disable_core();

  //start the DMA engine
  if(ADSP_FAILED(result = afe_dma_mgr_start(p_dev_state->dma_dev_hdl)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d",result);
    afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
    p_dev_state->dma_dev_hdl = NULL;
    return result;
  }

  // Enabling the interrupts will be done in the dma manager

  //enable the inteface
  if(2 < p_dev_state->num_channels)
  {
    /* Configure SAMPLE_PRESENT bits for LAYOUT 1  */
    uint32_t present_bits =  afe_hdmi14_get_present_bits(p_dev_state->hdmi_ca);

    hal_hdmi14_sample_present_bit_layout1(present_bits);

    if(AFE_NON_LINEAR_DATA == p_dev_state->data_format)
    {
      /* For non-linear data and channels greater than two use case,
       * configure High Bit rate enable bit*/
      hal_hdmi14_enable_disable_hbr(ENABLE);
    }
  }
  else
  {
    /* for 2 channel case, reset&disable the present bits and */
    hal_hdmi14_reset_sample_present_bit();
  }

#ifndef SIM // hdmi audio hw payload packetizing setup and enable

  hal_hdmi14_set_pkt_ctl_cs_60958_source_hwd();
  hal_hdmi14_set_pkt_ctl_channel_enable_layout1();

  uint32_t *ch_status_table_ptr = p_dev_state->IEC60958_ch_status_table_ptr; //pAfeDevAudioChInfo->HDMIIfConfigData.pHdmiChStatusTable;
  uint32_t validity_bit = 0;
  if(AFE_NON_LINEAR_DATA == p_dev_state->data_format)
  {
    validity_bit = 1;   //set validity bit for non L-PCM stream
  }
  hal_hdmi14_set_packet_info(ch_status_table_ptr, validity_bit);

  hal_hdmi14_set_pkt_ctl_cs_60958_update();

  hal_hdmi14_enable_hw_pack();
#endif

  hal_hdmi14_enable_core();
  hal_hdmi14_enable_packet_ctl();


  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hdm14 port successfully opened");

  return result;
}


/**
  @brief Stop the data transfer from hdmi14 port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_driver_stop(void *afe_drv)
{
  ADSPResult result = ADSP_EOK;
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;

  if(NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle Null");
    return ADSP_EFAILED;
  }

  // disable and clear dma interrupt
  afe_dma_mgr_disable_clear_dma_int(p_dev_state->dma_dev_hdl);

#ifndef SIM 
  //stop audio packet transmission
  hal_hdmi14_disable_packet_ctl();
#endif

  // Disable HBR
  if( (AFE_NON_LINEAR_DATA == p_dev_state->data_format) &&
      (2 < p_dev_state->num_channels) )
  {
    hal_hdmi14_enable_disable_hbr(DISABLE);
  }

  // Disable HDMI Audio engine
  hal_hdmi14_disable_core();

  // Disable HW packetizing
  hal_hdmi14_disable_hw_pack();

  //deallocate and deinit channel status table for 60958 format packing
  (void) afe_hdmi14_deinit_IEC60958_ch_status_table(p_dev_state);

  //stop the DMA engine
  afe_dma_mgr_stop(p_dev_state->dma_dev_hdl);

  //De-register with the DMA Manager including the isr callback
  afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
  p_dev_state->dma_dev_hdl = NULL;

  return result;
}


/**
  @brief Copy data from client buffer to the hdmi14 DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_driver_write(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
  ADSPResult result = ADSP_EOK;
  int16_t           num_channels, samp;
  uint32_t          total_num_samples;
  int32_t           *ptr_pBuf32;
  int64_t           *ptr_dst_64;
  uint16_t        	q_format_shift_factor;

  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;

  if((NULL == afe_drv) || (NULL == buffer_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle / buffer pointer Null");
    return ADSP_EFAILED;
  }

  // pre processing is required for the cases as below.
  // bit width,  number of channels
  num_channels = p_dev_state->num_channels;
  q_format_shift_factor = p_dev_state->q_format_shift_factor;

  /* Calculate the total number of samples*/
  total_num_samples = (num_dma_samples * num_channels);

  if(4 == p_dev_state->bytes_per_channel) /* For Non-I2S interface, Only Q31->Q28 scaling, No swap */
  {
    ptr_dst_64 = (int64_t *)buffer_ptr;

#ifdef __qdsp6__
    for(samp=total_num_samples; samp >= 2; samp -= 2)
    {
      /* Q31 to Q28 conversion, two samples at at time
       */
      (*ptr_dst_64) = Q6_P_vaslw_PR(*ptr_dst_64, q_format_shift_factor);
      ptr_dst_64++;
    }

    /* If number of samples are odd, scale the remaining sample
     */
    if(samp)
    {
      ptr_pBuf32 = (int32_t *)ptr_dst_64;

      (*ptr_pBuf32) <<= q_format_shift_factor;
    }
#else
    /*-------- Non Q6 Version ------*/

    ptr_pBuf32 = (int32_t *)buffer_ptr;

    for(int32_t i=0; i<total_num_samples; i++)
    {
      ptr_pBuf32[i] = (ptr_pBuf32[i] << q_format_shift_factor);
    }
#endif /* __qdsp6__ */
  }


  if(ADSP_FAILED(result = afe_dma_mgr_write(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
  }

  return result;
}

/**
  @brief Routine to read the properites for hdmi14 

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  //hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;


  if(NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle Null");
    return ADSP_EFAILED;
  }

  switch(prop_id)
  {
    case AFE_DEVICE_PORT_PROP:
    {
      afe_dev_port_config_t  *hdmi14_port_cfg_data_ptr = (afe_dev_port_config_t  *)prop_ptr;

      if (payload_size < sizeof(afe_dev_port_config_t))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 get property fail: Invalid payload size: %ld", payload_size);
        result = ADSP_EBADPARAM;
      }

      result = afe_hdmi14_get_dev_port_cfg(afe_drv, hdmi14_port_cfg_data_ptr);
      break;
    }
    case AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP:
    {
      afe_dev_timestamp_info_t  *time_stamp_info_ptr = (afe_dev_timestamp_info_t *) prop_ptr;

      if (payload_size < sizeof(afe_dev_timestamp_info_t))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM get property fail: Invalid payload size: %ld", payload_size);
        result = ADSP_EBADPARAM;
      }

      result = afe_hdmi14_get_dma_timestamp(afe_drv, time_stamp_info_ptr);
      break;
    }
    default:
      result = ADSP_EUNSUPPORTED;
      break;
  }

  return result;


}   

/**
  @brief Get hdmi14 dev port config to fill the info to the port structure.
        NOTE: This is the new function to remove port structure access inside device driver right after config.
                   Upper layer should call this function to fill the port structure.

  @param[in] afe_drv pointer for afe device port structure
  @param[out] hdmi14_port_cfg_data_ptr pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *hdmi14_port_cfg_data_ptr)
{
  ADSPResult result = ADSP_EOK;
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;

  if(HDMI14_DRIVER_INIT != hdmi14_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  if((NULL == afe_drv) || (NULL == hdmi14_port_cfg_data_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle / params pointer is null");
    return ADSP_EFAILED;
  }

  // this part is port structure which we'd better access this in the driver.
  // we can have new function such as get function which provide information related to this.

  // port data sturtcure to be filled.
  hdmi14_port_cfg_data_ptr->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
  if(AFE_NON_LINEAR_DATA == p_dev_state->data_format)
  {
      hdmi14_port_cfg_data_ptr->port_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
  }

  hdmi14_port_cfg_data_ptr->q_format_shift_factor  = p_dev_state->q_format_shift_factor;
  hdmi14_port_cfg_data_ptr->bytes_per_channel =  p_dev_state->bytes_per_channel;
  hdmi14_port_cfg_data_ptr->bit_width = p_dev_state->bit_width;
  hdmi14_port_cfg_data_ptr->channels = p_dev_state->num_channels;
  hdmi14_port_cfg_data_ptr->sample_rate = p_dev_state->sample_rate;
  hdmi14_port_cfg_data_ptr->is_interleaved = TRUE;

  return result;
}



/**
  @brief Routine to read the hardware timestamp registers for hdmi14

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr)
{
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_drv;
  uint64_t dma_int_timestamp = 0;
  uint32_t fifo_count = 0;
  ADSPResult result = ADSP_EOK;

  if(NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to hdmi14 device handle Null");
    return ADSP_EFAILED;
  }

  if(ADSP_FAILED(result = afe_dma_mgr_read_timestamp_fifo_count(p_dev_state->dma_dev_hdl, &dma_int_timestamp, &fifo_count)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 afe_dma_mgr_read_timestamp_fifo_count failed");
  }
  else
  {

    timestamp_info_ptr->dma_int_timestamp  = avtimer_drv_convert_hw_tick_to_time(dma_int_timestamp);

    /* FIFO count is in 32 bit words. Convert it to samples as per port byter per sample */
    timestamp_info_ptr->fifo_samples = fifo_count * sizeof(uint32_t)/p_dev_state->bytes_per_channel;
  }

  return result;
}

/**
  @brief Callback routine for the hdmi14 interrupt

  @param[in] afe_drv pointer to the hdmi14 dev port state structure

  @param[in] event_mask indicates if it is dma interrupt or error.

  @return  none

 */
static void afe_hdmi14_int_callback(void* afe_drv, uint32 event_mask)
{
  hdmi14_port_state_t         *dev_state_ptr = (hdmi14_port_state_t *)afe_drv;;

  if(NULL == afe_drv)
  {
    // critical error
    return;
  }

  if(event_mask & DMA_TYPE2_EVENT_FIFOERROR)
  {
    dev_state_ptr->fifo_ovf_cnt++;
  }

  if(event_mask & DMA_TYPE2_EVENT_BUSERROR)
  {
    dev_state_ptr->ahb_err_cnt++;
  }

  if(event_mask & DMA_TYPE2_EVENT_DATA)
  {
    if(NULL != dev_state_ptr->cb_info.afe_dev_int_cb)
    {
      dev_state_ptr->hdmi14_intr_cnt++;
      dev_state_ptr->cb_info.afe_dev_int_cb(dev_state_ptr->cb_info.cb_arg);
    }
  }
}


/**
  Copy data from client buffer to the hdmi14 Output DMA buffer
  write function which is called by dma manager

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address                    pointer to the client buffer
  @param[in]  dst_address                    pointer to the DMA buffer

  @return
  None

  @dependencies
  None.
 */

static void afe_hdmi14_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address)
{
  hdmi14_port_state_t *p_dev_state = (hdmi14_port_state_t *) afe_driver_context;

  if((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address)  )
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
    return;
  }

  //IEC60958 pack and copy data
  afe_hdmi14_copy_IEC60958_data(p_dev_state, (uint16_t *)src_address, dst_address);

  return;
}


/**
  @brief save hdmi14 port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] hdmi14_cfg_ptr   pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error
 */
static ADSPResult afe_hdmi14_dev_config_port (hdmi14_port_state_t* p_dev_state, afe_param_id_hdmi_multi_chan_audio_cfg_t *hdmi14_cfg_ptr)
{
  ADSPResult result = ADSP_EOK;

  // Printing the device configuration information
  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"Device configuration for HDMI14 Port");
  MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"data type: %d Channel Allocation: %d Sample rate: %lu",hdmi14_cfg_ptr->data_type, hdmi14_cfg_ptr->channel_allocation, hdmi14_cfg_ptr->sample_rate);
  MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"bit width: %d hdmi cfg minor version: %lu",hdmi14_cfg_ptr->bit_width,hdmi14_cfg_ptr->hdmi_cfg_minor_version);

  // parse channel allocation info
  if (HDMI14_MAX_CA_VALUE < hdmi14_cfg_ptr->channel_allocation )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 Un-supported Channel Allocation: %u", hdmi14_cfg_ptr->channel_allocation);
    return ADSP_EUNSUPPORTED;
  }
  p_dev_state->hdmi_ca = hdmi14_cfg_ptr->channel_allocation;

  // derive number of channels from CA value
  p_dev_state->num_channels = hdmi14_channel_alloc_array[p_dev_state->hdmi_ca][8];

  // parse bit width information
  if ((16 != hdmi14_cfg_ptr->bit_width) && (24 != hdmi14_cfg_ptr->bit_width))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x", hdmi14_cfg_ptr->bit_width);
    return ADSP_EUNSUPPORTED;
  }
  p_dev_state->bit_width = hdmi14_cfg_ptr->bit_width;

  p_dev_state->bytes_per_channel = (p_dev_state->bit_width > 16) ? 4 : 2;

  p_dev_state->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;

  // parse data format info
  if ((AFE_LINEAR_PCM_DATA != hdmi14_cfg_ptr->data_type)&&
      (AFE_NON_LINEAR_DATA != hdmi14_cfg_ptr->data_type))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 Un-supported data type: %u", hdmi14_cfg_ptr->data_type);
    return ADSP_EUNSUPPORTED;
  }

  p_dev_state->data_format = hdmi14_cfg_ptr->data_type;

  // parse the sample rate
  if ((AFE_PORT_SAMPLE_RATE_8K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_16K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_32K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_48K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_96K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_88_2K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_192K != hdmi14_cfg_ptr->sample_rate)&&
      (AFE_PORT_SAMPLE_RATE_22_05K  != hdmi14_cfg_ptr->sample_rate)&& // compresseed scenarios only
      (AFE_PORT_SAMPLE_RATE_44_1K  != hdmi14_cfg_ptr->sample_rate)&& // compresseed scenarios only, Tx in non compressed
      (AFE_PORT_SAMPLE_RATE_176_4K != hdmi14_cfg_ptr->sample_rate)) // compresseed scenarios only
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu", hdmi14_cfg_ptr->sample_rate);
    return ADSP_EUNSUPPORTED;
  }

  p_dev_state->sample_rate = hdmi14_cfg_ptr->sample_rate;

  p_dev_state->dma_input_bytes_per_sample =  HDMI14_PACKED_60958_BYTES_PER_SAMPLES;

  return result;
}


/**
  @brief calculate the number of dma buffer.
            The default value is 2 as ping-pong buffer.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  the number of buffer (0 in error case)

 */
static uint32 afe_hdmi14_get_num_dma_buffer(hdmi14_port_state_t *p_dev_state)
{
  uint32 num_dma_buffer = DEFAULT_HDMI14_DMA_BUFFERS;
  uint32 dma_buf_size_in_bytes_per_intr;

  dma_buf_size_in_bytes_per_intr = p_dev_state->dma_input_bytes_per_sample *  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;


  if(dma_buf_size_in_bytes_per_intr & HDMI14_DMA_BUFFER_16BYTE_ALIGNMENT)
  {
    // change it to 4 buffer scheme to meet the 16 byte alignment
    num_dma_buffer = 4;
    dma_buf_size_in_bytes_per_intr = num_dma_buffer * dma_buf_size_in_bytes_per_intr;

    if(dma_buf_size_in_bytes_per_intr & HDMI14_DMA_BUFFER_16BYTE_ALIGNMENT)
    {
      num_dma_buffer = 0;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "dma total buffer length should be alined to 4 32bit words %d", dma_buf_size_in_bytes_per_intr);
    }
  }

  return num_dma_buffer;

}


/**
  @brief calculate the watermark value.
            The default value is 8 dwords.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  watermark value which is 1 or 8
 */
static uint32 afe_hdmi14_get_watermark(hdmi14_port_state_t *p_dev_state)
{
  uint32 water_mark = DEAFULT_HDMI14_WATERMARK;
  uint32 dma_buf_size_in_bytes_per_intr;
  uint32 dma_buf_size_in_dword_per_intr;

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   */
  dma_buf_size_in_bytes_per_intr = p_dev_state->dma_input_bytes_per_sample*  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;

  dma_buf_size_in_dword_per_intr = dma_buf_size_in_bytes_per_intr >> 2;

  //8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
  //The burst mode needs to be disabled for this case for latency and drift detection issue
  if((DEAFULT_HDMI14_WATERMARK >= dma_buf_size_in_dword_per_intr) ||
      (dma_buf_size_in_dword_per_intr & 0x3)  )
  {
    water_mark = 1;
  }

  return water_mark;
}



/**
  @brief Obtain the chip specific information for initializing the driver

  @param[in] hdmi14_hw_info_ptr pointer for hdmi14 Device config structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_hdmi14_driver_device_cfg_init(hdmi14_dev_cfg_struct_t * hdmi14_hw_info_ptr)
{
  DALSYSPropertyVar prop_var;
  lpasshwio_prop_lpaif_struct_t  *lpaif_prop_dev_cfg_ptr = NULL;
  const char *dev_id="LPAIF";
  //uint32 i;

  DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

  /* Get device handle */
  if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl) )
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for hdmi14");
    return ADSP_EFAILED;
  }
  else
  {
    /* Read property ptr */
    if( DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "LpaifPropStructPtr", 0, &prop_var) )
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting hdmi14 Property Struct Ptr");
      return ADSP_EFAILED;
    }
    else
    {
      lpaif_prop_dev_cfg_ptr = (lpasshwio_prop_lpaif_struct_t *)prop_var.Val.pStruct;

      if (NOT_SUPPORTED == lpaif_prop_dev_cfg_ptr->hdmi_prop.is_supported)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 interface is not supported");
        return ADSP_EUNSUPPORTED;
      }

      hdmi14_hw_info_ptr->hw_revision = lpaif_prop_dev_cfg_ptr->hw_revision;

      /* HDMI register base address */
      hdmi14_hw_info_ptr->hdmi14_reg_base = lpaif_prop_dev_cfg_ptr->hdmi_prop.hdmi_reg_base;
      /* HDMI register page size */
      hdmi14_hw_info_ptr->hdmi14_reg_size = lpaif_prop_dev_cfg_ptr->hdmi_prop.hdmi_reg_size;
    }
  }

  return ADSP_EOK;
}


