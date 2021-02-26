/*======================================================================================
  @file sns_mem_heaps.c

  Heap manager for Sensors.

  Clients may register for low memory callbacks, once a specific heap usage has
  exceeded some threshold.  Clients specify a byte threshold, but all clients will
  be notified once the lowest client threshold has been reached.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ======================================================================================*/

/*======================================================================================
  INCLUDES
  ======================================================================================*/
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_diag_types.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_mem_def.h"
#include "sns_mem_util.h"
#include "sns_memheap_lite.h"
#include "sns_memmgr.h"
#include "sns_osa_thread.h"
#include "sns_printf_int.h"
#include "sns_rc.h"
#include "sns_thread_prio.h"
#include "sns_time.h"
#include "sns_types.h"

/*=============================================================================
  Constants and Macros
  ===========================================================================*/

#define SNS_MEMMGR_LOW_MEM_SIG 0x01

/* only trigger low mem callback when free memory difference is bigger than this 
threshold */
#define sns_memmgr_min_diff(a) ((a)>>4)
/*=============================================================================
  Typedefs
  ===========================================================================*/

/**
 * Available memory heap.
 */
typedef struct
{
  const sns_mem_heap_id heap_id;
  /* Total size of this heap */
  const uint32_t heap_size;
  /* Heap internal data */
  sns_mem_heap_type *heap_head;
  /* Heap starting address */
  void *heap_memory;
  /* Whether this heap is expected to be used in Island mode */
  const bool island_use;
  /* A list of all low memory callbacks registered with the memmgr */
  sns_isafe_list lowmem_cb_list;
} sns_mem_heap_info;

/**
 * Registered callbacks from a memmgr client.
 */
typedef struct
{
  sns_isafe_list_item list_entry;
  /* Amount of free memory (in bytes) below which to trigger callback */
  uint32_t threshold;
  /* Number of free bytes during last low mem callback */
  int32_t prev_free_bytes;
  /* Client-specified callback function; may be in bigImage */
  sns_mem_heap_lowmem_cb callback;
  intptr_t callback_arg;
  sns_mem_heap_id heap_id;
} sns_mem_heap_low_mem_client;

/*=============================================================================
  Global Data
  ===========================================================================*/
/**
 * peripherial memory, start address and size
 */
void    *sns_pram_addr;
uint32_t sns_pram_size;

/*=============================================================================
  Heaps
  ===========================================================================*/
/**
 * heapSize is amss_mem_heap size for Sensors PD.
 * heapSize is a weak variable that we can implement.
 * Default definition is in \slpi_proc\core\kernel\qurt\libs\c\sys\src\sys_sbrk.c
 */
unsigned int heapSize = 0x100000;

/**
 *  Heap:         Main Batch Heap
 *  Size:         SNS_MAIN_BATCH_HEAP_SIZE
 *  Description:  Primary heap that resides in the DDR. Not accessible in island mode
 */
#define SNS_BATCH_HEAP_SIZE           (SNS_BATCH_HEAP_ALLOC*1024)

/* Heap head */
sns_mem_heap_type sns_batch_heap_head SNS_SECTION(".bss.sns_main_heap");
/* Heap memory block*/
uint8_t sns_batch_heap [SNS_BATCH_HEAP_SIZE] SNS_SECTION(".bss.sns_main_heap");

/**
 *  Heap:         Main Heap
 *  Size:         SNS_MAIN_HEAP_SIZE
 *  Description:  Primary heap that resides in the DDR. Not accessible in island mode
 */
#define SNS_MAIN_HEAP_SIZE           (SNS_MAIN_HEAP_ALLOC*1024)

/* Heap head */
sns_mem_heap_type sns_main_heap_head SNS_SECTION(".bss.sns_main_heap");
/* Heap memory block*/
uint8_t sns_main_heap[SNS_MAIN_HEAP_SIZE] SNS_SECTION(".bss.sns_main_heap");

 /**
 *  Heap:         Island Heap
 *  Size:         SNS_ISLAND_HEAP_SIZE
 *  Description:  Primary heap that resides in the TCM. Accessible in island mode
 */
/* Heap size */
#define SNS_ISLAND_HEAP_SIZE       (SNS_ISLAND_HEAP_ALLOC*1024)

/* Heap head */
sns_mem_heap_type sns_island_heap_head SNS_SECTION(".bss.sns_island_heap");
#if !defined(__sensors_island_start)
uint32_t __sensors_island_start;
#endif
/* Heap memory block*/
uint8_t sns_island_heap[SNS_ISLAND_HEAP_SIZE] SNS_SECTION(".bss.sns_island_heap");
#if !defined(__sensors_island_end)
uint32_t __sensors_island_end;
#endif

/**
 *  Heap:         PRAM Heap
 *  Size:         SNS_PRAM_HEAP_SIZE
 *  Description:  Heap that resides in SLPI peripheral memory region. Accessible
 *                by all cores (SDC/Q6/QUP) in island mode
 */
/* Heap size */
/* Heap head */
sns_mem_heap_type sns_pram_heap_head SNS_SECTION(".bss.sns_island_heap");

#ifndef SSC_TARGET_PRAM_AVAILABLE
uint8_t sns_pram_heap[SNS_PRAM_HEAP_SIZE] SNS_SECTION(".bss.sns_island_heap");
#else
#define sns_pram_heap NULL
#endif

/**
 *  Heap:         Event Heap
 *  Size:         SNS_EVENT_HEAP_SIZE
 *  Description:  Event buffer that resides in the
 *                DDR. Not accessible in island mode
 */
#define SNS_EVENT_HEAP_SIZE       (SNS_EVENT_HEAP_ALLOC*1024)

/* Heap head */
sns_mem_heap_type sns_event_heap_head SNS_SECTION(".bss.sns_main_heap");
/* Heap memory block*/
uint8_t sns_event_heap[SNS_EVENT_HEAP_SIZE] SNS_SECTION(".bss.sns_main_heap");

/*=============================================================================
  Static Data
  ===========================================================================*/

/* All heaps known to the SSC */
static sns_mem_heap_info heap_info[] SNS_SECTION(".data.sns_island_heap") =
{
  { .heap_id = SNS_HEAP_MAIN, .heap_size = SNS_MAIN_HEAP_SIZE,
    .heap_head = &sns_main_heap_head, .heap_memory = sns_main_heap,
    .island_use = false},
  { .heap_id = SNS_HEAP_BATCH, .heap_size = SNS_BATCH_HEAP_SIZE,
    .heap_head = &sns_batch_heap_head, .heap_memory = sns_batch_heap,
    .island_use = false},
  { .heap_id = SNS_HEAP_ISLAND, .heap_size = SNS_ISLAND_HEAP_SIZE,
    .heap_head = &sns_island_heap_head, .heap_memory = sns_island_heap,
    .island_use = true},
  { .heap_id = SNS_HEAP_PRAM, .heap_size = SNS_PRAM_HEAP_SIZE,
    .heap_head = &sns_pram_heap_head, .heap_memory = sns_pram_heap,
    .island_use = true},
  { .heap_id = SNS_HEAP_EVENT, .heap_size = SNS_EVENT_HEAP_SIZE,
    .heap_head = &sns_event_heap_head, .heap_memory = sns_event_heap,
    .island_use = false},
};

/* Island client for low mem thread handler */
static sns_island_client_handle low_mem_island_client;
/* Low memory thread; created at init */
static sns_osa_thread *low_mem_thread;
/* Low memory clients to be invoked by the low memory thread */
static sns_isafe_list lowmem_cb_list;
/* Protects the low memory client list on all heaps */
sns_osa_lock heap_info_lock SNS_SECTION(".data.sns_island_heap");

/*=============================================================================
  Static Functions
  ===========================================================================*/

SNS_SECTION(".text.sns_island_heap") static bool
sns_belongs_to_heap(sns_mem_heap_id heap_id, void const *ptr)
{
  return ((uintptr_t)(heap_info[heap_id].heap_memory) != (uintptr_t)NULL &&
          (uintptr_t)(heap_info[heap_id].heap_memory) <= ((uintptr_t)(ptr)) &&
          ((uintptr_t)(ptr)) < ((uintptr_t)heap_info[heap_id].heap_memory + heap_info[heap_id].heap_size));
}

static sns_rc
sns_memmgr_log_heap_usage_encode_cb(void *log, size_t log_size,
                                    size_t encoded_log_size, void *encoded_log,
                                    size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;

  if(NULL == log || 0 == log_size || NULL == encoded_log ||
     0 == encoded_log_size || NULL == bytes_written)
  {
    return SNS_RC_FAILED;
  }

  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_mem_utilization_log_fields, log))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding memory utilization log : %s",
               PB_GET_ERROR(&stream));
    rc = SNS_RC_FAILED;
  }

  if(SNS_RC_SUCCESS == rc)
  {
    *bytes_written = stream.bytes_written;
  }
  return rc;
}

/**
 * Check whether any registered low memory client on this heap has had their
 * threshold met.  If so, signal low memory thread for handling.
 */
SNS_SECTION(".text.sns_island_heap") static void
sns_mem_heap_check_lowmem(sns_mem_heap_id heap_id, bool force)
{
  sns_isafe_list_iter iter;
  uint32_t free_bytes;

  sns_osa_lock_acquire(&heap_info_lock);
  free_bytes = heap_info[heap_id].heap_head->total_bytes -
               heap_info[heap_id].heap_head->allocated_bytes;

  for(sns_isafe_list_iter_init(&iter, &heap_info[heap_id].lowmem_cb_list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    sns_mem_heap_low_mem_client *cb = (sns_mem_heap_low_mem_client*)
        sns_isafe_list_iter_get_curr_data(&iter);
    uint32_t diff_bytes = free_bytes < cb->prev_free_bytes ? 
        cb->prev_free_bytes - free_bytes: 0;

    if(force || (cb->threshold >= free_bytes && 
        diff_bytes >= sns_memmgr_min_diff(heap_info[heap_id].heap_size)))
    {
      sns_isafe_list_iter low_mem_iter;

      SNS_ISLAND_EXIT();
      sns_isafe_list_iter_init(&low_mem_iter, &lowmem_cb_list, true);
      sns_isafe_list_iter_remove(&iter);
      sns_isafe_list_iter_insert(&low_mem_iter, &cb->list_entry, true);

      SNS_PRINTF(HIGH, sns_fw_printf, "Low memory on heap %i, available mem: %d, threshold: %d, forced: %d",
                  heap_id, free_bytes, cb->threshold, force);
      cb->prev_free_bytes = free_bytes;
      sns_osa_thread_sigs_set(low_mem_thread, SNS_MEMMGR_LOW_MEM_SIG);
    }
    if(cb->threshold < free_bytes)
    {
      cb->prev_free_bytes = heap_info[heap_id].heap_size;
    }
  }

  sns_osa_lock_release(&heap_info_lock);
}

/**
 * Low memory thread.  Waits for a registered low memory client threshold to
 * be exceeded.
 */
static void
low_mem_thread_func(sns_osa_thread_func_arg arg)
{
  UNUSED_VAR(arg);
  sns_isafe_list_iter client_iter, low_mem_iter;
  uint32_t accepted_sigs = SNS_MEMMGR_LOW_MEM_SIG;
  sns_osa_thread_active();

  while(1)
  {
    uint32_t sigs = 0;
    sns_osa_thread_idle();
    sns_osa_thread_sigs_wait(accepted_sigs, &sigs);
    sns_osa_thread_active();

    sns_island_block(low_mem_island_client);
    sns_osa_lock_acquire(&heap_info_lock);

    sns_isafe_list_iter_init(&low_mem_iter, &lowmem_cb_list, true);
    while(NULL != sns_isafe_list_iter_curr(&low_mem_iter))
    {
      sns_mem_heap_low_mem_client *cb = (sns_mem_heap_low_mem_client*)
          sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&low_mem_iter));
      sns_osa_lock_release(&heap_info_lock);
      size_t before, after;
      sns_time start, end;
      before = sns_memmgr_get_heap_usage(cb->heap_id);
      start = sns_get_system_time();
      cb->callback(cb->callback_arg);
      end = sns_get_system_time();
      after = sns_memmgr_get_heap_usage(cb->heap_id);

      SNS_PRINTF(HIGH, sns_fw_printf, "Low memory callaback 0x" SNS_DIAG_PTR " dropped %d bytes on heap %d in %d ms",
                 cb->callback, (before-after), cb->heap_id, ((end-start)*100)/1920000);
      sns_osa_lock_acquire(&heap_info_lock);
      sns_isafe_list_iter_init(&client_iter, &heap_info[cb->heap_id].lowmem_cb_list, true);
      sns_isafe_list_iter_insert(&client_iter, &cb->list_entry, true);
    }

    sns_osa_lock_release(&heap_info_lock);
    sns_island_unblock(low_mem_island_client);
  }
}

/**
 * Initialize and start the low memory handling thread.
 */
static void
start_low_mem_thread(void)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_osa_thread_attr attr;
  sns_osa_thread_attr_init(&attr);
  rc |= sns_osa_thread_attr_set_name(&attr, "SNS_LOW_MEM");
  rc |= sns_osa_thread_attr_set_stack(&attr, (uintptr_t)NULL, 4096);
  rc |= sns_osa_thread_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_NORMAL);
  rc |= sns_osa_thread_attr_set_priority(&attr, SNS_LOW_MEM_THREAD_PRIO);

  low_mem_island_client = sns_island_aggregator_register_client("low_mem");
  if(SNS_RC_SUCCESS == rc)
  {
    rc = sns_osa_thread_create(&low_mem_thread_func, NULL, &attr, &low_mem_thread);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
  }
  else
    SNS_PRINTF(MED, sns_fw_printf, "Error setting thread attributes");
}

/*=============================================================================
  Public Functions
  ===========================================================================*/

void
sns_heap_init(uint8_t *pram_heap_head)
{
  sns_rc rc;
  sns_osa_lock_attr attr;

  if(NULL == heap_info[SNS_HEAP_PRAM].heap_memory)
  {
    heap_info[SNS_HEAP_PRAM].heap_memory = pram_heap_head;
  }

  for(int i = 0; i < SNS_HEAP_MAX; i++)
  {
    if(heap_info[i].heap_memory != NULL)
    {
      //PEND: On silicon, island and PRAM memory set to 0xFFFF
      //causing sns_mem_init_heap() to return on fake success
      //This hack assumes sns_heap_init() is called only once
      heap_info[i].heap_head->sns_magic_num = 0;

      sns_mem_init_heap(
        heap_info[i].heap_head,
        heap_info[i].heap_memory,
        heap_info[i].heap_size);

    }
    sns_isafe_list_init(&heap_info[i].lowmem_cb_list);
  }

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_init(&attr, &heap_info_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  sns_isafe_list_init(&lowmem_cb_list);
}

void sns_heap_init_delayed(void)
{
  start_low_mem_thread();
}

SNS_SECTION(".text.sns_island_heap") void*
sns_malloc(sns_mem_heap_id heap_id, size_t size)
{
  void *ptr;
  
  SNS_ASSERT(heap_id < SNS_HEAP_MAX);
  ptr = sns_mem_malloc(heap_info[heap_id].heap_head, size);
  sns_mem_heap_check_lowmem(heap_id, (NULL == ptr));
  
  if(NULL != ptr)
  {
    sns_memzero(ptr, size);
  }
  else
  {
    uint16_t counter = 0;
    // Currently, interesetd only on Event heap. Max loop termination time is 5min and minimum loop termination time is 10ms. 
    // Low memory thread should be able to free up all the events with in 5min.
    while((SNS_HEAP_EVENT == heap_id) && (NULL == ptr) && (30000 > counter))
    {
      //Wait 10ms and retry malloc
      sns_busy_wait(sns_convert_ns_to_ticks(10*1000*1000));  
      ptr = sns_mem_malloc(heap_info[heap_id].heap_head, size);
      if(NULL != ptr)
      {
        sns_memzero(ptr, size);
      }
      counter++;
    }
  }

  return ptr;
}

SNS_SECTION(".text.sns_island_heap")
void sns_free(void *ptr)
{
  for(int i = 0; i < SNS_HEAP_MAX; i++)
  {
    if(sns_belongs_to_heap(i, ptr))
    {
      sns_mem_free(heap_info[i].heap_head, ptr);
      break;
    }
  }
}

void
sns_mem_heap_register_lowmem_cb(sns_mem_heap_id heap_id, uint32_t threshold,
  sns_mem_heap_lowmem_cb callback, intptr_t callback_arg)
{
  sns_isafe_list_iter iter;
  sns_mem_heap_low_mem_client *callback_data;

  callback_data = sns_malloc(SNS_HEAP_ISLAND, sizeof(*callback_data));
  if(NULL == callback_data)
    callback_data = sns_malloc(SNS_HEAP_MAIN, sizeof(*callback_data));
  SNS_ASSERT(NULL != callback_data);

  callback_data->heap_id = heap_id;
  callback_data->threshold = threshold;
  callback_data->callback = callback;
  callback_data->callback_arg = callback_arg;
  callback_data->prev_free_bytes = heap_info[heap_id].heap_size;

  sns_osa_lock_acquire(&heap_info_lock);
  sns_isafe_list_iter_init(&iter, &heap_info[heap_id].lowmem_cb_list, true);
  sns_isafe_list_item_init(&callback_data->list_entry, callback_data);
  sns_isafe_list_iter_insert(&iter, &callback_data->list_entry, true);
  sns_osa_lock_release(&heap_info_lock);
}

SNS_SECTION(".text.sns_island_heap") sns_mem_heap_id
sns_memmgr_get_heap_id(void const *ptr)
{
  for(int i = 0; i < SNS_HEAP_MAX; i++)
  {
    if(sns_belongs_to_heap(i, ptr))
    {
     return i;
    }
  }
  return SNS_HEAP_MAX;
}

SNS_SECTION(".text.sns_island_heap") void
sns_memmgr_log_heap_usage(uint64_t cookie)
{
  for(int i = 0; i < ARR_SIZE(heap_info); i++)
  {
    sns_diag_mem_utilization_log *log = sns_diag_log_alloc(
        sizeof(sns_diag_mem_utilization_log), SNS_LOG_MEMORY_UTILIZATION);

    if(!heap_info[i].island_use)
      SNS_ISLAND_EXIT();

    if(NULL != log)
    {
      if(SNS_HEAP_MAIN == heap_info[i].heap_id)
        log->heap_id = SNS_DIAG_HEAP_ID_HEAP_MAIN;
      else if(SNS_HEAP_BATCH == heap_info[i].heap_id)
        log->heap_id = SNS_DIAG_HEAP_ID_HEAP_BATCH;
      else if(SNS_HEAP_ISLAND == heap_info[i].heap_id)
        log->heap_id = SNS_DIAG_HEAP_ID_HEAP_ISLAND;
      else if(SNS_HEAP_PRAM == heap_info[i].heap_id)
        log->heap_id = SNS_DIAG_HEAP_ID_HEAP_PRAM;
      else if(SNS_HEAP_EVENT == heap_info[i].heap_id)
        log->heap_id = SNS_DIAG_HEAP_ID_HEAP_EVENT;
      else
        log->heap_id = 255;

      log->timestamp = sns_get_system_time();
      heap_info[i].heap_head->lock_fnc_ptr(heap_info[i].heap_head); // Lock the heap mutex
      log->total_memory = heap_info[i].heap_head->total_bytes;
      log->used_memory = heap_info[i].heap_head->used_bytes;
      log->has_cookie = true;
      log->cookie = cookie;
      heap_info[i].heap_head->free_fnc_ptr(heap_info[i].heap_head); // Unlock the heap mutex

      //SNS_PRINTF(LOW, sns_fw_printf, "Heap %u Size: %u Used: %u", i, log->total_memory, log->used_memory);
      sns_diag_publish_fw_log(SNS_LOG_MEMORY_UTILIZATION, sizeof(sns_diag_mem_utilization_log), log,
        sns_diag_mem_utilization_log_size, sns_memmgr_log_heap_usage_encode_cb);
    }
  }
}

SNS_SECTION(".text.sns_island_heap")
size_t sns_memmgr_get_heap_usage(sns_mem_heap_id heap_id)
{
  size_t ret_val = 0;

  if(heap_id < SNS_HEAP_MAX)
  {
    if(!heap_info[heap_id].island_use)
      SNS_ISLAND_EXIT();

    heap_info[heap_id].heap_head->lock_fnc_ptr(heap_info[heap_id].heap_head); // Lock the heap mutex
    ret_val = heap_info[heap_id].heap_head->used_bytes;
    heap_info[heap_id].heap_head->free_fnc_ptr(heap_info[heap_id].heap_head); // Unlock the heap mutex
  }
  return ret_val;
}
