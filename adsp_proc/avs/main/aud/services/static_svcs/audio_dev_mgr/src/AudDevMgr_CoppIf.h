/**
@file AudDevMgr_CoppIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with COPP.
*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_CoppIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger. 
==========================================================================*/

#ifndef AUD_DEV_MGR_COPP_IF_H
#define AUD_DEV_MGR_COPP_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"

#include "AudDynaPPSvc.h"
#include "capi_v2_sample_slip.h"
#include "AudDevMgr.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/
typedef struct
{
    uint32_t*    pAddr;                   /**< Physical memory address. This field is NULL. */
	uint16_t     data_payload_size; /** Common data payload size for entire structure. */
    uint32_t     unModuleId1;         /**< ID of the module. */
    uint32_t     unParamId1;          /**< ID of the parameter. */
    uint16_t     usParamSize1;       /**< Data size of this parameter ID/module ID combination. */
    uint16_t     usReserved1;         /**< Reserved for 32-bit alignment. */
	struct audproc_sampleslip_drift_pointer_t sCoppDriftPtrStruct;
	                                              /**< Parameter is a structure which contains the drift pointer 
												         information to be passed on to the COPP. */
    uint32_t     unModuleId2;         /**< ID of the module. */
    uint32_t     unParamId2;          /**< ID of the parameter. */
    uint16_t     usParamSize2;       /**< Data size of this parameter ID/module ID combination. */
    uint16_t     usReserved2;         /**< Reserved for 32-bit alignment. */
	struct audproc_sampleslip_enable_t sCoppSampleSlipEnableStruct;
	                                              /**< Parameter is a structure which contains the sample slip enable
											              information to be	 passed on to the COPP. */
} adm_copp_cal_drift_ptr_param_data_t;

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
ADSPResult AdmSendCoppMsg(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ, elite_svc_handle_t *pAfeHandle, uint32_t cmd);
ADSPResult Adm_SendCmdRunCopp(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ);
ADSPResult Adm_SendCmdCoppSetParams(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ, uint32_t opcode, 
									                            int32_t *pSetParamsCmdBase,  uint16_t unDataPayloadSize, uint32_t calVersion);
void Adm_SendCmdCoppSetParamsDriftPtrSetup(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t unCoppID, uint16_t unPrimaryDevice);

ADSPResult Adm_SendCmdStartCopp(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ,   
					void *pAfeDriftPtr, void *pAfeDriftPtr_EP2);

ADSPResult adm_config_pp_output_media_format(elite_svc_handle_t *phCopp,
                                             qurt_elite_queue_t *responseQ,
                                             uint32_t unNumChannels,
                                             uint8_t *pChannelMapping,
                                             uint32_t unSampleRate,
                                             uint32_t unBitsPerSample);
ADSPResult Adm_SendCmdCoppSetOutputSamplingRate(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ, uint32_t unSampleRate);

ADSPResult Adm_FlushCopp(AdmStatSvc_InfoType *pAdmStatSvc, AdmCalPort_Type *phCalPort, uint16_t unCoppID);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_COPP_IF_H
