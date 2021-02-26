/*==========================================================================
ELite Source File

This file implement Audio Post Processing Dynamic Service logic.

Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*===========================================================================
Edit History

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/12/09   JG      Created file.
01/13/10   JG      Modify default PP output frame size to be 480.
Increase AudPP stack size.
03/14/10   JG      FADD compliant change
03/29/10   JG      Move AudPP to inActive State for Disconnect command.
04/14/10   JG      Coordinate with ASM change that for PCM playback, EOS will
be sent, but there is possibility that no new MEDIA format
command sent for the next file in playlist.
04/23/10   PC      Introducing multiband IIR tuning filter
04/26/10   PC      Checkin on behalf of Seshu for MMPM initial integration.
04/29/10   JG      For error condition check in processing func, allow empty
buffer as input (remove assertion failure for empty buffer
case).
05/05/10   PC      Introducing Equalizer to AudPP
Also split out Audpp_Proc related function.
05/07/10   JG      Introducing Generic Resampler into AudPP
05/07/10   PC      Bug fix for resampler initialize function and tested resampler on
target
05/10/10   JG      Re-architecture on the code to be ISOD Compliant.
05/27/10   DG      Got the code building and the data path working for buffers
from upstream service. The code for handling data packets from
Scorpion is commented out
06/07/10   PC      Merging dev/apr to mainline                 .
06/07/10   PC      Introducing QCCP and Reverb
06/07/10   PC      Introducing Soft Volume
06/07/10   DG      Added support for pause/resume/flush
06/07/10   DG      Added support for getting data buffers from Scorpion
06/14/10   DG      Added support for handling media type command in flush
06/16/10   DG      Added support for COPP and POPreP topologies
06/16/10   DG      Added support for not processing data during disconnect.
06/18/10   PC      Introducing PBE
06/18/10   AP      Adding MBDRC Feature
06/24/10   PC      Bug fix for volume control and added basic Set Param
functionality
06/24/10   PC      Added COPP feature
06/25/10   AP      Added COPreP feature
06/26/10   AP      Added MBDRC Tx support
07/08/10   PC      Added DownMix 5.1 to stereo and the PP test fw for the same
07/08/10   DG      Separated the topologies into different modules.
07/30/10   PC      Added Spectrum Analyzer to PP code
08/24/10   PC      Added Time Scale Modification to PP code
09/02/10   PC      Added Qensemble feature to PP code
09/09/10   SS      Adding data tapping support (Sim) for Postprocessing
=========================================================================== */
/*---------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
//system
#include <math.h>

//Audio
#include "AudDynaPPSvc.h"
#include "audproc_comdef.h"
#include "EliteTopology_db_if.h"
#include "EliteCmnTopology_db_if.h"
#include "AFEInterface.h"

#ifndef MULTI_PORT_DEBUG
//#define MULTI_PORT_DEBUG
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define AUDDYNAPPSVC_DATA_Q_NAME       "DPSd00"
#define AUDDYNAPPSVC_CMD_Q_NAME        "DPSc00"

static const uint32_t SVC_LAYER_STACK_SIZE = 1024;

/**
 * bandwidth required for single stream decode
 * values are so due to legacy implementation.
 *
 * assuming 48kHz sampling, 2 channels, 2 bytes per channel.
 * in bytes per second.
 */
#define SINGLE_STREAM_DECODED_BW  (48000 * 2 * 2)

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static const elite_qxdm_log_code audpp_qxdm_log_code_tbl_in[] =
{
      QXDM_LOG_CODE_AUD_POPP_IN,
      QXDM_LOG_CODE_AUD_COPP_IN,
      QXDM_LOG_CODE_AUD_POPREP_IN,
      QXDM_LOG_CODE_AUD_COPREP_IN
};

static const elite_qxdm_log_code audpp_qxdm_log_code_tbl_out[] =
{
   QXDM_LOG_CODE_AUD_POPP_OUT,
   QXDM_LOG_CODE_AUD_COPP_OUT
};

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------
 * Function prototypes
 * ------------------------------------------------------------------------- */

// Initialization functions
static void AudPP_InitializeStatus(AudPPStatus_t *pPPStat);
static ADSPResult AudPP_SetSampleslipDirection(ThisAudDynaPPSvc_t* me);
static ADSPResult AudPP_CreateSvcQueues(ThisAudDynaPPSvc_t* me, const int count);
static void AudPP_SetConfigInfo(ThisAudDynaPPSvc_t* me, const AudPPSvcInitParams_t *pInitParams);
static void AudPP_setObjId(ThisAudDynaPPSvc_t* me, int instanceId, AudPP_AudProcType type);
static ADSPResult AudPP_InitializeLegacyTopology(ThisAudDynaPPSvc_t* me, uint32_t* stack_size_ptr);
static ADSPResult AudPP_InitializeCommonTopology(ThisAudDynaPPSvc_t* me, uint32_t* stack_size_ptr);
static void AudPP_InitializeInputPort(AudPPInputPort_t *port_ptr);

// destructor and cleanup functions
static void AudPP_DestroySvc (ThisAudDynaPPSvc_t* me);
static void AudPP_DestroyPPFeatures(ThisAudDynaPPSvc_t* me);
static void AudPP_DestroySvcQueues(ThisAudDynaPPSvc_t* me);
static void AudPP_DestroyPortsAndObject(ThisAudDynaPPSvc_t* me);

// Main work loop for service thread.
// Handle overall data & cmd processing for AudPP service.
static int AudPP_WorkLoop(void* pInstance);

// message handler functions
static int AudPP_Custom(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_DestroyYourself(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_Run(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_Flush(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_PausePrepare(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_SetParam(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_GetParam(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_GetTopoModuleList(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_GetNumOutputBuffers(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_ConnectToService(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_DisconnectFromService(void* pInstance, elite_msg_any_t* pMsg);
static int AudPP_ProcessRegisterDeregister_PPEvents(void* pInstance, elite_msg_any_t* pMsg);

// Function for logging the input PCM data to PP service
static void AudPP_SetQxdmLogCode(ThisAudDynaPPSvc_t* me);

// Other static helper functions
static bool_t AudPP_isSoftPauseMsgHeld(ThisAudDynaPPSvc_t* me);
static ADSPResult AudPP_HandleBackGroundThread_Cb(void *context_ptr,topo_bg_cb_info_t cb_info);
static void AudPP_InitializeBgThCbInfo(ThisAudDynaPPSvc_t *me);
static void AudPP_DataProc_ProcessPendingPackets(ThisAudDynaPPSvc_t *me, uint32_t port_id);
static ADSPResult AudPP_InitSignals(ThisAudDynaPPSvc_t* me);

static ADSPResult AudPP_streampath_register_pp_events(asm_stream_cmd_register_pp_events_payload_t *registerEvent, ThisAudDynaPPSvc_t *pMe, uint32_t numevents, audproc_event_query_payload *receivedPayload );
static ADSPResult AudPP_devicepath_register_pp_events(adm_register_pp_events_payload_t *registerEvent, ThisAudDynaPPSvc_t *pMe, uint32_t numevents, audproc_event_query_payload *receivedPayload );
static void AudPP_event_payload_copy(audproc_event_query_payload *payload,void *pData, AudPP_AudProcType dynaPPSvcType);


/* -----------------------------------------------------------------------
 ** Message handler functions
 ** ----------------------------------------------------------------------- */
// Build the function pointer table for message handling. Some common handlers
// like ReturnSuccess() which ignores the command and returns success, and
// Unsupported() which returns an error and outputs an error message are
// helpful for multiple functions that this service does not support.

static elite_svc_msg_handler_func pHandler[] =
{
      AudPP_Custom,        //0 -
      elite_svc_unsupported,        //1 -
      AudPP_DestroyYourself,       //2 - ELITE_CMD_DESTROY_SERVICE
      AudPP_ConnectToService,      //3 - ELITE_CMD_CONNECT
      AudPP_DisconnectFromService, //4 - ELITE_CMD_DISCONNECT
      elite_svc_unsupported,        //5 - ELITE_CMD_PAUSE
      AudPP_Run,                   //6 - ELITE_CMD_RUN
      AudPP_Flush,                 //7 - ELITE_CMD_FLUSH
      AudPP_SetParam,              //8 - ELITE_CMD_SET_PARAM
      AudPP_GetParam,              //9 - ELITE_CMD_GET_PARAM
      elite_svc_unsupported,        //10 - 13 are all data cmd
      elite_svc_unsupported,        //10 - 13 are all data cmd
      elite_svc_unsupported,        //10 - 13 are all data cmd
      elite_svc_unsupported,        //10 - 13 are all data cmd
      elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
      elite_svc_unsupported,          //15 - ELITE_APR_PACKET
      elite_svc_unsupported,          //16 - ELITE_MIPS_CHANGE_EVENT
      elite_svc_unsupported,          //17 - ELITE_DATA_SET_PARAM
      elite_svc_unsupported,          //18 - DML change event
      elite_svc_unsupported,          //19 - ELITE_BW_CHANGE_EVENT
      elite_svc_unsupported,          //20 - ELITE_DATA_MARK_BUFFER
      elite_svc_unsupported,          //21 - ELITE_DATA_BUFFER_V2
      elite_svc_unsupported,          //22 - ELITE_CUSTOM_EVENT
      AudPP_ProcessRegisterDeregister_PPEvents,          //23 - ELITE_CMD_REGISTER_APR_EVENT 0x17
};

//============================================================================
//public constructor

/**
 * Constructor for the PP
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in] pInitParams
 *   Pointer to the parameter structure used to initialize the
 *   PP.
 * @param[out] handle
 *   The service handle for PP, which can be used to send
 *   messages to it.
 *
 * @return ADSPResult
 */
ADSPResult AudPP_CreateSvc (const AudPPSvcInitParams_t *pInitParams, void **handle, void **far_handle)
{
   int count, result;
   ThisAudDynaPPSvc_t *me;
   QURT_ELITE_HEAP_ID heap_id;
   uint32_t stack_size = 0;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: Creating");
   // allocate instance struct
   if (DYNA_SVC_PP_TYPE_POPP == pInitParams->type)
   {
      me = (ThisAudDynaPPSvc_t*) qurt_elite_memory_malloc( sizeof(ThisAudDynaPPSvc_t), PP_SVC_POPP_OBJ_INSTANCE);
   }
   else if (DYNA_SVC_PP_TYPE_COPP == pInitParams->type)
   {
      me = (ThisAudDynaPPSvc_t*) qurt_elite_memory_malloc( sizeof(ThisAudDynaPPSvc_t), PP_SVC_COPP_OBJ_INSTANCE);
   }
   else
   {
      me = (ThisAudDynaPPSvc_t*) qurt_elite_memory_malloc( sizeof(ThisAudDynaPPSvc_t), QURT_ELITE_HEAP_DEFAULT);
   }

   if (NULL == me)
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "audproc_svc: Failed to allocate memory for the PP object.");
      return ADSP_ENOMEMORY;
   }

#ifdef DBG_BUFFER_ADDRESSES
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR PP service object instance start: 0x%8x, size %d", me, sizeof(ThisAudDynaPPSvc_t));
#endif /* DBG_BUFFER_ADDRESSES */
   // zero out all the fields.
   memset(me, 0, sizeof(ThisAudDynaPPSvc_t));

   // allocate near port structure
   me->in_port[AUDPP_PRIMARY_IN_PORT] = (AudPPInputPort_t*) qurt_elite_memory_malloc( sizeof(AudPPInputPort_t), QURT_ELITE_HEAP_DEFAULT);
   // allocate far port structure if this is an echo canceller session
   me->numActiveInPorts = 1;
   me->numActiveOutPorts = 1;
   if (pInitParams->is_echo_canceller_session == TRUE)
   {
      me->in_port[1] = (AudPPInputPort_t*) qurt_elite_memory_malloc( sizeof(AudPPInputPort_t), QURT_ELITE_HEAP_DEFAULT);
   }

   if (NULL == me->in_port[AUDPP_PRIMARY_IN_PORT] || ((pInitParams->is_echo_canceller_session == TRUE) && (NULL == me->in_port[1])))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "audproc_svc: Failed to allocate memory for the PP handle/s");
      AudPP_DestroyPortsAndObject(me);
      return ADSP_ENOMEMORY;
   }

   // Initialize default input port
   AudPP_InitializeInputPort(me->in_port[AUDPP_PRIMARY_IN_PORT]);

   if(me->in_port[1] != NULL)
   {
      // Increment active in ports and initialize this port
      me->numActiveInPorts++;
      AudPP_InitializeInputPort(me->in_port[1]);
   }

   // static variable counter to make queue and thread name strings unique.
   count = qurt_elite_atomic_increment(&qurt_elite_globalstate.nMsgQs) & 0x000FFFFFL;

   // First initialize everything that doesn't involve memory allocation.
   me->softPauseExpireSignalInitialized = FALSE;

   AudPP_setObjId(me, count, pInitParams->type);
   qurt_elite_channel_init(&me->channel);
   me->memoryMapClient = pInitParams->memoryMapClient;
   me->topologyBandwidth = 0;
   // Make a copy of ASM call back structure
   if(pInitParams->pCbData != NULL)
   {
      me->CbData = *((AudioStreamMgr_CallBackHandleType *)pInitParams->pCbData);
      //memcpy( &me->CbData, pInitParams->pCbData, sizeof(AudioStreamMgr_CallBackHandleType));
   }
   me->topo_event_client.vtbl = &topo_event_vtable;
   me->is_processing_set_media_type = FALSE;
   me->is_processing_data = FALSE;

   me->ulDataLogId = pInitParams->ulDataLogId;

   audpp_buf_delay_mgr_init(&me->bufDelayMgr, pInitParams->type, pInitParams->pBufDelay);

   if (NULL != pInitParams->pAlgorithmicDelay)
   {
      *pInitParams->pAlgorithmicDelay = 0;
   }
   me->pAlgorithmicDelay = pInitParams->pAlgorithmicDelay;
   me->event_root_node = NULL;

   AudPP_SetConfigInfo(me, pInitParams);
   AudPP_InitializeStatus(&me->audPPStatus);


   result = AudPP_CreateSvcQueues(me, count);
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to create message queues!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   result = AudPP_InitSignals(me);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to initialize signals with result: %d!", me->objId, result);
      AudPP_DestroySvc(me);
      return result;
   }

   for(uint32_t port_id=0;port_id < me->numActiveInPorts;port_id++)
   {
      me->in_port[port_id]->elite_data_msg_cache.pCachedInDataMsg = NULL;
      result = AudPP_InitFormatConv(&me->in_port[port_id]->formatConverter, me->objId);
      if (ADSP_FAILED(result))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc in_port %x: Failed to initialize format converter!", me->objId,port_id);
         AudPP_DestroySvc(me);
         return result;
      }
      me->in_port[port_id]->formatConverterInitialized = TRUE;
   }

   me->is_input_output_realtime = pInitParams->is_input_output_realtime;

   //Currently, it is expected that PP svc honor both the traditional ASM/ADM add topology command as well as the new AVCS register topology command.
   //These are maintained as separate databases. This is intentional as this will help in easier removal of the traditional ASM/ADM add topology command
   //when the decision is made to do so in the future.
   result = AudPP_InitializeLegacyTopology(me, &stack_size);
   if (ADSP_FAILED(result))
   {
      result = AudPP_InitializeCommonTopology(me, &stack_size);
      if (ADSP_SUCCEEDED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX AudPP_InitializeCommonTopology success", me->objId);
      }
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX AudPP_InitializeLegacyTopology success", me->objId);
   }

   if(ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to initialize Topology!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   AudPP_InitializeBgThCbInfo(me);
   result |= AudPP_SetSampleslipDirection(me);
   result |= AudPP_InitializePPFeatures(me);
   if (result != ADSP_EOK )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to initialize features!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   result = AudPP_setOutputMediaInfo(me, &(pInitParams->outputMediaTypeInitParams));

   stack_size += SVC_LAYER_STACK_SIZE;

   // determine heap id of thread stack
   switch (pInitParams->type)
   {
   case DYNA_SVC_PP_TYPE_POPP:
      heap_id = PP_SVC_POPP_THRD_STACK;
      break;
   case DYNA_SVC_PP_TYPE_COPP:
      heap_id = PP_SVC_COPP_THRD_STACK;
      break;
   case DYNA_SVC_PP_TYPE_POPREP:
   case DYNA_SVC_PP_TYPE_COPREP:
      heap_id = QURT_ELITE_HEAP_DEFAULT;
      break;
   default:
      heap_id = QURT_ELITE_HEAP_DEFAULT;
      break;
   }

   AudPP_SetQxdmLogCode(me);

   // Low latency PP sessions will have higher priority than legacy sessions.
   int32_t nPriority;
   nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_PP_SVC_PRIO_ID);
   if ( ((DYNA_SVC_PP_TYPE_POPP == pInitParams->type) || (DYNA_SVC_PP_TYPE_POPREP == pInitParams->type))
         && ((ASM_LOW_LATENCY_STREAM_SESSION == pInitParams->perf_mode) || (ASM_ULTRA_LOW_LATENCY_STREAM_SESSION == pInitParams->perf_mode)) )
   {
      nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_LL_PP_SVC_PRIO_ID);
   }
   if ( ((DYNA_SVC_PP_TYPE_COPP == pInitParams->type) || (DYNA_SVC_PP_TYPE_COPREP == pInitParams->type))
         && ((ADM_LOW_LATENCY_DEVICE_SESSION == pInitParams->perf_mode) || (ADM_ULTRA_LOW_LATENCY_DEVICE_SESSION == pInitParams->perf_mode)) )
   {
      nPriority = elite_get_thrd_prio(ELITETHREAD_DYNA_LL_PP_SVC_PRIO_ID);
   }

   // Launch the thread.
   char threadName[5]; // 4 spaces for the unique id and one for the NULL terminator.
   snprintf(threadName, 5, "%hX", me->objId);
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.threadId),
         threadName, NULL, stack_size,
         nPriority, AudPP_WorkLoop,
         (void*)me, heap_id)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to launch audproc_svc thread!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   // Return values
   memscpy(&me->serviceHandle,sizeof(elite_svc_handle_t),&(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle),sizeof(elite_svc_handle_t));
   //Copy of handle exposed outside for the sake of debugging.
   *handle = &(me->serviceHandle);

   /*
    * numActiveInPorts is incremented based on is_echo_cancellation_session set by ADM if EP2 is valid.
    * But HLOS can set EP2 always even if it is not EC session. Return the far handle only if EC is present.
    * EC present is known only when topology is present. Ideally we should set numActiveInPorts to 1, 
    * but that will require seperate handling in destroy because the memory for FAR port is allocate 
    * at the top before topo is create.
    */
   *far_handle = NULL;
   if(me->numActiveInPorts == 2)
   {
      if(topo_is_ec_present(me->pTopologyObj))
      {
         *far_handle = &(me->in_port[1]->handle );
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "p%hx audproc_svc: numActiveInPorts is 2 but EC is not present in topology. Returning NULL for far handle!", me->objId);
      }
   }
   *pInitParams->pMsgCb = AudPP_AprMsgCallback;
   *pInitParams->pCtxt = (void*)(me);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Created ThreadId%d ",me->objId, me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.threadId);

   return ADSP_EOK;
}


static ADSPResult AudPP_InitSignals(ThisAudDynaPPSvc_t* me)
{
   ADSPResult result = ADSP_EOK;

   //init signal
   result = qurt_elite_signal_init(&(me->softPauseExpireSignal));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to initialize soft pause signal!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }
   me->softPauseExpireSignalInitialized = TRUE;

   //add timer signal after adding all queues
   if (ADSP_FAILED(result=qurt_elite_channel_add_signal(&(me->channel), &(me->softPauseExpireSignal),0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to add softPause signal to channel: %d!", me->objId, result);
      AudPP_DestroySvc(me);
      return result;
   }

   result = qurt_elite_timer_create(&(me->softPauseTimer),
         QURT_ELITE_TIMER_ONESHOT_DURATION, QURT_ELITE_TIMER_USER, &(me->softPauseExpireSignal));
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to create softPause timer: %d!", me->objId, result);
      AudPP_DestroySvc(me);
      return result;
   }

   // signal to enable CMD Q data mask
   result = qurt_elite_signal_init(&(me->BGThenableCmdQSignal));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to initialize enable cmd Q signal!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   //add signal to enable cmd Q data mask to channel
   if (ADSP_FAILED(result=qurt_elite_channel_add_signal(&(me->channel), &(me->BGThenableCmdQSignal),0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to add enable cmd Q signal to channel: %d!", me->objId, result);
      AudPP_DestroySvc(me);
      return result;
   }

   // signal to enable DATA Q data mask
   result = qurt_elite_signal_init(&(me->BGThenableDataQSignal));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Failed to initialize enable cmd Q signal!", me->objId);
      AudPP_DestroySvc(me);
      return result;
   }

   //add signal to enable data Q data mask to channel
   if (ADSP_FAILED(result=qurt_elite_channel_add_signal(&(me->channel), &(me->BGThenableDataQSignal),0)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to add enable cmd Q signal to channel: %d!", me->objId, result);
      AudPP_DestroySvc(me);
      return result;
   }

   return result;

}



// destructor
/*===========================================================================
  FUNCTION AudPP_DestroySvc()

  DESCRIPTION
  Destructor for Dynamic Post Proc service.

  PARAMETERS
Input:  me (this pointer to the AudDynaPPSvc instance)
nbufOutQ (# output buffer that's beung processed and not in buffer queue)
RETURN VALUE
None

DEPENDENCIES


SIDE EFFECTS

NOTE:
Destructor should only be called at proper state.
At that time, all processing is stopped, all input data being processed
should be released (return to upstream buffer Q, or release to Scorpion).
All output buffer being filled in should be released too (enqueued to
downstream data queue).
===========================================================================*/
/**
 * This function destory this AudDynaPPSvc instance.
 * @param[in]  me This points to the instance.
 * @param[in] nBufOutQ This indicates the number of
 *       output buffer that's being processed and not in buffer
 *       queue.
 *
 * @return ADSPResult
 */
void AudPP_DestroySvc (ThisAudDynaPPSvc_t* me)
{
   uint16_t instance_id = 0;
   if (me)
   {
      instance_id = me->objId;
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Destroy Svc Begin", me->objId);

      // There should not be any outstanding buffers at this point, but check anyway
      AudPP_CheckForOutstandingBuffers(me);

      if(topo_is_bg_thread_created(me->bg_th_handle))
      {
         topo_bg_thread_destroy((void **)&me->bg_th_handle);
         AudPP_Dealloc_DataMsg_Cache(me);
      }

      AudPP_DestroyPPFeatures(me);

      /** release, force_vote doesn't matter.*/
      (void)AudPP_ProcessKppsBw(me, TRUE, FALSE);
      (void)AudPP_DeregisterWithAdsppm(me);

      if(me->event_root_node != NULL)
      {
         AudPP_delete_all_nodes(&me->event_root_node);
      }
      for(uint32_t port_id=0;port_id < me->numActiveInPorts;port_id++)
      {
         if(me->in_port[port_id]->formatConverterInitialized)
            AudPP_DestroyFormatConv(&me->in_port[port_id]->formatConverter);
      }

      if (0!= me->softPauseTimer.qTimerObj )
      {
         ADSPResult result;
         result = qurt_elite_timer_delete(&(me->softPauseTimer));
         if (ADSP_FAILED(result))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Failed to delete timer: %d!", me->objId, result);
         }
      }

      if (me->softPauseExpireSignalInitialized)
      {
         qurt_elite_signal_deinit(&(me->softPauseExpireSignal));
      }
      qurt_elite_signal_deinit(&(me->BGThenableCmdQSignal));
      qurt_elite_signal_deinit(&(me->BGThenableDataQSignal));

      AudPP_DestroySvcQueues(me);

      qurt_elite_channel_destroy(&me->channel);

      AudPP_DestroyPortsAndObject(me);

      //qurt_elite_memory_free(me);
      //me = NULL;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Destroy Svc End", instance_id);
}

/*===========================================================================
  FUNCTION AudPP_WorkLoop()

  DESCRIPTION
  Main working loop for Dynamic Post Proc service.
  The thread listens for command and data queues simultaneously
  (commands always prioritized). Depending on current PP state,
  When a data cmd recieved, it is dequeued, then either being
  passed down (for non-buffer related data command) by enqueue
  to downstream's dataQ, or being processed (if it's data-buffer
  related cmd) when output buffer available.
  The output buffer can be in different size compared to input
  buffer, normally in smaller size that downstream service can
  handle easily.

  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance

RETURN VALUE
success or failure.

DEPENDENCIES


SIDE EFFECTS
===========================================================================*/
/**
 * This function is the main work loop for the service. Commands
 * are handled with the highest priority. Data processing is handled
 * only when command queue is empty.
 *
 * The thread listens for command and data queues simultaneously
 * (commands always prioritized). Depending on current PP state,
 * When a data cmd recieved, it is dequeued, then either being
 * passed down (for non-buffer related data command) by enqueue
 * to downstream's dataQ, or being processed (if it's data-buffer
 * related cmd) when output buffer available.
 * The output buffer can be in different size compared to input
 * buffer, normally in smaller size that downstream service can
 * handle easily.
 *
 * @param[in] pInstance This points to the instance structure.
 * @return success or failure.
 */
static int AudPP_WorkLoop(void* pInstance)
{
   int result;             // general result value
   elite_msg_any_t         myInDataMsg[2];

   // instance structure
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;
   AudPPStatus_t*         pPPStat = &me->audPPStatus;
   uint32_t dataBitField = 0;

   // ========================================================================
   // set up masks for listening to the msg queues.
   // ========================================================================
   pPPStat->unCommandOnlyBitField = qurt_elite_queue_get_channel_bit(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ);
   pPPStat->unBufBitfield = qurt_elite_queue_get_channel_bit(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ) | qurt_elite_queue_get_channel_bit(me->bufQ);
   pPPStat->unDataBitfield[0] = qurt_elite_queue_get_channel_bit(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ) | qurt_elite_queue_get_channel_bit(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.dataQ);
   dataBitField |= pPPStat->unDataBitfield[0];
   pPPStat->unCmdQSignalOnlyBitfield = qurt_elite_signal_get_channel_bit(&(me->BGThenableCmdQSignal));
   pPPStat->unDataQSignalOnlyBitfield = qurt_elite_signal_get_channel_bit(&(me->BGThenableDataQSignal));

   for(uint32_t port_id=1;port_id<me->numActiveInPorts;port_id++)
   {
      pPPStat->unDataBitfield[port_id] = qurt_elite_queue_get_channel_bit(me->in_port[port_id]->handle.cmdQ) | qurt_elite_queue_get_channel_bit(me->in_port[port_id]->handle.dataQ);
      dataBitField |= pPPStat->unDataBitfield[port_id];
   }

   pPPStat->unSoftPauseSignalOnlyBitfield = qurt_elite_signal_get_channel_bit(&(me->softPauseExpireSignal));
   MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Entering workloop with fields Buf=%8lx, Data=%8lx, Command=%8lx, pauseTimer=%8lx, data[0]=%8lx, data[1]=%8lx", me->objId, pPPStat->unBufBitfield, dataBitField, pPPStat->unCommandOnlyBitField, pPPStat->unSoftPauseSignalOnlyBitfield,pPPStat->unDataBitfield[0],pPPStat->unDataBitfield[1]);

   // Initially the service is disconnected, so go to sleep state
   AudPP_GotoSleepState(me);

   (void)AudPP_RegisterWithAdsppm(me);

   uint32_t unChannelStatus;
   // ========================================================================
   // Forever workloop
   // ========================================================================
   for (;;)
   {

      pPPStat->unCurrentBitfield |= (pPPStat->unCmdQSignalOnlyBitfield | pPPStat->unDataQSignalOnlyBitfield);

      if(topo_bg_is_bg_thread_processing(me->bg_th_handle))
      {
         // if bg thread is processing stop listening to cmd mask
         // it will be enabled once bg procesing is done, bg thread will invoke cb function and set cmd mask
         pPPStat->unCurrentBitfield &= (~pPPStat->unCommandOnlyBitField);
         topo_bg_set_cmdQ_pending_flag(me->bg_th_handle, TRUE, AUDPP_PRIMARY_IN_PORT);
      }

      // block on any one or more of selected queues to get a msg
      unChannelStatus = qurt_elite_channel_wait(&(me->channel), pPPStat->unCurrentBitfield);

      if ( unChannelStatus & (pPPStat->unCmdQSignalOnlyBitfield) )
      {
         pPPStat->unCurrentBitfield |= pPPStat->unCommandOnlyBitField;
         qurt_elite_signal_clear(&(me->BGThenableCmdQSignal));

         topo_bg_set_cmdQ_pending_flag(me->bg_th_handle, FALSE, AUDPP_PRIMARY_IN_PORT);

         // check again again enabling cmd mask, to read cmd msg in this iteration itself
         unChannelStatus = qurt_elite_channel_wait(&(me->channel), pPPStat->unCurrentBitfield);
      }

      if ( unChannelStatus & (pPPStat->unDataQSignalOnlyBitfield) )
      {
         for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
         {
            pPPStat->unCurrentBitfield |= pPPStat->unDataBitfield[port_id];
         }
         qurt_elite_signal_clear(&(me->BGThenableDataQSignal));
      }


      // =====================================================================
      // Workloop Control Command Handling
      // =====================================================================
      // Check if commands came in

      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Background thread is not processing currently, POP the cmd ", me->objId);
      if (unChannelStatus
            & qurt_elite_queue_get_channel_bit(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ))
      {
         // call common utility function to process command queue in standard way.
         result = elite_svc_process_cmd_queue(pInstance, &(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle), pHandler, SIZE_OF_ARRAY(pHandler));

         // If service has been destroyed, exit.
         if (ADSP_ETERMINATED == result)
         {
            return ADSP_EOK;
         }  // if ADSP_ETERMINATED == result
      }     // if (unChannelStatus & qurt_elite_queue_get_channel_bit(me->serviceHandle.cmdQ))

      //check if softpause signal is sent (timer expired)
      if ( unChannelStatus & (pPPStat->unSoftPauseSignalOnlyBitfield) )
      {
         AudPP_SoftPauseTimerExpireHandler(me);
      }


      // =====================================================================
      // Workloop Data Command Handling
      // =====================================================================

      // Data processing can occur only in active state
      if (!AudPP_isSleeping(me))
      {
         //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : Waiting for dataQ signals. ", me->objId);
         for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
         {
            if(me->in_port[port_id]->pInDataMsg == NULL)
            {
               if(topo_is_bg_thread_created(me->bg_th_handle)) // if bg thread is created
               {
                  AudPP_DataProc_ProcessPendingPackets(me, port_id);
               }

               //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: TODO : Entered dataQ loop. ", me->objId);
               if ( unChannelStatus & qurt_elite_queue_get_channel_bit(me->in_port[port_id]->handle.dataQ))
               {
                  result = qurt_elite_queue_pop_front(me->in_port[port_id]->handle.dataQ, (uint64_t*) &myInDataMsg[port_id]);
                  if (ADSP_FAILED(result))
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Can Not dequeue dataQ. ", me->objId);
                  }
                  else
                  {
#ifdef MULTI_PORT_DEBUG
                     if(me->numActiveInPorts > 1)
                        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : Process dataQ signals 0x%x. ", me->objId,pPPStat->unDataBitfield[port_id]);
#endif
                     if(topo_is_bg_thread_created(me->bg_th_handle)) // if bg thread is created
                     {
                        if(topo_bg_is_bg_thread_processing(me->bg_th_handle))	// if bg thread is running
                        {
#ifdef MULTI_PORT_DEBUG							  
                           MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: BG thread is active. ", me->objId);
#endif
                           AudPP_DataProc_ProcessNewPacket_BGth_active(me, &myInDataMsg[port_id], port_id);
                        }
                        else
                        {
                           //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: BG thread is not active. ", me->objId);
                           AudPP_DataProc_ProcessNewPacket(me, &myInDataMsg[port_id], port_id);
                        }

                     }
                     else
                     {
                        //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: BG thread is not created. ", me->objId);
                        AudPP_DataProc_ProcessNewPacket(me, &myInDataMsg[port_id], port_id);
                     }

                  }
               }

            }
            else
            {
#ifdef MULTI_PORT_DEBUG
               if(me->numActiveInPorts > 1)
                  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: TODO : Previously unconsumed dataQ 0x%x available. ", me->objId,pPPStat->unDataBitfield[port_id]);
#endif
               AudPP_DataProc_DataBufFormatProc(me);
            }
         }
         if (AudPP_isSoftPauseMsgHeld(me))
         {
            // Need to wait for the timer to expire, which will ack the soft
            // pause message.
            pPPStat->unCurrentBitfield |= (pPPStat->unSoftPauseSignalOnlyBitfield);
         }
      }
   } // forever loop (workloop)
   return result;
}


static void AudPP_DataProc_ProcessPendingPackets(ThisAudDynaPPSvc_t *me, uint32_t port_id)
{
   uint32_t idx;
   AudPPInputPort_t *pInPort = me->in_port[port_id];

   if((pInPort->elite_data_msg_cache.num_pending_data_msgs > 0) &&
         (!(topo_bg_is_bg_thread_processing(me->bg_th_handle))))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Applying cached data Q msgs. pending msgs = %lu", me->objId, pInPort->elite_data_msg_cache.num_pending_data_msgs);
      // apply all the cached data msgs and make pending msgs count as 0
      for(idx =0 ;idx < pInPort->elite_data_msg_cache.num_pending_data_msgs; idx++)
      {
         AudPP_DataProc_ProcessNewPacket(me, &pInPort->elite_data_msg_cache.pCachedInDataMsg[idx], port_id);
         //pInPort->elite_data_msg_cache.pCachedInDataMsg[idx] = NULL;
      }
      pInPort->elite_data_msg_cache.num_pending_data_msgs = 0;
      topo_bg_set_dataQ_pending_flag(me->bg_th_handle, FALSE, port_id);
   } 
   return;
}

void AudPP_DataProc_ReleasePendingPackets(ThisAudDynaPPSvc_t *me)
{
   uint32_t idx;
   AudPPInputPort_t *pInPort;
   ADSPResult errCode;

   for(uint32_t port_id = 0; port_id < me->numActiveInPorts; port_id++)
   {
      pInPort = me->in_port[port_id];

     MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: releasing cached data Q msgs. pending msgs = %lu port id %lu", me->objId, pInPort->elite_data_msg_cache.num_pending_data_msgs, port_id);
     // apply all the cached data msgs and make pending msgs count as 0
     for(idx =0 ;idx < pInPort->elite_data_msg_cache.num_pending_data_msgs; idx++)
     {
        errCode = elite_msg_release_msg(&pInPort->elite_data_msg_cache.pCachedInDataMsg[idx]);
        if (ADSP_FAILED(errCode))
        {
           MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX AudPP_DataProc_ReleasePendingPackets in_port %x: Failed to release MediaTypeMsg!", me->objId,port_id);
        }
        //pInPort->elite_data_msg_cache.pCachedInDataMsg[idx] = NULL;
     }
     pInPort->elite_data_msg_cache.num_pending_data_msgs = 0;
   } 
   return;
}

/**
 * Initializes the status structure.
 *   - Dependencies: This function overwrites all the values in
 *     the structure, so any previous state will be lost.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[out] pPPStat
 * Pointer to the status structure.
 */
static void AudPP_InitializeStatus(AudPPStatus_t *pPPStat)
{
   pPPStat->dynaPPSvcState = DYNA_SVC_PP_STATE_PAUSE;

   pPPStat->isSleeping = TRUE;

   // flag indicating input data buffer from SC or upstream.
   // initialized to be INVALID data src for safety.

   pPPStat->outputBufInfo.pBuf = NULL;
   pPPStat->outputBufInfo.offsetInSamples = 0;
   pPPStat->outputBufInfo.sizeInSamples = 0;
   memset(&pPPStat->outputBufInfo.flags, 0 , sizeof(pPPStat->outputBufInfo.flags));
   pPPStat->outputBufInfo.timestamp = 0;

   pPPStat->pOutDataMsgDataBuf = NULL;

   pPPStat->unBufBitfield = 0;
   for(uint32_t i=0;i<AUDPP_MAX_INPUT_PORTS;i++)
      pPPStat->unDataBitfield[i] = 0;

   pPPStat->unCurrentBitfield = 0;
   pPPStat->m_fKeepProcessingonEos = FALSE;
#ifdef AUDPROC_SIM
   pPPStat->fdbg = fopen("ss_out.raw","wb");
   if (!pPPStat->fdbg)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SIM: unable to open debug output file");
   }
#endif // AUDPROC_SIM
}


/* =======================================================================
 **                          Message handler functions
 ** ======================================================================= */

/*===========================================================================
  FUNCTION AudPP_DestroyYourself()

  DESCRIPTION
  Message handler function for getting DESTROY_YOURSELF_CMD

  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance
pMsg to cmd/msg the service gets

RETURN VALUE
ADSP_ETERMINATED to indicate service get destroyed.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back.

NOTES:
Master service MUST do coordination before issuing the destroy command.
It has to make sure PP service has been disconnected from others (before that,
might need to make sure whether downstream service stops consuming input data
buffer and return buffers back to this specific service too.)
===========================================================================*/
static int AudPP_DestroyYourself (void* pInstance, elite_msg_any_t* pMsg)
{
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;       // instance structure
   ADSPResult result=ADSP_EOK;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Destroy youself Begin", me->objId);
#ifdef AUDPROC_SIM
   AudPPStatus_t* pPPStat = &me->audPPStatus;
   if (pPPStat->dbgptr)
   {
      qurt_elite_memory_free(pPPStat->dbgptr);
      pPPStat->dbgptr = NULL;
   }
   if (!pPPStat->fdbg)
   {
      fclose(pPPStat->fdbg);
      pPPStat->fdbg = NULL;
   }
#endif // AUDPROC_SIM

   AudPP_DestroySvc(me);

   // Ack back to sender
   elite_svc_send_ack(pMsg, result);

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: Destroy youself End, sent ack");
   // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
   return ADSP_ETERMINATED;

}


/*===========================================================================
  FUNCTION AudPP_Flush()

  DESCRIPTION
  Message handler function for getting FLUSH_CMD

  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance
pMsg to cmd/msg the service gets

RETURN VALUE
success/failure.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
Master service MUST do coordination before handling the flush command.
It has to wait until all services finishing their own flush process
before new data can be dumped in.
===========================================================================*/
/**
 * Handles the flush command. This function frees up the current input buffer
 * (if any), and also flushes all the packets from the data queue.
 *   - Dependencies: The service must be paused before this function is called.
 *   Also, the upstream service must not send buffers while we are flushing, since
 *   that can cause a race condition.
 *   - Side Effects: Resets the IIR filter, equalizer and resampler.
 *   - Re-entrant: Yes
 *
 * @param[in] pInstance
 * Pointer to the current instance structure.
 * @param[in] pMsg
 * Pointer to the Flush message.
 *
 * @return int
 * Returns an ADSP error code to indicate status.
 */
static int AudPP_Flush (void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Flush Begin", me->objId);

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }
   for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
   {
      AudPP_FreeCurrentInputBuffer(me,port_id);
   }

   AudPP_FreeCurrentOutputBuffer(me);

   // need to check all data commands in data Q, ack back to SC for EOS, and
   // release buffers if necessary.
   result = ADSP_EOK;
   for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
   {
      result |= AudPP_FlushDataQueue(me,port_id);
   }

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Data Queue flush failed", me->objId);
   }

   if (ADSP_SUCCEEDED(result))
   {
      topo_reset(me->pTopologyObj);
      result = 0;
      for(uint32_t port_id=0;port_id < me->numActiveInPorts;port_id++)
      {
         result |= AudPP_ResetFormatConv(&me->in_port[port_id]->formatConverter);
         if (ADSP_FAILED(result))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Failed to reset the input buffer format converter for flush.", me->objId);
         }
      }
   }

   //incr log-id after flush
   me->ulDataLogId++;

   // Ack back to sender
   elite_svc_send_ack(pMsg, result);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Flush End, sent ack", me->objId);
   return result;
}

/*===========================================================================
  FUNCTION AudPP_ConnectToService()

  DESCRIPTION
  Message handler function for getting CONNECT_TO_SVC_CMD

  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance
pMsg to cmd/msg the service gets

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
Postprocessing can be connected to different services based on the
chain type (POPP, COPP, and PDOPP).  From the beginning, PDOPP might
not be used separately, but related constant definition should be
defined for future extension.
===========================================================================*/
static int AudPP_ConnectToService(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;

   if (NULL == me
         || NULL == pMsg)
   {
      MSG_2( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Connect Audproc Service received bad pointers: 0x%p, 0x%p", me, pMsg);
      return ADSP_EFAILED;
   }

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_svc_handle_t *pPayload =
         (elite_svc_handle_t*)(((elite_msg_cmd_connect_t*)(pMsg->pPayload))->pSvcHandle);
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: instance 0x%8p connecting to SvcID 0x%8lx", me->objId,
         me, pPayload->unSvcId);

   // this example only allows 1 downstream peer
   if (NULL != me->pDownstreamPeer)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Connection FAILED, this service only allows 1 peer, already connected.", me->objId);
      result = ADSP_ENOTREADY;
   }
   else
   {
      // based on PP chain type, it can be connected to different services.
      // Additionally, each of them should be allowed to connect to its tester or itself.
      // service ID defined in Elite.h

      switch (me->ppCfgInfo.dynaPPSvcType)
      {
      case DYNA_SVC_PP_TYPE_POPP:
         if ((ELITE_POPP_SVCID != pPayload->unSvcId)
               && (ELITE_DYNAPPTST_SVCID != pPayload->unSvcId)
               && (ELITE_DYN_ENCODER_SVCID != pPayload->unSvcId)
               && (ELITE_MIXER_SVCID != pPayload->unSvcId)
               && (ELITE_DYNAPP_SVCID != pPayload->unSvcId))
         {
            // POPP can only be connected to itself, tester, mixer/splitter, or dynamic PP.
            // when there's no PDOPP, POPP could be connected to MIXER.
            // when there's PDOPP in place, do we need a separate ID
            // for splitter?
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: POPP Connection FAILED, cannot connect to this service type.", me->objId);
            result = ADSP_EBADPARAM;
         }
         else
         {
            //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: POPP Connection Succeeded.", me->objId);
            me->pDownstreamPeer = pPayload;
         }
         break;
      case DYNA_SVC_PP_TYPE_COPP:
         if ((ELITE_COPP_SVCID != pPayload->unSvcId)
               && (ELITE_DYNAPPTST_SVCID != pPayload->unSvcId)
               && (ELITE_MIXER_SVCID != pPayload->unSvcId))
         {
            // COPP can only be connected to itself, tester, and mixer.
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: COPP Connection FAILED, cannot connect to this service type.", me->objId);
            result = ADSP_EBADPARAM;
         }
         else
         {
            //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: COPP Connection Succeeded.", me->objId);
            me->pDownstreamPeer = pPayload;
         }
         break;
      case DYNA_SVC_PP_TYPE_POPREP:
         if ((ELITE_DYNAPPTST_SVCID != pPayload->unSvcId)
               && (ELITE_DYN_ENCODER_SVCID != pPayload->unSvcId))
         {
            // POPREP can only be connected to tester and encoder.
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: POPREP Connection FAILED, cannot connect to this service type.", me->objId);
            result = ADSP_EBADPARAM;
         }
         //else accept the connection
         else
         {
            //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: POPREP Connection Succeeded.", me->objId);
            me->pDownstreamPeer = pPayload;
         }
         break;
      case DYNA_SVC_PP_TYPE_COPREP:
         if ((ELITE_DYNAPPTST_SVCID != pPayload->unSvcId)
               && (ELITE_MIXER_SVCID != pPayload->unSvcId))
         {
            // COPREP can be connected to tester and Audio Record Matrix Mixer.
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: COPreP Connection FAILED, cannot connect to this service type.", me->objId);
            result = ADSP_EBADPARAM;
         }
         //else accept the connection
         else
         {
            //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: COPreP Connection Succeeded.", me->objId);
            me->pDownstreamPeer = pPayload;
         }
         break;
      case DYNA_SVC_PP_NUM_TYPES:
      default:
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: PP Connection FAILED, current service type is invalid", me->objId);
         result = ADSP_EBADPARAM;
      }
   }    // (NULL != me->pDownstreamPeer)

   // If the connection succeeded, we might need to move to active state
   if (!AudPP_isDisconnected(me))
   {
      if (!AudPP_isPaused(me))
      {
         AudPP_GotoActiveState(me);
      }
      if (me->in_port[AUDPP_PRIMARY_IN_PORT]->mediaFormatReceived)
      {
         // on a new connection send the media type.
         // For ULLPP stream to ULLPP/ULL device connections the media type needs to be sent
         // downstream on a new device connection.
         AudPP_sendMediaTypeMsg(me);
      }
   }

   elite_svc_send_ack(pMsg, result);

   return result;
}


/*===========================================================================
  FUNCTION AudPP_DisconnectFromService()

  DESCRIPTION
  Message handler function for getting DISCONNECT_FROM_SVC_CMD

  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance
pMsg to cmd/msg the service gets

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
Caller is expected to only send this message during a good state!!!
This is key important.  Otherwise, there might be caveat somewhere.
===========================================================================*/
static int AudPP_DisconnectFromService(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;

   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;

   if (NULL == me
         || NULL == pMsg)
   {
      MSG_2( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Disconnect Audproc Service received bad pointers: 0x%p, 0x%p", me, pMsg);
      return ADSP_EFAILED;
   }

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_svc_handle_t *pHandle =
         (elite_svc_handle_t*)(((elite_msg_cmd_connect_t*)(pMsg->pPayload))->pSvcHandle);
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Instance 0x%8p disconnecting handle 0x%8p", me->objId,
         me, pHandle);

   // this service only allows 1 downstream peer
   if (pHandle != me->pDownstreamPeer)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Disconnection FAILED, not connected to that service.", me->objId);
      result = ADSP_EBADPARAM;
   }

   //else accept the disconnection
   else
   {
      AudPP_GotoSleepState(me);

      AudPP_FreeCurrentOutputBuffer(me);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Disconnection Succeeded.", me->objId);
      me->pDownstreamPeer = NULL;
   }

   elite_svc_send_ack(pMsg, result);

   return result;
}


/*===========================================================================
  FUNCTION AudPP_SetParam()

  DESCRIPTION
  Message handler function for getting SET_PARAM_CMD.
  This is the generic function for getting all feature sets' configurable
  parameters via command queue.
  Based on postprocessing chain type, different feature sets allowed.
  PARAMETERS
Input:  pInstance to the AudDynaPPSvc instance
pMsg to cmd/msg the service gets

RETURN VALUE
success/failure/status.

DEPENDENCIES

SIDE EFFECTS
ACK cmd back upon finish processing

NOTES:
===========================================================================*/
static int AudPP_SetParam(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;

   if (NULL == me
         || NULL == pMsg)
   {
      MSG_2( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Audproc Service Set param received bad pointers: 0x%p, 0x%p", me, pMsg);
      return ADSP_EFAILED;
   }

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *)pMsg->pPayload;

   //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Set ParamId 0x%lx, Client token=0x%lx", me->objId, pParamMsg->unParamId, pParamMsg->unClientToken);
   //If set-param comes without any instance id then set-param is done to a module-instance with instance-Id zero.
   topo_set_get_param_mode_t mode = (pParamMsg->unParamId == ELITEMSG_PARAM_ID_CAL)? ZERO_INSTANCE: GIVEN_INSTANCE;
   switch(pParamMsg->unParamId)
   {
   case ELITEMSG_PARAM_ID_CAL:
   case ELITEMSG_PARAM_ID_CAL_V2:
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Set Param Begin", me->objId);

      result = topo_set_param(me->pTopologyObj, pParamMsg->pnParamData, pParamMsg->unSize, mode);

      if (ADSP_ENOMEMORY == result)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Ran out of memory in set param.", me->objId);
         AudPP_GotoFatalState(me);
      }

      elite_svc_send_ack(pMsg, result);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Set Param End, sent ack", me->objId);
   }
   break;

   default:
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: The Parameters in SetParam are not PP parameters 0x%8lx", me->objId, pParamMsg->unParamId );
      result = ADSP_EBADPARAM;
      elite_svc_send_ack(pMsg, result);
   }
   break;
   }

   return result;
}


static int AudPP_GetTopoModuleList(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   EliteMsg_CustomCfg_GetPPTopologyModuleInfo *pParamMsg;

   if (NULL == me || NULL == pMsg)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "AudPP_GetTopoModuleList list received bad pointers: 0x%p, 0x%p", me, pMsg);
      result = ADSP_EFAILED;
      return result;
   }

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: AudPP_GetTopoModuleList PP is in fatal error state, cannot process commands.", me->objId);
      result = ADSP_EFAILED;
      elite_svc_send_ack(pMsg, result);
      return result;
   }

   pParamMsg = (EliteMsg_CustomCfg_GetPPTopologyModuleInfo *) pMsg->pPayload;

   result = topo_get_module_list(me->pTopologyObj, (uint32_t*)pParamMsg->pnParamData, pParamMsg->unSize, pParamMsg->unSecOpCode);

   if(ADSP_FAILED(result))
   {
      pParamMsg = NULL;
      elite_svc_send_ack(pMsg, result);
      return result;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX AudPP_GetTopoModuleList success", me->objId);
   elite_svc_send_ack(pMsg, result);
   return result;
}

static ADSPResult AudPP_streampath_register_pp_events(asm_stream_cmd_register_pp_events_payload_t *registerEvent, ThisAudDynaPPSvc_t *me, uint32_t numevents, audproc_event_query_payload *receivedPayload)
{
   ADSPResult result = ADSP_EOK;
   for(uint8_t i = 0; i < numevents; i++)
   {
      registerEvent = registerEvent + i;
      if ((0 == registerEvent->module_id) && (0 != registerEvent->instance_id))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid payload received for event 0x%lX ",
               registerEvent->event_id);
         continue;
      }
      AudPP_event_payload_copy(receivedPayload,registerEvent,me->ppCfgInfo.dynaPPSvcType );
      if((0 == registerEvent->module_id) && (0 == registerEvent->instance_id))
      {
         uint32_t num_modules = topo_module_get_num_modules(me->pTopologyObj);

         for(int32_t index = 0; index < num_modules; index++)
         {
            registerEvent->module_id = topo_module_get_module_id(me->pTopologyObj,index);
            registerEvent->instance_id = topo_module_get_instance_id(me->pTopologyObj,index);
            result = topo_module_register_events(me->pTopologyObj, registerEvent->event_id,\
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->config_mask);
            if(ADSP_ENOTEXIST == result)
            {
               MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Module ID 0x%lX is not present in the topology", me->objId,registerEvent->module_id);
            }
            if( ADSP_FAILED(result))
            {
               MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX Event ID - 0x%lX is not registered with module - 0x%lX due to %lu error", me->objId, registerEvent->event_id,
                     registerEvent->module_id, result);
               continue;
            }
            receivedPayload->moduleID = registerEvent->module_id;
            receivedPayload->instanceID = registerEvent->instance_id;
            if(receivedPayload->is_registered == 0)
            {
               if(AudPP_event_delete_node(receivedPayload, &me->event_root_node) != NODE_SUCCESS)
               {
                  result = ADSP_EFAILED;
               }
            }
            else
            {
               if(AudPP_event_insert_node(receivedPayload,&me->event_root_node)!= NODE_SUCCESS)
               {
                  MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Could not create a node to store client details for (event ID, moduleID,instanceID) = (0x%lX, 0x%lX, 0x%X)",me->objId, registerEvent->event_id,
                        registerEvent->module_id,registerEvent->instance_id);
                  result = ADSP_EFAILED;
               }
            }
         }
      }
      else
      {
         if(ADSP_EOK == topo_module_find_module_instance_presence(me->pTopologyObj,registerEvent->module_id,registerEvent->instance_id))
         {
            result = topo_module_register_events(me->pTopologyObj, registerEvent->event_id,\
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->config_mask);
            if(ADSP_ENOTEXIST == result)
            {
               MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Module ID 0x%lX is not present in the topology", me->objId,registerEvent->module_id);
            }
            if(ADSP_FAILED(result))
            {
               MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX Event ID - 0x%lX is not registered with module - 0x%lX due to %lu error",me->objId, registerEvent->event_id,
                     registerEvent->module_id, result);
            }
            else
            {
               if(receivedPayload->is_registered == 0)
               {
                  if(AudPP_event_delete_node(receivedPayload, &me->event_root_node) != NODE_SUCCESS)
                  {
                     result = ADSP_EFAILED;
                  }
               }
               else
               {
                  if(AudPP_event_insert_node(receivedPayload,&me->event_root_node)!= NODE_SUCCESS)
                  {
                     MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Could not create a node to store client details for (event ID, moduleID, instanceID) = (0x%lX, 0x%lX, 0x%X)",me->objId, registerEvent->event_id,
                           registerEvent->module_id,registerEvent->instance_id);
                     result = ADSP_EFAILED;
                  }
               }
            }
         }
         else
         {
            MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX:Module ID - 0x%lX with instance ID - 0x%lX is not present in the topology. So event - 0x%lX cannot be registered with it",me->objId,
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->event_id);
         }
      }
   }
   return result;
}

static ADSPResult AudPP_devicepath_register_pp_events(adm_register_pp_events_payload_t *registerEvent, ThisAudDynaPPSvc_t *me, uint32_t numevents, audproc_event_query_payload *receivedPayload)
{
   ADSPResult result = ADSP_EOK;
   for(uint8_t i = 0; i < numevents; i++)
   {
      registerEvent = registerEvent + i;
      if ((0 == registerEvent->module_id) && (0 != registerEvent->instance_id))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Invalid payload received for event 0x%lX ",
               registerEvent->event_id);
         continue;
      }
      AudPP_event_payload_copy(receivedPayload,registerEvent,me->ppCfgInfo.dynaPPSvcType );
      if((0 == registerEvent->module_id) && (0 == registerEvent->instance_id))
      {

         uint32_t num_modules = topo_module_get_num_modules(me->pTopologyObj);
         for(int32_t index = 0; index < num_modules; index++)
         {
            registerEvent->module_id = topo_module_get_module_id(me->pTopologyObj,index);
            registerEvent->instance_id = topo_module_get_instance_id(me->pTopologyObj,index);
            result = topo_module_register_events(me->pTopologyObj, registerEvent->event_id,\
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->config_mask);
            if(ADSP_ENOTEXIST == result)
            {
               MSG_2(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Module ID 0x%lX is not present in the topology", me->objId,registerEvent->module_id);
            }
            if( ADSP_FAILED(result))
            {
               MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX Event ID - 0x%lX is not registered with module - 0x%lX due to %lu error", me->objId, registerEvent->event_id,
                     registerEvent->module_id, result);
               continue;
            }
            receivedPayload->moduleID = registerEvent->module_id;
            receivedPayload->instanceID = registerEvent->instance_id;
            if(receivedPayload->is_registered == 0)
            {
               if(AudPP_event_delete_node(receivedPayload, &me->event_root_node) != NODE_SUCCESS)
               {
                  result = ADSP_EFAILED;
               }
            }
            else
            {
               if(AudPP_event_insert_node(receivedPayload,&me->event_root_node)!= NODE_SUCCESS)
               {
                  MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Could not create a node to store client details for (event ID, moduleID, instanceID) = (0x%lX, 0x%lX, 0x%X)",me->objId, registerEvent->event_id,
                        registerEvent->module_id,registerEvent->instance_id);
                  result = ADSP_EFAILED;
               }
            }
         }
      }
      else
      {
         if(ADSP_EOK == topo_module_find_module_instance_presence(me->pTopologyObj,registerEvent->module_id,registerEvent->instance_id))
         {
            result = topo_module_register_events(me->pTopologyObj, registerEvent->event_id,\
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->config_mask);
            if(ADSP_FAILED(result))
            {
               MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX Event ID - 0x%lX is not registered with module - 0x%lX due to %lu error",me->objId, registerEvent->event_id,
                     registerEvent->module_id, result);
            }
            else
            {
               if(receivedPayload->is_registered == 0)
               {
                  if(AudPP_event_delete_node(receivedPayload, &me->event_root_node) != NODE_SUCCESS)
                  {
                     result = ADSP_EFAILED;
                  }
               }
               else
               {
                  if(AudPP_event_insert_node(receivedPayload,&me->event_root_node)!= NODE_SUCCESS)
                  {
                     MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX: Could not create a node to store client details for (event ID, moduleID, instanceID) = (0x%lX, 0x%lX, 0x%X)",me->objId, registerEvent->event_id,
                           registerEvent->module_id,registerEvent->instance_id);
                     result = ADSP_EFAILED;
                  }
               }
            }
         }
         else
         {
            MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"P%hX:Module ID - 0x%lX with instance ID - 0x%lX is not present in the topology. So event - 0x%lX cannot be registered with it",me->objId,
                  registerEvent->module_id, registerEvent->instance_id, registerEvent->event_id);
         }
      }
   }
   return result;
}

static int AudPP_ProcessRegisterDeregister_PPEvents(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;

   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t* ) pInstance;
   uint32_t cmdVersion;
   uint32_t numEvents,payloadSize;
   uint8_t *pData;

   // Don't process command in fatal error state
   if (AudPP_isInFatalState(me))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
            "P%hX audproc_svc: PP is in FATAL error state, cannot process commands ",
            me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_msg_register_apr_event_t *pParamMsg =
         (elite_msg_register_apr_event_t *) pMsg->pPayload;

   audproc_event_query_payload receivedPayload;

   receivedPayload.src_port = pParamMsg->src_port;
   receivedPayload.src_addr = pParamMsg->src_addr;
   receivedPayload.dest_port = pParamMsg->dest_port;
   receivedPayload.dest_addr = pParamMsg->dest_addr;
   receivedPayload.apr_token = pParamMsg->apr_token;

   if((DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_POPREP == me->ppCfgInfo.dynaPPSvcType))
   {
      asm_stream_cmd_register_pp_events_t *pCmd = (asm_stream_cmd_register_pp_events_t *)pParamMsg->pnParamData;

      if(NULL == pCmd)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Received NULL pointer for the payload while processing Register Events command", me->objId);
         return ADSP_EBADPARAM;
      }
      if (pParamMsg->unSize < sizeof(asm_stream_cmd_register_pp_events_t))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Received incorrect payload size of %lu", me->objId, pParamMsg->unSize  );
         return ADSP_EBADPARAM;
      }

      uint8_t *payload_ptr = (uint8_t*)pCmd ;
      cmdVersion  = pCmd->version;
      numEvents   = pCmd->num_events;
      pData       = payload_ptr + sizeof(asm_stream_cmd_register_pp_events_t);
      payloadSize = pParamMsg->unSize - sizeof(asm_stream_cmd_register_pp_events_t);

      // check for payload size
      if(payloadSize <  (numEvents * sizeof(asm_stream_cmd_register_pp_events_payload_t )))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Incorrect payloadSize after number of events %lu", me->objId, payloadSize );
         return ADSP_EBADPARAM;
      }
      asm_stream_cmd_register_pp_events_payload_t *registerEvent = (asm_stream_cmd_register_pp_events_payload_t*)pData;
      result = AudPP_streampath_register_pp_events(registerEvent, me, numEvents,&receivedPayload);

   }
   else //if(DYNA_SVC_PP_TYPE_COPP == pInitParams->type ||  DYNA_SVC_PP_TYPE_COPREP == pInitParams->type)
   {

      adm_register_pp_events_t *pCmd = (adm_register_pp_events_t *)pParamMsg->pnParamData;
      if(NULL == pCmd)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Received NULL pointer for the payload while processing Register Events command", me->objId);
         return ADSP_EBADPARAM;
      }
      if (pParamMsg->unSize < sizeof(adm_register_pp_events_t))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Received incorrect payload size of %lu", me->objId, pParamMsg->unSize  );
         return ADSP_EBADPARAM;
      }
      uint8_t *payload_ptr = (uint8_t*)pCmd ;
      cmdVersion  = pCmd->version;
      numEvents   = pCmd->num_events;
      pData       = payload_ptr + sizeof(adm_register_pp_events_t);
      payloadSize = pParamMsg->unSize - sizeof(adm_register_pp_events_t);

      // check for payload size
      if(payloadSize <  (numEvents * sizeof(adm_register_pp_events_payload_t )))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " P%hX Incorrect payloadSize after number of events %lu", me->objId, payloadSize );
         return ADSP_EBADPARAM;
      }
      adm_register_pp_events_payload_t *registerEvent = (adm_register_pp_events_payload_t*)pData;
      result = AudPP_devicepath_register_pp_events(registerEvent, me, numEvents,&receivedPayload);
   }
   elite_svc_send_ack(pMsg, result);

   MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,
         "P%hX audproc_svc : AudPP_ProcessRegisterPPEvents sent ack ",
         me->objId);

   return ADSP_EOK;

}

static int AudPP_GetNumOutputBuffers(void* pInstance, elite_msg_any_t* pMsg)
{
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
            "P%hX audproc_svc: PP is in FATAL error state, cannot process commands.",
            me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   EliteMsg_CustomPPGetNumOutputBuffers *pParamMsg = (EliteMsg_CustomPPGetNumOutputBuffers *) pMsg->pPayload;

   pParamMsg->unNumBuffers = AudPP_NumOutputBuffersRequired(me);

   elite_svc_send_ack(pMsg, ADSP_EOK);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "P%hX audproc_svc: Get Num Output Buffers, sent ack",
         me->objId);

   return ADSP_EOK;
}



static int AudPP_GetParam(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*)pInstance;

   if (NULL == me
         || NULL == pMsg)
   {
      MSG_2( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"Audproc Service Get param received bad pointers: 0x%p, 0x%p", me, pMsg);
      return ADSP_EFAILED;
   }

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_msg_param_cal_t *pParamMsg = (elite_msg_param_cal_t *)pMsg->pPayload;

   //MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Get ParamId 0x%lx, Client token=0x%lx", me->objId, pParamMsg->unParamId, pParamMsg->unClientToken);

   uint32_t paramHeaderSize, moduleId, instanceId, paramId;
   if( (ELITEMSG_PARAM_ID_CAL != pParamMsg->unParamId) && (ELITEMSG_PARAM_ID_CAL_V2 != pParamMsg->unParamId))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: The Parameters in GetParam are not PP parameters 0x%8lx",
            me->objId, pParamMsg->unParamId );
      result = ADSP_EBADPARAM;
      elite_svc_send_ack(pMsg, result);
      return result;
   }
   else
   {
      paramHeaderSize = (ELITEMSG_PARAM_ID_CAL == pParamMsg->unParamId)?
            sizeof(asm_stream_param_data_v2_t): sizeof(asm_stream_param_data_v3_t);
      if ( pParamMsg->unSize < paramHeaderSize )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Insufficient payload size for get-param.", me->objId);
         result = ADSP_ENEEDMORE;
         elite_svc_send_ack(pMsg, result);
         return result;
      }
   }

   if( ELITEMSG_PARAM_ID_CAL == pParamMsg->unParamId)
   {
      asm_stream_param_data_v2_t *pHeader = (asm_stream_param_data_v2_t*)(pParamMsg->pnParamData);
      paramHeaderSize = sizeof(asm_stream_param_data_v2_t);
      moduleId = pHeader->module_id;
      //If Get-Param comes without any instance-Id then, Get param is queried from the module-instance with instance-Id zero.s
      instanceId = 0;
      paramId = pHeader->param_id;
   }
   else //ELITEMSG_PARAM_ID_CAL_V2
   {
      asm_stream_param_data_v3_t *pHeader = (asm_stream_param_data_v3_t*)(pParamMsg->pnParamData);
      paramHeaderSize = sizeof(asm_stream_param_data_v3_t);
      moduleId = pHeader->module_id;
      instanceId = pHeader->instance_id;
      paramId = pHeader->param_id;

   }

   uint8_t *pBuffer = (uint8_t*)(pParamMsg->pnParamData);

   result = topo_get_param(me->pTopologyObj,
         pBuffer + paramHeaderSize,
         pParamMsg->unSize - paramHeaderSize,
         moduleId,
         instanceId,
         paramId,
         &pParamMsg->unSize,
         GIVEN_INSTANCE);
   if ( ADSP_ENOMEMORY == result )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Ran out of memory in get param.", me->objId);
      AudPP_GotoFatalState(me);
   }

   elite_svc_send_ack(pMsg, result);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Get Param End, sent ack", me->objId);

   return result;
}

/**
 * This function handles the suspend command.
 * It closes the hw jobs if it is active to save power.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] pInstance
 * Pointer to the current instance structure
 * @param[in] pMsg
 * Pointer to the suspend command message.
 *
 * @return int
 * Returns an ADSP error code indicating status.

 Suspend for all the modules which can use HW modules.

 */

static int AudPP_Suspend(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   /* Do not process if Svc is in fatal state */
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   if (DYNA_SVC_PP_STATE_PAUSE == me->audPPStatus.dynaPPSvcState)
   {
      (void)  AudPP_SuspendPrepare(me);
      /** release, force_vote doesn't matter.*/
      result = AudPP_ProcessKppsBw(me, TRUE, FALSE);
   }
   elite_svc_send_ack(pMsg, result);
   return result;
}

/**
 * This function handles the pause command. It pushes out the
 * current output buffer if any, and then goes to sleep state.
 * In this state, it waits on the command queue only.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] pInstance
 * Pointer to the current instance structure
 * @param[in] pMsg
 * Pointer to the pause command message.
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
static int AudPP_PausePrepare(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause-prepare Handler  begin", me->objId);

   EliteMsg_CustomPPPausePrepareType *pCustomPauseMsg = (EliteMsg_CustomPPPausePrepareType *) (pMsg->pPayload);

   if (DYNA_SVC_PP_STATE_PAUSE == me->audPPStatus.dynaPPSvcState)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Received pause in paused state. Returning.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EOK);
      return ADSP_EOK;
   }

   me->audPPStatus.dynaPPSvcState = DYNA_SVC_PP_STATE_PAUSE;


   /* if its POPP, start timer for soft pause */
   if (DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType)
   {
      /* save pause message payload so that ack after soft pause ended */
      me->pPauseMsgPayload = pCustomPauseMsg;

      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: soft pause before start &me=0x%x, me=0x%x", &me, me);

      result = AudPP_SoftPause_Start(me);
      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "audproc_svc: soft pause finished start &me=0x%x, me=0x%x", &me, me);
   }

   if ((DYNA_SVC_PP_TYPE_POPP != me->ppCfgInfo.dynaPPSvcType) || ADSP_FAILED(result))
   {
      /* if we are here, either (1) this is not POPP or (2) POPP soft pause timer failed to start.
         Then we should do regular pause by entering sleep state immediately and ack pause because
         (1) Non-POPP do not have soft pause (2) POPP: there is no guarantee the required input
         samples will arive on time for volume ramp down */

      me->pPauseMsgPayload = NULL;

      if (NULL != me->audPPStatus.pOutDataMsgDataBuf)
      {
         /*
            We need to deliver the pending output buffer, so that if
            some parameter like volume changed in the pause state,
            we can give the correct output when we get run.
          */
         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause, Delivering out %8lx ", me->objId, (uint32) me->audPPStatus.pOutDataMsgDataBuf);
         AudPP_deliverCurrentOutputBuffer(me);
      }

      //see if we need to send down an EOS marker
      if(pCustomPauseMsg->sendDownEosMarker)
      {
         MSG_1( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX: AudProc received pause, generating internal eos ", me->objId);
         result = AudPP_GenerateEos(me);
         if (ADSP_FAILED(result))
         {
            MSG_1( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"P%hX audproc_svc: Unable to send EOS message. Aborting pause.", me->objId);
            elite_svc_send_ack(pMsg, result);
            return result;
         }
      }

      AudPP_GotoSleepState(me);

      elite_svc_send_ack(pMsg, ADSP_EOK);

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause-prepare Handler End (Non-POPP or POPP-Timer failure), sent ack", me->objId);
   }

   return ADSP_EOK;
}

static int AudPP_Pause2(void* pInstance, elite_msg_any_t* pMsg)
{
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause 2 Handler Begin", me->objId);


   elite_svc_send_ack(pMsg, ADSP_EOK);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Pause 2 Handler End (Non-POPP or POPP-Timer failure), sent ack", me->objId);

   return ADSP_EOK;
}

/**
 * This function handles the run command. It moves PP into the
 * active state, and adds the data or buffer queue to the
 * current channel bitfield as appropriate.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] pInstance
 * Pointer to the current instance structure
 * @param[in] pMsg
 * Pointer to the run command message.
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
static int AudPP_Run(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Run Begin", me->objId);

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   if (DYNA_SVC_PP_STATE_RUN == me->audPPStatus.dynaPPSvcState)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Received run in running state. Returning.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EOK);
      return ADSP_EOK;
   }

   me->audPPStatus.dynaPPSvcState = DYNA_SVC_PP_STATE_RUN;
   if (!AudPP_isDisconnected(me))
   {
      AudPP_GotoActiveState(me);
   }

   result = AudPP_RunPrepare(me);
   if( result != ADSP_EOK)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"P%hX audproc_svc: Run Prepare failed",me->objId);
      elite_svc_send_ack(pMsg, result);
      me->audPPStatus.dynaPPSvcState = DYNA_SVC_PP_STATE_PAUSE;
      AudPP_GotoSleepState(me);
      return result;
   }

   if (me->audPPStatus.rampOnResume)
   {
      //setup volume control lib to ramp up after resume
      topo_set_ramp_on_resume(me->pTopologyObj);
   }

   /** in pp svc, force voting is not necessary as we aggregate in below func.
    *  for stream-pp after suspend, prev would've become zero and below func will vote now.
    *  but for device-pp suspend won't be called, hence below would be no-op.*/
   result = AudPP_ProcessKppsBw(me, FALSE, FALSE);

   elite_svc_send_ack(pMsg, ADSP_EOK);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "P%hX audproc_svc: Run End, sent ack", me->objId);

   return ADSP_EOK;
}

/**
 * This function handles the custom messages. It calls the
 * appropriate handling function based on the secondary
 * opcode.
 *   - Dependencies: None
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] pInstance
 * Pointer to the current instance structure
 * @param[in] pMsg
 * Pointer to the custom command message.
 *
 * @return int
 * Returns an ADSP error code indicating status.
 */
static int AudPP_Custom(void* pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result;

   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t*) pInstance;

   // Don't process command in fatal error state.
   if (AudPP_isInFatalState(me))
   {
      MSG_1( MSG_SSID_QDSP6, DBG_FATAL_PRIO,"P%hX audproc_svc: PP is in FATAL error state, cannot process commands.", me->objId);
      elite_svc_send_ack(pMsg, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   elite_msg_custom_header_t *pHeader = (elite_msg_custom_header_t *)(pMsg->pPayload);

   switch (pHeader->unSecOpCode)
   {
   case ELITEMSG_CUSTOM_CFG_PP_OUTPUT:
   {
      result = AudPP_ProcessOutputFormatMsg(me, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_PP_PAUSE_PREPARE:
   {
      result = AudPP_PausePrepare(pInstance, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_PP_PAUSE:
   {
      result = AudPP_Pause2(pInstance, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_CFG_PP_OUTPUT_SAMPLE_RATE:
   {
      result = AudPP_ProcessSetOutputSamplingRate(me, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES:
   case ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES_V2:
   {
      result = AudPP_GetTopoModuleList(me, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS:
   {
      result = AudPP_GetNumOutputBuffers(me, pMsg);
      break;
   }
   case ELITEMSG_CUSTOM_PP_SUSPEND:
   {
      result = AudPP_Suspend(pInstance, pMsg);
      break;
   }

   default:
   {
      MSG_1( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"P%hX audproc_svc: PP got unsupported custom message.", me->objId);
      result = ADSP_EUNSUPPORTED;
      elite_svc_send_ack(pMsg, result);
      break;
   }
   }

   return result;
}


static ADSPResult AudPP_HandleBackGroundThread_Cb(void *context_ptr,topo_bg_cb_info_t cb_info)
{
   // nothing happens here as of now
   ThisAudDynaPPSvc_t *me = (ThisAudDynaPPSvc_t *)context_ptr;
   ADSPResult result = ADSP_EOK;

   switch(cb_info.event_id)
   {
   case TOPO_BG_EVENT_ENABLE_MAIN_TH_CMDQ:
   {
      //AudPPStatus_t *pMyStat = &me->audPPStatus;
      //pMyStat->unCurrentBitfield |= pMyStat->unCommandOnlyBitField;
      qurt_elite_signal_send(&(me->BGThenableCmdQSignal));
      break;
   }
   case TOPO_BG_EVENT_ENABLE_MAIN_TH_DATAQ:
   {
      qurt_elite_signal_send(&(me->BGThenableDataQSignal));
      break;
   }
   default:
   {
      MSG_2( MSG_SSID_QDSP6, DBG_ERROR_PRIO,"P%hX AudPP_HandleBackGroundThread_CB: unsupported event id %lx", me->objId, (uint32_t)cb_info.event_id);
      result = ADSP_EUNSUPPORTED;
      break;
   }
   }
   return result;
}


static void AudPP_InitializeBgThCbInfo(ThisAudDynaPPSvc_t *me)
{
   me->bg_th_cb_info.cb_func = AudPP_HandleBackGroundThread_Cb;
   me->bg_th_cb_info.context_ptr = (void *)me;

   return;
}

/**
 * Function to reinit the output sample size for all the modules
 * in the topology.
 *   - Dependencies: This function should be called only once.
 *   - Side Effects: None
 *   - Re-entrant: Yes
 *
 * @param[in, out] me
 * Pointer to the current instance structure.
 *
 */
void AudPP_ReInitCompressTopoWithOutputSampleSize(ThisAudDynaPPSvc_t* me)
{
   topo_compressed_reinit_with_output_sample_size(me->pTopologyObj, me->ppCfgInfo.dynaPPSvcOutSizeInSample);
}



static void AudPP_DestroyPPFeatures(ThisAudDynaPPSvc_t* me)
{
   if (NULL != me->pTopologyObj)
   {
      topo_destroy(me->pTopologyObj);
   }
   me->pTopologyObj = NULL;

}

static void AudPP_setObjId(ThisAudDynaPPSvc_t* me, int instanceId, AudPP_AudProcType type)
{
   /*
    * The unique id is in the following format:
    * From most significant byte to least significant byte,
    * Byte 1 ans 2: Unique letter representing the PP type
    * Bytes 3 and 4: Two hex digits representing the instance id
    */
   uint16_t truncatedId;
   truncatedId = instanceId & 0xFF;

   me->objId = truncatedId;

   switch (type)
   {
   case DYNA_SVC_PP_TYPE_COPP:
      me->objId |= 0xCC00;
      break;
   case DYNA_SVC_PP_TYPE_COPREP:
      me->objId |= 0xCD00;
      break;
   case DYNA_SVC_PP_TYPE_POPP:
      me->objId |= 0xAA00;
      break;
   case DYNA_SVC_PP_TYPE_POPREP:
      me->objId |= 0xAB00;
      break;
   case DYNA_SVC_PP_NUM_TYPES:
   default:
      me->objId |= 0xFF00;
      break;
   }
}

static void AudPP_SetConfigInfo(ThisAudDynaPPSvc_t* me, const AudPPSvcInitParams_t *pInitParams)
{
   me->ppCfgInfo.dynaPPSvcType = pInitParams->type;
   me->ppCfgInfo.dynaPPSvcTopologyID = pInitParams->topology;
   me->ppCfgInfo.initializeWithCompressedFormat = pInitParams->initializeWithCompressedFormat;
   me->ppCfgInfo.perf_mode = pInitParams->perf_mode;

   uint16_t unSize;
   uint32_t unFreq = 48000;
   uint32_t unAfeFrameSizeInSamples;


   if (!(pInitParams->outputMediaTypeInitParams.keepInputSamplingRate))
   {
      unFreq = pInitParams->outputMediaTypeInitParams.outputSamplingRate;
   }

   elite_svc_get_frame_size(unFreq,&unAfeFrameSizeInSamples);

   unSize = unAfeFrameSizeInSamples * (pInitParams->outputBufferNumAfeFramesPerChannel);
   me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames = pInitParams->outputBufferNumAfeFramesPerChannel;
   QURT_ELITE_ASSERT(unSize > 0);

   me->ppCfgInfo.dynaPPSvcOutSizeInSample = unSize;
   me->ppCfgInfo.dynaPPSvcOutDurationInUsec = (((uint64_t)((me->ppCfgInfo.dynaPPSvcOutSizeInSample) * 1000 * 1000)) / unFreq);

   audpp_buf_delay_mgr_buf_duration_change(&me->bufDelayMgr, me->ppCfgInfo.dynaPPSvcOutSizeInSample, unFreq);

   /*   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "KK_DBG: buffer duration %d; output buffer size %d",
         me->ppCfgInfo.dynaPPSvcOutBufDuration, unSize);
    */

#ifdef AUDPROC_SIM
   {
      AudPPStatus_t* pPPStat = &me->audPPStatus;
      // Allocate memory to capture interleaved data,
      // but first check if some memory has already been allocated
      // and a buffer resizing is being attempted
      if (pPPStat->dbgptr)
      {
         qurt_elite_memory_free(pPPStat->dbgptr);
         pPPStat->dbgptr = NULL;
      }
      pPPStat->dbgptr = (int16_t*) qurt_elite_memory_malloc(unSize<<2, QURT_ELITE_HEAP_DEFAULT);
   }
#endif // AUDPROC_SIM

}

static ADSPResult AudPP_CreateSvcQueues(ThisAudDynaPPSvc_t* me, const int count)
{
   ADSPResult result;

   // name the queues. Name is concatenated with 5 items:
   // - a '/' required by POSIX.
   // - unique 3 letter code for service
   // - letter abbreviating which queue,
   //   e.g. 'd' 'c' 'b' for data, cmd, buf.
   // - number in case there are multiple queues of the same type,
   //   e.g. 2 buf q's
   // - count (up to 5 digits)
   //   e.g. the first instance of this service would name
   //   its dataQ "/EPLd00".and cmd q "/EPLc00"
   //snprintf(me->in_port[AUDPP_PRIMARY_IN_PORT]->dataQName, QURT_ELITE_DEFAULT_NAME_LEN, "%s%x%x", AUDDYNAPPSVC_DATA_Q_NAME, AUDPP_PRIMARY_IN_PORT, count);
   snprintf(me->cmdQName,  QURT_ELITE_DEFAULT_NAME_LEN, "%s%x%x", AUDDYNAPPSVC_CMD_Q_NAME, (unsigned int)AUDPP_PRIMARY_IN_PORT, count);
   snprintf(me->bufQName,  QURT_ELITE_DEFAULT_NAME_LEN, "DPSb0%x", count);


   // Create the queues.
   if (ADSP_FAILED(result = qurt_elite_queue_create(me->cmdQName, MAX_CMD_Q_ELEMENTS, &(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ)))
         || ADSP_FAILED(result = qurt_elite_queue_create(me->bufQName, BUF_Q_ELEMENTS_SIZE, &(me->bufQ))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to create message queues!", me->objId);
      return result;
   }

   // set up channel for listening to mqs
   if ( ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ,0))
         || ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, me->bufQ,0))
   )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to add msg queues to channel: %d!", me->objId, result);
      return result;
   }

   // Create far end data queue and add it to the channel -- extend with loop if needed
   for(uint32_t port_id=0;port_id<me->numActiveInPorts;port_id++)
   {
      snprintf(me->in_port[port_id]->dataQName, QURT_ELITE_DEFAULT_NAME_LEN, "%s%x%x", AUDDYNAPPSVC_DATA_Q_NAME, (unsigned int)port_id, count);
      if (ADSP_FAILED(result = qurt_elite_queue_create(me->in_port[port_id]->dataQName, MAX_DATA_Q_ELEMENTS, &(me->in_port[port_id]->handle.dataQ))))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to create input port %x data queue!", me->objId,port_id);
         return result;
      }
      // set up channel for listening to mqs
      if (ADSP_FAILED(result = qurt_elite_channel_addq(&me->channel, me->in_port[port_id]->handle.dataQ,0)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to add input port %x data queue to channel: %d!", me->objId, port_id, result);
         return result;
      }
      me->in_port[port_id]->handle.cmdQ = me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ;
   }
   // Allocate and queue up the output buffers.
   uint32_t numChannels = NUM_CHANNELS_STEREO;
   uint32_t bufSize = me->ppCfgInfo.dynaPPSvcOutSizeInSample * numChannels * BYTES_PER_SAMPLE_TWO;

   result = AudPP_AllocateOutputBuffers(me, bufSize, AudPP_NumOutputBuffersRequired(me));
   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "P%hX audproc_svc: Fail to create output buffers!", me->objId);
      return result;
   }

   return result;
}

static void AudPP_DestroySvcQueues(ThisAudDynaPPSvc_t* me)
{
   // call utility function to destroy data Q

   // call utility function to destroy far data Q
   for(uint32_t i=0;i<me->numActiveInPorts;i++)
   {
      elite_svc_destroy_data_queue(me->in_port[i]->handle.dataQ);
   }


   // call utility function to destroy cmd Q
   elite_svc_destroy_cmd_queue(me->in_port[AUDPP_PRIMARY_IN_PORT]->handle.cmdQ);

   // call common utility function to destroy buf Q
   elite_svc_destroy_buf_queue(me->bufQ, me->nBufsAllocated);
}

static void AudPP_DestroyPortsAndObject(ThisAudDynaPPSvc_t* me)
{
   if(me != NULL)
   {
      for(uint32_t i=0;i<AUDPP_MAX_INPUT_PORTS;i++)
      {
         if(me->in_port[i] != NULL)
         {
            qurt_elite_memory_free(me->in_port[i]);
            me->in_port[i] = NULL;
         }
      }
      qurt_elite_memory_free(me);
      me=NULL;
   }
}

/* Sets the QXDM log code as per the PP dynamic service type */
static void AudPP_SetQxdmLogCode(ThisAudDynaPPSvc_t* me)
{
   me->qxdm_log_code_in = audpp_qxdm_log_code_tbl_in[me->ppCfgInfo.dynaPPSvcType];
   if((DYNA_SVC_PP_TYPE_COPP == me->ppCfgInfo.dynaPPSvcType) || (DYNA_SVC_PP_TYPE_POPP == me->ppCfgInfo.dynaPPSvcType))
   {
	   me->qxdm_log_code_out = audpp_qxdm_log_code_tbl_out[me->ppCfgInfo.dynaPPSvcType];
   }
}

static bool_t AudPP_isSoftPauseMsgHeld(ThisAudDynaPPSvc_t* me)
{
   return (me->pPauseMsgPayload != NULL);
}

static ADSPResult AudPP_SetSampleslipDirection(ThisAudDynaPPSvc_t* me)
{
   ADSPResult result = ADSP_EOK;
   if (topo_is_module_present(me->pTopologyObj, AUDPROC_MODULE_ID_SAMPLESLIP))
   {
      struct ss_direction_pkt_t
      {
         asm_stream_param_data_v2_t header;
         audproc_sampleslip_direction_t param;
      } ss_direction_pkt;

      ss_direction_pkt.header.module_id = AUDPROC_MODULE_ID_SAMPLESLIP;
      ss_direction_pkt.header.param_id = AUDPROC_PARAM_ID_SAMPLESLIP_DIRECTION;
      ss_direction_pkt.header.param_size = sizeof(ss_direction_pkt) - sizeof(ss_direction_pkt.header); // This takes padding into account.
      ss_direction_pkt.header.reserved = 0;

      switch (me->ppCfgInfo.dynaPPSvcType)
      {
      case DYNA_SVC_PP_TYPE_COPP:
      case DYNA_SVC_PP_TYPE_POPP:
         ss_direction_pkt.param.direction = Playback;
         break;
      case DYNA_SVC_PP_TYPE_COPREP:
      case DYNA_SVC_PP_TYPE_POPREP:
         ss_direction_pkt.param.direction = Record;
         break;
      default:
         QURT_ELITE_ASSERT(0);
         ss_direction_pkt.param.direction = Playback;
         break;
      }

      result = topo_set_param(me->pTopologyObj, &ss_direction_pkt, sizeof(ss_direction_pkt), FIRST_INSTANCE);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX: Could not set sample slip direction.", me->objId);
         return result;
      }
   }
   return result;
}

static ADSPResult AudPP_InitializeLegacyTopology(ThisAudDynaPPSvc_t* me, uint32_t* stack_size_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint32_t topology_id = me->ppCfgInfo.dynaPPSvcTopologyID;
   AudPP_AudProcType pp_type = me->ppCfgInfo.dynaPPSvcType;
   const audproc_topology_definition_t *pTopologyDefinition;
   elite_topo_db_client_type client_type;

   result = AudPP_pptype_to_topo_db_client_type(pp_type, &client_type);
   if (ADSP_FAILED(result))
   {
      return result;
   }

   result = topo_db_get_topology(client_type, topology_id, &pTopologyDefinition);
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Topology %lx is not present in legacy database", me->objId, topology_id);
      result = ADSP_EUNSUPPORTED;
      return result;
   }

   result = topo_init(pTopologyDefinition,
         me->objId,
         me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames,
         &me->pTopologyObj,
         pp_type,
         stack_size_ptr,
         &me->topo_event_client,
         me->ppCfgInfo.initializeWithCompressedFormat,
         me->is_input_output_realtime,
         me->numActiveInPorts);
   if (ADSP_FAILED(result))
   {
      me->pTopologyObj = NULL;
   }

   return result;
}

static ADSPResult AudPP_InitializeCommonTopology(ThisAudDynaPPSvc_t* me, uint32_t* stack_size_ptr)
{
   ADSPResult result = ADSP_EOK;
   uint32_t topology_id = me->ppCfgInfo.dynaPPSvcTopologyID;
   AudPP_AudProcType pp_type = me->ppCfgInfo.dynaPPSvcType;
   elite_cmn_topo_db_entry_t *topology_ptr;
   uint32_t actual_size, size_req = 0;
   uint32_t handle;
   uint32_t num_modules;

   //query this topology's details
   result = elite_cmn_topo_db_get_topology(1 << AVCS_TOPO_CFG_AREA_AUDIO_BIT, topology_id, &topology_ptr, &actual_size, &handle);
   if (ADSP_FAILED(result) || 0 == actual_size || 0 == handle || ELITE_CMN_TOPO_DB_INVALID_HANDLE == handle || NULL == topology_ptr)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Topology %lx is not present in common database", me->objId, topology_id);
      result = ADSP_EUNSUPPORTED;
      return result;
   }

   //make sure actual_size returned is valid
   num_modules = topology_ptr->topo_def.num_modules;
   if(0 == num_modules)
   {
      //zero-module case
      size_req = sizeof(elite_cmn_topo_db_entry_t) - sizeof(avcs_module_instance_info_t);
   }
   else
   {
      //non-zero-module case
      size_req = sizeof(elite_cmn_topo_db_entry_t) + ((num_modules - 1)*sizeof(avcs_module_instance_info_t));
   }
   if(size_req != actual_size)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "P%hX audproc_svc: Topology %lx is present in common database. However actual_size %lu does not match size_req %lu",
            me->objId, topology_id, actual_size, size_req);
      (void)elite_cmn_topo_db_release_topology(handle);
      result = ADSP_EBADPARAM;
      return result;
   }

   result = common_topo_init(topology_ptr,
         me->objId,
         me->ppCfgInfo.dynaPPSvcOutBufNumAfeFrames,
         &me->pTopologyObj,
         pp_type,
         stack_size_ptr,
         &me->topo_event_client,
         me->ppCfgInfo.initializeWithCompressedFormat,
         me->is_input_output_realtime,
         me->numActiveInPorts);
   if (ADSP_FAILED(result))
   {
      me->pTopologyObj = NULL;
   }

   (void)elite_cmn_topo_db_release_topology(handle);
   return result;
}


static void AudPP_InitializeInputPort(AudPPInputPort_t *port_ptr)
{
   memset(port_ptr,0,sizeof(AudPPInputPort_t));

   port_ptr->handle.unSvcId = ELITE_DYNAPP_SVCID;
   port_ptr->mediaFormatReceived = FALSE;
   port_ptr->mediaFormatHeld = FALSE;
   port_ptr->eosReceived = FALSE;
   port_ptr->formatConverterInitialized = FALSE;
   port_ptr->prevInputBufInfo = port_ptr->inputBufInfo; // Remove

}
static void AudPP_event_payload_copy(audproc_event_query_payload *payload,
      void *pData, AudPP_AudProcType dynaPPSvcType)
{
   if((DYNA_SVC_PP_TYPE_POPP == dynaPPSvcType) || (DYNA_SVC_PP_TYPE_POPREP == dynaPPSvcType))
   {
      asm_stream_cmd_register_pp_events_payload_t *registerEvent = (asm_stream_cmd_register_pp_events_payload_t*) pData;
      payload->eventID    =  registerEvent->event_id;
      payload->moduleID   =  registerEvent->module_id;
      payload->instanceID =  registerEvent->instance_id;
      payload->eventbitfield = registerEvent->config_mask;
   }
   else
   {
      adm_register_pp_events_payload_t *registerEvent = (adm_register_pp_events_payload_t *) pData;
      payload->eventID    =  registerEvent->event_id;
      payload->moduleID   =  registerEvent->module_id;
      payload->instanceID =  registerEvent->instance_id;
      payload->eventbitfield = registerEvent->config_mask;
   }
   if((AUDPROC_MODULE_REGISTER_EVENT_FLAG & payload->eventbitfield) == TRUE)
   {
      payload->is_registered = TRUE;
   }
   else
   {
      payload->is_registered = FALSE;
   }
}
