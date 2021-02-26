/******************************************************************//**
 * @file RandomStackCanary.c
 *
 * @brief RandomStackCanaryLib Null functions
 *
 * Copyright (c) 2015 by Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
10/27/15   vk      Initial Null checkin
08/28/15   aus     Initial checkin
========================================================================*/

#include <Library/ProcAsmLib.h>
#include <Library/BaseLib.h>

/**
 * Get random stack canary
 */
UINTN GetRandomStackCanary (VOID)
{
  return 0xc0c0c0c0;
}

