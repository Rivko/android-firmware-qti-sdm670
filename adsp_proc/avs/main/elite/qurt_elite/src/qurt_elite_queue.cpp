/*========================================================================

*//** @file qurt_elite_queue.cpp
This file contains an efficient queue of 8-uint8_t nodes.

Copyright (c) 2010, 2015 QUALCOMM Technologies, Incorporated.  
All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_queue.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/17/15   mh      Removing channel group support
02/04/10   mwc     Created file.

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

/* =======================================================================
**                          Function Definitions
** ======================================================================= */

/****************************************************************************
** Queues
*****************************************************************************/

/*
 * Queue limitations:
 * - nMaxItems must be power of 2
 */
// This function is going to be deprecated in near future. qurt_elite_queue_init
// needs to be used instead of this function.
ADSPResult qurt_elite_queue_create(char *pzQ2Name, int nMaxItems, qurt_elite_queue_t** pQ)
{

   // verify size is more than 0, and is a power of 2.
   if (0 >= nMaxItems
      || 0 != (nMaxItems & (nMaxItems - 1)))
   {
      return ADSP_EBADPARAM;
   }

   // allocate space
   if (NULL == (*pQ = (qurt_elite_queue_t*) qurt_elite_memory_malloc(sizeof(qurt_elite_queue_t), QURT_ELITE_HEAP_DEFAULT)))
   {
      return ADSP_ENOMEMORY;
   }

   QURT_ELITE_ZEROAT(*pQ);

   // It is taken as a known fact that Q6 malloc returns pointers that are 8-uint8_t aligned, so no special
   // alignment is needed...
   // It has been agreed that each node in Queue will be uint64_t, 8-bytes
   if (NULL == ((*pQ)->data = (uint64_t*) qurt_elite_memory_malloc (sizeof(uint64_t) * nMaxItems, QURT_ELITE_HEAP_DEFAULT)))
   {
      if ((*pQ)->data) qurt_elite_memory_free((*pQ)->data);
      qurt_elite_memory_free(*pQ);
      // Set PQ to be NULL after free
      *pQ = NULL;
      return ADSP_ENOMEMORY;
   }

   // set up the modulo mask for wrapping around the indexes
   (*pQ)->nModuloOperator = nMaxItems - 1;

   // initialize mutex
   qurt_pimutex_init((qurt_mutex_t*)&((*pQ)->mqLock));

   // save name
   qurt_elite_strl_cpy((*pQ)->name, pzQ2Name, QURT_ELITE_DEFAULT_NAME_LEN);

   // save size info
   (*pQ)->nNumNodes = nMaxItems;

   // initialize indexes
   (*pQ)->nFrontIndex = 0;
   (*pQ)->nBackIndex = 0;

   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Q CREATE: Q=0x%lx NumItems=%d\n", (uint32_t)*pQ, nMaxItems);


   // return.
   return ADSP_EOK;
}

// This function is going to be deprecated in near future. qurt_elite_queue_deinit
// needs to be used instead of this function.

void qurt_elite_queue_destroy(qurt_elite_queue_t* pQ)
{
   // check for NULL
   if (!pQ) return;

   qurt_elite_channel_t* pChannel = pQ->pChannel;

   //release the channel bit if Q is added to a channel
   if (pChannel)
   {
      pChannel->unBitsUsedMask ^= pQ->myChannelBit ;
   }

   qurt_pimutex_destroy((qurt_mutex_t*)&pQ->mqLock);
   qurt_elite_memory_free(pQ->data);
   qurt_elite_memory_free(pQ);
}



ADSPResult qurt_elite_queue_push_back(qurt_elite_queue_t* pQ, uint64_t* pPayload)
{

   if ( (NULL == pQ) || (NULL == pQ->pChannel) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Q SEND: channel not initialized on Q");
      return ADSP_EBADPARAM;
   }
   // grab the mutex
   qurt_pimutex_lock((qurt_mutex_t*) &pQ->mqLock);

   if (TRUE == pQ->fDisableQ)
   {
	   qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Q DISABLED: Cannot enqueue the data");
    return ADSP_EFAILED;
   }
   

#ifdef DEBUG_QURT_ELITE_QUEUE
   uint32_t unOpcode = (uint32_t)(*pPayload >> 32);
   // filter out ELITE_DATA_BUFFER to avoid the flood.
   if (qurt_elite_globalstate.bEnableQLogging && 0x0000000AL != unOpcode)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Q PushBk: Thrd=0x%x, Q=0x%x, Msg=0x%x 0x%x",
         qurt_thread_get_id(), pQ, unOpcode, (uint32_t)(*pPayload));
   }
#endif //DEBUG_QURT_ELITE_QUEUE

   // make sure pipe is not full. We could implement a block waiting on freed node, but
   // rather treat this as an error case and keep it simple.
   // Check for full queue. Full if Front==Back and Signal is set
   if (pQ->nFrontIndex == pQ->nBackIndex
      && (qurt_signal2_get(&pQ->pChannel->anysig) & pQ->myChannelBit))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "OVERFLOWED QUEUE: Q=0x%lx", (uint32_t)pQ);
      qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);
      return ADSP_ENEEDMORE;
   }

   pQ->data[pQ->nBackIndex] = *pPayload;

   // bookkeeping
   pQ->nBackIndex = (pQ->nBackIndex + 1) & pQ->nModuloOperator;

   // send the signal
   (void) qurt_signal2_set(&pQ->pChannel->anysig, pQ->myChannelBit);

   // release the mutex
	qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);

   return ADSP_EOK;
}

ADSPResult qurt_elite_queue_pop_front(qurt_elite_queue_t* pQ,  uint64_t* pPayload)
{

   if (NULL==pQ->pChannel)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Q SEND: channel not initialized on Q");
      return ADSP_EBADPARAM;
   }
   // grab the mutex
   qurt_pimutex_lock((qurt_mutex_t*) &pQ->mqLock);

   // make sure not empty (this is non-blocking mq).
   // Empty if Front==Back and the signal is not set (low)
   if ((pQ->nFrontIndex == pQ->nBackIndex)
      && !(qurt_signal2_get(&pQ->pChannel->anysig) & pQ->myChannelBit))
   {
      qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);
      return ADSP_ENEEDMORE;
   }

   *pPayload = pQ->data[pQ->nFrontIndex];



#ifdef DEBUG_QURT_ELITE_QUEUE
   uint32_t unOpcode = (uint32_t)(*pPayload >> 32);
   // filter out ELITE_DATA_BUFFER to avoid the flood.
   if (qurt_elite_globalstate.bEnableQLogging && 0x0000000AL != unOpcode)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Q PopFnt: Thrd=0x%x, Q=0x%x, Msg=0x%x 0x%x",
         qurt_thread_get_id(), pQ, unOpcode, (uint32_t)(*pPayload));
   }
#endif //DEBUG_QURT_ELITE_QUEUE

   // update Front index
   pQ->nFrontIndex = (pQ->nFrontIndex + 1) & pQ->nModuloOperator;

   // if mq is empty, clear signal.
   if (pQ->nFrontIndex == pQ->nBackIndex)
   {
      (void) qurt_signal2_clear(&pQ->pChannel->anysig, pQ->myChannelBit);
   }

   // release the mutex
   qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);

   return ADSP_EOK;
}

ADSPResult qurt_elite_queue_pop_back(qurt_elite_queue_t* pQ,  uint64_t* pPayload )
{
   int nIndexToPop;

   if (NULL==pQ->pChannel)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Q SEND: channel not initialized on Q");
      return ADSP_EBADPARAM;
   }

   // grab the mutex
   qurt_pimutex_lock((qurt_mutex_t*) &pQ->mqLock);

   // make sure not empty (this is non-blocking mq).
   // Empty if Front==Back and the signal is not set (low)
   if ((pQ->nFrontIndex == pQ->nBackIndex)
      && !(qurt_signal2_get(&pQ->pChannel->anysig) & pQ->myChannelBit))
   {
      qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);
      return ADSP_ENEEDMORE;
   }

   nIndexToPop = (pQ->nBackIndex - 1) & pQ->nModuloOperator;

   *pPayload = pQ->data[nIndexToPop];



#ifdef DEBUG_QURT_ELITE_QUEUE
   uint32_t unOpcode = (uint32_t)(*pPayload >> 32);
   // filter out ELITE_DATA_BUFFER to avoid the flood.
   if (qurt_elite_globalstate.bEnableQLogging && 0x0000000AL != unOpcode)
   {
      MSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Q PopBk: Thrd=0x%x, Q=0x%x, Msg=0x%x 0x%x",
         qurt_thread_get_id(), pQ, unOpcode, (uint32_t)(*pPayload));
   }

#endif //DEBUG_QURT_ELITE_QUEUE
   // update back index
   pQ->nBackIndex = nIndexToPop;

   // if mq is empty, clear signal.
   if (pQ->nFrontIndex == pQ->nBackIndex)
   {
      (void) qurt_signal2_clear(&pQ->pChannel->anysig, pQ->myChannelBit);
   }

   // release the mutex
   qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);

   return ADSP_EOK;
}

/*
 * Queue limitations:
 * - nMaxItems must be power of 2
 */
ADSPResult qurt_elite_queue_init(char *pzQ2Name, int nMaxItems, qurt_elite_queue_t* pQ)
{
   // verify size is more than 0, and is a power of 2.
   if (0 >= nMaxItems
      || 0 != (nMaxItems & (nMaxItems - 1))
      || NULL == pQ)
   {
      return ADSP_EBADPARAM;
   }


   QURT_ELITE_ZEROAT(pQ);


   (pQ)->data  = &(pQ->ullDataPlaceholder);


   // set up the modulo mask for wrapping around the indexes
   (pQ)->nModuloOperator = nMaxItems - 1;

   // initialize mutex
   qurt_pimutex_init((qurt_mutex_t*) &((pQ)->mqLock));

   // save name
   qurt_elite_strl_cpy((pQ)->name, pzQ2Name, QURT_ELITE_DEFAULT_NAME_LEN);

   // save size info
   (pQ)->nNumNodes = nMaxItems;

   // initialize indexes
   (pQ)->nFrontIndex = 0;
   (pQ)->nBackIndex = 0;

   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Q CREATE: Q=0x%lx NumItems=%d\n", (uint32_t)pQ, nMaxItems);

   // return.
   return ADSP_EOK;
}

void qurt_elite_queue_deinit(qurt_elite_queue_t* pQ)
{
   // check for NULL
   if (!pQ) return;

   qurt_elite_channel_t* pChannel = pQ->pChannel;

   //release the channel bit if Q is added to a channel
   if (pChannel)
   {
      pChannel->unBitsUsedMask ^= pQ->myChannelBit ;
   }

   qurt_pimutex_destroy((qurt_mutex_t*)&pQ->mqLock);

}

ADSPResult qurt_elite_queue_disable(qurt_elite_queue_t* pQ)
{
    QURT_ELITE_ASSERT(pQ != NULL);
    qurt_pimutex_lock((qurt_mutex_t*) &pQ->mqLock);
    pQ->fDisableQ = TRUE;
    qurt_pimutex_unlock((qurt_mutex_t*) &pQ->mqLock);
	 MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Queue 0x%p disabled", pQ);
    return ADSP_EOK;
}

qurt_elite_channel_t* qurt_elite_queue_get_channel(qurt_elite_queue_t* pQ)
{
   return pQ->pChannel;
}

uint32_t qurt_elite_queue_get_channel_bit(qurt_elite_queue_t* pQ)
{
   return pQ->myChannelBit;
}

