/**
@file AudStreamMgr_Stream.cpp

@brief This file contains the implementation for stream-control-related functions
for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Stream.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Stream.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"

#include "MixerSvc_Interface.h"
#include "adsp_media_fmt.h"

#include "AudioStreamMgr_adsppm.h"

#include "qurt_elite_globalstate.h"

extern uint32_t asmMemoryMapClient;

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/**
 * return of max_size is an error
 * starts from current index.
 */
uint16_t ASM_FindNextValidNodeIndex(const ASM_GraphNodeIndex *NodeIndList, uint16_t index, uint16_t max_size)
{
   for ( uint16_t i = index; i < max_size; i++)
   {
      if (ASM_INVALID_NODE != NodeIndList[i]) return i;
   }
   return max_size;
}

ADSPResult AudioStreamMgr_CreateStreamState(AudioStreamMgr_SessionStateType *pSession, uint8_t index)
{
   AudioStreamMgr_StreamStateType *pStream = &pSession->aStreams[index];
   pStream->state = ASM_STREAM_CLOSE;

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_CreateAllStreamStates(AudioStreamMgr_SessionStateType *pSession)
{
   uint8_t i;

   for (i = 0; i < ASM_MAX_SUPPORTED_STREAM_ID + 1; i++)
   {
      pSession->ucStreamId2Index[i] = ASM_INVALID_INDEX;
   }

   for (i = 0; i< ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i ++ )
   {
      if ( ADSP_FAILED( AudioStreamMgr_CreateStreamState( pSession, i) ))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create AudioStreamMgr stream %d!", i);
         return ADSP_EFAILED;
      }
   }

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_InitializeStreamState(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id)
{
   ADSPResult result = ADSP_EOK;

   AudioStreamMgr_StreamStateType *pStream = ASM_AllotStream(pSession, stream_id);

   if (NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: allotting stream failed",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_ENORESOURCE;
   }

   //only is_first_gapless_stream==true stream can be in gapless close state
   //by not initializing we carry forward the state.

   if (pStream->state == ASM_STREAM_GAPLESS_CLOSE)
   {
      return ADSP_EOK;
   }

   if (pStream->state != ASM_STREAM_CLOSE)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]:trying to open an already opened stream",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_EALREADY;
   }

   memset(&pStream->nodes, 0, sizeof(pStream->nodes));

   for (uint16_t i = 0; i < ASM_NODE_MAX; i++)
   {
      pStream->nodes[i].index = (ASM_GraphNodeIndex)i;
   }

   memset(&pStream->mtMxPortParam, 0, sizeof(pStream->mtMxPortParam));
   memset(&pStream->PPCallback, 0, sizeof(pStream->PPCallback));
   memset(&pStream->AsmCbData, 0, sizeof(pStream->AsmCbData));
   memset(&pStream->src_fmt_id, 0, sizeof(pStream->src_fmt_id));
   memset(&pStream->sink_fmt_id, 0, sizeof(pStream->sink_fmt_id));
   memset(&pStream->pathFTRTBufDelayUs, 0, sizeof(pStream->pathFTRTBufDelayUs));
   memset(&pStream->pathRTBufDelayUs, 0, sizeof(pStream->pathRTBufDelayUs));
   memset(&pStream->pathAlgDelayUs, 0, sizeof(pStream->pathAlgDelayUs));
   pStream->generic_rx_dummy_handle = NULL;

   pStream->ucStreamId = stream_id; //this will be retained for a gapless stream even after gapless-closing.
   pStream->stream_perf_mode = ASM_LEGACY_STREAM_SESSION;
   memset(pStream->mem_map_handle, 0, sizeof(pStream->mem_map_handle));

   return result;
}

/**
 * the create function is supposed to check whether the svc is already created.
 * If so, it may not create again, but instead send a message to the already created svc and get the handle
 */
ADSPResult AudioStreamMgr_CreateAllStreamSvcs(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_OpenStreamParams_t *pOpenStream)
{
   ADSPResult result = ADSP_EOK;
   uint32_t node_index;
   //create dyna services
   for (uint16_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      node_index = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      if ( (ASM_INVALID_NODE != node_index) &&
            (NULL != ASM_CreateDynaSvcFTable[node_index].func_ptr) )
      {
         result = ASM_CreateDynaSvcFTable[node_index].func_ptr(pMe,pSession,pStream,pOpenStream);
         if (ADSP_FAILED(result))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "AudioStreamMgr [%u,%u]: Failed to create a dynamic service. Index %u", pSession->ucInternalSessionId, pStream->ucStreamId,i );
            return result;
         }
      }
   }
   return result;
}
/* =======================================================================
 **                          Stream Utility functions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_StreamAckOpenStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK, reqResult;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   uint8_t stream_id = pStream->ucStreamId;

   if (ADSP_SUCCEEDED(pSession->nOverallResult))
   {
      if (ADSP_FAILED(reqResult = AudioStreamMgr_RegisterQs(pMe, pSession, pStream)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Registering queue failed",pSession->ucInternalSessionId,stream_id);
         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EFAILED, NULL, 0, 0 );
         return result;
      }

      if(ASM_STREAM_CONNECTING == pStream->state)
      {
         pStream->state = ASM_STREAM_PAUSE;
      }

      //if this is a new session then move it to pause. Otherwise let it be
      //in the same state as before (PAUSE or RUN, not suspend)
      if (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)
      {
         pSession->unState = AUDIO_STREAM_MGR_SESSION_PAUSE;
      }

      //if the session is already running while this stream is opened put it to run state.
      if (AUDIO_STREAM_MGR_SESSION_RUN == pSession->unState)
      {
         /** for gapless a new stream can be opened when whole session is running, put any newly opened streams also in run. */
         if (ASM_IsGaplessSession(pSession) )
         {
            //note: if decoder needs run command, then we need to run it & below line is not sufficient.
            //sending command to stream is difficult because run topo sends command to all svcs: PP, Mtmx which are already in run.
            pStream->state = ASM_STREAM_RUN;
         }
         else
         {
            //for TX push mode, AudioStreamMgr_ProcessPcmEncCfgForPpMtx is needed to be called. Anyway currently only one stream is needed in TX push mode.
            //therefore, for simplicity do not support opening streams in run mode.
            if (ASM_TX_PUSH_MODE == pSession->unMode)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: cannot open a new stream when session is RUN state", pSession->ucInternalSessionId,  stream_id);
               result = ADSP_EFAILED;
            }
            else
            {
               //send run command to the stream

               result = ASM_RunNewlyOpenedStream(pMe, pSession, pStream);
            }
         }
      }
   }
   pSession->nOverallResult|=result;

   if (0 == pSession->unReqResps)
   {
      if (ADSP_SUCCEEDED(pSession->nOverallResult))
      {
         //update the oldest open stream id as this stream id if this is the first stream to be opened.
         uint32_t numStreams = ASM_GetNumStreamsOpen(pSession);
         if (numStreams == 1)
         {
            pSession->oldestOpenStreamId = pStream->ucStreamId;
         }

         //since encoder config block is sent in open command for push mode read, need to send it to PP and Tx Matrix.
         if (ASM_TX_PUSH_MODE == pSession->unMode)
         {
            asm_stream_cmd_open_push_mode_read_t *pOpenCmd ;
            elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);
            pOpenCmd = (asm_stream_cmd_open_push_mode_read_t *)elite_apr_if_get_payload_ptr(pPkt);
            uint8_t *pOpenCmdBytePtr = (uint8_t*)pOpenCmd;
            pOpenCmdBytePtr += sizeof(asm_stream_cmd_open_push_mode_read_t);

            result = AudioStreamMgr_ProcessPcmEncCfgForPpMtx(pMe, pSession, pStream, (void*)pOpenCmdBytePtr);
            if (pSession->unReqResps > 0)
            {
               // return only if there was an error or we are expecting a response for the enc cfg block
               return result;
            }
         }

         if ((ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE == pSession->unMode)||(ASM_LOOPBACK_COMPRESSED_IN_PCM_OUT_MODE == pSession->unMode))
         {
            return AudioStreamMgr_StreamSendMtMXMsgForRateMatching(pMe,pSession,pStream);
         }
      }

      result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );

   }

   return result;
}

ADSPResult AudioStreamMgr_StreamAckCloseStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   ADSPResult result = ADSP_EOK, reqResult;

   uint8_t stream_id = pStream->ucStreamId;
   bool_t stream_released = FALSE;

   if (0 != pStream->mem_map_handle[ASM_POS_BUF_INDEX])
   {
      result = ASM_MemUnmapHandler(asmMemoryMapClient, pStream->mem_map_handle[ASM_POS_BUF_INDEX]);
      pStream->mem_map_handle[ASM_POS_BUF_INDEX] = 0;
   }
   if (0 != pStream->mem_map_handle[ASM_CIRC_BUF_INDEX])
   {
      result = ASM_MemUnmapHandler(asmMemoryMapClient, pStream->mem_map_handle[ASM_CIRC_BUF_INDEX]);
      pStream->mem_map_handle[ASM_CIRC_BUF_INDEX] = 0;
   }

   //for Gapless:
   //this function is called only when last gapless stream is closed (among gapless use cases). we need to clear first gapless stream now.
   if (ASM_IsGaplessSession(pSession))
   {
	   stream_released = pSession->pFirstGaplessStrm->ucStreamId == stream_id ? TRUE : FALSE;
	   ASM_ReleaseStream(pSession, pSession->pFirstGaplessStrm->ucStreamId);
	   pSession->pFirstGaplessStrm = NULL;
   }

   /* When client opens first stream as gapless and second stream is not opened, we would have already
   released the stream above, avoiding freeing it agian with below condition check */
   if(!stream_released)
   {
	   ASM_ReleaseStream(pSession,stream_id);
   }

   //once all streams are closed, dealloc session .
   if (!ASM_GetNumStreamsOpen(pSession))
   {
      result = AudioStreamMgr_DeallocateSession(pMe, pSession->ucInternalSessionId);
   }
   else
   {
      //TODO: switch pSession->oldestOpenStreamId to the next one. for gapless it's done before this step.
   }

   // Requesting the ADSP PM to Release the necessary resources since CLOSE is successful
   if (ADSP_FAILED(reqResult = AudioStreamMgr_RequestReleasePm(pMe)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: cannot release resources from PM",
            pSession->ucInternalSessionId,
            stream_id);
      result = ADSP_EFAILED;
   }

   result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );

   return result;
}

ADSPResult  AudioStreamMgr_StreamDestroy(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   uint32_t unPayloadSize;
   uint32_t opcode;
   elite_msg_any_t msg;
   ADSPResult result =  ADSP_EFAILED;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id = pStream->ucStreamId;
   ASM_GraphNode *node;
   ASM_GraphNodeIndex node_ind;

   uint32_t token = 0;
   uint16_t index=0, max_size;
   max_size = sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes)/sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes[0]);

   uint32_t num_streams = ASM_GetNumStreamsOpen(pSession);

   while(true)
   {
      index = ASM_FindNextValidNodeIndex(pStream->pTopo->StrmSvcInfo.svc_nodes, index, max_size);
      if (index == max_size)
      {
         break;
      }

      node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[index];

      node = &pStream->nodes[node_ind];

      if (!node->pSvcHandle)//in case, this function may be called after destroy
      {
         index += 1;
         continue;
      }

      if (ASM_IsGaplessSession(pSession) && (num_streams > 1) && (node != &(pStream->nodes[ASM_NODE_DEC])))
      {
         // if we have 2 streams & gapless then destroy only the decoder svc of this stream, ignore other nodes.
      }
      else
      {
         if ( (node_ind == ASM_NODE_RX_MTX) ||
               (node_ind == ASM_NODE_TX_MTX) ||
               (node_ind == ASM_NODE_RX_ST_RTR) ||
               (node_ind == ASM_NODE_TX_ST_RTR) ||
               (node_ind == ASM_NODE_TX_GENERIC) ||
               (node_ind == ASM_NODE_RX_GENERIC))
         {
            index += 1;
            continue; //do not destroy, since they are not created by ASM.
         }

         token = ASM_PopulateClientToken(stream_id, node_ind);

         {
            unPayloadSize = sizeof( elite_msg_cmd_destroy_svc_t );
            opcode = ELITE_CMD_DESTROY_SERVICE;

            //return ok since we cannot destroy while the module is being used in another stream.
            if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
                  opcode,
                  pStSession->pSessionRespQ,
                  token,
                  0) ) )
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
                     pSession->ucInternalSessionId, stream_id);
               return result;
            }

            if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, node->pSvcHandle->cmdQ, (uint64_t*)&msg )))
            {
               MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send destroy to Svc [0x%lx,0x%lx]",
                     pSession->ucInternalSessionId, stream_id, node->pSvcHandle->unSvcId,
                     (uint32_t) (node->pSvcHandle->threadId));
               //return the created message back to buffer queue
               elite_msg_release_msg(&msg);
               return result;
            }

            MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Sent destroy to Svc [0x%lx]. token [0x%lx]",
                  pSession->ucInternalSessionId, stream_id, node->pSvcHandle->unSvcId, token);
         }
      }

      index += 1;

   } // destroy FROM UPSTREAM TO DOWNSTREAM

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_StreamConnectGraph(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result=ADSP_EOK;

   uint8_t stream_id = pStream->ucStreamId;

   ASM_GraphNode *pDownStrmNode = NULL;
   ASM_GraphNode *pUpStrmNode = NULL;
   ASM_GraphNodeIndex up_strm_node_ind = ASM_INVALID_NODE, down_strm_node_ind;

   uint16_t index=0;
   uint16_t max_size = sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes)/sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes[0]);

   while(true)
   {
      //order in this is upstream -> downstream
      index = ASM_FindNextValidNodeIndex(pStream->pTopo->StrmSvcInfo.svc_nodes, index, max_size);

      if (index == max_size)
      {
         break;
      }

      down_strm_node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[index];

      if ( (down_strm_node_ind == ASM_NODE_TX_MTX) ||
            (down_strm_node_ind == ASM_NODE_TX_ST_RTR) ||
            (down_strm_node_ind == ASM_NODE_TX_GENERIC) ||
            (down_strm_node_ind == ASM_NODE_RX_GENERIC))   // for LLNP, ULL & ULLPP Rx connection happens during map routing so ignore.
      {
         index += 1;
         continue; //do not connect Tx nodes, their connection happens by req handle
      }

      pDownStrmNode = &pStream->nodes[down_strm_node_ind];

      if (!pUpStrmNode)
      {
         pUpStrmNode = pDownStrmNode;
         up_strm_node_ind = down_strm_node_ind;
         index += 1;
         continue; //to handle first time -> we need at least 2 nodes to connect.
      }

      if (NULL == ASM_ConnectDynaSvcFTable[up_strm_node_ind].func_ptr)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: skipping connection to node %u (as per ftable) ",
               pSession->ucInternalSessionId, stream_id, up_strm_node_ind );
         index += 1;
         continue; //do not connect Tx nodes, their connection happens by req handle
      }

      result = ASM_ConnectDynaSvcFTable[up_strm_node_ind].func_ptr(pMe, pSession, pStream, up_strm_node_ind, down_strm_node_ind, FALSE);

      index += 1;
      pUpStrmNode = pDownStrmNode;
      up_strm_node_ind = down_strm_node_ind;
   }

   return result;
}

ADSPResult AudioStreamMgr_StreamDisConnectDecFromSwitchSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   elite_msg_any_t msg;
   ADSPResult result = ADSP_EOK;
   uint8_t stream_id = pStream->ucStreamId;

   pStream->state = ASM_STREAM_DISCONNECTING;

   // Disconnect from upstream to downstream
   ASM_GraphNode *pDownStrmNode = &pStream->nodes[ASM_NODE_SWITCH];
   ASM_GraphNode *pUpStrmNode = &pStream->nodes[ASM_NODE_DEC];

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if (!pUpStrmNode || !pDownStrmNode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: None of upstream and downstream can be NULL",
            pSession->ucInternalSessionId,  stream_id);
      result = ADSP_EFAILED;
      return result;
   }

   uint32_t unPayloadSize = sizeof( elite_msg_cmd_connect_t);
   uint32_t token = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_DEC);

   //for MIMO decoder, this command will close the stream as well.
   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CMD_DISCONNECT,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   elite_svc_handle_t *pUpStrmSvcHandle = pUpStrmNode->pSvcHandle;
   elite_svc_handle_t *pDownStrmSvcHandle = pDownStrmNode->pSvcHandle;

   ((elite_msg_cmd_connect_t *) msg.pPayload)->pSvcHandle = pDownStrmNode->pSvcHandle;

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: disconnecting Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
         pSession->ucInternalSessionId,  stream_id,
         pUpStrmSvcHandle->unSvcId,  (uint32_t) (pUpStrmSvcHandle->threadId),
         pDownStrmSvcHandle->unSvcId, (uint32_t) (pDownStrmSvcHandle->threadId));


   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pUpStrmSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to disconnect Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
            pSession->ucInternalSessionId,  stream_id,
            pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
            pDownStrmSvcHandle->unSvcId,        (uint32_t) (pDownStrmSvcHandle->threadId));
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      return result;
   }

   return result;
}


ADSPResult AudioStreamMgr_StreamDisConnectGraph(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   elite_msg_any_t msg;
   ADSPResult result;
   uint8_t stream_id = pStream->ucStreamId;

   pStream->state = ASM_STREAM_DISCONNECTING;

   // Disconnect from upstream to downstream
   ASM_GraphNode *pDownStrmNode = NULL;
   ASM_GraphNode *pUpStrmNode = NULL;
   ASM_GraphNodeIndex up_strm_node_ind = ASM_NODE_MAX, down_strm_node_ind;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint16_t index=0;
   uint16_t max_size = sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes)/sizeof(pStream->pTopo->StrmSvcInfo.svc_nodes[0]);

   while(true)
   {
      //order in this is upstream -> downstream
      index = ASM_FindNextValidNodeIndex(pStream->pTopo->StrmSvcInfo.svc_nodes, index, max_size);

      if (index == max_size)
      {
         break;
      }

      down_strm_node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[index];

      //if we have tx mtx port, disable it first
      if ( (down_strm_node_ind == ASM_NODE_TX_MTX) ||
           (down_strm_node_ind == ASM_NODE_TX_ST_RTR) ||
           (down_strm_node_ind == ASM_NODE_TX_GENERIC))
      {
         AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream, DISABLE_MATRIX, ADM_TX_DIR);
         index += 1;
         continue;
      }

      pDownStrmNode = &pStream->nodes[down_strm_node_ind];

      if (!pUpStrmNode)
      {
         pUpStrmNode = pDownStrmNode;
         up_strm_node_ind = down_strm_node_ind;
         index += 1;
         continue; //to handle first time -> we need at least 2 nodes to connect.
      }

      uint32_t unPayloadSize = sizeof( elite_msg_cmd_connect_t);
      uint32_t token = ASM_PopulateClientToken(pStream->ucStreamId, up_strm_node_ind);

      //for MIMO decoder, this command will close the stream as well.
      if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CMD_DISCONNECT,
            pStSession->pSessionRespQ,
            token,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      elite_svc_handle_t *pUpStrmSvcHandle = pUpStrmNode->pSvcHandle;
      elite_svc_handle_t *pDownStrmSvcHandle = pDownStrmNode->pSvcHandle;

      if (!pDownStrmSvcHandle)
      {
         //in ULL this handle pDownStrmSvcHandle is AFE handle. if it's null it may be valid case (either AFE didn't connect or is disconnected already).
         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: disconnecting Svc [0x%lx,0x%lx]. But downstream already closed or unconnected. Skipping.",
               pSession->ucInternalSessionId,  stream_id,
               pUpStrmSvcHandle->unSvcId,  (uint32_t) (pUpStrmSvcHandle->threadId));
         //return created msg to buffer return queue
         elite_msg_release_msg(&msg);

         index += 1;
         pUpStrmNode = pDownStrmNode;
         up_strm_node_ind = down_strm_node_ind;

         continue;
      }

      ((elite_msg_cmd_connect_t *) msg.pPayload)->pSvcHandle = pDownStrmNode->pSvcHandle;

      MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: disconnecting Svc [0x%lx,0x%lx] from Svc [0x%lx,0x%lx]!",
            pSession->ucInternalSessionId,  stream_id,
            pUpStrmSvcHandle->unSvcId,  (uint32_t) (pUpStrmSvcHandle->threadId),
            pDownStrmSvcHandle->unSvcId, (uint32_t) (pDownStrmSvcHandle->threadId));


      if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pUpStrmSvcHandle->cmdQ, (uint64_t*)&msg )))
      {
         MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to disconnect Svc [0x%lx,0x%lx] from Svc [0x%lx,0x%lx]!",
               pSession->ucInternalSessionId,  stream_id,
               pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
               pDownStrmSvcHandle->unSvcId,        (uint32_t) (pDownStrmSvcHandle->threadId));
         //return created msg to buffer return queue
         elite_msg_release_msg(&msg);
         return result;
      }

      index += 1;
      pUpStrmNode = pDownStrmNode;
      up_strm_node_ind = down_strm_node_ind;

   } // DISCONNECT FROM UPSTREAM TO DOWNSTREAM

   //for ULL/LLNP :
   //if AFE is already disconnected from Dec svc, then return device handle is called by below function.
   //    1) ADM has send disconnect as part of COPP close
   //    2) ASM session close is called immediately after ASM session open before AFE close
   result = ASM_CloseStreamAfterDisconnect(pMe, pSession, pStream);

   return result;
}

ADSPResult ASM_CloseStreamAfterDisconnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;
   uint32_t numOpenSessions = ASM_GetNumStreamsOpen(pSession);
   uint8_t stream_id = pStream->ucStreamId;

   //return Rx device handle except for gapless with 2 streams & only if we are not expecting any responses.
   //expected resp may be from disconnect & in that disconnect does return handle.
   if ( (0 == pSession->unReqResps) &&
         (pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle || pStream->nodes[ASM_NODE_RX_ST_RTR].pSvcHandle || pStream->generic_rx_dummy_handle) &&
               (!ASM_IsGaplessSession(pSession) || ((numOpenSessions == 1) && (ASM_IsGaplessSession(pSession))) ))
   {
      if (ADSP_FAILED(result = AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream,
            DISCONNECT_MATRIX, ADM_RX_DIR)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: fail to return matrix handle",
               pSession->ucInternalSessionId,  stream_id);
         goto __bailout;
      }
   }

   if (0 == pSession->unReqResps)
   {
      result = AudioStreamMgr_StreamDestroy(pMe, pSession, pStream);

      if((ADSP_EOK != result) && (0 == pSession->unReqResps))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AudioStreamMgr [%u,%u]: Stream Close failed with opcode: %lx",
               pSession->ucInternalSessionId,  stream_id, (uint32_t)result);
         goto __bailout;
      }
   }

   if (0 == pSession->unReqResps)
   {
      //if Tx matrix or stream router, now we need to disconnect.
      if (pStream->nodes[ASM_NODE_TX_MTX].pSvcHandle || pStream->nodes[ASM_NODE_TX_ST_RTR].pSvcHandle)
      {
         result = AudioStreamMgr_SessionReturnDeviceHandle(pMe, pSession, pStream, DISCONNECT_MATRIX, ADM_TX_DIR);
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: fail to return matrix handle",
                  pSession->ucInternalSessionId,  stream_id);
            goto __bailout;
         }
      }
   }

   if ((0 == pSession->unReqResps))
   {
      result = AudioStreamMgr_StreamAckCloseStream(pMe, pSession, pStream);
   }

   return result;

   __bailout:
   AudioStreamMgr_SessionAckFail( pMe, pSession, result );
   return result;
}

ADSPResult AudioStreamMgr_StreamFlushProcSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pProcSvcHandle = NULL ;
   uint32_t result;
   elite_msg_any_t msg;
   uint8_t stream_id   = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( unOpcode != ELITE_CMD_FLUSH ) {
      return ADSP_EBADPARAM;
   }

   pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);

   uint32_t unPayloadSize = sizeof(elite_msg_any_payload_t);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         unOpcode,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);
   if ( ADSP_FAILED( result ) )
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Flushing PP svc 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   return result;
}

ADSPResult AudioStreamMgr_StreamFlushCodecSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pDecEncSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg;
   uint32_t result;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if (  unOpcode != ELITE_CMD_FLUSH )
   {
      return ADSP_EBADPARAM;
   }

   if (NULL == pStream)
   {
      return ADSP_EBADPARAM;
   }
   uint8_t stream_id = pStream->ucStreamId;


   if(ASM_DEC_SVC == unDir)
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_DEC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);
   }
   else
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_ENC);
   }

   // Run/Pause/Flush shall have the same payload right now.
   uint32_t unPayloadSize = sizeof(elite_msg_any_payload_t);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         unOpcode,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);
   if ( ADSP_FAILED( result ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pDecEncSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Flushing Dec/Enc svc 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   return result;
}

/*This function flushes the switch input port associated with the given stream*/
ADSPResult AudioStreamMgr_StreamFlushSwitchSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{
   elite_msg_any_t msg;
   uint32_t result;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if (NULL == pStream)
   {
      return ADSP_EBADPARAM;
   }

   if ( unOpcode != ELITEMSG_CUSTOM_SWITCH_FLUSH)
   {
      return ADSP_EBADPARAM;
   }

   uint8_t stream_id   = pStream->ucStreamId;

   elite_svc_handle_t *pSwitchSvcHandle = (pStream->nodes[ASM_NODE_SWITCH]).pSvcHandle;
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_SWITCH);

   uint32_t unPayloadSize = sizeof(elite_msg_custom_switch_flush);

   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create mesage for flush to Switch",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   //set up payload
   elite_msg_custom_switch_flush *pPayload = (elite_msg_custom_switch_flush *) (msg.pPayload);
   pPayload->unSecOpCode = unOpcode;
   pPayload->pSvcHandle = (pStream->nodes[ASM_NODE_SWITCH]).pSvcHandle;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pSwitchSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send down switch flush command ",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Flushing switch svc 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   return result;
}

/*
This function sends destroy message to a dynamic service and wait for its ACK.
This function is a blocking call and shall ONLY be used in error handling case
 */
ADSPResult  AudioStreamMgr_StreamDestroyMod(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_GraphNodeIndex nodeIndex)
{
   uint32_t unPayloadSize;
   elite_msg_any_t msg;
   ADSPResult result;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   ASM_GraphNode *pSvcNode = NULL;

   if(ASM_NODE_MAX > nodeIndex){
      pSvcNode = &pStream->nodes[nodeIndex];
   } else{
      return ADSP_EOK;
   }

   if((NULL == pSvcNode) || (NULL == pSvcNode->pSvcHandle))
   {
      return ADSP_EOK;
   }

   uint32_t svc_id = pSvcNode->pSvcHandle->unSvcId;

   uint32_t token = (uint32_t)pSvcNode;
   unPayloadSize = sizeof(elite_msg_cmd_destroy_svc_t);

   MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: destroying module %lu due to unexpected error",
         pSession->ucInternalSessionId,  stream_id, svc_id);

   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CMD_DESTROY_SERVICE,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pSvcNode->pSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      //return message to buffer queue
      elite_msg_release_msg(&msg);
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to destroy Svc [0x%lx,0x%lx]",
            pSession->ucInternalSessionId,  stream_id,
            pSvcNode->pSvcHandle->unSvcId,
            (uint32_t) (pSvcNode->pSvcHandle->threadId));
      return result;
   }

   pSession->msg = msg;

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: send destroy to Svc [0x%lx]",
         pSession->ucInternalSessionId,  stream_id, svc_id);

   result = elite_svc_wait_for_ack(&msg);
   if ( ADSP_FAILED( result ) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to destroy Svc [0x%lx]",
            pSession->ucInternalSessionId,  stream_id, svc_id);
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   /// qurt thread join on the saved threadID. At this point, pSvcHandle
   /// is no longer valid.
   qurt_elite_thread_join(pSvcNode->threadId,&result);

   (void) elite_msg_release_msg( &msg);

   memset( pSvcNode, 0, sizeof(ASM_GraphNode));

   MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: module %lu is destroyed due to unexpected error",
         pSession->ucInternalSessionId,  stream_id, svc_id);

   return result;
}


/*
This function runs through all the modules in a given stream and destroys them
 */
ADSPResult  AudioStreamMgr_StreamPanicHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   //if more responses are expected, then set session state to BAD and return.
   //otherwise already expected responses will clash with DESTROY sent below.
   if (pSession->unReqResps > 0)
   {
      pStream->state = ASM_STREAM_BAD_STATE;
      return ADSP_EOK;
   }

   (void) AudioStreamMgr_DestroyStreamSvc(pMe,pSession,pStream);

   ASM_ReleaseStream(pSession, pStream->ucStreamId);

   return ADSP_EOK;
}

/**
 * This function returns the delay caused by the decoder service.
 *
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForDecoderSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir)
{
   uint8_t stream_id   = pStream->ucStreamId;

   //Note: since the get_path_delay is a session command (where streamId should be ignored), this method using stream_id may not be fine.

   AudioStreamMgr_AggregateBufDelay(pStream, unDir, pStream->nodes[ASM_NODE_DEC].buf_delay);
   pStream->pathAlgDelayUs += pStream->nodes[ASM_NODE_DEC].alg_delay;
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay query for decoder added buffering delay %lu, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay %lu, total = %lu us",
         pSession->ucInternalSessionId, stream_id, pStream->nodes[ASM_NODE_DEC].buf_delay, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->nodes[ASM_NODE_DEC].alg_delay, pStream->pathAlgDelayUs);


   return ADSP_EOK;
}

/**
 * This function returns the delay caused by the PP service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamGetPathDelayForPPService (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t unDir)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   //send command to the PP service to get the number of output buffers
   elite_msg_any_t msg;
   uint32_t unPayloadSize = sizeof(EliteMsg_CustomPPGetNumOutputBuffers);
   uint32_t token = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_PP);
   ASM_AddCustomDataToClientToken(&token, unDir);

   if (ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create custom message ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS",
            pSession->ucInternalSessionId,
            pStream->ucStreamId );
      return result;
   }

   EliteMsg_CustomPPGetNumOutputBuffers *pPPGetNumBuffersCmd = (EliteMsg_CustomPPGetNumOutputBuffers *)msg.pPayload;
   pPPGetNumBuffersCmd->unSecOpCode = ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS;
   pPPGetNumBuffersCmd->unNumBuffers = 0;

   //push message into PP cmd Q
   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pStream->nodes[ASM_NODE_PP].pSvcHandle->cmdQ, (uint64_t*)&msg)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS",
            pSession->ucInternalSessionId,
            pStream->ucStreamId);
      elite_msg_release_msg(&msg);
      return result;
   }

   return ADSP_EOK;
}

/**
 * This function returns the algorithmic delay caused by the PP service. The buffering delay is zero.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamGetPathDelayForPPServiceAlgOnly (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t unDir)
{
   uint8_t stream_id   = pStream->ucStreamId;

   AudioStreamMgr_AggregateBufDelay(pStream, unDir, 0);

   //get the algorithm delay + buffering delay from PP.
   pStream->pathAlgDelayUs += pStream->nodes[ASM_NODE_PP].alg_delay;

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay query for PP added buffering delay 0, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay %lu, total = %lu us",
         pSession->ucInternalSessionId, stream_id, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->nodes[ASM_NODE_PP].alg_delay, pStream->pathAlgDelayUs);
   return ADSP_EOK;

}

/**
 * This function returns the delay caused by the switch service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForSwitchSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t unDir)
{
   uint8_t stream_id   = pStream->ucStreamId;

   AudioStreamMgr_AggregateBufDelay(pStream, unDir, pStream->nodes[ASM_NODE_SWITCH].buf_delay);
   pStream->pathAlgDelayUs += pStream->nodes[ASM_NODE_SWITCH].alg_delay;

   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay query for switch service added buffering delay %lu, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay %lu, total = %lu us",
         pSession->ucInternalSessionId, stream_id, pStream->nodes[ASM_NODE_SWITCH].buf_delay, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->nodes[ASM_NODE_SWITCH].alg_delay, pStream->pathAlgDelayUs);

   return ADSP_EOK;
}

/**
 * This function returns the delay caused by the encoder service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForEncoderSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t unDir)
{
   uint8_t stream_id   = pStream->ucStreamId;

   AudioStreamMgr_AggregateBufDelay(pStream, unDir, 0);
   pStream->pathAlgDelayUs += 0;

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay query for encoder added buffering delay 0, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay 0, total = %lu us",
         pSession->ucInternalSessionId, stream_id, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->pathAlgDelayUs);

   return ADSP_EOK;
}

/**
 * This function adds zero delay to the stream path. It can be used as a dummy function for delays
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayZero (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t unDir)
{
   uint8_t stream_id   = pStream->ucStreamId;

   AudioStreamMgr_AggregateBufDelay(pStream, unDir, 0);
   pStream->pathAlgDelayUs += 0;

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay added buffering delay 0, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay 0, total = %lu us",
         pSession->ucInternalSessionId, stream_id, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->pathAlgDelayUs);

   return ADSP_EOK;
}
