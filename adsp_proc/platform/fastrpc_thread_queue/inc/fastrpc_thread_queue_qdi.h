#ifndef FASTRPC_THREAD_QUEUE_QDI_H
#define FASTRPC_THREAD_QUEUE_QDI_H
#include "fastrpc_thread_queue.h"
#include "qurt.h"
#include "qurt_qdi.h"

/**
 * If any API arguments change, or method changes sementics 
 * you must create a new api version on an unused number.  The new method
 * number must be added to the original p4 location of this file, even if
 * its only used by a single branch on a single target.
 *
 * Target/branch specific method implementations may return -1.
 *
 * p4://qctp406/source/qcom/qct/platform/adsp/proj/fastrpc_thread_queue/fastrpc_thread_queue.h
 *
 */
#define QDI_FTQ_NAME                   "/dev/fastrpc-smd"
#define QDI_FTQ_BASE                   256
#define QDI_FTQ_PROCESS_INIT           (QDI_FTQ_BASE + 0) //deprecated
#define QDI_FTQ_THREAD_INIT            (QDI_FTQ_BASE + 1) //deprecated
#define QDI_FTQ_THREAD_EXIT            (QDI_FTQ_BASE + 2) //deprecated
#define QDI_FTQ_INVOKE_RSP             (QDI_FTQ_BASE + 3)
#define QDI_FTQ_INVOKE_REQUEUE         (QDI_FTQ_BASE + 4)
#define QDI_FTQ_INVOKE_RSP_PENDING     (QDI_FTQ_BASE + 5)
#define QDI_FTQ_MEM_ADD                (QDI_FTQ_BASE + 6)
#define QDI_FTQ_POOL_ADD_PAGES         QDI_FTQ_MEM_ADD
#define QDI_FTQ_POOL_REMOVE_PAGES      (QDI_FTQ_BASE + 8)
#define QDI_FTQ_THREAD_EXIT_ERR        (QDI_FTQ_BASE + 9)
#define QDI_FTQ_PROCESS_EXIT           (QDI_FTQ_BASE + 10)
#define QDI_FTQ_PROCESS_INIT2          (QDI_FTQ_BASE + 11)
#define QDI_FTQ_THREAD_INIT2           (QDI_FTQ_BASE + 12)
#define QDI_FTQ_THREAD_GETMSG          (QDI_FTQ_BASE + 13)
#define QDI_FTQ_RESET_THREAD0          (QDI_FTQ_BASE + 14)
#define QDI_FTQ_PROCESS_SECURE         (QDI_FTQ_BASE + 15)
#define QDI_FTQ_START_U2G_SERVER       (QDI_FTQ_BASE + 16)
#define QDI_FTQ_PROCESS_STATE          (QDI_FTQ_BASE + 17)
#define QDI_FTQ_PROCESS_RELEASE         (QDI_FTQ_BASE + 18)

static __inline int ftq_qdi_open(void) {
   return qurt_qdi_open(QDI_FTQ_NAME);
}
static __inline int ftq_qdi_close(int handle) {
   return qurt_qdi_close(handle);
}
static __inline int ftq_qdi_process_init2(int handle, qurt_thread_t tidQ, char name[255], struct ftq_umsg* pmsg, unsigned *fut) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_PROCESS_INIT2, tidQ, name, pmsg, fut);
}
static __inline int ftq_qdi_thread_init2(int handle, qurt_thread_t tidQ, int tidA, struct ftq_umsg* pmsg, unsigned *fut) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_THREAD_INIT2, tidQ, tidA, pmsg, fut);
}
static __inline int ftq_qdi_thread_exit_err(int handle, qurt_thread_t tidQ, int err) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_THREAD_EXIT_ERR, tidQ, err);
}
static __inline int ftq_qdi_invoke_rsp(int handle, qurt_thread_t tidQ, int ret) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_INVOKE_RSP, tidQ, ret);
}
static __inline int ftq_qdi_invoke_requeue(int handle, qurt_thread_t tidQ) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_INVOKE_REQUEUE, tidQ);
}
static __inline int ftq_qdi_invoke_rsp_pending(int handle, int tidA, int ret) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_INVOKE_RSP_PENDING, tidA, ret);
}
static __inline int ftq_qdi_mem_add(int handle, unsigned addr, unsigned size) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_MEM_ADD, addr, size);
}
static __inline int ftq_qdi_pool_add_pages(int handle, qurt_mem_pool_t pool, unsigned addr, unsigned size) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_POOL_ADD_PAGES, pool, addr, size);
}
static __inline int ftq_qdi_pool_remove_pages(int handle, qurt_mem_pool_t pool, unsigned addr, unsigned size) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_POOL_REMOVE_PAGES, pool, addr, size);
}
static __inline int ftq_qdi_process_exit(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_PROCESS_EXIT);
}
static __inline int ftq_qdi_getmsg(int handle, qurt_thread_t tidQ, struct ftq_umsg* msg) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_THREAD_GETMSG, tidQ, msg);
}
static __inline int ftq_qdi_reset_thread0(int handle, qurt_thread_t tidQ) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_RESET_THREAD0, tidQ);
}

static __inline int ftq_qdi_set_process_secure(int handle, int state) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_PROCESS_SECURE, state);
}
static __inline int ftq_qdi_start_u2g_server(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_START_U2G_SERVER);
}
static __inline int ftq_qdi_get_process_state(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_PROCESS_STATE);
}

static __inline int ftq_qdi_set_process_release(int handle) {
   return qurt_qdi_handle_invoke(handle, QDI_FTQ_PROCESS_RELEASE);
}


#ifdef QURT_POOL_REMOVE_ALL_OR_NONE

static __inline int ftq_qdi_add_pages(int client_handle, unsigned ppn, unsigned pages, qurt_mem_pool_t pool) {
   return qurt_mem_pool_add_pages(pool, ppn, pages);
}

static __inline int ftq_qdi_remove_pages(qurt_mem_pool_t pool, unsigned ppn, unsigned pages) {
   return qurt_mem_pool_remove_pages(pool, ppn, pages, QURT_POOL_REMOVE_ALL_OR_NONE, 0, 0);
}

#else

static __inline int ftq_qdi_add_pages(int client_handle, unsigned ppn, unsigned pages, qurt_mem_pool_t pool) {
   return qurt_qdi_handle_invoke(client_handle, QDI_OS_MEM_POOL_ADD, ppn, pages);
}

static __inline int ftq_qdi_remove_pages(qurt_mem_pool_t pool, unsigned ppn, unsigned pages) {
   return -1;
}


#endif

/**
 * kernel library functions
 */
int ftq_qdi_group_kcreateu(const char* name, int pidA, int asid, int client_handle, int pd_type);
void ftq_qdi_group_kdestroyu(int client_handle);
void ftq_qdi_group_kdestroyu_staticpd(int client_handle);
int ftq_qdi_set_pgrp_exit(int asid);

#endif // FASTRPC_THREAD_QUEUE_QDI_H
