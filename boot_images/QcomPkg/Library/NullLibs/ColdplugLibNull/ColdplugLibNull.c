/** @file ColdPlugLibNull.c
  
  Stub functions for HotPlugLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     -----------------------------------------------------------
2017-07-19 wkimberl  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "coldplug_api.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/*************************************************************************//**
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
int coldplug_init(void)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_open_boot_device - Initialize all the Boot LUNs in an given
 *   coldplug device.
 *
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
int coldplug_open_boot_device(enum coldplug_device_type dev_type,
			int is_embedded_device)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_open_device - Initialize all the LUNs in an given
 *   coldplug device.
 *
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
int coldplug_open_device(enum coldplug_device_type dev_type,
			int is_embedded_device)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_close_device - Closes all the LUNs in an given
 *   coldplug device.
 *
 * @par Description:
 *   Closes all the LUNs in an given coldplug device
 *   Partition-handles opened in this device that are not yet close will
 *   become invalid.
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
int coldplug_close_device(enum coldplug_device_type dev_type,
			int is_embedded_device)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_background_init_start - Triggers a background initialization
 *   of the storage device and returns immediately.
 *
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
int coldplug_background_init_start(enum coldplug_device_type device_type,
	int is_embedded_device, uint32 init_context)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_background_init_finish - Completes the ongoing background
 *   initialization of the storage device.
 *
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
int coldplug_background_init_finish(enum coldplug_device_type device_type,
	int is_embedded_device, uint32 init_context)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_open_gpt_partition - Opens an GPT partition by using the GPT
 *   partition type.
 *
 * @par Description:
 *   Opens the given GPT partition as indicated by the partition-type GUID.
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
struct coldplug_handle* coldplug_open_gpt_partition(
		enum coldplug_device_type dev_type,
		int is_embedded_device,
		const struct coldplug_guid *guid)
{
  return NULL;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_open_mbr_partition - Opens an MBR partition by using the MBR
 *   partition ID.
 *
 * @par Description:
 *   Opens the given MBR partition as indicated by the partition-type GUID.
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
struct coldplug_handle* coldplug_open_mbr_partition(
		enum coldplug_device_type dev_type,
		int is_embedded_device, uint32 mbr_id)
{
  return NULL;
}


/*************************************************************************//**
 * @par Name:
 *   coldplug_close_partition - Closes an opened codplug partition
 *
 * @par Description:
 *   Closes an opened coldplug partition and releases the given
 *   coldplug-handle.
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *****************************************************************************/
int coldplug_close_partition(struct coldplug_handle *chdl)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_get_device_info - Queries the device information.
 *
 * @par Description:
 *   Queries the device specific information.
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param cdev_info:
 *   The device information will be retured in this structure. Which exact
 *   members in the structure will be populated with what data varies based
 *   on the underlying storage flash. Please see the underlying flash driver
 *   API file for more information.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *
 * @note:
 *   This feature is flash device specific and may not be supported on all
 *   all flash devices.
*****************************************************************************/
int coldplug_get_device_info(struct coldplug_handle *chdl,
	struct coldplug_device_info *cdev_info)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_read - Read data from an opened coldplug partition.
 *
 * @par Description:
 *   Read data from an opened coldplug partition.
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
int coldplug_read(struct coldplug_handle *chdl,
		uint64 lba, uint64 lba_count, uint8 *buf)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_write - Write data to an opened coldplug partition.
 *
 * @par Description:
 *   Write data to an opened coldplug partition.
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
int coldplug_write(struct coldplug_handle *chdl,
		uint64 lba, uint64 lba_count, uint8 *buf)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_get_size - Gets the size of an partition.
 *
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
int coldplug_get_size(struct coldplug_handle *chdl, uint32 *lba_size,
		uint64 *lba_count)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_set_pon_wp - Sets Power-On-Write-Protection on the given
 *                         partitiion's LBAs.
 *
 * @par Description:
 *   Sets Power-On-Write-Protection to the given partition for the specified
 *   number of LBAs.
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param start_lba:
 *   Starting LBA from which Power-On-Write-Protection should be applied.
 *
 * @param total_lba:
 *   Total number of LBAs starting from the start_lba to apply
 *   Power-On-Write-Protection.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *
 * @note:
 *   This feature is flash device specific and may not be supported on all
 *   all flash devices.
*****************************************************************************/
int coldplug_set_pon_wp(struct coldplug_handle *chdl,
		uint64 start_lba, uint64 total_lba)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_erase - Erases the specified number of LBAs.
 *
 * @par Description:
 *   Sets sps_end_point
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @param lba:
 *   Starting LBA with respect to the partition from where to start the ERASE.
 *
 * @param total_lba:
 *   Total number of LBAs to erase.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *
 * @note:
 *   This feature is flash device specific and may not be supported on all
 *   all flash devices.
*****************************************************************************/
int coldplug_erase(struct coldplug_handle *chdl,uint64 lba,uint64 total_lba)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_set_bootable_partition - Sets a LUN as an bootable partition
 *
 * @par Description:
 *   Sets a LUN as an bootable partition. This function will call the
 *   underlying storage driver APIs xxx_set_bootable_partition()
 *   Refer to the underlying storage driver's API documentation for more
 *   details.
 *
 * @param chdl:
 *   Coldplug handle obtained via one of the open APIs.
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
 *
 * @note:
 *   This feature is flash device specific and may not be supported on all
 *   all flash devices.
*****************************************************************************/
int coldplug_set_bootable_partition(struct coldplug_handle *chdl)
{
  return COLDPLUG_RESULT_SUCCESS;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_iter_open - Opens an coldplug iterator to iterate partitions.
 *
 * @par Description:
 *   Opens an iterator to iterate through all the partitions.
 *
 * @param dev_type
 *   Coldplug device type (eg: eMMC or UFS). See enum coldplug_device_type for
 *   the complete list of all the coldplug devices.
 *
 * @param is_embedded_device
 *   Is it an internal device (hardwired) or an external device (pluggable).
 *
 * @param only_fat_partitions
 *   0 -> will iterate through all the partitions including FAT partitions
 *   1 -> will interate through only FAT partitions.
 *
 * @return:
 *   Upon success a valid coldplug iterator handle will be returned,
 *   on failure a NULL pointer will be returned.
*****************************************************************************/
struct coldplug_iter* coldplug_iter_open(enum coldplug_device_type dev_type,
		int is_embedded_device, int only_fat_partitions)
{
  return NULL;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_iter_next - Gets the next coldplug partition.
 *
 * @par Description:
 *   Gets the next coldplug_iter_record.
 *
 * @param citer:
 *   Coldplug iterator handle obtained via the ::coldplug_iter_open
 *
 * @return:
 *   Upon success a valid coldplug iter record handle will be returned,
 *   on failure a NULL pointer will be returned.
*****************************************************************************/
struct coldplug_iter_record* coldplug_iter_next(struct coldplug_iter *citer)
{
  return NULL;
}

/*************************************************************************//**
 * @par Name:
 *   coldplug_iter_close - Closes an opened coldplug iterator.
 *
 * @par Description:
 *   Closes an previously opened coldplug iterator and frees up the
 *   iterator handle
 *
 * @param citer:
 *   Coldplug iterator handle obtained via the ::coldplug_iter_open
 *
 * @return:
 *   Upon success the COLDPLUG_RESULT_SUCCESS will be returned otherwise
 *   an appropriate error code will be returned. For a complete list of
 *   all the return values see enum ::coldplug_result.
*****************************************************************************/
int coldplug_iter_close(struct coldplug_iter *citer)
{
  return COLDPLUG_RESULT_SUCCESS;
}


struct coldplug_handle* coldplug_open_partition(const char *parti_name)
{
	return NULL;	
}

int coldplug_get_start_lba(struct coldplug_handle *chdl, uint64 *start_lba)
{
	return 0;		
}
