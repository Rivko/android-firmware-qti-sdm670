/** @file FwEntryIO_helpers.c
   
  Payload fw entry read write functions for data driven firmware update solution.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when          who      what, where, why
 ----------    ---      --------------------------------------------------
 2015/04/15    mic      Added 64bit firmware update support
 2012/10/11    rs       Added DeleteFile and DeletePartition functions
 2012/10/08    mic      Moved common macro to FwCommonLib
 2012/09/28    rs       Implementation of the helper functions to read/write
 2012/09/21    mic      Initial version

=============================================================================*/

#include <Uefi.h>
#include <Include/Library/FwCommonLib.h>
#include <Library/FwCommonLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Include/Library/MemoryAllocationLib.h>
#include "FwPayloadEntry.h"
#include "FwEntryIO_helpers.h"

#define FIRST_BLOCK 0
#define DEFAULT_COPY_LOOP_CNT 2

extern BOOLEAN bDoubleUpdateNeeded;
const FWENTRY_READ FwEntrySourceReadFunctions[FWENTRY_SOURCE_TYPE_MAX]={
  FwUpdateReadEntryFromFV
};
  
const FWENTRY_DELETE FwEntryDestDeleteFunctions[FWENTRY_UPDATE_TYPE_MAX]={
  FwUpdateDeletePartitionEntry,
  FwUpdateDeleteFatEntry,
  FwUpdateDeleteQcomDppEntry,
  FwUpdateDeleteOemDppEntry,
  FwUpdateDeleteQcomDppEntry,
  FwUpdateDeletePartitionEntry
   };

const FWENTRY_READ FwEntryDestReadFunctions[FWENTRY_UPDATE_TYPE_MAX]={
  FwUpdateReadEntryFromPartition,
  FwUpdateReadEntryFromFat,
  FwUpdateReadEntryFromDppQcom,
  FwUpdateReadEntryFromDppOem,
  FwUpdateReadEntryFromDppQcom,
  FwUpdateReadEntryFromPartition
};
  
const FWENTRY_WRITE FwEntryDestWriteFunctions[FWENTRY_UPDATE_TYPE_MAX]={
  FwUpdateWriteEntryToPartition,
  FwUpdateWriteEntryToFat,
  FwUpdateWriteEntryToDppQcom,
  FwUpdateWriteEntryToDppOem,
  FwUpdateWriteEntryToDppQcom,
  FwUpdateWriteEntryToPartition
};
  
const FWENTRY_DELETE FwEntryBackupDeleteFunctions[FWENTRY_BACKUP_TYPE_MAX]={
  FwUpdateDeletePartitionEntry,
  FwUpdateDeleteFatEntry
};

const FWENTRY_READ FwEntryBackupReadFunctions[FWENTRY_BACKUP_TYPE_MAX]={
  FwUpdateReadEntryFromPartition,
  FwUpdateReadEntryFromFat
};
  
const FWENTRY_WRITE FwEntryBackupWriteFunctions[FWENTRY_BACKUP_TYPE_MAX]={
  FwUpdateWriteEntryToPartition,
  FwUpdateWriteEntryToFat
};

/**
 * This function delete FwEntry in the backup path if the backup type is FAT, 
 * if the backup type is PARTITION then nothing happens
 *
 * @param FwEntry -  Firmware entry
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwEntryDeleteBackup(
    IN QPAYLOAD_FW_ENTRY *FwEntry
    )
{
  EFI_STATUS                        Status                 = EFI_SUCCESS;
  FWENTRY_BACKUP_TYPE               BackupType             = FWENTRY_BACKUP_TYPE_MAX;       
  VOID                             *Buffer                 = NULL;
  VOID                             *BufferDest             = NULL;
  UINTN                             BufSize                = 0;
  UINTN                             BufSizeDest            = 0;
  FWENTRY_READ                      FwReadBackupFunction   = NULL; 
  FWENTRY_DELETE                    FwDeleteBackupFunction = NULL;
  UINTN                             LoopCount              = 0;

  if (FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  /*Unknown update type should be ignored if double update is needed*/
  if(FwEntry->Metadata.BackupType >= FWENTRY_BACKUP_TYPE_MAX )
  {
    if(bDoubleUpdateNeeded)
    {
      return EFI_SUCCESS;
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }
  }

  BackupType = FwEntry->Metadata.BackupType;

  FwReadBackupFunction    = FwEntryBackupReadFunctions[BackupType];
  FwDeleteBackupFunction  = FwEntryBackupDeleteFunctions[BackupType];
 
  if ((FwReadBackupFunction == NULL) || 
      (FwDeleteBackupFunction == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  if(FwEntry->Metadata.BackupType == FWENTRY_BACKUP_TYPE_FAT_FILE)
  {
    Status = FwReadBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,&Buffer,&BufSize); 
    if (Status == EFI_NOT_FOUND)
    { 
      /*If the backup file doesn't exist, means the FwEntry is newly 
        introduced. There is no need to delete a non-exist file.  */
      Status = EFI_SUCCESS;
      goto FwEntryDeleteBackupExit;  
    }

    /*We need to write to the backup partition andn verify it - twice*/
    for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
    {
      Status = FwDeleteBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry);
      HANDLE_ERROR_LABEL(Status, FwDeleteBackupFunction, FwEntryDeleteBackupExit);
        
      Status = FwReadBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,&BufferDest,&BufSizeDest);
      if(Status == EFI_NOT_FOUND)
      {
        Status = EFI_SUCCESS;
        goto FwEntryDeleteBackupExit;
      }
                        
      Status = EFI_VOLUME_CORRUPTED;
      /*Free-up the BufferDest as everytime in this loop, there will be a new 
        allocation in the Read()*/
      if( BufferDest != NULL)
      {
        gBS->FreePool(BufferDest);
        BufferDest = NULL;
      }
    }
  }
   
FwEntryDeleteBackupExit:

  if (Buffer != NULL)
  {
    gBS->FreePool(Buffer);
    Buffer = NULL;
  }
  if (BufferDest != NULL)
  {
    gBS->FreePool(BufferDest);
    BufferDest = NULL;
  }

  return Status;
}

/**
 * This function backs up fw entry from destination path to 
 * backup path. If target entry doesn't exist in destination 
 * path, backup operation will be skipped 
 *
 * @param FwEntry -  Firmware entry 
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwEntryBackup(
    IN QPAYLOAD_FW_ENTRY *FwEntry
    )
{
  EFI_STATUS                        Status                = EFI_SUCCESS;
  FWENTRY_UPDATE_TYPE               UpdateType            = FWENTRY_UPDATE_TYPE_MAX;
  FWENTRY_BACKUP_TYPE               BackupType            = FWENTRY_BACKUP_TYPE_MAX;
  VOID                              *Buffer               = NULL;
  VOID                              *BufferDest           = NULL;
  UINTN                             BufSize               = 0;
  UINTN                             BufSizeDest           = 0;
  FWENTRY_READ                      FwReadDestFunction    = NULL; 
  FWENTRY_READ                      FwReadBackupFunction  = NULL; 
  FWENTRY_WRITE                     FwWriteBackupFunction = NULL;
  UINTN                             LoopCount             = 0;

  if (FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  /*Unknown update type should be ignored if double update is needed*/
  if ((FwEntry->Metadata.UpdateType >= FWENTRY_UPDATE_TYPE_MAX ) ||
      (FwEntry->Metadata.BackupType >= FWENTRY_BACKUP_TYPE_MAX ))
  {
    if(bDoubleUpdateNeeded)
    {
      return EFI_SUCCESS;
    }
    else
    {
      Status = EFI_INVALID_PARAMETER;
      DEBUG_ERR_LABEL("Unknown UpdateType/BackupType\n",FwEntryBackupExit);
    }
  }

  UpdateType = FwEntry->Metadata.UpdateType;
  BackupType = FwEntry->Metadata.BackupType;

  FwReadDestFunction     = FwEntryDestReadFunctions[UpdateType];
  FwWriteBackupFunction  = FwEntryBackupWriteFunctions[BackupType];
  FwReadBackupFunction   = FwEntryBackupReadFunctions[BackupType];

  if ((FwReadDestFunction == NULL) || 
      (FwWriteBackupFunction == NULL) || 
      (FwReadBackupFunction == NULL))
  {
    Status = EFI_INVALID_PARAMETER;
    DEBUG_ERR_LABEL("Bad FwReadDestFunction/FwWriteBackupFunction/FwReadBackupFunction\n",
                    FwEntryBackupExit);
  }

  Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,&Buffer,&BufSize);
  //If the destination doesn't exist we need to skip the backup for this FwEntry.
  //We are assuming a partition will always exist, but a DPP entry or File may not.
  //A success should be returned in that case.
  if (Status == EFI_NOT_FOUND)
  {
    return EFI_SUCCESS;
  }
  else
  {
    HANDLE_ERROR_LABEL(Status, FwReadDestFunction, FwEntryBackupExit);
  }

  //We need to write to the Backup partition and verify it - twice
  for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
  {
    Status = FwWriteBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,Buffer,BufSize);
    HANDLE_ERROR_LABEL(Status, FwWriteBackupFunction, FwEntryBackupExit);

    Status = FwReadBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,&BufferDest,&BufSizeDest);
    HANDLE_ERROR_LABEL(Status, FwReadBackupFunction, FwEntryBackupExit);

    //Check if what we read back from the backup partition is same as what we were writing
    if((BufSize == BufSizeDest) && (CompareMem (BufferDest,Buffer,BufSize) == 0))
    {
      Status = EFI_SUCCESS;
      goto FwEntryBackupExit;
    }
    else
    {
      Status = EFI_VOLUME_CORRUPTED;
      //Free-up the BufferDest as everytime in this loop, there will be a new allocation in the Read()
      if(BufferDest != NULL)
      {
        gBS->FreePool(BufferDest);
        BufferDest = NULL;
      }  
    }
  }

FwEntryBackupExit:

  if (Buffer != NULL)
  {
    gBS->FreePool(Buffer);
    Buffer = NULL;
  }

  if (BufferDest != NULL)
  {
    gBS->FreePool(BufferDest);
    BufferDest = NULL;
  }
  return Status;
}


/**
 * This function updates the FwEntry from payload path to 
 * destination path. 
 *
 * @param FwEntry -  Firmware entry
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwEntryUpdate(
    IN QPAYLOAD_FW_ENTRY *FwEntry
    ) 
{
  EFI_STATUS                        Status               = EFI_SUCCESS;
  FWENTRY_UPDATE_TYPE               UpdateType           = FWENTRY_UPDATE_TYPE_MAX;
  FWENTRY_SOURCE_TYPE               SourceType           = FWENTRY_SOURCE_TYPE_MAX;
  VOID                             *Buffer               = NULL;
  VOID                             *BufferDest           = NULL;
  UINTN                             BufSize              = 0;
  UINTN                             BufSizeDest          = 0;
  FWENTRY_READ                      FwReadSourceFunction = NULL; 
  FWENTRY_READ                      FwReadDestFunction   = NULL; 
  FWENTRY_WRITE                     FwWriteDestFunction  = NULL;
  FWENTRY_DELETE                    FwDeleteDestFunction = NULL;
  UINTN                             LoopCount            = 0;

  if(FwEntry == NULL)  
  {
    return EFI_INVALID_PARAMETER;
  }

  /*Unknown update type should be ignored if double update is needed*/
  if ((FwEntry->Metadata.UpdateType >= FWENTRY_UPDATE_TYPE_MAX ) ||
      (FwEntry->Metadata.BackupType >= FWENTRY_BACKUP_TYPE_MAX ))
  {
    if(bDoubleUpdateNeeded)
    {
      return EFI_SUCCESS;
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }
  }

  UpdateType = FwEntry->Metadata.UpdateType;
  SourceType = FWENTRY_SOURCE_TYPE_FV;
 
  if ((UpdateType >= FWENTRY_UPDATE_TYPE_MAX) || (SourceType >= FWENTRY_SOURCE_TYPE_MAX) )
  {
    return EFI_INVALID_PARAMETER;
  }
  
  switch(FwEntry->Metadata.Operation)
  {
    case FWENTRY_OPERATION_TYPE_DELETE:

      switch(UpdateType)
      {
          case FWENTRY_UPDATE_TYPE_PARTITION:
          case FWENTRY_UPDATE_TYPE_DPP_QCOM:
          case FWENTRY_UPDATE_TYPE_DPP_OEM:
          case FWENTRY_UPDATE_TYPE_OPM_PRIV_KEY:
          case FWENTRY_UPDATE_TYPE_SSD:
            DEBUG((EFI_D_ERROR,"Unsupported update type\n"));
            return EFI_UNSUPPORTED;

          case FWENTRY_UPDATE_TYPE_FAT_FILE:

            FwDeleteDestFunction  = FwEntryDestDeleteFunctions[UpdateType];
            FwReadDestFunction   = FwEntryDestReadFunctions[UpdateType];

            if ((FwDeleteDestFunction == NULL) || 
                (FwReadDestFunction == NULL))
            {
              return EFI_INVALID_PARAMETER;
            }

            for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
            {
              Status = FwDeleteDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry);  
              HANDLE_ERROR_LABEL(Status, FwDeleteDestFunction, FwEntryUpdateExit);
              
              Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,
                                          FwEntry,
                                          &BufferDest,
                                          &BufSizeDest);
              if(Status == EFI_NOT_FOUND)
              {
                Status = EFI_SUCCESS;
                goto FwEntryUpdateExit;
              }
              
              Status = EFI_VOLUME_CORRUPTED;
              /*Free-up the BufferDest as everytime in this loop, there will be a new 
                allocation in the Read()*/
              if( BufferDest != NULL)
              {
                gBS->FreePool(BufferDest);
                BufferDest = NULL;
              }
            }
          
            break;

          default:

            return EFI_INVALID_PARAMETER;

      }

    case FWENTRY_OPERATION_TYPE_UPDATE:

      FwReadSourceFunction = FwEntrySourceReadFunctions[SourceType];
      FwWriteDestFunction  = FwEntryDestWriteFunctions[UpdateType];
      FwReadDestFunction   = FwEntryDestReadFunctions[UpdateType];

      if ((FwReadSourceFunction == NULL) || 
          (FwWriteDestFunction == NULL) || 
          (FwReadDestFunction == NULL))
      {
        return EFI_INVALID_PARAMETER;
      }

      //Check if the payload was encrypted. In that case, no need to read from FV.
      //Just read from the DecryptedBuffer.

      if (FwEntry->IsEncrypted)
      {
        Buffer = FwEntry->DecryptedFwEntryBuffer;
        BufSize = FwEntry->DecryptedFwEntrySize;
        if ((Buffer == NULL) || (BufSize == 0))
        {
          return EFI_INVALID_PARAMETER;
        }
      }
      else
      {
        Status = FwReadSourceFunction(FWENTRY_OPERATION_PATH_TYPE_SOURCE,FwEntry,&Buffer,&BufSize);
        HANDLE_ERROR_LABEL(Status, FwReadSourceFunction, FwEntryUpdateExit);
      }

      //We need to write to the Dest partition and verify it - twice
      for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
      {
        Status = FwWriteDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,Buffer,BufSize);
        HANDLE_ERROR_LABEL(Status, FwWriteDestFunction, FwEntryUpdateExit);

        Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,&BufferDest,&BufSizeDest);
        HANDLE_ERROR_LABEL(Status, FwEntryUpdate, FwEntryUpdateExit);

        //Check if what we read back from the dest partition is same as what we were writing

        if((FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) ||
           (FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_SSD))

        {
          if ((CompareMem (BufferDest,Buffer,BufSize) == 0))
          {
            Status = EFI_SUCCESS;
            goto FwEntryUpdateExit;
          }
        }
        else
        {
          if ((CompareMem (BufferDest,Buffer,BufSize) == 0) && 
              (BufSize == BufSizeDest) )
          {
            Status = EFI_SUCCESS;
            goto FwEntryUpdateExit;
          }
        }

        Status = EFI_VOLUME_CORRUPTED;
        //Free-up the BufferDest as everytime in this loop, there will be a new allocation in the Read()
        if( BufferDest != NULL)
        {
          gBS->FreePool(BufferDest);
          BufferDest = NULL;
        }
      }

      break;

    case FWENTRY_OPERATION_TYPE_IGNORE:
      return EFI_SUCCESS;

    default:
      DEBUG((EFI_D_WARN,"Unsupported operation type\n"));
      return EFI_UNSUPPORTED;
  }

 
FwEntryUpdateExit:

  if (FwEntry->IsEncrypted == FALSE)
  {
    if (Buffer != NULL)
    {
      gBS->FreePool(Buffer);
      Buffer = NULL;
    }
  }
  if (BufferDest != NULL)
  {
    gBS->FreePool(BufferDest);
    BufferDest = NULL;
  }
  
  return Status;
}


/**
 * This function rolls back the FwEntry from the backup path to 
 * destination path. 
 *
 * @param FwEntry -  Firmware entry
 * 
 * @return EFI_STATUS 
 */

EFI_STATUS
FwEntryRollback(
    IN QPAYLOAD_FW_ENTRY *FwEntry
    )
{
  EFI_STATUS                        Status               = EFI_SUCCESS;
  FWENTRY_UPDATE_TYPE               UpdateType           = FWENTRY_UPDATE_TYPE_MAX;
  FWENTRY_BACKUP_TYPE               BackupType           = FWENTRY_BACKUP_TYPE_MAX;       
  VOID                             *Buffer               = NULL;
  VOID                             *BufferDest           = NULL;
  UINTN                             BufSize              = 0;
  UINTN                             BufSizeDest          = 0;
  FWENTRY_READ                      FwReadBackupFunction = NULL; 
  FWENTRY_READ                      FwReadDestFunction   = NULL; 
  FWENTRY_WRITE                     FwWriteDestFunction  = NULL;
  FWENTRY_DELETE                    FwDeleteDestFunction = NULL;

  UINTN                             LoopCount            = 0;

  if (FwEntry == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }
  /*Unknown update type should be ignored if double update is needed*/
  if ((FwEntry->Metadata.UpdateType >= FWENTRY_UPDATE_TYPE_MAX ) ||
      (FwEntry->Metadata.BackupType >= FWENTRY_BACKUP_TYPE_MAX ))
  {
    if(bDoubleUpdateNeeded)
    {
      return EFI_SUCCESS;
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }
  }

  UpdateType = FwEntry->Metadata.UpdateType;
  BackupType = FwEntry->Metadata.BackupType;

  if ((UpdateType > FWENTRY_UPDATE_TYPE_MAX) || (BackupType > FWENTRY_BACKUP_TYPE_MAX))
  {
    return EFI_INVALID_PARAMETER;
  }

  FwReadBackupFunction = FwEntryBackupReadFunctions[BackupType];
  FwWriteDestFunction  = FwEntryDestWriteFunctions[UpdateType];
  FwReadDestFunction   = FwEntryDestReadFunctions[UpdateType];
  FwDeleteDestFunction = FwEntryDestDeleteFunctions[UpdateType];

  if ((FwReadBackupFunction == NULL) || 
      (FwWriteDestFunction == NULL) || 
      (FwReadDestFunction == NULL) ||
      (FwDeleteDestFunction == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  switch(FwEntry->Metadata.Operation)
  {
    case FWENTRY_OPERATION_TYPE_DELETE:

      Status = FwReadBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,&Buffer,&BufSize);
      
      if (Status == EFI_NOT_FOUND)
      { 
        /*this case means theFwEntry doesn't exist in the Dest device path from 
          very beginning, that's why it is not found in the backup device 
          path, so just return EFI_SUCCESS in this case */
        if (Buffer != NULL)
        {
          gBS->FreePool(Buffer);
          Buffer = NULL;
        }
        
        Status = EFI_SUCCESS;
        goto FwEntryRollbackExit;
      }
      else
      {
        HANDLE_ERROR_LABEL(Status, FwReadBackupFunction, FwEntryRollbackExit);
      }
      
      /*We need to write to the dest partition andn verify it - twice*/
      for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
      {
        Status = FwWriteDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,Buffer,BufSize);
        HANDLE_ERROR_LABEL(Status, FwEntryRollback, FwEntryRollbackExit);
      
        Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,&BufferDest,&BufSizeDest);
        HANDLE_ERROR_LABEL(Status, FwEntryRollback, FwEntryRollbackExit);
      
        /*Check if what we read back from the dest partition is same as what we were 
          writing*/
        if((FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) ||
           (FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_SSD))
        {
          if ((CompareMem (BufferDest,Buffer,BufSize) == 0))
          {
            Status = EFI_SUCCESS;
            goto FwEntryRollbackExit;
          }
        }
        else
        {
          if ((CompareMem (BufferDest,Buffer,BufSize) == 0) && 
              (BufSize == BufSizeDest) )
          {
            Status = EFI_SUCCESS;
            goto FwEntryRollbackExit;
          }
        }

        Status = EFI_VOLUME_CORRUPTED;
        /*Free-up the BufferDest as everytime in this loop, there will be a 
          new allocation in the Read()*/
        if( BufferDest != NULL)
        {
          gBS->FreePool(BufferDest);
          BufferDest = NULL;
        }
      }

      break;

    case FWENTRY_OPERATION_TYPE_UPDATE:
      
      Status = FwReadBackupFunction(FWENTRY_OPERATION_PATH_TYPE_BACKUP,FwEntry,&Buffer,&BufSize);
      
      if (Status == EFI_NOT_FOUND)
      { 
        /*this case means the current FwEntry is a new added file, so couldn't find 
         the file in backup device path*/
        if (Buffer != NULL)
        {
          gBS->FreePool(Buffer);
          Buffer = NULL;
        }

        /*We need to delete the dest file and verify it - twice*/
        for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
        {
          Status = FwDeleteDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry);
          HANDLE_ERROR_LABEL(Status, FwDeleteDestFunction, FwEntryRollbackExit);
        
          Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,&BufferDest,&BufSizeDest);
          if(Status == EFI_NOT_FOUND)
          {
            Status = EFI_SUCCESS;
            goto FwEntryRollbackExit;
          }
                        
          Status = EFI_VOLUME_CORRUPTED;
          /*Free-up the BufferDest as everytime in this loop, there will be a new 
            allocation in the Read()*/
          if( BufferDest != NULL)
          {
            gBS->FreePool(BufferDest);
            BufferDest = NULL;
          }
        } 
      }
      else
      {
        HANDLE_ERROR_LABEL(Status, FwReadBackupFunction, FwEntryRollbackExit);
      
        /*We need to write to the backup partition andn verify it - twice*/
        for (LoopCount = 0; LoopCount < DEFAULT_COPY_LOOP_CNT; LoopCount++)
        {
          Status = FwWriteDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,Buffer,BufSize);
          HANDLE_ERROR_LABEL(Status, FwEntryRollback, FwEntryRollbackExit);
        
          Status = FwReadDestFunction(FWENTRY_OPERATION_PATH_TYPE_DEST,FwEntry,&BufferDest,&BufSizeDest);
          HANDLE_ERROR_LABEL(Status, FwEntryRollback, FwEntryRollbackExit);
        
          /*Check if what we read back from the dest partition is same as what we were 
            writing*/
          if((FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_PARTITION) ||
             (FwEntry->Metadata.UpdateType == FWENTRY_UPDATE_TYPE_SSD))
          {
            if ((CompareMem (BufferDest,Buffer,BufSize) == 0))
            {
              Status = EFI_SUCCESS;
              goto FwEntryRollbackExit;
            }
          }
          else
          {
            if ((CompareMem (BufferDest,Buffer,BufSize) == 0) && 
                (BufSize == BufSizeDest) )
            {
              Status = EFI_SUCCESS;
              goto FwEntryRollbackExit;
            }
          }

          Status = EFI_VOLUME_CORRUPTED;
          /*Free-up the BufferDest as everytime in this loop, there will be a 
            new allocation in the Read()*/
          if( BufferDest != NULL)
          {
            gBS->FreePool(BufferDest);
            BufferDest = NULL;
          }
        }
      }
      break;

    case FWENTRY_OPERATION_TYPE_IGNORE:
      
      Status = EFI_SUCCESS;
      goto FwEntryRollbackExit;

    default:
      DEBUG((EFI_D_WARN,"Unsupported operation type\n"));
      return EFI_UNSUPPORTED;
  }

FwEntryRollbackExit:

    if (Buffer != NULL)
    {
      gBS->FreePool(Buffer);
      Buffer = NULL;
    }
    if (BufferDest != NULL)
    {
      gBS->FreePool(BufferDest);
      BufferDest = NULL;
    }

  return Status;
}
  

/**
 * This function should delete entry by given pathType 
 *
 * @param pathType  - source, dest, or backup        
 * @param FwEntry   - Firmware entry        

 * @return EFI_STATUS 
 */
EFI_STATUS
FwDeleteEntry(
    IN FWENTRY_OPERATION_PATH_TYPE pathType,
    IN QPAYLOAD_FW_ENTRY           *FwEntry
  )
{
  EFI_STATUS    Status            = EFI_SUCCESS;
  FWENTRY_DELETE FwDeleteFunction = NULL;

  if (FwEntry == NULL)  
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FwGetEntryDeleteFunction(pathType,FwEntry,&FwDeleteFunction);
  HANDLE_ERROR_LABEL(Status,FwGetEntryDeleteFunction,FwDeleteEntryExit);

  Status = FwDeleteFunction(pathType,FwEntry);
  HANDLE_ERROR_LABEL(Status,FwDeleteFunction,FwDeleteEntryExit);

 FwDeleteEntryExit:

  return Status;
}


/**
 * This function should write entry buffer to target path 
 *
 * @param pathType  - source, dest, or backup        
 * @param FwEntry   - Firmware entry        
 * @param Buffer    - buffer
 * @param BufSize   - buffer size

 * @return EFI_STATUS 
 */
EFI_STATUS
FwWriteEntry(
    IN FWENTRY_OPERATION_PATH_TYPE pathType,
    IN QPAYLOAD_FW_ENTRY           *FwEntry,
    IN VOID                        *Buffer,
    IN UINTN                       BufSize
  )
{

  EFI_STATUS     Status          = EFI_SUCCESS;
  FWENTRY_WRITE  FwWriteFunction = NULL;

  if ((FwEntry == NULL) || (Buffer == NULL))  
  {
    return EFI_INVALID_PARAMETER;
  }
  
  if (BufSize == 0)
  {
    return EFI_SUCCESS;
  }

  Status = FwGetEntryWriteFunction(pathType,FwEntry,&FwWriteFunction);
  HANDLE_ERROR_LABEL(Status,FwGetEntryWriteFunction,FwWriteEntryExit);

  Status = FwWriteFunction(pathType,FwEntry,Buffer,BufSize);
  HANDLE_ERROR_LABEL(Status,FwWriteFunction,FwWriteEntryExit);
 
FwWriteEntryExit:

  return Status;
}
/**
 * This function should Read entry to a buffer from target path
 *
 * @param pathType   -source, dest, or backup        
 * @param FwEntry    - Firmware entry        
 * @param Buffer     - buffer
 * @param BufSize    - buffer size

 * @return EFI_STATUS 
 */
EFI_STATUS
FwReadEntry(
    IN FWENTRY_OPERATION_PATH_TYPE pathType,
    IN QPAYLOAD_FW_ENTRY           *FwEntry,
    OUT VOID                        **Buffer,
    OUT UINTN                      *BufSize
  )
{
  EFI_STATUS   Status         = EFI_SUCCESS;
  FWENTRY_READ FwReadFunction = NULL;

  if ((FwEntry == NULL) || 
      (Buffer == NULL) || 
      (BufSize == NULL))  
  {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = FwGetEntryReadFunction(pathType,FwEntry,&FwReadFunction);
  HANDLE_ERROR_LABEL(Status,FwGetEntryReadFunction,FwReadEntryExit);

  Status = FwReadFunction(pathType,FwEntry,Buffer,BufSize);
  HANDLE_ERROR_LABEL(Status,FwReadFunction,FwReadEntryExit);
 
 FwReadEntryExit:

  return Status;
}

/**
 * This function should get Delete function for pathType
 *
 * @param FwEntry    - Firmware entry     
 * @param Fn         - Delete function             
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwGetEntryDeleteFunction(
    IN FWENTRY_OPERATION_PATH_TYPE  PathType,
    IN QPAYLOAD_FW_ENTRY            *FwEntry,
    OUT FWENTRY_DELETE              *Fn
    )
{
  if ((FwEntry == NULL) || (Fn == NULL)) 
  { 
    return EFI_INVALID_PARAMETER;
  }

  switch(PathType)
  {
  case FWENTRY_OPERATION_PATH_TYPE_SOURCE:

    return EFI_UNSUPPORTED;

  case FWENTRY_OPERATION_PATH_TYPE_BACKUP:

    if (FwEntry->Metadata.BackupType < FWENTRY_BACKUP_TYPE_MAX)
    {
      *Fn = FwEntryBackupDeleteFunctions[FwEntry->Metadata.BackupType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }
    break;

  case FWENTRY_OPERATION_PATH_TYPE_DEST:

    if (FwEntry->Metadata.UpdateType < FWENTRY_UPDATE_TYPE_MAX)
    {
      *Fn = FwEntryDestDeleteFunctions[FwEntry->Metadata.UpdateType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }

    break;

  default:

    return EFI_INVALID_PARAMETER;

  }

  //Need to check if got a valid function from the array
  if (Fn == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
 * This function should get Read function for pathType
 *
 * @param FwEntry    - Firmware entry     
 * @param Fn         - Read function for firmware backup operation              
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwGetEntryReadFunction(
  IN FWENTRY_OPERATION_PATH_TYPE  PathType,
  IN QPAYLOAD_FW_ENTRY            *FwEntry,
  OUT FWENTRY_READ                *Fn
    )
{
  if ((FwEntry == NULL) || (Fn == NULL))  
  {
    return EFI_INVALID_PARAMETER;
  }

  switch (PathType)
  {
  case FWENTRY_OPERATION_PATH_TYPE_SOURCE:

    //Currently only source read option is from FV.
    *Fn = FwEntrySourceReadFunctions[FWENTRY_SOURCE_TYPE_FV];
    break;

  case FWENTRY_OPERATION_PATH_TYPE_BACKUP:

    if (FwEntry->Metadata.BackupType < FWENTRY_BACKUP_TYPE_MAX)
    {
      *Fn = FwEntryBackupReadFunctions[FwEntry->Metadata.BackupType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }

    break;

  case FWENTRY_OPERATION_PATH_TYPE_DEST:

    if (FwEntry->Metadata.UpdateType < FWENTRY_UPDATE_TYPE_MAX)
    {
      *Fn = FwEntryDestReadFunctions[FwEntry->Metadata.UpdateType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }

    break;

  default:

    return EFI_INVALID_PARAMETER;

  }
 
  //Need to check if got a valid function from the array
  if (Fn == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
}

/**
 * This function should get write function for path type
 *
 * @param FwEntry    - Firmware entry       
 * @param Fn         - Write function for firmware update operation      
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwGetEntryWriteFunction(
  IN FWENTRY_OPERATION_PATH_TYPE  PathType,
  IN QPAYLOAD_FW_ENTRY            *FwEntry,
  OUT FWENTRY_WRITE               *Fn
    )
{
  if ((FwEntry == NULL) || (Fn == NULL)) 
  {
    return EFI_INVALID_PARAMETER;
  } 
  
  switch(PathType)
  {
  case FWENTRY_OPERATION_PATH_TYPE_SOURCE:

    return EFI_UNSUPPORTED;

  case FWENTRY_OPERATION_PATH_TYPE_BACKUP:

    if (FwEntry->Metadata.BackupType < FWENTRY_BACKUP_TYPE_MAX)
    {
      *Fn = FwEntryBackupWriteFunctions[FwEntry->Metadata.BackupType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }

    break;

  case FWENTRY_OPERATION_PATH_TYPE_DEST:

    if (FwEntry->Metadata.UpdateType < FWENTRY_UPDATE_TYPE_MAX)
    {
      *Fn = FwEntryDestWriteFunctions[FwEntry->Metadata.UpdateType];
    }
    else
    {
      return EFI_INVALID_PARAMETER;
    }

    break;

  default:

    return EFI_INVALID_PARAMETER;

  }

  //Need to check if got a valid function from the array
  if (Fn == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
 * Delete Partition entry
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDeletePartitionEntry(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry
    )
{
  EFI_STATUS                   Status      = EFI_SUCCESS;
  EFI_HANDLE                   PartHandle  = NULL;
  
  if(FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  PartHandle = FwEntry->Handle[PathType];
  
  if(PartHandle == NULL)
  {
    return EFI_UNSUPPORTED;
  }

  if(PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE)
  {
    return EFI_UNSUPPORTED;
  }

  Status = FwUpdatePartitionDelete(PartHandle);
  HANDLE_ERROR_LABEL(Status, FwUpdatePartitionDelete, FwUpdateDeletePartitionExit);
  
FwUpdateDeletePartitionExit:
  
  return Status;
}

/**
 * Delete FAT entry
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDeleteFatEntry(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry
    )
{
  EFI_STATUS                         Status       = EFI_SUCCESS;
  EFI_HANDLE                         PartHandle   = NULL;
  CHAR16                            *FileName     = NULL;

  if (FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  PartHandle = FwEntry->Handle[PathType];

  if (PartHandle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  
  switch(PathType)
  {
  case FWENTRY_OPERATION_PATH_TYPE_SOURCE:

    return EFI_UNSUPPORTED;

  case FWENTRY_OPERATION_PATH_TYPE_BACKUP:

    FileName = FwEntry->Metadata.BackupPath.FileName;

    break;

  case FWENTRY_OPERATION_PATH_TYPE_DEST:

    FileName = FwEntry->Metadata.UpdatePath.FileName;

    break;

  default:
    return EFI_INVALID_PARAMETER;
    
  }
  
  Status = FwUpdateFatDelete(PartHandle,FileName);
  HANDLE_ERROR_LABEL(Status, FwUpdateFatDelete, FwUpdateDeleteFatEntryExit);

FwUpdateDeleteFatEntryExit:

  return Status;
}
/**
 * Delete Dpp entry
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @Param Type       - Dpp Type QCOM/OEM
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDeleteDppEntry(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN CHAR16                            *Type
    )
{
  EFI_STATUS  Status  = EFI_SUCCESS;
  CHAR16     *DppName = NULL;

  if (FwEntry == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  switch(PathType)
  {
  case FWENTRY_OPERATION_PATH_TYPE_SOURCE:
  case FWENTRY_OPERATION_PATH_TYPE_BACKUP:

    return EFI_UNSUPPORTED;

  case FWENTRY_OPERATION_PATH_TYPE_DEST:

    DppName = FwEntry->Metadata.UpdatePath.FileName;

    break;

  default:
    return EFI_INVALID_PARAMETER;
    
  }

  Status = FwUpdateDppDelete(Type,DppName);
  HANDLE_ERROR_LABEL(Status, FwUpdateFatDelete, FwUpdateDeleteDppEntryExit);

FwUpdateDeleteDppEntryExit:

  return Status;
}

/**
 * Delete QCOM Dpp entry
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDeleteQcomDppEntry(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry
    )
{
  return FwUpdateDeleteDppEntry(PathType,FwEntry,L"QCOM");
}

/**
 * Delete OEM Dpp entry
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateDeleteOemDppEntry(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry
    )
{
  return FwUpdateDeleteDppEntry(PathType,FwEntry,L"OEM");
};

/**
 * Reads Entry from an FV
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromFV(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize
    )

{
  EFI_STATUS                          Status     = EFI_SUCCESS;
  EFI_HANDLE                          PartHandle = NULL;
  
  if ((FwEntry == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL))
  {  
    return EFI_INVALID_PARAMETER;
  }
  *Buffer = NULL;

  //If PathType is Source we should return unsupported
  //as source is FV which is not FAT
  if (PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE) 
  {
    PartHandle = FwEntry->Handle[PathType];

    if (PartHandle == NULL)
    {
      return EFI_UNSUPPORTED;
    }
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_BACKUP) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_DEST) 
  {
    return EFI_UNSUPPORTED;
  }

  Status = FwUpdateFvRead(PartHandle,
                          &(FwEntry->Metadata.FileGuid),
                          Buffer,
                          BufferSize);
  
  HANDLE_ERROR_LABEL(Status,FwUpdateFvRead,FwUpdateReadEntryFromFVExit);

  return EFI_SUCCESS;

FwUpdateReadEntryFromFVExit:

  if(EFI_ERROR(Status))
  {
    if(*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }
  

  return Status;
}


/**
 * Reads an entry from an unformatted partition
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromPartition(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize
    )
{
  EFI_STATUS               Status      = EFI_SUCCESS;
  EFI_HANDLE               PartHandle  = NULL;

  if ((FwEntry == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL))
  {
    return EFI_INVALID_PARAMETER;
  } 
  *Buffer = NULL;

  PartHandle = FwEntry->Handle[PathType];

  if (PartHandle == NULL)
  {
    return EFI_UNSUPPORTED;
  }
         
  Status = FwUpdatePartitionRead(PartHandle,Buffer,BufferSize);
  HANDLE_ERROR_LABEL(Status,FwUpdatePartitionRead,FwUpdateReadEntryFromBlockExit);

  return EFI_SUCCESS;

FwUpdateReadEntryFromBlockExit:
  
  if(EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }
  return Status;
}

/**
 * Reads a entry from a FAT partition
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size

 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromFat(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize
    )
{
  EFI_STATUS Status     = EFI_SUCCESS;
  EFI_HANDLE PartHandle = NULL;
  CHAR16     *FileName  = NULL;

  if ((FwEntry == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  *Buffer = NULL;

  //If PathType is Source we should return unsupported
  //as source is FV which is not FAT
  if (PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_BACKUP) 
  {
    FileName = FwEntry->Metadata.BackupPath.FileName;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_DEST) 
  {
    FileName = FwEntry->Metadata.UpdatePath.FileName;
  }
  else
  {
    return EFI_INVALID_PARAMETER;
  }

  if (FileName == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  PartHandle = FwEntry->Handle[PathType];

  if (PartHandle == NULL)
  {
    return EFI_UNSUPPORTED;
  }

  Status = FwUpdateFatRead(PartHandle,FileName,Buffer,BufferSize); 

  if(Status == EFI_NOT_FOUND)
  {
    goto FwUpdateReadEntryFromFatExit;
  }
  else
  {
    HANDLE_ERROR_LABEL(Status, FwUpdateFatRead, FwUpdateReadEntryFromFatExit);
  }
  return EFI_SUCCESS;

FwUpdateReadEntryFromFatExit:

  /* Free up the buffer if there was a failure in Read(). */
  if (EFI_ERROR(Status))
  {
    if (*Buffer != NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }

  return Status;
}

/**
 * Reads a DPP item from DPP Qcom or OEM path
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 * @param Type       - OEM or DPP 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromDpp(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize,
    IN CHAR16                            *Type
    )
{
  EFI_STATUS       Status       = EFI_SUCCESS;
  CHAR16           *DPPName     = NULL;

  if ((FwEntry == NULL) || 
      (Buffer == NULL) || 
      (BufferSize == NULL) || 
      (Type == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }
  *Buffer = NULL;

  //If PathType is Source we should return unsupported
  //as source is FV and not DPP
  if (PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_BACKUP) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_DEST) 
  {
    DPPName = FwEntry->Metadata.UpdatePath.FileName;
  }
  else
  {
    return EFI_INVALID_PARAMETER;
  }

  if (DPPName == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FwUpdateDppRead(Type,DPPName,Buffer,BufferSize);
  HANDLE_ERROR_LABEL(Status, FwUpdateDppRead, FwUpdateReadEntryFromDppExit);

  return EFI_SUCCESS;

FwUpdateReadEntryFromDppExit:
  if (EFI_ERROR(Status))
  {
    if (*Buffer !=NULL)
    {
      gBS->FreePool(*Buffer);
      *Buffer = NULL;
    }
  }
  return Status;
}

/**
 * Reads a DPP item from DPP Qcom path
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromDppQcom(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize
    )
{
  return FwUpdateReadEntryFromDpp(PathType,FwEntry, Buffer, BufferSize,L"QCOM");
}

/**
 * Reads a DPP item from DPP OEM path
 *  into a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateReadEntryFromDppOem(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              **Buffer,
    IN OUT UINTN                         *BufferSize
    )
{
  return FwUpdateReadEntryFromDpp(PathType,FwEntry, Buffer, BufferSize,L"OEM");
}


/**
 * Writes a block file to an unformatted partition
 *  from a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteEntryToPartition(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              *Buffer,
    IN UINTN                             BufferSize
    )
{
  EFI_STATUS               Status;
  EFI_HANDLE               PartHandle;

  if ((FwEntry == NULL) || (Buffer == NULL)) 
    return EFI_INVALID_PARAMETER;

  PartHandle = FwEntry->Handle[PathType];

  if (PartHandle == NULL)
  {
    return EFI_UNSUPPORTED;
  }

  if(PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE)
  {
    return EFI_UNSUPPORTED;
  }

  Status = FwUpdatePartitionWrite(PartHandle,
                                  Buffer,
                                  BufferSize);
  HANDLE_ERROR_LABEL(Status,FwUpdatePartitionWrite,FwUpdateWriteEntryToBlockExit);

FwUpdateWriteEntryToBlockExit:

  return Status;
}

/**
 * Writes a fat file to FAT partition from a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteEntryToFat(
    IN FWENTRY_OPERATION_PATH_TYPE        PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              *Buffer,
    IN UINTN                              BufferSize
    )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        PartHandle;
  CHAR16                            *FileName = NULL;

  if ((FwEntry == NULL) || (Buffer == NULL)) 
    return EFI_INVALID_PARAMETER;

  //If PathType is Source we should return unsupported
  //as source is FV and not FAT
  if (PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_BACKUP) 
  {
    FileName = FwEntry->Metadata.BackupPath.FileName;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_DEST) 
  {
    FileName = FwEntry->Metadata.UpdatePath.FileName;
  }
  else
  {
    return EFI_INVALID_PARAMETER;
  }

  PartHandle = FwEntry->Handle[PathType];

  if (PartHandle == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = FwUpdateFatWrite(PartHandle,
                            FileName,
                            Buffer,
                            BufferSize);
  HANDLE_ERROR_LABEL(Status,FwUpdateFatWrite,FwUpdateWriteEntryToFatExit);

FwUpdateWriteEntryToFatExit:

  return Status;
}

/**
 * Writes Dpp item to Dpp Qcom or OEM path from a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size 
 * @param Type       - QCOM or OEM 
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteEntryToDpp(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              *Buffer,
    IN UINTN                             BufferSize,
    IN CHAR16                            *Type
    )
{
  EFI_STATUS                   Status  = EFI_SUCCESS;
  CHAR16                      *DPPName = NULL;

  if ((FwEntry == NULL) || (Buffer == NULL) || (Type == NULL)) 
    return EFI_INVALID_PARAMETER;

  //If PathType is Source we should return unsupported
  //as source is FV and not DPP
  if (PathType == FWENTRY_OPERATION_PATH_TYPE_SOURCE) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_BACKUP) 
  {
    return EFI_UNSUPPORTED;
  }
  else if (PathType == FWENTRY_OPERATION_PATH_TYPE_DEST) 
  {
    DPPName = FwEntry->Metadata.UpdatePath.FileName;
  }
  else
  {
    return EFI_INVALID_PARAMETER;
  }

  if(DPPName == NULL)
  {
    return EFI_UNSUPPORTED;
  }

  Status = FwUpdateDppWrite(Type,DPPName,Buffer,BufferSize);
  
  HANDLE_ERROR_LABEL(Status, FwUpdateDppWrite, FwUpdateWriteEntryToDppExit);

FwUpdateWriteEntryToDppExit:

  return Status;
}

/**
 * Writes Dpp item to Dpp Qcom path from a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteEntryToDppQcom(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              *Buffer,
    IN UINTN                             BufferSize
    )
{
  return FwUpdateWriteEntryToDpp(PathType,FwEntry,Buffer,BufferSize,L"QCOM");
}

/**
 * Writes Dpp item to Dpp Oem path from a buffer.
 * 
 * @param PathType   - Source/Dest/BackUp
 * @Param FwEntry    - FwEntry
 * @param Buffer     - Buffer to store the file
 * @param BufferSize - Buffer size
 *  
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateWriteEntryToDppOem(
    IN FWENTRY_OPERATION_PATH_TYPE       PathType,
    IN QPAYLOAD_FW_ENTRY                 *FwEntry,
    IN VOID                              *Buffer,
    IN UINTN                             BufferSize
    )
{
  return FwUpdateWriteEntryToDpp(PathType,FwEntry,Buffer,BufferSize,L"OEM");
}
