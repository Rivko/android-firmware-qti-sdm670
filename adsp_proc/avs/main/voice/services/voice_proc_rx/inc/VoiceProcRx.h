
/*========================================================================

@file VoiceProcRx.h
@brief This file provides interfaces to create and control an instance of the
voice proc rx dynamic service

Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_rx/inc/VoiceProcRx.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/11/10   ksa     Added doxygen markup
10/29/09   dc      Created file.

========================================================================== */
#ifndef VOICEPROCRX_H
#define VOICEPROCRX_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

// 20ms input frame length
#define VOICE_RX_INPUT_FRAME_LEN  20

//SA TODO: Use same structure for Tx and Rx
typedef struct vprx_create_params_t
{
   uint16_t afe_port_id;         // afe_port_id ID of Rx AFE port to connect to
   uint32_t topology_id;         // topology_id Topology ID to be used
   uint32_t sampling_rate;       // sampling_rate Sampling rate to run the voice proc at
   uint32_t session_num;         // session_num VPM session number that this instance belongs to
   uint16_t num_channels;        // topology_id Topology ID to be used
   uint32_t shared_mem_client;   // shared_mem_client Client to use when converting physical
   uint8_t  channel_map[8];      // channel mapping information for VPRx
} vprx_create_params_t;

/** Create an instance of the voiceproc rx service
    @param [out] handle Return handle to the created instance
    @param [in] vprx_create_params_t - create params
    @return An indication of success/failure
*/
ADSPResult vprx_create (void **handle,
      vprx_create_params_t *create_param_ptr);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif//#ifndef VOICEPROCRX_H

