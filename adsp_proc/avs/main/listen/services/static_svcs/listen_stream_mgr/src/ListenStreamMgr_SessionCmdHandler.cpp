/**
@file ListenStreamMgr_SessionCmdHandler.cpp

@brief This file contains the implementation for session cmdQ Handler functions
for ListenStreamMgr.

 */

/*========================================================================
$Header $ //source/qcom/qct/multimedia2/Listen/elite/static_svcs/ListenStreamMgr/main/latest/src/ListenStreamMgr_SessionCmdHandler.cpp#18 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------

==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "ListenStreamMgr_SessionCmdHandler.h"
#include "ListenStreamMgr_Session.h"
#include "ListenStreamMgr_AprIf.h"
#include "ListenStreamMgr_AprDataBase.h"
#include "ListenProcSvc.h"
#include "adsp_lsm_session_commands.h"
#include "EliteTopology_db_if.h"
#include "ListenStreamMgr_Deprecate.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
#define DEFAULT_16_BITS_PER_SAMPLE 16
#define DEFAULT_16K_SAMPLE_RATE 16000

/*----------------------------------------------------------------------------
 * Constant / Define Declarations
 *--------------------------------------------------------------------------*/
extern uint32_t lsm_memory_map_client;

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

/*
   This function process the service and session APR commands.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
 */
ADSPResult lsm_svc_and_session_apr_pkt_handler( lsm_t *me_ptr)
{
  ADSPResult result = ADSP_EOK;
  elite_apr_packet_t *apr_pkt_ptr = (elite_apr_packet_t *) (me_ptr->msg.pPayload);

  uint32_t ulOpCode = elite_apr_if_get_opcode( apr_pkt_ptr );

  switch ( ulOpCode )
  {
    case LSM_SESSION_CMD_OPEN_TX_V2:
    {
      result =  lsm_session_cmd_q_open_tx_stream_handler(me_ptr,apr_pkt_ptr);
      break;
    }
    case LSM_SESSION_CMD_CLOSE:
    {
      result =  lsm_session_cmd_q_close_tx_stream_handler(me_ptr,apr_pkt_ptr);
      break;
    }
    case LSM_SESSION_CMD_SHARED_MEM_MAP_REGIONS :
    {
      result = elite_mem_shared_memory_map_regions_cmd_handler(
          lsm_memory_map_client,
          lsm_get_apr_handle(),
          apr_pkt_ptr,
          LSM_SESSION_CMDRSP_SHARED_MEM_MAP_REGIONS);
      break;
    }
    case LSM_SESSION_CMD_SHARED_MEM_UNMAP_REGIONS :
    {
      result = elite_mem_shared_memory_un_map_regions_cmd_handler(
          lsm_memory_map_client,
          lsm_get_apr_handle(),
          apr_pkt_ptr);
      break;
    }
    /* Deprecated Support */
    case LSM_CMD_ADD_TOPOLOGIES:
    {
      result = lsm_service_cmd_q_add_topologies_handler(me_ptr, apr_pkt_ptr);
      break;
    }
    case LSM_SESSION_CMD_OPEN_TX:
    {
      result = lsm_session_cmd_q_open_tx_v1_stream_handler(me_ptr, apr_pkt_ptr);
      break;
    }
    /* end - Deprecated Support */
    default:
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM[%d]: unsupported cmd 0x%lx",
            (uint32_t)elite_apr_if_get_dst_port( apr_pkt_ptr ) , ulOpCode);
      result = ADSP_EUNSUPPORTED;
      /* Generate ACK and free apr packet */
      lsm_generate_ack( apr_pkt_ptr, ADSP_EUNSUPPORTED, NULL, 0, 0);

    }
  }

  return result;
}

/*
   This function process LSM_SESSION_CMD_OPEN_TX command that are send to a
   given session.

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The
                      channel bit in this instance that associated with the
                      given session might be turned on/off depending on command
                      processing.
   @param pSession[in/out] This points to the session that are to process the
                           client command.
   @return Success or Failure
 */
ADSPResult lsm_session_cmd_q_open_tx_stream_handler(lsm_t *pMe, elite_apr_packet_t *apr_pkt_ptr)
{
  ADSPResult result=ADSP_EOK;
  elite_apr_port_t  port   = elite_apr_if_get_dst_port( apr_pkt_ptr);
  uint32_t session_id = LSM_GET_SESSION_ID( port);
  lsm_session_state_type_t *pSession;
  qurt_elite_queue_t *output_bufq_ptr = NULL;
  qurt_elite_queue_t *dyn_svc_cmdq_ptr = NULL;
  lsm_session_cmd_open_tx_v2_t *cmd_open_tx_v2_ptr = NULL;
  /* This variable is provided to listen dynamic svc for initialization. */
  listen_proc_svc_init_params_t open_params;

  if ((session_id == 0) || (session_id > LSM_MAX_SESSION_ID) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id[%u]", session_id);
#endif
    result = ADSP_EBADPARAM;
    goto done;
  }
  //-1 is required as we are starting session id from 1.
  pSession = &(pMe->aSessions[session_id-1]);

  /* sanity check of session state */
  if ( (LISTEN_STREAM_MGR_SESSION_DEINIT != pSession->session_state))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "Session already opened, current State %d",
          pSession->session_state);
    result = ADSP_EALREADY;
    goto done;
  }


  elite_apr_if_get_payload( (void **)&cmd_open_tx_v2_ptr, apr_pkt_ptr );
  open_params.topology_id = cmd_open_tx_v2_ptr->topology_id;
  open_params.sampling_rate = DEFAULT_16K_SAMPLE_RATE;

  /* compose call back data for Listen proc Svc to raise event to client */
  lsm_callback_handle_t cbData;
  result = lsm_session_fill_cb_data_for_dynamic_svc(pMe, pSession, &cbData);

  if (ADSP_FAILED(result))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "LSM OPEN CMD: FAILED to create call back for Listen session[0x%p]",
          pSession);
#endif
    goto done;
  }

  /* hard coding to 16 bits. In future need to expose api for >16 bits */
  open_params.bits_per_sample = DEFAULT_16_BITS_PER_SAMPLE;
  open_params.num_channels =  1;
  open_params.session_id = pSession->ucInternalSessionId;

  /* Send the metadata request to Listen Proc svc in InitParams structure */
  open_params.callback_data_ptr = &cbData;

  result = listen_proc_svc_create ( &open_params,
                                    (void**)(&(pSession->dyn_svc.handle)));

  if (ADSP_FAILED(result) || !(pSession->dyn_svc.handle))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "OPEN CMD: FAILED to create listen service result[0x%x], handle[%p]",
          result,
          pSession->dyn_svc.handle);
    if(!(pSession->dyn_svc.handle))
    {
      /* Ensure that result is failed if not handle was created */
      result = ADSP_EFAILED;
    }
    goto done;
  }

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Listen ProcSvc Created succesfuly");

  pSession->dyn_svc.thread_id = (pSession->dyn_svc.handle)->threadId;
  pSession->session_state = LISTEN_STREAM_MGR_SESSION_INIT;

  //Register cmd queue
  dyn_svc_cmdq_ptr = pSession->dyn_svc.handle->cmdQ;

  // Register for cmd queue
  if ( ADSP_FAILED(result = lsm_apr_router_register_dyn_svc_cmd_q(cbData.self_port, dyn_svc_cmdq_ptr) ) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LSM [%u]: can not register output bufq",
          pSession->ucInternalSessionId);
#endif
    goto done;
  }

  //Register data queue
  output_bufq_ptr = pSession->dyn_svc.handle->gpQ; 

  // Register for buffer queue
  if ( ADSP_FAILED(result = lsm_apr_router_register_buf_q(cbData.self_port, output_bufq_ptr) ) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LSM [%u]: can not register output bufq",
          pSession->ucInternalSessionId);
#endif
    goto done;
  }

  done:
  /* generate ack and free apr packet */
  lsm_generate_ack( apr_pkt_ptr, result, NULL, 0, 0);
  return result;
}


/*
   This function process   LSM_SESSION_CMD_CLOSE command 

   @param pMe[in/out] This points to the instance of ListenStreamMgr. The channel bit in this instance
                     that associated with the given session might be turned on/off depending
                     on command processing.
   @param pSession[in/out]  This points to the session that are to process the client command.
   @return Success or Failure
 */
ADSPResult  lsm_session_cmd_q_close_tx_stream_handler(lsm_t *pMe, elite_apr_packet_t *apr_pkt_ptr)
{

  uint32_t unPayloadSize;
  int status;
  elite_msg_any_t msg;
  ADSPResult result = ADSP_EOK;
  lsm_session_state_type_t *pSession;
  elite_apr_port_t  port   = elite_apr_if_get_dst_port( apr_pkt_ptr);
  uint32_t session_id = LSM_GET_SESSION_ID( port);

  if ((session_id == 0) || (session_id > LSM_MAX_SESSION_ID) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Invalid session id: [%u]", session_id);
#endif
    result = ADSP_EBADPARAM;
    goto done;
  }
  //-1 is required as we are startign session id from 1.
  pSession = &(pMe->aSessions[session_id-1]);

  if (LISTEN_STREAM_MGR_SESSION_DEINIT == pSession->session_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "LSM CLOSE CMD: Invalid request to close an unopen Session %d",
          pSession->session_state);
    result = ADSP_EOK;
    goto done;
  }
  /*move state to DEINIT*/
  pSession->session_state = LISTEN_STREAM_MGR_SESSION_DEINIT;

  /* after this deregister none of the cmds can be pushed onto cmdQ from APR callback,
     until again new session is set up */
  if ( ADSP_FAILED(  lsm_apr_router_deregister_dyn_svc_cmd_q(port) ) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LSM [%u]: can not deregister dyn svc cmdQ", pSession->ucInternalSessionId);
#endif
  }
  /*De Register buf queue*/
  if ( ADSP_FAILED(  lsm_apr_router_deregister_buf_q(port) ) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"LSM [%u]: can not deregister output bufq",
          pSession->ucInternalSessionId);
#endif
  }
  /*  Destroy dynamic service */
  unPayloadSize = sizeof( elite_msg_cmd_destroy_svc_t);
  if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize,
                                                  ELITE_CMD_DESTROY_SERVICE, 
                                                  NULL,
                                                  0, 
                                                  0 ) ) )
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create destroy message: 0x%x",result);
#endif
    goto done;
  }
  if (ADSP_FAILED(result = qurt_elite_queue_push_back(pSession->dyn_svc.handle->cmdQ, (uint64_t*)&msg )))
  {
#ifdef LSM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send disconnect message :0x%x", result);
#endif
    elite_msg_finish_msg(&msg, ADSP_EFAILED);
    goto done;
  }
  /*wait on the thread join, till dynamic svc destroyed and returned static service will not service any commands*/
  qurt_elite_thread_join((pSession->dyn_svc.handle)->threadId, &status);

  done:
  /* Generate ACK and free apr packet*/
  result = lsm_generate_ack(apr_pkt_ptr, result, NULL, 0, 0 );
  return ADSP_EOK;
}
