/** @file AudioDecSvc_CapiV2Util.h
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_CapiV2Util.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/2014   rbhatnk     Created file.

==========================================================================
*/
#ifndef CAPI_V2_UTIL_H
#define CAPI_V2_UTIL_H

#include "AudioDecSvc_Structs.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define CAPI_INPUT_PORT  true
#define CAPI_OUTPUT_PORT false

#define AUD_DEC_SVC_SIZE_OF_STD_MEDIA_FMT (sizeof(capi_v2_set_get_media_format_t) + sizeof(capi_v2_standard_data_format_t))
#define AUD_DEC_SVC_MIN_SIZE_OF_RAW_MEDIA_FMT (sizeof(capi_v2_set_get_media_format_t) + sizeof(capi_v2_raw_compressed_data_format_t))

static inline uint32_t aud_dec_svc_media_fmt_size (data_format_t fmt_header)
{
   switch(fmt_header)
   {
   case CAPI_V2_FIXED_POINT:
   case CAPI_V2_IEC61937_PACKETIZED:
   case CAPI_V2_DSD_DOP_PACKETIZED:
   case CAPI_V2_GENERIC_COMPRESSED:
         return AUD_DEC_SVC_SIZE_OF_STD_MEDIA_FMT;
   case CAPI_V2_RAW_COMPRESSED:
         return AUD_DEC_SVC_MIN_SIZE_OF_RAW_MEDIA_FMT;
   default:
      return 0;
   }
   return 0;
}

typedef struct audio_dec_svc_capi_v2_media_fmt_t
{
   capi_v2_set_get_media_format_t main;
   union
   {
      capi_v2_standard_data_format_t std;
      capi_v2_raw_compressed_data_format_t raw_fmt;
   };
} audio_dec_svc_capi_v2_media_fmt_t;

ADSPResult audio_dec_svc_create_init_pack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioDecSvcInitParams_t *pInitParams, dec_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_dec_svc_create_init_passthru_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioDecSvcInitParams_t *pInitParams, dec_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_dec_svc_create_init_conv_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioDecSvcInitParams_t *pInitParams, dec_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_dec_svc_create_init_depack_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioDecSvcInitParams_t *pInitParams, dec_CAPI_init_params_t *pCapiInitParams);
ADSPResult audio_dec_svc_create_init_auto_capi_v2(capi_v2_t **capi_v2_ptr_ptr,  AudioDecSvcInitParams_t *pInitParams, dec_CAPI_init_params_t *pCapiInitParams);

ADSPResult audio_dec_svc_get_init_time_properties(capi_v2_t *capi_v2_ptr, dec_CAPI_init_time_get_params_t *init_time_get_params, dec_CAPI_container_t *capi_container);

ADSPResult aud_dec_svc_get_max_metadata_size(capi_v2_t *capi_v2_ptr,  uint32_t *metadata_size);
ADSPResult aud_dec_svc_get_metadata(capi_v2_t *capi_v2_ptr, uint32_t out_port_index, uint8_t *buf_ptr, uint32_t *buf_size);
ADSPResult aud_dec_svc_get_port_thresh(capi_v2_t *capi_v2_ptr, bool_t is_input, uint16_t port_index, uint32_t *thresh);
ADSPResult aud_dec_svc_set_pcm_out_bits_per_sample(capi_v2_t *capi_v2_ptr, uint16_t out_port_index, uint16_t bits_per_sample);
ADSPResult aud_dec_svc_algorithmic_reset(capi_v2_t *capi_v2_ptr, uint32_t input_port_index);
ADSPResult aud_dec_svc_set_pcm_out_chan_map(capi_v2_t *capi_v2_ptr, uint16_t out_port_index, DecChannelMap_t *chan_map);

ADSPResult aud_dec_svc_set_passthru_sampling_rate(capi_v2_t *capi_v2_ptr, uint16_t in_port_index, uint32_t sample_rate,uint32_t num_channels);
ADSPResult aud_dec_svc_set_in_media_fmt_raw_compr(capi_v2_t *capi_v2_ptr, uint16_t in_port_index,
                                             uint8_t *media_fmt_ptr, uint32_t media_fmt_size,
                                             uint32_t fmt_id, int8_t *buf_ptr, uint32_t buf_size);

ADSPResult aud_dec_svc_get_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size);
ADSPResult aud_dec_svc_set_param(capi_v2_t *capi_v2_ptr, uint32_t param_id, int8_t *payload, uint32_t size);
ADSPResult aud_dec_svc_reg_dereg_apr_events(capi_v2_t *capi_v2_ptr, uint32_t event_id, bool_t is_reg);
dec_AMDB_presence audio_dec_svc_get_amdb_presence(Dec_CAPI_Type type, uint32_t id1, uint32_t id2, void **amdb_capi_handle_ptr);
void audio_dec_svc_release_amdb_handle(void *amdb_handle);
void audio_dec_svc_query_dl_info(void *amdb_handle, bool_t *is_dl, uint32_t **start_addr, uint32_t *so_size);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_V2_UTIL_H */
