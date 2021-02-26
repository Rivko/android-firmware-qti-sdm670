/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputDrv.cpp#2 $
$DateTime: 2017/11/23 06:25:06 $
$Author: pwbldsvc $
$Change: 14912369 $
$Revision: #2 $

FILE:     hdmi_input_drv.cpp

DESCRIPTION: Main Interface to the AFE Slimbus driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.
QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputDrv.cpp#2 $ $DateTime: 2017/11/23 06:25:06 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
7/30/2012   RP   Created


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "qurt_elite.h"
#include "qurt_error.h"
#include "AFEDeviceDriver.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALSys.h"
#include "AFEInternal.h"
#include "AFEInterface.h"
#include "adsp_afe_service_commands.h"
#include "AFEHdmiOutputDrv_i.h"
#include "AFEHdmiOutputDrv.h"
#include "AFEHdmiOutputGroupDevice.h"
#include "hwd_devcfg.h"
#include "hwd_mmpm.h"
#include "AFEDmaManager.h"
#include "AFEHalLpassHdmiOutput.h"
#include "AFEHalMdssHdmiOutput.h"

#include "AFEChStatusUtils.h"
#include "avtimer_drv_api.h"



#define        CH_STATUS_SW
#undef         CH_STATUS_SW

/*=====================================================================
 Globals
 ======================================================================*/

#define HDMI_BLOCK_SIZE   192   //one HDMI block has 192 frames, one HDMI frame has two subframes, and one subframe has 32 time slots.

#define HDMI_IEC60958_P_BIT31          31
#define HDMI_IEC60958_C_BIT30          30
#define HDMI_IEC60958_U_BIT29          29
#define HDMI_IEC60958_V_BIT28          28

#define HDMI_IEC60958_PREAMBLE_BIT3    3
#define HDMI_IEC60958_PREAMBLE_BIT2    2
#define HDMI_IEC60958_PREAMBLE_BIT1    1
#define HDMI_IEC60958_PREAMBLE_BIT0    0

#define HDMI_IEC60958_AUX_START_BIT4   4
#define HDMI_IEC60958_MSB_ALIGN_BIT8   8

#define HDMI_CFG_V1                   0x1

#define HDMI_DRIVER_NEXT_EVEN_VALUE(num)  ((num + 1) & 0xFFFFFFFEUL)

#define HDMI_OUTPUT_NUM_BUFFERS_PING_PONG          2

#define DP_ENGINE_FIFO_DEPTH_IN_SAMPLES  192

#define HDMI_TOTAL_CA_CNT 0x20
const uint8 hdmiChannelAllocArray[HDMI_TOTAL_CA_CNT][9] =
{
		//channel Number
		//Ch #1   2     3      4     5     6     7     8
		{0,  1,  0,  0,  0,  0,  0,  0,  2},    //ca = 0
		{0,  1,  2,  0,  0,  0,  0,  0,  3},    //ca = 1
		{0,  1,  3,  0,  0,  0,  0,  0,  3},    //ca = 2
		{0,  1,  2,  3,  0,  0,  0,  0,  4},    //ca = 3
		{0,  1,  4,  0,  0,  0,  0,  0,  3},    //ca = 4
		{0,  1,  2,  4,  0,  0,  0,  0,  4},    //ca = 5
		{0,  1,  3,  4,  0,  0,  0,  0,  4},    //ca = 6
		{0,  1,  2,  3,  4,  0,  0,  0,  5},    //ca = 7
		{0,  1,  4,  5,  0,  0,  0,  0,  4},    //ca = 8
		{0,  1,  2,  4,  5,  0,  0,  0,  5},    //ca = 9
		{0,  1,  3,  4,  5,  0,  0,  0,  5},    //ca = 10
		{0,  1,  2,  3,  4,  5,  0,  0,  6},    //ca = 11
		{0,  1,  4,  5,  6,  0,  0,  0,  5},    //ca = 12
		{0,  1,  2,  4,  5,  6,  0,  0,  6},    //ca = 13
		{0,  1,  3,  4,  5,  6,  0,  0,  6},    //ca = 14
		{0,  1,  2,  3,  4,  5,  6,  0,  7},    //ca = 15
		{0,  1,  4,  5,  6,  7,  0,  0,  6},    //ca = 16
		{0,  1,  2,  4,  5,  6,  7,  0,  7},    //ca = 17
		{0,  1,  3,  4,  5,  6,  7,  0,  7},    //ca = 18
		{0,  1,  2,  3,  4,  5,  6,  7,  8},    //ca = 19
		{0,  1,  6,  7,  0,  0,  0,  0,  4},    //ca = 20
		{0,  1,  2,  6,  7,  0,  0,  0,  5},    //ca = 21
		{0,  1,  3,  6,  7,  0,  0,  0,  5},    //ca = 22
		{0,  1,  2,  3,  6,  7,  0,  0,  6},    //ca = 23
		{0,  1,  4,  6,  7,  0,  0,  0,  5},    //ca = 24
		{0,  1,  2,  4,  6,  7,  0,  0,  6},    //ca = 25
		{0,  1,  3,  4,  6,  7,  0,  0,  6},    //ca = 26
		{0,  1,  2,  3,  4,  6,  7,  0,  7},    //ca = 27
		{0,  1,  4,  5,  6,  7,  0,  0,  6},    //ca = 28
		{0,  1,  2,  4,  5,  6,  7,  0,  7},    //ca = 29
		{0,  1,  3,  4,  5,  6,  7,  0,  7},    //ca = 30
		{0,  1,  2,  3,  4,  5,  6,  7,  8},    //ca = 31
};

#define HDMI_MAX_CA_VALUE 0x1f
//CA value starts from 0, hence array size = HDMI_MAX_CA_VALUE+1.
static int16_t HDMI_CA_to_num_channel_array[HDMI_MAX_CA_VALUE+1] = \
		{2,    //NA_NA_NA_NA_NA_NA_FR_FL       0
	3,    //NA_NA_NA_NA_NA_LFE_FR_FL      1
	3,    //NA_NA_NA_NA_FC_NA_FR_FL       2
	4,    //NA_NA_NA_NA_FC_LFE_FR_FL      3

	3,    //NA_NA_NA_RC_NA_NA_FR_FL       4
	4,    //NA_NA_NA_RC_NA_LFE_FR_FL      5
	4,    //NA_NA_NA_RC_FC_NA_FR_FL       6
	5,    //NA_NA_NA_RC_FC_LFE_FR_FL      7

	4,    //NA_NA_RR_RL_NA_NA_FR_FL       8
	5,    //NA_NA_RR_RL_NA_LFE_FR_FL      9
	5,    //NA_NA_RR_RL_FC_NA_FR_FL       a
	6,    //NA_NA_RR_RL_FC_LFE_FR_FL      b

	5,    //NA_RC_RR_RL_NA_NA_FR_FL       c
	6,    //NA_RC_RR_RL_NA_LFE_FR_FL      d
	6,    //NA_RC_RR_RL_FC_NA_FR_FL       e
	7,    //NA_RC_RR_RL_FC_LFE_FR_FL      f

	6,    //RRC_RLC_RR_RL_NA_NA_FR_FL     10
	7,    //RRC_RLC_RR_RL_NA_LFE_FR_FL    11
	7,    //RRC_RLC_RR_RL_FC_NA_FR_FL     12
	8,    //RRC_RLC_RR_RL_FC_LFE_FR_FL    13

	4,    //FRC_FLC_NA_NA_NA_NA_FR_FL     14
	5,    //FRC_FLC_NA_NA_NA_LFE_FR_FL    15
	5,    //FRC_FLC_NA_NA_FC_NA_FR_FL     16
	6,    //FRC_FLC_NA_NA_FC_LFE_FR_FL    17

	5,    //FRC_FLC_NA_RC_NA_NA_FR_FL     18
	6,    //FRC_FLC_NA_RC_NA_LFE_FR_FL    19
	6,    //FRC_FLC_NA_RC_FC_NA_FR_FL     1a
	7,    //FRC_FLC_NA_RC_FC_LFE_FR_FL    1b

	6,    //FRC_FLC_RR_RL_NA_NA_FR_FL     1c
	7,    //FRC_FLC_RR_RL_NA_LFE_FR_FL    1d
	7,    //FRC_FLC_RR_RL_FC_NA_FR_FL     1e
	8,    //FRC_FLC_RR_RL_FC_LFE_FR_FL    1f -> HDMI_MAX_CA_VALUE
		};


//HDMI output hw related state, special to this hardware
static hdmi_output_port_state_t      hdmi_output_port_state[AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE/2];

//for communication between hdmi output driver and AFE SVC
static afe_dev_port_t              hdmi_output_port[AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE/2];

//global variable for hdmi output driver to share any variable common to all ports for example ADSPPM ID, etc.
afe_hdmi_output_driver_t    hdmi_output_driver;

/*=====================================================================
 Static Functions
 ======================================================================*/
static ADSPResult hdmi_output_driver_open_ss(afe_dev_port_t *dev_port_ptr);    //for single stream case

static ADSPResult hdmi_output_driver_open_ms(afe_dev_port_t *dev_port_ptr);    //for multiple stream case

static ADSPResult hdmi_output_driver_open_ms_sub(hdmi_output_port_state_t *dev_state_ptr);    //for multiple stream case

static ADSPResult hdmi_output_driver_close_ss(afe_dev_port_t *dev_port_ptr);    //for single stream case

static ADSPResult hdmi_output_driver_close_ms(afe_dev_port_t *dev_port_ptr);    //for multiple stream case


void afe_hdmi_output_pack_data(afe_hdmi_output_pack_info_t *pack_info_ptr,
		int16 *p_src,
		int32 *p_dst,
		int32 frame,
		int32 numch);

void afe_hdmi_output_pack_data_dp(afe_hdmi_output_pack_info_t *pack_info_ptr,
		int16 *p_src,
		int32 *p_dst,
		int32 frame,
		int32 numch);


static void afe_hdmi_output_int_callback(void* cb_context, uint32_t event_mask);


static ADSPResult afe_hdmi_dp_driver_int_notify(afe_dev_port_t *dev_port_ptr, uint32_t event_mask);    //for DP special handle case

static ADSPResult afe_hdmi_dp_driver_get_freq_idx(uint32 sample_freq, uint16_t *freq_index);
static uint16 afe_hdmi_dp_driver_get_cc_idx(uint32 num_channel); 


/*=====================================================================
 Functions
 ======================================================================*/

/**
  @brief Perform HDMI Output Driver Initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_driver_init(void)
{
	ADSPResult   result = ADSP_EOK;
	lpasshwio_prop_hdmi_output_struct_t *hdmi_hw_info_ptr = &hdmi_output_driver.hdmi_hw_info;

	afe_hdmi_output_group_device_state_struct_t        *p_group_device;

	memset(&hdmi_output_driver, 0, sizeof(afe_hdmi_output_driver_t));

	if(ADSP_EOK != (result = HwdDevCfg_ReadModuleConfigData(HWD_MODULE_TYPE_HDMI_OUTPUT, (void *)hdmi_hw_info_ptr)))
	{
		return result;
	}

	//create memory region for lpass HDMI hw
	if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&hdmi_output_driver.lpass_qmem_region,
			hdmi_hw_info_ptr->lpass_reg_base_addr, hdmi_hw_info_ptr->lpass_reg_size, &hdmi_output_driver.lpass_reg_virt_addr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for lpass HDMI output register base");
		return result;
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpass HDMI output virtual addr 0x%x phy addr 0x%x",
				(unsigned int)hdmi_output_driver.lpass_reg_virt_addr, (unsigned int)hdmi_hw_info_ptr->lpass_reg_base_addr);
	}

	//create memory region for lpass HDMI hw reset registers
	if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&hdmi_output_driver.lpass_reset_qmem_region,
			hdmi_hw_info_ptr->lpass_reset_reg_base_addr, hdmi_hw_info_ptr->lpass_reset_reg_size, &hdmi_output_driver.lpass_reset_reg_virt_addr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for HDMI output reset register base");
		HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.lpass_qmem_region);
		return result;
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "lpass HDMI output reset virtual addr 0x%x phy addr 0x%x",
				(unsigned int)hdmi_output_driver.lpass_reset_reg_virt_addr, (unsigned int)hdmi_hw_info_ptr->lpass_reset_reg_base_addr);
	}

	//create memory region for MDP HDMI core hw
	if(ADSP_EOK != (result = HwdDevCfg_GetVirtAddress(&hdmi_output_driver.hdmi_core_qmem_region,
			hdmi_hw_info_ptr->hdmi_core_reg_base_addr, hdmi_hw_info_ptr->hdmi_core_reg_size, &hdmi_output_driver.hdmi_core_reg_virt_addr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error obtaining virtual addr for MDP HDMI core register base");
		//how to de-mapping for above success case
		HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.lpass_qmem_region);
		HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.lpass_reset_qmem_region);
		return result;
	}
	else
	{
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "MDP HDMI core virtual addr 0x%x phy addr 0x%x",
				(unsigned int)hdmi_output_driver.hdmi_core_reg_virt_addr, (unsigned int)hdmi_hw_info_ptr->hdmi_core_reg_base_addr);
	}


	hal_lpass_hdmi_output_init(hdmi_hw_info_ptr->hw_revision, hdmi_hw_info_ptr->lpass_reg_base_addr, hdmi_output_driver.lpass_reg_virt_addr, hdmi_hw_info_ptr->lpass_reset_reg_base_addr, hdmi_output_driver.lpass_reset_reg_virt_addr);
	hal_mdss_hdmi_aud_init(hdmi_hw_info_ptr->hw_revision, hdmi_hw_info_ptr->hdmi_core_reg_base_addr, hdmi_output_driver.hdmi_core_reg_virt_addr);

	hdmi_output_driver.driver_state = HDMI_DRIVER_INIT;

	//get group_info_ptr and do initization here
	p_group_device = afe_get_hdmi_output_group_device_state_object();
	hdmi_output_driver.group_info_ptr = (void*)p_group_device;

	memset(hdmi_output_driver.group_info_ptr, 0, sizeof(afe_hdmi_output_group_device_state_struct_t));

	p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP;

	p_group_device->is_enabled = FALSE;

	return result;
}

/**
  @brief Perform HDMI Output Driver De-initialization

  @param[in] none

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_driver_deinit(void)
{
	if(HDMI_DRIVER_INIT != hdmi_output_driver.driver_state)
	{
		return ADSP_EFAILED;
	}

	/* release all resource allocated during init*/
	HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.lpass_qmem_region);
	HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.lpass_reset_qmem_region);
	HwdDevCfg_DestoryVirtAddress(hdmi_output_driver.hdmi_core_qmem_region);


	hdmi_output_driver.driver_state = HDMI_DRIVER_DEINIT;

	return ADSP_EOK;
}

/**
  @brief Perform HDMI Output port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_port_init(aud_stat_afe_svc_t* afe_svc_ptr)
{
	ADSPResult result = ADSP_EOK;
	uint16_t port_id, dir;
	afe_dev_port *dev_port_ptr = NULL;
	hdmi_output_port_state_t *port_state_ptr = NULL;
	int idx = 0;

	if(HDMI_DRIVER_INIT != hdmi_output_driver.driver_state)
	{
		return ADSP_ENOTREADY;
	}

	for(port_id = AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START; port_id <= AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_END; port_id++)
	{
		idx = (port_id - AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START)/2;   //idx = 0, 1, 2, 3

		dev_port_ptr = &hdmi_output_port[idx];
		port_state_ptr = &hdmi_output_port_state[idx];

		dir = port_id & AFE_PORT_DIR_MASK;

		if(dev_port_ptr && port_state_ptr && !dir)
		{

			memset(port_state_ptr, 0, sizeof(hdmi_output_port_state_t));

			if (ADSP_FAILED(result = afe_svc_init_port(dev_port_ptr, port_id, dir)))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to initialize HDMI Output port = %d, aborting", port_id);
				return result;
			}

			afe_svc_ptr->port_hdmi_output_ptr[idx] = dev_port_ptr;
			dev_port_ptr->avt_drift_params.avt_drift_struct_ptr = &port_state_ptr->hdmi_output_avt_drift;
			dev_port_ptr->avt_drift_params.avtimer_latch_type = TIMESTAMP_LATCH_TYPE_HARDWARE;

			/* register device process function */
			dev_port_ptr->afe_dev_process = (afe_dev_port_proc_func)afe_hdmi_output_driver_write;

			/* register device state data */
			dev_port_ptr->afe_drv = (void *)port_state_ptr;

		}
	}

	return result;
}

/**
  @brief Perform HDMI Output port initialization

  @param[in] afe_svc_ptr pointer for AFE static service

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_port_deinit(aud_stat_afe_svc_t* afe_svc_ptr)
{
	uint32_t port_id;
	int idx = 0;
	uint16_t dir;

	if(HDMI_DRIVER_INIT != hdmi_output_driver.driver_state)
	{
		return ADSP_ENOTREADY;
	}

	for(port_id = AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START; port_id <= AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_END; port_id++)
	{
		idx = (port_id - AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START)/2;   //idx = 0, 1, 2, 3

		dir = port_id & AFE_PORT_DIR_MASK;

		if ((NULL != afe_svc_ptr->port_hdmi_output_ptr[idx]) && (!dir))
		{
			afe_svc_deinit_port(afe_svc_ptr->port_hdmi_output_ptr[idx]);
		}
	}

	return ADSP_EOK;
}

/**
  @brief Set HDMI Output port config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_set_hdmi_output_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
	/* need to add error check for parameters. */
	afe_param_id_hdmi_multi_chan_audio_cfg_t *hdmi_cfg_ptr = (afe_param_id_hdmi_multi_chan_audio_cfg_t *)cfg;
	int i;

	uint32_t hdmi_cfg_version = (uint32_t) *(cfg);

	if(HDMI_DRIVER_INIT != hdmi_output_driver.driver_state)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI Output driver not ready to receive command");
		return ADSP_EFAILED;
	}

	if(hdmi_cfg_version > AFE_API_VERSION_HDMI_CONFIG)
	{
		hdmi_cfg_version = AFE_API_VERSION_HDMI_CONFIG;
	}

	if(HDMI_CFG_V1 != hdmi_cfg_version)
	{
		return ADSP_EUNSUPPORTED;
	}

	if (payload_size < sizeof(afe_param_id_hdmi_multi_chan_audio_cfg_t))
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI Output Cfg fail: Invalid payload size: %ld, port_id: 0x%x", payload_size, pDevPort->intf);
		return ADSP_EBADPARAM;
	}
	else
	{
		hdmi_output_port_state_t *p_dev_state = (hdmi_output_port_state_t *) pDevPort->afe_drv;

		// Printing the device configuration information
		MSG(MSG_SSID_QDSP6, DBG_MED_PRIO,"Device configuration for HDMI Output Port");
		MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO,"data type: %d Channel Allocation: %d Sample rate: %lu",hdmi_cfg_ptr->data_type, hdmi_cfg_ptr->channel_allocation, hdmi_cfg_ptr->sample_rate);
		MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"bit width: %d hdmi cfg minor version: %lu",hdmi_cfg_ptr->bit_width, hdmi_cfg_ptr->hdmi_cfg_minor_version);

		// parse channel allocation info
		if (HDMI_MAX_CA_VALUE < hdmi_cfg_ptr->channel_allocation )
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported Channel Allocation: %u, port: 0x%x", hdmi_cfg_ptr->channel_allocation, pDevPort->intf);
			return ADSP_EUNSUPPORTED;
		}
		p_dev_state->channel_alloc = hdmi_cfg_ptr->channel_allocation;

		if(0 != p_dev_state->channel_alloc)
		{
			for (i = 0; i <= HDMI_8_CHANNELS; i++)
			{
				p_dev_state->hdmi_ch_map[i] = (uint8)hdmiChannelAllocArray[p_dev_state->channel_alloc][i];
			}
		}

		// derive number of channels from CA value
		p_dev_state->num_channels = HDMI_CA_to_num_channel_array[p_dev_state->channel_alloc];

		// parse bit width information
		if ((16 != hdmi_cfg_ptr->bit_width) && (24 != hdmi_cfg_ptr->bit_width))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported bit width: %x, intf: %x", hdmi_cfg_ptr->bit_width, pDevPort->intf);
			return ADSP_EUNSUPPORTED;
		}
		p_dev_state->bit_width = hdmi_cfg_ptr->bit_width;

		// parse data format info
		if ((AFE_LINEAR_PCM_DATA != hdmi_cfg_ptr->data_type)&&
				(AFE_NON_LINEAR_DATA != hdmi_cfg_ptr->data_type))
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported data type: %u, intf: 0x%x", hdmi_cfg_ptr->data_type, pDevPort->intf);
			return ADSP_EUNSUPPORTED;
		}

		p_dev_state->data_type = hdmi_cfg_ptr->data_type;

		// parse the sample rate
		if ((AFE_PORT_SAMPLE_RATE_8K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_16K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_32K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_48K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_96K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_192K != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_22_05K  != hdmi_cfg_ptr->sample_rate)&& // compresseed scenarios only
				(AFE_PORT_SAMPLE_RATE_44_1K  != hdmi_cfg_ptr->sample_rate)&& // compresseed scenarios only, Tx in non compressed
				(AFE_PORT_SAMPLE_RATE_88_2K  != hdmi_cfg_ptr->sample_rate)&&
				(AFE_PORT_SAMPLE_RATE_176_4K != hdmi_cfg_ptr->sample_rate)) // compresseed scenarios only
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Un-supported sampling rate: %lu, intf: %x", hdmi_cfg_ptr->sample_rate, pDevPort->intf);
			return ADSP_EUNSUPPORTED;
		}

		p_dev_state->sample_rate = hdmi_cfg_ptr->sample_rate;

		if (AFE_NON_LINEAR_DATA == p_dev_state->data_type)
		{
			pDevPort->dev_data_type = AFE_IEC61937_COMPRESSED_DATA_PORT;
		}
		else if (AFE_DSD_DOP_DATA == p_dev_state->data_type)
		{
			pDevPort->dev_data_type = AFE_DSD_DOP_COMPRESSED_DATA_PORT;
		}
		else
		{
			pDevPort->dev_data_type = AFE_UNCOMPRESSED_DATA_PORT;
		}

		pDevPort->dev_bytes_per_channel = (p_dev_state->bit_width > 16) ? 4 : 2;
		pDevPort->dev_bit_width = p_dev_state->bit_width;
		pDevPort->dev_channels = p_dev_state->num_channels;
		pDevPort->dev_sample_rate = p_dev_state->sample_rate;

		//if applicable, overwrite the port media type with the device media type.
		//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
		afe_port_copy_media_type_dev_to_port(pDevPort);

		pDevPort->is_interleaved = TRUE;
	}

	return ADSP_EOK;
}


/**
  @brief Set HDMI Output port channel status config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] cfg_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_set_chstatus_cfg(afe_dev_port_t *pDevPort, int8_t *cfg, uint32_t payload_size)
{
	ADSPResult result = ADSP_EOK;
	hdmi_output_port_state_t *p_dev_state = (hdmi_output_port_state_t *) pDevPort->afe_drv;
	afe_param_id_chstatus_cfg_t *p_chstatus =  (afe_param_id_chstatus_cfg_t *)cfg;
	chstatus_check_point_t chsts_check_param;
	uint32 temp_mask_32bit0, temp_mask_32bit1, temp_mask_32bit2, temp_mask_32bit3, temp_mask_32bit4, temp_mask_32bit5;
	uint32 temp_ch_status_32bit;

	temp_mask_32bit0 = 0;
	temp_mask_32bit1 = 0;
	temp_mask_32bit2 = 0;
	temp_mask_32bit3 = 0;
	temp_mask_32bit4 = 0;
	temp_mask_32bit5 = 0;

	if((AFE_PORT_STATE_CONFIG == pDevPort->port_state) || (AFE_PORT_STATE_RUN == pDevPort->port_state))
	{
		chsts_check_param.bit_width    = p_dev_state->bit_width;
		chsts_check_param.data_format  = p_dev_state->data_type;
		chsts_check_param.num_channels = p_dev_state->num_channels;
		chsts_check_param.sample_rate  = p_dev_state->sample_rate;

		if(ADSP_EOK != (result = afe_validate_chstatus_cfg(p_chstatus, &chsts_check_param)))
		{
			return ADSP_EBADPARAM;
		}
	}

	p_dev_state->channel_status_type = p_chstatus->ch_status_type;

	if(AFE_CH_STATUS_A == p_chstatus->ch_status_type)
	{
		temp_mask_32bit0 = (p_chstatus->ch_stat_mask[3]<<24) | (p_chstatus->ch_stat_mask[2]<<16) |
				(p_chstatus->ch_stat_mask[1]<<8) | p_chstatus->ch_stat_mask[0];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[3]<<24) | (p_chstatus->ch_stat_buf[2]<<16) |
				(p_chstatus->ch_stat_buf[1]<<8) | p_chstatus->ch_stat_buf[0];
		p_dev_state->ch_sts_buf_0.ch_sts_data0 &=  (~temp_mask_32bit0);
		p_dev_state->ch_sts_buf_0.ch_sts_data0 |=  (temp_ch_status_32bit & temp_mask_32bit0);

		temp_mask_32bit1 = (p_chstatus->ch_stat_mask[7]<<24) | (p_chstatus->ch_stat_mask[6]<<16) |
				(p_chstatus->ch_stat_mask[5]<<8) | p_chstatus->ch_stat_mask[4];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[7]<<24) | (p_chstatus->ch_stat_buf[6]<<16) |
				(p_chstatus->ch_stat_buf[5]<<8) | p_chstatus->ch_stat_buf[4];
		p_dev_state->ch_sts_buf_0.ch_sts_data1 &=  (~temp_mask_32bit1);
		p_dev_state->ch_sts_buf_0.ch_sts_data1 |=  (temp_ch_status_32bit & temp_mask_32bit1);

		temp_mask_32bit2 = (p_chstatus->ch_stat_mask[11]<<24) | (p_chstatus->ch_stat_mask[10]<<16) |
				(p_chstatus->ch_stat_mask[9]<<8) | p_chstatus->ch_stat_mask[8];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[11]<<24) | (p_chstatus->ch_stat_buf[10]<<16) |
				(p_chstatus->ch_stat_buf[9]<<8) | p_chstatus->ch_stat_buf[8];
		p_dev_state->ch_sts_buf_0.ch_sts_data2 &=  (~temp_mask_32bit2);
		p_dev_state->ch_sts_buf_0.ch_sts_data2 |=  (temp_ch_status_32bit & temp_mask_32bit2);

		temp_mask_32bit3 = (p_chstatus->ch_stat_mask[15]<<24) | (p_chstatus->ch_stat_mask[14]<<16) |
				(p_chstatus->ch_stat_mask[13]<<8) | p_chstatus->ch_stat_mask[12];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[15]<<24) | (p_chstatus->ch_stat_buf[14]<<16) |
				(p_chstatus->ch_stat_buf[13]<<8) | p_chstatus->ch_stat_buf[12];
		p_dev_state->ch_sts_buf_0.ch_sts_data3 &=  (~temp_mask_32bit3);
		p_dev_state->ch_sts_buf_0.ch_sts_data3 |=  (temp_ch_status_32bit & temp_mask_32bit3);

		temp_mask_32bit4 = (p_chstatus->ch_stat_mask[19]<<24) | (p_chstatus->ch_stat_mask[18]<<16) |
				(p_chstatus->ch_stat_mask[17]<<8) | p_chstatus->ch_stat_mask[16];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[19]<<24) | (p_chstatus->ch_stat_buf[18]<<16) |
				(p_chstatus->ch_stat_buf[17]<<8) | p_chstatus->ch_stat_buf[16];
		p_dev_state->ch_sts_buf_0.ch_sts_data4 &=  (~temp_mask_32bit4);
		p_dev_state->ch_sts_buf_0.ch_sts_data4 |=  (temp_ch_status_32bit & temp_mask_32bit4);

		temp_mask_32bit5 = (p_chstatus->ch_stat_mask[23]<<24) | (p_chstatus->ch_stat_mask[22]<<16) |
				(p_chstatus->ch_stat_mask[21]<<8) | p_chstatus->ch_stat_mask[20];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[23]<<24) | (p_chstatus->ch_stat_buf[22]<<16) |
				(p_chstatus->ch_stat_buf[21]<<8) | p_chstatus->ch_stat_buf[20];
		p_dev_state->ch_sts_buf_0.ch_sts_data5 &=  (~temp_mask_32bit5);
		p_dev_state->ch_sts_buf_0.ch_sts_data5 |=  (temp_ch_status_32bit & temp_mask_32bit5);

	}
	else if(AFE_CH_STATUS_B == p_chstatus->ch_status_type)
	{
		temp_mask_32bit0 = (p_chstatus->ch_stat_mask[3]<<24) | (p_chstatus->ch_stat_mask[2]<<16) |
				(p_chstatus->ch_stat_mask[1]<<8) | p_chstatus->ch_stat_mask[0];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[3]<<24) | (p_chstatus->ch_stat_buf[2]<<16) |
				(p_chstatus->ch_stat_buf[1]<<8) | p_chstatus->ch_stat_buf[0];
		p_dev_state->ch_sts_buf_1.ch_sts_data0 &=  (~temp_mask_32bit0);
		p_dev_state->ch_sts_buf_1.ch_sts_data0 |=  (temp_ch_status_32bit & temp_mask_32bit0);

		temp_mask_32bit1 = (p_chstatus->ch_stat_mask[7]<<24) | (p_chstatus->ch_stat_mask[6]<<16) |
				(p_chstatus->ch_stat_mask[5]<<8) | p_chstatus->ch_stat_mask[4];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[7]<<24) | (p_chstatus->ch_stat_buf[6]<<16) |
				(p_chstatus->ch_stat_buf[5]<<8) | p_chstatus->ch_stat_buf[4];
		p_dev_state->ch_sts_buf_1.ch_sts_data1 &=  (~temp_mask_32bit1);
		p_dev_state->ch_sts_buf_1.ch_sts_data1 |=  (temp_ch_status_32bit & temp_mask_32bit1);

		temp_mask_32bit2 = (p_chstatus->ch_stat_mask[11]<<24) | (p_chstatus->ch_stat_mask[10]<<16) |
				(p_chstatus->ch_stat_mask[9]<<8) | p_chstatus->ch_stat_mask[8];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[11]<<24) | (p_chstatus->ch_stat_buf[10]<<16) |
				(p_chstatus->ch_stat_buf[9]<<8) | p_chstatus->ch_stat_buf[8];
		p_dev_state->ch_sts_buf_1.ch_sts_data2 &=  (~temp_mask_32bit2);
		p_dev_state->ch_sts_buf_1.ch_sts_data2 |=  (temp_ch_status_32bit & temp_mask_32bit2);

		temp_mask_32bit3 = (p_chstatus->ch_stat_mask[15]<<24) | (p_chstatus->ch_stat_mask[14]<<16) |
				(p_chstatus->ch_stat_mask[13]<<8) | p_chstatus->ch_stat_mask[12];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[15]<<24) | (p_chstatus->ch_stat_buf[14]<<16) |
				(p_chstatus->ch_stat_buf[13]<<8) | p_chstatus->ch_stat_buf[12];
		p_dev_state->ch_sts_buf_1.ch_sts_data3 &=  (~temp_mask_32bit3);
		p_dev_state->ch_sts_buf_1.ch_sts_data3 |=  (temp_ch_status_32bit & temp_mask_32bit3);

		temp_mask_32bit4 = (p_chstatus->ch_stat_mask[19]<<24) | (p_chstatus->ch_stat_mask[18]<<16) |
				(p_chstatus->ch_stat_mask[17]<<8) | p_chstatus->ch_stat_mask[16];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[19]<<24) | (p_chstatus->ch_stat_buf[18]<<16) |
				(p_chstatus->ch_stat_buf[17]<<8) | p_chstatus->ch_stat_buf[16];
		p_dev_state->ch_sts_buf_1.ch_sts_data4 &=  (~temp_mask_32bit4);
		p_dev_state->ch_sts_buf_1.ch_sts_data4 |=  (temp_ch_status_32bit & temp_mask_32bit4);

		temp_mask_32bit5 = (p_chstatus->ch_stat_mask[23]<<24) | (p_chstatus->ch_stat_mask[22]<<16) |
				(p_chstatus->ch_stat_mask[21]<<8) | p_chstatus->ch_stat_mask[20];
		temp_ch_status_32bit = (p_chstatus->ch_stat_buf[23]<<24) | (p_chstatus->ch_stat_buf[22]<<16) |
				(p_chstatus->ch_stat_buf[21]<<8) | p_chstatus->ch_stat_buf[20];
		p_dev_state->ch_sts_buf_1.ch_sts_data5 &=  (~temp_mask_32bit5);
		p_dev_state->ch_sts_buf_1.ch_sts_data5 |=  (temp_ch_status_32bit & temp_mask_32bit5);

	}


	if((AFE_PORT_STATE_RUN == pDevPort->port_state))
	{
		if((0x4 != temp_mask_32bit0) &&
				(0 != temp_mask_32bit1) &&
				(0 != temp_mask_32bit2) &&
				(0 != temp_mask_32bit3) &&
				(0 != temp_mask_32bit4) &&
				(0 != temp_mask_32bit5))
		{
			result = ADSP_EBADPARAM;
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_HDMI_OUT:afe_hdmi_output_set_chstatus_cfg : error : try to change other than cp bit [state:%d]", pDevPort->port_state);
		}

		if(ADSP_EOK == result)
		{
			hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg;
			//fill the channel status
			if(AFE_CH_STATUS_A == p_dev_state->channel_status_type)
			{
				chs_cfg.dma_idx = p_dev_state->dma_idx;
				chs_cfg.chs_buf_idx = (p_dev_state->dma_idx * 2);
				chs_cfg.hw_mode = 1;
				chs_cfg.ch_sts_data0 = p_dev_state->ch_sts_buf_0.ch_sts_data0;
				chs_cfg.ch_sts_data1 = p_dev_state->ch_sts_buf_0.ch_sts_data1;

				hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);
			}

			if(AFE_CH_STATUS_B == p_dev_state->channel_status_type)
			{
				chs_cfg.dma_idx = p_dev_state->dma_idx;
				chs_cfg.chs_buf_idx = (p_dev_state->dma_idx * 2) + 1;
				chs_cfg.hw_mode = 1;
				chs_cfg.ch_sts_data0 = p_dev_state->ch_sts_buf_1.ch_sts_data0;
				chs_cfg.ch_sts_data1 = p_dev_state->ch_sts_buf_1.ch_sts_data1;

				hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);
			}
			//dynamic udpate of channel status table which is needed new register definition.
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE_HDMI_OUT:afe_hdmi_output_set_chstatus_cfg : dynamic chsts update");
		}
	}

	return result;
}

/**
  @brief Get HDMI Output port config

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] param_buf_ptr   pointer to the configuration structure
  @param[in] payload_size   size of the configuration payload
  @param[out] params_buffer_len_req_ptr   pointer to store the size of the filled buffer

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_get_cfg (afe_dev_port_t *dev_port_ptr, int8_t *param_buf_ptr, uint32_t payload_size, uint16_t *params_buffer_len_req_ptr)
{
	ADSPResult result = ADSP_EOK;

	if(HDMI_DRIVER_INIT != hdmi_output_driver.driver_state)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI Output driver not ready to receive command");
		return ADSP_EFAILED;
	}

	hdmi_output_port_state_t *p_dev_state = (hdmi_output_port_state_t *) dev_port_ptr->afe_drv;

	*params_buffer_len_req_ptr = sizeof(afe_param_id_hdmi_multi_chan_audio_cfg_t);
	if(*params_buffer_len_req_ptr > payload_size )
	{
		MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Invalid payload size: HDMI cfg required size = %d, given size = %lu",
				*params_buffer_len_req_ptr, payload_size);
		return ADSP_ENOMEMORY;
	}

	afe_param_id_hdmi_multi_chan_audio_cfg_t *hdmi_cfg_ptr = (afe_param_id_hdmi_multi_chan_audio_cfg_t *)param_buf_ptr;

	/* Populate the payload parameters */
	hdmi_cfg_ptr->hdmi_cfg_minor_version = HDMI_CFG_V1;
	hdmi_cfg_ptr->bit_width = p_dev_state->bit_width;
	hdmi_cfg_ptr->channel_allocation = p_dev_state->channel_alloc;
	hdmi_cfg_ptr->data_type = p_dev_state->data_type;
	hdmi_cfg_ptr->sample_rate = p_dev_state->sample_rate;
	hdmi_cfg_ptr->reserved = 0;

	return result;
}

/**
  @brief Set up the data transfer for HDMI Output port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_driver_open(afe_dev_port_t *dev_port_ptr)
{
	ADSPResult result = ADSP_EOK;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;
	dma_mgr_open_params_t open_params;

	afe_hdmi_output_group_device_state_struct_t* p_group_device = afe_get_hdmi_output_group_device_state_object();

	if(NULL != dev_state_ptr->dma_dev_hdl)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI Output DMA manager handle non NULL when opening port");
		return ADSP_EALREADY;
	}

	//Register this port with the DMA Manager
	open_params.hardware_type = LPASS_HW_HDMI_OUTPUT;
	open_params.dma_type =  LPASS_HW_DMA_TYPE1;
	open_params.dma_dir = LPASS_HW_DMA_SINK; // hdmi output driver will be always sync.
	open_params.sample_rate = dev_port_ptr->sample_rate;
	open_params.input_bytes_per_sample = HAL_LPASS_HDMI_OUTPUT_BYTES_PER_SAMPLES;   //this is for DMA buffer
	open_params.output_bytes_per_sample = dev_port_ptr->bytes_per_channel;
	//For HDMI Output number of channels in DMA buffer is always even
	open_params.num_channels = HDMI_DRIVER_NEXT_EVEN_VALUE(dev_port_ptr->channels);
	// if the number of channel is bigger than 2, the buffer should be allocated for 8 channels data...
	if(2 < open_params.num_channels)
	{
		open_params.num_channels = HDMI_8_CHANNELS;
	}
	open_params.int_samples_per_period = dev_port_ptr->int_samples_per_period;
	open_params.dma_read_func_ptr = NULL;
	open_params.dma_write_func_ptr = (dma_mgr_write_func)afe_hdmi_output_write_dma;
	/* If LPM is exhausted, then allocate DMA buffer in DDR */
	open_params.buff_mem_type = DMA_MGR_LPM_DDR_BUFFER; //HDMI is not a low power use case
	open_params.data_type = dev_state_ptr->data_type;
	open_params.watermark = HAL_LPASS_HDMI_OUTPUT_WATERMARK;
	open_params.num_buffers = HDMI_OUTPUT_NUM_BUFFERS_PING_PONG;  //2

	// dma index will be allowed by dma manager
	open_params.dma_idx = AFE_INVALID_DMA_INDEX;

	// register the interrupt callback
	// dma manger will register interrupt callback
	open_params.dma_int_cb = afe_hdmi_output_int_callback;
	open_params.afe_driver_context = (void *)dev_port_ptr;

	afe_hdmi_output_init_chstatus(dev_state_ptr);

	if(ADSP_EOK != (result = afe_dma_mgr_open(&dev_state_ptr->dma_dev_hdl, &open_params)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to register with DMA Manager with status %d",result);
		goto __bailout;
	}

	dev_state_ptr->dma_idx = afe_dma_mgr_get_dma_index(dev_state_ptr->dma_dev_hdl);

	//Calculate the number DMA interrupts to ignore. This is a function of the DP Engine FIFO depth
	//the sampling rate and the number of channels. During the startup phase the DMA interrupts
	//can fire non-periodically due to the FIFO filling up
	dev_state_ptr->num_interrupts_to_ignore = DP_ENGINE_FIFO_DEPTH_IN_SAMPLES/
	    (dev_port_ptr->int_samples_per_period * open_params.num_channels);

	//No any hw register access in above code
	//ADSPPM call for AHB BW vaote and enable all needed clks --- done at AFE SVC

	//all HW required sequence here
	if(FALSE == p_group_device->is_enabled)
	{
		//single stream case
		if(ADSP_EOK != (result = hdmi_output_driver_open_ss(dev_port_ptr)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start HDMI output for single stream with status %d",result);
			goto __bailout;
		}
	}
	else
	{
		//multiple stream case
		if(ADSP_EOK != (result = hdmi_output_driver_open_ms(dev_port_ptr)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start HDMI output for multiple streams with status %d",result);

			hdmi_output_group_list_full_release();

			//set the group_device to default state
			p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP;
			p_group_device->is_enabled = FALSE;

			goto __bailout;
		}

		//Enable any global control for example interrupt. it is common for all DMAs in MS cases. Just one time only.
		if(!hdmi_output_driver.is_interrupt_enabled)
		{
			hal_lpass_hdmi_output_enable_packet_update_interrupt(1);   //1 - enable, 0 - disable
			hal_lpass_hdmi_output_enable_packet_update_b_interrupt(1);

			hdmi_output_driver.is_interrupt_enabled = TRUE;
		}
	}

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HDMI Output port successfully opened");

	return result;

	__bailout:

	if(dev_state_ptr->dma_dev_hdl)
	{
		afe_dma_mgr_close(dev_state_ptr->dma_dev_hdl);
		dev_state_ptr->dma_dev_hdl = NULL;
	}

	return result;
}


/**
  @brief Stop the data transfer from HDMI Output port

  @param[in] dev_port_ptr pointer for AFE port structure

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hdmi_output_driver_close(afe_dev_port_t *dev_port_ptr)
{
	ADSPResult result = ADSP_EOK;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;
	afe_hdmi_output_group_device_state_struct_t* p_group_device = afe_get_hdmi_output_group_device_state_object();

	if(FALSE == p_group_device->is_enabled)
	{
		//single stream case
		if(ADSP_EOK != (result = hdmi_output_driver_close_ss(dev_port_ptr)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to stop HDMI output for single stream with status %d",result);
		}

	}
	else
	{
		//multi stream case
		if(ADSP_EOK != (result = hdmi_output_driver_close_ms(dev_port_ptr)))
		{
			hdmi_output_group_list_full_release();
			//set the group_device to default state
			p_group_device->group_state = AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP;
			p_group_device->is_enabled = FALSE;

			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to stop HDMI output for multiple stream with status %d",result);
		}
	}

	/*reset device state structure */
	memset(dev_state_ptr, 0, sizeof(hdmi_output_port_state_t));

	//De-register the ISR callback, need to handle four HDMI ports share one interrupt case here
	if(hdmi_output_driver.is_last_operation)
	{
		if(hdmi_output_driver.is_interrupt_enabled)
		{
			//disable global lpass control like interrupt control here
			hal_lpass_hdmi_output_enable_packet_update_interrupt(0);   //1 - enable, 0 - disable
			hal_lpass_hdmi_output_enable_packet_update_b_interrupt(0);

			hdmi_output_driver.is_interrupt_enabled = FALSE;
		}

		hdmi_output_driver.is_last_operation = FALSE;
	}

	MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "HDMI Output port closed with port_id port_id: 0x%lx  status %d",dev_port_ptr->intf, result);

	return result;
}

/**
  @brief Copy data from client buffer to the HDMI Output DMA buffer

  @param[in] dev_port_ptr pointer for AFE port structure
  @param[in] buffer_ptr pointer to the client buffer
  @param[in] num_dma_samples number of samples to read from DMA buffer

  @return  ADSP_EOK on success, an error code on error

 */


ADSPResult afe_hdmi_output_driver_write(afe_dev_port_t *dev_port_ptr, int16_t *buffer_ptr, uint32_t num_dma_samples)
{
	ADSPResult result = ADSP_EOK;
	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	if(ADSP_EOK != (result = afe_dma_mgr_write(dev_state_ptr->dma_dev_hdl, (uint8_t *)buffer_ptr)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to read data from the DMA buffer");
	}

	return result;
}


/**
  @brief Callback routine for the i2s interrupt

  @param[in] afe_drv pointer to the i2s dev port state structure

  @param[in] event_mask indicates if it is dma interrupt or error.

  @return  none

 */
static void afe_hdmi_output_int_callback(void* cb_context, uint32_t event_mask)
{
	afe_dev_port_t *dev_port_ptr = (afe_dev_port_t *)cb_context;
	hdmi_output_port_state_t         *dev_state_ptr = NULL;

	if(NULL == cb_context)
	{
		// critical error
		return;
	}

	dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	if(event_mask & DMA_TYPE0_EVENT_UNDEERRUN)
	{
		dev_state_ptr->undr_cnt++;
	}

	if(event_mask & DMA_TYPE0_EVENT_ERROR)
	{
		dev_state_ptr->err_cnt++;
	}

	if((event_mask & DMA_TYPE0_EVENT_DP_METADATA_DONE) || (event_mask & DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR))
	{
		if((dev_state_ptr->p_ctx) && IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
		{
			//Notify AVS SVC thread UNMUTE/MUTE done
			afe_hdmi_dp_driver_int_notify(dev_port_ptr, event_mask);
		}
	}

	if(event_mask & DMA_TYPE0_EVENT_DATA)
	{
		//period interrupt need to set it to 0 during the open.
		dev_state_ptr->sample_cnt_in_period += 1;

		if (AFE_PORT_STATE_RUN == dev_port_ptr->port_state)
		{
			if(dev_state_ptr->sample_cnt_in_period > dev_state_ptr->num_interrupts_to_ignore)
			{
			  dev_port_ptr->dynamic_thread_ptr->isr_context_counter++;
			  qurt_elite_signal_send(&(dev_port_ptr->dma_signal));
			}
		}
	}
}


/**
  @brief Routine to read the hardware timestamp registers for HDMI Output
  The both time stamp and FIFO are latched by hardware when DMA period interrupt is triggered.

  @param[in] afe_port_ptr pointer to the port structure

  @return none

 */
void afe_hdmi_output_get_dma_timestamp_and_fifo_count(afe_dev_port_t *afe_port_ptr)
{
	uint64_t dma_int_timestamp;
	afe_avt_drift_params_t *avt_drift_params_ptr =
			(afe_avt_drift_params_t *)&afe_port_ptr->avt_drift_params;
	uint32_t fifo_count;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)afe_port_ptr->afe_drv;

	uint16 dma_idx = dev_state_ptr->dma_idx;

	hal_lpass_hdmi_output_get_dma_int_stc(dma_idx, &dma_int_timestamp);

	afe_port_ptr->port_av_timestamp = avtimer_drv_convert_hw_tick_to_time(dma_int_timestamp);

	hal_lpass_hdmi_output_get_dma_fifo(dma_idx, &fifo_count);

	//For HDMI output each sample is 32 bit in the DMA buffer
	//FIFO count is already in terms of number of 32 bit words in FIFO
	avt_drift_params_ptr->fifo_samples = fifo_count;

	//avt_drift_params_ptr->fifo_samples = fifo_count * sizeof(uint32_t)/afe_port_ptr->bytes_per_channel;

	return;
}

/**
  @brief Routine to get the current device time

  @param[in] afe_port_ptr pointer to the port structure

  @return none

  Currently this function is not used. 

 */
ADSPResult afe_get_time_for_hdmi_out_port(afe_dev_port_t *afe_port_ptr, uint64 *curr_time)
{
	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)afe_port_ptr->afe_drv;

	uint32 time_per_sample = 0;
	int64 num_samples = 0;
	int64 temp;
	int32 format = 0;
	uint32_t sample_in_dwrod;

	hal_lpass_hdmi_output_get_samples_in_dword_after_intr(dev_state_ptr->dma_idx, &sample_in_dwrod);

	//for HDMI case, 1 channel is always 1 DWs
	num_samples = afe_port_ptr->int_samples_per_period + sample_in_dwrod / afe_port_ptr->channels;

	//get time per sample.
	afe_device_gettime_per_sample(afe_port_ptr->sample_rate, &time_per_sample,&format);

	//caculate the time for all samples that has been transmitted.
	temp = num_samples*time_per_sample;

	//conver it to Q25 format.
	*curr_time = (int64)((temp + (((int64)0x01)<<(format-1))) >> format);

	return ADSP_EOK;
}



/**
  Copy data from client buffer to the HDMI Output DMA buffer

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information
  @param[in]  src_address           pointer to the client buffer
  @param[in]  dst_address           pointer to the DMA buffer

  @return
  None

  @dependencies
  None.
 */

void afe_hdmi_output_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address)
{
	afe_dev_port_t *dev_port_ptr =  (afe_dev_port_t *)afe_driver_context;

	if(NULL == afe_driver_context)
	{
		// critical error
		return;
	}

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *) dev_port_ptr->afe_drv;

	uint32_t num_dma_samples = dev_port_ptr->int_samples_per_period;

	afe_hdmi_output_pack_info_t hdmi_pack_info;

	hdmi_pack_info.in_sample_bit_width = dev_port_ptr->bit_width;
	hdmi_pack_info.num_channels = dev_port_ptr->channels;
	hdmi_pack_info.data_type = dev_state_ptr->data_type;
	hdmi_pack_info.ca = dev_state_ptr->channel_alloc;
	hdmi_pack_info.ch_status_table_ptr = NULL;
	hdmi_pack_info.hdmi_ch_map_ptr = &dev_state_ptr->hdmi_ch_map[0];
	hdmi_pack_info.q_format_shift_factor = dev_port_ptr->q_format_shift_factor;

	if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf))
	{
		if(2 == dev_port_ptr->channels)
		{
			afe_hdmi_output_fill_data_2ch(&hdmi_pack_info, (int32*)dst_address, (int16*) src_address, num_dma_samples, 0);
		}
		else
		{
			// 4 or 6 or 8 channels
			afe_hdmi_output_fill_data_8ch(&hdmi_pack_info, (int32*)dst_address, (int16*) src_address, num_dma_samples, 0);
		}
	}
	else
	{
		//Audio over DisplayPort case
		if(2 == dev_port_ptr->channels)
		{
			afe_hdmi_output_fill_data_2ch_dp(&hdmi_pack_info, (int32*)dst_address, (int16*) src_address, num_dma_samples, 0);
		}
		else
		{
			// 4 or 6 or 8 channels
			afe_hdmi_output_fill_data_8ch_dp(&hdmi_pack_info, (int32*)dst_address, (int16*) src_address, num_dma_samples, 0);
		}
	}

	return;
}

ADSPResult hdmi_output_driver_open_ss(afe_dev_port_t *dev_port_ptr)    //for single stream case
{
	ADSPResult result = ADSP_EOK;
	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;
	uint16 vbit = 0;
	hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg;
	hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg;
	hal_lpass_hdmi_output_ss_pack_trans_cfg_t ss_cfg;

	uint32 present_bits = 0;

	//Check DP device current state
	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		if(HDMI_DP_DEVICE_STATE_STOP != dev_state_ptr->dp_device_state)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED hdmi_output_driver_open_ss with DP device state %d",dev_state_ptr->dp_device_state);
			dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_STOP;
			return ADSP_EFAILED;
		}
	}

	//Belows are NOT needed for Audio over DP
	if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf))
	{
		hal_mdss_hdmi_cfg_audio_mode(MODE_1);   //single stream via new hw
		hal_mdss_hdmi_disable_core();
		hal_mdss_hdmi_reset_sample_present_bit();
		hal_mdss_hdmi_disable_hw_pack();
	}

	hal_lpass_hdmi_output_enable_legacy_mode(0);

	hal_lpass_hdmi_output_enable_parity_hw_mode(1);   //0 - diable, 1 - enable

	//vbit = 1 for compressed mode and 0 for linear PCM
	if(AFE_LINEAR_PCM_DATA == dev_state_ptr->data_type)
	{
		vbit = 0;
	}
	else
	{
		vbit = 1;
	}

	hal_lpass_hdmi_output_enable_vbit_hw_mode(HAL_LPASS_HDMI_OUTPUT_SINGLE_STREAM_MODE, dev_state_ptr->dma_idx, vbit);

	// Config lpass HDMI 60958 CS/USE DATA pack
	// fill channel status buf 0 with the info for this stream
	chs_cfg.dma_idx = dev_state_ptr->dma_idx;
	chs_cfg.chs_buf_idx = dev_state_ptr->ch_sts_buf_0.buf_idx;
	chs_cfg.hw_mode = 1;
	chs_cfg.ch_sts_data0 = dev_state_ptr->ch_sts_buf_0.ch_sts_data0;
	chs_cfg.ch_sts_data1 = dev_state_ptr->ch_sts_buf_0.ch_sts_data1;

	hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);

	// fill user buf 0 with the info for this stream
	// Not support now.
	ubs_cfg.dma_idx = dev_state_ptr->dma_idx;
	ubs_cfg.user_buf_idx = 0;
	ubs_cfg.hw_mode = 1;
	hal_lpass_hdmi_output_cfg_user_bits_buf(ubs_cfg);

	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg;

		audioinfo_cfg.dma_idx = dev_state_ptr->dma_idx;

		result = afe_hdmi_dp_driver_get_freq_idx(dev_state_ptr->sample_rate, &audioinfo_cfg.freq_idx);
		if(ADSP_EOK != result)
		{
		  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Illegal sampling rate value: %lu",
		        dev_state_ptr->sample_rate);
		  return result;
		}
		audioinfo_cfg.cc_idx = afe_hdmi_dp_driver_get_cc_idx(dev_state_ptr->num_channels);
		audioinfo_cfg.ct_idx = 0;   //not support
		audioinfo_cfg.ca_idx = dev_state_ptr->channel_alloc;

		//Configure CC, CT, CA, and SF
		hal_lpass_hdmi_output_dp_cfg_audio_info(audioinfo_cfg);

		//Enable DP engine
		hal_lpass_hdmi_output_dp_engine_enable(1);
		//LPASS_HDMITX_SSTREAM_CTRL.DP_ENABLE needs to be set.
	}

	//Configure/Start LPASS DMA

	//Clear ISR for this DMA then Enable ISR for this DMA - move to DMA manager
	//hal_lpass_hdmi_output_enable_dma_interrupt(dev_state_ptr->dma_idx, 1);
	if(ADSP_EOK != (result = afe_dma_mgr_start(dev_state_ptr->dma_dev_hdl)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d",result);
		return result;
	}

	if(0 != dev_state_ptr->channel_alloc)
	{
		present_bits = afe_hdmi_output_get_present_bits(dev_state_ptr->channel_alloc);
	}

	ss_cfg.sp_en = 1;        //do not care
	ss_cfg.force_mute = 0;   //do not care
	ss_cfg.sp_bits_layout1 = present_bits;
	ss_cfg.dma_idx = dev_state_ptr->dma_idx;
	hal_lpass_hdmi_output_ss_cfg_pack_trans(ss_cfg);

	//Enable lpass HDMI Tx pack trans
	hal_lpass_hdmi_output_ss_enable_pack_trans(1);   //1 - enable, 0 - disable

	if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf))
	{
	  //Enable HDMI core audio info frame trans
	  hal_mdss_hdmi_start_audio_info_frame();

	  /* Enable High bit rate for compressed pass-
	   * through with channels greater than 2. */
	  if( (AFE_NON_LINEAR_DATA == dev_state_ptr->data_type) && (2 < dev_state_ptr->num_channels) )
	  {
	    /* Configures HBR en bit */
	    hal_mdss_hdmi_enable_disable_hbr(ENABLE);
	  }

	  hal_mdss_hdmi_enable_core();
	  hal_mdss_hdmi_enable_packet_ctl();
	}

	//Set DP device current state
	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_START;
	}

	return result;
}

ADSPResult hdmi_output_driver_open_ms(afe_dev_port_t *dev_port_ptr)    //for multiple stream case
{
	ADSPResult result = ADSP_EOK;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	afe_hdmi_output_group_device_state_struct_t* p_group_device = afe_get_hdmi_output_group_device_state_object();

	//group case
	if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_START == p_group_device->group_state)
	{
		//fresh group start case

		//get the stream ID from p_group_device->start_list
		dev_state_ptr->stream_id = hdmi_output_group_list_get_stream_id(dev_port_ptr->intf, &p_group_device->start_list);

		//query start_list
		if(hdmi_output_group_list_is_first_item_query(&p_group_device->start_list, NULL))
		{
			hal_mdss_hdmi_cfg_audio_mode(MODE_2);   //multiple stream via new hw

			hal_mdss_hdmi_disable_core();

			hal_mdss_hdmi_reset_sample_present_bit();

			// Disable HW packetizing
			hal_mdss_hdmi_disable_hw_pack();

			//lpass HDMI hw cfg
			//HDMITX_LEGACY. LEGACY_EN = 1   --> use LPA_IF DMA
			//                         = 0   --> use new LPASS HDMITx hw
			hal_lpass_hdmi_output_enable_legacy_mode(0);
		}
		else
		{
			//do nothing
		}

		hdmi_output_group_list_valid_update(dev_port_ptr->intf, &p_group_device->start_list);

		//Configure/Start LPASS DMA
		if(ADSP_EOK != (result = hdmi_output_driver_open_ms_sub(dev_state_ptr)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in hdmi_output_driver_open_ms_sub with status %d",result);

			return result;
		}

		if(hdmi_output_group_list_is_last_item_query(&p_group_device->start_list, NULL))
		{
			hdmi_output_group_list_stream_route_update_all(&p_group_device->start_list);

			//Enable lpass HDMI Tx pack trans
			hal_lpass_hdmi_output_ms_enable_pack_trans(1);   //1 - enable, 0 - disable

			//Enable HDMI core audio info frame trans
			hal_mdss_hdmi_start_audio_info_frame();

			hal_mdss_hdmi_enable_core();
			hal_mdss_hdmi_enable_packet_ctl();

			//start hdmi core metadata update
			hal_mdss_hdmi_start_audio_metadata();
		}
		else
		{
			//do nothing
		}
	}
	else if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_UPDATE == p_group_device->group_state)
	{
		//get the stream ID from p_group_device->add_list
		dev_state_ptr->stream_id = hdmi_output_group_list_get_stream_id(dev_port_ptr->intf, &p_group_device->add_list);

		//sub-stream add case
		if(hdmi_output_group_list_is_first_item_query(&p_group_device->add_list, &p_group_device->remove_list))
		{
			//do sanity check against the merged start_list
			if(ADSP_EOK != (result = hdmi_output_group_cfg_update_validation()))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in hdmi_output_group_cfg_update_validation with status %d",result);
				return result;
			}
		}
		else
		{
			//do nothing

		}

		//Configure/Start LPASS DMA
		if(ADSP_EOK != (result = hdmi_output_driver_open_ms_sub(dev_state_ptr)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in hdmi_output_driver_open_ms_sub with status %d",result);

			return result;
		}

		hdmi_output_group_list_valid_update(dev_port_ptr->intf, &p_group_device->add_list);

		if(hdmi_output_group_list_is_last_item_query(&p_group_device->add_list, &p_group_device->remove_list))
		{
			//merge add_list and remove_list to start_list
			hdmi_output_group_list_merge(&p_group_device->start_list);

			hdmi_output_group_list_stream_route_update_all(&p_group_device->start_list);

			hal_mdss_hdmi_update_audio_metadata();

			//reset add_list and fully destroy remove_list
			memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
			hdmi_output_group_list_node_destroy(&p_group_device->remove_list);
		}
		else
		{
			//do nothing
		}
	}
	else
	{
		//error
	}

	return result;
}

ADSPResult hdmi_output_driver_open_ms_sub(hdmi_output_port_state_t *dev_state_ptr)    //for multiple stream case
{
	ADSPResult result = ADSP_EOK;

	hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg;
	hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg;
	uint16 vbit = 0;

	//vbit = 1 for compressed mode and 0 for linear PCM
	if(AFE_LINEAR_PCM_DATA == dev_state_ptr->data_type)
	{
		vbit = 0;
	}
	else
	{
		vbit = 1;
	}

	hal_lpass_hdmi_output_enable_vbit_hw_mode(HAL_LPASS_HDMI_OUTPUT_MULTIPLE_STREAM_MODE, dev_state_ptr->dma_idx, vbit);

	hal_lpass_hdmi_output_enable_parity_hw_mode(1);   //0 - diable, 1 - enable, shared for all streams

	// Config lpass HDMI 60958 CS/USE DATA pack
	// fill channel status buf 0 with the info for this stream
	chs_cfg.dma_idx = dev_state_ptr->dma_idx;
	chs_cfg.chs_buf_idx = dev_state_ptr->ch_sts_buf_0.buf_idx;
	chs_cfg.hw_mode = 1;
	chs_cfg.ch_sts_data0 = dev_state_ptr->ch_sts_buf_0.ch_sts_data0;
	chs_cfg.ch_sts_data1 = dev_state_ptr->ch_sts_buf_0.ch_sts_data1;
	hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);

	// fill user buf 0 with the info for this stream
	// user buffer not support now
	ubs_cfg.dma_idx = dev_state_ptr->dma_idx;
	ubs_cfg.user_buf_idx = 0;  //TBD get one unused user buffer
	ubs_cfg.hw_mode = 1;
	hal_lpass_hdmi_output_cfg_user_bits_buf(ubs_cfg);

	//unmute this stream
	hal_lpass_hdmi_output_ms_mute_stream(dev_state_ptr->stream_id, 0);   //1 - mute, 0 - unmute

	//Clear ISR for this DMA then Enable ISR for this DMA - move to DMA manager
	//hal_lpass_hdmi_output_enable_dma_interrupt(dev_state_ptr->dma_idx, 1);

	//Configure/Start LPASS DMA
	if(ADSP_EOK != (result = afe_dma_mgr_start(dev_state_ptr->dma_dev_hdl)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start DMA engine with status %d",result);

		return result;
	}

	return result;
}

static ADSPResult hdmi_output_driver_close_ss(afe_dev_port_t *dev_port_ptr)    //for single stream case
{
	ADSPResult result = ADSP_EOK;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg;
	hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg;

	//Check DP device current state
	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		if(HDMI_DP_DEVICE_STATE_MUTE_DONE != dev_state_ptr->dp_device_state)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED hdmi_output_driver_close_ss with DP device state %d",dev_state_ptr->dp_device_state);

			//let's continue
		}
	}

	//Need to follow below sequence
	//HDMITX_RDDMA_CTL1.ENABLE = 0
	//HDMITX_DMA1_CTRL.USE_HW_USER = 0
	//HDMITX_DMA1_CTRL.USE_HW_CHS = 0

	//stop the DMA engine
	afe_dma_mgr_stop(dev_state_ptr->dma_dev_hdl);

	//De-register with the DMA Manager
	afe_dma_mgr_close(dev_state_ptr->dma_dev_hdl);
	dev_state_ptr->dma_dev_hdl = NULL;

	//disbale interrupt for this DMA
	//hal_lpass_hdmi_output_enable_dma_interrupt(dev_state_ptr->dma_idx, 0);

	//Reset lpass HDMI 60958 CS/USE DATA packet
	chs_cfg.dma_idx = dev_state_ptr->dma_idx;
	chs_cfg.chs_buf_idx = dev_state_ptr->ch_sts_buf_0.buf_idx;
	chs_cfg.hw_mode = 0;
	chs_cfg.ch_sts_data0 = dev_state_ptr->ch_sts_buf_0.ch_sts_data0;
	chs_cfg.ch_sts_data1 = dev_state_ptr->ch_sts_buf_0.ch_sts_data1;

	hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);

	// fill user buf 0 with the info for this stream
	// Not support now.
	ubs_cfg.dma_idx = dev_state_ptr->dma_idx;
	ubs_cfg.user_buf_idx = 0;
	ubs_cfg.hw_mode = 0;
	hal_lpass_hdmi_output_cfg_user_bits_buf(ubs_cfg);

	//Reset V bits
	hal_lpass_hdmi_output_disable_vbit_hw_mode(HAL_LPASS_HDMI_OUTPUT_SINGLE_STREAM_MODE, dev_state_ptr->dma_idx);

	if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf))
	{
		//stop audio packet transmission
		hal_mdss_hdmi_disable_packet_ctl();
		// Disable HDMI Audio engine
		hal_mdss_hdmi_disable_core();
		// Disable HDMI core audio info frame trans
		hal_mdss_hdmi_stop_audio_info_frame();

		/* Disable HBR */
		if( (AFE_NON_LINEAR_DATA == dev_state_ptr->data_type) && (2 < dev_state_ptr->num_channels) )
		{
		  hal_mdss_hdmi_enable_disable_hbr(DISABLE);
		}
	}

	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		hal_lpass_hdmi_output_dp_audioinfo_cfg_t audioinfo_cfg;

		audioinfo_cfg.dma_idx = dev_state_ptr->dma_idx;

		audioinfo_cfg.freq_idx = 0;
		audioinfo_cfg.cc_idx = 0;
		audioinfo_cfg.ct_idx = 0;   //not support
		audioinfo_cfg.ca_idx = 0;

		//Reset CC, CT, CA, and SF
		hal_lpass_hdmi_output_dp_cfg_audio_info(audioinfo_cfg);

		//Disable DP engine
		hal_lpass_hdmi_output_dp_engine_enable(0);
		//LPASS_HDMITX_SSTREAM_CTRL.DP_ENABLE needs to be set.to 0
	}

	//FENG TBD do we rellay need below for audio over DP?
	// disable lpass HDMI Tx pack trans
	hal_lpass_hdmi_output_ss_enable_pack_trans(0);   //1 - enable, 0 - disable

	//Reset for single stream with LPAIF legacy DMA case
	hal_lpass_hdmi_output_enable_legacy_mode(1);   //0 - diable, 1 - enable

	if(IS_HDMI_OUTPUT_PORT_AFE_ID(dev_port_ptr->intf))
	{
		hal_mdss_hdmi_cfg_audio_mode(MODE_0);   //single stream via legacy hw, lpass uses LPAIF DMA
	}

	hdmi_output_driver.is_last_operation = TRUE;

	//Check DP device current state
	if(IS_HDMI_OVER_DP_PORT_AFE_ID(dev_port_ptr->intf))
	{
		dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_STOP;
	}

	return result;
}

static ADSPResult hdmi_output_driver_close_ms(afe_dev_port_t *dev_port_ptr)    //for multiple stream case
{
	ADSPResult result = ADSP_EOK;

	hal_lpass_hdmi_output_ch_status_cfg_t chs_cfg;
	hal_lpass_hdmi_output_user_bits_cfg_t ubs_cfg;

	hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	afe_hdmi_output_group_device_state_struct_t* p_group_device = afe_get_hdmi_output_group_device_state_object();

	hal_lpass_hdmi_output_ms_mute_stream(dev_state_ptr->stream_id, 1);   //1 - mute, 0 - unmute

	//Need to follow below sequence for DMA and CHS/USER
	//HDMITX_RDDMA_CTL1.ENABLE = 0
	//HDMITX_DMA1_CTRL.USE_HW_USER = 0
	//HDMITX_DMA1_CTRL.USE_HW_CHS = 0

	//stop the DMA engine, HDMITX_RDDMA_CTLx.ENABLE = 0
	afe_dma_mgr_stop(dev_state_ptr->dma_dev_hdl);

	//De-register with the DMA Manager
	afe_dma_mgr_close(dev_state_ptr->dma_dev_hdl);
	dev_state_ptr->dma_dev_hdl = NULL;

	//disbale interrupt for this DMA
	//hal_lpass_hdmi_output_enable_dma_interrupt(dev_state_ptr->dma_idx, 0);

	//Reset lpass HDMI 60958 CS/USE DATA packet
	//HDMITX_DMA1_CTRL.USE_HW_CHS = 0
	chs_cfg.dma_idx = dev_state_ptr->dma_idx;
	chs_cfg.chs_buf_idx = dev_state_ptr->ch_sts_buf_0.buf_idx;
	chs_cfg.hw_mode = 0;
	chs_cfg.ch_sts_data0 = dev_state_ptr->ch_sts_buf_0.ch_sts_data0;
	chs_cfg.ch_sts_data1 = dev_state_ptr->ch_sts_buf_0.ch_sts_data1;

	hal_lpass_hdmi_output_cfg_ch_status_buf(chs_cfg);

	// fill user buf 0 with the info for this stream
	// Not support now.
	//HDMITX_DMAx_CTRL.USE_HW_USER = 0
	ubs_cfg.dma_idx = dev_state_ptr->dma_idx;
	ubs_cfg.user_buf_idx = 0;
	ubs_cfg.hw_mode = 0;
	hal_lpass_hdmi_output_cfg_user_bits_buf(ubs_cfg);

	//Reset V bits
	hal_lpass_hdmi_output_disable_vbit_hw_mode(HAL_LPASS_HDMI_OUTPUT_MULTIPLE_STREAM_MODE, dev_state_ptr->dma_idx);

	//group case
	if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_STOP == p_group_device->group_state)
	{
		//full tear down
		hdmi_output_group_list_valid_update(dev_port_ptr->intf, &p_group_device->stop_list);

		//Is the last STOP in group?
		//check group_start_pending_cfg[] table to see if this the last item left
		if(hdmi_output_group_list_is_last_item_query(&p_group_device->stop_list, NULL))
		{
			//stop audio packet transmission
			hal_mdss_hdmi_disable_packet_ctl();

			// Disable HDMI Audio engine
			hal_mdss_hdmi_disable_core();

			// Disable HDMI core audio info frame trans
			hal_mdss_hdmi_stop_audio_info_frame();

			//disable hdmi core meta data
			hal_mdss_hdmi_stop_audio_metadata();

			// disable lpass HDMI Tx pack trans
			hal_lpass_hdmi_output_ms_enable_pack_trans(0);   //1 - enable, 0 - disable

			//Reset for single stream with LPAIF legacy DMA case
			hal_lpass_hdmi_output_enable_legacy_mode(1);   //0 - diable, 1 - enable

			hal_mdss_hdmi_cfg_audio_mode(MODE_0);   //single stream via legacy hw, lpass uses LPAIF DMA

			hdmi_output_group_list_node_destroy(&p_group_device->start_list);  //start_list is empty already
			hdmi_output_group_list_node_destroy(&p_group_device->stop_list);

			//reset all lists
			memset(&p_group_device->start_list, 0, sizeof(hdmi_out_group_list_t));
			memset(&p_group_device->stop_list, 0, sizeof(hdmi_out_group_list_t));
			memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
			memset(&p_group_device->remove_list, 0, sizeof(hdmi_out_group_list_t));

			hdmi_output_driver.is_last_operation = TRUE;
		}
		else
		{

		}

	}
	else if(AFE_HDMI_OUTPUT_GROUP_DEVICE_STATE_UPDATE == p_group_device->group_state)
	{
		//remove sub streams

		if(hdmi_output_group_list_is_first_item_query(&p_group_device->add_list, &p_group_device->remove_list))
		{
			//do sanity check first for add/remove case
			if(ADSP_EOK != (result = hdmi_output_group_cfg_update_validation()))
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in hdmi_output_group_cfg_update_validation with status %d",result);
				return result;
			}
		}
		else
		{
			//do nothing
		}

		hdmi_output_group_list_valid_update(dev_port_ptr->intf, &p_group_device->remove_list);

		if(hdmi_output_group_list_is_last_item_query(&p_group_device->add_list, &p_group_device->remove_list))
		{
			//check any DMA route required according to current group_start_pending_cfg and group_stop_excuted_cfg tables
			hdmi_output_group_list_merge(&p_group_device->start_list);

			hdmi_output_group_list_stream_route_update_all(&p_group_device->start_list);

			hal_mdss_hdmi_update_audio_metadata();

			//reset add_list and fully destroy remove_list
			memset(&p_group_device->add_list, 0, sizeof(hdmi_out_group_list_t));
			hdmi_output_group_list_node_destroy(&p_group_device->remove_list);
		}
		else
		{
			//do nothing
		}
	}
	else
	{
		//error
	}

	return result;
}

ADSPResult afe_hdmi_output_stream_route_update(uint16_t port_id, uint16_t stream_id)
{
	ADSPResult result = ADSP_EOK;
	afe_dev_port *dev_port_ptr = NULL;
	hdmi_output_port_state_t *port_state_ptr = NULL;
	int idx = 0;

	idx = (port_id - AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_START)/2;   //idx = 0, 1, 2, 3

	if((0 > idx) || (((AFE_PORT_ID_HDMI_MULTISTREAM_RANGE_SIZE/2)-1) < idx))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_hdmi_output_stream_route_update unsupported port id 0x%x", port_id);
		return ADSP_EBADPARAM;
	}

	dev_port_ptr = &hdmi_output_port[idx];
	port_state_ptr = &hdmi_output_port_state[idx];

	if(dev_port_ptr && port_state_ptr)
	{
		hal_lpass_hdmi_output_ms_pack_trans_cfg_t ms_cfg;

		port_state_ptr->stream_id = stream_id;

		ms_cfg.dma_idx = port_state_ptr->dma_idx;
		ms_cfg.stream_id = port_state_ptr->stream_id;

		hal_lpass_hdmi_output_ms_cfg_pack_trans(ms_cfg);

#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)
		hdmi_output_driver.cur_stream_id_array[hdmi_output_driver.cur_used_idx] = port_state_ptr->stream_id;
		hdmi_output_driver.cur_dma_idx_array[hdmi_output_driver.cur_used_idx] = port_state_ptr->dma_idx;
		hdmi_output_driver.cur_used_idx++;
#endif //#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

	}
	else
	{
		result = ADSP_EFAILED;
	}

	return result;
}



int32 afe_hdmi_output_fill_data_2ch(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame)
{
	int32  i = 0;
	int32  numch;
	uint32 packed_data;

#if defined(CH_STATUS_SW)
	uint32 packed_data1;
#endif

	uint32 align_shift;
	uint32 data_mask;
	hdmi_data_shift_type data_shift_dir = HDMI_DATA_SHIFT_LEFT;

	int32_t *src32 = (int32_t *)src;

	//always 16bits for non-linear use case.
	switch (pack_info_ptr->in_sample_bit_width)
	{
	case 20:  //data from AFE SVC is left justified (bit 31:12 valid)
		// msb aligned so that it should be same as 24bit case.
		align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
		data_mask    = 0xFFFFF000;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 24:  //data from AFE SVC is left justified (bit 31:8 valid)
	case 32:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
		data_mask   = 0xFFFFFF00;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 16:
	default:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 + HDMI_IEC60958_AUX_START_BIT4;
		data_mask   = 0x0000FFFF;
		break;
	}

	//numChannels for non-linear case is always 2.
	while (i < num_samples_to_copy * pack_info_ptr->num_channels)  //numSampleToCopy = number of sample for one channel
	{
		numch = pack_info_ptr->num_channels;
		i += numch;
		while (numch--)
		{
			if(16 < pack_info_ptr->in_sample_bit_width)
			{
				//for 24 bit case
				*src32 = (*src32) << pack_info_ptr->q_format_shift_factor;  //q_format_shift_factor = QFORMAT_SHIFT_FACTOR = 4
				packed_data = (uint32)(*src32++) & data_mask;
			}
			else
			{
				packed_data = (uint32)(*src++) & data_mask;
			}

			//packedData = (pack_info_ptr->in_sample_bit_width <= 16) ? (uint32)(*src++) & dataMask :(uint32)(*src32++) & dataMask;

			if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
			{
				packed_data >>= align_shift;
			}
			else
			{
				packed_data <<= align_shift;
			}

#if defined(CH_STATUS_SW)
			//Channel status is identical for both subframes of the interface.
			packed_data |= pack_info_ptr->ch_status_table_ptr[cur_frame];

			if(AFE_NON_LINEAR_DATA == pack_info_ptr->data_type) //TODO: it will be used for I2S HDMI input
			{
				packed_data |= 1UL << HDMI_IEC60958_V_BIT28;  //set validity flag to 1 for non-linear PCM only
				// 1 if is not reliable.
				//NOTE For transmissions not using a linear PCM coding, this bit may be set.
				// This is intended to prevent accidental decoding of non-audio data to analogue before a cmplete channel status block is received
			}

			// calculate parity (optimized version)
			// parity bit carries from bit 4 to 31 inclusive, carry an even number of ones and an even number of zeros (even parity)
			packed_data1  = packed_data & 0xFFFFFFF0;
			packed_data1 ^= packed_data1 >> 1;
			packed_data1 ^= packed_data1 >> 2;
			packed_data1  = (packed_data1 & 0x11111111U) * 0x11111111U;

			// fill P bit, Parity value at bit28
			*dst++ = packed_data | ((packed_data1 << 3) & 0x80000000);
#else 
			*dst++ = packed_data;

#endif //#if defined(CH_STATUS_SW)
		}

		// HDMI channel status cycle every 192 frames
		cur_frame++;
		if (HDMI_BLOCK_SIZE == cur_frame)
		{
			cur_frame = 0;
		}
	}

	return cur_frame;
}

int32 afe_hdmi_output_fill_data_8ch(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame)
{
	int32 i = 0;  //for dst
	//int32 j = 0;  //for internal use
	int32 k = 0;  //for src
	int32 map_loop = 0;
	int32 in_sample_bit_width = pack_info_ptr->in_sample_bit_width;
	int32* src32 = (int32*)src;

	while (i < num_samples_to_copy * HDMI_8_CHANNELS)
	{
		//The data stream from upper layer are all valid
		//No NULL channel info in bit stream data from upper layer
		//Take ca = 0x9 as example here
		//
		//Stream channel mapping or media format passed to AFE DAL is
		// {FL,
		//  FR,
		//  LFE,
		//  RL,
		//  RR}
		//
		//The stream data passed to AFE DAL is
		// {FL,
		//  FR,
		//..LFE,
		//..RL,
		//..RR}
		//
		//The data in DMA buffer (out of AFE DAL) is
		// {FL,
		//  FR,
		//  LFE,
		//  NULL,
		//  RL,
		//  RR,
		//  NULL,
		//  NULL}
		//
		//The dst data pointer is always moved in 8 DWs or 8 channel based
		//The src data pointer is moved based on the ch # of input data stream

		//reset the next dst 8 DWs to zero
		//AfeOsalMemset( (void*)&dst[i], 0, HDMI_8_CHANNELS * sizeof(int32) );

		for(map_loop = 0; map_loop < pack_info_ptr->hdmi_ch_map_ptr[HDMI_8_CHANNELS]; map_loop++)
		{
			if(16 == in_sample_bit_width)
			{
				afe_hdmi_output_pack_data(pack_info_ptr, &src[k+map_loop], &dst[i+pack_info_ptr->hdmi_ch_map_ptr[map_loop]], cur_frame, 1);
			}
			else
			{
				afe_hdmi_output_pack_data(pack_info_ptr, (int16 *)&src32[k+map_loop], &dst[i+pack_info_ptr->hdmi_ch_map_ptr[map_loop]], cur_frame, 1);
			}
		}

		i += 8;
		k += pack_info_ptr->hdmi_ch_map_ptr[HDMI_8_CHANNELS];  //source does not include NULL channel data

		cur_frame++;
		if (HDMI_BLOCK_SIZE == cur_frame)
		{
			cur_frame = 0;
		}
	}

	return cur_frame; // the number of blocks we filled
}

int32 afe_hdmi_output_fill_data_2ch_dp(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame)
{
	int32  i = 0;
	int32  numch;
	uint32 packed_data;

#if defined(CH_STATUS_SW)
	uint32 packed_data1;
#endif

	uint32 align_shift;
	uint32 data_mask;
	hdmi_data_shift_type data_shift_dir = HDMI_DATA_SHIFT_LEFT;

	int32_t *src32 = (int32_t *)src;

	//always 16bits for non-linear use case.
	switch (pack_info_ptr->in_sample_bit_width)
	{
	case 20:  //data from AFE SVC is left justified (bit 31:12 valid)
		// msb aligned so that it should be same as 24bit case.
		align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask    = 0xFFFFF000;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 24:  //data from AFE SVC is left justified (bit 31:8 valid)
	case 32:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask   = 0xFFFFFF00;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 16:
	default:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask   = 0x0000FFFF;
		break;
	}

	//numChannels for non-linear case is always 2.
	while (i < num_samples_to_copy * pack_info_ptr->num_channels)  //numSampleToCopy = number of sample for one channel
	{
		numch = pack_info_ptr->num_channels;
		i += numch;
		while (numch--)
		{
			if(16 < pack_info_ptr->in_sample_bit_width)
			{
				//for 24 bit case
				*src32 = (*src32) << pack_info_ptr->q_format_shift_factor;  //q_format_shift_factor = QFORMAT_SHIFT_FACTOR = 4
				packed_data = (uint32)(*src32++) & data_mask;
			}
			else
			{
				packed_data = (uint32)(*src++) & data_mask;
			}

			//packedData = (pack_info_ptr->in_sample_bit_width <= 16) ? (uint32)(*src++) & dataMask :(uint32)(*src32++) & dataMask;

			if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
			{
				packed_data >>= align_shift;
			}
			else
			{
				packed_data <<= align_shift;
			}

#if defined(CH_STATUS_SW)
			//Channel status is identical for both subframes of the interface.
			packed_data |= pack_info_ptr->ch_status_table_ptr[cur_frame];

			if(AFE_NON_LINEAR_DATA == pack_info_ptr->data_type) //TODO: it will be used for I2S HDMI input
			{
				packed_data |= 1UL << HDMI_IEC60958_V_BIT28;  //set validity flag to 1 for non-linear PCM only
				// 1 if is not reliable.
				//NOTE For transmissions not using a linear PCM coding, this bit may be set.
				// This is intended to prevent accidental decoding of non-audio data to analogue before a cmplete channel status block is received
			}

			// calculate parity (optimized version)
			// parity bit carries from bit 4 to 31 inclusive, carry an even number of ones and an even number of zeros (even parity)
			packed_data1  = packed_data & 0xFFFFFFF0;
			packed_data1 ^= packed_data1 >> 1;
			packed_data1 ^= packed_data1 >> 2;
			packed_data1  = (packed_data1 & 0x11111111U) * 0x11111111U;

			// fill P bit, Parity value at bit28
			*dst++ = packed_data | ((packed_data1 << 3) & 0x80000000);
#else 
			*dst++ = packed_data;

#endif //#if defined(CH_STATUS_SW)
		}

		// HDMI channel status cycle every 192 frames
		cur_frame++;
		if (HDMI_BLOCK_SIZE == cur_frame)
		{
			cur_frame = 0;
		}
	}

	return cur_frame;
}

int32 afe_hdmi_output_fill_data_8ch_dp(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame)
{
	int32 i = 0;  //for dst
	//int32 j = 0;  //for internal use
	int32 k = 0;  //for src
	int32 map_loop = 0;
	int32 in_sample_bit_width = pack_info_ptr->in_sample_bit_width;
	int32* src32 = (int32*)src;

	while (i < num_samples_to_copy * HDMI_8_CHANNELS)
	{
		//The data stream from upper layer are all valid
		//No NULL channel info in bit stream data from upper layer
		//Take ca = 0x9 as example here
		//
		//Stream channel mapping or media format passed to AFE DAL is
		// {FL,
		//  FR,
		//  LFE,
		//  RL,
		//  RR}
		//
		//The stream data passed to AFE DAL is
		// {FL,
		//  FR,
		//..LFE,
		//..RL,
		//..RR}
		//
		//The data in DMA buffer (out of AFE DAL) is
		// {FL,
		//  FR,
		//  LFE,
		//  NULL,
		//  RL,
		//  RR,
		//  NULL,
		//  NULL}
		//
		//The dst data pointer is always moved in 8 DWs or 8 channel based
		//The src data pointer is moved based on the ch # of input data stream

		//reset the next dst 8 DWs to zero
		//AfeOsalMemset( (void*)&dst[i], 0, HDMI_8_CHANNELS * sizeof(int32) );

		for(map_loop = 0; map_loop < pack_info_ptr->hdmi_ch_map_ptr[HDMI_8_CHANNELS]; map_loop++)
		{
			if(16 == in_sample_bit_width)
			{
				afe_hdmi_output_pack_data_dp(pack_info_ptr, &src[k+map_loop], &dst[i+pack_info_ptr->hdmi_ch_map_ptr[map_loop]], cur_frame, 1);
			}
			else
			{
				afe_hdmi_output_pack_data_dp(pack_info_ptr, (int16 *)&src32[k+map_loop], &dst[i+pack_info_ptr->hdmi_ch_map_ptr[map_loop]], cur_frame, 1);
			}
		}

		i += 8;
		k += pack_info_ptr->hdmi_ch_map_ptr[HDMI_8_CHANNELS];  //source does not include NULL channel data

		cur_frame++;
		if (HDMI_BLOCK_SIZE == cur_frame)
		{
			cur_frame = 0;
		}
	}

	return cur_frame; // the number of blocks we filled
}


// P | C | U | V | DATA20 | AUX4 | PREAMBLE4
void afe_hdmi_output_pack_data_dp(afe_hdmi_output_pack_info_t *pack_info_ptr,
		int16 *p_src,
		int32 *p_dst,
		int32 frame,
		int32 numch)
{
	uint32 packed_data;

#if defined(CH_STATUS_SW)
	uint32 packed_data1;
	uint32 parity;
#endif

	uint32 align_shift;
	uint32 data_mask;
	hdmi_data_shift_type data_shift_dir = HDMI_DATA_SHIFT_LEFT;
	int32_t *p_src32 = (int32_t *) p_src;

	switch (pack_info_ptr->in_sample_bit_width)
	{
	case 20:
		align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask    = 0xFFFFF000;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 24:
	case 32:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask   = 0xFFFFFF00;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 16:
	default:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
		data_mask   = 0x0000FFFF;
		break;
	}

	while (numch--)
	{
		if(16 < pack_info_ptr->in_sample_bit_width)
		{
			//for 24 bit case
			*p_src32 = (*p_src32) << pack_info_ptr->q_format_shift_factor;  //q_format_shift_factor = QFORMAT_SHIFT_FACTOR = 4
			packed_data = (uint32)(*p_src32++) & data_mask;
		}
		else
		{
			packed_data = (uint32)(*p_src++) & data_mask;
		}

		//packed_data = (pack_info_ptr->in_sample_bit_width <= 16) ? (uint32)(*p_src++) & data_mask :(uint32)(*p_src32++) & data_mask;

		if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
		{
			packed_data >>= align_shift;
		}
		else
		{
			packed_data <<= align_shift;
		}

#if defined(CH_STATUS_SW)
		packed_data |= pack_info_ptr->ch_status_table_ptr[frame];

		// calculate parity
		//Bit 31 (parity bit) carries a parity bit such that bits 4 to 31 inclusive carry an even number of ones and an even number of zeros
		packed_data1  = packed_data & 0xFFFFFFF0;
		packed_data1 ^= packed_data1 >> 1;
		packed_data1 ^= packed_data1 >> 2;
		packed_data1 = (packed_data1 & 0x11111111U) * 0x11111111U;
		// Parity value at bit28
		parity = (packed_data1 << 3) & 0x80000000;

		// fill P from generated parity
		*p_dst++ = (int32)(packed_data | parity);
#else

		*p_dst++ = (int32)(packed_data);
#endif //#if defined(CH_STATUS_SW)
	}
}

// P | C | U | V | DATA20 | AUX4 | PREAMBLE4
void afe_hdmi_output_pack_data(afe_hdmi_output_pack_info_t *pack_info_ptr,
		int16 *p_src,
		int32 *p_dst,
		int32 frame,
		int32 numch)
{
	uint32 packed_data;

#if defined(CH_STATUS_SW)
	uint32 packed_data1;
	uint32 parity;
#endif

	uint32 align_shift;
	uint32 data_mask;
	hdmi_data_shift_type data_shift_dir = HDMI_DATA_SHIFT_LEFT;
	int32_t *p_src32 = (int32_t *) p_src;

	switch (pack_info_ptr->in_sample_bit_width)
	{
	case 20:
		align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
		data_mask    = 0xFFFFF000;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 24:
	case 32:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
		data_mask   = 0xFFFFFF00;
		data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
		break;
	case 16:
	default:
		align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 + HDMI_IEC60958_AUX_START_BIT4;
		data_mask   = 0x0000FFFF;
		break;
	}

	while (numch--)
	{
		if(16 < pack_info_ptr->in_sample_bit_width)
		{
			//for 24 bit case
			*p_src32 = (*p_src32) << pack_info_ptr->q_format_shift_factor;  //q_format_shift_factor = QFORMAT_SHIFT_FACTOR = 4
			packed_data = (uint32)(*p_src32++) & data_mask;
		}
		else
		{
			packed_data = (uint32)(*p_src++) & data_mask;
		}

		//packed_data = (pack_info_ptr->in_sample_bit_width <= 16) ? (uint32)(*p_src++) & data_mask :(uint32)(*p_src32++) & data_mask;

		if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
		{
			packed_data >>= align_shift;
		}
		else
		{
			packed_data <<= align_shift;
		}

#if defined(CH_STATUS_SW)
		packed_data |= pack_info_ptr->ch_status_table_ptr[frame];

		// calculate parity
		//Bit 31 (parity bit) carries a parity bit such that bits 4 to 31 inclusive carry an even number of ones and an even number of zeros
		packed_data1  = packed_data & 0xFFFFFFF0;
		packed_data1 ^= packed_data1 >> 1;
		packed_data1 ^= packed_data1 >> 2;
		packed_data1 = (packed_data1 & 0x11111111U) * 0x11111111U;
		// Parity value at bit28
		parity = (packed_data1 << 3) & 0x80000000;

		// fill P from generated parity
		*p_dst++ = (int32)(packed_data | parity);
#else

		*p_dst++ = (int32)(packed_data);
#endif //#if defined(CH_STATUS_SW)
	}
}

ADSPResult afe_hdmi_output_init_chstatus(hdmi_output_port_state_t *p_dev_state)
{

	ADSPResult result = ADSP_EOK;
	uint32 data_format = p_dev_state->data_type;
	uint32 sampling_freq = 0;
	uint32 word_length = 0;
	hdmi_output_ch_sts_buf_t *p_tx_ch_sts_buf_0 = &p_dev_state->ch_sts_buf_0;
	hdmi_output_ch_sts_buf_t *p_tx_ch_sts_buf_1 = &p_dev_state->ch_sts_buf_1;

	switch (p_dev_state->sample_rate)
	{
	case 22050: // 0100
		sampling_freq = 0x4;
		break;
	case 32000:  // 0011 in 27 24
		sampling_freq = 0x3;
		break;
	case 44100:  // 0000 in 27 24
		sampling_freq = 0x0;
		break;
	case 48000:  // 0010 in 27..24
		sampling_freq = 0x2;
		break;
	case 88200:  // 1000 in 27..24
		sampling_freq = 0x8;
		break;
	case 96000:  // 1010 in 27..24
		sampling_freq = 0xa;
		break;
	case 176400: // 1100 in 27..24
		sampling_freq = 0xc;
		break;
	case 192000: // 1110 in 27..24
		sampling_freq = 0xe;
		break;
	default:
		break;
	}


	if(24 == p_dev_state->bit_width)
	{  // 1  0 1 1
		word_length = 0xb;
	}
	else // 16bits
	{
		word_length = 0;
	}

	uint32 ch_sts_buf0 = ( ((data_format<< 1) & 0x2) | ((sampling_freq << 24) & 0xf000000));
	uint32 ch_sts_buf1 = (word_length) & 0xf;

	//in case of AFE_PORT_PARAM_ID_CH_STATUS_CONFIG is ahead of AFE_PORT_PARAM_ID_HDMI_CONFIG
	//use the channel status setting from AFE_PORT_PARAM_ID_CH_STATUS_CONFIG. Need to handle the error check for this case (TBD)
	//otherwise use the one basing on sample_rate, bit_width, etc. from AFE_PORT_PARAM_ID_HDMI_CONFIG
	if(AFE_CH_STATUS_A == p_dev_state->channel_status_type)
	{
		ch_sts_buf0 = p_tx_ch_sts_buf_0->ch_sts_data0;
		ch_sts_buf1 = p_tx_ch_sts_buf_0->ch_sts_data1;
	}

	if(AFE_CH_STATUS_B == p_dev_state->channel_status_type)
	{
		ch_sts_buf0 = p_tx_ch_sts_buf_1->ch_sts_data0;
		ch_sts_buf1 = p_tx_ch_sts_buf_1->ch_sts_data1;
	}

	//tie channel status buf idx to DMA idx
	//DMA Index:              0           1          2            3
	//Ch Status Buf Index: (0, 1)      (2, 3)     (4, 5)       (6, 7)
	p_tx_ch_sts_buf_0->ch_sts_data0 = ch_sts_buf0;
	p_tx_ch_sts_buf_0->ch_sts_data1 = ch_sts_buf1;
	p_tx_ch_sts_buf_0->ch_sts_data2 = 0;
	p_tx_ch_sts_buf_0->ch_sts_data3 = 0;
	p_tx_ch_sts_buf_0->ch_sts_data4 = 0;
	p_tx_ch_sts_buf_0->ch_sts_data5 = 0;
	p_tx_ch_sts_buf_0->buf_idx = (p_dev_state->dma_idx * 2);


	p_tx_ch_sts_buf_1->ch_sts_data0 = ch_sts_buf0;
	p_tx_ch_sts_buf_1->ch_sts_data1 = ch_sts_buf1;
	p_tx_ch_sts_buf_1->ch_sts_data2 = 0;
	p_tx_ch_sts_buf_1->ch_sts_data3 = 0;
	p_tx_ch_sts_buf_1->ch_sts_data4 = 0;
	p_tx_ch_sts_buf_1->ch_sts_data5 = 0;
	p_tx_ch_sts_buf_1->buf_idx = p_tx_ch_sts_buf_0->buf_idx + 1;

	return result;
}

//According to table in page 158 of HDMI Compliance Test Specification
uint32 afe_hdmi_output_get_present_bits(uint32 ca)
{
	uint32 present_bits = 0;

	switch (ca)
	{
	case 0x1:
	case 0x2:
	case 0x3:
	default:
		present_bits = 3;  //0011
		break;
	case 0x4:
	case 0x8:
		present_bits = 5;  //0101
		break;
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x9:
	case 0xa:
	case 0xb:
		present_bits = 7;  //0111
		break;
	case 0xc:
	case 0x10:
	case 0x18:
	case 0x1c:
		present_bits = 0xd;  //1101
	case 0xd:
	case 0xe:
	case 0xf:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1d:
	case 0x1e:
	case 0x1f:
		present_bits = 0xf;  //1111
		break;
	case 0x14:
		present_bits = 9;  //1001
	case 0x15:
	case 0x16:
	case 0x17:
		present_bits = 0xb;  //1011
		break;
	}

	return present_bits;
}

MmpmCoreIdType afe_get_hdmitx_mmpm_coreid()
{
	return  MMPM_CORE_ID_LPASS_HDMITX;
}


MmpmBwPortIdType afe_get_hdmitx_mmpm_bwportid()
{
	return MMPM_BW_PORT_ID_HDMITX_MASTER;
}

ADSPResult afe_hdmi_dp_driver_int_notify(afe_dev_port_t *dev_port_ptr, uint32_t event_mask)    //for DP special handle case, send a msg to AFE SVC thread
{
	ADSPResult result = ADSP_EOK;
	uint32_t msg_size;
	elite_msg_any_t  msg;

	elite_msg_custom_afe_hdmi_over_dp_info_t   *pPayload = NULL;

	hdmi_output_port_state_t         *dev_state_ptr = NULL;

	dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

	msg_size = sizeof(elite_msg_custom_afe_hdmi_over_dp_info_t);

	if (ADSP_EOK == (result = elite_msg_create_msg( &msg, &msg_size, ELITE_CUSTOM_MSG, NULL, 0, ADSP_EOK)))
	{
		pPayload = (elite_msg_custom_afe_hdmi_over_dp_info_t*) msg.pPayload;
		pPayload->sec_op_code = ELITEMSG_CUSTOM_HDMI_OVER_DP;
		pPayload->afe_port_ptr = (void*)dev_port_ptr;
		pPayload->response_result = ADSP_EOK;
		pPayload->event_mask = event_mask;

		// push custom msg
		if (ADSP_EOK != (result = qurt_elite_queue_push_back(afe_svc_global_ptr->static_svc_cmdq_ptr, (uint64_t *)&msg)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in qurt_elite_queue_push_back() with return  result: %d", result);
			elite_msg_return_payload_buffer(&msg);
		}
	}
	else
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in elite_msg_create_msg() with return result: %d", result);
	}

	return result;
}


ADSPResult afe_hdmi_over_dp_unmute(afe_dev_port_t *dev_port_ptr, void* p_ctx)
{
	ADSPResult result = ADSP_EOK;

	if(dev_port_ptr)
	{
		hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

		if(dev_state_ptr)
		{
			if(HDMI_DP_DEVICE_STATE_START != dev_state_ptr->dp_device_state)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED afe_hdmi_over_dp_unmute with DP device state %d",dev_state_ptr->dp_device_state);

				result = ADSP_EFAILED;
			}
			else
			{
				//do UNMUTE via register write

				//save pkt pointer and state
				dev_state_ptr->p_ctx = p_ctx;
				dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_UNMUTE_START; //UNMUTE start

				hal_lpass_hdmi_output_dp_mute_stream(dev_state_ptr->stream_id, 0);   //1 - mute, 0 - unmute
			}
		}
	}

	return result;
}

ADSPResult afe_hdmi_over_dp_mute(afe_dev_port_t *dev_port_ptr, void* p_ctx)
{
	ADSPResult result = ADSP_EOK;

	if(dev_port_ptr)
	{
		hdmi_output_port_state_t *dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;

		if(dev_state_ptr)
		{
			if(HDMI_DP_DEVICE_STATE_UNMUTE_DONE != dev_state_ptr->dp_device_state)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "FAILED afe_hdmi_over_dp_mute with DP device state %d",dev_state_ptr->dp_device_state);

				result = ADSP_EFAILED;
			}
			else
			{
				//do MUTE via register write

				//save pkt pointer and state
				dev_state_ptr->p_ctx = p_ctx;
				dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_MUTE_START; //MUTE start

				hal_lpass_hdmi_output_dp_mute_stream(dev_state_ptr->stream_id, 1);   //1 - mute, 0 - unmute
			}
		}
	}

	return result;
}


/*Although DP and HDMI Audio InfoFrame follows the CEA specification, for the hardware we
 * are going with the IEC 60958 channel status notation for sampling frequency. This is
 * done to allow support for the sampling frequencies that are not supported in CEA. These
 * are needed for HBR use cases*/
static ADSPResult afe_hdmi_dp_driver_get_freq_idx(uint32 sample_freq, uint16_t *freq_index)
{
	uint16 freq_idx = 0;
	ADSPResult result = ADSP_EOK;

	switch(sample_freq)
	{
	case 22050:
	  freq_idx = 0x4;
	  break;
	case 24000:
	  freq_idx=  0x6;
	  break;
	case 32000:
		freq_idx = 0x3;
		break;
	case 44100:
		freq_idx = 0x0;
		break;
	case 48000:		
		freq_idx = 0x2;		
		break;
	case 64000:
	  freq_idx = 0xB;
	  break;
	case 88200:
		freq_idx = 0x8;
		break;
	case 96000:
		freq_idx = 0xA;
		break;
	case 128000:
	  freq_idx = 0x2B;
	  break;
	case 176400:
		freq_idx = 0xC;
		break;
	case 192000:
		freq_idx = 0xE;
		break;
	case 256000:
	  freq_idx = 0x1B;
	  break;
	case 384000:
	  freq_idx = 0x5;
	  break;
	case 352800:
	  freq_idx = 0xD;
	  break;
	case 512000:
	  freq_idx = 0x3B;
	  break;
	case 705600:
	  freq_idx = 0x2D;
	  break;
	case 768000:
	  freq_idx = 0x9;
	  break;
	case 1024000:
	  freq_idx = 0x35;
	  break;
	case 1411200:
	  freq_idx = 0x1D;
	  break;
	case 1536000:
	  freq_idx = 0x15;
	  break;
	default:
	  freq_idx = 0x1; //denotes sampling frequency not indicated
	  result =  ADSP_EUNSUPPORTED;
		break;
	}

	*freq_index = freq_idx;
	return result;
}

static uint16 afe_hdmi_dp_driver_get_cc_idx(uint32 num_channel)
{
	uint16 cc_idx = 0;

	switch(num_channel)
	{
	case 2:
		cc_idx = 1;
		break;
	case 3:
		cc_idx = 2;
		break;
	case 4:
		cc_idx = 3;
		break;
	case 5:
		cc_idx = 4;
		break;
	case 6:
		cc_idx = 5;
		break;
	case 7:
		cc_idx = 6;
		break;
	case 8:
		cc_idx = 7;
		break;
	default:
		break;
	}

	return cc_idx;
}


ADSPResult afe_hdmi_over_dp_cust_msg_handler(void *cust_payload_ptr)
{
	ADSPResult result=ADSP_EOK;
	elite_msg_custom_afe_hdmi_over_dp_info_t *cust_msg_ptr;
	uint32_t                   event_mask;
	afe_dev_port_t             *dev_port_ptr;
	hdmi_output_port_state_t   *dev_state_ptr;
	elite_apr_packet_t* pkt_ptr;
	int32_t    rc = 0;

	cust_msg_ptr = (elite_msg_custom_afe_hdmi_over_dp_info_t *)cust_payload_ptr;
	event_mask =  cust_msg_ptr->event_mask;

	dev_port_ptr = (afe_dev_port_t*)cust_msg_ptr->afe_port_ptr;

	if (NULL == dev_port_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Null afe port pointer or some corruption happened");
		return ADSP_EFAILED;
	}

	dev_state_ptr = (hdmi_output_port_state_t *)dev_port_ptr->afe_drv;
	/*Retrive the apr pkt pointer, which gets updated as part of mute start(dma stop), unmute start((dma stop)
      in AFE Svc thread. This handler is also running in AFESVC thread context*/
	pkt_ptr       =  (elite_apr_packet_t *)dev_state_ptr->p_ctx;

	if(DMA_TYPE0_EVENT_DP_AUD_DIS_ERROR & event_mask)
	{
		dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_MUTE_DONE;
		MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "Received SDE Audio disable interrupt");
		/* here APR Pkt ptr passed as NULL, as this event wont trigger as part of
           APR command handler processing like AFE start/AFE stop.*/
		result = afe_port_apr_dma_stop(dev_port_ptr,NULL);
	}
	else
	{
		//normal case
		if(HDMI_DP_DEVICE_STATE_UNMUTE_START == dev_state_ptr->dp_device_state)
		{
			//UNMUTE case
			dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_UNMUTE_DONE;
			// Init the variable to NULL, as this packet is freeing now
			dev_state_ptr->p_ctx =  NULL;
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ELITEMSG_CUSTOM_HDMI_OVER_DP,Received UNMUTE DONE dev_port: 0x%x, p_ctx: 0x%x", dev_port_ptr->intf, pkt_ptr);
		}

		if(HDMI_DP_DEVICE_STATE_MUTE_START == dev_state_ptr->dp_device_state)
		{
			//MUTE case
			dev_state_ptr->dp_device_state = HDMI_DP_DEVICE_STATE_MUTE_DONE;
			// Init the variable to NULL, as this packet is freeing now
			dev_state_ptr->p_ctx =  NULL;
			result = afe_port_apr_dma_stop(dev_port_ptr,pkt_ptr);
			MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ELITEMSG_CUSTOM_HDMI_OVER_DP, Received MUTE DONE  dev_port_ptr: 0x%x, p_ctx: 0x%x", dev_port_ptr->intf, pkt_ptr);
		}
	}

	if (pkt_ptr != NULL)
	{
		if (APR_EOK != (rc = elite_apr_if_end_cmd(afe_svc_global_ptr->apr_handle, pkt_ptr, result)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ELITEMSG_CUSTOM_HDMI_OVER_DP: Failed in elite_apr_if_end_cmd() with return code: %ld", rc);
			result = ADSP_EFAILED;
		}
	}

	return result;

}
