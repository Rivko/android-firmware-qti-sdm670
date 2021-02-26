#ifndef BOOT_MMU_TABLE_H
#define BOOT_MMU_TABLE_H

/*=============================================================================

                      MSM8996 SBL MMU Table

GENERAL DESCRIPTION
    This module contains the memory map for MSM8996 SBL.

Copyright 2013, 2014, 2015 by Qualcomm Technologies, Inc. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/22/18   aus     Update ddr memory block to cacheable
09/21/17   aus     Added SSC region in MMU table
06/06/17   ds      update_regions_mapped_by_pbl, cache settings update
04/04/17   kpa     Mark SCL_SBL1_OCIMEM_DATA_BASE as non cacheable
07/20/16   kpa     Initial revision for SDM845

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_cache_mmu.h"
#include "boot_page_table_armv8.h"

/*=============================================================================
Memory Map Structure
  PBL only maps a limited number of memories: 
  OCIMEM, BOOT IMEM, AOP memories, APPS PBL ROM regions.
  
  With Aarch64 Page Tables, we will need to take care of all required regions.
  
  SBL (XBLLoader) is responsible to map the memory it will use such as DDR 
  and ram dump regions.  

  Memory regions greater than or equal to 1GB are placed in the Level 1 pagetables. 
  Memory regions greater than or equal to 2MB are placed in the Level 2 pagetables.
  The rest regions less than 2MB (4KB)are placed in Level 3 pagetables. 
  All regions are marked with the default domain.
=============================================================================*/

/*=============================================================================
    update_regions_by_pbl
    PBL page table is directly used in XBL which already has few regions mapped.
    Below array has attribute updates for those already mapped regions.    
=============================================================================*/

static struct mem_block update_regions_mapped_by_pbl[] =
{
  { SCL_IMEM_BASE,
    SCL_IMEM_BASE,
    0x19000 >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
   
  { SCL_TZ_SDI_MAPPED_REGION,
    SCL_TZ_SDI_MAPPED_REGION,
    0x14000 >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
    
  { 
    SCL_BOOT_IMEM_BASE_ADDR,
    SCL_BOOT_IMEM_BASE_ADDR,
    SCL_BOOT_IMEM_BASE_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
  
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }
};

/*=============================================================================
    SBL initial Memory Map  
    This memory map is populated before DDR is intialized.
    Regions that have XPU protection turned on should be marked as 
    DEVICE memory(strongly ordered) to eliminate the processor doing 
    speculative data access and NON-EXECUTABLE to eliminate 
    speculative instruction execution.
=============================================================================*/

/* For DDR we need to populate remaining L1 entries.
   Data Execution Prevention(DEP) for SBL RW region:
   All SBL region except code should be marked as non executable
   
   Data Execution Prevention(DEP) for SBL CODE region:
   All SBL code region should be marked as read only and exec 
   DEP already taken care of via sbl_initial_memory_map[]
*/


static struct mem_block sbl_initial_memory_map[] =
{
 /* SBL1 Code and Vectors.  Reusable L3. */
  { SCL_SBL1_CODE_BASE,
    SCL_SBL1_CODE_BASE,
    SCL_SBL1_CODE_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_ONLY,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_EXECUTABLE_REGION, },

  /* SBL1 Stack and Data (RW + ZI), Training data buffer. Reusable L3 */
  { SCL_SBL1_STACK_BASE,
    SCL_SBL1_STACK_BASE,
    (SCL_SBL1_STACK_SIZE + SCL_SBL1_DATA_SIZE + SCL_SBL1_DATA_ZI_SIZE) >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /* SBL1 Page Table and Shared Functions Table, noncached */
  { SCL_SBL1_PAGE_TABLE_BASE,
    SCL_SBL1_PAGE_TABLE_BASE, 
    (SCL_SBL1_PAGE_TABLE_SIZE + SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE) >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },    

  /* System debug image(SDI) segment lies in OCIMEM (16KB)
     code section must be marked as executable.
  */
  { 
    SCL_SYSTEM_DEBUG_CO_RO_BASE,
    SCL_SYSTEM_DEBUG_CO_RO_BASE, 
    SCL_SYSTEM_DEBUG_CO_RO_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_ONLY,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_EXECUTABLE_REGION, },  

  { 
    SCL_SYSTEM_DEBUG_DATA_BASE,
    SCL_SYSTEM_DEBUG_DATA_BASE, 
    SCL_SYSTEM_DEBUG_DATA_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  { 
    SCL_SBL1_OCIMEM_DATA_BASE,
    SCL_SBL1_OCIMEM_DATA_BASE, 
    SCL_SBL1_OCIMEM_DATA_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  { 
    SHARED_IMEM_BASE,
    SHARED_IMEM_BASE, 
    SHARED_IMEM_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE,
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
    
  /* DDR region, must be marked as shared device initially. New L1 mapping */
  { DDR_MEM_BASE,
    DDR_MEM_BASE, 
    DDR_MEM_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_DEVICE_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
   
  /*AOP Data RAM, Cacheable, Non-Executable */  
  { SCL_AOP_DATA_RAM_BASE,
    SCL_AOP_DATA_RAM_BASE, 
    SCL_AOP_DATA_RAM_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /*DDR Driver code segment , Cacheable, Executable */  
  { SCL_SBL1_DDR_DSF_ROM_BASE,
    SCL_SBL1_DDR_DSF_ROM_BASE,
    SCL_SBL1_DDR_DSF_ROM_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_ONLY,
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_EXECUTABLE_REGION, },

  /*DDR Driver data segment , Cacheable, Non-Executable */      
  { SCL_SBL1_DDR_DSF_DATA_BASE,
    SCL_SBL1_DDR_DSF_DATA_BASE, 
    (SCL_SBL1_DDR_DSF_DATA_SIZE + SCL_SBL1_DDR_DSF_CFG_BUF_SIZE + SCL_SBL1_DDR_STRUCT_SIZE) >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },    
    
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }

};


/*=============================================================================
    SBL1 Addtional Memory Map  
    
    Addtional memory regions that need to be mapped in SBL1. 
    This memory map is populated after DDR is intialized.
    Regions that have XPU protection turned on should be marked as 
    DEVICE memory(strongly ordered) to eliminate the processor doing 
    speculative data access and NON-EXECUTABLE to eliminate 
    speculative instruction execution.
=============================================================================*/
const struct mem_block sbl1_addtional_memory_map[] =
{ 
  /*SBL1 DDR ZI, L2 cacheable */  
  { SCL_SBL1_DDR_DATA_BASE,
    SCL_SBL1_DDR_DATA_BASE, 
    SCL_SBL1_DDR_DATA_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /*SBL1 DDR Page table, non-cacheable 
    Page table region must be mapped as non-cachable to be consistent with 
    the setting in TTBR0(we set page table walk as non-cachable)*/  
  { SCL_SBL1_DDR_PAGE_TABLE_BASE,
    SCL_SBL1_DDR_PAGE_TABLE_BASE, 
    SCL_SBL1_DDR_PAGE_TABLE_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION,  },   
    
  /*SBL1 DDR Uncached ZI, 4KB , non-cacheable*/  
  { SCL_SBL1_DDR_UNCACHED_ZI_BASE,
    SCL_SBL1_DDR_UNCACHED_ZI_BASE, 
    SCL_SBL1_DDR_UNCACHED_ZI_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /* SMEM Region, 2MB , Shared memory */ 
  { SCL_SHARED_RAM_BASE,
    SCL_SHARED_RAM_BASE, 
    SCL_SHARED_RAM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /*QSEE pImem Region,L2 cacheable, Non-Executable,
    marked as Executable before jumping to it. */  
  { SCL_DDR_pIMEM_BASE,
    SCL_DDR_pIMEM_BASE, 
    SCL_DDR_pIMEM_SIZE >> 10,
    MMU_L2_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_DEVICE_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },    
    
  /*QHEE, cacheable, Non-Executable */  
  { SCL_QHEE_CODE_BASE,
    SCL_QHEE_CODE_BASE, 
    SCL_QHEE_TOTAL_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /*RPM Code RAM, Cacheable, Non-Executable */  
  { SCL_AOP_CODE_RAM_BASE,
    SCL_AOP_CODE_RAM_BASE, 
    SCL_AOP_CODE_RAM_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /*APPSBL, cacheable, NON-Executable */  
  { SCL_APPSBL_CODE_BASE,
    SCL_APPSBL_CODE_BASE, 
    SCL_APPSBL_TOTAL_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /* XBLRamDump region cacheable, non-exec */  
  { SCL_RAMDUMP_CODE_BASE,
    SCL_RAMDUMP_CODE_BASE, 
    SCL_RAMDUMP_TOTAL_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_MEM_WRITE_BACK_CACHE,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /* SSC region, read-write, non-exec */  
  { SSC_SCC_SCC_REG_BASE,
    SSC_SCC_SCC_REG_BASE, 
    SSC_SCC_SCC_REG_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_WRITE, 
    MMU_PAGETABLE_DEVICE_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },
	
  /* Security control region as read-only */
  { SECURITY_CONTROL_BASE,
    SECURITY_CONTROL_BASE, 
    QFPROM_READONLY_BLOCK_SIZE >> 10,
    MMU_L3_SECTION_MAPPING,
    MMU_PAGETABLE_MEM_READ_ONLY, 
    MMU_PAGETABLE_DEVICE_SHARED,
    MMU_PAGETABLE_NON_EXECUTABLE_REGION, },

  /* Add addtional memory region here */
  
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }
};

/* MMU Pagetable Region  
   Page table region must be mapped as non-cachable to be consistent with 
   the setting in TTBR0(we set page table walk as non-cachable)
*/
struct mem_block sbl1_mmu_page_table_mem_block = 
{ 
  /* Pagetable 64KB + 32b */
  0, 0, 0,
  MMU_L3_NS_SECTION_MAPPING, MMU_PAGETABLE_MEM_READ_WRITE, 
  MMU_PAGETABLE_MEM_NON_CACHEABLE, MMU_PAGETABLE_NON_EXECUTABLE_REGION  
};

/* Memory block to describe DDR memory */
struct mem_block sbl1_ddr_mem_block = 
{
  0, 0, 0,
  MMU_L1_SECTION_MAPPING, MMU_PAGETABLE_MEM_READ_WRITE, 
  MMU_PAGETABLE_MEM_WRITE_BACK_CACHE, MMU_PAGETABLE_NON_EXECUTABLE_REGION
};

#endif  /* BOOT_MMU_TABLE_H */
