/***********************************************************************
 * fs_os_string.c
 *
 * String manupulation api's for all FS modules.
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/fs_osal/src/fs_os_string.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os_string.h"
#include "comdef.h"
#include "fs_os_err.h"
#include "fs_os_msg.h"
#include "stringl/stringl.h"
#include <stdlib.h>

size_t
fs_os_strlcat(char *dst, const char *src, size_t siz)
{
  size_t new_len;

  new_len =  strlcat(dst, src, siz);
  FS_OS_ASSERT_FATAL (new_len < siz);

  return new_len;
}

size_t
fs_os_strlcpy(char *dst, const char *src, size_t siz)
{
  size_t new_len;

  new_len =  strlcpy(dst, src, siz);
  FS_OS_ASSERT_FATAL(new_len < siz);

  return new_len;
}

void
fs_os_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t copied_bytes;

  FS_OS_ASSERT_FATAL (dst != 0);
  FS_OS_ASSERT_FATAL (src != 0);

  FS_OS_ASSERT_FATAL (dst_size >= src_size);

  /* Check memory region overlap. We use the src_size for checks as that is the
   * max size that will be copied. Replace memscpy with memmove if you
   * encounter this ASSERT.  */
  if ((((uint8*)src <= (uint8*)dst) &&
       ((uint8*)dst < ((uint8*)src + src_size))) ||
      (((uint8*)dst <= (uint8*)src) &&
       ((uint8*)src < ((uint8*)dst + src_size))))
  {
    FS_OS_MSG_ERROR_0 ("Memscpy on overlapping regions!");
    FS_OS_ASSERT_FATAL (0);
  }

  if (dst_size < src_size)
  {
    src_size = dst_size;
  }

  copied_bytes = memscpy(dst, dst_size, src, src_size);
  FS_OS_ASSERT_FATAL (copied_bytes == src_size);
}

void
fs_os_memsmove(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t moved_bytes;

  FS_OS_ASSERT_FATAL (dst != 0);
  FS_OS_ASSERT_FATAL (src != 0);

  FS_OS_ASSERT_FATAL (dst_size >= src_size);

  if (dst_size < src_size)
  {
    src_size = dst_size;
  }

  moved_bytes = memsmove (dst, dst_size, src, src_size);
  FS_OS_ASSERT_FATAL (moved_bytes == src_size);
}
