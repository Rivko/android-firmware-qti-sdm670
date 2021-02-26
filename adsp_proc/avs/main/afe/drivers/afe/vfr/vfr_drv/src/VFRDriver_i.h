/*========================================================================
  This file contains AFE Vocoder Frame Reference handler related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VFRDriver_i.h#1 $
 ====================================================================== */
#ifndef _VFR_DRIVER_I_H_
#define _VFR_DRIVER_I_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALSysTypes.h"
#include "DALPropDef.h"
#include "DDIChipInfo.h"

#include "AFEInternal.h"
#include "VfrDriverInternal.h"


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

#define VFR_SVC_CMD_TASK_INDEX			      31

/** Number of hw latched vfr sources */ 
#define VFR_NUM_HW_LATCHED_SRC               1

/** Positive drift threshold for sw latching in number of
 *  samples */
#define VFR_SW_LATCH_DRIFT_POS_THRESH     	(3)

/** Negative drift threshold for sw latching in number of
 *  samples */
#define VFR_SW_LATCH_DRIFT_NEG_THRESH     	(-VFR_SW_LATCH_DRIFT_POS_THRESH)

/*==========================================================================
  Structure definitions
  ========================================================================== */



/*==========================================================================
  Function definitions
  ========================================================================== */

/**
  VFR device config initialization function

  @param[in]  p_vfr_driver   pointer to VFR driver object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_device_cfg_init(vfr_drv_state_t *p_vfr_driver);

/**
  VFR driver initialization function

  @param[in]  p_vfr_driver   pointer to VFR driver object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/

ADSPResult vfr_driver_init(vfr_drv_state_t *p_vfr_driver);

/**
  VFR driver de-initialization function

  @param[in]  p_vfr_driver   pointer to VFR driver object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_driver_deinit(vfr_drv_state_t *p_vfr_driver);

/**
  VFR Interrupt callback function

  @param[in]  ctxt     Pointer to VFR source object

  @result
  None

  @dependencies
  None.
*/
void vfr_interrupt_callback(void *ctxt);


/**
  Function for enabling the VFR source driver

  @param[in]  p_vfr_drv:   pointer to VFR service driver object 
  @param[in]  vfr_state_ptr:   pointer to VFR service source 
        object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_enable_src_driver(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr);

/**
  Function for disabling the VFR source driver

  @param[in]  p_vfr_drv:   pointer to VFR service driver object
  @param[in]  vfr_state_ptr:   pointer to VFR service source 
        object

  @result
  ADSP_EOK, on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_disable_src_driver(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr);

/**
 * This function gets drift for each port
 *  
 * @param[in] afe_svc_global_ptr: Pointer to AFE service global 
 *       object
 * @param[in] vfr_state_ptr: Pointer to VFR source object 
 * @param[in] resync_status: resync status flag
 * @return None
 */
void vfr_driver_calc_drift_updates(aud_stat_afe_svc_t *afe_svc_global_ptr, vfr_src_state_t *vfr_state_ptr, 
                                   uint8_t *resync_status);

/**
 * This function for updaing the drift for each port and sending 
 * the message to VFR clients 
 *
 * @param[in] vfr_state_ptr: Pointer to VFR source object 
 * @param[in] vfr_hw_info: Pointer to VFR hardware info object
 *  
 * @return Error code signifying success or failure
 */
void vfr_drift_update(vfr_src_state_t *vfr_state_ptr, vfr_hw_info_t *vfr_hw_info);

/**
 * This function latches the device counter for each interface
 *  
 * @param[in] afe_svc_global_ptr: Pointer to AFE service global 
 *       object
 * @param[in] vfr_state_ptr: Pointer to VFR source object 
 * @return None
 */
void vfr_driver_latch_dev_counters(aud_stat_afe_svc_t *afe_svc_global_ptr, vfr_src_state_t *p_vfr_src_state);

/**
 * This function reads the hardware latched AV timestamp
 *
 * @param[in] vfr_src_state_ptr: Pointer to VFR source object

 * @param[in] vfr_hw_info: Pointer to VFR hardware info
 * @return None
 */
void vfr_driver_get_vfr_timestamp(vfr_src_state_t *vfr_src_state_ptr, vfr_hw_info_t *vfr_hw_info);

/**
 * This function calculates the VFR drift with respect to the device
 *
 * @param[in] afe_port_ptr: Pointer to AFE port

 * @param[in] vfr_state_ptr: Pointer to VFR state structure
 * @return None
 */
void vfr_driver_get_vfr_drift_timestamp_based(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _VFR_DRIVER_I_H_ */
