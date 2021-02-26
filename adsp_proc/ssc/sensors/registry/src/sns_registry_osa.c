/*=============================================================================
  @file sns_registry_osa.c

  File system wrappers for use by the Sensors Registry.

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "sns_printf_int.h"
#include "sns_registry_osa.h"
#include "sns_time.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#define SNS_REGISTRY_OSA_RETRY_CNT 10

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

FILE*
sns_registry_fopen(char const *filename, const char * mode)
{
  FILE *file = NULL;

  for(int i = 0; NULL == file && i < SNS_REGISTRY_OSA_RETRY_CNT; i++)
  {
    file = fopen(filename, mode);
    if(NULL == file)
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "fopen failed %s, retrying", filename);
      sns_busy_wait(5);
    }
  }

  return file;
}

size_t
sns_registry_fwrite(void const *ptr, size_t size, size_t count, FILE *stream)
{
  size_t len = 0;

  for(int i = 0; 0 == len && i < SNS_REGISTRY_OSA_RETRY_CNT; i++)
  {
    len = fwrite(ptr, size, count, stream);
    if(count != len)
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "fwrite failed, retrying");
      sns_busy_wait(5);
      count -= len;
      ptr = (void*)((uintptr_t)ptr + len);
    }
  }

  return len;
}

size_t
sns_registry_fread(void *ptr, size_t size, size_t count, FILE *stream)
{
  size_t len = 0;

  for(int i = 0; 0 == len && i < SNS_REGISTRY_OSA_RETRY_CNT; i++)
  {
    len = fread(ptr, size, count, stream);
    if(0 == len)
    {
      SNS_SPRINTF(ERROR, sns_fw_printf, "fread failed, retrying");
      sns_busy_wait(5);
    }
  }

  return len;
}
