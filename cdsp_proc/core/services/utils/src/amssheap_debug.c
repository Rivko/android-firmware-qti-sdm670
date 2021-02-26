/*==========================================================================

                         AMSS Heap Debug Module

DESCRIPTION
  This file contains the AMSS heap debug functions and associated data.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The memdebug_init() function must be called before any other function
  in this module.

Copyright (c) 2006-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.qdsp6/2.1.c4/services/utils/src/amssheap_debug.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/05/13  ps     Add debug information to the block header
 09/05/12  mg     Modem heap using memheap.
 08/29/06  jc     Initial release.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "memheap.h"
#include "amssassert.h"
#ifdef FEATURE_MEM_DEBUG_COMMON

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

/* Code to enter and exit critical sections */
#ifdef FEATURE_MEMHEAP_MT
#define BEGIN_CRITICAL_SECTION(heap) (heap)->lock_fnc_ptr(heap)
#define END_CRITICAL_SECTION(heap)   (heap)->free_fnc_ptr(heap)
#else
#define BEGIN_CRITICAL_SECTION(heap)
#define END_CRITICAL_SECTION(heap)
#endif




extern mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr,
     /*  The block in the heap for which to return the following heap
         block
     */
   const unsigned int          check_next_section
);

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/






/*============================================================================
                          GLOBAL VARIABLES
============================================================================*/

/* All local module data */


/*============================================================================
                          LOCAL VARIABLES
============================================================================*/

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/

/*============================================================================
                          FORWARD DECLARATIONS
============================================================================*/
/*===========================================================================

FUNCTION memdebug_init

DESCRIPTION
  This function initializes local data for the memory debug module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_init(mem_heap_type *heap_ptr)
{
#ifdef FEATURE_MEM_DEBUG_ARRAY
  int i;
  /* memset(&memdebug, 0, sizeof(memdebug)); */
  for(i=0;i< MEMDEBUG_DB_SIZE;i++)
  {
	  heap_ptr->free_index_array[i]=i;
  }
#endif
	return;
}

/*===========================================================================

FUNCTION memdebug_malloc

DESCRIPTION
  This function logs a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc
(
  mem_heap_type *heap_ptr,
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr,         /* Pointer to allocated memory block */
  uint32 stats_index /*Index to statistics array which can be used to get tcb information*/
)
{
  mem_block_header_type *theBlock;
#ifdef FEATURE_MEM_DEBUG_ARRAY
  uint32           db_index_local;
#endif

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

#if defined(FEATURE_QDSP6) && defined(FEATURE_MEM_DEBUG_ARRAY)
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }
#endif

#ifdef FEATURE_MEM_DEBUG_ARRAY  
  //assign the new index
  db_index_local  =heap_ptr->free_index_array[heap_ptr->free_index_array_index];
  heap_ptr->free_index_array_index = (heap_ptr->free_index_array_index+1) % MEMDEBUG_DB_SIZE;
  //memdebug.db_index=db_index_local;
  theBlock->header_debug_info.debug_index=db_index_local;
  heap_ptr->debug_array->db[db_index_local].stats_index = stats_index;
  

  /* Add the entry */
  heap_ptr->debug_array->db[db_index_local].ptr = ptr;
  heap_ptr->debug_array->db[db_index_local].caller_ptr = caller_ptr;
  heap_ptr->debug_array->db[db_index_local].size = size;
#endif /* FEATURE_MEM_DEBUG_ARRAY */

#ifdef FEATURE_MEM_DEBUG
/* Store debug information in the block header*/
	 theBlock->header_debug_info.caller_ptr = caller_ptr;
	 theBlock->header_debug_info.stats_index =  stats_index;
#endif /* FEATURE_MEM_DEBUG */
 
}

/*===========================================================================

FUNCTION memdebug_malloc_updateparams

DESCRIPTION
  This function updates the information about a memory block allocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_malloc_updateparams
(
  mem_heap_type *heap_ptr,
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr,         /* Pointer to allocated memory block */
  uint32 stats_index
)
{
  mem_block_header_type *theBlock;
#ifdef FEATURE_MEM_DEBUG_ARRAY
  uint32           db_index_local;
#endif

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

#if defined(FEATURE_QDSP6) && defined(FEATURE_MEM_DEBUG_ARRAY)
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }
#endif

#ifdef FEATURE_MEM_DEBUG_ARRAY
  //assign the new index
  db_index_local  = theBlock->header_debug_info.debug_index;

  /* Add the entry */
  heap_ptr->debug_array->db[db_index_local].ptr = ptr;
  heap_ptr->debug_array->db[db_index_local].caller_ptr = caller_ptr;
  heap_ptr->debug_array->db[db_index_local].size = size;
  heap_ptr->debug_array->db[db_index_local].stats_index = stats_index;
#else
  theBlock->header_debug_info.caller_ptr = caller_ptr;
  theBlock->header_debug_info.stats_index=stats_index;
#endif
 
}



void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr,size_t size)
{
#ifdef FEATURE_MEM_DEBUG_ARRAY
  int db_index=-1;
#endif
  mem_block_header_type *theBlock;

  if (ptr == NULL)
  {
    return;
  }

  theBlock = (mem_block_header_type *)
                     ((char *) ptr - sizeof(mem_block_header_type));

#if defined(FEATURE_QDSP6) && defined(FEATURE_MEM_DEBUG_ARRAY)
  if(size <= 4)
  {
    theBlock = (mem_block_header_type *)
                            ((char *) ptr - sizeof(mem_block_header_type)+ 4);
  }
#endif


#ifdef FEATURE_MEM_DEBUG_ARRAY
  db_index=theBlock->header_debug_info.debug_index;

  if(( db_index >= 0) && (db_index < MEMDEBUG_DB_SIZE_MASK) && (heap_ptr->debug_array->db[db_index].ptr == ptr))
  {
      heap_ptr->debug_array->db[db_index].caller_ptr = caller_ptr;
  }
#endif

#ifdef FEATURE_MEM_DEBUG
	 theBlock->header_debug_info.caller_ptr = caller_ptr;
#endif

}
/*===========================================================================

FUNCTION memdebug_free

DESCRIPTION
  This function logs a memory block free.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void memdebug_free
(
  mem_heap_type *heap_ptr,
  void *ptr,         /* Pointer to allocated memory block */
  uint32 db_index
)
{
#ifdef FEATURE_MEM_DEBUG_ARRAY
  if((db_index >= 0) && (db_index < MEMDEBUG_DB_SIZE_MASK) && (heap_ptr->debug_array->db[db_index].ptr == ptr))
  {
    /* Clear the entry */
    memset(&heap_ptr->debug_array->db[db_index], 0, sizeof(heap_ptr->debug_array->db[db_index]));
    if((heap_ptr->free_index_array_index-1) >= 0)
      {
        heap_ptr->free_index_array[heap_ptr->free_index_array_index-1]=db_index;
        heap_ptr->free_index_array_index--;
      }
  }
#endif
}

#endif  /* FEATURE_MEM_DEBUG || FEATURE_MEM_DEBUG_ARRAY */
