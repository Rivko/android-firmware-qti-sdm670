/***********************************************************************
 * Coldplug SD/eMMC Block Device.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug SD/eMMC Block Device functions.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_bdev_sd.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"

#ifdef COLDPLUG_SD_OR_EMMC_ENABLE

#include "coldplug_bdev_ops.h"
#include "coldplug_device.h"
#include "coldplug_assert.h"
#include "coldplug_api.h"
#include "coldplug_util.h"
#include "coldplug_debug.h"

#include "sdcc_api.h"

#include <string.h>

#ifdef COLDPLUG_SD_OR_EMMC_DEBUG_DISABLE

static inline void coldplug_bdev_sd_debug_add(enum coldplug_debug_id debug_id,
			SDCC_STATUS sd_res, uint32 val1, uint32 val2)
{
	(void) debug_id; (void) sd_res; (void) val1; (void) val2;
}

#else

static void coldplug_bdev_sd_debug_add(enum coldplug_debug_id debug_id,
			SDCC_STATUS sd_res, uint32 val1, uint32 val2)
{
	struct coldplug_debug_info_type debug_info;

	memset(&debug_info, 0, sizeof(debug_info));
	debug_info.debug_id = debug_id;
	debug_info.dev_type = COLDPLUG_DEVICE_SD;
	debug_info.result = (int)sd_res;
	debug_info.val1 = val1;
	debug_info.val2 = val2;

	coldplug_debug_add(&debug_info);
}

#endif

static int coldplug_bdev_sd_ops_get_slot_and_lun_config(uint32 *max_slots,
		uint32 *max_luns, uint32 *boot_lun_no)
{
	COLDPLUG_ASSERT(max_slots != NULL);
	COLDPLUG_ASSERT(max_luns != NULL);
	COLDPLUG_ASSERT(boot_lun_no != NULL);

	*max_slots = COLDPLUG_MAX_SD_OR_EMMC_SLOTS;
	*max_luns = COLDPLUG_MAX_LUNS_IN_ONE_SLOT;
	*boot_lun_no = -1;

	return 0;
}

static int coldplug_bdev_sd_ops_open(void **bdev_hdl,
		uint32 slot_no, uint32 lun_no)
{
	sdcc_config_info_type sdcc_pconfig;
	struct sdcc_device *sd_hdl;
	boolean is_valid_slot;
	uint32 num_slots;
	int result = COLDPLUG_RESULT_ENODEV;
	SDCC_STATUS sdcc_res;

	COLDPLUG_ASSERT(bdev_hdl != NULL);

	/* Find out the total number of SD slots that are present on-target. */
	sdcc_res = sdcc_get_config_info(&sdcc_pconfig);
	if (sdcc_res != SDCC_NO_ERROR) {
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			sdcc_res, slot_no, lun_no);
		goto End;
	}

	num_slots = (uint32) sdcc_pconfig.num_slots;
	if (slot_no >= num_slots) {
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			SDCC_ERR_FEATURE_UNSUPPORTED, slot_no, lun_no);
		goto End;
	}

	is_valid_slot = sdcc_slot_has_support(slot_no, SDCC_SLOT_MEMORY);
	if (is_valid_slot == 0)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			SDCC_ERR_FEATURE_UNSUPPORTED, slot_no, lun_no);
		goto End;
	}

	sd_hdl = sdcc_handle_open(slot_no, lun_no);
	if (sd_hdl == NULL) {
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			SDCC_ERR_CARD_INIT, slot_no, lun_no);
		goto End;
	}

	*bdev_hdl = (void *)sd_hdl;
	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_close(void *bdev_hdl)
{
	struct sdcc_device *sd_hdl = NULL;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *)bdev_hdl;

	sd_result = sdcc_handle_close(sd_hdl);
	if (sd_result != SDCC_NO_ERROR) {
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_CLOSE,
			sd_result, 0, 0);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_read(void *bdev_hdl, uint64 lba_arg,
	uint64 count_arg, uint8 *buff)
{
	struct sdcc_device *sd_hdl = NULL;
	uint32 lba;
	uint16 count;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *)bdev_hdl;

	lba = (uint32) lba_arg;
	count = (uint16) count_arg;

	sd_result = sdcc_handle_read(sd_hdl, lba, buff, count);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_READ,
			sd_result, lba, count);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_write(void *bdev_hdl, uint64 lba_arg,
	uint64 count_arg, uint8 *buff)
{
	struct sdcc_device *sd_hdl = NULL;
	uint32 lba;
	uint16 count;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *)bdev_hdl;

	lba = (uint32) lba_arg;
	count = (uint16) count_arg;

	sd_result = sdcc_handle_write(sd_hdl, lba, buff, count);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_WRITE,
			sd_result, lba, count);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_get_size(void *bdev_hdl, uint32 *lba_size,
		uint64 *total_lba)
{
	struct sdcc_device *sd_hdl = NULL;
	sdcc_mem_info_type sdcc_pmem_info;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *)bdev_hdl;

	sd_result = sdcc_handle_mem_get_device_info(sd_hdl, &sdcc_pmem_info);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_GET_INFO,
			sd_result, 1, 1);
		goto End;
	}

	*lba_size = sdcc_pmem_info.block_len;
	*total_lba = (uint64) sdcc_pmem_info.card_size_in_sectors;

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_get_info(void *bdev_hdl,
		struct coldplug_device_info *dev_info)
{
	struct sdcc_device *sd_hdl = NULL;
	sdcc_mem_info_type sdcc_pmem_info;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *) bdev_hdl;

	memset(&sdcc_pmem_info, 0, sizeof(sdcc_pmem_info));
	sd_result = sdcc_handle_mem_get_device_info(sd_hdl, &sdcc_pmem_info);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_GET_INFO,
			sd_result, 0, 0);
		goto End;
	}

	dev_info->is_embedded_device = sdcc_pmem_info.is_embedded_device;
	dev_info->bytes_per_lba = sdcc_pmem_info.block_len;
	dev_info->total_lba_count =
		(uint64) sdcc_pmem_info.card_size_in_sectors;
	dev_info->manufacturer_id = sdcc_pmem_info.mfr_id;
	dev_info->oem_id = sdcc_pmem_info.oem_id;
	dev_info->product_rev = sdcc_pmem_info.prod_rev;
	dev_info->product_serial_number = sdcc_pmem_info.prod_serial_num;

	coldplug_memscpy(dev_info->product_name,
		sizeof(dev_info->product_name),
		sdcc_pmem_info.prod_name,
		sizeof(sdcc_pmem_info.prod_name));

	coldplug_memscpy(dev_info->manufactured_date,
		sizeof(dev_info->manufactured_date),
		sdcc_pmem_info.mfr_date,
		sizeof(sdcc_pmem_info.mfr_date));

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_set_pon_wp(void *bdev_hdl, uint64 start_lba_arg,
		uint64 total_lba_arg)
{
	struct sdcc_device *sd_hdl = NULL;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;
	uint32 start_lba, total_lba;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *) bdev_hdl;

	start_lba = (uint32) start_lba_arg;
	total_lba = (uint32) total_lba_arg;

	sd_result = sdcc_handle_set_power_on_write_protection(sd_hdl,
				 start_lba, total_lba);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_SET_PON_WP,
			sd_result, start_lba, total_lba);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_erase(void *bdev_hdl, uint64 start_lba_arg,
		uint64 total_lba_arg)
{
	struct sdcc_device *sd_hdl = NULL;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;
	uint32 start_lba, total_lba;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *) bdev_hdl;

	start_lba = (uint32) start_lba_arg;
	total_lba = (uint32) total_lba_arg;

	sd_result = sdcc_handle_erase(sd_hdl, start_lba, total_lba);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_ERASE,
			sd_result, start_lba, total_lba);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_format(void *bdev_hdl)
{
	COLDPLUG_ASSERT(bdev_hdl != NULL);

	/* Format : No supported for SD/eMMC */
	return COLDPLUG_RESULT_SUCCESS;
}

static int coldplug_bdev_sd_ops_set_bootable_partition(void *bdev_hdl)
{
	struct sdcc_device *sd_hdl = NULL;
	SDCC_STATUS sd_result;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	sd_hdl = (struct sdcc_device *) bdev_hdl;

	sd_result = sdcc_handle_set_active_bootable_partition(sd_hdl);
	if (sd_result != SDCC_NO_ERROR)	{
		coldplug_bdev_sd_debug_add(COLDPLUG_DEBUG_ID_SET_BOOTABLE,
			sd_result, 0,0);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_sd_ops_background_init_start(uint32 slot_no,
		uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_NOT_SUPPORTED;
}

static int coldplug_bdev_sd_ops_background_init_finish(uint32 slot_no,
		uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_NOT_SUPPORTED;
}

static struct coldplug_bdev_ops coldplug_bdev_sd_ops =
{
	coldplug_bdev_sd_ops_get_slot_and_lun_config,
	coldplug_bdev_sd_ops_open,
	coldplug_bdev_sd_ops_close,
	coldplug_bdev_sd_ops_read,
	coldplug_bdev_sd_ops_write,
	coldplug_bdev_sd_ops_get_size,
	coldplug_bdev_sd_ops_get_info,
	coldplug_bdev_sd_ops_set_pon_wp,
	coldplug_bdev_sd_ops_erase,
	coldplug_bdev_sd_ops_format,
	coldplug_bdev_sd_ops_set_bootable_partition,
	coldplug_bdev_sd_ops_background_init_start,
	coldplug_bdev_sd_ops_background_init_finish
};

struct coldplug_bdev_ops* coldplug_bdev_sd_get_ops(void)
{
	return &coldplug_bdev_sd_ops;
}

void coldplug_bdev_sd_init(void)
{
}

#endif /* COLDPLUG_SD_OR_EMMC_ENABLE */
