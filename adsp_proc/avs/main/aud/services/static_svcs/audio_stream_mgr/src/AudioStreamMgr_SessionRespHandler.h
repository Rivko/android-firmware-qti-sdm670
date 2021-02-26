/**
@file AudioStreamMgr_SessionRespHandler.h
@brief This file declares session response handler functions for AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SessionRespHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_SESSION_RESP_HANDLER_H
#define AUDIO_STREAM_MGR_SESSION_RESP_HANDLER_H

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

/* Session RespQ: processing functions and utility functions */
/*
   This function process unsupported messages in the session's response queue.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQUnSupportedHandler(AudioStreamMgr_t *pMe,
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

ADSPResult AudioStreamMgr_SessionRespQCustomEventHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );

/*
   This function processes ELITEMSG_CUSTOM_MT_MX_ADJUST_SESSION_CLK messages in the
   session's response queue.
   This message is an ack to ASM from matrix mixer that would say whether session
   clk adjustment would be successful and its relevant info if so.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the message in
                           the response queue.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SessionRespQAdjustSessionClkHandler( AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQOnRunAck( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStreamMgr_SessionRespQOnPause1Ack( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStreamMgr_SessionRespQOnPause2Ack( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStreamMgr_SessionRespQOnSuspendAck(AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession);
ADSPResult AudioStreamMgr_SessionRespQGenericAck( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQGenericAckForInternalMsg( AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession );
ADSPResult AudioStreamMgr_SessionRespQRegMtMxFlowEventHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQGetDevicePathDelay(AudioStreamMgr_t *pMe,
                                                          AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStreamMgr_SessionRespQGetPPNumOutputBuffers(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStreamMgr_SessionRespQGetPpParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQSetPpParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );

/** Events **/

ADSPResult AudioStreamMgr_SessionRespQGetPPTopologyModulesHandler(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStreamMgr_SessionRespQListenToEoSAck(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession );

ADSPResult AudioStreamMgr_SessionRespQEosV2Handler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_SESSION_RESP_HANDLER_H

