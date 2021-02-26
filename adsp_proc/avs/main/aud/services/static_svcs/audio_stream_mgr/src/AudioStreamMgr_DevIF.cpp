/**
@file AudStreamMgr_DevIF.cpp

@brief This file contains the implementation for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_DevIF.cpp#1 $

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
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "EliteMsg_AdmCustom.h"
#include "EliteMsg_AfeCustom.h"
#include "MixerSvc_Interface.h"
#include "StreamRouterSvc.h"
#include "AudioDecSvc.h"

#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"

#include "AudioStreamMgr_EliteMsg_Custom.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Private Declarations
 ** ----------------------------------------------------------------------- */
static ADSPResult AudioStreamMgr_FindRxStreamSvcAndGenericDevNodes(AudioStreamMgr_SessionStateType *pSession,
                                                       AudioStreamMgr_StreamStateType *pStream,
                                                       ASM_GraphNodeIndex *stream_svc_node,
                                                       ASM_GraphNodeIndex *dev_svc_node);

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

static const uint32_t DEFAULT_SAMPLING_RATE __attribute__ ((unused)) = 48; /// kHz
static const uint32_t DEFAULT_AFE_PERIOD    __attribute__ ((unused)) = 5 ; /// milli second
static ASM_GraphNode* AudioStreamMgr_FindTxSinkNode(AudioStreamMgr_StreamStateType *pStream);

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */


/**
 * This function request a Rx Matrix handle
 */
int AudioStreamMgr_SessionRequestRxDeviceHandle( AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   elite_msg_any_t msg;
   uint32_t unSize;
   ADSPResult result;
   uint32_t ulClientToken=0;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Request RX Handle",
         pSession->ucInternalSessionId,  stream_id);

   ASM_GraphNodeIndex dev_node_ind = ASM_GetRxDeviceNodeInTopo(pStream);

   if ( ((dev_node_ind == ASM_NODE_RX_GENERIC) && pStream->generic_rx_dummy_handle) ||
        ((dev_node_ind != ASM_NODE_RX_GENERIC) && pStream->nodes[dev_node_ind].pSvcHandle))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Returning as Rx handle is already present.",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_EOK;
   }

   bool_t is_compressed = (dev_node_ind == ASM_NODE_RX_ST_RTR) ? TRUE : FALSE;
   ulClientToken = ASM_PopulateClientToken(stream_id, dev_node_ind);

   unSize = sizeof(EliteMsg_CustomAdmStreamConnectType);
   if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize, ELITE_CUSTOM_MSG, 
         pStSession->pSessionRespQ,
         ulClientToken, 0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Fail to create msg",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   EliteMsg_CustomAdmStreamConnectType *pConnectMsgPayload = 
         (EliteMsg_CustomAdmStreamConnectType*) msg.pPayload;
   result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, ulClientToken,
         &(pStream->AsmCbData));
   pConnectMsgPayload->eliteCb.pCbFct =
         AudioStreamMgr_GenerateClientEventFromCb;
   pConnectMsgPayload->eliteCb.pCbHandle = &(pStream->AsmCbData);
   pConnectMsgPayload->eliteCb.ulHandleSize =
         sizeof(AudioStreamMgr_CallBackHandleType);

   if (ASM_IsGaplessSession(pSession))
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pSession->pFirstGaplessStrm);
   }
   else
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pStream);
   }

   pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_CONNECT;
   pConnectMsgPayload->direction = ADM_RX_DIR;
   pConnectMsgPayload->unDataLogId = pStream->unDataLogId;

   //set default values for Rx matrix
   pConnectMsgPayload->phPort = NULL;
   pConnectMsgPayload->bits_per_sample = 16;
   pConnectMsgPayload->is_compressed = is_compressed;
   pConnectMsgPayload->format_id = is_compressed?pStream->sink_fmt_id:0; //required only for compressed
   pConnectMsgPayload->stream_perf_mode = pStream->stream_perf_mode;
   pConnectMsgPayload->buffering_ms = ASM_RX_MATRIX_INPUT_BUFFERING_REGULAR;
   if ((ASM_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode ) ||
         (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode) ||
         (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode ) ||
         (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pStream->stream_perf_mode))
   {
      pConnectMsgPayload->buffering_ms = ASM_RX_MATRIX_INPUT_BUFFERING_LOWLATENCY;
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMe->pDeviceSvcCmdQ ,(uint64_t *)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:FAILED to connect ADM service!",
            pSession->ucInternalSessionId,  stream_id);
      elite_msg_release_msg(&msg);
      return result;
   }

   return result;
}

/**
 * This function request a Tx Matrix handle
 *
 */
int AudioStreamMgr_SessionRequestTxDeviceHandle( AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint8_t timestamp_type)
{
   elite_msg_any_t msg;
   uint32_t unSize;
   ADSPResult result;
   uint32_t ulClientToken=0;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Request TX Handle",
         pSession->ucInternalSessionId,  stream_id);
   ASM_GraphNodeIndex dev_node_ind = ASM_GetTxDeviceNodeInTopo(pStream);

   if ((dev_node_ind != ASM_NODE_TX_GENERIC) && (pStream->nodes[dev_node_ind].pSvcHandle))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Returning as Tx handle is already present.",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_EOK;
   }

   bool_t is_compressed = dev_node_ind == ASM_NODE_TX_ST_RTR;
   ulClientToken = ASM_PopulateClientToken(stream_id, dev_node_ind);

   unSize = sizeof(EliteMsg_CustomAdmStreamConnectType);
   if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize, ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken, 0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Fail to create msg",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   EliteMsg_CustomAdmStreamConnectType *pConnectMsgPayload =
         (EliteMsg_CustomAdmStreamConnectType*) msg.pPayload;
   result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, ulClientToken,
         &(pStream->AsmCbData));
   pConnectMsgPayload->eliteCb.pCbFct =
         AudioStreamMgr_GenerateClientEventFromCb;
   pConnectMsgPayload->eliteCb.pCbHandle = &(pStream->AsmCbData);
   pConnectMsgPayload->eliteCb.ulHandleSize =
         sizeof(AudioStreamMgr_CallBackHandleType);

   if (ASM_IsGaplessSession(pSession))
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pSession->pFirstGaplessStrm);
   }
   else
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pStream);
   }
   pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_CONNECT;
   pConnectMsgPayload->direction = ADM_TX_DIR;
   pConnectMsgPayload->unDataLogId = pStream->unDataLogId;

   pConnectMsgPayload->unPortMode = ADM_STREAM_CONNECT_PORT_MODE_RELATIVE_TS;
   if ( ASM_ABSOLUTE_TIMESTAMP == timestamp_type )
   {
      pConnectMsgPayload->unPortMode = ADM_STREAM_CONNECT_PORT_MODE_ABSOLUTE_TS;
   }

   //loopback may need delay on tx side, so configure with high buffer size and also use absolute TS for loopback.
   if (ASM_IsAsmLoopback(pSession->unMode))
   {
      pConnectMsgPayload->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_LOOPBACK;
      if( (false == is_compressed) && (ASM_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode ))
      {
         pConnectMsgPayload->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_LOWLATENCY_LOOPBACK;
      }

      pConnectMsgPayload->unPortMode = ADM_STREAM_CONNECT_PORT_MODE_ABSOLUTE_TS;
   }
   else
   {
      pConnectMsgPayload->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_REGULAR;
      if ((ASM_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode) ||
          (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode))
      {
         pConnectMsgPayload->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_LOWLATENCY;
      }
   }

   elite_svc_handle_t *pNextToSource;
   //get service next to TxMtx or TxStRtr in this topo
   ASM_GraphNodeIndex nodeNextToTxMtMxStRtrOrAfe = ASM_GetNodeNextToTxMtMxStRtrOrAfe(pStream);

   if (ASM_INVALID_NODE == nodeNextToTxMtMxStRtrOrAfe)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:There's no node next to Tx matrix/stream router or AFE",
            pSession->ucInternalSessionId,  stream_id);
      elite_msg_release_msg(&msg);
      return ADSP_EFAILED;
   }

   pNextToSource = (pStream->nodes[nodeNextToTxMtMxStRtrOrAfe]).pSvcHandle;

   if (NULL == pNextToSource)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:handle of node next to Tx matrix/stream router or AFE is not known yet",
            pSession->ucInternalSessionId,  stream_id);
      elite_msg_release_msg(&msg);
      return ADSP_EFAILED;
   }

   pConnectMsgPayload->phPort = pNextToSource;
   pConnectMsgPayload->bits_per_sample = pStream->bits_per_sample;
   pConnectMsgPayload->is_compressed = is_compressed;
   pConnectMsgPayload->stream_perf_mode = pStream->stream_perf_mode;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMe->pDeviceSvcCmdQ ,(uint64_t *)&msg);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:FAILED to connect ADM service!",
            pSession->ucInternalSessionId,  stream_id);
      elite_msg_release_msg(&msg);
      return result;
   }

   return result;
}

/**
 * This function sends on of the following
 * 1)Disconnect message to Rx matrix / Rx Stream Router
 * 2)Disconnect message to Tx matrix /Tx Stream Router
 * 3)Disable message to Tx matrix
 *
 * for AFE node, this function returns the dummy handle.
 */
ADSPResult AudioStreamMgr_SessionReturnDeviceHandle( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      bool_t TxDisconnectStep,
      uint8_t direction)
{
   elite_msg_any_t msg;
   uint32_t unSize;
   ADSPResult result;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   uint32_t token;
   ASM_GraphNodeIndex node_ind = ASM_INVALID_NODE;
   if (ADM_RX_DIR == direction)
   {
      node_ind = ASM_GetRxDeviceNodeInTopo(pStream);
   }
   else
   {
      node_ind = ASM_GetTxDeviceNodeInTopo(pStream);
   }

   // This function should not be called if:
   // 1) Node index is invalid
   // 2) Node index is ASM_NODE_RX_GENERIC dummy handle is NULL (ULL, ULLPP, and LLNP use cases).
   // 3) All other use cases where the service handle for this node is NULL; exceptions are for nodes ASM_NODE_RX_AFE & ASM_NODE_TX_AFE.
   if ( (node_ind == ASM_INVALID_NODE) ||
         ((node_ind == ASM_NODE_RX_GENERIC) && !pStream->generic_rx_dummy_handle) ||
         (((node_ind != ASM_NODE_RX_GENERIC) && (node_ind != ASM_NODE_TX_GENERIC)) && !pStream->nodes[node_ind].pSvcHandle))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Invalid node ind %u",
            pSession->ucInternalSessionId,  stream_id, node_ind);
      return ADSP_EFAILED;
   }

   bool_t is_compressed = (node_ind == ASM_NODE_RX_ST_RTR)||(node_ind == ASM_NODE_TX_ST_RTR);

   token = ASM_PopulateClientToken(stream_id, node_ind);

   unSize = sizeof(EliteMsg_CustomAdmStreamConnectType);
   if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token, 0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Fail to create msg",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   EliteMsg_CustomAdmStreamConnectType *pConnectMsgPayload = 
         (EliteMsg_CustomAdmStreamConnectType*) msg.pPayload;
   if (ASM_IsGaplessSession(pSession))
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pSession->pFirstGaplessStrm);
   }
   else
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pStream);
   }
   pConnectMsgPayload->direction = direction;
   pConnectMsgPayload->is_compressed = is_compressed;

   if (ADM_RX_DIR == direction )
   {
      pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_DISCONNECT;
   }  
   else
   {
      if (DISCONNECT_MATRIX == TxDisconnectStep)
      {
         //Step 1: Disconnect TX mastrix
         pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_DISCONNECT;
      }
      else if(DISABLE_MATRIX == TxDisconnectStep)
      {
         // Step 0: Disable port (Required before disconnection)
         pConnectMsgPayload->unSecOpCode =ELITEMSG_CUSTOM_ADM_STREAM_DISABLE;
      }
   } // disconnect from tx is two steps

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Disconnecting matrix/stream router. node %u, (disable/disconnect %u)",
         pSession->ucInternalSessionId,  stream_id, node_ind, TxDisconnectStep);

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pMe->pDeviceSvcCmdQ, (uint64_t*)&msg)))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to Disconnect Matrix/Stream Router service!",
            pSession->ucInternalSessionId,  stream_id);
   }

   return result;
}

/**
 * This function sends RUN, PAUSE  commands to the
 * matrix
 */
ADSPResult AudioStreamMgr_SessionRunPauseMatrixSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t  unSecOpcode,
      uint8_t   unDir)
{
   uint8_t stream_id         = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t result;

   elite_msg_any_t msg;

   if ( unSecOpcode != ELITEMSG_CUSTOM_MT_MX_PAUSE
         && unSecOpcode != ELITEMSG_CUSTOM_MT_MX_RUN)
   {
      return ADSP_EBADPARAM;
   }

   uint32_t ulClientToken = 0;
   elite_svc_handle_t *pMtMxSvcHandle = NULL ;
   uint8_t direction = 0;

   if(ASM_TX_DIR == unDir)
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_MTX);
      direction = ADM_TX_DIR;
   }
   else
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_RX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_MTX);
      direction = ADM_RX_DIR;
   }

   switch (unSecOpcode) {

   case ELITEMSG_CUSTOM_MT_MX_PAUSE:
   {
      uint32_t unPayloadSize = sizeof(EliteMsg_CustomMtMxPauseFlushType);

      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {

         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Fail to create mesage for pause/flush to MtMx",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      // setup payload
      EliteMsg_CustomMtMxPauseFlushType *pPayload = (EliteMsg_CustomMtMxPauseFlushType *) (msg.pPayload);
      pPayload->unSecOpCode = unSecOpcode;
      if(ADM_RX_DIR == direction)
      {
         pPayload->unPortID = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
         pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
      }
      else
      {
         pPayload->unPortID = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
         pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_TX_DIR].unPortDirection;
      }

         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AudioStreamMgr [%u,%u]: Pausing Matrix svc 0x%lx",
               pSession->ucInternalSessionId,  stream_id, ulClientToken);
      break;
   }
   case ELITEMSG_CUSTOM_MT_MX_RUN:
   {
      uint32_t unPayloadSize = sizeof(EliteMsg_CustomMtMxRunType);

      if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage for RUN to MtMx",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      // setup payload
      asm_session_cmd_run_v2_t *pParamPayload;
      pParamPayload = &pSession->prev_run_cmd;


      uint64_t ullStartTime = (uint64_t )pParamPayload->time_msw<<32;
      ullStartTime = ullStartTime | (pParamPayload->time_lsw);

      ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unStartFlag = pParamPayload->flags;
      ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->ullStartTime = ullStartTime;

      ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unSecOpCode =  unSecOpcode;
      if(ADM_RX_DIR == direction)
      {
         ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unPortID =  pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
         ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unPortDirection =  pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
      }
      else
      {
         ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unPortID =  pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
         ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unPortDirection =  pStream->mtMxPortParam[ASM_TX_DIR].unPortDirection;

         /* for loopback mode start the Tx matrix immediately (Rx matrix starts as specified by client). */
         if (ASM_IsAsmLoopback(pSession->unMode))
         {
            ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->unStartFlag = ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE;
            ((EliteMsg_CustomMtMxRunType *) msg.pPayload)->ullStartTime = 0;
         }
      }

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Running Matrix svc 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);

      break;
   }
   }// switch (unSecOpcode)

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMtMxSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to send run command to matrx service!",
            pSession->ucInternalSessionId,  stream_id);
   }

   return result;

}

ADSPResult AudioStreamMgr_StreamFlushMatrixSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t  unSecOpcode,
      uint8_t   unDir)
{
   uint8_t stream_id         = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t result;

   elite_msg_any_t msg;

   if(unSecOpcode != ELITEMSG_CUSTOM_MT_MX_FLUSH)
   {
      return ADSP_EBADPARAM;
   }

   uint32_t ulClientToken = 0;
   elite_svc_handle_t *pMtMxSvcHandle = NULL ;
   uint8_t direction = 0;

   if(ASM_TX_DIR == unDir)
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_MTX);
      direction = ADM_TX_DIR;
   }
   else
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_RX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_MTX);
      direction = ADM_RX_DIR;
   }

   uint32_t unPayloadSize = sizeof(EliteMsg_CustomMtMxPauseFlushType);

   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create mesage for pause/flush to MtMx",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   // setup payload
   EliteMsg_CustomMtMxPauseFlushType *pPayload = (EliteMsg_CustomMtMxPauseFlushType *) (msg.pPayload);
   pPayload->unSecOpCode = unSecOpcode;
   if(ADM_RX_DIR == direction)
   {
      pPayload->unPortID = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
      pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
   }
   else
   {
      pPayload->unPortID = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
      pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_TX_DIR].unPortDirection;
   }


   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Flushing Matrix svc 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMtMxSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to send run command to matrx service!",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   return result;

}

/**
 * This function sends Suspend command to the matrix
 */
ADSPResult AudioStreamMgr_SessionSuspendMatrixSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t  unSecOpcode,
      uint8_t   unDir)
{
   uint8_t stream_id         = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t result;
   elite_msg_any_t msg;
   uint32_t ulClientToken = 0;
   elite_svc_handle_t *pMtMxSvcHandle = NULL ;
   uint8_t direction = 0;
   uint32_t unPayloadSize = sizeof(EliteMsg_CustomMtMxPauseFlushType);

   if ( unSecOpcode != ELITEMSG_CUSTOM_MT_MX_SUSPEND)
   {
      return ADSP_EBADPARAM;
   }

   if(ASM_TX_DIR == unDir)
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_MTX);
      direction = ADM_TX_DIR;
   }
   else
   {
      pMtMxSvcHandle = (pStream->nodes[ASM_NODE_RX_MTX]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_MTX);
      direction = ADM_RX_DIR;
   }

   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create message for suspend to MtMx",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }
   // setup payload
   EliteMsg_CustomMtMxPauseFlushType *pPayload = (EliteMsg_CustomMtMxPauseFlushType *) (msg.pPayload);
   pPayload->unSecOpCode = unSecOpcode;

   if(ADM_RX_DIR == direction)
   {
      pPayload->unPortID = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
      pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
   }
   else
   {
      pPayload->unPortID = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
      pPayload->unPortDirection =  pStream->mtMxPortParam[ASM_TX_DIR].unPortDirection;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Suspending Matrix svc resources 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMtMxSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to send Suspend command to matrix service!",
            pSession->ucInternalSessionId,  stream_id);
   }

   return result;

}

/**
 * This function sends RUN, PAUSE and FLUSH commands to the
 * Stream Router
 */
ADSPResult AudioStreamMgr_SessionRunPauseStreamRouterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pStreamRouterSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg;
   uint32_t result;
   uint8_t stream_id   = pStream->ucStreamId;
   uint32_t port_id;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( unOpcode != ELITE_CMD_PAUSE
         && unOpcode != ELITE_CMD_RUN
         && unOpcode != ELITE_CMD_FLUSH )
   {
      return ADSP_EBADPARAM;
   }

   if(ASM_RX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_RX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
   }
   else if(ASM_TX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_TX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
   }
   else
   {
      return ADSP_EBADPARAM;
   }


   if((ELITE_CMD_PAUSE == unOpcode) || (ELITE_CMD_FLUSH == unOpcode))
   {
      // Pause/Flush have the same payload
      uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterPauseFlush);

      result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0);

      if(ADSP_FAILED( result ))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      EliteMsg_CustomStreamRouterPauseFlush *pPauseFlushMsg = (EliteMsg_CustomStreamRouterPauseFlush *)msg.pPayload;
      if(ELITE_CMD_PAUSE == unOpcode)
      {
         pPauseFlushMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_PAUSE;
      }
      else
      {
         pPauseFlushMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_FLUSH;
      }
      pPauseFlushMsg->port_id = port_id;

      result = ASM_PushCmdToPeerSvc(pMe, pSession, pStreamRouterSvcHandle->cmdQ, (uint64_t*)&msg);

      if(ADSP_FAILED(result))
      {
         //return the mesage to buf queue
         elite_msg_release_msg(&msg);
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
               pSession->ucInternalSessionId,  stream_id, unOpcode);
         return result;
      }

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pausing/Flushing Stream Router. Opcode = 0x%lx",
            pSession->ucInternalSessionId,  stream_id, unOpcode);

   }
   else //RUN
   {
      uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterRun);

      if (ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage for RUN to Stream Router",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      // setup payload
      asm_session_cmd_run_v2_t *pParamPayload;
      pParamPayload = &pSession->prev_run_cmd;
      EliteMsg_CustomStreamRouterRun *run_msg_ptr = (EliteMsg_CustomStreamRouterRun *)msg.pPayload;

      uint64_t ullStartTime = (uint64_t )pParamPayload->time_msw<<32;
      ullStartTime = ullStartTime | (pParamPayload->time_lsw);

      run_msg_ptr->unSecOpCode =  ELITEMSG_CUSTOM_STREAM_ROUTER_RUN;
      run_msg_ptr->port_id     = port_id;
      run_msg_ptr->llStartTime = ullStartTime;
      run_msg_ptr->unStartFlag = pParamPayload->flags & ASM_BIT_MASK_RUN_START_TIME;

      /* for loopback mode start the Tx matrix immediately (Rx matrix starts as specified by client). */
      if ((ASM_TX_DIR == unDir) && ASM_IsAsmLoopback(pSession->unMode))
      {
         run_msg_ptr->llStartTime = 0;
         run_msg_ptr->unStartFlag = ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE;
      }

      result = ASM_PushCmdToPeerSvc(pMe, pSession, pStreamRouterSvcHandle->cmdQ, (uint64_t*)&msg);

      if(ADSP_FAILED(result))
      {
         //return the mesage to buf queue
         elite_msg_release_msg(&msg);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send run message to Stream router",
               pSession->ucInternalSessionId,  stream_id);
         return result;
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Running Stream svc",
            pSession->ucInternalSessionId,  stream_id);
   }

   return result;
}

/**
 * This function sends RUN, PAUSE and FLUSH commands to the
 * Stream Router
 */
ADSPResult AudioStreamMgr_StreamFlushStreamRouterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pStreamRouterSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg;
   uint32_t result;
   uint8_t stream_id   = pStream->ucStreamId;
   uint32_t port_id;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( unOpcode != ELITE_CMD_FLUSH )
   {
      return ADSP_EBADPARAM;
   }

   if(ASM_RX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_RX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
   }
   else if(ASM_TX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_TX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
   }
   else
   {
      return ADSP_EBADPARAM;
   }


   // Pause/Flush have the same payload
   uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterPauseFlush);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);

   if(ADSP_FAILED( result ))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   EliteMsg_CustomStreamRouterPauseFlush *pPauseFlushMsg = (EliteMsg_CustomStreamRouterPauseFlush *)msg.pPayload;
   if(ELITE_CMD_PAUSE == unOpcode)
   {
      pPauseFlushMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_PAUSE;
   }
   else
   {
      pPauseFlushMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_FLUSH;
   }
   pPauseFlushMsg->port_id = port_id;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pStreamRouterSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pausing/Flushing Stream Router. Opcode = 0x%lx",
         pSession->ucInternalSessionId,  stream_id, unOpcode);

   return result;
}

/**
 * This function sends suspend commands to the
 * Stream Router
 */
ADSPResult AudioStreamMgr_SuspendStreamRouterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pStreamRouterSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg;
   uint32_t result;
   uint8_t stream_id   = pStream->ucStreamId;
   uint32_t port_id;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if(ASM_RX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_RX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
   }
   else if(ASM_TX_DIR == unDir)
   {
      pStreamRouterSvcHandle = (pStream->nodes[ASM_NODE_TX_ST_RTR]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_ST_RTR);
      port_id = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
   }
   else
   {
      return ADSP_EBADPARAM;
   }

   // Pause/Flush have the same payload
   uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterSuspend);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);

   if(ADSP_FAILED( result ))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   EliteMsg_CustomStreamRouterSuspend *pSuspendMsg = (EliteMsg_CustomStreamRouterSuspend *)msg.pPayload;
   pSuspendMsg->unSecOpCode = unOpcode;
   pSuspendMsg->port_id = port_id;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pStreamRouterSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: suspending Stream Router. Opcode = 0x%lx",
         pSession->ucInternalSessionId,  stream_id, unOpcode);

   return result;
}
/**
 * This function sends command to register matrix flow events
 */
ADSPResult AudioStreamMgr_SessionRegisterMatrixFlowEvent(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      asm_session_cmd_reg_tx_overflow_t *pEventCfg,
      uint32_t direction)
{

   elite_svc_handle_t *pMtMxSvcHandle = NULL ;
   uint32_t result;
   elite_msg_any_t msg;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t ulClientToken = 0;
   switch(direction)
   {
   case ASM_RX_DIR:
   {
      pMtMxSvcHandle = pStream->nodes[ASM_NODE_RX_MTX].pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_RX_MTX);
      break;
   }
   case ASM_TX_DIR:
   {
      pMtMxSvcHandle = pStream->nodes[ASM_NODE_TX_MTX].pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_MTX);
      break;
   }
   default:
   {
      //Not valid for NT mode and Loopback mode
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Ivalid mode for registering matrix flow event",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      return ADSP_EUNEXPECTED;
   }
   }

   uint32_t unPayloadSize = sizeof(EliteMsg_CustomMtMxUnderOverFlowEventType);

   if ( ADSP_FAILED( result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {

      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Fail to create mesage for flow event to MtMx",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      return result;
   }

   // setup payload
   EliteMsg_CustomMtMxUnderOverFlowEventType *pPayload = (EliteMsg_CustomMtMxUnderOverFlowEventType *) (msg.pPayload);
   pPayload->unSecOpCode      = ELITEMSG_CUSTOM_MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT;
   if(ASM_RX_DIR == direction)
   {
      pPayload->unPortID         = pStream->mtMxPortParam[ASM_RX_DIR].unPortID;
      pPayload->unPortDirection  = pStream->mtMxPortParam[ASM_RX_DIR].unPortDirection;
   }
   else
   {
      pPayload->unPortID         = pStream->mtMxPortParam[ASM_TX_DIR].unPortID;
      pPayload->unPortDirection  = pStream->mtMxPortParam[ASM_TX_DIR].unPortDirection;
   }

   pPayload->usEnable         = pEventCfg->enable_flag;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pMtMxSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to send register flow event command to matrx service!",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
   }

   return result;

}

/**
 * This function is used to diable/disconnect TX matrix and wait
 * for ACK
 */
ADSPResult AudioStreamMgr_DisconnectTxMatrixAndWaitForAck( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      bool_t disconnect_matrix,
      uint8_t stream_id
)
{
   elite_msg_any_t msg;
   uint32_t unSize;
   ADSPResult result;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t token =0;

   token = ASM_PopulateClientToken(stream_id, ASM_NODE_TX_MTX);

   unSize = sizeof(EliteMsg_CustomAdmStreamConnectType);
   if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token, 0))) {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Fail to create msg",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid Stream ID",pSession->ucInternalSessionId, stream_id);
      return ADSP_EFAILED;
   }

   EliteMsg_CustomAdmStreamConnectType *pConnectMsgPayload =
         (EliteMsg_CustomAdmStreamConnectType*) msg.pPayload;
   if (ASM_IsGaplessSession(pSession))
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pSession->pFirstGaplessStrm);
   }
   else
   {
	   pConnectMsgPayload->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pStream);
   }
   pConnectMsgPayload->direction = ADM_TX_DIR;

   if (DISCONNECT_MATRIX == disconnect_matrix)
   {
      //Step 1: Disconnect TX matrix
      pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_DISCONNECT;
   }
   else if (DISABLE_MATRIX == disconnect_matrix)
   {
      // Step 0: Disable port (Required before disconnection)
      pConnectMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_DISABLE;
   }

   qurt_elite_queue_t *pDisconnectCmdQ = pMe->pDeviceSvcCmdQ;

   // Attention should push to the queue associated with port
   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pDisconnectCmdQ, (uint64_t*)&msg)))
   {
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to Disconnect matrx service!",
            pSession->ucInternalSessionId,  stream_id);

      return result;
   }

   result = elite_svc_wait_for_ack(&msg);

   ASM_OnPopFromRespQ(pMe, pSession);

   elite_msg_release_msg(&msg);

   if ( ADSP_FAILED( result ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to Disconnect matrx service!",
            pSession->ucInternalSessionId,  stream_id);
   }

   return result;

}

/*======================================================================

FUNCTION      AudioStreamMgr_SessionGetDevicePathDelay

DESCRIPTION
Processes ASM_SESSION_CMD_GET_PATH_DELAY Apr command. Sends the custom message to ADM
to get the device side delay.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_StreamGetDevicePathDelay(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint32_t direction, ASM_GraphNodeIndex index, bool_t is_compressed)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   /**
    * for gapless, do this only for firstOpenStream, for any other stream ignore.
    */
   if (ASM_IsGaplessSession(pSession) &&
         (pStream != pSession->pFirstGaplessStrm))
   {
      return ADSP_EOK;
   }

   //send connect command to ADM to get the device path delay
   elite_msg_any_t msg;
   uint32_t unPayloadSize = sizeof(EliteMsg_CustomAdmGetDevicePathDelayCmd);
   uint32_t token = ASM_PopulateClientToken(pStream->ucStreamId, index);

   if (ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create custom mesage ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY",
            pSession->ucInternalSessionId,
            pStream->ucStreamId );
      return result;
   }

   EliteMsg_CustomAdmGetDevicePathDelayCmd *pAdmGetDevicePathDelayCmd = (EliteMsg_CustomAdmGetDevicePathDelayCmd *)msg.pPayload;
   pAdmGetDevicePathDelayCmd->unSecOpCode = unOpcode;
   if (ASM_IsGaplessSession(pSession))
   {
	   pAdmGetDevicePathDelayCmd->session_id = PORT_FROM_SESSION_STREAM(pSession, pSession->pFirstGaplessStrm);
   }
   else
   {
	   pAdmGetDevicePathDelayCmd->session_id = PORT_FROM_SESSION_STREAM(pSession, pStream);
   }
   pAdmGetDevicePathDelayCmd->direction = direction;
   pAdmGetDevicePathDelayCmd->is_compressed = is_compressed;
   pAdmGetDevicePathDelayCmd->device_path_buf_delay = 0;
   pAdmGetDevicePathDelayCmd->device_path_alg_delay = 0;
   pAdmGetDevicePathDelayCmd->unClientToken = ASM_PopulateClientToken(pStream->ucStreamId, index);

   //push message into ADM cmd Q
   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pMe->pDeviceSvcCmdQ, (uint64_t*)&msg)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY",
            pSession->ucInternalSessionId,
            pStream->ucStreamId);
      elite_msg_release_msg(&msg);
      return result;
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      AudioStreamMgr_SessionGetDevicePathDelayMtMx

DESCRIPTION
Processes ASM_SESSION_CMD_GET_PATH_DELAY Apr command. Sends the custom message to ADM
to get the device side delay.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_StreamGetDevicePathDelayUncompressed(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,    AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t direction)
{
   if (ASM_RX_DIR == direction)
   {
      return AudioStreamMgr_StreamGetDevicePathDelay(pMe, pSession, pStream, unOpcode, direction, ASM_NODE_RX_MTX, FALSE);
   }
   else
   {
      return AudioStreamMgr_StreamGetDevicePathDelay(pMe, pSession, pStream, unOpcode, direction, ASM_NODE_TX_MTX, FALSE);
   }
}
/*======================================================================

FUNCTION      AudioStreamMgr_SessionGetDevicePathDelayStRtr

DESCRIPTION
Processes ASM_SESSION_CMD_GET_PATH_DELAY Apr command. Sends the custom message to ADM
to get the device side delay.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_StreamGetDevicePathDelayCompressed(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,    AudioStreamMgr_StreamStateType *pStream,  uint32_t unOpcode,      uint8_t direction)
{
   if (ASM_RX_DIR == direction)
   {
      return AudioStreamMgr_StreamGetDevicePathDelay(pMe, pSession, pStream, unOpcode, direction, ASM_NODE_RX_ST_RTR, TRUE);
   }
   else
   {
      return AudioStreamMgr_StreamGetDevicePathDelay(pMe, pSession, pStream, unOpcode, direction, ASM_NODE_TX_ST_RTR, TRUE);
   }
}

static ADSPResult AudioStreamMgr_FindRxStreamSvcAndGenericDevNodes(AudioStreamMgr_SessionStateType *pSession,
                                                                 AudioStreamMgr_StreamStateType *pStream,
                                                                 ASM_GraphNodeIndex *stream_svc_node,
                                                                 ASM_GraphNodeIndex *dev_svc_node)
{
   QURT_ELITE_ASSERT(pSession);
   QURT_ELITE_ASSERT(pStream);
   if ((NULL == stream_svc_node) || (NULL == dev_svc_node))
   {
      return ADSP_EBADPARAM;
   }

   // Connections only happen in the Rx direction, reject other types.
   if ((pSession->unMode != ASM_RX_MODE) && (pSession->unMode != ASM_RX_PULL_MODE))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Error. Received invalid request to connect a Tx session. Session mode: %lu",
            pSession->ucInternalSessionId,  pStream->ucStreamId, pSession->unMode);
      return ADSP_EBADPARAM;
   }

   // Find the ASM service that is closest to the device and the generic device node indices
   *stream_svc_node = ASM_NODE_MAX;
   *dev_svc_node = ASM_NODE_MAX;
   for (int32_t i = AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM-1 ; i >= 0; i--)
   {
      // Search for asm service node - either the PP or Decoder
      if (((pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_PP)
            || (pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_DEC))
          && (ASM_NODE_MAX == *stream_svc_node))
      {
         *stream_svc_node = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Found stream svc node %lu at index %lu",
               pSession->ucInternalSessionId,  pStream->ucStreamId, *stream_svc_node, i);
      }

      // Search for the generic device node to use as the downstream device
      if ((pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_RX_GENERIC)
            && (ASM_NODE_MAX == *dev_svc_node))
      {
         *dev_svc_node = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: Found downstream device node %lu at index %lu",
               pSession->ucInternalSessionId,  pStream->ucStreamId, *dev_svc_node, i);
      }
   }

   if ((ASM_NODE_MAX == *stream_svc_node) || (ASM_NODE_MAX == *dev_svc_node))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Could not find required nodes! Upstream index %lu, device index %lu",
            pSession->ucInternalSessionId,  pStream->ucStreamId, *stream_svc_node, *dev_svc_node);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

/*======================================================================

FUNCTION      AudioStreamMgr_ProcessStreamDirectConnect

DESCRIPTION
This function processes the custom disconnect command to connect the ASM stream with
a downstream device handle. This is used by ULL, LLNP, and ULLPP modes.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_ProcessStreamDirectConnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EFAILED;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   EliteMsg_CustomASMStreamDirectConnectType *pPayload = (EliteMsg_CustomASMStreamDirectConnectType *)pkt.pPayload;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pPayload->unStreamID);

   //Check if the session is ULL, ULLPP, or LLNP session. Do not accept this command in session close state.
   if ( !pStream || ((AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) ||
         ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION != pStream->stream_perf_mode) &&
          (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION != pStream->stream_perf_mode) &&
               (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pStream->stream_perf_mode))))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Received connect cmd 0x%lx to invalid session. Session state: %u, stream_perf_mode: %u",
            pSession->ucInternalSessionId,  (pStream?pStream->ucStreamId:0), pPayload->unSecOpCode,
            pSession->unState, (pStream?pStream->stream_perf_mode:0));
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Received cmd 0x%lx to directly connect to downstream device 0x%lx",
         pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode, pPayload->phDownstreamDevice);

   // Find the last stream service node before the device and the device service node
   ASM_GraphNodeIndex stream_svc_node_ind;
   ASM_GraphNodeIndex device_svc_node_ind;
   result = AudioStreamMgr_FindRxStreamSvcAndGenericDevNodes(pSession, pStream, &stream_svc_node_ind, &device_svc_node_ind);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Failed to query for asm and device svc nodes",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      elite_svc_send_ack(&pkt, result);
      return result;
   }

   if (NULL == ASM_ConnectDynaSvcFTable[stream_svc_node_ind].func_ptr)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: NULL connection function ptr for svc ind %lu",
            pSession->ucInternalSessionId, pStream->ucStreamId, stream_svc_node_ind);
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   // Store the given device handle from the payload in the generic device service handle.
   (pStream->nodes[ASM_NODE_RX_GENERIC]).pSvcHandle = pPayload->phDownstreamDevice;

   // Connect the last service in the topo to the device
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: connecting stream svc [0x%lx,0x%lx] to device 0x%lx",
         pSession->ucInternalSessionId,   pStream->ucStreamId,
         pStream->nodes[stream_svc_node_ind].pSvcHandle, pStream->nodes[stream_svc_node_ind].threadId,
         pStream->nodes[device_svc_node_ind].pSvcHandle);

   result = ASM_ConnectDynaSvcFTable[stream_svc_node_ind].func_ptr(pMe, pSession, pStream, stream_svc_node_ind, device_svc_node_ind, TRUE);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: connection failed!",
            pSession->ucInternalSessionId, pStream->ucStreamId);
      // Store the given device handle from the payload in the generic device service handle.
      (pStream->nodes[ASM_NODE_RX_GENERIC]).pSvcHandle = NULL;
      return result;
   }

   return result;
}

/*======================================================================

FUNCTION      AudioStreamMgr_ProcessStreamDirectDisconnect

DESCRIPTION
This function processes the custom disconnect command to connect the ASM stream with
a downstream device handle. This is used by ULL, LLNP, and ULLPP modes.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_ProcessStreamDirectDisconnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   EliteMsg_CustomASMStreamDirectConnectType *pPayload = (EliteMsg_CustomASMStreamDirectConnectType *)pkt.pPayload;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pPayload->unStreamID);

   if (!pStream)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: Disconnect rcvd NULL strm ptr");
      elite_svc_send_ack(&pkt, ADSP_EBADPARAM);
      return ADSP_EBADPARAM;
   }

   // TODO: change this check to test against ASM_NODE_RX_GENERIC or TX_GENERIC instead of mode.
   if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION != pStream->stream_perf_mode) &&
       (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pStream->stream_perf_mode) &&
       (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION != pStream->stream_perf_mode))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Received cmd 0x%lx to directly disconnect an invalid stream type %u.",
            pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode, pStream->stream_perf_mode);
      elite_svc_send_ack(&pkt, ADSP_EUNEXPECTED);
      return ADSP_EUNEXPECTED;
   }

   //do not accept this command in close state.
   if (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Received disconnect cmd 0x%lx to disconnect while in close state.",
            pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode);
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      //make downstream device handle NULL because, otherwise, we will use this to disconnect the decoder.
      (pStream->nodes[ASM_NODE_RX_GENERIC]).pSvcHandle = NULL;
      return ADSP_ENOTREADY;
   }

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Received cmd 0x%lx to directly disconnect from downstream device 0x%lx",
         pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode, pPayload->phDownstreamDevice);

   ASM_GraphNodeIndex stream_svc_node_ind;
   ASM_GraphNodeIndex device_svc_node_ind;
   result = AudioStreamMgr_FindRxStreamSvcAndGenericDevNodes(pSession, pStream, &stream_svc_node_ind, &device_svc_node_ind);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Failed to query for asm and device svc nodes",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      elite_svc_send_ack(&pkt, result);
      return result;
   }

   //send disconnect command to decoder service
   // Disconnect from decoder service and AFE
   uint32_t unPayloadSize = sizeof( elite_msg_cmd_connect_t);
   uint32_t token = ASM_PopulateClientTokenForCustomCmd(pStream->ucStreamId, stream_svc_node_ind);
   elite_msg_any_t msg;

   result = elite_msg_create_msg(&msg, &unPayloadSize,
         ELITE_CMD_DISCONNECT,
         pStSession->pSessionRespQ,
         token,
            0);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      elite_svc_send_ack(&pkt, result);
      return result;
   }

   elite_svc_handle_t *pCurSvcHandle = (pStream->nodes[stream_svc_node_ind]).pSvcHandle;
   elite_svc_handle_t *pNextSvcHandle = (pStream->nodes[device_svc_node_ind]).pSvcHandle;

   ((elite_msg_cmd_connect_t *) msg.pPayload)->pSvcHandle = pNextSvcHandle;

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: disconnecting stream svc [0x%lx,0x%lx] from dev svc 0x%lx",
         pSession->ucInternalSessionId,  pStream->ucStreamId,
         pCurSvcHandle->unSvcId,  (uint32_t) (pCurSvcHandle->threadId),
         pNextSvcHandle->unSvcId);

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pCurSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: fail to disconnect stream svc [0x%lx,0x%lx] from dev svc 0x%lx",
            pSession->ucInternalSessionId,  pStream->ucStreamId,
            pCurSvcHandle->unSvcId,         (uint32_t) (pCurSvcHandle->threadId),
            pNextSvcHandle->unSvcId);
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      elite_svc_send_ack(&pkt, result);
      return result;
   }

  // NULL the downstream device handle so that we do not try to disconnect during AudioStreamMgr_StreamDisConnectGraph
  (pStream->nodes[device_svc_node_ind]).pSvcHandle = NULL;

   return ADSP_EFAILED;
}

/*======================================================================

FUNCTION      AudioStreamMgr_FindTxSinkGraphNode

DESCRIPTION
This function finds the node index of the service that receives data on the
ASM side (the Tx Sink).

DEPENDENCIES  none

RETURN VALUE  Node index for the service in the topo or ASM_INVALID_INDEX.

SIDE EFFECTS  none

======================================================================*/
static ASM_GraphNode* AudioStreamMgr_FindTxSinkNode(AudioStreamMgr_StreamStateType *pStream)
{
   if (NULL == pStream)
   {
      return NULL;
   }

   // Iterate through StrmSvcInfo to find the first matrix, stream router, or afe node.
   // Once the device node is found, find the first non-null node
   // Essentially, this is a generic way to find the handle to the PP or encoder.
   for (uint32_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if ((ASM_NODE_TX_MTX == pStream->pTopo->StrmSvcInfo.svc_nodes[i]) ||
          (ASM_NODE_TX_ST_RTR == pStream->pTopo->StrmSvcInfo.svc_nodes[i]) ||
          (ASM_NODE_TX_GENERIC == pStream->pTopo->StrmSvcInfo.svc_nodes[i]))
      {
         for (uint32_t j = ++i; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
         {
            if (ASM_INVALID_NODE != pStream->pTopo->StrmSvcInfo.svc_nodes[j])
            {
               return &(pStream->nodes[pStream->pTopo->StrmSvcInfo.svc_nodes[i]]);
            }
         }
      }
   }

   // If nothing is found, return NULL.
   return NULL;
}

/*======================================================================

FUNCTION      AudioStreamMgr_ProcessQueryTxSinkHandle

DESCRIPTION
This function handles the query for the sink service handle in the Tx path.
This is only used by LLNP Tx sessions to provide the AFE with the downstream
service handle.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_ProcessQueryTxSinkHandle(AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession)
{
   if ((NULL == pMe) || (NULL == pSession))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr_ProcessQueryTxSinkHandle rcvd bad params");
      return ADSP_EBADPARAM;
   }

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t)(pStSession->cmdMsg);
   EliteMsg_CustomAsmCommandQueryTxSinkHandle *pPayload =
         (EliteMsg_CustomAsmCommandQueryTxSinkHandle *)pkt.pPayload;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pPayload->unStreamID);
   if (NULL == pStream)
   {

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "AudioStreamMgr_ProcessQueryTxSinkHandle failed to retrieve stream instance");
      pPayload->phSinkHandle = NULL;
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Received query for tx sink service handle cmd 0x%lx",
         pSession->ucInternalSessionId, pStream->ucStreamId, pPayload->unSecOpCode);

   // do not accept this command in close state.
   if (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Cannot process cmd 0x%lx in closed state.",
            pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode);
      pPayload->phSinkHandle = NULL;
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }

   // this command is accepted only on LLNP Tx sessions
   if (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pStream->stream_perf_mode)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Received cmd 0x%lx on non-LLNP Tx session. Perf mode [0x%x].",
            pSession->ucInternalSessionId, pStream->ucStreamId, pPayload->unSecOpCode,
            pSession->unMode);
      pPayload->phSinkHandle = NULL;
      elite_svc_send_ack(&pkt, ADSP_EUNEXPECTED);
      return ADSP_EUNEXPECTED;
   }

   // Get the service handle to the sink
   pPayload->phSinkHandle = NULL;
   ASM_GraphNode *node = AudioStreamMgr_FindTxSinkNode(pStream);
   if (NULL != node)
   {
      pPayload->phSinkHandle = node->pSvcHandle;
   }

   elite_svc_send_ack(&pkt, ADSP_EOK);
   return ADSP_EOK;
}


/*======================================================================

FUNCTION      AudioStreamMgr_ProcessPrepareForDisconnect

DESCRIPTION
This function sends a message to the encoder service to prepare for
disconnection from upstream service. The encoder is expected to return
its buffers to the upstream service and send any pending output data
to the client.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_ProcessPrepareForDisconnect(AudioStreamMgr_t *pMe,
                                                      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result;
   if ((NULL == pMe) || (NULL == pSession))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr_ProcessPrepareForDisconnect rcvd bad params");
      return ADSP_EBADPARAM;
   }

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t)(pStSession->cmdMsg);
   EliteMsg_CustomAsmCommandPrepDisconnect *pPayload =
         (EliteMsg_CustomAsmCommandPrepDisconnect *)pkt.pPayload;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pPayload->unStreamID);
   if (NULL == pStream)
   {

      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "AudioStreamMgr_ProcessPrepareForDisconnect failed to retrieve stream instance");
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: Received prepare for disconnect cmd 0x%lx for LLNP Tx session",
         pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode);

   // Do not accept this command in session close state.
   // This command only applies for record streams
   // Check if the session is LLNP reject all others
   if ( (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) ||
        (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pStream->stream_perf_mode))
   {
      MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Received prepare diconnect cmd 0x%lx to invalid session. "
            "Session state: %u, Session Mode: %d, stream_perf_mode: %u",
            pSession->ucInternalSessionId,  pStream->ucStreamId, pPayload->unSecOpCode,
            pSession->unState, pSession->unMode, pStream->stream_perf_mode);
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }

   // For LLNP Tx find the reference to the first sink
   elite_svc_handle_t *pCurSvcHandle = NULL;
   ASM_GraphNode *node = AudioStreamMgr_FindTxSinkNode(pStream);
   if (NULL == node)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Couldn't find the sink node for LLNP Tx session",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }
   pCurSvcHandle = node->pSvcHandle;

   // Create the payload packet
   uint32_t unPayloadSize = sizeof(elite_msg_custom_header_t);
   uint32_t token = ASM_PopulateClientToken(pStream->ucStreamId, node->index);
   elite_msg_any_t msg;
   result = elite_msg_create_msg(&msg,
                                 &unPayloadSize,
                                 ELITE_CUSTOM_MSG,
                                 pStSession->pSessionRespQ,
                                 token,
                                 0);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to create message",
            pSession->ucInternalSessionId, pStream->ucStreamId);
      elite_svc_send_ack(&pkt, result);
      return result;
   }
   ((elite_msg_custom_header_t *)msg.pPayload)->unSecOpCode = ELITEMSG_CUSTOM_ENC_PREPARE_DISCONNECT;

   // Send the payload to the service
   result = ASM_PushCmdToPeerSvc(pMe, pSession, pCurSvcHandle->cmdQ, (uint64_t*)&msg);
   if (ADSP_FAILED(result))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to prepare Enc Svc [0x%lx,0x%lx] for disconnect!",
            pSession->ucInternalSessionId, pStream->ucStreamId,
            pCurSvcHandle->unSvcId, (uint32_t ) (pCurSvcHandle->threadId));
      //return the created msg to buffer return queue
      elite_msg_release_msg(&msg);
      elite_svc_send_ack(&pkt, result);
      return result;
   }

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_StreamFlushAFE(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t  unSecOpcode,
      uint8_t   unDir)
{
   // Nothing to do here
   (void)pMe;
   (void)pSession;
   (void)pStream;
   (void)unSecOpcode;
   (void)unDir;

   return ADSP_EOK;
}
