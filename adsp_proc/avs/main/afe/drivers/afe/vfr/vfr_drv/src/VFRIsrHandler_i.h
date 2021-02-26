/*========================================================================
  This file contains AFE Vocoder Frame Reference handler related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VFRIsrHandler_i.h#1 $
 ====================================================================== */
#ifndef _VFR_ISR_HANDLER_I_H_
#define _VFR_ISR_HANDLER_I_H_

/*==========================================================================
  Include Files
  ========================================================================== */
#include "mmdefs.h"
#include "Elite.h"

#include "VFRDriver_i.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */

typedef enum vfr_src_index
{  
   VFR_SRC_0_IDX = 0,
   VFR_SRC_1_IDX = 1
}vfr_src_index_t;

/*==========================================================================
  Structure definitions
  ========================================================================== */



/*==========================================================================
  Function definitions
  ========================================================================== */
/**
  VFR ISR handler for all the VFR sources

  @param[in]  intr_num: Interrupt number for this interrupt 

  @result
  None

  @dependencies
  None.
*/

void vfr_src_idx_0_isr_handler(void *arg);
void vfr_src_idx_1_isr_handler(void *arg);

/** Common callback function for all the above ISR's */ 
void vfr_fastintr_callback(void *intr_cntxt);

/**
  Function for registering the VFR interrupt callback function

  @param[in]  p_vfr_drv:   Pointer to VFR driver object
  @param[in]  vfr_state_ptr:   Pointer to VFR source object
  @param[in]  vfr_hw_info: Pointer to VFR hw info object

  @result
  ADSP_EOK on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_register_callback(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr, vfr_hw_info_t *vfr_hw_info);

/**
  Function for de-registering the VFR interrupt callback 
  function 

  @param[in]  vfr_state_ptr:   Pointer to VFR source object 
   
  @result
  ADSP_EOK on success, error code on failure

  @dependencies
  None.
*/
ADSPResult vfr_deregister_callback(vfr_src_state_t *vfr_state_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _VFR_ISR_HANDLER_I_H_ */
