/** @file GptListener.c

 * This file implements eMMC GPT partition Listener interface
 * on the non-secure side
 *
 * Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2015-10-19   jt      UFS GPT changes 
  2015-04-17   jt/rm   Add support for ufs rpmb
  2015-01-28   rm      Add GPTListenerAllocMem
  2014-04-16   bn      Fixed Klockwork warnings  
  2014-03-27   bn      Fixed 64-bit pointer size
  2013-04-05   jt      Return listener buffer size as part of init 
  2013-03-05   bn      Fixed klockwork warning
  2013-02-19   jt      Added command to configure GPT partitions
  2013-02-13   bn      Fixed klockwork warnings  
  2013-01-17   niting  Made listener buffer a dynamically allocated buffer
  2012-11-02   bb      Initial Version

 =============================================================================*/

#include <SDMgrMsgs.h>
#include <Library/GPTListener.h>

#include <Library/MemoryAllocationLib.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/EFIScm.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/RpmbLib.h>

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
extern EFI_GUID gQcomScmProtocolGuid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiEmmcBootPartition1Guid;
extern EFI_GUID gEfiEmmcBootPartition2Guid;
extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcGppPartition2Guid;
extern EFI_GUID gEfiEmmcGppPartition3Guid;
extern EFI_GUID gEfiEmmcGppPartition4Guid;
extern EFI_GUID gEfiEmmcRpmbPartitionGuid;
extern EFI_GUID gEfiUfsLU0Guid;
extern EFI_GUID gEfiUfsLU1Guid;
extern EFI_GUID gEfiUfsLU2Guid;
extern EFI_GUID gEfiUfsRPMBGuid; 
extern EFI_GUID gEfiUfsLU4Guid;
extern EFI_GUID gEfiUfsLU5Guid;
extern EFI_GUID gEfiUfsLU6Guid;
extern EFI_GUID gEfiUfsLU7Guid;

/*===========================================================================
  Public Structures and Variables
===========================================================================*/

/* Global structure for storing GPT Listener details */
typedef struct
{
  UINT8 Buffer[GPT_LISTENER_BUFFER_MAX];
  UINT32 IsInited;
  EFI_BLOCK_IO_PROTOCOL  *BlkIo;
  QCOM_SCM_PROTOCOL *ScmProtocol;
  UINT32 Reserved; // for alignment
} GPT_LISTENER_PARAMS;

typedef struct GptPartiGuid
{
   SDMgrDeviceType DevId;
   EFI_GUID *GUID;
} GptPartiGuidType;


static GPT_LISTENER_PARAMS *GPTListenerParams = NULL;
static BOOLEAN IsUFSDevice = FALSE; 

static GptPartiGuidType GptPartiGuidArray[SD_MGR_DEVICE_ID_MAX] =
  {
    {SD_MGR_DEVICE_EMMC_USER, &gEfiEmmcUserPartitionGuid },
    {SD_MGR_DEVICE_EMMC_BOOT1,&gEfiEmmcBootPartition1Guid},
    {SD_MGR_DEVICE_EMMC_BOOT0,&gEfiEmmcBootPartition2Guid},
    {SD_MGR_DEVICE_EMMC_RPMB, &gEfiEmmcRpmbPartitionGuid},
    {SD_MGR_DEVICE_EMMC_GPP1, &gEfiEmmcGppPartition1Guid},
    {SD_MGR_DEVICE_EMMC_GPP2, &gEfiEmmcGppPartition2Guid},
    {SD_MGR_DEVICE_EMMC_GPP3, &gEfiEmmcGppPartition3Guid},
    {SD_MGR_DEVICE_EMMC_GPP4, &gEfiEmmcGppPartition4Guid}
  };

static GptPartiGuidType GptUfsPartiGuidArray[SD_MGR_DEVICE_ID_MAX] =
  {
    {SD_MGR_DEVICE_EMMC_USER, &gEfiUfsLU0Guid },
    {SD_MGR_DEVICE_EMMC_BOOT1,&gEfiUfsLU1Guid },
    {SD_MGR_DEVICE_EMMC_BOOT0,&gEfiUfsLU2Guid },
    {SD_MGR_DEVICE_EMMC_RPMB, &gEfiUfsRPMBGuid },
    {SD_MGR_DEVICE_EMMC_GPP1, &gEfiUfsLU4Guid },
    {SD_MGR_DEVICE_EMMC_GPP2, &gEfiUfsLU5Guid },
    {SD_MGR_DEVICE_EMMC_GPP3, &gEfiUfsLU6Guid },
    {SD_MGR_DEVICE_EMMC_GPP4, &gEfiUfsLU7Guid }
  };

static SDMgrDeviceType SearchOrderArray[2] =
  {
    SD_MGR_DEVICE_EMMC_GPP1,
    SD_MGR_DEVICE_EMMC_USER
  };

#define GPT_PARTI_SECTOR_SIZE  512

/*===========================================================================
  FORWARD FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS EFIAPI GPTListenerCallback(void *Handle,void *BufferPtr,UINT32 BufferLen);
EFI_STATUS GPTInitProtocols(void);
void GPTListenerPartiInit (void *BufferPtr, UINT32 BufferLen);
void GPTListenerReadSectors (void *BufferPtr, UINT32 BufferLen);
void GPTListenerWriteSectors (void *BufferPtr, UINT32 BufferLen);
void GPTListenerConfigPartitions(void *BufferPtr, UINT32 BufferLen); 

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/**

  This function allocates memory for the GPT listener.

**/
EFI_STATUS EFIAPI GPTListenerAllocMem(void)
{
  GPTListenerParams = (GPT_LISTENER_PARAMS *)UncachedAllocateZeroPool(sizeof(GPT_LISTENER_PARAMS));

  if (GPTListenerParams == NULL)
  {
    DEBUG ((EFI_D_ERROR, "GPTListenerAllocMem: Failed memory allocation\n"));
    return EFI_OUT_OF_RESOURCES;
  } 

  return EFI_SUCCESS;
}

/**

  This initializes the GPT Listener.

**/
EFI_STATUS EFIAPI GPTListenerInit(UINT32 MediaId)
{
  EFI_STATUS Status = EFI_DEVICE_ERROR;
  QCOM_SCM_PROTOCOL *ScmProtocol;

  if (GPTListenerParams == NULL)
  {
    DEBUG ((EFI_D_ERROR, "GPTListenerInit: GPTListenerParams is NULL\n"));
    return EFI_OUT_OF_RESOURCES;
  } 
  
  do
  {
    if (TRUE == GPTListenerParams->IsInited)
    {
      Status = EFI_SUCCESS;
      break;
    }

    /* Init the protocols */
    Status = GPTInitProtocols();
    if (Status != EFI_SUCCESS)
    {
      break;
    }

    /* Check to see if we are using a UFS device */
    if (MediaId == SIGNATURE_32('u','f','s',' '))
    {
        IsUFSDevice = TRUE; 
    }

    /* Register the listener */
    ScmProtocol = GPTListenerParams->ScmProtocol;

    Status = ScmProtocol->ScmRegisterCallback(ScmProtocol,
                                              SD_MGR_GPT_LISTENER_ID, 
                                              GPTListenerCallback,
                                              (void *) GPTListenerParams->BlkIo, 
                                              &(GPTListenerParams->Buffer[0]),
                                              GPT_LISTENER_BUFFER_MAX);
    if (Status == EFI_SUCCESS)
    {
      GPTListenerParams->IsInited=TRUE;
    }

  } while(0);

  return Status;
}

/**

  This is the callback for GPT Listener events 

**/
EFI_STATUS EFIAPI GPTListenerCallback(void *Handle, void *BufferPtr, UINT32 BufferLen)
{
  UINT32 *tCmdID;

  do
  {
    if (BufferLen < sizeof(TzSDGptInitReqType))
    {
      // Error - Not enough buffer to do any operation
      break;
    }

    /* First field of all commands is the cmd_id. So using
       UINT32 to get access to cmd_id */
    tCmdID = (UINT32 *) BufferPtr;

    switch (*tCmdID)
    {
      case SD_MGR_MSG_GPT_PARTI_INIT_ID:
        GPTListenerPartiInit(BufferPtr, BufferLen);
        break;

      case SD_MGR_MSG_GPT_PARTI_READ_ID:
        GPTListenerReadSectors(BufferPtr, BufferLen);
        break;

      case SD_MGR_MSG_GPT_PARTI_WRITE_ID:
        GPTListenerWriteSectors(BufferPtr, BufferLen);
        break;
      
      case SD_MGR_MSG_GPT_PARTITION_ID : 
        GPTListenerConfigPartitions(BufferPtr, BufferLen); 
        break; 

      default:
        /* Invalid Command ID. Since the API doesn't return
           an error code, just setting the shared buffer to 0xFF */
        SetMem((UINT8 *) BufferPtr, 0xFF, BufferLen);
        break;
    }
  } while (0);

  return EFI_SUCCESS;
}

/**

  This function finds the SCM protocol for GPT operations

**/
EFI_STATUS GPTInitProtocols(void)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (NULL == GPTListenerParams->ScmProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomScmProtocolGuid, NULL,
                     (void**)&GPTListenerParams->ScmProtocol);
  }

  return Status;
}

/**

  This function get the GUID for the corresponding partition 

**/
EFI_STATUS GPTGetGUID (SDMgrDeviceType DevId, EFI_GUID **GUID)
{
  EFI_STATUS Status = EFI_DEVICE_ERROR;

  if (DevId <= SD_MGR_DEVICE_EMMC_GPP4)
  {
    if (IsUFSDevice)
    {
      *GUID = GptUfsPartiGuidArray[DevId].GUID;
    }
    else
    {
      *GUID = GptPartiGuidArray[DevId].GUID;

    }

    Status = EFI_SUCCESS;
  }

  return Status;
}

/**

  This function finds the GPT BlockIO handles

**/
EFI_STATUS GPTFindGPTPartitionHandle (SDMgrDeviceType *SearchArray,
  UINT32 DevCount, EFI_GUID *PartiGuid, EFI_BLOCK_IO_PROTOCOL  **BlkIo)
{
  EFI_GUID *RootDeviceType;
  UINT32 Attrib = 0, MaxHandles = 2;
  HandleInfo HandleInfoList[2];
  PartiSelectFilter HandleFilter;
  EFI_STATUS Status = EFI_NO_MEDIA, tmpStatus;

  while (DevCount)
  {
    /* Find the GUID for the device ID passed */
    if (EFI_SUCCESS != GPTGetGUID(*SearchArray, &RootDeviceType))
    {
      SearchArray++;
      DevCount--;
      continue;
    }

    /* Find the block IO protocol for the partition asked */
    Attrib |= BLK_IO_SEL_PARTITIONED_GPT;
    Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
    Attrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
    Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

    HandleFilter.PartitionType = PartiGuid;
    HandleFilter.RootDeviceType = RootDeviceType;
    HandleFilter.VolumeName = 0;

    MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);

    tmpStatus = GetBlkIOHandles(Attrib, &HandleFilter, HandleInfoList, &MaxHandles);

    if ((tmpStatus != EFI_SUCCESS) || (MaxHandles != 1))
    {
      SearchArray++;
      DevCount--;
      continue;
    }

    *BlkIo = HandleInfoList[0].BlkIo;
    Status = EFI_SUCCESS;
    break;
  }

  return Status;
}

/**

  This function handles the GPT DeviceInit listener request 

**/
void GPTListenerPartiInit (void *BufferPtr, UINT32 BufferLen)
{
  TzSDGptInitRspTypeV02 *DevInitRsp;
  TzSDGptInitReqType *DevInitReq;
  TzSDGptInitReqType mTzSDGptInitReq; /* copy of incoming request */
  SDMgrDeviceType *SearchArray = &SearchOrderArray[0];
  UINT32 SearchCount = 2, PartitionSize = 0;

  do
  {
    /* Copy over request */
    CopyMem(&mTzSDGptInitReq, BufferPtr, sizeof(TzSDGptInitReqType));
    DevInitReq = &mTzSDGptInitReq;

    DevInitRsp = (TzSDGptInitRspTypeV02 *) BufferPtr;

    DevInitRsp->CmdID = SD_MGR_MSG_GPT_PARTI_INIT_ID;
    DevInitRsp->Status = SD_MGR_MSG_ERROR;

    if (SD_MGR_MSG_GPT_PARTI_INIT_ID == DevInitReq->CmdID)
    {
      if (!TZSDMgrGptVersionCheck(DevInitReq->Version))
      {
        DevInitRsp->Status = SD_MGR_MSG_VERSION_MISMATCH;
        break;
      }

      if (SD_MGR_DEVICE_EMMC_ALL != DevInitReq->DevID)
      {
        SearchArray = &(DevInitReq->DevID);
        SearchCount = 1;
      }

      if (EFI_SUCCESS != GPTFindGPTPartitionHandle (SearchArray,
        SearchCount, (EFI_GUID *)(&DevInitReq->GptGuid), &(GPTListenerParams->BlkIo)))
      {
        DevInitRsp->Status = SD_MGR_MSG_DEVICE_NOT_FOUND_ERROR;
        break;
      }

      PartitionSize = (UINT32)(GPTListenerParams->BlkIo->Media->LastBlock + 1);

      /* Fill the response */
      DevInitRsp->Status = SD_MGR_MSG_SUCCESS;
      DevInitRsp->Version = SD_MGR_GPT_MSGS_VERSION;
      DevInitRsp->NumSectors = PartitionSize;
      DevInitRsp->ListBufSize = GPT_LISTENER_BUFFER_MAX;
      DevInitRsp->CtxtID = (UINTN) GPTListenerParams->BlkIo;
      DevInitRsp->BytesPerSector = GPTListenerParams->BlkIo->Media->BlockSize; 
    }
    else /* Error: Invalid Parameters */
    {
      DevInitRsp->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  }while (0);
}

/**

  This function handles the GPT read sectors listener request

**/
void GPTListenerReadSectors (void *BufferPtr, UINT32 BufferLen)
{
  UINT32 *RespBuffer;
  UINT32 BytesToRead;
  EFI_STATUS DriverStatus;
  EFI_BLOCK_IO_PROTOCOL  *BlkIo;
  TzSDGptRWRspType *GptRWRsp;
  TzSDGptRWReqType *GptRWReq;
  void *mTzSDGptRWReq = NULL;
  UINT32 ReqBuffLen = 0;

  /* Copy over request and request buffer */
  GptRWReq = (TzSDGptRWReqType *)BufferPtr;
  ReqBuffLen = GptRWReq->ReqBuffOffset + GptRWReq->ReqBuffLen;
  
  mTzSDGptRWReq = AllocatePool(ReqBuffLen); /* copy of request */
  if (NULL == mTzSDGptRWReq)
  {
    GptRWRsp = (TzSDGptRWRspType *) BufferPtr; 
    GptRWRsp->Status = SD_MGR_MSG_OUT_OF_RESOURCES; 
    DEBUG ((EFI_D_ERROR, "GPTListenerReadSectors: Failed mem allocation. mTzSDGptRWReq\n"));
    return;
  } 
  
  CopyMem(mTzSDGptRWReq, BufferPtr, ReqBuffLen);
  GptRWReq = mTzSDGptRWReq;

  do
  {
    GptRWRsp = (TzSDGptRWRspType *) BufferPtr;
    GptRWRsp->CmdID = SD_MGR_MSG_GPT_PARTI_READ_ID;
    GptRWRsp->Status = SD_MGR_MSG_ERROR;
    GptRWRsp->RspBuffOffset = ((UINTN) GptRWRsp + sizeof(TzSDGptRWRspType)) -
      (UINTN) BufferPtr;

    BlkIo = GPTListenerParams->BlkIo;

    if (SD_MGR_MSG_GPT_PARTI_READ_ID == GptRWReq->CmdID)
    {
      if (NULL == BlkIo)
      {
        break;
      }

      RespBuffer = (UINT32 *) ((UINTN) BufferPtr + GptRWRsp->RspBuffOffset);
      BytesToRead = GptRWReq->NumSectors * BlkIo->Media->BlockSize;

      /* Read the GPT partition */
      DriverStatus = BlkIo->ReadBlocks(BlkIo, BlkIo->Media->MediaId,
        GptRWReq->StartSector, BytesToRead, RespBuffer);

      /* Fill the response */
      GptRWRsp->Status = TZSDMgrGetErrorCode(DriverStatus);
      GptRWRsp->RspBuffLen = BytesToRead;
    }
    else /* Error: Invalid Parameters */
    {
      GptRWRsp->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  } while (0);

  if (mTzSDGptRWReq)
  {
    FreePool(mTzSDGptRWReq);
  }
}

/**

  This function handles the GPT write sectors listener request

**/
void GPTListenerWriteSectors (void *BufferPtr, UINT32 BufferLen)
{
  UINT32 *Buffer;
  UINT32 BytesToWrite;
  EFI_STATUS DriverStatus;
  EFI_BLOCK_IO_PROTOCOL  *BlkIo;
  TzSDGptRWRspType *GptRWRsp;
  TzSDGptRWReqType *GptRWReq;
  void *mTzSDGptRWReq = NULL;
  UINT32 ReqBuffLen = 0;

  /* Copy over request and request buffer */
  GptRWReq = (TzSDGptRWReqType *)BufferPtr;
  ReqBuffLen = GptRWReq->ReqBuffOffset + GptRWReq->ReqBuffLen;

  mTzSDGptRWReq = AllocatePool(ReqBuffLen); /* copy of request */
  if (NULL == mTzSDGptRWReq)
  {
    GptRWRsp = (TzSDGptRWRspType *) BufferPtr; 
    GptRWRsp->Status = SD_MGR_MSG_OUT_OF_RESOURCES; 
    DEBUG ((EFI_D_ERROR, "GPTListenerWriteSectors: Failed mem allocation. mTzSDGptRWReq\n"));
    return;
  }
  
  CopyMem(mTzSDGptRWReq, BufferPtr, ReqBuffLen);
  GptRWReq = mTzSDGptRWReq;

  do
  {
    GptRWRsp = (TzSDGptRWRspType *) BufferPtr;
    GptRWRsp->CmdID = SD_MGR_MSG_GPT_PARTI_WRITE_ID;
    GptRWRsp->Status = SD_MGR_MSG_ERROR;
    GptRWRsp->RspBuffOffset = ((UINTN) GptRWRsp + sizeof(TzSDGptRWRspType)) -
      (UINTN) BufferPtr;

    BlkIo = GPTListenerParams->BlkIo;

    if (SD_MGR_MSG_GPT_PARTI_WRITE_ID == GptRWReq->CmdID)
    {
      if (NULL == BlkIo)
      {
        break;
      }

      Buffer = (UINT32 *) ((UINTN) BufferPtr + GptRWReq->ReqBuffOffset);
      BytesToWrite = GptRWReq->NumSectors * BlkIo->Media->BlockSize;

      /* Read the GPT partition */
      DriverStatus = BlkIo->WriteBlocks(BlkIo, BlkIo->Media->MediaId,
        GptRWReq->StartSector, BytesToWrite, Buffer);

      /* Fill the response */
      GptRWRsp->Status = TZSDMgrGetErrorCode(DriverStatus);
      GptRWRsp->RspBuffLen = 0;
    }
    else /* Error: Invalid Parameters */
    {
      GptRWRsp->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  } while (0);

  if (mTzSDGptRWReq)
  {
    FreePool(mTzSDGptRWReq);
  }
}

/**

  This function handles the GPT Partition Config listener 
  request 

**/
void GPTListenerConfigPartitions (void *BufferPtr, UINT32 BufferLen)
{
  EFI_STATUS Status;
  TzSDGptPartitionReqType *PartiReq; 
  TzSDGptPartitionRspType *PartiRsp; 
  TzSDGptPartitionReqType mTzSDGptPartiReq; /* copy of incoming request */
  UINT32 NumPartitions = 0; 

  /* Copy over request */
  CopyMem(&mTzSDGptPartiReq, BufferPtr, sizeof(TzSDGptPartitionReqType));
  PartiReq = &mTzSDGptPartiReq;

  PartiRsp = (TzSDGptPartitionRspType *) BufferPtr;

  PartiRsp->CmdID = SD_MGR_MSG_GPT_PARTITION_ID;
  PartiRsp->Status = SD_MGR_MSG_ERROR;
  PartiRsp->RspBuffOffset = sizeof(TzSDGptPartitionRspType);

  if (SD_MGR_MSG_GPT_PARTITION_ID == PartiReq->CmdID)
  {
    Status = ConfigStorPartitions(PartiReq->DevID, PartiReq->PartitionGuid, 
                                  PartiReq->Version, &NumPartitions, 
                                  (UINT8 *) BufferPtr + PartiRsp->RspBuffOffset, 
                                  GPTListenerParams->BlkIo->Media);

    /* Update NumPartitions field */
    PartiRsp->NumPartitions = NumPartitions; 

    /* Fill the response */
    PartiRsp->Status = TZSDMgrGetErrorCode(Status);
  }
  else /* Error: Invalid Parameters */
  {
    PartiRsp->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
  }
}



