/***********************************************************************
 * Coldplug iterate module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug iterate module to iterate through all the partitions.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_iter.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_api.h"
#include "coldplug_assert.h"
#include "coldplug_device.h"
#include "stringl/stringl.h"

#include <string.h>

static struct coldplug_iter coldplug_iters[COLDPLUG_MAX_ITERS];

void coldplug_iter_init(void)
{
	memset(&coldplug_iters, 0, sizeof(coldplug_iters));
}

struct coldplug_iter* coldplug_iter_alloc(void)
{
	struct coldplug_iter *citer = NULL;
	uint32 i;

	for (i = 0; i < COLDPLUG_MAX_ITERS; ++i) {
		if (!coldplug_iters[i].is_in_use) {
			citer = &coldplug_iters[i];
			citer->is_in_use = 1;
			break;
		}
	}

	return citer;
}

void coldplug_iter_free(struct coldplug_iter *citer)
{
	COLDPLUG_ASSERT(citer != NULL);
	memset(citer, 0, sizeof(*citer));
}

struct coldplug_iter* coldplug_iter_open(enum coldplug_device_type dev_type,
		int is_embedded_device, int only_fat_partitions)
{
	struct coldplug_iter *citer;

	citer = coldplug_iter_alloc();
	if (citer == NULL)
		goto End;

	citer->dev_type = dev_type;
	citer->is_embedded_device = is_embedded_device;
	citer->only_fat_partitions = only_fat_partitions;

End:
	return citer;
}

struct coldplug_iter_record* coldplug_iter_next(struct coldplug_iter *citer)
{
	struct coldplug_iter_record *citer_rec = NULL;
	struct coldplug_partition *cparti = NULL;
	struct coldplug_lun *clun;
	struct coldplug_slot *cslot;
	struct coldplug_device *cdev;
	int is_fat = 0;

	COLDPLUG_ASSERT(citer != NULL);
	COLDPLUG_ASSERT(citer->is_in_use == 1);

	if (citer->cur_parti_idx >= COLDPLUG_MAX_PARTITIONS)
		goto End;

	while (citer->cur_parti_idx < COLDPLUG_MAX_PARTITIONS) {
		cparti = &coldplug_partitions[citer->cur_parti_idx++];
		if (!cparti->is_in_use)
			continue;

		clun = cparti->clun;
		cslot = clun->cslot;
		cdev = cslot->cdev;

		if (cdev->device_type != citer->dev_type)
			continue;

		if (cslot->is_embedded_device !=
		    citer->is_embedded_device)
			continue;

		if (citer->only_fat_partitions) {
			is_fat = coldplug_partition_is_fat(cparti);
			if (!is_fat)
				continue;
		}

		citer_rec = &citer->citer_rec;
		memset(citer_rec, 0, sizeof(citer->citer_rec));
		strlcpy(citer_rec->parti_name, cparti->name,
			sizeof(citer_rec->parti_name));
		citer_rec->start_lba = cparti->start_lba;
		citer_rec->total_lba = cparti->total_lba;
		goto End;
	}

	citer_rec = NULL;

End:
	return citer_rec;
}

int coldplug_iter_close(struct coldplug_iter *citer)
{
	COLDPLUG_ASSERT(citer != NULL);
	COLDPLUG_ASSERT(citer->is_in_use == 1);
	coldplug_iter_free(citer);
	return 0;
}
