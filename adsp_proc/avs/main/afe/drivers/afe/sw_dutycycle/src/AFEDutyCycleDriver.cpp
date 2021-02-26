/*========================================================================

 Copyright (c) 2014-2016,2017 QUALCOMM Technologies, Incorporated.
 All Rights Reserved.
 QUALCOMM Technologies Proprietary.

  $Header: //components/dev/avs.adsp/2.8/spamired.AVS.ADSP.2.8.feb_10/afe/drivers/afe/sw_dutycycle/src/AFEDutyCycleDriver.cpp#1 
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFEDutyCycleDriver.h"


#define  THREAD_STACK_SIZE      4096


static ADSPResult afe_port_sw_duty_cycle_set_up_data_path(void *afe_port_ptr);
static ADSPResult afe_reconfig_timer_for_duty_cycle_reset(void *ptr);
/*==========================================================================
  Functions
========================================================================== */
ADSPResult afe_port_sw_duty_cycle_alloc_resources(afe_dev_port_t *dev_port_ptr)
{
  if(!dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  ADSPResult result = ADSP_EOK;
  avtimer_open_param_t open_param;
  qurt_elite_timer_t   *timer_ptr = NULL;
  afe_dynamic_thread_task_t     task;
  void *dev_cfg_ptr = NULL;
  uint16_t params_buffer_len_req = 0;
  sw_duty_cycle_data_t 	*sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Duty cycle resource alloc starts");

  task.task_signal_ptr =&(sw_duty_cycle_data_ptr->sw_dc_timer_sig);
  task.vp_task_arg = (void*) dev_port_ptr;
  task.pfn_task_func = afe_port_sw_duty_cycle_set_up_data_path;


  result = afe_dynamic_thread_launch(&(sw_duty_cycle_data_ptr->dynamic_thread_ptr),
                                     &task, elite_get_thrd_prio(ELITETHREAD_STAT_AFE_SW_DUTY_CYCLE_PRIO_ID),
                                     THREAD_STACK_SIZE, (uint32)dev_port_ptr->intf);

  if (ADSP_EOK != result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Fail to create thread,result=0x%x port: 0x%x", result,dev_port_ptr->intf);
    goto __bail_out;
  }

  if (NULL == (timer_ptr = (qurt_elite_timer_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_timer_t),QURT_ELITE_HEAP_DEFAULT)))
  {
    result = ADSP_ENOMEMORY;
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create duty cycle reset task timer object");
    goto __bail_out;
  }
  sw_duty_cycle_data_ptr->sw_dc_timer_ptr = timer_ptr;

#ifdef AFE_TIME_SRVR_AV_TIMER
  //this will be created only for first time and will be teared down at final port stop.
  open_param.client_name = (char *)"swdc";
  open_param.flag = 0;
  if (ADSP_EOK != (result = avtimer_drv_hw_open((avtimer_drv_handle_t *)&(sw_duty_cycle_data_ptr->sw_dc_avt_drv_handle), &open_param)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open AVTimer Driver: %x,", dev_port_ptr->intf);
    goto __bail_out;
  }
#endif

  //create one shot timer
  if (ADSP_EOK != (result = afe_time_srvr_timer_create(sw_duty_cycle_data_ptr->sw_dc_timer_ptr, \
                                                       QURT_ELITE_TIMER_ONESHOT_ABSOLUTE,\
                                                       &(sw_duty_cycle_data_ptr->sw_dc_timer_sig))))
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Failed to start dc reset task timer");
    goto __bail_out;
  }

  sw_duty_cycle_data_ptr->is_timer_configured = FALSE;

  /* Backup the device cfg */
  if(IS_I2S_PORT_AFE_ID(dev_port_ptr->intf))
  {
    sw_duty_cycle_data_ptr->device_cfg_size = sizeof(afe_param_id_i2s_cfg_t);
    sw_duty_cycle_data_ptr->device_param_id = AFE_PARAM_ID_I2S_CONFIG;
  }
  else if(IS_SLIMBUS_PORT_AFE_ID(dev_port_ptr->intf))
  {
    sw_duty_cycle_data_ptr->device_cfg_size = sizeof(afe_param_id_slimbus_cfg_t);
    sw_duty_cycle_data_ptr->device_param_id = AFE_PARAM_ID_SLIMBUS_CONFIG;
  }
  else if(IS_PCM_PORT_AFE_ID(dev_port_ptr->intf))
  {
    sw_duty_cycle_data_ptr->device_cfg_size = sizeof(afe_param_id_pcm_cfg_t);
    sw_duty_cycle_data_ptr->device_param_id = AFE_PARAM_ID_PCM_CONFIG;
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported device for SW duty cycling for port: 0x%x", dev_port_ptr->intf);
    goto __bail_out;
  }

  if (NULL == (dev_cfg_ptr = qurt_elite_memory_malloc(sw_duty_cycle_data_ptr->device_cfg_size,QURT_ELITE_HEAP_DEFAULT)))
  {
    result = ADSP_ENOMEMORY;
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create duty cycle device cfg object");
    goto __bail_out;
  }

  memset(dev_cfg_ptr, 0, sw_duty_cycle_data_ptr->device_cfg_size);
  sw_duty_cycle_data_ptr->device_cfg_ptr = dev_cfg_ptr;

  if(ADSP_FAILED(
      result = afe_port_get_device_module_params(
          dev_port_ptr, sw_duty_cycle_data_ptr->device_param_id,
          (int8_t * )sw_duty_cycle_data_ptr->device_cfg_ptr,
          sw_duty_cycle_data_ptr->device_cfg_size, &params_buffer_len_req)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get device cfg for port: 0x%x", dev_port_ptr->intf);
    goto __bail_out;
  }

#ifndef SIM
  /* Regsitering call back which will be called when L2Cache NPA resource is available */
  sw_duty_cycle_data_ptr->l2cache_npa_handle = npa_create_sync_client("/core/cpu/l2cache", "AFE_SWDC", NPA_CLIENT_REQUIRED);
  npa_issue_required_request(sw_duty_cycle_data_ptr->l2cache_npa_handle, 1 );

#endif

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Duty cycle resource alloc ends");

  return result;

  __bail_out:

  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Duty Cycle resource alloc fails :%d",result);

  return result;
}

ADSPResult afe_port_sw_duty_cycle_init(afe_dev_port_t *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  sw_duty_cycle_data_t  *sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;

  /* mark as initialized after all are successfully created */
  sw_duty_cycle_data_ptr->is_initialized = TRUE;

  /* Enable duty cycle if any client requests are pending */
  afe_port_sw_duty_cycle_enable_disable_handler(dev_port_ptr);

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Duty Cycle Init Done succesfully");

  return result;
}

ADSPResult afe_port_sw_duty_cycle_deinit(afe_dev_port_t *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  sw_duty_cycle_data_t 	*sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;

  if (sw_duty_cycle_data_ptr != NULL)
  {
    // mark as de-initialized, it is redundant as it already made false before calling this function.
    sw_duty_cycle_data_ptr->is_initialized = FALSE;

    // disable duty cycle
    afe_port_sw_duty_cycle_enable_disable_handler(dev_port_ptr);
  }
  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Duty Cycle De-Init done");
  return result;
}

ADSPResult afe_port_sw_duty_cycle_set_param_handler(afe_dev_port_t *dev_port_ptr, uint32_t param_id, int8_t* params_buffer_ptr, uint16_t param_size)
{
  ADSPResult 		result = ADSP_EOK;
  sw_duty_cycle_data_t 	*sw_duty_cycle_data_ptr = NULL;

  // create the memory if it is not allocated
  if(NULL == dev_port_ptr->sw_duty_cycle_data_ptr)
  {
    dev_port_ptr->sw_duty_cycle_data_ptr = qurt_elite_memory_malloc(sizeof(sw_duty_cycle_data_t),QURT_ELITE_HEAP_DEFAULT);
    if(NULL == dev_port_ptr->sw_duty_cycle_data_ptr)
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "failed to allocate sw duty cycle object, Port_id: 0x%x",dev_port_ptr->intf);
      return ADSP_ENOMEMORY;
    }
    memset(dev_port_ptr->sw_duty_cycle_data_ptr, 0, sizeof(sw_duty_cycle_data_t));
  }

  sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;

  switch(param_id)
  {
    case AFE_PARAM_ID_SW_DUTY_CYCLE_CFG:
    {
      if (AFE_PORT_STATE_RUN == dev_port_ptr->port_state)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "This param id not allowed in RUN state: 0x%x", dev_port_ptr->intf);
        return ADSP_EUNEXPECTED;
      }

      // choose either the incoming version if we support it or else choose the version we support.
      uint32_t cfg_version = (uint32_t) *params_buffer_ptr;
      if(cfg_version > AFE_API_VERSION_SW_DUTY_CYCLE_CFG)
      {
        cfg_version = AFE_API_VERSION_SW_DUTY_CYCLE_CFG;
      }

      if(0x1 == cfg_version)
      {
        afe_param_id_sw_duty_cycle_cfg_v1_t *payload = (afe_param_id_sw_duty_cycle_cfg_v1_t *)params_buffer_ptr;
        if (param_size < sizeof(afe_param_id_sw_duty_cycle_cfg_v1_t))
        {
          MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Invalid param size for param id :0x%lx",param_id);
          return ADSP_EBADPARAM;
        }

        sw_duty_cycle_data_ptr->sleep_time_us  =  payload->sleep_time_ms*1000;
        MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"sleep time in msec:0x%lx",payload->sleep_time_ms);
      }
    }
    break;
    case AFE_PARAM_ID_ENABLE:
    {
      if (param_size < sizeof(afe_mod_enable_param_t))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Duty cycle Enable Failed: Invalid payload size: %d, Port_id: %d", param_size, dev_port_ptr->intf);
        result = ADSP_EBADPARAM;
        return result;
      }

      //don't allow the client to enable/disable this when port is in RUN state.
      if ((AFE_PORT_STATE_RUN == dev_port_ptr->port_state))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Not allowed enable/disable param when port is in RUN state : 0x%x", dev_port_ptr->intf);
        return ADSP_EUNEXPECTED;
      }

      sw_duty_cycle_data_ptr->sw_dc_master_enable =  *((int16_t*)params_buffer_ptr);
      afe_port_sw_duty_cycle_enable_disable_handler(dev_port_ptr);
    }
    break;
    default:
      result = ADSP_EUNSUPPORTED;
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param ID: 0x%lx, 0x%x", param_id, dev_port_ptr->intf);
      return result;
  }
  MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Processed param id::0x%lx succesfully for port::0x%x",param_id,dev_port_ptr->intf);
  return result;
}

ADSPResult afe_port_sw_duty_cycle_enable_disable_handler(afe_dev_port_t *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;
  sw_duty_cycle_data_t 	*sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;
  bufQList      *pQNode;
  bufQList      **ppQNode;
  afe_client_info_t  *psAfeClientInfo = NULL;
  uint16_t index = 0, mad_audio_ctr = 0;
  bool_t enable_request = FALSE;
  bool_t is_timer_configured = FALSE;

  if (NULL == sw_duty_cycle_data_ptr)
  {
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "SW DC object not created or corrupted");
    return ADSP_EFAILED;
  }
  pQNode = dev_port_ptr->afe_tx_client_list_ptr;
  ppQNode = &(dev_port_ptr->afe_tx_client_list_ptr);

  // check if there are mad audio clients
  for (index = 0; pQNode != NULL; index++)
  {
    psAfeClientInfo = (afe_client_info_t* ) pQNode->element;
    if (AFE_CLIENT_ENABLE == psAfeClientInfo->state)
    {
      //TBD:: If it needs to be generic we need to remove this check. currently
      //this check dc is enabled for MAD audio clients only.
      if (AFE_MAD_AUDIO == psAfeClientInfo->afe_client.cust_proc.audio_sense.type)
      {
        // client enabled and it is MAD AUDIO client, then increment the ctr
        mad_audio_ctr++;
        break;
      }
    }
    ppQNode = &(pQNode->next);
    pQNode = *ppQNode;
  }
  is_timer_configured = sw_duty_cycle_data_ptr->is_timer_configured;
  // if port DC ptr is created and initialized, then move ahead
  if (TRUE == sw_duty_cycle_data_ptr->is_initialized)
  {
    // Enable\disable based on MAD audio client connections and enable set to true from client
    if ((0 != mad_audio_ctr) && (TRUE == sw_duty_cycle_data_ptr->sw_dc_master_enable))
    {
      enable_request = TRUE;
    }
    //if clients are active and dc is not enabled yet, configure timer
    if ((TRUE == enable_request) && (TRUE != is_timer_configured))
    {
      if(ADSP_EOK != (result = afe_reconfig_timer_for_duty_cycle_reset(sw_duty_cycle_data_ptr)))
      {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Error in reconfiguring timer: 0x%x", result);
        return result;
      }
      sw_duty_cycle_data_ptr->is_timer_configured = TRUE;
    }
    //if none of clients are active and dc is enabled, set dc enable= false
    //from here AFE wont receive any interrupts until one of the client comes in enable state.
    if ((FALSE == enable_request) && (FALSE != is_timer_configured))
    {
      // cancel the duration which was set before this occured.
      if (sw_duty_cycle_data_ptr->sw_dc_timer_ptr != NULL)
      {
        afe_time_srvr_timer_cancel_oneshot(sw_duty_cycle_data_ptr->sw_dc_timer_ptr);
      }
      sw_duty_cycle_data_ptr->is_timer_configured = FALSE;
    }
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SW DC is not initialized or created: 0x%x", dev_port_ptr->intf);
  }

  return result;
}


ADSPResult afe_port_sw_duty_cycle_reset_handler(afe_dev_port_t *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if (AFE_PORT_STATE_RUN == dev_port_ptr->port_state)
  {
    afe_port_mutex_lock(dev_port_ptr);

    /* Set the port state to STOP */
    dev_port_ptr->port_state = AFE_PORT_STATE_STOP;

    afe_port_mutex_unlock(dev_port_ptr);

    result = afe_dev_stop(dev_port_ptr);

    result |= afe_port_de_init_at_port_stop(dev_port_ptr);

    result |= afe_mmpm_vote_sleep_latency(dev_port_ptr,AFE_SLEEP_LATENCY_MAX);

    //reconfig the timer for future time
    afe_reconfig_timer_for_duty_cycle_reset((sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr);

    MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE data path is closed as part of duty cycle reset, port id: 0x%X, result: 0x%x",\
          dev_port_ptr->intf, result);
  }
  if (result != ADSP_EOK)
  {
    result = ADSP_EFAILED;
  }
  return result;
}


static ADSPResult afe_reconfig_timer_for_duty_cycle_reset(void *ptr)
{
  ADSPResult result = ADSP_EOK;
  uint64 ll_us;
  sw_duty_cycle_data_t *sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)ptr;

  ll_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)sw_duty_cycle_data_ptr->sw_dc_avt_drv_handle);

  ll_us +=  sw_duty_cycle_data_ptr->sleep_time_us;

  if ( ADSP_EOK != (result = afe_time_srvr_timer_oneshot_start_absolute(sw_duty_cycle_data_ptr->sw_dc_timer_ptr, ll_us)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, " Failed in restarting timer 0x%x",result);
  }
  return result;
}

static ADSPResult afe_port_sw_duty_cycle_set_up_data_path(void *port_ptr)
{
  ADSPResult result = ADSP_EOK;
  afe_dev_port_t    *dev_port_ptr = (afe_dev_port_t *)port_ptr;
  sw_duty_cycle_data_t *sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;


  qurt_elite_mutex_lock(&(dev_port_ptr->port_mutex));

  if (sw_duty_cycle_data_ptr == NULL)
  {
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Memory freed for SW DC");
    result = ADSP_EFAILED;
    goto _dc_start_bail_out;
  }

  if (sw_duty_cycle_data_ptr->sw_dc_master_enable == FALSE)
  {
    MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"SW DC feature is already in disable state");
    result = ADSP_EALREADY;
    goto _dc_start_bail_out;
  }

  if ((AFE_PORT_STATE_RUN != dev_port_ptr->port_state) && (sw_duty_cycle_data_ptr->is_initialized == TRUE))
  {

    /* Set the device cfg */
    if(ADSP_FAILED(
        result = afe_port_set_device_module_params(dev_port_ptr, sw_duty_cycle_data_ptr->device_param_id,
                                                   (int8_t *)sw_duty_cycle_data_ptr->device_cfg_ptr,
                                                   sw_duty_cycle_data_ptr->device_cfg_size)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to set the device cfg during START cmd processing, port-id: 0x%x", dev_port_ptr->intf);
      goto _dc_start_bail_out;
    }

    MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdDmaStart: port_id: 0x%X, sample_rate: %lu, nChannels: %lu",
          (dev_port_ptr->intf), dev_port_ptr->sample_rate,  (dev_port_ptr->channels) );

    if (ADSP_EOK != (result = afe_port_init_at_port_start(dev_port_ptr)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize the port during START cmd processing, port-id: 0x%x", dev_port_ptr->intf);
      goto _dc_start_bail_out;
    }

    if(ADSP_FAILED(result = afe_mmpm_vote_sleep_latency(dev_port_ptr,AFE_SLEEP_LATENCY_DEFAULT)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to vote for min sleep latency: 0x%x", result);
      goto _dc_start_bail_out;
    }

    if (ADSP_EOK !=  (result = afe_dev_start(dev_port_ptr)))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Fail to open port 0x%x",dev_port_ptr->intf);
      goto _dc_start_bail_out;
    }

    dev_port_ptr->port_state = AFE_PORT_STATE_RUN;
  }
  else
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE port is already in RUN state or duty cycle deinitalised, 0x%x", dev_port_ptr->intf);
  }
  _dc_start_bail_out:

  qurt_elite_mutex_unlock(&(dev_port_ptr->port_mutex));
  return result;
}

ADSPResult afe_port_sw_duty_cycle_dealloc_resources(afe_dev_port_t *dev_port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!dev_port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  sw_duty_cycle_data_t *sw_duty_cycle_data_ptr = (sw_duty_cycle_data_t *)dev_port_ptr->sw_duty_cycle_data_ptr;

  if (sw_duty_cycle_data_ptr != NULL)
  {
#ifndef SIM
    if(sw_duty_cycle_data_ptr->l2cache_npa_handle != NULL)
    {
      npa_complete_request(sw_duty_cycle_data_ptr->l2cache_npa_handle);
      npa_destroy_client(sw_duty_cycle_data_ptr->l2cache_npa_handle);
      sw_duty_cycle_data_ptr->l2cache_npa_handle = NULL;
    }
#endif

#ifdef AFE_TIME_SRVR_AV_TIMER
    if (0 != sw_duty_cycle_data_ptr->sw_dc_avt_drv_handle)
    {
      avtimer_drv_hw_close((avtimer_drv_handle_t)sw_duty_cycle_data_ptr->sw_dc_avt_drv_handle);
      sw_duty_cycle_data_ptr->sw_dc_avt_drv_handle =  0;
    }
#endif
    if (sw_duty_cycle_data_ptr->sw_dc_timer_ptr != NULL)
    {
      //delete timer
      result = afe_time_srvr_timer_delete(sw_duty_cycle_data_ptr->sw_dc_timer_ptr);
      if(ADSP_EOK != result)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "error while deleting timer: %d", result);
      }
      qurt_elite_memory_free(sw_duty_cycle_data_ptr->sw_dc_timer_ptr);
      sw_duty_cycle_data_ptr->sw_dc_timer_ptr =NULL;
    }

    /* Free device cfg backup */
    if(sw_duty_cycle_data_ptr->device_cfg_ptr)
    {
      qurt_elite_memory_free(sw_duty_cycle_data_ptr->device_cfg_ptr);
      sw_duty_cycle_data_ptr->device_cfg_ptr = NULL;
    }

    if(ADSP_FAILED(result = afe_dynamic_thread_destroy(&(sw_duty_cycle_data_ptr->dynamic_thread_ptr))))
    {
      MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"Error in destroying duty cycle thread:0x%x",result);
    }

    //free data structure memory in final stop
    qurt_elite_memory_free(sw_duty_cycle_data_ptr);
    dev_port_ptr->sw_duty_cycle_data_ptr = NULL;
  }

  return result;
}

void afe_port_disable_sw_duty_cycle(afe_dev_port_t *pDevPort)
{
  if(!pDevPort)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return;
  }

  sw_duty_cycle_data *sw_duty_cycle_data_ptr = (sw_duty_cycle_data *)pDevPort->sw_duty_cycle_data_ptr;

  /* Disable dc feature in final port stop, so DC thread wont do any set up of channels if any timer interrupt
      comes at this instance and DC thread will return from function and waits on channel
      once mutex released by AFeS. */

  /**<sw_dc_master_enable is configured by HLOS,
   * during device recovery need to retain the value.*/
  if(AFE_DEV_RECOVERY != pDevPort->dev_proc_seq)
  {
    sw_duty_cycle_data_ptr->sw_dc_master_enable = FALSE;
  }

  /* Cancel any active configurations.
      No timer interrupts will come now after this point */
  afe_time_srvr_timer_cancel_oneshot(sw_duty_cycle_data_ptr->sw_dc_timer_ptr);
  sw_duty_cycle_data_ptr->is_timer_configured = FALSE;
}
