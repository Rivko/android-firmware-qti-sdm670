/*========================================================================

*//** @file AudioEncSvc_PushMode.cpp
This file contains Elite Audio encoder service include components.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header:


when       who     what, where, why
--------   ---     -------------------------------------------------------
01/09/13   RB      Created file.

========================================================================== */

#ifndef AUDIOENCSVC_PUSHMODE_H_
#define AUDIOENCSVC_PUSHMODE_H_

#include "Elite.h"
#include "qurt_elite.h"
#include "adsp_asm_stream_commands.h"
#include "avtimer_drv_api.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"
#include "AudioEncSvc_Includes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

struct enc_push_mode_t
{
   uint8_t *shared_circ_buf_start_ptr;
   uint32_t shared_circ_buf_size;
   asm_push_mode_read_position_buffer_t  *pos_buf_ptr;
   avtimer_drv_handle_t                   avt_drv_handle;           // handle of avtimer
   uint32_t                               num_water_mark_levels;
   asm_push_mode_watermark_level_t       *water_mark_levels_ptr;
};

ADSPResult AudioEncSvc_InitPushMode(AudioEncSvc_t *pMe, AudioEncSvcInitParams_t *pInitParams);
void AudioEncSvc_DeInitPushMode(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_SuspendPushMode(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_ResumePushMode(AudioEncSvc_t *pMe);
ADSPResult AudioEncSvc_ProcessPCMPushMode(AudioEncSvc_t* pMe);
void AudioEncSvc_ResetPushMode(AudioEncSvc_t *pMe);

#ifdef __cplusplus
} //extern "C"
#endif //__cplusplus

#endif /* AUDIOENCSVC_PUSHMODE_H_ */
