/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/usba/src/AFEUSBAudioDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEUSBAudioDriver.cpp

DESCRIPTION: Main Interface to the AFE USB Audio driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/usba/src/AFEUSBAudioDriver.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
02-22-2016  mk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEUSBAudioDriver_i.h"
/*=====================================================================
 Globals
 ======================================================================*/

// AFE USB Audio Driver instance
static usba_driver_state_t  g_usba_driver_state;
static afe_dev_port_t  afe_usba_ports_global[AFE_PORT_ID_USB_AUDIO_RANGE_SIZE] __attribute__((__section__(".bss.lpa_audio_static_svc_afe")));


/*=====================================================================
 Static Functions
 ======================================================================*/

/*=====================================================================
 Define Functions
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/
void afe_usb_data_callback(void *cbCtxt)
{
  afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)cbCtxt;

  if(AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
  {
    // signal to the port data thread about the finishing of the data transfer
    afe_port_ptr->dynamic_thread_ptr->isr_context_counter++;
    qurt_elite_signal_send(&afe_port_ptr->dma_signal);
  }
}


/**
 * Perform USB audio port initialization 
 *
 * @param[in] afe_svc_ptr pointer for AFE static service 
 *  
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
  ADSPResult                  result;
  uint16_t                    port_id;
  afe_dev_port                *afe_port_ptr = NULL;

  // Global initialization
  memset((char *) &afe_usba_ports_global[0],0, sizeof(afe_usba_ports_global));

  // initialize USB audio related ports
  for (port_id = AFE_PORT_ID_USB_AUDIO_RANGE_START; port_id <= AFE_PORT_ID_USB_AUDIO_RANGE_END; port_id++)
  {
    afe_port_ptr = &afe_usba_ports_global[GET_USB_AUDIO_PORT_INDEX(port_id)];
    afe_svc_ptr->ports_usba_ptr[GET_USB_AUDIO_PORT_INDEX(port_id)] = afe_port_ptr;

    if (ADSP_FAILED(result = afe_svc_init_port(afe_port_ptr, port_id, (port_id&AFE_PORT_DIR_MASK))))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to init port: %d, aborting\n", port_id);
      return result;
    }

    if (IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
    {
      afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_usba_read;
    }
    else
    {
      afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_usba_write;
    }
  }

  return ADSP_EOK;
}

/**
  @brief Perform USB audio port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_usba_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
  uint16_t        port_id;

  // de-initialize USB audio related ports
  for (port_id = AFE_PORT_ID_USB_AUDIO_RANGE_START; port_id <= AFE_PORT_ID_USB_AUDIO_RANGE_END; port_id++)
  {
    afe_svc_deinit_port(afe_svc_ptr->ports_usba_ptr[GET_USB_AUDIO_PORT_INDEX(port_id)]);
  }

  return ADSP_EOK;
}


/**
 * Perform one time initialization of the AFE USB audio driver
 *
 * @return  ADSP_EOK on success, an error code on faliure
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_driver_init(void)
{

  g_usba_driver_state.is_initialized = TRUE;

  return ADSP_EOK;
}

/**
 * Perform de-initialization of the AFE USB audio driver
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_driver_deinit(void)
{
  ADSPResult result = ADSP_EOK;
  g_usba_driver_state.is_initialized = FALSE;

  return result;
}


/**
  @brief Perform configuration of USB Audio device for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_cfg pointer to AFE port USB audio device config
        strcutre
  @param[in] cfg_payload_size config struct payload size
  @param[in] cfg_ver -> config version

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_usba_cfg (afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, uint32_t param_id)
{
  ADSPResult result = ADSP_EOK;

  /* If this is the first set cfg for the interface, allocate the driver state */
  if(NULL == afe_port_ptr->afe_drv)
  {
    afe_usba_dev_state_struct_t *dev_state_ptr = NULL;

    if(NULL
        == (dev_state_ptr = (afe_usba_dev_state_struct_t *)qurt_elite_memory_malloc(
            sizeof(afe_usba_dev_state_struct_t), QURT_ELITE_HEAP_DEFAULT)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed, cannot allocate driver state memory for port: 0x%lx!", afe_port_ptr->intf);
      result = ADSP_ENOMEMORY;
      return result;
    }
    else
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "USB audio driver alloc successfully at 0x%lx for port: 0x%lx!",
            dev_state_ptr, afe_port_ptr->intf);
    }

    // do the mem clean of the dev-state struct
    memset((char *) dev_state_ptr, 0, sizeof(afe_usba_dev_state_struct_t));


    afe_port_ptr->afe_drv = (void *)dev_state_ptr;

  }

  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;
  afe_usba_dev_cfg_info_t *cfg_ptr = &p_dev_state->afe_usba_dev_cfg;


  switch(param_id)
  {
    case AFE_PARAM_ID_USB_AUDIO_CONFIG:
    {
      // choose the appropriate config version
      uint32_t cfg_ver = (uint32_t) * (p_cfg);
      if(cfg_ver > AFE_API_VERSION_USB_AUDIO_CONFIG)
      {
        cfg_ver = AFE_API_VERSION_USB_AUDIO_CONFIG;
      }

      cfg_ptr->cfg_version = cfg_ver;
      switch(cfg_ver)
      {
        case 1:
        {
          afe_param_id_usb_audio_cfg_v1_t *p_usba_cfg = (afe_param_id_usb_audio_cfg_v1_t *)p_cfg;
          if(cfg_payload_size < sizeof(afe_param_id_usb_audio_cfg_v1_t))
          {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB audio Config Fails, Bad Param");
            return ADSP_EBADPARAM;
          }

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE USB audio port configuration for port 0x%x "
                "data_format %d", afe_port_ptr->intf, p_usba_cfg->data_format);

          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sample_rate %ld bit_width %d num_channels %d",
                p_usba_cfg->sample_rate, p_usba_cfg->bit_width, p_usba_cfg->num_channels);

          if((AFE_BITS_PER_SAMPLE_16 != p_usba_cfg->bit_width) &&
              (AFE_BITS_PER_SAMPLE_24 != p_usba_cfg->bit_width) &&
              (AFE_BITS_PER_SAMPLE_32 != p_usba_cfg->bit_width))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit_width: %d for port 0x%x", p_usba_cfg->bit_width, afe_port_ptr->intf);
            return ADSP_EUNSUPPORTED;
          }

          if((p_usba_cfg->num_channels < 1) || (p_usba_cfg->num_channels > AFE_PORT_MAX_AUDIO_CHAN_CNT))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %d for port 0x%x", p_usba_cfg->num_channels, afe_port_ptr->intf);
            return ADSP_EUNSUPPORTED;
          }

          if((AFE_PORT_SAMPLE_RATE_8K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_11_025K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_12K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_16K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_22_05K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_24K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_32K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_44_1K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_48K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_88_2K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_96K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_176_4K != p_usba_cfg->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_192K != p_usba_cfg->sample_rate)&&
              (AFE_PORT_SAMPLE_RATE_352_8K != p_usba_cfg->sample_rate)&&
              (AFE_PORT_SAMPLE_RATE_384K != p_usba_cfg->sample_rate))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, for port: 0x%x", p_usba_cfg->sample_rate, afe_port_ptr->intf);
            return ADSP_EUNSUPPORTED;
          }

          if((AFE_USB_AUDIO_DATA_FORMAT_LINEAR_PCM != p_usba_cfg->data_format))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported data_format: %lu, for port: 0x%x", p_usba_cfg->data_format, afe_port_ptr->intf);
            return ADSP_EUNSUPPORTED;
          }

          /* Determining the qformat shift factor based on bit width. This shift factor is being used for 24bit and 32bit only*/
          if(AFE_BITS_PER_SAMPLE_24 == p_usba_cfg->bit_width)
          {
            afe_port_ptr->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
          }
          else
          {
            afe_port_ptr->q_format_shift_factor = 0;
          }

          cfg_ptr->bit_width = p_usba_cfg->bit_width;
          cfg_ptr->num_channels = p_usba_cfg->num_channels;
          cfg_ptr->data_format = p_usba_cfg->data_format;
          cfg_ptr->sample_rate = p_usba_cfg->sample_rate;
        }
        break;

        default:
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio device Config Fails, Bad Param");
          return ADSP_EUNSUPPORTED;
      }

      cfg_ptr->bytes_per_channel = (cfg_ptr->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
      p_dev_state->afe_port_ptr = afe_port_ptr;

      // store the device state information in port structure for future needs
      afe_port_ptr->dev_data_type = AFE_UNCOMPRESSED_DATA_PORT;
      afe_port_ptr->dev_bit_width = cfg_ptr->bit_width;
      afe_port_ptr->dev_channels = p_dev_state->afe_usba_dev_cfg.num_channels;
      afe_port_ptr->dev_bytes_per_channel = p_dev_state->afe_usba_dev_cfg.bytes_per_channel;
      afe_port_ptr->dev_sample_rate = p_dev_state->afe_usba_dev_cfg.sample_rate;

      //if applicable, overwrite the port media type with the device media type.
      //this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
      afe_port_copy_media_type_dev_to_port(afe_port_ptr);

      afe_port_ptr->is_interleaved = TRUE;
    }
    break;
    case AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS:
    {
      // choose the appropriate config version
      uint32_t cfg_ver = (uint32_t) * (p_cfg);
      if(cfg_ver > AFE_API_VERSION_USB_AUDIO_CONFIG)
      {
        cfg_ver = AFE_API_VERSION_USB_AUDIO_CONFIG;
      }

      switch(cfg_ver)
      {
        case 1:
        {
          afe_param_id_usb_audio_dev_params_v1_t *p_usba_dev_params = (afe_param_id_usb_audio_dev_params_v1_t *)p_cfg;
          if(cfg_payload_size < sizeof(afe_param_id_usb_audio_dev_params_v1_t))
          {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB audio device params Fails, Bad Param");
            return ADSP_EBADPARAM;
          }

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE USB audio port device params for port 0x%x "
                "usb_device_token 0x%lX ", afe_port_ptr->intf,
                p_usba_dev_params->usb_device_token);

          cfg_ptr->usb_device_token = p_usba_dev_params->usb_device_token;
          cfg_ptr->dev_params_version = p_usba_dev_params->minor_version;
          p_dev_state->is_usb_device_token_rcvd = TRUE;
        }
        break;
      }
    }
    break;
    case AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT:
    {
      // choose the appropriate config version
      uint32_t lpcm_fmt_ver = (uint32_t) * (p_cfg);
      if(lpcm_fmt_ver > AFE_API_VERSION_USB_AUDIO_DEV_LPCM_FMT)
      {
        lpcm_fmt_ver = AFE_API_VERSION_USB_AUDIO_DEV_LPCM_FMT;
      }

      switch(lpcm_fmt_ver)
      {
        case 1:
        {
          afe_param_id_usb_audio_dev_lpcm_fmt_v1_t *p_usba_lpcm_med_fmt = (afe_param_id_usb_audio_dev_lpcm_fmt_v1_t *)p_cfg;
          if(cfg_payload_size < sizeof(afe_param_id_usb_audio_dev_lpcm_fmt_v1_t))
          {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB audio lpcm med fmt Fails, Bad Param");
            return ADSP_EBADPARAM;
          }

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE USB audio LPCM med format 0x%lx for port 0x%x ",
                p_usba_lpcm_med_fmt->media_format, afe_port_ptr->intf);

          cfg_ptr->lpcm_med_format.media_format = p_usba_lpcm_med_fmt->media_format;
          cfg_ptr->lpcm_fmt_version = p_usba_lpcm_med_fmt->minor_version;
        }
        break;

        default:
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio device Config Fails, Bad Param");
          return ADSP_EUNSUPPORTED;
      }
    }
    break;
    default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio set device Config Fails, Bad Param");
      return ADSP_EUNSUPPORTED;

  }

  return ADSP_EOK;
}

ADSPResult afe_get_usba_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size,
                             uint16_t *params_buffer_len_req_ptr, uint32_t param_id)
{
  /* If get is called without a set cfg, return error */
  if(NULL == afe_port_ptr->afe_drv)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed USB Audio device cfg not yet set for port: 0x%x", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;


  switch(param_id)
  {
    case AFE_PARAM_ID_USB_AUDIO_CONFIG:
    {
      uint32_t cfg_ver = p_dev_state->afe_usba_dev_cfg.cfg_version;
      switch(cfg_ver)
      {
        case 1:
        {
          *params_buffer_len_req_ptr = sizeof(afe_param_id_usb_audio_cfg_v1_t);
          if(*params_buffer_len_req_ptr > payload_size)
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: USB device cfg required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, payload_size);
            return ADSP_ENOMEMORY;
          }
          afe_param_id_usb_audio_cfg_v1_t *p_usba_cfg = (afe_param_id_usb_audio_cfg_v1_t *)param_buf_ptr;

          p_usba_cfg->bit_width = p_dev_state->afe_usba_dev_cfg.bit_width;
          p_usba_cfg->data_format = p_dev_state->afe_usba_dev_cfg.data_format;
          p_usba_cfg->num_channels = p_dev_state->afe_usba_dev_cfg.num_channels;
          p_usba_cfg->sample_rate = p_dev_state->afe_usba_dev_cfg.sample_rate;
          p_usba_cfg->minor_version = p_dev_state->afe_usba_dev_cfg.cfg_version;
        }
        break;

        default:
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
                cfg_ver, afe_port_ptr->intf);
          return ADSP_EBADPARAM;
        }
        break;
      }
    }
    break;
    case AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS:
    {
      if(TRUE == p_dev_state->is_usb_device_token_rcvd)
      {
        uint32_t params_ver = p_dev_state->afe_usba_dev_cfg.dev_params_version;
        switch(params_ver)
        {
          case 1:
          {
            *params_buffer_len_req_ptr = sizeof(afe_param_id_usb_audio_dev_params_v1_t);
            if(*params_buffer_len_req_ptr > payload_size)
            {
              MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: USB device cfg required size = %d, given size = %lu",
                    *params_buffer_len_req_ptr, payload_size);
              return ADSP_ENOMEMORY;
            }
            afe_param_id_usb_audio_dev_params_v1_t *p_usba_dev_params = (afe_param_id_usb_audio_dev_params_v1_t *)param_buf_ptr;

            p_usba_dev_params->usb_device_token = p_dev_state->afe_usba_dev_cfg.usb_device_token;
            p_usba_dev_params->minor_version = p_dev_state->afe_usba_dev_cfg.dev_params_version;
          }
          break;

          default:
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio cfg Get Cmd dev params fail: InCompatible params version: %ld, port_id: %d",
                  params_ver, afe_port_ptr->intf);
            return ADSP_EBADPARAM;
          }
          break;
        }
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio cfg Get Cmd fail: device parameters did not receive yet: port_id: %d",
              afe_port_ptr->intf);
        return ADSP_EBADPARAM;
      }
    }
    break;
    case AFE_PARAM_ID_USB_AUDIO_DEV_LPCM_FMT:
    {
      uint32_t lpcm_ver = p_dev_state->afe_usba_dev_cfg.lpcm_fmt_version;
      switch(lpcm_ver)
      {
        case 1:
        {
          *params_buffer_len_req_ptr = sizeof(afe_param_id_usb_audio_dev_lpcm_fmt_v1_t);
          if(*params_buffer_len_req_ptr > payload_size)
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: USB device cfg required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, payload_size);
            return ADSP_ENOMEMORY;
          }
          afe_param_id_usb_audio_dev_lpcm_fmt_v1_t *p_usba_lpcm_fmt = (afe_param_id_usb_audio_dev_lpcm_fmt_v1_t *)param_buf_ptr;

          p_usba_lpcm_fmt->media_format = p_dev_state->afe_usba_dev_cfg.lpcm_med_format.media_format;
          p_usba_lpcm_fmt->minor_version = p_dev_state->afe_usba_dev_cfg.lpcm_fmt_version;
        }
        break;

        default:
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio cfg Get Cmd dev params fail: InCompatible params version: %ld, port_id: %d",
                lpcm_ver, afe_port_ptr->intf);
          return ADSP_EBADPARAM;
        }
        break;

      }
    }
    break;
    default:
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "USB Audio cfg Get Cmd fail: unsupported param id: 0x%lX, port_id: %d",
            param_id, afe_port_ptr->intf);

      return ADSP_EBADPARAM;
    }

  }

  return ADSP_EOK;
}

/**
  @brief Open the configured USB Audio device for data transfers

  @param[in] afe_port_ptr pointer to AFE device port structure

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_usba_open (afe_dev_port_t* afe_port_ptr)
{  
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;

  if(NULL == p_dev_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to open usb device for intf: 0x%x as driver handle is NULL", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  if(TRUE != p_dev_state->is_usb_device_token_rcvd)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to open usb device for intf: 0x%x as USB device params are not yet rcvd", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  /* Open the USB driver and get the handle */
  if(UAC_STATUS_SUCCESS != uac_usb_open(&p_dev_state->usb_drv.usba_driver_hdl))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to open usb driver");
    return ADSP_EFAILED;
  }

  /* Set the device configuration */
  uac_session_cfg_t cfg;
  memset(&cfg, 0, sizeof(uac_session_cfg_t));

  cfg.bit_width = p_dev_state->afe_usba_dev_cfg.bit_width;
  cfg.data_format = (uac_data_format)p_dev_state->afe_usba_dev_cfg.data_format;
  cfg.hlos_token = p_dev_state->afe_usba_dev_cfg.usb_device_token;
  cfg.justification = UAC_DATA_LEFT_JUSTIFIED; // AVS always relies on left justified data, currently

  if(TX_DIR == afe_port_ptr->dir)
  {
    cfg.dir = USB_AUDIO_MIC;
  }
  else
  {
    cfg.dir = USB_AUDIO_SPEAKER;
  }

  cfg.num_channels = p_dev_state->afe_usba_dev_cfg.num_channels;
  cfg.num_of_samples = p_dev_state->afe_port_ptr->int_samples_per_period;
  cfg.sample_size_in_bytes = p_dev_state->afe_usba_dev_cfg.bytes_per_channel;
  cfg.sampling_rate = p_dev_state->afe_usba_dev_cfg.sample_rate;

  if(UAC_LINEAR_PCM == p_dev_state->afe_usba_dev_cfg.data_format)
  {
    cfg.media_format = (void*)&p_dev_state->afe_usba_dev_cfg.lpcm_med_format;
    cfg.media_format_buf_size = sizeof(p_dev_state->afe_usba_dev_cfg.lpcm_med_format);
  }

  if(UAC_STATUS_SUCCESS != uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_SET_CFG, &cfg, sizeof(cfg)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to set config to usb driver");
    return ADSP_EFAILED;
  }

  /* Get the USB data path delay */
  if(UAC_STATUS_SUCCESS != uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_GET_DATA_PATH_DELAY,
                                         &p_dev_state->usb_drv.data_path_delay, sizeof(p_dev_state->usb_drv.data_path_delay)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get data path delay from USB driver");
    return ADSP_EFAILED; // TODO: Check whether we need to return failure or not
  }

  /* Get avtimer and qtimer offset */
  /* UAC driver gives qtimer timestamp for data buffers. Qtimer timestamp need to be converted to AVtimer
   * Offset for AVtimer and Qtimer to convert Qtimer timestamp to AVtimer timestamp*/
  uint64_t avtimer_timestamp = 0, qtimer_timestamp = 0;

  if(ADSP_EOK != avtimer_drv_latch_avtimer_qtimer(afe_port_ptr->avt_drv, &avtimer_timestamp, &qtimer_timestamp))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to latch avtimer and qtimer together");
    return ADSP_EFAILED;
  }
  MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " avtimer_timestamp 0x%lx%lx qtimer_timestamp 0x%lx%lx for port 0x%x",
        (uint32_t)(avtimer_timestamp >> 32), (uint32_t)avtimer_timestamp, (uint32_t)(qtimer_timestamp >> 32),
        (uint32_t)qtimer_timestamp, afe_port_ptr->intf);

  p_dev_state->usb_drv.avtimer_qtimer_offset = avtimer_timestamp - qtimer_timestamp;

  afe_port_ptr->is_first_interrupt = TRUE;
  afe_port_ptr->is_interleaved = TRUE;

  /* Start the USB device */
  uac_session_start_t session_cfg;

  session_cfg.client_data_ptr = p_dev_state->afe_port_ptr;
  session_cfg.uac_data_cb = afe_usb_data_callback;
  if(UAC_STATUS_SUCCESS != uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_START_SESSION, &session_cfg, sizeof(session_cfg)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start the device session by USB driver");
    return ADSP_EFAILED;
  }

  return ADSP_EOK;
}

/**
 * Close the configured USB Audio device. 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @return
 * ADSP_EOK on success, an error code on error 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_close(afe_dev_port_t* afe_port_ptr)
{
  ADSPResult  result = ADSP_EOK;
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;

  if(NULL == p_dev_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to close usb device session for intf: 0x%x as driver handle is NULL", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  if (NULL == p_dev_state->usb_drv.usba_driver_hdl)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Invalid USB driver handle, port_id: 0x%x", afe_port_ptr->intf);
    return ADSP_EOK;
  }

  /* Stop the USB session */
  if(UAC_STATUS_SUCCESS != uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_STOP_SESSION, NULL, 0))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to stop the device session by USB driver");
    result = ADSP_EFAILED;
  }

  /* close USB device */
  if(UAC_STATUS_SUCCESS != uac_usb_close(p_dev_state->usb_drv.usba_driver_hdl))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close usb driver");
    result |= ADSP_EFAILED;
  }

  /* deallocate device handle */
  qurt_elite_memory_free(afe_port_ptr->afe_drv);
  afe_port_ptr->afe_drv = NULL;

  return result;
}

/**
 * Data is fetched from USB audio core bsp driver to local 
 * buffer in case of Mic path (Tx path) 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] pBuffer - buffer to store the fetched data 
 *  
 * @param[in] numSamples - number of samples to be fetched
 *  
 * @return  ADSP_EOK on success, an error code on error 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_read(afe_dev_port_t *afe_port_ptr, int16_t *pBuffer, uint32_t numSamples)
{
  ADSPResult result = ADSP_EOK;
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;
  uac_session_read_data_t read_data;
  uint32_t num_bytes_to_read;

  uac_status_e uac_result = UAC_STATUS_SUCCESS;

  num_bytes_to_read = numSamples * afe_port_ptr->channels * afe_port_ptr->bytes_per_channel;
  read_data.buffer_ptr = pBuffer;
  read_data.num_bytes = num_bytes_to_read;

  /* IOCTL to read the usb data into AFE buffer */
  if(UAC_STATUS_SUCCESS != (uac_result = uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_READ_DATA, &read_data, sizeof(read_data))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from usb driver, uac result %ld", uac_result);
  }

  if(read_data.actual_num_bytes_read != num_bytes_to_read)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Expected bytes to read %ld but the bytes actually read %ld",
          num_bytes_to_read, read_data.actual_num_bytes_read);
  }

  /* Converting to Q27 for 24bit case */
  if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_usba_dev_cfg.bit_width)
  {
    uint32_t samp, total_num_samp;
    int32_t *src_ptr_32, *dst_ptr_32;
    int64_t *src_ptr_64, *dst_ptr_64;


#ifdef __qdsp6__

    /* Calculate number of 32-bit samples */
    total_num_samp = (num_bytes_to_read >> 2);

    /* For Odd number of samples, copy one sample at a time */
    if(total_num_samp & 0x1)
    {
      src_ptr_32 = (int32_t *)pBuffer;
      dst_ptr_32 = (int32_t *)pBuffer;

      for(samp = total_num_samp; samp > 0; samp--)
      {
        (*dst_ptr_32++) = (*src_ptr_32++) >> QFORMAT_SHIFT_FACTOR;
      }
    }
    else
    {
      src_ptr_64 = (int64_t *)pBuffer; /* pBuffer points to 8 bytes aligned buffer  */
      dst_ptr_64 = (int64_t *)pBuffer; /* pBuffer points to 8 bytes aligned buffer       */

      /* Q31 to Q28 conversion
       */
      for(samp = total_num_samp; samp >= 2; samp -= 2)
      {
        (*dst_ptr_64++) = Q6_P_vasrw_PI(*src_ptr_64++, QFORMAT_SHIFT_FACTOR);
      }
    }

#else           /*------------ Non Q6 Version --------------------*/

    src_ptr_32 = (int32_t *)pBuffer;
    dst_ptr_32 = (int32_t *)pBuffer;

    for(samp = 0; samp < total_num_samp; samp++)
    {
      (*dst_ptr_32++) = (*src_ptr_32++) >> QFORMAT_SHIFT_FACTOR;
    }
#endif
  }

  p_dev_state->usb_drv.acc_avt_drift_us = read_data.acc_drift_us;


  return result;
}


/**
 * Device data will be writen to corebsp USB audio driver from
 * local buffer in case of Speaker path (Rx path) 
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *  
 * @param[in] pBuffer - data buffer to be pushed into DMA buffer
 *  
 * @param[in] numSamples - number of samples to be copied
 *  
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error 
 * code 
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_write(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
  ADSPResult result = ADSP_EOK;
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;
  uac_session_write_data_t write_data;
  uint32_t num_bytes_to_write;

  uac_status_e uac_result = UAC_STATUS_SUCCESS;

  num_bytes_to_write = numSamples * afe_port_ptr->channels * afe_port_ptr->bytes_per_channel;

  write_data.buffer_ptr = pBuffer;
  write_data.num_bytes = num_bytes_to_write;

  /* Converting from Q27 */
  if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_usba_dev_cfg.bit_width)
  {
    uint32_t samp = 0, total_num_samp = (num_bytes_to_write >> 2);
    int32_t *src_ptr_32 = (int32_t *)pBuffer;
    int32_t	*dst_ptr_32 = (int32_t *)pBuffer;

    for(samp = 0; samp < total_num_samp; samp++)
    {
      *dst_ptr_32++ = s32_shl_s32_sat(*src_ptr_32++, QFORMAT_SHIFT_FACTOR);
    }
  }

  /* IOCTL to read the usb data into AFE buffer */
  if(UAC_STATUS_SUCCESS != (uac_result = uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_WRITE_DATA,
                                                       &write_data, sizeof(write_data))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to write data from usb driver, uac result %ld", uac_result);
  }

  p_dev_state->usb_drv.acc_avt_drift_us = write_data.acc_drift_us;

  return result;
}


/**
 * Update drift for USB Audio port.
 *
 * @param[in] pDevPort Pointer to the instance of the port.
 * @return
 * void pointer to the handle.
 *
 * @dependencies
 * None.
 */
ADSPResult afe_update_avt_drift_for_usba_port(afe_dev_port_t *afe_port_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;

  if(afe_port_ptr->is_first_dma)
  {
    /* If first DMA reset the AVT vs device drift */
    afe_drv_update_avt_drift_info_element(&(afe_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), p_dev_state->usb_drv.acc_avt_drift_us, TRUE);
  }
  else /* For subsequent DMA interrupts */
  {
    afe_drv_update_avt_drift_info_element(&(afe_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), p_dev_state->usb_drv.acc_avt_drift_us, FALSE);
  }

  return result;
}

/**
 * For providing the delay introduced in USB Audio driver data
 * path delay
 *
 * @param[in] port_id port id of the USB audio device
 *@param[out] data_path_delay_us is the delay introduced due 
 *    to internal buffering of USB audio driver

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_usba_get_data_path_delay(afe_dev_port_t *afe_port_ptr, uint64_t* data_path_delay_us)
{
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;

  if(NULL != p_dev_state)
  {
    *data_path_delay_us = p_dev_state->usb_drv.data_path_delay.data_path_delay_us;
  }
  else
  {
    *data_path_delay_us = 0;
  }
  return ADSP_EOK;
}

/**
 * For getting the timestamp for USBaudio device
 * @param[in] port_id port id of the USB audio device

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_get_timestamp(afe_dev_port_t *afe_port_ptr)
{
  uac_get_timestamp_type_t timestamp;
  uac_status_e uac_result;
  afe_usba_dev_state_struct_t *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;

  timestamp.timestamp_us = 0;

  if(UAC_STATUS_SUCCESS != (uac_result = uac_usb_ioctl(p_dev_state->usb_drv.usba_driver_hdl, UAC_IOCTL_GET_TIMESTAMP,
                                                       &timestamp, sizeof(timestamp))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get timestamp from UAC driver, uac result %ld", uac_result);
    return ADSP_EFAILED;
  }

  afe_port_ptr->port_av_timestamp = timestamp.timestamp_us + p_dev_state->usb_drv.avtimer_qtimer_offset;

  return ADSP_EOK;
}

/**
 * For restoring usb audio device configuration
 * @param[in] port_id port id of the USB audio device

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_restore_usba_cfg(afe_dev_port_t *afe_port_ptr)
{
  ADSPResult                    result = ADSP_EFAILED;
  afe_usba_dev_state_struct_t    *p_dev_state = (afe_usba_dev_state_struct_t *)afe_port_ptr->afe_drv;
  afe_param_id_usb_audio_cfg_v1_t    usba_cfg_info;
  afe_param_id_usb_audio_dev_params_v1_t usba_dev_params;

  if (NULL != p_dev_state)
  {
    usba_cfg_info.minor_version = p_dev_state->afe_usba_dev_cfg.cfg_version;
    usba_cfg_info.bit_width = p_dev_state->afe_usba_dev_cfg.bit_width;
    usba_cfg_info.num_channels = p_dev_state->afe_usba_dev_cfg.num_channels;
    usba_cfg_info.sample_rate = p_dev_state->afe_usba_dev_cfg.sample_rate;
    usba_cfg_info.data_format = p_dev_state->afe_usba_dev_cfg.data_format;
    usba_cfg_info.reserved = 0;

    if(ADSP_EOK != (result = afe_set_usba_cfg(afe_port_ptr,
                                              (int8_t *)&usba_cfg_info,
                                              sizeof(usba_cfg_info),
                                              AFE_PARAM_ID_USB_AUDIO_CONFIG)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to restore USBA config, port_id: 0x%lx", afe_port_ptr->intf);

      result = ADSP_EFAILED;
    }
    usba_dev_params.minor_version = p_dev_state->afe_usba_dev_cfg.dev_params_version;
    usba_dev_params.usb_device_token = p_dev_state->afe_usba_dev_cfg.usb_device_token;

    if(ADSP_EOK != (result = afe_set_usba_cfg(afe_port_ptr,
                                              (int8_t *)&usba_dev_params,
                                              sizeof(usba_dev_params),
                                              AFE_PARAM_ID_USB_AUDIO_DEV_PARAMS)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to restore USBA dev params, port_id: 0x%lx", afe_port_ptr->intf);

      result = ADSP_EFAILED;
    }
  }

  return result;
}
