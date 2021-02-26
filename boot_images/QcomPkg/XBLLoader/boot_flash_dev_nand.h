#ifndef BOOT_FLASH_DEV_NAND_H
#define BOOT_FLASH_DEV_NAND_H

/*===========================================================================

                                 Boot Loader
                           NAND Flash Device Module
                                 Header File

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot NAND flash device module, and declarations for the public
  functions in the module.

Copyright 2010-2011, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   ck      Corrected copyright
05/07/14   kedara  Include boot_flash_dev_nand_if.h only in case of nand boot.
08/04/11   aus     Added support for storing partition table to SMEM
10/29/10   plc     Stub boot_flash_dev_nand_init_and_register with inline 
                   'do nothing' definition, if nand-boot feature isn't defined
09/21/10   plc     Initial port to SB3.0
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include BOOT_PBL_H
#include "miparti.h"

#include "boot_flash_dev_dal.h"

#ifdef FEATURE_BOOT_NAND_BOOT
#include "boot_flash_dev_nand_if.h"
#endif /*FEATURE_BOOT_NAND_BOOT*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_NAND_VERSION 2
#define BOOT_FLASH_DEV_NAND_NAME "boot_flash_dev_nand"


/*============================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

**  Function :  boot_flash_dev_nand_store_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*   Store partition table in SMEM
*   
* @par Dependencies
*   SMEM needs to be initialized 
*      
* @retval
*   None                                                               
* 
* @par Side Effects
*   None
* 
*/
#ifdef FEATURE_BOOT_NAND_BOOT
void boot_flash_dev_nand_store_partition_table
( 
  bl_shared_data_type *bl_shared_data 
);
#else
static inline void boot_flash_dev_nand_store_partition_table
( 
  bl_shared_data_type *bl_shared_data 
)
{
}
#endif

/*=========================================================================

**  Function :  boot_flash_dev_nand_init_and_register

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize the NAND flash device module.
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
#ifdef FEATURE_BOOT_NAND_BOOT
void boot_flash_dev_nand_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
);
#else
static inline void boot_flash_dev_nand_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
}
#endif

/*===========================================================================

**  Function :  boot_flash_dev_nand_load_partition_table

** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads the NAND flash partition table.
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
flash_partable_t boot_flash_dev_nand_load_partition_table
(
  struct bl_shared_data_type *bl_shared_data
);

#endif /* BOOT_FLASH_DEV_NAND_H */

