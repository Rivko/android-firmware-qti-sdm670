/*========================================================================
   This file Afe static service code and is an entry point for device service
  layer.

  Copyright (c) 2009-2012,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEDevService.cpp#2 $
 ====================================================================== */

/*==========================================================================
 Include files
  ========================================================================== */
#include "AFEStaticSvc.h"
#include "AFEInterface.h"
#include "Elite.h"
#include "AFEAanc.h"
#include "AFELoggingUtils.h"
#include "aprv2_ids_services.h"

static ADSPResult afe_svc_destroy (void *pAudStatAfeSvc, elite_msg_any_t* pMsg);
static ADSPResult afe_svc_start (void *pAudStatAfeSvc, elite_msg_any_t* pMsg);
static int afe_svc_work_loop (void* pAudStatAfeSvc);
/*==========================================================================
  Globals
  ========================================================================== */
static elite_svc_msg_handler_func afe_stat_serv_handler_ptr[] =
{
    afe_svc_custom_msg_handler,        //0 - ELITE_CUSTOM_MSG
    afe_svc_start,                     //1 - ELITE_CMD_START_SERVICE
    afe_svc_destroy,                   //2 - ELITE_CMD_DESTROY_SERVICE
    elite_svc_unsupported,             //3 - ELITE_CMD_CONNECT
    elite_svc_unsupported,             //4 - ELITE_CMD_DISCONNECT
    elite_svc_unsupported,             //5 - ELITE_CMD_PAUSE
    elite_svc_unsupported,             //6 - ELITE_CMD_RESUME
    elite_svc_unsupported,             //7 - ELITE_CMD_FLUSH
    elite_svc_unsupported,             //8 - ELITE_CMD_SET_PARAM
    elite_svc_unsupported,             //9 - ELITE_CMD_GET_PARAM
    elite_svc_unsupported,             //A - ELITE_DATA_BUFFER
    elite_svc_unsupported,             //B - ELITE_DATA_MEDIA_TYPE
    elite_svc_unsupported,             //C - ELITE_DATA_EOS
    elite_svc_unsupported,             //D - ELITE_DATA_RAW_BUFFER
    elite_svc_unsupported,             //E - ELITE_CMD_STOP_SERVICE
    afe_svc_apr_msg_handler,           //F - ELITE_APR_PACKET
};

/*==========================================================================
  Functions
  ========================================================================== */

/**
 * This function returns the AFE service version
 *
 * @param[out] svc_info_ptr pointer to the service info structure
 * @return error code
 */
ADSPResult afe_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_AFE_V;
  svc_info_ptr->api_version = AFE_API_VERSION;
  svc_info_ptr->api_branch_version = AFE_API_BRANCH_VERSION;

  return ADSP_EOK;
}

/**
 * This function lauch the  Afe device static service thread
 *
 * @param[in] none
 * @return none
 */
ADSPResult aud_create_stat_afe_svc(uint32_t dummy, void** ppHAfeDevService)
{
  ADSPResult                  result;
  aud_stat_afe_svc_t          *pAudStatAfeSvc;
  elite_svc_handle_t          *pHAfeDevService;
  qurt_elite_queue_t          *pTemp;
  char                        devServCmdQName[] = AFE_DEVICE_SERVICE_CMDQ_NAME;
  char                        devServRspQName[] = AFE_DEVICE_SERVICE_RSPQ_NAME;

  //Assign service handle pointer with Afe static service object
  pAudStatAfeSvc       = &afe_stat_svc_object_global;

  // This variable afe_svc_global_ptr
  afe_svc_global_ptr   = pAudStatAfeSvc;

  pHAfeDevService      = &(pAudStatAfeSvc->afe_service_handle);
  *ppHAfeDevService    = pHAfeDevService;

  //init static service channel
  qurt_elite_channel_init(&(pAudStatAfeSvc->channel));

  // Create the cmdQ.
  pTemp =(qurt_elite_queue_t *)afe_cmdQBuf;
  if (ADSP_FAILED(result = (qurt_elite_queue_init(devServCmdQName, AFE_SVC_MAX_CMD_Q_ELEMENTS,pTemp ))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDevServ: commandQ creation failed, result: 0x%lx", result);
    qurt_elite_globalstate.pAfeStatSvcCmdQ = NULL;
    return result;
  }
  //After success of adding the Q to channel update global Q ptr
  qurt_elite_globalstate.pAfeStatSvcCmdQ        = pTemp;

  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(pAudStatAfeSvc->channel), pTemp,0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDevServ: failed to add commandQ to channel, result: 0x%lx", result);
    return result;
  }
  pHAfeDevService->cmdQ                     = qurt_elite_globalstate.pAfeStatSvcCmdQ;
  afe_svc_global_ptr->static_svc_cmdq_ptr   = qurt_elite_globalstate.pAfeStatSvcCmdQ;

  //Create the rspQ.
  pTemp =(qurt_elite_queue_t *)afe_rspQBuf;
  if (ADSP_FAILED(result = (qurt_elite_queue_init(devServRspQName, AFE_SVC_MAX_RSP_Q_ELEMENTS,pTemp ))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDevServ: responseQ creation failed, result: 0x%lx", result);
    return result;
  }
  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(pAudStatAfeSvc->channel), pTemp,0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDevServ: failed to add responseQ to channel, result: 0x%lx", result);
    return result;
  }
  afe_svc_global_ptr->static_svc_rspq_ptr   = pTemp;

  // Launch the thread.
  if (ADSP_FAILED (result = qurt_elite_thread_launch(&(pHAfeDevService->threadId), AFE_STATIC_THREAD_NAME, \
                                                     NULL, AFE_THREAD_STACK_SIZE, elite_get_thrd_prio(ELITETHREAD_STAT_AFE_DEV_SVC_PRIO_ID),
                                                     afe_svc_work_loop, (void*) pAudStatAfeSvc,
                                                     QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEDevServ:Failed to create Port Threads");
  }

  return result;
}


/**
 * This function in the entry point of the static Afe service.
 * Static services acts as an unified entry point to all afe services
 * Afe static service handles all hardware port commands,
 * delegates pseudoport & VFR commands
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @return the ADSP_EOK error code
 */
static int afe_svc_work_loop (void* pAudStatAfeSvc)
{
  uint32_t                   unCurrentBitfield, unChannelStatus;
  aud_stat_afe_svc_t         *pAfeSvc;
  elite_svc_handle_t         *pHDevService;
  qurt_elite_channel_t            *pDevServChannel;
  ADSPResult                 result;

  pAfeSvc           = (aud_stat_afe_svc_t*)pAudStatAfeSvc;
  pHDevService      = &(pAfeSvc->afe_service_handle);
  pDevServChannel   = &(pAfeSvc->channel);

  // register with qurt_elite memory map.
  qurt_elite_memorymap_register(&(pAfeSvc->afe_memory_map_client));

  // Init the device driver and all ports.
  afe_device_driver_init();

  afe_dev_port_init(pAfeSvc);

  // Create static thread for Adaptive ANC algorithm
  afe_create_aanc_handler_svc(pAfeSvc);

  afe_bgt_pool_init();

  // save for mask bitfields
  unCurrentBitfield = qurt_elite_queue_get_channel_bit(pAfeSvc->static_svc_cmdq_ptr);

  // Enter forever loop to handle commands and ack
  for(;;)
  {
    // block on any one or more of selected queues to get a msg
    unChannelStatus = qurt_elite_channel_wait(pDevServChannel, unCurrentBitfield) ;

    // Check if commands came in
    if (unChannelStatus & qurt_elite_queue_get_channel_bit(pAfeSvc->static_svc_cmdq_ptr))
    {
      // call common utility to process command queue in standard way.
      const uint32_t afe_stat_serv_cmd_table_size = sizeof(afe_stat_serv_handler_ptr)/sizeof(afe_stat_serv_handler_ptr[0]);
      result = elite_svc_process_cmd_queue(pAfeSvc, pHDevService, afe_stat_serv_handler_ptr, afe_stat_serv_cmd_table_size);

      // If service has been destroyed, exit.
      if (ADSP_ETERMINATED == result) break;
    }
  }

  afe_destroy_aanc_handler_svc(pAfeSvc);

  afe_device_driver_deinit();

  qurt_elite_memorymap_unregister(pAfeSvc->afe_memory_map_client);

  // destroy the channel
  qurt_elite_channel_destroy(&afe_svc_global_ptr->channel);

  afe_svc_global_ptr = NULL;

  return ADSP_EOK;
}

/**
 * This function is command handler for ELITE_CMD_DESTROY_SERVICE.
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @param[in] pMsg, pointer to message.
 * @return the ADSP_EOK error code
 */
static ADSPResult afe_svc_destroy (void *pAudStatAfeSvc, elite_msg_any_t* pMsg)
{
  // de-initializes the afe real timer ports
  afe_dev_port_deinit(afe_svc_global_ptr);

  elite_msg_return_payload_buffer( pMsg );

  // send ADSP_ETERMINATED so calling routine knows the destroyer has been invoked.
  return ADSP_ETERMINATED;
}

/**
 * This function is about launching of port manager
 *
 * @param[in] port, instance of the port
 * @param[in] portInt, interface of the port
 * @param[in] portDir, sink or source of port property
 * @return the ADSP_EOK error code
 */
ADSPResult afe_svc_init_port(afe_dev_port_t *pDevPort, uint16_t portInt,  uint16_t portDir)
{
  ADSPResult                result = ADSP_EOK;

  // mem set the whole port struct to have default values
  memset(pDevPort, 0, sizeof(afe_dev_port_t));

  // initialize the port mutex
  qurt_elite_mutex_init(&(pDevPort->port_mutex));

  for(uint32_t src_idx = 0; src_idx < NUM_MAX_VFR_SRC; src_idx++)
  {
    qurt_elite_mutex_init(&pDevPort->drift_info[src_idx].port_vfr_mutex);
    pDevPort->drift_info[src_idx].port_vfr_mutex_init = TRUE;
  }

  // and add the signal to the channel. For a pseudo ports in real time mode, this serves as timer interrupt
  if (ADSP_FAILED(result = qurt_elite_signal_init(&(pDevPort->dma_signal))))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADSP_FAILED to init signal in port manager = %d!!\n", result);
    return result;
  }

  // initialize the port interface and direction values
  pDevPort->intf                      = portInt;
  pDevPort->dir                       = portDir;
  pDevPort->is_interleaved            = TRUE; // AUDIO_IF works on interleaved samples and hence defaulting
  pDevPort->is_send_eos_to_tx         = FALSE;
  pDevPort->loopback_cached_gain      = AFE_UNITY_GAIN_Q13;
  pDevPort->latency_mode              = AFE_PORT_DEFAULT_LATENCY_MODE;

  //resetting the media type to default
  afe_port_reset_media_type(pDevPort);

  // initialize AV timer drift related info
  pDevPort->avt_drift_params.is_avt_drift_updated      = FALSE;
  afe_drv_update_avt_drift_info_element(&(pDevPort->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), 0, TRUE);

  return result;
}

ADSPResult afe_svc_deinit_port(afe_dev_port_t *pDevPort)
{
  ADSPResult result = ADSP_EOK;

  for(uint32_t src_idx = 0; src_idx < NUM_MAX_VFR_SRC; src_idx++)
  {
    if(TRUE == pDevPort->drift_info[src_idx].port_vfr_mutex_init)
    {
      qurt_elite_mutex_destroy(&pDevPort->drift_info[src_idx].port_vfr_mutex);
      pDevPort->drift_info[src_idx].port_vfr_mutex_init = FALSE;
    }
  }

  qurt_elite_mutex_destroy(&pDevPort->port_mutex);

  return result;
}

/**
 * This function is command handler for ELITE_CMD_START_SERVIC.
 *
 * @param[in] pAudStatAfeSvc, pointer to AfeSvc instance.
 * @param[in] pMsg, pointer to message.
 * @return the ADSP_EOK error code
 */
static ADSPResult afe_svc_start (void *pAudStatAfeSvc, elite_msg_any_t* pMsg)
{
  ADSPResult result;
  aud_stat_afe_svc_t     *pAfeSvc;
  pAfeSvc = (aud_stat_afe_svc_t*)pAudStatAfeSvc;

  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AfeSvc: StartSvc  Enter");

  /* Register ISR call back function with APR */
  char aAfeSvcName[] = "qcom.adsp.afe";
  uint32_t ulAfeNameSize = strlen( aAfeSvcName );
  uint16_t usRetAddr = 0;
  if (ADSP_FAILED( result = elite_apr_if_register_by_name( &(pAfeSvc->apr_handle), &usRetAddr, aAfeSvcName, ulAfeNameSize, &afe_svc_apr_call_back_fct,NULL)) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfeSvc: Fail to register AfeSvc  0x%8x", result );
  }
  else
  {
    if (ADSP_FAILED( result = afe_svc_set_apr_handle(pAfeSvc->apr_handle)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfeSvc: Fail to Set AfeSvc  handle 0x%8x", result );
    }
  }

  elite_msg_finish_msg( pMsg, result);

  MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AfeSvc: StartSvc  Leave");
  return result;
}
