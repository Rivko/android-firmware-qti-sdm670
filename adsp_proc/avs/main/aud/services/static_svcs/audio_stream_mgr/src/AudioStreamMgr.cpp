/**
@file AudStreamMgr.cpp

@brief This file contains the implementation for AudioStreamMgr.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr.cpp#2 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr.h"
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_CustomCmdHandler.h"
#include "AudioStreamMgr_SessionRespHandler.h"
#include "AudioStreamMgr_StreamRespHandler.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_SysAprCmdHandler.h"
#include "AudioStreamMgr_PrivateDefs.h"

#include "EliteMsg_Custom.h"
#include "AFEInterface.h"
#include "audio_basic_op_ext.h"

#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "EliteTopology_db_if.h"
#include "AudioStreamMgr_adsppm.h"
#include "aprv2_ids_services.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* Only support the first three msgs currently in system cmdQ */
#define AUDIO_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q 16


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// thread settings. Thread priority should go to separate file.
//static const uint32_t AUDIO_STREAM_MGR_THREAD_PRIO = 100;
static char THREAD_NAME[]="ASM";
static const uint32_t THREAD_STACK_SIZE = 16384;

// public queue name
char AUDIO_STREAM_MGR_SYS_CMD_Q_NAME[] = "ASMsysC" ;
static const uint32_t unSysCmdQMask = 0x00000001L;

//event queue name and mask
char AUDIO_STREAM_MGR_SYS_EVT_Q_NAME[] = "ASMsysE" ;
static const uint32_t unSysEvtQMask = 0x00000002L;

//qurt_elite MemoryMap Client
uint32_t asmMemoryMapClient;

//declaration of ASM service object
static AudioStreamMgr_t gAsmStatSvcObject;

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
// destructor
static void AudioStreamMgr_Destroy (AudioStreamMgr_t* pMe);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int AudioStreamMgr_WorkLoop(void* pInstance);

// Process session cmd Q
static ADSPResult AudioStreamMgr_ProcessSessionCmdQ(AudioStreamMgr_t *pMe, int32_t nChannelBit);

// Process session response Q
static ADSPResult AudioStreamMgr_ProcessSessionRespQ(AudioStreamMgr_t *pMe, int32_t nChannelBit);

// Process system CmdQ
static ADSPResult  AudioStreamMgr_ProcessSysCmdQ(AudioStreamMgr_t *pMe, int32_t nChannelBit);

// process system reserveQ
static ADSPResult AudioStreamMgr_ProcessSysReserveQ(AudioStreamMgr_t *pMe, int32_t nChannelBit) ;

static ADSPResult AudioStreamMgr_SysQUnSupportedHandler(AudioStreamMgr_t *pMe) ;
static ADSPResult AudioStreamMgr_SysQDestroySvcHandler(AudioStreamMgr_t *pMe) ;
static ADSPResult AudioStreamMgr_SysQStartSvcHandler(AudioStreamMgr_t *pMe) ;
static ADSPResult AudioStreamMgr_ProcessChannels(AudioStreamMgr_t *pMe, uint32_t channel_status);

/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */

/* Queue handler table.
   */
static AudioStreamMgr_QueueHandler_f pQHandler[AUDIO_STREAM_MGR_MAX_NUM_CHANNEL_BITS] =
{
   AudioStreamMgr_ProcessSysCmdQ,
   AudioStreamMgr_ProcessSysReserveQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 0
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 1
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 2
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 3
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 4
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 5
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 6
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 7
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 8
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 9
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 10
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 11
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 12
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 13
   AudioStreamMgr_ProcessSessionRespQ,
   AudioStreamMgr_ProcessSessionCmdQ,  // session 14
   AudioStreamMgr_ProcessSessionRespQ
};


/* Msg handler for system cmdQ. Only support 3 messages currently. */
static AudioStreamMgr_MsgHandler_f pSysCmdQMsgHandler[AUDIO_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q]
={
   AudioStreamMgr_SysQUnSupportedHandler, // ELITE_CUSTOM_MSG
   AudioStreamMgr_SysQStartSvcHandler,  // ELITE_CMD_START_SERVICE
   AudioStreamMgr_SysQDestroySvcHandler,    // ELITE_CMD_DESTROY_SERVICE
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQUnSupportedHandler,
   AudioStreamMgr_SysQAprPktHandler
};

/* Msg handler for session resp Q*/
static AudioStreamMgr_SessionMsgHandler_f AudioStreamMgr_SessionRespQHandler[] =
{
   AudioStreamMgr_SessionRespQCustomMsgHandler,
   AudioStreamMgr_SessionRespQUnSupportedHandler,//AudioStreamMgr_SessionRespQStartSvcHandler,
   AudioStreamMgr_SessionRespQDestroySvcHandler,
   AudioStreamMgr_SessionRespQConnectHandler,
   AudioStreamMgr_SessionRespQDisConnectHandler,
   AudioStreamMgr_SessionRespQOnPause2Ack,
   AudioStreamMgr_SessionRespQOnRunAck,
   AudioStreamMgr_SessionRespQOnFlushAck,
   AudioStreamMgr_SessionRespQSetParamHandler,
   AudioStreamMgr_SessionRespQGetParamHandler,
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_BUFFER
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_MEDIA_TYPE
   AudioStreamMgr_SessionRespQEosV2Handler,        ///ELITE_DATA_EOS
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_RAW_BUFFER
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_CMD_STOP_SERVICE
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_APR_PACKET
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_MIPS_CHANGE_EVENT
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_SET_PARAM
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DML_REQUEST_EVENT
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_BW_CHANGE_EVENT
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_MARK_BUFFER
   AudioStreamMgr_SessionRespQUnSupportedHandler,  ///ELITE_DATA_BUFFER_V2
   AudioStreamMgr_SessionRespQCustomEventHandler,  ///ELITE_CUSTOM_EVENT
   AudioStreamMgr_SessionRespQGenericAck,          ///ELITE_CMD_REGISTER_APR_EVENT


};

/* Msg handler for session command Q*/
static AudioStreamMgr_SessionCmdHandler_f AudioStreamMgr_SessionCmdQHandler[] =
{
   AudioStreamMgr_SessionCmdQCustomMsgHandler,    ///0 - ELITE_CUSTOM_MSG
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///1 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///2 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///3 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///4 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///5 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///6 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///7 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///8 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///9 - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///A - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///B - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///C - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///D - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///E - UNSUPPORTED CMD
   AudioStreamMgr_SessionCmdQAprPktHandler,       ///F - ELITE_APR_PACKET
   AudioStreamMgr_SessionCmdQUnSupportedHandler,  ///10 - UNSUPPORTED CMD
};


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/**
 * This function returns the ASM service version
 */
ADSPResult asm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_ASM_V;
  svc_info_ptr->api_version = ASM_API_VERSION;
  svc_info_ptr->api_branch_version = ASM_API_BRANCH_VERSION;

  return ADSP_EOK;
}

/* =======================================================================
**         AudioStreamMgr Instance Create/Destroy/Workloop
** ======================================================================= */

ADSPResult AudioStreamMgr_Create (uint32_t inputParam, void **handle)
{
   int result;
   qurt_elite_queue_t  *pQTemp;

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: Creating");

   memset(&gAsmStatSvcObject,0,sizeof(AudioStreamMgr_t));

   AudioStreamMgr_t *pMe = (AudioStreamMgr_t*)&gAsmStatSvcObject;


   // Create global cmd queue
   pQTemp = (qurt_elite_queue_t *)pMe->asmCmdQBuf;
   if (ADSP_FAILED(result = qurt_elite_queue_init(AUDIO_STREAM_MGR_SYS_CMD_Q_NAME,
                      AUDIO_STREAM_MGR_MAX_SYS_CMD_Q_ELEMENTS, pQTemp)))

   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create AudioStreamMgr service message queues!");
      AudioStreamMgr_Destroy(pMe);
      return result;
   }
   pMe->serviceHandle.cmdQ = pQTemp;

   //initialize event queue
   pQTemp = (qurt_elite_queue_t *)pMe->asmEvtQBuf;
   if (ADSP_FAILED(result = qurt_elite_queue_init(AUDIO_STREAM_MGR_SYS_EVT_Q_NAME,
                      AUDIO_STREAM_MGR_MAX_SYS_EVT_Q_ELEMENTS, pQTemp)))

   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize AudioStreamMgr event queue!");
      AudioStreamMgr_Destroy(pMe);
      return result;
   }

   // set up channel
   qurt_elite_channel_init(&pMe->channel);

   //add system command queue
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pMe->serviceHandle.cmdQ, unSysCmdQMask)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: fail to add mqs to channel result = %d!", result);
      return(result);
   }

   //add system event queue
   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->channel, pQTemp, unSysEvtQMask)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr: failed to add event queue to channel with result = %d!", result);
      return(result);
   }

    // populate me
   pMe->serviceHandle.unSvcId = ELITE_STATASM_SVCID;

   /// Create all session states here
   AudioStreamMgr_CreateAllSessionStates(pMe);

   /// Once session is created, initialize data base
   if ( ADSP_FAILED( result = AudioStreamMgr_AprRouterInit(pMe)) ) {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: fail to init router result = %d!", result);
      return(result);
   }

   // Launch the thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->serviceHandle.threadId), THREAD_NAME, NULL,
      THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_AUDIO_STREAM_SVC_PRIO_ID), AudioStreamMgr_WorkLoop, (void*)pMe,
      QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create AudioStreamMgr Thread!");
      AudioStreamMgr_Destroy(pMe);
      return result;
   }

   *handle = &(pMe->serviceHandle);
   // register with qurt_elite memory map.
   qurt_elite_memorymap_register(&asmMemoryMapClient);

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: Created");

   return AudioStreamMgr_RegisterPm(pMe);

}


/*Destroy yourself*/
void AudioStreamMgr_Destroy(AudioStreamMgr_t *pMe)
{
   QURT_ELITE_ASSERT( pMe );

   (void)AudioStreamMgr_DestroyAllSessionState(pMe);

   elite_svc_deinit_cmd_queue(pMe->serviceHandle.cmdQ);
   qurt_elite_channel_destroy(&pMe->channel);
   (void)AudioStreamMgr_UnRegisterPm(pMe);
}

/* This function is the main work loop for the service. Commands from SC
  are handled with the highest priority, followed by MatrixMixer,PP and Decoder
  service.
 */
static int AudioStreamMgr_WorkLoop(void* pInstance)
{
   AudioStreamMgr_t *pMe = (AudioStreamMgr_t*)pInstance;       // instance structure
   ADSPResult result=ADSP_EOK;

   // set up mask for listening to the msg queues.

   pMe->unCurrentBitfield |= unSysCmdQMask ;
   pMe->unCurrentBitfield |= unSysEvtQMask ;

   //always listen to the response queues of the sessions since the
   //dynamic services can raise MIPS change events which need to be
   //processed immediately.
   for(uint32_t i = 3; i <= 31; i += 2)
   {
      pMe->unCurrentBitfield |= (1 << i);
   }

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering ASM workloop...");

   // Enter forever loop
   for(;;)
   {
     // block on any one or more of selected queues to get a msg
     pMe->unChannelStatus = qurt_elite_channel_wait(&(pMe->channel), pMe->unCurrentBitfield);

     MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Signal received = %lx !", pMe->unChannelStatus);

     //exhaust all response queues first
     uint32_t resp_q_status = 0;
     while((resp_q_status = qurt_elite_channel_poll(&pMe->channel, 0xAAAAAAA8)))
     {
         result = AudioStreamMgr_ProcessChannels(pMe, resp_q_status);
         /* If Service is destroyed, return immediately */
         if(result == ADSP_ETERMINATED) return ADSP_EOK;
     }

     //process other queues and also any response queues now
     pMe->unChannelStatus = qurt_elite_channel_poll(&pMe->channel, pMe->unCurrentBitfield);

     result = AudioStreamMgr_ProcessChannels(pMe, pMe->unChannelStatus);
     /* If Service is destroyed, return immediately */
          if(result == ADSP_ETERMINATED) return ADSP_EOK;
   } // forever loop

   return result;
}

/* =======================================================================
**                          SystemCmdQ processing
** ======================================================================= */

/* Do not process Systerm Reserve Q. Just report error. */
static ADSPResult AudioStreamMgr_ProcessSysReserveQ(AudioStreamMgr_t *pMe, int32_t nChannelBit)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr: processing event queue");

   qurt_elite_queue_t *pEvtQue = (qurt_elite_queue_t *)pMe->asmEvtQBuf;
   elite_msg_any_t msg;

   ADSPResult result = qurt_elite_queue_pop_front(pEvtQue, (uint64_t*)&msg);

   if(ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr: failed to dequeue from event queue");
      return result;
   }

   //just finish the message without processing
   return elite_msg_finish_msg( &msg, ADSP_EOK);
}

static ADSPResult  AudioStreamMgr_ProcessSysCmdQ(AudioStreamMgr_t *pMe, int32_t nChannelBit)
{
   ADSPResult result;

   // Take next msg off the q
   /*uint64_t temp1 = (pMe->serviceHandle.cmdQ->data[0]);
   *((uint64_t*)&(pMe->msg)) = temp1;
   qurt_printf("###### %llu ######",  pMe->serviceHandle.cmdQ->data[0]);*/
   result = qurt_elite_queue_pop_front(pMe->serviceHandle.cmdQ, (uint64_t*)&(pMe->msg));
   if(ADSP_EOK != result)
   {
	   return result;
   }
   if ( pMe->msg.unOpCode >= AUDIO_STREAM_MGR_MAX_OP_CODE_SYS_CMD_Q) {
      AudioStreamMgr_LogErrMsg( &pMe->msg );
      elite_msg_finish_msg( &pMe->msg, ADSP_EUNSUPPORTED);
      return ADSP_EUNSUPPORTED;
   }
   return pSysCmdQMsgHandler[pMe->msg.unOpCode](pMe) ;
}


static ADSPResult  AudioStreamMgr_SysQUnSupportedHandler(AudioStreamMgr_t *pMe)
{
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr: unsupporte msg");
   AudioStreamMgr_LogErrMsg( & (pMe->msg) );
   elite_msg_finish_msg( & (pMe->msg), ADSP_EUNSUPPORTED);
   return ADSP_EFAILED;
}



/* Message Handler to ELITE_CMD_START_SERVICE in sys Q */
static ADSPResult  AudioStreamMgr_SysQStartSvcHandler(AudioStreamMgr_t *pMe)
{
   ADSPResult result;
   char admCmdQName[QURT_ELITE_DEFAULT_NAME_LEN] = "admStatServ";

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: StartSvc  Enter");

   /* Look for AFE queue */
   if( NULL == qurt_elite_globalstate.pAdmStatSvcCmdQ)
   {
     MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AudioStreamMgr: Cannot Attach - queue name 1st 4 bytes=%lx",
      (uint32_t)admCmdQName);
       elite_msg_finish_msg( & (pMe->msg), ADSP_EFAILED);
       return ADSP_EFAILED;
   }

   pMe->pDeviceSvcCmdQ = qurt_elite_globalstate.pAdmStatSvcCmdQ;
   pMe->pAFESvcCmdQ    = qurt_elite_globalstate.pAfeStatSvcCmdQ;

   /****************************************************************
    ** Register ISR call back function with APR
   ******************************************************************/

   elite_apr_handle_t handle = 0;
   char aAsmName[] = "qcom.adsp.asm";
   uint32_t ulAsmNameSize = strlen( aAsmName);
   uint16_t usRetAddr = 0;
   if (ADSP_FAILED( result = elite_apr_if_register_by_name( &handle, &usRetAddr, aAsmName, ulAsmNameSize, &AudioStreamMgr_AprCallBackFct,NULL)) ) {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: Fail to register ASM  0x%8x", result );
      elite_msg_finish_msg( & (pMe->msg), ADSP_EFAILED);
      return result;
   }

   if (ADSP_FAILED( result = AudioStreamMgr_SetAprHandle( handle) )) {
      MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: Fail to set handle 0x%8x", result);
   }


   elite_msg_finish_msg( & (pMe->msg), ADSP_EOK);

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: StartSvc  Leave");

   return result;

}

/* Message handler ELITE_CMD_DESTROY_SERVICE */
static ADSPResult AudioStreamMgr_SysQDestroySvcHandler(AudioStreamMgr_t *pMe)
{

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: DestroySvc  Enter");

   /**Make a copy of msg because on destroy, pMe is freed */
   elite_msg_any_t msg = pMe->msg;

   AudioStreamMgr_Destroy(pMe);

   elite_msg_finish_msg( &msg, ADSP_EOK );

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr: DestroySvc  Leave");
   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;
}

/* =======================================================================
**                          Session CmdQ and RespQ processing
** ======================================================================= */
static ADSPResult AudioStreamMgr_ProcessSessionCmdQ(AudioStreamMgr_t *pMe, int32_t nChannelBit)
{

   ADSPResult result;
   uint8_t intSessionId = (nChannelBit >> 1) - 1;
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[intSessionId];

   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pStSession->pSessionCmdQ, (uint64_t*)&(pStSession->cmdMsg));
   if(ADSP_EOK != result)
   {
	   return result;
   }
   const uint32_t sessionCmdTableSize = sizeof(AudioStreamMgr_SessionCmdQHandler)/sizeof(AudioStreamMgr_SessionCmdQHandler[0]);
   if (pStSession->cmdMsg.unOpCode >= sessionCmdTableSize) {
      // Basic rule, final consumer release the message.
      return AudioStreamMgr_SessionCmdQUnSupportedHandler(pMe, intSessionId);
   }

   return AudioStreamMgr_SessionCmdQHandler[pStSession->cmdMsg.unOpCode]( pMe, intSessionId );
}

static ADSPResult AudioStreamMgr_ProcessSessionRespQ(AudioStreamMgr_t *pMe, int32_t nChannelBit)
{
   ADSPResult result;
   uint8_t intSessionId = (nChannelBit >> 1) - 1 ;
   AudioStreamMgr_StaticSessionStateType *pStSession = &pMe->aStSessions[intSessionId];
   AudioStreamMgr_SessionStateType *pSession = (pMe->pSessions[intSessionId]);

   elite_msg_any_t msg;
   // Take next msg off the q
   result = qurt_elite_queue_pop_front(pStSession->pSessionRespQ, (uint64_t*)&(msg));

   //check if response is received on a session that's not created yet.
   if (NULL == pSession)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,-]: Received response/event on invalid session", intSessionId);
      if ( ADSP_FAILED( result = elite_msg_release_msg(&msg)) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: Fail to finish msg: 0x%lx",
               intSessionId,   (uint32_t) result);
      }
      return ADSP_EFAILED;
   }

   pSession->msg = msg;

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AudioStreamMgr [%u,-]: Received response/event opcode 0x%lx", pSession->ucInternalSessionId, pSession->msg.unOpCode);

   const uint32_t sessionRespTableSize = sizeof(AudioStreamMgr_SessionRespQHandler)/sizeof(AudioStreamMgr_SessionRespQHandler[0]);
   if (pSession->msg.unOpCode >= sessionRespTableSize) {
      // Basic rule, final consumer release the message.
      return AudioStreamMgr_SessionRespQUnSupportedHandler(pMe, pSession);
   }

   return AudioStreamMgr_SessionRespQHandler[pSession->msg.unOpCode]( pMe, pSession );

}

static ADSPResult AudioStreamMgr_ProcessChannels(AudioStreamMgr_t *pMe, uint32_t channel_status)
{
   int32_t nEndBit, nStartBit, nMask;
   ADSPResult result=ADSP_EOK;

   if(!channel_status)
   {
      return ADSP_EOK;
   }

   // Process each ready channel once
   nEndBit   = AUDIO_STREAM_MGR_MAX_NUM_CHANNEL_BITS - s32_cl0_s32(channel_status);
   nStartBit = s32_get_lsb_s32(channel_status);
   nMask = 0x1 << nStartBit;

   for ( int32_t i = nStartBit; i < nEndBit; i++ )
   {
      if ( nMask & channel_status )
      {
         result = pQHandler[i](pMe,i);
      }

      nMask <<= 1;

      /*Service is destroyed, return immediately */
      if(result == ADSP_ETERMINATED) break;
   } // LOOP over all channels.

   return result;
}


