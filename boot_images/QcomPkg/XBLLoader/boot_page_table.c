/*=============================================================================

                       Boot Page Table

GENERAL DESCRIPTION
  This file contains definitions of functions for setting up a page table in SBL

Copyright 2012-2015,2017-2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/22/18   aus     Propagate attributes from L1/L2 to subsequent L2/L3 pagetables
06/29/17   yps     using break-before-make when updating translation table entries
06/27/17   yps     remove boot_init_page_table
01/30/17   digant  findentry_and_unmap
09/22/15   kpa     Added boot_mmu_get_page_table_entry_addr
01/26/15   kpa     Update error handling while mapping l2 l3 pages, update PT size
01/14/15   ck      Changed MMU_L1_PT_SIZE to MMU_NUM_L1_PT
01/13/15   ck      Fixed L3_PT_ADDRESS_BITS bitmask
12/08/14   ck      Added mmu_l1_pagetable entry for targets that cannot use PBL's
10/22/14   ck      Updated to ensure page tables are placed in ZI region
10/10/14   wg      Added logic to address mapping regions that cross 2MB L3 
                   bounds 
06/26/14   kedara  Fix compiler warnings.
06/06/14   plc     Set empty_l*_pagetable ptr and empty_l*page_ramaining 
                   pointers to what DDR pointers were set to after copy
06/02/14   plc     Update mmu_map_block_as_l1_pages to not overwrite existing 
                   table entries
05/05/14   plc     Adjust L2-page add so that L3 entries are only verified 
                   and if L2 is needed, then L2 entries made with default
				   attributes
04/10/14   plc     4KB align page tables and update boot_mmu_copy_page_table
04/03/14   plc     Fix bitmasks, limit calculations, use L1 base in IMEM as 
                   was used in PBL for L1 base, ensure L2 entries don't point 
				   to L3 table before overwriting them, index L2 entry 
				   correctly before checking for mapping to L3 table, call 
				   L2 map api from L3-map function, pull out TLBI functions
				   so that appropriate EL3/EL1 version can be called pre or
				   post QSEE execution accordingly
04/01/14   wg      Changed qmemcpy to use safe version qmemscpy.
03/25/14   plc     Updates to enable MMU and more appropriately configure
                   pagetables, also update page-table copy logic for Armv8
01/16/14   plc     Comment a few calls for 8994 virtio bringup, update to Armv8
11/24/13   plc     Fix casting issues with Aarch64
11/01/13   plc     Update for Arm6 syntax "attribute"
03/26/13   dh      in both L1 and L2 mapping, round up to page size alignment
10/17/12   dh      Fix the bug that when relocating L2 page tables L1 descriptor
		           is not updated to point to the new L2 table.
05/24/12   dh      Change boot_mmu_relocate_page_table to boot_mmu_copy_page_table
03/27/12   dh      Added boot_mmu_page_table_map_single_mem_block,
                   rename boot_mmu_populate_page_table_mem_map to
                   boot_mmu_page_table_map_mem_block_list
03/14/12   dh      Add boot_mmu_relocate_page_table for relocating page table.
                   Pass l1 page table base as an argument for all the page table APIs
                   
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "Target_cust.h"
#include "boot_comdef.h"
#include "boot_page_table_armv8.h"
#include "boot_cache_mmu.h"
#include "boot_util.h"
#include "boot_target.h"
#include "boot_error_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define L1_PT_INDEX 0x7FC0000000
#define L2_PT_INDEX 0x003FE00000
#define L3_PT_INDEX 0x00001FF000

#define L3_PT_ADDRESS_BITS 0x0000FFFFFFE00000

#define PT_ADDRESS_BITS   0x0000FFFFFFFFF000

#define PT_ATTRIBUTE_BITS 0xFFFF000000000FFF

#define PT_TABLE_DESC_BIT 0x2

#define PT_TABLE_VALID_ENTRY_BIT 0x1

/* SBL L1, L2 and L3 page table */

#ifdef FEATURE_EMPTY_L1_PAGE_TABLE
__attribute__((section(".bss.BOOT_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l1_pagetable[MMU_NUM_L1_PT];
#endif
__attribute__((section(".bss.BOOT_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l2_pagetable[MMU_L2_PT_SIZE*MMU_NUM_L2_PT];
__attribute__((section(".bss.BOOT_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l3_pagetable[MMU_L3_PT_SIZE*MMU_NUM_L3_PT];

__attribute__((section(".bss.BOOT_DDR_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l1_ddr_pagetable[MMU_NUM_DDR_L1_PT];
__attribute__((section(".bss.BOOT_DDR_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l2_ddr_pagetable[MMU_L2_PT_SIZE*MMU_NUM_DDR_L2_PT];
__attribute__((section(".bss.BOOT_DDR_PAGE_TABLE_ZONE"))) uint64 ALIGN(4*1024) mmu_l3_ddr_pagetable[MMU_L3_PT_SIZE*MMU_NUM_DDR_L3_PT];


/* Pointer to the next empty L2 page table we are going to use */
static uint64 *empty_l2_pagetable_ptr = mmu_l2_pagetable;

/* Pointer to the next empty L3 page table we are going to use */
static uint64 *empty_l3_pagetable_ptr = mmu_l3_pagetable;

/* Number of empty L2 page tables left */
static uint32 empty_l2_pagetable_num = MMU_NUM_L2_PT;

/* Number of empty L3 page tables left */
static uint32 empty_l3_pagetable_num = MMU_NUM_L3_PT;

/* Pointer to the next empty L2 page table we are going to use */
static uint64 *empty_l2_ddr_pagetable_ptr = NULL;

/* Pointer to the next empty L3 page table we are going to use */
static uint64 *empty_l3_ddr_pagetable_ptr = NULL;

/* Number of empty L2 page tables left */
static uint32 empty_l2_ddr_pagetable_num = MMU_NUM_DDR_L2_PT;

/* Number of empty L3 page tables left */
static uint32 empty_l3_ddr_pagetable_num = MMU_NUM_DDR_L3_PT;



/*===========================================================================

**  Function :  mmu_map_block_as_l1_sections

** ==========================================================================
*/
/*!
* 
* @brief
*   Maps a block descriptor into the page table as l1 sections
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
* 
* @par Dependencies
*   mmu_l1_pagetable must be allocated
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
static void mmu_map_block_as_l1_sections
(
  uint64 *mmu_l1_pagetable, 
  const struct mem_block *block
)
{
  uint64 address_base_index;
  uint64 limit;
  uint64 p_addr;

  /* Convert our base address into an index into the page table */
  address_base_index = (block->v_base & L1_PT_INDEX) >> 30;

  /* only map 1GB size sections ,  round up to 1GB alignment*/
  limit = address_base_index + ((block->size_in_kbytes + 1048575) >> 20);
  
  /* bits 38:30 provide the physical base address of the section */
  p_addr = block->p_base & L1_PT_INDEX; 


  for( ; address_base_index < limit; address_base_index++)
  {
    /*
   A Block descriptor for first stage, level one is as follows (Descriptor = 0b01):
     ___________________________________________________________________________________________________________________
    |       |        |  |	|    |        |                  |        |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------30|n-1 --12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:30]|UNK/SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
	                              *Output addr[47:30] specifies bits[47:30] 
								   of output addr (1GB block of memory) 
    */
    
	/* If L1 entry hasn't mapped any L2 page table, allocate a L2 page table for it */
    if((mmu_l1_pagetable[address_base_index] & PT_TABLE_DESC_BIT) == 0)
    {	
      mmu_l1_pagetable[address_base_index] =  (p_addr)                   |
                                              (block->access)            |
                                              (block->cachetype)         |
                                              (block->executable)        |
                                              MMU_PAGETABLE_BLOCK_DESCRIPTOR;
      
      /* If this L1 section is NS then update the NS bit of L1 section pagetable entry*/
      if(block->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
      {
        mmu_l1_pagetable[address_base_index] |= MMU_PAGETABLE_BLOCK_NS;
      }
      
      p_addr += 0x40000000; /* Point to next level */ 
    }
  }
   
} /* mmu_map_block_as_l1_sections */
 
/*===========================================================================

**  Function :  mmu_map_block_as_l2_pages

** ==========================================================================
*/
/*!
* 
* @brief
*   Maps a block descriptor into the page table as l2 pages
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
* 
* @par Dependencies
*   mmu_l1_pagetable must be allocated
*   There must be empty l2 page table left if new L2 page table is required
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
static void mmu_map_block_as_l2_pages
(
  uint64 *mmu_l1_pagetable,
  const struct mem_block *block 
)
{
  uint64 *l2_page_table;
  uint64  address_base_index, i;
  uint64  limit;
  uint64  p_addr;
  boot_boolean  l2_pt_exist = TRUE;
  uint64 attribute = 0;
  
  /* First initialize the first level descriptor for each 1 GB
     Bits[47:12] provide the physical base address of the level 2 page table

     ________________________________________________________________________________
    |  |     |  |   |       |        |                            |       |          |
    |63|62-61|60| 59|58---52|51----48|47------------------------12|11----2|1------- 0|
    |NS| AP  |XN|PXN|Ignored|UNK/SBZP|Next-level table addr[47:12]|Ignored|Descriptor|
    |__|_____|__|___|_______|________|____________________________|_______|__________|
        
  */
  
  /* Convert the virtual address[38:30] into an index of the L1 page table */
  address_base_index = (block->v_base & L1_PT_INDEX) >> 30;  
  
  /* Check if this 1GB entry has L2 page table mapped already */
  
  do
  {
    /* If L1 entry hasn't mapped any L2 page table, allocate a L2 page table for it */
    if((mmu_l1_pagetable[address_base_index] & PT_TABLE_DESC_BIT) == 0)
    {
      /* First check if we have any empty L2 page table left */
      BL_VERIFY(empty_l2_pagetable_num > 0, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
      
      /* Save attributes from the original entry */
	  attribute = mmu_l1_pagetable[address_base_index] & PT_ATTRIBUTE_BITS;
      
      /* We will now use the next empty l2 page table for this mapping
         Point L1 entry to the empty L2 page table*/
      l2_page_table = empty_l2_pagetable_ptr;
      mmu_l1_pagetable[address_base_index] = ((uint64)l2_page_table & PT_ADDRESS_BITS)  |          
                                             MMU_PAGETABLE_TABLE_DESCRIPTOR;
      
      /* If this L2 section is NS then update the NS bit of L1 coarse pagetable entry */
      if(block->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
      {
        mmu_l1_pagetable[address_base_index] |= MMU_PAGETABLE_TABLE_NS;
      }
      
      /* Advance pointer to next empty l2 page table */
      l2_pt_exist = FALSE;
      empty_l2_pagetable_ptr += MMU_L2_PT_SIZE;
      empty_l2_pagetable_num--;
    }
    
	/* If we are only verifying L2 entry for L3 request, break out here */
	else if ((block->memory_mapping == MMU_L3_SECTION_MAPPING) ||
	         (block->memory_mapping == MMU_L3_NS_SECTION_MAPPING))
	{
	  break;
	}

	
    /* Entry has L2 page table mapped already, so just get the existing L2 page table address */
    else
    {
      l2_page_table = (uint64*)(uintnt)(mmu_l1_pagetable[address_base_index] & PT_ADDRESS_BITS);  
    }
    
    /* The start physical address for the 1GB section placed inside the L2 page table */
    p_addr = block->p_base & L1_PT_INDEX;   
    
    /* Start index into the L2 page table for this section using the virtual address[29:21]*/
    address_base_index  = (block->v_base & L2_PT_INDEX) >> 21;
    
    /* The end index for the given section. This includes the total number of 2MB segments 
      in the given section, round up to 2MB alignment */
    limit = address_base_index + ((block->size_in_kbytes + 2047) >> 11);
    
    
    /*
     ___________________________________________________________________________________________________________________
    |       |        |  |	|    |        |                  |        |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------21|20----12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:21]|UNK/SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
	                              *Output addr[47:21] specifies bits[47:21] 
								   of output addr (2MB block of memory)	  
    */  
    
    /* Map all the 2MB segments in the 1GB section */
    for (i = 0; i < MMU_L2_PT_SIZE; i++)
    {
      /* If the index lies between the base index and the limit, then this is the given section 
      we need to map */
      if (((i >= address_base_index)  && (i < limit)) && 
          !((block->memory_mapping == MMU_L3_SECTION_MAPPING) ||
            (block->memory_mapping == MMU_L3_NS_SECTION_MAPPING)))
      {
	    /* If L2 entry hasn't mapped any L3 page table add the page, otherwise skip the mapping
           so we don't lose pointer to L3 page		*/
        if((l2_page_table[i] & PT_TABLE_DESC_BIT) == 0)
        {
          l2_page_table[i] =  (p_addr)                 |
                              (block->access)          |
                              (block->cachetype)       |
                              (block->executable)      |
                              MMU_PAGETABLE_BLOCK_DESCRIPTOR;
	      if(block->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
          {
            l2_page_table[i] |= MMU_PAGETABLE_BLOCK_NS;
          }
		}
      }
      /* If the 1GB was previously not mapped, 
        then put the remaining sections to attributes of the original entry */
      else if(!l2_pt_exist)
      {
        l2_page_table[i] = (p_addr)                              |
                           attribute        |
                           MMU_PAGETABLE_BLOCK_DESCRIPTOR;
      }
      
      /* Increment to the next 2MB segment in current L2 page table*/
      p_addr += 0x200000;
    }
    
  }while(0);
  
} /* mmu_map_block_as_l2_pages */

/*===========================================================================

**  Function :  mmu_map_block_as_l3_pages

** ==========================================================================
*/
/*!
* 
* @brief
*   Maps a block descriptor into the page table as l3 pages
*
* @param[in] mmu_l2_pagetable Pointer to the base of L2 page table
* 
* @par Dependencies
*   mmu_l1_pagetable must be allocated
*   mmu_l2_pagetable must be allocated
*   There must be empty l3 page table left if new L3 page table is required
*
* @retval
*   None
* 
* @par Side Effects
*   Existing entries in the pagetable will be replaced.
* 
*/
static void mmu_map_block_as_l3_pages
(
  uint64 *mmu_l1_pagetable,
  uint64 *mmu_l2_pagetable,
  const struct mem_block *block 
)
{
  uint64  *l2_page_table;
  uint64  *l3_page_table;
  uint64  address_base_index, i;
  uint64  limit;
  uint64  p_addr;
  boot_boolean  l3_pt_exist = TRUE;
  uint64 attribute = 0;
  
  /* First initialize the second level descriptor for each 2 MB
     Bits[47:12] provide the physical base address of the level 3 page table

     ________________________________________________________________________________
    |  |     |  |   |       |        |                            |       |          |
    |63|62-61|60| 59|58---52|51----48|47------------------------12|11----2|1------- 0|
    |NS| AP  |XN|PXN|Ignored|UNK/SBZP|Next-level table addr[47:12]|Ignored|Descriptor|
    |__|_____|__|___|_______|________|____________________________|_______|__________|
        
  */
  
  /* Make sure that this block is mapped in the L2 first.  If already mapped, 
     "mmu_map_block_as_l2_pages() will have no effect besides verification          */
  mmu_map_block_as_l2_pages(mmu_l1_pagetable, block);
  
  /* Now convert the virtual address[38:30] into an index of the L1 page table
     to find the correct L2 page to access.  All L3 mappings have been preceded with
	 an L2 mapping call, so it's assumed that L1 has a table page entry pointing to 
	 a valid L2 page table entry.                                                   */
  address_base_index = (block->v_base & L1_PT_INDEX) >> 30;  
  l2_page_table = (uint64*)(uintnt)(mmu_l1_pagetable[address_base_index] & PT_ADDRESS_BITS);
  
  /* Convert the virtual address[29:21] into an index of the L2 page table */
  address_base_index = (block->v_base & L2_PT_INDEX) >> 21;  
  
  /* Check if this 2MB entry has L3 page table mapped already */  
  do
  {
    /* If L2 entry hasn't mapped any L3 page table, allocate a L3 page table for it */
    if((l2_page_table[address_base_index] & PT_TABLE_DESC_BIT) == 0)
    {
      /* First check if we have any empty L3 page table left */
      BL_VERIFY(empty_l3_pagetable_num > 0, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
      
      /* Save attributes from the original entry */
	  attribute = l2_page_table[address_base_index] & PT_ATTRIBUTE_BITS;
      
      /* We will now use the next empty l2 page table for this mapping
         Point L2 entry to the empty L3 page table*/
      l3_page_table = empty_l3_pagetable_ptr;
      l2_page_table[address_base_index] = ((uint64)l3_page_table & PT_ADDRESS_BITS)  |           
                                             MMU_PAGETABLE_TABLE_DESCRIPTOR;
      
      /* If this L3 section is NS then update the NS bit of L2 table pagetable entry */
      if(block->memory_mapping == MMU_L3_NS_SECTION_MAPPING)
      {
        l2_page_table[address_base_index] |= MMU_PAGETABLE_TABLE_NS;
      }
      
      /* Advance pointer to next empty l3 page table */
      l3_pt_exist = FALSE;
      empty_l3_pagetable_ptr += MMU_L3_PT_SIZE;
      empty_l3_pagetable_num--;
    }
    
    /* Entry has L3 page table mapped already, so just get the existing L3 page table address */
    else
    {
      l3_page_table = (uint64*)(uintnt)(l2_page_table[address_base_index] & PT_ADDRESS_BITS);  
    }
    
    /* The start physical address for the 2MB section placed inside the L3 page table */
    p_addr = block->p_base & L3_PT_ADDRESS_BITS;   
    
    /* Start index into the L3 page table for this section using the virtual address[20:12]*/
    address_base_index  = (block->v_base & L3_PT_INDEX) >> 12;
    
    /* The end index for the given section. This includes the total number of 4KB segments 
      in the given section, round up to 4K alignment */
    limit = address_base_index + ((block->size_in_kbytes + 3) >> 2);
    
    
    /*
     __________________________________________________________________________________________________________
    |       |        |  |   |    |        |                  |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:12]|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|__|__|_______|_______|__|_____________|__________|
	                              *Output addr[47:12] specifies bits[47:12] 
								   of output addr (4kb page of memory)	
    
    */  
    
    /* Map all the 4KB segments in the 2MB section */
    for (i = 0; i < MMU_L3_PT_SIZE; i++)
    {
      /* If the index lies between the base index and the limit, then this is the given section 
      we need to map */
      if ((i >= address_base_index)  && (i < limit))
      {
        l3_page_table[i] =  (p_addr)                 |
                            (block->access)          |
                            (block->cachetype)       |
                            (block->executable)      |
                            MMU_PAGETABLE_PAGE_DESCRIPTOR;
	    if(block->memory_mapping == MMU_L3_NS_SECTION_MAPPING)
        {
          l3_page_table[i] |= MMU_PAGETABLE_BLOCK_NS;
        }
      }
      /* If the 2MB was previously not mapped, 
        then put the remaining sections to attributes of the original entry*/
      else if(!l3_pt_exist)
      {
        l3_page_table[i] = (p_addr)                              |
		                   attribute            |
                           MMU_PAGETABLE_PAGE_DESCRIPTOR;
      }
      
      /* Increment to the next 4KB segment in current L2 page table*/
      p_addr += 0x1000;
    }
    
  }while(0);
  
} /* mmu_map_block_as_l3_pages */


/*===========================================================================

**  Function :  boot_mmu_page_table_map_mem_block_list

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a memory map list.
* 
* @par Dependencies
*   Same size space allocated for OCMEM and DDR pagetables
*
* @param[in] mmu_l1_pagetable Pointer to the base of L1 page table
*
* @param[in] map Pointer to start of the memory map list
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
)
{
  boot_boolean result = TRUE;
  const struct mem_block  *map_ptr;
  struct mem_block temp_table;
  uint32 end_address;
  uint32 l2_block_start;
  uint32 l2_block_end;
  
  for (map_ptr = map; map_ptr->v_base != MMU_LAST_ADDRESS; map_ptr++)
  {
    if(map_ptr->memory_mapping == MMU_L1_SECTION_MAPPING ||
       map_ptr->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
    {
      mmu_map_block_as_l1_sections(mmu_l1_pagetable, map_ptr);
    }
    else if(map_ptr->memory_mapping == MMU_L2_SECTION_MAPPING ||
            map_ptr->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
    {
      mmu_map_block_as_l2_pages(mmu_l1_pagetable, map_ptr);
    }
	else if(map_ptr->memory_mapping == MMU_L3_SECTION_MAPPING ||
            map_ptr->memory_mapping == MMU_L3_NS_SECTION_MAPPING)
    {
      /* Checking for block overflow, if an L3 block of pages overlaps from one 
         L2 block to another L2 block then L3 block must split and added as 2 entries */
      end_address = map_ptr->p_base + ((uint32)map_ptr->size_in_kbytes << 10);     
      l2_block_start = map_ptr->p_base & L3_PT_ADDRESS_BITS; 
      l2_block_end = l2_block_start + 0x200000;
      
      if( end_address > l2_block_end )
      {
        
        temp_table = *map_ptr;
        /* base address to boundary */
        temp_table.size_in_kbytes = (l2_block_end - map_ptr->p_base)>>10;
        mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, &temp_table);
        temp_table.p_base += temp_table.size_in_kbytes<<10;
        temp_table.v_base += temp_table.size_in_kbytes<<10;
        /* boundary to end address */
        temp_table.size_in_kbytes = map_ptr->size_in_kbytes - temp_table.size_in_kbytes;
        mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, &temp_table);
      }
      else
      {
        mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, map_ptr);
      }
    }
  }

  return result;
} /* boot_mmu_page_table_map_mem_block_list */


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
)
{
  uint32 i;
  uint32 j;
  uint64 *src_l1_pagetable = (uint64*)mmu_get_page_table_base();
  uint64 *dst_l1_pagetable = mmu_l1_ddr_pagetable;
  uint64 *dst_l2_pagetable = mmu_l2_ddr_pagetable;
  uint64 *dst_l3_pagetable = mmu_l3_ddr_pagetable;
  boot_boolean result = TRUE;
  
  do
  { 
    /* Set the number of emtpy L2/L3 page table we can use*/
    empty_l2_ddr_pagetable_ptr = dst_l2_pagetable;
    empty_l2_ddr_pagetable_num = MMU_NUM_DDR_L2_PT;

    empty_l3_ddr_pagetable_ptr = dst_l3_pagetable;
    empty_l3_ddr_pagetable_num = MMU_NUM_DDR_L3_PT;
        
    /* Now copy the entire page table */
	/* Start with L1->L2 tables */
    for(i = 0; i< MMU_NUM_L1_PT; i++)
    {
      /* First copy the L1 page table entry */
      dst_l1_pagetable[i] = src_l1_pagetable[i];
      
	  /*If current L1 page table entry points to a L2 page table*/
      if((src_l1_pagetable[i] & PT_TABLE_DESC_BIT) != 0)
      {        
        /* Copy over the source L2 page table and 
           decrement the number of empty L2 page table*/
        qmemscpy(empty_l2_ddr_pagetable_ptr,
				MMU_L2_PT_SIZE * sizeof(uint64), 
                (void *)(uintnt)(src_l1_pagetable[i] & PT_ADDRESS_BITS), 
                MMU_L2_PT_SIZE * sizeof(uint64));
        
		/* Update the L1 descriptor in dst page table to point
		   to the new dst L2 page table, keep the attributes the same*/
		dst_l1_pagetable[i] = ((uint64)empty_l2_ddr_pagetable_ptr & PT_ADDRESS_BITS)|
		                      (src_l1_pagetable[i] & PT_ATTRIBUTE_BITS);
		
        empty_l2_ddr_pagetable_ptr += MMU_L2_PT_SIZE;
        empty_l2_ddr_pagetable_num--;
      }
    }
	  
	/* Save pointer and number remaining values to main record, as
	   logic is agnostic to actual location where pagetables reside */  
    empty_l2_pagetable_ptr = empty_l2_ddr_pagetable_ptr;
	empty_l2_pagetable_num = empty_l2_ddr_pagetable_num;
	  
    /* Now copy the L3 tables and map from L2->L3 */
    for (j = 0; j< (MMU_NUM_DDR_L2_PT - empty_l2_ddr_pagetable_num); j++)
    {
        for(i = 0; i< MMU_L2_PT_SIZE; i++)
        {
		  /*If current L2 page table entry points to a L3 page table*/
		  if((dst_l2_pagetable[(j*MMU_L2_PT_SIZE)+i] & PT_TABLE_DESC_BIT) != 0)
		  {        
			/* Copy over the source L3 page table and 
			   decrement the number of empty L3 page table*/
			qmemscpy(empty_l3_ddr_pagetable_ptr, 
			        MMU_L3_PT_SIZE * sizeof(uint64),
					(void *)(uintnt)(dst_l2_pagetable[(j*MMU_L2_PT_SIZE)+i] & PT_ADDRESS_BITS), 
					MMU_L3_PT_SIZE * sizeof(uint64));
			
			/* Update the L2 descriptors in dst page table and point
			   to the new dst L3 page table, keep the attributes the same*/
			dst_l2_pagetable[(j*MMU_L2_PT_SIZE)+i] = ((uint64)empty_l3_ddr_pagetable_ptr & PT_ADDRESS_BITS)|
								  (dst_l2_pagetable[(j*MMU_L2_PT_SIZE)+i] & PT_ATTRIBUTE_BITS);
			
			empty_l3_ddr_pagetable_ptr += MMU_L3_PT_SIZE;
			empty_l3_ddr_pagetable_num--;
		  }
		}
	}
	
    /* Save pointer and number remaining values to main record, as
       logic is agnostic to actual location where pagetables reside */  
    empty_l3_pagetable_ptr = empty_l3_ddr_pagetable_ptr;
	empty_l3_pagetable_num = empty_l3_ddr_pagetable_num;
  }while(0);
    
  return result;
} /* boot_mmu_copy_page_table */



/*===========================================================================

**  Function :  boot_mmu_page_table_map_single_mem_block

** ==========================================================================
*/
/*!
* 
* @brief
*   Populates a pagetable with the content of a single memory map block.
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
)
{
  uint32 empty_l2_pt_needed = 0;
  uint32 empty_l3_pt_needed = 0;
  boot_boolean result = TRUE;
  struct mem_block temp_table;
  uint32 end_address;
  uint32 l2_block_start;
  uint32 l2_block_end;
  uint64  *l2_page_table;
  boolean entry_invalid_enable = TRUE;
  xbl_internal_mem_region_type sbl_imem_ranges[]=SBL_IMEM_RANGES_TABLE;
  uint32 i = 0;
  /* Convert our base address into an L1 page table index */
  uint64 address_base_index;
  address_base_index = (map_ptr->v_base & L1_PT_INDEX) >> 30; 
  /* Check whether this address entry could be invalidte first */
  for( i = 0; i < sizeof(sbl_imem_ranges)/sizeof(xbl_internal_mem_region_type); i++)
  {
    if(entry_invalid_enable == TRUE)
    {
      if((map_ptr->v_base >= sbl_imem_ranges[i].start_addr && map_ptr->v_base < sbl_imem_ranges[i].start_addr + sbl_imem_ranges[i].size)
	  	&&(map_ptr->v_base + (((uintnt )map_ptr->size_in_kbytes)<<10)) < (map_ptr->v_base < (sbl_imem_ranges[i].start_addr + sbl_imem_ranges[i].size)))
      {
	  	entry_invalid_enable = FALSE;
	    break;
      }
    }
	else
	{
	  break;
	}
  }
  if (entry_invalid_enable == TRUE)
  {
    /* replace the old translation table entry with an invalidate entry and invalidate TLB*/
    boot_mmu_find_entry_and_unmap((uintnt*)mmu_l1_pagetable,(uintnt )map_ptr->v_base,((uintnt )map_ptr->size_in_kbytes)<<10);
  }

  do
  {
    if(map_ptr->memory_mapping == MMU_L1_SECTION_MAPPING ||
      map_ptr->memory_mapping == MMU_L1_NS_SECTION_MAPPING)
    {
      mmu_map_block_as_l1_sections(mmu_l1_pagetable, map_ptr);
    }
    else if(map_ptr->memory_mapping == MMU_L2_SECTION_MAPPING ||
            map_ptr->memory_mapping == MMU_L2_NS_SECTION_MAPPING)
    {
      if((mmu_l1_pagetable[address_base_index] & PT_TABLE_DESC_BIT) == 0)
      {
        empty_l2_pt_needed++;
      }
        /* If we have enough L2 page tables to use then do the mapping*/
      if(empty_l2_pt_needed <= empty_l2_pagetable_num)
      {
        mmu_map_block_as_l2_pages(mmu_l1_pagetable, map_ptr);
      }
      else
      {
        result = FALSE;
        break;
      }
    }
    else if(map_ptr->memory_mapping == MMU_L3_SECTION_MAPPING ||
            map_ptr->memory_mapping == MMU_L3_NS_SECTION_MAPPING)
    {
      if((mmu_l1_pagetable[address_base_index] & PT_TABLE_DESC_BIT) == 0)
      {
        empty_l2_pt_needed++;
        empty_l3_pt_needed++;
      }
      else
      {
        l2_page_table = (uint64*)(mmu_l1_pagetable[address_base_index] & PT_ADDRESS_BITS);
      
        /* Convert our base address into an L2 page table index */
        address_base_index = (map_ptr->v_base & L2_PT_INDEX) >> 21;
      
        if((l2_page_table[address_base_index] & PT_TABLE_DESC_BIT) == 0)
        {
          empty_l3_pt_needed++;
        }
      }
        /* If we have enough L2 page tables to use then do the mapping*/
      if((empty_l2_pt_needed <= empty_l2_pagetable_num) &&
	     (empty_l3_pt_needed <= empty_l3_pagetable_num))
      {
        /* Checking for block overflow, if an L3 block of pages overlaps from one 
           L2 block to another L2 block then L3 block must split and added as 2 entries */
        end_address = map_ptr->p_base + ((uint32)map_ptr->size_in_kbytes << 10);
        l2_block_start = map_ptr->p_base & L3_PT_ADDRESS_BITS; 
        l2_block_end = l2_block_start + 0x200000;     
        if( end_address > l2_block_end )
        {
          /* base address to boundary */
          temp_table = *map_ptr;
          temp_table.size_in_kbytes = (l2_block_end - map_ptr->p_base)>>10;
          mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, &temp_table);
          temp_table.p_base += temp_table.size_in_kbytes<<10;
          temp_table.v_base += temp_table.size_in_kbytes<<10;
          /* boundary to end address */
          temp_table.size_in_kbytes = map_ptr->size_in_kbytes - temp_table.size_in_kbytes;
          mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, &temp_table);
        }
        else
        {
          mmu_map_block_as_l3_pages(mmu_l1_pagetable, mmu_l2_pagetable, map_ptr);
        }
      }
      else
      {
        result = FALSE;
        break;
      }
    }    

  }while(0);
  
  return result;
}

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
)
{
  boot_boolean result = FALSE;
  uintnt  *l2_page_table, *l3_page_table;
  uintnt address_base_index;
  
  BL_VERIFY( (mmu_l1_pagetable != NULL) && (page_table_entry_ptr != NULL)
 , BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );


  /* Convert our base address into an L1 page table index */
  address_base_index = (memory_address & L1_PT_INDEX) >> 30; 
  
  *page_table_entry_ptr = (uintnt)NULL;
  do
  {
    if((mmu_l1_pagetable[address_base_index] & PT_TABLE_VALID_ENTRY_BIT) != 1)
    {
      /* Address not mapped */
      result = FALSE;
      break;
    }
    if((mmu_l1_pagetable[address_base_index] & PT_TABLE_DESC_BIT) == 0)
    {
      /* Address lies in L1 page table block entry */
      result = TRUE;
      *page_table_entry_ptr = (uintnt)&mmu_l1_pagetable[address_base_index];
      break;
    }
    else
    {  
      /* Address may lie in L2 or L3 page table */
      l2_page_table = (uintnt*)((uintnt)mmu_l1_pagetable[address_base_index] & PT_ADDRESS_BITS);
      /* Convert our base address into an L2 page table index */
      address_base_index = (memory_address & L2_PT_INDEX) >> 21;      
      
      if((l2_page_table[address_base_index] & PT_TABLE_VALID_ENTRY_BIT) != 1)
      {
        /* Address not mapped */
        result = FALSE;
        break;     
      }
      if((l2_page_table[address_base_index] & PT_TABLE_DESC_BIT) == 0)
      {
        /* Address lies in L2 page table block entry */
        result = TRUE;
        *page_table_entry_ptr = (uintnt)&l2_page_table[address_base_index];
        break;      
      }
      else
      {  
        /* Address may lie L3 page table */
        l3_page_table = (uintnt*)((uintnt)l2_page_table[address_base_index] & PT_ADDRESS_BITS);
        /* Start index into the L3 page table for this section using the virtual address[20:12]*/
        address_base_index  = (memory_address & L3_PT_INDEX) >> 12;
        if((l3_page_table[address_base_index] & PT_TABLE_VALID_ENTRY_BIT) != 1)
        {
          /* Address not mapped */
          result = FALSE;
          break;     
        }
        /* Address lies in L3 page table block entry */
        result = TRUE;
        *page_table_entry_ptr = (uintnt)&l3_page_table[address_base_index];
      }
    }
  }while(0);
  return result;
}

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
)
{
  boot_boolean result = FALSE;
  uintnt page_table_entry_first = 0;
  uintnt page_table_entry_last = 0;
  uintnt *page_table_entry_first_ptr = &page_table_entry_first;
  uintnt *page_table_entry_last_ptr = &page_table_entry_last;
  uintnt i = 0;
  BL_VERIFY( (mmu_l1_pagetable != NULL), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );
  
  if((boot_mmu_get_page_table_entry_addr(mmu_l1_pagetable, memory_address, page_table_entry_first_ptr))
    && (boot_mmu_get_page_table_entry_addr(mmu_l1_pagetable, (uintnt)(memory_address + size-1), page_table_entry_last_ptr))
    && ((uintnt)page_table_entry_last-(uintnt)page_table_entry_first)>0
    && (uintnt*)page_table_entry_first!=NULL && (uintnt*)page_table_entry_last!=NULL
    && (*((uintnt*)page_table_entry_first)&0x3) == (*((uintnt*)page_table_entry_last)&0x3))
  {
    for(i=0; i<=(((uintnt*)page_table_entry_last)-((uintnt*)page_table_entry_first)); i++){
		mmu_inval_trans_tbl_entry(((uintnt*)page_table_entry_first)+i);
    } 
	mmu_invalidate_tlb();
    result = TRUE;
  }
  return result;
}