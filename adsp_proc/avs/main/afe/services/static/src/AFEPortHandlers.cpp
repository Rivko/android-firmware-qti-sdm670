/*========================================================================
   This file contains AFE Port client handlers implementation

  Copyright (c) 2009 -2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEPortHandlers.cpp#5 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEPortManagement.h"
#include "AFESampRateConv.h"
#include "AFEDutyCycleDriver.h"
#include "AFEPopSuppressor_api.h"
#include "AFERateMatch.h"
#include "AFEMmpm_i.h"
#include "AFEDtmf.h"
#include "AFELimiter.h"

/*==========================================================================
  Function declarations
========================================================================== */
static ADSPResult afe_port_add_to_tx_client_list(afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg,
                                                 bufQList **list_node, int16_t *is_queue_init,
                                                 uint32_t* num_buffers);
static void afe_port_common_client_disconnect_handler(afe_dev_port_t* pDevPort, bufQList* list_node,
                                                      ADSPResult result, elite_msg_any_t *pMsg);
static ADSPResult afe_port_client_resample_init_common(afe_dev_port_t* pDevPort,
                                                       afe_client_info_t* pClientInfo,
                                                       uint32_t inFreq, uint32_t outFreq);

/*==========================================================================
  Function definitions
========================================================================== */
/**
 * This function adds a new client to the linked list. And it calls
 * appropriate functions to start book-keeping of the client
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg from client
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_client_connect_handler(void* ptr, elite_msg_any_t* pMsg)
{
  ADSPResult       result;
  bufQList         *list_node     = NULL;
  int16_t          is_queue_init  = FALSE;
  uint32_t         num_buffers    = 0;
  afe_dev_port_t* pDevPort = (afe_dev_port_t *)ptr;
  elite_msg_custom_afe_connect_t   	*pPayload;

  elite_msg_custom_afe_connect_t *pAfeConnectMsg = (elite_msg_custom_afe_connect_t *) pMsg->pPayload;

  if ( AFE_PORT_TX_DIR==(pDevPort->dir)|| AFE_PP_OUT==(pAfeConnectMsg->afe_client.data_path))
  {
    if (ADSP_FAILED(result = afe_port_add_to_tx_client_list(pDevPort, pMsg, &list_node, &is_queue_init, &num_buffers)))
    {
      afe_port_tx_client_disconnect_handler(pDevPort,list_node, pMsg, is_queue_init, num_buffers);
    }
  }
  else
  {
    if (ADSP_FAILED(result = afe_port_add_to_rx_client_list(pDevPort, pMsg, &list_node, &is_queue_init)))
    {
      afe_port_rx_client_disconnect_handler(pDevPort,list_node, pMsg, is_queue_init);
    }
  }



  // Update the group delays for the connected clients
  afe_port_set_group_delay(pDevPort);

  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);
  pPayload->afe_drift_ptr = &pDevPort->drift_info[AVT_DRIFT_INFO_IDX];

  /* Caller could be local routine without response queue */ 
  if(NULL != pPayload->responseq_ptr)
  {
    /* Ack only if response queue is present */
    elite_svc_send_ack(pMsg, result);
  }

  return result;
}

ADSPResult afe_port_validate_client_connect_payload(elite_msg_any_t* pMsg, uint16_t dir)
{
  elite_msg_custom_afe_connect_t   *pPayload;

  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);

  if (NULL == pPayload)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, NULL payload pointer");
    return ADSP_EBADPARAM;
  }

  /* Check for valid sample rate */
  if ((AFE_PORT_SAMPLE_RATE_8K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_11_025K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_12K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_16K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_22_05K != pPayload->afe_client.sample_rate) && /* compressed HDMI only */
      (AFE_PORT_SAMPLE_RATE_24K != pPayload->afe_client.sample_rate) && /* mostly for compressed data. */
      (AFE_PORT_SAMPLE_RATE_32K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_44_1K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_88_2K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_48K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_96K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_176_4K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_192K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_352_8K != pPayload->afe_client.sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_384K != pPayload->afe_client.sample_rate)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid sample rate: %lu",
          pPayload->afe_client.sample_rate);
    return ADSP_EBADPARAM;
  }

  /* Check for valid number of channels */
  if ( (0 == pPayload->afe_client.channel) ||
      (pPayload->afe_client.channel > AFE_PORT_MAX_CHANNEL_CNT)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid num channels: %lu",
          pPayload->afe_client.channel);
    return ADSP_EBADPARAM;
  }

  /* Check for valid bytes per sample */
  if ( (pPayload->afe_client.bytes_per_channel != AFE_BYTES_PER_SAMPLE_TWO) &&
      (pPayload->afe_client.bytes_per_channel != AFE_BYTES_PER_SAMPLE_FOUR)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid bytes per sample: %lu",
          pPayload->afe_client.bytes_per_channel);
    return ADSP_EBADPARAM;
  }

  /* check for valid data path */
  if(dir == AFE_PORT_TX_DIR)
  {
    if((pPayload->afe_client.data_path != AFE_TX_OUT) &&
        (pPayload->afe_client.data_path != AFE_TX_PP_AUDIO_SENSE_OUT) &&
        (pPayload->afe_client.data_path != AFE_PP_OUT))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid data path: 0x%x whereas port's dir= %d",
            pPayload->afe_client.data_path, dir);
      return ADSP_EBADPARAM;
    }
  }
  else
  {
    if((pPayload->afe_client.data_path != AFE_RX_IN) &&
        (pPayload->afe_client.data_path != AFE_RX_VOC_MIXER_IN) &&
        (pPayload->afe_client.data_path != AFE_RX_AUD_MIXER_IN) &&
        (pPayload->afe_client.data_path != AFE_PP_OUT))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid data path: 0x%x whereas port's dir= %d",
            pPayload->afe_client.data_path, dir);
      return ADSP_EBADPARAM;
    }
  }

  /* check for valid data type */
  if((pPayload->afe_client.data_type != LINEAR_PCM) &&
      (pPayload->afe_client.data_type != IEC_61937_NON_LINEAR))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE client connect, invalid data type: %lu",
          pPayload->afe_client.data_type);
    return ADSP_EBADPARAM;
  }

  return ADSP_EOK;
}

/**
 * This function adds an rx client to the list
 * Errors must be returned. Ack will be handled by calling function
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, pointer to client structure
 * @param[out] list_node, ptr to ptr of list node
 * @param[out] is_queue_init, boolean that indicates whether a queue was created in this function
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_add_to_rx_client_list(afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg, bufQList **list_node,
                                          int16_t *is_queue_init)
{
  afe_client_info_t                *psNewClient;
  ADSPResult                       result = ADSP_EOK;
  char                             dataQName[16];
  elite_msg_custom_afe_connect_t   *pPayload;
  char                             *queue_buf_ptr;
  qurt_elite_channel_t             *qurt_elite_channel;
  uint32_t                         scratch_buf_size = 0;
  uint32_t                         client_buf_size = 0, client_samples_per_period = 0;
  int8_t                           *scratch_buf_new = NULL;
  int8_t                           *scratch_buf_prev = NULL;


  *is_queue_init  =  FALSE;

  /* Add new client to the list */
  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);

  if ( ADSP_EOK != (result = afe_port_validate_client_connect_payload(pMsg, pDevPort->dir)) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Rx client connect failed, port_id: 0x%X", pDevPort->intf);
    return result;
  }

  *list_node = (afe_client_list *) qurt_elite_memory_malloc((sizeof(afe_client_list)), QURT_ELITE_HEAP_DEFAULT);
  if (NULL == *list_node)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client list node.");
    return ADSP_ENOMEMORY;
  }

  memset(*list_node, 0, sizeof(afe_client_list));

  //Create a new element, update the client information
  psNewClient = (afe_client_info_t*) qurt_elite_memory_malloc(sizeof(afe_client_info_t), QURT_ELITE_HEAP_DEFAULT);
  if (NULL == psNewClient)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client.");
    return ADSP_ENOMEMORY;
  }
  //Add to linked list element
  (*list_node)->element = (void*)psNewClient;
  (*list_node)->next = NULL;

  memset(psNewClient, 0, sizeof(afe_client_info_t));

  psNewClient->afe_client = pPayload->afe_client;
  psNewClient->int_samples_per_period = 0;
  /* Giving client_id a unique number */
  psNewClient->client_id = (uint32_t)psNewClient;

  /* check if client requested for pop suppressor only if port is in run state else check during port start.
   * pop suppressor will not be enabled when port is configured for low latency mode.
   * TO DO: Need to make changes in pop suppressor code to support 0.5ms frame size
   * */
  if ((TRUE == psNewClient->afe_client.cust_proc.is_pop_suppressor_required )&&(AFE_PORT_STATE_RUN == pDevPort->port_state)\
      && (AFE_PORT_DEFAULT_LATENCY_MODE== pDevPort->latency_mode) )
  {
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Rx client 0x%lX connected to port dir =%d, id =0x%x, requested for pop suppressor",
          psNewClient->client_id, pDevPort->dir, pDevPort->intf);

    //following function would validate if pop suppressor can be enabled or not, followed by memory allocation and initialization
    if(ADSP_EOK == ( afe_pop_suppressor_init(psNewClient->afe_client.sample_rate,psNewClient->afe_client.channel,psNewClient->afe_client.bytes_per_channel,
                                             pDevPort->sample_rate,&psNewClient->pop_suppressor_module_ptr,psNewClient->afe_client.buf_size)) )
    {
      /**<Update pop-suppressor delay in client_gp_delay, assumed to be 1ms.*/
      afe_port_calc_client_gp_delay_us(psNewClient);
    }
  }
  // If the port data type does not match client data type, but port is put at start, we need to error out here.
  // Ignoring the client data mismatch for compressed port
  if(AFE_PORT_STATE_RUN == pDevPort->port_state &&
      ((psNewClient->afe_client.data_type != LINEAR_PCM) && (pDevPort->port_data_type == AFE_UNCOMPRESSED_DATA_PORT)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: afe port datatype %ld mismatches client date type %ld",
          pDevPort->port_data_type, psNewClient->afe_client.data_type);
    return ADSP_EFAILED;
  }
  if( (TRUE == psNewClient->afe_client.interleaved) && (LINEAR_PCM == psNewClient->afe_client.data_type))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: Rx client 0x%lX connected with interleaved data for port_id : 0x%X",
          psNewClient->client_id, pDevPort->intf);

    return ADSP_EFAILED;
  }

  MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "Rx Client 0x%lX, connect to port: 0x%X, InterruptSamples: %lu, "
        "channels=%lu, dataType=%u, dataPath= 0x%x, interleaved=%u bit_width: %lu",
        psNewClient->client_id, pDevPort->intf, psNewClient->int_samples_per_period,
        psNewClient->afe_client.channel,  psNewClient->afe_client.data_type,
        psNewClient->afe_client.data_path, psNewClient->afe_client.interleaved,
        psNewClient->afe_client.bit_width);

  //Create data queue them for receiving data from rx client.
  qurt_elite_strl_cpy(dataQName,"afeRxDatI0D0N0", sizeof(dataQName));
  dataQName[9] = pDevPort->intf + '0';
  dataQName[11] = pDevPort->dir + '0';
  dataQName[13] = pDevPort->num_client + '0';

  queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_DEV_DATQ_MAX),QURT_ELITE_HEAP_DEFAULT);
  if (NULL == queue_buf_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port dataQ Creation Failed");
    return ADSP_ENOMEMORY;
  }

  psNewClient->this_client_manager.dataQ = (qurt_elite_queue_t*) queue_buf_ptr;

  qurt_elite_channel_init(&psNewClient->qurt_elite_channel);
  qurt_elite_channel = &psNewClient->qurt_elite_channel;


  if (ADSP_FAILED(result = qurt_elite_queue_init(dataQName, AFE_DEV_DATQ_MAX, psNewClient->this_client_manager.dataQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(qurt_elite_channel, psNewClient->this_client_manager.dataQ, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to init dataQ or add dataQ for client to channel = %d!!\n", result);
    *is_queue_init = TRUE;
    return result;
  }
  *is_queue_init = TRUE;

  // create new handle for the client, update the cmdQ,
  // and respond back to the client with new handle.
  psNewClient->this_client_manager.unSvcId  = ELITE_MIXER_SVCID;
  psNewClient->this_client_manager.threadId = 0;
  psNewClient->this_client_manager.cmdQ = afe_svc_global_ptr->static_svc_cmdq_ptr;
  pPayload->svc_handle_ptr = &(psNewClient->this_client_manager);

  //Update the index and current buffer
  psNewClient->remaining_samples = 0;
  psNewClient->curr_buff_ptr = NULL;

  // Initialize the resampler for this client.
  memset(&psNewClient->samp_rate_conv, 0, sizeof(samp_rate_conv_t));
  psNewClient->samp_rate_conv.resamp_type = INVALID_RESAMP_TYPE;
  if ((AFE_PORT_STATE_RUN == pDevPort->port_state)&&(LINEAR_PCM == psNewClient->afe_client.data_type))
  {
    if (ADSP_FAILED(result = afe_port_rx_client_resample_init(pDevPort, psNewClient)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "resampler create/init failed: %d", result);
      return result;
    }
  }

  /*If port is running, then latest client, votes immediately*/
  if(AFE_PORT_STATE_RUN == pDevPort->port_state)
  {
    afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psNewClient, TRUE);
    afe_port_update_dynamic_kpps(pDevPort, &psNewClient->afe_client, TRUE);

    if(ADSP_FAILED(
        result = afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AFE MMPM request failed for AFE_MMPM_CLIENT_TOPOLOGY, port-id: 0x%x",
            pDevPort->intf);
      return result;
    }
  }

  scratch_buf_size = pDevPort->scratch_buf_size;

  afe_port_get_frame_size(pDevPort->latency_mode, psNewClient->afe_client.sample_rate, &client_samples_per_period);

  client_buf_size = psNewClient->afe_client.channel * client_samples_per_period * sizeof(uint32_t);

  /* Scratchbuffer need to be reallocated if the new client has higher buf size requirement */
  if(client_buf_size > scratch_buf_size)
  {
    scratch_buf_size = client_buf_size;

    /* If port is in run state, allocate the scratch buffer immediately. Two scratch buffers will be present for short time */
    if(AFE_PORT_STATE_RUN == pDevPort->port_state)
    {
      if(NULL == (scratch_buf_new = (int8_t *)qurt_elite_memory_aligned_malloc(scratch_buf_size, 8, AFE_SINK_COPY_BUF)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating scratch buffer for port:0x%x", pDevPort->intf);
        return ADSP_ENOMEMORY;
      }
    }
  }

  afe_port_mutex_lock(pDevPort);

  /* Assign the new scratch buffer to port if the client with max media fmt is getting added */
  if(NULL != scratch_buf_new)
  {
    /* Cache the previous scratch buffer and deallocate in outside afe port mutex lock */
    if(NULL != pDevPort->scratch_buf)
    { 
      scratch_buf_prev = pDevPort->scratch_buf;
    }
    pDevPort->scratch_buf = scratch_buf_new;
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "scratch buffer is got assigned for , port:0x%x  of size %lu", pDevPort->intf, scratch_buf_size);
  }

  pDevPort->scratch_buf_size = scratch_buf_size; 

  //when everything is ready, add it to the list
  afe_port_add_to_client_list( &(pDevPort->afe_rx_client_list_ptr),  *list_node);

  // update the clients number.
  pDevPort->num_client += 1;

  switch(psNewClient->afe_client.data_path)
  {
    case AFE_RX_IN :
      break;

    case AFE_RX_VOC_MIXER_IN:
    {
      pDevPort->client_counter.voice_mixer_clients++;
    }
    break;

    case AFE_RX_AUD_MIXER_IN:
    {
      pDevPort->client_counter.audio_mixer_clients++;
    }
    break;

    case AFE_PP_OUT:
    case AFE_TX_OUT:
    case AFE_TX_PP_AUDIO_SENSE_OUT:
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Rx client connect failed, invalid data_path:0x%x, port:0x%x ",\
            psNewClient->afe_client.data_path, pDevPort->intf);
      result = ADSP_EBADPARAM;
      goto add_to_rx_client_bail_out;
    }
    break;
  }

  /* Set the client state to enable */
  psNewClient->state = AFE_CLIENT_ENABLE;

  add_to_rx_client_bail_out:
  afe_port_mutex_unlock(pDevPort);
  if(NULL != scratch_buf_prev)
  { 
    qurt_elite_memory_aligned_free(scratch_buf_prev);
  }

  return  result;
}

/*This function can be called by either connecting client/ disconnected client.
 *For connecting client, this function adds kpps/bw related to particular client to aggregated client kpps/bw.
 *For disconnected client, removes kpps/bw contribution due to that particular client from aggregated client kpps/bw.
 *
 *pDevPort          : Used pDevPort to access mmpm_info_ptr, from which aggregated client kpps/bw is updated/stored
 *psNewClient       : Used psNewClient to access afe_client, to store individual client kpps/bw
 *is_client_connect : This flag indicates, whether it is connecting client or disconnected client *
 **/
void afe_port_update_aggregated_client_kpps_and_bw(void *dev_ptr, void *client_info_ptr, bool_t is_client_connect)
{
  uint32_t           client_fwk_kpps = 0, client_fwk_bw = 0;
  afe_dev_port_t     *pDevPort       = (afe_dev_port_t *)dev_ptr;
  afe_client_info_t  *psNewClient    = (afe_client_info_t *)client_info_ptr;

  if( NULL == pDevPort || NULL == psNewClient )
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port or/and client pointer is NULL, port pointer:0x%x, client info pointer: 0x%x",\
          pDevPort, psNewClient);
    return;
  }

  afe_mmpm_info_t    *mmpm_info_ptr = (afe_mmpm_info_t *)pDevPort->mmpm_info_ptr;

  if( NULL == mmpm_info_ptr )
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "mmpm_info_ptr  is NULL");
    return;
  }

  if(TRUE == is_client_connect)
  {
    /*Updating afe client is connecting client, so, add it's contribution into aggregated client kpps/bw*/

    /*Client processing is divided as client framework and client re-sampler. If client needs re-sampling, then client re-sampler kpps/bw also added*/
    client_fwk_kpps                              = AFE_CLIENT_FWK_KPPS(pDevPort->sample_rate);
    psNewClient->client_kpps                    += client_fwk_kpps;
    mmpm_info_ptr->dyn_vot_info.agr_client_kpps += client_fwk_kpps;

    /*From profiling per port BW is 1MBPS (when port at 48k, 16bit, stereo). This implies 0.5MBPS (for port at 48k, 16bit, mono).
     * Considered BW value @ 48k, 16 bit mono channel as base for deriving BW scaling factors(BW values referred, in below explanation, is for
     * port @48k, 16bit mono channel).
     * For safe side, finalized 0.7MBPS. This use case does not account  resampler BW.
     * Band width is divided into DMA read/wrtie BW, Client framework BW  and client resampler BW (if resampler is needed)
     *0.7 MBPS is divided into 0.5MBPS for client framework BW and 0.2MBPS for DMA read/write BW. In addition to these, assumed resampler BW as 0.2 MBPS
     *
     *In summary, total BW for port @48k, 16bit mono channel is 0.9 MBPS = 0.2MBPS(DMA read/write)+ ( 0.5MBPS(client fwk)+0.2MBPS(resampler) )
     *Under CLIENT_TOPOLOGY, client_fwk and client_resampler bw will be voted. Under DEFAULT_TOPOLOGY, DMA read/wrtie BW will be voted.
     *
     *client_fwk_bw  = scaling_factor*common_bw_factor
     *       500000  = scaling_factor*(48000*2*1)           => scaling factor as 125/24
     * */
    client_fwk_bw                               = (125*afe_port_get_common_bw_factor(pDevPort))/24;
    psNewClient->client_bw                     += client_fwk_bw;
    mmpm_info_ptr->dyn_vot_info.agr_client_bw  += client_fwk_bw;

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE MPPS:: client fwk kpps: %lu, client_fwk_bw: %lu",\
          client_fwk_kpps, client_fwk_bw);

    /*Re-sampler kpps/bw is not needed for Compressed client*/
    if( (pDevPort->sample_rate != psNewClient->afe_client.sample_rate) && ( (LINEAR_PCM == psNewClient->afe_client.data_type) ) )
    {
      afe_port_aggregate_client_resampler_kpps_bw(pDevPort, psNewClient);
    }
  }
  else
  {
    /*Updating client is disconnected client, so remove it's contribution from aggregated client kpps/bw*/
    mmpm_info_ptr->dyn_vot_info.agr_client_kpps    -= psNewClient->client_kpps;
    psNewClient->client_kpps                        = 0;

    mmpm_info_ptr->dyn_vot_info.agr_client_bw      -= psNewClient->client_bw;
    psNewClient->client_bw                          = 0;
  }
}

/*This function calculates common bandwidth factor
 * pDevPort       : Uses port device pointer to calculate common bandwidth factor
 *returns  common bandwidth factor
 *
 * Introduced this function, to provide common bandwidth factor for both CLIENT_TOPOLOGY and DEFAULT_TOPOLOGY
 * */
uint32_t afe_port_get_common_bw_factor(void *dev_ptr)
{
  uint32_t common_bw_factor = 0;
  afe_dev_port_t *pDevPort = (afe_dev_port_t *)dev_ptr;

  // calculate the common bw factor (in Bytes per second) per one AFE frame
  if(pDevPort->intf == AFE_PORT_ID_MULTICHAN_HDMI_RX && pDevPort->channels > 2)
  {
    common_bw_factor = (8 * pDevPort->bytes_per_channel * pDevPort->sample_rate);
  }
  else if((IS_HDMI_OUTPUT_PORT_AFE_ID(pDevPort->intf) || IS_HDMI_OVER_DP_PORT_AFE_ID(pDevPort->intf)) && (pDevPort->channels > 2))
  {
    common_bw_factor = (8 * pDevPort->bytes_per_channel * pDevPort->sample_rate);
  }
  else if(IS_RT_PROXY_PORT_AFE_ID(pDevPort->intf))
  {
    // RT proxy port needs extra BW (in Bytes per second)to transfer from client mem to QDSP6 circ buf
    // and from circ buf to AFE. circ buf could be EBI or LPM. Avg BW is 2 times regular.
    common_bw_factor = ((2) * pDevPort->channels * pDevPort->bytes_per_channel
        * pDevPort->sample_rate);
  }
  else
  {
    //(in Bytes per second)
    common_bw_factor = (pDevPort->channels * pDevPort->bytes_per_channel
        * pDevPort->sample_rate);
  }

  return common_bw_factor;
}

/*afe_port_update_dynamic_kpps() is called (before adding new client to linked list) or (after removing client from linked list) or during DTMF initialization.
 *Considered Limiter and audio voice mixer kpps as afe port's dynamic kpps, since these operation's kpps might vary due to client connect/disconnect
 *or DTMF presence/absence.
 *
 * pDevPort          : pDevPort is used to get the information related to previous client nodes and current DTMF state
 * afe_client        : afe_client is pointer to updating client (that is going to be added to linked list or removed from linked list)
 *                     When DTMF is calling this function, it will send afe_client pointer as NULL
 * is_client_connect : This flag is used to check whether the updating client is the  connecting client (client to be added to list) or
 *                     disconnected client (removed from list)
 *                     When DTMF is calling this function, it will send is_client_connect as FALSE
 */
void afe_port_update_dynamic_kpps(void *dev_ptr, void *client_ptr, bool_t is_client_connect)
{
  uint32_t    current_limiter_kpps = 0, current_mixer_kpps = 0;
  uint16_t    total_audio_mixer_clients = 0, total_voice_mixer_clients = 0;

  afe_dev_port_t     *pDevPort       = (afe_dev_port_t *)dev_ptr;
  afe_client_t       *afe_client     = (afe_client_t *)client_ptr;
  bool_t             is_dtmf_enabled = FALSE;
  bool_t             limiter_bypass_mode = FALSE;
  if(NULL == pDevPort)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "port pointer (0x%x) is NULL", pDevPort);
    return;
  }

  afe_mmpm_info_t    *mmpm_info_ptr  = (afe_mmpm_info_t *)pDevPort->mmpm_info_ptr;

  if( NULL == mmpm_info_ptr)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "mmpm_info_ptr pointer is NULL, mmpm_info_ptr: 0x%x", mmpm_info_ptr);
    return;
  }

  afe_client_dynamic_voting_info_t *afe_client_dyn_vot_info_ptr = &(mmpm_info_ptr->dyn_vot_info);

  if(NULL == afe_client_dyn_vot_info_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_client_dyn_vot_info_ptr is NULL");
    return;
  }

  if(IS_IT_TX_PORT_AFE_ID(pDevPort->intf))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Limiter and audio voice mixer not applicable for port 0x%x", pDevPort->intf);
    return;
  }

  if(NULL != afe_client)
  {
    if( (afe_client->data_path == AFE_PP_OUT) || (afe_client->data_path == AFE_RX_IN)
    )
    {
      MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Limiter and audio voice mixer not applicable for port 0x%x with data path : 0x%x",
            pDevPort->intf, afe_client->data_path);
      return;
    }
  }

  total_voice_mixer_clients         = pDevPort->client_counter.voice_mixer_clients;
  total_audio_mixer_clients         = pDevPort->client_counter.audio_mixer_clients;

  if(NULL != afe_client && TRUE == is_client_connect)
  {
    /*Depends upon type of the new client, corresponding counter is incremented*/
    if(AFE_RX_VOC_MIXER_IN == afe_client->data_path)
    {
      total_voice_mixer_clients++;
    }
    else if(AFE_RX_AUD_MIXER_IN == afe_client->data_path)
    {
      total_audio_mixer_clients++;
    }
  }

  if(pDevPort->dtmf_info_ptr)
  {
    is_dtmf_enabled     = afe_port_check_dtmf_state(pDevPort);
    if(TRUE == is_dtmf_enabled)
    {
      /*For simplifying the conditions, DTMF is considered as audio mixer client*/
      total_audio_mixer_clients++;
    }
  }

  /*Remove previous limiter kpps value from aggregated client kpps*/
  afe_client_dyn_vot_info_ptr->agr_client_kpps -= afe_client_dyn_vot_info_ptr->prev_limiter_kpps;

  if( (total_audio_mixer_clients + total_voice_mixer_clients ) <= 1 )
  {
    current_limiter_kpps      = AFE_LIMITER_BYPASS_KPPS(pDevPort->sample_rate, pDevPort->channels);
    limiter_bypass_mode       = TRUE;
  }
  else
  {
    current_limiter_kpps      = AFE_LIMITER_ACTIVE_KPPS(pDevPort->sample_rate, pDevPort->channels);
  }

  afe_port_limiter_set_mode(pDevPort, limiter_bypass_mode);

  /*Update current kpps values into aggregated client kpps and prev_limiter kpps*/
  afe_client_dyn_vot_info_ptr->agr_client_kpps  += current_limiter_kpps;
  afe_client_dyn_vot_info_ptr->prev_limiter_kpps = current_limiter_kpps;

  /*Remove previous audio voice mixer kpps value from aggregated client kpps*/
  afe_client_dyn_vot_info_ptr->agr_client_kpps  -= afe_client_dyn_vot_info_ptr->prev_mixer_kpps;

  /*Calculate current audio voice mixer kpps and update it into aggregated client kpps and prev_mixer kpps*/
  if( (total_audio_mixer_clients > 0) && (total_voice_mixer_clients > 0) )
  {
    current_mixer_kpps = AFE_AUD_VOC_MIX_KPPS(pDevPort->sample_rate, pDevPort->channels);
  }

  afe_client_dyn_vot_info_ptr->agr_client_kpps   += current_mixer_kpps;
  afe_client_dyn_vot_info_ptr->prev_mixer_kpps    = current_mixer_kpps;

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE MPPS:: updated limiter kpps %lu, mixer kpps %lu",\
        current_limiter_kpps, current_mixer_kpps);
}
/**
 * This function adds new client to Tx port / Rx port (for EC)
 * Errors must be returned. Ack will be handled by calling function
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pClient, pointer to client structure
 * @param[in] resQ, client cmd msgQ
 * @param[out] list_node, ptr to ptr of list node
 * @param[out] is_queue_init, boolean that indicates whether a queue was created in this function
 * @param[out] num_bufs_created, number of buffers created
 * @return the ADSP_EOK error code
 */
static ADSPResult afe_port_add_to_tx_client_list(afe_dev_port_t *pDevPort, elite_msg_any_t* pMsg, bufQList **list_node,
                                                 int16_t *is_queue_init, uint32_t* num_bufs_created)
{
  afe_client_info_t               *psNewClient = NULL;
  audio_sense_info_t               *pAudSenseInfo= NULL;
  elite_msg_custom_afe_connect_t  *pPayload;
  ADSPResult                      result = ADSP_EOK;
  uint32_t                        scratch_buf_size = 0;
  uint32_t                        client_buf_size = 0, client_samples_per_period = 0;
  int8_t                          *scratch_buf_new = NULL;
  int8_t                          *scratch_buf_prev = NULL;


  *is_queue_init         = FALSE;
  *num_bufs_created      = 0;

  // Add new client to linked list
  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);

  if ( ADSP_EOK != (result = afe_port_validate_client_connect_payload(pMsg, pDevPort->dir)) )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Tx client connect failed, port_id: 0x%X", pDevPort->intf);
    return result;
  }

  *list_node = (afe_client_list *) qurt_elite_memory_malloc((sizeof(afe_client_list)), QURT_ELITE_HEAP_DEFAULT);
  if (NULL == *list_node)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client list node.");
    return ADSP_ENOMEMORY;
  }

  memset(*list_node, 0, sizeof(afe_client_list));

  psNewClient = (afe_client_info_t*) qurt_elite_memory_malloc(sizeof(afe_client_info_t), QURT_ELITE_HEAP_DEFAULT);
  if (NULL == psNewClient)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Out of memory creating client.");
    return ADSP_ENOMEMORY;
  }
  //Add to linked list element
  (*list_node)->element = (void*)psNewClient;
  (*list_node)->next = NULL;

  memset(psNewClient, 0, sizeof(afe_client_info_t));

  psNewClient->afe_client = pPayload->afe_client;
  //validation check
  if(AFE_PORT_STATE_RUN == pDevPort->port_state && ADSP_FAILED(afe_port_client_data_type_validation(pDevPort,&(psNewClient->afe_client))))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: afe port datatype %ld mismatches client date type %ld",
          pDevPort->port_data_type, psNewClient->afe_client.data_type);

    return ADSP_EFAILED;
  }

  if( (TRUE == psNewClient->afe_client.interleaved) &&
      (LINEAR_PCM == psNewClient->afe_client.data_type)
  )
  {
    psNewClient->afe_client.interleaved = FALSE;

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Tx client 0x%lX requested for interleaved data for port id: 0x%X, changing to de-interleaved",
          psNewClient->client_id, pDevPort->intf);
  }

  psNewClient->int_samples_per_period = 0;

  psNewClient->buf_size = pPayload->afe_client.buf_size;

  psNewClient->client_id = (uint32_t)psNewClient;

  MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "Tx Client 0x%lX, connect to port intf = 0x%x, InterruptSamples=%lu, "
        "channels=%lu, dataPath=0x%x, dataType= %u,interleaved = %u, num_buffers: %lu, bit_width: %lu",
        psNewClient->client_id, pDevPort->intf, psNewClient->int_samples_per_period, psNewClient->afe_client.channel,
        psNewClient->afe_client.data_path, psNewClient->afe_client.data_type,psNewClient->afe_client.interleaved,
        psNewClient->afe_client.num_buffers, psNewClient->afe_client.bit_width);

  if (ADSP_FAILED(result = afe_port_create_buf(pDevPort, psNewClient, 1, 0, is_queue_init, num_bufs_created)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to afe_port_create_buf = %d!!", result);
    return result;
  }

  // create new handle for the client, update the cmdQ,
  // and respond back to the client with new handle.
  psNewClient->this_client_manager.unSvcId  = ELITE_MIXER_SVCID;
  psNewClient->this_client_manager.threadId = pDevPort->thread_id;
  psNewClient->this_client_manager.cmdQ = afe_svc_global_ptr->static_svc_cmdq_ptr;
  pPayload->svc_handle_ptr = &(psNewClient->this_client_manager);

  // Initialize the resampler for this client.
  memset(&psNewClient->samp_rate_conv, 0, sizeof(samp_rate_conv_t));
  psNewClient->samp_rate_conv.resamp_type = INVALID_RESAMP_TYPE;

  if (AFE_PORT_STATE_RUN == pDevPort->port_state)
  {
    if(LINEAR_PCM == psNewClient->afe_client.data_type)
    {
      if (ADSP_FAILED(result = afe_port_tx_client_resample_init(pDevPort, psNewClient)))
      {
        return result;
      }
    }

    /* Send the media type to the client only if port is in RUN state */
    afe_port_send_media_t(pDevPort, psNewClient);
  }

  /*If port is running, then latest client, votes immediately*/
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

  scratch_buf_size = pDevPort->scratch_buf_size;

  afe_port_get_frame_size(pDevPort->latency_mode, psNewClient->afe_client.sample_rate, &client_samples_per_period);

  client_buf_size = psNewClient->afe_client.channel * client_samples_per_period * sizeof(uint32_t);

  /* Scratchbuffer need to be reallocated if the new client has higher buf size requirement */
  if(client_buf_size > scratch_buf_size)
  { 
    scratch_buf_size = client_buf_size;

    /* If port is in run state, allocate the scratch buffer immediately */
    if(AFE_PORT_STATE_RUN == pDevPort->port_state)
    {
      if(NULL == (scratch_buf_new = (int8_t *)qurt_elite_memory_aligned_malloc(scratch_buf_size, 8, AFE_SINK_COPY_BUF)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Malloc failure while allocating scratch buffer for port:0x%x", pDevPort->intf);
        return ADSP_ENOMEMORY;
      }
    }
  }

  //when everything is ready add the client to the list.
  afe_port_mutex_lock(pDevPort);

  /* Assign the new scratch buffer to port if the client with max media fmt is getting added */
  if(NULL != scratch_buf_new)
  {
    /* Cache the previous scratch buffer and deallocate in outside afe port mutex lock */
    if(NULL != pDevPort->scratch_buf)
    { 
      scratch_buf_prev = pDevPort->scratch_buf;
    }
    pDevPort->scratch_buf = scratch_buf_new;
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "scratch buffer is assigned for , port:0x%x of size %lu", pDevPort->intf, scratch_buf_size);
  }

  pDevPort->scratch_buf_size = scratch_buf_size; 

  afe_port_add_to_client_list( &(pDevPort->afe_tx_client_list_ptr),  *list_node);

  //  update the clients number.
  pDevPort->num_client +=1;

  psNewClient->state = AFE_CLIENT_ENABLE;
  pAudSenseInfo      =  &(psNewClient->afe_client.cust_proc.audio_sense);
  //For MAD clients, they should be disabled by default, in contrast to other clients.
  if((AFE_MAD_AUDIO == pAudSenseInfo->type)  || (AFE_MAD_BEACON == pAudSenseInfo->type) ||
      (AFE_MAD_ULTRASOUND == pAudSenseInfo->type))
  {
    psNewClient->state = AFE_CLIENT_DISABLE;  
  }
  afe_port_mutex_unlock(pDevPort);

  if(NULL != scratch_buf_prev)
  {
    qurt_elite_memory_aligned_free(scratch_buf_prev);
  }

  return  ADSP_EOK;
}


/**
 * This function is to get new buffer for tx-port client
 *
 * @param[in] psClientInfo, pointer to client info structure
 * @param[in] bufQ, bufferQ for client
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_get_new_buffer( afe_client_info_t *psClientInfo, qurt_elite_queue_t* bufQ)
{
  uint32_t unChannelStatus;
  qurt_elite_bufmgr_node_t outBufMgrNode;
  elite_msg_data_buffer_t *psPcmBuf;
  ADSPResult result = ADSP_EOK;

  unChannelStatus = qurt_elite_channel_poll(qurt_elite_queue_get_channel(bufQ),
                                            qurt_elite_queue_get_channel_bit(bufQ));
  if(unChannelStatus)
  {
    result = qurt_elite_queue_pop_front(bufQ, (uint64_t*)&outBufMgrNode);

    if(ADSP_EOK == result)
    {
      psPcmBuf = (elite_msg_data_buffer_t *)outBufMgrNode.pBuffer;
      //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: TX obtained buf: %X", (uint32_t) psPcmBuf);
      //For compressed data path, here deal with num of bytes instead of samples.
      if(psClientInfo->afe_client.data_type != LINEAR_PCM)
      {
        psClientInfo->remaining_samples = psClientInfo->actual_buf_size;
      }
      else
      {
        psClientInfo->remaining_samples = (psClientInfo->actual_buf_size)
                      / (psClientInfo->afe_client.bytes_per_channel * psClientInfo->afe_client.channel);
      }

      psClientInfo->ps_curr_pcm_buff = psPcmBuf;
      psClientInfo->ps_curr_pcm_buff->nActualSize = psClientInfo->actual_buf_size;
      psClientInfo->ps_curr_pcm_buff->nOffset = 0;
      psClientInfo->curr_buff_ptr = (int8_t *)&(psPcmBuf->nDataBuf);
    }
  }
  else
  {
    result = ADSP_EFAILED;
  }

  return result;
}

/**
 * This function pushes new client in a client list, and it is a general
 * client adding process for port
 *
 * @param[in] pClientList, client link list for port.
 * @param[in] size sizeof the element to be added to the list (bytes)
 * @return the client info, including client
 */
void afe_port_add_to_client_list(afe_client_list **pClientList, bufQList *pQNode )
{
  bufQList               *list;

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

/**
 * This function is to register disconnect marker from client
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_register_client_disconnect_marker_handler (void* ptr, elite_msg_any_t *pMsg)
{
  elite_msg_custom_afeclient_disconnect_marker_t        *pPayload;
  elite_svc_handle_t                    *svc_handle_ptr;
  afe_client_info_t                     *psAfeClientInfo = NULL;
  ADSPResult                            result = ADSP_EOK;
  bufQList                              *list_node = NULL;
  afe_dev_port_t* pDevPort = (afe_dev_port_t *)ptr;

  pPayload         =  (elite_msg_custom_afeclient_disconnect_marker_t *)(pMsg->pPayload);
  svc_handle_ptr   =  pPayload->svc_handle_ptr;
  list_node = pDevPort->afe_rx_client_list_ptr;
  while (list_node != NULL)
  {
    psAfeClientInfo = (afe_client_info_t *)list_node->element;
    if (&(psAfeClientInfo->this_client_manager) == svc_handle_ptr) break;
    list_node = list_node->next;
  }
  if (NULL != list_node)
  {
    afe_port_mutex_lock(pDevPort);
    psAfeClientInfo = (afe_client_info_t*) list_node->element;
    if (psAfeClientInfo->pop_suppressor_module_ptr)
    {
      psAfeClientInfo->client_disconnect_marker_flag=TRUE;       //register disconnect marker
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE registered disconnect marker from Rx client 0x%lX connected to port dir =%d, id =0x%x:",
            psAfeClientInfo->client_id,pDevPort->dir, pDevPort->intf);
    }
    else
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "pop suppressor is stubbed out or not requested by client 0x%lX at port id =0x%x: "
            ,psAfeClientInfo->client_id,pDevPort->intf);
    }
    afe_port_mutex_unlock(pDevPort);
  }
  else
  {
    result = ADSP_EFAILED;
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "client at port dir %d intf %d does not exist ",pDevPort->dir, pDevPort->intf);
  }

  elite_svc_send_ack(pMsg, result);
  return  result;
}
/**
 * This function is to client-disconnect processing
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_client_disconnect_handler (void* ptr, elite_msg_any_t *pMsg)
{
  elite_msg_custom_afe_connect_t        *pPayload;
  elite_svc_handle_t                    *svc_handle_ptr;
  afe_client_info_t                     *psAfeClientInfo = NULL;
  ADSPResult                            result = ADSP_EOK;
  bufQList                              *list_node = NULL;
  afe_dev_port_t* pDevPort = (afe_dev_port_t *)ptr;

  pPayload         =  (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);
  svc_handle_ptr   =  pPayload->svc_handle_ptr;

  if (NULL == list_node && NULL != pDevPort->afe_tx_client_list_ptr)
  {
    afe_port_mutex_lock(pDevPort);
    list_node = afe_port_search_unlink_client(&(pDevPort->afe_tx_client_list_ptr), svc_handle_ptr);
    if (NULL != list_node)
    {
      psAfeClientInfo = (afe_client_info_t*) list_node->element;
      pDevPort->num_client--;
    }
    afe_port_mutex_unlock(pDevPort);

    if (NULL != list_node)
    {
      afe_port_tx_client_disconnect_handler(pDevPort,list_node, pMsg, TRUE, psAfeClientInfo->afe_client.num_buffers);
    }
  }

  if (NULL == list_node && pDevPort->afe_rx_client_list_ptr != NULL)
  {
    afe_port_mutex_lock(pDevPort);
    list_node = afe_port_search_unlink_client(&(pDevPort->afe_rx_client_list_ptr), svc_handle_ptr);
    if (NULL != list_node)
    {
      psAfeClientInfo = (afe_client_info_t*) list_node->element;
      //update client counters
      pDevPort->num_client--;

      switch(psAfeClientInfo->afe_client.data_path)
      {
        case AFE_RX_VOC_MIXER_IN:
        {
          pDevPort->client_counter.voice_mixer_clients--;
        }
        break;

        case AFE_RX_AUD_MIXER_IN:
        {
          pDevPort->client_counter.audio_mixer_clients--;
        }
        break;

        default:
          break;
      }
    }
    afe_port_mutex_unlock(pDevPort);

    if (NULL != list_node)
    {
      afe_port_rx_client_disconnect_handler(pDevPort,list_node, pMsg, TRUE);
    }
  }

  if (NULL == list_node)
  {
    result = ADSP_EFAILED;
  }

  if (ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Client Disconnect failed: Unknown client");
  }

  elite_svc_send_ack(pMsg, result);

  return  result;
}

/**
 * to disconnect rx clients
 */
void afe_port_rx_client_disconnect_handler (afe_dev_port_t* pDevPort, bufQList *list_node, elite_msg_any_t *pMsg, int16_t is_deinit_queue)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_any_t   dataQMsg;

  if (NULL != list_node)
  {
    afe_client_info_t *psAfeClientInfo  = (afe_client_info_t*) list_node->element;
    if (NULL != psAfeClientInfo)
    {
      if (NULL != psAfeClientInfo->pop_suppressor_module_ptr)
      {
        afe_pop_suppressor_end(&psAfeClientInfo->pop_suppressor_module_ptr);
      }

      if (psAfeClientInfo->remaining_samples != 0)
      {
        if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(psAfeClientInfo->ps_curr_pcm_buff->pBufferReturnQ, \
                                                                   (elite_msg_any_payload_t*) psAfeClientInfo->ps_curr_pcm_buff)))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortHandler: Fail to qurt_elite_queue_push_back, Error = %d!!\n", result);
        }
        psAfeClientInfo->remaining_samples = 0;
      }

      /* If client data Q needs to be de-init'd */
      if (is_deinit_queue)
      {
        /* Check for any pending EOS msg in the data q */
        while (ADSP_EOK == (result = qurt_elite_queue_pop_front(psAfeClientInfo->this_client_manager.dataQ,
                                                                (uint64_t *)&dataQMsg)))
        {
          /* Handle the EOS from client */
          if (ELITE_DATA_EOS == dataQMsg.unOpCode)
          {
            afe_port_data_cmd_eos(psAfeClientInfo, pDevPort, &dataQMsg);
          }

          /* Return the buffer to its return Q and do not ack to client (basically do not do finish the msg here).
           * In EOS_v1, it might be ok to finish the msg, but EOS_v2 should not have ack as Matrix is not waiting for ack in this case
           */
          elite_msg_return_payload_buffer(&dataQMsg);

        } /* While the data Q is not empty */


        /* De-init the client data Q */
        qurt_elite_queue_deinit(psAfeClientInfo->this_client_manager.dataQ);
      }

      if (psAfeClientInfo->this_client_manager.dataQ != NULL)
      {
        qurt_elite_memory_free(psAfeClientInfo->this_client_manager.dataQ);
        psAfeClientInfo->this_client_manager.dataQ = NULL;
      }
    }
  }

  afe_port_common_client_disconnect_handler(pDevPort,list_node, result, pMsg);
}

/**
 * to disconnect tx clients
 */
void afe_port_tx_client_disconnect_handler (afe_dev_port_t* pDevPort, bufQList *list_node, elite_msg_any_t *pMsg, int16_t is_deinit_queue, uint32_t num_buffers)
{
  ADSPResult result                   = ADSP_EOK;
  if (NULL != list_node)
  {
    afe_client_info_t *psAfeClientInfo  = (afe_client_info_t*) list_node->element;

    if (NULL != psAfeClientInfo)
    {
      if (psAfeClientInfo->remaining_samples != 0)
      {
        if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(psAfeClientInfo->bufQ, \
                                                                   (elite_msg_any_payload_t*) psAfeClientInfo->ps_curr_pcm_buff)))
        {
          MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortHandler: thread encountered qurt_elite_queue_push_back error = %d!!\n", result);
        }
      }

      // Destroy the bufQ
      if (is_deinit_queue)
      {
        elite_svc_deinit_buf_queue(psAfeClientInfo->bufQ, num_buffers);
      }
      if (NULL != psAfeClientInfo->bufQ)
      {
        qurt_elite_memory_free(psAfeClientInfo->bufQ);
        psAfeClientInfo->bufQ = NULL;
      }
    }
  }

  afe_port_common_client_disconnect_handler(pDevPort,list_node,result, pMsg);

}


/**
 * this sends the ack also.
 */
static void afe_port_common_client_disconnect_handler (afe_dev_port_t* pDevPort, bufQList *list_node, ADSPResult result, elite_msg_any_t *pMsg)
{
  if (NULL != list_node)
  {
    afe_client_info_t *psAfeClientInfo = (afe_client_info_t*) list_node->element;
    afe_mmpm_info_t      *mmpm_info_ptr   = (afe_mmpm_info_t *)pDevPort->mmpm_info_ptr;

    if (NULL != psAfeClientInfo)
    {
      sample_rate_conv_deinit(&psAfeClientInfo->samp_rate_conv);

      if (NULL != psAfeClientInfo->rate_match_info_ptr)
      {
        afe_port_rate_match_info_destroy(psAfeClientInfo);
      }

      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Client 0x%lx disconnected from port: dir = %d, intf = %x",
            psAfeClientInfo->client_id, pDevPort->dir, pDevPort->intf);

      /**If port is not running, then port stop already devoted CLIENT_TOPOLOGY, therefore, the disconnecting client after port stop, don't need to vote/update
       *  anything*/
      if(AFE_PORT_STATE_RUN == pDevPort->port_state)
      {
        if(NULL != mmpm_info_ptr)
        {
          afe_port_update_dynamic_kpps(pDevPort, &psAfeClientInfo->afe_client, FALSE);
          afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psAfeClientInfo, FALSE);

          /*Checks whether disconnected client is last client or not*/
          if(0 != pDevPort->num_client)
          {
            /*Disconnected client is not the last client, so just kpps and bw related to this client which is decremented will be voted*/
            if(ADSP_FAILED(result = afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
            {
              MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "AFE MMPM request failed for AFE_MMPM_CLIENT_TOPOLOGY continuing with existing voting, port-id: 0x%x",
                    pDevPort->intf);
            }
          }
          else
          {
            /*Since the disconnected client is the last client, this voting changes CLIENT_TOPOLOGY to be inactive.*/
            afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
          }
        }
      }
      qurt_elite_memory_free(psAfeClientInfo);
    }
    qurt_elite_memory_free(list_node);
  }
}

/**
 * This function is to client-disable processing
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pMsg, msg to the port cmdQ
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_client_disable_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg)
{
  elite_msg_custom_afe_connect_t        *pPayload;
  elite_svc_handle_t                    *svc_handle_ptr;
  bufQList                              *pQNode;
  bufQList                              **ppQNode;
  afe_client_info_t                     *psAfeClientInfo = NULL;
  int                                   i;
  ADSPResult                            result = ADSP_EFAILED;

  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);
  svc_handle_ptr = pPayload->svc_handle_ptr;
  pQNode = pDevPort->afe_tx_client_list_ptr;
  ppQNode = &(pDevPort->afe_tx_client_list_ptr);

  //Find the connected client in the client linked list
  for (i = 0; pQNode != NULL;i++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;
    if ( svc_handle_ptr == &(psAfeClientInfo->this_client_manager))
    {
      //static svc thread which executes this statement, should not proceed until SendPcmSamples is done.
      //if it's allowed to proceed when send pcm to this client is going on, if the client rceives an ack,
      //it may go ahead and destroy buffers. in this case port thread might crash.
      afe_port_mutex_lock(pDevPort);
      psAfeClientInfo->state = AFE_CLIENT_DISABLE;
      afe_port_mutex_unlock(pDevPort);

      /* Generate and send the EoS to this client */
      if( (!IS_PSEUDO_PORT_AFE_ID(pDevPort->intf)) &&
          (AFE_PORT_STATE_RUN == pDevPort->port_state) &&
          (NULL != psAfeClientInfo->afe_client.afe_client_handle->dataQ))
      {
        afe_port_generate_tx_eos(psAfeClientInfo);
      }

      result = ADSP_EOK;
      break;
    }
    else
    {
      ppQNode = &(pQNode->next);
      pQNode = *ppQNode;
    }
  }

  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO,
          "ADSP_FAILED to find the pNode for psAfeClientInfo = %lx!!", (uint32_t) psAfeClientInfo);
  }

  // handle MAD clients
  if (NULL != pDevPort->hw_mad_ptr)
  {
    result |= afe_port_hw_mad_enable_disable_handler((void *)pDevPort);
  }

  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    result |= afe_port_sw_duty_cycle_enable_disable_handler(pDevPort);
  }

  elite_svc_send_ack(pMsg, result);

  return  result;
}


ADSPResult afe_port_client_enable_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg)
{
  elite_msg_custom_afe_connect_t        *pPayload;
  elite_svc_handle_t                    *svc_handle_ptr;
  bufQList                              *pQNode;
  bufQList                              **ppQNode;
  afe_client_info_t                     *psAfeClientInfo = NULL;
  int                                   i;
  ADSPResult                            result = ADSP_EFAILED;

  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);
  svc_handle_ptr = pPayload->svc_handle_ptr;
  pQNode = pDevPort->afe_tx_client_list_ptr;
  ppQNode = &(pDevPort->afe_tx_client_list_ptr);

  //Find the connected client in the client linked list
  for (i = 0; pQNode != NULL;i++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;
    if ( svc_handle_ptr == &(psAfeClientInfo->this_client_manager))
    {
      //static svc thread which executes this statement, should not proceed until SendPcmSamples is done.
      //if it's allowed to proceed when send pcm to this client is going on, if the client rceives an ack,
      //it may go ahead and destroy buffers. in this case port thread might crash.
      afe_port_mutex_lock(pDevPort);
      psAfeClientInfo->state = AFE_CLIENT_ENABLE;
      afe_port_mutex_unlock(pDevPort);

      /* send the Media Format to this client */
      if( (!IS_PSEUDO_PORT_AFE_ID(pDevPort->intf)) &&
          (AFE_PORT_STATE_RUN == pDevPort->port_state) &&
          (NULL != psAfeClientInfo->afe_client.afe_client_handle->dataQ))
      {
        afe_port_send_media_t(pDevPort, psAfeClientInfo);
      }

      result = ADSP_EOK;
      break;
    }
    else
    {
      ppQNode = &(pQNode->next);
      pQNode = *ppQNode;
    }
  }

  // handle MAD clients
  if (NULL != pDevPort->hw_mad_ptr)
  {
    result |= afe_port_hw_mad_enable_disable_handler((void *)pDevPort);
  }


  if (NULL != pDevPort->sw_duty_cycle_data_ptr)
  {
    result |= afe_port_sw_duty_cycle_enable_disable_handler(pDevPort);
  }


  if (ADSP_FAILED(result))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to find the pNode for psAfeClientInfo = %lx!!\n", (uint32_t) psAfeClientInfo);
  }

  elite_svc_send_ack(pMsg, result);
  return  result;
}

/**
 * This function is to search for a client and remove it from the list
 *
 * @param[in] ppQNode, pointer to the list,
 * @param[in] pClient, client to be removed
 * @param[in] pClientInfo, return node as a pointer
 *
 * @return bufQList, ptr to the node containing the current element, NULL if element not found.
 */
bufQList* afe_port_search_unlink_client(bufQList **ppQNode,elite_svc_handle_t  *svc_handle_ptr)
{

  afe_client_info_t   *psAfeClientInfo;
  bufQList*           pQNode;
  int                 i;

  pQNode = *ppQNode;
  for (i = 0; pQNode != NULL; i++)
  {
    psAfeClientInfo = (afe_client_info_t *)(*ppQNode)->element;
    if (&(psAfeClientInfo->this_client_manager) == svc_handle_ptr)
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
 * This function is the initialize all the client resampler
 * & others at port start
 * connected to this port.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @return none
 */
ADSPResult afe_port_client_reinit_at_port_start(afe_dev_port_t* pDevPort)
{
  int                    i;
  bufQList               *pQNode;
  afe_client_info_t      *psAfeClientInfo = NULL;
  ADSPResult             result = ADSP_EOK;
  bool_t                 vote_now_for_client_topo = FALSE;

  pQNode = pDevPort->afe_tx_client_list_ptr;
  for (i = 0; pQNode != NULL; i++)
  {
    psAfeClientInfo = (afe_client_info_t*)pQNode->element;
    if (pQNode->element != NULL)
    {
      psAfeClientInfo->int_samples_per_period = 0;
      if (ADSP_FAILED(result= afe_port_tx_client_resample_init(pDevPort, psAfeClientInfo)))
      {
        sample_rate_conv_deinit(&psAfeClientInfo->samp_rate_conv);

        psAfeClientInfo->state = AFE_CLIENT_DISABLE;

        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Port intf=%d,dir=%d. Resampler init failed for TX client id 0x%lX and hence this client is disabled", pDevPort->intf,pDevPort->dir,psAfeClientInfo->client_id);
      }
      else
      {
        psAfeClientInfo->prev_time = 0;

        /* Update the vote only if client re-sampler initialization succeeds*/
        vote_now_for_client_topo = TRUE;
        afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psAfeClientInfo, TRUE);
      }
    }
    pQNode = pQNode->next;
  }

  pQNode = pDevPort->afe_rx_client_list_ptr;
  for (i = 0; pQNode != NULL; i++)
  {
    psAfeClientInfo = (afe_client_info_t*)pQNode->element;
    if (pQNode->element != NULL)
    {
      psAfeClientInfo->int_samples_per_period = 0;
      if (ADSP_FAILED(result= afe_port_rx_client_resample_init(pDevPort, psAfeClientInfo)))
      {
        sample_rate_conv_deinit(&psAfeClientInfo->samp_rate_conv);
        psAfeClientInfo->state = AFE_CLIENT_DISABLE;
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Port intf=%d, dir=%d. Resampler init failed for RX client id 0x%lX, disabling client", pDevPort->intf,pDevPort->dir,psAfeClientInfo->client_id);
      }
      else
      {
        /**check if client has requested for pop suppressor.
        * pop suppressor will not be enabled when port is configured for low latency mode.
        * TO DO: Need to make changes in pop suppressor code to support 0.5ms frame size
        * */
        if ( (TRUE == psAfeClientInfo->afe_client.cust_proc.is_pop_suppressor_required)\
          && (AFE_PORT_DEFAULT_LATENCY_MODE== pDevPort->latency_mode) )
        {
          MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"pop suppressor init called from reinit at port start for client with clientId= 0x%lX connected to port dir =%d, intf =%x",
                psAfeClientInfo->client_id,pDevPort->dir, pDevPort->intf);

          if(ADSP_EOK == afe_pop_suppressor_init(psAfeClientInfo->afe_client.sample_rate,psAfeClientInfo->afe_client.channel,psAfeClientInfo->afe_client.bytes_per_channel,
                                                 pDevPort->sample_rate,&psAfeClientInfo->pop_suppressor_module_ptr,psAfeClientInfo->afe_client.buf_size))
          {
            /**<Update pop-suppressor delay in client_gp_delay*/
            afe_port_calc_client_gp_delay_us(psAfeClientInfo);
          }
        }
        psAfeClientInfo->prev_time = 0;

        afe_rate_match_info_t *rate_match_info_ptr = psAfeClientInfo->rate_match_info_ptr;

        /** release all the bufs for this client including partially consumed, if rate matching is enabled.*/
        if (rate_match_info_ptr != NULL)
        {
          if(ADSP_FAILED(
              result = afe_port_rate_match_info_reinit(pDevPort, psAfeClientInfo, rate_match_info_ptr)))
          {
            afe_port_rate_match_info_destroy(psAfeClientInfo);

            psAfeClientInfo->state = AFE_CLIENT_DISABLE;

            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Error in afe_port_rate_match_info_reinit port_id=%x,client_id=0x%lX, hence this client is disabled",
                  pDevPort->intf,psAfeClientInfo->client_id);
          }

          if (psAfeClientInfo->remaining_samples != 0)
          {
            if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(psAfeClientInfo->ps_curr_pcm_buff->pBufferReturnQ, \
                                                                       (elite_msg_any_payload_t*) psAfeClientInfo->ps_curr_pcm_buff)))
            {
              MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortHandler: thread encountered qurt_elite_queue_push_back error = %d!!\n", result);
            }

            psAfeClientInfo->remaining_samples = 0;
          }
        }

        /* vote for this client only when all re-initialization happens properly */
        if(AFE_CLIENT_ENABLE == psAfeClientInfo->state )
        {
          vote_now_for_client_topo = TRUE;
          afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psAfeClientInfo, TRUE);
        }
      }
    }
    pQNode = pQNode->next;
  }

  if(TRUE == vote_now_for_client_topo)
  {
    /*This voting is for clients already connected to port before port start. pDevPort itself enough to calculate dynamic kpps,
     *  hence sending afe_client ptr as NULL and is_client_connect as FALSE*/
    afe_port_update_dynamic_kpps(pDevPort, NULL, FALSE);
    if(ADSP_FAILED(result = afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AFE MMPM request failed for AFE_MMPM_CLIENT_TOPOLOGY, port-id: 0x%x",
            pDevPort->intf);
      afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
      return result;
    }
  }

  return result;
}

/**
 * This function is to vote for compressed clients which are connected to port before port start.
 * This function is invoked during port start
 * @param[in] pDevPort, pointer to instance of the port
 * @return none
 */
ADSPResult afe_port_compressed_client_voting_at_port_start(afe_dev_port_t* pDevPort)
{
  bufQList               *pQNode;
  afe_client_info_t      *psAfeClientInfo = NULL;
  ADSPResult             result = ADSP_EOK;
  bool_t                 vote_now_for_client_topo = FALSE;

  pQNode = pDevPort->afe_tx_client_list_ptr;
  while(NULL != pQNode)
  {
    psAfeClientInfo = (afe_client_info_t*)pQNode->element;
    if (pQNode->element != NULL)
    {
      if((LINEAR_PCM != psAfeClientInfo->afe_client.data_type))
      {
        vote_now_for_client_topo = TRUE;
        afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psAfeClientInfo, TRUE);
      }
    }
    pQNode = pQNode->next;
  }

  pQNode = pDevPort->afe_rx_client_list_ptr;
  while(NULL != pQNode)
  {
    psAfeClientInfo = (afe_client_info_t*)pQNode->element;
    if (pQNode->element != NULL)
    {
      if((LINEAR_PCM != psAfeClientInfo->afe_client.data_type))
      {
        vote_now_for_client_topo = TRUE;
        afe_port_update_aggregated_client_kpps_and_bw(pDevPort, psAfeClientInfo, TRUE);
      }
    }
    pQNode = pQNode->next;
  }

  if(TRUE == vote_now_for_client_topo)
  {
    if(ADSP_FAILED(result = afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_VOTE, TRUE)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AFE MMPM request failed for AFE_MMPM_CLIENT_TOPOLOGY, port-id: 0x%x",
            pDevPort->intf);
      afe_mmpm_voting(pDevPort, AFE_MMPM_CLIENT_TOPOLOGY, AFE_MMPM_DEVOTE, TRUE);
    }
  }

  return result;
}

/**
 * This function is the initialize the client resampler
 * connected to this port.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pClientInfo, ptr to instance of the client connecting to the port.
 * @return none
 */
ADSPResult afe_port_tx_client_resample_init(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo)
{
  uint32_t                         inFreq, outFreq;
  ADSPResult                       result = ADSP_EOK;

  outFreq = pClientInfo->afe_client.sample_rate;
  inFreq = pDevPort->sample_rate;
  result = afe_port_client_resample_init_common(pDevPort, pClientInfo, inFreq, outFreq);

  return result;
}

/**
 * This function is common resampler init code between rx and tx.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pClientInfo, ptr to instance of the client connecting to the port.
 * @param[in] inFreq, in frequency of the resampler.
 * @param[in] outFreq, out frequency of the resampler.
 * @return none
 */
static ADSPResult afe_port_client_resample_init_common(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo,
                                                       uint32_t inFreq, uint32_t outFreq)
{
  uint16_t min_num_channels, num_channels_client, num_channels_port;
  ADSPResult result = ADSP_EOK;
  uint32_t port_bytes_per_sample, client_bytes_per_sample, min_bytes_per_sample, min_bits_per_sample;
  samp_rate_conv_type_t resamp_type;

   /* Store the port latency mode in sample rate conversion structure for future reference*/
   pClientInfo->samp_rate_conv.port_latency_mode = pDevPort->latency_mode;

  if(inFreq == outFreq)
  {
    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "client 0x%lX and port dir = %d, intf = 0x%x sample rates equal. returning. ",
          pClientInfo->client_id, pDevPort->dir, pDevPort->intf);
    pClientInfo->int_samples_per_period = pDevPort->int_samples_per_period;

    /* Deinit the previous resampler 
	This can happen during runtime re-init of client resampler because of port 
	sampling rate change and no more need of resampling.*/
    sample_rate_conv_deinit(&pClientInfo->samp_rate_conv);

    afe_port_calc_client_gp_delay_us(pClientInfo);
    return result;
  }

  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort: Client 0x%lX Resample init, inFreq=%ld, outFreq=%ld, dir = %d, intf = 0x%x",
        pClientInfo->client_id, inFreq, outFreq, pDevPort->dir, pDevPort->intf);

  /* Calculate the minimum of port and client num channels */
  num_channels_client = pClientInfo->afe_client.channel;
  num_channels_port = pDevPort->channels;
  min_num_channels = (num_channels_client < num_channels_port) ?
      num_channels_client : num_channels_port;

  /* Calculate the minimum of port and client bytes per sample */
  port_bytes_per_sample = pDevPort->bytes_per_channel;
  client_bytes_per_sample = pClientInfo->afe_client.bytes_per_channel;

  min_bytes_per_sample = (port_bytes_per_sample < client_bytes_per_sample) ?
      port_bytes_per_sample : client_bytes_per_sample;

  min_bits_per_sample = ((pDevPort->bit_width) < (pClientInfo->afe_client.bit_width)) ?
      (pDevPort->bit_width) : (pClientInfo->afe_client.bit_width);

  /* Number of resamplers required is equal to minimum of port and client num channels */
  pClientInfo->unNumResamplers = min_num_channels;

  if((pClientInfo->afe_client.data_path == AFE_TX_OUT)
      || (pClientInfo->afe_client.data_path == AFE_PP_OUT)
      || (pClientInfo->afe_client.data_path == AFE_TX_PP_AUDIO_SENSE_OUT))
  {
      afe_port_get_frame_size(pDevPort->latency_mode, outFreq, &pClientInfo->int_samples_per_period);
    pClientInfo->samp_rate_conv.frame_size_in = pDevPort->int_samples_per_period;
    pClientInfo->samp_rate_conv.frame_size_out = pClientInfo->int_samples_per_period;
  }
  else
  {
      afe_port_get_frame_size(pDevPort->latency_mode, inFreq, &pClientInfo->int_samples_per_period);
    pClientInfo->samp_rate_conv.frame_size_in = pClientInfo->int_samples_per_period;
    pClientInfo->samp_rate_conv.frame_size_out = pDevPort->int_samples_per_period;
  }

  MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "client int_samples_per_period updated to: %ld, intf = %x",
        pClientInfo->int_samples_per_period, pDevPort->intf);

  if(pClientInfo->unNumResamplers > AFE_PORT_MAX_CHANNEL_CNT)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "AFEPortHandler : wrong number of resamplers %d in port 0x%x", pClientInfo->unNumResamplers, pDevPort->intf);
    return ADSP_EFAILED;
  }

  /* calling the function to determine the resampler type */
  resamp_type = get_samp_rate_conv_type(pClientInfo->afe_client.cust_proc.resampler_type, inFreq,
                                        outFreq, min_bytes_per_sample);

  /* resample init wrapper function */
  result = sample_rate_conv_init(&pClientInfo->samp_rate_conv, pClientInfo->unNumResamplers,
                                 min_bits_per_sample, inFreq, outFreq, resamp_type);

  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Resampler: port: 0x%x, client: 0x%lX and  Type assigned: %d "
        "buff_scheme: %lu rs_buff_ch_spacing_in_bytes: %lu",
        pDevPort->intf, pClientInfo->client_id,
        pClientInfo->samp_rate_conv.resamp_type, pClientInfo->samp_rate_conv.buff_scheme,
        pClientInfo->samp_rate_conv.rs_buff_ch_spacing_in_bytes);

  if(result != ADSP_EOK)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFEPortHandler : Error in initializing resampler object");
    return result;
  }

  // update client specific resampler delay. TBD: once resampler lib exposes the api us that for getting delay
  afe_port_calc_client_gp_delay_us(pClientInfo);

  return result;
}


/**
 * This function is the initialize the client resampler in Rx path
 * connected to this port.
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pClientInfo, ptr to instance of the client connecting to the port.
 * @return none
 */
ADSPResult afe_port_rx_client_resample_init(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo)
{
  uint32_t                         inFreq, outFreq;
  ADSPResult                       result = ADSP_EOK;

  inFreq = pClientInfo->afe_client.sample_rate;
  outFreq = pDevPort->sample_rate;
  result = afe_port_client_resample_init_common(pDevPort, pClientInfo, inFreq, outFreq);

  return result;
}

/**
 * This function is to send media type down to downstream
 *
 * @param[in] pDevPort, pointer to instance of the port
 * @param[in] pClientInfo, ptr to instance of the client connecting to the port.
 * @return ADSPResult
 */
ADSPResult afe_port_send_media_t(afe_dev_port_t* pDevPort, afe_client_info_t* pClientInfo)
{
  qurt_elite_bufmgr_node_t              bufMgrNode;
  int                                 	nActualSize;
  elite_msg_data_media_type_apr_t       *pMediaFormatData;
  elite_msg_any_t                      	msg;
  ADSPResult                           	result = ADSP_EOK;
  uint32_t        						      unMediaFormatID;

  // send regulat PCM format for all clients except audio
  unMediaFormatID = ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM;

  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_media_type_apr_t) + \
                                        sizeof(elite_multi_channel_pcm_fmt_blk_t), &bufMgrNode, &nActualSize)))
  {
    return ADSP_ENEEDMORE;
  }

  pMediaFormatData = (elite_msg_data_media_type_apr_t*) bufMgrNode.pBuffer;
  elite_multi_channel_pcm_fmt_blk_t *pMultiChMediaFormatBlk = (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pMediaFormatData);

  //Update the media format in the pClientInfo.
  pMultiChMediaFormatBlk->sample_rate = pClientInfo->afe_client.sample_rate;
  pMultiChMediaFormatBlk->num_channels = (uint16_t) (pClientInfo->afe_client.channel);
  pMultiChMediaFormatBlk->bits_per_sample = (uint16_t) (pClientInfo->afe_client.bit_width);
  pMultiChMediaFormatBlk->is_interleaved = (uint16_t) (pClientInfo->afe_client.interleaved);
  pMultiChMediaFormatBlk->is_signed = 1;

  memscpy(pMultiChMediaFormatBlk->channel_mapping,
          sizeof(pMultiChMediaFormatBlk->channel_mapping),
          pClientInfo->afe_client.channel_mapping,
          sizeof(pMultiChMediaFormatBlk->channel_mapping));

  pMediaFormatData->unMediaTypeFormat = ELITEMSG_DATA_MEDIA_TYPE_APR;
  pMediaFormatData->unMediaFormatID = unMediaFormatID;

  pMediaFormatData->pResponseQ = NULL;
  pMediaFormatData->unClientToken = 0;
  pMediaFormatData->pBufferReturnQ = bufMgrNode.pReturnQ;

  msg.unOpCode = ELITE_DATA_MEDIA_TYPE;
  msg.pPayload = (void*) pMediaFormatData;

  if (ADSP_EOK != (result = qurt_elite_queue_push_back(pClientInfo->afe_client.afe_client_handle->dataQ, (uint64_t*)&msg)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to connect services!!\n");
    elite_msg_return_payload_buffer(&msg);
    return result;
  }

  return result;
}


/**
 * This function is to create buffer and send it to the bufQ
 *
 * @param[in] pDevPort, port instance
 * @param[in] psNewClient, client instance
 * @param[in] buf_size_scale, how much to scale the actual buf size with
 * @param[in] buf_size_additional, how much to add to req buff size with buf_size_additional
 * @param[in] is_queue_init, whether or not queue is inited
 * @param[out] num_buffers_created, number of buffers actually created
 * @return the ADSP_EOK error code
 */
ADSPResult afe_port_create_buf(afe_dev_port_t *pDevPort, afe_client_info_t *psNewClient,
                               int16_t buf_size_scale, int16_t buf_size_additional, int16_t *is_queue_init, uint32_t *num_buffers_created )
{
  elite_msg_data_buffer_t    *psPcmBuff;
  ADSPResult                 result = ADSP_EOK;
  int                        reqSize, actualSize;
  uint32_t                   num_buffers, client_bytes_per_channel;
  char                       bufQName[16];
  char                       *queue_buf_ptr;
  qurt_elite_channel_t       *qurt_elite_channel;

  *num_buffers_created   =  0;
  *is_queue_init          =  FALSE;

  // create new buffers, buffQ and queue the buffers in bufQ.
  qurt_elite_strl_cpy(bufQName,"afeTxBufI0D0N0", sizeof(bufQName));
  bufQName[9] = pDevPort->intf + '0';
  bufQName[11] = pDevPort->dir + '0';
  bufQName[13] = pDevPort->num_client + '0';

  queue_buf_ptr = (char*) qurt_elite_memory_malloc(QURT_ELITE_QUEUE_GET_REQUIRED_BYTES(AFE_DEV_BUFQ_MAX),QURT_ELITE_HEAP_DEFAULT);

  if (NULL == queue_buf_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port bufQ Creation Failed");
    return ADSP_ENOMEMORY;
  }

  psNewClient->bufQ = (qurt_elite_queue_t*) queue_buf_ptr;

  qurt_elite_channel_init(&psNewClient->qurt_elite_channel);
  qurt_elite_channel = &psNewClient->qurt_elite_channel;


  if (ADSP_FAILED(result = qurt_elite_queue_init(bufQName, AFE_DEV_BUFQ_MAX, psNewClient->bufQ)) ||
      ADSP_FAILED(result = qurt_elite_channel_addq(qurt_elite_channel, psNewClient->bufQ, 0)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to init bufQ for client or add bufQ to channel %d!!\n", result);
    *is_queue_init = TRUE;
    return result;
  }
  *is_queue_init = TRUE;


  client_bytes_per_channel = psNewClient->afe_client.bytes_per_channel;

  if(3 == client_bytes_per_channel)
  {
    client_bytes_per_channel = 4;
  }

  //Create nNumBuffers of pcm Buffers and queue them up for usage
  num_buffers   = psNewClient->afe_client.num_buffers;
  actualSize    = psNewClient->afe_client.buf_size * \
      client_bytes_per_channel * \
      psNewClient->afe_client.channel * \
      buf_size_scale;

  psNewClient->actual_buf_size = actualSize;

  reqSize = GET_ELITEMSG_DATABUF_REQ_SIZE(actualSize + buf_size_additional);

  // Allocate and queue up the output buffers.
  for (uint32_t i = 0; i < num_buffers; i++)
  {
    //allocate the databuffer payload (metadata + pcm buffer size)
    psPcmBuff = (elite_msg_data_buffer_t*) qurt_elite_memory_malloc(reqSize, QURT_ELITE_HEAP_DEFAULT);
    if (NULL == psPcmBuff)
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"memory allocation failed to allocate elite msg data buffer object");
      return ADSP_ENOMEMORY;
    }

#ifdef DBG_BUFFER_ADDRESSES
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BUFFERADDR AFE Tx output Buffer: 0x%8x, size %d", psPcmBuff, reqSize);
#endif /* DBG_BUFFER_ADDRESSES */

    memset(psPcmBuff,0,reqSize);

    psPcmBuff->nActualSize     = actualSize;
    psPcmBuff->nMaxSize        = psPcmBuff->nActualSize + buf_size_additional;
    psPcmBuff->pBufferReturnQ  = psNewClient->bufQ;

    if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(psNewClient->bufQ, (elite_msg_any_payload_t*) psPcmBuff)))
    {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to fill ELiteExample buffer queue!! \n");
      qurt_elite_memory_free(psPcmBuff);
      return result;
    }
    (*num_buffers_created)++;
  }
  return result;
}

/**
 * This function is for sending the media type to all the Tx 
 * clients during the port start
 *
 * @param[in] pDevPort, pointer to port structure
 * @return ADSPResult, returns success or failure 
 * */ 
ADSPResult afe_port_send_media_type_to_tx_clients(afe_dev_port_t *pDevPort)
{
  ADSPResult                       result = ADSP_EOK;
  bufQList                         *sClientList;
  afe_client_info_t                *client_info_ptr;

  sClientList = pDevPort->afe_tx_client_list_ptr;

  /* Regular Tx clients */
  while (sClientList != NULL)
  {
    client_info_ptr = (afe_client_info_t *)sClientList->element;

    if (AFE_CLIENT_ENABLE == client_info_ptr->state)
    {
      result |= afe_port_send_media_t(pDevPort, client_info_ptr);
    }

    sClientList = sClientList->next;
  }

  if(ADSP_FAILED(result))
  {
    result = ADSP_EFAILED;
  }

  return result;
}

/**
 * This function is for sending the EOS message to all the Tx 
 * client connected to a port. 
 *
 * @param[in] pDevPort, pointer to port structure
 * @return ADSPResult, retuns success or failure 
 * */ 

ADSPResult afe_port_send_eos_to_tx_clients(afe_dev_port_t* pDevPort)
{
  ADSPResult                       result = ADSP_EOK;
  bufQList                         *sClientList;
  afe_client_info_t                *client_info_ptr;

  sClientList = pDevPort->afe_tx_client_list_ptr;

  /* Regular Tx clients */
  while (sClientList != NULL)
  {
    client_info_ptr = (afe_client_info_t *)sClientList->element;

    if (AFE_CLIENT_ENABLE == client_info_ptr->state)
    {
      result |= afe_port_send_last_buf_and_eos(client_info_ptr, pDevPort);
    }

    sClientList = sClientList->next;
  }

  if(ADSP_FAILED(result))
  {
    result = ADSP_EFAILED;
  }

  return result;
}

/**
 * This function is for sending the EOS message to one Tx client 
 *  
 * @param[in] client_info, pointer to client info structure 
 *  
 * @return ADSPResult, retuns success or failure 
 * */
ADSPResult afe_port_generate_tx_eos(afe_client_info_t *client_info)
{
  ADSPResult                             result = ADSP_EOK;
  qurt_elite_bufmgr_node_t               buf_mgr_node;
  elite_msg_data_eos_header_t            *eos_msg;
  elite_msg_any_t                        eos_data_msg;
  int                                    actual_size;

  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending EoS: client_id : 0x%lX", client_info->client_id);

  /* Send the EoS */
  if ( ADSP_FAILED(elite_mem_get_buffer(sizeof(elite_msg_data_eos_header_t), &buf_mgr_node, &actual_size)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get buffer mgr node while sending EoS.");
    return result;
  }

  eos_msg                    = (elite_msg_data_eos_header_t*) buf_mgr_node.pBuffer;
  eos_msg->pBufferReturnQ    = buf_mgr_node.pReturnQ;
  eos_msg->pResponseQ        = NULL;
  eos_msg->unClientToken     = 0;
  eos_msg->unEosFormat       = ELITEMSG_DATA_TX_EOS;
  eos_msg->unResponseResult  = 0;

  eos_data_msg.unOpCode      = ELITE_DATA_EOS;
  eos_data_msg.pPayload      = (void*) eos_msg;

  result = qurt_elite_queue_push_back(client_info->afe_client.afe_client_handle->dataQ, (uint64_t*) &eos_data_msg);

  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver EoS dowstream. Dropping");
    elite_msg_return_payload_buffer(&eos_data_msg);
  }

  return result;
}

ADSPResult afe_port_client_dc_reset_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg)
{
  ADSPResult                            result = ADSP_EOK;
  elite_msg_custom_afe_connect_t        *pPayload;
  elite_svc_handle_t                    *svc_handle_ptr;
  bufQList                              *pQNode;
  bufQList                              **ppQNode;
  afe_client_info_t                     *psAfeClientInfo = NULL;
  audio_sense_info_t                    *pAudSenseInfo=NULL;
  uint16_t                              i, mad_audio_ctr = 0, mad_beacon_ctr = 0, mad_ult_ctr = 0, index = 0;
  bool                                  sw_mad_client = FALSE;
  hw_mad_data_t                         *hw_mad_ptr = (hw_mad_data_t *) pDevPort->hw_mad_ptr;
  sw_duty_cycle_data_t                  *sw_duty_cycle_ptr = (sw_duty_cycle_data_t *)pDevPort->sw_duty_cycle_data_ptr;
  uint16_t mask_dc_reset_aggregation = 0;

  /*Iterate through all the tx clients and aggregate the mask_dc_reset
     If mask_dc_reset_aggregation = 0 then client state which sent DC reset will be moved to RESET
     Else it will be in ENABLE state only and EOS and MEDIA_FORMAT will be sent to process the new set of input data*/
  pQNode = pDevPort->afe_tx_client_list_ptr;
  ppQNode = &(pDevPort->afe_tx_client_list_ptr);
  for (index = 0; pQNode != NULL; index++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;

    if(TRUE == psAfeClientInfo->afe_client.cust_proc.audio_sense.mask_dc_reset)
    {
      mask_dc_reset_aggregation++;
    }

    ppQNode = &(pQNode->next);
    pQNode = *ppQNode;
  }

  pPayload = (elite_msg_custom_afe_connect_t *)(pMsg->pPayload);
  svc_handle_ptr = pPayload->svc_handle_ptr;
  pQNode = pDevPort->afe_tx_client_list_ptr;
  ppQNode = &(pDevPort->afe_tx_client_list_ptr);

  //Find the connected client in the client linked list
  for (i = 0; pQNode != NULL;i++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;

    pAudSenseInfo    =  &(psAfeClientInfo->afe_client.cust_proc.audio_sense);

    if (svc_handle_ptr == &(psAfeClientInfo->this_client_manager))
    { 
      if (AFE_CLIENT_ENABLE == psAfeClientInfo->state)
      {
        if(0 == mask_dc_reset_aggregation)
        {
          //Client should not be reset in the middle of sending data to clients
          afe_port_mutex_lock(pDevPort);
          // move the client state to RESET state for book keeping purpose
          psAfeClientInfo->state = AFE_CLIENT_RESET;
          afe_port_mutex_unlock(pDevPort);
        }
        else
        {
          MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_PASS_THROUGH enabled. sending EOS and Media format to client");

          //Port mutex is required to avoid a race condition where last buffer and EOS is sent from static service thread and data is
          //send from SW MAD background thread at the same time
          afe_port_mutex_lock(pDevPort);
          if(ADSP_EOK != (result = afe_port_send_last_buf_and_eos(psAfeClientInfo, pDevPort)))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the EOS msg, 0x%x, 0x%x", result, pDevPort->intf);
          }          

          if(ADSP_EOK != (result = afe_port_send_media_t(pDevPort, psAfeClientInfo)))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the MediaFmt msg, 0x%x, 0x%x", result, pDevPort->intf);
          }
          afe_port_mutex_unlock(pDevPort);
        }
      }

      // check if the DC reset cmd came from SW MAD 
      if (AFE_SW_MAD == pAudSenseInfo->type)
      {
        sw_mad_client = TRUE;
      }
    }

    if (AFE_CLIENT_ENABLE == psAfeClientInfo->state)
    {
      if (AFE_MAD_AUDIO == pAudSenseInfo->type)
      {
        // client enabled and it is MAD AUDIO client, then increment the ctr
        mad_audio_ctr++;
      }
      else if (AFE_MAD_BEACON == pAudSenseInfo->type)
      {
        // client enabled and it is MAD AUDIO client, then increment the ctr
        mad_beacon_ctr++;
      }
      else if (AFE_MAD_ULTRASOUND == pAudSenseInfo->type)
      {
        // client enabled and it is MAD AUDIO client, then increment the ctr
        mad_ult_ctr++;
      }
    }

    ppQNode = &(pQNode->next);
    pQNode = *ppQNode;
  }

  // low power mode sw mad client handling
  if (AFE_SW_MAD == pPayload->afe_client.cust_proc.audio_sense.type)
  {
    sw_mad_client = TRUE;
  }

  // if there is no active client receiving data, then we can go ahead and reset the duty cycle
  if ((0 == mad_audio_ctr)||(TRUE == sw_mad_client))
  {

    // if port HW MAD or sw duty cycle is created and initialized, then move ahead
    if (((NULL != hw_mad_ptr) && (TRUE == hw_mad_ptr->is_initialized))||\
        ((NULL != sw_duty_cycle_ptr) && (TRUE == sw_duty_cycle_ptr->is_initialized)))

    {
      //re-enable the reset clients
      pQNode = pDevPort->afe_tx_client_list_ptr;
      ppQNode = &(pDevPort->afe_tx_client_list_ptr);
      for (index = 0; pQNode != NULL; index++)
      {
        psAfeClientInfo = (afe_client_info_t* ) pQNode->element;
        afe_port_mutex_lock(pDevPort);
        // during state change, we touched only enabled clients and hence trying to revert the state back to enable only.
        if (AFE_CLIENT_RESET == psAfeClientInfo->state)
        {
          psAfeClientInfo->state = AFE_CLIENT_ENABLE;
        }
        afe_port_mutex_unlock(pDevPort);
        ppQNode = &(pQNode->next);
        pQNode = *ppQNode;
      }

      // hw mad duty cycle reset handling
      if (hw_mad_ptr != NULL)
      {
        afe_port_hw_mad_dc_reset_handler(pDevPort, AFE_HW_MAD_AUDIO);
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Performing duty cycle reset cmd in low power mode, 0x%X", pDevPort->intf);
      }
      else if (sw_duty_cycle_ptr != NULL)
      {
        afe_port_sw_duty_cycle_reset_handler(pDevPort);
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Performing Software duty cycle , 0x%X", pDevPort->intf);
      }

    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Received duty cycle reset cmd in high power mode, 0x%X", pDevPort->intf);

      // send the EOS and media fmt for MAD clients
      pQNode = pDevPort->afe_tx_client_list_ptr;
      ppQNode = &(pDevPort->afe_tx_client_list_ptr);
      for (index = 0; pQNode != NULL; index++)
      {
        psAfeClientInfo = (afe_client_info_t* ) pQNode->element;
        pAudSenseInfo    =  &(psAfeClientInfo->afe_client.cust_proc.audio_sense);
        // during state change, we touched only enabled clients and hence trying to revert the state back to enable only.
        if (AFE_CLIENT_RESET == psAfeClientInfo->state)
        {
          afe_port_mutex_lock(pDevPort);
          psAfeClientInfo->state = AFE_CLIENT_ENABLE;
          afe_port_mutex_unlock(pDevPort);

          if ((AFE_MAD_AUDIO == pAudSenseInfo->type)||(AFE_MAD_BEACON == pAudSenseInfo->type)||
              (AFE_MAD_ULTRASOUND == pAudSenseInfo->type)|| (AFE_SW_MAD == pAudSenseInfo->type))
          {
            afe_port_mutex_lock(pDevPort);
            if(ADSP_EOK != (result = afe_port_send_last_buf_and_eos(psAfeClientInfo, pDevPort)))
            {
              MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the EOS msg, 0x%x, 0x%x", result, pDevPort->intf);
            }
            afe_port_mutex_unlock(pDevPort);

            if(ADSP_EOK != (result = afe_port_send_media_t(pDevPort, psAfeClientInfo)))
            {
              MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the MediaFmt msg, 0x%x, 0x%x", result, pDevPort->intf);
            }
          }
        }

        ppQNode = &(pQNode->next);
        pQNode = *ppQNode;
      }
    }
  }

  if (ADSP_EOK != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADSP_FAILED to find the pNode for psAfeClientInfo = %lx!!\n", (uint32_t) psAfeClientInfo);
  }

  elite_msg_finish_msg(pMsg, result);

  return  result;
}
ADSPResult afe_port_client_criteria_register_handler (afe_dev_port_t* pDevPort, elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_custom_afe_client_criteria_register_t *pPayload = NULL;
  uint8_t criteria;
  bufQList *pQNode;
  bufQList  **ppQNode;
  elite_svc_handle_t *svc_handle_ptr;
  afe_client_info_t *psAfeClientInfo = NULL;
  audio_sense_info_t *pAudSenseInfo=NULL;
  bool_t criteria_enable = FALSE;
  uint16_t index = 0;
  bool_t is_client_matched = FALSE;

  pPayload = (elite_msg_custom_afe_client_criteria_register_t *)(pMsg->pPayload);

  if(NULL == pPayload)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_client_register received NULL payload");
    result = ADSP_EBADPARAM;
    goto __send_resp;
  }

  svc_handle_ptr = pPayload->svc_handle_ptr;
  criteria = pPayload->criteria;
  criteria_enable = pPayload->criteria_enable;

  pQNode = pDevPort->afe_tx_client_list_ptr;
  ppQNode = &(pDevPort->afe_tx_client_list_ptr);

  //Find the connected client in the client linked list
  for (index = 0; pQNode != NULL;index++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;

    if (svc_handle_ptr == &(psAfeClientInfo->this_client_manager))
    {
      is_client_matched = TRUE;
      break;
    }

    ppQNode = &(pQNode->next);
    pQNode = *ppQNode;
  }

  if(FALSE == is_client_matched)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_client_register received invalid client svc handle  0x%lx",(uint32_t)svc_handle_ptr);
    result = ADSP_EBADPARAM;
    goto __send_resp;
  }

  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
        "afe_port_client_register received with criterion : %d, criterion_enable = %d with client id = 0x%lX",
        criteria,criteria_enable,psAfeClientInfo->client_id);

  switch(criteria)
  {
    case AFE_PASS_THROUGH:
    {
      if(TRUE == criteria_enable)
      {
        /* make mask_dc_reset to TRUE to keep track of which clinets have registered for PASS_THROUGH criterion*/
        psAfeClientInfo->afe_client.cust_proc.audio_sense.mask_dc_reset = TRUE;

        pQNode = pDevPort->afe_tx_client_list_ptr;
        ppQNode = &(pDevPort->afe_tx_client_list_ptr);
        for (index = 0; pQNode != NULL; index++)
        {
          psAfeClientInfo = (afe_client_info_t* ) pQNode->element;

          pAudSenseInfo    =  &(psAfeClientInfo->afe_client.cust_proc.audio_sense);

          // Check for any LSM or SWMAD clients which have been in RESET state
          // and make those to ENABLE State and send EOS and MEDIA_FORMAT
          if (AFE_CLIENT_RESET == psAfeClientInfo->state)
          {

            //SWMAD will be however in ENABLE state only as we have only one instance
            // of SWMAD and it will not go to disable state if any of the MAD clients are active.
            // In this case the client which is sending Register command is active and so SWMAD will be in enable
            // But still making this for safety purpose
            if ((AFE_MAD_AUDIO == pAudSenseInfo->type)||
                (AFE_MAD_BEACON == pAudSenseInfo->type)||
                (AFE_MAD_ULTRASOUND == pAudSenseInfo->type)||
                (AFE_SW_MAD == pAudSenseInfo->type))
            {
              MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Moving Client : 0x%lX to enable state for DC Reset Aggregation",psAfeClientInfo->client_id);

              //Port mutex is required to avoid enabling client when data is being forwarded to clients in port thread
              afe_port_mutex_lock(pDevPort);

              psAfeClientInfo->state = AFE_CLIENT_ENABLE;

              if(ADSP_EOK != (result = afe_port_send_last_buf_and_eos(psAfeClientInfo, pDevPort)))
              {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the EOS msg, 0x%x, 0x%x", result, pDevPort->intf);
              }

              if(ADSP_EOK != (result = afe_port_send_media_t(pDevPort, psAfeClientInfo)))
              {
                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to send the MediaFmt msg, 0x%x, 0x%x", result, pDevPort->intf);
              }

              afe_port_mutex_unlock(pDevPort);
            }
          }

          ppQNode = &(pQNode->next);
          pQNode = *ppQNode;
        }
      }
      else
      {
        /* make mask_dc_reset to FALSE so that normal DC Reset functionality can be revoked*/
        psAfeClientInfo->afe_client.cust_proc.audio_sense.mask_dc_reset = FALSE;
      }
      break;
    }
    default:
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE received unsupported criterion: %d", criteria);
      result = ADSP_EFAILED;
      break;
    }
  }

  __send_resp:
  elite_svc_send_ack(pMsg, result);

  return result;
}

ADSPResult afe_port_update_client_media_fmt(afe_dev_port_t *p_dev_port, elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EFAILED;
  elite_svc_handle_t *svc_handle_ptr;
  bufQList *pQNode;
  bufQList **ppQNode;
  afe_client_info_t *p_afe_client_info = NULL;
  uint32_t client_cntr;
  afe_rate_match_info_t *rate_match_info_ptr;

  elite_msg_custom_client_media_format_update_t *pPayload;

  pPayload = (elite_msg_custom_client_media_format_update_t *)(pMsg->pPayload);
  svc_handle_ptr = pPayload->svc_handle_ptr;

  pQNode = p_dev_port->afe_rx_client_list_ptr;
  ppQNode = &(p_dev_port->afe_rx_client_list_ptr);

  /* Find the connected client in the client linked list */
  for(client_cntr = 0; pQNode != NULL; client_cntr++)
  {
    p_afe_client_info = (afe_client_info_t *)pQNode->element;

    if(svc_handle_ptr == &(p_afe_client_info->this_client_manager))
    {
      /* Re-init the resampler for this client */
      if(ADSP_EOK == (result = afe_port_rx_client_resample_init(p_dev_port, p_afe_client_info)))
      {
        /* Re-init the rate match info, and enable this client */
        rate_match_info_ptr = p_afe_client_info->rate_match_info_ptr;

        if(NULL != rate_match_info_ptr)
        {
          if(ADSP_FAILED(
              result = afe_port_rate_match_info_reinit(p_dev_port, p_afe_client_info, rate_match_info_ptr)))
          {
            afe_port_rate_match_info_destroy(p_afe_client_info);

            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Error in afe_port_rate_match_info_reinit port_id: 0x%lX, client_id: 0x%lX",
                  p_dev_port->intf, p_afe_client_info->client_id);

            /* Break the for loop */
            break;
          }
        }

        /* Enable this client if all the inits are success */
        if(ADSP_EOK == result)
        {
          qurt_elite_mutex_lock(&p_dev_port->port_mutex);
          p_afe_client_info->state = AFE_CLIENT_ENABLE;
          qurt_elite_mutex_unlock(&p_dev_port->port_mutex);

          MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Resamper re-init done for client_id: 0x%lX, port_id: 0x%lX",
                p_afe_client_info->client_id, p_dev_port->intf);

          /* Break the for loop */
          break;
        }
      }
    }
    else
    {
      ppQNode = &(pQNode->next);
      pQNode = *ppQNode;
    }
  }

  elite_msg_finish_msg(pMsg, result);

  return result;
}

ADSPResult afe_port_send_client_media_fmt_update(afe_dev_port_t *pDevPort,
                                                 afe_client_info_t *pClientInfo)
{
  ADSPResult result = ADSP_EOK;
  qurt_elite_bufmgr_node_t buf_mgr_node;
  elite_msg_any_t elite_msg;
  int actual_buf_size;

  elite_msg_custom_client_media_format_update_t *p_mf_update_msg;

  /* Get the message buffer from buffer manager */
  if(ADSP_FAILED(
      elite_mem_get_buffer(sizeof(elite_msg_custom_client_media_format_update_t), &buf_mgr_node,
                           &actual_buf_size)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED to get buf to send client media fmt msg, client_id: 0x%lX, port_id: 0x%lx",
          pClientInfo->client_id, pDevPort->intf);
    return ADSP_ENOMEMORY;
  }

  p_mf_update_msg = (elite_msg_custom_client_media_format_update_t *)buf_mgr_node.pBuffer;
  p_mf_update_msg->afe_id             = pDevPort->intf;
  p_mf_update_msg->buffer_returnq_ptr = buf_mgr_node.pReturnQ;
  p_mf_update_msg->responseq_ptr      = NULL;
  p_mf_update_msg->client_token       = 0;
  p_mf_update_msg->sec_op_code        = ELITEMSG_CUSTOM_AFECLIENT_MEDIA_FMT_UPDATE;
  p_mf_update_msg->svc_handle_ptr     = &pClientInfo->this_client_manager;

  elite_msg.unOpCode = ELITE_CUSTOM_MSG;
  elite_msg.pPayload = p_mf_update_msg;

  /* Push to the AFE service command queue */
  if(ADSP_EOK
      != (result = qurt_elite_queue_push_back(afe_svc_global_ptr->static_svc_cmdq_ptr,
                                              (uint64_t *)&elite_msg)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to push buf to AfeSvc cmd queue, result: %lu, port_id: 0x%lx",
          result, pDevPort->intf);

    /* Return the msg buffer */
    elite_msg_release_msg(&elite_msg);
  }

  return result;
}

ADSPResult afe_port_return_client_buffers(afe_dev_port_t *p_dev_port, bufQList *p_client_list,
                                          bool_t  is_rx_client)
{
  ADSPResult            result = ADSP_EOK;
  afe_client_info_t     *p_client_info;
  elite_msg_any_t       msg;

  while (p_client_list != NULL)
  {
    p_client_info = (afe_client_info_t *)p_client_list->element;

    /* Check for any partially used buffers, either for Tx or Rx clients */
    if (NULL != p_client_info)
    {
      if (p_client_info->remaining_samples != 0)
      {
        msg.pPayload = (void *)p_client_info->ps_curr_pcm_buff;

        if (ADSP_EOK != elite_msg_return_payload_buffer(&msg))
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to return client buffer, client_id: %u  result: %lu ",
                p_client_info->client_id, result);
        }
      }

      p_client_info->remaining_samples = 0;

      /* For Rx clients, return any more queued buffers */
      if (TRUE == is_rx_client)
      {
        do
        {
          /* Non-blocking MQ receive */
          result = qurt_elite_queue_pop_front(p_client_info->this_client_manager.dataQ,
                                              (uint64_t *)&msg);

          if (ADSP_EOK == result)
          {
            if(ELITE_DATA_BUFFER == msg.unOpCode)
            {
              /* return the data buffer without processing */
              elite_msg_return_payload_buffer(&msg);
            }
            else
            {
              /* Process EOS immediately. Cache media type and as part of port start,
               * re-initialization takes place with latest media type */
              afe_port_data_cmd(p_client_info, p_dev_port, &msg, TRUE);
            }
          }

        } while (ADSP_EOK == result);
      }
    }

    /* Point to next node in the list */
    p_client_list = p_client_list->next;
  } /* While loop */

  return result;
}

ADSPResult afe_port_reset_client_state(afe_dev_port_t *p_dev_port)
{
  ADSPResult          result = ADSP_EOK;

  /* Return Partially used buffers for Tx clients */
  if (p_dev_port->afe_tx_client_list_ptr)
  {
    afe_port_return_client_buffers(p_dev_port, p_dev_port->afe_tx_client_list_ptr, FALSE);
  }

  /* Return Partially used buffers for Rx clients */
  if (p_dev_port->afe_rx_client_list_ptr)
  {
    afe_port_return_client_buffers(p_dev_port, p_dev_port->afe_rx_client_list_ptr, TRUE);
  }

  return result;
}
