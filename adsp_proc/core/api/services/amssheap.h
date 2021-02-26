#ifndef AMSSHEAP_H
#define AMSSHEAP_H
/**
  @file amssheap.h
  @brief A simple sub-allocator to manage memory allocations and deallocations
  using a Next Fit strategy. 

*/

/*===========================================================================*/
/*
Copyright (c) 2013,2017 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
*/
/*===========================================================================

                                 Edit History

$Header: //components/rel/core.qdsp6/2.1/api/services/amssheap.h#2 $ $DateTime: 2017/09/14 04:56:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/14/17   nk      Changes to support Remote heap 
12/12/13   rks     Created.
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include <stddef.h>
#include "memheap.h"
/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif
/*===========================================================================

FUNCTION amssheap_get_current_heap_usages

DESCRIPTION
  returns the current amss_mem_heap usage(i.e. in use memory)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
ARGUMENTS
  None

===========================================================================*/
unsigned long amssheap_get_current_heap_usages(void);

/*===========================================================================

FUNCTION amssheap_get_current_in_use_blocks_count

DESCRIPTION
  returns the total current in use blocks in amss_mem_heap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
ARGUMENTS
  None
===========================================================================*/
unsigned long amssheap_get_current_in_use_blocks_count(void);
/*===========================================================================

FUNCTION amssheap_mem_memalign

DESCRIPTION
  This function allocate a memory block from amss_mem_heap of size(size)
  having the alocated memory block address aligned with block_size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

ARGUMENTS
  block_size - need to aligned with
  size       - size requested memory size

===========================================================================*/
void* amssheap_mem_memalign(
  size_t           block_size, /*need to aligned with*/
  size_t           size /*requested memory size*/
);

/*===========================================================================

FUNCTION amssheap_mem_get_block_size

DESCRIPTION
  Returns the block size from a assigned ptr in the amss_mem_heap.

  @param[in] void Pointer to the memory block to be queried.

  @return
  Returns the size (in bytes) of the block, excluding the block header 
  overhead and the unused bytes at the end of the block.

===========================================================================*/
unsigned long amssheap_mem_get_block_size (void *ptr);

/*===========================================================================

FUNCTION amssheap_add_section

DESCRIPTION
  Add a memory section to amss_mem_heap.

===========================================================================*/
void amssheap_add_section(
   /* Pointer to contiguous block of memory used for this section  */
   void                          *section_start,
   /* Size of the section  */
   unsigned long                  section_size
);

/**
  Returns the amss_mem_heap statistics for a thread with given memheap TLS index 

  @param[in]  thread_id    Thread index
  @param[out] stats double pointer to the heap statistics structure having the 
              statistics for the given thread id. 

  @return
   0 SUCCESS
  -1 Invalid arguments
  -2 Heap Statistics not enabled for this heap

  @dependencies
  The heap must have been initialized previously. \n
*/
#ifdef FEATURE_MEM_DEBUG_COMMON
int amssheap_get_task_stats(uint32 thread_id, memheap_task_stats_type **stats);
#endif

/**
  Register callbacks with amss_mem_heap to be called when the heap threshold goes 
below a value or if a section is released. 
 
  @param[in] mem_sections_register_struct

  @return
  Registration was successful.

  @dependencies
  None.
*/

unsigned int amss_mem_heap_register_callbacks(
   mem_sections_register_struct *sections_register);

/**
  Set the threshold for the largest free block available in amss heap.
Once the size of the largest free block goes below the threshold 
the memheap_warning_low_func_type callback will be called.
 
  @param[in] low_mem_block_size  size of free block to trigger 
                    request for more memory.

  @return
  SUCCESS 

  @dependencies
  None.
*/
unsigned int amss_mem_heap_set_thresholds(
      mem_heap_thresholds *thresholds);

#ifdef __cplusplus
   }
#endif

#endif /* AMSSHEAP_H */
