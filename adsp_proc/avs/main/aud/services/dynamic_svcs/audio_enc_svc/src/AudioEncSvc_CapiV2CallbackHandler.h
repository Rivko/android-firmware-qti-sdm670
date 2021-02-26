/** @file AudioEncSvc_CapiCallbackHandler.h
This file contains utility functions for Elite Audio Encoder service.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_enc_svc/src/AudioEncSvc_CapiV2CallbackHandler.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
07/22/2014   rbhatnk     Created file.

==========================================================================
*/
#ifndef CAPI_CALLBACK_HANDLER_H
#define CAPI_CALLBACK_HANDLER_H

#include "Elite.h"
#include "qurt_elite.h"
#include "Elite_CAPI_V2.h"
#include "adsp_media_fmt.h"
#include "AudioStreamMgr.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_private_api_ext.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef struct AudioEncSvc_t AudioEncSvc_t;

typedef struct enc_CAPI_callback_obj_t
{
   AudioEncSvc_t *pMe;
   uint32_t capi_index;

} enc_CAPI_callback_obj_t;

capi_v2_event_callback_info_t audio_enc_svc_get_capi_v2_callback_handler(AudioEncSvc_t *pMe, uint32_t capi_count);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_CALLBACK_HANDLER_H */
