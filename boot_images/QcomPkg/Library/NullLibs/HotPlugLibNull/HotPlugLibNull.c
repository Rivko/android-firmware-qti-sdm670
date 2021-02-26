/** @file HotPlugLibNull.c
  
  Stub functions for HotPlugLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/30/14   kedara  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "fs_public.h"
#include "fs_hotplug.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  hotplug_open_device_by_gpt_partition_type

** ==========================================================================
*/
/*!
 * Open the hotplug device using the partition GUID type.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @param guid_type
 *    GUID type of the partition to open.
 *
 * @return
 *    NULL on failure and a valid hdev pointer on success.
 *
 * @sa
 *   hotplug_open_device, hotplug_open_device_by_partition_type
*****************************************************************************/
struct hotplug_device* hotplug_open_device_by_gpt_partition_type (
  hotplug_dev_type dev_type,
  enum hotplug_iter_device_location dev_loc,
  const struct hotplug_guid *guid_type)
{
  return 0;
}


/*===========================================================================

**  Function :  hotplug_open_device_by_partition_type

** ==========================================================================
*/
/*!
 * Open the hotplug device for use.
 *
 * This must be called before read or write access, and gives the
 * device layer an opportunity to power up the interface or otherwise
 * prepare for media use.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @param partition_type
 *    The partition type from MBR/EBR.
 *
 * @return
 *    NULL on failure and a valid hdev pointer on success.
 *
 * @sa
 *   hotplug_open_device, hotplug_open_device_by_gpt_partition_type
*****************************************************************************/
struct hotplug_device*
hotplug_open_device_by_partition_type (hotplug_dev_type dev_type,
       enum hotplug_iter_device_location dev_loc, uint32 partition_type)
{
  return 0;
}


/*===========================================================================

**  Function :  hotplug_close_device

** ==========================================================================
*/
/*!
 * Close the device.
 *
 * This indicates that no further device access is expected, and
 * permits the device layer to shut down as appropriate.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, returned from the
 *    ::hotplug_open_device() function. Please don't use this function to
 *    close the hdev obtained via hotplug_find_dev_by_path or type functions.
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
*****************************************************************************/
int
hotplug_close_device (struct hotplug_device *hdev)
{
  return 0;
}


/*===========================================================================

**  Function :  hotplug_dev_read

** ==========================================================================
*/
/*!
 * Read attempts to read upto 'block_count' number of blocks starting
 * from 'lba' from the open device referred by 'hdev' into the buffer
 * starting at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, obtained from the
 *    hotplug_open_device function. This represents the persistent
 *    device handle.
 * @param lba
 *    Start reading from this block
 * @param buf
 *    Buffer that holds the read data
 * @param block_cnt
 *    Number of blocks to read
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
*****************************************************************************/
int
hotplug_dev_read (struct hotplug_device *hdev, uint32 lba,
                  unsigned char *buf, uint16 block_cnt)
{
  return 0;
}


/*===========================================================================

**  Function :  hotplug_dev_write

** ==========================================================================*/
/**
 * Write attempts to write upto 'block_count' number of blocks starting
 * from 'lba' to the open device referred by 'hdev' from the buffer starting
 * at 'buf'
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions.  This represents the persistent
 *    device handle.
 * @param lba
 *    Start writing from this block
 * @param buf
 *    Buffer that holds the data to be written
 * @param block_cnt
 *    Number of blocks to write
 *
 * @return
 *    0 for SUCCESS, Any non-zero value indicates FAILURE, and the specific
 *    failure values are device-specific.
 *
 * Note: This call fails if the device is not open
 *
 * Please click @ref hotplug_device "here" for information about the
 * parameters and return values.
*****************************************************************************/
int
hotplug_dev_write (struct hotplug_device *hdev, uint32 lba,
                   unsigned char *buf, uint16 block_cnt)
{
  return 0;
}


/*===========================================================================

**  Function :  hotplug_init_no_poll

** ==========================================================================
*/
/*!
 * @par Name:
 *   hotplug_init - Initialize the hotplug module.
 *
 * @par Synopsis:
 *  @code
 *    #include "fs_hotplug.h"
 *    void hotplug_init (void);
 *  @endcode
 *
 * @par Description:
 *    Initialize the hotplug module, and start its task. This must be called
 *    once during startup. EFS2 must be initialized first. EFS currently calls
 *    this after its own startup.
*****************************************************************************/
void
hotplug_init_no_poll (void)
{
}

/*===========================================================================

**  Function :  hotplug_dev_get_size

** ==========================================================================
*/
/*!
 * Gets the size (number of blocks and size of each block) for a given device.
 *
 * @param hdev
 *    This is a pointer to a Hotplug device, usually returned from
 *    one of the 'find' functions above.  This represents the persistent
 *    device handle.
 *
 * @param block_count
 *    How many blocks (sectors) are available on this media. This function will
 *    write to this variable.
 *
 * @param bytes_per_block
 *    All blocks are a uniform size of this many bytes. This function will
 *    write to this variable.
 *
 * @return
 *    Returns 0 on success and -1 on failure
 *    'block_count' and 'bytes_per_block' are populated with device specific
 *    numbers on success
 */
int
hotplug_dev_get_size (struct hotplug_device *hdev, uint32 *blocks,
                      uint16 *bytes_per_block)
{
  return 0;
}

/*===========================================================================

**  Function :  hotplug_poll_device

** ==========================================================================
*/
/*!
 * Polls the device that matches the supplied device-type and location.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @return
 *    0 on success, or negative error value on failure.
 */
int
hotplug_poll_device (hotplug_dev_type dev_type,
                     enum hotplug_iter_device_location dev_location)
{
  return 0;
}

/*===========================================================================

**  Function :  hotplug_poll_and_mount_first_fat

** ==========================================================================
*/
/*!
 * This API does the following:
 *   1. Polls the device that matches the supplied device-type and location.
 *   2. Inits EFS and mounts ROM-FS as the root file system.
 *   3. Mounts the first FAT-partition as "/mmc1" that matches the supplied
 *      device-type and location.
 *
 * @param dev_type
 *    The hotplug device type, refer to the enum hotplug_dev_type for the
 *    list of Hotplug devices.
 *
 * @param dev_loc
 *    The location of the hotplug device say external, internal or both.
 *
 * @return
 *    0 on success, or negative error value on failure.
 */
int
hotplug_poll_and_mount_first_fat (hotplug_dev_type dev_type,
                                  enum hotplug_iter_device_location dev_loc)
{
  return 0;
}

