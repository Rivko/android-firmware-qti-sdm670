/*========================================================================
   This file contains AFE Side tone IIR internal apis

 Copyright (c) 2009-2017 QUALCOMM Technologies, Incorporated (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/algos/sidetone_iir/fwk/inc/AFESidetoneIIR.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/26/2014   rv       Created

==========================================================================*/

/*==========================================================================
  File includes
========================================================================== */
#include "AFEInternal.h"

#ifndef _AFE_SIDETONEIIR_H_
#define _AFE_SIDETONEIIR_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/*==========================================================================
  Function Prototypes
========================================================================== */

/**
This function processes the given input samples using series of iir filters.
*/
ADSPResult afe_port_apply_sidetone_iir(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo,
                                       uint32_t nSamplesIn);

/**
  This function set the params for the module
  AFE_MODULE_SIDETONE_IIR_FILTER. It supports the following
  Params: AFE_PARAM_ID_ENABLE
        : AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration
        data to fetch.
  @param[in] param_size  Actual Param size


  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */

ADSPResult afe_port_set_sidetone_iir_filter_module_params(afe_dev_port_t *pDevPort,
                                                          uint32_t param_id,
                                                          int8_t *param_buffer_ptr,
                                                          uint32_t param_size);

/**
  This function get the params for the module
  AFE_MODULE_SIDETONE_IIR_FILTER. It supports the following
  Params: AFE_PARAM_ID_ENABLE
        : AFE_PARAM_ID_SIDETONE_IIR_FILTER_CONFIG

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[out] param_buffer_ptr Pointer where the calibration
        data to store.
  @param[in] param_buf_len  Param size
  @param[out] params_buffer_len_req_ptr  Actual Param size

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */

ADSPResult afe_port_get_sidetone_iir_filter_module_params(afe_dev_port_t *pDevPort,
                                                          uint32_t param_id,
                                                          int8_t* param_buffer_ptr,
                                                          int32_t param_buf_len,
                                                          uint16_t* params_buffer_len_req_ptr);

/**
  This function is to deallocate sidetone IIR resources

  @param[in] pDevPort Pointer to the instance of the port.

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_sidetone_iir_dealloc_resource(afe_dev_port_t *pDevPort);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_AFE_SIDETONEIIR_H_
