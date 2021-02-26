/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriver.cpp#3 $
$DateTime: 2017/12/05 00:40:48 $
$Author: pwbldsvc $
$Change: 14982000 $
$Revision: #3 $

FILE:     AFESlimbusDriver.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2011-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/slimbus/src/AFESlimbusDriver.cpp#3 $ $DateTime: 2017/12/05 00:40:48 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
08-08-2010  mspk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "ddislimbus.h"
#include "DALDeviceId.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "qurt_elite.h"
#include "AFESlimbusDriver.h"
#include "AFESlimbusDriverUtils_i.h"
#include "AFESlimbusTimerUtils_i.h"
#include "AFESlimbusSlaveCfg_i.h"
#include "adsp_afe_service_commands.h"
#include "aud_sync_hal.h"
#include "AFEDeviceDriver.h"
#include "AFEDmaManager.h"
#include "audio_basic_op.h"

/*=====================================================================
 Globals
 ======================================================================*/

// AFE SLIMBUS Driver instances for different SLIMBUS HW blocks
afe_slimbus_device_config_t  afe_sb_dev_cfg[MAX_SLIMBUS_HW_MODULES];

static afe_dev_port_t  	            afe_slimbus_ports_global[AFE_PORT_ID_SLIMBUS_RANGE_SIZE] __attribute__((__section__(".bss.lpa_audio_static_svc_afe")));
/*=====================================================================
 Static Functions
 ======================================================================*/

/*=====================================================================
 Define Functions
 ======================================================================*/
/* Left/Right Shift helper Macros */
#define REG_SHIFT_R(x, shift)           (((uint32_t)x) >> (shift))
#define REG_SHIFT_L(x, shift)           ((x) << (shift))
#define REG_MASK(x,mask)                ((x) & (mask))
#define REG_BYTE__M                     0xFF
/* Bit Field get/set MACROS */
/* Get Page number from original register Address */
#define REG_GET_PAGE_NUM(addr)          REG_SHIFT_R((addr),8)
/* Get Page register address from processor id */
#define REG_GET_PAGE_REG_ADDR(proc_id)  REG_SHIFT_L((proc_id),8)
/* Set Translated Address from original register address and processor id . i.e.
 * remove the page no and replace it with processor id */
#define REG_SET_TRANSLATED_ADDR(addr, proc_id) \
		(addr = REG_SHIFT_L((proc_id),8) |\
				REG_MASK((addr),REG_BYTE__M) )

/*=====================================================================
 Functions
 ======================================================================*/
/**
  @brief Perform SLIMBUS driver deallocation and cleanup

  @param[in] afe_port_ptr pointer for AFE port

  @return  void

 */
static void afe_slimbus_driver_end(afe_dev_port_t *afe_port_ptr)
{
	if(!afe_port_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed, Null port ptr!");
		return;
	}

	/**<For device recovery, need to retain driver structure,
	 * which is allocated dynamically during port config*/
	if(AFE_DEV_RECOVERY != afe_port_ptr->dev_proc_seq)
	{
		if(afe_port_ptr->afe_drv)
		{
			qurt_elite_memory_aligned_free(afe_port_ptr->afe_drv);
			afe_port_ptr->afe_drv = NULL;
		}

		afe_port_ptr->avt_drift_params.avt_drift_struct_ptr = NULL;
	}
}

/**
  @brief Perform SLIMBUS driver allocation and initialization.

  @param[in] afe_port_ptr pointer for AFE port

  @return  ADSP_EOK on success, an error code on error

 */
static ADSPResult afe_slimbus_driver_reinit(afe_dev_port_t *afe_port_ptr)
{
	if(!afe_port_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed, Null port ptr!");
		return ADSP_EFAILED;
	}

	if(afe_port_ptr->afe_drv)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed, driver is already allocated for port: 0x%x!", afe_port_ptr->intf);
		return ADSP_EUNEXPECTED;
	}

	ADSPResult result = ADSP_EOK;
	afe_slimbus_state_struct_t *dev_state_ptr = NULL;

	if(NULL
			== (dev_state_ptr = (afe_slimbus_state_struct_t *)qurt_elite_memory_aligned_malloc(
					sizeof(afe_slimbus_state_struct_t), 8, QURT_ELITE_HEAP_DEFAULT)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed, cannot allocate driver state memory for port: 0x%lx!", afe_port_ptr->intf);
		result = ADSP_ENOMEMORY;
		goto __bailout_resource;
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Slimbus driver alloc successfully at 0x%lx for port: 0x%lx!",
				dev_state_ptr, afe_port_ptr->intf);
	}

	memset(dev_state_ptr, 0, sizeof(afe_slimbus_state_struct_t));
	afe_port_ptr->afe_drv = (void *)dev_state_ptr;
	afe_port_ptr->avt_drift_params.avt_drift_struct_ptr = &dev_state_ptr->avt;

	return result;

	__bailout_resource:
	afe_slimbus_driver_end(afe_port_ptr);

	return result;
}

/**
  @brief Perform SLIMBUS port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult                  result;
	uint16_t                    port_id;
	afe_dev_port                *afe_port_ptr = NULL;

	// Global initialization
	memset((char *) &afe_slimbus_ports_global[0],0, sizeof(afe_slimbus_ports_global));

	// initialize slimbus related ports
	for (port_id = AFE_PORT_ID_SLIMBUS_RANGE_START; port_id <= AFE_PORT_ID_SLIMBUS_RANGE_END; port_id++)
	{
		afe_port_ptr = &afe_slimbus_ports_global[GET_SLIMBUS_PORT_INDEX(port_id)];
		afe_svc_ptr->ports_slimbus_ptr[GET_SLIMBUS_PORT_INDEX(port_id)] = afe_port_ptr;

		if (ADSP_FAILED(result = afe_svc_init_port(afe_port_ptr, port_id, (port_id&AFE_PORT_DIR_MASK))))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Fail to init port: %d, aborting\n", port_id);
			return result;
		}

		if (IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
		{
			afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_read;
		}
		else
		{
			afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_write;
		}

	  afe_port_ptr->is_interleaved = FALSE;
	}

	return ADSP_EOK;
}

/**
  @brief Perform SLIMBUS port de-initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	uint16_t        port_id;

	// de-initialize slimbus related ports
	for (port_id = AFE_PORT_ID_SLIMBUS_RANGE_START; port_id <= AFE_PORT_ID_SLIMBUS_RANGE_END; port_id++)
	{
		afe_svc_deinit_port(afe_svc_ptr->ports_slimbus_ptr[GET_SLIMBUS_PORT_INDEX(port_id)]);
	}

	return ADSP_EOK;
}

/**
  @brief Perform one time initialization of the AFE Slimbus driver

  @param[in] afe_slimbus_id ID for the requested slimbus HW block

  @return  valid 'p_sb_driver' on success, NULL on error

 */
ADSPResult afe_slimbus_driver_init(void)
{
	ADSPResult result = ADSP_EOK;

#ifdef SLIMBUS_DBG_ENABLE
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SLIMbus: Start of afe_slimbus_driver_init()");
#endif //SLIMBUS_DBG_ENABLE

	/* SLIMbus device config init */
	if ( ADSP_EOK != (result = (afe_slimbus_dev_cfg_init())) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SLIMbus: Device Config failed");

		return ADSP_EFAILED;
	}

#ifdef SLIMBUS_DBG_ENABLE
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SLIMbus: End of afe_slimbus_driver_init()");
#endif //SLIMBUS_DBG_ENABLE

	// return the afe slimbus driver node pointer
	return  ADSP_EOK;
}

/**
  @brief Perform de-initialization of the AFE Slimbus driver

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_driver_deinit()
{
	ADSPResult                   result = ADSP_EOK;

	for(uint32_t mod_idx = 0; mod_idx < MAX_SLIMBUS_HW_MODULES; mod_idx++)
	{
	  if (TRUE == afe_sb_dev_cfg[mod_idx].b_is_initialized)
	  {
	    MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "De-initializing SLIMBUS HW driver, slimbus dev id: %d", afe_sb_dev_cfg[mod_idx].afe_slimbus_id);

	    // un-initialize the driver before closing
	    afe_sb_dev_cfg[mod_idx].b_is_initialized = FALSE;
	  }
	}

	return result;
}


ADSPResult afe_slimbus_dev_cfg_init(void)
{
	DALSYSPropertyVar prop_var;
	const char *devId="AFESlimbus";
	DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);
	lpasshwio_prop_slimbus_struct_t  *sb_prop_dev_cfg_ptr = NULL;

	if(DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(devId, pHandle))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus DevCfg: Error getting Slimbus device handle");
		return ADSP_EFAILED;
	}

	/* Read property ptr */
	if( DAL_SUCCESS != DALSYS_GetPropertyValue(pHandle, "AFESlimbusPropStructPtr", 0, &prop_var) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus DevCfg: Error getting Slimbus Property Struct Ptr");
		return ADSP_EFAILED;
	}

	sb_prop_dev_cfg_ptr = (lpasshwio_prop_slimbus_struct_t *)prop_var.Val.pStruct;

	for(uint32_t mod_idx =0 ; mod_idx < MAX_SLIMBUS_HW_MODULES ; mod_idx++)
	{
	  afe_sb_dev_cfg[mod_idx].avtimer_latch_type = sb_prop_dev_cfg_ptr->avtimer_latch_type;

	  afe_sb_dev_cfg[mod_idx].b_is_hw_support_multi_endpoint = (bool_t)sb_prop_dev_cfg_ptr->sb_multi_ep_info[mod_idx].is_multi_endpoint;

	  /* store the list of max end points supported per master in global structure*/
	  if(TRUE == afe_sb_dev_cfg[mod_idx].b_is_hw_support_multi_endpoint)
	  {
	    memscpy( (uint32_t *) &afe_sb_dev_cfg[mod_idx].max_eps_per_master[0],
	             sizeof(afe_sb_dev_cfg[mod_idx].max_eps_per_master),
	             (uint32_t *) &sb_prop_dev_cfg_ptr->sb_multi_ep_info[mod_idx].max_eps_per_master[0],
	             sizeof(sb_prop_dev_cfg_ptr->sb_multi_ep_info[mod_idx].max_eps_per_master) );
	  }

	  /* Store the SLIMbus ID */
	  afe_sb_dev_cfg[mod_idx].afe_slimbus_id = mod_idx;

	  /* Set the init flag */
	  afe_sb_dev_cfg[mod_idx].b_is_initialized = TRUE;
	}

	return ADSP_EOK;
}


/**
  @brief Perform configuration of SLIMBUS streams for the given
         configuration

  @param[in] afe_port_ptr pointer to AFE device port structure
  @param[in] p_afesb_cfg pointer to AFE port slimbus config
        strcutre
  @param[in] cfg_payload_size config struct payload size
  @param[in] cfg_ver -> config version

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_slimbus_cfg (afe_dev_port_t* afe_port_ptr, int8_t *p_cfg, uint16_t cfg_payload_size, bool is_it_shared_chan, uint32_t sample_rate)
{
	ADSPResult result = ADSP_EOK;

	/* If this is the first set cfg for the interface, allocate the driver state */
	if(NULL == afe_port_ptr->afe_drv)
	{
		if(ADSP_FAILED(result = afe_slimbus_driver_reinit(afe_port_ptr)))
		{
			return result;
		}
	}

	afe_slimbus_state_struct_t *p_dev_state = (afe_slimbus_state_struct_t *) afe_port_ptr->afe_drv;
	afe_sb_cfg_info_t *cfg_ptr = &p_dev_state->afe_sb_cfg;

	// do the mem clean of the dev-state struct
	memset((char *) p_dev_state, 0, sizeof(afe_slimbus_state_struct_t));

	cfg_ptr->is_it_share_chan = is_it_shared_chan;

	if (TRUE == is_it_shared_chan)
	{
		// choose the appropriate config version
		uint32_t cfg_ver = (uint32_t) *(p_cfg);
		if(cfg_ver > AFE_API_VERSION_SLIMBUS_CONFIG)
		{
			cfg_ver = AFE_API_VERSION_SLIMBUS_CONFIG;
		}

		cfg_ptr->cfg_version = cfg_ver;
		switch (cfg_ver)
		{
		case SB_CFG_V1:
		{
			afe_param_id_slimbus_cfg_t *p_sb_cfg = (afe_param_id_slimbus_cfg_t *) p_cfg;
			if (cfg_payload_size < sizeof(afe_param_id_slimbus_cfg_t))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus Config Fails, Bad Param");
				return ADSP_EBADPARAM;
			}

			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE Slimbus port configuration for port 0x%x "
					"slimbus_dev_id %d data_format %d", afe_port_ptr->intf,
					p_sb_cfg->slimbus_dev_id, p_sb_cfg->data_format);

			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "sample_rate %ld bit_width %d num_channels %d",
					p_sb_cfg->sample_rate, p_sb_cfg->bit_width, p_sb_cfg->num_channels);

			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "shared_channel_mapping[0] %d shared_channel_mapping[1] %d "
					"shared_channel_mapping[2] %d shared_channel_mapping[3] %d",
					p_sb_cfg->shared_channel_mapping[0], p_sb_cfg->shared_channel_mapping[1],
					p_sb_cfg->shared_channel_mapping[2], p_sb_cfg->shared_channel_mapping[3]);

			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "shared_channel_mapping[4] %d shared_channel_mapping[5] %d "
					"shared_channel_mapping[6] %d shared_channel_mapping[7] %d",
					p_sb_cfg->shared_channel_mapping[4], p_sb_cfg->shared_channel_mapping[5],
					p_sb_cfg->shared_channel_mapping[6], p_sb_cfg->shared_channel_mapping[7]);


			if ((AFE_SLIMBUS_DEVICE_1 != p_sb_cfg->slimbus_dev_id)&&
					(AFE_SLIMBUS_DEVICE_2 != p_sb_cfg->slimbus_dev_id))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported slimbus device ID: %d for port 0x%x", p_sb_cfg->slimbus_dev_id, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if ((AFE_BITS_PER_SAMPLE_16 != p_sb_cfg->bit_width)&&
			    (AFE_BITS_PER_SAMPLE_24 != p_sb_cfg->bit_width) &&
			    (AFE_BITS_PER_SAMPLE_32 != p_sb_cfg->bit_width))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit_width: %d for port 0x%x", p_sb_cfg->bit_width, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if ((p_sb_cfg->num_channels < 1) || (p_sb_cfg->num_channels > AFE_PORT_MAX_AUDIO_CHAN_CNT))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %d for port 0x%x", p_sb_cfg->num_channels, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if ((AFE_PORT_SAMPLE_RATE_8K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_16K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_24K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_32K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_48K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_96K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_192K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_384K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_22_05K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_44_1K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_88_2K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_176_4K != p_sb_cfg->sample_rate)&&
					(AFE_PORT_SAMPLE_RATE_352_8K != p_sb_cfg->sample_rate))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, for port: 0x%x", p_sb_cfg->sample_rate, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if((AFE_SB_DATA_FORMAT_NOT_INDICATED != p_sb_cfg->data_format) &&
					(AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT != p_sb_cfg->data_format) &&
					(AFE_SB_DATA_FORMAT_DSD_DOP != p_sb_cfg->data_format) &&
					(AFE_SB_DATA_FORMAT_GENERIC_COMPRESSED != p_sb_cfg->data_format))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported data_format: %lu, for port: 0x%x", p_sb_cfg->data_format, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			/* AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT is supported with 32 bitwidth */
			if((AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT == p_sb_cfg->data_format) && ((AFE_BITS_PER_SAMPLE_32 != p_sb_cfg->bit_width)))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT data format is not supported with bitwidth %u, port: 0x%x",
						p_sb_cfg->bit_width, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			/* Determining the qformat shift factor based on bit width. This shift factor is being used for 24bit and 32bit only*/
			if(AFE_BITS_PER_SAMPLE_24 == p_sb_cfg->bit_width)
			{
				afe_port_ptr->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
			}
			else
			{
				afe_port_ptr->q_format_shift_factor = 0;
			}

			cfg_ptr->bit_width = p_sb_cfg->bit_width;
			cfg_ptr->bytes_per_channel = (cfg_ptr->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
			cfg_ptr->num_channels = cfg_ptr->num_slave_ports = p_sb_cfg->num_channels;
			cfg_ptr->data_format = p_sb_cfg->data_format;
			cfg_ptr->slimbus_dev_id = p_sb_cfg->slimbus_dev_id;
			cfg_ptr->sample_rate = p_sb_cfg->sample_rate;
			memscpy((char *) &cfg_ptr->shared_channel_mapping[0], sizeof(cfg_ptr->shared_channel_mapping),
					(char *) &p_sb_cfg->shared_channel_mapping[0], sizeof(cfg_ptr->shared_channel_mapping));
		}
		break;

		default:
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus Config Fails, Bad Param");
			return ADSP_EUNSUPPORTED;
		}
	}
	else
	{
		// choose the appropriate config version
		uint32_t cfg_ver = (uint32_t) *(p_cfg);
		if(cfg_ver > AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG)
		{
			cfg_ver = AFE_API_VERSION_SLIMBUS_SLAVE_PORT_CFG;
		}

		cfg_ptr->cfg_version = cfg_ver;
		switch (cfg_ver)
		{
		case 0x1:
		{
			afe_slimbus_slave_port_cfg_param_v1_t *p_sb_cfg = (afe_slimbus_slave_port_cfg_param_v1_t *) p_cfg;
			if (cfg_payload_size < sizeof(afe_slimbus_slave_port_cfg_param_v1_t))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus Config Fails, Bad Param");
				return ADSP_EBADPARAM;
			}

			if ((AFE_SLIMBUS_DEVICE_1 != p_sb_cfg->slimbus_dev_id)&&
					(AFE_SLIMBUS_DEVICE_2 != p_sb_cfg->slimbus_dev_id))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported slimbus device ID: %d for port 0x%x", p_sb_cfg->slimbus_dev_id, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if ((AFE_BITS_PER_SAMPLE_16 != p_sb_cfg->bit_width)&&
			    (AFE_BITS_PER_SAMPLE_24 != p_sb_cfg->bit_width)&&
			    (AFE_BITS_PER_SAMPLE_32 != p_sb_cfg->bit_width))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit_width: %d for port 0x%x", p_sb_cfg->bit_width, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}

			if ((p_sb_cfg->num_chnnels < 1) || (p_sb_cfg->num_chnnels > AFE_PORT_MAX_AUDIO_CHAN_CNT))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported num_channels: %d for port 0x%x", p_sb_cfg->num_chnnels, afe_port_ptr->intf);
				return ADSP_EUNSUPPORTED;
			}


			cfg_ptr->bit_width = p_sb_cfg->bit_width;
			cfg_ptr->bytes_per_channel = (cfg_ptr->bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
			cfg_ptr->num_channels = cfg_ptr->num_slave_ports = p_sb_cfg->num_chnnels;
			cfg_ptr->data_format = p_sb_cfg->data_format;
			cfg_ptr->slimbus_dev_id = p_sb_cfg->slimbus_dev_id;
			cfg_ptr->sample_rate = sample_rate;
			cfg_ptr->slave_dev_pgd_la = p_sb_cfg->slave_dev_pgd_la;
			cfg_ptr->slave_dev.slave_dev_la = p_sb_cfg->slave_dev_intfdev_la;
			cfg_ptr->slave_dev.uses_paging = FALSE;

			memscpy((char *) &cfg_ptr->slave_port_mapping[0], sizeof(cfg_ptr->slave_port_mapping),
					(char *) &p_sb_cfg->slave_port_mapping[0], sizeof(cfg_ptr->slave_port_mapping));
		}
		break;

		default:
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slimbus Config Fails, Bad Param");
			return ADSP_EUNSUPPORTED;
		}
	}

	if (AFE_PORT_MAX_AUDIO_CHAN_CNT < cfg_ptr->num_channels)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported number of channels");
		return ADSP_EUNSUPPORTED;
	}

	switch (cfg_ptr->bit_width)
	{
	case AFE_BITS_PER_SAMPLE_16:
		// packed and alignment doesn't matter
		p_dev_state->pack_mode = SLIMBUS_PORT_O_PACKED;
		p_dev_state->samples_per_dword = 2;
		break;
	case AFE_BITS_PER_SAMPLE_24:
		// non-packed and MSB aligned
		p_dev_state->pack_mode = SLIMBUS_PORT_O_ALIGN_MSB;
		p_dev_state->samples_per_dword = 1;
		break;
	case AFE_BITS_PER_SAMPLE_32:
		// None packed mode
		p_dev_state->pack_mode = SLIMBUS_PORT_O_NONE;
		p_dev_state->samples_per_dword = 1;
		break;
	default:
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid bit width");
		return ADSP_EUNSUPPORTED;
	}

	p_dev_state->afe_port_ptr = afe_port_ptr;

	// choose the master and slave device directions based on the requested configuration
	if (IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
	{
		p_dev_state->direction = SLIMBUS_PORT_SINK_FLOW;
	}
	else
	{
		p_dev_state->direction = SLIMBUS_PORT_SOURCE_FLOW;
	}

	// store the device state information in port structure for future needs
	afe_port_ptr->dev_data_type = AFE_UNCOMPRESSED_DATA_PORT;
	if(AFE_SB_DATA_FORMAT_DSD_DOP == cfg_ptr->data_format)
	{
		afe_port_ptr->dev_data_type = AFE_DSD_DOP_COMPRESSED_DATA_PORT;
	}
	else if(AFE_SB_DATA_FORMAT_GENERIC_COMPRESSED == cfg_ptr->data_format)
	{
		afe_port_ptr->dev_data_type = AFE_GENERIC_COMPRESSED_DATA_PORT;
	}

	/* In the case of AFEDevDataLinearPcmPacked16Bit, 32bit sample from device is unpacked to 2 16bit samples
        and they are considered as two different channels. So, Number of channels and bitwidth of port need to be
        updated accordingly */
	if(AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT == cfg_ptr->data_format)
	{
		/* Port bit width is half of device bitwidth */
		afe_port_ptr->dev_bit_width = cfg_ptr->bit_width >> 1;

		/* Port num channels is twice the device num channels */
		afe_port_ptr->dev_channels = p_dev_state->afe_sb_cfg.num_channels << 1;
	}
	else
	{
		afe_port_ptr->dev_bit_width = cfg_ptr->bit_width;
		afe_port_ptr->dev_channels = p_dev_state->afe_sb_cfg.num_channels;
	}
	afe_port_ptr->dev_bytes_per_channel = (afe_port_ptr->dev_bit_width > AFE_BITS_PER_SAMPLE_16) ? AFE_BYTES_PER_SAMPLE_FOUR : AFE_BYTES_PER_SAMPLE_TWO;
	afe_port_ptr->dev_sample_rate = p_dev_state->afe_sb_cfg.sample_rate;

	//if applicable, overwrite the port media type with the device media type.
	//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
	afe_port_copy_media_type_dev_to_port(afe_port_ptr);

	p_dev_state->device_open_success = FALSE;
	return ADSP_EOK;
}

ADSPResult afe_get_slimbus_cfg (afe_dev_port_t *afe_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	/* If get is called without a set cfg, return error */
	if(NULL == afe_port_ptr->afe_drv)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Slimbus cfg not yet set for port: 0x%x", afe_port_ptr->intf);
		return ADSP_EFAILED;
	}

	afe_slimbus_state_struct_t *p_dev_state = (afe_slimbus_state_struct_t *) afe_port_ptr->afe_drv;
	uint32_t cfg_ver = p_dev_state->afe_sb_cfg.cfg_version;

	switch (cfg_ver)
	{
	case SB_CFG_V1:
	{
		*params_buffer_len_req_ptr = sizeof(afe_param_id_slimbus_cfg_t);
		if(*params_buffer_len_req_ptr > payload_size )
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: SLIMBUS cfg required size = %d, given size = %lu",
					*params_buffer_len_req_ptr, payload_size);
			return ADSP_ENOMEMORY;
		}
		afe_param_id_slimbus_cfg_t *p_sb_cfg = (afe_param_id_slimbus_cfg_t *) param_buf_ptr;

		p_sb_cfg->bit_width =  p_dev_state->afe_sb_cfg.bit_width;
		p_sb_cfg->data_format =  p_dev_state->afe_sb_cfg.data_format;
		p_sb_cfg->num_channels =  p_dev_state->afe_sb_cfg.num_channels;
		p_sb_cfg->sample_rate =  p_dev_state->afe_sb_cfg.sample_rate;
		p_sb_cfg->sb_cfg_minor_version =  cfg_ver;
		p_sb_cfg->slimbus_dev_id = p_dev_state->afe_sb_cfg.slimbus_dev_id;

		memscpy(&(p_sb_cfg->shared_channel_mapping[0]), sizeof(p_sb_cfg->shared_channel_mapping),
				&(p_dev_state->afe_sb_cfg.shared_channel_mapping[0]), sizeof(p_sb_cfg->shared_channel_mapping));
	}
	break;

	default:
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SLIMBUS cfg Get Cmd Cfg fail: InCompatible cfg version: %ld, port_id: %d",
				cfg_ver, afe_port_ptr->intf);
		return ADSP_EBADPARAM;
	}
	break;
	}

	return ADSP_EOK;
}

/**
  @brief Open the configured SLIMBUS streams for data transfers

  @param[in] afe_port_ptr pointer to AFE device port structure

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_open (afe_dev_port_t* afe_port_ptr)
{
  ADSPResult              result = ADSP_EOK;
  DALBOOL                 bWatermark = FALSE;
  uint16_t                master_port_num, desc_num, mult_chan_group;
  uint32                  uFifoWord = 0, num_master_ports;
  afe_sb_master_port_info_t    *p_stream_info;
  DalDeviceHandle         *p_core_driver = NULL;
  uint32_t                num_sleep_timeouts, elapsed_time_in_msec;
  int32_t int_samples_per_period;

  afe_slimbus_state_struct_t *p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;

  if(NULL == p_dev_state)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
          "Failed to close slimbus for intf: 0x%x as driver handle is NULL", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  /* Get the handle to AFE slimbus driver */
  if (NULL == (p_dev_state->p_sb_driver = (afe_slimbus_driver_t *)afe_slimbus_init_dev_handle(p_dev_state->afe_sb_cfg.slimbus_dev_id)))
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to init core driver handle");
    return ADSP_EFAILED;
  }

  // Assigning appropriate slimbus interface function based on data format
  if(AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT == p_dev_state->afe_sb_cfg.data_format)
  {
    if(IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
    {
      if(p_dev_state->p_sb_driver->is_it_multi_end_point)
      {
        /* afe_slimbus_multi_ep_read function internally takes care of 16bit packed data*/
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_multi_ep_read;
      }
      else
      {
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_read_32bit;
      }
    }
    else
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "16bit Packed Data format is not supported in the Rx direction for port 0x%x", afe_port_ptr->intf);
      return ADSP_EUNSUPPORTED;
    }
  }
  else
  {
    if(IS_IT_TX_PORT_AFE_ID(afe_port_ptr->intf))
    {
      if(p_dev_state->p_sb_driver->is_it_multi_end_point)
      {
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_multi_ep_read;
      }
      else
      {
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_read;
      }
    }
    else
    {
      if(p_dev_state->p_sb_driver->is_it_multi_end_point)
      {
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_multi_ep_write;
      }
      else
      {
        afe_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_slimbus_write;
      }
    }
  }

  /* Open the SLIMbus core driver */
  if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_dev_state->p_sb_driver)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver, port_id: 0x%x", afe_port_ptr->intf);
    return ADSP_EFAILED;
  }

  p_dev_state->device_open_success = TRUE;

  p_core_driver = p_dev_state->p_sb_driver->p_core_driver;

  /* Get the AV timer latch type as per device config */
  afe_port_ptr->avt_drift_params.avtimer_latch_type = afe_sb_dev_cfg[p_dev_state->afe_sb_cfg.slimbus_dev_id].avtimer_latch_type;

  // take the sampling rate info & calculate the transfer size
  p_dev_state->afe_sb_cfg.sample_rate = afe_port_ptr->dev_sample_rate;
  afe_get_sample_rate_index(p_dev_state->afe_sb_cfg.sample_rate, &p_dev_state->sample_rate_index);
  if (-1 == p_dev_state->sample_rate_index)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate");
    return ADSP_EUNSUPPORTED;
  }

  int_samples_per_period = p_dev_state->afe_port_ptr->dev_int_samples_per_period;
  p_dev_state->bam_trfr_size_per_ch = (int_samples_per_period)*(p_dev_state->afe_sb_cfg.bytes_per_channel);

  result = afe_sb_open_stream_ports(p_dev_state);

  if(ADSP_EOK != result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to open stream ports for port id 0x%x, 0x%x", afe_port_ptr->intf, result);
    return ADSP_EFAILED;
  }

  result = afe_sb_open_data_channels(p_dev_state);

  if(ADSP_EOK != result)
  {
    MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to open data channels for port id 0x%x, 0x%x", afe_port_ptr->intf, result);
    return ADSP_EFAILED;
  }

  if (FALSE == p_dev_state->afe_sb_cfg.is_it_share_chan)
  {
    mult_chan_group = TRUE;
    if(1 == p_dev_state->afe_sb_cfg.num_channels)
    {
      mult_chan_group = FALSE;
    }
    if (ADSP_EOK != (result = afe_slimbus_slave_port_config(p_dev_state, mult_chan_group)))
    {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Slave Port cfg fails, 0x%x", result);
      return ADSP_EFAILED;
    }
  }

  afe_port_ptr->is_first_interrupt = TRUE;

  num_master_ports = p_dev_state->afe_sb_cfg.num_master_ports;
  p_stream_info = p_dev_state->a_master_port_info;

  // initiate BAM transfers
  for (master_port_num = 0; master_port_num < num_master_ports; master_port_num++, p_stream_info++)
  {
    // Submit the descriptors to BAM for the data transfer
    for (desc_num = 0; desc_num < (p_dev_state->num_descptrs); desc_num++)
    {
      // Cache flsuhing to ensure that physical memory gets updated
      qurt_elite_memorymap_cache_flush(p_stream_info->cust_desc_info[desc_num].virt_addr, p_stream_info->IOVec[desc_num].uSize);

      result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT,
                                            &p_stream_info->IOVec[desc_num], &p_stream_info->cust_desc_info[desc_num]);
      if (DAL_SUCCESS != result)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit descriptor to the BAM, 0x%x", result);
        return ADSP_EFAILED;
      }
    }
  }

  SlimBusChannelReqType h_chan_cfg;
  memset(&h_chan_cfg, 0, sizeof(h_chan_cfg));
  h_chan_cfg.uSampleSize_bits = p_dev_state->afe_sb_cfg.bit_width;
  h_chan_cfg.uRateMultiplier = p_dev_state->afe_sb_cfg.sample_rate;
  h_chan_cfg.eBaseSampleRate = SLIMBUS_RATE_FAM_1_HZ;
  h_chan_cfg.eDesiredProtocol = SLIMBUS_PROTO_AUTO_ISO;

  if(AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT == p_dev_state->afe_sb_cfg.data_format)
  {
    h_chan_cfg.eDataFormat = (SlimBusDataFormatType) AFE_SB_DATA_FORMAT_NOT_INDICATED;
  }
  else
  {
    h_chan_cfg.eDataFormat = (SlimBusDataFormatType) p_dev_state->afe_sb_cfg.data_format;
  }

  h_chan_cfg.eSidebandFormat = SLIMBUS_SIDEBAND_FORMAT_NOT_APPLICABLE;
  h_chan_cfg.eOptions = (SlimBusChannelOptionsType) (SLIMBUS_CHAN_O_AUTO_ACTIVATE|SLIMBUS_CHAN_O_PRESERVE_ORDERING);

  if(p_dev_state->p_sb_driver->is_it_multi_end_point)
  {
    /* Apply channel configuration to channel group of each multi end point master port */
    for(master_port_num=0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++)
    {
      // apply channel configuration to the group of streams
      result |= DalSlimBus_NextDefineDataChannel(p_core_driver, p_dev_state->a_h_multi_ep_chan_group[master_port_num], &h_chan_cfg);
    }
  }
  else
  {
    /* Legacy case, need to apply for a single channel group */

    // apply channel configuration to the group of streams
    result = DalSlimBus_NextDefineDataChannel(p_core_driver, p_dev_state->h_chan_group, &h_chan_cfg);
  }

  if (DAL_SUCCESS != result)
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to activate the data channel, 0x%x", result);
    return ADSP_EFAILED;
  }

  // Don't apply reconfig until FIFO gets filled with initial data
  if (SLIMBUS_PORT_SOURCE_FLOW == p_dev_state->direction)
  {
    // In case of speaker path, ensure that FIFO gets some data before enabling the data channel.
    // this is to avoid under-flow port event in the begining
    // We need to wait for all ports as BAM might not gaurentee to operate on multi-channel grouping order
    p_stream_info = p_dev_state->a_master_port_info;
    for (master_port_num = 0; master_port_num < num_master_ports; master_port_num++, p_stream_info++)
    {
      num_sleep_timeouts = 0;
      do
      {
        result = DalSlimBus_GetPortFifoStatus(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &uFifoWord, &bWatermark);

        /* Sleep duration is 250 usec. 4 such sleeps is a 1 msec wait.
         * Divide the sleep count by 4 to get the elapsed time in msec
         */
        elapsed_time_in_msec = num_sleep_timeouts >> 2;
        if (FIFO_WATERMARK_TIMEOUT_IN_MSEC == elapsed_time_in_msec)
          break;
        //sleep for .25 msec so as to not read the slimbus FIFO status continuously
        qurt_elite_timer_sleep(SLIMBUS_FIFO_SLEEP_DURATION);
        num_sleep_timeouts++;
      } while (!bWatermark);

      if (!bWatermark)
      {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Time out waiting for master port %x FIFO watermark to be hit", afe_port_ptr->intf);
        return ADSP_EFAILED;
      }

      if ((DAL_SUCCESS != result))
      {
        MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to fill master port FIFO, result = 0x%x, uFifoWord = %lu", result, uFifoWord);
        return ADSP_EFAILED;
      }
    }
  }

  // all the above configurations will take effect with the help of DoReconfigNow()
  if (DAL_SUCCESS != (result = DalSlimBus_DoReconfigureNow(p_core_driver)))
  {
    MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to do ReconfigNow(), 0x%x", result);
    return ADSP_EFAILED;
  }

  return ADSP_EOK;
}

/**
  @brief Close the SLIMBUS streams associated with the current
         AFE port.

  @param[in] afe_port_ptr pointer AFE dev port structure

 */
ADSPResult afe_slimbus_close(afe_dev_port_t* afe_port_ptr)
{
	ADSPResult  result = ADSP_EOK, curr_result;
	afe_slimbus_state_struct_t *p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;

	if(NULL == p_dev_state)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed to close slimbus for intf: 0x%x as driver handle is NULL", afe_port_ptr->intf);
		return ADSP_EFAILED;
	}

	if (NULL == p_dev_state->p_sb_driver)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Invalid SLIMBUS driver handle, port_id: 0x%x", afe_port_ptr->intf);
		return ADSP_EOK;
	}

	// close slimbus data channels
	if( ADSP_EOK != (curr_result = afe_sb_close_data_channels(p_dev_state)) )
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close SLIMbus data channels, port_id: 0x%x", afe_port_ptr->intf);
		result |= curr_result;
	}

	curr_result = afe_sb_close_stream_ports(p_dev_state);

	// close stream specific ports
	if(ADSP_EOK != curr_result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close SLIMbus stream ports, port_id: 0x%x", afe_port_ptr->intf);
		result |= curr_result;
	}

	/* If the slimbus_open() failed but device open was successful
       we still need to close the opened device.*/
	if(TRUE == p_dev_state->device_open_success)
	{
		result |= afe_slimbus_core_driver_close((void *)p_dev_state->p_sb_driver);
		p_dev_state->device_open_success = FALSE;
	}

	if (NULL != p_dev_state->p_sb_driver)
	{
		afe_slimbus_deinit_dev_handle((void *)p_dev_state->p_sb_driver);
		p_dev_state->p_sb_driver = NULL;
	}

	afe_slimbus_driver_end(afe_port_ptr);

	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close SLIMbus device port_id: 0x%x", afe_port_ptr->intf);
		result = ADSP_EFAILED;
	}

	return result;
}

/**
 * @brief Reads the de-interleaved data from working buffer and
 *        writes the data to descriptor buffers (of master ports
 *        that supports multi end point) in interleaved fashion.
 *
 * For a 4 channel 3 sample/channel signal : each literal is a sample.
 * Assume 2 master ports, each assigned with 2 channels
 *
 * from working buffer:|                       C1C1C1 C2C2C2 C3C3C3 C4C4C4                  |
 *
 *                     |  BAM buffer from master port 0   |   BAM buffer from master port 1 |
 * to BAM buffers     :|          C1C2 C1C2 C1C2          |          C3C4 C3C4 C3C4         |
 *
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] pBuffer - data buffer to be pushed into DMA buffer
 *
 * @param[in] numSamples - number of samples to be copied
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 */
ADSPResult afe_slimbus_multi_ep_write(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
  ADSPResult result = ADSP_EOK;
  SlimBusBamIOVecType IOVec;
  afe_sb_cust_desc_info_t IOVec_custom_data, *IOVec_custom_data_ptr;
  afe_slimbus_state_struct_t *p_dev_state= (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
  afe_sb_master_port_info_t *p_stream_info=(afe_sb_master_port_info_t *) p_dev_state->a_master_port_info;
  DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;
  uint16_t         master_port_num, samp, ch_idx, num_ch_per_port;

  IOVec_custom_data_ptr = &IOVec_custom_data;

  // Iterate the procedure for each master port
  for(master_port_num = 0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++, p_stream_info++)
  {
    // Get the free descriptor from the descriptor queue
    result = DalSlimBus_GetBamIOVecEx(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) &IOVec_custom_data_ptr);
    if (DAL_SUCCESS == result &&  0 != IOVec_custom_data_ptr->virt_addr)
    {
      // Copy the render samples in to the memory pointed by the descriptor

      /* Get the number of channels assigned to this master port*/
      num_ch_per_port = p_dev_state->a_num_ch_per_port[master_port_num];

      if(AFE_BITS_PER_SAMPLE_16 == p_dev_state->afe_sb_cfg.bit_width)
      {
        int16_t *src_ptr_16 = (int16_t *)pBuffer;
        int16_t *dst_ptr_16 = (int16_t *)IOVec_custom_data_ptr->virt_addr;

        /* Interleave the data from local buffer upto number of channels
         * assigned for master port and store it in descriptor buffer*/
        for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
        {
          for (samp = 0; samp < numSamples; samp++)
          {
            *(dst_ptr_16 + samp*num_ch_per_port + ch_idx) = *(src_ptr_16 + ch_idx*numSamples + samp);
          }
        }

        /* Move the local buffer pointer, that points
         * location to store data from next descriptor */
        pBuffer = pBuffer + numSamples*num_ch_per_port;
      }
      else if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_sb_cfg.bit_width)
      {
        int32_t *src_ptr_32 = (int32_t *)pBuffer;
        int32_t *dst_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;

        /* Interleave the data from local buffer upto number of channels
         *  assigned for master port and store it in descriptor buffer*/
        for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
        {
          for (samp = 0; samp < numSamples; samp++)
          {
            *(dst_ptr_32 + samp*num_ch_per_port + ch_idx) =s32_shl_s32_sat(*(src_ptr_32 + ch_idx*numSamples + samp),QFORMAT_SHIFT_FACTOR) ;
          }
        }

        /* Move the local buffer pointer, that points
         * location to store data from next descriptor */
        pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples*num_ch_per_port );
      }
      else if(AFE_BITS_PER_SAMPLE_32 == p_dev_state->afe_sb_cfg.bit_width)
      {
        int32_t *src_ptr_32 = (int32_t *)pBuffer;
        int32_t *dst_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;

        /* Store the data from local buffer to descriptor payload in
         * interleaved fashion.*/
        for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
        {
          for (samp = 0; samp < numSamples; samp++)
          {
            *(dst_ptr_32 + samp*num_ch_per_port + ch_idx) = *(src_ptr_32 + ch_idx*numSamples + samp);
          }
        }

        /* Move the local buffer pointer, that points
         * location to store data from next descriptor */
        pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples*num_ch_per_port);
      }

      // Cache flush to ensure that physical memory gets updated
      afe_dma_buff_cache_flush((uint32_t)IOVec_custom_data_ptr->virt_addr, (uint32_t)IOVec.uSize);

      // Submit the descriptor to BAM pipe for transferring to FIFO in slimbus hw
      result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) IOVec_custom_data_ptr);
      if (DAL_SUCCESS != result)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit the descriptor to BAM");
      }
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dropping data. Descriptor not available");
    }
  }

  return result;
}

/**
 * @brief Reads the interleaved data from descriptor buffer
 *       (master ports that supports multi end point) and
 *       writes the data to working buffer in de-interleaved fashion.
 *
 * For a 4 channel 3 sample/channel signal : each literal is a sample.
 * assume 2 master ports, each assigned with 2 channels
 *
 *                       |  BAM buffer from master port 0   |   BAM buffer from master port 1 |
 * from BAM buffers    : |        C1C2  C1LC2  C1C2         |         C3C4  C3C4  C3C4        |
 *
 * to working buffer   : |                      C1C1C1 C2C2C2 C3C3C3 C4C4C4                   |
 *
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] pBuffer - buffer to store the fetched data
 *
 * @param[in] numSamples - number of samples to be fetched
 *
 * @return  ADSP_EOK on success, an error code on error
 *
 * @dependencies
 * None.
 */
ADSPResult afe_slimbus_multi_ep_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
  ADSPResult result = ADSP_EOK;
  SlimBusBamIOVecType IOVec;
  afe_sb_cust_desc_info_t IOVec_custom_data, *IOVec_custom_data_ptr;
  afe_slimbus_state_struct_t *p_dev_state= (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
  afe_sb_master_port_info_t *p_stream_info=(afe_sb_master_port_info_t *) p_dev_state->a_master_port_info;
  DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;
  uint16_t         master_port_num, samp, ch_idx, num_ch_per_port;

  IOVec_custom_data_ptr = &IOVec_custom_data;

  // Go through for all the channels to get the transfered descriptor
  for(master_port_num = 0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++, p_stream_info++)
  {
    // Read the descriptor from descriptor queue
    result = DalSlimBus_GetBamIOVecEx(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) &IOVec_custom_data_ptr);
    if (DAL_SUCCESS == result &&  0 != IOVec_custom_data_ptr->virt_addr)
    {
      num_ch_per_port = p_dev_state->a_num_ch_per_port[master_port_num];

      // Invalidate the cache lines on successful arrival
      afe_dma_buff_cache_invalidate((uint32_t)IOVec_custom_data_ptr->virt_addr, (uint32_t)IOVec.uSize);

      if(AFE_BITS_PER_SAMPLE_16 == p_dev_state->afe_sb_cfg.bit_width)
      {
        int16_t *src_ptr_16 = (int16_t *)IOVec_custom_data_ptr->virt_addr;
        int16_t *dst_ptr_16 = (int16_t *)pBuffer;

        /* De-interleave the descriptor payload and store in working buffer */
        for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
        {
          for (samp = 0; samp < numSamples; samp++)
          {
            *(dst_ptr_16 + ch_idx*numSamples + samp) = *(src_ptr_16 + samp*num_ch_per_port + ch_idx);
          }
        }

        /*Move the working buffer pointer, equivalent to size of descriptor payload*/
        pBuffer = pBuffer + numSamples*num_ch_per_port;
      }
      else if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_sb_cfg.bit_width)
      {
        int32_t *src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;
        int32_t *dst_ptr_32 = (int32_t *)pBuffer;

        for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
        {
          for (samp = 0; samp < numSamples; samp++)
          {
            *(dst_ptr_32 + ch_idx*numSamples + samp) = (*(src_ptr_32 + samp*num_ch_per_port + ch_idx)) >> QFORMAT_SHIFT_FACTOR;
          }
        }

        pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples*num_ch_per_port );
      }
      else if(AFE_BITS_PER_SAMPLE_32 == p_dev_state->afe_sb_cfg.bit_width)
      {
        /*two 16-bit data packed in 32-bit*/
        if(AFE_SB_DATA_FORMAT_LINEAR_PCM_PACKED_16BIT == p_dev_state->afe_sb_cfg.data_format)
        {
          /*
            * For a 4 channel 3 sample/channel signal.
            * Assume 2 master ports, each assigned with 2 channels.
            * Two different channels 16bit data (Vi0 Ij1) is packed into one 32-bit Ci
            *
            *                       |  BAM buffer from master port 0               |   BAM buffer from master port 1              |
            * from BAM buffers    : |        C1C2  C1LC2  C1C2                     |         C3C4  C3C4  C3C4                     |
            *
            *Expanded form of 32-bit data into two 16-bit data, those related to two different channels
            *                       |V10I11 V20I21   V10I11 V20I21   V10I11 V20I21 | V30I31 V40I41   V30I31 V40I41  V30I31 V40I41 |
            *
            * store the data in de-interleaved fashion
            * to working buffer   : |    V10V10V10 I11I11I11 V20V20V20 I21I21I21   |   V30V30V30 I31I31I31 V40V40V40 I41I41I41    |
            * */

          // Copy the descriptor payload into local buffer
          int16_t *dst_ptr_16 = (int16_t *)pBuffer;
          int32_t *src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;

          /* Unpack the 32bit data to two 16bit and de-interleave them,
           *  considering two samples are from different channels */
          for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
          {
            /*points to store, first sample of corresponding channel*/
            dst_ptr_16 = (int16_t *)((int32_t *)pBuffer+ch_idx*numSamples);

            for(samp = 0; samp < numSamples; samp++)
            {
              (*dst_ptr_16) = (int16_t)( *(src_ptr_32+samp*num_ch_per_port+ ch_idx ) );
              (*(dst_ptr_16 + numSamples)) = (int16_t) ( *(src_ptr_32 + samp*num_ch_per_port + ch_idx) >>16 );

              dst_ptr_16++;
            }
          }

          /*Move the working buffer pointer, equivalent to size of descriptor payload*/
          pBuffer = (int16_t *)((int32_t *)pBuffer + num_ch_per_port*numSamples);
        }
        else
        {
          /* normal 32-bit data */

          int32_t *src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;
          int32_t *dst_ptr_32 = (int32_t *)pBuffer;

          /* De-interleave the descriptor payload and store in working buffer */
          for (ch_idx = 0; ch_idx < num_ch_per_port; ch_idx++)
          {
            for (samp = 0; samp < numSamples; samp++)
            {
              *(dst_ptr_32 + ch_idx*numSamples + samp) = *(src_ptr_32 + samp*num_ch_per_port + ch_idx);
            }
          }

          /*Move the working buffer pointer, equivalent to size of descriptor payload*/
          pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples*num_ch_per_port );
        }
      }

      // Transfer back the free descriptor to descriptor queue
      result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) IOVec_custom_data_ptr);
      if (DAL_SUCCESS != result)
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit the descriptor to BAM");
      }
    }
    else
    {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dropping data. Descriptor not available");
    }
  }
  return result;
}

/**
  @brief Read the Mic path data from descriptor buffers to
         client's buffer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] numSamples  Number of samples to be read

  @param[out] pBuffer client buffer for storing the read samples

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_read(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
	ADSPResult result = ADSP_EOK;
	SlimBusBamIOVecType IOVec;
	afe_sb_cust_desc_info_t IOVec_custom_data, *IOVec_custom_data_ptr;
	afe_slimbus_state_struct_t *p_dev_state= (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
	afe_sb_master_port_info_t *p_stream_info=(afe_sb_master_port_info_t *) p_dev_state->a_master_port_info;
	DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;
	uint16_t                    master_port_num, samp;
	int64_t                     *src_ptr_64, *dst_ptr_64;
	int32_t                     *src_ptr_32, *dst_ptr_32;
	uint32_t                    total_num_samp;

	IOVec_custom_data_ptr = &IOVec_custom_data;

	// Go through for all the channels to get the transfered descriptors
	for(master_port_num = 0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++, p_stream_info++)
	{
		// Read the descriptor from descriptor queue
		result = DalSlimBus_GetBamIOVecEx(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) &IOVec_custom_data_ptr);
		if (DAL_SUCCESS == result &&  0 != IOVec_custom_data_ptr->virt_addr)
		{
			// Invalidate the cache lines on successful arrival
			afe_dma_buff_cache_invalidate((uint32_t)IOVec_custom_data_ptr->virt_addr, (uint32_t)IOVec.uSize);

			// Copy the descriptor payload into local buffer
			//memcpy((int8_t *)pBuffer, (int8_t *)IOVec.uAddr, IOVec.uSize);

			if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_sb_cfg.bit_width)
			{

#ifdef __qdsp6__

				/* Calculate number of 32-bit samples */
				total_num_samp = (IOVec.uSize >> 2);

				/* For Odd number of samples, copy one sample at a time */
				if(total_num_samp & 0x1)
				{
					src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;
					dst_ptr_32 = (int32_t *)pBuffer;

					for(samp = total_num_samp; samp > 0; samp--)
					{
						(*dst_ptr_32++) = (*src_ptr_32++) >> QFORMAT_SHIFT_FACTOR;
					}
				}
				else
				{
					src_ptr_64 = (int64_t *)IOVec_custom_data_ptr->virt_addr; /* IOVec.uAddr points to 32 bytes aligned buffer  */
					dst_ptr_64 = (int64_t *)pBuffer; /* pBuffer points to 8 bytes aligned buffer       */

					/* Q31 to Q28 conversion
					 */

					for(samp = total_num_samp; samp >= 2; samp -= 2)
					{
						(*dst_ptr_64++) = Q6_P_vasrw_PI(*src_ptr_64++, QFORMAT_SHIFT_FACTOR);
					}
				}

#else           /*------------ Non Q6 Version --------------------*/

				src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;
				dst_ptr_32 = (int32_t *)pBuffer;

				for(samp = 0; samp < (IOVec.uSize >> 2); samp++)
				{
					(*dst_ptr_32++) = (*src_ptr_32++) >> QFORMAT_SHIFT_FACTOR;
				}
#endif
				pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples);
			}
			else
			{
				uint32_t num_bytes_per_ch = afe_port_ptr->int_samples_per_period * afe_port_ptr->bytes_per_channel;
				memscpy((int8_t *)pBuffer,
						num_bytes_per_ch * afe_port_ptr->channels,
						(int8_t *)IOVec_custom_data_ptr->virt_addr,
						IOVec.uSize);

				pBuffer = (int16_t*)((int8_t*)pBuffer + num_bytes_per_ch);
			}

			// Transfer back the free descriptor to descriptor queue
			result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) IOVec_custom_data_ptr);
			if (DAL_SUCCESS != result)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit the descriptor to BAM");
			}
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dropping data. Descriptor not available");
		}
	}
	return result;
}


/**
  @brief Read the 32bit Mic path data from descriptor buffers, extract 16bit samples and store to
         client's buffer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] num_samples_per_chan  Number of samples per channel
        to be read

  @param[out] buffer_ptr client buffer for storing the read samples

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_read_32bit(afe_dev_port_t *afe_port_ptr,  int16_t  *buffer_ptr, uint32_t num_samples_per_chan)
{
	ADSPResult result = ADSP_EOK;
	SlimBusBamIOVecType IOVec;
	afe_sb_cust_desc_info_t IOVec_custom_data, *IOVec_custom_data_ptr;
	afe_slimbus_state_struct_t *p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
	afe_sb_master_port_info_t *p_stream_info = (afe_sb_master_port_info_t *)p_dev_state->a_master_port_info;
	DalDeviceHandle *p_core_driver = p_dev_state->p_sb_driver->p_core_driver;
	uint16_t master_port_num, samp;
	int32_t *src_ptr_32;
	int16_t *dst_ptr_16;

	IOVec_custom_data_ptr = &IOVec_custom_data;
	// Go through for all the channels to get the transfered descriptors
	for(master_port_num = 0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++, p_stream_info++)
	{
		// Read the descriptor from descriptor queue
		result = DalSlimBus_GetBamIOVecEx(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) &IOVec_custom_data_ptr);
		if (DAL_SUCCESS == result &&  0 != IOVec_custom_data_ptr->virt_addr)
		{
			// Invalidate the cache lines on successful arrival
			afe_dma_buff_cache_invalidate((uint32_t)IOVec_custom_data_ptr->virt_addr, (uint32_t)IOVec.uSize);

			// Copy the descriptor payload into local buffer
			src_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;
			dst_ptr_16 = (int16_t *)buffer_ptr;

			/* Unpack the 32bit data to two 16bit and de-interleave them considering two samples are from different channels */
			for(samp = 0; samp < (IOVec.uSize >> 2); samp++)
			{
				(*dst_ptr_16) = (int16_t)(*src_ptr_32);
				(*(dst_ptr_16 + num_samples_per_chan)) = (int16_t)(*src_ptr_32 >> 16);

				dst_ptr_16++;
				src_ptr_32++;
			}

			buffer_ptr = (int16_t *)((int32_t *)buffer_ptr + num_samples_per_chan);

			// Transfer back the free descriptor to descriptor queue
			result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port,
					SLIMBUS_BAM_DEFAULT, &IOVec, (void **)IOVec_custom_data_ptr);
			if(DAL_SUCCESS != result)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit the descriptor to BAM");
			}
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dropping data. Descriptor not available");
		}
	}
	return result;
}

/**
  @brief Write the data to speaker path descriptors and do BAM
         transfer

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[in] pBuffer port buffer to write the samples

  @param[in] numSamples  Number of samples to be written.

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_slimbus_write(afe_dev_port_t *afe_port_ptr,  int16_t  *pBuffer, uint32_t numSamples)
{
	ADSPResult result = ADSP_EOK;
	SlimBusBamIOVecType IOVec;
	afe_sb_cust_desc_info_t IOVec_custom_data, *IOVec_custom_data_ptr;
	afe_slimbus_state_struct_t *p_dev_state= (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
	afe_sb_master_port_info_t *p_stream_info=(afe_sb_master_port_info_t *) p_dev_state->a_master_port_info;
	DalDeviceHandle *p_core_driver=p_dev_state->p_sb_driver->p_core_driver;
	uint16_t                    master_port_num, samp;
	int32_t                     *src_ptr_32, *dst_ptr_32;

	IOVec_custom_data_ptr = &IOVec_custom_data;

	// Iterate the procedure for each channel\stream
	for(master_port_num = 0; master_port_num < p_dev_state->afe_sb_cfg.num_master_ports; master_port_num++, p_stream_info++)
	{
		// Get the free descriptor from the descriptor queue
		result = DalSlimBus_GetBamIOVecEx(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) &IOVec_custom_data_ptr);
		if (DAL_SUCCESS == result &&  0 != IOVec_custom_data_ptr->virt_addr)
		{
			// Copy the render samples in to the memory pointed by the descriptor
			//memcpy((char *)IOVec.uAddr,(char *)pBuffer, IOVec.uSize);

			if(AFE_BITS_PER_SAMPLE_24 == p_dev_state->afe_sb_cfg.bit_width)
			{

				src_ptr_32 = (int32_t *)pBuffer;
				dst_ptr_32 = (int32_t *)IOVec_custom_data_ptr->virt_addr;

				for(samp = 0; samp < (IOVec.uSize >> 2); samp++)
				{
					*dst_ptr_32++ = s32_shl_s32_sat(*src_ptr_32++, QFORMAT_SHIFT_FACTOR);
				}

				pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples);
			}
			else /* 16/32 bit data */
			{
				afe_dma_buff_copy((int8_t *)IOVec_custom_data_ptr->virt_addr, IOVec.uSize ,(int8_t *)pBuffer, IOVec.uSize);
				if(AFE_BITS_PER_SAMPLE_16 == p_dev_state->afe_sb_cfg.bit_width)
				{
					pBuffer = pBuffer + numSamples;
				}
				else
				{
					pBuffer = (int16_t *)((int32_t *)pBuffer + numSamples);
				}
			}

			// Cache flush to ensure that physical memory gets updated
			afe_dma_buff_cache_flush((uint32_t)IOVec_custom_data_ptr->virt_addr, (uint32_t)IOVec.uSize);

			// Submit the descriptor to BAM pipe for transfering to FIFO in slimbus hw
			result = DalSlimBus_SubmitBamTransfer(p_core_driver, p_stream_info->h_master_port, SLIMBUS_BAM_DEFAULT, &IOVec, (void **) IOVec_custom_data_ptr);
			if (DAL_SUCCESS != result)
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to submit the descriptor to BAM");
			}
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dropping data. Descriptor not available");
		}
	}

	return result;
}

/**
  @brief Calculates the PCM sample's drift (in us) between
  	consequitive VFR interrupts. It provides the VFR shift
  	information also.

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[out] samp_drift - sample drift at 8khz

  @param[out] resync_status - VFR shift detection (TRUE\FALSE)

  @return  ADSP_EOK on success, an error code on error

 */
void afe_sb_read_vfr_drift(afe_dev_port_t* afe_port_ptr, vfr_src_state_t *vfr_state_ptr)
{
	uint64_t                     cur_vfr_samp_cnt = 0;
	uint32_t                     num_vfr_sample = 0;
	afe_slimbus_state_struct_t   *p_dev_state = NULL;
	afe_drift_info_t             *p_drift_info;
	uint16_t                     vfr_src_idx = vfr_state_ptr->vfr_src_index;
	uint32_t                     sample_rate, frame_counter;
	int32_t                      acc_drift_in_us = 0;

	p_drift_info = &afe_port_ptr->drift_info[vfr_src_idx];

	if (NULL != (p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv))
	{
		/* If VFR source has been disabled and re-enabled, while the port is still running
           resync the port's drift info structure. */

		if (TRUE == vfr_state_ptr->is_first_vfr)
		{
			afe_drv_resync_port_vfr_drift_info(p_drift_info);
		}

		if (TRUE == vfr_state_ptr->is_hw_latched)
		{
			afe_sb_read_progress_counter(p_dev_state, &cur_vfr_samp_cnt, SLIMBUS_COUNTER_VFR, vfr_src_idx);
		}
		else /* s/w latched, device counter already latched in VFR interrupt handler */
		{
			cur_vfr_samp_cnt = p_drift_info->dev_latched_counter_curr; /* This counter is not reset during re-sync */
		}

		sample_rate = afe_port_ptr->sample_rate;


		if (TRUE == p_drift_info->is_first_vfr)
		{
			p_drift_info->is_first_vfr = FALSE;
		}
		else /* For every subsequent VFR interrupt */
		{
			/* Calculate the number of samples transfered in 1 VFR interrupt duration */
			num_vfr_sample = (cur_vfr_samp_cnt - p_drift_info->dev_latched_counter_prev);

			/* Accumulate drift in samples */
			if (TRUE == vfr_state_ptr->is_hw_latched)
			{
				(p_drift_info->vfr_acc_sample_drift_to_update) += (p_drift_info->samples_in_vfr_frame - num_vfr_sample);
			}
			else /* SW latched device counter */
			{
				afe_device_update_acc_sample_drift(p_drift_info, num_vfr_sample);
			}

			/* Convert the accumulated drift in samples to time */
			acc_drift_in_us = divide_int32((int32_t)(p_drift_info->vfr_acc_sample_drift_to_update * NUM_MILLISEC_PER_SEC), sample_rate/NUM_MILLISEC_PER_SEC);

#ifdef VFR_DEBUG_MSG       		
			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "port-id: 0x%x, vfr_src_idx: %u, acc_drift_in_us: %ld, acc_drift_in_samples: %ld, num_vfr_sample: %ld",
					afe_port_ptr->intf, vfr_src_idx, acc_drift_in_us, p_drift_info->vfr_acc_sample_drift_to_update, num_vfr_sample);
#endif
		}

		/* Update the previous sample count */
		p_drift_info->dev_latched_counter_prev = cur_vfr_samp_cnt;
	}

	/* Update VFR frame counter and acc drift info */

	qurt_elite_mutex_lock( &(afe_port_ptr->port_mutex) );

	if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
	{
		/* Increment the VFR frame counter */
		frame_counter = (uint32_t) (p_drift_info->vfr_drift_info) + 1;

		/* Update the VFR drift info */
		p_drift_info->vfr_drift_info = (uint64_t)(((int64_t)acc_drift_in_us << 32) | ((uint64_t)frame_counter));
	}

	qurt_elite_mutex_unlock( &(afe_port_ptr->port_mutex) );

	return;
}

/**
  @brief Calculates the PCM sample's drift (in us) between
  	consequitive VFR interrupts. It provides the VFR shift
  	information also.

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[out] time_in_us - time stamp in micro sec

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_sb_get_time(afe_dev_port_t* afe_port_ptr, uint64_t *time_in_us)
{
	uint32_t time_per_sample;
	uint16_t q_format;
	uint64_t temp1, temp2, cur_pg_samp_cnt = 0;
	afe_slimbus_state_struct_t *p_dev_state = NULL;

	if (NULL == afe_port_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE dev port pointer is not valid");
		return ADSP_EBADPARAM;
	}

	p_dev_state = (afe_slimbus_state_struct_t *) afe_port_ptr->afe_drv;

	afe_sb_read_progress_counter(p_dev_state, &cur_pg_samp_cnt, SLIMBUS_COUNTER_FREE_RUN, 0 /*VFR src index, dont' care here*/);

	afe_get_time_per_sample(p_dev_state->sample_rate_index, &time_per_sample, &q_format);

	temp2         = ((uint64_t)(0x01)<<(q_format-1));

	//assumption: 64 bit cur_pg_samp_cnt generally doesn't go beyond 32 bits.
	temp1         = (uint64_t) ((uint64_t) (cur_pg_samp_cnt) * (uint64_t)time_per_sample);

	/*//doing below, we loose precision.
    upper = (uint64_t)((cur_pg_samp_cnt>>32)&0xFFFFFFFF);
    upper = upper * (uint64_t)time_per_sample;

    lower = (uint64_t)((cur_pg_samp_cnt)&0xFFFFFFFF);
    lower = lower * (uint64_t)time_per_sample;

    temp1 = (upper) + (lower>>32);
	 */
	*time_in_us   = (temp1 + temp2) >> q_format;

	return ADSP_EOK;
}

/**
  @brief Returns the current sample count for device

  @param[in] afe_port_ptr pointer AFE dev port structure

  @param[out] p_cur_pg_samp_cnt - Current device sample count

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_sb_get_dev_prog_counter(afe_dev_port_t* afe_port_ptr, uint64_t *p_cur_pg_samp_cnt)
{
	afe_slimbus_state_struct_t  *p_dev_state = NULL;

	if (NULL == afe_port_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE dev port pointer is not valid");
		return ADSP_EBADPARAM;
	}

	p_dev_state = (afe_slimbus_state_struct_t *) afe_port_ptr->afe_drv;

	afe_sb_read_progress_counter(p_dev_state, p_cur_pg_samp_cnt, SLIMBUS_COUNTER_FREE_RUN, 0);

	return ADSP_EOK;
}

/**
  @brief Get the logical address for a SB device, for a given device enumeration address

  @param[in] afe_port_ptr - Pointer to the AFE device port structure

  @param[in] device_enum_addr - 48-bit enumerated device address

  @param[out] device_la_ptr - pointer to the 8-bit device logical address

  @return  ADSP_EOK on success, an error code on error

  @dependencies
  None.

 */
ADSPResult afe_sb_get_logical_addr(void* ptr, uint64_t device_enum_addr, uint8_t *device_la_ptr)
{
	ADSPResult result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)ptr;

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	// Enumerated address is 48-bit (6 byte). Need to pass exact size to the Slimbus DAL layer
	if (DAL_SUCCESS != (result = DalSlimBus_GetDeviceLogicalAddr(p_sb_driver->p_core_driver, (uint8_t*)&device_enum_addr, SLIMBUS_EMUM_ADDR_SIZE_IN_BYTES, device_la_ptr)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to convert Slimbus enumeration to logical address 0x%x", result);
		result = ADSP_EFAILED;
	}

	afe_slimbus_core_driver_close((void *)p_sb_driver);

	return result;
}

/**
 * Update's a current page. Modifies registers address with processor id.
 *
 * Sample Page based register access Logic
 *
 * Consider:
 *    register_address = 0xABCD (0x[page_no][reg_in_page])
 *    proc_id = 0x1 (i.e. the page register is 0x100)
 *    Current page = 0x00 (i.e. addr 0x100 has value 0x00)
 *
 * Operations:
 * Step 1 - Update page to 0xAB :
 *    write value 0xAB to address 0x100
 * Step 2 - Access register CD in page :
 *    write/read to/from address 0x1CD
 *
 * Note: Caller needs to ensures mutex protection around this page set and next
 *       actual register read/write operation.
 * Note: Page update is non-blocking but synchronous. Caller needs to ensure
 *       next operation isn't asynchronous for proper page based transaction.
 *
 *
 * @param[in] p_core_driver - pointer to DAL Slimbus device handle
 *
 * @param[in] p_reg_prop - pointer to afe slimbus register properties
 *
 * @param[in] slave_dev_la - logical address of the slave device
 *
 * @param[in/out] p_reg_addr - pointer to the original register address(not
 *                             including processor id). Page information is
 *                             extracted from register address and replaced with
 *                             processor id.
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
static ADSPResult afe_sb_reg_update_page(DalDeviceHandle *p_core_driver,
		afe_reg_prop_t *p_reg_prop,
		uint32_t *p_reg_addr)
{
	DALResult dal_result = DAL_SUCCESS;
	uint8_t new_page = 0;
	uint16_t page_addr = 0;

	if(!p_reg_prop->uses_paging)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Wrong usage, paging scheme not set, 0x%x",
				p_reg_prop->uses_paging);
		return ADSP_EUNEXPECTED;
	}

	new_page = REG_GET_PAGE_NUM(*p_reg_addr);
	page_addr = REG_GET_PAGE_REG_ADDR(p_reg_prop->proc_id) +
			p_reg_prop->slimbus_la_offset;

	if(p_reg_prop->current_page != new_page)
	{
		dal_result = DalSlimBus_WriteValueElement(p_core_driver,
				(SlimBusLogicalAddrType)p_reg_prop->slave_dev_la,
				page_addr,
				sizeof(new_page),
				(uint8_t*)&new_page,
				sizeof(new_page),
				SLIMBUS_EVENT_HANDLE_NON_BLOCKING);

		/* Move to new Page*/
		if (DAL_SUCCESS != (dal_result))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Unable to write to Slimbus register 0x%x",
					dal_result);
			return ADSP_EFAILED;
		}
		p_reg_prop->current_page = new_page;
	}

	REG_SET_TRANSLATED_ADDR(*p_reg_addr, p_reg_prop->proc_id);
	return ADSP_EOK;
}

/**
 * Register page init ensure that the software page variable and the on-target
 * page variable are in same state.
 *
 * @param[in] p_drv pointer to global afe_slimbus_driver_t state
 *
 * @param[in] p_reg_prop Slimbus register property
 *
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_sb_reg_page_init(void* p_drv, afe_reg_prop_t* p_reg_prop)
{
	ADSPResult result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;
	uint32_t data_length = 0;
	uint8_t reg_page_value = 0;
	uint16_t reg_page_addr = 0;

	if(ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	reg_page_addr = REG_GET_PAGE_REG_ADDR(p_reg_prop->proc_id) +
			p_reg_prop->slimbus_la_offset;

	result = DalSlimBus_ReadValueElement(p_sb_driver->p_core_driver,
			(SlimBusLogicalAddrType)p_reg_prop->slave_dev_la,
			reg_page_addr,
			&reg_page_value,
			sizeof(reg_page_value),
			&data_length,
			NULL /* BLOCKING */);
	if((DAL_SUCCESS != result) || (sizeof(reg_page_value) != data_length))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Unable to read reg page value result = 0x%x, data_length = %d ",
				result, data_length);
		result = ADSP_EFAILED;
	}
	else
	{
		p_reg_prop->current_page = reg_page_value;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Flushed reg page value 0x%x successfully ", reg_page_value);
	}

	afe_slimbus_core_driver_close((void *)p_sb_driver);

	return result;
}

/**
 * Generic API to write to Control and Status Registers(CSR).
 * Parameters passed are device address, register address and register value
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] slave_dev_la logical address of the slave device
 *
 * @param[in] reg_addr is the logical address of the Slimbus Codec CSR
 *
 * @param[in] write_value_ptr is a pointer to value to be written to the Codec CSR
 *
 * @param[in] reg_field_bit_mask is the valid  bit-field of the parameter being passed
 *
 * @param[out] reg_bit_width is the width of the register to be written to, in bits. Possible values are
 *             8, 16 and 32
 *
 * @param[out] is_blocking indicates if write should be blocking. Possible values are TRUE and FALSE
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_sb_update_register(void* p_drv, afe_reg_prop_t*  p_reg_prop, \
		uint32_t	reg_addr, uint8_t* write_value_ptr, \
		uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
		bool_t is_blocking)
{
	ADSPResult         result = ADSP_EOK;
	uint32_t           data_length, register_value = 0, write_value = 0, reg_field_shift;
	uint32_t           register_size_in_bytes = reg_bit_width >> BITS_TO_BYTE_SHIFT_VALUE;
	DALSYSEventHandle  ve_write_mode = SLIMBUS_EVENT_HANDLE_NON_BLOCKING;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

	// NULL is for blocking
	if (TRUE == is_blocking)
	{
		ve_write_mode = NULL;
	}

	//Calculate the amount by which the value should be shifted. The below
	//Q6 macro utilizes the "count trailing 0's" to calculate the shift value
	reg_field_shift = Q6_R_ct0_R(reg_field_bit_mask);

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	/* Reg access sequencing lock - for Read-modify-Write & paging sequence */
	qurt_elite_mutex_lock(&(p_sb_driver->reg_seq_mutex));

	if(p_reg_prop->uses_paging)
	{
		/* Update Reg Addr */
		result = afe_sb_reg_update_page(p_sb_driver->p_core_driver,
				p_reg_prop,
				&reg_addr);
		if(ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed to update page reg, 0x%x",
					result);
			goto __bail_out;
		}
	}

	/* Apply Slimbus logical offset */
	reg_addr += p_reg_prop->slimbus_la_offset;

	// Read existing value in register
	result = DalSlimBus_ReadValueElement(p_sb_driver->p_core_driver, (SlimBusLogicalAddrType)p_reg_prop->slave_dev_la, reg_addr,  (uint8_t*)&register_value, \
			register_size_in_bytes,  &data_length, NULL);
	if(DAL_SUCCESS != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed to Read value reg, 0x%x",
				result);
		result = ADSP_EFAILED;
		goto __bail_out;
	}

	// Clear out the bit indicated in bit_mask
	register_value &= ~reg_field_bit_mask;

	// Read 8/16/32 bits by typecasting appropriately. Dont always read 32-bit
	// as it could result in un-aligned access or out-of-range/unclocked access.
	if(8 == reg_bit_width)
		write_value = *(uint8_t*)write_value_ptr;
	else if(16 == reg_bit_width)
		write_value = *(uint16_t*)write_value_ptr;
	else if(32 == reg_bit_width)
		write_value = *(uint32_t*)write_value_ptr;

	//align the value to be written to the correct position
	write_value = (write_value << reg_field_shift) & reg_field_bit_mask;

	// OR the value to be written with the original register contents
	write_value |= register_value;

	if (DAL_SUCCESS != (result = DalSlimBus_WriteValueElement(p_sb_driver->p_core_driver, (SlimBusLogicalAddrType)p_reg_prop->slave_dev_la, reg_addr, register_size_in_bytes, \
			(uint8_t*)&write_value, sizeof(write_value), ve_write_mode)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to write to Slimbus register 0x%x", result);
		result = ADSP_EFAILED;
	}

	__bail_out:

	/* Reg access sequencing unlock */
	qurt_elite_mutex_unlock(&(p_sb_driver->reg_seq_mutex));

	afe_slimbus_core_driver_close((void *) p_sb_driver);

	return result;
}

ADSPResult afe_sb_write_register(void *p_drv, afe_reg_prop_t*  p_reg_prop, \
		uint32_t	reg_addr, uint8_t* write_value_ptr, \
		uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
		bool_t is_blocking)
{
	ADSPResult         result = ADSP_EOK;
	uint32_t           write_value = 0, reg_field_shift;
	DALSYSEventHandle  ve_write_mode = SLIMBUS_EVENT_HANDLE_NON_BLOCKING;
	uint32_t           register_size_in_bytes = reg_bit_width >> BITS_TO_BYTE_SHIFT_VALUE;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

	// NULL is for blocking
	if (TRUE == is_blocking)
	{
		ve_write_mode = NULL;
	}

	//Calculate the amount by which the value should be shifted. The below
	//Q6 macro utilizes the "count trailing 0's" to calculate the shift value
	reg_field_shift = Q6_R_ct0_R(reg_field_bit_mask);

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	// Read 8/16/32 bits by typecasting appropriately. Dont always read 32-bit
	// as it could result in un-aligned access or out-of-range/unclocked access.
	if(8 == reg_bit_width)
		write_value = *(uint8_t*)write_value_ptr;
	else if(16 == reg_bit_width)
		write_value = *(uint16_t*)write_value_ptr;
	else if(32 == reg_bit_width)
		write_value = *(uint32_t*)write_value_ptr;

	//align the value to be written to the correct position
	write_value = (write_value << reg_field_shift) & reg_field_bit_mask;

	if(p_reg_prop->uses_paging)
	{
		/* Reg access sequencing lock - for Paging sequence */
		qurt_elite_mutex_lock(&(p_sb_driver->reg_seq_mutex));

		/* Update Reg Addr */
		result = afe_sb_reg_update_page(p_sb_driver->p_core_driver,
				p_reg_prop,
				&reg_addr);
		if(ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed to update page reg, 0x%x",
					result);
			goto __bail_out;
		}
	}

	/* Apply Slimbus logical offset */
	reg_addr += p_reg_prop->slimbus_la_offset;

	if (DAL_SUCCESS != (result = DalSlimBus_WriteValueElement(p_sb_driver->p_core_driver, (SlimBusLogicalAddrType)p_reg_prop->slave_dev_la, reg_addr, register_size_in_bytes, \
			(uint8_t*)&write_value, sizeof(write_value), ve_write_mode)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unable to write to Slimbus register 0x%x", result);
		result = ADSP_EFAILED;
	}

	__bail_out:

	if(p_reg_prop->uses_paging)
	{
		/* Reg access sequencing unlock */
		qurt_elite_mutex_unlock(&(p_sb_driver->reg_seq_mutex));
	}

	afe_slimbus_core_driver_close((void *)p_sb_driver);

	return result;
}

ADSPResult afe_sb_read_register(void *p_drv, afe_reg_prop_t*  p_reg_prop, \
		uint32_t	reg_addr, uint8_t* read_value_ptr, \
		uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
		bool_t is_blocking)
{
	ADSPResult         result = ADSP_EOK;
	uint32_t           register_value = 0, reg_field_shift;
	uint32_t           register_size_in_bytes = reg_bit_width >> BITS_TO_BYTE_SHIFT_VALUE;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;
	DALSYSEventHandle  dal_sb_handle = SLIMBUS_EVENT_HANDLE_NON_BLOCKING;

	// NULL is for blocking
	if (TRUE == is_blocking)
	{
		dal_sb_handle = NULL;
	}

	//Calculate the amount by which the value should be shifted. The below
	//Q6 macro utilizes the "count trailing 0's" to calculate the shift value
	reg_field_shift = Q6_R_ct0_R(reg_field_bit_mask);

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *) p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	if(p_reg_prop->uses_paging)
	{
		/* Reg access sequencing lock */
		qurt_elite_mutex_lock(&(p_sb_driver->reg_seq_mutex));

		/* Update Reg Addr */
		result = afe_sb_reg_update_page(p_sb_driver->p_core_driver,
				p_reg_prop,
				&reg_addr);
		if(ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed to update page reg, 0x%x",
					result);
			goto __bail_out;
		}
	}

	/* Apply Slimbus logical offset */
	reg_addr += p_reg_prop->slimbus_la_offset;

	// Read existing value in register
	result = DalSlimBus_ReadValueElement(p_sb_driver->p_core_driver, (SlimBusLogicalAddrType)p_reg_prop->slave_dev_la, reg_addr,  (uint8_t*)&register_value, \
			register_size_in_bytes,  NULL, dal_sb_handle);
	if(DAL_SUCCESS != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed to Read value reg, 0x%x",
				result);
		result = ADSP_EFAILED;
		goto __bail_out;
	}

	// Clear out the bit indicated in bit_mask
	register_value &= reg_field_bit_mask;
	register_value = register_value >> reg_field_shift;

	// Read 8/16/32 bits by typecasting appropriately. Dont always read 32-bit
	// as it could result in un-aligned access or out-of-range/unclocked access.
	if(8 == reg_bit_width)
		*(uint8_t*)read_value_ptr = (uint8_t) register_value;
	else if(16 == reg_bit_width)
		*(uint16_t*)read_value_ptr = (uint16_t) register_value;
	else if(32 == reg_bit_width)
		*(uint32_t*)read_value_ptr = (uint32_t) register_value;

	__bail_out:

	if(p_reg_prop->uses_paging)
	{
		/* Reg access sequencing unlock */
		qurt_elite_mutex_unlock(&(p_sb_driver->reg_seq_mutex));
	}

	afe_slimbus_core_driver_close((void *) p_sb_driver);

	return result;
}

/**
 * Generic API to read multiple contiguous registers
 *
 * @param[in] afe_port_ptr pointer to AFE device port structure
 *
 * @param[in] slave_dev_la logical address of the slave device
 *
 * @param[in] reg_addr is the logical address of the Slimbus Codec CSR
 *
 * @param[in] write_value_ptr is a pointer to value to be written to the Codec CSR
 *
 * @param[in] reg_field_bit_mask is the valid  bit-field of the parameter being passed
 *
 * @param[in] reg_bit_width is the width of the register to be 
 *  		   written to, in bits. Possible values are 8, 16
 *  		   and 32
 *@param[in] no_of_regs_to_read is the number of contiguous  
 *  	regiters to read
 *  
 * @param[in] is_blocking indicates if write should be blocking.
 *  	 Possible values are TRUE and FALSE
 *
 * @return
 * ADSP_EOK in case of success, otherwise corresponding error
 * code
 *
 * @dependencies
 * None.
 *
 */
ADSPResult afe_sb_read_multi_register(void *p_drv, afe_reg_prop_t*  p_reg_prop, \
		uint32_t	reg_addr, uint8_t* read_value_ptr, \
		uint32_t reg_field_bit_mask,  uint32_t reg_bit_width, \
		uint32_t no_of_regs_to_read, bool_t is_blocking)
{
	ADSPResult result = ADSP_EOK;
	uint32_t *reg_read_32 = NULL;
	uint16_t *reg_read_16 = NULL;
	uint8_t *reg_read_8 = NULL;
	uint32_t reg_field_shift;
	uint32_t register_size_in_bytes;
	uint32_t no_of_bytes_to_read = 0;
	uint32_t reg_index;
	afe_slimbus_driver_t         *p_sb_driver = (afe_slimbus_driver_t *)p_drv;
	DALSYSEventHandle  dal_sb_handle = SLIMBUS_EVENT_HANDLE_NON_BLOCKING;

	// NULL is for blocking
	if (TRUE == is_blocking)
	{
		dal_sb_handle = NULL;
	}

	if ((8 != reg_bit_width) && (16 != reg_bit_width) &&(32 != reg_bit_width))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported bitwidth to read from slimbus register");
		return ADSP_EUNSUPPORTED;
	}

	register_size_in_bytes = reg_bit_width >> BITS_TO_BYTE_SHIFT_VALUE;
	no_of_bytes_to_read = no_of_regs_to_read * register_size_in_bytes;

	if (no_of_bytes_to_read > SLIMBUS_READ_MAX_BYTES)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unsupported num of bytes to read from register");
		return ADSP_EUNSUPPORTED;
	}

	//Calculate the amount by which the value should be shifted. The below
	//Q6 macro utilizes the "count trailing 0's" to calculate the shift value
	reg_field_shift = Q6_R_ct0_R(reg_field_bit_mask);

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	if(p_reg_prop->uses_paging)
	{
		uint32_t read_range = no_of_regs_to_read*register_size_in_bytes;
		/* Page overflow check */
		if(REG_MASK(reg_addr,REG_BYTE__M) + read_range > REG_BYTE__M)
		{
			/* Initializing local variable for iteration */
			uint32_t total_range = no_of_regs_to_read;
			uint32_t reg_addr_offset = reg_addr;
			uint32_t split_range = (REG_BYTE__M - REG_MASK(reg_addr,REG_BYTE__M))
                                        		  /register_size_in_bytes;

			/* Sample iteration:
			 * Consider reg_addr = 200, total_range = 510 (i.e. total 3 page access),
			 * then split_range values are 55, 255, 200 */
			while(split_range)
			{
				/* Iterate through individual page with one level recursive call */
				result = afe_sb_read_multi_register(p_drv, p_reg_prop,
						reg_addr + reg_addr_offset,
						read_value_ptr + reg_addr_offset,
						reg_field_bit_mask, reg_bit_width,
						split_range,
						is_blocking);
				if(ADSP_EOK != result)
				{
					MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
							"Failed to read split_range: 0x%x, reg_addr_offset: 0x%x, "
							"reg_addr: 0x%x of reg page overflow scenario, 0x%x",
							split_range, reg_addr_offset, reg_addr, result);
					return result;
				}

				/* Next Page */
				reg_addr_offset += split_range*register_size_in_bytes;
				/* Decrease number of register read */
				total_range -= split_range;
				split_range = total_range >  REG_BYTE__M ? REG_BYTE__M : total_range;
			}

			/* All done, no need to proceed */
			return result;
		}

		/* Reg access sequencing lock */
		qurt_elite_mutex_lock(&(p_sb_driver->reg_seq_mutex));

		/* Update Reg Addr */
		result = afe_sb_reg_update_page(p_sb_driver->p_core_driver,
				p_reg_prop,
				&reg_addr);
		if(ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"Failed to update page reg, 0x%x",
					result);
			goto __bail_out;
		}
	}

	/* Apply Slimbus logical offset */
	reg_addr += p_reg_prop->slimbus_la_offset;

	// Read existing value in register
	result = DalSlimBus_ReadValueElement(p_sb_driver->p_core_driver, (SlimBusLogicalAddrType)p_reg_prop->slave_dev_la, reg_addr,  (uint8_t*)read_value_ptr, \
			no_of_bytes_to_read,  NULL, dal_sb_handle);
	if(DAL_SUCCESS != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"Failed to Read value reg, 0x%x",
				result);
		result = ADSP_EFAILED;
		goto __bail_out;
	}

	/* Check if all fields are required */
	/* Compare trailing 1's with bit width */
	if(((uint32_t)Q6_R_ct1_R(reg_field_bit_mask)) == reg_bit_width)
	{
		/* All read values are good, no more masking operation is required */
		goto __bail_out;
	}

	/* Masking operation */
	if(8 == reg_bit_width)
	{
		reg_read_8 = (uint8_t*)read_value_ptr;
		for (reg_index = 0; reg_index < no_of_regs_to_read; reg_index++)
		{
			// Clear out the bit indicated in bit_mask
			//MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "reg_read_8[%d] %d", reg_index, reg_read_8[reg_index]);
			reg_read_8[reg_index] &= reg_field_bit_mask;
			reg_read_8[reg_index] = reg_read_8[reg_index] >> reg_field_shift;
			//MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "reg_read_8[%d] %d address 0x%lx", reg_index, reg_read_8[reg_index], &reg_read_8[reg_index]);
		}
	}
	else if(16 == reg_bit_width)
	{
		reg_read_16 = (uint16_t*)read_value_ptr;
		for (reg_index = 0; reg_index < no_of_regs_to_read; reg_index++)
		{
			// Clear out the bit indicated in bit_mask
			reg_read_16[reg_index] &= reg_field_bit_mask;
			reg_read_16[reg_index] = reg_read_16[reg_index] >> reg_field_shift;
		}

	}
	else if(32 == reg_bit_width)
	{
		reg_read_32 = (uint32_t*)read_value_ptr;
		for (reg_index = 0; reg_index < no_of_regs_to_read; reg_index++)
		{
			// Clear out the bit indicated in bit_mask
			reg_read_32[reg_index] &= reg_field_bit_mask;
			reg_read_32[reg_index] = reg_read_32[reg_index] >> reg_field_shift;
		}
	}

	__bail_out:

	if(p_reg_prop->uses_paging)
	{
		/* Reg access sequencing unlock */
		qurt_elite_mutex_unlock(&(p_sb_driver->reg_seq_mutex));
	}
	afe_slimbus_core_driver_close((void *)p_sb_driver);

	return result;
}

ADSPResult afe_slimbus_core_driver_open(void *p_drv)
{
	ADSPResult result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

	if (!p_sb_driver)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL SLIMbus driver pointer, p_sb_driver: 0x%X", p_sb_driver);
		return ADSP_EFAILED;
	}

	if (NULL == (p_sb_driver->p_core_driver))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL SLIMbus core driver pointer: 0x%X", p_sb_driver->p_core_driver);
		return ADSP_EFAILED;
	}

	qurt_elite_mutex_lock(&p_sb_driver->sb_drv_mutex);

	/* If the device is not opened, first open the device */
	if(0 == p_sb_driver->dev_open_counter)
	{
		if(DAL_SUCCESS != (result = DalDevice_Open(p_sb_driver->p_core_driver, DAL_OPEN_SHARED)))
		{
			result = ADSP_EFAILED;
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open DAL device");
		}
	}

	/* Increment the device open counter for successful device open */
	if (ADSP_EOK == result)
	{
		p_sb_driver->dev_open_counter++;
	}

	qurt_elite_mutex_unlock(&p_sb_driver->sb_drv_mutex);

	return result;
}

ADSPResult afe_slimbus_core_driver_close(void *p_drv)
{
	ADSPResult result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

	if (NULL == p_sb_driver)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL SLIMbus driver pointer, p_sb_driver: 0x%X", p_sb_driver);
		return ADSP_EFAILED;
	}

	if (NULL == (p_sb_driver->p_core_driver))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL SLIMbus core driver pointer: 0x%X", p_sb_driver->p_core_driver);
		return ADSP_EFAILED;
	}

	qurt_elite_mutex_lock(&p_sb_driver->sb_drv_mutex);

	if(p_sb_driver->dev_open_counter > 0)
	{
		p_sb_driver->dev_open_counter--;

		if(0 == p_sb_driver->dev_open_counter)
		{
			if(DAL_SUCCESS != (result = DalDevice_Close(p_sb_driver->p_core_driver)))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Close DAL device");
				result = ADSP_EFAILED;
			}
		}
	}

	qurt_elite_mutex_unlock(&p_sb_driver->sb_drv_mutex);

	return result;
}

ADSPResult afe_slimbus_bw_voting(void *p_drv, uint32_t bw)
{
	ADSPResult result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

	if (NULL == p_sb_driver)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid Core SLIMBUS driver handle, not initialized");
		return ADSP_EUNSUPPORTED;
	}

	if (ADSP_EOK != (result = afe_slimbus_core_driver_open((void *)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to Open SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	//if value is 11534336 then CG is becoming as 10, that means clock is 24.576 MHz
	//if value is 8388608 then CG is becoming as 9, that means clock is 12.24 MHz
	//if value is 0 means relinquishing BW.
	MSG_1(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"DBG MSG Voting for increasing/decresing clk gear with bw %ld", bw);

	if(DAL_SUCCESS != (result = DalSlimBus_NextReserveMsgBandwidth(p_sb_driver->p_core_driver, bw)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to vote  BW, 0x%x", result);
		return ADSP_EFAILED;
	}

	if (DAL_SUCCESS != (result = DalSlimBus_DoReconfigureNow(p_sb_driver->p_core_driver)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to do ReconfigNow(), 0x%x", result);
		return ADSP_EFAILED;
	}

	if (ADSP_EOK != (result = afe_slimbus_core_driver_close((void*)p_sb_driver)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to close SLIMBUS core driver");
		return ADSP_EFAILED;
	}

	return result;
}

void* afe_slimbus_init_dev_handle(uint32_t afe_slimbus_id)
{
	ADSPResult              result = ADSP_EOK;
	uint32_t                dal_slimbus_id = 0x0;
	afe_slimbus_driver_t    *p_sb_drv = NULL;

	/* Convert AFE Slimbus driver Ids to DAL ids and also evaluate for valid Id */
	switch (afe_slimbus_id)
	{
	case AFE_SLIMBUS_DEVICE_1:
		dal_slimbus_id = DALDEVICEID_SLIMBUS_1;
		break;
	case AFE_SLIMBUS_DEVICE_2:
		dal_slimbus_id = DALDEVICEID_SLIMBUS_2;
		break;
	default:
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid SLIMBUS HW Module Id: %lu", afe_slimbus_id);
		return NULL;
	}

	/* Allocate and init the driver handle */
	if (NULL != (p_sb_drv = (afe_slimbus_driver_t *)qurt_elite_memory_malloc(sizeof(afe_slimbus_driver_t), QURT_ELITE_HEAP_DEFAULT)))
	{
		/* Store the SLIMbus ID in the allocated object */
		p_sb_drv->afe_slimbus_id = afe_slimbus_id;
		p_sb_drv->p_core_driver = NULL;

		/* Init the slimbus dev open counter to 0 */
		p_sb_drv->dev_open_counter = 0;

		/* Treat single endpoint per master port by default */
		p_sb_drv->is_it_multi_end_point = FALSE;

		/* Init the slimbus driver mutex */
		qurt_elite_mutex_init(&p_sb_drv->sb_drv_mutex);

		/* Initialize register sequencing mutex */
		qurt_elite_mutex_init( &(p_sb_drv->reg_seq_mutex) );

		/* Establish the connection to SLIMBUS driver. It will ensure that SLIMBUS SW driver and HW gets initialized */
		if ((DAL_SUCCESS != (result = DAL_SlimBusDeviceAttach(dal_slimbus_id, &p_sb_drv->p_core_driver))))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to attach to SLIMbus DAL device, result: 0x%x", result);
			goto __bailout;
		}

    /* Update multi-end point support */
    p_sb_drv->is_it_multi_end_point = afe_sb_dev_cfg[afe_slimbus_id].b_is_hw_support_multi_endpoint;

      if(p_sb_drv->is_it_multi_end_point)
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SLIMbus multi end point supported");
      }
      else
      {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SLIMbus multi end point not supported");
      }
    }

	return (void *)p_sb_drv;

	__bailout:
	/* Destroy the reg sequence Mutex */
	qurt_elite_mutex_destroy( &(p_sb_drv->reg_seq_mutex) );

	/* Destroy the SLIMbus driver Mutex */
	qurt_elite_mutex_destroy(&p_sb_drv->sb_drv_mutex);

	/* Free up the driver handle */
	qurt_elite_memory_free(p_sb_drv);

	return NULL;
}

void afe_slimbus_deinit_dev_handle(void *dev_handle)
{
	ADSPResult  result = ADSP_EOK;
	afe_slimbus_driver_t *p_sb_drv;

	if (NULL != dev_handle)
	{
		p_sb_drv = (afe_slimbus_driver_t *)dev_handle;

		if (NULL != p_sb_drv->p_core_driver)
		{
			/* Detach the SLIMbus device */
			if ((DAL_SUCCESS != (result = DAL_DeviceDetach(p_sb_drv->p_core_driver))))
			{
				MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to detach SLIMbus DAL device");
			}
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SLIMbus core drv pointer is NULL");
		}

		/* Destroy the reg sequence Mutex */
		qurt_elite_mutex_destroy( &(p_sb_drv->reg_seq_mutex) );

		/* Destroy the SLIMbus driver Mutex */
		qurt_elite_mutex_destroy(&p_sb_drv->sb_drv_mutex);

		/* Free up the driver handle */
		qurt_elite_memory_free(dev_handle);
	}
}

ADSPResult afe_restore_slimbus_cfg(afe_dev_port_t *afe_port_ptr)
{
	ADSPResult                    result = ADSP_EFAILED;
	afe_slimbus_state_struct_t    *p_dev_state = (afe_slimbus_state_struct_t *)afe_port_ptr->afe_drv;
	afe_param_id_slimbus_cfg_t    sb_cfg_info;

	memset(&sb_cfg_info, 0, sizeof(sb_cfg_info));

	if (NULL != p_dev_state)
	{
		sb_cfg_info.sb_cfg_minor_version = p_dev_state->afe_sb_cfg.cfg_version;
		sb_cfg_info.slimbus_dev_id = p_dev_state->afe_sb_cfg.slimbus_dev_id;
		sb_cfg_info.bit_width = p_dev_state->afe_sb_cfg.bit_width;
		sb_cfg_info.data_format = p_dev_state->afe_sb_cfg.data_format;
		sb_cfg_info.num_channels = p_dev_state->afe_sb_cfg.num_channels;
		sb_cfg_info.sample_rate = p_dev_state->afe_sb_cfg.sample_rate;

		memscpy(&sb_cfg_info.shared_channel_mapping,
				sizeof(sb_cfg_info.shared_channel_mapping),
				&p_dev_state->afe_sb_cfg.shared_channel_mapping,
				sizeof(p_dev_state->afe_sb_cfg.shared_channel_mapping));

		if(ADSP_EOK != (result = afe_set_slimbus_cfg(afe_port_ptr,
				(int8_t *)&sb_cfg_info,
				sizeof(sb_cfg_info),
				TRUE, 0)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to restore SLIMbus config, port_id: 0x%lx", afe_port_ptr->intf);

			result = ADSP_EFAILED;
		}
	}

	return result;
}

/**
 * To get core slimbus driver handle
 *
 * @param[in] dev_handle: Valid driver handle
 *
 * @return
 * handle to core slimbus driver
 *
 * @dependencies
 * None.
 *
 */
void *afe_sb_get_core_drv_hdl(void* p_drv)
{
  afe_slimbus_driver_t *p_sb_driver = (afe_slimbus_driver_t *)p_drv;

  if(NULL != p_sb_driver)
  {
    return (void*)p_sb_driver->p_core_driver;
  }
  return NULL;
}
