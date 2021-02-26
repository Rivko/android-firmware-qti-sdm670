/*=============================================================================
                           
                Boot SD and MMC Flash Translation Module

GENERAL DESCRIPTION
  This header file contains the implementation for the SDCC/eMMC/MicroSD
  flash translation module/layer.

Copyright 2009-2012, 2014, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/23/17   ds      coldplug changes
03/03/17   ds	   remove clobber prot
08/20/14   kedara  Fix compilation warnings.
05/12/14   kedara  Include HALbootHWIO.h instead of msmhwioreg.h 
05/30/12   dh      Close raw partition in boot_flash_trans_sdcc_factory_close
09/03/11   dh      Remove boot_flash_trans_sdcc_seg_read, boot_flash_trans_sdcc_get_logical_addr
                   boot_flash_trans_sdcc_set_copy_callback,boot_flash_trans_sdcc_read_WM7
08/18/10   aus     Fixed compiler warnings
12/29/09   vtw     File clean up and added FAT partition support.
08/31/09   VTW     Created.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_trans_sdcc.h"
#include "boot_flash_dev_sdcc_if.h"
#include "boot_sdcc.h"
#include "boot_util.h"
#include "HALbootHWIO.h"

#ifndef BOOT_FLASH_TRANS_MAX_INSTANCES
#define BOOT_FLASH_TRANS_MAX_INSTANCES (1)
#endif

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* Definition of private flash translation module data structure */
typedef struct boot_flash_trans_sdcc_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_trans_if_type  pub_if;
  
  /* The coldplug id or efs file handle */
  uint32 partition_id;  
  /* Partition type  */                                          
  uint32 partition_type;        
  
  /* Variables for maintaining a callback function */
  boot_flash_trans_copy_callback_func_type copy_callback_func;
  void *copy_callback_data;
  
} boot_flash_trans_sdcc_type;


/* The boot translation factory private data structure */
typedef struct boot_flash_trans_sdcc_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_flash_trans_factory_if_type pub_if;

  /* Array of the boot translation private data structures allocated
   * by factory */
  boot_flash_trans_sdcc_type trans_priv[BOOT_FLASH_TRANS_MAX_INSTANCES];

  /* Array indicating which elements of the boot translation private data
   * array are in use */
  boolean trans_priv_inuse[BOOT_FLASH_TRANS_MAX_INSTANCES];

} boot_flash_trans_not_factory_private_type;


/* The NOT translation factory private data */
static boot_flash_trans_not_factory_private_type fact_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

**  Function :  boot_flash_trans_sdcc_read

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to copy a region of data from a logical byte address to 
*   the destination address.  Given the logical range to copy, this function
*   ensures that the copy does not overwrite a protected bootloader memory
*   region, then invokes the device-specific SDCC copy routine.
* 
* @param[in] trans_if Pointer to the translation interface
* @param[in] dest_addr Destination address
* @param[in] byte_offset Logical source byte address
* @param[in] size Size to copy in bytes
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
static boolean boot_flash_trans_sdcc_read
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size                    /* Size to copy in bytes */
)
{
  boolean status = FALSE;
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_sdcc_type * const trans_priv =
    (boot_flash_trans_sdcc_type*)trans_if;
 
  BFA_SDCC_VERIFY( trans_if != NULL );

  /* Load from flash if size is >0 */
  if ( size>0 )
  {
	/* Call the boot sdcc read function. */
	status = boot_flash_dev_sdcc_read_sdcc ( dest_addr, 
	                                         byte_offset, 
									         size, 
									         trans_priv->partition_id,
									         trans_priv->partition_type);  	
  }
  else
  {
	/* Nothing to do */
    status = TRUE;
  }
  
  /* If the callback function has been set, call the callback function
     with the location and size of the area of RAM that was just read
     from flash */
  if ( trans_priv->copy_callback_func != NULL )
  {
    trans_priv->copy_callback_func( trans_priv->copy_callback_data, 
                                    dest_addr, size );
  }

  return status;
  
} /* boot_flash_trans_sdcc_read() */


/*=========================================================================

**  Function :  boot_flash_trans_sdcc_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function to initialize:
*   1. An instance of the boot SDCC flash translation module.
*   2. Function vtable pointers.
*  
* @param[in] trans_priv Pointer to the private SDCC flash translation structure
* @param[in] fhandle coldplug device ID or EFS handle
* @param[in] parti_type Partition type: RAW or FAT
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
static void boot_flash_trans_sdcc_init
(
  boot_flash_trans_sdcc_type *trans_priv,  /* Pointer to the private SDCC flash translation structure */
  uint32 fhandle,                          /* coldplug device ID or EFS handle */
  uint32 parti_type                        /* Partition type: RAW or FAT */
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_SDCC_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_IF_VERSION,
                         BOOT_FLASH_TRANS_SDCC_VERSION )
  };

  static boot_flash_trans_vtbl_type vtbl =
  {
    {
      boot_flash_trans_sdcc_read,                  /* read */
      NULL,                                        /* sloppy_read */
      NULL                                         /* get_logical_addr */
    },
    NULL,                                        /* set_copy_callback */
    NULL,                                        /* read_appsimage */
  };


  BL_VERIFY( trans_priv != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  /* Setup the module version structure and virtual function table */
  trans_priv->pub_if.vsn = &vsn;
  trans_priv->pub_if.vtbl = &vtbl;

  /* SD boot don't use the base addresss. We store the coldplug device pointer instead.  */
  trans_priv->partition_id = fhandle;
  trans_priv->partition_type = parti_type;

  /* Initialize the copy callback function pointer and data */
  trans_priv->copy_callback_func = NULL;
  trans_priv->copy_callback_data = NULL;    

} /* boot_flash_trans_sdcc_init() */


/*=========================================================================

**  Function :  boot_flash_trans_sdcc_factory_open

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to open an instance of the boot translation module.
* 
* @param[in] f_handle coldplug ID orm EFS file hanlet
* @param[in] parti_type Partition type
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
static boot_flash_trans_if_type *boot_flash_trans_sdcc_factory_open
(
  uint32 f_handle,                   /* coldplug ID orm EFS file hanlet. */
  uint32 parti_type                  /* Partition type */
)
{
  uint16 i;
  boot_flash_trans_sdcc_type *trans_priv = NULL;  /* Pointer to the private data structure */
                                                                                      
  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.trans_priv_inuse[i] )
    {
      trans_priv = &fact_priv.trans_priv[i];
      fact_priv.trans_priv_inuse[i] = TRUE;
      break;
    }
  }
  BL_VERIFY( trans_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );

  /* Initialize the SDCC translation interface. */
  boot_flash_trans_sdcc_init( trans_priv, f_handle, parti_type );

  return &trans_priv->pub_if;
  
} /* boot_flash_trans_sdcc_factory_open() */

/*=========================================================================
**  Function :  boot_flash_trans_sdcc_factory_close

** ==========================================================================
*/
/*!
* 
* @brief
*   Requests the factory to close an instance of the boot translation module.
* 
* @param[in] p_trans_if Pointer to the pointer of the flash translation module
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
static void boot_flash_trans_sdcc_factory_close
(
  boot_flash_trans_if_type **p_trans_if
)
{
  uint16 i;
  boot_flash_trans_sdcc_type *sdcc_trans_priv = NULL;  /* Pointer to the SDCC private data structure */

  BL_VERIFY( p_trans_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( *p_trans_if == (boot_flash_trans_if_type*) &fact_priv.trans_priv[i] )
    {
      BL_VERIFY( fact_priv.trans_priv_inuse[i], BL_ERR_RESOURCE_NOT_AVAILABLE|BL_ERROR_GROUP_BOOT );
      sdcc_trans_priv = &fact_priv.trans_priv[i];
     
      /* if partition type is FAT we also close the EFS file  */ 
      if (sdcc_trans_priv->partition_type == FAT_PARTI)
      {
        boot_flash_dev_sdcc_close_file(sdcc_trans_priv->partition_id);
      }

      /* if partition type is RAW we also close the coldplug handler  */ 
      if (sdcc_trans_priv->partition_type == RAW_PARTI)
      {
        boot_flash_dev_sdcc_close_file_by_image_type(GEN_IMG);
      }
    
      fact_priv.trans_priv_inuse[i] = FALSE;
      *p_trans_if = NULL; 
      return;
    }
  }

  /* If we have gotten here then the specified translation interface cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_SDCC_TRANS_INTERFACE_NOT_FOUND|BL_ERROR_GROUP_BOOT );
  
} /* boot_flash_trans_sdcc_factory_close() */

/*=========================================================================

**  Function :  boot_flash_trans_sdcc_factory_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initializes the SDCC boot translation factory.
*   The function initializes the flash translation factory functions.
* 
* @par Dependencies
*   None
*   
* @retval
*   Pointer to the SDCC translation factory interface
* 
* @par Side Effects
*   None
* 
*/
boot_flash_trans_factory_if_type* boot_flash_trans_sdcc_factory_init( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_FACTORY_SDCC_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_FACTORY_IF_VERSION,
                         BOOT_FLASH_TRANS_FACTORY_SDCC_VERSION )
  };
  static boot_flash_trans_factory_vtbl_type vtbl = 
  {
    boot_flash_trans_sdcc_factory_open,
    boot_flash_trans_sdcc_factory_close
  };
  static boolean init_done = FALSE;


  /* Only initialize the factory module once, if init() is called subsequently
   * it will return a pointer to the live module interface */
  if ( !init_done )
  {
    /* Setup the module version structure and virtual function table */
    fact_priv.pub_if.vsn = &vsn;
    fact_priv.pub_if.vtbl = &vtbl;

    /* Zero out the inuse marker for the private ELF data structure */
    qmemset( fact_priv.trans_priv_inuse, 0, sizeof(fact_priv.trans_priv_inuse) );

    init_done = TRUE;
  }

  /* Return pointer to the one SDCC translation factory interface */
  return &fact_priv.pub_if;
  
} /* boot_flash_trans_sdcc_factory_init() */

