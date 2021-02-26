#ifndef APR_MEMMGR_H
#define APR_MEMMGR_H
/*===========================================================================

DESCRIPTION
  A simple sub-allocator to manage memory allocations and deallocations
  using a Next Fit strategy.

  Note that these routines are FULLY re-entrant.  Furthermore, while
  performing memory allocation/deallocation calls on one heap, the routines
  may be interrupted to perform memory allocation/deallocation calls on
  different heaps without blocking or corruption.  However, should an
  interrupting task attempt to perform memory allocation/deallocation
  on the same heap that had a critical section interrupted, it will block
  allowing the first call to finish.  All this is accomplished by giving
  each heap its own semaphore.

  FEATURE_APR_MEMMGR_MT
    Activates multi threading support for the heap

  Usage Notes - Avoiding Fragmentation

  1. If all bytes in the heap are freed, the heap is guaranteed to be returned
     to the same state as after a call to apr_memmgr_init_heap, except that the
     values of maxUsed & maxRequest are not reset.  The order in which blocks
     are freed is irrelevant so long as they ALL are freed.

  2. After n consecutive malloc/calloc calls, in a heap with non-fragmented
     free space (and no intervening realloc calls that return a new pointer),
     calling free in the reverse order on the n blocks guarantees that the heap
     will be returned to the exact same state as before the n alloc calls.

  3. FEATURE_APR_MEMMGR_HEAP_SMALLER_OVERHEAD == 0
     (2) still applies, except the calls to free may be made in any order and
     after all n blocks are freed, the heap is guaranteed to be returned to the
     exact same state as prior to the n alloc calls.

  4. FEATURE_APR_MEMMGR_HEAP_SMALLER_OVERHEAD == 1
     The overhead per heap block is 8 bytes.  However it's not possible to backup
     the next-block-to-start-searching-for-free-space pointer, so the heap MAY
     become more fragmented, but it MAY NOT -- it depends on the alloc/free
     calling patterns.

  5. FEATURE_APR_MEMMGR_HEAP_SMALLER_OVERHEAD == 0
     The overhead per heap block is 12 bytes.  Backing up is possible, so some
     fragmention that otherwise MIGHT occur can be prevented.  There is very little
     performance cost for this, the big expense is the additional 4 bytes of overhead
     per heap block.

Copyright (C) 1997-2009 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.h_v   1.1   07 Mar 2002 18:48:56   rajeevg  $
$Header: //components/rel/apr.common/2.0/utils/inc/apr_memmgr.h#1 $ $DateTime: 2017/10/06 13:36:34 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/12/13   rm     Changed the implementation to wrap memheap2
08/29/09   ayu     Forked memheap to apr_memmgr because of the policy and
                   lock management interface changes.
07/23/09   ayu     Added APR_MEMMGR_LITE settings for OS independence support.
                   Removed comdef.h and assert.h dependency.
                   Removed explicit QUBE support.
                   Added externally controlled heap access policy.
                   Added externally providable lock and free functions.
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/17/03    gr     Mainlined FEATURE_APR_MEMMGR_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
10/01/01    gr     Added support for some basic heap statistics.
11/17/00   crr     Change stdlib.h to stddef.h for size_t def. Add forward
                     decl for struct apr_memmgr_struct.
11/09/00   jct     Match to coding convention, remove of semaphore use, replace
                   with REX critical sections, remove stddef.h inclusion
02/17/99   kjm     Added realloc function.  Minor additions to
                     apr_memmgr_struct.  Added giant function descriptions.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.h file.
08/22/97   kjm     Original SubAllocator.h file created.
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include <stddef.h>
#include "apr_memmgr_config.h"
#include "apr_comdef.h"
#include "memheap.h"
/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define APR_MEMMGR_VERSION (0x0200)    /* 2.0 */

/* This defines the amount of opaque data needed to be statically assigned
 * for the heap header
*/
#if defined(FEATURE_MEM_DEBUG_COMMON)
#define APR_MEMMGR_OPAQUE_DATASIZE ( 9000 )
#else
#define APR_MEMMGR_OPAQUE_DATASIZE ( 2048 )
#endif

#define APR_MEM_ALIGN_64(x) ( (uint32_t)((char_t*)x + 7) & (~7) )

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

/**
  Opaque heap structure. The internal heap mgr stores
  data in this structure. 
  @note Please treat this as a read only structure. 
  @note There is nothing in this structure 
        that can be read by a client
*/
typedef struct apr_memmgr_type{
	char opaque_data[APR_MEMMGR_OPAQUE_DATASIZE];
}apr_memmgr_type;

/* Allocation Failed Procedure
**   This function is called when there is not enough room in ioHeap to satisfy
**   a request of inRequest bytes.  This routine should free up any memory it
**   can before returning.  The request will then be tried again.  There are
**   no restrictions on what this function may do including memory allocation/
**   deallocation calls.  Note that before calling this function, calls to
**   the allocator failed proc on ioHeap are disabled.  After this function
**   returns, calls are re-enabled.  This does NOT affect other heaps, so if
**   multiple heaps share the same allocator failed proc, it should be
**   re-entrant.
*/

typedef void (*apr_memmgr_allocator_failed_proc_type)(
   struct apr_memmgr_type *heap_ptr,
      /* Heap on which allocation is attempted
      */
   size_t                    request_size
      /* Size of failed request
      */
);

/* The heap is protected by deny access policy routine. The policy can be
** used to prevent accesses such as deny heap operations in ISR context. A
** non-zero return value denies access, otherwise allows access.
*/
typedef int (*apr_memmgr_heap_deny_access_fnc_type)( void );

/* Each heap can have its own function that is used to lock the heap and
** free the heap.  mem_lock_fnc_type and mem_free_fnc_type are the types
** of these functions.
*/
/**
  Provides an abstraction so each function can have its own function to 
  lock the heap.

  @param[in] ptr Pointer to what is to be locked.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*apr_memmgr_lock_fnc_type)( void * ptr);

/**
  Provides an abstraction so each function can have its own function to 
  free the heap.

  @param[in] ptr Pointer to what is to be freed.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*apr_memmgr_free_fnc_type)( void * ptr);

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_INIT

DESCRIPTION
  Initializes the ioHeap object and sets up inMemoryChunk for use with the
  ioHeap object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  inFailProc may be NULL in which case no function will
  be called if apr_memmgr_malloc or apr_memmgr_calloc is about to fail.  If inFailProc
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(apr_memmgr_init_heap,1p,2p,2P>=3n) */
APR_INTERNAL void apr_memmgr_init_heap(
   apr_memmgr_type                       *heap_ptr,
      /* Statically allocated heap structure
      */
   void                                  *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                          heap_mem_size,
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */
   apr_memmgr_allocator_failed_proc_type  fail_fnc_ptr,
      /* Function to call when allocation fails, can be NULL
      */
   apr_memmgr_heap_deny_access_fnc_type   deny_fnc_ptr
      /* Function to call whenever heap access is required, can be NULL
      */
);

/*===========================================================================
FUNCTION APR_MEMMGR_CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(apr_memmgr_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
#ifdef FEATURE_APR_MEMMGR_DEBUG
   APR_INTERNAL void* apr_memmgr_calloc_debug(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t           elt_count,
         /* Number of elements to allocate
         */
      size_t           elt_size,
         /* Size of each element
         */
      char            *file_name,
         /* Name of file from which apr_memmgr_calloc was called
         */
      unsigned int     line_number
         /* Line number corresponding to apr_memmgr_calloc call
         */
   );
#else
   APR_INTERNAL void* apr_memmgr_calloc(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t           elt_count,
         /* Number of elements to allocate
         */
      size_t           elt_size
         /* Size of each element
         */
   );
#endif

#ifdef FEATURE_APR_MEMMGR_DEBUG
/*lint -save -e683 */
   #define  apr_memmgr_calloc(heap_ptr, elt_count, elt_size)\
      apr_memmgr_calloc_debug(heap_ptr,\
                              elt_count,\
                              elt_size,\
                              (char *) __FILENAME__,\
                              (unsigned int) __LINE__)

/*lint -restore */
#endif

/*===========================================================================
FUNCTION APR_MEMMGR_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(apr_memmgr_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
#ifdef FEATURE_APR_MEMMGR_DEBUG
   APR_INTERNAL void* apr_memmgr_malloc_debug(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t           size,
         /* Number of bytes to allocate
         */
      char            *file_name,
         /* Name of file from which apr_memmgr_malloc was called
         */
      unsigned int     line_number
         /* Line number corresponding to apr_memmgr_malloc call
         */
   );
#else
   APR_INTERNAL void* apr_memmgr_malloc(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t           size
         /* Number of bytes to allocate
         */
   );
#endif

#ifdef FEATURE_APR_MEMMGR_DEBUG
/*lint -save -e683 */
   #define  apr_memmgr_malloc(heap_ptr, size)\
      apr_memmgr_malloc_debug(heap_ptr,\
                              size,\
                              (char *) __FILENAME__,\
                              (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION APR_MEMMGR_REALLOC

DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like apr_memmgr_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like apr_memmgr_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to apr_memmgr_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call apr_memmgr_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===========================================================================*/
/*lint -sem(apr_memmgr_realloc,1p,3n>=0&&(@p==0||@P==3n)) */
#ifdef FEATURE_APR_MEMMGR_DEBUG
   APR_INTERNAL void* apr_memmgr_realloc_debug(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      void            *ptr,
         /* A block previously allocated from heap_ptr
         */
      size_t           size,
         /* Number of bytes to allocate
         */
      char            *file_name,
         /* Name of file from which apr_memmgr_realloc was called
         */
      unsigned int     line_number
         /* Line number corresponding to apr_memmgr_realloc call
         */
   );
#else
   APR_INTERNAL void *apr_memmgr_realloc(
      apr_memmgr_type *heap_ptr,
         /* Heap in which to re-allocate
         */
      void            *ptr,
         /* A block previously allocated from heap_ptr
         */
      size_t           size
         /* New size (in bytes) of the ptr block of memory
         */
   );
#endif

#ifdef FEATURE_APR_MEMMGR_DEBUG
/*lint -save -e683 */
   #define  apr_memmgr_realloc(heap_ptr, ptr, size)\
      apr_memmgr_realloc_debug(heap_ptr,\
                               ptr,\
                               size,\
                               (char *) __FILENAME__,\
                               (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION APR_MEMMGR_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
/*lint -sem(apr_memmgr_free,1p) */
#ifdef FEATURE_APR_MEMMGR_DEBUG
   APR_INTERNAL void apr_memmgr_free_debug(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      void            *ptr,
         /* Memory to free
         */
      char            *file_name,
         /* Name of file from which apr_memmgr_free was called
         */
      unsigned int     line_number
         /* Line number corresponding to apr_memmgr_free call
         */
   );
#else
   APR_INTERNAL void apr_memmgr_free(
      apr_memmgr_type *heap_ptr,
         /* Heap in which to free memory
         */
      void            *ptr
         /* Memory to free
         */
   );
#endif

#ifdef FEATURE_APR_MEMMGR_DEBUG
/*lint -save -e683 */
   #define  apr_memmgr_free(heap_ptr, ptr)\
      apr_memmgr_free_debug(heap_ptr,\
                            ptr,\
                            (char *) __FILENAME__,\
                            (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_SET_LOCK

DESCRIPTION
  This function sets up the specified heap to use (interrupt) locking and
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
   APR_INTERNAL int apr_memmgr_heap_set_lock(
   apr_memmgr_type          *heap_ptr,
   apr_memmgr_lock_fnc_type  lock_fnc_ptr,
   apr_memmgr_free_fnc_type  free_fnc_ptr
);

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_SET_NO_LOCK

DESCRIPTION
  This function sets up the specified heap to use no locking. This implies
  that this heap should only be used from the context of one task.
  Returns 1 on success and 0 on failure.
===========================================================================*/
   APR_INTERNAL int apr_memmgr_heap_set_no_lock( apr_memmgr_type *heap_ptr );

#ifdef __cplusplus
   }
#endif

#endif /* APR_MEMMGR_H */

