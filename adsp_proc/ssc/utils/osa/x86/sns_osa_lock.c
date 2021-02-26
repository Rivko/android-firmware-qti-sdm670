/*============================================================================
  @file sns_osa_lock.c

  @brief
  OS Abstraction layer for Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

#define _GNU_SOURCE

/*============================================================================
  INCLUDES
  ============================================================================*/
#include <pthread.h>
#include "sns_assert.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_types.h"

/*============================================================================
  Typedefs
  ============================================================================*/
typedef struct
{
  pthread_mutex_t mutex;
} sns_osa_x86_mutex;

/*============================================================================
  Public   Functions
  ============================================================================*/
sns_rc
sns_osa_lock_attr_init(sns_osa_lock_attr *attrib)
{
  UNUSED_VAR(attrib);
  _Static_assert(sizeof(sns_osa_lock) >= sizeof(sns_osa_x86_mutex),
  "__SIZEOF_LOCK is smaller than sizeof(sns_osa_lock)");
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_lock_attr_set_memory_partition(
  sns_osa_lock_attr      *attrib,
  sns_osa_mem_type       mem_type)
{
  UNUSED_VAR(attrib);
  UNUSED_VAR(mem_type);
  return SNS_RC_SUCCESS;
}

sns_rc
sns_osa_lock_create(const sns_osa_lock_attr *attrib, sns_osa_lock **lock)
{
  UNUSED_VAR(attrib);
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_osa_x86_mutex));
    *lock = (sns_osa_lock*)mutex;
    return sns_osa_lock_init(attrib, (sns_osa_lock*)mutex);
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to create sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc sns_osa_lock_init(const sns_osa_lock_attr *attrib, sns_osa_lock *lock)
{
  UNUSED_VAR(attrib);
  if(NULL != lock)
  {
    pthread_mutexattr_t mutex_attrib;
    pthread_mutexattr_init(&mutex_attrib);
    pthread_mutexattr_settype(&mutex_attrib, PTHREAD_MUTEX_RECURSIVE);

    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex *) lock;
    pthread_mutex_init(&mutex->mutex, &mutex_attrib);
    pthread_mutexattr_destroy(&mutex_attrib);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to init sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_lock_delete(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex*)lock;
    sns_osa_lock_deinit(lock);
    sns_free(mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to delete sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_lock_deinit(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex*)lock;
    pthread_mutex_destroy(&mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to deinit sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_lock_acquire(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex*)lock;
    pthread_mutex_lock(&mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to acquire sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_lock_try_acquire(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex*)lock;
    int err = pthread_mutex_trylock(&mutex->mutex);
    if(0 == err)
      return SNS_RC_SUCCESS;
    else
      return SNS_RC_NOT_AVAILABLE;
  }
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_lock_release(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_x86_mutex *mutex = (sns_osa_x86_mutex*)lock;
    pthread_mutex_unlock(&mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to release sns_osa_lock");
  return SNS_RC_NOT_SUPPORTED;
}
