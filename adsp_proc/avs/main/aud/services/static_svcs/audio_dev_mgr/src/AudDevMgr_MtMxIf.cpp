/**
@file AudDevMgr_MtMxIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with Audio Matrix Mixer.

 */

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_MtMxIf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger.  
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "AudDevMgr.h"
#include "AudDevMgr_MtMxIf.h"
#include "adsp_adm_api.h"
#include "adsp_asm_stream_commands.h"
#include "AudDevMgr_mmpm.h"
#include "EliteData_Util.h"
#include "AudDevMgr_AprIf.h"
/* =======================================================================
Function Definitions
========================================================================== */

ADSPResult Adm_MsgMtMxStreamConnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pConnectMsg, adm_client_type_t eClient)
{
	ADSPResult  result = ADSP_EOK;
	uint32_t    bytes_per_sample;
	AdmStreamSessionInfo_Type *pSessionInfo;

	uint16_t unIndex = Adm_CalculateNextAvlIndex(pAudStatAdmSvc, (uint16_t)(pConnectMsg->direction), eClient);
	if (ADM_INVALID_INDEX == unIndex)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error Adm_MsgMtMxStreamConnect: No more valid indices available.");
		result = ADSP_ENORESOURCE;
		return result;
	}

	if (ADM_RX_DIR == pConnectMsg->direction)
	{
		if(eLSM_TYPE == eClient)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error Adm_MsgMtMxStreamConnect: LSM Session Type unsupported in Rx");                 
			result = ADSP_EBADPARAM;
			return result;
		}
		//Associate this index with this session/stream ID.
		pAudStatAdmSvc->rxIndexToSessionStreamID[unIndex] = (uint16_t)(pConnectMsg->unSessionID);

		bool_t bIsInPortLive = FALSE;
		bool_t bIsInPortBurstMode = FALSE;
		MtMxInPortHandle_t *pMtMxPortHandle = NULL;
		elite_svc_handle_t *pMtMxHandle = pAudStatAdmSvc->hRxMatrix;

		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_MsgStreamConnect, issuing RX ConnectMtMxInPort cmd, Live: 0, BM: 0");

		pAudStatAdmSvc->unRxMtMxDelay[unIndex] = 0;

		result = Adm_SendCmdConnectMtMxInPort(pAudStatAdmSvc, 0xFFFFFFFF, bIsInPortLive, bIsInPortBurstMode,
				0, pMtMxHandle, &pMtMxPortHandle,
				&pConnectMsg->eliteCb,
				pConnectMsg->unDataLogId,
				&pAudStatAdmSvc->unRxMtMxDelay[unIndex],pConnectMsg->stream_perf_mode,pConnectMsg->unSessionID, NULL, FALSE);

		if (ADSP_FAILED(result) || (NULL == pMtMxPortHandle))
		{
			//Reset this index mapping as this session/stream was never opened.
			pAudStatAdmSvc->rxIndexToSessionStreamID[unIndex] = 0;
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_SendCmdConnectMtMxInPort failed, result = %d", result);
			return result;
		}

		//Provide handle of matrix input port to ASM
		pConnectMsg->phPort = &(pMtMxPortHandle->portHandle);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_MsgStreamConnect, RX ConnectMtMxInPort cmd: ASM Session ID: 0x%lX, i/p port ID: %lu",
				pConnectMsg->unSessionID, pMtMxPortHandle->inPortID);

		//Store the port ID in sessionIDToRxMtMxInPortIDMap array
		pAudStatAdmSvc->sessionIDToRxMtMxInPortIDMap[unIndex] = pMtMxPortHandle->inPortID;

		//Store the session ID in rxMtMxInPortIDToSessionIDMap array
		pAudStatAdmSvc->rxMtMxInPortIDToSessionIDMap[pMtMxPortHandle->inPortID] = pConnectMsg->unSessionID;

		//Provide the port ID to ASM
		pConnectMsg->unSessionID = pMtMxPortHandle->inPortID;

		//Get hold of the pSessionInfo
		pSessionInfo = &(pAudStatAdmSvc->rxStreamSessionInfo[unIndex]);

		//Set the primary COPP for this session
		pAudStatAdmSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
	}
	else if (ADM_TX_DIR == pConnectMsg->direction)
	{
		uint32_t unNumOutBufs, unFrameSize;

		//Associate this index with this session/stream ID.
		pAudStatAdmSvc->txIndexToSessionStreamID[unIndex] = (uint16_t)(pConnectMsg->unSessionID);

		elite_svc_handle_t *pMtMxHandle = pAudStatAdmSvc->hTxMatrix;
		elite_svc_handle_t *pSvc2Connect = pConnectMsg->phPort;
		MtMxOutPortHandle_t   *pMtMxPortHandle = NULL;

		uint32_t unBufferingMs = pConnectMsg->buffering_ms;
		if ( ADM_LOW_LATENCY_DEVICE_SESSION == pConnectMsg->stream_perf_mode )
		{
			unNumOutBufs = (unBufferingMs + (ADM_FRAME_SIZE_1MS - 1))/ADM_FRAME_SIZE_1MS;
			unFrameSize = ADM_FRAME_SIZE_1MS;
		}
		else
		{
			unNumOutBufs = (unBufferingMs + (ADM_FRAME_SIZE_5MS - 1))/ADM_FRAME_SIZE_5MS;
			unFrameSize = ADM_FRAME_SIZE_5MS;
		}

		uint8_t dev_channel_mapping[2] = {PCM_CHANNEL_L, PCM_CHANNEL_R};
		uint32_t unOutPortMode = pConnectMsg->unPortMode;
		uint8_t unNativeModeFlags = 0;
		bytes_per_sample = elite_data_get_bytes_per_sample(pConnectMsg->bits_per_sample);

		if(ADM_STREAM_CONNECT_PORT_MODE_ABSOLUTE_TS < unOutPortMode)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_MsgStreamConnect, issuing TX ConnectMtMxOutPort cmd, Mode = %lu is invalid. Setting to 0", unOutPortMode);
			unOutPortMode = 0;
		}

		MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_MsgStreamConnect, TX ConnectMtMxOutPort cmd, Byt/samp=%lu, NatMode=0, #ch=2, Mode = %lu, Buff = %lu ms, #Buff = %lu, FrSize = %lu",
				bytes_per_sample, unOutPortMode, unBufferingMs, unNumOutBufs,  unFrameSize);

		pAudStatAdmSvc->unTxMtMxDelay[unIndex] = 0;

		result = Adm_SendCmdConnectMtMxOutPort(pAudStatAdmSvc, 0xFFFFFFFF, PULL_MODE,
				ADM_NUM_CHANNELS_STEREO, &dev_channel_mapping[0],
				0, pMtMxHandle, &pSvc2Connect, &pMtMxPortHandle,
				&pConnectMsg->eliteCb,
				pConnectMsg->bits_per_sample, unOutPortMode,0,48000, NULL, NULL, NULL, &pAudStatAdmSvc->unTxMtMxDelay[unIndex], unNumOutBufs,pConnectMsg->stream_perf_mode, unNativeModeFlags,
				pConnectMsg->unSessionID, NULL);

		if (ADSP_FAILED(result) || (NULL == pMtMxPortHandle))
		{
			//Reset this index mapping as this session/stream was never opened.
			pAudStatAdmSvc->txIndexToSessionStreamID[unIndex] = 0;
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_SendCmdConnectMtMxOutPort failed, result = %d", result);
			return result;
		}

		//Provide handle of matrix output port to ASM
		pConnectMsg->phPort = &(pMtMxPortHandle->portHandle);

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_MsgStreamConnect, TX ConnectMtMxOutPort cmd: ASM Session ID: 0x%lX, o/p port ID: %lu",
				pConnectMsg->unSessionID, pMtMxPortHandle->outPortID);

		//Store the port ID in sessionIDToTxMtMxOutPortIDMap array
		pAudStatAdmSvc->sessionIDToTxMtMxOutPortIDMap[unIndex] = pMtMxPortHandle->outPortID;

		//Store the session ID in txMtMxOutPortIDToSessionIDMap array
		pAudStatAdmSvc->txMtMxOutPortIDToSessionIDMap[pMtMxPortHandle->outPortID] = pConnectMsg->unSessionID;

		//Provide the port ID to ASM
		pConnectMsg->unSessionID = pMtMxPortHandle->outPortID;

		//Get hold of the pSessionInfo
		pSessionInfo = &(pAudStatAdmSvc->txStreamSessionInfo[unIndex]);
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM rcvd invalid dir [%lu] in Adm_MsgMtMxStreamConnect", pConnectMsg->direction);
		return result;
	}

	//Set session sampling rate and performance mode
	pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
	pSessionInfo->stream_perf_mode = pConnectMsg->stream_perf_mode;

	return result;
}

ADSPResult Adm_MsgMtMxStreamDisconnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pPayload, adm_client_type_t eClient)
{
	ADSPResult                            result = ADSP_EOK;
	elite_svc_handle_t                    *pMtMxHandle;
	uint32_t                              unPortID;
	AdmStreamSessionInfo_Type             *pSessionInfo;
  
	if ((ADM_RX_DIR == pPayload->direction) && (eLSM_TYPE == eClient))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"Adm_MsgMtMxStreamDisconnect received for LSM session in Rx direction. Unsupported");
		return ADSP_EBADPARAM;
	}

	uint16_t unIndex = Adm_SessionStreamIDToIndex(pAudStatAdmSvc, (uint16_t)(pPayload->direction), (uint16_t)(pPayload->unSessionID), eClient);
	if(ADM_INVALID_INDEX == unIndex)
	{
		result = ADSP_EBADPARAM;
		return result;
	}

	if (ADM_RX_DIR == pPayload->direction)
	{
		//Obtain matrix handle and port ID
		pMtMxHandle = pAudStatAdmSvc->hRxMatrix;
		unPortID = pAudStatAdmSvc->sessionIDToRxMtMxInPortIDMap[unIndex];

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"Adm_MsgMtMxStreamDisconnect, issuing Adm_SendCmdDisconnectMtMxInPort, session ID: 0x%lX, i/p port ID: %lu",
				pPayload->unSessionID, unPortID);

		//Reset the port -> session lookup map
		pAudStatAdmSvc->rxMtMxInPortIDToSessionIDMap[unPortID] = 0;

		if (ADSP_FAILED(result = Adm_SendCmdDisconnectMtMxInPort(pAudStatAdmSvc, unPortID, pMtMxHandle)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_SendCmdDisconnectMtMxInPort failed, result = %d", result);
		}

		//Get hold of the pSessionInfo
		pSessionInfo = &(pAudStatAdmSvc->rxStreamSessionInfo[unIndex]);

		//Reset the primary COPP for this session as "Invalid"
		pAudStatAdmSvc->rxSessionIDToPrimCOPPIDMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
	}
	else if (ADM_TX_DIR == pPayload->direction)
	{
		//Obtain matrix handle and port ID
		pMtMxHandle =  pAudStatAdmSvc->hTxMatrix;
		unPortID = pAudStatAdmSvc->sessionIDToTxMtMxOutPortIDMap[unIndex];

		MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"Adm_MsgMtMxStreamDisconnect, issuing Adm_SendCmdDisconnectMtMxOutPort cmd, session ID: 0x%lX, o/p port ID: %lu",
				pPayload->unSessionID, unPortID);

		//Reset the port -> session lookup map
		pAudStatAdmSvc->txMtMxOutPortIDToSessionIDMap[unPortID] = 0;

		if (ADSP_FAILED(result = Adm_SendCmdDisconnectMtMxOutPort(pAudStatAdmSvc, unPortID, pMtMxHandle)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_SendCmdDisconnectMtMxOutPort failed, result = %d", result);
		}

		//Get hold of the pSessionInfo
		pSessionInfo = &(pAudStatAdmSvc->txStreamSessionInfo[unIndex]);
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error: ADM rcvd invalid dir [%lu] in Adm_MsgMtMxStreamDisconnect", pPayload->direction);
		return result;
	}

	//Reset session sampling rate and performance mode
	pSessionInfo->unSessionSampleRate = ADM_SAMPLING_RATE_48K;
	pSessionInfo->stream_perf_mode = ASM_LEGACY_STREAM_SESSION;

	//Reset session mapping mask.
	Adm_ResetSessionMappingMask(pAudStatAdmSvc, pPayload->unSessionID, pPayload->direction, eClient);

	if (ADM_RX_DIR == pPayload->direction)
	{
		//Update primary device
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Adm_MsgMtMxStreamDisconnect, updating primary device/port");
		if(ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDevicePcm(pAudStatAdmSvc)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: updating primary device/port failed, result = %d", result);
			return result;
		}

		//Reset this index mapping for this closed session/stream
		pAudStatAdmSvc->rxIndexToSessionStreamID[unIndex] = 0;
	}
	else
	{
		//Reset this index mapping for this closed session/stream
		pAudStatAdmSvc->txIndexToSessionStreamID[unIndex] = 0;
	}

	return result;
}

ADSPResult Adm_SendCmdDisableMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult                   result = ADSP_EOK;
	int                          nActualSize;
	qurt_elite_bufmgr_node_t     bufMgrNode;
	elite_msg_any_t              msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgOutPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
		return result;
	}
	EliteMsg_CustomCfgOutPortsType *pCfgOutPortsPayload = (EliteMsg_CustomCfgOutPortsType*) bufMgrNode.pBuffer;

	pCfgOutPortsPayload->cfgOutPortsPayload.numOutPorts = 1;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams =
			pAudStatAdmSvc->cfgOutPortsPayload.pOutPortCfgParams;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].index = unOutPortID;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].configuration = DISABLE_OUTPUT;

	pCfgOutPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgOutPortsPayload->unClientToken = 0;
	pCfgOutPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgOutPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgOutPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdDisableMtMxOutPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdDisableMtMxOutPort;
	}

	result = (ADSPResult)pCfgOutPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdDisableMtMxOutPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdDisconnectMtMxInPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unInPortID,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult               result = ADSP_EOK;
	int                     nActualSize;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	elite_msg_any_t          msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgInPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomCfgInPortsType *pCfgInPortsPayload = (EliteMsg_CustomCfgInPortsType*) bufMgrNode.pBuffer;

	pCfgInPortsPayload->cfgInPortsPayload.numInPorts = 1;
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams =
			pAudStatAdmSvc->cfgInPortsPayload.pInPortCfgParams;
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].index = unInPortID;
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].configuration = CLOSE_INPUT;

	pCfgInPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgInPortsPayload->unClientToken = 0;
	pCfgInPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgInPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_INPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgInPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdDisconnectMtMxInPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdDisconnectMtMxInPort;
	}

	result = (ADSPResult)pCfgInPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdDisconnectMtMxInPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdDisconnectMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult               result = ADSP_EOK;
	int                     nActualSize;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	elite_msg_any_t          msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgOutPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomCfgOutPortsType *pCfgOutPortsPayload = (EliteMsg_CustomCfgOutPortsType*) bufMgrNode.pBuffer;

	pCfgOutPortsPayload->cfgOutPortsPayload.numOutPorts = 1;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams =
			pAudStatAdmSvc->cfgOutPortsPayload.pOutPortCfgParams;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].index = unOutPortID;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].configuration = CLOSE_OUTPUT;

	pCfgOutPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgOutPortsPayload->unClientToken = 0;
	pCfgOutPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgOutPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgOutPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdDisconnectMtMxOutPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdDisconnectMtMxOutPort;
	}

	result = (ADSPResult)pCfgOutPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdDisconnectMtMxOutPort:
	elite_msg_release_msg(&msg);
	return result;
}

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
		volatile const afe_drift_info_t **ppAfeDriftPtr)
{
	ADSPResult               result = ADSP_EOK;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	int                     nActualSize;
	elite_msg_any_t          msg;
	uint32_t                i;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgOutPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
		return result;
	}

	EliteMsg_CustomCfgOutPortsType* pCfgOutPortsPayload = (EliteMsg_CustomCfgOutPortsType*) bufMgrNode.pBuffer;

	/* Number of output ports being configured */
	pCfgOutPortsPayload->cfgOutPortsPayload.numOutPorts = 1;

	/* Configuration parameters for the output port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams = pAudStatAdmSvc->cfgOutPortsPayload.pOutPortCfgParams;

	/* ID for the Output port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].index = unOutPortID;

	/* A new output port is being opened */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].configuration = NEW_OUTPUT;

	/* Number of buffers in the output queue for this port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].numOutputBufs = unNumOutBufs;

	/* Bytes per sample */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unBitWidth = unBitWidth;

	/* Maximum numbers of buffers in the output queue */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].maxBufQCapacity = 128;

	/* Handle of the downstream peer service to connect */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].pSvcHandle = *ppSvc2Connect;

	/* Output port mode of operation, pull only. Push is now not supported/needed*/
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].outputMode = unOutPortMode;

	/* Output port being configured in Native mode. */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unNativeModeFlags = unNativeModeFlags;

	/*Output port's number of channels */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].numChannels = unNumChannels;

	/* Output port channel mapping info */
	if (pChannelMapping)
	{
		for (i = 0; i < unNumChannels; i++)
		{
			pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].channelMapping[i] = *pChannelMapping++;
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error: Channel mapping must be sent to audio matrix");
		qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);
		return result;
	}

	/*Output port's sample rate */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unSampleRate = unSampleRate;

	/* Sets the TS mode. Currently applicable only on the TX o/p port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unPortMode = unOutPortMode2;

	/* Interleaved or non-interleaved data for this port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].interleaveFlag = FALSE;

	/* Bit mask indicating input ports feeding into this output port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].inPortsMask = outPortFeedMask;

	/* Pointer to ASM callback structure */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].pEliteCb = pEliteCb;

	/* Pointer to ASM callback structure */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unDataLogId = unDataLogId;

	/* Address of the downstream COPP buffering delay */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].punCoppBufDelay = punCoppBufDelay;

	/* Address of the downstream COPP algorithmic delay */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].punCoppAlgDelay = punCoppAlgDelay;

	/* Address of the downstream AFE delay */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].punAFEDelay = punAFEDelay;

	/* Address of the MXAR o/p port delay. MXAR is supposed to fill this value in */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].punMtMxDelay = punMtMxDelay;

	/* performance mode of the input port */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].perfMode = unPerfMode;

	/* The session/stream ID of connected ASM session */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unConnectedAsmSessionID = unConnectedAsmSessionID;

	/* Address to the AFE drift pointer */
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].ppAfeDriftPtr = ppAfeDriftPtr;

	/* Pointer to output port handle that is returned to the called */
	pCfgOutPortsPayload->cfgOutPortsPayload.pAck = ppMtMxPortHandle; // this is also where the mt mx will
	// ack back with a pointer to itshandle.

	pCfgOutPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgOutPortsPayload->unClientToken = 0;
	pCfgOutPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgOutPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgOutPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdConnectMtMxOutPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdConnectMtMxOutPort;
	}

	result = (ADSPResult)pCfgOutPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdConnectMtMxOutPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdReconfigMtMxOutPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unOutPortID,
		uint32_t unOutPortMode,
		uint32_t unNumChannels,
		uint8_t *pChannelMapping,
		elite_svc_handle_t *pMtMxHandle,
		elite_svc_handle_t **ppSvc2Connect,
		uint32_t unBitWidth,
		uint32_t unNumOutBufs,
		uint8_t unNativeModeFlags)
{
	ADSPResult                      result = ADSP_EOK;
	qurt_elite_bufmgr_node_t        bufMgrNode;
	int                             nActualSize;
	elite_msg_any_t                 msg;
	uint32_t                        i;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgOutPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
		return result;
	}
	EliteMsg_CustomCfgOutPortsType* pCfgOutPortsPayload = (EliteMsg_CustomCfgOutPortsType*) bufMgrNode.pBuffer;

	pCfgOutPortsPayload->cfgOutPortsPayload.numOutPorts = 1;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams =
			pAudStatAdmSvc->cfgOutPortsPayload.pOutPortCfgParams;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].index = unOutPortID;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].configuration = RECONFIGURE_OUTPUT;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].numOutputBufs = unNumOutBufs;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unBitWidth = unBitWidth;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].maxBufQCapacity = 128;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].pSvcHandle = *ppSvc2Connect;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].outputMode = unOutPortMode;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].numChannels = unNumChannels;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unNativeModeFlags = unNativeModeFlags;
	if (pChannelMapping)
	{
		for (i = 0; i < unNumChannels; i++)
		{
			pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].channelMapping[i] = *pChannelMapping++;
		}
	}
	else
	{
		result = ADSP_EBADPARAM;
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error: Channel mapping must be sent to audio matrix");
		qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);
		return result;
	}

	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].interleaveFlag = FALSE;
	//pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].inPortsMask = 0;
	//pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].pEliteCb = pEliteCb;
	//pCfgOutPortsPayload->cfgOutPortsPayload.pAck = NULL;

	pCfgOutPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgOutPortsPayload->unClientToken = 0;
	pCfgOutPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgOutPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgOutPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdReconfigMtMxOutPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdReconfigMtMxOutPort;
	}

	result = (ADSPResult)pCfgOutPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdReconfigMtMxOutPort:
	elite_msg_release_msg(&msg);
	return result;
}

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
		volatile const afe_drift_info_t **ppAfeDriftPtr, bool_t bIsAdmLoopback)
{
	ADSPResult                   result = ADSP_EOK;
	qurt_elite_bufmgr_node_t     bufMgrNode;
	int                          nActualSize;
	elite_msg_any_t              msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgInPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}

	EliteMsg_CustomCfgInPortsType* pCfgInPortsPayload = (EliteMsg_CustomCfgInPortsType*) bufMgrNode.pBuffer;

	/* Number of input ports being configured */
	pCfgInPortsPayload->cfgInPortsPayload.numInPorts = 1;

	/* Configuration parameters for this input port */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams = pAudStatAdmSvc->cfgInPortsPayload.pInPortCfgParams;

	/* ID for this Input port */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].index = unInPortID;

	/* A new input port is being opened */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].configuration = NEW_INPUT;

	/* The mask to indicate which output ports, this inport is feeding to */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].strMask = inPortRoutingMask;

	/* Number of input channels */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].numChannels = ADM_NUM_CHANNELS_STEREO;

	/* Flag to indicate Live or non-live port */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].bIsInPortLive = bIsInPortLive;

	/* Flag to indicate burst or non-burst mode of operation */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].bIsInPortBurstMode = bIsInPortBurstMode;

	/* Pointer to ASM callback structure */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].pEliteCb = pEliteCb;

	/* Data logging Id */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].unDataLogId = unDataLogId;

	/* Address of the MXAT i/p port delay. MXAT is supposed to fill this value in */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].punMtMxDelay = punMtMxDelay;

	/* performance mode of the input port */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].perfMode = unPerfMode;

	/* Address to the AFE drift pointer */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].ppAfeDriftPtr = ppAfeDriftPtr;

	/** ADM loopback or not */
	pCfgInPortsPayload->cfgInPortsPayload.pInPortCfgParams[0].bIsAdmLoopback = bIsAdmLoopback;

	/* Pointer to input port handler that is returned to the caller */
	pCfgInPortsPayload->cfgInPortsPayload.pAck = ppMtMxPortHandle; // this is where the mt mx will ack back.

	pCfgInPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgInPortsPayload->unClientToken = 0;
	pCfgInPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgInPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_INPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*) pCfgInPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdConnectMtMxInPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdConnectMtMxInPort;
	}

	result = (ADSPResult)pCfgInPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdConnectMtMxInPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdMapMtMxRoutings(AdmStatSvc_InfoType *pAudStatAdmSvc,
		elite_apr_packet_t *pPkt,
		elite_svc_handle_t *pMtMxHandle)
{
	adm_cmd_matrix_map_routings_v5_t     *pRoutingPayload;
	ADSPResult                              result = ADSP_EOK;
	qurt_elite_bufmgr_node_t                    bufMgrNode;
	int                                     nActualSize;
	elite_msg_any_t                          msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxMapRoutingsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomMtMxMapRoutingsType *pCustomMapRoutingsPayload = (EliteMsg_CustomMtMxMapRoutingsType*) bufMgrNode.pBuffer;

	pRoutingPayload = (adm_cmd_matrix_map_routings_v5_t*) elite_apr_if_get_payload_ptr(pPkt);

	pCustomMapRoutingsPayload->pRoutingPayload = pRoutingPayload;
	pCustomMapRoutingsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCustomMapRoutingsPayload->unClientToken = 0;
	pCustomMapRoutingsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomMapRoutingsPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_MAP_ROUTINGS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomMapRoutingsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMapMtMxRoutings;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMapMtMxRoutings;
	}

	result = (ADSPResult)pCustomMapRoutingsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdMapMtMxRoutings:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdRunMtMxPort(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unPort2Run,
		uint32_t unPortDirection,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult                              result = ADSP_EOK;
	qurt_elite_bufmgr_node_t                    bufMgrNode;
	int                                     nActualSize;
	elite_msg_any_t                          msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxRunType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomMtMxRunType *pRunPayload = (EliteMsg_CustomMtMxRunType*) bufMgrNode.pBuffer;
	pRunPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pRunPayload ->unClientToken = 0;
	pRunPayload->pBufferReturnQ = bufMgrNode.pReturnQ;

	pRunPayload->unPortID = unPort2Run;
	pRunPayload->unPortDirection = unPortDirection;
	pRunPayload->unStartFlag = MATRIX_MIXER_RUN_START_TIME_RUN_IMMEDIATE;
	pRunPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_RUN;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*) pRunPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdRunMtMxPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdRunMtMxPort;
	}

	result = (ADSPResult)pRunPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdRunMtMxPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdMatrixRampGains(AdmStatSvc_InfoType *pAudStatAdmSvc,
		elite_apr_packet_t *pPkt,
		elite_svc_handle_t *pMtMxHandle)
{
	adm_cmd_matrix_ramp_gains_v5_t         *pIncomingAPRPayload;
	ADSPResult                          result = ADSP_EOK;
	qurt_elite_bufmgr_node_t                bufMgrNode;
	int                                 nActualSize;
	elite_msg_any_t                     msg;

	if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxRampGainsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomMtMxRampGainsType *pCustomRampGainsPayload = (EliteMsg_CustomMtMxRampGainsType*) bufMgrNode.pBuffer;
	pIncomingAPRPayload = (adm_cmd_matrix_ramp_gains_v5_t*)elite_apr_if_get_payload_ptr(pPkt);

	pCustomRampGainsPayload->pRampGainsPayload = pIncomingAPRPayload;
	pCustomRampGainsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCustomRampGainsPayload->unClientToken = 0;
	pCustomRampGainsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomRampGainsPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_RAMP_GAINS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomRampGainsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMatrixRampGains;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMatrixRampGains;
	}

	result = (ADSPResult)pCustomRampGainsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdMatrixRampGains:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdMatrixMute(AdmStatSvc_InfoType *pAudStatAdmSvc,
		elite_apr_packet_t *pPkt,
		elite_svc_handle_t *pMtMxHandle)
{
	adm_cmd_matrix_mute_v5_t                *pIncomingAPRPayload;
	ADSPResult                          result = ADSP_EOK;
	qurt_elite_bufmgr_node_t              bufMgrNode;
	int                                 nActualSize;
	elite_msg_any_t                      msg;

	if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxMuteType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}
	EliteMsg_CustomMtMxMuteType *pCustomMutePayload = (EliteMsg_CustomMtMxMuteType*) bufMgrNode.pBuffer;
	pIncomingAPRPayload = (adm_cmd_matrix_mute_v5_t*)elite_apr_if_get_payload_ptr(pPkt);

	pCustomMutePayload->pMutePayload = pIncomingAPRPayload;
	pCustomMutePayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCustomMutePayload->unClientToken = 0;
	pCustomMutePayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomMutePayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_MUTE;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomMutePayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMatrixMute;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMatrixMute;
	}

	result = (ADSPResult)pCustomMutePayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdMatrixMute:
	elite_msg_release_msg(&msg);
	return result;
}

/*This function will send port state change command to MtMx*/
ADSPResult Adm_SendCmdMtMxPortStateChange(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unPortID,
		uint32_t unPortDirection,
		elite_svc_handle_t *pMtMxHandle,
		uint32_t unTargetSampleRate)
{
	ADSPResult                              result = ADSP_EOK;
	qurt_elite_bufmgr_node_t                bufMgrNode;
	int                                     nActualSize;
	elite_msg_any_t                          msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxPortStateChange), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}

	EliteMsg_CustomMtMxPortStateChange *pPortStateChangePayload = (EliteMsg_CustomMtMxPortStateChange*) bufMgrNode.pBuffer;
	pPortStateChangePayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pPortStateChangePayload->unClientToken = 0;
	pPortStateChangePayload->pBufferReturnQ = bufMgrNode.pReturnQ;

	pPortStateChangePayload->unPortID = unPortID;
	pPortStateChangePayload->unPortDirection = unPortDirection;
	pPortStateChangePayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_PORT_STATE_CHANGE;
	pPortStateChangePayload->unTargetSampleRate = unTargetSampleRate;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*) pPortStateChangePayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMtMxPortStateChange;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMtMxPortStateChange;
	}

	result = (ADSPResult)pPortStateChangePayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}

	__bailoutCmdMtMxPortStateChange:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdReconfigMtMxOutPortSampleRate(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle,
		uint32_t unSampleRate)
{
	ADSPResult                      result = ADSP_EOK;
	qurt_elite_bufmgr_node_t        bufMgrNode;
	int                             nActualSize;
	elite_msg_any_t                 msg;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgOutPortsType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
		return result;
	}
	EliteMsg_CustomCfgOutPortsType* pCfgOutPortsPayload = (EliteMsg_CustomCfgOutPortsType*) bufMgrNode.pBuffer;

	pCfgOutPortsPayload->cfgOutPortsPayload.numOutPorts = 1;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams =
			pAudStatAdmSvc->cfgOutPortsPayload.pOutPortCfgParams;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].index = unOutPortID;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].configuration = RECONFIGURE_OUTPUT_SAMPLE_RATE;
	pCfgOutPortsPayload->cfgOutPortsPayload.pOutPortCfgParams[0].unSampleRate = unSampleRate;

	pCfgOutPortsPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pCfgOutPortsPayload->unClientToken = 0;
	pCfgOutPortsPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCfgOutPortsPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_OUTPUT_PORTS;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCfgOutPortsPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg )))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdReconfigMtMxOutPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdReconfigMtMxOutPort;
	}

	result = (ADSPResult)pCfgOutPortsPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	__bailoutCmdReconfigMtMxOutPort:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SetPspdMtMxParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
		int32_t *paramBase,
		uint32_t dataPayloadSize,
		uint32_t unInPortID,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult                         result = ADSP_EOK;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	int                                         nActualSize;
	elite_msg_any_t                   msg;


	if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxPspdGetSetParamType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}

	EliteMsg_CustomMtMxPspdGetSetParamType *pCustomPsPdGetSetParamPayload = (EliteMsg_CustomMtMxPspdGetSetParamType*) bufMgrNode.pBuffer;

	pCustomPsPdGetSetParamPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomPsPdGetSetParamPayload->pResponseQ = pAdmStatSvc->admResponseQ;
	pCustomPsPdGetSetParamPayload->unClientToken = 0;
	pCustomPsPdGetSetParamPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_PSPD_SET_PARAM;
	pCustomPsPdGetSetParamPayload->unInPortID = unInPortID;
	pCustomPsPdGetSetParamPayload->unOutPortID = unOutPortID;
	pCustomPsPdGetSetParamPayload->unParamId = ELITEMSG_PARAM_ID_CAL;
	pCustomPsPdGetSetParamPayload->pnParamData = paramBase;
	pCustomPsPdGetSetParamPayload->unSize = dataPayloadSize;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomPsPdGetSetParamPayload;

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMatrixPSPDSetParam;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMatrixPSPDSetParam;
	}

	result = (ADSPResult)pCustomPsPdGetSetParamPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
		goto __bailoutCmdMatrixPSPDSetParam;
	}
	
	__bailoutCmdMatrixPSPDSetParam:
	elite_msg_release_msg(&msg);

	return result;
}

/* This function is to send custom message to Matrix to set primary output port of
all active sessions (input ports)*/
ADSPResult Adm_SetMtMxPrimaryOutputPort(AdmStatSvc_InfoType  *pAdmStatSvc)
{
	ADSPResult                  result = ADSP_EOK;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	int                         nActualSize;
	uint32_t                    unBufferSize=0, unNumActiveSessions = 0;
	elite_msg_any_t             msg;
	AdmStreamSessionInfo_Type *pSessionInfo = NULL;

	//find number of active sessions
	for (uint16_t unIndex = 0; unIndex < ASM_MAX_SUPPORTED_SESSION_ID; unIndex++)
	{
		pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
		if( ( 0 != pAdmStatSvc->rxsessionMappingMask[unIndex]) &&
			(ASM_ULTRA_LOW_LATENCY_STREAM_SESSION != pSessionInfo->stream_perf_mode) &&
			(ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION != pSessionInfo->stream_perf_mode) &&
			(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pSessionInfo->stream_perf_mode))
		{
			if (ADM_INVALID_PRIMARY_DEVICE == pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex])
			{
				MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error active SessionID: %d, Index: %d does not have primary device",
						pAdmStatSvc->rxIndexToSessionStreamID[unIndex], unIndex);
				return ADSP_EFAILED;
			}
			unNumActiveSessions++;
		}
	}

	if (0 == unNumActiveSessions)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_SetMtMxPrimaryOutputPort, No Active Sessions");
		return ADSP_EOK;
	}

	//calculate size of buffer required from buffer manager
	unBufferSize = sizeof(EliteMsg_CustomSetPrimaryPortType) + (unNumActiveSessions*sizeof(EliteMsg_CustomPrimaryPortMappingInfoType));

	if (ADSP_FAILED(result = elite_mem_get_buffer(unBufferSize, &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}

	EliteMsg_CustomSetPrimaryPortType *pBuffer = (EliteMsg_CustomSetPrimaryPortType*)bufMgrNode.pBuffer;

	//populate command payload
	EliteMsg_CustomSetPrimaryPortType *pCustomSetPrimaryPortPayload = pBuffer;
	pCustomSetPrimaryPortPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomSetPrimaryPortPayload->pResponseQ = pAdmStatSvc->admResponseQ;
	pCustomSetPrimaryPortPayload->unClientToken = 0;
	pCustomSetPrimaryPortPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_SET_PRIMARY_PORT;
	pCustomSetPrimaryPortPayload->unNumSessions = unNumActiveSessions;

	//populate mapping of session (input port) to primary output port.
	EliteMsg_CustomPrimaryPortMappingInfoType *pPrimaryPortMappingPayload = (EliteMsg_CustomPrimaryPortMappingInfoType*)(pBuffer+1);
	AdmCalPort_Type *phCalPort;
	for (uint16_t unIndex = 0; unIndex < ASM_MAX_SUPPORTED_SESSION_ID; unIndex++)
	{
		pSessionInfo = &(pAdmStatSvc->rxStreamSessionInfo[unIndex]);
		if ( ( 0 != pAdmStatSvc->rxsessionMappingMask[unIndex]) &&
				(ASM_ULTRA_LOW_LATENCY_STREAM_SESSION != pSessionInfo->stream_perf_mode) &&
				(ASM_ULTRA_LOW_LATENCY_POST_PROC_STREAM_SESSION != pSessionInfo->stream_perf_mode) &&
				(ASM_LOW_LATENCY_NO_PROC_STREAM_SESSION != pSessionInfo->stream_perf_mode))
		{
			pPrimaryPortMappingPayload->unInputPort = pAdmStatSvc->sessionIDToRxMtMxInPortIDMap[unIndex];
			phCalPort = &pAdmStatSvc->deviceCalPort[pAdmStatSvc->rxSessionIDToPrimCOPPIDMap[unIndex]];
			pPrimaryPortMappingPayload->unPrimaryOutputPort = (phCalPort->mtMxPort).phMtMxOutPort->outPortID;
			pPrimaryPortMappingPayload++;
		}
	}

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomSetPrimaryPortPayload;

	//assuming that this is applicable only in RX Matrix
	elite_svc_handle_t *pMtMxHandle;
	pMtMxHandle = pAdmStatSvc->hRxMatrix;
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMatrixSetPrimaryPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMatrixSetPrimaryPort;
	}

	result = (ADSPResult)pCustomSetPrimaryPortPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}

	__bailoutCmdMatrixSetPrimaryPort:
	elite_msg_release_msg(&msg);

	return result;
}

ADSPResult Adm_SetMtMxDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		int32_t *paramBase,
		uint32_t dataPayloadSize,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle)
{
	ADSPResult  result = ADSP_EOK;
	qurt_elite_bufmgr_node_t  bufMgrNode;
	int nActualSize;
	elite_msg_any_t msg;


	if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxOutputPortSetParamType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
		return result;
	}

	EliteMsg_CustomMtMxOutputPortSetParamType *pCustomMtMxOutputPortSetParamPayload = (EliteMsg_CustomMtMxOutputPortSetParamType*) bufMgrNode.pBuffer;

	pCustomMtMxOutputPortSetParamPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomMtMxOutputPortSetParamPayload->pResponseQ = pAdmStatSvc->admResponseQ;
	pCustomMtMxOutputPortSetParamPayload->unClientToken = 0;
	pCustomMtMxOutputPortSetParamPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_SET_PARAM;
	pCustomMtMxOutputPortSetParamPayload->unOutPortID = unOutPortID;
	pCustomMtMxOutputPortSetParamPayload->unParamId = ELITEMSG_PARAM_ID_CAL;
	pCustomMtMxOutputPortSetParamPayload->pnParamData = paramBase;
	pCustomMtMxOutputPortSetParamPayload->unSize = dataPayloadSize;

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomMtMxOutputPortSetParamPayload;
  

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
		goto __bailoutCmdMatrixSetParam;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMatrixSetParam;
	}

	result = (ADSPResult)pCustomMtMxOutputPortSetParamPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}
	
	__bailoutCmdMatrixSetParam:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_GetMtMxDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		uint32_t unOutPortID,
		elite_svc_handle_t *pMtMxHandle, uint32_t admMemoryMapClient)
{
	ADSPResult  result = ADSP_EOK;
	adm_cmd_get_mtmx_strtr_dev_params_v1_t    *pIncomingAPRPayload = NULL;
	adm_param_data_v6_t      *psParamData = NULL;
	elite_msg_any_t msg;
	qurt_elite_bufmgr_node_t  bufMgrNode;
	int nActualSize;
	
	pIncomingAPRPayload = (adm_cmd_get_mtmx_strtr_dev_params_v1_t*)elite_apr_if_get_payload_ptr(pPkt);
	
	if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomMtMxOutputPortGetParamType), &bufMgrNode, &nActualSize)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create msg for GET_DEV_PARAM cmd, result = %d", result);
		return result;
	}
	
	EliteMsg_CustomMtMxOutputPortGetParamType *pCustomMtMxOutputPortGetParamPayload = (EliteMsg_CustomMtMxOutputPortGetParamType*) bufMgrNode.pBuffer;
	
	memset(pCustomMtMxOutputPortGetParamPayload,0, sizeof(EliteMsg_CustomMtMxOutputPortGetParamType));
	
	pCustomMtMxOutputPortGetParamPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pCustomMtMxOutputPortGetParamPayload->pResponseQ = pAdmStatSvc->admResponseQ;
	pCustomMtMxOutputPortGetParamPayload->unClientToken = 0;
	pCustomMtMxOutputPortGetParamPayload->unSecOpCode = ELITEMSG_CUSTOM_MT_MX_OUTPUT_PORT_GET_PARAM;
	pCustomMtMxOutputPortGetParamPayload->unOutPortID = unOutPortID;
	pCustomMtMxOutputPortGetParamPayload->unParamId = ELITEMSG_PARAM_ID_CAL;
	
	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomMtMxOutputPortGetParamPayload;
	
	if (0 == pIncomingAPRPayload->mem_map_handle)
	{
		//In-Band...
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM processing ADM_CMD_GET_MTMX_STRTR_DEV_PARAMS_V1.. In-Band!");
		elite_apr_packet_t    *pCmdRspAprPkt = NULL;

		//CmdRSP payload contains status, modID, instanceID, paramID, param size, reserved field and the actual param values.
		uint32_t unCmdRspPayloadSize = sizeof(adm_cmd_rsp_get_mtmx_strtr_dev_params_v1_t) + pIncomingAPRPayload->param_max_size;

		//APR alloc memory for in-band CmdRSP and fill the header.
		result = elite_apr_if_alloc_cmd_rsp(pAdmStatSvc->admAprHandle,
				elite_apr_if_get_dst_addr( pPkt),
				elite_apr_if_get_dst_port( pPkt),
				elite_apr_if_get_src_addr( pPkt),
				elite_apr_if_get_src_port( pPkt),
				elite_apr_if_get_client_token( pPkt),
				ADM_CMDRSP_GET_MTMX_STRTR_DEV_PARAMS_V1,
				unCmdRspPayloadSize,
				&pCmdRspAprPkt );

		if (ADSP_FAILED(result) || (NULL == pCmdRspAprPkt) )
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to allocate APR pkt for InBand GetDeviceParams cmdrsp, result = %d", result);
			//Release the FADD msg that was bound to matrix.
			elite_msg_release_msg(&msg);			
			return result;
		}

		//Give matrix pointer to the CmdRSP APR pkt's payload (Offset by the status to access the module ID).
		int32_t* pCmdRspAprPktBase = (int32_t*)elite_apr_if_get_payload_ptr(pCmdRspAprPkt); 
		pCustomMtMxOutputPortGetParamPayload->pnParamData = (int32*)((int8_t*)pCmdRspAprPktBase + sizeof(adm_cmd_rsp_get_mtmx_strtr_dev_params_v1_t)); //Offset by the status.
		pCustomMtMxOutputPortGetParamPayload->unSize      = pIncomingAPRPayload->param_max_size;

		//Fill up the memory with the ModuleID and ParamID so that matrix doesnt have to do it
		psParamData = (adm_param_data_v6_t *)(pCustomMtMxOutputPortGetParamPayload->pnParamData);
		psParamData->module_id = pIncomingAPRPayload->module_id;
		psParamData->instance_id = pIncomingAPRPayload->instance_id;
		psParamData->reserved = 0;
		psParamData->param_id = pIncomingAPRPayload->param_id;
		psParamData->param_size = pIncomingAPRPayload->param_max_size;

		//Send FADD msg to Matrix svc and wait for ACK.
		result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg);

		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
			goto __bailoutInBandGetDeviceParams;
		}

		if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed, result = %d", result);
			goto __bailoutInBandGetDeviceParams;
		}

		result = (ADSPResult)pCustomMtMxOutputPortGetParamPayload->unResponseResult;
		if (ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
		}

		__bailoutInBandGetDeviceParams:
		//Update the status
		*pCmdRspAprPktBase = result;
		//Free up the incoming apr packet since we create another one for CmdRSP already.
		elite_apr_if_free( pAdmStatSvc->admAprHandle,  pPkt);
		//Send the CmdRSP back to client proc.
		result |= elite_apr_if_async_send( pAdmStatSvc->admAprHandle, pCmdRspAprPkt );
		//Release the FADD msg that was bound to matrix.
		elite_msg_release_msg(&msg);
	}
	else
	{
		//Out-of-band...
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM processing ADM_CMD_GET_MTMX_STRTR_DEV_PARAMS_V1.. Out-of-Band!");

		elite_mem_shared_memory_map_t sMemNode;

		//Ensure cache alignment of incoming physical address
		if(pIncomingAPRPayload->data_payload_addr_lsw & CACHE_ALIGNMENT_MASK)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Out-band GetDeviceParam Incoming addr LSW is not aligned to cache line");
			result  = ADSP_EBADPARAM;
			goto __bailoutOutBandGetDeviceParams;
		}

		//Phy->Virt
		sMemNode.unMemMapHandle = pIncomingAPRPayload->mem_map_handle;
		sMemNode.unMemMapClient = admMemoryMapClient;
		sMemNode.unPhysAddrLsw = pIncomingAPRPayload->data_payload_addr_lsw;
		sMemNode.unPhysAddrMsw = pIncomingAPRPayload->data_payload_addr_msw;
		sMemNode.unMemSize = (uint32_t)(pIncomingAPRPayload->param_max_size);

		result =  elite_mem_map_get_shm_attrib(sMemNode.unPhysAddrLsw, sMemNode.unPhysAddrMsw,
				sMemNode.unMemSize, &sMemNode);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Out-band GetPPParam failed Phy->Virt");
			goto __bailoutOutBandGetDeviceParams;
		}

		//Fill up the FADD message
		pCustomMtMxOutputPortGetParamPayload->pnParamData = (int32_t*)(sMemNode.unVirtAddr);
		pCustomMtMxOutputPortGetParamPayload->unSize      = pIncomingAPRPayload->param_max_size;

		//Fill up the memory with the ModuleID and ParamID so that matrix doesnt have to do it
		psParamData = (adm_param_data_v6_t *)(pCustomMtMxOutputPortGetParamPayload->pnParamData);
		psParamData->module_id = pIncomingAPRPayload->module_id;
		psParamData->instance_id = pIncomingAPRPayload->instance_id;
		psParamData->param_id = pIncomingAPRPayload->param_id;
		psParamData->param_size = pIncomingAPRPayload->param_max_size;
		psParamData->reserved = 0;

		//Send FADD msg to Matrix svc and wait for ACK.
		result = qurt_elite_queue_push_back(pMtMxHandle->cmdQ, (uint64_t*)&msg);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Matrix");
			goto __bailoutOutBandGetDeviceParams;
		}

		if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed, result = %d", result);
			goto __bailoutOutBandGetDeviceParams;
		}

		result = (ADSPResult)pCustomMtMxOutputPortGetParamPayload->unResponseResult;
		if (ADSP_EOK != result)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
			goto __bailoutOutBandGetDeviceParams;
		}

		//Flush the cache
		result = elite_mem_flush_cache(&sMemNode);
		if (ADSP_FAILED(result))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Out-band GetPPParam failed to flush cache");
		}

		__bailoutOutBandGetDeviceParams:
		//Update the status
		pCustomMtMxOutputPortGetParamPayload->unResponseResult = result;

		//Send the response as the status in ADM_CMDRSP_GET_MTMX_STRTR_DEV_PARAMS_V1
		AudDevMgr_GenerateAck(pPkt,
				pCustomMtMxOutputPortGetParamPayload->unResponseResult,
				&(pCustomMtMxOutputPortGetParamPayload->unResponseResult),
				sizeof(pCustomMtMxOutputPortGetParamPayload->unResponseResult),
				ADM_CMDRSP_GET_MTMX_STRTR_DEV_PARAMS_V1);

		//Release the FADD msg that was bound to matrix.
		elite_msg_release_msg(&msg);
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: ADM_CMD_GET_MTMX_STRTR_DEV_PARAMS_V1 completed, result = %d", result);
	return result;
}