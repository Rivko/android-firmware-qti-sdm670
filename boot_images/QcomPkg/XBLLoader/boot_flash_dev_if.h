#ifndef BOOT_FLASH_DEV_IF_H
#define BOOT_FLASH_DEV_IF_H

/*=============================================================================
                           
                          Boot Device Singleton

GENERAL DESCRIPTION
  Singleton for the boot device interface.

Copyright 2007-2011, 2014-2015, 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/17   kpa     remove unused api load_mibib_header_with_cookie
10/05/15   ck      Corrected copyright
11/20/14   kedara  Update to to include MIheader.h
05/07/14   kedara  Make inlined api's static.
03/04/11   dxiang  Support loading of general images with custom partition types
08/06/10   aus     Clean-up
01/06/10   VTW     Added vtbl  function to close EFS file.
09/22/09   dhaval  Declare API to load header with cookie
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"
#include "boot_singleton.h"
#include "boot_flash_trans_if.h"
#include "MIheader.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_IF_VERSION 1


/* Maximum number of bytes for a partition ID */
#define PARTITION_ID_MAX_SIZE 20

/*--------------------------------------------------------------------------
  Data structure for the flash partition ID 
 --------------------------------------------------------------------------*/
typedef struct
{
  uint8 partition_id[PARTITION_ID_MAX_SIZE];
}flash_partition_id;

/* Forward declarations */
struct bl_shared_data_type;
struct boot_flash_dev_if_type;


/* Definition of the boot device module virtual table structure */
typedef struct boot_flash_dev_vtbl_type
{
/*=========================================================================

**  Function : open_image

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to initialize an instance of the boot translation module, using
*     information obtained from the underlying boot device.  The base address
*     and region size used to initialize the translation layer are obtained
*     from the boot device (e.g. partition table) based on the given image type.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     Pointer to the boot translation layer public interface.
* 
*   @par Side Effects
*     None
* 
*/
  boot_flash_trans_if_type * (*open_image)
  (
    image_type image_id                     /* Image type to open */
  );

/*=========================================================================

**  Function : open_partition

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to initialize an instance of the boot translation module, using
*     information obtained from the underlying boot device.  The base address
*     and region size used to initialize the translation layer are obtained
*     from the boot device (e.g. partition table) based on the given partition
*     name.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     Pointer to the boot translation layer public interface.
* 
*   @par Side Effects
*     None
* 
*/
  boot_flash_trans_if_type * (*open_partition)
  (
    const char *parti_name                  /* partition to open */
  );

/*=========================================================================

**  Function : close_image

** ==========================================================================
*/
/*!
* 
*   @brief
*     Close an instance of the boot translation layer.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
  void (*close_image)
  (
    boot_flash_trans_if_type **p_trans_if
  );

/*=========================================================================

**  Function : close_partition

** ==========================================================================
*/
/*!
* 
*   @brief
*     Close an instance of the boot translation layer.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
  void (*close_partition)
  (
    boot_flash_trans_if_type **p_trans_if
  );

/*=========================================================================

**  Function : load_mibib_header

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to load an image header from the MIBIB of the flash device
*     based on the given image type.  This function returns false
*     if the device does not have headers in the MIBIB area.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     TRUE if the operation was successful, FALSE otherwise
* 
*   @par Side Effects
*     None
* 
*/
  boolean (*load_mibib_header)
  (
    image_type image_id,
    mi_boot_image_header_type *header_ptr   /* Pointer to image header */
  );

/*=========================================================================

**  Function : share_data

** ==========================================================================
*/
/*!
* 
*   @brief
*     Exports data shared by the boot device.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     TRUE if the operation was successful, FALSE otherwise
* 
*   @par Side Effects
*     None
* 
*/
  boolean (*share_data)
  (
    struct bl_shared_data_type    *bl_shared_data
  );

/*=========================================================================

**  Function : setup_mpu

** ==========================================================================
*/
/*!
* 
*   @brief
*     Setup the MPU for the boot device to enable a trusted environment.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     TRUE if the operation was successful, FALSE otherwise
* 
*   @par Side Effects
*     None
* 
*/
  boolean (*setup_mpu)
  (
    struct bl_shared_data_type    *bl_shared_data
  );

/*=========================================================================

**  Function : close_file_by_image_type

** ==========================================================================
*/
/*!
* 
*   @brief
*     Close a file handle. 
* 
*   @par Dependencies
*     None
* 
*   @retval
*     None
* 
*   @par Side Effects
*     None
* 
*/
  void (*close_file_by_image_type)
  (
    image_type image_id
  );

  
/*=========================================================================
  
**  Function : close_file_by_handle

** ==========================================================================
*/
/*!
*   
* 	@brief
* 	  Close a file handle. 
*   
* 	@par Dependencies
* 	  None
*   
* 	@retval
* 	  None
*   
* 	@par Side Effects
* 	  None
*   
*/
 void (*close_file_by_handle)
 (
   int16 file_handle
 );

 /*=========================================================================
  
**  Function : configure_target_image

** ==========================================================================
*/
/*!
*   
* 	@brief
* 	  Performs the necessary configuration to prepare a target image to be loaded
*     from a custom partition using the information pointed to by partition_id
*   
* 	@par Dependencies
* 	  None
*   
* 	@retval
* 	  None
*   
* 	@par Side Effects
* 	  None
*   
*/
 void (*configure_target_image)
 (
   uint8 * partition_id
 );

} boot_flash_dev_vtbl_type;


/* Definition of public interface to generic boot device module */
typedef struct boot_flash_dev_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_dev_vtbl_type *vtbl;

} boot_flash_dev_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Must declare the singleton before we can reference the get_singleton()
 * functions in the below inline functions */
BOOT_SINGLETON_DECLARE(boot_flash_dev_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_dev_open_image

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
static __inline boot_flash_trans_if_type* boot_flash_dev_open_image
(
  image_type image_id                     /* Image type to open */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->open_image( image_id );
} /* boot_flash_dev_open_image() */

/*=========================================================================

**  Function :  boot_flash_dev_open_partition

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
static __inline boot_flash_trans_if_type* boot_flash_dev_open_partition
(
  const char *parti_name                     /* partition to open */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->open_partition( parti_name );
} /* boot_flash_dev_open_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_close_image

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
static __inline void boot_flash_dev_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_if_get_singleton()->vtbl->close_image( p_trans_if );
} /* boot_flash_dev_close_image() */

/*=========================================================================

**  Function :  boot_flash_dev_close_partition

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
static __inline void boot_flash_dev_close_partition
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_if_get_singleton()->vtbl->close_partition( p_trans_if );
} /* boot_flash_dev_close_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_load_mibib_header

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
static __inline boolean boot_flash_dev_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr   /* Pointer to image header */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->load_mibib_header( image_id, header_ptr );
} /* boot_flash_dev_load_mibib_header() */

/*=========================================================================

**  Function :  boot_flash_dev_share_data

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
static __inline boolean boot_flash_dev_share_data
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->share_data( bl_shared_data );
} /* boot_flash_dev_share_data() */

/*=========================================================================

**  Function :  boot_flash_dev_setup_mpu

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
static __inline boolean boot_flash_dev_setup_mpu
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->setup_mpu( bl_shared_data );
} /* boot_flash_dev_setup_mpu() */

/*=========================================================================

**  Function :  boot_flash_dev_default_stub

** ==========================================================================
*/
/*!
* 
* @brief
*   Default no-op stub for various boot device functionality.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_default_stub
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return TRUE;
} /* boot_flash_dev_default_stub() */


/*=========================================================================

**  Function :  boot_flash_dev_close_file

** ==========================================================================
*/
/*!
* 
* @brief
*   This function closes a file object handle via an image type.
* 
* @par Dependencies
*   File must be open with efs_open.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static __inline void boot_flash_dev_close_file
(
  image_type image_id                     /* Image type to close */
)
{
  boot_flash_dev_if_get_singleton()->vtbl->close_file_by_image_type( image_id );
} /* boot_flash_dev_close_file() */

/*=========================================================================
**  Function :  boot_flash_configure_target_image
** ==========================================================================
*/
/*!
* 
* @brief
*   Performs the necessary configuration to prepare a target image to be loaded
*   from a custom partition using the information pointed to by partition_id
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
static __inline void boot_flash_configure_target_image
(
  uint8 * partition_id                     /* Target image partition ID */
)
{
  boot_flash_dev_if_get_singleton()->vtbl->configure_target_image( partition_id );
} /* boot_flash_configure_target_image */




#endif /* BOOT_FLASH_DEV_IF_H */

