/** @file QcomChargerPlatform_FileLa.c

  This file reads Battery Profile file .

  Copyright (c) 2016-2017,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 01/27/17     ai      Fix use of retail
 09/20/16     va      Mount LogFS and Search file by partition label 
 08/14/16     va      Adding log and fixing bug for mass storage feature
 07/15/16     cs      Adding Wipower support
 07/05/16     bh      Update file system call after API change
 05/13/16     va      Initial revision

=============================================================================*/
#include <Uefi.h>

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ParserLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/FuseControlLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileInfo.h>

#include <api/pmic/pm/pm_uefi.h>

/**
Protocol Dependencies
*/
#include <Protocol/EFIPlatformInfo.h>

#include "DDIPlatformInfo.h"
#include <Protocol/SimpleFileSystem.h>

/**
Charger headers
*/

#include "QcomChargerPlatform_File.h"
#include "QcomChargerPlatform_FileCommon.h"

#include "QcomChargerPlatform.h"

/*===========================================================================
  FUNCTION DECLARATION
===========================================================================*/
EFI_STATUS ChargerPlatformFileLa_DebugReadFile(CHAR16 *pFilePath, UINT8 *pFileBuffer, UINTN *pDataSize);

/* LogFS Partition GUID for Config Parameters */
extern EFI_GUID gEfiLogFSPartitionGuid;


#define BATTERY_PROVISION_PATH        L"BATTERY.PROVISION"
#define BATTERY_PROVISION_DEBUG_PATH  L"\\BATTERY.PROVISION"


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

EFI_STATUS ChargerPlatformFile_ReadBattProfileFile(QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo)
{
  EFI_STATUS  Status = EFI_SUCCESS, DebugStatus = EFI_SUCCESS;
  UINTN       FileSizeLogFs    = 0;
  UINT8      *pFileBufferLogFs = NULL;

  if(!pBattProfileInfo)
    return EFI_INVALID_PARAMETER;

  /* Attempt to Read battery profile from main FV */
  Status = ReadFromFV(BATTERY_PROVISION_PATH, (void **) &(pBattProfileInfo->pFileBuffer), &(pBattProfileInfo->DataSize));
  if (Status != EFI_SUCCESS)
  {
    CHARGER_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  Battery Profile File Open failed %r \r\n", __FUNCTION__, Status));
  }

  /* Attempt to Read battery profile from LogFS partition to over write profile - supported for debug mode */
  if (!RETAIL) 
  {
    /* Read from LogFS Partition */
    DebugStatus = ChargerPlatformFile_DebugReadFile(BATTERY_PROVISION_DEBUG_PATH, &pFileBufferLogFs, &FileSizeLogFs);
    if((EFI_SUCCESS == DebugStatus )&& (NULL != pFileBufferLogFs))
    {
      /*  Release FV loaded data */
      if(NULL != pBattProfileInfo->pFileBuffer)
        FreePool(pBattProfileInfo->pFileBuffer);
      pBattProfileInfo->pFileBuffer = NULL;
      pBattProfileInfo->DataSize = 0;

      pBattProfileInfo->pFileBuffer = pFileBufferLogFs;
      pBattProfileInfo->DataSize    = FileSizeLogFs;
      CHARGER_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Profile overwriting from LogFS %r Size = %d \r\n", __FUNCTION__, DebugStatus, FileSizeLogFs));
    }
    else
    {
      CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Profile overwriting Failed %r \r\n", __FUNCTION__, DebugStatus));
    }
  }

  return Status;
}


EFI_STATUS ChargerPlatformFile_DebugReadFile(CHAR16 *pFilePath, UINT8 **pFileBuffer, UINTN *pDataSize)
{
  EFI_STATUS  Status         = EFI_SUCCESS;
  EFI_GUID   *RootDeviceType = NULL;
  EFI_GUID   *PartitionType  = NULL;
  UINT32      BlkIOAttrib    = 0;

  /* Attempt to load Charger Config file from LogFS parition */
  /* Set attributes for fetching Block IO handle */
  BlkIOAttrib = BLK_IO_SEL_MATCH_PARTITION_LABEL;
  BlkIOAttrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;
  BlkIOAttrib |= BLK_IO_SEL_PARTITIONED_GPT;
  BlkIOAttrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  /* Load File from LogFs Paritition */
  Status = LoadFileFromPartition(BlkIOAttrib,
                                 pFilePath,
                                 RootDeviceType,
                                 PartitionType,
                                 L"logfs",
                                 pFileBuffer, pDataSize);
  if(Status == EFI_NO_MEDIA)
  {
    if (!RETAIL) 
    {
      Status = MountFatPartition(L"logfs");
      CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a LogFS mount Status = %r \r\n", __FUNCTION__, Status));
      /* Load File from LogFs Paritition */
      Status = LoadFileFromPartition(BlkIOAttrib,
                                     pFilePath,
                                     RootDeviceType,
                                     PartitionType,
                                     L"logfs",
                                     pFileBuffer, pDataSize);
    }
  }
  if((Status != EFI_SUCCESS ) || (NULL == pFileBuffer))
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Could not open Battery Profile File = %r \r\n",__FUNCTION__, Status));
    return EFI_LOAD_ERROR;
  }

  CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Battery Profile Read Success = %r, DataSize = %d \r\n", __FUNCTION__, Status, *pDataSize));

  return Status;
}


