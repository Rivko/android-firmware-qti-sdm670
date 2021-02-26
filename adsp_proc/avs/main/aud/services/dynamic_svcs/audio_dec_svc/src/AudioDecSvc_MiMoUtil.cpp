/*========================================================================

 *//** @file AudioDecSvc_MimoUtil.cpp
This file contains utility functions for MIMO of Elite Audio Decoder service.

Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_MiMoUtil.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
11/14/13   rbhatnk      Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "EliteMsg_Custom.h"
#include "AudioStreamMgr_AprIf.h"
#include "AdspCoreSvc.h"

#include "AudioDecSvc.h"
#include "AudioDecSvc_Util.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc_MiMoUtil.h"
#include "AudioDecSvc_PullMode.h"
#include "AudioDecSvc_CapiV2Util.h"

extern uint32_t asmMemoryMapClient;
/**
 * bandwidth required for single stream decode
 * values are so due to legacy implementation.
 *
 * assuming 48kHz sampling, 2 channels, 2 bytes per channel.
 * in bytes per second.
 */
#define SINGLE_STREAM_DECODED_BW  (48000 * 2 * 2)

char AUDIO_DEC_SVC_DATA_Q_NAME[]       = "DDSd";
static char AUDIO_DEC_SVC_BUF_Q_NAME[] = "DDSb";

static const uint32_t MIN_SAMPLE_FREQ = 8000;
static const uint32_t MIN_CHANNEL = 1;
static const uint32_t MIN_BYTES_PER_SAMPLE = 2;

// for in-out streams both input and output streams exist with same id.
AudioDecSvc_InpStream_t * AudioDecSvc_GetInputStreamById(AudioDecSvc_t *pMe, uint16_t stream_id)
{
   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if ( (pMe->in_streams_ptr[i]) && (pMe->in_streams_ptr[i]->common.stream_id == stream_id))
         return pMe->in_streams_ptr[i];
   }
   return NULL;
}
// for in-out streams both input and output streams exist with same id.
AudioDecSvc_OutStream_t * AudioDecSvc_GetOutputStreamById(AudioDecSvc_t *pMe, uint16_t stream_id)
{
   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if ( (pMe->out_streams_ptr[i]) && (pMe->out_streams_ptr[i]->common.stream_id == stream_id))
         return pMe->out_streams_ptr[i];
   }
   return NULL;
}

static uint32_t   get_num_io_streams(AudioDecSvc_t *pMe)
{
   uint32_t in_count = 0, out_count = 0;

   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if ( (pMe->in_streams_ptr[i]) && (pMe->in_streams_ptr[i]->common.io_type == AUDIO_DEC_SVC_IN_OUT_STREAM))
         in_count++;
   }

   for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
   {
      if ( (pMe->out_streams_ptr[i]) && (pMe->out_streams_ptr[i]->common.io_type == AUDIO_DEC_SVC_IN_OUT_STREAM))
         out_count++;
   }

   if ( (in_count == out_count) && (1 == in_count) )
      return in_count;

   return (in_count+out_count);
}

/**
 * decoder is already opened with either an input or an output stream. this command is sent to open more streams -either input or output;
 */
ADSPResult AudioDecSvc_OpenStream(AudioDecSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomDecOpenStreamType* pPayload = (EliteMsg_CustomDecOpenStreamType*)((pMe->cmdMsg).pPayload);

   //below condition prevents opening of IO first and then opening MIMO.
   //Due to this condition it can be safely assumed that for regular SISO decode case, input and output can be associated with each other.
   if (get_num_io_streams(pMe) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create the decoder stream since more than one IO stream present.");
      result = ADSP_EBADPARAM;
   }
   else
   {
	  if (pPayload->openParams.in.io_fmt_conv != DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO)
	  {
		  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MIMO is only supported with auto capi.");
		  return ADSP_EBADPARAM;
	  }

      switch(pPayload->openParams.io_type)
      {
      case AUDIO_DEC_SVC_MIMO_INPUT_STREAM:
      {
         result = AudioDecSvc_CreateInpStream(pMe, &pPayload->openParams, (void**)&pPayload->pHandle);
         break;
      }
      case AUDIO_DEC_SVC_MIMO_OUTPUT_STREAM:
      {
         result = AudioDecSvc_CreateOutStream(pMe, &pPayload->openParams, (void**)&pPayload->pHandle);
         break;
      }
      case AUDIO_DEC_SVC_IN_OUT_STREAM:
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder service creation received invalid io_type=%u ",pPayload->openParams.io_type);
         result = ADSP_EBADPARAM;
      }
      }
   }

   elite_msg_finish_msg(&(pMe->cmdMsg), result);

   return result;
}

ADSPResult AudioDecSvc_CloseStream(AudioDecSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   EliteMsg_CustomDecCloseStreamType* pPayload = (EliteMsg_CustomDecCloseStreamType*)((pMe->cmdMsg).pPayload);

   bool_t stream_found = false;
   for (uint16_t i=0; i < DEC_SVC_MAX_INPUT_STREAMS; i++)
   {
      if ( (pMe->in_streams_ptr[i]) && (&pMe->in_streams_ptr[i]->common.serviceHandle == pPayload->pHandle))
      {
         AudioDecSvc_DestroyInpStream(pMe, &pMe->in_streams_ptr[i]);
         stream_found = true;
         break;
      }
   }

   if (!stream_found)
   {
      for (uint16_t i=0; i < DEC_SVC_MAX_OUTPUT_STREAMS; i++)
      {
         if ( (pMe->out_streams_ptr[i]) && (&pMe->out_streams_ptr[i]->common.serviceHandle == pPayload->pHandle))
         {
            AudioDecSvc_DestroyOutStream(pMe, &pMe->out_streams_ptr[i]);
            break;
         }
      }
   }

   //release=false. vote only if there's an event. when CAPI sees one port is closed, it would've raised event.
   AudioDecSvc_ProcessKppsBw(pMe, FALSE, FALSE);

   elite_msg_finish_msg(&(pMe->cmdMsg), result);

   return result;
}

static void AudioDecSvc_CopyCommonInitParams(AudioDecSvc_t *pMe, AudioDecSvc_CommonStream_t *pCommon, AudioDecSvcIOStreamInitParams_t *pIOInitParams)
{
   pCommon->stream_id = pIOInitParams->stream_id;
   pCommon->io_type = pIOInitParams->io_type;
   pCommon->stream_type = pIOInitParams->stream_type;
   pCommon->serviceHandle.unSvcId = ELITE_DYN_DECODER_SVCID;
   pCommon->serviceHandle.threadId = pMe->thread_id;

   // Make a copy of ASM call back structure
   if (pIOInitParams->pCbData)
   {
      memscpy( &pCommon->CallbackHandle, sizeof(AudioStreamMgr_CallBackHandleType),
            pIOInitParams->pCbData, sizeof(AudioStreamMgr_CallBackHandleType));
   }

   pCommon->ulDataLogId = pIOInitParams->ulDataLogId;
   pCommon->serviceHandle.cmdQ = pMe->pCmdQ;

   //store the perf_mode received from ASM. The perf_mode is currently only applicable to PCM playback
   pCommon->perf_mode = pIOInitParams->perf_mode;

   pCommon->delay_ptr = pIOInitParams->pDelay;
   *pCommon->delay_ptr = 0;

   pCommon->pEventQ = pIOInitParams->pEventQ;
   pCommon->eventToken = pIOInitParams->eventToken;


   return;
}

ADSPResult AudioDecSvc_DestroyOutStream(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t **ppOutStream)
{
   ADSPResult result = ADSP_EOK;
   
   if ((*ppOutStream)->outDataBufferNode.pBuffer)
   {
	   AudioDecSvc_ReturnOutBuf(pMe, *ppOutStream);
   }

   while (0 < (*ppOutStream)->nBufsAllocated) //Destroy all the allocated buffers.
   {
	   (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel((*ppOutStream)->pOutBufQ),
			   qurt_elite_queue_get_channel_bit((*ppOutStream)->pOutBufQ));

	   AudioDecSvc_DestroyExternalBufs(pMe, *ppOutStream);
   }

   AudioDecSvc_DestroyCircularBufs(pMe, *ppOutStream);

   //destroy scratch buffer
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   if((last_capi) && (last_capi->scratch_out_buf != NULL))
   {
      qurt_elite_memory_free(last_capi->scratch_out_buf);
      last_capi->scratch_out_buf = NULL;
   }

   AudioDecSvc_EarlyEoS_Destroy(pMe, *ppOutStream);

   AudioDecSvc_DestroyOutputDataQ(pMe, *ppOutStream);

   qurt_elite_memory_free(*ppOutStream);
   *ppOutStream = NULL;

   return result;
}

ADSPResult AudioDecSvc_DestroyInpStream(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t **ppInpStream)
{
   ADSPResult result = ADSP_EOK;

   //destroy the main input buffer
   dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe);
   uint32_t stream_index = AudioDecSvc_GetInputStreamIndex(pMe, *ppInpStream);

   if( first_capi->in_buf[stream_index].data_ptr != NULL)
   {
      qurt_elite_memory_free( first_capi->in_buf[stream_index].data_ptr);
      first_capi->in_buf[stream_index].data_ptr = NULL;

      first_capi->inputs[stream_index].buf_ptr = NULL;
      first_capi->inputs_ptr[stream_index] = NULL;
   }

   AudioDecSvc_DeInitPullMode(pMe, *ppInpStream);

   AudioDecSvc_DestroyInputDataQ(pMe, *ppInpStream);

   qurt_elite_memory_free(*ppInpStream);
   *ppInpStream = NULL;

   //TODO: Send stream close info to capiv2 input port.
   //It is possible that only input stream is getting destroyed and it will be reopened later.

   return result;
}

void AudioDecSvc_DestroyOutputDataQ (AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStream)
{
   // clean up buffer queue
   if (pOutStream->pOutBufQ)
   {
      qurt_elite_queue_destroy(pOutStream->pOutBufQ);
   }
}
void AudioDecSvc_DestroyInputDataQ(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStream)
{
   // clean up data queue
   if (pInpStream->common.serviceHandle.dataQ)
   {
      AudioDecSvc_FlushInputDataQ(pMe, pInpStream);
      qurt_elite_queue_destroy(pInpStream->common.serviceHandle.dataQ);
   }
}

static uint32_t power_of_2(uint32_t n)
{
   n = n - 1;
   n |= (n >> 1);
   n |= (n >> 2);
   n |= (n >> 4);
   n |= (n >> 8);
   n |= (n >> 16);
   n += 1;
   return n;
}

ADSPResult AudioDecSvc_CreateInpDataQ(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_CAPI_container_t *last_capi)
{
   ADSPResult result = ADSP_EOK;
   char aDataQName[QURT_ELITE_DEFAULT_NAME_LEN];    //data queue name
   uint16_t stream_index=0;

   if (DEC_SVC_PULL_MODE == pMe->ulMode)
   {
      return result;
   }

   int count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;

   snprintf(aDataQName, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x",AUDIO_DEC_SVC_DATA_Q_NAME, count);


   uint32_t num_elements = MAX_INP_DATA_Q_ELEMENTS;
   //only for real-time path(AFE) decide num of buf based on last capi output. last capi output decides max time to decode.
   if ((last_capi) && (pMe->is_rt_dec))
   {
      uint32_t worst_case_frame_len = last_capi->out_buf[0].max_data_len / ((MIN_SAMPLE_FREQ * MIN_CHANNEL * MIN_BYTES_PER_SAMPLE)/1000); //TODO: this will not work with MIMO
      if (worst_case_frame_len != 0)
      {
         num_elements = power_of_2(worst_case_frame_len); //assume one ms buffers
      }
      if (num_elements > MAX_INP_DATA_Q_ELEMENTS_RT)
      {
         num_elements = MAX_INP_DATA_Q_ELEMENTS_RT;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioDecSvc number of dataQ elements is determined to be %lu", num_elements);

   if ( (ADSP_FAILED(result = qurt_elite_queue_create(aDataQName,
         num_elements, &(pInpStrm->common.serviceHandle.dataQ))))
   )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create Decoder service message queues!! \n");
      return result;
   }

   stream_index = AudioDecSvc_GetInputStreamIndex(pMe, pInpStrm);

   if (( ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pInpStrm->common.serviceHandle.dataQ, AUD_DEC_INP_INDEX_TO_DATA_SIG(stream_index))))   )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to add mqs to channel result = %d!!\n", result);
      return result;
   }
   return result;
}

ADSPResult AudioDecSvc_CreateInpStream(AudioDecSvc_t *pMe, AudioDecSvcIOStreamInitParams_t *pIOInitParams, void **handle)
{
   ADSPResult result = ADSP_EOK;
   uint16_t stream_index=0;

   CHECK_ERR_BITS_PER_SAMPLE(pIOInitParams->bits_per_sample)

   if (pIOInitParams->in.no_apr_dec &&
         (DEC_SVC_PULL_MODE == pIOInitParams->in.ulMode) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "pull mode cannot exist without APR client");
      return ADSP_EUNSUPPORTED;
   }

   if ( (DEC_SVC_IO_FORMAT_CONV_TYPE_AUTO != pIOInitParams->in.io_fmt_conv) &&
         ( (AudioDecSvc_IsPcmFmt(pIOInitParams->format_id) ||
               (ASM_MEDIA_FMT_DTMF == pIOInitParams->format_id) ||
               (DEC_SVC_PULL_MODE == pIOInitParams->in.ulMode) ||
               (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pIOInitParams->perf_mode) ||
               (ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION == pIOInitParams->perf_mode) )
         )
   )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Only auto mode supported for PCM (regular, pull mode, ULL mode) OR DTMF");
      return ADSP_EUNSUPPORTED;
   }

   if ( (DEC_SVC_UNKNOWN_MODE != pMe->ulMode) && (pMe->ulMode != pIOInitParams->in.ulMode))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Decoder mode is %d. trying to overwrite with mode %d. error.", pMe->ulMode, pIOInitParams->in.ulMode);
      return result;
   }

   pMe->ulMode = pIOInitParams->in.ulMode;

   //Get the next free stream.
   for (stream_index=0; stream_index < DEC_SVC_MAX_INPUT_STREAMS; stream_index++)
   {
      if (NULL == pMe->in_streams_ptr[stream_index]) break;
   }

   if(DEC_SVC_MAX_INPUT_STREAMS == stream_index)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "All streams are occupied, cannot create a new stream!");
      return ADSP_EFAILED;
   }

   pMe->in_streams_ptr[stream_index] = (AudioDecSvc_InpStream_t*) qurt_elite_memory_malloc( sizeof(AudioDecSvc_InpStream_t), ADEC_SVC_OBJ_INSTANCE);
   if (!pMe->in_streams_ptr[stream_index])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create new input stream in AudDecSvc.It requires %lu bytes",sizeof(AudioDecSvc_InpStream_t));
      return ADSP_ENOMEMORY;
   }

   memset(pMe->in_streams_ptr[stream_index], 0, sizeof(AudioDecSvc_InpStream_t));

   //initialize time stamp state
   (void) AudioDecSvc_InitTsState(&pMe->in_streams_ptr[stream_index]->TsState);

   pMe->in_streams_ptr[stream_index]->WasPrevDecResNeedMore = false;

   //intialize the memory map client for DecSvc
   pMe->in_streams_ptr[stream_index]->inp_buf_params.unMemMapClient = asmMemoryMapClient;

   //sr/cm change notification event
   pMe->in_streams_ptr[stream_index]->bEnableSrCmEvent = pIOInitParams->in.bEnableSrCmEvent;
   pMe->in_streams_ptr[stream_index]->no_apr_dec = pIOInitParams->in.no_apr_dec;

   pMe->in_streams_ptr[stream_index]->io_fmt_conv = pIOInitParams->in.io_fmt_conv;

   AudioDecSvc_CopyCommonInitParams(pMe, &pMe->in_streams_ptr[stream_index]->common, pIOInitParams);

   result = AudioDecSvc_InitPullMode(pMe, pMe->in_streams_ptr[stream_index], pIOInitParams);
   if (ADSP_FAILED(result))
   {
      return result;
   }

   //For real time decoding, output stream must have been created at this point.
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   result |= AudioDecSvc_CreateInpDataQ(pMe, pMe->in_streams_ptr[stream_index], last_capi);
   if (ADSP_FAILED(result)) return result;

   dec_CAPI_container_t *first_capi = AudioDecSvc_GetFirstCapi(pMe); //input stream can be opened only for first capi
   if(!first_capi) return ADSP_EUNEXPECTED;
   if(first_capi->capi_ptr != NULL) //Non PCM decoder.
   {
      dec_capi_port_index_t in_capi_port_index;
      in_capi_port_index.index = stream_index;
      in_capi_port_index.valid = true;

      //TODO: before querying CAPI for anything we need to set the port properties (whether this port inputs pri, sec etc & the fmt id)

      uint32_t inBufSize;
      result |= aud_dec_svc_get_port_thresh(first_capi->capi_ptr, CAPI_INPUT_PORT, in_capi_port_index.index, &inBufSize);
      result |= AudioDecSvc_CreateInternalInputBufs(pMe, first_capi,inBufSize,in_capi_port_index);

      if (ADSP_FAILED(result)) return result;

      //capi must've raised event by now since new port is opened.
      (void)AudioDecSvc_ProcessKppsBw(pMe, FALSE, FALSE);

   }

   *handle = &(pMe->in_streams_ptr[stream_index]->common.serviceHandle);

   pMe->in_streams_ptr[stream_index]->common.serviceHandle.threadId = pMe->thread_id;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dec svc created input stream_id 0x%x, at index %u", pMe->in_streams_ptr[stream_index]->common.stream_id, stream_index);

   return result;
}

ADSPResult AudioDecSvc_CreateOutStream(AudioDecSvc_t *pMe, AudioDecSvcIOStreamInitParams_t *pIOInitParams, void **handle)
{
   int count;
   ADSPResult result = ADSP_EOK;
   uint16_t stream_index;
   char aBufQName[QURT_ELITE_DEFAULT_NAME_LEN];     //output buffer queue name

   CHECK_ERR_BITS_PER_SAMPLE(pIOInitParams->bits_per_sample)

   for (stream_index=0; stream_index < DEC_SVC_MAX_OUTPUT_STREAMS; stream_index++)
   {
      if (NULL == pMe->out_streams_ptr[stream_index]) break;
   }
   if(DEC_SVC_MAX_OUTPUT_STREAMS == stream_index)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "All stream IDs occupied, cannot create a new stream!");
      return ADSP_EFAILED;
   }

   pMe->out_streams_ptr[stream_index] = (AudioDecSvc_OutStream_t*) qurt_elite_memory_malloc( sizeof(AudioDecSvc_OutStream_t), ADEC_SVC_OBJ_INSTANCE);
   if (!pMe->out_streams_ptr[stream_index])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Insufficient memory to create new output stream in AudDecSvc.It requires %lu bytes",sizeof(AudioDecSvc_OutStream_t));
      return ADSP_ENOMEMORY;
   }
   memset(pMe->out_streams_ptr[stream_index], 0, sizeof(AudioDecSvc_OutStream_t));

   count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;

   snprintf(aBufQName, QURT_ELITE_DEFAULT_NAME_LEN,"%s%x", AUDIO_DEC_SVC_BUF_Q_NAME,count);

   if ( ADSP_FAILED(result = qurt_elite_queue_create(aBufQName,
         MAX_OUT_BUF_Q_ELEMENTS, &(pMe->out_streams_ptr[stream_index]->pOutBufQ))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create Decoder service message queues!! \n");
      return result;
   }

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->out_streams_ptr[stream_index]->pOutBufQ, AUD_DEC_OUT_INDEX_TO_DATA_SIG(stream_index))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to add queue to the decoder's channel, result = %d\n",result);
      return result;
   }

   //by default all speaker positions are allowed in the output
   pMe->out_streams_ptr[stream_index]->out_chan_map.max_map.nChannels = MAX_SPKR_POS;
   for(uint32_t i = 0; i < MAX_SPKR_POS; i++)
   {
      pMe->out_streams_ptr[stream_index]->out_chan_map.max_map.nChannelMap[i] = i+1; // 1 to 16
   }
   pMe->out_streams_ptr[stream_index]->out_chan_map.is_out_chan_map_received = false;

   pMe->out_streams_ptr[stream_index]->decoderConverterMode = pIOInitParams->decoderConverterMode;
   /* Store the bits per sample received from ASM */
   pMe->out_streams_ptr[stream_index]->output_bits_per_sample = pIOInitParams->bits_per_sample;
   pMe->out_streams_ptr[stream_index]->output_bytes_per_sample = (pIOInitParams->bits_per_sample > 16) ? BYTES_PER_SAMPLE_FOUR : BYTES_PER_SAMPLE_TWO;

   pMe->out_streams_ptr[stream_index]->downstream_buf_delay_ptr = pIOInitParams->out.downstream_buf_delay_ptr;

   AudioDecSvc_CopyCommonInitParams(pMe, &pMe->out_streams_ptr[stream_index]->common, pIOInitParams);

   //output stream can be opened only for last CAPI
   dec_CAPI_container_t *last_capi = AudioDecSvc_GetLastCapi(pMe);
   uint32_t numOutBufs = 0;
   if(!last_capi) return ADSP_EUNEXPECTED;
   if(last_capi->capi_ptr != NULL)
   {
      dec_capi_port_index_t out_capi_port_index;
      out_capi_port_index.index = stream_index;
      out_capi_port_index.valid = true;

      //TODO: before querying CAPI for anything we need to set the port properties (whether this port output pri, sec etc )
      uint32_t outBufSize;
      result = aud_dec_svc_get_port_thresh(last_capi->capi_ptr, CAPI_OUTPUT_PORT,
    		  	  	  	  	  	  	  	   out_capi_port_index.index, &outBufSize);
      if (ADSP_FAILED(result)) return result;

      uint32_t max_metadata_size;
      result = aud_dec_svc_get_max_metadata_size(last_capi->capi_ptr,  &max_metadata_size);
      if (ADSP_FAILED(result)) return result;

      uint32_t reqOutSize;
      AudioDecSvc_GetRequiredOutBufSizeNum(pMe, &numOutBufs, &reqOutSize, outBufSize, max_metadata_size);
      pMe->out_streams_ptr[stream_index]->maxExtBufSize = reqOutSize;

      if(pIOInitParams->io_type == AUDIO_DEC_SVC_MIMO_OUTPUT_STREAM)
      {
    	  result |= AudioDecSvc_ReCreateCircBufs(pMe,pMe->out_streams_ptr[stream_index], reqOutSize, 0);
      }
   }
   else
   {
	   if (!AudioDecSvc_IsPcmFmt(last_capi->dec_fmt_id))
	   {
		   return ADSP_EUNEXPECTED;
	   }

	   uint32_t reqOutSize;
	   AudioDecSvc_GetInitPCMDecBufSize(pMe, pIOInitParams, &reqOutSize);

	   pMe->out_streams_ptr[stream_index]->maxExtBufSize = reqOutSize;
	   numOutBufs = DEFAULT_NUM_PCM_DEC_OUTPUT_BUFFERS;
   }

   result |= AudioDecSvc_CreateExternalBufs(pMe, pMe->out_streams_ptr[stream_index], last_capi, numOutBufs);
   result |= AudioDecSvc_CheckCreateScratchBufs(pMe, last_capi, pMe->out_streams_ptr[stream_index]->maxExtBufSize);

   if (ADSP_FAILED(result)) return result;

   //capi must've raised event by now since new port is opened.
   (void)AudioDecSvc_ProcessKppsBw(pMe, FALSE, FALSE);

   //initialize the service handle
   *handle = &(pMe->out_streams_ptr[stream_index]->common.serviceHandle);

   pMe->out_streams_ptr[stream_index]->common.serviceHandle.threadId = pMe->thread_id;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Dec svc created output stream_id 0x%x, at index %u", pMe->out_streams_ptr[stream_index]->common.stream_id, stream_index);

   return result;
}

