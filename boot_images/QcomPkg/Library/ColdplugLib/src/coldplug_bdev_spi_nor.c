/***********************************************************************
 * Coldplug SPI-NOR Block Device.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug SPI-NOR Block Device module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_bdev_spi_nor.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"

#ifdef COLDPLUG_SPI_NOR_ENABLE

#include "coldplug_bdev_ops.h"
#include "coldplug_device.h"
#include "coldplug_assert.h"
#include "coldplug_api.h"
#include "coldplug_util.h"
#include "coldplug_debug.h"

#include "flash.h"
#include "stringl/stringl.h"

#include <string.h>

#define COLDPLUG_BDEV_NOR_MAX_HANDLES  (2)

struct coldplug_bdev_nor_handle {
	int is_in_use;
	flash_handle_t flash_hdl;

	uint32 page_size;
	uint32 block_size;
	uint32 block_count;

	uint64 lba_size;
	uint64 total_lba;
};

static struct coldplug_bdev_nor_handle
	coldplug_bdev_nor_handles[COLDPLUG_BDEV_NOR_MAX_HANDLES];

#ifdef COLDPLUG_SPI_NOR_DEBUG_DISABLE

static inline void coldplug_bdev_spi_nor_debug_add(
	enum coldplug_debug_id debug_id,
	FLASH_STATUS nor_res, uint32 val1, uint32 val2)
{
	(void) debug_id; (void) nor_res; (void) val1; (void) val2;
}

#else

static void coldplug_bdev_spi_nor_debug_add(enum coldplug_debug_id debug_id,
		FLASH_STATUS nor_res, uint32 val1, uint32 val2)
{
	struct coldplug_debug_info_type debug_info;

	memset(&debug_info, 0, sizeof(debug_info));
	debug_info.debug_id = debug_id;
	debug_info.dev_type = COLDPLUG_DEVICE_SPI_NOR;
	debug_info.result = (int)nor_res;
	debug_info.val1 = val1;
	debug_info.val2 = val2;

	coldplug_debug_add(&debug_info);
}

#endif

static int coldplug_bdev_nor_ops_get_slot_and_lun_config(uint32 *max_slots,
		uint32 *max_luns, uint32 *boot_lun_no)
{
	COLDPLUG_ASSERT(max_slots != NULL);
	COLDPLUG_ASSERT(max_luns != NULL);
	COLDPLUG_ASSERT(boot_lun_no != NULL);

	*max_slots = COLDPLUG_MAX_SPI_NOR_SLOTS;
	*max_luns = COLDPLUG_MAX_SPI_NOR_LUNS_IN_ONE_SLOT;
	*boot_lun_no = -1;

	return 0;
}

static int coldplug_bdev_nor_ops_open(void **bdev_hdl,
		uint32 slot_no, uint32 lun_no)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;
	struct flash_info nor_info;
	FLASH_STATUS nor_res;
	uint32 i, dev_id;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);

	if (slot_no > COLDPLUG_MAX_SPI_NOR_SLOTS)
		goto Error;

	if (lun_no > COLDPLUG_MAX_SPI_NOR_LUNS_IN_ONE_SLOT)
		goto Error;

	for (i = 0; i < COLDPLUG_BDEV_NOR_MAX_HANDLES; ++i) {
		if (!coldplug_bdev_nor_handles[i].is_in_use) {
			nor_hdl = &coldplug_bdev_nor_handles[i];
			memset(nor_hdl, 0, sizeof(*nor_hdl));
			nor_hdl->is_in_use = 1;
			break;
		}
	}

	if (nor_hdl == NULL)  {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			0, slot_no,lun_no);
		goto Error;
	}

	dev_id = slot_no;
	nor_res = flash_device_open(dev_id,
			(flash_handle_t *)&nor_hdl->flash_hdl);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			nor_res, slot_no, lun_no);
		goto Error;
	}

	memset(&nor_info, 0, sizeof(nor_info));
	nor_res = flash_get_info(nor_hdl->flash_hdl, &nor_info);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_OPEN,
			nor_res, slot_no, lun_no);
		goto Error;
	}

	nor_hdl->page_size = nor_info.page_size_bytes;
	nor_hdl->block_size = nor_info.pages_per_block;
	nor_hdl->block_count = nor_info.block_count;

	nor_hdl->lba_size = nor_hdl->page_size * nor_hdl->block_size;
	nor_hdl->total_lba = nor_hdl->block_count;

	*bdev_hdl = nor_hdl;
	result = COLDPLUG_RESULT_SUCCESS;
	goto End;

Error:
	if (nor_hdl != NULL) {
		if (nor_hdl->flash_hdl != NULL)	{
			(void) flash_device_close(nor_hdl->flash_hdl);
		}
		memset(nor_hdl, 0, sizeof(*nor_hdl));
	}

End:
	return result;
}

static int coldplug_bdev_nor_ops_close(void *bdev_hdl)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;
	FLASH_STATUS nor_res;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	nor_hdl = (struct coldplug_bdev_nor_handle * ) bdev_hdl;

	nor_res = flash_device_close(nor_hdl->flash_hdl);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_CLOSE,
			nor_res, 0, 0);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_nor_ops_read(void *bdev_hdl, uint64 start_lba_arg,
		uint64 count_arg, uint8 *buff)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;
	uint32 lba, count, start_page, page_count;
	FLASH_STATUS nor_res;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	nor_hdl = (struct coldplug_bdev_nor_handle *) bdev_hdl;

	lba = (uint32) start_lba_arg;
	count = (uint32) count_arg;

	start_page = lba * nor_hdl->block_size;
	page_count = count * nor_hdl->block_size;

	nor_res = flash_read_pages(nor_hdl->flash_hdl, buff,
				start_page, page_count);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_READ,
			nor_res, lba, count);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_nor_ops_write(void *bdev_hdl, uint64 start_lba_arg,
		uint64 count_arg, uint8 *buff)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;
	uint32 start_page, page_count, start_lba, count;
	FLASH_STATUS nor_res;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	nor_hdl = (struct coldplug_bdev_nor_handle *) bdev_hdl;

	start_lba = (uint32) start_lba_arg;
	count = (uint32) count_arg;

	nor_res = flash_erase_blocks(nor_hdl->flash_hdl, start_lba, count);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_WRITE,
			nor_res, start_lba, count);
		goto End;
	}

	start_page = start_lba * nor_hdl->block_size;
	page_count = count * nor_hdl->block_size;
	nor_res = flash_write_pages(nor_hdl->flash_hdl, buff,
				start_page, page_count);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_WRITE,
				nor_res, start_lba, count);
		goto End;
	}

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_nor_ops_get_size(void *bdev_hdl, uint32 *lba_size,
		uint64 *total_lba)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	COLDPLUG_ASSERT(lba_size != NULL);
	COLDPLUG_ASSERT(total_lba != NULL);

	nor_hdl = (struct coldplug_bdev_nor_handle *) bdev_hdl;

	*lba_size  = nor_hdl->lba_size;
	*total_lba = nor_hdl->total_lba;

	return COLDPLUG_RESULT_SUCCESS;
}

static int coldplug_bdev_nor_ops_get_info(void *bdev_hdl,
		struct coldplug_device_info *dev_info)
{
	struct coldplug_bdev_nor_handle *nor_hdl = NULL;
	struct flash_info nor_info;
	FLASH_STATUS nor_res;
	int result = COLDPLUG_RESULT_ENODEV;

	COLDPLUG_ASSERT(bdev_hdl != NULL);
	COLDPLUG_ASSERT(dev_info != NULL);

	nor_hdl = (struct coldplug_bdev_nor_handle *) bdev_hdl;

	memset(&nor_info, 0, sizeof(nor_info));
	nor_res = flash_get_info(nor_hdl->flash_hdl, &nor_info);
	if (nor_res != FLASH_DEVICE_DONE) {
		coldplug_bdev_spi_nor_debug_add(COLDPLUG_DEBUG_ID_GET_INFO,
			nor_res, 0, 0);
		goto End;
	}

	dev_info->is_embedded_device = 1;
	dev_info->bytes_per_lba = nor_hdl->lba_size;
	dev_info->total_lba_count = nor_hdl->total_lba;
	dev_info->manufacturer_id = nor_info.maker_id;
	dev_info->product_rev = nor_info.version_id;
	dev_info->product_serial_number = nor_info.device_id;
	strlcpy((char *)dev_info->product_name,
		(const char *)nor_info.device_name,
		sizeof(dev_info->product_name));

	result = COLDPLUG_RESULT_SUCCESS;

End:
	return result;
}

static int coldplug_bdev_nor_ops_set_pon_wp(void *bdev_hdl,
		uint64 start_lba, uint64 total_lba)
{
	COLDPLUG_ASSERT(bdev_hdl != NULL);
	(void) start_lba; (void) total_lba;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_nor_ops_erase(void *bdev_hdl, uint64 start_lba,
		uint64 total_lba)
{
	COLDPLUG_ASSERT(bdev_hdl != NULL);
	(void) start_lba; (void) total_lba;
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_nor_ops_format(void *bdev_hdl)
{
	COLDPLUG_ASSERT(bdev_hdl != NULL);
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_nor_ops_set_bootable_partition(void *bdev_hdl)
{
	COLDPLUG_ASSERT(bdev_hdl != NULL);
	return COLDPLUG_RESULT_ENODEV;
}

static int coldplug_bdev_nor_ops_background_init_start(uint32 slot_no,
						uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_NOT_SUPPORTED;
}

static int coldplug_bdev_nor_ops_background_init_finish(uint32 slot_no,
	uint32 init_context)
{
	(void) slot_no; (void) init_context;
	return COLDPLUG_RESULT_NOT_SUPPORTED;
}



static struct coldplug_bdev_ops coldplug_bdev_nor_ops = {
	coldplug_bdev_nor_ops_get_slot_and_lun_config,
	coldplug_bdev_nor_ops_open,
	coldplug_bdev_nor_ops_close,
	coldplug_bdev_nor_ops_read,
	coldplug_bdev_nor_ops_write,
	coldplug_bdev_nor_ops_get_size,
	coldplug_bdev_nor_ops_get_info,
	coldplug_bdev_nor_ops_set_pon_wp,
	coldplug_bdev_nor_ops_erase,
	coldplug_bdev_nor_ops_format,
	coldplug_bdev_nor_ops_set_bootable_partition,
	coldplug_bdev_nor_ops_background_init_start,
	coldplug_bdev_nor_ops_background_init_finish
};

struct coldplug_bdev_ops* coldplug_bdev_spi_nor_get_ops(void)
{
	return &coldplug_bdev_nor_ops;
}

void coldplug_bdev_spi_nor_init(void)
{
	memset(&coldplug_bdev_nor_handles, 0,
		sizeof(coldplug_bdev_nor_handles));
}

#endif /* COLDPLUG_SPI_NOR_ENABLE */
