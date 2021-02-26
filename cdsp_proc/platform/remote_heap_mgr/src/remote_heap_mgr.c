/*==========================================================================

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

===========================================================================*/

#include <stdlib.h>

#define FARF_ERROR 1
#define FARF_HIGH 1

#include "qurt.h"
#include "AEEQList.h"
#include "verify.h"
#include "HAP_farf.h"
#include "AEEstd.h"
#include "platform_libs.h"
#include "remote_heap_mgr.h"
#include "remote_heap_mgr_private.h"
#include "remoteheap.h"
#include "timer.h"
#include "AEEStdErr.h"

static struct RemoteHeapMgrInstanceStruct gHeapMgrInstance = {0};


PL_DEP(fastrpc_smd);

static int remote_heap_mgr_init(){
    int nErr = AEE_SUCCESS;
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    int i=0;
    Work* work = NULL;

    VERIFY (me->isInitialized == 0);

    VERIFY(0 == PL_INIT(fastrpc_smd));
    QList_Ctor(&me->emptyList);
    QList_Ctor(&me->fullList);
    qurt_sem_init_val(&me->emptySemaphore,0);
    qurt_mutex_init(&me->mallocFailMutex);
    qurt_cond_init(&me->mallocFailCond);
    me->growRequestsinQueue = 0;

    // Add pre-alllcated nodes to the empty list
    for (i=0;i<REMOTE_HEAP_MGR_QUEUE_SIZE;i++){
        VERIFYC(NULL != (work = (Work*) malloc(sizeof(Work))), AEE_ENOMEMORY);

        // Enqueue the data
        QNode_CtorZ(&work->qnode);
        QList_AppendNode(&me->emptyList, &work->qnode);
        qurt_sem_up(&me->emptySemaphore);
    }

    // Create Queue, Semaphore and thread to handle commands
    VERIFY(AEE_SUCCESS == launch_cmd_processing_thread());
    FARF(HIGH, "HeapMgrInit complete");
    me->isInitialized = 1;
bail:
    return nErr;
}

static void remote_heap_mgr_deinit(){
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    int nErr = AEE_SUCCESS;
    QNode* pn = NULL;
    Work* pw = NULL;

    if(me->isInitialized == 0)
        return;

    // Up the semaphore. This will cause the worker thread to quit
    qurt_sem_up(&me->fullSemaphore);
    qurt_thread_join(me->cmdThreadId, &nErr);

    //Clean up the full queue
    while(!QList_IsEmpty(&me->fullList)){
        pn = QList_PopZ(&me->fullList);
        pw = ((pn == 0) ? 0 : STD_RECOVER_REC(Work, qnode, pn));
        FREEIF(pw);
    }

    //Clean up the empty queue
    while(!QList_IsEmpty(&me->emptyList)){
        pn = QList_PopZ(&me->emptyList);
        pw = ((pn == 0) ? 0 : STD_RECOVER_REC(Work, qnode, pn));
        FREEIF(pw);
    }

    qurt_mutex_destroy(&me->mallocFailMutex);
    qurt_cond_destroy(&me->mallocFailCond);
    qurt_sem_destroy(&me->fullSemaphore);
    qurt_sem_destroy(&me->emptySemaphore);


    PL_DEINIT(fastrpc_smd);
    FREEIF(me->cmdThreadStack);
    me->isInitialized = 0;
}

static int launch_cmd_processing_thread(){
    int nErr = AEE_SUCCESS, nQErr = AEE_SUCCESS;
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    qurt_thread_attr_t attr;
    me->cmdThreadStack = NULL;
    me->cmdThreadId = -1;

    qurt_sem_init_val(&me->fullSemaphore,0);
    qurt_thread_attr_init (&attr);
    qurt_thread_attr_set_stack_size (&attr, REMOTE_HEAP_MGR_STACK_SIZE);
    qurt_thread_attr_set_priority (&attr, REMOTE_HEAP_MGR_THREAD_PRIO);
    qurt_thread_attr_set_name (&attr, "rhm");
    VERIFYC(NULL != (me->cmdThreadStack =
           malloc(REMOTE_HEAP_MGR_STACK_SIZE * sizeof(uint8))), AEE_ENOMEMORY);
    qurt_thread_attr_set_stack_addr (&attr,
                                     (unsigned long long *)me->cmdThreadStack);
    VERIFYC(QURT_EOK == (nQErr = qurt_thread_create(&me->cmdThreadId, &attr,
           process_msg, NULL)), AEE_EQURTTHREADCREATE);

bail:
   if(nErr != AEE_SUCCESS){
	   FARF(ERROR, "Error:%x(%x) Remote Heap Mgr Create Instance Failed", nErr, nQErr);
        FREEIF(me->cmdThreadStack);
	}
    return nErr;
}


static void process_msg(void *arg){
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    QNode* pn = NULL;
    Work* work = NULL;
    int nErr = AEE_SUCCESS;

    do {
        qurt_sem_down(&me->fullSemaphore);
        pn = QList_PopZ(&me->fullList);
        work = ((pn == 0) ? 0 : STD_RECOVER_REC(Work, qnode, pn));

        if (work == NULL){
            FARF(ERROR, "Received exit");
            break;
        }

        FARF(HIGH,"Processing command. cmd=%d heap_ptr=0x%x size=%d, sleep_timer=%d",
             work->msg.cmd, work->msg.ptr.heap_ptr,
             work->msg.size, work->msg.sleep_timer);

        // Sleep if necessary
        if(work->msg.sleep_timer != 0){
            timer_sleep(work->msg.sleep_timer*1000,T_USEC, FALSE);
        }

        switch(work->msg.cmd) {
            case INIT_REMOTEHEAP:
                nErr = pl_grow_heap_dynamically(work->msg.ptr.heap_ptr, work->msg.size);
            break;
            case GROW_HEAP:
                // Do something here
                nErr =
                    pl_grow_heap(work->msg.ptr.heap_ptr, work->msg.size);
            break;
            case SHRINK_HEAP:
                // Do something here
                nErr =
                    pl_release_heap(work->msg.ptr.heap_ptr, work->msg.ptr.vaddr, work->msg.size);
            break;
        }

/*      If we need to retry (because the apps is busy),
        And we haven't retied too many times then update the timer*/
        if ( (nErr == E_APPS_BUSY_RETRY_LATER) &&
             (work->msg.sleep_timer < (1 << MAX_RETRIES)) ) {
            work->msg.sleep_timer =
                (work->msg.sleep_timer == 0)? 1 : 2*work->msg.sleep_timer;

            // Add the node back to the queue
            QList_AppendNode(&me->fullList, &work->qnode);
            qurt_sem_up(&me->fullSemaphore);
            continue;
        }
        // Signal the condition so waiting threads can continue
        // We signal even on failure to handle cases where the
        // apps is out of memory
        else if (work->msg.cmd == GROW_HEAP) {
			qurt_mutex_lock(&me->mallocFailMutex);
            me->growRequestsinQueue--;
            qurt_cond_broadcast(&me->mallocFailCond);
			qurt_mutex_unlock(&me->mallocFailMutex);
        }

        // Enqueue the data tp the empty list
        QNode_CtorZ(&work->qnode);
        QList_AppendNode(&me->emptyList, &work->qnode);
        qurt_sem_up(&me->emptySemaphore);
    } while(1);

    FREEIF(work);
}

int remote_heap_mgr_enqueue_cmd(struct remote_heap_mgr_work_item* _msg){
    int nErr = AEE_SUCCESS;
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    Work* work = NULL;
    QNode* pn = NULL;

    VERIFYC(_msg, AEE_EMEMPTR);

    // If this a request to add memory,
    // then lock the mutex. This will allow failed mallocs to wait
    // on completion of the allocation
    // Also store the requested growth size
    // so we can automatically allocate more if required
    if(_msg->cmd == GROW_HEAP){
        me->growRequestsinQueue++;
    }

    // Get a node from the empty queue.
    // We wait till we get an empty node
    qurt_sem_down(&me->emptySemaphore);
    pn = QList_PopZ(&me->emptyList);
    work = ((pn == 0) ? 0 : STD_RECOVER_REC(Work, qnode, pn));

    // Deep copy the message
    memcpy(&work->msg, _msg, sizeof(struct remote_heap_mgr_work_item));

    // Enqueue the data tp the full list
    QNode_CtorZ(&work->qnode);
    QList_AppendNode(&me->fullList, &work->qnode);
    qurt_sem_up(&me->fullSemaphore);

bail:
    if(nErr){
        FREEIF(work);
    }

    return nErr;
}

int remote_heap_mgr_handle_failure_cb_handler(void* heap_ptr,
        size_t request_size, size_t default_grow_size){
    int nErr = AEE_EMEMPTR;
    struct RemoteHeapMgrInstanceStruct* me = &gHeapMgrInstance;
    struct remote_heap_mgr_work_item item;

    FARF(LOW, "Handling failed malloc request.growRequestsinQueue=%d",
         me->growRequestsinQueue);
    // Verify that the last add request will fulfil the memory required in this
    // failure request
	qurt_mutex_lock(&me->mallocFailMutex);
    if(me->growRequestsinQueue == 0){
        item.cmd = GROW_HEAP;
        item.ptr.heap_ptr = heap_ptr;
        item.size = default_grow_size;
        item.sleep_timer = 0;
        FARF(LOW, "No grow requests in queue. Adding one!");
        remote_heap_mgr_enqueue_cmd(&item);
    }

    // Wait till a section has been added
    qurt_cond_wait(&me->mallocFailCond, &me->mallocFailMutex);
    FARF(LOW, "Section added. Malloc should succeed now");
    qurt_mutex_unlock(&me->mallocFailMutex);

    return nErr;
}

PL_DEFINE(remote_heap_mgr, remote_heap_mgr_init, remote_heap_mgr_deinit);
