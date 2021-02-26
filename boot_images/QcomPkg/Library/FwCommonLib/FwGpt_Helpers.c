/** @file FwGptHelpers.c
   
  GUID Partition Table (GPT) functions for FwCommonLib.

  Copyright (c) 2011-2015, Qualcomm Technologies, Inc . All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2015/04/15     mic         Added 64bit firmware update support
 2014/17/10     tj			KW Fixes
 2014/04/21     mic         Fixed KW warnings
 2012/12/11     mic         Fixed KW warnings
 2012/11/14     mic         Added robustness for GPT copy and restore functions
 2012/11/12     rs          Swap ParitionName also when switching GPT
 2012/10/10     jd          Handle GPT switch based on firmware entry list   
 2012/10/08     mic         renamed gpt functions
 2011/10/12     hozefak     Initial version

=============================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FwCommonLib.h>
#include <Library/DebugLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi/UefiGpt.h>
#include "FwGpt_Helpers.h"

typedef struct {
  /* Block IO protocol interface is returned for all type of filters */
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;

  /* This HDD dev path is returned only if Matching Partition type is requested
   * It should be noted that the contents of this memory should NOT be changed */
  const HARDDRIVE_DEVICE_PATH    *PartitionInfo;
} HandleInfo;

#define TYPE_MEDIA_DEVICE_PATH     4
#define SUBTYPE_HARD_DRIVE         1
#define TYPE_END_DEVICE_PATH       0x7F
#define QCOM_PARTITION_BOOTLOADERS 14
#define PRIMARY_HEADER_LBA         0x1
#define SIZE_OF_LBA                512

/**
 * Finds the blockIOHandle for the given Vendor GUID which is 
 * defined in MMCHS driver 
 * 
 * @param Memory_GUID - Vendor GUID to find the BlockIOHandle 
 * @param hBlockIO    - BlockIO handle
 * @return EFI_STATUS
 */

EFI_STATUS
GetEmmcTypeHandle(
    IN  EFI_GUID                Memory_GUID,
    OUT EFI_BLOCK_IO_PROTOCOL **hBlockIO
   )
{
    UINTN                     i                      = 0;
    UINTN                     NumHandles             = 0;
    EFI_HANDLE               *HandleBuffer           = NULL;
    EFI_BLOCK_IO_PROTOCOL    *BlockIoHandle          = NULL;
    EFI_DEVICE_PATH_PROTOCOL *DevPathInst            = NULL;
    VENDOR_DEVICE_PATH       *VendorData             = NULL;
    EFI_STATUS                Status                 = EFI_SUCCESS;


    if (hBlockIO == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    /* Initialize output parameter*/
    *hBlockIO = NULL;

    /* Get All BlockIO handles*/
    Status = gBS->LocateHandleBuffer(ByProtocol,
                                     &gEfiBlockIoProtocolGuid,
                                     NULL,
                                     &NumHandles,
                                     &HandleBuffer);
    HANDLE_ERROR_LABEL(Status,gBS->LocateHandleBuffer,GetEmmcTypeHandleExit);
                       
    /* Scan the BlockIO handle buffer, then compare the devpath vendor GUID with
       the input memory GUID and the DevPathType against TYPE_END_DEVICE_PATH.
       If both matches, return the BlockIO handle*/
    for (i = 0; i < NumHandles; i++)
    {
        Status = gBS->HandleProtocol (HandleBuffer[i],
                                      &gEfiBlockIoProtocolGuid,
                                      (VOID **) &BlockIoHandle);
        HANDLE_ERROR_LABEL(Status,gBS->HandleProtocol,GetEmmcTypeHandleExit);
  
    
        Status = gBS->HandleProtocol (HandleBuffer[i],
                                      &gEfiDevicePathProtocolGuid,
                                      (VOID**)&DevPathInst);
        HANDLE_ERROR_LABEL(Status,gBS->HandleProtocol,GetEmmcTypeHandleExit);
    
        VendorData = (VENDOR_DEVICE_PATH *)DevPathInst;
        DevPathInst = (EFI_DEVICE_PATH_PROTOCOL*)(((UINT8*)DevPathInst)+DevPathInst->Length[0]);
        if(DevPathInst->Type == TYPE_END_DEVICE_PATH)
        {    
            if(CompareGuid(&(VendorData->Guid) ,&Memory_GUID)) 
            {
                *hBlockIO = BlockIoHandle;
                Status = EFI_SUCCESS;
                goto GetEmmcTypeHandleExit;
            }
        }
    }

    Status = EFI_NOT_FOUND;

GetEmmcTypeHandleExit:

    return Status;

}

/**
 * This function will write the Buffer into the specified GPT
 * Table. It will also calculate the CRC's and rewrite the
 * header for the modified table
 * 
 * @param FileBuffer  :  Pointer to Buffer which needs to be
 *                       written to the GPT Tables 
 * @param FileSize    :  Pointer to the Size of the buffer
 * @param Memory_GUID :  Vendor GUID whose GPT has to be changed
 * 
 * @param TableType   : Primary or backup GPT table
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS 
WriteGPTTable(
    IN VOID         **FileBuffer,
    IN UINTN         *FileSize,
    IN EFI_GUID       Memory_GUID,
    IN GPT_TABLE_TYPE TableType
    )
{
    EFI_STATUS                  Status                 = EFI_SUCCESS;
    UINT32                      CurrentLBA             = 0;
    EFI_PARTITION_ENTRY        *StartPartitionEntry    = NULL;
    EFI_PARTITION_ENTRY        *PartitionEntry         = NULL;
    EFI_PARTITION_TABLE_HEADER *PartHdr                = NULL;
    EFI_PARTITION_TABLE_HEADER *BackupPartHdr          = NULL;
    EFI_PARTITION_TABLE_HEADER *OutPartitionHeader     = NULL;
    UINT32                      Partition_Array_CRC32  = 0;
    UINT32                      CRCforHeader           = 0;
    EFI_BLOCK_IO_PROTOCOL      *BlockIoHandleHardDrive = NULL;
    UINT32                      OutStartLBA            = 0;
   
    /* Get the BlockIO handle for the given EMMC type*/
    Status = GetEmmcTypeHandle(Memory_GUID, &BlockIoHandleHardDrive);
    HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,WriteGPTTableExit);

    if (BlockIoHandleHardDrive != NULL)
    {
      /* Allocate and zero out buffer for primary GPT */
      Status = gBS->AllocatePool(EfiBootServicesData, 
                                 BlockIoHandleHardDrive->Media->BlockSize,
                                (VOID*)&PartHdr );
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,WriteGPTTableExit);
      gBS->SetMem(PartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0); 

      /* Read current primary GPT to get the GPT info */
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PRIMARY_HEADER_LBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr);    
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->ReadBlocks,WriteGPTTableExit);

      /* Allocate and zero out buffer for Backup GPT */
      Status = gBS->AllocatePool(EfiBootServicesData, 
                                 BlockIoHandleHardDrive->Media->BlockSize,
                                 (VOID*)&BackupPartHdr );
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,WriteGPTTableExit);
      gBS->SetMem(BackupPartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 ); 
              
      /* Read current backup GPT by given PartHdr->AlternateLBA*/
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PartHdr->AlternateLBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     BackupPartHdr
                                                    );
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->ReadBlocks,WriteGPTTableExit);
      
      /* Assign the correct writing buffer and start LBA*/    
      if(TableType == BACKUP_GPT_TABLE ) 
      {   
          OutPartitionHeader = BackupPartHdr;
          OutStartLBA = PartHdr->LastUsableLBA + 1;         
      }
      else
      {   
          OutPartitionHeader = PartHdr;
          OutStartLBA = PartHdr->MyLBA + 1;        
      }

      StartPartitionEntry = (EFI_PARTITION_ENTRY*) *FileBuffer;  
      PartitionEntry = StartPartitionEntry;

      /*Calculate the PartitionEntryArrayCRC32 of the input GPT buffer*/   
      Status = gBS->CalculateCrc32 (
                      *FileBuffer, 
                      (OutPartitionHeader->NumberOfPartitionEntries * OutPartitionHeader->SizeOfPartitionEntry),
                      &Partition_Array_CRC32 );
      HANDLE_ERROR_LABEL(Status, gBS->CalculateCrc32, WriteGPTTableExit);

      OutPartitionHeader->PartitionEntryArrayCRC32 = Partition_Array_CRC32; 
      
      //Clear out the CRC to calculate the new header CRC
      OutPartitionHeader->Header.CRC32 = 0;
      Status = gBS->CalculateCrc32 (OutPartitionHeader, 
                                    OutPartitionHeader->Header.HeaderSize, 
                                    &CRCforHeader );
      HANDLE_ERROR_LABEL(Status, gBS->CalculateCrc32, WriteGPTTableExit);

      OutPartitionHeader->Header.CRC32 = CRCforHeader;
      
      //Start to write all the partition entries
      for(CurrentLBA = OutStartLBA; 
          CurrentLBA < OutStartLBA +
                      ((PartHdr->NumberOfPartitionEntries * PartHdr->SizeOfPartitionEntry)/BlockIoHandleHardDrive->Media->BlockSize);
          CurrentLBA++
         ) 
      {
         Status = BlockIoHandleHardDrive->WriteBlocks (BlockIoHandleHardDrive,
                                                       BlockIoHandleHardDrive->Media->MediaId,
                                                       CurrentLBA,
                                                       BlockIoHandleHardDrive->Media->BlockSize,
                                                       PartitionEntry
                                                       );
         HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->WriteBlocks, WriteGPTTableExit);

         PartitionEntry = PartitionEntry + 
                          (BlockIoHandleHardDrive->Media->BlockSize/sizeof(EFI_PARTITION_ENTRY));
      }

      // Write the Calculated CRC to the partition Header
      Status = BlockIoHandleHardDrive->WriteBlocks(BlockIoHandleHardDrive,
                                                   BlockIoHandleHardDrive->Media->MediaId,
                                                   OutPartitionHeader->MyLBA,
                                                   BlockIoHandleHardDrive->Media->BlockSize,
                                                   OutPartitionHeader);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->WriteBlocks, WriteGPTTableExit);
      
      /* Flush the BlockIOHandle */
      Status = BlockIoHandleHardDrive->FlushBlocks (BlockIoHandleHardDrive);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->FlushBlocks, WriteGPTTableExit);
    }
    else
    {
      Status = EFI_NOT_FOUND;
      HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,WriteGPTTableExit);
    }
    
WriteGPTTableExit:
    
    if (BackupPartHdr != NULL)
    {
      gBS->FreePool(BackupPartHdr);
      BackupPartHdr = NULL;
    }
    if (PartHdr != NULL)
    {
      gBS->FreePool(PartHdr);
      PartHdr = NULL;
    }

    return  Status;
}

/**
 * This function serves 2 purposes based on input parameter 
 * 'SwitchGpt" : 
 *  
 * If SwitchGpt = TRUE, it will switch GUID with backup and vice
 * versa as requested by parsing the input firmware entry list, 
 * and write the Buffer into the specified GPT Table. It will 
 * also calculate the CRC's and rewrite the header for the 
 * modified table 
 *  
 * If SwitchGpt = FALSE, it will go through the input firmware 
 * entry list and check if GPT entry switch is needed for the 
 * given Memory_GUID 
 * 
 * @param Memory_GUID :  Vendor GUID whose GPT has to be changed
 * @param List        :  Firmware entry list 
 * @param SwitchGpt   :  If TRUE, switch GPT entries if 
 *                       needed
 *                       If FALSE, return if GPT switch is
 *                       needed
 * @param NeedSwitchGpt: If SwitchGpt is FALSE, return if GPT 
 *                     switch is needed
 *                       If SwitchGpt is TRUE, ignored 
 * @return EFI_STATUS 
 */

EFI_STATUS 
SwitchBackupGptTableEntries(
    IN EFI_GUID             Memory_GUID,
    IN FW_LINKED_LIST_TYPE *List,
    IN BOOLEAN              SwitchGpt,
    OUT BOOLEAN            *NeedSwitchGpt
    )
{
    EFI_STATUS                    Status                 = EFI_SUCCESS;
    EFI_PARTITION_ENTRY          *StartPartitionEntry    = NULL;
    EFI_PARTITION_ENTRY          *PartitionEntry         = NULL;
    EFI_PARTITION_TABLE_HEADER   *PartHdr                = NULL;
    EFI_PARTITION_TABLE_HEADER   *BackupPartHdr          = NULL;
    EFI_PARTITION_TABLE_HEADER   *PartitionHeader        = NULL;
    UINT32                        Partition_Array_CRC32  = 0;
    UINT32                        CRCforHeader           = 0;
    EFI_BLOCK_IO_PROTOCOL        *BlockIoHandleHardDrive = NULL;
    UINT32                        StartLBA               = 0;
    UINTN                         FileSize               = 0;
	VOID						 *FileBuffer			 = NULL;
    FW_LIST_NODE_TYPE            *CurrentNode            = NULL;
    QPAYLOAD_FW_ENTRY            *FwEntry                = NULL;
    EFI_GUID                      Emmc_BOOT_GUID         = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID                      Emmc_GPP_GUID          = EFI_EMMC_GPP_PARTITION1_GUID;
    QCOM_FW_UPDATE_FW_FILE        eFwFile                = QCOM_FW_UPDATE_FILE_BP_GPT_FILE;    
    FWENTRY_EMMC_PARTITION_TYPE   Emmc_Part_Type;
    BOOLEAN                       MatchedEntryFound      = FALSE;
    UINTN                         i,j,l = 0;

    /* Check input parameters, 
       1. currently supports only Boot and GPP EMMC type
       2. List shouldn't be NULL
       3. if List-> Count == 0, return SUCCESS
       4. List -> Head == NULL is not allowed
       5. when SwitchGpt = FALSE, NeedSwitchGpt can not be NULL
     */
    /* Check 1 */
    if (CompareGuid(&Memory_GUID, &Emmc_BOOT_GUID))
    {
        Emmc_Part_Type = FWENTRY_EMMC_PARTITION_BOOT1;
        eFwFile = QCOM_FW_UPDATE_FILE_BP_GPT_FILE;
    }
    else if (CompareGuid(&Memory_GUID, &Emmc_GPP_GUID)) 
    {
        Emmc_Part_Type = FWENTRY_EMMC_PARTITION_GPP1;
        eFwFile = QCOM_FW_UPDATE_FILE_GPP_GPT_FILE;
    }
    else
    {
        Status = EFI_INVALID_PARAMETER;
        return Status;
    }

    /* Check 2 */
    if (List == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
    /* Check 3 */
    if (List->Count == 0)  
    {
        return EFI_SUCCESS;
    }

    /* Check 4 */
    if (List->Head == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
      
    /* Check 5 */
    if ((SwitchGpt == FALSE) &&
        (NeedSwitchGpt == NULL))
    {
        return EFI_INVALID_PARAMETER;
    }

    // Locate EMMC type BlockIO handler
    Status = GetEmmcTypeHandle(Memory_GUID, &BlockIoHandleHardDrive);
    HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,SwitchBackupGptTableEntriesExit);

    if ( BlockIoHandleHardDrive != NULL)
    {
      // Allocate and zero out buffer for primary GPT 
      Status = gBS->AllocatePool(EfiBootServicesData, 
                                 BlockIoHandleHardDrive->Media->BlockSize, 
                                 (VOID*)&PartHdr);
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,SwitchBackupGptTableEntriesExit);
      gBS->SetMem(PartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 ); 

      // Allocate and zero out buffer for Backup GPT 
      Status = gBS->AllocatePool(EfiBootServicesData, BlockIoHandleHardDrive->Media->BlockSize, (VOID*)&BackupPartHdr);
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,SwitchBackupGptTableEntriesExit);
      gBS->SetMem(BackupPartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 ); 

      // Read EMMC Partition header
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PRIMARY_HEADER_LBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr
                                                     );    
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->ReadBlocks, SwitchBackupGptTableEntriesExit);

      // Get the size of the Partition Table of BOOT partition
      Status = ReadGPTTable(Memory_GUID, &FileBuffer , &FileSize, BACKUP_GPT_TABLE);
	  HANDLE_ERROR_LABEL(Status, ReadGPTTable, SwitchBackupGptTableEntriesExit);

      // Read partition entries from backup
      Status = ReadFwFile(eFwFile, QCOM_FW_UPDATE_PARTITION_GPP_FAT16, (VOID**)&StartPartitionEntry, &FileSize); 
      HANDLE_ERROR_LABEL(Status, ReadFwFile, SwitchBackupGptTableEntriesExit);

      PartitionEntry = StartPartitionEntry;  
      
      /* Go through each partition entry
       for loop 1:
          divide the partition entry files into blocks, and go through each block
       for loop 2:
          devide the current block into partition entries, and go through each 
          entry
       for lopp 3:
          go through the partition entry info in the List to see if any match.
      */
      for(i = 0; i< (FileSize/BlockIoHandleHardDrive->Media->BlockSize); i++) 
      {
          for(l = 0; l < (BlockIoHandleHardDrive->Media->BlockSize/sizeof(EFI_PARTITION_ENTRY)); l++) 
          {
              CurrentNode = List->Head;
              for( j = 0; j < List->Count; j++) 
              {
                  FwEntry = (QPAYLOAD_FW_ENTRY*)(CurrentNode->Data);
                  if(FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION &&
                     FwEntry->Metadata.BackupType == FWENTRY_UPDATE_TYPE_PARTITION &&
                     FwEntry->Metadata.UpdatePath.EmmcPartitionType == Emmc_Part_Type &&
                     FwEntry->Metadata.BackupPath.EmmcPartitionType == Emmc_Part_Type)
                  {
                     if(CompareGuid(&(StartPartitionEntry->PartitionTypeGUID) ,&(FwEntry->Metadata.UpdatePath.PartitionTypeGUID)) &&
                        CompareMem(StartPartitionEntry->PartitionName, FwEntry->Metadata.UpdatePath.PartitionName, StrSize(StartPartitionEntry->PartitionName)) == 0)
                      {
                          MatchedEntryFound = TRUE;
                          StartPartitionEntry->PartitionTypeGUID = FwEntry->Metadata.BackupPath.PartitionTypeGUID;
                          CopyMem(StartPartitionEntry->PartitionName, FwEntry->Metadata.BackupPath.PartitionName, sizeof(StartPartitionEntry->PartitionName));
                          break;
                      }
                      else if(CompareGuid(&(StartPartitionEntry->PartitionTypeGUID) ,&(FwEntry->Metadata.BackupPath.PartitionTypeGUID)) &&
                              CompareMem(StartPartitionEntry->PartitionName, FwEntry->Metadata.BackupPath.PartitionName, StrSize(StartPartitionEntry->PartitionName)) == 0)
                      {
                          MatchedEntryFound = TRUE;
                          StartPartitionEntry->PartitionTypeGUID = FwEntry->Metadata.UpdatePath.PartitionTypeGUID;
                          CopyMem(StartPartitionEntry->PartitionName, FwEntry->Metadata.UpdatePath.PartitionName, sizeof(StartPartitionEntry->PartitionName));
                          break;
                      }
                  }
                  CurrentNode = CurrentNode->Next;
              }
              StartPartitionEntry ++;
          }
      }

      //Assign Original Pointer Value
      StartPartitionEntry = PartitionEntry;

      if(SwitchGpt == FALSE) 
      {
          *NeedSwitchGpt = MatchedEntryFound;
          Status = EFI_SUCCESS;
          goto SwitchBackupGptTableEntriesExit;
      }

      // Calculate the new CRC for the partition array and write to the backup partition header
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PartHdr->AlternateLBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     BackupPartHdr
                                                     );
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->ReadBlocks, SwitchBackupGptTableEntriesExit);

      Status = gBS->CalculateCrc32 (PartitionEntry, (BackupPartHdr->NumberOfPartitionEntries * BackupPartHdr->SizeOfPartitionEntry), &Partition_Array_CRC32 );
      HANDLE_ERROR_LABEL(Status, gBS->CalculateCrc32, SwitchBackupGptTableEntriesExit);
      
      PartitionHeader = BackupPartHdr;
      StartLBA = PartHdr->LastUsableLBA+1;
     
      PartitionHeader->PartitionEntryArrayCRC32 = Partition_Array_CRC32; 

      //Clear out the CRC to calculate the new header CRC
      PartitionHeader->Header.CRC32 = 0;
      Status = gBS->CalculateCrc32 (PartitionHeader, PartitionHeader->Header.HeaderSize, &CRCforHeader );
      HANDLE_ERROR_LABEL(Status, gBS->CalculateCrc32, SwitchBackupGptTableEntriesExit);

      PartitionHeader->Header.CRC32 = CRCforHeader;

      /*Write each partition entry*/
      for (i = StartLBA; 
           i < StartLBA +((PartHdr->NumberOfPartitionEntries * PartHdr->SizeOfPartitionEntry)/BlockIoHandleHardDrive->Media->BlockSize);
           i++) 
      {    
          Status = BlockIoHandleHardDrive->WriteBlocks (BlockIoHandleHardDrive,
                                                        BlockIoHandleHardDrive->Media->MediaId,
                                                        i,
                                                        BlockIoHandleHardDrive->Media->BlockSize,
                                                        PartitionEntry
                                                        );
          HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->WriteBlocks, SwitchBackupGptTableEntriesExit);

          PartitionEntry =  PartitionEntry + 
                            (BlockIoHandleHardDrive->Media->BlockSize/sizeof(EFI_PARTITION_ENTRY));
      }

      // Write the Calculated CRC to the partition Header
      Status = BlockIoHandleHardDrive->WriteBlocks(BlockIoHandleHardDrive,
                                                   BlockIoHandleHardDrive->Media->MediaId,
                                                   PartitionHeader->MyLBA,
                                                   BlockIoHandleHardDrive->Media->BlockSize,
                                                   PartitionHeader);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->WriteBlocks, SwitchBackupGptTableEntriesExit);

      /* Flush BlockIoHandleHardDrive */
      Status = BlockIoHandleHardDrive->FlushBlocks (BlockIoHandleHardDrive);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->FlushBlocks, SwitchBackupGptTableEntriesExit);
    }
    else
    {
      Status = EFI_NOT_FOUND;
      HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,SwitchBackupGptTableEntriesExit);
    }

SwitchBackupGptTableEntriesExit:

    //Free all the Allocated Memory
    if(StartPartitionEntry != NULL) 
    {
        gBS->FreePool(StartPartitionEntry);
        StartPartitionEntry = NULL;
    }

    if(PartHdr != NULL) 
    {
        gBS->FreePool(PartHdr);
        PartHdr = NULL;
    }

    if(BackupPartHdr != NULL) 
    {
        gBS->FreePool(BackupPartHdr);
        BackupPartHdr = NULL;

    }

    return Status;
}

/**
 * Modifies Boot Partition and GPP partition tables to point to the backup tables 
 * Also modifies and fills in the right CRC 
 * 
 * @param TABLE_TYPE
 * 
 * @param RestoreBootGpt 
 *  
 * @param RestoreGppGpt  
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS 
RestoreAllGPTTablesFromFat(
  IN GPT_TABLE_TYPE TABLE_TYPE,
  IN BOOLEAN RestoreBootGpt,
  IN BOOLEAN RestoreGppGpt
    )
{

    EFI_STATUS Status            = EFI_SUCCESS;
    EFI_GUID   Emmc_BOOT_GUID    = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID   Emmc_GPP_GUID     = EFI_EMMC_GPP_PARTITION1_GUID;
    UINTN      FileSize          = 0;
    VOID      *BootPartitionFile = NULL;
    VOID      *GPPartitionFile   = NULL;

   
    if(RestoreBootGpt) 
    {
        // Get Size of the Partition Table
        Status = ReadGPTTable(Emmc_BOOT_GUID , NULL , &FileSize, TABLE_TYPE );
        HANDLE_ERROR_LABEL(Status,ReadGPTTable,RestoreAllGPTTablesFromFatExit);
       
        Status = ReadFwFile(QCOM_FW_UPDATE_FILE_BP_GPT_FILE,QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &BootPartitionFile, &FileSize ); 
        HANDLE_ERROR_LABEL(Status,ReadFwFile,RestoreAllGPTTablesFromFatExit);

        Status = WriteGPTTable(&BootPartitionFile, &FileSize, Emmc_BOOT_GUID, TABLE_TYPE);
        HANDLE_ERROR_LABEL(Status,ReadFwFile,RestoreAllGPTTablesFromFatExit);
    }

    //BOOT TABLE
    if(RestoreGppGpt) 
    {
        Status = ReadGPTTable(Emmc_GPP_GUID  , NULL , &FileSize, TABLE_TYPE );
        HANDLE_ERROR_LABEL(Status,ReadGPTTable,RestoreAllGPTTablesFromFatExit);

        Status = ReadFwFile(QCOM_FW_UPDATE_FILE_GPP_GPT_FILE,QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &GPPartitionFile, &FileSize ); 
        HANDLE_ERROR_LABEL(Status,ReadFwFile,RestoreAllGPTTablesFromFatExit);

        Status = WriteGPTTable(&GPPartitionFile, &FileSize, Emmc_GPP_GUID ,TABLE_TYPE);
        HANDLE_ERROR_LABEL(Status,WriteGPTTable,RestoreAllGPTTablesFromFatExit);
    }
      
RestoreAllGPTTablesFromFatExit:

    if(BootPartitionFile != NULL)
    {
      gBS->FreePool(BootPartitionFile);
      BootPartitionFile = NULL;
    }
    if(GPPartitionFile != NULL)
    {
      gBS->FreePool(GPPartitionFile);
      GPPartitionFile = NULL;
    }

    return Status;
}

/**
 * Modifies gpt partition tables (BOOT partition and GPP) based 
 * on firmware entries in payload. 
 *
 * @param List - Firmware entry List 
 * @param ModifyBootGpt - Indicate whether or not modify BOOT 
 *                      GPT
 * @param ModifyGppGpt - Indicate whether or not modify GPP GPT 
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS 
ModifyAllBackupGptTablesForFailSafeUpdate(
    IN  FW_LINKED_LIST_TYPE *List,
    IN  BOOLEAN              ModifyBootGpt,
    IN  BOOLEAN              ModifyGppGpt
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID Emmc_BOOT_GUID = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID Emmc_GPP_GUID = EFI_EMMC_GPP_PARTITION1_GUID;
    
    // Check input parameters
    if(List->Count == 0 ||
       List->Head == NULL) 
    {
        return EFI_INVALID_PARAMETER;
    }

    if (ModifyBootGpt) 
    {
        // Switch backup GPT table entries for BOOT
        Status = SwitchBackupGptTableEntries(Emmc_BOOT_GUID, List, TRUE, NULL);
        if(EFI_ERROR(Status)) 
        {
            return Status;
        }
    }

    if(ModifyGppGpt) 
    {
        // Switch backup GPT table entries for GPP
        Status = SwitchBackupGptTableEntries(Emmc_GPP_GUID, List, TRUE, NULL);
    }
      
    return Status;
}

/**
 * Invalidate ALL Primary GPT Tables i.e USER BOOT and GPP
 * Partitions
 *
 * @param InvalidateBootGpt - Indicate whether or not invalidate 
 *                          BOOT partition GPT
 * @param InvalidateGppGpt - Indicate whether or not invalidate 
 *                         GPP partition GPT
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS 
InvalidateAllPrimaryGPT(
  IN BOOLEAN InvalidateBootGpt,
  IN BOOLEAN InvalidateGppGpt
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_GUID   Emmc_BOOT_GUID = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID   Emmc_GPP_GUID = EFI_EMMC_GPP_PARTITION1_GUID;

    if(InvalidateBootGpt) 
    {
        Status = InvalidatePrimaryGPT(Emmc_BOOT_GUID);
        if(EFI_ERROR(Status))
        {   
            return Status;
        }
    }

    if(InvalidateGppGpt) 
    {
        Status = InvalidatePrimaryGPT(Emmc_GPP_GUID);
    }
    
    return Status;
    
}

/**
 * Invalidate ALL Backup GPT Tables
 *
 * @param InvalidateBootGpt - Indicate whether or not invalidate
 *                          BOOT partition GPT
 * @param InvalidateGppGpt - Indicate whether or not invalidate 
 *                         GPP partition GPT
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS 
InvalidateAllBackUpGPT(
  IN BOOLEAN InvalidateBootGpt,
  IN BOOLEAN InvalidateGppGpt
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    EFI_GUID Emmc_BOOT_GUID = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID Emmc_GPP_GUID = EFI_EMMC_GPP_PARTITION1_GUID;

    if(InvalidateBootGpt) 
    {
      Status = InvalidateBackUpGPT(Emmc_BOOT_GUID);
      if(EFI_ERROR(Status))
      {   
        return Status;
      }
    }
        
    if(InvalidateGppGpt) 
    {
        Status = InvalidateBackUpGPT(Emmc_GPP_GUID);
    }
    
    return Status;
    

}

/**
 * Invalidate the Primary GPT Table
 *
 * @param Emmc_GUID
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS 
InvalidatePrimaryGPT(
    IN EFI_GUID Emmc_GUID
    )
{    
    EFI_STATUS                   Status = EFI_SUCCESS;
    EFI_BLOCK_IO_PROTOCOL       *BlockIoHandleHardDrive = NULL;
    EFI_PARTITION_TABLE_HEADER  *PartHdr = NULL;
        
    Status = GetEmmcTypeHandle(Emmc_GUID,&BlockIoHandleHardDrive);
    HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,InvalidatePrimaryGPTExit);

    if (BlockIoHandleHardDrive != NULL)
    {
      Status = gBS->AllocatePool(EfiBootServicesData  , BlockIoHandleHardDrive->Media->BlockSize,(VOID*)&PartHdr );
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,InvalidatePrimaryGPTExit);
      gBS->SetMem(PartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 );

      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PRIMARY_HEADER_LBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr
                                                     );
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->ReadBlocks, InvalidatePrimaryGPTExit);

      /* Invalidate the GPT by modify the CRC */
      PartHdr->Header.CRC32 = 0;
      
      Status = BlockIoHandleHardDrive->WriteBlocks(BlockIoHandleHardDrive,
                                                   BlockIoHandleHardDrive->Media->MediaId,
                                                   PRIMARY_HEADER_LBA,
                                                   BlockIoHandleHardDrive->Media->BlockSize,
                                                   PartHdr);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->WriteBlocks, InvalidatePrimaryGPTExit);

      Status = BlockIoHandleHardDrive->FlushBlocks (BlockIoHandleHardDrive);
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->FlushBlocks, InvalidatePrimaryGPTExit);
    }
    else
    {
      Status = EFI_NOT_FOUND;
      HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,InvalidatePrimaryGPTExit);
    }
               
InvalidatePrimaryGPTExit:  

    if (PartHdr != NULL)
    {
        gBS->FreePool(PartHdr);
        PartHdr = NULL;
    }

    return Status;

 }
               

/**
 * Invalidate the Backup GPT Table
 *
 * @param Emmc_GUID
 *
 * @return EFI_STATUS
 *
 */


EFI_STATUS 
InvalidateBackUpGPT(
    IN EFI_GUID Emmc_GUID
    )
{
    EFI_STATUS             Status                 = EFI_SUCCESS;
    EFI_BLOCK_IO_PROTOCOL *BlockIoHandleHardDrive = NULL;
    EFI_PARTITION_TABLE_HEADER *PartHdr           = NULL;
    EFI_PARTITION_TABLE_HEADER *PartHdr1          = NULL;
        
    Status = GetEmmcTypeHandle(Emmc_GUID,&BlockIoHandleHardDrive);
    HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,InvalidateBackUpGPTExit);
  
    if (BlockIoHandleHardDrive != NULL) 
    {
      Status = gBS->AllocatePool(EfiBootServicesData  , BlockIoHandleHardDrive->Media->BlockSize,(VOID*)&PartHdr );
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,InvalidateBackUpGPTExit);
      gBS->SetMem(PartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 );

      Status = gBS->AllocatePool(EfiBootServicesData  , BlockIoHandleHardDrive->Media->BlockSize,(VOID*)&PartHdr1 );
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,InvalidateBackUpGPTExit);
      gBS->SetMem(PartHdr1, BlockIoHandleHardDrive->Media->BlockSize, 0 );
     
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PRIMARY_HEADER_LBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr
                                                     );
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->ReadBlocks,InvalidateBackUpGPTExit);

      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PartHdr->AlternateLBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr1
                                                     );
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->ReadBlocks,InvalidateBackUpGPTExit);

      /* Invalidate the GPT by modify the CRC */
      PartHdr1->Header.CRC32 = 0;  

      Status = BlockIoHandleHardDrive->WriteBlocks(BlockIoHandleHardDrive,
                                                   BlockIoHandleHardDrive->Media->MediaId,
                                                   PartHdr->AlternateLBA,
                                                   BlockIoHandleHardDrive->Media->BlockSize,
                                                   PartHdr1);
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->WriteBlocks,InvalidateBackUpGPTExit);

      Status = BlockIoHandleHardDrive->FlushBlocks (BlockIoHandleHardDrive);
      HANDLE_ERROR_LABEL(Status,BlockIoHandleHardDrive->FlushBlocks,InvalidateBackUpGPTExit);
    }
    else
    {
      Status = EFI_NOT_FOUND;
      HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,InvalidateBackUpGPTExit);
    }              
InvalidateBackUpGPTExit:

    if (PartHdr != NULL)
    {
       gBS->FreePool(PartHdr);
       PartHdr = NULL;
    }
    if (PartHdr1 != NULL)
    {
       gBS->FreePool(PartHdr1);
       PartHdr1 = NULL;
    }

    return Status;

 }

/**
 * Reads the partition table and returns Filesize and FileBuffer
 * with data from the GPT tables. In case FileBuffer is NULL
 * will return only the FileSize by reading the GPT header
 *
 * @param Memory_GUID     : Vendor GUID for which GPT tables
 *                          have to be read
 * @param FileBuffer      : Pointer to buffer where GPT should
 *                          be read, if FileSize Function will
 *                          return only the FileSize
 * @param FileSize        : Size of the buffer to be read, i.e
 *                          size of the partition table
 * @param TableType
 * 
 *
 * @return EFI_STATUS
 *
 */

EFI_STATUS 
ReadGPTTable(
    IN  EFI_GUID         Memory_GUID,
    OUT VOID           **FileBuffer,
    OUT UINTN          *FileSize,
    IN  GPT_TABLE_TYPE   TableType
    )
{
    EFI_STATUS                  Status                 = EFI_SUCCESS;
    EFI_BLOCK_IO_PROTOCOL      *BlockIoHandleHardDrive = NULL;
    EFI_PARTITION_TABLE_HEADER *PartHdr                = NULL;
    EFI_PARTITION_ENTRY        *PartitionEntry         = NULL;
    UINT32                      i                      = 0;
    /*Logical Block Address */
    UINT32                      StartLBA               = 0;  

    /* NULL FileBuffer is allowed */
    if(FileSize == NULL) 
    {
      return EFI_INVALID_PARAMETER;
    }
    
    /* Initialize the out put buffer pointer and size */
    *FileSize = 0;

    /* Get the handle of given EMMC type Memory GUID */
    Status = GetEmmcTypeHandle(Memory_GUID , &BlockIoHandleHardDrive);
    HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,ReadGPTTableExit);

    if (BlockIoHandleHardDrive != NULL)
    {
      Status = gBS->AllocatePool(EfiBootServicesData , BlockIoHandleHardDrive->Media->BlockSize,(VOID*)&PartHdr );
      HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, ReadGPTTableExit);
      gBS->SetMem(PartHdr, BlockIoHandleHardDrive->Media->BlockSize, 0 ); 
    
      /* Read the EMMC partition Header */
      Status =  BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                     BlockIoHandleHardDrive->Media->MediaId,
                                                     PRIMARY_HEADER_LBA,
                                                     BlockIoHandleHardDrive->Media->BlockSize,
                                                     PartHdr
                                                     );    
      HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->ReadBlocks, ReadGPTTableExit);

      /* Get the Partition Table Size */
      *FileSize = (PartHdr->NumberOfPartitionEntries) * (PartHdr->SizeOfPartitionEntry);

      /* Return if FileSize is the only requestd data*/
      if (FileBuffer == NULL)
      {
          return EFI_SUCCESS;
      }

      /* Allocate and zero out a buffer to store the partition table */
      Status = gBS->AllocatePool(EfiBootServicesData, 
                                (PartHdr->NumberOfPartitionEntries * PartHdr->SizeOfPartitionEntry),
                                (VOID*)&PartitionEntry );
      HANDLE_ERROR_LABEL(Status, gBS->AllocatePool, ReadGPTTableExit);
      gBS->SetMem(PartitionEntry , (PartHdr->NumberOfPartitionEntries * PartHdr->SizeOfPartitionEntry), 0 );

      /* The output buffer points to the first partition entry */
      *FileBuffer = (VOID*)PartitionEntry;

      /* Depends on reading primary or back up GPT, Assign the correct startLBA 
         by given TableType 
      */
      if(TableType == PRIMARY_GPT_TABLE) 
      {
          StartLBA = PartHdr->MyLBA + 1;
      }
      else
      {
          StartLBA = PartHdr->LastUsableLBA + 1;
      }
      
      // Start reading all the partitions
      for( i = StartLBA; 
           i < (StartLBA + ((PartHdr->NumberOfPartitionEntries * PartHdr->SizeOfPartitionEntry)/BlockIoHandleHardDrive->Media->BlockSize)); 
           i++) 
      {    
          Status = BlockIoHandleHardDrive->ReadBlocks ( BlockIoHandleHardDrive,
                                                        BlockIoHandleHardDrive->Media->MediaId,
                                                        i,
                                                        BlockIoHandleHardDrive->Media->BlockSize,
                                                        PartitionEntry);
          HANDLE_ERROR_LABEL(Status, BlockIoHandleHardDrive->ReadBlocks, ReadGPTTableExit);

          PartitionEntry = PartitionEntry + (BlockIoHandleHardDrive->Media->BlockSize/sizeof(EFI_PARTITION_ENTRY));
      }

      return EFI_SUCCESS;
    }
    else
    {
      Status = EFI_NOT_FOUND;
      HANDLE_ERROR_LABEL(Status,GetEmmcTypeHandle,ReadGPTTableExit);
    }

ReadGPTTableExit:

    if (PartHdr != NULL)
    {
        gBS->FreePool(PartHdr);
        PartHdr = NULL;
    }
    if(FileBuffer != NULL)
    {
      if (*FileBuffer != NULL)
      {
        gBS->FreePool(*FileBuffer);
        *FileBuffer = NULL;
      }
    }
    return Status;
}


/**
 * Copy Boot Partition Table and GPP Partition Table to FAT16.bin in files BPT.TBL GPP.TBL
 *
 * @param BackupBootGpt : Indicate whether or not backup BOOT 
 *                      partition GPT
 * @param BackupGppGpt  : Indicate whether or not backup GPP 
 *                      partition GPT
 *
 * @return EFI _STATUS
 *
 */

EFI_STATUS 
CopyPartitionTabletoFat16(
   IN BOOLEAN BackupBootGpt,
   IN BOOLEAN BackupGppGpt
   )
{
    // After physical partition support is added to UEFI change Emmc_GUID to
    // corresponding Boot partition and primary partition Vendor GUID

    EFI_STATUS           Status              = EFI_SUCCESS;
    EFI_GUID             Emmc_BOOT_GUID      = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID             Emmc_GPP_GUID       = EFI_EMMC_GPP_PARTITION1_GUID;
    EFI_PARTITION_ENTRY *StartPartitionEntry = NULL;
    EFI_HANDLE           PartHandle          = NULL; 
    UINT16               *FileName           = NULL;         
    UINTN                FileSize            = 0;

    if ((BackupBootGpt == FALSE) &&
        (BackupGppGpt == FALSE))

    {
        return EFI_SUCCESS;
    }

    /* Get the handle of the destination FAT partition for GPP backup */
    Status = FwUpdateGetHandleForPartition(QCOM_FW_UPDATE_PARTITION_GPP_FAT16, 
                                           &PartHandle);
    HANDLE_ERROR_LABEL(Status,FwUpdateGetHandleForPartition,CopyPartitionTabletoFat16Exit);
    
    /* Check if it is required to back up GPT of Boot partition */
    if(BackupBootGpt) 
    {
        Status = ReadGPTTable(Emmc_BOOT_GUID,
                              (VOID*)&StartPartitionEntry, 
                              &FileSize, 
                              PRIMARY_GPT_TABLE);
        HANDLE_ERROR_LABEL(Status,ReadGPTTable,CopyPartitionTabletoFat16Exit);

        /* Get the name of the Boot partition backup file */
        Status = FwUpdateGetFileName(QCOM_FW_UPDATE_FILE_BP_GPT_FILE,&FileName);
        HANDLE_ERROR_LABEL(Status,FwUpdateGetFileName,CopyPartitionTabletoFat16Exit);

        /* Safely back up GPT table to a FAT file*/
        Status = FwUpdateFatWriteAndCheck(PartHandle,
                                          FileName,
                                          StartPartitionEntry,
                                          FileSize);
        HANDLE_ERROR_LABEL(Status,FwUpdateFatWriteAndCheck,CopyPartitionTabletoFat16Exit);
    }

    /* Free the buffer allocated in ReadGPTTable function*/
    if (StartPartitionEntry != NULL)
    {
        gBS->FreePool(StartPartitionEntry);
        StartPartitionEntry = NULL;
    }

    /* Check if it is required to back up GPT of GPP partition */
    if(BackupGppGpt) 
    {
        Status = ReadGPTTable(Emmc_GPP_GUID,
                              (VOID*)&StartPartitionEntry, 
                              &FileSize,
                              PRIMARY_GPT_TABLE );
        HANDLE_ERROR_LABEL(Status,ReadGPTTable,CopyPartitionTabletoFat16Exit);

        /* Get the name of the GPP partition backup file */
        Status = FwUpdateGetFileName(QCOM_FW_UPDATE_FILE_GPP_GPT_FILE,&FileName);
        HANDLE_ERROR_LABEL(Status,FwUpdateGetFileName,CopyPartitionTabletoFat16Exit);

        /* Safely back up GPT table to a FAT file*/
        Status = FwUpdateFatWriteAndCheck(PartHandle,
                                          FileName,
                                          StartPartitionEntry,
                                          FileSize);
        HANDLE_ERROR_LABEL(Status,FwUpdateFatWriteAndCheck,CopyPartitionTabletoFat16Exit);
    }

CopyPartitionTabletoFat16Exit:

    if (StartPartitionEntry != NULL)
    {
      gBS->FreePool(StartPartitionEntry);
      StartPartitionEntry = NULL;
    }

    return Status;
}

/**
 * Compare GPT with the table in FAT partition and check if it the swapped table 
 * or it is the primary original table 
 *
 * @param TableType       :   Type of GPT Table 
 * @param Emmc_GUID       :   Vendor GUID whose GPT has to be 
 *                        compared
 * @param gptTableState   :   returns if GPT table is in the
 *                            original or switched state
 *
 * @return EFI_STATUS
 *
 */
EFI_STATUS 
CmpGPTTable(
    IN  GPT_TABLE_TYPE   TableType,
    IN  EFI_GUID         Emmc_GUID,
    OUT GPT_TABLE_STATE *gptTableState
    )
{
    EFI_STATUS             Status                = EFI_SUCCESS;
    VOID                  *GPTfromFAT            = NULL;
    VOID                  *GPTData               = NULL;
    UINTN                  GPTfromFATSize        = 0;
    UINTN                  GPTDataSize           = 0;
    EFI_PARTITION_ENTRY   *PartitionEntryfromGPT = NULL;
    EFI_PARTITION_ENTRY   *PartitionEntryfromFAT = NULL;
    EFI_GUID               Emmc_BOOT_GUID        = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID               Emmc_GPP_GUID         = EFI_EMMC_GPP_PARTITION1_GUID;
    QCOM_FW_UPDATE_FW_FILE eFwFile               = QCOM_FW_UPDATE_FILE_BP_GPT_FILE;
    
    // Check input parameters
    if(CompareGuid(&Emmc_GUID, &Emmc_BOOT_GUID))
    {
        eFwFile = QCOM_FW_UPDATE_FILE_BP_GPT_FILE;
    }
    else if(CompareGuid(&Emmc_GUID, &Emmc_GPP_GUID)) 
    {
        eFwFile = QCOM_FW_UPDATE_FILE_GPP_GPT_FILE;
    }
    else
    {
        Status = EFI_INVALID_PARAMETER;
        return Status;
    }
    
    Status = ReadGPTTable(Emmc_GUID , &GPTData, &GPTDataSize, TableType );
    HANDLE_ERROR_LABEL(Status,ReadGPTTable,CmpGPTTableExit);

    Status = ReadFwFile(eFwFile, QCOM_FW_UPDATE_PARTITION_GPP_FAT16, &GPTfromFAT, &GPTfromFATSize ); 
    HANDLE_ERROR_LABEL(Status,ReadFwFile,CmpGPTTableExit);

    if(GPTDataSize != GPTfromFATSize) 
    {
        Status = EFI_BAD_BUFFER_SIZE;
        goto CmpGPTTableExit;
    }

    PartitionEntryfromGPT = (EFI_PARTITION_ENTRY *)GPTData;
    PartitionEntryfromFAT =(EFI_PARTITION_ENTRY *)GPTfromFAT;

    if(CompareMem((VOID*)PartitionEntryfromGPT, (VOID*)PartitionEntryfromFAT, GPTDataSize) == 0) 
    {
        *gptTableState= ORIGINAL_TABLE;  
    }
    else 
    {
        *gptTableState= SWITCHED_TABLE;
    }
    Status = EFI_SUCCESS;

CmpGPTTableExit:

    if(GPTData != NULL)
    {
        gBS->FreePool(GPTData);
        GPTData = NULL;
    }
    if(GPTfromFAT != NULL)
    {
        gBS->FreePool(GPTfromFAT);
        GPTfromFAT = NULL;
    }

    return Status;
}
       

/**
 * This function goes through the firmware entry list and checks
 * if backup GPT of BOOT and GPP1 partitions needs to be 
 * switched for fail-safe update. 
 *  
 * @param List          :  Firmware entry list 
 * @param SwitchBootGpt :  Upon return, indicates if BOOT backup
 *                      GPT needs to be switched
 * @param SwitchGppGpt  :  Upon return, indicates if GPP backup 
 *                      GPT needs to be switched
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS 
GetAllSwitchGptRequestsFromFwEntryList(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *SwitchBootGpt,
    OUT BOOLEAN            *SwitchGppGpt
    )
{
    EFI_STATUS Status         = EFI_SUCCESS;
    EFI_GUID   Emmc_BOOT_GUID = EFI_EMMC_BOOT_PARTITION1_GUID;
    EFI_GUID   Emmc_GPP_GUID  = EFI_EMMC_GPP_PARTITION1_GUID;

    if ((List == NULL) ||
        (SwitchBootGpt == NULL) ||
        (SwitchGppGpt == NULL))
    {
        return EFI_INVALID_PARAMETER;
    }
  
    Status = SwitchBackupGptTableEntries(Emmc_BOOT_GUID, List, FALSE, SwitchBootGpt);
    HANDLE_ERROR_LABEL(Status,SwitchBackupGptTableEntries,GetAllSwitchGptRequestsFromFwEntryListExit);

    Status = SwitchBackupGptTableEntries(Emmc_GPP_GUID, List, FALSE, SwitchGppGpt);
    HANDLE_ERROR_LABEL(Status,SwitchBackupGptTableEntries,GetAllSwitchGptRequestsFromFwEntryListExit);

GetAllSwitchGptRequestsFromFwEntryListExit:

    return Status;

}
