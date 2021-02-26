#ifndef BOOT_CACHE_MMU_H
#define BOOT_CACHE_MMU_H

/*=============================================================================

                    Boot Cache MMU Header File

GENERAL DESCRIPTION
  This header file contains the public interface for the cache
  and mmu control functions.
          
Copyright 2010-2015, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/17   rohik   Function added to check if I-cache and D-cache are enabled
06/29/17   yps     Added mmu_inval_trans_tbl_entry
06/27/17   yps     remove using set/way apis
06/22/17   yps     Added boot_enable_dcache() &boot_flush_regions_disable_dcache()
01/30/17   ds      find entry and unmap api
10/22/15   plc     Added prototype for mmu_invalidate_tlb(), EL-agnostic call
09/22/15   kpa     Added boot_mmu_get_page_table_entry_addr
08/01/15   kpa     Move cache management public api's to boot_cache.h
11/25/14   kpa     Added mmu_reconfigure
04/08/14   ck      Added boot_l2_page_table_init
04/03/14   plc     Create explicit calls for TLBI EL3 and EL1, pass L1 PT 
                   pointer to mmu-init function
03/25/14   plc     Update arguments for  boot_mmu_copy_page_table api
01/16/14   plc     Change to 64-bit armv8 functions, will revisit to add back 
                   support for legacy 32-bit armv7 style MMU tables
03/28/13   dh      Add mmu_get_page_table_base
03/13/13   kpa     Added mmu_set_dacr
03/01/13   jz      Expose mmu_flush_cache_and_disable
10/16/12   sl      Added boot_comdef.h
03/27/12   dh      Added boot_mmu_page_table_map_single_mem_block,
                   rename boot_mmu_populate_page_table_mem_map to
                   boot_mmu_page_table_map_mem_block_list
03/14/12   dh      Added boot_mmu_populate_page_table_mem_map,
                   boot_mmu_relocate_page_table and mmu_relocate_page_table_base
                   remove cache_mmu_re_enable
02/03/12   dh      Added mmu_invalidate_tlb
09/13/11   dh      Added dcache_flush_region
08/09/10   aus     Created.
===========================================================================*/



/*---------------------------------------------------------------------------
  Define DACR config value
---------------------------------------------------------------------------*/
/*Configure all domains as clients. If the domain remains as manager the
  TLB entry domain configurations are ignored by cpu during accesses and hence
  the permissions we set might not have any effect */
#define DACR_ALL_DOMAIN_CLIENTS        0x55555555


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"
#include "boot_cache.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

typedef struct{
  uint64 start_addr;  
  uint64 size;
}xbl_internal_mem_region_type;

/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

/* Forward declaration */ 
struct mem_block;


/*===========================================================================

**  Function :  boot_mmu_page_table_map_mem_block_list

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to the memory map
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
boot_boolean boot_mmu_page_table_map_mem_block_list
(
  uint64 *mmu_l1_pagetable,
  const struct mem_block  *map 
);


/*===========================================================================

**  Function :  boot_mmu_page_table_map_single_mem_block

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map block.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to the memory map block
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
boot_boolean boot_mmu_page_table_map_single_mem_block
(
  uint64 *mmu_l1_pagetable,
  const struct mem_block  *map_ptr
);


/*===========================================================================

**  Function :  boot_mmu_copy_page_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Copy the source L1, L2 and L3 page table to destination in DDR.
*   It will NOT relocate the page table base register(TTBR0) to the 
*   destination L1 page table. 
*   This API will assume the destination L1 and L2 page tables are 
*   already invalidated
*
* @param
*
* @par Dependencies
*   destination L1, L2, and L3 page tables must be allocated and invalidated, 
*   destination L2, L3 page tables must reside in contiguous memory
*
* @retval
*   True if successful, 
*   False if there is not enough space to copy existing page tables
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_mmu_copy_page_table
(
  void
);

/*===========================================================================

**  Function :  mmu_enable_instruction_cache

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables instruction cache.  
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
void mmu_enable_instruction_cache
( 
  void 
);

/*===========================================================================

**  Function :  cache_set_memory_barrier_location

** ==========================================================================
*/
/*!
* 
* @brief
*   Sets the location of the actual memory barrier in SMEM  
* 
* @param[in] memory_barrier_ptr Pointer to a non-buffered or non-cached memory
*
* @par Dependencies
*   cache_set_memory_barrier_location() must be called with a pointer to
*   non-buffered or non-cached memory before any call to memory_barrier(),
*   for the instruction and data streams to be syncronized by
*   the memory_barrier() operation.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void cache_set_memory_barrier_location
(
   void *memory_barrier_ptr
);

/*===========================================================================

**  Function :  memory_barrier

** ==========================================================================
*/
/*!
* 
* @brief
*   Creates a memory barrier  
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
void memory_barrier
(
  void
);


/*===========================================================================

**  Function :  mmu_invalidate_tlb

** ==========================================================================
*/
/*!
* 
* @brief
*   Invalidates the entire Translation Look-aside Buffer (TLB) as a unified
*   operation (Data and Instruction). Invalidates all unlocked entries in the
*   TLB. Causes the prefetch buffer to be flushed. All following instructions
*   are fetched after the TLB invalidation.
*   We should do this after any change of page table  
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
void  mmu_invalidate_tlb(void);

/*===========================================================================

**  Function :  boot_enable_mmu

** ==========================================================================
*/
/*!
* 
* @brief
*   Enables the MMU 
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
void  boot_enable_mmu(void);

/*===========================================================================

**  Function :  boot_disable_mmu

** ==========================================================================
*/
/*!
* 
* @brief
*   Disables the MMU 
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
void  boot_disable_mmu(void);

/*===========================================================================

**  Function :  mmu_relocate_page_table_base

** ==========================================================================
*/
/*!
* 
* @brief
*  Relocate the base register(TTBR0) of page table to the given address, 
*  all attributes of TTBR0 will be perserved
* 
* @par Dependencies
*   None
*
* @param[in] dst_l1_pagetable address of the destination page table base.
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/  
void mmu_relocate_page_table_base(uint64 *dst_l1_pagetable);

/* SBL L1 and L2 page table defines */
extern uint64 mmu_l1_pagetable[];
extern uint64 mmu_l2_pagetable[];

/*===========================================================================

**  Function :  mmu_set_dacr

** ==========================================================================
*/
/*!
* 
* @brief
*  Configure DACR, Domain Access Control Register.
* 
* @param[in] 
*  uint32 dacr_value:   Value to be written to DACR.
* 
* @par Dependencies
*  None
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void mmu_set_dacr( uint32 dacr_value);


/*===========================================================================

**  Function :  mmu_get_page_table_base

** ==========================================================================
*/
/*!
* 
* @brief
*  Return the page table base address in TTBR0. Attributes bits are cleared
* 
* @par Dependencies
*  None
* 
* @retval
*  page table base address in TTBR0
* 
* @par Side Effects
*  None
* 
*/
uint64 *mmu_get_page_table_base(void);


/*============================================================================
; mmu_reconfigure
;  INPUT       - None
;  DESCRIPTION -
;        Initializes the ARM MMU.
;                1- sets a default value in the control register
;                2- sets the physical address of the page table
;===========================================================================*/
void mmu_reconfigure(void);

/*=============================================================================
FUNCTION BOOT_INIT_PAGE_TABLE

DESCRIPTION
  

DEPENDENCIES
  

RETURN VALUE
  NONE

SIDE EFFECT
  All entries in the page table will be over written and updated.

=============================================================================*/
void boot_init_page_table(uint64*);

/*===========================================================================
**  Function :  boot_l2_page_table_init
** ==========================================================================
*/
/*!
* 
* @brief
*   Calculate the number of L2 page table entries available and set L2
    page table pointer.
* 
* @param[in]
*   uint32 * mmu_l1_pagetable_base
*   uint32   mmu_pagetable_size
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
void boot_l2_page_table_init(uint64 * mmu_l1_pagetable_base,
                             uint64 mmu_pagetable_size);

/*===========================================================================

**  Function :  boot_mmu_get_page_table_entry_addr

** ==========================================================================
*/
/*!
* 
* @brief
*   Retrieves address of page table entry corresponding to input address.
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] Memory address whose page table entry is to be searched
*
* @param[out] Address of page table entry if found
*
* @retval
*   boolean. TRUE if entry is found
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_mmu_get_page_table_entry_addr
(
  uintnt *mmu_l1_pagetable,
  uintnt memory_address,
  uintnt *page_table_entry_ptr
);

/*===========================================================================

**  Function :  boot_mmu_find_entry_and_unmap

** ==========================================================================
*/
/*!
* 
* @brief
*   Retrieves address of page table entry corresponding to input address 
*   and unmaps it
* 
* @par Dependencies
*   None
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] Memory address whose page table entry is to be searched
*
* @param[in] Size of the total memory to be unmapped
*
* @param[out] unmapping successful or not
*
* @retval
*   boolean. TRUE if unmapping was successful
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_mmu_find_entry_and_unmap
(
  uintnt *mmu_l1_pagetable,
  uintnt memory_address,
  uintnt size
);

/*===========================================================================

**  Function :  boot_dcache_enable

** ==========================================================================
*/
/*!
* 
* @brief
*   Enable the Dcache.  
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
void boot_enable_dcache(void);

/*===========================================================================

**  Function :  boot_flush_regions_disable_dcache

** ==========================================================================
*/
/*!
*
* @brief
*   Cleans and invalidates a memory regions in the cache and then disable cache. Please note that the
*   data in the cache would be written back to main memory if dirty and the 
*   region invalidated. So any further access to the data would result in a 
*   cache-miss.
*   @param[in] ptr   Start address of memory regions
*   @param[in] size Length of memory regions
*
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void boot_flush_regions_disable_dcache
(
  xbl_internal_mem_region_type* ptr,
  uint64 size
);                             
/*===========================================================================

**  Function :  mmu_inval_trans_tbl_entry

** ==========================================================================
*/
/*!
*
* @brief
*   Replace the old translation table entry with invalid entry, and excute a DSB instruction
* @param[in] ttptr   the translation table entry address
*
* @retval
*   None
* 
* @par Side Effects
*   None
*/
void mmu_inval_trans_tbl_entry
(
  void *ttptr
);
            
/*===========================================================================

**  Function :  boot_is_icache_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   Determine if I-cache is enabled
* 
* @par Dependencies
*   None
*
* @retval
*   uint32 non-zero if I-cache is enabled
* 
* @par Side Effects
*   None.
* 
*/
uint32 boot_is_icache_enabled ( void );

/*===========================================================================

**  Function :  boot_is_dcache_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   Determine if D-cache is enabled
* 
* @par Dependencies
*   None
*
* @retval
*   uint32 non-zero if D-cache is enabled
* 
* @par Side Effects
*   None.
* 
*/
uint32 boot_is_dcache_enabled ( void );

#endif /* BOOT_CACHE_MMU_H */
