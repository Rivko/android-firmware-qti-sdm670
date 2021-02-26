#ifndef BOOT_FLASH_DEV_SDCC_H
#define BOOT_FLASH_DEV_SDCC_H

/*=============================================================================

                        Boot Loader eMMC Flash Device Module

GENERAL DESCRIPTION
  Contains the implementation for the boot MMC flash device module.

Copyright 2009-2012, 2015 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
06/06/12   kedara  remove FEATURE_BOOT_SDCC_BOOT for header inclusion.
                   boot_flash_dev_sdcc_if.h needed to derive definition
                   for boot_flash_trans_factory_init_type.
09/01/11   dh      if FEATURE_BOOT_SDCC_BOOT is not defined don't include
                    boot_flash_dev_sdcc_if.h
11/12/10   plc     Add inline stub for init_and_register func, if 
                   FEATURE_BOOT_SDCC_BOOT is undefined, sdcc ifc is omitted
12/08/09   aus     Added write protection function 
08/31/09   VTW     Updated for eMMC boot.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include "boot_flash_dev_sdcc_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_SDCC_VERSION 1
#define BOOT_FLASH_DEV_SDCC_NAME "boot_flash_dev_sdcc"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_init_and_register

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize the boot SDCC flash device module.
*   The function performs the nessarily initializations to read
*   data from the SD cards.
*   It registers the class singletons for this module.
* 
* @param[in] flash_type Flash type
* @param[in] trans_factory_init Flash translation factory interface
* @param[in] bl_shared_data Pointer to the shared data
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_SDCC_BOOT
void boot_flash_dev_sdcc_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
);
#else
static inline void boot_flash_dev_sdcc_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
}
#endif /* FEATURE_BOOT_SDCC_BOOT */

#endif /* BOOT_FLASH_DEV_SDCC_H */

