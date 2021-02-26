#ifndef _AFE_RT_PROXY_DRIVER_H_
#define _AFE_RT_PROXY_DRIVER_H_
/*========================================================================
 This file is afe device driver layer interface for real-time proxy ports (mainly USB)

  Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/rt_proxy/inc/AFERtProxyDriver.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  03/15/11   rb      Created file.

  ========================================================================== */
/**
@file AFERtProxyDriver.h

@brief has high level APIs for RT proxy port driver
*/

#include "AFEInternal.h"
#include "mmdefs.h"
#include "VfrDriverInternal.h"

#if defined(__cplusplus)
extern "C" {
#endif



/*=====================================================================
 Global Variables
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
 * Perform one time initialization of the AFE RT proxy port driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_init(void);

/**
 * Perform de-initialization of the AFE Real time Proxy port driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_deinit(void);

/**
 * Perform Real time Proxy port compatible port's initialization
 *
 * @param[in] afe_svc_ptr pointer to AFE static service
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_rt_proxy_port_init(aud_stat_afe_svc_t* afe_svc_ptr);


/**
 * Perform Real time Proxy port compatible port's de-initialization
 *
 * @param[in] afe_svc_ptr pointer to AFE static service
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_rt_proxy_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform Real time Proxy port compatible device open
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_rt_proxy_port_open(afe_dev_port_t *pDevPort);

/**
 * Perform Real time Proxy port compatible device close
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_rt_proxy_port_close(afe_dev_port_t *pDevPort);

/**
 * Get the current time using Real time Proxy port port data transfer
 * rate
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[out] curr_time - time stamp in sample ticks
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_get_time_for_rt_proxy_port(afe_dev_port_t *pDevPort,uint64_t *curr_time);

/**
  @brief Perform configuration of RT Proxy devices for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_cfg pointer to AFE port Rt Proxy config strcutre

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_set_rt_proxy_cfg (afe_dev_port_t* dev_port, int8_t *p_cfg, uint16_t payload_size);
ADSPResult afe_get_rt_proxy_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr);

/**
  @brief set channel mapping of the incoming device stream

  @param[in] port_id port id of the rt_proxy port
  @param[in] stream_chan_map_arr pointer to the 8 element array of channel enumeration

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_rt_proxy_set_stream_channel_map(uint16_t port_id, uint8_t *stream_chan_map_arr);

/**
  @brief specified port is enabled for VFR drifts.

  @param[in] port_id port id of the rt_proxy port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_rt_proxy_add_to_vfr_sync(uint16_t port_id, uint8_t vfr_src_idx);


/**
  @brief get VFR drifts

  @param[in] port_id port id of the rt_proxy port
  @param[out] drift ptr to the value of the drift
  @param[out] resync_flag flag to indicate resync

  @return  ADSP_EOK on success, an error code on error
*/
void afe_rt_proxy_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr);

/**
 * when this callback is made during VFR interrupt, this function executes in fast-int context.
 * and latches required software counts
 */
void afe_rt_proxy_vfr_callback(uint8_t vfr_src_idx);

/**
 * updates the accumulated avt drift for rt proxy ports
 */
void afe_update_avt_drift_for_rt_proxy_port(afe_dev_port_t *dev_port_ptr);


/**
 * For providing the delay introduced in RT proxy circular 
 * buffer 
 *
 * @param[in] port_id port id of the rt_proxy port
 *@param[out] circ_buf_delay_us is the delay introduced due to  
 *  	circular buffer
  
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_circ_buf_delay(afe_dev_port_t *dev_port_ptr, uint64_t* circ_buf_delay_us);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFE_RT_PROXY_DRIVER_H_ */

