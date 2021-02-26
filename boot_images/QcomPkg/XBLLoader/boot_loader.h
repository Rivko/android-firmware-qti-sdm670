#ifndef BOOT_LOADER_H
#define BOOT_LOADER_H

/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module performs binary image and AMSS loading.

Copyright 2010-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/17   kpa     relocate elf api's to boot_elf_loader.h
05/08/17   kpa     remove unused api declarations
03/10/17   kpa     Update boot_get_elf_phdr_entry_info arguments
03/03/17   ds	   remove clobber prot
01/27/17   digants added whietlist protection header
10/14/16   kpa     Added boot_get_elf_hash_segment_size
04/29/16   kpa     Pass auth enable flag to boot_load_elf_image
07/15/15   ck      Added boot_elf_load_deinit
07/01/15   as      Removed boot_elf_get_filtered_segment_start
02/25/15   kpa     Add config entry whitelist parameter to boot_populate_sbl_info
01/05/14   kpa     remove boot_load_header_and_image
10/27/14   kedara  Added SAFE 1.0
10/09/14   kedara  Added boot_elf_get_filtered_segment_start
09/24/14   kedara  Added boot_set_elf_and_prog_header
08/08/14   ck      Updated sahara function prototypes for 64 bit support
06/13/14   ck      Added boot_elf_segment_in_whitelist
06/13/14   ck      Whitelist region pointer now passed to boot_load_elf_image
03/05/14   ck      Updated boot_elf_verify_image_header parameter list
12/09/13   jz      64bit elf loading support
10/24/13   ck      Removed boot_rpm_cookie_sync, boot_init_rpm_entry_address,
                   and boot_notify_bl_jump_to_rpm
10/21/13   dhaval  declare api to explicitly hash ehdr+phdr
07/29/13   aus     Support for loading ELF SDI image
02/19/13   kedara  Added boot_get_elf_header, sbl_sahara_get_elf_header,
                   boot_get_program_header, sbl_sahara_get_program_header.
11/28/12   dh      Add boot_rpm_cookie_sync
11/08/12   kedara  Updated boot_elf_set_auth_and_ssa_enable
09/21/12   jz      Updated boot_load_elf_image
06/15/12   kedara  Added boot_elf_set_is_ssa_enabled.
06/14/12   kedara  Added support to read FOTA partition and check for 
                   recovery mode cookie
09/01/11   dh      Removed unused functions
08/24/11   aus     Added support for sahara secure boot
06/27/11   aus     Added flashless boot support
04/29/11   dh      Add boot_load_image_header to support loading image header
03/18/11   dxiang  Add new function boot_load_elf_image() for ELF image loading 
02/22/11   dxiang  Add new function boot_load_ssd_partition() for modem encryption 
01/28/11   dxiang  Add new function boot_init_rpm_entry_addr()
08/04/10   aus     Clean-up secboot 3.0
07/22/10   aus     Initial revision.
============================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/
#include "MIheader.h"
#include "boot_sbl_shared.h"
#include "boot_comdef.h"
#include "boot_extern_sec_img_interface.h"
#include "boot_whitelist_prot.h"
#include "boot_elf_loader.h"

/*=============================================================================

                              DEFINITIONS

=============================================================================*/
/* Cookie that defines FOTA recovery mode */
#define FOTA_RECOVERY_COOKIE 0x64645343

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : sbl_sahara_entry
** ============================================================================
*/
/*!
*   @brief
*   This function provides an entry into the Sahara protocol from SBL1.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @details
*   This function provides an entry into the Sahara protocol from PBL.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void sbl_sahara_entry ( 
   mi_boot_image_header_type *header_ptr,
   uint32 image_id,
   uint32 image_type,
   uint32 sec_image_type,
   boot_boolean auth_flag,   
   boot_boolean jump_flag,   
   bl_shared_data_type * bl_shared_data   
);

/* ============================================================================
**  Function : sbl_sahara_get_elf_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns elf header base address.
*   
*   @details
*   This function returns elf header base address for the last loaded elf image
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_elf_header( void );


/* ============================================================================
**  Function : sbl_sahara_get_program_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns program header buffer base address.
*   
*   @details
*   This function returns program header buffer base address
*   for the last loaded elf image.
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_program_header( void );

#endif /* BOOT_LOADER_H */

