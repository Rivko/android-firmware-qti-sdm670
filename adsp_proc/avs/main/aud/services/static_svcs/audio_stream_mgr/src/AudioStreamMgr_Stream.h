/**
@file AudioStreamMgr_Stream.h
@brief This file declares session-control-related functions for AudioStreamMgr.

*/
 
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Stream.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/22/2013  rb      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_STREAM_H
#define AUDIO_STREAM_MGR_STREAM_H

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

ADSPResult AudioStreamMgr_CreateStreamState(AudioStreamMgr_SessionStateType *pSession, uint8_t index);

/**
 * return of max_size is an error
 */
uint16_t ASM_FindNextValidNodeIndex(const ASM_GraphNodeIndex *NodeIndexList, uint16_t index, uint16_t max_size);
ADSPResult AudioStreamMgr_CreateAllStreamStates(AudioStreamMgr_SessionStateType *pSession);

ADSPResult AudioStreamMgr_CreateAllStreamSvcs(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
         AudioStreamMgr_StreamStateType *pStream, ASM_OpenStreamParams_t *pOpenStream);
ADSPResult AudioStreamMgr_InitializeStreamState(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id);
/*
   This utility function is used to connect the graph once all services on the graph
   is created.  

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session in which the graph needs to be connected. 
   @return Success or Failure                        
*/
ADSPResult AudioStreamMgr_StreamConnectGraph(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession,
                                              AudioStreamMgr_StreamStateType *pStream);

ADSPResult AudioStreamMgr_StreamDisConnectDecFromSwitchSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);
/*
   This utility function is used to discconnect the graph.  Disconnect message is
   sent simultaneously to all dynamic servics in this session. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session in which the graph needs to be connected. 
   @return Success or Failure                        
*/
ADSPResult AudioStreamMgr_StreamDisConnectGraph(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession,
                                              AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_CloseStreamAfterDisconnect(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream);

/*
   This utility function is used to destory all services on the graph. 
   It should be called only after the services are disconnected to avoid
   race conditions. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session in which the graph needs to be destroyed. 
   @return Success or Failure                        
*/
ADSPResult  AudioStreamMgr_StreamDestroy(AudioStreamMgr_t *pMe,
                                          AudioStreamMgr_SessionStateType *pSession,
                                          AudioStreamMgr_StreamStateType *pStream);

/*
   The following two utility functions is used to ACK back open/close command. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. 
   @param pSession[in/out]  This points to the session. 
   @return Success or Failure                        
*/
ADSPResult AudioStreamMgr_StreamAckCloseStream(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession,
                                              AudioStreamMgr_StreamStateType *pStream);
ADSPResult AudioStreamMgr_StreamAckOpenStream(AudioStreamMgr_t *pMe,
                                              AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream);


                                                     
                                                     
/*
    This function sends destroy message to a dynamic service and wait for its ACK.
    This function is a blocking call and shall ONLY be used with extreme caution. 

    @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
    @param pSession [in]  This points to the session .
    @param pSvcNode [in/out]   Node pointer of the service being destroyed
    @param stream_id [in]      Stream ID
    @return Success or Failure   
    
*/
ADSPResult  AudioStreamMgr_StreamDestroyMod(AudioStreamMgr_t *pMe,
               AudioStreamMgr_SessionStateType *pSession,
               AudioStreamMgr_StreamStateType *pStream,
               ASM_GraphNodeIndex nodeIndex);
                                              
/*
    This is a generic utility function to ack failure to client on 
    the IPC command on this session. Since this is the utility function,
    it DOES not change signal mask. 

    @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
    @param pSession [in]  This points to the session .
    @param nRes [in]   This specifies the failure code. 
    @return Success or Failure   
    
*/
ADSPResult AudioStreamMgr_SessionAckFail( AudioStreamMgr_t *pMe,
                                          AudioStreamMgr_SessionStateType *pSession,
                                          ADSPResult nRes); 

/*
   This function can be used by ASM in case of panic to destroy the graph.
   It does not follow the usual close sequency by disconnecting first and then destroy
   since the error might happen in the process of connection and hence can not disconnect
   at all. 
   It simply destroy all services in the graph in sequential order. 
   This function can cause hang if the module in the graph do not respond to 
   destroy_yourself message.
   @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param stream_id [in]   Stream Id of current stream. 
   @return Success or Failure   
*/
ADSPResult  AudioStreamMgr_StreamPanicHandler(AudioStreamMgr_t *pMe,
                                          AudioStreamMgr_SessionStateType *pSession,
                                          AudioStreamMgr_StreamStateType *pStream);

/**
 * This function returns the delay caused by the decoder service.
 *
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForDecoderSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

/**
 * This function returns the delay caused by the PP service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamGetPathDelayForPPService (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

/**
 * This function returns the algorithmic delay caused by the PP service. The buffering delay is zero.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamGetPathDelayForPPServiceAlgOnly (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

/**
 * This function returns the delay caused by the switch service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForSwitchSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

/**
 * This function returns the delay caused by the encoder service.
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayForEncoderSvc (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

/**
 * This function adds zero delay to the stream path. It can be used as a dummy function for delays
 * @param pMe[in] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in]  This points to the session .
   @param opcode [in]   opcode to get the delay
   @param unDir [in] matrix direction (not always applicable)
   @return Success or Failure
 */
ADSPResult AudioStreamMgr_StreamUpdatePathDelayZero (  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,      AudioStreamMgr_StreamStateType *pStream, uint32_t unOpcode,      uint8_t unDir);

ADSPResult AudioStreamMgr_StreamFlushCodecSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir);

ADSPResult AudioStreamMgr_StreamFlushProcSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir);

/*This function flushes the switch input port associated with the given stream*/
ADSPResult AudioStreamMgr_StreamFlushSwitchSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_STREAM_H

