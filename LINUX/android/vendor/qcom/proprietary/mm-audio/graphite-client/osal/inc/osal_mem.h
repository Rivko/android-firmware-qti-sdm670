/*
 * osal_mem.h
 *
 * This header file declared OS-specific memory operations.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __OSAL_MEM_H__
#define __OSAL_MEM_H__

/*
 * osal_mem_alloc - Allocate memory of a certain size
 *
 * @size:    size of the memory block to be allocated.
 *
 * Prerequisites
 *    None
 */
void *osal_mem_alloc(size_t size);

/*
 * osal_mem_zalloc - Allocate memory of a certain size and fill
 *                   with value '0'.
 *
 * @size:    size of the memory block to be allocated.
 *
 * Prerequisites
 *    None
 */
void *osal_mem_zalloc(size_t size);

/*
 * osal_mem_calloc - Allocate memory for an array of items, and
 *                   fill with value '0'.
 *
 * @items:    number of items in the array to be allocate.
 * @size:     size of each item in the array.
 *
 * Prerequisites
 *    None
 */
void *osal_mem_calloc(size_t items, size_t size);

/*
 * osal_mem_realloc - Change size of a memory block
 *
 * @ptr:     pointer to the memory block. If NULL, allocate new memory.
 * @size:    size of the new block. If '0', free the memory pointed by ptr.
 *
 * Prerequisites
 *    None
 */
void *osal_mem_realloc(void *ptr, size_t size);

/*
 * osal_mem_free - Free/Deallocate a block of memory
 *
 * @ptr:    pointer to the memory block to be freed/deallocated.
 *
 * Prerequisites
 *    None
 */
void osal_mem_free(void *ptr);

/*
 * osal_mem_cpy - Copy a memory area
 *
 * @dst:         pointer to the destination memory block
 * @dst_size:    size of the destination memory block
 * @src:         pointer to the source memory block
 * @src_size:    size of the source memory block
 *
 * Prerequisites
 *    @dst and @src should not be NULL
 */
void *osal_mem_cpy(void *dst, size_t dst_size, void *src, size_t src_size);

/*
 * osal_mem_set - Fill memory with a constant value
 *
 * @ptr:     pointer to the memory block to be filled
 * @val:     constant value to be filled in the memory block
 * @size:    size of the memory block to be filled with @val.
 *
 * Prerequisites
 *    @ptr should not be NULL
 */
void *osal_mem_set(void *ptr, int val, size_t size);

#endif /*  __OSAL_MEM_H__ */
