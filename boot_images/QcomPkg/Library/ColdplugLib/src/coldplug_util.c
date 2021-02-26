/***********************************************************************
 * Coldplug utility functions.
 *
 * Coldplug
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 * Coldplug utility functions to memory copy operations.
 *
 ***********************************************************************/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ColdplugLib/src/coldplug_util.c#1 $ $DateTime: 2017/09/18 09:26:13 $ $Author: pwbldsvc $

  when         who   what, where, why
  ----------   ---   ---------------------------------------------------------
  2015-06-15   rp    Create

 ===========================================================================*/

#include "coldplug_config_i.h"
#include "coldplug_util.h"
#include "coldplug_comdef.h"
#include "coldplug_log.h"
#include "coldplug_assert.h"

#include <stringl/stringl.h>
#include "LoaderUtils.h"

void coldplug_memscpy(void *dst, size_t dst_size, const void *src,
			size_t src_size)
{
	size_t copied_bytes;

	COLDPLUG_ASSERT(dst != NULL);
	COLDPLUG_ASSERT(src != NULL);

	/* Check memory region overlap. We use the src_size for checks as that is the
	* max size that will be copied. Replace memscpy with memmove if you
	* encounter this ASSERT.  */

	if ((((uint8*)src <= (uint8*)dst) &&
	     ((uint8*)dst < ((uint8*)src + src_size))) ||
	    (((uint8*)dst <= (uint8*)src) &&
	     ((uint8*)src < ((uint8*)dst + src_size))))	{
		COLDPLUG_LOG("Memscpy on overlapping regions!");
		COLDPLUG_ASSERT(0); /* Check in debug builds */
	}

	copied_bytes = memscpy(dst, dst_size, src, src_size);
	COLDPLUG_ASSERT(copied_bytes == src_size); /* Check in debug builds */
}

void coldplug_memsmove(void *dst, size_t dst_size, const void *src,
			size_t src_size)
{
	size_t moved_bytes;

	COLDPLUG_ASSERT(dst != NULL);
	COLDPLUG_ASSERT(src != NULL);

	moved_bytes = memsmove(dst, dst_size, src, src_size);
	COLDPLUG_ASSERT(moved_bytes == src_size);
}
