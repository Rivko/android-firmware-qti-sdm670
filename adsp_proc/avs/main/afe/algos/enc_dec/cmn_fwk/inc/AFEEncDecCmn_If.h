/*========================================================================
  This file contains functions/structures to interface with the encoder.

  Copyright (c) 2016 QUALCOMM Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/algos/enc_dec/cmn_fwk/inc/AFEEncDecCmn_If.h#1 $

  Edit History
  when           who  what, where, why
  --------           ---      -------------------------------------------------------
  08/19/16   KR     Created file.
  ========================================================================== */
#ifndef AFEENCDECCMN_IF_H
#define AFEENCDECCMN_IF_H

#include "AFEInternal.h"
#include "EliteLinkedList.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*
 * This structure is used to cache client SET_PARAMS calls.
 */
typedef struct param_info_t { 
    uint32_t param_id;
    int8_t *param_buffer_ptr;
    uint16_t param_size;
} param_info_t;

/*=============================================================================
 * FUNCTION         afe_create_enc_dec
 *
 * DESCRIPTION    function to create the encoder and/or decoder svc and all the associated queues,
 *  buffers and establish connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult. ADSP_EOK will be returned for stubs OR use-case does not involve
 * encoder/decoder
 ===============================================================================*/
ADSPResult afe_create_enc_dec(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_create_enc_dec_dev_buffers
 *
 * DESCRIPTION    function to create the encoder and/or decoder svc related AFE-side device buffers
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult. ADSP_EOK will be returned for stubs OR use-case does not involve
 * encoder/decoder
 ===============================================================================*/
ADSPResult afe_create_enc_dec_dev_buffers(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_delete_cached_params_list
 *
 * DESCRIPTION    helper function to delete an encoder or decoder's list of cached
 *                  parameters. The linked list's delete cannot be used because
 *                  it does a shallow delete.
 *
 * @param[in]       cached_params_list the head of the linked list of param_info_t
 *                  to delete.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_delete_cached_params_list(elite_linked_list_node_t *cached_params_list);

/*=============================================================================
 * FUNCTION         afe_port_dev_destroy_enc_dec
 *
 * DESCRIPTION    function to destroy the encoder and/or decoder svc and all the associated queues,
 *  buffers and remove connections with AFE svc.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_destroy_enc_dec(afe_dev_port_t  *pDevPort);

/*=============================================================================
 * FUNCTION         afe_set_enc_dec_params
 *
 * DESCRIPTION    function to set parameters on the encoder/decoder svc
 *
 * @param[in]       pDevPort device port instance.
 * @param[in]       module_id unique moduleID for this parameter.
 * @param[in]       param_id unique paramID for this parameter.
 * @param[in]       param_buffer_ptr Pointer to the start of the parameter buffer.
 * @param[in]       param_size Size of the parameter.
  * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult.
 ===============================================================================*/
ADSPResult afe_set_enc_dec_params(afe_dev_port_t *pDevPort, uint32_t module_id, uint32_t param_id, int8_t *param_buffer_ptr, uint16_t param_size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //AFEENCDECCMN_IF_H
