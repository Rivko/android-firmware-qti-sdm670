/**
  @file TargetResetLib.c
 
  @brief Target Specific Reset Functions
 
  Copyright (c) 2013 by Qualcomm Technologies, Inc. All Rights Reserved.
 
**/

/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
10/01/13   vk      Shutdown on ASSERT
06/17/13   vishalo Initial version
========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include "PmicShutdown.h"

EFI_STATUS
TargetResetInit (VOID)
{
  return EFI_SUCCESS;
}

/**
  Reset the target
   
**/
VOID
TargetReset (VOID)
{
  /* This register controls the PSHOLD value. 
     1: Asserted
     0: Deasserted */
  MmioWrite32 (PcdGet32 (PcdPsHoldAddress), 0);

  /* Loop here until reset is complete */
  CpuDeadLoop ();
}

/**
  Power off the target. 
   
**/
VOID
TargetPowerOff (VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Shutdown support
  Status = PmicShutdown();

  if (EFI_SUCCESS == Status) 
  {
    /* This register controls the PSHOLD value. 
       1: Asserted
       0: Deasserted */
    MmioWrite32 (PcdGet32 (PcdPsHoldAddress), 0);

    /* Loop here until shutdown is complete */
    CpuDeadLoop ();
  }
}

