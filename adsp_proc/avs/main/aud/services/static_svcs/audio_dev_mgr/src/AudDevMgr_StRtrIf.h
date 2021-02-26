/**
@file AudioDevMgr_StRtrIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with Stream Router Service.
*/

/*========================================================================
Copyright (c) 2012-2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_StRtrIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger. 
==========================================================================*/

#ifndef AUD_DEV_MGR_MT_STRTR_IF_H
#define AUD_DEV_MGR_MT_STRTR_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "EliteMsg_AdmCustom.h"
#include "StreamRouterSvc.h"
#include "EliteAprIf.h"
#include "AudDevMgr_PrivateDefs.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/
#define BUFFER_DURATION_IN_MS          10
#define NUM_MS_PER_SEC                 1000

/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
ADSPResult Adm_MsgStRtrStreamConnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pAdmConnectMsg, adm_client_type_t eClient);
ADSPResult Adm_MsgStRtrStreamDisconnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pAdmConnectMsg, adm_client_type_t eClient);
ADSPResult Adm_RegisterIEC61937FmtUpdate(void *pAudStatAdmSvc,elite_apr_packet_t *pPkt);

ADSPResult Adm_RxStRtrAfeConnectCmdHandler(void *pAudStatAdmSvc,
		bool_t is_device_open,
		uint16_t session_id,
		uint16_t afe_port_id,
		uint32_t num_channels,
		uint32_t bits_per_sample,
		uint32_t sampling_rate,
		uint32_t topology_id,
		uint16_t device_id,
		uint32_t admMemoryMapClient,
		elite_apr_packet_t *pkt,
		adm_client_type_t eClient);


ADSPResult Adm_TxStRtrAfeConnectCmdHandler(void *pAudStatAdmSvc,
		bool_t   is_device_open,
		uint16_t session_id,
		uint16_t afe_port_id,
		uint32_t num_channels,
		uint32_t sampling_rate,
		uint32_t topology_id,
		uint16_t device_id,
		adm_client_type_t eClient);

ADSPResult Adm_RxStRtrAfeDisconnectCmdHandler(void *pAudStatAdmSvc, uint16_t afe_port_id);
ADSPResult Adm_TxStRtrAfeDisconnectCmdHandler(void *pAudStatAdmSvc, uint16_t afe_port_id);
ADSPResult Adm_MsgStRtrDisableOutputPort(AdmStatSvc_InfoType *pAudStatAdmSvc, uint32_t direction, uint32_t output_port_id);

ADSPResult Adm_MsgStRtrDisableOutputPort(AdmStatSvc_InfoType *pAudStatAdmSvc, uint32_t direction, uint32_t output_port_id);
ADSPResult Adm_StRtrStreamDeviceMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc,
		                                           adm_cmd_stream_device_map_routings_v5_t *map_routings_cmd_ptr,
		                                           uint32_t opcode,
		                                           adm_client_type_t eClient);

ADSPResult Adm_GetAfePortIdFromDeviceId(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t device_id, uint16_t *afe_port_id);

ADSPResult Adm_SetPspdRXStRtrParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
											                                                 elite_apr_packet_t *pPkt, 
																							 adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
																							 int32_t *paramBase, 
																							 uint32_t dataPayloadSize);

ADSPResult Adm_SetPspdTXStRtrParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
											                                                 elite_apr_packet_t *pPkt, 
																							 adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
																							 int32_t *paramBase, 
																							 uint32_t dataPayloadSize);
ADSPResult Adm_SetStrtrPrimaryOutputPort(AdmStatSvc_InfoType  *pAdmStatSvc);

ADSPResult Adm_SetStRtrDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
																							 elite_apr_packet_t *pPkt, 
																							 int32_t *paramBase, 
																							 uint32_t dataPayloadSize);
                                               
ADSPResult Adm_GetStRtrDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
																							 elite_apr_packet_t *pPkt);                                               


#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_MT_STRTR_IF_H
