/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/inc/bgt_handler_fwk.h#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   This file contains Background Task Handler implementation
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/
#ifndef __BGT_HANDLER_FWK_H__
#define __BGT_HANDLER_FWK_H__

/*==========================================================================
  Include files
  ========================================================================== */
#include "Elite.h"
#include "Elite_fwk_extns_background_processing.h"
#include "bgt_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================
  Macro defines
========================================================================== */

/** Indicates that the rest of the payload is an BGT deinit. */
#define BGT_HANDLER_CMD_DEINIT  0x00100001
/** Indicates that the rest of the payload is an BGT destroy. */
#define BGT_HANDLER_CMD_DESTROY 0x00100002


/** Payload structure for ELITE_CUSTOM_MSG sent to a service */
typedef struct elite_msg_custom_bgt_t
{
   qurt_elite_queue_t *buffer_returnq_ptr;
   /**< Queue to which this payload buffer must
    be returned. */
   qurt_elite_queue_t *responseq_ptr;
   /**< Queue to which to send the
    acknowledgement. NULL indicates that
    no response is required. */
   uint32_t client_token;
   /**< Token that is to be given in the
    acknowledgement. This is different than
    the response_result and can be used to
    identify who sent the acknowledgement
    back by the server. */
   uint32_t response_result;
   /**< This is to be filled with the
    acknowledgement results by the client.*/
   uint32_t sec_op_code;
   /**< Secondary operation code indicating
    the format for the rest of the
    payload. */
   void *payload_ptr;
   /**< payload pointer. */
} elite_msg_custom_bgt_t;


/** Param structure to create the BGT */
typedef struct bgt_create_param_t
{
   char                      thread_name[QURT_ELITE_DEFAULT_NAME_LEN];
   capi_v2_bgt_module_info_t *bgt_module_info_ptr;
   qurt_elite_queue_t        *svc_cmd_q_ptr;
   uint32_t                  svc_bgt_destroy_opcode;
}bgt_create_param_t;


/*==========================================================================
  Function Declarations
========================================================================== */

/**
 * This function creates bgt thread and initializes parameters
 *
 * @param[in] bgt_info_handle: address of the pointer to the bgt_info_pool.
 * @param[in] algo: algo type of the created bgt handler
 *
 * @return ADSPResult
 */
ADSPResult bgt_create(module_bgt_info_t **bgt_info_pptr, bgt_create_param_t *bgt_create_param_ptr);

/**
 * This function sends bgt commands
 *
 * @param[in] ptr to the bgt_info struct created by bgt_create
 * @param[in] ptr to the payload of the cmd, generally algo cfg structs.
 * @param[in] opCode of the cmd to be sent.
 *
 * @return ADSPResult
 */
ADSPResult bgt_send_cmd(module_bgt_info_t *bgt_info_ptr, uint32_t opCode);

/**
 * This function destroys bgt thread and does all the cleanups. Executed by the fwk svc.
 *
 * @param[in] bgt_info_handle: ptr to the bgt_info struct created by bgt_create
 * @return ADSPResult
 */
ADSPResult bgt_destroy(module_bgt_info_t *bgt_info_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // __BGT_HANDLER_FWK_H__
