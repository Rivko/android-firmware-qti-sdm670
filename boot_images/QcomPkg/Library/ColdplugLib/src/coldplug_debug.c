/***********************************************************************
 * Coldplug Debug module.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug Debug module to store last few errors in RAM.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_debug.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"

#ifndef COLDPLUG_DEBUG_DISABLE

#include "coldplug_debug.h"
#include "coldplug_assert.h"
#include "coldplug_log.h"
#include "coldplug_util.h"

#include <string.h>
#include <stdio.h>

static struct coldplug_debug_info_type
	coldplug_debug_info[COLDPLUG_DEBUG_INFO_MAX_COUNT];

static int coldplug_debug_info_idx;

void coldplug_debug_init(void)
{
	coldplug_debug_info_idx = 0;
	memset(&coldplug_debug_info, 0, sizeof(coldplug_debug_info));
}

void coldplug_debug_add(struct coldplug_debug_info_type *debug_info)
{
	struct coldplug_debug_info_type *debug_info_new = NULL;

	COLDPLUG_ASSERT(debug_info != NULL);

	if (coldplug_debug_info_idx >= COLDPLUG_DEBUG_INFO_MAX_COUNT)
		coldplug_debug_info_idx = 0;

	debug_info_new = &coldplug_debug_info[coldplug_debug_info_idx];
	coldplug_memscpy(debug_info_new, sizeof(*debug_info_new),
			debug_info, sizeof(*debug_info));
	++coldplug_debug_info_idx;
}

#endif /* COLDPLUG_DEBUG_DISABLE */
