/** @file FwPayloadEntry.h
   
  Payload entry structures and functions firmware update solution.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights 
  Reserved. Qualcomm Technologies Proprietary and Confidential. 

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 ----------     ---         --------------------------------------------------
 2015/04/15     mic         Added 64bit firmware update support
 2015/04/22     mic         Removed SSD decryption support
 2013/05/06     mic         Unsupport V1/V2 payload
 2013/02/15     rs          Updated to always try to decrypt FwEntry
 2012/12/11     mic         Fixed KW warnings
 2012/11/16     mic         Added payload header print function 
 2012/11/15     mic         Added Fail-safe fix on gpt switched case
 2012/09/21     mic         Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/FwCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/BlockIo.h>
#include "FwPayloadEntry.h"
#include "FwCapsule.h"
#include "FwEntryIO_helpers.h"

const CHAR16 *sOperationType[FWENTRY_OPERATION_TYPE_MAX] = {L"IGNORE",
                                                            L"UPDATE",
                                                            L"DELETE"
                                                           };
const CHAR16 *sUpdateType[FWENTRY_UPDATE_TYPE_MAX]       = {L"Partition",
                                                            L"FAT",
                                                            L"DPP QCOM", 
                                                            L"DPP_OEM", 
                                                            L"DPP_OPM_PRIV", 
                                                            L"SSD"
                                                           };
const CHAR16 *sBackUpType[FWENTRY_BACKUP_TYPE_MAX]       = {L"Partition",
                                                            L"FAT"
                                                           };

const CHAR16 *sEmmcType[FWENTRY_EMMC_PARTITION_MAX]      = {L"USER DATA",
                                                            L"BOOT1",
                                                            L"BOOT2",
                                                            L"RPMB",
                                                            L"GPP1",
                                                            L"GPP2",
                                                            L"GPP3",
                                                            L"GPP4"
                                                           };


/*FwEntry LinkedList */
FW_LINKED_LIST_TYPE         FwEntryList         = {0, NULL,FwEntryDestructor};
EFI_HANDLE                  FVSourceHandle      = NULL;
QPAYLOAD_METADATA_HEADER_V3 PayLoadHeader;
BOOLEAN                     bDoubleUpdateNeeded = FALSE;

/**
 * Function to print Payload header 
 * 
 * @param payloadHeader - Payload Header
 * @param DebugLevel    - Debug Level
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPrintPayloadHeader(
  IN QPAYLOAD_METADATA_HEADER_V3  *payloadHeader, 
  IN UINTN                         DebugLevel
  )
{

  DEBUG((DebugLevel,"    Payload info:\n"));
  DEBUG((DebugLevel,"    ====================================\n"));
  
  DEBUG((DebugLevel,"    Revision                 : %d\n", payloadHeader->Revision));
  DEBUG((DebugLevel,"    Header size (Byte)       : %d\n", payloadHeader->Size));
  DEBUG((DebugLevel,"    FwVer                    : 0x%x\n", payloadHeader->FirmwareVersion));
  DEBUG((DebugLevel,"    Lowest supported version : 0x%x\n", payloadHeader->LowestSupportedVersion));
  if (payloadHeader->Revision > 0x1)
  {
    DEBUG((DebugLevel,"    Breaking change number   : %d\n", payloadHeader->BreakingChangeNumber));
  }
  if (payloadHeader->Revision > 0x2)
  {    
    DEBUG((DebugLevel,"    Entry count              : %d\n", payloadHeader->EntryCount));
  }

  DEBUG((DebugLevel,"    ====================================\n"));

  return EFI_SUCCESS;
}

/**
 * Function to print FwEntry
 * 
 * @param pFwEntry - pointer point to FwEntry
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPrintFwEntry(
  IN QPAYLOAD_FW_ENTRY *pFwEntry, 
  IN UINTN             DebugLevel
  )
{

  if(pFwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG((DebugLevel,"    FwEntry:\n"));
  DEBUG((DebugLevel,"    ====================================\n"));
      
  if(DebugLevel == EFI_D_ERROR)
  {
    /*FileGuid*/
    DEBUG((DebugLevel,"      FileGuid             = "));
    FwCommonPrintGuid(pFwEntry->Metadata.FileGuid,DebugLevel);
    DEBUG((DebugLevel,"\n"));
  }
      
  /*Operation Type*/
  DEBUG((DebugLevel,"      Operation            = 0x%x,", pFwEntry->Metadata.Operation));
  if(pFwEntry->Metadata.Operation < FWENTRY_OPERATION_TYPE_MAX)
  {
    DEBUG((DebugLevel,"{%s}\n",sOperationType[pFwEntry->Metadata.Operation]));
  }
  else
  {
    DEBUG((DebugLevel,"{Unknown}\n"));
  }
  DEBUG((DebugLevel,"\n"));
      
  /*Update Type and Path*/
  DEBUG((DebugLevel,"      UpdateType           = 0x%x,", pFwEntry->Metadata.UpdateType));
  if(pFwEntry->Metadata.UpdateType< FWENTRY_UPDATE_TYPE_MAX)
  {
    DEBUG((DebugLevel,"{%s}\n",sUpdateType[pFwEntry->Metadata.UpdateType]));
  }
  else
  {
    DEBUG((DebugLevel,"{Unknown}\n"));
  }
        
  DEBUG((DebugLevel,"        EmmcPartitionType  = 0x%x,", pFwEntry->Metadata.UpdatePath.EmmcPartitionType));
  if(pFwEntry->Metadata.UpdatePath.EmmcPartitionType < FWENTRY_EMMC_PARTITION_MAX)
  {
    DEBUG((DebugLevel,"{%s}\n",sEmmcType[pFwEntry->Metadata.UpdatePath.EmmcPartitionType]));
  }
  else
  {
    DEBUG((DebugLevel,"{Unknown}\n"));
  }
      
  DEBUG((DebugLevel,"        PartitionName      = %s\n", pFwEntry->Metadata.UpdatePath.PartitionName));

  if(DebugLevel == EFI_D_ERROR)
  {
    DEBUG((DebugLevel,"        PartitionTypeGUID  = "));
    FwCommonPrintGuid(pFwEntry->Metadata.UpdatePath.PartitionTypeGUID,DebugLevel);
    DEBUG((DebugLevel,"\n"));
  }

  DEBUG((DebugLevel,"        FileName           = %s\n", pFwEntry->Metadata.UpdatePath.FileName));
  DEBUG((DebugLevel,"\n"));
        
  /*Backup Type and Path*/
  DEBUG((DebugLevel,"      BackupType           = 0x%x,", pFwEntry->Metadata.BackupType));
  if(pFwEntry->Metadata.BackupType< FWENTRY_BACKUP_TYPE_MAX)
  {
    DEBUG((DebugLevel,"{%s}\n",sBackUpType[pFwEntry->Metadata.BackupType]));
  }
  else
  {
    DEBUG((DebugLevel,"{Unknown}\n"));
  }
      
  DEBUG((DebugLevel,"        EmmcPartitionType  = 0x%x,", pFwEntry->Metadata.BackupPath.EmmcPartitionType));
  if(pFwEntry->Metadata.BackupPath.EmmcPartitionType < FWENTRY_EMMC_PARTITION_MAX)
  {
    DEBUG((DebugLevel,"{%s}\n",sEmmcType[pFwEntry->Metadata.BackupPath.EmmcPartitionType]));
  }
  else
  {
    DEBUG((DebugLevel,"{Unknown}\n"));
  }
      
  DEBUG((DebugLevel,"        PartitionName      = %s\n", pFwEntry->Metadata.BackupPath.PartitionName));

  if(DebugLevel == EFI_D_ERROR)
  {
    DEBUG((DebugLevel,"        PartitionTypeGUID  = "));
    FwCommonPrintGuid(pFwEntry->Metadata.BackupPath.PartitionTypeGUID,DebugLevel);
    DEBUG((DebugLevel,"\n"));
  }

  DEBUG((DebugLevel,"        FileName           = %s\n", pFwEntry->Metadata.BackupPath.FileName));
  DEBUG((DebugLevel,"\n"));
#if 0
  /*Crypto info*/
  DEBUG((DebugLevel,"      Encrypted            = 0x%x,", pFwEntry->IsEncrypted));
  if(pFwEntry->IsEncrypted)
  {
    DEBUG((DebugLevel,"True\n"));
  }
  else
  {
    DEBUG((DebugLevel,"False\n"));   
  }
#endif
  if (DebugLevel == EFI_D_ERROR)
  {
      DEBUG((DebugLevel,"        Decrypted address  = 0x%x\n", pFwEntry->DecryptedFwEntryBuffer));
      DEBUG((DebugLevel,"        Decrypted size     = 0x%x\n", pFwEntry->DecryptedFwEntrySize));
      DEBUG((DebugLevel,"\n      Handle address\n"));
    
      /*Handle info*/
      DEBUG((DebugLevel,"        Source             = 0x%x\n", pFwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_SOURCE]));
      DEBUG((DebugLevel,"        Dest               = 0x%x\n", pFwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_DEST]));
      DEBUG((DebugLevel,"        Backup             = 0x%x\n", pFwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_BACKUP]));

  }
    
  DEBUG((DebugLevel,"    ====================================\n"));
  
  return EFI_SUCCESS;
  
};

/**
 * Function to print FwEntry list
 * 
 * @param List - FwEntry LinkedList
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPrintFwList(
  IN FW_LINKED_LIST_TYPE *List,
  IN UINTN               DebugLevel
  )
{
  EFI_STATUS          Status        = EFI_SUCCESS;
  UINTN               NodeCount     = 0;
  FW_LIST_NODE_TYPE   *Node         = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList  = NULL;
  UINTN               i             = 0;

  if (List == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  //Get a pointer to the linked list
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwPrintFwListExit);
  
  //Get a pointer to the head node
  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwPrintFwListExit);
  
  if (Node == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  //Get number of nodes
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwPrintFwListExit);
    
  for (i = 0; i < NodeCount; i++)
  {
    Status = FwPrintFwEntry((QPAYLOAD_FW_ENTRY*) Node->Data,DebugLevel);
    HANDLE_ERROR_LABEL(Status,FwPrintFwEntry,FwPrintFwListExit);
    Node = Node->Next;
  }

FwPrintFwListExit:

  return EFI_SUCCESS;
};

/**
 * Function to destruct   QPAYLOAD_FW_ENTRY
 * 
 * @param Node - node for destruction
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryDestructor(
  IN VOID *Data
    )
{
  QPAYLOAD_FW_ENTRY *pFwEntry = NULL;

  if( Data == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  pFwEntry = (QPAYLOAD_FW_ENTRY*)Data;
  
  if(pFwEntry->DecryptedFwEntryBuffer != NULL)
  {
    /*The decrypted buffer is always uncached*/
    gBS->FreePool(pFwEntry->DecryptedFwEntryBuffer);
    pFwEntry->DecryptedFwEntryBuffer = NULL;
  }

  gBS->FreePool(pFwEntry);
  pFwEntry = NULL;
  
  return EFI_SUCCESS;
}

/**
 * Function to get  FwEntry linkedlist handle
 * 
 * @param List - double pointer of FwEntry list

 * @return : EFI_STATUS
 */
EFI_STATUS
FwGetFwEntryList(
    OUT FW_LINKED_LIST_TYPE **List
    )
{
  if(List == NULL)
  {
      return EFI_INVALID_PARAMETER;
  }
  
  if((FwEntryList.Count == 0) || (FwEntryList.Head == NULL))
  {
    return EFI_NOT_FOUND;
  }

  *List = &FwEntryList;
  return EFI_SUCCESS;
};

/**
 * . Function to get the Metadata 
 * 
 * @param Version    - metadata version
 * @param FileBuffer - Metadata buffer
 * @param BufferSize - Metadata size
 * @return : EFI_STATUS
 */
EFI_STATUS
FwFindMetadata(
    OUT UINT32 *Version,
    OUT VOID   **FileBuffer,
    OUT UINTN  *BufferSize,
    OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
    OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    )
{

  EFI_STATUS                    Status     = EFI_SUCCESS;
  QPAYLOAD_METADATA_HEADER_V1   *pMetaData = NULL;  
  EFI_GUID                      mGuid      = QCOM_FW_UPDATE_FILE_VERSION_METADATA_IN_PAYLOAD_GUID;

  if((Version == NULL) ||
     (FileBuffer == NULL) ||
     (BufferSize == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_ERR_METADATA_INVALID_PARAM;
  *FileBuffer = NULL;

  Status = FwUpdateGetFvHandle(&FVSourceHandle);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_METADATA_GET_FV_HANDLE;
    HANDLE_ERROR_LABEL(Status,QcomGetEsrtEntry,FwFindMetadataExit);
  }

  Status = FwUpdateFvRead(
              FVSourceHandle,
              &mGuid,
              FileBuffer,
              BufferSize
              );

  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_METADATA_FV_READ;
    HANDLE_ERROR_LABEL(Status,FwUpdateFvRead,FwFindMetadataExit);
  }

  /*Buffer size checking*/
  if( *BufferSize >= sizeof(QPAYLOAD_METADATA_HEADER_V1))
  {
    pMetaData = (QPAYLOAD_METADATA_HEADER_V1*)*FileBuffer;
      
    if((pMetaData->Signature1 == QPAYLOAD_METADATA_SIGNATURE1) && 
       (pMetaData->Signature2 == QPAYLOAD_METADATA_SIGNATURE2) )
    {
      *Version = pMetaData->Revision;
    }else
    { 
      Status = EFI_NOT_FOUND;
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
      *QcResultFlag = QCFW_UPDATE_ERR_METADATA_BAD_SIGNATURE;
      DEBUG_ERR_LABEL("    Bad metadata signature\n",FwFindMetadataExit);
    }
  }else
  {
    Status = EFI_BAD_BUFFER_SIZE;
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
    *QcResultFlag = QCFW_UPDATE_ERR_METADATA_BAD_HEADER_SIZE;
    DEBUG_ERR_LABEL("    Bad metadata header size\n",FwFindMetadataExit);
  }
  
  /*Get Payload metadata header*/
  switch(*Version)
  {
    case 0:
    case 1:
      if(*BufferSize == sizeof(QPAYLOAD_METADATA_HEADER_V1))
      {
        DEBUG((EFI_D_WARN,"    V1 payload found\n"));
        CopyMem((VOID*)&PayLoadHeader,*FileBuffer,sizeof(QPAYLOAD_METADATA_HEADER_V1));    
      }
      else
      {
        Status = EFI_BAD_BUFFER_SIZE;
        *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
        *QcResultFlag = QCFW_UPDATE_ERR_METADATA_BAD_V1_HEADER_SIZE;
        DEBUG_ERR_LABEL("    Bad metadata V1 header size\n",FwFindMetadataExit);
      }
      break;
  
    case 2:
      if(*BufferSize == sizeof(QPAYLOAD_METADATA_HEADER_V2))
      {
        DEBUG((EFI_D_WARN,"    V2 payload found\n"));
        CopyMem((VOID*)&PayLoadHeader,*FileBuffer,sizeof(QPAYLOAD_METADATA_HEADER_V2));
      }
      else
      {

        Status = EFI_BAD_BUFFER_SIZE;
        *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
        *QcResultFlag = QCFW_UPDATE_ERR_METADATA_BAD_V2_HEADER_SIZE;
        DEBUG_ERR_LABEL("    Bad metadata V2 header size\n",FwFindMetadataExit);
      }
      break;
  
    /*Version >= 3 should use V3 header, this provides extendable payload version 
      support*/
    case 3:
    default:
      if(*BufferSize >= sizeof(QPAYLOAD_METADATA_HEADER_V3))
      {
        DEBUG((EFI_D_WARN,"    V3 payload found\n"));
        CopyMem((VOID*)&PayLoadHeader,*FileBuffer,sizeof(QPAYLOAD_METADATA_HEADER_V3));
      }
      else
      {
        Status = EFI_BAD_BUFFER_SIZE ;
        *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
        *QcResultFlag = QCFW_UPDATE_ERR_METADATA_BAD_V3_HEADER_SIZE;
        DEBUG_ERR_LABEL("    Bad metadata V3  header size\n",FwFindMetadataExit);
      }
      break;
  }
  
  return EFI_SUCCESS;

FwFindMetadataExit:

  if( *FileBuffer != NULL)
  {
    gBS->FreePool(*FileBuffer);
    *FileBuffer = NULL;
  }
  
  return Status;
}

/**
 * Function to get partition handles
 * 
 * @param 
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntrySetPartitionHandles(
    IN OUT QPAYLOAD_FW_ENTRY* FwEntry
    )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  EFI_HANDLE                     hTemp  = NULL;
  QCOM_FW_UPDATE_STATE_VARIABLES State  = FW_STATE_NOT_IN_PROGRESS;
  EFI_GUID                       EmmcTypeGuid; 
  GPT_TABLE_STATE                GptState;

  if(FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  /* get source handle */
  Status = FwUpdateGetFvHandle(&FVSourceHandle);
  HANDLE_ERROR_LABEL(Status,FwUpdateGetFvHandle,FwEntrySetPartitionHandlesExit);

  FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_SOURCE] = FVSourceHandle;

  /* get original dest handle */
  switch(FwEntry->Metadata.UpdateType)
  {
  case FWENTRY_UPDATE_TYPE_PARTITION:
  case FWENTRY_UPDATE_TYPE_SSD:
   
    Status = FwUpdateGetPartitionHandle(&hTemp,
                                        FwEntry->Metadata.UpdatePath.EmmcPartitionType,
                                        &(FwEntry->Metadata.UpdatePath.PartitionTypeGUID),
                                        FwEntry->Metadata.UpdatePath.PartitionName,
                                        &gEfiBlockIoProtocolGuid);  
    HANDLE_ERROR_LABEL(Status,FwUpdateGetPartitionHandle,FwEntrySetPartitionHandlesExit);

    break;
    
  case FWENTRY_UPDATE_TYPE_FAT_FILE:

    Status = FwUpdateGetPartitionHandle(&hTemp,
                                        FwEntry->Metadata.UpdatePath.EmmcPartitionType,
                                        &(FwEntry->Metadata.UpdatePath.PartitionTypeGUID),
                                        FwEntry->Metadata.UpdatePath.PartitionName,
                                        &gEfiSimpleFileSystemProtocolGuid);
    HANDLE_ERROR_LABEL(Status,FwUpdateGetPartitionHandle,FwEntrySetPartitionHandlesExit);

    break;

  case FWENTRY_UPDATE_TYPE_DPP_OEM:
  case FWENTRY_UPDATE_TYPE_DPP_QCOM:
  case FWENTRY_UPDATE_TYPE_OPM_PRIV_KEY:
    hTemp = NULL;
    break;
  default:
    /* Unknown update type should be allowed if double update is needed */
    if(bDoubleUpdateNeeded)
    {
      hTemp = NULL;
      break;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      DEBUG((EFI_D_ERROR,"\nUnknown UpdateType\n"));
      goto FwEntrySetPartitionHandlesExit;
    }
    
  }
 
  FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_DEST] = hTemp;
  
  /* get original backup handle */
  switch(FwEntry->Metadata.BackupType)
  {
  case FWENTRY_BACKUP_TYPE_PARTITION:
    Status =FwUpdateGetPartitionHandle(&hTemp,
                                       FwEntry->Metadata.BackupPath.EmmcPartitionType,
                                       &(FwEntry->Metadata.BackupPath.PartitionTypeGUID),
                                       FwEntry->Metadata.BackupPath.PartitionName,
                                       &gEfiBlockIoProtocolGuid);
    HANDLE_ERROR_LABEL(Status,FwUpdateGetPartitionHandle,FwEntrySetPartitionHandlesExit);

    break;
  case FWENTRY_BACKUP_TYPE_FAT_FILE:
    Status =FwUpdateGetPartitionHandle(&hTemp,
                                       FwEntry->Metadata.BackupPath.EmmcPartitionType,
                                       &(FwEntry->Metadata.BackupPath.PartitionTypeGUID),
                                       FwEntry->Metadata.BackupPath.PartitionName,
                                       &gEfiSimpleFileSystemProtocolGuid);
    HANDLE_ERROR_LABEL(Status,FwUpdateGetPartitionHandle,FwEntrySetPartitionHandlesExit);

    break;

  default:
     /*Unknown update type should be allowed if double update is needed*/
    if(bDoubleUpdateNeeded)
    {
      hTemp = NULL;
      break;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      DEBUG((EFI_D_ERROR,"\nUnknown BackupType\n"));
      goto FwEntrySetPartitionHandlesExit;
    }
  }
  
  FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_BACKUP] = hTemp;

  /* If both update and back type are partition and EMMC type of update and 
     backup path are the same , and the state of State Machine is equal 
     to FW_STATE_BACKUP_DONE or onward, and the GPT table has been 
     switched, then exchange the destition handle and backup handle.
   */
  Status = GetState(&State);
  HANDLE_ERROR_LABEL(Status,GetState,FwEntrySetPartitionHandlesExit);

  if((FwEntry->Metadata.UpdatePath.EmmcPartitionType == FwEntry->Metadata.BackupPath.EmmcPartitionType) &&
     (FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) &&
     (FwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION) &&
     (State >= FW_STATE_BACKUP_DONE))
  {

    Status = FwUpdateGetEmmcPartitionTypeGuid(FwEntry->Metadata.UpdatePath.EmmcPartitionType,
                                              &EmmcTypeGuid);
    HANDLE_ERROR_LABEL(Status,FwUpdateGetEmmcPartitionTypeGuid,FwEntrySetPartitionHandlesExit);

    Status = CmpGPTTable(BACKUP_GPT_TABLE,EmmcTypeGuid,&GptState);
    HANDLE_ERROR_LABEL(Status,CmpGPTTable,FwEntrySetPartitionHandlesExit);

    if(GptState == SWITCHED_TABLE)
    {
       hTemp = FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_BACKUP]; 
      
       FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_BACKUP] 
                = FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_DEST];
      
       FwEntry->Handle[FWENTRY_OPERATION_PATH_TYPE_DEST] = hTemp;

    }   
  }
FwEntrySetPartitionHandlesExit:

  return Status;
}

/**
 * Function to decrypt a payload v3 fw entry
 * 
 * @param FwEntry - payload fw entry

 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryCrypto(
    IN QPAYLOAD_FW_ENTRY* FwEntry,
    OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
    OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    )
{
  EFI_STATUS Status           = EFI_SUCCESS;
  VOID       *SrcBuf          = NULL;
  UINTN      SrcBufSize       = 0;
  VOID       *DecryptedBuf    = NULL;
  UINTN      DecryptedBufSize = 0;  

  if((FwEntry == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  FwEntry->IsEncrypted = FALSE;

  if(FwEntry->Metadata.Operation != FWENTRY_OPERATION_TYPE_UPDATE)
  {
    return EFI_SUCCESS;
  }
  Status = FwUpdateReadEntryFromFV(FWENTRY_OPERATION_PATH_TYPE_SOURCE,
                                       FwEntry,
                                       &SrcBuf,
                                       &SrcBufSize);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_READ_ENTRY_FROM_FV;
    HANDLE_ERROR_LABEL(Status,FwUpdateReadEntryFromFV,FwEntryCryptoExit);
  }

  switch (FwEntry->Metadata.UpdateType)
  {
  case FWENTRY_UPDATE_TYPE_PARTITION:
  case FWENTRY_UPDATE_TYPE_FAT_FILE:
  case FWENTRY_UPDATE_TYPE_DPP_QCOM:
  case FWENTRY_UPDATE_TYPE_DPP_OEM:
  /* SSD decryption is no longer supported */
  //Status = FwCryptoSsdDecrypt(SrcBuf,SrcBufSize,&DecryptedBuf,&DecryptedBufSize);
  //if (EFI_ERROR(Status))
  //{
  //  *QcResultFlag = QCFW_UPDATE_ERR_CRYPTO_SSD_DECRYPT;
  //  HANDLE_ERROR_LABEL(Status,FwCryptoSsdDecrypt,FwEntryCryptoExit);
  //}

  break;
    
  case FWENTRY_UPDATE_TYPE_OPM_PRIV_KEY:
    
    Status = FWCryptoOpmPrivKey(SrcBuf,SrcBufSize,&DecryptedBuf,&DecryptedBufSize);
    if (EFI_ERROR(Status))
    {
      *QcResultFlag = QCFW_UPDATE_ERR_CRYPTO_OPM_PRIV_KEY_CRYPTO;
      HANDLE_ERROR_LABEL(Status,FWCryptoOpmPrivKey,FwEntryCryptoExit);
    }
	FwEntry->IsEncrypted = TRUE;
    break;
    
  case FWENTRY_UPDATE_TYPE_SSD:
    
    Status = FwCryptoMsmEncrypt(SrcBuf,SrcBufSize,&DecryptedBuf,&DecryptedBufSize);
    if (EFI_ERROR(Status))
    {
      *QcResultFlag = QCFW_UPDATE_ERR_CRYPTO_MSM_KEY_ENCRYPT;
      HANDLE_ERROR_LABEL(Status,FwCryptoMsmEncrypt,FwEntryCryptoExit);
    }
	FwEntry->IsEncrypted = TRUE;
    break;
    
  default:
    /*Unknown update type should be allowed if double update is needed*/
    if(bDoubleUpdateNeeded)
    {
      goto FwEntryCryptoExit;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_UNKNOWN_UPDATE_TYPE;
      DEBUG_ERR_LABEL("Unknown UpdateType\n",FwEntryCryptoExit);
    }
  }
  
  if(SrcBuf != NULL)
  {
    gBS->FreePool(SrcBuf);
    SrcBuf = NULL;
  }
  
  FwEntry->DecryptedFwEntryBuffer = DecryptedBuf;
  FwEntry->DecryptedFwEntrySize = DecryptedBufSize;

  return EFI_SUCCESS;
  
FwEntryCryptoExit:
  
  if(SrcBuf != NULL)
  {
    gBS->FreePool(SrcBuf);
    SrcBuf = NULL;
  }
  if(DecryptedBuf != NULL)
  {
    UncachedFreePool(DecryptedBuf);
    DecryptedBuf = NULL;
  }
  return Status;
}

/**
 *   Combination masking for Dpp fields
 * 
 * @param DevPath      - Device path
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryDevicePathMaskingForDpp(
  IN OUT FWENTRY_DEVICE_PATH *DevPath
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(DevPath == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  DevPath->EmmcPartitionType = FWENTRY_EMMC_PARTITION_DONT_CARE;
  ZeroMem((VOID*)DevPath->PartitionName, PARTITION_NAME_MAX_CHAR16_SIZE * sizeof(CHAR16));
  ZeroMem((VOID*)&DevPath->PartitionTypeGUID, sizeof(EFI_GUID)); 

  return Status;
};

/**
 *   Combination masking for FAT fields, currently all fields are required
 * 
 * @param DevPath      - Device path
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryDevicePathMaskingForFat(
  IN OUT FWENTRY_DEVICE_PATH *DevPath
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(DevPath == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  return Status;
};


/**
 *   Combination masking for partition fields
 * 
 * @param DevPath      - Device path
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryDevicePathMaskingForPartition(
    IN OUT FWENTRY_DEVICE_PATH *DevPath
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(DevPath == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  ZeroMem(DevPath->FileName, FILE_PATH_MAX_SIZE * sizeof(CHAR16));

  return Status;
};

/**
 *   Function to check a payload v3 fw entry value
 * 
 * @param mData       - metadata
 * @param bEntryGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryCombinationChecking(
    IN FWENTRY_METADATA *mData,
    OUT BOOLEAN         *bEntryGood
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if((mData == NULL) || (bEntryGood == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *bEntryGood = FALSE;

  /*Combination check only validate known OperationType/UpdateType/BackupType */

  if(mData->Operation == FWENTRY_OPERATION_TYPE_UPDATE)
  {
    if((mData->UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) ||
       (mData->UpdateType == FWENTRY_UPDATE_TYPE_SSD))
    {
      Status = FwEntryDevicePathMaskingForPartition(&mData->UpdatePath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForPartition,FwEntryCombinationCheckingExit);
    }
    else if(mData->UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE)
    {
      Status = FwEntryDevicePathMaskingForFat(&mData->UpdatePath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForFat,FwEntryCombinationCheckingExit);
    }
    else if((mData->UpdateType == FWENTRY_UPDATE_TYPE_DPP_QCOM) ||
       (mData->UpdateType == FWENTRY_UPDATE_TYPE_DPP_OEM) ||
       (mData->UpdateType == FWENTRY_UPDATE_TYPE_OPM_PRIV_KEY))
    {
      Status = FwEntryDevicePathMaskingForDpp(&mData->UpdatePath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForDpp,FwEntryCombinationCheckingExit);
    }else
    {   
      if(!bDoubleUpdateNeeded)
      {
        Status = EFI_UNSUPPORTED;
        DEBUG_ERR_LABEL("Unsupported OperationType + UpdateType",FwEntryCombinationCheckingExit);
      }
    }

    if(mData->BackupType== FWENTRY_BACKUP_TYPE_PARTITION)
    {
      Status = FwEntryDevicePathMaskingForPartition(&mData->BackupPath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForPartition,FwEntryCombinationCheckingExit);
    }
  
    else if(mData->BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE)
    {
      Status = FwEntryDevicePathMaskingForFat(&mData->BackupPath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForFat,FwEntryCombinationCheckingExit);
    }
    else
    {
      if(!bDoubleUpdateNeeded)
      {
        Status = EFI_UNSUPPORTED;
        DEBUG_ERR_LABEL("Unsupported OperationType + BackUpType",FwEntryCombinationCheckingExit);
      }
    }
  }
  else if(mData->Operation == FWENTRY_OPERATION_TYPE_DELETE)
  {
    if(mData->UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE)
    {
      Status = FwEntryDevicePathMaskingForFat(&mData->UpdatePath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForFat,FwEntryCombinationCheckingExit);
    }
    else
    {
      if(!bDoubleUpdateNeeded)
      {
        Status = EFI_UNSUPPORTED;
        DEBUG_ERR_LABEL("Unsupported OperationType + UpdateType",FwEntryCombinationCheckingExit);
      }
    }

    if(mData->BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE)
    {
      Status = FwEntryDevicePathMaskingForFat(&mData->BackupPath);
      HANDLE_ERROR_LABEL(Status,FwEntryDevicePathMaskingForFat,FwEntryCombinationCheckingExit);
    }
    else
    {
      if(!bDoubleUpdateNeeded)
      {
        Status = EFI_UNSUPPORTED;
        DEBUG_ERR_LABEL("Unsupported OperationType + BackupType",FwEntryCombinationCheckingExit);
      }
    }
  }
  else
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("Unsupported OperationType",FwEntryCombinationCheckingExit);
  }

  *bEntryGood = TRUE;
  return EFI_SUCCESS;

FwEntryCombinationCheckingExit:

  return Status;
};

/**
 *   Function to check a payload v3 fw entry value
 * 
 * @param mData       - metadata
 * @param bEntryGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryValueChecking(
    IN FWENTRY_METADATA *mData,
    OUT BOOLEAN         *bEntryGood
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if((mData == NULL) || (bEntryGood == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *bEntryGood = FALSE;

  if(mData->Operation >= FWENTRY_OPERATION_TYPE_MAX)
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("\nUnknown Operation type\n",FwEntryValueCheckingExit);
  }

  if((mData->UpdateType >= FWENTRY_UPDATE_TYPE_MAX))
  {
    if(!bDoubleUpdateNeeded)
    {
      Status = EFI_UNSUPPORTED;
      DEBUG_ERR_LABEL("\nUnknown UpdateType\n",FwEntryValueCheckingExit);
    }
  }

  if((mData->BackupType >= FWENTRY_BACKUP_TYPE_MAX))
  {
    if(!bDoubleUpdateNeeded)
    {
      Status = EFI_UNSUPPORTED;
      DEBUG_ERR_LABEL("\nUnknown BackupType\n",FwEntryValueCheckingExit);
    }
  }

  if((mData->UpdatePath.EmmcPartitionType >= FWENTRY_EMMC_PARTITION_MAX))
  {
    if(!bDoubleUpdateNeeded)
    {
      Status = EFI_UNSUPPORTED;
      DEBUG_ERR_LABEL("\nUnknown Update EmmcType\n",FwEntryValueCheckingExit);
    }
  }

  if((mData->BackupPath.EmmcPartitionType >= FWENTRY_EMMC_PARTITION_MAX))
  {
    if(!bDoubleUpdateNeeded)
    {
      Status = EFI_UNSUPPORTED;
      DEBUG_ERR_LABEL("\nUnknown Update EmmcType\n",FwEntryValueCheckingExit);
    }
  }

  *bEntryGood = TRUE;
  return EFI_SUCCESS;

FwEntryValueCheckingExit:

  return Status;
};


/**
 *   Function to validate a payload v3 fw entry
 * 
 * @param mData      - metadata
 * @param bEntryGood - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwEntryValidation(
    IN FWENTRY_METADATA *mData,
    OUT BOOLEAN         *bEntryGood,
    OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
    OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if((mData == NULL) || 
     (bEntryGood == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))

  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;
  *bEntryGood = FALSE;

  Status = FwEntryValueChecking(mData,bEntryGood);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_VALUE_CHECKING;
    HANDLE_ERROR_LABEL(Status,FwEntryValueChecking,FwEntryValidationExit);
  }
  
  if(!*bEntryGood)
  {
    Status = EFI_UNSUPPORTED;
    *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_BAD_VALUE;
    DEBUG_ERR_LABEL("\nFwEntry value validation failed\n",FwEntryValidationExit);
  }

  Status = FwEntryCombinationChecking(mData,bEntryGood);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_COMBINATION_CKECKING;
    HANDLE_ERROR_LABEL(Status,FwEntryCombinationChecking,FwEntryValidationExit);
  }
   
  if(!*bEntryGood)
  {
    Status = EFI_UNSUPPORTED;
    *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_BAD_COMBINATION;
    DEBUG_ERR_LABEL("\nFwEntry Combination validation failed\n",FwEntryValidationExit);
  }

  return EFI_SUCCESS;

FwEntryValidationExit:

  return Status;
};

/**
 *   Function to check if a Fat Update/Backup DevicePath is  execlusive in 
 *   a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListFatDevicePathExclusiveChecking(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *bListGood
    )
{  
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN               NodeCount       = 0;
  UINTN               TarGetNodeCount = 0;
  FW_LIST_NODE_TYPE   *TargetNode    = NULL;
  FW_LIST_NODE_TYPE   *HeadNode      = NULL;
  FW_LIST_NODE_TYPE   *BaseNode      = NULL;
  QPAYLOAD_FW_ENTRY   *BaseFwEntry   = NULL;
  QPAYLOAD_FW_ENTRY   *TargetFwEntry = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList   = NULL;
  UINTN               i              = 0;

  if((List == NULL) || (bListGood == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }

  *bListGood = FALSE;
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwListFatDevicePathExclusiveCheckingExit);
    
  Status = LinkedListGetHeadNode(FwEntryList,&HeadNode);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwListFatDevicePathExclusiveCheckingExit);
    
  if (HeadNode == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
    
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwListFatDevicePathExclusiveCheckingExit);
     
  BaseNode = HeadNode;

  for (i = 0; i < NodeCount; i++)
  {
    if (BaseNode != NULL)
    {
      BaseFwEntry = (QPAYLOAD_FW_ENTRY*)BaseNode->Data;

      /*case 1: BaseFwEntry's updatePath VS BaseFwEntry's BackupPath*/
      if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE) &&
         (BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE))
      {     
        if(CompareMem(&BaseFwEntry->Metadata.UpdatePath,
                      &BaseFwEntry->Metadata.BackupPath,
                      sizeof(FWENTRY_DEVICE_PATH)) == 0)
        {
          *bListGood = FALSE;
          goto FwListFatDevicePathExclusiveCheckingExit;
        }
      }

      TargetNode = BaseNode->Next;

      while((TargetNode != NULL) && (TarGetNodeCount <= (NodeCount)))
      {
        TargetFwEntry = (QPAYLOAD_FW_ENTRY*)TargetNode->Data;

        /*case 2: BaseFwEntry's updatePath VS TargetFwEntry's UpdatePath*/
        if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE) &&
           (TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE))
        {        
          if(CompareMem(&BaseFwEntry->Metadata.UpdatePath,
                        &TargetFwEntry->Metadata.UpdatePath,
                        sizeof(FWENTRY_DEVICE_PATH)) == 0)
          {
            *bListGood = FALSE;
            goto FwListFatDevicePathExclusiveCheckingExit;
          }
        }

        /*case 3: BaseFwEntry's updatePath VS TargetFwEntry's BackupPath*/
        if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE) &&
           (TargetFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE))
        {        
          if(CompareMem(&BaseFwEntry->Metadata.UpdatePath,
                        &TargetFwEntry->Metadata.BackupPath,
                        sizeof(FWENTRY_DEVICE_PATH)) == 0)
          {
            *bListGood = FALSE;
            goto FwListFatDevicePathExclusiveCheckingExit;
          }
        }

        /*case 4: BaseFwEntry's BackupPath VS TargetFwEntry's UpdatePath*/
        if((BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE) &&
           (TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_FAT_FILE))
        {        
          if(CompareMem(&BaseFwEntry->Metadata.BackupPath,
                        &TargetFwEntry->Metadata.UpdatePath,
                        sizeof(FWENTRY_DEVICE_PATH)) == 0)
          {
            *bListGood = FALSE;
            goto FwListFatDevicePathExclusiveCheckingExit;
          }
        }
        /*case 5: BaseFwEntry's BackupPath VS TargetFwEntry's BackupPath*/
        if((BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE) &&
           (TargetFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE))
        {        
          if(CompareMem(&BaseFwEntry->Metadata.BackupPath,
                        &TargetFwEntry->Metadata.BackupPath,
                        sizeof(FWENTRY_DEVICE_PATH)) == 0)
          {
            *bListGood = FALSE;
            goto FwListFatDevicePathExclusiveCheckingExit;
          }
        }
        TargetNode = TargetNode->Next; 
        TarGetNodeCount++;
      }        
    }
    else
    {
      break;
    }
    BaseNode = BaseNode->Next;
  }

  *bListGood = TRUE;

  return EFI_SUCCESS;

    
FwListFatDevicePathExclusiveCheckingExit:
       
  return Status;

}


/**
 *   Function to check if a DppItem is execlusive in 
 *   a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListOemDppItemNameExclusiveChecking(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *bListGood
    )
{  
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN               NodeCount       = 0;
  UINTN               TarGetNodeCount = 0;
  FW_LIST_NODE_TYPE   *TargetNode    = NULL;
  FW_LIST_NODE_TYPE   *HeadNode      = NULL;
  FW_LIST_NODE_TYPE   *BaseNode      = NULL;
  QPAYLOAD_FW_ENTRY   *BaseFwEntry   = NULL;
  QPAYLOAD_FW_ENTRY   *TargetFwEntry = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList   = NULL;
  UINTN               i              = 0;
  
  if((List == NULL) || (bListGood == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  
  *bListGood = FALSE;
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwListOemDppItemNameExclusiveCheckingExit);
     
  Status = LinkedListGetHeadNode(FwEntryList,&HeadNode);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwListOemDppItemNameExclusiveCheckingExit);
     
  if (HeadNode == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
     
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwListOemDppItemNameExclusiveCheckingExit);
      
  BaseNode = HeadNode;

  for (i = 0; i < NodeCount; i++)
  {
    if (BaseNode != NULL)
    {
      BaseFwEntry = (QPAYLOAD_FW_ENTRY*)BaseNode->Data;

      if(BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_DPP_OEM) 
      {     
        TargetNode = BaseNode->Next;  

        while((TargetNode != NULL) && (TarGetNodeCount <= (NodeCount)))
        {     
          TargetFwEntry = (QPAYLOAD_FW_ENTRY*)TargetNode->Data;
         
          if(TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_DPP_OEM)
          {
            if(CompareMem(BaseFwEntry->Metadata.UpdatePath.FileName, 
                          TargetFwEntry->Metadata.UpdatePath.FileName,
                          FILE_PATH_MAX_SIZE) == 0)
            {
              *bListGood = FALSE;
              goto FwListOemDppItemNameExclusiveCheckingExit;
            }
          } 

          TargetNode = TargetNode->Next; 
          TarGetNodeCount++;
        }
      }
      else
      {
        BaseNode = BaseNode->Next;
        continue;
      }     
    }
    else
    {
      break;
    }
    
  }

  *bListGood = TRUE;
  return EFI_SUCCESS;
    
FwListOemDppItemNameExclusiveCheckingExit:
       
  return Status;

}

/**
 *   Function to check if a DppItem is execlusive in 
 *   a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListQcomDppItemNameExclusiveChecking(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *bListGood
    )
{  
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN               NodeCount       = 0;
  UINTN               TarGetNodeCount = 0;
  FW_LIST_NODE_TYPE   *TargetNode    = NULL;
  FW_LIST_NODE_TYPE   *HeadNode      = NULL;
  FW_LIST_NODE_TYPE   *BaseNode      = NULL;
  QPAYLOAD_FW_ENTRY   *BaseFwEntry   = NULL;
  QPAYLOAD_FW_ENTRY   *TargetFwEntry = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList   = NULL;
  UINTN               i              = 0;
  
  if((List == NULL) || (bListGood == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  
  *bListGood = FALSE;
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwListQcomDppItemNameExclusiveCheckingExit);
     
  Status = LinkedListGetHeadNode(FwEntryList,&HeadNode);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwListQcomDppItemNameExclusiveCheckingExit);
     
  if (HeadNode == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
     
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwListQcomDppItemNameExclusiveCheckingExit);
      
  BaseNode = HeadNode;

  for (i = 0; i < NodeCount; i++)
  {
    if (BaseNode != NULL)
    {
      BaseFwEntry = (QPAYLOAD_FW_ENTRY*)BaseNode->Data;

      if(BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_DPP_QCOM) 
      {     
        TargetNode = BaseNode->Next;  

        while((TargetNode != NULL) && (TarGetNodeCount <= (NodeCount)))
        {     
          TargetFwEntry = (QPAYLOAD_FW_ENTRY*)TargetNode->Data;
         
          if(TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_DPP_QCOM)
          {
            if(CompareMem(BaseFwEntry->Metadata.UpdatePath.FileName, 
                          TargetFwEntry->Metadata.UpdatePath.FileName,
                          FILE_PATH_MAX_SIZE) == 0)
            {
              *bListGood = FALSE;
              goto FwListQcomDppItemNameExclusiveCheckingExit;
            }
          } 

          TargetNode = TargetNode->Next; 
          TarGetNodeCount++;
        }
      }
      else
      {
        BaseNode = BaseNode->Next;
        continue;
      }     
    }
    else
    {
      break;
    }
    
  }

  *bListGood = TRUE;
  return EFI_SUCCESS;
    
FwListQcomDppItemNameExclusiveCheckingExit:
       
  return Status;

}

/**
 *   Function to check if a partition Update/Backup DevicePath is execlusive in 
 *   a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListPartitionDevicePathExclusiveChecking(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *bListGood
    )
{
  /*Emmc Type +Partition Type guid + Partition name should be exclusive in 
   Update/Backup path if updateType/BackupType is partition.*/ 
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN               NodeCount       = 0;
  UINTN               TarGetNodeCount = 0;
  FW_LIST_NODE_TYPE   *TargetNode    = NULL;
  FW_LIST_NODE_TYPE   *HeadNode      = NULL;
  FW_LIST_NODE_TYPE   *BaseNode      = NULL;
  QPAYLOAD_FW_ENTRY   *BaseFwEntry   = NULL;
  QPAYLOAD_FW_ENTRY   *TargetFwEntry = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList   = NULL;
  UINTN               i              = 0;

  if((List == NULL) || (bListGood == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }

  *bListGood = FALSE;
  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwListPartitionDevicePathExclusiveCheckingExit);
    
  Status = LinkedListGetHeadNode(FwEntryList,&HeadNode);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwListPartitionDevicePathExclusiveCheckingExit);
    
  if (HeadNode == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
    
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwListPartitionDevicePathExclusiveCheckingExit);
     
  BaseNode = HeadNode;

  for (i = 0; i < NodeCount; i++)
  {
    if (BaseNode != NULL)
    {
      BaseFwEntry = (QPAYLOAD_FW_ENTRY*)BaseNode->Data;

      /*case 1: BaseFwEntry's updatePath VS BaseFwEntry's BackupPath*/
      if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) &&
         (BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION))
      {     
        if((BaseFwEntry->Metadata.UpdatePath.EmmcPartitionType == 
            BaseFwEntry->Metadata.BackupPath.EmmcPartitionType) &&
           (CompareGuid((GUID*)&BaseFwEntry->Metadata.UpdatePath.PartitionTypeGUID,
                        (GUID*)&BaseFwEntry->Metadata.BackupPath.PartitionTypeGUID)) &&
           (CompareMem(BaseFwEntry->Metadata.UpdatePath.PartitionName, 
                       BaseFwEntry->Metadata.BackupPath.PartitionName,
                       PARTITION_NAME_MAX_CHAR16_SIZE*sizeof(CHAR16)) == 0))
        {
          *bListGood = FALSE;
          goto FwListPartitionDevicePathExclusiveCheckingExit;
        }
      }

      TargetNode = BaseNode->Next;

      while((TargetNode != NULL) && (TarGetNodeCount <= (NodeCount)))
      {
        TargetFwEntry = (QPAYLOAD_FW_ENTRY*)TargetNode->Data;

        /*case 2: BaseFwEntry's updatePath VS TargetFwEntry's UpdatePath*/
        if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) &&
           (TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION))
        {        
          if((BaseFwEntry->Metadata.UpdatePath.EmmcPartitionType == 
              TargetFwEntry->Metadata.UpdatePath.EmmcPartitionType) &&
             (CompareGuid((GUID*)&BaseFwEntry->Metadata.UpdatePath.PartitionTypeGUID,
                          (GUID*)&TargetFwEntry->Metadata.UpdatePath.PartitionTypeGUID)) &&
             (CompareMem(BaseFwEntry->Metadata.UpdatePath.PartitionName, 
                         TargetFwEntry->Metadata.UpdatePath.PartitionName,
                         PARTITION_NAME_MAX_CHAR16_SIZE*sizeof(CHAR16)) == 0))
          {
            *bListGood = FALSE;
            goto FwListPartitionDevicePathExclusiveCheckingExit;
          }
        }

        /*case 3: BaseFwEntry's updatePath VS TargetFwEntry's BackupPath*/
        if((BaseFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) &&
           (TargetFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION))
        {        
          if((BaseFwEntry->Metadata.UpdatePath.EmmcPartitionType == 
              TargetFwEntry->Metadata.BackupPath.EmmcPartitionType) &&
             (CompareGuid((GUID*)&BaseFwEntry->Metadata.UpdatePath.PartitionTypeGUID,
                          (GUID*)&TargetFwEntry->Metadata.BackupPath.PartitionTypeGUID)) &&
             (CompareMem(BaseFwEntry->Metadata.UpdatePath.PartitionName, 
                         TargetFwEntry->Metadata.BackupPath.PartitionName,
                         PARTITION_NAME_MAX_CHAR16_SIZE*sizeof(CHAR16)) == 0))
          {
            *bListGood = FALSE;
            goto FwListPartitionDevicePathExclusiveCheckingExit;
          }
        }

        /*case 4: BaseFwEntry's BackupPath VS TargetFwEntry's UpdatePath*/
        if((BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION) &&
           (TargetFwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION))
        {        
          if((BaseFwEntry->Metadata.BackupPath.EmmcPartitionType == 
              TargetFwEntry->Metadata.UpdatePath.EmmcPartitionType) &&
             (CompareGuid((GUID*)&BaseFwEntry->Metadata.BackupPath.PartitionTypeGUID,
                          (GUID*)&TargetFwEntry->Metadata.UpdatePath.PartitionTypeGUID)) &&
             (CompareMem(BaseFwEntry->Metadata.BackupPath.PartitionName, 
                         TargetFwEntry->Metadata.UpdatePath.PartitionName,
                         PARTITION_NAME_MAX_CHAR16_SIZE*sizeof(CHAR16)) == 0))
          {
            *bListGood = FALSE;
            goto FwListPartitionDevicePathExclusiveCheckingExit;
          }
        }
        /*case 5: BaseFwEntry's BackupPath VS TargetFwEntry's BackupPath*/
        if((BaseFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION) &&
           (TargetFwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_PARTITION))
        {        
          if((BaseFwEntry->Metadata.BackupPath.EmmcPartitionType == 
              TargetFwEntry->Metadata.BackupPath.EmmcPartitionType) &&
             (CompareGuid((GUID*)&BaseFwEntry->Metadata.BackupPath.PartitionTypeGUID,
                          (GUID*)&TargetFwEntry->Metadata.BackupPath.PartitionTypeGUID)) &&
             (CompareMem(BaseFwEntry->Metadata.BackupPath.PartitionName, 
                         TargetFwEntry->Metadata.BackupPath.PartitionName,
                         PARTITION_NAME_MAX_CHAR16_SIZE*sizeof(CHAR16)) == 0))
          {
            *bListGood = FALSE;
            goto FwListPartitionDevicePathExclusiveCheckingExit;
          }
        }
        TargetNode = TargetNode->Next; 
        TarGetNodeCount++;
      }        
    }
    else
    {
      break;
    }
    BaseNode = BaseNode->Next;
  }

  *bListGood = TRUE;

  return EFI_SUCCESS;

FwListPartitionDevicePathExclusiveCheckingExit:

  
  return Status;
}

/**
 *   Function to check if SSD is execlusive in a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListSsdExclusiveChecking(
    IN FW_LINKED_LIST_TYPE *List,
    OUT BOOLEAN            *bListGood
    )
{
  EFI_STATUS          Status        = EFI_SUCCESS;
  UINTN               NodeCount     = 0;
  UINTN               UpdateCount   = 0;
  FW_LIST_NODE_TYPE   *Node         = NULL;
  QPAYLOAD_FW_ENTRY   *FwEntry      = NULL;
  FW_LINKED_LIST_TYPE *FwEntryList  = NULL;
  UINTN               i             = 0;
  BOOLEAN             SsdPresent    = FALSE;


  if((List == NULL) || (bListGood == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  *bListGood = FALSE;

  Status = FwGetFwEntryList(&FwEntryList);
  HANDLE_ERROR_LABEL(Status,FwGetFwEntryList,FwListSsdExclusiveCheckingExit);
  
  Status = LinkedListGetHeadNode(FwEntryList,&Node);
  HANDLE_ERROR_LABEL(Status,LinkedListGetHeadNode,FwListSsdExclusiveCheckingExit);
  
  if (Node == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  Status= LinkedListGetNodeCount(FwEntryList,&NodeCount);
  HANDLE_ERROR_LABEL(Status,LinkedListGetNodeCount,FwListSsdExclusiveCheckingExit);
   
  for (i = 0; i < NodeCount; i++)
  {
    if (Node != NULL)
    {
      FwEntry = (QPAYLOAD_FW_ENTRY*)Node->Data;
      if(FwEntry != NULL)
      {
        if(FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_SSD)
        {
          SsdPresent = TRUE;
          
        }      
        UpdateCount++;  
      }
    }
    else
    {
      break;
    }
    if((UpdateCount > 1) && (SsdPresent == TRUE))
    {
      *bListGood = FALSE;
      goto FwListSsdExclusiveCheckingExit;
    }
    Node = Node->Next;
  }

  *bListGood = TRUE;
  return EFI_SUCCESS;
  
FwListSsdExclusiveCheckingExit:

  return Status;

}

/**
 *   Function to validate a payload v3 fw entry 
 *   linkedlist
 * 
 * @param List       - firmware entry linkedlist
 * @param bListGood  - result of validation
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwListValidation(
    IN FW_LINKED_LIST_TYPE       *List,
    OUT BOOLEAN                  *bListGood,
    OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
    OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if((List == NULL) || 
     (bListGood == NULL) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;
  *bListGood = FALSE;

  /*If SSD update is present, it should be exclusive*/
  Status = FwListSsdExclusiveChecking(List, bListGood);

  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_SSD_EXCLUSIVE_CHECKING;
    HANDLE_ERROR_LABEL(Status,FwListSsdExclusiveChecking,FwListValidationExit);
  }
    
  if(!*bListGood)
  {
    Status = EFI_UNSUPPORTED;
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_SSD_NOT_EXCLUSIVE;
    DEBUG_ERR_LABEL("\nSSD exclusive validation failed\n",FwListValidationExit);   
  }
  
  /*A partition Update/Backup DevicePath should be execlusive in 
    a payload v3 fw entry */

  Status = FwListPartitionDevicePathExclusiveChecking(List, bListGood);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_PARTITION_DEVPATH_EXCLUSIVE_CHECKING;
    HANDLE_ERROR_LABEL(Status,FwListPartitionDevicePathExclusiveChecking,FwListValidationExit);
  }
    
  if(!*bListGood)
  {
    Status = EFI_UNSUPPORTED;
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_PARTITION_DEVPATH_NOT_EXCLUSIVE;
    DEBUG_ERR_LABEL("\nPartition exclusive validation failed\n",FwListValidationExit);   
  }

  /*A QCOM DPP Item name should be execlusive in 
    a payload v3 fw entry */

  Status = FwListQcomDppItemNameExclusiveChecking(List, bListGood);
  if (EFI_ERROR(Status))
  {
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_QCOM_DPP_DEVPATH_EXCLUSIVE_CHECKING;
    HANDLE_ERROR_LABEL(Status,FwListQcomDppItemNameExclusiveChecking,FwListValidationExit);
  }
    
  if(!*bListGood)
  {
    Status = EFI_UNSUPPORTED;
    *QcResultFlag = QCFW_UPDATE_ERR_LIST_QCOM_DPP_DEVPATH_NOT_EXCLUSIVE;
    DEBUG_ERR_LABEL("\nQcom DppItem Name exclusive validation failed\n",FwListValidationExit);   
  }

  /*A OEM DPP Item name should be execlusive in 
     a payload v3 fw entry */
  
   Status = FwListOemDppItemNameExclusiveChecking(List, bListGood);
   if (EFI_ERROR(Status))
   {
     *QcResultFlag = QCFW_UPDATE_ERR_LIST_OEM_DPP_DEVPATH_EXCLUSIVE_CHECKING;
     HANDLE_ERROR_LABEL(Status,FwListOemDppItemNameExclusiveChecking,FwListValidationExit);
   }
     
   if(!*bListGood)
   {
     Status = EFI_UNSUPPORTED;
     *QcResultFlag = QCFW_UPDATE_ERR_LIST_OEM_DPP_DEVPATH_NOT_EXCLUSIVE;
     DEBUG_ERR_LABEL("\nOEM DppItem Name exclusive validation failed\n",FwListValidationExit);   
   }

  /*A FAT device path should be execlusive in 
     a payload v3 fw entry */
  
   Status = FwListFatDevicePathExclusiveChecking(List, bListGood);
   if (EFI_ERROR(Status))
   {
     *QcResultFlag = QCFW_UPDATE_ERR_LIST_FAT_DEVPATH_EXCLUSIVE_CHECKING;
     HANDLE_ERROR_LABEL(Status,FwListFatDevicePathExclusiveChecking,FwListValidationExit);
   }
     
   if(!*bListGood)
   {
     Status = EFI_UNSUPPORTED;
     *QcResultFlag = QCFW_UPDATE_ERR_LIST_FAT_DEVPATH_NOT_EXCLUSIVE;
     DEBUG_ERR_LABEL("\nFat device path exclusive validation failed\n",FwListValidationExit);   
   }


  return EFI_SUCCESS;

FwListValidationExit:
  
  return Status;
};

/**
 *. This function should parse V3 payload
 * @param mBuf      - Metadata buffer
 * @param mBufSize - Metadata buffer size
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPayloadV3Parser(
  IN VOID *mBuf,
  IN UINTN mBufSize,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

    )
{
  EFI_STATUS         Status           = EFI_SUCCESS;
  UINT32             TotalFwEntrySize = 0;
  FWENTRY_METADATA   *pMetadataEntry  = NULL;
  BOOLEAN            bEntryGood       = FALSE;
  BOOLEAN            bListGood        = FALSE;
  QPAYLOAD_FW_ENTRY  *pFwEntry        = NULL;
  UINTN              i                = 0;
  UINTN              NodeCount        = 0;


  if((mBuf == NULL) ||
     (mBufSize < sizeof(QPAYLOAD_METADATA_HEADER_V3)) ||
     (MsftAttemptStatus == NULL) ||
     (QcResultFlag == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_ERR_METADATA_INVALID_PARAM;

  /*Basic metadata size validation*/
  QPAYLOAD_METADATA_HEADER_V3 *mDataHeader = (QPAYLOAD_METADATA_HEADER_V3*)mBuf;
  TotalFwEntrySize = mDataHeader->EntryCount * sizeof(FWENTRY_METADATA) 
                     + sizeof(QPAYLOAD_METADATA_HEADER_V3);
  
  if(TotalFwEntrySize != mBufSize)
  {
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
    *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_IMG_FORMAT;
    return EFI_INVALID_PARAMETER;
  }

  /*Parse FwEntry metadata value*/
  pMetadataEntry = (FWENTRY_METADATA*)(mDataHeader+1);

  for (i = 0; i < mDataHeader->EntryCount; i++)
  {
    Status = gBS->AllocatePool(EfiBootServicesData, sizeof(QPAYLOAD_FW_ENTRY),(VOID**)&pFwEntry);
    if (EFI_ERROR(Status) || (pFwEntry == NULL))
    {
      *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INSUFF_RESOURCE;
      *QcResultFlag = QCFW_UPDATE_ERR_UEFI_API_ALLOCATE_POOL;
      HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,FwPayloadV3ParserExit);
    }
    else
    {
      ZeroMem(pFwEntry,sizeof(QPAYLOAD_FW_ENTRY));
      CopyMem((VOID*)&pFwEntry->Metadata,(VOID*)pMetadataEntry,sizeof(FWENTRY_METADATA));
      pMetadataEntry ++;

      /*Screen Unknown UpdateType/BackupType*/
      if((pFwEntry->Metadata.UpdateType < FWENTRY_UPDATE_TYPE_MAX) &&
         (pFwEntry->Metadata.BackupType < FWENTRY_BACKUP_TYPE_MAX) &&
         (pFwEntry->Metadata.Operation < FWENTRY_OPERATION_TYPE_MAX)) 
      {
        /*FwEntry metada validation*/
        Status = FwEntryValidation(&pFwEntry->Metadata,&bEntryGood,MsftAttemptStatus,QcResultFlag);
        HANDLE_ERROR_LABEL(Status,FwEntryValidation,FwPayloadV3ParserExit);

        if(!bEntryGood)
        {
          Status = EFI_UNSUPPORTED;
          DEBUG_ERR_LABEL("\nFwEntry Validation failed\n",FwPayloadV3ParserExit);
        }

        /*Update FwEntry handles*/
        Status = FwEntrySetPartitionHandles(pFwEntry);
        if (EFI_ERROR(Status))
        {
          *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
          *QcResultFlag = QCFW_UPDATE_ERR_PARSER_SET_PARTITION_HANDLES;
          HANDLE_ERROR_LABEL(Status,FwEntrySetPartitionHandles,FwPayloadV3ParserExit);
        }
    
        /*Cryptography*/
        Status = FwEntryCrypto(pFwEntry,MsftAttemptStatus,QcResultFlag);
        HANDLE_ERROR_LABEL(Status,FwEntryCrypto,FwPayloadV3ParserExit);
      
        /*Add to fw entry linked list*/
        Status = LinkedListAddNode(&FwEntryList,(VOID*)pFwEntry);
        if (EFI_ERROR(Status))
        {
          *QcResultFlag = QCFW_UPDATE_ERR_LIST_ADD_NODE;
          HANDLE_ERROR_LABEL(Status,LinkedListAddNode,FwPayloadV3ParserExit);
        }
        NodeCount++;
        DEBUG((EFI_D_WARN,"\n    %d FwEntry found...\n",NodeCount));
        FwPrintFwEntry(pFwEntry,EFI_D_WARN);
        /*Set pFwEntry to NULL after adding the node to list so that it avoids 
          double freeing memory at FwPayloadV3ParserExit*/
        pFwEntry = NULL;

      }
      else
      { 
        if(bDoubleUpdateNeeded)
        {
          if(pFwEntry != NULL)
          {
            gBS->FreePool(pFwEntry);
            pFwEntry = NULL;
          }
          continue;
        }
        else
        {
          Status = EFI_UNSUPPORTED;
          *QcResultFlag = QCFW_UPDATE_ERR_FWENTRY_UNKNOWN_UPDATE_BACKUP_TYPE;
          DEBUG_ERR_LABEL("\nUnknown update/backup type\n",FwPayloadV3ParserExit);
        }
      }
    }
  }
  if(FwEntryList.Count == 0)
  {
    DEBUG((EFI_D_WARN,"\n    No FwEntry found...\n"));
    return EFI_SUCCESS;
  }

  DEBUG((EFI_D_WARN,"\n    Validating FwEntry list...\n"));
  Status = FwListValidation(&FwEntryList, &bListGood,MsftAttemptStatus,QcResultFlag);
  HANDLE_ERROR_LABEL(Status,FwListValidation,FwPayloadV3ParserExit);

  if(!bListGood)
  {
    Status = EFI_UNSUPPORTED;
    DEBUG_ERR_LABEL("\n    FwEntry list validation failed...\n",FwPayloadV3ParserExit);
  }

  DEBUG((EFI_D_WARN,"\n    FwEntry list validated. Totally %d FwEntries found\n",FwEntryList.Count));

  return EFI_SUCCESS;

FwPayloadV3ParserExit:

  DEBUG((EFI_D_ERROR,"Error: Current FwEntry info \n")); 
  FwPrintFwEntry(pFwEntry,EFI_D_ERROR);

  DEBUG((EFI_D_ERROR,"Error: List info \n"));
  FwPrintFwList(&FwEntryList, EFI_D_ERROR);
  
  if(pFwEntry != NULL)
  {
    gBS->FreePool(pFwEntry);
    pFwEntry = NULL;
  }

  /*Do not check the Status of LinkedListDeleteAllNodes since the overall 
  status should be the previous error status*/

  LinkedListDeleteAllNodes(&FwEntryList);
  return Status;
}

/**
 * Payload parser main function
 *
 * @param  MsftAttemptStatus   -Microsoft last attempt status
 * @param  QcResultFlag        -QCFirmwareUpdate result
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwPayloadParserMain(
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
    )
{
  EFI_STATUS Status               = EFI_SUCCESS;
  UINT32     Version              = 0;
  VOID       *mBuf                = NULL;
  UINTN      mBufSize             = 0;
  UINT32     BreakingChangeNumber = 0;

  *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_UNSUCCESSFUL;
  *QcResultFlag = QCFW_UPDATE_INIT;

  DEBUG((EFI_D_WARN,"\n  Parsing payload... \n"));
  /* Find payload format version from MetaData */
  Status = FwFindMetadata(&Version,&mBuf,&mBufSize,MsftAttemptStatus,QcResultFlag); 
  HANDLE_ERROR_LABEL(Status,FwPayloadParserMain,FwPayloadParserMainExit);

  /* Display payload info */
    Status = FwPrintPayloadHeader(&PayLoadHeader, EFI_D_WARN);
    HANDLE_ERROR_LABEL(Status,FwPrintPayloadHeader,FwPayloadParserMainExit);

  //Read the BreakingChangeNumber from disk
  Status = GetBreakingChangeNumber(&BreakingChangeNumber);
  
  if(Status != EFI_SUCCESS)
  {
     BreakingChangeNumber = 0;
  }
  DEBUG((EFI_D_WARN,"    Current breaking change number is %d\n",BreakingChangeNumber));
  DEBUG((EFI_D_WARN,"    Payload breaking change number is %d\n",PayLoadHeader.BreakingChangeNumber));

  bDoubleUpdateNeeded = (PayLoadHeader.BreakingChangeNumber > BreakingChangeNumber)? TRUE:FALSE;
  
  if(bDoubleUpdateNeeded)
  {
    DEBUG((EFI_D_WARN,"    Double update required\n\n"));
  }
  else
  {
    DEBUG((EFI_D_WARN,"    Double update not required\n\n"));
  }
 
  if(Version < 3)
  {
    DEBUG((EFI_D_WARN,"    V1/V2 payload is not supported anymore\n"));
    *MsftAttemptStatus = FW_LAST_ATMPT_STATUS_INVALID_IMG_FMT;
    *QcResultFlag = QCFW_UPDATE_ERR_PAYLOAD_IMG_FORMAT;
    Status = EFI_UNSUPPORTED;
    
    HANDLE_ERROR_LABEL(Status,FwPayloadV1V2Parser,FwPayloadParserMainExit);
  }
  else
  {
    Status = FwPayloadV3Parser(mBuf,mBufSize,MsftAttemptStatus,QcResultFlag);
    HANDLE_ERROR_LABEL(Status,FwPayloadV3Parser,FwPayloadParserMainExit);
  }
  
FwPayloadParserMainExit:

  if( mBuf != NULL)
  {
    gBS->FreePool(mBuf);
    mBuf = NULL;
  }
  
  return Status;
}


