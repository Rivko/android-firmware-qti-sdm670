/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#ifndef ADSP_AMDB_TEST
#include "qurt_elite.h"
#include "stringl.h"
#else
#include "test_main.h"
#endif

#include "qurt_elite_diag.h"

struct adsp_amdb_queue_t
{
   uint8_t *data_ptr;
   uint32_t push_location;
   uint32_t num_elements;
   uint32_t max_elements;
   uint32_t size_of_element;
   char name[16];

   qurt_elite_nmutex_t queue_nmutex; // Have to use qurt mutex instead of qurt_elite_mutex since qurt_elite uses pimutex, which is not compatible with condvars.
   qurt_elite_condvar_t queue_push_condition;
   qurt_elite_condvar_t queue_pop_condition;
};

/*----------------------------------------------------------------------------
 * Function Declarations
 * -------------------------------------------------------------------------*/
static bool_t amdb_queue_is_empty(const adsp_amdb_queue_t *queue_ptr)
{
   return (0 == queue_ptr->num_elements);
}

static bool_t amdb_queue_is_full(const adsp_amdb_queue_t *queue_ptr)
{
   return (queue_ptr->max_elements == queue_ptr->num_elements);
}

adsp_amdb_queue_t *amdb_queue_create(uint32_t max_items, uint32_t size_of_element, char name[])
{
   uint32_t size_needed = 0;

   size_needed += sizeof(adsp_amdb_queue_t);
   size_needed += size_of_element * max_items;

   uint8_t *mem_ptr = (uint8_t*)qurt_elite_memory_malloc(size_needed, QURT_ELITE_HEAP_DEFAULT);
   if (NULL == mem_ptr)
   {
      MSG_SPRINTF_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "amdb: Failed to allocate memory for queue object %s.", name);
      return NULL;
   }

   adsp_amdb_queue_t *queue_ptr = reinterpret_cast<adsp_amdb_queue_t*>(mem_ptr);
   mem_ptr += sizeof(adsp_amdb_queue_t);
   queue_ptr->data_ptr = mem_ptr;
   queue_ptr->push_location = 0;
   queue_ptr->num_elements = 0;
   queue_ptr->max_elements = max_items;
   queue_ptr->size_of_element = size_of_element;
   strlcpy(queue_ptr->name, name, sizeof(queue_ptr->name)/sizeof(queue_ptr->name[0]));

   qurt_elite_nmutex_init(&queue_ptr->queue_nmutex);
   qurt_elite_condvar_init(&queue_ptr->queue_push_condition);
   qurt_elite_condvar_init(&queue_ptr->queue_pop_condition);

   return queue_ptr;
}

void amdb_queue_push(adsp_amdb_queue_t *queue_ptr, const void *in_data_ptr)
{
   qurt_elite_nmutex_lock(&queue_ptr->queue_nmutex);
   while (amdb_queue_is_full(queue_ptr))
   {
      qurt_elite_condvar_wait(&queue_ptr->queue_push_condition, &queue_ptr->queue_nmutex);
   }

   uint8_t *push_location_ptr = queue_ptr->data_ptr + queue_ptr->push_location * queue_ptr->size_of_element;
   memscpy(push_location_ptr, queue_ptr->size_of_element, in_data_ptr, queue_ptr->size_of_element);
   queue_ptr->num_elements++;
   queue_ptr->push_location++;
   if(queue_ptr->push_location >= queue_ptr->max_elements)
   {
      queue_ptr->push_location = 0;
   }

   qurt_elite_condvar_signal(&queue_ptr->queue_pop_condition); // In case others are waiting to pop.
   if (!amdb_queue_is_full(queue_ptr))
   {
      qurt_elite_condvar_signal(&queue_ptr->queue_push_condition); // In case others are waiting to push.
   }
   qurt_elite_nmutex_unlock(&queue_ptr->queue_nmutex);
}

void amdb_queue_pop(adsp_amdb_queue_t *queue_ptr, void *out_data_ptr)
{
   qurt_elite_nmutex_lock(&queue_ptr->queue_nmutex);
   while (amdb_queue_is_empty(queue_ptr))
   {
      qurt_elite_condvar_wait(&queue_ptr->queue_pop_condition, &queue_ptr->queue_nmutex);
   }

   uint32_t pop_location = queue_ptr->push_location + queue_ptr->max_elements - queue_ptr->num_elements;
   if (pop_location >= queue_ptr->max_elements)
   {
      pop_location -= queue_ptr->max_elements;
   }
   uint8_t *pop_location_ptr = queue_ptr->data_ptr + pop_location * queue_ptr->size_of_element;
   memscpy(out_data_ptr, queue_ptr->size_of_element, pop_location_ptr, queue_ptr->size_of_element);
   queue_ptr->num_elements--;

   qurt_elite_condvar_signal(&queue_ptr->queue_push_condition); // In case others are waiting to push.
   if (!amdb_queue_is_empty(queue_ptr))
   {
      qurt_elite_condvar_signal(&queue_ptr->queue_pop_condition); // In case others are waiting to pop.
   }
   qurt_elite_nmutex_unlock(&queue_ptr->queue_nmutex);
}

void amdb_queue_destroy(adsp_amdb_queue_t *queue_ptr)
{
   qurt_elite_nmutex_destroy(&queue_ptr->queue_nmutex);
   qurt_elite_condvar_destroy(&queue_ptr->queue_push_condition);
   qurt_elite_condvar_destroy(&queue_ptr->queue_pop_condition);
   qurt_elite_memory_free(queue_ptr);
}

