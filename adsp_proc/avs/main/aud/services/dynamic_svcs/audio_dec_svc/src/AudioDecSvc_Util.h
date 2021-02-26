
/*========================================================================

This file contains declarations of Audio DecoderService utility functions.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_Util.h#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/12/10   Wen Jin   Created file.

========================================================================== */
#ifndef AUDIODECSVC_UTIL_H
#define AUDIODECSVC_UTIL_H

#include "EliteLoggingUtils_i.h"
#include "adsp_private_api.h"
#include "adsp_avs_common_api.h"

#ifndef MIN
#define MIN(m,n)  ((m < n) ? m : n)
#endif

#ifndef MAX
#define MAX(m,n)  ((m > n) ? m : n)
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Elite.h"
#include "qurt_elite.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"

#include "AudioDecSvc.h"
#include "AudioDecSvc_Structs.h"
#include "byte_conversion.h"

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
#define CHECK_ERR_BITS_PER_SAMPLE(bits_per_sample)                                              \
      if ((bits_per_sample != 16) && (bits_per_sample != 24) && (bits_per_sample != 32))                                   \
      {                                                                                         \
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid bits per sample %u",bits_per_sample);   \
         return ADSP_EBADPARAM;                                                                 \
      }

ADSPResult AudioDecSvc_CreateCAPIs(AudioDecSvc_t *pMe, AudioDecSvcInitParams_t *pInitParams, dec_init_time_get_params_t *dec_params);
void AudioDecSvc_GetInitPCMDecBufSize(AudioDecSvc_t* pMe, AudioDecSvcIOStreamInitParams_t *pInitIOParams, uint32_t *outBufSize);

ADSPResult AudioDecSvc_DetermineConverterOutputFormat(uint32_t media_fmt_id, uint32_t dec_conv_mode, uint32_t *output_media_fmt);
ADSPResult AudioDecSvc_CreateInternalInputBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container,
      uint32_t inBufSize, dec_capi_port_index_t in_capi_port_index);
ADSPResult AudioDecSvc_CreateInternalOutputBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container,
      uint32_t outBufSize, dec_capi_port_index_t out_capi_port_index);
ADSPResult AudioDecSvc_CheckCreateScratchBufs(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container, uint32_t outBufSize);
ADSPResult AudioDecSvc_CheckReCreateOneBufInCircBuf(AudioDecSvc_OutStream_t* pOutStrm, AudioDecSvc_Circular_Buffer_List_t *pCircBufList, int index, uint32_t BufSize);
ADSPResult AudioDecSvc_CheckReCreateAllBufsInCircBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm, uint32_t BufSize);
ADSPResult AudioDecSvc_ReCreateCircBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm, uint32_t BufSize, int32_t numBufsInAdditionToMimo);
ADSPResult AudioDecSvc_CreateExternalBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStream, dec_CAPI_container_t *capi_container, uint32_t numOutBufs);
void AudioDecSvc_DestroyInternalBuffers(AudioDecSvc_t* pMe);
void AudioDecSvc_DestroyExternalBufs(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t* pOutStrm);
void AudioDecSvc_DestroyCircularBufs(AudioDecSvc_t *pMe , AudioDecSvc_OutStream_t* pOutStrm);

ADSPResult AudioDecSvc_CheckReallocateExternalBuffer(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);

ADSPResult AudioDecSvc_UpdateMediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_CAPI_container_t *capi_cont, uint8_t *pFmtBlk, uint32_t ulFmtBlkSize);
ADSPResult AudioDecSvc_UpdateMediaFmt_PeerService(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm);

ADSPResult AudioDecSvc_GetInputDataCmd(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);
ADSPResult AudioDecSvc_FreeInputDataCmd(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, ADSPResult status);
ADSPResult AudioDecSvc_ResetDecoder(AudioDecSvc_t *pMe);
ADSPResult AudioDecSvc_FlushInputDataQ(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm);
void AudioDecSvc_setThreadStackSize( uint32_t *threadStackSize, uint32_t formatID);

ADSPResult AudioDecSvc_NotifyPeerSvcWithMediaFmtUpdate (AudioDecSvc_t* pMe,  AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, int32_t nSampleRateAfter,
                                                       int32_t nChannelsAfter, uint8_t * pucChannelMappingAfter,
                                                       uint16_t bits_per_sample);
ADSPResult AudioDecSvc_SendMediaFmtUpdate(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_any_t *msg);
ADSPResult AudioDecSvc_GetMediaFmtUpdateMsg(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm,
      int32_t nSampleRateAfter,
      int32_t nChannelsAfter,
      uint8_t *ucChannelMappingAfter,
      uint16_t output_bits_per_sample,
      data_format_t out_data_fmt,
      uint32_t fmt_id,
      elite_msg_any_t *msg
      );

ADSPResult AudioDecSvc_PopulateOutBuf(AudioDecSvc_t* pMe, AudioDecSvc_OutStream_t *pOutStrm);

ADSPResult AudioDecSvc_SendPcmToPeerSvc (AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, elite_msg_data_buffer_t *pOutBuf );
ADSPResult AudioDecSvc_InitTsState(DecTimeStampState_t *pTsState);
ADSPResult AudioDecSvc_FillTsStateFromInBuf(DecTimeStampState_t *pTsState,
      uint32_t dec_fmt,      uint64_t ts,      bool_t ts_valid,      bool_t ts_continue,      bool_t eof_flag, bool_t WasPrevDecResNeedMore);
ADSPResult AudioDecSvc_UpdateNextOutbufTS(uint32_t ulOutbufSize,
                                          elite_multi_channel_pcm_fmt_blk_t* pOutputFmt,
                                          DecTimeStampState_t* pTsState,
                                          uint32_t dec_fmt);

bool_t AudioDecSvc_IsNextBufTsDiscontinuous(DecTimeStampState_t* pTsState, uint32_t ulOutbufSize,
      elite_multi_channel_pcm_fmt_blk_t* pOutputFmt);

ADSPResult AudioDecSvc_UpdateInputTsByDataConsumed(DecTimeStampState_t* pTsState, uint32_t inputBytesConsumed, DecPcmFmt_t *pcmFmt);

ADSPResult AudioDecSvc_GetDecoderParams(AudioDecSvc_t *pMe,
                                        uint32_t *SampleRateAfter,
                                        DecChannelMap_t **channelCfgPtr);
ADSPResult AudioDecSvc_SendSrCmEvent(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm,
                                     uint32_t sample_rate,
                                     DecChannelMap_t *chan_map);
void AudioDecSvc_RemoveInitialSamples(uint32_t *samples_to_remove,
                                      uint32_t *valid_bytes_in_buffer,
                                      uint8_t * buffer_start,
                                      uint16_t num_channels,
                                      uint32_t sample_rate,
                                      uint64_t *next_buffer_ts,
                                      uint64_t *next_out_buf_sample_count,
                                      uint32_t bytes_per_sample);

void AudioDecSvc_RemoveTrailingSamples(uint32_t samples_to_remove,
                                       uint32_t *valid_bytes_in_buffer,
                                       uint8_t * buffer_start,
                                       uint16_t num_channels,
                                       uint32_t bytes_per_sample);

void AudioDecSvc_UpdatePrevFmtBlk(AudioDecSvc_t* pMe,AudioDecSvc_InpStream_t *pInpStrm,
                                  dec_CAPI_container_t *capi_cont,
                                  int32_t nSampleRateAfter,
                                  int32_t nChannelsAfter,
                                  uint8_t *ucChannelMappingAfter,
                                  uint16_t bits_per_sample,uint8_t capi_out_index);


ADSPResult AudioDecSvc_HandlePortDataThreshChangeEvent(AudioDecSvc_t *pMe);
ADSPResult AudioDecSvc_RaiseKCPSUpdateEvent(AudioDecSvc_t* pMe,uint32_t eventToken);
ADSPResult AudioDecSvc_RaiseBwUpdateEvent(AudioDecSvc_t* pMe, uint32_t pcm_bw, uint32_t enc_bw, uint32_t eventToken);

ADSPResult AudioDecSvc_CopyPcmOr61937MediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, void *pFmtBlk );
ADSPResult AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm);

#define AUDIOLOG_AUD_DEC_PCM_IN_TAP_ID      0x0001 /* Tap ID for logging PCM input      */
#define AUDIOLOG_AUD_DEC_BS_IN_TAP_ID       0x0002 /* Tap ID for logging bitstream input */
#define AUDIOLOG_AUD_DEC_PCM_OUT_TAP_ID     0x0003 /* Tap ID for logging PCM output */
#define AUDIOLOG_AUD_DEC_BS_OUT_TAP_ID      0x0004 /* Tap ID for logging packetized output */

ADSPResult AudioDecSvc_LogBitStreamData(int8_t *buf_addr, uint32_t buf_size,
                                        uint32_t session_id, uint32_t ulDecFormatId);
ADSPResult AudioDecSvc_LogPcmData(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, int8_t * buf_addr, uint32_t buf_size);
ADSPResult AudioDecSvc_LogPcmData_Out(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvc_OutStream_t *pOutStrm, int8_t * buf_addr, uint32_t buf_size);
ADSPResult AudioDecSvc_DiscardMarkBuffer(AudioDecSvc_t* pMe, AudioDecSvc_InpStream_t *pInpStrm);

uint16_t AudioDecSvc_CircBuf_GetNumEmptySlots(AudioDecSvc_Circular_Buffer_List_t *pCircBufList);
bool_t AudioDecSvc_CircBuf_IsDelayFull(AudioDecSvc_Circular_Buffer_List_t *pCircBufList);
ADSPResult AudioDecSvc_CircBuf_Flush(AudioDecSvc_Circular_Buffer_List_t *pCircBufList, AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);

/*Function to check if decoding has begun*/
static inline bool_t AudioDecSvc_HasDecodingBegun(AudioDecSvc_t *pMe)
{
   if (pMe->capiContainer[0]->PrevFmtBlk[0].num_channels == 0)
      return false;
   else
      return true;
}

ADSPResult AudioDecSvc_ProcessPopOutBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);

static inline void AudioDecSvc_ReturnOutBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   elite_msg_any_payload_t *pAnyBuf = (elite_msg_any_payload_t*)(pOutStrm->outDataBufferNode.pBuffer);
   if(NULL != pAnyBuf)
   {
	   if (pOutStrm->outDataBufferNode.pReturnQ)
	   {
		   (void) elite_msg_push_payload_to_returnq(pOutStrm->outDataBufferNode.pReturnQ, pAnyBuf);
		   //essentially returns to outputBufQ
	   }
	   else //return back to circ buf.
	   {
		   //nothing to do
	   }
	   pOutStrm->outDataBufferNode.pBuffer = NULL;
   }
}

ADSPResult AudioDecSvc_PushOutBuf(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint64_t *pPayload);

// Low latency decoder sessions will have higher priority than legacy sessions.
// Also when decoding is done in AFE, prio is high.
static inline int32_t AudioDecSvc_GetThreadPriority(AudioDecSvc_t *pMe, uint32_t perf_mode)
{
   int32_t nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_DEC_SVC_PRIO_ID);
   if ((ASM_LOW_LATENCY_STREAM_SESSION == perf_mode ) ||
       (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == perf_mode ) || pMe->is_rt_dec)
   {
      nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_LL_DEC_SVC_PRIO_ID);
   }
   return nPriority;
}

/**
 * outBufSize should be initialized
 */
static inline void AudioDecSvc_GetRequiredOutBufSizeNum(AudioDecSvc_t *pMe, uint32_t *numOutBufs, uint32_t *reqOutBufSize, uint32_t specifiedOutBufSize, uint32_t metadata_size)
{
   if (pMe->is_rt_dec)
   {
      *numOutBufs = DEFAULT_NUM_OUTPUT_BUFFERS_RT;
   }
   else
   {
      *numOutBufs = DEFAULT_NUM_OUTPUT_BUFFERS;
   }

   if (AudioDecSvc_IsPcmFmt(pMe->capiContainer[0]->dec_fmt_id))
   {
       *numOutBufs = DEFAULT_NUM_PCM_DEC_OUTPUT_BUFFERS;
   }

   *reqOutBufSize = specifiedOutBufSize;
   if ((metadata_size > 0))
   {
      *numOutBufs += NUM_ADDITIONAL_OUTPUT_BUFFERS_WITH_METADATA;
      if (*reqOutBufSize < (metadata_size + sizeof(elite_msg_data_set_param_t)))
      {
         *reqOutBufSize = (metadata_size + sizeof(elite_msg_data_set_param_t));
      }
   }
}

ADSPResult AudioDecSvc_ProcessKppsBw(AudioDecSvc_t *pMe, bool_t is_release, bool_t force_vote);
ADSPResult AudioDecSvc_GetRequiredThreadStackSize(AudioDecSvc_t *pMe, uint32_t *threadStackSize);
ADSPResult AudioDecSvc_AggregateKcpsRequired(AudioDecSvc_t *pMe, uint32_t *decoder_kcps);
ADSPResult AudioDecSvc_RegisterWithAdsppm(AudioDecSvc_t* pMe);
ADSPResult AudioDecSvc_DeregisterWithAdsppm(AudioDecSvc_t* pMe);

bool_t AudioDecSvc_IsLastCapi(AudioDecSvc_t *pMe, dec_CAPI_container_t *capi_container);
bool_t AudioDecSvc_IsLastCapiIndex(AudioDecSvc_t *pMe, uint32_t ind);
dec_CAPI_container_t *AudioDecSvc_GetFirstCapi(AudioDecSvc_t *pMe);
dec_CAPI_container_t *AudioDecSvc_GetLastCapi(AudioDecSvc_t *pMe);

void AudioDecSvc_DecErrorEvent_CheckRaise(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_err_event_t *dec_err_event);
void AudioDecSvc_DecErrorEvent_RecordError(dec_err_event_t *dec_err_event);
void AudioDecSvc_DecErrorEvent_Reset(dec_err_event_t *dec_err_event);
void AudioDecSvc_DecErrorEvent_Clear(dec_err_event_t *dec_err_event);
void AudioDecSvc_DecErrEvent_SetParam(dec_err_event_t *dec_err_event, uint8_t *pSetParamPayload);

void AudioDecSvc_EarlyEos_AddEntryToLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t delay_from_cur_buf);
void AudioDecSvc_EarlyEoS_ResetLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
uint32_t AudioDecSvc_EarlyEos_GetMinFromLastFrameDelaysArray(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
void AudioDecSvc_EarlyEoS_Reset(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
void AudioDecSvc_EarlyEoS_AddBuffersToCurrBitField(AudioDecSvc_t *pMe);
AudioDecSvc_OutStream_t *AudioDecSvc_EarlyEoS_GetStream(AudioDecSvc_t* pMe);
bool_t AudioDecSvc_EarlyEos_IsDelaySatisfied(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
ADSPResult AudioDecSvc_EarlyEoS_Destroy(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
ADSPResult AudioDecSvc_EarlyEoS_BufferUp(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint32_t delay_from_cur_buf);
ADSPResult AudioDecSvc_EarlyEoS_SendNotif(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
ADSPResult AudioDecSvc_EarlyEoS_Schedule(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
ADSPResult AudioDecSvc_EarlyEoS_Create(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, uint8_t *pSetParamPayload);
ADSPResult AudioDecSvc_EarlyEoS_ProcessTimerSignal(AudioDecSvc_t* pMe, uint32_t channelBitIndex);

static inline void AudioDecSvc_EarlyEoS_RaiseSignal(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   //if early-eos is enabled and an empty buf is queued, then trigger more decoding.
   if (pOutStrm->early_eos_ptr)
   {
      pMe->unCurrentBitfield |= AUD_DEC_EARLY_EOS_CIRC_BUF_FILL_SIG;
      qurt_elite_signal_send(&(pOutStrm->early_eos_ptr->fill_circ_buf_signal));
#ifdef EARLY_EOS_DEBUG
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS: AudioDecSvc_EarlyEoS_RaiseSignal");
#endif
   }
}

static inline void AudioDecSvc_EarlyEos_ClearSignal(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm)
{
   if (pOutStrm->early_eos_ptr)
   {
      pMe->unCurrentBitfield &= ~AUD_DEC_EARLY_EOS_CIRC_BUF_FILL_SIG;
      qurt_elite_signal_clear(&(pOutStrm->early_eos_ptr->fill_circ_buf_signal));
#ifdef EARLY_EOS_DEBUG
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Early-EoS: AudioDecSvc_EarlyEos_ClearSignal");
#endif
   }
}

ADSPResult AudioDecSvc_RaiseRenderedEoSv2(AudioDecSvc_t* pMe, elite_msg_any_t *cmdMsg);
ADSPResult AudioDecSvc_HandlePrebuffer(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm, AudioDecSvc_InpStream_t *pInpStrm);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDCMNUTIL_H
