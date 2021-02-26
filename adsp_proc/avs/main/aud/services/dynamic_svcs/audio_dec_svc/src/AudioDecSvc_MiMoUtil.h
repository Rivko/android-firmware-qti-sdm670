
/*========================================================================

This file contains declarations of Audio DecoderService MIMO utility functions.

Copyright (c) 2013-2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_MiMoUtil.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/14/13   rbhatnk   Created file.

========================================================================== */
#ifndef AUDIODECSVC_MIMO_UTIL_H
#define AUDIODECSVC_MIMO_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "Elite.h"
#include "qurt_elite.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"

#include "AudioDecSvc.h"
#include "AudioDecSvc_Structs.h"

AudioDecSvc_InpStream_t * AudioDecSvc_GetInputStreamById(AudioDecSvc_t *pMe, uint16_t stream_id);
AudioDecSvc_OutStream_t * AudioDecSvc_GetOutputStreamById(AudioDecSvc_t *pMe, uint16_t stream_id);
ADSPResult AudioDecSvc_OpenStream(AudioDecSvc_t *pMe);
ADSPResult AudioDecSvc_CloseStream(AudioDecSvc_t *pMe);

ADSPResult AudioDecSvc_CreateOutStream(AudioDecSvc_t *pMe, AudioDecSvcIOStreamInitParams_t *pIOInitParams, void **handle);
ADSPResult AudioDecSvc_CreateInpStream(AudioDecSvc_t *pMe, AudioDecSvcIOStreamInitParams_t *pIOInitParams, void **handle);
ADSPResult AudioDecSvc_DestroyOutStream(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t **ppOutStream);
ADSPResult AudioDecSvc_DestroyInpStream(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t **ppInpStream);
void AudioDecSvc_DestroyInputDataQ(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStream);
void AudioDecSvc_DestroyOutputDataQ (AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStream);
ADSPResult AudioDecSvc_CreateInpDataQ(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, dec_CAPI_container_t *last_capi);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDCMNUTIL_H

