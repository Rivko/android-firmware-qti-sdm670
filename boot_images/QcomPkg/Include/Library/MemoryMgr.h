/** @file MemoryMgr.h
   
  Contains Memory Manager functions interface

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/25/11   yg      New file

=============================================================================*/
#ifndef __MEM_MGR_H__
#define __MEM_MGR_H__

#include <Uefi.h>

/* Default Alignment */
#ifndef DEFAULT_MEM_ALIGNMENT
  #define DEFAULT_MEM_ALIGNMENT  32
#endif

extern VOID* MemCtx;

/* Initialize memory manager by providing the memory pool to manage
 *
 * IN:
 * MemPool        : Pointer to Memory that can be used for allocations
 * MemPoolSize    : Size of the memory pool passed in bytes
 *
 * Return
 *   Context Pointer on success, that need to be passed in for alloc or free
 *   NULL  : On Failure
 *   
 * */
VOID* InitializeMemMgr (UINT8            *MemPool, 
                        UINT32           MemPoolSize);

/* Allocate required amount of memory. This memory can be freed if the memory is
 * freed in reverse order (LIFO, ie last allocated first free)
 *
 * Ctxt       : Context of the memory allocator
 * Size       : Amount of memory requested
 *
 * Returns the pointer to memory location that's allocated on success
 * NULL on failure
 * */
UINT8* AllocMem (VOID* Ctxt, UINT32 Size);

/* Free allocated memory. If this chunk of memory was the last allocated
 * then it can be freed.
 *
 * Ctxt   : Context of the memory allocator
 * MemPtr : Memory that needs to be freed
 * Size   : Size of the memory being freed
 *
 * Returns 
 *   The pointer to memory if freed successfully
 *   NULL on failure.
 *
 * */
UINT8* FreeMem (VOID* Ctxt, UINT8* MemPtr, UINT32 Size);

#endif /* __MEM_MGR_H__ */


