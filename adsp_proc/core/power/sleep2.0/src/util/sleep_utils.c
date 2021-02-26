/*==============================================================================
  FILE:         sleep_utils.c
  
  OVERVIEW:     Provides sleep global general interest functions
 
  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/util/sleep_utils.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/
#include <stdlib.h>
#include "sleep_utils.h"

/*==============================================================================
                        INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * safe_signed_addition
 */
int64 safe_signed_addition(int64 a, int64 b)
{
  if((a < 0) && (b < 0) && ((INT64_MIN - a) > b))
  {
    /* clip to minimum */
    return INT64_MIN;
  }

  if((a > 0) && (b > 0) && ((INT64_MAX - a) < b))
  {
    /* clip to maximum */
    return INT64_MAX;
  }

  return (a + b);
}

/*
 * safe_signed_truncate
 */
int32 safe_signed_truncate(int64 a)
{
  if(a < INT32_MIN)
    return INT32_MIN;

  if(a > INT32_MAX)
    return INT32_MAX;

  return (int32)a;
}

/*
 * safe_unsigned_truncate
 */
uint32 safe_unsigned_truncate(uint64 a)
{
  if(a > UINT32_MAX)
    return UINT32_MAX;

  return (uint32)a;
}

