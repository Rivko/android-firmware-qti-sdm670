/*========================================================================

 *//** @file AudioDecSvc_CmdHandler.cpp
This file contains functions for Elite Decoder service.

Copyright (c) 2014-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_CmdHandler.cpp#2 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
02/12/14    rbhatnk      Created file.
02/07/18   npendeka      Modified
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"

#include "AudioDecSvc.h"
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc_MiMoUtil.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "audio_basic_op.h"
#include <audio_basic_op_ext.h>
#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "Interleaver.h"
#include "AudioDecSvc_ChanMap.h"
#include "AudioDecSvc_PullMode.h"
#include "adsp_mtmx_strtr_api.h"
#include "adsp_prv_avs_common_api.h"
#include "audio_module_chain_wrapper.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */
int AudioDecSvc_ConnectToService(AudioDecSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   EliteMsg_CustomDecConnectStreamType* pCmdMsgPayload = NULL;

   pCmdMsgPayload = (EliteMsg_CustomDecConnectStreamType*) ((pMe->cmdMsg).pPayload);

   AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetOutputStreamById(pMe, pCmdMsgPayload->stream_id);

   if (NULL == pOutStrm)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder service connecting when no output stream is opened for stream_id 0x%x!",pCmdMsgPayload->stream_id);

      elite_svc_send_ack( &(pMe->cmdMsg), ADSP_ENOTREADY);
      return ADSP_ENOTREADY;
   }

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //todo this won't work with MIMO

   elite_svc_handle_t *pSvcHandle = (elite_svc_handle_t *)pCmdMsgPayload->pDownStreamHandle;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder service connecting to down stream service. unCurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   if (NULL != pOutStrm->pDownStreamSvc)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Connection FAILED, Downstream service is already connected");
      result = ADSP_ENOTREADY;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Connection Succeeded.");
      pOutStrm->pDownStreamSvc = pSvcHandle;

      if (pInpStrm)
      {
         // If (ULL(PP) or LLNP) and AudioDecSvc_HasDecodingBegun(pMe)
         if(((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode)
               || (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pInpStrm->common.perf_mode)
               || (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode))
               && AudioDecSvc_HasDecodingBegun(pMe)) //the checks doesn't work for MIMO or CAPI chaining. but ULL doesn't need them
         {
            result = AudioDecSvc_NotifyPeerSvcWithMediaFmtUpdate(pMe,pInpStrm, pOutStrm,
                  pInpStrm->pcmFmt.ulPCMSampleRate,
                  pOutStrm->out_chan_map.num_out_chan,
                  pOutStrm->out_chan_map.out_chan,
                  pOutStrm->output_bits_per_sample);
            if (ADSP_FAILED(result))
            {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc:Fail to send Media Fmt update downstream during Connect");
            }
         }

         //Flush any data from circular buffer and update the media format to downstream.
         (void)AudioDecSvc_CircBuf_Flush(&pOutStrm->CircBufList, pMe, pOutStrm);

         //pCmdMsgPayload->unResponseResult = AUD_CONNECT_PP_TO_DEC_SERVICE_CMD_ACK;

         //First time when downstream service is connected, Start listen to outputQ
         if(DEC_SVC_PULL_MODE == pMe->ulMode)
         {
        	 AudioDecSvc_ListenToOutput(pMe);
         }

         //for PULL-ULL mode, after device switch, we need to set bit mask so that dec is in the proper state (run/pause)
         if (( (DEC_SVC_PULL_MODE == pMe->ulMode) &&
               ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode) ||
                     (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode))))
         {
            if (pMe->isPrevBitfieldValid)
            {
               pMe->unCurrentBitfield = pMe->unPreviousBitfield;
               pMe->isPrevBitfieldValid = false;
            }
            //if not valid, no change in bit mask. next run/pause will change the bit mask as needed.
         }
         else
         {
            if(pInpStrm->inpDataQMsg.pPayload)
            {
               AudioDecSvc_ListenToOutput(pMe);
            }
            else
            {
               //in case of pull-mode, since we don't have inp dataQ added to the channel, waiting has not effect.
               AudioDecSvc_ListenToInput(pMe);
            }
         }
      }
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder service done connecting to down stream service. unCurrentBitfield=0x%x with result=0x%x", (unsigned int)pMe->unCurrentBitfield, result);

   elite_svc_send_ack( &(pMe->cmdMsg), result);

   return ADSP_EOK;
}

int AudioDecSvc_DisconnectFromService(AudioDecSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_cmd_connect_t* pCmdMsgPayload = NULL;
   elite_svc_handle_t *pSvcHandle = NULL;

   pCmdMsgPayload = (elite_msg_cmd_connect_t*)(pMe->cmdMsg).pPayload;
   pSvcHandle = (elite_svc_handle_t* )pCmdMsgPayload->pSvcHandle;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder service disconnecting to down stream service. unCurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   uint16_t stream_index;
   bool_t conn_found = false;
   for (stream_index=0; stream_index < DEC_SVC_MAX_OUTPUT_STREAMS; stream_index++)
   {
      if (pMe->out_streams_ptr[stream_index] && (pMe->out_streams_ptr[stream_index]->pDownStreamSvc == pSvcHandle))
      {
         conn_found = true;
         break;
      }
   }

   if (!conn_found)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Disonnection FAILED: \
                                            Service Handle 0x%lx not matching any record",
                                            (uint32_t)pSvcHandle );
      elite_svc_send_ack( &(pMe->cmdMsg), result);
      return ADSP_EBADPARAM;
   }
   //else accept the dis-connection
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Disonnection Succeeded.\n");
      pMe->out_streams_ptr[stream_index]->pDownStreamSvc = NULL;
   }

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //todo this won't work with MIMO
   if ((DEC_SVC_PULL_MODE == pMe->ulMode) &&
         ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode) ||
               (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode)))
   {
      AudioDecSvc_SetPrevBitField(pMe);
   }

   //Stop listening to this output stream.
   AudioDecSvc_StopListenToMask(pMe,AUD_DEC_OUT_INDEX_TO_DATA_SIG(stream_index));

   //If not listening to any other output stream then
   if(0 == AudioDecSvc_GetAllOutputStreamMask(pMe))
   {
	   //check if we are waiting for any EoS ack from AFE, if so, AFE/PP is gonna discard them so raise now
	   //(check is done inside)
	   AudioDecSvc_RaiseRenderedEoSv2(pMe, NULL);

	   /*
	   since we are disconnected, we can no longer process input or output buffers.
	   so just listen to command queue
		*/

	   AudioDecSvc_ListenToControl(pMe);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder service done disconnecting to down stream service. unCurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   elite_svc_send_ack( &(pMe->cmdMsg), result);

   return ADSP_EOK;
}


// Because we do not want CAPI has visibility to adsp APIs, we keep this function here instead of
// moving it inside CAPI

ADSPResult AudioDecSvc_Flush(AudioDecSvc_t* pMe)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc : Executing flush Command, CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   //TODO: introduce stream_id in flush cmd sent from ASM.
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

   // clean up data queue
   if (pInpStrm->common.serviceHandle.dataQ)
   {
      AudioDecSvc_FlushInputDataQ(pMe, pInpStrm);
   }

   //after flush EoS_V2 must be dropped. just mark as not waiting
   pInpStrm->bWaitingForEoSRespFromAfe = FALSE;

   //reset decoder
   AudioDecSvc_ResetDecoder(pMe);

   //listen to input data queue now
   AudioDecSvc_ListenToInput(pMe);
   pMe->isPrevBitfieldValid = false; //after flush we come out of suspend state automatically & shouldnt preserve prev mask.

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc : Done executing flush Command, CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);
   /* Generate ACK to Flush cmd */
   return elite_msg_finish_msg(&(pMe->cmdMsg), ADSP_EOK);
}

/**
 * start suspend command during long pause durations to suspend active hardwares
 * */
ADSPResult AudioDecSvc_StartSuspend(AudioDecSvc_t* pMe)
{
   //TODO: to handle multiple suspends issued by ASM for MIMO.

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc : Start Executing Suspend Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   return AudioDecSvc_FinishSuspend(pMe);
}

/**
 * finish suspend command during long pause durations to suspend active hardwares
 * */
ADSPResult AudioDecSvc_FinishSuspend(AudioDecSvc_t* pMe)
{
   ADSPResult result = ADSP_EOK;

   if (DEC_SVC_PULL_MODE == pMe->ulMode)
   {
      for (uint16_t i=0; i<DEC_SVC_MAX_INPUT_STREAMS; i++)
      {
         if (pMe->in_streams_ptr[i] && (pMe->ulMode == DEC_SVC_PULL_MODE))
         {
            result |= AudioDecSvc_SuspendPullMode(pMe, pMe->in_streams_ptr[i]);
         }
      }
   }

   //for early-eos, timer is used. but that timer is allowed to expire (may delay XO shutdown) even at suspend.

   // no need to change the bit field as DML is no longer present.
   // DML had below logic: why not input: becoz it will switch on DML [DML no longer exist, but logic is preserved]
   // why not output: output switches input => DML

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc : done executing Suspend Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   if (ADSP_FAILED(result)) result = ADSP_EFAILED;

   result = elite_msg_finish_msg(&(pMe->cmdMsg), result);

   //release=true => force_vote is don't care.
   (void)AudioDecSvc_ProcessKppsBw(pMe, TRUE, FALSE);

   return result;
}

/**
 * Decoder is paused for all playback usecases, as Pause is implememented in two stages.
   only pause2 would be issued to decoder. Pause1 ACK ensure PP has been paused.
 */
ADSPResult AudioDecSvc_Pause(AudioDecSvc_t* pMe)
{
   //TODO: to handle multiple pauses issued by ASM for MIMO.
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[PAUSE] Executing pause Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   {
      AudioDecSvc_SetPrevBitField(pMe);

      //listen to only commands (don't process data)
      AudioDecSvc_ListenToControl(pMe);
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[PAUSE] Executing pause Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   /* Generate ACK to pause cmd */
   return elite_msg_finish_msg(&(pMe->cmdMsg), ADSP_EOK);
}

/**
 * Applicable only to pull mode and ULL performance mode
 * added support for wake up from suspend command
 */
ADSPResult AudioDecSvc_Run(AudioDecSvc_t* pMe)
{
   //TODO: to handle multiple runs issued by ASM for MIMO.
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[RUN] Executing Run Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   // 1. for PULL mode start listening to output buf at run.
   // 2. for ULL & regular mode, set it to what it was before.
   //    corner cases: previously we might be listening to any of 1. Input, 2. output & cmd.
   //    connect -> process -> run (prev mask not used)
   //    pause -> run (prev mask not used except for ULL.)
   //    pause -> suspend -> run (prev mask used)
   //    pause -> flush -> run (prev mask used)
   //    pause -> flush/suspend -> run (prev mask used)

   if (DEC_SVC_PULL_MODE == pMe->ulMode)
   {
      AudioDecSvc_ListenToOutput(pMe);
   }
   else if (pMe->isPrevBitfieldValid)
   {
      pMe->unCurrentBitfield = pMe->unPreviousBitfield;
      pMe->isPrevBitfieldValid = false;
   }

   if(DEC_SVC_PULL_MODE == pMe->ulMode) //in case of pull and ULL then also this IF cond works..
   {
      for (uint16_t i=0; i<DEC_SVC_MAX_INPUT_STREAMS; i++)
      {
         if (pMe->in_streams_ptr[i] && (pMe->ulMode == DEC_SVC_PULL_MODE))
         {
            result = AudioDecSvc_ResumePullMode(pMe, pMe->in_streams_ptr[i]);
         }
      }
   }
   if (ADSP_FAILED(result)) result = ADSP_EFAILED;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioDecSvc :[RUN] Done Executing Run Command. CurrentBitfield=0x%x", (unsigned int)pMe->unCurrentBitfield);

   /* Generate ACK to Flush cmd */
   result = elite_msg_finish_msg(&(pMe->cmdMsg), result);

   //release=false. force vote as there's probably no event now (suspend->run)
   (void)AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);

   return result;
}

ADSPResult AudioDecSvc_SetParam(AudioDecSvc_t *pMe)
{
   //check the param ID first
   ADSPResult res = ADSP_EOK;
   elite_msg_param_header_t *pParamHdr = (elite_msg_param_header_t *)pMe->cmdMsg.pPayload;
   uint32_t ulParamId = pParamHdr->unParamId;

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO: this won't work with MIMO (need stream_id in setparam)
   AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe); //TODO: this won't work with MIMO (need stream_id in setparam)

   switch (ulParamId)
   {
   case ELITEMSG_PARAM_ID_CAL:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *)pMe->cmdMsg.pPayload;

      asm_stream_cmd_set_encdec_param_t *pEncDecParam = (asm_stream_cmd_set_encdec_param_t*)pParamMsg->pnParamData;
      uint8_t *pSetParamPayload = ( (uint8_t*)pEncDecParam + sizeof(asm_stream_cmd_set_encdec_param_t) );

      switch (pEncDecParam->param_id)
      {
      case ASM_PARAM_ID_REG_DEC_ERR_EVENT:
      {
         AudioDecSvc_DecErrEvent_SetParam(&pInpStrm->dec_err_event, pSetParamPayload);

         break;
      }
      case ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP:
      {
         asm_dec_out_chan_map_param_t *pDecOutChanMapMsg = (asm_dec_out_chan_map_param_t *)pSetParamPayload;
         res = AudioDecSvc_ProcessOutChanMapCmd(pMe, pOutStrm, pDecOutChanMapMsg);
         break;
      }

      case ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION:
      {
         res = AudioDecSvc_EarlyEoS_Create(pMe, pOutStrm, pSetParamPayload);
         break;
      }
      default:
      {
         res = aud_dec_svc_set_param(pMe->mainCapiContainer->capi_ptr, pEncDecParam->param_id, (int8_t*)pSetParamPayload, pEncDecParam->param_size);
         break;
      }
      }
      if(ADSP_FAILED(res))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "error setting param. fmt=0x%x,ulParamId=0x%x,res=0x%x", (unsigned int)pMe->mainCapiContainer->dec_fmt_id, (unsigned int)pEncDecParam->param_id, res);
         res = ADSP_EFAILED;
      }
      break;
   }
   case AVS_DECODER_PARAM_ID_DEC_MEDIA_FMT:
   {
	  elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->cmdMsg.pPayload;
      dec_CAPI_container_t *main_capi = pMe->mainCapiContainer;
      res = AudioDecSvc_UpdateMediaFmt(pMe, pInpStrm, main_capi, (uint8_t *)pParamMsg->pnParamData, pParamMsg->unSize);
      break;
   }
   case AVS_ENCDEC_PARAM_ID_DEC_TO_ENC_COMMUNICATION:
   {
	  elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->cmdMsg.pPayload;
	  if (pParamMsg->unSize < sizeof(dec_to_enc_communication_payload_t))
	  {
		  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: dec_set_param()- received wrong key size for IMC communication");
		  res = ADSP_EFAILED;
		  break;
	  }
	  res= AudioDecSvc_IMCCheckInit(pMe, pParamMsg);
	  break;
   }
   default:
   {
      elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *) pMe->cmdMsg.pPayload;
      res = aud_dec_svc_set_param(pMe->mainCapiContainer->capi_ptr, ulParamId, (int8_t*)pParamMsg->pnParamData, pParamMsg->unSize);
      break;
   }
   }
   return elite_msg_finish_msg(&pMe->cmdMsg, res);
}



ADSPResult AudioDecSvc_ReturnUnsupported (AudioDecSvc_t* pMe)
{
   //unsupported command..finish msg with error code
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported command with opcode = 0x%lx", pMe->cmdMsg.unOpCode);
   return elite_msg_finish_msg(&pMe->cmdMsg, ADSP_EUNSUPPORTED);
}


ADSPResult AudioDecSvc_DtmfHandler(AudioDecSvc_t *pMe)
{
   uint32_t bDtmfRunning=0;
   //DtmfInfo_t *pDtmfInfo = &(pMe->DtmfInfo);
   EliteMsg_CustomDtmfParamsType* pDtmfPayload = NULL;

   pDtmfPayload = (EliteMsg_CustomDtmfParamsType*) ((pMe->cmdMsg).pPayload);

   aud_dec_svc_get_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_RUNNING, (int8_t*)&bDtmfRunning, sizeof(bDtmfRunning));

   //If second tone comes while the previous one is running then need to send the
   //End of tone event for the previous tone. All the info of src port and dst port are
   //stored in DtmfEndToneInfo structure.
   // pDtmfPayload->duration != 0 this check is necessary becasue we will get the DTMF
   //command for stopping the infinite tone. Then we should not send unnecesarily the end of tone event
   //from here. Event will be sent after ramping down the tone.
   if((TRUE == bDtmfRunning) && (pDtmfPayload->duration != 0) )
   {

      AudioDecSvc_SendDtmfToneEndedEvent(pMe);

   }

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO: this won't work with MIMO (need stream_id in DTMF)

   //store APR Info of the new tone
   pInpStrm->DtmfEndToneInfo  =  pDtmfPayload->dtmfAprInfo;

   bool_t truth = TRUE;

   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_HIGH_TONE, (int8_t*)&pDtmfPayload->tone_1, sizeof(pDtmfPayload->tone_1));
   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_LOW_TONE, (int8_t*)&pDtmfPayload->tone_2, sizeof(pDtmfPayload->tone_2));
   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_DURATION, (int8_t*)&pDtmfPayload->duration, sizeof(pDtmfPayload->duration));
   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_GAIN, (int8_t*)&pDtmfPayload->gain, sizeof(pDtmfPayload->gain));
   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_SET, (int8_t*)&truth, sizeof(truth));
   aud_dec_svc_set_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_RUNNING, (int8_t*)&truth, sizeof(truth));

   AudioDecSvc_ListenToOutput(pMe);

   return elite_msg_finish_msg(&(pMe->cmdMsg), ADSP_EOK);
}

void AudioDecSvc_SendDtmfToneEndedEvent(AudioDecSvc_t *pMe)
{
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO: this won't work with MIMO (need stream_id in DTMF)
   AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe); //TODO: this won't work with MIMO (need stream_id in DTMF)

   elite_msg_any_t toneEndMsg;
   uint32_t unToneEndPayloadSize = sizeof( elite_msg_data_eos_apr_t );

   elite_msg_create_msg( &toneEndMsg, &unToneEndPayloadSize,
         ELITE_DATA_EOS,
         NULL,
         0,
         0 );

   elite_msg_data_eos_apr_t *pToneEndPayload = (elite_msg_data_eos_apr_t *) (toneEndMsg.pPayload);
   pToneEndPayload->unEosFormat   = ELITEMSG_DATA_DTMF_EOT;
   pToneEndPayload->eosInfo  =  pInpStrm->DtmfEndToneInfo;


   //fill up the EOS ack event opcode so that services like AFE can blindly raise it
   pToneEndPayload->eosInfo.event_opcode = ASM_STREAM_EVENT_DTMF_TONE_ENDED;

   qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)&toneEndMsg);
}

#define GET_REG_STATUS(x) ((x & ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__MASK) \
      >> ASM_STREAM_CMD_REGISTER_ENCDEC_EVENTS_CONFIG_MASK_ENABLE__SHIFT)

ADSPResult AudioDecSvc_RegisterAprEvents (AudioDecSvc_t* pMe)
{
   //check the param ID first
   ADSPResult result = ADSP_EOK;
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO: this won't work with MIMO (need stream_id in setparam)
   //AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe); //TODO: this won't work with MIMO (need stream_id in setparam)

   uint32_t num_events, payload_size;
   uint8_t *data_ptr;
   uint32_t i = 0;
   asm_stream_cmd_register_encdec_event_t *reg_payload_ptr;


   elite_msg_register_apr_event_t *pParamMsg =
         (elite_msg_register_apr_event_t *) pMe->cmdMsg.pPayload;

   aud_dec_svc_event_query_payload_t received_payload;
   received_payload.cb.selfPort = pParamMsg->dest_port;
   received_payload.cb.selfAddr = pParamMsg->dest_addr;
   received_payload.cb.clientPort = pParamMsg->src_port;
   received_payload.cb.clientAddr = pParamMsg->src_addr;
   received_payload.cb.unAsynClientToken = pParamMsg->apr_token;
   received_payload.cb.aprHandle = pInpStrm->common.CallbackHandle.aprHandle; //randomly choose since all apr handles point to same ASM apr handle

   asm_stream_cmd_register_encdec_events_t *pCmd = (asm_stream_cmd_register_encdec_events_t *)pParamMsg->pnParamData;
   uint8_t *payload_ptr = (uint8_t*)pCmd ;

   if(NULL == pCmd)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: received NULL pointer for the payload while processing Register Events command");
      result = ADSP_EBADPARAM;
      goto _bailout;
   }

   if ( (pParamMsg->unSize < sizeof(asm_stream_cmd_register_encdec_events_t)) || (0 != pCmd->version) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: received incorrect payload size of %lu or unsupported version %lu", pParamMsg->unSize, pCmd->version);
      result = ADSP_EBADPARAM;
      goto _bailout;
   }

   num_events   = pCmd->num_events;
   data_ptr     = payload_ptr + sizeof(asm_stream_cmd_register_encdec_events_t);
   payload_size = pParamMsg->unSize - sizeof(asm_stream_cmd_register_encdec_events_t);

   // check for payload size
   if(payload_size <  (num_events * sizeof(asm_stream_cmd_register_encdec_event_t )))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: incorrect payload_size after number of events %lu", payload_size );
      result = ADSP_EBADPARAM;
      goto _bailout;
   }

   reg_payload_ptr = (asm_stream_cmd_register_encdec_event_t*) data_ptr;

   for (i = 0; i < num_events; i++)
   {
      bool_t need_to_register = GET_REG_STATUS(reg_payload_ptr->config_mask);
      received_payload.event_id = reg_payload_ptr->event_id;
      received_payload.event_bit_field = reg_payload_ptr->config_mask;

      aud_dec_svc_event_node_t *existing_node = aud_dec_svc_find_node(&received_payload, &pMe->event_list);

      capi_v2_t *capi_ptr = pMe->mainCapiContainer->capi_ptr;
      if(ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT_ID == received_payload.event_id)
      {
         //for IEC 61937 media format send the event registration to first capi instead of dec capi
         capi_ptr = pMe->capiContainer[0]->capi_ptr;
      }

      bool_t was_registered = FALSE;
      if (existing_node)
      {
         was_registered = GET_REG_STATUS(existing_node->payload.event_bit_field);
      }

      if ( !was_registered && need_to_register )
      {
         result |= aud_dec_svc_reg_dereg_apr_events(capi_ptr, reg_payload_ptr->event_id, need_to_register);

         if (NODE_SUCCESS != aud_dec_svc_event_insert_node(&received_payload, &pMe->event_list))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: failed to register %lu th APR event 0x%lx",
                  i, reg_payload_ptr->event_id, reg_payload_ptr->config_mask);
            result |= ADSP_EFAILED;
         }
      }
      else if ( was_registered && !need_to_register )
      {
         result |= aud_dec_svc_reg_dereg_apr_events(capi_ptr, reg_payload_ptr->event_id, need_to_register);

         if (NODE_SUCCESS != aud_dec_svc_event_delete_node(&received_payload, &pMe->event_list))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: failed to deregister %lu th APR event 0x%lx",
                  i, reg_payload_ptr->event_id, reg_payload_ptr->config_mask);
            result = ADSP_EFAILED;
         }
      }
      else //if (!( was_registered ^ need_to_register ))
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "audio_dec_svc: no change in event registration status for %lu th APR event. prev mask 0x%lx, new mask  0x%lx",
                           i, reg_payload_ptr->event_id, received_payload.event_bit_field, reg_payload_ptr->config_mask);
      }

      reg_payload_ptr++;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"audio_dec_svc : register/deregister %lu APR events done. result %lu", i, result);

_bailout:
   return elite_msg_finish_msg(&pMe->cmdMsg, result);

}

ADSPResult AudioDecSvc_SetPpParam(AudioDecSvc_t* pMe)
{
   ADSPResult res = ADSP_EOK;
   EliteMsg_CustomDecSetGetPpParamType *pParamMsg = (EliteMsg_CustomDecSetGetPpParamType *)pMe->cmdMsg.pPayload;

   if(NULL == pMe->pUll_modules)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc_SetPpParam is supported only for PCM ULL modules");
       res = ADSP_EUNSUPPORTED;
       return elite_msg_finish_msg(&pMe->cmdMsg, res);
   }

   uint32_t ulParamId = pParamMsg->unParamId;

   switch (ulParamId)
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         res = audio_module_chain_wrapper_set_param(pMe->pUll_modules, pParamMsg->pnParamData, pParamMsg->unSize, ulParamId);
         AudioDecSvc_ProcessKppsBw(pMe, FALSE, TRUE);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc_SetPpParam Unsupported param ID = 0x%lx",ulParamId);
         res = ADSP_EUNSUPPORTED;
         break;
      }
   }

   return elite_msg_finish_msg(&pMe->cmdMsg, res);

}

ADSPResult AudioDecSvc_GetPpParam(AudioDecSvc_t* pMe)
{
   ADSPResult res = ADSP_EOK;
   EliteMsg_CustomDecSetGetPpParamType *pParamMsg = (EliteMsg_CustomDecSetGetPpParamType *)pMe->cmdMsg.pPayload;

   if(NULL == pMe->pUll_modules)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc_GetPpParam is supported only for PCM ULL modules");
       res = ADSP_EUNSUPPORTED;
       return elite_msg_finish_msg(&pMe->cmdMsg, res);
   }

   uint32_t ulParamId = pParamMsg->unParamId;

   switch (ulParamId)
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         res = audio_module_chain_wrapper_get_param(pMe->pUll_modules, pParamMsg->pnParamData, &pParamMsg->unSize, pParamMsg->unSize, ulParamId);
         break;
      }
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc_GetPpParam Unsupported param ID = 0x%lx",ulParamId);
         res = ADSP_EUNSUPPORTED;
         break;
      }
   }

   return elite_msg_finish_msg(&pMe->cmdMsg, res);

}
