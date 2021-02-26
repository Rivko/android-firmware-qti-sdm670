/*========================================================================
@file AudioDecSvcImc.h

This file contains declarations related to IMC for decoder

Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
====================================================================== */

/*========================================================================
 Edit History

 $Header:

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 02/07/18   npendeka      Created file.

 ========================================================================== */

#ifndef AUDIODECSVCIMC_H
#define AUDIODECSVCIMC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"

#include "AudioStreamMgr.h"
#include "adsp_asm_stream_commands.h"
#include "adsp_media_fmt.h"
#include "adsp_media_fmt_prop.h"
#include "Elite_lib_get_imc.h"

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */
typedef enum imc_src_dst_t
{
   IMC_SOURCE = 0,
   IMC_DESTINATION
}imc_src_dst_t;

/**< State information of the source or destination */
typedef enum imc_src_dst_state_t
{
   IMC_STATE_NONE = 0,
   IMC_STATE_REGISTERED,
   IMC_STATE_SOURCE_OPENED_COMMUNICATION,
   IMC_STATE_SOURCE_CLOSED_COMMUNICATION
}imc_src_dst_state_t;

/**< IMC source or destination information */
typedef struct imc_src_dst_info_t
{
   int8_t               key[16];            // key for IMC which has to match destination key
   uint32_t             key_length;         // length of the key.
   imc_src_dst_state_t  state;              // IMC state. Module knows if it is source or destination depending on the communication id.
   imc_dest_info_t      *dest_handle;       // destination handle for communication Id instance.
   imc_src_info_t       *source_handle;     // Source handle for communication Id instance. For a destination there can be multiple sources.
   uint8_t              src_frame_num;      // Number of frames source tries to connect with destination
}imc_src_dst_info_t;

/* -----------------------------------------------------------------------
 ** Function definitions
 ** ----------------------------------------------------------------------- */
ADSPResult AudioDecSvc_IMCCheckInit(AudioDecSvc_t* me_ptr, elite_msg_param_cal_t *param_msg_ptr);
void AudioDecSvc_IMCSendBuf(AudioDecSvc_t* me_ptr, fwk_extn_event_id_compr_over_pcm_bit_rate_level_t *data_ptr);
void AudioDecSvc_IMCDestroy(AudioDecSvc_t *me_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIODECSVCIMC_H
