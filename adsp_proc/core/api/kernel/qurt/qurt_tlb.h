#ifndef QURT_TLB_H
#define QURT_TLB_H

/**
  @file qurt_tlb.h 

  @brief  Prototypes of TLB API  
		The following APIs allow explicit control of the portion of TLB that between TLB_firs_replaceble and TLB_LAST_REPLACEABLE. 
		Both are non-configurable for the time being. This portion of TLB is permanently assigned/locked unless manually removed 
		by qurt_tlb_remove. Implementation does not change depending on the configuration, i.e. whether CONFIG_STATIC is set or not. 
		In CONFIG_STATIC=y, TLB_LAST_REPLACEABLE is set to the last tlb index, which indicates that the entire TLB is permanently 
		assigned and is not backed up by pagetable (pagetable does not exist). TLB indecies are maintained through a 64-bit bitmask. 
		A new entry is placed in the first available free slot. 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file
12/16/12   cm      Tech pubs edits
=============================================================================*/
#include <qurt_types.h>

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**@ingroup func_qurt_tlb_entry_create
  Creates a new TLB entry with the specified mapping attributes in the Hexagon processor's
  TLB. \n
  @note1hang If the specified attributes are not valid (i.e., the address is not aligned with the
             size), the entry is created and an error result is returned.\n
  @note1cont To set the G bit in the new TLB entry, set the asid argument to -1.
  
  @param[out]  entry_id         TLB entry identifier.
  @param[in]   vaddr 			Virtual memory address.
  @param[in]   paddr  			Physical memory address.
  @param[in]   size  			Size of memory region to map (in bytes).
  @param[in]   cache_attribs  	Cache mode (writeback, etc.).
  @param[in]   perms  			Access permissions.  
  @param[in]   asid  			ASID (space ID).
 
  @return
  QURT_EOK -- TLB entry was successfully created.\n
  QURT_EFATAL -- Entry was not created; the TLB is full. \n
  QURT_ETLBCREATESIZE -- Entry was not created; the incorrect size was specified. \n
  QURT_ETLBCREATEUNALIGNED -- Entry was not created; an unaligned address was specified.

 */
int  qurt_tlb_entry_create (unsigned int *entry_id, qurt_addr_t vaddr, qurt_paddr_t paddr, qurt_size_t size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perms, int asid);


int qurt_tlb_entry_create_64 (unsigned int *entry_id, qurt_addr_t vaddr, qurt_paddr_64_t paddr_64, qurt_size_t size, qurt_mem_cache_mode_t cache_attribs, qurt_perm_t perms, int asid);

/**@ingroup func_qurt_tlb_entry_delete 
  Deletes the specified TLB entry from the Hexagon processor's TLB.
  If the specified entry does not exist, no deletion occurs and an error result is returned.

  @param[in]   entry_id  TLB entry identifier.			

  @return
  QURT_EOK -- TLB entry was successfully deleted. \n
  QURT_EFATAL -- TLB entry does not exist.

  @dependencies
  None.
 **/
int qurt_tlb_entry_delete (unsigned int entry_id);

/**@ingroup func_qurt_tlb_entry_query
  Searches for the specified TLB entry in the Hexagon processor's TLB.
  If the TLB entry is found, its entry identifier is returned.

  @param[out]   entry_id     TLB entry identifier.  
  @param[in]    vaddr  		 Virtual memory address.
  @param[in]    asid 		 ASID (space ID).

  @return  
  QURT_EOK -- TLB entry was successfully returned. \n
  QURT_EFATAL -- TLB entry does not exist.

  @dependencies
  None.
 **/
int qurt_tlb_entry_query (unsigned int *entry_id, qurt_addr_t vaddr, int asid);

/**@ingroup func_qurt_tlb_entry_set
  Sets the TLB entry by storing an entry at the specified location 
  in the Hexagon processor's TLB.

  @param[in]   entry_id  		TLB entry identifier.
  @param[in]   entry  			64-bit TLB entry to store.

  @return
  QURT_EOK -- Entry was successfully stored in the TLB. \n
  QURT_EFATAL -- Entry was not set at the specified location.

  @dependencies
  None.
 **/
int qurt_tlb_entry_set (unsigned int entry_id, unsigned long long int entry);

/**@ingroup func_qurt_tlb_entry_get
  Gets the TLB entry. \n
  Returns the specified 64-bit TLB entry in the Hexagon processor's TLB.

  @param[in]    entry_id  	TLB entry identifier.
  @param[out]   entry       64-bit TLB entry.

  @return
  QURT_EOK -- TLB entry was successfully returned. \n
  QURT_EFATAL -- TLB entry does not exist.   

  @dependencies
  None.
 **/
int qurt_tlb_entry_get (unsigned int entry_id, unsigned long long int *entry);

/**@ingroup func_qurt_tlb_entry_get_available
  Gets the number of TLB entries.\n
  Returns the number of unused entries in the Hexagon processor's TLB.

  @note1hang Unused entries can be allocated with qurt_tlb_entry_create().
  
  @return
  Integer -- Number of unused TLB entries.
 
  @dependencies
  None.
 */
unsigned short qurt_tlb_entry_get_available(void);


/**@ingroup func_qurt_tlb_get_pager_physaddrs
Searches the Hexagon processor's TLB, and returns all physical addresses that belong to the pager.
Each address returned indicates the starting address of a an active page.

The function return value indicates the number of addresses returned.

NOTE - If the number of pager addresses available exceeds the specified size of the return array, 
this function returns only as many addresses as will fit in the array.

  @param[out]  addrs --     Pointer to the return array of pager physical addresses.
 
  @return
    Integer          --     Number of addresses returned in array \n

  @dependencies
    None.
*/

unsigned int qurt_tlb_get_pager_physaddr(unsigned int** pager_phys_addrs);

/**@ingroup func_qurt_tlb_get_pager_virtaddr
Searches the Hexagon processor's TLB, and returns all virtual addresses that belong to the pager.
Each address returned indicates the starting address of an active page.

The function return value indicates the number of addresses returned.

NOTE - If the number of pager addresses available exceeds the specified size of the return array, 
this function returns only as many addresses as will fit in the array.

  @param[out]  addrs --     Pointer to the return array of pager virtual addresses.
 
  @return
    Integer          --     Number of addresses returned in array \n

  @dependencies
    None.
*/

unsigned int qurt_tlb_get_pager_virtaddr(unsigned int** pager_virt_addrs);


#endif /* QURT_TLB_H */
