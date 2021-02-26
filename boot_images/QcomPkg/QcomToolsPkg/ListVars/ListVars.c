/** @file ListVars.c
   
  List all Variables, and Variable Storage info

  Copyright (c) 2010-2012,2014, 2019 by Qualcomm Technologies, Inc. All Rights Reserved
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
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
 05/16/14   tonyo   Support for 32/64 bit environment
 02/04/13   vk      Continue enumerating if one query fails
 11/22/12   vk      Rename as ListVars
 07/10/12   niting  Cleanup.
 09/15/11   niting  Added support for QueryVariableInfo and
                    GetNextVariableName.
 09/13/11   niting  Cleanup.
 09/07/11   niting  Initial Revision.

=============================================================================*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>

#define VAR_PRINT_LINE_LENGTH      16             /* 16 bytes */
#define VARIABLE_BLOCKSIZE_BYTES   512            /* 512 bytes*/

#define BS_V_NS_ATTRIBUTES   EFI_VARIABLE_BOOTSERVICE_ACCESS

#define BS_NV_NS_ATTRIBUTES  (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE)

/* 
Currently this table is not supported 
#define BS_V_S_ATTRIBUTES    (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
*/

#define BS_NV_S_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

#define RT_V_NS_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS)

#define RT_NV_NS_ATTRIBUTES  (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE)

/* 
Currently this table is not supported 
#define RT_V_S_ATTRIBUTES    (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)
*/

#define RT_NV_S_ATTRIBUTES   (EFI_VARIABLE_BOOTSERVICE_ACCESS | \
                              EFI_VARIABLE_RUNTIME_ACCESS | \
                              EFI_VARIABLE_NON_VOLATILE | \
                              EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)


EFI_STATUS
EFIAPI
DumpStore ( VOID )
{
  EFI_STATUS                Status;
  UINT64                    MaxStoreSize;
  UINT64                    RemStoreSize;
  UINT64                    MaxVarSize;
  CHAR16                    VariableName[VARIABLE_BLOCKSIZE_BYTES];
  UINTN                    Size;
  EFI_GUID                  VendorGuid;
  UINT8                     *DataBuffer;
  UINTN                    DataSize;
  UINT32                    Attributes;
  UINT32                    VarCount = 0;
  UINT32                    Index;
  UINT32 LineIndex = 0;
  UINT8 LineStr[VAR_PRINT_LINE_LENGTH + 1]; // hold 16 characters + NULL char
  
  do
  {
    Status = gRT->QueryVariableInfo(BS_V_NS_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               BS_V_NS_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);
    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", BS_V_NS_ATTRIBUTES, Status);
  
    Status = gRT->QueryVariableInfo(BS_NV_NS_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               BS_NV_NS_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);
    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", BS_NV_NS_ATTRIBUTES, Status);
  
    Status = gRT->QueryVariableInfo(BS_NV_S_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               BS_NV_S_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);
    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", BS_NV_S_ATTRIBUTES, Status);
  
    Status = gRT->QueryVariableInfo(RT_V_NS_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               RT_V_NS_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);
    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", RT_V_NS_ATTRIBUTES, Status);
  
    Status = gRT->QueryVariableInfo(RT_NV_NS_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               RT_NV_NS_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);

    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", RT_NV_NS_ATTRIBUTES, Status);
  
    Status = gRT->QueryVariableInfo(RT_NV_S_ATTRIBUTES, 
                                    &MaxStoreSize, 
                                    &RemStoreSize, 
                                    &MaxVarSize);
    AsciiPrint("QueryVariableInfo:\n"
               "  Attributes: 0x%08x\n"
               "  MaximumVariableStorageSize: %ld\n"
               "  RemainingVariableStorageSize: %ld\n"
               "  MaximumVariableSize: %ld\n",
               RT_NV_S_ATTRIBUTES,
               MaxStoreSize, 
               RemStoreSize, 
               MaxVarSize);
    if (Status != EFI_SUCCESS)
      AsciiPrint("Failed to query Attributes = 0x%X, Status = 0x%X.\n", RT_NV_S_ATTRIBUTES, Status);

  } while (0);

  VariableName[0] = 0x0000;

  MaxVarSize = 65536; /* 64KB */
  DataSize = MaxVarSize;
  Status = gBS->AllocatePool(EfiRuntimeServicesData,
                             DataSize,
                             (VOID**)&DataBuffer);
  SetMem(DataBuffer, DataSize, 0xFF);

  AsciiPrint("Dumping Storage Tables:\n");
  for (VarCount = 0;;VarCount++)
  {
    Size      = VARIABLE_BLOCKSIZE_BYTES;
    DataSize  = MaxVarSize;

    Status = gRT->GetNextVariableName(&Size, VariableName, &VendorGuid);
    if (Status == EFI_NOT_FOUND) {
      AsciiPrint("Dumping complete\n");
      Status = EFI_SUCCESS;
      break;
    }

    AsciiPrint("Index: %d Name: %s\n"
               "  Vendor Guid: 0x%08x, 0x%04x, 0x%04x, "
               "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n"
               "  Name Size: %d\n",
               VarCount,
               VariableName,
               VendorGuid.Data1, VendorGuid.Data2, VendorGuid.Data3,
               VendorGuid.Data4[0], VendorGuid.Data4[1], VendorGuid.Data4[2],
               VendorGuid.Data4[3], VendorGuid.Data4[4], VendorGuid.Data4[5],
               VendorGuid.Data4[6], VendorGuid.Data4[7],
               Size);

    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to get next name with status: 0x%08x\n", Status);
      break;
    }

    Status = gRT->GetVariable(VariableName, &VendorGuid, &Attributes, &DataSize, DataBuffer);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to get variable data with status: 0x%08x\n", Status);
      break;
    }

    AsciiPrint("  Attributes:%d\n"
                       "  Data Size:%d\n",
               Attributes, 
               DataSize);

    AsciiPrint("  Data Values in Hex and converted to ASCII:\n");
    Index = 0;
    LineIndex = 0;
    LineStr[VAR_PRINT_LINE_LENGTH] = 0; // Setup NULL char
    SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
    while (Index < DataSize)
    {
      AsciiPrint("%02x ", DataBuffer[Index]);
      if ((DataBuffer[Index] < 0x20) || (DataBuffer[Index] > 0x7E))
      {
        LineStr[LineIndex] = '.'; // print a . for any unreadable chars
      }
      else
      {
        LineStr[LineIndex] = DataBuffer[Index];
      }

      LineIndex++;
      if (LineIndex == VAR_PRINT_LINE_LENGTH)
      {
        // Print ascii string
        AsciiPrint(" | %a\n", LineStr);
        SetMem(LineStr, VAR_PRINT_LINE_LENGTH, ' '); // fill with spaces
        LineIndex = 0;
      }

      Index++;
    }
    if (LineIndex > 0)
    {
      for (Index = LineIndex; Index < VAR_PRINT_LINE_LENGTH; Index++)
      {
        AsciiPrint("   "); // fill remaining chars with spaces
      }
      AsciiPrint(" | %a\n", LineStr);
    }

    AsciiPrint("\n");
    SetMem(DataBuffer, DataSize, 0xFF);
  }

  return Status;
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
ListVarsMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN DataSize;
  UINT32 VarData;
  DataSize = sizeof(UINT32);

  Status = gRT->GetVariable (L"BootCycles", 
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VarData);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: Number of Boot Cycles not set.\n");
    VarData = 0;
  }
  AsciiPrint("Number of Boot Cycles: %d\n", VarData);

  Status = gRT->GetVariable (L"RunCycles",
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VarData);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: Number of Run Cycles not set.\n");
    VarData = 0;
  }
  AsciiPrint("Number of Run Cycles: %d\n", VarData);

  Status = gRT->GetVariable (L"BSPowerCycles", 
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &VarData);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: Number of Boot Services Power Cycles not set.\n");
    VarData = 0;
  }
  AsciiPrint("Number of Boot Services Power Cycles: %d\n", VarData);

  Status = DumpStore();

  return Status;
}


