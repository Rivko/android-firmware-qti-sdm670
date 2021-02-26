#ifndef __MI_PROGRESSIVE_H__
#define __MI_PROGRESSIVE_H__
/*===========================================================================

                Progressive Boot Definitions

DESCRIPTION
  This header file gives the external definition of the structures used
  by the Progressive Boot Loader to individually and selectively load
  ELF segments from NAND flash to memory as needed.
  
  Copyright 2004-2007, 2009-2011, 2013-2015, 2017 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.


   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
22/02/17   yps    move this file from XBLLoader to boot api header folder
10/05/15   ck      Corrected copyright
10/29/14   kpa     Support 64 bit memory map
11/19/13   jz      Moved ELF related defines out to boot_elf_header.h
02/04/11   aus     Added MI_PBT_PHDR_SEGMENT type used in MBN Tools
06/04/10   sl      Decreased MI_PBT_MAX_SEGMENTS for memory efficiency.
05/14/10   niting  Added ELF segment type definitions.
10/26/09   msm     Changes for CMI.
03/02/09   dhaval  qsd8650 and mdm8200 single SU support changes
11/30/07   ty      Add support for demand loading
11/27/07   rt      Updated Copyright statement; make cosmetic changes
03/05/07   rt      Add support for L4BSP
02/28/07   rt      Mark BOOT and HASH segments as 'read_only'.
12/06/06   dp      Bring progressive boot version 5 updates
06/13/06   rt      Increase maximum number of progressive boot segments to 32
12/12/05   ty      Add support for shared segment
08/30/05   tbg     Added elf header definitions
08/24/05   tbg     Added support for segment memory attributes (RO/RW).
                   Incremented version number of header.
                   Pulled in changes from MIBIB.01.02 branch.
04/21/05   tbg     Added new defines for HASH and NOT_USED segments
02/07/05   drh     Use correct define in progressive structure
11/02/04   drh     Add conditional include
08/14/04   drh     Created.

===========================================================================*/



#include "boot_comdef.h"
#include "boot_elf_header.h"

#ifndef    SIZE4K
#define SIZE4K 4096
#endif

#ifndef    SIZE_4K_MINUS_1
#define SIZE_4K_MINUS_1 (SIZE4K - 1)
#endif

/*------------------------------------------------------------
 *
 *  Progressive Boot Table Definitions
 *
 * ---------------------------------------------------------*/
 
/* Magic numbers used in identifying valid progressive boot table (PBT) */
#define MI_PBT_MAGIC1     0xEFBEADDE
#define MI_PBT_MAGIC2     0x98765432

/* Must increment this version whenever structure of progressive header
 * changes.
 */
#define MI_PBT_VERSION   0x5


/* The latest ELF documentation is at http://www.caldera.com/developers/gabi/latest/ch5.pheader.html
 * and it talks about OS and PROCESSOR specific fields.
 * we use the bits (PF_MASKOS) in Elf32_PHdr.p_flags as recomended by ARM
 */

/* Definition for segment flags used in p_flag of program_segmen header
 *
 *                 Pool Indx    Segment type    Access type   Page/non page
 * bits in p_fag/-----27-----/----26-24-------/---- 23-21----/------20-------/
 */
/* Note: MI_PBT_MAX_SEGMENTS would impact OSBL memory usage */
#ifndef MI_PBT_MAX_SEGMENTS
#define MI_PBT_MAX_SEGMENTS 100
#endif

#define MI_PBT_NON_PAGED_SEGMENT   0x0
#define MI_PBT_PAGED_SEGMENT       0x1

#define MI_PBT_RW_SEGMENT          0x0
#define MI_PBT_RO_SEGMENT          0x1
#define MI_PBT_ZI_SEGMENT          0x2
#define MI_PBT_NOTUSED_SEGMENT     0x3
#define MI_PBT_SHARED_SEGMENT      0x4

#define MI_PBT_L4_SEGMENT          0x0
#define MI_PBT_AMSS_SEGMENT        0x1
#define MI_PBT_HASH_SEGMENT        0x2
#define MI_PBT_BOOT_SEGMENT        0x3
#define MI_PBT_L4BSP_SEGMENT       0x4
#define MI_PBT_SWAPPED_SEGMENT     0x5
#define MI_PBT_SWAP_POOL_SEGMENT   0x6
#define MI_PBT_PHDR_SEGMENT        0x7

#define MI_PBT_POOL_INDEX          0x1

#define MI_PBT_FLAG_PAGE_MODE_MASK        0x100000
#define MI_PBT_FLAG_ACCESS_TYPE_MASK      0xE00000
#define MI_PBT_FLAG_SEGMENT_TYPE_MASK     0x7000000
#define MI_PBT_FLAG_POOL_INDEX_MASK       0x8000000

#define MI_PBT_FLAG_PAGE_MODE_SHIFT       0x14
#define MI_PBT_FLAG_ACCESS_TYPE_SHIFT     0x15
#define MI_PBT_FLAG_SEGMENT_TYPE_SHIFT    0x18
#define MI_PBT_FLAG_POOL_INDEX_SHIFT      0x1B


#define MI_PBT_FLAGS_MASK                 0x0FF00000
#define MI_PBT_PHDR_FLAGS_SHIFT           0x14


#define MI_PBT_ELF_AMSS_NON_PAGED_RW_SEGMENT \
          (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT)

#define MI_PBT_ELF_AMSS_NON_PAGED_RO_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )

#define MI_PBT_ELF_AMSS_NON_PAGED_ZI_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_ZI_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )

#define MI_PBT_ELF_AMSS_NON_PAGED_NOTUSED_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_NOTUSED_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )

#define MI_PBT_ELF_AMSS_NON_PAGED_SHARED_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_SHARED_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )


#define MI_PBT_ELF_AMSS_PAGED_RW_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_AMSS_PAGED_RO_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)    | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_AMSS_PAGED_ZI_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_ZI_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)    | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_AMSS_PAGED_NOTUSED_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT)   | \
            (MI_PBT_NOTUSED_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_AMSS_PAGED_SHARED_SEGMENT \
          ( (MI_PBT_AMSS_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT)   | \
            (MI_PBT_SHARED_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )


#define MI_PBT_ELF_HASH_SEGMENT \
          ( (MI_PBT_HASH_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )

#define MI_PBT_ELF_BOOT_SEGMENT \
           ( (MI_PBT_BOOT_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
             (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT) )

#define MI_PBT_ELF_NON_PAGED_L4BSP_SEGMENT \
          (MI_PBT_L4BSP_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT)

#define MI_PBT_ELF_PAGED_L4BSP_SEGMENT \
           ( (MI_PBT_L4BSP_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
             (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX0 \
          ( (MI_PBT_SWAPPED_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)       | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_SWAPPED_PAGED_RO_SEGMENT_INDEX1 \
          ( (MI_PBT_SWAPPED_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_RO_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)       | \
            (MI_PBT_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT)      | \
            (MI_PBT_POOL_INDEX << MI_PBT_FLAG_POOL_INDEX_SHIFT) )

#define MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX0 \
          ( (MI_PBT_SWAP_POOL_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_ZI_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)         | \
            (MI_PBT_NON_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT) )

#define MI_PBT_ELF_SWAP_POOL_NON_PAGED_ZI_SEGMENT_INDEX1 \
          ( (MI_PBT_SWAP_POOL_SEGMENT << MI_PBT_FLAG_SEGMENT_TYPE_SHIFT) | \
            (MI_PBT_ZI_SEGMENT << MI_PBT_FLAG_ACCESS_TYPE_SHIFT)         | \
            (MI_PBT_NON_PAGED_SEGMENT << MI_PBT_FLAG_PAGE_MODE_SHIFT)    | \
            (MI_PBT_POOL_INDEX << MI_PBT_FLAG_POOL_INDEX_SHIFT))


#define MI_PBT_PAGE_MODE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_PAGE_MODE_MASK) >> \
           MI_PBT_FLAG_PAGE_MODE_SHIFT )

#define MI_PBT_ACCESS_TYPE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_ACCESS_TYPE_MASK) >> \
           MI_PBT_FLAG_ACCESS_TYPE_SHIFT )

#define MI_PBT_SEGMENT_TYPE_VALUE(x) \
         ( ((x) & MI_PBT_FLAG_SEGMENT_TYPE_MASK) >> \
            MI_PBT_FLAG_SEGMENT_TYPE_SHIFT )

#define MI_PBT_POOL_INDEX_VALUE(x) \
          ( ((x) & MI_PBT_FLAG_POOL_INDEX_MASK) >> \
            MI_PBT_FLAG_POOL_INDEX_SHIFT )


/* Segment memory attributes */
#define MI_PBT_MEM_READ_WRITE 0
#define MI_PBT_MEM_READ_ONLY  1

/* Size of computed HASH values for progressive boot segments */
#define MI_PROG_BOOT_DIGEST_SIZE  20

/* Standard ELF segment type definitions */
#define MI_PBT_ELF_PT_NULL    0
#define MI_PBT_ELF_PT_LOAD    1
#define MI_PBT_ELF_PT_DYNAMIC 2
#define MI_PBT_ELF_PT_INTERP  3
#define MI_PBT_ELF_PT_NOTE    4
#define MI_PBT_ELF_PT_SHLIB   5
#define MI_PBT_ELF_PT_PHDR    6
#define MI_PBT_ELF_TLS        7

/* Definition of record for each loadable segment */
struct segment_record {
  uint32 type;
  uint32 segment_offset;    /* offset from first data block in NAND       */
  uintnt v_address;         /* Virtual execution address Address in RAM   */
  uintnt p_address;         /* Physical execution address Address in RAM  */
  uint32 size;              /* Size in bytes that needs to be copied      */
  uint32 mem_size;          /* Size in bytes that is actually used in RAM */
  uint32 memory_attribute;  /* Indicates if memory is RW or RO            */
  uint32 align;             /* Segment alignment */
};

/* Definition of Progressive Boot Table containing segment records */
struct progressive_boot_block {
  uint32          magic1;                   /* MAGIC1                 */
  uint32          magic2;                   /* MAGIC2                 */
  uint32          version;                  /* PBT version number     */
  uint32          numsegments;              /* Number of ELF segments */
  uint32          elf_image_entry_point;    /* ELF entry point addr   */

  struct segment_record segment[MI_PBT_MAX_SEGMENTS];
                               /* One record for each segment */
};

typedef struct progressive_boot_block progressive_boot_block_type;


#endif /* __MI_PROGRESSIVE_H__ */

