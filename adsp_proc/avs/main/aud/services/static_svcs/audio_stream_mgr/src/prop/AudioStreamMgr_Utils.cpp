/**
@file AudStreamMgr_Utils.cpp

@brief This file contains the implementation of utility functions for AudioStreamMgr.

 */

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/23/16   HB		Create File for proprietary information.

==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/
#include "AudioStreamMgr_Util.h"
#include "EliteMsg_AdmCustom.h"
#include "adsp_internal_api.h"

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

ADSPResult AudioStreamMgr_GetProcInitParamFromNonPcmEncConfig(
      PPOutputParams_t *pParam,
      uint32_t unFmtBlkId,
      void     *pFrmtBlk
)
{
   switch ( unFmtBlkId)
   {
   case ASM_MEDIA_FMT_ADPCM:
   {
      asm_adpcm_fmt_blk_t *pEncConfig = (asm_adpcm_fmt_blk_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->num_channels;
      if(1 == pParam->ulNumChannels)
      {
         pParam->channelMap[0] = PCM_CHANNEL_C;
      }
      else
      {
         pParam->channelMap[0] = PCM_CHANNEL_L;
         pParam->channelMap[1] = PCM_CHANNEL_R;
      }
      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_AAC_V2:
   {
      asm_aac_enc_cfg_v2_t *pEncConfig = (asm_aac_enc_cfg_v2_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->channel_cfg;
      if(1 == pParam->ulNumChannels)
      {
         pParam->channelMap[0] = PCM_CHANNEL_C;
      }
      else
      {
         pParam->channelMap[0] = PCM_CHANNEL_L;
         pParam->channelMap[1] = PCM_CHANNEL_R;
      }
      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_MP3:
   {
      asm_mp3_enc_cfg_t *pEncConfig = (asm_mp3_enc_cfg_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->channel_cfg;
      if(1 == pParam->ulNumChannels)
      {
         pParam->channelMap[0] = PCM_CHANNEL_C;
      }
      else
      {
         pParam->channelMap[0] = PCM_CHANNEL_L;
         pParam->channelMap[1] = PCM_CHANNEL_R;
      }
      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_DTS:
   {
      asm_dts_enc_cfg_t *pEncConfig = (asm_dts_enc_cfg_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->num_channels;
      memscpy(pParam->channelMap, sizeof(pParam->channelMap), pEncConfig->channel_mapping, 8*sizeof(uint8_t));

      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_EAC3:
   case ASM_MEDIA_FMT_AC3:
   {
      asm_ddp_enc_cfg_t *pEncConfig = (asm_ddp_enc_cfg_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->num_channels;
      memscpy(pParam->channelMap,sizeof(pParam->channelMap), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
      return ADSP_EOK;
   }

   case ASM_MEDIA_FMT_SBC:
   {
      asm_sbc_enc_cfg_t *pEncConfig = (asm_sbc_enc_cfg_t *) pFrmtBlk ;
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
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = tNumChannel;
      if(1 == pParam->ulNumChannels)
      {
         pParam->channelMap[0] = PCM_CHANNEL_C;
      }
      else
      {
         pParam->channelMap[0] = PCM_CHANNEL_L;
         pParam->channelMap[1] = PCM_CHANNEL_R;
      }
      return ADSP_EOK;
   }

   case ASM_MEDIA_FMT_G711_ALAW_FS:
   case ASM_MEDIA_FMT_G711_MLAW_FS:
   {
      asm_g711_alaw_enc_cfg_t *pEncConfig = (asm_g711_alaw_enc_cfg_t*) pFrmtBlk;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = 1;
      pParam->channelMap[0] = PCM_CHANNEL_C;
      return ADSP_EOK;
   }

   case ASM_MEDIA_FMT_AMRNB_FS:
   case ASM_MEDIA_FMT_EVRC_FS:
   case ASM_MEDIA_FMT_EVRCB_FS:
   case ASM_MEDIA_FMT_V13K_FS:
   case ASM_MEDIA_FMT_FR_FS:
   {
      pParam->ulSampleRate = 8000;
      pParam->ulNumChannels = 1;
      pParam->channelMap[0] = PCM_CHANNEL_C;
      return ADSP_EOK;
   }

   case ASM_MEDIA_FMT_AMRWB_FS:
   case ASM_MEDIA_FMT_EVRCWB_FS:
   {
      pParam->ulSampleRate = 16000;
      pParam->ulNumChannels = 1;
      pParam->channelMap[0] = PCM_CHANNEL_C;
      return ADSP_EOK;
   }
   case ASM_MEDIA_FMT_WMA_V8:
   {
      asm_wmastdv8_enc_cfg_t *pEncConfig = (asm_wmastdv8_enc_cfg_t *) pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig->channel_cfg;
      if(1 == pParam->ulNumChannels)
      {
         pParam->channelMap[0] = PCM_CHANNEL_C;
      }
      else
      {
         pParam->channelMap[0] = PCM_CHANNEL_L;
         pParam->channelMap[1] = PCM_CHANNEL_R;
      }
      return ADSP_EOK;
   }

   default:
   {
      asm_custom_enc_cfg_t *pEncConfig = (asm_custom_enc_cfg_t *)pFrmtBlk ;
      pParam->ulSampleRate = pEncConfig->sample_rate;
      pParam->ulNumChannels = pEncConfig ->num_channels;
      memscpy(pParam->channelMap, sizeof(pParam->channelMap), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
      return ADSP_EOK;
   }
   }
   return ADSP_EOK;
}


ADSPResult AudioStreamMgr_GetMtxConfigParamFromNonPcmEncConfig(
                                                void     *pParam,
                                                uint32_t unFmtBlkId,
                                                void     *pFrmtBlk
                                                )
{
	EliteMsg_CustomAdmStreamMtMxReConfig *config_ports_msg_ptr = (EliteMsg_CustomAdmStreamMtMxReConfig *)pParam;

    switch(unFmtBlkId)
    {
    case ASM_MEDIA_FMT_G711_ALAW_FS:
    case ASM_MEDIA_FMT_G711_MLAW_FS:
    case ASM_MEDIA_FMT_AMRNB_FS:
    case ASM_MEDIA_FMT_AMRWB_FS:
    case ASM_MEDIA_FMT_EVRC_FS:
    case ASM_MEDIA_FMT_EVRCB_FS:
    case ASM_MEDIA_FMT_EVRCWB_FS:
    case ASM_MEDIA_FMT_V13K_FS:
    case ASM_MEDIA_FMT_FR_FS:
    {
       config_ports_msg_ptr->num_channels = 1;
       config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = FALSE;
       break;
    }
    case ASM_MEDIA_FMT_MP3:
    {
       asm_mp3_enc_cfg_t *pEncConfig = (asm_mp3_enc_cfg_t *) pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig->channel_cfg;
       if(1 == config_ports_msg_ptr->num_channels)
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       }
       else
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_L;
          config_ports_msg_ptr->channel_mapping[1] = PCM_CHANNEL_R;
       }
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->channel_cfg == 0)? TRUE : FALSE;
       break;
    }
    case ASM_MEDIA_FMT_AAC_V2:
    {
       asm_aac_enc_cfg_v2_t *pEncConfig = (asm_aac_enc_cfg_v2_t *) pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig->channel_cfg;
       if(1 == config_ports_msg_ptr->num_channels)
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       }
       else
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_L;
          config_ports_msg_ptr->channel_mapping[1] = PCM_CHANNEL_R;
       }
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->channel_cfg == 0)? TRUE : FALSE;
       break;
    }
    case ASM_MEDIA_FMT_DTS:
    {
       asm_dts_enc_cfg_t *pEncConfig = (asm_dts_enc_cfg_t *) pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig->num_channels;
       memscpy(config_ports_msg_ptr->channel_mapping, sizeof(config_ports_msg_ptr->channel_mapping), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->num_channels == 0)? TRUE : FALSE;
       break;
    }

    case ASM_MEDIA_FMT_SBC:
    {
       asm_sbc_enc_cfg_t *pEncConfig = (asm_sbc_enc_cfg_t *) pFrmtBlk ;
       if(pEncConfig->channel_mode > 2)
       {
          config_ports_msg_ptr->num_channels = 2;
       }
       else
       {
          config_ports_msg_ptr->num_channels = pEncConfig->channel_mode;
       }
       if(1 == config_ports_msg_ptr->num_channels)
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       }
       else
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_L;
          config_ports_msg_ptr->channel_mapping[1] = PCM_CHANNEL_R;
       }
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->channel_mode == 0)? TRUE : FALSE;
       break;
    }
    case ASM_MEDIA_FMT_ADPCM:
    {
       asm_adpcm_fmt_blk_t *pEncConfig = (asm_adpcm_fmt_blk_t *) pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig->num_channels;
       if(1 == config_ports_msg_ptr->num_channels)
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       }
       else
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_L;
          config_ports_msg_ptr->channel_mapping[1] = PCM_CHANNEL_R;
       }
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->num_channels == 0)? TRUE : FALSE;
       break;
    }
    case ASM_MEDIA_FMT_WMA_V8:
    {
       asm_wmastdv8_enc_cfg_t *pEncConfig = (asm_wmastdv8_enc_cfg_t *) pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig->channel_cfg;
       if(1 == config_ports_msg_ptr->num_channels)
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_C;
       }
       else
       {
          config_ports_msg_ptr->channel_mapping[0] = PCM_CHANNEL_L;
          config_ports_msg_ptr->channel_mapping[1] = PCM_CHANNEL_R;
       }
       config_ports_msg_ptr->bMtMxPassNativeNumChannels = (pEncConfig->channel_cfg == 0)? TRUE : FALSE;
       break;
    }

    default:
    {
       asm_custom_enc_cfg_t *pEncConfig = (asm_custom_enc_cfg_t *)pFrmtBlk ;
       config_ports_msg_ptr->num_channels = pEncConfig ->num_channels;
       memscpy(config_ports_msg_ptr->channel_mapping, sizeof(config_ports_msg_ptr->channel_mapping), pEncConfig->channel_mapping, 8*sizeof(uint8_t));
       break;
    }
    }

	return ADSP_EOK;
}

void AudioStreamMgr_GetASMNodeForEncDecParam(uint32_t paramId, ASM_GraphNodeIndex *pNodeIndex)
{
	switch (paramId)
	{
    case ASM_PARAM_ID_AAC_SBR_PS_FLAG:
    case ASM_PARAM_ID_REG_DEC_ERR_EVENT:
    case ASM_PARAM_ID_DEC_CONFIGURE_EARLY_EOS_NOTIFICATION:
    case ASM_PARAM_ID_AAC_DUAL_MONO_MAPPING:
    case ASM_PARAM_ID_AC3_KARAOKE_MODE:
    case ASM_PARAM_ID_AC3_DRC_MODE:
    case ASM_PARAM_ID_AC3_DUAL_MONO_MODE:
    case ASM_PARAM_ID_AC3_STEREO_DOWNMIX_MODE:
    case ASM_PARAM_ID_AC3_PCM_SCALEFACTOR:
    case ASM_PARAM_ID_AC3_DRC_BOOST_SCALEFACTOR:
    case ASM_PARAM_ID_AC3_DRC_CUT_SCALEFACTOR:
    case ASM_PARAM_ID_AC3_NUM_OUTPUT_CHANNELS:
    case ASM_PARAM_ID_AC3_ERROR_CONCEAL:
    case ASM_PARAM_ID_AC3_ERROR_MAX_RPTS:
    case ASM_PARAM_ID_AC3_CNV_ERROR_CONCEAL:
    case ASM_PARAM_ID_AC3_SUBSTREAM_SELECT:
    case ASM_PARAM_ID_AC3_MDCT_BANDLIMITING_MODE:
    case ASM_PARAM_ID_AC3_DRC_SUPPRESSION_MODE:
    case ASM_PARAM_ID_AC3_CHANNEL_CONFIG:
    case ASM_PARAM_ID_AC3_LFE_MODE:
    case ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP:
    case ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT:
    case ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX:
    case ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT:
    case ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX:
    case ASM_PARAM_ID_AAC_STEREO_MIX_COEFF_SELECTION_FLAG:
    case ASM_PARAM_ID_HEAAC_ERROR_CONCEAL:
    case ASM_PARAM_ID_HEAAC_2TO1_RESAMPLING:
    case ASM_PARAM_ID_HEAAC_REPLAY_GAIN:
    case ASM_PARAM_ID_HEAAC_DEFAULT_PRL:
    case ASM_PARAM_ID_HEAAC_DRC_TYPE_MODE:
    case ASM_PARAM_ID_HEAAC_EXT_BOOST:
    case ASM_PARAM_ID_HEAAC_EXT_METADATA:
    case ASM_PARAM_ID_HEAAC_NUM_OUTPUT_CHANNELS:
    case ASM_PARAM_ID_AC3_ACMOD_CHANGES_STATE:
	{
		*pNodeIndex = ASM_NODE_DEC;
		break;
	}
    case ASM_PARAM_ID_DDP_ENC_DATA_RATE:
    case ASM_PARAM_ID_DDP_ENC_LFE:
    case ASM_PARAM_ID_DDP_ENC_PH90_FILT:
    case ASM_PARAM_ID_DDP_ENC_GBL_DRC_PROF:
    case ASM_PARAM_ID_DDP_ENC_DIAL_NORM:
	{
		*pNodeIndex = ASM_NODE_ENC;
		break;
	}
	default:
	{
		*pNodeIndex = ASM_NODE_MAX;
		break;
	}
	}

}
