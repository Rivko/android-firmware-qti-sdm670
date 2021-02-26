/**
@file AudioStreamMgr_SessionCmdHandler.h
@brief This file declares session cmd handler functions for AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SessionCmdHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_SESSION_CMD_HANDLER_H
#define AUDIO_STREAM_MGR_SESSION_CMD_HANDLER_H

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
static const uint32_t CACHE_LINE_SIZE = 32; // Must be a power of 2
static const uint32_t CACHE_ALIGNMENT_MASK = (CACHE_LINE_SIZE - 1);

static inline bool_t isAlignedToCacheLine(uint32_t addr)
{
   return ((addr & CACHE_ALIGNMENT_MASK) == 0);
}
/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

/* Session cmd queue: processing functions and utility functions */

/*
   This function process the client command that are send to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/

ADSPResult AudioStreamMgr_SessionCmdQAprPktHandler( AudioStreamMgr_t *pMe, uint8_t intSessionId );

/*
   This function process unsupported messages in the session's command queue.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/

ADSPResult AudioStreamMgr_SessionCmdQUnSupportedHandler( AudioStreamMgr_t *pMe,uint8_t intSessionId );

/*
   This function processes client ASM_SESSION_CMD_ADJUST_SESSION_CLOCK command that is sent to
   a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that is associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that is to process the client command.
   @return Success or Failure
*/
ADSPResult  AudioStreamMgr_AdjustSessionClkCmdHandler(AudioStreamMgr_t *pMe,
                                                      uint8_t int_session_id);


ADSPResult  AudioStreamMgr_SessionPauseCmdHandler(AudioStreamMgr_t *pMe,
                                                    uint8_t int_session_id);

ADSPResult  AudioStreamMgr_SessionSuspendCmdHandler(AudioStreamMgr_t *pMe,
                                                    uint8_t int_session_id);
ADSPResult  AudioStreamMgr_SessionRunCmdHandler(AudioStreamMgr_t *pMe,
                                                uint8_t int_session_id);

/**
 * This is the command handler to ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS and
 *  ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS
 *
 *  @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
 *                    that associated with the given session might be turned on/off depending
 *                    on command processing.
 *  @param pSession[in/out]  This points to the session that are to process the message in
 *                          the response queue.
 *  @return Success or Failure
 */
ADSPResult AudioStreamMgr_RegisterMatrixFlowEventCmdHandler(AudioStreamMgr_t *pMe,
                                                  uint8_t int_session_id);
/*
   This function process the command to get path delay of a given session.
   Currently, supported only for loopback.
   path delay is the delay sufferred by signal from tx port to rx port through asm loopback.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult  AudioStreamMgr_GetPathDelay(AudioStreamMgr_t *pMe,
                        uint8_t int_session_id);

/*
   This function process the client command that are to configure Matrix/Stream Router.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.j
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_SetMtMxStRtrParamCmdHandler(AudioStreamMgr_t* pMe,
                                            uint8_t int_session_id);

/**
 * This is the command handler to ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2
 */
ADSPResult AudioStreamMgr_GetMtMxStRtrParamCmdHandler(AudioStreamMgr_t *pMe,
                                               uint8_t int_session_id);

ADSPResult AudioStreamMgr_SessionCmdClose(AudioStreamMgr_t *pMe,
                                               uint8_t int_session_id);
ADSPResult AudioStreamMgr_SessionCmdFlush(AudioStreamMgr_t *pMe,
                                               uint8_t int_session_id);

AudioStreamMgr_StreamStateType *ASM_GetStreamForMtmxStRtrSessionCmd(AudioStreamMgr_SessionStateType *pSession,
                  elite_apr_port_t  port);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_SESSION_CMD_HANDLER_H

