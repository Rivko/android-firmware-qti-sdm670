/********************************************************************
 * @file LoaderUtils.c
 *
 * @brief String/Utility function for XBL Loader
 *
 * Copyright (C) 2014 by Qualcomm Technologies Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
01/01/15   bh      Remove stlib function definitions
10/14/14   jb      Removed duplicate memset declaration
09/10/14   ck      Fixed bug with strlcpy's return value
08/21/14   kedara  Update snprintf implementation.
07/18/14   kedara  Added strncmp, strcmp api's.
06/20/14   kedara  Initial revision
========================================================================*/

#include "LoaderUtils.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include "assert.h"


#define PCD_MAX_ASCII_STRING_LENGTH 1000000

#define MAX_FORMAT_STR_SIZE 1024
char format_str_buff[MAX_FORMAT_STR_SIZE];

size_t memscpy( void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;
  CopyMem(dst, src, copy_size);
  return copy_size;
}

static void *xbl_memmove(void *dst, const void *src, size_t len)
{
  return CopyMem(dst, src, len);
}

size_t memsmove(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t  copy_size = (dst_size <= src_size)? dst_size : src_size;
  xbl_memmove(dst, src, copy_size);
  return copy_size;
}


size_t wstrlcpy(CHAR16 * dst,
                const CHAR16 * src,
                size_t size)
{
  /* StrnCpy is natively uses 2 byte characters (I.E. wchar) */
  if (StrnCpy(dst,
              src,
              size) != dst)
  {
    /* An error occured if return value from strncpy does not match dst */
  }

  /* Return the characters copied and not the null padding */
  return StrLen(dst);
}
