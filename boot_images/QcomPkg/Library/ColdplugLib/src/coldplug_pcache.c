/***********************************************************************
 * Coldplug Page Cache module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug Page Cache module to store device pages in RAM.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_pcache.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_device.h"
#include "coldplug_pcache.h"
#include "coldplug_assert.h"

#include <limits.h>
#include <string.h>

/* Holds the sector cache information, shared among GPT and MBR iterators. */
static struct coldplug_pcache_data coldplug_pcache_data_inst;

/* Buffer to hold the read data. */
static uint8 _coldplug_pcache_data_buffer[COLDPLUG_PCACHE_SIZE_IN_BYTES];

/* Use variables to reference the buffer, it allows off target tests to
 * change the cache size. */
static uint8 *coldplug_pcache_data_buffer = _coldplug_pcache_data_buffer;
static uint32 coldplug_pcache_data_buffer_size = COLDPLUG_PCACHE_SIZE_IN_BYTES;


uint32 coldplug_pcache_get_block_size(
	struct coldplug_pcache_data *data_cache)
{
	return data_cache->block_size_bytes;
}

uint32 coldplug_pcache_get_block_count(
	struct coldplug_pcache_data *data_cache)
{
	return data_cache->block_count;
}

/* Reads the number of sectors specified by blk_cnt into the cache. Reads start
 * from the given Logical Block Address(lba).*/
static uint8* coldplug_pcache_read_i(uint64 lba,
	struct coldplug_pcache_data *data_cache, uint32 blk_cnt)
{
	struct coldplug_lun *clun = data_cache->clun;
	uint8 *buffer = data_cache->buffer;
	uint32 max_blk_cnt, dev_size_blocks;
	int result;

	/* There is something wrong with upper layers if they supply zero. */
	COLDPLUG_ASSERT(blk_cnt != 0);

	if (blk_cnt > data_cache->buffer_size_blocks)
		blk_cnt = data_cache->buffer_size_blocks;

	dev_size_blocks = coldplug_pcache_get_block_count(data_cache);
	/* The LBA should be less than the device size */
	if (lba >= dev_size_blocks)
		return NULL;

	max_blk_cnt = dev_size_blocks - ((uint32)lba);
	if (blk_cnt > max_blk_cnt)
		blk_cnt = max_blk_cnt;

	data_cache->cache_size_blocks = 0;
	result = clun->cslot->cdev->bdev_ops->read(clun->bdev_hdl,
			(uint32)lba, blk_cnt, buffer);
	if (result != 0)
		return NULL;

	data_cache->first_cached_lba = lba;
	data_cache->cache_size_blocks = blk_cnt;
	data_cache->is_valid = 1;

	return buffer;
}

/* Change the buffer used for caching. This is used in off target tests
 * to test different cache sizes.
 *
 * param new_buffer new buffer to be used as a cache. If NULL is given
 *                  default values are restored.
 * param buf_size   Size of the buffer. If 0 is given, default values are
 *                  restored.
 */
void coldplug_pcache_change_cache(uint8 *new_buffer, uint32 buf_size)
{
	if (new_buffer == NULL || buf_size == 0) {
		coldplug_pcache_data_buffer = _coldplug_pcache_data_buffer;
		coldplug_pcache_data_buffer_size =
			sizeof(_coldplug_pcache_data_buffer);
	} else {
		coldplug_pcache_data_buffer = new_buffer;
		coldplug_pcache_data_buffer_size = buf_size;
	}
}

void coldplug_pcache_init(void)
{
	memset(&coldplug_pcache_data_inst, 0,
		sizeof(coldplug_pcache_data_inst));
}

struct coldplug_pcache_data* coldplug_pcache_init_for_lun(
		struct coldplug_lun *clun)
{
	struct coldplug_pcache_data *data_cache = &coldplug_pcache_data_inst;
	uint32 buffer_size_blocks;

	if (clun->lba_size > COLDPLUG_LBA_SIZE_IN_BYTES ||
	    clun->lba_size == 0 || clun->total_lba == 0)
		return NULL;

	memset(data_cache, 0, sizeof(*data_cache));
	data_cache->clun = clun;
	data_cache->block_size_bytes = clun->lba_size;
	data_cache->block_count = clun->total_lba;
	data_cache->buffer = coldplug_pcache_data_buffer;

	/* If the cache size in blocks is greater than the number of blocks in
	* the device, then use the device block count as the cache size.
	* We can't cache more sectors than what is in disk. */
	buffer_size_blocks = coldplug_pcache_data_buffer_size / clun->lba_size;
	if (buffer_size_blocks > clun->total_lba)
		buffer_size_blocks = clun->total_lba;

	data_cache->buffer_size_blocks = buffer_size_blocks;

	return data_cache;
}

/* Checks if we have the LBA cached.
 * returns pointer to the buffer if we do, NULL if the lba is not cached. */
static uint8* coldplug_pcache_check_cache(uint64 lba,
			struct coldplug_pcache_data *data_cache)
{
	uint64 first_cached_lba = data_cache->first_cached_lba;

	if (!data_cache->is_valid)
	return NULL;

	if (first_cached_lba <= lba) {
		uint64 lba_offset = lba - first_cached_lba;
		uint64 cached_cnt;

		cached_cnt = ((uint64)data_cache->cache_size_blocks) & UINT_MAX;
		if (cached_cnt > lba_offset) {
			uint8 *buffer = data_cache->buffer;
			/* we have the lba cached, retrurn it. */
			buffer += (lba_offset * data_cache->block_size_bytes);
			return buffer;
		}
	}

	return NULL;
}

uint8* coldplug_pcache_read(uint64 lba, uint32 read_ahead_hint,
		struct coldplug_pcache_data *data_cache)
{
	uint8 *buffer;

	buffer = coldplug_pcache_check_cache(lba, data_cache);

	/* if the lba is not cached, cache it */
	if (buffer == NULL) {
		buffer = coldplug_pcache_read_i(lba, data_cache,
						read_ahead_hint);
		if (buffer != NULL)
			buffer = coldplug_pcache_check_cache(lba, data_cache);
	}

	return buffer;
}
