
/*========================================================================
Elite

This file contains an example service belonging to the Elite framework.

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_dec/inc/VoiceDec.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/09   dc      Created file.

========================================================================== */
#ifndef VOICEDECCREATE_H
#define VOICEDECCREATE_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "qurt_elite.h"
#include "Elite.h"
#include "vsm_header_i.h" //common TTY definition, APR info

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/// Following are the custom message ID's for this service

enum
{
   VDEC_GET_KPPS_CMD=0,                  // msg ID for KPPS query.
   VDEC_SET_MUTE_CMD,                    // msg ID for mute command
   VDEC_SET_TIMING_CMD,                  // msg ID for VFR/timing command (first version)  -- VOICE_CMD_SET_TIMING_PARAMS
   VDEC_SET_TIMINGV2_CMD,                // msg ID for VFR/timing command (second version) -- VSM_CMD_SET_TIMING_PARAMS
   VDEC_REINIT_CMD,                      // msg ID for reinitializing dynamic service state/algorithms
   VDEC_STOP_RECORD_CMD,                 // msg ID for stop record
   VDEC_START_RECORD_CMD_V2,             // msg ID for start record
   VDEC_CONFIG_HOST_PCM,                 // msg ID for configuring host pcm
   VDEC_SET_TIMINGV3_CMD,                // msg ID for VFR/timing command (third version) -- VSM_CMD_SET_TIMING_PARAMS_V2
   VDEC_GET_DELAY_CMD,                   // msg ID for delay query.
   VDEC_SET_PARAM_V3,                    // msg ID for compressed set param
   VDEC_SET_STREAM_PP_SAMP_RATE,         // msg ID for setting samp rate on stream pp
   VDEC_SET_TTY_MODE,                    // msg ID for tty mode on rx path
   VDEC_SET_LTETTY_MODE,                 // msg ID for ltetty mode on rx path
   VDEC_SET_PKT_EXCHANGE_MODE,           // msg ID for setting pkt exchange mode
   VDEC_SET_OOB_PKT_EXCHANGE_CONFIG,     // msg ID for configuring OOB pkt exchange
   VDEC_REGISTER_EVENT,                  // msg ID for registering for an event
   VDEC_UNREGISTER_EVENT,                // msg ID for unregistering from an event
   VDEC_SET_ENC_RATE,                    // msg ID for setting enc rate
   VDEC_SET_VOC_PARAM,                   // msg ID for setting vocoder property
   VDEC_NUM_MSGS                         // count of the supported messages.
};


/**
* This function creates an instance of the Elite example service. This
* function signature should be used by all services and objects, so that
* we can later adopt a registry that will enable link-time featurization.
*
* @param[in] inputParam This can be any value or pointer to structure required
*    by service help set up the instance. In the example service, this input is
*    not used.
* @param[in] aprInfo This info is to send/process apr messages with client for
*    voc packet exchange
* @param[in] ttyState This is a common structure used in Tx and Rx for TTY
*    information.
* @param[in] loopbackEnable This flag is for internal loopback mode. It will
*    prevent sending dec pkt requests to modem.
* @param[in] nSessionNum. This is the session number
* @param[out] handle This will be the service entry handle returned to the caller.
* @return Success/failure of instance creation.
*/
ADSPResult vdec_create (uint32_t input_param,
                          voice_strm_apr_info_t  *apr_info_ptr,
                          voice_strm_tty_state_t *tty_state_ptr,
                          void **handle_ptr,
                          uint32_t session_num);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif//#ifndef VOICEDECCREATE_H

