/***********************************************************************
 * tftp_string_modem.c
 *
 * NHLOS String Manipulation API abstractions.
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 ***********************************************************************/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/src/tftp_string_modem.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-07-30   mj    Added tftp_strlen function
2014-12-30   dks   Fixes to config and log module
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_string.h"
#include "tftp_assert.h"
#include "tftp_log.h"

#if !defined (TFTP_NHLOS_BUILD)
  #error "This file should be included only for NHLOS Builds"
#endif

#include "stringl/stringl.h"

size_t
tftp_strlcat(char *dst, const char *src, size_t siz)
{
  size_t new_len;

  new_len =  strlcat (dst, src, siz);
  TFTP_ASSERT (new_len < siz);
  return new_len;
}


size_t
tftp_strlcpy(char *dst, const char *src, size_t siz)
{
  size_t new_len;

  new_len =  strlcpy (dst, src, siz);
  TFTP_ASSERT (new_len < siz);
  return new_len;
}

uint32
tftp_strlen (const char *str)
{
  uint32 len;

  len = (uint32)strlen (str);
  return len;
}

void
tftp_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t copied_bytes;

  TFTP_ASSERT (dst != NULL);
  TFTP_ASSERT (src != NULL);
  TFTP_ASSERT (dst_size >= src_size);

  /* Check memory region overlap. We use the src_size for checks as that is the
   * max size that will be copied. Replace memscpy with memmove if you
   * encounter this ASSERT.  */
  if ((((uint8*)src <= (uint8*)dst) &&
       ((uint8*)dst < ((uint8*)src + src_size))) ||
      (((uint8*)dst <= (uint8*)src) &&
       ((uint8*)src < ((uint8*)dst + src_size))))
  {
    TFTP_LOG_ERR ("Memscpy on overlapping regions!");
    TFTP_ASSERT (0);
  }

  if (dst_size < src_size)
  {
    src_size = dst_size;
  }

  copied_bytes = memscpy(dst, dst_size, src, src_size);
  TFTP_ASSERT (copied_bytes == src_size);
}


void
tftp_memsmove(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t moved_bytes;

  TFTP_ASSERT (dst != NULL);
  TFTP_ASSERT (src != NULL);

  TFTP_ASSERT (dst_size >= src_size);

  if (dst_size < src_size)
  {
    src_size = dst_size;
  }

  moved_bytes = memsmove (dst, dst_size, src, src_size);
  TFTP_ASSERT (moved_bytes == src_size);
}

