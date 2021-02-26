#ifndef VPRX_SVC_H
#define VPRX_SVC_H

/*========================================================================

*//** @file Vprx_Svc.h

Copyright (c) 2011-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the header for Voice Proc Rx(VPRX) Dynamic service. The
VPRX dynamic service has one thread. It receives commands from Voice Proc
Manager (VPM). VPRX initializes the Pre-Processing algorithms and
processes the data for encoder stream service. Multiple VPRX can be
instantiated and each of them is identified by a session number provided
while creating VPRX.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_rx/src/Vprx_Svc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/25/11   ss     Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Vprx_Modules.h"

#define VOICE_PARAM_LOOPBACK_ENABLE (0x00010E18)
// Keep some additional margin over and above the time taken for processing. This is needed for jitters and sample slip
#define VPRX_PROCESSING_MARGIN_IN_US 1500


/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
void vprx_cal_drift(vprx_t * pVprx,int32_t *inst_drift);
void vprx_cal_sample_slip_stuff(vprx_t* pVprx, int16_t* nSlipStuffSamples);
int32_t vprx_send_hpcm_buf_available_evt(vprx_t* pVprx, void* data_ptr);
ADSPResult vprx_update_io_stream_data(vprx_t *pVprx, uint32_t start_module_index);
bool_t vprx_is_custom_topology(uint32_t topology_id);

#endif//#ifndef VPRX_SVC_H
