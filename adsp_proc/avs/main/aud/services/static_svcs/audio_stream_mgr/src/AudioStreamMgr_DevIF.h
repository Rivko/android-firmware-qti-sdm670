/**
@file AudioStreamMgr_DevIF.h
@brief This file declares functions that are used AudioStreamMgr to 
communicate with Device Manager. 

*/
 
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_DevIF.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_DEVIF_H
#define AUDIO_STREAM_MGR_DEVIF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "AudioStreamMgr_Session.h"

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
  This function request a Rx matrix handle to send/receive
  audio data to/from for a session. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that is requesting handle.
   @param stream_id[in]  This is the stream ID of stream attempting to obtain device handle
   @param is_compressed[in]  Indicates if the input to the device is compressed audio
   @return Success or Failure              
             
*/
int AudioStreamMgr_SessionRequestRxDeviceHandle( AudioStreamMgr_t* pMe,
                                    AudioStreamMgr_SessionStateType *pSession,
                                    AudioStreamMgr_StreamStateType *pStream);

/*
  This function request a Tx matrix handle to send/receive
  audio data to/from for a session.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that is requesting handle.
   @param stream_id[in]  This is the stream ID of stream attempting to obtain device handle
   @param timestamp_type[in] timestamp type to be used in matrix.
   @param is_compressed[in]  Indicates if the output of the device is compressed audio
   @return Success or Failure

*/
int AudioStreamMgr_SessionRequestTxDeviceHandle( AudioStreamMgr_t* pMe,
                                    AudioStreamMgr_SessionStateType *pSession,
                                    AudioStreamMgr_StreamStateType *pStream,
                                    uint8_t timestamp_type);
/*
  This function return a matrix handle to send/receive
  audio data to/from for a session. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in/out]  This points to the session that is returning session.
   @param nTxDisconnectStep [in]  This indicates disconnect/disable in Tx direction
   @param stream_id [in]  This indicates stream_id in of the stream being disonnected
   @param direction [in]  This indicates Rx/Tx matrix
   @param is_compressed [in] Indicates if the given session is compressed
   @return Success or Failure  
             
*/
int AudioStreamMgr_SessionReturnDeviceHandle( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      bool_t TxDisconnectStep,
      uint8_t direction);
/*
  This function sends run/pause/message to Matrix svc based on opcode. 

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in/out]  This points to the session that is returning session.
   @param unSecOpcode [in]    Support ELITEMSG_CUSTOM_PAUSE/ELITEMSG_CUSTOM_RUN/ELITEMSG_CUSTOM_FLUSH. 
   @param ucCmdSeqIdx [in]   This will be put in LSB 8 bits of the client token. 
   @return Success or Failure              
             
*/
ADSPResult AudioStreamMgr_SessionRunPauseMatrixSvc(  AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    uint32_t  unSecOpcode,
                                                    uint8_t   unDir);

ADSPResult AudioStreamMgr_StreamFlushMatrixSvc(  AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    uint32_t  unSecOpcode,
                                                    uint8_t   unDir);

ADSPResult AudioStreamMgr_SessionSuspendMatrixSvc(  AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    uint32_t  unSecOpcode,
                                                    uint8_t   unDir);
/*
  This function sends run, pause, flush command to stream router svc.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession [in/out]  This points to the session .
   @param unOpcode [in]   Supports ELITE_CMD_PAUSE, ELITE_CMD_RUN, ELITE_CMD_FLUSH
   @param ucCmdSeqIdx [in]   This will be put in LSB 8 bits of the client token.
   @param unDir [in]    Indicates either Tx/Rx direction
   @return Success or Failure

*/
ADSPResult AudioStreamMgr_SessionRunPauseStreamRouterSvc(  AudioStreamMgr_t *pMe,
                                                                AudioStreamMgr_SessionStateType *pSession,
                                                                AudioStreamMgr_StreamStateType *pStream,
                                                                uint32_t unOpcode,
                                                                uint8_t unDir);

ADSPResult AudioStreamMgr_StreamFlushStreamRouterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir);
ADSPResult AudioStreamMgr_SuspendStreamRouterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir);
/*
   This function processes the custom connect command to connect the ASM service with a downstream
   device handle. This is used by ULL, LLNP, and ULLPP modes for connections that directly connect
   to the COPP or AFE bypassing the matrix mixer.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_ProcessStreamDirectConnect(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession);

/*
   This function processes the custom disconnect command to connect the ASM service with a downstream
   device handle. This is used by ULL, LLNP, and ULLPP modes for connections that directly connect
   to the COPP or AFE bypassing the matrix mixer.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_ProcessStreamDirectDisconnect(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession);

/*
   This function processes the custom query for encoder handle command. This should only be used by
   LLNP Tx sessions to provide the AFE with the downstream (encoder) service handle.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_ProcessQueryTxSinkHandle(AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession);

/*
   This function processes the custom msg to prepare the encoder for disconnection encoder.
   This should only be used by LLNP Tx sessions to tell the encoder to release its buffers prior
   to disconnection from the upstream service.

   @param pMe[in/out] This points to the instance of AudioStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
*/
ADSPResult AudioStreamMgr_ProcessPrepareForDisconnect(AudioStreamMgr_t *pMe,
                                                      AudioStreamMgr_SessionStateType *pSession);
                                                    
/**
 * This function register tx-overflow or rx-underflow event with matrix 
 * 
 * @param pMe [in] This points to the instance of AudioStreamMgr.
 * @param pSession [in] This points to the session that is under processing
 * @param pEventCfg [in] This points to configuration data. 
 * @param direction [in] Indicates Tx or Rx.
 * 
 * @return ADSPResult success or failure
 */
ADSPResult AudioStreamMgr_SessionRegisterMatrixFlowEvent(  AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    asm_session_cmd_reg_tx_overflow_t *pEventCfg,
                                                    uint32_t direction);


/**
 * This function is used to diable/disconnect TX matrix and wait 
 * for ACK 
 * 
 * @param pMe [in] This points to the instance of AudioStreamMgr.
 * @param pSession [in] This points to the session that is under processing
 * @param disconnect_matrix [in] This is used to distinguish 
 *                          between the disable and disconnect
 *                          steps
 * @param stream_id [in] Stream ID of the current stream 
 * 
 * @return ADSPResult success or failure
 */
ADSPResult AudioStreamMgr_DisconnectTxMatrixAndWaitForAck( AudioStreamMgr_t *pMe,
                                          AudioStreamMgr_SessionStateType *pSession,
                                          bool_t disconnect_matrix,
                                          uint8_t stream_id);
                                                    
/**
 * This function is used to query the device path delays from Stream Router
 *
 * @param pMe [in] This points to the instance of AudioStreamMgr.
 * @param pSession [in] This points to the session that is under processing
 * @param unOpCode [in] opcode of the command to be sent
 * #param direction[in] direction of the stream router
 *
 * @return ADSPResult success or failure
 */
ADSPResult AudioStreamMgr_StreamGetDevicePathDelayCompressed(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,    AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t direction);
/**
 * This function is used to query the device path delays from Matrix
 *
 * @param pMe [in] This points to the instance of AudioStreamMgr.
 * @param pSession [in] This points to the session that is under processing
 * @param unOpCode [in] opcode of the command to be sent
 * @param direction[in] direction of the Matrix
 *
 * @return ADSPResult success or failure
 */
ADSPResult AudioStreamMgr_StreamGetDevicePathDelayUncompressed(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,     AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,      uint8_t direction);

ADSPResult AudioStreamMgr_StreamFlushAFE(  AudioStreamMgr_t *pMe,
                                                    AudioStreamMgr_SessionStateType *pSession,
                                                    AudioStreamMgr_StreamStateType *pStream,
                                                    uint32_t  unSecOpcode,
                                                    uint8_t   unDir);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_DEVIF_H

