#ifndef __AFE_USB_AUDIO_DRIVER_H__
#define __AFE_USB_AUDIO_DRIVER_H__
/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/usba/inc/AFEUSBAudioDriver.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEUSBAudioDriver.h

DESCRIPTION: Main Interface to the AFE USB Audio driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/usba/inc/AFEUSBAudioDriver.h#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
02-22-2016  mk  Initial Draft


============================================================================*/
/**
@file AFEUSBAudioDriver.h

@brief has high level APIs for USB Audio driver
*/

#include "AFEInternal.h"
#include "mmdefs.h"
#include "VfrDriverInternal.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*=====================================================================
 Macros
 ======================================================================*/


/*=====================================================================
 Typedef's
 ======================================================================*/


/*=====================================================================
 Global Variables
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

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
ADSPResult afe_usba_port_init(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform USB audio port de-initialization 
 *
 * @param[in] afe_svc_ptr pointer for AFE static service 
 *  
 * @return  ADSP_EOK on success, an error code on error
 *  
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform one time initialization of the AFE USB audio driver
 *
 * @return  ADSP_EOK on success, an error code on faliure
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_driver_init(void);

/**
 * Perform de-initialization of the AFE USB audio driver
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult  afe_usba_driver_deinit(void);

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
ADSPResult afe_set_usba_cfg (afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, uint32_t param_id);
ADSPResult afe_get_usba_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr, uint32_t param_id);

/**
  @brief Open the configured USB Audio device for data transfers

  @param[in] afe_port_ptr pointer to AFE device port structure

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult  afe_usba_open (afe_dev_port_t* afe_port_ptr);

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
ADSPResult	afe_usba_close(afe_dev_port_t* afe_port_ptr);

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
ADSPResult  afe_usba_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples);


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
ADSPResult  afe_usba_write(afe_dev_port_t *afe_port_ptr, int16_t  *pBuffer, uint32_t numSamples);

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
ADSPResult afe_update_avt_drift_for_usba_port(afe_dev_port_t *afe_port_ptr);

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
ADSPResult afe_driver_usba_get_data_path_delay(afe_dev_port_t *afe_port_ptr, uint64_t* data_path_delay_us);

/**
 * For getting the timestamp for USBaudio device
 * @param[in] port_id port id of the USB audio device

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_usba_get_timestamp(afe_dev_port_t *afe_port_ptr);

/**
 * For restoring usb audio device configuration
 * @param[in] port_id port id of the USB audio device

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_restore_usba_cfg(afe_dev_port_t *afe_port_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*#ifndef __AFE_USB_AUDIO_DRIVER_H__*/

