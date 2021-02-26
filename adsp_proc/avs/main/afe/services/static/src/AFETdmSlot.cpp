/*========================================================================
  ELite

  This file contains AFE TDM slot operation

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

    $Header: //source/qcom/qct/multimedia2/Audio/elite/static_svcs/AFESvc/...

    when       who         what, where, why
    --------   ---         -------------------------------------------------------
   05/12/14    david       initial version

  ========================================================================== */
/*==========================================================================
  Include files
  ========================================================================== */

#include "AFEPortManagement.h" 
#include "AFETdmSlot.h"



/* ============================================================================
    Constant / Define Declarations
============================================================================ */
#define OFFSET_CONVERT_ENABLE                   1                        //for debug purpose only

typedef struct 
{
	uint16_t                header_type;
	uint16_t                port_idx;
} header_info_per_frame_t;

/*==========================================================================
  Function Definitions
========================================================================== */

static uint8_t get_sample_width_from_custom_tdm_header(void *p_dev_port);  //bit width per channel
static uint8_t get_asad_value_from_custom_tdm_header(void *p_dev_port);  
static uint8_t get_num_channel_from_asad_value(uint8_t asad);

ADSPResult slot_mapping_offset_handle(afe_dev_port_t *p_dev_port);

/**
 * tdm module.
 *
 * setting config & enable is possible only in CONFIG state
 * setting disable is possible only in STOP state.
 */
ADSPResult afe_port_set_tdm_module_params(afe_dev_port_t *pDevPort,uint32_t param_id,
		int8_t *param_buffer_ptr, uint16_t param_size)
{
	ADSPResult result = ADSP_EOK;

	if (NULL == param_buffer_ptr)                         //param buffer is null
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Port 0x%x: tdm module config: NULL pointer.",pDevPort->intf);
		return ADSP_EBADPARAM;
	}

	switch(param_id)
	{
	case AFE_PARAM_ID_PORT_SLOT_MAPPING_CONFIG:
	{
		uint16_t i, j;
		uint32_t slot_mapping_cfg_version = (uint32_t) *param_buffer_ptr;
		if(slot_mapping_cfg_version > AFE_API_VERSION_SLOT_MAPPING_CONFIG)
		{
			slot_mapping_cfg_version = AFE_API_VERSION_SLOT_MAPPING_CONFIG;
		}

		if(0x1 == slot_mapping_cfg_version)
		{
			if (param_size < sizeof(afe_param_id_slot_mapping_cfg_t))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: Invalid payload size: %d, port_id: 0x%x", param_size, pDevPort->intf);
				return ADSP_EBADPARAM;
			}

			// This condition is required only when the cfg params received before port got started.
			// If they received after port start, we will return bad parameter
			if(AFE_PORT_STATE_RUN == pDevPort->port_state)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: afe port has started already, port_id: 0x%x", pDevPort->intf);
				result = ADSP_EBADPARAM;
				break;
			}

			// copy all the params: this copy is necessary because, in CONFIG state we do not know sample rate etc that are needed for fully configuring the lib.
			afe_param_id_slot_mapping_cfg_t *cfg_params     = (afe_param_id_slot_mapping_cfg_t*)param_buffer_ptr;

			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port 0x%x: slot maaping config data [num_channel:%d] [slot_bitwidth:%d] [data_align_type:%lu]", pDevPort->intf, cfg_params->num_channel, cfg_params->bitwidth, cfg_params->data_align_type);

			for(i = 0; i < AFE_PORT_MAX_AUDIO_CHAN_CNT; i++)
			{
				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Array [Index:%d] slot slot mapping [slot_offset:0x%x]", i, cfg_params->offset[i]);
			}

			//Error check
			if((1 > cfg_params->num_channel) || (8 < cfg_params->num_channel))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: num of channel is not supported: port_id: 0x%x num_channel: %d", pDevPort->intf, cfg_params->num_channel);
				return ADSP_EBADPARAM;
			}

			if((16 != cfg_params->bitwidth) && (24 != cfg_params->bitwidth) && (32 != cfg_params->bitwidth))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: bit width is not supported: port_id: 0x%x bitwidth: %d", pDevPort->intf, cfg_params->bitwidth);
				return ADSP_EBADPARAM;
			}

			if((AFE_SLOT_MAPPING_DATA_ALIGN_MSB != cfg_params->data_align_type) && (AFE_SLOT_MAPPING_DATA_ALIGN_LSB != cfg_params->data_align_type))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: data align type is not supported: port_id: 0x%x data_align_type: %lu", pDevPort->intf, cfg_params->data_align_type);
				return ADSP_EBADPARAM;
			}

			for(i = 0; i < AFE_PORT_MAX_AUDIO_CHAN_CNT; i++)
			{
				if(AFE_SLOT_MAPPING_OFFSET_INVALID != cfg_params->offset[i])
				{
					pDevPort->slot_mapping_info.offset_info[i].offset_input = cfg_params->offset[i];
					pDevPort->slot_mapping_info.offset_info[i].offset_0 = cfg_params->offset[i];
					pDevPort->slot_mapping_info.offset_info[i].copy_width_0 =  cfg_params->bitwidth / 8;
				}
				else
				{
					break;
				}
			}

			if(cfg_params->num_channel != i)
			{
				MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: num valid offset is not matched with num_channel: port_id: 0x%x num valid offset: %d num_channel: %d", pDevPort->intf, i, cfg_params->num_channel);
				return ADSP_EBADPARAM;
			}

			//check offset overlap error
			for(i = 0; i < cfg_params->num_channel; i++)
			{
				for(j = 0; j < cfg_params->num_channel; j++)
				{
					if(i != j)
					{
						if((pDevPort->slot_mapping_info.offset_info[i].offset_input < (pDevPort->slot_mapping_info.offset_info[j].offset_input + cfg_params->bitwidth/8)) && (pDevPort->slot_mapping_info.offset_info[i].offset_input > pDevPort->slot_mapping_info.offset_info[j].offset_input))
						{
							MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: slot mapping offset overlap: port_id: 0x%x offset 1 : %d offset 2: %d", pDevPort->intf, pDevPort->slot_mapping_info.offset_info[i].offset_input, pDevPort->slot_mapping_info.offset_info[j].offset_input);
							return ADSP_EBADPARAM;
						}
					}
				}
			}

			pDevPort->slot_mapping_info.slot_bytes_per_channel = cfg_params->bitwidth / 8;  //only support 2 bytes, 3 bytes, and 4 bytes

			pDevPort->slot_mapping_info.slot_num_channel = cfg_params->num_channel;

			pDevPort->slot_mapping_info.data_align_type = cfg_params->data_align_type;

			// The call sequence should be:
			// 1. Configure AFE port
			// 2. Slot mapping configure
			// 3. Start AFE port
			// In no group device case, pDevPort->channels is got via channel_mode from afe_port_cmd_i2s_cfg_t.
			// It SHOULD BE overwriten the number of channels based on SLOT MAPPING configuration which is the actual number of channel for the stream.
			pDevPort->dev_channels = cfg_params->num_channel;
			// overwrite the bit width based on slot mapping configuration which is the actual stream bit width.
			pDevPort->dev_bytes_per_channel = (cfg_params->bitwidth > 16) ? 4 : 2;

			//if applicable, overwrite the port media type with the device media type.
			//this is applicable for all the use cases where the port media type is not explicitly set by the HLOS client.
			afe_port_copy_media_type_dev_to_port(pDevPort);

			pDevPort->slot_mapping_info.is_cfg_received = TRUE;

			if(OFFSET_CONVERT_ENABLE)
			{
				//Input offset needs to convert to TDM based
				slot_mapping_offset_handle(pDevPort);
			}


			MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "TDM slot configured port:0x%x ch: %d byte-per-ch: %d bitwidth: %d",
					pDevPort->intf, pDevPort->channels, pDevPort->bytes_per_channel, pDevPort->bit_width);
		}
		else
		{
			MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE TDM Slot Cfg fail: Incompatible cfg version: %u, port_id: 0x%x", (unsigned int)slot_mapping_cfg_version, pDevPort->intf);
			result = ADSP_EBADPARAM;
		}

		break;
	}
	case AFE_PARAM_ID_CUSTOM_TDM_HEADER_CONFIG:
	{
		uint16_t i;
		uint8_t *ptr_header_8;
		uint16_t *ptr_header_16;
		uint32_t tdm_header_cfg_version = (uint32_t) *param_buffer_ptr;

		if(tdm_header_cfg_version > AFE_API_VERSION_CUSTOM_TDM_HEADER_CONFIG)
		{
			tdm_header_cfg_version = AFE_API_VERSION_CUSTOM_TDM_HEADER_CONFIG;
		}

		if(0x1 == tdm_header_cfg_version)
		{
			if (param_size < sizeof(afe_param_id_custom_tdm_header_cfg_t))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg fail: Invalid payload size: %d, port_id: 0x%x", param_size, pDevPort->intf);
				return ADSP_EBADPARAM;
			}

			afe_param_id_custom_tdm_header_cfg_t *cfg_params     = (afe_param_id_custom_tdm_header_cfg_t*)param_buffer_ptr;


			MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE port 0x%x: CUSTOM TDM HEADER [start_offset:%d] [heder_width:%d] [header_type:%d] [num_of_frame_repeat:%d]", pDevPort->intf, cfg_params->start_offset, cfg_params->header_width, cfg_params->header_type, cfg_params->num_frame_repeat);

			if(2 != cfg_params->header_width)
			{
				//only support 2 bytes case now
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg fail: header width is not supported: port_id: 0x%x header_width: %d", pDevPort->intf, cfg_params->header_width);
				return ADSP_EBADPARAM;
			}

			if((AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT != cfg_params->header_type) && (AFE_CUSTOM_TDM_HEADER_TYPE_ENTERTAINMENT_MOST != cfg_params->header_type))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg fail: header type is not supported: port_id: 0x%x header_type: %d", pDevPort->intf, cfg_params->header_type);
				return ADSP_EBADPARAM;
			}

			if(8 != cfg_params->num_frame_repeat)
			{
				//only support 8 frame case now
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg fail: num_frame_repeat is not supported: port_id: 0x%x num_frame_repeat: %d", pDevPort->intf, cfg_params->num_frame_repeat);
				return ADSP_EBADPARAM;
			}

			if (param_size < (sizeof(afe_param_id_custom_tdm_header_cfg_t) + cfg_params->header_width * cfg_params->num_frame_repeat))
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg fail: Invalid payload size (total payload size should include custom header payload): %d, port_id: 0x%x", param_size, pDevPort->intf);
				return ADSP_EBADPARAM;
			}

			pDevPort->custom_header_info.header_offset.offset_input = cfg_params->start_offset;
			pDevPort->custom_header_info.header_offset.offset_0 = cfg_params->start_offset;
			pDevPort->custom_header_info.header_offset.copy_width_0 = cfg_params->header_width;
			pDevPort->custom_header_info.header_width_per_frame = cfg_params->header_width;
			pDevPort->custom_header_info.header_type = cfg_params->header_type;
			pDevPort->custom_header_info.num_frame_repeat = cfg_params->num_frame_repeat;

			afe_tdm_slot_mapping_offset_convert(&pDevPort->custom_header_info.header_offset, pDevPort->custom_header_info.header_width_per_frame);

			ptr_header_8 = (uint8_t *)(param_buffer_ptr + sizeof(afe_param_id_custom_tdm_header_cfg_t));

			//In case, the sad header type is dynamically changed between DEFAULT and ENTTAINMENT without port stop
			//if pDevPort->custom_header_info.p_header memory is allocated, i.e.  pDevPort->custom_header_info.p_header is not NULL, skip below.
			if(NULL == pDevPort->custom_header_info.p_header)
			{
				if(NULL == (pDevPort->custom_header_info.p_header = (void *) qurt_elite_memory_aligned_malloc(cfg_params->header_width * cfg_params->num_frame_repeat, 8, AFE_SINK_ADD32_BUF)))
				{
					return ADSP_ENOMEMORY;
				}
			}

			memscpy(pDevPort->custom_header_info.p_header,
					cfg_params->header_width * cfg_params->num_frame_repeat,
					(void*)ptr_header_8,
					cfg_params->header_width * cfg_params->num_frame_repeat);

			ptr_header_16 = (uint16_t *) pDevPort->custom_header_info.p_header;

			for(i = 0; i < cfg_params->num_frame_repeat; i++)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE CUSTOM TDM HEADER Cfg - header: 0x%x", ptr_header_16[i]);
			}

			pDevPort->custom_header_info.bit_width_from_header = get_sample_width_from_custom_tdm_header((void *)pDevPort);
			pDevPort->custom_header_info.asad_from_header = get_asad_value_from_custom_tdm_header((void *)pDevPort);
			pDevPort->custom_header_info.num_chan_from_header = get_num_channel_from_asad_value(pDevPort->custom_header_info.asad_from_header);

			MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "CUSTOM TDM HEADER parsed [bit_width_from_header:%i] [asad_from_header:0x%x] [num_chan_from_header:%i]", pDevPort->custom_header_info.bit_width_from_header, pDevPort->custom_header_info.asad_from_header, pDevPort->custom_header_info.num_chan_from_header);

			//backup default header info
			if(pDevPort->p_group_device_state && (AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT == pDevPort->custom_header_info.header_type))
			{
				afe_group_device_state_struct_t    *p_group_device = pDevPort->p_group_device_state;

				//backup default header only one time
				if(FALSE == p_group_device->default_header_backup.is_cfg_received)
				{
					memscpy((void*)&p_group_device->default_header_backup,
							sizeof(afe_custom_header_info_t),
							(void*)&pDevPort->custom_header_info,
							sizeof(afe_custom_header_info_t));

					if(NULL == (p_group_device->default_header_backup.p_header = (void *) qurt_elite_memory_aligned_malloc(cfg_params->header_width * cfg_params->num_frame_repeat, 8, AFE_SINK_ADD32_BUF)))
					{
						if(pDevPort->custom_header_info.p_header)
						{
							qurt_elite_memory_aligned_free(pDevPort->custom_header_info.p_header);
							pDevPort->custom_header_info.p_header = NULL;
						}

						p_group_device->default_header_backup.is_cfg_received = FALSE;

						return ADSP_ENOMEMORY;
					}
					else
					{
						memscpy(p_group_device->default_header_backup.p_header,
								cfg_params->header_width * cfg_params->num_frame_repeat,
								(void*)ptr_header_8,
								cfg_params->header_width * cfg_params->num_frame_repeat);
					}

					pDevPort->custom_header_info.is_default_header_backup = TRUE;

					//mark this done at last
					p_group_device->default_header_backup.is_cfg_received = TRUE;
				}
			}

			//mark this done at last
			pDevPort->custom_header_info.is_cfg_received = TRUE;

			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE CUSTOM TDM HEADER is configured port:0x%x", pDevPort->intf);
		}

		break;
	}
	default:
		result = ADSP_EBADPARAM;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "afe_port_set_tdm_module_params() :: param ID not supported \n");
	}

	return result;
}



static uint8_t get_sample_width_from_custom_tdm_header(void *p_dev_port)  //bit width per channel
{
	uint8_t bit_width;
	afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)p_dev_port;
	uint16_t* p_header_16 = (uint16_t*)afe_port_ptr->custom_header_info.p_header;
	uint16_t header1 = p_header_16[1];

	bit_width = (uint8_t)(header1 & 0x00FF);

	return bit_width;
}
static uint8_t get_asad_value_from_custom_tdm_header(void *p_dev_port)
{
	uint8_t asad;
	afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)p_dev_port;
	uint16_t* p_header_16 = (uint16_t*)afe_port_ptr->custom_header_info.p_header;
	uint16_t header3 = p_header_16[3];

	asad = (uint8_t)(header3 & 0x00FF);

	return asad;
}

static uint8_t get_num_channel_from_asad_value(uint8_t asad)
{
	uint16_t i, num_chan = 0;
	uint8_t temp = asad;

	for(i = 0; i < 8; i++)
	{
		if((temp >> (7 - i)) & 1)
		{
			num_chan++;
		}
	}

	return num_chan;
}
//NOTE:
// Below is valid for  p_group_device->bit_width = 32 case ONLY
// Need to handle p_group_device->bit_width = 16 case 
// When p_group_device->bit_width is 16, two channels equal onw DW.
// For example I2S case, the memory format is
// b31    b16 | b15      b0
// R ...... R | L        L
// R        R | L        L
// The data OUT or IN on SD line is LEFT channel first
// That means offset = 0 is mapped to the MSB of LEFT channel
//
ADSPResult slot_mapping_offset_handle(afe_dev_port_t *p_dev_port)
{
	ADSPResult result = ADSP_EOK;
	int16_t  i;
	int16_t width_per_channel = p_dev_port->slot_mapping_info.slot_bytes_per_channel;    //in bytes, 2, 3, 4

	afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)p_dev_port->p_group_device_state;

	if(p_group_device &&(16 == p_group_device->bit_width))
	{
		//the below configuration should not be valid:
		//configure hw as 16 bit per channel but the frame size is larger than 32 bit

		//If p_group_device->bit_width is 16, the hardware is configured as max 32 bit frame size.
		//As left channel is always IN or OUT first, the offset convert is not needed.
		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE TDM offset convert is not needed @ port:0x%x as bit withd is %d", p_dev_port->intf, p_group_device->bit_width);
	}
	else
	{

		for(i = 0; i < p_dev_port->slot_mapping_info.slot_num_channel; i++)
		{
			afe_tdm_slot_mapping_offset_convert(&p_dev_port->slot_mapping_info.offset_info[i], width_per_channel);
		}
	}

	return result;
}


ADSPResult afe_tdm_slot_mapping_offset_convert(tdm_slot_offset_info_t *p_slot_offset_info, int16_t width_per_slot)
{
	//width_per_slot is in byte unit

	ADSPResult result = ADSP_EOK;

	if(p_slot_offset_info)
	{
		int16_t which_dw;
		int16_t end_position_this_dw;    //in bytes
		int16_t offset_input;

		offset_input = p_slot_offset_info->offset_0;        //the input offset from client is always saved in offset_0 for each channel

		which_dw = offset_input/4;    //which DW basing on this offset. start counting from 0.
		end_position_this_dw = offset_input%4 + width_per_slot;       //point to copy start point

		if(4 >= end_position_this_dw)
		{
			//end_position_this_dw within this DW
			p_slot_offset_info->copy_width_0 = width_per_slot;
			p_slot_offset_info->offset_0 = 4 - end_position_this_dw + which_dw * 4;   //convert the offset to low memory point

			p_slot_offset_info->copy_width_1 = 0;
			p_slot_offset_info->offset_1 = 0;
		}
		else
		{
			//part I this data line
			p_slot_offset_info->copy_width_0 = 4 - offset_input%4;
			p_slot_offset_info->offset_0 = which_dw * 4;   //convert the offset to low memory point

			//part II
			which_dw = which_dw + 1;   //advance one DW
			p_slot_offset_info->copy_width_1 = end_position_this_dw - 4;
			p_slot_offset_info->offset_1 = which_dw * 4 + (4 - p_slot_offset_info->copy_width_1);
		}
	}
	else
	{
		result = ADSP_EFAILED;
	}

	return result;
}

//1. Add SAD header here, only for 48 KHz case
//go through all AFE ports in this group
//Set SAD header for each frame if associated port is configured SAD header
//if SAD header is configured and type is ENT, setup SAD header
ADSPResult afe_tdm_custom_header_handle(void *p_dev_port)
{
	ADSPResult result = ADSP_EOK;

	afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)p_dev_port;

	afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)afe_port_ptr->p_group_device_state;

	uint32_t  total_num_frame;
	uint16_t frame_idx, samp_cnt, i;
	uint16_t port_idx =0;
	afe_dev_port_t    *p_dev_port_cur = NULL;
	int8_t    *p_sad_header_cur = NULL;
	int8_t    *p_sad_header_init;
	header_info_per_frame_t  header_info_cur[4];     //set header_type as INVALID first. the max is 4 SD lines
	uint16_t *ptr_header_16 = NULL;

	uint16_t  num_frame_repeat = 8;
	uint16_t  copy_width_0 = 0;

	p_sad_header_init = p_group_device->p_group_io_buf;

	total_num_frame = (p_group_device->int_samples_per_period * p_group_device->bytes_per_channel * p_group_device->num_channels)
    		/ p_group_device->frame_size / p_group_device->num_frame_in_group;

	memset((void*)&header_info_cur[0], 0, sizeof(header_info_per_frame_t)*4);

	for(frame_idx = 0; frame_idx < p_group_device->num_frame_in_group; frame_idx++)
	{
		//go through all AFE ports in this group and find ports belong to each frame
		for (port_idx = 0; port_idx < AFE_GROUP_DEVICE_NUM_PORTS; port_idx++)
		{
			p_dev_port_cur = p_group_device->p_dev_port[port_idx];

			if (NULL != p_dev_port_cur)
			{
				//need to handle some ports' state is changed from STOP to RUN here
				if(AFE_PORT_STATE_RUN == p_dev_port_cur->port_state)
				{
					if(p_dev_port_cur->frame_idx == frame_idx )
					{
						//for this frame
						if(p_dev_port_cur->custom_header_info.is_cfg_received)
						{
							//AFE_CUSTOM_TDM_HEADER_TYPE_ENTERTAINMENT is always overwriten AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT
							if((AFE_CUSTOM_TDM_HEADER_TYPE_INVALID == header_info_cur[frame_idx].header_type) ||
									(AFE_CUSTOM_TDM_HEADER_TYPE_DEFAULT == header_info_cur[frame_idx].header_type))
							{
								header_info_cur[frame_idx].header_type = p_dev_port_cur->custom_header_info.header_type;
								header_info_cur[frame_idx].port_idx = port_idx;
							}
						}
					}
					else
					{
						continue;
					}
				}
			}
		}
	}


	//fill SAD header for this frame
	for(frame_idx = 0; frame_idx < p_group_device->num_frame_in_group; frame_idx++)
	{
		if(AFE_CUSTOM_TDM_HEADER_TYPE_INVALID != header_info_cur[frame_idx].header_type)
		{
			p_dev_port_cur = p_group_device->p_dev_port[header_info_cur[frame_idx].port_idx];

			p_sad_header_cur = p_sad_header_init;

			p_sad_header_cur = p_sad_header_cur + frame_idx * p_group_device->frame_size + p_dev_port_cur->custom_header_info.header_offset.offset_0;

			num_frame_repeat = p_dev_port_cur->custom_header_info.num_frame_repeat;
			copy_width_0 = p_dev_port_cur->custom_header_info.header_offset.copy_width_0;

			if(2 == p_dev_port_cur->custom_header_info.header_width_per_frame)
			{
				ptr_header_16 = (uint16_t *)p_dev_port_cur->custom_header_info.p_header;
			}
			else
			{
				//TBD
			}
		}
		else
		{
			//need to insert the default sad header if there is a valid backup
			if((p_group_device->default_header_backup.is_cfg_received) && (AFE_CUSTOM_TDM_HEADER_TYPE_INVALID != p_group_device->default_header_backup.header_type))
			{
				p_sad_header_cur = p_sad_header_init;

				p_sad_header_cur = p_sad_header_cur + frame_idx * p_group_device->frame_size + p_group_device->default_header_backup.header_offset.offset_0;

				num_frame_repeat = p_group_device->default_header_backup.num_frame_repeat;
				copy_width_0 = p_group_device->default_header_backup.header_offset.copy_width_0;

				if(2 == p_group_device->default_header_backup.header_width_per_frame)
				{
					ptr_header_16 = (uint16_t *)p_group_device->default_header_backup.p_header;
				}
				else
				{
					//TBD
				}
			}
		}

		if(ptr_header_16)
		{
			for(samp_cnt = 0; samp_cnt < total_num_frame; samp_cnt++)
			{
				i = samp_cnt % num_frame_repeat;  //i = 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, ...
				//copy the 0th SAD header to this frame
				memscpy(p_sad_header_cur,
						copy_width_0,
						(int8_t*)&ptr_header_16[i],
						copy_width_0);

				//p_group_device->default_header_backup.header_offset.copy_width_1 will be always 0. do not need to handle part II for custom header

				//jump to next sample in this buffer
				p_sad_header_cur += (p_group_device->frame_size * p_group_device->num_frame_in_group);
			}
		}
	}


	return result;
}

