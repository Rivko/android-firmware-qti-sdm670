
/*========================================================================

This file contains declarations of Audio DecoderService utility functions.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/08/13   rbhatnk   Created file.

========================================================================== */
#ifndef AUDIODECSVC_PULLMODE_H_
#define AUDIODECSVC_PULLMODE_H_

#include "Elite.h"
#include "qurt_elite.h"
#include "adsp_asm_stream_commands.h"
#include "avtimer_drv_api.h"
#include "adsp_media_fmt.h"
#include "adsp_asm_data_commands.h"
#include "AudioDecSvc_Structs.h"
#include "AudioDecSvc_Util.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

struct dec_pull_mode_t
{
   uint8_t                               *shared_circ_buf_start_ptr;
   uint32_t                               shared_circ_buf_size;
   asm_pull_mode_write_position_buffer_t *pos_buf_ptr;
   avtimer_drv_handle_t                   avt_drv_handle;           // handle of avtimer
   uint32_t                               num_water_mark_levels;
   asm_pull_mode_watermark_level_t       *water_mark_levels_ptr;
};

ADSPResult AudioDecSvc_InitPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm, AudioDecSvcIOStreamInitParams_t *pIOInitParams);
void AudioDecSvc_DeInitPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);
ADSPResult AudioDecSvc_SuspendPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);
ADSPResult AudioDecSvc_ResumePullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);
ADSPResult AudioDecSvc_ProcessOutputDataQ_PCMPullMode(AudioDecSvc_t *pMe, AudioDecSvc_OutStream_t *pOutStrm);
void AudioDecSvc_ResetPullMode(AudioDecSvc_t *pMe, AudioDecSvc_InpStream_t *pInpStrm);

#ifdef __cplusplus
} //extern "C"
#endif //__cplusplus

#endif /* AUDIODECSVC_PULLMODE_H_ */
