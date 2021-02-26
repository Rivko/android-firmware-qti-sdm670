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
typedef struct xcfg_ram_segment_info_type
{
  uintnt offset;
  uintnt size;
  /* physical address where the segment gets loaded */
  uintnt p_addr;
  /* Flag to track if item was hashed at least once */
  boolean is_verified;
  /*Flag to track if the entry is valid. used for scenario where segment into table
    needs to be scanned for info before updating all entries, but
    after setting max_phnum
  */  
  boolean is_valid;
  
}xcfg_ram_segment_info_type;

typedef struct xcfg_ram_elf_info_type
{
  void *hash_buffer;
  uintnt hash_buffer_size;
  xcfg_ram_segment_info_type *segment_info_ptr;
  uintnt max_phnum;
  void *elf_hdr;       /* Pointer to the elf header of image containing xbl
                          config image segments, ie of xbl elf */
  void *prog_hdr;      /* Pointer to the start of the program header for xbl elf */  
  
}xcfg_ram_elf_info_type;

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/
/*===========================================================================
**  Function :  xcfg_ramlib_get_physical_address
** ==========================================================================
*/
/*!
* 
* @brief
*   This function gives physical address corresponding to given offset for
*   config data that is already loaded in ram
*   
* @param[in]
*    blk_offset         data block offset that needs to be translated
*                       to phy addr where its already loaded
*    blk_size           data block size
*
* @param[out] 
*    physical_addr      pointer to elf header
*        
* @par Dependencies
*   None
*   
* @retval
*   xblconfig_status_type.   XBLCONFIG_SUCCESS if no error else error value
* 
* @par Side Effects
*   None
* 
*/
xblconfig_status_type xcfg_ramlib_get_physical_address
(
  uintnt blk_offset,
  uintnt blk_size,  
  uintnt *physical_addr
);

#endif /* XBL_CONFIG_LOADER_ELF_H */
