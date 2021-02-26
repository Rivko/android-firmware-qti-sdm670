/**
@file AudStreamMgr_StreamRespHandler.cpp

@brief This file contains the implementation for session response handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_StreamRespHandler.cpp#2 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013   RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_Stream.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_SessionRespHandler.h"
#include "AudioStreamMgr_StreamRespHandler.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "adsp_media_fmt.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"
#include "AudioStreamMgr_adsppm.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
extern uint32_t asmMemoryMapClient;

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/* Destroy_Svc handler */
ADSPResult AudioStreamMgr_SessionRespQDestroySvcHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_cmd_destroy_svc_t *pPayload = (elite_msg_cmd_destroy_svc_t *) pSession->msg.pPayload;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   ADSPResult result=ADSP_EOK;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Stream ID",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   ASM_GraphNode *pSvcNode = NULL;
   if(ASM_NODE_MAX > node_ind) pSvcNode = &pStream->nodes[node_ind];

   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   ASM_OnPopFromRespQ(pMe, pSession);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: destroy fail",
            pSession->ucInternalSessionId,  stream_id);
      AudioStreamMgr_LogErrMsg(&pSession->msg);
   }

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "AudioStreamMgr [%u,%u]:svc [0x%lx] Destroying",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken);

   /// qurt thread join on the saved threadID. At this point, pSvcHandle
   /// is no longer valid.
   qurt_elite_thread_join(pSvcNode->threadId, &result);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]:svc [0x%lx] Destroyed",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken);


   /// reset svc handle
   pSvcNode->pSvcHandle = NULL;

   elite_msg_release_msg(&pSession->msg);

   /// all modules have been destroyed. ACK close success
   if ( 0 == pSession->unReqResps )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Graph destroy complete",
            pSession->ucInternalSessionId,  stream_id);

      if ( pSession->nOverallResult )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: close fail",
               pSession->ucInternalSessionId,  stream_id);
         AudioStreamMgr_SessionAckFail( pMe, pSession, pSession->nOverallResult );
         return ADSP_EFAILED;
      }

      /*If this was a destroy to a single decoder in a two stream
      gapless session*/
      uint32_t num_open_streams = ASM_GetNumStreamsOpen(pSession);
      if ((num_open_streams == 2) && ASM_IsGaplessSession(pSession))
      {
         memset( pSvcNode, 0, sizeof(ASM_GraphNode));

         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, NULL, 0, 0 );

         ASM_ReleaseGaplessStream(pSession,pStream);

         //In gapless, as only 2 streams can be open at a time, &
         //we just closed one stream in ASM_ReleaseGaplessStream only one more stream is open
         AudioStreamMgr_StreamStateType *pOpenStream = ASM_GetAnOpenedStream(pSession);
         if(NULL == pOpenStream)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid OpenStream ID",pSession->ucInternalSessionId);
            return ADSP_EFAILED;
         }
         pSession->oldestOpenStreamId = pOpenStream->ucStreamId;

         return result;
      }

      // If Tx matrix or stream router, now we need to disconnect.
      if (TRUE == AudioStreamMgr_StreamTopoIsTxTopo(pStream))
      {
         result = AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream, DISCONNECT_MATRIX, ADM_TX_DIR);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: AudioStreamMgr_SessionReturnDeviceHandle fail",pSession->ucInternalSessionId,  stream_id);
         }
      }
      else
      {
         result = AudioStreamMgr_StreamAckCloseStream(pMe, pSession, pStream);
      }

   } /// all modules have been destroyed

   return result;
}

/* Set Param Handler */
ADSPResult AudioStreamMgr_SessionRespQSetParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_param_any_t *pPayload = (elite_msg_param_any_t *) pSession->msg.pPayload;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: SetParm ACK Tok 0x%lx, Res 0x%lx, ParamId 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult, pPayload->unParamId);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: SetParam fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
   }

   switch ( pPayload->unParamId )
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         return AudioStreamMgr_SessionRespQGenericAck(pMe, pSession); //calls ASM_OnPopFromRespQ
      }
      default:
         ASM_OnPopFromRespQ(pMe, pSession);
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Uexpected SetParm ACK T0x%lx, Id0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unClientToken, pPayload->unParamId);
         break;
   } // switch ( pPayload->unParamId ) {

   // release the message and return
   return elite_msg_release_msg(&pSession->msg);

}

/* Connect ACK handler */

ADSPResult AudioStreamMgr_SessionRespQConnectHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_cmd_connect_t *pPayload = (elite_msg_cmd_connect_t *) pSession->msg.pPayload;
   ADSPResult result = ADSP_EOK;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);
   bool_t is_custom_cmd = ASM_GetIsCustomCmdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   if (is_custom_cmd)
   {
      // in case of LLNP Rx, ULL, and ULLPP, this is entered whenever ADM sends custom command to
      // connect the device to the decoder or PP (this happens during map routing)
      elite_msg_custom_header_t *pCustomPayload = (elite_msg_custom_header_t *) (pStSession->cmdMsg.pPayload);
      if (ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT == pCustomPayload->unSecOpCode)
      {
         //ack back to ull stream session and afe connect command
         return AudioStreamMgr_SessionRespQGenericAckForInternalMsg( pMe, pSession);
      }

      // it should never reach here for custom commands which are issued during ULL/LLNP/ULLPP map routing
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Unexpected response.",
            pSession->ucInternalSessionId,  stream_id);

      ASM_OnPopFromRespQ(pMe, pSession);
      elite_msg_release_msg(&pSession->msg);
      return ADSP_EFAILED;
   }

   ASM_OnPopFromRespQ(pMe, pSession); //AudioStreamMgr_SessionRespQGenericAckForInternalMsg calls ASM_OnPopFromRespQ (see above uLL)

   ASM_GraphNode *pSvcNode = &pStream->nodes[node_ind];
   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   if ( ADSP_FAILED(pPayload->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Connection fail",
            pSession->ucInternalSessionId,  stream_id);
      if (0 == pSession->unReqResps)
      {
         AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
         AudioStreamMgr_DeallocateSessionIfLastStream(pMe, pSession->ucInternalSessionId);
         elite_msg_release_msg(&pSession->msg);
      }
      return pPayload->unResponseResult;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]:svc ID [0x%lx] connected Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pSvcNode->pSvcHandle->unSvcId,
         pPayload->unResponseResult);

   if(pSession->unReqResps != 0)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AudioStreamMgr [%u,%u]: Graph Incomplete: [0x%lx,0x%lx]",
            pSession->ucInternalSessionId,  stream_id,
            pSession->unReqResps, pSession->nOverallResult);
   }
   else
   {
      /*graph connect is complete*/
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,
            "AudioStreamMgr [%u,%u]: Graph Complete: [0x%lx,0x%lx]",
            pSession->ucInternalSessionId,  stream_id,
            pSession->unReqResps, pSession->nOverallResult);

      if (ASM_STREAM_BAD_STATE == pStream->state)
      {
         AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
      }

      // call utility function to ack success and register data queu, etc.
      result = AudioStreamMgr_StreamAckOpenStream(pMe,pSession, pStream);

   } /// End of necessary processing after graph is connected

   elite_msg_release_msg(&pSession->msg);
   return  result;
}

/**
 * DisConnect ACK handler
 */
ADSPResult AudioStreamMgr_SessionRespQDisConnectHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_cmd_connect_t *pPayload = (elite_msg_cmd_connect_t *) pSession->msg.pPayload;
   ADSPResult result = ADSP_EOK;

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);
   bool_t is_custom_cmd = ASM_GetIsCustomCmdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Stream ID",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   if (is_custom_cmd)
   {
      //in case of LLNP, ULL & ULLPP Rx this is entered whenever ADM sends custom command and we disconnect from downstream service (AFE or COPP).
      //node is dec node.
      elite_msg_custom_header_t *pCustomPayload = (elite_msg_custom_header_t *) (pStSession->cmdMsg.pPayload);

      //check if this disconnect is because of ELITEMSG_CUSTOM_ASM_ULL_STREAM_AFE_DISCONNECT
      if (ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_DISCONNECT == pCustomPayload->unSecOpCode)
      {
         // ack back to ull stream session and afe disconnect command
         return AudioStreamMgr_SessionRespQGenericAckForInternalMsg( pMe, pSession);
      }

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Unexpected response.",
            pSession->ucInternalSessionId,  stream_id);

      ASM_OnPopFromRespQ(pMe, pSession);
      elite_msg_release_msg(&pSession->msg);
      return ADSP_EFAILED;
   }

   //AudioStreamMgr_SessionRespQGenericAckForInternalMsg calls ASM_OnPopFromRespQ
   ASM_OnPopFromRespQ(pMe, pSession);

   ASM_GraphNode *pSvcNode = &pStream->nodes[node_ind];
   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }

   uint32_t numOpenStreams = ASM_GetNumStreamsOpen(pSession);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Disconnection fail",
            pSession->ucInternalSessionId,  stream_id);
   }

   // For LLNP Tx, pSvcNode will be NULL. Guard against this case
   uint32_t unSvcId = (NULL != pSvcNode->pSvcHandle) ? pSvcNode->pSvcHandle->unSvcId : 0;
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]:svc ID [0x%lx] disconnected Res0x%lx",
         pSession->ucInternalSessionId,  stream_id, unSvcId, pPayload->unResponseResult);

   //Check whether audio path disconnect is finished
   // If so, on rx path, send disconnect to mtmx
   //        on tx path, should start destroy svc

   /* If MtMx is not disconnected, it might hold onto some PP's
   buffer. Causing PP can not be destroyed.
   if disconnected, need to info AFE that it is disconnected
   from PP so that it returns buffer to PP
    */
   if (0 == pSession->unReqResps )
   {
      if (0 != pSession->nOverallResult)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]:fail to close Res0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               (uint32_t) pSession->nOverallResult);
         AudioStreamMgr_SessionAckFail( pMe, pSession, pSession->nOverallResult);
      }
      else
      {
         //return Rx device handle except for gapless with 2 streams.
         if ( (pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle || pStream->nodes[ASM_NODE_RX_ST_RTR].pSvcHandle ||
               pStream->generic_rx_dummy_handle) &&
               (!ASM_IsGaplessSession(pSession) || ((numOpenStreams == 1) && (ASM_IsGaplessSession(pSession))) ))
         {
            if (ADSP_FAILED(result = AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream,
                  DISCONNECT_MATRIX, ADM_RX_DIR)))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: fail to return matrix handle",
                     pSession->ucInternalSessionId,  stream_id);
               goto __bailout;
            }
         }
         else
         {
            if ((numOpenStreams == 2) && (ASM_IsGaplessSession(pSession)))
            {
               //now that dec and switch svc are disconnected, vacate switch svc port.
               if(ADSP_FAILED(result = AudioStreamMgr_VacateSwitchSvcPort(pMe, pSession, pStream)))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Failed to vacate switch svc port",
                        pSession->ucInternalSessionId,  stream_id);
                  if(!pSession->unReqResps)
                  {
                     goto __bailout;
                  }
               }
            }
            else
            {
               //now that all dynamic services are disconnected, destroy the session
               if(ADSP_FAILED(result = AudioStreamMgr_StreamDestroy(pMe, pSession, pStream)))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Failed to destroy modules",
                        pSession->ucInternalSessionId,  stream_id);
                  if(!pSession->unReqResps)
                  {
                     goto __bailout;
                  }
               }
            }
         }
      } /// overall response is success
   } /// End of necessary processing after graph is connected

   return  elite_msg_release_msg(&pSession->msg);

   __bailout:
   elite_msg_release_msg(&pSession->msg);
   return AudioStreamMgr_SessionAckFail( pMe, pSession, result);

}


ADSPResult AudioStreamMgr_SessionRespQOnAdmDisConnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t*) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Stream ID",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   ASM_GraphNode *pSvcNode = &pStream->nodes[node_ind];
   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   ASM_OnPopFromRespQ(pMe, pSession);

   ADSPResult result = ADSP_EOK;

   if ( ASM_STREAM_DISCONNECTING != pStream->state )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Invalid State: %d",
            pSession->ucInternalSessionId,  stream_id, pSession->unState);
      return ADSP_ENOTREADY;
   }

 //  uint32_t svc_id = 0;
   //in LLNP/ULL/ULLPP Rx: this makes downstream node's handle null
   if (node_ind == ASM_NODE_RX_GENERIC)
   {
      pStream->generic_rx_dummy_handle = NULL;
   }
   else
   {
      if (node_ind != ASM_NODE_TX_GENERIC)
      {
         // Sessions that directly connect to the AFE will not have a valid service handle
		/* CR#2201203 : Commenting out the below line as it is leading to crash due to invalid pSvcHandle.
		   TODO : Need to root cause it. */
       //  svc_id = pSvcNode->pSvcHandle->unSvcId;
      }
      pSvcNode->pSvcHandle = NULL;
   }

   // Once disconnect, can ask all dynamic service to destroy.
 /*  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: device svc ID [0x%lx] Disconnected",
         pSession->ucInternalSessionId,  stream_id,svc_id);*/
   elite_msg_release_msg( &(pSession->msg));
   result = ASM_CloseStreamAfterDisconnect(pMe, pSession, pStream);

   return result;
}

ADSPResult AudioStreamMgr_SessionRespQOnAdmConnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult res = ADSP_EOK;
   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t*) pSession->msg.pPayload;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Stream ID",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   ASM_GraphNode *curr_node = &pStream->nodes[node_ind];
   ASM_GraphNode *pNode = curr_node;

   uint32_t direction;
   ASM_GraphNodeIndex req_node;

   /* save the Matrix mixer or stream router handle from response
   Later use this handle to connect PP/packetizer to Matrx/Stream Router */
   EliteMsg_CustomAdmStreamConnectType* pConnectMsgPayload =
         (EliteMsg_CustomAdmStreamConnectType*)pPayload;
   direction = pConnectMsgPayload->direction;

   ASM_OnPopFromRespQ(pMe, pSession);

   if ( ASM_STREAM_CONNECTING != pStream->state
         || pPayload->unResponseResult != ADSP_EOK )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to open due to invalid State %d OR response status %lu",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, pPayload->unResponseResult);

      res = pPayload->unResponseResult;
      if(ADSP_SUCCEEDED(pPayload->unResponseResult))
      {
         //In case of wrong session state
         res = ADSP_EUNEXPECTED;
      }
      goto __bailout;
   }

   uint32_t unPortDirection;
   ASM_direction asm_dir;

   if (ADM_RX_DIR == direction)
   {
      asm_dir = ASM_RX_DIR;
      unPortDirection = (pConnectMsgPayload->is_compressed) ? (STRTR_PORT_DIR_INPUT) :
            (MATRIX_MIXER_PORT_DIR_INPUT);
   }
   else
   {
      asm_dir = ASM_TX_DIR;
      unPortDirection = (pConnectMsgPayload->is_compressed) ? (STRTR_PORT_DIR_OUTPUT) :
            (MATRIX_MIXER_PORT_DIR_OUTPUT);
   }

   // For LLNP/ULL/ULLPP Rx: this makes the downstream node's handle null
   // For LLNP Tx, the ADM will return a dummy service handle on stream connect as it doesn't have
   // a real handle until map routing to a device is performed. The ASM is passive as the ADM will
   // perform the connection bookkeeping.
   if (node_ind == ASM_NODE_RX_GENERIC)
   {
      pStream->generic_rx_dummy_handle = (elite_svc_handle_t *)pConnectMsgPayload->phPort;
   }
   else
   {
      pNode->pSvcHandle = (elite_svc_handle_t *) pConnectMsgPayload->phPort;

      /// Remember the port id and direction on ack
      pStream->mtMxPortParam[asm_dir].unPortID = pConnectMsgPayload->unSessionID;
      pStream->mtMxPortParam[asm_dir].unPortDirection = unPortDirection;
   }

   //check if the stream needs another device handle.
   req_node = ASM_CheckForAnotherDeviceHandle(pStream, node_ind);

   //request the other device handle.
   if ( ((req_node == ASM_NODE_RX_MTX) && (!pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle)) ||
         ((req_node == ASM_NODE_RX_ST_RTR) &&  (!pStream->nodes[ASM_NODE_RX_ST_RTR].pSvcHandle)) ||
        ((req_node == ASM_NODE_RX_GENERIC) &&  (!pStream->generic_rx_dummy_handle)))
   {
      res = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream);
      if (ADSP_FAILED(res))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudStreamMgr [%u,%u]: FAILED to request handle",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }
   }
   else if ( ((req_node == ASM_NODE_TX_MTX)  && (!pStream->nodes[ASM_NODE_TX_MTX].pSvcHandle)) ||
         ((req_node == ASM_NODE_TX_ST_RTR)  && (!pStream->nodes[ASM_NODE_TX_ST_RTR].pSvcHandle)))
   {
      res = AudioStreamMgr_SessionRequestTxDeviceHandle(pMe, pSession, pStream, ASM_RELATIVE_TIMESTAMP);
      if (ADSP_FAILED(res))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudStreamMgr [%u,%u]: FAILED to request handle",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }
   }
   else
   {
      // Response for Rx matrix handle, or Tx matrix handle in Record mode
      //In either case, proceed to connect graph
      res = AudioStreamMgr_StreamConnectGraph(pMe, pSession, pStream);

      if ( ADSP_FAILED( res ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to connect graph",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }
   }

   //release the response message
   elite_msg_release_msg( &(pSession->msg));

   if (pSession->unReqResps == 0)
   {
      res = AudioStreamMgr_StreamAckOpenStream(pMe,pSession,pStream);
   }
   return ADSP_EOK;

   __bailout:
   //release the response message
   if (pSession->unReqResps == 0)
   {
      elite_msg_release_msg( &(pSession->msg));
   }

   //In case Tx matrix connection was established:
   //First disable the matrix output port
   if((pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle)
   {
      AudioStreamMgr_DisconnectTxMatrixAndWaitForAck(pMe, pSession, DISABLE_MATRIX, stream_id);
   }
   //destroy all dynamic modules in graph
   AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
   //in case failed to connect RX Matrix
   //then also need to disconnect Tx matrix
   if((pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle)
   {
      AudioStreamMgr_DisconnectTxMatrixAndWaitForAck(pMe, pSession, DISCONNECT_MATRIX, stream_id);
   }
   // ACK failure & switch to cmdQ, if no responses are expected.
   if (pSession->unReqResps == 0)
   {
      pSession->nOverallResult |= ADSP_EFAILED;
      res = AudioStreamMgr_StreamAckOpenStream(pMe,pSession,pStream);
   }
   AudioStreamMgr_DeallocateSessionIfLastStream(pMe, pSession->ucInternalSessionId);
   return res;
}

ADSPResult AudioStreamMgr_SessionRespQOnAdmDisable(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession
)
{
   /// This is the same as a disconnect command
   ADSPResult res = AudioStreamMgr_SessionRespQDisConnectHandler(pMe, pSession);
   return res;
}

ADSPResult AudioStreamMgr_SessionRespQOnGetSwitchSvcHandle(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{

   ADSPResult result = ADSP_EOK;
   elite_svc_handle_t *pSwitchSvcHandle;
   elite_msg_custom_header_t *payload_ptr = (elite_msg_custom_header_t*) pSession->msg.pPayload;

   ASM_OnPopFromRespQ(pMe, pSession);

   elite_msg_custom_switch_get_svc_handle* p_get_svc_handle_payload =
         (elite_msg_custom_switch_get_svc_handle*)payload_ptr;

   uint32_t client_token = p_get_svc_handle_payload->unClientToken;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(client_token);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   //Now connect the decoder to empty port of the switch
   ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);

   //use the service handle given by the switch
   pSwitchSvcHandle = p_get_svc_handle_payload->pSvcHandle;

   AudioStreamMgr_StreamStateType *pFirstGaplessStream = pSession->pFirstGaplessStrm;

   //copy common params from other stream: this way closing of session is easily handled.
   //however, events from PP/Mtmx still come to the stream which was first opened (pFirstGaplessStream)
   //from pFirstGaplessStream kips, delay, BW are read from, and they are set to zero at init.
   pStream->nodes[ASM_NODE_SWITCH].threadId = pFirstGaplessStream->nodes[ASM_NODE_SWITCH].threadId;
   pStream->nodes[ASM_NODE_SWITCH].pSvcHandle = pSwitchSvcHandle; //different port.

   pStream->nodes[ASM_NODE_PP].threadId = pFirstGaplessStream->nodes[ASM_NODE_PP].threadId;
   pStream->nodes[ASM_NODE_PP].pSvcHandle = pFirstGaplessStream->nodes[ASM_NODE_PP].pSvcHandle;

   pStream->nodes[ASM_NODE_RX_MTX].threadId = pFirstGaplessStream->nodes[ASM_NODE_RX_MTX].threadId;
   pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle = pFirstGaplessStream->nodes[ASM_NODE_RX_MTX].pSvcHandle;

   //copy mtmx port info also from the first stream.
   pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection = pFirstGaplessStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
   pStream->mtMxPortParam[ASM_RX_DIR].unPortID = pFirstGaplessStream->mtMxPortParam[ASM_RX_DIR].unPortID;

   result = AudioStreamMgr_ConnectStreamDecSvc(pMe, pSession, pStream, ASM_NODE_DEC, ASM_NODE_SWITCH, FALSE);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to send connect to decoder",
            pSession->ucInternalSessionId,
            stream_id );
      // generate ack for open command and free apr pakcet
      pSession->nOverallResult |= ADSP_EFAILED;
      AudioStreamMgr_StreamAckOpenStream(pMe, pSession, pStream);
      return result;
   }

   //release the message
   elite_msg_release_msg(&pSession->msg);

   return result;
}


ADSPResult AudioStreamMgr_SessionRespQOnSwitchVacatePort(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EOK;

   ASM_OnPopFromRespQ(pMe, pSession);

   elite_msg_custom_header_t *payload_ptr = (elite_msg_custom_header_t*) pSession->msg.pPayload;

   elite_msg_custom_switch_vacate_port* p_vacate_handle_payload =
         (elite_msg_custom_switch_vacate_port*)payload_ptr;

   uint32_t client_token = p_vacate_handle_payload->unClientToken;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(client_token);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   //now that all dynamic services are disconnected, destroy the session
   if(ADSP_FAILED(result = AudioStreamMgr_StreamDestroy(pMe, pSession, pStream)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Failed to destroy modules",
            pSession->ucInternalSessionId,  stream_id);
      if(!pSession->unReqResps)
      {
         goto __bailout;
      }
   }

   //release the message
   elite_msg_release_msg(&pSession->msg);

   return ADSP_EOK;

   __bailout:
   elite_msg_release_msg(&pSession->msg);
   return AudioStreamMgr_SessionAckFail( pMe, pSession, result);

}


/*Redirect to generic flush ACK*/
ADSPResult AudioStreamMgr_SessionRespQOnSwitchFlush(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   return AudioStreamMgr_SessionRespQOnFlushAck(pMe, pSession);
}



/*
Handle the Flush ACK from Proc and Matrix.
 */
ADSPResult AudioStreamMgr_SessionRespQOnFlushAck( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_apr_packet_t *pPkt;
   elite_msg_cmd_run_t *pPayload = (elite_msg_cmd_run_t *) pSession->msg.pPayload;
   uint32_t resp_result = pPayload->unResponseResult;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Flush ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, resp_result);

   ASM_OnPopFromRespQ(pMe, pSession);

   // Releasse message inside respQ
   elite_msg_release_msg(&pSession->msg);

   if (0 == pSession->unReqResps)
   {
      // send ack to flush cmd from client
      pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
      if (ADSP_FAILED(pSession->nOverallResult))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u, %u]: Flush ACK overall result 0x%lx",
               pSession->ucInternalSessionId,  stream_id,pSession->nOverallResult);
      }
      resp_result = AudioStreamMgr_GenerateAck( pPkt, pSession->nOverallResult, NULL, 0, 0 );
   }

   return resp_result;
}

/* GetPPTopologyModuleList */
ADSPResult AudioStreamMgr_SessionRespQGetPPTopologyModulesHandler(AudioStreamMgr_t *pMe,
	AudioStreamMgr_SessionStateType *pSession)
{
	elite_msg_param_any_t *pPayload = (elite_msg_param_any_t *) pSession->msg.pPayload;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   ADSPResult result = ADSP_EOK;

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Get PP Topology Module ID List ACK Tok 0x%lx, Res 0x%lx",
          pSession->ucInternalSessionId, stream_id,
          pPayload->unClientToken,  pPayload->unResponseResult);

   ASM_OnPopFromRespQ(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   // Generate ACK.
   // for in-band, pPkt is created by ASM & it has asm_stream_cmdrsp_get_pp_topo_module_list_t.
   // Client created packet with asm_stream_topology_module_info_t
   // is already released in case of in-band.
   if ( ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST == elite_apr_if_get_opcode(pPkt) ) //in-band
   {
	   asm_stream_cmdrsp_get_pp_topo_module_list_t *pRetPayload = (asm_stream_cmdrsp_get_pp_topo_module_list_t*)(elite_apr_if_get_payload_ptr(pPkt));
	   pRetPayload->status = pPayload->unResponseResult;

	   //no need to flush cache since data is in-band

	   result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
   }
   else if ( ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2 == elite_apr_if_get_opcode(pPkt) ) //in-band
   {
	   asm_stream_cmdrsp_get_pp_topo_module_list_v2_t *pRetPayload = (asm_stream_cmdrsp_get_pp_topo_module_list_v2_t*)(elite_apr_if_get_payload_ptr(pPkt));
	   pRetPayload->status = pPayload->unResponseResult;

	   //no need to flush cache since data is in-band

	   result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
   }
   // for out-of-band pPkt is created by client & it has asm_stream_cmd_get_pp_params_v2_t
   else
   {
	   elite_mem_shared_memory_map_t    bufferMemNode;
	   if(ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST == elite_apr_if_get_opcode(pPkt))
	   {
		   asm_stream_cmd_get_pp_topo_module_list_t *pAprPayload =
				   (asm_stream_cmd_get_pp_topo_module_list_t *) ( elite_apr_if_get_payload_ptr(pPkt));
		   bufferMemNode.unMemMapClient     = asmMemoryMapClient;
		   bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
		   bufferMemNode.unMemSize          = pAprPayload->param_max_size;
		   bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
		   bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
		   bufferMemNode.unVirtAddr         = 0;
	   }
	   else //ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
	   {
		   asm_stream_cmd_get_pp_topo_module_list_v2_t *pAprPayload =
				   (asm_stream_cmd_get_pp_topo_module_list_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt));
		   bufferMemNode.unMemMapClient     = asmMemoryMapClient;
		   bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
		   bufferMemNode.unMemSize          = pAprPayload->param_max_size;
		   bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
		   bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
		   bufferMemNode.unVirtAddr         = 0;
	   }

	   result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
											   bufferMemNode.unMemSize, &bufferMemNode);
	   if (ADSP_FAILED(result))
	   {
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for Get PP Topology Module ID List.",
					pSession->ucInternalSessionId,  stream_id);
		   pPayload->unResponseResult = ADSP_EFAILED;
	   }
	   else
	   {
		   result = elite_mem_flush_cache(&bufferMemNode);
		   if (ADSP_FAILED(result))
		   {
			   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Out-band Get PP Topology Module ID List failed to flush cache",
						pSession->ucInternalSessionId,  stream_id );
			   pPayload->unResponseResult = ADSP_EFAILED;
		   }
	   }
	   //send the ack anyway
       if(ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST == elite_apr_if_get_opcode(pPkt))
       {
    	   asm_stream_cmdrsp_get_pp_topo_module_list_t response;
    	   response.status = pPayload->unResponseResult;
    	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
    			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST);
       }
       else //ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
       {
    	   asm_stream_cmdrsp_get_pp_topo_module_list_v2_t response;
    	   response.status = pPayload->unResponseResult;
    	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
    			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2);
       }
   }
   // Output error messages in Q6 log
   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Get  PP Topology Module ID List fail, R%lx, T%lx",
              pSession->ucInternalSessionId,  stream_id,
              pPayload->unResponseResult,   pPayload->unClientToken);
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      result = ADSP_EFAILED;
   }

   // Release message inside respQ
   elite_msg_release_msg(&pSession->msg);

   return ADSP_EOK;
}

/* Get Param Handler */
ADSPResult AudioStreamMgr_SessionRespQGetParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_param_any_t *pPayload = (elite_msg_param_any_t *) pSession->msg.pPayload;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   ADSPResult result = ADSP_EOK;

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: GetParm ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);

   ASM_OnPopFromRespQ(pMe, pSession);

   switch ( pPayload->unParamId )
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

         elite_msg_param_cal_t *pCalPayload = (elite_msg_param_cal_t*)(pPayload);

         if(ELITEMSG_PARAM_ID_CAL == pPayload->unParamId)
         {
			 asm_stream_param_data_v2_t *psPPParamData = (asm_stream_param_data_v2_t *)(pCalPayload->pnParamData);
			 psPPParamData->param_size = (uint16_t)pCalPayload->unSize;
         }
         else
         {
			 asm_stream_param_data_v3_t *psPPParamData = (asm_stream_param_data_v3_t *)(pCalPayload->pnParamData);
			 psPPParamData->param_size = pCalPayload->unSize;
         }

         // Generate ACK.
         // for in-band, pPkt is created by ASM & it has asm_stream_cmdrsp_get_pp_params_v2_t. Client created packet with asm_stream_cmd_get_pp_params_v2_t
         // is already released in case of in-band.
         if ( ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt) ) //in-band
         {
            asm_stream_cmdrsp_get_pp_params_v2_t *pRetPayload = (asm_stream_cmdrsp_get_pp_params_v2_t*)(elite_apr_if_get_payload_ptr(pPkt));
            pRetPayload->status = pPayload->unResponseResult;

            //no need to flush cache since data is in-band

            result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
         }
         else if ( ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3 == elite_apr_if_get_opcode(pPkt) ) //in-band
         {
            asm_stream_cmdrsp_get_pp_params_v3_t *pRetPayload = (asm_stream_cmdrsp_get_pp_params_v3_t*)(elite_apr_if_get_payload_ptr(pPkt));
            pRetPayload->status = pPayload->unResponseResult;

            //no need to flush cache since data is in-band

            result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
         }
         // for out-of-band pPkt is created by client & it has asm_stream_cmd_get_pp_params_v2_t
         else
         {
             elite_mem_shared_memory_map_t    bufferMemNode;
        	 if(ASM_STREAM_CMD_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt))
        	 {
                 asm_stream_cmd_get_pp_params_v2_t *pAprPayload =
                       (asm_stream_cmd_get_pp_params_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt));
                 bufferMemNode.unMemMapClient     = asmMemoryMapClient;
                 bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
                 bufferMemNode.unMemSize          = pAprPayload->param_max_size;
                 bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
                 bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
                 bufferMemNode.unVirtAddr         = 0;
        	 }
        	 else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
        	 {
                 asm_stream_cmd_get_pp_params_v3_t *pAprPayload =
                       (asm_stream_cmd_get_pp_params_v3_t *) ( elite_apr_if_get_payload_ptr(pPkt));
                 bufferMemNode.unMemMapClient     = asmMemoryMapClient;
                 bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
                 bufferMemNode.unMemSize          = pAprPayload->param_max_size;
                 bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
                 bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
                 bufferMemNode.unVirtAddr         = 0;
        	 }

            //Flush the cache
            result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
                  bufferMemNode.unMemSize, &bufferMemNode);
            if (ADSP_FAILED(result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for GetParam.",
                     pSession->ucInternalSessionId,  stream_id);
               pPayload->unResponseResult = ADSP_EFAILED;
            }
            else
            {

               result = elite_mem_flush_cache(&bufferMemNode);
               if (ADSP_FAILED(result))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Out-band GetPPParam failed to flush cache",
                        pSession->ucInternalSessionId,  stream_id );
                  pPayload->unResponseResult = ADSP_EFAILED;
               }
            }

            if(ASM_STREAM_CMD_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt))
            {
         	   asm_stream_cmdrsp_get_pp_params_v2_t response;
         	   response.status = pPayload->unResponseResult;
         	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
         			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2);
            }
            else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
            {
         	   asm_stream_cmdrsp_get_pp_params_v3_t response;
         	   response.status = pPayload->unResponseResult;
         	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
         			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3);
            }
         }
         break;
      }
      default:
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: GetParm ACK invalid ParamID 0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unParamId );
         break;
      }
   } // Sanity check on ack param ID

   // Output error messages in Q6 log
   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: GetParam fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      result = ADSP_EFAILED;
   }

   // Releasse message inside respQ
   elite_msg_release_msg(&pSession->msg);

   return ADSP_EOK;

}

/* Get Param Handler */
ADSPResult AudioStreamMgr_SessionRespQGetMtMxStRtrParam( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   elite_msg_param_any_t *pPayload = (elite_msg_param_any_t *) pSession->msg.pPayload;
   uint8_t stream_id =  99; //set a wrong value
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   ADSPResult result = ADSP_EOK;

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_OnPopFromRespQ(pMe, pSession);

   uint32_t opCode = elite_apr_if_get_opcode(pPkt);
   switch (opCode)
   {
      case ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2: //in-band
      {
         stream_id = ASM_GET_STREAM_ID(elite_apr_if_get_src_port( pPkt));
         MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AudioStreamMgr [%u,%u]: GetMtMxStRtr ACK Tok 0x%lx, Res 0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unClientToken, pPayload->unResponseResult);

         // Generate ACK.
         // for in-band, pPkt is created by ASM & it has asm_session_cmdrsp_get_mtmx_strtr_params_v2_t.
         // Client created packet with asm_session_cmd_get_mtmx_strtr_params_v2_t
         // is already released in case of in-band. asm_session_param_data_t immediately follows this.
         asm_session_cmdrsp_get_mtmx_strtr_params_v2_t *pRetPayload = (asm_session_cmdrsp_get_mtmx_strtr_params_v2_t*)(elite_apr_if_get_payload_ptr(pPkt));
         pRetPayload->status     = pPayload->unResponseResult;

         //no need to flush cache since data is in-band

         result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
         break;
      }
      case ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2: //out-band
      {
         stream_id = ASM_GET_STREAM_ID(elite_apr_if_get_dst_port( pPkt));
         MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AudioStreamMgr [%u,%u]: GetMtMxStRtr ACK Tok 0x%lx, Res 0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unClientToken, pPayload->unResponseResult);

         // for out-of-band, pPkt is created by client & it has asm_session_cmd_get_mtmx_strtr_params_v2_t
         asm_session_cmd_get_mtmx_strtr_params_v2_t *pAprPayload =
               (asm_session_cmd_get_mtmx_strtr_params_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt) );

         elite_mem_shared_memory_map_t    bufferMemNode;
         bufferMemNode.unMemMapClient     = asmMemoryMapClient;
         bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
         bufferMemNode.unVirtAddr         = 0;
         bufferMemNode.unMemSize          = pAprPayload->param_max_size;
         bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
         bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;

         //Flush the cache
         result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
               bufferMemNode.unMemSize, &bufferMemNode);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for GetMtMxStRtr.",
                  pSession->ucInternalSessionId,  stream_id);
            pPayload->unResponseResult = ADSP_EFAILED;
         }
         else
         {
            result = elite_mem_flush_cache(&bufferMemNode);

            if (ADSP_FAILED(result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Out-band GetMtMxStRtr failed to flush cache",
                     pSession->ucInternalSessionId,  stream_id );
               pPayload->unResponseResult = ADSP_EFAILED;
            }
         }
         //send the ack anyway,
         //for out-band, asm_session_cmdrsp_get_mtmx_strtr_params_v2_t and asm_session_param_data_t are not contiguous
         result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult,
               &(pPayload->unResponseResult), sizeof(pPayload->unResponseResult), 0);
         break;
      }
      default:
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: GetMtMxStRtr ACK invalid opcode 0x%lx",
               pSession->ucInternalSessionId,  stream_id, opCode );
         break;
      }
   }
   // Output error messages in Q6 log
   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: GetMtMxStRtr fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      result = ADSP_EFAILED;
   }

   // Releasse message inside respQ
   elite_msg_release_msg(&pSession->msg);

   return ADSP_EOK;

}

ADSPResult AudioStream_Mgr_SessionRespQDecOpenStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   EliteMsg_CustomDecOpenStreamType *pPayload = (EliteMsg_CustomDecOpenStreamType *) pSession->msg.pPayload;
   ADSPResult result = ADSP_EOK;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   ASM_GraphNode *pSvcNode = &pStream->nodes[node_ind];
   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }
   if ( ADSP_FAILED(pPayload->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: open dec stream failed.",
            pSession->ucInternalSessionId,  stream_id);
      if (0 == pSession->unReqResps)
      {
         AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
         AudioStreamMgr_DeallocateSessionIfLastStream(pMe, pSession->ucInternalSessionId);
         elite_msg_release_msg(&pSession->msg);
      }
      return pPayload->unResponseResult;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Open dec stream success",
         pSession->ucInternalSessionId,  stream_id);

   pSvcNode->pSvcHandle = pPayload->pHandle;

   if (pSession->unReqResps == 0)
   {
      if (ASM_IsRxDeviceHandleNeeded(pStream)) //for write bit stream there's no device, but openPP, openCompr, transcode loopback may have devices.
      {
         if (ADSP_FAILED(result = AudioStreamMgr_SessionRequestRxDeviceHandle(pMe, pSession, pStream)))
         {
            goto __bailout;
         }
      }
      else
      {
         if (ADSP_FAILED((result = AudioStreamMgr_StreamConnectGraph(pMe,pSession,pStream))))
         {
            goto __bailout;
         }
      }
   }

   if (pSession->unReqResps == 0)
   {
      if (ADSP_FAILED(result))
      {

         // ack failure
         //destroy all dynamic modules in graph
         AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
      }

      pSession->nOverallResult |= result;

      // call utility function to ack success and register data queu, etc.
      result = AudioStreamMgr_StreamAckOpenStream(pMe,pSession, pStream);
   }

   __bailout:
   elite_msg_release_msg(&pSession->msg);
   return  result;
}

ADSPResult AudioStream_Mgr_SessionRespQDecCloseStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   EliteMsg_CustomDecCloseStreamType *pPayload = (EliteMsg_CustomDecCloseStreamType *) pSession->msg.pPayload;
   ADSPResult result = ADSP_EOK;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u]: Invalid Stream ID",pSession->ucInternalSessionId);
      return ADSP_EFAILED;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   ASM_GraphNode *pSvcNode = &pStream->nodes[node_ind];
   if(NULL == pSvcNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Service Node",pSession->ucInternalSessionId,stream_id);
      return ADSP_EFAILED;
   }

   if ( ADSP_FAILED(pPayload->unResponseResult))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: close dec stream failed.",
            pSession->ucInternalSessionId,  stream_id);
      if (0 == pSession->unReqResps)
      {
         AudioStreamMgr_StreamPanicHandler(pMe, pSession, pStream);
         AudioStreamMgr_DeallocateSessionIfLastStream(pMe, pSession->ucInternalSessionId);
         elite_msg_release_msg(&pSession->msg);
      }
      return pPayload->unResponseResult;
   }

   pSvcNode->pSvcHandle = NULL;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Close dec stream success",
         pSession->ucInternalSessionId,  stream_id);

   elite_msg_release_msg(&pSession->msg);

   if ( 0 == pSession->unReqResps )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Graph destroy/close complete",
            pSession->ucInternalSessionId,  stream_id);

      if ( pSession->nOverallResult )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: close fail",
               pSession->ucInternalSessionId,  stream_id);
         AudioStreamMgr_SessionAckFail( pMe, pSession, pSession->nOverallResult );
         return ADSP_EFAILED;
      }

      //if Tx matrix or stream router, now we need to disconnect.
      if (pStream->nodes[ASM_NODE_TX_MTX].pSvcHandle || pStream->nodes[ASM_NODE_TX_ST_RTR].pSvcHandle)
      {
         result = AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream, DISCONNECT_MATRIX, ADM_TX_DIR);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: AudioStreamMgr_SessionReturnDeviceHandle fail",pSession->ucInternalSessionId,  stream_id);
         }
      }
      else
      {
         result = AudioStreamMgr_StreamAckCloseStream(pMe, pSession, pStream);
      }

   } /// all modules have been destroyed

   return  result;
}

ADSPResult AudioStream_Mgr_SessionRespQDecConnectStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   return AudioStreamMgr_SessionRespQConnectHandler(pMe, pSession);
}
