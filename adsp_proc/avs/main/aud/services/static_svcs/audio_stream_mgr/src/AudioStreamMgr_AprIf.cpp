/**
@file AudStreamMgr_AprIf.cpp

@brief This file contains the implementation for session-control-related functions
for AudioStreamMgr.

*/

/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/src/AudioStreamMgr_AprIf.cpp#1 $

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
#include "qurt_elite.h"
#include "Elite.h"

#include "AudioStreamMgr_AprIf.h"
#include "AudioStreamMgr_AprDataBase.h"
#include "adsp_private_api_ext.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static elite_apr_handle_t AudioStreamMgr_AprHandle = 0;


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

ADSPResult AudioStreamMgr_VerifyHandle(elite_apr_port_t nPort);

ADSPResult AudioStreamMgr_SetAprHandle(elite_apr_handle_t handle )
{
   if ( 0 != AudioStreamMgr_AprHandle ) {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AudioStreamMgr: AprHandle Already Set 0x%lx, 0x%lx", (uint32_t) AudioStreamMgr_AprHandle, (uint32_t) handle );
      return ADSP_EFAILED;
   }
   AudioStreamMgr_AprHandle = handle;
   return ADSP_EOK;
}

elite_apr_handle_t AudioStreamMgr_GetAprHandle( )
{
   return AudioStreamMgr_AprHandle;
}

ADSPResult AudioStreamMgr_FreeAprPkt(  elite_apr_packet_t* pPkt)
{
   return elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
}

int32_t AudioStreamMgr_AprCallBackFct(elite_apr_packet_t* packet, void* dispatch_data)
{
   uint32_t ulOpCode         = elite_apr_if_get_opcode( packet);
   elite_apr_port_t  port   = elite_apr_if_get_dst_port( packet);
   elite_msg_any_t msg;
   msg.unOpCode = ELITE_APR_PACKET;
   msg.pPayload = (void*)packet;

   int32_t result = APR_EOK;
   qurt_elite_queue_t *pDestQ = NULL;

   MSG_3( MSG_SSID_QDSP6, DBG_LOW_PRIO,"AudioStreamMgr: Rec cmd 0x%lx at [Addr=0x%lx, Port=0x%lx]" ,
          ulOpCode, (uint32_t) ( elite_apr_if_get_dst_addr( packet)) , (uint32_t) (port) );



   // Lock data base
   AudioStreamMgr_AprRouterLock();

   // put different msgs into different queue based on opcode
   switch ( ulOpCode ) {

      case ASM_DATA_CMD_WRITE_V2:
      case ASM_DATA_CMD_MARK_BUFFER_V2:
      case ASM_DATA_CMD_EOS:
      case ASM_DATA_CMD_EOS_V2:
      case ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2:
      case ASM_DATA_CMD_REMOVE_INITIAL_SILENCE:
      case ASM_DATA_CMD_REMOVE_TRAILING_SILENCE:
      case ASM_DATA_CMD_IEC_60958_FRAME_RATE:
      case ASM_DATA_CMD_IEC_60958_MEDIA_FMT:
      case ASM_DATA_CMD_RESYNC:

         // check for valid handle
         // EOS and media format update is sent to the RX path  since in TX Path,
         //   it is Q6 that generates EOS.
        if ( (ADSP_FAILED(result = AudioStreamMgr_VerifyHandle(port)))
              ||(ADSP_FAILED( result = AudioStreamMgr_AprRouterGetDataQ(port,
                                                          AUDIO_STREAM_MGR_APR_ROUTER_RX_DIR,
                                                          &pDestQ))))
        {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                 "AudioStreamMgr: fail to get rx data queue 0x%lx", (uint32_t) port);
             AudioStreamMgr_AprRouterUnLock();
         }
         break;
      case ASM_DATA_CMD_READ_V2:
      case ASM_DATA_CMD_READ_COMPRESSED_V2:
          // check for valid handle
         if ( (ADSP_FAILED(result = AudioStreamMgr_VerifyHandle(port)))
             ||(ADSP_FAILED( result = AudioStreamMgr_AprRouterGetDataQ(port,
                                                          AUDIO_STREAM_MGR_APR_ROUTER_TX_DIR,
                                                          &pDestQ))))
         {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                 "AudioStreamMgr: fail to get tx data queue 0x%lx", (uint32_t) port);
             AudioStreamMgr_AprRouterUnLock();
         }

         break;
        case ASM_CMD_SHARED_MEM_MAP_REGIONS:
        case ASM_CMD_SHARED_MEM_UNMAP_REGIONS:
        case ASM_CMD_ADD_TOPOLOGIES:
        case ASM_CMD_REGISTER_FOR_ADSPPM_VOTES:
        case ASM_CMD_DEREGISTER_FROM_ADSPPM_VOTES:
        case ASM_CMD_GET_ADSPPM_VOTES:
        {
            uint8_t sessionId  = ASM_GET_GROUP_ID( port );
            uint8_t streamId   = ASM_GET_STREAM_ID( port );
            if ( (0 != sessionId && 0 != streamId)
                 || (ADSP_FAILED(result = AudioStreamMgr_GetSysCmdQ(&pDestQ)))  ){

                MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"AudioStreamMgr: invalid handle [%d,%d]",sessionId,streamId);
                AudioStreamMgr_AprRouterUnLock();
                result = ADSP_EHANDLE;
            }
            break;

        }
      default:
          // check for valid handle
         if ( (ADSP_FAILED(result = AudioStreamMgr_VerifyHandle(port)))
             ||(ADSP_FAILED( result = AudioStreamMgr_AprRouterGetCmdQ(port,
                                                          &pDestQ))))
         {
             MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                 "AudioStreamMgr: fail to get cmd queue 0x%lx", (uint32_t) port);
             AudioStreamMgr_AprRouterUnLock();
         }
         break;
   }  // get the corresponding queue based on opcode

   if ( ADSP_FAILED( result) ) {
      // send error message to client: this also frees the packet.
      uint32_t rc = elite_apr_if_end_cmd( AudioStreamMgr_AprHandle, packet, result );
      if (ADSP_FAILED(rc))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                        "AudioStreamMgr: failed at elite_apr_if_end_cmd 0x%lx. Ack not sent. port=0x%lx", rc, (uint32_t) port);
      }

      //returning fail causes another free in APR. therefore return success.
      return APR_EOK;
   }

   // Push msg to the queue.
   /*
      Note: The Dest Q can be NULL when the message does not need to be queued and has
      been processed in some other way - using the PP callback for example. Nothing needs
      to be done in that case.
   */
   if ( NULL != pDestQ )
   {
      if ( ADSP_FAILED( result = qurt_elite_queue_push_back( pDestQ, (uint64_t*) (&msg))))
      {

         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "AudioStreamMgr: fail to push to queue 0x%p, port=0x%lx", pDestQ, (uint32_t) port);

         // send error message to client: this also frees the packet.
         uint32_t rc = elite_apr_if_end_cmd( AudioStreamMgr_AprHandle, packet, result);
         if (ADSP_FAILED(rc))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                           "AudioStreamMgr: failed at elite_apr_if_end_cmd 0x%lx. Ack not sent. port=0x%lx", rc, (uint32_t) port);
         }

         //returning fail causes another free in APR. therefore return success.
         result = APR_EOK;
      }
   }

   AudioStreamMgr_AprRouterUnLock();
   return result;
   //return 0;
}

ADSPResult AudioStreamMgr_GenerateClientEventFromCb( void  *pInCb,
                                               uint32_t eventOpcode,
                                               uint32_t token,
                                                void *pEventPayload,
                                                uint32_t ulPayloadSize)
{

   AudioStreamMgr_CallBackHandleType *pCb =
       (AudioStreamMgr_CallBackHandleType*)pInCb;

    ADSPResult result;
    switch (eventOpcode) {
        case ASM_DATA_EVENT_EOS:
        case ASM_SESSION_EVENT_RX_UNDERFLOW:
        case ASM_SESSION_EVENT_TX_OVERFLOW:
        case ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY:
        case ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY:
        case ASM_DATA_EVENT_DEC_ERR_NOTIFY:
        case ASM_DATA_EVENT_MARK_BUFFER_V2:
        case ASM_DATA_EVENT_WATERMARK:
        case ASM_DATA_EVENT_EARLY_EOS:
        case ASM_STREAM_ENCDEC_EVENT:
        case ASM_STREAM_PP_EVENT:
        case ASM_STREAM_IEC_61937_MEDIA_FMT_EVENT:
          {
          result = elite_apr_if_alloc_send_event( pCb->aprHandle,
                               pCb->selfAddr,
                               pCb->selfPort,
                               pCb->clientAddr,
                               pCb->clientPort,
                               token,
                               eventOpcode,
                               pEventPayload,
                               ulPayloadSize);
             break;
          }
       default:
          {
          return ADSP_EUNSUPPORTED;
             break;
          }
    }

   return result;
}

/** ACK function for call back */
ADSPResult AudioStreamMgr_GenerateAck(  elite_apr_packet_t* pPkt,/// This is the received packet that requires ACK.
                                       ADSPResult status,
                                       void *pAckPayload,      /// payload that is required in ACK, specified by caller
                                       uint32_t ulSize, /// payload size.
                                       uint32_t    ulAckOpcode     /// Optonal The opcode for ACK.
                                    )
{
   ADSPResult result ;
   uint32_t cmdOpcode = elite_apr_if_get_opcode(pPkt);
   switch( cmdOpcode ) {
      case ASM_DATA_CMD_EOS:

		 //In case of secondary input stream ASM will send the ASM_DATA_EVENT_RENDERED_EOS to client.
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ASM sending ASM_DATA_EVENT_RENDERED_EOS to the client.");

         result = elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                  pPkt->dst_addr,   /// Switch src/dst address field in ack
                                  pPkt->dst_port,
                                  pPkt->src_addr,
                                  pPkt->src_port,
                                  pPkt->token,
                                  ASM_DATA_EVENT_RENDERED_EOS,
                                  NULL,
                                  0);

         elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
         break;

      case ASM_DATA_CMD_EOS_V2:

       //In case of secondary input stream ASM will send the ASM_DATA_EVENT_RENDERED_EOS_V2 to client.
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"ASM sending ASM_DATA_EVENT_RENDERED_EOS_V2 to the client.");

         result = elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                  pPkt->dst_addr,   /// Switch src/dst address field in ack
                                  pPkt->dst_port,
                                  pPkt->src_addr,
                                  pPkt->src_port,
                                  pPkt->token,
                                  ASM_DATA_EVENT_RENDERED_EOS_V2,
                                  NULL,
                                  0);

         elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
         break;

      // For a lot of commands, the ack is basically the inverse
      // the pPkt with slight change
      case ASM_DATA_CMD_WRITE_V2:
      {

           MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                     "AudioStreamMgr:Port=0x%x: ISR status 0x%lx, ASM_DATA_EVENT_WRITE_DONE_V2 0x%8x , [0x%lx,%lu] ",
                     pPkt->dst_port, (uint32_t) status, ASM_DATA_EVENT_WRITE_DONE_V2, (uint32_t) pAckPayload, ulSize );

           asm_data_event_write_done_v2_t writeDonePayload;

           asm_data_cmd_write_v2_t *writePayload;
           writePayload = (asm_data_cmd_write_v2_t *)elite_apr_if_get_payload_ptr(pPkt);

           writeDonePayload.buf_addr_lsw     = writePayload->buf_addr_lsw;
           writeDonePayload.buf_addr_msw     = writePayload->buf_addr_msw;
           writeDonePayload.mem_map_handle   = writePayload->mem_map_handle;
           writeDonePayload.status = status;

            result = elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                     pPkt->dst_addr,   /// Switch src/dst address field in ack
                                     pPkt->dst_port,
                                     pPkt->src_addr,
                                     pPkt->src_port,
                                     pPkt->token,
                                     ASM_DATA_EVENT_WRITE_DONE_V2, ///ASM_DATA_EVENT_EOS,
                                     &writeDonePayload,// NULL,
                                     sizeof(writeDonePayload) //0
                                            );
            elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
            break;
      }
      case ASM_DATA_CMD_READ_V2:
         if ( pAckPayload == NULL || ulSize <= 0  ) {
           MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                 "AudioStreamMgr:Port=0x%x: ISR Error: ASM_DATA_CMD_READ_V2 ACK requires a payload, [0x%lx,%lu] ",
                 pPkt->dst_port,(uint32_t) pAckPayload, ulSize );
           result = ADSP_EFAILED;
         }  else {

            MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                  "AudioStreamMgr:Port=0x%x: ISR status 0x%lx, ASM_DATA_EVENT_READ_DONE_V2 [0x%lx,%lu] ",
                  pPkt->dst_port,(uint32_t) status, (uint32_t) pAckPayload, ulSize );
            result = elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                     pPkt->dst_addr,   /// Switch src/dst address field in ack
                                     pPkt->dst_port,
                                     pPkt->src_addr,
                                     pPkt->src_port,
                                     pPkt->token,
                                     ASM_DATA_EVENT_READ_DONE_V2, ///ASM_DATA_EVENT_EOS,
                                     pAckPayload,
                                     ulSize );

            elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
         }
         break;

      case ASM_DATA_CMD_READ_COMPRESSED_V2:
               if ( pAckPayload == NULL || ulSize <= 0  ) {
                 MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                       "AudioStreamMgr:Port=0x%x: ISR Error: ASM_DATA_CMD_READ_COMPRESSED_V2 ACK requires a payload, [0x%lx,%lu] ",
                       pPkt->dst_port,(uint32_t) pAckPayload, ulSize );
                 result = ADSP_EFAILED;
               }  else {

                  MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                        "AudioStreamMgr:Port=0x%x: ISR status 0x%lx, ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 [0x%lx,%lu] ",
                        pPkt->dst_port,(uint32_t) status, (uint32_t) pAckPayload, ulSize );
                  result = elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                           pPkt->dst_addr,   /// Switch src/dst address field in ack
                                           pPkt->dst_port,
                                           pPkt->src_addr,
                                           pPkt->src_port,
                                           pPkt->token,
                                           ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2, ///ASM_DATA_EVENT_EOS,
                                           pAckPayload,
                                           ulSize );

                  elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
               }
               break;

      case ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2:
      case ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3:
      case ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST:
      case ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2:
      case ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2:
         {
             // For in-band get_pp_param, in-band pp_topology_module_info and
             // get matrix/stream router param, in which ASM already allocate
             // APR packet with all appropriate fields.
            result = elite_apr_if_async_send( AudioStreamMgr_AprHandle, pPkt );
            break;
         }
      case ASM_STREAM_CMD_GET_PP_PARAMS_V2:
      case ASM_STREAM_CMD_GET_PP_PARAMS_V3:
      case ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST:
      case ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST_V2:
      case ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2:
         {
            uint32_t respOpcode = (cmdOpcode == ASM_STREAM_CMD_GET_PP_PARAMS_V2)?
                              ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2: (cmdOpcode == ASM_STREAM_CMD_GET_PP_PARAMS_V3)?
                              ASM_STREAM_CMDRSP_GET_PP_PARAMS_V3: (cmdOpcode == ASM_SESSION_CMD_GET_MTMX_STRTR_PARAMS_V2)?
                              ASM_SESSION_CMDRSP_GET_MTMX_STRTR_PARAMS_V2: (cmdOpcode == ASM_STREAM_CMD_GET_PP_TOPO_MODULE_LIST)?
                              ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST: ASM_STREAM_CMDRSP_GET_PP_TOPO_MODULE_LIST_V2;

            // For out-of-band get_pp_param
            result = elite_apr_if_alloc_send_ack( AudioStreamMgr_AprHandle,
                                     pPkt->dst_addr,   /// Switch src/dst address field in ack
                                     pPkt->dst_port,
                                     pPkt->src_addr,
                                     pPkt->src_port,
                                     pPkt->token,
                                     respOpcode, ///opcode,
                                     pAckPayload,
                                     ulSize );
            elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);
            break;
         }
      default:
         if ( pAckPayload  && ulAckOpcode ) {

            MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                  "AudioStreamMgr:Port=0x%x: ISR status 0x%lx, Ack 0x%lx , [0x%lx,%lu] ",
                  pPkt->dst_port,(uint32_t) status, ulAckOpcode, (uint32_t) pAckPayload, ulSize );
            result = elite_apr_if_alloc_send_ack( AudioStreamMgr_AprHandle,
                                     pPkt->dst_addr,   /// Switch src/dst address field in ack
                                     pPkt->dst_port,
                                     pPkt->src_addr,
                                     pPkt->src_port,
                                     pPkt->token,
                                     ulAckOpcode,
                                     pAckPayload,
                                     ulSize );

            elite_apr_if_free( AudioStreamMgr_AprHandle, pPkt);

         } else {
           MSG_5(MSG_SSID_QDSP6, DBG_LOW_PRIO,
                  "AudioStreamMgr:Port=0x%x: ISR status 0x%lx, Basic Ack: 0x%lx , [0x%lx,%lu] ",
                  pPkt->dst_port,(uint32_t) status, elite_apr_if_get_opcode(pPkt), (uint32_t) pAckPayload, ulSize );
           result = elite_apr_if_end_cmd( AudioStreamMgr_AprHandle, pPkt, status );
         }

         break;

   }

   if ( ADSP_FAILED( result ) ) {
      MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AudioStreamMgr:Port=0x%x: ISR fail to send ack 0x%lx", pPkt->dst_port, (uint32_t) result);
   }

   return result;
}

ADSPResult AudioStreamMgr_VerifyHandle(elite_apr_port_t nPort)
{
    /// Check whether the handle is in valid range
    uint8_t sessionId  = ASM_GET_GROUP_ID( nPort );
    uint8_t streamId   = ASM_GET_STREAM_ID( nPort );
    if ( 0 == sessionId ||
         ASM_MAX_SUPPORTED_SESSION_ID < sessionId ||
         0 == streamId ||
         ASM_MAX_SUPPORTED_STREAM_ID < streamId )  {
       return ADSP_EHANDLE;
    }
    return ADSP_EOK;
}


ADSPResult AudioStreamMgr_GenerateRenderedEosEvent(elite_msg_data_eos_info_t *pClientInfo)
{
   return elite_apr_if_alloc_send_event( AudioStreamMgr_AprHandle,
                                   pClientInfo->self_addr,
                                   pClientInfo->self_port,
                                   pClientInfo->client_addr,
                                   pClientInfo->client_port,
                                   pClientInfo->client_token,
                                   pClientInfo->event_opcode,
                                   NULL,
                                   0);

}

