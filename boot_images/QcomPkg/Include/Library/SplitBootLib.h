/** @file UefiSplitBoot.h
  UEFI Split boot interface.

  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.

  Qualcomm Technologies Proprietary and Confidential.

**/
#ifndef __UEFI_SPLITBOOT_H__
#define __UEFI_SPLITBOOT_H__

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/19/17   yg      Created
 
=============================================================================*/

#include <Uefi.h>

typedef VOID (*BootContinueFnPtr) (VOID*);

/* This function doesn't return if succeeds, instead continues the execution
 * from BootContinue function */
EFI_STATUS InitSplitBoot (BootContinueFnPtr BootContinueFn, VOID* Arg);

VOID AuxCpuCEntry (UINT64 cpu);

#endif  /* __UEFI_SPLITBOOT_H__ */


