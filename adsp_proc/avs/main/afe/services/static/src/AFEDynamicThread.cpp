/*========================================================================
   This file contains AFE dynamic thread launch API implementation.

  Copyright (c) 2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEDynamicThread.cpp#1 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "AFEDynamicThread.h"
#include "audio_basic_op_ext.h"
#include "AFEErrorHandler.h"

#define AFE_DYNM_THRD_KILL_TASK         0
#define AFE_DYNM_THRD_PROCESS_TASK      1

/* The thread waits for only kill or process signals on the channel
 * So, instead of waiting on all the channel bits, we wait on only these.
 */
#define AFE_DYNM_THRD_CHANNEL_WAIT_MASK ((1<<AFE_DYNM_THRD_KILL_TASK) | \
                                         (1<<AFE_DYNM_THRD_PROCESS_TASK))

/**
 * This is the work loop of a thread launched by dynamic thread.
 * @param[in]  vp_afe_dynamic_thread_ptr
 * @return the ADSP_EOK error code
 */
static int afe_dynamic_thread_work_loop(void *vp_afe_dynamic_thread)
{
  ADSPResult result            = ADSP_EOK;

  uint32_t   ch_status         = 0;
  uint32_t   task_index        = 0;
  uint32_t   isr_count_tracker = 0;
  uint32_t   signal_miss_count = 0;

  afe_dynamic_thread_t      *afe_dynamic_thread_ptr;
  qurt_elite_channel_t      *pChannel;
  afe_dynamic_thread_task_t task;
  volatile uint32_t         *p_isr_counter;

  /* Check for vailidity of incoming pointers.*/
  if (NULL == vp_afe_dynamic_thread)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed: Memory not allocated to input params.");
    return ADSP_ENOMEMORY;
  }

  afe_dynamic_thread_ptr = (afe_dynamic_thread_t*) vp_afe_dynamic_thread;
  pChannel               = &(afe_dynamic_thread_ptr->channel);
  task                   = afe_dynamic_thread_ptr->task;

  /* Initialize local tracking counter to zero.*/
  p_isr_counter          = &(afe_dynamic_thread_ptr->isr_context_counter);

  for (;;)
  {
    /* Wait for any signal.*/
    ch_status = qurt_elite_channel_wait(pChannel, AFE_DYNM_THRD_CHANNEL_WAIT_MASK);

    /* Type of the task to process.*/
    task_index = s32_get_lsb_s32(ch_status);

    /* If kill task bit is active. */
    if (AFE_DYNM_THRD_KILL_TASK == task_index)
    {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Afe Dyn Thread: 0x%lx, port_id: 0x%lx Stopping this thread.",
            (uint32_t)afe_dynamic_thread_ptr->thread_id,
            afe_dynamic_thread_ptr->port_id);

      result = ADSP_EOK;
      break;
    }  /* else if normal task.*/
    else if (AFE_DYNM_THRD_PROCESS_TASK == task_index)
    {
      /* Clear the signal.*/
      qurt_elite_signal_clear(task.task_signal_ptr);

      /* If ISR interrupt count is greater than its previous interrupt
       * count by 1, then one or more interrupts have not been serviced.*/
      signal_miss_count = *p_isr_counter - isr_count_tracker;
      if(signal_miss_count > 1)
      {
        /* There is no need to clear the signal again*/
        afe_dynamic_thread_ptr->task.total_ignored_signals += (signal_miss_count - 1);
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Afe Dyn Thrd ID: 0x%lx signal miss, port_id: 0x%lx, ignored signals: %lu",
              (uint32_t)afe_dynamic_thread_ptr->thread_id, afe_dynamic_thread_ptr->port_id,
              afe_dynamic_thread_ptr->task.total_ignored_signals);

        /* Call the AFE error handler */
        afe_err_handler(afe_dynamic_thread_ptr->port_id);
      }

      /* Fetch the current ISR count value.*/
      isr_count_tracker = *p_isr_counter;

      /* Call the actual execution function.*/
      result = task.pfn_task_func(task.vp_task_arg);

      /* If task fails.*/
      if (ADSP_FAILED(result))
      {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Task failed");
      }

      /* These counters are required to identify if interrupt service
       * completion time is exceeding device interrupt duration.*/
      signal_miss_count = *p_isr_counter - isr_count_tracker;

      /** For timer interrupt driven threads, signal miss will be detected by
       *  checking is_signal_miss flag. If signal miss happen for timer interrupt
       *  driven threads, then this flag should be set while executing actual function.*/
      if ( (signal_miss_count > 0) || (TRUE == afe_dynamic_thread_ptr->is_signal_miss) )
      {
        /**< For timer interrupt driven threads, no need to clear the task signal, since
         * task signal is not at all being set.
         *
         * Corner case: If signal miss happened while processing the task and at the same
         *              time, kill signal also being set.
         * Actually, kill signal happens during dynamic thread destroy and in that case
         * port state will be in STOP state.
         * Device recovery will happens only when port is in RUN state (later it moves to
         * CONIFG state, internally). Therefore, kill signal will be detected by channel
         * wait at the top of this work loop and thereby comes out from infinite loop.*/
        if(FALSE == afe_dynamic_thread_ptr->is_signal_miss)
        {
          /* Clear the signal(s) which is/are not going to be addressed */
          /* Wait for any signal.*/
          ch_status = qurt_elite_channel_wait(pChannel, AFE_DYNM_THRD_CHANNEL_WAIT_MASK);

          /* Type of the task to process.*/
          task_index = s32_get_lsb_s32(ch_status);

          if (AFE_DYNM_THRD_PROCESS_TASK == task_index)
          {
            /* Clear the signal.*/
            qurt_elite_signal_clear(task.task_signal_ptr);
          }
        }

        /* Increment the signal miss counter */
        afe_dynamic_thread_ptr->task.total_ignored_signals += (signal_miss_count + ( (TRUE == afe_dynamic_thread_ptr->is_signal_miss)?1:0 ));
        MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "Afe Dyn Thrd ID: 0x%lx signal miss ISR time exceed, port_id: 0x%lx, ignored signals: %lu",
              (uint32_t)afe_dynamic_thread_ptr->thread_id,
              afe_dynamic_thread_ptr->port_id,
              afe_dynamic_thread_ptr->task.total_ignored_signals);

        /* Call the AFE error handler */
        afe_err_handler(afe_dynamic_thread_ptr->port_id);

        /* Reset counters and clear signal miss flag, otherwise un-serviced
         * interrupt warning messages will keep on coming even for serviced interrupts.*/
        isr_count_tracker = 0;
        *p_isr_counter = 0;
        afe_dynamic_thread_ptr->is_signal_miss = FALSE;
      }
    }
  }

  return result;
}

/**
 * Initializes the thread and launches it.
 * @param[in] afe_dynamic_thread_ptr: Pointer to dynamic thread.
 * @param[in] afe_thread_task_ptr: Name of dynamic thread task.
 * @param[in] thread_priority: Customized thread priority.
 * @param[in] stack_ptr: Pointer to thread stack.
 * @param[in] stack_size: Size of stack.
 * @param[in] port_id: AFE port id for which dynamic thread is launched.
 * @return ADSPResult code.
 */
ADSPResult afe_dynamic_thread_launch(afe_dynamic_thread_t **afe_port_dynamic_thread_ptr,
                                     afe_dynamic_thread_task_t *afe_thread_task_ptr,
                                     uint32_t thread_priority,
                                     uint32_t stack_size,
                                     uint32_t port_id)
{
   ADSPResult           result                  = ADSP_EOK;
   afe_dynamic_thread_t *afe_dynamic_thread_ptr = NULL;
   uint32_t             mask                    = 0;
   char                 thread_name[QURT_ELITE_DEFAULT_NAME_LEN];

   *afe_port_dynamic_thread_ptr = NULL;

   /* Check for vailidity of incoming pointers.*/
   if (NULL == afe_thread_task_ptr)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed: NULL dynamic thread task pointer, port_id: 0x%lx",
            port_id);
      return ADSP_EFAILED;
   }

   /* Allocate memory for dynamic thread state object */
   if( NULL == (afe_dynamic_thread_ptr = (afe_dynamic_thread_t *) qurt_elite_memory_malloc(sizeof(afe_dynamic_thread_t),
                                                                                           QURT_ELITE_HEAP_DEFAULT)) )
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
            "Failed to allocate memory for dynamic thread state obj, port_id: 0x%lx",
            port_id);
      return ADSP_ENOMEMORY;
   }

   /* Initialize dynamic thread data structure.*/
   memset(afe_dynamic_thread_ptr, 0, sizeof(afe_dynamic_thread_t));

   /* Initialize channel.*/
   qurt_elite_channel_init(&(afe_dynamic_thread_ptr->channel));

   /* Associate task to dynamic thread task pointer.*/
   afe_dynamic_thread_ptr->port_id                    = port_id;
   afe_dynamic_thread_ptr->task.task_signal_ptr       = afe_thread_task_ptr->task_signal_ptr;
   afe_dynamic_thread_ptr->task.pfn_task_func         = afe_thread_task_ptr->pfn_task_func;
   afe_dynamic_thread_ptr->task.vp_task_arg           = afe_thread_task_ptr->vp_task_arg;
   afe_dynamic_thread_ptr->task.total_ignored_signals = 0;

   /* Set thread attributes.*/
   afe_dynamic_thread_ptr->stack_size          = stack_size;
   afe_dynamic_thread_ptr->thread_priority     = thread_priority;
   afe_dynamic_thread_ptr->isr_context_counter = 0;

   /* Add kill task signal to dynamic thread channel.*/
   mask = s32_set_bit_s32_s32(0, AFE_DYNM_THRD_KILL_TASK);

   if (ADSP_FAILED(result = qurt_elite_channel_add_signal( &(afe_dynamic_thread_ptr->channel),
                                                           &(afe_dynamic_thread_ptr->kill_signal), mask)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Failed to add kill task signal to channel, bailing out thread launch, port_id: 0x%lx, result: %d",
            port_id, result);
      goto __bailout_dynamic_thread_launch;
   }

   /* Add normal task signal to thread channel.*/
   mask = s32_set_bit_s32_s32(0, AFE_DYNM_THRD_PROCESS_TASK);

   if (ADSP_FAILED(result = qurt_elite_channel_add_signal(&(afe_dynamic_thread_ptr->channel),
                                                          afe_dynamic_thread_ptr->task.task_signal_ptr, mask)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Failed to add thread signal to channel, bailing out thread launch, port_id: 0x%lx, result: %d",
            port_id, result);
      goto __bailout_dynamic_thread_launch;
   }

   /* Assign dynamic thread name.*/
   snprintf(thread_name, QURT_ELITE_DEFAULT_NAME_LEN, "AfeDy%2lx", port_id);

   /* Launch dynamic thread.*/
   if( ADSP_EOK != (result = qurt_elite_thread_launch(&(afe_dynamic_thread_ptr->thread_id),
                                                              thread_name,
                                                              NULL,
                                                              afe_dynamic_thread_ptr->stack_size,
                                                              afe_dynamic_thread_ptr->thread_priority,
                                                              afe_dynamic_thread_work_loop,
                                                              (void*)afe_dynamic_thread_ptr,
                                                              QURT_ELITE_HEAP_DEFAULT)))
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
            "Failed to launch dynamic thread, port_id: 0x%lx, result: %d",
            port_id, result);
      goto __bailout_dynamic_thread_launch;
   }

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
         "Thread launched successfully, port_id: 0x%lx, thrdId: 0x%lx.",
         port_id, (uint32_t)afe_dynamic_thread_ptr->thread_id);

   /* Assign local thread handle to port handle.*/
   *afe_port_dynamic_thread_ptr = afe_dynamic_thread_ptr;

   return result;

__bailout_dynamic_thread_launch:

   if (NULL != afe_dynamic_thread_ptr)
   {
      afe_dynamic_thread_destroy(&afe_dynamic_thread_ptr);
   }

   return ADSP_EFAILED;
}

/**
 * Destroy thread launched by AFE dynamic thread.
 * @param[in] afe_dynamic_thread_ptr_ptr: thread whose thread is active.
 * @return the ADSPResult error code.
 */
ADSPResult afe_dynamic_thread_destroy(
   afe_dynamic_thread_t **afe_dynamic_thread_ptr_ptr)
{
   ADSPResult           result                  = ADSP_EOK;
   afe_dynamic_thread_t *afe_dynamic_thread_ptr = *afe_dynamic_thread_ptr_ptr;

  /* Check for validity of incoming pointers.*/
   if(NULL == afe_dynamic_thread_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed memory not allocated to input params.");
      result = ADSP_ENOMEMORY;
   }
   else
   {
      /* Valid thread id means it was created successfully. */
      if(afe_dynamic_thread_ptr->thread_id > 0)
      {
         /* Send kill signal.*/
         qurt_elite_signal_send(&(afe_dynamic_thread_ptr->kill_signal));

         qurt_elite_thread_join(afe_dynamic_thread_ptr->thread_id, &result);

         /* Destroy thread's synchronization attributes.*/
         qurt_elite_signal_deinit(&(afe_dynamic_thread_ptr->kill_signal));
         qurt_elite_signal_deinit(afe_dynamic_thread_ptr->task.task_signal_ptr);
         qurt_elite_channel_destroy(&(afe_dynamic_thread_ptr->channel));

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "AFE Dyn Thread: 0x%lx, port_id: 0x%lx, Destroyed.",
               (uint32_t)afe_dynamic_thread_ptr->thread_id,
               afe_dynamic_thread_ptr->port_id);
      }

      qurt_elite_memory_free((void *)afe_dynamic_thread_ptr);
      *afe_dynamic_thread_ptr_ptr = NULL;
   }

   return result;
}
