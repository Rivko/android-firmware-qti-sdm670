#ifndef BOOT_PAGE_TABLE_ARMV7_H
#define BOOT_PAGE_TABLE_ARMV7_H

/*===========================================================================
                    Boot Page Table ARMV7 Definitions Header File

GENERAL DESCRIPTION
    Definitions specific to the boot Page Table for ARM V7

Copyright 2009, 2010-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/05/15   ck      Corrected copyright
04/14/14   ck      Added MMU_L1_PAGETABLE_L2_PT_ADDRESS_BASE_MASK
02/25/14   ck      Increased MMU_NUM_L2_PT to 14 for Bear family
06/12/13   dh      Add shared L2 page table configuration and add comments
03/01/13   jz      Added MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE
03/13/12   dh      Change MMU_NUM_L2_PT to 12, add MMU_L2_PAGETABLE_INNER_WT_OUTER_WB_CACHE
02/07/12   dh      Add NS bit define for L1 page table entry
10/21/10   dh      Add two global extern pointers to L1/L2 page table
04/20/11   dh      Rename to boot_page_table_armv7.h
08/25/09   as      Adding a Dummy file.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif

/*
   The mmu configuration is composed of 4096 word entries which each map
   one megabyte of memory.
   4GB/1MB -> 4096 word entries
   The page table contains either a section descriptor or a coarse page
   table descriptor.

   A section descriptor for L1 is composed as follows (Descriptor = 0b10):
     _______________________________________________________________________________
    |                          |     |     |  |  |       |        |      | |      |      |   |   |          |
    |31-------------20|19|18|17|16|-15-|14-12|11--10|9|8----5|-4-|-3-|-2-|1------- 0|
    |Phys Address base|nS| 0|nG| S| APX| TEX |Access|P|DOMAIN| XN| C | B |Descriptor|
    |_________________|_ |__|__|__|____|_____|______|_|______|___|___|___|__________|
  
   A coarse page table desc is composed as follows (Descriptor = 0b01):
     ________________________________________________________________________________
    |                                                                               |
    |31----------------------------------------------10|9|8------5|4|3 |2|1 ------ 0|
    |         coarse PT Address base                   |0| DOMAIN |0|nS|0|Descriptor|
    |_______________________________________________________________________________|
  
  A level 2 page descriptor is composed as follows :
   ______________________________________________________________________
  |                        |  |  |   |     |       |        |    |
  |31---------------------------12|11|10| 9 |8 7 6|5 --- 4|3-------2|1  0|
  |     Phys Address base         |nG|S |APX| TEX | Access|Cach/Buff|1 XN| 
  |_______________________________|__|__|___|_____|_______|_________|____|
  
*/
#define MMU_L1_PT_SIZE                4096  /* no of entries in pagetable */
#define MMU_L2_PT_SIZE                256   /* segment size for L2 page table */  
#define MMU_NUM_L2_PT                 14    /* max number of L2 pagetables allocated in boot loader */

/*
   Bits [1:0] define the entry type
*/
#define MMU_PAGETABLE_INVALID_DESCRIPTOR             0x0   /* Translation Fault */
#define MMU_PAGETABLE_SECONDARY_PAGE_DESCRIPTOR      0x1   /* Use 2nd Table for Fine sections */

#define MMU_PAGETABLE_SECTION_DESCRIPTOR             0x2   /* Simple 1 or 16MB or 64KB or 4KB*/
#define MMU_PAGETABLE_RESERVED_DESCRIPTOR            0x3   /* Reserved */


#define MMU_L1_PAGETABLE_COARSE_NS                   0x8
#define MMU_L1_PAGETABLE_SECTION_NS                  0x80000

#define MMU_L1_PAGETABLE_L2_PT_ADDRESS_BASE_MASK     0xFFFFF300

/*
   Bits [3:2] control the cache and write-buffer functions
   These work with the TEX bits defined later - see there
*/
#define MMU_PAGETABLE_B_SET                           (1 << 2)
#define MMU_PAGETABLE_B_CLEAR                         (0 << 2)
#define MMU_PAGETABLE_C_SET                           (1 << 3)
#define MMU_PAGETABLE_C_CLEAR                         (0 << 3)

/*
   Bits [4] is the eXecute Never bit for L1 and bit [0] is the eXecute
   Never bit for L2. The region is executable if set to 0 and not executable if set
   to 1.
*/
#define MMU_L1_PAGETABLE_EXECUTABLE_REGION                (0 << 4)
#define MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION            (1 << 4)
#define MMU_L2_PAGETABLE_EXECUTABLE_REGION                (0)
#define MMU_L2_PAGETABLE_NON_EXECUTABLE_REGION            (1)

/*
   Bits [8:5] specify one of the 16 domains
   everything is mapped to one domain for simplicity:
   The default domain is set to 0
*/
#define MMU_PAGETABLE_DEF_DOMAIN                      (0 << 5)

/*
   Bit [9] Specifies the P bit - whether the memory 
   region has ECC enabled. TRM p0r2 says "Should be Zero"
*/
#define MMU_PAGETABLE_ECC_ENABLED                     (1 << 9)
#define MMU_PAGETABLE_ECC_DISABLED                    (0 << 9)

/*
   Bits [11:10] Specifies the Access Permisions (AP bits) along
   with the APX bit [15] for L1. Bits [5:4] specifies the Access Permission (AP)
   along with the APX bit [9] for L2.
                                                         AP         APX
*/
#define MMU_L1_PAGETABLE_MEM_READ_WRITE                 ((3 << 10) | (0 << 15))
#define MMU_L1_PAGETABLE_MEM_READ_ONLY                  ((2 << 10) | (1 << 15))
#define MMU_L2_PAGETABLE_MEM_READ_ONLY                  ((2 << 4 ) | (1 << 9 ))
#define MMU_L2_PAGETABLE_MEM_READ_WRITE                 ((3 << 4 ) | (0 << 9 ))

/*
   Bit [16] specifies the S bit for L1 and Bit [10] for L2 - if a normal memory region is shared.
*/
#define MMU_L1_PAGETABLE_NORMAL_REGION_SHARED            (1 << 16)
#define MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED         (0 << 16)
#define MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED         (0 << 10)
#define MMU_L2_PAGETABLE_NORMAL_REGION_SHARED            (1 << 10)


/*
   Bits [14:12]/[8:6] Specifies the Type EXtension Field Encoding (TEX bits) and
   Bit  [16] /[10]    Specifies the Shared Attribute and
   Bits [3:2]         Specifies the Cache Attributes
*/

/*L1 PT: Inner Write-through, No Allocate on Write 
         Outer Non-cacheable, 
         Non-shared */
#define MMU_L1_PAGETABLE_INNER_WT_OUTER_NONCACHE \
          ( (0x4 << 12) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_SET \
            | MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED )

/*L1 PT: Inner Write-Back  No Allocate on Write
         Outer Write-Back  No Allocate on Write
         Non-shared */
#define MMU_L1_PAGETABLE_MEM_WRITE_BACK_CACHE \
          ( (0x0 << 12) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_SET \
            | MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED )
            
/*L1 PT: Inner Non-cacheable
         Outer Non-cacheable 
         Non-shared */            
#define MMU_L1_PAGETABLE_MEM_NON_CACHEABLE \
          ( (0x1 << 12) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED )
            
/*L1 PT: Inner write-back and Allocate on Write,
         Outter Non-cacheble,
         Non-shared */              
#define MMU_L1_PAGETABLE_INNER_WBWA_OUTER_NONCACHE \
            ( (0x4 << 12) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED )

/*L1 PT: Inner Non-cacheable
         Outer Non-cacheable  
         Shared */
#define MMU_L1_PAGETABLE_MEM_SHARED \
          ( (0x1 << 12) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L1_PAGETABLE_NORMAL_REGION_SHARED )
            
/*L1 PT: Device 
         Shared */            
#define MMU_L1_PAGETABLE_DEVICE_SHARED \
          ( (0x0 << 12) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L1_PAGETABLE_NORMAL_REGION_NONSHARED )
 
/*L2 PT: Inner Write-Back  No Allocate on Write
         Outer Write-Back  No Allocate on Write
         Non-shared */
#define MMU_L2_PAGETABLE_MEM_WRITE_BACK_CACHE \
          ( (0x0 << 6) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_SET \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )
            
/*L2 PT: Inner Write-through, No Allocate on Write 
         Outer Non-cacheable, 
         Non-shared */            
#define MMU_L2_PAGETABLE_INNER_WT_OUTER_NONCACHE \
          ( (0x4 << 6) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_SET \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )

/*L2 PT: Inner Write-through, No Allocate on Write 
         Outer Write-Back, Write Allocate, 
         Non-shared */             
#define MMU_L2_PAGETABLE_INNER_WT_OUTER_WB_CACHE \
            ( (0x5 << 6) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_SET \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )
            
/*L2 PT: Inner Write-through, No Allocate on Write 
         Outer Write-Back, Write Allocate, 
         Shared */
#define MMU_L2_PAGETABLE_INNER_WT_OUTER_WB_CACHE_SHARED \
            ( (0x5 << 6) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_SET \
            | MMU_L2_PAGETABLE_NORMAL_REGION_SHARED )
            
/*L2 PT: Inner Write-Back, Write Allocate
         Outer Non-cacheable, 
         Non-shared */            
#define MMU_L2_PAGETABLE_INNER_WBWA_OUTER_NONCACHE \
            ( (0x4 << 6) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )
            
/*L2 PT: Inner Non-cacheable,
         Outer Write-Back, Write Allocate, 
         Non-shared */            
#define MMU_L2_PAGETABLE_INNER_NONCACHE_OUTER_WB_CACHE \
                ( (0x5 << 6) \
                | MMU_PAGETABLE_B_CLEAR \
                | MMU_PAGETABLE_C_CLEAR \
                | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )
                
/*L2 PT: Inner Non-cacheable,
         Outer Non-cacheable, 
         Non-shared */ 
#define MMU_L2_PAGETABLE_MEM_NON_CACHEABLE \
          ( (0x1 << 6) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )

/*L2 PT: Inner Non-cacheable,
         Outer Non-cacheable, 
         Shared */ 
#define MMU_L2_PAGETABLE_MEM_NON_CACHEABLE_SHARED \
          ( (0x1 << 6) \
            | MMU_PAGETABLE_B_CLEAR \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L2_PAGETABLE_NORMAL_REGION_SHARED )
            
/*L2 PT: Device
         Shared */ 
#define MMU_L2_PAGETABLE_DEVICE_SHARED \
          ( (0x0 << 6) \
            | MMU_PAGETABLE_B_SET \
            | MMU_PAGETABLE_C_CLEAR \
            | MMU_L2_PAGETABLE_NORMAL_REGION_NONSHARED )

           
#define MMU_LAST_ADDRESS 0xffffffff

#ifndef _ARM_ASM_

typedef enum
{
  /* Secure L1 section */
  MMU_L1_SECTION_MAPPING = 0,
  
  /* Non-secure L1 section */
  MMU_L1_NS_SECTION_MAPPING,
  
  /* Secure L2 section */
  MMU_L2_SECTION_MAPPING,
  
  /* Non-secure L2 section */
  MMU_L2_NS_SECTION_MAPPING,
  
  MMU_MEMORY_MAPPING_TYPE_MAX_SIZE = 0x7FFFFFFF

}MMU_MemoryMappingType;

struct mem_block
{
  uint32                     p_base;
  uint32                     v_base;
  uint32                     size_in_kbytes;
  MMU_MemoryMappingType      memory_mapping;
  uint32                     access;
  uint32                     cachetype;
  uint32                     executable;
  uint32                     domain;
} ;

#endif /* _ARM_ASM_ */

#endif /* BOOT_PAGE_TABLE_ARMV7_H */
