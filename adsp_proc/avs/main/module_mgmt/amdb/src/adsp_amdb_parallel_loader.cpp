/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "adsp_amdb_parallel_loader.h"
#include "qurt_elite_diag.h"
#ifndef ADSP_AMDB_TEST
#include "qurt_elite_thread.h"
#include "qurt_elite_atomic.h"
#include "elite_thrd_prio.h"
#else
#include "test_main.h"
#endif

#include "adsp_amdb_queue.h"
#include "adsp_amdb_resource_voter.h"

static int amdb_loader_thread_entry_function(void *context);

static const uint32_t AMDB_MAX_THREADS = QURT_ELITE_MAX_HW_THREADS; // Each thread will typically wait for the apps
                                                                    // to read and then do sig verification and
                                                                    // linking. Since the only opportunity for a
                                                                    // thread to wait is while waiting for the apps,
                                                                    // which is in the beginning, there is no point
                                                                    // in having more threads.
static const uint32_t AMDB_THREAD_STACK_SIZE = 8 * (1<<10); // 4K stack is needed for dlopen according to the platform team. Keeping stack as 8k till ADSPPM team gives a proper number.
static const uint32_t AMDB_TASK_Q_LENGTH = 64; // Must be a power of 2.
static const uint32_t AMDB_NUM_CLIENT_HANDLES = 8; // Must be a power of 2.

struct amdb_loader_handle
{
   adsp_amdb_get_modules_callback_f cb;
   void *context_ptr;
   amdb_loader_load_function load_function;
   qurt_elite_atomic_word_t refs;
   amdb_parallel_loader *parallel_loader_ptr;
};
static void amdb_loader_addref_handle(amdb_loader_handle *loader_handle_ptr);

struct amdb_parallel_loader
{
   uint32_t num_threads;
   qurt_elite_thread_t threads[AMDB_MAX_THREADS];
   adsp_amdb_queue_t *task_queue_ptr;
   adsp_amdb_queue_t *handle_queue_ptr;
   amdb_loader_handle handles[AMDB_NUM_CLIENT_HANDLES];
   amdb_voter_t *voter_ptr;
};

enum amdb_loader_task_type
{
   DLOPEN_TASK,
   THREAD_EXIT,
   VOTE,
   RELEASE_VOTE
};

struct amdb_loader_task
{
   amdb_loader_task_type type;
   union
   {
      struct dlopen_task_t
      {
         amdb_loader_handle *client_handle;
         uint64_t task_info;
      } dlopen_task;
      // Other task structures can be added here.
   } payload;
};

amdb_parallel_loader *amdb_loader_create(void)
{
   amdb_parallel_loader *obj_ptr = (amdb_parallel_loader*)qurt_elite_memory_malloc(sizeof(amdb_parallel_loader), QURT_ELITE_HEAP_DEFAULT);
   if (NULL == obj_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to allocate the amdb parallel loader.");
      return NULL;
   }

   obj_ptr->num_threads = 0;
   obj_ptr->task_queue_ptr = NULL;
   obj_ptr->handle_queue_ptr = NULL;
   obj_ptr->voter_ptr = NULL;

   char task_q_name[] = "AMDBTQ";
   obj_ptr->task_queue_ptr = amdb_queue_create(AMDB_TASK_Q_LENGTH, sizeof(amdb_loader_task), task_q_name);
   if (NULL == obj_ptr->task_queue_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to allocate the task queue for the amdb parallel loader.");
      amdb_loader_destroy(obj_ptr);
      return NULL;
   }

   char handle_q_name[] = "AMDBHQ";
   obj_ptr->handle_queue_ptr = amdb_queue_create(AMDB_NUM_CLIENT_HANDLES, sizeof(amdb_loader_handle*), handle_q_name);
   if (NULL == obj_ptr->handle_queue_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to allocate the handle queue for the amdb parallel loader.");
      amdb_loader_destroy(obj_ptr);
      return NULL;
   }

   for (uint32_t i = 0; i < sizeof(obj_ptr->handles)/sizeof(obj_ptr->handles[0]); i++)
   {
      obj_ptr->handles[i].cb = NULL;
      obj_ptr->handles[i].context_ptr = NULL;
      qurt_elite_atomic_set(&obj_ptr->handles[i].refs, 0);

      amdb_loader_handle *h = &obj_ptr->handles[i];

      amdb_queue_push(obj_ptr->handle_queue_ptr, &h);
   }

   obj_ptr->voter_ptr = amdb_voter_create();
   if (NULL == obj_ptr->voter_ptr)
   {
      amdb_loader_destroy(obj_ptr);
      return NULL;
   }
   
   uint32_t AMDB_THREAD_PRIORITY = elite_get_thrd_prio(ELITETHREAD_AMDB_THREADPOOL_PRIO_ID);

   // Note: Threads should be launched only after everything else is initialized, since they may access the object.
   char thread_name[] = "AMDB0";
   for (uint32_t i = 0; i < AMDB_MAX_THREADS; i++)
   {
      ADSPResult res = qurt_elite_thread_launch(&obj_ptr->threads[i], thread_name, NULL, AMDB_THREAD_STACK_SIZE, AMDB_THREAD_PRIORITY, amdb_loader_thread_entry_function, obj_ptr, QURT_ELITE_HEAP_DEFAULT);
      if(ADSP_FAILED(res))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: Failed to launch thread %lu.", i);
         break;
      }

      obj_ptr->num_threads++;
      thread_name[4]++;
   }

   if (0 == obj_ptr->num_threads)
   {
      // No thread could be launched.
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to launch any background thread.");
      amdb_loader_destroy(obj_ptr);
      return NULL;
   }

   return obj_ptr;
}

amdb_loader_handle *amdb_loader_get_handle(amdb_parallel_loader *obj_ptr, adsp_amdb_get_modules_callback_f callback_f, void *context_ptr, amdb_loader_load_function load_function)
{
   if (NULL == callback_f)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: NULL callback function in the parallel loader.");
      return NULL;
   }

   amdb_loader_handle *h = NULL;
   amdb_queue_pop(obj_ptr->handle_queue_ptr, &h);
   h->cb = callback_f;
   h->context_ptr = context_ptr;
   h->load_function = load_function;
   h->parallel_loader_ptr = obj_ptr;
   qurt_elite_atomic_set(&h->refs, 1);

   // Vote for resources for dlopen
   amdb_loader_task task;
   task.type = VOTE;
   amdb_queue_push(obj_ptr->task_queue_ptr, &task);

   MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: Started parallel loading for handle 0x%p", h);
   return h;
}

void amdb_loader_push_task(amdb_loader_handle *loader_handle_ptr, uint64_t task_info)
{
   amdb_loader_addref_handle(loader_handle_ptr);
   amdb_loader_task task;
   task.type = DLOPEN_TASK;
   task.payload.dlopen_task.task_info = task_info;
   task.payload.dlopen_task.client_handle = loader_handle_ptr;

   amdb_queue_push(loader_handle_ptr->parallel_loader_ptr->task_queue_ptr, &task);
}

static void amdb_loader_addref_handle(amdb_loader_handle *loader_handle_ptr)
{
   qurt_elite_atomic_increment(&loader_handle_ptr->refs);
}

void amdb_loader_release_handle(amdb_loader_handle *loader_handle_ptr)
{
   int value = qurt_elite_atomic_decrement(&loader_handle_ptr->refs);

   if (0 == value)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "amdb: Finished parallel loading for client 0x%p", loader_handle_ptr);

      loader_handle_ptr->cb(loader_handle_ptr->context_ptr);

      amdb_queue_push(loader_handle_ptr->parallel_loader_ptr->handle_queue_ptr, &loader_handle_ptr);

      // Release vote for resources
      amdb_loader_task task;
      task.type = RELEASE_VOTE;
      amdb_queue_push(loader_handle_ptr->parallel_loader_ptr->task_queue_ptr, &task);
   }
}

void amdb_loader_destroy(amdb_parallel_loader *obj_ptr)
{
   if (NULL != obj_ptr)
   {
      for (uint32_t i = 0; i < obj_ptr->num_threads; i++)
      {
         amdb_loader_task task;
         task.type = THREAD_EXIT;
         amdb_queue_push(obj_ptr->task_queue_ptr, &task);
      }

      for (uint32_t i = 0; i < obj_ptr->num_threads; i++)
      {
         int status = 0;
         qurt_elite_thread_join(obj_ptr->threads[i], &status);
      }

      if (NULL != obj_ptr->task_queue_ptr)
      {
         amdb_queue_destroy(obj_ptr->task_queue_ptr);
         obj_ptr->task_queue_ptr = NULL;
      }

      if (NULL != obj_ptr->handle_queue_ptr)
      {
         amdb_queue_destroy(obj_ptr->handle_queue_ptr);
         obj_ptr->handle_queue_ptr = NULL;
      }

      if (NULL != obj_ptr->voter_ptr)
      {
         amdb_voter_destroy(obj_ptr->voter_ptr);
         obj_ptr->voter_ptr = NULL;
      }

      qurt_elite_memory_free(obj_ptr);
   }
}

static int amdb_loader_thread_entry_function(void *context)
{
   amdb_parallel_loader *obj_ptr = reinterpret_cast<amdb_parallel_loader*>(context);

   while (1) {
      // Pop from the queue.
      amdb_loader_task task;
      amdb_queue_pop(obj_ptr->task_queue_ptr, &task);

#ifdef ADSP_AMDB_TEST
      extern volatile bool_t stall_background_threads;
      while(stall_background_threads);
#endif

      switch(task.type)
      {
      case DLOPEN_TASK:
      {
         task.payload.dlopen_task.client_handle->load_function(task.payload.dlopen_task.task_info);
         amdb_loader_release_handle(task.payload.dlopen_task.client_handle);
         break;
      }
      case THREAD_EXIT:
         return 0;
      case VOTE:
         amdb_voter_vote(obj_ptr->voter_ptr);
         break;
      case RELEASE_VOTE:
         amdb_voter_release(obj_ptr->voter_ptr);
         break;
      default:
         // Unknown opcode; can't do anything here.
         break;
      }
   }

   return 0;
}
