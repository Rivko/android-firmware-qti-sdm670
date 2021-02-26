
/*========================================================================
ELite

This file contains service belonging to the voice encoder

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_enc/inc/venc_svc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
11/16/09   pgurram   Created file.

========================================================================== */
#ifndef VOICEENCCREATE_H
#define VOICEENCCREATE_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
//#include "VoiceSvc.h"  //common TTY definition, APR Info
#include "vsm_header_i.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/// Following are the custom message ID's for this service
enum
{
   VENC_GET_KPPS_CMD=0,                // msg ID for KPPS query.
   VENC_SET_MUTE_CMD,                  // msg ID for mute
   VENC_SET_TIMING_CMD,                // msg ID for timing cmd (first version)
   VENC_SET_TIMINGV2_CMD,              // msg ID for timing cmd (second version)
   VENC_REINIT_CMD,                    // msg ID for reinitializing dynamic service state/algorithms 
   VENC_STOP_RECORD_CMD,               // msg ID for stopping record
   VENC_START_RECORD_CMD_V2,           // msg ID for starting recrod
   VENC_CONFIG_HOST_PCM,               // msg ID for configuring host pcm
   VENC_SET_STREAM_PP_SAMP_RATE,       // msg ID for setting samp rate on stream pp
   VENC_SET_TTY_MODE,                  // msg ID for setting tty mode on tx path
   VENC_SET_LTETTY_MODE,               // msg ID for setting ltetty mode on tx path
   VENC_SET_PKT_EXCHANGE_MODE,         // msg ID for setting pkt exchange mode
   VENC_SET_OOB_PKT_EXCHANGE_CONFIG,   // msg ID for configuring OOB pkt exchange
   VENC_SET_TIMINGV3_CMD,              // msg ID for VFR/timing command (third version)
   VENC_GET_DELAY_CMD,                 // msg ID for querying delay
   VENC_REGISTER_EVENT,                // msg ID for registering for an event
   VENC_UNREGISTER_EVENT,              // msg ID for unregistering from an event
   VENC_SET_PARAM_V3,                  // msg ID for compressed set param
   VENC_SET_ENC_RATE,                  // msg ID for set rate command
   VENC_SET_VOC_PARAM,                 // msg ID for set vocoder property
   VENC_NUM_MSGS,                      // count of the supported messages.
};

/** This function creates an instance of the voice encoder. This function signature
    should be used by all services and objects, so that we can later adopt a
    registry that will enable link-time featurization.

    @param[in] inputParam - This can be any value or pointer to structure required
                            by service help set up the instance. In this service,
                            this input is not used.
    @param[in] aprInfo - pointer to apr information for process apr messages
    @param[in] ttyState - pointer to tty static memory
    @param[in] loopbackHandle - loopback handle for vsm
    @param[out] handle - pointer service handle
    @param[in] nSessionNum - session number
*/

ADSPResult venc_create (uint32_t input_param,
                          voice_strm_apr_info_t  *apr_info_ptr,
                          voice_strm_tty_state_t *tty_state_ptr,
                          elite_svc_handle_t *loopback_handle_ptr,
                          void **handle_ptr,uint32_t session_num);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif//#ifndef VOICEENCCREATE_H

