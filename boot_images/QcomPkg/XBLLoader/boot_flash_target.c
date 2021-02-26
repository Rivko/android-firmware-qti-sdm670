/*=============================================================================

                        Boot Flash Initialization

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the BOOT
  software to intialize the boot flash device module.

Copyright 2007-2011, 2013, 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
04/01/15   as      Update for SPI-NOR boot support..
02/03/15   wek     Force storage to UFS for device programmer.
07/11/13   jz      Updated for UFS boot support
06/27/11   aus     Added flashless boot support
11/10/10   aus     Removing compiler warnings
09/21/10   plc     Added support for NAND boot
08/04/10   aus     Clean-up secboot 3.0
05/26/10   plc     Initial port to secboot 3.0 common code
12/09/09   ty      Initial port to BOOT
11/18/09   as      Boot Optimization Changes
11/05/09   msm     Remove the linker error for non-SDCC builds.
09/22/09   dhaval  Added support for mmc flash device initialization
08/19/08   dhaval  Removed FEATURE_BOOT_GENERATE_UUID definition
06/13/08   MJS     Enable NAND MPU.
12/07/07   MJS     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include "boot_error_handler.h"
#include "boot_flash_dev_sdcc.h"
#include "boot_flash_trans_sdcc.h"
#include "boot_flash_dev_nand.h"
#include "boot_flash_trans_nand.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  boot_flash_target_init

** ==========================================================================
*/
/*!
*
* @brief
*   This function initializes the flash device module according to the boot
*   flash type.
*
* @param[in] bl_shared_data Pointer to the shared data structure
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
void boot_flash_target_init
(
  bl_shared_data_type *bl_shared_data
)
{
  boot_flash_type boot_device;
  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  /* Get the boot device type */
  boot_device = boot_pbl_get_flash_type();

  if (bl_shared_data->build_type == SBL_BUILD_DEVICEPROGRAMMER_DDR)
  {
    boot_device = UFS_FLASH;
  }

  /* Initialize the flash device module based on the boot flash type. */
  switch ( boot_device )
  {
    case NAND_FLASH:
	  /* Initialize the NAND flash device module. */
      boot_flash_dev_nand_init_and_register( boot_device,
                                            boot_flash_trans_nand_factory_init,
                                            bl_shared_data );
	  break;
    case SDC_FLASH:
    case MMC_FLASH:
    case UFS_FLASH:
    case SPI_FLASH:
      /* Initialize the MMC flash device module. */
      boot_flash_dev_sdcc_init_and_register( boot_device,
                                            boot_flash_trans_sdcc_factory_init,
                                            bl_shared_data );
    case NO_FLASH:
    case RESERVED_1_FLASH:
    default:
      break;
  }

  if(boot_device != NO_FLASH)
  {
     /* Lock the boot device singleton */
     boot_flash_dev_if_lock_singleton();
  }

} /* boot_flash_target_init() */

