/**
@file AudioStreamMgr_Util.h
@brief This file declares common utility functions used by AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Util.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
11/10/2010 RP      added more utility functions
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_UTIL_H
#define AUDIO_STREAM_MGR_UTIL_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "AudioStreamMgr_Type.h"
#include "AudDynaPPSvc.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr_Session.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
/**
 * PP output buffers are 10 frame equivalents of the AFE Frame for ASM_LEGACY_STREAM_SESSION
 */
#define PP_BUFFER_NUM_FRAME_LEGACY_MODE           (10)

/**
 * PP output buffers are 1 frame equivalent of the AFE Frame for
 * ASM_LOW_LATENCY_STREAM_SESSION 
 */
#define PP_BUFFER_NUM_FRAME_LOWLATENCY_MODE           (1)

/**
 * Default sampling rate while opening PP, 48000 Hz.
 */
#define ASM_DEFAULT_PP_SAMPLE_RATE 48000

/** \brief This function maps internal session ID to IPC level session ID.
Based on ISOD, the client specifie session from 1 to 15.
Internally, ASM mapps them to 0 to 14.
*/
inline static uint8_t MAP_INTERNAL_SESSION_ID_TO_EXTERNAL(uint8_t ucIntSessionID) { return (ucIntSessionID + 1);  }

inline static uint8_t MAP_EXTERNAL_SESSION_ID_TO_INTERNAL(uint8_t ucExtSessionID) { return (ucExtSessionID - 1);  }

inline static elite_apr_port_t PORT_FROM_SESSION_STREAM(AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream)
{
   return ASM_GET_PORT_ID(MAP_INTERNAL_SESSION_ID_TO_EXTERNAL(pSession->ucInternalSessionId), pStream->ucStreamId);
}
/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/* Utility fuction to log the header of msg. */
static inline void AudioStreamMgr_LogMsgHigh(elite_msg_any_t *pMsg)
{
   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t*) pMsg->pPayload;
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr: Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
         pMsg->unOpCode, (uint32_t) (pPayload),
         (uint32_t) (pPayload->pBufferReturnQ),(uint32_t) (pPayload->pResponseQ),
         pPayload->unClientToken,pPayload->unResponseResult);
}


/* Utility fuction to log the header of msg when error happens. */
static inline void AudioStreamMgr_LogErrMsg(elite_msg_any_t *pMsg)
{
   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t*) pMsg->pPayload;
   MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr: Fail Msg=[0x%lx,0x%lx],bq0x%lx,rq0x%lx,0x%lx,0x%lx",
         pMsg->unOpCode, (uint32_t) (pPayload),
         (uint32_t) (pPayload->pBufferReturnQ), (uint32_t) (pPayload->pResponseQ),
         pPayload->unClientToken,pPayload->unResponseResult);
}

static inline ADSPResult ASM_PushCmdToPeerSvc(AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      qurt_elite_queue_t *queue, uint64_t *msg)
{
   ADSPResult result = qurt_elite_queue_push_back(queue, msg);

   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr: queue push error");
      return result;
   }

   pSession->unReqResps++;
   /*Listen to respQ, switch off client command Q*/
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   pMe->unCurrentBitfield &= ~(pStSession->unSessionCmdQMask);

   return ADSP_EOK;
}

static inline void ASM_OnPopFromRespQ(AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   pSession->unReqResps--;

   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t*)pSession->msg.pPayload;

   pSession->nOverallResult |= pPayload->unResponseResult;

   if (0 == pSession->unReqResps)
   {
      /*Listen to respQ, switch on client command Q*/
      AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
      pMe->unCurrentBitfield |= (pStSession->unSessionCmdQMask);
   }
}

typedef struct
{
    uint32_t ulSampleRate;
    uint32_t ulNumChannels;
    uint8_t  channelMap[8];
} PPOutputParams_t;

ADSPResult AudioStreamMgr_GenerateProcInitParam( PPOutputParams_t *pParam,
                                                 uint32_t unSamplingRate, /* 0 for keeping native sampling rate*/
                                                  uint16_t usNumChannels   /* 0 for keeping native channel number */
                                                 );
/*
   Get PP Init Param from EncConfig
*/
ADSPResult AudioStreamMgr_GetProcInitParamFromEncConfig(
                                                PPOutputParams_t *pParam,
                                                uint32_t unFmtBlkId,
                                                void     *pFrmtBlk
                                                );
												
/*
   Get PP Init Param from nonpcm EncConfig
*/
ADSPResult AudioStreamMgr_GetProcInitParamFromNonPcmEncConfig(
                                                PPOutputParams_t *pParam,
                                                uint32_t unFmtBlkId,
                                                void     *pFrmtBlk
                                                );

/*
   Get Mtmx Re-Config Param from nonpcm EncConfig
*/
ADSPResult AudioStreamMgr_GetMtxConfigParamFromNonPcmEncConfig(
                                                void     *pParam,
                                                uint32_t unFmtBlkId,
                                                void     *pFrmtBlk
                                                );

ADSPResult AudioStreamMgr_RegisterQs(AudioStreamMgr_t *pMe,
                                       AudioStreamMgr_SessionStateType *pSession,
                                       AudioStreamMgr_StreamStateType *pStream);
/*
   Function that deregisters queues
*/
ADSPResult AudioStreamMgr_DeregisterQs(AudioStreamMgr_t *pMe,
                                       AudioStreamMgr_SessionStateType *pSession,
                                       AudioStreamMgr_StreamStateType *pStream);


/*
   Function that processes encoder bitrate
*/
ADSPResult AudioStreamMgr_ProcessEncoderBitRate(AudioStreamMgr_SessionStateType *pSession,
                                                uint8_t stream_id,
                                                asm_stream_cmd_set_encdec_param_t *pPayload);

/*
   Function that processes encoder config block
*/
ADSPResult AudioStreamMgr_ProcessEncoderConfigBlock(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    asm_stream_cmd_set_encdec_param_t *pPayload);

ADSPResult AudioStreamMgr_ProcessPcmEncCfgForPpMtx(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    void *pEncCfgBlk);
/**
 * sends the SET_ENCDEC param command to the enc or dec.
*/
ADSPResult AudioStreamMgr_SendEncDecParam(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
                           AudioStreamMgr_StreamStateType *pStream,
                           ASM_GraphNodeIndex nodeIndex,
                           asm_stream_cmd_set_encdec_param_t *pPayload);

void AudioStreamMgr_GetASMNodeForEncDecParam(uint32_t paramId, ASM_GraphNodeIndex *pNodeIndex);

ADSPResult ASM_ExecSuspendFTableEntry(ASM_GraphNodeIndex node_index,
                                           AudioStreamMgr_t* pMe,
                                           AudioStreamMgr_SessionStateType *pSession,
                                           AudioStreamMgr_StreamStateType *pStream);
ADSPResult ASM_ExecFlushFTableEntry(ASM_GraphNodeIndex node_index,
                                           AudioStreamMgr_t* pMe,
                                           AudioStreamMgr_SessionStateType *pSession,
                                           AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_ExecPause1FTableEntry(ASM_GraphNodeIndex node_index,
                                           AudioStreamMgr_t* pMe,
                                           AudioStreamMgr_SessionStateType *pSession,
                                           AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_ExecPause2FTableEntry(ASM_GraphNodeIndex node_index,
                                           AudioStreamMgr_t* pMe,
                                           AudioStreamMgr_SessionStateType *pSession,
                                           AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_ExecRunFTableEntry(ASM_GraphNodeIndex node_index,
                                         AudioStreamMgr_t* pMe,
                                         AudioStreamMgr_SessionStateType *pSession,
                                         AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_ExecQueryDelayFTableEntry(ASM_GraphNodeIndex node_index,
      AudioStreamMgr_t* pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_MemMapHandler(uint32_t nMemMapClient, uint16_t memPoolId,
                  uint16_t propertyFlag, bool_t isCached, uint16_t numRegions,
                  avs_shared_map_region_payload_t *pReg, uint32_t *memMapHandle);

ADSPResult ASM_MemUnmapHandler(uint32_t nMemMapClient, uint32_t memMapHandle);

static inline bool_t ASM_IsAsmLoopback(asm_session_mode_t mode)
{
   return ((ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE == mode) ||
         (ASM_LOOPBACK_PCM_IN_COMPRESSED_OUT_MODE == mode) ||
         (ASM_LOOPBACK_COMPRESSED_IN_PCM_OUT_MODE == mode) ||
         (ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE == mode) ||
         (ASM_LOOPBACK_COMPRESSED_IN_MULTI_OUT_MODE == mode));
}

static inline bool_t ASM_IsPcmFormat(uint32_t dec_fmt_id)
{
   return ( (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 == dec_fmt_id) ||
         (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 == dec_fmt_id) ||
         (ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 == dec_fmt_id) );
}

void AudioStreamMgr_AggregateBufDelay(AudioStreamMgr_StreamStateType *pStream, uint8_t unDir, uint32_t delay);

ADSPResult AudioStreamMgr_FinalizeGetPathDelayV2(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession, ADSPResult errorCode);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_UTIL_H

