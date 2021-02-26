#ifndef BOOT_PAGE_TABLE_ARMV8_H
#define BOOT_PAGE_TABLE_ARMV8_H

/*===========================================================================
                    Boot Page Table ARMV8 Definitions Header File

GENERAL DESCRIPTION
    Definitions specific to the boot Page Table for ARM V8

Copyright 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/07/15   plc     Added MAIR_INNER_NC_OUTER_WBWARA support
01/14/15   kedara  Fix device config macro name.
01/14/15   ck      MMU page table values are now target specific
12/09/14   kedara  Further updates to MAIR attributes  
09/09/14   kedara  Update MAIR indices to be same as PBL
04/10/14   plc     Decrease number of L3 PT's so all PT's fit in 64KB space
04/03/14   plc     Update MAIR comments, fix defines to include bit shift
03/25/14   plc     Cleanup comments while enabling MMU
11/19/13   plc     Initial version for ArmV8a 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif

/*
   The MMU Configuration is capable of representing pages of minimum 4kb size.

   Block descriptors with 4kb granule configuration describe:
     Level 1: 1GB Block memory
	 Level 2: 2MB Block memory
	 Level 3: 4kb Page memory
	   *note Level 3 descriptors have Descriptor bits [1:0] set to 0b11, similar
	    to Table descriptors.  Whereas Block descriptors have bits [1:0] set to 
		0b01.
   
   These descriptors may be one of:
    • An invalid or fault entry.
    • A table entry, that points to the next-level translation table.
    • A block entry, that defines the memory properties for the access.
    • A reserved format.
   Bit[1] of the descriptor indicates the descriptor type, and bit[0] indicates 
   whether the descriptor is valid.
   
   4KB granule: A zero-level descriptor does not support block translation.
   Where a block entry:
    • In a first-level table describes the mapping of the associated 1GB input address range.
    • In a second-level table describes the mapping of the associated 2MB input address range.
   
   The page table contains either table descriptors or block descriptors.

_____________________________________________________________________________________________   
I.) BLOCK/PAGE DESCRIPTORS

   A Block descriptor for first stage, level one is as follows (Descriptor = 0b01):
     ___________________________________________________________________________________________________________________
    |       |        |  |	|    |        |                  |        |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------30|n-1 --12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:30]|UNK/SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
    
    *Output addr[47:30] specifies bits[47:30] of output addr (1GB block of memory) 
    Descriptor bit [0]: identifies whether the descriptor is valid, and is 1 for a valid descriptor
    Descriptor bit [1]: 0 = Block, descriptor gives base address and attributes of a block of memory
                        1 = Page or Table, descriptor gives address and some attributes of next level/page
    XN or UXN, bit[54]: 
      UXN, Unprivileged Execute never defined only for level 1 of the EL1&0 translation regime
      XN, Unprivileged Execute never defined only for level 1 of the EL2&3 translation regimes
    PXN, bit[53]:
        PXN, Privileged execute never defined only for level 1 of the EL1&0 translation regime:
           •For the EL2 and EL3 translation regimes, the descriptors define a PXN bit that is reserved,
               SBZ, and is ignored by hardware.
           •For level 2 of the Non-secure EL1&0 translation regime, the corresponding bit position is
               reserved, SBZ, and is ignored by hardware.   
    Contiguous, bit[52]:
        A hint bit indicating that the translation table entry is one of a contiguous set or entries, 
        that might be cached in a single TLB entry.  When the value of the Contiguous bit is 1, 
        it indicates that the entry is one of a number of adjacent translation table
        entries that point to a contiguous output address range, of 16 adjacent entries.   
    nG, bit [11]:
        The not global bit.  Determines whether the TLB entry applies to all ASID values, or current.
        nG == 0: Translation is global meaning region is available for all processes
        nG == 1: Translation is non-global or process-specific, relating to only current ASID as defined
          in either the TTBR0_EL1 or TTBR1_EL1
    AF, bit [10]: 
        Access flag indicates when a page or section of memory is accessed for the first time
        If an attempt to read into the TLB a translation table descriptor for which value is 0, a 
        fault will be thrown, expecting SW to handle fault to set value to 1 for that entry
    SH, bits[9:8]: Shareability field: 
      SH[1:0] Normal memory
               00 Non-shareable
               01 UNPREDICTABLE
               10 Outer Shareable
               11 Inner Shareable
    AP[2:1], bits[7:6]: Data access permissions bits
         The ARMv8 translation table descriptor format defines AP[2:1] as the Access Permissions bits, and
             does not define an AP[0] bit.
    AP[2]:
         0 Read/write
         1 Read-only
    NS, bit[5]: Non-secure bit.  For memory accesses from Secure state, specifies whether the output
       address is in Secure or Non-secure address map.  Ignored by accesses from non-secure state or from Secure 
       state if translation table entry was held in Non-secure memory
       NS:
         0 Access the Secure physical address space
         1 Access the Non-secure physical address space
    AttrIndx[2:0], bits[4:2]
       Stage 1 memory attributes index field, indicates the 8-bit field in MAIR_ELx that specifies the 
       attributes for the corresponding memory region.  The required field is Attr"n", where 
       "n" = AttrIndx[2:0].  
         
    Populated Mair Register Indeces with MMU-used values/presets
       #define MMU_PAGETABLE_MAIR_0 (MAIR_DEVICE_nGnRE << (8*0))
       #define MMU_PAGETABLE_MAIR_1 (MAIR_INNER_NC_OUTER_NC << (8*1))
       #define MMU_PAGETABLE_MAIR_2 (MAIR_INNER_WBWARA_OUTER_NC << (8*2))
       #define MMU_PAGETABLE_MAIR_3 (MAIR_INNER_WTRA_OUTER_NC << (8*3))
       #define MMU_PAGETABLE_MAIR_4 (MAIR_INNER_WT_OUTER_WT << (8*4))
       #define MMU_PAGETABLE_MAIR_5 (MAIR_INNER_NC_OUTER_WTRA << (8*5))
       #define MMU_PAGETABLE_MAIR_6 (MAIR_INNER_WBWARA_OUTER_WBWARA << (8*6))
       #define MMU_PAGETABLE_MAIR_7 (MAIR_INNER_WTRA_OUTER_WTRA << (8*7))
			 
       *MAIR_EL3 bit assignments are:
       63 -- 56, 55 -- 48, 47 -- 40, 39 -- 32, 31 -- 24, 23 -- 16, 15 -- 8, 7 -- 0
        Attr7     Attr6     Attr5     Attr4     Attr3     Attr2     Attr1    Attr0
                 
        Attr<n>[7:4]     Meaning
        0000             Device Memory.
        00RW, RW not 00  Normal Memory, Outer Write-through transient
        0100,            Normal Memory, Outer non-Cacheable
        01RW, RW not 00  Normal Memory, Outer Write-back transient
        10RW,            Normal Memory, Outer Write-through non-transient
        11RW,            Normal Memory, Outer Write-back non-transient
          R = Outer Read Allocate Policy (0 = Do not Allocate, 1 = Allocate)              
          W = Outer Write Allocate Policy (0 = Do not Allocate, 1 = Allocate)
          
        Attr<n>[3:0]     Meaning when Attr<n>[7:4] is not 0000
        0000             UNPREDICTABLE
        00RW, RW not 00  Normal Memory, Inner Write-through transient
        0100             Normal Memory, Inner Non-Cacheable
        01RW, RW not 00  Normal Memory, Inner Write-back transient        
        1000             Normal Memory, Inner Write-through non-transient (RW=00)
        10RW, RW not 00  Normal Memory, Inner Write-through non-transient
        1100             Normal Memory, Inner Write-back non-transient (RW=00)
        11RW, RW not 00  Normal Memory, Inner Write-back non-transient
          R = Inner Read Allocate Policy (0 = Do not Allocate, 1 = Allocate)              
          W = Inner Write Allocate Policy (0 = Do not Allocate, 1 = Allocate)

   A Block descriptor for second level is as follows (Descriptor = 0b01):

     ___________________________________________________________________________________________________________________
    |       |        |  |	|    |        |                  |        |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------21|20----12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:21]|UNK/SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
	                              *Output addr[47:21] specifies bits[47:21] 
								   of output addr (2MB block of memory)						
							   
   A Page descriptor for third level is as follows (Descriptor = 0b11):
     __________________________________________________________________________________________________________
    |       |        |  |   |    |        |                  |  |  |       |       |  |             |          |
    |63---59|58----55|54|53 |52  |51----48|47--------------12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
    |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:12]|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
    |_______|________|__|___|____|________|__________________|__|__|_______|_______|__|_____________|__________|
	                              *Output addr[47:12] specifies bits[47:12] 
								   of output addr (4kb page of memory)				
_______________________________________________________________________________________
II. TABLE DESCRIPTORS

   A Table descriptor for zero, first, second levels is as follows (Descriptor = 0b11):
     ________________________________________________________________________________
    |  |     |  |   |       |        |                            |       |          |
    |63|62-61|60| 59|58---52|51----48|47------------------------12|11----2|1------- 0|
    |NS| AP  |XN|PXN|Ignored|UNK/SBZP|Next-level table addr[47:12]|Ignored|Descriptor|
    |__|_____|__|___|_______|________|____________________________|_______|__________|
	                              
								   
		   A zero-level Table descriptor returns the address of the first-level table.
           A first-level Table descriptor returns the address of the second-level table.
           A second-level Table descriptor returns the address of the third-level table.
     
	NS=NS Table:  For memory accesses from Secure state, specifies the security 
	      level for subsequent levels of lookup.
          For memory accesses from Non-secure state, this bit is ignored.
		    NSTable == 0 The defined table address is in the Secure physical 
			  address space. In the descriptors in that translation table, NS 
			  bits and NSTable bits have their defined meanings.
            NSTable == 1 The defined table address is in the Non-secure physical 
			  address space. Because this table is fetched from the Non-secure address 
			  space, the NS and NSTable bits in the descriptors in this table must be
              ignored. This means that, for this table:
               • The value of the NS bit in any block or page descriptor is ignored. 
			     The block or page address refers to Non-secure memory.
               • The value of the NSTable bit in any table descriptor is ignored, 
			     and the table address refers to Non-secure memory. When this table 
				 is accessed, the NS bit in any block or page descriptor is ignored, 
			  	 and all descriptors in the table refer to Non-secure memory.
    AP=AP Table:  Access permissions limit for subsequent levels of lookup.
            APTable[0] is reserved, SBZ:
              • In the EL2 translation regime.
              • In the EL3 translation regime.
			APTable[1:0] Effect
              00 No effect on permissions in subsequent levels of lookup.
              01 a Access at EL0 not permitted, regardless of permissions in subsequent levels of lookup.
              10 Write access not permitted, at any Exception level, regardless of permissions in subsequent 
			     levels of lookup.
    XN=UXN or XN Table: Execute-Never (XN) controls determine whether an instruction fetched from
            the memory region can be executed. This sets limit for subsequent levels of lookup.
			UXN and PXN are Unprivelaged/Privelaged Execute-Never table bits.
	        This bit is called UXNTable in the EL1&0 translation regime, where it only determines whether
            execution at EL0 of instructions fetched from the region identified at a lower level of lookup
            permitted. In the other translation regimes the bit is called XNTable.
			The restrictions apply only to subsequent levels of lookup at the same stage of translation, and:
             • UXNTable or XNTable restricts the XN control:
               — When the value of the XNTable bit is 1, the XN bit is treated as 1 in all subsequent 
			     levels of lookup, regardless of its actual value.
               — When the value of the UXNtable bit is 1, the UXN bit is treated as 1 in all subsequent 
				 levels of lookup, regardless of its actual value.
               — When the value of a UXNtable or XNTable bit is 0 the bit has no effect.
    PXN=PXN Table: PXN limit for subsequent levels of lookup.
	         • For the EL1&0 translation regime, PXNTable restricts the PXN control:
	            — When PXNTable is set to 1, the PXN bit is treated as 1 in all subsequent levels of lookup, 
				  regardless of the actual value of the bit.
                - When PXNTable is set to 0 it has no effect.
			 
			This bit is reserved, SBZ:
             • In the EL2 translation regime.
             • In the EL3 translation regime.     
  
*/


/*
   Bits [1:0] define the entry type
*/
#define MMU_PAGETABLE_INVALID_DESCRIPTOR             0x0   /* Translation Fault */

#define MMU_PAGETABLE_BLOCK_DESCRIPTOR               0x1   /* Level 1 (1GB) or Level 2 (2MB) block */

#define MMU_PAGETABLE_RESERVED_DESCRIPTOR            0x2   /* Reserved */

#define MMU_PAGETABLE_TABLE_DESCRIPTOR               0x3   /* Table descriptor returning address of next 
                                                              level table descriptor, lvl 0->1, 1->2, etc. */
															  
#define MMU_PAGETABLE_PAGE_DESCRIPTOR                0x3   /* Level 3 (4kb) page */

/*
   Bit [10] triggers an exception if region is being accessed
   for the first time.  To disable this exception, set AF to 1 initially
*/
#define MMU_PAGETABLE_ACCESS_FLAG_SET                 (1 << 10)
#define MMU_PAGETABLE_ACCESS_FLAG_CLEAR               (0 << 10)
															  
#define MMU_PAGETABLE_BLOCK_NS                     0x20
#define MMU_PAGETABLE_TABLE_NS                     0x8000000000000000


/* 
   TODO: add support for Transient locations if helpful, evaluate speed increase
*/

/*
   Bits [3:2] control the cache and write-buffer functions
   These work with the TEX bits defined later - see there
*/
/*
#define MMU_PAGETABLE_B_SET                           (1 << 2)
#define MMU_PAGETABLE_B_CLEAR                         (0 << 2)
#define MMU_PAGETABLE_C_SET                           (1 << 3)
#define MMU_PAGETABLE_C_CLEAR                         (0 << 3)
*/

/*
   Attr bits for Stage 1 point to MAIR_ELx registers which configure cache 
   and write-buffer functions.  Stage 1 will point to different MAIR_EL3 
   field entries.

   *MAIR_EL3 bit assignments are:
       63 -- 56, 55 -- 48, 47 -- 40, 39 -- 32, 31 -- 24, 23 -- 16, 15 -- 8, 7 -- 0
        Attr7     Attr6     Attr5     Attr4     Attr3     Attr2     Attr1    Attr0
                 
              Attr<n>[7:4] Meaning
        0000             Device Memory.
        00RW, RW not 00  Normal Memory, Outer Write-through transient
        0100,            Normal Memory, Outer non-Cacheable
        01RW, RW not 00  Normal Memory, Outer Write-back transient
        10RW,            Normal Memory, Outer Write-through non-transient
        11RW,            Normal Memory, Outer Write-back non-transient
                    R = Outer Read Allocate Policy (0 = Do not Allocate, 1 = Allocate)              
          W = Outer Write Allocate Policy (0 = Do not Allocate, 1 = Allocate)
          
        Attr<n>[3:0]     Meaning when Attr<n>[7:4] is not 0000
        0000             UNPREDICTABLE
        00RW, RW not 00  Normal Memory, Inner Write-through transient
        0100             Normal Memory, Inner Non-Cacheable
              01RW, RW not 00  Normal Memory, Inner Write-back transient        
        1000             Normal Memory, Inner Write-through non-transient (RW=00)
        10RW, RW not 00  Normal Memory, Inner Write-through non-transient
        1100             Normal Memory, Inner Write-back non-transient (RW=00)
        11RW, RW not 00  Normal Memory, Inner Write-back non-transient
          R = Inner Read Allocate Policy (0 = Do not Allocate, 1 = Allocate)              
          W = Inner Write Allocate Policy (0 = Do not Allocate, 1 = Allocate)   
*/


/* Stage 1 attributes */
/* 
   Provide used bit-definitions of MAIR register bit fields 
   Note all configurations are non-transient currently 
*/
#define MAIR_DEVICE_nGnRE               0x04
#define MAIR_INNER_NC_OUTER_NC          0x44
#define MAIR_INNER_WBWARA_OUTER_NC      0x4F
#define MAIR_INNER_WTRA_OUTER_NC        0x4A
#define MAIR_INNER_WT_OUTER_WT          0x88
#define MAIR_INNER_NC_OUTER_WTRA        0xA4
#define MAIR_INNER_WBWARA_OUTER_WBWARA  0xFF
#define MAIR_INNER_WTRA_OUTER_WTRA      0xAA
#define MAIR_INNER_NC_OUTER_WBWARA      0xF4


/* Populate Mair Register Indeces with MMU-used values/presets */
#define MMU_PAGETABLE_MAIR_0 (MAIR_DEVICE_nGnRE << (8*0))
#define MMU_PAGETABLE_MAIR_1 (MAIR_INNER_NC_OUTER_NC << (8*1))
#define MMU_PAGETABLE_MAIR_2 (MAIR_INNER_WBWARA_OUTER_NC << (8*2))
#define MMU_PAGETABLE_MAIR_3 (MAIR_INNER_WTRA_OUTER_NC << (8*3))
#define MMU_PAGETABLE_MAIR_4 (MAIR_INNER_WT_OUTER_WT << (8*4))
#define MMU_PAGETABLE_MAIR_5 (MAIR_INNER_NC_OUTER_WBWARA << (8*5))
#define MMU_PAGETABLE_MAIR_6 (MAIR_INNER_WBWARA_OUTER_WBWARA << (8*6))
#define MMU_PAGETABLE_MAIR_7 (MAIR_INNER_WTRA_OUTER_WTRA << (8*7))


/* Define friendly name for index into MAIR register that is used 
   for attribute index (bits 4:2) of each page table entry        */

#define MMU_MAIR_DEVICE_nGnRE               (0 << 2)
#define MMU_MAIR_INNER_NC_OUTER_NC          (1 << 2)
#define MMU_MAIR_INNER_WBWARA_OUTER_NC      (2 << 2)
#define MMU_MAIR_INNER_WTRA_OUTER_NC        (3 << 2)
#define MMU_MAIR_INNER_WT_OUTER_WT          (4 << 2)
#define MMU_MAIR_INNER_NC_OUTER_WBWARA      (5 << 2)
#define MMU_MAIR_INNER_WBWARA_OUTER_WBWARA  (6 << 2)
#define MMU_MAIR_INNER_WTRA_OUTER_WTRA      (7 << 2)


/*
   Bit [54] is the eXecute Never bit for L1-L3. The region is executable if set 
   to 0 and not executable if set to 1.
   Bit [53] is the Privlaged eXecute Never bit which only is considered by HW for 
   EL0/EL1 Non Secure Stage 1.  
*/
#define MMU_PAGETABLE_UNPRIV_EXECUTABLE_REGION         (0ul << 54)
#define MMU_PAGETABLE_UNPRIV_NON_EXECUTABLE_REGION     (1ul << 54)
#define MMU_PAGETABLE_PRIV_EXECUTABLE_REGION           (0ul << 53)
#define MMU_PAGETABLE_PRIV_NON_EXECUTABLE_REGION       (1ul << 53)

#define MMU_PAGETABLE_EXECUTABLE_REGION                MMU_PAGETABLE_UNPRIV_EXECUTABLE_REGION|MMU_PAGETABLE_PRIV_EXECUTABLE_REGION
#define MMU_PAGETABLE_NON_EXECUTABLE_REGION            MMU_PAGETABLE_UNPRIV_NON_EXECUTABLE_REGION|MMU_PAGETABLE_PRIV_NON_EXECUTABLE_REGION
/*
   Bits [7:6] Specifies the Access Permisions (AP bits) for Stage 1.
              * Bit [6] has no effect for EL1, just removes access for EL0.
*/
#define MMU_PAGETABLE_MEM_READ_WRITE                 (0 << 7)
#define MMU_PAGETABLE_MEM_READ_ONLY                  (1 << 7)

/*
   Bits [9:8] specifies the SH bit for L1-L3 - if a normal memory region is shared.
               00 Non-shareable
               01 UNPREDICTABLE
               10 Outer Shareable
               11 Inner Shareable
*/
#define MMU_PAGETABLE_NORMAL_REGION_OUTER_SHARED      (2 << 9)
#define MMU_PAGETABLE_NORMAL_REGION_INNER_SHARED      (3 << 9)

#define MMU_PAGETABLE_NORMAL_REGION_SHARED            (MMU_PAGETABLE_NORMAL_REGION_OUTER_SHARED \
                                                       | MMU_PAGETABLE_NORMAL_REGION_INNER_SHARED)
#define MMU_PAGETABLE_NORMAL_REGION_NONSHARED         ((0 << 9) | (0 << 8))

/* PT: Inner Write-through, Allocate on Read
       Outer Non-cacheable, 
       Non-shared */
#define MMU_PAGETABLE_INNER_WT_OUTER_NONCACHE \
            ( MMU_MAIR_INNER_WTRA_OUTER_NC \
            | MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )

/* PT: Inner Write-Back, Allocate on Read Write
       Outer Write-Back, Allocate on Read Write
       Non-shared */
#define MMU_PAGETABLE_MEM_WRITE_BACK_CACHE \
            ( MMU_MAIR_INNER_WBWARA_OUTER_WBWARA \
		    | MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )
            
/* PT: Inner Non-cacheable
       Outer Non-cacheable 
       Non-shared */            
#define MMU_PAGETABLE_MEM_NON_CACHEABLE \
            ( MMU_MAIR_INNER_NC_OUTER_NC \
		    | MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )
            
/* PT: Inner write-back and Allocate on Read Write,
       Outer Non-cacheble,
       Non-shared */              
#define MMU_PAGETABLE_INNER_WBWARA_OUTER_NONCACHE \
            ( MMU_MAIR_INNER_WBWARA_OUTER_NC \
			| MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )

/* PT: Inner Non-cacheable
       Outer Non-cacheable  
       Shared */
#define MMU_PAGETABLE_MEM_SHARED \
            ( MMU_MAIR_INNER_NC_OUTER_NC \
		    | MMU_PAGETABLE_NORMAL_REGION_SHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )
            
/* PT: Device 
       Shared */            
#define MMU_PAGETABLE_DEVICE_SHARED \
            ( MMU_MAIR_DEVICE_nGnRE \
		    | MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )

/* PT: Inner Write-through, Allocate on Read
       Outer Write-through, Read Allocate,
       Non-shared */             
#define MMU_PAGETABLE_INNER_WTRA_OUTER_WTRA \
            ( MMU_MAIR_INNER_WTRA_OUTER_WTRA \
			| MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )
            
/* PT: Inner Write-through, Read Allocate
       Outer Write-through, Read Allocate, 
       Shared */
#define MMU_PAGETABLE_INNER_WT_OUTER_WB_CACHE_SHARED \
            ( MMU_MAIR_INNER_WTRA_OUTER_WTRA \
			| MMU_PAGETABLE_NORMAL_REGION_SHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )
            
/* PT: Inner Non-cacheable,
       Outer Write-Back, Write Allocate, 
       Non-shared */            
#define MMU_PAGETABLE_INNER_NONCACHE_OUTER_WBWA_CACHE \
            ( MMU_MAIR_INNER_NC_OUTER_WBWA \
			| MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )

/* PT: Inner Non-cacheable,
       Outer Write-Back and Allocate on Read Write
       Non-shared */            
#define MMU_PAGETABLE_INNER_NONCACHE_OUTER_WBWARA \
            ( MMU_MAIR_INNER_NC_OUTER_WBWARA \
			| MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
			| MMU_PAGETABLE_ACCESS_FLAG_SET )			
			
/* PT: Inner Write-through, No Allocate on Write 
       Outer Non-cacheable, 
       Non-shared */
#define MMU_PAGETABLE_INNER_WT_OUTER_WT \
            ( MMU_MAIR_INNER_WT_OUTER_WT \
            | MMU_PAGETABLE_NORMAL_REGION_NONSHARED \
      | MMU_PAGETABLE_ACCESS_FLAG_SET )      

           
#define MMU_LAST_ADDRESS 0xffffffff /* When we actually support >4GB addressing, this will change */


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
  
  /* Secure L3 section */
  MMU_L3_SECTION_MAPPING,
  
  /* Non-secure L3 section */
  MMU_L3_NS_SECTION_MAPPING,
    
  MMU_MEMORY_MAPPING_TYPE_MAX_SIZE = 0x7FFFFFFF

}MMU_MemoryMappingType;

struct mem_block
{
  uint64                     p_base;
  uint64                     v_base;
  uint64                     size_in_kbytes;
  MMU_MemoryMappingType      memory_mapping;
  uint64                     access;
  uint64                     cachetype;
  uint64                     executable;
} ;

#endif /* BOOT_PAGE_TABLE_ARMV7_H */
