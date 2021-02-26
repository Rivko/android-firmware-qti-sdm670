#ifndef BOOT_FLASH_TARGET_H
#define BOOT_FLASH_TARGET_H

/*=============================================================================
                         
                        Boot Flash Initialization Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the BOOT
  software to intialize the boot flash device module.

Copyright 2007, 2009-2010, 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/05/15   ck      Corrected copyright
05/26/10   plc     Initial port to secboot 3.0 common code  
12/09/09   ty      Initial port to BOOT
12/07/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/
#include "boot_sbl_shared.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#ifdef BOOT_HAS_FLASH_BOOT
extern uint8 bl_page_buf[];
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
void boot_flash_target_init( bl_shared_data_type *bl_shared_data );

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
void boot_share_flash_data( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  boot_flash_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the boot (flash) device.
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
void boot_flash_init( bl_shared_data_type *bl_shared_data );

#endif /* BOOT_FLASH_TARGET_H */

