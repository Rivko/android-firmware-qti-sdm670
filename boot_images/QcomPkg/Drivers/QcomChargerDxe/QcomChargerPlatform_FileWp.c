/** @file QcomChargerPlatform_FileWp.c

  This file Reads Battery Profile for fule gauge

  Copyright (c) 2016-2017,  Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when         who     what, where, why
 --------     ---     -----------------------------------------------------------
 01/30/17     sm      Config file feature using LogFS instead of EFIESP
 01/27/17     ai      Fix use of retail
 10/26/16     va      Mount EFIESP partition for config file feature
 08/26/16     va      Read using partition label and read from efiesp for debug
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


#define BATTERY_PROVISION_PATH    L"\\PROVISION\\BATTERY.PROVISION"
#define BATTERY_PROVISION_DEBUG_PATH  L"\\BATTERY.PROVISION"


/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

EFI_STATUS ChargerPlatformFile_GetFileBuffer(CHAR16 *pFilePath, UINT8 **pFileBuffer, UINTN *pDataSize)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_GUID   *RootDeviceType = NULL;
  EFI_GUID   *PartitionType  = NULL;
  UINT32      BlkIOAttrib    = 0;

  /* Load Battery Profile file from Plat Provision Dir */
  /* Set attributes for fetching Block IO handle */
  BlkIOAttrib = BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;
  BlkIOAttrib |= BLK_IO_SEL_PARTITIONED_GPT;

  BlkIOAttrib |= BLK_IO_SEL_MATCH_PARTITION_LABEL;

  BlkIOAttrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  /* Load File from Plat\Provision Paritition */
  Status = LoadFileFromPartition(BlkIOAttrib,
                                 pFilePath,
                                 RootDeviceType,
                                 PartitionType,
                                 L"PLAT",
                                 pFileBuffer, pDataSize);

  if ((Status != EFI_SUCCESS ) || (NULL == pFileBuffer))
  {
    CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Could not open File = %r Path %a \r\n",__FUNCTION__, Status, pFilePath));
    return EFI_LOAD_ERROR;
  }

  return Status;

}


EFI_STATUS ChargerPlatformFile_ReadBattProfileFile( QCOM_CHARGER_PLATFORM_BATT_PROFILE_DATA *pBattProfileInfo)
{
  EFI_STATUS  Status            = EFI_SUCCESS; /*, DebugStatus = EFI_SUCCESS;
  UINT8      *pFileBufferEfiEsp = NULL;
  UINTN      FileSizeEfiEsp    = 0; */

  if(!pBattProfileInfo)
    return EFI_INVALID_PARAMETER;

  Status = ChargerPlatformFile_GetFileBuffer(BATTERY_PROVISION_PATH, &(pBattProfileInfo->pFileBuffer), &(pBattProfileInfo->DataSize));
  if (Status != EFI_SUCCESS )
  {
    CHARGER_FILE_UART_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a Could not open File = %r Path %a \r\n",__FUNCTION__, Status, BATTERY_PROVISION_PATH));
    return EFI_LOAD_ERROR;
  }
#if 0 /* TBD after discussion with test and CE folks if they need below debug feature */
  if (!RETAIL)
  {
    DebugStatus = ChargerPlatformFile_DebugReadFile(BATTERY_PROVISION_DEBUG_PATH, &pFileBufferEfiEsp, &FileSizeEfiEsp);
    if((EFI_SUCCESS == DebugStatus ) && (NULL != pFileBufferEfiEsp))
    {
      /*  Release Plat Partition loaded buffer */
      if(NULL != pBattProfileInfo->pFileBuffer)
      {
        CHARGER_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Releasing Plat file buffer \r\n", __FUNCTION__));
        FreePool(pBattProfileInfo->pFileBuffer);
      }
      pBattProfileInfo->pFileBuffer = NULL;
      pBattProfileInfo->DataSize = 0;

      pBattProfileInfo->pFileBuffer = pFileBufferEfiEsp;
      pBattProfileInfo->DataSize    = FileSizeEfiEsp;
      CHARGER_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a  FG Battery Profile overwriting from EfiEsp %r Size = %d \r\n", __FUNCTION__, DebugStatus, FileSizeEfiEsp));
    }
  }
#endif
  return Status;
}



EFI_STATUS ChargerPlatformFile_DebugReadFile(CHAR16 *pFilePath, UINT8 **pFileBuffer, UINTN *pDataSize)
{
  EFI_STATUS DebugStatus = EFI_SUCCESS;
  EFI_GUID   *RootDeviceType = NULL;
  EFI_GUID   *PartitionType  = NULL;
  UINT32      BlkIOAttrib    = 0;

  if(!pFilePath || !pFileBuffer)
    return EFI_INVALID_PARAMETER;

  /* Attempt to Read battery profile from LogFs partition to over write profile - supported for debug mode */

  BlkIOAttrib = BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  BlkIOAttrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;
  BlkIOAttrib |= BLK_IO_SEL_PARTITIONED_GPT;

  BlkIOAttrib |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
  BlkIOAttrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  /* Load File from LogFs Paritition */
  DebugStatus = LoadFileFromPartition(BlkIOAttrib,
                                 pFilePath,
                                 RootDeviceType,
                                 PartitionType,
                                 L"logfs",
                                 pFileBuffer, pDataSize);
  if(DebugStatus == EFI_NO_MEDIA)
  {
     DebugStatus = MountFatPartition(L"logfs");
     CHARGER_DEBUG((EFI_D_WARN, "QcomChargerDxe:: %a LogFs mount Status = %r \r\n", __FUNCTION__, DebugStatus));
       /* Load File from LogFs Paritition */
     DebugStatus = LoadFileFromPartition(BlkIOAttrib,
                                 pFilePath,
                                 RootDeviceType,
                                 PartitionType,
                                    L"logfs",
                                 pFileBuffer, pDataSize);

   }
  if((EFI_SUCCESS == DebugStatus ) && (NULL != pDataSize))
  {
     CHARGER_FILE_UART_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a read debug file success, size = %d \r\n", __FUNCTION__, *pDataSize));
  }
  else
  {
     CHARGER_DEBUG(( EFI_D_WARN, "QcomChargerDxe:: %a read debug file failed %r \r\n", __FUNCTION__, DebugStatus));
  }

  return DebugStatus;
}



