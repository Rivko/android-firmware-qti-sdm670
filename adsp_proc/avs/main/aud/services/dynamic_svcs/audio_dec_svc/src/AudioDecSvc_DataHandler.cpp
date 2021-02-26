/*========================================================================

 *//** @file AudioDecSvc_DataHandler.cpp
This file contains functions for Elite Decoder service.

Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_DataHandler.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
02/12/14    rbhatnk      Created file.

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
#include "audio_basic_op.h"
#include <audio_basic_op_ext.h>
#include "adsp_asm_api.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_GetSetBits.h"
#include "Interleaver.h"
#include "AudioDecSvc_ChanMap.h"
#include "AudioDecSvc_PullMode.h"
#include "adsp_mtmx_strtr_api.h"
#include "AudioDecSvc_CapiV2CallbackHandler.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"

#include "adsp_asm_stream_commands.h"

//#define AUD_DEC_SVC_CAPI_IO_SIZE_LOG
//#define AUDENCDEC_TIMESTAMP_LOGS
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

// Main processing function for Decoding the bitstream data
static ADSPResult AudioDecSvc_Process(AudioDecSvc_t *pMe, uint8_t capi_index);
static ADSPResult AudioDecSvc_ProcessLoop(AudioDecSvc_t *pMe, ADSPResult *prevDecResult);
//after process of all CAPIs.
static ADSPResult AudioDecSvc_PostProcess(AudioDecSvc_t *pMe,  AudioDecSvc_OutStream_t *pOutStrm, elite_msg_any_t *media_fmt_msg);

static ADSPResult AudioDecSvc_ProcessOutputDataQForCAPIs(AudioDecSvc_t* pMe,AudioDecSvc_OutStream_t *pOutStrm);
static ADSPResult AudioDecSvc_FillCapiInpBuffer(AudioDecSvc_t* pMe,uint8_t capi_index);
static bool_t AudDecSvc_IsInputPCM(Dec_CAPI_Type capi_type);

static bool_t AudDecSvc_IsOutputPCM(Dec_CAPI_Type capi_type);
static ADSPResult AudioDecSvc_ProcessOutputDataQ_PCMPlayback(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, bool_t release_partial);
static ADSPResult AudioDecSvc_ProcessPCMNonEmptyOutputBuf(AudioDecSvc_t *pMe);
static ADSPResult AudioDecSvc_ReleasePCMNonEmptyOutputBuf(AudioDecSvc_t *pMe);
static void AudioDecSvc_ProcessEos(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, bool_t bResetDecoder);
static void AudioDecSvc_SendMetadata(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm);
static void AudioDecSvc_HandleEosCases(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, bool_t outBufNotEmpty);
static void AudioDecSvc_Reset_SecondaryInpStream(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);
static ADSPResult AudioDecSvc_HandleResync(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
static void AudioDecSvc_Process_UpdateCapiInputTS(dec_CAPI_container_t *capi_container, int32_t nBytesUsed, AudioDecSvc_InpStream_t * pInpStrm, uint8_t index);

/*
 * sends metadata, output buf, and EoS.
 */
static ADSPResult AudioDecSvc_SendOutBufs(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm);

static bool_t AudDecSvc_IsInputPCM(Dec_CAPI_Type capi_type)
{
   switch(capi_type)
   {
      case DEC_CAPI_TYPE_DEPACKETIZER:
      case DEC_CAPI_TYPE_COP_DEPACKETIZER:
      {
         return TRUE;
      }
      default:
      {
         return FALSE;
      }
   }
}

static bool_t AudDecSvc_IsOutputPCM(Dec_CAPI_Type capi_type)
{
   switch(capi_type)
   {
      case DEC_CAPI_TYPE_AUTO:
      case DEC_CAPI_TYPE_PACKETIZER:
      case DEC_CAPI_TYPE_COP_PACKETIZER:
      {
         return TRUE;
      }
      default:
      {
         return FALSE;
      }
   }
}

static void AudioDecSvc_Process_UpdateCapiInputTS(dec_CAPI_container_t *capi_container, int32_t nBytesUsed, AudioDecSvc_InpStream_t * pInpStrm, uint8_t index)
{
   uint64_t bps_i, bps_o, offset;
   uint32_t bytes_per_sample;

   if( capi_container->inputs[index].flags.is_timestamp_valid )
   {
      if(AudDecSvc_IsInputPCM(capi_container->capi_type) )
      {
         //use input media forma. capi_container->PrevFmtBlk stores output media format
         bps_i = pInpStrm->pcmFmt.ulPCMSampleRate * pInpStrm->pcmFmt.usBytesPerSample * pInpStrm->pcmFmt.chan_map.nChannels;
         offset = 0;
         if(bps_i)
         {
            offset = ((uint64_t)nBytesUsed*1000000)/bps_i;
         }
         capi_container->inputs[index].timestamp += offset;
#ifdef AUDENCDEC_TIMESTAMP_LOGS
         MSG_3(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Incrementing input for new TS [%lu %lu], offset=%ld",
               (uint32_t)(capi_container->inputs[index].timestamp>>32), (uint32_t)capi_container->inputs[index].timestamp, (uint32_t)offset);
#endif
      }

      //If current capi consumes only partial input and generates output then adjust time stamp based on output samples generates.
      //assumption here - as everywhere - is that the capi does do internal buffering and cosumes only 1 complete buffer
      if(AudDecSvc_IsOutputPCM(capi_container->capi_type) && (capi_container->out_buf[index].actual_data_len != 0) )
      {
         //input PCM can be the first CAPI only, so use input media forma. capi_container->PrevFmtBlk stores output media format
         bytes_per_sample = (capi_container->PrevFmtBlk[0].bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;
         bps_o = capi_container->PrevFmtBlk[0].sample_rate * bytes_per_sample * capi_container->PrevFmtBlk[0].num_channels;
         offset = 0;
         if(bps_o)
         {
            offset = ((uint64_t)capi_container->out_buf[index].actual_data_len*1000000)/bps_o;
         }
         capi_container->inputs[index].timestamp += offset;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
         MSG_7(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Incrementing input for new TS [%lu %lu], offset=%ld, outsize %ld, SR %ld CH %ld bps %ld",
               (uint32_t)(capi_container->inputs[index].timestamp>>32), (uint32_t)capi_container->inputs[index].timestamp, (uint32_t)offset,
               capi_container->out_buf[index].actual_data_len, capi_container->PrevFmtBlk[0].sample_rate, capi_container->PrevFmtBlk[0].num_channels,
               bytes_per_sample);
#endif
      }
   }
}

static void AudioDecSvc_SetBitFieldAfterDataBufferCommand(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
    //Stop listening to this input data Q for which Data buffer is received.
    uint32_t curInpStrmMask = AUD_DEC_INP_INDEX_TO_DATA_SIG(AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm));
    AudioDecSvc_StopListenToMask(pMe, curInpStrmMask);

    //Current input mask in bit-field.
    uint32_t curInpMaskInCurBitfield = (pMe->unCurrentBitfield & AudioDecSvc_GetAllInputStreamMask(pMe));

    //Starts listening to all output data Q, if not waiting on any input Q
    if(0 == curInpMaskInCurBitfield)
    {
        AudioDecSvc_ListenToOutput(pMe);
    }
}

static void AudioDecSvc_SetBitFieldAfterEOSCommand(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
	//Starts listening to all output data Q if EOS received on non-secondary input stream.
	if(pInpStrm->common.stream_type != ASM_STREAM_TYPE_SECONDARY)
	{
		AudioDecSvc_ListenToOutput(pMe);
	}
	else
	{
		AudioDecSvc_SetBitFieldAfterDataBufferCommand(pMe,pInpStrm);
	}
}

static ADSPResult AudioDecSvc_ProcessInputDataQ_PeerService(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;

   //process messages
   switch ( pInpStrm->inpDataQMsg.unOpCode )
   {
   case ELITE_DATA_BUFFER:
   {
      elite_msg_data_buffer_t* pInpBuf = (elite_msg_data_buffer_t*) pInpStrm->inpDataQMsg.pPayload;

      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "buf_recv_cnt is %d",pMe->buf_recv_cnt);
      //checking on whether we are waiting for a media format command is to be done in the CAPI V2.

      // if input buffer do not contain integer PCM samples per channel, return it immediately with error code
      if( AudioDecSvc_IsPcmFmt(pMe->capiContainer[0]->dec_fmt_id) )  //check first CAPI because it accepts inputs.
      {
         uint32_t unit_size = pInpStrm->pcmFmt.chan_map.nChannels * pInpStrm->pcmFmt.usBytesPerSample;

         if ( pInpBuf->nActualSize % unit_size )
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Returning an input buffer that do not contain the same PCM samples on all channel!");
            return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EBADPARAM);
         }
      }

      pInpStrm->inp_buf_params.unVirtAddr = (uint32_t)&pInpBuf->nDataBuf;
      pInpStrm->inp_buf_params.unMemSize = pInpBuf->nActualSize;

      pInpStrm->inp_buf_params_backup = pInpStrm->inp_buf_params;

      AudioDecSvc_SetBitFieldAfterDataBufferCommand(pMe, pInpStrm);
	  
      //set EoF flag to False in the CAPI, since only the last copy from input buffer
      //assures an integral number of frames in the decoder internal buffer
      dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
      first_capi->inputs[AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm)].flags.end_of_frame = false;

      //get time stamp info from the input buffer
      (void) AudioDecSvc_FillTsStateFromInBuf(&pInpStrm->TsState,
            pMe->mainCapiContainer->dec_fmt_id,
            pInpBuf->ullTimeStamp,
            asm_get_timestamp_valid_flag(pInpBuf->nFlag),   //bit 31.. so works
            FALSE, FALSE,
            pInpStrm->WasPrevDecResNeedMore);

      result = AudioDecSvc_ProcessPCMNonEmptyOutputBuf(pMe);

      pInpStrm->common.total_bytes += pInpBuf->nActualSize;

      break;
   }
   case ELITE_DATA_MEDIA_TYPE:
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Decoder svc media format update cmd from peer service");

      AudioDecSvc_ReleasePCMNonEmptyOutputBuf(pMe);

      return AudioDecSvc_UpdateMediaFmt_PeerService(pMe, pInpStrm);

      break;
   }
   case ELITE_DATA_EOS:
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Decoder svc received EOS cmd from peer service");

      pInpStrm->endOfStream =  AUD_DEC_EOS_V1;

      AudioDecSvc_SetBitFieldAfterEOSCommand(pMe, pInpStrm);

      result = AudioDecSvc_ProcessPCMNonEmptyOutputBuf(pMe);

      break;
   }
   default:
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudDecSvc Rx'd unsupported message!! ");
      result = AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EUNSUPPORTED);
      break;
   }
   }
   return (result);
}

static ADSPResult AudioDecSvc_ProcessInputDataQ_AprClient(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult nResult = ADSP_EOK;
   uint32_t uAprOpCode;
   elite_apr_packet_t* pAprPacket;

   //get the payload of the msg
   pAprPacket = (elite_apr_packet_t*) (pInpStrm->inpDataQMsg.pPayload);

   uAprOpCode = elite_apr_if_get_opcode( pAprPacket ) ;

   switch (uAprOpCode)
   {
   //Input data(bitstream) command
   case ASM_DATA_CMD_WRITE_V2:
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "buf_recv_cnt is %d",pMe->buf_recv_cnt);
      //first check whether we are waiting for a media format command

      asm_data_cmd_write_v2_t *pDataPayload;
      pDataPayload = (asm_data_cmd_write_v2_t *)elite_apr_if_get_payload_ptr(pAprPacket);

      //If payload of input data msg is not valid, return
      if (NULL==pDataPayload)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cannot get APR data payload");
         //return the buffer and wait for new ones
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EBADPARAM);
      }

      //if input buffer is zero size, return it immediately
      if(0 == pDataPayload->buf_size)
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Returning an input buffer of zero size!");
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
      }
      //if input buffer is not 32 byte aligned return it
      if(pDataPayload->buf_addr_lsw & MASK_32BYTE_ALIGNMENT)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Input buffer not 32 byte aligned, returning it!");
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EBADPARAM);
      }

      // if input buffer do not contain integer PCM samples per channel, return it immediately with error code
      if( AudioDecSvc_IsPcmFmt(pMe->capiContainer[0]->dec_fmt_id) ) //enough to check first CAPI as it is the one accepting inputs.
      {
         uint32_t unit_size = pInpStrm->pcmFmt.chan_map.nChannels * pInpStrm->pcmFmt.usBytesPerSample;

         if ( pDataPayload->buf_size % unit_size )
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Returning an input buffer that do not contain the same PCM samples on all channel!");
            return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EBADPARAM);
         }
      }

      /*if this compressed pass-through session then check if frame rate has been received*/
      if(DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937 == pInpStrm->io_fmt_conv)
      {
         if(!pInpStrm->is_frame_rate_set)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Compressed pass-thru mode and frame rate not received! Returning buffer");
            return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EUNEXPECTED);
         }
      }

      pInpStrm->inp_buf_params.unMemMapHandle = pDataPayload->mem_map_handle;
      //Convert the physical address of the shared memory buffer to virtual address
      if (ADSP_FAILED(nResult = elite_mem_map_get_shm_attrib_ref_counted(pDataPayload->buf_addr_lsw,pDataPayload->buf_addr_msw,
            pDataPayload->buf_size,
            &(pInpStrm->inp_buf_params) )))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioDecSvc:Phy to Virt Failed(paddr,vaddr)-->(%lx%lx,%lx)\n",
               pInpStrm->inp_buf_params.unPhysAddrMsw,
               pInpStrm->inp_buf_params.unPhysAddrLsw,
               pInpStrm->inp_buf_params.unVirtAddr);
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EBADPARAM);
      }

      //Backing up inp_buf_params for cache invalidation in FreeInputDataCmd
      pInpStrm->inp_buf_params_backup = pInpStrm->inp_buf_params;

      //invalidate the cache before reading from shared memory.
      elite_mem_invalidate_cache( &(pInpStrm->inp_buf_params) );

      AudioDecSvc_SetBitFieldAfterDataBufferCommand(pMe, pInpStrm);

      AudioDecSvc_EarlyEoS_AddBuffersToCurrBitField(pMe);

      //get time stamp info from the input buffer
      (void) AudioDecSvc_FillTsStateFromInBuf(&pInpStrm->TsState,
            pMe->capiContainer[0]->dec_fmt_id,
            ((((uint64_t)pDataPayload->timestamp_msw)<<32)|(uint64_t)pDataPayload->timestamp_lsw),
            (bool_t) asm_get_timestamp_valid_flag(pDataPayload->flags),
            (bool_t) asm_get_ts_continue_flag(pDataPayload->flags),
            asm_get_eof_flag(pDataPayload->flags),
            pInpStrm->WasPrevDecResNeedMore);

      //get the end of frame flag
      pInpStrm->bEndOfFrame = asm_get_eof_flag(pDataPayload->flags);

      //set EoF flag to False in the CAPI, since only the last copy from input buffer
      //assures an integral number of frames in the decoder internal buffer
      dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
      first_capi->inputs[AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm)].flags.end_of_frame = false;

      //get last buffer flag
      pInpStrm->silence_removal.last_buffer = asm_get_last_buffer_flag(pDataPayload->flags);

      nResult = AudioDecSvc_ProcessPCMNonEmptyOutputBuf(pMe);

      break;

   } // case ASM_DATA_CMD_WRITE_V2, end of processiong client data buffer

   case ASM_DATA_CMD_EOS:
   case ASM_DATA_CMD_EOS_V2:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Decoder svc received EOS cmd 0x%lx!!!!", uAprOpCode);

      if (pInpStrm->bWaitingForEoSRespFromAfe)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder svc received EOS when another EoS is being processed", uAprOpCode);
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EALREADY);
      }

      pInpStrm->endOfStream =  (uAprOpCode==ASM_DATA_CMD_EOS)?AUD_DEC_EOS_V1:AUD_DEC_EOS_V2;

      AudioDecSvc_SetBitFieldAfterEOSCommand(pMe, pInpStrm);

      AudioDecSvc_EarlyEoS_AddBuffersToCurrBitField(pMe);

      nResult = AudioDecSvc_ProcessPCMNonEmptyOutputBuf(pMe);

      break;
   }

   case ASM_DATA_CMD_MARK_BUFFER_V2:
   {
      AudioDecSvc_ReleasePCMNonEmptyOutputBuf(pMe);

      elite_msg_any_t markBuffer;
      uint32_t unMarkBufferPayloadSize = sizeof( elite_msg_data_mark_buffer_t );
      ADSPResult nRes;
      nRes = elite_msg_create_msg( &markBuffer,
            &unMarkBufferPayloadSize,
            ELITE_DATA_MARK_BUFFER,
            NULL,
            0,
            0);
      if (ADSP_FAILED(nRes))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ERROR allocating Mark Buffer");
         //free input data cmd
         AudioDecSvc_DiscardMarkBuffer(pMe,pInpStrm);
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
      }
      asm_data_cmd_mark_buffer_v2_t *apr_payload = NULL;
      elite_apr_if_get_payload((void **)&apr_payload, pAprPacket);
      elite_msg_data_mark_buffer_t *pMarkBufferPayload = (elite_msg_data_mark_buffer_t *) (markBuffer.pPayload);

      pMarkBufferPayload->token_lsw = apr_payload->token_lsw;
      pMarkBufferPayload->token_msw = apr_payload->token_msw;
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Decoder svc received Marked Buffer %lu !!!!",apr_payload->token_lsw);
      AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe);
      if (ADSP_FAILED(qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)&markBuffer)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ERROR sending Mark Buffer downstream");
         AudioDecSvc_DiscardMarkBuffer(pMe,pInpStrm);
         elite_msg_release_msg(&markBuffer);
      }

      //free input data cmd
      return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
   }
   break;

   case ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2:
   {
      MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "Decoder svc media format update cmd!!!!");

      AudioDecSvc_ReleasePCMNonEmptyOutputBuf(pMe);

      //get media format update cmd payload
      asm_data_cmd_media_fmt_update_v2_t *pMediaFmt;
      pMediaFmt = (asm_data_cmd_media_fmt_update_v2_t *)elite_apr_if_get_payload_ptr(pAprPacket);

      nResult = ADSP_EFAILED;
      if(pMediaFmt)
      {
         uint32_t ulFmtBlkSize =
               ((asm_data_cmd_media_fmt_update_v2_t*)pMediaFmt)->fmt_blk_size;
         uint8_t *pFmtBlk = (uint8_t*)pMediaFmt + sizeof(asm_data_cmd_media_fmt_update_v2_t);

         dec_CAPI_container_t * first_capi = AudioDecSvc_GetFirstCapi(pMe);  //since only the first decoder receives the data path media fmt

         nResult = AudioDecSvc_UpdateMediaFmt(pMe, pInpStrm, first_capi, pFmtBlk, ulFmtBlkSize);
      }

      return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, nResult);
   }

   case ASM_DATA_CMD_REMOVE_INITIAL_SILENCE:
   {
      asm_data_cmd_remove_initial_silence_t *initial_silence_ptr;
      void *pDummy;
      elite_apr_if_get_payload(&pDummy, pAprPacket);
      initial_silence_ptr = (asm_data_cmd_remove_initial_silence_t *)pDummy;
      int decoder_delay=0;

      /*Only update if decoding has not yet begun*/
      if(!AudioDecSvc_HasDecodingBegun(pMe))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AudioDecSvc: received REMOVIE_INITIAL_SILENCE, samples to remove:%lu",
               initial_silence_ptr->num_samples_to_remove);
         pInpStrm->silence_removal.initial_samples_to_remove = initial_silence_ptr->num_samples_to_remove;

         decoder_delay = 0; //ignore failure
         aud_dec_svc_get_param(pMe->capiContainer[0]->capi_ptr,CAPI_PARAM_DECODER_DELAY,
               (int8_t*)&decoder_delay,sizeof(decoder_delay));

         pInpStrm->silence_removal.initial_samples_to_remove += decoder_delay;

         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioDecSvc:ASM_DATA_CMD_REMOVE_INITIAL_SILENCE recevied after decoding begun, ignoring");
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EFAILED);
      }
   }

   case ASM_DATA_CMD_REMOVE_TRAILING_SILENCE:
   {

      asm_data_cmd_remove_trailing_silence_t *trailing_silence_ptr;
      void *pDummy;
      int trailing_delay=0;
      elite_apr_if_get_payload(&pDummy, pAprPacket);
      trailing_silence_ptr = (asm_data_cmd_remove_trailing_silence_t *)pDummy;

      /*Only update if last buffer has not yet been processed*/
      if(!pInpStrm->silence_removal.last_buffer)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AudioDecSvc: received REMOVIE_TRAILING_SILENCE, samples to remove:%lu",
               trailing_silence_ptr->num_samples_to_remove);
         pInpStrm->silence_removal.trailing_samples_to_remove =  trailing_silence_ptr->num_samples_to_remove;

         trailing_delay = 0; //ignore failure.
         aud_dec_svc_get_param(pMe->capiContainer[0]->capi_ptr,CAPI_PARAM_TRAILING_DELAY,
               (int8_t*)&trailing_delay,sizeof(trailing_delay));

         if((trailing_silence_ptr->num_samples_to_remove-trailing_delay) > 0)
         {
            pInpStrm->silence_removal.trailing_samples_to_remove =  (trailing_silence_ptr->num_samples_to_remove-trailing_delay);
         }
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
      }
      else
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioDecSvc:ASM_DATA_CMD_REMOVE_TRAILING_SILENCE recevied after last buffer, ignoring");
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EFAILED);
      }

   }
   case ASM_DATA_CMD_IEC_60958_FRAME_RATE:
   case ASM_DATA_CMD_IEC_60958_MEDIA_FMT:
   {
      void *pDummy;
      elite_apr_if_get_payload(&pDummy, pAprPacket);
      asm_data_cmd_iec_60958_media_format_t media_fmt;

      if( ASM_DATA_CMD_IEC_60958_FRAME_RATE == uAprOpCode)
      {
         asm_data_cmd_iec_60958_frame_rate *frame_rate_ptr = (asm_data_cmd_iec_60958_frame_rate *)pDummy;
         media_fmt.sampling_rate = frame_rate_ptr->frame_rate;
         media_fmt.num_channels = 2; //set it to stereo
      }
      else
      {
         asm_data_cmd_iec_60958_media_format_t *media_fmt_ptr = (asm_data_cmd_iec_60958_media_format_t *)pDummy;
         media_fmt.sampling_rate = media_fmt_ptr->sampling_rate;
         media_fmt.num_channels = media_fmt_ptr->num_channels;
         
         if( 2 != media_fmt_ptr->num_channels && 8 != media_fmt_ptr->num_channels )
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc: Received invalid number of channels %lu ",
                   media_fmt_ptr->num_channels);
            return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EFAILED);
         }
      }

      /*This message should be received only in compressed pass through mode*/
      if(DEC_SVC_IO_FORMAT_CONV_TYPE_61937_TO_61937 != pInpStrm->io_fmt_conv)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc: Received media format in Non Pass-through case ");
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EFAILED);
      }
      else
      {
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Received sampling rate = %lu, num_channels = %lu",
               media_fmt.sampling_rate, media_fmt.num_channels);

         (void) aud_dec_svc_set_passthru_sampling_rate(pMe->capiContainer[0]->capi_ptr,
               AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm), media_fmt.sampling_rate, media_fmt.num_channels);

         pInpStrm->is_frame_rate_set = TRUE;
         return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
      }
   }
   case ASM_DATA_CMD_RESYNC:
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Received Resync cmd 0x%lx!!", uAprOpCode);

       AudioDecSvc_OutStream_t * pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe);
       uint32_t index = AUD_DEC_OUT_INDEX_TO_SIG_BIT_IND((AudioDecSvc_GetOutputStreamIndex(pMe,pOutStrm)));

       pInpStrm->TsState.bIsTSResync = TRUE;

       /* Call ProcessOutputDataQ to send down any pending buffers */
       (void)AudioDecSvc_ProcessOutputDataQ(pMe,index);

       ADSPResult result = AudioDecSvc_HandleResync(pMe,pOutStrm);
       if(ADSP_EOK != result)
       {
           /* No need to set the flag as resync command was not sent downstream */
           pInpStrm->TsState.bIsTSResync = FALSE;
       }
       return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, result);

   }

   default:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unexpected opCode for data write 0x%lx", uAprOpCode );
      return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EUNSUPPORTED);
   }
   } /* switch (uAprOpCode) */

   return nResult;
}

ADSPResult AudioDecSvc_ProcessInputDataQ(AudioDecSvc_t *pMe, uint32_t channelBitIndex)
{
   ADSPResult nResult = ADSP_EOK;

   QURT_ELITE_ASSERT(AUD_DEC_INP_SIG_BIT_IND_TO_INDEX(channelBitIndex) < DEC_SVC_MAX_INPUT_STREAMS);

   AudioDecSvc_InpStream_t *pInpStrm = pMe->in_streams_ptr[AUD_DEC_INP_SIG_BIT_IND_TO_INDEX(channelBitIndex)];

   // Take next msg off the q
   nResult = AudioDecSvc_GetInputDataCmd(pMe, pInpStrm);
   if(ADSP_EOK != nResult)
   {
      //here, failure to get input data msg could mean (a)that we already hold
      //on to a data msg but erroneously tried to dequeue a new one (b) non-APR message.
      //release the msg and maintain status quo.
      return AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, nResult);
   }

   if (ELITE_APR_PACKET != pInpStrm->inpDataQMsg.unOpCode)
   {
      return AudioDecSvc_ProcessInputDataQ_PeerService(pMe, pInpStrm);
   }
   else
   {
      return AudioDecSvc_ProcessInputDataQ_AprClient(pMe, pInpStrm);
   }

   return nResult;
}

/**
 * Function to process partially filled output buffer for PCM decoder
 *
 * PCM Decoder has only one output buffer. Decoder waits on it's
 * output buffer Q activity once it receives input buffer, if that
 * buffer is already popped in last input buffer process(and it was not
 * full due to input not multiple of 1ms (AFE frame size) and that buffer
 * should not be pushed back to output Q else that much data will be
 * lost when it is popped next time), then there is nothing in decoder output
 * buffer Q, which will cause no data processing as it will wait
 * forever on output buffer Q and that buffer Q is empty so it's deadlock.
 * To remove that deadlock, first previously popped partially filled output
 * should be processed and then wait for output buffer after sending
 * downstream if buffer is full else same sequence should be executed.
 */
static ADSPResult AudioDecSvc_ProcessPCMNonEmptyOutputBuf(AudioDecSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   if (AudioDecSvc_IsPcmFmt(pMe->mainCapiContainer->dec_fmt_id))
   {
      AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe);
      if (NULL != pOutStrm->outDataBufferNode.pBuffer)
      {
#ifdef EARLY_EOS_DEBUG
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EOS [0x%lx]: AudioDecSvc_ProcessPCMNonEmptyOutputBuf ", pOutStrm->common.ulDataLogId);
#endif

         elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) pOutStrm->outDataBufferNode.pBuffer;
         dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
         if(NULL == last_capi)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Failed to get last capi");
            return ADSP_EFAILED;
         }
         AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

         //if discontinuous TS then release previously partially filled buffer immediately
         bool_t need_to_release = AudioDecSvc_IsNextBufTsDiscontinuous(&pInpStrm->TsState, pOutBuf->nActualSize, &last_capi->PrevFmtBlk[0]);

         /* Output buffer is already popped, this needs to be processed first */
         result = AudioDecSvc_ProcessOutputDataQ_PCMPlayback(pMe, pOutStrm, need_to_release);

      }
   }
   return result;
}

static ADSPResult AudioDecSvc_ReleasePCMNonEmptyOutputBuf(AudioDecSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   if (AudioDecSvc_IsPcmFmt(pMe->mainCapiContainer->dec_fmt_id))
   {
      AudioDecSvc_OutStream_t *pOutStrm = AudioDecSvc_GetDefaultOutputStream(pMe);
      if (NULL != pOutStrm->outDataBufferNode.pBuffer)
      {
#ifdef EARLY_EOS_DEBUG
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EOS [0x%lx]: AudioDecSvc_ReleasePCMNonEmptyOutputBuf ", pOutStrm->common.ulDataLogId);
#endif

         /* Output buffer is already popped, this needs to be processed first */
         result = AudioDecSvc_ProcessOutputDataQ_PCMPlayback(pMe, pOutStrm, TRUE);
      }
   }
   return result;
}

static void AudioDecSvc_RaiseListenToEoSEventToAsm(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t msg;

   uint32_t payloadSize = sizeof(EliteEvent_CustomASMListenToEoSAck);
   result = elite_msg_create_msg(&msg, &payloadSize, ELITE_CUSTOM_EVENT, NULL, pInpStrm->common.eventToken, ADSP_EOK);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create custom event!");
      return ;
   }
   EliteEvent_CustomASMListenToEoSAck *pEvent = (EliteEvent_CustomASMListenToEoSAck*) msg.pPayload;
   pEvent->unSecOpCode = ELITE_EVENT_CUSTOM_ASM_LISTEN_TO_EOS_ACK;

   result = qurt_elite_queue_push_back(pInpStrm->common.pEventQ, (uint64_t*)&msg);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to push  custom event!");
      elite_msg_release_msg(&msg);
      return ;
   }

   return ;
}

static void AudioDecSvc_ProcessEos(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, bool_t bResetDecoder)
{
   AudioDecSvc_EosType_t eos_type = pInpStrm->endOfStream;

   if (bResetDecoder)
   {
      //reset the decoder
      AudioDecSvc_ResetDecoder(pMe);
   }

   //send EOS command downstream
   // Need to repacket this as an internal EOS command.

   qurt_elite_queue_t    *resp_q_ptr = NULL;
   bool_t eos_v2_for_ull = FALSE;
   if (eos_type == AUD_DEC_EOS_V2)
   {
      if ((ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInpStrm->common.perf_mode) ||
            (ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION == pInpStrm->common.perf_mode) ||
             (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pInpStrm->common.perf_mode))
      {
         resp_q_ptr = pMe->pRespQ;
         pMe->unCurrentBitfield |= AUD_DEC_RESP_SIG;
         eos_v2_for_ull = TRUE;
      }
   }

   elite_msg_any_t eosMsg;
   uint32_t unEosPayloadSize = sizeof( elite_msg_data_eos_apr_t );
   ADSPResult nRes;
   nRes = elite_msg_create_msg( &eosMsg,
         &unEosPayloadSize,
         ELITE_DATA_EOS,
         resp_q_ptr, //for eos_v2, dec will receive ack from downstream in ULL/LLNP/ULL-PP.
         (uint32_t) pInpStrm, //use inpStrm as client token
         0);
   if (ADSP_FAILED(nRes))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ERROR sending EOS downstream, out of buffer. client will not receive rendered EOS event from connected downstream!");
      return;
   }
   elite_msg_data_eos_apr_t *pEosPayload = (elite_msg_data_eos_apr_t *) (eosMsg.pPayload);
   pEosPayload->unEosFormat   = ELITEMSG_DATA_EOS_APR  ;

   //Extract client info from apr packet
   elite_msg_extract_client_info( (elite_apr_packet_t *) (pInpStrm->inpDataQMsg.pPayload),
         &(pEosPayload->eosInfo) );

   //fill up the EOS ack event opcode so that services like AFE can blindly raise it
   if (eos_type == AUD_DEC_EOS_V2)
   {
      elite_apr_packet_t *pAprPacket = (elite_apr_packet_t *) (pInpStrm->inpDataQMsg.pPayload);
      asm_data_cmd_eos_v2_t *pEoS = (asm_data_cmd_eos_v2_t *)elite_apr_if_get_payload_ptr(pAprPacket);

      pEosPayload->eosInfo.event_opcode = ASM_DATA_EVENT_RENDERED_EOS_V2;
      pEosPayload->eosInfo.mask = pEoS->mask;
      pEosPayload->eosInfo.eos_client_queue = pInpStrm->common.pEventQ;
      pEosPayload->eosInfo.eos_client_token = pInpStrm->common.eventToken;

      AudioDecSvc_RaiseListenToEoSEventToAsm(pMe, pInpStrm);
   }
   else
   {
      pEosPayload->eosInfo.event_opcode = ASM_DATA_EVENT_RENDERED_EOS;
      pEosPayload->eosInfo.mask = 0;
      pEosPayload->eosInfo.eos_client_queue = 0;
      pEosPayload->eosInfo.eos_client_token = 0;
   }

   ADSPResult result = qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)&eosMsg);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ERROR sending EOS downstream!");
      elite_msg_release_msg(&eosMsg);
      return;
   }

   if (eos_v2_for_ull)
   {
      //below copy helps in raising eos event when downstream svc (AFE/PP) disconnects while we are waiting for response.
      pInpStrm->eosInfo = pEosPayload->eosInfo;
      pInpStrm->bWaitingForEoSRespFromAfe = TRUE;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"DecSvc: sending EOS downstream. eos version %u. mask 0x%lx", eos_type, pEosPayload->eosInfo.mask);

}

static bool_t AudioDecSvc_CircBuf_IsEmpty(AudioDecSvc_Circular_Buffer_List_t *pCircBufList)
{
   //If tail is equal to head then there is no valid buffer in list
   return (pCircBufList->tail == pCircBufList->head);
}

uint16_t AudioDecSvc_CircBuf_GetNumEmptySlots(AudioDecSvc_Circular_Buffer_List_t *pCircBufList)
{
   if (!pCircBufList) return 0;
   //filled buffers are from tail to head-1. and empty from head to tail-1.
   //--filled------|head|==empty==|tail|---filled--
   if (pCircBufList->tail > pCircBufList->head)
   {
      return (pCircBufList->tail - pCircBufList->head);
   }
   else // ===empty===|tail|---filled---|head|===empty==
   {
      return (pCircBufList->tail + pCircBufList->numBufs - pCircBufList->head);
   }
}

/**
 * circ buf has delay portion and MIMO portion (jitter)
 * this function returns whether delay portion is filled or not.
 *
 */
bool_t AudioDecSvc_CircBuf_IsDelayFull(AudioDecSvc_Circular_Buffer_List_t *pCircBufList)
{

   if (AudioDecSvc_CircBuf_GetNumEmptySlots(pCircBufList) <= (1+DEC_SVC_MIMO_MAX_BUF_IN_CIRC_BUFFER)) //1 -> because, circ buf always has at least one empty slot by design. head points to it.
   {
      return true;
   }
   return false;
}

/**
 * flushes circ buf but sends latest media fmt if any,
 * since we cannot lose media fmt.
 *
 * subtracts off buf delays.
 */
ADSPResult AudioDecSvc_CircBuf_Flush(AudioDecSvc_Circular_Buffer_List_t *pCircBufList, AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   if (NULL == pCircBufList)
   {
      return ADSP_EOK;
   }

   elite_msg_any_t latest_media_fmt_msg = {0}; //latest data is closest to the head.
   //subtract off the delay from circ buf
   uint8_t index = 0;

   if (NULL == pCircBufList->buflist)
   {
      return ADSP_EOK;
   }

   //filled buffers are from tail to head-1. and empty from head to tail-1.
   //--filled------|head|==empty==|tail|---filled--
   if (pCircBufList->tail > pCircBufList->head)
   {
	  for (index = pCircBufList->tail; index < pCircBufList->numBufs; index++ )
	  {
		 if (pOutStrm->CircBufList.buflist[index].data)
		 {
			elite_msg_data_buffer_t * pBuf = pOutStrm->CircBufList.buflist[index].data;
			AudioDecSvc_SubtractBufDelayUsingClientToken(pMe, pOutStrm, &pBuf->unClientToken);
		 }
		 if (pOutStrm->CircBufList.buflist[index].msg[0].pPayload)
		 {
			latest_media_fmt_msg = pOutStrm->CircBufList.buflist[index].msg[0];
		 }
	  }

      for (index = 0; index < pCircBufList->head; index++)
      {
         if (pOutStrm->CircBufList.buflist[index].data)
         {
            elite_msg_data_buffer_t * pBuf = pOutStrm->CircBufList.buflist[index].data;
            AudioDecSvc_SubtractBufDelayUsingClientToken(pMe, pOutStrm, &pBuf->unClientToken);
         }

         if (pOutStrm->CircBufList.buflist[index].msg[0].pPayload)
         {
        	 latest_media_fmt_msg = pOutStrm->CircBufList.buflist[index].msg[0];
         }
      }
   }
   else // ===empty===|tail|---filled---|head|===empty==
   {
      for (index = pCircBufList->tail; index < pCircBufList->head; index++ )
      {
         if (pOutStrm->CircBufList.buflist[index].data)
         {
            elite_msg_data_buffer_t * pBuf = pOutStrm->CircBufList.buflist[index].data;
            AudioDecSvc_SubtractBufDelayUsingClientToken(pMe, pOutStrm, &pBuf->unClientToken);
         }
         if (pOutStrm->CircBufList.buflist[index].msg[0].pPayload)
         {
            latest_media_fmt_msg = pOutStrm->CircBufList.buflist[index].msg[0]; //at the end latest will be assigned.
         }
      }
   }

   //Empty Circular buffer. All buffers are free.
   pCircBufList->head = 0; //mark circ buf empty.
   pCircBufList->tail = 0;

   //free all media fmt messages except the latest which has to be sent down

   for (index = 0; index < pCircBufList->numBufs; index++)
   {
      if (pOutStrm->CircBufList.buflist[index].msg[0].pPayload)
      {
         if (latest_media_fmt_msg.pPayload != pOutStrm->CircBufList.buflist[index].msg[0].pPayload)
         {
            elite_msg_release_msg(&(pOutStrm->CircBufList.buflist[index].msg[0]));
         }
         memset(&(pOutStrm->CircBufList.buflist[index].msg[0]), 0, sizeof(pOutStrm->CircBufList.buflist[index].msg[0]));
      }
   }

   if (latest_media_fmt_msg.pPayload)
   {
      AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrm, &(latest_media_fmt_msg));
   }

#ifdef EARLY_EOS_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: AudioDecSvc_CircBuf_Flush", pOutStrm->common.ulDataLogId);
#endif
   return ADSP_EOK;
}

static uint16_t AudioDecSvc_CircBuf_GetFilledBufInd(AudioDecSvc_Circular_Buffer_List_t *pCircBufList)
{
   //reads oldest data
   return pCircBufList->tail;
}

static uint16_t AudioDecSvc_CircBuf_GetEmptyBufInd(AudioDecSvc_Circular_Buffer_List_t *pCircBufList)
{
   return pCircBufList->head;
}

static void AudioDecSvc_CircBuf_QueueEmptyBuf(AudioDecSvc_Circular_Buffer_List_t *pCircBufList, elite_msg_data_buffer_t* pBuf, AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   if (NULL == pCircBufList->buflist)
   {
      return ;
   }

#ifdef EARLY_EOS_DEBUG
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: AudioDecSvc_CircBuf_QueueEmptyBuf head %u, tail %u, numBuf %u, num empty %u, 0x%p",
         pOutStrm->common.ulDataLogId, pCircBufList->head, pCircBufList->tail, pCircBufList->numBufs, AudioDecSvc_CircBuf_GetNumEmptySlots(pCircBufList), pBuf);
#endif

   //Sometimes the buffer remains unused (only media format is used)
   //In those cases NULL pointer is passed to just increment the read index. And empty buffer is retained.
   if(pBuf) pCircBufList->buflist[pCircBufList->tail].data = pBuf;

   bool_t is_empty_before = AudioDecSvc_CircBuf_IsEmpty(pCircBufList);

   pCircBufList->tail = (pCircBufList->tail + 1) % (pCircBufList->numBufs);

   if (is_empty_before)
   {
      pCircBufList->head = pCircBufList->tail; //queuing an empty buf doesn't make it full.it must keep it empty.
   }

   if (!AudioDecSvc_EarlyEos_IsDelaySatisfied(pMe, pOutStrm))
   {
      AudioDecSvc_EarlyEoS_RaiseSignal(pMe, pOutStrm);
   }
}


static void AudioDecSvc_CircBuf_QueueFilledBuf(AudioDecSvc_Circular_Buffer_List_t *pCircBufList, elite_msg_data_buffer_t* pBuf, AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_any_t *media_fmt_msg)
{
   if (NULL == pCircBufList->buflist)
   {
      return ;
   }

#ifdef EARLY_EOS_DEBUG
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: AudioDecSvc_CircBuf_QueueFilledBuf head %u, tail %u, numBuf %u, num empty %u, 0x%p, media fmt 0x%p",
         pOutStrm->common.ulDataLogId, pCircBufList->head, pCircBufList->tail, pCircBufList->numBufs, AudioDecSvc_CircBuf_GetNumEmptySlots(pCircBufList), pBuf, media_fmt_msg->pPayload);
#endif

   //Media format and buffer can be queued separately or together. But media format should not be queued after buffer.
   //Because when buffer is queued the read write index (head) is incremented.
   
   //It is possible that media format is queued before. Overwriting the previous media format.
   if(media_fmt_msg->pPayload)
   {
	   pCircBufList->buflist[pCircBufList->head].msg[0] = *media_fmt_msg;
   }

   //If buffer is still in use and just want to update the media format then function can be called with null buf pointer.
   //When the buffer is filled and ready to be queued then function can be called again with valid pointer.
   if(pBuf)
   {
	   pCircBufList->buflist[pCircBufList->head].data = pBuf;
	   //increment the head
	   pCircBufList->head = (pCircBufList->head + 1) % (pCircBufList->numBufs);

	   //if queue fills when filledBuf is queued, then drop oldest buf at tail.
	   //this is required to make sure queue is empty only when head=tail.
	   if (pCircBufList->head == pCircBufList->tail)
	   {
		  pCircBufList->tail = (pCircBufList->tail + 1) % (pCircBufList->numBufs);
	   }

	   //if buf gets filled, then clear signal
	   if (AudioDecSvc_EarlyEos_IsDelaySatisfied(pMe, pOutStrm))
	   {
		  AudioDecSvc_EarlyEos_ClearSignal(pMe, pOutStrm);
	   }
   }
}

/**
 * pops a buf from bufQ
 * reads the oldest data from the circ buf & sends it down.
 * replaces the oldest data's place with the empty buf.
 */
static ADSPResult AudioDecSvc_SendFromCircBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_data_buffer_t *pOutBuf = NULL;

   //Get buffer present at read index (tail) from list
   uint16_t index = AudioDecSvc_CircBuf_GetFilledBufInd(&pOutStrm->CircBufList);
   elite_msg_data_buffer_t * pBuf = pOutStrm->CircBufList.buflist[index].data;

   //Sending message to downstream.
   for (uint32_t i  = 0; i < AUD_DEC_SVC_MAX_MSG_WITH_DATA; i++)
   {
	   if (pOutStrm->CircBufList.buflist[index].msg[i].pPayload)
	   {
	      AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrm, &(pOutStrm->CircBufList.buflist[index].msg[i]));
	   }
	   memset(&(pOutStrm->CircBufList.buflist[index].msg[0]), 0, sizeof(pOutStrm->CircBufList.buflist[index].msg));
   }

   if(pBuf->nActualSize)//valid data present in buffer, must be swapped with output data Q buffer.
   {
	   //Pop buffer from output data Q.
	   if(ADSP_FAILED(result = AudioDecSvc_ProcessPopOutBuf(pMe, pOutStrm)))
	   {
	      return result;
	   }

	   pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);

	   if (NULL == pOutBuf)
	   {
	      return ADSP_EUNEXPECTED;
	   }
	   
	   //Swap the buffer.
	   AudioDecSvc_CircBuf_QueueEmptyBuf(&pOutStrm->CircBufList, pOutBuf, pMe, pOutStrm);	   
       pOutStrm->outDataBufferNode.pBuffer = (void*)pBuf;

#ifdef EARLY_EOS_DEBUG
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS [0x%lx]: sending from circ buf %p, returnQ %p", pOutStrm->common.ulDataLogId, pBuf, pOutStrm->outDataBufferNode.pReturnQ);
#endif
       result = AudioDecSvc_SendOutBufs(pMe, pOutStrm);
   }
   else
   {
      //no data present, just media format message is present. Do not swap with output data Q buffer.
      //just increments the read (tail) index
      AudioDecSvc_CircBuf_QueueEmptyBuf(&pOutStrm->CircBufList, NULL, pMe, pOutStrm);
   }
   pOutStrm->outDataBufferNode.pBuffer = NULL;
   return result;
}

ADSPResult AudioDecSvc_ProcessCircBufFill(AudioDecSvc_t *pMe, uint32_t channelBitIndex)
{
   if ( ((uint32_t)1)<<channelBitIndex != AUD_DEC_EARLY_EOS_CIRC_BUF_FILL_SIG)
   {
      return ADSP_EFAILED;
   }

   //assigning buffers to capi outputs
   bool_t found = FALSE;
   uint16_t index;
   for(index = 0; index < DEC_SVC_MAX_OUTPUT_STREAMS; index++)
   {
      if (pMe->out_streams_ptr[index])
      {
         if (pMe->out_streams_ptr[index]->early_eos_ptr)
         {
            found = TRUE;
            break;
         }
      }
   }

   if (!found)
   {
      return ADSP_EFAILED;
   }

   AudioDecSvc_OutStream_t *pOutStrm = pMe->out_streams_ptr[index];

   //PCM decoder is supported only with 1 CAPI container.
   if ( AudioDecSvc_IsPcmFmt(pMe->mainCapiContainer->dec_fmt_id) )
   {
      return AudioDecSvc_ProcessOutputDataQ_PCMPlayback(pMe, pOutStrm, FALSE);
   }

   return AudioDecSvc_ProcessOutputDataQForCAPIs(pMe, pOutStrm);
}

ADSPResult AudioDecSvc_ProcessOutputDataQ(AudioDecSvc_t *pMe, uint32_t channelBitIndex)
{
   ADSPResult nResult = ADSP_EFAILED;

   QURT_ELITE_ASSERT(AUD_DEC_OUT_SIG_BIT_IND_TO_INDEX(channelBitIndex) < DEC_SVC_MAX_OUTPUT_STREAMS);

   AudioDecSvc_OutStream_t *pOutStrm = pMe->out_streams_ptr[AUD_DEC_OUT_SIG_BIT_IND_TO_INDEX(channelBitIndex)];

   if(pOutStrm->outDataBufferNode.pBuffer != NULL)
   {
	   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Unexpected error occured. Out buf is already present while processing output data Q.");
	   return ADSP_EUNEXPECTED;
   }

   if (pOutStrm->metadata_xfr.is_waiting_to_push_metadata)
   {
      return AudioDecSvc_SendOutBufs(pMe, pOutStrm);
   }

   //If circ buf has some data then send from there.
   if((NULL != pOutStrm->CircBufList.buflist) &&
		   (!AudioDecSvc_CircBuf_IsEmpty(&pOutStrm->CircBufList)))
   {
      return AudioDecSvc_SendFromCircBuf(pMe, pOutStrm);
   }

   //PCM decoder is supported only with 1 CAPI container. PULL mode is always with only PCM.
   if (DEC_SVC_PULL_MODE == pMe->ulMode)
   {
      return AudioDecSvc_ProcessOutputDataQ_PCMPullMode(pMe, pOutStrm);
   }

   //PCM decoder is supported only with 1 CAPI container.
   if ( AudioDecSvc_IsPcmFmt(pMe->mainCapiContainer->dec_fmt_id) )
   {
      //This is needed to flush out any partial data for PCM decoder
      AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);
      return AudioDecSvc_ProcessOutputDataQ_PCMPlayback(pMe, pOutStrm, pInpStrm->TsState.bIsTSResync);
   }

   nResult = AudioDecSvc_ProcessOutputDataQForCAPIs(pMe, pOutStrm);

   return nResult;
}

//#define MULTI_CAPI_FILE_LOG 1

//TODO: this memcpy can be avoided if same buf is used for input of first capi and output of next capi. But prob is,
//      then buf_size is not a member of 1 capi, but 2 capis & its size depends on 2 capis - difficult to handle.
//TODO: In below actual_len is assigned with nonzero value, before passing to the CAPIs. this may not be desired as CAPIs might assume out buf is empty.
//      this can be handled by creating a temp var for capi_v2_buf_t & using it with actual=0 & appropriate max len.
static ADSPResult AudioDecSvc_CopyPrevOutputToNextInput(dec_CAPI_container_t *next_capi, dec_CAPI_container_t *prev_capi)
{
   uint32_t empty_space_in_next_inp = (next_capi->in_buf[0].max_data_len -
         next_capi->in_buf[0].actual_data_len);

   uint32_t bytes_to_copy = empty_space_in_next_inp > prev_capi->out_buf[0].actual_data_len ?
         prev_capi->out_buf[0].actual_data_len: empty_space_in_next_inp;

   if( ( 0 == next_capi->in_buf[0].actual_data_len ) && (prev_capi->outputs[0].flags.is_timestamp_valid))
   {
      //if next capi does not have input samples, then while copying set TS of the buffer same as previous capi output TS.
      //If the current CAPI is not able to process output because it is waiting for start of next compressed data
      //then the capi will always have two buffers and generate one output.
      //If such CAPI does not do internal buffering, then it will never sync to input buffer TS in steady state.
      //the ideal behavior for such CAPIs is to buffer the data internally  and adjust TS accordingly.
      next_capi->inputs[0].flags = prev_capi->outputs[0].flags;
      next_capi->inputs[0].timestamp = prev_capi->outputs[0].timestamp;

      //Copy to output as well so even if CAPI does not update then we have proper values updated at output.
      next_capi->outputs[0].flags = next_capi->inputs[0].flags;
      next_capi->outputs[0].timestamp = next_capi->inputs[0].timestamp;

#ifdef AUDENCDEC_TIMESTAMP_LOGS
      MSG_2( MSG_SSID_QDSP6, DBG_MED_PRIO, "copying TS [%lu %lu] from previous CAPI to next CAPI",
            (int32_t)(next_capi->inputs[0].timestamp>>32), (int32_t)next_capi->inputs[0].timestamp );
#endif
   }
   memscpy(next_capi->in_buf[0].data_ptr + next_capi->in_buf[0].actual_data_len,
         empty_space_in_next_inp,
         prev_capi->out_buf[0].data_ptr, //always starts from beginning due to memmove
         bytes_to_copy);

   next_capi->in_buf[0].actual_data_len += bytes_to_copy;

   //if prev out has some valid data in it, then move it to the beginning.
   uint32_t bytes_remain_in_prev_out = prev_capi->out_buf[0].actual_data_len > bytes_to_copy ?
         (prev_capi->out_buf[0].actual_data_len - bytes_to_copy): 0;

   if (bytes_remain_in_prev_out)
   {
      memsmove(prev_capi->out_buf[0].data_ptr,bytes_remain_in_prev_out,
            prev_capi->out_buf[0].data_ptr + bytes_to_copy,
            bytes_remain_in_prev_out);
   }
   prev_capi->out_buf[0].actual_data_len = bytes_remain_in_prev_out;

#if (MULTI_CAPI_FILE_LOG == 1)
   int8_t * buf_addr;
   uint32_t buf_size;

   buf_addr = (int8_t *)(next_capi->input_buf_list.pBuf[0].Data) + next_capi->bytes_logged;
   buf_size = (next_capi->in_buf[0].actual_data_len -  next_capi->bytes_logged);

   if(buf_size)
   {
      FILE *fp;
      char fileName[100];
      snprintf(fileName, 100, "CAPI_in_%p.bin", next_capi);
      fp = fopen(fileName,"a"); // in append mode
      fwrite(buf_addr,sizeof(int8_t),buf_size,fp);
      fclose(fp);
   }
#endif

   return ADSP_EOK;
}

//sets up the first CAPI with input data (from client buffer or LPM)
static ADSPResult AudioDecSvc_FillCapiInpBuffer(AudioDecSvc_t* pMe,uint8_t capi_input_index)
{
   ADSPResult nResult = ADSP_EOK;
   dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
   AudioDecSvc_InpStream_t* pInpStrm = pMe->in_streams_ptr[capi_input_index];

   uint8_t* pucIntBufOffset = (uint8_t*)first_capi->in_buf[capi_input_index].data_ptr;
   uint32_t  nInternalBufSize  = first_capi->in_buf[capi_input_index].max_data_len;
   uint32_t  nIntBufWrIdx = (uint32_t)first_capi->in_buf[capi_input_index].actual_data_len;
   uint32_t  nEmptyIntBufBytes = (uint32_t)((int)nInternalBufSize - (int)nIntBufWrIdx);

   int32_t nBytesToCopy;

   uint8_t  **ppucIn;

   ppucIn = (uint8_t**)&pInpStrm->inp_buf_params.unVirtAddr;

   //if input buffer of first CAPI is empty then sync to the TS from input buffer.   
   if(pMe->is_rt_dec)
   {
      if( (0 == nIntBufWrIdx) && AudDecSvc_IsInputPCM(first_capi->capi_type)) 
      {
#ifdef AUDENCDEC_TIMESTAMP_LOGS
         MSG_4(MSG_SSID_QDSP6, DBG_MED_PRIO, "Synced to input TS for 1st CAPI, prev TS[%lu %lu], new TS[%lu %lu]",
               (int32_t)(first_capi->inputs[capi_input_index].timestamp>>32),
               (int32_t)first_capi->inputs[capi_input_index].timestamp,
               (int32_t)(pInpStrm->TsState.ullInbufTS>>32), (int32_t)pInpStrm->TsState.ullInbufTS);
#endif
         first_capi->inputs[capi_input_index].flags.is_timestamp_valid = pInpStrm->TsState.bInbufTSValid;
         first_capi->inputs[capi_input_index].timestamp = pInpStrm->TsState.ullInbufTS;
      }
   }
   //bytes to be copied = minimum of left over client data or left over empty space in internal buffer
   nBytesToCopy = (pInpStrm->inp_buf_params.unMemSize < nEmptyIntBufBytes)? pInpStrm->inp_buf_params.unMemSize:nEmptyIntBufBytes;

   memscpy((pucIntBufOffset+nIntBufWrIdx), nEmptyIntBufBytes, (const void*)*ppucIn, nBytesToCopy);

   //book keeping of remaining bytes in client and empty bytes in internal buffer
   pInpStrm->inp_buf_params.unMemSize -= nBytesToCopy;
   *ppucIn += nBytesToCopy;

   nEmptyIntBufBytes -= nBytesToCopy;
   first_capi->in_buf[capi_input_index].actual_data_len += nBytesToCopy;

   //if we have copied the entire data from client buffer
   //return the buffer with an ack
   if(0 == pInpStrm->inp_buf_params.unMemSize)
   {
      //Ack to Client, because this acks ASM_DATA_CMD_WRITE_V2, ASM will fill the ack opcode, therefore,
      //NULL ack payload, ack payload size, and ack opCode
      nResult = AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
   }
   return nResult;
}


static ADSPResult AudioDecSvc_ProcessLoop(AudioDecSvc_t *pMe, ADSPResult *prevDecResult)
{
   ADSPResult nResult = ADSP_EOK;
   dec_CAPI_container_t *capi_container;

   for (uint8_t capi_index = 0; capi_index < DEC_SVC_MAX_CAPI; capi_index++)
   {
      capi_container = pMe->capiContainer[capi_index];

      if (!capi_container) break;

      //except for first CAPI, handle change in media format based on the output of previous CAPI
      //also Copy output of previous CAPI to next CAPI input.
      if (capi_index != 0)
      {
    	 for (uint32_t port_index = 0; port_index < DEC_SVC_MAX_OUTPUT_STREAMS; port_index++)
    	 {
			 //TODO: to handle the media fmt change between CAPIs
			 if (pMe->capiContainer[capi_index-1]->media_fmt_event[port_index])
			 {
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sample rate or channel map changed between "
						"CAPI %u and CAPI %u. Need to implement this!",capi_index, capi_index-1);
				pMe->capiContainer[capi_index-1]->media_fmt_event[port_index] = false;
			 }
    	 }

         //copy output of first CAPI to input of second except for first one.
         //first one has input already copied earlier
         //for last buffer output buf is the dec svc output buffer. already assigned.
         (void) AudioDecSvc_CopyPrevOutputToNextInput(capi_container, pMe->capiContainer[capi_index-1]);
      }

      //Go for decoding
      nResult = AudioDecSvc_Process( pMe, capi_index);

      if ( ADSP_FAILED(nResult))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder CAPI %u, process error 0x%x",capi_index, nResult);
      }

      *prevDecResult = nResult; //keep the decode result. nResult is used for other purpose below.

      //any valid input bytes left in internal buffer shouldn't be logged, mark as already logged.
      capi_container->bytes_logged = capi_container->in_buf[0].actual_data_len;

      //if previous output was not finished, copy it now.
      if ((capi_index != 0) && (pMe->capiContainer[capi_index-1]->out_buf[0].actual_data_len > 0))
      {
         (void) AudioDecSvc_CopyPrevOutputToNextInput(capi_container, pMe->capiContainer[capi_index-1]);
         if (pMe->capiContainer[capi_index-1]->out_buf[0].actual_data_len)

         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Despite copying prev output to current (%u) input twice, prev output is not finished.",capi_index);
         }

         //any valid input bytes left in internal buffer shouldn't be logged, mark as already logged.
         capi_container->bytes_logged = capi_container->in_buf[0].actual_data_len;
      }

      //even if one dec returns failure or need more return.
      if (ADSP_FAILED(nResult)) return nResult;
   }

   //release=false. vote only if there's an event.
   AudioDecSvc_ProcessKppsBw(pMe, FALSE, FALSE);

   return nResult;
}

static ADSPResult AudioDecSvc_ProcessGetEmptyBufFromCircBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_data_buffer_t *pOutBuf = NULL;

   // if a buf is already popped, return
   if (NULL != pOutStrm->outDataBufferNode.pBuffer)
   {
      return ADSP_EOK;
   }

   if (NULL == pOutStrm->CircBufList.buflist)
   {
      return ADSP_EOK;
   }

   uint16_t index = AudioDecSvc_CircBuf_GetEmptyBufInd(&pOutStrm->CircBufList);

   memset(&(pOutStrm->CircBufList.buflist[index].msg), 0, sizeof(pOutStrm->CircBufList.buflist[index].msg));

   nResult = AudioDecSvc_CheckReCreateOneBufInCircBuf(pOutStrm, &pOutStrm->CircBufList, (int)index, pOutStrm->maxExtBufSize);
   pOutStrm->outDataBufferNode.pBuffer = (void*)pOutStrm->CircBufList.buflist[index].data;
   pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);
   pOutBuf->nActualSize = 0;
   pOutStrm->outDataBufferNode.pReturnQ = NULL; //return back to circ buf.

#ifdef EARLY_EOS_DEBUG
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Early-eos: AudioDecSvc_ProcessGetEmptyBufFromCircBuf 0x%p", pOutBuf);
#endif

   return nResult;
}

/**
 * sets up the first CAPI with input data (from client buffer or LPM)
 * and last CAPI with output buffers (from output queue).
 */
static ADSPResult AudioDecSvc_ProcessOutputDataQForCAPIs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult nResult = ADSP_EOK;
   ADSPResult rc = ADSP_EOK;
   ADSPResult prevDecResult = ADSP_EOK;
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
   AudioDecSvc_OutStream_t *pOutStrmTemp = NULL;
   AudioDecSvc_InpStream_t *pInpStrmTemp = NULL;
   int16_t index = 0;
   uint32_t listen_to_input_mask = 0;

   if(NULL == last_capi)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Fail to get last capi");
      return ADSP_EFAILED;
   }

   //Get the channel status of all output data Q
   uint32_t OutStrmMask = AudioDecSvc_GetAllOutputStreamMask(pMe);
   uint32_t unChannelStatus = qurt_elite_channel_poll(&(pMe->channel), OutStrmMask) & pMe->unCurrentBitfield;

   //get default input stream(primary or primary_secondary stream for mimo decoder)
   AudioDecSvc_InpStream_t* pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

   //assigning buffers to Last Capi output ports.
   for(index = 0; index < DEC_SVC_MAX_OUTPUT_STREAMS; index++)
   {
      pOutStrmTemp = pMe->out_streams_ptr[index];
      if(NULL == pOutStrmTemp) continue;

      //Buffer should have been returned to either Q or Circular BufList in previous iteration.
      if (NULL != pOutStrmTemp->outDataBufferNode.pBuffer)
      {
	      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Buffer is not NULL, Unexpected Error.");
    	  nResult = ADSP_EUNEXPECTED;
    	  goto __bailout__;
      }

      //If Signal is set in output data q then pop from output data q
      if(AUD_DEC_OUT_INDEX_TO_DATA_SIG(index) & unChannelStatus)
      {
         nResult |= AudioDecSvc_ProcessPopOutBuf(pMe, pOutStrmTemp);
      }
      else //if signal is not set then use buffer present at head in circ buf list
      {
         nResult |= AudioDecSvc_ProcessGetEmptyBufFromCircBuf(pMe, pOutStrmTemp);
      }

      //A buffer must be present.
      nResult |= (NULL == pOutStrmTemp->outDataBufferNode.pBuffer)? ADSP_EUNEXPECTED : ADSP_EOK;

      if (ADSP_FAILED(nResult))
      {
         goto __bailout__;
      }

      elite_msg_data_buffer_t* pOutBuf = (elite_msg_data_buffer_t*) (pOutStrmTemp->outDataBufferNode.pBuffer);

      last_capi->out_buf[index].data_ptr = (int8_t *) (&(pOutBuf->nDataBuf));
      last_capi->out_buf[index].actual_data_len = 0;
      pOutBuf->nActualSize = 0;
   }

   //filling capi input buffers
   for(index = 0; index < DEC_SVC_MAX_INPUT_STREAMS; index++)
   {
      pInpStrmTemp = pMe->in_streams_ptr[index];
      if(pInpStrmTemp == NULL) continue;

      //If there is more data to be copied from client buffer.
      //Even if client data is not present, process has to be called to flush any remaining input data (esp. @ EoS)
      if(pInpStrmTemp->inp_buf_params.unMemSize > 0)
      {
         nResult |= AudioDecSvc_FillCapiInpBuffer(pMe,index);
      }

      //if we have copied the entire data from client buffer
      if(0 == pInpStrmTemp->inp_buf_params.unMemSize)
      {
         //If EoF was set in the input buffer, then last copy to decoder internal buffer ensures
         //integral number of frames in the decoder internal buffer.
         first_capi->inputs[index].flags.end_of_frame = pInpStrmTemp->bEndOfFrame;

         //If EOS is set for non-secondary input stream then set the eos and eof flag for each capiV2 input.
         //If EOS is set for secondary input stream then set the eos and eof flag for that capiV2 input port.
         if(pInpStrm->endOfStream || pInpStrmTemp->endOfStream)
         {
			 first_capi->inputs[index].flags.marker_eos = true;
			 first_capi->inputs[index].flags.end_of_frame = true;
         }
      }

      //log any new input data
      int8_t * buf_addr;
      uint32_t buf_size;

      buf_addr = (int8_t *)(first_capi->in_buf[index].data_ptr) + first_capi->bytes_logged;
      buf_size = (first_capi->in_buf[index].actual_data_len -  first_capi->bytes_logged);

      if(buf_size)
      {
         AudioDecSvc_LogBitStreamData(buf_addr, buf_size, pInpStrmTemp->common.ulDataLogId, first_capi->dec_fmt_id);
      }
   }

   if (ADSP_FAILED(nResult)) goto __bailout__;

   //process over all CAPIs
   nResult = AudioDecSvc_ProcessLoop(pMe, &prevDecResult);

   //Handle data port threshold change.
   if (pMe->event_mask & AUD_DEC_SVC_EVENT__PORT_DATA_THRESH_CHANGE_MASK)
   {
	  rc = AudioDecSvc_HandlePortDataThreshChangeEvent(pMe);
	  //Exit if error occurred.
      if (ADSP_FAILED(rc)) goto __bailout__;

      //If any external buffer is recreated then return.
      for (index = 0; index < DEC_SVC_MAX_OUTPUT_STREAMS; index++)
      {
    	  if ( (NULL != pMe->out_streams_ptr[index]) &&
    			  (NULL == pMe->out_streams_ptr[index]->outDataBufferNode.pBuffer))
    	  {
    		  goto __bailout__;
    	  }
      }
   }

   for(index = 0; index < DEC_SVC_MAX_INPUT_STREAMS; index++)
   {
      pInpStrmTemp = pMe->in_streams_ptr[index];
      if(pInpStrmTemp == NULL) continue;

      //If client buffer is empty and either CAPI buf is zero size or not sufficient,
      //set data signal to listen for data
      if( (pInpStrmTemp->inp_buf_params.unMemSize == 0) &&
            ( (0 == first_capi->in_buf[index].actual_data_len) || (ADSP_EOK != prevDecResult) ||
                  (pInpStrmTemp->WasPrevDecResNeedMore) ) )
      {
         //If this was the last input buffer and we have completely processed then set the flag to propagate EOS downstream.
         //If an error occurs while processing this buffer then it is unrecoverable and so send EOS anyway.
         if(pInpStrmTemp->endOfStream)
         {
            //1. if secondary input stream has received  eos command then raise ASM_DATA_EVENT_RENDERED_EOS event to client
            //2. for primary or primary_secondary input stream ASM_DATA_EVENT_RENDERED_EOS will be raised by AFE.
            if(pInpStrmTemp->common.stream_type == ASM_STREAM_TYPE_SECONDARY)
            {
               AudioDecSvc_Reset_SecondaryInpStream(pMe,pInpStrmTemp);
               elite_apr_packet_t *pAprPacket = ( elite_apr_packet_t *) (pInpStrmTemp->inpDataQMsg.pPayload);
               if(ADSP_FAILED(AudioStreamMgr_GenerateAck(pAprPacket, ADSP_EOK, NULL,  0, 0)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EOS ack failed for secondary input stream.");
               }
            }
            else
            {
               pMe->need_to_send_eos = TRUE;
            }
         }
         else
         {
        	listen_to_input_mask |= AUD_DEC_INP_INDEX_TO_DATA_SIG(index);
         }
      }
   }

   if (listen_to_input_mask)
   {
	   //If any input is insufficient then Stops listening to all output.
	   //And start listening to required input.
	   uint32_t omask = AudioDecSvc_GetAllOutputStreamMask(pMe);
	   AudioDecSvc_StopListenToMask(pMe, omask);
	   AudioDecSvc_StartListenToMask(pMe, listen_to_input_mask);
   }

   for(index = 0; index < DEC_SVC_MAX_OUTPUT_STREAMS; index++)
   {
      pOutStrmTemp = pMe->out_streams_ptr[index];
      if(pOutStrmTemp == NULL) continue;

      elite_msg_any_t media_fmt_msg = {0};
      elite_msg_data_buffer_t* pOutBuf = (elite_msg_data_buffer_t*) (pOutStrmTemp->outDataBufferNode.pBuffer);

      if(ADSP_SUCCEEDED(nResult))
      {
    	  nResult |= AudioDecSvc_PostProcess(pMe, pOutStrmTemp, &media_fmt_msg);
      }

      //if decode is successful and if output buffer size is > 0,
      //deliver it to downstream, otherwise return the buffer to bufQ
      bool_t outBufNotEmpty = pOutBuf->nActualSize > 0;

      //Returning buffers to their respective Q.
      if(ADSP_SUCCEEDED(nResult) && outBufNotEmpty)
      {
         AudioDecSvc_PopulateOutBuf(pMe, pOutStrmTemp);

         //sending buffer downstream
         if(AudioDecSvc_IsBufFromOutBufQ(pOutStrmTemp))
         {
            if (media_fmt_msg.pPayload)
            {
               rc |= AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrmTemp, &media_fmt_msg);
               media_fmt_msg.pPayload= NULL;
            }
            rc |= AudioDecSvc_SendOutBufs(pMe, pOutStrmTemp);
            if(ADSP_FAILED(rc)) goto __bailout__;
         }
         else
         {
            AudioDecSvc_CircBuf_QueueFilledBuf(&pOutStrmTemp->CircBufList, pOutBuf,
            		pMe, pOutStrmTemp, &media_fmt_msg);
            pOutStrmTemp->outDataBufferNode.pBuffer = NULL;
         }
      }
      else
      {
         //Returning the output buffer to Q as it doesnt have any data to deliver downstream
#ifdef DBG_AUDIO_DECODE
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "No data in OP buffer. return it to Q!\n");
#endif
         //sending buffer downstream
         if (media_fmt_msg.pPayload)
         {
            if(AudioDecSvc_IsBufFromOutBufQ(pOutStrmTemp) ||
            		AudioDecSvc_CircBuf_IsEmpty(&pOutStrmTemp->CircBufList))
            {
               rc |= AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrmTemp, &media_fmt_msg);
            }
            else
            {
               //output buffer has no valid data(actual size is zero) only media format is valid.
               AudioDecSvc_CircBuf_QueueFilledBuf(&pOutStrmTemp->CircBufList, pOutBuf, pMe, pOutStrmTemp, &media_fmt_msg);
            }
            media_fmt_msg.pPayload= NULL;
         }

         //circ buf is not affected as AudioDecSvc_CircBuf_GetEmptyBufInd didn't change any index.
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrmTemp);

         //if no output gets generated for last buf, then send EoS now. if output gets generated then AudioDecSvc_SendOutBufs will send EoS.
         AudioDecSvc_HandleEosCases(pMe, pInpStrm, pOutStrmTemp, outBufNotEmpty);
      }
   }

   return nResult | rc;

__bailout__:
   //return all the buffer to their respective Q if anything failed.
   for (index = 0; index < DEC_SVC_MAX_OUTPUT_STREAMS; index++)
   {
      pOutStrmTemp = pMe->out_streams_ptr[index];
      if(pOutStrmTemp)
      {
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrmTemp);
      }
   }
   return nResult | rc;

}

static ADSPResult AudioDecSvc_ProcessOutputDataQ_PCMPlayback(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, bool_t release_partial)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_data_buffer_t *pOutBuf = NULL;
   uint8_t** ppucIn;
   elite_msg_any_t media_fmt_msg = {0};
   //Get the channel status of all output data Q
   uint32_t OutStrmMask = AudioDecSvc_GetAllOutputStreamMask(pMe);
   uint32_t unChannelStatus = qurt_elite_channel_poll(&(pMe->channel), OutStrmMask) & pMe->unCurrentBitfield;

   //for PCM assumption is only one input and output (error check is made)
   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

   bool_t sendPartial = release_partial || pInpStrm->endOfStream;

   //if client buffer is empty, then release it. There is only one extern
   //output buffer, if control is reached here which means buffer is already
   //returned by downstream service and no check is required for output
   //buffer queue. by freeing the input buffer after downstream returns it, end-to-end delay is reduced as client has less opportunity of buffering data now.
   if(0 == pInpStrm->inp_buf_params.unMemSize)
   {
      //At EoS don't release inp buf (need to process EOS first); also when partial buf (release_partial) is to be sent, don't release input buf (because input buf is probably null)
      if (!sendPartial)
      {
         nResult = AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
         AudioDecSvc_ListenToInput(pMe);
         return nResult;
      }
   }

   if (NULL == pOutStrm->pDownStreamSvc)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc: Decoder service not yet connected to Downstream");
      return nResult;
   }

   if (unChannelStatus & AudioDecSvc_GetOutputStreamMask(pMe, pOutStrm))
   {
	  nResult = AudioDecSvc_ProcessPopOutBuf(pMe, pOutStrm);
   }
   else
   {
	  nResult = AudioDecSvc_ProcessGetEmptyBufFromCircBuf(pMe, pOutStrm);
   }

   //There should be a valid buffer.
   pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);
   if(NULL == pOutBuf && ADSP_SUCCEEDED(nResult)) {nResult = 	ADSP_EUNEXPECTED;}

   if(ADSP_FAILED(nResult))
   {
	  AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
      return nResult;
   }

   //if release partial, then no new data is to be read.
   if (!release_partial)
   {
      ppucIn = (uint8_t**)&(pInpStrm->inp_buf_params.unVirtAddr);

      uint32_t numBytesProcessed = 0;

      //always fill output buffer completely (ULL/LLNP/LL/legacy)
      nResult = AudioDecSvc_ProcessPCM(pMe, pInpStrm, pOutStrm, ppucIn, &pInpStrm->inp_buf_params.unMemSize, &numBytesProcessed, FALSE, pOutBuf->nMaxSize, FALSE);

      if (ADSP_FAILED(nResult))
      {
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
         return nResult;
      }
   }

   //at end of stream, if output buffer contains less than max-size & channel spacing is based on max-out-put-size (unpacked), now pack it
   //dont do this at non-EOS times even though nActualSize<nMaxSize because, we will go and wait for more data before sending data.
   if (sendPartial && (pOutBuf->nActualSize < pOutBuf->nMaxSize) )
   {
      uint32_t bytes_per_ch = pOutBuf->nActualSize/pInpStrm->pcmFmt.chan_map.nChannels;
      uint32_t max_bytes_per_ch = pOutBuf->nMaxSize/pInpStrm->pcmFmt.chan_map.nChannels;
      int8_t *out_buf = ( ((int8_t *)(&pOutBuf->nDataBuf)) );
      for (uint32_t nCh=1; nCh < pInpStrm->pcmFmt.chan_map.nChannels; nCh++)
      {
         memsmove(out_buf+(bytes_per_ch*nCh), bytes_per_ch, out_buf+(max_bytes_per_ch*nCh), bytes_per_ch);
      }
   }

   /* At this point, PCM data in output buffer is converted to output bytes per sample */

   //check if channel mapping changed
   bool_t chan_map_changed = AudioDecSvc_IsChanMapChanged(pMe->capiContainer[0]->PrevFmtBlk[0].num_channels,
         pMe->capiContainer[0]->PrevFmtBlk[0].channel_mapping,
         pInpStrm->pcmFmt.chan_map.nChannels,
         pInpStrm->pcmFmt.chan_map.nChannelMap);

   // if there is any change in fs/channels from previous frame
   // send media format to down stream before the fresh data is enqueued
   if ( (pInpStrm->pcmFmt.ulPCMSampleRate != pMe->capiContainer[0]->PrevFmtBlk[0].sample_rate)
         || chan_map_changed )
   {
      //notify client about the changes if the event is enabled
      (void) AudioDecSvc_SendSrCmEvent(pMe, pInpStrm, pInpStrm->pcmFmt.ulPCMSampleRate,
            &pInpStrm->pcmFmt.chan_map);


      uint32_t num_chan = pOutStrm->out_chan_map.num_out_chan;

      //recompute output channel mapping that will be sent to downstream svc
      AudioDecSvc_GetOutChanMap(&pInpStrm->pcmFmt.chan_map, &pOutStrm->out_chan_map);

      if ( (pOutStrm->common.total_bytes != 0) &&
            (num_chan != 0) &&
            (pMe->capiContainer[0]->PrevFmtBlk[0].sample_rate != 0) )
      {
         pOutStrm->common.total_bytes = pOutStrm->common.total_bytes *
               pOutStrm->out_chan_map.num_out_chan / num_chan *
               pInpStrm->pcmFmt.ulPCMSampleRate / pMe->capiContainer[0]->PrevFmtBlk[0].sample_rate; //bytes per ch doesn't change.
      }

     
      nResult = AudioDecSvc_GetMediaFmtUpdateMsg(pMe,pInpStrm, pOutStrm,
            pInpStrm->pcmFmt.ulPCMSampleRate,
            pOutStrm->out_chan_map.num_out_chan,
            pOutStrm->out_chan_map.out_chan,
            pOutStrm->output_bits_per_sample,
            pMe->capiContainer[0]->out_data_fmt_type[0],
            pMe->capiContainer[0]->dec_fmt_id,
            &media_fmt_msg
            );

      if (ADSP_FAILED(nResult))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc:Fail to send Media Fmt update");
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
         return nResult;
      }

      //update prev format block
      AudioDecSvc_UpdatePrevFmtBlk(pMe,pInpStrm,
            pMe->capiContainer[0],
            pInpStrm->pcmFmt.ulPCMSampleRate,
            pInpStrm->pcmFmt.chan_map.nChannels,
            pInpStrm->pcmFmt.chan_map.nChannelMap,
            pOutStrm->output_bits_per_sample,0);
   }

   bool_t outBufFull = ((int32_t)pOutBuf->nMaxSize == pOutBuf->nActualSize);
   bool_t outBufPartialSend = (pOutBuf->nActualSize) && (sendPartial);

   //remove initial silence
   if (outBufFull || outBufPartialSend)
   {
      uint32_t init_samples_removed_before = pInpStrm->silence_removal.initial_samples_to_remove;
      if(pInpStrm->silence_removal.initial_samples_to_remove > 0)
      {
         AudioDecSvc_RemoveInitialSamples(&pInpStrm->silence_removal.initial_samples_to_remove,
               (uint32_t *)&pOutBuf->nActualSize,
               (uint8_t *)&(pOutBuf->nDataBuf),
               pInpStrm->pcmFmt.chan_map.nChannels,
               pInpStrm->pcmFmt.ulPCMSampleRate,
               &pInpStrm->TsState.ullNextOutBufTS,
               &pInpStrm->TsState.ullNextOutBufSampleCount,
               pOutStrm->output_bytes_per_sample);

         outBufFull = ((int32_t)pOutBuf->nMaxSize == pOutBuf->nActualSize);
         outBufPartialSend = (pOutBuf->nActualSize) && (sendPartial);
      }

      uint32_t init_samples_removed_after = pInpStrm->silence_removal.initial_samples_to_remove;
      if (pOutStrm->early_eos_ptr)
      {
         if (init_samples_removed_after != init_samples_removed_before)
         {
            pOutStrm->early_eos_ptr->is_init_samples_removed = TRUE;
         }
         else
         {
            pOutStrm->early_eos_ptr->is_init_samples_removed = FALSE;
         }
      }
   }

   bool_t input_buffers_empty = false;

   if(0 == pInpStrm->inp_buf_params.unMemSize)
   {
      input_buffers_empty = true;
   }


   if (outBufFull || outBufPartialSend)
   {
      //remove trailing silence
      if(pInpStrm->silence_removal.last_buffer)
      {
         //Currently trailing silence removal is only applicable for last frame
         //Make sure this is the last frame by seeing if all data in internal buffer has been
         //consumed and no data remains in the client buffer
         if(input_buffers_empty && (pInpStrm->silence_removal.trailing_samples_to_remove > 0))
         {
            AudioDecSvc_RemoveTrailingSamples(pInpStrm->silence_removal.trailing_samples_to_remove,
                  (uint32_t *)&pOutBuf->nActualSize,
                  (uint8_t *)&(pOutBuf->nDataBuf),
                  pInpStrm->pcmFmt.chan_map.nChannels,
                  pOutStrm->output_bytes_per_sample);
         }
         outBufFull = ((int32_t)pOutBuf->nMaxSize == pOutBuf->nActualSize);
         outBufPartialSend = (pOutBuf->nActualSize) && (sendPartial);
      }
   }

   pInpStrm->WasPrevDecResNeedMore = false;

   if(outBufFull || outBufPartialSend)
   {
      uint32_t num_samples_per_chan = pOutBuf->nActualSize/(pOutStrm->output_bytes_per_sample*pInpStrm->pcmFmt.chan_map.nChannels);
      //do channel downmixing if required
      AudioDecSvc_GetOutput(pMe,pOutStrm,
            pMe->capiContainer[0],
            &pInpStrm->pcmFmt.chan_map,
            (void*) &pOutBuf->nDataBuf,
            num_samples_per_chan,
            pOutStrm->output_bytes_per_sample,
            (uint32_t*)(&pOutBuf->nActualSize));

      AudioDecSvc_PopulateOutBuf(pMe, pOutStrm);

      if (AudioDecSvc_IsBufFromOutBufQ(pOutStrm))
      {
         if (media_fmt_msg.pPayload)
         {
            AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrm, &media_fmt_msg);
         }
         nResult = AudioDecSvc_SendPcmToPeerSvc(pMe, pInpStrm, pOutStrm, pOutBuf);
      }
      else
      {
    	 AudioDecSvc_CircBuf_QueueFilledBuf(&pOutStrm->CircBufList, pOutBuf, pMe, pOutStrm, &media_fmt_msg);
      }
      if(ADSP_FAILED(nResult))  AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
      pOutStrm->outDataBufferNode.pBuffer = NULL;
      media_fmt_msg.pPayload = NULL;
   }
   else //Actual size is zero or sendpartial is false.
   {
	  if (pOutBuf->nActualSize)
	  {
		 pInpStrm->WasPrevDecResNeedMore = true; //for partial buffers, mark need-more. this ensures we dont sync to new buf TS before checking for TS discontinuity
	  }
      if (media_fmt_msg.pPayload)
      {
         if (AudioDecSvc_IsBufFromOutBufQ(pOutStrm))
         {
            AudioDecSvc_SendMediaFmtUpdate(pMe, pOutStrm, &media_fmt_msg);
         }
         else
         {
        	if(pInpStrm->WasPrevDecResNeedMore)
        	{
        		//Buffer is still in use, queue only media format.
        		AudioDecSvc_CircBuf_QueueFilledBuf(&pOutStrm->CircBufList, NULL, pMe, pOutStrm, &media_fmt_msg);
        	}
        	else
        	{
        		//Buffer is empty, will be returned.
        		AudioDecSvc_CircBuf_QueueFilledBuf(&pOutStrm->CircBufList, pOutBuf, pMe, pOutStrm, &media_fmt_msg);
        	}
         }
         media_fmt_msg.pPayload = NULL;
      }

      if (!pInpStrm->WasPrevDecResNeedMore)
      {
         //return output buffer (if it's empty)
         AudioDecSvc_ReturnOutBuf(pMe, pOutStrm);
      }

      if (!sendPartial)
      {
         nResult = AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm, ADSP_EOK);
         AudioDecSvc_ListenToInput(pMe);
      }
   }

   //If this was the last input buffer and we have completely processed then propagate EOS downstream.
   if((pInpStrm->endOfStream) && ((pOutBuf->nActualSize == 0) || input_buffers_empty))
   {
      pMe->need_to_send_eos = TRUE;
      AudioDecSvc_HandleEosCases(pMe, pInpStrm, pOutStrm, (pOutBuf->nActualSize > 0));
   }

   return nResult;
}

/**
 * For a given output buf, this function may be called once
 * or twice.
 *
 * for regular mode: input buf is copied to output buf after deinterleaving and converting byte len.
 *       channel spacing is min of inp size & output size.
 *       out buf is indexed starting from first in each call.
 * for pull mode: input buf is copied to output buf after deinterleaving and converting byte len.
 *       but the call may be made multiple times since the inp buf is actually circular, but this func handles only lin buf.
 *       channel spacing is always output buf size
 *       out buf is indexed from starting for the first call, and
 *       from anywhere for the subsequent calls.
 *
 *
 * ppucIn - ptr to the ptr to the buffer
 * pRemBytesInSrcBuf - ptr to the remaining bytes in the input buffer.
 * numBytesProcessed - num of bytes processed after sending to output.
 * isPackedChSpacing (always false) -> if channel spacing is decided by number of input samples (packed) or
 *                            simply from output buf size (used in pull mode/ULL) (unpacked).
 * maxUsableOutBufSize -> although out buf size is alloc to max, we need to use only correct length
 *                      (1ms or so in case of pull mode)
 * isInvalidateCache ->whether the source buffer cache needs to be invalidated.
 *
 */
ADSPResult AudioDecSvc_ProcessPCM(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, uint8_t **ppucIn, uint32_t *pRemBytesInSrcBuf,
      uint32_t *numBytesProcessed, bool_t isPackedChSpacing, uint32_t maxUsableOutBufSize, bool_t isInvalidateCache)
{
   ADSPResult nResult = ADSP_EOK;
   elite_msg_data_buffer_t *pOutBuf = NULL;
   uint8_t* pucIn;
   uint32_t bytes_per_sample_in;
   uint32_t bytes_per_sample_out;

   pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);

   pucIn = *ppucIn;

   //Remaining bytes to be copied from source Buffer (Client/LPM buffer)

   uint32_t  RemBytesInSrcBuf = *pRemBytesInSrcBuf;

   bytes_per_sample_in = pInpStrm->pcmFmt.usBytesPerSample; /* PCM media format input bytes per sample */
   bytes_per_sample_out = pOutStrm->output_bytes_per_sample;

   uint32_t inp_samples_avail = RemBytesInSrcBuf / bytes_per_sample_in;

   uint32_t rem_out_buf_len = ( maxUsableOutBufSize - pOutBuf->nActualSize );
   uint32_t max_out_samples = ( rem_out_buf_len / bytes_per_sample_out );

   //samples for all channels
   uint32_t samples_to_copy = (inp_samples_avail > max_out_samples) ? max_out_samples : inp_samples_avail;

   uint32_t in_bytes_to_copy = samples_to_copy * bytes_per_sample_in;

   uint32_t num_in_channel = (pInpStrm->pcmFmt.chan_map.nChannels);
   uint32_t num_samples_per_chan = (samples_to_copy / num_in_channel);

   uint32_t num_out_bytes_per_chan = (num_samples_per_chan * bytes_per_sample_out);

   uint32_t byte_offset = pOutBuf->nActualSize/num_in_channel;

   //isPackedChSpacing == true => packed mode (channels are not spaced apart).
   uint32_t out_chan_spacing_in_bytes = (isPackedChSpacing) ?  (num_out_bytes_per_chan ) : (maxUsableOutBufSize / num_in_channel);


   int8_t *out_buf;
   int8_t *out_ptr[PCM_FORMAT_MAX_NUM_CHANNEL];

   if (isInvalidateCache)
   {
      if(ADSP_FAILED(nResult = qurt_elite_memorymap_cache_invalidate((uint32_t)pucIn, in_bytes_to_copy)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:LPM:Failure cache invalidate.");
         return nResult;
      }
   }

   /* Logging the input PCM data to Audio Decoder Module */
   AudioDecSvc_LogPcmData(pMe, pInpStrm, (int8_t *)pucIn, in_bytes_to_copy);

   // convert from big endian to little endian
   if(pInpStrm->pcmFmt.endianness == DEC_BIG_ENDIAN)
   {
      if(ADSP_FAILED(nResult = endianness_convertor_with_intlv_in((int8_t *)pucIn, samples_to_copy, bytes_per_sample_in*8)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Failed to convert endianness of the input samples.");
         return nResult;
      }
   }

   //space for deinterleaving
   out_buf = ( ((int8_t *)(&pOutBuf->nDataBuf)) + byte_offset);

   for(uint32_t i = 0; i < num_in_channel; i++)
   {
      out_ptr[i] = out_buf + (i * out_chan_spacing_in_bytes) ;
   }

   // input can be in 2,3,4 byte registers and can be Q15,Q23,Q31. Output is 2 or 4 bytes and Q15 or Q27.
   //32 (Q31) -> 16 (Q15), 24 (Q23) -> 16 (Q15), 16->16
   if(2 == bytes_per_sample_out)
   {
      /* 1. First conversion to 16 bits */
      nResult = byte_convertor_intlv_16_out((int8_t *)pucIn, (int8_t *)(pMe->capiContainer[0]->scratch_out_buf), samples_to_copy, bytes_per_sample_in*8, (bytes_per_sample_in*8-1));
      if (ADSP_FAILED(nResult))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Error bytes_per_sample_in = %lu, bytes_per_sample_out = %lu, bps = %d",
               bytes_per_sample_in, bytes_per_sample_out, pInpStrm->pcmFmt.usBitsPerSample);
      }

      /* Now do de-interleaving of 16 bit */
      /* kw check: condition is applied for max number of channels */
      if(num_in_channel <= PCM_FORMAT_MAX_NUM_CHANNEL)
      {
         DeInterleave_Multich_16((int16_t *)pMe->capiContainer[0]->scratch_out_buf, (int16_t **)out_ptr,  num_in_channel, num_samples_per_chan);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6,  DBG_ERROR_PRIO, "AudioDecSvc: Error: Number of channels:[%lu]", num_in_channel);
      }
   }
   //16 -> 32 (Q27), 24 -> 32 (Q27), 32 (Q31) ->32 (Q27) OR
   //16 -> 32 (Q31), 24 -> 32 (Q31), 32 (Q31) ->32 (Q31)
   else if ( 4 == bytes_per_sample_out )
   {
      /* first convert to 32 bits (Q27 or Q31) */
      if(24 == pOutStrm->output_bits_per_sample)
      {
         nResult = byte_convertor_intlv_32_out((int8_t *)pucIn, (int8_t *)(pMe->capiContainer[0]->scratch_out_buf), samples_to_copy, bytes_per_sample_in*8, (bytes_per_sample_in*8-1));
      }
      else
      {
         nResult = byte_convertor_intlv_32_out_Q31((int8_t *)pucIn, (int8_t *)(pMe->capiContainer[0]->scratch_out_buf), samples_to_copy, bytes_per_sample_in*8, (bytes_per_sample_in*8-1));
      }

      if (ADSP_FAILED(nResult))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Error bytes_per_sample_in = %lu, bytes_per_sample_out = %lu, bps = %d",
               bytes_per_sample_in, bytes_per_sample_out, pInpStrm->pcmFmt.usBitsPerSample);
      }

      /* kw check: condition is applied for max number of channels */
      if(num_in_channel <= PCM_FORMAT_MAX_NUM_CHANNEL)
      {
         DeInterleave_Multich_32((int32_t *)pMe->capiContainer[0]->scratch_out_buf, (int32_t **)out_ptr, num_in_channel, num_samples_per_chan);
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6,  DBG_ERROR_PRIO, "AudioDecSvc: Error: Number of channels:[%lu]", num_in_channel);
      }
   }
   else
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Error bytes_per_sample_in = %lu, bytes_per_sample_out = %lu", bytes_per_sample_in, bytes_per_sample_out);
   }

   /* At this point, PCM data in output buffer is converted to output bytes per sample */

   uint32_t new_bytes = samples_to_copy * bytes_per_sample_out;
   *numBytesProcessed = new_bytes;
   //Get the output size
   pOutBuf->nActualSize += new_bytes;

   //update input buffer remaining bytes and byte pointers
   *pRemBytesInSrcBuf -= in_bytes_to_copy;
   *ppucIn += in_bytes_to_copy;

   AudioDecSvc_UpdateInputTsByDataConsumed(&pInpStrm->TsState, in_bytes_to_copy, &pInpStrm->pcmFmt);

   return nResult;
}

/* =======================================================================
 **                          Message handler functions
 ** ======================================================================= */

//#define AUD_DEC_SVC_CAPI_IO_SIZE_LOG

static ADSPResult AudioDecSvc_Process(AudioDecSvc_t *pMe, uint8_t capi_index)
{
   dec_CAPI_container_t *capi_container = pMe->capiContainer[capi_index];
   AudioDecSvc_InpStream_t * pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);
   //# bytes in CAPI buffer before decoding
   uint32_t inpSizeBefore[DEC_SVC_MAX_INPUT_STREAMS] = {0};
   uint32_t remainingBytesAfter[DEC_SVC_MAX_INPUT_STREAMS] = {0};

   /* Call the decoder CAPI */
   for (uint16_t input_stream_index = 0; input_stream_index<DEC_SVC_MAX_INPUT_STREAMS; input_stream_index++)
   {
      //having a nonNULL pointer implies that this CAPI has this input port
      if (NULL != capi_container->in_buf[input_stream_index].data_ptr)
      {
         inpSizeBefore[input_stream_index] = capi_container->in_buf[input_stream_index].actual_data_len;

#ifdef AUD_DEC_SVC_CAPI_IO_SIZE_LOG
         MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Dec %lx CAPI %u process before: inLengh %lu, outLen[0] %lu for capi input port %u\n", pInpStrm->common.ulDataLogId,
               capi_index, inpSizeBefore[input_stream_index],
               capi_container->out_buf[0].actual_data_len, input_stream_index);

         uint8_t *data_ptr = (uint8_t*)capi_container->in_buf[input_stream_index].data_ptr;
         MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Dec %lx CAPI %u: bytes in: %x %x %x %x  %x %x %x  ", pInpStrm->common.ulDataLogId,
               capi_index, *(data_ptr), *(data_ptr+1),*(data_ptr+2), *(data_ptr+3),
               *(data_ptr+4), *(data_ptr+5),*(data_ptr+6));

         MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Dec %lx CAPI %u: EoF %lu, EoS %lu", pInpStrm->common.ulDataLogId,
               capi_index, capi_container->inputs[input_stream_index].flags.end_of_frame, capi_container->inputs[input_stream_index].flags.marker_eos);
#endif
      }
   }

   /**
    * At input of process, the input->actual_len is the size of input & data starts from data_ptr.
    *                      the output->actual_len is uninitialized & CAPI can write from data_ptr.
    * At output of process, the input->actual_len is the amount of data consumed (read) by CAPI. remaining data is from data_ptr+actual_len
    *                      the output->actual_len is output data, & data starts from data_ptr.
    */
   capi_v2_err_t nResult = capi_container->capi_ptr->vtbl_ptr->process(capi_container->capi_ptr, capi_container->inputs_ptr, capi_container->outputs_ptr);

   for(uint8_t index = 0 ; index < DEC_SVC_MAX_INPUT_STREAMS ; index++)
   {
      if(capi_container->in_buf[index].data_ptr)
      {
#ifdef AUD_DEC_SVC_CAPI_IO_SIZE_LOG
         MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Dec %lx CAPI %u process after: nResult 0x%x, inLengh consumed %lu, outLen[0] %lu for capi input port %u\n",
               pInpStrm->common.ulDataLogId, capi_index,
               nResult, capi_container->in_buf[index].actual_data_len,
               capi_container->out_buf[0].actual_data_len,index);
#endif
         //error checks
         if((capi_container->in_buf[index].actual_data_len > inpSizeBefore[index]))
         {
            //The number of remaining bytes are negative or greater than valid number of bytes
            //at the beginning of decode. This indicates that decoding did not happen correctly
            //and decoder might be in a bad state
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Invalid # of available input bytes after CAPI process call at input port %u : Available bytes before = %lu, Consumed bytes after = %lu",
                  index, inpSizeBefore[index], capi_container->in_buf[index].actual_data_len);

            //set the result to DEC_RECOVERY_FAILURE which will discard the contents of the
            //current buffer
            nResult = CAPI_V2_EFAILED;

            //reset the decoder in order to avoid undefined behavior with future buffers
            aud_dec_svc_algorithmic_reset(capi_container->capi_ptr, index);

            break ;
         }
         else
         {
            remainingBytesAfter[index] = inpSizeBefore[index] - capi_container->in_buf[index].actual_data_len;
         }
      }
   }

   //WasPrevDecResNeedMore is only valid for 1st capi in the topology
   //capi_container->WasPrevResNeedMore = false;
   if(capi_index == 0)
   {
      pInpStrm->WasPrevDecResNeedMore = false;
   }

   if (CAPI_V2_SUCCEEDED(nResult) || (nResult==CAPI_V2_ENEEDMORE))
   {
      /* this condition handles
         success - some input consumed (or not), some out produced
         need_more - zero or all the input consumed (in either case we need to look into o/p buf), no output produced
         recovery_success - some in consumed, no output produced
       */

      //need more is detected whenever,
      //a) decode result is success or need_more
      //b) no output is produced in each of the outputs
      //c) no input is consumed from stream containing primary
      //when dec says need_more, one frame is not over yet, and we cannot sync to new buf TS, as the TS are synced to frames.
      //in rec success cases, there might be more frames in the buf (hence we won't even look into inp buf q until need-more is returned).
      bool_t out_produced = false, first_alive_port = true;

      //error out if some outputs produce while others not. (all outputs must be in sync)
      for(uint8_t index = 0 ; index < DEC_SVC_MAX_OUTPUT_STREAMS ; index++)
      {
         if(capi_container->out_buf[index].data_ptr)
         {
        	if (first_alive_port)
        	{
        		out_produced = (capi_container->out_buf[index].actual_data_len != 0)? true: false;
        		first_alive_port = false;
        	}
        	else if (out_produced != (capi_container->out_buf[index].actual_data_len != 0))
        	{
                MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "All outputs must be synced");
                return ADSP_EFAILED;
        	}
        	else
        	{
        		//this port output is in sync with previous output port.
        	}
         }
      }

	  bool_t was_need_more = false;
	  
      if(capi_index == 0)
      {
         for(uint8_t index = 0 ; index < DEC_SVC_MAX_INPUT_STREAMS ; index++)
         {
            if(capi_container->in_buf[index].data_ptr)
            {
               if ((remainingBytesAfter[index] == 0) ||
                     (remainingBytesAfter[index] == inpSizeBefore[index])) //if in didn't get consumed
               {
                  if (!out_produced) //no output produced
                  {
                     pMe->in_streams_ptr[index]->WasPrevDecResNeedMore = true;
                     was_need_more = true;
                  }
               }
            }
         }
      }

      if ( (capi_index != 0) && (was_need_more))
      {
         //MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "need more cannot be returned except by first decoder");
         //as first decoder must generate enough data for the later ones. If this happens, we still continue and it may work without TS, but with TS
         //it may not work. Also, if this is due to buffering in the CAPI, then there's no way to get those buffers back later as no additional calls will be made to the
         //CAPIs which buffer. process of additional CAPIs is called only if first one generates an output.
      }

      if (!was_need_more)
      {
         if(capi_container == pMe->mainCapiContainer)
         {
            AudioDecSvc_DecErrorEvent_Clear(&pInpStrm->dec_err_event);

#ifdef DBG_AUDIO_DECODE
            for(uint8_t index = 0 ; index < DEC_SVC_MAX_INPUT_STREAMS ; index++)
            {
               if(capi_container->in_buf[index].data_ptr)
               {
                  MSG_2(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "Decoding consumed(%d) input bytes at capi input port %d",
                        capi_container->in_buf[index].actual_data_len,(int)index );
               }
            }
#endif
         }
      }
      nResult = CAPI_V2_EOK; //change needmore to EOK
   }
   else //CAPI failed
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Decoding failed");

      if(capi_container == pMe->mainCapiContainer)
      {
         AudioDecSvc_DecErrorEvent_RecordError (&pInpStrm->dec_err_event);
      }
   }

   if (pMe->mainCapiContainer == capi_container) AudioDecSvc_DecErrorEvent_CheckRaise(pMe, pInpStrm, &pInpStrm->dec_err_event);


   for(uint8_t index = 0 ; (index < DEC_SVC_MAX_INPUT_STREAMS) &&  (capi_container->in_buf[index].data_ptr != NULL) ; index++)
   {
      //Get the consumed input length
      int32_t nBytesUsed = capi_container->in_buf[index].actual_data_len;

      memsmove(capi_container->in_buf[index].data_ptr,
            remainingBytesAfter[index],
            capi_container->in_buf[index].data_ptr + nBytesUsed,
            remainingBytesAfter[index] );
      capi_container->in_buf[index].actual_data_len = remainingBytesAfter[index];

      /*
       * adjust TS while adjusting input if input is PCM or COP. TS corresponds to
       * the first input sample in the buffer if it is PCM media fmt
       */
       if(pMe->is_rt_dec)
       {
          AudioDecSvc_Process_UpdateCapiInputTS(capi_container, nBytesUsed, pInpStrm, index);
       }
   }


   return capi_v2_err_to_adsp_result(nResult);
}

static ADSPResult AudioDecSvc_PostProcess(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_any_t *media_fmt_msg)
{
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   AudioDecSvc_InpStream_t * pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe);

   uint8_t capi_out_index = AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm);
   if(NULL == last_capi)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:Failed to get last capi");
      return ADSP_EFAILED;
   }

   //if input is\ 16 bits and output is 24/32 bits need to shift
   if((16 == last_capi->PrevFmtBlk[capi_out_index].bits_per_sample) && (24 == pOutStrm->output_bits_per_sample))
   {
      /* 16 -> 24 bit conversion if required */
      uint32_t num_channels = last_capi->PrevFmtBlk[capi_out_index].num_channels;
      uint32_t num_samples_per_chan = (last_capi->out_buf[capi_out_index].actual_data_len) /
            (num_channels * BYTES_PER_SAMPLE_TWO);
      int8_t * final_dst_ptr = (int8_t *)last_capi->out_buf[capi_out_index].data_ptr;

      memscpy(last_capi->scratch_out_buf, last_capi->out_buf[capi_out_index].max_data_len,
            final_dst_ptr, last_capi->out_buf[capi_out_index].actual_data_len);

      /* Q15->Q27 conversion */
      byte_up_down_convertor_deintlv((int8_t *)last_capi->scratch_out_buf, final_dst_ptr, num_samples_per_chan, num_channels,
            num_samples_per_chan, num_samples_per_chan, BYTE_UP_CONV, PCM_16BIT_Q_FORMAT, ELITE_32BIT_PCM_Q_FORMAT);

      /* Double the out size in bytes */
      last_capi->out_buf[capi_out_index].actual_data_len <<= 1;
   }

   DecChannelMap_t channel_cfg_after;
   channel_cfg_after.nChannels = last_capi->PrevFmtBlk[capi_out_index].num_channels;
   for (uint16_t ch=0; ch < channel_cfg_after.nChannels; ch++)
   {
      channel_cfg_after.nChannelMap[ch] = last_capi->PrevFmtBlk[capi_out_index].channel_mapping[ch];
   }

   if ( last_capi->media_fmt_event[capi_out_index] )
   {
      // if there is any change in fs/channel mapping from
      // previous frame send media format to down stream before the
      // fresh data is enqueued
      //notify client about the changes if the event is enabled
	  //TODO: SrCm event should be a property of output stream.
      (void) AudioDecSvc_SendSrCmEvent(pMe,pInpStrm,
            last_capi->PrevFmtBlk[capi_out_index].sample_rate,
            &channel_cfg_after);

      uint32_t num_ch = channel_cfg_after.nChannels;
      //recompute output channel mapping that will be sent to downstream svc
      AudioDecSvc_GetOutChanMap(&channel_cfg_after, &pOutStrm->out_chan_map);

      if ( (pOutStrm->common.total_bytes != 0) &&
            (num_ch != 0) &&
            (num_ch != channel_cfg_after.nChannels) )
      {
         pOutStrm->common.total_bytes = pOutStrm->common.total_bytes * channel_cfg_after.nChannels / num_ch;
      }

      AudioDecSvc_GetMediaFmtUpdateMsg(pMe,pInpStrm, pOutStrm,
            last_capi->PrevFmtBlk[capi_out_index].sample_rate,
            pOutStrm->out_chan_map.num_out_chan,
            pOutStrm->out_chan_map.out_chan,
            pOutStrm->output_bits_per_sample,
            last_capi->out_data_fmt_type[0],
            last_capi->dec_fmt_id,
            media_fmt_msg
            );

      //clear the event
      last_capi->media_fmt_event[capi_out_index] = false;
   }

   uint32_t init_samples_removed_before = pInpStrm->silence_removal.initial_samples_to_remove;
   //check if we need to remove initial/trailing silence
   //TODO: mimo case handling.
   if(pInpStrm->silence_removal.initial_samples_to_remove > 0)
   {
      AudioDecSvc_RemoveInitialSamples(&pInpStrm->silence_removal.initial_samples_to_remove,
            (uint32_t *)&last_capi->out_buf[capi_out_index].actual_data_len,
            (uint8_t *)last_capi->out_buf[capi_out_index].data_ptr,
            last_capi->PrevFmtBlk[capi_out_index].num_channels,
            last_capi->PrevFmtBlk[capi_out_index].sample_rate,
            &pInpStrm->TsState.ullNextOutBufTS,
            &pInpStrm->TsState.ullNextOutBufSampleCount,
            pOutStrm->output_bytes_per_sample);
   }
   uint32_t init_samples_removed_after = pInpStrm->silence_removal.initial_samples_to_remove;

   if (pOutStrm->early_eos_ptr)
   {
      if (init_samples_removed_after != init_samples_removed_before)
      {
         pOutStrm->early_eos_ptr->is_init_samples_removed = TRUE;
      }
      else
      {
         pOutStrm->early_eos_ptr->is_init_samples_removed = FALSE;
      }
   }

   //Check if last buffer is received on default input stream, before removing trailing silences.
   if(pInpStrm->silence_removal.last_buffer)
   {
      //Currently trailing silence removal is only applicable for last frame
      //Make sure this is the last frame by seeing if all data in internal buffer has been
      //consumed and no data remains in the client buffer. Also if we have consumed all data
      //in client buffer and run into error treat this as last frame.
      if((last_capi->in_buf[AudioDecSvc_GetInputStreamIndex(pMe,pInpStrm)].actual_data_len == 0)  &&
            (pInpStrm->inp_buf_params.unMemSize == 0) &&
            (pInpStrm->silence_removal.trailing_samples_to_remove > 0))
      {
         AudioDecSvc_RemoveTrailingSamples(pInpStrm->silence_removal.trailing_samples_to_remove,
               (uint32_t *)&last_capi->out_buf[capi_out_index].actual_data_len,
               (uint8_t *)last_capi->out_buf[capi_out_index].data_ptr,
               last_capi->PrevFmtBlk[capi_out_index].num_channels,
               pOutStrm->output_bytes_per_sample);
      }
   }

   uint32_t samples_per_ch =
         last_capi->out_buf[capi_out_index].actual_data_len/(last_capi->PrevFmtBlk[capi_out_index].num_channels * pOutStrm->output_bytes_per_sample);

   uint32_t* pOutBufActualSize = (uint32_t *) &(((elite_msg_data_buffer_t*)(pOutStrm->outDataBufferNode.pBuffer))->nActualSize);

   AudioDecSvc_GetOutput(pMe,
         pOutStrm,
         last_capi,
         &channel_cfg_after,
         (void*) last_capi->out_buf[capi_out_index].data_ptr,
         samples_per_ch,
         pOutStrm->output_bytes_per_sample,
         pOutBufActualSize);

   //if it is real-time decoder then set output TS from last CAPI output TS
   if(pMe->is_rt_dec && samples_per_ch)
   {
      pInpStrm->TsState.ullNextOutBufTS = last_capi->outputs[capi_out_index].timestamp;
      last_capi->outputs[capi_out_index].timestamp = last_capi->inputs[capi_out_index].timestamp;
#ifdef AUDENCDEC_TIMESTAMP_LOGS
   MSG_3(MSG_SSID_QDSP6,  DBG_HIGH_PRIO, "New output TS: %lu %lu, samples_per_ch=%ld ",
         (uint32_t)(last_capi->outputs[capi_out_index].timestamp>>32), (uint32_t)last_capi->outputs[capi_out_index].timestamp,
         samples_per_ch);
#endif

   }
   return ADSP_EOK;
}


static void AudioDecSvc_HandleEosCases(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, bool_t outBufNotEmpty)
{
   if(pMe->capiContainer[0]->dec_fmt_id == ASM_MEDIA_FMT_DTMF)
   {
      AudioDecSvc_ListenToOutput(pMe);

      uint32_t bDtmfRunning=0;
      aud_dec_svc_get_param(pMe->capiContainer[0]->capi_ptr, CAPI_PARAM_DTMF_RUNNING, (int8_t*)&bDtmfRunning, sizeof(bDtmfRunning));

      // This condition will be true if the tone is completely played out.
      if((FALSE == bDtmfRunning) && (outBufNotEmpty))
      {
         //send EOS
         AudioDecSvc_SendDtmfToneEndedEvent(pMe);
      }
   }

   //If this was the last input buffer and we have completely processed then propagte EOS downstream.
   //If an error occurs while processing this buffer then it is unrecoverable and so send EOS anyway.
   //There must be not valid buffer in circular list
   if(pMe->need_to_send_eos)
   {
      // when it's time to send EoS but for buffers left in circ buffer, raise early-eos.
      // at this time all of bitstream is decoded.
      if (pInpStrm->endOfStream == AUD_DEC_EOS_V2)
      {
         AudioDecSvc_EarlyEoS_Schedule(pMe, pOutStrm);
      }

      if(AudioDecSvc_CircBuf_IsEmpty(&pOutStrm->CircBufList))
      {
     	 //disable the output signals if no further data buffer is pending.
    	 uint32_t index = AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm);
     	 AudioDecSvc_StopListenToMask(pMe, AUD_DEC_OUT_INDEX_TO_DATA_SIG(index));

     	 uint32_t allOutMask = AudioDecSvc_GetAllOutputStreamMask(pMe);

     	 if(pMe->unCurrentBitfield & allOutMask) //If still listening to some output
     	 {
     		 // send eos to peer service for current output stream
     		 AudioDecSvc_ProcessEos(pMe, pInpStrm, pOutStrm, (bool_t)FALSE);
     	 }
     	 else //If not listening to any output mask.
     	 {
             // send eos to peer service for last output stream and reset decoder
             AudioDecSvc_ProcessEos(pMe, pInpStrm, pOutStrm, (bool_t)TRUE);
             AudioDecSvc_FreeInputDataCmd(pMe, pInpStrm,  ADSP_EOK);
             //EOS is processed for all output stream; wait on input queue
             AudioDecSvc_ListenToInput(pMe);
     	 }
      }

      //After EoS, stop waking up for empty buffers in circ buf. wake up for only output buffers.
      AudioDecSvc_EarlyEos_ClearSignal(pMe, pOutStrm);
   }
}

static void AudioDecSvc_Reset_SecondaryInpStream(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm)
{
	dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
	uint32_t capi_input_port = AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm);

	//Clear CapiV2 internal input buffer.
	first_capi->in_buf[capi_input_port].actual_data_len = 0;
	first_capi->inputs[capi_input_port].flags.marker_eos = false;
	first_capi->inputs[capi_input_port].flags.is_timestamp_valid = false;
	first_capi->inputs[capi_input_port].flags.end_of_frame = false;
	first_capi->bytes_logged = 0;

	//reset decoder CAPI input port.
	if (first_capi->capi_ptr)
	{
		aud_dec_svc_algorithmic_reset(first_capi->capi_ptr, capi_input_port);
	}

	//increment log id so that after every reset EoS, fresh log file is created.
	pInpStrm->common.ulDataLogId++;

	pInpStrm->endOfStream = AUD_DEC_EOS_NONE;

	AudioDecSvc_DecErrorEvent_Reset(&pInpStrm->dec_err_event);

	//reset timestamp state
	(void) AudioDecSvc_InitTsState(&pInpStrm->TsState);

	pInpStrm->WasPrevDecResNeedMore = false;

}

static ADSPResult AudioDecSvc_SendOutBufs(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult nResult = ADSP_EOK;

   AudioDecSvc_InpStream_t *pInpStrm = AudioDecSvc_GetDefaultInputStream(pMe); //TODO: this won't work for MIMO

   /* meta data */
   //if meta-data has to be sent and there's an output queue proceed, else return & wait for output buf.
   //check if CAPI has some meta-data to send now for this output port.
   if (pMe->mainCapiContainer->metadata_available_event[AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm)])
   {
      if ( 0 != qurt_elite_channel_poll(&(pMe->channel), AudioDecSvc_GetAllOutputStreamMask(pMe)) )
      {
         AudioDecSvc_SendMetadata(pMe, pOutStrm);
      }
      else
      {
         //wait for output buffer, EoS will be sent afterwards, after sending metadata & out buf.
         pOutStrm->metadata_xfr.curr_wait_mask_backup = pMe->unCurrentBitfield;
         pOutStrm->metadata_xfr.is_waiting_to_push_metadata = TRUE;
         AudioDecSvc_ListenToOutput(pMe);
         return nResult;
      }
   }

   elite_msg_data_buffer_t *pOutBuf = (elite_msg_data_buffer_t*) (pOutStrm->outDataBufferNode.pBuffer);

   pOutStrm->common.total_bytes += pOutBuf->nActualSize;

   AudioDecSvc_HandlePrebuffer(pMe, pOutStrm, pInpStrm);

   bool_t outBufNotEmpty =  pOutBuf->nActualSize > 0;

   nResult = AudioDecSvc_SendPcmToPeerSvc(pMe, pInpStrm, pOutStrm, pOutBuf);

   if (pOutStrm->metadata_xfr.is_waiting_to_push_metadata)
   {
      //restore the mask
      pMe->unCurrentBitfield = pOutStrm->metadata_xfr.curr_wait_mask_backup;
      pOutStrm->metadata_xfr.is_waiting_to_push_metadata = FALSE;
   }

   /* EoS */
   AudioDecSvc_HandleEosCases(pMe, pInpStrm, pOutStrm, outBufNotEmpty);

   return nResult;
}

static void AudioDecSvc_SendMetadata(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   ADSPResult nResult;
   uint32_t buf_size;
   elite_msg_data_set_param_t *pMsgHeader;
   uint8_t *ptr;
   elite_msg_any_t *msg;

   qurt_elite_bufmgr_node_t outBufMgrNode;
   // Take next buffer off the output buf q
   if(ADSP_FAILED(nResult = qurt_elite_queue_pop_front(pOutStrm->pOutBufQ, (uint64_t*)&(outBufMgrNode))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc:PBQ:Failure on Out Buf Pop. Dropping metadata. ");
      goto __bailout;
   }

   // Create a set_param message using the output buffer mgr node.
   msg = elite_msg_convt_buf_node_to_msg(
         &outBufMgrNode,
         ELITE_DATA_SET_PARAM,
         NULL, /* do not need response */
         0,    /* token */
         0     /* do not care response result*/
   );

   //elite_msg_data_set_param_t is followed by many groups of { asm_stream_param_data_t & value }
   ptr = (uint8_t*)msg->pPayload;

   pMsgHeader = (elite_msg_data_set_param_t*)ptr;
   buf_size = pOutStrm->maxExtBufSize;

   ptr += sizeof(elite_msg_data_set_param_t);

   //ask CAPI to populate metadata into the ptr
   nResult = aud_dec_svc_get_metadata(pMe->mainCapiContainer->capi_ptr,
         AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm), ptr, &buf_size);
   if (ADSP_FAILED(nResult))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc: Dropping metadata due to error in getparam. ");
      goto __bailout1;
   }

   if (buf_size == 0)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioDecSvc: metadata size is zero! ");
      goto __bailout1;
   }

   pMsgHeader->unPayloadSize = buf_size;

   if (ADSP_FAILED(qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)msg )))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to send data set param message. Dropping metadata.");
      goto __bailout1;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Sent data set param to PP with metadata");
   }

   pMe->mainCapiContainer->metadata_available_event[AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm)] = false;

   return;

   __bailout1:
   (void) elite_msg_push_payload_to_returnq(pOutStrm->pOutBufQ, (elite_msg_any_payload_t*)(outBufMgrNode.pBuffer));
   __bailout:
   pMe->mainCapiContainer->metadata_available_event[AudioDecSvc_GetOutputStreamIndex(pMe, pOutStrm)] = false;

   return;
}

static ADSPResult AudioDecSvc_HandleResync(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
    ADSPResult nRes = ADSP_EOK;

    elite_msg_any_t resyncBuffer;
    uint32_t unResyncBufPayloadSize = sizeof(elite_msg_data_resync_t);
    nRes = elite_msg_create_msg( &resyncBuffer,
                    &unResyncBufPayloadSize,
                    ELITEMSG_DATA_RESYNC,
                    NULL,   // do not need response
                    0,      // token : store delay in the token
                    0);     // do not care response result

    if (ADSP_FAILED(nRes))
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioDecSvc: ERROR allocating Resync Buffer");
        return nRes;
    }
    //TODO: Revisit for MIMO
    if (ADSP_FAILED(nRes = qurt_elite_queue_push_back(pOutStrm->pDownStreamSvc->dataQ, (uint64_t*)&resyncBuffer)))
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioDecSvc: ERROR sending Resync Buffer downstream");
        elite_msg_release_msg(&resyncBuffer);
        return nRes;
    }
   return nRes;
}
