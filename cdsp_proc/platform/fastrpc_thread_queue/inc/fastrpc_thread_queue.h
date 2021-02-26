/*========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
  ====================================================================== */
#ifndef FASTRPC_THREAD_QUEUE_H
#define FASTRPC_THREAD_QUEUE_H

#include "HAP_farf.h"
#include "AEEQList.h"
#include "AEEStdDef.h"
#include "AEEQListExt.h"
#include "fastrpc_internal.h"
#include "fastrpc_port.h"
#include "AEEatomic.h"
#include "qurt.h"

/**
 * Used for flag in ftq_group to identify PD type
 */
#define STATIC_PD_TYPE	1
#define DYNAMIC_PD_TYPE	2

/**
 * library for keeping track of threads and PDs to service HLOS FastRPC requests
 * should never allocate when enqueueing
 */

struct ftq_thread;
/**
 * reserved messages used to avoid allocation when a thread isn't there yet
 */
struct ftq_rmsg {
   QNode qn;
   struct fastrpc_port_msg msg;
};

/**
 * a threads space for a messge, could be usermalloced, so we cant trust the contents
 */
struct ftq_umsg {
   struct fastrpc_port_msg msg;
   int pending;
};

struct ftq_group;
/**
 * a thread struture, could be in the kernel or user pd
 */
struct ftq_thread {
   QNode AA; //! key for looking up by android thread id in the global thread list for all processes
   QNode QQ; //! key for looking up by qurt thread id in the process-specific thread list
   QNode GC; //! key for looking up by qurt thread id in the process-specific thread cleanup list
   QNode PP; //! key for looking up by android thread id in the process-specific thread list
   struct ftq_umsg *umsg;  //! space for messages, could be usermalloced
   struct ftq_umsg lmsg;
   struct ftq_group *pgrp;
   qurt_thread_t tidQ;     
   int tidA;
   boolean isMigrated;
   int client_handle;
   unsigned *fut;
   int exit;
   int initdone;
};

struct ftq_groups;

struct spd_group {
	QNode qn;
	int clienthandle;
	int asid;
	char *staticPdname;
	int spdattach;
};


DEFINE_TQLIST(group_threads, struct ftq_thread, QQ);
DEFINE_TQLIST(group_threadsA, struct ftq_thread, PP);
DEFINE_TQLIST(gc_threads, struct ftq_thread, GC);
DEFINE_TQLIST(pending_invokes, struct ftq_rmsg, qn);
DEFINE_TQLIST(new_invokes, struct ftq_rmsg, qn);
//! group of threads in the kernel or for a pd, representing a single HLOS pd
struct ftq_group {
   group_threads threadsQ;
   gc_threads threadsGC;
   struct ftq_thread* thread0;  //! the main thread used for creating new threads
   struct ftq_groups* procs;
   QNode AA;                    //! key for HLOS pid
   int client_handle;
   int asid;
   int pidA;
   int pdwait;
   int secure;
   int pdexit;
   unsigned exit;
   char name[256];
   int staticpd_pdr;
   int pd_type;					//! Flag to identify PD as static/dynamic
   /** 
    * invokes and pending are lists of ftq_rmsg
    * anything in these lists should come from procs->free
    */
   new_invokes invokes;  //! new incomming messages for thread0
   pending_invokes pending;  //! pending messages for newly created threads
   group_threadsA threadsA;  //! QList of all threads in process key-ed by Android thread ID
};

void ftq_close(void(*group_dtor)(int pidA));

struct ftq_thread* ftq_thread_find(qurt_thread_t tidQ);
struct ftq_group* ftq_group_lookup_by_asid(int asid);
struct spd_group* spd_group_asid_lookup(const char* staticPdName);
void spdgroups_append(struct spd_group* spd);
int spdgroups_remove(uint32_t asid);
int ftq_group_pidA_to_asid(int pidA, int* asid);
int get_pd_type_from_ftq_group_pidA(int pidA, int* pd_type);

int ftq_enqueue(struct fastrpc_port_msg* msg);
int ftq_thread_wake(struct ftq_thread *th);

int ftq_group_init(int pidA, struct ftq_group* pgrp);
int ftq_group_default_init(int pidA);
int ftq_group_default_deinit(int pidA,
                     void(*thread_dtor)(struct ftq_thread* th),
                     int ret);
int ftq_group_reset_thread0(struct ftq_group *pgrp, qurt_thread_t tidQ, int handle);

boolean ftq_group_is_default(int pidA);
 
int ftq_group_default_wait(void);

struct ftq_group* ftq_group_deinit(int pidA, void(*thread_dtor)(struct ftq_thread* th),
                                   int ret);
struct ftq_group* ftq_group_deinit_staticpd(int pidA, void(*thread_dtor)(struct ftq_thread* th),
                                   int ret);

struct ftq_thread* ftq_thread_exit(struct ftq_group* pgrp, qurt_thread_t tidQ);

void ftq_group_cleanup_threads(struct ftq_group* pgrp,
                              qurt_thread_t self,
                              void(*thread_dtor)(struct ftq_thread* th));

int ftq_group_notify(struct ftq_group* pgrp);
int ftq_thread_notify(struct ftq_group* pgrp, qurt_thread_t tidQ);

void ftq_group_exit(int pidA);

//! thread 0 should use this to respond to newly created thread requests
int ftq_thread0_requeue(struct ftq_group* pgrp, qurt_thread_t tidQ);
int ftq_thread0_requeue_pid(struct ftq_thread* th, int pidA);

int ftq_thread0_init(struct ftq_thread* th, 
                     qurt_thread_t tidQ);
/**
 * migrate thread into the defaultPID, blocking until the PID is created
 */
int ftq_thread_migrate_default(struct ftq_thread* th, 
                               struct ftq_umsg* umsg,
                               unsigned *fut,
                               qurt_thread_t tidQ,
                               int(*init_locked)(struct ftq_thread* th));
int ftq_thread_migrate(struct ftq_thread* th, 
                       struct ftq_umsg* umsg,
                       unsigned *fut,
                       qurt_thread_t tidQ,
                       int pidA,
                       int(*init_locked)(struct ftq_thread* th));
/**
 * if tidA is 0, then we are creating thread0
 * otherwise, a pending message must be in the pgrp->pending list with the tidA id
 */
int ftq_thread_init(struct ftq_thread* th, 
                    int tidA,
                    qurt_thread_t tidQ);
//! normal response
void ftq_thread_respond(struct ftq_group* pgrp, qurt_thread_t tidQ, int ret);

void ftq_group_respond_pending(struct ftq_group* pgrp, int tidA, int ret);

//! pop and enqueue msg from invokes list
void ftq_thread0_requeue_invokes(struct ftq_thread* th);

void ftq_thread_ctor2(struct ftq_group* pgrp, struct ftq_thread* th, int client_handle, struct ftq_umsg* umsg, unsigned *fut);
//! deinit should be called outside the thread, on a thread that can "free" the thread it's deiniting
void ftq_thread_deinit(struct ftq_thread* th, int ret);

//! leave should be called by the thread that is leaving
void ftq_thread_leave(struct ftq_thread* th, int ret);

void fastrpc_get_procs_mut(qurt_mutex_t **mut);

static __inline int ftq_is_kernel_msg(struct fastrpc_port_msg* msg) {
   return 0 == msg->msg.pid || msg->msg.invoke.header.handle == 1;
}

#endif // FASTRPC_THREAD_QUEUE_H

