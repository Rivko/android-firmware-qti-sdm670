/**
@file AudDevMgr_AsmIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with Audio Stream Manager.
*/

/*========================================================================
Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_AsmIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger.  
==========================================================================*/

#ifndef AUD_DEV_MGR_ASM_IF_H
#define AUD_DEV_MGR_ASM_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"

#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "EliteAprIf.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

ADSPResult Adm_SendCmdAsmSetOutputSamplingRate(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                               uint32_t unSampleRate,
                                               uint32_t session_id);

ADSPResult Adm_SendCmdAsmSetOutputMediaFormat(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                              uint16_t session_id,
                                              uint32_t sampling_rate,
                                              uint32_t num_channels,
                                              uint8_t *channel_mapping,
                                              uint32_t bits_per_sample);
								  
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_ASM_IF_H

