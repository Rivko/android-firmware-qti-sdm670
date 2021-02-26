/** @file AcpiPlatformTest.c
   
  Test application for AcpiPlatformDxe.

  Copyright (c) 2014-2015, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who       what, where, why
 --------   ---       -----------------------------------------------------------
 02/25/15   vk        Fix LLVM warning
 09/16/14   Wayne     Match UEFI3 changes for Boot.xf
 06/11/14   aus       Removed PRODMODE check as test apps are not added to the image for PRODMODE
 05/23/14   swi       Included default testcase index value for testing
 05/14/14   swi       Modified to interface better with APT testing
 03/26/14   swi       Changed output to EFI_D_WARN as appropriate
 02/14/14   yraparla  Created test application to verify AcpiPlatformDxe

=============================================================================*/

#include <Uefi.h>
#include <Guid/Acpi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomLib.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/EFIAcpiPlatform.h>
#include <Protocol/EFIAcpiPlatformTest.h>
#include <Protocol/LoadedImage.h>
#include <Library/QcomTargetLib.h>

/** Total valid testcase count */
#define MAX_ACPITESTCASE_NUM 0x4
#define DEFAULT_TESTCASE_INDEX 0x0
#define NAME_COL 0x0
#define DESC_COL 0x1

/** Array of valid testcase names and descriptions. To avoid having to update the test protocol for 
  every testcase change, we set up an implicit understanding between the test application and driver.
  The testcase names listed in the name column are declared as enum in the driver. To update the 
  list, you must modify the acpiplatform.c enum as well. The enum value corresponds to the row index
  value in this array. This allows us to pass the index value to the driver which then compares 
  it to the enum value.

  This design still allows the test application to know which testcases are valid (instead of waiting on
  status from driver). It also removes string comparisons in the driver code which helps readability.
  
  This code is to be revamped eventually when automation is enabled.
 **/
char *TESTCASELIST[MAX_ACPITESTCASE_NUM][2] =
{
  {"ACPITESTCASE_ONE", "Get all tables with GETMODE_BOTH"},
  {"ACPITESTCASE_TWO", "Set DBG2 table to increment revision and Get table with GETMODE_BOTH"},
  {"ACPITESTCASE_THREE", "Max out registrations to force an EFI_OUT_OF_RESOURCES"},
  {"ACPITESTCASE_FOUR", "Provide incorrect table signature for EFI_INVALID_PARAMETER"}
};


/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application. If no testcase is provided on
  command line, the app will register the default testcase ACPITESTCASE_ONE

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
AcpiPlatformTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL *pEfiAcpiPlatformTestProtocol = NULL;
  UINTN                           Argc;
  CHAR8                         **Argv;
  CHAR8                          *ArgStr = NULL;
  UINTN                           i;
    
  // Debug image, we are ok to start the test
  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  
  if (EFI_ERROR(Status) || Argc > 1)
    goto badInput;

  
  /* If no parameter given, use default testcase name
     Else look for testcase from command line argument*/
  if(Argc == 0)
  {
    AsciiPrint ("  Registering the default test case (ACPITESTCASE_ONE)\r\n");  
    ArgStr = TESTCASELIST[DEFAULT_TESTCASE_INDEX][NAME_COL];
  }
  else
  {
    ArgStr = Argv[0];  
  }


  for(i=0; i<MAX_ACPITESTCASE_NUM; i++)
  {
    /* confirm testcase given is valid */
    if(AsciiStriCmp(ArgStr, TESTCASELIST[i][NAME_COL]) == 0){
      TEST_START("AcpiPlatformTest");
      /* Testcase valid, load AcpiPlatformTest protocol */
      Status = gBS->LocateProtocol(&gQcomAcpiPlatformTestProtocolGuid,
              NULL, (VOID**) &pEfiAcpiPlatformTestProtocol);
      if (EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN, " Locate ACPI Test Protocol failed, Status = (0x%x)\r\n", Status));
        goto endOfTest;
      }  
      AsciiPrint ("  Testcase is %a\r\n", ArgStr);  
      
      Status = pEfiAcpiPlatformTestProtocol-> AcpiTableRegisterTest(pEfiAcpiPlatformTestProtocol, i);
      goto endOfTest;
    }
  }
  
badInput:  
  DEBUG((DEBUG_LOAD, " Command line arguments failed. Usage: start acpiplatformtest <testcase>\r\n"
           " ex: start acpiplatformtest ACPITESTCASE_ONE\r\n"));
  DEBUG((DEBUG_LOAD, " Here are valid testcases:\r\n"));
  for(i=0; i<MAX_ACPITESTCASE_NUM; i++)
  {
    AsciiPrint ("  %a  %a\r\n", TESTCASELIST[i][NAME_COL], TESTCASELIST[i][DESC_COL]);
  }
  //Removed this line so that automated testing does not fail
  Status = EFI_INVALID_PARAMETER;
   
endOfTest:
  if(Argv)
    FreePool(Argv);
  TestStatus("AcpiPlatformTest",Status);
  TEST_STOP("AcpiPlatformTest");
  
  return Status;
}
