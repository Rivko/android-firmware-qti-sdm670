/**
@brief module manager AMDB iterface

 */

/*========================================================================

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
==========================================================================*/

/*-----------------------------------------------------------------------
  Copyright (c) 2016 QUALCOMM Technologies, Inc.  All Rights Reserved.
  QUALCOMM Technologies, Inc Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

-----------------------------------------------------------------------*/

#ifndef _MM_BUILT_IN_MODULES_H_
#define _MM_BUILT_IN_MODULES_H_

#include "adsp_amdb_api.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"
#include "adsp_vsm_api.h"
#include "adsp_vparams_api.h"

typedef struct mm_module_id_cmp_t
{
   uint32_t mtype;
   uint32_t id1;
   //uint32_t id2; cmp->compressed, to reduce space taken
} mm_module_id_cmp_t;

/**
 * modules in this list must be added to AMDB even if they are not mentioned in the ACDB file.
 * Only if they are marked as not-required in the ACDB file, we can STUB them
 *
 */
mm_module_id_cmp_t mm_backward_compatibility_list[] =
{
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_MP2 },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_ADPCM },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_EVRC_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_EVRC_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_EVRCB_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_V13K_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_AMRNB_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_AMRWB_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_EVRCWB_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_G711_ALAW_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_G711_MLAW_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_FR_FS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_AMR_WB_PLUS_V2 },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_DTMF },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_DTS },
      { AMDB_MODULE_TYPE_DECODER, ASM_MEDIA_FMT_DTS_LBR },

      { AMDB_MODULE_TYPE_PACKETIZER, ASM_MEDIA_FMT_AC3 },
      { AMDB_MODULE_TYPE_PACKETIZER, ASM_MEDIA_FMT_EAC3 },
      { AMDB_MODULE_TYPE_PACKETIZER, ASM_MEDIA_FMT_DTS },
      { AMDB_MODULE_TYPE_PACKETIZER, ASM_MEDIA_FMT_MAT },

      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_G711_ALAW_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_G711_MLAW_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_AMRNB_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_AMRWB_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_EVRC_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_EVRCB_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_EVRCWB_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_V13K_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_FR_FS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_DTS },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_AC3 },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_EAC3 },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_SBC },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_ADPCM },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_WMA_V8 },
      { AMDB_MODULE_TYPE_ENCODER, ASM_MEDIA_FMT_MP3 },

      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_13K_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_13K_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_EVRC_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_EVRC_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_4GV_NB_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_4GV_NB_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_4GV_WB_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_4GV_WB_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_4GV_NW_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_4GV_NW_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_AMR_NB_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_AMR_NB_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_AMR_WB_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_AMR_WB_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_EFR_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_EFR_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_FR_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_FR_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_HR_MODEM },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_HR_MODEM },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_PCM_8_KHZ },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_PCM_8_KHZ },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_PCM_16_KHZ },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_PCM_16_KHZ },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_G711_ALAW },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_G711_ALAW },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_G711_MLAW },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_G711_MLAW },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_G711_ALAW_V2 },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_G711_ALAW_V2 },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_G711_MLAW_V2 },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_G711_MLAW_V2 },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_G729AB },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_G729AB },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_AMR_NB_IF2 },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_AMR_NB_IF2 },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_4GV_NW },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_4GV_NW },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_EAMR },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_EAMR },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_PCM_48_KHZ },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_PCM_48_KHZ },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_PCM_32_KHZ },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_PCM_32_KHZ },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_EVRC_NW_2K },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_EVRC_NW_2K },
      { AMDB_MODULE_TYPE_ENCODER, VSM_MEDIA_TYPE_EVS },
      { AMDB_MODULE_TYPE_DECODER, VSM_MEDIA_TYPE_EVS },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_WV },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_WV_V2 },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_ST },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_FNS },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_ENC_CNG },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_TX_STREAM_LIMITER },
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_DTMF_DETECTION},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_HPCM_DEVICE_TX},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_HPCM_DEVICE_RX},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_HPCM_STREAM_TX},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_HPCM_STREAM_RX},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_RX_GAIN},
      { AMDB_MODULE_TYPE_GENERIC, VOICE_MODULE_FNS_V2}
};

#define MM_NUM_MOD_IN_BACKWARD_COMPAT_LIST (sizeof(mm_backward_compatibility_list)/sizeof(mm_backward_compatibility_list[0]))

#endif /* _MM_BUILT_IN_MODULES_H_ */
