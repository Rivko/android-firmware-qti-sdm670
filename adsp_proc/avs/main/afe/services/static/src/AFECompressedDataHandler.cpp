/*========================================================================
   This file contains AFE Port compressed data handlers implementation

  Copyright (c) 2009-2012,2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFECompressedDataHandler.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "AFEPortManagement.h"

/*==========================================================================
  Function declarations
  ========================================================================== */
void afe_generate_null_data_bursts(int8_t* out_buf_ptr, int16_t num_data_bytes, afe_port_data_type data_type);
ADSPResult afe_port_render_compressed_data(afe_dev_port_t *pDevPort);
ADSPResult afe_port_forward_compressed_data(afe_dev_port_t *pDevPort);
ADSPResult afe_port_get_client_compressed_data(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo, int8_t* out_buf_ptr, uint16_t num_samp);

ADSPResult afe_port_send_compressed_data_to_client(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo, \
		int8_t* out_buf_ptr, uint16_t num_data_bytes);


/*==========================================================================
  Function definitions
  ========================================================================== */
ADSPResult afe_port_render_compressed_data_thread_safe(void *vpDevPort)
{
	ADSPResult result = ADSP_EOK;
	afe_dev_port_t *pDevPort = (afe_dev_port_t*) vpDevPort;
	qurt_elite_mutex_lock( &(pDevPort->port_mutex) );
	result = afe_port_render_compressed_data(pDevPort);
	qurt_elite_mutex_unlock( &(pDevPort->port_mutex) );
	return result;
}

ADSPResult afe_port_render_compressed_data(afe_dev_port_t *pDevPort)
{
	bufQList            *sClientList;
	uint16_t            nNumDataBytes;
	int8_t              *mem_start_addr;
	afe_client_info_t   *client;
	bool_t              is_compressed_client_present = FALSE;// indicates whether compressed client is present or not
	ADSPResult          result = ADSP_EOK;

	if (AFE_PORT_STATE_RUN != pDevPort->port_state)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port signal when it is not in RUN state.");
		return ADSP_EOK;
	}

	afe_port_update_timestamp(pDevPort);
	afe_port_update_avt_drift(pDevPort);

	// reading compressed data from clients happens for all channels once
	nNumDataBytes = (pDevPort->int_samples_per_period)*(pDevPort->channels)*(pDevPort->bytes_per_channel);
	mem_start_addr = (int8_t *) (pDevPort->working_buf_1);

	//Prepare DmaSamples by going through each client
	sClientList = pDevPort->afe_rx_client_list_ptr;
	while (sClientList != NULL)
	{
		client = (afe_client_info_t *)sClientList->element;
		if ((AFE_RX_IN == client->afe_client.data_path))
		{
			// handles only one compressed client from this port task
			afe_port_get_client_compressed_data(pDevPort, client, mem_start_addr, nNumDataBytes);
			is_compressed_client_present= TRUE;
			break;
		}
		sClientList = sClientList->next;
	}

	if(!is_compressed_client_present)
	{
		// no client is added, generate NULL data bursts
		afe_generate_null_data_bursts(mem_start_addr, nNumDataBytes, pDevPort->port_data_type);
	}

#ifdef SIM
	//FILE *fp;
	//fp = fopen("afe_compressed_data.raw","a");
	//fwrite(mem_start_addr,1,nNumDataBytes,fp);
	//fclose(fp);
	//int64_t curr_time = qurt_elite_timer_get_time();
	//MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE write: %d, %d", (uint32_t)(curr_time>>32), (uint32_t)curr_time);
#endif //SIM

	if (NULL != pDevPort->avsync_test_ptr)
	{
		afe_port_collect_avsync_stats(pDevPort, mem_start_addr);
	}

	/* Log interleaved data */
	/* Call to log the compressed bit stream data */
	afe_port_data_logging (pDevPort, mem_start_addr, pDevPort->is_interleaved);

	// De-interleave the data if end device is expecting data in de-interleaved fashion
	// Compressed path have the data in interleaved always
	if (FALSE == pDevPort->is_interleaved)
	{
		//deinterleave mem_start_addr_copy and store in mem_start_addr
		afe_port_de_interleave_samples(mem_start_addr, pDevPort->working_buf_2,
				pDevPort->channels, pDevPort->int_samples_per_period, pDevPort->bytes_per_channel);

		mem_start_addr = pDevPort->working_buf_2;
	}

	// here we need to give numSamples per channel as last argument as the DAL layer code considers
	// the channel configuration internally while reading the samples from svc buffer
	if (NULL != pDevPort->afe_dev_process)
	{
		result =  pDevPort->afe_dev_process(pDevPort, (int16_t *)mem_start_addr, pDevPort->int_samples_per_period);
	}
	else if(NULL != pDevPort->afe_drv_ftbl)
	{
		if(pDevPort->afe_drv_ftbl->process)
		{
			result =  pDevPort->afe_drv_ftbl->process(pDevPort->afe_drv, (int16_t *)mem_start_addr, pDevPort->int_samples_per_period);
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

	return result;
}

ADSPResult afe_port_get_client_compressed_data(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo, int8_t* out_buf_ptr, uint16_t num_data_bytes)
{
	ADSPResult           result = ADSP_EOK;
	uint32_t             unChannelStatus;
	elite_msg_any_t      myDataQMsg;
	uint16_t              min_bytes;

	while (1)
	{
		// take the minimum samples needed for or available for filling the buffer
		min_bytes = num_data_bytes;
		if (num_data_bytes > pClientInfo->remaining_samples)
		{
			min_bytes = pClientInfo->remaining_samples;
		}

		if (NULL != pClientInfo->curr_buff_ptr)
		{
			/* Though the port buffer is allocated for 8 channel, 24-bit, restricting the destination buffer size
             to per interrupt length to also act as an error check */
			memscpy(out_buf_ptr,
					num_data_bytes,
					pClientInfo->curr_buff_ptr,
					min_bytes);

			out_buf_ptr += min_bytes;
			pClientInfo->curr_buff_ptr += min_bytes;
			pClientInfo->remaining_samples -= min_bytes;
			num_data_bytes -= min_bytes;

			// relive the buffer if there are no samples to consume
			if (0 == pClientInfo->remaining_samples)
			{
				//relieve buffer back to client
				if (ADSP_FAILED(result = elite_msg_push_payload_to_returnq(pClientInfo->ps_curr_pcm_buff->pBufferReturnQ, \
						(elite_msg_any_payload_t*) pClientInfo->ps_curr_pcm_buff)))
				{
					MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFEPortMng: ChannelSelf thread encountered AudQPushBack error = %d!!\n", result);
					return ADSP_EFAILED;
				}
				pClientInfo->curr_buff_ptr = NULL;
			}
		}

		// return if we filled the buffer
		if (0 == num_data_bytes)
		{
			return ADSP_EOK;
		}

		// Not enough samples, so read from client queue
		qurt_elite_queue_t *     client_dataQ      = pClientInfo->this_client_manager.dataQ;
		qurt_elite_channel_t     *dataQ_chan       = qurt_elite_queue_get_channel(client_dataQ);
		uint32_t                 dataQ_chan_bit    = qurt_elite_queue_get_channel_bit(client_dataQ);
		uint16_t                 data_is_available = 0;

		//Time for new samples from Client's dataQueue
		while( (unChannelStatus = qurt_elite_channel_poll(dataQ_chan, dataQ_chan_bit)) != 0 )
		{
			//pop dataQ until it's empty or until we have data buffer.
			result = qurt_elite_queue_pop_front(client_dataQ, (uint64_t*)&myDataQMsg);

			if (ADSP_FAILED(result))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "data is not available on mq %d qurt_elite_queue_pop_front returned with error: %d\n", \
						(int)(client_dataQ), result);
				break;
			}

			if (ELITE_DATA_BUFFER == myDataQMsg.unOpCode )
			{
				data_is_available = 1;
				break;
			}
			//handle data cmd
			afe_port_data_cmd(pClientInfo, pDevPort, &myDataQMsg, FALSE);
		}

		/* If data is available from client */
		if (data_is_available == 1)
		{
			//update the instance status
			pClientInfo->ps_curr_pcm_buff    =  (elite_msg_data_buffer_t*) myDataQMsg.pPayload;
			pClientInfo->curr_buff_ptr       =  (int8_t *)&(pClientInfo->ps_curr_pcm_buff->nDataBuf);
			pClientInfo->buf_size            =  ((pClientInfo->ps_curr_pcm_buff)->nActualSize);
			pClientInfo->remaining_samples   =  pClientInfo->buf_size;

			if(pClientInfo->remaining_samples & 1)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Odd buffer size %lu from ASM/stream router for port id 0x%x. Ignoring the samples and relieving the buffer", pClientInfo->buf_size, pDevPort->intf);
				// Buffer relieving will be done in the next iteration of while loop
				pClientInfo->remaining_samples = 0;
			}
			else
			{
				afe_cont_buf_latency_log(pDevPort, pClientInfo);
			}
			continue;
		}
		else // (data_is_available == 0) //data is not available from client
		{
			uint64_t currTime, diff;
			pClientInfo->under_run_counter++;

			// Use the wallk clk time just to avoid the flood of under-run messages
			currTime = qurt_elite_timer_get_time();

			diff = currTime - pClientInfo->prev_time;

			if (diff >= AFE_UNDERRUN_TIME_THRESH)
			{
				MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort intf=%x, dir=%d: Compressed data not available in Rx path from client 0x%lX. CurrTime-PrevTime = %ld us. Num Underrun = %lu!!\n",\
						pDevPort->intf,pDevPort->dir, pClientInfo->client_id, (uint32_t) diff,pClientInfo->under_run_counter);

				pClientInfo->prev_time = currTime;
				pClientInfo->under_run_counter = 0;
			}
			afe_generate_null_data_bursts(out_buf_ptr, num_data_bytes,pDevPort->port_data_type);
			break;
		}
	}

	return ADSP_EOK;
}

void afe_generate_null_data_bursts(int8_t* out_buf_ptr, int16_t num_data_bytes, afe_port_data_type data_type)
{

	if(AFE_IEC61937_COMPRESSED_DATA_PORT == data_type)
	{
		int16_t *ptr = (int16_t *)out_buf_ptr;
		int16_t pa = 0xF872, pb = 0x4E1F, pc = 0xE000, pd = 0x0000;
		//writing 8 bytes at a time, so divide by 8;
		for (int i=0; i < (num_data_bytes>>3); i++)
		{
			*(ptr++)    = pa;
			*(ptr++)    = pb;
			*(ptr++)    = pc;
			*(ptr++)    = pd;
		}
	}
	else if(AFE_DSD_DOP_COMPRESSED_DATA_PORT == data_type)
	{
		//For QCOM DSD over PCM (DOP), the pause word is required to be 0x69
		memset(out_buf_ptr, 0x69, num_data_bytes);
	}
	else if(AFE_GENERIC_COMPRESSED_DATA_PORT == data_type)
	{
		memset(out_buf_ptr, 0, num_data_bytes);
	}
	else
	{
		//print unsupported error message and return.
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe port has incorrect data type %ld", data_type);
		return;
	}
}


ADSPResult afe_port_forward_compressed_data_thread_safe(void *vpDevPort)
{
	ADSPResult result = ADSP_EOK;
	afe_dev_port_t *pDevPort = (afe_dev_port_t*) vpDevPort;
	qurt_elite_mutex_lock( &(pDevPort->port_mutex) );
	result = afe_port_forward_compressed_data(pDevPort);
	qurt_elite_mutex_unlock( &(pDevPort->port_mutex) );
	return result;
}

ADSPResult afe_port_forward_compressed_data(afe_dev_port_t *pDevPort)
{
	bufQList           *sClientList;
	ADSPResult          result = ADSP_EOK;
	afe_client_info_t   *client;
	uint32_t       nNumDataBytes;
	int8_t  *mem_start_addr;

	/* Read the samples from dma buffer to port memory. */
	if(NULL != pDevPort->afe_dev_process)
	{
		result =  pDevPort->afe_dev_process(pDevPort, (int16_t *)pDevPort->working_buf_1, pDevPort->int_samples_per_period);
	}
	else if(NULL != pDevPort->afe_drv_ftbl)
	{
		if(pDevPort->afe_drv_ftbl->process)
		{
			result =  pDevPort->afe_drv_ftbl->process(pDevPort->afe_drv, (int16_t *)pDevPort->working_buf_1, pDevPort->int_samples_per_period);
		}
		else
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_drv_ftbl fowarding process function has not been configured in port 0x%x", pDevPort->intf);
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe fowarding process function has not been configured in port 0x%x", pDevPort->intf);
	}


	nNumDataBytes = (pDevPort->int_samples_per_period)*(pDevPort->channels)*(pDevPort->bytes_per_channel);
	mem_start_addr = pDevPort->working_buf_1;

	/* Interleave if device data is de-interleaved. Compressed data path is interleaved */
	if (FALSE == pDevPort->is_interleaved)
	{
		afe_port_interleave_samples(mem_start_addr, pDevPort->working_buf_2, pDevPort->channels,
				pDevPort->int_samples_per_period, pDevPort->bytes_per_channel);

		mem_start_addr = pDevPort->working_buf_2;
	}

	/* Log interleaved data */
	/* Call to log the compressed bit stream data */
	afe_port_data_logging (pDevPort, mem_start_addr, pDevPort->is_interleaved);

	afe_port_update_timestamp(pDevPort);
	afe_port_update_avt_drift(pDevPort);

	if (NULL != pDevPort->avsync_test_ptr)
	{
		afe_port_collect_avsync_stats(pDevPort, mem_start_addr);
	}

	sClientList = pDevPort->afe_tx_client_list_ptr;
	while (sClientList != NULL)
	{
		client = (afe_client_info_t *)sClientList->element;
		if ((AFE_TX_OUT == client->afe_client.data_path)   &&  (ADSP_EOK == afe_port_client_data_type_validation(pDevPort,&(client->afe_client))))
		{
			// handles only one compressed client from this port task
			afe_port_send_compressed_data_to_client(pDevPort, client, mem_start_addr, nNumDataBytes);
			break;
		}
		else
		{

			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ERROR: afe port datatype %ld mismatches client date type %ld",
					pDevPort->port_data_type, client->afe_client.data_type);
		}
		sClientList = sClientList->next;
	}

	return result;
}

ADSPResult afe_port_send_compressed_data_to_client(afe_dev_port_t *pDevPort, afe_client_info_t *pClientInfo, \
		int8_t* out_buf_ptr, uint16_t num_data_bytes)
{
	uint32_t min_bytes;
	int8_t *ptr_dst_client_buf;
	qurt_elite_bufmgr_node_t outBufMgrNode;
	elite_msg_any_t*        pPeerDataQMsg;
	ADSPResult result = ADSP_EOK;

	/* If the client is disabled, omit this client */
	if (AFE_CLIENT_ENABLE != pClientInfo->state)
	{
		return ADSP_EOK;
	}

	while (1)
	{
		if (pClientInfo->remaining_samples == 0)
		{
			uint32_t                   unChannelStatus;
			qurt_elite_bufmgr_node_t   outBufMgrNode;
			elite_msg_data_buffer_t    *psPcmBuf;


			unChannelStatus = qurt_elite_channel_poll(qurt_elite_queue_get_channel(pClientInfo->bufQ), qurt_elite_queue_get_channel_bit(pClientInfo->bufQ));
			if (unChannelStatus)
			{
				if (ADSP_EOK == (result = qurt_elite_queue_pop_front(pClientInfo->bufQ, (uint64_t*)&outBufMgrNode)))
				{
					psPcmBuf = (elite_msg_data_buffer_t *)outBufMgrNode.pBuffer;
					//MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: TX obtained buf: %X", (uint32_t) psPcmBuf);
					pClientInfo->remaining_samples = (pClientInfo->actual_buf_size);
					pClientInfo->ps_curr_pcm_buff = psPcmBuf;
					pClientInfo->ps_curr_pcm_buff->nActualSize = pClientInfo->actual_buf_size;
					pClientInfo->curr_buff_ptr = (int8_t *) &(psPcmBuf->nDataBuf);

					pClientInfo->ps_curr_pcm_buff->nFlag = AFE_PORT_CLIENT_BUF_VALID_TIME_STAMP_FLAG;
					pClientInfo->ps_curr_pcm_buff->ullTimeStamp = pDevPort->port_av_timestamp;
				}
			}
			else
			{
				uint64_t currTime, diff;
				pClientInfo->under_run_counter++;

				// Use the wallk clk time just to avoid the flood of under-run messages
				currTime = qurt_elite_timer_get_time();
				diff = currTime - pClientInfo->prev_time;
				if (diff >= AFE_UNDERRUN_TIME_THRESH)
				{
					MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfePort intf=%x, dir=%d: Audio buffer not available in Tx path for client 0x%lX. CurrTime-PrevTime = %ld us. Num Underrun = %lu!!\n",\
							pDevPort->intf,pDevPort->dir, pClientInfo->client_id,  (uint32_t)diff,pClientInfo->under_run_counter);
					pClientInfo->prev_time = currTime;
					pClientInfo->under_run_counter = 0;
				}
				return(ADSP_EOK);
			}
		} //if(pClientInfo->remaining_samples == NULL)

		// take the minimum bytes needed for or available for filling the buffer
		min_bytes = num_data_bytes;
		if (num_data_bytes > pClientInfo->remaining_samples)
		{
			min_bytes = pClientInfo->remaining_samples;
		}

		ptr_dst_client_buf = pClientInfo->curr_buff_ptr;

		memscpy(ptr_dst_client_buf,
				pClientInfo->remaining_samples, /* Remaining dst buf size in bytes */
				out_buf_ptr,
				min_bytes);

		out_buf_ptr += min_bytes;

		pClientInfo->curr_buff_ptr += min_bytes;
		pClientInfo->remaining_samples -= min_bytes;
		num_data_bytes -= min_bytes;

		if (pClientInfo->remaining_samples == 0)
		{
			/* Send output buffers to downstream service. */
			outBufMgrNode.pReturnQ = pClientInfo->bufQ;
			outBufMgrNode.pBuffer = pClientInfo->ps_curr_pcm_buff;
			//MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEPortMng: TX sending buf: %X", (uint32_t) pClientInfo->ps_curr_pcm_buff);
			pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(
					&outBufMgrNode,
					ELITE_DATA_BUFFER,
					NULL,                   /* do not need response */
					0,                      /* token */
					0);                     /* do not care response result*/
			result = qurt_elite_queue_push_back(pClientInfo->afe_client.afe_client_handle->dataQ, (uint64_t*)pPeerDataQMsg);

			if(ADSP_FAILED(result))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to deliver buffer dowstream. Dropping!\n");
				(void) elite_msg_push_payload_to_returnq(pClientInfo->bufQ, (elite_msg_any_payload_t*) outBufMgrNode.pBuffer);
			}
		}

		if(num_data_bytes == 0)
		{
			return ADSP_EOK;
		}
	}

	return ADSP_EOK;
}
