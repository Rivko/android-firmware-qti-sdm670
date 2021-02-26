#ifndef BOOT_SDCC_H
#define BOOT_SDCC_H

/*===========================================================================

                Boot SDCC/eMMC/MicroSD Header File

GENERAL DESCRIPTION
  This header file contains declarations and definitions specific
  to the SDCC/eMMC/MicroSD boot support.

Copyright 2009-2015,2017 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/17   ds      coldplug changes
10/05/15   ck      Corrected copyright
11/20/14   kedara  Code cleanup,update to to include MIheader.h
04/02/13   dh      Add hotplug_get_partition_size_by_image_id
07/20/12   dh      Add dev_sdcc_write_bytes(doesn't support unaligned write)
09/15/11   dh      Remove unused efs functions
03/04/11   dxiang  Support loading of general images with custom partition types 
03/03/11   plc     Move "SD_PATH" location to .builds
02/22/11   dxiang  Increase IMAGE_NUMBERS to accomodate loading of SSD partition 
08/14/10   aus     Merged boot_efs_def.h and boot_sdcc.h
08/10/10   kpa     Added definitions for  ramdumps to sd card feature.
04/14/10   plc     Need larger number of images to account for current reuse
                   of old image ID's, will change again with SB3.0 transition
01/11/10   vtw     Added partition type.
09/21/09   tkuo    Update comments. 
08/26/09   VTW     Initial revision.
============================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/

#include "coldplug_api.h"
#include "fs_errno.h"
#include "MIheader.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
					  
===========================================================================*/

/*---------------------------------------------------------------------------
  Local error code
---------------------------------------------------------------------------*/
#define COLDPLUG_SUCCESS               0x0
#define EFS_NO_ERR                    ENOERR

/*---------------------------------------------------------------------------
  Size of each sector in SDCC/eMMC/MicroSD.
---------------------------------------------------------------------------*/
#define SECTOR_SIZE                  0x200
#define SECTOR_SIZE_BITSHIFT         0x9
#define SECTOR_MASK                  0xFFFFFE00

/*---------------------------------------------------------------------------
  Null-terminate strings HFAT file names.
----------------------------------------------------------------------------*/
#define ADSP_FILE_NAME            "/sys_boot/image/ADSP.MBN" 
#define AMSS_FILE_NAME            "/sys_boot/image/AMSS.MBN" 
#define APPSBL_FILE_NAME          "/sys_boot/image/EMMCBOOT.MBN" 
#define BOOT_IMGAGE_FILE_NAME     "/sys_boot/image/BOOT.IMG"
#define NULL_FILE_NAME            ""

/*----------------------------------------------------------------------------
  Maximum characters for file or path name 
 ---------------------------------------------------------------------------*/
#define MAX_FILE_NAME                 256

/*----------------------------------------------------------------------------
 Invalid file descriptor 
 ----------------------------------------------------------------------------*/
#define INVALID_FILE_HANDLE           -1

/*----------------------------------------------------------------------------
 EFS error  
 ----------------------------------------------------------------------------*/
#define EFS_SUCCESS                    0
#define EFS_ERROR                     -1

/*--------------------------------------------------------------------------
  Data structure describes the boot image file.  
 --------------------------------------------------------------------------*/
typedef struct 
{
  char name[MAX_FILE_NAME];
  int32 file_handle;
} efs_file_type ;

/*---------------------------------------------------------------------------
  Data structure that holds the partition info.
---------------------------------------------------------------------------*/
typedef struct coldplug_handle* coldplug_handle_ptr;
typedef struct
{
   coldplug_handle_ptr chdl;
   uint32 blksz;
   uint64 blkcnt;
} boot_coldplug_partition_t;

/*---------------------------------------------------------------------------
  Partition types support.
---------------------------------------------------------------------------*/
typedef enum
{
  RAW_PARTI,
  FAT_PARTI
} partition_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

**  Function :  boot_get_coldplug_dev_handle

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs the following:
*   1. Get the coldplug device handle of partition  specific to the image id.
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
* @par Side Effects
*   None
* 
*/
int16 boot_get_coldplug_dev_handle
(
  image_type image_id                  /* Image type to open */
);


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
* @param[in] parti_id Coldplug ID or file handle depending on parti type
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
);


/*===========================================================================

**  Function :  boot_coldplug_close_device_by_image_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function closes the file using coldplug_close_device.
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
);


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
    uint32 file_handle		 /* file handle */
);


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
* @par Side Effects
*   None
* 
*/
void boot_coldplug_set_general_partition_id(uint8 * partition_id);


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
);


/*==============================================================================

**  Function :  boot_coldplug_get_partition_size_by_image_id

** =============================================================================
*/
/*!
*
* @brief
*   This function return the parition size in byte given an image id
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
);
#endif /* BOOT_SDCC_H */
