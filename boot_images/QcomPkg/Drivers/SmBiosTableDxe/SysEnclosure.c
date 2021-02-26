/** @file
  Implements the System Enclosure /Chassis Table (Type 3) for 
  SMBios. 
   
  Copyright (c) 2012-2013, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/09/13   vk      Add Type 3 from cfg file
 02/14/13   niting  Fixed OemDefined field length
 07/09/12   yg      Cfg file override Code generated values
 03/23/12   kpa     Add support for config file based smbios table init
 03/07/12   kedara  Initial revision

=============================================================================*/

#include <Uefi.h>

#include <Protocol/Smbios.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <SmbiosCfgInfo.h>

#pragma pack(1)

typedef struct _SMBIOS_SYSTEM_ENCLOSURE_INFO
{
  EFI_SMBIOS_TABLE_HEADER  Header;
  UINT8                    ManufacturerStringNum;
  UINT8                    Type;
  UINT8                    VersionStringNum;
  UINT8                    SerialNumberStringNum;
  UINT8                    AssetTagNumberStringNum;
  UINT8                    BootUpState;
  UINT8                    PowerSupplyState;
  UINT8                    ThermalState;
  UINT8                    SecurityStatus;
  UINT32                   OemDefined;
  UINT8                    Height;
  UINT8                    NumberOfPowerCords;
  UINT8                    ContainedElementCount;
  UINT8                    ContainedElementRecordLength;
  UINT8                    ContainedElements; 

  UINT8                    String1[2];
  UINT8                    String2[2];
  UINT8                    String3[2];
  UINT8                    String4[2];
  UINT8                    EndNull;
}SMBIOS_SYSTEM_ENCLOSURE_INFO;

typedef struct _SYS_ENCLOSURE_CHASSIS_STRING_STRUCTURE{
  UINT8                    String1[2];
  UINT8                    String2[2];
  UINT8                    String3[2];
  UINT8                    String4[2];
  UINT8                    EndNull;
}SYSTEM_ENCLOSURE_STRING_STRUCTURE;

SMBIOS_SYSTEM_ENCLOSURE_INFO smbiosSysEnclosureInfo = 
{
  {
    EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE,
    sizeof(SMBIOS_SYSTEM_ENCLOSURE_INFO) - sizeof(SYSTEM_ENCLOSURE_STRING_STRUCTURE),
    0 //handle
  },
  1,                       // *** String 1 - SysEnclosureInfoManufacturerString - can specify a string here for reference design
  8,                       // Type of Chassis/Sys Enclosure: default 0x8: portable 
  2,                       // ***String 2:  Version  Number, SysEnclosureInfoVersionString. 
  3,                       // ***String 3:  Serial Number,  SysEnclosureInfoSerialNumberString
  4,                       // ***String 4:  Asset Tag Number,  SysEnclosureInfoAssetTagNumString
  1,                       // BootUpState, value 1: Other
  1,                       // PowerSupplyState, value 1: Other
  1,                       // ThermalState, value 1: Other
  3,                       // SecurityStatus, value 3: None. Identifies the enclosure’s physical
                           //  security status when last booted
  0,                       // OemDefined, Contains OEM- or BIOS vendor-specific information
  0,                       // Height; 
  0,                       // NumberOfPowerCords. Identifies the number of power cords associated
                           //  with the enclosure or chassis. A value of 00h indicates that  number is unspecified.
  0,                       // ContainedElementCount: Identifies the number of Contained Element records
                           //  that follow, in the range 0 to 255
  0,                       // ContainedElementRecordLength
  0,                       // ContainedElements

  "1",
  "2",
  "3",
  "4",
  0
};


#pragma pack()

UINTN  SysEnclosureInfoManufacturerStringNum = 1;
CHAR8*  SysEnclosureInfoManufacturerString = (CHAR8*)PcdGetPtr(PcdSystemMfrStr);

UINTN  SysEnclosureInfoVersionStringNum = 2;
CHAR8*  SysEnclosureInfoVersionString = "0";   // defaulting to 0. 

UINTN  SysEnclosureInfoSerialNumberStringNum = 3;
CHAR8*  SysEnclosureInfoSerialNumberString= "0";  // defaulting to 0. 

UINTN  SysEnclosureInfoAssetTagNumStringNum = 4;
CHAR8*  SysEnclosureInfoAssetTagNumString = "0";  // defaulting to 0. 

/**
  Add System Enclosure  Info /Type 3 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SysEnclosureTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 3 /System Enclosure table.
  SmbiosTableNode->TablePtr = (VOID *) &smbiosSysEnclosureInfo; 
  SmbiosTableNode->TableType = EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE; 
  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS UpdateSysEnclosureInfoTable (EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                Status;
  EFI_SMBIOS_HANDLE         SysEnclosureHandle = 0;  

  if(Smbios == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }
 /* NOTE: Added in SMBIOS.CFG */ 
#if 0
  //Add the System Enclosure / Chassis Info ( Type3)  table
  Status = Smbios->Add(Smbios, NULL, &SysEnclosureHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbiosSysEnclosureInfo);
  if(Status != EFI_SUCCESS)
    return Status;
  
  // Update Manufacturer 
  Status = Smbios->UpdateString(Smbios,  
                                &SysEnclosureHandle,  
                                &SysEnclosureInfoManufacturerStringNum, 
                                SysEnclosureInfoManufacturerString); 
  if(Status != EFI_SUCCESS)
    return Status;

  // Update Version Number
  Status = Smbios->UpdateString(Smbios,  
                                &SysEnclosureHandle,  
                                &SysEnclosureInfoVersionStringNum, 
                                SysEnclosureInfoVersionString); 
  if(Status != EFI_SUCCESS)
    return Status;

  // Update Serial Number
  Status = Smbios->UpdateString(Smbios,  
                                &SysEnclosureHandle,  
                                &SysEnclosureInfoSerialNumberStringNum, 
                                SysEnclosureInfoSerialNumberString); 
  if(Status != EFI_SUCCESS)
    return Status;

  // Update Asset Tag Number
  Status = Smbios->UpdateString(Smbios,  
                                &SysEnclosureHandle,  
                                &SysEnclosureInfoAssetTagNumStringNum, 
                                SysEnclosureInfoAssetTagNumString); 
  if(Status != EFI_SUCCESS)
    return Status;
#endif
  // Store table handle and Update strings if any.
  // Cfg file overrides the code generated data since OEM's own this table
  Status = UpdateTableData(Smbios, SysEnclosureHandle, EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE);

  return Status;
}


