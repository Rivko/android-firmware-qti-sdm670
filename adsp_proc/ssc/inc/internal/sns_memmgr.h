#pragma once
/*=============================================================================
  @file sns_memmgr.h

  Memory manager abstraction layer for Sensors.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Includes
  ===========================================================================*/
#include <stdbool.h>
#include <stddef.h>
#include "sns_osa_thread.h"
#include "sns_rc.h"

/*=============================================================================
  Type Definitions
  ===========================================================================*/
/* Heap Ids */
typedef enum
{
  SNS_HEAP_MAIN = 0,
  SNS_HEAP_BATCH,
  SNS_HEAP_ISLAND,
  SNS_HEAP_PRAM,
  SNS_HEAP_EVENT,
  SNS_HEAP_MAX,
} sns_mem_heap_id;

/*
 * Memmgr callback function type.
 */
typedef void (*sns_mem_heap_lowmem_cb)(intptr_t args);

/*=============================================================================
  Macros
  ===========================================================================*/

#ifdef SNS_INCREASED_SDC_PRAM
// Reduce the Q6 heap to leave space for SDC buffering in PRAM:
#define SNS_PRAM_HEAP_SIZE 1024
#else
// Increse Q6 heap, leaving little space for SDC buffering in PRAM:
#define SNS_PRAM_HEAP_SIZE (1024*10)
#endif /* SNS_INCREASED_SDC_PRAM */
#define SNS_HEAP_PRAM_NAME "SENSORS"

/*=============================================================================
  Global Data
  ===========================================================================*/

/**
 * peripherial memory, start address and size
 */
extern void *sns_pram_addr;
extern uint32_t sns_pram_size;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/*!
  @brief Initializes all the heaps

  @param [i] pram_heap_head Pointer to the start of the PRAM memory pool
*/
void sns_heap_init(uint8_t *pram_heap_head);

/*!
  @brief Perform delayed heap initialization.  Must be called after *both*
  sns_heap_init and sns_osa_init.
*/
void sns_heap_init_delayed(void);

/*!
  @brief allocates memory from the heap and returns a pointer to the
               newly allocated memory block

  @note The memory is always zeroed out internally.

  @param [i] heapID           heap to allocate from
  @param [i] size                number of bytes required

  @return
   pointer to the block of memory if successful
   NULL if alloc failed
*/
void* sns_malloc(sns_mem_heap_id heap_id, size_t size);

/*!
  @brief free's a block of memory

  @note The memory is not zeroed out in sns_free().

  @param [i] ptr pointer to memory block to be freed
*/
void sns_free(void *ptr);

/*!
  @brief Register to be notified of low memory

  @param [i] heap_id The heap to be monitored
  @param [i] limit Amount of free memory below which to trigger callback
  @param [i] callback The callback function to be registered
  @param [i] args Pointer that will be returned as a parameter to the callback
*/
void sns_mem_heap_register_lowmem_cb(
  sns_mem_heap_id heap_id,
  uint32_t limit,
  sns_mem_heap_lowmem_cb callback,
  intptr_t args);

/*!
  @brief Get the heap ID for a pointer

  @param [i] ptr Pointer to be checked

  @return
  heap id of the ptr if it belongs to a heap managed by sns_memmgr
  SNS_HEAP_MAX otherwise
*/
sns_mem_heap_id sns_memmgr_get_heap_id(void const *ptr);

/*!
  @brief Generate and submit heap usage log packet

  @param [i] cookie Cookie that is used to populate the
                    user_defiend field in the log packet
*/
void sns_memmgr_log_heap_usage(uint64_t cookie);

/*!
  @brief Get the current heap usage

 @param[i] heap_id  Heap whose usage is to be measured

 @return number of bytes malloc'd in the heap
*/
size_t sns_memmgr_get_heap_usage(sns_mem_heap_id heap_id);
