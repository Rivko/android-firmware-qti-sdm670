/*=============================================================================
  @file sns_signal.c

  Provides internal signal service that can spawn an internal thread to handle
  asynchronus events

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "sns_assert.h"
#include "sns_fw_sensor.h"
#include "sns_island.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_signal.h"
#include "sns_time.h"


/*=============================================================================
  Macros
  ===========================================================================*/

#define SNS_SIG_SVC_STACK_SIZE 2*1024

/*=============================================================================
  Static Functions
  ===========================================================================*/

/**
 * Main loop of threads spawned by the internal signal service.
 * Each thread is "owned" by a single sensor
 *
 * @param[i] args    The state of this thread that is passed in
 *                   by sns_osa_thread_create
 */

SNS_SECTION(".text.sns") static void
sns_signal_task(void * args)
{
  sns_signal_thread *sig_thread = (sns_signal_thread *)args;
  uint32_t signal;
  uint32_t current_signal;
  sns_osa_thread_active();

  while(sig_thread->active)
  {
    signal = 0;
    if (NULL != sig_thread->island_client)
    {
      sns_island_unblock(sig_thread->island_client);
    }
    sns_osa_thread_idle();
    sns_osa_thread_sigs_wait(0xFFFFFFFF, &signal);
    sns_osa_thread_active();
    if (NULL != sig_thread->island_client)
    {
      sns_island_block(sig_thread->island_client);
    }

    sns_osa_lock_acquire(((sns_fw_sensor *)sig_thread->sensor)->library->library_lock);
    //SNS_PRINTF(HIGH, sns_fw_printf, "Signalled %x", signal);
    current_signal = 0x1;
    for(uint32_t i = 0; (i < SNS_SIGNAL_MAX_SIGNALS) && (0 != signal);
      i++, current_signal = current_signal << 1)
    {
      if((signal & current_signal))
      {
        if(sig_thread->registered[i])
        {
          //SNS_PRINTF(HIGH, sns_fw_printf, "Notifying of signal %x & %x", signal, current_signal);
          sig_thread->cb(sig_thread->sensor, current_signal, sig_thread->user_args[i]);
        }
        signal ^= current_signal;
      }
    }

    sns_osa_lock_release(((sns_fw_sensor *)sig_thread->sensor)->library->library_lock);
  }
  sns_osa_thread_idle();
}

/*=============================================================================
  Public Functions
  ===========================================================================*/
sns_signal_thread*
sns_signal_thread_create(struct sns_sensor *sensor,
                         char const *thread_name,
                         uint8_t thread_priority,
                         sns_handle_signal_event cb,
                         sns_signal_task_ptr sig_task_fn,
                         bool island_thread)
{
  sns_signal_thread *sig_thread = NULL;
  sns_osa_thread_attr attr;
  sns_rc rc = SNS_RC_SUCCESS;

  if(island_thread)
  {
    sig_thread = sns_malloc(SNS_HEAP_ISLAND, sizeof(sns_signal_thread));
  }
  else
  {
    sig_thread = sns_malloc(SNS_HEAP_MAIN, sizeof(sns_signal_thread));
  }
  SNS_ASSERT(NULL != sig_thread);

  sig_thread->island_client = NULL;
  sig_thread->sensor = sensor;
  sig_thread->active = true;
  sig_thread->cb = cb;
  sig_thread->thread = NULL;
  for(uint32_t i = 0; i < SNS_SIGNAL_MAX_SIGNALS; i++)
  {
    sig_thread->user_args[i] = NULL;
    sig_thread->registered[i] = false;
  }

  SNS_ISLAND_EXIT();

  rc |= sns_osa_thread_attr_init(&attr);
  rc |= sns_osa_thread_attr_set_name(&attr, thread_name);

  if(island_thread)
  {
    rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  }
  else
  {
    sig_thread->island_client = sns_island_aggregator_register_client(thread_name);
    rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_NORMAL);
  }
  rc |= sns_osa_thread_attr_set_stack(&attr, (uintptr_t)NULL, SNS_SIG_SVC_STACK_SIZE);
  rc |= sns_osa_thread_attr_set_priority(&attr, thread_priority);

  if(NULL == sig_task_fn)
  {
    rc |= sns_osa_thread_create(sns_signal_task, sig_thread, &attr, &sig_thread->thread);
  }
  else
  {
    rc |= sns_osa_thread_create(sig_task_fn, sig_thread, &attr, &sig_thread->thread);
  }
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  return sig_thread;
}

sns_rc
sns_signal_thread_destroy(sns_signal_thread *thread)
{
  sns_rc rc = SNS_RC_FAILED;

  if(NULL != thread)
  {
    SNS_ISLAND_EXIT();
    sns_memzero(thread, sizeof(thread));
    // Force a thread out of idle prior to deleting
    sns_osa_thread_sigs_set(thread->thread, 0x01);
    sns_osa_thread_delete(thread->thread);
    sns_free(thread);
    rc = SNS_RC_SUCCESS;
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Invalid thread handle: " SNS_DIAG_PTR, thread);
  }

  return rc;
}

SNS_SECTION(".text.sns") sns_rc
sns_signal_register_signal(sns_signal_thread *thread,
                                    void *user_arg,
                                    uint32_t *new_signal)
{
  sns_rc rc = SNS_RC_FAILED;

  if(NULL != thread)
  {
    uint32_t i;
    for(i = 0; i < SNS_SIGNAL_MAX_SIGNALS; i++)
    {
      if(!thread->registered[i])
      {
        thread->registered[i] = true;
        thread->user_args[i] = user_arg;
        if(NULL != new_signal)
        {
          *new_signal = 0x1UL << i;
        }
        rc = SNS_RC_SUCCESS;
        break;
      }
    }
    if(SNS_SIGNAL_MAX_SIGNALS == i)
    {
      // No more signals can be registered on this task.
      // Create a new task and register for a signal on that
      // task
      rc = SNS_RC_NOT_AVAILABLE;
    }
  }
  return rc;
}

SNS_SECTION(".text.sns") sns_rc
sns_signal_unregister_signal(sns_signal_thread *thread, uint32_t signal)
{
 if(NULL != thread)
  {
    for(uint32_t i = 0; i < SNS_SIGNAL_MAX_SIGNALS ;  i++)
    {
      if(signal == (0x1UL << i))
      {
        thread->registered[i] = false;
        thread->user_args[i] = NULL;
        return SNS_RC_SUCCESS;
      }
    }
  }
  SNS_PRINTF(ERROR, sns_fw_printf, "Invalid operation - thread handle: "
      SNS_DIAG_PTR " signal : %u", thread, signal );

  return SNS_RC_FAILED;
}
