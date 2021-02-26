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
#include <pthread.h>
#include <semaphore.h>
#include "sns_osa.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/

typedef struct sns_osa_thread
{
  pthread_t thread;
} sns_osa_thread;

typedef struct sns_osa_mutex
{
  pthread_mutex_t mutex;
} sns_osa_mutex;

typedef struct sns_osa_sem
{
  sem_t sem;
} sns_osa_sem;

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int
sns_osa_thread_create(sns_osa_thread **thread_out, sns_osa_thread_func func, void *arg)
{
  sns_osa_thread *thread;

  thread = sns_malloc(sizeof(*thread));
  SNS_ASSERT(NULL != thread);

  *thread_out = thread;
  return pthread_create(&thread->thread, NULL, func, arg);
}

int
sns_osa_mutex_create(sns_osa_mutex **mutex_out)
{
  sns_osa_mutex *mutex;
  pthread_mutexattr_t attr;
  int rv;

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  mutex = sns_malloc(sizeof(*mutex));
  SNS_ASSERT(NULL != mutex);

  rv = pthread_mutex_init(&mutex->mutex, &attr);
  pthread_mutexattr_destroy(&attr);

  *mutex_out = mutex;
  return rv;
}

int
sns_osa_mutex_lock(sns_osa_mutex *mutex)
{
  return pthread_mutex_lock(&mutex->mutex);
}

int
sns_osa_mutex_unlock(sns_osa_mutex *mutex)
{
  return pthread_mutex_unlock(&mutex->mutex);
}

int
sns_osa_mutex_destroy(sns_osa_mutex *mutex)
{
  return pthread_mutex_destroy(&mutex->mutex);
}

int
sns_osa_sem_create(struct sns_osa_sem **sem_out)
{
  sns_osa_sem *sem;
  int rv;

  sem = sns_malloc(sizeof(*sem));
  SNS_ASSERT(NULL != sem);

  rv = sem_init(&sem->sem, 0, 0);

  *sem_out = sem;
  return rv;
}

int
sns_osa_sem_post(struct sns_osa_sem *sem)
{
  return sem_post(&sem->sem);
}

int
sns_osa_sem_wait(struct sns_osa_sem *sem)
{
  return sem_wait(&sem->sem);
}

int
sns_osa_sem_destroy(struct sns_osa_sem *sem)
{
  return sem_destroy(&sem->sem);
}
