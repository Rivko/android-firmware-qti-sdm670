#ifndef BOOT_FLASH_DEV_SDCC_IF_H
#define BOOT_FLASH_DEV_SDCC_IF_H

/*=============================================================================

                         Boot MMC/SD Singleton

GENERAL DESCRIPTION
  Singleton for the eMMC boot device interface.

Copyright 2009-2010, 2012, 2014-2015, 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/17   kpa     removed dev_sdcc_load_mibib_header_with_cookie
10/05/15   ck      Corrected copyright
05/07/14   kedara  Make inlined api's static.
05/30/12   dh      Add boot_flash_dev_sdcc_close_file_by_image_type
01/07/10   VTW     Updated to add  FAT partition support.
08/31/09   VTW     Updated to support eMMC boot.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash_dev_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_SDCC_IF_VERSION 1


/* Forward declarations */
struct boot_flash_dev_sdcc_if_type;


/* Definition of the SDCC boot device module virtual table structure */
typedef struct boot_flash_dev_sdcc_vtbl_type
{
  /*----------------------------------- 
   * Generic boot flash device interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Provides 
   * interface to generic boot flash device flash routines.
   * MMC-specific flash device routines and variables follow
   * the generic interface and extend the interface.
   */
  boot_flash_dev_vtbl_type parent;

  boolean (*read_sdcc)
  (
    void   *ram_addr,                       /* Destination address in RAM */
    uint32 src_offset_from_partition_start, /* source address in bytes from 
                                                                                    partition start */
    uint32 bytes_to_read,                   /* number of bytes to read */
    uint32 parti_id,                        /* hotplug ID or file handle */
    uint32 parti_type                       /* partition type  */
  );
 
  boolean (*read_segment)
  (
    void   *ram_addr,                       /* Destination address in RAM */
    uint32 src_offset_from_partition_start, /* source address in bytes from 
                                                                                  partition start */
    uint32 bytes_to_read,                   /* number of bytes to read */
    uint32 parti_id,                        /* hotplug ID or file handle */
    uint32 parti_type                       /* partition type  */
  );

  boolean (*load_ramfs)
  (
    void   *ram_addr,                       /* Destination address in RAM */
    uint32 src_offset_from_partition_start, /* source address in bytes from 
                                                                                partition start */
    uint32 bytes_to_read                     /* number of bytes to read */
  );

} boot_flash_dev_sdcc_vtbl_type;


/* Definition of public interface to SDCC flash device module */
typedef struct boot_flash_dev_sdcc_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_dev_sdcc_vtbl_type *vtbl;

} boot_flash_dev_sdcc_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/* Must declare the singleton before we can reference the get_singleton()
 * functions in the below inline functions */
BOOT_SINGLETON_DECLARE(boot_flash_dev_sdcc_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_cast_parent_if

** ==========================================================================
*/
/*!
* 
*  @brief
*   Takes a pointer to the boot device interface and casts to the parent
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
static __inline boot_flash_dev_if_type* boot_flash_dev_sdcc_cast_parent_if
(
  boot_flash_dev_sdcc_if_type *dev_if
)
{
  return (boot_flash_dev_if_type*) dev_if;
} /* boot_flash_dev_sdcc_cast_parent_if() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_open_image

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
static __inline boot_flash_trans_if_type* boot_flash_dev_sdcc_open_image
(
  image_type image_id                     /* Image type to initialize */
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.open_image( image_id );
} /* boot_flash_dev_sdcc_open_image() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_open_partition

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
static __inline boot_flash_trans_if_type* boot_flash_dev_sdcc_open_partition
(
  const char *parti_name                     /* Partition to open */
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.open_partition( parti_name );
} /* boot_flash_dev_sdcc_open_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_close_image

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
static __inline void boot_flash_dev_sdcc_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.close_image( p_trans_if );
} /* boot_flash_dev_sdcc_close_image() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_close_partition

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
static __inline void boot_flash_dev_sdcc_close_partition
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.close_partition( p_trans_if );
} /* boot_flash_dev_sdcc_close_partition() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_load_mibib_header

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
static __inline boolean boot_flash_dev_sdcc_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr   /* Pointer to image header */
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.load_mibib_header( image_id, header_ptr );
} /* boot_flash_dev_sdcc_load_mibib_header() */


/*=========================================================================

**  Function :  boot_flash_dev_sdcc_share_data

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
static __inline boolean boot_flash_dev_sdcc_share_data
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.share_data( bl_shared_data );
} /* boot_flash_dev_sdcc_share_data() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_setup_mpu

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
static __inline boolean boot_flash_dev_sdcc_setup_mpu
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.setup_mpu( bl_shared_data );
} /* boot_flash_dev_sdcc_setup_mpu() */

/*===========================================================================

 **  Function : pointer  boot_flash_dev_read_sdcc

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads data from the SDCC device and copies it to the 
*   destination address in RAM.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the read was successful, otherwise FALSE.
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_sdcc_read_sdcc
(
  void   *ram_addr,                        /* Destination address in RAM */
  uint32  flash_addr,                      /* Logical flash source address */
  uint32  size,                            /* Size to copy in bytes */
  uint32  parti_id,                        /* hotplug ID or EFS file handle */
  uint32  part_type                        /* raw or FAT partition */
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->read_sdcc( ram_addr, 
  	                                                              flash_addr, 
  	                                                              size, 
  	                                                              parti_id,
  	                                                              part_type);

} /* boot_flash_dev_sdcc_read_sdcc() */


/*===========================================================================

 **  Function : pointer  boot_flash_dev_read_sdcc_read_segment

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads data from the SDCC device and copies it to the 
*   destination address in RAM.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the read was successful, otherwise FALSE.
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_dev_sdcc_read_segment
(
  void   *ram_addr,                        /* Destination address in RAM */
  uint32  flash_addr,                      /* Logical flash source address */
  uint32  size,                            /* Size to copy in bytes */
  uint32  parti_id,                        /* hotplug ID or EFS file handle */
  uint32  part_type                        /* raw or FAT partition */
)
{
  return boot_flash_dev_sdcc_if_get_singleton()->vtbl->read_segment( ram_addr, 
  	                                                                 flash_addr, 
  	                                                                 size, 
  	                                                                 parti_id,
  	                                                                 part_type );

} /* boot_flash_dev_sdcc_read_segment() */

/*=========================================================================

**  Function :  boot_flash_dev_sdcc_close_file

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an EFS fille via a valid file handle.
* @par Dependencies
*   EFS APIs
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static __inline void boot_flash_dev_sdcc_close_file
(
  int16 file_handle
)
{

  boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.close_file_by_handle( file_handle );

}


/*=========================================================================

**  Function :  boot_flash_dev_sdcc_close_file_by_image_type

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an raw partition via a partition id
* @par Dependencies
*   hotplug APIs
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static __inline void boot_flash_dev_sdcc_close_file_by_image_type
(
  image_type image_id
)
{
  boot_flash_dev_sdcc_if_get_singleton()->vtbl->parent.close_file_by_image_type( image_id );
}



#endif /* BOOT_FLASH_DEV_SDCC_IF_H */

