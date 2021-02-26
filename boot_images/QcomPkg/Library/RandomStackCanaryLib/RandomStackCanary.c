/******************************************************************//**
 * @file RandomStackCanary.c
 *
 * @brief RandomStackCanaryLib functions
 *
 * Copyright (c) 2015 by Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
08/28/15   aus     Initial checkin
========================================================================*/

#include <Library/ProcAsmLib.h>
#include <Library/BaseLib.h>
#include <Library/TzRngLib.h>

/**
 * Get random stack canary
 */
UINTN GetRandomStackCanary (VOID)
{
  return GetRNGVal();
}

