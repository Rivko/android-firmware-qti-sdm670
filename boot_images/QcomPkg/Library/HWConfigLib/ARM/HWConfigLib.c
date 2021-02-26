/** @file HWConfigLib.c
   
  Check system configuration

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/14/15   bh      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/ProcAsmLib.h>
#include <Library/DebugLib.h>
#include <Library/HWConfigLib.h>

#define CPSR_A_BIT 8

EFI_STATUS
ValidateHWConfig(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Abit = ReadABit();
  UINT32 DacrReg = ReadDACRReg();

  ASSERT(!Abit);

  if (DacrReg != 0x1)
  {
    DEBUG((EFI_D_WARN, "Warning: DACR check failed\n"));
  }
  
  return Status;
}

