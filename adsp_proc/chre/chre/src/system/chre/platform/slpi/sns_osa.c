/*=============================================================================
  @file sns_osa.c

  Several Operating System Abstractions to be used by the sns_qsocket client
  library.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "qurt.h"
#include "qurt_rmutex.h"
#include "qurt_sem.h"
#include "sns_osa.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#define CHRE_SNS_THREAD_STACK_SIZE 2048

#define CHRE_SNS_THREAD_PRIO 0x8D


/*=============================================================================
  type definitions
  ===========================================================================*/

typedef struct sns_osa_thread
{
  qurt_thread_t thread;
  sns_osa_thread_func func;
  void *arg;
} sns_osa_thread;

typedef struct sns_osa_mutex
{
  qurt_mutex_t mutex;
} sns_osa_mutex;

typedef struct sns_osa_sem
{
  qurt_sem_t sem;
} sns_osa_sem;

/*=============================================================================
  Static Data
  ===========================================================================*/

static uint8_t thread_stack[CHRE_SNS_THREAD_STACK_SIZE];

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Wrapper function, as QURT's thread entry function prototype is different
 * from pthread, which this OSA is modeled after.
 */
static void
thread_func(void *arg)
{
  sns_osa_thread *thread = arg;
  thread->func(thread->arg);
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int
sns_osa_thread_create(sns_osa_thread **thread_out, sns_osa_thread_func func, void *arg)
{
  int err;
  char thread_name[] = "CHRE_SNS";
  qurt_thread_attr_t attr;
  sns_osa_thread *thread;

  thread = sns_malloc(sizeof(*thread));
  SNS_ASSERT(NULL != thread);
  thread->func = func;
  thread->arg = arg;

  qurt_thread_attr_init(&attr);
  qurt_thread_attr_set_name(&attr, thread_name);
  qurt_thread_attr_set_priority(&attr, CHRE_SNS_THREAD_PRIO);
  qurt_thread_attr_set_stack_size(&attr, CHRE_SNS_THREAD_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&attr, &thread_stack[0]);

  err = qurt_thread_create(&thread->thread, &attr, &thread_func, thread);
  if(QURT_EOK != err)
  {
    SNS_LOG(ERROR, "Failed to create CHRE_SNS thread");
    return -1;
  }

  *thread_out = thread;
  return 0;
}

int
sns_osa_mutex_create(sns_osa_mutex **mutex_out)
{
  sns_osa_mutex *mutex;

  mutex = sns_malloc(sizeof(*mutex));
  SNS_ASSERT(NULL != mutex);

  qurt_rmutex_init(&mutex->mutex);

  *mutex_out = mutex;
  return 0;
}

int
sns_osa_mutex_lock(sns_osa_mutex *mutex)
{
  qurt_rmutex_lock(&mutex->mutex);

  return 0;
}

int
sns_osa_mutex_unlock(sns_osa_mutex *mutex)
{
  qurt_rmutex_unlock(&mutex->mutex);

  return 0;
}

int
sns_osa_mutex_destroy(sns_osa_mutex *mutex)
{
  qurt_rmutex_destroy(&mutex->mutex);
  sns_free(mutex);

  return 0;
}

int
sns_osa_sem_create(struct sns_osa_sem **sem_out)
{
  sns_osa_sem *sem;

  sem = sns_malloc(sizeof(*sem));
  SNS_ASSERT(NULL != sem);

  qurt_sem_init_val(&sem->sem, 0);

  *sem_out = sem;
  return 0;
}

int
sns_osa_sem_post(struct sns_osa_sem *sem)
{
  qurt_sem_up(&sem->sem);
  return 0;
}

int
sns_osa_sem_wait(struct sns_osa_sem *sem)
{
  qurt_sem_down(&sem->sem);
  return 0;
}

int
sns_osa_sem_destroy(struct sns_osa_sem *sem)
{
  qurt_sem_destroy(&sem->sem);
  sns_free(sem);
  return 0;
}
