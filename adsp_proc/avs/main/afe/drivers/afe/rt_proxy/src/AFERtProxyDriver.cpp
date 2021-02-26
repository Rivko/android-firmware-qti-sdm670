/*========================================================================

  This file is afe device driver layer implementation for real-time proxy ports (mainly USB)
  RTPP -> real time proxy port

  Copyright (c) 2013-2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

  $Header: //source/qcom/qct/multimedia2/Audio/drivers/afe/rt_proxy/...

  when       who     what, where, why
  --------   ---     -------------------------------------------------------
  03/15/11   rb      Created file.

  ========================================================================== */
/*==========================================================================
  the Include files
  ========================================================================== */
#include "AFEDeviceDriver.h"
#include "AFERtProxyDriver_i.h"
#include "audio_basic_op.h"

afe_dev_port_t            port_rt_proxy  [AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE];   //this is for the AFE svc purpose
afe_rt_proxy_port_dev_t   dev_rt_proxy   [AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE];   //this is for the purpose of RT proxy port Data Mgr (indices correspond)

/** circular buffer access */
ADSPResult afe_rt_proxy_port_linear_to_circ(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, \
		uint8_t *src_linear_buff_ptr, uint32_t length, uint32_t num_bytes, \
		uint16_t *is_high_water_mark);

ADSPResult afe_rt_proxy_port_circ_to_linear(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, uint8_t *dst_linear_buff_ptr, \
		uint32_t length, uint16_t *is_low_water_mark);

/*=====================================================================
 Globals
 ======================================================================*/

/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform Real time Proxy port port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_rt_proxy_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult result = ADSP_EFAILED;
	afe_dev_port_t     *pDevPort = NULL;
	uint16_t           port_id;

	for (int num_port = 0; num_port < AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE; num_port++)
	{
		//Init RT proxy ports
		port_id = AFE_PORT_ID_RT_PROXY_PORT_001_RX+num_port;
		pDevPort = afe_svc_ptr->port_rt_proxy_ptr[num_port] = &port_rt_proxy[num_port];

		if (ADSP_FAILED(result = afe_svc_init_port(pDevPort, port_id, \
				(num_port&AFE_PORT_DIR_MASK))))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port Init Fails, port_id = %x", port_id);
			return result;
		}
		else
		{
			pDevPort->afe_drv       = NULL;
			memset(&dev_rt_proxy[num_port], 0, sizeof(afe_rt_proxy_port_dev_t));
			(dev_rt_proxy[num_port]).port_id = port_id;
			qurt_elite_mutex_init(&(dev_rt_proxy[num_port].circ_buf_mutex));

			if (AFE_PORT_TX_DIR == pDevPort->dir)
			{
				pDevPort->afe_dev_process = (afe_dev_port_proc_func)afe_rt_proxy_port_read;
			}
			else
			{
				pDevPort->afe_dev_process = (afe_dev_port_proc_func)afe_rt_proxy_port_write;
			}
		}
#ifdef RTPP_VERBOSE_DEBUG
MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Real Time Proxy Port, port_id = %x inited", port_id);
#endif
	}
	return ADSP_EOK;
}

/**
  @brief Perform Real time Proxy port port deinitialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_rt_proxy_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult result = ADSP_EFAILED;
	afe_dev_port_t    *pDevPort = NULL;

	for (int num_port=0; num_port < AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE; num_port++)
	{
		//Deinit RT proxy ports
		pDevPort = afe_svc_ptr->port_rt_proxy_ptr[num_port];

#ifdef RTPP_VERBOSE_DEBUG
		MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Real Time Proxy Port, port_id = %x deinited", pDevPort->intf);
#endif

		if (ADSP_FAILED(result = afe_svc_deinit_port(pDevPort)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port denit Fails, port_id =%d", AFE_PORT_ID_RT_PROXY_PORT_RANGE_START+num_port);
		}

		qurt_elite_mutex_destroy(&(dev_rt_proxy[num_port].circ_buf_mutex));
	}
	return result;
}

/**
 * Perform one time initialization of the AFE Rt Proxy Port driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_init(void)
{
	ADSPResult                       result;

	result = afe_rtppdm_init();
#ifdef RTPP_VERBOSE_DEBUG
	MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Real Time Proxy Port Driver inited %x", result);
#endif
	return result;
}


/**
 * Perform de-initialization of the AFE Rt Proxy Port driver
 *
 * @param[in] None
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_deinit(void)
{
	ADSPResult  result;
	result = afe_rtppdm_deinit();
#ifdef RTPP_VERBOSE_DEBUG
	MSG_1(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Real Time Proxy Port Driver deinited %x", result);
#endif

	return result;
}

/**
  @brief Perform configuration of RT Proxy devices for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_cfg pointer to AFE port Rt Proxy config strcutre

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_rt_proxy_cfg (afe_dev_port_t* dev_port, int8_t *p_cfg, uint16_t payload_size)
{
	ADSPResult                    result = ADSP_EOK;
	afe_rt_proxy_port_dev_t       *rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(dev_port->intf)];
	dev_port->afe_drv =  (void*) rt_proxy_dev_ptr;

	// choose the appropriate config version
	uint32_t cfg_ver = (uint32_t) *(p_cfg);
	if(cfg_ver > AFE_API_VERSION_RT_PROXY_CONFIG)
	{
		cfg_ver = AFE_API_VERSION_RT_PROXY_CONFIG;
	}

	rt_proxy_dev_ptr->dev_cfg_minor_ver = cfg_ver;
	switch (cfg_ver)
	{
	case RTPROXY_CFG_V1:
	{
		afe_param_id_rt_proxy_port_cfg_t  *config = (afe_param_id_rt_proxy_port_cfg_t*)p_cfg;

		/* error check for bit width */
		if((AFE_BITS_PER_SAMPLE_16 != config->bit_width) && (AFE_BITS_PER_SAMPLE_24 != config->bit_width)&& (AFE_BITS_PER_SAMPLE_32 != config->bit_width))
		{
			result = ADSP_EUNSUPPORTED;
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy cfg Set Cmd Cfg fail: unsupported bitwidth: %d, port_id: 0x%x",
					config->bit_width, dev_port->intf);
			break;
		}

		if ((config->num_channels < 1) || (config->num_channels > AFE_PORT_MAX_AUDIO_CHAN_CNT))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %d for port 0x%x", config->num_channels, dev_port->intf);
			return ADSP_EUNSUPPORTED;
		}

		/* error check for sample rate */
		if ((AFE_PORT_SAMPLE_RATE_8K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_16K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_24K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_32K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_48K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_96K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_192K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_384K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_22_05K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_44_1K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_88_2K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_176_4K != config->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_352_8K != config->sample_rate))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, for port: 0x%x", config->sample_rate, dev_port->intf);
			return ADSP_EUNSUPPORTED;
		}

		dev_port->dev_sample_rate                 = config->sample_rate;
		dev_port->dev_channels                    = config->num_channels;
		dev_port->dev_bytes_per_channel           = (config->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
		dev_port->dev_bit_width 		  = config->bit_width;

		//if applicable, overwrite the port media type with the device media type.
		//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
		afe_port_copy_media_type_dev_to_port(dev_port);

		rt_proxy_dev_ptr->jitter_allowance    = config->jitter_allowance;
		rt_proxy_dev_ptr->frame_size          = config->frame_size;
		rt_proxy_dev_ptr->low_water_mark      = config->low_water_mark;
		rt_proxy_dev_ptr->config_high_water_mark = config->high_water_mark;
		rt_proxy_dev_ptr->interleaved         = config->interleaved;
		rt_proxy_dev_ptr->bit_width           = config->bit_width;
#if 0
for (int i=0; i<8; i++)
{
	rt_proxy_dev_ptr->device_channel_mapping[i] = config->channel_mapping[i];
}
#endif
rt_proxy_dev_ptr->bytes_per_sample    = dev_port->channels * dev_port->bytes_per_channel;

MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port config: port_id=%x,channels=%lu,bytes_per_ch=%d,jitter_allowance=%d,frame_size=%d", \
		dev_port->intf, dev_port->channels, dev_port->bytes_per_channel, rt_proxy_dev_ptr->jitter_allowance, rt_proxy_dev_ptr->frame_size);
MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port config: port_id=%x, low_water_mark=%d,high(config)=%d,interleaved=%d.", dev_port->intf, \
		rt_proxy_dev_ptr->low_water_mark, rt_proxy_dev_ptr->config_high_water_mark, rt_proxy_dev_ptr->interleaved);
#if 0
MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port config: port_id=%x,ch_map:%d,%d,%d,%d,%d,%d,%d,%d", \
		dev_port->intf,rt_proxy_dev_ptr->device_channel_mapping[0],rt_proxy_dev_ptr->device_channel_mapping[1],\
		rt_proxy_dev_ptr->device_channel_mapping[2],rt_proxy_dev_ptr->device_channel_mapping[3],rt_proxy_dev_ptr->device_channel_mapping[4],\
		rt_proxy_dev_ptr->device_channel_mapping[5],rt_proxy_dev_ptr->device_channel_mapping[6],rt_proxy_dev_ptr->device_channel_mapping[7]);
#endif
	}
	break;

	default:
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy cfg Set Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
				cfg_ver, dev_port->intf);
		return ADSP_EUNSUPPORTED;
	}
	break;
	}

	return result;
}

ADSPResult afe_get_rt_proxy_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	ADSPResult                    result = ADSP_EOK;
	afe_rt_proxy_port_dev_t       *p_dev_state = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(afe_port_ptr->intf)];
	uint32_t cfg_ver = p_dev_state->dev_cfg_minor_ver;

	switch (cfg_ver)
	{
	case RTPROXY_CFG_V1:
	{

		*params_buffer_len_req_ptr = sizeof(afe_param_id_rt_proxy_port_cfg_t);
		if(*params_buffer_len_req_ptr > payload_size )
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: rt proxy cfg required size = %d, given size = %lu",
					*params_buffer_len_req_ptr, payload_size);
			return ADSP_EBADPARAM;
		}

		afe_param_id_rt_proxy_port_cfg_t  *p_rt_proxy_cfg = (afe_param_id_rt_proxy_port_cfg_t*)param_buf_ptr;

		p_rt_proxy_cfg->bit_width = p_dev_state->bit_width;
		p_rt_proxy_cfg->frame_size = p_dev_state->frame_size;
		p_rt_proxy_cfg->high_water_mark = p_dev_state->high_water_mark;
		p_rt_proxy_cfg->low_water_mark = p_dev_state->low_water_mark;
		p_rt_proxy_cfg->interleaved = p_dev_state->interleaved;
		p_rt_proxy_cfg->jitter_allowance = p_dev_state->jitter_allowance;
		p_rt_proxy_cfg->sample_rate = afe_port_ptr->sample_rate;

		p_rt_proxy_cfg->num_channels = afe_port_ptr->channels;
		p_rt_proxy_cfg->reserved = 0;
		p_rt_proxy_cfg->rt_proxy_cfg_minor_version = cfg_ver;
	}
	break;

	default:
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
				cfg_ver, afe_port_ptr->intf);
		return ADSP_EBADPARAM;
	}
	break;
	}

	return result;
}

/**
 * Perform Real time Proxy port compatible device open
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_rt_proxy_port_open(afe_dev_port_t *pDevPort)
{
	ADSPResult              result      = ADSP_EOK;
	qurt_elite_timer_t           *timer_ptr  = NULL;
	uint32_t                buf_size    = 0;
	uint16_t                temp        = 0;
	uint8_t                 *buf        = NULL;
	uint64_t                block_transfer_time_us = 0;
	afe_circ_buff_t         *circ_buf_ptr;
	uint32_t				afe_port_frame_size = 0;
	afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr;

	rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(pDevPort->intf)];
	circ_buf_ptr     = &rt_proxy_dev_ptr->circ_buf;

	//allocate circ buffer
	temp                                   = ((rt_proxy_dev_ptr->jitter_allowance > rt_proxy_dev_ptr->frame_size)? \
			rt_proxy_dev_ptr->jitter_allowance : \
			rt_proxy_dev_ptr->frame_size); //larger one of the jitter allowance & framesize.

			rt_proxy_dev_ptr->lower_drift_thresh   = temp + pDevPort->int_samples_per_period * \
					rt_proxy_dev_ptr->bytes_per_sample; //1 device port frame size more than temp

			afe_port_frame_size					  =  pDevPort->int_samples_per_period * \
					rt_proxy_dev_ptr->bytes_per_sample; //1 device port frame size more than temp

			/* assign the circular buffersize based on fractional or integer sampling rate of the port.
   This is done to ensure optimum memory is used in either cases to cover for drifts/frame size mismatches between HLOS client and port
   1. In an ideal scenario, we need to have ping pong type mechanism in circular buffer. Hence, atleast two time frame size (afe port frame + client frame)
   is needed.
   2. To cover for any drifts between HLOS timing and afe internal timing (AVTimer), we need to allocate buffer on ping and pong portion. 
   currently, this is set to be AFE_RTPP_DRIFT_ALLOWANCE_MS. this is arbitarily chosen
   3. HLOS may issue buffers at a constant rate, say every 10ms, or with some jitter. this jitter is typically a factor/multiple of frame size.
   To ensure over-writing of data is not present, we need to account for this jitter frame as well in ping and pong portion
   4. On top of this, as explained earlier, for fractional rates, instead of just ping and pong,we implement triple buffering. This extra cushion is 
   needed to cover for drifts when afe write/read rate is different from HLOS read/write rate respectively. */   
			if (afe_is_sample_rate_fract(pDevPort->sample_rate))
			{
				buf_size                             = (AFE_RTPP_BUF_SIZE_FRACT_FACTOR * (rt_proxy_dev_ptr->frame_size + afe_port_frame_size)) + \
						(2 * rt_proxy_dev_ptr->jitter_allowance) +  \
						(2 * AFE_RTPP_DRIFT_ALLOWANCE_MS * afe_port_frame_size);
			}
			else
			{
				buf_size                             = (AFE_RTPP_BUF_SIZE_INT_FACTOR * (rt_proxy_dev_ptr->frame_size + afe_port_frame_size)) + \
						(2 * rt_proxy_dev_ptr->jitter_allowance) +  \
						(2 * AFE_RTPP_DRIFT_ALLOWANCE_MS * afe_port_frame_size);
			}
			rt_proxy_dev_ptr->upper_drift_thresh   = buf_size - (rt_proxy_dev_ptr->lower_drift_thresh); //symmetric (so that either underrun or overrun can be handled)
			rt_proxy_dev_ptr->ideal_fullness       = buf_size >> 1; //1/2 the buf size, symmetric (so that either underrun or overrun can be handled)
			rt_proxy_dev_ptr->real_fullness        = 0;
			rt_proxy_dev_ptr->high_water_mark      = buf_size - rt_proxy_dev_ptr->config_high_water_mark; //

			rt_proxy_dev_ptr->init_device_clock_us = 0;
			rt_proxy_dev_ptr->device_clock         = 0;
			rt_proxy_dev_ptr->timer_adjust_counter = 0;
			rt_proxy_dev_ptr->interrupt_counter    = 0;


			rt_proxy_dev_ptr->net_timer_adjust     = 0;

			rt_proxy_dev_ptr->afe_byte_cnt         = 0;

			if (ADSP_FAILED(result = afe_rt_proxy_port_get_adj_val( \
					&(rt_proxy_dev_ptr->timer_adjust_counter_constant),\
					&(rt_proxy_dev_ptr->timer_adjust_value), pDevPort->sample_rate)))
			{
				return result;
			}

			if (!IS_IT_TX_PORT_AFE_ID(pDevPort->intf)) //RX port and TX port have opposite meanings of drift correction wrt the thresholds.
			{
				rt_proxy_dev_ptr->timer_adjust_value = -rt_proxy_dev_ptr->timer_adjust_value;
			}

			buf         = (uint8_t*) qurt_elite_memory_malloc(buf_size, QURT_ELITE_HEAP_DEFAULT);
			//buf         = (uint8_t*) afe_dev_lpa_aligned_malloc(buf_size, 4 /* align size */); //using LPM: TBD

			if (NULL == buf)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Rt Proxy Circular buffer malloc failed.");
				return ADSP_ENOMEMORY;
			}
			memset(buf,0,buf_size);
			circ_buf_ptr->top_addr      = buf;
			circ_buf_ptr->bottom_addr   = buf + buf_size;
			circ_buf_ptr->head_addr     = buf;
			circ_buf_ptr->tail_addr     = buf;
			circ_buf_ptr->buffer_size   = buf_size;

			rt_proxy_dev_ptr->scratch_buffer_ptr   = NULL;
			rt_proxy_dev_ptr->was_first_write_done = FALSE;
			rt_proxy_dev_ptr->was_first_read_done  = FALSE;

			if (0 == rt_proxy_dev_ptr->interleaved) //if not interleaved, then we need to interleave
			{
				//allocate memory for interleaved input/output
				rt_proxy_dev_ptr->scratch_buffer_ptr = (uint8_t*) qurt_elite_memory_malloc(rt_proxy_dev_ptr->frame_size, QURT_ELITE_HEAP_DEFAULT);
				if (NULL == rt_proxy_dev_ptr->scratch_buffer_ptr)
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Rt Proxy scratch buffer malloc failed.");
					return ADSP_ENOMEMORY;
				}
			}

			//call data mgr open (to send a message to data mgr thread)
			afe_rtppdm_port_start(pDevPort->intf);

			pDevPort->afe_drv = (void*) rt_proxy_dev_ptr;

#ifdef DEBUG_FILE_LOG
			FILE *fp;
			fp = fopen("afe_rx_port_write.pcm","w");     // clear content
			fclose(fp);
			fp = fopen("afe_tx_port_read.pcm","w");      // clear content
			fclose(fp);
			fp = fopen("afe_rx_port_to_USB.pcm","w");    // clear content
			fclose(fp);
			fp = fopen("afe_tx_port_from_USB.pcm","w");  // clear content
			fclose(fp);
#endif

			MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x, size=%ld, upper=%d, lower=%d, counter_const=%ld, adjust_val=0x%x%x", rt_proxy_dev_ptr->port_id,\
					rt_proxy_dev_ptr->circ_buf.buffer_size,  rt_proxy_dev_ptr->upper_drift_thresh, rt_proxy_dev_ptr->lower_drift_thresh,rt_proxy_dev_ptr->timer_adjust_counter_constant, \
					(unsigned int)(rt_proxy_dev_ptr->timer_adjust_value>>32), (unsigned int)rt_proxy_dev_ptr->timer_adjust_value);

			//create timer
			timer_ptr = (qurt_elite_timer_t *)qurt_elite_memory_malloc(sizeof(qurt_elite_timer_t),QURT_ELITE_HEAP_DEFAULT);
			if (NULL == timer_ptr)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to allocate real time proxy port timer object");
				return ADSP_ENOMEMORY;
			}
			if (ADSP_FAILED(result = afe_time_srvr_timer_create(timer_ptr, QURT_ELITE_TIMER_ONESHOT_ABSOLUTE, &(pDevPort->dma_signal))))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to create timer for RT proxy port");
				qurt_elite_memory_free(timer_ptr);
				return result;
			}
			pDevPort->timer_ptr = timer_ptr;

			block_transfer_time_us = ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate);

			rt_proxy_dev_ptr->device_clock = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)pDevPort->avt_drv) + block_transfer_time_us;

			if ( ADSP_EOK != afe_time_srvr_timer_oneshot_start_absolute(pDevPort->timer_ptr, rt_proxy_dev_ptr->device_clock))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: %x: starting one shot timer  failed",pDevPort->intf);
			}

			rt_proxy_dev_ptr->init_device_clock_us = rt_proxy_dev_ptr->device_clock;

			return ADSP_EOK;
}
/**
 * Perform Real time Proxy port compatible device close
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
void afe_rt_proxy_port_close(afe_dev_port_t *pDevPort)
{
	ADSPResult result = ADSP_EOK;
	afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr;

	rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(pDevPort->intf)];

	//call data mgr close (to send a message to data mgr thread)
	result = afe_rtppdm_port_stop(pDevPort->intf);

	//delete timer
	if(NULL != pDevPort->timer_ptr)
	{
	  result = afe_time_srvr_timer_delete(pDevPort->timer_ptr);
	  if(ADSP_EOK != result)
	  {
	    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_rt_proxy_port_close: Failed with return code: %d", result);
	  }


	  qurt_elite_memory_free(pDevPort->timer_ptr);

	  pDevPort->timer_ptr  = NULL;
	}

	pDevPort->afe_drv    = NULL;

	//mutex is required because either threadpool thread or the data mgr thread might be accessing the circ buf
	qurt_elite_mutex_lock(&(rt_proxy_dev_ptr->circ_buf_mutex));
	//free circ buff
	if(NULL != rt_proxy_dev_ptr->circ_buf.top_addr)
	{
		qurt_elite_memory_free(rt_proxy_dev_ptr->circ_buf.top_addr);
		memset(&rt_proxy_dev_ptr->circ_buf, 0, sizeof(afe_circ_buff_t));
	}
	if (NULL != rt_proxy_dev_ptr->scratch_buffer_ptr)
	{
		qurt_elite_memory_free(rt_proxy_dev_ptr->scratch_buffer_ptr);
		rt_proxy_dev_ptr->scratch_buffer_ptr = NULL;
	}
	qurt_elite_mutex_unlock(&(rt_proxy_dev_ptr->circ_buf_mutex));

	return;
}
/**
 * function that reads for tx dir
 */
ADSPResult afe_rt_proxy_port_read(afe_dev_port_t *dev_port, int16_t *buf_ptr, uint32_t num_samples)
{
	ADSPResult                 result = ADSP_EOK;
	afe_rt_proxy_port_dev_t    *rt_proxy_dev_ptr;
	uint16_t                   is_low_water_mark;
	uint32_t                   block_size;
	int32_t                    i;

	block_size        = dev_port->bytes_per_channel * dev_port->channels * num_samples;
	rt_proxy_dev_ptr  = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(dev_port->intf)];

	//to read 1 ms of data from the circ buffer
	qurt_elite_mutex_lock(&rt_proxy_dev_ptr->circ_buf_mutex);
	if (rt_proxy_dev_ptr->was_first_write_done)
	{
		result = afe_rt_proxy_port_circ_to_linear(rt_proxy_dev_ptr, (uint8_t*)buf_ptr, \
				block_size, &is_low_water_mark);

		/* Q31 to Q27 conversion*/
		if(AFE_BITS_PER_SAMPLE_24 == dev_port->bit_width)
		{
#ifdef __qdsp6__

			int64_t  *ptr_dst_64 = (int64_t *)buf_ptr;
			int32_t numIteration  = (dev_port->channels * num_samples) >> 1;

			for(i=0; i < numIteration; i++)
			{
				/* Q31 to Q27 conversion,  8 bytes at at time */
				(*ptr_dst_64) = Q6_P_vasrw_PI(*ptr_dst_64, QFORMAT_SHIFT_FACTOR);
				ptr_dst_64++;
			}
#else
			int32_t   *pBuf_frm_ptr32 = (int32_t *)buf_ptr;
			int32_t numIteration = dev_port->channels * num_samples;

			/*------Non Q6 Version ---------*/
			for(int i=0; i< numIteration ; i++)
			{
				pBuf_frm_ptr32[i] = (pBuf_frm_ptr32[i] >> QFORMAT_SHIFT_FACTOR);
			}
#endif /* __qdsp6__ */
		}
	}
	else
	{
		is_low_water_mark = TRUE;
		memset(buf_ptr,0,block_size);
#ifdef RTPP_MEDIUM_DEBUG
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE afe_rt_proxy_port_read: %x: Underrun since first write not done yet", dev_port->intf);
#endif
	}
	qurt_elite_mutex_unlock(&rt_proxy_dev_ptr->circ_buf_mutex);

	rt_proxy_dev_ptr->afe_byte_cnt += block_size;

#ifdef DEBUG_FILE_LOG
	FILE *fp;
	fp = fopen("afe_tx_port_read.pcm","a"); // in append mode
	fwrite(buf_ptr,sizeof(int16_t),num_samples* dev_port->channels,fp);
	fclose(fp);
#endif

	//since buffer arrived, we can send high water mark if it occurs again
	//rt_proxy_dev_ptr->is_high_water_mark_event_sent = FALSE; //TODO to remove

	//send low water mark event only if a buffer was written by RTPPDM,
	if (is_low_water_mark &&
			FALSE == rt_proxy_dev_ptr->is_low_water_mark_event_sent)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: low water mark reached during AVTimer interrupt. port=%x",dev_port->intf);
		if (0 != rt_proxy_dev_ptr->low_water_mark) //send event only if low water mark is not zero.
		{
			result = afe_rtppdm_send_low_watermark_event(dev_port->intf);
			if (ADSP_FAILED(result))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: failed to send low water mark event during AVTimer interrupt");
			}
			else
			{
				rt_proxy_dev_ptr->is_low_water_mark_event_sent = TRUE;
			}
		}
	}
#ifdef RTPP_VERBOSE_DEBUG
	MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "RT_LOG--------------> RTPP: Mic read from circ buf---------\n ");
#endif
	rt_proxy_dev_ptr->interrupt_counter++;
	//restart timer
	result = afe_rt_proxy_port_timer_adjustment(rt_proxy_dev_ptr, dev_port);

	return result;
}

/**
 * function that writes for rx dir
 */
ADSPResult afe_rt_proxy_port_write(afe_dev_port_t *dev_port, int16_t *buf_ptr, uint32_t num_samples)
{
	ADSPResult                 result = ADSP_EOK;
	afe_rt_proxy_port_dev_t    *rt_proxy_dev_ptr;
	uint16_t                   is_high_water_mark;
	uint32_t                   block_size;
	afe_circ_buff_t            *circ_buff_ptr;
	int32_t                    i;

	block_size        = dev_port->bytes_per_channel * dev_port->channels * num_samples;
	rt_proxy_dev_ptr  = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(dev_port->intf)];
	circ_buff_ptr     = &rt_proxy_dev_ptr->circ_buf;

#ifdef DEBUG_FILE_LOG
	FILE *fp;
	fp = fopen("afe_rx_port_write.pcm","a"); // in append mode
	fwrite(buf_ptr,sizeof(int16_t),num_samples* dev_port->channels, fp);
	fclose(fp);
#endif

	qurt_elite_mutex_lock(&rt_proxy_dev_ptr->circ_buf_mutex);
	if (FALSE == rt_proxy_dev_ptr->was_first_write_done)
	{
		//adjust the ptr to be 1/2 away
		//circ_buff_ptr->head_addr = circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
		circ_buff_ptr->head_addr =
				circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >> 1));

		// Wrap around the head if necessary as tail is moving
		if(circ_buff_ptr->head_addr >= circ_buff_ptr->bottom_addr)
		{
			circ_buff_ptr->head_addr =
					circ_buff_ptr->top_addr
					+ (circ_buff_ptr->head_addr - circ_buff_ptr->bottom_addr);
		}
	}

	/* Q27 to Q31 conversion */
	if(AFE_BITS_PER_SAMPLE_24 == dev_port->bit_width)
	{

	  int32_t   *pBuf_frm_ptr32 =  (int32_t *)buf_ptr;
	  int32_t numIteration = dev_port->channels *num_samples;

	  for(i=0; i<numIteration; i++)
	  {
	    /* Q27 to Q31 conversion */
	    (*pBuf_frm_ptr32) = s32_shl_s32_sat(*pBuf_frm_ptr32, QFORMAT_SHIFT_FACTOR);
	    pBuf_frm_ptr32++;
	  }
	}

	//to write 1 ms of data into the circ buffer
	result = afe_rt_proxy_port_linear_to_circ(rt_proxy_dev_ptr, (uint8_t*)buf_ptr, block_size, \
			dev_port->bytes_per_channel * dev_port->channels,
			&is_high_water_mark);

	rt_proxy_dev_ptr->afe_byte_cnt += block_size;
	/**
	 * first write done is true only after writing frame-size. else we may underrun.
	 */
	if (FALSE == rt_proxy_dev_ptr->was_first_write_done && ADSP_SUCCEEDED(result) && \
			rt_proxy_dev_ptr->afe_byte_cnt >= rt_proxy_dev_ptr->frame_size)
	{
		rt_proxy_dev_ptr->was_first_write_done = TRUE;
		//adjust the ptrs
		//circ_buff_ptr->head_addr = circ_buff_ptr->tail_addr + (((circ_buff_ptr->buffer_size) >>4)<<2);
	}
	qurt_elite_mutex_unlock(&rt_proxy_dev_ptr->circ_buf_mutex);

	//since data arrived we can send low water mark if it occurs again
	//rt_proxy_dev_ptr->is_low_water_mark_event_sent = FALSE; //TODO to remove

	//send high water mark event only if a buffer was read by RTPPDM
	if (is_high_water_mark &&
			FALSE == rt_proxy_dev_ptr->is_high_water_mark_event_sent)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: high water mark reached during AVTimer interrupt port=%x",dev_port->intf);
		if (0 != rt_proxy_dev_ptr->high_water_mark)
		{
			result = afe_rtppdm_send_high_watermark_event(dev_port->intf);
			if (ADSP_FAILED(result))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: failed to send high water mark event during AVTimer interrupt");
			}
			else
			{
				rt_proxy_dev_ptr->is_high_water_mark_event_sent = TRUE;
			}
		}
	}
#ifdef RTPP_VERBOSE_DEBUG
	MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "RT_LOG--------------> RTPP: Spkr wrote to circ buf---------\n ");
#endif
	rt_proxy_dev_ptr->interrupt_counter++;
	//restart timer
	result = afe_rt_proxy_port_timer_adjustment(rt_proxy_dev_ptr, dev_port);

	return result;
}

/**
 * Get the current time using Rt Proxy Port port data transfer
 * rate
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[out] curr_time - time stamp in sample ticks
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_get_time_for_rt_proxy_port(afe_dev_port_t *dev_port,uint64_t *curr_time)
{
	// afe_rt_proxy_port_dev_t       *rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(dev_port->intf)];

	// *curr_time        =  rt_proxy_dev_ptr->device_clock;
	*curr_time = avtimer_drv_get_time((avtimer_drv_handle_t)dev_port->avt_drv);

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
ADSPResult afe_rt_proxy_port_circ_to_linear(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, uint8_t *dst_linear_buff_ptr, \
		uint32_t length, uint16_t *is_low_water_mark)
{
	uint32_t temp;
	int32_t  real_fullness;
	afe_circ_buff_t   *src_circ_buff_ptr     = &rt_proxy_dev_ptr->circ_buf;
	uint32_t          low_water_mark         = rt_proxy_dev_ptr->low_water_mark;
	uint16_t          port_id                = rt_proxy_dev_ptr->port_id;
	*is_low_water_mark = FALSE;

	/*
    Check the tail/head circular distance, if distance
      1. larger than length, read the samples.
      2. less or equal length, set the linear buffer as zeros and return;
	 */

	real_fullness = (int32_t)(src_circ_buff_ptr->tail_addr - src_circ_buff_ptr->head_addr); //filled space
	if (real_fullness < 0)
	{
		real_fullness += src_circ_buff_ptr->buffer_size;
	}
	if (real_fullness < 0 || (uint32_t) real_fullness > src_circ_buff_ptr->buffer_size)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE C2L: port=%x:source circ buffer tail/head corrupted!",port_id);
		return ADSP_EFAILED;
	}

	//low water mark detection: if buffer will have less than low water mark samples.
	if ( ( (int32_t)(real_fullness-length) <= (int32_t)low_water_mark) )
	{
		*is_low_water_mark = TRUE;
		//memset(dst_linear_buff_ptr, 0, length);  //clear the dest buffer
	}
	// This limit the access to buffer size and real_fullness
	if ( (uint32_t) real_fullness < length)
	{
		memset(dst_linear_buff_ptr+real_fullness, 0, length-real_fullness);//make the rest as zero
		length = real_fullness; //read whatever is available.
#ifdef RTPP_MEDIUM_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE C2L: Underrun,port=%x, afe=%ld, dev=%ld",port_id, rt_proxy_dev_ptr->afe_byte_cnt, rt_proxy_dev_ptr->dev_byte_cnt);
#endif
	}
	rt_proxy_dev_ptr->real_fullness  = real_fullness - length; //could become zero, but not -ve.
	/*
    The following computing based on:
      1. src_circ_buff->top_addr <= src_circ_buff_ptr->head_addr, src_circ_buff_ptr->tail_addr
         < src_circ_buff_ptr->bottom_addr
      2. length <= src_circ_buff_ptr->buffer_size
	 */

	temp = src_circ_buff_ptr->bottom_addr - src_circ_buff_ptr->head_addr;

	if (temp >= length )
	{
		// if head to bottom has enough space to read
		memscpy(dst_linear_buff_ptr, length,
				src_circ_buff_ptr->head_addr, length);

		//qurt_elite_mutex_lock(&src_circ_buff_ptr->buf_mutex);
		src_circ_buff_ptr->head_addr = src_circ_buff_ptr->head_addr + length;
		//qurt_elite_mutex_unlock(&src_circ_buff_ptr->buf_mutex);
	}
	else
		// (temp < length) case
	{
		// if head to bottom does not have enough space to read
		memscpy(dst_linear_buff_ptr, temp,
				src_circ_buff_ptr->head_addr, temp);

		memscpy(dst_linear_buff_ptr + temp, (length - temp),
				src_circ_buff_ptr->top_addr,  (length - temp));

		//qurt_elite_mutex_lock(&src_circ_buff_ptr->buf_mutex);
		src_circ_buff_ptr->head_addr = src_circ_buff_ptr->top_addr + (length - temp);
		// Based on (temp < length) and limit by circular buffer size
		// This wrarp around cannot happen but only a precaution
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

ADSPResult afe_rt_proxy_port_linear_to_circ(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, \
		uint8_t *src_linear_buff_ptr, uint32_t length, uint32_t num_bytes, \
		uint16_t *is_high_water_mark)
{
	int32_t real_fullness, temp;
	*is_high_water_mark = FALSE;
	afe_circ_buff_t *dst_circ_buff_ptr  = &rt_proxy_dev_ptr->circ_buf;
	uint32_t high_water_mark            = rt_proxy_dev_ptr->high_water_mark;
	uint16_t port_id                    = rt_proxy_dev_ptr->port_id;

	/*
    Check the destination tail/head circular distance, if
      1. there is not enough space for writing, update the head address in the ead
      2. otherwise, normal
	 */
	real_fullness = (int32_t)(dst_circ_buff_ptr->tail_addr - dst_circ_buff_ptr->head_addr); //this is the filled space

	if ( real_fullness < 0)
	{
		real_fullness += dst_circ_buff_ptr->buffer_size;
	}
	if ( real_fullness < 0 || (uint32_t) real_fullness > dst_circ_buff_ptr->buffer_size)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE L2C: port=%x:dst circ buffer tail/head corrupted!",port_id);
		return ADSP_EFAILED;
	}
	//high water mark detection: if buffer will be filled more than high water mark
	if ( ((real_fullness+length) >= high_water_mark) )
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

	temp = dst_circ_buff_ptr->bottom_addr - dst_circ_buff_ptr->tail_addr;

	if (temp >= (int32_t) length )
	{
		// if tail to bottom has enough space to store
		memscpy(dst_circ_buff_ptr->tail_addr, temp,
				src_linear_buff_ptr, length);

		dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->tail_addr + length;
	}
	else
		// (temp < length) case
	{
		// if tail to bottom does not have enough space to store
		memscpy(dst_circ_buff_ptr->tail_addr, temp,
				src_linear_buff_ptr, temp);

		memscpy(dst_circ_buff_ptr->top_addr,
				(dst_circ_buff_ptr->buffer_size),
				(src_linear_buff_ptr+temp),
				(length - temp));

		dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr + (length - temp);
	}

	// Wrap around for bottom to top, > case is only for precaution
	if ((dst_circ_buff_ptr->tail_addr) >= (dst_circ_buff_ptr->bottom_addr) )
	{
		dst_circ_buff_ptr->tail_addr = dst_circ_buff_ptr->top_addr;
	}

	// if dst circular buffer over-run, overwrite the head address.
	if ((int32_t)length >= (int32_t)(dst_circ_buff_ptr->buffer_size - real_fullness)) //when empty space is smaller than length we wrote
	{
#ifdef RTPP_MEDIUM_DEBUG
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE L2C: Overrun. port=%x, afe=%ld, dev=%ld",port_id, rt_proxy_dev_ptr->afe_byte_cnt, rt_proxy_dev_ptr->dev_byte_cnt);
#endif
		//qurt_elite_mutex_lock(&dst_circ_buff_ptr->buf_mutex);
		dst_circ_buff_ptr->head_addr = dst_circ_buff_ptr->tail_addr + num_bytes; //move ahead head_addr a little

		// Wrap around head if pass the bottom
		if (dst_circ_buff_ptr->head_addr >= dst_circ_buff_ptr->bottom_addr)
		{
			dst_circ_buff_ptr->head_addr =
					dst_circ_buff_ptr->top_addr
					+ (dst_circ_buff_ptr->head_addr - dst_circ_buff_ptr->bottom_addr);
		}
		rt_proxy_dev_ptr->real_fullness = dst_circ_buff_ptr->buffer_size;
		//qurt_elite_mutex_unlock(&dst_circ_buff_ptr->buf_mutex);
	}
	else
	{
		rt_proxy_dev_ptr->real_fullness = real_fullness + length;
	}
	return ADSP_EOK;
}
#if 0
ADSPResult afe_rt_proxy_set_stream_channel_map(uint16_t port_id, uint8_t *stream_chan_map_arr)
{
	afe_rt_proxy_port_dev_t                      *rt_proxy_dev_ptr;

	rt_proxy_dev_ptr        = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];
	for (int i = 0; i < 8; i++)
	{
		rt_proxy_dev_ptr->stream_channel_mapping[i] = stream_chan_map_arr[i];
	}

	return ADSP_EOK;
}
#endif
/**
 * this callback is made during VFR interrupt.
 * this function executes in *** fast-int *** context.
 * and it latches required software counts
 *
 * local variables should be avoided, since stack is limited.
 */


void afe_rt_proxy_vfr_callback(uint8_t vfr_src_idx)
{
	for (int i = 0; i < AFE_PORT_ID_RT_PROXY_PORT_RANGE_SIZE; i++)
	{
		if (TRUE == port_rt_proxy[i].drift_info[vfr_src_idx].vfr_enabled)
		{
			/* latch all software counts */
			if ((AFE_PORT_STATE_RUN == port_rt_proxy[i].port_state) && (NULL != port_rt_proxy[i].avt_drv))
			{
				port_rt_proxy[i].drift_info[vfr_src_idx].dev_latched_counter_curr   = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)port_rt_proxy[i].avt_drv);
			}

			dev_rt_proxy[i].vfr_info[vfr_src_idx].curr_vfr_net_timer_adjust = dev_rt_proxy[i].net_timer_adjust;

#ifdef RTPP_DEBUG_VFR
			dev_rt_proxy[i].vfr_info[vfr_src_idx].vfr_curr_pcycles          = qurt_elite_profile_get_pcycles();
#endif
		}
	}
}

/**
  @brief specified port is enabled for VFR drifts.

  @param[in] port_id port id of the rt_proxy port

  @return  ADSP_EOK on success, an error code on error
 */
ADSPResult afe_rt_proxy_add_to_vfr_sync(uint16_t port_id, uint8_t vfr_src_idx)
{
	ADSPResult                 result = ADSP_EOK;
	afe_rt_proxy_port_dev_t    *rt_proxy_dev_ptr;
	afe_rtp_vfr_info_t         *p_vfr_info;

	rt_proxy_dev_ptr        = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(port_id)];

	p_vfr_info = &rt_proxy_dev_ptr->vfr_info[vfr_src_idx];

	p_vfr_info->curr_vfr_net_timer_adjust = 0;
	p_vfr_info->prev_vfr_net_timer_adjust = 0;


	return result;
}

/**
  @brief get VFR drifts during AFE svc vfr processing

  @param[in] port_id port id of the rt_proxy port
  @param[out] drift ptr to the value of the drift. drift in micro-seconds between 2 VFR strobes.
  @param[out] resync_flag flag to indicate resync
  Difference compared to audio if is that there the h/w latches the samples & hence taking care of small us drift is hw responsibility.
  here, that has to be taken care of at sw.
  Also, -ve drift -> VFR is slow compared to USB timer & +ve drift implies VFR is fast.

  Note: When VFR occurs in < 20ms, due to resync, the drift values might be really high & it has to be reported as resync.

  @return  ADSP_EOK on success, an error code on error
 */
void afe_rt_proxy_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
	afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(afe_port_ptr->intf)];

	int32_t                 drift_in_us;
	int64_t                 drift_val=0, wall_clk_diff=0, net_timer_adjust_diff=0;
	uint16_t                vfr_src_idx;
	afe_drift_info_t        *p_drift_info;
	afe_rtp_vfr_info_t      *p_rtp_vfr_info;
	uint32_t                frame_counter;

	vfr_src_idx = vfr_state_ptr->vfr_src_index;

	p_drift_info = &afe_port_ptr->drift_info[vfr_src_idx];

	p_rtp_vfr_info =  &rt_proxy_dev_ptr->vfr_info[vfr_src_idx];

	drift_in_us = (int32_t)(p_drift_info->vfr_drift_info >> 32);

	if (NULL != rt_proxy_dev_ptr)
	{
		/* If VFR source has been disabled and re-enabled, while the port is still running
          resync the port's drift info structure. */

		if (TRUE == vfr_state_ptr->is_first_vfr)
		{
			afe_drv_resync_port_vfr_drift_info(p_drift_info);

			p_rtp_vfr_info->prev_vfr_net_timer_adjust = 0;
		}

		if (TRUE == p_drift_info->is_first_vfr)
		{
			p_drift_info->is_first_vfr = FALSE;
			drift_in_us = 0;
		}
		else /* For every subsequent VFR */
		{
			wall_clk_diff = p_drift_info->dev_latched_counter_curr - p_drift_info->dev_latched_counter_prev;

			net_timer_adjust_diff = (p_rtp_vfr_info->curr_vfr_net_timer_adjust - p_rtp_vfr_info->prev_vfr_net_timer_adjust);

			//net_timer_adjust is +ve if device is slow wrt avt (assume avt rate=vfr rate for simplicity).
			//+ve net_timer_adjust tends to make drift_val -ve & sample_drift +ve. +ve sample drift => VFR faster than device (device slower than VFR)
			drift_val = (VFR_FRAME_SIZE_IN_US - (wall_clk_diff - net_timer_adjust_diff)) ; //TODO: wrap around of net_timer_adjust (which is very rare);

			drift_in_us += (int32_t) drift_val;
		}

		p_drift_info->dev_latched_counter_prev    = p_drift_info->dev_latched_counter_curr;
		p_rtp_vfr_info->prev_vfr_net_timer_adjust = p_rtp_vfr_info->curr_vfr_net_timer_adjust;
	}

	/* Update vfr frame ctr and acc drift info */
	if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
	{
		frame_counter = (uint32_t) (p_drift_info->vfr_drift_info) + 1;
		p_drift_info->vfr_drift_info = (uint64_t)(((int64_t)drift_in_us << 32) | ((uint64_t)frame_counter));
	}

	return;
}

/**
 * below function is used for both RX & TX ports. the difference in drift mechanism is handled during init of timer_adjust value.
 * RX port has to speed up when circular buffer reaches lower threshold whereas TX port has to speed up when it reaches upper threshold.
 * For TX port: since data comes into the circ buffer faster, it will be near upper threshold where the timer is made to run at faster rate.
 * For RX port: since data goes out at faster rate, it’ll be near lower threshold where the timer is made to run at faster rate
 *
 * it tries to match the actual USB (real time) device rate
 */
ADSPResult afe_rt_proxy_port_timer_adjustment(afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr, afe_dev_port_t *dev_port)
{
	uint64_t                next_intr_set_time_us;
	//int64_t                 diff;
	int64_t                 adjustment = 0;
	ADSPResult              result = ADSP_EOK;
	int64_t                 expected_timer_signal_us = 0;

#ifdef RTPP_DEBUG_TIMER_ADJUST
	MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x, net_adjust=0x%x%x, afe_bytes=%ld,dev_bytes=%ld", \
			rt_proxy_dev_ptr->port_id, (int32_t)(rt_proxy_dev_ptr->net_timer_adjust>>32), \
			(int32_t) (rt_proxy_dev_ptr->net_timer_adjust),rt_proxy_dev_ptr->afe_byte_cnt, rt_proxy_dev_ptr->dev_byte_cnt);

	uint64_t temp = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv);
	MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x fullness = %ld, counter=%ld, devclk=0x%x%x, wallclk=0x%x%x", rt_proxy_dev_ptr->port_id,\
			rt_proxy_dev_ptr->real_fullness,rt_proxy_dev_ptr->timer_adjust_counter, \
			(uint32_t) (rt_proxy_dev_ptr->device_clock>>32), (uint32_t) (rt_proxy_dev_ptr->device_clock),  \
			(uint32_t) (temp>>32), (uint32_t) temp  ); //wallclk & devclk should have almost matching values, here

#endif

	if (rt_proxy_dev_ptr->real_fullness == 0)
	{
		adjustment = 0;
		rt_proxy_dev_ptr->timer_adjust_counter = 0;
	}
	else if (rt_proxy_dev_ptr->real_fullness >= rt_proxy_dev_ptr->upper_drift_thresh )
	{
		if (rt_proxy_dev_ptr->timer_adjust_counter == 0) //decrement every interrupt which is every block_transfer_time (1ms typically)
		{
			adjustment = -rt_proxy_dev_ptr->timer_adjust_value; //1 for RX and -1 for TX if using av timer.
			rt_proxy_dev_ptr->timer_adjust_counter = rt_proxy_dev_ptr->timer_adjust_counter_constant;
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x  timer adjustment = %d us, net adj=0x%x%x",\
					rt_proxy_dev_ptr->port_id, (int) adjustment, (unsigned int) (rt_proxy_dev_ptr->net_timer_adjust>>32), (unsigned int)(rt_proxy_dev_ptr->net_timer_adjust));
		}
		rt_proxy_dev_ptr->timer_adjust_counter--;
	}
	else if (rt_proxy_dev_ptr->real_fullness <= rt_proxy_dev_ptr->lower_drift_thresh )
	{
		if (rt_proxy_dev_ptr->timer_adjust_counter == 0) //decrement every interrupt which is every block_transfer_time (1ms typically)
		{
			adjustment = +rt_proxy_dev_ptr->timer_adjust_value; //-1 for RX and 1 for TX if using av timer.
			rt_proxy_dev_ptr->timer_adjust_counter = rt_proxy_dev_ptr->timer_adjust_counter_constant;
			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x  timer adjustment = %d us, net adj=0x%x%x",\
					rt_proxy_dev_ptr->port_id, (int)adjustment, (unsigned int) (rt_proxy_dev_ptr->net_timer_adjust>>32), (unsigned int)(rt_proxy_dev_ptr->net_timer_adjust));
		}
		rt_proxy_dev_ptr->timer_adjust_counter--;
	}
	else
	{
		adjustment = 0;
		rt_proxy_dev_ptr->timer_adjust_counter = 0; //so that next time thresh is reached, it'll apply adjustment immediately
	}

	//net accumulate since port start
	rt_proxy_dev_ptr->net_timer_adjust += adjustment;
	expected_timer_signal_us = (int64_t)(rt_proxy_dev_ptr->interrupt_counter * (int64_t)(dev_port->int_samples_per_period * NUM_US_PER_SEC))/dev_port->sample_rate;
	next_intr_set_time_us = rt_proxy_dev_ptr->init_device_clock_us + expected_timer_signal_us + adjustment;

	//if future tick is less than 200us away, then keep the tick farther away.
	//Note this screws up VFR counts & hence it's an error. this step is done only so that we do not get next int, before this one is complete.
	//this way, threadpool won't see the task as busy & ignore the signal (threadpool ignores a signal if previous signal is not completely processed yet.
	/*diff  = (int64_t) (next_intr_set_time_us - afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv));
   if ( diff <= AFE_RTPP_FUTURE_TIMER_GAP_US)
   {
      next_intr_set_time_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv) + block_transfer_time_us + adjustment;
      MSG_5(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: %x  underrun (timer),diff=0x%x%x, net adjust=0x%x%x",
               rt_proxy_dev_ptr->port_id, (unsigned int) (diff>>32),(unsigned int) (diff), (unsigned int) (rt_proxy_dev_ptr->net_timer_adjust>>32), (unsigned int)(rt_proxy_dev_ptr->net_timer_adjust));
   }*/

#ifdef RTPP_DEBUG_TIMER_ADJUST
	temp = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv);
	MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "RT Proxy Port: %x, devclk=0x%x%x, wallclk=0x%x%x, blk=0x%d, adjust=0x%d ", rt_proxy_dev_ptr->port_id,\
			(unsigned int) (rt_proxy_dev_ptr->device_clock>>32), (unsigned int) (rt_proxy_dev_ptr->device_clock),
			(unsigned int) (temp>>32), (unsigned int) temp, (unsigned int) (expected_timer_signal_us - rt_proxy_dev_ptr->device_clock),(unsigned int) adjustment);
#endif

	int num_set_timer_attempts=0;

	while ( (ADSP_FAILED(result = afe_time_srvr_timer_oneshot_start_absolute(dev_port->timer_ptr, next_intr_set_time_us))) && (num_set_timer_attempts++ < AFE_RTPP_MAX_SET_TIMER_ATTEMPTS) ) //10 trials & as long as there is a failure.
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "RT Proxy Port: %x: starting one shot timer  failed,num_set_timer_attempts=%d",rt_proxy_dev_ptr->port_id,num_set_timer_attempts);
		next_intr_set_time_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv) + (expected_timer_signal_us - rt_proxy_dev_ptr->device_clock) + adjustment; //try with future time. this will screw up VFR timing.
		rt_proxy_dev_ptr->interrupt_counter = 0;
		rt_proxy_dev_ptr->init_device_clock_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)dev_port->avt_drv);
	}

	rt_proxy_dev_ptr->device_clock = next_intr_set_time_us;

	return result;
}

ADSPResult afe_rt_proxy_port_get_adj_val(uint32_t *adj_counter, int64_t *adj_val, uint32_t sample_rate)
{
	ADSPResult result = ADSP_EOK;

	switch(sample_rate)
	{
	case AFE_PORT_SAMPLE_RATE_8K:
	case AFE_PORT_SAMPLE_RATE_16K:
	case AFE_PORT_SAMPLE_RATE_24K:
	case AFE_PORT_SAMPLE_RATE_32K:
	case AFE_PORT_SAMPLE_RATE_48K:
	case AFE_PORT_SAMPLE_RATE_96K:
	case AFE_PORT_SAMPLE_RATE_192K:
	case AFE_PORT_SAMPLE_RATE_384K:
	{
		*adj_counter = DRIFT_ADJUST_INTERVAL;
		*adj_val     = TIMER_RESOLUTION_US;
		break;
	}
	case AFE_PORT_SAMPLE_RATE_22_05K:
	case AFE_PORT_SAMPLE_RATE_44_1K:
	case AFE_PORT_SAMPLE_RATE_88_2K:
	case AFE_PORT_SAMPLE_RATE_176_4K:
	case AFE_PORT_SAMPLE_RATE_352_8K:
	{
		*adj_counter = DRIFT_ADJUST_INTERVAL;
		*adj_val     = TIMER_RESOLUTION_US;
		break;
	}
	default:
	{
		*adj_counter = 0;
		*adj_val     = 0;
		result = ADSP_EFAILED;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "timer adjustments queried for unsupported rate :%ld",sample_rate);
		break;
	}
	}
	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "adj_counter=%d, adj_val=%d",(int) *adj_counter,(int) *adj_val);
	return result;
}
/**
 * updates the accumulated avt drift for rt proxy ports
 */
void afe_update_avt_drift_for_rt_proxy_port(afe_dev_port_t *dev_port_ptr)
{
	afe_rt_proxy_port_dev_t *rt_proxy_dev_ptr;

	rt_proxy_dev_ptr = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(dev_port_ptr->intf)];

	//TODO: should convert net_timer_adjust to int32_t?
	//make the frame counter zero in the first AVt interrupt.
	afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), (int32_t) rt_proxy_dev_ptr->net_timer_adjust, dev_port_ptr->is_first_dma);

}

/**
 * For providing the delay introduced in RT proxy circular 
 * buffer 
 *
 * @param[in] port_id port id of the rt_proxy port
 *@param[out] circ_buf_delay_us is the delay introduced due to  
 *  	circular buffer

 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_driver_rt_proxy_circ_buf_delay(afe_dev_port_t *pDevPort, uint64_t* circ_buf_delay_us)
{
	afe_rt_proxy_port_dev_t  *rt_proxy_dev_ptr  = NULL;
	//For RT proxy port, the read (head) and write (tail) pointers of circular buffer are 
	//set apart by circular buffer size / 2 on first read or write.
	rt_proxy_dev_ptr  = &dev_rt_proxy[GET_RT_PROXY_PORT_INDEX(pDevPort->intf)];
	*circ_buf_delay_us = (((rt_proxy_dev_ptr->circ_buf.buffer_size >> 1) / rt_proxy_dev_ptr->bytes_per_sample) * 1000) / pDevPort->int_samples_per_period;

	return ADSP_EOK;

}
