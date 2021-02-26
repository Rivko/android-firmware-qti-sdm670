#ifndef _AFE_PSEUDO_PORT_DRIVER_H_
#define _AFE_PSEUDO_PORT_DRIVER_H_
/*========================================================================
 This file is afe device driver layer interface for Pseudo Ports

  Copyright (c) 2014 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 **====================================================================== */

/*========================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pseudo_port/inc/AFEPseudoPort.h#1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  

  ========================================================================== */
/**
@file AFEPseudoPort.h

@brief has high level APIs for pseudo port driver
*/

#include "AFEInternal.h"
#include "mmdefs.h"


/*=====================================================================
 Global Variables
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
 * Perform one time initialization of the AFE pseudo port
 *
 * @param[in] afe_svc_ptr: Pointer to AFE svc object
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_pseudo_port_init(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Perform de-initialization of the AFE pseudo prot
 *
 * @param[in] afe_svc_ptr: Pointer to AFE svc object
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_pseudo_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr);

/**
 * Open the pseudo port
 *
 * @param[in] pDevPort;  pointer to port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_pseudo_port_open(afe_dev_port_t *pDevPort);


/**
 * Close the pseudo port
 *
 * @param[in] pDevPort;  pointer to port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_pseudo_port_close(afe_dev_port_t *pDevPort);

/**
 * Get the accumulated VFR drift 
 *
 * @param[in] pDevPort;  pointer to port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_pseudoport_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr);

/**
 * Get the accumulated VFR drift 
 *
 * @param[in] pDevPort;  pointer to port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_set_int_pseudo_port_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size);

#endif /* #ifndef _AFE_PSEUDO_PORT_DRIVER_H_ */
