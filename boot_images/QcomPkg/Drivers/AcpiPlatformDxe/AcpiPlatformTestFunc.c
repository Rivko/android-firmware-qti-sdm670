/** @file AcpiPlatformTestFunc.c
  Qualcomm ACPI Platform Driver Test Functions

  Copyright (c) 2013-2015, Qualcomm Technologies Inc. All rights
  reserved. 
**/ 

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/14/14   swi     New file for test functionality and test callbacks
=============================================================================*/

#include <PiDxe.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/EFIAcpiPlatform.h>
#include <Protocol/EFIAcpiPlatformTest.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/EfiFileLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/OfflineCrashDump.h> // TODO: remove this when the call to ProcessBGRT is removed.
#include <Guid/Gpt.h>
#include <IndustryStandard/Acpi.h>
#include <Library/QcomTargetLib.h>

#define MAX_ACPI_TABLES  64

EFI_STATUS
AcpiTableRegisterTest 
(
  IN  EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL    *This,
  IN  UINT8   						         TestCase
);

//Testcase string value
CHAR8	*TestCaseName = NULL;

typedef enum
{
  ACPITESTCASE_ONE,
  ACPITESTCASE_TWO,
  ACPITESTCASE_THREE,
  ACPITESTCASE_FOUR,
  NUM_ACPITESTCASES
} ACPITESTCASE;

/**
 PrintAcpiTable prints the given acpi table in a readable format
 
 @param[in] AcpiTable      The ACPI table requested.

 @retval EFI_INVALID_PARAMETER AcpiTable is NULL or AcpiTableSize doesnt
						   match the actual size in the table struct
 **/

static EFI_STATUS EFIAPI PrintAcpiTable(
  IN VOID           *AcpiTable,
  IN UINTN           AcpiTableSize,
  IN BOOLEAN         IsPrefix
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (AcpiTable == NULL)
  {
    DEBUG((DEBUG_LOAD, " PrintAcpiTable requested table was not found.\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((DEBUG_LOAD, " PrintAcpiTable IsPrefix? %d\n", IsPrefix )); 
  DEBUG((DEBUG_LOAD, " Signature = (%x) Length = (%x)\n", 
	   ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->Signature, ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->Length));  
  DEBUG((DEBUG_LOAD, " Revision = (%x) Checksum = (%x) OemId[6] = (%x) \n",
	    ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->Revision, ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->Checksum,
		((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->OemId ));
  DEBUG((DEBUG_LOAD, " OemTableId = (%x) OemRevision = (%x) \n", ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->OemTableId, 
	   ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->OemRevision ));
  DEBUG((DEBUG_LOAD, " CreatorId = (%x) CreatorRevision = (%x)\n\n", ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->CreatorId,
	   ((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTable)->CreatorRevision ));


  return Status;
}

/**
 TableGetCallbackTest is called by AcpiPlatformDxe to get and print all tables. 

 @param[in] AcpiTable      The ACPI table requested.
 @param[in] AcpiTableSize  The size of the ACPI table.
 @param[in] IsPrefix       Whether table is prefix or postfix
 
 @retval EFI_SUCCESS       The function executed successfully.
 @retval EFI_INVALID_PARAMETER AcpiTable is NULL
**/
static EFI_STATUS EFIAPI TableGetCallbackTest (
  IN VOID           *AcpiTable,
  IN UINTN           AcpiTableSize,
  IN BOOLEAN         IsPrefix
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  //UINTN i;
  VOID *AcpiTablePtr;
  if (AcpiTable == NULL)
  {
    DEBUG((DEBUG_LOAD, " TableGetCallbackTest requested table was not found.\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  for(AcpiTablePtr = AcpiTable; (UINT8 *) AcpiTablePtr < (UINT8 *)AcpiTable+AcpiTableSize; 
    AcpiTablePtr = (UINT8 *)AcpiTablePtr+((EFI_ACPI_DESCRIPTION_HEADER*) AcpiTablePtr)->Length)
  {
    PrintAcpiTable(AcpiTablePtr, ((EFI_ACPI_5_0_COMMON_HEADER*) AcpiTablePtr)->Length, IsPrefix);
  }

	
  FreePool(AcpiTable);
  return Status;
}

/**
 TableSetCallbackTest is called by AcpiPlatformDxe to set a table for test purposes.
 Because of that, the behavior of the function may be purposefully bad in some cases
 to emulate and test how the driver functions (e.g. bad input, bad table update) 

 @param[in, out] AcpiTable The specific table to be updated.
 @param[in, out] AcpiTableSize  The size of the Acpi table.

 @retval EFI_SUCCESS      The function executed successfully.
 @retval EFI_INVALID_PARAMETER AcpiTable is NULL or AcpiTableSize is 
                          NULL or AcpiTableSize is less than the table-listed size of 
                          the table
**/
static EFI_STATUS EFIAPI TableSetCallbackTest (
  IN OUT VOID         **AcpiTable,
  IN OUT UINTN         *AcpiTableSize
)
{ 
  EFI_STATUS Status = EFI_SUCCESS;
  if (AcpiTable == NULL || *AcpiTable == NULL 
    || AcpiTableSize == NULL )
  {
    DEBUG((EFI_D_ERROR, " TableSetCallbackTest: Acpi table was not found.\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(*AcpiTableSize < ((EFI_ACPI_DESCRIPTION_HEADER*) *AcpiTable)->Length)
  {
    DEBUG((EFI_D_ERROR, " TableSetCallbackTest: Acpi table size different than expected\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  if(TestCaseName == NULL)
  {
    DEBUG((EFI_D_ERROR, " TableSetCallbackTest: TestCaseName was not set, this should not occur.\r\n"));
    return EFI_INVALID_PARAMETER;
  }
  /* Differentiate between test cases */
  if(AsciiStriCmp (TestCaseName, "ACPITESTCASE_DEFAULT") == 0)
  {
    DEBUG((DEBUG_LOAD, " TableSetCallbackTest: Default test case, nothing to run.\r\n"));
  }
  else if(AsciiStriCmp (TestCaseName, "ACPITESTCASE_TWO") == 0)
  {
	DEBUG((DEBUG_LOAD, " TableSetCallbackTest: ACPITESTCASE_TWO increments %a revision to %x.\r\n", 
		 ((EFI_ACPI_DESCRIPTION_HEADER*) *AcpiTable)->Signature, (((EFI_ACPI_DESCRIPTION_HEADER*) *AcpiTable)->Revision)+1));
    ((EFI_ACPI_DESCRIPTION_HEADER*) *AcpiTable)->Revision = (((EFI_ACPI_DESCRIPTION_HEADER*) *AcpiTable)->Revision)+1;
  }

  return Status;
}

/**
 AcpiTableRegisterTest is called by AcpiPlatformTest application 
 to register callbacks and test functionality of callback framework. 
 Output has to be read manually over serial to confirm correctness.
 
 @param[in] This          A pointer to the EFI_QCOM_ACPIPLATFORM_REGISTER_PROTOCOL instance.
 @param[in] TestCase      The testcase number we are executing, defined in EfiAcpiPlatform.h
 
 @retval EFI_SUCCESS      The function executed successfully.
 @retval EFI_INVALID_PARAMETER This is null or TestCase not valid
 @retval EFI_UNSUPPORTED If the image is not debug, this functionality is not supported
**/
EFI_STATUS AcpiTableRegisterTest
(
  IN EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL  *This,
  IN UINT8    						      TestCase
)
{
  UINTN RegistrationCount =0;
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_ACPIPLATFORM_PROTOCOL *pEfiAcpiPlatformProtocol = NULL;
    DEBUG((DEBUG_LOAD, " AcpiTableRegister started \r\n", Status));
	
	/* When PRODMODE is enabled, the compiler throws an error since rest of code won't ever run.
	  to bypass that error I check to make sure Status is not an error, which is shouldn't be. However
	  this allows the check to be runtime instead of compile time.
	  */
  if(PRODMODE_ENABLED && (!EFI_ERROR (Status)))
  {
    DEBUG(( EFI_D_ERROR, " ACPIPlatformDXE only supports test functions in debug images. "
	  "Please recompile with Debug flag\r\n"));
    return EFI_UNSUPPORTED;
  }
  
  if(This == NULL)
  {
    DEBUG(( EFI_D_ERROR, " ACPIPlatformDXE Callback registration failed, "
      "This is NULL\r\n"));
    return EFI_INVALID_PARAMETER;
  }

  if(TestCase >= NUM_ACPITESTCASES){
	  DEBUG((EFI_D_ERROR, " Incorrect testcase given: %a \r\n", TestCase));
	  return EFI_INVALID_PARAMETER;
  }

  // Locate the AcpiPlatform protocol
  Status = gBS->LocateProtocol(&gQcomAcpiPlatformProtocolGuid,
                              NULL, (VOID**) &pEfiAcpiPlatformProtocol);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, " Locate ACPI Protocol failed, Status = (0x%x)\r\n", Status));
    return Status;
  }
	
  /* Below is a switch statement to select callback registration */
  switch(TestCase) 
  {
  
    case ACPITESTCASE_ONE:
      /* We use GETMODE_BOTH as that gives us postfix and prefix, and we use signature for all the tables */
      DEBUG((DEBUG_LOAD, "  AcpiTableRegister: GETMODE_BOTH for all tables\r\n"));
	  Status = pEfiAcpiPlatformProtocol->AcpiTableRegister( pEfiAcpiPlatformProtocol, ACPI_TABLE_SIGNATURE_ALL, 
			GETMODE_BOTH, TableGetCallbackTest, NULL);
    break;
  case ACPITESTCASE_TWO:
    /* We are writing to DBG2 table and confirming the change occured */
      DEBUG((DEBUG_LOAD, "  AcpiTableRegister: Set DBG2 and confirm changes\r\n"));
      Status = pEfiAcpiPlatformProtocol->AcpiTableRegister( pEfiAcpiPlatformProtocol, 
			EFI_ACPI_5_0_DEBUG_PORT_2_TABLE_SIGNATURE, GETMODE_BOTH, TableGetCallbackTest, TableSetCallbackTest);	  
    break;	  
  case ACPITESTCASE_THREE:
    /* Max out number of registrations */
      DEBUG((DEBUG_LOAD, "  AcpiTableRegister: Max out registrations \r\n"));
	  while( RegistrationCount <= MAX_ACPI_TABLES )
	  {
	    DEBUG((DEBUG_LOAD, "  ACPITESTCASE_THREE: %d\r\n", RegistrationCount ));
	    Status = pEfiAcpiPlatformProtocol->AcpiTableRegister( pEfiAcpiPlatformProtocol, ACPI_TABLE_SIGNATURE_ALL, 
		GETMODE_POSTFIX, TableGetCallbackTest, NULL);
		RegistrationCount++;
	  }
    break;
  case ACPITESTCASE_FOUR:
    /* Provide incorrect signature */
      DEBUG((DEBUG_LOAD, "  AcpiTableRegister: test incorrect signature \r\n"));
      Status = pEfiAcpiPlatformProtocol->AcpiTableRegister( pEfiAcpiPlatformProtocol, 0xdeadbeef, 
	  GETMODE_POSTFIX, TableGetCallbackTest, NULL);
    break;
  default:
  	  DEBUG((DEBUG_LOAD, "  AcpiTableRegister incorrect testcase \r\n"));
	  return EFI_INVALID_PARAMETER;    
  }
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, " AcpiTableRegister failed in %a, Status = (0x%x)\r\n", TestCase, Status));
  }
  DEBUG((DEBUG_LOAD, " AcpiTableRegister finished with %x\r\n", Status));
  return Status;
}
