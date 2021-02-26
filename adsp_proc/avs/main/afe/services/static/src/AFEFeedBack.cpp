/*========================================================================
 This file contains AFE loopback topology related code

 Copyright (c) 2012-2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
 QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
 by the U.S. Government, Diversion contrary to U.S. law prohibited.

 $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEFeedBack.cpp#1 $
 ====================================================================== */

/*==========================================================================
 Include files
 ========================================================================== */
#include "AFEFeedback.h"
#include "AFEInternal.h"
#include "AFEPortManagement.h"

/*==========================================================================
 Functions
 ========================================================================== */
/**
 * This function pushes new client in a client list, and it is a general
 * client adding process for port
 *
 * @param[in] pClientList, client link list for port.
 * @param[in] size sizeof the element to be added to the list (bytes)
 * @return the client info, including client
 */
void afe_port_cust_add_to_client_list(feedback_client_list_t **pClientList, feedback_client_list_t *pQNode )
{
  feedback_client_list_t               *list;

  list = *pClientList;
  if (list != NULL)
  {
    /*list is not empty, go to end of list and add new node */
    while (list->next != NULL)
      list = list->next;

    list->next = pQNode;
  }
  else
  {
    /*First element in the node, so update the clientList */
    *pClientList = pQNode;
  }
}


ADSPResult afe_port_set_feedback_path_params(void *dev_port_ptr, uint32_t param_id,
                                             int8_t* params_buffer_ptr, uint16_t param_size)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = (afe_dev_port_t*)dev_port_ptr;
  uint32_t feedback_cfg_version;
  int8_t *cached_cfg_ptr;

  switch(param_id)
  {
    case AFE_PARAM_ID_FEEDBACK_PATH_CFG:
    {
      /* Choose the appropriate config version  */
      feedback_cfg_version = *((uint32_t *)params_buffer_ptr);

      if(feedback_cfg_version > AFE_API_VERSION_FEEDBACK_PATH_CFG)
      {
        feedback_cfg_version = AFE_API_VERSION_FEEDBACK_PATH_CFG;
      }

      if(AFE_API_VERSION_FEEDBACK_PATH_CFG == feedback_cfg_version)
      {
        if(param_size < sizeof(afe_fbsp_feedback_path_cfg_v1_t))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "feedback_path Cfg fail: Invalid payload size: %d, port_id: 0x%x", param_size, pDevPort->intf);
          return ADSP_EBADPARAM;
        }

        afe_fbsp_feedback_path_cfg_v1_t *feedback_cfg_param =
            (afe_fbsp_feedback_path_cfg_v1_t *)params_buffer_ptr;

        if( IS_PSEUDO_PORT_AFE_ID(
            feedback_cfg_param->dst_portid) || IS_PSEUDO_PORT_AFE_ID(pDevPort->intf))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback. dst_port_id 0x%lx and/or src_port_id 0x%x should not be pseudoport!", feedback_cfg_param->dst_portid, pDevPort->intf);
          return ADSP_EBADPARAM;
        }

        /* If Source port is not an Tx port */
        if(AFE_PORT_RX_DIR == pDevPort->dir)
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback. src_port 0x%x should be TX ", pDevPort->intf);
          return ADSP_EBADPARAM;
        }

        /* If destination port is not an Rx port */
        if((feedback_cfg_param->dst_portid & 0x1) != 0)
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback. dst_port_id %ld should be RX ", feedback_cfg_param->dst_portid);
          return ADSP_EBADPARAM;
        }

        /* Validate num channels either 2 or 4 */
        if((feedback_cfg_param->num_channels != 2) && (feedback_cfg_param->num_channels != 4))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback port 0x%lx num channels should be either 2 or 4, set channels %ld",
                feedback_cfg_param->dst_portid, feedback_cfg_param->num_channels );
          return ADSP_EBADPARAM;
        }

        /* Checking for unique chan info */
        int8_t map[4];
        int8_t ch = 0, ch_id = 0;
        memset(&map, 0, sizeof(map));

        for(ch = 0; ch < feedback_cfg_param->num_channels; ch++)
        {
          if((feedback_cfg_param->chan_info[ch] <= 1)
              && (feedback_cfg_param->chan_info[ch] >= 4))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback port 0x%lx, incorrect ch info range",
                  feedback_cfg_param->dst_portid);
            return ADSP_EBADPARAM;
          }

          ch_id = feedback_cfg_param->chan_info[ch] - 1;
          if(1 == map[ch_id]) // already received this id
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback port 0x%lx, duplicate ch info",
                  feedback_cfg_param->dst_portid);
            return ADSP_EBADPARAM;
          }
          map[ch_id] = 1;
        }

        if(2 == feedback_cfg_param->num_channels)
        {
          /* If ch == 2, then channel mapping should have (1,2),(2,1) or (3,4),(4,3) in first two slots
           * notice their sum are unique, 0 not permitted, V and I should always occur in pair */
          if(((feedback_cfg_param->chan_info[0] + feedback_cfg_param->chan_info[1]) != 3)
              && ((feedback_cfg_param->chan_info[0] + feedback_cfg_param->chan_info[1]) != 7))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling feedback port 0x%lx, expected to receive mapping of (1,2),(2,1) or (3,4),(4,3)",
                  feedback_cfg_param->dst_portid);
            return ADSP_EBADPARAM;
          }
        }

        // Allocate memory for feedback path cfg
        if(AFE_PORT_STATE_RUN != pDevPort->port_state)
        {
          if(NULL != pDevPort->feedback_path_info_ptr)
          {
            qurt_elite_memory_free(pDevPort->feedback_path_info_ptr);
            pDevPort->feedback_path_info_ptr = NULL;
          }
          afe_port_multi_feedback_info_t *feedback_path_info_ptr = NULL;

          if(NULL
              == (feedback_path_info_ptr =
                  (afe_port_multi_feedback_info_t*)qurt_elite_memory_malloc(
                      (sizeof(afe_port_multi_feedback_info_t)), QURT_ELITE_HEAP_DEFAULT)))
          {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create feedback path config object");
            return ADSP_ENOMEMORY;
          }
          memset(feedback_path_info_ptr, 0, sizeof(afe_port_multi_feedback_info_t)); //also keeps in disabled & inactive state.
          pDevPort->feedback_path_info_ptr = feedback_path_info_ptr;
        }
        else // for multiple configuration of feedback path, not supporting RTC of FB path right now
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to apply new FB path info, RTC on this is not supported!");
          return ADSP_EBADPARAM;
        }

        // copy all the params: this copy is necessary because, in CONFIG state we do not know sample rate etc that are needed for fully configuring the lib.
        cached_cfg_ptr = (int8_t *)&pDevPort->feedback_path_info_ptr->feedback_cfg;

        memscpy(cached_cfg_ptr, sizeof(afe_fbsp_feedback_path_cfg_v1_t), params_buffer_ptr,
                sizeof(afe_fbsp_feedback_path_cfg_v1_t));

        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Cmd feedback cfg: num_channels %lu, port_id: 0x%x", ((afe_fbsp_feedback_path_cfg_v1_t*)cached_cfg_ptr)->num_channels, pDevPort->intf);
      }
      else
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cmd feedback cfg fail: InCompatible cfg version: %lu, port_id: 0x%x", feedback_cfg_version, pDevPort->intf);
        result = ADSP_EUNSUPPORTED;
      }
    }
    break;

    default:
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Feedback path config, Unsupported Param id ::0x%lx", param_id);
      result = ADSP_EUNSUPPORTED;
      break;
  }

  return result;
}

/**
 * This function is to search for a client and remove it from the list
 *
 * @param[in] ppQNode, pointer to the list,
 * @param[in] svc_handle_ptr, client to be removed 
 * @return feedback_client_list_t, ptr to the node containing 
 *         the current element, NULL if element not found.
 */
feedback_client_list_t* afe_port_remove_custom_client(feedback_client_list_t **ppQNode, elite_svc_handle_t *svc_handle_ptr)
{
  afe_custom_client_info_t *psAfeClientInfo;
  feedback_client_list_t* pQNode;
  int i;

  pQNode = *ppQNode;
  for(i = 0; pQNode != NULL; i++)
  {
    psAfeClientInfo = (afe_custom_client_info_t *)(*ppQNode)->element;
    if(&(psAfeClientInfo->this_client_manager) == svc_handle_ptr)
    {
      *ppQNode = pQNode->next;
      break;
    }
    else
    {
      ppQNode = &(pQNode->next);
      pQNode = *ppQNode;
    }
  }
  return pQNode;
}

/**
 * This function adds an rx client to the list Errors must be
 * returned. Ack will be handled by calling function 
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, pointer to client structure
 * @param[out] list_node, ptr to ptr of list node
 * @param[out] is_queue_init, boolean that indicates whether a queue was created in this function
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_cust_rx_client_connect(afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg,
                                           feedback_client_list_t **list_node, int16_t *is_queue_init, void **buf_q)
{
  afe_custom_client_info_t *new_client_ptr;
  ADSPResult result = ADSP_EOK;
  char dataQName[16];
  elite_msg_custom_afe_cust_client_connect_t *pPayload;
  char *queue_buf_ptr;

  *is_queue_init = FALSE;

  /* Add new client to the list */
  pPayload = (elite_msg_custom_afe_cust_client_connect_t *)(pMsg->pPayload);

  /* Allocated the node to be added to the client list */
  *list_node = (feedback_client_list_t *)qurt_elite_memory_malloc((sizeof(feedback_client_list_t)),
                                                                  QURT_ELITE_HEAP_DEFAULT);
  if(NULL == *list_node)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client list node.");
    return ADSP_ENOMEMORY;
  }

  /* Create a new element, update the client information */
  new_client_ptr = (afe_custom_client_info_t *)qurt_elite_memory_malloc(
      sizeof(afe_custom_client_info_t), QURT_ELITE_HEAP_DEFAULT);
  if(NULL == new_client_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client, port_id: 0x%x", pDevPort->intf);
    return ADSP_ENOMEMORY;
  }

  /* Add the node to linked list */
  (*list_node)->element = (void *)new_client_ptr;
  (*list_node)->next = NULL;

  /* Clear the client structure */
  memset(new_client_ptr, 0, sizeof(afe_custom_client_info_t));

  /* Copy the info from client into new object */
  new_client_ptr->afe_custom_client = pPayload->afe_client;

  /* Update the client ID */
  new_client_ptr->client_id = pDevPort->client_counter.cust_client;

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Rx Cust Client: 0x%x, connect to port_id: 0x%x, channels: %lu",
        new_client_ptr->client_id, pDevPort->intf, new_client_ptr->afe_custom_client.num_channel);

  /* Create data queue them for receiving data from Rx client */
  qurt_elite_strl_cpy(dataQName, "afeRxDatI0D0N0", sizeof(dataQName));
  dataQName[9] = pDevPort->intf + '0';
  dataQName[11] = pDevPort->dir + '0';
  dataQName[13] = pDevPort->num_client + '0';

  queue_buf_ptr = (char*)qurt_elite_memory_malloc(
      QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_FB_DEV_DATQ_MAX), QURT_ELITE_HEAP_DEFAULT);
  if(NULL == queue_buf_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port dataQ Creation Failed");
    return ADSP_ENOMEMORY;
  }

  new_client_ptr->this_client_manager.dataQ = (qurt_elite_queue_t*)queue_buf_ptr;

  qurt_elite_channel_init(&new_client_ptr->qurt_elite_channel);

  /* Initialize the queue and add it to the channel */
  if(ADSP_FAILED(
      result = qurt_elite_queue_init(dataQName, AFE_FB_DEV_DATQ_MAX, new_client_ptr->this_client_manager.dataQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(&new_client_ptr->qurt_elite_channel, new_client_ptr->this_client_manager.dataQ, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to init dataQ or add dataQ for client to channel = %d!!\n", result);
    *is_queue_init = TRUE;
    return result;
  }

  *is_queue_init = TRUE;

  /* Create new handle for the client, update the cmdQ,
   * and respond back to the client with new handle.
   */
  new_client_ptr->this_client_manager.unSvcId = ELITE_MIXER_SVCID;
  new_client_ptr->this_client_manager.threadId = 0;
  new_client_ptr->this_client_manager.cmdQ = afe_svc_global_ptr->static_svc_cmdq_ptr;
  pPayload->svc_handle_ptr = &(new_client_ptr->this_client_manager);

  /* Add the client to the list */
  afe_port_cust_add_to_client_list(&(pDevPort->afe_cust_client_list_ptr), *list_node);

  /* Update the number of clients connected to this port */
  pDevPort->num_client += 1;

  /* Increment the custom client counter */
  pDevPort->client_counter.cust_client++;

  *buf_q = (void*)&new_client_ptr->bufQ;

  return ADSP_EOK;
}

/**
 * to disconnect rx clients
 */
void afe_port_cust_rx_client_disconnect_handler(afe_dev_port_t* pDevPort, feedback_client_list_t *list_node,
                                                elite_msg_any_t *pMsg, int16_t is_deinit_queue)
{
  afe_custom_client_info_t *client_info_ptr;

  if(NULL != list_node)
  {
    client_info_ptr = (afe_custom_client_info_t *)list_node->element;

    if(NULL != client_info_ptr)
    {
      /* Drain any queued buffers and de-init the data queue */
      if(is_deinit_queue)
      {
        if(client_info_ptr->this_client_manager.dataQ != NULL)
        {
          elite_msg_any_t   dataQMsg;
          ADSPResult result;
          do
          {
            // Non-blocking MQ receive
            result = qurt_elite_queue_pop_front(client_info_ptr->this_client_manager.dataQ, (uint64_t *)&dataQMsg);

            // return the buffer to its rightful q.
            if(ADSP_EOK == result)
            {
              qurt_elite_queue_push_back(client_info_ptr->bufQ, (uint64_t*)&dataQMsg);
            }

          } while(ADSP_EOK == result);

          qurt_elite_memory_free(client_info_ptr->this_client_manager.dataQ);
          client_info_ptr->this_client_manager.dataQ = NULL;
        }
      }


      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Custom Rx Client_id: 0x%x disconnected from port_id: 0x%x",
            client_info_ptr->client_id, pDevPort->intf);

      /* Free up the client object */
      qurt_elite_memory_free(client_info_ptr);
    }

    /* Free up the client list node */
    qurt_elite_memory_free(list_node);
  }

}

/**
 * This function adds new custom client to Tx port 
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, connect message
 * @param[out] list_node, ptr to ptr of list node 
 * @param[out] is_queue_init, boolean that indicates whether a queue was created in this function
 * @param[out] num_bufs_created, number of buffers created
 * @return the ADSP_EOK error code
 */
static ADSPResult afe_port_cust_tx_client_connect(
    afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg, feedback_client_list_t **list_node, int16_t *is_queue_init,
    afe_feedback_path_t *feedback_path_ptr, void **buf_q)
{
  afe_custom_client_info_t *ptr_new_client = NULL;
  elite_msg_custom_afe_cust_client_connect_t *pPayload;
  ADSPResult result = ADSP_EOK;
  char bufQName[16];
  char *queue_buf_ptr;

  if(!(pDevPort && pMsg && list_node && is_queue_init && feedback_path_ptr && buf_q))
  {
    MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed null pointer passed: pDevPort(%p), pMsg(%p), list_node(%p), is_queue_init(%p), feedback_path_ptr(%p), buf_q(%p)",
          pDevPort, pMsg, list_node, is_queue_init, feedback_path_ptr, buf_q);
    return ADSP_EFAILED;
  }

  *is_queue_init = FALSE;

  // Add new client to linked list
  pPayload = (elite_msg_custom_afe_cust_client_connect_t *)(pMsg->pPayload);

  *list_node = (feedback_client_list_t *)qurt_elite_memory_malloc((sizeof(feedback_client_list_t)),
                                                                  QURT_ELITE_HEAP_DEFAULT);
  if(NULL == *list_node)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client list node.");
    return ADSP_ENOMEMORY;
  }

  ptr_new_client = (afe_custom_client_info_t *)qurt_elite_memory_malloc(
      sizeof(afe_custom_client_info_t), QURT_ELITE_HEAP_DEFAULT);

  if(NULL == ptr_new_client)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client, port_id: 0x%x", pDevPort->intf);
    return ADSP_ENOMEMORY;
  }

  /* Add to linked list element */
  (*list_node)->element = (void *)ptr_new_client;
  (*list_node)->next = NULL;

  /* Clear the client object */
  memset(ptr_new_client, 0, sizeof(afe_custom_client_info_t));

  /* Copy the client info to new object */
  ptr_new_client->afe_custom_client = pPayload->afe_client;
  ptr_new_client->client_id = pDevPort->client_counter.cust_client;

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Tx Custom Client: 0x%x, connect to port_id: 0x%x, channels: %lu",
        ptr_new_client->client_id, pDevPort->intf, ptr_new_client->afe_custom_client.num_channel);

  /* Create data queue them for receiving data from Rx client */
  /* Create new buffers, buffQ and queue the buffers in bufQ. */
  qurt_elite_strl_cpy(bufQName, "afeTxBufI0D0N0", sizeof(bufQName));
  bufQName[9] = pDevPort->intf + '0';
  bufQName[11] = pDevPort->dir + '0';
  bufQName[13] = pDevPort->num_client + '0';

  queue_buf_ptr = (char *)qurt_elite_memory_malloc(
      QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_FB_DEV_BUFQ_MAX), QURT_ELITE_HEAP_DEFAULT);

  if(NULL == queue_buf_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Tx , BufQ Creation failed, port_id: 0x%x",
          pDevPort->intf);
    return ADSP_ENOMEMORY;
  }

  ptr_new_client->bufQ = (qurt_elite_queue_t*)queue_buf_ptr;

  qurt_elite_channel_init(&ptr_new_client->qurt_elite_channel);

  if(ADSP_FAILED(
      result = qurt_elite_queue_init(bufQName, AFE_FB_DEV_BUFQ_MAX, ptr_new_client->bufQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(&ptr_new_client->qurt_elite_channel, ptr_new_client->bufQ, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Tx Custom client_id: 0x%x, failed to initialize bufQ for client or add bufQ to channel",
          ptr_new_client->client_id);
    *is_queue_init = TRUE;
    return result;
  }

  *is_queue_init = TRUE;

  /* Create new handle for the client, update the cmdQ,
   * and respond back to the client with new handle.
   */
  ptr_new_client->this_client_manager.unSvcId = ELITE_MIXER_SVCID;
  ptr_new_client->this_client_manager.threadId = pDevPort->thread_id;
  ptr_new_client->this_client_manager.cmdQ = afe_svc_global_ptr->static_svc_cmdq_ptr;
  feedback_path_ptr->src_port_handle_ptr = &(ptr_new_client->this_client_manager);
  ptr_new_client->afe_custom_client.client_handle = feedback_path_ptr->dst_port_handle_ptr; //[client_type];

  /* Add the client to the list */
  afe_port_cust_add_to_client_list(&(pDevPort->afe_cust_client_list_ptr), *list_node);

  pDevPort->num_client += 1;

  /* Increment client counter */
  pDevPort->client_counter.cust_client++;
  feedback_path_ptr->src_port_client_ptr = (void *)ptr_new_client;

  qurt_elite_queue_t **buf_q_ptr;

  buf_q_ptr = (qurt_elite_queue_t **)(*buf_q);

  *buf_q_ptr = ptr_new_client->bufQ;

  return ADSP_EOK;
}

/**
 * to disconnect tx clients
 */
void afe_port_cust_tx_client_disconnect_handler(afe_dev_port_t* pDevPort, feedback_client_list_t *list_node,
                                                elite_msg_any_t *pMsg, int16_t is_deinit_queue)
{
  afe_custom_client_info_t *client_info_ptr;

  if(NULL != list_node)
  {
    client_info_ptr = (afe_custom_client_info_t *)list_node->element;

    if(NULL != client_info_ptr)
    {
      /* Free up all the buffers and de-init the output buffer queue */
      if(is_deinit_queue)
      {
        elite_svc_deinit_buf_queue(client_info_ptr->bufQ,
                                   client_info_ptr->afe_custom_client.num_buffers);
      }

      /* Free up the output buffer queue */
      if(NULL != client_info_ptr->bufQ)
      {
        qurt_elite_memory_free(client_info_ptr->bufQ);
        client_info_ptr->bufQ = NULL;
      }

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Tx custom Client_id: 0x%x disconnected from port_id: 0x%x",
            client_info_ptr->client_id, pDevPort->intf);

      /* Free up the client object */
      qurt_elite_memory_free(client_info_ptr);
    }

    /* Free up the client list node */
    qurt_elite_memory_free(list_node);
  }

  return;
}

/**
 * This function is to client-disconnect processing
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_cust_client_disconnect_handler(afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_custom_afe_cust_client_connect_t *pPayload;
  elite_svc_handle_t *svc_handle_ptr;
  feedback_client_list_t *list_node = NULL;

  pPayload = (elite_msg_custom_afe_cust_client_connect_t *)(pMsg->pPayload);
  svc_handle_ptr = pPayload->svc_handle_ptr;

  qurt_elite_thread_set_prio(elite_get_thrd_prio(ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID));
  qurt_elite_mutex_lock(&(pDevPort->port_mutex));

  if(NULL != pDevPort->afe_cust_client_list_ptr)
  {
    /* Search client node and remove it from the list */
    list_node = afe_port_remove_custom_client(&(pDevPort->afe_cust_client_list_ptr), svc_handle_ptr);
    if(NULL != list_node)
    {
      pDevPort->num_client--;
      pDevPort->client_counter.cust_client--;
    }

    /* Call the client discnonect handler as per port direction */
    if(RX_DIR == pDevPort->dir)
    {
      afe_port_cust_rx_client_disconnect_handler(pDevPort, list_node, pMsg, TRUE);
    }
    else
    {
      afe_port_cust_tx_client_disconnect_handler(pDevPort, list_node, pMsg, TRUE);
    }
  }

  qurt_elite_mutex_unlock(&(pDevPort->port_mutex));
  qurt_elite_thread_set_prio(elite_get_thrd_prio(ELITETHREAD_STAT_AFE_DEV_SVC_PRIO_ID)); // May not be needed to revert to bg thread priority. Check this??

  if(NULL == list_node)
  {
    result = ADSP_EFAILED;
  }

  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Client Disconnect failed: Unknown client");
    elite_svc_send_ack(pMsg, result);
  }
  return result;
}

/**
 * This function adds a new client to the linked list. And it calls
 * appropriate functions to start book-keeping of the client
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg from client
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_cust_client_connect_handler(
    afe_dev_port_t* pDevPort, elite_msg_any_t* pMsg,
    afe_feedback_path_t *feedback_path_ptr, void **buf_q)
{
  ADSPResult result;
  feedback_client_list_t *list_node = NULL;
  int16_t is_queue_init = FALSE;
  int16_t th_prio;

  th_prio = qurt_elite_thread_prio_get();

  qurt_elite_thread_set_prio(elite_get_thrd_prio(ELITETHREAD_STAT_AFE_PORT_MANAGER_PRIO_ID));
  qurt_elite_mutex_lock(&(pDevPort->port_mutex));

  if(AFE_PORT_TX_DIR == pDevPort->dir)
  {
    if(ADSP_FAILED(
        result = afe_port_cust_tx_client_connect(pDevPort, pMsg, &list_node, &is_queue_init,
                                                 feedback_path_ptr, buf_q)))
    {
      afe_port_cust_tx_client_disconnect_handler(pDevPort, list_node, pMsg, is_queue_init);
    }
  }
  else /* Rx port */
  {
    if(ADSP_FAILED(
        result = afe_port_cust_rx_client_connect(pDevPort, pMsg, &list_node, &is_queue_init, buf_q)))
    {
      afe_port_cust_rx_client_disconnect_handler(pDevPort, list_node, pMsg, is_queue_init);
    }
  }

  qurt_elite_mutex_unlock(&(pDevPort->port_mutex));
  qurt_elite_thread_set_prio(th_prio);

  return result;
}

/**
 This function is to enable feedback path

 @param[in] dev_port_ptr Pointer to the instance of the port.
 @param[in] feedback_path_info_ptr  pointer to the feedback
 path info structure

 @return
 ADSP_EOK error code.

 @dependencies
 None.
 */

ADSPResult afe_port_enable_feedback_path(uint32_t src_id,
                                         afe_fbsp_feedback_path_cfg_v1_t *feedback_path_cfg_ptr,
                                         uint32_t *feedback_handle_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_custom_client_t *pAfeCustClient = NULL;
  afe_dev_port_t *pRxPort = NULL;
  afe_dev_port_t *pDevPort = NULL;
  afe_feedback_path_t *feedback_path_ptr= NULL;
  elite_msg_custom_afe_cust_client_connect_t *pConnectPayload;
  elite_msg_custom_afe_cust_client_connect_t fb_connect_payload;
  elite_msg_any_t msg;

  if(!(src_id && feedback_path_cfg_ptr && feedback_handle_ptr))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFE feedback path: failed NULL, src_id(0x%lx) feedback_path_cfg_ptr(0x%lx) FB handle ptr(0x%lx)",
          src_id, (uint32_t)feedback_path_cfg_ptr, (uint32_t)feedback_handle_ptr);
    return ADSP_EFAILED;
  }

  result = afe_svc_get_port(src_id, &pDevPort);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE feedback path: failed to get src tx port : 0x%lx", src_id);
    return ADSP_EFAILED;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "AFE Feedback path: connecting port_id: 0x%x to port_id: 0x%lx",
        pDevPort->intf, feedback_path_cfg_ptr->dst_portid);

  /* Connect message */
  pConnectPayload = &fb_connect_payload;

  /* Config the structure of the msg payload afeclient */
  pAfeCustClient = &(pConnectPayload->afe_client);
  pAfeCustClient->num_channel = feedback_path_cfg_ptr->num_channels;

  memscpy(pAfeCustClient->channel_mapping, sizeof(pAfeCustClient->channel_mapping),
          feedback_path_cfg_ptr->chan_info, AFE_FBSP_VI_CHAN_MAX * sizeof(int32_t));

  pAfeCustClient->buf_size = 0;
  pAfeCustClient->num_buffers = 0;

  /* Give the tx port handle to rx port for feedback */
  pAfeCustClient->client_handle = NULL;

  /* Config the structure of the msg payload */
  pConnectPayload->afe_id = feedback_path_cfg_ptr->dst_portid;
  pConnectPayload->responseq_ptr = NULL;
  pConnectPayload->client_token = 0;
  pConnectPayload->buffer_returnq_ptr = NULL;

  msg.unOpCode = ELITE_CUSTOM_MSG;
  msg.pPayload = (void*)&fb_connect_payload;

  /* Call connect function to dst rx port */
  result = afe_svc_get_port(feedback_path_cfg_ptr->dst_portid, &pRxPort);

  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE feedback path: failed to get dst rx port : 0x%lx", feedback_path_cfg_ptr->dst_portid);
  }
  else
  {
    void *buf_q_ptr = NULL;

    result = afe_port_cust_client_connect_handler(pRxPort, &msg, NULL, &buf_q_ptr);

    if(ADSP_FAILED(result))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdFeedback path Enable, connect to dst rx port : 0x%lx failed", feedback_path_cfg_ptr->dst_portid);
    }
    else
    {
      /*Allocate the FB path connection */
      if(pDevPort->client_counter.cust_client >= AFE_MAX_MODULES_PER_TOPOLOGY)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed number of feedback paths(%d) allowed is exceeded!", pDevPort->client_counter.cust_client);
        return ADSP_ENOMEMORY;
      }

      if(NULL
          == (feedback_path_ptr = (afe_feedback_path_t *)qurt_elite_memory_malloc((sizeof(afe_feedback_path_t)), QURT_ELITE_HEAP_DEFAULT)))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for feedback path!");
        return ADSP_ENOMEMORY;
      }
      memset(feedback_path_ptr, 0, sizeof(afe_feedback_path_t));

      /* Get the dst rx-port client handle, and store it in feedback path info. */
      feedback_path_ptr->dst_port_handle_ptr = pConnectPayload->svc_handle_ptr;

      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Feedback path Enable, connect to dst Rx port : 0x%lx success", feedback_path_cfg_ptr->dst_portid);

      /* Connect to source port client list */
      if(ADSP_SUCCEEDED(
          result = afe_port_cust_client_connect_handler(pDevPort, &msg, feedback_path_ptr, &buf_q_ptr)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdFeedback path Enable, connect to src port : 0x%x success", pDevPort->intf);
        *feedback_handle_ptr = (uint32_t)feedback_path_ptr;
      }
      else
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdFeedback path Enable, connect to src port : 0x%x failed", pDevPort->intf);
        afe_port_cust_client_disconnect_handler(pDevPort, &msg);
        qurt_elite_memory_free(feedback_path_ptr);
      }
    }/* if connect to RX port failed */

  } /* if getting rx port failed */
  return result;
}

/**
 This function is to disable feedback path

 @param[in] dev_port_ptr Pointer to the instance of the port.
 @param[in] feedback_path_info_ptr  pointer to the feedback
 path info structure

 @return
 ADSP_EOK error code.

 @dependencies
 None.
 */

ADSPResult afe_port_disable_feedback_path(uint32_t src_id, uint32_t dest_id, uint32_t feedback_handle)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_custom_afe_cust_client_connect_t *pPayload;
  elite_msg_any_t msg;
  afe_dev_port_t *pRxPort;
  int nActualSize;
  qurt_elite_bufmgr_node_t bufMgrNode;
  afe_dev_port_t *pDevPort = NULL;

  if(!(src_id && dest_id && feedback_handle))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE feedback path: failed NULL, FB handle(0x%lx) src_id(0x%lx) dest_id(0x%lx)",
          feedback_handle, src_id, dest_id);
    return ADSP_EFAILED;
  }

  result = afe_svc_get_port(src_id, &pDevPort);
  if(ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE feedback path: failed to get src tx port : 0x%lx", src_id);
    return ADSP_EFAILED;
  }

  if(ADSP_FAILED(
      result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_cust_client_connect_t), &bufMgrNode,
                                    &nActualSize)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AfeFeedback msg payload buffer NOT obtained.");
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AfeFeedback msg payload buffer %p.",bufMgrNode.pBuffer);
    /* Config the disconnect message payload */
    pPayload = (elite_msg_custom_afe_cust_client_connect_t*)bufMgrNode.pBuffer;
    pPayload->afe_id = dest_id;
    pPayload->responseq_ptr = NULL;
    pPayload->client_token = 0;
    pPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
    pPayload->sec_op_code = ELITEMSG_CUSTOM_AFE_CUST_CLIENT_DISCONNECT_REQ;
    pPayload->svc_handle_ptr = ((afe_feedback_path_t *)feedback_handle)->dst_port_handle_ptr;

    msg.unOpCode = ELITE_CUSTOM_MSG;
    msg.pPayload = (void*)pPayload;

    if(ADSP_EOK != (result = afe_svc_get_port(dest_id, &pRxPort)))
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "feedback path disable: failed to get rx port");
    }
    else
    {
      //call disconnect to rx port
      if(ADSP_EOK != (result = afe_port_cust_client_disconnect_handler(pRxPort, &msg)))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "feedback path disable: failed to disconnect from rx port");
      }
      else //disconnect from RX success
      {
        pPayload->svc_handle_ptr = ((afe_feedback_path_t *)feedback_handle)->src_port_handle_ptr;
        //free the feedback path memory
        qurt_elite_memory_free((void *)feedback_handle);

        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "feedback path disable: disconnect from rx port success");
        // disconnect from Tx port
        if(ADSP_SUCCEEDED(result = afe_port_cust_client_disconnect_handler(pDevPort, &msg)))
        {
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "feedback path disable: disconnect src port from tx client success");
        }
        else
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "feedback path disable: disconnect src port from tx client failed");
        }
      }
    } //get port failed

    elite_msg_return_payload_buffer(&msg);
  } //get BufMgr node failed

  return result;
}
