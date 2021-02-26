/**@file

Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

Portions Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  PlatformSmbios.h

Abstract:

  Header file for PlatformSmbiosDxe Driver.

**/

#ifndef _PLATFORM_SMBIOS_H
#define _PLATFORM_SMBIOS_H

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Ppi/ArmMpCoreInfo.h>
#include <Library/FruLib.h>

//
// Data Table extern definitions.
//
#define SMBIOS_TABLE_EXTERNS(NAME) \
extern EFI_SMBIOS_TABLE_HEADER NAME ## Data; \
extern EFI_STRING_ID NAME ## Strings[];

#define SMBIOS_TABLE_ENTRY(NAME1, NAME2) \
  { & NAME1 ## Data, NAME1 ## Strings, NAME2, SMBIOS_HANDLE_PI_RESERVED}

//
// Data table entry update function.
//
typedef EFI_STATUS (EFIAPI EFI_PLATFORM_SMBIOS_DATA_FUNCTION) (
  IN     EFI_SMBIOS_PROTOCOL        *Smbios,
  IN     EFI_HII_HANDLE             HiiHandle,
  IN     EFI_SMBIOS_TABLE_HEADER    *RecordData,
  IN     EFI_STRING_ID              *StringPack OPTIONAL,
  IN OUT EFI_SMBIOS_HANDLE          *SmbiosHandle
  );

//
// Data table entry definition.
//
typedef struct {
  //
  // intermediate input data for SMBIOS record
  //
  EFI_SMBIOS_TABLE_HEADER               *Data;
  EFI_STRING_ID                         *StringPack;
  EFI_PLATFORM_SMBIOS_DATA_FUNCTION     *Function;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
} EFI_PLATFORM_SMBIOS_DATA_TABLE;


/**
  Platform Specific Smbios Initialization

  @param  Smbios                  The EFI_SMBIOS_PROTOCOL instance.
  @param  HiiHandle               Pointer to an HiiHandle to use
  @param  PlatformSmbiosDataTable Pointer to the Platform specific Smbios table data array

  @retval EFI_SUCCESS           Initialization was successful.
  @retval EFI_INVALID_PARAMETER Incoming pointers were set to NULL.

**/
EFI_STATUS
EFIAPI
PlatformSpecificSmbiosInit (
  IN  EFI_SMBIOS_PROTOCOL            *Smbios,
  OUT EFI_HII_HANDLE                 *HiiHandle,
  OUT EFI_PLATFORM_SMBIOS_DATA_TABLE **PlatformSmbiosDataTable
  );

/**
  Add an SMBIOS record.

  @param  Smbios                The EFI_SMBIOS_PROTOCOL instance.
  @param  HiiHandle             The HiiHandle to use
  @param  Record                The data for the fixed portion of the SMBIOS record. The format of the record is
                                determined by EFI_SMBIOS_TABLE_HEADER.Type. The size of the formatted area is defined
                                by EFI_SMBIOS_TABLE_HEADER.Length and either followed by a double-null (0x0000) or
                                a set of null terminated strings and a null.
  @param  StringPack            Optional parameter that contains an array of EFI_STRING_IDs to append to the record

  @retval EFI_SUCCESS           Record was added.
  @retval EFI_OUT_OF_RESOURCES  Record was not added due to lack of system resources.

**/
EFI_STATUS
EFIAPI
AddSmbiosData (
  IN  EFI_SMBIOS_PROTOCOL     *Smbios,
  IN  EFI_HII_HANDLE          HiiHandle,
  IN  EFI_SMBIOS_TABLE_HEADER *Record,
  IN  EFI_STRING_ID           *StringPack OPTIONAL,
  IN OUT EFI_SMBIOS_HANDLE    *SmbiosHandle
  );

/**
  Get the Platform's Smbios Data
**/
EFI_PLATFORM_SMBIOS_DATA_TABLE *
EFIAPI
GetPlatformSmbiosDataTable (
  VOID
  );

UINT8 GetNumCores (VOID);
ARM_CORE_INFO *GetCoreInfo (VOID);

EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type00BiosInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type04ProcessorInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type07AddL1CacheTables;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type07AddL2CacheTables;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type07AddL3CacheTables;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type08PortConnectorInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type09SystemSlotsInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type13BiosLanguageInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION MemorySmbiosFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type32SystemBootInformationFunction;
EFI_PLATFORM_SMBIOS_DATA_FUNCTION Type38IpmiDeviceInformationFunction;

EFI_STATUS
EFIAPI
Type01ProcessorInformationFunction (
  IN  EFI_SMBIOS_PROTOCOL     *Smbios,
  IN  EFI_HII_HANDLE          HiiHandle,
  IN  EFI_SMBIOS_TABLE_HEADER *Template,
  IN  EFI_STRING_ID           *StringPack OPTIONAL,
  IN OUT EFI_SMBIOS_HANDLE    *SmbiosHandle,
  IN OUT FruAreaProduct       *Product
);

EFI_STATUS
EFIAPI
Type02BaseboardInformationFunction (
  IN  EFI_SMBIOS_PROTOCOL     *Smbios,
  IN  EFI_HII_HANDLE          HiiHandle,
  IN  EFI_SMBIOS_TABLE_HEADER *Template,
  IN  EFI_STRING_ID           *StringPack OPTIONAL,
  IN OUT EFI_SMBIOS_HANDLE    *SmbiosHandle,
  IN OUT FruAreaBoard         *Board
);

EFI_STATUS
EFIAPI
Type03ChassisInformationFunction (
  IN  EFI_SMBIOS_PROTOCOL     *Smbios,
  IN  EFI_HII_HANDLE          HiiHandle,
  IN  EFI_SMBIOS_TABLE_HEADER *Template,
  IN  EFI_STRING_ID           *StringPack OPTIONAL,
  IN OUT EFI_SMBIOS_HANDLE    *SmbiosHandle,
  IN OUT FruAreaChassis       *Chassis
);


#endif /* _PLATFORM_SMBIOS_H */

/* eof - PlatformSmbios.h */
