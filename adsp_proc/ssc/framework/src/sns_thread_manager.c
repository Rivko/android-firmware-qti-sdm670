/*=============================================================================
  @file sns_thread_manager.c

  Most work in the SEE is performed by several groups of worker threads.  They
  handle the processing of request messages incoming to a Sensor, as well as
  the processing of event messages sent to a Sensor or Sensor Instance.

  There are three pools of worker threads, one for use within Island Mode,
  another group for while operating outside of Island Mode, and one more whose
  sole purpose is to handle high-priority tasks.  All threads are created
  during boot-up.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdio.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_stream_service.h"
#include "sns_island.h"
#include "sns_list.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_sem.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_rc.h"
#include "sns_thread_manager.h"
#include "sns_thread_prio.h"
#include "sns_timer.h"
#include "sns_types.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#define SNS_THREAD_STACK_LEN 4096

#define SNS_TASK_LEN 10

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* A set of pending tasks.  Completed tasks will be noted with a NULL
 * library; unused slots have a NULL func.
 */
typedef struct sns_tmgr_task_arr
{
  sns_list_item list_entry;
  struct {
    /* Library whose lock to acquire before calling function;
     * if non-NULL, task is still pending. */
    struct sns_sensor_library *library;
    /* Function to call to handle this task; if non-NULL, this task has
     * completed*/
    sns_thread_mgr_task_func func;
    /* Argument to pass into function */
    void *func_arg;
  } tasks[SNS_TASK_LEN];
  /* Write index into the tasks array */
  uint_fast8_t write_idx;
} sns_tmgr_task_arr;

/* A particular worker thread */
typedef struct sns_worker_thread
{
  sns_osa_thread *thread;
  struct sns_thread_pool *pool;
  /* Will point to the semaphore in a sns_thread_pool::sem */
  sns_osa_sem *sem_wait;
  /* Stack space used by this thread */
  uint8_t *stack;
  /* Flag to signal thread to exit */
  bool exit;

  struct
  {
    sns_time total_time;  /* Total (active + idle) time */
    sns_time active_time; /* Active time: used for thread utilization calculation */
  } power_info;

  sns_time start_time;
  sns_time prev_start_time;
  sns_time start_active_time;
  uint32_t thread_utilization;

  /* Library this worker thread is processing; must hold pending_tasks_lock */
  sns_sensor_library *library;
  /* stream or request this thread is working on */
  void* func_arg;
  /* points to entry point function of the current task */
  sns_thread_mgr_task_func func;
} sns_worker_thread;

/* A pool of worker threads, assigned to a particular SNS_OSA_MEM_TYPE. */
typedef struct sns_thread_pool
{
  /* Name of this thread pool */
  char name;
  /* Heap on which this thread's stack is allocated */
  sns_osa_mem_type type;
  /* How much memory to allocate for each thread's stack */
  uint32_t stack_size;
  /* Default thread priority to register with system */
  uint32_t priority;

  /* Semaphore all threads in this pool wait upon */
   sns_osa_sem *sem;
  /* Array of worker threads assigned to this pool */
  sns_worker_thread *threads;
  /* Length of threads array */
  int threads_len;

  /* Protects following fields */
  sns_osa_lock *pending_tasks_lock;
  /* Quantity of non-island entries on pending_tasks queue. */
  unsigned int block_island_cnt;
  /* See sns_tmgr_task_arr */
  sns_list pending_tasks;

  sns_island_client_handle island_client;
} sns_thread_pool;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

static sns_thread_pool thread_pools[] SNS_SECTION(".data.sns") =
  {{ .name = 'P', .type = SNS_OSA_MEM_TYPE_ISLAND, .priority = SNS_THREAD_PRIO_HIGH, .threads_len = SNS_PRIO_HIGH_ISLAND_THREAD_LENGTH, .stack_size = SNS_THREAD_STACK_LEN },
   { .name = 'I', .type = SNS_OSA_MEM_TYPE_ISLAND, .priority = SNS_THREAD_PRIO_LOW, .threads_len = SNS_PRIO_LOW_ISLAND_THREAD_LENGTH, .stack_size = SNS_THREAD_STACK_LEN }};

/* Controls whether worker threads will handle further tasks;
 * simply an optimization over using the mutex alone. */
static _Atomic unsigned int threads_disabled SNS_SECTION(".data.sns");
/* Synchronization for worker threads waiting for low mem completion */
static sns_osa_lock *threads_disable_lock SNS_SECTION(".data.sns");

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Wait (block) until the block has been lifted.
 */
SNS_SECTION(".text.sns") static void
threads_enabled_wait(void)
{
  if(atomic_load(&threads_disabled))
  {
    sns_osa_lock_acquire(threads_disable_lock);
    sns_osa_lock_release(threads_disable_lock);
  }
}

/**
 * Check whether the given library is in use by any of the worker threads
 * associated with the given thread pool.
 *
 * PEND: Strongly consider searching in-use libraries of all worker threads,
 * in order to better utilize worker threads.
 */
SNS_SECTION(".text.sns") static bool
library_in_use(sns_thread_pool const *pool, void const *library)
{
  for(int i = 0; i < pool->threads_len; i++)
    if(library == pool->threads[i].library)
      return true;

  return false;
}

/**
 * Get the next available task for the worker thread.
 *
 * @note Must hold pool->pending_tasks_lock while calling this function.
 *
 * @param[i] thread Who will be running the found task
 * @param[o] func Registered callback function
 * @param[o] func_arg Optional callback function argument
 */
SNS_SECTION(".text.sns") static void
get_task(sns_worker_thread *thread, sns_thread_mgr_task_func *func,
    void **func_arg)
{
  sns_list_iter iter;
  sns_thread_pool *pool = thread->pool;

  *func = NULL;
  *func_arg = NULL;

  sns_tmgr_task_arr *task = sns_list_item_get_data(
      sns_list_iter_init(&iter, &pool->pending_tasks, true));
  while(NULL != task)
  {
    bool task_empty = true;
    for(int i = 0; i < task->write_idx; i++)
    {
      if(NULL != task->tasks[i].library)
      {
        task_empty = false;
        if(!library_in_use(pool, task->tasks[i].library))
        {
          thread->library  = task->tasks[i].library;
          thread->func_arg = task->tasks[i].func_arg;
          thread->func     = task->tasks[i].func;

          if(!sns_island_is_island_ptr((intptr_t)task->tasks[i].library))
            SNS_ISLAND_EXIT();

          *func = task->tasks[i].func;
          *func_arg = task->tasks[i].func_arg;
          task->tasks[i].library = NULL;

          task = NULL;
          return;
        }
      }
    }

    if(task_empty)
    {
      if(1 == sns_list_iter_len(&iter) &&
         sns_island_is_island_ptr((intptr_t)task))
      {
        sns_memzero(task->tasks, sizeof(task->tasks));
        task->write_idx = 0;
        break;
      }
      else
      {
        sns_list_iter_remove(&iter);
        sns_free(task);

        if(!sns_island_is_island_ptr((intptr_t)task) &&
           0 == --pool->block_island_cnt)
          sns_island_unblock(pool->island_client);

        task = sns_list_iter_get_curr_data(&iter);
      }
    }
    else
    {
      task = sns_list_item_get_data(sns_list_iter_advance(&iter));
    }
  }
}

/**
 * Attempt to process all pending tasks, using the given thread.  Will only
 * return once all tasks are handled, or are blocked by another thread.
 */
SNS_SECTION(".text.sns") static void
process_pending_tasks(sns_worker_thread *thread)
{
  sns_thread_pool *pool = thread->pool;
  sns_thread_mgr_task_func func = NULL;
  void *func_arg = NULL;

  for(;;)
  {
    threads_enabled_wait();

    sns_osa_lock_acquire(pool->pending_tasks_lock);
    get_task(thread, &func, &func_arg);
    sns_osa_lock_release(pool->pending_tasks_lock);

    if(NULL == func)
      break;

    sns_osa_lock_acquire(thread->library->library_lock);
    func(func_arg);
    sns_osa_lock_release(thread->library->library_lock);

    sns_osa_lock_acquire(pool->pending_tasks_lock);
    thread->library = NULL;
    thread->func = NULL;
    thread->func_arg = NULL;
    sns_osa_lock_release(pool->pending_tasks_lock);
  }
}

/**
 * Compute per-thread utilization; used to determine appropriate frequency
 * voting.
 */
SNS_SECTION(".text.sns") static void
compute_utilization(sns_worker_thread *thread)
{
  sns_time thread_util_sampling_period =
    sns_convert_ns_to_ticks(SNS_THREAD_UTIL_SAMPLING_PERIOD_NS);

  thread->prev_start_time = thread->start_time;
  thread->start_time = sns_get_system_time();
  if(0 != thread->start_active_time)
  {
    thread->power_info.total_time +=
      (thread->start_time - thread->prev_start_time);
    thread->power_info.active_time +=
      (thread->start_time - thread->start_active_time);
    if(thread->power_info.total_time > thread_util_sampling_period)
    {
      thread->thread_utilization = (uint16_t)((thread->power_info.active_time * 100) /
          thread->power_info.total_time);
      sns_pwr_scpm_mgr_update_thread_utilization(thread->thread, thread->thread_utilization);
      thread->power_info.total_time = 0;
      thread->power_info.active_time = 0;
    }
  }
}

/**
 * Main thread loop for all worker threads.
 */
SNS_SECTION(".text.sns") static void*
main_loop(void *arg)
{
  sns_worker_thread *thread = (sns_worker_thread*)arg;
  sns_osa_thread_active();

  thread->start_time = 0;
  thread->prev_start_time = 0;
  thread->start_active_time = 0;
  thread->thread_utilization = 0;

  for(;;)
  {
    compute_utilization(thread);

    sns_osa_thread_idle();
    sns_osa_sem_wait(thread->sem_wait);
    sns_osa_thread_active();

    thread->start_active_time = sns_get_system_time();

    if(thread->exit)
      break;

    process_pending_tasks(thread);

    // PEND: Check if *all* pending tasks lists are empty; if yes, check whether
    // entries exist on the closing_libraries list
  }

  SNS_ISLAND_EXIT();
  SNS_PRINTF(LOW, sns_fw_printf, "Thread " SNS_DIAG_PTR " exiting", thread);
  sns_osa_thread_idle();
  return (void*)0;
}

static void
sns_worker_deinit(sns_thread_pool *pool)
{
  for(int i = 0; i < pool->threads_len; i++)
  {
    pool->threads[i].exit = true;
    sns_osa_sem_post(pool->threads[i].sem_wait);
  }
  for(int i = 0; i < pool->threads_len; i++)
  {
    if(NULL != pool->threads[i].thread)
      sns_osa_thread_delete(pool->threads[i].thread);

    if(NULL != pool->threads[i].stack)
      sns_free(pool->threads[i].stack);
  }
  if(NULL != pool->sem)
    sns_osa_sem_delete(pool->sem);
}

static sns_rc
sns_worker_init(sns_thread_pool *pool, sns_osa_thread_attr *attr)
{
  sns_osa_sem_attr sem_attr;
  sns_osa_lock_attr lock_attr;
  sns_rc rc;
  sns_mem_heap_id heap = SNS_OSA_MEM_TYPE_ISLAND == pool->type
      ? SNS_HEAP_ISLAND : SNS_HEAP_MAIN;

  rc = sns_osa_sem_attr_init(&sem_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_sem_attr_set_memory_partition(&sem_attr, pool->type);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_sem_create(&sem_attr, &pool->sem);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_lock_attr_init(&lock_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&lock_attr, pool->type);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&lock_attr, &pool->pending_tasks_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  pool->block_island_cnt = 0;
  pool->threads = sns_malloc(heap, pool->threads_len * sizeof(*pool->threads));
  if(NULL == pool->threads)
    return SNS_RC_FAILED;

  sns_memset(pool->threads, 0, pool->threads_len * sizeof(*pool->threads));
  sns_list_init(&pool->pending_tasks);

  pool->island_client = sns_island_aggregator_register_client(&pool->name);

  for(int i = 0; i < pool->threads_len; i++)
  {
    char name[16];

    pool->threads[i].pool = pool;
    pool->threads[i].exit = false;
    pool->threads[i].sem_wait = pool->sem;

    pool->threads[i].stack = sns_malloc(heap, pool->stack_size);
    if(NULL == pool->threads[i].stack)
      return SNS_RC_FAILED;

    snprintf(name, sizeof(name), "SNS_SEE_%c_%d", pool->name, i);

    sns_osa_thread_attr_set_stack(attr,
        (uintptr_t)pool->threads[i].stack, pool->stack_size);
    sns_osa_thread_attr_set_priority(attr, pool->priority);
    sns_osa_thread_attr_set_memory_partition(attr, pool->type);
    sns_osa_thread_attr_set_name(attr, name);
    sns_osa_thread_attr_set_user_info(attr, &pool->threads[i]);

    rc = sns_osa_thread_create((sns_osa_thread_func)main_loop,
        &pool->threads[i], attr, &pool->threads[i].thread);
    if(SNS_RC_SUCCESS != rc)
      return rc;
  }
  return rc;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") void
sns_thread_manager_add(struct sns_sensor_library *library,
    sns_thread_mgr_task_func func, void *func_arg, bool priority)
{
  sns_list_iter iter;
  sns_tmgr_task_arr *task;
  sns_list_item *task_item = NULL;
  sns_thread_pool *pool = &thread_pools[priority ? 0 : 1];

  sns_osa_lock_acquire(pool->pending_tasks_lock);
  task_item = sns_list_iter_init(&iter, &pool->pending_tasks, false);
  task = (sns_tmgr_task_arr*)sns_list_item_get_data(task_item);
  if(NULL == task || SNS_TASK_LEN - 1 == task->write_idx)
  {
    task = sns_malloc(SNS_HEAP_ISLAND, sizeof(*task));
    if(NULL == task)
    {
      if(0 == pool->block_island_cnt++)
        sns_island_block(pool->island_client);
      task = sns_malloc(SNS_HEAP_MAIN, sizeof(*task));
    }
    SNS_ASSERT(NULL != task);

    sns_list_item_init(&task->list_entry, task);
    sns_list_iter_insert(&iter, &task->list_entry, true);
  }

  task->tasks[task->write_idx].library = library;
  task->tasks[task->write_idx].func = func;
  task->tasks[task->write_idx].func_arg = func_arg;
  task->write_idx++;
  sns_osa_lock_release(pool->pending_tasks_lock);

  if(sns_osa_sem_get_value(pool->sem) < pool->threads_len * 2)
    sns_osa_sem_post(pool->sem);
}

void
sns_thread_manager_deinit(void)
{
  for(int i = 0; i < ARR_SIZE(thread_pools); i++)
    sns_worker_deinit(&thread_pools[i]);
}

sns_rc
sns_thread_manager_init(void)
{
  sns_osa_thread_attr thread_attr;
  sns_osa_lock_attr lock_attr;
  sns_rc rc;

  rc = sns_osa_lock_attr_init(&lock_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&lock_attr, &threads_disable_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_thread_attr_init(&thread_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  for(int i = 0; i < ARR_SIZE(thread_pools); i++)
  {
    rc = sns_worker_init(&thread_pools[i], &thread_attr);
    if(SNS_RC_SUCCESS != rc)
    {
      sns_thread_manager_deinit();
      return rc;
    }
  }

  sns_thread_manager_disable(true);
  return rc;
}

void
sns_thread_manager_disable(bool status)
{
  atomic_store(&threads_disabled, (unsigned int)status);
  if(status)
    sns_osa_lock_acquire(threads_disable_lock);
  else
    sns_osa_lock_release(threads_disable_lock);
}

SNS_SECTION(".text.sns") void
sns_thread_manager_remove_pending_events(void *func_arg)
{
  for(int i = 0; i < ARR_SIZE(thread_pools); i++)
  {
    sns_thread_pool *pool = &thread_pools[i];
    sns_list_iter iter;
    sns_osa_lock_acquire(pool->pending_tasks_lock);
    sns_tmgr_task_arr *task = sns_list_item_get_data(
        sns_list_iter_init(&iter, &pool->pending_tasks, true));

    while(NULL != task)
    {
      for(int j = 0; j < task->write_idx; j++)
      {
        if((NULL != task->tasks[j].library) &&
            (task->tasks[j].func_arg == func_arg))
        {
          task->tasks[j].library = NULL;
        }
      }
      task = sns_list_item_get_data(sns_list_iter_advance(&iter));
    }
    sns_osa_lock_release(pool->pending_tasks_lock);
  }
}
