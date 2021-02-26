#ifndef MEMHEAP_H
#define MEMHEAP_H

/*=============================================================================

GENERAL DESCRIPTION
  Implements memory manager functions

REGIONAL FUNCTIONS


EXTERNALIZED FUNCTIONS

   memheap_init
   memheap_deinit
   mem_malloc
   mem_free


INITIALIZATION AND SEQUENCING REQUIREMENTS

    memheap_init() must be called once before any other mem_malloc.

Copyright (c) 2015-2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

.$Id: //components/rel/ssc.slpi/3.2/utils/memory/inc/sns_memheap_lite.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/09/15   gp     File created.

=============================================================================*/
/** @addtogroup utils_memory
@{ */

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "com_dtypes.h"
#include "sns_osa_lock.h"

#define FEATURE_MEM_DEBUG

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */

/** @cond
*/


/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

/* Doxygen comments for this struct are in typedef struct mem_heap_struct */
struct mem_heap_struct;


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
typedef void (*mem_lock_fnc_type)( void * ptr);

/**
  Provides an abstraction so each function can have its own function to
  free the heap.

  @param[in] ptr Pointer to what is to be freed.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*mem_free_fnc_type)( void * ptr);

/** @brief Header for each memory block allocated. The header describes the
  properties of the block allocated to the client. Note that the size of this
  structure must be a multiple of the largest alignment size required by the
  host CPU.
*/
typedef struct sns_mem_block_header_type {
  uint16        header_guard;

  unsigned char extra;        /**< Extra bytes at the end of a block. */
  char          free_flag:4;  /**< Flag to indicate if this memory block
                                   is free. */
  char          last_flag:4;/**< Flag to indicate if this is the last block
                                   in the allocated section. */
  uint32 forw_offset; /**< Forward offset. The value of the offset
                                   includes the size of the header and the
                                   allocated block. */

  struct sns_mem_block_header_type *stack_next; /**< Next ptr used by the memheap stack*/

#ifdef FEATURE_MEM_DEBUG
  void          *caller_ptr;
  uint32        tid;
#endif

}sns_mem_block_header_type;



#ifndef MEMHEAP_CRIT_SECT_SIZE
   #define MEMHEAP_CRIT_SECT_SIZE 64
#endif

typedef struct mem_heap_stack{
  uint32_t min_block_size;
  uint32_t max_block_size;
  sns_mem_block_header_type *first_block;
}mem_heap_stack;

#define MEMHEAP_STACK_0 (mem_heap_stack){.min_block_size =    0, .max_block_size =   32, .first_block = NULL}
#define MEMHEAP_STACK_1 (mem_heap_stack){.min_block_size =   32, .max_block_size =   64, .first_block = NULL}
#define MEMHEAP_STACK_2 (mem_heap_stack){.min_block_size =   64, .max_block_size =  128, .first_block = NULL}
#define MEMHEAP_STACK_3 (mem_heap_stack){.min_block_size =  128, .max_block_size =  256, .first_block = NULL}
#define MEMHEAP_STACK_4 (mem_heap_stack){.min_block_size =  256, .max_block_size =  384, .first_block = NULL}
#define MEMHEAP_STACK_5 (mem_heap_stack){.min_block_size =  384, .max_block_size =  512, .first_block = NULL}
#define MEMHEAP_STACK_6 (mem_heap_stack){.min_block_size =  512, .max_block_size = 1024, .first_block = NULL}
#define MEMHEAP_STACK_7 (mem_heap_stack){.min_block_size = 1024, .max_block_size = 2048, .first_block = NULL}
#define MEMHEAP_STACK_8 (mem_heap_stack){.min_block_size = 2048, .max_block_size = 4096, .first_block = NULL}
#define MEMHEAP_STACK_9 (mem_heap_stack){.min_block_size = 4096, .max_block_size = UINT32_MAX, .first_block = NULL}
#define MEMHEAP_STACK_LEN 10

typedef struct mem_heap_struct {
  sns_osa_lock                   memheap_crit_sect;
  uint32                         sns_magic_num;
  sns_mem_block_header_type     *first_block;
  /**< Next free block in the heap. */
  unsigned long                  total_blocks;
  /**< Total blocks in the heap. */
  unsigned long                  total_bytes;
  /**< Total bytes available in the heap. */
  unsigned long                  used_bytes;
  /**< Amount of bytes requested in the heap. */
  unsigned long                  max_used;
  /**< Maximum amount that has been used in the heap. */
  unsigned long                  max_request;
  /**< Pointer to the failed function. */
  mem_lock_fnc_type              lock_fnc_ptr;
  mem_free_fnc_type              free_fnc_ptr;
   /**< this is random number used in XOR(guard_bytes)
    calculation of free memory block  */
  uint16                         sns_magic_num_free;
/**< this is random number used in XOR(guard_bytes)
    calculation of used memory block  */
  uint16                         sns_magic_num_used;
  uint16                         sns_magic_num_index;
  unsigned long                  allocated_bytes;
  /**< Amount of bytes actually allocated in the heap
       including block headers and alignment adjustment. */

  /**< Stacks of free blocks */
  mem_heap_stack freeStacks[MEMHEAP_STACK_LEN];
} sns_mem_heap_type;

/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif


/**
  Initializes the I/O heap object and sets up heap_mem_ptr for use with
  the I/O heap object.

  The heap_mem_ptr parameter may be aligned on any boundary. Beginning bytes
  are skipped until a paragraph boundary is reached. NULL pointers must not
  be passed in for heap_mem_ptr. The fail_fnc_ptr parameter may be NULL;
  in which case, no function is called if mem_malloc() or mem_calloc()
  is about to fail. If fail_fnc_ptr is provided, it is called once and the
  allocation is attempted again. See mem_allocator_failed_proc_type()
  for details.

  There is no protection for initializing a heap more than once. If a heap
  is reinitialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed. If this is
  the desired behavior, a heap may be initialized more than once.

  @param[in] heap_ptr         Pointer to the statically allocated heap
                              structure.
  @param[in,out] heap_mem_ptr Pointer to the contiguous block of memory used
                              for this heap.
  @param[in] heap_mem_size    Size in bytes of the memory pointed to by
                              heap_mem_ptr.
  @param[in] fail_fnc_ptr     Function to call when allocation fails; can be
                              NULL.

  @return
  None.

  @dependencies
  None.
*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
void sns_mem_init_heap(
   sns_mem_heap_type                 *heap_ptr,
   void                          *heap_mem_ptr,
   unsigned long                  heap_mem_size
);

/**
  Deinitializes the heap_ptr object only if the heap is in the Reset state.
  Users are responsible for freeing all allocated pointers before calling
  this function.

  @param[in] heap_ptr  Pointer to the heap structure to be deinitialized.

  @return
  None.

  @dependencies
  None.
*/
void sns_mem_deinit_heap(
   sns_mem_heap_type                 *heap_ptr
);

/**
  Allocates a block of size bytes from the heap. If heap_ptr is NULL
  or size is 0, the NULL pointer is silently returned.

  @param[in] heap_ptr    Pointer to the heap from which to allocate.
  @param[in] size        Number of bytes to allocate.

  @return
  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

  @dependencies
  heap_ptr must not be NULL. \n
  size must not be 0.
*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */

void* sns_mem_malloc(
  sns_mem_heap_type *heap_ptr,
  unsigned int         size
);

/**
  Deallocates the ptr block of memory. If ptr or heap_ptr is NULL, or
  ptr is outside the range of memory managed by heap_ptr, the function
  call does nothing and is guaranteed to be harmless. This function will
  ASSERT if it can detect an attempt to free an already freed block.
  However, detection is not always possible.

  @param[in] heap_ptr    Pointer to the heap from which to allocate.
  @param[in] ptr         Pointer to the memory to be freed.
  @param[in] file_name   Name of the file from which mem_free() was called
                         (only used in Debug mode).
  @param[in] line_number Line number corresponding to the mem_free() call
                         (only used in Debug mode).

  @return
  None.

  @dependencies
  None.
*/
/*lint -sem(mem_free,1p) */
void sns_mem_free(
  sns_mem_heap_type *heap_ptr,
  void          *ptr
);

/* ------------------------------------------------------------------------
**
** Memory Heap Debug and Test Routines
**
** ------------------------------------------------------------------------ */

/** @cond
*/
/* Anonymous enum used for keeping tracking of memory. */
enum {
  kBlockFree = 1,/* needed some sapce for checksum in block header so reduced it to 1*/
  kBlockUsed = 0,
  kLastBlock = 1,/* needed some sapce for checksum in block header so reduced it to 1*/
  kMinChunkSize = 16
};
/** @endcond */

#ifdef __cplusplus
   }
#endif

/** @} */ /* end_addtogroup utils_services */
#endif /* MEMHEAP_H */
