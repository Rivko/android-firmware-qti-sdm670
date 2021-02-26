#ifndef BOOT_MINIDUMP_H
#define BOOT_MINIDUMP_H
/*=============================================================================

                               Boot minidump
                               Header File
GENERAL DESCRIPTION
  This file provides the APIs for minidump feature
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/21/17   aus     Removed packed attribute
12/12/17   aus     Updated structures as packed
10/12/17   aus     Updated structures as per design
01/17/17   bd      Initial version

=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define MD_REVISION                       1   
#define SBL_MINIDUMP_SMEM_ID              602
#define MD_SHARED_IMEM_BASE               (SHARED_IMEM_BASE + 0xB88)
#define MAX_NUM_OF_SS                     10
#define MD_REGION_NAME_LENGTH             16

#define MD_REGION_VALID                   ('V' << 24 | 'A' << 16 | 'L' << 8 | 'I' << 0)
#define MD_REGION_INVALID                 ('I' << 24 | 'N' << 16 | 'V' << 8 | 'A' << 0)
#define MD_REGION_INIT                    ('I' << 24 | 'N' << 16 | 'I' << 8 | 'T' << 0)
#define MD_REGION_NOINIT                  0

#define MD_SS_ENCR_REQ                    (0 << 24 | 'Y' << 16 | 'E' << 8 | 'S' << 0)
#define MD_SS_ENCR_NOTREQ                 (0 << 24 | 0 << 16 | 'N' << 8 | 'R' << 0)
#define MD_SS_ENCR_DONE                   ('D' << 24 | 'O' << 16 | 'N' << 8 | 'E' << 0)
#define MD_SS_ENCR_START                  ('S' << 24 | 'T' << 16 | 'R' << 8 | 'T' << 0)
#define MD_SS_ENABLED                     ('E' << 24 | 'N' << 16 | 'B' << 8 | 'L' << 0)
#define MD_SS_DISABLED                    ('D' << 24 | 'S' << 16 | 'B' << 8 | 'L' << 0)


/* SMEM region struct */
typedef struct 
{
  uint8     region_name[MD_REGION_NAME_LENGTH];       
  uint32    seq_num;        /* provision to allow unique naming */
  uint32    md_valid;       /* To dump or not */
  uint64    region_base_address;
  uint64    region_size;  
}  md_ss_region;

/* minidump SMEM SubSystem region */
typedef struct  /* SS SMEM ToC struct */
{
   uint32       md_ss_toc_init;
   uint32       md_ss_enable_status;
   uint32       encryption_status;
   uint32       encryption_required; 
   uint32       ss_region_count; /*region count */
   uint64       md_ss_smem_regions_baseptr;
}  md_ss_toc;

/* SMEM struct minidump_smem_global region */
typedef struct 
 { 
   uint32       md_toc_init;
   uint32       md_revision;
   uint32       md_enable_status;
   md_ss_toc    md_ss_toc[MAX_NUM_OF_SS];
}  md_global_toc;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
void add_minidump_regions(void);

void dload_debug_encr_all_regions(uint32 start_index, uint32 num_regions);

void dload_debug_check_oem_key(uint32 start_index, uint32 num_regions);

void dload_debug_crypto_init(void);

void dload_debug_add_unencr_regions(void);

#endif  /* BOOT_MINIDUMP_H */
