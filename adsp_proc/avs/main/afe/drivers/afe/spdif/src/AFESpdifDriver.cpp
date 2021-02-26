/*========================================================================
This file contains main interface to the AFE Spdif driver

  Copyright (c) 2009-2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifDriver.cpp#1 $
 ====================================================================== */


/*=====================================================================
 Includes
 ======================================================================*/
#include "AFEDeviceDriver.h"
#include "adsp_afe_service_commands.h"
#include "AFEInterface.h"
#include "AFESpdifDriver_i.h"
#include "AFESpdifDriver.h"
#include "halspdif.h"
#include "hwd_devcfg.h"
#include "AFESpdifDriverUtils_i.h"
#include "AFESpdifClkDriver_i.h"
#include "AFEChStatusUtils.h"
#include "qurt_elite.h"
#include "avtimer_drv_api.h"


#define SPDIF_SIM_TEST
#undef SPDIF_SIM_TEST

#define Q28_TO_Q31_FORMAT_SHIFT_FACTOR         4

#define NUM_FRAME_PER_BLOCK                    192

#define MAX_NUM_BLOCK_CNT                      0xFFFF        //65536    

#define SPDIF_IST_STACK_SIZE_BYTES             1024

//static declarations for  AFEDev Ports.
static afe_dev_port_t port_rx_spdif; 

static spdif_state_struct_t  spdif_rx;

static afe_spdif_driver_t    afe_spdif_driver;

static afe_dev_avt_drift_t spdif_avt_drift;

/**< spdif interrupt IST info */
static qurt_elite_interrupt_ist_t spdif_ist_int_info;

ADSPResult afe_spdif_write(afe_dev_port_t *afe_port_ptr,  int16_t  *p_buffer, uint32_t num_samples);

// need to move it to the utils files as simple function for unit level test code.
ADSPResult afe_spdif_driver_init(void)
{
	ADSPResult      result = ADSP_EOK;
	lpasshwio_prop_spdif_struct_t *p_spdiftx_hw_info = &afe_spdif_driver.spdiftx_hw_info;

	if(SPDIF_STATE_NOINIT == afe_spdif_driver.state)
	{
		/* read spdif dev config data */
		if(ADSP_EOK == (result =  afe_spdif_device_cfg_init(p_spdiftx_hw_info)))
		{
			if(p_spdiftx_hw_info->hw_revision)
			{

				/* address mapping for virtual address */
				if(0 != p_spdiftx_hw_info->base_reg_phy_addr)
				{
					if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&(afe_spdif_driver.spdif_qmem_region),
							p_spdiftx_hw_info->base_reg_phy_addr, p_spdiftx_hw_info->reg_size, &(afe_spdif_driver.spdif_virtual_addr))))
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Error obtaining virtual addr for spdif");
					}
					else
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_SPDIF: virtual addr 0x%x phy addr 0x%x",(unsigned int)afe_spdif_driver.spdif_virtual_addr, (unsigned int)p_spdiftx_hw_info->base_reg_phy_addr );
					}
				}
				else
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Invalid spdif regsiter physical address");
					result = ADSP_EFAILED;
				}

				if(0 != p_spdiftx_hw_info->reset_reg_phy_addr)
				{
					if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&(afe_spdif_driver.spdif_reset_qmem_region),
							p_spdiftx_hw_info->reset_reg_phy_addr, p_spdiftx_hw_info->reset_reg_size, &(afe_spdif_driver.spdif_reset_virtual_addr))))
					{
						MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Error obtaining virtual addr for spdif reset");
					}
					else
					{
						MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_SPDIF: virtual addr 0x%x phy addr 0x%x for spdif reset",(unsigned int)afe_spdif_driver.spdif_virtual_addr, (unsigned int)p_spdiftx_hw_info->base_reg_phy_addr );
					}
				}
				else
				{
					MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Invalid spdif reset regsiter physical address");
					result = ADSP_EFAILED;
				}


				if(ADSP_EOK == result)
				{
					/* call spdif hal init */
					hal_spdif_init(p_spdiftx_hw_info->hw_revision, \
							p_spdiftx_hw_info->base_reg_phy_addr, \
							afe_spdif_driver.spdif_virtual_addr, \
							p_spdiftx_hw_info->reset_reg_phy_addr, \
							afe_spdif_driver.spdif_reset_virtual_addr);

					if(ADSP_FAILED(result = afe_spdif_init_clks())) /* register clock regime for clock control */
					{
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: failed to register clock regime status %d",result);
					}
				}

				/* set num of hw buffer */
				afe_spdif_driver.spdif_hw_buf_info.num_sub_buf = p_spdiftx_hw_info->num_hw_buf;
				afe_spdif_driver.state = SPDIF_STATE_INIT;
				MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_SPDIF: spdif driver init is done");
			}
			else
			{
				result = ADSP_EUNSUPPORTED;
			}
		}
	}

	return result;
}


ADSPResult afe_spdif_driver_deinit()
{
	ADSPResult result = ADSP_EOK;

	if(SPDIF_STATE_NOINIT == afe_spdif_driver.state)
	{
		return ADSP_EUNSUPPORTED;
	}

	/* release all resource allocated during init*/
	HwdDevCfg_DestoryVirtAddress(afe_spdif_driver.spdif_qmem_region);
	HwdDevCfg_DestoryVirtAddress(afe_spdif_driver.spdif_reset_qmem_region);

	/* deregister clock regime for clock control */
	if(ADSP_FAILED(result = afe_spdif_deinit_clks()))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: failed to register clock regime status %d",result);
	}

	afe_spdif_driver.state = SPDIF_STATE_NOINIT;
	return result;
}

ADSPResult afe_spdif_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult      result = ADSP_EOK;
	afe_dev_port    *dev_port_ptr = NULL;
	uint16_t        port_id, dir;
	spdif_state_struct_t *p_dev_state = NULL;

	if(SPDIF_STATE_NOINIT == afe_spdif_driver.state)
	{
		return ADSP_EUNSUPPORTED;
	}

	/* initlaize global data */
	/*init device port state data */
	dev_port_ptr = &port_rx_spdif;
	afe_spdif_driver.dev_port_ptr = dev_port_ptr;
	p_dev_state = &spdif_rx;

	port_id = AFE_PORT_ID_SPDIF_RX;

	dir = port_id&AFE_PORT_DIR_MASK;
	if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Fail to create port = %d, aborting", port_id);
		return result;
	}
	afe_svc_ptr->ports_spdif_ptr = dev_port_ptr;
	dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = &spdif_avt_drift;

	/* register device process function */
	dev_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_spdif_write;

	/*init device state structure */
	memset(p_dev_state, 0, sizeof(spdif_state_struct_t));

	/* register device state data */
	dev_port_ptr->afe_drv = (void *) p_dev_state;

	return result;
}

ADSPResult afe_spdif_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	if(SPDIF_STATE_NOINIT == afe_spdif_driver.state)
	{
		return ADSP_EUNSUPPORTED;
	}

	if (NULL != afe_svc_ptr->ports_spdif_ptr)
	{
		afe_svc_deinit_port(afe_svc_ptr->ports_spdif_ptr);
	}

	return ADSP_EOK;
}

#ifdef SPDIF_SIM_TEST
qurt_elite_timer_t   spdif_timer;
#endif

ADSPResult afe_spdif_open(afe_dev_port_t *p_dev_port)
{
	ADSPResult      result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *)p_dev_port->afe_drv;
	afe_spdif_driver_t *p_spdif_driver = &afe_spdif_driver;
	afe_spdif_hw_buf_info_t *p_hw_buf_info  = &afe_spdif_driver.spdif_hw_buf_info;
	p_dev_state->int_samples_per_period = p_dev_port->int_samples_per_period;
	char_t spdif_thread_name[] = "spdif_ist";

	/* register interrupt handler */
	if (ADSP_FAILED(result = qurt_elite_interrupt_register(&spdif_ist_int_info,(uint16_t)p_spdif_driver->spdiftx_hw_info.int_irq_no,
			afe_spdif_interrupt_callback, (void *)&afe_spdif_driver, spdif_thread_name, SPDIF_IST_STACK_SIZE_BYTES)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: failed to register spdif interrupt with status %d",result);
	}

	/*allocate buffer based on block transfer time : double buffer */
	if(ADSP_FAILED(result = afe_spdif_allocate_hardware_buf(p_dev_state, p_hw_buf_info)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Fail to allocate device memory port = %d, aborting", p_dev_port->intf);
	}
	else if(ADSP_FAILED(result = afe_spdif_start(p_spdif_driver, p_dev_state)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Fail to start spdif device port = %d, aborting", p_dev_port->intf);
		afe_spdif_deallocate_hardware_buf(p_hw_buf_info);
	}
	else
	{
		p_spdif_driver->state = SPDIF_STATE_START;
	}

#ifdef SPDIF_SIM_TEST
	result = avtimer_drv_create(&spdif_timer, QURT_ELITE_TIMER_ONESHOT_DURATION, &p_dev_port->dma_signal);
	avtimer_drv_start_duration(&spdif_timer, 900);
	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: afe spdif fisrt write ########");
#endif

	return result;
}

ADSPResult afe_spdif_close(afe_dev_port_t *p_dev_port)
{

	ADSPResult      result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *)p_dev_port->afe_drv;
	afe_spdif_hw_buf_info_t *p_hw_buf_info  = &afe_spdif_driver.spdif_hw_buf_info;

	if(ADSP_FAILED(result = afe_spdif_stop()))    /* disable spdif hardware*/
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: Fail to stop spdif device port = %d, aborting", p_dev_port->intf);

	}

	/* deregister interrupt with callback function. */
	if (ADSP_FAILED(qurt_elite_interrupt_unregister(&spdif_ist_int_info)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: failed to unregister resampler interrupt\n");
	}

	/* free hardware buffer */
	afe_spdif_deallocate_hardware_buf(p_hw_buf_info);

	/*reset device state structure */
	memset(p_dev_state, 0, sizeof(spdif_state_struct_t));

	/* reset driver state structure */
	afe_spdif_driver.int_stat.fifo_uflow_int = 0;
	afe_spdif_driver.int_stat.mc_err_int = 0;
	afe_spdif_driver.int_stat.ts_diff_int = 0;
	afe_spdif_driver.int_stat.tx_block_done_int = 0;

	afe_spdif_driver.tsdiff = 0;
	afe_spdif_driver.start_timestamp = 0;
	afe_spdif_driver.transfer_stat.prev_num_block = 0;
	afe_spdif_driver.transfer_stat.prev_num_frame = 0;
	afe_spdif_driver.transfer_stat.rollover_cnt = 0;

	afe_spdif_driver.buffer_index = 0;

	afe_spdif_driver.state = SPDIF_STATE_STOP;


	return result;
}

ADSPResult afe_spdif_set_cfg (afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *) p_dev_port->afe_drv;
	afe_param_id_spdif_audio_cfg_t *pspdif_cfg_ptr =  (afe_param_id_spdif_audio_cfg_t *)cfg;

	/* need to add error check for parameters. */
	if((16 != pspdif_cfg_ptr->bit_width) && (24 != pspdif_cfg_ptr->bit_width))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF:afe_spdif_set_cfg : unsupported bitwidth %d port 0x%x", p_dev_state->bit_width, p_dev_port->intf);
		return ADSP_EUNSUPPORTED;
	}

	if((2 < pspdif_cfg_ptr->num_channels) || (!pspdif_cfg_ptr->num_channels))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF:afe_spdif_set_cfg : unsupported num of channels %d port 0x%x", p_dev_state->num_channels, p_dev_port->intf);
		return ADSP_EUNSUPPORTED;
	}

	if ((AFE_PORT_SAMPLE_RATE_22_05K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_32K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_44_1K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_48K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_88_2K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_96K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_176_4K != pspdif_cfg_ptr->sample_rate)&&
			(AFE_PORT_SAMPLE_RATE_192K != pspdif_cfg_ptr->sample_rate))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, port: 0x%x", pspdif_cfg_ptr->sample_rate, p_dev_port->intf);
		return ADSP_EUNSUPPORTED;
	}

	if ((AFE_LINEAR_PCM_DATA != pspdif_cfg_ptr->data_format)&&
			(AFE_NON_LINEAR_DATA != pspdif_cfg_ptr->data_format))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported data format: %u, port: 0x%x", pspdif_cfg_ptr->data_format, p_dev_port->intf);
		return ADSP_EUNSUPPORTED;
	}

	p_dev_state->spdif_cfg_minor_version = pspdif_cfg_ptr->spdif_cfg_minor_version;
	p_dev_state->sample_rate = pspdif_cfg_ptr->sample_rate;
	p_dev_state->num_channels = pspdif_cfg_ptr->num_channels;
	p_dev_state->bit_width = pspdif_cfg_ptr->bit_width;
	p_dev_state->data_format = pspdif_cfg_ptr->data_format;

	if(ADSP_EOK == (result = afe_spdif_init_chstatus(p_dev_state)))
	{
		if(AFE_LINEAR_PCM_DATA == p_dev_state->data_format)
		{
			p_dev_port->dev_data_type = AFE_UNCOMPRESSED_DATA_PORT;
		}
		else if(AFE_NON_LINEAR_DATA == p_dev_state->data_format)
		{
			p_dev_port->dev_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
		}
		else
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF:afe_spdif_set_cfg : incorrect data format %d", p_dev_state->data_format);
			return ADSP_EBADPARAM;
		}

		p_dev_port->dev_bytes_per_channel = (p_dev_state->bit_width > 16) ? 4 : 2;
		p_dev_port->dev_bit_width = p_dev_state->bit_width;
		p_dev_port->dev_channels = p_dev_state->num_channels;
		p_dev_port->dev_sample_rate = p_dev_state->sample_rate;

		//if applicable, overwrite the port media type with the device media type.
		//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
		afe_port_copy_media_type_dev_to_port(p_dev_port);

		p_dev_port->is_interleaved = TRUE;
		afe_spdif_driver.state = SPDIF_STATE_CONFIG;
	}

	return result;
}


ADSPResult afe_spdif_get_cfg (afe_dev_port_t *p_dev_port, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	ADSPResult result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *) p_dev_port->afe_drv;
	afe_param_id_spdif_audio_cfg_t *pspdif_cfg_ptr =  (afe_param_id_spdif_audio_cfg_t *)param_buf_ptr;

	pspdif_cfg_ptr->spdif_cfg_minor_version = p_dev_state->spdif_cfg_minor_version;
	pspdif_cfg_ptr->sample_rate = p_dev_state->sample_rate;
	pspdif_cfg_ptr->num_channels = p_dev_state->num_channels;
	pspdif_cfg_ptr->bit_width = p_dev_state->bit_width;
	pspdif_cfg_ptr->data_format = p_dev_state->data_format;

	return result;
}

ADSPResult afe_spdif_set_chstatus_cfg(afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *) p_dev_port->afe_drv;
	afe_param_id_chstatus_cfg_t *p_spdif_chstatus =  (afe_param_id_chstatus_cfg_t *)cfg;
	chstatus_check_point_t chsts_check_param;
	uint32 temp_mask_32bit0, temp_mask_32bit1, temp_mask_32bit2, temp_mask_32bit3, temp_mask_32bit4, temp_mask_32bit5;
	uint32 temp_ch_status_32bit;

	temp_mask_32bit0 = 0;
	temp_mask_32bit1 = 0;
	temp_mask_32bit2 = 0;
	temp_mask_32bit3 = 0;
	temp_mask_32bit4 = 0;
	temp_mask_32bit5 = 0;

	if(SPDIF_STATE_CONFIG <= afe_spdif_driver.state)
	{
		chsts_check_param.bit_width    = p_dev_state->bit_width;
		chsts_check_param.data_format  = p_dev_state->data_format;
		chsts_check_param.num_channels = p_dev_state->num_channels;
		chsts_check_param.sample_rate  = p_dev_state->sample_rate;

		if(ADSP_EOK == (result = afe_validate_chstatus_cfg(p_spdif_chstatus, &chsts_check_param)))
		{
			p_dev_state->channel_status_type |= p_spdif_chstatus->ch_status_type;

			if(AFE_CH_STATUS_A == p_spdif_chstatus->ch_status_type)
			{
				temp_mask_32bit0 = (p_spdif_chstatus->ch_stat_mask[3]<<24) | (p_spdif_chstatus->ch_stat_mask[2]<<16) |
						(p_spdif_chstatus->ch_stat_mask[1]<<8) | p_spdif_chstatus->ch_stat_mask[0];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[3]<<24) | (p_spdif_chstatus->ch_stat_buf[2]<<16) |
						(p_spdif_chstatus->ch_stat_buf[1]<<8) | p_spdif_chstatus->ch_stat_buf[0];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 &=  (~temp_mask_32bit0);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 |=  (temp_ch_status_32bit & temp_mask_32bit0);

				temp_mask_32bit1 = (p_spdif_chstatus->ch_stat_mask[7]<<24) | (p_spdif_chstatus->ch_stat_mask[6]<<16) |
						(p_spdif_chstatus->ch_stat_mask[5]<<8) | p_spdif_chstatus->ch_stat_mask[4];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[7]<<24) | (p_spdif_chstatus->ch_stat_buf[6]<<16) |
						(p_spdif_chstatus->ch_stat_buf[5]<<8) | p_spdif_chstatus->ch_stat_buf[4];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 &=  (~temp_mask_32bit1);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 |=  (temp_ch_status_32bit & temp_mask_32bit1);

				temp_mask_32bit2 = (p_spdif_chstatus->ch_stat_mask[11]<<24) | (p_spdif_chstatus->ch_stat_mask[10]<<16) |
						(p_spdif_chstatus->ch_stat_mask[9]<<8) | p_spdif_chstatus->ch_stat_mask[8];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[11]<<24) | (p_spdif_chstatus->ch_stat_buf[10]<<16) |
						(p_spdif_chstatus->ch_stat_buf[9]<<8) | p_spdif_chstatus->ch_stat_buf[8];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 &=  (~temp_mask_32bit2);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 |=  (temp_ch_status_32bit & temp_mask_32bit2);

				temp_mask_32bit3 = (p_spdif_chstatus->ch_stat_mask[15]<<24) | (p_spdif_chstatus->ch_stat_mask[14]<<16) |
						(p_spdif_chstatus->ch_stat_mask[13]<<8) | p_spdif_chstatus->ch_stat_mask[12];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[15]<<24) | (p_spdif_chstatus->ch_stat_buf[14]<<16) |
						(p_spdif_chstatus->ch_stat_buf[13]<<8) | p_spdif_chstatus->ch_stat_buf[12];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 &=  (~temp_mask_32bit3);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 |=  (temp_ch_status_32bit & temp_mask_32bit3);

				temp_mask_32bit4 = (p_spdif_chstatus->ch_stat_mask[19]<<24) | (p_spdif_chstatus->ch_stat_mask[18]<<16) |
						(p_spdif_chstatus->ch_stat_mask[17]<<8) | p_spdif_chstatus->ch_stat_mask[16];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[19]<<24) | (p_spdif_chstatus->ch_stat_buf[18]<<16) |
						(p_spdif_chstatus->ch_stat_buf[17]<<8) | p_spdif_chstatus->ch_stat_buf[16];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 &=  (~temp_mask_32bit4);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 |=  (temp_ch_status_32bit & temp_mask_32bit4);

				temp_mask_32bit5 = (p_spdif_chstatus->ch_stat_mask[23]<<24) | (p_spdif_chstatus->ch_stat_mask[22]<<16) |
						(p_spdif_chstatus->ch_stat_mask[21]<<8) | p_spdif_chstatus->ch_stat_mask[20];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[23]<<24) | (p_spdif_chstatus->ch_stat_buf[22]<<16) |
						(p_spdif_chstatus->ch_stat_buf[21]<<8) | p_spdif_chstatus->ch_stat_buf[20];
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 &=  (~temp_mask_32bit5);
				p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 |=  (temp_ch_status_32bit & temp_mask_32bit5);

			}
			else if(AFE_CH_STATUS_B == p_spdif_chstatus->ch_status_type)
			{
				temp_mask_32bit0 = (p_spdif_chstatus->ch_stat_mask[3]<<24) | (p_spdif_chstatus->ch_stat_mask[2]<<16) |
						(p_spdif_chstatus->ch_stat_mask[1]<<8) | p_spdif_chstatus->ch_stat_mask[0];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[3]<<24) | (p_spdif_chstatus->ch_stat_buf[2]<<16) |
						(p_spdif_chstatus->ch_stat_buf[1]<<8) | p_spdif_chstatus->ch_stat_buf[0];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 &=  (~temp_mask_32bit0);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 |=  (temp_ch_status_32bit & temp_mask_32bit0);

				temp_mask_32bit1 = (p_spdif_chstatus->ch_stat_mask[7]<<24) | (p_spdif_chstatus->ch_stat_mask[6]<<16) |
						(p_spdif_chstatus->ch_stat_mask[5]<<8) | p_spdif_chstatus->ch_stat_mask[4];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[7]<<24) | (p_spdif_chstatus->ch_stat_buf[6]<<16) |
						(p_spdif_chstatus->ch_stat_buf[5]<<8) | p_spdif_chstatus->ch_stat_buf[4];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 &=  (~temp_mask_32bit1);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 |=  (temp_ch_status_32bit & temp_mask_32bit1);

				temp_mask_32bit2 = (p_spdif_chstatus->ch_stat_mask[11]<<24) | (p_spdif_chstatus->ch_stat_mask[10]<<16) |
						(p_spdif_chstatus->ch_stat_mask[9]<<8) | p_spdif_chstatus->ch_stat_mask[8];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[11]<<24) | (p_spdif_chstatus->ch_stat_buf[10]<<16) |
						(p_spdif_chstatus->ch_stat_buf[9]<<8) | p_spdif_chstatus->ch_stat_buf[8];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 &=  (~temp_mask_32bit2);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 |=  (temp_ch_status_32bit & temp_mask_32bit2);

				temp_mask_32bit3 = (p_spdif_chstatus->ch_stat_mask[15]<<24) | (p_spdif_chstatus->ch_stat_mask[14]<<16) |
						(p_spdif_chstatus->ch_stat_mask[13]<<8) | p_spdif_chstatus->ch_stat_mask[12];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[15]<<24) | (p_spdif_chstatus->ch_stat_buf[14]<<16) |
						(p_spdif_chstatus->ch_stat_buf[13]<<8) | p_spdif_chstatus->ch_stat_buf[12];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 &=  (~temp_mask_32bit3);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 |=  (temp_ch_status_32bit & temp_mask_32bit3);

				temp_mask_32bit4 = (p_spdif_chstatus->ch_stat_mask[19]<<24) | (p_spdif_chstatus->ch_stat_mask[18]<<16) |
						(p_spdif_chstatus->ch_stat_mask[17]<<8) | p_spdif_chstatus->ch_stat_mask[16];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[19]<<24) | (p_spdif_chstatus->ch_stat_buf[18]<<16) |
						(p_spdif_chstatus->ch_stat_buf[17]<<8) | p_spdif_chstatus->ch_stat_buf[16];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 &=  (~temp_mask_32bit4);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 |=  (temp_ch_status_32bit & temp_mask_32bit4);

				temp_mask_32bit5 = (p_spdif_chstatus->ch_stat_mask[23]<<24) | (p_spdif_chstatus->ch_stat_mask[22]<<16) |
						(p_spdif_chstatus->ch_stat_mask[21]<<8) | p_spdif_chstatus->ch_stat_mask[20];
				temp_ch_status_32bit = (p_spdif_chstatus->ch_stat_buf[23]<<24) | (p_spdif_chstatus->ch_stat_buf[22]<<16) |
						(p_spdif_chstatus->ch_stat_buf[21]<<8) | p_spdif_chstatus->ch_stat_buf[20];
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 &=  (~temp_mask_32bit5);
				p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 |=  (temp_ch_status_32bit & temp_mask_32bit5);

			}
		}

		if(SPDIF_STATE_START == afe_spdif_driver.state)
		{
			if((0x4 != temp_mask_32bit0) &&
					(0 != temp_mask_32bit1) &&
					(0 != temp_mask_32bit2) &&
					(0 != temp_mask_32bit3) &&
					(0 != temp_mask_32bit4) &&
					(0 != temp_mask_32bit5))
			{
				result = ADSP_EBADPARAM;
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF:afe_spdif_set_chstatus_cfg : error : try to change other than cp bit [state:%d]", afe_spdif_driver.state);
			}

			if(ADSP_EOK == result)
			{
				//fill the channel status
				if(AFE_CH_STATUS_A & p_dev_state->channel_status_type)
				{
					hal_spdif_write_tx_chsts_a(p_dev_state->tx_ch_sts_buf_a);
				}

				if(AFE_CH_STATUS_B & p_dev_state->channel_status_type)
				{
					hal_spdif_write_tx_chsts_b(p_dev_state->tx_ch_sts_buf_b);
				}
				//dynamic udpate of channel status table which is needed new register definition.
				MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_SPDIF:afe_spdif_set_chstatus_cfg : dynamic chsts update");
			}
		}
	}
	else
	{
		result = ADSP_ENOTREADY;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF:afe_spdif_set_chstatus_cfg : incorrect state %d", afe_spdif_driver.state);
	}
	return result;
}


ADSPResult afe_spdif_get_chstatus_cfg(afe_dev_port_t *p_dev_port, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	ADSPResult result = ADSP_EOK;
	spdif_state_struct_t *p_dev_state = (spdif_state_struct_t *) p_dev_port->afe_drv;
	afe_param_id_chstatus_cfg_t *p_spdif_cfg =  (afe_param_id_chstatus_cfg_t *)param_buf_ptr;

	if(AFE_CH_STATUS_A == p_spdif_cfg->ch_status_type)
	{
		p_spdif_cfg->ch_stat_buf[0] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 & 0xff);
		p_spdif_cfg->ch_stat_buf[1] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[2] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[3] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf0 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[4] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 & 0xff);
		p_spdif_cfg->ch_stat_buf[5] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[6] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[7] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf1 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[8] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 & 0xff);
		p_spdif_cfg->ch_stat_buf[9] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[10] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[11] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf2 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[12] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 & 0xff);
		p_spdif_cfg->ch_stat_buf[13] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[14] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[15] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf3 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[16] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 & 0xff);
		p_spdif_cfg->ch_stat_buf[17] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[18] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[19] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf4 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[20] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 & 0xff);
		p_spdif_cfg->ch_stat_buf[21] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[22] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[23] = (p_dev_state->tx_ch_sts_buf_a.ch_sts_buf5 & 0xff000000) >> 24;
	}
	else
	{
		p_spdif_cfg->ch_stat_buf[0] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 & 0xff);
		p_spdif_cfg->ch_stat_buf[1] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[2] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[3] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf0 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[4] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 & 0xff);
		p_spdif_cfg->ch_stat_buf[5] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[6] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[7] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf1 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[8] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 & 0xff);
		p_spdif_cfg->ch_stat_buf[9] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[10] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[11] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf2 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[12] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 & 0xff);
		p_spdif_cfg->ch_stat_buf[13] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[14] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[15] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf3 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[16] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 & 0xff);
		p_spdif_cfg->ch_stat_buf[17] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[18] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[19] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf4 & 0xff000000) >> 24;
		p_spdif_cfg->ch_stat_buf[20] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 & 0xff);
		p_spdif_cfg->ch_stat_buf[21] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 & 0xff00) >> 8;
		p_spdif_cfg->ch_stat_buf[22] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 & 0xff0000) >> 16;
		p_spdif_cfg->ch_stat_buf[23] = (p_dev_state->tx_ch_sts_buf_b.ch_sts_buf5 & 0xff000000) >> 24;

	}

	return result;
}


ADSPResult afe_spdif_set_clk_cfg(afe_dev_port_t *p_dev_port, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;
	afe_param_id_spdif_clk_cfg_t *p_spdif_clk_cfg =  (afe_param_id_spdif_clk_cfg_t *)cfg;
	uint32                 clk_value = p_spdif_clk_cfg->clk_value;
	uint32                 clk_root =  p_spdif_clk_cfg->clk_root;

	result = afe_spdif_set_clks(clk_root, clk_value);

	return result;
}

ADSPResult afe_spdif_write(afe_dev_port_t *afe_port_ptr,  int16_t *p_buffer, uint32_t num_samples)
{
	ADSPResult result = ADSP_EOK;
	afe_spdif_driver_t *p_spdif_driver = &afe_spdif_driver;
	uint32 *p_sub_buf_phy_addr = NULL;
	uint32 *p_sub_buf_virt_addr = NULL;
	uint32 fill_level_release = 0;
	uint32 num_bytes_to_copy;
	afe_spdif_hw_buf_info_t *p_hw_buf_info = &afe_spdif_driver.spdif_hw_buf_info;
	int64  *ptr_dst_64;
	int32  *p_buf_frm_ptr32;
	uint32 total_num_samples, i;

	/* Q27 to Q31 conversion if it is 24bit width for both mono and stereo*/
	if(4 == afe_port_ptr->bytes_per_channel)
	{
		/* if the number of samples are odd */
		if(1 == afe_port_ptr->channels)
		{
			/* Calculate the total number of samples*/
			total_num_samples = (num_samples * afe_port_ptr->channels);

			p_buf_frm_ptr32 = (int32 *)p_buffer;

			for(i=0; i<total_num_samples; i++)
			{
				p_buf_frm_ptr32[i] = (p_buf_frm_ptr32[i] << Q28_TO_Q31_FORMAT_SHIFT_FACTOR);
			}
		}
		else
		{
			ptr_dst_64 = (int64_t *)p_buffer;

			for(i=0; i < num_samples; i++)
			{
				/* Q27 to Q31 conversion, two samples at a time */
				(*ptr_dst_64) = Q6_P_vaslw_PI(*ptr_dst_64, Q28_TO_Q31_FORMAT_SHIFT_FACTOR);
				ptr_dst_64++;
			}
		}
	}

	hal_spdif_read_tx_release_level(&fill_level_release);

#ifdef SPDIF_SIM_TEST
	fill_level_release = 1;
#endif

	if(fill_level_release <  p_hw_buf_info->num_sub_buf) // number of sub buffer which is still pending:  no buffer to write
	{

		//copy the data to the hardware buffer
		num_bytes_to_copy = p_hw_buf_info->hw_buf_length_per_int*4;

		uint32 ringBufPos = p_spdif_driver->buffer_index;

		//setup buffer pointer
		if(p_spdif_driver->buffer_index < p_hw_buf_info->num_sub_buf)
		{
			p_sub_buf_phy_addr = p_hw_buf_info->hw_buf_phys_addr + (p_hw_buf_info->hw_buf_length_per_int*ringBufPos);
			p_sub_buf_virt_addr = p_hw_buf_info->hw_buf_virt_addr+ (p_hw_buf_info->hw_buf_length_per_int*ringBufPos);
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: afe spdif write buffer index error overflow");
			return result;
		}

		p_spdif_driver->buffer_index++;
		if(p_spdif_driver->buffer_index ==  p_hw_buf_info->num_sub_buf)
		{
			p_spdif_driver->buffer_index = 0;
		}

		memscpy(p_sub_buf_virt_addr, num_bytes_to_copy, p_buffer, num_bytes_to_copy);

		if(QURT_EOK != qurt_mem_cache_clean((qurt_addr_t)p_sub_buf_virt_addr, num_bytes_to_copy, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE))
		{
			result = ADSP_EFAILED;
		}
		else
		{
			hal_spdif_write_tx_subbuf_fifo(p_sub_buf_phy_addr, 0, 0);
			//To get TS DIFF in spdif
			hal_spdif_write_timestamp(p_spdif_driver->start_timestamp);
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: afe spdif write overflow: fill_level_release %d", (int)fill_level_release);
	}

#ifdef SPDIF_SIM_TEST
	avtimer_drv_start_duration(&spdif_timer, 975);
	MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: afe spdif write ########");
#endif

	return result;
}


/* NOTE: there is no way to get the current device time becuase the statistic register can not cover the long duration */

ADSPResult afe_get_time_for_spdif_port(afe_dev_port_t *p_dev_port, uint64 *curr_time)
{ 
	uint32_t n_frame = 0;
	uint32_t n_block = 0;
	uint32 time_per_sample = 0;
	int64 temp;
	int64 num_samples = 0;
	int32 format;
	uint32 cur_rollover_cnt = 0;
	afe_spdif_driver_t *p_spdif_driver = &afe_spdif_driver;
	//uint32 num_samples_per_int = afe_spdif_driver.spdif_hw_buf_info.num_samples_per_int;
	//uint32 num_tx_block_done_int = &afe_spdif_driver.int_stat.tx_block_done_int;

	if(SPDIF_STATE_NOINIT == p_spdif_driver->state)
	{
		return ADSP_EUNSUPPORTED;
	}

	hal_spdif_read_tx_stats(&n_frame, &n_block);
	if(p_spdif_driver->transfer_stat.prev_num_block > n_block)
	{
		p_spdif_driver->transfer_stat.rollover_cnt++;
		cur_rollover_cnt = p_spdif_driver->transfer_stat.rollover_cnt;
	}

	//calculate total number of samples that has been transmitted.
	num_samples = (cur_rollover_cnt*MAX_NUM_BLOCK_CNT*NUM_FRAME_PER_BLOCK) + (n_block*NUM_FRAME_PER_BLOCK) + n_frame;

	//get time per sample.
	afe_spdif_gettime_per_sample(p_dev_port->sample_rate, &time_per_sample,&format);

	//caculate the time for all samples that has been transmitted.
	temp = num_samples*time_per_sample;

	//conver it to Q25 format.
	*curr_time = (int64)((temp + (((int64)0x01)<<(format-1))) >> format);

	return ADSP_EOK;
}

ADSPResult afe_update_avt_drift_for_spdif_port(afe_dev_port_t *dev_port_ptr)
{
	afe_dev_avt_drift_t *avt_drift_ptr = dev_port_ptr->avt_drift_params.avt_drift_struct_ptr;
	uint64_t diff1 = 0, diff2 = 0;

	if (1 == afe_spdif_driver.int_stat.ts_diff_int)
	{
		avt_drift_ptr->init_avtimer_timestamp  = dev_port_ptr->port_av_timestamp;
		afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), 0, TRUE);
	}
	else
	{
		diff1 = dev_port_ptr->port_av_timestamp - avt_drift_ptr->init_avtimer_timestamp;

		//For HW latching, device time is just the total time in terms of DMA samples transferred
		diff2 = afe_spdif_driver.int_stat.ts_diff_int * ((int64_t)(dev_port_ptr->int_samples_per_period *NUM_US_PER_SEC)/dev_port_ptr->sample_rate);

		int32_t acc_avt_drift_us =  (int32_t) (diff1 - diff2) ; //+ve -> avtimer faster -> since avtimer counted more.
		/* note: above is same as: acc_avt_drift_us +=  (curr_dev_time - prev_dev_time) - (curr_avt_timestamp - prev_avt_timestamp)*/
		afe_drv_update_avt_drift_info_element(&(dev_port_ptr->drift_info[AVT_DRIFT_INFO_IDX].avt_drift_info), acc_avt_drift_us, FALSE);
#ifdef AVT_BASED_AFE_DRIFT_DEBUG
		avt_drift_ptr->avtimer_timestamp = dev_port_ptr->port_av_timestamp;
		avt_drift_ptr->dev_time =   afe_spdif_driver.int_stat.ts_diff_int * dev_port_ptr->block_transfer_time_us;
#endif // AVT_BASED_AFE_DRIFT_DEBUG
	}

	return ADSP_EOK;
}


uint64_t afe_spdif_get_tsdiff_timestamp(afe_dev_port_t *afe_port_ptr)
{
	uint64_t tsdiff_int_timestamp = 0;

	hal_spdif_get_tsdiff(&afe_spdif_driver.tsdiff);
	tsdiff_int_timestamp = afe_spdif_driver.tsdiff; //STC

	tsdiff_int_timestamp = avtimer_drv_convert_hw_tick_to_time(tsdiff_int_timestamp);

	return tsdiff_int_timestamp;
}

MmpmCoreIdType afe_get_spdif_mmpm_coreid()
{
	return  MMPM_CORE_ID_LPASS_SPDIF;
}

