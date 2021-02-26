 /***********************************************************************
 * osal_qurt_types.h
 * This file maps the osal types to native QURT types.
 *
 * Copyright (C) 2011, 2013-14 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/services/osal/qurt/inc/osal_qurt_types.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-07-06    ph   Added support for pthread thread local storage  
2013-10-03    ph   OSAL APIs updated to use ATS timers instead of qurt timers.
2011-09-15    sg   Created

===========================================================================*/

#ifndef __OSAL_QURT_TYPES_H__
#define __OSAL_QURT_TYPES_H__

#include "qube.h"
#include "qurt.h"
#include "atomic_ops.h"
#include "timer.h"

/* just a random value picked for size of table that maps the tcb pointer to the
thread ID. Can be increased to whatever value seems enough */
#define QURT_MAX_THREADS 30

typedef unsigned int                     osal_sigs_t;

typedef timer_cb_data_type        osal_timer_param_type;

/* this structure contains elements that unique to each thread */
typedef struct
{
/* The handle created to store the thread_id of the QURT thread */
  int       thd_id;

/* the currently asserted messages for the thread */
  qurt_anysignal_t       current_sigs;

  qurt_mutex_t         tcb_msg_lock;

  char             task_name[30]; /* random number */

}qurt_tcb_t;


/* This table maps the tcb pointer of the thread to the thread ID. Needed
for the thread_self() API. */
typedef struct
{
	int              thd_id;
	qurt_tcb_t      *tcb_id;
}tcb_map_table;

/* Thread attributes object */
typedef struct
{
  dword stack_size;
  void (*start_func)(uint32 );
  void *arg;
  void *name;
  dword priority;
  void *stack_address;
  char  *p_tskname;              /* A/N string for task name */
  boolean suspended;              /* is task initially susp?  */
  int dog_report_val;          /* Dog report value         */
} blast_thread_attr_t;


/* Bind OSAL structures to Qube structures */
typedef qurt_tcb_t                      osal_tcb_t;
typedef timer_type                    osal_timer_t;
typedef timer_group_type              osal_timer_group;
typedef qurt_mutex_t                    osal_crit_sect_t;
typedef qurt_mutex_t                    osal_mutex_t;
typedef unsigned long long            osal_timer_cnt_t;
typedef size_t                           osal_stack_word_type;
typedef void *                            osal_timer_cb_type;
typedef int                              osal_priority_type;
typedef blast_thread_attr_t               osal_thread_attr_t;
typedef atomic_word_t                      osal_atomic_word_t;
 typedef atomic_plain_word_t                osal_atomic_plain_word_t;
 typedef qurt_thread_t                     osal_thread_t;
typedef int                             osal_tls_key_t;

 /* Mutex object containing the handle to the mutex and a name for the mutex */
typedef struct
{
  osal_mutex_t handle;
  const char *name;
} osal_mutex_arg_t;

#endif /* not __OSAL_QURT_TYPES_H__ */

