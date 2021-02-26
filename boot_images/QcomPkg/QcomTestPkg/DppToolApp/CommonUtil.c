#include "CommonUtil.h"

/** @file CommonUtil.c
   
  Qualcomm Dpp tool application

  Copyright (c) 2010-2013, Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/13   mic     Initial version
=============================================================================*/
/**
 * Function to get a matching partition handle
 * after comparing the Emmc type, Partition Name and 
 * Partition type GUID
 *  
 * @param PartHandle    - partition handle
 * @Param PartTypeGuid  - Partition type GUID
 * @Param PartitionName - Partition name
 * @Param Protocol      - Protocol
 * @return : EFI_STATUS
 */
EFI_STATUS
GetPartitionHandle(
    IN OUT EFI_HANDLE              *PartHandle,
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

  if ((PartHandle == NULL) ||
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
  HANDLE_ERROR_LABEL(Status,gBS->LocateHandleBuffer,GetPartitionHandle);
         
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
    DevPathInst = (EFI_DEVICE_PATH_PROTOCOL*)(((UINT8*)DevPathInst)+DevPathInst->Length[0]);

    if(DevPathInst->Type == TYPE_MEDIA_DEVICE_PATH)
    {    
      if((CompareGuid(TempPartTypeGuid,PartTypeGuid)) 
         && (CompareMem(TempPartitionName, PartitionName, StrSize(TempPartitionName)) == 0)) 
      {
        *PartHandle = HandleBuffer[HandleCounter];
        Status = EFI_SUCCESS;
        goto GetPartitionHandle;
      }
    }                   
  }
  
  Status = EFI_NOT_FOUND;

GetPartitionHandle:
  if(HandleBuffer != NULL)
  {
    gBS->FreePool(HandleBuffer);
    HandleBuffer = NULL;
  }
  
  return Status;
};


EFI_STATUS
EFIAPI
PrintDataInHexAndChar (
  IN UINT8 *pData,
  IN UINT32 Size
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINTN  i = 0;
  UINTN  j = 0;
  UINT32 NumCols = OUTPUT_WIDTH;
  UINT32 NumRows = 0;
  UINT32 LastRowColumn = 0;
  UINT8 cData = 0;
  
  NumRows = Size / NumCols;
  LastRowColumn = Size % NumCols;
  NumRows = LastRowColumn == 0 ? NumRows : (NumRows + 1);

  AsciiPrint("-------------------------------------------------------------------\n");
  for (i = 0; i < NumRows; i++)
  {
    if (i == NumRows - 1) 
    {
      NumCols = LastRowColumn;
    }
    
    for (j = 0; j < NumCols; j++) 
    {
      AsciiPrint("%02X ", pData[i*OUTPUT_WIDTH+j]);
    }

    if (i == NumRows - 1) 
    {
      for (j = 0; j < OUTPUT_WIDTH - NumCols; j++) 
      {
        AsciiPrint("   ");
      }
    }

    AsciiPrint("   ");

    for (j = 0; j < NumCols; j++) 
    {
      cData = pData[i*OUTPUT_WIDTH+j];
      if (cData <= 0x20) 
      {
        AsciiPrint("%c", '.');
      }
      else
      {
        AsciiPrint("%c", cData);
      }
    }

    AsciiPrint("\n");
  }

  return Status;
}

EFI_STATUS
AllocateAndZero(
    IN EFI_MEMORY_TYPE  PoolType,
    IN UINTN            Size,
    OUT VOID            **Buffer
    )
{
    EFI_STATUS Status;
    Status = gBS->AllocatePool(PoolType,Size,Buffer);

    if(EFI_ERROR(Status))
    {
        return Status;
    }
    ZeroMem(*Buffer,Size );
    return EFI_SUCCESS;
};
