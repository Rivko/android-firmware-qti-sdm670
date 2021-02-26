/*========================================================================
   This file implements functions for handling AV Sync in elite frame work.
   It also addresses loopback rate matching.

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEAvSync.cpp#1 $
 ====================================================================== */

/*==========================================================================
  the Include files
  ========================================================================== */
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "AFEPortManagement.h"
#include "AFERtProxyDriver.h"
#include "AFERivaDriver.h"
#include "AFEPopSuppressor_api.h"
#include "AFEUSBAudioDriver.h"
#include "AFELimiter.h"
#include "AFEEncoder_If.h"
#include "AFEDecoder_If.h"


ADSPResult afe_port_get_avsync_test_prv_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
		int8_t* param_buffer_ptr, int32_t param_buf_len, uint16_t* params_buffer_len_req_ptr)
{

	switch(param_id)
	{
	case AFE_PRV_PARAM_ID_AVSYNC_STATS:
	{
		*params_buffer_len_req_ptr = sizeof(afe_prv_avsync_stats_param_t);
		if (*params_buffer_len_req_ptr > param_buf_len)
		{
			MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "afe_port_get_avsync_test_prv_module_params fail, required size = %d, given size = %lu",*params_buffer_len_req_ptr, param_buf_len);
			return ADSP_EBADPARAM;
		}
		else
		{
			afe_avsync_stats_t *av_stats_ptr = (afe_avsync_stats_t *) pDevPort->avsync_test_ptr;
			afe_prv_avsync_stats_param_t *req_cfg_ptr = (afe_prv_avsync_stats_param_t *)param_buffer_ptr;
			memset(req_cfg_ptr, 0, sizeof(afe_prv_avsync_stats_param_t));
			if (NULL != av_stats_ptr)
			{
				req_cfg_ptr->non_zero_ts_lsw  = (uint32_t ) (av_stats_ptr->nonzero_timestamp);
				req_cfg_ptr->non_zero_ts_msw  = (uint32_t ) ((av_stats_ptr->nonzero_timestamp)>>32);
				req_cfg_ptr->is_valid = TRUE;
				MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Get AVSYNC Stats: TSmsw: %ld, TSlsw: %ld, Valid: %d, port-id:0x%x", req_cfg_ptr->non_zero_ts_msw, req_cfg_ptr->non_zero_ts_lsw, req_cfg_ptr->is_valid, pDevPort->intf);
			}
		}
	}
	break;
	default:
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param id: %ld, port-id:0x%x", param_id, pDevPort->intf);
		return ADSP_ENOMEMORY;
	}
	}

	return ADSP_EOK;
}

ADSPResult afe_port_set_avsync_test_prv_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
		int8_t *param_buffer_ptr, uint16_t param_size)
{
	afe_avsync_stats_t *av_stats_ptr = (afe_avsync_stats_t *) pDevPort->avsync_test_ptr;

	switch(param_id)
	{
	case AFE_PARAM_ID_ENABLE:
	{
		if (param_size < sizeof(afe_mod_enable_param_t) )
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port 0x%x: improper payload size at ENABLE/DISABLE", pDevPort->intf);
			return ADSP_EBADPARAM;
		}
		else
		{
			int16_t enable_flag = *((int16_t*)param_buffer_ptr);

			if (TRUE == enable_flag)
			{
				if (NULL == av_stats_ptr)
				{
					if (NULL == (av_stats_ptr = (afe_avsync_stats_t*)qurt_elite_memory_malloc((sizeof(afe_avsync_stats_t)), QURT_ELITE_HEAP_DEFAULT)))
					{
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create AVSYNC stats struct object, port-id: 0x%x", pDevPort->intf);
						return ADSP_ENOMEMORY;
					}
					memset(av_stats_ptr, 0, sizeof(afe_avsync_stats_t));
					pDevPort->avsync_test_ptr = av_stats_ptr;
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Enabled AVSYNC stats, port-id: 0x%x",pDevPort->intf);
				}
			}
			else
			{
				if (NULL != av_stats_ptr)
				{
					qurt_elite_memory_free(av_stats_ptr);
					pDevPort->avsync_test_ptr = NULL;
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Disabled AVSYNC stats, port-id: 0x%x",pDevPort->intf);
				}
			}
		}
	}
	break;
	default:
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported param id: %ld, port-id: 0x%x", param_id, pDevPort->intf);
		return ADSP_ENOMEMORY;
	}
	}

	return ADSP_EOK;
}

void afe_port_collect_avsync_stats(afe_dev_port_t *pDevPort, int8_t *buffer)
{
	afe_avsync_stats_t *av_stats_ptr = (afe_avsync_stats_t *) pDevPort->avsync_test_ptr;
	uint32_t loop_index = 0;

	if(0 == av_stats_ptr->nonzero_timestamp)
	{
		if (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
		{
			uint32_t total_samples;
			total_samples = pDevPort->int_samples_per_period * pDevPort->channels * pDevPort->bytes_per_channel;
			for (loop_index = 0; loop_index < total_samples; loop_index++)
			{
				if(0 != buffer[loop_index])
				{
					if (IS_IT_TX_PORT_AFE_ID(pDevPort->intf))
					{
						av_stats_ptr->nonzero_timestamp = avtimer_drv_get_time((avtimer_drv_handle_t)pDevPort->avt_drv) - ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate) + ((uint64_t)((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC) * loop_index)/pDevPort->sample_rate)/total_samples;
					}
					else
					{
						av_stats_ptr->nonzero_timestamp = avtimer_drv_get_time((avtimer_drv_handle_t)pDevPort->avt_drv) + ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate) + ((uint64_t)((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC) * loop_index)/pDevPort->sample_rate)/total_samples;
					}

					MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Port-Id: 0x%x, Non zero TS %lu, Buffer[loop_index] %d", pDevPort->intf, (uint32_t)av_stats_ptr->nonzero_timestamp, buffer[loop_index]);
					break;
				}
			}
		}
		else // compressed ports
		{
			uint16_t *bufferPtr, type;
			uint16_t Pa = 0xF872, Pb = 0x4E1F;
			bufferPtr = (uint16_t *)buffer;

			while(loop_index < (pDevPort->int_samples_per_period * pDevPort->channels))
			{
				if((Pa == bufferPtr[loop_index]) && (Pb == bufferPtr[loop_index + 1]))
				{
					type = bufferPtr[loop_index + 2] & 0x1F;
					if((0 != type) && (3 != type))
					{
						if (IS_IT_TX_PORT_AFE_ID(pDevPort->intf))
						{
							av_stats_ptr->nonzero_timestamp = avtimer_drv_get_time((avtimer_drv_handle_t)pDevPort->avt_drv)- ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate) + ((uint64_t)((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC) * loop_index)/pDevPort->sample_rate)/(pDevPort->int_samples_per_period * pDevPort->channels);
						}
						else
						{
							av_stats_ptr->nonzero_timestamp = avtimer_drv_get_time((avtimer_drv_handle_t)pDevPort->avt_drv)+ ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate) + ((uint64_t)((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC) * loop_index)/pDevPort->sample_rate)/(pDevPort->int_samples_per_period * pDevPort->channels);
						}
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Valid data burst Ts: %lu, port-id: 0x%x", (uint32_t)av_stats_ptr->nonzero_timestamp, pDevPort->intf);
						break;
					}
					loop_index += 4;
				}
				else
				{
					loop_index += 2;
				}
			}
		} // while loop end
	}

	return;
}

afe_client_info_t* afe_port_get_client_node(bufQList **ppQNode,elite_svc_handle_t  *svc_handle_ptr)
{
	afe_client_info_t   *psAfeClientInfo;
	bufQList*           pQNode;

	pQNode = *ppQNode;
	while(pQNode != NULL)
	{
		psAfeClientInfo = (afe_client_info_t *)(*ppQNode)->element;
		if (&(psAfeClientInfo->this_client_manager) == svc_handle_ptr)
		{
			return psAfeClientInfo;
		}
		else
		{
			ppQNode = &(pQNode->next);
			pQNode = *ppQNode;
		}
	}
	return NULL;
}

void afe_port_calc_cmn_group_delay(afe_dev_port_t *pDevPort)
{
	uint64_t cmn_delay = 0;

	// For uncompressed data only, we have algos in AFE port processing
	if((AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
			&& (FALSE == IS_IT_TX_PORT_AFE_ID(pDevPort->intf)))
	{
		// limiter delay, convert AFE_LIM_DELAY (Q15) into integer
		cmn_delay += (((uint64_t) AFE_LIM_DELAY_1MS_Q15 * NUM_US_PER_SEC) >> 15);
	}

	pDevPort->port_cmn_group_delay_us = cmn_delay;

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port_cmn_group_delay_us = %lu", (uint32_t) cmn_delay);

	return;
}

void afe_port_calc_client_gp_delay_us(afe_client_info_t *client)
{
	uint64_t pop_sup_delay = 0, samp_rate_conv_delay = 0;

	/**<Remove previous client_group_delay and add latest delay.*/
	if (NULL != client->afe_client.afe_dp_delay_ptr)
	{
		*(client->afe_client.afe_dp_delay_ptr) -= client->client_gp_delay_us;
	}

	/**<Re-calculate the delay afresh.*/
	client->client_gp_delay_us = 0;

	/**<If pop-suppressor is present then update pop-
	 * suppressor algorithmic delay in client group delay.*/
	if(NULL != client->pop_suppressor_module_ptr)
	{
		afe_pop_suppressor_get_delay(client->pop_suppressor_module_ptr, &pop_sup_delay);
		client->client_gp_delay_us += pop_sup_delay;
	}

	/**<Client resampler delay.*/
	sample_rate_conv_get_delay(&client->samp_rate_conv, &samp_rate_conv_delay);
	client->client_gp_delay_us += samp_rate_conv_delay;

#ifdef DEBUG_AFE
	MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO, "client group_delay_us = %ld%ld, rs_delay = %ld, rs_buf_delay = %ld, Client: 0x%lx",\
			(uint32_t) ((client->client_gp_delay_us)>>32), (uint32_t)(client->client_gp_delay_us), client->samp_rate_conv.delay_in_us, client->samp_rate_conv.rs_buff_delay_us, (uint32_t) client);
#endif //DEBUG_AFE
	/**<Accumulate newly calculated client delay.*/
	if (NULL != client->afe_client.afe_dp_delay_ptr)
	{
		*(client->afe_client.afe_dp_delay_ptr) += client->client_gp_delay_us;
	}
}

void afe_port_apply_capture_time_stamp(afe_dev_port_t *pDevPort, afe_client_info_t *client)
{
	uint64_t time_stamp = pDevPort->port_av_timestamp;
	uint64_t block_transfer_time_us;

	/* If resampler uses buffering, then we need to update the delay.
	 * Resampler buffering delay will varies based on where we are
	 * currently in terms of buffering the frames.
	 */
	if(RS_NO_BUFFERING != client->samp_rate_conv.buff_scheme)
	{
		afe_port_calc_client_gp_delay_us(client);
	}

	block_transfer_time_us = ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)
			/ pDevPort->sample_rate);

	if(IS_IT_TX_PORT_AFE_ID(pDevPort->intf))
	{
		if(AFE_TX_OUT == client->afe_client.data_path)
		{
			// currently this is used for side tone path where we can have dma buffer and client specific delays only
			time_stamp -= ( block_transfer_time_us + client->client_gp_delay_us);
		}
		else
		{
			// this is used for all Tx clients at Tx port. Here we need to consider cmn group delay and client specific delay
			time_stamp -= (block_transfer_time_us + pDevPort->port_cmn_group_delay_us
					+ client->client_gp_delay_us);
		}
	}
	else /* Rx port */
	{
		if(AFE_PP_OUT == client->afe_client.data_path)
		{
			// currently this is for audio+voice feedback
			time_stamp += (block_transfer_time_us - client->client_gp_delay_us);
		}
		else
		{
			// for rest of the Tx clients in Rx port
			time_stamp += (block_transfer_time_us + pDevPort->port_cmn_group_delay_us
					- client->client_gp_delay_us);
		}
	}

	client->ps_curr_pcm_buff->nFlag = AFE_PORT_CLIENT_BUF_VALID_TIME_STAMP_FLAG;
	client->ps_curr_pcm_buff->ullTimeStamp = time_stamp;

	return;
}

ADSPResult afe_port_set_group_delay(afe_dev_port_t *pDevPort)
{
	ADSPResult result = ADSP_EOK;
	uint64_t group_delay_us = 0, dp_delay = 0;
	bufQList            *sClientList = NULL;
	afe_client_info_t *client = NULL;
	uint64_t 				circ_buf_delay_us = 0;

	// group delay = algo delay + dma buf delay + client specific delay
	if(IS_SPDIF_PORT_AFE_ID(pDevPort->intf))
	{
		group_delay_us = (pDevPort->port_cmn_group_delay_us + 2*((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate));
	}
	else if(IS_RT_PROXY_PORT_AFE_ID(pDevPort->intf))
	{
		afe_driver_rt_proxy_circ_buf_delay(pDevPort, &circ_buf_delay_us);
		group_delay_us = pDevPort->port_cmn_group_delay_us + circ_buf_delay_us;
	}
	else if(IS_RIVA_PORT_AFE_ID(pDevPort->intf))
	{
		afe_driver_riva_circ_buf_delay(pDevPort, &circ_buf_delay_us);
		group_delay_us = pDevPort->port_cmn_group_delay_us + circ_buf_delay_us;
	}
	else if(IS_USBA_PORT_AFE_ID(pDevPort->intf))
	{
		uint64_t 				data_path_delay_us = 0;
		afe_driver_usba_get_data_path_delay(pDevPort, &data_path_delay_us);
		group_delay_us = pDevPort->port_cmn_group_delay_us + data_path_delay_us;
	}
	else
	{
		group_delay_us = (pDevPort->port_cmn_group_delay_us + ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate));
	}

	if (NULL != pDevPort->port_enc_dec_ptr)
	{
		uint32_t delay = 0;
		if (pDevPort->dir == RX_DIR)
		{
			afe_port_encoder_params_t *enc_param_ptr = (afe_port_encoder_params_t*)pDevPort->port_enc_dec_ptr;
			delay = enc_param_ptr->encoder_delay;
		}
		else
		{
			afe_port_decoder_params_t *dec_param_ptr = (afe_port_decoder_params_t*)pDevPort->port_enc_dec_ptr;
			delay = dec_param_ptr->decoder_delay;
		}

		group_delay_us += delay;

		MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "group_delay_us = %lu, enc/dec delay %lu", (uint32_t) group_delay_us, delay);
	}

	// Providing the Device SW delay (AFE delay) and Device HW delay to clients who request
	// these by passing non null pointers for respective delay pointers in client structure
	if (NULL != pDevPort->afe_tx_client_list_ptr)
	{
		sClientList = pDevPort->afe_tx_client_list_ptr;
		while (sClientList != NULL)
		{
			client = (afe_client_info_t *)sClientList->element;
			if (NULL != client->afe_client.afe_dp_delay_ptr)
			{
				dp_delay = (group_delay_us + client->client_gp_delay_us);
				*(client->afe_client.afe_dp_delay_ptr) = dp_delay;
				MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "group_delay_us = %lu, TxClient: 0x%lx", (uint32_t) dp_delay, (uint32_t) client);
			}
			if (NULL != client->afe_client.afe_dev_hw_delay_ptr)
			{
				*(client->afe_client.afe_dev_hw_delay_ptr) = pDevPort->dev_hw_delay_us;
			}
			sClientList = sClientList->next;
		}
	}

	if (NULL != pDevPort->afe_rx_client_list_ptr)
	{
		sClientList = pDevPort->afe_rx_client_list_ptr;
		while (sClientList != NULL)
		{
			client = (afe_client_info_t *)sClientList->element;
			if (NULL != client->afe_client.afe_dp_delay_ptr)
			{
				dp_delay = (group_delay_us + client->client_gp_delay_us);
				*(client->afe_client.afe_dp_delay_ptr) = dp_delay;
				MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "group_delay_us = %lu, RxClient: 0x%lx", (uint32_t) dp_delay, (uint32_t) client);
			}
			if (NULL != client->afe_client.afe_dev_hw_delay_ptr)
			{
				*(client->afe_client.afe_dev_hw_delay_ptr) = pDevPort->dev_hw_delay_us;
			}
			sClientList = sClientList->next;
		}
	}

	return result;
}


/**
 * This function is to get render time stamp of the pcm data*
 * @param[in] pDevPort, pointer to AFE port structure
 * @param[in] client, pointer to afe client
 * @param[out] time_stamp, render time stamp
 * @return None
 * */

void afe_port_get_render_time_stamp(afe_dev_port_t *pDevPort, afe_client_info_t *client,
		uint64_t *time_stamp)
{
	uint64_t block_transfer_time_us = ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)
			/ pDevPort->sample_rate);

	if(NULL != time_stamp)
	{
		/* If resampler uses buffering, then we need to update the delay.
		 * Resampler buffering delay will varies based on where we are
		 * currently in terms of buffering the frames.
		 */
		if(RS_NO_BUFFERING != client->samp_rate_conv.buff_scheme)
		{
			afe_port_calc_client_gp_delay_us(client);
		}

		*time_stamp = (pDevPort->port_av_timestamp + block_transfer_time_us
				+ pDevPort->port_cmn_group_delay_us + client->client_gp_delay_us);
	}

	return;
}

/**
 * This function is to init the buffer latency data structures
 * @param[in] pDevPort, pointer to AFE port structure
 * @return ADSPResult
 * */
ADSPResult afe_cont_buf_latency_init(afe_dev_port *pDevPort)
{
	ADSPResult result = ADSP_EOK;
	afe_cont_buf_latency_log_info_t	*cont_buf_latency_log_info_ptr = NULL;
	afe_cont_buf_latency_log_t		*cont_buf_latency_log_ptr = NULL;
	afe_cont_buf_latency_stats_v1_t 	*cont_buf_latency_stats_ptr = NULL;

	if (NULL == (cont_buf_latency_log_info_ptr = (afe_cont_buf_latency_log_info_t *) qurt_elite_memory_malloc((sizeof(afe_cont_buf_latency_log_info_t)), QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate memory for buf latency struct, port_id: 0x%X", pDevPort->intf);
		pDevPort->cont_buf_latency_log_info_ptr = NULL;
		return ADSP_ENOMEMORY;
	}

	pDevPort->cont_buf_latency_log_info_ptr = cont_buf_latency_log_info_ptr;


	/* Init the logging utility */
	if(ADSP_EOK != (result = algo_data_logging_init(&pDevPort->cont_buf_latency_log_info_ptr->logging_ptr)) )
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize logging utility for port_id: 0x%X", pDevPort->intf);
		return result;
	}

	cont_buf_latency_log_ptr = &pDevPort->cont_buf_latency_log_info_ptr->cont_buf_latency_log;
	cont_buf_latency_log_ptr->algo_log_header.module_id = AFE_MODULE_AUDIO_DEV_INTERFACE;
	cont_buf_latency_log_ptr->algo_log_header.param_id = AFE_PARAM_ID_CONT_BUF_LATENCY_STATS;
	cont_buf_latency_log_ptr->algo_log_header.param_size = sizeof(afe_cont_buf_latency_log_t);
	cont_buf_latency_log_ptr->algo_log_header.reserved = 0;

	cont_buf_latency_stats_ptr = &cont_buf_latency_log_ptr->cont_buf_latency_stats;
	cont_buf_latency_stats_ptr->cont_buf_latency_stats_minor_version = AFE_API_VERSION_CONT_BUF_LATENCY_STATS;

	/* Register for logging buffer latencies */

	if(ADSP_FAILED(result = algo_data_logging_register(pDevPort->cont_buf_latency_log_info_ptr->logging_ptr,
			((uint64_t)(pDevPort->int_samples_per_period * 1000)/pDevPort->sample_rate),
			((uint64_t)(pDevPort->int_samples_per_period * 1000)/pDevPort->sample_rate),
			sizeof(afe_cont_buf_latency_log_t),
			QXDM_LOG_CODE_AFE_ALGO_CALIB,
			&(pDevPort->cont_buf_latency_log_info_ptr->log_client_hdl))))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register for continous latency logging: %d", result);
		return ADSP_EFAILED;
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort 0x%x: continuous latency inited successfully", pDevPort->intf);

	return result;

}

/**
 * This function is to deinit the buffer latency data structures
 * @param[in] pDevPort, pointer to AFE port structure
 * @return ADSPResult
 * */
ADSPResult afe_cont_buf_latency_deinit(afe_dev_port *pDevPort)
{
	ADSPResult result = ADSP_EOK;

	if (NULL != pDevPort->cont_buf_latency_log_info_ptr)
	{
		if(NULL != pDevPort->cont_buf_latency_log_info_ptr->logging_ptr)
		{
			algo_data_logging_deregister(pDevPort->cont_buf_latency_log_info_ptr->logging_ptr, pDevPort->cont_buf_latency_log_info_ptr->log_client_hdl);
			/* DeInit the logging utility */
			algo_data_logging_deinit(&pDevPort->cont_buf_latency_log_info_ptr->logging_ptr);
		}

		qurt_elite_memory_free(pDevPort->cont_buf_latency_log_info_ptr);
		pDevPort->cont_buf_latency_log_info_ptr = NULL;
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort 0x%x: continuous latency deinited successfully", pDevPort->intf);
	return result;

}

/**
 * This function is to log the buffer latency data
 * @param[in] pDevPort, pointer to AFE port structure
 * @param[in] pClientInfo, pointer to afe client
 * @return ADSPResult
 * */
ADSPResult afe_cont_buf_latency_log(afe_dev_port *pDevPort, afe_client_info_t *pClientInfo)
{
	ADSPResult result = ADSP_EOK;

	/* If 0x17D8 log code is enabled, log the buffer time stamp and render time stamp to diag */
	if(ADSP_EOK == algo_log_status(QXDM_LOG_CODE_AFE_ALGO_CALIB))
	{
		/* Latency logging should be enabled only when there is one client */
		/* Log only if there is valid time stamp from client */
		if((1 == pDevPort->num_client) && (pClientInfo->ps_curr_pcm_buff->nFlag & ELITE_DATA_BUFFER_TIME_STAMP_BITMASK))
		{
			afe_cont_buf_latency_log_info_t *cont_buf_latency_log_info_ptr;
			afe_cont_buf_latency_stats_v1_t *cont_buf_latency_stats_ptr;

			cont_buf_latency_log_info_ptr = pDevPort->cont_buf_latency_log_info_ptr;
			cont_buf_latency_stats_ptr = &cont_buf_latency_log_info_ptr->cont_buf_latency_log.cont_buf_latency_stats;

			/* get the buffer time stamp */
			cont_buf_latency_stats_ptr->buf_time_stamp_msw = (uint32_t)(pClientInfo->ps_curr_pcm_buff->ullTimeStamp >> 32);
			cont_buf_latency_stats_ptr->buf_time_stamp_lsw = (uint32_t)(pClientInfo->ps_curr_pcm_buff->ullTimeStamp & 0xFFFFFFFF);

			/* get the render time stamp */
			uint64_t time_stamp = 0;
			afe_port_get_render_time_stamp(pDevPort, pClientInfo, &time_stamp);

			cont_buf_latency_stats_ptr->rndr_time_stamp_msw = (uint32_t)(time_stamp >> 32);
			cont_buf_latency_stats_ptr->rndr_time_stamp_lsw = (uint32_t)(time_stamp & 0xFFFFFFFF);

			//	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort 0x%x: buf_time_stamp 0x%lx%lx", pDevPort->intf, cont_buf_latency_stats_ptr->buf_time_stamp_msw, cont_buf_latency_stats_ptr->buf_time_stamp_lsw);
			//	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort 0x%x: rndr_time_stamp 0x%lx%lx", pDevPort->intf, cont_buf_latency_stats_ptr->rndr_time_stamp_msw, cont_buf_latency_stats_ptr->rndr_time_stamp_lsw);

			/* do the logging */
			algo_data_log_now(cont_buf_latency_log_info_ptr->logging_ptr,
					(int8_t *)&cont_buf_latency_log_info_ptr->cont_buf_latency_log,
					sizeof(afe_cont_buf_latency_log_t),
					cont_buf_latency_log_info_ptr->log_client_hdl);
		}
	}

	return result;
}
