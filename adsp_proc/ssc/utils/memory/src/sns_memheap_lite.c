/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.

 Copyright (c) 1997 - 2018 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Id: //components/rel/ssc.slpi/3.2/utils/memory/src/sns_memheap_lite.c#3 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/13/15    ps       Initial Release on SPSS

============================================================================ */


/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include "err.h"
#include "sns_assert.h"
#include "sns_island.h"
#include "sns_memheap_lite.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_types.h"
#include "string.h"
//#include "PrngML.h"

extern void *memset(void *dest, int c, size_t n);

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define OVERFLOW_CHECK(elt_count, elt_size) (!(elt_count >= (1U<<10) || elt_size >= (1U<<22)) || ((((uint64)elt_count * (uint64)elt_size) >> 32) == 0))

#define BOUNDARY_CHECK(theBlock, heap_ptr) ((theBlock >= ((sns_mem_heap_type*)heap_ptr)->first_block) && (theBlock < (((sns_mem_heap_type*)heap_ptr)->first_block + heap_ptr->total_bytes)))

#define FRD_OFFSET_CHECK(block, heap_ptr) ((((sns_mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((sns_mem_heap_type*)heap_ptr)->first_block) + ((sns_mem_heap_type*)heap_ptr)->total_bytes)))

//#define MEMHEAP_VERIFY_HEADER(block, heap_ptr) (((sns_mem_block_header_type *)block)->header_guard == ((sns_mem_heap_type*)heap_ptr)->block_header_guard ? TRUE : FALSE)

/* XOR based Guard byte calculations and restore */
#define INTEGRITY_CHECK_ON_USED_HEADER(sns_magic_num_used, block) \
            (block[0]^block[1]^block[2]^block[3]^sns_magic_num_used)


#define INTEGRITY_CHECK_ON_FREE_HEADER(sns_magic_num_free, block) \
            (block[0]^block[1]^block[2]^block[3]^sns_magic_num_free)


#define ADD_GUARD_BYTES_TO_USED_HEADER(sns_magic_num_used, block) \
            (block[0] = block[1]^block[2]^block[3]^sns_magic_num_used)


#define ADD_GUARD_BYTES_TO_FREE_HEADER(sns_magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^sns_magic_num_free)


#define MAX_HEAP_INIT 8

#define MIN_ALLOC_SIZE (kMinChunkSize + ALIGN_8(sizeof(sns_mem_block_header_type)))

uint32 sns_magic_num[MAX_HEAP_INIT] SNS_SECTION(".data.sns_island_heap") = {(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1};
uint16 sns_magic_num_index_array[MAX_HEAP_INIT] SNS_SECTION(".data.sns_island_heap") = {0,1,2,3,4,5,6,7};
uint16 sns_magic_num_index SNS_SECTION(".data.sns_island_heap") = 0;

static sns_mem_block_header_type *sns_mem_find_free_block(
   sns_mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
);

static void sns_mem_heap_get_random_num(void*  random_ptr, int random_len);


typedef struct frd_Offset_info_type{
  unsigned long pad;
  unsigned long freeBlock_frdOff;
}frd_Offset_info;

#define MEMHEAP_MIN_BLOCK_SIZE 16

/* Code to enter and exit critical sections.
*/
   #define BEGIN_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->lock_fnc_ptr) \
         (heap)->lock_fnc_ptr(heap); \
     } while (0)
   #define END_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->free_fnc_ptr) \
         (heap)->free_fnc_ptr(heap); \
     } while (0)

static void sns_mem_init_block_header(sns_mem_block_header_type *, unsigned long, sns_mem_heap_type *hep_ptr);

#ifdef FEATURE_MEM_DEBUG
  #ifndef MEM_HEAP_CALLER_ADDRESS_LEVEL
    #define MEM_HEAP_CALLER_ADDRESS_LEVEL 1
  #endif
  #define  MEM_HEAP_CALLER_ADDRESS(level) ((void *)__builtin_return_address(level));
#endif //FEATURE_MEM_DEBUG


#define MEMHEAP_ASSERT( xx_exp ) \
      SNS_ASSERT(xx_exp)

/* Lock function for Memheap.
*/
SNS_SECTION(".text.sns_island_heap") static void
sns_mem_heap_enter_crit_sect( void * heap_ptr)
{
  if(NULL != heap_ptr)
  {
     sns_osa_lock_acquire(&((sns_mem_heap_type *)heap_ptr)->memheap_crit_sect);
  }
} /* END mem_heap_enter_crit_sect */

/* Matching free function for mem_heap_lock_mutex().
*/
SNS_SECTION(".text.sns_island_heap") static void
sns_mem_heap_leave_crit_sect( void * heap_ptr)
{
  if(NULL != heap_ptr)
  {
     sns_osa_lock_release(&((sns_mem_heap_type *)heap_ptr)->memheap_crit_sect);
  }
} /* END mem_heap_leave_crit_sect */


SNS_SECTION(".text.sns_island_heap") static void memheap_copy_frd_offset_at_end(sns_mem_block_header_type *mem_block, sns_mem_heap_type *heap_ptr)
{
  frd_Offset_info *temp = NULL;
  // store the frd_offset at the last bytes in the free block
  temp = (frd_Offset_info *)((char*)mem_block + (mem_block->forw_offset - sizeof(frd_Offset_info)));
  if(mem_block->forw_offset != kMinChunkSize){
    temp->pad = sns_magic_num[heap_ptr->sns_magic_num_index];
  }
  temp->freeBlock_frdOff = mem_block->forw_offset;
}

SNS_SECTION(".text.sns_island_heap") static void
  sns_memheap_push_block(mem_heap_stack *stack, sns_mem_block_header_type *block)
{
  SNS_ASSERT(NULL == block->stack_next);
  block->stack_next = stack->first_block;
  stack->first_block = block;
}

SNS_SECTION(".text.sns_island_heap") static sns_mem_block_header_type *
  sns_memheap_pop_block_of_min_size(sns_mem_heap_type *heap_ptr, mem_heap_stack *stack, size_t min_size)
{
  sns_mem_block_header_type *ret_block = NULL;
  sns_mem_block_header_type *curr_block = stack->first_block;
  sns_mem_block_header_type **last_ref = &stack->first_block;
  while(NULL != curr_block)
  {
    uint16* pblk;
    if(curr_block->free_flag == kBlockFree){
        pblk = (uint16*)(curr_block);
        MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk));
     }
     else{
        pblk = (uint16*)(curr_block);
        MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->sns_magic_num_used, pblk));
     }

    if(curr_block->forw_offset >= min_size)
    {
      // Update the last reference to point to next block
      *last_ref = curr_block->stack_next;
      curr_block->stack_next = NULL;
      ret_block = curr_block;
      break;
    }
    else
    {
      // Update the last reference to store location of this block's next ptr
      last_ref = &curr_block->stack_next;
      curr_block = curr_block->stack_next;
    }
  }
  return ret_block;
}

SNS_SECTION(".text.sns_island_heap") static bool
  sns_memheap_remove_block_from_stack(sns_mem_heap_type *heap_ptr, mem_heap_stack *stack, sns_mem_block_header_type *block)
{
  bool ret_val = false;
  sns_mem_block_header_type *curr_block = stack->first_block;
  sns_mem_block_header_type **last_ref = &stack->first_block;
  while(NULL != curr_block)
  {
    uint16* pblk;
    if(curr_block->free_flag == kBlockFree){
        pblk = (uint16*)(curr_block);
        MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk));
     }
     else{
        pblk = (uint16*)(curr_block);
        MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->sns_magic_num_used, pblk));
     }

    if(curr_block == block)
    {
      // Update the last reference to point to next block
      *last_ref = curr_block->stack_next;
      curr_block->stack_next = NULL;
      ret_val = true;
      break;
    }
    else
    {
      // Update the last reference to store location of this block's next ptr
      last_ref = &curr_block->stack_next;
      curr_block = curr_block->stack_next;
    }
  }
  return ret_val;
}

/*===========================================================================
FUNCTION
memheap_clear_footer

DESCRIPTION
In a used block the footer should be cleared to avoid incorrectly defragmenting the heap memory.

===========================================================================*/
SNS_SECTION(".text.sns_island_heap") static void memheap_clear_footer (sns_mem_block_header_type *mem_block, sns_mem_heap_type *heap_ptr)
{
  UNUSED_VAR(heap_ptr);
  frd_Offset_info *temp = NULL;
  // clear the footer in the used block
  temp = (frd_Offset_info *)((char*)mem_block + (mem_block->forw_offset - sizeof(frd_Offset_info)));
  if(mem_block->forw_offset != kMinChunkSize){
    temp->pad = 0;
  }
  temp->freeBlock_frdOff = 0;
}

/*===========================================================================
FUNCTION
mem_heap_get_random_num

DESCRIPTION
Helper API to get a random number of the specified length.

===========================================================================*/
SNS_SECTION(".text.sns_island_heap") static void sns_mem_heap_get_random_num(void*  random_ptr, int random_len)
{
#ifdef FEATURE_MEMHEAP2_USE_PRNG
 if(PRNGML_ERROR_NONE != PrngML_getdata(((uint8*)random_ptr), random_len))
  {
    MEMHEAP_ASSERT(0);
  }
#else
  if(random_len == 4)
  {
    uint32 *ran_num = (uint32 *)random_ptr;
    *ran_num = 0xabcddcba;
  }
  else if(random_len == 2)
  {
    uint16 *ran_num = (uint16 *)random_ptr;
    *ran_num = 0xabcd;
  }
#endif
}

/*===========================================================================
FUNCTION MEM_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  infail_fnc_ptr may be NULL in which case no function will
  be called if mem_malloc or mem_calloc is about to fail.  If infail_fnc_ptr
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
SNS_SECTION(".text.sns_island_heap") void sns_mem_init_heap(
   sns_mem_heap_type                 *heap_ptr,
      /* statically allocated heap structure
      */
    void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                  heap_mem_size
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */
)
{
  char *memory_end_ptr;
    /* 1 beyond computed end of memory passed in to use as heap.
    */
  char *memory_start_ptr;
    /* The computed beginning of the memory passed in to use as heap.  This
       computed value guarantees the heap actually starts on a paragraph
       boundary.
    */
  unsigned long chunks;
    /* How many whole blocks of size kMinChunkSize fit in the area of
       memory starting at memory_start_ptr and ending at (memory_end_ptr-1)
    */
  uint16 * pblk = NULL;

  sns_osa_lock_attr attr;

  MEMHEAP_ASSERT(heap_ptr);


  MEMHEAP_ASSERT(sns_magic_num_index < MAX_HEAP_INIT); /* support at the most 8 heaps*/

  if( (heap_ptr->sns_magic_num) &&
      (heap_ptr->sns_magic_num == sns_magic_num[heap_ptr->sns_magic_num_index])){
    /* heap is already initialized so just return */
    return;
  }

  memset(heap_ptr, 0, sizeof(sns_mem_heap_type));

  MEMHEAP_ASSERT(heap_mem_ptr);
  MEMHEAP_ASSERT(heap_mem_size);
  MEMHEAP_ASSERT(heap_mem_size >= (2*kMinChunkSize-1));

  memory_start_ptr = (char *)heap_mem_ptr;
  memory_end_ptr   = memory_start_ptr + heap_mem_size;

  sns_osa_lock_attr_init(&attr);
  sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  sns_osa_lock_init(&attr, &(heap_ptr->memheap_crit_sect));

    /* by default it is critical section */
  heap_ptr->lock_fnc_ptr = sns_mem_heap_enter_crit_sect;
  heap_ptr->free_fnc_ptr = sns_mem_heap_leave_crit_sect;

  /* Advance to the nearest paragraph boundary. This while loop should work
  ** regardless of how many bits are required for address pointers near or
  ** far, etc.
  **
  ** Turn off lint "size incompatibility" warning because cast from pointer
  ** to unsigned long will lose bits, but we don't care because we're only
  ** interested in the least significant bits and we never cast back into a
  ** pointer, so the warning can be safely ignored
  */
  /*lint --e(507)*/

  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  {
     ++memory_start_ptr;
  }

  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  heap_ptr->first_block            = (sns_mem_block_header_type *) memory_start_ptr;
  sns_mem_heap_get_random_num((&sns_magic_num[sns_magic_num_index_array[sns_magic_num_index]]), 4);
  heap_ptr->sns_magic_num = sns_magic_num[sns_magic_num_index_array[sns_magic_num_index]];
  heap_ptr->sns_magic_num_index = sns_magic_num_index_array[sns_magic_num_index];
  sns_mem_heap_get_random_num(&(heap_ptr->sns_magic_num_free), 2);
  sns_mem_heap_get_random_num(&(heap_ptr->sns_magic_num_used), 2);
  sns_magic_num_index++;
  sns_mem_init_block_header(heap_ptr->first_block, chunks * kMinChunkSize, heap_ptr);
  heap_ptr->first_block->last_flag = (char) kLastBlock;
  heap_ptr->total_blocks           = 1;
  heap_ptr->max_used               = 0;
  heap_ptr->max_request            = 0;
  heap_ptr->used_bytes             = 0;
  heap_ptr->allocated_bytes        = 0;
  heap_ptr->total_bytes            = chunks * kMinChunkSize;
  pblk = (uint16*)(heap_ptr->first_block);
  heap_ptr->freeStacks[0] = MEMHEAP_STACK_0;
  heap_ptr->freeStacks[1] = MEMHEAP_STACK_1;
  heap_ptr->freeStacks[2] = MEMHEAP_STACK_2;
  heap_ptr->freeStacks[3] = MEMHEAP_STACK_3;
  heap_ptr->freeStacks[4] = MEMHEAP_STACK_4;
  heap_ptr->freeStacks[5] = MEMHEAP_STACK_5;
  heap_ptr->freeStacks[6] = MEMHEAP_STACK_6;
  heap_ptr->freeStacks[7] = MEMHEAP_STACK_7;
  heap_ptr->freeStacks[8] = MEMHEAP_STACK_8;
  heap_ptr->freeStacks[9] = MEMHEAP_STACK_9;

  for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
  {
    if(heap_ptr->first_block->forw_offset >= heap_ptr->freeStacks[i].min_block_size &&
       heap_ptr->first_block->forw_offset <  heap_ptr->freeStacks[i].max_block_size)
    {
      sns_memheap_push_block(&heap_ptr->freeStacks[i], heap_ptr->first_block);
      break;
    }
  }

  ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk);
  return;
} /* END mem_init_heap */


/*===========================================================================
FUNCTION MEM_DEINIT_HEAP

DESCRIPTION
  De-Initializes the heap_ptr object only if the heap is in reset state.
  User is responsible for freeing all the allocated pointers before  calling
  into this function.
===========================================================================*/
SNS_SECTION(".text.sns_island_heap") void sns_mem_deinit_heap(
   sns_mem_heap_type                 *heap_ptr
      /* statically allocated heap structure
      */

)
{
/* Change request has been communicated to Gaurang P.
 *temporary workaround until official change gets promoted */

  // return the magic number
  sns_magic_num_index--;
  if(sns_magic_num_index < MAX_HEAP_INIT)
  {
    sns_magic_num_index_array[sns_magic_num_index]=heap_ptr->sns_magic_num_index;
  }
#if 1 //support OM transitions where clients are not forced to de-allocate their memory
  memset(heap_ptr, 0, sizeof(sns_mem_heap_type));
#else
 /* De-initialize heap only if all the allocated blocks are freed */
 if(heap_ptr->used_bytes == 0)
 {
   //qurt_pimutex_destroy((qurt_mutex_t*)&(heap_ptr->memheap_crit_sect));
   memset(heap_ptr, 0, sizeof(sns_mem_heap_type));
 }
 else
 {
    MEMHEAP_ASSERT(heap_ptr->used_bytes == 0);
 }
#endif
}
/*===========================================================================
FUNCTION MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
===========================================================================*/
SNS_SECTION(".text.sns_island_heap") void sns_mem_init_block_header(
   sns_mem_block_header_type *block_ptr,
     /* Memory header block to be initialized
     */
   unsigned long          size,
     /* The size of the block of memory controlled by this
        memory header block INCLUDING the size of the
        header block itself
     */
     sns_mem_heap_type *heap_ptr
)
{
  char *p_temp;

  (void)heap_ptr; //Unused variable

  MEMHEAP_ASSERT(block_ptr);
  p_temp = ((char*)block_ptr);
  memset(p_temp, 0 , ALIGN_8(sizeof(sns_mem_block_header_type)));
  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->forw_offset = size;
  block_ptr->stack_next = NULL;
  return ;
} /* END mem_init_block_header */


/*===========================================================================
FUNCTION MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
SNS_SECTION(".text.sns_island_heap") void* sns_mem_malloc(
  sns_mem_heap_type *heap_ptr,
     /* Heap from which to allocate
     */

  unsigned int         size
     /* Number of bytes to allocate
     */
)
{

  unsigned long chunks;
    /* the computed minimum size of the memory block in chunks needed
       to satisfy the request */

  unsigned long actualSize;
    /* the computed minimum size of the memory block in bytes needed
       to satisfy the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes at the end of the allocated
       memory block.  Will always be < kMinChunkSize + MIN_ALLOC_SIZE */

  sns_mem_block_header_type *freeBlock = NULL;
    /* the free block found of size >= actualSize */

  void *answer = NULL;
    /* the address of memory to be returned to the caller */

  uint16 * pblk = NULL;
  uint32 blockHeaderSize=ALIGN_8(sizeof(sns_mem_block_header_type));

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL);

  if (!size) return NULL;

  /* quick check if requested size of memory is available */
  if( (unsigned long) size > heap_ptr->total_bytes ) return NULL;

  /* chunks overflow check : check max memory that can be malloc'd at a time */
  if( (0xFFFFFFFF - ( kMinChunkSize + ALIGN_8(sizeof(sns_mem_block_header_type))) )
      < ((unsigned long) size)) return NULL;

  chunks = ((unsigned long) size + blockHeaderSize
            + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                (actualSize - size - blockHeaderSize);

  BEGIN_CRITICAL_SECTION(heap_ptr);

  MEMHEAP_ASSERT(heap_ptr->sns_magic_num == sns_magic_num[heap_ptr->sns_magic_num_index]);
  freeBlock = sns_mem_find_free_block(heap_ptr, actualSize);

  if (freeBlock)
  {
      /* split the block (if necessary) and return the new block */

      MEMHEAP_ASSERT(freeBlock->forw_offset > 0);
      // frd offset and actual size are chunk aligned
      if (freeBlock->forw_offset > actualSize + MIN_ALLOC_SIZE)
      {
        /* must split into two free blocks */


        sns_mem_block_header_type *newBlock = (sns_mem_block_header_type *)
                                          ((char *) freeBlock + actualSize);
        sns_mem_init_block_header(newBlock, freeBlock->forw_offset - actualSize, heap_ptr);
        newBlock->last_flag = freeBlock->last_flag;
        freeBlock->forw_offset = actualSize;
        freeBlock->last_flag = 0;
        ++heap_ptr->total_blocks;

        for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
        {
          if(newBlock->forw_offset >= heap_ptr->freeStacks[i].min_block_size &&
             newBlock->forw_offset <  heap_ptr->freeStacks[i].max_block_size)
          {
            sns_memheap_push_block(&heap_ptr->freeStacks[i], newBlock);
            break;
          }
        }

         pblk = (uint16*)newBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk);
        memheap_copy_frd_offset_at_end(newBlock, heap_ptr);
      }
      else
      {
        bonusBytes += freeBlock->forw_offset - actualSize;
      }

      /* mark the block as used and return it */

      freeBlock->free_flag = kBlockUsed;
      freeBlock->extra = bonusBytes;

      heap_ptr->used_bytes += size;
      heap_ptr->allocated_bytes += freeBlock->forw_offset;

      MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->allocated_bytes);

      if (heap_ptr->used_bytes > heap_ptr->max_used) {
        heap_ptr->max_used = heap_ptr->used_bytes;
      }
      if (size > heap_ptr->max_request) {
        heap_ptr->max_request = size;
      }

      pblk = (uint16*)freeBlock;
      ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->sns_magic_num_used, pblk);
      answer = (char *) freeBlock + blockHeaderSize;
      memheap_clear_footer(freeBlock, heap_ptr);
  }

#ifdef FEATURE_MEM_DEBUG
  if(answer != NULL && freeBlock != NULL)
  {
    freeBlock->caller_ptr=MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);
    freeBlock->tid = sns_osa_thread_get_thread_id();
  }
#endif

  END_CRITICAL_SECTION(heap_ptr);

  return answer;
} /* END mem_malloc */


/*===========================================================================
FUNCTION MEM_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
/*lint -sem(mem_free,1p) */
SNS_SECTION(".text.sns_island_heap") void sns_mem_free(
  sns_mem_heap_type *heap_ptr,
     /* Heap in which to free memory
     */

  void          *ptr
     /* Memory to free
     */
)
{
  sns_mem_block_header_type *theBlock;
    /* The computed address of the memory header block in the heap that
       controls the memory referenced by ptr */
  frd_Offset_info *temp;
  uint32          sizeBlockHeader=ALIGN_8(sizeof(sns_mem_block_header_type));

  uint16 *pblk = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL);


  /*commented out the below assert since NULL free occurances are found and once
   corrsponding fixes are in its need to be uncomment*/
  MEMHEAP_ASSERT(heap_ptr->sns_magic_num == sns_magic_num[heap_ptr->sns_magic_num_index]);



  /*commented out the below assert since NULL free occurances are found and once
   corrsponding fixes are in its need to be uncomment*/
  if(ptr == NULL)
  {
     //MEMHEAP_ERROR(" NULL ptr occurenaces in mem_free()",0,0,0);
    //MEMHEAP_ASSERT(0);

    return ;
  }


  /* free the passed in block */

  MEMHEAP_ASSERT(heap_ptr->first_block);
  BEGIN_CRITICAL_SECTION(heap_ptr);


  theBlock = (sns_mem_block_header_type *)
                          ((char *) ptr - sizeBlockHeader);


  //check for block alignment to 16
  MEMHEAP_ASSERT((((uint32)theBlock)%kMinChunkSize) == 0);

  /* boundry check for the ptr passed to free */
  MEMHEAP_ASSERT(BOUNDARY_CHECK(theBlock, heap_ptr));

  /* Try to detect corruption. */

  MEMHEAP_ASSERT(!theBlock->free_flag);  /* Attempt to detect multiple
                                            frees of same block */
  /* Make sure forw_offset is reasonable */
  MEMHEAP_ASSERT(theBlock->forw_offset >= sizeBlockHeader);
  /* Make sure extra is reasonable */
  MEMHEAP_ASSERT(theBlock->extra < kMinChunkSize + MIN_ALLOC_SIZE);

  /* Make sure forw_offset is not spiling over the heap boundry */
  MEMHEAP_ASSERT(FRD_OFFSET_CHECK(theBlock, heap_ptr));

 //check for heap canary
  pblk = (uint16*)(theBlock);
  MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->sns_magic_num_used, pblk));

  {
    MEMHEAP_ASSERT((theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra) <= heap_ptr->used_bytes);
    heap_ptr->used_bytes -= theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra;
    heap_ptr->allocated_bytes -= theBlock->forw_offset;
    MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->allocated_bytes);

    theBlock->free_flag = (char) kBlockFree;
    /* try to do defragmentation if possible*/
    /*check if it can concatenate the previous block of the theBlock*/
    if(theBlock != heap_ptr->first_block)
    {
      sns_mem_block_header_type *preFreeBlock;
      int validBlock = FALSE;
      temp = (frd_Offset_info*)((char*)theBlock - sizeof(frd_Offset_info));

      if(!((temp->freeBlock_frdOff)%kMinChunkSize)){

        if(MEMHEAP_MIN_BLOCK_SIZE == temp->freeBlock_frdOff){
           validBlock = TRUE;
        }
        else if(temp->pad  == sns_magic_num[heap_ptr->sns_magic_num_index]){
          validBlock = TRUE;
        }

        if(validBlock){
           preFreeBlock =  (sns_mem_block_header_type *)((char*)theBlock - temp->freeBlock_frdOff);
           pblk = (uint16*)preFreeBlock;
           if(preFreeBlock >= heap_ptr->first_block){
              if((preFreeBlock->forw_offset == temp->freeBlock_frdOff)
                 &&(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk)) /*we don’t want to ASSERT if integrity check fails , just don’t concatenate the block*/
                 &&(preFreeBlock->free_flag  == kBlockFree))
              {

                 /* preFreeBlock is free and now we can join it with the new
                   free block theBlock so remove it from the stack*/

                for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
                {
                  if(preFreeBlock->forw_offset >= heap_ptr->freeStacks[i].min_block_size &&
                     preFreeBlock->forw_offset <  heap_ptr->freeStacks[i].max_block_size)
                  {
                    SNS_ASSERT(sns_memheap_remove_block_from_stack(heap_ptr,&heap_ptr->freeStacks[i], preFreeBlock));
                    break;
                  }
                }

                  /* Set preFreeBlock's end bytes to 0 as the frd_offset has changed */
                 temp->freeBlock_frdOff = 0;
                 temp->pad = 0;
                 --heap_ptr->total_blocks;
                 preFreeBlock->forw_offset=preFreeBlock->forw_offset+theBlock->forw_offset;
                 preFreeBlock->last_flag = theBlock->last_flag;


                 /* now set the theBlock's header info. to NULL */
                 {
                   uint32 *temp = (uint32*)theBlock; /*did it like this for optimization purpose*/
                   temp[0] = 0;
                   temp[1] = 0;
                 }
                 theBlock = preFreeBlock;
              }
           }
        }/*if(validBlock)*/

      }/*if(!((temp->freeBlock_frdOff)%kMinChunkSize))*/

    }/*if(theBlock != heap_ptr->first_block)*/

    /*check if it can concatenate the next block of the theBlock*/
    if(theBlock->last_flag != kLastBlock){
      char *end_address=(char*)(heap_ptr->first_block) + heap_ptr->total_bytes;
      if(((char*)theBlock + theBlock->forw_offset) < end_address)
      {
         sns_mem_block_header_type *nextFreeBlock =
           (sns_mem_block_header_type *)((char *)theBlock + theBlock->forw_offset);
         if(nextFreeBlock->free_flag == kBlockFree)
         {
           pblk = (uint16*)nextFreeBlock;
           MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk));

           for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
           {
             if(nextFreeBlock->forw_offset >= heap_ptr->freeStacks[i].min_block_size &&
                nextFreeBlock->forw_offset <  heap_ptr->freeStacks[i].max_block_size)
             {
               SNS_ASSERT(sns_memheap_remove_block_from_stack(heap_ptr,&heap_ptr->freeStacks[i], nextFreeBlock));
               break;
             }
           }

           theBlock->forw_offset += nextFreeBlock->forw_offset;
           theBlock->last_flag = nextFreeBlock->last_flag;

           /* now set the nextFreeBlock's header info. to NULL */
           {
             uint32 *temp = (uint32*)nextFreeBlock; /*did it like this for optimization purpose*/
             temp[0] = 0;
             temp[1] = 0;
           }
           --heap_ptr->total_blocks;
         }else{
            /* Assert if the next block is used but fails the integrity check
               It is highly likely that the owner of the current block
               corrupted the next block and we should try to catch early. */
            pblk = (uint16*)nextFreeBlock;
            MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->sns_magic_num_used, pblk));
         }
      }
    }

    for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
    {
      if(theBlock->forw_offset >= heap_ptr->freeStacks[i].min_block_size &&
         theBlock->forw_offset <  heap_ptr->freeStacks[i].max_block_size)
      {
        sns_memheap_push_block(&heap_ptr->freeStacks[i], theBlock);
        break;
      }
    }

    pblk = (uint16*)theBlock;
    ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk);
    memheap_copy_frd_offset_at_end(theBlock, heap_ptr);

    /* Wipe out the block. Required by safe heap. */
    //memset(((char *)theBlock + ALIGN_8(sizeof(sns_mem_block_header_type))),0xA5,
    //       theBlock->forw_offset - ALIGN_8(sizeof(sns_mem_block_header_type)));

    /* reset heap to initial state if everything is now freed */
    if (!heap_ptr->used_bytes) {

      sns_mem_init_block_header(heap_ptr->first_block, heap_ptr->total_bytes, heap_ptr);
      heap_ptr->first_block->last_flag = (char) kLastBlock;
      heap_ptr->total_blocks = 1;
      pblk = (uint16*)(heap_ptr->first_block);
      ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->sns_magic_num_free, pblk);
    }
  }

  END_CRITICAL_SECTION(heap_ptr);
  return;
} /* END mem_free */

/*===========================================================================
FUNCTION MEM_FIND_FREE_BLOCK

DESCRIPTION
  Find a free block of at least inSizeNeeded total bytes.  Collapse
  adjacent free blocks along the way.

  Returns a pointer to a memory block header describing a free block
  of at least inSizeNeeded total bytes.  Returns NULL if no such free
  block exists or can be created by collapsing adjacent free blocks.
===========================================================================*/
SNS_SECTION(".text.sns_island_heap") static sns_mem_block_header_type *sns_mem_find_free_block(
   sns_mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
)
{
  sns_mem_block_header_type *free_block = NULL;

  for(uint8_t i = 0; i < ARR_SIZE(heap_ptr->freeStacks); i++)
  {
    if(size_needed <= heap_ptr->freeStacks[i].max_block_size)
    {
      free_block = sns_memheap_pop_block_of_min_size(heap_ptr, &heap_ptr->freeStacks[i], size_needed);
    }
    if(NULL != free_block)
    {
      break;
    }
  }
  return free_block;
} /* END mem_find_free_block */

/*===========================================================================
FUNCTION mem_heap_init_lock_free_fns

DESCRIPTION
     init the lock and free functions of the heap , its called from amssheap.c
===========================================================================*/
SNS_SECTION(".text.sns_island_heap") void sns_mem_heap_init_lock_free_fns(sns_mem_heap_type  *heap_ptr)
{
   MEMHEAP_ASSERT (heap_ptr != NULL);
   heap_ptr->lock_fnc_ptr = sns_mem_heap_enter_crit_sect;
   heap_ptr->free_fnc_ptr = sns_mem_heap_leave_crit_sect;
}
