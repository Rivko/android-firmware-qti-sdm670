#ifndef BOOT_FLASH_TRANS_IF_H
#define BOOT_FLASH_TRANS_IF_H

/*===========================================================================

                 Boot Flash Translation Interface Header File

GENERAL DESCRIPTION
  This header file contains the definition of the public interface to the
  boot-specific flash translation module/layer.

Copyright 2007-2008, 2010, 2014, 2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/17   ds	   remove clobber prot
01/27/17   digants changes for whitelist based protection
06/03/14   ck      Added function calls for flash statistics
05/07/14   kedara  Make inlined api's static.
01/05/10   aus     Virtual function table entry for API to load WM 7 OS image.
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "flash_trans_if.h"
#include "boot_statistics.h"
#include "boot_whitelist_prot.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_TRANS_IF_VERSION 1

#define BOOT_FLASH_TRANS_FACTORY_IF_VERSION 1

/* Forward declaration */
struct boot_flash_trans_if_type;


/*=========================================================================

**  Function : boot_flash_trans_copy_callback_func_type

** ==========================================================================
*/
/*!
* 
* @brief
*   Function pointer typedef for a callback function that can be called
*   intermittently when the boot flash translation module is copying data
*   from flash; the callback_data pointer will be data that is associated
*   with an instance of the callback, ram_addr will point to the last data
*   read from flash and ram_size will indicate the size of the last data
*   read from flash.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
typedef void (*boot_flash_trans_copy_callback_func_type)
(
  void *callback_data,  /* Pointer to data associanted with callback instance */
  void *ram_addr,       /* Pointer to last data read from flash */
  uint32 ram_size       /* Size of last data read from flash */
);

/* Definition of the boot translation module virtual table structure */
typedef struct boot_flash_trans_vtbl_type
{
  /*----------------------------------- 
   * Generic flash module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Provides 
   * interface to generic flash routines.  Boot-specific flash routines
   * follow the generic interface and extend the interface.
   */
  flash_trans_vtbl_type parent;

/*=========================================================================

**  Function : set_copy_callback

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to set a callback function that will be called intermittently
*     when copying data from flash; the pointer to the last data read from flash
*     and the data size will be passed to the callback when it is called.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     TRUE if the operation succeeded, FALSE otherwise
* 
*   @par Side Effects
*     None
* 
*/
  boolean (*set_copy_callback)
  (
    struct boot_flash_trans_if_type *trans_if, /* Pointer to public interface */
    boot_flash_trans_copy_callback_func_type callback_func, /* Callback func */
    void *callback_data                  /* Data to pass to callback function */
  );

/*=========================================================================

**  Function : read_appsimage

** ==========================================================================
*/
/*!
* 
*   @brief
*     Function to copy a region of data from a logical byte address
*     to the destination address. This API contains read logic to
*     be used only for loading Apps OS Image. 
* 
*   @par Dependencies
*     None
*   
*   @retval
*     TRUE if the operation succeeded, FALSE otherwise
* 
*   @par Side Effects
*     This API should only be invoked in the context of loading Apps OS image.
* 
*/
  boolean (*read_appsimage)
  (
    struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                               interface */
    void   *dest_addr,                      /* Destination address */
    uint32  byte_offset,                    /* Logical source byte address */
    uint32  size                            /* Size to copy in bytes */
  ); 

} boot_flash_trans_vtbl_type;

/* Definition of public interface to boot translation module */
typedef struct boot_flash_trans_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_trans_vtbl_type *vtbl;

} boot_flash_trans_if_type;


/* Definition of the boot translation factory module virtual table structure */
typedef struct boot_flash_trans_factory_vtbl_type
{
/*=========================================================================

**  Function : open

** ==========================================================================
*/
/*!
* 
*   @brief
*     Requests the factory to open an instance of the boot translation module.
* 
*   @par Dependencies
*     None
* 
*   @retval
*     Pointer to the opened boot translation interface
* 
*   @par Side Effects
*     None
* 
*/
  boot_flash_trans_if_type* (*open)
  (
    uint32 region_base_offset,      /* Base offset of the flash region */
    uint32 region_size              /* Maximum size of the flash region */
  );

/*=========================================================================

**  Function : close

** ==========================================================================
*/
/*!
* 
*  @brief
*     Requests the factory to close an instance of the boot translation module.
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
  void (*close)
  (
    boot_flash_trans_if_type **p_trans_if
  );
} boot_flash_trans_factory_vtbl_type;


/* Definition of public interface to boot translation factory module */
typedef struct boot_flash_trans_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_trans_factory_vtbl_type *vtbl;

} boot_flash_trans_factory_if_type;


/* Typedef of factory intit routine */
typedef boot_flash_trans_factory_if_type* (*boot_flash_trans_factory_init_type)
(
  void
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

**  Function :  boot_flash_trans_cast_parent_if

** ==========================================================================
*/
/*!
* 
*  @brief
*   Takes a pointer to the boot translation interface and casts to the parent
*   interface type (generic translation interface).
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
static __inline flash_trans_if_type* boot_flash_trans_cast_parent_if
(
  boot_flash_trans_if_type *trans_if
)
{
  return (flash_trans_if_type*) trans_if;
} /* boot_flash_trans_cast_parent_if() */

/*=========================================================================

**  Function :  boot_flash_trans_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_trans_read
(
  struct  boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size,                           /* Size to copy in bytes */
  buffer_type bf_type                     /* Type of data buffer being loaded to */
)
{
  boolean result = FALSE;
  
  /* check the data being loaded against the active whitelist 
  for image or meta/hdr depending on the buffer_type argument passed to this function */
  
  if(!(boot_whitelist_address_range_check((uint64)(dest_addr),(uint64)(size),bf_type)))
  {
    return FALSE;
  }
  
  //BL_VERIFY(result,BL_ERR_IMG_SECURITY_FAIL|BL_ERROR_GROUP_SECURITY);
  
  /* Start flash statistic timer. */
  boot_statistics_flash_read_start();

  result =
    trans_if->vtbl->parent.read(boot_flash_trans_cast_parent_if(trans_if),
                                dest_addr,
                                byte_offset,
                                size);

  /* If flash read was successful then stop the timer and report bytes read. */
  if (result)
  {
    boot_statistics_flash_read_stop(size);
  }

  return result;

} /* boot_flash_trans_read() */

/*=========================================================================

**  Function :  boot_flash_trans_sloppy_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address to the
*   destination address, and to allow the copy operation to round the start
*   and end addresses of the copy region up or down to the nearest page
*   boundaries (for page or sector based devices) for efficiency.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
*  @par Side Effects
*   This function may adjust the start address of the copy down to the next
*   lowest page boundary (for NAND flash), and adjust the size of the copy
*   up to match the next highest page boundary (for NAND flash).  This means
*   that this function may copy data to RAM below the buffer pointer and
*   beyond the limit of the buffer as defined by the size parameter.  This
*   function should not be used to copy data into a fixed buffer, only to
*   load whole images from flash to RAM, where it is safe to copy whole
*   pages from flash to RAM (for NAND).
* 
*/
static __inline boolean boot_flash_trans_sloppy_read
(
  struct  boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  boolean result = FALSE;

  /* Start flash statistic timer. */
  boot_statistics_flash_read_start();

  result =
    trans_if->vtbl->parent.sloppy_read(boot_flash_trans_cast_parent_if(trans_if),
                                       dest_addr,
                                       byte_offset,
                                       size);

  /* If flash read was successful then stop the timer and report bytes read. */
  if (result)
  {
    boot_statistics_flash_read_stop(size);
  }
  
  return result;

} /* boot_flash_trans_sloppy_read() */

/*=========================================================================

**  Function :  boot_flash_trans_get_logical_addr

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to translate a device-dependent physical flash source address
*   to a logical byte address.
* 
* @par Dependencies
*   None
* 
* @retval
*   The logical byte address.
* 
* @par Side Effects
*   None
* 
*/
static __inline uint32 boot_flash_trans_get_logical_addr
(
  struct boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                            interface */
  uint32 image_source                    /* Device-dependent physical
                                            flash source address */
)
{
  return trans_if->vtbl->parent.get_logical_addr( boot_flash_trans_cast_parent_if( trans_if ),
                                                  image_source );
} /* boot_flash_trans_get_logical_addr() */

/*=========================================================================

**  Function :  boot_flash_trans_set_copy_callback

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to set a callback function that will be called intermittently
*   when copying data from flash; the pointer to the last data read from flash
*   and the data size will be passed to the callback when it is called.
* 
* @par Dependencies
*   None
* 
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static __inline boolean boot_flash_trans_set_copy_callback
(
  struct boot_flash_trans_if_type *trans_if, /* Pointer to public interface */
  boot_flash_trans_copy_callback_func_type callback_func, /* Callback func */
  void *callback_data                  /* Data to pass to callback function */
)
{
  return trans_if->vtbl->set_copy_callback( trans_if, callback_func, callback_data );
} /* boot_flash_trans_set_copy_callback() */


/*=========================================================================

**  Function :  boot_flash_trans_factory_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to open an instance of the boot translation module.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer to the opened boot translation interface
* 
* @par Side Effects
*   None
* 
*/
static __inline boot_flash_trans_if_type* boot_flash_trans_factory_open
(
  boot_flash_trans_factory_if_type *trans_fact_if,
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  return trans_fact_if->vtbl->open( region_base_offset,
                                    region_size );
} /* boot_flash_trans_factory_open() */

/*===========================================================================

**  Function :  boot_flash_trans_factory_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to close an instance of the boot translation module.
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
static __inline void boot_flash_trans_factory_close
(
  boot_flash_trans_factory_if_type *trans_fact_if,
  boot_flash_trans_if_type **p_trans_if
)
{
  trans_fact_if->vtbl->close( p_trans_if );
} /* boot_flash_trans_factory_close() */

/*===========================================================================

**  Function :  boot_flash_trans_read_wm7

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address
*   to the destination address. This API contains read logic to
*   be used only for loading Windows Mobile 7 OS Image.
* 
* @par Dependencies
*   None
*   
* @retval
*   TRUE if the operation succeeded, FALSE otherwise
* 
* @par Side Effects
*    This API should only be invoked in the context of loading Windows
*    Mobile 7 image.
* 
*/
static __inline boolean boot_flash_trans_read_wm7
(
  struct  boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->read_appsimage( boot_flash_trans_cast_parent_if( trans_if ),
                                      dest_addr, byte_offset, size );
} /* boot_flash_trans_read_wm7() */

#endif /* BOOT_FLASH_TRANS_IF_H */

