/**
@file AudioStreamMgr_StreamUtil.h
@brief This file declares common utility functions used by AudioStreamMgr.

*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_StreamUtil.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/2010 DC      Created file.
11/10/2010 RP      added more utility functions
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_STREAM_UTIL_H
#define AUDIO_STREAM_MGR_STREAM_UTIL_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "AudioStreamMgr_Type.h"
#include "AudDynaPPSvc.h"
#include "adsp_mtmx_strtr_api.h"

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
ADSPResult AudioStreamMgr_OpenSecondGaplessStream(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      elite_apr_packet_t *pPkt,
      ASM_OpenStreamParams_t *pOpenStream);

ADSPResult AudioStreamMgr_VacateSwitchSvcPort(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);

/*This function closes one of the gapless streams when two are opened in the
same session*/
ADSPResult AudioStreamMgr_CloseSingleGaplessStream(AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession,
                                             AudioStreamMgr_StreamStateType *pStream);
/*
   Function that creates decoder service and launches the thread
*/
ADSPResult AudioStreamMgr_CreateDecSvc(AudioStreamMgr_t *pMe,
                                       AudioStreamMgr_SessionStateType *pSession,
                                       AudioStreamMgr_StreamStateType *pStream,
                                       ASM_OpenStreamParams_t *pOpenStream);

/*
   Function that creates encoder service and launches the thread
*/
ADSPResult AudioStreamMgr_CreateEncSvc(AudioStreamMgr_t *pMe,
                                       AudioStreamMgr_SessionStateType *pSession,
                                       AudioStreamMgr_StreamStateType *pStream,
                                       ASM_OpenStreamParams_t *pOpenStream);
/*
   Function that creates Post/Pre proc service and launches the thread
*/
ADSPResult AudioStreamMgr_CreatePPSvc(AudioStreamMgr_t *pMe,
                                            AudioStreamMgr_SessionStateType *pSession,
                                            AudioStreamMgr_StreamStateType *pStream,
                                            ASM_OpenStreamParams_t *pOpenStream);

/*
   Function that creates the switch service and launches the thread
*/
ADSPResult AudioStreamMgr_CreateSwitchSvc(  AudioStreamMgr_t *pMe,
                  AudioStreamMgr_SessionStateType *pSession,
                  AudioStreamMgr_StreamStateType *pStream,
                  ASM_OpenStreamParams_t *pOpenStream);

ADSPResult AudioStreamMgr_CreateCombinerSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream);

ADSPResult AudioStreamMgr_CreateSplitterSvc(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_OpenStreamParams_t *pOpenStream);

ADSPResult AudioStreamMgr_CloseStreamDecSvc (AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream);

ADSPResult AudioStreamMgr_ConnectStreamDecSvc(AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex upstream_node_ind,
                                                   ASM_GraphNodeIndex downstream_node_ind,
                                                   bool_t is_custom);

ADSPResult AudioStreamMgr_ConnectStreamGeneric(AudioStreamMgr_t *pMe,
                                                   AudioStreamMgr_SessionStateType *pSession,
                                                   AudioStreamMgr_StreamStateType *pStream,
                                                   ASM_GraphNodeIndex upstream_node_ind,
                                                   ASM_GraphNodeIndex downstream_node_ind,
                                                   bool_t is_custom);
/*
   Function that closes a regular stream
*/
ADSPResult AudioStreamMgr_CloseRegularStream(AudioStreamMgr_t *pMe,
                                             AudioStreamMgr_SessionStateType *pSession,
                                             AudioStreamMgr_StreamStateType *pStream);

/** This function destroys the stream services (does not
 *  release matrix ports) */
ADSPResult AudioStreamMgr_DestroyStreamSvc(AudioStreamMgr_t *pMe,
                                           AudioStreamMgr_SessionStateType *pSession,
                                           AudioStreamMgr_StreamStateType *pStream);

/** Gets the current stream ID from the client message being
 *  processed */
uint8_t AudioStreamMgr_GetStrmId(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession);

ASM_GraphNode* ASM_GetNextNodeInTopo(ASM_GraphNode *node_array, const ASM_SvcList *topo, ASM_GraphNode *curr_node);
ASM_GraphNodeIndex ASM_GetNextNodeIndexInTopo(const ASM_SvcList *topo, ASM_GraphNodeIndex curr_node);
ASM_GraphNodeIndex ASM_GetNodeNextToTxMtMxStRtrOrAfe(AudioStreamMgr_StreamStateType *pStream);
void ASM_GetNextSortedStreamAndNode(AudioStreamMgr_SessionStateType *pSession,
      ASM_StreamIdNodeIndex_t *stream_id_node_ind);

ASM_MtxStRtrType ASM_GetMatrixOrStreamRouterNode(AudioStreamMgr_StreamStateType *pSession,
      ASM_direction direction, ASM_GraphNodeIndex* asmNodeIndex);

bool_t ASM_IsRxDeviceHandleNeeded(AudioStreamMgr_StreamStateType *pStream);
ASM_GraphNodeIndex ASM_CheckForAnotherDeviceHandle(AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex dev_node_ind);
ASM_GraphNodeIndex ASM_GetRxDeviceNodeInTopo(AudioStreamMgr_StreamStateType *pStream);
ASM_GraphNodeIndex ASM_GetTxDeviceNodeInTopo(AudioStreamMgr_StreamStateType *pStream);

AudioStreamMgr_StreamStateType *ASM_AllotStream(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id);

void ASM_ReleaseStream(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id);
void ASM_ReleaseGaplessStream(AudioStreamMgr_SessionStateType *pSession, AudioStreamMgr_StreamStateType *pStream);

AudioStreamMgr_StreamStateType *ASM_GetStreamInstance(AudioStreamMgr_SessionStateType *pSession, uint8_t stream_id);

/* gives the first opened stream
 * temp approach until better method is used - useful for ADM -> ASM commands.
 */
AudioStreamMgr_StreamStateType *ASM_GetAnOpenedStream(AudioStreamMgr_SessionStateType *pSession);

uint32_t ASM_GetNumStreamsOpen(AudioStreamMgr_SessionStateType *pSession);
uint32_t ASM_GetNumStreamsConnectedToRxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession);
AudioStreamMgr_StreamStateType *ASM_GetStreamConnectedToRxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession);
AudioStreamMgr_StreamStateType *ASM_GetStreamConnectedToTxMatrixStRtr(AudioStreamMgr_SessionStateType *pSession);

ADSPResult ASM_SortStreamsForPathDelay(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession);
ADSPResult ASM_SortStreams(AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession, ASM_STREAM_SORT_METHOD method);
void ASM_StoreNodesInTempStreamIdNodeIndexList(AudioStreamMgr_SessionStateType *pSession, ASM_topo_type topo_type);
ADSPResult ASM_CheckIntraSessionTopoErrors(AudioStreamMgr_SessionStateType *pSession);

ADSPResult ASM_IterateCmdOverNodesAndStreamsParallely(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      ASM_ExecRunPauseSuspendFn execFn);

ADSPResult ASM_IterateRunPauseSuspendOverNodesAndStreams(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      ASM_GraphNodeIndex curr_node_index,
      ASM_ExecRunPauseSuspendFn execFn);

ADSPResult ASM_ParseOpenReadPush(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenWritePull(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenReadCompr(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenWriteCompr(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenLoopback(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenRead(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenReadWrite(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);
ADSPResult ASM_ParseOpenWrite(elite_apr_packet_t *pPkt, ASM_OpenStreamParams_t *pOpen, uint8_t int_session_id, uint8_t stream_id);

ADSPResult AudioStreamMgr_StreamSendMtMXMsgForRateMatching(AudioStreamMgr_t * pMe,
		AudioStreamMgr_SessionStateType *pSession,
		AudioStreamMgr_StreamStateType *pStream);

ADSPResult ASM_RunNewlyOpenedStream(AudioStreamMgr_t * pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream);

/* Function to check if a stream is part of a Tx Topology
 * @param pStream [in]: Stream to check topo
 *
 * @return: True if the stream is part of a Tx topo
 *          False otherwise, or if an invalid stream is provided
 */
bool_t AudioStreamMgr_StreamTopoIsTxTopo(AudioStreamMgr_StreamStateType* pStream);
volatile uint32_t* AudioStreamMgr_GetPostDecSvcVarBufDelay(AudioStreamMgr_StreamStateType *pStream);

ADSPResult AudioStreamMgr_SendRegisterEventCmdToDynaSvc(AudioStreamMgr_t* pMe, AudioStreamMgr_StaticSessionStateType *pStSession,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex nodeIndex,
      elite_apr_packet_t *pPkt);

ADSPResult AudioStreamMgr_SendIEC61937RegisterEventCmdToDynaSvc(AudioStreamMgr_t* pMe, AudioStreamMgr_StaticSessionStateType *pStSession,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream, ASM_GraphNodeIndex nodeIndex,
      elite_apr_packet_t *pPkt);

union ASM_ClientToken_t
{
   uint32_t client_token;
   struct
   {
      uint8_t stream_id;
      uint8_t node_index;
      bool_t  is_custom_cmd : 1; //whether the cmd that originated this cmd-rsp seq is a custom cmd (& not apr cmd).
      unsigned int reserved1 : 3;
      unsigned int custom_data : 4; // This data is specific to the particular command.
      uint8_t reserved2;
   };
};

static inline uint32_t ASM_PopulateClientTokenForCustomCmd(uint8_t stream_id, ASM_GraphNodeIndex node_index)
{
   ASM_ClientToken_t client_token;

   //both inputs are assumed to be < 256
   QURT_ELITE_ASSERT(node_index < ((1<<(8*sizeof(client_token.node_index))))); //8 is num of bits in bytes
   QURT_ELITE_ASSERT(stream_id < ((1<<(8*sizeof(client_token.stream_id)))));

   client_token.client_token = 0;

   client_token.stream_id = stream_id;
   client_token.node_index = node_index;
   client_token.is_custom_cmd = TRUE;

   return client_token.client_token;
}

static inline uint32_t ASM_PopulateClientToken(uint8_t stream_id, ASM_GraphNodeIndex node_index)
{
   ASM_ClientToken_t client_token;

   //both inputs are assumed to be < 256
   QURT_ELITE_ASSERT(node_index < ((1<<(8*sizeof(client_token.node_index))))); //8 is num of bits in bytes
   QURT_ELITE_ASSERT(stream_id < ((1<<(8*sizeof(client_token.stream_id)))));

   client_token.client_token = 0;

   client_token.stream_id = stream_id;
   client_token.node_index = node_index;

   return client_token.client_token;
}

static inline void ASM_AddCustomDataToClientToken(uint32_t *client_token_ptr, unsigned int custom_data)
{
   const uint32_t NUM_BITS_IN_CUSTOM_DATA = 4;
   QURT_ELITE_ASSERT(custom_data < (1 << NUM_BITS_IN_CUSTOM_DATA));

   ASM_ClientToken_t client_token;
   client_token.client_token = *client_token_ptr;
   client_token.custom_data = custom_data;
   *client_token_ptr = client_token.client_token;
}

static inline uint8_t ASM_GetIsCustomCmdFromClientToken(uint32_t client_token)
{
   ASM_ClientToken_t token;
   token.client_token = client_token;

   return token.is_custom_cmd;
}

static inline uint8_t ASM_GetStreamIdFromClientToken(uint32_t client_token)
{
   ASM_ClientToken_t token;
   token.client_token = client_token;

   return token.stream_id;
}

static inline ASM_GraphNodeIndex ASM_GetNodeIndexFromClientToken(uint32_t client_token)
{
   ASM_ClientToken_t token;
   token.client_token = client_token;

   return (ASM_GraphNodeIndex)(token.node_index);
}

static inline unsigned int ASM_GetCustomDataFromClientToken(uint32_t client_token)
{
   ASM_ClientToken_t token;
   token.client_token = client_token;

   return token.custom_data;
}


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_STREAM_UTIL_H

