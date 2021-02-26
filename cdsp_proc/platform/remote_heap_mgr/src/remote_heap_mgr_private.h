/*==========================================================================

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

===========================================================================*/

#ifndef REMOTE_HEAP_MGR_PRIVATE_H
#define REMOTE_HEAP_MGR_PRIVATE_H

#define REMOTE_HEAP_MGR_STACK_SIZE          4 * 1024
#define REMOTE_HEAP_MGR_THREAD_PRIO	        0x3
#define REMOTE_HEAP_MGR_QUEUE_SIZE  10

#ifndef FREEIF
#define FREEIF(pv) \
do {\
    if(pv) { \
        void* tmp = (void*)pv;\
        pv = 0;\
        free(tmp);\
    } \
} while(0)
#endif /*FREEIF*/

struct RemoteHeapMgrInstanceStruct{
    void* cmdThreadStack;
    qurt_thread_t cmdThreadId;
    qurt_sem_t fullSemaphore;
    qurt_sem_t emptySemaphore;
    qurt_mutex_t mallocFailMutex;
    qurt_cond_t  mallocFailCond;
    unsigned int growRequestsinQueue;
    QList emptyList;
    QList fullList;
    uint8 isInitialized;
};

typedef struct{
    QNode qnode;
    struct remote_heap_mgr_work_item msg;
}Work;

static int remote_heap_mgr_init(void);
static void remote_heap_mgr_deinit(void);
static void process_msg(void *);
static int launch_cmd_processing_thread(void);

#endif /*REMOTE_HEAP_MGR_PRIVATE_H*/
