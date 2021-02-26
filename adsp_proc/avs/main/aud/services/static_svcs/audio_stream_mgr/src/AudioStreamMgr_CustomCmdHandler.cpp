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
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "AudioStreamMgr_CustomCmdHandler.h"
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

/* =======================================================================
 **                          Function Declarations
 ** ======================================================================= */
static ADSPResult AudioStreamMgr_ProcessSetPPOutput(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession);

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/* This function process the Custom command that are send to a given session. */
ADSPResult AudioStreamMgr_SessionCmdQCustomMsgHandler( AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[intSessionId];

   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t *) (pStSession->cmdMsg.pPayload);

   /** Sanity check */
   if ( (ELITE_CUSTOM_MSG != pStSession->cmdMsg.unOpCode) || (!pPayload) )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u, -]: invalid opcode/payload 0x%lx,0x%lx",
            intSessionId, pStSession->cmdMsg.unOpCode,  (uint32_t) pPayload );
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   AudioStreamMgr_SessionStateType *pSession = pMe->pSessions[intSessionId];
   //session may be null.
   if (NULL == pSession )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u, -]: invalid session received a custom message",
            intSessionId);
      elite_svc_send_ack(&pkt, ADSP_EUNSUPPORTED);
      return ADSP_EUNSUPPORTED;
   }

   pSession->unReqResps = 0;
   pSession->nOverallResult = ADSP_EOK;

   switch ( pPayload->unSecOpCode )
   {
   case ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT_SAMPLE_RATE:
   {
      return AudioStreamMgr_ProcessSetPPOutputSamplingRate(pMe,pSession);
   }
   case ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_CONNECT:
   {
      return AudioStreamMgr_ProcessStreamDirectConnect(pMe, pSession);
   }
   case ELITEMSG_CUSTOM_ASM_STREAM_DIRECT_DISCONNECT:
   {
      return AudioStreamMgr_ProcessStreamDirectDisconnect(pMe,pSession);
   }
   case ELITEMSG_CUSTOM_ASM_LLNP_STREAM_QUERY_TX_SINK_HANDLE:
   {
      return AudioStreamMgr_ProcessQueryTxSinkHandle(pMe, pSession);
   }
   case ELITEMSG_CUSTOM_ASM_LLNP_PREPARE_FOR_DISCONNECT:
   {
      return AudioStreamMgr_ProcessPrepareForDisconnect(pMe, pSession);
   }
   case ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT:
   {
      return AudioStreamMgr_ProcessSetPPOutput(pMe, pSession);
   }
   default:
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: unsupported custom command 0x%lx",
            pSession->ucInternalSessionId,  pPayload->unSecOpCode);

      // Generate ACK
      elite_svc_send_ack(&pkt, ADSP_EUNSUPPORTED);
   }
   }

   return ADSP_EOK;
}

/*This function process the custom PP output sampling rate configuration command.
 * This is a custom message as here, stream ID is not known correctly. however, since the PP receives
 * only one stream even in gapless, stream id is immaterial. */
ADSPResult AudioStreamMgr_ProcessSetPPOutputSamplingRate(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult status;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   EliteMsg_CustomASMCfgPPOutputSampleRateType *pPayload = (EliteMsg_CustomASMCfgPPOutputSampleRateType *)pkt.pPayload;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pPayload->unStreamID);

   //do not accept this command in close state.
   if ((AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)  || !pStream || (ASM_STREAM_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Received PP output sampling rate config cmd 0x%lx in invalid state.",
            pSession->ucInternalSessionId,  pPayload->unSecOpCode);
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }
   uint8_t stream_id = pStream->ucStreamId;

   if (!pStream->nodes[ASM_NODE_PP].pSvcHandle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: PP service does not exist. Cannot set output sampling rate!",
            pSession->ucInternalSessionId,  pPayload->unSecOpCode);
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Received PP output sampling rate config cmd 0x%lx",
         pSession->ucInternalSessionId, pStream->ucStreamId, pPayload->unSecOpCode);

   //send output config message to PP
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);
   uint32_t ulPayloadSize = sizeof(EliteMsg_CustomCfgPPOutputSampleRateType);

   elite_msg_any_t msg;
   if ( ADSP_FAILED( status = elite_msg_create_msg( &msg, &ulPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create custom message for PP output configuration!",
            pSession->ucInternalSessionId, stream_id);
      elite_svc_send_ack(&pkt, ADSP_ENOMEMORY);
      return ADSP_ENOMEMORY;
   }

   //fill up the custom message
   EliteMsg_CustomCfgPPOutputSampleRateType *ppConfigMsg = (EliteMsg_CustomCfgPPOutputSampleRateType *)msg.pPayload;
   ppConfigMsg->unSecOpCode = ELITEMSG_CUSTOM_CFG_PP_OUTPUT_SAMPLE_RATE;
   ppConfigMsg->keepInputSamplingRate = FALSE;
   ppConfigMsg->outputSamplingRate = pPayload->outputSamplingRate;


   //get the service handle of PP
   elite_svc_handle_t *pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;

   //push the message to PP command queue
   status = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(status))
   {
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send PP Configuration parameters to PP service!\n",
            pSession->ucInternalSessionId, stream_id);

      // Generate ACK
      elite_svc_send_ack(&pkt, status);
      return status;
   }

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_ProcessSetPPOutput(AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EFAILED;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   EliteMsg_CustomASMCfgPPOutput *pPayload = (EliteMsg_CustomASMCfgPPOutput *)pkt.pPayload;
   uint8_t stream_id = pPayload->unStreamID;
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   //do not accept this command in close state.
   if ((AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)  || !pStream || (ASM_STREAM_CLOSE == pStream->state))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Received PP channel and bitwidth config cmd 0x%lx in invalid state.",
            pSession->ucInternalSessionId,  pPayload->unSecOpCode);
      elite_svc_send_ack(&pkt, ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }

   if (!pStream->nodes[ASM_NODE_PP].pSvcHandle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: PP service does not exist. Cannot set output media type!",
            pSession->ucInternalSessionId,  pPayload->unSecOpCode);
      elite_svc_send_ack(&pkt, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Received PP channel and bitwidth config cmd 0x%lx",
         pSession->ucInternalSessionId, pStream->ucStreamId, pPayload->unSecOpCode);

   // send output channel and bitwidth configuration message to PP
   elite_msg_any_t msg;
   uint32_t ulClientToken = ASM_PopulateClientTokenForCustomCmd(stream_id, ASM_NODE_PP);
   uint32_t ulPayloadSize = sizeof(EliteMsg_CustomCfgPPOutputType);
   result = elite_msg_create_msg(&msg,
                                 &ulPayloadSize,
                                 ELITE_CUSTOM_MSG,
                                 pStSession->pSessionRespQ,
                                 ulClientToken,
                                 0);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create custom message for PP output configuration!",
            pSession->ucInternalSessionId, stream_id);
      elite_svc_send_ack(&pkt, ADSP_ENOMEMORY);
      return ADSP_ENOMEMORY;
   }

   // Copy the payload contents
   EliteMsg_CustomCfgPPOutputType *pPayloadCopy = (EliteMsg_CustomCfgPPOutputType *)msg.pPayload;
   pPayloadCopy->unSecOpCode = ELITEMSG_CUSTOM_CFG_PP_OUTPUT;
   pPayloadCopy->params.keepInputSamplingRate = TRUE;
   pPayloadCopy->params.outputSamplingRate = 0;
   if ((0 < pPayload->outputSamplingRate)
         && (pPayload->outputSamplingRate <= ASM_MAX_SUPPORTED_SAMPLING_RATE_HZ))
   {
      pPayloadCopy->params.keepInputSamplingRate = FALSE;
      pPayloadCopy->params.outputSamplingRate = pPayload->outputSamplingRate;
   }

   pPayloadCopy->params.useNativeBitsPerSample = TRUE;
   pPayloadCopy->params.outputBitsPerSample = 0;
   if (0 != pPayload->outputBitsPerSample)
   {
      pPayloadCopy->params.useNativeBitsPerSample = FALSE;
      pPayloadCopy->params.outputBitsPerSample = pPayload->outputBitsPerSample;
   }

   pPayloadCopy->params.useNativeNumChannels = FALSE;
   pPayloadCopy->params.outputNumChannels = pPayload->outputNumChannels;
   memscpy((void *)pPayloadCopy->params.outChannelMap,
           (sizeof(pPayloadCopy->params.outChannelMap)/sizeof(pPayloadCopy->params.outChannelMap[0])),
           (void *)pPayload->outputChannelMap,
           (sizeof(pPayload->outputChannelMap)/sizeof(pPayload->outputChannelMap[0])));

   //get the service handle of PP
   elite_svc_handle_t *pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;

   //push the message to PP command queue
   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);
   if (ADSP_FAILED(result))
   {
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send PP Configuration parameters to PP service!\n",
            pSession->ucInternalSessionId, stream_id);

      // Generate ACK
      elite_svc_send_ack(&pkt, result);
      return result;
   }
   return result;
}
