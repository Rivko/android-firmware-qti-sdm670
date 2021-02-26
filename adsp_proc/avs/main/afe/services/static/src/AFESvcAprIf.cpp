/*========================================================================
   This file contains AFE APR handler implementation

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFESvcAprIf.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "Elite.h"
#include "AFESvcAprIf.h"
#include "EliteAprIf.h"
#include "adsp_afe_service_commands.h"
#include "AFEInternal.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static elite_apr_handle_t afe_svc_apr_handle = 0;

/* =======================================================================
**                          Function Definitions
** ======================================================================= */
ADSPResult afe_svc_set_apr_handle(elite_apr_handle_t handle )
{
   if ( 0 != afe_svc_apr_handle )
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfeSvc: AprHandle Already Set 0x%lx, 0x%lx",afe_svc_apr_handle, handle );
      return ADSP_EFAILED;
   }
   afe_svc_apr_handle = handle;
   return ADSP_EOK;
}

elite_apr_handle_t afe_svc_get_apr_handle(void )
{
  return afe_svc_apr_handle;
}

ADSPResult afe_svc_free_apr_pkt(  elite_apr_packet_t* pkt_ptr)
{
   return elite_apr_if_free( afe_svc_apr_handle, pkt_ptr);
}

int32_t afe_svc_apr_call_back_fct(elite_apr_packet_t* packet, void* dispatch_data)
{
    elite_msg_any_t msg;
    msg.unOpCode = ELITE_APR_PACKET;
    msg.pPayload = (void*)packet;
    uint32_t ulOpCode = elite_apr_if_get_opcode( packet);
    int32_t result = APR_EOK;
    qurt_elite_queue_t *pDestQ;

    MSG_2( MSG_SSID_QDSP6, DBG_HIGH_PRIO,"At <0x%x> APRCMD  <0x%8lx>  AfeSvc: Rec cmd." , (int)elite_apr_if_get_dst_addr(packet),ulOpCode);

    switch (ulOpCode)
    {
       case AFE_SERVICE_CMD_REGISTER_RT_PORT_DRIVER:
       case AFE_SERVICE_CMD_UNREGISTER_RT_PORT_DRIVER:
       case AFE_PORT_DATA_CMD_RT_PROXY_PORT_READ_V2:
       case AFE_PORT_DATA_CMD_RT_PROXY_PORT_WRITE_V2:
       {
          pDestQ = afe_svc_global_ptr->rt_port_mgr_q;
       }
       break;
       default:
       {
          pDestQ = afe_svc_global_ptr->static_svc_cmdq_ptr;
       }
    }

   if (pDestQ == NULL)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to get Afe queue");
        elite_apr_if_end_cmd(afe_svc_apr_handle, packet, ADSP_EFAILED);

        // elite_apr_if_end_cmd() frees the message packet that is allocated by APR.
        // Hence we will send APR_EOK to avoid double free after returning from this
        // call back function (Caller function will free the packet if it is error).
        return APR_EOK;
    }

    // Push msg to the queue.
    if ( ADSP_FAILED( result = qurt_elite_queue_push_back( pDestQ, (uint64_t*) (&msg))))
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfeSvc: Fail to push to queue 0x%lx", (uint32_t) pDestQ);
       // send error message
       elite_apr_if_end_cmd( afe_svc_apr_handle, packet, result);

        // elite_apr_if_end_cmd() frees the message packet that is allocated by APR.
        // Hence we will send APR_EOK to avoid double free after returning from this
        // call back function (Caller function will free the packet if it is error)
    }

    return APR_EOK;
}


/** ACK function for call back */
ADSPResult afe_svc_generate_ack(  elite_apr_packet_t* pkt_ptr,/// This is the received packet that requires ACK.
                                       ADSPResult status,
                                       void *pAckPayload,      /// payload that is required in ACK, specified by caller
                                       uint32_t ulSize, /// payload size.
                                       uint32_t ulAckOpcode     /// Optonal The opcode for ACK.
                                    )
{
    ADSPResult result = ADSP_EOK;

    switch(elite_apr_if_get_opcode(pkt_ptr))
    {
        case AFE_PORT_CMD_GET_PARAM_V2:
        {
            // For out of band get param
            MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                             "AfeSvc: ISR status 0x%x, AFE_PORT_CMDRSP_GET_PARAM_V2 0x%8x , [0x%8lx,%lu] ",
                             status, AFE_PORT_CMDRSP_GET_PARAM_V2, (uint32_t) pAckPayload, ulSize );
            result = elite_apr_if_alloc_send_ack ( afe_svc_apr_handle,
                                       pkt_ptr->dst_addr,   /// Switch src/dst address field in ack
                                       pkt_ptr->dst_port,
                                       pkt_ptr->src_addr,
                                       pkt_ptr->src_port,
                                       pkt_ptr->token,
                                       AFE_PORT_CMDRSP_GET_PARAM_V2, //opcode for ACK
                                       pAckPayload, //pointer payload
                                       ulSize); //length of payload
            elite_apr_if_free( afe_svc_apr_handle, pkt_ptr );
        }
        break;
        
        case AFE_PORT_CMD_GET_PARAM_V3:
        {
            // For out of band get param
            MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                             "AfeSvc: ISR status 0x%x, AFE_PORT_CMDRSP_GET_PARAM_V3 0x%8x , [0x%8lx,%lu] ",
                             status, AFE_PORT_CMDRSP_GET_PARAM_V3, (uint32_t) pAckPayload, ulSize );
            result = elite_apr_if_alloc_send_ack ( afe_svc_apr_handle,
                                       pkt_ptr->dst_addr,   /// Switch src/dst address field in ack
                                       pkt_ptr->dst_port,
                                       pkt_ptr->src_addr,
                                       pkt_ptr->src_port,
                                       pkt_ptr->token,
                                       AFE_PORT_CMDRSP_GET_PARAM_V3, //opcode for ACK
                                       pAckPayload, //pointer payload
                                       ulSize); //length of payload
            elite_apr_if_free( afe_svc_apr_handle, pkt_ptr );
        }
        break;
        default:
        {
            result = elite_apr_if_end_cmd( afe_svc_apr_handle, pkt_ptr, status );
        }
   }

   if (ADSP_FAILED(result))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AfeSvc: afe_svc_generate_ack fail to send ack 0x%x", result);
   }
   return result;
}

ADSPResult afe_svc_generate_eos_ack( elite_msg_data_eos_info_t *pClientInfo )
{
   return elite_apr_if_alloc_send_event( afe_svc_apr_handle,
                                   pClientInfo->self_addr,
                                   pClientInfo->self_port,
                                   pClientInfo->client_addr,
                                   pClientInfo->client_port,
                                   pClientInfo->client_token,
                                   pClientInfo->event_opcode,
                                   NULL,
                                   0);
}
