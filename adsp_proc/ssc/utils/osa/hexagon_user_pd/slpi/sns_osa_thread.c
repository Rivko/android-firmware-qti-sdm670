/*============================================================================
  @file sns_osa_thread.c

  @brief
  OS Abstraction layer for Sensors.

  PEND: Common functionality between x86 and Hexagon versions should extracted.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include "qurt.h"
#include "qurt_anysignal.h"
#include "qurt_atomic_ops.h"

#include "sns_assert.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_list.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_rc.h"

/*============================================================================
  Macros
  ============================================================================*/

#define SNS_DEFAULT_THREAD_PRIO 0xec

/*============================================================================
  Typedefs
  ============================================================================*/

typedef struct
{
  /* List item. All qurt threads are maintained in sns_osa_threads */
  sns_list_item list_entry;
  /* Thread ID. This is generated by QuRT. */
  qurt_thread_t thread_id;
  /* QuRT signals for this thread */
  qurt_signal_t signals;
  /* Stack memory if stack allocated from heap */
  uintptr_t stack;
  /* Stack allocated from heap */
  bool stack_alloc_from_heap;
  /* Lock to synchronize thread creation.*/
  sns_osa_lock red_light;
  /* User provided entry function */
  sns_osa_thread_func         thread_func;
  /* Args to user provided entry function */
  sns_osa_thread_func_arg     thread_func_arg;
  /* User information */
  void *info;
} sns_osa_thread_qurt;

typedef struct
{
  qurt_thread_attr_t qurt_attrib;
  bool defer_stack_alloc;
  sns_osa_mem_type mem_type;
  size_t stack_size;
  void *info;
}sns_osa_thread_qurt_attr;

/*============================================================================
  static Data
  ============================================================================*/

/* A list of all threads. Used to identify current thread */
static struct sns_list sns_osa_threads SNS_SECTION(".data.sns_osa");
static sns_osa_lock *sns_osa_threads_lock SNS_SECTION(".data.sns_osa");
static unsigned int active_threads_count  SNS_SECTION(".data.sns_osa") = 0;
static sns_osa_lock *sns_osa_threads_count_lock SNS_SECTION(".data.sns_osa");

/*============================================================================
  Extern functions
  ============================================================================*/

/*============================================================================
  static Functions
  ============================================================================*/

/*!
  @brief Returns the current thread

  @return
  thread pointer if thread was found in the sns_osa_threads list
  NULL otherwise
*/

SNS_SECTION(".text.sns_osa") static sns_osa_thread_qurt*
sns_osa_get_current_thread(void)
{
  qurt_thread_t thread_id = qurt_thread_get_id();
  sns_list_iter iter;
  sns_osa_thread_qurt *rv = NULL;
  sns_list_item *curr_thread = NULL;

  //sns_osa_lock_acquire(sns_osa_threads_lock);
  for(curr_thread = sns_list_iter_init(&iter, &sns_osa_threads, true);
      NULL != curr_thread;
      curr_thread = sns_list_iter_advance(&iter))
  {
    sns_osa_thread_qurt *qurt_thread = (sns_osa_thread_qurt*)
    sns_list_item_get_data(curr_thread);
    if(thread_id == qurt_thread->thread_id)
    {
      rv = qurt_thread;
      break;
    }
  }
  //sns_osa_lock_release(sns_osa_threads_lock);

  return rv;
}

/*!
   @brief Suspends a newly created thread until the parent has finished
   initializing data structures used by sns_osa_threads list.
   When the parent has completed the initialization, this function calls the user
   provided thread entry function.

   @param[i] arg     Newly created thread information
*/
void sns_osa_thread_launch(void *arg)
{
  sns_osa_thread_qurt *thread = (sns_osa_thread_qurt *)arg;
  SNS_ASSERT(NULL != thread);

  // Wait till the parent has finished creating this thread.
  sns_osa_lock_acquire(&thread->red_light);

  // Verify thread init has completed.
  SNS_ASSERT(thread->thread_id == qurt_thread_get_id());
  SNS_ASSERT(sns_list_item_present(&thread->list_entry));
  SNS_ASSERT(NULL != sns_osa_get_current_thread());

  // Release the mutex since it is no longer needed.
  sns_osa_lock_release(&thread->red_light);

  // Clean up this mutex. It's no longer used.
  sns_osa_lock_deinit(&thread->red_light);

  // Enter the task's main loop
  if (NULL != thread->thread_func)
  {
    thread->thread_func(thread->thread_func_arg);
  }

  // Thread clean up happens in
  // sns_osa_thread_delete()
}

/*============================================================================
  Public   Functions
  ============================================================================*/

sns_rc
sns_osa_thread_attr_init(sns_osa_thread_attr *attrib)
{
  _Static_assert(sizeof(sns_osa_thread_attr) >= sizeof(sns_osa_thread_qurt_attr),
    "__SIZEOF_ATTR_THREAD is smaller than sizeof(sns_osa_thread_qurt_attr)");

  if(NULL != attrib)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib = (sns_osa_thread_qurt_attr*)attrib;
    qurt_thread_attr_init(&osa_thread_attrib->qurt_attrib);
    osa_thread_attrib->defer_stack_alloc = false;

    qurt_thread_attr_set_priority(&osa_thread_attrib->qurt_attrib,
        SNS_DEFAULT_THREAD_PRIO);

    return SNS_RC_SUCCESS;
  }

  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to init osa_thread attribute");
  return SNS_RC_NOT_SUPPORTED;
}


sns_rc
sns_osa_thread_attr_set_stack(
  sns_osa_thread_attr      *attrib,
  uintptr_t                 stack_start,
  size_t                    stack_size)
{
  if(NULL != attrib)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib =
      (sns_osa_thread_qurt_attr*)attrib;
    qurt_thread_attr_set_stack_size(&osa_thread_attrib->qurt_attrib, stack_size);
    if(NULL != stack_start)
    {
      qurt_thread_attr_set_stack_addr(&osa_thread_attrib->qurt_attrib,
          (void*)stack_start);
    }
    else
    {
      osa_thread_attrib->defer_stack_alloc = true;
      osa_thread_attrib->stack_size = stack_size;
    }
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set osa_thread stack attribute");
  return SNS_RC_NOT_SUPPORTED;
}


sns_rc
sns_osa_thread_attr_set_priority(
  sns_osa_thread_attr      *attrib,
  uint8_t                   priority)
{
  if(0 < priority)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib =
      (sns_osa_thread_qurt_attr*)attrib;
    qurt_thread_attr_set_priority(&osa_thread_attrib->qurt_attrib, priority);
    return SNS_RC_SUCCESS;
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set osa_thread priority attribute");
    return SNS_RC_INVALID_VALUE;
  }
}


sns_rc sns_osa_thread_attr_set_name(
  sns_osa_thread_attr      *attrib,
  const char               *name)
{
  if(NULL != attrib && NULL != name)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib = (sns_osa_thread_qurt_attr*)attrib;
    qurt_thread_attr_set_name(&osa_thread_attrib->qurt_attrib, (char*)name);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set osa_thread name attribute");
  return SNS_RC_NOT_SUPPORTED;
}


sns_rc
sns_osa_thread_attr_set_memory_partition(
  sns_osa_thread_attr      *attrib,
  sns_osa_mem_type       mem_type)
{
  if(NULL != attrib)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib = (sns_osa_thread_qurt_attr*)attrib;
    osa_thread_attrib->mem_type = mem_type;
    if(SNS_OSA_MEM_TYPE_ISLAND == mem_type)
    {
      qurt_thread_attr_set_tcb_partition(&osa_thread_attrib->qurt_attrib,  1);
    }
    else
    {
      qurt_thread_attr_set_tcb_partition(&osa_thread_attrib->qurt_attrib, 0);
    }
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set osa_thread mem_type attribute");
  return SNS_RC_NOT_SUPPORTED;
}


sns_rc
sns_osa_thread_attr_set_user_info(
  sns_osa_thread_attr    *attrib,
  void                   *info)
{
  if(NULL != attrib)
  {
    sns_osa_thread_qurt_attr *osa_thread_attrib = (sns_osa_thread_qurt_attr*)attrib;
    osa_thread_attrib->info = info;
    return SNS_RC_SUCCESS;
  }
  return SNS_RC_NOT_SUPPORTED;
}

sns_rc
sns_osa_thread_create(
  sns_osa_thread_func         thread_func,
  sns_osa_thread_func_arg     thread_func_arg,
  const sns_osa_thread_attr  *attrib,
  sns_osa_thread             **thread)
{
  SNS_ASSERT(NULL != thread_func);
  SNS_ASSERT(NULL != attrib);
  SNS_ASSERT(NULL != thread);

  sns_osa_thread_qurt_attr *osa_thread_attrib = (sns_osa_thread_qurt_attr*)attrib;
  sns_osa_thread_qurt *qurt_thread = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_osa_thread_qurt));
  sns_list_iter iter;
  int qurt_err;

  if(NULL != qurt_thread)
  {
    sns_osa_lock_attr lock_attr;
    sns_osa_lock_attr_init(&lock_attr);
    sns_osa_lock_attr_set_memory_partition(&lock_attr, osa_thread_attrib->mem_type);
    sns_osa_lock_init(&lock_attr, &qurt_thread->red_light);

    qurt_thread->thread_func = thread_func;
    qurt_thread->thread_func_arg = thread_func_arg;

    qurt_anysignal_init(&qurt_thread->signals);

    sns_list_item_init(&qurt_thread->list_entry, qurt_thread);
    qurt_thread->stack_alloc_from_heap = false;
    qurt_thread->info = osa_thread_attrib->info;
    *thread = (sns_osa_thread*)qurt_thread;

    if(osa_thread_attrib->defer_stack_alloc)
    {
      sns_mem_heap_id stack_mem_source =
        (SNS_OSA_MEM_TYPE_ISLAND == osa_thread_attrib->mem_type) ? SNS_HEAP_ISLAND:
        SNS_HEAP_MAIN;
      void *stack = sns_malloc(stack_mem_source, osa_thread_attrib->stack_size);
      SNS_ASSERT(NULL != stack);
      qurt_thread_attr_set_stack_addr(&osa_thread_attrib->qurt_attrib, stack);
      qurt_thread->stack_alloc_from_heap = true;
      qurt_thread->stack = (uintptr_t)stack;
    }

    sns_osa_lock_acquire(sns_osa_threads_lock);
    sns_list_iter_init(&iter, &sns_osa_threads, true);
    sns_list_iter_insert(&iter, &qurt_thread->list_entry, true);
    sns_osa_lock_release(sns_osa_threads_lock);

    // red_light stops the newly launched thread from
    // calling it's thread_func() before this context finishes
    // initializing the thread.
    sns_osa_lock_acquire(&qurt_thread->red_light);

    qurt_err = qurt_thread_create (
                 &qurt_thread->thread_id,
                 &osa_thread_attrib->qurt_attrib,
                 sns_osa_thread_launch,
                 (void *) qurt_thread);
    SNS_ASSERT(0 != qurt_thread->thread_id);

    // Unblock the newly launched thread
    sns_osa_lock_release(&qurt_thread->red_light);

    if(QURT_EOK == qurt_err)
    {
      SNS_PRINTF(HIGH, sns_fw_printf, "Created Thread with id %x", qurt_thread->thread_id);
      return SNS_RC_SUCCESS;
    }
  }
  *thread = NULL;
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set create thread");
  return SNS_RC_NOT_SUPPORTED;
}


sns_rc
sns_osa_thread_delete(sns_osa_thread *thread)
{
  int status;
  int qurt_err;

  if(NULL != thread)
  {
    sns_osa_thread_qurt *qurt_thread = (sns_osa_thread_qurt*)thread;
    qurt_err = qurt_thread_join(qurt_thread->thread_id, &status);
    if(QURT_EOK == qurt_err || QURT_ENOTHREAD == qurt_err)
    {
      sns_list_iter iter;

      sns_osa_lock_acquire(sns_osa_threads_lock);
      sns_list_iter_init(&iter, &sns_osa_threads, true);
      for(sns_list_iter_init(&iter, &sns_osa_threads, true);
          NULL != sns_list_iter_curr(&iter);
          sns_list_iter_advance(&iter))
      {
        if(qurt_thread == (sns_osa_thread_qurt*)
           sns_list_iter_get_curr_data(&iter))
        {
          sns_list_iter_remove(&iter);
        }
      }
      sns_osa_lock_release(sns_osa_threads_lock);

      if(qurt_thread->stack_alloc_from_heap)
      {
        sns_free((void*)qurt_thread->stack);
      }
      sns_free(qurt_thread);
      return SNS_RC_SUCCESS;
    }
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set delete thread");
  return SNS_RC_NOT_SUPPORTED;
}


SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_thread_sigs_wait(
  sns_osa_thread_sigs         sigs_mask,
  sns_osa_thread_sigs        *sigs_rcvd)
{
  sns_osa_thread_qurt *qurt_thread = sns_osa_get_current_thread();
  SNS_ASSERT(NULL != qurt_thread);
  qurt_anysignal_wait(&qurt_thread->signals, sigs_mask);
  if(NULL != sigs_rcvd)
  {
    *sigs_rcvd = qurt_anysignal_get(&qurt_thread->signals);
    qurt_anysignal_clear(&qurt_thread->signals, *sigs_rcvd);
  }
  return SNS_RC_SUCCESS;
}


SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_thread_sigs_set(sns_osa_thread *thread, sns_osa_thread_sigs sigs)
{
  if(NULL != thread)
  {
    sns_osa_thread_qurt *qurt_thread = (sns_osa_thread_qurt*)thread;
    SNS_ASSERT(NULL != qurt_thread);
    qurt_anysignal_set(&qurt_thread->signals, sigs);
    return SNS_RC_SUCCESS;
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Failed to set signal %d for thread", sigs);
  return SNS_RC_NOT_SUPPORTED;
}


SNS_SECTION(".text.sns_osa") sns_rc
sns_osa_thread_sigs_check(
  sns_osa_thread_sigs         sigs_mask,
  bool                        consume,
  sns_osa_thread_sigs        *sigs_rcvd)
{
  sns_osa_thread_qurt *qurt_thread = sns_osa_get_current_thread();
  SNS_ASSERT(NULL != qurt_thread);
  if(NULL != sigs_rcvd)
  {
    *sigs_rcvd = qurt_anysignal_get(&qurt_thread->signals) & sigs_mask;
    if(consume)
    {
      qurt_anysignal_clear(&qurt_thread->signals, *sigs_rcvd);
    }
  }
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns_osa") uint8_t
sns_osa_thread_get_priority(void)
{
  return qurt_thread_get_priority(qurt_thread_get_id());
}

SNS_SECTION(".text.sns_osa") uint8_t
sns_osa_thread_set_priority(uint8_t priority)
{
  uint8_t old_priority = sns_osa_thread_get_priority();
  if(0 < priority)
  {
    qurt_thread_set_priority(qurt_thread_get_id(), priority);
  }
  return old_priority;
}

SNS_SECTION(".text.sns_osa") void*
sns_osa_thread_get_user_info()
{
  sns_osa_thread_qurt *thread = sns_osa_get_current_thread();
  SNS_ASSERT(NULL != thread);
  return thread->info;
}

SNS_SECTION(".text.sns_osa") int
sns_osa_thread_get_thread_id()
{
  return (int)qurt_thread_get_id();
}

void
sns_osa_init(void)
{
  sns_osa_lock_attr attr;
  sns_rc rc;

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &sns_osa_threads_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&attr, &sns_osa_threads_count_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_list_init(&sns_osa_threads);
}

SNS_SECTION(".text.sns_osa") void
sns_osa_thread_active()
{
  sns_osa_lock_acquire(sns_osa_threads_count_lock);
  active_threads_count += 1;
  sns_osa_lock_release(sns_osa_threads_count_lock);
}

SNS_SECTION(".text.sns_osa") void
sns_osa_thread_idle()
{
  sns_osa_lock_acquire(sns_osa_threads_count_lock);
  active_threads_count -= 1;
  //SNS_PRINTF(HIGH, sns_fw_printf, "Active threads: %d", active_threads_count);
  if (0 == active_threads_count)
  {
    sns_island_enable();
  }
  sns_osa_lock_release(sns_osa_threads_count_lock);
}