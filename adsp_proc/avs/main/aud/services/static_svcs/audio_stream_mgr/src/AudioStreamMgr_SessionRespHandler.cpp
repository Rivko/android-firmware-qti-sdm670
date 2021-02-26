/**
@file AudStreamMgr_SessionRespHandler.cpp

@brief This file contains the implementation for session response handler functions
for AudioStreamMgr.

 */

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_SessionRespHandler.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
04/01/10   DC      Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/




/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudioStreamMgr_Type.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_DevIF.h"
#include "AudioStreamMgr_Util.h"
#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "AudioStreamMgr_Session.h"
#include "AudioStreamMgr_StreamCmdHandler.h"
#include "AudioStreamMgr_SessionCmdHandler.h"
#include "AudioStreamMgr_StreamUtil.h"
#include "AudioStreamMgr_SessionRespHandler.h"
#include "AudioStreamMgr_StreamRespHandler.h"
#include "AudioStreamMgr_EliteMsg_Custom.h"
#include "adsp_media_fmt.h"

#include "EliteMsg_Custom.h"
#include "EliteMsg_AfeCustom.h"
#include "EliteMsg_AdmCustom.h"

#include "AudDynaPPSvc.h"
#include "AudioDecSvc.h"
#include "AudioEncSvc.h"
#include "SwitchSvc.h"
#include "StreamRouterSvc.h"

#include "MixerSvc_Interface.h"
#include "AudioStreamMgr_adsppm.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
extern uint32_t asmMemoryMapClient;

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */


/* =======================================================================
 **        Session State Machine: RespQ message handler
 ** ======================================================================= */

/* Rules of thumbs:
1. releasing messages sent to RespQ:
message is releasesd by the message handler itself at the top level.
Utility function do not release message.
 */


ADSPResult AudioStreamMgr_SessionRespQUnSupportedHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result;
   // Basic rule, final consumer release the message.
   if ( ADSP_FAILED( result = elite_msg_release_msg(&pSession->msg)) ) {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,-]: Fail to finish msg: 0x%lx",
            pSession->ucInternalSessionId,   (uint32_t) result);
   }
   return ADSP_EUNSUPPORTED;
}

/** Process custom msg on respq. Currently only AFE connection */
ADSPResult AudioStreamMgr_SessionRespQCustomMsgHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t*) pSession->msg.pPayload;

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: Received custom response/event with opcode 0x%lx", pSession->ucInternalSessionId,
         pPayload->unSecOpCode);

   ADSPResult result = ADSP_EOK;
   // Given the potential custom messages.
   switch (pPayload->unSecOpCode)
   {
      case ELITEMSG_CUSTOM_MT_MX_REGISTER_UNDERFLOW_OVERFLOW_EVENT:
      {
         result = AudioStreamMgr_SessionRespQRegMtMxFlowEventHandler(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_ADM_STREAM_CONNECT:
      {
         result = AudioStreamMgr_SessionRespQOnAdmConnect(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_ADM_STREAM_DISCONNECT:
      {
         result = AudioStreamMgr_SessionRespQOnAdmDisConnect(pMe,pSession);
         break;
      }

      case ELITEMSG_CUSTOM_ADM_STREAM_DISABLE:
      {
         result = AudioStreamMgr_SessionRespQOnAdmDisable(pMe,pSession);
         break;
      }

      case ELITEMSG_CUSTOM_MT_MX_ADJUST_SESSION_CLK:
      {
         result = AudioStreamMgr_SessionRespQAdjustSessionClkHandler(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_MT_MX_RUN:
      case ELITEMSG_CUSTOM_STREAM_ROUTER_RUN:
      {
         result = AudioStreamMgr_SessionRespQOnRunAck(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_DEC_SUSPEND:
      case ELITEMSG_CUSTOM_ENC_SUSPEND:
      case ELITEMSG_CUSTOM_MT_MX_SUSPEND:
      case ELITEMSG_CUSTOM_PP_SUSPEND:
      case ELITEMSG_CUSTOM_STREAM_ROUTER_SUSPEND:
      {
         result = AudioStreamMgr_SessionRespQOnSuspendAck(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_MT_MX_PAUSE:
      case ELITEMSG_CUSTOM_STREAM_ROUTER_PAUSE:
      {
         result = AudioStreamMgr_SessionRespQOnPause2Ack(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_MT_MX_FLUSH:
      case ELITEMSG_CUSTOM_STREAM_ROUTER_FLUSH:
      {
         result = AudioStreamMgr_SessionRespQOnFlushAck(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_FLUSH_READBUFS:
      {
         result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_CFG_PP_OUTPUT:
      {
         result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_PP_PAUSE_PREPARE:
      {
         result = AudioStreamMgr_SessionRespQOnPause1Ack(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_PP_PAUSE:
      {
         result = AudioStreamMgr_SessionRespQOnPause2Ack(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_SWITCH_GET_SVC_HANDLE:
      {
         result = AudioStreamMgr_SessionRespQOnGetSwitchSvcHandle(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_SWITCH_VACATE_PORT:
      {
         result = AudioStreamMgr_SessionRespQOnSwitchVacatePort(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_SWITCH_FLUSH:
      {
         result = AudioStreamMgr_SessionRespQOnSwitchFlush(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_DTMF_CTL:
      {
         result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_ADM_STREAM_MTMX_RECONFIG:
      {
         result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
         break;
      }

      case ELITEMSG_CUSTOM_CFG_PP_OUTPUT_SAMPLE_RATE:
      {
         result = AudioStreamMgr_SessionRespQGenericAckForInternalMsg(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY:
      {
         result = AudioStreamMgr_SessionRespQGetDevicePathDelay(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_MT_MX_SET_PARAM:
      case ELITEMSG_CUSTOM_STREAM_ROUTER_SET_PARAM:
      {
         result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_MT_MX_GET_PARAM:
      {
         result = AudioStreamMgr_SessionRespQGetMtMxStRtrParam(pMe, pSession);
         break;
      }
    case ELITEMSG_CUSTOM_MT_MX_SET_RATEMATCHING_PARAM:
  	 {
  		result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
  		break;
     }
      case ELITEMSG_CUSTOM_STREAM_ROUTER_GET_PARAM:
      {
         result = AudioStreamMgr_SessionRespQGetMtMxStRtrParam(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES:
      case ELITEMSG_CUSTOM_GET_PP_TOPOLOGY_MODULES_V2:
      {
         result = AudioStreamMgr_SessionRespQGetPPTopologyModulesHandler(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_DEC_OPEN_STREAM:
      {
         result = AudioStream_Mgr_SessionRespQDecOpenStream(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_DEC_CLOSE_STREAM:
      {
         result = AudioStream_Mgr_SessionRespQDecCloseStream(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_DEC_CONNECT_STREAM:
      {
         result = AudioStream_Mgr_SessionRespQDecConnectStream(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS:
      {
         result = AudioStreamMgr_SessionRespQGetPPNumOutputBuffers(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_ENC_PREPARE_DISCONNECT:
      {
         result = AudioStreamMgr_SessionRespQGenericAckForInternalMsg(pMe, pSession);
         break;
      }
	  case ELITE_CMD_REGISTER_APR_EVENT:
	  {
		  result = AudioStreamMgr_SessionRespQGenericAck(pMe, pSession);
		  break;
	  }
      case ELITEMSG_CUSTOM_DEC_SET_PP_PARAM:
      {
         result = AudioStreamMgr_SessionRespQSetPpParamHandler(pMe, pSession);
         break;
      }
      case ELITEMSG_CUSTOM_DEC_GET_PP_PARAM:
      {
         result = AudioStreamMgr_SessionRespQGetPpParamHandler(pMe, pSession);
         break;
      }
      default:
      {
         AudioStreamMgr_LogErrMsg( &(pSession->msg));
         elite_msg_release_msg( &pSession->msg);
         result = ADSP_EUNSUPPORTED;
         break;
      }

   }

   return result;
}

/* Set PP Param Resp Handler */
ADSPResult AudioStreamMgr_SessionRespQSetPpParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   EliteMsg_CustomDecSetGetPpParamType *pPayload = (EliteMsg_CustomDecSetGetPpParamType *) pSession->msg.pPayload;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Set PP Parm ACK Tok 0x%lx, Res 0x%lx, ParamId 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult, pPayload->unParamId);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: SetParam fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
   }

   switch ( pPayload->unParamId )
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         return AudioStreamMgr_SessionRespQGenericAck(pMe, pSession); //calls ASM_OnPopFromRespQ
      }
      default:
         ASM_OnPopFromRespQ(pMe, pSession);
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Uexpected SetParm ACK T0x%lx, Id0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unClientToken, pPayload->unParamId);
         break;
   } // switch ( pPayload->unParamId ) {

   // release the message and return
   return elite_msg_release_msg(&pSession->msg);
}

/* Get PP Param Resp Handler */
ADSPResult AudioStreamMgr_SessionRespQGetPpParamHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   EliteMsg_CustomDecSetGetPpParamType *pPayload = (EliteMsg_CustomDecSetGetPpParamType *) pSession->msg.pPayload;
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   ADSPResult result = ADSP_EOK;

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Get PP Parm ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);

   ASM_OnPopFromRespQ(pMe, pSession);

   switch ( pPayload->unParamId )
   {
      case ELITEMSG_PARAM_ID_CAL:
      case ELITEMSG_PARAM_ID_CAL_V2:
      {
         elite_apr_packet_t *pPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

         if(ELITEMSG_PARAM_ID_CAL == pPayload->unParamId)
         {
			 asm_stream_param_data_v2_t *psPPParamData = (asm_stream_param_data_v2_t *)(pPayload->pnParamData);
			 psPPParamData->param_size = (uint16_t)pPayload->unSize;
         }
         else
         {
			 asm_stream_param_data_v3_t *psPPParamData = (asm_stream_param_data_v3_t *)(pPayload->pnParamData);
			 psPPParamData->param_size = pPayload->unSize;
         }

         // Generate ACK.
         // for in-band, pPkt is created by ASM & it has asm_stream_cmdrsp_get_pp_params_v2_t. Client created packet with asm_stream_cmd_get_pp_params_v2_t
         // is already released in case of in-band.
         if ( ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt) ) //in-band
         {
            asm_stream_cmdrsp_get_pp_params_v2_t *pRetPayload = (asm_stream_cmdrsp_get_pp_params_v2_t*)(elite_apr_if_get_payload_ptr(pPkt));
            pRetPayload->status = pPayload->unResponseResult;

            //no need to flush cache since data is in-band

            result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
         }
         else if ( ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3 == elite_apr_if_get_opcode(pPkt) ) //in-band
         {
            asm_stream_cmdrsp_get_pp_params_v3_t *pRetPayload = (asm_stream_cmdrsp_get_pp_params_v3_t*)(elite_apr_if_get_payload_ptr(pPkt));
            pRetPayload->status = pPayload->unResponseResult;

            //no need to flush cache since data is in-band

            result = AudioStreamMgr_GenerateAck(pPkt, pPayload->unResponseResult, NULL, 0, 0 );
         }
         // for out-of-band pPkt is created by client & it has asm_stream_cmd_get_pp_params_v2_t
         else
         {
             elite_mem_shared_memory_map_t    bufferMemNode;
        	 if(ASM_STREAM_CMD_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt))
        	 {
                 asm_stream_cmd_get_pp_params_v2_t *pAprPayload =
                       (asm_stream_cmd_get_pp_params_v2_t *) ( elite_apr_if_get_payload_ptr(pPkt));
                 bufferMemNode.unMemMapClient     = asmMemoryMapClient;
                 bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
                 bufferMemNode.unMemSize          = pAprPayload->param_max_size;
                 bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
                 bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
                 bufferMemNode.unVirtAddr         = 0;
        	 }
        	 else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
        	 {
                 asm_stream_cmd_get_pp_params_v3_t *pAprPayload =
                       (asm_stream_cmd_get_pp_params_v3_t *) ( elite_apr_if_get_payload_ptr(pPkt));
                 bufferMemNode.unMemMapClient     = asmMemoryMapClient;
                 bufferMemNode.unMemMapHandle     = pAprPayload->mem_map_handle;
                 bufferMemNode.unMemSize          = pAprPayload->param_max_size;
                 bufferMemNode.unPhysAddrLsw      = pAprPayload->data_payload_addr_lsw;
                 bufferMemNode.unPhysAddrMsw      = pAprPayload->data_payload_addr_msw;
                 bufferMemNode.unVirtAddr         = 0;
        	 }

            //Flush the cache
            result =  elite_mem_map_get_shm_attrib(bufferMemNode.unPhysAddrLsw, bufferMemNode.unPhysAddrMsw,
                  bufferMemNode.unMemSize, &bufferMemNode);
            if (ADSP_FAILED(result))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]:Failed to map physical memory for GetParam.",
                     pSession->ucInternalSessionId,  stream_id);
               pPayload->unResponseResult = ADSP_EFAILED;
            }
            else
            {

               result = elite_mem_flush_cache(&bufferMemNode);
               if (ADSP_FAILED(result))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr [%u,%u]: Out-band GetPPParam failed to flush cache",
                        pSession->ucInternalSessionId,  stream_id );
                  pPayload->unResponseResult = ADSP_EFAILED;
               }
            }

            if(ASM_STREAM_CMD_GET_PP_PARAMS_V2 == elite_apr_if_get_opcode(pPkt))
            {
         	   asm_stream_cmdrsp_get_pp_params_v2_t response;
         	   response.status = pPayload->unResponseResult;
         	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
         			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2);
            }
            else //ASM_STREAM_CMD_GET_PP_PARAMS_V3
            {
         	   asm_stream_cmdrsp_get_pp_params_v3_t response;
         	   response.status = pPayload->unResponseResult;
         	   result =  AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, (void*)&response,
         			   sizeof(response), ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3);
            }
         }
         break;
      }
      default:
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Get PP Parm ACK invalid ParamID 0x%lx",
               pSession->ucInternalSessionId,  stream_id,
               pPayload->unParamId );
         break;
      }
   } // Sanity check on ack param ID

   // Output error messages in Q6 log
   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: GetParam fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      result = ADSP_EFAILED;
   }

   // Releasse message inside respQ
   elite_msg_release_msg(&pSession->msg);

   return ADSP_EOK;

}

ADSPResult AudioStreamMgr_SessionRespQCustomEventHandler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_custom_header_t *pPayload = (elite_msg_custom_header_t*) pSession->msg.pPayload;

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u,-]: Received custom event with opcode 0x%lx", pSession->ucInternalSessionId,
         pPayload->unSecOpCode);

   ADSPResult result = ADSP_EOK;
   // Given the potential custom messages.
   switch (pPayload->unSecOpCode)
   {
      case ELITE_EVENT_CUSTOM_ASM_LISTEN_TO_EOS_ACK:
      {
         result = AudioStreamMgr_SessionRespQListenToEoSAck(pMe, pSession);
         break;
      }
      default:
      {
         AudioStreamMgr_LogErrMsg( &(pSession->msg));
         elite_msg_release_msg( &pSession->msg);
         result = ADSP_EUNSUPPORTED;
         break;
      }

   }

   return result;
}


ADSPResult AudioStreamMgr_SessionRespQAdjustSessionClkHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( pPkt);
   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamForMtmxStRtrSessionCmd(pSession, port);
   uint8_t stream_id = 99;
   if (pStream)   stream_id = pStream->ucStreamId;

   ASM_OnPopFromRespQ(pMe, pSession);

   asm_session_cmdrsp_adjust_session_clock_v2_t errorRsp;
   //init error rsp
   memset(&errorRsp, 0, sizeof(errorRsp));

   EliteMsg_CustomMtMxAdjustSessionClkType *pPayload =
         (EliteMsg_CustomMtMxAdjustSessionClkType*) pSession->msg.pPayload;

   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Received Adjust Session Clk cmd ACK",
         pSession->ucInternalSessionId,  stream_id );

   if ( ADSP_FAILED(pPayload->unResponseResult) ) {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Adjust Session Clk failed",
            pSession->ucInternalSessionId,  stream_id );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      errorRsp.status = ADSP_EFAILED;
   }
   else
   {
      //fill up the response payload
      errorRsp.status = ADSP_EOK;
      errorRsp.actual_adjust_time_msw =
            (uint32_t)((pPayload->llActualAdjustTime >> 32) & 0x00000000FFFFFFFFLL);
      errorRsp.actual_adjust_time_lsw =
            (uint32_t)(pPayload->llActualAdjustTime & 0x00000000FFFFFFFFLL);
      errorRsp.cmd_latency_msw =
            (uint32_t)((pPayload->ullCmdLatency >> 32) & 0x00000000FFFFFFFFULL);
      errorRsp.cmd_latency_lsw =
            (uint32_t)(pPayload->ullCmdLatency & 0x00000000FFFFFFFFULL);
   }

   //send ack
   ADSPResult res = AudioStreamMgr_GenerateAck( pPkt, errorRsp.status,
         (void*)&errorRsp , sizeof(errorRsp) ,
         ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2 );

   //release response msg
   elite_msg_release_msg(&pSession->msg);

   return res;
}


/*
Handle the ACK from Proc and Matrix.
 */
ADSPResult AudioStreamMgr_SessionRespQOnRunAck( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload); //this pPkt may be either run command packet or an open command
   //packet depending on which command triggered run to other services
   ADSPResult result = ADSP_EOK;

   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex curr_node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( NULL == pStream )
   {
      /* Switch on channel to list to session cmdQ*/
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Run ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      /* Switch on channel to list to session cmdQ*/
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_StreamIdNodeIndex_t stream_id_node_ind;
   stream_id_node_ind.stream_id = stream_id;
   stream_id_node_ind.node_ind = curr_node_ind;

   ASM_GetNextSortedStreamAndNode(pSession, &stream_id_node_ind);

   //if stream-id changes,then assign the old stream state as RUN
   if (stream_id != stream_id_node_ind.stream_id)
   {
      if (stream_id_node_ind.stream_id != 0)
      {
         stream_id = stream_id_node_ind.stream_id;
      }
      pStream->state = ASM_STREAM_RUN;
   }

   ASM_GraphNodeIndex next_node_index = stream_id_node_ind.node_ind;

   pStream = ASM_GetStreamInstance(pSession, stream_id_node_ind.stream_id);

   if ((ASM_INVALID_NODE == next_node_index) || (NULL == pStream))
   {
      //no more svc in run topo. ack the client cmd
      pSession->unState = AUDIO_STREAM_MGR_SESSION_RUN;

      result = AudioStreamMgr_RequestReleasePm(pMe);

      if (ADSP_FAILED( result ) )
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u,%u]: cannot allocate resources from PM",
               pSession->ucInternalSessionId, stream_id);
      }

      result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
   }
   else
   {
      result = ASM_ExecRunFTableEntry(next_node_index, pMe, pSession, pStream);
      // if fail to send message, ACK back to client and relisten to cmd q
      if ( ADSP_FAILED( result ) || (pSession->unReqResps == 0))
      {
         result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
      }
   }

   elite_msg_release_msg(&pSession->msg);
   return result;

}

/*
Handle the pause ACK from Proc and Matrix.
 */
ADSPResult AudioStreamMgr_SessionRespQOnPause1Ack( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ADSPResult result = ADSP_EOK;

   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   ASM_OnPopFromRespQ(pMe, pSession);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pause ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);


   if ( ADSP_FAILED(pPayload->unResponseResult) ) {
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );
      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   //once all responses are available for pause1, send pause 2 serially.
   if(pSession->unReqResps == 0)
   {
      ASM_StoreNodesInTempStreamIdNodeIndexList(pSession, ASM_TOPO_TYPE_PAUSE2_TOPO);
      pStream = ASM_GetStreamInstance(pSession, pSession->aTempStreamIdNodeIndexList[0].stream_id);
      //since all PPs are in pause state now, we can pause other services.
      result = ASM_IterateRunPauseSuspendOverNodesAndStreams(pMe, pSession, pStream, ASM_INVALID_NODE, ASM_ExecPause2FTableEntry);
      if ( ADSP_FAILED( result ) || (pSession->unReqResps == 0))
      {
         result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
      }
   }

   elite_msg_release_msg(&pSession->msg);
   return result;
}

/*
Handle the pause ACK from Proc and Matrix.
 */
ADSPResult AudioStreamMgr_SessionRespQOnPause2Ack( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);

   ADSPResult result = ADSP_EOK;

   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex curr_node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( NULL == pStream ) {
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Pause ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);


   if ( ADSP_FAILED(pPayload->unResponseResult) ) {
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_StreamIdNodeIndex_t stream_id_node_ind;
   stream_id_node_ind.stream_id = stream_id;
   stream_id_node_ind.node_ind = curr_node_ind;

   ASM_GetNextSortedStreamAndNode(pSession, &stream_id_node_ind);

   //if stream-id changes,then assign the old stream state as PAUSE
   if (stream_id != stream_id_node_ind.stream_id)
   {
      if (stream_id_node_ind.stream_id != 0)
      {
         stream_id = stream_id_node_ind.stream_id;
      }
      pStream->state = ASM_STREAM_PAUSE;
   }


   ASM_GraphNodeIndex next_node_index = stream_id_node_ind.node_ind;

   pStream = ASM_GetStreamInstance(pSession, stream_id_node_ind.stream_id);

   if ((ASM_INVALID_NODE == next_node_index)||(NULL == pStream))
   {
      //reached the end of topology, ack the client cmd
      pSession->unState = AUDIO_STREAM_MGR_SESSION_PAUSE;
      result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, NULL, 0, 0 );
   }
   else
   {
      result = ASM_ExecPause2FTableEntry(next_node_index, pMe, pSession, pStream);
      if ( ADSP_FAILED( result ) || (pSession->unReqResps == 0) )
      {
         result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
      }
   }
   elite_msg_release_msg(&pSession->msg);
   return result;
}

/* Session response on Suspend Ack */
ADSPResult AudioStreamMgr_SessionRespQOnSuspendAck( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_apr_packet_t *pPkt = (elite_apr_packet_t*) ((pStSession->cmdMsg).pPayload);
   //elite_apr_port_t    port = elite_apr_if_get_dst_port( pPkt );
   ADSPResult result = ADSP_EOK;

   elite_msg_any_payload_t *pPayload = (elite_msg_any_payload_t *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
   ASM_GraphNodeIndex curr_node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);

   if ( NULL == pStream )
   {
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: Suspend ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);

   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: Suspend Failed Tok 0x%lx, Res 0x%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unClientToken, pPayload->unResponseResult);
      result = AudioStreamMgr_GenerateAck( pPkt, pPayload->unResponseResult, NULL, 0, 0 );

      elite_msg_release_msg(&pSession->msg);
      return result;
   }

   ASM_StreamIdNodeIndex_t stream_id_node_ind;
   stream_id_node_ind.stream_id = stream_id;
   stream_id_node_ind.node_ind = curr_node_ind;

   ASM_GetNextSortedStreamAndNode(pSession, &stream_id_node_ind);

   //if stream-id changes,then assign the old stream state as SUSPEND
   if (stream_id != stream_id_node_ind.stream_id)
   {
      if (stream_id_node_ind.stream_id != 0)
      {
         stream_id = stream_id_node_ind.stream_id;
      }
      pStream->state = ASM_STREAM_SUSPEND;
   }

   ASM_GraphNodeIndex next_node_index = stream_id_node_ind.node_ind;

   pStream = ASM_GetStreamInstance(pSession, stream_id_node_ind.stream_id);

   if((ASM_INVALID_NODE == next_node_index)||(NULL == pStream))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr :[Suspend] Successful \n");
      //reached the end of topology, ack the client cmd
      pSession->unState = AUDIO_STREAM_MGR_SESSION_SUSPEND;
      result = AudioStreamMgr_GenerateAck( pPkt, ADSP_EOK, NULL, 0, 0 );

      AudioStreamMgr_RequestReleasePm(pMe);
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr : Session Response Calling next node");
      result = ASM_ExecSuspendFTableEntry(next_node_index, pMe, pSession, pStream);
      if ( ADSP_FAILED( result ) || (pSession->unReqResps == 0))
      {
         result = AudioStreamMgr_GenerateAck( pPkt, result, NULL, 0, 0 );
      }
   }
   elite_msg_release_msg(&pSession->msg);
   return result;

}

ADSPResult AudioStreamMgr_SessionRespQGenericAck( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   elite_msg_any_payload_t *pRespPayload = (elite_msg_any_payload_t *) pSession->msg.pPayload;

   uint32_t status = pRespPayload->unResponseResult;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pRespPayload->unClientToken);
   uint32_t unClientToken = pRespPayload->unClientToken;

   // check if this is a response to an internal message.
   if (ASM_GetIsCustomCmdFromClientToken(unClientToken))
   {
      return AudioStreamMgr_SessionRespQGenericAckForInternalMsg(pMe, pSession);
   }

   // Handle normal ack back to client
   elite_msg_release_msg(&pSession->msg);

   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);
   elite_apr_packet_t *pAprPkt = (elite_apr_packet_t*) (pStSession->cmdMsg.pPayload);

   // update expected response mask
   ASM_OnPopFromRespQ(pMe, pSession);

   if ( ADSP_FAILED(status) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]: internal command to module %d failed w/ error code = %d",
            pSession->ucInternalSessionId,  stream_id, (int)unClientToken, (int)status);
   }

   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]: Token 0x%lx opcode = 0x%lx, unReqResps %lu",
         pSession->ucInternalSessionId,  stream_id, unClientToken, pAprPkt->opcode, pSession->unReqResps);

   ADSPResult result = ADSP_EOK;
   if(0 == pSession->unReqResps)
   {
      //we got all expected responses...ack back the client command
      uint32_t opCode = pAprPkt->opcode;
      result = AudioStreamMgr_GenerateAck(pAprPkt, pSession->nOverallResult, NULL, 0, 0 );
      if( ADSP_FAILED(result) )
      {
         MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]:sending ack to client command %x failed w/ error code = %d",
               pSession->ucInternalSessionId,  stream_id,
               (int)opCode, (int)result);
      }
   }

   return result;
}

ADSPResult AudioStreamMgr_SessionRespQGenericAckForInternalMsg( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   elite_msg_any_t pkt = (elite_msg_any_t) (pStSession->cmdMsg);
   elite_msg_custom_header_t *pRespPayload = (elite_msg_custom_header_t*) pSession->msg.pPayload;
   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pRespPayload->unClientToken);

   uint32_t status = pRespPayload->unResponseResult;

   uint32_t secOpcode =  pRespPayload->unSecOpCode;

   elite_msg_release_msg(&pSession->msg);

   ASM_OnPopFromRespQ(pMe, pSession);

   if ( ADSP_FAILED(status) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [Session ID = %u, Stream ID = %u]: internal command to module %d failed w/ error code = %d",
            pSession->ucInternalSessionId,  stream_id, (int)pRespPayload->unClientToken, (int)status);
   }

   uint32_t opcode = pkt.unOpCode;
   MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: opcode = 0x%lx, secOpCode=%lx, unReqResps %lu",
         pSession->ucInternalSessionId,  stream_id, opcode, secOpcode, pSession->unReqResps);

   if(0 == pSession->unReqResps)
   {
      //Generate ACK
      elite_svc_send_ack(&pkt, pSession->nOverallResult);
   }

   return ADSP_EOK;
}


ADSPResult AudioStreamMgr_SessionRespQRegMtMxFlowEventHandler( AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession )
{
   AudioStreamMgr_StaticSessionStateType *pStSession = AudioStreamMgr_GetStaticSessionState(pMe, pSession);

   EliteMsg_CustomMtMxUnderOverFlowEventType *pPayload = (EliteMsg_CustomMtMxUnderOverFlowEventType *)
            (pSession->msg.pPayload);
   elite_apr_packet_t *pInAprPkt = (elite_apr_packet_t *) (pStSession->cmdMsg.pPayload);

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   ASM_OnPopFromRespQ(pMe, pSession);

   ADSPResult result;

   MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"AudioStreamMgr [%u,%u]: RegMtMxFlowEvent ACK Tok 0x%lx, Res 0x%lx",
         pSession->ucInternalSessionId,  stream_id,
         pPayload->unClientToken, pPayload->unResponseResult);

   // Output error messages in Q6 log
   if ( ADSP_FAILED(pPayload->unResponseResult) )
   {
      MSG_4(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr [%u,%u]: RegMtMxFlowEvent fail, R%lx, T%lx",
            pSession->ucInternalSessionId,  stream_id,
            pPayload->unResponseResult,  pPayload->unClientToken
      );
      AudioStreamMgr_LogErrMsg(&pSession->msg);
      result = ADSP_EFAILED;
   }

   // Ack response and free apr packet
   result = AudioStreamMgr_GenerateAck(pInAprPkt, pPayload->unResponseResult, NULL, 0, 0 );

   // Releasse message inside respQ
   elite_msg_release_msg(&pSession->msg);

   return result;

}


/*======================================================================

FUNCTION      AudioStreamMgr_SessionRespQGetDevicePathDelay

DESCRIPTION
      Processes the response to getDevicePathDelay query & sends the response to the client.

DEPENDENCIES  none

RETURN VALUE  error code

SIDE EFFECTS  none

======================================================================*/
ADSPResult AudioStreamMgr_SessionRespQGetDevicePathDelay(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{
   ADSPResult result = ADSP_EOK;

   EliteMsg_CustomAdmGetDevicePathDelayCmd *pPayload =
         (EliteMsg_CustomAdmGetDevicePathDelayCmd *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Invalid Stream ID",
            pSession->ucInternalSessionId, stream_id);
      (void) AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, ADSP_EFAILED);
      return ADSP_EFAILED;
   }

   ASM_OnPopFromRespQ(pMe, pSession);

   result = pPayload->unResponseResult;

   if(ADSP_FAILED(result))
   {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: response received from ADM for ELITEMSG_CUSTOM_ADM_GET_DEVICE_PATH_DELAY. result = %d",
            pSession->ucInternalSessionId, stream_id, result);
   }
   else
   {
      //accumulate all delays.
      AudioStreamMgr_AggregateBufDelay(pStream, pPayload->direction, pPayload->device_path_buf_delay);
      pStream->pathAlgDelayUs += pPayload->device_path_alg_delay;

      MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Session path delay query for device side added buffering delay %lu, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay %lu, total %lu, result = %d",
            pSession->ucInternalSessionId, stream_id, pPayload->device_path_buf_delay, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pPayload->device_path_alg_delay, pStream->pathAlgDelayUs, result);
         }

   if (0 == pSession->unReqResps)
         {
      ADSPResult errorCode = ADSP_SUCCEEDED(pSession->nOverallResult) ? ADSP_EOK : ADSP_EFAILED;
      result = AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, errorCode);
      }

   //release the internal message
   elite_msg_release_msg(&pSession->msg);

   return result;
   }

ADSPResult AudioStreamMgr_SessionRespQGetPPNumOutputBuffers(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
      {
   ADSPResult result = ADSP_EOK;

   EliteMsg_CustomPPGetNumOutputBuffers *pPayload =
         (EliteMsg_CustomPPGetNumOutputBuffers *) pSession->msg.pPayload;

   uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);

   AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
   if(NULL == pStream)
         {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: Invalid Stream ID",
            pSession->ucInternalSessionId, stream_id);
      (void) AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, ADSP_EFAILED);
      return ADSP_EFAILED;
      }

   ASM_OnPopFromRespQ(pMe, pSession);

   result = pPayload->unResponseResult;

      if (ADSP_FAILED(result))
      {
      MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "AudioStreamMgr [%u,%u]: response received from PP for ELITEMSG_CUSTOM_PP_GET_NUM_OUTPUT_BUFFERS. result = %d",
            pSession->ucInternalSessionId, stream_id, result);
   }
   else
   {
      //get the algorithm delay + buffering delay from PP.
      uint32_t num_pp_buffers = pPayload->unNumBuffers;
      uint8_t unDir = ASM_GetCustomDataFromClientToken(pPayload->unClientToken);

      for (uint32_t i = 0; i < num_pp_buffers; i++)
      {
         AudioStreamMgr_AggregateBufDelay(pStream, unDir, pStream->nodes[ASM_NODE_PP].buf_delay);
      }

      pStream->pathAlgDelayUs += pStream->nodes[ASM_NODE_PP].alg_delay;

      MSG_7(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr [%u,%u]: Session path delay query for PP added buffering delay %lu, total FTRT delay = %lu us, total RT delay = %lu us, algorithmic delay %lu, total = %lu us",
            pSession->ucInternalSessionId, stream_id, pStream->nodes[ASM_NODE_PP].buf_delay, pStream->pathFTRTBufDelayUs, pStream->pathRTBufDelayUs, pStream->nodes[ASM_NODE_PP].alg_delay, pStream->pathAlgDelayUs);
      }

   if (0 == pSession->unReqResps)
   {
      ADSPResult errorCode = ADSP_SUCCEEDED(pSession->nOverallResult) ? ADSP_EOK : ADSP_EFAILED;
      result = AudioStreamMgr_FinalizeGetPathDelayV2(pMe, pSession, errorCode);
   }

   //release the internal message
   elite_msg_release_msg(&pSession->msg);

   return result;
}


/**
 * handled rendered EoS event
 */
ADSPResult AudioStreamMgr_SessionRespQEosV2Handler(AudioStreamMgr_t *pMe,
      AudioStreamMgr_SessionStateType *pSession)
{

   elite_msg_data_eos_apr_t *pPayload = (elite_msg_data_eos_apr_t *) pSession->msg.pPayload;

   if(AUDIO_STREAM_MGR_SESSION_CLOSE != pSession->unState)
   {
      uint8_t stream_id = ASM_GetStreamIdFromClientToken(pPayload->unClientToken);
      ASM_GraphNodeIndex node_ind = ASM_GetNodeIndexFromClientToken(pPayload->unClientToken);

      AudioStreamMgr_StreamStateType *pStream = ASM_GetStreamInstance(pSession, stream_id);
      if(NULL == pStream)
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr [%u, %u] received EoS Ack V2 request on invalid stream!",
               pSession->ucInternalSessionId,stream_id);
      }
      else
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AudioStreamMgr [%u,%u]: Received a EoS V2 Ack event from node index %d",
               pSession->ucInternalSessionId, pStream->ucStreamId, node_ind);

         if (pStream->wait_for_eos_v2_ack)
         {
            pStream->wait_for_eos_v2_ack = FALSE;

            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u, %u] raising rendered EoS event", pSession->ucInternalSessionId,stream_id);
            AudioStreamMgr_GenerateRenderedEosEvent( &(pPayload->eosInfo) );
         }
         else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO,"AudioStreamMgr [%u, %u] not waiting for EoS V2. not raising rendered EoS event ", pSession->ucInternalSessionId,stream_id);
         }
      }
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "AudioStreamMgr : Invalid state to receive a Eos V2 Ack event. Ignoring it!");
   }

   return elite_msg_release_msg(&pSession->msg);

}
