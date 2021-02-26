/**
@file AudDevMgr_AprIf.cpp
@brief This file contains the implementation of AudioDevMgr's interface with APR.
*/

/*========================================================================
Copyright (c) 2010-2014, 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_dev_mgr/src/AudDevMgr_AprIf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/23/12   KR      Non-backward compatible API changes for Badger.
========================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "AudDevMgr_AprIf.h"
#include "AudDevMgr_DataBase.h"
#include "AudDevMgr.h"
#include "EliteAprIf.h"
#include "adsp_adm_api.h"
#include "AudDevMgr_PrivateDefs.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static elite_apr_handle_t AudDevMgr_AprHandle = 0;
static qurt_elite_queue_t *AudDevMgr_CmdQueue = NULL;

/* =======================================================================
Function Definitions
========================================================================== */
ADSPResult AudDevMgr_SetAprHandle(elite_apr_handle_t handle )
{
   if ( 0 != AudDevMgr_AprHandle )
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM: AprHandle Already Set!");
      return ADSP_EFAILED;
   }
   AudDevMgr_AprHandle = handle;
   return ADSP_EOK;
}

//Assuming that this function is called after the queue is successfully created. Hence, not checking for NULL.
void AudDevMgr_SetCmdQueue(qurt_elite_queue_t *admCmdQ )
{
   AudDevMgr_CmdQueue = admCmdQ;
}

ADSPResult AudDevMgr_FreeAprPkt(elite_apr_packet_t* pPkt)
{
   return elite_apr_if_free( AudDevMgr_AprHandle, pPkt);
}

int32_t AudDevMgr_AprCallBackFct(elite_apr_packet_t* packet, void* dispatch_data)
{
   uint32_t ulOpCode         = elite_apr_if_get_opcode( packet);
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( packet);
   elite_msg_any_t msg;
   qurt_elite_queue_t * pDestQ;
   msg.unOpCode = ELITE_APR_PACKET;
   msg.pPayload = (void*)packet;

   int32_t result = APR_EOK;
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM_AprCallBackFct: At 0x%x APRCMD 0x%lX ADM rcvd payload size %lu",
         ((elite_apr_if_get_dst_addr(packet) << 16) | port), ulOpCode, elite_apr_if_get_payload_size(packet));

   //Put different msgs into different queues based on ulOpCode
   switch ( ulOpCode )
   {
      case ADM_CMD_DEVICE_OPEN_V5:
      case ADM_CMD_DEVICE_OPEN_V6:
      case ADM_CMD_DEVICE_OPEN_V7:
      case ADM_CMD_DEVICE_CLOSE_V5:
      case ADM_CMD_MATRIX_MAP_ROUTINGS_V5:
      case ADM_CMD_STREAM_DEVICE_MAP_ROUTINGS_V5:
      case ADM_CMD_STREAM_DEVICE_UNMAP_ROUTINGS_V5:
      case ADM_CMD_SET_PP_PARAMS_V5:
	  case ADM_CMD_SET_PP_PARAMS_V6:
      case ADM_CMD_GET_PP_PARAMS_V5:
      case ADM_CMD_GET_PP_PARAMS_V6:
      case ADM_CMD_GET_PP_TOPO_MODULE_LIST:
      case ADM_CMD_GET_PP_TOPO_MODULE_LIST_V2:
      case ADM_CMD_MATRIX_RAMP_GAINS_V5:
      case ADM_CMD_MATRIX_RAMP_GAINS_V6:
      case ADM_CMD_MATRIX_MUTE_V5:
      case ADM_CMD_MATRIX_MUTE_V6:
      case ADM_CMD_CONNECT_AFE_PORT_V5:
      case ADM_CMD_DISCONNECT_AFE_PORT_V5:
      case ADM_CMD_SHARED_MEM_MAP_REGIONS:
      case ADM_CMD_SHARED_MEM_UNMAP_REGIONS:
      case ADM_CMD_ADD_TOPOLOGIES_V5:
	  case ADM_CMD_SET_PSPD_MTMX_STRTR_PARAMS_V5:
	  case ADM_CMD_SET_PSPD_MTMX_STRTR_PARAMS_V6:
      case ADM_CMD_REGISTER_IEC_61937_FMT_UPDATE:
	  case ADM_CMD_REGISTER_PP_EVENTS:
	  case ADM_CMD_SET_MTMX_STRTR_DEV_PARAMS_V1:
	  case ADM_CMD_GET_MTMX_STRTR_DEV_PARAMS_V1:
      {
         pDestQ = AudDevMgr_CmdQueue;
         break;
      }
      default:
      {
           MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM_AprCallBackFct: ISR does not support cmd!");
		   result = APR_EUNSUPPORTED;
           break;
      }
   }

   if (ADSP_FAILED(result))
   {
       (void)elite_apr_if_end_cmd(AudDevMgr_AprHandle, packet, result);
       /* Whenever the dispatch function returns an error, APR frees the packet, but does not send the error response back to client.
	   *   In order to free the packet and send error response back to client, we have to use elite_apr_if_end_cmd.
       *   Hence to prevent same packet freed again by APR in the error case, we always return APR_EOK. */
	   return APR_EOK;
   }

   //Push msg to the queue, if required
   if ( NULL != pDestQ )
   {
      if (ADSP_FAILED(result = qurt_elite_queue_push_back(pDestQ, (uint64_t*)(&msg))))
      {
          MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM_AprCallBackFct: ISR failed to push cmd to dest queue!");
          (void)elite_apr_if_end_cmd( AudDevMgr_AprHandle, packet, result);
          /* Whenever the dispatch function returns an error, APR frees the packet, but does not send the error response back to client.
	       *   In order to free the packet and send error response back to client, we have to use elite_apr_if_end_cmd.
           *   Hence to prevent same packet freed again by APR in the error case, we always return APR_EOK. */
          result = APR_EOK;
      }
   }
   return result;
}

/* AudDevMgr_GenerateAck : ACK function for call back.
    elite_apr_packet_t* pPkt  : This is the received packet that requires ACK.
    void *pAckPayload           : Payload that is required in ACK, specified by caller.
    uint32_t ulSize                : Payload size.
    uint32_t ulAckOpcode      : Optional - The opcode for ACK.
*/
ADSPResult AudDevMgr_GenerateAck(elite_apr_packet_t* pPkt, ADSPResult status, void *pAckPayload,
                                                         uint32_t ulSize, uint32_t    ulAckOpcode)
{
    ADSPResult result =ADSP_EFAILED;
    switch (elite_apr_if_get_opcode(pPkt))
    {
       default :
       {
           if (pAckPayload && ulAckOpcode)
           {
              MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM APR ISR: status %d, ACK opcode= 0x%x",
                    (int)status, ulAckOpcode);

              //APR alloc memory and send the cmdrsp to client processor.
              result = elite_apr_if_alloc_send_ack(AudDevMgr_AprHandle,
                                               pPkt->dst_addr,  // Switch src/dst address field in ack
                                               pPkt->dst_port,
                                               pPkt->src_addr,
                                               pPkt->src_port,
                                               pPkt->token,
                                               ulAckOpcode,    // cmdrsp opcode
                                               pAckPayload,
                                               ulSize);

              //Free the original incoming APR packet.
              elite_apr_if_free(AudDevMgr_AprHandle, pPkt);
           }
           else
           {
               MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ADM APR ISR: Basic ACK, status %d, ACK opcode= 0x%x",
                     (int)status,elite_apr_if_get_opcode(pPkt));

               result = elite_apr_if_end_cmd( AudDevMgr_AprHandle, pPkt, status );
           }
           break;
        }
    }

    if ((ADSP_FAILED (result)))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "ADM: ISR failed to send ack 0x%x", result);
    }
    return result;
}/* AudDevMgr_GenerateClientCb : Generate ADM event function for client callback.
    AdmEventCbData_t* callback_ptr  : This is the stored ADM callback information..
    uint32_t opcode           : The opcode for the event.
    void *payload_ptr         : pointer to the payload.
    uint32_t payload_size     : size of the payload referenced by payload_ptr.
 */
ADSPResult AudDevMgr_GenerateClientCb(AdmEventCbData_t *callback_ptr, uint32_t opcode, void * payload_ptr, uint32_t payload_size)
{
   return elite_apr_if_alloc_send_event(AudDevMgr_AprHandle,
         callback_ptr->dest_addr,  // Switch src/dst address field in event
         callback_ptr->dest_port,
         callback_ptr->src_addr,
         callback_ptr->src_port,
         callback_ptr->token,
         opcode,    // event opcode
         payload_ptr,
         payload_size);
}

