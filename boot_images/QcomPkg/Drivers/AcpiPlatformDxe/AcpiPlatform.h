/*============================================================================
  @file AcpiPlatfrom.h

AcpiPlatfrom functions and structures
               Copyright (c) 2014-2015,2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*=============================================================================
                              EDIT HISTORY  


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/18/17   ltg     Added DPP protocol.
 10/14/15   wayne   Added new struct for register callback.
 10/27/14   wayne   Change log print level to EFI_D_INFO
 10/17/14   wayne   Created
=============================================================================*/

#ifndef ACPIPLATFORM_H
#define ACPIPLATFORM_H

#include <PiDxe.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/EFIAcpiPlatform.h>
#include <Protocol/EFIAcpiPlatformTest.h>
#include <Protocol/EFIDpp.h>
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
#include <IndustryStandard/AcpiAml.h>
#include <Protocol/EFIChipInfo.h>


#define ACPI_PLATFORM_DEBUG_PRINT EFI_D_INFO


typedef struct
{
    CHAR8                              VariableName[AML_NAMESTRING_LENGTH];
    GETMODE                            GetMode;                               
    AMLVariableGetCallbackPtr          GetAmlVariableCallback;
    AMLVariableSetCallbackPtr          SetAmlVariableCallback;
}AMLRegisterStruct;

// EFI_ACPIPLATFORM_PROTOCOL implementation
extern EFI_STATUS GetAcpiTable
(
  IN EFI_QCOM_ACPIPLATFORM_PROTOCOL  *This,
  OUT VOID                           *AcpiTablePtr,
  OUT UINTN                          *AcpiTableSize,
  IN  BOOLEAN                         IsPreFix
);

extern EFI_STATUS
GetAcpiTableSize 
(
  IN EFI_QCOM_ACPIPLATFORM_PROTOCOL  *This,
  OUT UINTN                          *PreFixUpAcpiTableSize,
  OUT UINTN                          *PostFixUpAcpiTableSize, 
  OUT UINTN                          *TotalNumOfAcpiTables 
);

extern EFI_STATUS
AcpiTableRegister
(
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  UINT32                             TableSignature,
  IN  GETMODE                            GetMode,                                       
  IN  ACPITableGetCallbackPtr            GetTableCallback,
  IN  ACPITableSetCallbackPtr            SetTableCallback
);
extern EFI_STATUS
AmlVariableRegister
(
  IN  EFI_QCOM_ACPIPLATFORM_PROTOCOL    *This,
  IN  CHAR8                             *VariableName,
  IN  GETMODE                            GetMode,                                       
  IN  AMLVariableGetCallbackPtr          GetAmlVariableCallback,
  IN  AMLVariableSetCallbackPtr          SetAmlVariableCallback
);

extern EFI_STATUS
AcpiTableRegisterTest 
(
  IN  EFI_QCOM_ACPIPLATFORMTEST_PROTOCOL    *This,
  IN  UINT8                                  TestCase
);


// local table for recording callback registration info
typedef struct
{
  IN  UINT32                             TableSignature;
  IN  GETMODE                            GetMode;                                       
  IN  ACPITableGetCallbackPtr            GetTableCallback;
  IN  ACPITableSetCallbackPtr            SetTableCallback;
} RegisterTableType;

// local table for recording AML variable callback registration info
typedef struct
{
  IN  CHAR8                                VariableName[AML_NAMESTRING_LENGTH];
  IN  GETMODE                              GetMode;                                       
  IN  AMLVariableGetCallbackPtr            GetAmlVariableCallback;
  IN  AMLVariableSetCallbackPtr            SetAmlVariableCallback;
  IN  UINTN                                TableSignature;
} RegisterAmlVariableTable;

#define MAX_ACPI_TABLES  64
#define MAX_AML_VARIABLES  64


#endif
