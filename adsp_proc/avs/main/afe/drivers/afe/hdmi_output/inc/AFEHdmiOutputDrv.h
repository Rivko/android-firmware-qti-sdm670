/*========================================================================
   This file contains functions to set the parameters for different modules.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/inc/AFEHdmiOutputDrv.h#1 $
 ====================================================================== */
#ifndef AFE_HDMI_OUTPUT_DRV_H
#define AFE_HDMI_OUTPUT_DRV_H

/*==========================================================================
  Include Files
========================================================================== */
#include "lpasshwio_devcfg.h"
#include "AFEInternal.h"
#include "mmdefs.h"
#include "AFEDmaManager.h"
#include "mmpm.h"

/*==========================================================================
  Macro Defines
  ========================================================================== */


/*==========================================================================
  Structure definitions
  ========================================================================== */


/*==========================================================================
  Function Declerations
  ========================================================================== */

/**
  @brief Perform HDMI Output Driver Initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_driver_init(void);

/**
  @brief Perform HDMI Output Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_driver_deinit(void);

/**
  @brief Perform HDMI Output port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_port_init(aud_stat_afe_svc_t* afe_svc_ptr);

/**
  @brief Perform HDMI Output port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
  @brief Set HDMI Output port config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_set_hdmi_output_cfg(afe_dev_port_t *dev_port_ptr, int8_t *cfg, uint32_t payload_size);

/**
  @brief Get HDMI Output port config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_get_cfg (afe_dev_port_t *dev_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

/**
  @brief Set HDMI Output channel status config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_set_chstatus_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size);



/**
  @brief Set up the data transfer for HDMI Output port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_driver_open(afe_dev_port_t *dev_port_ptr);

/**
  @brief Stop the data transfer from HDMI Output port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_hdmi_output_driver_close(afe_dev_port_t *dev_port_ptr);

/**
  @brief Copy data from client buffer to the HDMI Output DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

*/

ADSPResult afe_hdmi_output_driver_write(afe_dev_port_t *dev_port_ptr, int16_t *buffer_ptr, uint32_t num_dma_samples);

/**
  @brief Routine to read the hardware timestamp registers for HDMI Output

  @param[in] afe_port_ptr pointer to the port structure

  @return none

*/
void afe_hdmi_output_get_dma_timestamp(afe_dev_port_t *afe_port_ptr);


/**
  @brief Routine to AV Timer Drift Value for HDMI Output port

  @param[in] afe_port_ptr pointer to the AFE port data structure

  @return  error code

*/
ADSPResult afe_update_avt_drift_for_hdmi_output_port(afe_dev_port_t *dev_port_ptr);

/**
  @brief Routine to get device time for HDMI Output port

  @param[in] afe_port_ptr pointer to the AFE port data structure

  @return  error code

*/

ADSPResult afe_get_time_for_hdmi_out_port(afe_dev_port_t *dev_port, uint64 *curr_time);

/**
  @brief Routine to get device time and DMA FIFO latch count for HDMI Output port

  @param[in] afe_port_ptr pointer to the AFE port data structure

  @return  error code

*/

void afe_hdmi_output_get_dma_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr);


/**
  @brief Routine to get hdmitx mmpm core id 

  @return  MMPM_CORE_ID_LPASS_HDMITX or MMPM_CORE_ID_NONE

*/

MmpmCoreIdType afe_get_hdmitx_mmpm_coreid();


/**
  @brief Routine to get hdmitx mmpm BW port id 

  @return  MMPM_BW_PORT_ID_HDMITX_MASTER or MMPM_BW_PORT_ID_NONE

*/

MmpmBwPortIdType afe_get_hdmitx_mmpm_bwportid();

/**
  @brief Routine to unmute hdmi-over-dp port

  @param[in] afe_port_ptr pointer to the AFE port data structure
  @param[in] p_ctx pointer to internal data structure

  @return  error code

*/
ADSPResult afe_hdmi_over_dp_unmute(afe_dev_port_t *dev_port_ptr, void* p_ctx);

/**
  @brief Routine to mute hdmi-over-dp port

  @param[in] afe_port_ptr pointer to the AFE port data structure
  @param[in] p_ctx pointer to internal data structure

  @return  error code

*/
ADSPResult afe_hdmi_over_dp_mute(afe_dev_port_t *dev_port_ptr, void* p_ctx);

/**
  @brief Routine to handle custom messages  for hdmi-over-dp 
         port

  @param[in] cust_payload_ptr  pointer to the custome message 
        payload

  @return  error code

*/
ADSPResult afe_hdmi_over_dp_cust_msg_handler(void *cust_payload_ptr);
#endif //AFE_HDMI_OUTPUT_DRV_H
