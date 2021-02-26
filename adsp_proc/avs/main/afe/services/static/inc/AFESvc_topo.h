/** @file AFESvc_topo.h

 @brief This file contains AFE service APPI functions 

 */

/*========================================================================
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFESvc_topo.h#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 4/8/2014 mk   Initial version
 ==========================================================================*/


/*-----------------------------------------------------------------------
   Copyright (c) 2012-2013 Qualcomm  Technologies, Inc.  All rights reserved.
   Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

#ifndef _AFESVC_TOPO_H_
#define _AFESVC_TOPO_H_

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qurt_elite_types.h"
#include "Elite_CAPI_V2.h"
#include "adsp_afe_service_commands.h"


#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/*
  This function is to set topology ID

  @param[out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cfg_ptr: pointer to configuration payload
  @param[in]     payload_size: size of the configuration payload

  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_set_topo_id(void *dev_port_ptr, int8_t *cfg_ptr, uint32_t payload_size);

/*
  Callback function for modules in topology

  @param[in]     context_ptr: Context pointer
  @param[in]     id: event id
  @param[in]     event_ptr: pointer to the event

  @return
  capi_v2_err_t

  @dependencies
  None.
*/
capi_v2_err_t afe_topo_event_callback(void *context_ptr, capi_v2_event_id_t id, capi_v2_event_info_t *event_ptr);

/*
  This function is to start the topology

  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_start(void *dev_port_ptr);

/*
  This function is to stop the topology

  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_stop(void *dev_port_ptr);

/*
  This function is to destroy the topology

  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_destroy(void *dev_port_ptr);

/*
  This function is the call back for creating the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_create_cb(void *handle, void *dev_port_ptr, uint32_t cb_type);

/*
  This function is the call back for starting the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_start_cb(void *topo_ptr, void *dev_port_ptr, uint32_t cb_type);

/*
  This function is the call back for stoping the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_stop_cb(void *topo_ptr, void *dev_port_ptr, uint32_t cb_type);

/*
  This function is the call back for destroying the topology

  @param[in]     handle: handle to topology pointer
  @param[in/out]     dev_port_ptr: pointer to AFE dev port
  @param[in]     cb_type: call back type
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_destroy_cb(void *handle, void *dev_port_ptr, uint32_t cb_type);

/*
  This function is to set the parameters to the topology

  @param[in]     dev_port_ptr: pointer to AFE dev port
  @param[in]     module_id: module id
  @param[in]     param_id: param id
  @param[in]     param_size: size of the parameter
  @param[in]     calibration_data_pay_load_ptr: pointer to the parameter buffer

  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_setparam(void *dev_port_ptr, uint32_t module_id, uint16_t instance_id, uint32_t param_id, uint16_t param_size, int8_t *calibration_data_pay_load_ptr);

/*
  This function is to acknowledge the codec interrupt

  @param[in]     client_info_ptr: pointer to codec information structure
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_ack_cdc_intr(void *client_info_ptr);

/*
  This function is to get the interrupt index used in codec handler

  @param[in]     int_id: interrupt GUID
  @param[out]    cdc_int_id: interrupt index used in codec handler
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_cdc_get_int_index(uint32_t int_id, uint16_t *cdc_int_id);

/*
  This function is to get the interrupt GUID

  @param[in]    cdc_int_id: interrupt index used in codec handler
  @param[out]     int_id: interrupt GUID
  
  @return
  ADSPResult

  @dependencies
  None.
*/
ADSPResult afe_topo_cdc_get_int_id(uint16_t cdc_int_id, uint32_t *int_id);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_AFESVC_TOPO_H_



