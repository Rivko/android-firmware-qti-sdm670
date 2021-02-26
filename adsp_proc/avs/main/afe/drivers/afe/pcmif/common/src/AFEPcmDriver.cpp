/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/src/AFEPcmDriver.cpp#3 $
$DateTime: 2017/12/07 12:24:07 $
$Author: pwbldsvc $
$Change: 15009245 $
$Revision: #3 $

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

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pcmif/common/src/AFEPcmDriver.cpp#3 $ $DateTime: 2017/12/07 12:24:07 $ $Author: pwbldsvc $

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
#include "AFEPcmDriver.h"
#include "AFEPcmDriver_i.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalPcm.h"
#include "AFEAudioIfCommon.h"
#include "AFEDeviceDriverInterface.h"
#include "hwd_devcfg.h"
#include "AFEHalDump.h"

/*=====================================================================
 Globals
 ======================================================================*/
//Note: if it is possible, decouple driver from service layer for better modulization.


//static declarations for afe ports per port
static afe_dev_port_t port_rx_primary_pcm;
static afe_dev_port_t port_tx_primary_pcm;
static afe_dev_port_t port_rx_secondary_pcm;
static afe_dev_port_t port_tx_secondary_pcm;
static afe_dev_port_t port_rx_tertiary_pcm;
static afe_dev_port_t port_tx_tertiary_pcm;
static afe_dev_port_t port_rx_quaternary_pcm;
static afe_dev_port_t port_tx_quaternary_pcm;
static afe_dev_port_t port_rx_quinary_pcm;
static afe_dev_port_t port_tx_quinary_pcm;



//static delcarations for pcm dev port state per pcm interfaces.
static pcm_port_state_t  pcm_rx_primary;
static pcm_port_state_t  pcm_tx_primary;
static pcm_port_state_t  pcm_rx_secondary;
static pcm_port_state_t  pcm_tx_secondary;
static pcm_port_state_t  pcm_rx_tertiary;
static pcm_port_state_t  pcm_tx_tertiary;
static pcm_port_state_t  pcm_rx_quaternary;
static pcm_port_state_t  pcm_tx_quaternary;
static pcm_port_state_t  pcm_rx_quinary;
static pcm_port_state_t  pcm_tx_quinary;


//global variable for pcm driver to share any variable common to all ports for example device config, etc.
static pcm_drv_state_t    pcm_driver; 


/*=====================================================================
 Static Functions
 ======================================================================*/
static ADSPResult afe_pcm_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size);
static ADSPResult afe_pcm_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);
static ADSPResult afe_pcm_driver_start(void *afe_drv, dev_start_params_t *pcm_start_params_ptr);
static ADSPResult afe_pcm_driver_stop(void *afe_drv);
static ADSPResult afe_pcm_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr);
static ADSPResult afe_pcm_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size);
static ADSPResult afe_pcm_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *pcm_port_cfg_data_ptr);
static void afe_pcm_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address);
static void afe_pcm_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address);
static ADSPResult afe_pcm_dev_config_port (pcm_port_state_t* p_dev_state, afe_param_id_pcm_cfg_t *pcm_cfg_ptr);


static void afe_pcm_int_callback(void* afe_drv, uint32 event_mask);
static ADSPResult afe_pcm_driver_read(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static ADSPResult afe_pcm_driver_write(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static uint32 afe_pcm_get_watermark(pcm_port_state_t *p_dev_state);
static uint32 afe_pcm_get_num_dma_buffer(pcm_port_state_t *p_dev_state);
static ADSPResult afe_pcm_driver_device_cfg_init(pcm_dev_cfg_struct_t * pcm_hw_info_ptr);

static afe_driver_func_tbl_t sink_ftbl =
{

   afe_pcm_driver_write,
   afe_pcm_set_param,
   afe_pcm_get_param,
   afe_pcm_driver_start,
   afe_pcm_driver_stop,
   NULL,
   afe_pcm_get_properties,
};

static afe_driver_func_tbl_t source_ftbl =
{
   afe_pcm_driver_read,
   afe_pcm_set_param,
   afe_pcm_get_param,
   afe_pcm_driver_start,
   afe_pcm_driver_stop,
   NULL,
   afe_pcm_get_properties,
};



/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform PCM Driver Initialization
            The driver init function does
                  1. get hardware device config for pcm
                  2. get the virtual addres for hardware registers
                  3. get the avtimer tick frequency which will be used for converting the dma timestamp to usec.
                  4. initialize all the device driver structures 

  @param[in]
  None

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_pcm_driver_init(void)
{
	ADSPResult   result = ADSP_EOK;

	memset(&pcm_driver, 0, sizeof(pcm_drv_state_t));
 	pcm_dev_cfg_struct_t *pcm_hw_info_ptr = &pcm_driver.pcm_hw_info;

   // 1. read pcm hwd dev config and cache it to the pcm_driver global state
   if(ADSP_FAILED(result = afe_pcm_driver_device_cfg_init(pcm_hw_info_ptr)))
	{
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error afe_pcm_driver_device_cfg_init");
		return result;
	}

   // 2. create memory region for lpass pcm interface hw
   //   This information is need to be saved to  pcm_driver global state

   // pcm register interface
   if(ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&pcm_driver.pcm_qmem_region,
            pcm_hw_info_ptr->pcm_reg_base, pcm_hw_info_ptr->pcm_reg_size, &pcm_driver.pcm_virt_addr)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for pcm register base");
      return result;
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "pcm virtual addr 0x%x phy addr 0x%x",
      (unsigned int)pcm_driver.pcm_virt_addr, (unsigned int)pcm_hw_info_ptr->pcm_reg_base);
   }

   // 3. do the hal init which save the physical address and virtual address offset.
   hal_pcm_init(pcm_hw_info_ptr->hw_revision, pcm_hw_info_ptr->pcm_reg_base, pcm_driver.pcm_virt_addr, pcm_hw_info_ptr->pcm_reg_offset);

   //4. initialize all the device driver structures 

   pcm_driver.driver_state = PCM_DRIVER_INIT;

	return ADSP_EOK;
}

/**
  @brief Perform PCM Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_pcm_driver_deinit(void)
{

   if(PCM_DRIVER_INIT != pcm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }

   /* release all resource allocated during init*/
   HwdDevCfg_DestoryVirtAddress(pcm_driver.pcm_qmem_region);

   // change the driver state.
   pcm_driver.driver_state = PCM_DRIVER_DEINIT;

   return ADSP_EOK;
}

/**
  @brief Perform PCM port initialization
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
ADSPResult afe_pcm_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
    ADSPResult      result;
    afe_dev_port    *dev_port_ptr = NULL;
    uint16_t        port_id;
    afe_dev_port_proc_func afe_dev_process = NULL;
    pcm_intf_idx_t pcm_inf;
    uint32_t dir;
    pcm_port_state_t *p_dev_state = NULL;
    afe_driver_func_tbl_t *ftable_ptr;

    if(PCM_DRIVER_INIT != pcm_driver.driver_state)
    {
       return ADSP_EFAILED;
    }

    for (port_id = AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START; port_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END; port_id++)
    {
          switch (port_id)
          {
             case AFE_PORT_ID_PRIMARY_PCM_RX:
                {
                   dev_port_ptr = &port_rx_primary_pcm;
                   p_dev_state = &pcm_rx_primary;
                   ftable_ptr = &sink_ftbl;
                   pcm_inf = PCM_PRIMARY;
                   dir = PCM_SINK;
                }
                break;
             case AFE_PORT_ID_PRIMARY_PCM_TX:
                {
                   dev_port_ptr = &port_tx_primary_pcm;
                   p_dev_state = &pcm_tx_primary;
                   ftable_ptr = &source_ftbl;                   
                   pcm_inf = PCM_PRIMARY;
                   dir = PCM_SOURCE;
                }
                break;
             case AFE_PORT_ID_SECONDARY_PCM_RX:
                {
                   dev_port_ptr = &port_rx_secondary_pcm;
                   p_dev_state = &pcm_rx_secondary;
                   ftable_ptr = &sink_ftbl;
                   pcm_inf = PCM_SECONDARY; 
                   dir = PCM_SINK;
                }
                break;
             case AFE_PORT_ID_SECONDARY_PCM_TX:
                {
                   dev_port_ptr = &port_tx_secondary_pcm;
                   p_dev_state = &pcm_tx_secondary;
                   ftable_ptr = &source_ftbl;                   
                   pcm_inf = PCM_SECONDARY;
                   dir = PCM_SOURCE;
                }                 
                break;
             case AFE_PORT_ID_TERTIARY_PCM_RX:
                {
                   dev_port_ptr = &port_rx_tertiary_pcm;
                   p_dev_state = &pcm_rx_tertiary;
                   ftable_ptr = &sink_ftbl;
                   pcm_inf = PCM_TERTIARY;
                   dir = PCM_SINK;
                }     
                break;
             case AFE_PORT_ID_TERTIARY_PCM_TX:
                {
                   dev_port_ptr = &port_tx_tertiary_pcm;
                   p_dev_state = &pcm_tx_tertiary;
                   ftable_ptr = &source_ftbl;
                   pcm_inf = PCM_TERTIARY;
                   dir = PCM_SOURCE;
                }    
                break;
             case AFE_PORT_ID_QUATERNARY_PCM_RX:
                {
                   dev_port_ptr = &port_rx_quaternary_pcm;
                   p_dev_state = &pcm_rx_quaternary;
                   ftable_ptr = &sink_ftbl;
                   pcm_inf = PCM_QUATERNARY;
                   dir = PCM_SINK;
                }  
                break;
             case AFE_PORT_ID_QUATERNARY_PCM_TX:
                {
                   dev_port_ptr = &port_tx_quaternary_pcm;
                   p_dev_state = &pcm_tx_quaternary;
                   ftable_ptr = &source_ftbl;
                   pcm_inf = PCM_QUATERNARY;
                   dir = PCM_SOURCE;
                }                
                break;
             case AFE_PORT_ID_QUINARY_PCM_RX:
             {
               dev_port_ptr = &port_rx_quinary_pcm;
               p_dev_state = &pcm_rx_quinary;
               ftable_ptr = &sink_ftbl;
               pcm_inf = PCM_QUINARY;
               dir = PCM_SINK;
             }
             break;
             case AFE_PORT_ID_QUINARY_PCM_TX:
             {
               dev_port_ptr = &port_tx_quinary_pcm;
               p_dev_state = &pcm_tx_quinary;
               ftable_ptr = &source_ftbl;
               pcm_inf = PCM_QUINARY;
               dir = PCM_SOURCE;
             }
             break;
              default:
              {
                // Audio Port ID is not pcm and skipped
                dev_port_ptr = NULL;
                afe_dev_process = NULL;
              }
          }

          if (NULL != dev_port_ptr)
          {
             dir = port_id&AFE_PORT_DIR_MASK;
             if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
             {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create port = %d, aborting", port_id);
                return result;
             }
             memset(p_dev_state, 0, sizeof(pcm_port_state_t));

             // As the pcm port id can not changed without HLOS approval, these port pointer arrary can not be changed at this moment.
             afe_svc_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)] = dev_port_ptr;
             
             dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = &p_dev_state->pcm_avt_drift;
             dev_port_ptr->afe_dev_process = afe_dev_process;
             dev_port_ptr->afe_drv_ftbl = ftable_ptr;
             dev_port_ptr->avt_drift_params.avtimer_latch_type = TIMESTAMP_LATCH_TYPE_HARDWARE;

             // This can be assinged during open once afe introduce the open and close command for AFE port.
             dev_port_ptr->afe_drv = (void *) p_dev_state;

             // afe device state per port...
             p_dev_state->intf = pcm_inf;
             p_dev_state->direction = dir;

          }   
    }

    return ADSP_EOK;
}



/**
  @brief Perform PCM port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_pcm_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
   // check the driver state
   if(PCM_DRIVER_INIT != pcm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }   

   // free resources if there is anything is allocated during pcm port init.
   afe_dev_port    *dev_port_ptr = NULL;
   
   //  The port id is not straitforward because the port id are mixed up for pcm, 
   // initialize slimbus related ports
   for (uint16_t port_id = AFE_PORT_ID_AUDIO_IF_PORT_RANGE_START; port_id <= AFE_PORT_ID_AUDIO_IF_PORT_RANGE_END; port_id++)
   {
      switch (port_id)
      {
         case AFE_PORT_ID_PRIMARY_PCM_RX:
         case AFE_PORT_ID_PRIMARY_PCM_TX:
         case AFE_PORT_ID_SECONDARY_PCM_RX:
         case AFE_PORT_ID_SECONDARY_PCM_TX:
         case AFE_PORT_ID_TERTIARY_PCM_RX:
         case AFE_PORT_ID_TERTIARY_PCM_TX:
         case AFE_PORT_ID_QUATERNARY_PCM_RX:
         case AFE_PORT_ID_QUATERNARY_PCM_TX:
            dev_port_ptr = afe_svc_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)];        
            break;
         default:
         {
            // Audio Port ID is not pcm and skipped
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
  @brief Set pcm port config
               shoudn't pass any of afe_dev_port_t structure type from this point to make driver independent from service layer.

  @param[in] afe_drv pointer for AFE dev port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;

   // check the driver state
   if(PCM_DRIVER_INIT != pcm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }   

   if((NULL == afe_drv) || (NULL == cfg_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / params pointer is null");
      return ADSP_EFAILED;
   }   

   switch(param_id)
   {
      case AFE_PARAM_ID_PCM_CONFIG:
      { 
         // choose the appropriate config version
         uint32_t pcm_cfg_version = (uint32_t) *(cfg_ptr);
         if(pcm_cfg_version > AFE_API_VERSION_PCM_CONFIG)
         {
            pcm_cfg_version = AFE_API_VERSION_PCM_CONFIG;
         }

         switch (pcm_cfg_version)
         {
            case AFE_API_VERSION_PCM_CONFIG:
            {
               afe_param_id_pcm_cfg_t *pcm_cfg_ptr =  (afe_param_id_pcm_cfg_t *)cfg_ptr;

               if (payload_size < sizeof(afe_param_id_pcm_cfg_t))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM Cfg fail: Invalid payload size: %ld, pcm port index:%d, dir:%d", \
                     payload_size, p_dev_state->intf, p_dev_state->direction);
                     result = ADSP_EBADPARAM;
               }
               else
               {
                  result = afe_pcm_dev_config_port(p_dev_state, pcm_cfg_ptr);
                  p_dev_state->dev_cfg_minor_ver = pcm_cfg_version;
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
  @brief Get PCM port config
               
  @param[in] afe_drv pointer for afe device port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_get_param (void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;

   if((NULL == afe_drv) || (NULL == param_buf_ptr) ||  (NULL == params_buffer_len_req_ptr) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / params pointers are null");
      return ADSP_EFAILED;
   }

   switch(param_id)
   {
      case AFE_PARAM_ID_PCM_CONFIG:
      { 
         switch (p_dev_state->dev_cfg_minor_ver)
         {
             case AFE_API_VERSION_PCM_CONFIG:
             {
                 *params_buffer_len_req_ptr = sizeof(afe_param_id_pcm_cfg_t);
                 if(*params_buffer_len_req_ptr > payload_size )
                 {
                     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: PCM cfg required size = %d, given size = %lu",
                                     *params_buffer_len_req_ptr, payload_size);
                     return ADSP_ENOMEMORY;
                 }
                 afe_param_id_pcm_cfg_t  *pcm_cfg_ptr = (afe_param_id_pcm_cfg_t *)param_buf_ptr;
       
                 /* Populate the payload parameters */
                 pcm_cfg_ptr->pcm_cfg_minor_version = p_dev_state->dev_cfg_minor_ver;
                 pcm_cfg_ptr->bit_width    = p_dev_state->bit_width;
                 pcm_cfg_ptr->sample_rate  = p_dev_state->sample_rate;
                 pcm_cfg_ptr->num_channels  = p_dev_state->num_channels;
                 pcm_cfg_ptr->aux_mode = p_dev_state->sync;
                 pcm_cfg_ptr->ctrl_data_out_enable  = p_dev_state->ctrl_data_oe;
                 pcm_cfg_ptr->frame_setting = p_dev_state->frame;
                 pcm_cfg_ptr->quant_type = p_dev_state->quant;
                 pcm_cfg_ptr->reserved = 0;
                 memset(pcm_cfg_ptr->slot_number_mapping, 0, sizeof(pcm_cfg_ptr->slot_number_mapping));
                 for (uint16_t index=0; index<AFE_AUX_PCM_MAX_NUM_SLOTS; index++)
                 {
                    pcm_cfg_ptr->slot_number_mapping[index] = p_dev_state->slot_number_mapping[index];
                 }
               
              }
              break;
              default:
              {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, pcm intf : %d",
                          p_dev_state->dev_cfg_minor_ver, p_dev_state->intf);
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
  @brief Set up the data transfer for PCM port
               1. check if any shared resource is already is used. mux, data lines,...
               2. open dma manger with callback function
               3. config pcm interface
               4. start the dma
               5. enable the pcm interface

  @param[in] afe_drv pointer for afe device port structure
  @param[in]  pcm_open_params_ptr for open parameters

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_driver_start(void *afe_drv, dev_start_params_t *pcm_start_params_ptr)
{
   ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;
   dma_mgr_open_params_t open_params;
   uint32_t pcm_index = (uint32_t)p_dev_state->intf;
   uint32_t pcm_dir =  (uint32_t)p_dev_state->direction;
   bool_t intf_supported = FALSE;

   if((NULL == afe_drv) || (NULL == pcm_start_params_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / Open Params Null");
      return ADSP_EFAILED;
   }

   if(pcm_index <= PCM_INTERFACE_MAX)
   {
     /* Checks the interface supported by chipset or not */
     for(uint32_t pcm_curr_idx = 0; pcm_curr_idx <= PCM_INTERFACE_MAX; pcm_curr_idx++)
     {
       if( pcm_index == (pcm_driver.pcm_hw_info.inf_type_list[pcm_curr_idx]) )
       {
         intf_supported = TRUE;
         break;
       }
     }
   }

   if (FALSE == intf_supported)
   {
     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid PCM index: 0x%x", pcm_index);
     return ADSP_EFAILED;
   }

#ifdef AUDIOIF_REG_DUMP   
   uint32_t dma_index = 0;
   hal_afe_clock_dump(LPA_IF_PCM, pcm_index, LPA_IF_START);
#endif

   // config mux only for PRI, SEC, TER, QUA PCM.
   // what if there is no mux required for pri, sec, ter, qua....
   //1. alloc any shared resource
   uint32_t mux_index = (uint32_t)p_dev_state->intf;
   if(ADSP_FAILED(result = afe_audioif_mux_alloc(AUDIOIF_PCM, mux_index)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux already used for pcm %ul", p_dev_state->intf);
      return result;
   }

   /* Set the block transfer time */
   // this will be determined during the beginning of start cmd and after the config cmd 
   p_dev_state->int_samples_per_period = pcm_start_params_ptr->int_samples_per_period;
   
   // callback info for interrupt handling
   p_dev_state->cb_info.afe_dev_int_cb = pcm_start_params_ptr->cb_info.afe_dev_int_cb;
   p_dev_state->cb_info.cb_arg = pcm_start_params_ptr->cb_info.cb_arg;

   // dma manager open
   memset(&open_params, 0, sizeof(dma_mgr_open_params_t));

   // hardware type and dma type
   open_params.hardware_type = LPASS_HW_PCM;
   open_params.dma_type = LPASS_HW_DMA_TYPE0;

	//  open dma manger with callback function for read or write
	if(PCM_SINK == p_dev_state->direction)
   {
      open_params.dma_dir = LPASS_HW_DMA_SINK;
      open_params.dma_write_func_ptr = (dma_mgr_write_func)afe_pcm_write_dma;
   }
   else
   {
      open_params.dma_dir = LPASS_HW_DMA_SOURCE;
    	open_params.dma_read_func_ptr = (dma_mgr_read_func)afe_pcm_read_dma;
   }

   open_params.sample_rate = p_dev_state->sample_rate;

   open_params.int_samples_per_period = p_dev_state->int_samples_per_period;

   open_params.input_bytes_per_sample =  p_dev_state->bytes_per_channel;
      
   open_params.output_bytes_per_sample =  p_dev_state->bytes_per_channel;

   open_params.num_channels = p_dev_state->num_channels;
   
   // dma buffer type   
	open_params.buff_mem_type = DMA_MGR_LPM_BUFFER; 

   // 4 kinds of format is possible here. combination of linear/non-linear  packed/non-packed
   open_params.data_type = p_dev_state->data_format;

 	// register the interrupt callback 
   open_params.dma_int_cb = afe_pcm_int_callback;
   open_params.afe_driver_context = afe_drv;

   // this info will be passing down to dma hal and it is only for audioif cases including pcm, .
   // This value should be matching to audioif dma hal config function
   open_params.ifconfig_dma_control  = hal_pcm_get_intf(p_dev_state->intf,p_dev_state->direction);

   // get the watermark value. if watermark is 1, then dma burst mode is supposed to be off.  
 	open_params.watermark = afe_pcm_get_watermark(p_dev_state);

	// dma buffer size need to be checked carefully based on alignment requirement...
	if( 0 == (open_params.num_buffers = afe_pcm_get_num_dma_buffer(p_dev_state)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get req DMA num buffers");
    goto __bailout;
  }

	if(ADSP_FAILED(result = afe_dma_mgr_open(&p_dev_state->dma_dev_hdl, &open_params)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register with DMA Manager with status %d",result);
		goto __bailout;
	}

      /* HAL V2 way to reset , release, config depends on direction */
   hal_pcm_reset(pcm_index, pcm_dir);
   hal_pcm_reset_release(pcm_index, pcm_dir);        
  
   // config pcm interface
   pcm_config_t pcm_config_param;
   memset(&pcm_config_param, 0, sizeof(pcm_config_t));
   
   pcm_config_param.slot_num = p_dev_state->slot_num;
   pcm_config_param.slot[0] = p_dev_state->slot_number_mapping[0];
   pcm_config_param.slot[1] = p_dev_state->slot_number_mapping[1];
   pcm_config_param.slot[2] = p_dev_state->slot_number_mapping[2];
   pcm_config_param.slot[3] = p_dev_state->slot_number_mapping[3];
   pcm_config_param.frame = p_dev_state->frame;
   pcm_config_param.sync_src = p_dev_state->msm_mode;
   pcm_config_param.sync = p_dev_state->sync;
   pcm_config_param.quant = p_dev_state->quant;
   pcm_config_param.ctrl_data_oe = p_dev_state->ctrl_data_oe;

   hal_pcm_config(pcm_index, pcm_dir, &pcm_config_param);

   //start the DMA engine
	if(ADSP_FAILED(result = afe_dma_mgr_start(p_dev_state->dma_dev_hdl)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d",result);
		afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
		p_dev_state->dma_dev_hdl = NULL;
		goto __bailout;
	}

	// Enabling the interrupts will be done in the dma manager

   //enable the inteface
   hal_pcm_enable(pcm_index, pcm_dir);

#ifdef AUDIOIF_REG_DUMP      
   dma_index = afe_dma_mgr_get_dma_index(p_dev_state->dma_dev_hdl);
   hal_afe_audioif_dump(LPA_IF_PCM, pcm_index, pcm_dir, dma_index, LPA_IF_START);
#endif

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PCM port successfully opened");

   __bailout:

   return result;
}


/**
  @brief Stop the data transfer from pcm port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_driver_stop(void *afe_drv)
{
	ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;

   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle Null");
      return ADSP_EFAILED;
   }

#ifdef AUDIOIF_REG_DUMP  
   uint32_t dma_index = afe_dma_mgr_get_dma_index(p_dev_state->dma_dev_hdl);
#endif

   // disable and clear dma interrupt
   afe_dma_mgr_disable_clear_dma_int(p_dev_state->dma_dev_hdl);

   uint32_t pcm_index = (uint32_t)p_dev_state->intf;
   uint32_t pcm_dir =  (uint32_t)p_dev_state->direction;

   hal_pcm_disable(pcm_index, pcm_dir);

   //  free any shared resource
   uint32_t mux_index = (uint32_t)p_dev_state->intf;
   if(ADSP_FAILED(result = afe_audioif_mux_free(AUDIOIF_PCM, mux_index)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux free failed pcm %ul", p_dev_state->intf);
   }

	//stop the DMA engine
	afe_dma_mgr_stop(p_dev_state->dma_dev_hdl);

	//De-register with the DMA Manager including the isr callback
	afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
	p_dev_state->dma_dev_hdl = NULL;

#ifdef AUDIOIF_REG_DUMP    
   hal_afe_audioif_dump(LPA_IF_PCM, p_dev_state->intf, p_dev_state->direction, dma_index, LPA_IF_STOP);
   hal_afe_clock_dump(LPA_IF_PCM, p_dev_state->intf, LPA_IF_STOP);
#endif
   return result;
}


/**
  @brief Read data from the pcm DMA buffer to the client buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_driver_read(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{

	ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *)afe_drv;
   int16_t           num_channels;
   uint32_t          total_num_samples, samp;
   int32_t           *ptr_pBuf32;
   int64_t           *ptr_dst_64;
   uint16_t     		q_format_shift_factor;

   if((NULL == afe_drv) || (NULL == buffer_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / buffer pointer Null");
      return ADSP_EFAILED;
   }

   num_channels = p_dev_state->num_channels;
   q_format_shift_factor = p_dev_state->q_format_shift_factor;

   if(ADSP_FAILED(result = afe_dma_mgr_read(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
	}

   // post processing is required for the cases as below.  bit width and number of channels.
   
   /* Calculate the total number of samples*/
   total_num_samples = (num_dma_samples * num_channels);
   
   /* For Non-I2S interface, Only Q31->Q28 scaling, No channel re-ordering */
   if (p_dev_state->bytes_per_channel == 4)
   {
      /* Calculate the total number of samples*/
      total_num_samples = (num_dma_samples * num_channels);

      ptr_dst_64 = (int64_t *)buffer_ptr;

#ifdef __qdsp6__
      for(samp=total_num_samples; samp >= 2; samp -= 2)
      {
         /* Q31 to Q28 conversion, two samples at at time
         */
         (*ptr_dst_64) = Q6_P_vasrw_PR(*ptr_dst_64, q_format_shift_factor);
          ptr_dst_64++;
      }

      /* If number of samples are odd, scale the remaining sample
      */
      if(samp)
      {
         ptr_pBuf32 = (int32_t *)ptr_dst_64;

         (*ptr_pBuf32) >>= q_format_shift_factor;
      }
#else
      /*-------- Non Q6 Version ------*/

      ptr_pBuf32 = (int32_t *)buffer_ptr;

      for(int32_t i=0; i<total_num_samples; i++)
      {
         ptr_pBuf32[i] = (ptr_pBuf32[i] >> q_format_shift_factor);
      }
#endif /* __qdsp6__ */
   }
   
   return result;

}

/**
  @brief Copy data from client buffer to the pcm DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_driver_write(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
   ADSPResult result = ADSP_EOK;
   int16_t           num_channels, samp;
   uint32_t          total_num_samples;
   int32_t           *ptr_pBuf32;
   int64_t           *ptr_dst_64;
   uint16_t        	q_format_shift_factor;

   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;

   if((NULL == afe_drv) || (NULL == buffer_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / buffer pointer Null");
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
  @brief Routine to read the properites for PCM 

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;


   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle Null");
      return ADSP_EFAILED;
   }
      
   switch(prop_id)
   {
      case AFE_DEVICE_PORT_PROP:
      {
         afe_dev_port_config_t  *pcm_port_cfg_data_ptr = (afe_dev_port_config_t  *)prop_ptr;

         if (payload_size < sizeof(afe_dev_port_config_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM get property fail: Invalid payload size: %ld, pcm port index:%d, dir:%d", \
              payload_size, p_dev_state->intf, p_dev_state->direction);
              result = ADSP_EBADPARAM;
         }
        
         result = afe_pcm_get_dev_port_cfg(afe_drv, pcm_port_cfg_data_ptr);
         break;
      }
      case AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP:
      {
         afe_dev_timestamp_info_t  *time_stamp_info_ptr = (afe_dev_timestamp_info_t *) prop_ptr;
      
         if (payload_size < sizeof(afe_dev_timestamp_info_t))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM get property fail: Invalid payload size: %ld, pcm port index:%d, dir:%d", \
              payload_size, p_dev_state->intf, p_dev_state->direction);
              result = ADSP_EBADPARAM;
         }
        
         result = afe_pcm_get_dma_timestamp(afe_drv, time_stamp_info_ptr);
         break;
      }
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }
   
   return result;


}   

/**
  @brief Get PCM dev port config to fill the info to the port structure.
        NOTE: This is the new function to remove port structure access inside device driver right after config.
                   Upper layer should call this function to fill the port structure.
   
  @param[in] afe_drv pointer for afe device port structure
  @param[out] pcm_port_cfg_data_ptr pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_get_dev_port_cfg (void *afe_drv, afe_dev_port_config_t  *pcm_port_cfg_data_ptr)
{
   ADSPResult result = ADSP_EOK;
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;

   if(PCM_DRIVER_INIT != pcm_driver.driver_state)
   {
      return ADSP_EFAILED;
   }  
   
   if((NULL == afe_drv) || (NULL == pcm_port_cfg_data_ptr))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle / params pointer is null");
      return ADSP_EFAILED;
   }   

   // this part is port structure which we'd better access this in the driver.
   // we can have new function such as get function which provide information related to this.

   // port data sturtcure to be filled.
   pcm_port_cfg_data_ptr->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
   pcm_port_cfg_data_ptr->q_format_shift_factor  = p_dev_state->q_format_shift_factor;
   pcm_port_cfg_data_ptr->bytes_per_channel =  p_dev_state->bytes_per_channel;
   pcm_port_cfg_data_ptr->bit_width = p_dev_state->bit_width;
   pcm_port_cfg_data_ptr->channels = p_dev_state->num_channels;
   pcm_port_cfg_data_ptr->sample_rate = p_dev_state->sample_rate;
   pcm_port_cfg_data_ptr->is_interleaved = TRUE;

   return result;
}



/**
  @brief Routine to read the hardware timestamp registers for PCM

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr)
{
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_drv;
   uint64_t dma_int_timestamp = 0;
   uint32_t fifo_count = 0;
   ADSPResult result = ADSP_EOK;

   if(NULL == afe_drv)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to pcm device handle Null");
      return ADSP_EFAILED;
   }
  
   if(ADSP_FAILED(result = afe_dma_mgr_read_timestamp_fifo_count(p_dev_state->dma_dev_hdl, &dma_int_timestamp, &fifo_count)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_read_timestamp_fifo_count failed intf[%d] dir[%d]" 
            ,p_dev_state->intf, p_dev_state->direction);
   }
   else
   {
     timestamp_info_ptr->dma_int_timestamp = avtimer_drv_convert_hw_tick_to_time(dma_int_timestamp);
      
      /* FIFO count is in 32 bit words. Convert it to samples as per port byter per sample */
      timestamp_info_ptr->fifo_samples = fifo_count * sizeof(uint32_t)/p_dev_state->bytes_per_channel;
   }

   return result;
}

/**
  @brief Callback routine for the pcm interrupt

  @param[in] afe_drv pointer to the pcm dev port state structure

  @param[in] event_mask indicates if it is dma interrupt or error.

  @return  none

*/
static void afe_pcm_int_callback(void* afe_drv, uint32 event_mask)
{
   pcm_port_state_t         *dev_state_ptr = (pcm_port_state_t *)afe_drv;;

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
         dev_state_ptr->pcm_intr_cnt++;
         dev_state_ptr->cb_info.afe_dev_int_cb(dev_state_ptr->cb_info.cb_arg);
      }
   }
}

/**
  Copy data from the pcm DMA buffer to the client buffer
  This  function which is called by dma manager 

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address				pointer to the DMA buffer
  @param[in]  dst_address				pointer to the client buffer

  @return
  None

  @dependencies
  None.
 */
static void afe_pcm_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address)
{
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *)afe_driver_context;


   if((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address)  )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
      return;
   }

   uint32_t length = p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel* p_dev_state->num_channels;

   memscpy(dst_address, length ,src_address, length);
   
	return;
}

/**
  Copy data from client buffer to the pcm Output DMA buffer
  write function which is called by dma manager

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address                    pointer to the client buffer
  @param[in]  dst_address                    pointer to the DMA buffer

  @return
  None

  @dependencies
  None.
 */

static void afe_pcm_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address)
{
   pcm_port_state_t *p_dev_state = (pcm_port_state_t *) afe_driver_context;

   if((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address)  )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
      return;
   }

   uint32_t length = p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel * p_dev_state->num_channels;
   
   afe_dma_buff_copy((int8_t *)dst_address, length ,(int8_t *)src_address, length);

   return;
}


/**
  @brief save pcm port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] pcm_cfg_ptr   pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error
*/
static ADSPResult afe_pcm_dev_config_port (pcm_port_state_t* p_dev_state, afe_param_id_pcm_cfg_t *pcm_cfg_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint16_t index=0, slot_no=0, num_slot_valid = 0, error_check_flag = 0;
   uint16_t index_cur=0, slot_no_cur=0;

   // Printing the device configuration information
   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"Device configuration PCM Port");
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Aux Mode: %d Bit width: %d ctrl data out enable: %d",pcm_cfg_ptr->aux_mode, pcm_cfg_ptr->bit_width, pcm_cfg_ptr->ctrl_data_out_enable);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Frame Setting: %d Num Channels: %d PCM cfg minor version: %lu",pcm_cfg_ptr->frame_setting, pcm_cfg_ptr->num_channels, pcm_cfg_ptr->pcm_cfg_minor_version);
   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"Quant type: %d Sample Rate: %lu Sync Src: %d", pcm_cfg_ptr->quant_type, pcm_cfg_ptr->sample_rate, pcm_cfg_ptr->sync_src);
    
   // parse the bit width information
   if (16 != pcm_cfg_ptr->bit_width)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x, pcm intf index: %x", pcm_cfg_ptr->bit_width, p_dev_state->intf);
      return ADSP_EUNSUPPORTED;
   }

   p_dev_state->bit_width = pcm_cfg_ptr->bit_width;

   p_dev_state->bytes_per_channel = (p_dev_state->bit_width > 16) ? 4 : 2;


   /* Determining the qformat shift factor based on bit width. We use this shift factor for 24bit and 32bit only*/
   if (32 == p_dev_state->bit_width)
   {
      p_dev_state->q_format_shift_factor = 0;
   }
   else
   {
      p_dev_state->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
   } 


   // parse quantization type
   /**
          * This determine the bit width per slot.
          * 8bits:  A_LAW_WITHOUT_PADDING, MU_LAW_WITHOUT_PADDING
          * 16bits : LINEAR_WITHOUT_PADDING, LINEAR_WITH_PADDING, A_LAW_WITH_PADDING, MU_LAW_WITH_PADDING 
          */   
   switch (pcm_cfg_ptr->quant_type)
   {
      case AFE_PORT_PCM_LINEAR_NOPADDING:
           p_dev_state->quant = LINEAR_WITHOUT_PADDING;
           break;
      case AFE_PORT_PCM_ALAW_NOPADDING:
      case AFE_PORT_PCM_MULAW_NOPADDING:
      case AFE_PORT_PCM_ALAW_PADDING:
      case AFE_PORT_PCM_MULAW_PADDING:
      case AFE_PORT_PCM_LINEAR_PADDING:
      default:
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported quant_type: %x, pcm intf index: %x", pcm_cfg_ptr->quant_type, p_dev_state->intf);
            return ADSP_EUNSUPPORTED;
         }
   }

   // parse channel info
   if ((pcm_cfg_ptr->num_channels > 4) || (pcm_cfg_ptr->num_channels == 0))
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %d for pcm intf index 0x%x", pcm_cfg_ptr->num_channels, p_dev_state->intf);
       return ADSP_EUNSUPPORTED;
   }
   p_dev_state->num_channels = pcm_cfg_ptr->num_channels;

   //For PCM interface configure, we support two types of use case now:
   //Case 1: “num_channels” setting is matched with “slot_number_mapping[]” setting
   //        Single channel and single slot
   //        Multi-channel and multi-slots
   
   //Case 2: “num_channels” is always 1. “slot_number_mapping[]” setting can be 2 or more slots 
   //        Use two 8 KHz slots for 16 KHz mono data is in this group. 
   for (index=0; index < AFE_AUX_PCM_MAX_NUM_SLOTS; index++)
   {
      slot_no =  pcm_cfg_ptr->slot_number_mapping[index];
   
      if ((1 <= slot_no )&&( 32 >= slot_no))
      {
         if(1 == error_check_flag)
         {
            //Error check 1: The valid slot value settiing should be always continuely placed in form index 0
            //Return back for example if slot_number_mapping[0] = Valid_Slot, slot_number_mapping[1] = 0, slot_number_mapping[2] = Valid_Slot
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported, all unused slot_number_mapping should be set as 0, slot_number_mapping[%d]: %d",index, pcm_cfg_ptr->slot_number_mapping[index]);
            return ADSP_EUNSUPPORTED;
         }
   
         MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"slot_number_mapping[%d]: %d",index, pcm_cfg_ptr->slot_number_mapping[index]);
   
         p_dev_state->slot_number_mapping[index] = slot_no-1;
         num_slot_valid++;
      }
      else
      {
         error_check_flag = 1;
         //reset all unused slot_number_mapping[] to 0
         p_dev_state->slot_number_mapping[index] = 0;
      }
   }

   //Error check 2: for multiple channel case, valid num of slot setting should be equal to num of channel setting.
   if((1 < p_dev_state->num_channels) && (p_dev_state->num_channels != num_slot_valid))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported valid slot number and num channel not matched, num_channel %ld, valid_slot_no %d", p_dev_state->num_channels, num_slot_valid);
      return ADSP_EUNSUPPORTED;
   }

    //Error check 3: if the same slot setting is used two times
    num_slot_valid = (num_slot_valid >= p_dev_state->num_channels) ? num_slot_valid : p_dev_state->num_channels;

    for (index=0; index < num_slot_valid; index++)
    {
       slot_no_cur = p_dev_state->slot_number_mapping[index];

       for (index_cur=0; index_cur < num_slot_valid; index_cur++)
       {
          if((index_cur != index) && (slot_no_cur == p_dev_state->slot_number_mapping[index_cur]))
          {
             MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported, client uses the same slot setting for two times");
             return ADSP_EUNSUPPORTED;
          }
       }
    }

    p_dev_state->slot_num = num_slot_valid;

    // parse sync_src information
    switch (pcm_cfg_ptr->sync_src)
    {
        case AFE_PORT_PCM_SYNC_SRC_EXTERNAL:
            p_dev_state->msm_mode = PCM_MSM_MODE_SLAVE;
            break;

        case AFE_PORT_PCM_SYNC_SRC_INTERNAL:
        default:
            p_dev_state->msm_mode = PCM_MSM_MODE_MASTER;
            break;
    }

    // parse aux_mode information
    switch (pcm_cfg_ptr->aux_mode)
    {
        case AFE_PORT_PCM_AUX_MODE_PCM:
            p_dev_state->sync = PCM_MODE;
            break;

        case AFE_PORT_PCM_AUX_MODE_AUX:
        default:
            p_dev_state->sync = AUX_MODE;
            break;
    }

    // parse frame setting info
    switch (pcm_cfg_ptr->frame_setting)
    {
       case AFE_PORT_PCM_BITS_PER_FRAME_8:
           p_dev_state->frame = BITS_PER_FRAME_8;
           break;

       case AFE_PORT_PCM_BITS_PER_FRAME_16:
           p_dev_state->frame = BITS_PER_FRAME_16;
           break;

       case AFE_PORT_PCM_BITS_PER_FRAME_32:
           p_dev_state->frame = BITS_PER_FRAME_32;
           break;

       case AFE_PORT_PCM_BITS_PER_FRAME_64:
            p_dev_state->frame = BITS_PER_FRAME_64;
            break;

       case AFE_PORT_PCM_BITS_PER_FRAME_128:
           p_dev_state->frame = BITS_PER_FRAME_128;
           break;

       case AFE_PORT_PCM_BITS_PER_FRAME_256:
            p_dev_state->frame = BITS_PER_FRAME_256;
            break;
    }

    // parse frame setting info
     p_dev_state->ctrl_data_oe = pcm_cfg_ptr->ctrl_data_out_enable;

    // parse the sample rate
    if ((AFE_PORT_SAMPLE_RATE_8K != pcm_cfg_ptr->sample_rate)&&
        (AFE_PORT_SAMPLE_RATE_16K != pcm_cfg_ptr->sample_rate)&&
        (AFE_PORT_SAMPLE_RATE_24K != pcm_cfg_ptr->sample_rate)&&
        (AFE_PORT_SAMPLE_RATE_32K != pcm_cfg_ptr->sample_rate)&&
        (AFE_PORT_SAMPLE_RATE_48K != pcm_cfg_ptr->sample_rate))
    {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, pcm intf index: %x", pcm_cfg_ptr->sample_rate, p_dev_state->intf);
       return ADSP_EUNSUPPORTED;
    }
    p_dev_state->sample_rate = pcm_cfg_ptr->sample_rate;   

   
   return result;
}


/**
  @brief calculate the number of dma buffer.
            The default value is 2 as ping-pong buffer.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  the number of buffer (0 in error case)

*/
static uint32 afe_pcm_get_num_dma_buffer(pcm_port_state_t *p_dev_state)
{
   uint32 num_dma_buffer = DEFAULT_PCM_DMA_BUFFERS;
   uint32 dma_buf_size_in_bytes_per_intr; 
 
   dma_buf_size_in_bytes_per_intr = p_dev_state->bytes_per_channel*  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;
   
   /*twice the length of each buffer - ping-pong*/
   uint32 circ_buf_length = num_dma_buffer * dma_buf_size_in_bytes_per_intr;
  
  //Iterate to find the next higher DMA buffer size which is 16 byte aligned
   while((circ_buf_length & PCM_DMA_BUFFER_16BYTE_ALIGNMENT) && (num_dma_buffer < MAX_PCM_DMA_BUFFERS))
   {
      //Double the number of DMA buffers
      num_dma_buffer <<= 1 ;

      //Double the DMA buf size
      circ_buf_length = dma_buf_size_in_bytes_per_intr * num_dma_buffer;
    }

    if(num_dma_buffer > MAX_PCM_DMA_BUFFERS)
      {
         num_dma_buffer = 0;
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "DMA total buffer length (%lu) is not 16 byte aligned", circ_buf_length);
   }
   
   
   return num_dma_buffer;

}


/**
  @brief calculate the watermark value.
            The default value is 8 dwords.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  watermark value which is 1 or 8
*/
static uint32 afe_pcm_get_watermark(pcm_port_state_t *p_dev_state)
{
   uint32 water_mark = DEAFULT_PCM_WATERMARK;
   uint32 dma_buf_size_in_bytes_per_intr; 
   uint32 dma_buf_size_in_dword_per_intr;

   /* check if burst mode can be enabled
       * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
       * then disable the burst mode and use watermark as 1 dword
       */
   dma_buf_size_in_bytes_per_intr = p_dev_state->bytes_per_channel*  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;

   dma_buf_size_in_dword_per_intr = dma_buf_size_in_bytes_per_intr >> 2;

   //8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
   //The burst mode needs to be disabled for this case for latency and drift detection issue
   if((DEAFULT_PCM_WATERMARK >= dma_buf_size_in_dword_per_intr) ||
         (dma_buf_size_in_dword_per_intr & 0x3)  )
   {
      water_mark = 1;
   }

   return water_mark;
}



/**
  @brief Obtain the chip specific information for initializing the driver

  @param[in] pcm_hw_info_ptr pointer for PCM Device config structure

  @return  ADSP_EOK on success, an error code on error

*/
static ADSPResult afe_pcm_driver_device_cfg_init(pcm_dev_cfg_struct_t * pcm_hw_info_ptr)
{
   DALSYSPropertyVar prop_var;
   lpasshwio_prop_lpaif_struct_t  *lpaif_prop_dev_cfg_ptr = NULL;
   const char *dev_id="LPAIF";
   uint32 i;

   DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

   /* Get device handle */
   if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for PCM");
      return ADSP_EFAILED;
   }
	else
   {
      /* Read property ptr */
      if( DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "LpaifPropStructPtr", 0, &prop_var) )
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting PCM Property Struct Ptr");
         return ADSP_EFAILED;
      }
      else
      {
         lpaif_prop_dev_cfg_ptr = (lpasshwio_prop_lpaif_struct_t *)prop_var.Val.pStruct;

         if (NOT_SUPPORTED == lpaif_prop_dev_cfg_ptr->pcm_prop.is_supported)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PCM interface is not supported");
            return ADSP_EUNSUPPORTED;
         }

         /* hw revision */
         pcm_hw_info_ptr->hw_revision = lpaif_prop_dev_cfg_ptr->hw_revision;

         /* PCM register base address */
         pcm_hw_info_ptr->pcm_reg_base = lpaif_prop_dev_cfg_ptr->lpaif_reg_base;
         /* PCM register page size */
         pcm_hw_info_ptr->pcm_reg_size = lpaif_prop_dev_cfg_ptr->lpaif_reg_size;
         /* PCM register offset */
         pcm_hw_info_ptr->pcm_reg_offset = lpaif_prop_dev_cfg_ptr->pcm_prop.pcm_reg_offset_addr;
         
         /* number of pcm interfaces */         
         pcm_hw_info_ptr->inf_type_cnt = lpaif_prop_dev_cfg_ptr->pcm_prop.inf_type_cnt;
         for(i=0; i < MAX_PCM_TYPE_NUM; i++)
         {
            pcm_hw_info_ptr->inf_type_list[i] = lpaif_prop_dev_cfg_ptr->pcm_prop.inf_type_list[i];
            pcm_hw_info_ptr->inf_type_data_line_caps[i] = lpaif_prop_dev_cfg_ptr->pcm_prop.inf_type_data_line_caps[i];
            pcm_hw_info_ptr->inf_type_direction[i] = lpaif_prop_dev_cfg_ptr->pcm_prop.inf_type_direction[i];
         }
      }
   }

	return ADSP_EOK;
}


