#ifndef BOOT_FLASH_DEV_NAND_IF_H
#define BOOT_FLASH_DEV_NAND_IF_H

/*===========================================================================

                                  Boot Loader
                          NAND Flash Device Interface
                                  Header File

DESCRIPTION
  This header file contains the definition of the public interface to the
  boot-specific NAND flash device module.

Copyright 2010, 2014-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/17   kpa     removed boot_flash_dev_nand_load_mibib_header_with_cookie
10/05/15   ck      Corrected copyright
05/07/14   kedara  Make inlined api's static.
09/21/10   plc     Initial port to SB3.0

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_dev_if.h"
#include "boot_flash_dev_dal.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_NAND_IF_VERSION 1

/* Forward declarations */
struct boot_flash_dev_nand_if_type;


/* Definition of the NAND boot device module virtual table structure */
typedef struct boot_flash_dev_nand_vtbl_type
{
  /*----------------------------------- 
   * Generic boot device interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Provides 
   * interface to generic boot flash device flash routines.
   * NAND-specific flash device routines and variables follow
   * the generic interface and extend the interface.
   */
  boot_flash_dev_vtbl_type parent;

  /*===========================================================================

  **  Function : pointer  get_nand_if

** ==========================================================================
*/
/*!
* 
*   @brief
*     Returns a pointer to the NAND flash driver interface.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     Pointer to the NAND flash driver interface, NULL if an error occurred
* 
*   @par Side Effects
*     None
* 
  */
  struct fs_device_data * (*get_nand_if)
  (
    void
  );

} boot_flash_dev_nand_vtbl_type;


/* Definition of public interface to NAND boot device module */
typedef struct boot_flash_dev_nand_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_dev_nand_vtbl_type *vtbl;

} boot_flash_dev_nand_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Must declare the singleton before we can reference the get_singleton()
 * functions in the below inline functions */
BOOT_SINGLETON_DECLARE(boot_flash_dev_nand_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_dev_nand_cast_parent_if

** ==========================================================================
*/
/*!
* 
*  @brief
*   Takes a pointer to the NAND boot device interface and casts to the parent
*   interface type (boot device interface).
* 
* @par Dependencies
*   None
*   
* @retval
*   A pointer to a boot device interface
* 
* @par Side Effects
*   None
* 
*/
static __inline boot_flash_dev_if_type* boot_flash_dev_nand_cast_parent_if
(
  boot_flash_dev_nand_if_type *dev_if
)
{
  return (boot_flash_dev_if_type*) dev_if;
} /* boot_flash_dev_nand_cast_parent_if() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_open_image

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot translation module, using
*   information obtained from the underlying boot device.  The base address
*   and region size used to initialize the translation layer are obtained
*   from the boot device (e.g. partition table) based on the given image type.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the boot translation layer public interface.
* 
* @par Side Effects
*   None
* 
*/
static __inline boot_flash_trans_if_type* boot_flash_dev_nand_open_image
(
  image_type image_id                     /* Image type to open */
)
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->parent.open_image( image_id );
} /* boot_flash_dev_nand_open_image() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_open_partition

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize an instance of the boot translation module, using
*   information obtained from the underlying boot device.  The base address
*   and region size used to initialize the translation layer are obtained
*   from the boot device (e.g. partition table) based on the given partition
*   name.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the boot translation layer public interface.
* 
* @par Side Effects
*   None
* 
*/
static __inline boot_flash_trans_if_type* boot_flash_dev_nand_open_partition
(
  const char *parti_name                     /* Partition to open */
)
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->parent.open_partition( parti_name );
} /* boot_flash_dev_nand_open_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_close_image

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an instance of the boot translation layer.
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
static __inline void boot_flash_dev_nand_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_nand_if_get_singleton()->vtbl->parent.close_image( p_trans_if );
} /* boot_flash_dev_nand_close_image() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_close_partition

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an instance of the boot translation layer.
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
static __inline void boot_flash_dev_nand_close_partition
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_nand_if_get_singleton()->vtbl->parent.close_partition( p_trans_if );
} /* boot_flash_dev_nand_close_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_load_mibib_header

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to load an image header from the MIBIB of the flash device
*   based on the given image type.  This function returns false
*   if the device does not have headers in the MIBIB area.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation was successful, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_nand_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr   /* Pointer to image header */
)
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->parent.load_mibib_header( image_id, header_ptr );
} /* boot_flash_dev_nand_load_mibib_header() */


/*=========================================================================

**  Function :  boot_flash_dev_nand_share_data

** ==========================================================================
*/
/*!
* 
* @brief
*   Exports data shared by the boot device.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation was successful, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_nand_share_data
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->parent.share_data( bl_shared_data );
} /* boot_flash_dev_nand_share_data() */

/*=========================================================================

**  Function :  boot_flash_dev_nand_setup_mpu

** ==========================================================================
*/
/*!
* 
* @brief
*   Setup the MPU for the boot device to enable a trusted environment.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation was successful, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_nand_setup_mpu
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->parent.setup_mpu( bl_shared_data );
} /* boot_flash_dev_nand_setup_mpu() */

/*===========================================================================

**  Function :  boot_flash_dev_nand_get_nand_if

** ==========================================================================
*/
/*!
* 
* @brief
*   Returns a pointer to the NAND flash driver interface.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the NAND flash driver interface, NULL if an error occurred
* 
* @par Side Effects
*   None
* 
*/
static __inline struct fs_device_data* boot_flash_dev_nand_get_nand_if( void )
{
  return boot_flash_dev_nand_if_get_singleton()->vtbl->get_nand_if();
} /* boot_flash_dev_nand_get_nand_if() */


#endif /* BOOT_FLASH_DEV_NAND_IF_H */

