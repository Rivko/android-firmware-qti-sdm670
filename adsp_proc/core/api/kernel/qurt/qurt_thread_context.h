#ifndef QURT_THREAD_CONTEXT_H
#define QURT_THREAD_CONTEXT_H
/**
  @file qurt_thread_context.h 
  @brief Kernel thread context structure
			
EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



#include <qurt_qdi_constants.h>

static inline int qurt_thread_iterator_create(void)
{
   return qurt_qdi_handle_invoke(QDI_HANDLE_GENERIC, QDI_OS_THREAD_ITERATOR_CREATE);
}

static inline qurt_thread_t qurt_thread_iterator_next(int iter)
{
   return qurt_qdi_handle_invoke(iter, QDI_OS_THREAD_ITERATOR_NEXT);
}

static inline int qurt_thread_iterator_destroy(int iter)
{
   return qurt_qdi_close(iter);
}

#define THREAD_ITERATOR_END ((qurt_thread_t)(-1))

int qurt_thread_context_get_tname(unsigned int thread_id, char *name, unsigned char max_len);
int qurt_thread_context_get_prio(unsigned int thread_id, unsigned char *prio);
int qurt_thread_context_get_pcycles(unsigned int thread_id, unsigned long long int *pcycles);
int qurt_thread_context_get_stack_base(unsigned int thread_id, unsigned int *sbase);
int qurt_thread_context_get_stack_size(unsigned int thread_id, unsigned int *ssize);

int qurt_thread_context_get_pid(unsigned int thread_id, unsigned int *pid);
int qurt_thread_context_get_pname(unsigned int thread_id, char *name, unsigned int len);


/* new structure defining how tcb is interpreted to crash dump tools
 * definitions of keys in consts.h */
struct qurt_debug_thread_info {
    char name[QURT_MAX_NAME_LEN]; //64
    struct {
        unsigned key;
        unsigned val;
    } os_info[40];  
    unsigned gen_regs[32];
    unsigned user_cregs[32];
    unsigned guest_cregs[32];
    unsigned monitor_cregs[64];
}; /* should add up to 1K */

/* return: size of debug thread info structure, may change without warning */
size_t qurt_system_tcb_dump_get_size(void);

/* return: QURT_EOK for success, otherwise failure
 * arguments:
        - thread_id: tcb to process by thread_id
        - ptr: pointer to the local buffer where the contents will be dumped
        - size: size of the debug thread info structure, obtained from above API
 */
int qurt_system_tcb_dump_get(qurt_thread_t thread_id, void *ptr, size_t size);

#endif
