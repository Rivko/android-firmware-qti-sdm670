/*=============================================================================
  @file sns_mem_util.h

  Utility functions associated with memory and memory management.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdlib.h>
#include "sns_island.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"

#if defined(SSC_TARGET_HEXAGON)
#include <stringl.h>
#else

#include <string.h>
#include "sns_types.h"

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static inline size_t
memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t min_size = dst_size < src_size ? dst_size : src_size;
  memcpy(dst, src, min_size);
  return min_size;
}

static inline size_t
strlcat(char *dst, const char *src, size_t size)
{
  uint32_t i, j;

  for(i = 0; '\0' != dst[i] && i < size; i++)
    ;

  for(j = 0; i < size - 1 && '\0' != src[j]; i++, j++)
    dst[i] = src[j];

  if(i != size)
    dst[i] = '\0';
  return i;
}

#endif

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns_island_heap") void*
sns_memset(void *ptr, int value, size_t size)
{
  return memset(ptr, value, size);
}

SNS_SECTION(".text.sns_island_heap") void*
sns_memzero(void *ptr, size_t size)
{
  return memset(ptr, 0, size);
}

SNS_SECTION(".text.sns_island_heap") int
sns_memcmp(const void *ptr1, const void *ptr2, size_t size)
{
  return memcmp(ptr1, ptr2, size);
}

SNS_SECTION(".text.sns_island_heap") size_t
sns_memscpy(void *dest, size_t dst_size, const void *source, size_t src_size)
{
  return memscpy(dest, dst_size, source, src_size);
}

SNS_SECTION(".text.sns_island_heap") size_t
sns_strlcpy(char *dst, const char *src, size_t size)
{
  //TODO: Fix strlcpy TLB miss
  size_t retval = memscpy(dst, size, src, strlen(src));
  uint32_t terminate_idx = SNS_MIN(size - 1, strlen(src));
  dst[terminate_idx] = '\0';
  return retval;
}

SNS_SECTION(".text.sns_island_heap") size_t
sns_strlcat(char *dst, const char *src, size_t size)
{
  return strlcat(dst, src, size);
}
