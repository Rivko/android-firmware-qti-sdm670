/**
@file AudioStreamMgr_StreamRespHandler.h
@brief This file declares session response handler functions for AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_StreamRespHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013 RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_STREAM_RESP_HANDLER_H
#define AUDIO_STREAM_MGR_STREAM_RESP_HANDLER_H

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

ADSPResult AudioStreamMgr_SessionRespQOnAdmConnect(AudioStreamMgr_t *pMe,
                                                          AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQOnAdmDisConnect(AudioStreamMgr_t *pMe,
                                                             AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQOnAdmDisable(AudioStreamMgr_t *pMe,
                                                          AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQOnGetSwitchSvcHandle(AudioStreamMgr_t *pMe,
                                                                  AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQOnSwitchVacatePort(AudioStreamMgr_t *pMe,
                                                                AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQOnSwitchFlush(AudioStreamMgr_t *pMe,
                                                           AudioStreamMgr_SessionStateType *pSession);
/*
   This function process ELITE_CUSTOM_MSG messages in the session's response queue.
   Typically, need to further look up the secondary opcode in the message payload
   buffer.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQCustomMsgHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

/*
   This function process ELITE_CMD_DESTROY_SERVICE messages in the session's response queue.
   This message is an Acknolowledge to ASM from a dynamic service that signals the dynamic service
   is ready to be destroyed.  ASM needs to qurt_thread_join on the dynamic service's thread to avoid
   memory leak. To avoid block in system, dynamic service SHOULD only send such an ack when it finish
   all blocking operations such as releasing buffer queue etc before they send this ack.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQDestroySvcHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

/*
   This function process ELITE_CMD_CONNECT messages in the session's response queue.
   This message is an Acknolowledge to ASM from a dynamic service that signals the dynamic service
   is connected to downstream svc.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQConnectHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

/*
   This function process ELITE_CMD_DISCONNECT messages in the session's response queue.
   This message is an Acknolowledge to ASM from a dynamic service that signals the dynamic service
   is disconnected from downstream svc.  After disconnection, dynamic service should not send
   any data to downstream.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQDisConnectHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

/*
   This function process ELITE_CMD_SET_PARAM messages in the session's response queue.
   This message is an Acknolowledge to ASM from a dynamic service that signals the dynamic service
   accepts the param in SET_PARAM message.  This is currently used to configure ProcSvc.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQSetParamHandler( AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );


ADSPResult AudioStreamMgr_SessionRespQOnFlushAck( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );
/* Get Param ACK handler */
ADSPResult AudioStreamMgr_SessionRespQGetParamHandler( AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQRegMtMxFlowEventHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQGetMtMxStRtrParam( AudioStreamMgr_t *pMe,
                                                      AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStream_Mgr_SessionRespQDecOpenStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStream_Mgr_SessionRespQDecCloseStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStream_Mgr_SessionRespQDecConnectStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_STREAM_RESP_HANDLER_H

