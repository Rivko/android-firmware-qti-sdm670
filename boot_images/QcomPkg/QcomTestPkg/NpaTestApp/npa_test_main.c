/** @file npa_test_main.c
   
  Tiny UEFI command-line test application for RPM

  Copyright (c) 2013, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/27/13   rli      Initial revision.
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>

int HelloNPA_main( void );
int test_Clock_XORemote(void);

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  int status;
  TEST_START("NPA");
	status = HelloNPA_main();
	AsciiPrint("HelloNPA_main return:%d.\n", status);
    TestStatus("NPA",status);
  TEST_STOP("NPA");

  TEST_START("NPA/RPM");
	status = test_Clock_XORemote();
	AsciiPrint("test_Clock_XORemote return:%d.\n", status); 

    TestStatus("NPA/RPM",status);
  TEST_STOP("NPA/RPM");

  return EFI_SUCCESS;
}
