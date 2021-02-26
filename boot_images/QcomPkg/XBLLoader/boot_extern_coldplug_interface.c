/*===========================================================================

                    BOOT EXTERN COLDPLUG DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external coldplug drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/17   ds      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_coldplug_interface.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*===========================================================================

**  Function :  boot_coldplug_open_gpt_partition

** ==========================================================================
*/
/*!
 * Opens an GPT partition by using the GPT partition type..
 *
 * @param dev_type:
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device:
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *
 * @param guid:
 *   The partition-type GUID of the GPT partition that needs to be opened.
 *
 * @return:
 *   Upon success a valid coldplug handle to the GPT-partition will be
 *   returned, On failure NULL handle will be returned.
*****************************************************************************/
struct coldplug_handle* boot_coldplug_open_gpt_partition
(
  enum coldplug_device_type dev_type,
	int is_embedded_device,
	struct coldplug_guid *guid
 )
{
  return coldplug_open_gpt_partition(dev_type, is_embedded_device, guid);
}


/*===========================================================================

**  Function :  boot_coldplug_open_mbr_partition

** ==========================================================================
*/
/*!
 * Open the coldplug device for use.
 *
 * Opens an MBR partition by using the MBR partition ID.
 *
 * @param dev_type:
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device:
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *
 * @param mbr_id:
 *   The MBR partition ID of the MBR partition that needs to be opened.
 *
 * @return:
 *   Upon success a valid coldplug handle to the MBR-partition will be
 *   returned, On failure NULL handle will be returned.
*****************************************************************************/
struct coldplug_handle* boot_coldplug_open_mbr_partition 
(
  enum coldplug_device_type dev_type,
	int is_embedded_device,
  uint32 mbr_id
)
{
  return coldplug_open_mbr_partition(dev_type, is_embedded_device, mbr_id);
}


/*===========================================================================

**  Function :  boot_coldplug_close_partition

** ==========================================================================
*/
/*!
 * Close the partition.
 * Closes an opened coldplug partition and releases the given coldplug-handle
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.

*****************************************************************************/
int boot_coldplug_close_partition (struct coldplug_handle *chdl)
{
  return coldplug_close_partition(chdl);
}


/*===========================================================================

**  Function :  boot_coldplug_read

** ==========================================================================
*/
/*!
 * Read attempts to read upto 'lba_count' number of blocks starting
 * from 'lba' from the open device referred by 'chdl' into the buffer
 * starting at 'buf'
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param lba:
 *   Staring Logical Block Address with respect to the partition from where
 *   to start reading from.
 *
 * @param lba_count:
 *   Number of LBAs to read.
 *
 * @param buf:
 *   The buffer where to store the read data.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_read
(
  struct coldplug_handle *chdl,
  uint64 lba,
  uint64 lba_count,
  uint8 *buf
)
{
  return coldplug_read(chdl, lba, lba_count, buf);
}


/*===========================================================================

**  Function :  boot_coldplug_write

** ==========================================================================*/
/**
 * Write attempts to write upto 'lba_count' number of blocks starting
 * from 'lba' to the open device referred by 'chdl' from the buffer starting
 * at 'buf'
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param lba:
 *   Staring Logical Block Address with respect to the partition from where
 *   to start writing from.
 *
 * @param lba_count:
 *   Number of LBAs to write.
 *
 * @param buf:
 *   The buffer where to read the data to be written.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_write
(
  struct coldplug_handle *chdl,
  uint64 lba,
  uint64 lba_count,
  uint8 *buf
)
{
  return coldplug_write(chdl, lba, lba_count, buf);
}


/*===========================================================================

**  Function :  boot_coldplug_init

** ==========================================================================
*/
/*!
 * @par Name:
 *   coldplug_init - Initialize the coldplug module.
 *
 * @par Description:
 *   Initialize the coldplug module. This API must be called once during
 *   startup. This API initializes the coldplug module data structures
 *   and does not init any storage device.
 *
 * @par Return value:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
void boot_coldplug_init(void)
{
  coldplug_init();
}


/*===========================================================================

**  Function :  boot_sdcc_boot_set_device_info

** ==========================================================================
*/
/*!
 * This function sets device_info structure, obtained from boot
 * to be used for optimized device enumeration at boot.
 *
 * @param[in] device_info
 *    pointer to device information from boot
 */
void boot_sdcc_boot_set_device_info(sdcc_boot_device_info_type *device_info)
{
  sdcc_boot_set_device_info(device_info);
}


/*===========================================================================

**  Function :  boot_coldplug_get_size

** ==========================================================================
*/
/*!
 * @par Description:
 *   Gets the size of an partition.
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param lba_size:
 *    The size of an LBA in bytes will be returned in this variable.
 *
 * @param lba_count:
 *    The total number of LBAs present in this partition will be returned
 *    in this variable.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_get_size
(
  struct coldplug_handle *chdl, 
  uint32 *lba_size, 
  uint64 *lba_count
)
{
   return coldplug_get_size(chdl, lba_size, lba_count);
}

/*===========================================================================

**  Function :  boot_coldplug_background_init_start

** ==========================================================================
 */
/*!
 * @par Description:
 *   Triggers a background initialization of the storage device and
 *   returns immediately.
 *
 *   This device undergoing background initialization will not be able to
 *   service any flash requests, so there should not be any API calls to access
 *   this device until this background init completes. 
 *
 *   This background initialization should be completed by calling the
 *   coldplug_background_init_finish() API.
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *   1 -> Internal device
 *   0 -> External device
 *
 * @param init_context
 *   Currently unused, supply 0 for now. This parameter is added for future
 *   in case if we are not able to do the bacground initialization in one shot
 *   and if the storage device requires some policing to finish the
 *   initialization. If this is the case then this API will need to be called
 *   multiple times with some hints in this parameter to the storage driver.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_background_init_start
(
  enum coldplug_device_type device_type,
	int is_embedded_device, uint32 init_context
)
{
  return coldplug_background_init_start(device_type, is_embedded_device, init_context);
}

/*===========================================================================

**  Function :  boot_coldplug_open_boot_device

** ==========================================================================
 */
/*!
 * @par Description:
 *   Initializing a storage device could take some time and boot LUNs are
 *   special LUNs that does not require the device to be initialized to read
 *   data from them. Also Boot LUNs do not support all the usual operations
 *   that can be performed on regular LUNs.
 *
 *   Initialize all the Boot LUNs in an given coldplug device, parses the
 *   partition table (GPT / MBR) and after which the partitions can be opened
 *   for read/write.
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable)?
 *   1 -> Internal device
 *   0 -> External device
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *
*****************************************************************************/
int boot_coldplug_open_boot_device(enum coldplug_device_type dev_type, int is_embedded_device)
{
  return coldplug_open_boot_device(dev_type,is_embedded_device);
}

/*===========================================================================

**  Function :  boot_coldplug_background_init_finish

** ==========================================================================
 */
/*!
 * @par Description:
 *   Completes the ongoing background initialization of the storage device.
 *   This API will block until the bacground init completes.
 *
 *   This API completes the bacground initialization started by the
 *   coldplug_background_init_start() API.
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *   1 -> Internal device
 *   0 -> External device
 *
 * @param init_context
 *   Currently unused, supply 0 for now. This parameter is added for future
 *   in case if we are not able to do the bacground initialization in one shot
 *   and if the storage device requires some policing to finish the
 *   initialization. If this is the case then this API will need to be called
 *   multiple times with some hints in this parameter to the storage driver.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_background_init_finish
(
  enum coldplug_device_type device_type,
	int is_embedded_device, 
  uint32 init_context
)
{
  return coldplug_background_init_finish(device_type,is_embedded_device, init_context);
}

/*===========================================================================

**  Function :  boot_coldplug_open_device

** ==========================================================================
 */
/*!
 * @par Description:
 *   Initialize all the LUNs in an given coldplug device, parses the partition
 *   table (GPT / MBR) and after which the partitions can be opened
 *   for read/write.
 *   If the Boot-LUN is already parsed with the coldplug_open_boot_device() API,
 *   then the boot-LUN will be skipped during this init. 
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *   1 -> Internal device
 *   0 -> External device
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int boot_coldplug_open_device(enum coldplug_device_type dev_type,	int is_embedded_device)
{
  return coldplug_open_device(dev_type, is_embedded_device);
}
