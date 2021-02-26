/*========================================================================

 *//** @file AudioEncSvc_Prop_Util.cpp
This file contains Elite Audio encoder service.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/28/16   HB		Create file for proprietary information.

========================================================================== */
#include "AudioEncSvc_Includes.h"
#include "AudioEncSvc_CapiV2Util.h"

bool_t AudioEncSvc_IsProcessB4Eos(AudioEncSvc_t* pMe)
{
	bool_t need_to_process_b4_eos = false;
	for (uint8_t i=0; i < ENC_SVC_MAX_CAPI; i++)
	{
		if (!pMe->capiContainer[i]) break;

		pMe->capiContainer[i]->in_buf_params.inputs.flags.marker_eos = true;

		if( (ASM_MEDIA_FMT_EAC3 == pMe->capiContainer[i]->enc_cfg_id) ||
				(ASM_MEDIA_FMT_AC3 == pMe->capiContainer[i]->enc_cfg_id) ||
				(ASM_MEDIA_FMT_WMA_V8 == pMe->capiContainer[i]->enc_cfg_id))
		{
			need_to_process_b4_eos = true;
		}
	}
	return need_to_process_b4_eos;
}

/*
 *Function AudioEncSvc_UpdateNonPcmEncCfg
 *
 *Parameters: AudioEncSvc_t* pMe: Instance of enc svc
 *            asm_enc_cfg_blk_param_v2_t *pEncCfgParam: Encoder config params
 *
 *Description: Updates encoder CAPI with encoder config sent by client

 * Encoder config is sent for encoders only (main capi). not for packetizers or depacketizers.
 *
 *Returns: ADSPResult
 *
 */
ADSPResult AudioEncSvc_UpdateNonPcmEncCfg(AudioEncSvc_t *pMe, void* pConfig)
{
	enc_CAPI_container_t *main_capi = pMe->mainCapiContainer;

	//read the format blocks
	uint32_t ulEncCfgId = main_capi->enc_cfg_id;
	uint8_t* pConfigBlk = (uint8_t*)pConfig;
	int32_t result = ADSP_EOK;

	switch (ulEncCfgId)
	{
	case ASM_MEDIA_FMT_ADPCM:
	{
		asm_adpcm_fmt_blk_t *pEncConfig = (asm_adpcm_fmt_blk_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->num_channels;
		if(1 == pMe->out_med_fmt_per_enc_cfg.num_channels)
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		}
		else
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_L;
			pMe->out_med_fmt_per_enc_cfg.chan_map[1] = PCM_CHANNEL_R;
		}
		break;
	}
	case ASM_MEDIA_FMT_AAC_V2:
	{
		asm_aac_enc_cfg_v2_t *pEncConfig = (asm_aac_enc_cfg_v2_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->channel_cfg;
		if(1 == pMe->out_med_fmt_per_enc_cfg.num_channels)
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		}
		else
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_L;
			pMe->out_med_fmt_per_enc_cfg.chan_map[1] = PCM_CHANNEL_R;
		}

		MSG_5(MSG_SSID_QDSP6, DBG_MED_PRIO, "AAC encoder cfg block: SR %lu, ch %u. aac-fmt %u, bit-rate %lu, mode %lu",
				pMe->out_med_fmt_per_enc_cfg.sample_rate, pMe->out_med_fmt_per_enc_cfg.num_channels, pEncConfig->aac_fmt_flag, pEncConfig->bit_rate, pEncConfig->enc_mode);
		break;
	}
	case ASM_MEDIA_FMT_MP3:
	{
		asm_mp3_enc_cfg_t *pEncConfig = (asm_mp3_enc_cfg_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->channel_cfg;
		if(1 == pMe->out_med_fmt_per_enc_cfg.num_channels)
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		}
		else
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_L;
			pMe->out_med_fmt_per_enc_cfg.chan_map[1] = PCM_CHANNEL_R;
		}
		break;
	}
	case ASM_MEDIA_FMT_DTS:
	{
		asm_dts_enc_cfg_t *pEncConfig = (asm_dts_enc_cfg_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->num_channels;
		memscpy(pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
		break;
	}
	case ASM_MEDIA_FMT_EAC3:
	case ASM_MEDIA_FMT_AC3:
	{
		asm_ddp_enc_cfg_t *pEncConfig = (asm_ddp_enc_cfg_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->num_channels;
		memscpy(pMe->out_med_fmt_per_enc_cfg.chan_map,sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
		break;
	}

	case ASM_MEDIA_FMT_SBC:
	{
		asm_sbc_enc_cfg_t *pEncConfig = (asm_sbc_enc_cfg_t *) pConfigBlk ;
		uint16_t tNumChannel;
		if( ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO == pEncConfig->channel_mode )
		{
			tNumChannel = 1;
		}
		else if( ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO == pEncConfig->channel_mode
				|| ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO == pEncConfig->channel_mode
				|| ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO == pEncConfig->channel_mode )
		{
			tNumChannel = 2;
		}
		else
		{
			tNumChannel = 0; //native mode
		}
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = tNumChannel;
		if(1 == pMe->out_med_fmt_per_enc_cfg.num_channels)
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		}
		else
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_L;
			pMe->out_med_fmt_per_enc_cfg.chan_map[1] = PCM_CHANNEL_R;
		}

		MSG_6(MSG_SSID_QDSP6, DBG_MED_PRIO, "SBC encoder cfg block: SR %lu, ch %u. alloc-method %lu, bit-rate %lu, blklen %lu, subbands %lu",
				pMe->out_med_fmt_per_enc_cfg.sample_rate, pMe->out_med_fmt_per_enc_cfg.num_channels,
				pEncConfig->alloc_method, pEncConfig->bit_rate, pEncConfig->blk_len, pEncConfig->num_subbands);

		break;
	}
	case ASM_MEDIA_FMT_G711_ALAW_FS:
	case ASM_MEDIA_FMT_G711_MLAW_FS:
	{
		asm_g711_alaw_enc_cfg_t *pEncConfig = (asm_g711_alaw_enc_cfg_t*) pConfigBlk;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = 1;
		pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		break;
	}
	case ASM_MEDIA_FMT_AMRNB_FS:
	case ASM_MEDIA_FMT_EVRC_FS:
	case ASM_MEDIA_FMT_EVRCB_FS:
	case ASM_MEDIA_FMT_EVRCWB_FS:
	case ASM_MEDIA_FMT_FR_FS:
	case ASM_MEDIA_FMT_AMRWB_FS:
	case ASM_MEDIA_FMT_V13K_FS:
	{
		pMe->out_med_fmt_per_enc_cfg.sample_rate = 0;
		pMe->out_med_fmt_per_enc_cfg.num_channels = 0;
		pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		break;
	}
	case ASM_MEDIA_FMT_WMA_V8:
	{
		asm_wmastdv8_enc_cfg_t *pEncConfig = (asm_wmastdv8_enc_cfg_t *) pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig->channel_cfg;
		if(1 == pMe->out_med_fmt_per_enc_cfg.num_channels)
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_C;
		}
		else
		{
			pMe->out_med_fmt_per_enc_cfg.chan_map[0] = PCM_CHANNEL_L;
			pMe->out_med_fmt_per_enc_cfg.chan_map[1] = PCM_CHANNEL_R;
		}
		break;
	}
	default:
	{
		asm_custom_enc_cfg_t *pEncConfig = (asm_custom_enc_cfg_t *)pConfigBlk ;
		pMe->out_med_fmt_per_enc_cfg.sample_rate = pEncConfig->sample_rate;
		pMe->out_med_fmt_per_enc_cfg.num_channels = pEncConfig ->num_channels;
		MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "encoder cfg block: SR %lu, ch %u", pMe->out_med_fmt_per_enc_cfg.sample_rate, pMe->out_med_fmt_per_enc_cfg.num_channels);
		memscpy(pMe->out_med_fmt_per_enc_cfg.chan_map, sizeof(pMe->out_med_fmt_per_enc_cfg.chan_map), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
		break;
	}
	}

    return result;
}
