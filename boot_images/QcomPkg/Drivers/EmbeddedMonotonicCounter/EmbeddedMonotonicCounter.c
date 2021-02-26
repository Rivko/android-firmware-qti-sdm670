/** @file

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.<BR>
  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/25/15   bh      As per spec, don't increment high count on "Get" call
                    Variable should be saved in Qcom token space 
 06/29/12   vishalo Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/MonotonicCounter.h>

extern EFI_GUID gQcomTokenSpaceGuid;

/* Vendor GUID of the variable for the high part of monotonic counter */
#define mEfiMtcName ((CHAR16 *) L"MTC")
STATIC UINT32      gHighCount = 0;

EFI_STATUS
EFIAPI
GetNextMonotonicCount (
  OUT UINT64  *Count
  )
{
  EFI_STATUS    Status;
  STATIC UINT32 LowCount = 0;
  UINTN         BufferSize;
  if (Count == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if((LowCount + 1) < LowCount){
    LowCount = 0; /* Overflow, Increment HighCount and update variable */
    gHighCount++;

    BufferSize = sizeof (UINT32);
    Status = gRT->SetVariable(mEfiMtcName,
                              &gQcomTokenSpaceGuid, 
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 
                              BufferSize,
                              &gHighCount);
    if (EFI_ERROR (Status)) {
      DEBUG(( EFI_D_ERROR, " Unable to set MTC variable, Status =  (0x%x)\r\n", Status));
      gHighCount = 0; /* MTC variable not set, HighCount is Volatile */
    }
  }
  *Count = ( (( (UINT64) gHighCount << 32 ) | (++LowCount)) );
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetNextHighMonotonicCount (
  OUT UINT32  *HighCount
  )
{
  if (HighCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HighCount = gHighCount;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MonotonicCounterDriverInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  EFI_HANDLE  Handle = NULL;

  // Make sure the Monotonic Counter Architectural Protocol is not already installed in the system
  ASSERT_PROTOCOL_ALREADY_INSTALLED(NULL, &gEfiMonotonicCounterArchProtocolGuid);

  // Fill in the EFI Boot Services and EFI Runtime Services Monotonic Counter Fields
  gBS->GetNextMonotonicCount     = GetNextMonotonicCount;
  gRT->GetNextHighMonotonicCount = GetNextHighMonotonicCount;

  BufferSize = sizeof (UINT32);
  Status = gRT->GetVariable(mEfiMtcName, 
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &BufferSize,
                            &gHighCount);

  if (EFI_ERROR (Status)) {
    gHighCount = 0;
  }
  gHighCount++; /* Increment for this boot */

  BufferSize = sizeof (UINT32);
  Status = gRT->SetVariable(mEfiMtcName, 
                            &gQcomTokenSpaceGuid, 
                            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 
                            BufferSize, 
                            &gHighCount);
  if (EFI_ERROR (Status)) {
    DEBUG(( EFI_D_ERROR, " Unable to set MTC variable, Status =  (0x%x)\r\n", Status));
    gHighCount = 0; /* MTC variable not set, HighCount is Volatile */
  }

  // Install the Monotonic Counter Architectural: Protocol onto a new handle
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiMonotonicCounterArchProtocolGuid,
                  NULL,
                  NULL
                  );
  return Status;
}
