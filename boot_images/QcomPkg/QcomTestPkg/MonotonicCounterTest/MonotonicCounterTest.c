/** @file MonotonicCounterTest.c
   
  Tests for UEFI Monotonic Counter Test

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/29/12   vishalo Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/MonotonicCounter.h>
#include <Library/PrintLib.h>

/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
MonotonicCounterTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT64      MonotonicCount[10];
  UINT32      HighMonotonicCount[2];
  UINT32      i = 0;

  TEST_START("MonotonicCounterTest");

  for(i=0; i < 10; i++) {  
    Status = gBS->GetNextMonotonicCount(&MonotonicCount[i]);
    if (EFI_ERROR(Status)) {
      AsciiPrint("ERROR: Monotonic Counter: MonotonicCounter failed \n");
      goto TestStop;
    }
    else
      AsciiPrint ("MC[%d]: 0x%016lx\n",i , MonotonicCount[i]); 
  }

  for(i=0; i < 9; i++) {  
    if( ((MonotonicCount[i+1]) - MonotonicCount[i] ) != 1) {
       AsciiPrint ("ERROR: MC[%d]: MonotonicCounter not incrementing correctly\n", i);
       Status = EFI_DEVICE_ERROR;
       goto TestStop;
    }
  }

  /* Overflow test, should increase High Count */
  AsciiPrint ("Monotonic Counter: Overflow test \n");
  gBS->GetNextMonotonicCount(&MonotonicCount[0]);
  AsciiPrint ("MC Read 1: 0x%016lx\n", MonotonicCount[0]);

  for(i=0; i < 0xFFFFFFFF; i++) {  
    gBS->GetNextMonotonicCount(&MonotonicCount[1]);
  }
  AsciiPrint ("MC Read 2: 0x%016lx\n", MonotonicCount[1]);

  if( ((MonotonicCount[1] >> 32) - (MonotonicCount[0] >> 32)) == 1 ) {
    Status = EFI_SUCCESS;
  } 
  else {
    AsciiPrint ("ERROR: MC not incremented after LowCount overflow");
    Status = EFI_DEVICE_ERROR;
  }

  /* Test GetNextHighMonotonicCount */
  gRT->GetNextHighMonotonicCount(&HighMonotonicCount[0]);
  gRT->GetNextHighMonotonicCount(&HighMonotonicCount[1]);
  if( ((HighMonotonicCount[1]) - (HighMonotonicCount[0])) == 1 ) {
    Status = EFI_SUCCESS;
  } 
  else {
    AsciiPrint("ERROR: MC not incremented in GetNextHighCount(...) ");
    Status = EFI_DEVICE_ERROR;
  }

  TestStop: 
  TestStatus("MonotonicCounter", Status);
  TEST_STOP("MonotonicCounterTest");
  return EFI_SUCCESS;
}
