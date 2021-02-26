/*========================================================================
   This file contains logging utilities used by multiple Afe Ports.

  Copyright (c) 2009-2012, 2016 QUALCOMM Technologies, Inc. All Rights Reserved. 
  QUALCOMM Technologies, Inc Proprietary. Export of this technology or 
  software is regulated by the U.S. Government, Diversion contrary to 
  U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFELoggingUtils.cpp#1 $
 ====================================================================== */

/*============================================================================
 *                       INCLUDE FILES FOR MODULE
 *==========================================================================*/
#include "qurt_elite.h"
#include "AFELoggingUtils.h"
#include "AFEPortManagement.h"
#include "audio_basic_op.h"

#define GROUP_DEV_LOGGING_ENABLE
#undef GROUP_DEV_LOGGING_ENABLE


#ifdef GROUP_DEV_LOGGING_ENABLE
//the group PCM logging will be failed if doing logging for every DMA interrupt.
//belows are used to control group PCM for every 1 over 10 DMA interrupts

//for MI2S group (AFE_GROUP_DEVICE_ID_MI2S_RX, AFE_GROUP_DEVICE_ID_MI2S_TX)
static uint32_t group_q_mi2s_tx_log_skipping = 0;
static uint32_t group_q_mi2s_rx_log_skipping = 0;

//for Primary I2S group (AFE_GROUP_DEVICE_ID_PRIMARY_RX, AFE_GROUP_DEVICE_ID_PRIMARY_TX)
static uint32_t group_p_mi2s_tx_log_skipping = 0;
static uint32_t group_p_mi2s_rx_log_skipping = 0;

//for tdm group devices
static uint32_t group_primary_tdm_rx_log_skipping = 0;
static uint32_t group_primary_tdm_tx_log_skipping = 0;

static uint32_t group_secondary_tdm_rx_log_skipping = 0;
static uint32_t group_secondary_tdm_tx_log_skipping = 0;

static uint32_t group_tertiary_tdm_rx_log_skipping = 0;
static uint32_t group_tertiary_tdm_tx_log_skipping = 0;

static uint32_t group_quaternary_tdm_rx_log_skipping = 0;
static uint32_t group_quaternary_tdm_tx_log_skipping = 0;


#endif


void afe_port_data_logging_init(afe_dev_port_t *pDevPort)
{
	uint16_t unLogSizePerInt;
	uint32_t totalLogSize = 0;

	unLogSizePerInt = pDevPort->int_samples_per_period * pDevPort->channels * pDevPort->bytes_per_channel;

	/** For port sampling rate > 192kHz, the size of the logging buffer
	    is reduced to 1 ms. This helps in reducing the logging overhead 
	    during interrupt processing
	*/
	if(pDevPort->sample_rate > AFE_PORT_SAMPLE_RATE_192K )
	{
		totalLogSize = AFE_PCM_BUF_SIZE_1MS * unLogSizePerInt;
	}
	else
	{
		totalLogSize = AFE_PCM_BUF_SIZE_10MS * unLogSizePerInt;
	}

	// seq num for session id
	(pDevPort->data_log.port_session_id_for_log)++;

	pDevPort->data_log.bytes_per_int_all_ch = unLogSizePerInt;
	pDevPort->data_log.total_log_size = totalLogSize;
	pDevPort->data_log.log_buf_ptr = NULL;
	pDevPort->data_log.log_buf_ptr = (int16_t *)qurt_elite_memory_malloc(totalLogSize, QURT_ELITE_HEAP_DEFAULT);
	if(NULL == pDevPort->data_log.log_buf_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port %x Error allocating memory for log buffer", pDevPort->intf);
	}
	pDevPort->data_log.log_size_used = 0;
}

void afe_port_data_logging_deinit(afe_dev_port_t *pDevPort)
{
	elite_log_info log_info_var;
	pcm_data_info *pcm_data = &(log_info_var.data_info.pcm_data_fmt);

	/* If all the log buffer size has been used, do commit the log buffer. */
	if(pDevPort->data_log.log_size_used > 0)
	{
		uint16_t logId;
		logId = AUDIOLOG_CHAN_PCM_RX_INTERNAL_AFE + pDevPort->intf;

		log_info_var.qxdm_log_code = QXDM_LOG_CODE_AFE_RX_TX_OUT;
		log_info_var.buf_ptr = (uint8_t *)pDevPort->data_log.log_buf_ptr;
		log_info_var.buf_size = pDevPort->data_log.log_size_used;
		log_info_var.session_id = pDevPort->data_log.port_session_id_for_log;
		log_info_var.log_tap_id = logId;
		log_info_var.log_time_stamp = qurt_elite_timer_get_time();
		log_info_var.data_fmt = ELITE_LOG_DATA_FMT_PCM;
		pcm_data->num_channels = pDevPort->channels;
		pcm_data->sampling_rate = pDevPort->sample_rate;
		pcm_data->bits_per_sample = (pDevPort->bytes_per_channel << 3);
		pcm_data->interleaved = TRUE;
		pcm_data->channel_mapping = NULL;
		elite_allocbuf_and_log_data_pkt(&log_info_var);
	}

	if(pDevPort->data_log.log_buf_ptr != NULL)
	{
		qurt_elite_memory_free(pDevPort->data_log.log_buf_ptr);
	}
	pDevPort->data_log.log_buf_ptr = NULL;
	pDevPort->data_log.log_size_used = 0;
}

void afe_port_data_logging(afe_dev_port_t *pDevPort, int8_t *mem_start_addr, bool_t is_data_interleaved)
{
	uint16_t unLogSizePerInt;
	uint16_t logId, nNumSamples;
	uint32_t log_size_used = 0, totalLogSize = 0;
	int8_t *log_buf_ptr;
	elite_log_info log_info_var;
	pcm_data_info *pcm_data;

	/*< check for status of 0x1586 log code and non-zero check for total size of log in bytes per diag commit.*/
	if(0 == pDevPort->data_log.total_log_size || (ADSP_EFAILED == afe_log_status(QXDM_LOG_CODE_AFE_RX_TX_OUT)))
	{
		return;
	}

	/* afe log buf size, and data size per interrupt */
	unLogSizePerInt = pDevPort->data_log.bytes_per_int_all_ch;
	totalLogSize = pDevPort->data_log.total_log_size;
	nNumSamples = pDevPort->int_samples_per_period;
	log_size_used = pDevPort->data_log.log_size_used;
	log_buf_ptr = (int8_t *)pDevPort->data_log.log_buf_ptr;

	/* Populate the log buffer for every DMA/BAM interrupt */
	if(AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
	{
		/* If log buffer available, populate it with port samples  */

		/* Old parser does not support deinterleaved data for multichannel (> 2 channel) config
       Though new parser supports deinterleaved data, here we are interleaving for code simplification 
		 */
		if(!is_data_interleaved)
		{
			afe_port_interleave_samples(mem_start_addr, (int8_t *)(log_buf_ptr + (log_size_used)), pDevPort->channels, nNumSamples, pDevPort->bytes_per_channel);
		}
		else
		{
			memscpy((log_buf_ptr + log_size_used),
					(totalLogSize - log_size_used),
					mem_start_addr,
					unLogSizePerInt);
		}

		if(24 == pDevPort->bit_width)
		{
			/* convert the format Q27 to Q31 */
			int32_t *pBuf_frm_ptr32 = (int32_t *)(log_buf_ptr + (log_size_used));
			uint32_t total_num_samples = (pDevPort->int_samples_per_period) * (pDevPort->channels);
			for(uint32_t i = 0; i < total_num_samples; i++)
			{
				pBuf_frm_ptr32[i] = s32_shl_s32_sat(pBuf_frm_ptr32[i], pDevPort->q_format_shift_factor);
			}
		}
	}
	else /* Compressed data port */
	{
		memscpy((log_buf_ptr + log_size_used),
				(totalLogSize - log_size_used),
				mem_start_addr,
				unLogSizePerInt);
	}

	/* Update used log buffer size */
	log_size_used += unLogSizePerInt;

	/* If all the log buffer size has been used, do commit the log buffer. */
	if(log_size_used >= (totalLogSize))
	{
		/* Fill the logId with the port ID. */
		logId = AUDIOLOG_CHAN_PCM_RX_INTERNAL_AFE + pDevPort->intf;

		log_info_var.qxdm_log_code = QXDM_LOG_CODE_AFE_RX_TX_OUT;
		log_info_var.buf_ptr = (uint8_t *)log_buf_ptr;
		log_info_var.buf_size = log_size_used;
		log_info_var.session_id = pDevPort->data_log.port_session_id_for_log;
		log_info_var.log_tap_id = logId;
		log_info_var.log_time_stamp = qurt_elite_timer_get_time();

		if(AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
		{
			log_info_var.data_fmt = ELITE_LOG_DATA_FMT_PCM;

			pcm_data = &(log_info_var.data_info.pcm_data_fmt);

			pcm_data->num_channels = pDevPort->channels;
			pcm_data->sampling_rate = pDevPort->sample_rate;
			pcm_data->bits_per_sample = (pDevPort->bytes_per_channel << 3);
			pcm_data->interleaved = TRUE;
			pcm_data->channel_mapping = NULL;
		}
		else /* compressed data port, logging the port datatype as is */
		{
			log_info_var.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
			log_info_var.data_info.media_fmt_id = pDevPort->port_data_type;
		}

		elite_allocbuf_and_log_data_pkt(&log_info_var);
		/* Reset the used log size to 0 */
		log_size_used = 0;
	}

	pDevPort->data_log.log_size_used = log_size_used;
	pDevPort->data_log.log_buf_ptr = (int16_t *)log_buf_ptr;
}

/**
 * This function is for checking log code status whether it is 
 * enable or not. 
 *
 * @param[in] log_code log code to be checked 
 * @param[out] ADSP_EOK if the code is enabled otherwise 
 *  	 ADSP_EFAILED
 * 
 */
ADSPResult afe_log_status(uint16_t log_code)
{
#ifndef SIM
	if(log_status(log_code))
	{
		return(ADSP_EOK);
	}
	return(ADSP_EFAILED);
#else
	return(ADSP_EOK);
#endif
}

void afe_group_data_logging_init(uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState)
{

#ifdef GROUP_DEV_LOGGING_ENABLE

	uint16_t sizeFactor = 0, unLogSizePerInt;
	uint32_t totalLogSize = 0;

	MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_group_data_logging_init [G:%x] [SR:%d] [BW:%d] [CH:%d] [INTS:%d]",\
			group_id, pGroupDeviceState->sample_rate, pGroupDeviceState->bytes_per_channel, pGroupDeviceState->num_channels, pGroupDeviceState->int_samples_per_period);

	switch(group_id)
	{
	case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX:
		group_p_mi2s_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX:
		group_p_mi2s_tx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX:
		group_q_mi2s_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX:
		group_q_mi2s_tx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX:
		group_primary_tdm_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX:
		group_primary_tdm_tx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX:
		group_secondary_tdm_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX:
		group_secondary_tdm_tx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX:
		group_tertiary_tdm_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX:
		group_tertiary_tdm_tx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX:
		group_quaternary_tdm_rx_log_skipping = 0;
		break;
	case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX:
		group_quaternary_tdm_tx_log_skipping = 0;
		break;
	default:
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_group_data_logging_init error unknown group id");
		break;
	}


	unLogSizePerInt = pGroupDeviceState->int_samples_per_period * pGroupDeviceState->num_channels * pGroupDeviceState->bytes_per_channel;

	totalLogSize = AFE_PCM_BUF_SIZE_10MS * unLogSizePerInt;

	// seq num for session id
	(pGroupDeviceState->data_log.port_session_id_for_log)++;

	pGroupDeviceState->data_log.bytes_per_int_all_ch = unLogSizePerInt;
	pGroupDeviceState->data_log.total_log_size = totalLogSize;
	pGroupDeviceState->data_log.log_buf_ptr = NULL;
	pGroupDeviceState->data_log.log_buf_ptr = (int16_t *)qurt_elite_memory_malloc(totalLogSize, QURT_ELITE_HEAP_DEFAULT);
	if(NULL == pGroupDeviceState->data_log.log_buf_ptr)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Group Device %x Error allocating memory for log buffer", group_id);
		pGroupDeviceState->data_log.total_log_size = 0;

	}
	pGroupDeviceState->data_log.log_size_used = 0;

#endif

}

void afe_group_data_logging_deinit(uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState)
{

#ifdef GROUP_DEV_LOGGING_ENABLE

	elite_log_info log_info_var;
	pcm_data_info *pcm_data = &(log_info_var.data_info.pcm_data_fmt);

	/* If all the log buffer size has been used, do commit the log buffer. */
	if(pGroupDeviceState->data_log.log_size_used > 0)
	{
		uint16_t logId;
		logId = AUDIOLOG_GROUP_AFE + group_id;

		log_info_var.qxdm_log_code = QXDM_LOG_CODE_AFE_RX_TX_OUT;
		log_info_var.buf_ptr = (uint8_t *)pGroupDeviceState->data_log.log_buf_ptr;
		log_info_var.buf_size = pGroupDeviceState->data_log.log_size_used;
		log_info_var.session_id = pGroupDeviceState->data_log.port_session_id_for_log;
		log_info_var.log_tap_id = logId;
		log_info_var.log_time_stamp = qurt_elite_timer_get_time();
		log_info_var.data_fmt = ELITE_LOG_DATA_FMT_PCM;
		pcm_data->num_channels = pGroupDeviceState->num_channels;
		pcm_data->sampling_rate = pGroupDeviceState->sample_rate;
		pcm_data->bits_per_sample = (pGroupDeviceState->bytes_per_channel << 3);
		pcm_data->interleaved = TRUE;
		pcm_data->channel_mapping = NULL;
		elite_allocbuf_and_log_data_pkt(&log_info_var);
	}

	if(pGroupDeviceState->data_log.log_buf_ptr != NULL)
	{
		qurt_elite_memory_free(pGroupDeviceState->data_log.log_buf_ptr);
	}
	pGroupDeviceState->data_log.log_buf_ptr = NULL;
	pGroupDeviceState->data_log.log_size_used = 0;

#endif

}

#ifdef GROUP_DEV_LOGGING_ENABLE
bool_t afe_group_data_logging_skip(uint16 group_id)
{
	bool_t rc = FALSE;

	switch(group_id)
	{
	case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_RX:
	{
		if(0 != (group_p_mi2s_rx_log_skipping % 10))
		{
			group_p_mi2s_rx_log_skipping++;
			rc = TRUE;
		}
		group_p_mi2s_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_PRIMARY_MI2S_TX:
	{
		if(0 != (group_p_mi2s_tx_log_skipping % 10))
		{
			group_p_mi2s_tx_log_skipping++;
			rc = TRUE;
		}
		group_p_mi2s_tx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_RX:
	{
		if(0 != (group_q_mi2s_rx_log_skipping % 10))
		{
			group_q_mi2s_rx_log_skipping++;
			rc = TRUE;
		}
		group_q_mi2s_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_QUATERNARY_MI2S_TX:
	{
		if(0 != (group_q_mi2s_tx_log_skipping % 10))
		{
			group_q_mi2s_tx_log_skipping++;
			rc = TRUE;
		}
		group_q_mi2s_tx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_RX:
	{
		if(0 != (group_primary_tdm_rx_log_skipping % 10))
		{
			group_primary_tdm_rx_log_skipping++;
			rc = TRUE;
		}
		group_primary_tdm_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_PRIMARY_TDM_TX:
	{
		if(0 != (group_primary_tdm_tx_log_skipping % 10))
		{
			group_primary_tdm_tx_log_skipping++;
			rc = TRUE;
		}
		group_primary_tdm_tx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_RX:
	{
		if(0 != (group_secondary_tdm_rx_log_skipping % 10))
		{
			group_secondary_tdm_rx_log_skipping++;
			rc = TRUE;
		}
		group_secondary_tdm_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_SECONDARY_TDM_TX:
	{
		if(0 != (group_secondary_tdm_tx_log_skipping % 10))
		{
			group_secondary_tdm_tx_log_skipping++;
			rc = TRUE;
		}
		group_secondary_tdm_tx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_RX:
	{
		if(0 != (group_tertiary_tdm_rx_log_skipping % 10))
		{
			group_tertiary_tdm_rx_log_skipping++;
			rc = TRUE;
		}
		group_tertiary_tdm_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_TERTIARY_TDM_TX:
	{
		if(0 != (group_tertiary_tdm_tx_log_skipping % 10))
		{
			group_tertiary_tdm_tx_log_skipping++;
			rc = TRUE;
		}
		group_tertiary_tdm_tx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_RX:
	{
		if(0 != (group_quaternary_tdm_rx_log_skipping % 10))
		{
			group_quaternary_tdm_rx_log_skipping++;
			rc = TRUE;
		}
		group_quaternary_tdm_rx_log_skipping++;
		break;
	}
	case AFE_GROUP_DEVICE_ID_QUATERNARY_TDM_TX:
	{
		if(0 != (group_quaternary_tdm_tx_log_skipping % 10))
		{
			group_quaternary_tdm_tx_log_skipping++;
			rc = TRUE;
		}
		group_quaternary_tdm_tx_log_skipping++;
		break;
	}
	default:
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_group_data_logging error unknown group id");
		break;
	}

	return(rc);
}
#endif


void afe_group_data_logging(uint16 group_id, afe_group_device_state_struct_t *pGroupDeviceState)
{

#ifdef GROUP_DEV_LOGGING_ENABLE

uint16_t unLogSizePerInt;
uint16_t logId, nNumSamples;
uint32_t log_size_used = 0, totalLogSize = 0;
int8_t *log_buf_ptr;
elite_log_info log_info_var;
//pcm_data_info     *pcm_data;

/*< check for status of 0x1586 log code and non-zero check for total size of log in bytes per diag commit.*/
if(0 == pGroupDeviceState->data_log.total_log_size || (ADSP_EFAILED == afe_log_status(QXDM_LOG_CODE_AFE_RX_TX_OUT)))
{
	return;
}

// logging every 10ms to reduce the overhead of data logging.
if(afe_group_data_logging_skip(group_id))
{
	return;
}

/* afe log buf size, and data size per interrupt */
unLogSizePerInt = pGroupDeviceState->data_log.bytes_per_int_all_ch;
totalLogSize = pGroupDeviceState->data_log.total_log_size;
nNumSamples = pGroupDeviceState->int_samples_per_period;
log_size_used = pGroupDeviceState->data_log.log_size_used;
log_buf_ptr = (int8_t *)pGroupDeviceState->data_log.log_buf_ptr;

int8_t *p_group_io_buf = pGroupDeviceState->p_group_io_buf;

/* Populate the log buffer for every DMA/BAM interrupt */
memscpy((log_buf_ptr + log_size_used),
		(totalLogSize - log_size_used),
		p_group_io_buf,
		unLogSizePerInt);

/* Update used log buffer size */
log_size_used += unLogSizePerInt;

/* If all the log buffer size has been used, do commit the log buffer. */
if(log_size_used >= (totalLogSize))
{
	/* Fill the logId with the port ID. */
	logId = AUDIOLOG_GROUP_AFE + group_id;

	log_info_var.qxdm_log_code = QXDM_LOG_CODE_AFE_RX_TX_OUT;
	log_info_var.buf_ptr = (uint8_t *)log_buf_ptr;
	log_info_var.buf_size = log_size_used;
	log_info_var.session_id = pGroupDeviceState->data_log.port_session_id_for_log;
	log_info_var.log_tap_id = logId;
	log_info_var.log_time_stamp = qurt_elite_timer_get_time();
	log_info_var.data_fmt = ELITE_LOG_DATA_FMT_BITSTREAM;
	log_info_var.data_info.media_fmt_id = AFE_COMPRESSED_DATA_PORT;

	elite_allocbuf_and_log_data_pkt(&log_info_var);
	/* Reset the used log size to 0 */
	log_size_used = 0;
}

pGroupDeviceState->data_log.log_size_used = log_size_used;
pGroupDeviceState->data_log.log_buf_ptr = (int16_t *)log_buf_ptr;
#endif

}

