/*========================================================================
This file contains main interface to the AFE Spdif driver

Copyright 2009-2015 Qualcomm Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/inc/AFESpdifDriver.h#1 $
 ====================================================================== */

#ifndef _AFE_SPDIF_DRIVER_H_
#define _AFE_SPDIF_DRIVER_H_

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "AFEInternal.h"
#include "mmdefs.h"
#include "mmpm.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* It initializes the device ports at the begining of system
* start.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_spdif_driver_init(void);

/**
* It de-initialzies the device ports.
*
* @return value Error code signifying success or failure
*/
ADSPResult afe_spdif_driver_deinit(void);

/**
  @brief Perform SPDIF port initialization 
   
  @param[in] afe_svc_ptr pointer for AFE static service 

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_spdif_port_init(aud_stat_afe_svc_t* afe_svc_ptr);

/**
  @brief Perform SPDIF port de-initialization 
   
  @param[in] afe_svc_ptr pointer for AFE static service 

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_spdif_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
* Open the port and start dma.
*
* @param[in] pDevPort Pointer to the port instance
* @return value Error code signifying success or failure
*/
ADSPResult afe_spdif_open(afe_dev_port_t *pDevPort);

/**
* close the port and stop dma.
*
*  @param[in] pDevPort pointer to dev port object
*  
* @return value Error code signifying success or failure
*/
ADSPResult afe_spdif_close(afe_dev_port_t *pDevPort);


/**
* Get the current time from the DMA rate
*  
* @param[in] dev_port_ptr - device port ptr
* @param[out] curr_time - current time in us
* @param[in] fast_int_call - whether the call is made from fast-int
* @return value Error code signifying success or failure
*/
ADSPResult afe_get_time_for_spdif_port(afe_dev_port_t *dev_port_ptr, uint64 *curr_time);

/**
 * Update drift for spdif if port.
 *
 * @param[in] pDevPort Pointer to the instance of the port.
 * @return
 * void pointer to the handle.
 *
 * @dependencies
 * None.
 */
ADSPResult afe_update_avt_drift_for_spdif_port(afe_dev_port_t *dev_port_ptr);


/**
 * read the timestamp of the last TS diff interrupt from hardware register.
 * @param[in] afe_port_ptr Pointer to the instance of the port.
 * @return
 * uint64_t AV timer timestamp when the last TS diff interrupt fired.
 *
 * @dependencies
 * None.
 */
uint64_t afe_spdif_get_tsdiff_timestamp(afe_dev_port_t *afe_port_ptr);


ADSPResult afe_spdif_set_cfg (afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size);
ADSPResult afe_spdif_get_cfg (afe_dev_port_t *pDevPort, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

ADSPResult afe_spdif_set_chstatus_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size);

ADSPResult afe_spdif_get_chstatus_cfg(afe_dev_port_t *pDevPort, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

ADSPResult afe_spdif_set_clk_cfg (afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size); 
/**
  @brief Routine to get spdif mmpm core id 

  @return  MMPM_CORE_ID_LPASS_SPDIF or MMPM_CORE_ID_NONE

*/
MmpmCoreIdType afe_get_spdif_mmpm_coreid();

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // !defined(_AFE_AUDIO_IF_DRIVER_H_)
