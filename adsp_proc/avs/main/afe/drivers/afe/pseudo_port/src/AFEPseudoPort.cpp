/*========================================================================
   This file contains AFE Pseudo port implementation

Copyright 2009-2015 QUALCOMM Technologies, Inc. (QTI).
All Rights Reserved.
QUALCOMM Proprietary/GTDR

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/pseudo_port/src/AFEPseudoPort.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEInternal.h"
#include "AFEDeviceDriver.h"
#include "VfrDriverInternal.h"

/*==========================================================================
  Macro Declaration
  ========================================================================== */

/** @brief Enums for Pseudoport interface cfg version */
typedef enum
{
	PSEUDOPORT_CFG_V1 = 0x1
}afe_param_id_pseudoport_cfg_minor_version;

/*==========================================================================
  Globals & definitions
  ========================================================================== */
static afe_dev_port_t pseudo_port_objects[AFE_PORT_ID_PSEUDOPORT_RANGE_SIZE];
#define AFE_PP_MAX_SET_TIMER_ATTEMPTS (10)

//logging for timer adjustment debug
//#define PP_DEBUG_TIMER_ADJUST

/*==========================================================================
  Function definitions
  ========================================================================== */
static ADSPResult afe_pseudo_port_set_timer_for_fractional_rates(afe_dev_port_t *pDevPort, int16_t* buf_ptr, uint32_t num_samples);

/**
  @brief Perform Pseudo port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult                  result;
	uint16_t                    port_id;
	afe_dev_port                *afe_port_ptr = NULL;

	// Global initialization
	memset((char *) &pseudo_port_objects[0],0, sizeof(pseudo_port_objects));

	// initialize Pseudo ports
	for (port_id = AFE_PORT_ID_PSEUDOPORT_RANGE_START; port_id <= AFE_PORT_ID_PSEUDOPORT_RANGE_END; port_id++)
	{
		afe_port_ptr = &pseudo_port_objects[GET_PSEUDO_PORT_INDEX(port_id)];
		afe_svc_ptr->pseudo_ports_ptr[GET_PSEUDO_PORT_INDEX(port_id)] = afe_port_ptr;

		if (ADSP_FAILED(result = afe_svc_init_port(afe_port_ptr, port_id, 0)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to init port: %d, aborting", port_id);
			return result;
		}
		afe_port_ptr->afe_drv = NULL;
		afe_port_ptr->avt_drift_params.avt_drift_struct_ptr = NULL;
		if (afe_is_sample_rate_fract(afe_port_ptr->sample_rate))
		{
			afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_pseudo_port_set_timer_for_fractional_rates;
		}
		else
		{
			afe_port_ptr->afe_dev_process = NULL;
		}

	}

	return ADSP_EOK;
}

/**
  @brief Perform Pseudo port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	uint16_t        port_id;

	// de-initialize pseudo ports
	for (port_id = AFE_PORT_ID_PSEUDOPORT_RANGE_START; port_id <= AFE_PORT_ID_PSEUDOPORT_RANGE_END; port_id++)
	{
		afe_svc_deinit_port(afe_svc_ptr->pseudo_ports_ptr[GET_PSEUDO_PORT_INDEX(port_id)]);
	}

	return ADSP_EOK;
}

/**
  @brief Perform Pseudo port configuration

  @param[in] pDevPort: Pointer to port structure 
  @param[in] p_cfg   : Pointer to config payload

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_config (afe_dev_port_t* pDevPort, afe_param_id_pseudo_port_cfg_t *p_cfg)
{
	ADSPResult                    result = ADSP_EOK;

	/* Validate the bit width information */
	if ((AFE_BITS_PER_SAMPLE_16 != p_cfg->bit_width) && (AFE_BITS_PER_SAMPLE_24 != p_cfg->bit_width) && (AFE_BITS_PER_SAMPLE_32 != p_cfg->bit_width))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x, intf: %x", p_cfg->bit_width, pDevPort->intf);
		return ADSP_EUNSUPPORTED;
	}

	/* Validate the sampling rate information */
	if ((AFE_PORT_SAMPLE_RATE_8K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_16K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_48K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_96K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_192K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_384K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_44_1K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_88_2K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_176_4K != p_cfg->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_352_8K != p_cfg->sample_rate))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, intf: %x", p_cfg->sample_rate, pDevPort->intf);
		return ADSP_EUNSUPPORTED;
	}

	/* Validate the Channels information */
	if((p_cfg->num_channels > AFE_PORT_MAX_AUDIO_CHAN_CNT) || (p_cfg->num_channels < 1))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported Numm of Channels: %u, intf: %x", p_cfg->num_channels, pDevPort->intf);
		return ADSP_EUNSUPPORTED;
	}

	pDevPort->dev_bytes_per_channel = (AFE_BITS_PER_SAMPLE_16 == p_cfg->bit_width)? AFE_BYTES_PER_SAMPLE_TWO: AFE_BYTES_PER_SAMPLE_FOUR;
	pDevPort->dev_bit_width 		= p_cfg->bit_width;
	pDevPort->dev_sample_rate       = p_cfg->sample_rate;
	pDevPort->dev_channels        = p_cfg->num_channels;
	pDevPort->q_format_shift_factor = ((AFE_BITS_PER_SAMPLE_24 == p_cfg->bit_width)? QFORMAT_SHIFT_FACTOR : 0);

	//if applicable, overwrite the port media type with the device media type.
	//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
	afe_port_copy_media_type_dev_to_port(pDevPort);

	result = afe_port_get_frame_size(pDevPort->latency_mode, pDevPort->sample_rate, &pDevPort->int_samples_per_period);
	if(result != ADSP_EOK)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, " AFEPsuedoPort : Invalid sampling rate detected,Portid::0x%x",pDevPort->intf);
		return result;
	}
	if (afe_is_sample_rate_fract(pDevPort->sample_rate))
	{
		pDevPort->afe_dev_process = (afe_dev_port_proc_func)afe_pseudo_port_set_timer_for_fractional_rates;
	}
	else
	{
		pDevPort->afe_dev_process = NULL;
	}

	return result;
}

/**
  @brief Set param functionality for pseudo port config

  @param[in] pDevPort: Pointer to AFE port structure 
  @param[in] cfg: Pointer to config payload
  @param[in] payload_size: Size of the config payload

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_int_pseudo_port_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;

	switch (pDevPort->intf)
	{
	case AFE_PORT_ID_PSEUDOPORT_01:
	case AFE_PORT_ID_PSEUDOPORT_02:
	case AFE_PORT_ID_VOICE_RECORD_RX:
	case AFE_PORT_ID_VOICE_RECORD_TX:
	case AFE_PORT_ID_VOICE_PLAYBACK_TX:
	{
		// choose the appropriate config version
		uint32_t pseudoport_cfg_version = (uint32_t) *(cfg);
		if(pseudoport_cfg_version > AFE_API_VERSION_PSEUDO_PORT_CONFIG)
		{
			pseudoport_cfg_version = AFE_API_VERSION_PSEUDO_PORT_CONFIG;
		}

		switch (pseudoport_cfg_version)
		{
		case PSEUDOPORT_CFG_V1:
		{
			afe_param_id_pseudo_port_cfg_t *pseudoport_cfg_ptr =  (afe_param_id_pseudo_port_cfg_t *)cfg;

			if (payload_size < sizeof(afe_param_id_pseudo_port_cfg_t))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pseudoport Cfg fail: Invalid payload size: %ld, port_id: %d", payload_size, pDevPort->intf);
				result = ADSP_EBADPARAM;
			}
			else
			{
				result = afe_pseudo_port_config(pDevPort,pseudoport_cfg_ptr);
			}
		}
		break;
		default:
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pseudoport Cfg fail: Invalid  version %lu",pseudoport_cfg_version);
			return ADSP_EUNSUPPORTED;
		}
	}
	break;
	default:
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pseudoport Cfg fail: Invalid  port_id: %d", pDevPort->intf);
		result = ADSP_EUNSUPPORTED;
		break;
	}

	return result;
}

/**
  @brief Perform Pseudo port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_open(afe_dev_port_t *pDevPort)
{
	ADSPResult result = ADSP_EOK;
	uint64_t   block_transfer_time_us = 0;

	/* Create periodic timer for pseudo port if integer rate, else one shot timer*/
	block_transfer_time_us = ((uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC)/pDevPort->sample_rate);

	if (afe_is_sample_rate_fract(pDevPort->sample_rate))
	{
		result = afe_port_create_and_start_timer(pDevPort,
				QURT_ELITE_TIMER_ONESHOT_ABSOLUTE,
				block_transfer_time_us);
	}
	else
	{
		result = afe_port_create_and_start_timer(pDevPort,
				QURT_ELITE_TIMER_PERIODIC,
				block_transfer_time_us);
	}

	return result;
}

/**
  @brief Perform Pseudo port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_pseudo_port_close(afe_dev_port_t *pDevPort)
{
	ADSPResult result = ADSP_EOK;

	/* Delete the timer object */
	result = afe_port_delete_timer(pDevPort);

	return result;
}

void afe_pseudoport_get_vfr_drift(afe_dev_port_t *afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
	int32_t              drift_in_us;
	afe_drift_info_t     *p_drift_info;
	uint16_t             vfr_src_idx = vfr_state_ptr->vfr_src_index;

	p_drift_info = &afe_port_ptr->drift_info[vfr_src_idx];

	drift_in_us = (int32_t) ((p_drift_info->vfr_drift_info)>>32);

	/* If VFR source has been disabled and re-enabled, while the port is still running
      resync the port's drift info structure. */

	if (TRUE == vfr_state_ptr->is_first_vfr)
	{
		afe_drv_resync_port_vfr_drift_info(p_drift_info);
		p_drift_info->is_first_vfr = TRUE;
	}

	if (TRUE == p_drift_info->is_first_vfr)
	{
		p_drift_info->is_first_vfr = FALSE;
		drift_in_us = 0;
	}
	else
	{
		drift_in_us += (int32_t) ((VFR_FRAME_SIZE_IN_US - (vfr_state_ptr->cur_av_time_stamp - vfr_state_ptr->prev_av_time_stamp)));
	}

	// update vfr frame ctr and acc drift info
	qurt_elite_mutex_lock( &(afe_port_ptr->port_mutex) );
	if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
	{
		uint32_t temp = (uint32_t) (p_drift_info->vfr_drift_info) + 1;
		p_drift_info->vfr_drift_info = (uint64_t)(((int64_t)drift_in_us<<32)|((uint64_t)temp));
	}
	qurt_elite_mutex_unlock( &(afe_port_ptr->port_mutex) );

	return;
}

/**
 * Set the ONESHOT timer for every read/write
 *
 * @param[in] pDevPort
 *
 * @return ADSP_EOK on success, an error if error
 *
 * @dependencies
 * None
 */

static ADSPResult afe_pseudo_port_set_timer_for_fractional_rates(afe_dev_port_t *pDevPort, int16_t* buf_ptr, uint32_t num_samples)
{
	ADSPResult result = ADSP_EOK;
	uint64_t   expected_timer_signal_us = 0;
	uint64_t   elapsed_time_us = 0;
	int32_t    num_set_timer_attempts=0;

	/* Set periodic timer for pseudo port. For every new port interrupt, we try to set the next one shot timer based on init time and current running int counter. 
	 If this timer set fails, retry for 10 times so that stall can be avoided*/
	expected_timer_signal_us = pDevPort->init_port_clock + ((uint64_t)(pDevPort->running_int_counter * (uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC))/pDevPort->sample_rate);	

	while ( (ADSP_FAILED(result = afe_time_srvr_timer_oneshot_start_absolute(pDevPort->timer_ptr, expected_timer_signal_us))) && (num_set_timer_attempts++ < AFE_PP_MAX_SET_TIMER_ATTEMPTS) ) //10 trials & as long as there is a failure.
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Pseudo Port: %x: starting one shot timer  failed,num_set_timer_attempts=%d",pDevPort->intf,num_set_timer_attempts);
		elapsed_time_us = pDevPort->init_port_clock + ((uint64_t)((--pDevPort->running_int_counter) * (uint64_t)(pDevPort->int_samples_per_period * NUM_US_PER_SEC))/pDevPort->sample_rate);
		expected_timer_signal_us = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)pDevPort->avt_drv) + (expected_timer_signal_us - elapsed_time_us); //try with future time. this will screw up VFR timing.
		pDevPort->running_int_counter = 0;
		pDevPort->init_port_clock = afe_time_srvr_timer_get_time((avtimer_drv_handle_t)pDevPort->timer_ptr);
	}
#ifdef PP_DEBUG_TIMER_ADJUST
	MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Pseudo Port: %x  timer set to =0x%x%x",
			pDevPort->intf, (unsigned int) (expected_timer_signal_us>>32),(unsigned int) (expected_timer_signal_us));
#endif
	return result;
}
