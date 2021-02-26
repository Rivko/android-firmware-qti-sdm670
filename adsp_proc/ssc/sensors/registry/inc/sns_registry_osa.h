#pragma once
/*=============================================================================
  @file sns_registry_osa.h

  File system wrappers for use by the Sensors Registry.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

/* Retry open which return NULL */
FILE* sns_registry_fopen(char const *filename, const char * mode);

/* Retry write which return length = 0 */
size_t sns_registry_fwrite(void const *ptr, size_t size, size_t count, FILE *stream);

/* Retry read which return length = 0 */
size_t sns_registry_fread(void *ptr, size_t size, size_t count, FILE *stream);
