/** @file  

               KeyStoreLoader.c

GENERAL DESCRIPTION
  This library functionality loads the SSD Key Store from EMMC and sends it to TZ..

	
  Copyright (c) 2012-2014,2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

/*===========================================================================
                             EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 08/15/14   sm       Switched to new SCM API
 03/18/14   sm       Updated types and standardized typecasting
 07/25/13   shl      Changed printing level.
 01/08/12   rsb      Fix KeyStoreSize to match size of SSD partition
 12/10/12   shl      Switch to new SCM Dxe
 11/08/12   shl      Initial version

===========================================================================*/

#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>
#include "KeyStoreLoader.h"
#include "QcomLib.h"

/*===========================================================================

                     DEFINITIONS AND CONSTANTS

===========================================================================*/

// SSD GUID
static EFI_GUID KeyStorePartitionGuid = { 0x2C86E742, 0x745E, 0x4FDD, { 0xBF, 0xD8, 0xB6, 0xA7, 0xAC, 0x63, 0x87, 0x72 }};

/* Ideally we should not find more than 1 handle */
static HandleInfo HandleInfoList[4];
static UINT32 MaxHandles;
static PartiSelectFilter HandleFilter;


/**
*  tz_ssd_keystore_syscall ()
*
*  @brief 
*  ssd keystore function
*  
*  @param[in]  Buffer          input buffer 
*  @param[in]  BufferSize      input buffer size
*
*  @return
*  EFI_STATUS
*/

static EFI_STATUS
tz_ssd_keystore_syscall
(
  IN VOID            *Buffer,
  IN UINTN            BufferSize
)
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  QCOM_SCM_PROTOCOL             *pQcomScmProtocol = NULL;
  UINT64                         Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                         Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_ssd_protect_keystore_req_t *SysCallReq = (tz_ssd_protect_keystore_req_t*)Parameters;
  tz_ssd_protect_keystore_rsp_t *SysCallRsp = (tz_ssd_protect_keystore_rsp_t*)Results;

  SysCallReq->keystore_ptr                  = (UINTN)Buffer;
  SysCallReq->keystore_len                  = BufferSize;

  // Locate QCOM_SCM_PROTOCOL. 
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Locate SCM Protocol failed, Status =  (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  /* Make a SCM Sys call */
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_SSD_PROTECT_KEYSTORE_ID,
                                            TZ_SSD_PROTECT_KEYSTORE_ID_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->common_rsp.status != 1)
  {    
    DEBUG(( EFI_D_ERROR, "TZ_SSD_PROTECT_KEYSTORE_ID failed, Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
    goto ErrorExit;
  }
  
  if( SysCallRsp->status != TZ_SSD_PKS_SUCCESS )
  {
    DEBUG(( EFI_D_INFO, "SysCallRsp->status = (0x%x)   ", SysCallRsp->status ));
  	switch ( SysCallRsp->status )
  	{
      case TZ_SSD_PKS_INVALID_PTR:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_INVALID_PTR \r\n" ));
        break;
  	  case TZ_SSD_PKS_INVALID_LEN:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_INVALID_LEN \r\n" ));
        break;
      case TZ_SSD_PKS_UNALIGNED_PTR:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_UNALIGNED_PTR \r\n" ));
        break;    
  	  case TZ_SSD_PKS_PROTECT_MEM_FAILED:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_PROTECT_MEM_FAILED \r\n" ));
        break;
  	  case TZ_SSD_PKS_INVALID_NUM_KEYS:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_INVALID_NUM_KEYS \r\n" ));
        break;
      case TZ_SSD_PKS_DECRYPT_FAILED:
        DEBUG(( EFI_D_INFO, "TZ_SSD_PKS_DECRYPT_FAILED \r\n" ));
        break;
  	  default:
        DEBUG(( EFI_D_INFO, "Unknown TZ return type \r\n" ));
        break;
  	}

    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

ErrorExit:
  return Status;
}


/**
*  LoadKeyStore ()
*
*  @brief 
*  LoadKeyStore function reads key store from EMMC and passes it to TZ
*  
*  @param[in]  VOID
*
*  @return
*  EFI_STATUS
*/
EFI_STATUS LoadKeyStore(void)
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                   *KeyStorePtr = NULL;
  UINTN                   KeyStoreSize = 0;
  EFI_BLOCK_IO_PROTOCOL   *BlkIo;

  // Select the BlkIo handle that represents the partition by the referenced 
  // GUID type in GPT partition on Non removable media.
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  MaxHandles = (UINT32)sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionType = &KeyStorePartitionGuid;
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_INFO, "GetBlkIOHandles  ---, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // Having more than one partition is considered ambiguity, so return invalid param 
  if (MaxHandles > 1)
  {
    DEBUG(( EFI_D_INFO, "MaxHandles > 1 \r\n" ));
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  if (MaxHandles == 0)
  {
    DEBUG ((EFI_D_INFO, "Key Blob partition not found \r\n"));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  KeyStoreSize = ((BlkIo->Media->LastBlock)+1) * BlkIo->Media->BlockSize;

  KeyStorePtr = UncachedAllocatePool (KeyStoreSize);
  if (KeyStorePtr == NULL)
  {
    DEBUG ((EFI_D_INFO, "UncachedAllocatePool -- \r\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, KeyStoreSize, KeyStorePtr);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "BlkIo->ReadBlocks  ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  DEBUG(( EFI_D_INFO, "KeyStoreSize = %d\r\n", KeyStoreSize ));

  Status = tz_ssd_keystore_syscall( KeyStorePtr, KeyStoreSize);
  if (EFI_ERROR (Status))
  {
    DEBUG(( EFI_D_INFO, "tz_ssd_keystore_syscall  ---, Status = (0x%p)\r\n", Status));
    goto ErrorExit;
  }

ErrorExit:
  if( KeyStorePtr != NULL )
  {
	// clear content for security concern
    ZeroMem( KeyStorePtr, KeyStoreSize );
    UncachedFreePool (KeyStorePtr);
  }

  return Status;
}
