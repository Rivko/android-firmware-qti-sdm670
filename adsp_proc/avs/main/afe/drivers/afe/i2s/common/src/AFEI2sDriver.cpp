/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sDriver.cpp#3 $
$DateTime: 2017/12/07 12:24:07 $
$Author: pwbldsvc $
$Change: 15009245 $
$Revision: #3 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

 Copyright (c) 2013-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/i2s/common/src/AFEI2sDriver.cpp#3 $ $DateTime: 2017/12/07 12:24:07 $ $Author: pwbldsvc $

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
#include "AFEI2sDriver.h"
#include "AFEI2sDriver_i.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalI2s.h"
#include "AFEAudioIfCommon.h"
#include "AFEDeviceDriverInterface.h"
#include "AFEHalDump.h"
#include "AFEI2sGroupDevice_i.h"

/*=====================================================================
 Globals
 ======================================================================*/
//Note: if it is possible, decouple driver from service layer for better modulization.

//global variable for i2s driver to share any variable common to all ports for example device config, etc.
static i2s_drv_state_t    i2s_driver;

//note: there is no shared resource access between i2s ports after init and before deinit.
//        however, the mux and dma is shared between ports for i2s/pcm/hdmi legacy
//        These will be required to be protected in that module.


/*=====================================================================
 Static Functions
 ======================================================================*/
static ADSPResult afe_i2s_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size);
static ADSPResult afe_get_i2s_dev_port_cfg(void *afe_drv, afe_dev_port_config_t  *i2s_port_cfg_data_ptr);
static ADSPResult afe_i2s_get_param(void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);
static ADSPResult afe_i2s_driver_start(void *afe_drv, dev_start_params_t *i2s_start_params_ptr);
static ADSPResult afe_i2s_driver_stop(void *afe_drv);
static ADSPResult afe_i2s_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr);
static ADSPResult afe_i2s_set_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size);
static ADSPResult afe_i2s_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size);
static void afe_i2s_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address);
static void afe_i2s_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address);
static ADSPResult afe_dev_config_i2s_port(i2s_port_state_t *p_dev_state, afe_param_id_i2s_cfg_t *pI2sCfg);
static void afe_i2s_int_callback(void *afe_drv, uint32 event_mask);
static ADSPResult afe_i2s_driver_read(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static ADSPResult afe_i2s_driver_write(afe_drv_h_t afe_drv,  int16_t *buffer_ptr, uint32_t num_dma_samples);
static uint32 afe_i2s_get_watermark(i2s_port_state_t *p_dev_state);
static uint32 afe_i2s_get_num_dma_buffer(i2s_port_state_t *p_dev_state);
static ADSPResult afe_i2s_driver_device_cfg_init(i2s_dev_cfg_struct_t *i2s_hw_info_ptr);
static ADSPResult afe_i2s_init_IEC60958_ch_status_table(i2s_port_state_t *p_dev_state);
static ADSPResult afe_i2s_deinit_IEC60958_ch_status_table(i2s_port_state_t *p_dev_state);
static void afe_i2s_fill_IEC60958_data(i2s_port_state_t *p_dev_state, uint16_t *src, uint32_t *dst);
static void afe_i2s_copy_IEC60958_data(i2s_port_state_t *p_dev_state, uint32_t *src, uint16_t *dst);
static void afe_i2s_fill_hdmi_ch_status_table(uint32_t *table_ptr, int32_t index, uint8_t ch_status);
static void afe_i2s_hdmi_copy_audio_sample(int16_t *p_dst, int32_t *p_src, int32_t sample_bit_width);
static void afe_i2s_parse_IEC60958_subframe(i2s_IEC60958_subframe_info_t *p_subframe, int32_t *data, int16_t *dst, int32_t sample_bit_width);

typedef struct
{
  afe_dev_port dev_port;
  afe_dev_avt_drift_t avt_drift;
}afe_aif_object_t;

i2s_port_state_t *i2s_port_state_arr_ptr = NULL;
afe_aif_object_t *afe_aif_obj_array_ptr = NULL;
uint32_t i2s_total_port_count;
int8_t *port_init_mem_blob_ptr = NULL;


static afe_driver_func_tbl_t sink_ftbl =
{

    afe_i2s_driver_write,
    afe_i2s_set_param,
    afe_i2s_get_param,
    afe_i2s_driver_start,
    afe_i2s_driver_stop,
    afe_i2s_set_properties,
    afe_i2s_get_properties,
};

static afe_driver_func_tbl_t source_ftbl =
{
    afe_i2s_driver_read,
    afe_i2s_set_param,
    afe_i2s_get_param,
    afe_i2s_driver_start,
    afe_i2s_driver_stop,
    afe_i2s_set_properties,
    afe_i2s_get_properties,
};


static uint32_t afe_i2s_driver_next_even_value(uint32_t num)
{
  return (num + 1) & 0xFFFFFFFEUL;
}

/* Return the Rx/Tx driver struct obj index based on i2s h/w interface index */
static inline uint32_t afe_i2s_get_drv_obj_idx(uint32_t i2s_hw_intf_idx, uint32_t i2s_dir)
{
  return (2*i2s_hw_intf_idx + i2s_dir);
}

/* Return the driver index of the pair drv obj, return Rx for Tx and Tx for Rx.*/
static inline uint32_t afe_i2s_get_pair_drv_obj_idx(uint32_t i2s_hw_intf_idx, uint32_t i2s_dir)
{
  return (2*i2s_hw_intf_idx + !i2s_dir);
}


/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform I2S Driver Initialization
            The driver init function does
                  1. get hardware device config for i2s
                  2. get the virtual addres for hardware registers
                  3. get the avtimer tick frequency which will be used for converting the dma timestamp to usec.
                  4. initialize all the device driver structures 

  @param[in]
  None

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_i2s_driver_init(void)
{
  ADSPResult   result = ADSP_EOK;

  memset(&i2s_driver, 0, sizeof(i2s_drv_state_t));
  i2s_dev_cfg_struct_t *i2s_hw_info_ptr = &i2s_driver.i2s_hw_info;

  // 1. read i2s hwd dev config and cache it to the i2s_driver global state
  if (ADSP_FAILED(result = afe_i2s_driver_device_cfg_init(i2s_hw_info_ptr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error afe_i2s_driver_device_cfg_init");
    return result;
  }

  // 2. create memory region for lpass i2s interface hw
  //   This information is need to be saved to  i2s_driver global state

  // i2s register interface
  if (ADSP_FAILED(result = HwdDevCfg_GetVirtAddress(&i2s_driver.i2s_qmem_region,
                                                    i2s_hw_info_ptr->i2s_reg_base, i2s_hw_info_ptr->i2s_reg_size, &i2s_driver.i2s_virt_addr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for i2s register base");
    return result;
  }
  else
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "i2s virtual addr 0x%x phy addr 0x%x",
          (unsigned int)i2s_driver.i2s_virt_addr, (unsigned int)i2s_hw_info_ptr->i2s_reg_base);
  }

  // 3. do the hal init which save the physical address and virtual address offset.
  hal_i2s_init(i2s_hw_info_ptr->hw_revision, i2s_hw_info_ptr->i2s_reg_base, i2s_driver.i2s_virt_addr, i2s_hw_info_ptr->i2s_reg_offset, i2s_hw_info_ptr->int_i2s_reg_offset);

  //4. initialize all the device driver structures

  /* Initialize the driver mutex */
  qurt_elite_mutex_init(&i2s_driver.i2s_drv_mutex);

  i2s_driver.driver_state = I2S_DRIVER_INIT;

  return ADSP_EOK;
}

/**
  @brief Perform I2S Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_i2s_driver_deinit(void)
{

  if (I2S_DRIVER_INIT != i2s_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  /* release all resource allocated during init*/
  HwdDevCfg_DestoryVirtAddress(i2s_driver.i2s_qmem_region);

  /* De-initialize the driver mutex */
  qurt_elite_mutex_destroy(&i2s_driver.i2s_drv_mutex);

  // change the driver state.
  i2s_driver.driver_state = I2S_DRIVER_DEINIT;

  return ADSP_EOK;
}

/**
  @brief Perform I2S port initialization
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
ADSPResult afe_i2s_port_init(aud_stat_afe_svc_t *afe_svc_ptr)
{
  ADSPResult      result;
  afe_dev_port    *dev_port_ptr = NULL;
  uint16_t        port_id;
  uint32_t dir;
  i2s_port_state_t *p_dev_state = NULL;
  afe_driver_func_tbl_t *ftable_ptr;
  afe_dev_avt_drift_t *avt_drift_ptr = NULL;
  uint32_t port_structs_aligned_size = 0;
  uint32_t i2s_port_state_aligned_size =0;
  uint32_t intf_port_count = 0;
  uint32_t intf_iter, intf_idx;
  uint32_t *intf_port_list = NULL;
  uint32_t mem_blob_total_size =0;

  if (I2S_DRIVER_INIT != i2s_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  i2s_dev_cfg_struct_t *i2s_hw_info_ptr = &i2s_driver.i2s_hw_info;

  /* Get the total port count */
  i2s_total_port_count =0;
  for (intf_iter = 0; intf_iter < i2s_hw_info_ptr->inf_type_cnt; intf_iter++)
  {
    i2s_total_port_count += i2s_hw_info_ptr->intf_port_info[intf_iter]->num_ports;
  }

  /* Allocate memory for all the ports in the interface */
  port_structs_aligned_size = QURT_ELITE_ROUNDUP_MULTIPLE4(sizeof(afe_aif_object_t)) * i2s_total_port_count ;

  /* Allocate memory for i2s port state structures
     Each interface needs two port structs, for RX and TX. */
  i2s_port_state_aligned_size = QURT_ELITE_ROUNDUP_MULTIPLE4(sizeof(i2s_port_state_t)) * i2s_hw_info_ptr->inf_type_cnt;
  i2s_port_state_aligned_size = i2s_port_state_aligned_size * 2;

  mem_blob_total_size = i2s_port_state_aligned_size + port_structs_aligned_size;

  /* Allocate memory required for all the ports structres */
  port_init_mem_blob_ptr = (int8_t *)qurt_elite_memory_malloc(mem_blob_total_size, QURT_ELITE_HEAP_DEFAULT);

  if(NULL == port_init_mem_blob_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Device port initalization: ports memory allocation failure");
    return ADSP_EFAILED;
  }
  memset(port_init_mem_blob_ptr, 0, mem_blob_total_size);

  /* copy the global mem blob addr to temp variable */
  int8_t* temp_mem_blob_ptr = port_init_mem_blob_ptr;

  afe_aif_obj_array_ptr = (afe_aif_object_t *) temp_mem_blob_ptr;
  afe_aif_object_t *cur_aif_obj_ptr = afe_aif_obj_array_ptr;

  temp_mem_blob_ptr += port_structs_aligned_size;

  i2s_port_state_arr_ptr = (i2s_port_state_t *)temp_mem_blob_ptr;
  i2s_port_state_t *cur_port_state_ptr = i2s_port_state_arr_ptr;

  /* Init ports, for all the interfaces defined in device config file */
  for (intf_iter = 0; intf_iter < i2s_hw_info_ptr->inf_type_cnt; intf_iter++)
  {
    /* Get I2S interface index */
    intf_idx = i2s_hw_info_ptr->intf_port_info[intf_iter]->intf_idx;

    /* Get number of ports for each interface */
    intf_port_count = i2s_hw_info_ptr->intf_port_info[intf_iter]->num_ports;

    /* Get the port list */
    intf_port_list = i2s_hw_info_ptr->intf_port_info[intf_iter]->port_list;

    /* Intialize the ports associated with the interface */
    for(uint16_t idx =0; idx < intf_port_count; idx++,cur_aif_obj_ptr++)
    {
      /* get port ID and direction */
      port_id = intf_port_list[idx];
      dir = port_id & AFE_PORT_DIR_MASK;

      /* get dev port memory and init the device port */
      dev_port_ptr  = (afe_dev_port *) (&cur_aif_obj_ptr->dev_port);
      if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to create port = %d, aborting", port_id);
        return result;
      }

      /* Get the port type */
      if( IS_IT_TX_PORT_AFE_ID(port_id) )
      {
        ftable_ptr = &source_ftbl;
      }
      else
      {
        ftable_ptr = &sink_ftbl;
      }
      dev_port_ptr->afe_drv_ftbl = ftable_ptr;
      dev_port_ptr->avt_drift_params.avtimer_latch_type = TIMESTAMP_LATCH_TYPE_HARDWARE;
      afe_svc_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(port_id)] = dev_port_ptr;

      /* Allocate port state memory for the given (intf, dir) if not allocated previously.
         All the ports IDs with same (intf# & dir), will have one port state struct */
      if(NULL == i2s_driver.p_i2s_intf_state[afe_i2s_get_drv_obj_idx(intf_idx, dir)])
      {
        p_dev_state   = cur_port_state_ptr++;
        p_dev_state->intf = (i2s_intf_idx_t) intf_idx;
        p_dev_state->direction = dir;

        /* Store the i2s port state pointer to driver structure in global driver struct */
        i2s_driver.p_i2s_intf_state[afe_i2s_get_drv_obj_idx(intf_idx, dir)] = p_dev_state;
      }
      else
      {
        /* If port state is allocated for the given (intf, dir), just return the pointer */
        p_dev_state = i2s_driver.p_i2s_intf_state[afe_i2s_get_drv_obj_idx(intf_idx, dir)];
      }
      dev_port_ptr->afe_drv = (void *)p_dev_state;

      /* Get device avt drift pointer */
      avt_drift_ptr = (afe_dev_avt_drift_t *) (&cur_aif_obj_ptr->avt_drift);
      dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = avt_drift_ptr;
    }
  }

  return ADSP_EOK;
}



/**
  @brief Perform I2S port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_i2s_port_deinit(aud_stat_afe_svc_t *afe_svc_ptr)
{
  // check the driver state
  if (I2S_DRIVER_INIT != i2s_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  // free resources if there is anything is allocated during i2s port init.
  afe_dev_port    *dev_port_ptr = NULL;

  // Interate through the aif object array and de initialize all the ports
  afe_aif_object_t *cur_aif_obj_ptr = afe_aif_obj_array_ptr;
  for (uint16_t port_itr = 0; port_itr < i2s_total_port_count; port_itr++,cur_aif_obj_ptr++)
  {
    dev_port_ptr = &cur_aif_obj_ptr->dev_port;
    if (NULL != dev_port_ptr)
    {
      afe_svc_deinit_port(dev_port_ptr);
    }
  }

  /* Free memory which was dynamically allocated for port structures in port_init */
  qurt_elite_memory_free(port_init_mem_blob_ptr);

  return ADSP_EOK;
}


/**
  @brief Set i2s port config
               shoudn't pass any of afe_dev_port_t structure type from this point to make driver independent from service layer.

  @param[in] afe_drv pointer for AFE dev port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_set_param(void *afe_drv, uint32_t param_id, int8_t *cfg_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  // check the driver state
  if (I2S_DRIVER_INIT != i2s_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  if ((NULL == afe_drv) || (NULL == cfg_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / params pointer is null");
    return ADSP_EFAILED;
  }

  switch (param_id)
  {
    case AFE_PARAM_ID_I2S_CONFIG:
    {
      // choose the appropriate config version
      uint32_t i2s_cfg_version = (uint32_t) * (cfg_ptr);
      if (i2s_cfg_version > AFE_API_VERSION_I2S_CONFIG)
      {
        i2s_cfg_version = AFE_API_VERSION_I2S_CONFIG;
      }

      switch (i2s_cfg_version)
      {
        case AFE_API_VERSION_I2S_CONFIG:
        {
          afe_param_id_i2s_cfg_t *i2s_cfg_ptr =  (afe_param_id_i2s_cfg_t *)cfg_ptr;

          if (payload_size < sizeof(afe_param_id_i2s_cfg_t))
          {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Cfg fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
                  payload_size, p_dev_state->intf, p_dev_state->direction);
            result = ADSP_EBADPARAM;
          }
          else
          {
            result = afe_dev_config_i2s_port(p_dev_state, i2s_cfg_ptr);
            p_dev_state->dev_cfg_minor_ver = i2s_cfg_version;
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
  @brief Get I2S port config

  @param[in] afe_drv pointer for afe device port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_get_param(void *afe_drv, uint32_t param_id, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  if ((NULL == afe_drv) || (NULL == param_buf_ptr) ||  (NULL == params_buffer_len_req_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / params pointers are null");
    return ADSP_EFAILED;
  }

  switch (param_id)
  {
    case AFE_PARAM_ID_I2S_CONFIG:
    {
      switch (p_dev_state->dev_cfg_minor_ver)
      {
        case AFE_API_VERSION_I2S_CONFIG:
        {
          *params_buffer_len_req_ptr = sizeof(afe_param_id_i2s_cfg_t);
          if (*params_buffer_len_req_ptr > payload_size)
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: I2S cfg required size = %d, given size = %lu",
                  *params_buffer_len_req_ptr, payload_size);
            return ADSP_ENOMEMORY;
          }
          afe_param_id_i2s_cfg_t  *i2s_cfg_ptr = (afe_param_id_i2s_cfg_t *)param_buf_ptr;

          /* Populate the payload parameters */
          i2s_cfg_ptr->i2s_cfg_minor_version = p_dev_state->dev_cfg_minor_ver;
          i2s_cfg_ptr->bit_width    = p_dev_state->bit_width;
          i2s_cfg_ptr->channel_mode = p_dev_state->channel_mode;
          i2s_cfg_ptr->mono_stereo  = 0;
          i2s_cfg_ptr->ws_src       = p_dev_state->msm_mode;
          i2s_cfg_ptr->sample_rate  = p_dev_state->sample_rate;
          i2s_cfg_ptr->data_format  = p_dev_state->data_format;
        }
        break;
        default:
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, i2s intf : %d",
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
  @brief Validate the mi2s date line and direction configuration 
         against device config

  @param[in] p_dev_state; Pointer to interface state obj

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_validate_cfg_param(i2s_port_state_t *p_dev_state)
{
  ADSPResult              result = ADSP_EOK;
  uint32_t                i2s_index = (uint32_t)p_dev_state->intf;
  uint32_t                i2s_dir =  (uint32_t)p_dev_state->direction;
  uint32_t                data_line_cap, i2s_dir_dev_cfg;
  i2s_port_state_t        *p_dev_state_pair;
  uint32_t                intf_total_num_ch;
  bool_t                  intf_supported = FALSE;
  uint32_t i2s_curr_idx;

  if(i2s_index <= I2S_INTERFACE_MAX)
  {
    /* Checks the interface supported by chipset or not */
    for(i2s_curr_idx = 0; i2s_curr_idx < i2s_driver.i2s_hw_info.inf_type_cnt; i2s_curr_idx++)
    {
      if( i2s_index == i2s_driver.i2s_hw_info.intf_port_info[i2s_curr_idx]->intf_idx)
      {
        intf_supported = TRUE;
        break;
      }
    }
  }

  if (FALSE == intf_supported)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid I2S index: 0x%x", i2s_index);
    return ADSP_EFAILED;
  }

  data_line_cap   = i2s_driver.i2s_hw_info.intf_port_info[i2s_curr_idx]->inf_type_data_line_caps;
  i2s_dir_dev_cfg = i2s_driver.i2s_hw_info.intf_port_info[i2s_curr_idx]->inf_type_direction;

  /* Check if the num channels is within data line cap */
  if (p_dev_state->num_channels > data_line_cap)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S num channel [%lu] exceeds h/w supported data line [%lu], i2s_idx: 0x%x",
          p_dev_state->num_channels, data_line_cap, i2s_index);

    return ADSP_EFAILED;
  }

  /* If this is uni-directional mi2s, check if the interface direction
      configured is correct */
  if (DATA_LINE_BIDIRECT != i2s_dir_dev_cfg)
  {
    if ( ((I2S_SINK == i2s_dir) && (DATA_LINE_SINK != i2s_dir_dev_cfg)) ||
        ((I2S_SOURCE == i2s_dir) && (DATA_LINE_SOURCE != i2s_dir_dev_cfg)) )
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S incorrect dir [%lu], supported dir [%lu], i2s_idx: 0x%x",
            i2s_dir, !i2s_dir, i2s_index);

      return ADSP_EFAILED;
    }
  }
  else /* Bi-directional */
  {
    /* First get the pair (Rx->Tx, Tx->Rx) interface */
    p_dev_state_pair = i2s_driver.p_i2s_intf_state[afe_i2s_get_pair_drv_obj_idx(i2s_index, i2s_dir)];

    /* Lock the driver mutex */
    qurt_elite_mutex_lock(&i2s_driver.i2s_drv_mutex);

    /* Check if the pair interface is already running */
    if (I2S_INTERFACE_START == p_dev_state_pair->state)
    {
      /* Check if the total number of channels for both Rx/Tx interface pair exceeds
            the data line cap */
      intf_total_num_ch = afe_i2s_driver_next_even_value(p_dev_state->num_channels) + \
          afe_i2s_driver_next_even_value(p_dev_state_pair->num_channels);

      if (intf_total_num_ch > data_line_cap)
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S num channel [%lu] exceeds h/w supported data line [%lu], i2s_idx: 0x%x",
              p_dev_state->num_channels, data_line_cap, i2s_index);

        result = ADSP_EFAILED;
      }
      /* Check if the pair interface has any conflicting date line usage */
      else if(p_dev_state_pair->channel_mode_mask & p_dev_state->channel_mode_mask)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S data line [%lu], already in use, i2s_idx: 0x%x",
              p_dev_state->channel_mode, i2s_index);

        result = ADSP_EFAILED;
      }
    }

    /* un-lock the driver mutex */
    qurt_elite_mutex_unlock(&i2s_driver.i2s_drv_mutex);
  }

  return result;
}


/**
  @brief Set up the data transfer for I2St port
               1. check if any shared resource is already is used. mux, data lines,...
               2. open dma manger with callback function
               3. config i2s interface
               4. start the dma
               5. enable the i2s interface

  @param[in] afe_drv pointer for afe device port structure
  @param[in]  i2s_open_params_ptr for open parameters

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_start(void *afe_drv, dev_start_params_t *i2s_start_params_ptr)
{
  ADSPResult              result = ADSP_EOK;
  i2s_port_state_t        *p_dev_state = (i2s_port_state_t *)afe_drv;
  dma_mgr_open_params_t   open_params;
  uint32_t                i2s_index = (uint32_t)p_dev_state->intf;
  uint32_t                i2s_dir =  (uint32_t)p_dev_state->direction;

  if ((NULL == afe_drv) || (NULL == i2s_start_params_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / Open Params Null");
    return ADSP_EFAILED;
  }

  if(ADSP_EOK != (result = afe_i2s_driver_validate_cfg_param(p_dev_state)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S Config param validation failed, i2s_idx: 0x%x", i2s_index);
    return ADSP_EFAILED;
  }

#ifdef AUDIOIF_REG_DUMP
  uint32_t dma_index = 0;
  hal_afe_clock_dump(LPA_IF_I2S, i2s_index, LPA_IF_START);
#endif

  // the restart se need to update the new sampling rate, clear the flag and new sampling rate...
  if (TRUE == p_dev_state->is_restart_seq)
  {
    // new sampling reate is need to be applied
    p_dev_state->sample_rate = p_dev_state->new_sampling_rate;
    // reset the flag
    p_dev_state->is_restart_seq = FALSE;
    p_dev_state->new_sampling_rate = 0;
  }

  //1. alloc any shared resource
  uint32_t mux_index = (uint32_t)p_dev_state->intf;
  if (ADSP_FAILED(result = afe_audioif_mux_alloc(AUDIOIF_I2S, mux_index)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux already used for i2s %ul", p_dev_state->intf);
    return result;
  }


  /* Set the block transfer time */
  // this will be determined during the beginning of start cmd and after the config cmd
  p_dev_state->int_samples_per_period = i2s_start_params_ptr->int_samples_per_period;

  // callback info for interrupt handling
  p_dev_state->cb_info.afe_dev_int_cb = i2s_start_params_ptr->cb_info.afe_dev_int_cb;
  p_dev_state->cb_info.cb_arg = i2s_start_params_ptr->cb_info.cb_arg;

  // dma manager open
  memset(&open_params, 0, sizeof(dma_mgr_open_params_t));

  // hardware type and dma type
  open_params.hardware_type = LPASS_HW_I2S;
  open_params.dma_type = LPASS_HW_DMA_TYPE0;

  //  open dma manger with callback function for read or write
  if (I2S_SINK == p_dev_state->direction)
  {
    open_params.dma_dir = LPASS_HW_DMA_SINK;
    open_params.dma_write_func_ptr = (dma_mgr_write_func)afe_i2s_write_dma;
  }
  else
  {
    open_params.dma_dir = LPASS_HW_DMA_SOURCE;
    open_params.dma_read_func_ptr = (dma_mgr_read_func)afe_i2s_read_dma;
  }

  open_params.sample_rate = p_dev_state->sample_rate;

  open_params.int_samples_per_period = p_dev_state->int_samples_per_period;

  /* The input_bytes_per_sample is for actual dma buffer size per sample.
          The output_bytes_per_sample is related to client buffer.
   */
  if ((AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) ||
      (AFE_LINEAR_PCM_DATA_PACKED_60958 == p_dev_state->data_format))
  {

    //allocate and init memory  channel status table for 60958 format packing
    if (ADSP_FAILED(result = afe_i2s_init_IEC60958_ch_status_table(p_dev_state)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The ch status table init has failed [i2s:%ul]", p_dev_state->intf);
      return result;
    }
  }

  // input_bytes_per_sample is related to dma buffer. if it is DATA PACKED for 60958, it is always 4 bytes.
  // it might be same as p_dev_state->bytes_per_channel or not.
  open_params.input_bytes_per_sample = p_dev_state->dma_input_bytes_per_sample;

  open_params.output_bytes_per_sample =  p_dev_state->bytes_per_channel;

  // i2s will be always opened as even number of channels like 2ch, 4ch, 6ch, 8ch...
  open_params.num_channels = afe_i2s_driver_next_even_value(p_dev_state->num_channels);

  // dma buffer type
  open_params.buff_mem_type = DMA_MGR_LPM_BUFFER;


  // 4 kinds of format is possible here. combination of linear/non-linear  packed/non-packed
  open_params.data_type = p_dev_state->data_format;

  // dma index will be allowed by dma manager
  open_params.dma_idx = AFE_INVALID_DMA_INDEX;

  // register the interrupt callback
  // dma manger will register single interrupt hanlder for i2s, pcm, hdmi, digimic
  open_params.dma_int_cb = afe_i2s_int_callback;
  open_params.afe_driver_context = afe_drv;

  // this info will be passing down to dma hal and it is only for audioif cases including pcm, i2s, etc...
  // This value should be matching to audioif dma hal config function

  /* this check is needed, because the INTF values for pri/sec/ter.. I2S and INT0/1 I2S are same for WRDMA_CTL
     registers. Also bit positions are different in WRDMA_CTL registers. So in order to update proper field,
     we need to use this variable. This is applicable only for INT I2S, and for other interfaces this
     value will be zero as part of memset of open params to zero*/
  if (IS_INTERNAL_MI2S(p_dev_state->intf)) 
  {
    open_params.ifconfig_dma_control_int  = hal_i2s_get_intf(p_dev_state->intf, i2s_dir);
  }
  else
  {
    open_params.ifconfig_dma_control  = hal_i2s_get_intf(p_dev_state->intf, i2s_dir);
  }

  // get the watermark value. if watermark is 1, then dma burst mode is supposed to be off.
  open_params.watermark = afe_i2s_get_watermark(p_dev_state);

  // dma buffer size need to be checked carefully based on alignment requirement...
   if (0 == (open_params.num_buffers = afe_i2s_get_num_dma_buffer(p_dev_state)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get req DMA num buffers");
      goto __bailout;
   }

  if (ADSP_FAILED(result = afe_dma_mgr_open(&p_dev_state->dma_dev_hdl, &open_params)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register with DMA Manager with status %d", result);
    goto __bailout;
  }

  // config i2s interface
  i2s_config_t i2s_config_param;

  /* use 32bitwidth on i2s interface for 24bit use case*/
  if (AFE_BITS_PER_SAMPLE_24 == p_dev_state->bit_width)
  {
    i2s_config_param.bit_width = AFE_BITS_PER_SAMPLE_32;
  }
  else
  {
    i2s_config_param.bit_width = p_dev_state->bit_width;
  }

  i2s_config_param.channel_mode = (uint32_t)p_dev_state->channel_mode;
  // i2s is opened as always as stereo mode regardess of number of channels.
  i2s_config_param.stereo_mode = TRUE;
  if (I2S_MSM_MODE_MASTER == p_dev_state->msm_mode)
  {
    i2s_config_param.ws_master = TRUE;
  }
  else
  {
    i2s_config_param.ws_master = FALSE;
  }

  hal_i2s_config(i2s_index, i2s_dir, &i2s_config_param);

  //start the DMA engine
  if (ADSP_FAILED(result = afe_dma_mgr_start(p_dev_state->dma_dev_hdl)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d", result);
    afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
    p_dev_state->dma_dev_hdl = NULL;
    goto __bailout;
  }

  // Enabling the interrupts will be done in the dma manager

  //enable the inteface
  hal_i2s_enable(i2s_index, i2s_dir);

  p_dev_state->state = I2S_INTERFACE_START;

#ifdef AUDIOIF_REG_DUMP
  dma_index = afe_dma_mgr_get_dma_index(p_dev_state->dma_dev_hdl);
  hal_afe_audioif_dump(LPA_IF_I2S, i2s_index, i2s_dir, dma_index, LPA_IF_START);
#endif

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "I2S port intf %d successfully opened", p_dev_state->intf);

  __bailout:

  return result;
}


/**
  @brief Stop the data transfer from i2s port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_stop(void *afe_drv)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  if (NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle Null");
    return ADSP_EFAILED;
  }

#ifdef AUDIOIF_REG_DUMP
  uint32_t dma_index = afe_dma_mgr_get_dma_index(p_dev_state->dma_dev_hdl);
#endif

  if (I2S_INTERFACE_STOP == p_dev_state->state)
  {
    return ADSP_EFAILED;
  }

  // disable and clear dma interrupt
  afe_dma_mgr_disable_clear_dma_int(p_dev_state->dma_dev_hdl);

  if ((AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) ||
      (AFE_LINEAR_PCM_DATA_PACKED_60958 == p_dev_state->data_format))
  {
    //deallocate and deinit channel status table for 60958 format packing
    (void)afe_i2s_deinit_IEC60958_ch_status_table(p_dev_state);
  }

  // the restart seq will skip disabling interface and mux free
  if (FALSE == p_dev_state->is_restart_seq)
  {

    uint32_t i2s_index = (uint32_t)p_dev_state->intf;
    uint32_t i2s_dir =  (uint32_t)p_dev_state->direction;

    hal_i2s_disable(i2s_index, i2s_dir);

  }

  //  free any shared resource
  uint32_t mux_index = (uint32_t)p_dev_state->intf;
  if (ADSP_FAILED(result = afe_audioif_mux_free(AUDIOIF_I2S, mux_index)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "The audioif mux free failed i2s %ul", p_dev_state->intf);
  }

  //stop the DMA engine
  afe_dma_mgr_stop(p_dev_state->dma_dev_hdl);

  //De-register with the DMA Manager including the isr callback
  afe_dma_mgr_close(p_dev_state->dma_dev_hdl);
  p_dev_state->dma_dev_hdl = NULL;

  p_dev_state->state = I2S_INTERFACE_STOP;

#ifdef AUDIOIF_REG_DUMP
  hal_afe_audioif_dump(LPA_IF_I2S, p_dev_state->intf, p_dev_state->direction, dma_index, LPA_IF_STOP);
  hal_afe_clock_dump(LPA_IF_I2S, p_dev_state->intf, LPA_IF_STOP);
#endif

  return result;
}


/**
  @brief Read data from the i2s DMA buffer to the client buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_read(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{

  ADSPResult result = ADSP_EOK;
  int16_t   num_chan_by_2;
  uint16_t  swap_dist;
  int16_t   num_channels, num_chan_minus_2;
  int16_t   num_chan_minus_3;
  int16_t   *ptr_pBuf16;
  int32_t   * pBuf_to_ptr32,*pBuf_frm_ptr32;
  int64_t   *ptr_dst_64;
  int32_t   * ptr_src1_32,*ptr_src2_32,*ptr_dst_32;
  int16_t   * pBuf_to_ptr16,*pBuf_frm_ptr16;
  uint32_t  total_num_samples, samp, i, bit_width;
  uint16_t  q_format_shift_factor;

  // This can not be chanegd only for i2s driver... dependency with other drivers.
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  if ((NULL == afe_drv) || (NULL == buffer_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / buffer pointer Null");
    return ADSP_EFAILED;
  }

  if (ADSP_FAILED(result = afe_dma_mgr_read(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
  }

  // post processing is required for the cases as below.  bit width and number of channels.

  num_channels = p_dev_state->num_channels;
  q_format_shift_factor = p_dev_state->q_format_shift_factor;
  bit_width = p_dev_state->bit_width;

  /* Calculate the total number of samples*/
  total_num_samples = (num_dma_samples * num_channels);

  if (2 == p_dev_state->bytes_per_channel)
  {
    if (1 == num_channels)
    {

#ifdef __qdsp6__
      ptr_src1_32 = (int32_t *)buffer_ptr;  /* First sample in the buffer */
      ptr_dst_32 = ptr_src1_32;
      ptr_src2_32 = ptr_src1_32 + 1;        /* Second sample */

      /* For mono case, data is stereo in DMA buffers, with right as invalid channel.
               Stereo to mono conversion, or dropping the right channel
               Read two 32-bit words, take the high 16-bits (L-channel)
               from both the 32-bit words and pack in a single 32-bit word
       */
      for (samp = num_dma_samples; samp >= 2; samp -= 2)
      {
        /* In place packing */
        (*ptr_dst_32++) = Q6_R_combine_RlRl(*ptr_src2_32, *ptr_src1_32);

        /* Increment the pointer to point to next pair of LR channel samples  */
        ptr_src1_32 += 2;
        ptr_src2_32 += 2;
      }

      /* If odd number of samples per channel */
      if (samp)
      {
        pBuf_to_ptr16 = (int16_t *)ptr_dst_32;
        pBuf_frm_ptr16 = (int16_t *)ptr_src1_32;
        *pBuf_to_ptr16 = *pBuf_frm_ptr16;
      }
#else
      /*----- Un-vectorized non-Q6 version ------------*/
      pBuf_frm_ptr16 = (int16_t *)buffer_ptr;
      pBuf_to_ptr16 = pBuf_frm_ptr16;

      for (i = 0; i < num_dma_samples; ++i)
      {
        *pBuf_to_ptr16++ = *pBuf_frm_ptr16++;
        pBuf_frm_ptr16++;
      }
#endif /* __qdsp6__ */
    }
    else if (4 == num_channels)
    {
      /* Samples are interleaved
               changing the order from L1 L2 R1 R2 -> L1 R1 L2 R2
               Reorder numChannels samples per iteration    */

#ifdef __qdsp6__
      ptr_dst_64 = (int64_t *)buffer_ptr;  /* Inplace output buffer      */
      ptr_src1_32 = (int32_t *)buffer_ptr; /* First sample in the buffer */
      ptr_src2_32 = ptr_src1_32 + 1;       /* Second sample  */

      /* Swap using vectorization, total number of samples will always be multiple of 4  */
      for (samp = total_num_samples; samp > 0; samp -= 4)
      {
        /* In block of L1 L2 R1 R2, swap R1 and L2 -> L1 R1 L2 R2  */
        *ptr_dst_64++ = Q6_P_packhl_RR(*ptr_src2_32, *ptr_src1_32);

        ptr_src2_32 += 2;
        ptr_src1_32 += 2;
      }
#else
      /*---------- Un-optimized non-q6 version ----------------*/
      pBuf_frm_ptr16 = (int16_t *)buffer_ptr;

      for (samp = 1; samp < total_num_samples; samp += num_channels)
      {
        afe_swap_s16(pBuf_frm_ptr16[samp], pBuf_frm_ptr16[samp + 1]);
      }
#endif /* __qdsp6__ */
    }
    else if (num_channels > 4)
    {
      pBuf_frm_ptr16 = (int16_t *)buffer_ptr;
      num_chan_by_2 = num_channels >> 1;
      swap_dist = num_channels >> 2; //sapcing between channel samples to be swaped

      /* Samples are interleaved
                changing the order from L1 L2...Ln R1 R2...Rn -> L1 R1 L2 R2...Ln Rn
               Reorder num_channels samples per iteration  */

      num_chan_minus_2 = (num_channels - 2);
      num_chan_minus_3 = (num_channels - 3);

      for (samp = total_num_samples; samp > 0; samp -= num_channels)
      {
        ptr_pBuf16 = &pBuf_frm_ptr16[num_chan_by_2];

        for (i = 0; i < swap_dist; i++)
        {
          afe_swap_s16(ptr_pBuf16[i], ptr_pBuf16[i - swap_dist]);
        }

        afe_swap_s16(pBuf_frm_ptr16[1], pBuf_frm_ptr16[2]);
        afe_swap_s16(pBuf_frm_ptr16[num_chan_minus_2], pBuf_frm_ptr16[num_chan_minus_3]);

        /* Increment the pointer to next block of num_channels samples   */
        pBuf_frm_ptr16 += num_channels;
      }
    }
  }
  else /* if(afe_port_ptr->bytes_per_channel == 4) */
  {
    pBuf_frm_ptr32 = (int32_t *)buffer_ptr;

    if (1 == num_channels)
    {
#ifdef __qdsp6__
      ptr_src1_32 = (int32_t *)buffer_ptr;  /* First sample in the buffer */
      ptr_src2_32 = ptr_src1_32 + 2;        /* Third sample  */
      ptr_dst_64 = (int64_t *)buffer_ptr;

      /* For mono case, data is stereo in DMA buffers, with right as
               invalid channel.
               Stereo to mono conversion, or dropping the right channel
               Read two 32-bit words, take the high 16-bits (L-channel)
               from both the 32-bit words and pack in a single 32-bit word   */
      if(AFE_BITS_PER_SAMPLE_24 == bit_width)
      {
        for (samp = num_dma_samples; samp >= 2; samp -= 2)
        {
          /* In place packing and shift */
          (*ptr_dst_64) = Q6_P_combine_RR(*ptr_src2_32, *ptr_src1_32);
          *ptr_dst_64 = Q6_P_vasrw_PR(*ptr_dst_64, QFORMAT_SHIFT_FACTOR);
          ptr_dst_64++;

          /* Increment the pointer to point to next pair of LR channel samples  */
          ptr_src1_32 += 4;
          ptr_src2_32 += 4;
        }
      }
      else
      {
        for (samp = num_dma_samples; samp >= 2; samp -= 2)
        {
          /* In place packing*/
          (*ptr_dst_64) = Q6_P_combine_RR(*ptr_src2_32, *ptr_src1_32);
          ptr_dst_64++;

          /* Increment the pointer to point to next pair of LR channel samples  */
          ptr_src1_32 += 4;
          ptr_src2_32 += 4;
        }
      }

      /* If odd number of samples per channel  */
      if (samp)
      {
        pBuf_to_ptr32 = (int32_t *)ptr_dst_64;
        pBuf_frm_ptr32 = (int32_t *)ptr_src1_32;

        *pBuf_to_ptr32 = (*pBuf_frm_ptr32 >> q_format_shift_factor);
      }
#else
      /*----------- Non Q6 Version ---------------*/
      /*  Q31->Q28 conversion, drop one channel along with scaling   */
      pBuf_to_ptr32 = pBuf_frm_ptr32;

      for (i = 0; i < num_dma_samples; ++i)
      {
        *pBuf_to_ptr32++ = ((*pBuf_frm_ptr32++) >> q_format_shift_factor);
        pBuf_frm_ptr32++;
      }
#endif /* __qdsp6__ */
    }
    else if (2 == num_channels)
    {
      /* only scaling as no re-ordering is required   */
#ifdef __qdsp6__
      ptr_dst_64 = (int64_t *)buffer_ptr;

      /* Q31 to Q28 conversion, two samples at at time */
      if(AFE_BITS_PER_SAMPLE_24 == bit_width)
      {
        for (i = 0; i < num_dma_samples; i++)
        {
          (*ptr_dst_64) = Q6_P_vasrw_PR(*ptr_dst_64, QFORMAT_SHIFT_FACTOR);
          ptr_dst_64++;
        }
      }
      else
      {
        ptr_dst_64 += num_dma_samples;
      }
#else
      /*------Non Q6 Version ---------*/
      for (i = 0; i < total_num_samples; i++)
      {
        pBuf_frm_ptr32[i] = (pBuf_frm_ptr32[i] >> q_format_shift_factor);
      }
#endif /* __qdsp6__ */
    }
    else if (4 == num_channels)
    {
      /* Samples are interleaved
               changing the order from L1 L2 R1 R2 -> L1 R1 L2 R2

               Reorder numChannels samples per iteration
               1 swap per numChannels samples   */

      for (samp = total_num_samples; samp > 0; samp -= 4)
      {
        /* Swap, and scale Q31 PCM to Q28  */
        afe_swap_shiftr(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);

        /* Scale the Ist and 4th channel samples  */
        pBuf_frm_ptr32[0] >>= q_format_shift_factor;
        pBuf_frm_ptr32[3] >>= q_format_shift_factor;

        /* Increment the pointer to point to next block of num_channel samples */
        pBuf_frm_ptr32 += 4;
      }
    }
    else if (6 == num_channels)
    {
      /* Samples are interleaved
               changing the order from L1 L2...Ln R1 R2...Rn -> L1 R1 L2 R2...Ln Rn
               Reorder numChannels samples per iteration
       */
      for (samp = total_num_samples; samp > 0; samp -= 6)
      {
        /* Scale the first channel sample  */
        pBuf_frm_ptr32[0] >>= q_format_shift_factor;

        /* Indices 2 & 3 are only swapped */
        afe_swap_s32(pBuf_frm_ptr32[2], pBuf_frm_ptr32[3]);

        /* Swap and convert to Q28 */
        afe_swap_shiftr(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);
        afe_swap_shiftr(pBuf_frm_ptr32[3], pBuf_frm_ptr32[4], q_format_shift_factor);

        /* Scale the last channel sample in the block of numChannels samples  */
        pBuf_frm_ptr32[5] >>= q_format_shift_factor;

        /* Increment the pointer to point to next block of num_channel samples  */
        pBuf_frm_ptr32 += num_channels;
      }
    }
    else if (8 == num_channels)
    {
      /* Samples are interleaved
               changing the order from L1 L2...Ln R1 R2...Rn -> L1 R1 L2 R2...Ln Rn
               Reorder numChannels samples per iteration
       */
      for (samp = total_num_samples; samp > 0; samp -= num_channels)
      {
        /* Shift the first and last sample in numChannels samples block, which do not get swapped*/
        /* Scale the first channel sample*/
        pBuf_frm_ptr32[0] >>= q_format_shift_factor;

        /* Swap and scale the samples at the same time */

        /* Swap arg1 and arg2, right shift only arg 1 */
        afe_swap_s32_rsh_s1(pBuf_frm_ptr32[2], pBuf_frm_ptr32[4], q_format_shift_factor);
        /* Swap arg1 and arg2, right shift only arg 2 */
        afe_swap_s32_rsh_s2(pBuf_frm_ptr32[3], pBuf_frm_ptr32[5], q_format_shift_factor);

        /* Swap and arith right shift both arg1 & arg2 */
        afe_swap_shiftr(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);
        afe_swap_shiftr(pBuf_frm_ptr32[5], pBuf_frm_ptr32[6], q_format_shift_factor);

        /* Scale the last channel sample in the block of numChannels samples */
        pBuf_frm_ptr32[7] >>= q_format_shift_factor;

        /* Increment the pointer to point to next block of num_channel samples */
        pBuf_frm_ptr32 += num_channels;
      }
    } /* if(num_channels > 4) */
  }


  return result;

}

/**
  @brief Copy data from client buffer to the i2s DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_write(afe_drv_h_t afe_drv, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
  ADSPResult result = ADSP_EOK;
  int16_t    num_chan_by_2;
  uint16_t   swap_dist;
  int16_t    num_channels, num_chan_minus_2;
  int16_t    num_chan_minus_3;
  int16_t    *ptr_pBuf16;
  int32_t    * pBuf_to_ptr32,*pBuf_frm_ptr32,*ptr_pBuf32;
  int64_t    *ptr_dst_64;
  int32_t    * ptr_src1_32,*ptr_src2_32;
  int16_t    * pBuf_to_ptr16,*pBuf_frm_ptr16;
  uint32_t   total_num_samples, samp, i,bit_width;
  uint16_t   q_format_shift_factor;

  // This can not be chanegd only for i2s driver... dependency with other drivers.
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  if ((NULL == afe_drv) || (NULL == buffer_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / buffer pointer Null");
    return ADSP_EFAILED;
  }

  // pre processing is required for the cases as below.
  // bit width,  number of channels
  num_channels = p_dev_state->num_channels;
  q_format_shift_factor = p_dev_state->q_format_shift_factor;
  bit_width = p_dev_state->bit_width;

  /* Calculate the total number of samples*/
  total_num_samples = (num_dma_samples * num_channels);

  if (2 == p_dev_state->bytes_per_channel)
  {
    pBuf_frm_ptr16 = (int16_t *)buffer_ptr;

    if (1 == num_channels)
    {
      ptr_pBuf16 = pBuf_frm_ptr16 + num_dma_samples - 1;
      pBuf_to_ptr16 = pBuf_frm_ptr16 + num_dma_samples * 2 - 1;

      for (i = 0; i < num_dma_samples; ++i)
      {
        *pBuf_to_ptr16-- = *ptr_pBuf16;
        *pBuf_to_ptr16-- = *ptr_pBuf16--;
      }
    }
    else if (4 == num_channels)
    {
      /* Samples are interleaved changing the order from L1 R1 L2 R2 -> L1 L2 R1 R2
                  Reorder num_channels samples per iteration
                  1 swap per num_channels samples */
#ifdef __qdsp6__
      ptr_dst_64 = (int64_t *)buffer_ptr;          /* Pointer to inplace dest buffer */
      ptr_src1_32 = (int32_t *)buffer_ptr;         /* Pointer to first sample        */
      ptr_src2_32 = ptr_src1_32 + 1;            /* Pointer to second sample       */

      for (samp = total_num_samples; samp > 0; samp -= 4)
      {
        /* Swap the middle two channels in the block of 4 channel samples */
        *ptr_dst_64++ = Q6_P_packhl_RR(*ptr_src2_32, *ptr_src1_32);

        ptr_src2_32 += 2;
        ptr_src1_32 += 2;
      }
#else
      /*---------- Un-optimized non-q6 version ----------------*/
      for (samp = 1; samp < (total_num_samples); samp += num_channels)
      {
        afe_swap_s16(pBuf_frm_ptr16[samp], pBuf_frm_ptr16[samp + 1]):
      }
#endif /* __qdsp6__ */
    }
    else if (num_channels > 4)
    {
      pBuf_frm_ptr16 = (int16_t *)buffer_ptr;
      num_chan_by_2 = num_channels >> 1;
      swap_dist = num_channels >> 2; //sapcing between channel samples to be swaped

      /* Samples are interleaved changing the order from  L1 R1 L2 R2...Ln Rn -> L1 L2...Ln R1 R2...Rn
               Reorder num_channels samples per iteration  */

      num_chan_minus_2 = (num_channels - 2);
      num_chan_minus_3 = (num_channels - 3);

      for (samp = total_num_samples; samp > 0; samp -= num_channels)
      {
        afe_swap_s16(pBuf_frm_ptr16[1], pBuf_frm_ptr16[2]);
        afe_swap_s16(pBuf_frm_ptr16[num_chan_minus_2], pBuf_frm_ptr16[num_chan_minus_3]);

        ptr_pBuf16 = &pBuf_frm_ptr16[num_chan_by_2];

        for (i = 0; i < swap_dist; i++)
        {
          afe_swap_s16(ptr_pBuf16[i], ptr_pBuf16[i - swap_dist]);
        }

        /* Increment the pointer to point to next block of num_channel samples  */
        pBuf_frm_ptr16 += num_channels;
      }
    }
  }
  else
  {
    pBuf_frm_ptr32 = (int32_t *)buffer_ptr;

    if (1 == num_channels)
    {
      ptr_pBuf32 = pBuf_frm_ptr32 + num_dma_samples - 1;
      pBuf_to_ptr32 = pBuf_frm_ptr32 + num_dma_samples * 2 - 1;

      /* Convert back Q27 -> Q31 */
      for (samp = 0; samp < num_dma_samples; samp++)
      {
        *ptr_pBuf32 = s32_shl_s32_sat(*ptr_pBuf32, q_format_shift_factor);
        *pBuf_to_ptr32-- = *ptr_pBuf32;
        *pBuf_to_ptr32-- = *ptr_pBuf32--;
      }
    }
    else if (2 == num_channels)
    {
      /* only scaling as no re-ordering is required   */
      ptr_dst_64 = (int64_t *)buffer_ptr;

      if(AFE_BITS_PER_SAMPLE_24 == bit_width)
      {
        for (i = 0; i < total_num_samples; i++)
        {
          /* Q27 to Q31 conversion, one sample at a time */
          (*pBuf_frm_ptr32) = s32_shl_s32_sat(*pBuf_frm_ptr32, q_format_shift_factor);
          pBuf_frm_ptr32++;
        }
      }
      else
      {
        ptr_dst_64 += num_dma_samples;
      }

    }
    else if (4 == num_channels)
    {
      /* Samples are interleaved changing the order from L1 R1 L2 R2 -> L1 L2 R1 R2
                  Reorder num_channels samples per iteration
                  1 swap per num_channels samples*/
      for (samp = total_num_samples; samp > 0; samp -= 4)
      {
        /* Swap, and scale Q31 PCM to Q28 */
        afe_swap_shiftl(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);

        /* Scale the Ist and 4th channel samples */
        pBuf_frm_ptr32[0] = s32_shl_s32_sat(pBuf_frm_ptr32[0], q_format_shift_factor);
        pBuf_frm_ptr32[3] = s32_shl_s32_sat(pBuf_frm_ptr32[3], q_format_shift_factor);

        /* Increment the pointer to point to next block of num_channel samples */
        pBuf_frm_ptr32 += 4;
      }
    }
    else if (6 == num_channels)
    {
      pBuf_frm_ptr32 = (int32_t *)buffer_ptr;

      /* Samples are interleaved changing the order from  L1 R1 L2 R2...Ln Rn -> L1 L2...Ln R1 R2...Rn
                  Reorder num_channels samples per iteration
       */

      for (samp = total_num_samples; samp > 0; samp -= 6)
      {
        /* Shift the first and last sample in num_channels samples block, which do not get swapped */

        /* Scale the first channel sample */
        pBuf_frm_ptr32[0] = s32_shl_s32_sat(pBuf_frm_ptr32[0], q_format_shift_factor);

        /* Swap and shift Q28 to Q31 */
        afe_swap_shiftl(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);
        afe_swap_shiftl(pBuf_frm_ptr32[3], pBuf_frm_ptr32[4], q_format_shift_factor);

        /* Only swap */
        afe_swap_s32(pBuf_frm_ptr32[2], pBuf_frm_ptr32[3]);

        /* Shift the last sample in the block of num_channels samples  */
        pBuf_frm_ptr32[5] = s32_shl_s32_sat(pBuf_frm_ptr32[5], q_format_shift_factor);

        /* Increment the pBuffer pointer to point to next block of num_channels samples  */
        pBuf_frm_ptr32 += num_channels;
      }
    }
    else if (8 == num_channels)
    {
      for (samp = total_num_samples; samp > 0; samp -= num_channels)
      {
        /* Shift the first and last sample in num_channels samples block, which do not get swapped*/
        /* Scale the first channel sample*/
        pBuf_frm_ptr32[0] = s32_shl_s32_sat(pBuf_frm_ptr32[0], q_format_shift_factor);

        /* Swap and shift Q28 to Q31 */
        afe_swap_shiftl(pBuf_frm_ptr32[1], pBuf_frm_ptr32[2], q_format_shift_factor);
        afe_swap_shiftl(pBuf_frm_ptr32[5], pBuf_frm_ptr32[6], q_format_shift_factor);

        /* Swap and left shift only arg2 */
        afe_swap_s32_lsh_s2(pBuf_frm_ptr32[2], pBuf_frm_ptr32[4], q_format_shift_factor);

        /* Swap and left shift only arg1 */
        afe_swap_s32_lsh_s1(pBuf_frm_ptr32[3], pBuf_frm_ptr32[5], q_format_shift_factor);

        /* Shift the last sample in the block of num_channels samples */
        pBuf_frm_ptr32[7] = s32_shl_s32_sat(pBuf_frm_ptr32[7], q_format_shift_factor);

        /* Increment the pBuffer pointer to point to next block of num_channels samples */
        pBuf_frm_ptr32 += num_channels;
      }
    }
  }

  if (ADSP_FAILED(result = afe_dma_mgr_write(p_dev_state->dma_dev_hdl, (uint8_t *)buffer_ptr)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
  }

  return result;
}

/**
  @brief Routine to set the properties for I2S

  @param[in] afe_drv_h_t pointer to the dev structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_set_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;


  if (NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle Null");
    return ADSP_EFAILED;
  }

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "I2S set prop id:%d , i2s port index:%d, dir:%d",\
        prop_id, p_dev_state->intf, p_dev_state->direction);

  switch (prop_id)
  {
    case AFE_DEVICE_RESTART_PROP:
    {
      afe_dev_restart_info_t  *i2s_restart_cfg_data_ptr = (afe_dev_restart_info_t  *)prop_ptr;

      if (payload_size < sizeof(afe_dev_restart_info_t))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S set property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
              payload_size, p_dev_state->intf, p_dev_state->direction);
        result = ADSP_EBADPARAM;
      }
      p_dev_state->new_sampling_rate = i2s_restart_cfg_data_ptr->sample_rate;
      p_dev_state->is_restart_seq = TRUE;
      break;
    }
    default:
      result = ADSP_EUNSUPPORTED;
      break;
  }

  return result;


}


/**
  @brief Routine to read the properites for I2S 

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_get_properties(afe_drv_h_t afe_drv, uint32_t prop_id, int8_t *prop_ptr, uint32_t payload_size)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;


  if (NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle Null");
    return ADSP_EFAILED;
  }

  switch (prop_id)
  {
    case AFE_DEVICE_PORT_PROP:
    {
      afe_dev_port_config_t  *i2s_port_cfg_data_ptr = (afe_dev_port_config_t  *)prop_ptr;

      if (payload_size < sizeof(afe_dev_port_config_t))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S get property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
              payload_size, p_dev_state->intf, p_dev_state->direction);
        result = ADSP_EBADPARAM;
      }

      result = afe_get_i2s_dev_port_cfg(afe_drv, i2s_port_cfg_data_ptr);
      break;
    }
    case AFE_DEVICE_TIMESTAMP_AND_FIFO_PROP:
    {
      afe_dev_timestamp_info_t  *time_stamp_info_ptr = (afe_dev_timestamp_info_t *)prop_ptr;

      if (payload_size < sizeof(afe_dev_timestamp_info_t))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S get property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
              payload_size, p_dev_state->intf, p_dev_state->direction);
        result = ADSP_EBADPARAM;
      }

      result = afe_i2s_get_dma_timestamp(afe_drv, time_stamp_info_ptr);
      break;
    }
    case AFE_DEVICE_RESTART_PROP:
    {
      afe_dev_restart_info_t  *i2s_restart_cfg_data_ptr = (afe_dev_restart_info_t  *)prop_ptr;

      if (payload_size < sizeof(afe_dev_restart_info_t))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S get property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
              payload_size, p_dev_state->intf, p_dev_state->direction);
        result = ADSP_EBADPARAM;
      }
      i2s_restart_cfg_data_ptr->sample_rate = p_dev_state->sample_rate; //get current dev sampling rate: old...
      break;
    }
    case AFE_DEVICE_GROUP_PROP:
    {
      afe_dev_grp_config_info_t  *i2s_group_cfg_data_ptr = (afe_dev_grp_config_info_t  *)prop_ptr;

      if (payload_size < sizeof(afe_dev_grp_config_info_t))
      {
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S get property fail: Invalid payload size: %ld, i2s port index:%d, dir:%d",\
              payload_size, p_dev_state->intf, p_dev_state->direction);
        result = ADSP_EBADPARAM;
      }
      result = afe_dev_group_config_i2s_port(p_dev_state, i2s_group_cfg_data_ptr);

      break;
    }
    default:
      result = ADSP_EUNSUPPORTED;
      break;
  }

  return result;


}

/**
  @brief Get I2S dev port config to fill the info to the port structure.
        NOTE: This is the new function to remove port structure access inside device driver right after config.
                   Upper layer should call this function to fill the port structure.

  @param[in] afe_drv pointer for afe device port structure
  @param[out] i2s_port_cfg_data_ptr pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_get_i2s_dev_port_cfg(void *afe_drv, afe_dev_port_config_t  *i2s_port_cfg_data_ptr)
{
  ADSPResult result = ADSP_EOK;
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;

  if (I2S_DRIVER_INIT != i2s_driver.driver_state)
  {
    return ADSP_EFAILED;
  }

  if ((NULL == afe_drv) || (NULL == i2s_port_cfg_data_ptr))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle / params pointer is null");
    return ADSP_EFAILED;
  }

  // this part is port structure which we'd better access this in the driver.
  // we can have new function such as get function which provide information related to this.

  // port data sturtcure to be filled.
  i2s_port_cfg_data_ptr->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
  if ((AFE_NON_LINEAR_DATA == p_dev_state->data_format) || (AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format))
  {
    i2s_port_cfg_data_ptr->port_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
  }
  else if (AFE_DSD_DOP_DATA == p_dev_state->data_format)
  {
    i2s_port_cfg_data_ptr->port_data_type = AFE_DSD_DOP_COMPRESSED_DATA_PORT;
  }
  else if (AFE_GENERIC_COMPRESSED_DATA == p_dev_state->data_format)
  {
    i2s_port_cfg_data_ptr->port_data_type = AFE_GENERIC_COMPRESSED_DATA_PORT;
  }

  /* In the case of AFE_LINEAR_PCM_DATA_PACKED_16BIT, 32bit sample from device is unpacked into
      two 16bit samples and they are considered as two different channels. So, Number of channels
      and bitwidth of port need to be updated accordingly */
  if (AFE_LINEAR_PCM_DATA_PACKED_16BIT == p_dev_state->data_format)
  {
    i2s_port_cfg_data_ptr->bit_width = p_dev_state->bit_width >> 1;
    i2s_port_cfg_data_ptr->channels = p_dev_state->num_channels << 1;
    i2s_port_cfg_data_ptr->bytes_per_channel =  (i2s_port_cfg_data_ptr->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
  }
  else
  {
    i2s_port_cfg_data_ptr->bit_width = p_dev_state->bit_width;
    i2s_port_cfg_data_ptr->channels = p_dev_state->num_channels;
    i2s_port_cfg_data_ptr->bytes_per_channel =  p_dev_state->bytes_per_channel;
  }

  i2s_port_cfg_data_ptr->sample_rate = p_dev_state->sample_rate;
  i2s_port_cfg_data_ptr->q_format_shift_factor  = p_dev_state->q_format_shift_factor;
  i2s_port_cfg_data_ptr->is_interleaved = TRUE;

  return result;
}



/**
  @brief Routine to read the hardware timestamp registers for I2S Input

  @param[in] afe_port_ptr pointer to the port structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_get_dma_timestamp(void *afe_drv, afe_dev_timestamp_info_t *timestamp_info_ptr)
{
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_drv;
  uint64_t dma_int_timestamp = 0;
  uint32_t fifo_count = 0;
  ADSPResult result = ADSP_EOK;

  if (NULL == afe_drv)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to i2s device handle Null");
    return ADSP_EFAILED;
  }

  //avt_drift_params_ptr =  &p_dev_state->i2s_avt_drift;

  if (ADSP_FAILED(result = afe_dma_mgr_read_timestamp_fifo_count(p_dev_state->dma_dev_hdl, &dma_int_timestamp, &fifo_count)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_dma_mgr_read_timestamp_fifo_count failed intf[%d] dir[%d]"
          , p_dev_state->intf, p_dev_state->direction);
  }
  else
  {
    timestamp_info_ptr->dma_int_timestamp = avtimer_drv_convert_hw_tick_to_time(dma_int_timestamp);

    /* FIFO count is in 32 bit words. Convert it to samples as per port byter per sample */
    timestamp_info_ptr->fifo_samples = fifo_count * sizeof(uint32_t) / p_dev_state->bytes_per_channel;
    // need to revist ????? david
  }

  return result;
}

/**
  @brief Callback routine for the i2s interrupt

  @param[in] afe_drv pointer to the i2s dev port state structure

  @param[in] event_mask indicates if it is dma interrupt or error.

  @return  none

 */
static void afe_i2s_int_callback(void *afe_drv, uint32 event_mask)
{
  i2s_port_state_t         *dev_state_ptr = (i2s_port_state_t *)afe_drv;;

  if (NULL == afe_drv)
  {
    // critical error
    return;
  }

  if (event_mask & DMA_TYPE2_EVENT_FIFOERROR)
  {
    dev_state_ptr->fifo_ovf_cnt++;
  }

  if (event_mask & DMA_TYPE2_EVENT_BUSERROR)
  {
    dev_state_ptr->ahb_err_cnt++;
  }

  if (event_mask & DMA_TYPE2_EVENT_DATA)
  {
    if (NULL != dev_state_ptr->cb_info.afe_dev_int_cb)
    {
      dev_state_ptr->i2s_intr_cnt++;
      dev_state_ptr->cb_info.afe_dev_int_cb(dev_state_ptr->cb_info.cb_arg);
    }
  }
}

/**
  Copy data from the i2s DMA buffer to the client buffer
  This  function which is called by dma manager 

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address       pointer to the DMA buffer
  @param[in]  dst_address       pointer to the client buffer

  @return
  None

  @dependencies
  None.
 */
static void afe_i2s_read_dma(void *afe_driver_context, uint32_t *src_address, uint8_t *dst_address)
{
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_driver_context;


  if ((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
    return;
  }

  if ((AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) ||
      (AFE_LINEAR_PCM_DATA_PACKED_60958 == p_dev_state->data_format))
  {
    //IEC60958 pack and copy data
    afe_i2s_copy_IEC60958_data(p_dev_state, src_address, (uint16_t *)dst_address);
  }
  else
  {
    // i2s is always 2 channel of data even for mono
    uint32_t length = p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel * afe_i2s_driver_next_even_value(p_dev_state->num_channels);
    memscpy(dst_address, length, src_address, length);
  }

  return;
}

/**
  Copy data from client buffer to the i2s Output DMA buffer
  write function which is called by dma manager

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address                    pointer to the client buffer
  @param[in]  dst_address                    pointer to the DMA buffer

  @return
  None

  @dependencies
  None.
 */

static void afe_i2s_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address)
{
  i2s_port_state_t *p_dev_state = (i2s_port_state_t *)afe_driver_context;

  if ((NULL == afe_driver_context) || (NULL == src_address) || (NULL == dst_address))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pointer to dma handle / buffer addresses Null");
    return;
  }

  if ((AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) ||
      (AFE_LINEAR_PCM_DATA_PACKED_60958 == p_dev_state->data_format))
  {
    //IEC60958 pack and copy data
    afe_i2s_fill_IEC60958_data(p_dev_state, (uint16_t *)src_address, dst_address);
  }
  else
  {
    uint32_t length = p_dev_state->int_samples_per_period * p_dev_state->bytes_per_channel
        * afe_i2s_driver_next_even_value(p_dev_state->num_channels);

    afe_dma_buff_copy((int8_t *)dst_address, length ,(int8_t *)src_address, length);
  }

  return;
}

/**
  @brief save i2s port config and error check for the params.

  @param[in] p_dev_state pointer for AFE dev port structure
  @param[in] i2s_cfg_ptr   pointer to the configuration structure

  @return  ADSP_EOK on success, an error code on error
 */
static ADSPResult afe_dev_config_i2s_port(i2s_port_state_t * p_dev_state, afe_param_id_i2s_cfg_t * i2s_cfg_ptr)
{
  ADSPResult result = ADSP_EOK;

  // Printing the device configuration information
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Device config I2S Port Interface index:%d, direction:%d, cfg version:%lu", p_dev_state->intf, p_dev_state->direction, i2s_cfg_ptr->i2s_cfg_minor_version);
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Bit width:%d, Channel Mode:%d, Data Format:%d", i2s_cfg_ptr->bit_width, i2s_cfg_ptr->channel_mode, i2s_cfg_ptr->data_format);
  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MonoStereo: %u Sample Rate:%lu, MSM mode:%d", i2s_cfg_ptr->mono_stereo, i2s_cfg_ptr->sample_rate, i2s_cfg_ptr->ws_src);
  //Since DSD HW will only accept two channels, we will error out for all DSD ports not stereo.
  if ((AFE_DSD_DOP_DATA == i2s_cfg_ptr->data_format) && (1 != i2s_cfg_ptr->mono_stereo))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %u for AFE_DSD_DOP_DATA data format, i2s intf: 0x%x", i2s_cfg_ptr->mono_stereo, p_dev_state->intf);
    return ADSP_EBADPARAM;
  }
  // need to check line capabilities here.
  switch (i2s_cfg_ptr->channel_mode)
  {
    //supported for all i2s interfaces
    case AFE_PORT_I2S_SD0:
    case AFE_PORT_I2S_SD1:
    case AFE_PORT_I2S_SD2:
    case AFE_PORT_I2S_SD3:
    {
      if (i2s_cfg_ptr->mono_stereo == AFE_PORT_I2S_STEREO)
        p_dev_state->num_channels = 2;
      else if (i2s_cfg_ptr->mono_stereo == AFE_PORT_I2S_MONO)
        p_dev_state->num_channels = 1;
      else
        result = ADSP_EBADPARAM;
    }
    break;
    case AFE_PORT_I2S_QUAD01:
    case AFE_PORT_I2S_QUAD23:
    {
      p_dev_state->num_channels = 4;
    }
    break;
    case AFE_PORT_I2S_6CHS:
    {
      p_dev_state->num_channels = 6;
    }
    break;
    case AFE_PORT_I2S_8CHS:
    {
      p_dev_state->num_channels = 8;
    }
    break;
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported channel_mode: i2s intf %lu",  p_dev_state->intf);
      result = ADSP_EBADPARAM;

    }
  }

  // parse the bit width information
  if ((AFE_BITS_PER_SAMPLE_16 != i2s_cfg_ptr->bit_width) && (AFE_BITS_PER_SAMPLE_24 != i2s_cfg_ptr->bit_width) && (AFE_BITS_PER_SAMPLE_32 != i2s_cfg_ptr->bit_width))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x, i2s intf: %lu", i2s_cfg_ptr->bit_width,  p_dev_state->intf);
    result = ADSP_EBADPARAM;
  }

  p_dev_state->bit_width = i2s_cfg_ptr->bit_width;

  p_dev_state->bytes_per_channel = (p_dev_state->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;


  /* Determining the qformat shift factor based on bit width. We use this shift factor for 24bit only*/
  if (AFE_BITS_PER_SAMPLE_24 == p_dev_state->bit_width)
  {
    p_dev_state->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
  }
  else
  {
    p_dev_state->q_format_shift_factor = 0;
  }

  // parse the channel mode information
  switch (i2s_cfg_ptr->channel_mode)
  {
    case AFE_PORT_I2S_SD0:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD0;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD0_MASK;
      break;
    case AFE_PORT_I2S_SD1:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD1;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD1_MASK;
      break;
    case AFE_PORT_I2S_SD2:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD2;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD2_MASK;
      break;
    case AFE_PORT_I2S_SD3:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD3;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD3_MASK;
      break;
    case AFE_PORT_I2S_QUAD01:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD0_AND_SD1;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD0_AND_SD1_MASK;
      break;
    case AFE_PORT_I2S_QUAD23:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_SD2_AND_SD3;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_SD2_AND_SD3_MASK;
      break;
    case AFE_PORT_I2S_6CHS:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_6_CHANNELS;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_6_CHANNELS_MASK;
      break;
    case AFE_PORT_I2S_8CHS:
      p_dev_state->channel_mode = I2S_CHANNEL_MODE_8_CHANNELS;
      p_dev_state->channel_mode_mask = I2S_CHANNEL_MODE_8_CHANNELS_MASK;
      break;
    default:
      result = ADSP_EBADPARAM;
      break;
  }

  // parse the MSM mode information
  switch (i2s_cfg_ptr->ws_src)
  {
    case AFE_PORT_CONFIG_I2S_WS_SRC_EXTERNAL:
      p_dev_state->msm_mode = I2S_MSM_MODE_SLAVE;
      break;
    case AFE_PORT_CONFIG_I2S_WS_SRC_INTERNAL:
      p_dev_state->msm_mode = I2S_MSM_MODE_MASTER;
      break;
    default:
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported ws_src: %u, i2s intf: 0x%x", i2s_cfg_ptr->ws_src, p_dev_state->intf);
      result = ADSP_EBADPARAM;
      break;
  }

  switch (i2s_cfg_ptr->data_format)
  {
    case AFE_NON_LINEAR_DATA_PACKED_60958:
    case AFE_LINEAR_PCM_DATA_PACKED_60958:
      p_dev_state->dma_input_bytes_per_sample =  PACKED_60958_BYTES_PER_SAMPLES;
      break;
    case AFE_NON_LINEAR_DATA:
    case AFE_LINEAR_PCM_DATA:
    case AFE_LINEAR_PCM_DATA_PACKED_16BIT:
    case AFE_DSD_DOP_DATA:
    case AFE_GENERIC_COMPRESSED_DATA:
      p_dev_state->dma_input_bytes_per_sample = p_dev_state->bytes_per_channel;
      break;
    default:
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported data format: %u, i2s intf: 0x%x", i2s_cfg_ptr->data_format, p_dev_state->intf);
      return ADSP_EBADPARAM;
      break;
  }

  if ((AFE_LINEAR_PCM_DATA_PACKED_16BIT == i2s_cfg_ptr->data_format) && (AFE_BITS_PER_SAMPLE_32 != i2s_cfg_ptr->bit_width))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bitwidth: %u for AFE_LINEAR_PCM_DATA_PACKED_16BIT data format, i2s intf: 0x%x", i2s_cfg_ptr->bit_width, p_dev_state->intf);
    return ADSP_EBADPARAM;
  }

  p_dev_state->data_format = i2s_cfg_ptr->data_format;

  switch (i2s_cfg_ptr->sample_rate)
  {
    case AFE_PORT_SAMPLE_RATE_8K:
    case AFE_PORT_SAMPLE_RATE_11_025K:
    case AFE_PORT_SAMPLE_RATE_16K:
    case AFE_PORT_SAMPLE_RATE_22_05K:
    case AFE_PORT_SAMPLE_RATE_24K:
    case AFE_PORT_SAMPLE_RATE_48K:
    case AFE_PORT_SAMPLE_RATE_96K:
    case AFE_PORT_SAMPLE_RATE_32K:
    case AFE_PORT_SAMPLE_RATE_192K:
    case AFE_PORT_SAMPLE_RATE_384K:
    case AFE_PORT_SAMPLE_RATE_44_1K:
    case AFE_PORT_SAMPLE_RATE_88_2K:
    case AFE_PORT_SAMPLE_RATE_176_4K:
    case AFE_PORT_SAMPLE_RATE_352_8K:
      p_dev_state->sample_rate = i2s_cfg_ptr->sample_rate;
      break;
    default:
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, i2s intf: %lu", i2s_cfg_ptr->sample_rate, p_dev_state->intf);
      return ADSP_EBADPARAM;
  }

  return result;
}


/**
  @brief calculate the number of dma buffer.
            The default value is 2 as ping-pong buffer.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  the number of buffer (0 in error case)

 */
static uint32 afe_i2s_get_num_dma_buffer(i2s_port_state_t *p_dev_state)
{
   uint32 num_dma_buffer = DEFAULT_I2S_DMA_BUFFERS;
   uint32 dma_buf_size_in_bytes_per_intr;

   dma_buf_size_in_bytes_per_intr = p_dev_state->dma_input_bytes_per_sample *\
      p_dev_state->int_samples_per_period *\
      p_dev_state->num_channels;

   /*twice the length of each buffer - ping-pong*/   
   uint32 circ_buf_length = num_dma_buffer * dma_buf_size_in_bytes_per_intr;

   /* Iterate over to find the next higher DMA buffer size
      which is 16 byte aligned */
   while ((circ_buf_length & I2S_DMA_BUFFER_16BYTE_ALIGNMENT) &&
          (num_dma_buffer < MAX_I2S_DMA_BUFFERS))
   {
      /* Double the number of DMA buffers */
      num_dma_buffer <<= 1;

      /* Double the DMA buffer size */
      circ_buf_length = num_dma_buffer * dma_buf_size_in_bytes_per_intr ;
   }

   /* The lowest supported fractional sampling rate is 11.025 KHz.
      For 16 byte alignment, number of buffers should never be
      more than MAX_I2S_DMA_BUFFERS */
   if (num_dma_buffer > MAX_I2S_DMA_BUFFERS)
   {
      num_dma_buffer = 0;

      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "DMA total buffer length (%lu) is not 16 byte aligned",
            circ_buf_length);
   }

   return num_dma_buffer;
}


/**
  @brief calculate the watermark value.
            The default value is 8 dwords.

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  watermark value which is 1 or 8
 */
static uint32 afe_i2s_get_watermark(i2s_port_state_t * p_dev_state)
{
  uint32 water_mark = DEAFULT_I2S_WATERMARK;
  uint32 dma_buf_size_in_bytes_per_intr;
  uint32 dma_buf_size_in_dword_per_intr;

  /* check if burst mode can be enabled
   * if per length is not bigger than fifo length or per buf size is not aligned to 16 bytes,
   * then disable the burst mode and use watermark as 1 dword
   */
  dma_buf_size_in_bytes_per_intr = p_dev_state->dma_input_bytes_per_sample *  p_dev_state->int_samples_per_period *  p_dev_state->num_channels;

  dma_buf_size_in_dword_per_intr = dma_buf_size_in_bytes_per_intr >> 2;

  //8k mono case. The intSamplePeriod will be 8 * 1 / 2 = 4 DWs
  //The burst mode needs to be disabled for this case for latency and drift detection issue
  if ((DEAFULT_I2S_WATERMARK >= dma_buf_size_in_dword_per_intr) ||
      (dma_buf_size_in_dword_per_intr & 0x3))
  {
    water_mark = 1;
  }

  return water_mark;
}


/**
  @brief allocate and init  IEC60958 channel status table

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  ADSP_EOK on success, an error code on error
 */
static ADSPResult afe_i2s_init_IEC60958_ch_status_table(i2s_port_state_t * p_dev_state)
{
  ADSPResult result = ADSP_EOK;

  // this is used only for IEC60958 input
  p_dev_state->audio_blk_sync = 0;

  //allocate memory for IEC60958 Channel status format table
  /* pointer of IEC 60958-3 Mode 0 Channel Status General Format for Consumer User table */
  p_dev_state->IEC60958_ch_status_table_ptr = (uint32_t *)qurt_elite_memory_malloc(sizeof(uint32_t) * IEC60958_BLOCK_SIZE,
                                                                                   QURT_ELITE_HEAP_DEFAULT);
  if (NULL == p_dev_state->IEC60958_ch_status_table_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in allocating memory for IEC60958 channel status table");
    return ADSP_ENORESOURCE;
  }
  memset(p_dev_state->IEC60958_ch_status_table_ptr, 0, sizeof(uint32_t) * IEC60958_BLOCK_SIZE);
  p_dev_state->IEC60958_frame_index  = 0;


  /**
   * The IEC60958 channel status table configured based on sampleRate, pcmDataType, and bitWidth  
   * Mode 0 channel status are 192 bits but we are using 192 Dwords representing the bits.
   * NOTE: Preamble B only take care. Preamble M and W is set to 0.
   */

  p_dev_state->IEC60958_ch_status_table_ptr[0] = 1UL << IEC60958_PREAMBLE_BIT3;  //block start bit in preamble bit 3 and consumer use.

  if (AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) //used for sim test.  //TODO: it will be used for I2S HDMI input
  {
    //The purpose of channel status bit 1 is to indicate if IEC 60958 is used to convey
    //linear PCM or non-linear PCM. This bit shall be set to '1' when IEC 60958 is used
    //to convey non-linear PCM encoded audio bitstreams
    p_dev_state->IEC60958_ch_status_table_ptr[1] = 1UL << IEC60958_C_BIT30;
  }

  switch (p_dev_state->sample_rate)
  {
    case 32000:  // 1100 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[24] = 1UL << IEC60958_C_BIT30;
      p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << IEC60958_C_BIT30;
      break;
    case 44100:  // 0000 in 24..27
      break;
    case 88200:  // 0001 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << IEC60958_C_BIT30;
      break;
    case 176400: // 0011 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[26] = 1UL << IEC60958_C_BIT30;
      p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << IEC60958_C_BIT30;
      break;
    case 48000:  // 0100 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << IEC60958_C_BIT30;
      break;
    case 96000:  // 0101 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << IEC60958_C_BIT30;
      p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << IEC60958_C_BIT30;
      break;
    case 192000: // 0111 in 24..27
      p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << IEC60958_C_BIT30;
      p_dev_state->IEC60958_ch_status_table_ptr[26] = 1UL << IEC60958_C_BIT30;
      p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << IEC60958_C_BIT30;
      break;
    default:
      break;
  }

  //why do we set the bit width as 24 bits always?
  if (24 <= p_dev_state->bit_width)
  {
    //bit 32: set to 1 to indicate the maximum audio sample word length as 24 bits
    p_dev_state->IEC60958_ch_status_table_ptr[32] = 1UL << IEC60958_C_BIT30;
    //bit 33 to 35: set to "101" to indicate 24 bits
    p_dev_state->IEC60958_ch_status_table_ptr[33] = 1UL << IEC60958_C_BIT30;
    p_dev_state->IEC60958_ch_status_table_ptr[35] = 1UL << IEC60958_C_BIT30;
  }

  return result;
}

/**
  @brief deallocate and deinit  IEC60958 channel status table

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  ADSP_EOK on success, an error code on error
 */
static ADSPResult afe_i2s_deinit_IEC60958_ch_status_table(i2s_port_state_t * p_dev_state)
{
  ADSPResult result = ADSP_EOK;

  //free the IEC60958 table
  qurt_elite_memory_free(p_dev_state->IEC60958_ch_status_table_ptr);
  p_dev_state->IEC60958_ch_status_table_ptr = NULL;
  p_dev_state->IEC60958_frame_index = 0;
  p_dev_state->audio_blk_sync = 0;

  return result;
}


/*
Status block info for each channel will be filled.     
Assumption: IEC60958 LPCM has stereo content (2 - channel).    
 */
static void afe_i2s_fill_hdmi_ch_status_table(uint32_t * table_ptr, int32_t index, uint8_t ch_status)
{
  table_ptr[index] = ch_status;
  /* if it is 191, then we got the all info for channel status bits */
  /* parse the info and check if there are changes */

}


static void afe_i2s_hdmi_copy_audio_sample(int16_t * p_dst, int32_t * p_src, int32_t sample_bit_width)
{
  switch (sample_bit_width)
  {
    case 8: /*fall through*/
    case 16:
      //dst[dst_index] = 0;
      p_dst[0] = ((int16 *)p_src)[1];
      break;
    case 20: /*fall through*/
    case 24: /*fall through*/
    case 32: /*fall through*/
    default:
      memscpy((int32 *)p_dst, sizeof(int32), p_src, sizeof(int32));
      break;
  }
}


static void afe_i2s_parse_IEC60958_subframe(i2s_IEC60958_subframe_info_t * p_subframe, int32_t * data, int16_t * dst, int32_t sample_bit_width)
{
  uint32_t audio_sample = 0;
  uint8_t flags = 0;
  int32_t sample = 0;

  audio_sample = (uint32)(*data);
  flags = IEC60958_GET_FLAGS_INFO(audio_sample);
  audio_sample = IEC60958_GET_AUDIO_SAMPLE(audio_sample);

  sample = (int32_t)(audio_sample << IEC60958_MSB_ALIGN_BITS_MASK);

  p_subframe->validity_flag = ((flags & IEC60958_V_BIT_MASK) ? 1 : 0);
  p_subframe->user_data     = ((flags & IEC60958_U_BIT_MASK) ? 1 : 0);
  p_subframe->ch_status     = ((flags & IEC60958_C_BIT_MASK) ? 1 : 0);
  p_subframe->parity_bit    = ((flags & IEC60958_P_BIT_MASK) ? 1 : 0);

  afe_i2s_hdmi_copy_audio_sample(dst, &sample, sample_bit_width);
}


/**
  @brief unpack the data from IEC60958 format and copy it to dma buffer.
     data format is IEC60958 Subframe format and there is no null subframe within a sample for i2s

  @param[in] p_dev_state pointer for AFE dev port state structure
  @param[in] src client buffer
  @param[in] dst dma buffer

 */
static void afe_i2s_copy_IEC60958_data(i2s_port_state_t * p_dev_state, uint32_t * src, uint16_t * dst)
{
  int32_t  dst_offset = 0;
  int32_t  dummy_sample = 0;
  uint32_t frame_missed_pkt_cnt = 0;
  i2s_IEC60958_subframe_info_t sub_frame_info;
  uint8_t audio_blk_start_flag = 0;
  int32_t sample_bit_width = p_dev_state->bit_width;
  int32_t num_channels = p_dev_state->num_channels;
  uint32_t num_samples_to_copy = p_dev_state->int_samples_per_period;
  uint32_t cur_frame = p_dev_state->IEC60958_frame_index;


  int32_t *p_src = (int32_t *)src;
  int16_t *p_dst = (int16_t *)dst;

  dst_offset = (sample_bit_width <= 16) ? 1 : 2;

  for (uint32_t i = 0; i < num_samples_to_copy; i++)
  {
    for (int32_t pri_ch_idx = 0; pri_ch_idx < num_channels; pri_ch_idx++)
    {
      if ((0 == cur_frame) && (0 == pri_ch_idx))
      {
        uint32 chdata = (uint32)(*p_src);

        audio_blk_start_flag = ((chdata & IEC60958_PREAMBLE_MASK) == IEC60958_PREAMBLE_B_VAL);

        if (audio_blk_start_flag)
        {
          //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFERead: Audio Block Start = %d\n", audio_blk_start_flag);

          /*Initialize all variables in start of audio block*/
          //sampleRateInvalidFlag = 0;
          memset(&sub_frame_info, 0, sizeof(i2s_IEC60958_subframe_info_t));

          afe_i2s_parse_IEC60958_subframe(&sub_frame_info, p_src, p_dst, sample_bit_width);

          /*Assumption: Clients (AFE Svc / ADM / ASM) does not require channel status information for Non Linear PCM.
                          So, below parsing works only for LPCM*/
          afe_i2s_fill_hdmi_ch_status_table(p_dev_state->IEC60958_ch_status_table_ptr,
                                            cur_frame,
                                            sub_frame_info.ch_status);
          p_dev_state->audio_blk_sync = 1;
        }
        else
        {
          //TODO - As of now, filling the packets with zeros when it is not in IEC60958 format.
          // Need to check whether it is fine to the client
          afe_i2s_hdmi_copy_audio_sample(p_dst, &dummy_sample, sample_bit_width);

          if (p_dev_state->audio_blk_sync)
          {
            frame_missed_pkt_cnt++;
          }
        }
      }
      else
      {
        memset(&sub_frame_info, 0, sizeof(i2s_IEC60958_subframe_info_t));

        afe_i2s_parse_IEC60958_subframe(&sub_frame_info, p_src, p_dst, sample_bit_width);

        if (0 == pri_ch_idx)
        {
          afe_i2s_fill_hdmi_ch_status_table(p_dev_state->IEC60958_ch_status_table_ptr, cur_frame, sub_frame_info.ch_status);
        }
      }
      p_src++;
      p_dst += dst_offset;
    }

    // HDMI channel status cycle every 192 frames
    if (p_dev_state->audio_blk_sync)
    {
      cur_frame++;
      if (IEC60958_BLOCK_SIZE == cur_frame)
      {
        cur_frame = 0;
      }
    }
  } //for (i = 0; i < numSamples; i++)
  if (frame_missed_pkt_cnt)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "B-Frame missed count in 1ms data:%d\n", frame_missed_pkt_cnt);
  }

  p_dev_state->IEC60958_frame_index = cur_frame;
}



/**
  @brief pack the data for IEC60958 format and copy it to dma buffer.
     data format is IEC60958 Subframe format and there is no null subframe within a sample for i2s

  @param[in] p_dev_state pointer for AFE dev port state structure
  @param[in] src client buffer
  @param[in] dst dma buffer

 */
static void afe_i2s_fill_IEC60958_data(i2s_port_state_t * p_dev_state, uint16_t * src, uint32_t * dst)
{
  uint32_t  i = 0, numch = 0;
  uint32_t packed_data, packed_data1, align_shift, data_mask, data_shift_dir;
  uint32_t cur_frame = p_dev_state->IEC60958_frame_index;
  uint32_t *src32 = (uint32_t *)src;

  //always 16bits for non-linear use case.
  switch (p_dev_state->bit_width)
  {
    case 20:  //data from AFE SVC is left justified (bit 31:12 valid)
      // msb aligned so that it should be same as 24bit case.
      align_shift  = IEC60958_MSB_ALIGN_BIT8 - 4;
      data_mask    = 0xFFFFF000;
      data_shift_dir = I2S_IEC60958_DATA_SHIFT_RIGHT;
      break;
    case 24:  //data from AFE SVC is left justified (bit 31:8 valid)
    case 32:
      align_shift = IEC60958_MSB_ALIGN_BIT8 - 4;
      data_mask   = 0xFFFFFF00;
      data_shift_dir = I2S_IEC60958_DATA_SHIFT_RIGHT;
      break;
    case 16:
    default:
      align_shift = IEC60958_MSB_ALIGN_BIT8 + IEC60958_AUX_START_BIT4;
      data_mask   = 0x0000FFFF;
      data_shift_dir = I2S_IEC60958_DATA_SHIFT_LEFT;
      break;
  }

  //numChannels for non-linear case is always 2.
  while (i < p_dev_state->int_samples_per_period * p_dev_state->num_channels)  //numSampleToCopy = number of sample for one channel
  {
    numch = p_dev_state->num_channels;
    i += numch;
    while (numch--)
    {
      packed_data = (p_dev_state->bit_width <= 16) ? (uint32)(*src++) & data_mask : (uint32)(*src32++) & data_mask;

      if (I2S_IEC60958_DATA_SHIFT_RIGHT == data_shift_dir)
      {
        packed_data >>= align_shift;
      }
      else
      {
        packed_data <<= align_shift;
      }

      //Channel status is identical for both subframes of the interface.
      packed_data |= p_dev_state->IEC60958_ch_status_table_ptr[cur_frame];

      if (AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) //TODO: it will be used for I2S HDMI input
      {
        packed_data |= 1UL << IEC60958_V_BIT28;  //set validity flag to 1 for non-linear PCM only
        // 1 if is not reliable.
        //NOTE For transmissions not using a linear PCM coding, this bit may be set.
        // This is intended to prevent accidental decoding of non-audio data to analogue before a cmplete channel status block is received
      }

      // calculate parity (optimized version)
      // parity bit carries from bit 4 to 31 inclusive, carry an even number of ones and an even number of zeros (even parity)
      packed_data1  = packed_data & 0xFFFFFFF0;
      packed_data1 ^= packed_data1 >> 1;
      packed_data1 ^= packed_data1 >> 2;
      packed_data1  = (packed_data1 & 0x11111111U) * 0x11111111U;

      // fill P bit, Parity value at bit28
      *dst++ = packed_data | ((packed_data1 << 3) & 0x80000000);
    }

    // HDMI channel status cycle every 192 frames
    cur_frame++;
    if (IEC60958_BLOCK_SIZE == cur_frame)
    {
      cur_frame = 0;
    }
  }

  p_dev_state->IEC60958_frame_index = cur_frame;
}

/**
  @brief Obtain the chip specific information for initializing the driver

  @param[in] i2s_hw_info_ptr pointer for I2S Device config structure

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_i2s_driver_device_cfg_init(i2s_dev_cfg_struct_t * i2s_hw_info_ptr)
{
  DALSYSPropertyVar prop_var;
  lpasshwio_prop_lpaif_struct_t  *lpaif_prop_dev_cfg_ptr = NULL;
  const char *dev_id = "LPAIF";
  uint32 intf_cnt;

  DALSYS_PROPERTY_HANDLE_DECLARE(dal_hdl);

  /* Get device handle */
  if (DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, dal_hdl))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting the DAL device handle for I2S");
    return ADSP_EFAILED;
  }
  else
  {
    /* Read property ptr */
    if (DAL_SUCCESS != DALSYS_GetPropertyValue(dal_hdl, "LpaifPropStructPtr", 0, &prop_var))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error getting HDMI input Property Struct Ptr");
      return ADSP_EFAILED;
    }
    else
    {
      lpaif_prop_dev_cfg_ptr = (lpasshwio_prop_lpaif_struct_t *)prop_var.Val.pStruct;

      if (NOT_SUPPORTED == lpaif_prop_dev_cfg_ptr->i2s_prop.is_supported)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "I2S interface is not supported");
        return ADSP_EUNSUPPORTED;
      }

      /* hw revision */
      i2s_hw_info_ptr->hw_revision = lpaif_prop_dev_cfg_ptr->hw_revision;

      /* I2S register base address */
      i2s_hw_info_ptr->i2s_reg_base = lpaif_prop_dev_cfg_ptr->lpaif_reg_base;
      /* I2S register page size */
      i2s_hw_info_ptr->i2s_reg_size = lpaif_prop_dev_cfg_ptr->lpaif_reg_size;
      /* I2S register offset */
      i2s_hw_info_ptr->i2s_reg_offset = lpaif_prop_dev_cfg_ptr->i2s_prop.i2s_reg_offset_addr;
      /* Internal I2S register offset */
      i2s_hw_info_ptr->int_i2s_reg_offset = lpaif_prop_dev_cfg_ptr->i2s_prop.int_i2s_reg_offset_addr;
      /* number of i2s interfaces */
      i2s_hw_info_ptr->inf_type_cnt = lpaif_prop_dev_cfg_ptr->i2s_prop.inf_type_cnt;
      for (intf_cnt = 0; intf_cnt < i2s_hw_info_ptr->inf_type_cnt; intf_cnt++)
      {
        i2s_hw_info_ptr->intf_port_info[intf_cnt] = (i2s_intf_port_info_t* ) lpaif_prop_dev_cfg_ptr->i2s_prop.intf_port_info[intf_cnt];
      }
    }
  }

  return ADSP_EOK;
}
