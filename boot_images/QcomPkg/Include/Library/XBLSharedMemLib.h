/** @file XBLSharedMemLib.h
   
  Contains XBL Shared Memory Lib interface

  Copyright (c) 2014, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/11/14   niting  New file

=============================================================================*/
#ifndef __XBLSHAREDMEMLIB_H__
#define __XBLSHAREDMEMLIB_H__

#include <Uefi.h>

/* Initialize or queries context to shared memory manager. This context
 * must be passed when allocating memory via XBLAllocSharedMem.
 * Region string designates the region name to be initialized or queried.
 * If initializing, MemPool and MemPoolSize must be passed.
 * If querying, MemPool and MemPoolSize must be NULL.
 *
 * IN:
 * Region         : Null terminated Ascii string to memory region. Maximum 16-bytes.
 * MemPool        : Pointer to Memory that can be used for allocations
 * MemPoolSize    : Size of the memory pool passed in bytes
 *
 * Return
 *   Context Pointer on success, that needs to be passed in for alloc
 *   Context Pointer if already allocated (MemPool/MemPoolSize will be ignored)
 *   NULL  : If invalid pointers passed
 *   NULL  : If no more context slots are available for allocations
 *   NULL  : Any other error
 *   
 * */
VOID* XBLInitSharedMemMgr (CHAR8            *Region,
                           UINT8            *MemPool, 
                           UINTN            *MemPoolSize);

/* Allocate required amount of memory using the associated Id. This Id
 * can be up to a 16-byte NULL terminated string. If the input string is
 * greater than 16-bytes, the Id will be truncated to 16 bytes with the last
 * byte as NULL terminator. If the Id is found to be already allocated in the
 * memory Ctxt, then a pointer to the existing allocation will be
 * returned and the Size parameter will be updated to the existing
 * allocation. This memory size cannot be updated once allocated.
 *
 * Ctxt       : Context of the memory allocator
 * Name       : Null terminated Ascii string associated with allocation
 * Size       : Amount of memory requested. On output, size of memory
 *              allocated. If not enough memory available, then this will
 *              return the available memory left and a NULL pointer will
 *              be returned by the function. Size of allocation will be
 *              rounded up to nearest alignment boundary.
 *
 * Returns the pointer to memory location that's allocated on success
 * NULL on failure or invalid inputs
 * */
UINT8* XBLAllocSharedMem (VOID*   Ctxt, 
                          CHAR8  *Name,
                          UINTN  *Size);

#endif /* __XBLSHAREDMEMLIB_H__ */

