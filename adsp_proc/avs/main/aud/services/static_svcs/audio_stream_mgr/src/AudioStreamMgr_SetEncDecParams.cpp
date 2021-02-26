/**
@file AudStreamMgr_SetEncDecParams.cpp

@brief This file contains the implementation of functions for
setting encoder/decoder configuration parameters.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SetEncDecParams.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/22/2011  RP     Created
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
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
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_GetSetBits.h"

#include "adsp_media_fmt.h"
#include "adsp_error_codes.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"

#include "MixerSvc_Interface.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_SendEncDecParam(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_GraphNodeIndex nodeIndex,
      asm_stream_cmd_set_encdec_param_t *pPayload)
{
   ASM_GraphNode *pEncDec = &pStream->nodes[nodeIndex];
   elite_svc_handle_t *pSvcHandle = pEncDec->pSvcHandle;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if (NULL == pSvcHandle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: SendEncDecParam: invalid service handle",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_EUNSUPPORTED;
   }

   //use the same message structure as encoder cfg
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, nodeIndex);
   uint32_t ulPayloadSize = sizeof(elite_msg_param_cal_t);

   elite_msg_any_t msg;
   ADSPResult status;
   if ( ADSP_FAILED( status = elite_msg_create_msg( &msg, &ulPayloadSize,
         ELITE_CMD_SET_PARAM,
         pStSession->pSessionRespQ,
         ulClientToken,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: SendEncDecParam: Failed to create setparam message",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_ENOMEMORY;
   }

   //fill up the set param command
   elite_msg_param_cal_t *cfgMsg = (elite_msg_param_cal_t *)msg.pPayload;
   cfgMsg->unParamId             = ELITEMSG_PARAM_ID_CAL;
   cfgMsg->pnParamData           = (int32_t *) pPayload;

   //get svc command queue and push the msg into it
   status = ASM_PushCmdToPeerSvc(pMe, pSession, pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(status))
   {
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: SendEncDecParam:: Failed to send command",
            pSession->ucInternalSessionId,  stream_id);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

static ADSPResult AudioStreamMgr_SendEncCfgBlockToPP(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      void *pEncCfgBlk )
{
   ADSPResult status = ADSP_EOK;
   uint8_t stream_id = pStream->ucStreamId;
   uint32_t enc_cfg_id = pStream->sink_fmt_id;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ((pStream->nodes[ASM_NODE_PP]).pSvcHandle)
   {
      //fill PP output params
      PPOutputParams_t ppParams;
      memset(&ppParams, 0, sizeof(PPOutputParams_t));
      status = AudioStreamMgr_GetProcInitParamFromEncConfig(&ppParams,
            enc_cfg_id,
            pEncCfgBlk);
      if (ADSP_EOK != status)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to set PP output params!",
               pSession->ucInternalSessionId,  stream_id);
         return status;
      }
      if (ppParams.ulNumChannels > 8 )
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to set PP channels %u!",
               pSession->ucInternalSessionId,  stream_id, (unsigned int) ppParams.ulNumChannels);
         return ADSP_EFAILED;
      }

      //send output config message to PP
      uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);
      uint32_t ulPayloadSize = sizeof(EliteMsg_CustomCfgPPOutputType);

      elite_msg_any_t msg;
      if ( ADSP_FAILED( status = elite_msg_create_msg( &msg, &ulPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create custom message for PP output configuration!",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_ENOMEMORY;
      }

      //fill up the custom message
      EliteMsg_CustomCfgPPOutputType *ppConfigMsg = (EliteMsg_CustomCfgPPOutputType *)msg.pPayload;
      ppConfigMsg->unSecOpCode = ELITEMSG_CUSTOM_CFG_PP_OUTPUT;
      ppConfigMsg->params.outputNumChannels = ppParams.ulNumChannels;
      ppConfigMsg->params.outputSamplingRate = ppParams.ulSampleRate;
      memset(ppConfigMsg->params.outChannelMap, PCM_CHANNEL_INVALID, 8*sizeof(ppConfigMsg->params.outChannelMap[0]));

      if (0 == ppParams.ulNumChannels)
      {
         //0 implies native num channels
         ppConfigMsg->params.useNativeNumChannels = true;
      }
      else
      {
         ppConfigMsg->params.useNativeNumChannels = false;
         memscpy(ppConfigMsg->params.outChannelMap, sizeof(ppConfigMsg->params.outChannelMap), ppParams.channelMap, ppParams.ulNumChannels * sizeof(ppConfigMsg->params.outChannelMap[0]));
      }
      if (0 == ppParams.ulSampleRate)
      {
         //0 implies native sample rate
         ppConfigMsg->params.keepInputSamplingRate = true;
      }
      else
      {
         ppConfigMsg->params.keepInputSamplingRate = false;
      }

      // do not configure bits per sample
      ppConfigMsg->params.useNativeBitsPerSample = true;
      ppConfigMsg->params.outputBitsPerSample = 0;

      //get the service handle of PP
      elite_svc_handle_t *pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;

      //push the message to PP command queue
      status = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

      if (ADSP_FAILED(status))
      {
         //return created msg to buffer return queue
         elite_msg_release_msg(&msg);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send PP Configuration parameters!\n",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_EFAILED;
      }
   }
   return status;
}

static ADSPResult AudioStreamMgr_SendEncCfgBlockToMatrix(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      void *pEncCfgBlk )
{
   ADSPResult status = ADSP_EOK;
   uint8_t stream_id = pStream->ucStreamId;
   uint32_t enc_cfg_id = pStream->sink_fmt_id;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   //If TX matrix is present, set param on it's output port.
   if((pStream->nodes[ASM_NODE_TX_MTX]).pSvcHandle)
   {
      uint32_t ulClientToken = 0;
      uint32_t ulPayloadSize = sizeof(EliteMsg_CustomAdmStreamMtMxReConfig);
      elite_msg_any_t adm_reconfig_msg;
      if (ADSP_FAILED( status = elite_msg_create_msg( &adm_reconfig_msg,
            &ulPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create matrix output port config message!",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_ENOMEMORY;
      }

      EliteMsg_CustomAdmStreamMtMxReConfig *config_ports_msg_ptr = (EliteMsg_CustomAdmStreamMtMxReConfig *)adm_reconfig_msg.pPayload;
      config_ports_msg_ptr->unSecOpCode = ELITEMSG_CUSTOM_ADM_STREAM_MTMX_RECONFIG;
      config_ports_msg_ptr->phPort = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;
      config_ports_msg_ptr->unSessionID = PORT_FROM_SESSION_STREAM(pSession, pStream);
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session ID value to ADM = %lu!",
            pSession->ucInternalSessionId,  stream_id, config_ports_msg_ptr->unSessionID);
      config_ports_msg_ptr->direction = ADM_TX_DIR;
      config_ports_msg_ptr->bits_per_sample = pStream->bits_per_sample;

      //loopback may need delay on tx side, so configure with high buffer size.
      if (ASM_IsAsmLoopback(pSession->unMode))
      {
         config_ports_msg_ptr->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_LOOPBACK;
      }
      else
      {
         config_ports_msg_ptr->buffering_ms = ASM_TX_MATRIX_OUTPUT_BUFFERING_REGULAR;
      }

     switch(enc_cfg_id)
     {
     case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2:
     case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3:
     {
        asm_multi_channel_pcm_enc_cfg_v3_t *pEncConfig = (asm_multi_channel_pcm_enc_cfg_v3_t *) pEncCfgBlk ;
        config_ports_msg_ptr->num_channels = pEncConfig->num_channels;
        memset(config_ports_msg_ptr->channel_mapping, 0, sizeof(config_ports_msg_ptr->channel_mapping));
        memscpy(config_ports_msg_ptr->channel_mapping, sizeof(config_ports_msg_ptr->channel_mapping), pEncConfig->channel_mapping, sizeof (pEncConfig->channel_mapping));
        config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->num_channels == 0)? TRUE : FALSE;
        break;
     }
     case ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4:
     {
        asm_multi_channel_pcm_enc_cfg_v4_t *pEncConfig = (asm_multi_channel_pcm_enc_cfg_v4_t *) pEncCfgBlk ;
        config_ports_msg_ptr->num_channels = pEncConfig->num_channels;
        memset(config_ports_msg_ptr->channel_mapping, 0, sizeof(config_ports_msg_ptr->channel_mapping));
        memscpy(config_ports_msg_ptr->channel_mapping, sizeof(config_ports_msg_ptr->channel_mapping), pEncConfig->channel_mapping, sizeof (pEncConfig->channel_mapping));
        config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->num_channels == 0)? TRUE : FALSE;
        break;
     }
     default:
     {
	    AudioStreamMgr_GetMtxConfigParamFromNonPcmEncConfig((void*)config_ports_msg_ptr, enc_cfg_id, pEncCfgBlk);
        break;
     }
     }

      //send this message to ADM
      if(ADSP_FAILED(status = ASM_PushCmdToPeerSvc(pMe, pSession, pMe->pDeviceSvcCmdQ, (uint64_t*)&adm_reconfig_msg)))
      {
         //return created msg to buffer return queue
               elite_msg_release_msg(&adm_reconfig_msg);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send reconfig message to ADM!\n",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_EFAILED;
      }
   }
   return status;
}

ADSPResult AudioStreamMgr_ProcessPcmEncCfgForPpMtx(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      void *pEncCfgBlk)
{
   uint8_t stream_id = pStream->ucStreamId;

   ADSPResult result = AudioStreamMgr_SendEncCfgBlockToPP(pMe, pSession, pStream, pEncCfgBlk);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send enc cfg to PP",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   result = AudioStreamMgr_SendEncCfgBlockToMatrix(pMe, pSession, pStream,  pEncCfgBlk);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send enc cfg to TX matrix",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   return result;

}

/**
 * Even if encoder is not present, this command is handled.
 *
 * This is required especially for multichannel PCM loopback where encoder
 * is actually not present. However, in order to configure the Tx matrix output
 * to multichannel (default is stereo only), the client sends enc cfg command.
 * Although, sending enc cfg without enc doesn't make sense, the idea is that the absence of
 * encoder in PCM loopback is just an optimization and it shouldn't change the command flow.
 *
 */

ADSPResult AudioStreamMgr_ProcessEncoderConfigBlock(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      asm_stream_cmd_set_encdec_param_t *pPayload)
{
   asm_enc_cfg_blk_param_v2_t *pEncCfgParam = (asm_enc_cfg_blk_param_v2_t*)((uint8_t*)pPayload +
         sizeof(asm_stream_cmd_set_encdec_param_t));
   void *pEncCfgBlk = (void*)((uint8_t*)pEncCfgParam + sizeof(asm_enc_cfg_blk_param_v2_t));
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id = pStream->ucStreamId;

   //check the session state
   if ( AUDIO_STREAM_MGR_SESSION_PAUSE != pSession->unState )
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Invalid session state %d for \
                                            setting the encoder configuration block",
                                            pSession->ucInternalSessionId,  stream_id, pSession->unState);
      return ADSP_ENOTREADY;
   }

   ADSPResult result = AudioStreamMgr_ProcessPcmEncCfgForPpMtx(pMe, pSession, pStream, pEncCfgBlk);

   if (ADSP_FAILED(result))
   {
      return result;
   }

   if ((pStream->nodes[ASM_NODE_ENC]).pSvcHandle)
   {
      //Now send the encoder config block to encoder service
      uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_ENC);
      uint32_t ulPayloadSize = sizeof(elite_msg_param_cal_t);

      elite_msg_any_t encMsg;
      if ( ADSP_FAILED( result = elite_msg_create_msg( &encMsg, &ulPayloadSize,
            ELITE_CMD_SET_PARAM,
            pStSession->pSessionRespQ,
            ulClientToken,
            0) ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to create setparam message for encoder configuration!",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_ENOMEMORY;
      }

      //fill up the set param command
      elite_msg_param_cal_t *encCfgMsg = (elite_msg_param_cal_t *)encMsg.pPayload;
      encCfgMsg->unParamId = ELITEMSG_PARAM_ID_CAL;
      encCfgMsg->pnParamData = (int32_t *)pPayload;

      //get enc svc command queue and push the msg into it
      elite_svc_handle_t *pEncSvcHandle = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle;

      result = ASM_PushCmdToPeerSvc(pMe, pSession, pEncSvcHandle->cmdQ, (uint64_t*)&encMsg);

      if (ADSP_FAILED(result))
      {
         //return created msg to buffer return queue
         elite_msg_release_msg(&encMsg);
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Failed to send encoder config to enc svc!\n",
               pSession->ucInternalSessionId,  stream_id);
         return ADSP_EFAILED;
      }
   }

   return ADSP_EOK;
}
