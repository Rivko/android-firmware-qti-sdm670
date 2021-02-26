/*==============================================================================
@file CoreIntrinsicsARM.c

Intrinsics for common assembly operations

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreIntrinsicsARM.c#1 $
==============================================================================*/
#include "CoreIntrinsics.h"

unsigned char __clz(unsigned int val)
{
  int i;

  for(i = 0; i < 32; i++)
    if(val & (1 << (31 - i)))
      break;

  return i;
}
