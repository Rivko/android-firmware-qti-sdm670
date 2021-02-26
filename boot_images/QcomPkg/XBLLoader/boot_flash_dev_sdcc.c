/*=============================================================================

                        Boot Loader eMMC Flash Device Module

GENERAL DESCRIPTION
  Contains the implementation for the boot MMC flash device module.

Copyright 2009-2011, 2014, 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/12/17   ds      coldplug changes
05/08/17   kpa     Removed dev_sdcc_load_mibib_header_with_cookie
09/03/14   ck      Refactored to match new pbl_sbl_shared header file.
08/20/14   kedara  Fix compilation warnings.
09/15/11   dh      Remove unused efs functions since we don't boot from FAT
09/08/11   dh      Switch all calls to sdcc/hotplug/efs driver to boot extern interface
09/01/11   dh      Remove boot_api.h boot_mc.h boot_util.h from include headers
03/04/11   dxiang  Support loading of general images with custom partition types
11/03/10   dh      Added boot_set_sdcc_boot_device_info to optimize sdcc init time
08/06/10   aus     Clean up secboot 3.0
08/05/10   kpa     Added definition for struct efs_file.
07/22/10   aus     Added fixes for buffer overflow and handling various hotplug read cases 
06/28/10   plc     Only support booting from EMBEDDED regardless of fastboot 
                   method to support booting from external SDCC. 
05/26/10   plc     Refactoring for Secboot 3.0
04/22/10   plc     Add in function to access boot-source information, and
                   open hotplug device handle accordingly-internal vs. external
				   Also remove check of FAT partition for image if RAW
				   partition exists in partition table.
04/14/10   plc     Add in support to load from external SD or eMMC, Mutually 
                   exclusive now, (eMMC must be erased to load from SD)
04/07/10   plc     Add in support to load from FAT or RAW, from mainline
03/29/10   plc     Remove extraneous hotplug_init() call before efs_init, was 
                   needed for RUMI build, but no longer needed.
03/09/10   plc     Add support for new hotplug API, opening handle by type, not
                   friendly name
03/09/10   plc     Add RPMSBL to header definition so relatve enum stays 
                   consistent across files
01/21/10   aus     Fixed ADSP loading issue for eMMC boot 
12/29/09   vtw     Code cleanup and added FAT partition support.
12/07/09   msm     Added function for write protection
11/19/09   msm     Code for new hotplug 
08/31/09   VTW     Created.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_sdcc.h"
#include "boot_flash_trans_if.h"
#include "boot_error_if.h"
#include "boot_sbl_if.h"
#include "boot_sdcc.h"
#include BOOT_PBL_H
#include "boot_extern_coldplug_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Definition of private boot MMC device data structure */
typedef struct boot_flash_dev_sdcc_type
{
  /*-----------------------------------
   * Public module interface
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_dev_sdcc_if_type pub_if;

  /* Pointer to the boot translation layer interface */
  boot_flash_trans_factory_if_type *trans_fact_if;

} boot_flash_dev_sdcc_type;


/* The SDCC private data structure */
static boot_flash_dev_sdcc_type sdcc_dev_priv;

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

**  Function :  dev_sdcc_open_image

** ==========================================================================
*/
/*!
* 
* @brief
*   This function opens an image on either FAT or RAW partition. 
*   Depending the image type the function calls the coldplug or EFS APIs.
* 
* @param[in] image_id Image type to open
*
* @par Dependencies
*   Coldplug and EFS APIs.
* 
* @retval
*   Pointer to the boot translation layer public interface.
* 
* @par Side Effects
*   None
* 
*/
static boot_flash_trans_if_type* dev_sdcc_open_image
(
  image_type image_id                  /* Image type to open */
)
{
  int32 handle = INVALID_FILE_HANDLE;
  uint16 parti_type = RAW_PARTI; 
  	
  /* First verify the input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_IMGID_NOT_FOUND|BL_ERROR_GROUP_BOOT );

  /* Check the RAW partition first */
  handle = boot_get_coldplug_dev_handle(image_id);
   
  /* Initialze the SDCC flash translation */     
  if (handle == INVALID_FILE_HANDLE)
  {
    return NULL;
  }
  else
  {
    /* Pass in coldplug parttion id or efs file handle and 
       partition type. To use later for reading from FAT or RAW partition.  */
	
    return boot_flash_trans_factory_open( sdcc_dev_priv.trans_fact_if, 
                                          handle, parti_type);                                                                         
  }

} /* dev_sdcc_open_image() */

/*=========================================================================

**  Function :  dev_sdcc_open_partition

** ==========================================================================
*/
/*!
* 
* @brief
*   This is just a stub function. Reserve when the name of the custom AMSS partition used by the customer.
*   If the customer doesn't use a custom name, this function returns NULL.
* 
* @param[in] parti_name partition to open
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
static boot_flash_trans_if_type* dev_sdcc_open_partition
(
  const char *parti_name                  /* partition to open */
)
{
  return NULL;
} /* dev_sdcc_open_partition() */


/*=========================================================================

**  Function :  dev_sdcc_close_file_by_image_type

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an open file.
*   Note that: The function does nothing if the file is not opened (Backward compatible with NAND boot).
* 
* @param[in] image_id Image type to close
*
* @par Dependencies
*   The file must be opened. 
* 
* @retval
*   None
* 
* @par Side Effects
*   Any attempt to access the file after calling this function will result in an error condition. 
* 
*/
static void dev_sdcc_close_file_by_image_type
(
  image_type image_id                  /* Image type to close */
)
{
  int status = EFS_SUCCESS;
  
  /* First verify the input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_IMGID_NOT_FOUND|BL_ERROR_GROUP_BOOT );  
  
  /* Close coldplug */
  status = boot_coldplug_close_device_by_image_type ( image_id );
  BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL|BL_ERROR_GROUP_BOOT ); 

} /* dev_sdcc_close_file_by_image_type() */

/*=========================================================================

**  Function :  dev_sdcc_close_file_by_handle

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an open file via a file handle.
*   Note that: The function does nothing if the file is not opened (Backward compatible with NAND boot).
* 
* @param[in] file_handle File object handle to close
*
* @par Dependencies
*   The file must be opened. 
* 
* @retval
*   None
* 
* @par Side Effects
*   Any attempt to access the file after calling this function will result in an error condition. 
* 
*/
static void dev_sdcc_close_file_by_handle
(
  int16 file_handle                  /* File object handle to close */
)
{
  int status = EFS_SUCCESS;
  
  /* First verify the file handle is valid. */
  BL_VERIFY( file_handle != INVALID_FILE_HANDLE, BL_ERR_EFS_INVALID_HANDLE|BL_ERROR_GROUP_BOOT );  
  
  /* Close coldplug */
  status = boot_coldplug_close_device_by_handle ( file_handle );
	BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL|BL_ERROR_GROUP_BOOT ); 
} /* dev_sdcc_close_file_by_handle() */

/*=========================================================================

**  Function :  dev_sdcc_close_image

** ==========================================================================
*/
/*!
* 
* @brief
*   Close an instance of the boot translation layer.
*   This results in closing a flash translation if connection and
*   performs clean-up.
*   If the partition is FAT, this function also closes the EFS file in the partition.
* 
* @param[in] p_trans_if Pointer to the pointer of the flash translation interface
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
static void dev_sdcc_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  
  boot_flash_trans_factory_close( sdcc_dev_priv.trans_fact_if, p_trans_if );
  
} /* dev_sdcc_close_image() */

/*===========================================================================

**  Function :  dev_sdcc_load_mibib_header

** ==========================================================================
*/
/*!
* 
* @brief
*   This is a stub function.
*
* @param[in] image_id Image type
* @param[in] image_header_ptr Pointer to image header
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
static boolean dev_sdcc_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *image_header_ptr  /* Pointer to image header */
)
{
  /* SB3.0 SDCC devices don't have image headers in the MIBIB */
  return FALSE;

} /* dev_sdcc_load_mibib_header() */

/*===========================================================================

**  Function :  dev_sdcc_read

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads a data block from the from SDCC/eMMC/MicroSD into
*   the RAM buffer.
*   Note that the LBA is 0.
* 
* @param[in] ram_addr Destination address in RAM
* @param[in] src_offset Source address in bytes from partition start                                  
* @param[in] bytes_to_read Number of bytes to read
* @param[in] parti_id coldplug ID or file handle depending on parti type
* @param[in] parti_type Partition type to read from
*
* @par Dependencies
*   coldplug APIs
* 
* @retval
*   TRUE on success FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean dev_sdcc_read
(
  void   *ram_addr,          /* Destination address in RAM */
  uint32 src_offset,         /* source address in bytes from partition start */                                  
  uint32 bytes_to_read,      /* number of bytes to read */
  uint32 parti_id,           /* coldplug ID or file handle depending on parti type */
  uint32 parti_type          /* partition type to read from */
)
{
  boolean ret_val = FALSE;
  
  switch (parti_type)
  {
    case RAW_PARTI: 
      ret_val = dev_sdcc_read_bytes(ram_addr, src_offset, bytes_to_read, parti_id);
      break;

    default:
      /* Only boot from raw partition. */
      BL_ERR_FATAL( BL_ERR_SDC_INVALID_PARTI|BL_ERROR_GROUP_BOOT );
  }

  return ret_val;
  
} /* dev_sdcc_read */

/*===========================================================================

**  Function :  dev_sdcc_read_segment

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads a data block from the from SDCC/eMMC/MicroSD into
*   the RAM buffer.
*   The read starts at the src_offset of a sector.
* 
* @param[in] ram_addr Destination address in RAM
* @param[in] src_offset Source address in bytes from partition start                                  
* @param[in] bytes_to_read Number of bytes to read
* @param[in] parti_id coldplug ID or file handle depending on parti type
* @param[in] parti_type Partition type to read from
*
* @par Dependencies
*   coldplug APIs
* 
* @retval
*   TRUE on success FALSE otherwise
* 
* @par Side Effects
*   None
* 
*/
static boolean dev_sdcc_read_segment
(
  void   *ram_addr,          /* Destination address in RAM */
  uint32 src_offset,         /* source address in bytes from partition start */                                      
  uint32 bytes_to_read,      /* number of bytes to read */
  uint32 parti_id,           /*holtplug ID or file handle */
  uint32 parti_type          /* partition type  */
)
{
  boolean ret_val = FALSE;
  
  switch (parti_type)
  {
    case RAW_PARTI: 
      ret_val = dev_sdcc_read_bytes( ram_addr, src_offset, bytes_to_read, parti_id );
      break;

    default:
      /* Only boot from raw partition. */
      BL_ERR_FATAL( BL_ERR_SDC_INVALID_PARTI|BL_ERROR_GROUP_BOOT );
  }
  
  return ret_val;

}

/*===========================================================================
**  Function :  dev_sdcc_configure_partition_id 
** ==========================================================================
*/
/*!
* 
* @brief
*   This function takes in a byte pointer to the partition ID information and passes
*   it along to the boot coldplug API to configure the partition ID information for
*   general image loading from a custom partition. 
* 
* @param[in] partition_id - Byte pointer to partition ID information
*
* @par Dependencies
*   coldplug APIs
* 
* @retval
*   None
* 
*/
static void dev_sdcc_configure_partition_id(uint8 * partition_id)
{
  boot_coldplug_set_general_partition_id(partition_id);
}


/*=========================================================================

**  Function :  boot_set_sdcc_boot_device_info

** ==========================================================================
*/
/*!
* 
* @brief
* This function reuses the the sdcc device info passed by PBL to
* pre initilize sdcc device in order to save boot time.
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
void boot_set_sdcc_boot_device_info
(
  boot_sbl_if_shared_info_type  *bl_shared_data
)
{
    boot_flash_shared_dev_info_type *flash_shared_data = NULL;
    boot_flash_shared_sdc_dev_type *sdc_data;
    static sdcc_boot_device_info_type boot_pbl_sdcc_boot_device_info;

    BL_VERIFY(bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);
    
    /*if the sdcc device info is passed down by previous sbl then we 
      can just use it*/
    if(bl_shared_data->sdcc_boot_device_info != NULL)
    {
      boot_sdcc_boot_set_device_info((sdcc_boot_device_info_type *)
                                      bl_shared_data->sdcc_boot_device_info);
    }
    /*other wise we need to set the device info according to PBL data*/
    else
    {
      flash_shared_data = boot_pbl_get_flash_shared_data();
      BL_VERIFY(flash_shared_data != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT);

      if (flash_shared_data->ctrl_type == SDC_CTRL)
      {
        sdc_data = &flash_shared_data->dev_info.sdc;

        /* Update relative card address */
        boot_pbl_sdcc_boot_device_info.rca = sdc_data->rca;

        /* Update the card type */
        switch(sdc_data->card_type)
        {
          case SDC_CARD_SD:
            boot_pbl_sdcc_boot_device_info.card_type = SDCC_CARD_SD;
            break;
            
          case SDC_CARD_MMC:
            boot_pbl_sdcc_boot_device_info.card_type = SDCC_CARD_MMC;
            break;
            
          case SDC_CARD_SDHC:
            boot_pbl_sdcc_boot_device_info.card_type = SDCC_CARD_SDHC;
            break;
            
          case SDC_CARD_MMCHC:
            boot_pbl_sdcc_boot_device_info.card_type = SDCC_CARD_MMCHC;
            break;
            
          default:
            boot_pbl_sdcc_boot_device_info.card_type = SDCC_CARD_UNKNOWN;
           break;
        }

        /* Update the clock information */
        /* Need revisit */
        switch(sdc_data->mclk)
        {
          case PBL_CLOCK_SDCC_FREQ_19P2MHZ:
            boot_pbl_sdcc_boot_device_info.clock = SDCC_SD_DATA_TRANSFER_20_MHZ;
            break;
          default:
            break;
        }

        /* Update the chip select base */
        switch(sdc_data->port)
        {
          case SDC_PORT1:
            boot_pbl_sdcc_boot_device_info.driveno = SDCC_DRIVENO_0;
            break;
          case SDC_PORT2:
            boot_pbl_sdcc_boot_device_info.driveno = SDCC_DRIVENO_1;
            break;
          default:
            break;
        }

        /* Update the data width */
        switch(flash_shared_data->data_width)
        {
          case SDC_MMC_BUSWIDTH_1BIT:
            boot_pbl_sdcc_boot_device_info.bus_width = SDCC_BUSWIDTH_1BIT;
            break;
          case SDC_MMC_BUSWIDTH_4BIT:
            boot_pbl_sdcc_boot_device_info.bus_width = SDCC_BUSWIDTH_4BIT;
            break;
          case SDC_MMC_BUSWIDTH_8BIT:
            boot_pbl_sdcc_boot_device_info.bus_width = SDCC_BUSWIDTH_8BIT;
            break;
          default:
            break;
        }
     
        boot_pbl_sdcc_boot_device_info.is_raw_valid = 0;
     
        /*share this device info to next sbl*/
        bl_shared_data->sdcc_boot_device_info = &boot_pbl_sdcc_boot_device_info;
        
        /*pass retrieved sd/emmc initialization data to sd driver*/
        boot_sdcc_boot_set_device_info(&boot_pbl_sdcc_boot_device_info);
      }
    }
}/* boot_set_sdcc_boot_device_info */


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
void boot_flash_dev_sdcc_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_DEV_SDCC_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_DEV_SDCC_IF_VERSION,
                         BOOT_FLASH_DEV_SDCC_VERSION )
  };

  static boot_flash_dev_sdcc_vtbl_type vtbl =
  {
    {
      dev_sdcc_open_image,                    /*  (*open_image) */
      dev_sdcc_open_partition,                /* (*open_partition) */
      dev_sdcc_close_image,                   /* (*close_image) */
      dev_sdcc_close_image,                    /* (*close_partition) */
      dev_sdcc_load_mibib_header,             /* (*load_mibib_header) */
      boot_flash_dev_default_stub,            /* share_data */
      boot_flash_dev_default_stub,            /* setup_mpu */
      dev_sdcc_close_file_by_image_type,      /* close_file_by_image_type */
      dev_sdcc_close_file_by_handle,          /* close_file_by_handle */
      dev_sdcc_configure_partition_id         /* configure_target_image */
    },
     dev_sdcc_read,                          /* read_sdcc  */
     dev_sdcc_read_segment                   /* read_segment*/
  };

  /* Setup the module version structure and virtual function table */
  sdcc_dev_priv.pub_if.vsn = &vsn;
  sdcc_dev_priv.pub_if.vtbl = &vtbl;
  
  /*retrieve sd/eMMC initialization shared data passed from PBL and 
    pass that to SD driver to avoid re-initialization of SD/eMMC
    and save boot time*/
  boot_set_sdcc_boot_device_info(bl_shared_data->sbl_shared_data);
  
  /* Initialize coldplug */
  boot_coldplug_init();

  
  /* Register the singletons */
  boot_flash_dev_if_register_singleton( boot_flash_dev_sdcc_cast_parent_if( &sdcc_dev_priv.pub_if ) );
  boot_flash_dev_sdcc_if_register_singleton( &sdcc_dev_priv.pub_if );

  /* Finally, initialize the translation layer factory */
  sdcc_dev_priv.trans_fact_if = trans_factory_init();
  BL_VERIFY( sdcc_dev_priv.trans_fact_if != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

} /* boot_flash_dev_sdcc_init_and_register() */
