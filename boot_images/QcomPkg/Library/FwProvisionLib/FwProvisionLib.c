/** @file FwProvisionLib.c
   
  Firmware provisioning functions used for factory provisioning for 
  Windows-on-Snapdragon (WoS) 
  
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
  Copyright (C) 2012 Apple, Inc. All rights reserved. 

  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

  **/

/*=============================================================================
                              EDIT HISTORY


when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
2015/04/15   mic       Added 64bit support
2015/03/22   mic       Updated log message
2014/12/08   mic       Removed fuse blowing support
2014/01/26   mic       Fixed KW warnings
2013/08/01   mic       Added WP provisioning support
2013/08/01   wuc       Updated HDCP 2.x provisioning
2013/05/23   rsb       Add support for HDCP 2.x provisioning
2013/04/09   rsb       Switch from RPMBProvisionNoPrompt to RPMBProvision
2012/12/11   mic       Fixed KW warnings
2012/10/08   mic       Moved common macro to FwCommonLib
2012/09/04   rsb       Add reset for provisioning operation
2012/08/10   mic       Changed debug message level for critical errors
2012/07/17   mic       Added ToolSupportProtocol for OPM handshake and removed 
                       ToolSupportLib
2012/07/13   mic       Added WinSecApp OPM handshake support
2012/07/09   mic       Re-organized debug message logic for privision flag check
2012/07/06   mic       Added OPM Provisioning support, removed ReadKeystoreFile function. 
2012/07/02   rsb       Removed EFI_INVALID_PARAMETER check from Debug Policy
                       provisioning since it is no longer required
2012/06/15   mic       Adjusted the return status check for the result of Debug 
                       Policy provisioning
2012/06/05   rs        Replaced AsciiPrints with DEBUG Macro
2012/05/07   jd        Migrated to DPP Protocol. Cleanup
2012/03/12   rs        Added SSD Provisioning changes
2012/02/26   jd        Added support for raw-partition based device provisioning  
2012/01/25   jd        Use a separate commit interface for FactoryTool provisioning
2011/09/09   jthompso  first revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFIEncryption.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/EFIDpp.h>
#include <Protocol/EFIToolSupport.h>
#include <Protocol/LoadedImage.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Guid/FileInfo.h>
#include <Guid/GlobalVariable.h>
#include <Guid/ImageAuthentication.h>
#include <Library/FwCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomLib.h>
#include "FwProvFuseLib.h"

#define FIRST_BLOCK 0

#define ON  1
#define OFF 0

/**
 * This switch should be turned off once device provisioning
 * solution is completely migrated to raw-partition based DPP.
 */
#define DPP_ON_GPP_FAT16    OFF

/**
 * This switch is for DPP test. Should be turned off once the 
 * feature is verified. 
 */
#define TEST_DPP            OFF

/**
 * This switch is for testing purposes to verify that HDCP 2.x
 * provisioning is working correctly. During provisioning step,
 * the encrypted data is read out from DPP, decrypted, and
 * compared against the original.
 *
 * This capability is not supported on production devices and must
 * be disabled.
 */
#define TEST_HDCP_KEY_PROVISION_WITH_DECRYPT    OFF

#define OA3_FILE_COPIES     1

typedef enum _PROV_KEY_TYPE
{
  TYPE_KEKPUB = 0,
  TYPE_SIGNATUREDB,
  TYPE_SIGNATUREDBX,
  TYPE_PKPUB,
} PROV_KEY_TYPE;

typedef enum _PROV_HDCPKEY_TYPE
{
  TYPE_HDCPCEK = 0,
  TYPE_HDCPDATA,
} PROV_HDCPKEY_TYPE;

UINT16   *SMBIOSFileNameSrc= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_SMBIOS_CFG_FILE_NAME;
UINT16   *SMBIOSFileNameDest= L"\\"SMBIOS_FOLDER_PATH"\\"PROVISIONING_SMBIOS_CFG_FILE_NAME;
UINT16   *KeyStoreFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_SSD_KEY_FILE_NAME;
UINT16   *KEKFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_KEKPUB_FILE_NAME;
UINT16   *DBFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_SIGNATUREDB_FILE_NAME;
UINT16   *DBXFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_SIGNATUREDBX_FILE_NAME;
UINT16   *PKFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_PKPUB_FILE_NAME;
/*OPM key names for writing to DPP*/
UINT16   *OPMpubFileName =  PROVISIONING_OPM_PUB_FILE_NAME;
UINT16   *OPMprivFileName=  PROVISIONING_OPM_PRIV_FILE_NAME;
/*OPM key paths for reading from ESP*/
UINT16   *OPMpubFileNamePath =  L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_OPM_PUB_FILE_NAME;
UINT16   *OPMprivFileNamePath =  L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_OPM_PRIV_FILE_NAME;

UINT16   *DebugPolicyFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_DEBUGPOLICY_FILE_NAME;

/*HDCP 2.x file paths for reading from EFI system partition */
UINT16   *HDCPCEKFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_HDCPCEK_FILE_NAME;
UINT16   *HDCPDataFileName= L"\\"DPP_FOLDER_PATH"\\"PROVISIONING_HDCPDATA_FILE_NAME;

static EFI_GUID RSDP_V2_guid = {0x8868E871, 0xE4F1, 0x11D3, {0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81}};
static EFI_GUID RSDP_V1_guid = {0xEB9D2D30, 0x2D88, 0x11D3, {0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D}};

static EFI_GUID DebugModeVariableGuid = {0x77fa9abd, 0x0359, 0x4d32, { 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b}};

static QCOM_FW_UPDATE_FACTORY_PROVISIONED_FLAG_VALUE FactoryProvisionedFlag;
static PROVISIONING_COMMIT_FILE_TYPE FactoryFlag;
static PROVISIONING_RESULT_FILE_TYPE FactoryResult;

const QCOM_FW_UPDATE_COPYFWFILE_ARGUMENTS OA3ProvisioningCopyList[OA3_FILE_COPIES] = 
{
    {QCOM_FW_UPDATE_OA3_MSDM_ORIG_FILE,         QCOM_FW_UPDATE_PARTITION_USER_ESP,  QCOM_FW_UPDATE_PARTITION_GPP_FAT16, QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE}
};


const QCOM_FW_UPDATE_COPYFWFILE_ARGUMENTS DeviceProvisioningTypeList[DPP_DATA_TYPE_MAX] = 
{
    {QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE,          QCOM_FW_UPDATE_PARTITION_USER_ESP,  QCOM_FW_UPDATE_PARTITION_GPP_FAT16, QCOM_FW_UPDATE_DPP_QCOM_DIR_FILE},
    {QCOM_FW_UPDATE_DPP_OEM_DIR_FILE,           QCOM_FW_UPDATE_PARTITION_USER_ESP,  QCOM_FW_UPDATE_PARTITION_GPP_FAT16, QCOM_FW_UPDATE_DPP_OEM_DIR_FILE}
};


/**
 * This function hashes a given firmware update file 
 * and compares the result against its hash file. 
 * 
 * 
 * @param FwFile - Enum for the desired file to be hashed.
 * 
 * @return BOOLEAN - True if hash matches, FALSE if not.
 */
BOOLEAN
HashProvisioningFile(
  IN QCOM_FW_UPDATE_FW_FILE FwFile
  )
{
  return FALSE;
}


EFI_STATUS
SSDBlockAlignFileBuffer(
  IN UINTN            FileSize,
  OUT VOID            **FileBuffer,
  IN UINTN            FileSizeAligned
  )
{
  VOID*       FileBufferAligned = NULL;

  if (FileBuffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  FileBufferAligned = UncachedAllocatePool(FileSizeAligned);

  if (FileBufferAligned == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem(FileBufferAligned,*FileBuffer,FileSize);

  *FileBuffer = FileBufferAligned;  

  return EFI_SUCCESS;
}

/**
 * This function orchestrates the device Provisioning process 
 * for Windows-on-ARM firmware. 
 * 
 * @param Active - Indicate if the provisioning process is 
 *               initiated actively by FactoryTool (TRUE), or
 *               passively as part of migration (FALSE)
 *  
 * @param ResetRequests - Increment counter if there is a
 *                  need to reset after provisioning
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchDeviceProvisioning(
  IN BOOLEAN Active,
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS  Status;
  UINTN       ProvisionDataSize;
  VOID        *ProvisionDataBuffer = NULL;
  UINT8       ProvisionedFlagValue;
  BOOLEAN     DeviceProvisioningComplete = FALSE;
  CHAR16      *Type = NULL;
  CHAR16      **ProvisionFilesList = NULL;
  UINTN       ProvisionFilesCount;
  UINT32      TypeIndex;
  UINTN       FileIndex;
  CHAR16      *FilePath = NULL;
  CHAR16      *SubfolderPath = NULL;

  QCOM_FW_UPDATE_FW_PARTITION SrcPart;
  QCOM_FW_UPDATE_FW_FILE      SrcFwFile;
  EFI_DPP_PROTOCOL            *DppProtocol = NULL;

  if(Active)
  {
    DEBUG((EFI_D_WARN,"Performing Device Provisioning...\n"));
  }

  //
  // Get DPP protocol interface
  //
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Failed to locate DPP protocol\n"));
    goto LaunchDeviceProvisioningExit;
  }

  // Go through each provision data types...
  for(TypeIndex = 0; TypeIndex < DPP_DATA_TYPE_MAX; TypeIndex++)
  {
    if(Active)
    {
      SrcPart = DeviceProvisioningTypeList[TypeIndex].SrcPart;
      SrcFwFile = DeviceProvisioningTypeList[TypeIndex].FwFile;
    }
    else
    {
      SrcPart = DeviceProvisioningTypeList[TypeIndex].DestPart;
      SrcFwFile = DeviceProvisioningTypeList[TypeIndex].FwFileDest;
    }

    // Assemble the provision file list from the staging location
    Status = ReadFatFileList(SrcPart,
                             SrcFwFile,
                             &SubfolderPath,
                             &Type,
                             &ProvisionFilesList,
                             &ProvisionFilesCount);

    if(EFI_ERROR(Status))
    {
      if(Status == EFI_INVALID_PARAMETER)
      {
        // Path doesn't exist. 
        //AsciiPrint(__FUNCTION__": Skip device provisioning for type %d\n", TypeIndex);
        Status = EFI_SUCCESS;
        continue;
      }
      else
      {
        // Other errors, abort.
        HANDLE_ERROR_LABEL(Status, ReadFatFileList, LaunchDeviceProvisioningExit);
      }
    }

    if(ProvisionFilesList == NULL)
    {
      Status = EFI_INVALID_PARAMETER;
      goto LaunchDeviceProvisioningExit;
    }

    for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
    {
     
      // Assemble the full file path
      FilePath = AllocateZeroPool(StrSize(ProvisionFilesList[FileIndex]) + StrSize(SubfolderPath));
      if(FilePath == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto LaunchDeviceProvisioningExit;
      }
            
      CopyMem(FilePath, SubfolderPath, StrSize(SubfolderPath));
      CopyMem(&FilePath[StrLen(SubfolderPath)], ProvisionFilesList[FileIndex], StrSize(ProvisionFilesList[FileIndex]));

      // Read provision data from file
      Status = ReadFwFile2(FilePath,
                           SrcPart,
                           &ProvisionDataBuffer,
                           &ProvisionDataSize);    
    
      HANDLE_ERROR_LABEL(Status, ReadFwFile2, LaunchDeviceProvisioningExit);
    
      //Store provisioning data into DPP.
      Status = DppProtocol->SetDPP(DppProtocol,
                                   Type,
                                   ProvisionFilesList[FileIndex],
                                   ProvisionDataBuffer,
                                   ProvisionDataSize);
    
      HANDLE_ERROR_LABEL(Status, SetDPP, LaunchDeviceProvisioningExit);
    
#if TEST_DPP
      // Start TEST
      // Check DPP data integrity
      {
        UINTN      TestDataSize ;
        VOID        *TestDataBuffer = NULL;
    
        Status = DppProtocol->GetDPP(DppProtocol,
                                     Type,
                                     ProvisionFilesList[FileIndex],
                                     NULL,
                                     &TestDataSize);
                    
        if(EFI_ERROR(Status))
        {
          DEBUG((EFI_D_ERROR,"LaunchDeviceProvisioning: Failed to get DPP data size, %r on file #%d\n",Status, FileIndex));
        }
        else
        {
          DEBUG((EFI_D_ERROR,"LaunchDeviceProvisioning: DPP data size = %d, on file #%d\n",TestDataSize, FileIndex));
        }

        TestDataBuffer = AllocateZeroPool(TestDataSize);

        if(TestDataBuffer == NULL)
        {
          DEBUG((EFI_D_ERROR,"LaunchDeviceProvisioning: Failed to allocate memory for TestDataBuffer on file #%d\n", FileIndex));
        }
        else
        {
          Status = DppProtocol->GetDPP(DppProtocol,
                                       Type,
                                       ProvisionFilesList[FileIndex],
                                       TestDataBuffer,
                                       &TestDataSize);
        
          if(EFI_ERROR(Status))
          {
            DEBUG((EFI_D_ERROR,"LaunchDeviceProvisioning: GetDPP returned: %r on file #%d\n",Status, FileIndex));
          }
          else
          {
            if(ProvisionDataSize != TestDataSize)
            {
              DEBUG((EFI_D_WARN,"LaunchDeviceProvisioning: GetDPP returned a different data size :(\n"));
            }
            else
            {
              if(CompareMem(ProvisionDataBuffer, TestDataBuffer, ProvisionDataSize) != 0)
              {
                DEBUG((EFI_D_WARN,"LaunchDeviceProvisioning: GetDPP returned different data :(\n"));
              }
              else
              {
                DEBUG((EFI_D_WARN,"Verified GetDPP returned correct data :)\n"));
              }
            }
          }
        
          if(TestDataBuffer != NULL)
          {
            gBS->FreePool(TestDataBuffer);
            TestDataBuffer = NULL;
          }   
        }
      }
      // End TEST
#endif // TEST_DPP
 
      if(Active)
      {
#if DPP_ON_GPP_FAT16
        // Delete provision file from the GPP
        Status = DeleteFwFile2(FilePath,
                               DeviceProvisioningTypeList[TypeIndex].DestPart);

        //Copy provision data file to GPP
        Status = WriteFwFile2(FilePath,
                              DeviceProvisioningTypeList[TypeIndex].DestPart,
                              &ProvisionDataBuffer,
                              &ProvisionDataSize);
            
        HANDLE_ERROR_LABEL(Status, WriteFwFile2, LaunchDeviceProvisioningExit);
#endif //DPP_ON_GPP_FAT16

        // Delete provision file from the staging location
        Status = DeleteFwFile2(FilePath,
                               DeviceProvisioningTypeList[TypeIndex].SrcPart);
    
        //HANDLE_ERROR_LABEL(Status, DeleteFwFile2, LaunchDeviceProvisioningExit);
      }

      // Cleanup per file 
      if(ProvisionDataBuffer != NULL)
      {
        gBS->FreePool(ProvisionDataBuffer);
        ProvisionDataBuffer = NULL;
      }

      if(FilePath != NULL)
      {
        gBS->FreePool(FilePath);
        FilePath = NULL;
      }
    }

    // Cleanup per type
    if(ProvisionFilesList != NULL)
    {
      for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
      {
        gBS->FreePool(ProvisionFilesList[FileIndex]);
      }

      gBS->FreePool(ProvisionFilesList);
      ProvisionFilesList = NULL;
    }

    if (Type != NULL) 
    {
      gBS->FreePool(Type);
      Type = NULL;
    }
  }

  // Everything executed well, set completion flag
  DeviceProvisioningComplete = TRUE;

LaunchDeviceProvisioningExit:

  if(Active)
  {
    // Update status
    if(DeviceProvisioningComplete)
    {
      // Set device provisioning result to success
      FactoryResult.Provision = PROVISIONING_RESULT_SUCCESS;
    
      // Clear device provisioning flag
      FactoryFlag.Provision = PROVISIONING_OPERATION_NO_COMMIT;
    
      // Set Factory Provisioned Flag.
      ProvisionedFlagValue = QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_SET_AND_MIGRATED;
      Status = SetFlag(QCOM_FW_UPDATE_FLAG_FACTORY_PROVISIONED_FLAG,
                       (VOID*)&ProvisionedFlagValue);

      // Queue up reset request
      (*ResetRequests)++;

      DEBUG((EFI_D_WARN,"Device Provisioning completed.\n"));
    }
    else
    {
      // Set device provisioning result to failure
      FactoryResult.Provision = PROVISIONING_RESULT_FAIL_GENERAL;
      DEBUG((EFI_D_ERROR,"Device Provisioning failed.\n"));
    }
  }

  // Cleanup
  if(ProvisionDataBuffer != NULL)
  {
    gBS->FreePool(ProvisionDataBuffer);
  }

  if(FilePath != NULL)
  {
    gBS->FreePool(FilePath);
  }

  if (Type != NULL) 
  {
    gBS->FreePool(Type);
  }

  if(ProvisionFilesList != NULL)
  {
    for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
    {
      gBS->FreePool(ProvisionFilesList[FileIndex]);
    }
    gBS->FreePool(ProvisionFilesList);
  }

  return Status;
}


/**
 *  This function copies
 *  GoldenEFS, DPP, and SSD
 *  to the EFI System Partition (ESP)
 *  during device bootup.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
CopyFactoryProvisioningData()
{
  EFI_STATUS  Status;
  CHAR16      *Type = NULL;
  CHAR16      **ProvisionFilesList = NULL;
  UINTN       ProvisionFilesCount;
  UINT32      TypeIndex;
  UINTN       FileIndex;
  CHAR16      *FilePath = NULL;
  CHAR16      *SubfolderPath = NULL;

  DEBUG((EFI_D_WARN,"Copying Factory Provisioning Data....\n"));

  // Go through each provision data types...
  for(TypeIndex = 0; TypeIndex < DPP_DATA_TYPE_MAX; TypeIndex++)
  {
    // Assemble the provision file list from the GPP FAT16
    Status = ReadFatFileList(DeviceProvisioningTypeList[TypeIndex].DestPart,
                             DeviceProvisioningTypeList[TypeIndex].FwFileDest,
                             &SubfolderPath,
                             &Type,
                             &ProvisionFilesList,
                             &ProvisionFilesCount);

    if(EFI_ERROR(Status))
    {
      if(Status == EFI_INVALID_PARAMETER)
      {
        // Path doesn't exist. 
        //AsciiPrint(__FUNCTION__": Skip copying provision files for type %d\n", TypeIndex);
        Status = EFI_SUCCESS;
        continue;
      }
      else
      {
        // Other errors, abort.
        HANDLE_ERROR_LABEL(Status, ReadFatFileList, CopyFactoryProvisioningDataExit);
      }
    }

    if(ProvisionFilesList == NULL)
    {
      Status = EFI_INVALID_PARAMETER;
      goto CopyFactoryProvisioningDataExit;
    }

    for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
    {
      // Assemble the full file path
      FilePath = AllocateZeroPool(StrSize(ProvisionFilesList[FileIndex]) + StrSize(SubfolderPath));
      if(FilePath == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto CopyFactoryProvisioningDataExit;
      }

      CopyMem(FilePath, SubfolderPath, StrSize(SubfolderPath));
      CopyMem(&FilePath[StrLen(SubfolderPath)], ProvisionFilesList[FileIndex], StrSize(ProvisionFilesList[FileIndex]));

      // Copy provision file
      Status = CopyFwFile2(DeviceProvisioningTypeList[TypeIndex].DestPart,
                           FilePath,
                           DeviceProvisioningTypeList[TypeIndex].SrcPart,
                           FilePath);

      if(FilePath != NULL)
      {
        gBS->FreePool(FilePath);
        FilePath = NULL;
      }
    }

    // Cleanup per type
    if(ProvisionFilesList != NULL)
    {
      for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
      {
        gBS->FreePool(ProvisionFilesList[FileIndex]);
      }
      gBS->FreePool(ProvisionFilesList);
      ProvisionFilesList = NULL;
    }
    if (Type != NULL) 
    {
      gBS->FreePool(Type);
      Type = NULL;
    }
  }

CopyFactoryProvisioningDataExit:

  // Cleanup
  if(FilePath != NULL)
  {
    gBS->FreePool(FilePath);
  }

  if (Type != NULL) 
  {
    gBS->FreePool(Type);
  }

  if(ProvisionFilesList != NULL)
  {
    for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
    {
      gBS->FreePool(ProvisionFilesList[FileIndex]);
    }
    gBS->FreePool(ProvisionFilesList);
  }

  return Status;
}

/**
  WriteEncryptedKeyStore ()

  @brief
  Reads the keystore file into a buffer 
  
  @param[in]  PartHandle          Partition handle to write to
  @param[in]  EncryptedData       Pointer to the encrypted 
                                  keystore
  @param[in]  EncryptedDataSize   Size in bytes of the encrypted
                                  keystore to write to SSD
  @return
  EFI_STATUS
 
 */
EFI_STATUS
WriteEncryptedKeyStore(
  IN EFI_HANDLE   PartHandle,
  IN UINT8       *EncryptedData,
  IN UINT32       EncryptedDataSize
  )
{
  EFI_STATUS                       Status;
  EFI_BLOCK_IO_PROTOCOL           *BlockIo;
  UINTN                            BytesToMove;
  BOOLEAN                          bBlockAligned = FALSE;

  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiBlockIoProtocolGuid,
                               (VOID**)&BlockIo);

  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, WriteEncryptedKeystoreExit);

  //Align Bytes to Move to block size.
  ALIGN_TO_BLOCK(EncryptedDataSize,BlockIo->Media->BlockSize,BytesToMove);

  if(!(EncryptedDataSize==BytesToMove))
  {
    /*Need to allocate a new buffer only if it's not block aligned*/
    bBlockAligned = TRUE;
    Status = SSDBlockAlignFileBuffer(EncryptedDataSize,(VOID **)&EncryptedData,BytesToMove);
    HANDLE_ERROR_LABEL(Status, SSDBlockAlignFileBuffer, WriteEncryptedKeystoreExit);
  }

  Status = BlockIo->WriteBlocks(BlockIo,
                                BlockIo->Media->MediaId,
                                (UINT64) FIRST_BLOCK,
                                BytesToMove,
                                EncryptedData);

  HANDLE_ERROR_LABEL(Status, BlockIo->WriteBlocks, WriteEncryptedKeystoreExit);

  /*Flush the blocks to memory*/
  Status = BlockIo->FlushBlocks(BlockIo);

WriteEncryptedKeystoreExit: 

  if ((bBlockAligned == TRUE) && (EncryptedData != NULL))
  {
     UncachedFreePool(EncryptedData);
     EncryptedData = NULL;
  }

  return Status;
}

/**
  ReadDataFile ()

  @brief
  Reads the contents of a data file into a buffer 
  
  @param[in]  PartHandle          Partition handle to read from
  @param[in]  FileName            Path to data file to read
  @param[out] Buffer              Returns a pointer to the
                                  buffer
  @param[out] FileSize            Returns Size in bytes of the
                                  data
                                  
  @param[in] bCachedBuff       the returned buffer cache attribute
  @return
  EFI_STATUS
 
 */

EFI_STATUS
ReadDataFile(
  IN  EFI_HANDLE    PartHandle,
  IN  CHAR16       *FileName,
  OUT VOID        **Buffer,
  OUT UINTN        *FileSize,
  IN  BOOLEAN      bCachedBuff
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Fs;
  EFI_FILE_PROTOCOL               *FsRoot;
  EFI_FILE_PROTOCOL               *File;
  EFI_FILE_INFO                   *FileInfo = NULL;
  UINTN                            FileInfoSize;

  /* Initialize output buffer to NULL initially. */
  *Buffer = NULL;

  Status = gBS->HandleProtocol(PartHandle,
                               &gEfiSimpleFileSystemProtocolGuid,
                               (VOID **)&Fs);

  HANDLE_ERROR_LABEL(Status, gBS->HandleProtocol, ReadDataFileExit);

  Status = Fs->OpenVolume(Fs,&FsRoot);

  HANDLE_ERROR_LABEL(Status, Fs->OpenVolume, ReadDataFileExit);

  Status = FsRoot->Open(FsRoot,
              &File,
              FileName,                          
              EFI_FILE_MODE_READ,
              0);

  HANDLE_ERROR_LABEL(Status, FsRoot->Open, ReadDataFileExit);

  /*Get File Info*/
  Status = File->GetInfo(File,
               &gEfiFileInfoGuid,
               &FileInfoSize,
               (VOID*)FileInfo);
  
  if(Status == EFI_BUFFER_TOO_SMALL)
  {
      
    Status = gBS->AllocatePool(EfiBootServicesData,
                               FileInfoSize,
                               (VOID**)&FileInfo);

    HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, ReadDataFileExit);
 
    Status = File->GetInfo(File,
                           &gEfiFileInfoGuid,
                           &FileInfoSize,
                           (VOID*)FileInfo);

    HANDLE_ERROR_LABEL(Status, File->GetInfo, ReadDataFileExit);
  }
  else
  {
    HANDLE_ERROR_LABEL(Status, File->GetInfo, ReadDataFileExit);
  }

  if (FileInfo != NULL)
  {
    *FileSize = FileInfo->FileSize;
  
    if(bCachedBuff)
    {
      Status = gBS->AllocatePool(EfiBootServicesData,
                                 FileInfo->FileSize,
                                 (VOID **)Buffer);
    
      HANDLE_ERROR_LABEL(Status, File->Read, ReadDataFileExit);

    }
    else
    {
      *Buffer = UncachedAllocatePool(*FileSize);
      if(*Buffer == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto ReadDataFileExit;
      }
    }
  

    Status = File->Read(File,
                        FileSize,
                        *Buffer);

    HANDLE_ERROR_LABEL(Status, File->Read, ReadDataFileExit);
  }
  else
  {
    Status = EFI_NOT_FOUND;
    HANDLE_ERROR_LABEL(Status, File->GetInfo, ReadDataFileExit);
  }

ReadDataFileExit: 

  /*Cleanup*/
  if(FileInfo)
  {
    gBS->FreePool(FileInfo);
    FileInfo = NULL;
  }

  /* Free up any memory if there was a failure reading the file. */
  if (EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      if(bCachedBuff)
      {
        gBS->FreePool(*Buffer);
      }
      else
      {
        UncachedFreePool(*Buffer);  
      }
      *Buffer = NULL;
    }
  }

  return Status;
}

/**
 * This function performs following tasks for SSD Provisioning: 
 *  
 * 1. Scan the presence of an SSD Key Store in ESP 
 * 2. Copies it into a buffer and calls into TZ to encrypt it
 * 3. Writes the encrypted keystore to eth SSD Partition 
 * 4. Delete the SSD Key file from ESP
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchSSDProvisioning(
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       PartHandle;
  BOOLEAN                          SSDProvisioningComplete = FALSE;
  VOID                            *KeyStoreBuffer = NULL;
  UINTN                            KeyStoreFileSize;
  UINT8                           *EncryptedData = NULL;
  UINT32                           EncryptedDataSize;
  EFI_ENCRYPTION_PROTOCOL         *Encryption= NULL;

  DEBUG((EFI_D_WARN,"Performing SSD Provisioning...\n"));
  
  Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_USER_ESP, &PartHandle);

  HANDLE_ERROR_LABEL(Status, FwUpdateGetHandleForPartition, SSDExit);

  /*Read the Keystore into a buffer*/
  Status = ReadDataFile(PartHandle,(CHAR16*) KeyStoreFileName, &KeyStoreBuffer, &KeyStoreFileSize, FALSE);

  HANDLE_ERROR_LABEL(Status, ReadDataFile, SSDExit);

  Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                               NULL,
                               (VOID**)&Encryption);

  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, SSDExit);

  /*Send buffer to Encrypt*/
  Status = Encryption->Encrypt(TZ_SSD,
                               (UINT8*)KeyStoreBuffer,
                               (UINT32)KeyStoreFileSize,
                               &EncryptedData,                
                               &EncryptedDataSize);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Error: Encrypt call to TZ returned: %r \n",Status));
    goto SSDExit;
  }

  /*Write EncryptedData to SSD*/
  Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_GPP_SSD, &PartHandle);
  
  HANDLE_ERROR_LABEL(Status, FwUpdateGetHandleForPartition, SSDExit);

  Status = WriteEncryptedKeyStore(PartHandle,EncryptedData,EncryptedDataSize);
  
  HANDLE_ERROR_LABEL(Status, WriteEncryptedKeyStore, SSDExit);

  SSDProvisioningComplete = TRUE;

SSDExit:

  /*Delete File from ESP*/ 
  Status = DeleteFwFile(QCOM_FW_UPDATE_SSD_KEYSTORE_FILE,QCOM_FW_UPDATE_PARTITION_USER_ESP);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Error: DeleteFwFile returned: %r on SSD Keystore file\n",Status));
  }

  // Update result flag accordingly
  if(SSDProvisioningComplete)
  {
    // Set SSD provisioning result
    FactoryResult.SSD = PROVISIONING_RESULT_SUCCESS;
    // Queue up reset request
    (*ResetRequests)++;

    DEBUG((EFI_D_WARN,"SSD Provisioning completed.\n"));
  }
  else
  {
    // Set SSD provisioning result
    FactoryResult.SSD = PROVISIONING_RESULT_FAIL_GENERAL;
    DEBUG((EFI_D_ERROR,"SSD Provisioning failed.\n"));
  }
  
  /* Free up the ssd key buffer */
  if (KeyStoreBuffer!= NULL)
  {  
    UncachedFreePool(KeyStoreBuffer);
    KeyStoreBuffer = NULL;
  }
 
  FactoryFlag.SSD = PROVISIONING_OPERATION_NO_COMMIT; 

  return Status;
}

/**
 * This function performs following tasks for OA3 Provisioning: 
 *  
 * 1. Scan the presence of OA3.0 ACPI patch file in ESP 
 * 2. If the patch file exist, copy it to FAT16 in GPP 
 * 3. Delete ACPI patch file from ESP
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchOA3Provisioning(
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS  Status;
  BOOLEAN     FoundFile = FALSE;
  BOOLEAN     OA3ProvisioningComplete = TRUE;
  UINTN       CopyCounter;

  DEBUG((EFI_D_WARN,"Performing OA 3.0 Provisioning...\n"));

  // Step 1:
  // Search the presence of prior instance of ACPI MSDM table
  // If prior instance of the table is detected, stop the process and
  // return failure with reason.
  for(CopyCounter = 0; CopyCounter < OA3_FILE_COPIES; CopyCounter++)
  {
    Status = FindFwFile(OA3ProvisioningCopyList[CopyCounter].FwFileDest,
                        OA3ProvisioningCopyList[CopyCounter].DestPart,
                        &FoundFile);

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: FindFwFile returned: %r on file #%d.\n", Status, CopyCounter));
    }
    else
    {
      if(FoundFile)
      {
        break;
      }
    }
  }

  if(FoundFile)
  {
    // Stop the process since prior instance of ACPI MSDM table exists
    // Set OA3 result to failure with reason
    FactoryResult.OA3 = PROVISIONING_RESULT_FAIL_PRIOR_INSTANCE_EXIST;
    DEBUG((EFI_D_WARN,"Prior instance of OA3 provisioning data exists. Operation aborted.\n"));
    goto oa3exit;
  }

  // Step 2:
  // Copy over OA3 provisioning data to GPP
  for(CopyCounter = 0; CopyCounter < OA3_FILE_COPIES; CopyCounter++)
  {
    Status = CopyFwFile(OA3ProvisioningCopyList[CopyCounter].FwFile,
                        OA3ProvisioningCopyList[CopyCounter].SrcPart,
                        OA3ProvisioningCopyList[CopyCounter].DestPart,
                        OA3ProvisioningCopyList[CopyCounter].FwFileDest);    

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: CopyFwFile returned: %r on file #%d.\n",Status, CopyCounter));
      OA3ProvisioningComplete = FALSE;
    }    
  }

  // Step 3:
  // Update result flag accordingly
  if(OA3ProvisioningComplete)
  {
    // Set OA3 provisioning result
    FactoryResult.OA3 = PROVISIONING_RESULT_SUCCESS;
    // Queue up reset request
    (*ResetRequests)++;
    DEBUG((EFI_D_WARN,"OA 3.0 Provisioning completed.\n"));
  }
  else
  {
    // Set OA3 provisioning result
    FactoryResult.OA3 = PROVISIONING_RESULT_FAIL_GENERAL;
    DEBUG((EFI_D_ERROR,"OA 3.0 Provisioning failed.\n"));
  }

oa3exit:

  // Step 4:
  // Clear OA3 commit flag
  // Delete OA3 provisioning data from EFI system partition

  FactoryFlag.OA3 = PROVISIONING_OPERATION_NO_COMMIT;

  for(CopyCounter = 0; CopyCounter < OA3_FILE_COPIES; CopyCounter++)
  {
    Status = DeleteFwFile(OA3ProvisioningCopyList[CopyCounter].FwFile,
                          OA3ProvisioningCopyList[CopyCounter].SrcPart);
    
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: DeleteFwFile returned: %r on file #%d.\n",Status, CopyCounter));
    }
  }

  return Status;
}


/**
 * This function erases OA3 provisioning: 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchEraseOA3Provisioning(
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS  Status;
  BOOLEAN     FoundFile = FALSE;
  BOOLEAN     OA3ErasingComplete = TRUE;
  UINTN       CopyCounter;

  DEBUG((EFI_D_WARN,"Performing Erasing OA 3.0...\n"));

  // Step 1:
  // Search the presence of prior instance of ACPI MSDM table
  // If prior instance of the table is detected, delete it.
  for(CopyCounter = 0; CopyCounter < OA3_FILE_COPIES; CopyCounter++)
  {
    Status = FindFwFile(OA3ProvisioningCopyList[CopyCounter].FwFileDest,
                        OA3ProvisioningCopyList[CopyCounter].DestPart,
                        &FoundFile);

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: FindFwFile returned: %r on file #%d.\n", Status, CopyCounter));
      OA3ErasingComplete = FALSE;
      continue;
    }

    if(FoundFile)
    {
      Status = DeleteFwFile(OA3ProvisioningCopyList[CopyCounter].FwFileDest,
                            OA3ProvisioningCopyList[CopyCounter].DestPart);
      
      if(EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN,"Warning: DeleteFwFile returned: %r on file #%d.\n",Status, CopyCounter));
        OA3ErasingComplete = FALSE;
      }
      else
      {
        (*ResetRequests)++;
      }
    }
  }

  // Step 2:
  // Update commit flag and result flag accordingly
  if(OA3ErasingComplete)
  {
    // Set EraseOA3 provisioning result
    FactoryResult.EraseOA3 = PROVISIONING_RESULT_SUCCESS;
    DEBUG((EFI_D_WARN,"Erasing OA 3.0 completed.\n"));
  }
  else
  {
    // Set EraseOA3 provisioning result
    FactoryResult.EraseOA3 = PROVISIONING_RESULT_FAIL_GENERAL;
    DEBUG((EFI_D_ERROR,"Erasing OA 3.0 failed.\n"));
  }

  // Step 3:
  // Clear EraseOA3 commit flag
  FactoryFlag.EraseOA3 = PROVISIONING_OPERATION_NO_COMMIT;

  return Status;
}


/**
 * This function erases all provision data including OA3 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchEraseAllProvisioning(
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS  Status;
  BOOLEAN     FoundFile = FALSE;
  BOOLEAN     EraseAllComplete = TRUE;
  CHAR16      *Type = NULL;
  CHAR16      **ProvisionFilesList = NULL;
  UINTN       ProvisionFilesCount;
  UINT32      TypeIndex;
  UINTN       FileIndex;
  CHAR16      *FilePath = NULL;
  CHAR16      *SubfolderPath = NULL;
  UINT32      LocationIndex ;
  QCOM_FW_UPDATE_FW_PARTITION SrcPart;
  QCOM_FW_UPDATE_FW_FILE      SrcFwFile;
  EFI_DPP_PROTOCOL            *DppProtocol = NULL;

  DEBUG((EFI_D_WARN,"Performing Erasing ALL...\n"));

  // Erase DPP
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  if(EFI_ERROR(Status))
  {
      DEBUG((EFI_D_ERROR,"Failed to locate DPP protocol\n"));
      goto LaunchEraseAllProvisioningExit;
  }

  Status = DppProtocol->EraseDPP(DppProtocol);

  if(EFI_ERROR(Status))
  {
    EraseAllComplete = FALSE;
    //AsciiPrint(__FUNCTION__":  Failed to erase DPP\n");
  }

  // Erase OA3.0
  // Search the presence of prior instance of ACPI MSDM table
  // If prior instance of the table is detected, delete it.
  for(FileIndex = 0; FileIndex < OA3_FILE_COPIES; FileIndex++)
  {
    Status = FindFwFile(OA3ProvisioningCopyList[FileIndex].FwFileDest,
                        OA3ProvisioningCopyList[FileIndex].DestPart,
                        &FoundFile);

    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: FindFwFile returned: %r on file #%d.\n", Status, FileIndex));
      EraseAllComplete = FALSE;
      continue;
    }

    if(FoundFile)
    {
      Status = DeleteFwFile(OA3ProvisioningCopyList[FileIndex].FwFileDest,
                            OA3ProvisioningCopyList[FileIndex].DestPart);
      
      if(EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN,"Warning: DeleteFwFile returned: %r on file #%d.\n",Status, FileIndex));
        EraseAllComplete = FALSE;
      }
      else
      {
        (*ResetRequests)++;
      }
    }
  }

  // Erase GPP FAT16 provision files
  // Go through each provision data types...
  for(LocationIndex = 0; LocationIndex < 2; LocationIndex++)
  {
    for(TypeIndex = 0; TypeIndex < DPP_DATA_TYPE_MAX; TypeIndex++)
    {
      if(LocationIndex == 0)
      {
        SrcPart = DeviceProvisioningTypeList[TypeIndex].SrcPart;
        SrcFwFile = DeviceProvisioningTypeList[TypeIndex].FwFile;
      }
      else
      {
        SrcPart = DeviceProvisioningTypeList[TypeIndex].DestPart;
        SrcFwFile = DeviceProvisioningTypeList[TypeIndex].FwFileDest;
      }

      // Assemble the provision file list from the GPP FAT16
      Status = ReadFatFileList(SrcPart,
                               SrcFwFile,
                               &SubfolderPath,
                               &Type,
                               &ProvisionFilesList,
                               &ProvisionFilesCount);

      if(ProvisionFilesList == NULL)
      {
        Status = EFI_INVALID_PARAMETER;
        goto LaunchEraseAllProvisioningExit;
      }

      if(EFI_ERROR(Status))
      {
        if((Status == EFI_INVALID_PARAMETER) || (Status == EFI_NOT_FOUND))
        {
          // Path doesn't exist. 
          //AsciiPrint(__FUNCTION__": Skip type %d\n", TypeIndex);
          Status = EFI_SUCCESS;
          continue;
        }
        else
        {
          // Other errors, abort.
          //HANDLE_ERROR_LABEL(Status, ReadFatFileList, LaunchEraseAllProvisioningExit);
          //AsciiPrint(__FUNCTION__": Error reading provision files for type %d\n", TypeIndex);
          EraseAllComplete = FALSE;
        }
      }
  
      for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
      {
        // Assemble the full file path
        FilePath = AllocateZeroPool(StrSize(ProvisionFilesList[FileIndex]) + StrSize(SubfolderPath));
        if(FilePath == NULL)
        {
          Status = EFI_OUT_OF_RESOURCES;
          EraseAllComplete = FALSE;
          goto LaunchEraseAllProvisioningExit;
        }
              
        CopyMem(FilePath, SubfolderPath, StrSize(SubfolderPath));
        CopyMem(&FilePath[StrLen(SubfolderPath)], ProvisionFilesList[FileIndex], StrSize(ProvisionFilesList[FileIndex]));
  
        // Delete provision file from the staging location
        Status = DeleteFwFile2(FilePath,
                               SrcPart);
  
        if(FilePath != NULL)
        {
          gBS->FreePool(FilePath);
          FilePath = NULL;
        }
      }
  
      // Cleanup per type
      if(ProvisionFilesList != NULL)
      {
        for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
        {
          gBS->FreePool(ProvisionFilesList[FileIndex]);
        }
        gBS->FreePool(ProvisionFilesList);
        ProvisionFilesList = NULL;
      }
      if (Type != NULL) 
      {
        gBS->FreePool(Type);
        Type = NULL;
      }
    }
  }

  // Erase SMBIOS configuration file
  FoundFile = FALSE;
  Status = FindFwFile2(SMBIOSFileNameDest,
                       QCOM_FW_UPDATE_PARTITION_GPP_FAT16,
                       &FoundFile);
  
  if(EFI_ERROR(Status))
  {
    EraseAllComplete = FALSE;
    goto LaunchEraseAllProvisioningExit;
  }

  if (FoundFile)
  {
    Status = DeleteFwFile2(SMBIOSFileNameDest, QCOM_FW_UPDATE_PARTITION_GPP_FAT16);
    if(EFI_ERROR(Status))
    {
      EraseAllComplete = FALSE;
    }
    else
    {
      (*ResetRequests)++;
    }
  }

LaunchEraseAllProvisioningExit:

  // Cleanup
  if(FilePath != NULL)
  {
    gBS->FreePool(FilePath);
  }

  if (Type != NULL) 
  {
    gBS->FreePool(Type);
  }

  if(ProvisionFilesList != NULL)
  {
    for(FileIndex = 0; FileIndex < ProvisionFilesCount; FileIndex++)
    {
      gBS->FreePool(ProvisionFilesList[FileIndex]);
    }
    gBS->FreePool(ProvisionFilesList);
  }

  if(EraseAllComplete)
  {
    // Set EraseAll provisioning result
    FactoryResult.EraseAll = PROVISIONING_RESULT_SUCCESS;
    DEBUG((EFI_D_WARN,"Erasing ALL completed.\n"));
  }
  else
  {
    // Set EraseAll provisioning result
    FactoryResult.EraseAll = PROVISIONING_RESULT_FAIL_GENERAL;
    DEBUG((EFI_D_ERROR,"Erasing ALL failed.\n"));
  }

  // Clear EraseAll commit flag
  FactoryFlag.EraseAll = PROVISIONING_OPERATION_NO_COMMIT;

  return Status;
}


/**
 * This function locates the firmware volume handle and firmware volume file name GUID for a given application name UI String.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS FindAppInFV (
  IN  CHAR16      *UiString,
  OUT EFI_HANDLE  *FvHandle,
  OUT EFI_GUID    *NameGuid
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    NextStatus;
  UINTN                         NoHandles;
  EFI_HANDLE                    *Buffer;
  UINTN                         Index;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  VOID                          *Key;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         UiStringLen ;
  CHAR16                        *UiSection;
  UINT32                        Authentication;

  if (UiString == NULL)
  {
    /* The argument requires a UI String */
    return EFI_NOT_FOUND;  
  }
  
  UiStringLen = StrLen (UiString);
  
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &NoHandles, &Buffer);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->HandleProtocol (Buffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **)&Fv);
      if (!EFI_ERROR (Status)) {
        Status = gBS->AllocatePool(EfiBootServicesData, Fv->KeySize, (VOID**)&Key);

        if (EFI_ERROR (Status))
        {
           gBS->FreePool (Buffer);
           return Status;
        }
        ZeroMem(Key, Fv->KeySize);
        
        FileType = EFI_FV_FILETYPE_APPLICATION;
        
        do {
          NextStatus = Fv->GetNextFile (Fv, Key, &FileType, NameGuid, &Attributes, &Size);
          if (!EFI_ERROR (NextStatus)) {

            UiSection = NULL;
            Status = Fv->ReadSection (
                          Fv, 
                          NameGuid, 
                          EFI_SECTION_USER_INTERFACE, 
                          0,
                          (VOID **)&UiSection,
                          &Size,
                          &Authentication
                          );
            if (!EFI_ERROR (Status)) {
              if (StrnCmp (UiString, UiSection, UiStringLen) == 0) {
                //
                // We found a UiString match. 
                //
                *FvHandle = Buffer[Index];
                gBS->FreePool (Key);
                gBS->FreePool (UiSection);
                gBS->FreePool (Buffer);
                
                return Status;
              }
              gBS->FreePool (UiSection);
            }
          }
        } while (!EFI_ERROR (NextStatus));
        
        gBS->FreePool (Key);
      }
    }
    
    gBS->FreePool (Buffer);
   }

  return EFI_NOT_FOUND;
}


/**
 * This function launches RPMB provisioning
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchRPMBProvisioning(
  IN EFI_HANDLE        ImageHandle,
  IN OUT UINT32       *ResetRequests
   )
{
  EFI_STATUS  Status;
  EFI_HANDLE  FvHandle;
  EFI_GUID    NameGuid;
  EFI_DEVICE_PATH_PROTOCOL *FvDevicePath = NULL;
  EFI_DEVICE_PATH_PROTOCOL *AppDevicePath = NULL;
  EFI_HANDLE LoadedImageHandle = NULL;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo = NULL;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *FvDevicePathNode = NULL;
  EFI_DEVICE_PATH_UTILITIES_PROTOCOL* DevPathUtilities;
  CHAR8 AppArgs[] = "-NoPrompt";

  DEBUG((EFI_D_WARN,"Performing RPMB Provisioning...\n"));

  // Step 1:
  // Locate the firmware volume handle and firmware volume file name GUID for the given application name across all firmware volumes
  // mounted.
  Status = FindAppInFV(L"RPMBProvision", &FvHandle, &NameGuid);
  HANDLE_ERROR_LABEL(Status, FindAppInFV, LaunchRPMBProvisioningExit);

  Status = gBS->HandleProtocol (
                   FvHandle,
                   &gEfiDevicePathProtocolGuid,
                   (VOID *) &FvDevicePath
                   );
  HANDLE_ERROR_LABEL(Status, HandleProtocol, LaunchRPMBProvisioningExit);

  Status = gBS->LocateProtocol(&gEfiDevicePathUtilitiesProtocolGuid,
                                 NULL,
                                 (VOID**)&DevPathUtilities);
  HANDLE_ERROR_LABEL(Status, LocateProtocol, LaunchRPMBProvisioningExit);


  Status = gBS->AllocatePool(EfiBootServicesData, sizeof(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH), (VOID**)&FvDevicePathNode);
  HANDLE_ERROR_LABEL(Status, AllocatePool, LaunchRPMBProvisioningExit);

  if (FvDevicePathNode == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG_ERR_LABEL("\nLaunchRPMBProvisioning: FvDevicePathNode is NULL\n",LaunchRPMBProvisioningExit);
  }

  FvDevicePathNode->Header.Type     = MEDIA_DEVICE_PATH;
  FvDevicePathNode->Header.SubType  = MEDIA_PIWG_FW_FILE_DP;
  FvDevicePathNode->Header.Length[0] = (sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH))&0x00FF;
  FvDevicePathNode->Header.Length[1] = (sizeof (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH))&0xFF00;

  CopyGuid (&FvDevicePathNode->FvFileName, &NameGuid);

  AppDevicePath = DevPathUtilities->AppendDeviceNode(FvDevicePath,
                                                 (EFI_DEVICE_PATH_PROTOCOL*)FvDevicePathNode);

  // Step 3:
  // Load the firmware volume application into memory.
  Status = gBS->LoadImage (TRUE, ImageHandle, AppDevicePath, NULL, 0, &LoadedImageHandle);
  HANDLE_ERROR_LABEL(Status, LoadImage, LaunchRPMBProvisioningExit);

  if (LoadedImageHandle == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG_ERR_LABEL("\nLaunchRPMBProvisioning: LoadedImageHandle is NULL\n",LaunchRPMBProvisioningExit);
  }

  /* Setup the RPMBProvision application arguments to not prompt the user. */
  Status = gBS->HandleProtocol (LoadedImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
  HANDLE_ERROR_LABEL(Status, HandleProtocol, LaunchRPMBProvisioningExit);

  ImageInfo->LoadOptionsSize = (UINT32)AsciiStrSize (AppArgs);

  /* Note: The LoadOptions memory will be freed implicitly after StartImage completes. */
  Status = gBS->AllocatePool(EfiBootServicesData, ImageInfo->LoadOptionsSize, (VOID **)&ImageInfo->LoadOptions);
  HANDLE_ERROR_LABEL(Status, AllocatePool, LaunchRPMBProvisioningExit);

  if (ImageInfo->LoadOptions == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    DEBUG_ERR_LABEL("\nLaunchRPMBProvisioning: ImageInfo->LoadOptions is NULL\n",LaunchRPMBProvisioningExit);
  }

  AsciiStrCpy (ImageInfo->LoadOptions, AppArgs);

  Status = gBS->StartImage (LoadedImageHandle, NULL, NULL);

LaunchRPMBProvisioningExit:

  // Step 2:
  // Update commit flag and result flag accordingly
  if(!EFI_ERROR (Status))
  {
    // Set RPMB provisioning result
    FactoryResult.RPMB = PROVISIONING_RESULT_SUCCESS;

    // Queue up reset request
    (*ResetRequests)++;

    DEBUG((EFI_D_WARN,"RPMB Provisioning completed.\n"));
  }
  else
  {
    if (Status == EFI_UNSUPPORTED)
    {
      FactoryResult.RPMB = PROVISIONING_RESULT_FAIL_PRIOR_INSTANCE_EXIST;
    }
    else
    {
      FactoryResult.RPMB = PROVISIONING_RESULT_FAIL_GENERAL;
    }

    DEBUG((EFI_D_ERROR,"RPMB Provisioning failed.\n"));
  }

  // Step 3:
  // Clear RPMB commit flag
  FactoryFlag.RPMB = PROVISIONING_OPERATION_NO_COMMIT;

  if (FvDevicePathNode != NULL)
  {
    gBS->FreePool(FvDevicePathNode);
    FvDevicePathNode = NULL;
  }

  return Status;
}


/**
 * This function performs UEFI Secure Boot provisioning for a given key data type
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchSecureBootProvisioning(
  IN     PROV_KEY_TYPE  KeyType,
  IN OUT UINT32        *ResetRequests)
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;
  EFI_HANDLE                       PartHandle = NULL;
  BOOLEAN                          ProvisioningSuccess = FALSE;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize ;
  UINT16                          *FileName = NULL;
  CHAR16                          *VarName = NULL;
  EFI_GUID                        *VarGuid = NULL;
  STATIC EFI_GUID GlobalVariableGuid = EFI_GLOBAL_VARIABLE;
  STATIC EFI_GUID DBGuid = EFI_IMAGE_SECURITY_DATABASE_GUID;
  UINT32 Attributes = (EFI_VARIABLE_RUNTIME_ACCESS | 
                       EFI_VARIABLE_BOOTSERVICE_ACCESS |
                       EFI_VARIABLE_NON_VOLATILE |
                       EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS);

  DEBUG((EFI_D_WARN,"Performing UEFI Secure Boot Provisioning...\n"));

  Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_USER_ESP, &PartHandle);

  HANDLE_ERROR_LABEL(Status, LaunchSecureBootProvisioning, SecureBootProvExit);

  /*Read the key data into a buffer and set the variable. */

  switch (KeyType)
  {
    case TYPE_KEKPUB:
      FileName = KEKFileName;
      VarName = EFI_KEY_EXCHANGE_KEY_NAME;
      VarGuid = &GlobalVariableGuid;
        
      break;
    case TYPE_SIGNATUREDB:
      FileName = DBFileName;
      VarName = EFI_IMAGE_SECURITY_DATABASE;
      VarGuid = &DBGuid;

      break;
    case TYPE_SIGNATUREDBX:
      FileName = DBXFileName;
      VarName = EFI_IMAGE_SECURITY_DATABASE1;
      VarGuid = &DBGuid;

      break;
    case TYPE_PKPUB:
      FileName = PKFileName;
      VarName = EFI_PLATFORM_KEY_NAME;
      VarGuid = &GlobalVariableGuid;

      break;
    default:
      Status = EFI_UNSUPPORTED;
      break;
  }

  HANDLE_ERROR_LABEL(Status, LaunchSecureBootProvisioning, SecureBootProvExit);

  Status = ReadDataFile(PartHandle, FileName, &DataBuffer, &DataSize, TRUE);

  HANDLE_ERROR_LABEL(Status, LaunchSecureBootProvisioning, SecureBootProvExit);

  Status = gRT->SetVariable(VarName,VarGuid,Attributes,DataSize,DataBuffer);

  HANDLE_ERROR_LABEL(Status, LaunchSecureBootProvisioning, SecureBootProvExit);

  ProvisioningSuccess = TRUE;

SecureBootProvExit:

  /* Free up the key buffer */
  if (DataBuffer != NULL)
  {
      gBS->FreePool(DataBuffer);
      DataBuffer = NULL;
  }

  if (FileName != NULL)
  {
      /*Delete File from ESP*/ 
      Status = DeleteFwFile2(FileName,QCOM_FW_UPDATE_PARTITION_USER_ESP);
  }
  
  if(EFI_ERROR(Status))
  {
      DEBUG((EFI_D_WARN,"Warning: DeleteFwFile returned: %r on UEFI secure boot provisioning file\n",Status));
  }

  // Update result flag accordingly
  if(ProvisioningSuccess)
  {
    switch (KeyType)
    {
      case TYPE_KEKPUB:
        FactoryResult.KEKpub = PROVISIONING_RESULT_SUCCESS;
        FactoryFlag.KEKpub = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_WARN,"KEK Provisioning completed\n"));
        break;
      case TYPE_SIGNATUREDB:
        FactoryResult.SignatureDB = PROVISIONING_RESULT_SUCCESS;
        FactoryFlag.SignatureDB = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_WARN,"SignatureDB Provisioning completed\n"));
        break;
      case TYPE_SIGNATUREDBX:
        FactoryResult.SignatureDBX = PROVISIONING_RESULT_SUCCESS;
        FactoryFlag.SignatureDBX = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_WARN,"SignatureDBX Provisioning completed\n"));
        break;
      case TYPE_PKPUB:
        FactoryResult.PKpub = PROVISIONING_RESULT_SUCCESS;
        FactoryFlag.PKpub = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_WARN,"PK Provisioning completed\n"));
        break;
      default:
        break;
    }

    // Queue up reset request
    (*ResetRequests)++;
  }
  else
  {
    switch (KeyType)
    {
      case TYPE_KEKPUB:
        FactoryResult.KEKpub = PROVISIONING_RESULT_FAIL_GENERAL;
        FactoryFlag.KEKpub = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_ERROR,"KEK Provisioning failed\n"));
        break;
      case TYPE_SIGNATUREDB:
        FactoryResult.SignatureDB = PROVISIONING_RESULT_FAIL_GENERAL;
        FactoryFlag.SignatureDB = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_ERROR,"SignatureDB Provisioning failed\n"));
        break;
      case TYPE_SIGNATUREDBX:
        FactoryResult.SignatureDBX = PROVISIONING_RESULT_FAIL_GENERAL;
        FactoryFlag.SignatureDBX = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_ERROR,"SignatureDBX Provisioning failed\n"));
        break;
      case TYPE_PKPUB:
        FactoryResult.PKpub = PROVISIONING_RESULT_FAIL_GENERAL;
        FactoryFlag.PKpub = PROVISIONING_OPERATION_NO_COMMIT;
        DEBUG((EFI_D_ERROR,"PK Provisioning failed\n"));
        break;
      default:
        break;
    }
  }
  
  return Status;
}


/**
 * This function performs UEFI Secure Boot provisioning for a given key data type
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchSMBIOSProvisioning(
  IN OUT UINT32        *ResetRequests
  )
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;

  DEBUG((EFI_D_WARN,"Performing SMBIOS Provisioning...\n"));

  Status = CopyFwFile2(QCOM_FW_UPDATE_PARTITION_USER_ESP, SMBIOSFileNameSrc, QCOM_FW_UPDATE_PARTITION_GPP_FAT16, SMBIOSFileNameDest);

  HANDLE_ERROR_LABEL(Status, LaunchSMBIOSProvisioning, SMBIOSProvExit);

SMBIOSProvExit:

  /* Delete File from ESP */ 
  DeleteFwFile2(SMBIOSFileNameSrc, QCOM_FW_UPDATE_PARTITION_USER_ESP);

  /* Update result flag accordingly */
  if(!EFI_ERROR (Status))
  {
    /* Set EraseOA3 provisioning result */
    FactoryResult.SMBIOS = PROVISIONING_RESULT_SUCCESS;

    /* Queue up reset request */
    (*ResetRequests)++;

    DEBUG((EFI_D_WARN,"SMBIOS Provisioning completed\n"));
  }
  else
  {
    /* Set provisioning result */
    FactoryResult.SMBIOS = PROVISIONING_RESULT_FAIL_GENERAL;

    DEBUG((EFI_D_ERROR,"SMBIOS Provisioning failed\n"));
  }

  /* Clear EraseOA3 commit flag */
  FactoryFlag.SMBIOS = PROVISIONING_OPERATION_NO_COMMIT;

  return Status;
}

/**
 * This function performs Windows debug policy provisioning
 * or removal of Windows debug policy. 
 * 
 * @param bRemove - Indicate if the operation is to remove
 *                  the current debug policy (TRUE) or not (FALSE)
 *  
 * @param ResetRequests - Increment counter if there is a
 *                  need to reset after provisioning
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchDebugPolicyProvisioning(
  IN     BOOLEAN   bRemove,
  IN OUT UINT32   *ResetRequests)
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;
  EFI_HANDLE                       PartHandle = NULL;
  BOOLEAN                          ProvisioningSuccess = FALSE;
  VOID                            *DataBuffer = NULL;
  UINTN                            DataSize ;
  UINT32 Attributes = (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE);

  if (bRemove == TRUE)
  { 
      DEBUG((EFI_D_WARN,"Removing Windows debug policy...\n"));

      /* Write a value of 0 with a size of 0 to delete the UEFI boot NV variable. */
      Status = gRT->SetVariable(L"CurrentPolicy", &DebugModeVariableGuid, Attributes, 0, &DataSize);
      if (Status == EFI_NOT_FOUND)
      {
        /* Indicate that the provisioning step is successful if the Windows debug policy is already
           removed. */
        Status = EFI_SUCCESS;
      }

      HANDLE_ERROR_LABEL(Status, LaunchDebugPolicyProvisioning, DebugPolicyProvExit);
  }
  else
  {
      DEBUG((EFI_D_WARN,"Performing Windows debug policy provisioning...\n"));

      Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_USER_ESP, &PartHandle);

      HANDLE_ERROR_LABEL(Status, LaunchDebugPolicyProvisioning, DebugPolicyProvExit);

      Status = ReadDataFile(PartHandle, DebugPolicyFileName, &DataBuffer, &DataSize, TRUE);

      HANDLE_ERROR_LABEL(Status, LaunchDebugPolicyProvisioning, DebugPolicyProvExit);

      Status = gRT->SetVariable(L"CurrentPolicy", &DebugModeVariableGuid, Attributes, DataSize, DataBuffer);

      HANDLE_ERROR_LABEL(Status, LaunchDebugPolicyProvisioning, DebugPolicyProvExit);
  }

  ProvisioningSuccess = TRUE;

DebugPolicyProvExit:

  /* Free up the key buffer */
  if (DataBuffer != NULL)
  {
    gBS->FreePool(DataBuffer);
    DataBuffer = NULL;
  }

  if (bRemove == FALSE)
  {
    /*Delete debug policy file from ESP*/ 
    Status = DeleteFwFile2(DebugPolicyFileName, QCOM_FW_UPDATE_PARTITION_USER_ESP);

    if(EFI_ERROR(Status))
    {
       DEBUG((EFI_D_WARN,"Warning: DeleteFwFile returned: %r on debug policy provisioning file\n",Status));
    }
  }
  
  // Update result flag accordingly
  if(ProvisioningSuccess)
  {
    if (bRemove == TRUE)
    {
      FactoryResult.DebugPolicyRemove = PROVISIONING_RESULT_SUCCESS;
      FactoryFlag.DebugPolicyRemove = PROVISIONING_OPERATION_NO_COMMIT;
      DEBUG((EFI_D_WARN,"DebugPolicy is removed \n"));
    }
    else
    {
      FactoryResult.DebugPolicy = PROVISIONING_RESULT_SUCCESS;
      FactoryFlag.DebugPolicy = PROVISIONING_OPERATION_NO_COMMIT;
      DEBUG((EFI_D_WARN,"DebugPolicy Provisioning completed\n"));
    }

    // Queue up reset request
    (*ResetRequests)++;
  }
  else
  {
    if (bRemove == TRUE)
    {
      FactoryResult.DebugPolicyRemove = PROVISIONING_RESULT_FAIL_GENERAL;
      FactoryFlag.DebugPolicyRemove = PROVISIONING_OPERATION_NO_COMMIT;
      DEBUG((EFI_D_ERROR,"DebugPolicy remove failed\n"));
    }
    else
    {
      FactoryResult.DebugPolicy = PROVISIONING_RESULT_FAIL_GENERAL;
      FactoryFlag.DebugPolicy = PROVISIONING_OPERATION_NO_COMMIT;
      DEBUG((EFI_D_ERROR,"DebugPolicy Provisioning failed\n"));
    }
  }
  
  return Status;
}

/**
 * This function performs following tasks for OPM Provisioning: 
 *  
 * 1. Scan the presence of OPM public key and OPM private key Store in ESP 
 * 2. Copies it into a buffer and calls into TZ to decrypt it by fixed key and SSD key, then encrypt it by MSM HW key 
 * 3. Writes the encrypted keystore to eth SSD Partition 
 * 4. Delete the OPM public key and encrypted OPM private key file from ESP
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchOPMProvisioning(VOID)
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;
  EFI_HANDLE                       PartHandle = NULL;
  BOOLEAN                          OPMProvisioningComplete = FALSE;
  VOID                            *PubKeyBuffer = NULL, *PrivKeyBuffer = NULL  ;
  UINTN                            PubKeySize, PrivKeySize;
  UINT8                           *EncryptedData = NULL;
  UINT32                           EncryptedDataSize ; 
  EFI_ENCRYPTION_PROTOCOL         *Encryption = NULL;
  EFI_DPP_PROTOCOL                *DppProtocol = NULL;
  EFI_QCOM_TOOLSUPPORT_PROTOCOL   *ToolSupportProtocol = NULL;
  UINT8                           *key1 = NULL;
  UINT8                            key1_len8 ;
  UINTN                            key1_len32;

  DEBUG((EFI_D_WARN,"Performing OPM Provisioning...\n"));
  
  Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_USER_ESP, &PartHandle);
  HANDLE_ERROR_LABEL(Status, FwUpdateGetHandleForPartition, OPMExit);

  /*Read the OPM public key into a buffer*/
  Status = ReadDataFile(PartHandle, OPMpubFileNamePath, &PubKeyBuffer, &PubKeySize, TRUE);  
  HANDLE_ERROR_LABEL(Status, OPMpubFileNamePath, OPMExit);

  /*Read the encrypted OPM private key into a buffer*/
  Status = ReadDataFile(PartHandle, OPMprivFileNamePath, &PrivKeyBuffer, &PrivKeySize, FALSE);
  HANDLE_ERROR_LABEL(Status, OPMprivFileName, OPMExit);

  /*locate encryption potocal*/
  Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                               NULL,
                               (VOID**)&Encryption);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, OPMExit);

  /*locate tool support potocal*/
   Status = gBS->LocateProtocol(&gQcomToolSupportProtocolGuid,
                                NULL,
                                (VOID**)&ToolSupportProtocol);
  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, OPMExit);

  /*get handshake key length*/
  Status = ToolSupportProtocol->GetDataLenProtocol(ToolSupportProtocol,&key1_len8);
  if (EFI_ERROR (Status)) 
  {
    DEBUG((EFI_D_WARN,"Warning: GetDataLen returned: %r \n",Status));
    goto OPMExit;
  }

  key1_len32 = (UINTN)key1_len8;
  
  /*allocate buffer for handshake key*/  
  Status = gBS->AllocatePool(EfiBootServicesData, key1_len32, (VOID**)&key1);
  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_WARN,"Warning: reserve buffer for key1 returned: %r \n",Status));
    goto OPMExit;
  }
  ZeroMem(key1, key1_len32);

  /*get handshake key*/  
  Status = ToolSupportProtocol->GetDataProtocol(ToolSupportProtocol,key1);
  if (EFI_ERROR (Status))
  {
    DEBUG((EFI_D_WARN,"Warning: get key1 returned: %r \n",Status));
    goto OPMExit;
  }
  
  /*perform handshake*/  
  Status = Encryption->Handshake(key1,key1_len32);
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN,"Warning: RSA decrypt call to TZ returned: %r \n",Status));
    goto OPMExit;
  }

  /*Send buffer to Encrypt*/
  Status = Encryption->Encrypt(TZ_AES_FIXED_KEY_TO_MSM,
                               (UINT8*)PrivKeyBuffer,
                               PrivKeySize,
                               &EncryptedData,                
                               &EncryptedDataSize);
  
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN,"Warning: Encrypt call to TZ returned: %r \n",Status));
    goto OPMExit;
  }
  DEBUG((EFI_D_WARN,"HW key encryption success\n"));
 
  /*Write EncryptedData to DPP*/
  Status = gBS->LocateProtocol(&gEfiDppProtocolGuid,
                               NULL,
                               (VOID**)&DppProtocol);
  ERR_HANDLE(Status);

  //write backup OPM public key and backup MSM HW key encrypted OPM private key to DPP
  DEBUG((EFI_D_WARN,"Writing opm pub key to DPP\n"));  
  Status = WriteBuffertoDPPAndCheck(PubKeyBuffer,PubKeySize,OPMpubFileName, L"QCOM",DEFAULT_COPY_LOOP_CNT);  
  ERR_HANDLE(Status);    
  
  DEBUG((EFI_D_WARN,"Writing opm priv key to DPP\n"));    
  Status = WriteBuffertoDPPAndCheck(EncryptedData,EncryptedDataSize,OPMprivFileName, L"QCOM",DEFAULT_COPY_LOOP_CNT);  
  ERR_HANDLE(Status);    
  
  OPMProvisioningComplete = TRUE;

OPMExit:

  /* Free up the key1 buffer */
  if (key1 != NULL)
  {
    gBS->FreePool(key1);
    key1 = NULL;
  }

  /* Free up the public key buffer */
  if (PubKeyBuffer != NULL)
  {
    gBS->FreePool(PubKeyBuffer);
    PubKeyBuffer = NULL;
  }

  /* Free up the private key buffer */
  if (PrivKeyBuffer != NULL)
  {  
    UncachedFreePool(PrivKeyBuffer);
    PrivKeyBuffer = NULL;
  }

  /*Delete OPM public key file from ESP*/ 
  Status = DeleteFwFile2(OPMpubFileNamePath, QCOM_FW_UPDATE_PARTITION_USER_ESP);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Warning: DeleteFwFile returned: %r on OPM public key provisioning file\n",Status));
  }  
  
  /*Delete OPM private key file from ESP*/ 
  Status = DeleteFwFile2(OPMprivFileNamePath, QCOM_FW_UPDATE_PARTITION_USER_ESP);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Warning: DeleteFwFile returned: %r on OPM private key provisioning file\n",Status));    
  }  


  // Update result flag accordingly
  if(OPMProvisioningComplete)
  {
    // Set OPM provisioning result
    FactoryResult.OPM = PROVISIONING_RESULT_SUCCESS;
    DEBUG((EFI_D_WARN,"OPM Provisioning completed.\n"));    
  }
  else
  {
    // Set OPM provisioning result
    FactoryResult.OPM = PROVISIONING_RESULT_FAIL_GENERAL;
    DEBUG((EFI_D_ERROR,"OPM Provisioning failed.\n"));    
  }

  FactoryFlag.OPM = PROVISIONING_OPERATION_NO_COMMIT; 

  return Status;
}

/**
 * This function performs following tasks for HDCP 2.x Provisioning: 
 *  
 * 1. Scan the presence of an HDCP 2.x CEK or Data in ESP
 * 2. Copies it into a buffer and calls into TZ to encrypt it
 * 3. Writes the encrypted data to the Device Provisioning Partition 
 * 4. Delete the Key data from ESP
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchHDCPProvisioning(
  IN     PROV_HDCPKEY_TYPE  KeyType,
  IN OUT UINT32 *ResetRequests
  )
{
  EFI_STATUS                       Status = EFI_UNSUPPORTED;
  EFI_STATUS                       DeleteStatus = EFI_UNSUPPORTED;
  EFI_HANDLE                       PartHandle = NULL;
  BOOLEAN                          HDCPProvisioningComplete = FALSE;
  VOID                            *KeyStoreBuffer= NULL;
  UINTN                            KeyStoreFileSize ;
  UINT8                           *EncryptedData = NULL;
  UINT32                           EncryptedDataSize ;
  EFI_ENCRYPTION_PROTOCOL         *Encryption = NULL;
  UINT16                          *pProvFileName = NULL;
  UINT16                          *pProvDppName = NULL;
  UINT8                           *pFactoryResult = NULL;
  UINT8                           *pFactoryFlag = NULL;
  EFI_ENCRYPTION_ALGORITHM_TYPE    ProvisionCmdId;

  if (ResetRequests == NULL)
  {
     Status = EFI_INVALID_PARAMETER;
     goto HDCPExit;
  }

  switch (KeyType)
  {
    case TYPE_HDCPCEK:
      DEBUG((EFI_D_WARN,"Performing HDCP 2.x CEK Provisioning...\n"));
      pProvFileName = HDCPCEKFileName;
      pProvDppName = PROVISIONING_HDCPCEK_FILE_NAME;
      pFactoryResult = &(FactoryResult.HDCPCEK);
      pFactoryFlag = &(FactoryFlag.HDCPCEK);
      ProvisionCmdId = TZ_HDCP_KEY_CEK;
      break;

    case TYPE_HDCPDATA:
      DEBUG((EFI_D_WARN,"Performing HDCP 2.x DATA Provisioning...\n"));
      pProvFileName = HDCPDataFileName;
      pProvDppName = PROVISIONING_HDCPDATA_FILE_NAME;
      pFactoryResult = &(FactoryResult.HDCPData);
      pFactoryFlag = &(FactoryFlag.HDCPData);
      ProvisionCmdId = TZ_HDCP_KEY_DATA;
      break;

    default:
      DEBUG((EFI_D_WARN,"Unknown HDCP 2.x Provisioning type %x...\n",KeyType));
      goto HDCPExit;
  }
  
  Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_USER_ESP, &PartHandle);

  HANDLE_ERROR_LABEL(Status, FwUpdateGetHandleForPartition, HDCPExit);

  /*Read the Keystore into a buffer, cached is fine for HDCP. */
  Status = ReadDataFile(PartHandle,(CHAR16*) pProvFileName, &KeyStoreBuffer, &KeyStoreFileSize, TRUE);

  HANDLE_ERROR_LABEL(Status, ReadDataFile, HDCPExit);

  Status = gBS->LocateProtocol(&gEfiEncryptionProtocolGuid,
                               NULL,
                               (VOID**)&Encryption);

  HANDLE_ERROR_LABEL(Status, gBS->LocateProtocol, HDCPExit);

  /*Send buffer to Encrypt*/
  Status = Encryption->Encrypt(ProvisionCmdId,
                               (UINT8*)KeyStoreBuffer,
                               KeyStoreFileSize,
                               &EncryptedData,                
                               &EncryptedDataSize);

  if(EFI_ERROR(Status) || (EncryptedData == NULL))
  {
    DEBUG((EFI_D_ERROR,"Error: Encrypt call to TZ returned: %r \n",Status));
    goto HDCPExit;
  }

  Status = WriteBuffertoDPPAndCheck(EncryptedData,EncryptedDataSize,pProvDppName,L"QCOM",DEFAULT_COPY_LOOP_CNT);
  
  HANDLE_ERROR_LABEL(Status, WriteBuffertoDPPAndCheck, HDCPExit);

  HDCPProvisioningComplete = TRUE;

HDCPExit:

  if (pProvFileName != NULL)
  {
    /*Delete HDCP file from EFI System Partition */ 
    DeleteStatus = DeleteFwFile2(pProvFileName, QCOM_FW_UPDATE_PARTITION_USER_ESP);

    if(EFI_ERROR(DeleteStatus))
    {
      DEBUG((EFI_D_ERROR,"Error: DeleteFwFile2 returned: %r on HDCP file\n",DeleteStatus));
    }
  }

  if ((pFactoryResult != NULL) && (ResetRequests != NULL))
  {
    // Update result flag accordingly
    if(HDCPProvisioningComplete)
    {
      *pFactoryResult = PROVISIONING_RESULT_SUCCESS;
      
      // Queue up reset request required so that future DXE can process the data and take effect
      (*ResetRequests)++;

      DEBUG((EFI_D_WARN,"HDCP Provisioning completed.\n"));
    }
    else
    {
      *pFactoryResult = PROVISIONING_RESULT_FAIL_GENERAL;
      DEBUG((EFI_D_ERROR,"HDCP Provisioning failed.\n"));
    }
  }
  
  /* Free up the HDCP key buffer */
  if (KeyStoreBuffer != NULL)
  {  
    gBS->FreePool(KeyStoreBuffer);
    KeyStoreBuffer = NULL;
  }

  /* Must also free the EncryptedData for TZ_MSM_HW_KEY type encryption. */
  if (EncryptedData != NULL)
  {
    gBS->FreePool(EncryptedData);
    EncryptedData = NULL;
  }

#if TEST_HDCP_KEY_PROVISION_WITH_DECRYPT
  if (BufTemp != NULL)
  {
    gBS->FreePool(BufTemp);
    BufTemp = NULL;
  }
#endif // TEST_HDCP_KEY_PROVISION_WITH_DECRYPT

  if (pFactoryFlag != NULL)
  {
    *pFactoryFlag = PROVISIONING_OPERATION_NO_COMMIT; 
  }
  
  return Status;
}

/**
 * This function orchestrates the 
 * Factory Provisioning process for Windows-on-ARM firmware.
 * 
 * @param ImageHandle - EFI image handle 
 * @param NeedReset - Notify the caller if system reset is 
 *                  needed to complete factory provisioning
 *                  process
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
LaunchFactoryProvisioning(
  IN EFI_HANDLE  ImageHandle,
  OUT BOOLEAN    *NeedReset
  )
{
  EFI_STATUS  Status;
  UINT32      ResetRequests ;
  BOOLEAN     SkipFactoryProvisioning = FALSE;
  BOOLEAN     SkipUEFISecureBoot = FALSE;
  BOOLEAN     SkipOPM = FALSE;

  //AsciiPrint("Launching Factory Provisioning...\n");

  *NeedReset = FALSE;

  // Check for presence of Factory Provisioning Flag file in ESP
  Status = CheckFlag(QCOM_FW_UPDATE_FLAG_FACTORY_FLAG,
                     (VOID*)&FactoryFlag);

  if(EFI_ERROR(Status))
  {
    if(Status != EFI_NOT_FOUND)
    {
      DEBUG((EFI_D_WARN,"CheckFlag returned: %r. \n",Status));
    }
    ZeroMem((VOID*)&FactoryFlag, (UINTN)sizeof(FactoryFlag));
    SkipFactoryProvisioning = TRUE;
    Status = EFI_SUCCESS;
  }
  else if(FactoryFlag.Version != PROVISIONING_COMMIT_FILE_VERSION)
  {
    DEBUG((EFI_D_WARN,"Warning: Unsupported Factory Provisioning Commit File Version!\n"));
    SkipFactoryProvisioning = TRUE;
  }

  // Migrate any existing provisioning files on GPP FAT16 (which is done through other means)
  if(SkipFactoryProvisioning || FactoryFlag.Provision != PROVISIONING_OPERATION_COMMIT)
  {
    // Note: FactoryProvisionedFlag is already retrieved previously
    if(FactoryProvisionedFlag == QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_SET)
    {
      Status = LaunchDeviceProvisioning(FALSE, &ResetRequests);
      if(!EFI_ERROR(Status))
      {
        DEBUG((EFI_D_WARN,"Provisioning Data Migrated.\n"));
        // Update Factory Provisioned Flag.
        FactoryProvisionedFlag = QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_SET_AND_MIGRATED;
        Status = SetFlag(QCOM_FW_UPDATE_FLAG_FACTORY_PROVISIONED_FLAG,
                         (VOID*)&FactoryProvisionedFlag);
      }
    }
  }

  // Skip factory provisioning if previously determined
  if(SkipFactoryProvisioning)
  {
    DEBUG((EFI_D_WARN,"Factory Provisioning skipped.\n"));
    goto LaunchFactoryProvisioningExit;
  }

  // Check for presence of Factory Provisioning Result file in ESP
  Status = GetFactoryProvisioningResult(&FactoryResult);

  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN,"Warning: GetFactoryProvisioningResult returned: %r.  Setting Factory result to default.\n",Status));
    ZeroMem((VOID*)&FactoryResult, (UINTN)sizeof(FactoryResult));
    FactoryResult.Version = PROVISIONING_RESULT_FILE_VERSION;
  }

  //
  // Check factory provisioning operations
  //

  if(FactoryFlag.Provision == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchDeviceProvisioning(TRUE, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchDeviceProvisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.OA3 == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchOA3Provisioning(&ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchOA3Provisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.SMBIOS == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchSMBIOSProvisioning(&ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchSMBIOSProvisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.HDCPCEK == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchHDCPProvisioning(TYPE_HDCPCEK, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchHDCPProvisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.HDCPData == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchHDCPProvisioning(TYPE_HDCPDATA, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchHDCPProvisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.SSD == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchSSDProvisioning(&ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchSSDProvisioning returned: %r.\n", Status));
    }
    else
    {
      /* If we successfully provisioned the SSD, we must skip provisioning the OPM keys
         until the next reboot. */
      SkipOPM = TRUE;
    }
  }

  if(FactoryFlag.EraseAll == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchEraseAllProvisioning(&ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchEraseAllProvisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.EraseOA3 == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchEraseOA3Provisioning(&ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchEraseOA3Provisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.DebugPolicy == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchDebugPolicyProvisioning(FALSE, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchEraseOA3Provisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.DebugPolicyRemove == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchDebugPolicyProvisioning(TRUE, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchEraseOA3Provisioning returned: %r.\n", Status));
    }
  }

  if(FactoryFlag.RPMB == PROVISIONING_OPERATION_COMMIT)
  {
    Status = LaunchRPMBProvisioning(ImageHandle, &ResetRequests);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_WARN,"Warning: LaunchRPMBProvisioning returned: %r.\n", Status));
    }
    else
    {
      /* If we successfully provisioned the RPMB, we must skip provisioning the UEFI secure boot variables
         until the next reboot. */
      SkipUEFISecureBoot = TRUE;
    }
  }
  if (SkipOPM == FALSE)
  {
    if(FactoryFlag.OPM == PROVISIONING_OPERATION_COMMIT)
    {
      Status = LaunchOPMProvisioning();
      if(EFI_ERROR(Status))
      {
        //AsciiPrint("Warning: LaunchEraseOA3Provisioning returned: %r.\n", Status);
      }
    }
  }
  else
  {
    SkipUEFISecureBoot = TRUE;
  }
  /* Only attempt UEFI secure boot provisioning as long as RPMB provisioning is not attempted or already provisioned. */
  if (SkipUEFISecureBoot == FALSE)
  {
    if(FactoryFlag.KEKpub == PROVISIONING_OPERATION_COMMIT)
    {
      Status = LaunchSecureBootProvisioning(TYPE_KEKPUB, &ResetRequests);
      if(EFI_ERROR(Status))
      {
        //AsciiPrint("Warning: LaunchKEKpubProvisioning returned: %r.\n", Status);
      }
    }

    if(FactoryFlag.SignatureDB == PROVISIONING_OPERATION_COMMIT)
    {
      Status = LaunchSecureBootProvisioning(TYPE_SIGNATUREDB, &ResetRequests);
      if(EFI_ERROR(Status))
      {
        //AsciiPrint("Warning: LaunchSignatureDBProvisioning returned: %r.\n", Status);
      }
    }

    if(FactoryFlag.SignatureDBX == PROVISIONING_OPERATION_COMMIT)
    {
      Status = LaunchSecureBootProvisioning(TYPE_SIGNATUREDBX, &ResetRequests);
      if(EFI_ERROR(Status))
      {
        //AsciiPrint("Warning: LaunchSignatureDBProvisioning returned: %r.\n", Status);
      }
    }

    if(FactoryFlag.PKpub == PROVISIONING_OPERATION_COMMIT)
    {
      Status = LaunchSecureBootProvisioning(TYPE_PKPUB, &ResetRequests);
      if(EFI_ERROR(Status))
      {
        //AsciiPrint("Warning: LaunchPKpubProvisioning returned: %r.\n", Status);
      }
    }
  }

  // Update factory provisioning flag file
  Status = SetFlag(QCOM_FW_UPDATE_FLAG_FACTORY_FLAG, &FactoryFlag);
  if(EFI_ERROR(Status))
  {
    //AsciiPrint("Warning: SetFlag returned: %r.\n", Status);
  }
    
  // Update factory provisioning result file
  Status = SetFactoryProvisioningResult(&FactoryResult);
  if(EFI_ERROR(Status))
  {
    //AsciiPrint("Warning: SetFactoryProvisioningResult returned: %r.\n", Status);
  }

  //AsciiPrint("Factory Provisioning completed.\n");

LaunchFactoryProvisioningExit:

  // Check if system reset is needed
  if(ResetRequests > 0)
  {
    *NeedReset = TRUE;
  }

  return Status;
}


/**
 * 
 * This function gathers the flag needed to determine 
 * the course of action for provisioning.
 * 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwProvisionCheckFlags()
{
  EFI_STATUS Status;

  //Check for presence of Factory Provisioned Flag in General Purpose Area 1
  Status = CheckFlag(QCOM_FW_UPDATE_FLAG_FACTORY_PROVISIONED_FLAG,
                     (VOID*)&FactoryProvisionedFlag);

  if(EFI_ERROR(Status))
  {
      //AsciiPrint("Warning: CheckFlag returned: %r.  Setting Factory Provisioned flag to default.\n",Status);
      FactoryProvisionedFlag = QCOM_FW_UPDATE_FLAG_VALUE_FACTORY_PROVISIONED_NOT_SET;
  }
  

  return EFI_SUCCESS;
}

/**
 * This Function does following tasks:
 *  
 *   1. Saves physical address of RSDP in a UEFI variable
 *   2. If ACPI MSDM table is not present, pre-allocate
 *      memory and reserve it for OA30 online-injection.
 * 
 * @param ImageHandle - Image handle
 * @param SystemTable - Pointer to EFI system table
 * @param ReserveMSDM - Reserve memory for MSDM (TRUE) or not (FALSE)
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
PrepareOnlineOA30Injection(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable,
  IN BOOLEAN            ReserveMSDM
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_CONFIGURATION_TABLE * pcfg_table;
  UINTN cs_cnt;
  UINTN i;
  VOID *pRSDP_V1 = NULL;
  VOID *pRSDP_V2 = NULL;
  VOID *pMSDM = NULL;
  UINT64 *Addr = NULL;
  UINT32 AddrSize = sizeof(UINT64);
  BOOLEAN FoundFile = FALSE;
    
  cs_cnt =SystemTable->NumberOfTableEntries;
  pcfg_table=SystemTable->ConfigurationTable;
        
  for (i=0;i<cs_cnt; i++)
  {
    if(CompareGuid((GUID*)&RSDP_V2_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      pRSDP_V2 = pcfg_table->VendorTable;
    }

    if(CompareGuid((GUID*)&RSDP_V1_guid,(GUID*)&pcfg_table->VendorGuid))
    {
      pRSDP_V1 = pcfg_table->VendorTable;
    }

    pcfg_table++;
  }    

  Addr = (UINT64*)AllocateZeroPool(AddrSize);
  if(Addr == NULL)
  {
      Status = EFI_OUT_OF_RESOURCES;
      goto SaveRSDPExit;
  }

  if(pRSDP_V2 != NULL)
  {
      *Addr = (UINT64)pRSDP_V2;
  }
  else if(pRSDP_V1 != NULL)
  {
      *Addr = (UINT64)pRSDP_V1;
  }

  if(*Addr != 0)
  {
      //AsciiPrint("RsdpPhyAddr = 0x%x%x\n", (*Addr)>>32, (*Addr)&0xFFFFFFFF);
      // Save RSDP physical address in UEFI run-time variable
      Status = gRT->SetVariable(L"RsdpBase",
                                &gQcomTokenSpaceGuid ,
                                EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                AddrSize,
                                Addr);

      // Save RSDP physical address in file
      //Status = WriteFwFile2(L"rsdp.bin",
      //                        QCOM_FW_UPDATE_PARTITION_USER_ESP,
      //                        (VOID**)&Addr,
      //                        &AddrSize);
  }

  if (!ReserveMSDM)
  {
    goto SaveRSDPExit;
  }
  
  // Check if ACPI MSDM table already exist
  Status = FindFwFile(QCOM_FW_UPDATE_OA3_MSDM_DEST_FILE,
                      QCOM_FW_UPDATE_PARTITION_GPP_FAT16,
                      &FoundFile);

  if(EFI_ERROR(Status))
  {
      //Failed to find the FW file, assume file not found
      FoundFile = FALSE;
  }

  // Reserve memory for ACPI MSDM table only if no prior instance exist
  if(!FoundFile)
  {
      // Current ACPI MSDM table size is 85 bytes
      Status = gBS->AllocatePool(EfiACPIReclaimMemory, 128, &pMSDM);

      if(EFI_ERROR(Status) || pMSDM == NULL)
      {
          DEBUG((EFI_D_ERROR,"Failed to reserve buffer for MSDM\n"));
          goto SaveRSDPExit;
      }

      *Addr = (UINT64)pMSDM;

      if(*Addr != 0)
      {
          //AsciiPrint("MsdmPhyAddr = 0x%x%x\n", (*Addr)>>32, (*Addr)&0xFFFFFFFF);

          // Save physical address of reserved memory in UEFI run-time variable
          Status = gRT->SetVariable(L"MsdmBase",
                                    &gQcomTokenSpaceGuid ,
                                    EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                                    AddrSize,
                                    Addr);

          // Save physical address of reserved memory in file
          //Status = WriteFwFile2(L"msdm.bin",
          //                      QCOM_FW_UPDATE_PARTITION_USER_ESP,
          //                      &Addr,
          //                      &AddrSize);
      }
  }

SaveRSDPExit:

  if(Addr != NULL)
  {
      FreePool(Addr);
  }

  return Status;
}

/**
 * The user Entry Point for provisioning. 
 * 
 * 
 * @param ImageHandle - Passed from calling application
 * @param SystemTable - Passed from calling application
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
RunFwProvisionMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status            = EFI_SUCCESS;
  
  DEBUG((EFI_D_INFO,"Entering RunFwProvisionMain:\n"));

  /* QRDProvStage1 */
  Status = QrdProvStage1();
  if(EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR,"Warning: QrdProvStage1 returned error: %r\n", Status));
    Status = EFI_SUCCESS;
    goto RunFwProvisionMainExit;
  }  

RunFwProvisionMainExit:

  return Status;
}
  
