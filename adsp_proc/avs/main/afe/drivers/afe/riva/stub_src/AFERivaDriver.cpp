/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/stub_src/AFERivaDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFERivaDriver.cpp

DESCRIPTION: Main Interface to the AFE Riva driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/riva/stub_src/AFERivaDriver.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
12/10/10    JQ      Created file

========================================================================== */

/*=====================================================================
 Includes
 ======================================================================*/
#include "AFERivaDriver.h"

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform RIVA port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_riva_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{

	return ADSP_EUNSUPPORTED;
}

/**
  @brief Perform RIVA port deinitialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

*/
ADSPResult afe_riva_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{

	return ADSP_EUNSUPPORTED;
}

/**
 * Perform one time initialization of the AFE Riva driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_init(void)
{
   return ADSP_EUNSUPPORTED;
}

/**
 * Perform de-initialization of the AFE Riva driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_riva_deinit(void)
{
   return ADSP_EUNSUPPORTED;
}

/**
 * Perform RIVA compatible device open
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_riva_open(afe_dev_port_t *pDevPort)
{
   return ADSP_EUNSUPPORTED;
}

/**
 * Perform RIVA compatible device close
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_riva_close(afe_dev_port_t *pDevPort)
{
   return;
}

/**
 * Get the current time using Riva port data transfer
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
ADSPResult afe_get_time_for_riva_port(afe_dev_port_t *dev_port,uint64 *curr_time)
{
   return ADSP_EUNSUPPORTED;
}

/**
 * Process the IPC command from Riva, and update the riva port, BT Voc/Aud, FM
 * instance.
 *
 * @param[in] afe_svc_ptr pointer to AfeSvc instance
 */
ADSPResult afe_driver_ipc_cmd_proc(aud_stat_afe_svc_t *afe_svc_ptr)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief get VFR drifts

  @param[in] port_id port id of the riva port
  @param[out] drift ptr to the value of the drift
  @param[out] resync_flag flag to indicate resync

  @return  ADSP_EOK on success, an error code on error
*/
void afe_riva_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
   return;
}

ADSPResult afe_set_int_bt_cfg(afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size)
{
   return ADSP_EUNSUPPORTED;
}

ADSPResult afe_get_int_bt_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
   return ADSP_EUNSUPPORTED;
}

/**
 * gets accumulated drift for riva port
 */
void afe_update_avt_drift_for_riva_port(afe_dev_port_t *dev_port_ptr)
{
   return;
}
void afe_riva_crash_handler()
{
   return;
}
void afe_riva_recovery_handler ()
{
   return;
}

/**
  @brief Get current AV timer timestamp for riva port

  @param[in] afe_port_ptr, pointer to AFE port
  @param[out] riva_av_timestamp,  AV timer timestamp

  @return  None
*/
void afe_riva_get_av_timestamp(afe_dev_port_t *afe_port_ptr, uint64_t *riva_av_timestamp)
{
	return;
}

/**
 * For providing the delay introduced in RIVA port circular
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
ADSPResult afe_driver_riva_circ_buf_delay(afe_dev_port_t *pDevPort, uint64_t* circ_buf_delay_us)
{
	return ADSP_EUNSUPPORTED;
}

/**
  @brief Indicates whether RIVA connected to LPASS or not .

  @param[in] afe_port_ptr pointer to corresponding AFE port

  @return  TRUE if connected, FALSE if not connected
   *
 * @dependencies
 * None.
 *
 */
bool_t afe_is_riva_active(afe_dev_port_t* afe_port_ptr)
{
  return FALSE;
}
