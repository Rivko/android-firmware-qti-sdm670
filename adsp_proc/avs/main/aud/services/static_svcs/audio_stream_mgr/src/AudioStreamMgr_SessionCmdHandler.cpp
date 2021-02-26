/**
@file AudStreamMgr_SessionCmdHandler.cpp

@brief This file contains the implementation for session cmdQ Handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header $ //source/qcom/qct/multimedia2/Audio/elite/static_svcs/AudioStreamMgr/main/latest/src/AudioStreamMgr_SessionCmdHandler.cpp#18 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_StreamCmdHandler.h"
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

static void ASM_BringAllStreamsOutOfSuspend(AudioStreamMgr_SessionStateType *pSession)
{
   for (uint8_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if (pSession->aStreams[i].state == ASM_STREAM_SUSPEND)
      {
         pSession->aStreams[i].state = ASM_STREAM_PAUSE;
      }
   }
}

/* =======================================================================
 **        Session State Machine: CmdQ message handler for APR messages
 ** ======================================================================= */
/* This is to process client command. */
ADSPResult AudioStreamMgr_SessionCmdQAprPktHandler( AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[intSessionId]; //this may be null

   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[intSessionId];

   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   /** Sanity check */
   if ( (ELITE_APR_PACKET != pStSession->cmdMsg.unOpCode) || (!pPkt) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: invalid opcode/payload 0x%lx",
            intSessionId, pStSession->cmdMsg.unOpCode);

      return ADSP_EFAILED;
   }

   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   uint8_t stream_id        = ASM_GET_STREAM_ID( port ); //depending on stream cmd or session cmd, then stream id may not be relevant.

   /* kw check: stream id value checked*/
   ASM_CHECK_STREAM_ID(stream_id);

   uint32_t ulOpCode = elite_apr_if_get_opcode( pPkt );

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: receive apr command 0x%lx at port [0x%4x]",
         intSessionId,  stream_id, ulOpCode, port );

   if ((NULL != pSession))
   {
      if((pSession->unState == AUDIO_STREAM_MGR_SESSION_SUSPEND) && (ulOpCode != ASM_SESSION_CMD_SUSPEND))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Resuming from Suspend state of session", intSessionId, stream_id);
         pSession->unState = AUDIO_STREAM_MGR_SESSION_PAUSE;

         //currently, all streams are sync'ed wrt session cmd like run/pause/suspend.
         //Hence bringing all of them out of suspend is fine. If we have diff states for diff streams, then
         //this method won't work.
         ASM_BringAllStreamsOutOfSuspend(pSession);
      }

      pSession->unReqResps = 0;
      pSession->nOverallResult = ADSP_EOK;
   }

   switch ( ulOpCode )
   {

   case ASM_STREAM_CMD_OPEN_WRITE_V3:
   {
      return AudioStreamMgr_OpenRxStreamCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_READ_V3:
   {
      return AudioStreamMgr_OpenTxStreamCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_READWRITE_V2:
   {
      return AudioStreamMgr_OpenRxTxStreamCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_LOOPBACK_V2:
   case ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK:
   {
      return AudioStreamMgr_OpenLoopBackCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED:
   {
      return AudioStreamMgr_OpenWriteCompressedCmdHandler(pMe,intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_READ_COMPRESSED:
   {
      return AudioStreamMgr_OpenReadCompressedCmdHandler(pMe,intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE:
   {
      return AudioStreamMgr_OpenPullModeRxStreamCmdHandler(pMe,intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_OPEN_PUSH_MODE_READ:
   {
      return AudioStreamMgr_OpenPushModeTxStreamCmdHandler(pMe,intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_CLOSE:
   {
      return AudioStreamMgr_CloseStreamCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_SET_ENCDEC_PARAM:
   case ASM_STREAM_CMD_SET_ENCDEC_PARAM_V2:
   {
      return AudioStreamMgr_SetEncDecParamCmdHandler(pMe, intSessionId, stream_id);
   }

   case  ASM_STREAM_CMD_SET_PP_PARAMS_V2:
   case  ASM_STREAM_CMD_SET_PP_PARAMS_V3:
   {
      return AudioStreamMgr_SetPpParamCmdHandler(pMe,intSessionId, stream_id);
   }

   case ASM_SESSION_CMD_RUN_V2:
   {
      return AudioStreamMgr_SessionRunCmdHandler(pMe,intSessionId);
   }

   case ASM_SESSION_CMD_PAUSE:
   {
      return AudioStreamMgr_SessionPauseCmdHandler(pMe,intSessionId);
   }
   case ASM_SESSION_CMD_SUSPEND:
   {
      return AudioStreamMgr_SessionSuspendCmdHandler(pMe,intSessionId);
   }

   case ASM_STREAM_CMD_FLUSH:
   case ASM_STREAM_CMD_FLUSH_V2:
   {
      return AudioStreamMgr_StreamFlushCmdHandler(pMe,intSessionId,stream_id);
   }

   case ASM_STREAM_CMD_FLUSH_READBUFS:
   {
      return AudioStreamMgr_FlushReadBufsCmdHandler(pMe, intSessionId,stream_id);
   }

   case ASM_STREAM_CMD_GET_PP_PARAMS_V2:
   case ASM_STREAM_CMD_GET_PP_PARAMS_V3:
   {
      return AudioStreamMgr_GetPpParamCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST:
   case ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2:
   {
      return AudioStreamMgr_GetTopoModuleCmdHandler(pMe, intSessionId, stream_id);
   }

   case ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS:
   case ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS:
   {
      return AudioStreamMgr_RegisterMatrixFlowEventCmdHandler(pMe, intSessionId);
   }
   case ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2:
   {
      return AudioStreamMgr_AdjustSessionClkCmdHandler(pMe, intSessionId);
   }
   case ASM_STREAM_CMD_DTMF_CTL:
   {
      return AudioStreamMgr_SessionDtmfCmdHandler(pMe,intSessionId,stream_id);
   }
   case ASM_SESSION_CMD_GET_PATH_DELAY_V2:
   {
      return AudioStreamMgr_GetPathDelay(pMe,intSessionId);
   }
   case  ASM_SESSION_CMD_SET_MTMX_STRTR_PARAMS_V2:
   {
      return AudioStreamMgr_SetMtMxStRtrParamCmdHandler(pMe, intSessionId);
   }
   case ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2:
   {
      return AudioStreamMgr_GetMtMxStRtrParamCmdHandler(pMe, intSessionId);
   }
   case ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS:
   {
      return AudioStreamMgr_RegisterEncDecEvents(pMe, intSessionId, stream_id);
   }
   case ASM_STREAM_CMD_REGISTER_IEC_61937_FMT_UPDATE:
   {
      return AudioStreamMgr_RegisterIEC61937MediaFmtEvents(pMe, intSessionId, stream_id);
   }
   case ASM_STREAM_CMD_REGISTER_PP_EVENTS:
   {
	   return AudioStreamMgr_RegisterDeRegisterPPEvents(pMe,intSessionId,stream_id);
   }
   default:
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: unsupported command 0x%lx at port [0x%4lx]",
            intSessionId,  stream_id,
            ulOpCode, (uint32_t)elite_apr_if_get_dst_port( pPkt ) );


      // Generate ACK and free apr packet
      AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0);

   }
   }

   return ADSP_EOK;
}

/*This function process unsupported messages in the session's command queue.*/
ADSPResult AudioStreamMgr_SessionCmdQUnSupportedHandler(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[intSessionId];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[intSessionId];

   if (NULL == pSession)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: invalid session", intSessionId);
      return ADSP_EFAILED;
   }

   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: unsupported command 0x%lx at port [0x%4lx]",
         intSessionId, (uint32_t)elite_apr_if_get_opcode( pPkt ), (uint32_t)elite_apr_if_get_dst_port( pPkt ) );

   /*no ack for unsupported commands like in elite_svc_unsupported function*/

   return ADSP_EUNSUPPORTED;
}


/*
Based on ISOD, on both TX/RX, session is in Paused state when created, meaning both
Proc Svc and Matrix Svc are in Paused state.

To move a session to RUN state, Proc needs to be Run first, then follow with a RUN
to Matrix.

run command is given from source to sink.

 */
ADSPResult  AudioStreamMgr_SessionRunCmdHandler(AudioStreamMgr_t *pMe, uint8_t int_session_id)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   //this is a session cmd. streamId is not relevant
   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: enter SessionRun command handler",
         pSession->ucInternalSessionId);

   /* ERROR checking for not in puase state */
   if ( pSession->unState != AUDIO_STREAM_MGR_SESSION_PAUSE )
   {
      // do nothing if already in RUN state and still ack succeccs.
      // For any other state, ACK NOTREADY
      if (pSession->unState == AUDIO_STREAM_MGR_SESSION_RUN )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,-]: Already in Run State",
               pSession->ucInternalSessionId);
         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, NULL, 0, 0 );
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: Bad State State:%d",
               pSession->ucInternalSessionId, pSession->unState);
         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_ENOTREADY, NULL, 0, 0 );
      }
      return result;
   }

   asm_session_cmd_run_v2_t *pParamPayload;
   uint32_t start_type;
   pParamPayload = (asm_session_cmd_run_v2_t *)elite_apr_if_get_payload_ptr(pPkt);

   //store the previous run command so that streams can be opened in run state and be automatically put to run state using this.
   pSession->prev_run_cmd = *pParamPayload;

   result = asm_get_run_start_time(pParamPayload, &start_type);

   if ( ( (ASM_RX_PULL_MODE == pSession->unMode) ||
         (ASM_TX_PUSH_MODE == pSession->unMode) )  &&
         (ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE != start_type) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: Run mode %lu unsupported in session mode %d",
            pSession->ucInternalSessionId,  start_type, pSession->unMode);

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0 );
   }

   result = ASM_SortStreams(pMe, pSession, ASM_STREAM_SORT_SRC_TO_SINK);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: couldn't sort streams.",
            pSession->ucInternalSessionId);

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0 );
   }

   ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_RUN_TOPO);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[0].stream_id);

   result = ASM_IterateRunPauseSuspendOverNodesAndStreams(pMe, pSession, pStream, ASM_INVALID_NODE, ASM_ExecRunFTableEntry);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send Run command 0x%lx!!",
            pSession->ucInternalSessionId,  (uint32_t) result);
      return result;
   }

   if (0 == pSession->unReqResps)
   {
      AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: leave RUN command handler",
         pSession->ucInternalSessionId);
   return result;

}

/*
 * Suspend command handler for long pause state of DSP
 *
 * suspend is sent from sink to source.
 * */
ADSPResult  AudioStreamMgr_SessionSuspendCmdHandler(AudioStreamMgr_t *pMe, uint8_t int_session_id)
{
   ADSPResult result  = ADSP_EOK;
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,-]: enter SUSPEND command handler",
         pSession->ucInternalSessionId);

   if ( pSession->unState != AUDIO_STREAM_MGR_SESSION_PAUSE &&(pSession->unState != AUDIO_STREAM_MGR_SESSION_SUSPEND))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]:Session is not in pause state, suspend failed!!\n",
            pSession->ucInternalSessionId);
      AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0 );//UNSUPPORTED
      return ADSP_EUNSUPPORTED;
   }
   if( pSession->unState == AUDIO_STREAM_MGR_SESSION_SUSPEND )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Already in suspend Session !!\n",
            pSession->ucInternalSessionId);
      AudioStreamMgr_GenerateAck( pPkt, ADSP_EALREADY, NULL, 0, 0 );// ADSP_EALREADY
      return ADSP_EOK;
   }

   result = ASM_SortStreams(pMe, pSession, ASM_STREAM_SORT_SINK_TO_SRC);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: couldn't sort streams.",
            pSession->ucInternalSessionId);

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0 );
   }

   ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_SUSPEND_TOPO);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[0].stream_id);

   result = ASM_IterateRunPauseSuspendOverNodesAndStreams(pMe, pSession, pStream, ASM_INVALID_NODE, ASM_ExecSuspendFTableEntry);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send suspend command 0x%lx!!",
            pSession->ucInternalSessionId,  (uint32_t) result);
      return result;
   }

   if (0 == pSession->unReqResps)
   {
      AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,-]: leave SUSPEND command handler",
         pSession->ucInternalSessionId);
   return ADSP_EOK;
}

/*
Based on ISOD, on both TX/RX,  both
Proc Svc and Matrix Svc needs to be paused.

To move a session to pause state, Proc needs to be pause first, then follow with a pause
to Matrix.


pause involves 2 pause commands
1. pause sent to PP (starting from the one close to the matrix)
2. pause sent to all services (sink to source).
 */
ADSPResult  AudioStreamMgr_SessionPauseCmdHandler(AudioStreamMgr_t *pMe, uint8_t int_session_id)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: enter PAUSE command handler",
         pSession->ucInternalSessionId);

   /* ERROR checking for not in puase state */
   if ( pSession->unState != AUDIO_STREAM_MGR_SESSION_RUN )
   {
      // do nothing if already in PAUSE state and still ack succeccs.
      // For any other state, ACK NOTREADY
      if (pSession->unState == AUDIO_STREAM_MGR_SESSION_PAUSE )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,-]: Already in PAUSE State",
               pSession->ucInternalSessionId);
         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, NULL, 0, 0 );
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: Bad State, State=%d",
               pSession->ucInternalSessionId, pSession->unState);
         result = AudioStreamMgr_GenerateAck( pPkt, ADSP_ENOTREADY, NULL, 0, 0 );
      }
      return result;
   }

   //pause is sink to source because, if source is paused first then sink will under-run.
   result = ASM_SortStreams(pMe, pSession, ASM_STREAM_SORT_SINK_TO_SRC);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: couldn't sort streams.",
            pSession->ucInternalSessionId);

      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNSUPPORTED, NULL, 0, 0 );
   }

   ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_PAUSE1_TOPO);

   result = ASM_IterateCmdOverNodesAndStreamsParallely(pMe, pSession, ASM_ExecPause1FTableEntry);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send pause1 command 0x%lx!!",
            pSession->ucInternalSessionId,  (uint32_t) result);
   }

   if (0 == pSession->unReqResps)
   {
      ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_PAUSE2_TOPO);

      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[0].stream_id);

      result = ASM_IterateRunPauseSuspendOverNodesAndStreams(pMe, pSession, pStream, ASM_INVALID_NODE, ASM_ExecPause2FTableEntry);

      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send pause2 command 0x%lx!!",
               pSession->ucInternalSessionId,  (uint32_t) result);
         return result;
      }
   }

   if (0 == pSession->unReqResps)
   {
      AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
   }

   MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: leave PAUSE command handler",
         pSession->ucInternalSessionId);
   return result;


}

AudioStreamMgr_StreamStateType *ASM_GetStreamForMtmxStRtrSessionCmd(AudioStreamMgr_SessionStateType *pSession, elite_apr_port_t  port)
{
   AudioStreamMgr_StreamStateType *pStream = NULL;
   uint32_t numStreamToRxMtxStRtr = ASM_GetNumStreamsConnectedToRxMatrixStRtr(pSession);

   uint8_t stream_id = ASM_MAX_SUPPORTED_STREAM_ID+1; //this is a session cmd. streamId is traditionally nonrelevant, but now we support multiple connections to mtx, stream router for group session.

   if (0 == numStreamToRxMtxStRtr)
   {
      pStream = NULL;
   }
   else if (1 == numStreamToRxMtxStRtr) //if only one stream is connected to mtx/stRtr, support this way for backward compatibility (by ignoring stream id provided).
   {
      pStream = ASM_GetStreamConnectedToRxMatrixStRtr(pSession);
      if(pStream)
      {
         stream_id = pStream->ucStreamId;
      }
   }
   else
   {
      stream_id = ASM_GET_STREAM_ID( port );
      if ( stream_id > ASM_MAX_SUPPORTED_STREAM_ID)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"incorrect stream id %lu", (uint32_t) stream_id);
         return NULL;
      }

      pStream = ASM_GetStreamInstance(pSession, stream_id);

      //check if this stream is connected to matrix or stream router.
      if (pStream && !pStream->nodes[ASM_NODE_RX_ST_RTR].pSvcHandle && !pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle)
      {
         return NULL;
      }
   }
   return pStream;
}


ADSPResult AudioStreamMgr_RegisterMatrixFlowEventCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pInAprPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pInAprPkt);

   uint32_t direction;
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pInAprPkt);
   uint8_t stream_id;
   AudioStreamMgr_StreamStateType *pStream = NULL;
   
   if (ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS == pInAprPkt->opcode)
   {
      direction = ASM_RX_DIR;
	   pStream = ASM_GetStreamForMtmxStRtrSessionCmd(pSession, port);
   }
   else
   {
      QURT_ELITE_ASSERT(ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS == pInAprPkt->opcode);
      direction = ASM_TX_DIR;
	   pStream = ASM_GetStreamConnectedToTxMatrixStRtr(pSession);
   }

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u, %u]: Cannot register matrix underflow/overflow event on a invalid/closed session!",
            pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99);
      //ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck((elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload), ADSP_EUNEXPECTED, 0 , 0 , 0);
   }
   stream_id = pStream->ucStreamId;

   asm_session_cmd_reg_tx_overflow_t *pEventCfg = (asm_session_cmd_reg_tx_overflow_t *)
                              elite_apr_if_get_payload_ptr( pInAprPkt );

   // Sanity check about mode
   // No such event for NT mode; NO rx_ underflow event for
   // recording; no tx_overflow event for playback;both events allowed
   // for loopback mode.
   if ( ( (ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS == pInAprPkt->opcode) &&
         (!pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle) ) ||

         ( (ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS == pInAprPkt->opcode) &&
               (!pStream->nodes[ASM_NODE_TX_MTX].pSvcHandle) ))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: Invalid mode 0x%x for matrix flow event 0x%lx",
            pSession->ucInternalSessionId,
            stream_id,
            pSession->unMode,
            pInAprPkt->opcode);
      result = ADSP_EUNSUPPORTED;
      goto __bailout;
   }

   // send message to matrix
   result = AudioStreamMgr_SessionRegisterMatrixFlowEvent(  pMe,
         pSession,
         pStream,
         pEventCfg,
         direction);

   if ( ADSP_FAILED( result ))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,%u]: \
                                         fail to configure matrix flow event 0x%lx",
                                         pSession->ucInternalSessionId,
                                         stream_id,
                                         (uint32_t) result );
      goto __bailout;
   }

   return result;

   __bailout:
   result = AudioStreamMgr_GenerateAck( pInAprPkt, result, NULL, 0, 0 );
   return result;
}

ADSPResult  AudioStreamMgr_AdjustSessionClkCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id)
{
   uint32_t ulClientToken=0;

   //rx matrix mixer port is the last one in the graph
   uint32_t unPayloadSize = 0;
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomMtMxAdjustSessionClkType *adjustClkCmd = NULL;
   elite_svc_handle_t *pMtMxSvcHandle = NULL;
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];

   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);
   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   asm_session_cmdrsp_adjust_session_clock_v2_t errorRsp;
   //init error rsp
   memset(&errorRsp, 0, sizeof(errorRsp));

   asm_session_cmd_adjust_session_clock_v2_t *pCmd;
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   uint8_t stream_id = 99;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamForMtmxStRtrSessionCmd(pSession, port);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u, %u]: Cannot adjust session time on a invalid/closed session!",pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99);
      //ACK that this command is not expected in such state and release packet
      errorRsp.status = ADSP_EUNSUPPORTED;
      goto __bailout;
   }
   stream_id = pStream->ucStreamId;

   pCmd = (asm_session_cmd_adjust_session_clock_v2_t *)elite_apr_if_get_payload_ptr(pPkt);

   //create an internal message and send to matrix port
   ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_MTX);

   unPayloadSize = sizeof(EliteMsg_CustomMtMxAdjustSessionClkType);

   elite_msg_any_t msg;
   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create Elite message",
            pSession->ucInternalSessionId,  stream_id);

      //ack with error
      errorRsp.status = ADSP_ENOMEMORY;
      goto __bailout;
   }

   adjustClkCmd = (EliteMsg_CustomMtMxAdjustSessionClkType *)msg.pPayload;

   adjustClkCmd->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_ADJUST_SESSION_CLK;
   adjustClkCmd->unPortID = pStream->mtMxPortParam[ADM_RX_DIR].unPortID;
   adjustClkCmd->unPortDirection = pStream->mtMxPortParam[ADM_RX_DIR].unPortDirection;
   adjustClkCmd->llAdjustTime = (int64_t)((((uint64_t)pCmd->adjust_time_msw) << 32)
         | (((uint64_t)pCmd->adjust_time_lsw) & 0x00000000FFFFFFFFULL));

   pMtMxSvcHandle = (pStream->nodes[ASM_NODE_RX_MTX]).pSvcHandle;
   if (NULL == pMtMxSvcHandle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: NULL pMtMxSvcHandle!",
            pSession->ucInternalSessionId,  stream_id);
      //destroy the message and ack with error
      (void) elite_msg_release_msg(&msg);
      errorRsp.status = ADSP_EFAILED;
      goto __bailout;
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMtMxSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send Elite message",
            pSession->ucInternalSessionId,  stream_id);

      //destroy the message and ack with error
      (void) elite_msg_release_msg(&msg);
      errorRsp.status = ADSP_EFAILED;
      goto __bailout;
   }

   return ADSP_EOK;

   __bailout:
   return AudioStreamMgr_GenerateAck( pPkt, errorRsp.status,
         (void*)&errorRsp , sizeof(errorRsp) ,
         ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2 );
}

/*
   This function process the client command that are to configure Matrix/Stream Router.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_SetMtMxStRtrParamCmdHandler(AudioStreamMgr_t* pMe,
      uint8_t int_session_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   uint8_t stream_id;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamForMtmxStRtrSessionCmd(pSession, port);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u, %u]: set mtmx Strtr param on a invalid/closed session!",pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99);
      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }
   stream_id = pStream->ucStreamId;

   ADSPResult result;
   elite_msg_any_t msg;
   ASM_GraphNode *asmNode;
   ASM_GraphNodeIndex asmNodeIndex;
   asm_session_cmd_set_mtmx_strtr_params_v2_t *pParamPayload;
   pParamPayload     = (asm_session_cmd_set_mtmx_strtr_params_v2_t *) elite_apr_if_get_payload_ptr(pPkt);
   ASM_direction dir = (ASM_direction)pParamPayload->direction;

   /** this call also makes sure valid session with matrix stream router exists and valid direction is set.*/
   ASM_MtxStRtrType mtMxOrStRtr  = ASM_GetMatrixOrStreamRouterNode(pStream, dir, &asmNodeIndex);

   if ( (ASM_NODE_MAX == asmNodeIndex))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot set params. either stream is closed or there's no matrix/stream router in this session. state=%d,node-ind=0x%lx,mode=%d",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, (uint32_t)asmNodeIndex, pSession->unMode);

      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }

   asmNode = &pStream->nodes[asmNodeIndex];

   uint32_t dataPayloadAddrLsw   = pParamPayload->data_payload_addr_lsw;
   uint32_t dataPayloadAddrMsw   = pParamPayload->data_payload_addr_msw;
   uint32_t memMapHandle         = pParamPayload->mem_map_handle;
   uint32_t dataPayloadSize      = pParamPayload->data_payload_size;
   uint32_t cmdStructSize        = sizeof(asm_session_cmd_set_mtmx_strtr_params_v2_t);
   int32_t *paramBase            = NULL;

   if (0 == memMapHandle) //in-band
   {
      // Address just beyond the Set params header asm_session_cmd_set_mtmx_strtr_params_v2_t
      //points to asm_session_param_data_v2_t
      paramBase  = (int32_t *)( ((int8_t*) pParamPayload) + cmdStructSize);
   }
   else //out-of-band
   {
      if (!isAlignedToCacheLine(dataPayloadAddrLsw))
      {
         result = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Physical memory address is not aligned to cache line, in set param.",
               pSession->ucInternalSessionId,  stream_id);
         return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
      }

      elite_mem_shared_memory_map_t bufferMemNode;
      bufferMemNode.unMemMapClient     = asmMemoryMapClient;
      bufferMemNode.unMemMapHandle     = memMapHandle;
      bufferMemNode.unMemSize          = dataPayloadSize;
      bufferMemNode.unPhysAddrLsw      = dataPayloadAddrLsw;
      bufferMemNode.unPhysAddrMsw      = dataPayloadAddrMsw;
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

   uint32_t ulClientToken           = ASM_PopulateClientToken(stream_id, asmNodeIndex);
   elite_svc_handle_t *pSvcHandle   = asmNode->pSvcHandle;
   uint32_t unPayloadSize;

   if (ASM_DEV_IF_MATRIX == mtMxOrStRtr)
   {
      unPayloadSize = sizeof(EliteMsg_CustomMtMxGetSetParamType);
   }
   else
   {
      unPayloadSize = sizeof(EliteMsg_CustomStreamRouterGetSetParam);
   }

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

   if (ASM_DEV_IF_MATRIX == mtMxOrStRtr)
   {
      EliteMsg_CustomMtMxGetSetParamType *pMsgPayload = (EliteMsg_CustomMtMxGetSetParamType*) msg.pPayload;
      pMsgPayload->unPortDirection  = pStream->mtMxPortParam[dir].unPortDirection;
      pMsgPayload->unPortID         = pStream->mtMxPortParam[dir].unPortID;
      pMsgPayload->unSecOpCode      = ELITEMSG_CUSTOM_MT_MX_SET_PARAM;
      pMsgPayload->unParamId        = ELITEMSG_PARAM_ID_CAL;
      pMsgPayload->pnParamData      = paramBase;
      pMsgPayload->unSize           = dataPayloadSize;
   }
   else
   {
      EliteMsg_CustomStreamRouterGetSetParam *pMsgPayload = (EliteMsg_CustomStreamRouterGetSetParam*) msg.pPayload;
      pMsgPayload->unPortDirection  = pStream->mtMxPortParam[dir].unPortDirection;
      pMsgPayload->unPortID         = pStream->mtMxPortParam[dir].unPortID;
      pMsgPayload->unSecOpCode      = ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PARAM;
      pMsgPayload->unParamId        = ELITEMSG_PARAM_ID_CAL;
      pMsgPayload->pnParamData      = paramBase;
      pMsgPayload->unSize           = dataPayloadSize;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Send SetParam to svc[0x%lx,0x%lx]",
         pSession->ucInternalSessionId,  stream_id,
         pSvcHandle->unSvcId,        (uint32_t) (pSvcHandle->threadId));

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      //return message to buffer queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send SetParam command to matrix/stream router %d",
            pSession->ucInternalSessionId,  stream_id, mtMxOrStRtr);
      return AudioStreamMgr_GenerateAck(pPkt, result, NULL, 0, 0 );
   }

   return result;
}

/**
 * This is the command handler to ASM_STREAM_CMD_GET_PP_PARAMS/ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS
 */

ADSPResult AudioStreamMgr_GetMtMxStRtrParamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   ADSPResult result        = ADSP_EOK;
   elite_apr_packet_t *pPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   uint8_t stream_id;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamForMtmxStRtrSessionCmd(pSession, port);

   if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u, %u]: Cannot adjust session time on a invalid/closed session!",pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99);
      // ACK that this command is not expected in such state and release packet
      return AudioStreamMgr_GenerateAck( pPkt, ADSP_EUNEXPECTED, 0 , 0 , 0);
   }
   stream_id = pStream->ucStreamId;

   elite_apr_packet_t       *pAckAprPkt     = NULL;
   elite_svc_handle_t       *pSvcHandle     = NULL;
   uint32_t responseResult, ulClientToken = 0;
   void *responsePtr = NULL;
   elite_msg_any_t msg ;
   uint32_t cmdRespOpcode, cmdRespSize;
   ASM_GraphNode                    *asmNode;
   ASM_GraphNodeIndex                    asmNodeIndex;
   elite_mem_shared_memory_map_t    bufferMemNode;
   asm_session_cmd_get_mtmx_strtr_params_v2_t *pMtMxStStrAprPayload = NULL;
   asm_session_cmdrsp_get_mtmx_strtr_params_v2_t MtMxStStrResponse;

   //Get APR payload
   pMtMxStStrAprPayload = (asm_session_cmd_get_mtmx_strtr_params_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt));
   cmdRespOpcode        = ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2;
   cmdRespSize          = sizeof(asm_session_cmdrsp_get_mtmx_strtr_params_v2_t);

   ASM_direction dir                = (ASM_direction)pMtMxStStrAprPayload->direction;
   /** this call also makes sure valid session with matrix stream router exists and valid direction is set.*/
   ASM_MtxStRtrType mtMxOrStRtr     = ASM_GetMatrixOrStreamRouterNode(pStream, dir, &asmNodeIndex);

   bufferMemNode.unMemMapClient     = asmMemoryMapClient;
   bufferMemNode.unMemMapHandle     = pMtMxStStrAprPayload->mem_map_handle;
   bufferMemNode.unMemSize          = pMtMxStStrAprPayload->param_max_size;
   bufferMemNode.unPhysAddrLsw      = pMtMxStStrAprPayload->data_payload_addr_lsw;
   bufferMemNode.unPhysAddrMsw      = pMtMxStStrAprPayload->data_payload_addr_msw;
   bufferMemNode.unVirtAddr         = 0;
   memset(&MtMxStStrResponse, 0, cmdRespSize);

   // Sanity check for states: can get matrix/stream router for all states except close.
   if ( (ASM_NODE_MAX == asmNodeIndex))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot get params. either stream is closed or there's no pp/matrix/stream router in this session. state=%d,node-ind=0x%lx,mode=%d",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, (uint32_t)ASM_NODE_MAX, pSession->unMode);
      result = ADSP_ENOTREADY;
      responseResult = (uint32_t )result;
      goto __bailout;
   }

   asmNode = &pStream->nodes[asmNodeIndex];

   // Param Max Size can not be less then sizeof(asm_session_param_data_v2_t)
   if ( bufferMemNode.unMemSize < sizeof(asm_session_param_data_v2_t) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: invalid max size %ld",
            pSession->ucInternalSessionId,  stream_id, bufferMemNode.unMemSize);
      result = ADSP_EBADPARAM;
      responseResult = (uint32_t )result;
      goto __bailout;
   }

   pSvcHandle     = asmNode->pSvcHandle;
   ulClientToken  = ASM_PopulateClientToken(stream_id, asmNodeIndex);
   uint32_t unPayloadSize;

   if (ASM_DEV_IF_MATRIX == mtMxOrStRtr)
   {
      unPayloadSize = sizeof(EliteMsg_CustomMtMxGetSetParamType);
   }
   else
   {
      unPayloadSize = sizeof(EliteMsg_CustomStreamRouterGetSetParam);
   }

   // Allocate message: common to both in-band and out-of-band.
   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken, 0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create ELITE_CUSTOM_MSG for getParam message",
            pSession->ucInternalSessionId,  stream_id);
      responseResult = (uint32_t )result;
      goto __bailout;
   }

   int32_t *pParamBase; //points to asm_session_param_data_v2_t

   // Now the message content depends on whether in-band or out-of-band.
   // For in-band, static service needs to allocate large enough APR packet
   if ( 0 == bufferMemNode.unMemMapHandle ) //in-band
   {
      uint32_t ackPyloadSize = cmdRespSize + bufferMemNode.unMemSize;

      // Allocate an APR packet
      result = elite_apr_if_alloc_cmd_rsp( AudioStreamMgr_GetAprHandle(),
            elite_apr_if_get_dst_addr( pPkt),
            elite_apr_if_get_dst_port( pPkt),
            elite_apr_if_get_src_addr( pPkt),
            elite_apr_if_get_src_port( pPkt),
            elite_apr_if_get_client_token( pPkt),
            cmdRespOpcode,
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
         responseResult = (uint32_t )result;
         goto __bailout1;
      }

      //for in-band, asm_session_param_data_v2_t immediately follows status
      int32_t* pCmdRspAprPktBase = (int32_t*)elite_apr_if_get_payload_ptr(pAckAprPkt);
      pParamBase = pCmdRspAprPktBase + 1; //Offset by the status to access the module ID.

      // store APR packet: thus in-band is identified by this packet which as
      //opcode=ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2
      pStSession->cmdMsg.pPayload = pAckAprPkt;

   }
   else
   {
      if (!isAlignedToCacheLine(bufferMemNode.unPhysAddrLsw))
      {
         result = ADSP_EBADPARAM;
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Physical memory address is not aligned to cache line, in get param.",
               pSession->ucInternalSessionId,  stream_id);
         responseResult = (uint32_t )result;
         goto __bailout1;
      }

      result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
            bufferMemNode.unMemSize, &bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for GetParam.",
               pSession->ucInternalSessionId,  stream_id);
         responseResult = (uint32_t )result;
         goto __bailout1;
      }
      //no need to invalidate cache since the buffer is only being written.
      //for out-band, asm_session_cmdrsp_get_mtmx_strtr_params_v2_t and asm_session_param_data_v2_t are not contiguous
      pParamBase  = (int32_t*)(bufferMemNode.unVirtAddr);
   }

   if (ASM_DEV_IF_MATRIX == mtMxOrStRtr)
   {
      EliteMsg_CustomMtMxGetSetParamType *pMsgPayload = (EliteMsg_CustomMtMxGetSetParamType*) msg.pPayload;
      pMsgPayload->unPortDirection  = pStream->mtMxPortParam[dir].unPortDirection;
      pMsgPayload->unPortID         = pStream->mtMxPortParam[dir].unPortID;
      pMsgPayload->unSecOpCode      = ELITEMSG_CUSTOM_MT_MX_GET_PARAM;
      pMsgPayload->unParamId        = ELITEMSG_PARAM_ID_CAL;
      pMsgPayload->pnParamData      = pParamBase; //this points to asm_session_param_data_v2_t
      pMsgPayload->unSize           = bufferMemNode.unMemSize; //on response mtmx/strtr updates this with actual size
   }
   else
   {
      EliteMsg_CustomStreamRouterGetSetParam *pMsgPayload = (EliteMsg_CustomStreamRouterGetSetParam*) msg.pPayload;
      pMsgPayload->unPortDirection  = pStream->mtMxPortParam[dir].unPortDirection;
      pMsgPayload->unPortID         = pStream->mtMxPortParam[dir].unPortID;
      pMsgPayload->unSecOpCode      = ELITEMSG_CUSTOM_STREAM_ROUTER_GET_PARAM;
      pMsgPayload->unParamId        = ELITEMSG_PARAM_ID_CAL;
      pMsgPayload->pnParamData      = pParamBase; //this points to asm_session_param_data_v2_t
      pMsgPayload->unSize           = bufferMemNode.unMemSize; //on response mtmx/strtr updates this with actual size
   }

   //Fill up the memory with the ModuleID and ParamID so that matrix/stream router doesn't have to do it
   asm_session_param_data_v2_t *psParamData;
   psParamData = (asm_session_param_data_v2_t *)(pParamBase);
   psParamData->module_id   = pMtMxStStrAprPayload->module_id;
   psParamData->param_id    = pMtMxStStrAprPayload->param_id;
   psParamData->param_size  = bufferMemNode.unMemSize;
   psParamData->reserved    = 0;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send GetParam to Matrix/Stream Router 0x%lx",
            pSession->ucInternalSessionId,  stream_id, (uint32_t) result );
      responseResult = (uint32_t )result;
      goto __bailout1;
   }

   if (0 == bufferMemNode.unMemMapHandle ) //in-band
   {
      // free up the incoming apr packet since we create another one for ack already.
      elite_apr_if_free( AudioStreamMgr_GetAprHandle(),  pPkt);
   }

   return result;

   __bailout1:
   elite_msg_release_msg( &msg );
   __bailout:
   MtMxStStrResponse.status = responseResult;
   responsePtr = (void*)&MtMxStStrResponse;

   if (pAckAprPkt)
   {
      elite_apr_if_free( AudioStreamMgr_GetAprHandle(),  pAckAprPkt);
      pAckAprPkt = NULL;
   }
   result =  AudioStreamMgr_GenerateAck( pPkt, result, (void*)responsePtr,
         cmdRespSize, cmdRespOpcode);

   return result;

}

ADSPResult  AudioStreamMgr_GetPathDelay(AudioStreamMgr_t *pMe,
      uint8_t int_session_id)
{
   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[int_session_id];
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[int_session_id];
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   ASM_CHECK_SESSION_NULLNESS(pSession, pPkt);

   ADSPResult result = ADSP_EOK;

   /* ERROR checking for not in run or pause state */
   if ( (pSession->unState != AUDIO_STREAM_MGR_SESSION_RUN) &&
         (pSession->unState != AUDIO_STREAM_MGR_SESSION_PAUSE) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Session path delay query invalid if session is in not in run or pause state.",
            pSession->ucInternalSessionId);
      result =  ADSP_EUNEXPECTED;
      goto __bailout;
   }

   /* supported only for loopback, pull mode playback and push mode record  */
   if ( (!ASM_IsAsmLoopback(pSession->unMode)) &&
         (ASM_RX_PULL_MODE != pSession->unMode) &&
         (ASM_TX_PUSH_MODE != pSession->unMode) &&
         (ASM_RX_MODE != pSession->unMode) &&
         (ASM_TX_MODE != pSession->unMode) &&
         (ASM_COMPRESSED_PLAYBACK_MODE != pSession->unMode) &&
         (ASM_COMPRESSED_INPUT_MODE != pSession->unMode) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Session path delay query invalid for this mode %u",
            pSession->ucInternalSessionId, pSession->unMode);
      result =  ADSP_EUNEXPECTED;
      goto __bailout;
   }

   result = ASM_SortStreamsForPathDelay(pMe, pSession);
   if (ADSP_FAILED(result)) goto __bailout;

   for (uint32_t j = 0; j < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; j++)
   {
      if (0 == pSession->aTempStreamIdNodeIndexList[j].stream_id) break;
      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[j].stream_id);
      if(pStream != NULL)
      {
          pStream->pathFTRTBufDelayUs = 0;
          pStream->pathRTBufDelayUs = 0;
          pStream->pathAlgDelayUs = 0;
      }
   }

   ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_FLUSH_QUERY_DELAY_TOPO);

   result = ASM_IterateCmdOverNodesAndStreamsParallely(pMe, pSession, ASM_ExecQueryDelayFTableEntry);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send get_path_delay command 0x%lx!!",
            pSession->ucInternalSessionId,  (uint32_t) result);
   }

   pSession->nOverallResult |= result;

   //if no response to be waited for then send the ack.
   if (0 == pSession->unReqResps)
   {
      ADSPResult errorCode = ADSP_SUCCEEDED(pSession->nOverallResult) ? ADSP_EOK : ADSP_EFAILED;
      result = AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, errorCode);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,"AudioStreamMgr [%u,-]: leave Session path delay query  handler",
         pSession->ucInternalSessionId);

   return result;

   __bailout:
   //send the failure ACK
   result = AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, result);
   return result;
}


