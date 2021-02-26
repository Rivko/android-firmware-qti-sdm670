/** @file
  Updates the SMBios table with the memory array info structure.
   
  Copyright (c) 2010-2013,2016-2017 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/02/17   ltg     Modify MemoryType in smbiosMemoryDeviceInfo
                    Add smbiosMemoryArrayMappedAddressInfo struct
 11/26/13   vk      GetDDRInfo returns UINTN
 07/09/13   vk      Update DDR2 to DDR3
 01/30/13   vk      Fix warning
 07/26/12   vishalo Enable Type 17 table, add latest spec fields
 07/20/12   vishalo Disable adding Type 17 table
 03/23/12   kpa     Add support for config file based smbios table init
 11/23/11   jz      Use library routine to get memory size
 07/20/11   yg      Converted to Lib
 05/25/11   niting  Initial revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>

#include <Protocol/Smbios.h>

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RamPartitionTableLib.h>
#include <Library/QcomBaseLib.h>
#include <SmbiosCfgInfo.h>

#define UPPER_32_BITS_MASK 0xFFFFFFFF00000000

#pragma pack(1)

typedef struct _SMBIOS_MEMORY_ARRAY_INFO
{
  EFI_SMBIOS_TABLE_HEADER  Header;

  UINT8                    Location;
  UINT8                    Use;
  UINT8                    MemoryErrorCondition;
  UINT32                   MaximumCapacity;
  UINT16                   MemErrorInfoHandle;
  UINT16                   NumMemoryDevices;

  UINT8                    EndNull1;
  UINT8                    EndNull2;
}SMBIOS_MEMORY_ARRAY_INFO;

typedef struct _MEMORY_ENDNULL_STRUCTURE{
  UINT8                    EndNull1;
  UINT8                    EndNull2;
}MEMORY_ENDNULL_STRUCTURE;

typedef struct _SMBIOS_MEMORY_DEVICE_INFO
{
  EFI_SMBIOS_TABLE_HEADER  Header;

  UINT16                   PhysMemArrayHandle;
  UINT16                   MemErrorHandle;
  UINT16                   TotalWidth;
  UINT16                   DataWidth;
  UINT16                   Size;
  UINT8                    FormFactor;
  UINT8                    DeviceSet;
  UINT8                    DeviceLocationStringNum;
  UINT8                    BankLocationStringNum;
  UINT8                    MemoryType;
  UINT16                   TypeDetail;
  UINT16                   Speed;
  UINT8                    ManufacturerStringNum;
  UINT8                    SerialNumberStringNum;
  UINT8                    AssetTagStringNum;
  UINT8                    PartNumberStringNum;
  UINT8                    Attributes;
  UINT32                   ExtendedSize;
  UINT16                   ConfiguredMemoryClockSpeed;
 
 
  UINT8                    EndNull1;
  UINT8                    EndNull2;
}SMBIOS_MEMORY_DEVICE_INFO;

typedef struct _SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS {
  EFI_SMBIOS_TABLE_HEADER  Header;

  UINT32                   StartingAddress;
  UINT32                   EndingAddress;
  UINT16                   MemoryArrayHandle;
  UINT8                    PartitionWidth;
  UINT64                   ExtendedStartingAddress;
  UINT64                   ExtendedEndingAddress;

  UINT8                    EndNull1;
  UINT8                    EndNull2;
}SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS;

SMBIOS_MEMORY_ARRAY_INFO smbiosMemoryArrayInfo = 
{
  {
    EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY,
    sizeof(SMBIOS_MEMORY_ARRAY_INFO) - sizeof(MEMORY_ENDNULL_STRUCTURE),
    0 //handle
  },

  0x3,                     //  Location - 3 = on chip
  0x3,                     //  Use - 3 = System memory
  0x2,                     //  MemoryErrorCondition - 2 is unknown
  0,                       //  MaximumCapacity  *** Fill in at run time 
  0xFFFF,                  //  MemErrorInfoHandle;
  1,                       //  NumMemoryDevices;

  0,
  0
};

SMBIOS_MEMORY_DEVICE_INFO smbiosMemoryDeviceInfo = 
{
    {
      EFI_SMBIOS_TYPE_MEMORY_DEVICE,
      sizeof(SMBIOS_MEMORY_DEVICE_INFO) - sizeof(MEMORY_ENDNULL_STRUCTURE),
      0 //handle
    },

    0,                       // PhysMemArrayHandle  - Fill in at runtime ;
    0xFFFF,                  // MemErrorHandle;
    0xFFFF,                  // TotalWidth;
    0xFFFF,                  // DataWidth;
    0,                       // Size - Fill in at runtime
    0x0A,                    // FormFactor - 0x0A TSOP
    0,                       // DeviceSet - not part of a device set
    0,                       // DeviceLocationStringNum;
    0,                       // BankLocationStringNum;
    0x1E,                    // MemoryType - LPDDR4
    0,                       // TypeDetail;
    400,                     // Speed;
    0,                       // ManufacturerStringNum;
    0,                       // SerialNumberStringNum;
    0,                       // AssetTagStringNum;
    0,                       // PartNumberStringNum;
    0,                       // Attributes;
    0,                       // ExtendedSize;
    0,                       // ConfiguredMemoryClockSpeed;

    0,
    0
};

SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS smbiosMemoryArrayMappedAddressInfo = 
{
    {
      EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS,
      sizeof(SMBIOS_MEMORY_ARRAY_MAPPED_ADDRESS) - sizeof(MEMORY_ENDNULL_STRUCTURE),
      0 //handle
    },

    0xFFFFFFFF,              // StartingAddress
    0xFFFFFFFF,              // EndingAddress
    0xFFFF,                  // MemoryArrayHandle; - Fill in at runtime
    2,                       // PartitionWidth; - Fill in at runtime
    0,                       // Extended Starting Address - Fill in at runtime
    0,                       // Extended Ending Address - Fill in at runtime
    
    0,
    0
};

#pragma pack()

/**
  Add Memory Array Info /Type 16 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS MemoryArrayTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 16 /Memory Array info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbiosMemoryArrayInfo ; 
  SmbiosTableNode->TableType = EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY; 
  Status = EFI_SUCCESS;

  return Status;
}

/**
  Add Memory Device Info /Type 17 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS MemoryDeviceInfoInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 17 /Memory Device info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbiosMemoryDeviceInfo ; 
  SmbiosTableNode->TableType = EFI_SMBIOS_TYPE_MEMORY_DEVICE; 
  Status = EFI_SUCCESS;


  return Status;
}

/**
  Add Memory Array Mapped Address Info /Type 19 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success
  

**/
EFI_STATUS MemoryArrayMappedAddressInfoInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 19 /Memory Array Mapped Address info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbiosMemoryArrayMappedAddressInfo ; 
  SmbiosTableNode->TableType = EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS; 
  Status = EFI_SUCCESS;

  return Status;
}

EFI_STATUS UpdateMemoryArrayInfoTable (EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS              Status;
  EFI_SMBIOS_HANDLE       MemoryArrayHandle = 0;  
  EFI_SMBIOS_HANDLE       MemoryDeviceHandle = 0;  
  EFI_SMBIOS_HANDLE       MemoryArrayMappedAddressHandle = 0;  

  UINTN                   MemoryCapacity;
  UINTN                   StartAddress;

  /* Get available memory from partition table */
  InitRamPartitionTableLib();
  Status = GetTotalPhysicalMemory(&MemoryCapacity);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }

  Status = GetLowestPhysicalStartAddress(&StartAddress);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }

  // round mem capacity to next 64MB boundary
  if (MemoryCapacity & 0xFFFFFF){
    MemoryCapacity = (UINT64)(MemoryCapacity + 0x4000000) & 0xFFFFFFFFFC000000;
  }
  smbiosMemoryArrayInfo.MaximumCapacity = (UINT32) ((MemoryCapacity / (1024)));

  //Add the Memory Info 
  Status = Smbios->Add(Smbios, NULL, &MemoryArrayHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbiosMemoryArrayInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }
  
  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, MemoryArrayHandle, 
                                     EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY );

  //Add the Memory Device
  smbiosMemoryDeviceInfo.PhysMemArrayHandle = MemoryArrayHandle;
  smbiosMemoryDeviceInfo.Size = (UINT16)(MemoryCapacity / 1024 / 1024 );
  
  Status = Smbios->Add(Smbios, NULL, &MemoryDeviceHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbiosMemoryDeviceInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }
  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, MemoryDeviceHandle, 
                                     EFI_SMBIOS_TYPE_MEMORY_DEVICE );

  //Add the Memory Array Mapped Address
  smbiosMemoryArrayMappedAddressInfo.MemoryArrayHandle = MemoryArrayHandle;

  // if the starting or ending addresses are greater than the max 32-bit value, populate the extended address fields
  // otherwise, populate the regular address fields
  if ( (((UINT64) StartAddress) & UPPER_32_BITS_MASK) || (((UINT64) (StartAddress + MemoryCapacity - 1)) & UPPER_32_BITS_MASK) ){
    smbiosMemoryArrayMappedAddressInfo.ExtendedStartingAddress = (UINT64) StartAddress;
    smbiosMemoryArrayMappedAddressInfo.ExtendedEndingAddress = (UINT64) (StartAddress + MemoryCapacity - 1);
  }
  else{
    smbiosMemoryArrayMappedAddressInfo.StartingAddress = (UINT32) StartAddress;
    smbiosMemoryArrayMappedAddressInfo.EndingAddress = (UINT32) (StartAddress + MemoryCapacity - 1);
  }

  Status = Smbios->Add(Smbios, NULL, &MemoryArrayMappedAddressHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbiosMemoryArrayMappedAddressInfo);
  if(Status == EFI_SUCCESS)
  {
    // Store table handle and Update strings if any.
    UpdateTableData(Smbios, MemoryArrayMappedAddressHandle, 
                                     EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS );
  }
  return Status;

}

