/** @file
  Initializes SMBios table.

  Copyright (c) 2011,2012, 2014-2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/05/16   bh      Update file system call after API change
 04/23/15   vk      Add UFS support
 10/16/14   vk      LoadFileFromPartition signature update
 09/11/12   niting  Fix status of init
 07/09/12   yg      Add filesize param
 04/25/12   yg      Revert Type 3 table removal 
 04/24/12   yg      Remove System Enclosure table
 03/20/12   kedara  Support config file based table loading.
 03/07/12   kedara  Added support for System Enclosure (Type 3) table
 05/25/11   niting  Initial revision

=============================================================================*/

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/Smbios.h>

#include <Library/QcomLib.h>
#include <Library/EfiFileLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootConfig.h>

#define  CONFIG_FILE_PATH                         L"\\SMBIOS\\SMBIOS.CFG"
#include <SmbiosCfgInfo.h>


EFI_STATUS UpdateSysEnclosureInfoTable( EFI_SMBIOS_PROTOCOL     *Smbios);
EFI_STATUS UpdateBiosInfoTable( EFI_SMBIOS_PROTOCOL     *Smbios);
EFI_STATUS UpdateProcessorInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios);
EFI_STATUS UpdateSystemInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios);
EFI_STATUS UpdateMemoryArrayInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios);
EFI_STATUS UpdateCacheInfoTable (EFI_SMBIOS_PROTOCOL     *Smbios);

//Root Partition
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiUfsLU4Guid;

//Partition Types
extern EFI_GUID gEfiPlatPartitionTypeGuid;

/**
  Set up the SMBios table

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
SmBiosTableDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_SMBIOS_PROTOCOL     *Smbios;
  EFI_STATUS              Status;
  UINT8                   *SmbiosCfgBuffPtr = NULL;
  UINTN                   FileSize = 0;
  UINT32                  BlkIOAttrib = 0;
  
  // Pointer to list of smbios tables to be added
  SMBIOS_TABLE_NODE *SmbiosListHeadPtr;
  
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios);

  ASSERT_EFI_ERROR (Status);

  // Initialize Smbios Table List struct
  SmbiosConfigInit(&SmbiosListHeadPtr);
  
  // Set attributes for fetching Block IO handle
  BlkIOAttrib = BLK_IO_SEL_PARTITIONED_GPT;
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
  BlkIOAttrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;
  
  // Load Smbios file into buffer

  if (boot_from_emmc())
  {
    //Attempt to load from Plat partition in eMMC GPP
    Status = LoadFileFromPartition(BlkIOAttrib,
                     CONFIG_FILE_PATH,
                     &gEfiEmmcGppPartition1Guid,
                     &gEfiPlatPartitionTypeGuid,
                     NULL,
                     &SmbiosCfgBuffPtr,
                     &FileSize
                     );
    //Attempt to load from User Partition
    if (Status != EFI_SUCCESS)
    {
      Status = LoadFileFromPartition(BlkIOAttrib,
                       CONFIG_FILE_PATH,
                       &gEfiEmmcUserPartitionGuid,
                       &gEfiPlatPartitionTypeGuid,
                       NULL,
                       &SmbiosCfgBuffPtr,
                       &FileSize
                       );
    }
  }
  else if (boot_from_ufs())
  {
    Status = LoadFileFromPartition(BlkIOAttrib,
                     CONFIG_FILE_PATH,
                     &gEfiUfsLU4Guid,
                     &gEfiPlatPartitionTypeGuid,
                     NULL,
                     &SmbiosCfgBuffPtr,
                     &FileSize
                     );
  }

  if (Status == EFI_SUCCESS)
  {
    // Parse config file and update tables if it exists.
    UpdateCfgBasedInfoTable(SmbiosCfgBuffPtr, FileSize, SmbiosListHeadPtr);
    AddSmbiosTables(Smbios,SmbiosListHeadPtr);
  }

  Status = EFI_SUCCESS;
  
  // Override Config file changes if any with preset values.
  UpdateBiosInfoTable(Smbios);
  UpdateCacheInfoTable(Smbios);
  UpdateProcessorInfoTable(Smbios);
  UpdateSystemInfoTable(Smbios);
  UpdateMemoryArrayInfoTable(Smbios);
  UpdateSysEnclosureInfoTable(Smbios);

  return Status;
}
