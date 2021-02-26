/*=============================================================================

                          Boot Flash Initialization

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the BOOT
  software to intialize the boot flash device module.

Copyright 2007, 2009-2010, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/05/15   ck      Corrected copyright
09/13/10   dh      Added boot logger
08/18/10   aus     Code Clean-up
05/26/10   plc     Initial port to secboot 3.0 common code  
12/28/09   ty      Initial port to BOOT
12/07/07   MJS     Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include "boot_flash_dev_if.h"
#include "boot_flash_target.h"
#include "boot_error_handler.h"
#include "boot_logger.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  boot_setup_flash_mpu

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls the setup_mpu function in the flash device
*   interface.
* 
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
void boot_setup_flash_mpu( bl_shared_data_type *bl_shared_data )
{
  boolean rc;

  BL_VERIFY( ( bl_shared_data != NULL ), BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  rc = boot_flash_dev_setup_mpu( bl_shared_data );
  BL_VERIFY( rc, BL_ERR_FLASH_DRIVER|BL_ERROR_GROUP_BOOT );

} /* boot_setup_flash_mpu() */

/*===========================================================================

**  Function :  boot_share_flash_data

** ==========================================================================
*/
/*!
* 
* @brief
*   Wrapper function that calls the share_data function in the flash device
*   interface.
*  
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
void boot_share_flash_data( bl_shared_data_type *bl_shared_data )
{
} /* boot_share_flash_data() */

/*===========================================================================

**  Function :  boot_flash_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the boot (flash) device.
* 
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
void boot_flash_init( bl_shared_data_type *bl_shared_data )
{
  /* Nothing to do here for now, just invoke the target-specific function */
  boot_log_message("boot_flash_init, Start");
  boot_log_start_timer();
  boot_flash_target_init( bl_shared_data );
  boot_log_stop_timer("boot_flash_init, Delta");
} /* boot_flash_init() */
