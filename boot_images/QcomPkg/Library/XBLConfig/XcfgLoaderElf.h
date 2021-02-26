#ifndef XBL_CONFIG_LOADER_ELF_H
#define XBL_CONFIG_LOADER_ELF_H
/*=============================================================================

                      XBLConfig Image Interface Module Sources

GENERAL DESCRIPTION
  This file defines XBL config api's that interface with Elf image
  load module.

Copyright 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
10/14/16     kpa      Initial version
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "XBLConfig.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
typedef struct xcfg_segment_info_type
{
  uintnt offset;
  uintnt size;
  
  /* Flag to track if item was hashed at least once */
  boolean is_verified;
  
}xcfg_segment_info_type;

typedef struct xcfg_elf_info_type
{
  void *hash_buffer;
  uintnt hash_buffer_size;
  xcfg_segment_info_type *segment_info_ptr;
  uintnt max_phnum;
  
}xcfg_elf_info_type;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

#endif /* XBL_CONFIG_LOADER_ELF_H */
