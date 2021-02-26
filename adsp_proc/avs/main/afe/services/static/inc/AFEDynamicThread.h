/*========================================================================
  This file contains AFE Dynamic thread design related apis

  Copyright (c) 2015 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFEDynamicThread.h#1 $
 ====================================================================== */
#ifndef _AFEDYNAMICTHREAD_H_
#define _AFEDYNAMICTHREAD_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */

/**
 * A task that is executed by the thread.
 */
typedef struct afe_dynamic_thread_task
{
   qurt_elite_signal_t *task_signal_ptr;           /**< Signal that triggers this task each time the signal is raised.*/
   ADSPResult          (*pfn_task_func)(void*);    /**< Processing function */
   void*               vp_task_arg;                /**< Single argument to the  function */
   uint32_t            total_ignored_signals;      /**< Total number of signals ignored because task was busy.*/

} afe_dynamic_thread_task_t;


/**
 * Parameters of the AFEDynamicThread.
 */
typedef struct afe_dynamic_thread
{
   uint32_t                      port_id;
   /**< Port id to which this thread is associated.*/

   qurt_elite_channel_t          channel;
   /**< Channel on which a thread waits.for task signals.*/

   qurt_elite_signal_t           kill_signal;
   /**< Kill signal for parent thread to control dynamically launched thread.*/

   afe_dynamic_thread_task_t     task;
   /**< Task to be served.*/

   qurt_elite_thread_t           thread_id;
   /**< Thread Id of thread launched dynamically.*/

   uint32_t                      thread_priority;
   /**< Thread priority for thread to be launched.*/

   uint32_t                      stack_size;
   /**< Stack size of thread.*/

   volatile uint32_t             isr_context_counter;
   /**< Counter to count no. of interrupt signals send by ISR Thread. This
      * counter must be incremented in all device driver interrupt handlers,
      * which are using dynamic thread service.*/

   bool_t                        is_signal_miss;
   /**< flag to detect signal miss for timer interrupt driven threads.*/
} afe_dynamic_thread_t;


/**
 * Initializes the dynamic thread data structure and does thread launch as well.
 * @param[in] afe_dynamic_thread_ptr: Pointer to dynamic thread.
 * @param[in] afe_thread_task_ptr: Name of dynamic thread task.
 * @param[in] thread_priority: Customized thread priority.
 * @param[in] stack_size: Size of stack.
 * @param[in] port_id: AFE port id for which dynamic thread is launched.
 * @return ADSPResult code.
 */
ADSPResult afe_dynamic_thread_launch(
   afe_dynamic_thread_t **afe_dynamic_thread_ptr,
   afe_dynamic_thread_task_t *afe_thread_task_ptr, uint32_t thread_priority,
   uint32_t stack_size, uint32_t port_id);

/**
 * Destroy thread launched by AFE dynamic thread.
 * @param[in] afe_dynamic_thread_ptr: Pointer to dynamic thread.
 * @return the ADSPResult error code.
 */
ADSPResult afe_dynamic_thread_destroy (afe_dynamic_thread_t **afe_dynamic_thread_ptr_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _AFEDYNAMICTHREAD_H_ */
