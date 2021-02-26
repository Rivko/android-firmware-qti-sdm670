/*========================================================================
   This file contains AFE Pseudo port stub implementation

  Copyright (c) 2009-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pseudo_port/stub_src/AFEPseudoPort.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInternal.h"
#include "AFEDeviceDriver.h"
#include "VfrDriverInternal.h"
/*==========================================================================
  Globals & definitions
  ========================================================================== */


/*==========================================================================
  Function definitions
  ========================================================================== */

/**
  @brief Perform Pseudo port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
  return ADSP_EOK;
}

/**
  @brief Perform Pseudo port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_open(afe_dev_port_t *pDevPort)
{
  return ADSP_EOK;
}

/**
  @brief Perform Pseudo port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_close(afe_dev_port_t *pDevPort)
{
   return ADSP_EOK;
}

/**
  @brief Function to get the accumulated VFR drift for pseudo 
         port

  @param[in]  afe_port_ptr:  pointer to port object
  @param[in]  vfr_state_ptr:  pointer to VFR state object
   
  @result
  None

  @dependencies
  None.
*/
void afe_pseudoport_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
  return;
}

/**
  @brief Perform Pseudo port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	return ADSP_EOK;
}

/**
  @brief Set param functionality for pseudo port config

  @param[in] pDevPort: Pointer to AFE port structure 
  @param[in] cfg: Pointer to config payload
  @param[in] payload_size: Size of the config payload

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_int_pseudo_port_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;

	return result;
}
