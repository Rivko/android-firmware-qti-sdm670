/*========================================================================
   This file contains AFE Port manager implementation (AFE Tx\Rx process)

  Copyright (c) 2009 -2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEPortManager.cpp#3 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "AFEDtmf.h"
#include "Interleaver.h"
#include "AFEPortManagement.h"
#include "AFEVfrHandler.h"
#include "AFESidetoneIIR.h"
#include "AFESampRateConv.h"
#include "AFETdmSlot.h"
#include "AFEPopSuppressor_api.h"
#include "topo_utils.h"
#include "AFEDsatDriver.h"
#include "AFERateMatch.h"
#include "adsp_asm_data_commands.h"
#include "AFEErrorHandler.h"
#include "AFEEncoder_If.h"
#include "AFEDecoder_If.h"
#include "AFELimiter.h"

/*==========================================================================
  Function declarations
  ========================================================================== */
static void afe_port_accumulate_client_samples(int16_t client_counter, int32_t nSamplesOut,
                                               int32_t nSamplesOutActual,
                                               int32_t* curr_ptr_dev_port_buf,
                                               int16_t* curr_ptr_client_buf, int32_t nChanSpacing,
                                               int32_t minChannel, int32_t nClientChannels);
static void afe_port_accumulate_client_samples_32(int16_t client_counter, int32_t nSamplesOut,
                                                  int32_t nSamplesOutActual,
                                                  int32_t* curr_ptr_dev_port_buf,
                                                  int32_t* curr_ptr_client_buf,
                                                  int32_t nChanSpacing, int32_t minChannel,
                                                  int32_t nClientChannels);
static void afe_port_accumulate_client_samples_64(int16_t client_counter, int32_t nSamplesOut,
                                                  int32_t nSamplesOutActual,
                                                  int64_t* curr_ptr_dev_port_buf,
                                                  int32_t* curr_ptr_client_buf,
                                                  int32_t nChanSpacing, int32_t minChannel,
                                                  int32_t nClientChannels);
static void afe_port_get_client_samples_and_add(afe_client_info_t *pClientInfo,
                                                afe_dev_port_t *pDevPort, int8_t *pDestBuf, int8_t *pScratchBuf,
                                                uint16_t client_counter);
static ADSPResult afe_port_send_samples_to_multiple_clients(afe_dev_port_t *pDevPort,
                                                            uint32_t unClientSampleMask,
                                                            int8_t *pSrcBuf, int8_t *pScratchBuf);

static void afe_port_add_sidetone(afe_dev_port_t* pDevPort, int8_t* in_out_buf_ptr, int8_t* tmp_buf_ptr,
                                  int8_t* scratch_buf_ptr, uint16_t *sidetone_counter);
static ADSPResult afe_port_mix_sidetone_samples(afe_dev_port_t* pDevPort, int8_t *port_buf_ptr,
                                                int8_t *sidetone_buf_ptr, int8_t *output_buf_ptr);
/*==========================================================================
  Global Variable  Definitions
========================================================================== */

//#define DEBUG_AFE
//#define DBG_MSG_ENABLE

#ifdef DEBUG_AFE
FILE *in_file = NULL;
char in_filename[200] = "in.pcm";
#endif //DEBUG_AFE


// #define AFE_NON_DIAG_DBG
#ifdef AFE_NON_DIAG_DBG
#define AFE_LOG_BUF_SIZE (1024*500)
int16_t afe_log_buf_tx[AFE_LOG_BUF_SIZE], afe_log_buf_rx[AFE_LOG_BUF_SIZE];
int16_t *afe_log_buf_tx_ptr = &afe_log_buf_tx[0], *afe_log_buf_rx_ptr = &afe_log_buf_rx[0];
#endif // AFE_NON_DIAG_DBG
/*==========================================================================
  Function defines
  ========================================================================== */

static void afe_port_add_sidetone(afe_dev_port_t* pDevPort, int8_t* in_out_buf_ptr, int8_t* work_buf_ptr,
                                  int8_t* scratch_buf_ptr, uint16_t *sidetone_counter)
{
  bufQList            *client_list_ptr;
  afe_client_info_t   *client_ptr;

  client_list_ptr = pDevPort->afe_rx_client_list_ptr;
  while(client_list_ptr != NULL)
  {
    client_ptr = (afe_client_info_t *)client_list_ptr->element;

    if((AFE_RX_IN == client_ptr->afe_client.data_path)
        && (LINEAR_PCM == client_ptr->afe_client.data_type))
    {
      /* Collect the side tone samples in secondary port buffer  */
      memset(work_buf_ptr, 0, pDevPort->working_buf_size);
      afe_port_get_client_samples_and_add(client_ptr, pDevPort, work_buf_ptr, scratch_buf_ptr, (*sidetone_counter)++);

      afe_port_mix_sidetone_samples(pDevPort, in_out_buf_ptr, work_buf_ptr, in_out_buf_ptr);
    }
    client_list_ptr = client_list_ptr->next;
  }
}

ADSPResult afe_group_device_forward_pcm_samples_thread_safe(void *p_group_dev_state)
{
  ADSPResult        result = ADSP_EOK;

  uint16_t          port_idx =0;
  afe_dev_port_t    *p_dev_port = NULL, *p_dev_port_run_state = NULL;
  bool_t			   is_mutex_locked[AFE_GROUP_DEVICE_NUM_PORTS] = {FALSE};

  afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)p_group_dev_state;

  /* Check if atleast one member port is in RUN state for this group */
  if (AFE_GROUP_DEVICE_STATE_RUN != p_group_device->group_state)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DMA interrupt signal when group is not in RUN state.");
    return ADSP_EOK;
  }

  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    /* Acquire lock only of running member port's mutexes.
     *
     * Do not acquire lock without checking port state, because during second member port start,
     * AFES thread locks the mutex of second member port and unlocks it only after doing all
     * initializations. During this time, if group device dynamic thread tries to acquire that mutex,
     * then dynamic thread needs to wait till AFES thread unlocks, leading to AFE signal miss*/
    if ( (NULL != p_dev_port) && (AFE_PORT_STATE_RUN == p_dev_port->port_state) )
    {
      qurt_elite_mutex_lock(&(p_dev_port->port_mutex));

      is_mutex_locked[port_idx] = TRUE;

      /* Locate and save the first running member port for future reference
       *
       * There may be a chance, where member port can be stopped in between
       * above port state check and mutex lock. So doing port state check
       * again, to get actual first running port.*/
      if( (NULL == p_dev_port_run_state) && (AFE_PORT_STATE_RUN == p_dev_port->port_state) )
      {
        // The is the first running port
        p_dev_port_run_state = p_dev_port;
      }
    }
  }

  /* Clear the multiplexed output buffer */
  memset(p_group_device->p_group_io_buf, 0, p_group_device->group_io_buf_size);


  /* Copy data from DMA buffer to p_group_io_buf via audioif function */
  if (NULL == p_dev_port_run_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe grp 0x%x forward process function no port in run state", p_group_device->group_id);
  }
  else if(NULL == p_dev_port_run_state->afe_drv_ftbl)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_drv_ftbl forward process function has not been configured in port 0x%x", p_dev_port_run_state->intf);
  }
  else if(p_dev_port_run_state->afe_drv_ftbl->process)
  {
    result =  p_dev_port_run_state->afe_drv_ftbl->process(p_dev_port_run_state->afe_drv,
                                                          (int16_t *)p_group_device->p_group_io_buf,
                                                          p_dev_port_run_state->int_samples_per_period);
  }

  afe_group_data_logging(p_group_device->group_id, p_group_device);

  /* Atleast one member port is in RUN state for this group */
  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    if ( (NULL != p_dev_port) &&
        (AFE_PORT_STATE_RUN == p_dev_port->port_state)
        && (TRUE == is_mutex_locked[port_idx])
    )
    {
      //forward PCM data to each client
      result = afe_port_forward_pcm_samples(p_dev_port);
    }
  }

  /* Release all the member port's locked mutexes */
  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    if ( (NULL != p_dev_port) && (TRUE == is_mutex_locked[port_idx]) )
    {
      qurt_elite_mutex_unlock(&(p_dev_port->port_mutex));
    }
  }

  return result;

}



ADSPResult afe_group_device_render_pcm_samples_thread_safe(void *p_group_dev_state)
{
  ADSPResult        result = ADSP_EOK;
  uint16_t          port_idx =0;
  afe_dev_port_t    *p_dev_port = NULL, *p_dev_port_run_state = NULL;
  bool_t            is_mutex_locked[AFE_GROUP_DEVICE_NUM_PORTS] = {FALSE};
  bool_t            is_header_filled = FALSE;

  afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)p_group_dev_state;

  /* Check if atleast one member port is in RUN state for this group */
  if (AFE_GROUP_DEVICE_STATE_RUN != p_group_device->group_state)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "DMA interrupt signal when group is not in RUN state.");
    return ADSP_EOK;
  }

  /* Acquire lock only of running member port's mutexes.
   *
   * Do not acquire lock without checking port state, because during second member port start,
   * AFES thread locks the mutex of second member port and unlocks it only after doing all
   * initializations. During this time, if group device dynamic thread tries to acquire that mutex,
   * then dynamic thread needs to wait till AFES thread unlocks, leading to AFE signal miss*/
  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    if ( (NULL != p_dev_port) && (AFE_PORT_STATE_RUN == p_dev_port->port_state) )
    {
      qurt_elite_mutex_lock(&(p_dev_port->port_mutex));

      is_mutex_locked[port_idx] = TRUE;
    }
  }


  /* Clear the multiplexed output buffer */
  memset(p_group_device->p_group_io_buf, 0, p_group_device->group_io_buf_size);

  /* Atleast one member port is in RUN state for this group */
  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    if ( (NULL != p_dev_port) &&
        (AFE_PORT_STATE_RUN == p_dev_port->port_state) &&
        (TRUE == is_mutex_locked[port_idx])
    )
    {
      /* Render data for the member port */
      result = afe_port_render_pcm_samples(p_dev_port);

      /* Render the data based on slot remap info */
      result = afe_slot_mapping_rx(p_dev_port, &is_header_filled);

      /* This point to a last running dev_port for below rendering */
      p_dev_port_run_state = p_dev_port;
    }
  }

  afe_group_data_logging(p_group_device->group_id, p_group_device);

  /* Render the multiplexed output buffer */
  if (NULL == p_dev_port_run_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe grp 0x%x rendering process function no port in run state", p_group_device->group_id);
  }
  else if(NULL == p_dev_port_run_state->afe_drv_ftbl)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_drv_ftbl rendering process function has not been configured in port 0x%x", p_dev_port_run_state->intf);
  }
  else if(p_dev_port_run_state->afe_drv_ftbl->process)
  {
    result =  p_dev_port_run_state->afe_drv_ftbl->process(p_dev_port_run_state->afe_drv,
                                                          (int16_t *)p_group_device->p_group_io_buf,
                                                          p_dev_port_run_state->int_samples_per_period);
  }

  /* Release all the member port's locked mutexes */
  for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
  {
    p_dev_port = p_group_device->p_dev_port[port_idx];

    if ( (NULL != p_dev_port) && (TRUE == is_mutex_locked[port_idx]) )
    {
      qurt_elite_mutex_unlock(&(p_dev_port->port_mutex));
    }
  }

  return result;
}

ADSPResult afe_port_render_pcm_samples_thread_safe(void *vpDevPort)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t *pDevPort = (afe_dev_port_t*) vpDevPort;
  qurt_elite_mutex_lock( &(pDevPort->port_mutex) );
  result = afe_port_render_pcm_samples(pDevPort);
  qurt_elite_mutex_unlock( &(pDevPort->port_mutex) );
  return result;
}

/**
 * This function is for sink data from client to DMA.
 * It traverses through the RX linked list, passes them through the topology,
 * (optionally encodes the data), prepares DMA samples.
 *
 * NOTE: Called for RX port only
 *
 * @param[in] pDevPort, instance of the port
 * @return none
 */
ADSPResult afe_port_render_pcm_samples(afe_dev_port_t *pDevPort)
{
  bufQList            *client_list_ptr;
  uint32_t             num_samples_per_int,num_dev_samples_per_int;
  afe_client_info_t   *client_ptr;
  ADSPResult          result = ADSP_EOK;
  uint16_t            sidetone_counter=0, rx_client_counter = 0;
  int8_t              *dev_buf_ptr = NULL;
  afe_port_encoder_params_t *afe_port_encoder_ptr = (afe_port_encoder_params_t *)(pDevPort->port_enc_dec_ptr);

  if (AFE_PORT_STATE_RUN != pDevPort->port_state)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port signal when it is not in RUN state.");
    return ADSP_EOK;
  }

  num_samples_per_int = pDevPort->int_samples_per_period;
  afe_port_update_timestamp(pDevPort);
  afe_port_dangergen_enable(pDevPort);
  afe_port_update_avt_drift(pDevPort);

  //memset the working buffer
  memset(pDevPort->working_buf_1, 0, pDevPort->working_buf_size);

  //Prepare DmaSamples by going through each client
  client_list_ptr = pDevPort->afe_rx_client_list_ptr;
  while(NULL != client_list_ptr)
  {
    client_ptr = (afe_client_info_t *)client_list_ptr->element;
    if ((AFE_RX_VOC_MIXER_IN == client_ptr->afe_client.data_path) || (AFE_RX_AUD_MIXER_IN == client_ptr->afe_client.data_path))
    {
      afe_port_get_client_samples_and_add(client_ptr, pDevPort, pDevPort->working_buf_1, pDevPort->scratch_buf, rx_client_counter++);
    }
    client_list_ptr = client_list_ptr->next;
  }

  //add DTMF
  if(NULL != pDevPort->dtmf_info_ptr)
  {
    afe_port_gen_and_add_DTMF(pDevPort, (int16_t*)pDevPort->working_buf_1, (int16_t *)pDevPort->scratch_buf,
                              &rx_client_counter);
  }

  //limiter is non in-place module. input: working_buf_1, output: working_buf_2
  afe_port_limiter_process(pDevPort, pDevPort->working_buf_1, pDevPort->working_buf_2,
                           rx_client_counter, num_samples_per_int);

  //pass through topology. input: working_buf_2, output: working_buf_2
  if (NULL != pDevPort->port_topo_ptr)
  {
    topo_process(pDevPort->port_topo_ptr, pDevPort->working_buf_2, pDevPort->working_buf_2, NULL,0,FALSE);
  }

  //send audio+voice samples to audio+voice clients
  if ( NULL != pDevPort->afe_tx_client_list_ptr) //Regular EC client
  {
    afe_port_send_samples_to_multiple_clients(pDevPort, AFE_PP_OUT, pDevPort->working_buf_2,
                                              (int8_t *)pDevPort->scratch_buf);
  }

  //collect the side-tone samples and mix with port samples
  afe_port_add_sidetone(pDevPort, pDevPort->working_buf_2, pDevPort->working_buf_1, pDevPort->scratch_buf,
                        &sidetone_counter);

  //at this moment, working_buf_2 has the correct samples
  dev_buf_ptr = pDevPort->working_buf_2;

  //At this point, the common port processing ends. use-case specific processing begins
  if(NULL == afe_port_encoder_ptr)
  {
    //normal use case (no encoding in AFE)
    if (TRUE == pDevPort->is_interleaved)
    {
      //for interleaved device type, input: working_buf_2 (de-interleaved), output: working_buf_1 (interleaved)
      afe_port_interleave_samples(pDevPort->working_buf_2, pDevPort->working_buf_1, pDevPort->channels, num_samples_per_int, pDevPort->bytes_per_channel);
      dev_buf_ptr = pDevPort->working_buf_1;
    }

    //log RX PCM samples when 0x1586 log code is enabled in QXDM
    afe_port_data_logging(pDevPort, dev_buf_ptr, pDevPort->is_interleaved);

    //collect TS for AV-sync test
    if (NULL != pDevPort->avsync_test_ptr)
    {
      afe_port_collect_avsync_stats(pDevPort, dev_buf_ptr);
    }
  }
  else
  {
    //encoding in AFE

    //Input to encoder: de-interleaved PCM, output from encoder: interleaved encoded data.
    //Logging before encoder is handled in AFE svc. Logging after encoding is handled in encoder svc.
    //Output on device is as follows:
    //i2s (mono): same data duplicated on two channels. i2s (stereo): encoded data in snake fashion.
    //Slimbus (mono): only mono data transmitted. Slimbus (stereo): encoded data is de-interleaved and sent on separate slimbus descriptors.
    if (TRUE == pDevPort->is_interleaved)
    {
      //for interleaved device type, input: working_buf_2 (de-interleaved), output: working_buf_1 (interleaved)
      afe_port_interleave_samples(pDevPort->working_buf_2, pDevPort->working_buf_1, pDevPort->channels, num_samples_per_int, pDevPort->bytes_per_channel);

      //log RX PCM samples when 0x1586 log code is enabled in QXDM
      afe_port_data_logging(pDevPort, pDevPort->working_buf_1, pDevPort->is_interleaved);
    }
    else
    {
      //for non-interleaved device type, log RX PCM samples when 0x1586 log code is enabled in QXDM
      afe_port_data_logging(pDevPort, dev_buf_ptr, pDevPort->is_interleaved);
    }

    //at this moment, working_buf_2 (still) has the correct samples. therefore, dev_buf_ptr is correctly assigned
    //collect TS for AV-sync test
    if (NULL != pDevPort->avsync_test_ptr)
    {
      afe_port_collect_avsync_stats(pDevPort, dev_buf_ptr);
    }

    num_dev_samples_per_int = pDevPort->dev_int_samples_per_period;

    //update bytes_needed (on output of encoder)

    uint32_t bytes_needed = pDevPort->dev_channels * num_dev_samples_per_int * pDevPort->dev_bytes_per_channel;
    int8_t *enc_out_ptr = NULL;

    //setup the encoder output buffer
    if (TRUE == pDevPort->is_interleaved)
    {
      //for interleaved device type
      enc_out_ptr = afe_port_encoder_ptr->working_buf_dev;
    }
    else
    {
      //for non-interleaved device type
      enc_out_ptr = afe_port_encoder_ptr->working_buf_int_dev;
    }

    //input: dev_buf_ptr (working_buf_2) -> enc_svc dataQ -> afe_dummy_svc dataQ -> output: enc_out_ptr
    afe_send_and_get_data_encoder(pDevPort, dev_buf_ptr, enc_out_ptr, bytes_needed);

    if (FALSE == pDevPort->is_interleaved)
    {
      //for non-interleaved device type, input: enc_out_ptr (interleaved encoded), output: working_buf_dev (de-interleaved encoded)
      afe_port_de_interleave_samples(enc_out_ptr, afe_port_encoder_ptr->working_buf_dev, pDevPort->dev_channels, num_dev_samples_per_int, pDevPort->dev_bytes_per_channel);
    }
    //for interleaved device type, output is already in working_buf_dev. nothing to be done.

    //set the dev_buf_ptr
    dev_buf_ptr = afe_port_encoder_ptr->working_buf_dev;
    // update the num-sampels_per_int to dev_samples_per_int, as this value will be different.
    num_samples_per_int =  num_dev_samples_per_int;
  }

  //At this point, the use-case specific part ends. common device processing begins.
  if ((NULL != pDevPort->afe_dev_process) && (NULL == pDevPort->p_group_device_state))
  {
    result =  pDevPort->afe_dev_process(pDevPort, (int16_t *)dev_buf_ptr, num_samples_per_int);
  }
  else if(NULL != pDevPort->afe_drv_ftbl && NULL == pDevPort->p_group_device_state)
  {
    if(pDevPort->afe_drv_ftbl->process)
    {
      result =  pDevPort->afe_drv_ftbl->process(pDevPort->afe_drv, (int16_t *)dev_buf_ptr, num_samples_per_int);
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_drv_ftbl rendering process function has not been configured in port 0x%x", pDevPort->intf);
    }
  }

  pDevPort->group_device_out_buf_ptr = dev_buf_ptr;

  /* If signal miss information is configured, then
   * forces signal miss, when the port's running
   * interrupt count matches with the configured
   * interrupt count*/
  if(NULL != signal_miss_pool_global_ptr)
  {
    afe_err_handler_force_signal_miss(pDevPort);
  }

  afe_port_dangergen_disable(pDevPort);

  return result;
}

ADSPResult afe_port_forward_pcm_samples_thread_safe(void *vpDevPort)
{
  ADSPResult result;
  afe_dev_port_t *pDevPort = (afe_dev_port_t*) vpDevPort;
  qurt_elite_mutex_lock( &(pDevPort->port_mutex) );
  result = afe_port_forward_pcm_samples(pDevPort);
  qurt_elite_mutex_unlock( &(pDevPort->port_mutex) );
  return result;
}

/**
 * This function is for source data from DMA to client.
 * It collects DMA samples, (optionally decodes the data), passes them through the topology,
 * traverses through the TX linked list and forwards data to them.
 *
 * NOTE: Called for TX port only
 *
 * @param[in] pDevPort, instance of the port
 * @return none
 */
ADSPResult afe_port_forward_pcm_samples(afe_dev_port_t *pDevPort)
{
  ADSPResult          result = ADSP_EOK;
  uint32_t            num_samples_per_int;
  int8_t              *working_buf_ptr = NULL;
  afe_port_decoder_params_t *afe_port_decoder_ptr = (afe_port_decoder_params_t *)(pDevPort->port_enc_dec_ptr);
  bool_t is_log_data_interleaved = pDevPort->is_interleaved;

  if (AFE_PORT_STATE_RUN != pDevPort->port_state)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port signal when it is not in RUN state.");
    return ADSP_EOK;
  }

  if (NULL == pDevPort->afe_dev_process && NULL == pDevPort->afe_drv_ftbl)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Process function is not registered with port");
    return ADSP_EFAILED;
  }

  num_samples_per_int = pDevPort->int_samples_per_period;
  afe_port_update_timestamp(pDevPort);
  afe_port_dangergen_enable(pDevPort);
  afe_port_update_avt_drift(pDevPort);

  //assign working_buf_ptr according to the use case
  if(NULL == afe_port_decoder_ptr)
  {
    working_buf_ptr = pDevPort->working_buf_1;
  }
  else
  {
    if(TRUE == pDevPort->is_interleaved)
    {
      //for interleaved device type
      working_buf_ptr = afe_port_decoder_ptr->working_buf_dev;
    }
    else
    {
      //for non-interleaved device type
      working_buf_ptr = afe_port_decoder_ptr->working_buf_int_dev;
    }
  }

  //get samples from DMA
  if(NULL != pDevPort->p_group_device_state)
  {
    //PCM data from DMA buffer is in p_group_device->p_group_io_buf, copy from p_group_io_buf to pDevPort->working_buf_1 basing on slot mapping
    uint32_t  num_port_bytes_all_channel = (pDevPort->channels * pDevPort->bytes_per_channel);
    memset(working_buf_ptr, 0, pDevPort->int_samples_per_period*num_port_bytes_all_channel);
    result = afe_slot_mapping_tx(pDevPort);
  }
  else if(NULL != pDevPort->afe_dev_process)
  {
    result =  pDevPort->afe_dev_process(pDevPort, (int16_t *)working_buf_ptr, num_samples_per_int);
  }
  else if(NULL != pDevPort->afe_drv_ftbl)
  {
    if(pDevPort->afe_drv_ftbl->process)
    {
      result =  pDevPort->afe_drv_ftbl->process(pDevPort->afe_drv, (int16_t *)working_buf_ptr, num_samples_per_int);
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_drv_ftbl rendering process function has not been configured in port 0x%x", pDevPort->intf);
    }
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe rendering process function has not been configured in port 0x%x", pDevPort->intf);
  }

  if(NULL != afe_port_decoder_ptr)
  {
    //decoding in AFE

    //Input from device is as follows:
    //i2s (mono): mono channel on the first half of the buffer, second half ignored. i2s (stereo): encoded data in snake fashion.
    //Slimbus (mono): only mono data received. Slimbus (stereo): encoded data is de-interleaved and received on separate slimbus descriptors.
    if(FALSE == pDevPort->is_interleaved)
    {
      //for non-interleaved device type, input: working_buf_int_dev (de-interleaved encoded), output: working_buf_dev (interleaved encoded)
      afe_port_interleave_samples(working_buf_ptr, afe_port_decoder_ptr->working_buf_dev, pDevPort->dev_channels, num_samples_per_int, pDevPort->dev_bytes_per_channel);
    }
    //for interleaved device type, output is already in working_buf_dev. nothing to be done.

    //input to decoder: interleaved encoded data, output from decoder: de-interleaved PCM
    //update bytes_needed (from output of decoder)
    uint32_t bytes_needed = pDevPort->channels * num_samples_per_int * pDevPort->bytes_per_channel;

    //input: working_buf_dev -> dec_svc dataQ -> afe_dummy_svc dataQ -> output: working_buf_ptr
    afe_send_and_get_data_decoder(pDevPort, afe_port_decoder_ptr->working_buf_dev, working_buf_ptr, bytes_needed);

    afe_port_decoder_params_t *afe_port_decoder_ptr = (afe_port_decoder_params_t *)(pDevPort->port_enc_dec_ptr);

    /** Dec delay is updated only after it sends media fmt.*/
    if (afe_port_decoder_ptr->prev_delay != afe_port_decoder_ptr->decoder_delay)
    {
      afe_port_set_group_delay(pDevPort);
      afe_port_decoder_ptr->prev_delay = afe_port_decoder_ptr->decoder_delay;
    }

    //once decoding is done, data is deinterleaved.
    is_log_data_interleaved = FALSE;
  }

  //collect TS for AV-sync test
  if (NULL != pDevPort->avsync_test_ptr)
  {
    afe_port_collect_avsync_stats(pDevPort, working_buf_ptr);
  }

  //log TX PCM samples when 0x1586 log code is enabled in QXDM
  afe_port_data_logging(pDevPort, working_buf_ptr, is_log_data_interleaved);

  //for non-decoding use case, we need de-interleave the data for interleaved device type
  //for decoding use case, the data is already de-interleaved, nothing to do
  //for non-interleaved device type, nothing to do
  if(TRUE == pDevPort->is_interleaved && NULL == afe_port_decoder_ptr)
  {
    //for interleaved device type, input: working_buf_ptr (interleaved), output: working_buf_2 (de-interleaved)
    afe_port_de_interleave_samples(working_buf_ptr, pDevPort->working_buf_2, pDevPort->channels, num_samples_per_int, pDevPort->bytes_per_channel);
    working_buf_ptr = pDevPort->working_buf_2;
  }

  //send port samples to clients expecting unprocessed data (prior to topo)
  afe_port_send_samples_to_multiple_clients(pDevPort, AFE_TX_OUT, working_buf_ptr, pDevPort->scratch_buf);

  //pass through topology. input: working_buf_ptr, output: working_buf_ptr
  if (NULL != pDevPort->port_topo_ptr)
  {
    topo_process(pDevPort->port_topo_ptr, working_buf_ptr, working_buf_ptr,NULL,0,FALSE);
  }

  //send port samples to LSM clients/clients expecting AFE topo processed output
  if (pDevPort->sw_mad_ptr)
  {
    afe_port_send_samples_to_multiple_clients(pDevPort, AFE_PP_OUT, working_buf_ptr, pDevPort->scratch_buf);
  }
  else
  {
    afe_port_send_samples_to_multiple_clients(pDevPort, AFE_PP_OUT| AFE_TX_PP_AUDIO_SENSE_OUT, working_buf_ptr, pDevPort->scratch_buf);
  }

  pDevPort->group_device_out_buf_ptr = working_buf_ptr;

  /* If signal miss information is configured, then
   * forces signal miss, when the port's running
   * interrupt count matches with the configured
   * interrupt count*/
  if(NULL != signal_miss_pool_global_ptr)
  {
    afe_err_handler_force_signal_miss(pDevPort);
  }

  afe_port_dangergen_disable(pDevPort);

  return result;
}

void afe_port_return_buf_to_return_q(afe_client_info_t *pClientInfo)
{
  ADSPResult result;

  //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: RX return buf: %X", (uint32_t) pClientInfo->ps_curr_pcm_buff);
  if(pClientInfo->ps_curr_pcm_buff)
  {
    if(ADSP_FAILED(
        result = elite_msg_push_payload_to_returnq(
            pClientInfo->ps_curr_pcm_buff->pBufferReturnQ,
            (elite_msg_any_payload_t* ) pClientInfo->ps_curr_pcm_buff)))
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortMng: elite_msg_push_payload_to_returnq error = %d cl_id: 0x%lX",
            result, pClientInfo->client_id);
      return;
    }

    pClientInfo->remaining_samples = 0;
    pClientInfo->ps_curr_pcm_buff = NULL;
  }
}

void afe_port_send_buf_to_data_q(afe_client_info_t *pClientInfo)
{
  ADSPResult result;
  qurt_elite_bufmgr_node_t outBufMgrNode;
  elite_msg_any_t* pPeerDataQMsg;

  /* Send output buffers to downstream service. */
  outBufMgrNode.pReturnQ = pClientInfo->bufQ;
  outBufMgrNode.pBuffer = pClientInfo->ps_curr_pcm_buff;
  //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: TX sending buf: %X", (uint32_t) pClientInfo->ps_curr_pcm_buff);
  pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(&outBufMgrNode,
                                                  ELITE_DATA_BUFFER,
                                                  NULL, /* do not need response */
                                                  0, /* token */
                                                  0); /* do not care response result*/
  result = qurt_elite_queue_push_back(pClientInfo->afe_client.afe_client_handle->dataQ,
                                      (uint64_t*)pPeerDataQMsg);

  if(ADSP_FAILED(result))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
    (void)elite_msg_push_payload_to_returnq(pClientInfo->bufQ,
                                            (elite_msg_any_payload_t*)outBufMgrNode.pBuffer);
  }

  pClientInfo->curr_buff_ptr = NULL;
  pClientInfo->ps_curr_pcm_buff = NULL;
}

void afe_port_update_non_rate_match_client_buf(afe_client_info_t *pClientInfo,
                                               bool_t return_client_buffer, uint32_t samples_in)
{
  if( NULL == pClientInfo->rate_match_info_ptr)
  {
    uint32_t in_size_in_bytes = samples_in * pClientInfo->afe_client.bytes_per_channel;

    // Update the client's remaining samples left.
    pClientInfo->remaining_samples -= samples_in;

    // If the client's buffer is fully used-out, send the buffer back to client.
    if((pClientInfo->remaining_samples == 0) && (TRUE == return_client_buffer))
    {
      afe_port_return_buf_to_return_q(pClientInfo);
    }
    else /* Update the buffer pointer*/
    {
      if(pClientInfo->afe_client.interleaved)
      {
        pClientInfo->curr_buff_ptr += (in_size_in_bytes * pClientInfo->afe_client.channel);
      }
      else
      {
        pClientInfo->curr_buff_ptr += (in_size_in_bytes);
      }
    }
  }
}

void afe_port_print_underrun_msg(afe_client_info_t *pClientInfo, afe_dev_port_t *pDevPort)
{
  uint64_t currTime, diff;
  pClientInfo->under_run_counter++;

  // Use the wallk clk time just to avoid the flood of under-run messages
  currTime = qurt_elite_timer_get_time();

  diff = currTime - pClientInfo->prev_time;

  if(diff >= AFE_UNDERRUN_TIME_THRESH)
  {
    MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "AfePort intf=%x, dir=%d: Buffer not available from client 0x%lX. CurrTime-PrevTime = %ld us. Num Underrun = %lu",
          pDevPort->intf,pDevPort->dir, pClientInfo->client_id, (uint32_t) diff,pClientInfo->under_run_counter);

    pClientInfo->prev_time = currTime;
    pClientInfo->under_run_counter = 0;
  }
}

/**
 * gets the bufs from queue
 */
ADSPResult afe_port_get_buf_from_client_queue(afe_client_info_t *pClientInfo,
                                              afe_dev_port_t *pDevPort, int8_t *pDestBuf,
                                              int8_t **client_data_ptr, uint32_t *cl_ch_spacing_ptr,
                                              uint32_t *nSamplesOutActual, uint32_t *nSamplesIn)
{
  int32_t nClientChannels;
  uint32_t unChannelStatus;
  uint32_t nSamplesOut;
  elite_msg_any_t myDataQMsg;
  ADSPResult result;
  uint32_t output_size_in_samples;
  int8_t *out_ptr;
  int8_t *dummy_src_ptr = NULL;
  bool_t dummy_flag1 = FALSE;
  bool_t dummy_flag2 = FALSE;
  bool_t underrun_flag;

  *client_data_ptr = NULL;
  *cl_ch_spacing_ptr = 0;

  // If rx-client has no buffer to fill the dma buffer, request for one buffer.
  if(pClientInfo->remaining_samples == 0)
  {
    qurt_elite_queue_t * client_dataQ = pClientInfo->this_client_manager.dataQ;
    qurt_elite_channel_t *dataQ_chan = qurt_elite_queue_get_channel(client_dataQ);
    uint32_t dataQ_chan_bit = qurt_elite_queue_get_channel_bit(client_dataQ);
    uint16_t data_is_available = 0;

    //Time for new samples from Client's dataQueue
    while((AFE_CLIENT_ENABLE == pClientInfo->state)
        && (0 != (unChannelStatus = qurt_elite_channel_poll(dataQ_chan, dataQ_chan_bit))))
    {
      //pop dataQ until it's empty or until we have data buffer.
      result = qurt_elite_queue_pop_front(client_dataQ, (uint64_t*)&myDataQMsg);
      if(ADSP_FAILED(result))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure on mq %p qurt_elite_queue_pop_front: %d\n",
              (client_dataQ), result);
        break;
      }

      if(ELITE_DATA_BUFFER == myDataQMsg.unOpCode)
      {
        data_is_available = 1;
        break;
      }

      //handle data cmd
      afe_port_data_cmd(pClientInfo, pDevPort, &myDataQMsg, FALSE);
    }

    /* If data is available from client */
    if(data_is_available == 1)
    {
      //update the instance status
      pClientInfo->ps_curr_pcm_buff = (elite_msg_data_buffer_t*)myDataQMsg.pPayload;
      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port intf: 0x%x received buffer of size %d samples", pDevPort->intf, pClientInfo->ps_curr_pcm_buff->nActualSize/2);

      //retrieve the information about the data buffer.
      nClientChannels = pClientInfo->afe_client.channel;

      pClientInfo->remaining_samples = (((pClientInfo->ps_curr_pcm_buff)->nActualSize)
          / (pClientInfo->afe_client.bytes_per_channel * nClientChannels));

      if(NULL != pClientInfo->pop_suppressor_module_ptr)
      {

        //following function will do the ramp up (if required) also maintain the steady state
        afe_pop_suppressor_process((int8_t *)&(pClientInfo->ps_curr_pcm_buff->nDataBuf), &out_ptr,
                                   pClientInfo->remaining_samples, &output_size_in_samples,
                                   pClientInfo->pop_suppressor_module_ptr, dummy_flag1, &dummy_flag2,
                                   DATA_AVAILABLE);
        pClientInfo->curr_buff_ptr = out_ptr; //update pClientInfo->curr_buff_ptr to a pointer from pop suppressor module
        pClientInfo->remaining_samples = output_size_in_samples;
      }
      else
      {
        pClientInfo->curr_buff_ptr = (int8_t *)&(pClientInfo->ps_curr_pcm_buff->nDataBuf);
      }
      pClientInfo->buf_size = pClientInfo->remaining_samples;
      afe_cont_buf_latency_log(pDevPort, pClientInfo);
    }
    else // (data_is_available == 0) //data is not available from client
    {
      underrun_flag = TRUE;
      if(NULL != pClientInfo->pop_suppressor_module_ptr)
      {

        //following function would set the ramp down condition in case  first underrun is detected. for other underruns the
        //the underrun flag would be true and hence normal flow would go through

        afe_pop_suppressor_process(dummy_src_ptr, &out_ptr, pClientInfo->remaining_samples,
                                   &output_size_in_samples, pClientInfo->pop_suppressor_module_ptr,
                                   dummy_flag1, &underrun_flag, DATA_UNAVAILABLE);
        // underrun flag will be set to 0 by module if 1 ms samples are to be drained out
        // in that case pClientInfo->curr_buff_ptr  and pClientInfo->remaining_samples needs to be updated
        if(!underrun_flag)
        {
          pClientInfo->curr_buff_ptr = out_ptr;
          pClientInfo->remaining_samples = output_size_in_samples;
        }
      }

      if(underrun_flag)
      {
        afe_port_print_underrun_msg(pClientInfo, pDevPort);
        return ADSP_EFAILED;
      }
    }
  } //(pClientInfo->remaining_samples == 0)

  /* Mix the rx-client samples. */
  nSamplesOut = pDevPort->int_samples_per_period;
  *nSamplesOutActual = nSamplesOut;
  nClientChannels = pClientInfo->afe_client.channel;
  *nSamplesIn = pClientInfo->int_samples_per_period;

  /* For partially filled buffer, in case of EOS, we would hit this condition.*/
  if((*nSamplesIn) > pClientInfo->remaining_samples)
  {
    (*nSamplesIn) = pClientInfo->remaining_samples;

    /* If sampling rate of client data is same as port, then, output samples is same as input samples.
     * otherwise nSamplesOutActual would get updated in the resampler call */
    *nSamplesOutActual = (*nSamplesIn);

    MSG_4(MSG_SSID_QDSP6,DBG_ERROR_PRIO,
          "Partial buffer sent by cl_id: 0x%lX, sent samples %ld, expected samples %lu, intf: 0x%x",
          pClientInfo->client_id, *nSamplesIn, pClientInfo->int_samples_per_period, pDevPort->intf);
  }

  /*
   * Client data: 4 channels, 3 buf each. where each literal is a buf. (buf deinterleaved)
   * L1 L1 L1  L2 L2 L2    R1 R1 R1  R2 R2 R2
   *
   * Port data: (buffer interleaved)
   * L1 L2 R1 R2  L1 L2 R1 R2  L1 L2 R1 R2  L1 L2 R1 R2
   */
  *client_data_ptr = pClientInfo->curr_buff_ptr;
  *cl_ch_spacing_ptr = pClientInfo->buf_size;   //spacing between adj channels in curr_buff_ptr

  return ADSP_EOK;
}

/**
 * gets the bufs from queue and rate matches between client and port.
 * since media format can change the values of bytes_per_channel, client channels, samples in,
 * they are part of this function.
 */
ADSPResult afe_port_get_buf_from_client_queue_rate_match(afe_client_info_t *pClientInfo,
                                                         afe_dev_port_t *pDevPort, int8_t *pDestBuf,
                                                         int8_t **client_data_ptr,
                                                         uint32_t *cl_ch_spacing_ptr)
{
  uint32_t unChannelStatus;
  uint32_t dataQ_chan_bit;
  uint32_t nSamplesIn;
  elite_msg_any_t myDataQMsg;
  ADSPResult result;
  qurt_elite_queue_t * client_dataQ;
  qurt_elite_channel_t *dataQ_chan;
  uint32_t num_samples_to_read;
  uint32_t client_bytes_per_channel;
  uint32_t nClientChannels;
  afe_rate_match_info_t *rate_match_info_ptr = NULL;
  int8_t *rate_match_buf_ptr = NULL;
  int32_t client_port_drift_samples = 0;
  uint16_t read_no_buf = 0;

  *client_data_ptr = NULL;
  num_samples_to_read = pClientInfo->int_samples_per_period;
  client_bytes_per_channel = pClientInfo->afe_client.bytes_per_channel;
  nClientChannels = pClientInfo->afe_client.channel;
  rate_match_info_ptr = pClientInfo->rate_match_info_ptr;
  rate_match_buf_ptr = rate_match_info_ptr->data_buf_ptr;
  nSamplesIn = pClientInfo->int_samples_per_period;

  /** correct only if rate matching is enabled and sufficient buffers have been set*/
  if(rate_match_info_ptr->num_buffers_released >= (AFE_NUM_BUFS_RELEASE_B4_SS)
      && rate_match_info_ptr->num_buffers_collected >= (rate_match_info_ptr->num_buff_to_collect))
  {
    //correct drift everytime by 1 sample if any.
    afe_port_client_get_drift_to_correct(pClientInfo, pDevPort, &client_port_drift_samples);
    //client_port_drift_samples -> +ve =>client is faster (slip = read more samples & drop some). -ve -> stuff = read less samples and add some
    num_samples_to_read = pClientInfo->int_samples_per_period + client_port_drift_samples;
  }

  //channel spacing of the rate match buffer is greater of the two.
  *cl_ch_spacing_ptr = (num_samples_to_read > nSamplesIn) ?
      num_samples_to_read : nSamplesIn;
  uint32_t to_read_var = num_samples_to_read;             //how much to read from client buf.
  uint32_t did_read_var = 0;                               //how much is already read from client buf.
  uint32_t min_samples, temp, diff = 0, underrun_reason = 0; //temporary variables. underrun_reason is for debugging.

  /** pop queue until we have enough samples to read.
   * we may read samples from 1 or 2 buffers.
   * conditions for leaving this infinite loop:
   *   -we read enough samples
   *   -or an underrun happens
   */
  while(TRUE)
  {
    // read any samples present in the current buffer & copy it to rate match buf.
    // if current buf has more samples, read only what's necessary.
    min_samples = ((uint32_t)pClientInfo->remaining_samples < to_read_var) ?
        (uint32_t)pClientInfo->remaining_samples : to_read_var;

    //read and copy to rate match buf
    if(min_samples > 0)
    {
      //if we need to read at the boundary of buffers, then 2 dataQ elements need to be held. Also buff may not be contiguous.
      //hence we need to copy to a local buffer & release client buffer.
      afe_port_cpy_client_to_rate_match_buf(rate_match_buf_ptr, pClientInfo->curr_buff_ptr,
                                            pClientInfo->afe_client.interleaved,
                                            min_samples * client_bytes_per_channel,
                                            pClientInfo->buf_size * client_bytes_per_channel,
                                            nClientChannels,
                                            (*cl_ch_spacing_ptr) * client_bytes_per_channel,
                                            rate_match_info_ptr->data_buf_size);

      if(pClientInfo->afe_client.interleaved)
      {
        temp = min_samples * client_bytes_per_channel * nClientChannels;
        pClientInfo->curr_buff_ptr += temp;
        rate_match_buf_ptr += temp;
      }
      else
      {
        temp = min_samples * client_bytes_per_channel;
        pClientInfo->curr_buff_ptr += temp;
        rate_match_buf_ptr += temp;
      }

      pClientInfo->remaining_samples -= min_samples;

      //push the client buf back to queue
      if(pClientInfo->remaining_samples == 0)
      {
        afe_port_return_buf_to_return_q(pClientInfo);
      }

      did_read_var += min_samples;
      to_read_var -= min_samples;
    }

    //break if we have read enough samples.
    if(0 == to_read_var)
    {
      break;
    }

    client_dataQ = pClientInfo->this_client_manager.dataQ;
    dataQ_chan = qurt_elite_queue_get_channel(client_dataQ);
    dataQ_chan_bit = qurt_elite_queue_get_channel_bit(client_dataQ);
    read_no_buf = 0;

    /** Time for new samples from Client's dataQueue
     *  pop dataQ until it's empty or until we have enough data buffers.
     *  loop exit conditions:
     *   -no more buffers in the queue
     *   -errors in popping
     *   -we have enough buffers
     */
    while((AFE_CLIENT_ENABLE == pClientInfo->state)
        && (0 != (unChannelStatus = qurt_elite_channel_poll(dataQ_chan, dataQ_chan_bit))))
    {
      result = qurt_elite_queue_pop_front(client_dataQ, (uint64_t*)&myDataQMsg);

      if(ADSP_FAILED(result))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failure on mq %d qurt_elite_queue_pop_front: %d\n", (int)(client_dataQ), result);
        break;
      }

      if(ELITE_DATA_BUFFER == myDataQMsg.unOpCode)
      {
        //drop at least 1 buffer in the beginning for the tx port (client).
        if(rate_match_info_ptr->num_buffers_released < AFE_NUM_BUFS_RELEASE_B4_SS)
        {
          rate_match_info_ptr->num_buffers_released++;
          pClientInfo->ps_curr_pcm_buff = (elite_msg_data_buffer_t*)myDataQMsg.pPayload;
          afe_port_return_buf_to_return_q(pClientInfo);
          memset(rate_match_buf_ptr, 0, nSamplesIn * client_bytes_per_channel * nClientChannels);
          underrun_reason = 1;
          read_no_buf = 1; //indication to not read any buffers
        }
        //collect at least 1 buffer in the beginning
        else if(rate_match_info_ptr->num_buffers_collected < rate_match_info_ptr->num_buff_to_collect)
        {
          rate_match_info_ptr->num_buffers_collected++;
          rate_match_info_ptr->pcm_bufs[(rate_match_info_ptr->curr_pcm_buf_wr_ind)] =
              (elite_msg_data_buffer_t*)myDataQMsg.pPayload;
          if((++rate_match_info_ptr->curr_pcm_buf_wr_ind) >= AFE_MAX_PCM_BUFS)
          {
            rate_match_info_ptr->curr_pcm_buf_wr_ind = 0;
          }
          memset(rate_match_buf_ptr, 0, nSamplesIn * client_bytes_per_channel * nClientChannels);
          underrun_reason = 2;
          read_no_buf = 1; //indication to not read any buffers
          continue;
        }
        //use the buffers in a circular order.
        else
        {
          read_no_buf = 0;
          //copy the new pcm buf into our circ array.
          rate_match_info_ptr->pcm_bufs[(rate_match_info_ptr->curr_pcm_buf_wr_ind)] =
              (elite_msg_data_buffer_t*)myDataQMsg.pPayload;
          if((++rate_match_info_ptr->curr_pcm_buf_wr_ind) >= AFE_MAX_PCM_BUFS)
          {
            rate_match_info_ptr->curr_pcm_buf_wr_ind = 0;
          }
          diff = (rate_match_info_ptr->curr_pcm_buf_wr_ind) - (rate_match_info_ptr->curr_pcm_buf_rd_ind);
          //at least have 2 buffers in the circ array.
          if((diff & (AFE_MAX_PCM_BUFS - 1)) >= 2)
          {
            break;    //we have enough buffers
          }
          else
          {
            continue; //see if we can get some more buffers, to keep our internal circ buf array with 2 bufs.
          }
        }
      }
      else
      {
        //handle data cmd, & flush all the buffers held.
        afe_port_data_cmd(pClientInfo, pDevPort, &myDataQMsg, FALSE);
        //let's break & say underrun when media type comes, because sampling rate etc are different after a data cmd.
        //rate matching would be diff, too.
        //return any partial buffers being held
        if(pClientInfo->remaining_samples != 0)
        {
          afe_port_return_buf_to_return_q(pClientInfo);
        }
        //indication to not read any buffers. data_cmd flushes the buffers held.
        read_no_buf = 1;
        break;
      }
    }        //while (channel status)

    diff = (rate_match_info_ptr->curr_pcm_buf_wr_ind) - (rate_match_info_ptr->curr_pcm_buf_rd_ind);

    /* If data is available from client or in the internal buffers & not a data cmd (that could change sampling rate etc)
     * or if we shouldn't read */
    if((read_no_buf != 1) && ((diff & (AFE_MAX_PCM_BUFS - 1)) > 0))
    {
      pClientInfo->ps_curr_pcm_buff =
          rate_match_info_ptr->pcm_bufs[rate_match_info_ptr->curr_pcm_buf_rd_ind];

      //MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: RX obtained buf: %X, diff = %d", (uint32_t) pClientInfo->ps_curr_pcm_buff, diff&(AFE_MAX_PCM_BUFS-1));

      //make null after using so that non-NULL serves as a marker for releasing the buffers
      rate_match_info_ptr->pcm_bufs[(rate_match_info_ptr->curr_pcm_buf_rd_ind)] = NULL;
      if((++rate_match_info_ptr->curr_pcm_buf_rd_ind) >= AFE_MAX_PCM_BUFS)
      {
        rate_match_info_ptr->curr_pcm_buf_rd_ind = 0;
      }
      //retrieve the information about the data buffer.
      pClientInfo->remaining_samples = (((pClientInfo->ps_curr_pcm_buff)->nActualSize)
          / (pClientInfo->afe_client.bytes_per_channel * nClientChannels));
      pClientInfo->curr_buff_ptr = (int8_t *)&(pClientInfo->ps_curr_pcm_buff->nDataBuf);
      pClientInfo->buf_size = pClientInfo->remaining_samples;

      afe_cont_buf_latency_log(pDevPort, pClientInfo);

      continue;
    }
    else // data is not available from client
    {
      //print underrun immediately, because it shouldn't happen due to the initial sync through num_buffers_collected, num_buffers_released
      uint64_t currTime, diff;
      pClientInfo->under_run_counter++;

      // Use the wall clk time just to avoid the flood of under-run messages
      currTime = qurt_elite_timer_get_time();

      diff = currTime - pClientInfo->prev_time;

      if((diff >= AFE_UNDERRUN_TIME_THRESH) || (0 != underrun_reason))
      {
        MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "port_id: 0x%lX, client_id: 0x%lX, rate_match_underrun by: %lu, to_read: %lu, did_read: %lu, reason: %lu",
              pDevPort->intf, pClientInfo->client_id,
              to_read_var, num_samples_to_read,
              did_read_var, underrun_reason);

        pClientInfo->prev_time = currTime;
        pClientInfo->under_run_counter = 0;
      }

      /** write zeros to the rest of the buffer if already did read something*/
      if(did_read_var > 0)
      {
        afe_port_cpy_client_to_rate_match_buf(rate_match_buf_ptr,
                                              NULL,
                                              pClientInfo->afe_client.interleaved,
                                              to_read_var * client_bytes_per_channel,
                                              pClientInfo->buf_size * client_bytes_per_channel,
                                              nClientChannels,
                                              (*cl_ch_spacing_ptr) * client_bytes_per_channel,
                                              rate_match_info_ptr->data_buf_size);

        if(pClientInfo->afe_client.interleaved)
        {
          rate_match_buf_ptr += to_read_var * client_bytes_per_channel * nClientChannels;
        }
        else
        {
          rate_match_buf_ptr += to_read_var * client_bytes_per_channel;
        }
        did_read_var += to_read_var;
        to_read_var = 0;
        break;
      }
      else
      {
        return ADSP_EFAILED;
      }
    }
  } //while(TRUE)

  //client_port_drift_samples +ve, samples will be slip, and when negative -> stuff.
  //input channel spacing is channelSpacing, but output is nSamplesIn.
  afe_port_client_sample_slip_stuff(rate_match_info_ptr, did_read_var,
                                    pClientInfo->afe_client.interleaved, client_bytes_per_channel,
                                    nClientChannels, client_port_drift_samples, nSamplesIn);

  /*
   * Client data: 4 channels, 3 buf each. where each literal is a buf. (buf deinterleaved)
   * L1 L1 L1  L2 L2 L2    R1 R1 R1  R2 R2 R2
   *
   * Port data: (buffer interleaved)
   * L1 L2 R1 R2  L1 L2 R1 R2  L1 L2 R1 R2  L1 L2 R1 R2
   */
  *client_data_ptr = rate_match_info_ptr->data_buf_ptr;
  *cl_ch_spacing_ptr = nSamplesIn; //whether slip or stuff, channel spacing in rate match buf is nSamplesIn ultimately.

  return ADSP_EOK;
}

/**
  This function returns buffers in the dataQ,if any.

  @param[in] pDataQ     Pointer to the data queue descriptor.

  @return
  None.

  @dependencies
  None.
 */
static void afe_return_pending_buffers (qurt_elite_queue_t* pDataQ)
{
  elite_msg_any_t dataQMsg;
  ADSPResult result;

  // clean up data queue
  if(pDataQ)
  {
    // Drain any queued buffers
    do
    {
      // Non-blocking MQ receive
      result = qurt_elite_queue_pop_front(pDataQ, (uint64_t*)&dataQMsg);

      // return the buffer to its rightful q.
      if(ADSP_EOK == result)
      {
        elite_msg_return_payload_buffer(&dataQMsg);
        MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"pop suppressor returning pending Buffers!!");
      }

    } while(ADSP_EOK == result);

  }

  return;
}

/**
 * This function is processing the client samples on the Rx path.
 *
 * @param[in] pClientInfo, client info for that port
 * @param[in] client_data_pptr, ptr to ptr of the client data buff
 * @param[in] pDevPort, pre to port
 * @param[in] cl_ch_spacing_ptr, ptr to cl ch spacing in samples
 * @return none
 */
static void afe_port_process_rx_client_samples(afe_client_info_t *pClientInfo,
                                               int8_t **client_data_pptr, afe_dev_port_t *pDevPort,
                                               uint32_t *cl_ch_spacing_ptr,
                                               uint32_t *samples_out_actual_ptr,
                                               uint32_t nSamplesIn)
{
  uint32_t client_bit_width;
  uint32_t port_bit_width;
  uint32_t port_bytes_per_channel;
  uint32_t nSamplesOut;
  uint32_t nPortChannels;
  uint32_t out_size_in_bytes;
  uint32_t cl_ch_spacing_in_bytes;
  uint32_t min_channel;
  uint32_t nClientChannels;
  uint32_t client_bytes_per_channel;
  uint32_t nSize = 0;
  uint32_t numResampler;
  int8_t *resamp_out_buf_ptr;

  client_bit_width = pClientInfo->afe_client.bit_width;
  port_bit_width = pDevPort->bit_width;
  nClientChannels = pClientInfo->afe_client.channel;
  client_bytes_per_channel = pClientInfo->afe_client.bytes_per_channel;

  // MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port 0x%x: port bit width %d client_bit_width %ld",pDevPort->intf, port_bit_width, client_bit_width);

  port_bytes_per_channel = pDevPort->bytes_per_channel;
  nSamplesOut = pDevPort->int_samples_per_period;
  nPortChannels = pDevPort->channels;
  out_size_in_bytes = (*samples_out_actual_ptr) * port_bytes_per_channel;
  cl_ch_spacing_in_bytes = (*cl_ch_spacing_ptr) * client_bytes_per_channel;
  min_channel = s32_min_s32_s32(nClientChannels, nPortChannels);

  /* We didnt read any new data, thats why skip the following processing */
  if(RS_BUFF_FULL != pClientInfo->samp_rate_conv.rs_buff_flag)
  {
    /* Stereo to mono conv by averaging. Inplace conversion */
    if(nClientChannels == 2 && nPortChannels == 1)
    {
      afe_port_stereo2mono_by_avg(*client_data_pptr, (*client_data_pptr + cl_ch_spacing_in_bytes),
                                  *client_data_pptr, nSamplesIn, client_bytes_per_channel);
    }
    /* If port bytes per channel != client bytes per channel
     * if(client_bytes_per_ch > port_bytes_per_ch)
     * 24->16 bit inplace conversion on client channels;
     */

    if(client_bit_width > port_bit_width)
    {
      /* 32 to 16 bit converion, in place covnersion while maintaining the actual client
       * channel spacing
       * Effective output channel spacing doubles, since the PCM converted to 16-bit
       */
      afe_port_sample_up_down_convertor(
          pDevPort, *client_data_pptr, *client_data_pptr, nSamplesIn, min_channel, (*cl_ch_spacing_ptr),
          (client_bytes_per_channel / port_bytes_per_channel) * (*cl_ch_spacing_ptr),
          pClientInfo->afe_client.bit_width, BYTE_DOWN_CONV);

      /* Update the effective client channel spacing for 16-bit PCM
       */
      (*cl_ch_spacing_ptr) = (client_bytes_per_channel / port_bytes_per_channel) * (*cl_ch_spacing_ptr);
    }
  }

  /* If the port and client sampling rates are different */
  if(pDevPort->sample_rate != pClientInfo->afe_client.sample_rate)
  {
    /* Resampler is non-inplace module. Setting the scratch buffer as output
     */
    resamp_out_buf_ptr = pDevPort->scratch_buf;
    (*cl_ch_spacing_ptr) = nSamplesOut;

    /* Total number of Resamplers = min(port_channel, client_channel)
     * Resampler bytes_per_sample = min(port bytes per sample, client bytes per sample)
     */
    numResampler = pClientInfo->unNumResamplers;

    pClientInfo->samp_rate_conv.in_ch_spacing_in_bytes = cl_ch_spacing_in_bytes;
    pClientInfo->samp_rate_conv.out_ch_spacing_in_bytes = out_size_in_bytes;

    /* numResampler would be used for voice resampler.
     * num channels are not needed for fir capiv2 resampler. they are observed into capiv2
     */
    sample_rate_conv_process(&pClientInfo->samp_rate_conv, (int8 *)(*client_data_pptr),
                             (int8 *)resamp_out_buf_ptr, &nSize, numResampler);

    /*  nSize is number of samples generated by resampler */
    *samples_out_actual_ptr =  nSize;

    //now, mem_start_addr_copy is deinterleaved with numChan = nClientChannels
    *client_data_pptr = resamp_out_buf_ptr;
    cl_ch_spacing_in_bytes = out_size_in_bytes; //spacing between adj channels on client_data_ptr.

    /* For 16-> 32 bit conversion, re-sampled port channels are spaced as per port bytes per sample
     * Re-sampler is applied on 16-bit PCM, so effective port channel spacing in terms of number of
     * samples in the resampler output buffer is doubled.
     *
     * 16->32 bit conversion is applied below. 16-bit PCM spaced as per 4 bytes per sample
     * Channel spacing in bytes still remains the same.
     */

    if(client_bytes_per_channel < port_bytes_per_channel)
    {
      (*cl_ch_spacing_ptr) = nSamplesOut * (port_bytes_per_channel / client_bytes_per_channel);
    }
    else
    {
      (*cl_ch_spacing_ptr) = nSamplesOut;
    }
  }
  else
  {
    /* Down stream modules expect the data in Scratch buffer
     In resampling case, processed data is present in scratch buffer already
     In other cases, we need to copy the data to scratch buffer from client buffer */

    int8_t *src_ptr = (int8_t*)*client_data_pptr;
    int8_t *dst_ptr = (int8_t*)pDevPort->scratch_buf;
    uint32_t src_size;

    /* bit width up conversion happens after this, so in that case
        data is still in client format */
    if(client_bytes_per_channel < port_bytes_per_channel)
    {
      src_size = client_bytes_per_channel * nSamplesIn;
    }
    else
    {
      src_size = port_bytes_per_channel * nSamplesIn;
    }

    for(uint32_t chan = 0; chan < min_channel; chan++)
    {
      memscpy(dst_ptr, pDevPort->working_buf_size, src_ptr, src_size);

      src_ptr += cl_ch_spacing_in_bytes;
      dst_ptr += src_size;
    }
    *client_data_pptr = pDevPort->scratch_buf;

    /* Now the channel spacing is number of samples */
    *cl_ch_spacing_ptr = nSamplesIn;
  }

  return;
}

/**
 * This function is for client data acquisition. It goes through each client
 * and gathers PCM samples for each DMA.
 *
 * Assumes data in afe_dev_port_t.mem_start_addr32  is sample deinterleaved
 * result goes in afe_dev_port_t.mem_start_addr32
 * corrupts afe_dev_port_t.mem_start_addr_copy
 *
 * @param[in] pClientInfo, client info for that port
 * @param[in] pDevPort, handles of the port
 * @param[in] pDestBuf, destination buffer 32 bit
 * @param[in] client_counter, each time this function is called for a client,
 *             this should be incremented. Helps in optimization. starts from 0.
 * @return none
 */
static void afe_port_get_client_samples_and_add(afe_client_info_t *pClientInfo,
                                                afe_dev_port_t *pDevPort, int8_t *pDestBuf,
                                                int8_t *pScratchBuf, uint16_t client_counter)
{

  /* If the client is disabled, omit this client.    */
  if(AFE_CLIENT_ENABLE != pClientInfo->state)
  {
    return;
  }

  ADSPResult result;
  uint32_t nClientChannels;
  uint32_t nPortChannels;
  uint32_t cl_ch_spacing;
  uint32_t min_channel;
  uint32_t nSamplesIn;
  uint32_t nSamplesOut;
  uint32_t nSamplesOutActual;
  int8_t *client_data_ptr = NULL;
  afe_rate_match_info_t *rate_match_info_ptr = NULL;
  int8_t* dummy_src_ptr = NULL;
  uint32_t output_size_in_samples;
  bool_t return_client_buffer = TRUE;
  uint32_t client_bit_width;
  uint32_t port_bit_width;
  uint32_t i = 0;
  resamp_buff_flag_t curr_rs_buff_flag;

  nSamplesOut = pDevPort->int_samples_per_period;
  nSamplesOutActual = pDevPort->int_samples_per_period;
  nClientChannels = pClientInfo->afe_client.channel;
  nPortChannels = pDevPort->channels;
  client_bit_width = pClientInfo->afe_client.bit_width;
  cl_ch_spacing = pClientInfo->int_samples_per_period;
  port_bit_width = pDevPort->bit_width;
  min_channel = s32_min_s32_s32(nClientChannels, nPortChannels);
  nSamplesIn = pClientInfo->int_samples_per_period;

  /* This flag is set if resampler is active with fractional rate or it is set to no buffering.*/
  curr_rs_buff_flag = pClientInfo->samp_rate_conv.rs_buff_flag;

  //is_avt_drift_updated is true only every 100ms
  if(TRUE == pDevPort->avt_drift_params.is_avt_drift_updated)
  {
    //send drift info to the client if it's subscribed
    afe_port_send_avt_drift_update(pDevPort, pClientInfo); //TODO: to move this to end of DMA processing if it consumes lot of time.
  }
  rate_match_info_ptr = pClientInfo->rate_match_info_ptr;

  /* Just Collect the stats for drift correction
   * is_avt_drift_updated is true only every 100ms  */
  if((NULL != rate_match_info_ptr) && (TRUE == pDevPort->avt_drift_params.is_avt_drift_updated))
  {
    //calculate drift between the client and this port if rate matching is necessary: loopback.
    afe_port_client_calc_drift(pClientInfo, pDevPort);
  }

  /* This is when no buffering is needed in Resampler */
  if(RS_NORMAL == curr_rs_buff_flag)
  {
    /* No need to read any new samples from the client buffer */

    if(NULL != rate_match_info_ptr)
    {
      result = afe_port_get_buf_from_client_queue_rate_match(pClientInfo, pDevPort, pDestBuf,
                                                             &client_data_ptr, &cl_ch_spacing);
    }
    else
    {
      result = afe_port_get_buf_from_client_queue(pClientInfo, pDevPort, pDestBuf, &client_data_ptr,
                                                  &cl_ch_spacing, &nSamplesOutActual, &nSamplesIn);
    }

    //in underrun cases error is returned from above functions.
    if(ADSP_FAILED(result))
    {
      return;
    }

    afe_port_process_rx_client_samples(pClientInfo, &client_data_ptr, pDevPort, &cl_ch_spacing,
                                       &nSamplesOutActual, nSamplesIn);
  }
  /* This is when buffering up is needed in Resampler and the internal RS buffer is full, thats why skipping
   * reading new data */
  else if(RS_BUFF_FULL == curr_rs_buff_flag)
  {
    /* Dont read any new buffer from the client, just process the existing buffer */
    afe_port_process_rx_client_samples(pClientInfo, &client_data_ptr, pDevPort, &cl_ch_spacing,
                                       &nSamplesOutActual, nSamplesIn);
  }
  /* This is when buffering down is needed in Resampler and the internal RS buffer is empty, thats why we need to read
   * two buffers as one will be rendered and one will be saved as buffer for next interrupts */
  else if(RS_BUFF_EMPTY == curr_rs_buff_flag)
  {
    for(i = 0; i < 2; i++)
    {
      if(NULL != rate_match_info_ptr)
      {
        result = afe_port_get_buf_from_client_queue_rate_match(pClientInfo, pDevPort, pDestBuf,
                                                               &client_data_ptr, &cl_ch_spacing);
      }
      else
      {
        result = afe_port_get_buf_from_client_queue(pClientInfo, pDevPort, pDestBuf, &client_data_ptr,
                                                    &cl_ch_spacing, &nSamplesOutActual, &nSamplesIn);
      }

      //in underrun cases error is returned from above functions.
      if(ADSP_FAILED(result))
      {
        return;
      }

      afe_port_process_rx_client_samples(pClientInfo, &client_data_ptr, pDevPort, &cl_ch_spacing,
                                         &nSamplesOutActual, nSamplesIn);

      /* Since, when fractional resampling is used, we can safely return the client buffer when it is used up,
       * as resampler output buffer will be used for subsequent processing. */
      afe_port_update_non_rate_match_client_buf(pClientInfo, return_client_buffer, nSamplesIn);
    }
  }

  // Ramp down process should be called after resampler inorder to account for delay introduced by resampler
  // hence the module now operates at port sampling rate
  // this function would do the actual ramp down based on condition of underrun or disconnect.
  if(NULL != pClientInfo->pop_suppressor_module_ptr)
  {
    afe_pop_suppressor_process(
        dummy_src_ptr,
        &client_data_ptr,                        //inplace operation on client_data_ptr
        pClientInfo->remaining_samples, &output_size_in_samples,
        pClientInfo->pop_suppressor_module_ptr, pClientInfo->client_disconnect_marker_flag,
        &return_client_buffer, RAMP_DOWN_PROCESS);

    pClientInfo->remaining_samples = output_size_in_samples;

    // incase RAMP down due to underrun, resampler memory needs to be cleared to avoid state data
    // when underrun ceases
    if((pClientInfo->afe_client.sample_rate != pDevPort->sample_rate)
        && (FALSE == return_client_buffer))
    {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"pop suppressor clearing resampler memory ");
      sample_rate_conv_memory_init(&pClientInfo->samp_rate_conv, 1 /*unNumResamplers*/);
    }
    if(pClientInfo->client_disconnect_marker_flag)
    {
      //Make sure after rampdown there are no buffers in the BufferQ for this client.
      afe_return_pending_buffers(pClientInfo->this_client_manager.dataQ);
    }
    pClientInfo->client_disconnect_marker_flag = FALSE;
  }

  if(client_bit_width < port_bit_width)
  {
    /* Up-convert client channels or resampled channels in port buffer to 32 bits
     */
    afe_port_sample_up_down_convertor(pDevPort, client_data_ptr, client_data_ptr, nSamplesOutActual, min_channel,
                                      cl_ch_spacing, nSamplesOut, pClientInfo->afe_client.bit_width,
                                      BYTE_UP_CONV);

    cl_ch_spacing = nSamplesOut;
  }
  /* Deinterleaved (and resampled, if applicable) data in buffer pointed to by client_data_ptr */

  /* Accumulate the client samples in port buffer
   * Port accumulator buffer is 32-bits for both 16/24-bits inputs
   * Port accumulator buffer is 64 bits for 32 bit inputs
   */

  if(2 == pDevPort->bytes_per_channel)
  {
    afe_port_accumulate_client_samples(client_counter, nSamplesOut, nSamplesOutActual, (int32_t *)pDestBuf,
                                       (int16_t *)client_data_ptr, cl_ch_spacing, nPortChannels,
                                       nClientChannels);
  }
  else
  {
    if (AFE_BITS_PER_SAMPLE_24 == pDevPort->bit_width)
    {
      afe_port_accumulate_client_samples_32(client_counter, nSamplesOut, nSamplesOutActual, (int32_t *)pDestBuf,
                                            (int32_t *)client_data_ptr, cl_ch_spacing, nPortChannels,
                                            nClientChannels);
    }
    else
    {
      afe_port_accumulate_client_samples_64(client_counter, nSamplesOut, nSamplesOutActual, (int64_t *)pDestBuf,
                                            (int32_t *)client_data_ptr, cl_ch_spacing, nPortChannels,
                                            nClientChannels);
    }
  }
  //mem_start_addr is deinterleaved

  //update client buffer etc only when no rate matching is done, because, rate matching takes care of these.
  if(RS_NORMAL == curr_rs_buff_flag)
  {
    afe_port_update_non_rate_match_client_buf(pClientInfo, return_client_buffer, nSamplesIn);
  }
}

/**
 * This function is for accumulating client samples in device
 * port buffer
 *
 * @param[in] client_counter, counter for the clients of a device
 *       port
 * @param[in] nSamplesOut, total number of output sampler for
 *       port channels
 * @param[out] curr_ptr_dev_port_buf, pointer to device port
 *       buffer for accumulating all the client samples.
 *       Address pointed to by this buffer should be 8 byte
 *       aligned for vector operations.
 * @param[in] curr_ptr_client_buf, pointer to client buffer to be
 *       accumulated in device port buffer
 *       Address pointed to by this buffer should be min 4 bytes
 *       aligned for vector operations
 * @param[in] nChanSpacing, spacing between client channels.
 * #param[in] nPortChannels, number of port channels.
 * #param[in] nClientChannels, number of client channels.
 */
static void afe_port_accumulate_client_samples(int16_t client_counter, int32_t nSamplesOut, int32_t nSamplesOutActual,
                                               int32_t* curr_ptr_dev_port_buf, int16_t* curr_ptr_client_buf,
                                               int32_t nChanSpacing, int32_t nPortChannels, int32_t nClientChannels)
{
#ifdef __qdsp6__
  int32_t sampIndex;
  int64_t src2, src3;
  int64_t *dest_ptr;   /* Pointer should point to 8 bytes aligned addres      */
  int32_t *src_ptr;    /* Pointer should point to min 4 bytes aligned address */
  int32_t chan;

  int32_t minChannel;           /* min of port and client channel          */

  /* If the number of client channel == 1, and nPortChannels > nClientChannels
      then max stereo conversion and rest of the channels are set to 0
   */
  if(nClientChannels == 1)
  {
    minChannel = s32_min_s32_s32(nPortChannels, 2);
  }
  else /* Copy only the minimum of two channels to port buffers */
  {
    minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);
  }

  /* Below calculations are needed considering
      nSamplesOutActual could be non-multiple of 4
   */
  int32_t nSamplesOutActualBy4 = nSamplesOutActual >> 2;              /* (int)nSamplesOutActual/4 */
  int32_t nSamplesOutActualMod4 = nSamplesOutActual % 4;

  /* Remaining port samples, after the copy/accumulation operation */
  int32_t nSamplesOutActualRem = (nSamplesOut - nSamplesOutActual);

  int32_t nSamplesOutActualDiv4 = nSamplesOutActualBy4 << 2;          /* Maximum multiple of 4 length */

  /* Saving the pointer to client data buffer */
  int16_t * client_ptr = curr_ptr_client_buf;

  /* Remaining client samples, after the copy/accumulation operation */
  int32_t rem_client_samples = (nChanSpacing - nSamplesOutActual);

  /* Copy the samples from client buf to dev port buf channel-by-channel

       if (nPortChannels <= nClientChannels)
        Copy only nPortChannels from client buf to devPort buf

       if (nPortChannels > nClientChannels)
       if  nClientChannels == 1,
         then the client channel is repeated in all port channels
       else
         Copy only nClientChannels and rest of port channels are set to 0
   */

  for(chan = 0; chan < minChannel; chan++)
  {

    dest_ptr = (int64_t *)curr_ptr_dev_port_buf;
    src_ptr  = (int32_t *)curr_ptr_client_buf;

    /* For the first client, just copy the samples from client buf to port buf
       If client samples < one interrupt samples, zero out the remaining samples
     */
    if (client_counter == 0)
    {
      /* Unpack and sign extend two 16-bit samples from
           client buf in to two 32-bit samples and store in
           port buf.
           Loop unrolled to unpack four 16-bit samples in
           one iteration.
       */
      //If client buffer is aligned(port buffer always is)
      //Attempt Optimization
      if((0 == ((int32_t)curr_ptr_client_buf & AFE_ALIGNMENT_MASK_4_BYTE)) && (0 == ((int32_t)curr_ptr_dev_port_buf & AFE_ALIGNMENT_MASK_8_BYTE)))
      {
        for (sampIndex=0; sampIndex < nSamplesOutActualBy4; sampIndex++)
        {
          (*dest_ptr++) = Q6_P_vsxthw_R(*src_ptr++);
          (*dest_ptr++) = Q6_P_vsxthw_R(*src_ptr++);
        }

        /* Advance the pointers to first non-multiple of 4 location
                      in the client and port buffers
                      Required only if the frame size is non-multipe of 4
         */
        curr_ptr_dev_port_buf += nSamplesOutActualDiv4;
        curr_ptr_client_buf += nSamplesOutActualDiv4;

        /* If the frame size is non-multiple of 4, copy the remaining
           samples, one-by-one, max 3 samples
         */
        for(sampIndex=0; sampIndex < nSamplesOutActualMod4; sampIndex++)
        {
          (*curr_ptr_dev_port_buf++) = (int32_t)(*curr_ptr_client_buf++);
        }
      }
      //If client buffer not aligned (Buffer not in multiple of 4)
      //sxt samples one-by-one
      else
      {
        for(sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
        {
          (*curr_ptr_dev_port_buf++) = (int32_t)(*curr_ptr_client_buf++);
        }
      }
      /* If nSamplesOutActual < samples_per_interrupt, zero out the
           remaining destination port buf for the first client
            curr_ptr_dev_port_buf points to first remaining port sample
       */
    }
    else //For subsequent clients, accumulate client samples in port buf
    {
      /* Unpack and sign extend two 16-bit samples from client buf
           Accumulate two 32-bit client samples with port buf
           Unpack and accumulate 4-16 bit samples per loop iteration
       */
      //If client buffer is aligned(port buffer always is)
      //Attempt Optimization
      if((0 == ((int32_t)curr_ptr_client_buf & AFE_ALIGNMENT_MASK_4_BYTE)) && (0 == ((int32_t)curr_ptr_dev_port_buf & AFE_ALIGNMENT_MASK_8_BYTE)))
      {
        for (sampIndex=0; sampIndex < nSamplesOutActualBy4; sampIndex++)
        {
          src2 = Q6_P_vsxthw_R(*src_ptr++);
          *dest_ptr = Q6_P_vaddw_PP(*dest_ptr, src2);
          dest_ptr++;

          src3 = Q6_P_vsxthw_R(*src_ptr++);
          *dest_ptr = Q6_P_vaddw_PP(*dest_ptr, src3);
          dest_ptr++;
        }

        /* Advance the pointers to first non-multiple of 4 location
                      in the client and port buffers
                      Required only if the frame size is non-multipe of 4
         */
        curr_ptr_dev_port_buf += nSamplesOutActualDiv4;
        curr_ptr_client_buf += nSamplesOutActualDiv4;

        /* If the frame size is non-multiple of 4, accumulate the remaining
           samples, one-by-one.
         */
        for(sampIndex=0; sampIndex < nSamplesOutActualMod4; sampIndex++)
        {
          (*curr_ptr_dev_port_buf++) += (int32_t)(*curr_ptr_client_buf++);
        }
      }
      //If client buffer not aligned (Buffer not in multiple of 4)
      //sxt samples one-by-one
      else
      {
        for(sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
        {
          (*curr_ptr_dev_port_buf++) += (int32_t)(*curr_ptr_client_buf++);
        }
      }
    }

    /* Till this point, curr_ptr_dev_port_buf & curr_ptr_client_buf
       pointeds have been incremented by nSamplesOutActual.
     */

    /* In devPort buffers, channels are spaced by nSamplesOut samples
       To point to next channel
       increment curr_ptr_dev_port_buf by nSamplesOutActualRem
     */

    curr_ptr_dev_port_buf += nSamplesOutActualRem;

    if(nClientChannels > 1)
    {
      /* In client buffers, channels  are spaced by nChanSpacing
          (>= nSamplesOut) samples
          To point to next channel
          inncrement curr_ptr_client_buf by remClientSamples
       */
      curr_ptr_client_buf += rem_client_samples;
    }
    else
    {
      /* If number of client channel is 1, repeat the same channel */
      curr_ptr_client_buf = client_ptr;
    }
  } /* for loop */


#else
  /*------------- Un-Optimized Non-q6 Version --------------------------*/

  int32_t rem_client_samples = (nChanSpacing - nSamplesOutActual);
  int32_t rem_port_samples = (nSamplesOut - nSamplesOutActual);

  int16_t * client_ptr = curr_ptr_client_buf;

  int32_t minChannel;

  /* If the number of client channel == 1, and nPortChannels > nClientChannels
      then max conversion upto stereo and zero out remaining channels
   */
  if(nClientChannels == 1)
  {
    minChannel = s32_min_s32_s32(nPortChannels, 2);
  }
  else /* Copy only the minimum of two channels to port buffers */
  {
    minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);
  }

  for(chan = 0; chan < minChannel; chan++)
  {
    if (client_counter == 0)
    {
      for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
      {
        (*curr_ptr_dev_port_buf++) = (int32_t)(*curr_ptr_client_buf++) ;
      }
      for ( ; sampIndex < nSamplesOut; sampIndex++) //make the rest zero
      {
        (*curr_ptr_dev_port_buf++) = 0 ;
      }
    }
    else //accumulate client samples in port buf
    {
      for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
      {
        (*curr_ptr_dev_port_buf++) += (int32_t)(*curr_ptr_client_buf++) ;
      }
      curr_ptr_dev_port_buf += rem_port_samples;
    }

    if(nClientChannels > 1)
    {
      curr_ptr_client_buf += rem_client_samples;
    }
    else
    {
      curr_ptr_client_buf = client_ptr;
    }
  }

#endif /* #ifdef __qdsp6__ */
}

static void afe_port_accumulate_client_samples_32(int16_t client_counter, int32_t nSamplesOut, int32_t nSamplesOutActual,
                                                  int32_t* curr_ptr_dev_port_buf, int32_t* curr_ptr_client_buf,
                                                  int32_t nChanSpacing, int32_t nPortChannels, int32_t nClientChannels)
{
  int32_t     sampIndex, chan;
  int32_t     rem_client_samples = (nChanSpacing - nSamplesOutActual);
  int32_t     rem_port_samples = (nSamplesOut - nSamplesOutActual);
  int32_t     *client_ptr = curr_ptr_client_buf;
  int32_t     minChannel;
  int64_t     *port_but_ptr, *client_buf_ptr;

  /* If the number of client channel == 1, and nPortChannels > nClientChannels
      then max conversion upto stereo and zero out remaining channels
   */
  if(nClientChannels == 1)
  {
    minChannel = s32_min_s32_s32(nPortChannels, 2);
  }
  else /* Copy only the minimum of two channels to port buffers */
  {
    minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);
  }

  /* For the first client, just copy the samples from client buf to port buf
      If client samples < one interrupt samples, zero out the remaining samples
   */
  for(chan = 0; chan < minChannel; chan++)
  {
    if (client_counter == 0)
    {
      /* Copy the actual number of client samples  */
      memscpy(curr_ptr_dev_port_buf,
              nSamplesOut * sizeof(int32_t),
              curr_ptr_client_buf,
              sizeof(int32_t) * nSamplesOutActual);

      /* Increment the port buffer pointer by 1 channel size  */
      curr_ptr_dev_port_buf += nSamplesOut;

      /* Increment the client buffer pointer by 1 channel size
            only if num client channel > 1, else repeat same channel   */
      if(nClientChannels > 1)
      {
        curr_ptr_client_buf += nChanSpacing;
      }
    }
    else //accumulate client samples in port buf
    {

#ifdef __qdsp6__

      /*In case of odd samples or buffer pointers are not 8byte aligned */

      if ((nSamplesOutActual & 1L) || ((int32_t)curr_ptr_dev_port_buf & 0x7)  || ((int32_t)curr_ptr_client_buf & 0x7))
      {
        for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
        {
          (*curr_ptr_dev_port_buf++) += (*curr_ptr_client_buf++) ;
        }
      }
      else
      {
        port_but_ptr = (int64_t *)curr_ptr_dev_port_buf;
        client_buf_ptr = (int64_t *)curr_ptr_client_buf;

        /* Accumulate 2 32-bit samples per loop iteration */
        for(sampIndex = nSamplesOutActual; sampIndex >= 2; sampIndex -= 2)
        {
          *port_but_ptr = Q6_P_vaddw_PP(*port_but_ptr, *client_buf_ptr++);
          port_but_ptr++;
        }

        curr_ptr_dev_port_buf = (int32_t *)port_but_ptr;
        curr_ptr_client_buf = (int32_t *)client_buf_ptr;
      }
#else
      /*----- Unoptimized Non-Q6 Version ------*/
      for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
      {
        (*curr_ptr_dev_port_buf) += (*curr_ptr_client_buf++) ;
        curr_ptr_dev_port_buf++;
      }
#endif /* #ifdef __qdsp6__ */

      /* Increment the port buffer pointer by remaining port samples  */
      curr_ptr_dev_port_buf += rem_port_samples;

      /* Increment the client buffer pointer by remaining samples
            only if num client channel > 1, else repeat same channel  */
      if(nClientChannels > 1)
      {
        curr_ptr_client_buf += rem_client_samples;
      }
      else
      {
        curr_ptr_client_buf = client_ptr;
      }
    }
  } /* for loop*/
}

static void afe_port_accumulate_client_samples_64(int16_t client_counter, int32_t nSamplesOut, int32_t nSamplesOutActual,
                                                  int64_t* curr_ptr_dev_port_buf, int32_t* curr_ptr_client_buf,
                                                  int32_t nChanSpacing, int32_t nPortChannels, int32_t nClientChannels)
{
  int32_t     sampIndex, chan;
  int32_t     rem_client_samples = (nChanSpacing - nSamplesOutActual);
  int32_t     rem_port_samples = (nSamplesOut - nSamplesOutActual);
  int32_t     *client_ptr = curr_ptr_client_buf;
  int32_t     minChannel;

  /** If the number of client channel == 1, and nPortChannels > nClientChannels
   *  then max conversion upto stereo and zero out remaining channels
   */
  if(nClientChannels == 1)
  {
    minChannel = s32_min_s32_s32(nPortChannels, 2);
  }
  else /* Copy only the minimum of two channels to port buffers */
  {
    minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);
  }

  /* If client samples < one interrupt samples, zero out the remaining samples
   */
  for(chan = 0; chan < minChannel; chan++)
  {

#ifdef __qdsp6__

    for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
    {
      *curr_ptr_dev_port_buf = Q6_P_add_PP(*curr_ptr_dev_port_buf,(int64_t)(*curr_ptr_client_buf++));
      curr_ptr_dev_port_buf++;
    }

#else
    /*----- Unoptimized Non-Q6 Version ------*/
    for (sampIndex=0; sampIndex < nSamplesOutActual; sampIndex++)
    {
      (*curr_ptr_dev_port_buf) += (int64_t)(*curr_ptr_client_buf++);
      curr_ptr_dev_port_buf++;
    }
#endif /* #ifdef __qdsp6__ */

    /* Increment the port buffer pointer by remaining port samples  */
    curr_ptr_dev_port_buf += rem_port_samples;

    /* Increment the client buffer pointer by remaining samples
            only if num client channel > 1, else repeat same channel  */
    if(nClientChannels > 1)
    {
      curr_ptr_client_buf += rem_client_samples;
    }
    else
    {
      curr_ptr_client_buf = client_ptr;
    }
  } /* for loop*/
}

/**
 * This function is processing the client samples on the Tx path.
 *
 * Assumes deinterleaved data in pSrcBuf
 * corrupts pScratchBuf
 *
 * @param[in] pClientInfo, client info for that port
 * @param[in] pDevPort, handles of the port
 * @return none
 */
static void afe_port_process_tx_client_samples(afe_client_info_t *pClientInfo, afe_dev_port_t *pDevPort)
{
  uint16_t nPortChannels = pDevPort->channels;
  uint16_t nClientChannels = pClientInfo->afe_client.channel;
  uint16_t nChan;
  int8_t *curr_ptr_client_buf;
  int8_t *curr_ptr_left_in;
  uint16_t client_bytes_per_ch = pClientInfo->afe_client.bytes_per_channel;
  uint32_t nSamplesOut = pClientInfo->int_samples_per_period;
  uint32_t out_size_in_bytes = nSamplesOut * client_bytes_per_ch;

  /* Assuming de-interleaved data in port buffers */
  uint32_t cl_ch_spacing_in_bytes = (pClientInfo->buf_size * client_bytes_per_ch);

  /* Pointer to current location in the output buffer for client */
  int8_t *ptr_dst_client_buf = pClientInfo->curr_buff_ptr
      + (pClientInfo->ps_curr_pcm_buff->nOffset * client_bytes_per_ch);

  /* For nClientChannels <= nPortChannels, data for nClientChannels is
   * transferred to clientBuf, with or without resampler as applicable
   *
   * If nClientChannels > nPortChannels, then up conversion only upto
   * max 2 channels, and rest of the client channels are set to 0
   */
  if(nClientChannels > nPortChannels)
  {
    /* Initalize the for loop counter to first un-set client channel       */
    nChan = nPortChannels;

    /* For mono to Many conversion, only do mono to stereo converion set remaining channels zero if > 2
     */
    if(1 == nPortChannels)
    {
      /* Pointer to first left client channel */
      curr_ptr_left_in = ptr_dst_client_buf;

      /* Pointer to first right client channel */
      curr_ptr_client_buf = ptr_dst_client_buf + cl_ch_spacing_in_bytes;

      /* Replicate first left channel in to first right */
      memscpy(curr_ptr_client_buf, pClientInfo->remaining_samples * client_bytes_per_ch, curr_ptr_left_in,
              out_size_in_bytes);

      /* Increment the counter to point to 3rd client channel (if present)*/
      nChan++;
    }

    /* If client channel > 2, zero out the extra client channels
     * remaining channels = (nClientChannels - nPortChannels)
     */
    if(nClientChannels > 2)
    {
      for(; (nChan < nClientChannels); nChan++)
      {
        curr_ptr_client_buf = ptr_dst_client_buf + (nChan * cl_ch_spacing_in_bytes);

        memset(curr_ptr_client_buf, 0, out_size_in_bytes);
      }
    }
  }

  /* Apply gain for loopback */
  afe_port_loopback_info *loopback_info_ptr = pDevPort->loopback_info_ptr;
  if((NULL != loopback_info_ptr) &&                             // Loopback enabled
      (loopback_info_ptr->afe_loopback_state == AFE_PORT_LOOPBACK_STATE_RUN) &&                       //
      (loopback_info_ptr->src_port_client_ptr == pClientInfo) &&  // Loopback src client
      (AFE_UNITY_GAIN != loopback_info_ptr->gain))               // Non-unity gain
  {
    curr_ptr_client_buf = ptr_dst_client_buf;

    for(nChan = 0; nChan < nClientChannels; nChan++) //client data is deinterleaved
    {
      if(2 == client_bytes_per_ch)
      {
        afe_port_multiply_saturate((int16_t *)curr_ptr_client_buf, (int16_t *)curr_ptr_client_buf,
                                   loopback_info_ptr->gain, nSamplesOut);
      }
      else
      {
        afe_port_multiply_saturate_int32((int32_t *)curr_ptr_client_buf, (int32_t *)curr_ptr_client_buf,
                                         loopback_info_ptr->gain, nSamplesOut);
      }

      /* Increment the pointer to point to next channel */
      curr_ptr_client_buf += cl_ch_spacing_in_bytes;
    }
  }

  /* For TX_VOICE client with SIDE TONE data, apply IIR filter, if enabled */
  if((NULL != pDevPort->loopback_info_ptr)
      && (LB_MODE_SIDETONE == pDevPort->loopback_info_ptr->routing_mode)
      && (AFE_PORT_LOOPBACK_STATE_RUN == pDevPort->loopback_info_ptr->afe_loopback_state)
      && (loopback_info_ptr->src_port_client_ptr == pClientInfo) && // Loopback src client
      (NULL != pDevPort->loopback_info_ptr->sidetone_iir_info_ptr))
  {
    afe_port_apply_sidetone_iir(pDevPort, pClientInfo, nSamplesOut);
  }
}


/**
 * This function is for sending data from tx dma to client.
 *
 * Assumes deinterleaved data in pSrcBuf
 * corrupts pScratchBuf
 *
 * @param[in] pClientInfo, client info for that port
 * @param[in] pDevPort, handles of the port
 * @param[in] pSrcBuf, src of data which will be sent to clients
 * @param[in] pScratchBuf, scratch buf needed for temp copies.
 * @return none
 */
void afe_port_send_pcm_samples(afe_client_info_t *pClientInfo, afe_dev_port_t* pDevPort,
                               int8_t *ptr_src_port_buf, int8_t *ptr_scratch_buf)
{
  /* If the client is disabled, omit this client.    */
  if(AFE_CLIENT_ENABLE != pClientInfo->state)
  {
    return;
  }

  samp_rate_conv_t *paResampler;
  uint16_t nPortChannels;
  uint16_t nClientChannels;
  uint16_t minChannel;
  uint16_t nChan;
  uint16_t numResampler;
  uint32_t cl_ch_spacing_in_bytes;
  uint32_t cl_ch_spacing;
  uint32_t resamp_out_ch_spacing_in_bytes;
  uint32_t nSamplesIn;
  uint32_t nSamplesOut;
  int8_t *ptr_dst_client_buf;
  int8_t *curr_ptr_dev_port_buf;
  int8_t *curr_ptr_client_buf;
  int8_t *curr_ptr_right_in;
  uint16_t port_bytes_per_ch;
  uint16_t client_bytes_per_ch;
  uint32_t port_bit_width;
  uint32_t client_bit_width;
  uint32_t in_size_in_bytes;
  uint32_t out_size_in_bytes;
  uint32_t nSize;
  uint32_t byte_conv_mode;
  int8_t *inp_data_ptr;
  int8_t *ptr_stereo_to_mono_avg_out;
  uint8_t i;
  uint8_t num_rs_iter;

  paResampler = (samp_rate_conv_t *)&pClientInfo->samp_rate_conv;
  /* For fractional rs, there is a case when we need to send buffers twice when RS_BUFF_FULL, for other scenarious we
   * do it as normal 1 iteration. */
  num_rs_iter = 1;

  if(TRUE == pDevPort->avt_drift_params.is_avt_drift_updated)
  {
    afe_port_send_avt_drift_update(pDevPort, pClientInfo);
  }

  /* Get new buffer if current buffer is full */
  if(0 == pClientInfo->remaining_samples)
  {
    if(ADSP_SUCCEEDED(afe_port_get_new_buffer(pClientInfo, pClientInfo->bufQ)))
    {
      afe_port_apply_capture_time_stamp(pDevPort, pClientInfo);
    }
    else
    {
      afe_port_print_underrun_msg(pClientInfo, pDevPort);
      return;
    }
  }

  port_bytes_per_ch = pDevPort->bytes_per_channel;
  client_bytes_per_ch = pClientInfo->afe_client.bytes_per_channel;

  port_bit_width = pDevPort->bit_width;
  client_bit_width = pClientInfo->afe_client.bit_width;

  nPortChannels = pDevPort->channels;
  nClientChannels = pClientInfo->afe_client.channel;

  nSamplesIn = pDevPort->int_samples_per_period;
  nSamplesOut = pClientInfo->int_samples_per_period;

  in_size_in_bytes = (nSamplesIn * port_bytes_per_ch);
  out_size_in_bytes = (nSamplesOut * client_bytes_per_ch);

  /* Pointer to current location in the output buffer for client */
  ptr_dst_client_buf = pClientInfo->curr_buff_ptr
      + (pClientInfo->ps_curr_pcm_buff->nOffset * client_bytes_per_ch);

  /* Assuming de-interleaved data in port buffers */
  cl_ch_spacing_in_bytes = (pClientInfo->buf_size * client_bytes_per_ch);
  cl_ch_spacing = pClientInfo->buf_size;

  /* Stereo to mono conv, for other cases use the first channel approach */
  if((pDevPort->dir == AFE_PORT_RX_DIR) && (1 == nClientChannels) && (2 == nPortChannels))
  {
    /* Pointer to right channel in LR pair */
    curr_ptr_right_in = (ptr_src_port_buf + in_size_in_bytes);

    /* 1. For stereo-to-mono + resampler + byte up conversion
     * Src (stereotoMono) scratch_buf (resampler) client buffer (upconv) client buffer
     *
     * 2. For stereo-to-mono + byte down conversion + resampler
     * Src (stereotoMono) scratch_buf (downconv) scratch_buf (resampler) client buffer
     *
     * 3. For stereo-to-mono + resampler
     * Src (stereotoMono) scratch_buf (resampler) client buffer
     */

    ptr_stereo_to_mono_avg_out = ptr_scratch_buf;

    afe_port_stereo2mono_by_avg(ptr_src_port_buf, curr_ptr_right_in, ptr_stereo_to_mono_avg_out,
                                nSamplesIn, port_bytes_per_ch);

    /* Copying contents of scratch_buf back to src_port_buf so as to keep scratch_buf free */
    memscpy(ptr_src_port_buf, nSamplesIn*port_bytes_per_ch,
    		ptr_stereo_to_mono_avg_out, nSamplesIn*port_bytes_per_ch);
  }

  /* Minimum of the port and client channels */
  minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);

  /* If sampling rates of port and client are different */

  if(pDevPort->sample_rate != pClientInfo->afe_client.sample_rate)
  {
    resamp_out_ch_spacing_in_bytes = cl_ch_spacing_in_bytes;

    /* If port bytes per channel != client bytes per channel
     *
     * if(port_bytes_per_ch > client_bytes_per_ch)
     * 24->16 bit inplace conversion on port channels, followed by re-sampler.
     *
     * if(client_bytes_per_ch > port_bytes_per_ch)
     * Re-sample 16-bit port channel samples, followed by up conversion
     * to 24-bit and write it back to client output buf.
     */

    inp_data_ptr = ptr_src_port_buf;

    /* Depending on if resampling happens after bit conversion or before,
     * resampler_out_buf needs to be adjusted.
     * 
     * Bit down conversion -> resampler_out_buf_ptr = ptr_dst_client_buf
     * Bit up conversion   -> resampler_out_buf_ptr = ptr_scratch_buf
     */
    
    int8_t *resampler_out_buf_ptr = ptr_dst_client_buf;

    if(port_bit_width > client_bit_width)
    {
      /* 32->16 bit conversion, in place conversion in src_port_buf */
      afe_port_sample_up_down_convertor(pDevPort, ptr_src_port_buf, ptr_scratch_buf, nSamplesIn,
                                        minChannel, nSamplesIn, nSamplesIn,
                                        pClientInfo->afe_client.bit_width, BYTE_DOWN_CONV);
      inp_data_ptr = ptr_scratch_buf;
      /* Port channels converted to client bytes per channel          */
      port_bytes_per_ch = client_bytes_per_ch;

      /* Re-calculate the input size in bytes          */
      in_size_in_bytes = port_bytes_per_ch * nSamplesIn;
    }
    else if(port_bit_width < client_bit_width)
    {
      /* Re-sampled port channels are spaced as per port pcm byte per sample          */
      resamp_out_ch_spacing_in_bytes = nSamplesOut * port_bytes_per_ch;
      resampler_out_buf_ptr = ptr_scratch_buf;
    }

    /* Total number of Resamplers = min(port_channel, client_channel)
     * Resampler bytes_per_sample = min(port bytes per sample, client bytes per sample)
     */
    numResampler = pClientInfo->unNumResamplers;

    /* Resample min(nClientChannels, nPortChannels) number of channels
     * Resampled output copied directly in to the client output buffer
     */
    paResampler->in_ch_spacing_in_bytes = in_size_in_bytes;
    paResampler->out_ch_spacing_in_bytes = resamp_out_ch_spacing_in_bytes;

    /* numResampler would be used for voice resampler.
     * num channels are not needed for fir capiv2 resampler. they are observed into capiv2
     */

    /* In case of fractional resampling, if the internal buffer is full, we need to send output the buffer twice */
    if(RS_BUFF_FULL == paResampler->rs_buff_flag)
    {
      num_rs_iter = 2;
    }

    for(i = 0; i < num_rs_iter; i++)
    {
      sample_rate_conv_process(paResampler, (int8 *)inp_data_ptr, (int8 *)resampler_out_buf_ptr,
                               (uint32_t *)&nSize, numResampler);

      if(RS_BUFF_EMPTY == paResampler->rs_buff_flag)
      {
        return;
      }

      if(port_bit_width < client_bit_width)
      {
        /* Upconvert client channels to 24-bits
         * 16->32 bit conversion,
         */
        afe_port_sample_up_down_convertor(pDevPort, resampler_out_buf_ptr, ptr_dst_client_buf, nSamplesOut,
                                          minChannel, nSamplesOut, cl_ch_spacing,
                                          pClientInfo->afe_client.bit_width, BYTE_UP_CONV);
      }

      afe_port_process_tx_client_samples(pClientInfo, pDevPort);

      /* Update the client's current buffer pointer for all clients */
      pClientInfo->curr_buff_ptr += out_size_in_bytes;

      /* Update the client's remaining buffer left. */
      pClientInfo->remaining_samples -= nSamplesOut;

      /* If the client's buffer is fully loaded, send the buffer back to client.  */
      if(0 == pClientInfo->remaining_samples)
      {
        afe_port_send_buf_to_data_q(pClientInfo);

        if(ADSP_SUCCEEDED(afe_port_get_new_buffer(pClientInfo, pClientInfo->bufQ)))
        {
          afe_port_apply_capture_time_stamp(pDevPort, pClientInfo);
        }
        else
        {
          MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                "AfePort intf=%x, dir=%d: Buffer not available for client 0x%lX.",
                pDevPort->intf,pDevPort->dir, pClientInfo->client_id);

          afe_port_print_underrun_msg(pClientInfo, pDevPort);
          return;
        }
      }

      /* Pointer to current location in the output buffer for client */
      ptr_dst_client_buf = pClientInfo->curr_buff_ptr
          + (pClientInfo->ps_curr_pcm_buff->nOffset * client_bytes_per_ch);

    }
  }
  else /* Copy the minimum of port and client channel to client buffer */
  {
    curr_ptr_dev_port_buf = ptr_src_port_buf;   //pointer to src port buf
    curr_ptr_client_buf = ptr_dst_client_buf;   //pointer to dest client buf

    minChannel = s32_min_s32_s32(nPortChannels, nClientChannels);

    if(port_bit_width != client_bit_width)
    {
      byte_conv_mode = (port_bit_width > client_bit_width) ?
          BYTE_DOWN_CONV : BYTE_UP_CONV;

      afe_port_sample_up_down_convertor(pDevPort, curr_ptr_dev_port_buf, curr_ptr_client_buf, nSamplesIn,
                                        minChannel, nSamplesIn, cl_ch_spacing,
                                        pClientInfo->afe_client.bit_width, byte_conv_mode);
    }
    else
    {
      /* Copy only min of port and client channels */
      for(nChan = 0; (nChan < minChannel); nChan++)
      {
        memscpy(curr_ptr_client_buf, pClientInfo->remaining_samples * client_bytes_per_ch,
                curr_ptr_dev_port_buf, out_size_in_bytes);

        /* Advance the pointers to point to next channel in port and client bufs             */
        curr_ptr_dev_port_buf += in_size_in_bytes;
        curr_ptr_client_buf += cl_ch_spacing_in_bytes;
      }
    }

    afe_port_process_tx_client_samples(pClientInfo, pDevPort);

    /* Update the client's current buffer pointer for all clients */
    if(pClientInfo->afe_client.interleaved)
    {
      pClientInfo->curr_buff_ptr += (out_size_in_bytes * nClientChannels);
    }
    else
    {
      pClientInfo->curr_buff_ptr += out_size_in_bytes;
    }

    /* Update the client's remaining buffer left. */
    pClientInfo->remaining_samples -= nSamplesOut;
  }

  /* If the client's buffer is fully loaded, send the buffer back to client.  */
  if(0 == pClientInfo->remaining_samples)
  {
    afe_port_send_buf_to_data_q(pClientInfo);
  }
}

ADSPResult afe_port_validate_media_fmt(elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk)
{
  if (NULL == pMediaFormatBlk)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL media format block ");
    return ADSP_EBADPARAM;
  }

  /* Check for valid sample rate */
  if ( (AFE_PORT_SAMPLE_RATE_8K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_11_025K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_12K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_16K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_22_05K != pMediaFormatBlk->sample_rate) && /* compressed HDMI only */
      (AFE_PORT_SAMPLE_RATE_24K != pMediaFormatBlk->sample_rate) && /* mostly for compressed data. */
      (AFE_PORT_SAMPLE_RATE_32K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_44_1K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_88_2K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_48K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_96K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_176_4K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_192K != pMediaFormatBlk->sample_rate)  &&
      (AFE_PORT_SAMPLE_RATE_352_8K != pMediaFormatBlk->sample_rate) &&
      (AFE_PORT_SAMPLE_RATE_384K != pMediaFormatBlk->sample_rate)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE media fwt , invalid sample rate: %lu",
          pMediaFormatBlk->sample_rate);
    return ADSP_EBADPARAM;
  }

  /* Check for valid number of channels */
  if ( (0 == pMediaFormatBlk->num_channels) ||
      (pMediaFormatBlk->num_channels > AFE_PORT_MAX_CHANNEL_CNT)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE media fmt, invalid num channels: %u",
          pMediaFormatBlk->num_channels);
    return ADSP_EBADPARAM;
  }

  /* Check for valid bits per sample */
  if ((AFE_BITS_PER_SAMPLE_16 != pMediaFormatBlk->bits_per_sample) &&
      (AFE_BITS_PER_SAMPLE_24 != pMediaFormatBlk->bits_per_sample) &&
      (AFE_BITS_PER_SAMPLE_32 != pMediaFormatBlk->bits_per_sample)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE media fmt, invalid bits per sample: %u",
          pMediaFormatBlk->bits_per_sample);
    return ADSP_EBADPARAM;
  }

  /* Check for valid interleaved */
  if ( (pMediaFormatBlk->is_interleaved != 1) &&
      (pMediaFormatBlk->is_interleaved != 0)
  )
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE media fmt, invalid interleaved flag: %u",
          pMediaFormatBlk->is_interleaved);
    return ADSP_EBADPARAM;
  }

  return ADSP_EOK;
}

ADSPResult afe_port_data_cmd_eos(afe_client_info_t *pClientInfo,
                                 afe_dev_port_t *pDevPort,
                                 elite_msg_any_t *pMsg)
{
  ADSPResult result = ADSP_EOK;
  elite_msg_data_eos_apr_t *pEosPayload;

  pEosPayload = (elite_msg_data_eos_apr_t *)(pMsg->pPayload);

  MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "EoS msg port_id: 0x%lX, client_id: 0x%lX, EOSFormat: 0x%lx. event_opcode 0x%lx",
        pDevPort->intf, pClientInfo->client_id, pEosPayload->unEosFormat, pEosPayload->eosInfo.event_opcode);

  //Note: for ELITEMSG_DATA_RESET_SESSION_CLK, after EoS, data could come & we must handle it.
  //For others data could come, and we may or may not accept it (although currently, we accept it).

  if((pEosPayload->unEosFormat != ELITEMSG_DATA_EOS_APR)
      && (pEosPayload->unEosFormat != ELITEMSG_DATA_DTMF_EOT)
      && (pEosPayload->unEosFormat != ELITEMSG_DATA_RESET_SESSION_CLK)
      && (pEosPayload->unEosFormat != ELITEMSG_DATA_GAPLESS_TERMINATION_EOS))
  {
    MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unknown unEosFormat: 0x%lx, client_id: 0x%lX, port_id: 0x%lX",
          pEosPayload->unEosFormat, pClientInfo->client_id, pDevPort->intf);

    return ADSP_EFAILED;
  }

  //for ASM_DATA_EVENT_RENDERED_EOS_V2, AFE sends ack from the caller of this function.
  //ack to the HLOS client goes from ASM after taking a hop at matrix/dec/stream-router.
  if (pEosPayload->eosInfo.event_opcode != ASM_DATA_EVENT_RENDERED_EOS_V2)
  {
    result = afe_svc_generate_eos_ack(&(pEosPayload->eosInfo));
  }

  /* Set the eos received flag to true, so that TX clients can send eos msg upstream
 for gapless scenario, intermediate EOS (ELITEMSG_DATA_RESET_SESSION_CLK) should not be propagated */

  if((IS_PSEUDO_PORT_AFE_ID(pDevPort->intf))
      && (ELITEMSG_DATA_RESET_SESSION_CLK != pEosPayload->unEosFormat))
  {
    pDevPort->is_send_eos_to_tx = TRUE;

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RX EoS received, client_id: 0x%lX, port_id: 0x%lX",
          pClientInfo->client_id, pDevPort->intf);
  }

  return result;
}

/**
 * This function is to process cmd from dataQ.
 *
 * @param[in] pClientInfo, client info for that port
 * @param[in] pDevPort, hanles of the port
 * @param[in] pMsg, msg from dataQ
 * @param[in] is_client_mt_cached, flag that indicates to cache media type
 * @return ADSPResult
 */
ADSPResult afe_port_data_cmd(afe_client_info_t *pClientInfo, afe_dev_port_t* pDevPort, elite_msg_any_t* pMsg, bool is_client_mt_cached)
{
  ADSPResult result = ADSP_EFAILED;
  uint32_t unOpCode;
  bool_t is_resamp_init_required = FALSE;
  uint32_t min_num_channels, min_bytes_per_sample;
  uint32_t media_fmt_num_channels, port_num_channels, port_bytes_per_sample,
  media_format_bytes_per_sample;
  uint32_t scratch_buf_size = 0;
  uint32_t client_buf_size = 0, client_samples_per_period = 0;
  int8_t *scratch_buf_tmp = NULL;

  elite_msg_data_media_type_apr_t *pDataMedia;

  unOpCode = pMsg->unOpCode;

  // process data according to op code
  switch(unOpCode)
  {
    case ELITE_DATA_MEDIA_TYPE:
    {
      pDataMedia = (elite_msg_data_media_type_apr_t*)(pMsg->pPayload);

      //Make sure it's the right media type. Change to case (if):
      if(pDataMedia->unMediaTypeFormat != ELITEMSG_DATA_MEDIA_TYPE_APR)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: Error! Unknown unMediaTypeFormat!!!\n");
        break;
      }

      //Make sure it's the fmt linear pcm format or multi-channel media format.
      switch(pDataMedia->unMediaFormatID)
      {
        case ELITEMSG_MEDIA_FMT_MULTI_CHANNEL_PCM:
        {
          elite_multi_channel_pcm_fmt_blk_t *pMediaFormatBlk =
              (elite_multi_channel_pcm_fmt_blk_t*)elite_msg_get_media_fmt_blk(pDataMedia);

          if(ADSP_EOK != (result = afe_port_validate_media_fmt(pMediaFormatBlk)))
          {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported media format for port 0x%x", pDevPort->intf);
            break;
          }
          //if media format is PCM, port need be in pcm type, or vice versa,
          //otherwise, it will report error and exit.
          if(AFE_UNCOMPRESSED_DATA_PORT != pDevPort->port_data_type)
          {
            pClientInfo->state = AFE_CLIENT_DISABLE;
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port Data type %d mismatches incoming pcm data",
                  pDevPort->port_data_type);
            result = ADSP_EBADPARAM;
            break;

          }

          /* If it is a sidetone client and fractional resampling is being used, then we need to disable it
           * as it is unsupported. */
          if(pClientInfo->afe_client.cust_proc.is_it_sidetone
              && afe_is_fractional_resampling_needed(pMediaFormatBlk->sample_rate, pDevPort->sample_rate))
          {
            pClientInfo->state = AFE_CLIENT_DISABLE;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Fractional resampling unsupported for Sidetone client: 0x%lX, port: 0x%X",
                  pClientInfo->client_id, pDevPort->intf);
            break;
          }

          /* In case of is_client_mt_cached = TRUE, just store the media type.
           * Pop-suppressor, client resampler and rate matching re-initialization
           * happens during port start
           *
           * is_client_mt_cached will be TRUE only for client buffers handling
           * during port stop execution. To avoid the loss of media type during this
           * scenario, media type is cached and the buffer is returned immediately.*/
          if(FALSE == is_client_mt_cached)
          {
            media_format_bytes_per_sample = ((pMediaFormatBlk->bits_per_sample)> AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR :AFE_BYTES_PER_SAMPLE_TWO;
            if(NULL != pClientInfo->pop_suppressor_module_ptr)
            {
              /* Pop Suppressor would only work for new MF if the new sampling rate is less than or equal to the original
               * client connect sampling rate. Also, check if the channels and bytes per ch change,
               * then we need to destroy the module if these conditions are not satisfied */
              if((pClientInfo->afe_client.sample_rate < pMediaFormatBlk->sample_rate)
                  || (pClientInfo->afe_client.channel != pMediaFormatBlk->num_channels)
                  || (pClientInfo->afe_client.bytes_per_channel
                      != media_format_bytes_per_sample))
              {
                MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                      "Invalid media type update for supporting pop suppressor, new sample rate: %d,"
                      " channel: %d, bytes_per_ch: %d,"
                      "old sample rate: %d, channel: %d, bytes_per_ch: %d ",
                      pMediaFormatBlk->sample_rate, pMediaFormatBlk->num_channels, (pMediaFormatBlk->bits_per_sample)>>3,
                      pClientInfo->afe_client.sample_rate, pClientInfo->afe_client.channel,
                      pClientInfo->afe_client.bytes_per_channel);

                if(ADSP_EOK == afe_pop_suppressor_end(&pClientInfo->pop_suppressor_module_ptr) )
                {
                  /**<remove pop-suppressor delay in client_gp_delay.*/
                  afe_port_calc_client_gp_delay_us(pClientInfo);
                }
              }
            }

            /* These checks are required because, resampler channels and bit width depends on the
             * min(port_channels, client_channels) min(port bytes, client bytes).
             * Lets say in if client is connected with media format 16KHz, 16bit and 2ch and
             * port is running at 48Khz,16 bit, 2ch. In this case, resampler channels and bit width
             * will be 2ch and 2 bytes respectively.
             *
             * In media format data command, if client sends 16khz, 24 bit, 2ch, eventhough bit width is
             * changed  but min bytes is will not be updated. It will stay at previous value i..e 2 bytes.
             * so  there is no need of for resampler reinit*/

            /* As resampler init can cause signal misses due to excessive processing in init,
             * these checks are required to avoid signal miss in case mediafmt update is not causing resample reinit */

            /* Initialize the current resampler channels and bitwidth */
            min_num_channels = pClientInfo->unNumResamplers;
            min_bytes_per_sample = pClientInfo->samp_rate_conv.bytes_per_sample;

            /* Calculate the minimum of port and media format channels   */
            if(pClientInfo->afe_client.channel != pMediaFormatBlk->num_channels)
            {
              media_fmt_num_channels = pMediaFormatBlk->num_channels;
              port_num_channels = pDevPort->channels;
              min_num_channels = (media_fmt_num_channels < port_num_channels) ?
                  media_fmt_num_channels : port_num_channels;
            }

            /* Calculate the minimum of port and new media format bytes per sample */
            if(pClientInfo->afe_client.bytes_per_channel != (uint32_t)(media_format_bytes_per_sample))
            {
              port_bytes_per_sample = pDevPort->bytes_per_channel;
              min_bytes_per_sample = (media_format_bytes_per_sample < port_bytes_per_sample) ?
                  media_format_bytes_per_sample : port_bytes_per_sample;
            }

            /* Checking only these three parameters,as these (SR, BW, Channels)
             * only affect resampler initialization */
            if((pClientInfo->afe_client.sample_rate != pMediaFormatBlk->sample_rate)
                || (pClientInfo->unNumResamplers != min_num_channels)
                || (pClientInfo->samp_rate_conv.bytes_per_sample != min_bytes_per_sample))
            {
              is_resamp_init_required = TRUE;
            }
          }//is_client_mt_cached = FALSE

          //Update the media format in the pClientInfo.
          pClientInfo->afe_client.sample_rate = pMediaFormatBlk->sample_rate;
          pClientInfo->afe_client.channel = pMediaFormatBlk->num_channels;
          pClientInfo->afe_client.bytes_per_channel = (pMediaFormatBlk->bits_per_sample > AFE_BITS_PER_SAMPLE_16)?AFE_BYTES_PER_SAMPLE_FOUR:AFE_BYTES_PER_SAMPLE_TWO;
          pClientInfo->afe_client.bit_width = pMediaFormatBlk->bits_per_sample;
          pClientInfo->afe_client.interleaved = (bool_t)pMediaFormatBlk->is_interleaved;

          memscpy(pClientInfo->afe_client.channel_mapping, sizeof(pClientInfo->afe_client.channel_mapping),
                  pMediaFormatBlk->channel_mapping, sizeof(pClientInfo->afe_client.channel_mapping));

          /* Not printing these msgs in dynamic thread context*/
#ifdef  DBG_MSG_ENABLE
          MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                "Media type msg port_id=%x,client_id=0x%lX,sample_rate=%lu,"
                "channels=%d,bytes_p_ch=%lu,int_samples_per_period=%lu, bit_width=%lu",
                pDevPort->intf,pClientInfo->client_id,pMediaFormatBlk->sample_rate,pMediaFormatBlk->num_channels,
                pClientInfo->afe_client.bytes_per_channel,pClientInfo->int_samples_per_period, pClientInfo->afe_client.bit_width);
#endif

          /* Collect the max media fmt information from (Tx and Rx) client structures */
          scratch_buf_size = pDevPort->scratch_buf_size;

          afe_port_get_frame_size(pDevPort->latency_mode, pClientInfo->afe_client.sample_rate, &client_samples_per_period);
          client_buf_size = pClientInfo->afe_client.channel * client_samples_per_period * sizeof(uint32_t);

          if(client_buf_size > scratch_buf_size)
          {
            scratch_buf_size = client_buf_size;

            if(NULL != pDevPort->scratch_buf)
            {
              if(NULL
                  == (scratch_buf_tmp = (int8_t *)qurt_elite_memory_aligned_malloc(scratch_buf_size, 8,
                                                                                   AFE_SINK_COPY_BUF)))
              {
                MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                      "Malloc failure while allocating scratch buffer for port:0x%x", pDevPort->intf);
                return ADSP_ENOMEMORY;
              }
              else
              {
                qurt_elite_memory_aligned_free(pDevPort->scratch_buf);
                pDevPort->scratch_buf = scratch_buf_tmp;
#ifdef  DBG_MSG_ENABLE
                MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                      "scratch buffer is assigned for , port:0x%x of size %lu",
                      pDevPort->intf, scratch_buf_size);
#endif
              }
            }

            /* Update and cache scratch buffer size. If scratch buffer is not
             * allocated at this point, use this value to allocate scratch
             * buffer during port start*/
            pDevPort->scratch_buf_size = scratch_buf_size;
          }

          /* In case of is_client_mt_cached = TRUE, just store the media type.
           * Pop-suppressor, client resampler and rate matching re-initialization
           * happens during port start
           *
           * is_client_mt_cached will be TRUE only for client buffers handling
           * during port stop execution. To avoid the loss of media type during this
           * scenario, media type is cached and the buffer is returned immediately.*/
          if(FALSE == is_client_mt_cached)
          {
            //Do the corresponding resample initialization.
            if(TRUE == is_resamp_init_required)
            {
              pClientInfo->int_samples_per_period = 0;

              /* Disable this client. Port mutex is already acquired as part of
               * data command handling */
              pClientInfo->state = AFE_CLIENT_DISABLE;

              /* Send message to AFE service thread for off-loading resampler init
               * This will also take care of rate match info re-init */
              afe_port_send_client_media_fmt_update(pDevPort, pClientInfo);
            }
            /* Re-init the rate match info */
            else
            {
              /* After the first media format command, rate match info needs to init
               * as client samples per interrupt are updated only during Rx port start */
              afe_rate_match_info_t *rate_match_info_ptr = pClientInfo->rate_match_info_ptr;

              if(NULL != rate_match_info_ptr)
              {
                //in this case, mutex is already acquired. hence ok to modify rate_match_info_ptr
                if(ADSP_FAILED(
                    result = afe_port_rate_match_info_reinit(pDevPort, pClientInfo, rate_match_info_ptr)))
                {
                  afe_port_rate_match_info_destroy(pClientInfo);
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "Error in afe_port_rate_match_info_reinit port_id=%x,client_id=0x%lX",
                        pDevPort->intf,pClientInfo->client_id);
                }
              }
            }
          }
        }
        break;
        case ELITEMSG_MEDIA_FMT_COMPRESSED:
        {
          elite_compressed_fmt_blk_t *pMediaFormatBlk =
              (elite_compressed_fmt_blk_t*)elite_msg_get_media_fmt_blk(pDataMedia);

          if(AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
          {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "UNCOMPRESSED PORT: Invalid media format received.");
            break;
          }
          else
          {
            if(pDevPort->sample_rate != pMediaFormatBlk->sample_rate)
            {
              result = ADSP_EUNSUPPORTED;
              MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                    "COMPRESSED PORT: error: Sample rate Mismatch: port Fs =%lu, client Fs =%lu",
                    pDevPort->sample_rate, pMediaFormatBlk->sample_rate);
              break;
            }
          }

          //if media format is DSD, port need be in dsd type, or vice versa,
          //otherwise, it will report error and exit.
          if((ASM_MEDIA_FMT_DSD == pMediaFormatBlk->media_format) ^
              (AFE_DSD_DOP_COMPRESSED_DATA_PORT == pDevPort->port_data_type))
          {
            pClientInfo->state = AFE_CLIENT_DISABLE;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port Data type %d mismatches incoming media fmt %ld",
                  pDevPort->port_data_type, pMediaFormatBlk->media_format);
            result = ADSP_EBADPARAM;
            break;

          }

          //if media format is generic compressed, port need be in dsd type, or vice versa,
          //otherwise, it will report error and exit.
          if((ASM_MEDIA_FMT_GENERIC_COMPRESSED == pMediaFormatBlk->media_format) ^
              (AFE_GENERIC_COMPRESSED_DATA_PORT == pDevPort->port_data_type))
          {
            pClientInfo->state = AFE_CLIENT_DISABLE;
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE Port Data type %d mismatches incoming media fmt %ld",
                  pDevPort->port_data_type, pMediaFormatBlk->media_format);
            result = ADSP_EBADPARAM;
            break;
          }

          if (ASM_MEDIA_FMT_DSD == pMediaFormatBlk->media_format)
          {
            //If DSD data is incoming, need to set the right data now.
            pClientInfo->afe_client.data_type = DSD_DOP_NON_LINEAR;
          }

          if (ASM_MEDIA_FMT_GENERIC_COMPRESSED == pMediaFormatBlk->media_format)
          {
            pClientInfo->afe_client.data_type = GENERIC_COMPRESSED;
          }

          //Update the media format in the pClientInfo.
          pClientInfo->afe_client.sample_rate = pMediaFormatBlk->sample_rate;
          pClientInfo->afe_client.channel = pMediaFormatBlk->num_channels;
          pClientInfo->afe_client.bytes_per_channel = ((pMediaFormatBlk->bits_per_sample)>AFE_BITS_PER_SAMPLE_16)? AFE_BYTES_PER_SAMPLE_FOUR:AFE_BYTES_PER_SAMPLE_TWO;
          pClientInfo->afe_client.bit_width = pMediaFormatBlk->bits_per_sample;
          pClientInfo->afe_client.interleaved = TRUE; //doesn't make sense, but initializing.
          pClientInfo->int_samples_per_period = 0;

          MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
                "Compressed Media type msg port_id=%x,client_id=0x%lX,sample_rate=%lu,"
                "channels=%d,bytes_p_ch=%lu,int_samples_per_period=%lu, client_data_type=%d, port_data_type=%d",
                pDevPort->intf,pClientInfo->client_id,pMediaFormatBlk->sample_rate,pMediaFormatBlk->num_channels,
                pClientInfo->afe_client.bytes_per_channel,pClientInfo->int_samples_per_period, pClientInfo->afe_client.data_type, pDevPort->port_data_type);
          //no need to handle channelMapping (call AFEIoctl) as compressed data doesn't have one.

          //no rate matching to be init'd as compressed ports don't have any rate matching.

          result = ADSP_EOK;
          break;
        }
        default:
        {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AfePort: Error! Unknown unMediaFormatID!!!\n");
          break;
        }
      }
    }
    break;
    case ELITE_DATA_EOS:
    {
      result = afe_port_data_cmd_eos(pClientInfo, pDevPort, pMsg);
    }
    break;
    default:
    {
      result = ADSP_EOK;
      break;
    }
  } // end_switch

  // elite_msg_finish_msg will send a ack if msg have a response queue. if no response queue,
  // send the buffer back to qurt_elite_bufmgr.
  // elite_msg_release_msg simply return the bufer to hexo_bufMgr.
  elite_msg_finish_msg(pMsg, result);
  return result;
}

static ADSPResult afe_port_send_samples_to_multiple_clients(afe_dev_port_t *pDevPort, uint32_t unClientDataPath, \
                                                            int8_t *pSrcBuf, int8_t *pScratchBuf)
{
  bufQList *sClientList;
  ADSPResult result = ADSP_EOK;
  afe_client_info_t *client;

  sClientList = pDevPort->afe_tx_client_list_ptr;
  while(sClientList != NULL)
  {
    client = (afe_client_info_t *)sClientList->element;

    if((client->afe_client.data_path & unClientDataPath)
        && (LINEAR_PCM == client->afe_client.data_type))
    {
      //call send pcm samples also, so that buffers have continuous data (whether or not it's used by downstream)
      afe_port_send_pcm_samples(client, pDevPort, pSrcBuf, pScratchBuf); //client buf offset is zero.(client->ps_curr_pcm_buff->nOffset==0)

      //send the currently filled buffers & then send EoS msg
      if(pDevPort->is_send_eos_to_tx == TRUE) //useful for pseudoport only
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TX send samples EOS %x", pDevPort->intf);
        afe_port_send_last_buf_and_eos(client, pDevPort);
      }
    }

    sClientList = sClientList->next;
  }

  /* After sending EOS to all the Tx clients,
   * set the FLAG to false */
  if(TRUE == pDevPort->is_send_eos_to_tx)
  {
    pDevPort->is_send_eos_to_tx = FALSE;
  }

  return result;
}

/**
 * This function is for sending the EOS message to all the Tx
 * client connected to a port.
 *  Partially filled output buffers are also sent to the client.
 *
 * @param[in] client_info, pointer to client info structure
 * @param[in] dev_port_ptr, pointer to port structure
 * @return ADSPResult, retuns success or failure
 * */
ADSPResult afe_port_send_last_buf_and_eos(afe_client_info_t *client_info,  afe_dev_port_t* dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(AFE_CLIENT_DISABLE == client_info->state)
  {
    return ADSP_EOK;
  }

  /* Send the filled portion of the buffer */
  if(client_info->remaining_samples != 0)
  {
    /* Edit the actual size in the buffer struct to actually filled size */
    if(LINEAR_PCM != client_info->afe_client.data_type)
    {
      client_info->ps_curr_pcm_buff->nActualSize = client_info->actual_buf_size
          - client_info->remaining_samples;
    }
    else
    {
      client_info->ps_curr_pcm_buff->nActualSize = client_info->actual_buf_size
          - (client_info->remaining_samples * client_info->afe_client.bytes_per_channel
              * client_info->afe_client.channel);

      if((client_info->ps_curr_pcm_buff->nActualSize < 0) || (client_info->actual_buf_size < 0))
      {
        MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Incorrect client buf payload size: %ld, or incorrect client actual_buf_size: %ld, "
              "client_id : 0x%lX, port_id : 0x%X",
              client_info->ps_curr_pcm_buff->nActualSize, client_info->actual_buf_size,
              client_info->client_id, dev_port_ptr->intf);
        return ADSP_EFAILED;
      }
    }

    afe_port_send_buf_to_data_q(client_info);

    /* Reset the remaining samples */
    client_info->remaining_samples = 0;
  }

  if(ADSP_EOK != (result = afe_port_generate_tx_eos(client_info)))
  {
    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Sending EoS Failed: client_id : 0x%lX, port_id : 0x%X",
          client_info->client_id, dev_port_ptr->intf);
  }

  return result;
}

/**
 This function is for mixing sidetone samples and audio samples.
 Mixes the sidetone samples with samples in pDevPort->mem_start_addr and places them on pDevPort->mem_start_addr_copy of the pDevPort (Rx port)
 sidetone samples come as deinterleaved, since that's how we initialize when enabling sidetone
 assumption: mem_start_addr is deinterleaved, output mem_start_addr_copy is deinterleaved

 @param[in] pDevPort, ptr to instance of the port
 @param[in] port_buf_ptr, ptr to port samples
 @param[in] sidetone_buf_ptr, ptr to sidetone samples
 @param[in] output_buf_ptr, ptr to output buffer thaat would have the mixed samples
 @param[out] if > 0, then processing has been done successfully, else processing is either not done or is failed.
 @return
 ADSP_EOK error code.

 @dependencies
 None.
 */
static ADSPResult afe_port_mix_sidetone_samples(afe_dev_port_t* pDevPort, int8_t *port_buf_ptr,
                                                int8_t *sidetone_buf_ptr, int8_t *output_buf_ptr)
{
  if(!(pDevPort && port_buf_ptr && sidetone_buf_ptr && output_buf_ptr))
  {
    MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
          "FAILED NULL ptr pDevPort(%p), port_buf_ptr(%p), sidetone_buf_ptr(%p), output_buf_ptr(%p)",
          pDevPort, port_buf_ptr, sidetone_buf_ptr, output_buf_ptr);
    return ADSP_EFAILED;
  }

  ADSPResult result = ADSP_EOK;

  uint32_t sampIndex, nSamplesIn, nChan;
  int16_t *st_plus_port_samp_ptr_16, *port_samp_ptr_16;
  int32_t *st_plus_port_samp_ptr_32, *port_samp_ptr_32, *st_curr_ptr_32;

  nSamplesIn = pDevPort->int_samples_per_period;

  if(2 == pDevPort->bytes_per_channel)
  {
    st_plus_port_samp_ptr_16 = (int16_t *)output_buf_ptr;
    port_samp_ptr_16 = (int16_t *)port_buf_ptr;

    /* 16-bit side tone samples are in 32-bit port buffers unpacked LSB aligned */
    st_curr_ptr_32 = (int32_t *)sidetone_buf_ptr;

    for(nChan = 0; nChan < pDevPort->channels; nChan++)
    {
      for(sampIndex = 0; sampIndex < nSamplesIn; sampIndex++)
      {
        (*st_plus_port_samp_ptr_16++) = s16_saturate_s32(
            (int32_t)(*port_samp_ptr_16++) + (int16_t)(*st_curr_ptr_32++));
      }
    }
  }
  else /* 4 bytes per sample */
  {
    if (AFE_BITS_PER_SAMPLE_24 == pDevPort->bit_width)
    {
      st_plus_port_samp_ptr_32 = (int32_t *)output_buf_ptr;
      port_samp_ptr_32 = (int32_t *)port_buf_ptr;
      st_curr_ptr_32 = (int32_t *)sidetone_buf_ptr;

      for(nChan = 0; nChan < pDevPort->channels; nChan++)
      {
        for(sampIndex = 0; sampIndex < nSamplesIn; sampIndex++)
        {
          (*st_plus_port_samp_ptr_32) = s32_add_s32_s32_sat((*port_samp_ptr_32++), (*st_curr_ptr_32++));

          /* Saturate the 32-bit samples to Q27 */
          *st_plus_port_samp_ptr_32 = s32_shl_s32_sat(*st_plus_port_samp_ptr_32,
                                                      pDevPort->q_format_shift_factor);
          *st_plus_port_samp_ptr_32 = s32_shr_s32_sat(*st_plus_port_samp_ptr_32,
                                                      pDevPort->q_format_shift_factor);
          st_plus_port_samp_ptr_32++;
        }
      }
    }
    else
    {
      st_plus_port_samp_ptr_32 = (int32_t *)output_buf_ptr;
      port_samp_ptr_32 = (int32_t *)port_buf_ptr;
      int64_t *st_curr_ptr_64 = (int64_t *)sidetone_buf_ptr;

      for(nChan = 0; nChan < pDevPort->channels; nChan++)
      {
        for(sampIndex = 0; sampIndex < nSamplesIn; sampIndex++)
        {
          int64_t temp64 = s64_add_s64_s64(((int64_t)(*port_samp_ptr_32++)), (*st_curr_ptr_64++));
          (*st_plus_port_samp_ptr_32) = s32_saturate_s64(temp64);
          st_plus_port_samp_ptr_32++;
        }
      }
    }
  }

  return result;
}

ADSPResult afe_port_set_port_module_params(afe_dev_port_t *pDevPort, uint32_t param_id, int8_t *param_buffer_ptr, uint32_t param_size)
{
  ADSPResult result = ADSP_EOK;

  //AFE_MODULE_PORT SetParam need to be set prior to RUN state.
  if (AFE_PORT_STATE_RUN == pDevPort->port_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x:  Error - cannot be called in RUN state, exiting.", pDevPort->intf);
    result = ADSP_ENOTREADY;
    return result;
  }

  switch (param_id)
  {
    case AFE_PARAM_ID_PORT_MEDIA_TYPE:
    {
      uint32_t version = (uint32_t)(*((uint32_t*)(param_buffer_ptr)));
      if(0 == version)
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - incorrect minor_version (%lu), exiting.",
              pDevPort->intf, version);
        result = ADSP_EBADPARAM;
        return result;
      }

      if(AFE_API_VERSION_PORT_MEDIA_TYPE < version)
      {
        version = AFE_API_VERSION_PORT_MEDIA_TYPE;
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: afe_port_media_type_t - setting minor_version to %lu (max).",
              pDevPort->intf, version);
      }

      switch(version)
      {
        case PORT_MODULE_MEDIA_TYPE_V1:
        {
          afe_port_media_type_v1_t *inp_afe_port_media_type_ptr= (afe_port_media_type_v1_t *)param_buffer_ptr;

          if((AFE_PORT_SAMPLE_RATE_8K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_11_025K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_12K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_16K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_22_05K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_24K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_32K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_44_1K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_48K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_88_2K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_96K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_176_4K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_192K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_352_8K != inp_afe_port_media_type_ptr->sample_rate) &&
              (AFE_PORT_SAMPLE_RATE_384K != inp_afe_port_media_type_ptr->sample_rate))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - sample_rate (%lu), exiting.",
                  pDevPort->intf, inp_afe_port_media_type_ptr->sample_rate);
            result = ADSP_EBADPARAM;
            return result;
          }

          if((AFE_BITS_PER_SAMPLE_16 != inp_afe_port_media_type_ptr->bit_width) && (AFE_BITS_PER_SAMPLE_24 != inp_afe_port_media_type_ptr->bit_width) && (AFE_BITS_PER_SAMPLE_32 != inp_afe_port_media_type_ptr->bit_width))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - bit_width (%d), exiting.",
                  pDevPort->intf, inp_afe_port_media_type_ptr->bit_width);
            result = ADSP_EBADPARAM;
            return result;
          }

          if((1 > inp_afe_port_media_type_ptr->num_channels) || (AFE_PORT_MAX_AUDIO_CHAN_CNT < inp_afe_port_media_type_ptr->num_channels))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - num_channels (%d), exiting.",
                  pDevPort->intf, inp_afe_port_media_type_ptr->num_channels);
            result = ADSP_EBADPARAM;
            return result;
          }

          if((AFE_PORT_DATA_FORMAT_LINEAR_PCM != inp_afe_port_media_type_ptr->data_format) &&
              (AFE_PORT_DATA_FORMAT_GENERIC_COMPRESSED != inp_afe_port_media_type_ptr->data_format))
          {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - data_format (%d), exiting.",
                  pDevPort->intf, inp_afe_port_media_type_ptr->data_format);
            result = ADSP_EBADPARAM;
            return result;
          }

          if(0 != inp_afe_port_media_type_ptr->reserved)
          {
            MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: error in afe_port_media_type_t - reserved (%d), ignoring.",
                  pDevPort->intf, inp_afe_port_media_type_ptr->reserved);
          }

          //assign incoming payload -> port media type
          pDevPort->sample_rate = inp_afe_port_media_type_ptr->sample_rate;
          pDevPort->bit_width = inp_afe_port_media_type_ptr->bit_width;
          pDevPort->channels = inp_afe_port_media_type_ptr->num_channels;
          pDevPort->bytes_per_channel = (pDevPort->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
          if(AFE_PORT_DATA_FORMAT_LINEAR_PCM == inp_afe_port_media_type_ptr->data_format)
          {
            pDevPort->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MODULE_PORT: port_id: 0x%x port enum: AFE_UNCOMPRESSED_DATA_PORT (port media type set param)", pDevPort->intf);
          }
          else if(AFE_PORT_DATA_FORMAT_GENERIC_COMPRESSED == inp_afe_port_media_type_ptr->data_format)
          {
            pDevPort->port_data_type = AFE_GENERIC_COMPRESSED_DATA_PORT;
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_MODULE_PORT: port_id: 0x%x port enum: AFE_GENERIC_COMPRESSED_DATA_PORT (port media type set param)", pDevPort->intf);
          }
          pDevPort->is_port_media_type_set = TRUE;

          break;
        }
        default:
        {
          MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_MODULE_PORT SetParam port_id: 0x%x: Error in afe_port_media_type_t - incorrect minor_version (%lu), exiting.",
                pDevPort->intf, version);
          result = ADSP_EBADPARAM;
          return result;
        }
      }

      break;
    }
    default:
    {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param_id 0x%lx for AFE_MODULE_PORT in port:0x%lx", param_id, pDevPort->intf);
      result = ADSP_EUNSUPPORTED;
      break;
    }
  }

  return result;
}
