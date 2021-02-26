/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.  

Copyright (c) 2015-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ServicesLib/memheap_lite.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/19/16    nk     Changes to clear the Footer
09/17/15    mg     Initial Release

============================================================================ */


/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap_lite.h"
#include <stdio.h>
#include "DALSys.h"
#include "PrngML.h"
#include "CoreVerify.h"


/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define OVERFLOW_CHECK(elt_count, elt_size) (!(elt_count >= (1U<<10) || elt_size >= (1U<<22)) || ((((uint64)elt_count * (uint64)elt_size) >> 32) == 0))

#define BOUNDARY_CHECK(theBlock, heap_ptr) ((theBlock >= ((mem_heap_type*)heap_ptr)->first_block) && (theBlock < (((mem_heap_type*)heap_ptr)->first_block + heap_ptr->total_bytes)))

#define FRD_OFFSET_CHECK(block, heap_ptr) ((((mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((mem_heap_type*)heap_ptr)->first_block) + ((mem_heap_type*)heap_ptr)->total_bytes)))

/* XOR based Guard byte calculations and restore */
#define INTEGRITY_CHECK_ON_USED_HEADER(magic_num_used, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_used)


#define INTEGRITY_CHECK_ON_FREE_HEADER(magic_num_free, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_free)


#define ADD_GUARD_BYTES_TO_USED_HEADER(magic_num_used, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_used)


#define ADD_GUARD_BYTES_TO_FREE_HEADER(magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_free)





#define MAX_HEAP_INIT 3
uint32 magic_num[MAX_HEAP_INIT] = {(uint32)-1,(uint32)-1,(uint32)-1};
uint16 magic_num_index_array[MAX_HEAP_INIT] = {0,1,2};
uint16 magic_num_index = 0; 



static mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
);

static void mem_heap_get_random_num(void*  random_ptr, int random_len);


typedef struct frd_Offset_info_type{
  uint32 pad;
  uint32 freeBlock_frdOff;
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


static void mem_init_block_header(mem_block_header_type *, unsigned long, mem_heap_type *hep_ptr);

#define MEMHEAP_ASSERT( xx_exp ) \
	if( !(xx_exp) ) \
	{ \
		ERR_FATAL("Assertion" #xx_exp "failed", 0,0,0);\
	}

/* Lock function for Memheap.
*/
static void
mem_heap_enter_crit_sect( void * ptr)
{

  DALSYS_SyncEnter(((mem_heap_type *)ptr)->memheap_crit_sect);
 
 
} /* END mem_heap_enter_crit_sect */

/* Matching free function for mem_heap_lock_mutex().
*/
static void
mem_heap_leave_crit_sect( void * ptr)
{
  
  DALSYS_SyncLeave(((mem_heap_type *)ptr)->memheap_crit_sect);
  
 
} /* END mem_heap_leave_crit_sect */


static void memheap_copy_frd_offset_at_end(mem_block_header_type *mem_block, mem_heap_type *heap_ptr)
{
  frd_Offset_info *temp = NULL;
  // store the frd_offset at the last bytes in the free block
  temp = (frd_Offset_info *)((char*)mem_block + (mem_block->forw_offset - sizeof(frd_Offset_info)));
  if(mem_block->forw_offset != kMinChunkSize){
    temp->pad = magic_num[heap_ptr->magic_num_index];
  }
  temp->freeBlock_frdOff = mem_block->forw_offset;
}

/*===========================================================================
FUNCTION mem_heap_get_random_num
DESCRIPTION

===========================================================================*/
static void mem_heap_get_random_num(void*  random_ptr, int random_len)
{
#ifdef FEATURE_MEMHEAP2_USE_PRNG
 if(PRNGML_ERROR_NONE != PrngML_getdata_lite(((uint8*)random_ptr), random_len))
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
FUNCTION MEM_GET_NEXT_BLOCK

DESCRIPTION
  Return the next block header in the heap for the block following the
  given one.  If the given block is the last block in the heap, return
  the first block in the heap.  Never returns NULL.

  Returns a pointer the the memory block header of the block following the given
  block (or the first block of the heap if the given block was the last
  block of the heap).

===========================================================================*/
static mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr
     /*  The block in the heap for which to return the following heap
         block
     */
)
{
   mem_block_header_type *nextBlkPtr = NULL;
   uint16 *pblk=NULL;
   MEMHEAP_ASSERT(block_ptr != NULL);
   MEMHEAP_ASSERT(heap_ptr != NULL); //this may be redundent since , it a static function re-visit this for possible optimisation
   MEMHEAP_ASSERT(block_ptr->forw_offset != 0);
   MEMHEAP_ASSERT(FRD_OFFSET_CHECK(block_ptr, heap_ptr));
   MEMHEAP_ASSERT(!((block_ptr->forw_offset)%kMinChunkSize));   
  
   nextBlkPtr =  block_ptr->last_flag  ? heap_ptr->first_block
           : (mem_block_header_type *) ((char *) block_ptr + block_ptr->forw_offset);
   pblk = (uint16*)nextBlkPtr;
   if(nextBlkPtr->free_flag == kBlockFree){
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
   return nextBlkPtr;
}/* END mem_get_next_block */




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
void mem_init_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
    void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned int                  heap_mem_size
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

  MEMHEAP_ASSERT(heap_ptr);

 
  MEMHEAP_ASSERT(magic_num_index < MAX_HEAP_INIT); /* support at the most 3 heaps*/
  
  if( (heap_ptr->magic_num) && 
      (heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index])){
  	/* heap is already initialized so just return */
  	return;
  }

  memset(heap_ptr, 0, sizeof(mem_heap_type));

  MEMHEAP_ASSERT(heap_mem_ptr);
  MEMHEAP_ASSERT(heap_mem_size);
  MEMHEAP_ASSERT(heap_mem_size >= (2*kMinChunkSize-1));

  memory_start_ptr = (char *)heap_mem_ptr;
  memory_end_ptr   = memory_start_ptr + heap_mem_size;

  if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,(DALSYSSyncHandle *)&(heap_ptr->memheap_crit_sect),NULL)) 
  {
      return;
  }
    /* by default it is critical section */
  heap_ptr->lock_fnc_ptr = mem_heap_enter_crit_sect;
  heap_ptr->free_fnc_ptr = mem_heap_leave_crit_sect;
  //use the memory required for memheap_crit_sect from the heap getting initialized */
 while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;         
  }

  /*move the memory start pointer by sizeof(osal_crit_sect_t) as we have used that much out of the heap;*/
  //memory_start_ptr = (char *)memory_start_ptr + sizeof(osal_crit_sect_t);

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
  /*while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;
  }*/
  
  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  heap_ptr->first_block            = (mem_block_header_type *) memory_start_ptr;
  heap_ptr->next_block             = heap_ptr->first_block;

 
  mem_heap_get_random_num((&magic_num[magic_num_index_array[magic_num_index]]), 4);
  heap_ptr->magic_num = magic_num[magic_num_index_array[magic_num_index]];
  heap_ptr->magic_num_index = magic_num_index_array[magic_num_index];
  mem_heap_get_random_num(&(heap_ptr->magic_num_free), 2);
  mem_heap_get_random_num(&(heap_ptr->magic_num_used), 2); 
  magic_num_index++;  
  mem_init_block_header(heap_ptr->first_block, chunks * kMinChunkSize, heap_ptr);
  heap_ptr->first_block->last_flag = (char) kLastBlock;
  heap_ptr->total_blocks           = 1;
  heap_ptr->max_used               = 0;
  heap_ptr->max_request            = 0;
  heap_ptr->used_bytes             = 0;
  heap_ptr->total_bytes            = chunks * kMinChunkSize;  
  pblk = (uint16*)(heap_ptr->first_block);
  ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
  return;
} /* END mem_init_heap */


/*===========================================================================
FUNCTION MEM_DEINIT_HEAP

DESCRIPTION
  De-Initializes the heap_ptr object only if the heap is in reset state.
  User is responsible for freeing all the allocated pointers before  calling 
  into this function.
===========================================================================*/
void mem_deinit_heap(
   mem_heap_type                 *heap_ptr
      /* Statically allocated heap structure
      */
)
{
  // return the magic number
  magic_num_index--;
  if(magic_num_index < MAX_HEAP_INIT)
  {
	 magic_num_index_array[magic_num_index]=heap_ptr->magic_num_index;
  }
 /* De-initialize heap only if all the allocated blocks are freed */
 if(heap_ptr->used_bytes == 0)
 {
   //qurt_pimutex_destroy((qurt_mutex_t*)&(heap_ptr->memheap_crit_sect));
   memset(heap_ptr, 0, sizeof(mem_heap_type));
 }
 else
 {
	MEMHEAP_ASSERT(heap_ptr->used_bytes == 0);
 }
}

/*===========================================================================
FUNCTION MEM_FORCE_DEINIT_HEAP

DESCRIPTION
  Deinitializes the heap_ptr object even if the heap is not in the Reset state.
  Even if the heap has memory allocated (or used bytes is not zero1) it allows 
  the heap to deinit
  The mem_force_deinit_heap API client is responsible for any side effect of the 
  heap force deinit such as :
   - the user who had the memory still allocated , if later calls free() for that
     memory ptr.
   - the user who had the memory still allocated, may still be actively using the 
     memory so if after the force_deinit heap the heap memory relinquished for some
     other purpose then the active user may corrupt it since it still holds 
     the heap memory address
===========================================================================*/
void mem_force_deinit_heap(
   mem_heap_type                 *heap_ptr
      /* Statically allocated heap structure
      */
)
{
  // return the magic number
  magic_num_index--;
  if(magic_num_index < MAX_HEAP_INIT)
  {
	 magic_num_index_array[magic_num_index]=heap_ptr->magic_num_index;
  }

  /* force De-initialize heap even if all the allocated blocks are not freed */
  //qurt_pimutex_destroy((qurt_mutex_t*)&(heap_ptr->memheap_crit_sect));
   memset(heap_ptr, 0, sizeof(mem_heap_type));
}

/*===========================================================================
FUNCTION MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
===========================================================================*/
void mem_init_block_header(
   mem_block_header_type *block_ptr,
     /* Memory header block to be initialized 
     */
   unsigned long          size,
     /* The size of the block of memory controlled by this
        memory header block INCLUDING the size of the
        header block itself
     */
     mem_heap_type *heap_ptr
)
{
  char *p_temp;
  MEMHEAP_ASSERT(block_ptr);
  p_temp = ((char*)block_ptr);
  memset(p_temp, 0 , sizeof(mem_block_header_type));
  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->forw_offset = size;

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
void* mem_malloc(
  mem_heap_type *heap_ptr,
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
       memory block.  Will always be < kMinChunkSize */

  mem_block_header_type *freeBlock = NULL;
    /* the free block found of size >= actualSize */

  void *answer = NULL;
    /* the address of memory to be returned to the caller */

  uint16 * pblk = NULL;
  uint32 blockHeaderSize=sizeof(mem_block_header_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL); 

  

  if (!size) return NULL;

  
  
  /* quick check if requested size of memory is available */
  if( (unsigned long) size > heap_ptr->total_bytes ) return NULL;

  /* chunks overflow check : check max memory that can be malloc'd at a time */
  if( (0xFFFFFFFF - ( kMinChunkSize + sizeof(mem_block_header_type)) ) 
  	    < ((unsigned long) size)) return NULL;

  chunks = ((unsigned long) size + blockHeaderSize
            + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                (actualSize - size - blockHeaderSize);  

  BEGIN_CRITICAL_SECTION(heap_ptr);

  MEMHEAP_ASSERT(heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index]);   
  freeBlock = mem_find_free_block(heap_ptr, actualSize);

  if (freeBlock)
  {
      /* split the block (if necessary) and return the new block */
 
      MEMHEAP_ASSERT(freeBlock->forw_offset > 0);
      // frd offset and actual size are chunk aligned 
      if (freeBlock->forw_offset > actualSize)
      {
        /* must split into two free blocks */

 
        mem_block_header_type *newBlock = (mem_block_header_type *)
                                          ((char *) freeBlock + actualSize);
        mem_init_block_header(newBlock, freeBlock->forw_offset - actualSize, heap_ptr);
        newBlock->last_flag = freeBlock->last_flag;
        freeBlock->forw_offset = actualSize;
        freeBlock->last_flag = 0;


        ++heap_ptr->total_blocks;        
         pblk = (uint16*)newBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
		/* Update the footer of the free block obtained through spliting  */
        memheap_copy_frd_offset_at_end(newBlock, heap_ptr);
      }
      else{  /*set the footer (frd_Offset_info) to NULL for security */
        frd_Offset_info *temp;
        temp = (frd_Offset_info*)((char*)freeBlock + freeBlock->forw_offset - sizeof(frd_Offset_info));
        temp->freeBlock_frdOff = 0;
        temp->pad = 0;
      }

      /* mark the block as used and return it */
  
      freeBlock->free_flag = kBlockUsed;
      freeBlock->extra = bonusBytes;      
      /*  set up next block to search for
          next allocation request */     
      heap_ptr->next_block = mem_get_next_block(heap_ptr, freeBlock);
       
       
      heap_ptr->used_bytes += size;
      MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
      
      if (heap_ptr->used_bytes > heap_ptr->max_used) {
        heap_ptr->max_used = heap_ptr->used_bytes;
      }
      if (size > heap_ptr->max_request) {
        heap_ptr->max_request = size;
      }
    
	  pblk = (uint16*)freeBlock;
      ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);
      answer = (char *) freeBlock + blockHeaderSize;
  }

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
void mem_free(
  mem_heap_type *heap_ptr,
     /* Heap in which to free memory
     */
 
  void          *ptr
     /* Memory to free
     */
)
{
  mem_block_header_type *theBlock;
    /* The computed address of the memory header block in the heap that
       controls the memory referenced by ptr */ 
  frd_Offset_info *temp;
  uint32          sizeBlockHeader=sizeof(mem_block_header_type);

  uint16 *pblk = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  MEMHEAP_ASSERT(heap_ptr != NULL);

 
  /*commented out the below assert since NULL free occurances are found and once 
   corrsponding fixes are in its need to be uncomment*/
  MEMHEAP_ASSERT(heap_ptr->magic_num == magic_num[heap_ptr->magic_num_index]);
 
 

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
  
 
  theBlock = (mem_block_header_type *)
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
  MEMHEAP_ASSERT(theBlock->extra < kMinChunkSize);
 
  /* Make sure forw_offset is not spiling over the heap boundry */
  MEMHEAP_ASSERT(FRD_OFFSET_CHECK(theBlock, heap_ptr));
  
 //check for heap canary
  pblk = (uint16*)(theBlock);
  MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));

  

  if (!theBlock->free_flag) /* Be intelligent about not munging the heap if
                              a multiple free of the same block is detected */
  {

   
    MEMHEAP_ASSERT((theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra) <= heap_ptr->used_bytes);
    heap_ptr->used_bytes -= theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra;
    MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
    
    theBlock->free_flag = (char) kBlockFree;
	 /* try to do defragmentation if possible*/
    /*check if it can concatenate the previous block of the theBlock*/
	if(theBlock != heap_ptr->first_block)
    {
      mem_block_header_type *preFreeBlock;
      int validBlock = FALSE;
      temp = (frd_Offset_info*)((char*)theBlock - sizeof(frd_Offset_info));

      if(!((temp->freeBlock_frdOff)%kMinChunkSize)){

        if(MEMHEAP_MIN_BLOCK_SIZE == temp->freeBlock_frdOff){
           validBlock = TRUE;
        }
        else if(temp->pad  == magic_num[heap_ptr->magic_num_index]){
          validBlock = TRUE;
        }

        if(validBlock){
           preFreeBlock =  (mem_block_header_type *)((char*)theBlock - temp->freeBlock_frdOff);
           pblk = (uint16*)preFreeBlock;
           if(preFreeBlock >= heap_ptr->first_block){
              if((preFreeBlock->forw_offset == temp->freeBlock_frdOff)
                 &&(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)) /*we don’t want to ASSERT if integrity check fails , just don’t concatenate the block*/
                 &&(preFreeBlock->free_flag  == kBlockFree))
              {

                 /* preFreeBlock is free and now we can join it with the new
                   free block theBlock so remove it from the list if apply*/
               
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
         mem_block_header_type *nextFreeBlock =
           (mem_block_header_type *)((char *)theBlock + theBlock->forw_offset);
         if(nextFreeBlock->free_flag == kBlockFree)
         {
           pblk = (uint16*)nextFreeBlock;
           MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
          
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
            MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
         }
      }
    }

   

  
    /* now backup the next pointer if applicable */
    //next_block = mem_get_next_block(heap_ptr, theBlock);

    if (theBlock < heap_ptr->next_block) {
      /* Backup now to lessen possible fragmentation */
      heap_ptr->next_block = theBlock;
    }   

    pblk = (uint16*)theBlock;
    ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
	memheap_copy_frd_offset_at_end(theBlock, heap_ptr);

    /* reset heap to initial state if everything is now freed */
    if (!heap_ptr->used_bytes) {

      /* reset heap now, but retain statistics */
      heap_ptr->next_block = heap_ptr->first_block;
      mem_init_block_header(heap_ptr->first_block, heap_ptr->total_bytes, heap_ptr);
      heap_ptr->first_block->last_flag = (char) kLastBlock;
      heap_ptr->total_blocks = 1;
	   pblk = (uint16*)(heap_ptr->first_block);
      ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);          
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
static mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
)
{
  long searchBlocks;
    /* The maximum number of blocks to search.  After searching this
       many, we've been through the heap once and the allocation fails
       if we couldn't find/create a satisfactory free block */

  mem_block_header_type *followingBlock;
    /* a loop variable used to walk through the blocks of the heap */
  uint16 *pblk;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(heap_ptr->first_block);
  /* sanity check for the  heap_ptr->next_block  */
  if((heap_ptr->next_block)->free_flag == kBlockFree){
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
        
  searchBlocks = (long) heap_ptr->total_blocks;

  followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);
  
  for (; searchBlocks > 0; --searchBlocks, heap_ptr->next_block=followingBlock,
                  followingBlock=mem_get_next_block(heap_ptr, heap_ptr->next_block))
  {
    MEMHEAP_ASSERT(heap_ptr->next_block->forw_offset > 0);
    MEMHEAP_ASSERT(followingBlock->forw_offset > 0);

    if (heap_ptr->next_block->free_flag)
    {        
      
      if (heap_ptr->next_block->forw_offset >= size_needed) {
        return heap_ptr->next_block;
      }
    }
  }
  return 0;       /* didn't find anything */
} /* END mem_find_free_block */


