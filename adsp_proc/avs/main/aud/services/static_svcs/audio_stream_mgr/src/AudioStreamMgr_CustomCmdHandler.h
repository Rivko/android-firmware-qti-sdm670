/**
@file AudioStreamMgr_CustomCmdHandler.h
@brief This file declares session cmd handler functions for AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_CustomCmdHandler.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013 RB      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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
/*
   This function process the Custom command that are send to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/

ADSPResult AudioStreamMgr_SessionCmdQCustomMsgHandler( AudioStreamMgr_t *pMe, uint8_t intSessionId );

/*
   This function process the custom PP output sampling rate configuration command that are send
   to a given session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_ProcessSetPPOutputSamplingRate(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_STREAM_CMD_HANDLER_H

