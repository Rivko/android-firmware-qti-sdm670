/*========================================================================

*//** @file qurt_elite_thread.cpp
This file contains utilities for threads.

Copyright (c) 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_thread.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   mwc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/
#ifdef QURT_ELITE_ENABLE_STACK_FILLING
#undef QURT_ELITE_ENABLE_STACK_FILLING
#endif


#if defined(SIM)
#define QURT_ELITE_ENABLE_STACK_FILLING
#endif
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */

// this stub is needed to seed the new thread with info not available through qurt_hread_create().
//typedef void (*qurt_elite_thread_handler_func_t) (void *);

static void qurt_elite_thread_stub(void *pThreadNode);

/*
 Local function for creaating thread. 
 pTid           : Thread ID.
 pzThreadName   : Thread name.
 pStack         : Location of a preallocated stack (NULL causes a new allocation).
				  pStack must point to the lowest address in the stack.
 nStackSize     : Size of the thread stack.
 nPriority      : Thread priority, where 0 is the lowest and 255 is the highest.
 pfStartRoutine : Entry function of the thread.
 arg            : This parameter is passed to the entry
                  function and can be a case to any pointer type.
 heap_id        : Thread stack will be allocated in the heap
                  specified by this parameter.
 return
 An indication of success or failure.
*/
ADSPResult qurt_elite_thread_launch(qurt_elite_thread_t *pTid, char *pzThreadName, char* pStack,
   size_t nStackSize, int nPriority, int (*pfStartRoutine)(void *), void* arg, QURT_ELITE_HEAP_ID heap_id)
{
   int count;
   bool_t fStatus = FALSE;
   bool_t fPreAllocatedStack = FALSE;
   qurt_thread_attr_t attr;
   int ret;

   qurt_elite_thread_list_t *pThreadNode=NULL;

   // boundary checks
   if (0 > nPriority || 255 < nPriority || 256 > nStackSize) return ADSP_EBADPARAM;

   // if no stack has been provided, allocate it.
   if (!pStack)
   {
      // round up stack size
      nStackSize = (nStackSize + 7) & (-8);
      if(NULL == (pStack = (char*) qurt_elite_memory_malloc(nStackSize, heap_id)))
      {
         return ADSP_ENOMEMORY;
      }
      fPreAllocatedStack = FALSE;
   }
   else
   {
      fPreAllocatedStack = TRUE;
   }

   #ifdef QURT_ELITE_ENABLE_STACK_FILLING
   // fill stack with a pattern.
   for (uint32_t* pStackFiller = (uint32_t*) (pStack);
      pStackFiller < (uint32_t*) (pStack + nStackSize);
      pStackFiller += QURT_ELITE_STACK_FILL_SPACING)
   {
      *pStackFiller = QURT_ELITE_STACK_FILL_WORD;
   }
   #endif
   
   // scan all the structures to find which one is free
   qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
   for (count=0;count<QURT_ELITE_MAX_SW_THREADS;count++)
   {
      if(QURT_ELITE_THREAD_NONE==(qurt_elite_globalstate.aThreadContextBuf[count].nThreadState))
      {
         qurt_elite_globalstate.aThreadContextBuf[count].nThreadState = QURT_ELITE_THREAD_LAUNCHING;
         pThreadNode = &(qurt_elite_globalstate.aThreadContextBuf[count]);
         fStatus = TRUE;
         break;
      }
   }
   qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   if(FALSE == fStatus)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Allocating more than supported threads !");
      if (FALSE == fPreAllocatedStack)
      {
         qurt_elite_memory_free(pStack);
      }
      return ADSP_EFAILED;
   }

   pThreadNode->pStack = pStack;
   pThreadNode->nStackSize = nStackSize;
   pThreadNode->fPreAllocatedStack = fPreAllocatedStack;

#ifdef DBG_BUFFER_ADDRESSES
   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "THRD: Name = %s, Stack Start Addr=0x%x, Size=%d",pzThreadName, pThreadNode->pStack, nStackSize);
#endif /* DBG_BUFFER_ADDRESSES */

   // reverse the priority so that we are in line with POSIX- higher number = higher priority
   nPriority = 255 - nPriority;


   // Set up init struct;
   pThreadNode->nExitStatus = 0xdeadbabe;
   qurt_elite_strl_cpy(&(pThreadNode->name[0]), pzThreadName, QURT_ELITE_THREAD_NAME_MAX_LENGTH);
   pThreadNode->pfStartRoutine = pfStartRoutine;
   pThreadNode->arg = arg;

   // spin the thread, entering into the stub which will set up remaining parameters.
   // TODO: what if thread creation fails, what does QURT do?
   // Analysis: QURT calls qurt_malloc then asm func trap0 to launch the thread,
   // there are no failure code returned

   qurt_thread_attr_init (&attr);
   qurt_thread_attr_set_stack_size (&attr, pThreadNode->nStackSize);
   qurt_thread_attr_set_stack_addr (&attr, pThreadNode->pStack);
   qurt_thread_attr_set_priority (&attr, (unsigned short)nPriority);
   qurt_thread_attr_set_bus_priority (&attr, QURT_THREAD_BUS_PRIO_ENABLED);
   qurt_thread_attr_set_name(&attr, (char*)pzThreadName);
   
    ret  = qurt_thread_create((qurt_thread_t*)pTid, &attr, qurt_elite_thread_stub, (void*)pThreadNode);

    if (QURT_EOK != ret)
    {
       *pTid = ret;
       // If there is failure to create thread, delete thread stack.
       if (FALSE == pThreadNode->fPreAllocatedStack)
       {
          qurt_elite_memory_free(pThreadNode->pStack);
       }
       // If there is failure to create thread, clear thread state and return EFAILED ack
       pThreadNode->nThreadState = QURT_ELITE_THREAD_NONE;	
       return ADSP_EFAILED;	   
    }

   // low prio debug message in case of difficult issues
#ifdef DEBUG_QURT_ELITE_THREAD
   MSG_4(MSG_SSID_QDSP6, DBG_LOW_PRIO, "THRD CREATE: Thread=0x%x stacktop=0x%x stackbottom=0x%x prio=%d", \
      *pTid, pThreadNode->pStack, pThreadNode->pStack + nStackSize, nPriority);
#endif /* DEBUG_QURT_ELITE_THREAD */
   // Med prio debug message to help associate thread name with thread ID
   // Due to Qshrink requirement of not supporting varaible string,
   // thread name has to be printed out in Hex.
#if (defined(SIM))
   MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "THRD CREATE: Thread=0x%x Name = %s", \
      *pTid, pzThreadName);
#else
   // Once we past Line 113:  memcpy(&pThreadInit->name1,
   // &(pzThreadName[8]), 8); we can treat pzThreadName as a char
   // [8].
   MSG_9(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "THRD CREATE: Thread=0x%x Name(Hex)= %x, %x, %x, %x, %x, %x, %x, %x", \
         *pTid,
         pzThreadName[0], pzThreadName[1], pzThreadName[2],pzThreadName[3],
         pzThreadName[4], pzThreadName[5], pzThreadName[6],pzThreadName[7] );
#endif

   return ADSP_EOK;
}

static void qurt_elite_thread_stub(void *arg)
{
   qurt_elite_thread_list_t *pThreadNode =(qurt_elite_thread_list_t *) arg;
   
   // record tid for thread joiner to lookup.
   pThreadNode->tid = qurt_thread_get_id();

   /* set thread state to running before calling entry function */
   qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
   pThreadNode->nThreadState = QURT_ELITE_THREAD_RUNNING;
   qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   int exit_status = pThreadNode->pfStartRoutine(pThreadNode->arg);
   pThreadNode->nExitStatus = exit_status;

#ifdef DEBUG_QURT_ELITE_THREAD
   MSG_2(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Exiting thread %d with status %d", qurt_thread_get_id(), exit_status);
#endif //DEBUG_QURT_ELITE_THREAD

   /* update thread state to exiting before exit thread */
   qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
   pThreadNode->nThreadState = QURT_ELITE_THREAD_EXITING;
   qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   qurt_thread_exit(exit_status);

   // code never reached, but is pleasing to the compiler.
   //return NULL;
   return;
}

void qurt_elite_thread_join(qurt_elite_thread_t nTid, int* nStatus)
{
   *nStatus = ADSP_EBADPARAM;
   int dummy,count;

   // ignore qurt_thread_join return value and status, since it is only reliable if
   // join() was called before exit()
   (void) qurt_thread_join(nTid, &dummy);

   // Find exit status, and remove node from global state structure
   for(count=0;count<QURT_ELITE_MAX_SW_THREADS;count++)
   {
      if (qurt_elite_globalstate.aThreadContextBuf[count].tid == nTid)
      {
         // check if thread is joined in state other than EXITING state.
         if (QURT_ELITE_THREAD_EXITING == (qurt_elite_globalstate.aThreadContextBuf[count].nThreadState))
         {
            *nStatus = qurt_elite_globalstate.aThreadContextBuf[count].nExitStatus;
            // Only free stack if it was not pre-allocated
            if (!(qurt_elite_globalstate.aThreadContextBuf[count].fPreAllocatedStack))
            {
               qurt_elite_memory_free(qurt_elite_globalstate.aThreadContextBuf[count].pStack);
            }
            qurt_elite_globalstate.aThreadContextBuf[count].nThreadState = QURT_ELITE_THREAD_NONE;
            return;
         }
     else
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "THRD JOIN: trying to join Thread %d in state %d", nTid, qurt_elite_globalstate.aThreadContextBuf[count].nThreadState);
            return;
         }
      }
   }
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "THRD JOIN: Thread %d is not found in global state structure", nTid);
}


void qurt_elite_thread_list_init(void)
{
   int i;
   for(i=0;i<QURT_ELITE_MAX_SW_THREADS;++i)
   {
      memset(&(qurt_elite_globalstate.aThreadContextBuf[i]),0,sizeof(qurt_elite_thread_list_t));
   }

}

int qurt_elite_thread_prio_get(void)
{
   return 255 - qurt_thread_get_priority(qurt_thread_get_id());
}

void qurt_elite_thread_set_prio(int nPrio)
{
   (void) qurt_thread_set_priority(qurt_thread_get_id(), 255 - nPrio);
}
