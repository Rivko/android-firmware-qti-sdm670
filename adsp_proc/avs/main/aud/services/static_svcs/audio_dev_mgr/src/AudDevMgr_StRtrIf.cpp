/**
@file AudDevMgr_StRtrIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with  Stream Router Service.
 */

/*===========================================================================
Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_StRtrIf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger. 
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "AudDevMgr.h"
#include "AudDevMgr_StRtrIf.h"
#include "adsp_adm_api.h"
#include "StreamRouterSvc.h"
#include "AudDevMgr_PrivateDefs.h"
#include "AudDevMgr_CoppIf.h"
#include "AudDevMgr_DataBase.h"

/* =======================================================================
Function Definitions
========================================================================== */


/*======================================================================

FUNCTION      Adm_MsgStRtrStreamConnect

DESCRIPTION
Sends the customStreamRouterConnect message to stream router.
Specifies the direction of the connection (input/output)

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_MsgStRtrStreamConnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pAdmConnectMsg, adm_client_type_t eClient)
{
	ADSPResult  result = ADSP_EOK;
	uint32_t direction = pAdmConnectMsg->direction;

	if (eLSM_TYPE == eClient)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect Stream Router Connect received for LSM service. Unsupported");
		return ADSP_EUNSUPPORTED;
	}

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Connection to Stream Router requested, Direction = %lu", direction);

	if((ADM_TX_DIR != direction) && (ADM_RX_DIR != direction))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Adm_MsgStreamConnect, Invalid value for direction in connect message");
		return ADSP_EBADPARAM;
	}

	if(ADM_RX_DIR == direction)
	{
		if(STRTR_MAX_PORTS == pAudStatAdmSvc->rxStRtrParams.num_inp_connections)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Adm_MsgStreamConnect, Rx Stream Router - no more vacant input ports");
			return ADSP_ENORESOURCE;
		}
	}
	else
	{
		if(STRTR_MAX_PORTS == pAudStatAdmSvc->txStRtrParams.num_out_connections)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Adm_MsgStreamConnect, Tx Stream Router - no more vacant output ports");
			return ADSP_ENORESOURCE;
		}
	}

	//custom connect message to stream router
	elite_msg_any_t msg;
	uint32_t unSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize, ELITE_CUSTOM_MSG,
			pAudStatAdmSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect, Failed to create connect msg to StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_CONNECT;
	pStRtrConnectMsg->svc_handle_ptr = pAdmConnectMsg->phPort;
	pStRtrConnectMsg->format_id = pAdmConnectMsg->format_id;
	pStRtrConnectMsg->data_log_id = pAdmConnectMsg->unDataLogId;
	pStRtrConnectMsg->buffering_ms = pAdmConnectMsg->buffering_ms;
	pStRtrConnectMsg->pEliteCb = &pAdmConnectMsg->eliteCb;

	//Stream connect on the RX i/p port and TX o/p port: AFE delay is not applicable
	pStRtrConnectMsg->punAFEDelay = NULL;
	pStRtrConnectMsg->punStrRtrDelay = NULL;

	if(ADM_RX_DIR == direction)
	{
		if(NULL == pAudStatAdmSvc->rxStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect, Error NULL RX stream router handle");
			result = ADSP_EFAILED;
			elite_msg_release_msg(&msg);
			return result;
		}

		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_INPUT;
		result = qurt_elite_queue_push_back(pAudStatAdmSvc->rxStRtrParams.hSvcHandle->cmdQ,
				(uint64_t *)&msg);
	}
	else
	{
		if(NULL == pAudStatAdmSvc->txStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect, Error NULL TX stream router handle");
			result = ADSP_EFAILED;
			elite_msg_release_msg(&msg);
			return result;
		}
    
		pStRtrConnectMsg->punStrRtrDelay = &(pAudStatAdmSvc->txStRtrParams.unStrRtrDelay[pAudStatAdmSvc->txStRtrParams.num_out_connections]);
		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_OUTPUT;
		result = qurt_elite_queue_push_back(pAudStatAdmSvc->txStRtrParams.hSvcHandle->cmdQ,
				(uint64_t *)&msg);
	}

	if(ADSP_EOK != result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect, Failed to push connect msg to StRtr");
		elite_msg_release_msg(&msg);
		return result;
	}

	//wait for connect Ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Wait for ack failed");
		elite_msg_release_msg(&msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Adm_MsgStreamConnect, failure in custom connect to Stream Router");
		result = pStRtrConnectMsg->unResponseResult;
		elite_msg_release_msg(&msg);
		return result;
	}

	//Allocate an index for this connection
	uint16_t unIndex = Adm_CalculateNextAvlIndex(pAudStatAdmSvc, (uint16_t)(direction), eClient);
	if (ADM_INVALID_INDEX == unIndex)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Error Adm_MsgStRtrStreamConnect: No more valid indices available.");
		result = ADSP_ENORESOURCE;
		elite_msg_release_msg(&msg);
		return result;
	}

	//fill the relevant parameters in ACK to ASM
	pAdmConnectMsg->phPort = pStRtrConnectMsg->svc_handle_ptr;
	if(ADM_RX_DIR == direction)
	{
		pAudStatAdmSvc->rxStRtrParams.num_inp_connections++;
		pAudStatAdmSvc->rxStRtrParams.port_id_to_session[pStRtrConnectMsg->port_id] = pAdmConnectMsg->unSessionID;

		//Associate this index with this session/stream ID.
		pAudStatAdmSvc->rxIndexToSessionStreamID[unIndex] = (uint16_t)(pAdmConnectMsg->unSessionID);
	}
	else
	{
		pAudStatAdmSvc->txStRtrParams.num_out_connections++;
		pAudStatAdmSvc->txStRtrParams.port_id_to_session[pStRtrConnectMsg->port_id] = pAdmConnectMsg->unSessionID;

		//Associate this index with this session/stream ID.
		pAudStatAdmSvc->txIndexToSessionStreamID[unIndex] = (uint16_t)(pAdmConnectMsg->unSessionID);
	}
	//send the port id back to ASM
	pAdmConnectMsg->unSessionID = pStRtrConnectMsg->port_id;

	//release custom connect message
	elite_msg_release_msg(&msg);

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Connection to Stream Router succeeded");

	return result;
}
/*======================================================================

FUNCTION      Adm_RegisterIEC61937FmtUpdate

DESCRIPTION
Sends the register compressed media format change msg to the stream router
to register/deregister stream router compressed media format change.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/

ADSPResult Adm_RegisterIEC61937FmtUpdate(void *pAudStatAdmSvc,elite_apr_packet_t *pPkt)
{
	ADSPResult                           result = ADSP_EFAILED;
	uint16_t                             unCoppID=0;
	uint16_t                             strtr_port_id = 0;
	adm_iec_61937_media_fmt_reg_t        *reg_ptr=NULL;
	AdmStatSvc_InfoType                 *pAdmStatSvc = (AdmStatSvc_InfoType*)pAudStatAdmSvc;
	qurt_elite_queue_t                  *pDestQ;
	int16_t                              curr_port=0;
	elite_msg_any_t strtr_msg;

	if(NULL == pAdmStatSvc->txStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_RegisterIEC61937FmtUpdate, Error NULL TX stream router handle");
		result = ADSP_EFAILED;
		(void)elite_apr_if_end_cmd(pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

	pDestQ = pAdmStatSvc->txStRtrParams.hSvcHandle->cmdQ;
	reg_ptr = (adm_iec_61937_media_fmt_reg_t *)elite_apr_if_get_payload_ptr(pPkt);

	unCoppID = reg_ptr->device_id;
	for(curr_port=0; curr_port<STRTR_MAX_PORTS; curr_port++)
	{
		if(pAdmStatSvc->txStRtrParams.compr_copp_info[curr_port].unCoppID == unCoppID)
		{
			strtr_port_id = curr_port;
			reg_ptr->device_id = strtr_port_id;
			break;
		}
	}//send the custom connect command to stream router to get the input port service handle
	if(curr_port == STRTR_MAX_PORTS)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: RegIEC61937Fmt: No valid port id found for device_id 0x%x",
				unCoppID);
		(void)elite_apr_if_end_cmd( pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}
	MSG_3(MSG_SSID_QDSP6, DBG_LOW_PRIO,"ADM: RegIEC61937Fmt: Incoming msg: port_id 0x%x, device_id 0x%x, enable 0x%x",
			strtr_port_id,unCoppID, reg_ptr->enable);
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomCompressedFmtReg);
	if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: RegIEC61937Fmt, Failed to create msg to Tx StRtr, result = 0x%lx",
				(uint32_t)result);
		(void)elite_apr_if_end_cmd( pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

	EliteMsg_CustomCompressedFmtReg *pEliteMsg = (EliteMsg_CustomCompressedFmtReg*) strtr_msg.pPayload;
	pEliteMsg->token = unCoppID;
	pEliteMsg->port_id = strtr_port_id;
	pEliteMsg->dest_addr = pPkt->dst_addr;
	pEliteMsg->dest_port = pPkt->dst_port;
	pEliteMsg->src_addr = pPkt->src_addr;
	pEliteMsg->src_port = pPkt->src_port;
	pEliteMsg->enable = reg_ptr->enable;
	pEliteMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_REG_COMP_FMT_CHG;

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(pDestQ, (uint64_t *)&strtr_msg)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM:RegisterIEC61937FmtChange, Failed to push msg to StRtr cmdQ, result = 0x%lx",
				(uint32_t) result);
		elite_msg_release_msg(&strtr_msg);
		(void)elite_apr_if_end_cmd( pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: RegisterIEC61937FmtChange, Wait for SrtRtr connect ack failed. result = 0x%lx",
				(uint32_t) result);
		elite_msg_release_msg(&strtr_msg);
		(void)elite_apr_if_end_cmd( pAdmStatSvc->admAprHandle, pPkt, result);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pEliteMsg->unResponseResult)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: RegisterIEC61937FmtChange, Failure in register compressed format change to Tx stream router.result = 0x%lx",
				(uint32_t) result);
		result = pEliteMsg->unResponseResult;
	}
	//release the custom message
	elite_msg_release_msg(&strtr_msg);
	(void)elite_apr_if_end_cmd( pAdmStatSvc->admAprHandle, pPkt, result);

	return result;
}

/*======================================================================

FUNCTION      Adm_MsgStRtrStreamDisconnect

DESCRIPTION
Sends the customStreamRouterConnect message to stream router to disconnect
Specifies the direction of the connection (input/output)

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_MsgStRtrStreamDisconnect(AdmStatSvc_InfoType *pAudStatAdmSvc, EliteMsg_CustomAdmStreamConnectType *pAdmConnectMsg, adm_client_type_t eClient)
{
	ADSPResult  result = ADSP_EOK;
	uint32_t direction = pAdmConnectMsg->direction;
	uint32_t port_id   = STRTR_INVALID_PORT_ID;

	if (eLSM_TYPE == eClient)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Error Adm_MsgStreamDisconnect received for LSM session. Unsupported");
		return ADSP_EBADPARAM;
	}

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Disconnection from Stream Router requested");

	uint16_t unIndex = Adm_SessionStreamIDToIndex(pAudStatAdmSvc, (uint16_t)(direction), (uint16_t)(pAdmConnectMsg->unSessionID),eClient);
	if(ADM_INVALID_INDEX == unIndex)
	{
		result = ADSP_EBADPARAM;
		return result;
	}

	if((ADM_TX_DIR != direction) && (ADM_RX_DIR != direction))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Error Adm_MsgStreamDisconnect, Invalid value for direction in connect message");
		return ADSP_EBADPARAM;
	}

	if(ADM_RX_DIR == direction)
	{
		for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
		{
			if(pAdmConnectMsg->unSessionID == pAudStatAdmSvc->rxStRtrParams.port_id_to_session[i])
			{
				port_id = i;
				break;
			}
		}
		if(STRTR_INVALID_PORT_ID == port_id)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Error Adm_MsgStreamDisconnect, Disconnection requested for unconnected session");
			return ADSP_EBADPARAM;
		}
	}
	else//ADM_TX_DIR
	{
		for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
		{
			if(pAdmConnectMsg->unSessionID == pAudStatAdmSvc->txStRtrParams.port_id_to_session[i])
			{
				port_id = i;
				break;
			}
		}
		if(STRTR_INVALID_PORT_ID == port_id)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Error Adm_MsgStreamDisconnect, Disconnection requested for unconnected session");
			return ADSP_EBADPARAM;
		}
	}

	//custom disconnect message to stream router
	elite_msg_any_t msg;
	uint32_t unSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	if ( ADSP_FAILED( result = elite_msg_create_msg(&msg, &unSize, ELITE_CUSTOM_MSG,
			pAudStatAdmSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamDisconnect, Failed to create disconnect msg to StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT;
	pStRtrConnectMsg->port_id = port_id;

	if(ADM_RX_DIR == direction)
	{
		if(NULL == pAudStatAdmSvc->rxStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamDisconnect, Error NULL RX stream router handle");
			result = ADSP_EFAILED;
			elite_msg_release_msg(&msg);
			return result;
		}

		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_INPUT;
		result = qurt_elite_queue_push_back(pAudStatAdmSvc->rxStRtrParams.hSvcHandle->cmdQ, (uint64_t *)&msg);
	}
	else
	{
		if(NULL == pAudStatAdmSvc->txStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamDisconnect, Error NULL TX stream router handle");
			result = ADSP_EFAILED;
			elite_msg_release_msg(&msg);
			return result;
		}

		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_OUTPUT;
		result = qurt_elite_queue_push_back(pAudStatAdmSvc->txStRtrParams.hSvcHandle->cmdQ, (uint64_t *)&msg);
	}

	if(ADSP_EOK != result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStreamConnect, Failed to push disconnect msg to StRtr");
		elite_msg_release_msg(&msg);
		return result;
	}

	//wait for connect Ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Wait for ack failed");
		elite_msg_release_msg(&msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Adm_MsgStreamConnect, failure in custom disconnect to Stream Router");
		result = pStRtrConnectMsg->unResponseResult;
		elite_msg_release_msg(&msg);
		return result;
	}

	//fill the relevant parameters in ACK to ASM
	if(ADM_RX_DIR == direction)
	{
		pAudStatAdmSvc->rxStRtrParams.num_inp_connections--;
		pAudStatAdmSvc->rxStRtrParams.port_id_to_session[port_id] = 0;
		pAudStatAdmSvc->rxStRtrParams.session_id_to_delay_ptr_idx[unIndex] = ADM_INVALID_DELAY_PTR_ID;

	}
	else
	{
		pAudStatAdmSvc->txStRtrParams.num_out_connections--;
		pAudStatAdmSvc->txStRtrParams.port_id_to_session[port_id] = 0;
		pAudStatAdmSvc->txStRtrParams.session_id_to_delay_ptr_idx[unIndex] = ADM_INVALID_DELAY_PTR_ID;
	}

	//Reset session mapping mask.
	Adm_ResetCompressedSessionMappingMask(pAudStatAdmSvc, pAdmConnectMsg->unSessionID, direction, eClient);

	if (ADM_RX_DIR == direction)
	{
		//Reset this index mapping for this closed session/stream
		pAudStatAdmSvc->rxIndexToSessionStreamID[unIndex] = 0;

		// Detect primary device of all active streams and devices.
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM:  updating compressed primary device/port");
		if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDeviceCompressed(pAudStatAdmSvc)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: updating compressed primary device/port failed, result = %d", result);
			elite_msg_release_msg(&msg);
			return result;
		}
	}
	else
	{
		//Reset this index mapping for this closed session/stream
		pAudStatAdmSvc->txIndexToSessionStreamID[unIndex] = 0;
		pAudStatAdmSvc->txSessionIDToPrimCompressedDevIndexMap[unIndex] = ADM_INVALID_PRIMARY_DEVICE;
	}

	//release custom connect message
	elite_msg_release_msg(&msg);

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Disconnection from Stream Router succeeded");

	return result;
}

/*======================================================================

FUNCTION      Adm_RxStRtrAFEConnectCmdHandler

DESCRIPTION
1) Send custom connect command to AFE to obtain AFE port service handle
2) Now send this to stream router using custom stream router connect
message to connect stream router to AFE port.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_RxStRtrAfeConnectCmdHandler(void *pAudStatAdmSvc,
		bool_t is_device_open,
		uint16_t session_id,
		uint16_t afe_port_id,
		uint32_t num_channels,
		uint32_t sampling_rate,
		uint32_t bits_per_sample,
		uint32_t topology_id,
		uint16_t device_id,
		uint32_t admMemoryMapClient,
		elite_apr_packet_t *pPkt, 
		adm_client_type_t eClient )
{
	if (eLSM_TYPE == eClient)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Error CONNECT_AFE_PORT received for LSM session. Unsupported");
		return ADSP_EBADPARAM;
	}
	ADSPResult result = ADSP_EOK;
	ADSPResult joinResult = ADSP_EOK;
	AdmStatSvc_InfoType *pAdmStatSvc = (AdmStatSvc_InfoType*)pAudStatAdmSvc;
	uint32_t input_port_id = STRTR_INVALID_PORT_ID;
	ComprCoppInfo_t *pComprCoppInfo;
	elite_svc_handle_t *afe_client_handle, *rxStRtr_peer, *temp_afe_svc_handle;
	elite_msg_custom_afe_connect_t *afe_connect_msg_ptr;
	uint16_t  unIndex = ADM_INVALID_INDEX;

	if(NULL == pAdmStatSvc->rxStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_RxStRtrAfeConnectCmdHandler, Error NULL RX stream router handle");
		result = ADSP_EFAILED;
		return result;
	}

	//If this was the classic ADM_CMD_CONNECT_AFE_PORT_V5 command, a valid session_id will be provided
	if(!is_device_open)
	{
		unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_RX_DIR, session_id, eClient);
		if(ADM_INVALID_INDEX == unIndex)
		{
			result = ADSP_EBADPARAM;
			return result;
		}
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"ADM processing CONNECT_AFE_PORT, Rx Router, session id = 0x%hX, afe port id = 0x%X",
			session_id, afe_port_id);

	if(STRTR_MAX_PORTS == pAdmStatSvc->rxStRtrParams.num_out_connections)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: CONNECT_AFE_PORT - no more output ports in Rx Stream Router");
		return ADSP_ENORESOURCE;
	}

	if(!is_device_open)
	{
		//For ADM_CMD_CONNECT_AFE session ID needs to be provided since device leg is
		//paired with session in the same command
		//For DEVICE_OPEN this is not required, since pairing is done in MAP_ROUTINGS command
		//make sure that the session id matches with a connected session
		for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
		{
			if(session_id == pAdmStatSvc->rxStRtrParams.port_id_to_session[i])
			{
				input_port_id = i;
				break;
			}
		}

		if(STRTR_INVALID_PORT_ID == input_port_id)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Error CONNECT_AFE_PORT - session id does not match any opened compressed mode session");
			return ADSP_EBADPARAM;
		}

		//Reset session mask needs to be called because this command also serves as the map-routing for this session.
		Adm_ResetCompressedSessionMappingMask(pAdmStatSvc, session_id, RX_STRTR_ID, eClient);
	}

	// compressed copp structure pointer.
	pComprCoppInfo = &(pAdmStatSvc->rxStRtrParams.compr_copp_info[device_id - ADM_COMPRESSED_MIN_RX]);

	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		AudPPSvcInitParams_t PPParams;
		memset(&PPParams, 0, sizeof(AudPPSvcInitParams_t));

		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing CREATE_COPP cmd");

		PPParams.outputMediaTypeInitParams.keepInputSamplingRate = FALSE;
		PPParams.outputMediaTypeInitParams.outputSamplingRate = sampling_rate;
		PPParams.outputMediaTypeInitParams.useNativeNumChannels = TRUE;
		PPParams.outputMediaTypeInitParams.outputNumChannels = num_channels; //NA: as NativeNumChannels is TRUE
		PPParams.outputMediaTypeInitParams.useNativeBitsPerSample = TRUE;
		PPParams.outputMediaTypeInitParams.outputBitsPerSample = 0;
		PPParams.outputBufferNumAfeFramesPerChannel = ADM_LEGACY_NUM_AFE_FRAMES;
		PPParams.topology = topology_id;
		PPParams.type = DYNA_SVC_PP_TYPE_COPP; //TBD:This need to change
		PPParams.memoryMapClient = admMemoryMapClient;
		PPParams.pMsgCb = &(pComprCoppInfo->structPPCallbackInfo.cbFuntion);
		PPParams.pCtxt = &(pComprCoppInfo->structPPCallbackInfo.pContext);
		PPParams.pBufDelay = &(pComprCoppInfo->unCoppBufDelay);
		PPParams.pAlgorithmicDelay = &(pComprCoppInfo->unCoppAlgDelay);
		PPParams.initializeWithCompressedFormat = TRUE;

		//Increment the Number of RX Audio COPP opened
		pAdmStatSvc->unNumRxAudioCoppOpenIssued++;
		if(ADM_MAX_COPP_OPEN_CTR < pAdmStatSvc->unNumRxAudioCoppOpenIssued)
		{
			pAdmStatSvc->unNumRxAudioCoppOpenIssued = 0;
		}

		PPParams.ulDataLogId = ADM_LOG_ID_BASE_RX + (pAdmStatSvc->unNumRxAudioCoppOpenIssued << 4);
		PPParams.perf_mode = ADM_LEGACY_DEVICE_SESSION;
		PPParams.is_echo_canceller_session = FALSE;

		elite_svc_handle_t   *pDummyHandle;
		if (ADSP_FAILED(result = AudPP_CreateSvc(&PPParams, (void **) &pComprCoppInfo->phDevCopp,(void**)&pDummyHandle)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: CREATE_COPP cmd failed, result = %d", result);
			return result;
		}
		QURT_ELITE_ASSERT(NULL == pDummyHandle);
		//afe client information
		afe_client_handle = pComprCoppInfo->phDevCopp;
	}
	else
	{
		//if compressed copp is not present, stream router will connect to AFE
		afe_client_handle = pAdmStatSvc->rxStRtrParams.hSvcHandle;
	}

	//update topology ID & device ID.
	pComprCoppInfo->unCoppTopologyID = topology_id;
	pComprCoppInfo->unCoppID = device_id;

	//Send the connect command to AFE
	elite_msg_any_t afe_msg;
	qurt_elite_queue_t *afe_cmd_q = qurt_elite_globalstate.pAfeStatSvcCmdQ;

	uint32_t unPayloadSize = sizeof(elite_msg_custom_afe_connect_t);
	uint32_t token = (uint32_t)afe_cmd_q;

	if (ADSP_FAILED(result = elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom mesage for AFE connect");
		goto __bailoutCmdRxComprCoppOpen4;
	}

	afe_connect_msg_ptr = (elite_msg_custom_afe_connect_t *)afe_msg.pPayload;

	afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFECONNECT_REQ;
	memset(&afe_connect_msg_ptr->afe_client, 0, sizeof(afe_client_t));
	afe_connect_msg_ptr->afe_client.sample_rate = sampling_rate; //Ignored by AFE for RX clients
	afe_connect_msg_ptr->afe_client.data_type = IEC_61937_NON_LINEAR;
	afe_connect_msg_ptr->afe_client.data_path = AFE_RX_IN;
	afe_connect_msg_ptr->afe_client.cust_proc.subscribe_to_avt_drift = FALSE;
	afe_connect_msg_ptr->afe_client.channel = num_channels; //Ignored by AFE for RX clients
	//TBD: Revisit this calculation if need be. Still in time domain.
	afe_connect_msg_ptr->afe_client.buf_size = (sampling_rate * BUFFER_DURATION_IN_MS)/NUM_MS_PER_SEC; //Ignored by AFE for RX clients
	afe_connect_msg_ptr->afe_client.num_buffers = 2; //Ignored by AFE for RX clients
	afe_connect_msg_ptr->afe_client.bytes_per_channel = (bits_per_sample  / 8);
	afe_connect_msg_ptr->afe_client.bit_width = bits_per_sample;
	afe_connect_msg_ptr->afe_client.interleaved = 1;
	afe_connect_msg_ptr->afe_client.afe_client_handle = afe_client_handle;
	afe_connect_msg_ptr->afe_id = afe_port_id;
	afe_connect_msg_ptr->afe_drift_ptr = pComprCoppInfo->pAfeDriftPtr;
	afe_connect_msg_ptr->afe_client.afe_dp_delay_ptr = &(pAdmStatSvc->rxStRtrParams.unAFEDelay[pAdmStatSvc->rxStRtrParams.num_out_connections]);

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing AFE_CONNECT_REQ cmd, EP1: %d", afe_port_id);

	//push message into AFE cmd Q
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEConnectCmd Failed to send custom AFE connect msg");
		elite_msg_release_msg(&afe_msg);
		goto __bailoutCmdRxComprCoppOpen4;
	}

	//wait for connect Ack from AFE svc
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Wait for ack failed");
		elite_msg_release_msg(&afe_msg);
		goto __bailoutCmdRxComprCoppOpen4;
	}

	//check response result
	if(ADSP_EOK != afe_connect_msg_ptr->response_result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Error in AFE connect message Ack");
		result = afe_connect_msg_ptr->response_result;
		elite_msg_release_msg(&afe_msg);
		goto __bailoutCmdRxComprCoppOpen4;
	}

	//store the AFE port handle that is returned
	temp_afe_svc_handle = afe_connect_msg_ptr->svc_handle_ptr;
	pComprCoppInfo->pAfeDriftPtr = afe_connect_msg_ptr->afe_drift_ptr;

	//release afe custom connect message
	elite_msg_release_msg(&afe_msg);

	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
				"ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing CONNECT_COPP cmd");
		if (ADSP_FAILED(result = AdmSendCoppMsg(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ,
				temp_afe_svc_handle, ELITE_CMD_CONNECT)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: CONNECT_COPP cmd failed, result = %d", result);
			goto __bailoutCmdRxComprCoppOpen3;
		}
		//RX Stream router downstream peer handle
		rxStRtr_peer = pComprCoppInfo->phDevCopp;
	}
	else
	{
		//if compressed copp is not present, stream router will connect to AFE
		rxStRtr_peer = temp_afe_svc_handle;
	}

	//now send the custom connect command to stream router
	{
		elite_msg_any_t strtr_msg;
		unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
		qurt_elite_queue_t *strtr_cmd_q = pAdmStatSvc->rxStRtrParams.hSvcHandle->cmdQ;
		token = (uint32_t)strtr_cmd_q;
		if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
				pAdmStatSvc->admResponseQ, 0, 0)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to create connect msg to Rx StRtr");
			goto __bailoutCmdRxComprCoppOpen2;
		}

		EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
		pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_CONNECT;
		//fill the service handle of afe port obtained above
		pStRtrConnectMsg->svc_handle_ptr = rxStRtr_peer;
		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_OUTPUT;
		pStRtrConnectMsg->punAFEDelay = &(pAdmStatSvc->rxStRtrParams.unAFEDelay[pAdmStatSvc->rxStRtrParams.num_out_connections]);
		pStRtrConnectMsg->punStrRtrDelay = &(pAdmStatSvc->rxStRtrParams.unStrRtrDelay[pAdmStatSvc->rxStRtrParams.num_out_connections]);

		pStRtrConnectMsg->punCoppBufDelay = NULL;
		pStRtrConnectMsg->punCoppAlgDelay = NULL;
		if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
		{
			pStRtrConnectMsg->punCoppBufDelay = &(pComprCoppInfo->unCoppBufDelay);
			pStRtrConnectMsg->punCoppAlgDelay = &(pComprCoppInfo->unCoppAlgDelay);
		}
		else
		{
			pStRtrConnectMsg->punCoppBufDelay = NULL;
			pStRtrConnectMsg->punCoppAlgDelay = NULL;
		}

		pStRtrConnectMsg->num_channels = num_channels;
		pStRtrConnectMsg->sampling_rate = sampling_rate;
		pStRtrConnectMsg->bits_per_sample = bits_per_sample;
		pStRtrConnectMsg->pAfeDriftPtr = pComprCoppInfo->pAfeDriftPtr;
		if(!is_device_open)
		{
			pStRtrConnectMsg->port_id = input_port_id;
			pStRtrConnectMsg->is_stream_to_be_connected = TRUE;
		}
		else
		{
			pStRtrConnectMsg->is_stream_to_be_connected = FALSE;
		}

		if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to push msg to StRtr cmdQ");
			elite_msg_release_msg(&strtr_msg);
			goto __bailoutCmdRxComprCoppOpen2;
		}

		//wait for ack from stream router
		if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEConnectCmd, Wait for SrtRtr connect ack failed");
			elite_msg_release_msg(&strtr_msg);
			goto __bailoutCmdRxComprCoppOpen2;
		}

		//check the response result
		if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failure in custom connect to Rx stream router");
			result = pStRtrConnectMsg->unResponseResult;
			elite_msg_release_msg(&strtr_msg);
			goto __bailoutCmdRxComprCoppOpen2;
		}

		//update Rx Router output parameters. port id is returned in the response
		pComprCoppInfo->strtr_port_id = pStRtrConnectMsg->port_id;
		pComprCoppInfo->afe_port_id = afe_port_id;
		pAdmStatSvc->rxStRtrParams.port_id_to_delay_ptr_idx[pStRtrConnectMsg->port_id] = pAdmStatSvc->rxStRtrParams.num_out_connections;
		pAdmStatSvc->rxStRtrParams.num_out_connections++;
		pAdmStatSvc->rxStRtrParams.afe_port_svc_handle[pStRtrConnectMsg->port_id] = temp_afe_svc_handle;
		if(!is_device_open)
		{
			if(ADM_INVALID_DELAY_PTR_ID == pAdmStatSvc->rxStRtrParams.session_id_to_delay_ptr_idx[unIndex])
			{
				//we only need to store the delay pointer id for the primary session (first one to connect)
				pAdmStatSvc->rxStRtrParams.session_id_to_delay_ptr_idx[unIndex] = pAdmStatSvc->rxStRtrParams.num_out_connections;
			}

			//Update session mask needs to be called because this command also serves as the map-routing for this session.
			Adm_UpdateCompressedSessionMappingMask(pAdmStatSvc, session_id, device_id, RX_STRTR_ID, ADM_UCSMM_CONNECT, eClient);

		}

		//release custom connect message to stream router
		elite_msg_release_msg(&strtr_msg);
	}

	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		/* send a Run cmd to COPP */
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: As part of Rx ADM_CMD_COPP_OPEN, issuing RUN_COPP cmd");
		if (ADSP_FAILED(result = Adm_SendCmdRunCopp(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: RUN_COPP cmd failed, result = %d", result);
			goto __bailoutCmdRxComprCoppOpen1;
		}

		/* Register the PP callback info with ADM database */
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: As part of ADM_CMD_COPP_OPEN, registering PP clbk");

		if (ADSP_FAILED(result = Adm_DataBaseRegisterPPCb(pComprCoppInfo->unCoppID,
				pComprCoppInfo->structPPCallbackInfo, TRUE)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Registering PP clbk failed, result = %d", result);
			goto __bailoutCmdRxComprCoppOpen1;
		}
	}

	//If no map routing command is expected, we should update the primary port here.
	if(!is_device_open)
	{
		if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDeviceCompressed(pAdmStatSvc)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: updating compressed primary device/port failed, result = %d", result);
			goto __bailoutCmdRxComprCoppOpen1;
		}
	}

	return result;

	__bailoutCmdRxComprCoppOpen1:
	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		//In case of error in AFE connect command, disconnect stream router input port
		uint32_t res;
		elite_msg_any_t strtr_msg;
		qurt_elite_queue_t *strtr_cmd_q = pAdmStatSvc->rxStRtrParams.hSvcHandle->cmdQ;

		unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
		token = (uint32_t)strtr_cmd_q;
		if (ADSP_FAILED(res = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
				pAdmStatSvc->admResponseQ, 0, 0)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to create connect msg to Tx StRtr");
			return result;
		}

		EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
		pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT;
		pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_OUTPUT;
		pStRtrConnectMsg->port_id = pComprCoppInfo->strtr_port_id;

		if(ADSP_FAILED(res = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to push msg to StRtr cmdQ");
			elite_msg_release_msg(&strtr_msg);
			return result;
		}

		elite_svc_wait_for_ack(&strtr_msg);

		elite_msg_release_msg(&strtr_msg);
	}
	__bailoutCmdRxComprCoppOpen2:
	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		/* 2. Disconnect COPP from AFE Rx */
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of freeing up resources, issuing COPP_DISCONNECT cmd");
		if (ADSP_FAILED(result  = AdmSendCoppMsg(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ,
				temp_afe_svc_handle, ELITE_CMD_DISCONNECT)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DISCONNECT cmd failed, result = %d", result);
		}
	}
	__bailoutCmdRxComprCoppOpen3:
	/* 3. Disconnect AFE Rx */
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of freeing up resources, issuing AFE_PORT_DISCONNECT cmd");
	//Disconnect AFE if stream router connection request did not go through
	if (ADSP_FAILED(elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom AFE mesage");
		return result;
	}

	afe_connect_msg_ptr = (elite_msg_custom_afe_connect_t *)afe_msg.pPayload;

	//Populate the custom AFE connect message
	afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
	afe_connect_msg_ptr->afe_id = afe_port_id;
	afe_connect_msg_ptr->svc_handle_ptr = temp_afe_svc_handle;

	//push message into AFE cmd Q
	if (ADSP_FAILED(qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to send custom AFE connect msg");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//wait for connect Ack from AFE svc
	if (ADSP_FAILED(elite_svc_wait_for_ack(&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFE connect message, Wait for ack failed");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//release the custom afe connect message
	elite_msg_release_msg(&afe_msg);

	__bailoutCmdRxComprCoppOpen4:
	/* 4. Destroy COPP */
	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != topology_id)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of freeing up resources, issuing COPP_DESTROY cmd");
		qurt_elite_thread_t coppThreadID = pComprCoppInfo->phDevCopp->threadId;
		if (ADSP_FAILED(result |= AdmSendCoppMsg(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ,
				NULL, ELITE_CMD_DESTROY_SERVICE)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: COPP_DESTROY cmd failed, result = %d", result);
		}

		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Waiting for COPP to join");
		qurt_elite_thread_join(coppThreadID, &joinResult);
		if (ADSP_FAILED(joinResult))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP thread join failed, joinResult = %d", joinResult);
		}
		else
		{
			MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: COPP thread joined success");
		}
		result |= joinResult;
	}

	pAdmStatSvc->rxStRtrParams.num_out_connections--;
	pAdmStatSvc->rxStRtrParams.afe_port_svc_handle[pComprCoppInfo->strtr_port_id] = NULL;
	pAdmStatSvc->rxStRtrParams.unAFEDelay[pAdmStatSvc->rxStRtrParams.port_id_to_delay_ptr_idx[pComprCoppInfo->strtr_port_id]] = 0;

	pComprCoppInfo->unCoppID = ADM_COPP_ID_INVALID;
	pComprCoppInfo->unCoppTopologyID = AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE;
	pComprCoppInfo->afe_port_id = AFE_PORT_ID_INVALID;
	pComprCoppInfo->strtr_port_id = STRTR_INVALID_PORT_ID;
	pComprCoppInfo->phDevCopp = NULL;
	pComprCoppInfo->unCoppBufDelay = 0;
	pComprCoppInfo->unCoppAlgDelay = 0;

	return result;
}


/*======================================================================

FUNCTION      Adm_TxStRtrAFEConnectCmdHandler

DESCRIPTION
1) Send custom connect command to stream router to obtain the input
port service handle
2) Now send this to AFE using custom AFE connect
message to connect AFE port to stream router.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_TxStRtrAfeConnectCmdHandler(void *pAudStatAdmSvc,
		bool_t   is_device_open,
		uint16_t session_id,
		uint16_t afe_port_id,
		uint32_t num_channels,
		uint32_t sampling_rate,
		uint32_t topology_id,
		uint16_t device_id,
		adm_client_type_t eClient)
{
	if (eLSM_TYPE == eClient)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Error CONNECT_AFE_PORT received for LSM session. Unsupported");
		return ADSP_EBADPARAM;
	}
	ADSPResult result = ADSP_EOK;
	AdmStatSvc_InfoType *pAdmStatSvc = (AdmStatSvc_InfoType*)pAudStatAdmSvc;
	uint32_t output_port_id = STRTR_INVALID_PORT_ID;
	ComprCoppInfo_t *pComprCoppInfo;

	uint16_t  unIndex = ADM_INVALID_INDEX;

	if(NULL == pAdmStatSvc->txStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_TxStRtrAfeConnectCmdHandler, Error NULL TX stream router handle");
		result = ADSP_EFAILED;
		return result;
	}

	//If this was the classic ADM_CMD_CONNECT_AFE_PORT_V5 command, a valid session_id will be provided
	if(!is_device_open)
	{
		unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, ADM_TX_DIR, session_id, eClient);
		if(ADM_INVALID_INDEX == unIndex)
		{
			result = ADSP_EBADPARAM;
			return result;
		}
	}

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"ADM processing ADM_CMD_CONNECT_AFE_PORT, Tx router, session id = 0x%hX, afe port id = 0x%X",
			session_id, afe_port_id);

	//Make sure that there is a free stream router device side port
	if(STRTR_MAX_PORTS == pAdmStatSvc->txStRtrParams.num_inp_connections)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ADM_CMD_CONNECT_AFE_PORT - no more input ports in Tx Stream Router");
		return ADSP_ENORESOURCE;
	}

	if(!is_device_open)
	{
		//make sure that the session id matches with a connected session
		for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
		{
			if(session_id == pAdmStatSvc->txStRtrParams.port_id_to_session[i])
			{
				output_port_id = i;
				break;
			}
		}

		if(STRTR_INVALID_PORT_ID == output_port_id)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: ADM_CMD_CONNECT_AFE_PORT - session id does not match any opened compressed mode session");
			return ADSP_EBADPARAM;
		}

		//Reset session mask needs to be called because this command also serves as the map-routing for this session.
		Adm_ResetCompressedSessionMappingMask(pAdmStatSvc, session_id, TX_STRTR_ID, eClient);
	}

	//send the custom connect command to stream router to get the input port service handle
	elite_msg_any_t strtr_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	qurt_elite_queue_t *strtr_cmd_q = pAdmStatSvc->txStRtrParams.hSvcHandle->cmdQ;
	uint32_t token = (uint32_t)strtr_cmd_q;
	if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to create connect msg to Tx StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_CONNECT;
	pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_INPUT;
	pStRtrConnectMsg->punAFEDelay =  &(pAdmStatSvc->txStRtrParams.unAFEDelay[pAdmStatSvc->txStRtrParams.num_inp_connections]);
	pStRtrConnectMsg->punStrRtrDelay = &(pAdmStatSvc->txStRtrParams.unStrRtrDelay[pAdmStatSvc->txStRtrParams.num_inp_connections]);
	pStRtrConnectMsg->data_log_id = (afe_port_id << 20) | (pAdmStatSvc->txStRtrParams.unLogSeqId << 8);
	pAdmStatSvc->txStRtrParams.unLogSeqId++;

	if(!is_device_open)
	{
		pStRtrConnectMsg->port_id = output_port_id;
		pStRtrConnectMsg->is_stream_to_be_connected = TRUE;
	}
	else
	{
		pStRtrConnectMsg->is_stream_to_be_connected = FALSE;
	}

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEConnectCmd, Wait for SrtRtr connect ack failed");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failure in custom connect to Tx stream router");
		result = pStRtrConnectMsg->unResponseResult;
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//store the stream router input port service handle
	elite_svc_handle_t *temp_strtr_svc_handle = pStRtrConnectMsg->svc_handle_ptr;
	uint32_t input_port_id = pStRtrConnectMsg->port_id;

	//release the custom message
	elite_msg_release_msg(&strtr_msg);

	//Send the connect command to AFE
	elite_msg_any_t afe_msg;
	qurt_elite_queue_t *afe_cmd_q = qurt_elite_globalstate.pAfeStatSvcCmdQ;

	unPayloadSize = sizeof(elite_msg_custom_afe_connect_t);
	token = (uint32_t)afe_cmd_q;

	if (ADSP_FAILED(result = elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom mesage for AFE connect");
		goto __bailout;
	}

	{
		elite_msg_custom_afe_connect_t *afe_connect_msg_ptr = (elite_msg_custom_afe_connect_t *)afe_msg.pPayload;

		afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFECONNECT_REQ;
		memset(&afe_connect_msg_ptr->afe_client, 0, sizeof(afe_client_t));
		afe_connect_msg_ptr->afe_client.sample_rate = sampling_rate;
		afe_connect_msg_ptr->afe_client.data_type = IEC_61937_NON_LINEAR;
		afe_connect_msg_ptr->afe_client.data_path = AFE_TX_OUT;
		afe_connect_msg_ptr->afe_client.cust_proc.subscribe_to_avt_drift = FALSE;
		afe_connect_msg_ptr->afe_client.channel = num_channels;
		elite_svc_get_frame_size(sampling_rate,&afe_connect_msg_ptr->afe_client.buf_size);  //get operating frame size in samples per msec.
                afe_connect_msg_ptr->afe_client.buf_size *=  BUFFER_DURATION_IN_MS;  //calculate 10msec buffer size
		afe_connect_msg_ptr->afe_client.num_buffers = 2;
		afe_connect_msg_ptr->afe_client.bytes_per_channel = 2;
		afe_connect_msg_ptr->afe_client.bit_width = 16;
		afe_connect_msg_ptr->afe_client.interleaved = 1;
		afe_connect_msg_ptr->afe_client.afe_client_handle = temp_strtr_svc_handle;

		afe_connect_msg_ptr->afe_id = afe_port_id;
		afe_connect_msg_ptr->afe_drift_ptr = NULL;
		afe_connect_msg_ptr->afe_client.afe_dp_delay_ptr = &(pAdmStatSvc->txStRtrParams.unAFEDelay[pAdmStatSvc->txStRtrParams.num_inp_connections]);

		MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Issuing AFE_CONNECT_REQ cmd, EP1: 0x%X, SR %lu, num ch %lu, bit-width %lu ",\
				afe_port_id, sampling_rate, num_channels, afe_connect_msg_ptr->afe_client.bit_width);

		//push message into AFE cmd Q
		if (ADSP_FAILED(result = qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEConnectCmd Failed to send custom AFE connect msg");
			elite_msg_release_msg(&afe_msg);
			goto __bailout;
		}

		//wait for connect Ack from AFE svc
		if (ADSP_FAILED(result = elite_svc_wait_for_ack(&afe_msg)))
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Wait for ack failed");
			elite_msg_release_msg(&afe_msg);
			goto __bailout;
		}

		//check response result
		if(ADSP_EOK != afe_connect_msg_ptr->response_result)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Error in AFE connect message Ack");
			result = afe_connect_msg_ptr->response_result;
			elite_msg_release_msg(&afe_msg);
			goto __bailout;
		}

		//Update the Tx Stream Router state in ADM
		pAdmStatSvc->txStRtrParams.port_id_to_delay_ptr_idx[input_port_id] = pAdmStatSvc->txStRtrParams.num_inp_connections;
		pAdmStatSvc->txStRtrParams.num_inp_connections++;
		pAdmStatSvc->txStRtrParams.afe_port_svc_handle[input_port_id] = afe_connect_msg_ptr->svc_handle_ptr;

		if(!is_device_open)
		{
			pAdmStatSvc->txStRtrParams.session_id_to_delay_ptr_idx[unIndex] = pAdmStatSvc->txStRtrParams.num_inp_connections;

			//Update session mask needs to be called because this command also serves as the map-routing for this session.
			Adm_UpdateCompressedSessionMappingMask(pAdmStatSvc, session_id, device_id, TX_STRTR_ID, ADM_UCSMM_CONNECT, eClient);
		}

		//release AFE custom connect message
		elite_msg_release_msg(&afe_msg);
	}

	//update compressed COPP information (topology ID, device ID, afe port id, stream router id).
	pComprCoppInfo = &(pAdmStatSvc->txStRtrParams.compr_copp_info[device_id-ADM_COMPRESSED_MIN_TX]);
	pComprCoppInfo->unCoppTopologyID = topology_id;
	pComprCoppInfo->unCoppID = device_id;
	pComprCoppInfo->afe_port_id = afe_port_id;
	pComprCoppInfo->strtr_port_id = input_port_id;

	return result;

	__bailout:

	//In case of error in AFE connect command, disconnect stream router input port
	uint32_t res;
	unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	token = (uint32_t)strtr_cmd_q;
	if (ADSP_FAILED(res = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to create connect msg to Tx StRtr");
		return result;
	}

	pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT;
	pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_INPUT;
	pStRtrConnectMsg->port_id = input_port_id;

	if(ADSP_FAILED(res = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEConnectCmd, Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	elite_svc_wait_for_ack(&strtr_msg);

	elite_msg_release_msg(&strtr_msg);

	//Update the Tx Stream Router state in ADM
	pAdmStatSvc->txStRtrParams.num_inp_connections--;
	pAdmStatSvc->txStRtrParams.afe_port_svc_handle[input_port_id] = NULL;
	pAdmStatSvc->txStRtrParams.unAFEDelay[pAdmStatSvc->txStRtrParams.port_id_to_delay_ptr_idx[input_port_id]] = 0;

	return result;
}


/*======================================================================

FUNCTION      Adm_RxStRtrAfeDisconnectCmdHandler

DESCRIPTION
1)Send the disconnect message to AFE.
2)Send the disconnect to stream router output port associated wit this
AFE port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_RxStRtrAfeDisconnectCmdHandler(void *pAudStatAdmSvc, uint16_t afe_port_id)
{
	ADSPResult result = ADSP_EOK;
	AdmStatSvc_InfoType *pAdmStatSvc = (AdmStatSvc_InfoType*)pAudStatAdmSvc;
	uint32_t output_port_id = STRTR_INVALID_PORT_ID;
	uint16_t unCoppID, unCoppIDIndex;

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"ADM: Processing ADM_CMD_AFE_DISCONNECT in RX mode. AFE Port ID[%u]", afe_port_id);

	if(NULL == pAdmStatSvc->rxStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_RxStRtrAfeDisconnectCmdHandler, Error NULL RX stream router handle");
		result = ADSP_EFAILED;
		return result;
	}

	ComprCoppInfo_t *pComprCoppInfo;

	for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
	{
		pComprCoppInfo = &(pAdmStatSvc->rxStRtrParams.compr_copp_info[i]);
		if(afe_port_id == pComprCoppInfo->afe_port_id)
		{
			output_port_id = pComprCoppInfo->strtr_port_id;
			unCoppIDIndex = (uint16_t)i;
			unCoppID = unCoppIDIndex + ADM_COMPRESSED_MIN_RX;
			break;
		}
	}

	if(STRTR_INVALID_PORT_ID == output_port_id)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisConnectCmd, Invalid AFE port ID: %u", afe_port_id);
		return ADSP_EBADPARAM;
	}

	//first disable stream router output queue
	if(ADSP_FAILED(result = Adm_MsgStRtrDisableOutputPort(pAdmStatSvc, RX_STRTR_ID, output_port_id)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisConnectCmd, Failed to send disable to stream router");
		return result;
	}

	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != pComprCoppInfo->unCoppTopologyID)
	{
		// Disconnect COPP from AFE Rx
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of ADM_CMD_COPP_CLOSE, issuing COPP_DISCONNECT cmd");
		if (ADSP_FAILED(result = AdmSendCoppMsg(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ,
				pAdmStatSvc->rxStRtrParams.afe_port_svc_handle[output_port_id], ELITE_CMD_DISCONNECT)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DISCONNECT cmd failed, result = %d", result);
			return result;
		}
	}

	//send AFE disconnect message
	elite_msg_any_t afe_msg;
	uint32_t unPayloadSize = sizeof( elite_msg_custom_afe_connect_t);
	qurt_elite_queue_t *afe_cmd_q = qurt_elite_globalstate.pAfeStatSvcCmdQ;
	uint32_t token = (uint32_t)afe_cmd_q;

	if (ADSP_FAILED(result = elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom AFE mesage");
		return result;
	}

	elite_msg_custom_afe_connect_t *afe_connect_msg_ptr =
			(elite_msg_custom_afe_connect_t *)afe_msg.pPayload;

	//Populate the custom AFE connect message
	afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
	afe_connect_msg_ptr->afe_id = afe_port_id;
	afe_connect_msg_ptr->svc_handle_ptr = pAdmStatSvc->rxStRtrParams.afe_port_svc_handle[output_port_id];

	//push message into AFE cmd Q
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to send custom AFE connect msg");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//wait for connect Ack from AFE svc
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFE connect message, Wait for ack failed");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//check response result
	if(ADSP_EOK != afe_connect_msg_ptr->response_result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error in AFE connect message Ack");
		result = afe_connect_msg_ptr->response_result;
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//release the custom afe connect message
	elite_msg_release_msg(&afe_msg);


	qurt_elite_thread_t coppThreadID = 0;
	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != pComprCoppInfo->unCoppTopologyID)
	{
		/*Destroy COPP handle.
        Store the COPP's thread ID before destroying it, so it can be used later for joining
		 */
		coppThreadID = pComprCoppInfo->phDevCopp->threadId;
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of Rx ADM_CMD_COPP_CLOSE, issuing COPP_DESTROY cmd");
		if (ADSP_FAILED(result = AdmSendCoppMsg(pComprCoppInfo->phDevCopp, pAdmStatSvc->admResponseQ,
				NULL, ELITE_CMD_DESTROY_SERVICE)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP_DESTROY cmd failed, result = %d", result);
			return result;
		}
	}

	//now disconnect stream router output port
	//now send the custom disconnect command to stream router
	elite_msg_any_t strtr_msg;
	qurt_elite_queue_t *strtr_cmd_q = pAdmStatSvc->rxStRtrParams.hSvcHandle->cmdQ;
	unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	token = (uint32_t)strtr_cmd_q;
	if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: AFEDisConnectCmd, Failed to create connect msg to Rx StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg = (EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT;
	pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_OUTPUT;
	pStRtrConnectMsg->port_id = output_port_id;

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: AFEDisConnectCmd,Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: AFEDisconnectCmd, Wait for SrtRtr connect ack failed");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: AFEDisconnectCmd, Failure in custom connect to Rx stream router");
		result = pStRtrConnectMsg->unResponseResult;
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//update Rx Router output parameters
	//disconnect the same port id, from ASM_CLOSE command.
	pAdmStatSvc->rxStRtrParams.num_out_connections--;
	pAdmStatSvc->rxStRtrParams.afe_port_svc_handle[output_port_id] = NULL;
	pAdmStatSvc->rxStRtrParams.unAFEDelay[pAdmStatSvc->rxStRtrParams.port_id_to_delay_ptr_idx[output_port_id]] = 0;

	//release custom connect message to stream router
	elite_msg_release_msg(&strtr_msg);

	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "All cmds to stop Rx Device have been issued");

	if (AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE != pComprCoppInfo->unCoppTopologyID)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Waiting for COPP to join");
		int joinResult;
		qurt_elite_thread_join(coppThreadID, &joinResult);
		if (ADSP_FAILED(joinResult))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: COPP thread join failed, result = %d", result);
			return joinResult;
		}
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: COPP thread joined");

		/* De-register the PP callback info with ADM database */
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of ADM_CMD_COPP_CLOSE, de-registering PP clbk");
		if (ADSP_FAILED(result = Adm_DataBaseDeRegisterPPCb(pComprCoppInfo->unCoppID,
				pComprCoppInfo->structPPCallbackInfo, TRUE)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: De-registering PP clbk failed, result = %d", result);
			return result;
		}
	}

	//Remove this Device ID (COPP ID) from any existing session mapping masks
	Adm_RemoveCoppIDFromCompressedSessionMappingMask(pAdmStatSvc, unCoppID, RX_STRTR_ID);

	//Initialize topology ID & device ID.
	pComprCoppInfo->unCoppID = ADM_COPP_ID_INVALID;
	pComprCoppInfo->unCoppTopologyID = AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE;
	pComprCoppInfo->afe_port_id = AFE_PORT_ID_INVALID;
	pComprCoppInfo->strtr_port_id = STRTR_INVALID_PORT_ID;
	pComprCoppInfo->phDevCopp = NULL;
	pComprCoppInfo->unCoppBufDelay = NULL;
	pComprCoppInfo->unCoppAlgDelay = NULL;


	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "As part of ADM_CMD_DEVICE_CLOSE_V5, updating compressed primary device/port");
	if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDeviceCompressed(pAdmStatSvc)))
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: updating compressed primary device/port failed, result = %d", result);
	}

	return result;

}

/*======================================================================

FUNCTION      Adm_TxStRtrAfeDisconnectCmdHandler

DESCRIPTION
1)Send the disconnect to stream router input port. This would return any
buffers being held on to
2)Send disconnect to AFE port

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_TxStRtrAfeDisconnectCmdHandler(void *pAudStatAdmSvc, uint16_t afe_port_id)
{
	ADSPResult result = ADSP_EOK;
	AdmStatSvc_InfoType *pAdmStatSvc = (AdmStatSvc_InfoType*)pAudStatAdmSvc;
	uint32_t input_port_id = STRTR_INVALID_PORT_ID;
	uint16_t unCoppIDIndex, unCoppID;

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
			"ADM: Processing ADM_CMD_AFE_DISCONNECT in TX mode. AFE Port ID[%u]", afe_port_id);

	if(NULL == pAdmStatSvc->txStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_TxStRtrAfeDisconnectCmdHandler, Error NULL TX stream router handle");
		result = ADSP_EFAILED;
		return result;
	}

	ComprCoppInfo_t *pComprCoppInfo;
	for(uint32_t i=0; i<STRTR_MAX_PORTS; i++)
	{
		pComprCoppInfo = &(pAdmStatSvc->txStRtrParams.compr_copp_info[i]);
		if(afe_port_id == pComprCoppInfo->afe_port_id)
		{
			input_port_id = pComprCoppInfo->strtr_port_id;
			unCoppIDIndex = (uint16_t)i;
			unCoppID = unCoppIDIndex + ADM_COMPRESSED_MIN_TX;
			break;
		}
	}

	if(STRTR_INVALID_PORT_ID == input_port_id)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisConnectCmd, Invalid AFE port ID: %u", afe_port_id);
		return ADSP_EBADPARAM;
	}

	//First disable AFE output port. AFE will stop sending data down
	elite_msg_any_t afe_msg;
	qurt_elite_queue_t *afe_cmd_q = qurt_elite_globalstate.pAfeStatSvcCmdQ;

	uint32_t unPayloadSize = sizeof(elite_msg_custom_afe_connect_t);
	uint32_t token = (uint32_t)afe_cmd_q;
	elite_msg_custom_afe_connect_t *afe_connect_msg_ptr;

	if (ADSP_FAILED(result = elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom mesage for AFE disable");
		return result;
	}

	afe_connect_msg_ptr = (elite_msg_custom_afe_connect_t *)afe_msg.pPayload;

	afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFECLIENTDISABLE;
	afe_connect_msg_ptr->afe_id = afe_port_id;
	afe_connect_msg_ptr->svc_handle_ptr = pAdmStatSvc->txStRtrParams.afe_port_svc_handle[input_port_id];

	//push message into AFE cmd Q
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisconnectCmd Failed to send custom AFE disable msg");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//wait for  Ack from AFE svc
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFE disable, Wait for ack failed");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//check response result
	if(ADSP_EOK != afe_connect_msg_ptr->response_result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error in AFE Disable message Ack");
		result = afe_connect_msg_ptr->response_result;
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//release the custom message
	elite_msg_release_msg(&afe_msg);

	//send disconnect command to Tx stream router
	elite_msg_any_t strtr_msg;
	unPayloadSize = sizeof(EliteMsg_CustomStreamRouterConnect);
	qurt_elite_queue_t *strtr_cmd_q = pAdmStatSvc->txStRtrParams.hSvcHandle->cmdQ;
	token = (uint32_t)strtr_cmd_q;
	if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEDisconnectCmd, Failed to create connect msg to Tx StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterConnect *pStRtrConnectMsg =
			(EliteMsg_CustomStreamRouterConnect*) strtr_msg.pPayload;
	pStRtrConnectMsg->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISCONNECT;
	pStRtrConnectMsg->port_dir = STRTR_PORT_DIR_INPUT;
	pStRtrConnectMsg->port_id = input_port_id;

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: AFEDisconnectCmd, Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisconnectCmd, Wait for SrtRtr disconnect ack failed");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != pStRtrConnectMsg->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: AFEDisconnectCmd, Failure in custom disconnect to Tx stream router");
		result = pStRtrConnectMsg->unResponseResult;
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//release the custom message
	elite_msg_release_msg(&strtr_msg);

	//send disconnect to AFE port
	unPayloadSize = sizeof(elite_msg_custom_afe_connect_t);
	token = (uint32_t)afe_cmd_q;

	if (ADSP_FAILED(result = elite_msg_create_msg( &afe_msg, &unPayloadSize,
			ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ,
			token,
			0) ) )
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Fail to create custom mesage for AFE disconnect");
		return result;
	}

	afe_connect_msg_ptr = (elite_msg_custom_afe_connect_t *)afe_msg.pPayload;
	afe_connect_msg_ptr->sec_op_code = ELITEMSG_CUSTOM_AFEDISCONNECT_REQ;
	afe_connect_msg_ptr->afe_id = afe_port_id;
	afe_connect_msg_ptr->svc_handle_ptr = pAdmStatSvc->txStRtrParams.afe_port_svc_handle[input_port_id];

	//push message into AFE cmd Q
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(afe_cmd_q, (uint64_t*)&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: AFEDisconnectCmd Failed to send custom AFE connect msg");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//wait for connect Ack from AFE svc
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&afe_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Wait for ack failed");
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//check response result
	if(ADSP_EOK != afe_connect_msg_ptr->response_result)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Adm_MsgStreamConnect, Error in AFE connect message Ack");
		result = afe_connect_msg_ptr->response_result;
		elite_msg_release_msg(&afe_msg);
		return result;
	}

	//Update the Tx Stream Router state in ADM
	pAdmStatSvc->txStRtrParams.num_inp_connections--;
	pAdmStatSvc->txStRtrParams.afe_port_svc_handle[input_port_id] = NULL;
	pAdmStatSvc->txStRtrParams.unAFEDelay[pAdmStatSvc->txStRtrParams.port_id_to_delay_ptr_idx[input_port_id]] = 0;

	//Remove this Device ID (COPP ID) from any existing session mapping masks
	Adm_RemoveCoppIDFromCompressedSessionMappingMask(pAdmStatSvc, unCoppID, TX_STRTR_ID);

	//re-initialize compressed copp information
	pComprCoppInfo->unCoppTopologyID = AUDPROC_TOPOLOGY_ID_COMPRESSED_NONE;
	pComprCoppInfo->unCoppID = ADM_COPP_ID_INVALID;
	pComprCoppInfo->afe_port_id = AFE_PORT_ID_INVALID;
	pComprCoppInfo->strtr_port_id = STRTR_INVALID_PORT_ID;

	//release afe custom connect message
	elite_msg_release_msg(&afe_msg);

	return result;
}


/*======================================================================

FUNCTION      Adm_MsgStRtrDisableOutputPort

DESCRIPTION
Sends disable to stream router output port. Stream router stops sending
data downstream after receiving this

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult Adm_MsgStRtrDisableOutputPort(AdmStatSvc_InfoType *pAdmStatSvc, uint32_t direction, uint32_t output_port_id)
{

	ADSPResult result = ADSP_EOK;
	elite_msg_any_t strtr_msg;
	qurt_elite_queue_t *strtr_cmd_q;

	if(RX_STRTR_ID == direction)
	{
		if(NULL == pAdmStatSvc->rxStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStRtrDisableOutputPort, Error NULL RX stream router handle");
			result = ADSP_EFAILED;
			return result;
		}

		strtr_cmd_q = pAdmStatSvc->rxStRtrParams.hSvcHandle->cmdQ;
	}
	else if(TX_STRTR_ID == direction)
	{
		if(NULL == pAdmStatSvc->txStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_MsgStRtrDisableOutputPort, Error NULL TX stream router handle");
			result = ADSP_EFAILED;
			return result;
		}

		strtr_cmd_q = pAdmStatSvc->txStRtrParams.hSvcHandle->cmdQ;
	}
	else
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Incorrect direction in stream router disable handler");
		return ADSP_EBADPARAM;
	}

	uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterDisablePort);

	if ( ADSP_FAILED( result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create Disable msg to StRtr");
		return result;
	}
	EliteMsg_CustomStreamRouterDisablePort *disable_cmd_ptr = (EliteMsg_CustomStreamRouterDisablePort *)strtr_msg.pPayload;
	disable_cmd_ptr->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_DISABLE_PORT;
	disable_cmd_ptr->port_id = output_port_id;

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Wait for SrtRtr pause ack failed");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != disable_cmd_ptr->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: Failure in PAUSE to Rx stream router");
		result = disable_cmd_ptr->unResponseResult;
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//release stream router custom message
	elite_msg_release_msg(&strtr_msg);

	return result;
}


ADSPResult Adm_StRtrStreamDeviceMapRoutingsHandler(AdmStatSvc_InfoType *pAdmStatSvc, adm_cmd_stream_device_map_routings_v5_t *map_routings_cmd_ptr, uint32_t opcode, adm_client_type_t eClient)
{
	if (eLSM_TYPE == eClient)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Error: Incorrect MapRoutings function invoked for LSM session");
		return ADSP_EBADPARAM;
	}

	ADSPResult result = ADSP_EOK;
	qurt_elite_queue_t *strtr_cmd_q;

	if(ADM_COMPRESSED_AUDIO_OUT == map_routings_cmd_ptr->mode)
	{
		if(NULL == pAdmStatSvc->rxStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_StRtrStreamDeviceMapRoutingsHandler, Error NULL RX stream router handle");
			result = ADSP_EFAILED;
			return result;
		}

		strtr_cmd_q = pAdmStatSvc->rxStRtrParams.hSvcHandle->cmdQ;
	}
	else if(ADM_COMPRESSED_AUDIO_IN == map_routings_cmd_ptr->mode)
	{
		if(NULL == pAdmStatSvc->txStRtrParams.hSvcHandle)
		{
			MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_StRtrStreamDeviceMapRoutingsHandler, Error NULL TX stream router handle");
			result = ADSP_EFAILED;
			return result;
		}

		strtr_cmd_q = pAdmStatSvc->txStRtrParams.hSvcHandle->cmdQ;
	}
	else
	{
		return ADSP_EBADPARAM;
	}

	adm_stream_device_map_node_v5_t *map_node_ptr = (adm_stream_device_map_node_v5_t *)(map_routings_cmd_ptr + 1);

	for(uint32_t i=0; i<map_routings_cmd_ptr->num_sessions; i++)
	{
		uint32_t stream_port_id;
		uint16_t session_id = map_node_ptr->session_id;

		//Extract the session and stream ID from the incoming ID and validate them.
		uint16_t extractedSessionID = 0, extractedStreamID = 0;
		extractedSessionID = (0xff00 & session_id) >> 8;
		extractedStreamID  = (0x00ff & session_id);

		if(
				(0 == session_id) ||
				((session_id > ASM_MAX_SUPPORTED_SESSION_ID) &&
						((0 == extractedSessionID) || (extractedSessionID > ASM_MAX_SUPPORTED_SESSION_ID) ||
								(0 == extractedStreamID)  || (extractedStreamID > ASM_MAX_SUPPORTED_STREAM_ID)))
		)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Error ADM_CMD_STREAM_DEVICE_(UN)MAP_ROUTINGS: Received invalid SessionID 0x%hX", session_id);
			return ADSP_EBADPARAM;
		}
		MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM ADM_CMD_STREAM_DEVICE_(UN)MAP_ROUTINGS ASM SSID: 0x%hX, SessionID: %d, StreamID: %d",
				session_id, extractedSessionID, extractedStreamID);

		//Map the incoming  SSID to Index
		uint32_t strRtrID = (ADM_COMPRESSED_AUDIO_OUT == map_routings_cmd_ptr->mode) ? (RX_STRTR_ID) : (TX_STRTR_ID);
		uint16_t unIndex = Adm_SessionStreamIDToIndex(pAdmStatSvc, (uint16_t)strRtrID, session_id, eClient);
		if(ADM_INVALID_INDEX == unIndex)
		{
			return ADSP_EBADPARAM;
		}

		//If incoming SSID is legacy, map it to contemporary SSID
		if (RX_STRTR_ID == strRtrID)
		{
			session_id = pAdmStatSvc->rxIndexToSessionStreamID[unIndex];
		}
		else
		{
			session_id = pAdmStatSvc->txIndexToSessionStreamID[unIndex];
		}
		MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM ADM_CMD_STREAM_DEVICE_(UN)MAP_ROUTINGS Contemporary SSID: 0x%hX", session_id);

		//make sure the session id is a valid connected session
		uint32_t j;
		for(j=0; j<STRTR_MAX_PORTS; j++)
		{
			if(session_id == pAdmStatSvc->rxStRtrParams.port_id_to_session[j])
			{
				//if session is RX session make sure mode is compressed output
				if(ADM_COMPRESSED_AUDIO_OUT != map_routings_cmd_ptr->mode)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
							"ADM: ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received session id 0x%hX is Rx session, but mode is not compressed output (ok for loopback)", session_id);
				}
				stream_port_id = j;
				break;
			}

			if(session_id == pAdmStatSvc->txStRtrParams.port_id_to_session[j])
			{
				//if session is TX session make sure mode is compressed input
				if(ADM_COMPRESSED_AUDIO_IN != map_routings_cmd_ptr->mode)
				{
					MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
							"ADM: ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received session id 0x%hX is Tx session, but mode is not compressed input (ok for loopback)", session_id);
				}
				stream_port_id = j;
				break;
			}
		}

		if(STRTR_MAX_PORTS == j)
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
					"ADM: Error ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received session id 0x%hX does not belong to valid opened session", session_id);
			return ADSP_EBADPARAM;
		}

		if(ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS_V5 == opcode)
		{
			//Reset session mapping mask.
			Adm_ResetCompressedSessionMappingMask(pAdmStatSvc, session_id, strRtrID, eClient);
		}

		//update stream port ID(input port ID) in mapping command payload
		map_node_ptr->session_id = stream_port_id;

		uint16_t *device_end_point_id_ptr = (uint16_t *)(map_node_ptr + 1);
		for(j=0; j<map_node_ptr->num_device_end_points; j++)
		{
			uint16_t afe_port_id=0;
			uint16_t device_port_id=0;
			//get the AFE port Id from the device end point id
			MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Device ID %d",*device_end_point_id_ptr);
			result = Adm_GetAfePortIdFromDeviceId(pAdmStatSvc, *device_end_point_id_ptr, &afe_port_id);
			if(ADSP_EOK != result)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
						"ADM: Error ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received AFE port Id %d is Rx, but mode is not compressed output", afe_port_id);
				return ADSP_EBADPARAM;
			}
			//verify that the device end point id (AFE port Id) belongs to a valid connected port
			uint32_t k;
			for(k=0; k<STRTR_MAX_PORTS; k++)
			{
				if(afe_port_id == pAdmStatSvc->rxStRtrParams.compr_copp_info[k].afe_port_id)
				{
					//if device is opened for Rx but the mode is not compressed output
					if(ADM_COMPRESSED_AUDIO_OUT != map_routings_cmd_ptr->mode)
					{
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
								"ADM: Error ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received AFE port Id %d is Rx, but mode is not compressed output", afe_port_id);
						return ADSP_EBADPARAM;
					}
					device_port_id = pAdmStatSvc->rxStRtrParams.compr_copp_info[k].strtr_port_id;
					break;
				}

				if(afe_port_id == pAdmStatSvc->txStRtrParams.compr_copp_info[k].afe_port_id)
				{
					//if device is opened for Tx but the mode is not compressed input
					if(ADM_COMPRESSED_AUDIO_IN != map_routings_cmd_ptr->mode)
					{
						MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
								"ADM: Error ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received AFE port Id %d is Rx, but mode is not compressed output", afe_port_id);
						return ADSP_EBADPARAM;
					}
					device_port_id = pAdmStatSvc->txStRtrParams.compr_copp_info[k].strtr_port_id;
					break;
				}
			}

			if(STRTR_MAX_PORTS == k)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
						"ADM: Error ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS: Received AFE port id %d does not belong to valid opened device connections", afe_port_id);
				return ADSP_EBADPARAM;
			}

			if(ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS_V5 == opcode)
			{
				//Update session mapping mask (add this connection)
				Adm_UpdateCompressedSessionMappingMask(pAdmStatSvc, session_id, *device_end_point_id_ptr, strRtrID, ADM_UCSMM_CONNECT, eClient);

			}
			else
			{
				//Update session mapping mask (remove this connection)
				Adm_UpdateCompressedSessionMappingMask(pAdmStatSvc, session_id, *device_end_point_id_ptr, strRtrID, ADM_UCSMM_DISCONNECT, eClient);

			}

			//update device port ID(output port ID for Rx/Input port for Tx) in mapping command payload
			*device_end_point_id_ptr = device_port_id;

			device_end_point_id_ptr++;
      
			//Update the Session -> Primary compressed Tx device table mapping.
			if (0 == j && TX_STRTR_ID == strRtrID )
			{      
				pAdmStatSvc->txSessionIDToPrimCompressedDevIndexMap[unIndex] = device_port_id;
 				MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM Compressed: TX Session ID [0x%hX] --> Primary device port ID [%d]", session_id, device_port_id);
			}
		}

		//calculate the byte offset, if number of devices is odd, force 4 byte alignment
		uint32_t num_devices_rounded_up = (map_node_ptr->num_device_end_points & 1) ? (map_node_ptr->num_device_end_points + 1) :
				map_node_ptr->num_device_end_points;
		map_node_ptr = (adm_stream_device_map_node_v5_t *)((uint16_t *)(map_node_ptr+1) + num_devices_rounded_up);
	}

	//send the map/unmap command to stream router with the session and AFE port id
	elite_msg_any_t strtr_msg;
	uint32_t unPayloadSize = sizeof(EliteMsg_CustomStreamRouterMapRoutings);

	if (ADSP_FAILED(result = elite_msg_create_msg(&strtr_msg, &unPayloadSize, ELITE_CUSTOM_MSG,
			pAdmStatSvc->admResponseQ, 0, 0)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create (Un)Map Routings msg to StRtr");
		return result;
	}

	EliteMsg_CustomStreamRouterMapRoutings *strtr_map_routings_cmd_ptr = (EliteMsg_CustomStreamRouterMapRoutings *)strtr_msg.pPayload;
	if(ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS_V5 == opcode)
	{
		strtr_map_routings_cmd_ptr->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_MAP_ROUTINGS;
	}
	else
	{
		strtr_map_routings_cmd_ptr->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_UNMAP_ROUTINGS;
	}
	strtr_map_routings_cmd_ptr->pRoutingPayload = map_routings_cmd_ptr;

	if(ADSP_FAILED(result = qurt_elite_queue_push_back(strtr_cmd_q, (uint64_t *)&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: (Un)Map Routings, Failed to push msg to StRtr cmdQ");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//wait for ack from stream router
	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&strtr_msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: (Un)Map Routings, Wait for SrtRtr (un)map routings ack failed");
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	//check the response result
	if(ADSP_EOK != strtr_map_routings_cmd_ptr->unResponseResult)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
				"ADM: (Un)Map Routings, Failure in custom (un)map routings command to stream router");
		result = strtr_map_routings_cmd_ptr->unResponseResult;
		elite_msg_release_msg(&strtr_msg);
		return result;
	}

	if (ADM_COMPRESSED_AUDIO_OUT == map_routings_cmd_ptr->mode)
	{
		// Detect primary device of all active streams and devices.
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: ADM_CMD_DEVICE_MAP_ROUTINGS_V5, updating compressed primary device/port");
		if (ADSP_FAILED(result = Adm_DetectAndUpdatePrimaryDeviceCompressed(pAdmStatSvc)))
		{
			MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: updating compressed primary device/port failed, result = %d", result);
			elite_msg_release_msg(&strtr_msg);
			return result;
		}
	}

	elite_msg_release_msg(&strtr_msg);
	return result;
}

ADSPResult Adm_GetAfePortIdFromDeviceId(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t device_id, uint16_t *afe_port_id)
{
	if((ADM_COMPRESSED_MIN_RX <= device_id) && (device_id <= ADM_COMPRESSED_MAX_RX))
	{
		*afe_port_id = pAdmStatSvc->rxStRtrParams.compr_copp_info[device_id - ADM_COMPRESSED_MIN_RX].afe_port_id;
		return ADSP_EOK;
	}
	else if((ADM_COMPRESSED_MIN_TX <= device_id) && (device_id <= ADM_COMPRESSED_MAX_TX))
	{
		*afe_port_id = pAdmStatSvc->txStRtrParams.compr_copp_info[device_id - ADM_COMPRESSED_MIN_TX].afe_port_id;
		return ADSP_EOK;
	}
	return ADSP_EFAILED;
}

ADSPResult Adm_SetPspdRXStRtrParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
		int32_t *paramBase,
		uint32_t dataPayloadSize)
{
	ADSPResult result = ADSP_EOK;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM Set Param called on RX strtr. Not currently supported. Exiting gracefully");
	return result;
}

ADSPResult Adm_SetPspdTXStRtrParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		adm_cmd_set_pspd_mtmx_strtr_params_v5_t *pIncomingAPRPayload,
		int32_t *paramBase,
		uint32_t dataPayloadSize)
{
	ADSPResult result = ADSP_EOK;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM Set Param called on TX strrtr. Not currently supported. Exiting gracefully");
	return result;
}

ADSPResult Adm_SetStrtrPrimaryOutputPort(AdmStatSvc_InfoType  *pAdmStatSvc)
{
	ADSPResult                  result = ADSP_EOK;
	qurt_elite_bufmgr_node_t    bufMgrNode;
	int                         nActualSize;
	uint32_t                    unBufferSize=0, unNumActiveSessions = 0;
	elite_msg_any_t             msg;

	if(NULL == pAdmStatSvc->rxStRtrParams.hSvcHandle)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"ADM: Adm_SetStrtrPrimaryOutputPort, Error NULL RX stream router handle");
		result = ADSP_EFAILED;
		return result;
	}

	//find number of active sessions
	for (uint16_t unIndex = 0; unIndex < ASM_MAX_SUPPORTED_SESSION_ID; unIndex++)
	{
		if( ( 0 != pAdmStatSvc->rxCompressedSessionMappingMask[unIndex]) )
		{
			if (ADM_INVALID_PRIMARY_DEVICE == pAdmStatSvc->rxSessionIDToPrimCompressedDevIndexMap[unIndex])
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
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Adm_SetStrtrPrimaryOutputPort, No Active Sessions");
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
	pCustomSetPrimaryPortPayload->unClientToken = NULL;
	pCustomSetPrimaryPortPayload->unSecOpCode = ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PRIMARY_PORT;
	pCustomSetPrimaryPortPayload->unNumSessions = unNumActiveSessions;

	//populate mapping of session (input port) to primary output port.
	EliteMsg_CustomPrimaryPortMappingInfoType *pPrimaryPortMappingPayload = (EliteMsg_CustomPrimaryPortMappingInfoType*)(pBuffer+1);
	ComprCoppInfo_t *pComprCopp;
	uint32_t stream_port_id = 0;
	for (uint16_t unIndex = 0; unIndex < ASM_MAX_SUPPORTED_SESSION_ID; unIndex++)
	{
		if ( ( 0 != pAdmStatSvc->rxCompressedSessionMappingMask[unIndex])  )
		{
			//make sure the session id is a valid connected session
			uint32_t j;
			for(j=0; j<STRTR_MAX_PORTS; j++)
			{
				if(pAdmStatSvc->rxIndexToSessionStreamID[unIndex] == pAdmStatSvc->rxStRtrParams.port_id_to_session[j])
				{
					stream_port_id = j;
					break;
				}
			}

			if(STRTR_MAX_PORTS == j)
			{
				MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
						"ADM: Error: session id 0x%hX does not belong to valid opened session", pAdmStatSvc->rxIndexToSessionStreamID[unIndex]);
				result = ADSP_EBADPARAM;
				elite_msg_push_payload_to_returnq(pCustomSetPrimaryPortPayload->pBufferReturnQ,(elite_msg_any_payload_t*)pCustomSetPrimaryPortPayload);
				return result;
			}

			pPrimaryPortMappingPayload->unInputPort = stream_port_id;
			pComprCopp = &pAdmStatSvc->rxStRtrParams.compr_copp_info[pAdmStatSvc->rxSessionIDToPrimCompressedDevIndexMap[unIndex]];
			pPrimaryPortMappingPayload->unPrimaryOutputPort = pComprCopp->strtr_port_id;
			pPrimaryPortMappingPayload++;
		}
	}

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*)pCustomSetPrimaryPortPayload;

	//assuming that this is applicable only in RX Router
	elite_svc_handle_t *pStRtrHandle;
	pStRtrHandle = pAdmStatSvc->rxStRtrParams.hSvcHandle;
	if (ADSP_FAILED(result = qurt_elite_queue_push_back(pStRtrHandle->cmdQ, (uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to Audio Stream Router");
		goto __bailoutCmdStrtrSetPrimaryPort;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdStrtrSetPrimaryPort;
	}

	result = (ADSPResult)pCustomSetPrimaryPortPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}

	__bailoutCmdStrtrSetPrimaryPort:
	elite_msg_release_msg(&msg);

	return result;
}

ADSPResult Adm_SetStRtrDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt,
		int32_t *paramBase,
		uint32_t dataPayloadSize)
{
	ADSPResult result = ADSP_EBADPARAM;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM device Set Param called on strrtr. Not currently supported. Exiting.");
	return result;
}

ADSPResult Adm_GetStRtrDeviceParamsCmdHandler(AdmStatSvc_InfoType  *pAdmStatSvc, 
		elite_apr_packet_t *pPkt)
{
	ADSPResult result = ADSP_EBADPARAM;
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM device Get Param called on strrtr. Not currently supported. Exiting.");
	return result;
}
