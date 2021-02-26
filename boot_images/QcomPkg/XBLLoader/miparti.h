#ifndef __MIPARTI_H__
#define __MIPARTI_H__
/*===========================================================================

                Multi-Image Boot Partition Table Definitions

DESCRIPTION
  This header file gives the external definition of Partition Table,
  located in page 1 of the fourth and fifth non-bad blocks in NAND 
  flash for Multi-Image Boot targets.
  
  There are two copies of the data in these blocks which allow us to 
  alternate between them and always have a good copy which points to 
  valid boot loader, even if a programming operation is accidentally
  interrupted

  Copyright 2004-2009, 2012, 2015 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.


   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
09/10/12   kedara  Increase number of partitions to 32
11/03/09   as      Changes to support dual nand interleave
10/26/09   msm     Changes for CMI.
03/02/09   dhaval  qsd8650 and mdm8200 single SU support changes
03/12/09   mm      Merged to tip
09/15/08   mh      Add FLASH_PART_MLC_START_BOUNDARY for SLC/MLC device
10/27/07   rt      Add a partition attribute defining software ECC profile
10/12/07   rt      Force enum to wrap around to '0x0' from '0xFF'
09/04/07   dp      Add 2X Mode Support
08/04/07   rt      Add attribute to support name based partition upgrade
05/22/07   mm      Add support for Hardware ECC on Main and Spare Areas.
02/28/07   rt      Add support for NOR partitioning
05/12/06   rt      Add macros to define partition attributes
10/27/05   rt      Add user partition table structure
11/02/04   drh     Change all names from MI* to FLASH*.  Add GROW define.
                   Add conditional include
08/14/04   drh     Created.

===========================================================================*/

#include "boot_comdef.h"

/*------------------------------------------------------------
 *
 *  Partition Table Definitions
 *
 * ---------------------------------------------------------*/
 
/* Total length of zero filled Name string.  This is not a C
 * string, as it can occupy the total number of bytes, and if
 * it does, it does not require a zero terminator.  It cannot
 * be manipulated with standard string handling library functions
 */
#define FLASH_PART_NAME_LENGTH 16

/* Magic numbers used in identifying valid partition table */
#define FLASH_PART_MAGIC1     0x55EE73AA
#define FLASH_PART_MAGIC2     0xE35EBDDB

/* Magic numbers used in identifying valid user partition table */
/* The magic numbers are really temporary for now; Will be changed later */
#define FLASH_USR_PART_MAGIC1     0xAA7D1B9A
#define FLASH_USR_PART_MAGIC2     0x1F7D48BC

/* Must increment this version whenever structure of partition table
 * changes.
 */
#define FLASH_PARTITION_VERSION   0x3


/* Value which indicates the partition can grow to fill the
 * rest of the flash.  Must only be used on the last partition.
 */
#define FLASH_PARTITION_GROW  0xFFFFFFFF

/* Macro definitions related to partition attribute flags */
#define FLASH_PART_FLAG_BASE  0xFF
#define FLASH_PART_ATTR_BASE  0xFF

/* Attributes for NAND paritions */
#define FLASH_PART_HEX_SIZE   0xFE

/* Attributes for NOR partitions */
#define FLASH_PART_SIZE_BLK_CNT 0xFE
#define FLASH_PART_FIXED_ADDR   0xFD
#define FLASH_PART_NEW_BANK     0xFB

/* Attribute Masks */
#define FLASH_PART_ATTRIBUTE1_BMSK 0x000000FF
#define FLASH_PART_ATTRIBUTE2_BMSK 0x0000FF00
#define FLASH_PART_ATTRIBUTE3_BMSK 0x00FF0000

/* Attribute Shift Offsets */
#define FLASH_PART_ATTRIBUTE1_SHFT 0
#define FLASH_PART_ATTRIBUTE2_SHFT 8
#define FLASH_PART_ATTRIBUTE3_SHFT 16

#define FLASH_PART_ATTRIB( val, attr ) (((val) & (attr##_BMSK)) >> attr##_SHFT)

/* Partition attributes */
typedef enum {
  /* No specific attribute defintion. Fall back to default operations
     within NAND controller or the NOR driver. */
  FLASH_PARTITION_DEFAULT_ATTRB = 0xFF,

  /* For NAND this corresponds to read based operations that sequentially
     operate on the pages per block with no special handling. This applies
     to clients such as bootloaders and CODE that open the partitions only for
     READ based access.
     For NOR this corresponds to read only partitions. */
  FLASH_PARTITION_READ_ONLY = 0,

  /* For SLC NAND/OneNAND this corresponds to read/write based operations that
     sequentially operate on the pages per block.
     For MLC NAND/OneNAND this corresponds to read/write based operations that
     take into consideration the corresponding logical page to physical page
     translation to use the MLC in SLC mode and thereby half the density to
     circumvent power loss issues due to paired pages in the MLC.
  */
  FLASH_PARTITION_SLC_READWRITE,

  /* Applies only to MLC devices, when corresponding partition opened in
     read-write mode.
     For MLC NAND/OneNAND this corresponds to read/write based operations that
     sequentially operate on the pages per block with no special handling.
  */
  FLASH_PARTITION_MLC_READWRITE,
  
  /* For SLC OneNAND this corresponds to read/write based operations that
     operate on the pages and blocks in 2X Mode
     Currently applies only to SLC OneNAND.
  */
  FLASH_PARTITION_SLC_READWRITE_2X
  
} flash_partition_attrb_t;


/* Partition attributes related to Spare ECC*/
typedef enum {
  /* No specific attribute defintion. Fall back to default operations
     within NAND controller or the NOR driver. */
  FLASH_PARTITION_DEFAULT_ATTRB2 = 0xFF,

  /* ECC Enabled in Main Area */
  FLASH_PARTITION_MAIN_AREA_ONLY = 0,

  /* ECC Enabled on Main & Spare Area */
  FLASH_PARTITION_MAIN_AND_SPARE_ECC,

  /* This attribute determines software ECC protection
   * on the spare area for data partitions. Currently,
   * this profile is used by WinMobile 6.0 */
  FLASH_PARTITION_SPARE_SW_ECC_PROFILE_A

} flash_partition_attrb2_t;


/* This type defines partition attributes used by external clients. These
 * attributes are not used by flash driver internally to reconfigurw the NAND
 * controller. In future these can be accessed by clients using a driver API.
 */
typedef enum {
  /* No specific attribute defintion. Fall back to default operations
   * within NAND controller or the NOR driver.
   */
  FLASH_PART_DEF_ATTRB3 = 0xFF,

  /* Applies to partitions that can be upgraded using name based upgrade
   * mechanism by providing the partition name. This attribute has to be defined
   * on partitions that are required to be upgrade-able using this mechanism.
   */
  FLASH_PART_ALLOW_NAMED_UPGRD = 0,

  /* This partition attributed is used by flash driver to help
     deliniate start of MLC region in SLC/MLC combo Devices.
   */  
  FLASH_PART_MLC_START_BOUNDARY = 1,


  /* For Target builds that support  dual nand devices */
  FLASH_DUAL_NAND_INTERLEAVE = 2

} flash_partition_attrb3_t;


/* System Partition table entry definition */
struct flash_partition_entry;
typedef struct flash_partition_entry *flash_partentry_t;
typedef struct flash_partition_entry *flash_partentry_ptr_type;

struct flash_partition_entry {

  /* Name of the partition in the form of 0:ALL, 0:EFS2, etc. */
  char name[FLASH_PART_NAME_LENGTH];

  /* Offset in blocks from beginning of device */
  uint32 offset;

  /* length in blocks of the partition */
  uint32 length;

  /* Partition attributes */
  uint8 attrib1;
  uint8 attrib2;
  uint8 attrib3;

  /* Numeric ID of flash part (first = 0, second = 1) */
  uint8 which_flash;
};

/* User partition table entry definition */
struct flash_usr_partition_entry;
typedef struct flash_usr_partition_entry *flash_usr_partentry_t;
typedef struct flash_usr_partition_entry *flash_usr_partentry_ptr_type;

struct flash_usr_partition_entry {

  /* Name of the partition in the form of 0:ALL, 0:EFS2, etc. */
  char name[FLASH_PART_NAME_LENGTH];

  /* Size in KB for the partition */
  uint32 img_size;

  /* Padding in KB for static handling of NAND bad blocks in this partition */
  /* This field can also be used to define minimum size requirement for a
   * parition defined in terms of number of sectors/blocks.
   * Please note that this is possible because there never are bad blocks on a
   * a NOR device */
  uint16 padding;

  /* Numeric ID of flash part (first = 0, second = 1) */
  uint16 which_flash;

  /* Attributes for this partition - This get copied to the attribx flags in
     system partition table */
  uint8 reserved_flag1;
  uint8 reserved_flag2;
  uint8 reserved_flag3;

  uint8 reserved_flag4;  /* layout based flags */

};

/*  Number of partition tables which will fit in 512 byte page, calculated
 *  by subtracting the partition table header size from 512 and dividing
 *  the result by the size of each entry.  There is no easy way to do this
 *  with calculations made by the compiler.
 *
 *     16 bytes for header
 *     28 bytes for each entry
 *     512 - 16 = 496
 *     496/28 = 17.71 = round down to 16 entries
 */
#define FLASH_NUM_PART_ENTRIES  32

/* NOR device does not have concept of pages. This macro retricts the max
 * length of a parition table on NOR flash to 512 bytes.
 */
#define NOR_FLASH_PARTI_TBL_LENGTH 512

/* System Partition table definition */
struct flash_partition_table;
typedef struct flash_partition_table *flash_partable_t;

struct flash_partition_table {

  /* Partition table magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* Partition table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.  Be sure to increment
   * version number if you change it.
   */
  uint32 numparts;   /* number of partition entries */
  struct flash_partition_entry part_entry[FLASH_NUM_PART_ENTRIES];
};


/* User partition table definition */
struct flash_usr_partition_table;
typedef struct flash_usr_partition_table *flash_usr_partable_t;


struct flash_usr_partition_table {

  /* Partition table magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* Partition table data.  This portion of the structure may be changed
   * as necessary to accommodate new features.  Be sure to increment
   * version number if you change it.
   */
  uint32 numparts;   /* number of partition entries */
  struct flash_usr_partition_entry part_entry[FLASH_NUM_PART_ENTRIES];
};

#endif /* __MIPARTI_H__ */
