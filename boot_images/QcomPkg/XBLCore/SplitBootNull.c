/** @file UefiSplitBoot.c
  UEFI Split boot library stub.

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/19/17   yg      Created
 
=============================================================================*/

#include <Uefi.h>
#include "Library/SplitBootLib.h"

/* This function doesn't return if succeeds, instead continues the execution
 * from BootContinue function */
EFI_STATUS InitSplitBoot (BootContinueFnPtr BootContinueFn, VOID* Arg)
{
  /* For now fall back to non split booting */
  return EFI_UNSUPPORTED;
}

VOID AuxCpuCEntry (UINT64 cpu)
{
  /* Unsupported */
}

