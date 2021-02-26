/*==============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#ifndef RPCMEM_H
#define RPCMEM_H

#include "AEEStdDef.h"

#define RPCMEM_HEAP_DEFAULT 0x80000000
#define RPCMEM_HEAP_NOREG   0x40000000
#define RPCMEM_HEAP_UNCACHED 0x20000000

#ifdef __cplusplus
extern "C" {
#endif

/**
 * call once to initialize the library
 */
void rpcmem_init(void);
/**
 * call once for cleanup
 */
void rpcmem_deinit(void);

/**
 * allocate via ION a buffer of size
 * @heapid, the heap id to use
 * @flags, ion flags to use to allocate, this is ignored on ics
 * @size, the buffer size to allocate
 * @retval, 0 on failure, pointer to buffer on success
 *
 * Pass RPCMEM_HEAP_DEFAULT for flags if unsure on what heapid
 * and flags to pass. RPCMem internally takes care of picking
 * the right heap id and flags value. For example:
 * buf = rpcmem_alloc(0, RPCMEM_HEAP_DEFAULT, size);
 */

void* rpcmem_alloc(int heapid, uint32 flags, int size);

/**
 * free buffer, ignores invalid buffers
 */
void rpcmem_free(void* po);

/**
 * returns associated fd
 */
int rpcmem_to_fd(void* po);

#ifdef __cplusplus
}
#endif

#endif //RPCMEM_H
