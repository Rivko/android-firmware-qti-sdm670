/**
@file ListenStreamMgr_AprIf.cpp

@brief This file contains the implementation for session-control-related functions
for ListenStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/listen/services/static_svcs/listen_stream_mgr/src/ListenStreamMgr_AprIf.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     ------------------------------------------------------- 
12/18/2012  Sudhir      Initial version
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/



/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#include "ListenStreamMgr_AprIf.h"
#include "ListenStreamMgr_AprDataBase.h"
#include "adsp_lsm_api.h"
#include "ListenStreamMgr_Deprecate.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/



/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */


/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */
int32_t lsm_apr_callback_function(elite_apr_packet_t* packet, void* dispatch_data)
{
  uint32_t rc;
  uint32_t ulOpCode         = elite_apr_if_get_opcode( packet);
  elite_apr_port_t  port   = elite_apr_if_get_dst_port( packet);
  elite_msg_any_t msg;
  msg.unOpCode = ELITE_APR_PACKET;
  msg.pPayload = (void*)packet;

  int32_t result = APR_EOK;
  qurt_elite_queue_t *pDestQ = NULL;

  MSG_3( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"LSM:Rec cmd 0x%lx at [Addr=0x%lx, Port=0x%lx]" ,
         ulOpCode, (uint32_t) ( elite_apr_if_get_dst_addr( packet)) , (uint32_t) (port) );

  switch ( ulOpCode )
  {
    //If there are any commands to static service need to add the opcodes here.

    case LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS:
    case LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS:
    case LSM_SESSION_CMD_OPEN_TX_V2:
    case LSM_SESSION_CMD_CLOSE:
    /* Deprecated Support */
    case LSM_CMD_ADD_TOPOLOGIES:
    case LSM_SESSION_CMD_OPEN_TX:
    /* End - Deprecated Support */
    {
      if ( (ADSP_FAILED(result = lsm_apr_router_get_svc_cmd_q(&pDestQ))))
      {
#ifdef LSM_DEBUG
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to get svc cmd queue");
#endif
        goto _bail_out;
      }
    }
    break;

    case LSM_DATA_CMD_READ:
      // check for valid handle
      if ( (ADSP_FAILED( result = lsm_apr_router_get_buf_q(port,&pDestQ))))
      {
#ifdef LSM_DEBUG
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LSM: fail to get buf queue for port 0x%lx", (uint32_t) port);
#endif
        goto _bail_out;
      }
      break;
    default:
      //get dyn svc cmdQ of the session for remaining commands
      if ( (ADSP_FAILED(result = lsm_apr_router_get_dyn_svc_cmd_q(port,&pDestQ))))
      {
#ifdef LSM_DEBUG
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed to get cmd queue 0x%lx", (uint32_t) port);
#endif
        goto _bail_out;
      }
      break; 
  }  // get the corresponding queue based on opcode

  // Push msg to the queue.
  if ( NULL != pDestQ )
  {
    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( pDestQ, (uint64_t*) (&msg))))
    {
#ifdef LSM_DEBUG
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Fail to push to queue 0x%p, port=0x%lx", pDestQ, (uint32_t) port);
#endif
      goto _bail_out;
    }
  }
  //this will send only during success.
  return result;
_bail_out:
  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Failed 0x%x", result);
  // send error message to client: this also frees the packet.
  if(ADSP_FAILED(rc = elite_apr_if_end_cmd( lsm_apr_handle, packet, result)))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"lsm_apr_callback_function: Failed in elite_apr_if_end_cmd with return code: %ld", rc);
#endif
  }
  //returning fail causes another free in APR. therefore return success.
  return APR_EOK;
}

ADSPResult lsm_generate_ack(elite_apr_packet_t* pPkt,
                            ADSPResult status,
                            void *pAckPayload,
                            uint32_t ulSize,
                            uint32_t ulAckOpcode)
{
  ADSPResult result;
  uint32_t opCode = elite_apr_if_get_opcode(pPkt);

  switch (opCode)
  {
    case LSM_DATA_CMD_READ:
    {
      if ( pAckPayload == NULL || ulSize < sizeof(lsm_data_event_read_done_t)  )
      {
#ifdef LSM_DEBUG
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "LSM:Port=0x%x: ISR Error: LSM_DATA_CMD_READ ACK received a payload, [0x%lx,%lu] ",
              pPkt->dst_port,(uint32_t) pAckPayload, ulSize );
#endif
        result = ADSP_EFAILED;
      }
      else
      {

        MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "Sending LSM_DATA_EVENT_READ_DONE for LSM:Port=0x%x: status 0x%lx, token %lu, Size %lu ",
              pPkt->dst_port,(uint32_t) status, pPkt->token, ulSize );
        result = elite_apr_if_alloc_send_event( lsm_apr_handle,
                                                pPkt->dst_addr,   /// Switch src/dst address field in ack
                                                pPkt->dst_port,
                                                pPkt->src_addr,
                                                pPkt->src_port,
                                                pPkt->token,
                                                LSM_DATA_EVENT_READ_DONE,
                                                pAckPayload,
                                                ulSize );

        elite_apr_if_free( lsm_apr_handle, pPkt);
      }
      break;
    }
    /* Deprecated */
    case LSM_SESSION_CMDRSP_GET_PARAMS:
    case LSM_SESSION_CMD_GET_PARAMS:
    {
      result = lsm_generate_ack_get_param_v1(pPkt,  status, pAckPayload, ulSize);
      break;
    }
    /* End - Deprecated */
    case LSM_SESSION_CMDRSP_GET_PARAMS_V2:
    case LSM_SESSION_CMDRSP_GET_PARAMS_V3:
    {
      /*For in-band GET_PARAM, LSM has already allocated APR packet with necessary fields*/
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Sending LSM_SESSION_CMDRSP_GET_PARAMS for LSM:Port=0x%x: status 0x%lx, token %lu, Size %lu",
            pPkt->dst_port,(uint32_t) status, pPkt->token, ulSize);
      result = elite_apr_if_async_send( lsm_apr_handle, pPkt );
      break;
    }

    case LSM_SESSION_CMD_GET_PARAMS_V2:
    case LSM_SESSION_CMD_GET_PARAMS_V3:
    {
      uint32_t response_opcode = (LSM_SESSION_CMD_GET_PARAMS_V2 == opCode)?
                                  LSM_SESSION_CMDRSP_GET_PARAMS_V2: LSM_SESSION_CMDRSP_GET_PARAMS_V3;

      /* For out-of-band GET_PARAM command need to allocate response packet*/
      result = elite_apr_if_alloc_send_ack(lsm_apr_handle,
                                           pPkt->dst_addr,   /// Switch src/dst address field in ack
                                           pPkt->dst_port,
                                           pPkt->src_addr,
                                           pPkt->src_port,
                                           pPkt->token,
                                           response_opcode, ///opcode,
                                           pAckPayload,
                                           ulSize );
      elite_apr_if_free(lsm_apr_handle, pPkt);
      break;
    }

    default:
      if (pAckPayload && ulAckOpcode)
      {

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "GenerateAck status 0x%lx, Ack 0x%lx", (uint32_t) status, ulAckOpcode);
        result = elite_apr_if_alloc_send_ack(
            lsm_apr_handle,
            pPkt->dst_addr, // Switch src/dst address field in ack
            pPkt->dst_port,
            pPkt->src_addr,
            pPkt->src_port, pPkt->token,
            ulAckOpcode,
            pAckPayload, ulSize);

        elite_apr_if_free(lsm_apr_handle, pPkt);

      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "GenerateAck status 0x%lx, for opcode: 0x%lx ",(uint32_t) status, opCode);
        result = elite_apr_if_end_cmd(lsm_apr_handle, pPkt, status);
      }

      break;
  }

  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "GenerateAck ISR fail to send ack 0x%lx", (uint32_t) result);
  }

  return result;
}


