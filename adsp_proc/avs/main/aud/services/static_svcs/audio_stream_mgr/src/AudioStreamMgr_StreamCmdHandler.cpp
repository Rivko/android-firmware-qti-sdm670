/**
@file AudStreamMgr_StreamCmdHandler.cpp

@brief This file contains the implementation for session cmdQ Handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header $ //source/qcom/qct/multimedia2/Audio/elite/static_svcs/AudioStreamMgr/main/latest/src/AudioStreamMgr_SessionCmdHandler.cpp#18 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013 rb      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Stream.h"
#include "AudioStreamMgr_StreamCmdHandler.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "AudioStreamMgr_PrivateDefs.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"

#include "adsp_media_fmt.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"

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

/*
This function close a stream(session).

RX Close sequence:
(0) Session enters disconnecting state.
(1) Sending disconnect msg to both DEC & PP at once.

MUST disconnect DEC && PP first to stop sending
buffer back to PP before request matrix to return all buffer
back to PP. There will be deadlock if this sequence
is not followed.

(2) After receiving ACK from both DEC & PP, (Dec&PP in disconnected state)
sending custom-disconnect request to mtmx so that it returns
lingering buffer back to  PP.

(3) After ACK from AFE, send destroy_service all at once to
all dynamic service.  Dynamic service might wait for each
other's buffer to return but eventually will exit assuming
they always destroy data queue first and destroy buffer queue
at the end.
(4) On receiving the destroy_service, do qurt_thread_join
to clean up.  The graph is torn down after all ACKs. Session
enters CLOSED state.



TX Close sequence:
(0)  Session enters disconnecting state.
(1)   Sending disconnect msg to PP to disconnect PP from enc.
and ELITEMSG_CUSTOM_AFECLIENTDISABLE to mtmx so that it stops
sending buffer down.
(2)   After receiving both ACKs, send destroy_service all at once to
all dynamic service.  Dynamic service might wait for each
other's buffer to return but eventually will exit assuming
they always destroy data queue first and destroy buffer queue
at the end.

Have to destroy PP/Encoder so that all buffers from
AFE is released back to AFE before ASM inform ASM (
do we need this with Matrix too???? )

(4)   On receiving the destroy_service, do qurt_thread_join
to clean up.
(5)   after all dynamic service is destroyed, send ELITEMSG_CUSTOM_AFEDISCONNECT_REQ
so that Afe knows it's okay to destroy it's bufferQ.
(6)   stop afe.
The graph is torn down after all ACKs. Session
enters CLOSED state.


 */

ADSPResult  AudioStreamMgr_CloseStreamCmdHandler(AudioStreamMgr_t *pMe, uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   ADSPResult result = ADSP_EOK;

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if (ADSP_FAILED(result = AudioStreamMgr_CloseStreamCmdHelper(pMe, pSession, pStream)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream close failed.",
            pSession->ucInternalSessionId,  stream_id);
      //Ack failure
      AudioStreamMgr_GenerateAck( pPkt, result, 0 , 0 , 0 );
   }
   else
   {
      if (0 == pSession->unReqResps)
      {
         AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, 0 , 0 , 0 );
      }
   }

   return result;
}

ADSPResult  AudioStreamMgr_CloseStreamCmdHelper(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;

   if ((NULL == pStream) ||
         ((ASM_STREAM_RUN != pStream->state) && (ASM_STREAM_PAUSE != pStream->state) && (ASM_STREAM_SUSPEND != pStream->state)) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Invalid request to close an unopen Session %d",
            pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99, pStream?pStream->state:0);

      return ADSP_EUNEXPECTED;
   }

   //if close comes when waiting for EoS ack, we stop waiting.
   pStream->wait_for_eos_v2_ack = FALSE;

   uint32_t numOpenStreams = ASM_GetNumStreamsOpen(pSession);

   //treat closing of gapless specially (as it doesn't align with session == group of streams)
   if ((numOpenStreams == 2) && ASM_IsGaplessSession(pSession))
   {
      result = AudioStreamMgr_CloseSingleGaplessStream(pMe, pSession, pStream);
   }
   else
   {
      result = AudioStreamMgr_CloseRegularStream(pMe, pSession, pStream);
   }

   if(ADSP_FAILED(result))
   {
      return result;
   }

   return result;
}

ADSPResult AudioStreamMgr_SetEncDecParamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   ASM_GraphNodeIndex nodeIndex = ASM_NODE_MAX;

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   asm_stream_cmd_set_encdec_param_t *pPayload;

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot set encdec params on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   ADSPResult status = ADSP_EOK;
   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );
   if (ulOpCode == ASM_STREAM_CMD_SET_ENCDEC_PARAM)
   {
      pPayload = (asm_stream_cmd_set_encdec_param_t *)elite_apr_if_get_payload_ptr(pPkt);
   }
   else
   {
      asm_stream_cmd_set_encdec_param_v2_t *pPayloadv2 = (asm_stream_cmd_set_encdec_param_v2_t *) elite_apr_if_get_payload_ptr(pPkt);
      pPayload = (asm_stream_cmd_set_encdec_param_t*) &pPayloadv2->param_id; //NOTE: assuming that the V2 struct is similar after the param_id.

      if ( (pPayloadv2->service_id != ASM_ENCODER_SVC) && (pPayloadv2->service_id != ASM_DECODER_SVC) )
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid svc id %lu ",
               pSession->ucInternalSessionId,  stream_id, pPayloadv2->service_id );
         status = ADSP_EBADPARAM; goto __bailout;
      }

      nodeIndex = (pPayloadv2->service_id == ASM_ENCODER_SVC) ? ASM_NODE_ENC :
            (pPayloadv2->service_id == ASM_DECODER_SVC ? ASM_NODE_DEC : ASM_NODE_MAX);

   }

   if (!pPayload)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to get APR payload! ASM cannot handle the command.",
            pSession->ucInternalSessionId,  stream_id );
      status = ADSP_EHANDLE; goto __bailout;
   }

   //check the payload
   switch (pPayload->param_id)
   {
   case ASM_PARAM_ID_ENCDEC_BITRATE:
   {
      if ( (nodeIndex != ASM_NODE_ENC) && (nodeIndex != ASM_NODE_MAX) ) //max for v1
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid node idx %lu for bit rate ",
               pSession->ucInternalSessionId,  stream_id, nodeIndex );
         status = ADSP_EBADPARAM; goto __bailout;
      }

      if(ADSP_FAILED(status = AudioStreamMgr_SendEncDecParam(pMe, pSession, pStream, ASM_NODE_ENC, pPayload)))
      {
         goto __bailout;
      }

      break;
   }

   case ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2:
   {
      if ( (nodeIndex != ASM_NODE_ENC) && (nodeIndex != ASM_NODE_MAX) ) //max for v1
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid node idx %lu for bit rate ",
               pSession->ucInternalSessionId,  stream_id, nodeIndex );
         status = ADSP_EBADPARAM; goto __bailout;
      }

      if(ADSP_FAILED(status = AudioStreamMgr_ProcessEncoderConfigBlock(pMe, pSession, pStream, pPayload)))
      {
         goto __bailout;
      }

      break;
   }

   default:
   {
      if (nodeIndex == ASM_NODE_MAX) //v1
      {
         AudioStreamMgr_GetASMNodeForEncDecParam(pPayload->param_id, &nodeIndex);
      }

      if(nodeIndex != ASM_NODE_DEC && nodeIndex != ASM_NODE_ENC)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Sending the parameter ID 0x%lx to decoder by default",
               pSession->ucInternalSessionId,  stream_id, pPayload->param_id);
         /* We are sending the param to decoder by default in order to support a certain 3rd party decoder set param.
          * We need to come up with a permanent solution for this problem.
          */
         if(ADSP_FAILED(status = AudioStreamMgr_SendEncDecParam(pMe, pSession, pStream, ASM_NODE_DEC, pPayload)))
         {
            goto __bailout;
         }
      }
      else
      {
         if(ADSP_FAILED(status = AudioStreamMgr_SendEncDecParam(pMe, pSession, pStream, nodeIndex, pPayload)))
         {
            goto __bailout;
         }
      }
   }
   }

   if (pSession->unReqResps == 0)
   {
      return AudioStreamMgr_GenerateAck(pPkt, status, 0, 0, 0);
   }
   return ADSP_EOK;

   __bailout:
   return AudioStreamMgr_GenerateAck(pPkt, status, 0, 0, 0);
}

/* This function is to handle PP configuration related commands*/
ADSPResult AudioStreamMgr_SetPpParamCmdHandler(AudioStreamMgr_t* pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   elite_apr_packet_t *pPkt  = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   ASM_GraphNodeIndex node_idx;

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot set PP params on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   node_idx = (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode)?ASM_NODE_DEC:ASM_NODE_PP;

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u, %u]: enter PoppCfgParamsCmdHandler node idx %lu",
         pSession->ucInternalSessionId,  stream_id, node_idx);


   if (!pStream->nodes[node_idx].pSvcHandle)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Set PP param command is not supported as no PP is instantiated.");

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   elite_msg_any_t msg;
   ADSPResult result = ADSP_EOK;
   int32_t *paramBase = NULL;
   uint32_t memMapHandle, cmdHeaderSize, addrLsw, addrMsw, paramPayloadSize;
   uint32_t unParamId;


   if(ulOpCode == ASM_STREAM_CMD_SET_PP_PARAMS_V2)
   {
      asm_stream_cmd_set_pp_params_v2_t *pParamPayload;
      pParamPayload = (asm_stream_cmd_set_pp_params_v2_t *)elite_apr_if_get_payload_ptr(pPkt);
      cmdHeaderSize = sizeof(asm_stream_cmd_set_pp_params_v2_t);
      memMapHandle = pParamPayload->mem_map_handle;
      addrLsw = pParamPayload->data_payload_addr_lsw;
      addrMsw = pParamPayload->data_payload_addr_msw;
      paramPayloadSize = pParamPayload->data_payload_size;
      unParamId = ELITEMSG_PARAM_ID_CAL;
   }
   else //ASM_STREAM_CMD_SET_PP_PARAMS_V3
   {
      asm_stream_cmd_set_pp_params_v3_t *pParamPayload;
      pParamPayload = (asm_stream_cmd_set_pp_params_v3_t *)elite_apr_if_get_payload_ptr(pPkt);
      cmdHeaderSize = sizeof(asm_stream_cmd_set_pp_params_v3_t);
      memMapHandle = pParamPayload->mem_map_handle;
      addrLsw = pParamPayload->data_payload_addr_lsw;
      addrMsw = pParamPayload->data_payload_addr_msw;
      paramPayloadSize = pParamPayload->data_payload_size;
      unParamId = ELITEMSG_PARAM_ID_CAL_V2;
   }


   if (0 == memMapHandle) //in-band
   {
      // Address just beyond the Set params header, points to asm_stream_param_data_v2_t/asm_stream_param_data_v3_t
      paramBase  = (int32_t *)((int8_t*)elite_apr_if_get_payload_ptr(pPkt) + cmdHeaderSize);
   }
   else //out-of-band
   {
      if (!isAlignedToCacheLine(addrLsw))
      {
         result = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Physical memory address is not aligned to cache line, in set param.",
               pSession->ucInternalSessionId,  stream_id);
         return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
      }

      {
         elite_mem_shared_memory_map_t bufferMemNode;
         bufferMemNode.unMemMapClient     = asmMemoryMapClient;
         bufferMemNode.unMemMapHandle     = memMapHandle;
         bufferMemNode.unMemSize          = paramPayloadSize;
         bufferMemNode.unPhysAddrLsw      = addrLsw;
         bufferMemNode.unPhysAddrMsw      = addrMsw;
         bufferMemNode.unVirtAddr         = 0;

         result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
               bufferMemNode.unMemSize, &bufferMemNode);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for SetParam.",
                  pSession->ucInternalSessionId,  stream_id);
            return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
         }

         // Since this buffer will be read, need to invalidate the cache.
         result = elite_mem_invalidate_cache(&bufferMemNode);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to invalidate cache for SetParam.",
                  pSession->ucInternalSessionId,  stream_id);
            return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
         }
         paramBase  = (int32_t*)(bufferMemNode.unVirtAddr);
      }
   }

   elite_svc_handle_t * pProcSvcHandle = (pStream->nodes[node_idx]).pSvcHandle;

   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, node_idx);

   if(ASM_NODE_DEC == node_idx)
   {
      uint32_t unPayloadSize = sizeof( EliteMsg_CustomDecSetGetPpParamType );
      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Failed to create message",
               pSession->ucInternalSessionId,  stream_id);
         return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
      }

      EliteMsg_CustomDecSetGetPpParamType *pMsgPayload = (EliteMsg_CustomDecSetGetPpParamType*) msg.pPayload;
      pMsgPayload->unSecOpCode        =  ELITEMSG_CUSTOM_DEC_SET_PP_PARAM;
      pMsgPayload->unParamId  = unParamId;
      pMsgPayload->pnParamData   = paramBase;
      pMsgPayload->unSize        = paramPayloadSize;

     MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Send SetParam to DecSvc[0x%lx,0x%lx]",
            pSession->ucInternalSessionId,  stream_id,
            pProcSvcHandle->unSvcId,        (uint32_t) (pProcSvcHandle->threadId));
   }
   else
   {
      uint32_t unPayloadSize = sizeof( elite_msg_param_cal_t );
      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CMD_SET_PARAM,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Failed to create message",
               pSession->ucInternalSessionId,  stream_id);
         return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
      }

      elite_msg_param_cal_t *pMsgPayload = (elite_msg_param_cal_t*) msg.pPayload;
      pMsgPayload->unParamId  = unParamId;
      pMsgPayload->pnParamData   = paramBase;
      pMsgPayload->unSize        = paramPayloadSize;

      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Send SetParam to PPSvc[0x%lx,0x%lx]",
            pSession->ucInternalSessionId,  stream_id,
            pProcSvcHandle->unSvcId,        (uint32_t) (pProcSvcHandle->threadId));

   }
   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      //return message to buffer queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send POPP Configuration parameters !!\n",
            pSession->ucInternalSessionId,  stream_id);
      return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: leave PoppCfgParamsCmdHandler",
         pSession->ucInternalSessionId,  stream_id);

   return result;
}

/*
Based on ISOD, on both TX/RX,  session has to be paused before flush. .

Flush always go from upstream to downstream.
 */
ADSPResult  AudioStreamMgr_StreamFlushCmdHandler(AudioStreamMgr_t *pMe, uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot flush a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, 0 , 0 , 0);
   }

   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );
   asm_stream_cmd_flush_v2_t *pAprPayload = NULL;
   uint32_t flush_bit_stream = 0;
   bool_t b_flush_bit_stream = TRUE;

   if (ulOpCode == ASM_STREAM_CMD_FLUSH_V2)
   {
      pAprPayload = (asm_stream_cmd_flush_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt));
      flush_bit_stream = asm_get_stream_bitstream_flush_mode_flag_in_flush_v2_cmd(pAprPayload->mode_flags);
      if (ASM_STREAM_NOT_TO_FLUSH_BITSTREAM == flush_bit_stream)
      {
         b_flush_bit_stream = FALSE;
      }
   }

   if (ADSP_FAILED(result = AudioStreamMgr_StreamFlushCmdHelper(pMe, pSession, pStream, b_flush_bit_stream)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: stream flush failed.",
            pSession->ucInternalSessionId,  stream_id);
      //Ack failure
      AudioStreamMgr_GenerateAck( pPkt, result, 0 , 0 , 0 );
   }
   else
   {
      if (0 == pSession->unReqResps)
      {
         AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, 0 , 0 , 0 );
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: leave Flush command handler",
         pSession->ucInternalSessionId,  stream_id);
   return result;

}

/* send commands to different nodes in parallel */
ADSPResult AudioStreamMgr_StreamFlushCmdHelper(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream, bool_t flush_bit_stream)
{
   ADSPResult result = ADSP_EOK;
   uint32_t index, max_size, node_index;

   /* ERROR checking for not in puase state */
   if ( !pStream || (pSession->unState != AUDIO_STREAM_MGR_SESSION_PAUSE) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u]: Bad State, State=%d. stream pointer%p",
            pSession->ucInternalSessionId, pSession->unState, (void *)pStream);
      result = ADSP_ENOTREADY;
      return result;
   }
   uint8_t stream_id = pStream->ucStreamId;

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: enter Flush Stream command handler",
         pSession->ucInternalSessionId, stream_id);

   //if flush comes when waiting for EoS ack, we stop waiting.
   pStream->wait_for_eos_v2_ack = FALSE;

   //start with the first node in the flush topology
   index = 0;
   max_size = sizeof(pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO].svc_nodes)/sizeof(pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO].svc_nodes[0]);

   while(true)
   {
      index = ASM_FindNextValidNodeIndex(pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO].svc_nodes, index, max_size);

      if (index == max_size)
      {
         break;
      }

      node_index = pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO].svc_nodes[index];
      if (!flush_bit_stream && (node_index == ASM_NODE_DEC))
      {
         index += 1; //skip decoder if bit stream must not be flushed.
         continue;
      }

      //call the flush handler for this node
      result = ASM_ExecFlushFTableEntry(pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO].svc_nodes[index], pMe, pSession, pStream);

      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send flush command!!\n",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      index += 1;
   }

   return result;
}

ADSPResult  AudioStreamMgr_FlushReadBufsCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   if(!pPkt)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null APR packet in Flush Readbufs cmd handler!");
      return ADSP_EFAILED;
   }

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot Flush readbuf on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, 0 , 0 , 0);
   }


   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Entered Flush Readbufs cmd handler",
         pSession->ucInternalSessionId, stream_id);

   //no need to ensure that session is paused.
   //just check whether this is Tx or NT session
   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         ((ASM_TX_MODE != pSession->unMode) && (ASM_NT_MODE != pSession->unMode)))
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: \
         No opened stream in the requested session or not a tunneled Tx or NT session",
         pSession->ucInternalSessionId,  stream_id );

      // ACK with an error
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, NULL, 0, 0 );
   }

   //send an internal message to encoder service
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_ENC);

   uint32_t unPayloadSize = sizeof(EliteMsg_CustomFlushReadbufsType);

   elite_msg_any_t msg;
   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Failed to create Elite message",
            pSession->ucInternalSessionId,  stream_id);

      //ack with error
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_ENOMEMORY, NULL, 0, 0 );
   }

   EliteMsg_CustomFlushReadbufsType *flushReadbufsCmd =
         (EliteMsg_CustomFlushReadbufsType *)msg.pPayload;

   flushReadbufsCmd->unSecOpCode = ELITEMSG_CUSTOM_FLUSH_READBUFS;

   elite_svc_handle_t *pEncSvcHandle = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle;
   result = ASM_PushCmdToPeerSvc(pMe, pSession, pEncSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Failed to send Elite message",
            pSession->ucInternalSessionId,  stream_id);

      //destroy the message and ack with error
      (void) elite_msg_release_msg(&msg);
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, NULL, 0, 0 );
   }

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_GetTopoModuleCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, 0 , 0 , 0);
      return ADSP_EFAILED;
   }

   void *pAprPayload = NULL;
   //msg from ASM to PP
   elite_msg_any_t msg ;
   elite_apr_packet_t *pAckAprPkt = NULL;
   EliteMsg_CustomCfg_GetPPTopologyModuleInfo *pMsgPayload = NULL;
   elite_svc_handle_t *pProcSvcHandle = NULL;
   uint32_t ulClientToken = 0;
   uint32_t unPayloadSize = sizeof(elite_msg_custom_header_t);
   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,
         "AudioStreamMgr [%u,%u]: enter ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST/V2 command handler",
         pSession->ucInternalSessionId,
         stream_id);

   //Get APR payload
   pAprPayload = (void *) ( elite_apr_if_get_payload_ptr(pPkt));

   // Sanity check for states: can get pp for all states except close.
   if ( !pStream || (pSession->unState == AUDIO_STREAM_MGR_SESSION_CLOSE) ||
         (ASM_STREAM_CLOSE == pStream->state) || (ASM_STREAM_GAPLESS_CLOSE == pStream->state) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Bad State, State=%d. stream state %u",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, pStream->state);
      result = ADSP_ENOTREADY;
      goto __bailout;
   }

   uint32_t paramMaxSize, respPayloadSize, calVersion, memMapHandle, addrLsw, addrMsw, respOpCode;
   if(ulOpCode == ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST)
   {
      asm_stream_cmd_get_pp_topo_module_list_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_topo_module_list_t *) pAprPayload;
      paramMaxSize = (uint32_t)pAprPayloadHeader->param_max_size;
      respOpCode = ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST;
      respPayloadSize = sizeof(asm_stream_cmdrsp_get_pp_topo_module_list_t);
      calVersion = ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES;
      memMapHandle = pAprPayloadHeader->mem_map_handle;
      addrLsw = pAprPayloadHeader->data_payload_addr_lsw;
      addrMsw = pAprPayloadHeader->data_payload_addr_msw;
   }
   else //ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
   {
      asm_stream_cmd_get_pp_topo_module_list_v2_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_topo_module_list_v2_t *) pAprPayload;
      paramMaxSize = pAprPayloadHeader->param_max_size;
      respOpCode = ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2;
      respPayloadSize = sizeof(asm_stream_cmdrsp_get_pp_topo_module_list_v2_t);
      calVersion = ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES_V2;
      memMapHandle = pAprPayloadHeader->mem_map_handle;
      addrLsw = pAprPayloadHeader->data_payload_addr_lsw;
      addrMsw = pAprPayloadHeader->data_payload_addr_msw;
   }

   // Param Max Size can not be zero
   if ( !paramMaxSize )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: invalid max size %d",
            pSession->ucInternalSessionId,  stream_id, paramMaxSize);
      result = ADSP_EBADPARAM;
      goto __bailout;
   }

   /// Allocate message: common to both in-band and out-of-band.
   pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;

   if(!pProcSvcHandle)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Attempting to get PP Params for a session that does not contain PP");

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);

   if (ADSP_FAILED(result = elite_msg_create_msg(&msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create get topology modules custom mesage",
            pSession->ucInternalSessionId,  stream_id);
      goto __bailout;
   }

   pMsgPayload = (EliteMsg_CustomCfg_GetPPTopologyModuleInfo*) msg.pPayload;
   pMsgPayload->unSecOpCode = calVersion;

   // Now the message content depends on whether in-band or out-of-band.
   // For in-band, static service needs to allocate large enough APR packet
   if ( 0 == memMapHandle ) //in-band
   {
      uint32_t ackPyloadSize = respPayloadSize + paramMaxSize;

      // Allocate an APR packet
      result = elite_apr_if_alloc_cmd_rsp(AudioStreamMgr_GetAprHandle(),
            elite_apr_if_get_dst_addr( pPkt),
            elite_apr_if_get_dst_port( pPkt),
            elite_apr_if_get_src_addr( pPkt),
            elite_apr_if_get_src_port( pPkt),
            elite_apr_if_get_client_token( pPkt),
            respOpCode,
            ackPyloadSize,
            &pAckAprPkt );

      //if alloc fails or if pkt is still NULL, error out.
      if (NULL == pAckAprPkt)
      {
         result = ADSP_EFAILED;
      }
      if ( (ADSP_FAILED( result )))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to allocate apr packet [0x%lx, S%lu]",
               pSession->ucInternalSessionId,  stream_id,
               (uint32_t) result, ackPyloadSize );
         goto __bailout1;
      }

      int32_t* pCmdRspAprPktBase = (int32_t*)elite_apr_if_get_payload_ptr(pAckAprPkt);
      pMsgPayload->pnParamData = (int32_t*)((int8_t*)pCmdRspAprPktBase + respPayloadSize); //Offset by the status.

      // store APR packet: thus in-band is identified by this packet which as opcode=ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2
      pStSession->cmdMsg.pPayload = pAckAprPkt;
   }
   else
   {
      if (!isAlignedToCacheLine(addrLsw))
      {
         result = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Physical memory address is not aligned to cache line, in get topo modules.",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout1;
      }

      {
         elite_mem_shared_memory_map_t    bufferMemNode;
         bufferMemNode.unMemMapClient     = asmMemoryMapClient;
         bufferMemNode.unMemMapHandle     = memMapHandle;
         bufferMemNode.unMemSize          = paramMaxSize;
         bufferMemNode.unPhysAddrLsw      = addrLsw;
         bufferMemNode.unPhysAddrMsw      = addrMsw;
         bufferMemNode.unVirtAddr         = 0;

         result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
               bufferMemNode.unMemSize, &bufferMemNode);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]:Failed to map physical memory for Get PP topo modules.",
                  pSession->ucInternalSessionId,  stream_id);
            goto __bailout1;
         }
         //no need to invalidate cache since the buffer is only being written.
         pMsgPayload->pnParamData  = (int32_t*)(bufferMemNode.unVirtAddr);
      }
   }

   pMsgPayload->unSize        = paramMaxSize;
   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: FAILED to send GetPPModulesInfo to PP 0x%lx",
            pSession->ucInternalSessionId,  stream_id, (uint32_t) result );
      goto __bailout1;

   }

   if ( 0 == memMapHandle ) //in-band
   {
      // free up the incoming apr packet since we create another one for ack already.
      elite_apr_if_free( AudioStreamMgr_GetAprHandle(),  pPkt);
   }

   return result;

   __bailout1:
   elite_msg_release_msg( &msg );
   __bailout:
   if (pAckAprPkt)   {elite_apr_if_free(AudioStreamMgr_GetAprHandle(),  pAckAprPkt); pAckAprPkt = NULL;}
   //Update the result in response.
   if(ulOpCode == ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST)
   {
      asm_stream_cmdrsp_get_pp_topo_module_list_t response;
      response.status = (uint32_t )result;
      result =  AudioStreamMgr_GenerateAck( pPkt, result, (void*)&response,
            sizeof(response), ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST);
   }
   else //ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2
   {
      asm_stream_cmdrsp_get_pp_topo_module_list_v2_t response;
      response.status = (uint32_t )result;
      result =  AudioStreamMgr_GenerateAck( pPkt, result, (void*)&response,
            sizeof(response), ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2);
   }
   return result;
}

ADSPResult AudioStreamMgr_GetPpParamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   int32_t *pnParamData = NULL;

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   ASM_GraphNodeIndex node_idx;

   if(NULL == pStream)
   {
      AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, 0 , 0 , 0);
      return ADSP_EFAILED;
   }

   node_idx = (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode)?ASM_NODE_DEC:ASM_NODE_PP;

   void *pAprPayload = NULL;
   //msg from ASM to PP
   elite_msg_any_t msg ;
   elite_apr_packet_t *pAckAprPkt = NULL;
   elite_svc_handle_t *pProcSvcHandle = NULL;
   uint32_t ulClientToken = 0;
   uint32_t unPayloadSize;
   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );

   MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: "
                                      "enter ASM_STREAM_CMD_GET_PP_PARAMS_V2/V3 command handler with node idx %lu",
         pSession->ucInternalSessionId,
                                      stream_id, node_idx);

   //Get APR payload
   pAprPayload = (void *) ( elite_apr_if_get_payload_ptr(pPkt));

   // Sanity check for states: can get pp for all states except close.
   if ( !pStream || (pSession->unState == AUDIO_STREAM_MGR_SESSION_CLOSE) ||
         (ASM_STREAM_CLOSE == pStream->state) || (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Bad State, State=%d. stream state %u",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, pStream->state);
      result = ADSP_ENOTREADY;
      goto __bailout;
   }

   uint32_t reqPayloadSize, paramMaxSize, respPayloadSize, calVersion, memMapHandle, addrLsw, addrMsw, respOpCode;
   if(ulOpCode == ASM_STREAM_CMD_GET_PP_PARAMS_V2)
   {
      asm_stream_cmd_get_pp_params_v2_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_params_v2_t *) pAprPayload;
      reqPayloadSize = sizeof(asm_stream_param_data_v2_t);
      paramMaxSize = (uint32_t)pAprPayloadHeader->param_max_size;
      respOpCode = ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2;
      respPayloadSize = sizeof(asm_stream_cmdrsp_get_pp_params_v2_t);
      calVersion = ELITEMSG_PARAM_ID_CAL;
      memMapHandle = pAprPayloadHeader->mem_map_handle;
      addrLsw = pAprPayloadHeader->data_payload_addr_lsw;
      addrMsw = pAprPayloadHeader->data_payload_addr_msw;
   }
   else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
   {
      asm_stream_cmd_get_pp_params_v3_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_params_v3_t *) pAprPayload;
      reqPayloadSize = sizeof(asm_stream_param_data_v3_t);
      paramMaxSize = pAprPayloadHeader->param_max_size;
      respOpCode = ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3;
      respPayloadSize = sizeof(asm_stream_cmdrsp_get_pp_params_v3_t);
      calVersion = ELITEMSG_PARAM_ID_CAL_V2;
      memMapHandle = pAprPayloadHeader->mem_map_handle;
      addrLsw = pAprPayloadHeader->data_payload_addr_lsw;
      addrMsw = pAprPayloadHeader->data_payload_addr_msw;
   }

   // Param Max Size can not be zero
   if ( paramMaxSize < reqPayloadSize )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: invalid max size %d",
            pSession->ucInternalSessionId,  stream_id, paramMaxSize);
      result = ADSP_EBADPARAM;
      goto __bailout;
   }

   /// Allocate message: common to both in-band and out-of-band.
   pProcSvcHandle = (pStream->nodes[node_idx]).pSvcHandle;

   if(!pProcSvcHandle)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Attempting to get PP Params for a session that does not contain PP");

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   ulClientToken = ASM_PopulateClientToken(stream_id, node_idx);

   // Now the message content depends on whether in-band or out-of-band.
   // For in-band, static service needs to allocate large enough APR packet
   if ( 0== memMapHandle ) //in-band
   {
      uint32_t ackPyloadSize = respPayloadSize + paramMaxSize;

      // Allocate an APR packet
      result = elite_apr_if_alloc_cmd_rsp( AudioStreamMgr_GetAprHandle(),
            elite_apr_if_get_dst_addr( pPkt),
            elite_apr_if_get_dst_port( pPkt),
            elite_apr_if_get_src_addr( pPkt),
            elite_apr_if_get_src_port( pPkt),
            elite_apr_if_get_client_token( pPkt),
            respOpCode,
            ackPyloadSize,
            &pAckAprPkt );

      //if alloc fails or if pkt is still NULL, error out.
      if (NULL == pAckAprPkt)
      {
         result = ADSP_EFAILED;
      }
      if ( (ADSP_FAILED( result )))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to allocate apr packet [0x%lx, S%lu]",
               pSession->ucInternalSessionId,  stream_id,
               (uint32_t) result, ackPyloadSize );
         goto __bailout;
      }

      int32_t* pCmdRspAprPktBase = (int32_t*)elite_apr_if_get_payload_ptr(pAckAprPkt);
      pnParamData = (int32_t*)((int8_t*)pCmdRspAprPktBase + respPayloadSize); //Offset by the status to access the module ID.

      // store APR packet: thus in-band is identified by this packet which as opcode=ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2
      pStSession->cmdMsg.pPayload = pAckAprPkt;

   }
   else
   {
      if (!isAlignedToCacheLine(addrLsw))
      {
         result = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Physical memory address is not aligned to cache line, in get param.",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }

      {
         elite_mem_shared_memory_map_t    bufferMemNode;
         bufferMemNode.unMemMapClient     = asmMemoryMapClient;
         bufferMemNode.unMemMapHandle     = memMapHandle;
         bufferMemNode.unMemSize          = paramMaxSize;
         bufferMemNode.unPhysAddrLsw      = addrLsw;
         bufferMemNode.unPhysAddrMsw      = addrMsw;
         bufferMemNode.unVirtAddr         = 0;

         result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
               bufferMemNode.unMemSize, &bufferMemNode);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for GetParam.",
                  pSession->ucInternalSessionId,  stream_id);
            goto __bailout;
         }
         //no need to invalidate cache since the buffer is only being written.
         pnParamData  = (int32_t*)(bufferMemNode.unVirtAddr);
      }
   }

   //Fill up the memory with the ModuleID and ParamID so that PP doesn't have to do it
   if(ulOpCode == ASM_STREAM_CMD_GET_PP_PARAMS_V2)
   {
      asm_stream_param_data_v2_t *psPPParamData = (asm_stream_param_data_v2_t *)(pnParamData);
      asm_stream_cmd_get_pp_params_v2_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_params_v2_t *) pAprPayload;
      psPPParamData->module_id   = pAprPayloadHeader->module_id;
      psPPParamData->param_id    = pAprPayloadHeader->param_id;
      psPPParamData->param_size  = (uint32_t)pAprPayloadHeader->param_max_size;
      psPPParamData->reserved    = 0;
   }
   else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
   {
      asm_stream_param_data_v3_t *psPPParamData = (asm_stream_param_data_v3_t *)(pnParamData);
      asm_stream_cmd_get_pp_params_v3_t *pAprPayloadHeader = (asm_stream_cmd_get_pp_params_v3_t *) pAprPayload;
      psPPParamData->module_id   = pAprPayloadHeader->module_id;
      psPPParamData->instance_id = pAprPayloadHeader->instance_id;
      psPPParamData->reserved    = 0;
      psPPParamData->param_id    = pAprPayloadHeader->param_id;
      psPPParamData->param_size  = pAprPayloadHeader->param_max_size;
   }

   if(ASM_NODE_DEC == node_idx)
   {
      unPayloadSize = sizeof(EliteMsg_CustomDecSetGetPpParamType);
      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Fail to create ELITE_CMD_GET_PARAM mesage",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }

      EliteMsg_CustomDecSetGetPpParamType *pMsgPayload = (EliteMsg_CustomDecSetGetPpParamType*) msg.pPayload;
      pMsgPayload->unSize        = paramMaxSize;
      pMsgPayload->unParamId  = calVersion;
      pMsgPayload->pnParamData = pnParamData;
      pMsgPayload->unSecOpCode        =  ELITEMSG_CUSTOM_DEC_GET_PP_PARAM;
   }
   else
   {
      unPayloadSize = sizeof( elite_msg_param_cal_t );
      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CMD_GET_PARAM,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Fail to create ELITE_CMD_GET_PARAM mesage",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }

      elite_msg_param_cal_t *pMsgPayload = (elite_msg_param_cal_t*) msg.pPayload;
      pMsgPayload->unSize        = paramMaxSize;
      pMsgPayload->unParamId  = calVersion;
      pMsgPayload->pnParamData = pnParamData;
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send GetParam to PP 0x%lx",
            pSession->ucInternalSessionId,  stream_id, (uint32_t) result );
      goto __bailout1;

   }

   if ( 0 == memMapHandle ) //in-band
   {
      // free up the incoming apr packet since we create another one for ack already.
      elite_apr_if_free( AudioStreamMgr_GetAprHandle(),  pPkt);
   }

   return result;

   __bailout1:
   elite_msg_release_msg( &msg );
   __bailout:
   if (pAckAprPkt)   {elite_apr_if_free( AudioStreamMgr_GetAprHandle(),  pAckAprPkt); pAckAprPkt = NULL; }

   //Update the result in response.
   if(ulOpCode == ASM_STREAM_CMD_GET_PP_PARAMS_V2)
   {
      asm_stream_cmdrsp_get_pp_params_v2_t response;
      response.status = (uint32_t )result;
      result =  AudioStreamMgr_GenerateAck( pPkt, result, (void*)&response,
            sizeof(asm_stream_cmdrsp_get_pp_params_v2_t), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2);
   }
   else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
   {
      asm_stream_cmdrsp_get_pp_params_v3_t response;
      response.status = (uint32_t )result;
      result =  AudioStreamMgr_GenerateAck( pPkt, result, (void*)&response,
            sizeof(asm_stream_cmdrsp_get_pp_params_v3_t), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3);
   }
   return result;
}

ADSPResult AudioStreamMgr_SessionDtmfCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot process dtmf ctl cmd on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   asm_stream_cmd_dtmf_ctl_t *pPayload;
   uint32_t ulPayloadSize;
   uint32_t ulClientToken=0;
   EliteMsg_CustomDtmfParamsType *pDtmfParamsMsg=NULL;
   pPayload = (asm_stream_cmd_dtmf_ctl_t *)elite_apr_if_get_payload_ptr(pPkt);

   ADSPResult status = ADSP_EOK;

   if (!pPayload)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to get APR payload! ASM cannot handle the command.",
            pSession->ucInternalSessionId,  stream_id );
      status = ADSP_EHANDLE; goto __bailout;
   }

   if (ASM_TX_MODE == pSession->unMode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [Session ID = %u, Stream ID = %u]:\
                                            DTMF Cannot set for record stream",
                                            pSession->ucInternalSessionId,  stream_id);
      status = ADSP_EUNSUPPORTED; goto __bailout;
   }

   ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);

   ulPayloadSize = sizeof(EliteMsg_CustomDtmfParamsType);

   elite_msg_any_t decMsg;
   if ( ADSP_FAILED( status = elite_msg_create_msg( &decMsg, &ulPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create DTMF msg!",
            pSession->ucInternalSessionId,  stream_id );
      status = ADSP_EHANDLE; goto __bailout;

   }

   //fill up the DTMF param command
   pDtmfParamsMsg = (EliteMsg_CustomDtmfParamsType *)decMsg.pPayload;

   pDtmfParamsMsg->unSecOpCode        =  ELITEMSG_CUSTOM_DTMF_CTL;
   pDtmfParamsMsg->tone_1      =  pPayload->tone_1;
   pDtmfParamsMsg->tone_2       =   pPayload->tone_2;
   pDtmfParamsMsg->gain           =    pPayload->gain;
   pDtmfParamsMsg->duration = pPayload->duration;

   //Extract client info from apr packet to send the tone ended event after played out tone
   elite_msg_extract_client_info( (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload),
         &(pDtmfParamsMsg->dtmfAprInfo) );

   //get dec svc command queue and push the msg into it
   elite_svc_handle_t *pDecSvcHandle;
   pDecSvcHandle = (pStream->nodes[ASM_NODE_DEC]).pSvcHandle;

   status = ASM_PushCmdToPeerSvc(pMe, pSession, pDecSvcHandle->cmdQ, (uint64_t*)&decMsg);

   if (ADSP_FAILED(status))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send DTMF params.",
            pSession->ucInternalSessionId,  stream_id );
      status = ADSP_EHANDLE;
      //destroy the message and ack with error
      (void) elite_msg_release_msg(&decMsg);
      goto __bailout;
   }

   return ADSP_EOK;

   __bailout:
   AudioStreamMgr_GenerateAck(pPkt, status, 0, 0, 0);
   //handled the command successuflly so return OK
   return ADSP_EOK;

}

ADSPResult AudioStreamMgr_RegisterDeRegisterPPEvents(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) ((pStSession->cmdMsg).pPayload);
   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]; enter AudioStreamMgr_StreamCmdRegisterEvents cmd handler",
         pSession->ucInternalSessionId, stream_id);

   if(!pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state ) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state ))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u,Stream ID = %u ]: cannot register events on a closed stream ",
            pSession->ucInternalSessionId, stream_id);

      // Ack that this command is not expected in such state and release packet
      return 	  AudioStreamMgr_GenerateAck( pPkt , ADSP_EUNEXPECTED, 0 , 0 ,0);
   }

   if(!pStream->nodes[ASM_NODE_PP].pSvcHandle)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM Register events command is not supported as PP is not instantiated");

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 ,0);
   }

   ADSPResult result = AudioStreamMgr_SendRegisterEventCmdToDynaSvc(pMe, pStSession, pSession, pStream, ASM_NODE_PP, pPkt);

   if (pSession->unReqResps == 0)
   {
      return AudioStreamMgr_GenerateAck(pPkt, result, 0, 0, 0);
   }

   return result;

}

ADSPResult AudioStreamMgr_RegisterEncDecEvents(AudioStreamMgr_t* pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   ASM_GraphNodeIndex nodeIndex = ASM_NODE_MAX;

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   asm_stream_cmd_register_encdec_events_t *pPayload = (asm_stream_cmd_register_encdec_events_t *)elite_apr_if_get_payload_ptr(pPkt);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: RegisterEncDecEvents: Cannot register for encdec params on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   ADSPResult status = ADSP_EOK;

   if (pPayload->version != 0)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: RegisterEncDecEvents: Invalid version %lu ",
            pSession->ucInternalSessionId,  stream_id, pPayload->version );
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EBADPARAM, 0 , 0 , 0);
   }

   if ( (pPayload->service_id != ASM_DECODER_SVC) ) //currently only decoder svc is supported.
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: RegisterEncDecEvents: Invalid svc id %lu ",
            pSession->ucInternalSessionId,  stream_id, pPayload->service_id );
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EBADPARAM, 0 , 0 , 0);
   }

   nodeIndex = (pPayload->service_id == ASM_ENCODER_SVC) ? ASM_NODE_ENC :
         (pPayload->service_id == ASM_DECODER_SVC ? ASM_NODE_DEC : ASM_NODE_MAX);


   if (( ASM_NODE_MAX == nodeIndex) || (NULL == pStream->nodes[nodeIndex].pSvcHandle))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: RegisterEncDecEvents: invalid service handle",
            pSession->ucInternalSessionId,  stream_id);
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, 0 , 0 , 0);
   }

   status  = AudioStreamMgr_SendRegisterEventCmdToDynaSvc(pMe, pStSession, pSession, pStream, nodeIndex, pPkt);

   if (pSession->unReqResps == 0)
   {
      return AudioStreamMgr_GenerateAck(pPkt, status, 0, 0, 0);
   }
   return status;
}

ADSPResult AudioStreamMgr_RegisterIEC61937MediaFmtEvents(AudioStreamMgr_t* pMe,
      uint8_t int_session_id, uint8_t stream_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   ASM_GraphNodeIndex nodeIndex = ASM_NODE_MAX;

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) ||
         (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: RegisterEncDecEvents: Cannot register for encdec params on a closed stream!",
            pSession->ucInternalSessionId,  stream_id);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   ADSPResult status = ADSP_EOK;

   nodeIndex = ASM_NODE_DEC;

   status  = AudioStreamMgr_SendIEC61937RegisterEventCmdToDynaSvc(pMe, pStSession, pSession, pStream, nodeIndex, pPkt);

   if (pSession->unReqResps == 0)
   {
      return AudioStreamMgr_GenerateAck(pPkt, status, 0, 0, 0);
   }
   return status;
}
