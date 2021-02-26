/** @file AudioDecSvc_CapiCallbackHandler.h
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_CapiV2CallbackHandler.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
02/05/2014   rbhatnk     Created file.

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
#include "shared_aud_cmn_lib.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

typedef struct AudioDecSvc_t AudioDecSvc_t;

typedef struct dec_CAPI_callback_obj_t
{
   AudioDecSvc_t *pMe;
   uint32_t capi_index;

} dec_CAPI_callback_obj_t;

capi_v2_event_callback_info_t audio_dec_svc_get_capi_v2_callback_handler(AudioDecSvc_t *pMe, uint32_t capi_count);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* CAPI_CALLBACK_HANDLER_H */
