#ifndef CACHE_MMU_H
#define CACHE_MMU_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     Q D S P 6   C A C H E   A N D   M M U   C O N T R O L

DESCRIPTION

   This header file contains the public interface for the QDSP6 cache
   and mmu control functions.
          
EXTERNALIZED FUNCTIONS
  
mmu_invalidate_data_cache_lines
   Invalidate cache lines for a specified address range.

dcache_inval_region
   Invalidate a memory region in the cache.

dcache_clean_region
   Cleans a memory region in the cache.

dcache_clean_line
   Cleans a line in the cache.

dcache_clean_inval_region
   Cleans and invalidates a memory region in the data cache.

memory_barrier()
  Creates a memory barrier.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2003-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/api/kernel/arch/qdsp6/cache_mmu.h#1 $

============================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
  FUNCTION  MMU_INVALIDATE_DATA_CACHE_LINES

  DESCRIPTION
    Invalidate cache lines for a specified address range.
      
  PARAMETERS
    start_ptr - base address of data region to invalidate.
    byte_cnt  - number of bytes in data region to invalidate.
        
  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    Data cache lines that contain bytes from the specified region will be
    invalidated.

===========================================================================*/
void mmu_invalidate_data_cache_lines
(
  void *       start_ptr,
  unsigned int byte_cnt
);

/*============================================================================
; dcache_inval_region
;
; PROTOTYPE
;   void dcache_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Invalidate a memory region in the cache.    
;============================================================================*/
void dcache_inval_region(void *addr, unsigned int length);

/*============================================================================
; dcache_clean_line
;
; PROTOTYPE
;   void dcache_clean_line(void *addr);
;
; ARGS
;   void *addr            - Address in cache line
;
; DESCRIPTION
;   Cleans a line in the cache
;============================================================================*/
void dcache_clean_line(void *addr);

/*============================================================================
; dcache_clean_region
;
; PROTOTYPE
;   void dcache_clean_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans a memory region in the cache.    
;============================================================================*/
void dcache_clean_region(void *addr, unsigned int length);

/*============================================================================
; dcache_clean_inval_region
;
; PROTOTYPE
;   void dcache_clean_inval_region(void *addr, unsigned int length);
;
; ARGS
;   void *addr            - Start address of memory region
;   unsigned int length   - Length of memory region
;
; DESCRIPTION
;   Cleans and invalidates a memory region in the data cache.    
;============================================================================*/
void dcache_clean_inval_region(void *addr, unsigned int length);

/*============================================================================
; memory_barrier
;
; PROTOTYPE
;   void memory_barrier(void);
;
; ARGS
;
; DESCRIPTION
;   Creates a memory barrier
;
; DEPENDENCIES
;   On Q6 coress, the "barrier" instruction is a user
;   level instruction to ensure "all accesses before the barrier will be 
;   globally observable before any access after the barrier may be observed.
;============================================================================*/
void memory_barrier(void);

#ifdef __cplusplus
}
#endif
#endif /* CACHE_MMU_H */
