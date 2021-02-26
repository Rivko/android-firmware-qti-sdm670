#ifndef BOOT_CACHE_H
#define BOOT_CACHE_H

/*=============================================================================

                    Boot Cache MMU Header File

GENERAL DESCRIPTION
  This header file contains the public interface for the cache
  and mmu control functions.
          
Copyright 2015 by Qualcomm Technologies, Incorporated.  
                      All Rights Reserved.
=============================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/15   kedara  Added dcache_inval_region.
08/01/15   kedara  created.
===========================================================================*/




/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"


/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/



/*===========================================================================

**  Function :  mmu_flush_cache

** ==========================================================================
*/
/*!
* 
* @brief
*   Flush the entire cache system. The data cache flush is achieved using 
*   atomic clean / invalidates working outwards from L1 cache. This is done 
*   using Set/Way based cache maintainance instructions.
*   The instruction cache can still be invalidated back to the point of
*   unification in a single instruction.  
*
* @par Dependencies
*   None
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void mmu_flush_cache
( 
  void 
);

/*===========================================================================

**  Function :  dcache_flush_region

** ==========================================================================
*/
/*!
*
* @brief
*   Cleans and invalidates a memory region in the cache. Please note that the
*   data in the cache would be written back to main memory if dirty and the 
*   region invalidated. So any further access to the data would result in a 
*   cache-miss.
*   @param[in] addr   Start address of memory region
*   @param[in] length Length of memory region
*
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void dcache_flush_region(void *addr, unsigned int length);

/*===========================================================================

**  Function :  dcache_inval_region

** ==========================================================================
*/
/*!
*
* @brief
*   Invalidate a memory region in the cache. Please note that the data in 
*   the cache will not be written back to the main memory. However, any 
*   further accesses to data in this region would result in a cache-miss.  
*
*   @param[in] addr   Start address of memory region
*   @param[in] length Length of memory region
*
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void dcache_inval_region(void *addr, unsigned int length);

#endif /* BOOT_CACHE_H */
