/*========================================================================
   This file contains AFE loopback topology related code

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFELoopBack.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInternal.h"
#include "AFEPortManagement.h"
#include "AFERateMatch.h"
#include "AFESidetoneIIR.h"

/*==========================================================================
  Functions
  ========================================================================== */
ADSPResult afe_port_alloc_resources_for_loopback_module(afe_dev_port_t *pDevPort)
{
   ADSPResult result = ADSP_EOK;
   uint16_t vote_now = FALSE;
   if(AFE_PORT_STATE_RUN == pDevPort->port_state)
   {
      vote_now = TRUE;
   }

   if(NULL == pDevPort->loopback_info_ptr)
   {
      afe_port_loopback_info *loopback_info_ptr = NULL;
      if(NULL
            == (loopback_info_ptr = (afe_port_loopback_info*)qurt_elite_memory_malloc(
                  (sizeof(afe_port_loopback_info)), QURT_ELITE_HEAP_DEFAULT)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create loopback info struct object");
         return ADSP_ENOMEMORY;
      }

      /* Do memset of looback_info structure to zero, which clears any garbage data */
      memset(loopback_info_ptr, 0, sizeof(afe_port_loopback_info));

      if(ADSP_EOK
            != (result = afe_mmpm_voting(pDevPort, AFE_MMPM_LOOPBACK_TOPOLOGY,
                                         AFE_MMPM_VOTE, vote_now)))
      {
         afe_mmpm_voting(pDevPort, AFE_MMPM_LOOPBACK_TOPOLOGY, AFE_MMPM_DEVOTE,
                         vote_now);
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "MMPM voting failed while enabling loopback module: 0x%x", pDevPort->intf);
         qurt_elite_memory_free(loopback_info_ptr);
         return result;
      }
      loopback_info_ptr->afe_loopback_state = AFE_PORT_LOOPBACK_STATE_STOP;
      pDevPort->loopback_info_ptr = loopback_info_ptr;
   }

   return result;
}

void afe_port_dealloc_resources_for_loopback_module(afe_dev_port_t *pDevPort)
{
  afe_port_loopback_info *loopback_info_ptr = pDevPort->loopback_info_ptr;

  /* if side tone IIR is enabled */
  if(NULL != pDevPort->loopback_info_ptr->sidetone_iir_info_ptr)
  {
    afe_sidetone_iir_dealloc_resource(pDevPort);
  }

  pDevPort->loopback_info_ptr = NULL;

  if(NULL != loopback_info_ptr)
  {
    afe_mmpm_voting(pDevPort, AFE_MMPM_LOOPBACK_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
    qurt_elite_memory_free(loopback_info_ptr);
  }

  return;
}

/**
* This function adds a loopback rx client for the specific tx port.
*
* @param[in] pDevPort, pointer to instance of the port
* @param[in] pMsg, msg from client
* @return the ADSP_EOK error code
*/
ADSPResult afe_port_client_loopback_connect_handler(afe_dev_port_t* pDevPort, elite_msg_any_t* pMsg)
{
   ADSPResult       result = ADSP_EOK;
   bufQList         *list_node     = NULL;
   int16_t          is_queue_init  = FALSE;
   bool_t           is_rate_match;

   volatile uint64_t *tx_port_acc_avt_drift_info = NULL;

   elite_msg_custom_afe_loopback_connect_t *pAfeLpConnectMsg   = (elite_msg_custom_afe_loopback_connect_t *) pMsg->pPayload;

   //copy so that when ack is done & the client side releases pAfeLpConnectMsg, we are not reading junk.
   is_rate_match              = pAfeLpConnectMsg->is_rate_match;
   tx_port_acc_avt_drift_info = pAfeLpConnectMsg->tx_port_acc_avt_drift_info;

   //ack is sent from afe_port_add_to_rx_client_list.
   if (ADSP_FAILED(result = afe_port_add_to_rx_client_list(pDevPort, pMsg, &list_node, &is_queue_init)))
   {
      afe_port_rx_client_disconnect_handler(pDevPort,list_node, pMsg, is_queue_init);
      return result;
   }

   if (NULL != list_node && TRUE == is_rate_match)
   {
      afe_client_info_t *afe_client_info_ptr       = (afe_client_info_t*) list_node->element;
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Loopback Rate match enabled: rx_port_id=%x,client_id=0x%lX", pDevPort->intf, afe_client_info_ptr->client_id);

      afe_rate_match_info_t *rate_match_info_ptr   = (afe_rate_match_info_t*)qurt_elite_memory_malloc((sizeof(afe_rate_match_info_t)), QURT_ELITE_HEAP_DEFAULT);
      if (NULL == rate_match_info_ptr)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "No memory for rate_match_info_ptr port_id=%x,client_id=0x%lX", pDevPort->intf,afe_client_info_ptr->client_id);
         return ADSP_ENOMEMORY;
      }

      memset(rate_match_info_ptr, 0, sizeof(afe_rate_match_info_t));

      rate_match_info_ptr->client_avt_drift_info   = tx_port_acc_avt_drift_info;

      if (ADSP_FAILED(result = afe_port_rate_match_info_reinit(pDevPort, afe_client_info_ptr, rate_match_info_ptr)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error in afe_port_rate_match_info_reinit port_id=%x,client_id=0x%lX", pDevPort->intf,afe_client_info_ptr->client_id);
         qurt_elite_memory_free(rate_match_info_ptr);
		 rate_match_info_ptr = NULL;
         afe_port_rx_client_disconnect_handler(pDevPort,list_node, pMsg, is_queue_init);
         return result;
      }

      //no need to lock mutex, because, this is just one value. read/write is atomic
      afe_client_info_ptr->rate_match_info_ptr = rate_match_info_ptr;
   }

   return result;
}

/**
 * This function is disconnect and reconnect to Rx port for AFE 
 * loopback 
 *
 * @param[in] pDevPort, Tx port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_reconnect_rx_loopback_client(afe_dev_port_t *pDevPort)
{
  ADSPResult                                result = ADSP_EOK;
  elite_msg_any_t                           msg;
  afe_client_t                              *rx_port_loopback_client = NULL;
  afe_dev_port_t                            *pRxPort = NULL;
  afe_port_loopback_info                    *loopback_info_ptr;
  elite_msg_custom_afe_connect_t            *pConnectPayload;
  elite_msg_custom_afe_loopback_connect_t   lb_connect_payload;

  loopback_info_ptr = pDevPort->loopback_info_ptr;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Reconfig LoopBack: reconnecting: src_port_id: 0x%X, dst_port_id: 0x%X",pDevPort->intf,loopback_info_ptr->dst_port_id);

  /* Config the disconnect message payload */
  pConnectPayload                      = &lb_connect_payload.connect_msg;

  pConnectPayload->afe_id              =  loopback_info_ptr->dst_port_id;

  pConnectPayload->responseq_ptr       =  NULL;
  pConnectPayload->client_token        =  0;
  pConnectPayload->buffer_returnq_ptr  =  NULL;
  pConnectPayload->sec_op_code         =  ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
  pConnectPayload->svc_handle_ptr      =  loopback_info_ptr->dst_port_handle_ptr;

  msg.unOpCode                  =  ELITE_CUSTOM_MSG;
  msg.pPayload                  =  (void*) pConnectPayload;

  /* Fetch the Rx port as per the port ID */
  result = afe_svc_get_port(loopback_info_ptr->dst_port_id, &pRxPort);

  if (ADSP_FAILED( result ) )
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE LoopBack Reconnect: Failed to get dest Rx port");
  }
  else
  {
    /* Call to disconnect from Rx port */
    result = afe_port_client_disconnect_handler(pRxPort, &msg);

    if ( ADSP_FAILED(result) )
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE LoopBack Reconnect : Failed to disconnect from dest Rx port");
    }
    else
    {
      /* If successfully disconnected from Rx port, reconnect to Rx port */
      /* Config the structure of the msg payload afeclient */
      rx_port_loopback_client                      = &(pConnectPayload->afe_client);
      memset(rx_port_loopback_client, 0, sizeof(afe_client_t));
      rx_port_loopback_client->interleaved         = FALSE;
      rx_port_loopback_client->buf_size            = AFE_MAX_SAMPLE_RATE;  //not used
      rx_port_loopback_client->bytes_per_channel   = pDevPort->bytes_per_channel;
      rx_port_loopback_client->bit_width   		= pDevPort->bit_width;
      rx_port_loopback_client->sample_rate         = pDevPort->sample_rate;
      rx_port_loopback_client->cust_proc.subscribe_to_avt_drift = FALSE;

      /* Give the tx port handle to rx port for loopback */
      rx_port_loopback_client->afe_client_handle   = NULL;
      rx_port_loopback_client->afe_dp_delay_ptr    = NULL;

      /* Config the structure of the msg payload */
      pConnectPayload->afe_id                = loopback_info_ptr->dst_port_id;

      pConnectPayload->responseq_ptr         = NULL;
      pConnectPayload->client_token          = 0;
      pConnectPayload->buffer_returnq_ptr    = NULL;
      pConnectPayload->sec_op_code           = ELITEMSG_CUSTOM_AFE_LOOPBACK_CONNECT_REQ;

      if(LB_MODE_SIDETONE == loopback_info_ptr->routing_mode)
      {
        rx_port_loopback_client->cust_proc.resampler_type = IIR_BASED_SRC;
        rx_port_loopback_client->cust_proc.is_it_sidetone = TRUE;
        rx_port_loopback_client->data_path                = AFE_RX_IN;
        rx_port_loopback_client->channel                = 1;
      }
      else
      {
        rx_port_loopback_client->cust_proc.resampler_type = FIR_BASED_SRC;
        rx_port_loopback_client->data_path                = AFE_RX_AUD_MIXER_IN;
        rx_port_loopback_client->channel                = pDevPort->channels;
      }

      if (loopback_info_ptr->rate_matching_enable)
      {
        rx_port_loopback_client->num_buffers            = AFE_LOOPBACK_NUM_CLIENT_BUFS;
        lb_connect_payload.tx_port_acc_avt_drift_info   = &(pDevPort->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info);
      }
      else
      {
         rx_port_loopback_client->num_buffers                           = 2;
         lb_connect_payload.tx_port_acc_avt_drift_info                  = NULL;
      }    
      lb_connect_payload.is_rate_match                               = loopback_info_ptr->rate_matching_enable;

      msg.unOpCode                    = ELITE_CUSTOM_MSG;
      msg.pPayload                    = (void*) &lb_connect_payload;

      /* Call to reconnect to Rx port */
      if (ADSP_EOK != (result = afe_port_client_loopback_connect_handler(pRxPort, &msg)))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE LoopBack Reconnect : Connect to dest Rx port failed!");
      }
      else
      {
        /* Get the rx-port client handle, and store it in loopback info. */
        loopback_info_ptr->dst_port_handle_ptr = pConnectPayload->svc_handle_ptr;

        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE LoopBack Reconnect: Reconnect to dest Rx port success");
      } /* connected to Rx port */

    } /* successfully disconnected from Rx port */

  } /* successfully got the Rx port */

  return result;
}

/**
 * This function is to destroy the Tx client output queue 
 *  
 * @param[in] pDevPort, Tx port instance
 * @param[in] psAfeClientInfo, client instance
 * @param[in] is_deinit_queue, to de-init the queue or not
 * @param[out] num_buffers_created, number of buffers actually created 
 */
void afe_port_destroy_loopback_client_buf(afe_dev_port_t *pDevPort, afe_client_info_t *psAfeClientInfo,
                                                int16_t is_deinit_queue, uint32_t num_buffers)
{
   ADSPResult 	result = ADSP_EOK;
   
   if (NULL != psAfeClientInfo)
   {
      /* Return the buffers which Tx port client could be holding */
      if (psAfeClientInfo->remaining_samples != 0)
      {
         if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(psAfeClientInfo->bufQ, \
                                                                    (elite_msg_any_payload_t*) psAfeClientInfo->ps_curr_pcm_buff)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortHandler: thread encountered qurt_elite_queue_push_back error = %d!!\n", result);
         }
      }
       
      /* Destroy the bufQ */
      if (is_deinit_queue)
      {
         elite_svc_deinit_buf_queue(psAfeClientInfo->bufQ, num_buffers);
      }

      qurt_elite_memory_free(psAfeClientInfo->bufQ);
      psAfeClientInfo->bufQ = NULL;
   }
}

/**
 * This function is to add the loopback client in Tx-Port
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_loopback_connect_to_src_port(afe_dev_port_t* pDevPort, afe_client_list **list_node, int16_t* is_queue_init,
                                                 uint32_t *num_bufs_created, uint16_t routing_mode )
{
    ADSPResult                     result = ADSP_EOK;
    afe_port_loopback_info         *loopback_info_ptr;
    afe_client_t                   *pAfeClient;
    afe_client_info_t              *psNewClient;

    //Get the loopback_info_ptr
    loopback_info_ptr = pDevPort->loopback_info_ptr;

    *list_node = (afe_client_list *) qurt_elite_memory_malloc((sizeof(afe_client_list)), QURT_ELITE_HEAP_DEFAULT);

    if (NULL == *list_node)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client list node.");
       return ADSP_ENOMEMORY;
    }

    // Add new client to src-port linked list
    psNewClient = (afe_client_info_t*) qurt_elite_memory_malloc(sizeof(afe_client_info_t), QURT_ELITE_HEAP_DEFAULT);

    if (NULL == psNewClient)
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client .");
       return ADSP_ENOMEMORY;
    }
    //Add to linked list element
    (*list_node)->element = (void*)psNewClient;
    (*list_node)->next = NULL;

    memset(psNewClient, 0, sizeof(afe_client_info_t));

    pAfeClient                         = &(psNewClient->afe_client);

    /* Contruct the tx client information */
    pAfeClient->afe_client_handle      = loopback_info_ptr->dst_port_handle_ptr;
    pAfeClient->sample_rate            = pDevPort->sample_rate;
    pAfeClient->bytes_per_channel      = pDevPort->bytes_per_channel;
    pAfeClient->bit_width      			= pDevPort->bit_width;
    pAfeClient->interleaved            = FALSE;
    pAfeClient->data_type              = LINEAR_PCM;
    // Max 48k, 16k, 8k, and 4 channels in tx port.
    pAfeClient->buf_size               = pDevPort->int_samples_per_period;
    pAfeClient->num_buffers            = *num_bufs_created;
    pAfeClient->cust_proc.subscribe_to_avt_drift = FALSE;

    // create new handle for the client, update the cmdQ,
    psNewClient->int_samples_per_period         = pDevPort->int_samples_per_period;
    psNewClient->buf_size                       = pAfeClient->buf_size;
    psNewClient->this_client_manager.unSvcId    = ELITE_MIXER_SVCID;
    psNewClient->this_client_manager.threadId   = pDevPort->thread_id;
    psNewClient->this_client_manager.cmdQ       = afe_svc_global_ptr->static_svc_cmdq_ptr;
    loopback_info_ptr->src_port_handle_ptr       = &(psNewClient->this_client_manager);
    memset(&psNewClient->samp_rate_conv, 0, sizeof(samp_rate_conv_t));
    psNewClient->samp_rate_conv.resamp_type = INVALID_RESAMP_TYPE;
    psNewClient->unNumResamplers                = 0;


    if(LB_MODE_SIDETONE == routing_mode)
    {
      pAfeClient->cust_proc.resampler_type = IIR_BASED_SRC;
      pAfeClient->channel = 1;
    }
    else
    {
      pAfeClient->cust_proc.resampler_type = FIR_BASED_SRC;
      pAfeClient->channel = pDevPort->channels;
    }

    /* Create src port client buffer only if the src port is already in RUN state */
    if(AFE_PORT_STATE_RUN == pDevPort->port_state)
    {
       if (ADSP_FAILED(result = afe_port_create_buf(pDevPort, psNewClient, 1, 0, is_queue_init, num_bufs_created)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to afe_port_create_buf = %d!!\n", result);
          return result;
       }
    }

	/* Send the media type to the client */
    afe_port_send_media_t(pDevPort, psNewClient);

    /*afe_port_update_aggregated_client_kpps_and_bw() checks for supported clients to update 
      client kpps and bw, for this client data path need to be updated in advance*/

    /* Configure the client data path for Tx client of the source port in loopback
       Routing mode values have been validated before calling loopback
       enable
    */
    switch(routing_mode)
    {
      case LB_MODE_DEFAULT:
      case LB_MODE_EC_REF_VOICE_AUDIO:
      {
        pAfeClient->data_path = AFE_PP_OUT;
      }
      break;
      case LB_MODE_SIDETONE:
      {
        pAfeClient->data_path = AFE_TX_OUT;
      }
      break;
    }

    /**If port is running, vote for connecting client immediately, else, port init during port start will
      vote for this client*/
    if(AFE_PORT_STATE_RUN == pDevPort->port_state)
    {
       afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psNewClient, TRUE);
       if(ADSP_FAILED(result = afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "AFE MMPM request failed for AFE_MMPM_CLIENT_TOPOLOGY, port-id: 0x%x",
                pDevPort->intf);

          return result;
       }
    }

    afe_port_mutex_lock(pDevPort);

    afe_port_add_to_client_list( &(pDevPort->afe_tx_client_list_ptr), *list_node);

    // Record the client info to LoopbackInfo structure
    loopback_info_ptr->src_port_client_ptr = (void *)psNewClient;

    psNewClient->client_id = (uint32_t)psNewClient;
    // update the clients number.
    pDevPort->num_client += 1;

    afe_port_mutex_unlock(pDevPort);

    MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "LB Client 0x%lX, connect to port: 0x%X, InterruptSamples: %lu, "
          "channels=%lu, dataType=%u, dataPath= 0x%x, interleaved=%u bit_width: %lu",
          psNewClient->client_id, pDevPort->intf,psNewClient->int_samples_per_period,
          psNewClient->afe_client.channel,  psNewClient->afe_client.data_type,
          psNewClient->afe_client.data_path, psNewClient->afe_client.interleaved,
          psNewClient->afe_client.bit_width);

    return  result;
}

/**
 * This function is to disconnect loopback client info from Tx-Port
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_loopback_disconnect_from_src_port(afe_dev_port_t* pDevPort)
{
    ADSPResult                     result = ADSP_EOK;
    afe_port_loopback_info         *loopback_info_ptr;
    bufQList                       *list_node = NULL;
    uint32_t                       num_buffers = 0;

    loopback_info_ptr = pDevPort->loopback_info_ptr;

    afe_port_mutex_lock(pDevPort);

    list_node = afe_port_search_unlink_client(&(pDevPort->afe_tx_client_list_ptr), loopback_info_ptr->src_port_handle_ptr);
    if (NULL != list_node)
    {
       pDevPort->num_client -= 1;
       num_buffers = ((afe_client_info_t*) list_node->element)->afe_client.num_buffers;
    }

    afe_port_mutex_unlock(pDevPort);

    afe_port_tx_client_disconnect_handler(pDevPort,list_node, NULL, TRUE, num_buffers);

    return  result;
}

ADSPResult afe_port_enable_loopback(afe_dev_port_t* pDevPort, void *loopback_cfg_param)
{
   ADSPResult                                result = ADSP_EOK;
   afe_port_loopback_info                    *loopback_info_ptr;
   afe_client_t                              *pAfeClient;
   afe_dev_port_t                            *pRxPort;
   afe_client_list                           *list_node = NULL;
   int16_t                                   is_queue_init = FALSE;
   uint32_t                                  num_buffers_created = 0;
   uint16_t                                  routing_mode = LB_MODE_DEFAULT;
   elite_msg_custom_afe_loopback_connect_t   lb_connect_payload;
   elite_msg_custom_afe_connect_t            *pConnectPayload;
   elite_msg_any_t                           msg;

   /* Allocate resources for loopback module */
   if (ADSP_EOK != (result = afe_port_alloc_resources_for_loopback_module(pDevPort)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdLoopbackEnable: ERROR in allocating resources, src port_id : 0x%x",
            pDevPort->intf);
      return result;
   }

   loopback_info_ptr = pDevPort->loopback_info_ptr;

   if (AFE_PORT_LOOPBACK_STATE_STOP != loopback_info_ptr->afe_loopback_state)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFECmdLoopbackEnable ERROR, ALREADY ENABLED, src portId : 0x%x", pDevPort->intf);
      return ADSP_EALREADY;
   }

   /* Already checked for the supported config version, no need to check again */   
   afe_loopback_cfg_v2_t *loopback_cfg_param_ptr = (afe_loopback_cfg_v2_t *)loopback_cfg_param;
   
   /* Populate port's loopback info struct with config info */
   loopback_info_ptr->rate_matching_enable = loopback_cfg_param_ptr->rate_matching_enable;    
   loopback_info_ptr->dst_port_id  = loopback_cfg_param_ptr->dst_port_id;
   loopback_info_ptr->routing_mode = loopback_cfg_param_ptr->routing_mode;   
   loopback_info_ptr->loopback_cfg_minor_version = loopback_cfg_param_ptr->loopback_cfg_minor_version;   
   loopback_info_ptr->gain = pDevPort->loopback_cached_gain;
   routing_mode = loopback_cfg_param_ptr->routing_mode;
      
   if(LB_MODE_SIDETONE == routing_mode)
   {
	  if (TX_DIR != pDevPort->dir)
	  {
		  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Sidetone cannot be enabled: Port 0x%x is not Tx port", pDevPort->intf);       
		  return ADSP_EBADPARAM;
	  }
   }

   if(LB_MODE_EC_REF_VOICE_AUDIO == routing_mode)
   {
	  if (RX_DIR != pDevPort->dir)
	  {
		  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EC Ref for voice and audio cannot be enabled: Port 0x%x is not Rx port", pDevPort->intf);       
		  return ADSP_EBADPARAM;
	  }
   }

   if(LB_MODE_EC_REF_VOICE == routing_mode)
   {
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "EC Ref for voice is not supported: Port 0x%x", pDevPort->intf);
	  return ADSP_EBADPARAM;
   }

   /* Connect message */
   pConnectPayload                 = &lb_connect_payload.connect_msg;
   
   /* Config the structure of the msg payload afeclient */
   pAfeClient                      = &(pConnectPayload->afe_client);
   memset(pAfeClient, 0, sizeof(afe_client_t));
   pAfeClient->interleaved         = FALSE;
   pAfeClient->buf_size            = AFE_MAX_SAMPLE_RATE; /* Dont care for Rx port client */
   pAfeClient->bytes_per_channel   = pDevPort->bytes_per_channel;
   pAfeClient->bit_width   			= pDevPort->bit_width;

   pAfeClient->sample_rate         = pDevPort->sample_rate;
   pAfeClient->cust_proc.subscribe_to_avt_drift   = FALSE;
   pAfeClient->afe_dp_delay_ptr = NULL;
   pAfeClient->afe_dev_hw_delay_ptr = NULL;
   
   /* Give the tx port handle to rx port for loopback */
   pAfeClient->afe_client_handle   = NULL;
   
   /* Config the structure of the msg payload */
   pConnectPayload->afe_id         = loopback_info_ptr->dst_port_id;
   pConnectPayload->responseq_ptr         = NULL;
   pConnectPayload->client_token          = 0;
   pConnectPayload->buffer_returnq_ptr    = NULL;

   if(LB_MODE_SIDETONE == routing_mode)
   {
     pAfeClient->data_path                             = AFE_RX_IN;
     pAfeClient->cust_proc.resampler_type              = IIR_BASED_SRC;
     pAfeClient->cust_proc.is_it_sidetone              = TRUE;
     pAfeClient->channel                               = 1; /* Mono only config for sidetone */
   }
   else
   {
     pAfeClient->data_path                             = AFE_RX_AUD_MIXER_IN;
     pAfeClient->cust_proc.resampler_type              = FIR_BASED_SRC;
      pAfeClient->channel                               = pDevPort->channels;          
   }
   
   if (loopback_info_ptr->rate_matching_enable)
   {
     pAfeClient->num_buffers                           = AFE_LOOPBACK_NUM_CLIENT_BUFS;
     lb_connect_payload.tx_port_acc_avt_drift_info     = &(pDevPort->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info);
   }
   else
   {
      pAfeClient->num_buffers                          = 2;
      lb_connect_payload.tx_port_acc_avt_drift_info    = NULL;
   }
   
   lb_connect_payload.is_rate_match                  = loopback_info_ptr->rate_matching_enable;

   msg.unOpCode                    = ELITE_CUSTOM_MSG;
   msg.pPayload                    = (void*) &lb_connect_payload;

   num_buffers_created = pAfeClient->num_buffers;

   /* Call connect function to dst rx port */
   result = afe_svc_get_port(loopback_info_ptr->dst_port_id, &pRxPort);

   if (ADSP_FAILED( result ) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE loopback: failed to get dst rx port : 0x%X", loopback_info_ptr->dst_port_id);
   }
   else
   {
      result = afe_port_client_loopback_connect_handler(pRxPort, &msg);
   
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AFECmdLoopbackEnable, connect to dst rx port : 0x%X failed", loopback_info_ptr->dst_port_id);
      }
      else
      {
         /* Get the dst rx-port client handle, and store it in loopback info. */
         loopback_info_ptr->dst_port_handle_ptr = pConnectPayload->svc_handle_ptr;   
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AfeLoopback, connect to dst Rx port : 0x%X success", loopback_info_ptr->dst_port_id);
   
         /* Connect to source port client list */
         if (ADSP_SUCCEEDED(result = afe_port_loopback_connect_to_src_port(pDevPort, &list_node, &is_queue_init, &num_buffers_created, routing_mode)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AfeLoopback, connect to src port : 0x%X success", pDevPort->intf);
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AfeLoopback, connect to src port : 0x%x failed", pDevPort->intf);
            afe_port_tx_client_disconnect_handler(pDevPort,list_node, NULL, is_queue_init, num_buffers_created);
         }
      }/* if connect to RX port failed */
   } /* if getting rx port failed */

   /* If AfeLoopback start failed, ack back to apr client */
   if (ADSP_SUCCEEDED (result))
   {
      loopback_info_ptr->afe_loopback_state = AFE_PORT_LOOPBACK_STATE_RUN;
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AfeLoopback start to run, src port_id : 0x%X, routing_mode : %d, rm_enable: %d", pDevPort->intf, routing_mode,loopback_info_ptr->rate_matching_enable);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to start AFE loopback, src port_id : 0x%X", pDevPort->intf);
      afe_port_dealloc_resources_for_loopback_module(pDevPort);
   }

   return result;
}

ADSPResult afe_port_disable_loopback(afe_dev_port_t* pDevPort, void *loopback_cfg_param)
{
   ADSPResult                         result   = ADSP_EOK;
   afe_port_loopback_info             *loopback_info_ptr;
   elite_msg_custom_afe_connect_t     *pPayload;
   qurt_elite_bufmgr_node_t           bufMgrNode;
   int                                nActualSize;
   elite_msg_any_t                    msg;
   afe_client_info_t                  *afeClientInfo;
   afe_dev_port_t                     *pRxPort;
   afe_loopback_cfg_v2_t               *loopback_cfg_param_ptr = NULL;

   loopback_info_ptr                  = pDevPort->loopback_info_ptr;

   /* Check the loopback info struct existed */
   if (NULL != loopback_info_ptr)
   {
      /* Check the loopback state, if loopback was already started, send back error code. */
      if (AFE_PORT_LOOPBACK_STATE_RUN == loopback_info_ptr->afe_loopback_state)
      {
         /* Get the opposite rx port from loopback command, and then record */

         loopback_cfg_param_ptr = (afe_loopback_cfg_v2_t *)loopback_cfg_param;
            loopback_info_ptr->dst_port_id = loopback_cfg_param_ptr->dst_port_id;

         /* Src port stop sending data to dst rx port */
         afeClientInfo = (afe_client_info_t *)(loopback_info_ptr->src_port_client_ptr);

         afe_port_mutex_lock(pDevPort);
         afeClientInfo->state = AFE_CLIENT_DISABLE;
         afe_port_mutex_unlock(pDevPort);

         if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AfeLoopback msg payload buffer NOT obtained.");
         }
         else
         {
            /* Config the disconnect message payload */
            pPayload                      =  (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;
            pPayload->afe_id              =  loopback_info_ptr->dst_port_id;
            pPayload->responseq_ptr       =  NULL;
            pPayload->client_token        =  0;
            pPayload->buffer_returnq_ptr  =  bufMgrNode.pReturnQ;
            pPayload->sec_op_code         =  ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
            pPayload->svc_handle_ptr      =  loopback_info_ptr->dst_port_handle_ptr;
            msg.unOpCode                  =  ELITE_CUSTOM_MSG;
            msg.pPayload                  =  (void*) pPayload;

            //call connect function to rx port
            //get port
            if (ADSP_EOK != (result = afe_svc_get_port(loopback_info_ptr->dst_port_id, &pRxPort)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE loopback: failed to get rx port");
            }
            else
            {
               //call disconnect
               if (ADSP_EOK != (result = afe_port_client_disconnect_handler(pRxPort, &msg)))
               {
                  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE loopback: failed to disconnect from rx port");
               }
               else //disconnect from RX success
               {
                  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AFECmdLoopback, disconnect from rx port success");

                  if (ADSP_SUCCEEDED(result = afe_port_loopback_disconnect_from_src_port(pDevPort)))
                  {
                     MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: AfeLoopback, disconnect src port from tx client success");
                  }
                  else
                  {
                     MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AfeLoopback, disconnect src port from tx client failed");
                  }
               } //disconnect from RX success
            } //get port failed

            elite_msg_return_payload_buffer(&msg);
         }//get BufMgr node failed
      } //if loopback in RUN state

      // Release the loopback info buffer, and reset it.
      afe_port_dealloc_resources_for_loopback_module(pDevPort);

      //re initialising gain to UNITY, so that for further enable loopback cmds wont use previous gain
      pDevPort->loopback_cached_gain = AFE_UNITY_GAIN;
   } //loopback struct NULL
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: AfeLoopbackDisable ERROR, ALREADY DISABLED!");
      result = ADSP_EALREADY;
   }

   return result;
}

/**
 * This function is to reconfig the media type to downstream dataQ in rx-port
 *
 * @param[in] pDevPort,  pointer to port instance
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_reconfig_loopback(afe_dev_port_t *pDevPort)
{
    ADSPResult                     result = ADSP_EOK;
    afe_client_t                   *pAfeClient;
    afe_client_info_t              *psAfeClientInfo;
    int16_t                        is_queue_init;
    uint32_t                       num_buffers_created;
    afe_port_loopback_info        *loopback_info_ptr = pDevPort->loopback_info_ptr;

    /* Get the txPortClientInfo and txPortClient structure    */
    psAfeClientInfo                 = (afe_client_info_t *)(loopback_info_ptr->src_port_client_ptr);
    pAfeClient                      = &(psAfeClientInfo->afe_client);

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Reconfig LoopBack : port_id=%x, client_id=0x%lX, dst_rx_port_id=%x",pDevPort->intf,psAfeClientInfo->client_id, loopback_info_ptr->dst_port_id);

    /* If the Src port client buffer has already been created but src port
       configuration has been changed, then disconnect/reconnect dest rx
       port client followed by destroying the src port client output queue
    */
    if( (NULL != psAfeClientInfo->bufQ) &&
        ((pDevPort->sample_rate != pAfeClient->sample_rate) ||
          ( ( LB_MODE_SIDETONE != loopback_info_ptr->routing_mode) && (pDevPort->channels != pAfeClient->channel) ) ||
         (pDevPort->bytes_per_channel != pAfeClient->bytes_per_channel)) )

    {
       /* First disconnect and reconnect to Rx port. This is needed to
          reclaim any buffer Rx port client could be holding before
          Tx port destroys its output queue to create new queue
       */
       if( (ADSP_FAILED (result = afe_port_reconnect_rx_loopback_client(pDevPort))) )
       {
           MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Reconfig LoopBack : Failed to reconnect to RX port");
           return result;
       }

       /* destroy the Tx client output queue       */
       afe_port_destroy_loopback_client_buf(pDevPort, psAfeClientInfo, TRUE, psAfeClientInfo->afe_client.num_buffers);
    }

    /* Contruct the tx client information. */
    pAfeClient->afe_client_handle   = loopback_info_ptr->dst_port_handle_ptr;
    pAfeClient->sample_rate         = pDevPort->sample_rate;
    pAfeClient->bytes_per_channel   = pDevPort->bytes_per_channel;
	pAfeClient->bit_width   		= pDevPort->bit_width;
    pAfeClient->interleaved         = FALSE;
    pAfeClient->buf_size            = pDevPort->int_samples_per_period;

    /* Update the tx client info */
    psAfeClientInfo->int_samples_per_period = pDevPort->int_samples_per_period;
    psAfeClientInfo->buf_size = pAfeClient->buf_size;

    /* For sidetone client, no change required */
    if (LB_MODE_SIDETONE != loopback_info_ptr->routing_mode  )
    {
      pAfeClient->channel = pDevPort->channels;
    }

    /* If the Src port client buffer has not been created, OR
       Buffer has been created already, but Src port
       configuration has been changed, bufQ would be set
       to NULL
    */
    if(NULL == psAfeClientInfo->bufQ)
    {
       MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Reconfig LoopBack : recreating buffers: port_id=%x, client_id=0x%lX, dst_port_id=%x",pDevPort->intf,psAfeClientInfo->client_id, loopback_info_ptr->dst_port_id);
       if (ADSP_FAILED(result = afe_port_create_buf(pDevPort, psAfeClientInfo, 1, 0, &is_queue_init, &num_buffers_created)))
       {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to afe_port_create_buf = %d!!\n", result);

          /* If Queue initialization was not successful, destroy the bufQ*/
          if (is_queue_init)
          {
             elite_svc_deinit_buf_queue(psAfeClientInfo->bufQ, num_buffers_created);
          }

          /* Free up the queue memory */
          qurt_elite_memory_free(psAfeClientInfo->bufQ);
          psAfeClientInfo->bufQ = NULL;

          return result;
       }
    }

    return result;
}

/**
  This function get the params for the module
  AFE_MODULE_LOOPBACK. It supports the following Params:
  AFE_PARAM_ID_LOOPBACK_GAIN2

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[out] param_buffer_ptr Pointer where the calibration
        data to store.
  @param[in] param_buf_len  Param size
  @param[out] params_buffer_len_req_ptr  Actual Param size

  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_get_loopback_module_params (afe_dev_port_t *pDevPort,uint32_t param_id,
      int8_t* param_buffer_ptr, int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr)
{

   uint32_t loopback_cfg_version;
   afe_port_loopback_info  *loopback_info_ptr = pDevPort->loopback_info_ptr;

   if (NULL == loopback_info_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Loopback is not enabled for port id : 0x%x", pDevPort->intf);
      return ADSP_ENOTREADY;
   }

   switch(param_id)
   {
      case AFE_PARAM_ID_LOOPBACK_CONFIG:
         {
            /* Choose the appropriate config version  */
            loopback_cfg_version = loopback_info_ptr->loopback_cfg_minor_version;
            afe_loopback_cfg_v2_t  loopback_cfg;
            
            /* Populate the payload parameters */
            loopback_cfg.loopback_cfg_minor_version = loopback_info_ptr->loopback_cfg_minor_version;
            loopback_cfg.dst_port_id = loopback_info_ptr->dst_port_id;
            loopback_cfg.routing_mode = loopback_info_ptr->routing_mode;

            if(AFE_PORT_LOOPBACK_STATE_STOP != loopback_info_ptr->afe_loopback_state)
            {
               loopback_cfg.enable = TRUE;
            }
            else
            {
               loopback_cfg.enable = FALSE;
            }
            loopback_cfg.reserved = 0;

            switch(loopback_cfg_version)
            {
               case LOOPBACK_CFG_V1:
                  {
                     *params_buffer_len_req_ptr = sizeof(afe_loopback_cfg_v1_t);

                     if(*params_buffer_len_req_ptr > param_buf_len )
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size:afe_get_loopback_cfg_v1_t required size = %d, given size = %lu",
                              *params_buffer_len_req_ptr, param_buf_len);
                        return ADSP_ENOMEMORY;
                     }

                     memscpy(param_buffer_ptr, param_buf_len, (void *)&loopback_cfg, sizeof(afe_loopback_cfg_v1_t));
                     }
                  break;
               case LOOPBACK_CFG_V2:
                  {
                     *params_buffer_len_req_ptr = sizeof(afe_loopback_cfg_v2_t);                     

                     if(*params_buffer_len_req_ptr > param_buf_len )
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size:afe_get_loopback_cfg_v1_t required size = %d, given size = %lu",
                              *params_buffer_len_req_ptr, param_buf_len);
                        return ADSP_ENOMEMORY;
                     }

                     loopback_cfg.rate_matching_enable = loopback_info_ptr->rate_matching_enable;
                     memscpy(param_buffer_ptr, param_buf_len, (void *)&loopback_cfg, sizeof(afe_loopback_cfg_v2_t));
                  }
                  break;
               default:
                  {
                     MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Loopback Get Cmd Cfg fail: InCompatible cfg version: %lu, port_id: 0x%x",
                           loopback_cfg_version, pDevPort->intf);
                     return ADSP_EBADPARAM;
                  }
            } /* loopback config version*/
         } /* loopback cfg param ID */
         break;
      case AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH:
      {
         afe_loopback_gain_per_path_param_t *loopbackgain_param = (afe_loopback_gain_per_path_param_t *)param_buffer_ptr;

         *params_buffer_len_req_ptr = sizeof(afe_loopback_gain_per_path_param_t);

         if(*params_buffer_len_req_ptr > param_buf_len )
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size:afe_get_loopback_gain_per_path required size = %d, given size = %lu",*params_buffer_len_req_ptr, param_buf_len);
            return ADSP_ENOMEMORY;
         }

         /* Populate payload parameters */
         loopbackgain_param->gain = loopback_info_ptr->gain;
         loopbackgain_param->rx_port_id = loopback_info_ptr->dst_port_id;

      }
     break;
      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported Param ID =0x%lx",param_id);
         return ADSP_EBADPARAM;
      }
   } /* switch param ID */

   return ADSP_EOK;
}

/**
  This function set the params for the module
  AFE_MODULE_LOOPBACK. It supports the following Params:
  AFE_PARAM_ID_LOOPBACK_GAIN2

  @param[in] pDevPort Pointer to the instance of the port.
  @param[in] param_id  Parameter id  .
  @param[in] param_buffer_ptr Pointer from where the calibration
        data to fetch.
  @param[in] params_buffer_len_req_ptr  Actual Param size


  @return
  ADSP_EOK error code.

  @dependencies
  None.
 */
ADSPResult afe_port_set_loopback_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
                                                      int8_t* params_buffer_ptr,uint16_t param_size)
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    loopback_cfg_version;
   uint16_t    routing_mode;

   switch(param_id)
   {
      case AFE_PARAM_ID_LOOPBACK_CONFIG:
         {
            /* Choose the appropriate config version  */
            loopback_cfg_version =  *((uint32_t *)params_buffer_ptr);

            if(loopback_cfg_version > AFE_API_VERSION_LOOPBACK_CONFIG)
            {
               loopback_cfg_version = AFE_API_VERSION_LOOPBACK_CONFIG;
            }

            if(LOOPBACK_CFG_V1 == loopback_cfg_version)
            {
               if (param_size < sizeof(afe_loopback_cfg_v1_t))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "CMD LOOPBACK fail: Invalid payload size for loopback_cfg_version %lu: %d, port_id: 0x%x",loopback_cfg_version, param_size, pDevPort->intf);
                  return ADSP_EBADPARAM;
               }
            }
            else if (LOOPBACK_CFG_V2 == loopback_cfg_version)
            {
              if (param_size < sizeof(afe_loopback_cfg_v2_t))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "CMD LOOPBACK fail: Invalid payload size for loopback_cfg_version %lu: %d, port_id: 0x%x",loopback_cfg_version, param_size, pDevPort->intf);
                  return ADSP_EBADPARAM;
               } 
            }
	    else
	    {
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Cmd Loopback fail: InCompatible cfg version: %lu, port_id: 0x%x", loopback_cfg_version, pDevPort->intf);
                return ADSP_EUNSUPPORTED;
	    }
            afe_loopback_cfg_v2_t loopback_cfg_param;
            memset((void *)&loopback_cfg_param, 0, sizeof(afe_loopback_cfg_v2_t));

            memscpy((void *)&loopback_cfg_param,sizeof(loopback_cfg_param),params_buffer_ptr,param_size);            

            if ( IS_PSEUDO_PORT_AFE_ID(loopback_cfg_param.dst_port_id) || IS_PSEUDO_PORT_AFE_ID(pDevPort->intf) )
               {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling loopback. dst_port_id 0x%x and/or src_port_id 0x%x should not be pseudoport!", loopback_cfg_param.dst_port_id, pDevPort->intf);
                  return ADSP_EUNSUPPORTED;
               }

               /* If destination port is not an Rx port */
            if ( (loopback_cfg_param.dst_port_id & 0x1) != 0 )
               {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling loopback. dst_port_id %d should be RX ", loopback_cfg_param.dst_port_id);
                  return ADSP_EUNSUPPORTED;
               }

               /* Check for the valid routing mode values */
            routing_mode = loopback_cfg_param.routing_mode;

               if( (routing_mode < LB_MODE_DEFAULT) || (routing_mode > LB_MODE_EC_REF_VOICE) )
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error enabling loopback. Invalid routing mode : %u for src_port_id : 0x%x",
                        routing_mode, pDevPort->intf);
                  return ADSP_EUNSUPPORTED;
               }

            if(LOOPBACK_CFG_V1 == loopback_cfg_version)
            {
               /* for V1 loopback configuration, rate matching is always enabled as long as routing mode doesnt have ST */
               if (LB_MODE_SIDETONE != routing_mode)
               {
                  loopback_cfg_param.rate_matching_enable = ENABLE_LOOPBACK_RATE_MATCHING;
               }
            }
            
            if (TRUE == loopback_cfg_param.enable)
               {
                  /* AfeLoopback Enable */
               result = afe_port_enable_loopback(pDevPort, (void *)&loopback_cfg_param);
               }
               else
               {
                  /* AfeLoopback Disable */
               result = afe_port_disable_loopback(pDevPort, (void *)&loopback_cfg_param);
            }
         }
         break;

       case AFE_PARAM_ID_LOOPBACK_GAIN_PER_PATH:
       {
           afe_loopback_gain_per_path_param_t *loopback_params_ptr = (afe_loopback_gain_per_path_param_t*)params_buffer_ptr;
           if (sizeof(afe_loopback_gain_per_path_param_t) != param_size)
           {
               MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_loopback_gain_param_t():: Bad Param Size");
               result = ADSP_EBADPARAM;
               break;
           }

           /* Gain is cached if gain set cmd comes before loopback enable */
           pDevPort->loopback_cached_gain = loopback_params_ptr->gain;

           if(NULL != pDevPort->loopback_info_ptr)
           {
              pDevPort->loopback_info_ptr->gain = loopback_params_ptr->gain;
           }

           MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Set Loopback gain: intf=0x%x,dir=%d,gain=0x%x",\
                 pDevPort->intf,pDevPort->dir,loopback_params_ptr->gain);
       }
       break;

       default:
          MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MODULE_LOOPBACK::Unsupported Param id ::0x%lx", param_id);
          result = ADSP_EUNSUPPORTED;
          break;
   }

   return result;
}

