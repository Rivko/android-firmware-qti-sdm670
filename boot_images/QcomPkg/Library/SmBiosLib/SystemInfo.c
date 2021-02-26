/** @file
  Updates the SMBios table with the system info structure. 
   
  Copyright (c) 2010-2013, 2016 Qualcomm Technologies Inc. All rights reserved.
  
**/
/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 12/07/16   lg      Remove duplicated code and call GetUUID() in SystemInfoTableInit()
 09/13/16   jt      Change SdccCardInfo to MemCardInfo 
 01/30/13   vk      Fix warning
 08/17/12   yg      Allow Cfg override UUID in SystemInfo
 06/06/12   yg      Cfg data overrides the code generated data for SystemInfo
 04/26/12   yg      Populate appropriate SMBios values
 03/23/12   kpa     Add support for config file based smbios table init
 07/20/11   yg      Added UUID based on eMMC info
 05/25/11   niting  Initial revision
=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Smbios.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/QcomLib.h>
#include <SmbiosCfgInfo.h>

#pragma pack(1)

#define QCOM_PLAT_INVALID    0
#define QCOM_PLAT_MAX        8

typedef struct _SMBIOS_SYSTEM_INFO
{
  EFI_SMBIOS_TABLE_HEADER  Header;
  UINT8                    ManufacturerStringNum;
  UINT8                    ProductNameStringNum;
  UINT8                    VersionStringNum;
  UINT8                    SerialNumberStringNum;
  UINT8                    UUID[16];
  UINT8                    WakeUpType;
  UINT8                    SkuNumberStringNum;
  UINT8                    FamilyStringNum;

  UINT8                    String1[2];
  UINT8                    String2[2];
  UINT8                    String3[2];
  UINT8                    String4[2];
  UINT8                    String5[2];
  UINT8                    String6[2];
  UINT8                    EndNull;
}SMBIOS_SYSTEM_INFO;

typedef struct _SYSTEM_STRING_STRUCTURE{
  UINT8                    String1[2];
  UINT8                    String2[2];
  UINT8                    String3[2];
  UINT8                    String4[2];
  UINT8                    String5[2];
  UINT8                    String6[2];
  UINT8                    EndNull;
}SYSTEM_STRING_STRUCTURE;

SMBIOS_SYSTEM_INFO smbiosSystemInfo = 
{
  {
    EFI_SMBIOS_TYPE_SYSTEM_INFORMATION,
    sizeof(SMBIOS_SYSTEM_INFO) - sizeof(SYSTEM_STRING_STRUCTURE),
    0 //handle
  },

  1,                                    // ManufacturerStringNum
  2,                                    // ProductNameStringNum;
  3,                                    // VersionStringNum
  5,                                    // SerialNumberStringNum;
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},    // UUID[16];
  0x6,                                  // WakeUpType 6 = power switch;
  6,                                    // SkuNumberStringNum;
  4,                                    // FamilyStringNum;

  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  0
};

#pragma pack()

UINTN  SystemInfoManufacturerStringNum = 1;
CHAR8* SystemInfoManufacturerString = (CHAR8*)PcdGetPtr(PcdSystemMfrStr);

UINTN  SystemInfoProductNameStringNum = 2;
UINTN  SystemInfoProductVersionStringNum = 3;
UINTN  SystemInfoFamilyNameStringNum = 4;

MEM_CARD_INFO          CardInfoData;


/**
  Add System Info/ Type 1 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS SystemInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 1 /System info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbiosSystemInfo ; 
  SmbiosTableNode->TableType = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION; 
  
  Status = GetUUID(&smbiosSystemInfo.UUID[0], 16);

  return EFI_SUCCESS;
}

EFI_STATUS UpdateSystemInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios)
{
  EFI_STATUS              Status;
  EFI_SMBIOS_HANDLE       SystemHandle = 0;  
  CHAR8                   StrBuffer[64];
  CHAR8                  *PlatformTypePtr, *ChipNamePtr, *ChipVerPtr;

  //Add the System Info table
  Status = Smbios->Add(Smbios, NULL, &SystemHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbiosSystemInfo);
  if(Status != EFI_SUCCESS)
    return Status;

  Status = Smbios->UpdateString(Smbios,  
                                &SystemHandle,  
                                &SystemInfoManufacturerStringNum, 
                                SystemInfoManufacturerString); 
  if(Status != EFI_SUCCESS)
    return Status;

  Status = GetPlatformStrings(StrBuffer, sizeof(StrBuffer),
                      &PlatformTypePtr, &ChipNamePtr, &ChipVerPtr);

  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to get Platform strings\n"));
    return Status;
  }

  Status =	Smbios->UpdateString(Smbios,  
                                 &SystemHandle,  
                                 &SystemInfoProductNameStringNum, 
                                 PlatformTypePtr); 
  if(Status != EFI_SUCCESS)
    return Status;

  Status =	Smbios->UpdateString(Smbios,  
                                 &SystemHandle,  
                                 &SystemInfoFamilyNameStringNum, 
                                 ChipNamePtr); 
  if(Status != EFI_SUCCESS)
    return Status;

  Status =	Smbios->UpdateString(Smbios,  
                                 &SystemHandle,  
                                 &SystemInfoProductVersionStringNum, 
                                 ChipVerPtr); 

  // Update the data/strings from the cfg here so that OEM's can update
  // system info since they own this data. Only for this case CFG data
  // overrides the code generated data
  UpdateTableData(Smbios, SystemHandle, EFI_SMBIOS_TYPE_SYSTEM_INFORMATION );

  return Status;
}


