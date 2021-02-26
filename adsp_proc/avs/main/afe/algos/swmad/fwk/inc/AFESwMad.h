#ifndef _AFE_SWMAD_API_H
#define _AFE_SWMAD_API_H
/*========================================================================
   This file is the header file for AFE sw mad frame work implementation

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: TBD
 ====================================================================== */
#include "mmdefs.h"
#include "adsp_error_codes.h"
/*==========================================================================
  Include files
  ========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif
/*========================================================================
  Global Variables
========================================================================== */

/*==========================================================================
  Function Declarations
========================================================================== */

/**<
 * This function is to allocate resources related to sw mad
 * during port start .
 *
 * @param[in] ptr Pointer to the afe port structure.
 * @return
 * ADSP_EOK error code.
 * */
ADSPResult afe_port_sw_mad_alloc_resources(void* pDevPort);

/**<
 * This function is to de-allocate resources related to sw mad
 *during port stop .
 *
 * @param[in] ptr Pointer to the afe port structure.
 *
 * @return
 * ADSP_EOK error code.
 **/
ADSPResult afe_port_sw_mad_dealloc_resources(void* pDevPort);

/**
 * This function is the init function of sw mad thread
 *
 * @param[in] cfg_ptr: pointer to the cfg struct
 * @param[in] data_ptr: pointer to the swmad_data struct
 * @return ADSPResult
 */
ADSPResult afe_swmad_init_data(void *swmad_data_ptr, void **data_ptr);

/**
 * This function deinits swmad data of sw mad thread
 *
 * @param[in] data_ptr: ptr to the swmad_data struct
 * @return ADSPResult
 */
ADSPResult afe_swmad_deinit_data(void **data_ptr);

/**
  This function Is the swmad process function when received data.

  @param[in] data_ptr: ptr to the swmad data struct.

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_swmad_process(void *data_ptr);

/**
  This function set the params for the module
  AFE_MODULE_HW_MAD. It supports the following Params:
  AFE_PARAM_ID_HW_MAD_CFG
  AFE_PARAM_ID_HW_MAD_CTRL
  AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG


  @param[in] pDevPort: Pointer to the instance of the port.
  @param[in] param_id: Parameter id  .
  @param[in] param_buffer_ptr: Pointer from where the calibration
        data to fetch.
  @param[in] params_buffer_len_req_ptr: Actual Param size


  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
/**
  This function set the params for the module
  AFE_MODULE_SW_MAD. It supports the following Params:
  AFE_PARAM_ID_SW_MAD_CFG
  AFE_PARAM_ID_SVM_MODEL
  AFE_PARAM_ID_ENABLE

  @param[in] pDevPort: Pointer to the instance of the port.
  @param[in] param_id: Parameter id  .
  @param[in] param_buffer_ptr: Pointer from where the calibration
        data to fetch.
  @param[in] params_buffer_len_req_ptr: Actual Param size


  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_set_swmad_module_params(void *port_ptr,uint32_t param_id,
                                                      int8_t* params_buffer_ptr,uint16_t param_size);

/**
  This function set the params for the module
  AFE_MODULE_SW_MAD. It supports the following Params:
  AFE_PARAM_ID_SW_MAD_CFG
  AFE_PARAM_ID_SVM_MODEL
  AFE_PARAM_ID_ENABLE

  @param[in] port_ptr: Pointer to the instance of the port.
  @param[in] param_id: Parameter id  .
  @param[in] param_buffer_ptr: Pointer from where the calibration
        data to fetch.
  @param[in] param_buf_len: size of the getparam buffer
  @param[out] params_buffer_len_req_ptr: Actual Param size


  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_get_swmad_module_params(void *port_ptr,
                                            uint32_t param_id, 
                                            int8_t *param_buffer_ptr,
                                            int32_t param_buf_len,
                                            uint16_t *params_buffer_len_req_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_AFE_SWMAD_API_H
