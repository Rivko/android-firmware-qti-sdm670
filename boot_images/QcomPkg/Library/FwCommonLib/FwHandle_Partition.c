/** @file PartitionHandles.c
   
  Handles core partition functions for FwCommonLib. 
   
  Maintains a registry of the partitions needed for FwUpdate 
    and provides lookup functions for these partitions and
    their appropriate read/write functions.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/11/15     mic         Added FwUpdateGetEmmcPartitionTypeGuid function
 2012/10/17     rs          Changes in FwUpdateGetPartitionHandle to check for
                            Partition Name
 2012/07/10     rs          Changes in FindinFV to check all FWFile Enums
 2011/10/12     jthompso    Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/FwCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/FirmwareVolume2.h>
#include <Include/Library/QcomLib.h>
#include "FwHandle_Partition.h"


extern GUID gEfiPartitionTypeGuid;
extern EFI_GUID gQcomTokenSpaceGuid;
extern EFI_GUID gEfiPartitionRecordGuid;

extern const QCOM_FW_UPDATE_FILE_IDENTIFIER FilePaths[];

const EFI_GUID FwEntryEmmcPartitionGuids[FWENTRY_EMMC_PARTITION_MAX]={
  EFI_EMMC_USER_PARTITION_GUID,
  EFI_EMMC_BOOT_PARTITION1_GUID,
  EFI_EMMC_BOOT_PARTITION2_GUID,
  EFI_EMMC_RPMB_PARTITION_GUID,
  EFI_EMMC_GPP_PARTITION1_GUID,
  EFI_EMMC_GPP_PARTITION2_GUID,
  EFI_EMMC_GPP_PARTITION3_GUID,
  EFI_EMMC_GPP_PARTITION4_GUID
};

// HandleRegistry keeps mapping of Partition Enums to EFI handles
// for the actual partitions.  
// Note: Each enum must match its corresponding entry's index in HandleRegistry.
static QCOM_FW_UPDATE_PARTITION_HANDLE HandleRegistry[QCOM_FW_UPDATE_PARTITION_MAX] = {
 
  {QCOM_FW_UPDATE_PARTITION_GPP_SSD,   NULL, QCOM_FW_UPDATE_PARTITION_TYPE_BLOCK, QCOM_FW_UPDATE_PARTITION_TYPE_GPP_SSD,   EFI_EMMC_GPP_PARTITION1_GUID },
  {QCOM_FW_UPDATE_PARTITION_GPP_FAT16, NULL, QCOM_FW_UPDATE_PARTITION_TYPE_FAT,   QCOM_FW_UPDATE_PARTITION_TYPE_GPP_FAT16, EFI_EMMC_GPP_PARTITION1_GUID },
  {QCOM_FW_UPDATE_PARTITION_USER_ESP,  NULL, QCOM_FW_UPDATE_PARTITION_TYPE_FAT,   QCOM_FW_UPDATE_PARTITION_TYPE_USER_ESP,  EFI_EMMC_USER_PARTITION_GUID }
};

//These are the functions used to read a file from a partition
// Each partiton type needs its own separate method
// These are dispatched in CopyFwFile
VOID* ReadFileFunctions[QCOM_FW_UPDATE_PARTITION_TYPE_MAX] ={

    FwUpdateReadFileFromBlock,
    FwUpdateReadFileFromFat,
    FwUpdateReadFileFromFV,
    DummyFunction //Need to fill this in.

};

//These are the functions used to write a file to a partition
// Each partiton type needs its own separate method
// These are dispatched in CopyFwFile
VOID* WriteFileFunctions[QCOM_FW_UPDATE_PARTITION_TYPE_MAX] ={

    FwUpdateWriteFileToBlock,
    FwUpdateWriteFileToFat,
    DummyFunction, //Should never happen, will add bounds-check.
    DummyFunction // Need to fill this in

};


//These are the functions used to read a file from a partition
// Each partiton type needs its own separate method
// These are dispatched in CopyFwFile2
VOID* ReadFileFunctions2[QCOM_FW_UPDATE_PARTITION_TYPE_MAX] ={

    DummyFunction,
    FwUpdateReadFileFromFat2,
    DummyFunction,
    DummyFunction //Need to fill this in.

};

//These are the functions used to write a file to a partition
// Each partiton type needs its own separate method
// These are dispatched in CopyFwFile2
VOID* WriteFileFunctions2[QCOM_FW_UPDATE_PARTITION_TYPE_MAX] ={

    DummyFunction,
    FwUpdateWriteFileToFat2,
    DummyFunction, //Should never happen, will add bounds-check.
    DummyFunction // Need to fill this in

};

/**
 * Function to get a EMMC type guid by a given 
 * emmc partition type enum 
 *  
 * @param EmmcType      - Emmc partition type enum
 * @Param EmmcTypeGuid  - Emmc type GUID
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetEmmcPartitionTypeGuid(
    IN  FWENTRY_EMMC_PARTITION_TYPE  EmmcType,
    OUT EFI_GUID                    *EmmcTypeGuid
    )
{
    if ((EmmcType >=  FWENTRY_EMMC_PARTITION_MAX) ||
        (EmmcTypeGuid == NULL))
    {
        return EFI_INVALID_PARAMETER;
    }

    CopyMem((VOID*)EmmcTypeGuid,(VOID*)&FwEntryEmmcPartitionGuids[EmmcType],sizeof(EFI_GUID));
    
    return EFI_SUCCESS;
  
}
/**
 * Function to get a matching partition handle
 * after comparing the Emmc type, Partition Name and 
 * Partition type GUID
 *  
 * @param PartHandle    - partition handle
 * @param EmmcType      - Emmc partition type
 * @Param PartTypeGuid  - Partition type GUID
 * @Param PartitionName - Partition name
 * @Param Protocol      - Protocol
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateGetPartitionHandle(
    IN OUT EFI_HANDLE              *PartHandle,
    IN FWENTRY_EMMC_PARTITION_TYPE  EmmcType,
    IN EFI_GUID                    *PartTypeGuid,
    IN CHAR16                      *PartitionName,
    IN EFI_GUID                    *Protocol
    )
{
  EFI_STATUS                 Status             = EFI_SUCCESS;
  EFI_HANDLE                *HandleBuffer       = NULL;
  UINTN                      NumHandles         = 0;
  UINTN                      HandleCounter      = 0;
  EFI_GUID                  *TempPartTypeGuid   = NULL;
  CHAR16                    *TempPartitionName  = NULL;
  EFI_PARTITION_ENTRY       *TempPartEntry      = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *DevPathInst        = NULL;
  VENDOR_DEVICE_PATH        *VendorData         = NULL;


  if ((PartHandle == NULL) ||
      (EmmcType >= FWENTRY_EMMC_PARTITION_MAX) ||
      (PartTypeGuid == NULL) ||

      (PartitionName == NULL) ||
      (Protocol == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
     
  Status = gBS->LocateHandleBuffer(ByProtocol,
                                  Protocol,
                                   NULL,
                                   &NumHandles,
                                   &HandleBuffer);
  HANDLE_ERROR_LABEL(Status,gBS->LocateHandleBuffer,FwUpdateGetPartitionHandleExit);
         
  for(HandleCounter = 0; HandleCounter < NumHandles; HandleCounter++)
  {   
    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                                &gEfiPartitionTypeGuid,
                                (VOID**)&TempPartTypeGuid);    
    if(EFI_ERROR(Status))
    {
      continue;
    }

    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                                 &gEfiDevicePathProtocolGuid,
                                (VOID**)&DevPathInst); 
    if(EFI_ERROR(Status))
    {
      continue;
    }


    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                               &gEfiPartitionRecordGuid,
                               (VOID **) &TempPartEntry);
    if(EFI_ERROR(Status))
    {
      continue;
    }

    TempPartitionName = TempPartEntry->PartitionName;
    VendorData = (VENDOR_DEVICE_PATH *)DevPathInst;
    DevPathInst = (EFI_DEVICE_PATH_PROTOCOL*)(((UINT8*)DevPathInst)+DevPathInst->Length[0]);

    if(DevPathInst->Type == TYPE_MEDIA_DEVICE_PATH)
    {    
      if((CompareGuid(&(VendorData->Guid) ,&(FwEntryEmmcPartitionGuids[EmmcType]))) 
         && (CompareGuid(TempPartTypeGuid,PartTypeGuid)) 
         && (CompareMem(TempPartitionName, PartitionName, StrSize(TempPartitionName)) == 0)) 
      {
        *PartHandle = HandleBuffer[HandleCounter];
        Status = EFI_SUCCESS;
        goto FwUpdateGetPartitionHandleExit;
      }
    }                   
  }
  
  Status = EFI_NOT_FOUND;

FwUpdateGetPartitionHandleExit:
  if(HandleBuffer != NULL)
  {
    gBS->FreePool(HandleBuffer);
    HandleBuffer = NULL;
  }
  
  return Status;
};

/**
 * Locates instances of a given
 *  protocol and adds them to the
 *  handle registry, if applicable.
 *  
 * @param Protocol - Protocol to locate. 
 * @return EFI_STATUS 
 */
EFI_STATUS
AddHandlesForProtocol(
    IN EFI_GUID *Protocol
    )
{

    EFI_STATUS                              Status;
    EFI_HANDLE                              *HandleBuffer;
    UINTN                                   NumHandles = 0;
    UINTN                                   HandleCounter;
    UINTN                                   RegistryCounter;
    EFI_GUID                                *PartType;
   

    Status = gBS->LocateHandleBuffer(ByProtocol,
                            Protocol,
                            NULL,
                            &NumHandles,
                            &HandleBuffer);
    ERR_HANDLE(Status);

    for(RegistryCounter = 0; RegistryCounter < QCOM_FW_UPDATE_PARTITION_MAX; RegistryCounter++)
    {        
        if(HandleRegistry[RegistryCounter].Handle == NULL)
        {
            for(HandleCounter=0;HandleCounter<NumHandles;HandleCounter++)
            {   
                    //Get Partition Type
                    Status = gBS->HandleProtocol(HandleBuffer[HandleCounter],
                                                 &gEfiPartitionTypeGuid,
                                                 (VOID**)&PartType);    
                    if(EFI_ERROR(Status))
                    {
                        continue;
                    }

                    if(CompareGuid(PartType,&(HandleRegistry[RegistryCounter].PartType)))
                    {
                        HandleRegistry[RegistryCounter].Handle = HandleBuffer[HandleCounter];
                        break;
                    }                    
            }
        }
    }
    return EFI_SUCCESS;
}   

/**
 * Function iterates through all
 *  handles to EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
 *  and EFI_BLOCK_IO_PROTOCOL and fills
 *  in the HandleRegistry.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
UpdateHandleRegistry(
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    EFI_STATUS  Status = EFI_SUCCESS;
    
    //Add BlockIo Handles
    Status = AddHandlesForProtocol(&gEfiBlockIoProtocolGuid);
    ERR_HANDLE(Status);

    //Add FileSystem Handles
    Status = AddHandlesForProtocol(&gEfiSimpleFileSystemProtocolGuid);
    ERR_HANDLE(Status);

    return Status;
}


/**
 *  
 * Returns the Handle for the 
 * given partition from the HandleRegistry. 
 * 
 * 
 * @param Part - Partition Enum, the partition whose 
 *          handle; we want. 
 * @param Handle - Pointer to the output Handle.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetHandleForPartition(
  IN  QCOM_FW_UPDATE_FW_PARTITION Part,
  OUT EFI_HANDLE                  *Handle
  )
{
  if((Part > QCOM_FW_UPDATE_PARTITION_MAX) ||
     (Handle == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if( HandleRegistry[Part].Handle != NULL)
  {
    *Handle = HandleRegistry[Part].Handle;   
    return EFI_SUCCESS;

  }else
  {
    *Handle = NULL;
    return EFI_INVALID_PARAMETER;
    /*To Do 
      Get partition type*/

    /*locate handle*/   
  }
}

/**
 * Looks up the Read function 
 *  for the appropriate partition 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Read() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetReadFunction(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    )
{

    if(Part <QCOM_FW_UPDATE_PARTITION_MAX &&
       HandleRegistry[Part].Type <QCOM_FW_UPDATE_PARTITION_TYPE_MAX)
    {
        *Fn = ReadFileFunctions[HandleRegistry[Part].Type];   
        return EFI_SUCCESS;
    }
    
    return EFI_INVALID_PARAMETER;
}
/**
 * Looks up the Write function 
 *  for the appropriate partition. 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Write() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetWriteFunction(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    )
{

    if(Part <QCOM_FW_UPDATE_PARTITION_MAX &&
       HandleRegistry[Part].Type <QCOM_FW_UPDATE_PARTITION_TYPE_MAX)
    {
        *Fn = WriteFileFunctions[HandleRegistry[Part].Type];   
        return EFI_SUCCESS;
    }
    
    return EFI_INVALID_PARAMETER;
}


/**
 * Looks up the Read function 
 *  for the appropriate partition 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Read() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetReadFunction2(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    )
{

    if(Part <QCOM_FW_UPDATE_PARTITION_MAX &&
       HandleRegistry[Part].Type <QCOM_FW_UPDATE_PARTITION_TYPE_MAX)
    {
        *Fn = ReadFileFunctions2[HandleRegistry[Part].Type];   
        return EFI_SUCCESS;
    }
    
    return EFI_INVALID_PARAMETER;
}
/**
 * Looks up the Write function 
 *  for the appropriate partition. 
 * 
 * 
 * @param Part - Partition enum
 * @param Fn - Pointer to Write() function.
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateGetWriteFunction2(
    IN  QCOM_FW_UPDATE_FW_PARTITION Part,
    VOID                            **Fn
    )
{

    if(Part <QCOM_FW_UPDATE_PARTITION_MAX &&
       HandleRegistry[Part].Type <QCOM_FW_UPDATE_PARTITION_TYPE_MAX)
    {
        *Fn = WriteFileFunctions2[HandleRegistry[Part].Type];   
        return EFI_SUCCESS;
    }
    
    return EFI_INVALID_PARAMETER;
}

/**
 * This function reads partition info into a buffer.
 * 
 * 
 * @param SrcPart - Enum for the source partition
 * @param PartInfo - Buffer to store partition info
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
ReadPartitionInfo(
    IN  QCOM_FW_UPDATE_FW_PARTITION SrcPart,
    OUT PPARTITION_INFO             PartInfo
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle;

    Status = FwUpdateGetHandleForPartition(SrcPart,
                                           &PartHandle);

    ERR_HANDLE(Status);

    Status = FwUpdateGetPartitionInfo(PartHandle, PartInfo);

    ERR_HANDLE(Status);

    return Status;
}


/**
 * Reads block data from a partition.
 *
 * @param SrcPart - Enum for the source partition
 * @param Offset  - Logical block address to read from.
 * @param Length  - Number of bytes to read. Must be
 *                  aligned to block size.
 * @param Buffer  - Buffer to store the data. 
 *
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS
ReadPartitionBlocks(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN UINT32                       Offset,
    IN OUT UINT32                   *Length,
    OUT VOID                        **Buffer
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle;
    
    Status = FwUpdateGetHandleForPartition(SrcPart,
                                           &PartHandle);

    ERR_HANDLE(Status);

    Status = FwUpdateReadBlocks(PartHandle, Offset, Length, Buffer);

    ERR_HANDLE(Status);

    return Status;
}


/**
 * Writes block data to a partition.
 *
 * @param SrcPart - Enum for the source partition
 * @param Offset  - Logical block address to write to.
 * @param Length  - Number of bytes to write. Must be
 *                  aligned to block size.
 * @param Buffer  - Buffer to store the data.
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS
WritePartitionBlocks(
    IN QCOM_FW_UPDATE_FW_PARTITION  SrcPart,
    IN UINT32                       Offset,
    IN OUT UINT32                   *Length,
    IN VOID                         **Buffer
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle;
    
    Status = FwUpdateGetHandleForPartition(SrcPart,
                                           &PartHandle);

    ERR_HANDLE(Status);

    Status = FwUpdateWriteBlocks(PartHandle, Offset, Length, Buffer);

    ERR_HANDLE(Status);

    return Status;
}

/**
 * Given a FAT directory path, retrieve the names of all files  
 * that immediately reside under specified path. This is not a 
 * recursive operation so subfolders are not pinged.
 * 
 * @param Part - Enumeration of FAT partition handle. 
 * @param FwFile - Enumeration of directory path. 
 * @param Path - Upon return, store the unicode string of 
 *             directory path
 * @param Subfolder - Upon return, store the bottom-level 
 *                subfolder name of specified directory path,
 *                e.g. if path is "\DPP\QCOM\", then return
 *                "QCOM".
 * @param FileList - Upon return, a pointer to a list of unicode 
 *                 Null-terminated file names.
 * @param FileCount - Upon return, the number of files. 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
ReadFatFileList(
    IN QCOM_FW_UPDATE_FW_PARTITION  Part,
    IN QCOM_FW_UPDATE_FW_FILE       FwFile,
    OUT CHAR16                      **Path,
    OUT CHAR16                      **Subfolder,
    OUT CHAR16                      ***FileList,
    OUT UINTN                       *FileCount
    )
{
    EFI_STATUS Status;
    EFI_HANDLE PartHandle;

    Status = FwUpdateGetHandleForPartition(Part, &PartHandle);
  
    ERR_HANDLE(Status);

    Status = FwUpdateGetFileName(FwFile, Path);

    ERR_HANDLE(Status);

    Status = FwUpdateGetFileList(PartHandle,
                                 *Path,
                                 Subfolder,
                                 FileList,
                                 FileCount);

    ERR_HANDLE(Status);
    
    return Status;
}

/**
 * Given a FW File Guid, returns whether this belongs to 
 * the set of valid Bootloader or ACPI file GUIDs possible 
 * in a payload
 * 
 * @param FileGuid  - Enumeration of FWFile GUIDs. 
 * @param ValidGuid - return whether the FileGuid is a valid 
 *            bootloader GUID or not
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
isValidBootLoaderGuid(
  IN const EFI_GUID *FileGuid,
  OUT BOOLEAN *ValidGuid
  )
{
    EFI_STATUS Status      = EFI_SUCCESS;
    EFI_GUID NullGuid      = QCOM_FW_UPDATE_NULL_GUID;
    EFI_GUID MetaDataGuid  = QCOM_FW_UPDATE_FILE_VERSION_METADATA_IN_PAYLOAD_GUID;
    EFI_GUID UpdateAppGuid = QCOM_FW_UPDATE_FILE_UPDATE_APP_IN_PAYLOAD_GUID;

    if(CompareGuid(FileGuid,&NullGuid) || CompareGuid(FileGuid,&MetaDataGuid) 
       || CompareGuid(FileGuid,&UpdateAppGuid))
    {
        *ValidGuid = FALSE;
    }
    else
    {
        *ValidGuid = TRUE;
    }
    return Status;
}

