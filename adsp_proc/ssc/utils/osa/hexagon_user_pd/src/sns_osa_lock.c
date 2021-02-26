/*============================================================================
  @file sns_osa_lock.c

  @brief
  OS Abstraction layer for Sensors.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include "qurt.h"
#include "qurt_rmutex.h"
#include "sns_assert.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_rc.h"

/*============================================================================
  Typedefs
  ============================================================================*/

/* Internal structure for implementing a mutex for QuRT */
typedef struct
{
  // QuRT mutex handle
  qurt_mutex_t mutex;
  void *lock_holder;
  uint32_t thread_id;
  sns_time lock_timestamp;
} sns_osa_qurt_mutex;

/* Internal structure for implementing mutex attributes for QuRT */
typedef struct
{
  // Determines where the memory is malloc'd for this mutex
  sns_osa_mem_type mem_type;
} sns_osa_qurt_mutex_attr;

/*============================================================================
  Public   Functions
  ============================================================================*/
SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_attr_init(sns_osa_lock_attr *attrib)
{
  _Static_assert(sizeof(sns_osa_lock_attr) >= sizeof(sns_osa_qurt_mutex_attr),
    "__SIZEOF_ATTR_LOCK is smaller than sizeof(sns_osa_qurt_mutex_attr)");
  _Static_assert(sizeof(sns_osa_lock) >= sizeof(sns_osa_qurt_mutex),
  "__SIZEOF_LOCK is smaller than sizeof(sns_osa_lock)");
  if(NULL != attrib)
  {
    sns_osa_qurt_mutex_attr* attr = (sns_osa_qurt_mutex_attr* ) attrib;
    attr->mem_type = SNS_OSA_MEM_TYPE_NORMAL;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to init osa_lock attribute");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_attr_set_memory_partition(
  sns_osa_lock_attr      *attrib,
  sns_osa_mem_type       mem_type)
{
  if(NULL != attrib)
  {
    sns_osa_qurt_mutex_attr *qurt_mutex_attr = (sns_osa_qurt_mutex_attr*)attrib;
    qurt_mutex_attr->mem_type = mem_type;
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set mem_type attribute for sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_create(sns_osa_lock_attr const *attrib, sns_osa_lock **lock)
{
  sns_rc rv = SNS_RC_INVALID_VALUE;

  if(NULL != lock && NULL != attrib)
  {
    *lock = NULL;
    sns_osa_qurt_mutex_attr *qurt_mutex_attr = (sns_osa_qurt_mutex_attr*)attrib;
    sns_osa_qurt_mutex *new_mutex = NULL;
    sns_mem_heap_id id = qurt_mutex_attr->mem_type == SNS_OSA_MEM_TYPE_NORMAL
      ? SNS_HEAP_MAIN : SNS_HEAP_ISLAND;

    new_mutex = sns_malloc(id, sizeof(*new_mutex));
    if(NULL == new_mutex)
    {
      rv = SNS_RC_NOT_AVAILABLE;
    }
    else
    {
      *lock = (sns_osa_lock*)new_mutex;
      rv = sns_osa_lock_init(attrib, (sns_osa_lock*)new_mutex);
    }
  }

  if(SNS_RC_SUCCESS != rv)
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to create sns_osa_lock %i", rv);

  return rv;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_init(const sns_osa_lock_attr *attrib, sns_osa_lock *lock)
{
  if(NULL != attrib && NULL != lock)
  {
    sns_osa_qurt_mutex *new_mutex = (sns_osa_qurt_mutex *)lock;
    qurt_rmutex_init(&new_mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to init sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_delete(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_qurt_mutex *qurt_mutex = (sns_osa_qurt_mutex*)lock;
    sns_osa_lock_deinit(lock);
    sns_free((void*)qurt_mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to delete sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_deinit(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_qurt_mutex *qurt_mutex = (sns_osa_qurt_mutex*)lock;
    qurt_rmutex_destroy(&qurt_mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to deinit sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_acquire(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_qurt_mutex *qurt_mutex = (sns_osa_qurt_mutex*)lock;
    qurt_rmutex_lock(&qurt_mutex->mutex);
    qurt_mutex->lock_holder = __builtin_return_address(1);
    qurt_mutex->thread_id = sns_osa_thread_get_thread_id();
    qurt_mutex->lock_timestamp = sns_get_system_time();
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to acquire sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_release(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_qurt_mutex *qurt_mutex = (sns_osa_qurt_mutex*)lock;
    qurt_rmutex_unlock(&qurt_mutex->mutex);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to release sns_osa_lock.");
  return SNS_RC_NOT_SUPPORTED;
}

SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_lock_try_acquire(sns_osa_lock *lock)
{
  if(NULL != lock)
  {
    sns_osa_qurt_mutex *qurt_mutex = (sns_osa_qurt_mutex*)lock;
    if(0 == qurt_rmutex_try_lock(&qurt_mutex->mutex))
    {
      qurt_mutex->lock_holder = __builtin_return_address(1);
      qurt_mutex->thread_id = sns_osa_thread_get_thread_id();
      qurt_mutex->lock_timestamp = sns_get_system_time();
      return SNS_RC_SUCCESS;
    }
    else
      return SNS_RC_NOT_AVAILABLE;
  }
  return SNS_RC_NOT_SUPPORTED;
}

