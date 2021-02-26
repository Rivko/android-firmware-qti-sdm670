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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_pcache.h#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#ifndef __COLDPLUG_PCACHE_H__
#define __COLDPLUG_PCACHE_H__

#include "coldplug_config_i.h"
#include "coldplug_comdef.h"
#include "coldplug_device.h"

/* Read cache info. */
struct coldplug_pcache_data {
	struct coldplug_lun *clun;

	uint32 block_size_bytes; /* Size of a Logical Block in bytes */
	uint32 block_count; /* Number of blocks in the device */

	uint8 *buffer; /* Points to the buffer that holds  logical block data*/
	uint32 buffer_size_blocks; /* Number of Logical Blocks that can fit
					in the buffer. */

	int is_valid;
	uint32 cache_size_blocks; /* Number of blocks in the cache. */
	uint64 first_cached_lba; /* LBA of the first cached block in
					the buffer. */
};


void coldplug_pcache_init(void);
struct coldplug_pcache_data* coldplug_pcache_init_for_lun(
					struct coldplug_lun *clun);
uint32 coldplug_pcache_get_block_size(
		struct coldplug_pcache_data *data_cache);
uint32 coldplug_pcache_get_block_count 
		(struct coldplug_pcache_data *data_cache);
uint8* coldplug_pcache_read(uint64 lba, uint32 read_ahead_hint,
			struct coldplug_pcache_data *data_cache);

#endif /* __COLDPLUG_PCACHE_H__ */
