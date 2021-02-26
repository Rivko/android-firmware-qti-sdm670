#ifndef MIBIB_H
#define MIBIB_H
/*===========================================================================

                Multi-Image Boot Information Block Definitions

DESCRIPTION
  This header file gives the external definition of the Boot Information
  Block, located in page 0 of the fourth and fifth non-bad blocks in NAND 
  flash for Multi-Image Boot targets.
  
  There are two copies of the data in these blocks which allow us to 
  alternate between them and always have a good copy which points to 
  valid boot loader, even if a programming operation is accidentally
  interrupted.
  
  NOTE:  Even though this block will contain various headers, each one
         of them is in its own page, so there is not a single structure
         which would encompass them all.  As each page is read in, the
         appropriate structure pointer should be applied, based on the
         structure being accessed.

  Copyright 2004-2013 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/24/13   jz      Added QSEE/QHEE_IMG
01/18/13   kedara  Added explicit enum values to struct image_type.
06/07/12   kedara  Added new image ID for ACDB, WDT, MBA.
08/04/11   aus     Renamed LPASS to DSP3
07/14/11   aus     Added new image ID for LPASS
03/04/11   dxiang  Introduce general image type into image_type enumeration
02/22/11   dxiang  Introduce new type into image_type enumeration
09/21/10   plc     Added secboot3 SBL image names
08/02/10   aus     Added support for secboot 3
05/16/10   rk      Corrected the enum order on image_type
04/26/10   niting  Assigned image types to definitive value
04/09/10   niting  Updated image types 
04/05/10   niting  Moved image_type definition
03/03/10   plc     Added macors for "0:SPBL" partition names
03/11/09   mm      Added ADSP Q5 decoupling.
02/14/08   op      Added macros for "0:DBL" partition names
01/08/08   op      Added macros for DSP partition names
10/21/07   rt      Add support for Secure Boot 2.0
09/22/07   op      Add support for "0:ALL" partition
02/28/07   rt      Add support for NOR partitioning
11/16/06   rt      Added macro for FTL partition name
05/12/06   rt      Bump the version number
10/27/05   rt      Added CRC page in MIBIB
10/26/05   rt      Added user partition table page in MIBIB
08/23/05   drh     Add define SIM Secure partition name
03/30/05   amw     Added additional image type defines
03/15/05   drh     Add new limit to search for MIBIB length.
11/02/04   drh     Add conditional include
08/14/04   drh     Created.

===========================================================================*/


#include "comdef.h"

/*------------------------------------------------------------
 *
 *  Multi-Image Boot Block Information Definitions
 *
 * ---------------------------------------------------------*/
 
/* Magic numbers used in identifying valid MIBIB */
#define MIBIB_MAGIC1     0xFE569FAC
#define MIBIB_MAGIC2     0xCD7F127A

/* Must increment this version whenever structure of bad block table
 * changes.
 */

/* SB 2.0 is supported by MIBIB version 4 structures. If target is using
 * SB 2.0 use version '4' MIBIB.
 */ 
#define MIBIB_VERSION   0x4

/* Before we have a partition table, use this limit */
#define MIBIB_DEFAULT_PARTI_SIZE  0x10

/* Multi-Image Boot Information block header structure */

struct mi_boot_info_block;
typedef struct mi_boot_info_block *mi_boot_info_t;

struct mi_boot_info_block {

  /* MIBIB magic numbers and version number.
   *   WARNING!!!!
   *   No matter how you change the structure, do not change
   *   the placement of the first three elements so that future
   *   compatibility will always be guaranteed at least for
   *   the identifiers.
   */
  uint32 magic1;
  uint32 magic2;
  uint32 version;

  /* MIBIB age field
   *   NOTE:
   *   When comparing one age to another age, since this is a 32-bit
   *   number, it will be impossible to wrap, given the number
   *   of times we would have to increment the number.  A simple
   *   greater than or less than comparison will be sufficient.  No
   *   need to check for wrap around.
   */
  uint32 age;
};


/* Definitions for which pages contain which headers */

#define MIBIB_PAGE_MAGIC_VERSION_PAGE    0x0
#define MIBIB_PAGE_PARTITION_TABLE       0x1
#define MIBIB_PAGE_USR_PARTI_TBL         0x2
#define MIBIB_PAGE_MIBIB_BLK_CRC         0x3


/* Define the last page number in the MIBIB.  This is used when
 * copying forward the information from the old MIBIB to the
 * new MIBIB.  
 *
 * WARNING:  If you add a new page to the MIBIB, you must
 *           update this define to use the last defined page.
 */
#define MIBIB_PAGE_LAST_PAGE  (MIBIB_PAGE_MIBIB_BLK_CRC + 1)


/* Definition for the longest span of blocks we will scan through
 * looking for a valid MIBIB.  This is way overkill, but will allow
 * for a large number of bad blocks to be encountered.  We determine
 * this fudge factor in the following way.  An 4096 block flash is allowed
 * to have about 2% bad blocks.  They can all cluster in the MIBIB
 * area.  If so, then we need to search through at least 82 blocks
 * plus the number of good blocks we are requiring in the MIBIB area.
 * That number is 16 blocks.  Adding together, we get 98, so for
 * good measure, we will use a round 96 (0x60) blocks.  This will have a
 * speed impact when the flash is completely erased, but when there
 * are actually MIBIB images in the flash we will stop looking when
 * we find the MIBIB.
 */
#define MIBIB_BLOCK_SEARCH_MAX          0x40


/* Defines for Partition names.  Used both in partition table
 * C file and in JNAND code.  These must not be changed without
 * close investigation of the code.
 */
#define MIBIB_PARTI_NAME               "0:MIBIB"
#define MIBIB_SIMSECURE_PARTI_NAME     "0:SIM_SECURE"
#define MIBIB_QCSBL_PARTI_NAME         "0:QCSBL"
#define MIBIB_OEMSBL1_PARTI_NAME       "0:OEMSBL1"
#define MIBIB_OEMSBL2_PARTI_NAME       "0:OEMSBL2"
#define MIBIB_AMSS_PARTI_NAME          "0:AMSS"
#define MIBIB_APPS_PARTI_NAME          "0:APPS"
#define MIBIB_APPSBL_PARTI_NAME        "0:APPSBL"
#define MIBIB_FOTA_PARTI_NAME          "0:FOTA"
#define MIBIB_EFS2_MODEM_PARTI_NAME    "0:EFS2"
#define MIBIB_EFS2_APPS_PARTI_NAME     "0:EFS2APPS"
#define MIBIB_EFS2_QDSP_PARTI_NAME     "0:EFS2QDSP"
#define MIBIB_FTL_APPS_PARTI_NAME      "0:FTL"
#define MIBIB_DSP1_PARTI_NAME          "0:DSP1"
#define MIBIB_DSP2_PARTI_NAME          "0:DSP2"
#define MIBIB_ADSP_Q5_PARTI_NAME       "0:QDSP_Q5"

/* This is a special partition name used by flash driver to represent
 * complete device. Note that this partition name is reserved for system
 * use and cannot be used to name any other user defined partition.
 */  
#define MIBIB_DEVICE_ALL_PARTI_NAME     "0:ALL"

/* Partition names used by NOR multi-image boot architecture */
#define MIBIB_NOR_BOOT_PARTI_NAME       "0:BOOTVECT"
#define MIBIB_NOR_PARTI_INFO_PARTI_NAME "0:MIPIB"
#define MIBIB_OEMSBL_PARTI_NAME         "0:OEMSBL"

/* Partition names used by Secure Boot architecture 2.0 */
#define MIBIB_DBL_PARTI_NAME            "0:DBL"
#define MIBIB_FSBL_PARTI_NAME           "0:FSBL"
#define MIBIB_OSBL_PARTI_NAME           "0:OSBL"

/* Partition names used by Secure Boot architecture 3.0 */
#define MIBIB_SBL1_PARTI_NAME          "0:SBL1"
#define MIBIB_SBL2_PARTI_NAME          "0:SBL2"
#define MIBIB_SBL3_PARTI_NAME          "0:SBL3"
#define MIBIB_RPM_PARTI_NAME           "0:RPM"
#define MIBIB_TZ_PARTI_NAME            "0:TZ"

/* Image type definition */
/************************************************************/
/*  Image Type Enum definition is moved from miheader.h     */
/************************************************************/

typedef enum
{
  NONE_IMG = 0,
  OEM_SBL_IMG  = 1,
  AMSS_IMG     = 2,
  QCSBL_IMG    = 3,
  HASH_IMG     = 4,
  APPSBL_IMG   = 5,
  APPS_IMG     = 6,
  HOSTDL_IMG   = 7,
  DSP1_IMG     = 8,
  FSBL_IMG     = 9,
  DBL_IMG      = 10,
  OSBL_IMG     = 11,
  DSP2_IMG     = 12,
  EHOSTDL_IMG  = 13,
  NANDPRG_IMG  = 14,
  NORPRG_IMG   = 15,
  RAMFS1_IMG   = 16,
  RAMFS2_IMG   = 17,
  ADSP_Q5_IMG  = 18,
  APPS_KERNEL_IMG  = 19,
  BACKUP_RAMFS_IMG = 20,
  SBL1_IMG      = 21,
  SBL2_IMG      = 22,
  RPM_IMG       = 23,  
  SBL3_IMG      = 24,
  TZ_IMG        = 25,
  SSD_KEYS_IMG  = 26, 
  GEN_IMG       = 27,
  DSP3_IMG      = 28,
  ACDB_IMG      = 29,  
  WDT_IMG       = 30,
  MBA_IMG       = 31,
  QSEE_IMG      = 32,
  QHEE_IMG      = 33,

 /******************************************************/
 /* Always add enums at the end of the list. there are */
 /*  hard dependencies on this enum in apps builds     */
 /*  which DONOT SHARE this definition file            */
 /******************************************************/

  /* add above */
  MAX_IMG = 0x7FFFFFFF
}image_type;

#endif /* MIBIB_H */
