/** @file AudioEncSvc_CapiV2Util.h
This file contains utility functions for Elite Audio Encoder service.

Copyright (c) 2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_CapiV2Util.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/2014   rbhatnk     Created file.

==========================================================================
*/
#ifndef CAPI_V2_UTIL_H
#define CAPI_V2_UTIL_H

#include "AudioEncSvc_Includes.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define CAPI_INPUT_PORT  true
#define CAPI_OUTPUT_PORT false

#define AUD_ENC_SVC_SIZE_OF_STD_MEDIA_FMT (sizeof(capi_v2_set_get_media_format_t) + sizeof(capi_v2_standard_data_format_t))
#define AUD_ENC_SVC_MIN_SIZE_OF_RAW_MEDIA_FMT (sizeof(capi_v2_set_get_media_format_t) + sizeof(capi_v2_raw_compressed_data_format_t))

static inline uint32_t aud_enc_svc_media_fmt_size (data_format_t fmt_header)
{
   switch(fmt_header)
   {
   case CAPI_V2_FIXED_POINT:
   case CAPI_V2_IEC61937_PACKETIZED:
         return AUD_ENC_SVC_SIZE_OF_STD_MEDIA_FMT;
   case CAPI_V2_RAW_COMPRESSED:
         return AUD_ENC_SVC_MIN_SIZE_OF_RAW_MEDIA_FMT;
   default:
      return 0;
   }
   return 0;
}

ADSPResult audio_enc_svc_create_init_pack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_enc_svc_create_init_depack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_enc_svc_create_init_auto_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_enc_svc_create_init_cop_pack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioEncSvcInitParams_t *pInitParams, enc_CAPI_init_params_t *pCapiInitParams);

ADSPResult audio_enc_svc_get_init_time_properties(capi_v2_t *capi_v2_ptr, enc_CAPI_init_time_get_params_t *init_time_get_params, enc_CAPI_container_t *capi_container);

ADSPResult aud_enc_svc_get_port_thresh(capi_v2_t *capi_v2_ptr, bool_t is_input, uint16_t port_index, uint32_t *thresh);
ADSPResult aud_enc_svc_algorithmic_reset(capi_v2_t *capi_v2_ptr);

ADSPResult aud_enc_svc_set_in_sample_rate(capi_v2_t *capi_v2_ptr, uint16_t in_port_index, uint32_t sample_rate);
ADSPResult aud_enc_svc_set_media_fmt(capi_v2_t *capi_v2_ptr, audio_enc_svc_capi_v2_media_fmt_t *media_fmt, capi_v2_property_id_t prop_id);

ADSPResult aud_enc_svc_get_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size);
ADSPResult aud_enc_svc_set_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size);
enc_AMDB_presence audio_enc_svc_get_amdb_presence(Enc_CAPI_Type type, uint32_t id1, uint32_t id2, void **amdb_capi_handle_ptr);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_UTIL_H */
