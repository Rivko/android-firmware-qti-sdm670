/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.

Copyright (C) 1997-2009 by QUALCOMM Technologies, Inc.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Header: //components/rel/apr.common/2.0/utils/src/apr_memmgr.c#1 $ $DateTime: 2017/10/06 13:36:34 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/12/13   rm      Changed the implementation to wrap memheap2
08/29/09   ayu     Forked memheap to apr_memmgr because of the policy and
                   lock management interface changes.
07/23/09   ayu     Added APR_MEMMGR_LITE settings for OS independence support.
                   Removed comdef.h and assert.h dependency.
                   Removed explicit QUBE support.
                   Added externally controlled heap access policy.
                   Added externally providable lock and free functions.
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
04/06/04    gr     Fixed a bug in the definition of the BEGIN_CRITICAL_SECTION
                   and END_CRITICAL_SECTION macros.
11/25/03    gr     Fixes for compiler warnings when FEATURE_APR_MEMMGR_DEBUG is
                   turned on.
11/18/03    rj     Added check for apr_memmgr_malloc success before saving header
                   info.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/19/03    gr     Added functionality to store information in flash when
                   an ASSERT fails.
09/17/03    gr     Mainlined FEATURE_APR_MEMMGR_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
07/08/03    gr     Simplified apr_memmgr_heap_task_lock and apr_memmgr_heap_task_free.
06/26/03    gr     Renamed ASSERT to APR_MEMMGR_ASSERT. By default, APR_MEMMGR_ASSERT
                   throws a fatal error and prints out the task id.
06/25/03    gr     Added more ASSERTS. Changed the behavior of
                   FEATURE_APR_MEMMGR_DEBUG to keep just the last n stores and
                   frees. The id of the allocating task is now stored in
                   the info array. Set the block header pad to a non-zero
                   value to try and catch heap corruption.
06/21/03    gr     Added ASSERTS in apr_memmgr_realloc and apr_memmgr_free to catch bad
                   pointers passed in.
06/03/03   kjm     Fixed problem in realloc when block is actually grown.
                   If nextBlock is pointing to the free block merged onto
                   the end of the grown block, nextBlock was failing to
                   get updated to point to the following block.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
01/20/03   lad     Changed critical section macros to use INTLOCK() instead
                   of TASKLOCK().  Featurized ASSERT calls to optimize.
03/22/02    gr     Corrected a bug in apr_memmgr_malloc.
10/10/01    gr     BEGIN_CRITICAL_SECTION and END_CRITICAL_SECTION now map
                   to TASKLOCK and TASKFREE, instead of to rex_enter_crit_sect
                   and rex_leave_crit_sect.
10/01/01    gr     Added support for some basic heap statistics.
11/09/00   jct     Revised for coding standard, integrate with REX critical
                   sections
09/09/99   kjm     Added even more ASSERTs to detect corrupt heaps.
04/22/99   kjm     apr_memmgr_realloc now correctly updates used_bytes, max_used and
                     max_request.
02/17/99   kjm     Added realloc function.  Added support for two
                     variations of heap block headers.  Support backing up
                     and auto-reset when last block is freed to reduce
                     fragmentation.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.cp file.
08/22/97   kjm     Original SubAllocator.cp file created.
============================================================================ */

#define APR_MEMMGR_LITE /* No Rex or any custom OS dependencies. */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "apr_memmgr.h"
/* ------------------------------------------------------------------------
** Externs
** ------------------------------------------------------------------------ */

/* defaults to NULL.  If not NULL, function will be called
   whenever an allocation has failed due to an out of heap
   space condition and is about to return NULL to the caller */
extern apr_memmgr_allocator_failed_proc_type apr_memmgr_allocator_failed_hook;

#if defined(APR_MEMMGR_LITE)
   #define APR_MEMMGR_ASSERT_NONE /* Enable no assertions. */
   #if 0
      #define APR_MEMMGR_ASSERT_WARN /* Enable logging error to DIAG. Uses TCB_SELF(). */
      #define APR_MEMMGR_ASSERT_LOG /* Enable logging error fatal messages to EFS. Not portable; touches Rex, Dog, and EFS or EFS2. Uses TCB_SELF(). */
   #endif /* 0 */
#elif defined(FEATURE_WINCE)
   #include "wce_rex.h"
#else
   #include "rex.h"
#endif

#ifdef FEATURE_APR_MEMMGR_DEBUG
  #include "crc.h"
#endif

#ifdef FEATURE_WINCE
  #define APR_MEMMGR_ASSERT_WARN
#endif /* FEATURE_WINCE */

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
enum {
  apr_memmgr_kBlockFree = -1,
  apr_memmgr_kBlockUsed = 0,
  apr_memmgr_kLastBlock = -1,
  apr_memmgr_kMinChunkSize = 16
};


typedef mem_heap_block_iterator_type apr_memmgr_heap_block_iterator_type;


typedef mem_heap_totals_type apr_memmgr_heap_totals_type;


typedef mem_block_header_type apr_memmgr_block_header_type;

typedef mem_heap_type apr_memheap_type;
/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */

/* NOTUSED */
#define NOTUSED(i) if(i){}

#ifdef APR_MEMMGR_LITE
   #define TCB_TYPE    int
   #define TCB_SELF()  0
#else
   #define TCB_TYPE    rex_tcb_type*
   #define TCB_SELF()  rex_self()
#endif /* APR_MEMMGR_LITE */

#if 0 /* Need to remove compilation warning. */
#ifdef APR_MEMMGR_LITE
   #define ERR( fmt, a, b, c ) { ((void*)NULL) = 0; /* Poor man's error fatal (data abort). */ }
   #define ERR_FATAL( fmt, a, b, c ) ERR( fmt, a, b, c );
#else
   #include "err.h"
#endif /* APR_MEMMGR_LITE */
#endif

/* By default, APR_MEMMGR_ASSERT will generate a fatal error.
*/
#ifdef APR_MEMMGR_ASSERT_NONE
   #define APR_MEMMGR_ASSERT( xx_exp ) ((void) 0)
   #define APR_MEMMGR_ASSERT_EX( xx_exp, xx_file, xx_line ) \
      APR_MEMMGR_ASSERT( xx_exp )
#elif defined( APR_MEMMGR_ASSERT_WARN )

   #ifndef FEATURE_WINCE

   #define APR_MEMMGR_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        ERR( "In task 0x%x, Assertion " #xx_exp " failed", \
           (unsigned long) TCB_SELF(), 0, 0 ); \
     }

   #else /* FEATURE_WINCE */

     #define APR_MEMMGR_ASSERT( xx_exp ) \
       if( !(xx_exp) ) \
       { \
          ERR( "In task 0x%x, Assertion  %d failed", \
             (unsigned long) TCB_SELF(), xx_exp , 0 ); \
       }

   #endif /* FEATURE_WINCE */

   #define APR_MEMMGR_ASSERT_EX( xx_exp, xx_file, xx_line ) \
      APR_MEMMGR_ASSERT( xx_exp )
#else
      #define apr_memmgr_log_error(a, b, c)

   #define APR_MEMMGR_ASSERT( xx_exp ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        apr_memmgr_log_error( #xx_exp, "", 0 ); \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

   #define APR_MEMMGR_ASSERT_EX( xx_exp, xx_file, xx_line ) \
     if( !(xx_exp) ) \
     { \
        TCB_TYPE  self = TCB_SELF();                             \
        apr_memmgr_log_error( #xx_exp, xx_file, xx_line ); \
        ERR_FATAL( "In task 0x%x, Assertion " #xx_exp " failed", \
                   (unsigned long) self, 0, 0 );                 \
     }

#endif

typedef void (*apr_memheap_allocator_failed_proc_type)(
   struct mem_heap_struct *heap_ptr,
   size_t                  request_size
   );

/*===========================================================================
FUNCTION APR_MEMMGR_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  infail_fnc_ptr may be NULL in which case no function will
  be called if apr_memmgr_malloc or apr_memmgr_calloc is about to fail.  If infail_fnc_ptr
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(apr_memmgr_init_heap,1p,2p,2P>=3n) */
void apr_memmgr_init_heap(
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
)
{
	/* TODO: -- deny_fnc_ptr  --*/
	void *heap_wrapper_ptr = (void *)APR_MEM_ALIGN_64(heap_ptr);
	APR_MEMMGR_ASSERT(heap_wrapper_ptr);
	mem_init_heap((apr_memheap_type *)heap_wrapper_ptr,heap_mem_ptr,heap_mem_size,(apr_memheap_allocator_failed_proc_type)fail_fnc_ptr);

} /* END apr_memmgr_init_heap */

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
   void* apr_memmgr_malloc_debug(
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
   )
#else
   void* apr_memmgr_malloc(
      apr_memmgr_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t           size
         /* Number of bytes to allocate
         */
   )
#endif
{
	return mem_malloc((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr),size);

} /* END apr_memmgr_malloc */


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
   void apr_memmgr_free_debug(
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
   )
#else
   void apr_memmgr_free(
      apr_memmgr_type *heap_ptr,
         /* Heap in which to free memory
         */
      void            *ptr
         /* Memory to free
         */
   )
#endif
{
	mem_free((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr),ptr);
} /* END apr_memmgr_free */


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
   void* apr_memmgr_realloc_debug(
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
   )
#else
   void *apr_memmgr_realloc(
      apr_memmgr_type *heap_ptr,
         /* Heap in which to re-allocate
         */
      void            *ptr,
         /* A block previously allocated from heap_ptr
         */
      size_t           size
         /* New size (in bytes) of the ptr block of memory
         */
   )
#endif
{
	return mem_realloc((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr),ptr,size);
} /* END apr_memmgr_realloc */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_USED

DESCRIPTION
  Returns the number of allocated bytes, excluding overhead.
===========================================================================*/
size_t apr_memmgr_heap_used (apr_memmgr_type *heap_ptr)
{
	return mem_heap_used((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr));
}

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_OVERHEAD

DESCRIPTION
  Returns the number of bytes of overhead, per allocation, for the heap.
===========================================================================*/
size_t apr_memmgr_heap_overhead (apr_memmgr_type *heap_ptr)
{
	return mem_heap_overhead((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr));
}

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_CONSUMED

DESCRIPTION
  Returns the number of bytes consumed by allocation and overhead.
===========================================================================*/
size_t apr_memmgr_heap_consumed (apr_memmgr_type *heap_ptr)
{
	return mem_heap_consumed((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr));
    }

/*===========================================================================
FUNCTION APR_MEMMGR_GET_BLOCK_LOGICAL_SIZE

DESCRIPTION
  Returns the logical block size of a memory block in the heap

  Returns the size (in bytes) of the block excluding block header overhead and
  unused bytes at the end of the block.
===========================================================================*/
/*lint -sem(apr_memmgr_get_block_logical_size,1p) */
unsigned long apr_memmgr_get_block_logical_size(
   const apr_memmgr_block_header_type *block_hdr_ptr
     /*  A memory block to get the logical size of
     */
)
{
	/* TODO: signature difference??? */
	return mem_get_block_logical_size((const apr_memmgr_block_header_type *)block_hdr_ptr,NULL);
} /* apr_memmgr_get_block_logical_size */

/* ------------------------------------------------------------------------
**
** Routines that return information about the heap
**
** ------------------------------------------------------------------------ */

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_BLOCK_ITERATOR_INIT

DESCRIPTION
  Initializes an iterator structure to be used for walking the blocks
  in the specified heap.
===========================================================================*/
void apr_memmgr_heap_block_iterator_init(
   apr_memmgr_heap_block_iterator_type *block_iter_ptr,
     /*  The Iterator structure to be initialized
     */
   const apr_memmgr_type               *heap_ptr
     /*  The heap this iterator should iterate over
     */
)
{
	/* TODO: signature difference??? */
	mem_heap_block_iterator_init((mem_heap_block_iterator_type *)block_iter_ptr,NULL);
} /* apr_memmgr_heap_block_iterator_init */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_BLOCK_ITERATOR_RESET

DESCRIPTION
  Reset a heap block iterator to begin the iteration of heap blocks from
  the beginning.
===========================================================================*/
void apr_memmgr_heap_block_iterator_reset(
   apr_memmgr_heap_block_iterator_type *block_iter_ptr
     /* The iterator structure to be reset
     */
)
{
	mem_heap_block_iterator_reset((mem_heap_block_iterator_type *)block_iter_ptr);
} /* apr_memmgr_heap_block_iterator_reset */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_BLOCK_ITERATOR_NEXT

DESCRIPTION
  Return a memory block header for the next block (or next virtual block)
  in the heap being iterated.  Description is only valid if the function
  returns a non-zero value.

  The heap being iterated should not be changed during the iteration as the
  iterator may get lost or return bogus values.  There is no protection
  against this however.

  Returns:
      zero      There are no more heap blocks to return
      non-zero  The next heap block was returned in outBlock successfully
===========================================================================*/
int apr_memmgr_heap_block_iterator_next(
   apr_memmgr_heap_block_iterator_type *block_iter_ptr,
     /* Iterator to return next iterated block from
     */
   apr_memmgr_block_header_type        *out_block_ptr,
     /* Pointer to a location where a copy of a memory block header
        is placed describing the returned memory block.  Only
        valid if the function returns a non-zero value.
     */
   int                           fake_free_coalesced
     /* Input flag.  zero => return the next block header from the heap
        exactly as it is.  non-zero => return the next virtual block
        header from the heap.  Virtual block headers are created by
        collapsing all adjacent free blocks into one.
     */
)
{
	return mem_heap_block_iterator_next((mem_heap_block_iterator_type *)block_iter_ptr, 
		                          (mem_block_header_type *) out_block_ptr, 
								  fake_free_coalesced);
} /* apr_memmgr_heap_block_iterator_next */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_GET_TOTALS

DESCRIPTION
  Returns heap totals for the heap.  This function IS thread safe.
  Heap must already have been initialized.
  Return totals for the heap.
===========================================================================*/
void apr_memmgr_heap_get_totals(
   apr_memmgr_type             *heap_ptr,
      /* The heap to return totals for
      */
   apr_memmgr_heap_totals_type *totals
      /* A pointer to a totals structure to receive the various
         heap information
      */
)
{
	mem_heap_get_totals((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr),(mem_heap_totals_type *) totals);
} /* apr_memmgr_heap_get_totals */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_SET_LOCK

DESCRIPTION
  This function sets up the specified heap to use (interrupt) locking and
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
apr_memmgr_heap_set_lock(
   apr_memmgr_type          *heap_ptr,
   apr_memmgr_lock_fnc_type  lock_fnc_ptr,
   apr_memmgr_free_fnc_type  free_fnc_ptr
)
{
   apr_memheap_type *heap_wrapper_ptr = (apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr);
   APR_MEMMGR_ASSERT (heap_wrapper_ptr != NULL);
#ifdef FEATURE_APR_MEMMGR_MT
   if ( ( heap_ptr != NULL ) &&
        ( ( lock_fnc_ptr != NULL ) && ( free_fnc_ptr != NULL ) ) )
   {
      heap_wrapper_ptr->lock_fnc_ptr = lock_fnc_ptr;
      heap_wrapper_ptr->free_fnc_ptr = free_fnc_ptr;
      return 1;
   }
   else
      return 0;
#else
   return 1;
#endif
} /* END apr_memmgr_heap_set_int_lock */


/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_SET_NO_LOCK

DESCRIPTION
  This function sets up the specified heap to use no locking. This implies
  that this heap should only be used from the context of one task.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
apr_memmgr_heap_set_no_lock( apr_memmgr_type *heap_ptr )
{
   apr_memheap_type *heap_wrapper_ptr = (apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr);
   APR_MEMMGR_ASSERT (heap_wrapper_ptr != NULL);

#ifdef FEATURE_APR_MEMMGR_MT
   if ( heap_ptr != NULL )
   {
      heap_wrapper_ptr->lock_fnc_ptr = NULL;
      heap_wrapper_ptr->free_fnc_ptr = NULL;
      return 1;
   }
   else
      return 0;
#else
   return 1;
#endif

} /* END apr_memmgr_heap_set_no_lock */

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_GET_FREEBLOCK_INFO

DESCRIPTION
  Returns information about the sizes of the largest n free blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
apr_memmgr_heap_get_freeblock_info(
  apr_memmgr_type *heap_ptr,
  unsigned int     num_blocks,
  unsigned int    *buf
)
{
	mem_heap_get_freeblock_info((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr),num_blocks,buf);
} /* apr_memmgr_heap_get_freeblock_info */

/*===========================================================================
FUNCTION APR_MEMMGR_HEAP_GET_CHUNK_SIZE

DESCRIPTION
  Returns the minimum chunk size used for allocations from a heap. All
  allocations from a specific heap are multiples of this size.
===========================================================================*/
unsigned int
apr_memmgr_heap_get_chunk_size( apr_memmgr_type *heap_ptr )
{
	return mem_heap_get_chunk_size((apr_memheap_type *)APR_MEM_ALIGN_64(heap_ptr));
} /* END apr_memmgr_heap_get_chunk_size */

