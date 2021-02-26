/**
@file AudStreamMgr_StreamUtil.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_StreamUtil.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013 RB     created file
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_Stream.h"
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
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
extern uint32_t asmMemoryMapClient;

// maximum number of buffers in output buffer queue of switch svc.
static const uint32_t SWITCH_SVC_NUM_OUTPUT_BUFS = 2;

/* This function created the decoder service and launches
the decoder service thread*/
ADSPResult AudioStreamMgr_CreateDecSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   ASM_GraphNode *pDecNode = &(pStream->nodes[ASM_NODE_DEC]);
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   ADSPResult result=ADSP_EOK;

   if(ASM_MEDIA_FMT_MIDI == pStream->src_fmt_id)
   {

      /*Make sure stream is not gapless before creating MIDI service */
      if(ASM_IsGaplessSession(pSession))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr: MIDI cannot be part of gapless session");
         return ADSP_EUNSUPPORTED;
      }

      /** Create MIDI Dynamic Service.**/
      /*      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Creating MIDI Service");

      MidiInitParams_t midi_param;
      midi_param.apr_handle = AudioStreamMgr_GetAprHandle();
      midi_param.pKcps = &pDecNode->kips;

      result = MidiSvc_Create ( (uint32_t)(&midi_param), (void**)(&pDecNode->pSvcHandle) );

      if (ADSP_FAILED(result))
      {
         return result;
      }

      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: MIDI Service Created");
       */
   }
   else
   {
      AudioStreamMgr_CallBackHandleType DecCbData;
      uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_DEC);
      (void) AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &DecCbData);

      AudioDecSvcInitParams_t decInitParams;
      memset(&decInitParams, 0, sizeof(decInitParams));

      decInitParams.io_param.format_id = pOpenStream->src_fmt_id; //meaning changes on whether this stream is input or output. can be zero for compressed passthrough
      //note: in case open_compressed src fmt is set as sink so that the decoder outputs 61937 after conversion.
      decInitParams.io_param.stream_type = pOpenStream->stream_type; //meaning changes on whether this stream is input or output.
      decInitParams.io_param.stream_id = pStream->ucStreamId;

      decInitParams.io_param.io_type = AUDIO_DEC_SVC_IN_OUT_STREAM;

      decInitParams.io_param.bits_per_sample = pOpenStream->bits_per_sample;
      decInitParams.io_param.decoderConverterMode = pOpenStream->decoder_conv_mode;
      decInitParams.io_param.eventToken = client_token;
      decInitParams.io_param.pEventQ = pStSession->pSessionRespQ;
      decInitParams.io_param.pCbData = &DecCbData;
      decInitParams.io_param.pDelay = &pDecNode->buf_delay; // The decoder does not provide algorithmic delay.
      pDecNode->alg_delay = 0;
      decInitParams.io_param.perf_mode = pStream->stream_perf_mode;
      decInitParams.io_param.ulDataLogId = pStream->unDataLogId;
      decInitParams.is_rt_dec = pOpenStream->is_rt_dec;

      //need to provide only switch svc delay as only that's variable based on SR.
      decInitParams.io_param.out.downstream_buf_delay_ptr = AudioStreamMgr_GetPostDecSvcVarBufDelay(pSession->pFirstGaplessStrm);

      decInitParams.pEventQ = pStSession->pSessionRespQ;

      if ( (decInitParams.io_param.io_type == AUDIO_DEC_SVC_MIMO_INPUT_STREAM) ||
            (decInitParams.io_param.io_type == AUDIO_DEC_SVC_IN_OUT_STREAM))
      {
         decInitParams.io_param.in.bEnableSrCmEvent = pOpenStream->sr_cm_event_needed;
         decInitParams.io_param.in.no_apr_dec = pOpenStream->no_apr_enc_dec;

         switch(pOpenStream->dec_io_fmt_conv)
         {
         case ASM_IO_FORMAT_CONV_AUTO:                //decode raw to PCM OR encode PCM to raw.
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO;
            break;
         case ASM_IO_FORMAT_CONV_61937_TO_PCM:       //depacketizer->decodes
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_PCM;
            break;
         case ASM_IO_FORMAT_CONV_61937_TO_61937:     //compressed pass through
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937;
            break;
         case ASM_IO_FORMAT_CONV_61937_CONV_61937:   //depacketizer->conv->packetize
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_61937_CONV_61937;
            break;
         case ASM_IO_FORMAT_CONV_61937_TO_RAW:       //depacketize
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW;
            break;
         case ASM_IO_FORMAT_CONV_RAW_TO_61937:       //packetize
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_RAW_TO_61937;
            break;
         case ASM_IO_FORMAT_CONV_61937_TO_MULTI:      //depacketize->MIMO decoder
            decInitParams.io_param.in.io_fmt_conv = DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_MULTI;
            break;
         case ASM_IO_FORMAT_CONV_PCM_TO_61937:        //encode->packetize.
         default:
            return ADSP_EUNSUPPORTED;
         }
      }

      {
         //creating a new decoder (can be called for output of MIMO decoder, input or both)

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Decoder Service",
               pSession->ucInternalSessionId,  pStream->ucStreamId);

         result = AudioDecSvc_Create (&decInitParams, (void**)(&pDecNode->pSvcHandle));

         if (ADSP_FAILED(result))
         {
            return result;
         }

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Decoder Service Created",
               pSession->ucInternalSessionId,  pStream->ucStreamId);

         pDecNode->threadId = pDecNode->pSvcHandle->threadId;
      }
   }
   return ADSP_EOK;
}

/* This function creates the encoder service*/
ADSPResult AudioStreamMgr_CreateEncSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   ASM_GraphNode *pEncNode = &(pStream->nodes[ASM_NODE_ENC]);
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   ADSPResult result=ADSP_EOK;
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Encoder Service",
         pSession->ucInternalSessionId,  pStream->ucStreamId);

   // compose call back data for encsvc to raise event or send back to ASM.
   AudioStreamMgr_CallBackHandleType cbData;
   uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_ENC);
   result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &cbData);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to create call back for Encoder service",
            pSession->ucInternalSessionId,  pStream->ucStreamId);

      return result;
   }

   /// This pointer is provided to enc svc for initialization.
   AudioEncSvcInitParams_t encInitParams;
   memset(&encInitParams, 0, sizeof(AudioEncSvcInitParams_t));

   encInitParams.ulConfigId = pStream->sink_fmt_id;
   // Send the metadata request to enc svc in InitParams structure
   encInitParams.bFrameMetaDataEnable = pOpenStream->meta_info_flag;
   encInitParams.pCbData = &cbData;

   encInitParams.bits_per_sample = pStream->bits_per_sample;
   encInitParams.ulDataLogId = pStream->unDataLogId;
   encInitParams.frames_per_buffer = pOpenStream->enc_frames_per_buffer;
   encInitParams.no_apr_enc = pOpenStream->no_apr_enc_dec;
   encInitParams.pEventQ = pStSession->pSessionRespQ;
   encInitParams.eventToken = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_ENC);
   encInitParams.perf_mode = pStream->stream_perf_mode;

   switch(pOpenStream->enc_io_fmt_conv)
   {
   case ASM_IO_FORMAT_CONV_AUTO:                //decode raw to PCM OR encode PCM to raw.
      encInitParams.io_fmt_conv = ENC_SVC_IO_FORMAT_CONV_TYPE_AUTO;
      break;
   case ASM_IO_FORMAT_CONV_61937_TO_PCM:       //depacketizer->decodes
      return ADSP_EUNSUPPORTED;
      break;
   case ASM_IO_FORMAT_CONV_61937_TO_61937:     //compressed pass through
      return ADSP_EUNSUPPORTED;
      break;
   case ASM_IO_FORMAT_CONV_61937_CONV_61937:   //depacketizer->conv->packetize
      return ADSP_EUNSUPPORTED;
      break;
   case ASM_IO_FORMAT_CONV_61937_TO_RAW:       //depacketize
      encInitParams.io_fmt_conv = ENC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_RAW;
      break;
   case ASM_IO_FORMAT_CONV_RAW_TO_61937:       //packetize
      return ADSP_EUNSUPPORTED;
      break;
   case ASM_IO_FORMAT_CONV_PCM_TO_61937:        //encode->packetize.
      encInitParams.io_fmt_conv = ENC_SVC_IO_FORMAT_CONV_TYPE_PCM_TO_61937;
      break;
   default:
      return ADSP_EUNSUPPORTED;
   }

   result = AudioEncSvc_Create ( (uint32_t) (&encInitParams), (void**)(&pEncNode->pSvcHandle));

   if (ADSP_FAILED(result))
   {
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Encoder Service Created",
         pSession->ucInternalSessionId,  pStream->ucStreamId);

   pEncNode->threadId = pEncNode->pSvcHandle->threadId;

   return ADSP_EOK;
}

/* This function creates the Post/Pre proc service*/
ADSPResult AudioStreamMgr_CreatePPSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   ASM_GraphNode *pPPNode = &(pStream->nodes[ASM_NODE_PP]);
   AudPP_AudProcType type;
   ADSPResult result=ADSP_EOK;
   //AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( (ASM_TX_MODE == pSession->unMode) ||
         (ASM_TX_PUSH_MODE == pSession->unMode) )
   {
      type = DYNA_SVC_PP_TYPE_POPREP;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Pre-Proc Service",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
   }
   else
   {
      type = DYNA_SVC_PP_TYPE_POPP;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Post-Proc Service",
            pSession->ucInternalSessionId,  pStream->ucStreamId );
   }

   // compose call back data for PP svc to raise event or send back to ASM.
   AudioStreamMgr_CallBackHandleType cbData;
   uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_PP);
   result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &cbData);

   AudPPSvcInitParams_t PPInitParams;
   memset(&PPInitParams, 0, sizeof(AudPPSvcInitParams_t));
   PPInitParams.outputMediaTypeInitParams.keepInputSamplingRate = FALSE;
   PPInitParams.outputMediaTypeInitParams.outputSamplingRate = pOpenStream->default_pp_sample_rate;
   PPInitParams.outputMediaTypeInitParams.useNativeNumChannels = TRUE;
   PPInitParams.outputMediaTypeInitParams.outputNumChannels = 2; //NA: as NativeNumChannels is TRUE
   PPInitParams.outputMediaTypeInitParams.useNativeBitsPerSample = TRUE;
   PPInitParams.outputMediaTypeInitParams.outputBitsPerSample = 0; //NA: as NativeBitsPerSample is TRUE
   PPInitParams.outputMediaTypeInitParams.outChannelMap[0] = PCM_CHANNEL_L;
   PPInitParams.outputMediaTypeInitParams.outChannelMap[1] = PCM_CHANNEL_R;

   uint32_t pp_buffer_num_frames_per_channel;
   if ((ASM_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode) ||
         (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pStream->stream_perf_mode) ||
         (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pStream->stream_perf_mode) ||
         (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pStream->stream_perf_mode))
   {
      pp_buffer_num_frames_per_channel = PP_BUFFER_NUM_FRAME_LOWLATENCY_MODE;
   }
   else
   {
      pp_buffer_num_frames_per_channel = PP_BUFFER_NUM_FRAME_LEGACY_MODE;
   }

   PPInitParams.outputBufferNumAfeFramesPerChannel = pp_buffer_num_frames_per_channel;
   PPInitParams.topology = pOpenStream->pp_topo_id;
   PPInitParams.type = type;
   PPInitParams.memoryMapClient = asmMemoryMapClient;
   AudioStreamMgr_PPCallbackType PPCb;
   PPInitParams.pMsgCb = &(PPCb.function);
   PPInitParams.pCtxt = &(PPCb.context);
   PPInitParams.pBufDelay = &(pStream->nodes[ASM_NODE_PP]).buf_delay;
   PPInitParams.pAlgorithmicDelay = &(pStream->nodes[ASM_NODE_PP]).alg_delay;
   PPInitParams.ulDataLogId = pStream->unDataLogId;
   PPInitParams.initializeWithCompressedFormat = FALSE;
   PPInitParams.perf_mode = pStream->stream_perf_mode;
   PPInitParams.pCbData = &cbData;
   //MAP:TBD: We cannot use ASM_IsAsmLoopback call as this change is only for PCM in and PCM out
   if (ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE == pSession->unMode)
   {
      PPInitParams.is_input_output_realtime = TRUE;
   }

   PPInitParams.is_echo_canceller_session = FALSE;
   elite_svc_handle_t   *pDummyHandle; // Dummy far handle for stream processing service.
   result = AudPP_CreateSvc ( &PPInitParams, (void**)(&pPPNode->pSvcHandle), (void**)&pDummyHandle );

   //QURT_ELITE_ASSERT(NULL == pDummyHandle);

   if (ADSP_FAILED(result))
   {
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: PP Service Created",
         pSession->ucInternalSessionId,  pStream->ucStreamId);

   pStream->PPCallback = PPCb;
   pPPNode->threadId = pPPNode->pSvcHandle->threadId;

   return ADSP_EOK;
}


/* This function creates the Switch service. Switch service is used only in
gapless mode to direct inputs from two decoders into a common POPP. */
ADSPResult AudioStreamMgr_CreateSwitchSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   ASM_GraphNode *pSwitchNode = &(pStream->nodes[ASM_NODE_SWITCH]);
   ADSPResult result=ADSP_EOK;

   //Fill ASM Cb for switch svc
   AudioStreamMgr_CallBackHandleType cbData;
   uint32_t client_token  = ASM_PopulateClientToken(pStream->ucStreamId, ASM_NODE_SWITCH);
   //This will fail only when &cbData==NULL, thus we don't bail out here.
   result = AudioStreamMgr_FillCbDatForDynSvc(pMe, pSession, client_token, &cbData);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Creating Switch Service",
         pSession->ucInternalSessionId,  pStream->ucStreamId );

   switch_svc_init_params init_params;
   init_params.delay_ptr = &pSwitchNode->buf_delay; // The switch service does not return the algorithmic delay.
   pSwitchNode->alg_delay = 0;
   init_params.pCbData = &cbData;
   init_params.max_out_bufs = SWITCH_SVC_NUM_OUTPUT_BUFS;
   result = switch_svc_create((uint32_t)(&init_params), (void**)(&pSwitchNode->pSvcHandle));

   if (ADSP_FAILED(result))
   {
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: switch Service Created",
         pSession->ucInternalSessionId,  pStream->ucStreamId);

   pSwitchNode->threadId = pSwitchNode->pSvcHandle->threadId;

   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_CreateSplitterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   return ADSP_ENOTIMPL;
}

ADSPResult AudioStreamMgr_CreateCombinerSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream)
{
   return ADSP_ENOTIMPL;
}

ADSPResult AudioStreamMgr_CloseStreamDecSvc (AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result;

   elite_msg_any_t msg;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t token = 0;

   token = ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);

   ASM_GraphNode *pDecNode = &(pStream->nodes[ASM_NODE_DEC]);

   uint32_t unPayloadSize = sizeof( EliteMsg_CustomDecCloseStreamType );

   //return ok since we cannot destroy while the module is being used in another stream.
   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId, stream_id);
      return result;
   }

   EliteMsg_CustomDecCloseStreamType *pCloseMsgPayload =
         (EliteMsg_CustomDecCloseStreamType*) msg.pPayload;
   pCloseMsgPayload->unSecOpCode = ELITEMSG_CUSTOM_DEC_CLOSE_STREAM;
   pCloseMsgPayload->pHandle = pDecNode->pSvcHandle;

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pDecNode->pSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send close stream to Svc [0x%lx,0x%lx]",
            pSession->ucInternalSessionId, stream_id, pDecNode->pSvcHandle->unSvcId,
            (uint32_t) (pDecNode->pSvcHandle->threadId));
      //return the created message back to buffer queue
      elite_msg_release_msg(&msg);
      return result;
   }

   return result;
}

ADSPResult AudioStreamMgr_ConnectStreamGeneric(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_GraphNodeIndex upstream_node_ind,
      ASM_GraphNodeIndex downstream_node_ind,
      bool_t is_custom)
{
   elite_msg_any_t msg;
   ADSPResult result;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   ASM_GraphNode *pUpStrmNode = &pStream->nodes[upstream_node_ind];
   ASM_GraphNode *pDownStrmNode = &pStream->nodes[downstream_node_ind];

   uint32_t unPayloadSize = sizeof( elite_msg_cmd_connect_t);
   uint32_t token = 0;
   if (is_custom)
   {
      token = ASM_PopulateClientTokenForCustomCmd(stream_id, upstream_node_ind);
   }
   else
   {
      token = ASM_PopulateClientToken(stream_id, upstream_node_ind);
   }

   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CMD_CONNECT,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,
            stream_id );
      return result;
   }

   elite_svc_handle_t *pUpStrmSvcHandle = pUpStrmNode->pSvcHandle;
   elite_svc_handle_t *pDownStrmSvcHandle = pDownStrmNode->pSvcHandle;

   ((elite_msg_cmd_connect_t *) msg.pPayload)->pSvcHandle = pDownStrmSvcHandle;

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: connecting Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
         pSession->ucInternalSessionId,   stream_id,
         pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
         pDownStrmSvcHandle->unSvcId,         (uint32_t) (pDownStrmSvcHandle->threadId));

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pUpStrmSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to connect Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
            pSession->ucInternalSessionId,   stream_id,
            pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
            pDownStrmSvcHandle->unSvcId,         (uint32_t) (pDownStrmSvcHandle->threadId));
      //return the created msg to buffer return queue
      elite_msg_release_msg(&msg);
      return result;
   }
   return result;
}

ADSPResult AudioStreamMgr_ConnectStreamDecSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex upstream_node_ind,
      ASM_GraphNodeIndex downstream_node_ind,
      bool_t is_custom)
{
   elite_msg_any_t msg;
   ADSPResult result;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   ASM_GraphNode *pUpStrmNode = &pStream->nodes[upstream_node_ind];
   ASM_GraphNode *pDownStrmNode = &pStream->nodes[downstream_node_ind];

   uint32_t unPayloadSize = sizeof( EliteMsg_CustomDecConnectStreamType);
   uint32_t token = 0;
   if (is_custom)
   {
      token = ASM_PopulateClientTokenForCustomCmd(stream_id, upstream_node_ind);
   }
   else
   {
      token = ASM_PopulateClientToken(stream_id, upstream_node_ind);

   }

   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,
            stream_id );
      return result;
   }

   elite_svc_handle_t *pUpStrmSvcHandle = pUpStrmNode->pSvcHandle;
   elite_svc_handle_t *pDownStrmSvcHandle = pDownStrmNode->pSvcHandle;

   EliteMsg_CustomDecConnectStreamType *pPayload = (EliteMsg_CustomDecConnectStreamType*)msg.pPayload;
   pPayload->unSecOpCode = ELITEMSG_CUSTOM_DEC_CONNECT_STREAM;
   pPayload->stream_id = stream_id;
   pPayload->pDownStreamHandle = pDownStrmSvcHandle;

   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "AudioStreamMgr [%u,%u]: connecting Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
         pSession->ucInternalSessionId,   stream_id,
         pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
         pDownStrmSvcHandle->unSvcId,         (uint32_t) (pDownStrmSvcHandle->threadId));

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pUpStrmSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to connect Svc [0x%lx,0x%lx] to Svc [0x%lx,0x%lx]!",
            pSession->ucInternalSessionId,   stream_id,
            pUpStrmSvcHandle->unSvcId,         (uint32_t) (pUpStrmSvcHandle->threadId),
            pDownStrmSvcHandle->unSvcId,         (uint32_t) (pDownStrmSvcHandle->threadId));
      //return the created msg to buffer return queue
      elite_msg_release_msg(&msg);
      return result;
   }
   return result;
}

/* This function disconnects the playback graph*/
ADSPResult AudioStreamMgr_CloseRegularStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;

   if(ADSP_FAILED(result = AudioStreamMgr_DeregisterQs(pMe, pSession, pStream)))
   {
      return result;
   }
   pStream->PPCallback.context = NULL;
   pStream->PPCallback.function = NULL;

   result =  AudioStreamMgr_StreamDisConnectGraph(pMe, pSession, pStream);

   return result;
}

/*This function is called to open a gapless stream when another stream in the
session is already opened.
we cannot call generic method because this stream has switch, PP and Rx mtx common to the first
 */

/*This function is called to open a gapless stream when another stream in the
session is already opened.
we cannot call generic method because this stream has switch, PP and Rx mtx common to the first
 */
ADSPResult AudioStreamMgr_OpenSecondGaplessStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      elite_apr_packet_t *pPkt,
      ASM_OpenStreamParams_t *pOpenStream)
{

   ADSPResult result = ADSP_EOK;
   elite_msg_any_t msg;
   uint32_t token = 0;
   elite_svc_handle_t *pSwitchSvcHandle;
   uint32_t unPayloadSize;
   ASM_GraphNode *pSwitchNode = NULL;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id = pStream->ucStreamId;

   result = AudioStreamMgr_CreateDecSvc(pMe,pSession,pStream,pOpenStream);

   if(ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: Failed to create second decoder service in gapless session",
            pSession->ucInternalSessionId,  stream_id );
      return result;
   }

   pSwitchNode = &pSession->pFirstGaplessStrm->nodes[ASM_NODE_SWITCH];

   if (!pSwitchNode->pSvcHandle)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to get switch node",
            pSession->ucInternalSessionId,stream_id );
      goto __bailout;
   }

   //Send a message to switch requesting vacant port handle
   token = ASM_PopulateClientToken(stream_id, ASM_NODE_SWITCH);

   unPayloadSize = sizeof( elite_msg_custom_switch_get_svc_handle);

   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create custom mesage",
            pSession->ucInternalSessionId,
            stream_id );
      goto __bailout;
   }

   ((elite_msg_custom_switch_get_svc_handle *)msg.pPayload)->unSecOpCode =
         ELITEMSG_CUSTOM_SWITCH_GET_SVC_HANDLE;

   pSwitchSvcHandle = pSwitchNode->pSvcHandle;

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pSwitchSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to request handle from switch svc!",
            pSession->ucInternalSessionId,   stream_id);
      //return the msg back to buffer q
      elite_msg_release_msg(&msg);
      goto __bailout;
   }

   return ADSP_EOK;

   __bailout:
   //destroy decoder serivce
   (void) AudioStreamMgr_DestroyStreamSvc(pMe, pSession, pStream);

   return result;
}

ADSPResult AudioStreamMgr_VacateSwitchSvcPort(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;
   uint8_t stream_id = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint32_t token, unPayloadSize;
   elite_msg_any_t msg;
   elite_svc_handle_t *pSwitchSvcHandle;

   //send the port ID of the port to be to be vacated to switch
   //This flushes the associated port and also
   //resets the port state (indicating vacant port)
   token = ASM_PopulateClientToken(stream_id, ASM_NODE_SWITCH);

   unPayloadSize = sizeof( elite_msg_custom_switch_vacate_port);

   if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         token,
         0) ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create custom message",
            pSession->ucInternalSessionId,
            stream_id );
      // generate ack and free apr pakcet
      return result;
   }

   ((elite_msg_custom_switch_vacate_port *)msg.pPayload)->unSecOpCode = ELITEMSG_CUSTOM_SWITCH_VACATE_PORT;
   ((elite_msg_custom_switch_vacate_port *)msg.pPayload)->pSvcHandle = (pStream->nodes[ASM_NODE_SWITCH]).pSvcHandle;

   pSwitchSvcHandle = (pStream->nodes[ASM_NODE_SWITCH]).pSvcHandle;

   if (ADSP_FAILED(result = ASM_PushCmdToPeerSvc(pMe, pSession, pSwitchSvcHandle->cmdQ, (uint64_t*)&msg )))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [%u,%u]: FAILED to send port vacate  msg to switch svc!",
            pSession->ucInternalSessionId,   stream_id);
      // return msg to buffer queue
      elite_msg_release_msg(&msg);
      // generate ack and free apr pakcet
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: sent vacate to switch svc",
         pSession->ucInternalSessionId, stream_id );

   return result;
}

/*This function closes one of the gapless streams when two are opened in the
same session*/
ADSPResult AudioStreamMgr_CloseSingleGaplessStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{

   ADSPResult result = ADSP_EOK;

   result = AudioStreamMgr_DeregisterQs(pMe, pSession, pStream);
   if(ADSP_FAILED(result))
      return result;

   result = AudioStreamMgr_StreamDisConnectDecFromSwitchSvc(pMe, pSession, pStream);

   return result;

}

/** This function destroys the stream services (does not
 *  release matrix ports) */
ADSPResult AudioStreamMgr_DestroyStreamSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult res = ADSP_EOK;

   if (!pStream) return ADSP_EOK;

   //destroy decoder svc first
   res |= AudioStreamMgr_StreamDestroyMod(pMe, pSession, pStream, ASM_NODE_DEC);
   res |= AudioStreamMgr_StreamDestroyMod(pMe, pSession, pStream, ASM_NODE_ENC);
   res |= AudioStreamMgr_StreamDestroyMod(pMe, pSession, pStream, ASM_NODE_SWITCH);
   res |= AudioStreamMgr_StreamDestroyMod(pMe, pSession, pStream, ASM_NODE_PP);

   if (0!= pStream->mem_map_handle[ASM_POS_BUF_INDEX])
   {
      res |= ASM_MemUnmapHandler(asmMemoryMapClient, pStream->mem_map_handle[ASM_POS_BUF_INDEX]);
      pStream->mem_map_handle[ASM_POS_BUF_INDEX] = 0;
   }
   if (0 != pStream->mem_map_handle[ASM_CIRC_BUF_INDEX])
   {
      res |= ASM_MemUnmapHandler(asmMemoryMapClient, pStream->mem_map_handle[ASM_CIRC_BUF_INDEX]);
      pStream->mem_map_handle[ASM_CIRC_BUF_INDEX] = 0;
   }

   return res;
}

uint8_t AudioStreamMgr_GetStrmId(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   return ASM_GET_STREAM_ID( port );
}

/**
 * if curr_node == NULL, then first non-null node is returned.
 */
ASM_GraphNode* ASM_GetNextNodeInTopo(ASM_GraphNode *node_array, const ASM_SvcList *topo, ASM_GraphNode *curr_node)
{
   ASM_GraphNode *next_node = NULL;
   //iterate through the topo from beginning to end
   uint32_t i = 0;
   bool_t node_found = false;
   for(i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if (ASM_INVALID_NODE == topo->svc_nodes[i]) continue;

      if (node_found || !curr_node)
      {
         next_node = &node_array[topo->svc_nodes[i]];
         break;
      }

      if(curr_node == &node_array[topo->svc_nodes[i]])
      {
         node_found = true;
      }
   }

   return next_node;
}

/**
 * if curr_node == ASM_INVALID_NODE, then first non-invalid node is returned.
 */
ASM_GraphNodeIndex ASM_GetNextNodeIndexInTopo(const ASM_SvcList *topo, ASM_GraphNodeIndex curr_node)
{
   ASM_GraphNodeIndex next_node = ASM_INVALID_NODE;
   //iterate through the topo from beginning to end
   uint32_t i = 0;
   bool_t node_found = false;
   for(i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if (ASM_INVALID_NODE == topo->svc_nodes[i]) continue;

      if (node_found || (curr_node == ASM_INVALID_NODE))
      {
         next_node = topo->svc_nodes[i];
         break;
      }
      if(curr_node == topo->svc_nodes[i])
      {
         node_found = true;
      }
   }

   return next_node;
}

ASM_GraphNodeIndex ASM_GetNodeNextToTxMtMxStRtrOrAfe(AudioStreamMgr_StreamStateType *pStream)
{
   ASM_GraphNodeIndex node_index = ASM_INVALID_NODE;
   bool_t node_found = false;
   for (uint32_t i=0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if (ASM_INVALID_NODE == pStream->pTopo->StrmSvcInfo.svc_nodes[i])
      {
         continue;
      }

      if (node_found)
      {
         return pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      }

      if ( (ASM_NODE_TX_MTX == pStream->pTopo->StrmSvcInfo.svc_nodes[i]) ||
            (ASM_NODE_TX_ST_RTR == pStream->pTopo->StrmSvcInfo.svc_nodes[i]) ||
            (ASM_NODE_TX_GENERIC == pStream->pTopo->StrmSvcInfo.svc_nodes[i]))
      {
         node_found = true;
      }
   }
   return node_index;
}

/**
 * matrix or stream router node in the given direction is returned in asmNode.
 * the type (stream router, matrix) is indicated in the return type.
 */
ASM_MtxStRtrType ASM_GetMatrixOrStreamRouterNode(AudioStreamMgr_StreamStateType *pStream,
      ASM_direction direction, ASM_GraphNodeIndex* asmNodeIndex)
{
   ASM_GraphNodeIndex mtmx_node, strRtr_node;
   elite_svc_handle_t *mtmx_handle, *stRtr_handle;

   /* node with non-NULL service handle is chosen */
   if (ASM_RX_DIR == direction)
   {
      mtmx_node   = ASM_NODE_RX_MTX;
      strRtr_node = ASM_NODE_RX_ST_RTR;
   }
   else if (ASM_TX_DIR == direction)
   {
      mtmx_node   = ASM_NODE_TX_MTX;
      strRtr_node = ASM_NODE_TX_ST_RTR;
   }
   else
   {
      *asmNodeIndex = ASM_NODE_MAX;
      return ASM_DEV_IF_NONE;
   }

   mtmx_handle    = pStream->nodes[mtmx_node].pSvcHandle;
   stRtr_handle   = pStream->nodes[strRtr_node].pSvcHandle;

   if(mtmx_handle != NULL)
   {
      *asmNodeIndex = mtmx_node;
      return ASM_DEV_IF_MATRIX;
   }
   else if (stRtr_handle != NULL)
   {
      *asmNodeIndex = strRtr_node;
      return ASM_DEV_IF_STREAM_ROUTER;
   }
   else
   {
      *asmNodeIndex = ASM_NODE_MAX;
      return ASM_DEV_IF_NONE;
   }
}

//make sure there are not more than one Rx device in topo.
ASM_GraphNodeIndex ASM_GetRxDeviceNodeInTopo(AudioStreamMgr_StreamStateType *pStream)
{
   for (uint8_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      ASM_GraphNodeIndex node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      if ( (node_ind == ASM_NODE_RX_MTX) ||
            (node_ind == ASM_NODE_RX_ST_RTR) ||
            (node_ind == ASM_NODE_RX_GENERIC) )
      {
         return node_ind;
      }
   }
   return ASM_INVALID_NODE;
}

//make sure there are not more than one Tx device in topo.
ASM_GraphNodeIndex ASM_GetTxDeviceNodeInTopo(AudioStreamMgr_StreamStateType *pStream)
{
   for (uint8_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      ASM_GraphNodeIndex node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      if ( (node_ind == ASM_NODE_TX_MTX) ||  (node_ind == ASM_NODE_TX_ST_RTR) || (node_ind == ASM_NODE_TX_GENERIC))
      {
         return node_ind;
      }
   }
   return ASM_INVALID_NODE;
}

bool_t ASM_IsRxDeviceHandleNeeded(AudioStreamMgr_StreamStateType *pStream)
{
   for (uint8_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      ASM_GraphNodeIndex node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      if ( (node_ind == ASM_NODE_RX_MTX) ||
            (node_ind == ASM_NODE_RX_ST_RTR) ||
            (node_ind == ASM_NODE_RX_GENERIC) )
      {
         return true;
      }
   }
   return false;
}

ASM_GraphNodeIndex ASM_CheckForAnotherDeviceHandle(AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex dev_node_ind)
{
   ASM_GraphNodeIndex req_dev_node = ASM_INVALID_NODE;

   for (uint8_t i = 0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      ASM_GraphNodeIndex node_ind = pStream->pTopo->StrmSvcInfo.svc_nodes[i];
      if ( (node_ind == ASM_NODE_RX_MTX) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
      if ( (node_ind == ASM_NODE_TX_MTX) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
      if ( (node_ind == ASM_NODE_RX_ST_RTR) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
      if ( (node_ind == ASM_NODE_TX_ST_RTR) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
      if ( (node_ind == ASM_NODE_RX_GENERIC) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
      if ( (node_ind == ASM_NODE_TX_GENERIC) && (node_ind != dev_node_ind) )
      {
         req_dev_node = node_ind;
      }
   }
   return req_dev_node;
}

AudioStreamMgr_StreamStateType *ASM_AllotStream(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id)
{

   //for streams in gapless close state, the stream is re-used only if its of the same stream-id
   //otherwise, a new slot in the aStreams is used.
   if (ASM_INVALID_INDEX != pSession->ucStreamId2Index[stream_id])
   {
      return &(pSession->aStreams[pSession->ucStreamId2Index[stream_id]]);
   }

   for (uint16_t i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if ( (ASM_STREAM_CLOSE == pSession->aStreams[i].state) )
      {
         pSession->ucStreamId2Index[stream_id] = i;
         return &pSession->aStreams[i];
      }
   }

   return NULL;
}

void ASM_ReleaseStream(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id)
{
   uint8_t index = pSession->ucStreamId2Index[stream_id];
   if(index == ASM_INVALID_INDEX)
   {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: trying to release a released stream, stream index set to %lu", pSession->ucInternalSessionId, stream_id, (uint32_t)index);
      return;
   }
   pSession->aStreams[index].state = ASM_STREAM_CLOSE;
   pSession->ucStreamId2Index[stream_id] = ASM_INVALID_INDEX;
}

/**
 * release a stream based on whether it's first gapless stream. Only first gapless stream is kept in gapless close.
 */
void ASM_ReleaseGaplessStream(AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream)
{
   /** Do not completely close the first gapless stream*/
   if (pStream == pSession->pFirstGaplessStrm)
   {
      pStream->state = ASM_STREAM_GAPLESS_CLOSE;
      //do not mark index as invalid, as we need to access it for events coming from PP, MtMx which still use its nodes
      //although this is wrong because the stream at that index ideally should be reuseable for another stream_id.
   }
   else
   {
      ASM_ReleaseStream(pSession, pStream->ucStreamId);
   }
}

AudioStreamMgr_StreamStateType *ASM_GetStreamInstance(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id)
{
   uint8_t index = pSession->ucStreamId2Index[stream_id];
   if (ASM_INVALID_INDEX != index)
   {
      return &(pSession->aStreams[index]);
   }
   else
   {
      return NULL;
   }
}

/**
 * if a stream is in gapless close state it's as good as not open although
 * it has PP, Mtx nodes being used.
 */
uint32_t ASM_GetNumStreamsOpen(AudioStreamMgr_SessionStateType *pSession)
{
   uint32_t count = 0;
   if (NULL == pSession)
   {
      return count;
   }

   for (uint32_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if ((ASM_STREAM_CLOSE != pSession->aStreams[i].state) &&
            (ASM_STREAM_GAPLESS_CLOSE != pSession->aStreams[i].state))
      {
         count++;
      }
   }
   return count;
}

/**
 *
 */
uint32_t ASM_GetNumStreamsConnectedToRxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession)
{
   uint32_t count = 0;
   for (uint32_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if (ASM_STREAM_CLOSE != pSession->aStreams[i].state)
      {
         if ( (pSession->aStreams[i].nodes[ASM_NODE_RX_MTX].pSvcHandle) || (pSession->aStreams[i].nodes[ASM_NODE_RX_ST_RTR].pSvcHandle))
         {
            count++;
         }
      }
   }
   return count;
}

/**
 * should be called if only one stream is connected to matrix or stream router. If not, the first one is returned.
 */
AudioStreamMgr_StreamStateType *ASM_GetStreamConnectedToTxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession)
{
   for (uint32_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if (ASM_STREAM_CLOSE != pSession->aStreams[i].state)
      {
         if ( (pSession->aStreams[i].nodes[ASM_NODE_TX_MTX].pSvcHandle) || (pSession->aStreams[i].nodes[ASM_NODE_TX_ST_RTR].pSvcHandle))
         {
            return &pSession->aStreams[i];
         }
      }
   }
   return NULL;
}

/**
 * should be called if only one stream is connected to matrix or stream router. If not, the first one is returned.
 */
AudioStreamMgr_StreamStateType *ASM_GetStreamConnectedToRxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession)
{
   for (uint32_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if (ASM_STREAM_CLOSE != pSession->aStreams[i].state)
      {
         if ( (pSession->aStreams[i].nodes[ASM_NODE_RX_MTX].pSvcHandle) || (pSession->aStreams[i].nodes[ASM_NODE_RX_ST_RTR].pSvcHandle))
         {
            return &pSession->aStreams[i];
         }
      }
   }
   return NULL;
}

AudioStreamMgr_StreamStateType *ASM_GetAnOpenedStream(AudioStreamMgr_SessionStateType *pSession)
{
   AudioStreamMgr_StreamStateType *pStream = NULL;
   for (uint32_t i=0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      if ((ASM_STREAM_CLOSE != pSession->aStreams[i].state) &&
            (ASM_STREAM_GAPLESS_CLOSE != pSession->aStreams[i].state))
      {
         pStream = &pSession->aStreams[i];
      }
   }
   return pStream;
}

struct ASM_StreamTypeCounts_t
{
   uint8_t clientToDeviceStreams;
   uint8_t gaplessStreams;
   uint8_t otherStreams;
} ;

//currently supports Rx session & loopback as only they can have more than 1 stream now.

static void ASM_CountStreamTypes(AudioStreamMgr_SessionStateType *pSession, ASM_StreamTypeCounts_t *pCounts)
{
   memset(pCounts, 0, sizeof(ASM_StreamTypeCounts_t));

   uint8_t i, ind;
   AudioStreamMgr_StreamStateType *pStream;

   for (i = 1; i <= ASM_MAX_SUPPORTED_STREAM_ID; i++)
   {
      ind = pSession->ucStreamId2Index[i];
      if (ind == ASM_INVALID_INDEX)
      {
         continue;
      }

      pStream = &pSession->aStreams[ind];

      if ( (ASM_STREAM_CLOSE == pStream->state) ||
            (ASM_STREAM_GAPLESS_CLOSE == pStream->state) )
      {
         continue;
      }

      switch(pStream->pTopo->TopoId)
      {
      case ASM_TOPO_RX_REGULAR:
      case ASM_TOPO_RX_ULL:
      case ASM_TOPO_RX_ULLPP:
      case ASM_TOPO_RX_PULL_ULL:
      case ASM_TOPO_RX_PULL_ULLPP:
      case ASM_TOPO_RX_COMPR:
         pCounts->clientToDeviceStreams++;
         break;
      case ASM_TOPO_RX_GAPLESS:
         pCounts->gaplessStreams++;
         break;
      default:
         pCounts->otherStreams++;
         break;
      }
   }
   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Num streams: other %u, client2dev %u, gapless %u ",
         pSession->ucInternalSessionId, pCounts->otherStreams,pCounts->clientToDeviceStreams,
         pCounts->gaplessStreams);

   return ;
}

static ADSPResult ASM_CheckStreamPerfModeConsistency(AudioStreamMgr_SessionStateType *pSession)
{
   uint8_t i, ind;
   AudioStreamMgr_StreamStateType *pStream, *pAnotherStream = NULL;

   for (i = 1; i <= ASM_MAX_SUPPORTED_STREAM_ID; i++)
   {
      ind = pSession->ucStreamId2Index[i];
      if (ind == ASM_INVALID_INDEX)
      {
         continue;
      }

      pStream = &pSession->aStreams[ind];

      if ( (ASM_STREAM_CLOSE == pStream->state) ||
            (ASM_STREAM_GAPLESS_CLOSE == pStream->state) )
      {
         continue;
      }

      if (pAnotherStream == NULL)
      {
         pAnotherStream = pStream;
         continue;
      }

      if (pAnotherStream->stream_perf_mode != pStream->stream_perf_mode)
      {
         MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: stream id %u has perf mode %u but stream id %u has %u",
               pSession->ucInternalSessionId, pAnotherStream->ucStreamId, pAnotherStream->stream_perf_mode, pStream->ucStreamId, pStream->stream_perf_mode);
         return ADSP_EUNSUPPORTED;
      }
   }
   return ADSP_EOK;
}

static uint16_t ASM_GetTotalStreams(ASM_StreamTypeCounts_t *pStreamCounts)
{
   return pStreamCounts->clientToDeviceStreams+pStreamCounts->gaplessStreams+
         pStreamCounts->otherStreams;
}

ADSPResult ASM_CheckIntraSessionTopoErrors(AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EOK;

   ASM_StreamTypeCounts_t streamCounts;

   ASM_CountStreamTypes(pSession, &streamCounts);

   uint16_t total = ASM_GetTotalStreams(&streamCounts);

   //bounds checks:
   if ( (total > ASM_MAX_SUPPORTED_STREAMS_PER_SESSION) ||
         (streamCounts.otherStreams > 0 ) ||
         (streamCounts.gaplessStreams > ASM_MAX_STREAMS_GAPLESS_SESSION) ||
         (streamCounts.clientToDeviceStreams > ASM_MAX_CLIENT_TO_DEVICE_STREAMS) )
   {
      return ADSP_EFAILED;
   }

   //if gapless is true, no other streams can exist.
   if ((streamCounts.gaplessStreams > 0) && (total != streamCounts.gaplessStreams))
   {
      return ADSP_EFAILED;
   }

   //if regular rx or ULL is true, no other streams can exist.
   if ((streamCounts.clientToDeviceStreams > 0) && (total != streamCounts.clientToDeviceStreams) )
   {
      return ADSP_EFAILED;
   }

   //if no MIMO dec, no Combiner exists, or no gapless or no splitter then num of stream cannot be more than 1
   //this check is ensured in bounds check.

   //no need to check for states as we allow streams to be opened in run state now.

   if (ADSP_FAILED(result = ASM_CheckStreamPerfModeConsistency(pSession)))
   {
      return result;
   }

   return result;
}

/**
 * returns whether the nodes with given handle is already present in the pSession->aTempStreamIdNodeIndexList or not.
 */
static bool_t ASM_IsNodeAlreadyPresentInTempStreamIdNodeIndexList(AudioStreamMgr_SessionStateType *pSession, elite_svc_handle_t *node_handle)
{
   uint32_t i=0, j=0;
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION ; i++)
   {
      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[i].stream_id);
      if (NULL == pStream) break;

      for (j = 0; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
      {
         ASM_GraphNodeIndex nodeInd = pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[j];

         if (nodeInd == ASM_INVALID_NODE) break;

         ASM_GraphNode *node = &pStream->nodes[nodeInd];

         if (node->pSvcHandle == node_handle)
         {
            return true;
         }
      }
   }

   return false;
}

/**
 * Since path delay is a session cmd, all the streams contributing to the session delay have to be
 * populated in temp struct aTempStreamIdNodeIndexList first.
 *
 * note: only stream is populated. to populate node, call ASM_StoreNodesInTempStreamIdNodeIndexList
 */
ADSPResult ASM_SortStreamsForPathDelay(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EOK;

   AudioStreamMgr_StreamStateType *pStream = NULL;

   AudioStreamMgr_StreamStateType *paTempStreams[ASM_MAX_SUPPORTED_STREAMS_PER_SESSION];
   memset(paTempStreams, 0, sizeof(paTempStreams));

   uint32_t gapless = 0;
   uint32_t others = 0, i;

   //if gapless, then keep the first gapless stream at index 0 (this node contributes to delay from switch,pp,device)
   if (ASM_IsGaplessSession(pSession))
   {
      paTempStreams[gapless] = pSession->pFirstGaplessStrm;
      gapless++;
   }

   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      pStream = &pSession->aStreams[i];

      if ( (ASM_STREAM_CLOSE == pStream->state) ||
            (ASM_STREAM_GAPLESS_CLOSE == pStream->state)) //pFirstGaplessStrm continues if it's in GAPLESS_CLOSE state
      {
         continue;
      }

      switch(pStream->pTopo->TopoId)
      {
      case ASM_TOPO_RX_GAPLESS:
         // for gapless 2 kinds of streams are in picture a) open stream b) pFirstGaplessStrm
         // In #a, only oldestOpenStreamId is to be used & this contributes to delay from decoder
         // #b, contributes to delay from switch, PP, matrix, device side - since the nodes used are from pFirstGaplessStrm
         // it's possible that a & b point to the same stream in which case array will have only one element.
         if ( (pStream->ucStreamId == pSession->oldestOpenStreamId) &&
               (pStream->ucStreamId != pSession->pFirstGaplessStrm->ucStreamId)&&(gapless<ASM_MAX_SUPPORTED_STREAMS_PER_SESSION))
         {
            paTempStreams[gapless] = pStream;
            gapless++;
         }
         break;
      case ASM_TOPO_NT_MODE:
      case ASM_TOPO_RX_REGULAR:
      case ASM_TOPO_RX_ULL:
      case ASM_TOPO_RX_ULLPP:
      case ASM_TOPO_TX_REGULAR:
      case ASM_TOPO_TX_LLNP:
      case ASM_TOPO_PCM2PCM_LOOPBACK:
      case ASM_TOPO_PCM2COMPR_LOOPBACK:
      case ASM_TOPO_RX_COMPR:
      case ASM_TOPO_TX_COMPR:
      case ASM_TOPO_RX_PULL:
      case ASM_TOPO_RX_PULL_ULL:
      case ASM_TOPO_RX_PULL_ULLPP:
      case ASM_TOPO_TX_PUSH:
      case ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK:
      case ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK:
      case ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK:
      case ASM_TOPO_COMPR2PCM_LOOPBACK:
         //in general use delays from all existing streams.
         paTempStreams[others] = pStream;
         others++;
         break;
      case ASM_INVALID_TOPO:
      default:
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Num streams: topo id %u not support supported fot get path delay",
               pSession->ucInternalSessionId, pStream->pTopo->TopoId);
         break;
      }
   }

   if (others > 1)
   {
      return ADSP_EFAILED;
   }

   //store the stream into instance var aTempStreamIdNodeIndexList
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      pStream = paTempStreams[i];
      if (pStream)
      {
         pSession->aTempStreamIdNodeIndexList[i].stream_id = pStream->ucStreamId;
      }
      else
      {
         pSession->aTempStreamIdNodeIndexList[i].stream_id = 0;
      }
   }

   return result;
}

/**
 * Sorts streams and populates into pSession->aTempStreamIdNodeIndexList
 *
 * note: only stream is populated. to populate node, call ASM_StoreNodesInTempStreamIdNodeIndexList
 */
ADSPResult ASM_SortStreams(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession, ASM_STREAM_SORT_METHOD method)
{
   ASM_StreamTypeCounts_t streamCounts;

   ASM_CountStreamTypes(pSession, &streamCounts);

   uint32_t gapless = 0;

   uint32_t otherStreams = 0, i;
   uint32_t totalStreams = ASM_GetTotalStreams(&streamCounts);

   AudioStreamMgr_StreamStateType *pStream;

   AudioStreamMgr_StreamStateType *paTempStreams[ASM_MAX_SUPPORTED_STREAMS_PER_SESSION];

   memset(paTempStreams, 0, sizeof(paTempStreams));

   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      pStream = &pSession->aStreams[i];

      if ( (ASM_STREAM_CLOSE == pStream->state) ||
            (ASM_STREAM_GAPLESS_CLOSE == pStream->state))
      {
         continue;
      }

      switch(pStream->pTopo->TopoId)
      {
      case ASM_TOPO_RX_GAPLESS:
         paTempStreams[gapless] = pStream; //order doesn't matter.
         gapless++;
         break;
      case ASM_TOPO_NT_MODE:
      case ASM_TOPO_RX_REGULAR:
      case ASM_TOPO_RX_ULL:
      case ASM_TOPO_RX_ULLPP:
      case ASM_TOPO_TX_REGULAR:
      case ASM_TOPO_PCM2PCM_LOOPBACK:
      case ASM_TOPO_PCM2COMPR_LOOPBACK:
      case ASM_TOPO_RX_COMPR:
      case ASM_TOPO_TX_COMPR:
      case ASM_TOPO_RX_PULL:
      case ASM_TOPO_RX_PULL_ULL:
      case ASM_TOPO_RX_PULL_ULLPP:
      case ASM_TOPO_TX_PUSH:
      case ASM_TOPO_COMPR2COMPR_GEN_LOOPBACK:
      case ASM_TOPO_COMPR2COMPR_CONV_LOOPBACK:
      case ASM_TOPO_COMPR2COMPR_DIRECT_LOOPBACK:
      case ASM_TOPO_COMPR2PCM_LOOPBACK:
      case ASM_INVALID_TOPO:
      default:
         paTempStreams[0] = pStream; //only one stream allowed.
         otherStreams++;
         break;
      }
   }

   if (otherStreams > 1 || totalStreams > ASM_MAX_SUPPORTED_STREAMS_PER_SESSION)
   {
      return ADSP_EFAILED;
   }

   //above sorts from source to sink, if sink to source reverse array.
   if (ASM_STREAM_SORT_SINK_TO_SRC == method)
   {
      for (i = 0; i < totalStreams/2; i++)
      {
         pStream = paTempStreams[totalStreams - 1 - i];
         paTempStreams[totalStreams - 1 - i] = paTempStreams[i] ;
         paTempStreams[i] = pStream;
      }
   }

   //store the stream into instance var aTempStreamIdNodeIndexList
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION; i++)
   {
      pStream = paTempStreams[i];
      if (pStream)
      {
         pSession->aTempStreamIdNodeIndexList[i].stream_id = pStream->ucStreamId;
      }
      else
      {
         pSession->aTempStreamIdNodeIndexList[i].stream_id = 0;
      }
   }

   return ADSP_EOK;
}

//#define DEBUG_NODE_LISTING

void ASM_StoreNodesInTempStreamIdNodeIndexList(AudioStreamMgr_SessionStateType *pSession, ASM_topo_type topo_type)
{
#ifdef DEBUG_NODE_LISTING
   char buf[100]; uint32_t bufind=0;
#endif
   //Initialize
   uint32_t i=0, j=0, k=0;
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION ; i++)
   {
      for (j = 0; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
      {
         pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[j] = ASM_INVALID_NODE;
      }
   }

   //look up if node is already present and store accordingly
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION ; i++)
   {
      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[i].stream_id);
      if (NULL == pStream) break;
      k=0;

      for (j = 0; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
      {
         ASM_GraphNodeIndex nodeInd = pStream->pTopo->CmdTopo[topo_type].svc_nodes[j];

         if (nodeInd == ASM_INVALID_NODE) continue;

         ASM_GraphNode *node = &pStream->nodes[nodeInd];

         bool_t present = ASM_IsNodeAlreadyPresentInTempStreamIdNodeIndexList(pSession, node->pSvcHandle);
         if (!present)
         {
            pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[k] = nodeInd; k++; //k is needed to make nodes contiguous

#ifdef DEBUG_NODE_LISTING
            if(bufind < 100)
            {
               int nn=snprintf(&(buf[bufind]),100-bufind,"(%u,%u) ",pSession->aTempStreamIdNodeIndexList[i].stream_id,nodeInd);
               bufind+=nn;
            }
#endif

         }
      }
   }

#ifdef DEBUG_NODE_LISTING
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [-,-]: Cmd will be issued in this order (stream-id,node-ind)=%s",buf);
#endif
}

/**
 * if curr node is NULL, it starts from beginning.
 *
 * Generally this is useful for pause1 & query delay which are sent parallely
 *
 */
ADSPResult ASM_IterateCmdOverNodesAndStreamsParallely(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      ASM_ExecRunPauseSuspendFn execFn)
{
   ADSPResult result = ADSP_EOK;

   uint32_t i=0, j=0;
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION ; i++)
   {
      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[i].stream_id);
      if (NULL == pStream) break;

      for (j = 0; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
      {
         ASM_GraphNodeIndex node_ind = pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[j];
         if (ASM_INVALID_NODE == node_ind) break;

         result |= execFn(node_ind, pMe, pSession, pStream);
      }
   }

   return result;
}

/**
 * stream_id_node_ind contains both input as well as output
 *
 * Assumed that the nodes in this are contiguous
 */
void ASM_GetNextSortedStreamAndNode(AudioStreamMgr_SessionStateType *pSession,
      ASM_StreamIdNodeIndex_t *stream_id_node_ind)
{
   bool_t node_found = false, stream_found = false;
   ASM_StreamIdNodeIndex_t ret;
   ret.stream_id = 0; ret.node_ind = ASM_INVALID_NODE;

   uint32_t i=0, j=0;
   for (i = 0; i < ASM_MAX_SUPPORTED_STREAMS_PER_SESSION ; i++)
   {
      node_found = false;
      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[i].stream_id);
      if (NULL == pStream)
      {
         *stream_id_node_ind = ret;
         return;
      }

      //if stream was already found in prev iter,
      //then first node of this stream is the node to be returned if its not invalid
      if (stream_found)
      {
         ASM_GraphNodeIndex node_ind = pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[0];
         if (node_ind == ASM_INVALID_NODE)
         {
            continue; //look for the next stream
         }
         stream_id_node_ind->stream_id = pStream->ucStreamId;
         stream_id_node_ind->node_ind = node_ind;
         return;
      }

      if (pStream->ucStreamId == stream_id_node_ind->stream_id)
      {
         //once stream is found, look for nodes in this stream
         stream_found = true;
         for (j = 0; j < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; j++)
         {
            ASM_GraphNodeIndex node_ind = pSession->aTempStreamIdNodeIndexList[i].cmd_node_ind[j];
            //if sent node is invalid, then return first node if its not invalid
            //if node was found in prev iter, then return this node if its not invalid
            if (node_found || (stream_id_node_ind->node_ind == ASM_INVALID_NODE))
            {
               if (node_ind == ASM_INVALID_NODE) //if next node is invalid, go to next stream by breaking inner loop.
               {
                  break;
               }
               else
               {
                  stream_id_node_ind->stream_id = pStream->ucStreamId;
                  stream_id_node_ind->node_ind = node_ind;
                  return;
               }
            }

            if ( stream_id_node_ind->node_ind == node_ind)
            {
               node_found = true;
            }
         }
      }
   }

   *stream_id_node_ind = ret;
   return;
}

/**
 * if curr node is NULL, it starts from beginning.
 *
 * done serially.
 */
ADSPResult ASM_IterateRunPauseSuspendOverNodesAndStreams(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_GraphNodeIndex curr_node_index,
      ASM_ExecRunPauseSuspendFn execFn)
{
   ASM_StreamIdNodeIndex_t stream_id_node_ind;

   if(NULL == pStream) return ADSP_EFAILED;

   stream_id_node_ind.stream_id = pStream->ucStreamId;
   stream_id_node_ind.node_ind = curr_node_index;

   ASM_GetNextSortedStreamAndNode(pSession, &stream_id_node_ind);

   pStream = ASM_GetStreamInstance(pSession, stream_id_node_ind.stream_id);

   if ((ASM_INVALID_NODE != stream_id_node_ind.node_ind) && pStream)
   {
      return execFn(stream_id_node_ind.node_ind, pMe, pSession, pStream);
   }
   else
   {
      return ADSP_EOK;
   }
}

/**
 * Send a setparam message from ASM to Rx matrix
 * to set Rendering decision and clock recovery
 * at matrix input port
 */
ADSPResult AudioStreamMgr_StreamSendMtMXMsgForRateMatching(AudioStreamMgr_t * pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   uint8_t stream_id;

   //if ( !pStream || (AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState) || (ASM_STREAM_CLOSE == pStream->state) )
   if ( !pStream || (ASM_STREAM_CLOSE == pStream->state) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u, %u]: Trying to set mtmx param on a invalid/closed session!",pSession->ucInternalSessionId,  pStream?pStream->ucStreamId:99);
      return ADSP_EFAILED;
   }

   stream_id = pStream->ucStreamId;

   ASM_GraphNode *asmNode;
   ASM_GraphNodeIndex asmNodeIndex;
   //asm_mtmx_ratematching_set_params RenderingDecisionParamPayload;

   //ASM_direction dir = (ASM_direction)pSession->unMode;
   ASM_direction dir = ASM_RX_DIR;

   /** this call also makes sure valid session with matrix stream router exists and valid direction is set.*/
   ASM_MtxStRtrType mtMxOrStRtr  = ASM_GetMatrixOrStreamRouterNode(pStream, dir, &asmNodeIndex);

   if ( (ASM_NODE_MAX == asmNodeIndex))
   {
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Cannot set params. either stream is closed or there's no matrix/stream router in this session. state=%d,node-ind=0x%lx,mode=%d",
            pSession->ucInternalSessionId,  stream_id, pSession->unState, (uint32_t)asmNodeIndex, pSession->unMode);
      return ADSP_EFAILED;
   }

   asmNode = &pStream->nodes[asmNodeIndex];

   uint32_t  unPayloadSize;
   elite_svc_handle_t *pSvcHandle   = asmNode->pSvcHandle;
   elite_msg_any_t msg;
   EliteMsg_CustomMtMxSetRateMatchingParamType *pParamMtMxSetRateMatchingParam;

   unPayloadSize = sizeof( EliteMsg_CustomMtMxSetRateMatchingParamType);
   if(ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         0,0 )))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASM: Failed to create setparam mesage to send to MtMx");
      return result;
   }

   pParamMtMxSetRateMatchingParam = (EliteMsg_CustomMtMxSetRateMatchingParamType*)msg.pPayload;
   pParamMtMxSetRateMatchingParam->unPortDirection  = pStream->mtMxPortParam[dir].unPortDirection;
   pParamMtMxSetRateMatchingParam->unPortID         = pStream->mtMxPortParam[dir].unPortID;
   pParamMtMxSetRateMatchingParam->unSecOpCode      = ELITEMSG_CUSTOM_MT_MX_SET_RATEMATCHING_PARAM;
   pParamMtMxSetRateMatchingParam->renderingDecison = (uint32_t)ASM_SESSION_MTMX_STRTR_PARAM_RENDER_DECISION_LOCAL_STC;
   pParamMtMxSetRateMatchingParam->ClkRecoveryMode  = (uint32_t)ASM_SESSION_MTMX_STRTR_PARAM_CLOCK_RECOVERY_INDEPENDENT;

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(result))
   {
      //return message to buffer queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: FAILED to send internal SetParam command to matrix %d",
            pSession->ucInternalSessionId,  stream_id, mtMxOrStRtr);
      return ADSP_EFAILED;
   }

   return result;
}

ADSPResult ASM_RunNewlyOpenedStream(AudioStreamMgr_t * pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream)
{
   ADSPResult result = ADSP_EOK ;

   //pSession->prev_run_cmd will be used

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Since the session is run state, executing run command for this stream.", pSession->ucInternalSessionId,  pStream->ucStreamId);

   memset(pSession->aTempStreamIdNodeIndexList, 0, sizeof(pSession->aTempStreamIdNodeIndexList));

   pSession->aTempStreamIdNodeIndexList[0].stream_id = pStream->ucStreamId;
   uint32_t k=0;
   for (uint32_t i=0; i<AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if (pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_RUN_TOPO].svc_nodes[i] != ASM_INVALID_NODE)
      {
         pSession->aTempStreamIdNodeIndexList[0].cmd_node_ind[k] = pStream->pTopo->CmdTopo[ASM_TOPO_TYPE_RUN_TOPO].svc_nodes[i];
         k++;
      }
   }

   result = ASM_IterateRunPauseSuspendOverNodesAndStreams(pMe, pSession, pStream, ASM_INVALID_NODE, ASM_ExecRunFTableEntry);

   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: FAILED to send Run command 0x%lx!!",
            pSession->ucInternalSessionId,  (uint32_t) result);
   }

   return result;
}

bool_t AudioStreamMgr_StreamTopoIsTxTopo(AudioStreamMgr_StreamStateType *pStream)
{
   if (NULL == pStream)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr_StreamTopoIsTxTopo received NULL pStream ptr");
      return FALSE;
   }

   for (uint32_t i=0; i<AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if ((pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_TX_MTX) ||
            (pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_TX_ST_RTR) ||
            (pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_TX_GENERIC))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [-,%u]:  found node %d as part of Tx topo.",
               pStream->ucStreamId, pStream->pTopo->StrmSvcInfo.svc_nodes[i]);
         return TRUE;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [-,%u]:   Stream is not part of a Tx topo.", pStream->ucStreamId);
   return FALSE;
}

/**
 * returns delay ptr of the svc that has variable delay after decoder.
 *
 * currently only candidate is switch svc.
 *
 * pStreams is first gapless stream because other streams dont have SWITCH SVC node.
 */
volatile uint32_t* AudioStreamMgr_GetPostDecSvcVarBufDelay(AudioStreamMgr_StreamStateType *pStream)
{
   if (NULL == pStream)
   {
      return NULL;
   }

   uint32_t i=0;
   for (i=0; i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM; i++)
   {
      if ((pStream->pTopo->StrmSvcInfo.svc_nodes[i] == ASM_NODE_DEC) )
      {
         break;
      }
   }

   if (i < AUDIO_STREAM_MGR_MAX_SVC_PER_STREAM-1)
   {
      if (pStream->pTopo->StrmSvcInfo.svc_nodes[i+1] == ASM_NODE_SWITCH)
      {
         return &pStream->nodes[ASM_NODE_SWITCH].buf_delay;
      }
   }

   return NULL;
}

ADSPResult AudioStreamMgr_SendRegisterEventCmdToDynaSvc(AudioStreamMgr_t* pMe, AudioStreamMgr_StaticSessionStateType *pStSession,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex nodeIndex,
      elite_apr_packet_t *pPkt)
{
   ADSPResult status = ADSP_EOK;
   uint32_t unPayloadSize = sizeof(elite_msg_register_apr_event_t);
   elite_msg_register_apr_event_t *pMsgPayload = NULL;
   //use the same message structure as encoder cfg
   uint32_t ulClientToken = ASM_PopulateClientToken(pStream->ucStreamId, nodeIndex);
   elite_msg_any_t msg;

   if(ADSP_FAILED(status = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CMD_REGISTER_APR_EVENT,
         pStSession->pSessionRespQ,
         ulClientToken,
         0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: RegisterEncDecEvents: Failed to create message in register events handler",
            pSession->ucInternalSessionId, pStream->ucStreamId  );

      //ack with error
      return ADSP_ENOMEMORY;
   }

   pMsgPayload = (elite_msg_register_apr_event_t *)msg.pPayload;
   pMsgPayload->unSize       = elite_apr_if_get_payload_size(pPkt);
   pMsgPayload->pnParamData  = (int32_t*)elite_apr_if_get_payload_ptr(pPkt);
   pMsgPayload->src_port     = elite_apr_if_get_src_port(pPkt);
   pMsgPayload->src_addr     = elite_apr_if_get_src_addr(pPkt);
   pMsgPayload->dest_port    = elite_apr_if_get_dst_port(pPkt);
   pMsgPayload->dest_addr    = elite_apr_if_get_dst_addr(pPkt);
   pMsgPayload->apr_token    = pPkt->token;

   //get svc command queue and push the msg into it
   status = ASM_PushCmdToPeerSvc(pMe, pSession, pStream->nodes[nodeIndex].pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(status))
   {
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Register Events:: Failed to send command",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
   }

   return status;
}

ADSPResult AudioStreamMgr_SendIEC61937RegisterEventCmdToDynaSvc(AudioStreamMgr_t* pMe, AudioStreamMgr_StaticSessionStateType *pStSession,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex nodeIndex,
      elite_apr_packet_t *pPkt)
{
   ADSPResult status = ADSP_EOK;

   uint32_t unPayloadSize = sizeof(elite_msg_register_apr_event_t) + \
                            sizeof(asm_stream_cmd_register_encdec_events_t)+ \
                            sizeof(asm_stream_cmd_register_encdec_event_t);

   elite_msg_register_apr_event_t *pMsgPayload = NULL;

   uint32_t ulClientToken = ASM_PopulateClientToken(pStream->ucStreamId, nodeIndex);
   elite_msg_any_t msg;
   int8_t *pMem = NULL;
   if(ADSP_FAILED(status = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CMD_REGISTER_APR_EVENT,
         pStSession->pSessionRespQ,
         ulClientToken,
         0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: RegisterEncDecEvents: Failed to create message in register events handler",
            pSession->ucInternalSessionId, pStream->ucStreamId  );

      //ack with error
      return ADSP_ENOMEMORY;
   }

   pMsgPayload = (elite_msg_register_apr_event_t *)msg.pPayload;
   pMsgPayload->unSize       = sizeof(asm_stream_cmd_register_encdec_events_t)+ sizeof(asm_stream_cmd_register_encdec_event_t);
   pMsgPayload->src_port     = elite_apr_if_get_src_port(pPkt);
   pMsgPayload->src_addr     = elite_apr_if_get_src_addr(pPkt);
   pMsgPayload->dest_port    = elite_apr_if_get_dst_port(pPkt);
   pMsgPayload->dest_addr    = elite_apr_if_get_dst_addr(pPkt);
   pMsgPayload->apr_token    = pPkt->token;
   pMsgPayload->pnParamData  = (int32_t*)((int8_t*)msg.pPayload + sizeof(elite_msg_register_apr_event_t));

   pMem = (int8_t*)msg.pPayload + sizeof(elite_msg_register_apr_event_t);

   //Convert IEC61937 payload to ENCDEC register event payload
   asm_cmd_rgstr_iec_61937_fmt_update_t *pIEC61937Payload = (asm_cmd_rgstr_iec_61937_fmt_update_t *)elite_apr_if_get_payload_ptr(pPkt);
   asm_stream_cmd_register_encdec_events_t *pEncDecRegisterHeader = (asm_stream_cmd_register_encdec_events_t *)pMem;

   pEncDecRegisterHeader->version = 0;
   pEncDecRegisterHeader->service_id = ASM_DECODER_SVC;
   pEncDecRegisterHeader->num_events = 1;

   asm_stream_cmd_register_encdec_event_t *pEncDecRegisterPayload = (asm_stream_cmd_register_encdec_event_t *)(pMem+sizeof(asm_stream_cmd_register_encdec_events_t));
   pEncDecRegisterPayload->config_mask = pIEC61937Payload->enable;
   pEncDecRegisterPayload->event_id = ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT_ID;


   //get svc command queue and push the msg into it
   status = ASM_PushCmdToPeerSvc(pMe, pSession, pStream->nodes[nodeIndex].pSvcHandle->cmdQ, (uint64_t*)&msg);

   if (ADSP_FAILED(status))
   {
      //return created msg to buffer return queue
      elite_msg_release_msg(&msg);
      qurt_elite_memory_free(pMem);
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Register Events:: Failed to send command",
            pSession->ucInternalSessionId,  pStream->ucStreamId);
   }
   return status;
}
