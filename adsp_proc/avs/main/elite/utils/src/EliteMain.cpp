/**
@file EliteMain.cpp

@brief This file  contains the implementation for Elite Fwk high level APIs

*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/31/16   mk        Added elite thread prio init changes
02/18/11   mspk      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2013-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
-----------------------------------------------------------------------*/


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "EliteMem.h"
#include "Elite.h"
#include "EliteMain.h"
#include "AdspCoreSvc.h"
#ifndef MDSPMODE
#include "q6_memory_defns.h"
#endif
#include "EliteTopology_db.h"
#include "EliteCmnTopology_db.h"

#include "module_mgr.h"
#include "adsp_amdb_static.h"
#include "inter_module_comm_server.h"

#ifdef AUDTST_ELITE_ENCDEC_SVC
#include "AudencdecTstLauncher.h"
#endif
#if defined(AFE_TEST) || defined(AFE_DAL_TEST)
#include "AFETstLauncher.h"
#endif // #if defined(AFE_TEST) || defined(AFE_DAL_TEST)
#ifdef LISTEN_SVC_TEST
#include "ListenStreamMgrSvcTest.h"
#endif // LISTEN_SVC_TEST
#ifdef ADSPCORESVCTEST
#include "AdspCoreSvcTst.h"
#endif //ADSPCORESVCTEST
#ifdef AUDPPSVC_TEST
#include "audproctst_svc.h"
#endif // AUDPPSVC_TEST
#include "AudDynaPPSvc.h"

#include "AudioStreamMgr.h"
#include "ListenStreamMgr.h"
#if 1
#include "vocsvc.h"
#include "VoiceStreamMgr.h"
#include "VoiceMixerSvc.h"
#include "VoiceTimer.h"
#endif
#include "AFEDevService.h"
#include "afe_main.h"

#include "voice_proc_mgr.h"

//Audio PD restart
#include "pd_audio_restart.h"

/* Audio */
#include "aprv2_api_inline.h"
#include "apr_errcodes.h"
#include "aprv2_msg_if.h"

#ifndef MDSPMODE
#include "adsp_err_fatal.h"
#endif


/* =======================================================================
**                          Macro definitions
** ======================================================================= */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

#ifdef ADSP_TEST_ENTRY
extern "C" int adsp_test_entry(int argc, char **argv);
#endif

#if defined(SIM)
#include "EliteLoggingUtils_i.h"
#include "DALSys.h"
#ifdef MDSPMODE
   #include "time_svc.h"
   #include "timer.h"
#endif
extern "C" void timer_client_init(void);
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
void MtMxAFETest(void);
ADSPResult  AudCreateStatAudDevMgr(uint32 dummy, void** ppHAfeDevMgr);
#ifndef MDSPMODE
ADSPResult usm_create(uint32_t inputParam, void **handle);
#endif
#ifdef __cplusplus
}
#endif

#define NUM_ELEM(a) sizeof(a)/sizeof((a)[0])

/* =======================================================================
**                          Global Variable Definitions
** ======================================================================= */
//Stubbing out voice for rel/avs2.0. Temporary change until voice is ready

elite_svc_handle_t *txVoiceMatrix;
elite_svc_handle_t *rxVoiceMatrix;
VoiceTimer_t *voiceTimer; // Voice Timer

elite_svc_handle_t *adspCoreSvc;

// A list of all Elite static services.
// This is used for creation and destruction of static services.
#ifdef AUDIOFW_8200A
static ADSPResult (* audStatSvcCreate[]) (uint32, void **)  = {
    aud_create_stat_afe_svc,
    AdspCoreSvc_Create
    };
#else // for other targets like 8660, 8960, 8974 etc
static ADSPResult (* audStatSvcCreate[]) (uint32, void **)  = {
    voice_proc_mgr_create,
    VoiceStreamMgr_Create,
    aud_create_stat_afe_svc,
    AudCreateStatAudDevMgr,
    AudioStreamMgr_Create,    //  creation of playback service
    aud_create_stat_lsm_svc,  //Always on Listen stream manager Svc ,
  #ifndef MDSPMODE
	usm_create,
  #endif
    AdspCoreSvc_Create
    };
#endif

static const int32_t NUM_ELITE_STAT_SVC = NUM_ELEM(audStatSvcCreate);
static elite_svc_handle_t *audStatSvcList[NUM_ELITE_STAT_SVC];

/**
  Queue definitions for starting static services
*/
#define ELITEMAIN_MAX_CMD_Q_ELEMENTS  8
#define ELITEMAIN_CMD_Q_SIZE_IN_BYTES (QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(ELITEMAIN_MAX_CMD_Q_ELEMENTS))

/**
  Diag Trace buffer for audio PD
*/
#ifdef ENABLE_AUDIO_PD_F3_TRACE
extern "C" boolean diag_lsm_f3_trace_init(uint8_t *address, unsigned int length);
extern "C" boolean diag_lsm_f3_trace_deinit(void);
#define DIAG_F3_TRACE_BUFFER_SIZE_AUDIO_PD 102400
uint8_t audioPDF3TraceBuf[DIAG_F3_TRACE_BUFFER_SIZE_AUDIO_PD] = {0};
#endif

/* =======================================================================
**                          Functions
** ======================================================================= */
static int CreateEliteStaticService( void )
{
   int i, nRes = ADSP_EOK;
   uint32 ulDummy = 0;



   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "CreateEliteStaticService Begin");

//Stubbing out voice for rel/avs2.0. Temporary change until voice is ready
#if 1
   VMtMxParams_t voicematrixparams;    

    if ( ADSP_FAILED( nRes = VoiceTimerCreate(ulDummy, (void**) (&voiceTimer) ) ) )
    {
       MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Fail to create Voice Timer");
       return nRes;
    }

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating Voice Timer %d",
         qurt_elite_globalstate.heap_stats.curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

    voicematrixparams.outPortsFrameSize = 20;
    voicematrixparams.maxInPorts = 8;
    voicematrixparams.maxOutPorts = 8;

    if ( ADSP_FAILED( nRes = CreateVoiceMatrixMixerSvc(ulDummy, (void**) (&rxVoiceMatrix), &voicematrixparams,VMX_RX_INSTANCE_ID ) ) )
    {
       MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create Voice Rx matrix service");
       return nRes;
    }

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating Voice RX matrix %d",
         qurt_elite_globalstate.heap_stats.curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */


    if ( ADSP_FAILED( nRes = CreateVoiceMatrixMixerSvc(ulDummy, (void**) (&txVoiceMatrix), &voicematrixparams,VMX_TX_INSTANCE_ID ) ) )
    {
       MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create Voice Tx matrix service");
       return nRes;
    }

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating Voice Tx matrix %d",
         qurt_elite_globalstate.heap_stats.curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */
#endif

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating ACS %d",
         qurt_elite_globalstate.heap_stats.curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */


   /// For demo, create static service for demo here
   for ( i = 0; i < NUM_ELITE_STAT_SVC; i ++ ) {
     //  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Creating Static Svc pFn(%x)", audStatSvcCreate[i]);
       if ( ADSP_FAILED( nRes = audStatSvcCreate[i](ulDummy, (void**) (&audStatSvcList[i]) ) ) ) {
           MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Fail to create static service %d", i);
           return nRes;
       }

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
       MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating static service %d: %d", i,
             qurt_elite_globalstate.heap_stats.curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

   }

   MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "CreateEliteStaticService End");

   return nRes;
}



static int StartEliteStaticService(void)
{
    int i, nRes=ADSP_EOK;
    uint32 unSize;
    elite_msg_any_t anyMsg;
    int32 readyMask = 0, waitMask = 0;
    qurt_elite_channel_t rChannel;
    uint32           unChannelBitField;
    char queue_name[]="EMAIN";
    QURT_ELITE_ALIGN(char responseQBuf[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(ELITEMAIN_CMD_Q_SIZE_IN_BYTES)], 8);
    qurt_elite_queue_t  *pResponseQ = (qurt_elite_queue_t*)responseQBuf;

    qurt_elite_channel_init(&rChannel);

    if (ADSP_FAILED(nRes= qurt_elite_queue_init(queue_name, ELITEMAIN_MAX_CMD_Q_ELEMENTS, pResponseQ))
        || ADSP_FAILED(nRes= qurt_elite_channel_addq(&rChannel,pResponseQ,0))
        )
    {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to create or add queues to the channel while starting static service,error code %d",nRes);
        goto StartEliteStaticService_cleanup;
    }


    MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "StartEliteStaticService start");

    for ( i = 0 ; i < NUM_ELITE_STAT_SVC; i ++ )
    {
        if (NULL != audStatSvcList[i])
        {
            waitMask |= (1<<i);

            // Compose a command to start services.
            unSize = sizeof(elite_msg_cmd_start_svc_t);
            if ( ADSP_FAILED( nRes = elite_msg_create_msg( &anyMsg, &unSize,
                                                                      ELITE_CMD_START_SERVICE, pResponseQ, i, 0) ) ) {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to create msg, result 0x%8x\n",nRes);
               goto StartEliteStaticService_cleanup;
            }

            // Send the START command to static services.
            if (ADSP_FAILED(nRes = qurt_elite_queue_push_back( audStatSvcList[i]->cmdQ, (uint64*)&anyMsg  ) ))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to deliver msg to service, result 0x%8x\n",nRes);
               goto StartEliteStaticService_cleanup;
            }
        }
	    else
	    {
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Static Svc: index 0x%x Not Started, Might be stubbed out", i);
		}
    }

    /* Wait until all static services ACK back. */
    unChannelBitField = qurt_elite_queue_get_channel_bit(pResponseQ);
    while ( readyMask != waitMask )
    {
      MSG_1( MSG_SSID_QDSP6, DBG_MED_PRIO, "Static Svc start Mask = %ld\n",readyMask );

      /* wait for signal to come */
      qurt_elite_channel_wait( &rChannel, unChannelBitField );

      /* Enter forever loop */
      for (;; )
      {
         int32 res = qurt_elite_queue_pop_front(pResponseQ, (uint64*)(&anyMsg));
         /* break out of loop if there are no more messages */
         if (res == ADSP_ENEEDMORE) break;

         elite_msg_cmd_start_svc_t *pPayload = (elite_msg_cmd_start_svc_t*) anyMsg.pPayload;
         MSG_2( MSG_SSID_QDSP6, DBG_MED_PRIO, "Static Svc %lu start status %lu\n",
                   pPayload->unClientToken, pPayload->unResponseResult );
         readyMask |= ( 0x01 << (pPayload->unClientToken) );

         // Now release msg back to qurt_elite_buf_mgr
         elite_msg_release_msg(&anyMsg);

      } // for (;;);
    } // while ( readyMask != waitMask )

StartEliteStaticService_cleanup:
    elite_svc_deinit_cmd_queue(pResponseQ);
    qurt_elite_channel_destroy(&rChannel);

    MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "StartEliteStaticService end with result %d",nRes);
    return nRes;
}


static int DestroyEliteStaticService(void)
{
    int i, nThreadId, nRes=ADSP_EOK;
    elite_msg_any_t anyMsg;
    uint32 unSize;

    MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "DestroyEliteStaticService start");

    for ( i = 0 ; i < NUM_ELITE_STAT_SVC; i ++ )
    {

		if (audStatSvcList[i])
		{
			// Save thread id to avoid the race condition that
			// the thread is closed and qurt_elite_thread_join
			// use an invalide pointer.
			nThreadId = audStatSvcList[i]->threadId;

			unSize = sizeof(elite_msg_cmd_destroy_svc_t);
			// Compose a command to close services.
			if ( ADSP_FAILED( nRes = elite_msg_create_msg( &anyMsg, &unSize,
																	  ELITE_CMD_DESTROY_SERVICE, NULL, 0, 0) ) ) {
			   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to create msg, result 0x%8x\n",nRes);
			   return ADSP_EFAILED;
			}

			// Send the close command to AudStatPcmSvc
			nRes = qurt_elite_queue_push_back( audStatSvcList[i]->cmdQ, (uint64*)&anyMsg  );

			if (ADSP_FAILED(nRes))
			{
			   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to deliver msg to service, result 0x%8x\n",nRes);
			   return ADSP_EFAILED;
			}

			// Wait here until service destruction is complete.
			qurt_elite_thread_join(nThreadId, &nRes);

			if (ADSP_FAILED(nRes))
			{
			   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to join thread: 0x%8x\n",nRes);
			   return nRes;
			}
	    }
		else
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Static Svc: index 0x%x Not Destroyed as it is not created", i);
		}
    }

    MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "DestroyEliteStaticService end");
    return nRes;
}

ADSPResult elite_framework_init(void)
{
    ADSPResult result_elite_mem_init, result_amdb_init, result_imc;
    int32_t result_apr_init;
#ifdef ENABLE_AUDIO_PD_F3_TRACE
    if (TRUE != diag_lsm_f3_trace_init(audioPDF3TraceBuf, DIAG_F3_TRACE_BUFFER_SIZE_AUDIO_PD))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"diag_lsm_f3_trace_init(%lx,%d) failed", 
                audioPDF3TraceBuf, DIAG_F3_TRACE_BUFFER_SIZE_AUDIO_PD);
    }
#endif

    // Init ADSP Fatal Error to force crash by registering as a client
	#ifndef MDSPMODE
    Adspfatalerr_Init_Client();
	#endif

    // Init global state structure
    qurt_elite_globalstate_init();

    //Initialize LPA
    ADSPResult result_lpainit = lpa_init();
    if (ADSP_EOK != result_lpainit) {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to initialize LPA with result = %d", result_lpainit);
        return result_lpainit;
    }

    /* Initialize elite thread priority */
    ADSPResult result_thrdprioinit = elite_thrd_prio_init();
    if (ADSP_EOK != result_thrdprioinit)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to initialize elite thread prio result = %d", result_thrdprioinit);
        return result_thrdprioinit;
    }

 #ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after global state init %d",
          qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);
 #endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

    result_elite_mem_init = elite_mem_init();
    // Init Elite global memory managment
    if ( ADSP_FAILED( result_elite_mem_init)) {
        MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init Elite global memory pool with result %d",result_elite_mem_init);
        return ADSP_EFAILED;
    }

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after elite_mem_init() %d",
          qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

    result_amdb_init = adsp_amdb_init();
    if ( result_amdb_init != APR_EOK ) {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init amdb with status %ld ",result_amdb_init);
      return ADSP_EFAILED;
    }

    /// Init apr infrasture
    result_apr_init = apr_init();
    if ( result_apr_init != APR_EOK ) {

       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init apr with status %ld ",result_apr_init);
       return ADSP_EFAILED;
    }
#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after APR init %d",
          qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

   // Init AFE
   if(ADSP_FAILED(afe_main_init()))
   {
     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE main init failed!");
     return ADSP_EFAILED;
   }

    //intialize IMC service used for inter module communication.
    result_imc = imc_init();
    if (ADSP_FAILED(result_imc))
    {
       MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to initialize intermodule communication server!");
       return ADSP_EFAILED;
    }

    return ADSP_EOK;
}

ADSPResult elite_framework_start (void)
{
   ADSPResult result_add_static=mm_register_all_static_modules();
   if (ADSP_FAILED(result_add_static))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Register static modules error: %ld", result_add_static);
      return result_add_static;
   }

	//Initialize the common global topology database
	ADSPResult result =elite_cmn_db_init();
	if (ADSP_FAILED(result))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Common global topo db init: elite_cmn_db_init failed with result: %ld", result);
		return result;
	}

	//Initialize the old topology database (for backward compatibility)
	ADSPResult result_pp_db_init = Elite_InitializeDataBases();
	if (ADSP_FAILED(result_pp_db_init))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Elite_InitializeDataBases failed with status %ld", result_pp_db_init);
             return result_pp_db_init;
	}

    ADSPResult ResultCreateStatSvc = CreateEliteStaticService();
    // Create static service
    if(ADSP_FAILED(ResultCreateStatSvc)){
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failure when creating Elite Static Service with status %d",ResultCreateStatSvc);
		return ADSP_EFAILED;
	}

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after creating static services %d",
          qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

    // start static service
	ADSPResult ResultStartStatSvc = StartEliteStaticService();
    if((ADSP_EOK != ResultStartStatSvc)&&(ADSP_ENEEDMORE != ResultStartStatSvc)){
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failure when starting Elite Static Service with status %d",ResultStartStatSvc);
		return ADSP_EFAILED;
	}
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Heap use after stat svc start %lu",
                  qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);

#ifdef QURT_ELITE_DBG_HEAP_CONSUMPTION
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HEAPUSE after starting static services %d",
          qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);
#endif /* QURT_ELITE_DBG_HEAP_CONSUMPTION */

    // Initialize CVD 
    MSG( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "====== Initializing CVD ======" );
    ( void ) vocsvc_call_adsp( VOCSVC_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) vocsvc_call_adsp( VOCSVC_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
    
    // set service status - up and ready.
    qurt_elite_globalstate.uSvcUpStatus = 1;

#if defined(SIM)
    ADSPResult res = elite_sim_init_file_logging();
    if (ADSP_FAILED(res))
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "File logging init failed with %d",res);
    }
#endif

    {

       //Disable elite_framework_unit_tsts if PCM flag is passed in env
#ifndef SIMBOOT
       elite_framework_unit_tsts();
       //removing check based on return value. This causing failed sim tests to hang.
#endif
    }
    //Register for audio pd restart. Note that we are not caring about the return code for now.
#ifndef MDSPMODE //ToDo: this is temp change to compile AVS.2.8 for 9x65 target. will be removed by compiling stub for pd_restart.
    audio_pd_restart_register();
#endif
    return ADSP_EOK;
}

ADSPResult elite_framework_stop(void)
{
	//Deregister audio pd restart. Note that we are not caring about the return code for now.
#ifndef MDSPMODE //ToDo: this is temp change to compile AVS.2.8 for 9x65 target. will be removed by compiling stub for pd_restart.
	audio_pd_restart_deregister();
#endif	
    // De-initialize CVD
    MSG( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "====== Deinitializing CVD ======" );
    ( void ) vocsvc_call_adsp( VOCSVC_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
    ( void ) vocsvc_call_adsp( VOCSVC_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    MSG( MSG_SSID_QDSP6, DBG_HIGH_PRIO, "====== Deinitializing Static Service ======" );
    ADSPResult ResultDestroyStatSvc = DestroyEliteStaticService();
    // Destroy static service
    if(ADSP_FAILED(ResultDestroyStatSvc)){
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failure when destroying Elite Static Service with status %d",ResultDestroyStatSvc);
		return ADSP_EFAILED;
	}

    Elite_DeinitializeDataBases();
	elite_cmn_db_deinit();
    return ADSP_EOK;
}

ADSPResult elite_framework_deinit(void)
{
    int32_t result_apr_deinit;

    //inter module communication deinit
    imc_deinit();

    result_apr_deinit = apr_deinit();
    if ( result_apr_deinit != APR_EOK )
    {

      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init apr with status %ld ",result_apr_deinit);
      return ADSP_EFAILED;
   }

    // Deinitialize amdb
    adsp_amdb_deinit();

    // clean up Elite global memory pool
    elite_mem_destroy();

    // clean up global state structure
    qurt_elite_globalstate_deinit();

    //Deinitialize LPA
    lpa_deinit();

    // De-init AFE
    afe_main_deinit();

    // De-init ADSP Error fatal client
	#ifndef MDSPMODE
    Adspfatalerr_Deinit_Client();
	#endif

#ifdef ENABLE_AUDIO_PD_F3_TRACE
   if (TRUE != diag_lsm_f3_trace_deinit())
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"diag_lsm_f3_trace_deinit() Failed");
    }
#endif   
   MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"elite framework de-inited");
    return ADSP_EOK;
}

#ifndef SIMBOOT
int elite_framework_unit_tsts()
{
   int bTestsFailed = 0;

#ifdef ADSP_TEST_ENTRY

   bTestsFailed = adsp_test_entry(0, NULL);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Heap use after test entry %lu",
         qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap);

#elif defined(SIM)

#ifdef AUDPPSVC_TEST
   bTestsFailed |= AudioPPTstLauncher();
#endif // AUDPPSVC_TEST

#ifdef AUDTST_ELITE_ENCDEC_SVC
   bTestsFailed |= AudencdecTstLauncher();
#endif

#if defined(AFE_TEST) || defined(AFE_DAL_TEST)
   bTestsFailed |= afe_tst_launcher();
#endif // #if defined(AFE_TEST) || defined(AFE_DAL_TEST)

#ifdef LISTEN_SVC_TEST
   bTestsFailed |=listen_unit_test_launcher();
#endif //LISTEN_SVC_TEST

#ifdef USND_TEST
   bTestsFailed |= usnd_unit_test_launcher();
#endif // USND_TEST

#endif // !ADSP_TEST_ENTRY && defined SIM

   return bTestsFailed;
}
int elite_framework_sim_execution(void)
{
#ifdef SIM
	DALSYS_InitMod(NULL);

	if(ADSP_EOK != elite_framework_init())
	{
		MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Elite Framework Init Failed");
		return ADSP_EFAILED;

	}
#ifdef MDSPMODE
    
    int32_t result_apr_init = apr_init();
    if ( result_apr_init != APR_EOK ) {

       MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "FAILED to init apr with status %ld ",result_apr_init);
       return ADSP_EFAILED;
    }

	/// ATS Timer initializations
	timer_init();
	ADSPResult ATS_result;
	static char ATSThreadName[] = "ATS_Timer";
	static char pATSTimerStack[4096];
	qurt_elite_thread_t Tid;
	
	if (ADSP_FAILED (ATS_result = qurt_elite_thread_launch(&Tid,ATSThreadName, pATSTimerStack, 4096, 250, timer_task, NULL,QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "ATS Timer: Thread launch failed, result = %d", ATS_result);
		return ATS_result;
	}
#endif

	if(ADSP_EOK != elite_framework_start())
	{

		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Elite Framework Start Failed");
		return ADSP_EFAILED;
	}

	elite_framework_stop();

	elite_framework_deinit();
#endif

	return ADSP_EOK;
}
#endif