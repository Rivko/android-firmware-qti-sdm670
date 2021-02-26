/***********************************************************************
 * Coldplug FAT module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug FAT module.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_fat.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_fat.h"
#include "coldplug_mbr.h"
#include "coldplug_device.h"

#include <string.h>

static int check_fat_signature(struct coldplug_pcache_data *data_cache)
{
	struct coldplug_mbr_layout *mbr;
	uint8 *buffer;

	buffer = coldplug_pcache_read(0ULL, 1, data_cache);
	if (buffer == NULL)
		return -1;

	mbr = (struct coldplug_mbr_layout *) buffer;
	if (mbr->signature[0] != 0x55 || mbr->signature[1] != 0xAA)
		return -1;

	/* Success if this is a FAT boot record. */
	if (mbr->boot_code[11] == 0x00 && mbr->boot_code[12] == 0x02)
		return 0;

	return -1;
}

int coldplug_add_fat_partition(struct coldplug_lun *clun)
{
	struct coldplug_partition *cparti;
	int result;

	cparti = coldplug_partition_alloc(clun);
	if (cparti == NULL) {
		result = COLDPLUG_RESULT_ENOMEM;
		goto End;
	}

	cparti->is_bootable = 0;
	cparti->parti_info.partition_type = COLDPLUG_PARTITION_TYPE_FAT;
	cparti->start_lba = 0;
	cparti->total_lba = clun->total_lba;

	result = 0;

End:
	return result;
}

int coldplug_fat_parse(struct coldplug_lun *clun,
			struct coldplug_pcache_data *data_cache)
{
	int result = -1;

	/* If there is a Fat boot record, add it. */
	if (check_fat_signature(data_cache) == 0) {
		result = coldplug_add_fat_partition(clun);
	}

	return result;
}
