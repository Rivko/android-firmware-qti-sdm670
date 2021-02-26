/**
@file AudStreamMgr_Session.cpp

@brief This file contains the implementation for session-control-related functions 
for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_Session.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_Stream.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"
#include "StreamRouterSvc.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"

#include "MixerSvc_Interface.h"
#include "adsp_media_fmt.h"

#include "AudioStreamMgr_adsppm.h"

#include "qurt_elite_globalstate.h"

extern uint32_t asmMemoryMapClient;

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
// maximum number of commands expected ever in command queue.
static const uint32_t AUDIO_STREAM_MGR_MAX_CMD_Q_ELEMENTS = 4;
static const uint32_t AUDIO_STREAM_MGR_MAX_RESP_Q_ELEMENTS = 16;

//declare buffers for maximumm number of sessions
static char asmSessionCmdQ[ASM_MAX_SUPPORTED_SESSION_ID][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AUDIO_STREAM_MGR_MAX_CMD_Q_ELEMENTS)];
static char asmSessionRespQ[ASM_MAX_SUPPORTED_SESSION_ID][QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AUDIO_STREAM_MGR_MAX_RESP_Q_ELEMENTS)];


/* Function table for Suspend command:
 */
const ASM_RunPauseFlushFTableEntry ASM_SuspendFTable[] =
{
      /* Decoder */
      {AudioStreamMgr_SessionSuspendCodecSvc, ELITEMSG_CUSTOM_DEC_SUSPEND, ASM_DEC_SVC},

      /* PP : To disable HW modules if used */
      {AudioStreamMgr_SessionSuspendProcSvc, ELITEMSG_CUSTOM_PP_SUSPEND, 0},

      /* Rx Matrix */
      {AudioStreamMgr_SessionSuspendMatrixSvc, ELITEMSG_CUSTOM_MT_MX_SUSPEND, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_SessionSuspendMatrixSvc, ELITEMSG_CUSTOM_MT_MX_SUSPEND, ASM_TX_DIR},

      /* Switch : No implementation yet */
      {NULL, 0, 0},

      /* Encoder */
      {AudioStreamMgr_SessionSuspendCodecSvc, ELITEMSG_CUSTOM_ENC_SUSPEND, ASM_ENC_SVC},

      /* Rx Stream Router */
      {AudioStreamMgr_SuspendStreamRouterSvc, ELITEMSG_CUSTOM_STREAM_ROUTER_SUSPEND, ASM_RX_DIR},

      /* Tx Stream Router */
      {AudioStreamMgr_SuspendStreamRouterSvc, ELITEMSG_CUSTOM_STREAM_ROUTER_SUSPEND, ASM_TX_DIR},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},

};
/* Function table and its correponding opcode.
Flush operation is by calling this function table in sequential order,
results in a flush from upstream to downstream). 
The last parameter (direction) is relevant only for Loopback mode 
 */

const ASM_RunPauseFlushFTableEntry ASM_FlushFTable[] = 
{
      /* Decoder */
      {AudioStreamMgr_StreamFlushCodecSvc, ELITE_CMD_FLUSH, ASM_DEC_SVC},

      /* PP */
      {AudioStreamMgr_StreamFlushProcSvc, ELITE_CMD_FLUSH, ASM_DONT_CARE_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_StreamFlushMatrixSvc, ELITEMSG_CUSTOM_MT_MX_FLUSH, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_StreamFlushMatrixSvc, ELITEMSG_CUSTOM_MT_MX_FLUSH, ASM_TX_DIR},

      /* Switch */
      {AudioStreamMgr_StreamFlushSwitchSvc, ELITEMSG_CUSTOM_SWITCH_FLUSH, ASM_DONT_CARE_DIR},

      /* Encoder */
      {AudioStreamMgr_StreamFlushCodecSvc, ELITE_CMD_FLUSH, ASM_ENC_SVC},

      /* Rx Stream Router */
      {AudioStreamMgr_StreamFlushStreamRouterSvc, ELITE_CMD_FLUSH, ASM_RX_DIR},

      /* Tx Stream Router */
      {NULL, 0, 0},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {AudioStreamMgr_StreamFlushAFE, 0, ASM_TX_DIR},
};

/* Function table for Pause1 sequence:
Playback:  proc, matrix and then midi(if applicable)
Record:    proc, matrix
NT:        proc

 */
const ASM_RunPauseFlushFTableEntry ASM_PauseFTable1[] =
{
      /* Decoder */
      {NULL, 0, 0},

      /* PP */
      {AudioStreamMgr_SessionRunPauseProcSvc, ELITEMSG_CUSTOM_PP_PAUSE_PREPARE, ASM_DONT_CARE_DIR},

      /* Rx Matrix */
      {NULL, 0, 0},

      /* Tx Matrix */
      {NULL, 0, 0},

      /* Switch */
      {NULL, 0, 0},

      /* Encoder: needed for push/pull mode */
      {NULL, 0, 0},

      /* Rx Stream Router */
      {NULL, 0, 0},

      /* Tx Stream Router */
      {NULL, 0, 0},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},
};

/* Function table for Pause2 sequence:
Playback:  proc, matrix and then midi(if applicable)
Record:    proc, matrix 
NT:        proc

 */
const ASM_RunPauseFlushFTableEntry ASM_PauseFTable2[] =
{ 
      /* Decoder */
      {AudioStreamMgr_SessionRunPauseCodecSvc, ELITE_CMD_PAUSE, ASM_DEC_SVC},

      /* PP */
      {AudioStreamMgr_SessionRunPauseProcSvc, ELITEMSG_CUSTOM_PP_PAUSE, ASM_DONT_CARE_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_SessionRunPauseMatrixSvc, ELITEMSG_CUSTOM_MT_MX_PAUSE, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_SessionRunPauseMatrixSvc, ELITEMSG_CUSTOM_MT_MX_PAUSE, ASM_TX_DIR},

      /* Switch */
      {NULL, 0, 0},

      /* Encoder: needed for push/pull mode */
      {AudioStreamMgr_SessionRunPauseCodecSvc, ELITE_CMD_PAUSE, ASM_ENC_SVC},

      /* Rx Stream Router */
      {AudioStreamMgr_SessionRunPauseStreamRouterSvc, ELITE_CMD_PAUSE, ASM_RX_DIR},

      /* Tx Stream Router */
      {NULL, 0, 0},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},
};   

/*
Function table for Run Sequence:
Playback:  midi (if applicable),  matrix AND proc
Record:    matrix and proc
NT:        proc
 */
const ASM_RunPauseFlushFTableEntry ASM_RunFTable[] = 
{ 
      /* Decoder */
      {AudioStreamMgr_SessionRunPauseCodecSvc, ELITE_CMD_RUN, ASM_DEC_SVC},

      /* PP */
      {AudioStreamMgr_SessionRunPauseProcSvc, ELITE_CMD_RUN, ASM_DONT_CARE_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_SessionRunPauseMatrixSvc, ELITEMSG_CUSTOM_MT_MX_RUN, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_SessionRunPauseMatrixSvc, ELITEMSG_CUSTOM_MT_MX_RUN, ASM_TX_DIR},

      /* Switch */
      {NULL, 0, 0},

      /* Encoder: needed for push/pull mode */
      {AudioStreamMgr_SessionRunPauseCodecSvc, ELITE_CMD_RUN, ASM_ENC_SVC},

      /* Rx Stream Router */
      {AudioStreamMgr_SessionRunPauseStreamRouterSvc, ELITE_CMD_RUN, ASM_RX_DIR},

      /* Tx Stream Router */
      {AudioStreamMgr_SessionRunPauseStreamRouterSvc, ELITE_CMD_RUN, ASM_TX_DIR},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},
};  

/*
 Table for looking up the function table for delay queries
 */
const ASM_RunPauseFlushFTableEntry *ASM_QueryDelayFTables[] = {
   ASM_QueryDelayFTableRx, // ASM_RX_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_TX_MODE
   ASM_QueryDelayFTableNT, // ASM_NT_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_LOOPBACK_PCM_IN_PCM_OUT_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_LOOPBACK_PCM_IN_COMPRESSED_OUT_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_LOOPBACK_COMPRESSED_IN_PCM_OUT_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_LOOPBACK_COMPRESSED_IN_COMPRESSED_OUT_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_LOOPBACK_COMPRESSED_IN_MULTI_OUT_MODE
   ASM_QueryDelayFTableRx, // ASM_COMPRESSED_PLAYBACK_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_COMPRESSED_INPUT_MODE
   ASM_QueryDelayFTableRx, // ASM_RX_PULL_MODE
   ASM_QueryDelayFTableTxLoopback, // ASM_TX_PUSH_MODE
   ASM_QueryDelayFTableRx, // ASM_UNKNOWN_MODE
   ASM_QueryDelayFTableDummy // To handle the case when a new mode is added but this table is not updated.
};

const uint32_t ASM_QueryDelayFTablesSize = sizeof(ASM_QueryDelayFTables)/sizeof(ASM_QueryDelayFTables[0]);

/*
Function table for querying the delay for the Rx path:
 */
const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableRx[] =
{
      /* Decoder */
      {AudioStreamMgr_StreamUpdatePathDelayForDecoderSvc, 0, ASM_RX_DIR},

      /* PP */
      {AudioStreamMgr_StreamGetPathDelayForPPService, 0, ASM_RX_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_StreamGetDevicePathDelayUncompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_StreamGetDevicePathDelayUncompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_TX_DIR},

      /* Switch */
      {AudioStreamMgr_StreamUpdatePathDelayForSwitchSvc, 0, ASM_RX_DIR},

      /* Encoder */
      {AudioStreamMgr_StreamUpdatePathDelayForEncoderSvc, 0, ASM_RX_DIR},

      /* Rx Stream Router */
      {AudioStreamMgr_StreamGetDevicePathDelayCompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_RX_DIR},

      /* Tx Stream Router */
      {AudioStreamMgr_StreamGetDevicePathDelayCompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_TX_DIR},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},
};

/*
Function table for querying the delay for the Tx and loopback paths:
 */
const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableTxLoopback[] =
{
      /* Decoder */
      {AudioStreamMgr_StreamUpdatePathDelayForDecoderSvc, 0, ASM_TX_DIR},

      /* PP */
      {AudioStreamMgr_StreamGetPathDelayForPPService, 0, ASM_TX_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_StreamGetDevicePathDelayUncompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_StreamGetDevicePathDelayUncompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_TX_DIR},

      /* Switch */
      {AudioStreamMgr_StreamUpdatePathDelayForSwitchSvc, 0, ASM_TX_DIR},

      /* Encoder */
      {AudioStreamMgr_StreamUpdatePathDelayForEncoderSvc, 0, ASM_TX_DIR},

      /* Rx Stream Router */
      {AudioStreamMgr_StreamGetDevicePathDelayCompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_RX_DIR},

      /* Tx Stream Router */
      {AudioStreamMgr_StreamGetDevicePathDelayCompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_TX_DIR},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {AudioStreamMgr_StreamGetDevicePathDelayUncompressed, ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY, ASM_TX_DIR},
};

/*
Function table for querying the delay for the NT path.
 */
const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableNT[] =
{
      /* Decoder */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* PP */
      {AudioStreamMgr_StreamGetPathDelayForPPServiceAlgOnly, 0, ASM_RX_DIR},

      /* Rx Matrix */
      {NULL, 0, 0},

      /* Tx Matrix */
      {NULL, 0, 0},

      /* Switch */
      {NULL, 0, 0},

      /* Encoder */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Rx Stream Router */
      {NULL, 0, 0},

      /* Tx Stream Router */
      {NULL, 0, 0},

      /* AFE Rx */
      {NULL, 0, 0},

      /* AFE Tx */
      {NULL, 0, 0},
};

/*
Function table for querying the delay for new modes for which tables are not defined.
 */
const ASM_RunPauseFlushFTableEntry ASM_QueryDelayFTableDummy[] =
{
      /* Decoder */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* PP */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Rx Matrix */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Tx Matrix */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Switch */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Encoder */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Rx Stream Router */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* Tx Stream Router */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* splitter */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* combiner */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* AFE Rx */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},

      /* AFE Tx */
      {AudioStreamMgr_StreamUpdatePathDelayZero, 0, ASM_RX_DIR},
};

/*
Function table for creating dynamic svcs
 */
const ASM_CreateDynaSvcFTableEntry ASM_CreateDynaSvcFTable[] =
{
      /* Decoder */
      {AudioStreamMgr_CreateDecSvc},

      /* PP */
      {AudioStreamMgr_CreatePPSvc},

      /* Rx Matrix */
      {NULL},

      /* Tx Matrix */
      {NULL},

      /* Switch */
      {AudioStreamMgr_CreateSwitchSvc},

      /* Encoder */
      {AudioStreamMgr_CreateEncSvc},

      /* Rx Stream Router */
      {NULL},

      /* Tx Stream Router */
      {NULL},

      /* AFE Rx */
      {NULL},

      /* AFE Tx */
      {NULL},
};

/*
Function table for closing dynamic svcs: usually destroy command is generic, but close is used for those svcs
which handle multiple streams.
 */
const ASM_CloseDynaSvcFTableEntry ASM_CloseDynaSvcFTable[] =
{
      /* Decoder */
      {AudioStreamMgr_CloseStreamDecSvc},

      /* PP */
      {NULL},

      /* Rx Matrix */
      {NULL},

      /* Tx Matrix */
      {NULL},

      /* Switch */
      {NULL},

      /* Encoder */
      {NULL},

      /* Rx Stream Router */
      {NULL},

      /* Tx Stream Router */
      {NULL},

      /* AFE Rx */
      {NULL},

      /* AFE Tx */
      {NULL},
};

/*
Function table for closing dynamic svcs: usually destroy command is generic, but close is used for those svcs
which handle multiple streams.
 */
const ASM_ConnectDynaSvcFTableEntry ASM_ConnectDynaSvcFTable[] =
{
      /* Decoder */
      {AudioStreamMgr_ConnectStreamDecSvc},

      /* PP */
      {AudioStreamMgr_ConnectStreamGeneric},

      /* Rx Matrix */
      {AudioStreamMgr_ConnectStreamGeneric},

      /* Tx Matrix */
      {NULL},

      /* Switch */
      {AudioStreamMgr_ConnectStreamGeneric},

      /* Encoder */
      {AudioStreamMgr_ConnectStreamGeneric},

      /* Rx Stream Router */
      {AudioStreamMgr_ConnectStreamGeneric},

      /* Tx Stream Router */
      {NULL},

      /* AFE Rx */
      {NULL},

      /* AFE Tx */
      {NULL},
};

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/* =======================================================================
 **                          Session State Machine Create/Destroy
 ** ======================================================================= */
ADSPResult AudioStreamMgr_SessionAckFail( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      ADSPResult nRes)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   return AudioStreamMgr_GenerateAck( pPkt, nRes, NULL, 0, 0 );
}

/* This function creates all sessions state machine here. */
ADSPResult AudioStreamMgr_CreateAllSessionStates(AudioStreamMgr_t *pMe) 
{
   int i;

   for (i = 0; i < ASM_MAX_SUPPORTED_SESSION_ID ; i ++ )
   {
      if ( ADSP_FAILED( AudioStreamMgr_CreateStaticSessionState( pMe, i) ))
      {         
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create AudioStreamMgr session %d!", i);
         return ADSP_EFAILED; 
      }
   }

#if (ASM_MAX_SUPPORTED_SESSION_ID > ASM_NUM_QUEUES)
#error ("max sessions should be less than max queues in global state");
#endif

   /*update global state with session queue pointer information*/
   for (i = 0; i < ASM_MAX_SUPPORTED_SESSION_ID ; i ++ )
   {
      AudioStreamMgr_StaticSessionStateType *pStSession = &(pMe->aStSessions[i]);
      /* since pAsmSessionCmdQ is accessed by others,
       * external session id should be used as index. This means one more element. */
      qurt_elite_globalstate.pAsmSessionCmdQ[i+1] = pStSession->pSessionCmdQ;
   }

   /* Assign the state of the preallocated session to close to mark that they are not used */
   for (i = 0; i < ASM_MAX_PREALLOCED_SESSIONS ; i ++ )
   {
      AudioStreamMgr_SessionStateType *pSession = &(pMe->aSessionsBuf[i]);
      AudioStreamMgr_CreateSessionState(pSession, ASM_INVALID_INT_SESSION_ID);
   }

   return ADSP_EOK; 
}

ADSPResult AudioStreamMgr_CreateSessionState(AudioStreamMgr_SessionStateType *pSession, uint8_t intSessionId)
{
   ADSPResult result = ADSP_EOK;

   result = AudioStreamMgr_CreateAllStreamStates(pSession);

   if (ADSP_SUCCEEDED(result))
   {
      pSession->unState = AUDIO_STREAM_MGR_SESSION_CLOSE;
      pSession->ucInternalSessionId = intSessionId;
   }

   return result;
}

/**
 * \brief This function creates the necessary session state
 * structure given the internal session ID that it needs to
 * create. Currently only supports up to 15 sessions.
 *
 */
ADSPResult AudioStreamMgr_CreateStaticSessionState(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = &(pMe->aStSessions[intSessionId]);
   ADSPResult result;
   int nStartBit; 
   qurt_elite_queue_t *pTempCmdQ, *pTempRespQ;

   /*Sanity Check for debugging only*/
   QURT_ELITE_ASSERT( intSessionId < ASM_MAX_SUPPORTED_SESSION_ID );

   /* Create cmd  and resp queue */
   char aCmdQName[QURT_ELITE_DEFAULT_NAME_LEN], aRespQName[QURT_ELITE_DEFAULT_NAME_LEN];
   snprintf(aCmdQName, QURT_ELITE_DEFAULT_NAME_LEN,"ASMsc%d", intSessionId);
   snprintf(aRespQName, QURT_ELITE_DEFAULT_NAME_LEN,"ASMsr%d", intSessionId);

   pTempCmdQ = (qurt_elite_queue_t *)&asmSessionCmdQ[intSessionId];
   pTempRespQ = (qurt_elite_queue_t *)&asmSessionRespQ[intSessionId];

   // cmd and resp queue init for each ASM session 
   if (ADSP_FAILED(result = qurt_elite_queue_init(aCmdQName,AUDIO_STREAM_MGR_MAX_CMD_Q_ELEMENTS,pTempCmdQ))
         || ADSP_FAILED(result = qurt_elite_queue_init(aRespQName,AUDIO_STREAM_MGR_MAX_RESP_Q_ELEMENTS, pTempRespQ)))

   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create AudioStreamMgr session cmd nd resp queues!");
      return result;
   }
   pStSession->pSessionCmdQ = pTempCmdQ;
   pStSession->pSessionRespQ = pTempRespQ;

   /* Calcuate the bit mask for this session's command queue.
   The system reserves bit 0 and 1 of the channel. Session X
   cmd queue shall take bit 2*x+2 and rep queue takes bit
   2*x+3 */
   nStartBit   = 2 * intSessionId  + 2;
   pStSession->unSessionCmdQMask   = 1 << nStartBit;

   /* Add queue to channel */
   if ( ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, 
         pStSession->pSessionCmdQ,
         pStSession->unSessionCmdQMask))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel,
               pStSession->pSessionRespQ,
               pStSession->unSessionCmdQMask << 1))
   )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "ADSP_FAILED to add mqs to channel result = %d!", result);
      return result;
   }

   /* Immediately switch on channel */
   pMe->unCurrentBitfield |= pStSession->unSessionCmdQMask;

   pMe->pSessions[intSessionId] = NULL;

   return ADSP_EOK;
}

/** This function initializes the session state variables
 *  every time a stream is opened in a new session  */
void AudioStreamMgr_InitializeSessionState(AudioStreamMgr_SessionStateType *pSession)
{

   //if the session is closed, init all svc state structs
   if(AUDIO_STREAM_MGR_SESSION_CLOSE == pSession->unState)
   {
      pSession->unMode = ASM_RX_MODE;
      pSession->prev_floor_kips = 0;
      pSession->prev_total_kips = 0;
      pSession->pFirstGaplessStrm = NULL;

      memset(&pSession->prev_run_cmd, 0, sizeof(pSession->prev_run_cmd));
   }

   pSession->unReqResps = 0;
   pSession->nOverallResult = ADSP_EOK;
}

/**
 * This function  simply cleans the queue but do not
 * destroy it. Use it with precaution. Potential corner case is
 * that when this function try to clean queue, another thread
 * keeps on putting the message into the queue.
 *
 * @param cmdQ [in] Queue to be drained
 * @return ADSP_ENEEDMORE indicates sucess.
 */
ADSPResult AudioStreamMgr_CleanQ( qurt_elite_queue_t *cmdQ)
{
   ADSPResult result = ADSP_EOK;
   elite_msg_any_t cmdMsg;

   // clean up command queue
   if (cmdQ)
   {
      // Drain any queued commands
      do
      {
         if (ADSP_SUCCEEDED(result = qurt_elite_queue_pop_front(cmdQ, (uint64_t*)&cmdMsg)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Got a message after DESTROY command!!\n");
            /** Should release message buffer if response is not required 
             * elite_svc_send_ack(&cmdMsg, ADSP_ENOTREADY);
             */
            elite_msg_finish_msg( &cmdMsg, ADSP_ENOTREADY);  
         }

      } while (ADSP_EOK == result);
   }


   return ( (result == ADSP_ENEEDMORE) ? ADSP_EOK:result ) ;
}

ADSPResult AudioStreamMgr_DestroyAllSessionState(AudioStreamMgr_t *pMe)
{
   ADSPResult result = ADSP_EOK;

   for (int i = 0; i < ASM_MAX_SUPPORTED_SESSION_ID; i++ )
   {
      result |= AudioStreamMgr_DestroyStaticSessionState(pMe, i);
   }

   /*update global state with session queue pointer information*/
   for (int i =0; i< ASM_MAX_SUPPORTED_SESSION_ID ; i ++ )
   {
      qurt_elite_globalstate.pAsmSessionCmdQ[i+1] = NULL;
   }

   /* Assign the state of the preallocated session to close to mark that they are not used */
   for (int i =0; i< ASM_MAX_PREALLOCED_SESSIONS ; i ++ )
   {
      AudioStreamMgr_SessionStateType *pSession = &(pMe->aSessionsBuf[i]);
      AudioStreamMgr_CreateSessionState(pSession, ASM_INVALID_INT_SESSION_ID);
   }

   return result;
}
/**
 * This function destroy session state assuming this session is
 * no longer in use. This function should only be called when
 * the whole static service is destroyed
 *
 *
 */
ADSPResult AudioStreamMgr_DestroyStaticSessionState(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{

   /**Sanity Check for debugging only*/
   QURT_ELITE_ASSERT( intSessionId < ASM_MAX_SUPPORTED_SESSION_ID  );

   AudioStreamMgr_StaticSessionStateType *pStSession=NULL;
   if(intSessionId < ASM_MAX_SUPPORTED_SESSION_ID)	//additional check needed otherwise KW is saying pSession to be an array of -ve size
      pStSession = &(pMe->aStSessions[intSessionId]);

   QURT_ELITE_ASSERT( pStSession );

   if(NULL == pStSession)
   {
      return ADSP_EFAILED;
   }
   elite_svc_deinit_cmd_queue(pStSession->pSessionCmdQ);
   elite_svc_deinit_cmd_queue(pStSession->pSessionRespQ);

   memset(pStSession, 0, sizeof(AudioStreamMgr_StaticSessionStateType));

   pMe->pSessions[intSessionId] = NULL;

   return ADSP_EOK;
}

/* =======================================================================
 **                          Session Utility functions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_AllocateSession(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   ADSPResult result = ADSP_EOK;

   // intSessionId ranges from 0 to ASM_MAX_SUPPORTED_SESSION_ID-1
   if ( (intSessionId >= ASM_MAX_SUPPORTED_SESSION_ID))
   {
      return ADSP_EFAILED;
   }

   if (pMe->pSessions[intSessionId])
   {
      return ADSP_EOK;
   }

   for (uint8_t i = 0; i < ASM_MAX_PREALLOCED_SESSIONS; i++)
   {
      if ((uint8_t)ASM_INVALID_INT_SESSION_ID == pMe->aSessionsBuf[i].ucInternalSessionId)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Allocating the session from prealloced mem", intSessionId);
         pMe->pSessions[intSessionId] = &pMe->aSessionsBuf[i];
         result = AudioStreamMgr_CreateSessionState(pMe->pSessions[intSessionId], intSessionId);
         return ADSP_EOK;
      }
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Allocating the session from heap mem", intSessionId);
   pMe->pSessions[intSessionId] = (AudioStreamMgr_SessionStateType*)
                                 qurt_elite_memory_malloc(sizeof(AudioStreamMgr_SessionStateType), QURT_ELITE_HEAP_DEFAULT);

   if (NULL == pMe->pSessions[intSessionId])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Fail to alloc memory for session", intSessionId);
      result = ADSP_ENOMEMORY;
   }
   else
   {
      result = AudioStreamMgr_CreateSessionState(pMe->pSessions[intSessionId], intSessionId);
   }

   return (ADSP_FAILED(result) ? ADSP_EFAILED : ADSP_EOK);
}

ADSPResult AudioStreamMgr_DeallocateSessionIfLastStream(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   if (!ASM_GetNumStreamsOpen(pMe->pSessions[intSessionId]))
   {
      return AudioStreamMgr_DeallocateSession(pMe, intSessionId);
   }
   return ADSP_EOK;
}

ADSPResult AudioStreamMgr_DeallocateSession(AudioStreamMgr_t *pMe, uint8_t intSessionId)
{
   ADSPResult result = ADSP_EOK;

   // intSessionId ranges from 0 to ASM_MAX_SUPPORTED_SESSION_ID-1
   if ( (intSessionId >= ASM_MAX_SUPPORTED_SESSION_ID))
   {
      return ADSP_EFAILED;
   }

   if (NULL == pMe->pSessions[intSessionId])
   {
      return ADSP_EOK;
   }

   for (uint8_t i = 0; i < ASM_MAX_PREALLOCED_SESSIONS; i++)
   {
      if (pMe->pSessions[intSessionId]->ucInternalSessionId == pMe->aSessionsBuf[i].ucInternalSessionId)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Deallocating the session to prealloced mem", intSessionId);
         pMe->aSessionsBuf[i].unState = AUDIO_STREAM_MGR_SESSION_CLOSE;
         pMe->aSessionsBuf[i].oldestOpenStreamId = 0;
         pMe->aSessionsBuf[i].ucInternalSessionId = ASM_INVALID_INT_SESSION_ID;
         pMe->pSessions[intSessionId] = NULL;
         return ADSP_EOK;
      }
   }

   if (pMe->pSessions[intSessionId])
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,-]: Deallocating the session to heap mem", intSessionId);
      qurt_elite_memory_free(pMe->pSessions[intSessionId]);
      pMe->pSessions[intSessionId] = NULL;

   }
   return result;
}

ADSPResult AudioStreamMgr_SessionDestroy(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EOK;

   for (uint8_t i = 1; i <= ASM_MAX_SUPPORTED_STREAM_ID; i++)
   {
      if (ASM_INVALID_INDEX != pSession->ucStreamId2Index[i])
      {
         AudioStreamMgr_StreamStateType *pStream = &pSession->aStreams[pSession->ucStreamId2Index[i]];
         result |= AudioStreamMgr_StreamDestroy(pMe, pSession, pStream);
      }
   }

   return (ADSP_FAILED(result) ? ADSP_EFAILED : ADSP_EOK);
}

ADSPResult AudioStreamMgr_SessionSuspendProcSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unSecOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pProcSvcHandle = NULL ;
   uint32_t result;
   elite_msg_any_t msg;
   uint8_t stream_id   = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);

   /* payload for Suspend command */
   uint32_t unPayloadSize = sizeof(elite_msg_custom_header_t);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);
   if ( ADSP_FAILED( result ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t *) (msg.pPayload);
   pPayload->unSecOpCode = unSecOpcode;
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Suspending Proc svc. token 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unSecOpcode);
      return result;
   }

   return result;
}

ADSPResult AudioStreamMgr_SessionRunPauseProcSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pProcSvcHandle = NULL ;
   uint32_t result;
   elite_msg_any_t msg;
   uint8_t stream_id   = pStream->ucStreamId;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( unOpcode != ELITEMSG_CUSTOM_PP_PAUSE 
         && unOpcode != ELITEMSG_CUSTOM_PP_PAUSE_PREPARE
         && unOpcode != ELITE_CMD_RUN) {
      return ADSP_EBADPARAM;
   }

   pProcSvcHandle = (pStream->nodes[ASM_NODE_PP]).pSvcHandle;
   uint32_t ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_PP);

   if(ELITE_CMD_RUN == unOpcode )
   {
      uint32_t unPayloadSize = sizeof(elite_msg_any_payload_t);

      result = elite_msg_create_msg( &msg, &unPayloadSize,
            unOpcode,
            pStSession->pSessionRespQ,
            ulClientToken,
            0);
      if ( ADSP_FAILED( result ) ) 
      {

         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
               pSession->ucInternalSessionId,  stream_id);
         return result;      
      }
   }
   else 
   {
      uint32_t unPayloadSize = sizeof(EliteMsg_CustomPPPauseType);
      result = elite_msg_create_msg( &msg, &unPayloadSize,
            ELITE_CUSTOM_MSG,
            pStSession->pSessionRespQ,
            ulClientToken,
            0);
      if ( ADSP_FAILED( result ) ) 
      {

         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
               pSession->ucInternalSessionId,  stream_id);
         return result;      
      }

      if (unOpcode == ELITEMSG_CUSTOM_PP_PAUSE_PREPARE)
      {
         //fill payload
         EliteMsg_CustomPPPausePrepareType *pPayload = (EliteMsg_CustomPPPausePrepareType *)msg.pPayload;
         pPayload->unSecOpCode = ELITEMSG_CUSTOM_PP_PAUSE_PREPARE;

         //if tunneled Tx or NT mode, EOS marker needs to be sent down, otherwise not
         pPayload->sendDownEosMarker = false;
         if((ASM_TX_MODE == pSession->unMode) || (ASM_NT_MODE == pSession->unMode))
         {
            pPayload->sendDownEosMarker = true;
         }

      }
      else
      {
         //fill payload
         EliteMsg_CustomPPPauseType *pPayload = (EliteMsg_CustomPPPauseType *)msg.pPayload;
         pPayload->unSecOpCode = ELITEMSG_CUSTOM_PP_PAUSE;

         //if tunneled Tx or NT mode, EOS marker needs to be sent down, otherwise not
         pPayload->sendDownEosMarker = false;
         if((ASM_TX_MODE == pSession->unMode) || (ASM_NT_MODE == pSession->unMode))
         {
            pPayload->sendDownEosMarker = true;
         }
      }
   }

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pProcSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   switch (unOpcode) {
   case ELITEMSG_CUSTOM_PP_PAUSE:
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pausing PP svc, token 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);

      break;
   case ELITEMSG_CUSTOM_PP_PAUSE_PREPARE:
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Begin Pausing PP svc. token 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);

      break;
   case ELITE_CMD_RUN:
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Running PP svc. token 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);

      break;
   default:
      result = ADSP_EBADPARAM;
   }

   return result;
}

ADSPResult AudioStreamMgr_SessionSuspendCodecSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unSecOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pDecEncSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg;
   uint32_t result;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if (NULL == pStream)
   {
      return ADSP_EBADPARAM;
   }

   if ( (unSecOpcode != ELITEMSG_CUSTOM_DEC_SUSPEND)&&(unSecOpcode != ELITEMSG_CUSTOM_ENC_SUSPEND))
   {
      return ADSP_EBADPARAM;
   }

   uint8_t stream_id = pStream->ucStreamId;


   if(ASM_DEC_SVC == unDir)
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_DEC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);
   }
   else
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_ENC);
   }

   /* payload for Suspend command */
   uint32_t unPayloadSize = sizeof(elite_msg_custom_header_t);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         ELITE_CUSTOM_MSG,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);
   if ( ADSP_FAILED( result ) )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;
   }

   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t *) (msg.pPayload);
   pPayload->unSecOpCode = unSecOpcode;
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Suspending Dec/Enc svc. token 0x%lx",
         pSession->ucInternalSessionId,  stream_id, ulClientToken);

   result = ASM_PushCmdToPeerSvc(pMe, pSession, pDecEncSvcHandle->cmdQ, (uint64_t*)&msg);
   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unSecOpcode);
      return result;
   }

   return result;
}

ADSPResult AudioStreamMgr_SessionRunPauseCodecSvc(  AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession,
      AudioStreamMgr_StreamStateType *pStream,
      uint32_t unOpcode,
      uint8_t unDir)
{

   elite_svc_handle_t *pDecEncSvcHandle = NULL ;
   uint32_t ulClientToken = 0;
   elite_msg_any_t msg; 
   uint32_t result; 
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   if ( unOpcode != ELITE_CMD_PAUSE 
         && unOpcode != ELITE_CMD_RUN )
   {
      return ADSP_EBADPARAM; 
   }

   if (NULL == pStream)
   {
      return ADSP_EBADPARAM;
   }
   uint8_t stream_id = pStream->ucStreamId;

   if(ASM_DEC_SVC == unDir)
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_DEC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_DEC);
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, 
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Flushing/Pausing/Running decoder service",
            pSession->ucInternalSessionId,  stream_id);
   } 
   else
   {
      pDecEncSvcHandle = (pStream->nodes[ASM_NODE_ENC]).pSvcHandle;
      ulClientToken = ASM_PopulateClientToken(stream_id, ASM_NODE_ENC);
      MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, 
            "AudioStreamMgr [Session ID = %u, Stream ID = %u]: Flushing/Pausing/Running encoder service",
            pSession->ucInternalSessionId,  stream_id);
   }

   // Run/Pause/Flush shall have the same payload right now. 
   uint32_t unPayloadSize = sizeof(elite_msg_any_payload_t);

   result = elite_msg_create_msg( &msg, &unPayloadSize,
         unOpcode,
         pStSession->pSessionRespQ,
         ulClientToken,
         0);
   if ( ADSP_FAILED( result ) ) 
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Fail to create mesage",
            pSession->ucInternalSessionId,  stream_id);
      return result;      
   }


   result = ASM_PushCmdToPeerSvc(pMe, pSession, pDecEncSvcHandle->cmdQ, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      //return the mesage to buf queue
      elite_msg_release_msg(&msg);
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: failed to send msg with opcode %lu ",
            pSession->ucInternalSessionId,  stream_id, unOpcode);
      return result;
   }

   switch (unOpcode) {
   case ELITE_CMD_PAUSE:
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pausing Dec/Enc svc, token 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);
      break;
   }
   case ELITE_CMD_RUN:
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Running Dec/Enc svc, token 0x%lx",
            pSession->ucInternalSessionId,  stream_id, ulClientToken);
      break;
   }
   default:
   {
      result = ADSP_EBADPARAM;
      break;
   }
   };

   return result;
}

/*
This utility function fills call back data for dynamic svc. 
 */
ADSPResult AudioStreamMgr_FillCbDatForDynSvc( AudioStreamMgr_t *pMe, AudioStreamMgr_SessionStateType *pSession,
      uint32_t client_token,
      AudioStreamMgr_CallBackHandleType *pCbData)
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   if ( pCbData )
   {

      elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

      pCbData->aprHandle   = AudioStreamMgr_GetAprHandle();             
      pCbData->clientAddr  = elite_apr_if_get_src_addr(pPkt); 
      pCbData->clientPort  = elite_apr_if_get_src_port(pPkt);   
      pCbData->selfAddr    = elite_apr_if_get_dst_addr(pPkt); 
      pCbData->selfPort    = elite_apr_if_get_dst_port(pPkt); 

      pCbData->pAsyncRespQ = pStSession->pSessionRespQ;

      pCbData->unAsynClientToken = client_token;
      return ADSP_EOK; 
   } else {
      return ADSP_EBADPARAM; 
   }

}

