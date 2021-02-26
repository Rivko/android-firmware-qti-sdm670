/***********************************************************************
 * coldplug_api.h
 *
 * Coldplug module.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * The Coldplug module initializes the storage device (eMMC / UFS / SD) and
 * parses the partitions (MBR / GPT) and allows users to read and write to
 * the partitions.
 *
 * The coldplug module also supports mounting of FAT-partitions with the
 * file system (EFS) to read/writes files from the FAT file system.
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/storage/coldplug_api.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2017-05-18   rp    Create

===========================================================================*/

#ifndef __COLDPLUG_API_H__
#define __COLDPLUG_API_H__

#include "coldplug_config.h"
#include "coldplug_comdef.h"

/*************************************************************************//**
 * The device types that the coldplug module supports.
 *****************************************************************************/
enum coldplug_device_type
{
	COLDPLUG_DEVICE_DEVNULL,
	COLDPLUG_DEVICE_SD,
	COLDPLUG_DEVICE_UFS,
	COLDPLUG_DEVICE_SPI_NOR,
	COLDPLUG_DEVICE_MAX,
};

/*************************************************************************//**
 * The device specific information of a coldplug device.
 *
 * This structure is populated by the coldplug_get_device_info() API.
 *
 * This structure is an superset of all the members across the flash media
 * the coldplug module supports. So which members of this structure will have
 * what information will vary depending on the underlying flash media. Please
 * see the underlying flash media API file for finer details.
 *
 * @note:
 *   The device specific information may not be available for all the
 *   flash device types.
*****************************************************************************/
struct coldplug_device_info
{
	int    is_embedded_device;
	uint32 bytes_per_lba;
	uint64 total_lba_count;
	uint32 manufacturer_id;
	uint32 oem_id;
	uint32 product_rev;
	uint32 product_serial_number;
	uint8  product_serial_number_string[258];
	uint8  product_name[32];
	uint8  manufactured_date[16];
};

/*************************************************************************//**
 * The return value for most of the coldplug APIs.
*****************************************************************************/
enum coldplug_result
{
	COLDPLUG_RESULT_SUCCESS          =  0,
	COLDPLUG_RESULT_INVALID_ARG      = -1,
	COLDPLUG_RESULT_NOT_SUPPORTED    = -2,
	COLDPLUG_RESULT_ENODEV           = -3,
	COLDPLUG_RESULT_ENOMEM           = -4,
	COLDPLUG_RESULT_DEVICE_ERROR     = -5,
	COLDPLUG_RESULT_INVALID_HANDLE   = -6,
	COLDPLUG_RESULT_ERANGE           = -7,
	COLDPLUG_RESULT_MOUNT_FAILED     = -8,
	COLDPLUG_RESULT_ENOEFS           = -9,
	COLDPLUG_RESULT_UNKNOWN          = -10,
};

/*************************************************************************//**
 * Definition of a Globally Unique Identifier (GUID)
*****************************************************************************/
PACKED struct coldplug_guid
{
	uint32 data1;
	uint16 data2;
	uint16 data3;
	uint8 data4[8];
} PACKED_POST;


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
int coldplug_init(void);

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
			int is_embedded_device);

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
			int is_embedded_device);

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
			int is_embedded_device);
			
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
	int is_embedded_device, uint32 init_context);

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
	int is_embedded_device, uint32 init_context);

struct coldplug_handle;

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
		const struct coldplug_guid *guid);

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
		int is_embedded_device, uint32 mbr_id);


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
int coldplug_close_partition(struct coldplug_handle *chdl);

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
	struct coldplug_device_info *cdev_info);

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
		uint64 lba, uint64 lba_count, uint8 *buf);

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
		uint64 lba, uint64 lba_count, uint8 *buf);

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
		uint64 *lba_count);

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
		uint64 start_lba, uint64 total_lba);

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
int coldplug_erase(struct coldplug_handle *chdl,uint64 lba,uint64 total_lba);

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
int coldplug_set_bootable_partition(struct coldplug_handle *chdl);

/*************************************************************************//**
 * @defgroup coldplug_iterate Coldplug Device Iterate functions
 *  @ingroup coldplug_module
 * \n \n
 * These functions can be used to iterate through all the devices managed
 * by the coldplug module. Coldplug supports the following types of
 * iteration:
 *   1. Iterate through all partitions for a given device type.
 *   2. Iterate through only the FAT partitions for a given device type.
 *
 * And also while iterating, we can also specify to iterate only the
 * embedded-media or external-media.
 *
 * \link ::coldplug_iter_open coldplug_iter_open \endlink \n
 * \link ::coldplug_iter_next coldplug_iter_next \endlink \n
 * \link ::coldplug_iter_close coldplug_iter_close \endlink \n
 *
*****************************************************************************/
struct coldplug_iter_record {
	char parti_name[COLDPLUG_MAX_PARTITION_NAME_LEN];
	uint64 start_lba;
	uint64 total_lba;
};

struct coldplug_iter {
	int is_in_use;

	enum coldplug_device_type dev_type;
	int is_embedded_device;
	int only_fat_partitions;

	uint32 cur_parti_idx;
	struct coldplug_iter_record citer_rec;
};

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
		int is_embedded_device, int only_fat_partitions);

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
struct coldplug_iter_record* coldplug_iter_next(struct coldplug_iter *citer);

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
int coldplug_iter_close(struct coldplug_iter *citer);

#endif /* __COLDPLUG_API_H__ */
