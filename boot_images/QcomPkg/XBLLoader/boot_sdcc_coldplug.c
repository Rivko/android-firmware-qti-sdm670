/*=============================================================================

                     Boot eMMC Coldplug Flash Device Module

GENERAL DESCRIPTION
  Contains the implementation for the boot MMC flash device module.

Copyright 2010-2015,2017,2018 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/06/18   c_vkuppa Adjust size of end_sector_bytes in dev_sdcc_read_bytes to 64-bit
08/04/17   aus     Added support for emmc
06/22/17   ds      use coldplug instead of hotplug
08/10/15   ck      Added logic to only poll the hotplug device once
06/03/15   vk      Fix partition size truncation
04/01/15   as      Add hotplug support for SPI Flash.
08/20/14   kedara  Fix compilation warnings.
05/07/14   kedara  Fix type mismatch error for image_partition_id last entry
11/01/13   plc     Update for Arm6 syntax ("attribute")
10/18/13   jz      Added call to poll hotplug device in the system
08/23/13   jz      Updated for UFS boot support, fixed hard-coded sector size
08/23/13   jz      Fixed get_hotplug_dev_handle to support boot from SD card
06/24/13   dh      Update to use run time sector size detection. Expand sector buffer
                   to 4K for UFS devices
06/18/13   wg      Fixed P1 KW warning array bounding
04/23/13   dh      Fix a bug in dev_sdcc_write_bytes that may write
                   one extra tailing sector
03/18/13   dh      Support unaligned write on dev_sdcc_write_bytes
07/20/12   dh      Add dev_sdcc_write_bytes(doesn't support unaligned write)
09/08/11   dh      Switch all calls to sdcc/hotplug/efs driver to boot extern interface
09/01/11   dh      Remove boot_api.h from include headers
03/21/11   dh      Support loading use GPT partition
03/04/11   dxiang  Support loading of general images with custom partition types
02/22/11   dxiang  Add additional entries for SSD partition loading
10/15/10   plc     Fix bug in dev_sdcc_read_bytes where breaking out of nested
                   while loop on failure, results in trailing bytes still being
                   loaded from SDCC, and failure being masked as false success
08/13/10   aus     Created.

=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/
#include "boot_error_if.h"
#include "boot_sdcc.h"
#include "boot_util.h"
#include "boot_extern_coldplug_interface.h"
#include "boot_sbl_shared.h"
#include "boot_flash_dev_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
/* max sector size we can have, which is 4k for UFS device */
#define MAX_SECTOR_SIZE 4096

#define COLDPLUG_PARTITION_ID_BOOT_QCSBL               0x45
/* As both OSBL and OEMSBL are mutually exclusive, thus have the unified
 * IDs 0x46 for both OSBL and OEMSBL */
#define COLDPLUG_PARTITION_ID_BOOT_OEMSBL              0x46
#define COLDPLUG_PARTITION_ID_BOOT_APPSSBL             0x47
#define COLDPLUG_PARTITION_ID_BOOT_APPS                0x48
#define COLDPLUG_PARTITION_ID_BOOT_MODEM               0x49
#define COLDPLUG_PARTITION_ID_BOOT_MODEM_FS1           0x4A
#define COLDPLUG_PARTITION_ID_BOOT_MODEM_FS2           0x4B
#define COLDPLUG_PARTITION_ID_BOOT_FOTA                0x4C
#define COLDPLUG_PARTITION_ID_BOOT_QCSBLHD_CFG_DATA    0x4D
#define COLDPLUG_PARTITION_ID_ADSP                     0x50

static boolean poll_done = FALSE;

/* To use aligned buffer for coldplug reads */
static ALIGN(32) uint8 dev_sdcc_buf_unalign[MAX_SECTOR_SIZE];

#define UINT16_MAX_VALUE 0xFFFF

/*---------------------------------------------------------------------------
  Image path on the MMC device.
---------------------------------------------------------------------------*/
/* The image_partition_id[] array holds a list of image specific partition IDs
 * which are used by the coldplug API to open partitions and get a handle of
 * the partition to perform various operations. */
flash_partition_id image_partition_id[] =
{
  { {COLDPLUG_PARTITION_ID_BOOT_QCSBLHD_CFG_DATA} },   /* DBL */
  { {COLDPLUG_PARTITION_ID_BOOT_QCSBL} },              /* QCSBL */
  { {COLDPLUG_PARTITION_ID_BOOT_OEMSBL} },             /* OSBL */
  { {COLDPLUG_PARTITION_ID_BOOT_APPSSBL} },            /* APPSBL */
  { {COLDPLUG_PARTITION_ID_BOOT_MODEM} },              /* AMSS */
  { {COLDPLUG_PARTITION_ID_BOOT_APPS} },               /* APPS */
  { {COLDPLUG_PARTITION_ID_BOOT_MODEM_FS1} },          /* MODEM_ST1 */
  { {COLDPLUG_PARTITION_ID_BOOT_MODEM_FS2} },          /* MODEM_ST2 */
  { {COLDPLUG_PARTITION_ID_ADSP} },                    /* ADSP */
  { {COLDPLUG_PARTITION_ID_BOOT_FOTA} },               /* FOTA */
  { {0x51} },                                         /* SBL2 */
  { {0x5d} },                                         /* SSD_KEYS */
  { {0x0} },                                          /* GEN_IMG */
  { {0x0} },                                          /* NULL: LAST_ENTRY */
};

/* The path_id[] is an array which is one to one mapped with the image_type
 * enumeration. This is used to index into image_partition_id[] in order
 * to associate specific partition ID information with each image */
const int path_id[] =
  { /* NONE_IMG */    -1,
    /* OEM_SBL_IMG */ -1,
    /* AMSS_IMG */     4,
    /* QCSBL_IMG */   -1,
    /* HASH_IMG */    -1,
    /* APPSBL_IMG */   9,
    /* APPS_IMG */     5,
    /* HOSTDL_IMG */  -1,
    /* DSP1_IMG */    -1,
    /* FSBL_IMG */    -1,
    /* DBL_IMG */      0,
    /* OSBL_IMG */     2,
    /* DSP2_IMG */    -1,
    /* EHOSTDL_IMG */ -1,
    /* NANDPRG_IMG */ -1,
    /* NORPRG_IMG */  -1,
    /* RAMFS1_IMG */   6,
    /* RAMFS2_IMG */   7,
    /* ADSP_Q5_IMG */  8,
    /* APPS_KERNEL_IMG */ -1,
    /* BACKUP_RAMFS_IMG */ -1,
    /* SBL1_IMG */    -1,
    /* SBL2_IMG */    10,
    /* RPM_IMG  */     3,
    /* SBL3_IMG */     1,
    /* TZ_IMG   */     2,
    /* SSD_KEYS_IMG */ 11,
    /* GEN_IMG */      12,
  };

/* IMAGE_NUMBERS represents the total number of valid partition IDs in image_partition_id[] */
static const uint32 IMAGE_NUMBERS = sizeof(image_partition_id)/sizeof(flash_partition_id) - 1;

/* PATH_ID_INDEX_MAX represents the maximum value we can use to index into path_id[] */
static const uint32 PATH_ID_INDEX_MAX = sizeof(path_id)/sizeof(int);

/* The structure holds images partition info */
static boot_coldplug_partition_t sbl_partitions[IMAGE_NUMBERS];

/*=============================================================================
                              FUNCTION DEFINITIONS
=============================================================================*/

/*=========================================================================
**  Function :  boot_coldplug_set_general_partition_id
** ==========================================================================
*/
/*!
*
* @brief
*   This function takes in a pointer to the partition ID information and makes a deep
*   copy of the information into the image_partition_id[] array corresponding
*   to a general image ID (GEN_IMG).
*
* @param[in] partition_id - Byte pointer to the partition id
*
* @par Dependencies
*   None
*
* @retval
*   None
*
*/
void boot_coldplug_set_general_partition_id(uint8 * partition_id)
{
  BL_VERIFY(partition_id != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  /* Copy data from the byte pointer into the image_partition_id[] array corresponding to GEN_IMG */
  qmemcpy( image_partition_id[path_id[(uint16)GEN_IMG]].partition_id,
           partition_id,
           sizeof(flash_partition_id));
}

/*=========================================================================
**  Function : boot_coldplug_check_partition_id_is_mbr
** ==========================================================================
*/
/*!
*
* @brief
*   This function takes in a pointer to the partition ID information and determines
*   whether or not it is of MBR type. With MBR partition tables, partition types are
*   limited to 1 byte. Other formats such as GUID Partition Table (GPT) may use up to
*   16 bytes for partition ID information.
*
* @param[in] partition_id - Byte pointer to the partition id
*
* @par Dependencies
*   None
*
* @retval
*   TRUE - Partition ID is of MBR type (Only holds 1 byte of information)
*   FALSE - Otherwise
*
*/
static boolean boot_coldplug_check_partition_id_is_mbr(uint8 * partition_id)
{
  boolean ret_val = TRUE;
  uint32 i = 0;
 
  BL_VERIFY(partition_id != NULL, BL_ERR_NULL_PTR_PASSED|BL_ERROR_GROUP_BOOT );

  /* We check for MBR type by ensuring only the first byte contains any data */
  for(i = 1; i < PARTITION_ID_MAX_SIZE; i++)
  {
    /* Starting from the second byte, check if byte is not 0x0 */
    if(partition_id[i] != 0x0){
      ret_val = FALSE;
      break;
    }
  }

  return ret_val;
}

/*=========================================================================

**  Function :  boot_get_coldplug_dev_handle

** ==========================================================================
*/
/*!
*
* @brief
*   This function performs the following:
*   1. Get the coldplug device handle of partition specific to the image id.
*   2. Verify the device size and block size.
*   3. Setup an instance of the boot translation module, pass in the start sector
*      of image partition.
*
* @param[in] image_id Image type to open
*
* @par Dependencies
*   Coldplug APIs
*
* @retval
*   Coldplug ID on success otherwise -1.
*
*/
int16 boot_get_coldplug_dev_handle
(
  image_type image_id                  /* Image type to open */
)
{
  int16 coldplug_id = INVALID_FILE_HANDLE;
  boot_flash_type fastboot_option = NO_FLASH;
  int16 status = -1;
  uint32 mbr_partition_id = 0;
  uint8 * partition_id_ptr = NULL;
  boolean partition_type_is_mbr = FALSE;
  int dev_loc = 1;
  enum coldplug_device_type dev_type = COLDPLUG_DEVICE_DEVNULL;

  /* First verify the input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_IMGID_NOT_FOUND|BL_ERROR_GROUP_BOOT );

  /* Get the flash type and appropriately assign the device location and device type */
  fastboot_option = boot_pbl_get_flash_type();

  if (fastboot_option == MMC_FLASH)
  {
    dev_loc = 1;
    dev_type = COLDPLUG_DEVICE_SD;
  }
  else if (fastboot_option == SDC_FLASH)
  {
    dev_loc = 0;
    dev_type = COLDPLUG_DEVICE_SD;
  }
  else if (fastboot_option == UFS_FLASH)
  {
    dev_loc = 1;
    dev_type = COLDPLUG_DEVICE_UFS;
  }
  else if (fastboot_option == SPI_FLASH)
  {
    dev_loc = 1;
    dev_type = COLDPLUG_DEVICE_SPI_NOR;
  }
  else if(fastboot_option == USB_FLASH)
  {
    return coldplug_id;
  }
  else
  {
    BL_ERR_FATAL(BL_ERROR_GROUP_STORAGE);
  }

  if (poll_done == FALSE)
  {
    status = boot_coldplug_open_boot_device(dev_type, dev_loc);
	BL_VERIFY( ((status == COLDPLUG_RESULT_SUCCESS) || (status == COLDPLUG_RESULT_NOT_SUPPORTED)), BL_ERR_HOTPLUG|BL_ERROR_GROUP_BOOT );  

	// todo : now we can read anything from boot-lun
    
    status = boot_coldplug_background_init_start(dev_type,dev_loc,0);
    BL_VERIFY( ((status == COLDPLUG_RESULT_SUCCESS) || (status == COLDPLUG_RESULT_NOT_SUPPORTED)), BL_ERR_HOTPLUG|BL_ERROR_GROUP_BOOT );
    
    // todo : cannot access device, so don't call any coldplug APIs
    // todo : this is the spot to initialize other things in parallel to UFS
    
    status = boot_coldplug_background_init_finish(dev_type,dev_loc,0);
    BL_VERIFY( ((status == COLDPLUG_RESULT_SUCCESS) || (status == COLDPLUG_RESULT_NOT_SUPPORTED)), BL_ERR_HOTPLUG|BL_ERROR_GROUP_BOOT );

    status = boot_coldplug_open_device(dev_type, dev_loc);
    BL_VERIFY( (status == COLDPLUG_RESULT_SUCCESS), BL_ERR_HOTPLUG|BL_ERROR_GROUP_BOOT );

    poll_done = TRUE;
  }
  
  /* Get the coldplug ID, used to index into image_partition_id[] */
  coldplug_id = path_id[(uint16)image_id];
  BL_VERIFY( coldplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT );
  BL_VERIFY( coldplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT );

  /* Assign pointer to partition ID information */
  partition_id_ptr = image_partition_id[coldplug_id].partition_id;

  /* Determine if partition ID information is of MBR type */
  partition_type_is_mbr = boot_coldplug_check_partition_id_is_mbr(partition_id_ptr);

  /* If partition ID is MBR, we assign the partition ID information to an integer
   * to be passed into the coldplug API. */
  if (partition_type_is_mbr == TRUE)
  {
    mbr_partition_id = *((uint32*)partition_id_ptr);
    sbl_partitions[coldplug_id].chdl =
        boot_coldplug_open_mbr_partition (dev_type, dev_loc, mbr_partition_id);
  }
  else
  {
    /* since we are not using MBR parition, we know partition_id_ptr points
       to a GUID struct, so call into GPT coldplug api instead */
    sbl_partitions[coldplug_id].chdl =
        boot_coldplug_open_gpt_partition (dev_type,
                                                   dev_loc,
                                                   (struct coldplug_guid *)partition_id_ptr);
  }

  /* After successfully opening the partition and getting the device handle */
  if (sbl_partitions[coldplug_id].chdl != NULL)
  {
    /* Partition block infomation in the coldplug device API. */
    status = boot_coldplug_get_size(sbl_partitions[coldplug_id].chdl,
        &sbl_partitions[coldplug_id].blksz,&sbl_partitions[coldplug_id].blkcnt);

    BL_VERIFY( (status == COLDPLUG_RESULT_SUCCESS), BL_ERR_HOTPLUG|BL_ERROR_GROUP_BOOT );

    /* Check for the device block count & block size */
    BL_VERIFY( (sbl_partitions[coldplug_id].blkcnt || 
          sbl_partitions[coldplug_id].blksz), BL_ERR_SDC_INVALID_SIZE|BL_ERROR_GROUP_BOOT);
  }  
  else
  {
    coldplug_id = INVALID_FILE_HANDLE;
  }

  return coldplug_id;
} /* get_coldplug_dev_handle */

/*=============================================================================

**  Function :  dev_sdcc_read_bytes

** ==========================================================================
*/
/*!
*
* @brief
*   This function reads the specified number of bytes from eMMC device to the
*   buffer provided. Takes care not to overflow the buffer
*
* @param[in] ram_addr Destination address in RAM
* @param[in] src_offset Source address in bytes from partition start
* @param[in] bytes_to_read Number of bytes to read
* @param[in] parti_id coldplug ID or file handle depending on parti type
*
* @par Dependencies
*   None
*
* @retval
*   TRUE on success FALSE otherwise.
*
* @par Side Effects
*   None
*
*/
boolean dev_sdcc_read_bytes
(
  void   *ram_addr,          /* Destination address in RAM */
  uint64 src_offset,         /* source address in bytes from partition start */
  uint32 bytes_to_read,      /* number of bytes to read */
  uint32 parti_id           /* coldplug ID or file handle depending on parti type */
)
{

  uint32 sector_size, start_sector, end_sector, num_sectors, leading_pad;
  uint32 trailing_length;
  uint64 end_sector_bytes;
  uint16 block_count = 0;

  boolean return_status = FALSE;

  boot_coldplug_partition_t* coldplug_dev = 0;
  uint8* dest_addr = (uint8*)ram_addr;

  coldplug_dev = &sbl_partitions[parti_id];

 do
 {
    if (coldplug_dev->chdl == NULL )
    {
      return_status = FALSE;
      break;
    }

    sector_size = coldplug_dev->blksz;

    if (sector_size == 0 )
    {
      return_status = FALSE;
      break;
    }

    /*Make sure device sector size doesn't exceed the sector buffer size */
    BL_VERIFY(sector_size <= MAX_SECTOR_SIZE, BL_ERR_INVALID_SECTOR_SIZE|BL_ERROR_GROUP_BOOT);
    
    start_sector = src_offset / sector_size;
    leading_pad = src_offset % sector_size;

    end_sector_bytes = src_offset + bytes_to_read;
    end_sector = end_sector_bytes  / sector_size;
    trailing_length = end_sector_bytes % sector_size;

    num_sectors = (trailing_length != 0) ? (end_sector - start_sector + 1) :
                                                              (end_sector - start_sector);

    /* This handles the case of reading a partial sector. In this partial sector
       read we need to skip the leangth of leading bytes */
    if (leading_pad > 0 )
    {
      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_read(coldplug_dev->chdl, start_sector,
        1, dev_sdcc_buf_unalign))
      {
        return_status = FALSE;
        break;
      }

      /* If the requested read lies within this sector, that is the read is less than
         a sector */
      if ( (end_sector == start_sector) && (trailing_length != 0) )
      {
        qmemcpy(dest_addr, &dev_sdcc_buf_unalign[leading_pad], bytes_to_read);
        return_status = TRUE;
        break;
      }
      else
      {
        qmemcpy(dest_addr, &dev_sdcc_buf_unalign[leading_pad], (sector_size - leading_pad));
        dest_addr += (sector_size - leading_pad);
      }

       /* As we have read a partial sector decrement the total number
          of sectors to be read and increment the start sector */
      if (num_sectors > 0)
      {
        num_sectors --;
      }

      start_sector++;
    }/* leading_pad */

    /* If trailing bytes exist, that is a partial read of the last sector, then we
       decrement the number of sectors and do the partial read of the last sector separately */
    if ( (trailing_length != 0) && (num_sectors > 0))
    {
      num_sectors--;
    }

    /* Read all the remaining sectors by looping. */
    while (num_sectors > 0)
    {
      /* If the number of sectors to be read exceeds max value of block_count,
         do it in a loop */
      block_count = (num_sectors > UINT16_MAX_VALUE)? UINT16_MAX_VALUE : num_sectors;

      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_read(coldplug_dev->chdl,
         start_sector, block_count, dest_addr))
      {
        return_status = FALSE;
        break;
      }

      num_sectors  -= block_count;
      start_sector += block_count;
      dest_addr    += block_count*sector_size;
    } /* num_sectors */

    /* If num_sectors is still > 0, that means we broke out prematurely, and
       need to break out of the overall loop, so we catch the FALSE return_status
       and don't continue loading the final trailing bytes, if there are any     */
    if (num_sectors > 0)
    {
       break;
    }

    /* Read the traling bytes - the partial sector at the end */
    if ( trailing_length != 0 )
    {
      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_read(coldplug_dev->chdl, start_sector,
      1, dev_sdcc_buf_unalign))
      {
        return_status = FALSE;
        break;
      }

      qmemcpy(dest_addr, dev_sdcc_buf_unalign, trailing_length);
    }

    return_status = TRUE;
  }while(0);

  return return_status;

} /* dev_sdcc_read_bytes */

/*===========================================================================

**  Function :  boot_coldplug_close_device_by_image_type

** ==========================================================================
*/
/*!
*
* @brief
*   This function closes the file using boot_coldplug_close_partition.
*
* @param[in] image_id Image type
*
* @par Dependencies
*   EFS APIs
*
* @retval
*   TRUE on success FALSE otherwise.
*
* @par Side Effects
*   None
*
*/
int boot_coldplug_close_device_by_image_type
(
  image_type image_id       /* image type */
)
{
  int status = EFS_ERROR;
  int16 coldplug_id = -1;

  BL_VERIFY( image_id < PATH_ID_INDEX_MAX, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT ); 

  /* Get the coldplug ID, used to index into image_partition_id[] */
  coldplug_id = path_id[(uint16)image_id];
  BL_VERIFY( coldplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT );
  BL_VERIFY( coldplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT);

  /* First check if image is in RAW partition */
  if (sbl_partitions[coldplug_id].chdl != NULL)
  {
     /* Close coldplug */
     status = boot_coldplug_close_partition(sbl_partitions[coldplug_id].chdl);
   BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL|BL_ERROR_GROUP_BOOT );
  }

  return status;

} /* coldplug_close_device_by_image_type */

/*===========================================================================

**  Function :  boot_coldplug_close_device_by_handle

** ==========================================================================
*/
/*!
*
* @brief
*   This function closes the file using coldplug_close_device.
*
* @param[in] file_handle file handle
*
* @par Dependencies
*   EFS APIs
*
* @retval
*   TRUE on success FALSE otherwise.
*
* @par Side Effects
*   None
*
*/
int boot_coldplug_close_device_by_handle
(
    uint32 file_handle     /* file handle */
)
{

  int status = EFS_ERROR;

  
  BL_VERIFY( file_handle < IMAGE_NUMBERS, BL_ERR_EFS_INVALID_HANDLE|BL_ERROR_GROUP_BOOT);
  
  /* First check if image is in RAW partition */
  if (sbl_partitions[file_handle].chdl != NULL)
  {
    /* Close coldplug */
    status = boot_coldplug_close_partition (sbl_partitions[file_handle].chdl);
    BL_VERIFY( status == EFS_SUCCESS, BL_ERR_EFS_CLOSE_FAIL|BL_ERROR_GROUP_BOOT );
  }

  return status;

} /* coldplug_close_device_by_handle */

/*==============================================================================

**  Function :  dev_sdcc_write_bytes

** =============================================================================
*/
/*!
*
* @brief
*   This function writes the specified number of bytes to eMMC device from the
*   buffer provided starting at specified offset of the partition
*   We currently don't support unaligned write
*   (destination offset that doesn't start at sector boundary)
*
* @param[in] ram_addr Source address in RAM
* @param[in] dst_offset Destination address in bytes from partition start
* @param[in] bytes_to_write Number of bytes to write
* @param[in] image_id Image type
*
* @par Dependencies
*   None
*
* @retval
*   TRUE on success FALSE otherwise.
*
* @par Side Effects
*   None
*
*/
boolean dev_sdcc_write_bytes
(
  void   *ram_addr,          /* Source address in RAM */
  uint64 dst_offset,         /* Destination address in bytes from partition start */
  uint32 bytes_to_write,     /* Number of bytes to write */
  image_type image_id        /* Image type */
)
{
  boolean return_status = FALSE;
  int16 coldplug_id;
  boot_coldplug_partition_t *coldplug_dev;
  uint32 start_sector, leading_length;
  uint32 end_sector, trailing_length;
  uint16 block_count = 0;
  uint8 *buf_addr = (uint8*)ram_addr;
  uint32 sector_size;

  coldplug_id = path_id[(uint16)image_id];
  coldplug_dev = &sbl_partitions[coldplug_id];
  sector_size = coldplug_dev->blksz;

  /*Make sure device sector size doesn't exceed the sector buffer size */
  BL_VERIFY( (sector_size != 0) && (sector_size <= MAX_SECTOR_SIZE), BL_ERR_INVALID_SECTOR_SIZE|BL_ERROR_GROUP_BOOT);

  start_sector = dst_offset/sector_size;
  leading_length = dst_offset % sector_size;
  BL_VERIFY(leading_length < sector_size, BL_ERR_INVALID_SECTOR_SIZE|BL_ERROR_GROUP_BOOT);
  
  end_sector = (dst_offset + bytes_to_write)/sector_size;
  trailing_length = (dst_offset + bytes_to_write) % sector_size;

  do
  {
    /* Nothing to write */
    if(bytes_to_write == 0)
    {
      return_status = TRUE;
      break;
    }

    if (coldplug_dev->chdl== NULL )
    {
      break;
    }

    if (leading_length > 0)
    {
      /* Read the orginal data from start sector */
      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_read(coldplug_dev->chdl, start_sector,
          1, dev_sdcc_buf_unalign))
      {
        break;
      }

      /* When end_sector equals start_sector, the whole write won't exceed one sector */
      if (end_sector == start_sector)
      {
        /* Check the write size, make sure we don't overflow the buffer.
           Not necessary but added for klocwork*/
        if((bytes_to_write + leading_length) <= sector_size)
        {
          qmemcpy(&dev_sdcc_buf_unalign[leading_length], buf_addr, bytes_to_write);
        }
        else
        {
          /* Should never happen */
          return_status = FALSE;
          break;
        }

        /* Now write back this one leading sector and we are done */
        if (COLDPLUG_RESULT_SUCCESS == boot_coldplug_write(coldplug_dev->chdl,
                                                      start_sector,
                                                      1,
                                                      dev_sdcc_buf_unalign))
        {
          return_status = TRUE;
        }
        break;
      }

      /* The write size is more than this sector, need to copy data till this sector's end */
      qmemcpy(&dev_sdcc_buf_unalign[leading_length], buf_addr, sector_size - leading_length);
      buf_addr += sector_size - leading_length;
      /* Now write back this one leading sector */
      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_write(coldplug_dev->chdl,
                                                    start_sector,
                                                    1,
                                                    dev_sdcc_buf_unalign))
      {
        break;
      }

      /* As we have write a partial sector,increment the start sector */
      start_sector++;
    }

    /* write all the aligned sectors by looping. */
    while (start_sector < end_sector)
    {
      /* If the number of sectors to be write exceeds max value of block_count,
         do it in a loop */
      block_count = ((end_sector - start_sector) > UINT16_MAX_VALUE)? UINT16_MAX_VALUE : (end_sector - start_sector);

      if (COLDPLUG_RESULT_SUCCESS != boot_coldplug_write(coldplug_dev->chdl,
                                                    start_sector,
                                                    block_count,
                                                    buf_addr))
      {
        return_status = FALSE;
        break;
      }
      start_sector += block_count;
      buf_addr    += block_count*sector_size;
    }

    /* If start_sector does not equals end_sector, that means we broke out prematurely, and
       need to break out of the overall loop, so we catch the FALSE return_status
       and don't continue writing the final trailing bytes, if there are any */
    if (start_sector != end_sector)
    {
       break;
    }

    /* Write trailing sector */
    if (trailing_length > 0)
    {
      /* Read sector for partial write */
      if (boot_coldplug_read(coldplug_dev->chdl,
                                end_sector,
                                1,
                                dev_sdcc_buf_unalign)
          != COLDPLUG_RESULT_SUCCESS)
      {
        break;
      }

      /* Copy trailing data to write buffer */
      qmemcpy(dev_sdcc_buf_unalign, buf_addr, trailing_length);

      /* Write back updated sector */
      if (boot_coldplug_write(coldplug_dev->chdl,
                                 end_sector,
                                 1,
                                 dev_sdcc_buf_unalign)
         != COLDPLUG_RESULT_SUCCESS)
      {
        break;
      }
    }

    return_status = TRUE;

  } while (0);

  return return_status;

} /* dev_sdcc_write_bytes */


/*==============================================================================

**  Function :  boot_coldplug_get_partition_size_by_image_id

** =============================================================================
*/
/*!
*
* @brief
*   This function return the parition size in byte.
*
* @par Dependencies
*   None
*
* @retval
*   returns Parition size, 0 if error or parition doesn't exit
*
* @par Side Effects
*   None
*
*/
uint64 boot_coldplug_get_partition_size_by_image_id
(
  image_type image_id        /* Image type */
)
{
  int16 coldplug_id = -1;
  boot_coldplug_partition_t *coldplug_dev;
  uint64 parition_size = 0;
  uint64 block_count = 0;
  uint32 bytes_per_block = 0;
  
  BL_VERIFY( image_id < PATH_ID_INDEX_MAX, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT ); 

  /* Get the coldplug ID, used to index into image_partition_id[] */
  coldplug_id = path_id[(uint16)image_id];
  BL_VERIFY( coldplug_id != INVALID_FILE_HANDLE, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT );
  BL_VERIFY( coldplug_id < IMAGE_NUMBERS, BL_ERR_IMG_NOT_FOUND|BL_ERROR_GROUP_BOOT );

  coldplug_dev = &sbl_partitions[coldplug_id];

  if (coldplug_dev->chdl != NULL )
  {
    if ( COLDPLUG_RESULT_SUCCESS ==
         boot_coldplug_get_size(coldplug_dev->chdl, &bytes_per_block, &block_count))
    {
      parition_size = (uint64)block_count * bytes_per_block;
    }
  }

  return parition_size;
}