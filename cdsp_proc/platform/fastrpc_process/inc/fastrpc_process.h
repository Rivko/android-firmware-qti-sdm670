#ifndef FASTRPC_PROCESS_H
#define FASTRPC_PROCESS_H

#include "qurt_thread.h"

int fastrpc_thread_notify(qurt_thread_t tidQ);
int fastrpc_thread_wait(void);
int fastrpc_group_notify(void);
int fastrpc_thread_context_enter(void);
int fastrpc_thread_context_leave(void);

int fastrpc_kprocess_createk(void);
void fastrpc_kprocess_destroyk(void);
int fastrpc_uprocess_dispatchu(void);
int adsp_default_listener_register(void);
int fastrpc_uprocess_migrate_cpz(int heapid);
int fastrpc_uprocess_group_wait(void);
int fastrpc_uprocess_reset(void);
int fastrpc_uprocess_alloc(void);
void fastrpc_uprocess_free(void);
struct fastrpc_uthread* fastrpc_uthread_find(qurt_thread_t tidQ);

#endif // FASTRPC_PROCESS_H
