/**
@file AudDevMgr_AsmIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with Audio Stream Manager.
 */

/*========================================================================
Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_AsmIf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/28/12   KR      Non-backward compatible API changes for Badger.  
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudDevMgr.h"
#include "AudDevMgr_AsmIf.h"

/* =======================================================================
Function Declarations
========================================================================== */
static qurt_elite_queue_t* Adm_GetAsmCmdQForSessionID(uint32_t session_id);

/* =======================================================================
Function Definitions
========================================================================== */

static qurt_elite_queue_t* Adm_GetAsmCmdQForSessionID(uint32_t session_id)
{
   //Extract the session and stream ID from the incoming ID.
   uint16_t extracted_session_id = 0;
   extracted_session_id = (0xff00 & (uint16_t)(session_id)) >> 8;

   if (ASM_NUM_QUEUES <= extracted_session_id)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Given SSID %lu extracted invalid sessionID %d",
            session_id, extracted_session_id);
      return NULL;
   }

   return qurt_elite_globalstate.pAsmSessionCmdQ[extracted_session_id];
}

/*This function will send port state change command to MtMx*/
ADSPResult Adm_SendCmdAsmSetOutputSamplingRate(AdmStatSvc_InfoType *pAudStatAdmSvc,
		uint32_t unSampleRate,
		uint32_t session_id)
{
	ADSPResult                              result = ADSP_EOK;
	qurt_elite_bufmgr_node_t                bufMgrNode;
	int                                     nActualSize;
	elite_msg_any_t                         msg;
	qurt_elite_queue_t                      *asmSessionCmdQ;

	if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(EliteMsg_CustomASMCfgPPOutputSampleRateType), &bufMgrNode, &nActualSize)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
		return result;
	}

	EliteMsg_CustomASMCfgPPOutputSampleRateType *pRunPayload = (EliteMsg_CustomASMCfgPPOutputSampleRateType*) bufMgrNode.pBuffer;
	pRunPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
	pRunPayload->unClientToken = 0;
	pRunPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
	pRunPayload->outputSamplingRate = unSampleRate;
	pRunPayload->unSecOpCode = ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT_SAMPLE_RATE;
	pRunPayload->unStreamID = (0x00ff & (uint16_t)(session_id));

	msg.unOpCode = ELITE_CUSTOM_MSG;
	msg.pPayload = (void*) pRunPayload;

	MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
	      "ADM: Sending ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT_SAMPLE_RATE to SSID 0x%lX", session_id);

	asmSessionCmdQ = Adm_GetAsmCmdQForSessionID(session_id);
	if (NULL == asmSessionCmdQ)
	{
		result = ADSP_EFAILED;
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to get ASM SSID %d cmdQ ptr", session_id);
		goto __bailoutCmdMtMxPortStateChange;
	}

	if (ADSP_FAILED(result = qurt_elite_queue_push_back(
			asmSessionCmdQ,
			(uint64_t*)(&msg))))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to ASM");
		goto __bailoutCmdMtMxPortStateChange;
	}

	if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
		goto __bailoutCmdMtMxPortStateChange;
	}

	result = (ADSPResult)pRunPayload->unResponseResult;
	if (ADSP_EOK != result)
	{
		MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
	}

	__bailoutCmdMtMxPortStateChange:
	elite_msg_release_msg(&msg);
	return result;
}

ADSPResult Adm_SendCmdAsmSetOutputMediaFormat(AdmStatSvc_InfoType *pAudStatAdmSvc,
                                              uint16_t session_id,
                                              uint32_t sampling_rate,
                                              uint32_t num_channels,
                                              uint8_t *channel_mapping,
                                              uint32_t bits_per_sample)
{
   ADSPResult result = ADSP_EOK;
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: Sending ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT SSID 0x%lX",
         session_id);

   qurt_elite_queue_t *asmSessionCmdQ = Adm_GetAsmCmdQForSessionID(session_id);
   if (NULL == asmSessionCmdQ)
   {
      result = ADSP_EFAILED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to get ASM SSID %d cmdQ ptr", session_id);
      return result;
   }

   // get a buffer
   qurt_elite_bufmgr_node_t bufMgrNode;
   int actualSize;
   result = elite_mem_get_buffer(sizeof(EliteMsg_CustomASMCfgPPOutput),
                                 &bufMgrNode,
                                 &actualSize);
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
      return result;
   }

   // populate payload
   EliteMsg_CustomASMCfgPPOutput *pPayload =
         (EliteMsg_CustomASMCfgPPOutput *)bufMgrNode.pBuffer;
   pPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pPayload->pResponseQ = pAudStatAdmSvc->admResponseQ;
   pPayload->unClientToken = 0;
   pPayload->unSecOpCode = ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT;
   pPayload->outputSamplingRate = sampling_rate;
   pPayload->outputNumChannels =
         (ADM_NUM_CHANNELS_EIGHT >= num_channels) ? num_channels: ADM_NUM_CHANNELS_EIGHT;
   for (uint32_t i = 0; i < pPayload->outputNumChannels; i++)
   {
      pPayload->outputChannelMap[i] = channel_mapping[i];
   }
   pPayload->outputBitsPerSample = bits_per_sample;
   pPayload->unStreamID = (0x00ff & (uint16_t)(session_id));

   elite_msg_any_t msg;
   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void *)pPayload;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(asmSessionCmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to queue");
      goto __bailout;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailout;
   }

   result = (ADSPResult)pPayload->unResponseResult;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
      goto __bailout;
   }

__bailout:
   elite_msg_release_msg(&msg);
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "ADM: ELITEMSG_CUSTOM_ASM_CFG_PP_OUTPUT finished with result 0x%x", result);
   return result;
}
