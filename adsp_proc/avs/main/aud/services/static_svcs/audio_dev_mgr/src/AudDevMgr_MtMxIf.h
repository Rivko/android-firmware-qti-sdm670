/**
@file AudioDevMgr_MtMxIf.h
@brief This file declares functions that are used by AudioDevMgr to communicate with Audio Matrix Mixer.
*/

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_MtMxIf.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
==========================================================================*/

#ifndef AUD_DEV_MGR_MT_MX_IF_H
#define AUD_DEV_MGR_MT_MX_IF_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/
/// System
#include "qurt_elite.h"

/// Audio
#include "Elite.h"
#include "EliteMsg_AdmCustom.h"
#include "MixerSvc_Interface.h"
#include "EliteAprIf.h"
#include "AudDevMgr_PrivateDefs.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/

ADSPResult Adm_MsgMtMxStreamConnect(AdmStatSvc_InfoType *pAudStatAdmSvc,
														EliteMsg_CustomAdmStreamConnectType *pConnectMsg,
														adm_client_type_t eClient);
ADSPResult Adm_MsgMtMxStreamDisconnect(AdmStatSvc_InfoType *pAudStatAdmSvc,
														EliteMsg_CustomAdmStreamConnectType *pPayload,
														adm_client_type_t eClient);

ADSPResult Adm_SendCmdDisconnectMtMxInPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
														 uint32_t unInPortID,
														 elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdDisconnectMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
														  uint32_t unOutPortID,
														  elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdDisableMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
													  uint32_t unOutPortID,
													  elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdConnectMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                         int32_t unOutPortID,
                                         uint32_t unOutPortMode,
                                         uint32_t unNumChannels,
                                         uint8_t *pChannelMapping,
                                         uint32_t outPortFeedMask,
                                         elite_svc_handle_t *pMtMxHandle,
                                         elite_svc_handle_t **ppSvc2Connect,
                                         MtMxOutPortHandle_t **ppMtMxPortHandle,
                                         Elite_CbType *pEliteCb,
                                         uint32_t unBitWidth,
                                         uint32_t unOutPortMode2,
                                         uint32_t unDataLogId,
                                         uint32_t unSampleRate,
                                         volatile uint32_t *punCoppBufDelay,
                                         volatile uint32_t *punCoppAlgDelay,
                                         volatile uint32_t *punAFEDelay,
                                         volatile uint32_t *punMtMxDelay,
                                         uint32_t unNumOutBufs,
                                         uint16_t unPerfMode,
                                         uint8_t unNativeModeFlags,
										 uint32_t unConnectedAsmSessionID,
										 volatile const afe_drift_info_t **ppAfeDriftPtr);

ADSPResult Adm_SendCmdConnectMtMxInPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                        int32_t unInPortID,
                                        bool_t bIsInPortLive,
													 bool_t bIsInPortBurstMode,
                                        uint32_t inPortRoutingMask,
                                        elite_svc_handle_t *pMtMxHandle,
                                        MtMxInPortHandle_t **ppMtMxPortHandle,
                                        Elite_CbType *pEliteCb,
                                        uint32_t unDataLogId,
                                        volatile uint32_t *punMtMxDelay,
                                        uint16_t unPerfMode,
                                        uint32_t unConnectedAsmSessionID,
                                        volatile const afe_drift_info_t **ppAfeDriftPtr,
                                        bool_t bIsAdmLoopback);

ADSPResult Adm_SendCmdReconfigMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
														uint32_t unOutPortID,
														uint32_t unOutPortMode,
														uint32_t unNumChannels,
														uint8_t *pChannelMapping,
														elite_svc_handle_t *pMtMxHandle,
														elite_svc_handle_t **ppSvc2Connect,
														uint32_t unBitWidth,
														uint32_t unNumOutBufs,
														uint8_t unNativeModeFlags);

ADSPResult Adm_SendCmdMapMtMxRoutings(AdmStatSvc_InfoType *pAudStatAdmSvc,
												  elite_apr_packet_t *pPkt,
												  elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdMatrixRampGains(AdmStatSvc_InfoType *pAudStatAdmSvc,
												  elite_apr_packet_t *pPkt,
												  elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdRunMtMxPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                  uint32_t unPort2Run,
                                  uint32_t unPortDirection,
                                  elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdDestroyMtMx(elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdMatrixMute(AdmStatSvc_InfoType *pAudStatAdmSvc,
											elite_apr_packet_t *pPkt,
											elite_svc_handle_t *pMtMxHandle);

ADSPResult Adm_SendCmdMtMxPortStateChange(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                  uint32_t unPortID,
                                  uint32_t unPortDirection,
                                  elite_svc_handle_t *pMtMxHandle,
                                  uint32_t unTargetSampleRate);
								  
ADSPResult Adm_SendCmdReconfigMtMxOutPortSampleRate(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                         uint32_t unOutPortID,
                                         elite_svc_handle_t *pMtMxHandle,
                                         uint32_t unSampleRate);
								  
ADSPResult Adm_SetPspdMtMxParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
											                                                 elite_apr_packet_t *pPkt, 
																							 adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
																							 int32_t *paramBase, 
																							 uint32_t dataPayloadSize,
																							 uint32_t unInPortID,
																							 uint32_t unOutPortID,
																							 elite_svc_handle_t *pMtMxHandle);
																							 
ADSPResult Adm_SetMtMxPrimaryOutputPort(AdmStatSvc_InfoType  *pAdmStatSvc);

ADSPResult Adm_SetMtMxDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
																							elite_apr_packet_t *pPkt, 
																							int32_t *paramBase, 
																							uint32_t dataPayloadSize,
																							uint32_t unOutPortID,
																							elite_svc_handle_t *pMtMxHandle);
                                              
ADSPResult Adm_GetMtMxDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
																							elite_apr_packet_t *pPkt, 
																							uint32_t unOutPortID,
																							elite_svc_handle_t *pMtMxHandle, uint32_t admMemoryMapClient);                                              
                                               
#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUD_DEV_MGR_MT_MX_IF_H
