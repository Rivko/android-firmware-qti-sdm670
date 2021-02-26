#ifndef VPTX_SVC_H
#define VPTX_SVC_H

/*========================================================================

*//** @file Vptx_Svc.h

Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file contains the header for Voice Proc Tx(VPTX) Dynamic service. The
VPTX dynamic service has one thread. It receives commands from Voice Proc
Manager (VPM). VPTX initializes the Pre-Processing algorithms and
processes the data for encoder stream service. Multiple VPTX can be
instantiated and each of them is identified by a session number provided
while creating VPTX.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/Vptx_Svc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/20/11   dc     Created file.

========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Vptx_Modules.h"

#define VOICE_PARAM_LOOPBACK_ENABLE (0x00010E18)

// Keep some additional margin over and above the time taken for processing. This is needed for AFE delivery jitters and sample slip
#define VPTX_PROCESSING_MARGIN_IN_US 1500

/* -----------------------------------------------------------------------
** Function prototypes
** ----------------------------------------------------------------------- */
ADSPResult vptx_write_output(vptx_t* pVptx, int8_t* pProcessedData, int32_t nSamples);
void vptx_circ_buf_init(vptx_t* pVptx, uint32_t nNearCircBufSamples, uint32_t nFarCircBufSamples, uint32_t nOutCircBufSamples, uint32_t nLoopCircBufSamples);
int32_t vptx_send_dtmf_tone_status(vptx_t* pVptx, void* data_ptr);
int32_t vptx_send_hpcm_buf_available_evt(vptx_t* pVptx, void* data_ptr);
int32_t vptx_send_tx_speech_activity_status(vptx_t* pVptx, void* data_ptr);
int16_t voice_get_hpcm_phase(vptx_t* pVptx);
void vptx_cal_drift(vptx_t *pVptx,int32_t *inst_drift); // extract and accumulate drift info with av/hp timer/vfr
void vptx_cal_sample_slip_stuff(int16_t ss_num_samp_adj,         // in:     number of samples to be adjusted
                                int32_t prebuffer_samples,       // in:     number of pre-buffer samples for sample slipping
                                int32_t* ss_buf_samples_ptr,     // out:    updates the number of pre-buffer samples left in the pool (for sample slipping)
                                uint32_t session_num,            // in:     vptx session number for debug messages
                                near_far_t near_far_path);       // in:     near or far path

ADSPResult vptx_setup_sns_qmi(vptx_t *pVptx, uint32_t qmi_reg_mask, uint32_t qmi_ind_mask);
void vptx_destroy_sns_qmi(vptx_t *pVptx);
void vptx_handle_sns_qmi_ind(vptx_t *pVptx);
ADSPResult vptx_register_sns_qmi(vptx_t *pVptx);
ADSPResult vptx_deregister_sns_qmi(vptx_t *pVptx);
void vptx_clear_sns_qmi_signal(vptx_t *pVptx);
void vptx_check_set_internal_param(vptx_t *pVptx);

bool_t vptx_is_custom_topology(uint32_t topology_id);
ADSPResult vptx_validate_sample_rate_for_static_topology(uint32_t topology_id, uint32_t sample_rate);
void vptx_set_media_type_for_static_topology(vptx_t *pVptx);

#endif//#ifndef VPTX_SVC_H
