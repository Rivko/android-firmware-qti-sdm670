/** @file TargetInitLib.c
 
  @brief Target Specific Initialization Functions
 
  Copyright (c) 2013,2017 by Qualcomm Technologies, Inc. All Rights Reserved.

**/ 
 
/*=======================================================================
                        Edit History
  when       who     what, where, why
  --------   ----    ---------------------------------------------------
  02/27/17   vk      Remove unused
  11/15/13   vk      Disable DebugDisableFuse and Crash check
  03/04/13   niting  Add check for HLOS crash cookied address based on HW version
  01/28/13   vishalo Initial Version

========================================================================*/

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>


VOID
TargetEarlyInit (VOID)
{
}

VOID
TargetLateInit (VOID)
{
  return;
}
