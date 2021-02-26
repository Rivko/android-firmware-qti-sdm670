/** @file qcom_setlocale.c
  
  Stub for setlocale function
  
  Copyright (c) 2015, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*
==============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ---------------------------------------------------------
 02/10/15   bh      Initial version 

==============================================================================
*/

#include <locale.h>
#include <stdlib.h>
#include <string.h>

static char *buf;

char *setlocale(int cat, const char *name)
{
  return buf;
}
