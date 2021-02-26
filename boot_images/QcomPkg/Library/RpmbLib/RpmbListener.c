/** @file RpmbListener.c

 * This file implements eMMC RPMB partition Listener interface
 * on the non-secure side
 *
 * Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2016-03-11   jt      Remove RpmbHalfSectorGranularity
  2015-04-17   jt/rm   Add support for ufs rpmb
  2015-01-28   rm      Add RPMBListenerAllocMem
  2014-04-16   bn      Fixed Klockwork warnings
  2014-03-27   bn      Fixed 64-bit pointer size
  2013-07-08   jt      Support both old/new write request versions 
  2013-05-28   jt      Get reliable write count from cfg file
  2013-03-05   bn      Fixed klockwork warning
  2013-02-19   jt      Added command to configure RPMB partitions
  2013-02-13   bn      Fixed klockwork warnings  
  2013-01-17   niting  Made listener buffer a dynamically allocated buffer
  2012-11-02   bb      Initial Version

 =============================================================================*/

#include <SDMgrMsgs.h>
#include <Library/RpmbListener.h>

#include <Library/MemoryAllocationLib.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/RpmbLib.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFIRpmb.h>
#include <Library/UncachedMemoryAllocationLib.h>

/*===========================================================================
  Public Structures and Variables
===========================================================================*/

/* Global structure for storing RPMB Listener details */
typedef struct
{
  UINT8 ListenerBuffer[RPMB_LISTENER_BUFFER_MAX];
  UINT32 IsInited;
  EFI_SDCC_RPMB_PROTOCOL *RPMBProtocol;
  QCOM_SCM_PROTOCOL *ScmProtocol;
  UINT32 Reserved;
} RPMB_LISTENER_PARAMS;

static RPMB_LISTENER_PARAMS *RPMBListenerParams = NULL;
UINT32 ReliableWriteCount = 0;
BOOLEAN IsUFS = FALSE; 

#define RPMB_SECTOR_SIZE  512

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
extern EFI_GUID gQcomScmProtocolGuid;
extern EFI_GUID gEfiSdccRpmbProtocolGuid;

/*===========================================================================
  FORWARD FUNCTION DECLARATIONS
===========================================================================*/

EFI_STATUS RPMBInitProtocols(void);
EFI_STATUS RPMBListenerCallback(void *Handle,void *BufferPtr,UINT32 BufferLen);
void RPMBListenerPartiInit (void *BufferPtr, UINT32 BufferLen);
void RPMBListenerReadSectors (void *BufferPtr, UINT32 BufferLen);
void RPMBListenerWriteSectors (void *BufferPtr, UINT32 BufferLen);
void RPMBListenerConfigPartitions(void *BufferPtr, UINT32 BufferLen); 

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/

/* This function allocates memory for the RPMB Listener */
EFI_STATUS EFIAPI RPMBListenerAllocMem()
{
  RPMBListenerParams = UncachedAllocateZeroPool(sizeof(RPMB_LISTENER_PARAMS));

  if (RPMBListenerParams == NULL)
  {
    DEBUG ((EFI_D_ERROR, "RPMBListenerAllocMem: Failed memory allocation\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/* This initializes the RPMB Listener */
EFI_STATUS EFIAPI RPMBListenerInit(UINT32 MediaId)
{
  EFI_STATUS Status = EFI_DEVICE_ERROR;
  QCOM_SCM_PROTOCOL *ScmProtocol;

  if (RPMBListenerParams == NULL)
  {
    DEBUG ((EFI_D_ERROR, "RPMBListenerInit: RPMBListenerParams is NULL\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  
  do
  {
    if (TRUE == RPMBListenerParams->IsInited)
    {
      Status = EFI_SUCCESS;
      break;
    }

    /* Init the protocols */
    Status = RPMBInitProtocols();
    if (Status != EFI_SUCCESS)
    {
      break;
    }
    
    /* Check to see if we are using a UFS device */
    if (MediaId == SIGNATURE_32('u','f','s',' '))
    {
        IsUFS = TRUE; 
    }

    /* Register the listener */
    ScmProtocol = RPMBListenerParams->ScmProtocol;

    Status = ScmProtocol->ScmRegisterCallback(ScmProtocol,
                                              SD_MGR_RPMB_LISTENER_ID, 
                                              RPMBListenerCallback,
                                              RPMBListenerParams->RPMBProtocol, 
                                              &(RPMBListenerParams->ListenerBuffer[0]),
                                              RPMB_LISTENER_BUFFER_MAX);
    if (Status == EFI_SUCCESS)
    {
      RPMBListenerParams->IsInited=TRUE;
    }

  } while(0);

  return Status;
}

/* This is the callback for RPMB Listener events */
EFI_STATUS RPMBListenerCallback(void *Handle, void *BufferPtr, UINT32 BufferLen)
{
  UINT32 *tCmdID;

  do
  {
    if (BufferLen < sizeof(TzSDRpmbInitReqType))
    {
      // Error - Not enough buffer to do any operation
      break;
    }

    /* First field of all commands is the cmd_id. So using
       UINT32 to get access to cmd_id */
    tCmdID = (UINT32 *) BufferPtr;

    switch (*tCmdID)
    {
      case SD_MGR_MSG_RPMB_INIT_ID:
        RPMBListenerPartiInit(BufferPtr, BufferLen);
        break;

      case SD_MGR_MSG_RPMB_READ_ID:
        RPMBListenerReadSectors(BufferPtr, BufferLen);
        break;

      case SD_MGR_MSG_RPMB_WRITE_ID:
        RPMBListenerWriteSectors(BufferPtr, BufferLen);
        break;

      case SD_MGR_MSG_RPMB_PARTITION_ID: 
        RPMBListenerConfigPartitions(BufferPtr, BufferLen); 
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

/* This function locates the RPMB/SCM protocols for RPMB operation */
EFI_STATUS RPMBInitProtocols()
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (NULL == RPMBListenerParams->RPMBProtocol)
  {
    Status = gBS->LocateProtocol(&gEfiSdccRpmbProtocolGuid, NULL,
                     (void**)&RPMBListenerParams->RPMBProtocol);

    if (EFI_SUCCESS != Status)
    {
       return Status;
    }
  }

  if (NULL == RPMBListenerParams->ScmProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomScmProtocolGuid, NULL,
                     (void**)&RPMBListenerParams->ScmProtocol);
  }

  return Status;
}

/* This function handles the RPMB DeviceInit listener request */
void RPMBListenerPartiInit (void *BufferPtr, UINT32 BufferLen)
{
  UINT32 RPMBSize;
  TzSDRpmbInitRspType_v02 *DevInitRsp;
  TzSDRpmbInitReqType *DevInitReq;
  TzSDRpmbInitReqType mTzSDRpmbInitReq; /* copy of incoming request */

  do
  {
    /* Copy over request */
    CopyMem(&mTzSDRpmbInitReq, BufferPtr, sizeof(TzSDRpmbInitReqType));
    DevInitReq = &mTzSDRpmbInitReq;

    DevInitRsp = (TzSDRpmbInitRspType_v02 *) BufferPtr;

    DevInitRsp->CmdID = SD_MGR_MSG_RPMB_INIT_ID;
    DevInitRsp->Status = SD_MGR_MSG_ERROR;

    if (SD_MGR_MSG_RPMB_INIT_ID == DevInitReq->CmdID)
    {
      if (!TZSDMgrRpmbVersionCheck(DevInitReq->Version))
      {
        DevInitRsp->Status = SD_MGR_MSG_VERSION_MISMATCH;
        break;
      }

      if (NULL == RPMBListenerParams->RPMBProtocol)
      {
        break;
      }

      /* RPMB Size in Bytes */
      RPMBSize = RPMBListenerParams->RPMBProtocol->RPMBPartitionSizeInBytes;

      /* Get Reliable Write count */
      ReliableWriteCount = RPMBListenerParams->RPMBProtocol->ReliableWriteCount;

      /* Fill the response */
      DevInitRsp->Status = SD_MGR_MSG_SUCCESS;
      DevInitRsp->NumSectors = (RPMBSize/RPMB_SECTOR_SIZE);
      DevInitRsp->RelWrCount = ReliableWriteCount;
      
      if (IsUFS)
      {
          DevInitRsp->DevType = SD_MGR_UFS_RPMB; 
      }
      else
      {
          DevInitRsp->DevType = SD_MGR_DEVICE_EMMC_RPMB; 
      }
    }
    else /* Error: Invalid Parameters */
    {
      DevInitRsp->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  } while (0);
}

/* This function handles the RPMB read sectors listener request */
void RPMBListenerReadSectors (void *BufferPtr, UINT32 BufferLen)
{
  EFI_STATUS DriverStatus;
  UINT32 *CmdPktBuffer, *RespBuffer;
  EFI_SDCC_RPMB_PROTOCOL *RpmbProtocol;
  TzSDRpmbRWRspType *RpmbRWRes;
  TzSDRpmbRWReqType *RpmbRWReq;
  void* mTzSDRpmbRWReq = NULL;
  UINT32 ReqBuffLen = 0;

  if (NULL == BufferPtr)
  {
    DEBUG ((EFI_D_ERROR, "RPMBListenerReadSectors: BufferPtr is NULL\n"));
    return;  
  }

  /* Copy over request and request buffer */
  RpmbRWReq = (TzSDRpmbRWReqType *)BufferPtr;
  ReqBuffLen = RpmbRWReq->ReqBuffOffset + RpmbRWReq->ReqBuffLen;
  
  mTzSDRpmbRWReq = AllocatePool(ReqBuffLen); /* copy of request */
  if (NULL == mTzSDRpmbRWReq)
  {
    RpmbRWRes = (TzSDRpmbRWRspType *) BufferPtr;
    RpmbRWRes->Status = SD_MGR_MSG_OUT_OF_RESOURCES;
    DEBUG ((EFI_D_ERROR, "RPMBListenerReadSectors: Failed mem allocation.\n"));
    return;
  }
  
  CopyMem(mTzSDRpmbRWReq, BufferPtr, ReqBuffLen);
  RpmbRWReq = mTzSDRpmbRWReq;

  do
  {
    RpmbRWRes = (TzSDRpmbRWRspType *) BufferPtr;

    RpmbRWRes->CmdID = SD_MGR_MSG_RPMB_READ_ID;
    RpmbRWRes->Status = SD_MGR_MSG_ERROR;
    RpmbRWRes->RspBuffOffset = ((UINTN) RpmbRWRes +
      sizeof(TzSDRpmbRWRspType)) - (UINTN) BufferPtr;

    RpmbProtocol = RPMBListenerParams->RPMBProtocol;

    if (SD_MGR_MSG_RPMB_READ_ID == RpmbRWReq->CmdID)
    {
      if (NULL == RpmbProtocol)
      {
        break;
      }

      CmdPktBuffer = (UINT32 *)((UINTN) RpmbRWReq + RpmbRWReq->ReqBuffOffset);
      RespBuffer = (UINT32 *) ((UINTN) BufferPtr + RpmbRWRes->RspBuffOffset);

      /* Read the RPMB partition */
      DriverStatus = RpmbProtocol->RPMBReadBlocks(RpmbProtocol,
        CmdPktBuffer, RpmbRWReq->NumHalfSectors, RespBuffer);

      /* Fill the response */
      RpmbRWRes->Status = TZSDMgrGetErrorCode(DriverStatus);
      RpmbRWRes->RspBuffLen = RpmbRWReq->NumHalfSectors * RPMB_SECTOR_SIZE;
    }
    else /* Error: Invalid Parameters */
    {
      RpmbRWRes->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  } while (0);

  if (mTzSDRpmbRWReq)
  {
    FreePool(mTzSDRpmbRWReq);
  }
}

/* This function handles the RPMB write sectors listener request */
void RPMBListenerWriteSectors (void *BufferPtr, UINT32 BufferLen)
{
  EFI_STATUS DriverStatus;
  UINT32 *CmdPktBuffer, *RespBuffer;
  EFI_SDCC_RPMB_PROTOCOL *RpmbProtocol;
  TzSDRpmbRWRspType *RpmbRWRes;
  TzSDRpmbRWReqType *RpmbRWReq;
  void* mTzSDRpmbRWReq = NULL;
  UINT32 ReqBuffLen = 0;

  if (NULL == BufferPtr)
  {
    DEBUG ((EFI_D_ERROR, "RPMBListenerWriteSectors: BufferPtr is NULL\n"));
    return;  
  }
  
  /* Copy over request and request buffer */
  RpmbRWReq = (TzSDRpmbRWReqType *)BufferPtr;
  ReqBuffLen = RpmbRWReq->ReqBuffOffset + RpmbRWReq->ReqBuffLen;

  mTzSDRpmbRWReq = AllocatePool(ReqBuffLen); /* copy of request */
  if (NULL == mTzSDRpmbRWReq)
  {
    RpmbRWRes = (TzSDRpmbRWRspType *) BufferPtr; 
    RpmbRWRes->Status = SD_MGR_MSG_OUT_OF_RESOURCES;
    DEBUG ((EFI_D_ERROR, "RPMBListenerWriteSectors: Failed mem allocation.\n"));
    return;
  }
  
  CopyMem(mTzSDRpmbRWReq, BufferPtr, ReqBuffLen);
  RpmbRWReq = mTzSDRpmbRWReq;

  do
  {
    RpmbRWRes = (TzSDRpmbRWRspType *) BufferPtr;

    RpmbRWRes->CmdID = SD_MGR_MSG_RPMB_WRITE_ID;
    RpmbRWRes->Status = SD_MGR_MSG_ERROR;
    RpmbRWRes->RspBuffOffset = ((UINTN) RpmbRWRes +
      sizeof(TzSDRpmbRWRspType)) - (UINTN) BufferPtr;

    RpmbProtocol = RPMBListenerParams->RPMBProtocol;

    if (SD_MGR_MSG_RPMB_WRITE_ID == RpmbRWReq->CmdID)
    {
      if (NULL == RpmbProtocol)
      {
        break;
      }

      CmdPktBuffer = (UINT32 *)((UINTN) BufferPtr + RpmbRWReq->ReqBuffOffset);
      RespBuffer = (UINT32 *)((UINTN) BufferPtr + RpmbRWRes->RspBuffOffset);

      /* Set the reliable write count */
      if (RpmbRWReq->ReqBuffOffset == sizeof(TzSDRpmbRWReqType))
      {
        ReliableWriteCount = RpmbRWReq->RelWrCount;
      }

      /* Write to the RPMB partition */
      DriverStatus = RpmbProtocol->RPMBWriteBlocks(RpmbProtocol,
        CmdPktBuffer, RpmbRWReq->NumHalfSectors, RespBuffer);

      /* Fill the response */
      RpmbRWRes->Status = TZSDMgrGetErrorCode(DriverStatus);
      RpmbRWRes->RspBuffLen = RPMB_SECTOR_SIZE;
    }
    else /* Error: Invalid Parameters */
    {
      RpmbRWRes->Status = SD_MGR_MSG_INVALID_PARAM_ERROR;
    }
  } while (0);

  if (mTzSDRpmbRWReq)
  {
    FreePool(mTzSDRpmbRWReq);
  }
}

/* This function handles the RPMB Partition Config listener request */
void RPMBListenerConfigPartitions (void *BufferPtr, UINT32 BufferLen)
{
  EFI_STATUS Status;
  TzSDRpmbPartitionReqType *PartiReq; 
  TzSDRpmbPartitionRspType *PartiRsp; 
  TzSDRpmbPartitionReqType mTzSDRpmbPartiReq; /* copy of incoming request */
  UINT32 NumPartitions = 0; 

  /* Copy over request */
  CopyMem(&mTzSDRpmbPartiReq, BufferPtr, sizeof(TzSDRpmbPartitionReqType));
  PartiReq = &mTzSDRpmbPartiReq;

  PartiRsp = (TzSDRpmbPartitionRspType *) BufferPtr;

  PartiRsp->CmdID = SD_MGR_MSG_RPMB_PARTITION_ID;
  PartiRsp->Status = SD_MGR_MSG_ERROR;
  PartiRsp->RspBuffOffset = sizeof(TzSDRpmbPartitionRspType);

  if ((SD_MGR_MSG_RPMB_PARTITION_ID == PartiReq->CmdID) && 
      (SD_MGR_DEVICE_EMMC_RPMB == PartiReq->DevID))
  {
    Status = ConfigStorPartitions(PartiReq->DevID, NULL, 
                                  PartiReq->Version, &NumPartitions, 
                                  (UINT8 *) BufferPtr + PartiRsp->RspBuffOffset, 
                                  NULL);

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

