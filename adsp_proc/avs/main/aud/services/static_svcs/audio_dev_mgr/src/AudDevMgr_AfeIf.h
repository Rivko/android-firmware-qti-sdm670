/**
@file AudDevMgr_AfeIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with an AFE Port.
*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_AfeIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
==========================================================================*/

#ifndef AUD_DEV_MGR_AFE_IF_H
#define AUD_DEV_MGR_AFE_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
  ADSPResult Adm_SendCmdConnectAFE(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                   int32_t port,
                                   elite_svc_handle_t *pCoppHandle,
                                   afe_client_data_path data_path,
                                   elite_svc_handle_t **pphAfe,
                                   uint32_t sample_rate,
                                   uint32_t unNumChannels,
                                   uint8_t *pChannelMapping,
                                   uint32_t unNumAfeFrames,
                                   uint32_t num_buffers,
                                   uint32_t cmd,
                                   uint32_t bit_width,
                                   volatile const afe_drift_info_t **ppAfeDriftPtr,
                                   volatile uint32_t *punAFEDelay,
                                   bool_t bShouldSubscribeToAVDrift);

ADSPResult Adm_SendCmdDisconnectAFE(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                           uint32_t port,
                                           elite_svc_handle_t *phCopp,
                                           uint32_t cmd);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_AFE_IF_H

