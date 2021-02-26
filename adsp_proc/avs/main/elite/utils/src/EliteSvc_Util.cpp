/*========================================================================

*//** @file EliteSvc_Util.cpp
This file contains utilities to be used by typical services.

Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/EliteSvc_Util.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/26/09   mwc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "Elite.h"
#include "qurt_elite.h"
#include "adsp_asm_data_commands.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

ADSPResult elite_svc_process_cmd_queue(void* pInstance, elite_svc_handle_t *pHandle,
   elite_svc_msg_handler_func pHandler[], uint32_t handler_table_size)
{
   elite_msg_any_t msg;
   ADSPResult result;

   // Process all commands in the queue
   for (;;)
   {
      // Take next msg off the q
      result = qurt_elite_queue_pop_front(pHandle->cmdQ, (uint64_t*) &msg);

      // Process the msg
      if (ADSP_EOK == result)
      {
         if (msg.unOpCode >= handler_table_size)
         {
            if (ADSP_FAILED(result = elite_svc_unsupported(pInstance, &msg)))
            {
               // in case of errors, return them
               return result;
            }
         }

         // table lookup to call handling function, with FALSE to indicate processing of msg
         if (ADSP_FAILED(result = pHandler[msg.unOpCode](pInstance, &msg)))
         {
            // in case of errors, return them
            return result;
         }
         // successfully processed msg, check for additional msgs
      }
      else if (ADSP_ENEEDMORE == result)
      {
         // if there are no more msgs in the q, return successfully
         return ADSP_EOK;
      }
      else
      {
         // all other results of QueuePopFront, return error code
         // actually QueuePopFront does not return other codes, keep it safe here
         return result;
      }

   } //for (;;)

}

ADSPResult elite_svc_unsupported (void* pInstance, elite_msg_any_t* pMsg)
{
   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Unsupported message ID 0x%lx!!\n", pMsg->unOpCode);
   return ADSP_EUNSUPPORTED;
}

ADSPResult elite_svc_return_success (void* pInstance, elite_msg_any_t* pMsg)
{
   return ADSP_EOK;
}

void elite_svc_destroy_data_queue (qurt_elite_queue_t* pDataQ)
{
   elite_msg_any_t   dataQMsg;
   ADSPResult result;

   // clean up data queue
   if (pDataQ)
   {
      // Drain any queued buffers
      do
      {
         // Non-blocking MQ receive
         result = qurt_elite_queue_pop_front(pDataQ, (uint64_t*)&dataQMsg);

         // return the buffer to its rightful q.
         if (ADSP_EOK == result)
         {
            elite_msg_return_payload_buffer(&dataQMsg);
         }

      } while (ADSP_EOK == result);

      // destroy the q.
      qurt_elite_queue_destroy(pDataQ);
   }

   return;
}

void elite_svc_destroy_cmd_queue (qurt_elite_queue_t* cmdQ)
{
   ADSPResult result;
   elite_msg_any_t cmdMsg;

   // clean up command queue
   if (cmdQ)
   {
      // Drain any queued commands
      do
      {
         if (ADSP_SUCCEEDED(result = qurt_elite_queue_pop_front(cmdQ, (uint64_t*)&cmdMsg)))
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Got a message after DESTROY command!!\n");
            /** Should release message buffer if response is not required
             * elite_svc_send_ack(&cmdMsg, ADSP_ENOTREADY);
             */
            elite_msg_finish_msg( &cmdMsg, ADSP_ENOTREADY);
         }

      } while (ADSP_EOK == result);

      // destroy the q.
      qurt_elite_queue_destroy(cmdQ);
   }

   return;
}

void elite_svc_free_buffers_in_buf_queue (qurt_elite_queue_t* bufQ, int nBufs)
{
   // clean up buffer queue
   if (bufQ)
   {
      // all nodes on bufQ will have the same structure {BufAddr, returnQ}, regardless
      // whether the actual buffer is malloced from heap or acquired from buffer manager
      // therefore, we can always use the buffer manager node to pop nodes on bufQ
      qurt_elite_bufmgr_node_t bufNode;
      ADSPResult result;

      // Wait for all buffers to be returned. This has to be a blocking operation.
      int nReturnedBufs = 0;

      // Drain the buffers
      while (nReturnedBufs < nBufs)
      {
         // wait for buffers
         (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(bufQ), qurt_elite_queue_get_channel_bit(bufQ));

         result = qurt_elite_queue_pop_front(bufQ, (uint64_t*)&bufNode);

         // retrieve the buffer
         if (ADSP_EOK == result)
         {
            // Free the buffer
            qurt_elite_memory_free (bufNode.pBuffer);
            bufNode.pBuffer = NULL;
            nReturnedBufs++;
         }

         // shouldn't reach this point.
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Error %d in destroying buffers!!\n", result);
         }
      }

   }
   return;
}




void elite_svc_destroy_buf_queue (qurt_elite_queue_t* bufQ, int nBufs)
{
   // clean up buffer queue
   if (bufQ)
   {
      // all nodes on bufQ will have the same structure {BufAddr, returnQ}, regardless
      // whether the actual buffer is malloced from heap or acquired from buffer manager
      // therefore, we can always use the buffer manager node to pop nodes on bufQ
      qurt_elite_bufmgr_node_t bufNode;
      ADSPResult result;

      // Wait for all buffers to be returned. This has to be a blocking operation.
      int nReturnedBufs = 0;

      // Drain the buffers
      while (nReturnedBufs < nBufs)
      {
         // wait for buffers
         (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(bufQ), qurt_elite_queue_get_channel_bit(bufQ));

         result = qurt_elite_queue_pop_front(bufQ, (uint64_t*)&bufNode);

         // retrieve the buffer
         if (ADSP_EOK == result)
         {
            // Free the buffer
            qurt_elite_memory_free (bufNode.pBuffer);
            bufNode.pBuffer = NULL;
            nReturnedBufs++;
         }

         // shouldn't reach this point.
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Error %d in destroying buffers!!\n", result);
         }
      }

      qurt_elite_queue_destroy(bufQ);
   }
   return;
}

void elite_svc_send_ack (elite_msg_any_t *pMsg, ADSPResult result)
{
   (void)elite_msg_ack_msg(pMsg, (uint32_t)result);
}

ADSPResult elite_svc_wait_for_ack(elite_msg_any_t *pMsg)
{
   ADSPResult result;
   elite_msg_any_t response;
   elite_msg_any_payload_t* pMsgPayload = (elite_msg_any_payload_t*) pMsg->pPayload;
   qurt_elite_queue_t* pRespQ = pMsgPayload->pResponseQ;

   if (NULL==pRespQ)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Message waiting for ACK on NULL Queue 0x%lx!!!\n", pMsg->unOpCode);
      return ADSP_EBADPARAM;
   }

   // only listen for response.
   (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(pRespQ), qurt_elite_queue_get_channel_bit(pRespQ));

   if (ADSP_FAILED(result = qurt_elite_queue_pop_front(pRespQ, (uint64_t*)&response)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Message 0x%lx failed to get ACK!!!\n", pMsg->unOpCode);
      return result;
   }

   if (pMsg->unOpCode != response.unOpCode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Message 0x%lx got mismatched ACK 0x%lx!!!\n", pMsg->unOpCode, response.unOpCode);
      return ADSP_EBADPARAM;
   }

   return result;
}

ADSPResult elite_svc_wait_for_ack_and_get_msg(elite_msg_any_t *pMsg, elite_msg_any_t *pResponse)
{
   ADSPResult result;
   elite_msg_any_payload_t* pMsgPayload = (elite_msg_any_payload_t*) pMsg->pPayload;
   qurt_elite_queue_t* pRespQ = pMsgPayload->pResponseQ;

   if (!pRespQ)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Message waiting for ACK on NULL Queue 0x%lx!!!\n", pMsg->unOpCode);
      return ADSP_EBADPARAM;
   }

   // only listen for response.
   (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(pRespQ), qurt_elite_queue_get_channel_bit(pRespQ));

   if (ADSP_FAILED(result = qurt_elite_queue_pop_front(pRespQ, (uint64_t*)pResponse)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Message 0x%lx failed to get ACK!!!\n", pMsg->unOpCode);
   }

   if (pMsg->unOpCode != pResponse->unOpCode)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_FATAL_PRIO,
         "Message 0x%lx got mismatched ACK 0x%lx!!!\n", pMsg->unOpCode, pResponse->unOpCode);
      return ADSP_EBADPARAM;
   }

   return result;
}

void elite_svc_deinit_cmd_queue(qurt_elite_queue_t* cmdQ)
{
   ADSPResult result;
   elite_msg_any_t cmdMsg;

   // clean up command queue
   if (cmdQ)
   {
      // Drain any queued commands
      do
      {
         if (ADSP_SUCCEEDED(result = qurt_elite_queue_pop_front(cmdQ, (uint64_t*)&cmdMsg)))
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Got a message after DESTROY command!!\n");
            /** Should release message buffer if response is not required
             * elite_svc_send_ack(&cmdMsg, ADSP_ENOTREADY);
             */
            elite_msg_finish_msg( &cmdMsg, ADSP_ENOTREADY);
         }

      } while (ADSP_EOK == result);

      // destroy the q.
      qurt_elite_queue_deinit(cmdQ);
   }

   return;
}


void elite_svc_deinit_data_queue (qurt_elite_queue_t* pDataQ)
{
   elite_msg_any_t   dataQMsg;
   ADSPResult result;

   // clean up data queue
   if (pDataQ)
   {
      // Drain any queued buffers
      do
      {
         // Non-blocking MQ receive
         result = qurt_elite_queue_pop_front(pDataQ, (uint64_t*)&dataQMsg);

         // return the buffer to its rightful q.
         if (ADSP_EOK == result)
         {
            elite_msg_return_payload_buffer(&dataQMsg);
         }

      } while (ADSP_EOK == result);

      // destroy the q.
      qurt_elite_queue_deinit(pDataQ);
   }

   return;
}


void elite_svc_deinit_buf_queue (qurt_elite_queue_t* bufQ, int nBufs)
{
   // clean up buffer queue
   if (bufQ)
   {
      // all nodes on bufQ will have the same structure {BufAddr, returnQ}, regardless
      // whether the actual buffer is malloced from heap or acquired from buffer manager
      // therefore, we can always use the buffer manager node to pop nodes on bufQ
      qurt_elite_bufmgr_node_t bufNode;
      ADSPResult result;

      // Wait for all buffers to be returned. This has to be a blocking operation.
      int nReturnedBufs = 0;

      // Drain the buffers
      while (nReturnedBufs < nBufs)
      {
         // wait for buffers
         (void) qurt_elite_channel_wait(qurt_elite_queue_get_channel(bufQ), qurt_elite_queue_get_channel_bit(bufQ));

         result = qurt_elite_queue_pop_front(bufQ, (uint64_t*)&bufNode);

         // retrieve the buffer
         if (ADSP_EOK == result)
         {
            // Free the buffer
            qurt_elite_memory_free (bufNode.pBuffer);
            bufNode.pBuffer = NULL;
            nReturnedBufs++;
         }

         // shouldn't reach this point.
         else
         {
            MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Error %d in destroying buffers!!\n", result);
         }
      }

      qurt_elite_queue_deinit(bufQ);
   }
   return;
}

/**
 * Used to send Rendered EoS V2 event to ASM
 */
ADSPResult elite_svc_raise_rendered_eos_v2_event (elite_msg_data_eos_info_t *pEoSInfo)
{
   ADSPResult result = ADSP_EOK;

   if (pEoSInfo->event_opcode == ASM_DATA_EVENT_RENDERED_EOS_V2)
   {
      if (NULL == pEoSInfo->eos_client_queue)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_svc_raise_rendered_eos_v2_event error: queue is NULL");
         return ADSP_EFAILED;
      }

      uint32_t clientToken = pEoSInfo->eos_client_token;

      uint32_t unPayLoadBufSize = sizeof(elite_msg_data_eos_apr_t);

      elite_msg_any_t msg;

      ADSPResult res = elite_msg_create_msg( &msg,
            &unPayLoadBufSize,
            ELITE_DATA_EOS,
            NULL,
            clientToken,
            0);

      if (ADSP_FAILED( res ))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_svc_raise_rendered_eos_v2_event Failed to create msg! Error: %d\n", res);
         return res;
      }

      elite_msg_data_eos_apr_t *payload = (elite_msg_data_eos_apr_t*) msg.pPayload;
      payload->unEosFormat = 0;
      payload->eosInfo = *pEoSInfo;

      if (ADSP_FAILED(result = qurt_elite_queue_push_back(pEoSInfo->eos_client_queue, (uint64_t*)&msg)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_svc_raise_rendered_eos_v2_event. failed to push EoS V2 to ASM");
      }

      MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "elite_svc_raise_rendered_eos_v2_event. pushed to ASM");
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "elite_svc_raise_rendered_eos_v2_event. opcode 0x%lx not supported.", pEoSInfo->event_opcode);
   }

   return result;
}
