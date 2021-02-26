#pragma once
/*=============================================================================
  @file sns_mem_util.h

  Utility functions associated with memory and memory management.

  @note: All utilities in this file can be used in island mode.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include "stdlib.h"

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/* See memset */
void* sns_memset(void *ptr, int value, size_t size);

/* See memset */
void* sns_memzero(void *ptr, size_t size);

/* See memcmp */
int sns_memcmp(const void *ptr1, const void *ptr2, size_t size);

/* See memscpy */
size_t sns_memscpy(void *dest, size_t dest_size, const void *source, size_t source_size);

/* See strlcpy */
size_t sns_strlcpy(char *dst, const char *src, size_t size);

/* See strlcat */
size_t sns_strlcat(char *dst, const char *src, size_t size);
