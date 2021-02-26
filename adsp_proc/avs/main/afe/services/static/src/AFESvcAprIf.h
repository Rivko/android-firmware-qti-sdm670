/*========================================================================
   This file contains AFE APR handler implementation

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFESvcAprIf.h#1 $
 ====================================================================== */
#ifndef _AFE_SVCAPRIF_H_
#define _AFE_SVCAPRIF_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInterface.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * function to set AFE APR handle.
 *
 * @param[in] handle handle to be set
 * @return return code ADSPResult
 */
ADSPResult afe_svc_set_apr_handle(elite_apr_handle_t handle);


/**
 * function to free APR packet.
 *
 * @param[in] pPkt pointer to APR packet
 * @return return code ADSPResult
 */
ADSPResult afe_svc_free_apr_pkt(  elite_apr_packet_t* pPkt); 

/**
 * APR callback function for sending cmd to AFE.
 *
 * @param[in] APR cmd packet
 * @param[in] dispatch_data not used
 * @return return code
 */
int32_t afe_svc_apr_call_back_fct(elite_apr_packet_t* packet, void* dispatch_data);

/**
 * wrapper function to generate APR ack to client.
 *
 * @param[in] pPkt This is the received packet that requires ACK.
 * @param[in] status of the ack.
 * @param[in] pAckPayload payload that is required in ACK, specified by caller
 * @param[in] ulSize payload size.
 * @param[in] ulAckOpcode Optonal The opcode for ACK.
 * @return return code ADSPResult
 */
ADSPResult afe_svc_generate_ack(  elite_apr_packet_t* pPkt,
                                       ADSPResult status,                                       
                                       void *pAckPayload,
                                       uint32_t ulSize,
                                       uint32_t ulAckOpcode
                                    ) ;

/**
 * function to generate End of Stream ack for ASM.
 *
 * @param[in] pClientInfo EoS client info
 * @return return code ADSPResult
 */
ADSPResult afe_svc_generate_eos_ack(elite_msg_data_eos_info_t *pClientInfo);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif

