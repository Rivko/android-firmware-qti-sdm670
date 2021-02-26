/*========================================================================
   This file contains HW codec specific configuration, interrupt handling

  Copyright (c) 2011-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/codec/src/AFEHwMadDriver.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
========================================================================== */
#include "AFECodecHandler.h"
#include "AFEHwMadDriver_i.h"
#include "AFEHwMadDriver.h"
#include "AFEInternal.h"
#include "AFESwMad.h"
#include "AFEMmpm.h"

// SLIMBUS BW vote
#define HWMAD_VOTING_SLIMBUS_BW  (1504000)

/*==========================================================================
  Functions
========================================================================== */
ADSPResult afe_port_hw_mad_alloc_resources(void *port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  afe_dev_port_t *pDevPort = (afe_dev_port_t *)port_ptr;
  hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD resource alloc Starts");

  /* Voting for SLIMBUS BW */
  if(ADSP_EOK != (result = afe_slimbus_bw_voting(cdc_handler_global_ptr->sb_cfg.p_sb_driver, HWMAD_VOTING_SLIMBUS_BW)))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO, "voting Slimbus BW is failing %d",result);
    return ADSP_EFAILED;
  }

  /* Registering call back which will be called when L2Cache NPA resource is available */
  hw_mad_ptr->listen_l2cach_npa_handle = npa_create_sync_client("/core/cpu/l2cache", "AFE_HWMAD", NPA_CLIENT_REQUIRED);
  npa_issue_required_request(hw_mad_ptr->listen_l2cach_npa_handle, 1 );

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD resource alloc Ends");

  return result;
}

ADSPResult afe_port_hw_mad_init(void *port_ptr)
{
  ADSPResult       result = ADSP_EOK;

  if(!port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  afe_dev_port_t   *pDevPort = (afe_dev_port_t *)port_ptr;
  hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD Initialization Starts");

  /* Initialize hw mad codec registers device cfg */
  afe_cdc_hw_mad_reg_cfg_init((void *)hw_mad_ptr);

  /* Mark as initialized */
  hw_mad_ptr->is_initialized = TRUE;

  /* Enable hw MAD module and register codec interrupts if
      any client requests are pending */
  result = afe_port_hw_mad_enable_disable_handler(port_ptr);

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD Initialization Ends");

  return result;
}

ADSPResult afe_port_hw_mad_dealloc_resources(void *port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  afe_dev_port_t *pDevPort = (afe_dev_port_t *)port_ptr;
  hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;

  if (NULL == hw_mad_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD destroy failed, hw mad obj pointer is NULL");
    return ADSP_EFAILED;
  }

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD dealloc Starts");

  if(hw_mad_ptr->listen_l2cach_npa_handle != NULL)
  {
    npa_complete_request(hw_mad_ptr->listen_l2cach_npa_handle);
    npa_destroy_client(hw_mad_ptr->listen_l2cach_npa_handle);
    hw_mad_ptr->listen_l2cach_npa_handle = NULL;
    /* npa_issue_required_request( listen_l2cache_npa_handle, 0 ); - equivalent to above */
  }

  // TBD: Check do we need to write any MAD hw reg, like sleep time
  if(ADSP_EOK != (result = afe_slimbus_bw_voting(cdc_handler_global_ptr->sb_cfg.p_sb_driver,0)))
  {
    MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "voting Slimbus BW is failing %d",result);
  }

  /* Free hw mad memory */
  qurt_elite_memory_free(hw_mad_ptr);

  /* Set the ptr to NULL */
  pDevPort->hw_mad_ptr = NULL;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD dealloc ends");

  return result;
}

ADSPResult afe_port_hw_mad_deinit(void *port_ptr)
{
  ADSPResult result = ADSP_EOK;

  if(!port_ptr)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer");
    return ADSP_EFAILED;
  }

  afe_dev_port_t *pDevPort = (afe_dev_port_t *)port_ptr;
  hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD De-Initialization Starts");

  /* Master disable all the MAD blocks */
  hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO].sw_master_enable = FALSE;
  hw_mad_ptr->mad_data[AFE_HW_MAD_BEACON].sw_master_enable = FALSE;
  hw_mad_ptr->mad_data[AFE_HW_MAD_ULTRA_SOUND].sw_master_enable = FALSE;

  /* Disable hw MAD module and de-register codec interrupts if no client needs */
  result |= afe_port_hw_mad_enable_disable_handler(port_ptr);

  /* Mark as de-initialized */
  hw_mad_ptr->is_initialized = FALSE;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HWMAD De-Initialization Ends");

  return result;
}

ADSPResult afe_port_hw_mad_set_param_handler(void *port_ptr, uint32_t param_id, int8_t* params_buffer_ptr, uint16_t param_size)
{
	ADSPResult 		result = ADSP_EOK;
	hw_mad_data_t 	*hw_mad_ptr = NULL;
	afe_dev_port_t 	*pDevPort =  (afe_dev_port_t *)port_ptr;

	// create the memory if it is nto allocated
	if(!pDevPort->hw_mad_ptr)
	{
		pDevPort->hw_mad_ptr = qurt_elite_memory_malloc(sizeof(hw_mad_data_t),QURT_ELITE_HEAP_DEFAULT);
		if(!pDevPort->hw_mad_ptr)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HWMAD: memory allocation failed"); //err check
			return ADSP_ENOMEMORY;
		}
		memset(pDevPort->hw_mad_ptr, 0, sizeof(hw_mad_data_t));
	}

	hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;

	switch(param_id)
	{
	   case AFE_PARAM_ID_HW_MAD_CFG: 
	   {
		   if (AFE_PORT_STATE_RUN == pDevPort->port_state)
		   {
			  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW MAD cfg failed, port is in RUN state: 0x%x", pDevPort->intf);
			  return ADSP_EUNEXPECTED;
		   }

		   // choose either the incoming version if we support it or else choose the version we support.
		   uint32_t hwmad_cfg_version = (uint32_t) *params_buffer_ptr;
		   if(hwmad_cfg_version > AFE_API_VERSION_HW_MAD_CONFIG)
		   {
			   hwmad_cfg_version = AFE_API_VERSION_HW_MAD_CONFIG;
		   }

		   if(0x1 == hwmad_cfg_version)
		   {
			   afe_hw_mad_cfg_param_v1_t *payload = (afe_hw_mad_cfg_param_v1_t *)params_buffer_ptr;
			   if (param_size < sizeof(afe_hw_mad_cfg_param_v1_t))
			   {
				   MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Invalid param size for param id :0x%lx",param_id);
				   return ADSP_EBADPARAM;
			    }

			   hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO].sleep_time = afe_cdc_hwmad_sleep_time_conversion(payload->audio_sleep_time_ms);
			   hw_mad_ptr->mad_data[AFE_HW_MAD_BEACON].sleep_time = afe_cdc_hwmad_sleep_time_conversion(payload->beacon_sleep_time_ms);
			   hw_mad_ptr->mad_data[AFE_HW_MAD_ULTRA_SOUND].sleep_time = afe_cdc_hwmad_sleep_time_conversion(payload->us_sleep_time_ms);
		   }
	   }
	   break;
	   case AFE_PARAM_ID_HW_MAD_CTRL:
	   {
		   // choose either the incoming version if we support it or else choose the version we support.
		   uint32_t hwmad_ctrl_version = (uint32_t) *params_buffer_ptr;
		   if(hwmad_ctrl_version > AFE_API_VERSION_HW_MAD_CTRL)
		   {
			   hwmad_ctrl_version = AFE_API_VERSION_HW_MAD_CTRL;
		   }

		   if(0x1 == hwmad_ctrl_version)
		   {
			   afe_hw_mad_ctrl_param_v1_t *payload = (afe_hw_mad_ctrl_param_v1_t *)params_buffer_ptr;
			   if (param_size < sizeof(afe_hw_mad_ctrl_param_v1_t))
			   {
				   MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Invalid param size for param id :0x%lx",param_id);
				   return ADSP_EBADPARAM;
			   }

			   // check for MAD Audio bit
			   if ((payload->mad_type) & (1 << AFE_HW_MAD_AUDIO_BIT))
			   {
				   hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO].sw_master_enable = payload->mad_enable;
			   }

			   // check for MAD beacon bit
			   if ((payload->mad_type) & (1 << AFE_HW_MAD_BEACON_BIT))
			   {
				   hw_mad_ptr->mad_data[AFE_HW_MAD_BEACON].sw_master_enable = payload->mad_enable;
			   }

			   // check for MAD ultra sound bit
			   if ((payload->mad_type) & (1 << AFE_HW_MAD_ULT_BIT))
			   {
				   hw_mad_ptr->mad_data[AFE_HW_MAD_ULTRA_SOUND].sw_master_enable = payload->mad_enable;
			   }
			   // reflect master HW MAD control on the hw enablement\disablement
			   result = afe_port_hw_mad_enable_disable_handler(pDevPort);
		   }
	   }
	   break;
	   case  AFE_PARAM_ID_SLIMBUS_SLAVE_PORT_CFG:
	   {
		   if (AFE_PORT_STATE_RUN == pDevPort->port_state)
		   {
			   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HW MAD cfg failed, port is in RUN state: 0x%x", pDevPort->intf);
			   return ADSP_EUNEXPECTED;
		   }

		   // choose either the incoming version if we support it or else choose the version we support.
		   uint32_t slimbus_cfg_version = (uint32_t) *params_buffer_ptr;
		   if(slimbus_cfg_version > AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG)
		   {
			   slimbus_cfg_version = AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG;
		   }

		   if(0x1 == slimbus_cfg_version)
		   {
			   if (param_size < sizeof(afe_slimbus_slave_port_cfg_param_v1_t))
			   {
				   MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Invalid param size for param id :0x%lx",param_id);
				   return ADSP_EBADPARAM;
			   }
			   memscpy(&hw_mad_ptr->sb_cfg, sizeof(afe_slimbus_slave_port_cfg_param_v1_t),
					   params_buffer_ptr, sizeof(afe_slimbus_slave_port_cfg_param_v1_t));
		   }

		   // move the port state to Config state
		   pDevPort->port_state = AFE_PORT_STATE_CONFIG;
	   }
	   break;
	   default:
		   result = ADSP_EUNSUPPORTED;
		   MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param ID: 0x%lx, 0x%x", param_id, pDevPort->intf);
		   return result;
	}
    MSG_2(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Processed param id::0x%lx succesfully for port::0x%x",param_id,pDevPort->intf); 
	return result;
}

ADSPResult afe_port_hw_mad_enable_disable_handler(void *ptr)
{
	ADSPResult result = ADSP_EOK;
	afe_dev_port_t *pDevPort = (afe_dev_port_t *)ptr;
	hw_mad_data_t 	*hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;
	bufQList      *pQNode;
	bufQList      **ppQNode;
	afe_client_info_t  *psAfeClientInfo = NULL;
	audio_sense_info_t  *pAudSenseInfo=NULL;
	uint16_t index = 0, mad_audio_ctr = 0, mad_beacon_ctr = 0, mad_ult_ctr = 0;

	pQNode = pDevPort->afe_tx_client_list_ptr;
	ppQNode = &(pDevPort->afe_tx_client_list_ptr);

	// check if there are mad audio clients
	for (index = 0; pQNode != NULL; index++)
	{
	  psAfeClientInfo = (afe_client_info_t* ) pQNode->element;

	  pAudSenseInfo       =  &(psAfeClientInfo->afe_client.cust_proc.audio_sense);

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

	// if port HW MAD is created and initialized, then move ahead
	if ((NULL != hw_mad_ptr) && (TRUE == hw_mad_ptr->is_initialized))
	{
		// register\de-register MAD Audio interrupt
		result |= hw_mad_interrupt_register_handler(pDevPort, mad_audio_ctr, AFE_HW_MAD_AUDIO);

		// register\de-register MAD Beacon interrupt
		result |= hw_mad_interrupt_register_handler(pDevPort, mad_beacon_ctr, AFE_HW_MAD_BEACON);

		// register\de-register MAD Ultrasound interrupt
		result |= hw_mad_interrupt_register_handler(pDevPort, mad_ult_ctr, AFE_HW_MAD_ULTRA_SOUND);
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HW MAD module is not initialized or created: 0x%x", pDevPort->intf);
	}

	return result;
}

ADSPResult afe_port_hw_mad_interrupt_handler(void *ptr, cdc_client_info_t *client_info)
{
   ADSPResult result = ADSP_EOK;
   uint32_t nSize;
   elite_msg_any_t msg;
   hw_mad_mod_t *mad_mod_ptr = NULL;
   bool enable_int_ack_ctrl = FALSE;
   afe_dev_port_t *pDevPort = (afe_dev_port_t *)ptr;
   elite_msg_custom_afe_cdc_int_info_t *pConnectPayload = NULL;
   hw_mad_data_t *hw_mad_ptr = (hw_mad_data_t *)pDevPort->hw_mad_ptr;
   elite_svc_handle_t *cdc_svc_hptr = &(afe_svc_global_ptr->cdc_service_handle);

   if(NULL == hw_mad_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " HW MAD is already deintialised, ignoring interrupt");
      return ADSP_EOK;
   }

   /* Any interrupt need to be acked properly, in the case of errors also. */
   switch(client_info->int_index)
   {
      case AFE_CDC_INT_MAD_AUDIO:
      {
         if(AFE_PORT_STATE_RUN != pDevPort->port_state)
         {
            // check whether do we need to send the ACK to codec int handler or not
            enable_int_ack_ctrl = hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO].client_info.enable_int_ack_ctrl;

            // mad module ptr
            mad_mod_ptr = &(hw_mad_ptr->mad_data[AFE_HW_MAD_AUDIO]);

            // cfg the device
            afe_set_slimbus_cfg(pDevPort, (int8_t *)&hw_mad_ptr->sb_cfg, sizeof(hw_mad_ptr->sb_cfg),
                                FALSE, 16000);

            MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFECmdDmaStart: port_id: 0x%X, sample_rate: %lu, nChannels: %lu",
                  (pDevPort->intf), pDevPort->sample_rate, (pDevPort->channels) );

            if(ADSP_EOK != (result = afe_port_init_at_port_start(pDevPort)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize the port during START cmd processing, port-id: 0x%x", pDevPort->intf);
               goto _mad_audio_bail_out;
            }
            if(ADSP_FAILED(result = afe_mmpm_vote_sleep_latency(pDevPort,AFE_SLEEP_LATENCY_DEFAULT)))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to vote for min sleep latency: 0x%x", result);
               goto _mad_audio_bail_out;
            }

            if(ADSP_EOK != (result = afe_dev_start(pDevPort)))
            {
               MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"Fail to open port 0x%x",pDevPort->intf);
               goto _mad_audio_bail_out;
            }
            pDevPort->port_state = AFE_PORT_STATE_RUN;
         }
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE port is already in RUN satte, 0x%x", pDevPort->intf);
         }
         break;

         _mad_audio_bail_out:
         /* add code to clean up the resources */
         ;
      }
      break;
      default:
      {
         result = ADSP_EFAILED;
      }
      break;
   }

   // send ACK msg as per the config
   if(TRUE == enable_int_ack_ctrl)
   {
      nSize = sizeof(elite_msg_custom_afe_cdc_int_info_t);
      if(ADSP_EOK != elite_msg_create_msg(&msg, &nSize, ELITE_CUSTOM_MSG, NULL, 0, ADSP_EOK))
      {
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to create EliteMsg");
         return ADSP_EFAILED;
      }
      pConnectPayload = (elite_msg_custom_afe_cdc_int_info_t*)msg.pPayload;

      memscpy(&pConnectPayload->client_info, sizeof(cdc_client_info_t), &mad_mod_ptr->client_info,
              sizeof(cdc_client_info_t));

      pConnectPayload->sec_op_code = ELITEMSG_CUSTOM_CDC_INT_CLIENT_ACK;
      if(ADSP_EOK != qurt_elite_queue_push_back(cdc_svc_hptr->cmdQ, (uint64_t *)&msg))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to send interrupt ack msg %d", result);
         elite_msg_return_payload_buffer(&msg);
         return ADSP_EFAILED;
      }
   }

   return result;
}

ADSPResult afe_port_hw_mad_dc_reset_handler(void *ptr, uint16_t mad_type)
{
   ADSPResult result = ADSP_EOK;
   afe_dev_port_t *pDevPort = (afe_dev_port_t *)ptr;

   if(AFE_PORT_STATE_RUN == pDevPort->port_state)
   {
      afe_port_mutex_lock(pDevPort);

      /* Set the port state to STOP */
      pDevPort->port_state = AFE_PORT_STATE_STOP;

      afe_port_mutex_unlock(pDevPort);

      afe_hw_mad_dc_codec_sequence((hw_mad_data_t *)pDevPort->hw_mad_ptr, mad_type);

      result = afe_dev_stop(pDevPort);

      result |= afe_port_de_init_at_port_stop(pDevPort);

      result |= afe_mmpm_vote_sleep_latency(pDevPort, AFE_SLEEP_LATENCY_MAX);

      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Slimbus data path is closed as part of duty cycle reset, port id: 0x%X, result: 0x%x",pDevPort->intf, result);
   }

   return result;
}
