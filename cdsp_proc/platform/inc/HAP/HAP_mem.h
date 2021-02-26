#ifndef HAP_MEM_H
#define HAP_MEM_H
/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "AEEStdDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAP_MEM_CONTEXT_DEFAULT (void*)0

/*
 * Protections are chosen from these bits, or-ed together
 */
#define HAP_PROT_NONE   0x00    /* no permissions */
#define HAP_PROT_READ   0x01    /* pages can be read */
#define HAP_PROT_WRITE  0x02    /* pages can be written */
#define HAP_PROT_EXEC   0x04    /* pages can be executed */

int HAP_cx_malloc(void* pctx, uint32 bytes, void** pptr);
int HAP_cx_free(void* pctx, void* ptr);

static inline int HAP_malloc(uint32 bytes, void** pptr)
{
  return HAP_cx_malloc(HAP_MEM_CONTEXT_DEFAULT, bytes, pptr);
}

static inline int HAP_free(void* ptr)
{
  return HAP_cx_free(HAP_MEM_CONTEXT_DEFAULT, ptr);
}

struct HAP_mem_stats {
   uint64 bytes_free; /** number of bytes free from all the segments,
                        * may not be available for a single alloc
                        */
   uint64 bytes_used; /** number of bytes used */
   uint64 seg_free;   /** number of segments free */
   uint64 seg_used;   /** number of segments used */
   uint64 min_grow_bytes; /** minimum number of bytes to grow the heap by when creating a new segment */
};

/**
 * Get the current statistics from the heap.
 * 
 * @param stats, the stats structure
 * @retval, 0 on success
 */
int HAP_mem_get_stats(struct HAP_mem_stats *stats);

/**
 * Set the minimum and maximum grow size.
 *
 * The heap will grow the memeory by at least min and at most max.
 *
 * @param min, minimum bytes to grow the heap by when requesting a new segment
 * @param max, maximum bytes to grow the heap by when requesting a new segment
 * @retval, 0 on success
 *
 * min_grow_bytes = MIN(max,MAX(min,min_grow_bytes));
 *
 * The value will be aligned to the next 1MB boundary.
 *
 * actual_grow_bytes = min_grow_bytes + request_size
 * actual_grow_bytes = ALIGN(actual_grow_bytes,0x100000)
 *
 * default is HAP_mem_set_grow_size(0x100000/2, MAX_UINT64);
 */
int HAP_mem_set_grow_size(uint64 min, uint64 max);

/**
 * Map buffer associated with the file descriptor to memory. The reference
 * count gets incremented if the file descriptor is already mapped.
 *
 * @param addr, mapping at fixed address
 * @param len, size of the buffer to be mapped
 * @param prot, protection flags - Read, Write, Execute
 * @param flags, HAP_MAP_NO_MAP - Increment reference count with no mapping
 *               0 - map the buffer and increment the reference count
 * @param fd, file descriptor for the buffer
 * @param offset, offset into the buffer
 * @retval, mapped address or 0 if HAP_MAP_NO_MAP flag set on success
 *          -1 on failure
 */
void* HAP_mmap(void *addr, int len, int prot, int flags, int fd, long offset);

/**
 * Unmaps the buffer from memory if the reference count goes to 0.
 *
 * @param addr, mapped address
 * @param len, size of the mapped buffer
 * return, 0 on success
 */
int HAP_munmap(void *addr, int len);

/**
 * Get virtual and physical address associated with the buffer and increments
 * the reference count.
 *
 * @param fd, file descriptor for the buffer
 * @param vaddr, virtual address associated with the buffer
 * @param paddr, physical address associated with the buffer
 * @retval, 0 on success
 */
int HAP_mmap_get(int fd, void **vaddr, uint64 *paddr);

/**
 * Decrements the reference count of the file descriptor.
 *
 *@param fd, file descriptor of the buffer
 *@retval, 0 on success
 */
int HAP_mmap_put(int fd);

#ifdef __cplusplus
}
#endif

#endif // HAP_MEM_H

