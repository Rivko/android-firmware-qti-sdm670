/** @file HWConfigLib.c
   
  Check system configuration

  Copyright (c) 2015-2016, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/20/16   vk      Add AARCH64 version
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
  if (Abit == 1)
  {
    DEBUG((EFI_D_WARN, "ERROR: A bit not set !\n"));
    ASSERT(!Abit);
  }
  
  return Status;
}

