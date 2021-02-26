/**
@file AudioStreamMgr_StreamCmdHandler.h
@brief This file declares session cmd handler functions for AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_StreamCmdHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013  RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2014, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_STREAM_CMD_HANDLER_H
#define AUDIO_STREAM_MGR_STREAM_CMD_HANDLER_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "AudioStreamMgr_Type.h"
#include "AudioEncSvc.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

static inline uint32_t ASM_GetNextDataLogId(AudioStreamMgr_t* pMe, uint16_t port)
{
   uint16_t seq_id = pMe->unDataLogSeqNum++;
   return  (((uint32_t)port)<<20) | ( ((uint32_t) seq_id)<<8);
}

/*
   This function processes client OPEN_READWRITE command that is sent to a given session
   and a given stream.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenRxTxStreamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id,  uint8_t stream_id);
/*
   This function process the client OPEN_PLAYBACK command that are send to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenRxStreamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);
/*
   This function process the client OPEN_RECORD command that are send to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenTxStreamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client OPEN_LOOPBACK command that is sent to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenLoopBackCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

ADSPResult AudioStreamMgr_OpenTranscodeLoopBackCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client OPEN_WRITE_COMPRESSED command that is sent to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenWriteCompressedCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client OPEN_READ_COMPRESSED command that is sent to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenReadCompressedCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE command that is sent to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenPullModeRxStreamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client ASM_STREAM_CMD_OPEN_PUSH_MODE_READ command that is sent to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_OpenPushModeTxStreamCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);

/*
   This function process the client CLOSE command that are to CLOSE a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult  AudioStreamMgr_CloseStreamCmdHandler(AudioStreamMgr_t *pMe,
                                                uint8_t int_session_id, uint8_t stream_id);
ADSPResult  AudioStreamMgr_CloseStreamCmdHelper(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);
/*
   This function process the Client command that are to configure AudioProcSvc.
   Currently, this only supports Volume Control on playback path.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SetPpParamCmdHandler(AudioStreamMgr_t* pMe,
                                            uint8_t int_session_id, uint8_t stream_id);

/*
   This function processes client SET_ENDEC_PARAM command that is sent to a given session
   and a given stream.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SetEncDecParamCmdHandler(AudioStreamMgr_t* pMe,
                                            uint8_t int_session_id, uint8_t stream_id);

ADSPResult AudioStreamMgr_RegisterEncDecEvents(AudioStreamMgr_t* pMe,
                                            uint8_t int_session_id, uint8_t stream_id);

ADSPResult AudioStreamMgr_RegisterIEC61937MediaFmtEvents(AudioStreamMgr_t* pMe,
                                            uint8_t int_session_id, uint8_t stream_id);

ADSPResult  AudioStreamMgr_StreamFlushCmdHandler(AudioStreamMgr_t *pMe,
                                                 uint8_t int_session_id, uint8_t stream_id);

ADSPResult AudioStreamMgr_StreamFlushCmdHelper(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream, bool_t flush_bit_stream);
/*
   This function processes ASM_STREAM_CMD_FLUSH_READBUFS command that is sent to
   a given session/stream.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that is associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that is to process the client command.
   @return Success or Failure
*/
ADSPResult  AudioStreamMgr_FlushReadBufsCmdHandler(AudioStreamMgr_t *pMe,
                                                 uint8_t int_session_id, uint8_t stream_id);

/**
 * This is the command handler to ASM_STREAM_CMD_GET_PP_PARAMS_V2
 */
ADSPResult AudioStreamMgr_GetPpParamCmdHandler(AudioStreamMgr_t *pMe,
                                               uint8_t int_session_id, uint8_t stream_id);

/*
This command handler is to handle ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST
*/
ADSPResult AudioStreamMgr_GetTopoModuleCmdHandler(AudioStreamMgr_t *pMe,
                                                  uint8_t int_session_id, uint8_t stream_id);


ADSPResult AudioStreamMgr_SessionDtmfCmdHandler(AudioStreamMgr_t *pMe,
      uint8_t int_session_id, uint8_t stream_id);
ADSPResult AudioStreamMgr_RegisterDeRegisterPPEvents(AudioStreamMgr_t *pMe,
												  uint8_t int_session_id, uint8_t stream_id	);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_STREAM_CMD_HANDLER_H

