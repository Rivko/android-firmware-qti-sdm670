/** @file
  Updates the SMBios table with the cache info structure.
   
  Copyright (c) 2010-2013, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/30/13   vk      Fix warning
 03/23/12   kpa     Add support for config file based smbios table init
 07/20/11   yg      Converted to Lib
 05/25/11   niting  Initial revision

=============================================================================*/

#include <Uefi.h>

#include <Protocol/Smbios.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <SmbiosCfgInfo.h>

#pragma pack(1)

typedef struct _SMBIOS_CACHE_INFO
{
  EFI_SMBIOS_TABLE_HEADER  Header;
  UINT8                    SocketDesignationStringNum;
  UINT16                   CacheConfiguration;
  UINT16                   MaxSize;
  UINT16                   InstalledSize;
  UINT16                   SupportedSRAMType; 
  UINT16                   CurrentSRAMType; 
  UINT8                    CacheSpeed;
  UINT8                    ErrorCorrectionType;
  UINT8                    SystemCacheType;
  UINT8                    Associativity;

  UINT8                    String1[2];
  UINT8                    EndNull;
}SMBIOS_CACHE_INFO;

typedef struct _CACHE_STRING_STRUCTURE{
  UINT8                    String1[2];
  UINT8                    EndNull;
}CACHE_STRING_STRUCTURE;

SMBIOS_CACHE_INFO smbios_L1InstructionCacheInfo = 
{
  {
    EFI_SMBIOS_TYPE_CACHE_INFORMATION,
    sizeof(SMBIOS_CACHE_INFO) - sizeof(CACHE_STRING_STRUCTURE),
    0 //handle
  },

  1,                       // ** String SocketDesignationStringNum;
  0x280,                   // CacheConfiguration;
  0x180,                   // MaxSize (384 kB) Total L1-I across all cores
  0x180,                   // InstalledSize (384 kB)Total L1-I across all core
  0,                       // SupportedSRAMType (other) 
  0,                       // CurrentSRAMType (other) 
  0,                       // CacheSpeed (unknown)
  0x4,                     // ErrorCorrectionType (parity)
  0x3,                     // SystemCacheType (instruction)
  0x8,                     // Associativity (16 way)

  "1",
  0
};


SMBIOS_CACHE_INFO smbios_L1DataCacheInfo = 
{
  {
    EFI_SMBIOS_TYPE_CACHE_INFORMATION,
    sizeof(SMBIOS_CACHE_INFO) - sizeof(CACHE_STRING_STRUCTURE),
    0 //handle
  },

  1,                       // ** String SocketDesignationStringNum;
  0x280,                   // CacheConfiguration;
  0x180,                    // MaxSize (384 kB) Total L1-D across all core
  0x180,                    // InstalledSize (384 kB) Total L1-D across all core
  0,                       // SupportedSRAMType (other) 
  0,                       // CurrentSRAMType (other) 
  0,                       // CacheSpeed (unknown)
  0x4,                     // ErrorCorrectionType (parity)
  0x4,                     // SystemCacheType (data)
  0x8,                     // Associativity (16 way)

  "1",
  0
};

SMBIOS_CACHE_INFO smbios_L2CacheInfo = 
{
  {
    EFI_SMBIOS_TYPE_CACHE_INFORMATION,
    sizeof(SMBIOS_CACHE_INFO) - sizeof(CACHE_STRING_STRUCTURE),
    0 //handle
  },

  1,                       // ** String SocketDesignationStringNum;
  0x281,                   // CacheConfiguration;
  0x500,                  // MaxSize (1280 kB)Total L2 across all core
  0x500,                  // InstalledSize (1280 kB)Total L2 across all core
  0,                       // SupportedSRAMType (other) 
  0,                       // CurrentSRAMType (other) 
  0,                       // CacheSpeed (unknown)
  0x4,                     // ErrorCorrectionType (parity)
  0x5,                     // SystemCacheType (unified)
  0x7,                     // Associativity (8 way)

  "1",
  0
};

SMBIOS_CACHE_INFO smbios_L3CacheInfo = 
{
  {
    EFI_SMBIOS_TYPE_CACHE_INFORMATION,
    sizeof(SMBIOS_CACHE_INFO) - sizeof(CACHE_STRING_STRUCTURE),
    0 //handle
  },

  1,                       // ** String SocketDesignationStringNum;
  0x282,                   // CacheConfiguration;
  0x800,                  // MaxSize (1280 kB)Total L2 across all core
  0x800,                  // InstalledSize (1280 kB)Total L2 across all core
  0,                       // SupportedSRAMType (other) 
  0,                       // CurrentSRAMType (other) 
  0,                       // CacheSpeed (unknown)
  0x4,                     // ErrorCorrectionType (parity)
  0x5,                     // SystemCacheType (unified)
  0x7,                     // Associativity (8 way)

  "1",
  0
};

#pragma pack()

UINTN  CacheInfoL1InstStringNum = 1;
CHAR8*  CacheInfoL1InstString = "L1 Instruction Cache";

UINTN  CacheInfoL1DataStringNum = 1;
CHAR8*  CacheInfoL1DataString = "L1 Data Cache";

UINTN  CacheInfoL2StringNum = 1;
CHAR8*  CacheInfoL2String = "L2 Cache";

UINTN  CacheInfoL3StringNum = 1;
CHAR8*  CacheInfoL3String = "L3 Cache";


EFI_SMBIOS_HANDLE       L1InstCacheHandle = 0;  
EFI_SMBIOS_HANDLE       L1DataCacheHandle = 0;  
EFI_SMBIOS_HANDLE       L2CacheHandle = 0;  
EFI_SMBIOS_HANDLE       L3CacheHandle = 0;  


/**
  Add L1 Instruction cache Info /Type 7 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success

**/
EFI_STATUS L1InstCacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;
     
  // Add type 0 /Bios info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbios_L1InstructionCacheInfo ; 
  SmbiosTableNode->TableType = SMBIOS_L1_INSTN_CACHE_TABLE; 
  Status = EFI_SUCCESS;

  return Status;
}

/**
  Add L1 Data cache Info /Type 7 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success

**/
EFI_STATUS L1DataCacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;
    
  // Add type 0 /Bios info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbios_L1DataCacheInfo ; 
  SmbiosTableNode->TableType = SMBIOS_L1_DATA_CACHE_TABLE; 
  Status = EFI_SUCCESS;

  return Status;
}

/**
  Add L2 cache Info /Type 7 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success

**/
EFI_STATUS L2CacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 0 /Bios info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbios_L2CacheInfo ; 
  SmbiosTableNode->TableType = SMBIOS_L2_CACHE_TABLE; 
  Status = EFI_SUCCESS;

  return Status;
}

/**
  Add L3 cache Info /Type 7 table to Smbios table List

  @param  SmbiosTableNode   Pointer to the Smbios Table list 
                            node

  @retval EFI_SUCCESS    Initialization success

**/
EFI_STATUS L3CacheInfoTableInit( SMBIOS_TABLE_NODE     *SmbiosTableNode)
{
  EFI_STATUS       Status = EFI_INVALID_PARAMETER ;

  if( SmbiosTableNode == NULL)
    return Status;

  // Add type 7 /Bios info table.
  SmbiosTableNode->TablePtr= (VOID *) &smbios_L3CacheInfo ; 
  SmbiosTableNode->TableType = SMBIOS_L3_CACHE_TABLE; 
  Status = EFI_SUCCESS;

  return Status;
}
EFI_STATUS UpdateCacheInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios)
{
  EFI_STATUS              Status;

  //Add the L1 Instruction Cache Info 
  Status = Smbios->Add(Smbios, NULL, &L1InstCacheHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbios_L1InstructionCacheInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }
  
  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, L1InstCacheHandle, SMBIOS_L1_INSTN_CACHE_TABLE );

  Status = Smbios->UpdateString(Smbios,  
                                &L1InstCacheHandle,  
                                &CacheInfoL1InstStringNum, 
                                CacheInfoL1InstString); 
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }

  //Add the L1 Data Cache Info 
  Status = Smbios->Add(Smbios, NULL, &L1DataCacheHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbios_L1DataCacheInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }
  
  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, L1DataCacheHandle, SMBIOS_L1_DATA_CACHE_TABLE );

  Status = Smbios->UpdateString(Smbios,  
								&L1DataCacheHandle,  
								&CacheInfoL1DataStringNum, 
								CacheInfoL1DataString); 
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }


  //Add the L2 Cache Info 
  Status = Smbios->Add(Smbios, NULL, &L2CacheHandle, (EFI_SMBIOS_TABLE_HEADER*)&smbios_L2CacheInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }

  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, L2CacheHandle, SMBIOS_L2_CACHE_TABLE );

  Status =  Smbios->UpdateString(Smbios,  
					                  		 &L2CacheHandle,  
                                 &CacheInfoL2StringNum, 
                                 CacheInfoL2String); 

  if(Status != EFI_SUCCESS)
  {
    return Status;
  }

  //Add the L3 Cache Info 
  Status = Smbios->Add(Smbios, NULL, &L3CacheHandle,
  (EFI_SMBIOS_TABLE_HEADER*)&smbios_L3CacheInfo);
  if(Status != EFI_SUCCESS)
  {
    return Status;
  }
  // Store table handle and Update strings if any.
  UpdateTableData(Smbios, L3CacheHandle, SMBIOS_L3_CACHE_TABLE );

  Status =  Smbios->UpdateString(Smbios,  
					                  		 &L3CacheHandle,  
                                 &CacheInfoL3StringNum, 
                                 CacheInfoL3String); 

  return Status;
}


