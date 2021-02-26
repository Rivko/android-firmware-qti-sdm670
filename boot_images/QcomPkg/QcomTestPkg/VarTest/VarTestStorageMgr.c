/** @file VarTestStorageMgr.c
   
  Variable Test Storage Manager, contains storage interface for VarTestApp
  Code runs in UEFI as part of VarTestApp

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/06/12   vk      Initial Revision.

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>

#include "VarStorageMgr.h"

#define VARIABLE_ATTRIBUTE (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
//#define VARIABLE_ATTRIBUTE (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS)

EFI_STATUS
GetTableStat (IN  VOID              *Handle,
              OUT UINT64            *MaxStorageSize,
              OUT UINT64            *FreeStorageSize,
              OUT UINT64            *MaxVarSize)
{
  if ((Handle == NULL) || (MaxVarSize == NULL) || (FreeStorageSize == NULL) || (MaxVarSize == NULL))
    return EFI_INVALID_PARAMETER;

  return gRT->QueryVariableInfo (VARIABLE_ATTRIBUTE,
                                 MaxStorageSize, 
                                 FreeStorageSize, 
                                 MaxVarSize);
}

EFI_STATUS
SetVariableEx (IN     VOID         *Handle,
               IN     CHAR16       *VariableName,
               IN     EFI_GUID     *VendorGuid,
               IN     UINTN        DataSize,
               IN     VOID         *Data,
               IN     VOID         *ExtendedRecord OPTIONAL,
               IN     UINTN        *ExtendedRecordSize OPTIONAL)
{
  if ((Handle == NULL) || (VariableName == NULL) || (VendorGuid == NULL) || (Data == NULL) || (DataSize <= 0))
    return EFI_INVALID_PARAMETER;
  return gRT->SetVariable(VariableName, VendorGuid, VARIABLE_ATTRIBUTE, DataSize, Data);
}


EFI_STATUS
DeleteVariableEx (IN     VOID         *Handle,
                  IN     CHAR16       *VariableName,
                  IN     EFI_GUID     *VendorGuid)
{

  EFI_STATUS Status;
  UINTN DataSize = 0;
  Status = gRT->SetVariable(VariableName, VendorGuid, VARIABLE_ATTRIBUTE, DataSize, NULL);
  return Status;
}

EFI_STATUS
GetVariableEx (IN     VOID         *Handle,
               IN     CHAR16       *VariableName, 
               IN     EFI_GUID     *VendorGuid,
               IN OUT UINTN        *DataSize,
               OUT    VOID         *Data,
               OUT    VOID         *ExtendedRecord OPTIONAL,
               IN OUT UINT32       *ExtendedRecordSize OPTIONAL)
{
  UINT32 Attributes = 0;
  return gRT->GetVariable(VariableName, VendorGuid, &Attributes, DataSize, Data);
}

EFI_STATUS
GetNextVariableNameEx (IN     VOID         *Handle,
                       IN OUT UINTN        *VariableNameSize, 
                       IN OUT CHAR16       *VariableName,
                       IN OUT EFI_GUID     *VendorGuid)
{
  EFI_STATUS Status;
  Status = gRT->GetNextVariableName (VariableNameSize, 
                                     VariableName,
                                     VendorGuid);
  return Status;
}



