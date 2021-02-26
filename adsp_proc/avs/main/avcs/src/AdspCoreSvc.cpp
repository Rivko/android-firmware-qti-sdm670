/**
@file AdspCoreSvc.cpp***********************

@brief This file contains the implementation for AdspCoreSvc.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/avcs/src/AdspCoreSvc.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/19/10   KK      Created file.
07/23/16   HB	   Removed proprietary information.
==========================================================================*/

/*------------------------------------------------------------------------
   Copyright (c) 2013-2017 QUALCOMM Technologies, Incorporated.
   All Rights Reserved.
   QUALCOMM Technologies Proprietary.
--------------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
/// Adsp APIs
#include "adsp_core_api.h"
#include "adsp_asm_api.h"
#include "adsp_adm_api.h"
#ifndef MDSPMODE
#include "adsp_usm_api.h"
#endif
#include "adsp_afe_service_commands.h"
#include "adsp_vpm_api.h"
#include "adsp_vsm_api.h"
#include "adsp_lsm_api.h"
#include "cvd_adsp_version.h"
#include "EliteTopology_db.h"
#include "EliteCmnTopology_db.h"
#include "EliteCmnTopology_db_if.h"

// Private apis
#include "adsp_prv_avcs_api.h"

/// ADSP error codes
#include "adsp_error_codes.h"

/// APR Service IDs
#include "aprv2_ids_services.h"

/// Q6 Build Version
#ifndef MDSPMODE
#include "q6_build_version.h"
#endif

/// ADSPCoreSvc header
#include "AdspCoreSvc.h"
#include "AdspCoreSvc_InstanceStruct.h"
#include "AdspCoreSvc_PerfMon.h"
#include "qurt_elite_adsppm_wrapper.h"





// PP topology
#include "AudDynaPPSvc.h"

#include "module_mgr.h"
#include "adsp_amdb_static.h"

//static service public header files
#include "AFEInterface.h"
#include "AudioStreamMgr.h"
#include "AudDevMgr_AprIf.h"
#include "ListenStreamMgr.h"
#include "cvd_version.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/**  Internal API. Opcode for exiting the premordial main
 *   thread */
#define AVCS_CMD_EXIT_PREMORDIAL_THREAD     0x00012910

/**  Internal API. Opcode for deleting all the custom topologies. */
#define AVCS_CMD_CLEAR_PP_DBS   0x00012913

/**
  Internal API. Sets loggingg cfg file. */
#define AVCS_CMD_SET_LOG_CFG_FILE    0x0001291C
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

//Maximum number of topologies defined in a command.
#define AVCS_MAX_NUMBER_TOPOLOGIES_IN_CMD 4096

//Maximum number of modules in a topology.
#define AVCS_MAX_NUMBER_MODULES_IN_TOPO 1024

// thread settings. Thread priority should go to separate file.
static char AVCS_THREAD_NAME[]="ACS";
static const uint32_t AVCS_THREAD_STACK_SIZE = 16384;
static AvsCoreSvc_t *pMe = NULL; //defined static for easier debugging.

//qurt_elite MemoryMap Client
uint32_t avcsMemoryMapClient;

/** Maximum number of low power memory segments.
    1. Audio read only code section
    2. Audio read only data section
    3. OS read-only data
*/
#define MAX_LOW_POWER_MEM_SEGMENTS  (3)

#define CACHE_LINE_SIZE  32
#define CACHE_ALIGNMENT_MASK  (CACHE_LINE_SIZE - 1)

#define SIZE_OF_ARRAY(x)         (sizeof(x)/sizeof(x[0]))
/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */

// destructor
static void AdspCoreSvc_Destroy (AvsCoreSvc_t* pMe);

// Main work loop for service thread. Pulls msgs off of queues and processes them.
static int AdspCoreSvc_WorkLoop(void* pInstance);

//Start service handler
static ADSPResult AdspCoreSvc_StartSvcHandler (void *pInstance, elite_msg_any_t* pMsg);

static ADSPResult AdspCoreSvc_CustomCmdHandler (void *pInstance, elite_msg_any_t* pMsg);

//Destroy service handler
static ADSPResult AdspCoreSvc_DestroySvcHandler(void *pInstance, elite_msg_any_t* pMsg);

// Process response Q
static ADSPResult AdspCoreSvc_ProcessRspQueue(AvsCoreSvc_t *pInstance);

//APR Call back function
static int32_t AdspCoreSvc_isr_dispatch_cb ( elite_apr_packet_t* packet, void* dispatch_data );

//Function to process all APR commands.
static int AdspCoreSvcApr(void* pInstance, elite_msg_any_t* pMsg);

//Get API versions of all static services
static int AdspCoreSvc_GetSvcApiVersions(elite_apr_packet_t* pAprPacket);

//Get Build Version and API version of all
static int AdspCoreSvc_GetFwkVersion(elite_apr_packet_t* pAprPacket);

//Get Memory stats
static int AdspCoreSvc_GetMemoryStats(elite_apr_packet_t* pAprPacket, uint32_t cmd);

//Remote API to AVCS to vote AVTimer by remote clients
static ADSPResult AVS_RemoteAVTimerVoteRequest(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

//Remote API to AVCS to release AVTimer by remote clients
static ADSPResult AVS_RemoteAVTimeReleaserRequest(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

//AVCS common register topology handler
static ADSPResult AdspCoreSvc_AprRegisterTopologiesHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

//AVCS common de-register topology handler
static ADSPResult AdspCoreSvc_AprDeRegisterTopologiesHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

static ADSPResult AdspCoreSvc_AddRemovePoolPages(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket, bool_t is_add);
static ADSPResult AdspCoreSvc_AmdbReset(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);
static ADSPResult AdspCoreSvc_PrintAllAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);
static ADSPResult AdspCoreSvc_DeregisterAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);
static ADSPResult AdspCoreSvc_RegisterAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

//Lock lines with the designated size
static ADSPResult AVS_LockCacheLines(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket);

/*Function to return the API version for core service*/
static ADSPResult avcs_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

ADSPResult AVS_SetLogCfgFile(elite_apr_packet_t *pPkt);

/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */


/* Queue handler table.
   TODO: further optimize to reduce the memory.
   */
static elite_svc_msg_handler_func pHandler[] =
{
   AdspCoreSvc_CustomCmdHandler,          //0 - ELITE_CMD_CUSTOM
   AdspCoreSvc_StartSvcHandler,          //1 - ELITE_CMD_START_SERVICE
   AdspCoreSvc_DestroySvcHandler, //2 - ELITE_CMD_DESTROY_SERVICE
   elite_svc_unsupported,          //3 - ELITE_CMD_CONNECT
   elite_svc_unsupported,          //4 - ELITE_CMD_DISCONNECT
   elite_svc_unsupported,          //5 - ELITE_CMD_PAUSE
   elite_svc_unsupported,          //6 - ELITE_CMD_RESUME
   elite_svc_unsupported,          //7 - ELITE_CMD_FLUSH
   elite_svc_unsupported,          //8 - ELITE_CMD_SET_PARAM
   elite_svc_unsupported,          //9 - ELITE_CMD_GET_PARAM
   elite_svc_unsupported,          //10 - ELITE_DATA_BUFFER
   elite_svc_unsupported,          //11 - ELITE_DATA_MEDIA_TYPE
   elite_svc_unsupported,          //12 - ELITE_DATA_EOS
   elite_svc_unsupported,          //13 - ELITE_DATA_RAW_BUFFER
   elite_svc_unsupported,          //14 - ELITE_CMD_STOP_SERVICE
   AdspCoreSvcApr               //15 - ELITE_APR_PACKET

};

typedef struct topo_list topo_list;
struct topo_list
{
	uint32_t topology_id;
	topo_list *next_ptr;
};

typedef ADSPResult (*version_function_t) (avs_svc_api_info_t *svc_info);

/*fucntion table for individial service version functions*/
static const version_function_t get_svc_version_func[] =
{
    avcs_svc_get_api_version,
    afe_svc_get_api_version,
    asm_svc_get_api_version,
    adm_svc_get_api_version,
    lsm_svc_get_api_version,
    mvm_svc_get_api_version,
    cvs_svc_get_api_version,
    cvp_svc_get_api_version
};

avs_svc_api_info_t svc_info[SIZE_OF_ARRAY(get_svc_version_func)];

//Utility function to determine if a list of module IDs provided is unique
static bool_t IsModuleListUnique(avcs_topology_definition_t *topo_def_ptr, void *mod_def_ptr, uint32_t topo_version);

//Utility function to determine if a topology ID is unique amongst the topologies provided in the command's payload
static bool_t IsTopologyUnique(uint32_t unTopologyID, topo_list *pHead);

//Declaration of core service object
static AvsCoreSvc_t gCoreStatSvcObject;

//extern qurt_elite_signal_t global_wait_signal_at_main;


/** Below are the changes required to support dynamic loading tests on SIM.
 * On-target dlinitex and dldeinit are called from dev cfg, but on SIM since
 * that is not compiled, we call it from here. */
#ifdef SIM_DYNAMIC_LOADING
#include "dlfcn.h"

char* builtin[] __attribute__((weak)) = {(char*)"libc.so",(char*)"libgcc.so"};

extern "C" {
void HAP_debug_v2(int level, const char* file, int line,
		const char* format, ...){
	char buf[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);
	printf("\nLevel:%d %s:%d %s\n", level, file, line, buf);
}

void HAP_debug(const char *msg, int level, const char *filename, int line){
	HAP_debug_v2(level, filename, line, msg);
}

int std_memsmove(void *dst, int dst_size, const void *src, int src_size){
	return memsmove(dst,dst_size,src,src_size);
}

int std_memscpy(void *dst, int dst_size, const void *src, int src_size){
	return memscpy(dst,dst_size,src,src_size);
}

}
#endif

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/* =======================================================================
**         AdspCoreSvc Instance Create/Destroy/Workloop
** ======================================================================= */

ADSPResult AdspCoreSvc_Create (uint32_t inputParam, void **svcHandle)
{
   int result;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: Creating");
   qurt_elite_queue_t *pTempCmdQ, *pTempRespQ;

   // Assign the instance handle with core sevice struct
   pMe = &gCoreStatSvcObject;
   if (!pMe) return ADSP_ENOMEMORY;

   result = AdspCoreSvc_Internal_Init();
   if (ADSP_FAILED(result))
   {
      return result;
   }

   pTempCmdQ = (qurt_elite_queue_t*)pMe->avcsCmdQBuf;
   pTempRespQ = (qurt_elite_queue_t*)pMe->avcsRespQBuf;
   // Create acs cmd queue, rsp queue, and timer signal
   char cmdQName[] = "AcsCmdQ";
   char respQName[] = "AcsRspQ";
   if (ADSP_FAILED(result = qurt_elite_queue_init(cmdQName,
                      AVCS_MAX_CMD_Q_ELEMENTS, pTempCmdQ))
      || ADSP_FAILED(result = qurt_elite_queue_init(respQName,
                 AVCS_MAX_CMD_Q_ELEMENTS, pTempRespQ))
      || ADSP_FAILED(result = qurt_elite_signal_init(&(pMe->avcsTimerSignal))))

   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create AdspCoreSvc message resources!");
      AdspCoreSvc_Destroy(pMe);
      return result;
   }
   pMe->avcsSvcHandle.cmdQ = pTempCmdQ;
   pMe->avcsResponseQ = pTempRespQ;
   qurt_elite_globalstate.pAvcsCmdQ = pMe->avcsSvcHandle.cmdQ;

   // set up channel
   qurt_elite_channel_init(&pMe->avcsChannel);

   if (ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->avcsChannel, pMe->avcsSvcHandle.cmdQ, 0))
      || ADSP_FAILED(result = qurt_elite_channel_addq(&pMe->avcsChannel, pMe->avcsResponseQ, 0))
      || ADSP_FAILED(result = qurt_elite_channel_add_signal(&pMe->avcsChannel, &(pMe->avcsTimerSignal), 0)))
   {
      AdspCoreSvc_Destroy(pMe);
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: failed to configure channel = %d!", result);
      return(result);
   }
   // populate me
   pMe->avcsSvcHandle.unSvcId = ELITE_CORE_SVCID;

#ifdef SIM_DYNAMIC_LOADING
   {
		DL_vtbl vtbl[] = {{sizeof(DL_vtbl), HAP_debug_v2, NULL,NULL,NULL}};

      int rc = dlinitex(2, builtin, vtbl);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "rtld_init returned %d", rc);
      if (1 != rc)
      {
         return NULL;
      }
   }
#endif

   pMe->pDynLoader = adsp_dynamic_loader_create(&pMe->avcsChannel);

   // Launch the thread
   if (ADSP_FAILED(result = qurt_elite_thread_launch(&(pMe->avcsSvcHandle.threadId), AVCS_THREAD_NAME, NULL,
      AVCS_THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_ADSP_CORE_SVC_PRIO_ID), AdspCoreSvc_WorkLoop, (void*)pMe,
      QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create Elite AvsCoreSvc Thread!");
      AdspCoreSvc_Destroy(pMe);
      return result;
   }

   *svcHandle = &(pMe->avcsSvcHandle);

   // register with qurt_elite memory map.
   qurt_elite_memorymap_register(&avcsMemoryMapClient);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS: Created, memorymapclient: %lx", avcsMemoryMapClient);
   return ADSP_EOK;
}

/*Destroy yourself*/
static void AdspCoreSvc_Destroy(AvsCoreSvc_t *pMe)
{
   QURT_ELITE_ASSERT( pMe );

   if (NULL != pMe->pDynLoader)
   {
	   adsp_dynamic_loader_destroy(pMe->pDynLoader);
	   pMe->pDynLoader = NULL;
   }

   elite_svc_deinit_cmd_queue(pMe->avcsSvcHandle.cmdQ);
   elite_svc_deinit_cmd_queue(pMe->avcsResponseQ);
   qurt_elite_channel_destroy(&pMe->avcsChannel);
   qurt_elite_signal_deinit(&(pMe->avcsTimerSignal));

   AdspCoreSvc_Internal_DeInit();
}

/* APR Call back function*/
static int32_t AdspCoreSvc_isr_dispatch_cb ( elite_apr_packet_t* pPacket, void* dispatch_data )
{
   int32_t        rc = APR_EOK;
   elite_msg_any_t     sMsg;
   ADSPResult result;

   if ( NULL == pPacket  )
   {
      rc = APR_EBADPARAM;
      return rc;
   }

   /* Queue the packets in to the ASC command queue and dispatch it based on the opcode.  Assuming
    * queue onto Elite service succeeds, the receiving service is expected to respond that the command was accepted,
    * to process, and free the APR packet when the requested operation is complete.  Current design is to use simple 8 uint8_t
    * elite header to manage APR packet delivery, so no additional copies of APR payload is required, and FADD message
    * doesn't need to be obtained from buffer manager to handle delivery to the Elite service.  This assumes it is OK to hold
    * onto the incoming apr_packet_t *packet pointer until the packet is processed by the destination service.
    */

   /* Report command acceptance when requested. */
   /*
   if ( elite_apr_if_msg_type_is_cmd( pPacket) )
   {

      if( APR_EOK != ( rc = elite_apr_if_send_event_accepted( pMe->aprHandle, pPacket)))
      {
         (void) elite_apr_if_free( pMe->aprHandle, pPacket );
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "ASC Error sending APR_IBASIC_RSP_ACCEPTED", rc);
         return rc;
      }
    }
    */
   /* simple delivery...copy pointer.  receiver must free packet if queue'd successfully */
   sMsg.unOpCode = ELITE_APR_PACKET;
   sMsg.pPayload = (void *) pPacket;

   /* Queue packet onto ASC command Q via standard Elite op/payload format */
   if (ADSP_FAILED(result =qurt_elite_queue_push_back(pMe->avcsSvcHandle.cmdQ,(uint64_t*)&sMsg)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ASC APR isr Q to manager: qurt_elite_queue_push_back error = %d!!\n", result);
      (void) elite_apr_if_end_cmd( pMe->aprHandle, pPacket, APR_EBUSY );
      /** Why to return APR_EOK even in the error case
       *    whenever the dispatch function returns an error, APR frees
       *    the packet, but does not send the error response back to
       *    client. In order to free the packet and send error response
       *    back to client, we have to use elite_apr_if_end_cmd.
       *    Hence to prevent same packet freed again by APR in the
       *    error case, we always return APR_EOK. */
      rc = APR_EOK;
   }

   return rc;
}


/* This function is the main work loop for the service. Commands from SC
  are handled with the highest priority
 */
static int AdspCoreSvc_WorkLoop(void* pInstance)
{
   uint32_t cmdQMask, rspQMask, sigMask, dynLoaderMask;

   int result = ADSP_EFAILED;                            // general result value
   AvsCoreSvc_t *pMe = (AvsCoreSvc_t*)pInstance;       // instance structure

   // get the bitfield/channel mask for the cmdQ & rspQ from the channel;
   cmdQMask = qurt_elite_queue_get_channel_bit(pMe->avcsSvcHandle.cmdQ);
   rspQMask = qurt_elite_queue_get_channel_bit(pMe->avcsResponseQ);
   sigMask  = qurt_elite_signal_get_channel_bit(&(pMe->avcsTimerSignal));
   if (NULL != pMe->pDynLoader)
   {
	   dynLoaderMask = adsp_dynamic_loader_get_signal_mask(pMe->pDynLoader);
   }
   else
   {
	   dynLoaderMask = 0;
   }

   pMe->prof.uProfilingLevel = AVS_PROFILING_OFF;

   pMe->unAvcsChannelWaitMask =  (cmdQMask | rspQMask | sigMask | dynLoaderMask);

   pMe->avs_state = AVS_STATE_STATIC_SVC_MOD_READY;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: AVS state static svc ready (0x1)");
   // Enter forever loop
   for(;;)
   {
      // block on any one or more of selected queues to get a msg
      pMe->unAvcsChannelStatus = qurt_elite_channel_wait(&pMe->avcsChannel, pMe->unAvcsChannelWaitMask);

      // Check if commands came in
      if (pMe->unAvcsChannelStatus & cmdQMask)
      {
         //ToDo, simplfy this if ASC is only going to receive SC commands and not from anyone, in which case, it is better not to use
         //common utility thereby saving some memory and complexity

         // call common utility to process command queue in standard way.
         const uint32_t cmdTableSize = sizeof(pHandler)/sizeof(pHandler[0]);
         result = elite_svc_process_cmd_queue(pInstance, &(pMe->avcsSvcHandle), pHandler, cmdTableSize);

         // If service has been destroyed, exit.
             if (ADSP_ETERMINATED == result) return ADSP_EOK;
      }

      if(pMe->unAvcsChannelStatus & rspQMask)
      {
          // call AdspCoreSvc ResponeQ processing.
          result = AdspCoreSvc_ProcessRspQueue(pMe);
      }

      if(pMe->unAvcsChannelStatus & sigMask)
      {
           // Clear signal
           qurt_elite_signal_clear(&(pMe->avcsTimerSignal));

           // call AdspCoreSvc ResponeQ processing.
           AdspCoreSvc_ProcessPerfMon(pMe);

       }

      if(pMe->unAvcsChannelStatus & dynLoaderMask)
      {
    	  // The dynamic loader thread has finished.
    	  adsp_dynamic_loader_destroy(pMe->pDynLoader);
    	  pMe->pDynLoader = NULL;
    	  pMe->unAvcsChannelStatus &= ~dynLoaderMask;
    	  //when dynamic loader thread is done, all modules are ready.
    	  pMe->avs_state = AVS_STATE_ALL_MODULES_READY;
    	  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: AVS state all modules available (0x5)");
      }
   }
   return ADSP_EOK;
}

/* Message handler ELITE_CMD_START_SERVICE */
static ADSPResult AdspCoreSvc_StartSvcHandler (void *pInstance, elite_msg_any_t* pMsg)
{
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: StartSvcHandler Enter");
    ADSPResult result;
    AvsCoreSvc_t *pAvcsStatSvc = (AvsCoreSvc_t*)pInstance;
    // Register AVCS's ISR callback function with APR

    char svcName[] = "qcom.adsp.core";
    if (ADSP_FAILED( result = elite_apr_if_register_by_name( &(pAvcsStatSvc->aprHandle),
                                                            &(pAvcsStatSvc->aprAddr),
                                                            svcName, sizeof(svcName),
                                                            &AdspCoreSvc_isr_dispatch_cb,NULL)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Failed to register with APR   0x%8x", result );
      return result;
    }

    elite_msg_finish_msg( pMsg, ADSP_EOK);
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc: Started successfully");
    return ADSP_EOK;
}

static ADSPResult AdspCoreSvc_CustomCmdHandler (void *pInstance, elite_msg_any_t* pMsg)
{
   ADSPResult result = ADSP_EOK;

   elite_msg_custom_header_t* pPayload = (elite_msg_custom_header_t*)(pMsg->pPayload);

   switch (pPayload->unSecOpCode)
   {
   default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: Invalid custom cmd 0x%lx", pPayload->unSecOpCode);
      // Generate ACK
      elite_msg_finish_msg(pMsg, ADSP_EUNSUPPORTED);
      result = ADSP_EUNSUPPORTED;
   }

   return result;
}

/* Message handler ELITE_CMD_DESTROY_SERVICE */
static ADSPResult AdspCoreSvc_DestroySvcHandler(void *pInstance, elite_msg_any_t* pMsg)
{

    elite_msg_any_t msg;

    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: DestroySvc  Enter");

    /**Make a copy of msg because on destroy, pMe is freed */
    msg.unOpCode = pMsg->unOpCode;
    msg.pPayload = pMsg->pPayload;

    /* Release the APR resource...doesn't occur since we don't destroy the static service */
    (void)elite_apr_if_deregister( ((AvsCoreSvc_t*)pInstance)->aprHandle );

    AdspCoreSvc_Destroy((AvsCoreSvc_t*)pInstance);

    elite_msg_finish_msg( &msg, ADSP_EOK );

    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: DestroySvc  Leave");
    // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
    return ADSP_ETERMINATED;
}



static int AdspCoreSvcApr(void* pInstance, elite_msg_any_t* pMsg)
{

	int32_t        rc = APR_EOK;
	ADSPResult result = ADSP_EOK;
	QURT_ELITE_ASSERT(pMsg);

	elite_apr_packet_t *pAprPacket = (elite_apr_packet_t *) pMsg->pPayload;


	/* debug info.  Destination address should be ASC domain/service ID to get to this ISR, DstPort
	 * should be the session handle that this is directed to, in case a session has already been established
	 */

	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"At %x APRCMD %lx ASC Received Payload size %lu",
			((elite_apr_if_get_dst_addr(pAprPacket)<<16)|elite_apr_if_get_dst_port(pAprPacket)),
			elite_apr_if_get_opcode(pAprPacket),
			elite_apr_if_get_payload_size(pAprPacket));


	/* parse out the received pakcet.  Note in this current framework we are not prioritizing commands
	 * that can be completed immediately (non-gating).  We are simply processing command in order they are received, need a separate
	 * Elite Q to push to for command which can't be handled directly from this main dispatcher */

	uint32_t cmd = elite_apr_if_get_opcode(pAprPacket);
	switch( cmd )
	{

	case AVCS_GET_VERSIONS:
	{
		result = AdspCoreSvc_GetSvcApiVersions(pAprPacket);
		break;
	}

        case AVCS_CMD_GET_FWK_VERSION:
        {
                result = AdspCoreSvc_GetFwkVersion(pAprPacket);
                break;
        }

	case AVCS_CMD_GET_MEMORY_STATS:
	{
		result = AdspCoreSvc_GetMemoryStats(pAprPacket, cmd);


		break;
	}

	case AVCS_CMD_RESET_PEAK_HEAP_USE:
	{
		MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AvsCoreSvc: ResetPeakHeap Received");
		qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].peak_heap = 0;
		qurt_elite_globalstate.non_avs_stats.peak_heap = 0;
		/* send response */
		if (APR_EOK != (result = elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EOK)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: ResetPeakHeap Response send failed  = %d!", result);
		}

		break;

	}

	case AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS:
	{
		/* call handling function */
		AdspCoreSvc_SetPeriodicProfilingEventsCmdHandler(pMe, pAprPacket, 0);
		break;
	}

	case AVCS_CMD_EXIT_PREMORDIAL_THREAD:
	{
		/* First send the response, since setting the signal might start calling deinit functions, which might lead to crash */
		if (APR_EOK != (result = elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EOK)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Exit premordial thread Response send failed  = %d!", result);
		}

		//set the signal waiting for ever in the premordial main thread.
		//qurt_elite_signal_send(&global_wait_signal_at_main);


		break;

	}

	case AVCS_EVENT_GET_STATE:
	{
		// Check if all static svcs are up and ready, if they are, send stateRsp event; otherwise, do nothing.

		if (1 == qurt_elite_globalstate.uSvcUpStatus)
		{
			avcs_event_state_t dspState;
			dspState.state = pMe->avs_state;
			/* Allocate and send response event*/
			if (
					APR_EOK != (rc = elite_apr_if_alloc_send_event (  pMe->aprHandle,
							elite_apr_if_get_dst_addr(pAprPacket),
							0,
							elite_apr_if_get_src_addr(pAprPacket),
							elite_apr_if_get_src_port(pAprPacket),
							elite_apr_if_get_client_token(pAprPacket),
							AVCS_EVENT_STATE,
							&dspState,
							sizeof(avcs_event_state_t)))
							|| APR_EOK != (rc = elite_apr_if_free( pMe->aprHandle, pAprPacket))
			){
				/* In case of failure, just continue */
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: Could not allocate/send service stateRsp event packet = %d!", result);
				break;
			}else {
				MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: Got clt getState, sent svc stateRsp");
			}
		}else {
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AvsCoreSvc: Clt getState but svc not up");
		}

		break;
	}



	case AVCS_CMD_CLEAR_PP_DBS:
	{
		(void) elite_cmn_topo_db_reset();
		(void) Elite_ResetTopologyDatabase();
		(void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EOK);
		break;
	}

	case AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST:
	{
		result = AVS_RemoteAVTimerVoteRequest(pMe, pAprPacket);
		break;
	}

	case AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST:
	{
		result = AVS_RemoteAVTimeReleaserRequest(pMe, pAprPacket);
		break;
	}

	case AVCS_CMD_GET_COMMON_PROF_INFO:
	{
		AdspCoreSvc_GetCommonProfInfoCmdHandler(pMe, pAprPacket);
		break;
	}
	case AVCS_CMD_SET_LOG_CFG_FILE:
	{
		result = AVS_SetLogCfgFile(pAprPacket);
		(void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
		break;
	}
	case CVD_EVENT_ADSPPM_VOTE_DONE:
	{
		(void)AVS_AdsppmVoteDone(pMe, pAprPacket);
		(void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EOK);
		break;
	}
	case AVCS_COMMAND_LOCK_LINES:
	{
		result =  AVS_LockCacheLines(pMe,pAprPacket);
		(void) elite_apr_if_end_cmd(pMe->aprHandle,pAprPacket,result);
		break;
	}

	case AVCS_CMD_SHARED_MEM_MAP_REGIONS:
	{
		result = elite_mem_shared_memory_map_regions_cmd_handler(avcsMemoryMapClient, pMe->aprHandle, pAprPacket, AVCS_CMDRSP_SHARED_MEM_MAP_REGIONS);
		break;
	}
	case AVCS_CMD_SHARED_MEM_UNMAP_REGIONS:
	{
		result = elite_mem_shared_memory_un_map_regions_cmd_handler(avcsMemoryMapClient, pMe->aprHandle, pAprPacket);
		break;
	}
	case AVCS_CMD_REGISTER_TOPOLOGIES:
	{
		result = AdspCoreSvc_AprRegisterTopologiesHandler(pMe, pAprPacket);
		break;
	}

	case AVCS_CMD_DEREGISTER_TOPOLOGIES:
	{
		result = AdspCoreSvc_AprDeRegisterTopologiesHandler(pMe, pAprPacket);
		break;
	}
	case AVCS_REGISTER_AMDB_MODULES:
	{
		result = AdspCoreSvc_RegisterAmdbModules(pMe, pAprPacket);
		break;
	}
	case AVCS_DEREGISTER_AMDB_MODULES:
	{
		result = AdspCoreSvc_DeregisterAmdbModules(pMe, pAprPacket);
		break;
	}
	case AVCS_AMDB_PRINT_ALL_MODULES:
	{
		result = AdspCoreSvc_PrintAllAmdbModules(pMe, pAprPacket);
		break;
	}
	case AVCS_AMDB_RESET:
	{
		result = AdspCoreSvc_AmdbReset(pMe, pAprPacket);
		break;
	}
	case AVCS_CMD_ADD_POOL_PAGES:
	{
	   result = AdspCoreSvc_AddRemovePoolPages(pMe, pAprPacket, TRUE);
	   break;
	}
   case AVCS_CMD_REMOVE_POOL_PAGES:
   {
      result = AdspCoreSvc_AddRemovePoolPages(pMe, pAprPacket, FALSE);
      break;
   }
	default:
	{
		result = AdspCoreSvcPrivateApr(pInstance, pMsg);
		break;
	}
	}

	return result;
}


// ToDo later.. //TBD if responseQ is required or not.. Currently returning unsupported, since there is no usage for it.
static ADSPResult AdspCoreSvc_ProcessRspQueue(AvsCoreSvc_t *pInstance)
{
   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AdspCoreSvc Failed: Processing response queue is unsupported");
   return ADSP_EUNSUPPORTED;
}

static int AdspCoreSvc_GetSvcApiVersions(elite_apr_packet_t* pAprPacket)
{

    elite_apr_packet_t *pPacket = NULL;
    int32_t      rc = APR_EOK;

     /* Allocate the return packet */
    if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
         pMe->aprHandle,
         elite_apr_if_get_dst_addr( pAprPacket),
         elite_apr_if_get_dst_port( pAprPacket),
         elite_apr_if_get_src_addr( pAprPacket),
         elite_apr_if_get_src_port( pAprPacket),
         elite_apr_if_get_client_token( pAprPacket),
         AVCS_GET_VERSIONS_RSP,
         sizeof(avcs_service_cmd_rsp_get_api_versions_t) + NUMBER_OF_STATIC_SERVICES* sizeof(avcs_svc_info),
         &pPacket )))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetVersions Response packet allocation failed  = %ld!", rc);
       rc = ADSP_ENOMEMORY;
       goto getSvcApiVersions_error_1;
    }
    if (NULL == pPacket)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetVersions Response packet allocation failed. pPacket is NULL");
       rc = ADSP_ENOMEMORY;
       goto getSvcApiVersions_error_1;
    }

    void      *pPayload;
    void *pDummy;
      /* populate payload */
    elite_apr_if_get_payload((void**)(&pPayload), pPacket);

    avcs_service_cmd_rsp_get_api_versions_t *pGetVersionsPayload;
    pGetVersionsPayload = (avcs_service_cmd_rsp_get_api_versions_t *)pPayload;

    avcs_svc_info **ppSvcInfo;

    pDummy = ((uint8_t *)pPayload + sizeof(avcs_service_cmd_rsp_get_api_versions_t));
    ppSvcInfo = (avcs_svc_info **)&pDummy;

    /*This field might have some meaningful non zero value once the multi-process design is implemented.
    Until then the BuildID value should be set to zero*/
    pGetVersionsPayload->build_id = 0;
    pGetVersionsPayload->num_services = NUMBER_OF_STATIC_SERVICES;

#ifdef ADSPCORESVC_DEBUG
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc (ulBuildID , ulServices) = (%x,%x)!!\n", pGetVersionsPayload->build_id, pGetVersionsPayload->num_services);
#endif //ADSPCORESVC_DEBUG

    (*ppSvcInfo+0)->service_id = APRV2_IDS_SERVICE_ID_ADSP_CORE_V;
    (*ppSvcInfo+0)->version    = AVCS_CORE_VERSION;
    (*ppSvcInfo+1)->service_id = APRV2_IDS_SERVICE_ID_ADSP_AFE_V;
    (*ppSvcInfo+1)->version    = ADSP_AFE_VERSION;
    (*ppSvcInfo+2)->service_id = APRV2_IDS_SERVICE_ID_ADSP_ASM_V;
    (*ppSvcInfo+2)->version    = ADSP_ASM_VERSION;
    (*ppSvcInfo+3)->service_id = APRV2_IDS_SERVICE_ID_ADSP_ADM_V;
    (*ppSvcInfo+3)->version    = ADSP_ADM_VERSION;
    //same cvd version for MVM,CVS,CVP
    (*ppSvcInfo+4)->service_id = APRV2_IDS_SERVICE_ID_ADSP_MVM_V;
    (*ppSvcInfo+4)->version    = ADSP_CVD_VERSION;
    (*ppSvcInfo+5)->service_id = APRV2_IDS_SERVICE_ID_ADSP_CVS_V;
    (*ppSvcInfo+5)->version    = ADSP_CVD_VERSION;
    (*ppSvcInfo+6)->service_id = APRV2_IDS_SERVICE_ID_ADSP_CVP_V;
    (*ppSvcInfo+6)->version    = ADSP_CVD_VERSION;
#ifdef MDSPMODE
    (*ppSvcInfo+7)->service_id = APRV2_IDS_SERVICE_ID_ADSP_LSM_V;
    (*ppSvcInfo+7)->version    = ADSP_LSM_VERSION;

#else
    (*ppSvcInfo+7)->service_id = APRV2_IDS_SERVICE_ID_ADSP_USM_V;
    (*ppSvcInfo+7)->version    = ADSP_USM_VERSION;
    (*ppSvcInfo+8)->service_id = APRV2_IDS_SERVICE_ID_ADSP_LSM_V;
    (*ppSvcInfo+8)->version    = ADSP_LSM_VERSION;
#endif

#ifdef ADSPCORESVC_DEBUG
    int i;
    for (i=0;i<NUMBER_OF_STATIC_SERVICES;i++) {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc (ulServiceID , ulVersion) = (%x,%x)!!\n", (*ppSvcInfo+i)->service_id, (*ppSvcInfo+i)->version);
    }
#endif //ADSPCORESVC_DEBUG

    /* send response */
    if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetVersions Response send failed  = %ld!", rc);
       rc = ADSP_EFAILED;
       goto getSvcApiVersions_error_2;
    }

    /* free the original packet */
    (void)elite_apr_if_free( pMe->aprHandle, pAprPacket);
    return ADSP_EOK;

getSvcApiVersions_error_2:
    (void) elite_apr_if_free(pMe->aprHandle, pPacket);
getSvcApiVersions_error_1:
    (void)elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, rc);
    return rc;
}

static ADSPResult avcs_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_CORE_V;
  svc_info_ptr->api_version = AVCS_API_VERSION;
  svc_info_ptr->api_branch_version = AVCS_API_BRANCH_VERSION;

  return ADSP_EOK;
}

static int AdspCoreSvc_GetFwkVersion(elite_apr_packet_t* pAprPacket)
{

  elite_apr_packet_t *pPacket = NULL;
  int32_t      rc = APR_EOK;
  ADSPResult result = ADSP_EOK;

  uint32_t num_valid_services = 0;
  avs_svc_api_info_t *svc_info_ptr = &svc_info[0];

  /*Query all services for their versions*/
  for(uint32_t i=0; i<SIZE_OF_ARRAY(get_svc_version_func); i++)
  {
    result = get_svc_version_func[i](svc_info_ptr);
    if(ADSP_EOK == result)
    {
      num_valid_services++;
      svc_info_ptr++;
    }
  }

  uint32_t rsp_payload_size = sizeof(avcs_cmdrsp_get_fwk_version_t) +
      num_valid_services * sizeof(avs_svc_api_info_t);

  /* Allocate the return packet */
  if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
      pMe->aprHandle,
      elite_apr_if_get_dst_addr( pAprPacket),
      elite_apr_if_get_dst_port( pAprPacket),
      elite_apr_if_get_src_addr( pAprPacket),
      elite_apr_if_get_src_port( pAprPacket),
      elite_apr_if_get_client_token( pAprPacket),
      AVCS_CMDRSP_GET_FWK_VERSION,
      rsp_payload_size,
      &pPacket )))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AvsCoreSvc: GetFwkVersions Response packet allocation failed  = %ld!", rc);
    rc = ADSP_ENOMEMORY;
    goto getSvcApiVersions_error_1;
  }

  if (NULL == pPacket)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
        "AvsCoreSvc: GetFwkVersions Response packet allocation failed. pPacket is NULL");
    rc = ADSP_ENOMEMORY;
    goto getSvcApiVersions_error_1;
  }

  void *payload_ptr;
  /* populate payload */
  elite_apr_if_get_payload((void**)(&payload_ptr), pPacket);

  avcs_cmdrsp_get_fwk_version_t *get_fwk_version_rsp_payload_ptr;
  get_fwk_version_rsp_payload_ptr = (avcs_cmdrsp_get_fwk_version_t *)payload_ptr;

  svc_info_ptr = (avs_svc_api_info_t *)((uint8_t *)payload_ptr +
      sizeof(avcs_cmdrsp_get_fwk_version_t));

  /*first fill the build related information*/
  get_fwk_version_rsp_payload_ptr->build_major_version = AVS_BUILD_MAJOR_VERSION;
  get_fwk_version_rsp_payload_ptr->build_minor_version = AVS_BUILD_MINOR_VERSION;
  get_fwk_version_rsp_payload_ptr->build_branch_version = AVS_BUILD_BRANCH_VERSION;
  get_fwk_version_rsp_payload_ptr->build_subbranch_version = AVS_BUILD_SUBBRANCH_VERSION;
  get_fwk_version_rsp_payload_ptr->num_services = num_valid_services;

  /*now fill the service level parameters*/
  memscpy(svc_info_ptr,
          num_valid_services * sizeof(avs_svc_api_info_t),
          &svc_info[0],
          num_valid_services * sizeof(avs_svc_api_info_t));


#ifdef ADSPCORESVC_DEBUG
  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "AdspCoreSvc: AVS Build Version: %d.%d.%d.c%d",
        get_fwk_version_rsp_payload_ptr->build_major_version,
        get_fwk_version_rsp_payload_ptr->build_minor_version,
        get_fwk_version_rsp_payload_ptr->build_branch_version,
        get_fwk_version_rsp_payload_ptr->build_subbranch_version);
#endif //ADSPCORESVC_DEBUG


#ifdef ADSPCORESVC_DEBUG
  for (uint32_t i=0; i<num_valid_services; i++)
  {
    MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Service ID%x, API Version:%d, Branch version %d",
          svc_info[i].service_id, svc_info[i].api_version, svc_info[i].api_branch_version;
  }
#endif //ADSPCORESVC_DEBUG

  /* send response */
  if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetFwkVersions Response send failed  = %ld!", rc);
    rc = ADSP_EFAILED;
    goto getSvcApiVersions_error_2;
  }

  /* free the original packet */
  (void)elite_apr_if_free( pMe->aprHandle, pAprPacket);
  return ADSP_EOK;

getSvcApiVersions_error_2:
  (void) elite_apr_if_free(pMe->aprHandle, pPacket);
getSvcApiVersions_error_1:
  (void)elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, rc);
  return rc;
}


static int AdspCoreSvc_GetMemoryStats(elite_apr_packet_t* pAprPacket, uint32_t cmd)
{
    elite_apr_packet_t *pPacket = NULL;
    int32_t      rc = APR_EOK;

    /* Allocate the return packet */
    if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
         pMe->aprHandle,
         elite_apr_if_get_dst_addr( pAprPacket),
         elite_apr_if_get_dst_port( pAprPacket),
         elite_apr_if_get_src_addr( pAprPacket),
         elite_apr_if_get_src_port( pAprPacket),
         elite_apr_if_get_client_token( pAprPacket),
         AVCS_CMDRSP_GET_MEMORY_STATS,
         sizeof(avcs_cmd_rsp_get_memory_stats_t),
         &pPacket )))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetMemoryStats Response packet allocation failed  = %ld!", rc);
       rc = ADSP_ENOMEMORY;
       goto getMemoryStats_error_1;
    }
    if (NULL == pPacket)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetMemoryStats Response packet allocation failed. pPacket is NULL");
       rc = ADSP_ENOMEMORY;
       goto getMemoryStats_error_1;
    }

    /* populate payload */
    avcs_cmd_rsp_get_memory_stats_t *pPayload;
    void *pDummy;
    elite_apr_if_get_payload((&pDummy), pPacket);
    pPayload = (avcs_cmd_rsp_get_memory_stats_t *)pDummy;

    pPayload->num_elite_mallocs           = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_mallocs;
    pPayload->num_elite_frees             = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].num_frees;
    pPayload->current_elite_heap_use      = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap;
    pPayload->peak_elite_heap_use         = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].peak_heap;
    pPayload->num_non_elite_mallocs       = qurt_elite_globalstate.non_avs_stats.num_mallocs;
    pPayload->num_non_elite_frees         = qurt_elite_globalstate.non_avs_stats.num_frees;
    pPayload->current_non_elite_heap_use  = qurt_elite_globalstate.non_avs_stats.curr_heap;
    pPayload->peak_non_elite_heap_use     = qurt_elite_globalstate.non_avs_stats.peak_heap;
	pPayload->num_elite_nondefault_mallocs           = 0;
	pPayload->num_elite_nondefault_frees             = 0;
	pPayload->current_elite_nondefault_heap_use      = 0;
	pPayload->peak_elite_nondefault_heap_use         = 0;
  for (uint32_t numHeap = 1; numHeap <= QURT_ELITE_HEAP_MGR_MAX_NUM_HEAPS; numHeap++)
  {
    pPayload->num_elite_nondefault_mallocs += qurt_elite_globalstate.avs_stats[numHeap].num_mallocs;
    pPayload->num_elite_nondefault_frees += qurt_elite_globalstate.avs_stats[numHeap].num_frees;
    pPayload->current_elite_nondefault_heap_use += qurt_elite_globalstate.avs_stats[numHeap].curr_heap;
    pPayload->peak_elite_nondefault_heap_use += qurt_elite_globalstate.avs_stats[numHeap].peak_heap;
  }

    /* send response */
    if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetMemoryStats Response send failed  = %ld!", rc);
       rc = ADSP_EFAILED;
       goto getMemoryStats_error_2;
    }
    /* free the original packet */
    (void)elite_apr_if_free( pMe->aprHandle, pAprPacket);

    return ADSP_EOK;

getMemoryStats_error_2:
    (void) elite_apr_if_free(pMe->aprHandle, pPacket);
getMemoryStats_error_1:
    (void)elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, rc);
    return rc;
}

static ADSPResult AVS_RemoteAVTimerVoteRequest(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   elite_apr_packet_t *pPacket=NULL;
   int32_t rc = APR_EOK;

   if(!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_cmd_remote_avtimer_vote_request_t))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for AVTimer request.");
      rc = ADSP_EUNEXPECTED;
      goto remoteAVTimerVoteRequest_error_1;
   }

   /* Allocate the return packet */
   if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
       pMe->aprHandle,
       elite_apr_if_get_dst_addr( pAprPacket),
       elite_apr_if_get_dst_port( pAprPacket),
       elite_apr_if_get_src_addr( pAprPacket),
       elite_apr_if_get_src_port( pAprPacket),
       elite_apr_if_get_client_token( pAprPacket),
       AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST,
       sizeof(avcs_cmd_rsp_remote_avtimer_vote_request_t), &pPacket )))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: AVS_RemoteAVTimerVoteRequest Response packet allocation failed  = %ld!", rc);
      rc = ADSP_ENOMEMORY;
      goto remoteAVTimerVoteRequest_error_1;
   }
   if (NULL == pPacket)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: AVS_RemoteAVTimerVoteRequest Response packet allocation failed. pPacket is NULL");
      rc = ADSP_ENOMEMORY;
      goto remoteAVTimerVoteRequest_error_1;
   }

   void *pPayload;

   /* response payload */
   elite_apr_if_get_payload((void**)(&pPayload), pPacket);
   avcs_cmd_rsp_remote_avtimer_vote_request_t *pRspPayload;
   pRspPayload = (avcs_cmd_rsp_remote_avtimer_vote_request_t *)pPayload;

   /* parse command payload */
   avcs_cmd_remote_avtimer_vote_request_t *pCmd;
   pCmd = (avcs_cmd_remote_avtimer_vote_request_t*)(elite_apr_if_get_payload_ptr(pAprPacket));
   avtimer_open_param_t  avtimer_open_param_avcs;

   avtimer_open_param_avcs.client_name = &(pCmd->client_name[0]);
   avtimer_open_param_avcs.flag = 0; //not used in api

   if (ADSP_FAILED(rc = avtimer_drv_hw_open((void**)(&(pRspPayload->avtimer_handle)), &avtimer_open_param_avcs)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: AVS_RemoteAVTimerVoteRequest request failed  = %ld!", rc);
      rc = ADSP_EFAILED;
      goto remoteAVTimerVoteRequest_error_2;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: avtimer_handle = 0x%lx, name = %s!",
         (pRspPayload->avtimer_handle), avtimer_open_param_avcs.client_name);

   /* send response */
   if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: AVS_RemoteAVTimerVoteRequest Response send failed  = %ld!", rc);
      rc = ADSP_EFAILED;
      goto remoteAVTimerVoteRequest_error_2;
   }

   /* free the original packet */
   (void) elite_apr_if_free( pMe->aprHandle, pAprPacket);

   return ADSP_EOK;

remoteAVTimerVoteRequest_error_2:
    //free response packet
    (void) elite_apr_if_free(pMe->aprHandle, pPacket);
remoteAVTimerVoteRequest_error_1:
    //ack back the apr packet
    (void)elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, rc);
    return rc;
}

static ADSPResult AVS_RemoteAVTimeReleaserRequest(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;

   if(!pAprPacket)
   {
      return ADSP_EBADPARAM;
   }

   if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_cmd_remote_avtimer_release_request))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for AVTimer request.");
      (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, ADSP_EFAILED);
      return ADSP_EUNEXPECTED;
   }

   /* parse command payload */
   avcs_cmd_remote_avtimer_release_request *pCmd = (avcs_cmd_remote_avtimer_release_request*)(elite_apr_if_get_payload_ptr(pAprPacket));

   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: avtimer_handle = 0x%lx", (pCmd->avtimer_handle));

   if (ADSP_FAILED(result = avtimer_drv_hw_close((void*)(pCmd->avtimer_handle))))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: AVS_RemoteAVTimeReleaserRequest release request failed  = %d!", result);
      result = ADSP_EFAILED;
   }

   /* end command */
   (void) elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);

   return result;
}

static ADSPResult AVS_LockCacheLines(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
	ADSPResult result = ADSP_EOK;
	qurt_mem_region_t *pQmem_region = &(pMe->cacheOp.memRegion);
	memset((void*)pQmem_region,0,sizeof(qurt_mem_region_t));
	if( NULL == pAprPacket)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"AvsCoreSvc: ERROR in locking cache lines, one or more of the following is NULL.  pAprPacket = %p",pAprPacket);
		return ADSP_EFAILED;
	}
	if(elite_apr_if_get_payload_size(pAprPacket) < sizeof(avcs_command_lock_lines_t))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Apr payload of insufficient size received for line locking command.");
		return ADSP_EUNEXPECTED;
	}
	avcs_command_lock_lines_t*pBuf = (avcs_command_lock_lines_t*)elite_apr_if_get_payload_ptr(pAprPacket);
	MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cache partitioning received command with opcode %ld", pBuf->sec_opcode);

	switch (pBuf->sec_opcode)
	{
	case AVCS_COMMAND_LOCK_LINES_INIT:
		qurt_mem_pool_t partition_pool;

		if(ADSP_EOK != (result = qurt_mem_pool_attach((char*)"DEFAULT_PHYSPOOL", &partition_pool)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cache Partition failed. Unable to attach to default physpool\n");
		}
		else
		{
			qurt_mem_region_attr_t partition_attr;

			qurt_mem_region_attr_init( &partition_attr);
			qurt_mem_region_attr_set_cache_mode( &partition_attr, QURT_MEM_CACHE_WRITEBACK);
			qurt_mem_region_attr_set_mapping  ( &partition_attr, QURT_MEM_MAPPING_VIRTUAL );


			/* create the memory region */
			if (ADSP_EOK != (result = qurt_mem_region_create(pQmem_region,
					pBuf->total_size_bytes,
					partition_pool,
					&partition_attr )))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create mem region with result %ld", result);
				return result;
			}

			if (ADSP_EOK != (result = qurt_mem_region_attr_get(*pQmem_region, &partition_attr)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get mem region attribute with result %ld",result);
			}
			else
			{
				unsigned int addr = 0;
				qurt_mem_region_attr_get_virtaddr(&partition_attr, &addr);
				pMe->cacheOp.virtAddr = (uint32) addr;
			}
			pMe->cacheOp.totalSize = pBuf->total_size_bytes;
		}
		break;
	case AVCS_COMMAND_LOCK_LINES_EXEC:
		if(pMe->cacheOp.lockedSize < pMe->cacheOp.totalSize)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
					"qurt cache locking with addr %lx size %ld",
               (pMe->cacheOp.virtAddr+pMe->cacheOp.lockedSize),pBuf->total_size_bytes);
			result = qurt_mem_l2cache_line_lock(pMe->cacheOp.virtAddr+pMe->cacheOp.lockedSize,pBuf->total_size_bytes);
			if (QURT_EOK != result)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
						"qurt cache locking failed with status %ld",
						result);
				return ADSP_EFAILED;
			}
			pMe->cacheOp.lockedSize += pBuf->total_size_bytes;
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
					"qurt cache locking completed, returning OK");
		}
		break;
	case AVCS_COMMAND_LOCK_LINES_DEINIT:
	default:
		if( 0 < pMe->cacheOp.lockedSize)
		{
			qurt_mem_l2cache_line_unlock(pMe->cacheOp.virtAddr,pMe->cacheOp.lockedSize);
		}
		if ( 0 < pMe->cacheOp.totalSize)
		{
			qurt_mem_region_delete(pMe->cacheOp.memRegion);
		}
		break;

	}
	MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "cache partitioning returning result %ld", result);
	return result;


}

static ADSPResult AdspCoreSvc_AprRegisterTopologiesHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
	ADSPResult result = ADSP_EOK;
	avcs_cmd_register_topologies_t *pPayload = (avcs_cmd_register_topologies_t*)elite_apr_if_get_payload_ptr(pAprPacket);
	avcs_topology_buffer_t *pVirtAddr;
	uint8_t *pTemp;
	uint32_t unNumberTopologiesInPayload = 0;
	uint32_t moduleInfoBufferSize = 0;
	uint32_t buffer_size = 0;
	avcs_topology_buffer_t *pHeader;
	avcs_topology_definition_t *pTopoDefBase;
	void *pModDefBase;
	uint32_t *pPayloadBase;
	topo_list *pHead = NULL;
	topo_list *pTail = NULL;

	if (0 == pPayload->mem_map_handle)
	{
		//In-band case: avcs_topology_buffer_t starts just past the last entry of the incoming payload's header
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Processing in-band.");
		pPayloadBase = (uint32_t*)(pPayload + 1);
		pVirtAddr = (avcs_topology_buffer_t*)(pPayloadBase);
	}
	else
	{
		//Out-band case: avcs_topology_buffer_t starts at the address mentioned in the incoming payload's header
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Processing out-band.");
		uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
		uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

		//Check cache line alignment
		bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);
		if (!isAlignedToCacheLine)
		{
			result = ADSP_EBADPARAM;
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Physical address is not aligned to cache line.");
			goto _bailout_avcs_register_topologies;
		}

		//Physical->Virtual
		elite_mem_shared_memory_map_t bufferMemNode;
		bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
		bufferMemNode.unMemMapClient = avcsMemoryMapClient;
		result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->buffer_size, &bufferMemNode);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Failed to map physical memory.");
			goto _bailout_avcs_register_topologies;
		}

		// Since this buffer will be read, need to invalidate the cache.
		result = elite_mem_invalidate_cache(&bufferMemNode);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Failed to invalidate memory.");
			goto _bailout_avcs_register_topologies;
		}

		pVirtAddr = (avcs_topology_buffer_t*)(bufferMemNode.unVirtAddr);
	}

	//Size validation - part 1
	buffer_size = pPayload->buffer_size;
	if (buffer_size < sizeof(avcs_topology_buffer_t))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Size of register topologies buffer %lu bytes is insufficient.", buffer_size);
		result = ADSP_ENEEDMORE;
		goto _bailout_avcs_register_topologies;
	}
	buffer_size -= sizeof(avcs_topology_buffer_t);

	//Num_topologies validation
	pHeader = (pVirtAddr);
	if (0 == pHeader->num_topologies || AVCS_MAX_NUMBER_TOPOLOGIES_IN_CMD < pHeader->num_topologies)
	{
		//Note: The API does not restrict the upper bound on the  number of topologies that may be registered in one command.
		//However, this boundary check is in place to protect against any memory corruption.
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid NumTopos: %lu", pHeader->num_topologies);
		result = ADSP_EBADPARAM;
		goto _bailout_avcs_register_topologies;
	}

	//Get hold of #of topos in payload and advance to the next part of the payload.
	unNumberTopologiesInPayload = pHeader->num_topologies;
	pVirtAddr++;
	pTemp = (uint8_t *)pVirtAddr;

	//Iterate through all the topologies in the command.
	while (unNumberTopologiesInPayload > 0)
	{
		//Size validation - part 2
		if (buffer_size < sizeof(avcs_topology_definition_t))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Size of register topologies buffer %lu bytes is insufficient to hold one topology definition.", buffer_size);
			result = ADSP_ENEEDMORE;
			goto _bailout_avcs_register_topologies;
		}
		buffer_size -= sizeof(avcs_topology_definition_t);

		//Get hold of the base of the avcs_topology_definition_t structure.
		pTopoDefBase = (avcs_topology_definition_t *)(pTemp);

		//Contents of avcs_topology_definition_t validation.
		//1. Version check (version 0 and 1 is supported)
		if (1 < pTopoDefBase->version)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid TopoVer: %lu", pTopoDefBase->version);
			result = ADSP_EBADPARAM;
			goto _bailout_avcs_register_topologies;
		}

		//2. Unique topology ID validation (with existing common global database entries).
		if(ADSP_EOK != elite_cmn_topo_db_is_topology_unique(pTopoDefBase->topology_id))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid TopoID: %lx is not unique.", pTopoDefBase->topology_id);
			result = ADSP_EBADPARAM;
			goto _bailout_avcs_register_topologies;
		}

		//3. Unique topology ID validation (within this command's payload).
		if(FALSE == IsTopologyUnique(pTopoDefBase->topology_id, pHead))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid TopoID: %lx is not unique (repeated within in this payload).", pTopoDefBase->topology_id);
			result = ADSP_EBADPARAM;
			goto _bailout_avcs_register_topologies;
		}

		//4. topo_config validation.
		if((0!= ((pTopoDefBase->topo_config) & (0xFFFFFFF0))) || (0 == pTopoDefBase->topo_config))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid TopoCfg: %lu", pTopoDefBase->topo_config);
			result = ADSP_EBADPARAM;
			goto _bailout_avcs_register_topologies;
		}

		//5. num_modules validation.
		if (AVCS_MAX_NUMBER_MODULES_IN_TOPO < pTopoDefBase->num_modules)
		{
			//Note: The API does not restrict the upper bound on the  number of modules that may be chained in one topology.
			//However, this boundary check is in place to protect against any memory corruption.
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Invalid NumMods: %lu", pTopoDefBase->num_modules);
			result = ADSP_EBADPARAM;
			goto _bailout_avcs_register_topologies;
		}

		//Size validation - part 3
		moduleInfoBufferSize = pTopoDefBase->num_modules *
				((pTopoDefBase->version==0)? sizeof(avcs_module_info_t): sizeof(avcs_module_instance_info_t));
		if (buffer_size < moduleInfoBufferSize)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Size of register topologies buffer %lu bytes is insufficient to hold %lu module definitions.",
					buffer_size, pTopoDefBase->num_modules);
			result = ADSP_ENEEDMORE;
			goto _bailout_avcs_register_topologies;
		}
		buffer_size -=  moduleInfoBufferSize;

		//Advance to the next part of the payload.
		pTemp += sizeof(avcs_topology_definition_t);

		//Validate that this module list has no repeating modules within it. Do this check only for topologies with non-zero num_modules.
		if(0 != pTopoDefBase->num_modules)
		{
			if(FALSE == IsModuleListUnique(pTopoDefBase, pTemp, pTopoDefBase->version))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: TopoID: %lx has repeating modules.", pTopoDefBase->topology_id);
				result = ADSP_EBADPARAM;
				goto _bailout_avcs_register_topologies;
			}
		}

		//Store this topology_id in the local linked list for immediate comparison.
		topo_list *pLocal = (topo_list *)qurt_elite_memory_malloc(sizeof(topo_list), QURT_ELITE_HEAP_DEFAULT);
		if(NULL == pLocal)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Memory allocation failure");
			result = ADSP_ENOMEMORY;
			goto _bailout_avcs_register_topologies;
		}
		pLocal->next_ptr = NULL;
		pLocal->topology_id = pTopoDefBase->topology_id;
		if(NULL == pTail)
		{
			pHead = pLocal;
			pTail = pLocal;
		}
		else
		{
			pTail->next_ptr = pLocal;
			pTail = pLocal;
		}

		//Advance to the next part of the payload. This will work even if num_modules is zero.
		pTemp += moduleInfoBufferSize;
		unNumberTopologiesInPayload--;
	}

	//If the code has reached this point, it means the payload is accurate. Topology addition can now proceed.
	unNumberTopologiesInPayload = pHeader->num_topologies;
	pTemp = (uint8_t *)pVirtAddr;
	while (unNumberTopologiesInPayload > 0)
	{
		//Get pTopoDefBase and pModDefBase.
		//If num_modules is zero, pModDefBase would be the same as pTopoDefBase.
		pTopoDefBase = (avcs_topology_definition_t *)(pTemp);
		pTemp += sizeof(avcs_topology_definition_t);
		pModDefBase = pTemp;

		//Debug prints
		uint32_t unTopoAreaBitMask = ((pTopoDefBase->topo_config) & (0x0000000f));
		MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: TopoVer: %lu, TopoID: %lx, TopoCfg: %lu, NumMods: %lu, Area: %lu, Topology #%lu",
				pTopoDefBase->version, pTopoDefBase->topology_id, pTopoDefBase->topo_config,
				pTopoDefBase->num_modules, unTopoAreaBitMask, (pHeader->num_topologies - unNumberTopologiesInPayload + 1));

		//Add single topology
		result = elite_cmn_topo_db_add_topology(pTopoDefBase, pModDefBase,ELITE_CMN_CUSTOM_TOPOLOGIES);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Failed to add topology into the common topology database.");
			goto _bailout_avcs_register_topologies;
		}

		//Advance to the next part of the payload. This will work even if num_modules is zero.
		pTemp += (sizeof(avcs_module_info_t)*pTopoDefBase->num_modules);
		unNumberTopologiesInPayload--;
	}

	_bailout_avcs_register_topologies:

	//Delete the local linked list
	topo_list *pCurr = pHead;
	while(NULL != pCurr)
	{
		topo_list *pNext = pCurr->next_ptr;
		qurt_elite_memory_free(pCurr);
		pCurr = pNext;
	}
	pHead = NULL;
	pTail = NULL;

	elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
	return result;
}

static ADSPResult AdspCoreSvc_AprDeRegisterTopologiesHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
  ADSPResult result = ADSP_EOK;
  avcs_cmd_deregister_topologies_t *pPayload = (avcs_cmd_deregister_topologies_t*)elite_apr_if_get_payload_ptr(pAprPacket);
  avcs_topology_buffer_t *pVirtAddr;
  uint8_t *pTemp;
  uint32_t unNumberTopologiesInPayload = 0;
  uint32_t buffer_size = 0;
  avcs_topology_buffer_t *pHeader;
  avcs_topology_deregister_t *pTopoDefBase;
  uint32_t *pPayloadBase;
  topo_list *pHead = NULL;
  topo_list *pTail = NULL;

  if (0 == pPayload->mem_map_handle)
  {
    //In-band case: avcs_topology_buffer_t starts just past the last entry of the incoming payload's header
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Processing in-band.");
    pPayloadBase = (uint32_t*)(pPayload + 1);
    pVirtAddr = (avcs_topology_buffer_t*)(pPayloadBase);
  }
  else
  {
    //Out-band case: avcs_topology_buffer_t starts at the address mentioned in the incoming payload's header
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Processing out-band.");
    uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
    uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

    //Check cache line alignment
    bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);
    if (!isAlignedToCacheLine)
    {
      result = ADSP_EBADPARAM;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Physical address is not aligned to cache line.");
      goto _bailout_avcs_deregister_topologies;
    }

    //Physical->Virtual
    elite_mem_shared_memory_map_t bufferMemNode;
    bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
    bufferMemNode.unMemMapClient = avcsMemoryMapClient;
    result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->buffer_size, &bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Failed to map physical memory.");
      goto _bailout_avcs_deregister_topologies;
    }

    // Since this buffer will be read, need to invalidate the cache.
    result = elite_mem_invalidate_cache(&bufferMemNode);
    if (ADSP_FAILED(result))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Failed to invalidate memory.");
      goto _bailout_avcs_deregister_topologies;
    }

    pVirtAddr = (avcs_topology_buffer_t*)(bufferMemNode.unVirtAddr);
  }

  switch(pPayload->mode)
  {
    case AVCS_CMD_DEREGISTER_SELECTED_TOPOLOGIES:

      //Size validation - part 1
      buffer_size = pPayload->buffer_size;
      if (buffer_size < sizeof(avcs_topology_buffer_t))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Size of deregister topologies buffer %lu bytes is insufficient.", buffer_size);
        result = ADSP_EBADPARAM;
        goto _bailout_avcs_deregister_topologies;
      }
      buffer_size -= sizeof(avcs_topology_buffer_t);

      //Num_topologies validation
      pHeader = (pVirtAddr);
      if (0 == pHeader->num_topologies || AVCS_MAX_NUMBER_TOPOLOGIES_IN_CMD < pHeader->num_topologies)
      {
        //Note: The API does not restrict the upper bound on the  number of topologies that may be deregistered in one command.
        //However, this boundary check is in place to protect against any memory corruption.
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Invalid NumTopos: %lu", pHeader->num_topologies);
        result = ADSP_EBADPARAM;
        goto _bailout_avcs_deregister_topologies;
      }

      //Get hold of #of topos in payload and advance to the next part of the payload.
      unNumberTopologiesInPayload = pHeader->num_topologies;
      pVirtAddr++;
      pTemp = (uint8_t *)pVirtAddr;

      //Iterate through all the topologies in the command.
      while (unNumberTopologiesInPayload > 0)
      {
        //Size validation - part 2
        if (buffer_size < sizeof(avcs_topology_deregister_t))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Size of deregister topologies buffer %lu bytes is insufficient to hold one topology definition.", buffer_size);
          result = ADSP_EBADPARAM;
          goto _bailout_avcs_deregister_topologies;
        }
        buffer_size -= sizeof(avcs_topology_deregister_t);

        //Get hold of the base of the avcs_topology_deregister_t structure.
        pTopoDefBase = (avcs_topology_deregister_t *)(pTemp);

        //2. The topology to be removed must exist in the database, i.e. must not be unique.
        if(ADSP_EOK != elite_cmn_topo_db_is_active_custom_topology_found(pTopoDefBase->topology_id))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Invalid TopoID: %lx is not found.", pTopoDefBase->topology_id);
          result = ADSP_EBADPARAM;
          goto _bailout_avcs_deregister_topologies;
        }

        //3. Unique topology ID validation (within this command's payload).
        //TODO: Instead of alloc'ing the whole linked list, I can store a bookkeeping variable and then search in the payload itself.
        if(FALSE == IsTopologyUnique(pTopoDefBase->topology_id, pHead))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Invalid TopoID: %lx is not unique (repeated within in this payload).", pTopoDefBase->topology_id);
          result = ADSP_EBADPARAM;
          goto _bailout_avcs_deregister_topologies;
        }

        //Store this topology_id in the local linked list for immediate comparison.
        topo_list *pLocal = (topo_list *)qurt_elite_memory_malloc(sizeof(topo_list), QURT_ELITE_HEAP_DEFAULT);
        if(NULL == pLocal)
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Memory allocation failure");
          result = ADSP_ENOMEMORY;
          goto _bailout_avcs_deregister_topologies;
        }
        pLocal->next_ptr = NULL;
        pLocal->topology_id = pTopoDefBase->topology_id;
        if(NULL == pTail)
        {
          pHead = pLocal;
          pTail = pLocal;
        }
        else
        {
          pTail->next_ptr = pLocal;
          pTail = pLocal;
        }
        pTemp += sizeof(avcs_topology_deregister_t);
        unNumberTopologiesInPayload--;
      }

      //If the code has reached this point, it means the payload is accurate. Topology removal can now proceed.
      unNumberTopologiesInPayload = pHeader->num_topologies;
      pTemp = (uint8_t *)pVirtAddr;
      while (unNumberTopologiesInPayload > 0)
      {
        //Get pTopoDefBase and pModDefBase.
        //If num_modules is zero, pModDefBase would be the same as pTopoDefBase.
        pTopoDefBase = (avcs_topology_deregister_t *)(pTemp);

        //Debug prints
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: TopoID: %lx, Topology #%lu",
              pTopoDefBase->topology_id, (pHeader->num_topologies - unNumberTopologiesInPayload + 1));

        //Deregister single topology
        result = elite_cmn_topo_db_deregister_single_topology(pTopoDefBase->topology_id);
        if (ADSP_FAILED(result))
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Failed to deregister topology from the common topology database.");
		  result = ADSP_EBADPARAM;
          goto _bailout_avcs_deregister_topologies;
        }

        //Advance to the next part of the payload. This will work even if num_modules is zero.
        pTemp += sizeof(avcs_topology_deregister_t);
        unNumberTopologiesInPayload--;
      }
      break;
    case AVCS_CMD_DEREGISTER_ALL_CUSTOM_TOPOLOGIES:
      result = elite_cmn_topo_db_deregister_all_custom_topologies();
      if (ADSP_FAILED(result))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Failed to deregister all custom topologies from the common topology database.");
		result = ADSP_EBADPARAM;
        goto _bailout_avcs_deregister_topologies;
      }
      break;
    default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_DEREGISTER_TOPOLOGIES: Unsupported operation mode, bailing out.");
      result = ADSP_EBADPARAM;
      goto _bailout_avcs_deregister_topologies;
  }

  _bailout_avcs_deregister_topologies:

  //Delete the local linked list
  topo_list *pCurr = pHead;
  while(NULL != pCurr)
  {
    topo_list *pNext = pCurr->next_ptr;
    qurt_elite_memory_free(pCurr);
    pCurr = pNext;
  }
  pHead = NULL;
  pTail = NULL;

  elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
  return result;
}

static bool_t IsModuleListUnique(avcs_topology_definition_t *topo_def_ptr, void *mod_def_ptr, uint32_t topo_version)
{
	if (topo_version == 0)
	{
		avcs_module_info_t *module_info_ptr = (avcs_module_info_t*)mod_def_ptr;
		// Check for duplicate module IDs
		for (uint32_t i = 0; i < topo_def_ptr->num_modules; i++)
		{
			for (uint32_t j = i + 1; j < topo_def_ptr->num_modules; j++)
			{
				if (module_info_ptr[j].module_id == module_info_ptr[i].module_id)
				{
					MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Error: Module ID 0x%lx is repeated in topology ID 0x%lx", module_info_ptr[i].module_id, topo_def_ptr->topology_id);
					return FALSE;
				}
			}
		}
	}
	else if (topo_version == 1)
	{
		avcs_module_instance_info_t *module_info_ptr = (avcs_module_instance_info_t*)mod_def_ptr;
		// Check for duplicate module ID - Instance ID pair
		for (uint32_t i = 0; i < topo_def_ptr->num_modules; i++)
		{
			for (uint32_t j = i + 1; j < topo_def_ptr->num_modules; j++)
			{
				if ((module_info_ptr[j].module_id == module_info_ptr[i].module_id) &&
						(module_info_ptr[j].instance_id == module_info_ptr[i].instance_id))
				{
					MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Error: Module ID 0x%lx, Instance ID 0x%lx is repeated in topology ID 0x%lx", module_info_ptr[i].module_id, module_info_ptr[i].instance_id,topo_def_ptr->topology_id);
					return FALSE;
				}
			}
		}
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Topology ID 0x%lx, Un-supported Version %lu.", topo_def_ptr->topology_id, topo_version);
		return FALSE;
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Topology ID 0x%lx has unique set of module IDs - instance IDs.", topo_def_ptr->topology_id);
	return TRUE;
}

static bool_t IsTopologyUnique(uint32_t unTopologyID, topo_list *pHead)
{
	if(NULL == pHead)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Topology ID 0x%lx is not repeated yet in this payload. It is unique.", unTopologyID);
		return TRUE;
	}
	else
	{
		topo_list *pCurr = pHead;
		while(NULL != pCurr)
		{
			if(pCurr->topology_id == unTopologyID)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Error: Topology ID 0x%lx has repeated within this payload. It is not unique.", unTopologyID);
				return FALSE;
			}
			else
			{
				pCurr=pCurr->next_ptr;
			}
		}
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REGISTER_TOPOLOGIES: Topology ID 0x%lx is not repeated yet in this payload. It is unique.", unTopologyID);
	return TRUE;
}


ADSPResult AdspCoreSvc_RegisterAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   avcs_register_amdb_module_t *pPayload = (avcs_register_amdb_module_t*)elite_apr_if_get_payload_ptr(pAprPacket);
   bool_t is_out_band = FALSE;
   void *data_ptr;
   elite_mem_shared_memory_map_t bufferMemNode={0};

   if ((pPayload->client_id == AVCS_AMDB_CLIENT_ID_NONE)||
         (pPayload->client_id == AVCS_AMDB_CLIENT_ID_ALL) )
   {
      result = ADSP_EUNSUPPORTED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_REGISTER_AMDB_MODULES: invalid client id %lu", pPayload->client_id);
      elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
      return result;
   }

   if (0 == pPayload->mem_map_handle)
   {
      //In-band case: data starts after avcs_register_amdb_module_t
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_REGISTER_AMDB_MODULES: Processing in-band.");
      data_ptr = (void*)(pPayload+1);
      is_out_band = FALSE;
   }
   else
   {
      //Out-band case: avcs_topology_buffer_t starts at the address mentioned in the incoming payload's header
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_REGISTER_AMDB_MODULES: Processing out-band.");
      uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
      uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

      //Check cache line alignment
      bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);
      if (!isAlignedToCacheLine)
      {
         result = ADSP_EBADPARAM;
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_REGISTER_AMDB_MODULES: Physical address is not aligned to cache line.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      //Physical->Virtual
      bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
      bufferMemNode.unMemMapClient = avcsMemoryMapClient;
      result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->size, &bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_REGISTER_AMDB_MODULES: Failed to map physical memory.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      // Since this buffer will be read, need to invalidate the cache.
      result = elite_mem_invalidate_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_REGISTER_AMDB_MODULES: Failed to invalidate memory.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      data_ptr = (void*)bufferMemNode.unVirtAddr;
      is_out_band = TRUE;
   }

   result = mm_register_modules_from_reg_api(data_ptr, pPayload->size, is_out_band, pPayload->client_id);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_REGISTER_AMDB_MODULES: mm_register_modules_from_reg_api returned %lu.", result);

   //result per module is written in the shared mem. need to flush cache.
   if(is_out_band)
   {
      result = elite_mem_flush_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_REGISTER_AMDB_MODULES: Failed to flush cache memory.");
      }
   }

   elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
   return result;
}

ADSPResult AdspCoreSvc_DeregisterAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   avcs_deregister_amdb_module_t *pPayload = (avcs_deregister_amdb_module_t*)elite_apr_if_get_payload_ptr(pAprPacket);
   bool_t is_out_band = FALSE;
   void *data_ptr;
   elite_mem_shared_memory_map_t bufferMemNode={0};

   if ((pPayload->client_id == AVCS_AMDB_CLIENT_ID_NONE)||
         (pPayload->client_id == AVCS_AMDB_CLIENT_ID_ALL) )
   {
      result = ADSP_EUNSUPPORTED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: invalid client id %lu", pPayload->client_id);
      elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
      return result;
   }

   if (0 == pPayload->mem_map_handle)
   {
      //In-band case: data starts after avcs_register_amdb_module_t
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Processing in-band.");
      data_ptr = (void*)(pPayload+1);
      is_out_band = FALSE;
   }
   else
   {
      //Out-band case: avcs_topology_buffer_t starts at the address mentioned in the incoming payload's header
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Processing out-band.");
      uint32_t phyAddrLsw = (uint32_t)(pPayload->data_payload_addr_lsw);
      uint32_t phyAddrMsw = (uint32_t)(pPayload->data_payload_addr_msw);

      //Check cache line alignment
      bool_t isAlignedToCacheLine = ((phyAddrLsw & CACHE_ALIGNMENT_MASK) == 0);
      if (!isAlignedToCacheLine)
      {
         result = ADSP_EBADPARAM;
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Physical address is not aligned to cache line.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      //Physical->Virtual
      bufferMemNode.unMemMapHandle = pPayload->mem_map_handle;
      bufferMemNode.unMemMapClient = avcsMemoryMapClient;
      result =  elite_mem_map_get_shm_attrib(phyAddrLsw, phyAddrMsw, pPayload->size, &bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Failed to map physical memory.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      // Since this buffer will be read, need to invalidate the cache.
      result = elite_mem_invalidate_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Failed to invalidate memory.");
         elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
         return result;
      }

      data_ptr = (void*)bufferMemNode.unVirtAddr;
      is_out_band = TRUE;
   }
   result = mm_remove_modules_from_dereg_api(data_ptr, pPayload->size, is_out_band, pPayload->client_id);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: mm_remove_modules_from_dereg_api returned %lu.", result);

   //result per module is written in the shared mem. need to flush cache.
   if(is_out_band)
   {
      result = elite_mem_flush_cache(&bufferMemNode);
      if (ADSP_FAILED(result))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_DEREGISTER_AMDB_MODULES: Failed to flush cache memory.");
      }
   }
   elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
   return result;
}

ADSPResult AdspCoreSvc_PrintAllAmdbModules(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   avcs_print_amdb_modules_t *pPayload = (avcs_print_amdb_modules_t*)elite_apr_if_get_payload_ptr(pAprPacket);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_PRINT_ALL_MODULES: called for client-id = %lu (client-id based implementation not done yet)", pPayload->client_id);

   if (pPayload->client_id == AVCS_AMDB_CLIENT_ID_NONE)
   {
      result = ADSP_EUNSUPPORTED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVCS_AMDB_PRINT_ALL_MODULES: invalid client id %lu", pPayload->client_id);
      elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);
      return result;
   }

   //TODO: to implement client-id here.
   adsp_amdb_print_all_capi_v2();

   elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);

   return result;
}

static ADSPResult AdspCoreSvc_AmdbReset(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   ADSPResult result = ADSP_EOK;
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_RESET: called ");

   //assumption is that this API is not called when modules are being used.
   //behave like what happens during boot-up
   adsp_amdb_deinit();
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_RESET: deinit AMDB");

   adsp_amdb_init();
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_RESET: init AMDB");

   //then load static modules
   result = mm_register_all_static_modules();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_RESET: adding all static modules. result %d ", result);

   //then load all built-ins based on ACDB file.
   result = mm_register_modules_based_on_acdb();
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_AMDB_RESET: registering all based on ACDB file. result %d ", result);

   elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);

   return result;
}

static ADSPResult AdspCoreSvc_AddRemovePoolPages(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket, bool_t is_add)
{
   ADSPResult result = ADSP_EOK;
   uint32_t pool_id, size;
   uint32_t addr_lsw = 0, addr_msw = 0;
   QURT_ELITE_MEMORYPOOLTYPE qurt_elite_pool_id;
   if (is_add)
   {
      avcs_cmd_add_pool_pages_t *pPayload = (avcs_cmd_add_pool_pages_t*)elite_apr_if_get_payload_ptr(pAprPacket);
      pool_id = pPayload->pool_id;
      size = pPayload->size;
      addr_lsw = pPayload->phy_addr_lsw;
      addr_msw = pPayload->phy_addr_msw;
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_ADD_POOL_PAGES : pool_id %lu, size %lu addr = 0x%lx%lx", pool_id, size, addr_msw, addr_lsw);
   }
   else
   {
      avcs_cmd_remove_pool_pages_t *pPayload = (avcs_cmd_remove_pool_pages_t*)elite_apr_if_get_payload_ptr(pAprPacket);
      pool_id = pPayload->pool_id;
      size = pPayload->size;
      addr_lsw = pPayload->phy_addr_lsw;
      addr_msw = pPayload->phy_addr_msw;
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_REMOVE_POOL_PAGES : pool_id %lu, size %lu addr = 0x%lx%lx", pool_id, size, addr_msw, addr_lsw);
   }

   switch(pool_id)
   {
   case ADSP_MEMORY_MAP_HLOS_PHYSPOOL:
   {
      qurt_elite_pool_id = QURT_ELITE_MEMORYMAP_HLOS_PHYSPOOL;
      break;
   }
   default:
   {
      result = ADSP_EUNSUPPORTED;
      goto _bailout;
   }
   }
   result = qurt_elite_memorymap_add_remove_pool_page(is_add, qurt_elite_pool_id, size, ( ((uint64_t)addr_msw) << 32 ) | addr_lsw);

_bailout:
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVCS_CMD_ADD/REMOVE_POOL_PAGES: result = %lu", result);

   elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, result);

   return result;
}
