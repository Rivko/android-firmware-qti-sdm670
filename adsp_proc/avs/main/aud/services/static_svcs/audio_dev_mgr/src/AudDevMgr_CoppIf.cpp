/**
@file AudDevMgr_CoppIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with COPP.
*/

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_CoppIf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/12   KR      Non-backward compatible API changes for Badger. 
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "AudDevMgr_CoppIf.h"
#include "AudDevMgr_mmpm.h"
#include "voice_proc_mgr.h"
#include "VoiceProcTx.h"
#include "adsp_private_api.h"

/* =======================================================================
Function Definitions
========================================================================== */

ADSPResult AdmSendCoppMsg(elite_svc_handle_t *phCopp,
                          qurt_elite_queue_t *responseQ,
                          elite_svc_handle_t *hAfeClient,
                          uint32_t cmd)
{
    uint32_t  unPayloadSize;
    ADSPResult result;
    elite_msg_any_t msg;
    unPayloadSize = sizeof( elite_msg_cmd_connect_t);
    if ( ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize, cmd,
                                                responseQ, 0, 0 )))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
       return result;
    }
    ((elite_msg_cmd_connect_t *) (msg.pPayload))->pSvcHandle = hAfeClient;

    if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg )))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to PP");
       goto __bailoutCmdSendCoppMsg;
    }

    //Need to wait for an Ack here, should not be cmdQ, should be a temp responseq
    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
       goto __bailoutCmdSendCoppMsg;
    }

    result = (ADSPResult)((elite_msg_cmd_connect_t *) (msg.pPayload))->unResponseResult;
    if (ADSP_EOK != result)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
    }
__bailoutCmdSendCoppMsg:
    elite_msg_release_msg(&msg);
    return result;
};

ADSPResult Adm_SendCmdRunCopp(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ)
{
    uint32_t  unPayloadSize, cmd;
    ADSPResult result;
    elite_msg_any_t msg;
    cmd = ELITE_CMD_RUN;
    unPayloadSize = sizeof( elite_msg_cmd_run_t);
    if(ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize, cmd,
                                                responseQ, 0, 0)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
       return result;
    }

    if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to PP");
       goto __bailoutCmdRunCopp;
    }

    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
       goto __bailoutCmdRunCopp;
    }

    result = (ADSPResult)((elite_msg_cmd_run_t *) (msg.pPayload))->unResponseResult;
    if (ADSP_EOK != result)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
    }
__bailoutCmdRunCopp:
    elite_msg_release_msg(&msg);
    return result;
}

ADSPResult Adm_SendCmdCoppSetParams(elite_svc_handle_t *phCopp,
                                       qurt_elite_queue_t *responseQ,
                                       uint32_t opcode,
		                               int32_t *pSetParamsCmdBase, 
									   uint16_t unDataPayloadSize,
									   uint32_t calVersion)
{
   uint32_t  unPayloadSize;
   ADSPResult result;
   elite_msg_any_t msg;
   elite_msg_param_cal_t *pParamCalType;

   unPayloadSize = sizeof( elite_msg_param_cal_t);
   if(ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize, opcode,
                                               responseQ, 0, 0)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
      return result;
   }

   pParamCalType = (elite_msg_param_cal_t*)msg.pPayload;
   pParamCalType->unParamId = calVersion;
   pParamCalType->pnParamData = pSetParamsCmdBase;
   pParamCalType->unSize = unDataPayloadSize;

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: SetPPParam. pnParamData: 0x%lx, Size: %u",
		   (uint32_t)pParamCalType->pnParamData, pParamCalType->unSize);

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to PP");
      goto __bailoutCmdCoppSetParams;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailoutCmdCoppSetParams;
   }

   result = (ADSPResult)pParamCalType->unResponseResult;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
   }
__bailoutCmdCoppSetParams:
   elite_msg_release_msg(&msg);
   return result;
}

ADSPResult Adm_SendCmdStartCopp(elite_svc_handle_t *phCopp, qurt_elite_queue_t *responseQ,   
					void *pAfeDriftPtr, void *pAfeDriftPtr_EP2)
{
    uint32_t  unPayloadSize, cmd;
    ADSPResult result;
    elite_msg_any_t msg;
	elite_msg_custom_voiceproc_run_payload_t *payload_ptr;

    cmd = ELITE_CUSTOM_MSG;
    unPayloadSize = sizeof( elite_msg_custom_voiceproc_run_payload_t);
    
    if(ADSP_FAILED(result = elite_msg_create_msg( &msg, &unPayloadSize, cmd,
                                                responseQ, 0, 0)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create mesage");
       return result;
    }

	payload_ptr = (elite_msg_custom_voiceproc_run_payload_t *) msg.pPayload;
    payload_ptr->unSecOpCode = VOICEPROC_RUN_CMD;
    payload_ptr->afe_drift_ptr = pAfeDriftPtr;
    payload_ptr->far_src_drift_ptr = pAfeDriftPtr_EP2;

	MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM Drift Ptr: NearEnd %p, FarEnd %p", pAfeDriftPtr, pAfeDriftPtr_EP2);

    if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to PP");
       goto __bailoutCmdStartCopp;
    }

    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
       goto __bailoutCmdStartCopp;
    }

    result = (ADSPResult)((elite_msg_cmd_start_svc_t *) (msg.pPayload))->unResponseResult;
    if (ADSP_EOK != result)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
    }
__bailoutCmdStartCopp:
    elite_msg_release_msg(&msg);
    return result;
}

ADSPResult adm_config_pp_output_media_format(elite_svc_handle_t *phCopp,
                                             qurt_elite_queue_t *responseQ,
                                             uint32_t unNumChannels,
                                             uint8_t *pChannelMapping,
                                             uint32_t unSampleRate,
                                             uint32_t unBitsPerSample)
{
   qurt_elite_bufmgr_node_t             bufMgrNode;
   ADSPResult                       result;
   elite_msg_any_t                  msg;
   EliteMsg_CustomCfgPPOutputType   *pCfgPPOutputPayload;
   int                              nActualSize;
   uint32_t                				i;

   if ( ADSP_FAILED(result = (elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgPPOutputType), &bufMgrNode, &nActualSize))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
      return result;
   }
   pCfgPPOutputPayload = (EliteMsg_CustomCfgPPOutputType*) bufMgrNode.pBuffer;

   pCfgPPOutputPayload->pResponseQ = responseQ;
   pCfgPPOutputPayload->unClientToken = 0;
   pCfgPPOutputPayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pCfgPPOutputPayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_PP_OUTPUT;

   pCfgPPOutputPayload->params.keepInputSamplingRate = TRUE;
   pCfgPPOutputPayload->params.outputSamplingRate = 0;
   if ((0 < unSampleRate) && (unSampleRate <= ADM_SAMPLING_RATE_MAX))
   {
      pCfgPPOutputPayload->params.keepInputSamplingRate = FALSE;
      pCfgPPOutputPayload->params.outputSamplingRate = unSampleRate;
   }

   pCfgPPOutputPayload->params.useNativeBitsPerSample = TRUE;
   pCfgPPOutputPayload->params.outputBitsPerSample = 0;
   if (0 != unBitsPerSample)
   {
      pCfgPPOutputPayload->params.useNativeBitsPerSample = FALSE;
      pCfgPPOutputPayload->params.outputBitsPerSample = unBitsPerSample;
   }

   pCfgPPOutputPayload->params.useNativeNumChannels = FALSE;
   pCfgPPOutputPayload->params.outputNumChannels = unNumChannels;
   if (pChannelMapping)
   {
      for (i = 0; i < unNumChannels; i++)
      {
          pCfgPPOutputPayload->params.outChannelMap[i] = *pChannelMapping++;
      }
   }
   else
   {
       result = ADSP_EBADPARAM;
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error: Channel mapping must be sent to COPP");
       qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);
       return result;
   }

   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void*)pCfgPPOutputPayload;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to PP");
      goto __bailoutCmdConfigPPOutputMediaFormat;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailoutCmdConfigPPOutputMediaFormat;
   }

   result = (ADSPResult)pCfgPPOutputPayload->unResponseResult;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
   }
__bailoutCmdConfigPPOutputMediaFormat:
   elite_msg_release_msg(&msg);
   return result;
}

void Adm_SendCmdCoppSetParamsDriftPtrSetup(AdmStatSvc_InfoType *pAdmStatSvc, uint16_t unCoppID, uint16_t unPrimaryDevice)
{
	uint32_t                             cmd = ELITE_CMD_SET_PARAM;
    int                                  result = ADSP_EOK;
    int32_t                              *pSetParamsCmdBase;
    adm_copp_cal_drift_ptr_param_data_t  coppDriftPtrParamData;
    AdmCalPort_Type                      *phCalPort, *phCalPrimaryPort;
    elite_svc_handle_t                   *pCoppHandle;

    //Obtain COPP handles and information
	phCalPort = &pAdmStatSvc->deviceCalPort[unCoppID];
    pCoppHandle = phCalPort->phDevCopp;

	//Setup the COPP SetParam payload
	coppDriftPtrParamData.pAddr = NULL;

	//The payload size is the size of the entire structure minus the size of pAddr and data_payload_size
	//4+4+2+2+sizeof(struct audproc_sampleslip_drift_pointer_t)+4+4+2+2+sizeof(struct audproc_sampleslip_enable_t);
	coppDriftPtrParamData.data_payload_size = sizeof(adm_copp_cal_drift_ptr_param_data_t) - sizeof(uint32_t*) - sizeof(uint32_t);

	coppDriftPtrParamData.unModuleId1 = AUDPROC_MODULE_ID_SAMPLESLIP;
	coppDriftPtrParamData.unParamId1 = AUDPROC_PARAM_ID_SAMPLESLIP_DRIFT_POINTER;
	coppDriftPtrParamData.usParamSize1 = sizeof(struct audproc_sampleslip_drift_pointer_t);
	coppDriftPtrParamData.usReserved1 = 0;
    // set primary drift pointer only if primary device is set.	
    if ((ADM_INVALID_PRIMARY_DEVICE != unPrimaryDevice) && (unPrimaryDevice < ADM_MAX_COPPS))
    {
       phCalPrimaryPort = &pAdmStatSvc->deviceCalPort[unPrimaryDevice];		
       coppDriftPtrParamData.sCoppDriftPtrStruct.primary_drift_info_ptr = phCalPrimaryPort->unEID1.pAfeDriftPtr;
    }
    else 	
    {
       coppDriftPtrParamData.sCoppDriftPtrStruct.primary_drift_info_ptr = NULL;	
    }

	coppDriftPtrParamData.sCoppDriftPtrStruct.current_drift_info_ptr = phCalPort->unEID1.pAfeDriftPtr;
	coppDriftPtrParamData.sCoppDriftPtrStruct.stream_to_device_drift_info_ptr = NULL;

	coppDriftPtrParamData.unModuleId2 = AUDPROC_MODULE_ID_SAMPLESLIP;
	coppDriftPtrParamData.unParamId2 = AUDPROC_PARAM_ID_SAMPLESLIP_ENABLE;
	coppDriftPtrParamData.usParamSize2 = sizeof(struct audproc_sampleslip_enable_t);
	coppDriftPtrParamData.usReserved2 = 0;
	coppDriftPtrParamData.sCoppSampleSlipEnableStruct.enable = 1;

	// If EC module in COPP path, configure sample slip in far end port.
    if(phCalPort->phDevCoppEcRef != NULL)
    {
    	coppDriftPtrParamData.unModuleId1 = AUDPROC_MODULE_ID_SAMPLESLIP_FAR;
    	coppDriftPtrParamData.unModuleId2 = AUDPROC_MODULE_ID_SAMPLESLIP_FAR;
    	QURT_ELITE_ASSERT(coppDriftPtrParamData.sCoppDriftPtrStruct.primary_drift_info_ptr != NULL);
    	coppDriftPtrParamData.sCoppDriftPtrStruct.current_drift_info_ptr = phCalPort->unEID2.pAfeDriftPtr;
    }

	pSetParamsCmdBase = (int32_t*)(&(coppDriftPtrParamData.unModuleId1));

	//Call SetParam
    if (ADSP_FAILED(result= Adm_SendCmdCoppSetParams(pCoppHandle, pAdmStatSvc->admResponseQ, cmd, pSetParamsCmdBase, coppDriftPtrParamData.data_payload_size, ELITEMSG_PARAM_ID_CAL)))
    {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ADM_CMD_SET_PP_PARAMS (Drift Ptr setup) cmd on COPP %d failed, result = %d. Exit gracefully", unCoppID, result);
	   return;
    }

	return;
}

/* This function reconfigures COPP output sampling rate to required value*/
ADSPResult Adm_SendCmdCoppSetOutputSamplingRate(elite_svc_handle_t *phCopp,
                                                qurt_elite_queue_t *responseQ, uint32_t unSampleRate)
{
   qurt_elite_bufmgr_node_t                   bufMgrNode;
   ADSPResult                                 result;
   elite_msg_any_t                            msg;
   EliteMsg_CustomCfgPPOutputSampleRateType   *pCfgPPOutputSampleRatePayload;
   int                                        nActualSize;

   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: cmd set COPP output sample rate to %lu, begin", unSampleRate);
   
   if ( ADSP_FAILED(result = (elite_mem_get_buffer(sizeof(EliteMsg_CustomCfgPPOutputSampleRateType), &bufMgrNode, &nActualSize))))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to create message");
      return result;
   }
   pCfgPPOutputSampleRatePayload = (EliteMsg_CustomCfgPPOutputSampleRateType*) bufMgrNode.pBuffer;

   pCfgPPOutputSampleRatePayload->pResponseQ = responseQ;
   pCfgPPOutputSampleRatePayload->unClientToken = 0;
   pCfgPPOutputSampleRatePayload->pBufferReturnQ = bufMgrNode.pReturnQ;
   pCfgPPOutputSampleRatePayload->unSecOpCode = ELITEMSG_CUSTOM_CFG_PP_OUTPUT_SAMPLE_RATE;

   pCfgPPOutputSampleRatePayload->keepInputSamplingRate = FALSE;   
   pCfgPPOutputSampleRatePayload->outputSamplingRate = unSampleRate;

   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void*)pCfgPPOutputSampleRatePayload;

   if (ADSP_FAILED(result = qurt_elite_queue_push_back(phCopp->cmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to COPP");
      goto __bailoutCmdConfigPPOutputSampleRate;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailoutCmdConfigPPOutputSampleRate;
   }

   result = (ADSPResult)pCfgPPOutputSampleRatePayload->unResponseResult;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
   }
__bailoutCmdConfigPPOutputSampleRate:
   elite_msg_release_msg(&msg);
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: cmd set COPP output sample rate to %lu, end", unSampleRate);
   return result;   
 }


ADSPResult Adm_FlushCopp(AdmStatSvc_InfoType *pAdmStatSvc,
                         AdmCalPort_Type *phCalPort,
                         uint16_t unCoppID)
{
   if (!pAdmStatSvc || !phCalPort || !phCalPort->phDevCopp)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Flush Copp rcvd NULL ptrs");
      return ADSP_EBADPARAM;
   }

   elite_svc_handle_t *pProcSvcHandle = phCalPort->phDevCopp;
   ADSPResult result;
   elite_msg_any_t msg;
   uint32_t unPayloadSize = sizeof(elite_msg_any_payload_t);
   result = elite_msg_create_msg(&msg,
                                 &unPayloadSize,
                                 ELITE_CMD_FLUSH,
                                 pAdmStatSvc->admResponseQ,
                                 0,
                                 0);
   if (ADSP_FAILED(result))
   {

      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to allocate mem for FLUSH msg ");
      return result;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ADM: Flushing COPP ID #%lu svc 0x%p",
         unCoppID, pProcSvcHandle);
   elite_msg_any_payload_t* pMsgPayload = (elite_msg_any_payload_t*)msg.pPayload;

   result = qurt_elite_queue_push_back(phCalPort->phDevCopp->cmdQ, (uint64_t*)(&msg));
   if (ADSP_FAILED(result))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to COPP command queue");
      goto __bailout;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailout;
   }

   result = (ADSPResult)pMsgPayload->unResponseResult;
   if (ADSP_FAILED(result))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ADM: Failed to FLUSH COPP ID #%lu svc 0x%p",
            unCoppID, pProcSvcHandle);
      goto __bailout;
   }

__bailout:
   elite_msg_release_msg(&msg);
   return result;
}
