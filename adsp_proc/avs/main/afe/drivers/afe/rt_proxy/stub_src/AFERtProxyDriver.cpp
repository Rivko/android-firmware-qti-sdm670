/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/rt_proxy/stub_src/AFERtProxyDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFERtProxyDriver.cpp

DESCRIPTION: Main Interface to the AFE RT Proxy driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2011-2013,2017 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/rt_proxy/stub_src/AFERtProxyDriver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/10/10    JQ      Created file

========================================================================== */

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFERtProxyDriver.h"

/*=====================================================================
 Globals
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
ADSPResult afe_driver_rt_proxy_init(void)
{
	return ADSP_EUNSUPPORTED;
}

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
ADSPResult afe_driver_rt_proxy_deinit(void)
{
	return ADSP_EUNSUPPORTED;
}

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
ADSPResult afe_rt_proxy_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	return ADSP_EUNSUPPORTED;
}


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
ADSPResult afe_rt_proxy_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	return ADSP_EUNSUPPORTED;
}

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
ADSPResult afe_rt_proxy_port_open(afe_dev_port_t *pDevPort)
{
	return ADSP_EUNSUPPORTED;
}

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
void afe_rt_proxy_port_close(afe_dev_port_t *pDevPort)
{
	return;
}

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
ADSPResult afe_get_time_for_rt_proxy_port(afe_dev_port_t *pDevPort,uint64_t *curr_time)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform configuration of RT Proxy devices for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_cfg pointer to AFE port Rt Proxy config strcutre

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_set_rt_proxy_cfg (afe_dev_port_t* dev_port, int8_t *p_cfg, uint16_t payload_size)
{
	return ADSP_EUNSUPPORTED;
}

ADSPResult afe_get_rt_proxy_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief set channel mapping of the incoming device stream

  @param[in] port_id port id of the rt_proxy port
  @param[in] stream_chan_map_arr pointer to the 8 element array of channel enumeration

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_rt_proxy_set_stream_channel_map(uint16_t port_id, uint8_t *stream_chan_map_arr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief specified port is enabled for VFR drifts.

  @param[in] port_id port id of the rt_proxy port

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_rt_proxy_add_to_vfr_sync(uint16_t port_id, uint8_t vfr_src_idx)
{
	return ADSP_EUNSUPPORTED;
}


/**
  @brief get VFR drifts

  @param[in] port_id port id of the rt_proxy port
  @param[out] drift ptr to the value of the drift
  @param[out] resync_flag flag to indicate resync

  @return  ADSP_EOK on success, an error code on error
*/
void afe_rt_proxy_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
	return;
}

/**
 * when this callback is made during VFR interrupt, this function executes in fast-int context.
 * and latches required software counts
 */
void afe_rt_proxy_vfr_callback(uint8_t vfr_src_idx)
{
	return;
}

/**
 * updates the accumulated avt drift for rt proxy ports
 */
void afe_update_avt_drift_for_rt_proxy_port(afe_dev_port_t *dev_port_ptr)
{
	return;
}


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
ADSPResult afe_driver_rt_proxy_circ_buf_delay(afe_dev_port_t *dev_port_ptr, uint64_t* circ_buf_delay_us)
{
	return ADSP_EUNSUPPORTED;
}

