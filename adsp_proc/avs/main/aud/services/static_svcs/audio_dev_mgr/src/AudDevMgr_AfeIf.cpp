/**
@file AudDevMgr_AfeIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with AFE.
*/

/*========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_AfeIf.cpp#1 $

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
#include "AudDevMgr_AfeIf.h"
#include "AFEInterface.h"

/* =======================================================================
Function Definitions
========================================================================== */
ADSPResult Adm_SendCmdDisconnectAFE(AdmStatSvc_InfoType *pAudStatAdmSvc, uint32_t port,
												elite_svc_handle_t *phAfeClient, uint32_t cmd)
{
    qurt_elite_bufmgr_node_t         bufMgrNode;
    elite_msg_any_t               msg;
    elite_msg_custom_afe_connect_t  *pConnectMsgPayload;
    ADSPResult                    result;
    qurt_elite_queue_t              *afeCmdQ;
    int                          nActualSize;
    if (ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
    {
       MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "ADM: Failed to create mesage");
       return result;
    }
    pConnectMsgPayload = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;
    pConnectMsgPayload->afe_id = port;
    pConnectMsgPayload->responseq_ptr = pAudStatAdmSvc->admResponseQ;
    pConnectMsgPayload->client_token = 0;
    pConnectMsgPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
    pConnectMsgPayload->sec_op_code =cmd;
    pConnectMsgPayload->svc_handle_ptr = phAfeClient;

    msg.unOpCode = ELITE_CUSTOM_MSG;
    msg.pPayload = (void*) pConnectMsgPayload;
    if (qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL)
    {
       result = ADSP_EFAILED;
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to get AFE cmdQ ptr");
       goto __bailoutCmdDisconnectAFE;
    }
    afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
    if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to AFE");
       goto __bailoutCmdDisconnectAFE;
    }

    if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
    {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
       goto __bailoutCmdDisconnectAFE;
    }

    result = (ADSPResult)pConnectMsgPayload->response_result;
    if (ADSP_EOK != result)
    {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
    }
__bailoutCmdDisconnectAFE:
    elite_msg_release_msg(&msg);
    return result;
}


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
                                 bool_t bShouldSubscribeToAVDrift)
{
   elite_msg_custom_afe_connect_t		*pConnectMsgPayload;
   qurt_elite_bufmgr_node_t				bufMgrNode;
   ADSPResult									result;
   qurt_elite_queue_t						*afeCmdQ;
   elite_msg_any_t                 		msg;
   int                            		nActualSize;
   uint32_t                				i;
   uint32_t 					unAfeFrameSizeInSamples;

   if ( ADSP_FAILED(result = elite_mem_get_buffer(sizeof(elite_msg_custom_afe_connect_t), &bufMgrNode, &nActualSize)))
   {
      MSG(MSG_SSID_QDSP6,DBG_ERROR_PRIO, "ADM: Failed to create mesage");
      return result;
   }

   elite_svc_get_frame_size(sample_rate, &unAfeFrameSizeInSamples);
  
   pConnectMsgPayload = (elite_msg_custom_afe_connect_t*) bufMgrNode.pBuffer;

   memset(&pConnectMsgPayload->afe_client, 0, sizeof(afe_client_t));
   pConnectMsgPayload->afe_client.sample_rate = sample_rate;
   pConnectMsgPayload->afe_client.data_path   = data_path;
   pConnectMsgPayload->afe_client.cust_proc.subscribe_to_avt_drift = bShouldSubscribeToAVDrift;
   pConnectMsgPayload->afe_client.channel = unNumChannels;
   pConnectMsgPayload->afe_client.buf_size = unAfeFrameSizeInSamples * unNumAfeFrames;
   pConnectMsgPayload->afe_client.num_buffers =  num_buffers;
   pConnectMsgPayload->afe_client.bytes_per_channel = (bit_width>ADM_BITS_PER_SAMPLE_16 ? ADM_BYTES_PER_SAMPLE_FOUR : ADM_BYTES_PER_SAMPLE_TWO);
   pConnectMsgPayload->afe_client.bit_width = bit_width;
   pConnectMsgPayload->afe_client.interleaved = 0;
   pConnectMsgPayload->afe_client.afe_client_handle =  pCoppHandle;
   pConnectMsgPayload->afe_client.afe_dp_delay_ptr = punAFEDelay;

   if (pChannelMapping)
   {
      for (i = 0; i < unNumChannels; i++)
      {
		  pConnectMsgPayload->afe_client.channel_mapping[i] = *pChannelMapping++;
      }
   }
   else
   {
       result = ADSP_EBADPARAM;
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error: Channel mapping must be sent to AFE");
       qurt_elite_queue_push_back(bufMgrNode.pReturnQ, (uint64_t*)&bufMgrNode);
       return result;
   }
   pConnectMsgPayload->afe_id = port;
   pConnectMsgPayload->responseq_ptr = pAudStatAdmSvc->admResponseQ;
   pConnectMsgPayload->client_token = 0;
   pConnectMsgPayload->buffer_returnq_ptr = bufMgrNode.pReturnQ;
   pConnectMsgPayload->sec_op_code = cmd; 

   msg.unOpCode = ELITE_CUSTOM_MSG;
   msg.pPayload = (void*) pConnectMsgPayload;

   if (qurt_elite_globalstate.pAfeStatSvcCmdQ == NULL)
   {
      result = ADSP_EFAILED;
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to get AFE cmdQ ptr");
      goto __bailoutCmdConnectAFE;

   }

   afeCmdQ = qurt_elite_globalstate.pAfeStatSvcCmdQ;
   if (ADSP_FAILED(result = qurt_elite_queue_push_back(afeCmdQ, (uint64_t*)&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Failed to push cmd to AFE");
      goto __bailoutCmdConnectAFE;
   }

   if (ADSP_FAILED(result = elite_svc_wait_for_ack(&msg)))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Wait for ack failed");
      goto __bailoutCmdConnectAFE;
   }

   result = (ADSPResult)pConnectMsgPayload->response_result;
   if (ADSP_EOK != result)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: Error [0x%x] in cmd ack", result);
      goto __bailoutCmdConnectAFE;
   }

   pConnectMsgPayload = (elite_msg_custom_afe_connect_t *) msg.pPayload;
   *pphAfe = pConnectMsgPayload->svc_handle_ptr;
   *ppAfeDriftPtr = pConnectMsgPayload->afe_drift_ptr;

__bailoutCmdConnectAFE:
   elite_msg_release_msg(&msg);
   return result;
}
