/*========================================================================

*//** @file qurt_elite_bufmgr
This file contains a utility library to manage a set of pre-allocated buffers
among multiple clients.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_bufmgr.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/13/10   mwc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static const uint32_t MSB_32 = 0x80000000L;
static const uint32_t CORRUPTION_DETECT_MAGIC = 0x836ADF71;

/* -----------------------------------------------------------------------
** Static variables
** ----------------------------------------------------------------------- */
static qurt_elite_atomic_word_t nInstanceCount = QURT_ELITE_ATOMIC_INIT(0);

/* -----------------------------------------------------------------------
** Data Structures & types
** ----------------------------------------------------------------------- */
/// This struct is the bin for each power-of-2 buffer size.
struct qurt_elite_bufbin_t
{
   int nBufs;
   qurt_elite_queue_t *pQ;
};

/// This is the state instance of the buffer manager.
struct qurt_elite_bufmgr_t
{
   char*          pStartAddr;
   uint32_t       size;
   qurt_elite_mutex_t     mutex;
   qurt_elite_channel_t   channel;
   uint32_t         unAnyBufsMask;
   qurt_elite_bufbin_t aBufferBin[32];
};

/* Dummy static variable whose address will be used to
   indicate that buffer node is allocated from heap. */
static uint32_t heap_alloc_indicator;

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

ADSPResult qurt_elite_bufmgr_create(const uint32_t* nBufsInBin, char* pStartAddr, uint32_t size, qurt_elite_bufmgr_t **ppBufMgr)
{

   ADSPResult result;
   int i;

   if (NULL == pStartAddr || NULL == ppBufMgr)
   {
      return ADSP_EBADPARAM;
   }

   // Allocate memory for this instance
   if (!(*ppBufMgr = (qurt_elite_bufmgr_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_bufmgr_t), QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "Out of memory trying to qurt_elite_bufmgr_create!");
      return ADSP_ENOMEMORY;
   }

   // zero-initialize instance.
   QURT_ELITE_ZEROAT(*ppBufMgr);

   // save start address of buffer
   qurt_elite_bufmgr_t *pBufMgr = *ppBufMgr;
   pBufMgr->pStartAddr = pStartAddr;
   pBufMgr->size = size;

   // Inititialize the channel & mutex
   qurt_elite_channel_init(&pBufMgr->channel);
   qurt_elite_mutex_init(&pBufMgr->mutex);

   // build up the array of buffer bins
   uint32_t* pBuffer = (uint32_t*)pStartAddr;
   for (i = 3; i < 32; i++)
   {
      // check if no buffers are requested for each bin, and skip.
      if (0 == nBufsInBin[i]) continue;

      // Add this bin to the mask
      pBufMgr->unAnyBufsMask |= (MSB_32 >> i);

      // save number of buffers
      qurt_elite_bufbin_t *pBin = &pBufMgr->aBufferBin[i];
      pBin->nBufs = nBufsInBin[i];

      // create the queue name
      char name[QURT_ELITE_DEFAULT_NAME_LEN];
      int count = qurt_elite_atomic_increment(&nInstanceCount) & 0x000000FFL;
      snprintf(name, QURT_ELITE_DEFAULT_NAME_LEN, "BFRMGR%xBIN%d", count, i);

      // round up queue nodes to nearest power of 2
      int nQueueNodes = 1 << (32 - Q6_R_cl0_R(nBufsInBin[i] - 1));

      // create Q and add it to channel.
      if (ADSP_FAILED(result = qurt_elite_queue_create(name,  nQueueNodes, &pBin->pQ))
         || ADSP_FAILED(result = qurt_elite_channel_addq (&pBufMgr->channel, pBin->pQ, (MSB_32 >> i))))
      {
         MSG(MSG_SSID_QDSP6, DBG_FATAL_PRIO, "qurt_elite_bufmgr_create failed to create queues for buffer nodes!");
         qurt_elite_bufmgr_destroy(pBufMgr);
         *ppBufMgr = NULL;
         return result;
      }

      // fill the queue with pointers
      qurt_elite_bufmgr_node_t bufNode;
      bufNode.pReturnQ = pBin->pQ;
      for (int j = 0; j < pBin->nBufs; j++)
      {
         /*
            fill 4 metadata words for finding buffer's home queue and for debug info.
            Word 0 - the return queue handle;
                     &heap_alloc_indicator implies that the buf is allocated from heap
            Word 1 - (for corruption detection) the return queue handle XOR a magic number.
            Word 2 - thread ID of the allocating function. 0 implies unallocated buffer.
            Word 3 - for 8-byte alignment.
         */
         *pBuffer++ = (uint32_t)(pBin->pQ);
         *pBuffer++ = (uint32_t)(pBin->pQ) ^ CORRUPTION_DETECT_MAGIC;
         *pBuffer++ = 0;
         *pBuffer++ = 0;
         bufNode.pBuffer = (char*)pBuffer;

         qurt_elite_queue_push_back(pBin->pQ, (uint64_t*) &bufNode);
         pBuffer += (1 << i) / sizeof(uint32_t);
      }
   }
   return ADSP_EOK;
}

bool_t qurt_elite_is_bufmgr_node(qurt_elite_bufmgr_t *bufmgr_ptr,
                                 qurt_elite_queue_t* return_q_ptr, 
                                 void* buf_ptr)
{
   uint32_t start_addr = (uint32_t)bufmgr_ptr->pStartAddr;
   uint32_t end_addr = start_addr + bufmgr_ptr->size;
   uint32_t addr = (uint32_t)buf_ptr;

   //a buffer is a bufmgr node if the buffer address falls within the
   //bufmgr range or if the return queue address == &heap_alloc_indicator
   if ( ((addr >= start_addr) && (addr < end_addr))
        || ((uint32_t *)return_q_ptr == &heap_alloc_indicator) )
   {
      return true;
   }

   return false;
}

ADSPResult qurt_elite_bufmgr_return_buf(void* pBuf)
{
   // check for corruption of metadata
   uint32_t *pMetadata = (uint32_t*)(pBuf) - (QURT_ELITE_BUFMGR_METADATA_SIZE / sizeof(uint32_t));
   if (pMetadata[0] != (pMetadata[1] ^ CORRUPTION_DETECT_MAGIC))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
         "qurt_elite Bufmgr: trying to free buffer at %p, but this pointer is corrupted! Buffer is leaked!",
         pBuf);

      QURT_ELITE_ASSERT(0);
      return ADSP_EFAILED;
   }

   //check if the buffer has been allocated from heap
   if((uint32_t)&heap_alloc_indicator == pMetadata[0])
   {
#ifdef DEBUG_QURT_ELITE_BUFMGR
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BufMgr HEAP ReturnBuffer: Thrd=0x%x, Buff=0x%x",
            qurt_thread_get_id(), pBuf);
#endif
	   qurt_elite_memory_free(pMetadata);
	   return ADSP_EOK;
   }

   //check thread ID. if it is already zero, it means double free is happening on this buffer
   if (0 == pMetadata[2])
   {
	  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
	        "qurt_elite Bufmgr: Double free detected. Buffer at %p is already freed!",
	         pBuf);
	  QURT_ELITE_ASSERT(0);
      return ADSP_EFAILED;
   }
   else
   {
	  //set thread ID to zero
	  pMetadata[2] = 0;

      // form bufmgr node and push it back to its home queue.
      qurt_elite_bufmgr_node_t bufNode;
      bufNode.pReturnQ = (qurt_elite_queue_t*)(pMetadata[0]);
      bufNode.pBuffer = pBuf;
#ifdef DEBUG_QURT_ELITE_BUFMGR
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BufMgr ReturnBuffer: Thrd=0x%x, Buff=0x%x",
            qurt_thread_get_id(), pBuf);
#endif
      ADSPResult result = qurt_elite_queue_push_back(bufNode.pReturnQ, (uint64_t*) &bufNode);
      if (ADSP_FAILED(result))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "qurt_elite Bufmgr: trying to free buffer at %p, but the queue returned error! Buffer is leaked!",
         pBuf);
         return ADSP_EFAILED;
      }
      return ADSP_EOK;
   }
}


// If this function hangs, it means there is a buffer leak.
void qurt_elite_bufmgr_destroy(qurt_elite_bufmgr_t *pBufMgr)
{
   ADSPResult result __attribute__((unused));
   qurt_elite_bufmgr_node_t bufNode;

   if (!pBufMgr) return;

   // lock out all clients and drain all the buffers
   qurt_elite_mutex_lock (&pBufMgr->mutex);

   for (int i = 0; i < 32; i++)
   {
      if (pBufMgr->unAnyBufsMask & (MSB_32 >> i))
      {
         for (int j = 0; j < pBufMgr->aBufferBin[i].nBufs; j++)
         {
            (void) qurt_elite_channel_wait(&pBufMgr->channel, MSB_32 >> i);
            result = qurt_elite_queue_pop_front(pBufMgr->aBufferBin[i].pQ, (uint64_t*)&bufNode );
            QURT_ELITE_ASSERT(ADSP_SUCCEEDED(result));
         }
         qurt_elite_queue_destroy(pBufMgr->aBufferBin[i].pQ);
      }
   }

   // destroy channel
   qurt_elite_channel_destroy(&pBufMgr->channel);

   // destroy mutex
   qurt_elite_mutex_unlock(&pBufMgr->mutex);
   qurt_elite_mutex_destroy(&pBufMgr->mutex);

   // free this.
   qurt_elite_memory_free (pBufMgr);
}

ADSPResult qurt_elite_bufmgr_poll_for_buffer(qurt_elite_bufmgr_t *pBufMgr, int nDesiredSize,
   qurt_elite_bufmgr_node_t *pNode, int* pnActualSize)
{
   uint32_t unChannelMask;
   uint32_t unChannelStatus;
   ADSPResult result;

   // mask off all the bufs that are too small
   unChannelMask = pBufMgr->unAnyBufsMask & ((1 << Q6_R_cl0_R(nDesiredSize - 1)) - 1);

   // enter critical section
   qurt_elite_mutex_lock (&pBufMgr->mutex);

   // Take node off back of stack. Use back instead of front in attempt to
   // keep using the same buffers. Better for cache performance.
   if (!(unChannelStatus = qurt_elite_channel_poll(&pBufMgr->channel, unChannelMask))
      || ADSP_FAILED(result = qurt_elite_queue_pop_back(pBufMgr->aBufferBin[Q6_R_cl0_R(unChannelStatus)].pQ,
      (uint64_t*)pNode)))
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Buffer Manager failed to find a free buffer. Trying to allocate from heap");
      qurt_elite_mutex_unlock (&pBufMgr->mutex);

      uint32_t *buf = (uint32_t *)qurt_elite_memory_malloc(nDesiredSize + QURT_ELITE_BUFMGR_METADATA_SIZE, QURT_ELITE_HEAP_DEFAULT);
      if(NULL == buf)
      {
    	  MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Buffer Manager failed to allocate even from heap!");
    	  return ADSP_ENEEDMORE;
      }

      //ReturnQ is set to addr of heap_alloc_indicator to indicate that this buffer came from heap
      buf[0] = (uint32_t)&heap_alloc_indicator;
      buf[1] = buf[0] ^ CORRUPTION_DETECT_MAGIC;
      //thread ID is not useful for heap allocations but retaining for consistency sake;
      buf[2] = (uint32_t)qurt_thread_get_id();
      buf[3] = 0;

      pNode->pReturnQ = (qurt_elite_queue_t *) (buf[0]);
      pNode->pBuffer = &buf[4];

#ifdef DEBUG_QURT_ELITE_BUFMGR
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BufMgr HEAP GetBuffer: Thrd=0x%x, Buff=0x%x",
            qurt_thread_get_id(), pNode->pBuffer);
#endif

      *pnActualSize = nDesiredSize;
      return ADSP_EOK;
   }

   *pnActualSize = 1 << Q6_R_cl0_R(unChannelStatus);

   //set the thread ID of the calling function
   uint32_t *tid_ptr = (uint32_t*)(pNode->pBuffer) - 2;
   *tid_ptr = (uint32_t)qurt_thread_get_id();

#ifdef DEBUG_QURT_ELITE_BUFMGR
   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "BufMgr GetBuffer: Thrd=0x%x, Buff=0x%x",
         qurt_thread_get_id(), pNode->pBuffer);
#endif

   // leave critical section
   qurt_elite_mutex_unlock (&pBufMgr->mutex);
   return ADSP_EOK;
}

