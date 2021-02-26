/**
@file AudioStreamMgr_Session.h
@brief This file declares session-control-related functions for AudioStreamMgr.

*/
 
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Session.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_SESSION_H
#define AUDIO_STREAM_MGR_SESSION_H

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
extern const ASM_RunPauseFlushFTableEntry ASM_SuspendFTable[];

extern const ASM_RunPauseFlushFTableEntry ASM_FlushFTable[];

      
/* Function table for Pause sequence:
      Playback:  proc, matrix and then midi(if applicable)
      Record:    proc, matrix 
*/
extern const ASM_RunPauseFlushFTableEntry ASM_PauseFTable1[];

/* Function table for Pause sequence:
      Playback:  proc, matrix and then midi(if applicable)
      Record:    proc, matrix
*/
extern const ASM_RunPauseFlushFTableEntry ASM_PauseFTable2[];

/*
   Function table for Run Sequence:
      Playback:  midi (if applicable),  matrix AND proc
      Record:    matrix and proc
*/
extern const ASM_RunPauseFlushFTableEntry ASM_RunFTable[];

/*
   Function table for querying the delay for the Rx path:
      Playback:  midi (if applicable),  matrix AND proc
*/
extern const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableRx[];

/*
   Function table for querying the delay for the Tx and loopback:
      Loopback:  midi (if applicable),  matrix AND proc
      Record:    matrix and proc
*/
extern const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableTxLoopback[];

/*
   Function table for querying the delay for the Tx and loopback:
      NT:  midi (if applicable),  matrix AND proc
*/
extern const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableNT[];

/*
   Function table for querying the delay for unknown modes.
*/
extern const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableDummy[];

extern const ASM_RunPauseFlushFTableEntry *ASM_QueryDelayFTables[];
extern const uint32_t ASM_QueryDelayFTablesSize;

extern const ASM_CreateDynaSvcFTableEntry ASM_CreateDynaSvcFTable[];
extern const ASM_CloseDynaSvcFTableEntry ASM_CloseDynaSvcFTable[];
extern const ASM_ConnectDynaSvcFTableEntry ASM_ConnectDynaSvcFTable[];

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

static inline AudioStreamMgr_StaticSessionStateType *AudioStreamMgr_GetStaticSessionState(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   return &pMe->aStSessions[pSession->ucInternalSessionId];
}

/*  These functions are used to create/destroy session state machine*/

/* This function creates all sessions state machine here. 
   @param pMe [in/out]  This points to the instance of AudioStreamMgr in which session state
                        structures are to be created.

   @return Success or fail 
*/
ADSPResult AudioStreamMgr_CreateAllSessionStates(AudioStreamMgr_t *pMe) ;

/*
  This function creates the necessary session state
  structure given the internal session ID that it needs to
  create. Currently only supports up to 15 sessions.

   @param pMe [in/out] This points to the instance of AudioStreamMgr in which session state
                        machines are to be created.
   @param intSessionId [in]  This is the ID of the session state that are to be created.
   @return Success or fail         
*/
ADSPResult AudioStreamMgr_CreateStaticSessionState(AudioStreamMgr_t *pMe, uint8_t intSessionId);

ADSPResult AudioStreamMgr_CreateSessionState(AudioStreamMgr_SessionStateType *pSession, uint8_t intSessionId);

void AudioStreamMgr_InitializeSessionState(AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStreamMgr_DestroyAllSessionState(AudioStreamMgr_t *pMe) ;
/*
  This function destroy the necessary session state
  structure given the internal session ID that it needs to
  destroy. Currently only supports up to 15 sessions.

   @param pMe [in/out] This points to the instance of AudioStreamMgr in which session state
                        machines are to be destroyed.
   @param intSessionId [in]  This is the ID of the session state that are to be destroyed.
   @return Success or fail         
*/
ADSPResult AudioStreamMgr_DestroyStaticSessionState(AudioStreamMgr_t *pMe, uint8_t intSessionId);
      

/*
  This function compose callback data for dynamic service. 

   @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param ucModuleId [in]   This specifies the module id of dyanmic service. 
   @param pCbData [out]   This is to be filled with callback data.
   @param unDir [in]    Indicates either Tx/Rx direction 
   @return Success or Failure              
             
*/                                              
ADSPResult AudioStreamMgr_FillCbDatForDynSvc( AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
                                              uint32_t client_token,
                                                  AudioStreamMgr_CallBackHandleType *pCbData); 
                                                     
ADSPResult AudioStreamMgr_SessionSuspendCodecSvc(  AudioStreamMgr_t *pMe,
                                                       AudioStreamMgr_SessionStateType *pSession,
                                                       AudioStreamMgr_StreamStateType *pStream,
                                                       uint32_t unSecOpcode,
                                                       uint8_t unDir);
ADSPResult AudioStreamMgr_SessionSuspendProcSvc(  AudioStreamMgr_t *pMe,
                                                       AudioStreamMgr_SessionStateType *pSession,
                                                       AudioStreamMgr_StreamStateType *pStream,
                                                       uint32_t unOpcode,
                                                       uint8_t unDir);
ADSPResult AudioStreamMgr_SessionRunPauseProcSvc(  AudioStreamMgr_t *pMe,
                                                      AudioStreamMgr_SessionStateType *pSession,
                                                      AudioStreamMgr_StreamStateType *pStream,
                                                      uint32_t unOpcode,
                                                      uint8_t unDir);
ADSPResult AudioStreamMgr_SessionRunPauseCodecSvc(  AudioStreamMgr_t *pMe,
                                                       AudioStreamMgr_SessionStateType *pSession,
                                                       AudioStreamMgr_StreamStateType *pStream,
                                                       uint32_t unOpcode,
                                                       uint8_t unDir);

static inline bool_t   ASM_IsGaplessSession(AudioStreamMgr_SessionStateType *pSession)
{
   return (pSession->pFirstGaplessStrm != NULL);
}

ADSPResult AudioStreamMgr_AllocateSession(AudioStreamMgr_t *pMe, uint8_t intSessionId);
ADSPResult AudioStreamMgr_DeallocateSession(AudioStreamMgr_t *pMe, uint8_t intSessionId);
ADSPResult AudioStreamMgr_DeallocateSessionIfLastStream(AudioStreamMgr_t *pMe, uint8_t intSessionId);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_SESSION_H

