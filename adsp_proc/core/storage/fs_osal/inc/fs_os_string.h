/***********************************************************************
 * fs_os_string.h
 *
 * String manupulation api's for all FS modules.
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/fs_osal/inc/fs_os_string.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-13   dks   Create

===========================================================================*/

#ifndef __FS_OS_STRING_H__
#define __FS_OS_STRING_H__

#include "fs_os_config_i.h"
#include "comdef.h"
#include <stddef.h>

/**
  fs_os_strlcat - Size bounded string concatenation.

  Concatenates the source string to destination string.

  This function ensures that the destination string will
  not be improperly terminated and that there will be
  no concatenation beyond the size of the destination buffer.
*/
size_t fs_os_strlcat(char *dst, const char *src, size_t siz);


/**
  fs_os_strlcpy - Size bounded string copy.

  Copies the source string to the destination buffer.

  This function ensures that the destination buffer will always
  be NULL terminated and that there will not be a copy beyond
  the size of the destination buffer.
*/
size_t fs_os_strlcpy(char *dst, const char *src, size_t siz);

/**
  fs_os_memscpy - Size bounded memory copy.

  Copies the source string to the destination buffer.

  This function ensures to copy only a maximum of destination buffer bytes.
*/
void fs_os_memscpy(void *dst,size_t dst_size,const void *src, size_t src_siz);

/**
  fs_os_memsmove - Size bounded memory move.

  Copies the source buffer to the destination buffer when the buffers could
  overlap.

  This function ensures to copy only a maximum of destination buffer bytes.
*/
void fs_os_memsmove(void *dst,size_t dst_siz,const void *src, size_t src_siz);

#endif /* not __FS_OS_STRING_H__ */

