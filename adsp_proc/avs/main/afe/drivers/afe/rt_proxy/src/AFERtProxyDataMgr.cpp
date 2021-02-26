/*========================================================================

  This file is implementation for real-time proxy port data manager (mainly for USB)

 Copyright (c) 2011-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //source/qcom/qct/multimedia2/Audio/drivers/afe/rt_proxy/...

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  03/16/11   rb      Created file.

  ========================================================================== */
/*==========================================================================
  the Include files
  ========================================================================== */

#include "AFERtProxyDriver_i.h"
#include "Interleaver.h"

/*=====================================================================
 Globals
 ======================================================================*/
QURT_ELITE_ALIGN(static char afe_data_mgr_dataBufQ[QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_RTPPDM_Q_ELEMENTS)],8);
afe_rtppdm_t              afe_rtppdm;
/** circular buffer access */
ADSPResult afe_rtppdm_linear_to_circ(afe_circ_buff_t *dst_circ_buff_ptr, \
                                     uint8_t *src_linear_buff_ptr, uint32_t length, uint32_t num_bytes, \
                                     uint16_t *is_high_water_mark, uint32_t high_water_mark, uint16_t port_id);
ADSPResult afe_rtppdm_circ_to_linear(uint8_t *dst_linear_buff_ptr, \
                                     afe_circ_buff_t *src_circ_buff_ptr, \
                                     uint32_t length, uint16_t *is_low_water_mark, \
                                     uint32_t low_water_mark, uint16_t port_id);
/*=====================================================================
 Functions
 ======================================================================*/

/*=====================================================================
 Message Handler
======================================================================*/
static elite_svc_msg_handler_func afe_rtppdm_cmd_handler [] =
{
    afe_rtppdm_custom_cmd_handler,                  //0 - ELITE_CUSTOM_MSG
    elite_svc_unsupported,                          //1 - ELITE_CMD_START_SERVICE
    afe_rtppdm_destroy_handler,                     //2 - ELITE_CMD_DESTROY_SERVICE
    elite_svc_unsupported,                          //3 - ELITE_CMD_CONNECT
    elite_svc_unsupported,                          //4 - ELITE_CMD_DISCONNECT
    elite_svc_unsupported,                          //5 - ELITE_CMD_PAUSE
    elite_svc_unsupported,                          //6 - ELITE_CMD_RESUME
    elite_svc_unsupported,                          //7 - ELITE_CMD_FLUSH
    elite_svc_unsupported,                          //8 - ELITE_CMD_SET_PARAM
    elite_svc_unsupported,                          //9 - ELITE_CMD_GET_PARAM
    elite_svc_unsupported,                          //A - ELITE_DATA_BUFFER
    elite_svc_unsupported,                          //B - ELITE_DATA_MEDIA_TYPE
    elite_svc_unsupported,                          //C - ELITE_DATA_EOS
    elite_svc_unsupported,                          //D - ELITE_DATA_RAW_BUFFER
    elite_svc_unsupported,                          //E - ELITE_CMD_STOP_SERVICE
    afe_rtppdm_apr_cmd_handler,                     //F - ELITE_APR_PACKET
};

/**
 * =============================FOLLOWING CODE EXECUTES IN THE CALLING THREAD'S(AfeS) CONTEXT =========================================
 */
/**
  @brief Perform Real time Proxy port data mgr initialization

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_rtppdm_init(void)
{
  ADSPResult         result           = ADSP_EFAILED;
  afe_rtppdm_t       *data_mgr        = &afe_rtppdm;
  char               dataBufQName[]   = AFE_RTPPDM_DATA_BUF_Q_NAME;
  qurt_elite_queue_t      *pTemp           = NULL;

  memset(data_mgr, 0, sizeof(afe_rtppdm_t));
  qurt_elite_channel_init(&(data_mgr->data_mgr_channel));

  //create, initialize a dataBufQ
  pTemp = (qurt_elite_queue_t *) afe_data_mgr_dataBufQ;
  if (ADSP_FAILED(result = (qurt_elite_queue_init(dataBufQName, \
                                                  AFE_RTPPDM_Q_ELEMENTS, pTemp ))))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port data_buf_q init failed");
    data_mgr->data_mgr_handle.cmdQ            = pTemp;
    afe_svc_global_ptr->rt_port_mgr_q         = pTemp;
    return result;
  }
  data_mgr->data_mgr_handle.cmdQ      = pTemp;

  if (ADSP_FAILED(result = qurt_elite_channel_addq(&(data_mgr->data_mgr_channel), pTemp,0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port data_buf_q add to channel failed: %x", result);
    return result;
  }

  //register with AFE Apr interface for pushing buffers down the queue, when read/write cmd comes in
  afe_svc_global_ptr->rt_port_mgr_q   = pTemp;

  //launch a thread & wait on the above queue for buffers
  if (ADSP_FAILED (result = qurt_elite_thread_launch(&(data_mgr->data_mgr_handle.threadId), \
                                                     (char*)AFE_RTPPDM_THREAD_NAME, \
                                                     NULL, AFE_RTPPDM_THREAD_STACK_SIZE, \
                                                     elite_get_thrd_prio(ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID),
                                                     afe_rtppdm_work_loop, \
                                                     (void*) &afe_rtppdm,
                                                     QURT_ELITE_HEAP_DEFAULT)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port data_mgr thread launch failed");
    data_mgr->data_mgr_handle.threadId = 0;
    return result;
  }

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Success init RT proxy port driver");
  return ADSP_EOK;
}

/**
  @brief Perform Real time Proxy port data manager deinitialization
  sends a message to RT data mgr thread to terminate itself.

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_rtppdm_deinit(void)
{
  int                              result=ADSP_EOK;
  elite_msg_any_t                  anyMsg;
  uint32                           unSize;
  afe_rtppdm_t                     *data_mgr      = &afe_rtppdm;


  /*Bail out if RT Proxy DM thread was never launched*/
  if(0 == data_mgr->data_mgr_handle.threadId)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT proxy DM thread was never launched");
    return ADSP_EALREADY;
  }

  unSize = sizeof(elite_msg_cmd_destroy_svc_t);

  // Compose a command to close rt proxy port data mgr.
  if ( ADSP_FAILED( result = elite_msg_create_msg( &anyMsg, &unSize,
                                                   ELITE_CMD_DESTROY_SERVICE, NULL, 0, 0) ) ) {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to create msg to destroy RT proxy port data mgr, result 0x%8x\n",result);
    return ADSP_EFAILED;
  }

  // Send the close command to the data mgr
  result = qurt_elite_queue_push_back( data_mgr->data_mgr_handle.cmdQ, (uint64*)&anyMsg );

  if (ADSP_FAILED(result))
  {
    /* Return the msg buffer */
    elite_msg_release_msg(&anyMsg);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Fail to deliver msg to service, result 0x%8x\n",result);
    return ADSP_EFAILED;
  }
  // Wait here until service destruction is complete.
  qurt_elite_thread_join(data_mgr->data_mgr_handle.threadId, &result);

  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to join RT proxy Port data mgr thread: 0x%8x\n",result);
    return result;
  }

  return ADSP_EOK;
}

ADSPResult afe_rtppdm_send_custom_cmd(uint16_t port_id, uint32_t opcode)
{
  qurt_elite_bufmgr_node_t            bufMgrNode;
  int                             nActualSize;
  elite_msg_any_t                 msg;
  elite_msg_custom_afe_header_t   *msg_payload;
  ADSPResult                      result = ADSP_EOK;

  //get buf from the buf mgr
  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_custom_afe_header_t), &bufMgrNode, &nActualSize)))
  {
    result = ADSP_ENEEDMORE;
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to create buf to forward %lx command to rt proxy port data manager. %x", opcode, result);
    return result;
  }
  msg_payload = (elite_msg_custom_afe_header_t*) bufMgrNode.pBuffer;

  msg_payload->afe_id                         =  port_id;
  msg_payload->responseq_ptr                  =  NULL;
  msg_payload->client_token                   =  0;
  msg_payload->buffer_returnq_ptr             =  bufMgrNode.pReturnQ;
  msg_payload->sec_op_code                    =  opcode;

  msg.unOpCode        =  ELITE_CUSTOM_MSG;
  msg.pPayload        =  (void*) msg_payload;

  //push to the queue of rt proxy port data manager
  result = qurt_elite_queue_push_back(afe_rtppdm.data_mgr_handle.cmdQ, (uint64_t*)&msg);
  if (ADSP_FAILED(result))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to push msg to forward %lx command to rt proxy port data manager. %x", opcode, result);

    /* Return the msg buffer */
    elite_msg_release_msg(&msg);
    return result;
  }

  //no wait for the ack. buffer is cleaneup up by the RTPPDM thread

  return result;
}

ADSPResult afe_rtppdm_port_start(uint16_t port_id)
{
  //send a message to proxy port data manager thread
  return afe_rtppdm_send_custom_cmd(port_id, ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_START);
}
ADSPResult afe_rtppdm_port_stop(uint16_t port_id)
{
  //send a message to proxy port data manager thread
  return afe_rtppdm_send_custom_cmd(port_id, ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_STOP);
}

/**
 * =============================FOLLOWING CODE EXECUTES IN RT PROXY PORT DATA MANAGER THREAD'S CONTEXT =========================================
 */

/**
 * Workloop of the RT proxy port data manager
 */
int afe_rtppdm_work_loop (void* vp_afe_rtppdm)
{
  afe_rtppdm_t      *data_mgr = (afe_rtppdm_t*) vp_afe_rtppdm;
  uint32_t          unChannelStatus;
  ADSPResult        result = ADSP_EOK;
  qurt_elite_channel_t   *data_mgr_channel = &data_mgr->data_mgr_channel;
  uint32_t          unCurrentBitfield = qurt_elite_queue_get_channel_bit(data_mgr->data_mgr_handle.cmdQ);

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Entering AFE RT proxy port data manager work loop...");

  for(;;)
  {
    // block on any one or more of selected queues to get a msg
    unChannelStatus = qurt_elite_channel_wait(data_mgr_channel, unCurrentBitfield) ;

    // Check if commands came in
    if (unChannelStatus & unCurrentBitfield)
    {
      // call common utility to process command queue in standard way.
      const uint32_t cmd_table_size = sizeof(afe_rtppdm_cmd_handler)/sizeof(afe_rtppdm_cmd_handler[0]);
      result = elite_svc_process_cmd_queue(vp_afe_rtppdm, \
                                           &data_mgr->data_mgr_handle, \
                                           afe_rtppdm_cmd_handler, \
                                           cmd_table_size);

      // If service has been destroyed, exit.
      if (ADSP_ETERMINATED == result) break;

      if (ADSP_FAILED(result))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port Data Mgr: cmd failed %x", result);
      }
      else
      {
        //MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "RT Proxy Port Data Mgr: cmd success %x", result);
      }
    }
  }

  return ADSP_EOK;
}

/**
 * Function to kill the Proxy Port Data Manager thread.
 */
ADSPResult afe_rtppdm_destroy_handler (void *data_mgr, elite_msg_any_t* pMsg)
{
  elite_msg_return_payload_buffer(pMsg);

  return ADSP_ETERMINATED;
}

/**
 * Function that handles the data commands from the client
 */
ADSPResult afe_rtppdm_apr_cmd_handler (void* data_mgr, elite_msg_any_t* pMsg)
{
  elite_apr_packet_t         *pkt_ptr  = (elite_apr_packet_t *)(pMsg->pPayload);
  ADSPResult                 result = ADSP_EOK;

  switch ( pkt_ptr->opcode )
  {
    case AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2:
    {
      result = afe_rtppdm_read_handler(pkt_ptr);
      break;
    }
    case AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2:
    {
      result = afe_rtppdm_write_handler(pkt_ptr);
      break;
    }
    case AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER:
    {
      result = afe_rtppdm_register_handler(pkt_ptr);
      break;
    }
    case AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER:
    {
      result = afe_rtppdm_unregister_handler(pkt_ptr);
      break;
    }
    default:
    {
      result = ADSP_EUNSUPPORTED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port Data Mgr: Invalid opcode %8lx", pkt_ptr->opcode);
      elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
      break;
    }
  }
  return result;
}
/**
 *
 */
ADSPResult afe_rtppdm_custom_cmd_handler (void* data_mgr, elite_msg_any_t* pMsg)
{
  ADSPResult                      result = ADSP_EOK;
  uint16_t                        port_id;
  afe_port_state                  port_state;
  uint32_t                        opcode;
  uint16_t                        event_type = AFE_EVENT_TYPE_RT_PROXY_PORT_INVALID;
  elite_msg_custom_afe_header_t   *pCustomAfeMsgPayload;
  afe_rt_proxy_port_dev_t         *rt_proxy_dev_ptr = NULL;

  opcode   = ((elite_msg_custom_header_t *)pMsg->pPayload)->unSecOpCode;
  switch (opcode)
  {
    case ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_START:
    {
      event_type  =  AFE_EVENT_TYPE_RT_PROXY_PORT_START;
      port_state  =  AFE_PORT_STATE_RUN;
      break;
    }
    case ELITEMSG_CUSTOM_AFE_RT_PROXY_PORT_STOP:
    {
      event_type  =  AFE_EVENT_TYPE_RT_PROXY_PORT_STOP;
      port_state  =  AFE_PORT_STATE_STOP;
      break;
    }
    default:
    {
      result = ADSP_EBADPARAM;
      elite_msg_finish_msg(pMsg, result); //no ack
      return result;
    }
  }
  //Assuming we do not have any other custom commands for now...
  //set the port state and send an event
  pCustomAfeMsgPayload    = (elite_msg_custom_afe_header_t *)pMsg->pPayload;

  port_id                 = pCustomAfeMsgPayload->afe_id;
  rt_proxy_dev_ptr        = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

  rt_proxy_dev_ptr->port_state     = port_state;
  rt_proxy_dev_ptr->dev_byte_cnt   = 0;

  result = afe_rtppdm_send_event(port_id, event_type);

  elite_msg_finish_msg(pMsg, result); //no ack

  return result;
}

/**
 * function to send an event to the RT driver (this fn could be executed from threadpool thread)
 */
ADSPResult afe_rtppdm_send_event (uint16_t port_id, uint16_t event_type)
{
  ADSPResult                       result = ADSP_EOK;
  afe_rt_proxy_port_dev_t          *rt_proxy_dev_ptr;
  afe_event_rt_proxy_port_status_t event_payload;

  rt_proxy_dev_ptr           =  &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

  event_payload.port_id      =  port_id;
  event_payload.event_type   =  event_type;

  if (0 != rt_proxy_dev_ptr->apr_client_port && 0 != rt_proxy_dev_ptr->apr_client_addr &&
      /*0 != afe_rtppdm.apr_self_port &&*/ 0 != afe_rtppdm.apr_self_addr)
  {
    result = elite_apr_if_alloc_send_event ( afe_svc_get_apr_handle(),
                                             afe_rtppdm.apr_self_addr,
                                             afe_rtppdm.apr_self_port,
                                             rt_proxy_dev_ptr->apr_client_addr,
                                             rt_proxy_dev_ptr->apr_client_port,
                                             afe_rtppdm.event_token++,
                                             AFE_EVENT_RT_PROXY_PORT_STATUS,    //opcode for ACK
                                             &event_payload,   //pointer to payload
                                             sizeof(afe_event_rt_proxy_port_status_t));       //length of payload
    if (ADSP_FAILED(result))
    {
      MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to send event to RT driver. Port_id=%x, event_type=%x. res=%x.", port_id,event_type,result);
    }
    else
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "sent event to RT driver. Port_id=%x, event_type=%x", port_id,event_type);
    }
  }
  else
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT driver not registered for the events. Port_id=%x, event_type=%x. res=%x.", port_id,event_type,result);
  }

  return result;
}
/**
 * register command
 */
ADSPResult afe_rtppdm_register_handler (elite_apr_packet_t* pkt_ptr)
{
  ADSPResult                                   result =  ADSP_EOK;
  afe_service_cmd_register_rt_port_driver_t    *register_cmd;
  int32_t                                      rc;
  afe_rt_proxy_port_dev_t                      *rt_proxy_dev_ptr;
  uint16_t                                     port_id;

  /* Validate the inbound payload size. */
  if ( elite_apr_if_get_payload_size(pkt_ptr) != sizeof(afe_service_cmd_register_rt_port_driver_t) )
  {
    result = APR_EBADPARAM;
    rc = elite_apr_if_end_cmd(afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  register_cmd            = (afe_service_cmd_register_rt_port_driver_t *) elite_apr_if_get_payload_ptr(pkt_ptr);
  port_id                 = register_cmd->port_id;
  rt_proxy_dev_ptr        = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];
  //store the APR destination address
  afe_rtppdm.apr_self_port          = elite_apr_if_get_dst_port(pkt_ptr);
  afe_rtppdm.apr_self_addr          = elite_apr_if_get_dst_addr(pkt_ptr);
  rt_proxy_dev_ptr->apr_client_port = elite_apr_if_get_src_port(pkt_ptr);
  rt_proxy_dev_ptr->apr_client_addr = elite_apr_if_get_src_addr(pkt_ptr);

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Registered APR RT driver client for port %x, APR:(addr=%x,port=%x)", \
        port_id, rt_proxy_dev_ptr->apr_client_addr, rt_proxy_dev_ptr->apr_client_port);

  //send ack to the client
  rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);

  if (rc) return ADSP_EFAILED;
  return result;
}

/**
 * unregister command
 */
ADSPResult afe_rtppdm_unregister_handler (elite_apr_packet_t* pkt_ptr)
{
  ADSPResult                                   result =  ADSP_EOK;
  afe_service_cmd_unregister_rt_port_driver_t  *unregister_cmd;
  uint32_t                                     rc;
  afe_rt_proxy_port_dev_t                      *rt_proxy_dev_ptr;
  uint16_t                                     port_id;

  /* Validate the inbound payload size. */
  if ( elite_apr_if_get_payload_size(pkt_ptr) != sizeof(afe_service_cmd_unregister_rt_port_driver_t) )
  {
    result = APR_EBADPARAM;
    rc = elite_apr_if_end_cmd(afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  unregister_cmd          = (afe_service_cmd_unregister_rt_port_driver_t *) elite_apr_if_get_payload_ptr(pkt_ptr);
  port_id                 = unregister_cmd->port_id;
  rt_proxy_dev_ptr        = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

  //reset the APR destination address
  rt_proxy_dev_ptr->apr_client_port = 0;
  rt_proxy_dev_ptr->apr_client_addr = 0;

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "UNRegistered APR RT driver client for port %x, APR:(addr=%x,port=%x)", \
        port_id, rt_proxy_dev_ptr->apr_client_addr, rt_proxy_dev_ptr->apr_client_port);

  //send ack to the client
  rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);

  if (rc) return ADSP_EFAILED;
  return result;
}

/**
 * RT driver sitting on apps reads data from DSP in RX or spkr path: reads the circular buffer
 */
ADSPResult afe_rtppdm_read_handler(elite_apr_packet_t* pkt_ptr)
{
  uint16_t                   port_id;
  uint32_t                   rc;
  uint16_t                   is_low_water_mark;
  uint16_t                   block_size;
  uint8_t                    *buf_ptr =  NULL;
  ADSPResult                 result   =  ADSP_EOK;
  afe_dev_port_t             *dev_port_ptr      = NULL;
  afe_rt_proxy_port_dev_t    *rt_proxy_dev_ptr  = NULL;
  afe_circ_buff_t            *circ_buff_ptr;
  afe_port_data_cmd_rt_proxy_port_read_v2_t    *read_cmd;
  uint32_t                   virtual_addr;

  /* Validate the inbound payload size. */
  if ( elite_apr_if_get_payload_size(pkt_ptr) != sizeof(afe_port_data_cmd_rt_proxy_port_read_v2_t) )
  {
    result = APR_EBADPARAM;
    rc = elite_apr_if_end_cmd(afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  read_cmd = (afe_port_data_cmd_rt_proxy_port_read_v2_t *) elite_apr_if_get_payload_ptr(pkt_ptr);

  port_id = read_cmd->port_id;

  if (ADSP_FAILED(result = afe_rtppdm_validate_port(port_id, RX_DIR, &dev_port_ptr, &rt_proxy_dev_ptr)))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  rt_proxy_dev_ptr  = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];
  block_size        = read_cmd->available_bytes;
  circ_buff_ptr     = &rt_proxy_dev_ptr->circ_buf;

  if (ADSP_FAILED(result = afe_rtppdm_map_phy_addr(read_cmd->mem_map_handle, read_cmd->buffer_address_lsw, read_cmd->buffer_address_msw, block_size, &virtual_addr, port_id)))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }
  buf_ptr           = (uint8_t*)virtual_addr;

  //read from circular buffer,
  qurt_elite_mutex_lock(&rt_proxy_dev_ptr->circ_buf_mutex);
  if (rt_proxy_dev_ptr->was_first_write_done)
  {
    //if this is the first read (first read was not done)
    if (!rt_proxy_dev_ptr->was_first_read_done)
    {
      rt_proxy_dev_ptr->was_first_read_done = TRUE;
      //during the first read, adjust read (head) ptr to be half away from write.
      circ_buff_ptr->head_addr =
          circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >> 1));

      // Wrap around if necessary as tail is moving
      // Otherwise, head_addr may point outside the buffer because of big frame
      if(circ_buff_ptr->head_addr >= circ_buff_ptr->bottom_addr)
      {
        circ_buff_ptr->head_addr =
            circ_buff_ptr->top_addr 
            + (circ_buff_ptr->head_addr - circ_buff_ptr->bottom_addr);
      }

    }
    result = afe_rtppdm_circ_to_linear((uint8_t*)buf_ptr, circ_buff_ptr, \
                                       block_size, &is_low_water_mark, rt_proxy_dev_ptr->low_water_mark, port_id);
  }
  else
  {
    is_low_water_mark = TRUE;
    memset(buf_ptr, 0, block_size);
#ifdef RTPP_MEDIUM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE RTPPDM-read: %x:Underrun since first write not done yet",port_id);
#endif
  }
  qurt_elite_mutex_unlock(&rt_proxy_dev_ptr->circ_buf_mutex);

  rt_proxy_dev_ptr->dev_byte_cnt += block_size;

  if (0 == rt_proxy_dev_ptr->interleaved) //deinterleave if needed before sending to the client
  {
    afe_rtppdm_deinterleave((uint8_t*)buf_ptr, rt_proxy_dev_ptr->scratch_buffer_ptr, \
                            dev_port_ptr->channels, dev_port_ptr->bytes_per_channel,
                            block_size, rt_proxy_dev_ptr->frame_size);

    /* Copy the data of scratch buffer into buf_ptr as buf_ptr contains
     * memory handle, which will be sent back to RT proxy driver. */
    memscpy(buf_ptr, block_size, rt_proxy_dev_ptr->scratch_buffer_ptr,
            rt_proxy_dev_ptr->frame_size);
  }

  // in RX dir, AFE writes the buffers, after writing flush the cache, so that values in cache gets written to mem.
  if (ADSP_FAILED(result = qurt_elite_memorymap_cache_flush((uint32_t)virtual_addr, block_size)))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM:%x: cache flush failed",port_id);
    if (rc) return ADSP_EFAILED;
    return result;
  }

#ifdef DEBUG_FILE_LOG
  FILE *fp;
  fp = fopen("afe_rx_port_to_USB.pcm","a"); // in append mode
  fwrite(buf_ptr,sizeof(uint8_t),block_size, fp);
  fclose(fp);
#endif

  //since buffer arrived, we can send the events if it occurs again
  rt_proxy_dev_ptr->is_high_water_mark_event_sent = FALSE;
  rt_proxy_dev_ptr->is_low_water_mark_event_sent  = FALSE;

  //send low water mark event if needed
  if (is_low_water_mark &&
      FALSE == rt_proxy_dev_ptr->is_low_water_mark_event_sent) //TODO:check on is_low_..sent should be removed.
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RTPPDM: low water mark event (Underrun) during data flow port=%x",port_id);
    if (0 != rt_proxy_dev_ptr->low_water_mark)
    {
      result = afe_rtppdm_send_low_watermark_event(port_id);
      if (ADSP_FAILED(result))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RTPPDM: failed to send low water mark event during data flow");
      }
      else
      {
        rt_proxy_dev_ptr->is_low_water_mark_event_sent = TRUE;
      }
    }
  }
#ifdef RTPP_VERBOSE_DEBUG
  MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "RT_LOG--------------> RTPPDM: Spkr read from circ buf %d bytes---------\n ", block_size);
#endif

  //ack the apr packet
  rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, ADSP_EOK);



  if (rc)
  {
    return ADSP_EFAILED;
  }
  return result;
}

/**
 * RT driver sitting on apps writes data into DSP in TX or mic path: writes into the circular buffer
 */
ADSPResult afe_rtppdm_write_handler(elite_apr_packet_t* pkt_ptr)
{
  uint16_t                   port_id;
  uint32_t                   rc;
  uint16_t                   is_high_water_mark;
  uint16_t                   block_size;
  uint8_t                    *buf_ptr =  NULL;
  ADSPResult                 result   =  ADSP_EOK;
  afe_circ_buff_t            *circ_buff_ptr;
  afe_dev_port_t             *dev_port_ptr      = NULL;
  afe_rt_proxy_port_dev_t    *rt_proxy_dev_ptr  = NULL;
  afe_port_data_cmd_rt_proxy_port_write_v2_t    *write_cmd;
  uint32_t                   virtual_addr;

  write_cmd = (afe_port_data_cmd_rt_proxy_port_write_v2_t *) elite_apr_if_get_payload_ptr(pkt_ptr);

  port_id = write_cmd->port_id;

  if (ADSP_FAILED(result = afe_rtppdm_validate_port(port_id, TX_DIR, &dev_port_ptr, &rt_proxy_dev_ptr)))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  rt_proxy_dev_ptr  = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];
  block_size        = write_cmd->available_bytes;
  circ_buff_ptr     = &rt_proxy_dev_ptr->circ_buf;

  if (ADSP_FAILED(result = afe_rtppdm_map_phy_addr(write_cmd->mem_map_handle, write_cmd->buffer_address_lsw, write_cmd->buffer_address_msw, block_size, &virtual_addr, port_id)))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    if (rc) return ADSP_EFAILED;
    return result;
  }

  // in TX dir, client sends filled buffers, before reading them invalidate the cache to make sure we dont read old values in cache.
  if (ADSP_FAILED(result = qurt_elite_memorymap_cache_invalidate( (uint32_t)virtual_addr, block_size )))
  {
    rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, result);
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM:%x: invalidate cache failed",port_id);
    if (rc) return ADSP_EFAILED;
    return result;
  }
  buf_ptr           = (uint8_t*) virtual_addr;

#ifdef DEBUG_FILE_LOG
  FILE *fp;
  fp = fopen("afe_tx_port_from_USB.pcm","a"); // in append mode
  fwrite(buf_ptr,sizeof(uint8_t),block_size, fp);
  fclose(fp);
#endif

  qurt_elite_mutex_lock(&rt_proxy_dev_ptr->circ_buf_mutex);
  if (FALSE == rt_proxy_dev_ptr->was_first_write_done)
  {
    //adjust the ptr to be 1/2 away
    //circ_buff_ptr->head_addr = circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
    circ_buff_ptr->head_addr =
        circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >> 1));

    // Wrap around if necessary as tail is moving
    if(circ_buff_ptr->head_addr >= circ_buff_ptr->bottom_addr)
    {
      circ_buff_ptr->head_addr =
          circ_buff_ptr->top_addr 
          + (circ_buff_ptr->head_addr - circ_buff_ptr->bottom_addr);
    }
  }

  if (0 == rt_proxy_dev_ptr->interleaved) //interleave if not so.
  {
    afe_rtppdm_interleave((uint8_t*)buf_ptr, rt_proxy_dev_ptr->scratch_buffer_ptr, \
                          dev_port_ptr->channels, dev_port_ptr->bytes_per_channel,
                          block_size, rt_proxy_dev_ptr->frame_size);

    buf_ptr = rt_proxy_dev_ptr->scratch_buffer_ptr;
  }

  //write incoming data into the circ buffer
  if(block_size > rt_proxy_dev_ptr->frame_size)
  {
    //Incoming size is bigger than the frame size
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RTPPDM: write size (%ld) is bigger than frame size (%ld)", block_size, rt_proxy_dev_ptr->frame_size);
    // Skip the old data to compensate
    block_size = rt_proxy_dev_ptr->frame_size;
    buf_ptr += (block_size - rt_proxy_dev_ptr->frame_size);

    result = afe_rtppdm_linear_to_circ(&rt_proxy_dev_ptr->circ_buf, (uint8_t*)buf_ptr, block_size, \
                                       rt_proxy_dev_ptr->bytes_per_sample,
                                       &is_high_water_mark, rt_proxy_dev_ptr->high_water_mark, port_id);
  }
  else
  {
    //Incoming size is smaller or equal to frame size
    result = afe_rtppdm_linear_to_circ(&rt_proxy_dev_ptr->circ_buf, (uint8_t*)buf_ptr, block_size, \
                                       rt_proxy_dev_ptr->bytes_per_sample,
                                       &is_high_water_mark, rt_proxy_dev_ptr->high_water_mark, port_id);
  }

  rt_proxy_dev_ptr->dev_byte_cnt += block_size;

  if (FALSE == rt_proxy_dev_ptr->was_first_write_done && ADSP_SUCCEEDED(result) && \
      rt_proxy_dev_ptr->dev_byte_cnt >= rt_proxy_dev_ptr->frame_size)
  {
    rt_proxy_dev_ptr->was_first_write_done = TRUE;
  }
  qurt_elite_mutex_unlock(&rt_proxy_dev_ptr->circ_buf_mutex);

  //since data arrived we can send low water mark if it occurs again
  rt_proxy_dev_ptr->is_low_water_mark_event_sent  = FALSE;
  rt_proxy_dev_ptr->is_high_water_mark_event_sent = FALSE;

  //send high water mark event if needed
  if (is_high_water_mark &&
      FALSE == rt_proxy_dev_ptr->is_high_water_mark_event_sent) //TODO: check on is_high_..sent should be removed.
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RTPPDM: high water mark event (Overrun) during data flow port=%x",port_id);
    if (0 != rt_proxy_dev_ptr->config_high_water_mark)
    {
      result = afe_rtppdm_send_high_watermark_event(port_id);
      if (ADSP_FAILED(result))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RTPPDM: failed to send high water mark event during data flow");
      }
      else
      {
        rt_proxy_dev_ptr->is_high_water_mark_event_sent = TRUE;
      }
    }
  }
#ifdef RTPP_VERBOSE_DEBUG
  MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "RT_LOG--------------> RTPPDM: Mic wrote to circ buf %d bytes---------\n ",block_size);
#endif

  //ack the apr packet
  rc = elite_apr_if_end_cmd( afe_svc_get_apr_handle(), pkt_ptr, ADSP_EOK);

  if (rc) return ADSP_EFAILED;

  return result;
}

/**
 * utlility to validate port condition before read/write, also checks for stop.
 */
ADSPResult afe_rtppdm_validate_port(uint16_t port_id, uint16_t expected_dir, \
                                    afe_dev_port_t **dev_port_ptr_ptr, afe_rt_proxy_port_dev_t **rt_proxy_dev_ptr_ptr)
{
  uint16_t                   actual_dir = IS_IT_TX_PORT_AFE_ID(port_id);
  *rt_proxy_dev_ptr_ptr      =  NULL;
  *dev_port_ptr_ptr          =  NULL;

  if (! (IS_RT_PROXY_PORT_AFE_ID(port_id) && (actual_dir == expected_dir)) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port Data Mgr: Invalid port %x",port_id);
    return ADSP_EFAILED;
  }

  *dev_port_ptr_ptr       = &port_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

  *rt_proxy_dev_ptr_ptr   = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

  if ( AFE_PORT_STATE_STOP == (*rt_proxy_dev_ptr_ptr)->port_state )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port Data Mgr: port %x is not running. returning buffer/data with fail status.",port_id);
    return ADSP_EFAILED;
  }
  return ADSP_EOK;
}

ADSPResult afe_rtppdm_map_phy_addr(uint32 mem_map_handle, uint32_t phy_address_lsw, uint32_t phy_address_msw, uint32_t size, uint32_t* virtual_addr, uint16_t port_id)
{
  ADSPResult result;
  *virtual_addr = 0;

  if (0 == afe_svc_global_ptr->afe_memory_map_client || 0 == mem_map_handle)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM: %x: ERROR, Client or addrr Null, cannot get virtual address",port_id);
    result = ADSP_EFAILED;
    return result;
  }
  result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(afe_svc_global_ptr->afe_memory_map_client, mem_map_handle, phy_address_lsw, phy_address_msw, virtual_addr);

  if (ADSP_FAILED(result) || (0 == *virtual_addr) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM:%x: ERROR, get virtual addr failed",port_id);
    result = ADSP_EFAILED;
  }
  return result;
}

//TODO: to test below 2 functions
/**
 * num_samples_per_channel -> number of samples in each of the channels
 * num_bytes_per_sample -> bytes in each sample. 16 bit sample -> 2 bytes
 * num_bytes -> //number of bytes present in the buffer including all channels
 */
void  afe_rtppdm_deinterleave(uint8_t* src_addr, uint8_t* dst_addr, 
                              uint32_t num_channels, uint32_t num_bytes_per_sample, 
                              uint32_t num_bytes, uint32_t dst_buf_size_in_bytes)
{
  uint32_t num_bytes_per_channel = num_bytes / num_channels;
  uint32_t num_samples_per_channel = num_bytes_per_channel / \
      num_bytes_per_sample;
  uint8_t  *dst_ch_ptr, *src_ch_ptr;
  uint32_t samp_idx, chan_idx;

  if (num_channels == 1)
  {
    memscpy(dst_addr, dst_buf_size_in_bytes, src_addr, num_bytes);
  }
  else if (num_channels == 2 && num_bytes_per_sample == 2)
  {
    DeInterleave_16((int16_t*)src_addr, (int16_t*)dst_addr,
                    (int16_t*) (dst_addr + num_bytes_per_channel),
                    num_bytes_per_channel >> 1);
  }
  else
  {
    for(samp_idx = 0; samp_idx < num_samples_per_channel; samp_idx++)
    {
      for(chan_idx = 0; chan_idx < num_channels; chan_idx++)
      {
        dst_ch_ptr = dst_addr + (chan_idx * num_bytes_per_channel) + \
            (samp_idx * num_bytes_per_sample);
        src_ch_ptr = src_addr + \
            (((num_channels * samp_idx) + chan_idx) * num_bytes_per_sample);
        memscpy((void *)dst_ch_ptr, num_bytes_per_sample,
                (void *)src_ch_ptr, num_bytes_per_sample);
      }
    }
  }
}

/**
 * num_bytes -> //number of bytes present in the buffer including all channels
 * num_bytes_per_sample -> bytes per sample of a channel
 */
void afe_rtppdm_interleave(uint8_t* src_addr, uint8_t* dst_addr, 
                           uint32_t num_channels, uint32_t num_bytes_per_sample, 
                           uint32_t num_bytes, uint32_t dst_buf_size_in_bytes)
{
  uint32_t num_bytes_per_channel = num_bytes / num_channels;
  uint32_t num_samples_per_channel = num_bytes_per_channel / \
      num_bytes_per_sample;
  uint8_t  *dst_ch_ptr, *src_ch_ptr;
  uint32_t samp_idx, chan_idx;

  if (num_channels == 1)
  {
    memscpy(dst_addr, dst_buf_size_in_bytes, src_addr, num_bytes);
  }
  else if (num_channels == 2 && num_bytes_per_sample == 2)
  {
    Interleave_16((int16_t*)src_addr,
                  (int16_t*)(src_addr + num_bytes_per_channel),
                  (int16_t*)dst_addr, num_bytes_per_channel >> 1);
  }
  else
  {
    for(samp_idx = 0; samp_idx < num_samples_per_channel; samp_idx++)
    {
      for(chan_idx = 0; chan_idx < num_channels; chan_idx++)
      {
        src_ch_ptr = src_addr + (chan_idx * num_bytes_per_channel) + \
            (samp_idx * num_bytes_per_sample);
        dst_ch_ptr = dst_addr + \
            (((num_channels * samp_idx) + chan_idx) * num_bytes_per_sample);
        memscpy((void *)dst_ch_ptr, num_bytes_per_sample,
                (void *)src_ch_ptr, num_bytes_per_sample);
      }
    }
  }
}

/**
 * arranges samples of interleaved data buffer in src_ch_map to dest_ch_map
 *
 * @param[in] src_ch_map channel mapping of source
 * @param[in] dest_ch_map channel mapping of destination
 * @param[in] bytes_per_ch_samples bytes per sample including all channels
 * @param[in] buffer buffer of data
 * @param[in] num_channels number of channels
 * @param[in] buf_size number of bytes in the buffer
 *
 * @return success or not
 */
ADSPResult  afe_rt_proxy_channel_mapping(uint8_t *src_ch_map, uint8_t *dest_ch_map, uint16_t bytes_per_ch_samples, \
                                         uint8_t *buffer, uint16_t num_channels, uint16_t buf_size)
{
  bool_t   is_mapping_necessary=FALSE;
  // if somebody has set channel mapping then only we need to map.
  if (0 == (uint64_t) (*src_ch_map) || 0 == (uint64_t) (*dest_ch_map) )
  {
    return ADSP_EOK;
  }

  for (int i = 0; i < 8; i++)
  {
    if (src_ch_map[i] != dest_ch_map[i])
    {
      is_mapping_necessary = TRUE;
      break;
    }
  }
  if (!is_mapping_necessary) return ADSP_EOK;

  //do channel mapping //TODO

  return ADSP_EOK;
}

/**
 * This is to read a circular buffer and write to linear buffer
 * head updated on read. tail will be updated in write. If tail needs to be updated, then mutex has to be acquired.
 *
 * @param[in] dst_linear_buff_ptr pointer to destination linear buffer
 * @param[in] src_circ_buff_ptr source circular buffer to read from
 * @param[in] length length to read
 * @param[out] is_low_water_mark whether low water mark is reached.
 */
ADSPResult afe_rtppdm_circ_to_linear(uint8_t *dst_linear_buff_ptr, afe_circ_buff_t *src_circ_buff_ptr, \
                                     uint32_t length, uint16_t *is_low_water_mark, uint32_t low_water_mark, uint16_t port_id)
{
  uint32_t circ_filled_space_head2bottom;
  int32_t  tail_head_gap;
  *is_low_water_mark = FALSE;

  /*
    Check the tail/head circular distance, if distance
      1. larger than length, read the samples.
      2. less or equal length, set the linear buffer as zeros and return;
   */

  tail_head_gap = (int32_t)(src_circ_buff_ptr->tail_addr - src_circ_buff_ptr->head_addr); //filled space
  if (tail_head_gap < 0)
  {
    tail_head_gap += src_circ_buff_ptr->buffer_size;
  }
  if (tail_head_gap < 0 || (uint32_t) tail_head_gap > src_circ_buff_ptr->buffer_size)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM C2L: port=%x:source circ buffer tail/head corrupted!",port_id);
    return ADSP_EFAILED;
  }

  //low water mark detection: if buffer will have less than low water mark samples.
  if ( ( (int32_t)(tail_head_gap-length) <= (int32_t)low_water_mark) )
  {
    *is_low_water_mark = TRUE;
    //memset(dst_linear_buff_ptr, 0, length);  //clear the dest buffer
  }

  if ( (uint32_t) tail_head_gap < length)
  {
    memset(dst_linear_buff_ptr+tail_head_gap, 0, length-tail_head_gap);//make the rest as zero
    length = tail_head_gap; //read whatever is available.
#ifdef RTPP_MEDIUM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM C2L: Underrun,port=%x",port_id);
#endif
  }

  /*
    The following computing based on:
      1. src_circ_buff->top_addr <= src_circ_buff_ptr->head_addr, src_circ_buff_ptr->tail_addr
         < src_circ_buff_ptr->bottom_addr
      2. length <= src_circ_buff_ptr->buffer_size
   */

  circ_filled_space_head2bottom = src_circ_buff_ptr->bottom_addr - src_circ_buff_ptr->head_addr;

  if (circ_filled_space_head2bottom >= length )
  {
    // if head to bottom has enough space to read
    memscpy(dst_linear_buff_ptr, length,
            src_circ_buff_ptr->head_addr, length);

    //qurt_elite_mutex_lock(&src_circ_buff_ptr->buf_mutex);
    src_circ_buff_ptr->head_addr = src_circ_buff_ptr->head_addr + length;
    //qurt_elite_mutex_unlock(&src_circ_buff_ptr->buf_mutex);
  }
  else
    // circ_filled_space_head2bottom < length
  {
    // if head to bottom does not have enough space to read
    memscpy(dst_linear_buff_ptr, length,
            src_circ_buff_ptr->head_addr,  circ_filled_space_head2bottom);

    memscpy(dst_linear_buff_ptr+circ_filled_space_head2bottom, (length - circ_filled_space_head2bottom),
            src_circ_buff_ptr->top_addr, (length - circ_filled_space_head2bottom));

    //qurt_elite_mutex_lock(&src_circ_buff_ptr->buf_mutex);
    src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr + (length - circ_filled_space_head2bottom);
    //qurt_elite_mutex_unlock(&src_circ_buff_ptr->buf_mutex);
  }
  if ((src_circ_buff_ptr->head_addr) >= (src_circ_buff_ptr->bottom_addr) )
  {
    src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr;
  }

  return ADSP_EOK;
}

/**
 * This is to read a linear buffer and write to circular buffer.
 * tail updated on write
 *
 * @param[in] dst_circ_buff_ptr destination circular buffer to write
 * @param[in] src_linear_buff_ptr source linear buffer address
 * @param[in] length length to write
 * @param[in] num_bytes numBytes gap between head and tail, if dst under-run.
 * @param[out] is_overrun whether overrun is detected
 *
 * @return whether write to circ buf was done successfully or not
 */
ADSPResult afe_rtppdm_linear_to_circ(afe_circ_buff_t *dst_circ_buff_ptr, uint8_t *src_linear_buff_ptr, uint32_t length, uint32_t num_bytes, \
                                     uint16_t *is_high_water_mark, uint32_t high_water_mark, uint16_t port_id)
{
  int32_t tail_head_gap;
  int32_t circ_empty_space_tail2bottom;
  *is_high_water_mark = FALSE;

  /*
    Check the destination tail/head circular distance, if
      1. there is not enough space for writing, update the head address in the ead
      2. otherwise, normal
   */
  tail_head_gap = (int32_t)(dst_circ_buff_ptr->tail_addr - dst_circ_buff_ptr->head_addr); //this is the filled space

  if ( tail_head_gap < 0)
  {
    tail_head_gap += dst_circ_buff_ptr->buffer_size;
  }
  if ( tail_head_gap < 0 || (uint32_t) tail_head_gap > dst_circ_buff_ptr->buffer_size)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM L2C: port=%x:dst circ buffer tail/head corrupted!",port_id);
    return ADSP_EFAILED;
  }
  //high water mark detection: if buffer will be filled more than high water mark
  if ( ((tail_head_gap+length) >= high_water_mark) )
  {
    *is_high_water_mark = TRUE;
    //we still write whatever is possible
  }

  /*
   The following computing based on:
   1. dstCircBuf.top_addr <= dstCircBuf.head_addr, dstCircBuf.tail_addr
   < dstCircBuf.bottom_addr
   2. length <= dstCircBuf.buffer_size
   */
  circ_empty_space_tail2bottom = dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->tail_addr;

  if (circ_empty_space_tail2bottom >= (int32_t)length )
  {
    // if tail to bottom has enough space to store
    memscpy(dst_circ_buff_ptr->tail_addr, circ_empty_space_tail2bottom,
            src_linear_buff_ptr, length);

    dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->tail_addr + length;
  }
  else
    // circ_empty_space_tail2bottom < length
  {
    // if tail to bottom does not have enough space to store
    memscpy(dst_circ_buff_ptr->tail_addr, circ_empty_space_tail2bottom,
            src_linear_buff_ptr, circ_empty_space_tail2bottom);

    memscpy(dst_circ_buff_ptr->top_addr,
            (dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->top_addr),
            (src_linear_buff_ptr+circ_empty_space_tail2bottom),
            (length - circ_empty_space_tail2bottom));

    dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr + (length - circ_empty_space_tail2bottom);
  }

  if ((dst_circ_buff_ptr->tail_addr) >= (dst_circ_buff_ptr->bottom_addr) )
  {
    dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr;
  }

  // if dst circular buffer over-run, overwrite the head address.
  if (length >= (dst_circ_buff_ptr->buffer_size - tail_head_gap)) //when empty space is smaller than length we wrote
  {
#ifdef RTPP_MEDIUM_DEBUG
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RTPPDM L2C: Overrun. port=%x",port_id);
#endif

    //qurt_elite_mutex_lock(&dst_circ_buff_ptr->buf_mutex);
    dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->tail_addr + num_bytes; //move ahead head_addr a little
    // Wrap around if pass bottom
    if (dst_circ_buff_ptr->head_addr >= dst_circ_buff_ptr->bottom_addr)
    {
      dst_circ_buff_ptr->head_addr =
          dst_circ_buff_ptr->top_addr
          + (dst_circ_buff_ptr->head_addr - dst_circ_buff_ptr->bottom_addr);
    }
    //qurt_elite_mutex_unlock(&dst_circ_buff_ptr->buf_mutex);
  }
  return ADSP_EOK;
}
