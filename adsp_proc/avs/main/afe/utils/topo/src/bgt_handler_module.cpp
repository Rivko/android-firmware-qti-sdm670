/*========================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/utils/topo/src/bgt_handler_module.cpp#1 $

Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/22/2014   rv       Created

==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------*/

/*==========================================================================
  Include files
  ========================================================================== */
#include "bgt_handler_module.h"
#include "bgt_handler.h"

/*==========================================================================
  Function Declarations
========================================================================== */

static ADSPResult bgt_pop_buf_from_queue(qurt_elite_queue_t *q_ptr, void **payload_ptr_ptr)
{
   ADSPResult result = ADSP_EOK;
   qurt_elite_channel_t *bufQ_chan;

   uint32_t bufQ_chan_bit;
   uint32_t unChannelStatus;
   qurt_elite_bufmgr_node_t outBufMgrNode;

   bufQ_chan = qurt_elite_queue_get_channel(q_ptr);
   bufQ_chan_bit = qurt_elite_queue_get_channel_bit(q_ptr);

   if(0 != (unChannelStatus = qurt_elite_channel_poll(bufQ_chan, bufQ_chan_bit)))
   {
      /* Pop dataQ until it's empty */
      if(ADSP_FAILED(result = qurt_elite_queue_pop_front(q_ptr, (uint64_t * )&outBufMgrNode)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to pop buffer from Queue: 0x%lx", (uint32_t)q_ptr);
         return result;
      }

      *payload_ptr_ptr = (elite_msg_data_buffer_t *)outBufMgrNode.pBuffer;
   }
   else
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed nothing received in the channel!");
      result = ADSP_EFAILED;
   }

   /* Failure code is the indicator if a buffer was popped or not */
   return result;
}


static void bgt_drain_data_queue(qurt_elite_queue_t *q_ptr)
{
   elite_msg_any_t dataQMsg;
   ADSPResult result;

   // clean up data queue
   if(q_ptr)
   {
      // Drain any queued buffers
      do
      {
         // Non-blocking MQ receive
         result = qurt_elite_queue_pop_front(q_ptr, (uint64_t*)&dataQMsg);

         // return the buffer to its rightful q.
         if(ADSP_EOK == result)
         {
            elite_msg_return_payload_buffer(&dataQMsg);
         }

      } while(ADSP_EOK == result);
   }

   return;
}



ADSPResult bgt_create_buffers(module_bgt_info_t *bgt_info_ptr, uint32_t num_buffers, uint32_t buf_size)
{
   ADSPResult result = ADSP_EOK;

   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   elite_msg_data_buffer_t *msg_data_buf_ptr;
   int                     req_buf_Size;

   req_buf_Size = GET_ELITEMSG_DATABUF_REQ_SIZE(buf_size);

   /* Allocate and queue up the output buffers */
   for (uint32_t i = 0; i < num_buffers; i++)
   {
      /* Allocate the databuffer payload */
      msg_data_buf_ptr = (elite_msg_data_buffer_t *) qurt_elite_memory_malloc(req_buf_Size, QURT_ELITE_HEAP_DEFAULT);

      if (NULL == msg_data_buf_ptr)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Out of Memory allocating output buffer for buf q: 0x%lx",
               (uint32_t)bgt_info_ptr->buf_q_ptr);
         return ADSP_ENOMEMORY;
      }

      /* Clear the allocated structure */
      memset(msg_data_buf_ptr, 0, req_buf_Size);

      msg_data_buf_ptr->nActualSize     = buf_size;
      msg_data_buf_ptr->nMaxSize        = buf_size;
      msg_data_buf_ptr->pBufferReturnQ  = bgt_info_ptr->buf_q_ptr;

      if(ADSP_FAILED(
            result = elite_msg_push_payload_to_returnq(bgt_info_ptr->buf_q_ptr,
                  (elite_msg_any_payload_t * ) msg_data_buf_ptr)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Failed to push payload to output buffer queue: 0x%lx",
               (uint32_t)bgt_info_ptr->buf_q_ptr);

         qurt_elite_memory_free(msg_data_buf_ptr);
         return result;
      }

      /* update the number of buffers created to the client queue info, so that when we deinit the Q,
       * Proper number of buffs are deallocated
       */
      bgt_info_ptr->num_buffs_created++;
   }

   return ADSP_EOK;
}

void bgt_destroy_buffers(module_bgt_info_t *bgt_info_ptr)
{
   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return;
   }

   /* Return any buffer in data Q */
   bgt_drain_data_queue(bgt_info_ptr->bgt_svc_handle.dataQ);

   /* Free buffers in Buf Q */
   elite_svc_free_buffers_in_buf_queue(bgt_info_ptr->buf_q_ptr, bgt_info_ptr->num_buffs_created);
   bgt_info_ptr->num_buffs_created = 0;

   return;
}


ADSPResult bgt_pop_buf_from_buf_q(module_bgt_info_t *bgt_info_ptr, void **data_ptr_ptr)
{
   ADSPResult result;
   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   if(ADSP_FAILED(
         result = bgt_pop_buf_from_queue(bgt_info_ptr->buf_q_ptr, data_ptr_ptr)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"BGT failed %d, no buffer available in buf Q!", result);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}

ADSPResult bgt_pop_buf_from_data_q(module_bgt_info_t *bgt_info_ptr, void **data_ptr_ptr)
{
   ADSPResult result;
   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   if(ADSP_FAILED(
         result = bgt_pop_buf_from_queue(bgt_info_ptr->bgt_svc_handle.dataQ, data_ptr_ptr)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"BGT failed %d, no buffer available in data Q!", result);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}


/**
* This function sends bgt data to bgt handlers
*
* @param[in] ptr to the bgt_info struct created by bgt_create
* @param[in] ptr to the payload of the data msg, generally data to be sent.
* @param[in] size of the data payload to be sent.
* @return ADSPResult
*/
ADSPResult bgt_send_buf_to_data_q(module_bgt_info_t *bgt_info_ptr, void *data_ptr)
{
   ADSPResult result;

   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   if(NULL == data_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt msg payload ptr");
      return ADSP_EFAILED;
   }
   elite_msg_data_buffer_t *msg_payload_ptr = (elite_msg_data_buffer_t *)data_ptr;
   qurt_elite_bufmgr_node_t outBufMgrNode;
   elite_msg_any_t* pPeerDataQMsg;

   /* send the buffer */
   outBufMgrNode.pReturnQ = msg_payload_ptr->pBufferReturnQ;
   outBufMgrNode.pBuffer = (void *)msg_payload_ptr;
   pPeerDataQMsg = elite_msg_convt_buf_node_to_msg(&outBufMgrNode,
         ELITE_DATA_BUFFER,
         NULL, /* do not need response */
         0, /* token */
         0); /* do not care response result*/

   if(ADSP_FAILED(
         result = qurt_elite_queue_push_back(bgt_info_ptr->bgt_svc_handle.dataQ,
               (uint64_t* )pPeerDataQMsg)))
   {
      //return the msg buffer...
      qurt_elite_queue_push_back(msg_payload_ptr->pBufferReturnQ, (uint64_t*)pPeerDataQMsg);
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "FAILED to send input msg!!");
   }

   return result;
}


ADSPResult bgt_return_buf_to_buf_q(module_bgt_info_t *bgt_info_ptr, void *data_ptr)
{
   ADSPResult result;
   if(NULL == bgt_info_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt info ptr");
      return ADSP_EFAILED;
   }

   if(NULL == data_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "NULL bgt data ptr");
      return ADSP_EFAILED;
   }

   if(ADSP_FAILED(
         result = elite_msg_push_payload_to_returnq(bgt_info_ptr->buf_q_ptr, (elite_msg_any_payload_t *) data_ptr)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"BGT failed to return buffer to buf Q: %d", result);
      return ADSP_EFAILED;
   }

   return ADSP_EOK;
}
