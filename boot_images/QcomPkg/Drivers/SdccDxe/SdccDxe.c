/** @file SdccDxe.c

  SDCC UEFI Driver code

  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

  Copyright (c) 2008-2009, Apple Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2017/10/17   sb      Populate card_type in GetCardInfo 
2017/06/12   jt      Overwrite SD capabilities in Exit Boot Services 
2017/05/12   sb      Unified erase protocol support for eMMC
2016/12/09   jt      Remove legacy code 
2016/03/11   jt      Adjust ReliableWriteCount
2016/01/26   jt      Update card info 
2015/11/12   jt      Raise TPL to TPL_CALLBACK  
2015/10/19   jt      Pass MediaId as argument for GPTListenerInit 
2015/10/13   rm      Added BlockIO2 functions
2015/08/07   rm      Fix SD card support on device not booting from eMMC
2015/07/13   al      Changing TPL_NOTIFY to TPL_CALLBACK
2015/04/17   jt/rm   Add support for ufs rpmb
2015/04/03   rm      Add check for boot device
2015/01/29   jb      Add virtual address change logic
2015/01/28   rm      Added RPMB Listener and listener memory allocation fix
2014/10/14   bn      Added SDCC_CONFIG_PROTOCOL
2014/07/30   bn      Initial version. Branched from 8994 UEFI

=============================================================================*/

#include "SdccDxe.h"
#include <api/storage/sdcc_errno.h>
#include <Guid/EventGroup.h>
#include <Library/UefiCfgLib.h>
#include <api/storage/sdcc_api.h>
#include <Library/UefiRuntimeLib.h>
#include <SdccBsp.h>
#include <BootConfig.h>
#include "SdccBlockIO2.h"

extern UINTN sdcc_reg_hc[];

static sdcc_mem_info_type   gCardInfo;
static struct sdcc_device   *hHandleSd = NULL;
static struct sdcc_device   *hHandleEmmc = NULL;

/* Event for external SD card detection */
static EFI_EVENT DetectSdCardEvent = NULL;
static BOOLEAN SdDetectEventCreated = FALSE;

/* SCM registration event */
static EFI_EVENT ScmInitialized = NULL;
static VOID *ScmInitializedToken = NULL;

/* Exit boot services event */
static EFI_EVENT ExitBootServicesEvent = NULL;

/* Virtual address change event */
static EFI_EVENT mVirtualAddressChangeEvent = NULL;

/*****************Block IO 2************************/
/*Block IO 2 request list size, which is the number
  of requests */
#define REQ_LIST_SIZE 128

UINT32                   EmmcDriveNo = 0; 
EFI_EVENT                BlkIo2InternalEvent;
extern BLKIO2_REQ_LIST   EmmcReqList;
extern DRIVER_INFO       MMCHS_TransferInfo;
/*****************Block IO 2************************/

/* eMMC Device Path */
typedef struct {
   VENDOR_DEVICE_PATH  Mmc;
   EFI_DEVICE_PATH     End;
} MMCHS_DEVICE_PATH;

static MMCHS_DEVICE_PATH gMmcHsDevicePath [SDCC_EMMC_MAX_PARTITION_ACCESS];

static VENDOR_DEVICE_PATH MmcPath =
{
   HARDWARE_DEVICE_PATH,
   HW_VENDOR_DP,
   (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
   (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
   0
};

static EFI_DEVICE_PATH EndPath =
{
   END_DEVICE_PATH_TYPE,
   END_ENTIRE_DEVICE_PATH_SUBTYPE,
   sizeof (EFI_DEVICE_PATH_PROTOCOL),
   0
};

MMCHS_DEVICE_PATH gSdDevicePath = {
   {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
      (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
      EFI_SD_REMOVABLE_GUID
   },
   {
      END_DEVICE_PATH_TYPE,
      END_ENTIRE_DEVICE_PATH_SUBTYPE,
      sizeof (EFI_DEVICE_PATH_PROTOCOL),
      0
   }
};

static MMCHS_DEV MmchsDeviceSd = {
   MMCHS_DEV_SIGNATURE,                      /* Signature */
   0xFFFF,                                   /* Invalid slot until initialized */
   NULL,                                     /* Client Handle */
   NULL,                                     /* Device Handle */
   0,                                        /* BlockIo */
   0,                                        /* CardInfo */
   0,                                        /* RPMB Protocol (eMMC only. initialized to NULL for SD) */
   0,                                        /* Write Protect Protocol (Initialized to NULL) */
   0,                                        /* eMMC Erase Protocol (Initialized to NULL) */
   0,                                        /* eMMC Secure Erase Protocol (Initialized to NULL) */
   0                                         /* Sdcc Config Protocol (Initialized to NULL) */
};

static MMCHS_DEV MmchsDeviceEmmc[SDCC_EMMC_MAX_PARTITION_ACCESS] = {
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 },
   { MMCHS_DEV_SIGNATURE, 0xFFFF, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* SD Block IO's Media */
static EFI_BLOCK_IO_MEDIA gMediaSd = {
   SIGNATURE_32('s','d','i','o'),            /* MediaId */
   TRUE,                                     /* RemovableMedia */
   FALSE,                                    /* MediaPresent */
   FALSE,                                    /* LogicalPartition */
   FALSE,                                    /* ReadOnly */
   FALSE,                                    /* WriteCaching */
   512,                                      /* BlockSize */
   4,                                        /* IoAlign */
   0,                                        /* LastBlock */
   0,                                        /* LowestAlignedLba */
   0,                                        /* LogicalBlocksPerPhysicalBlock */
   0                                         /* OptimalTransferLengthGranularity */
};

/* eMMC Block IO's Media */
static EFI_BLOCK_IO_MEDIA EmmcMedia = {
   SIGNATURE_32('e','m','m','c'),            /* MediaId */
   FALSE,                                    /* RemovableMedia */
   FALSE,                                    /* MediaPresent */
   FALSE,                                    /* LogicalPartition */
   FALSE,                                    /* ReadOnly */
   FALSE,                                    /* WriteCaching */
   512,                                      /* BlockSize */
   4,                                        /* IoAlign */
   0,                                        /* LastBlock */
   0,                                        /* LowestAlignedLba */
   0,                                        /* LogicalBlocksPerPhysicalBlock */
   0                                         /* OptimalTransferLengthGranularity */
};

static EFI_BLOCK_IO_MEDIA gMediaEmmc[SDCC_EMMC_MAX_PARTITION_ACCESS];

/* SD Card Info Protocol */
static EFI_MEM_CARDINFO_PROTOCOL gSdCardInfoProtocol = {
   EFI_MEM_CARD_INFO_PROTOCOL_REVISION,      /* Revision */
   MMCHSGetCardInfo                          /* GetCardInfo */
};

/* eMMC Card Info Protocol */
static EFI_MEM_CARDINFO_PROTOCOL gEmmcCardInfoProtocol = {
   EFI_MEM_CARD_INFO_PROTOCOL_REVISION,      /* Revision */
   MMCHSGetCardInfo                          /* GetCardInfo */
};

/* eMMC Write Protect Protocol */
static EFI_EMMC_WP_PROTOCOL gEmmcWriteProtectProtocol = {
   EFI_EMMC_WP_PROTOCOL_REVISION,            /* Revision */
   EmmcWriteProtectGetDeviceCapabilities,    /* GetDeviceCapabilities */
   EmmcSetWriteProtect,                      /* SetWriteProtect */
   EmmcSendWriteProtectType                  /* SendWriteProtectType */
};

/* eMMC Erase Protocol */
static EFI_ERASE_BLOCK_PROTOCOL gEmmcEraseProtocol = {
   EFI_ERASE_BLOCK_PROTOCOL_REVISION,         /* Revision */
   512,                                       /* EraseLengthGranularity */
   EmmcEraseBlocks                            /* EraseBlocks */
};

/* Sdcc Config Protocol */
static EFI_SDCC_CONFIG_PROTOCOL gSdccConfigProtocol = {
   EFI_SDCC_CONFIG_PROTOCOL_REVISION,        /* Revision */
   SdccEnableSdhciMode                       /* SdccEnableSdhciMode */
};

/* RPMB Protocol */
static EFI_SDCC_RPMB_PROTOCOL gSdccRpmbProtocol = {
   EFI_RPMB_PROTOCOL_REVISION,               /* Revision */
   0,                                        /* Reliable Write Count */
   0,                                        /* RPMB Partition Size */
   RPMBReadCounterPkt,                       /* Read Counter*/
   RPMBProgProvisionKey,                     /* Program Provision Key */
   RPMBReadBlocks,                           /* Read Sectors */
   RPMBWriteBlocks,                          /* Write Sectors */
   0                                         /* Max RMPB Read/Write Transfer Size */
};

/* SD Block IO Protocol */
EFI_BLOCK_IO_PROTOCOL gBlockIoSd = {
   EFI_BLOCK_IO_PROTOCOL_REVISION2,          /* Revision */
   &gMediaSd,                                /* Media */
   MMCHSReset,                               /* Reset */
   MMCHSReadBlocks,                          /* ReadBlocks */
   MMCHSWriteBlocks,                         /* WriteBlocks */
   MMCHSFlushBlocks                          /* FlushBlocks */
};

/* eMMC Block IO Protocol */
EFI_BLOCK_IO_PROTOCOL gBlockIoEmmc = {
   EFI_BLOCK_IO_PROTOCOL_REVISION2,          /* Revision */
   0,                                        /* Media */
   MMCHSReset,                               /* Reset */
   MMCHSReadBlocksWrapper,                   /* ReadBlocks */
   MMCHSWriteBlocksWrapper,                  /* WriteBlocks */
   MMCHSFlushBlocks                          /* FlushBlocks */
};

/* eMMC Block IO2 Protocol */
EFI_BLOCK_IO2_PROTOCOL gBlockIo2Emmc = {
   0,                                        /* Media */
   MMCHSResetEx,                             /* Reset */
   MMCHSReadBlocksEx,                        /* ReadBlocksEx */
   MMCHSWriteBlocksEx,                       /* WriteBlocksEx */
   MMCHSFlushBlocksEx                        /* FlushBlocksEx */
};

/* Device Paths for all the eMMC physical partitions */
static EFI_GUID EmmcDevicePathGuids[SDCC_EMMC_MAX_PARTITION_ACCESS] = {
   EFI_EMMC_USER_PARTITION_GUID, EFI_EMMC_BOOT_PARTITION1_GUID,
   EFI_EMMC_BOOT_PARTITION2_GUID, EFI_EMMC_RPMB_PARTITION_GUID,
   EFI_EMMC_GPP_PARTITION1_GUID, EFI_EMMC_GPP_PARTITION2_GUID,
   EFI_EMMC_GPP_PARTITION3_GUID, EFI_EMMC_GPP_PARTITION4_GUID
};


/**
   Decode the error code returned from the SDCC APIs

   @param  ecode     Incoming error code
**/
void MMCHSDecodeErrorCode (
   IN SDCC_STATUS                   ecode
   )
{
   switch (ecode)
   {
      case SDCC_NO_ERROR:
         /* Not really any error - No message required */
         break;
      case SDCC_ERR_UNKNOWN:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_UNKNOWN\n"));
         break;
      case SDCC_ERR_CMD_TIMEOUT:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CMD_TIMEOUT\n"));
         break;
      case SDCC_ERR_TIMEOUT:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_TIMEOUT\n"));
         break;
      case SDCC_ERR_CMD_CRC_FAIL:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CMD_CRC_FAIL\n"));
         break;
      case SDCC_ERR_DATA_CRC_FAIL:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_DATA_CRC_FAIL\n"));
         break;
      case SDCC_ERR_CMD_SENT:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CMD_SENT\n"));
         break;
      case SDCC_ERR_PROG_DONE:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_PROG_DONE\n"));
         break;
      case SDCC_ERR_CARD_READY:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CARD_READY\n"));
         break;
      case SDCC_ERR_INVALID_TX_STATE:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_INVALID_TX_STATE\n"));
         break;
      case SDCC_ERR_SET_BLKSZ:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SET_BLKSZ\n"));
         break;
      case SDCC_ERR_SDIO_R5_RESP:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SDIO_R5_RESP\n"));
         break;
      case SDCC_ERR_DMA:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_DMA\n"));
         break;
      case SDCC_ERR_READ_FIFO:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_READ_FIFO\n"));
         break;
      case SDCC_ERR_WRITE_FIFO:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_WRITE_FIFO\n"));
         break;
      case SDCC_ERR_ERASE:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_ERAE\n"));
         break;
      case SDCC_ERR_SDIO:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SDIO\n"));
         break;
      case SDCC_ERR_SDIO_READ:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SDIO_READ\n"));
         break;
      case SDCC_ERR_SDIO_WRITE:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SDIO_WRITE\n"));
         break;
      case SDCC_ERR_SWITCH:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SWITCH\n"));
         break;
      case SDCC_ERR_INVALID_PARAM:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_INVALID_PARAM\n"));
         break;
      case SDCC_ERR_CARD_UNDETECTED:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CARD_UNDETECTED\n"));
         break;
      case SDCC_ERR_FEATURE_UNSUPPORTED:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_FEATURE_UNSUPPORTED\n"));
         break;
      case SDCC_ERR_SECURE_COMMAND_IN_PROGRESS:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns "
                              "ERR_SECTURE_COMMAND_IN_PROGRESS\n"));
         break;
      case SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CMD_NOT_ALLOWED\n"));
         break;
      case SDCC_ERR_ABORT_READ_SEC_CMD:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_READ_SEC_CMD\n"));
         break;
      case SDCC_ERR_CARD_INIT:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CARD_INIT\n"));
         break;
      case SDCC_ERR_CARD_REMOVED:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_CARD_REMOVED\n"));
         break;
      case SDCC_ERR_PWR_ON_WRITE_PROT:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns "
                              "ERR_PWR_ON_WRITE_PROTECT \n"));
         break;
      case SDCC_ERR_WP_VIOLATION:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_WP_VIOLATION\n"));
         break;
      case SDCC_ERR_SPS_MODE:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns ERR_SPS_MODE\n"));
         break;
      default:
         DEBUG ((EFI_D_ERROR, "SDCC driver returns error "
                              "that can not be decoded.\n"));
         break;
   }
}

/**
   A wrapper for Block IO and Block IO 2. whether Block IO or Block IO 2 protocol
   will be used depending on caller and device status.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  Token	  A pointer to the token associated with the transaction.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.
   
   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
EFI_STATUS

**/
EFI_STATUS
MMCHSWrapper (
  IN VOID                     *pThis,
  IN UINT32                    MediaId,
  IN EFI_LBA                   Lba,
  IN OUT EFI_BLOCK_IO2_TOKEN  *Token,
  IN UINTN                     BufferSize,
  IN OUT VOID                 *Buffer
   )
{
   EFI_STATUS        Status = EFI_SUCCESS;
   MMCHS_DEV        *MmchsDevice;
   UINT32            block_size = 0;
   DEVICE_STATUS     ds = DEVICE_IDLE;
   SDCC_STATUS       rc;
   BLKIO2_REQ_LIST *pReqList = &EmmcReqList;
   
   if (MMCHS_TransferInfo.TransferType == BLOCKIO_READ || 
       MMCHS_TransferInfo.TransferType == BLOCKIO_WRITE)
   {
      EFI_BLOCK_IO_PROTOCOL  *This = pThis;
      
      /* Cancel timer for Block IO */
      if (TRUE == MMCHS_TransferInfo.TimerSet)
         gBS->SetTimer (BlkIo2InternalEvent, TimerCancel, 0); 

      ds = sdcc_get_device_status(EmmcDriveNo, &rc);

      if (ds == DEVICE_ERROR)
      {
         Status =  EFI_DEVICE_ERROR;
         goto MMCHSWrapperExit;
      }
      
      /* If no Block IO 2 tranfer is in progress */
      if (!pReqList->CurReq->IsStart)
      {
         if (MMCHS_TransferInfo.TransferType == BLOCKIO_READ)
         {
            Status = MMCHSReadBlocks(This, MediaId, Lba, BufferSize, Buffer);
         }
         else 
         {           
            Status = MMCHSWriteBlocks(This, MediaId, Lba, BufferSize, Buffer);
         }

         /* Set timer if necessary*/
         if (TRUE == MMCHS_TransferInfo.TimerSet)
         {
            gBS->SetTimer (BlkIo2InternalEvent, 
                           TimerPeriodic, 
                           BLK_IO2_POLLING_PERIOD);
         }
         goto MMCHSWrapperExit;
      }
      
      /* If Block IO 2 is in progress, Block IO will take over the transfer 
         and has priority to run. Block IO 2 resumes after Block IO */
      if (pReqList->CurReq->IsStart)
      {         
         Status = BlkIoTakeOverTransfer(This, MediaId, Lba, 
                                        BufferSize, Buffer);
      }

      /* Set timer to call Block IO 2 */
      if (TRUE  == MMCHS_TransferInfo.TimerSet)
         gBS->SetTimer (BlkIo2InternalEvent, 
                        TimerPeriodic, 
                        BLK_IO2_POLLING_PERIOD);
          
   }
   /* If caller calls Block IO 2 */
   else 
   {  
      EFI_BLOCK_IO2_PROTOCOL *This = pThis;

      /* Validate passed-in pointers are not NULL */
      if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media) || 
          (NULL == Token) || (NULL == Token->Event))
      {
         Status = EFI_INVALID_PARAMETER;
         goto MMCHSWrapperExit;
      }
      
      /* Validate if buffer is multiple of block size */
      if ((BufferSize%(This->Media->BlockSize)) != 0 )
      {
         Status = EFI_BAD_BUFFER_SIZE;
         goto MMCHSWrapperExit;
      }

      /* Validate block size and access range */
      if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0)) 
      {
         Status = EFI_INVALID_PARAMETER;
         goto MMCHSWrapperExit;
      }
   
      if (This->Media->MediaId != MediaId)
      {
         Status = EFI_MEDIA_CHANGED;
         goto MMCHSWrapperExit;
      }

      if (0 == BufferSize)
      {
         Status = EFI_BAD_BUFFER_SIZE;
         goto MMCHSWrapperExit;
      }
     
      MmchsDevice = MMCHS_DEV_FROM_BLOCKIO2 (This);       
      block_size = MmchsDevice->BlkIo.Media->BlockSize;

      if ((Lba + (BufferSize/block_size)) > (MmchsDevice->BlkIo.Media->LastBlock + 1))
      {
         Status = EFI_INVALID_PARAMETER;
         goto MMCHSWrapperExit;
      }   
      
      /* Append request to queue */

      EfiAcquireLock (&pReqList->ReqListLock);
      if (pReqList->Full == TRUE)
      { 
         EfiReleaseLock (&pReqList->ReqListLock);      
         Status = EFI_OUT_OF_RESOURCES;
         goto MMCHSWrapperExit;
      }
  
      pReqList->LastReq->Token = Token;
      pReqList->LastReq->This = This;
      pReqList->LastReq->MediaId = MediaId;
      pReqList->LastReq->Lba = Lba;
      pReqList->LastReq->BufferSize = BufferSize;
      pReqList->LastReq->Buffer = Buffer;
      
      /* Append opeartion type */      
      if (MMCHS_TransferInfo.TransferType == BLOCKIO2_READ)
      {        
         pReqList->LastReq->Type = READ;  
      }
      else
      {        
         pReqList->LastReq->Type = WRITE;
      }

      pReqList->LastReq ++;
  
      if(pReqList->LastReq == pReqList->Tail) 
      {
         pReqList->LastReq = pReqList->Head;  
      }
      
      if(pReqList->LastReq == pReqList->CurReq) 
      { 
         pReqList->Full = TRUE;                    
      }

      EfiReleaseLock (&pReqList->ReqListLock);           
      
      /* Call Notity function to do real Block IO 2 transfer */
      BlkIo2TimerEventNotify(NULL, NULL);   
   }

MMCHSWrapperExit:
   return Status;
}

/**
   Read BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.
   
   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
**/
EFI_STATUS
MMCHSReadBlocksWrapper (
  IN EFI_BLOCK_IO_PROTOCOL  *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  OUT VOID                  *Buffer
   )
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   /* SDCC accesses has NOTIFY priority */
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   MMCHS_TransferInfo.TransferType = BLOCKIO_READ;
   Status = MMCHSWrapper(This, MediaId, Lba, NULL, BufferSize, Buffer);

   gBS->RestoreTPL (PrevTPL);

   return Status;
}

/**
   Write BufferSize bytes from Buffer to Lba.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be 
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.
   
   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error 
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched 
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple 
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/
EFI_STATUS
MMCHSWriteBlocksWrapper (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  IN VOID                    *Buffer
   )
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   /* SDCC accesses has NOTIFY priority */
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   MMCHS_TransferInfo.TransferType = BLOCKIO_WRITE;
   Status = MMCHSWrapper(This, MediaId, Lba, NULL, BufferSize, Buffer);

   gBS->RestoreTPL (PrevTPL);

   return Status;
}
 
/**
  Read BufferSize bytes from Lba into Buffer.
  
  This function reads the requested number of blocks from the device. All the
  blocks are read, or an error is returned.
  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_or EFI_MEDIA_CHANGED is returned and
  non-blocking I/O is being used, the Event associated with this request will
  not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    Id of the media, changes every time the media is 
                              replaced.
  @param[in]       Lba        The starting Logical Block Address to read from.
  @param[in, out]  Token	    A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.  
  @param[out]      Buffer     A pointer to the destination buffer for the data. The 
                              caller is responsible for either having implicit or 
                              explicit ownership of the buffer.

  @retval EFI_SUCCESS           The read request was queued if Token->Event is
                                not NULL.The data was read correctly from the
                                device if the Token->Event is NULL.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing
                                the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId is not for the current media.
  @retval EFI_BAD_BUFFER_SIZE   The BufferSize parameter is not a multiple of the
                                intrinsic block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid, 
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.
**/ 
EFI_STATUS
MMCHSReadBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                Lba,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  OUT VOID                      *Buffer
  )
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   /* SDCC accesses has NOTIFY priority */
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   /* Do not use Block IO 2 if not initialized succesfully */
   if (MMCHS_TransferInfo.BlkIO2Initialized == FALSE)
   {
      Status = EFI_OUT_OF_RESOURCES;
      goto MMCHSReadBlocksExExit;
   }
   
   MMCHS_TransferInfo.TransferType = BLOCKIO2_READ;
   Status = MMCHSWrapper(This, MediaId, Lba, Token, BufferSize, Buffer);

MMCHSReadBlocksExExit:
   gBS->RestoreTPL (PrevTPL);
   
   return Status;
}

/**
  Write BufferSize bytes from Buffer to Lba.

  This function writes the requested number of blocks to the device. All blocks
  are written, or an error is returned.If EFI_DEVICE_ERROR, EFI_NO_MEDIA,
  EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED is returned and non-blocking I/O is
  being used, the Event associated with this request will not be signaled.

  @param[in]       This       Indicates a pointer to the calling context.
  @param[in]       MediaId    The media ID that the write request is for.
  @param[in]       Lba        The starting logical block address to be written. The
                              caller is responsible for writing to only legitimate
                              locations.
  @param[in, out]  Token      A pointer to the token associated with the transaction.
  @param[in]       BufferSize Size of Buffer, must be a multiple of device block size.
  @param[in]       Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The write request was queued if Event is not NULL.
                                The data was written correctly to the device if
                                the Event is NULL.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid, 
                                or the buffer is not on proper alignment.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack
                                of resources.

**/
EFI_STATUS
MMCHSWriteBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL *This,
  IN     UINT32                 MediaId,
  IN     EFI_LBA                Lba,
  IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
  IN     UINTN                  BufferSize,
  OUT VOID                      *Buffer
  )
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   /* Do not use Block IO 2 if not initialized succesfully */
   if (MMCHS_TransferInfo.BlkIO2Initialized == FALSE)
   {
      Status = EFI_OUT_OF_RESOURCES;
      goto MMCHSWriteBlocksExExit;
   }  

   MMCHS_TransferInfo.TransferType = BLOCKIO2_WRITE;
   Status = MMCHSWrapper(This, MediaId, Lba, Token, BufferSize, Buffer);

MMCHSWriteBlocksExExit:
   gBS->RestoreTPL (PrevTPL);
   
   return Status;
}

/**
  Reset the block device hardware.

  @param[in]  This                 Indicates a pointer to the calling context.
  @param[in]  ExtendedVerification Indicates that the driver may perform a more
                                   exhausive verfication operation of the device
                                   during reset.

  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
                               not be reset.

**/

EFI_STATUS
EFIAPI
MMCHSResetEx (
  IN EFI_BLOCK_IO2_PROTOCOL  *This,
  IN BOOLEAN                 ExtendedVerification
  )
{
   return EFI_SUCCESS;
}

/**
  Flush the Block Device.
 
  If EFI_DEVICE_ERROR, EFI_NO_MEDIA,_EFI_WRITE_PROTECTED or EFI_MEDIA_CHANGED
  is returned and non-blocking I/O is being used, the Event associated with
  this request will not be signaled.  

  @param[in]      This     Indicates a pointer to the calling context.
  @param[in,out]  Token    A pointer to the token associated with the transaction

  @retval EFI_SUCCESS          The flush request was queued if Event is not NULL.
                               All outstanding data was written correctly to the
                               device if the Event is NULL.
  @retval EFI_DEVICE_ERROR     The device reported an error while writting back
                               the data.
  @retval EFI_WRITE_PROTECTED  The device cannot be written to.
  @retval EFI_NO_MEDIA         There is no media in the device.
  @retval EFI_MEDIA_CHANGED    The MediaId is not for the current media.
  @retval EFI_OUT_OF_RESOURCES The request could not be completed due to a lack
                               of resources.

**/
EFI_STATUS
EFIAPI
MMCHSFlushBlocksEx (
  IN     EFI_BLOCK_IO2_PROTOCOL   *This,
  IN OUT EFI_BLOCK_IO2_TOKEN      *Token
  )
{
   return EFI_SUCCESS;
}

/**
   Reset the Block Device.

   @param  This                 Indicates a pointer to the calling context.
   @param  ExtendedVerification Driver may perform diagnostics on reset.

   @retval EFI_SUCCESS          The device was reset.
   @retval EFI_DEVICE_ERROR     The device is not functioning properly
                                and could not be reset.

**/
EFI_STATUS
EFIAPI
MMCHSReset (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN BOOLEAN                 ExtendedVerification
   )
{
   return EFI_SUCCESS;
}


/**
   Read BufferSize bytes from Lba into Buffer.

   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.

   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
EFI_STATUS

**/
EFI_STATUS
EFIAPI
MMCHSReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  OUT VOID                   *Buffer
  )
{
   SDCC_STATUS  rc;
   MMCHS_DEV    *MmchsDevice;
   EFI_STATUS   status;
   struct sdcc_device *sdcc_handle;
   UINT32 transfer_size = 0, block_count = 0, block_size = 0;
   UINT8 *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */
   if ((BufferSize%(This->Media->BlockSize)) != 0 )
   {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0))
   {
      return EFI_INVALID_PARAMETER;
   }

   if (This->Media->MediaId != MediaId)
   {
      return EFI_MEDIA_CHANGED;
   }

   MmchsDevice = MMCHS_DEV_FROM_BLOCKIO (This);

   sdcc_handle = MmchsDevice->DeviceHandle;

   /* Polling removable card only if its handle is invalid */
   if ((MmchsDevice->BlkIo.Media->RemovableMedia) && (NULL == sdcc_handle))
   {
      status = DetectCard(MmchsDevice, SDCC_SD_PHYSICAL_PARTITION_USER);

      if (EFI_SUCCESS == status)
      {
         gBS->ReinstallProtocolInterface(MmchsDevice->ClientHandle,
                                         &gEfiBlockIoProtocolGuid, &MmchsDevice->BlkIo,
                                         &MmchsDevice->BlkIo);

         return EFI_MEDIA_CHANGED;
      }
      else
      {
         /* return either EFI_NO_MEDIA or EFI_DEVICE_ERROR */
         return status;
      }
   }

   /* If BufferSize is zero, the host is pinging the card for its presence */
   if (0 == BufferSize)
   {
      if (!sdcc_handle_is_mem_present(sdcc_handle))
      {
         (void) sdcc_handle_close(sdcc_handle);
         MmchsDevice->DeviceHandle = NULL;
         MmchsDevice->BlkIo.Media->MediaPresent = FALSE;
         return EFI_NO_MEDIA;
      }
      else
      {
         return EFI_SUCCESS;
      }
   }

   block_size = MmchsDevice->BlkIo.Media->BlockSize;

   block_count = (BufferSize/block_size);

   /* Check if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/block_size)) > (MmchsDevice->BlkIo.Media->LastBlock + 1))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Break transfer into smaller transfers.
      BAM has a limit of SDCC_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;

#ifdef ENABLE_DEBUG_OUTPUT
   DEBUG ((EFI_D_INFO, "RD: 0x%x    %d\n",Lba, block_count));
#endif 
   
   while (block_count > 0)
   {
      transfer_size = (block_count > SDCC_MAX_BLOCK_TRANSFER_SIZE) ?
         SDCC_MAX_BLOCK_TRANSFER_SIZE : block_count;

      rc = sdcc_handle_read( sdcc_handle, Lba, temp_buffer, transfer_size);

      Lba = Lba + transfer_size;
      block_count = block_count - transfer_size;
      temp_buffer = temp_buffer + (transfer_size * block_size);

      if (SDCC_NO_ERROR != rc)
      {
         /* There's an error */
         MMCHSDecodeErrorCode (rc);

         if ((SDCC_ERR_CARD_REMOVED == rc) || (SDCC_ERR_CARD_UNDETECTED == rc) ||
             (SDCC_ERR_CMD_TIMEOUT == rc))
         {
            (void) sdcc_handle_close(sdcc_handle);
            MmchsDevice->DeviceHandle = NULL;
            MmchsDevice->BlkIo.Media->MediaPresent = FALSE;
            return EFI_NO_MEDIA;
         }
         else if (SDCC_ERR_INVALID_PARAM == rc)
         {
            return EFI_INVALID_PARAMETER;
         }
         else
         {
            return EFI_DEVICE_ERROR;
         }
      }
   }

   return EFI_SUCCESS;
}

/**
   Write BufferSize bytes from Lba into Buffer.

   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.

   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/
EFI_STATUS
EFIAPI
MMCHSWriteBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   IN VOID                    *Buffer
   )
{
   SDCC_STATUS  rc;
   MMCHS_DEV    *MmchsDevice;
   struct sdcc_device *sdcc_handle;
   UINT32 transfer_size = 0, block_count = 0, block_size = 0;
   UINT8 *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */
   if (((BufferSize%(This->Media->BlockSize)) != 0))
   {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/This->Media->BlockSize)) > (This->Media->LastBlock + 1))
   {
      return EFI_INVALID_PARAMETER;
   }

   if (This->Media->MediaId != MediaId)
   {
      return EFI_MEDIA_CHANGED;
   }

   MmchsDevice = MMCHS_DEV_FROM_BLOCKIO (This);

   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Break transfer into smaller transfers.
      BAM has a limit of SDCC_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;

   block_size = MmchsDevice->BlkIo.Media->BlockSize;

   block_count = (BufferSize/block_size);

#ifdef ENABLE_DEBUG_OUTPUT
   DEBUG ((EFI_D_INFO, "WR: 0x%x    %d\n",Lba, block_count));
#endif 
   
   while (block_count > 0)
   {
     transfer_size = (block_count > SDCC_MAX_BLOCK_TRANSFER_SIZE) ?
                         SDCC_MAX_BLOCK_TRANSFER_SIZE : block_count;

     rc = sdcc_handle_write( sdcc_handle, Lba, temp_buffer, transfer_size);

     Lba = Lba + transfer_size;
     block_count = block_count - transfer_size;
     temp_buffer = temp_buffer + (transfer_size * block_size);

     if (rc != SDCC_NO_ERROR)
     {
        MMCHSDecodeErrorCode (rc);

        if ((SDCC_ERR_CARD_REMOVED == rc) || (SDCC_ERR_CARD_UNDETECTED == rc) ||
            (SDCC_ERR_CMD_TIMEOUT == rc))
        {
           (void) sdcc_handle_close(sdcc_handle);
           MmchsDevice->DeviceHandle = NULL;
           MmchsDevice->BlkIo.Media->MediaPresent = FALSE;
           return EFI_NO_MEDIA;
        }
        else if (SDCC_ERR_INVALID_PARAM == rc)
        {
           return EFI_INVALID_PARAMETER;
        }
        else if (SDCC_ERR_WP_VIOLATION == rc)
        {
           return EFI_WRITE_PROTECTED;
        }
        else
        {
           return EFI_DEVICE_ERROR;
        }
     }
   }

   return EFI_SUCCESS;
}


/**
   Flush the Block Device.

   @param  This              Indicates a pointer to the calling context.

   @retval EFI_SUCCESS       All outstanding data was written to the device
   @retval EFI_DEVICE_ERROR  The device reported an error while
                             writting back the data
   @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS
EFIAPI
MMCHSFlushBlocks (
   IN EFI_BLOCK_IO_PROTOCOL  *This
   )
{
   return EFI_SUCCESS;
}

/**
   Function: MMCHSGetCardInfo

   This function retrieves the card information.

   @param  card_info              Pointer to a MEM_CARD_INFO
                                  variable that is used to store
                                  the retrieved card information

   @retval EFI_SUCCESS            The function completed successfully.
   @retval EFI_INVALID_PARAMETER  The pass-in parameter is invalid.
**/
EFI_STATUS
EFIAPI
MMCHSGetCardInfo (
   EFI_MEM_CARDINFO_PROTOCOL   *This,
   MEM_CARD_INFO               *card_info
   )
{
   SDCC_STATUS rc;
   MMCHS_DEV   *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;
   
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == card_info) || (NULL == This))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto MMCHSGetCardInfoExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_CARD_INFO (This);

   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto MMCHSGetCardInfoExit;
   }

   /* Get SD/eMMC Card Info */
   rc = sdcc_handle_mem_get_device_info(sdcc_handle, &gCardInfo);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n\n\n", rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto MMCHSGetCardInfoExit;
   }

   if ((gCardInfo.card_type == SDCC_CARD_MMCHC) || 
       (gCardInfo.card_type == SDCC_CARD_MMC))
   {
     card_info->card_type[0] = 'E';
     card_info->card_type[1] = 'M';
     card_info->card_type[2] = 'M';
     card_info->card_type[3] = 'C';
   }
   else
   {
     card_info->card_type[0] = 'S';
     card_info->card_type[1] = 'D';
     card_info->card_type[2] = ' ';
     card_info->card_type[3] = ' ';
   }

   card_info->mfr_id = gCardInfo.mfr_id;
   card_info->oem_id = gCardInfo.oem_id;
   card_info->reliable_write_count = gCardInfo.reliable_write_sector_count;
   card_info->rpmb_size_in_byte = gCardInfo.rpmb_parti_size_in_byte;

   CopyMem(card_info->product_serial_num, &gCardInfo.prod_serial_num, sizeof(UINT32));  
   CopyMem(&(card_info->mfr_date[0]), &gCardInfo.mfr_date, 8);

   EfiStatus = EFI_SUCCESS;

MMCHSGetCardInfoExit:
   gBS->RestoreTPL (PrevTPL);
   
   return EfiStatus;
}

/**
   Function: RPMBReadCounterPkt

   This function retrieves Read Counter Response packet from eMMC device.
   The Response packet includes the Write Counter as well as the MAC which
   is used later to validate the Response packet

   @param  RespPktBuffer [OUT]    Pointer to the response from the Read
                                  Counter command


   @retval  RPMB_NO_ERROR           Successfully read the Counter packet.
   @retval  RPMB_ERR_INVALID_PARAM  NULL pointer is passed in to the function.
   @retval  RPMB_ERR_READ           Failed to read the Counter packet.

**/
INT32
EFIAPI
RPMBReadCounterPkt (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *RespPktBuffer
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device  *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   INT32 RpmbStatus = RPMB_ERR_GENERAL;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == This) || (NULL == RespPktBuffer))
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBReadCounterPktExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_RPMB (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBReadCounterPktExit;      
   }

   rc = rpmb_read_counter_pkt(sdcc_handle, RespPktBuffer);

   if (SDCC_NO_ERROR != rc)
   {
      MMCHSDecodeErrorCode (rc);
      RpmbStatus = RPMB_ERR_READ;
      goto RPMBReadCounterPktExit;
   }

   RpmbStatus = RPMB_NO_ERROR;

RPMBReadCounterPktExit:
   gBS->RestoreTPL (PrevTPL);

   return RpmbStatus;
}

/**
 Function: RPMBProgProvisionKey

 This function programs the eMMC's key using the provided packet
 formatted as ProvisionKey command.

 @param CmdPktBuffer  [IN] : Pointer to a ProvisionKey command packet
                             to be sent to eMMC to program its key
 @param RespPktBuffer [OUT]: Pointer to the response packet for the
                             ProvisionKey command

 @retval  RPMB_NO_ERROR            Successfully provision the eMMC's Key.
 @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function.
 @retval  RPMB_ERR_GENERAL         Failed to provision the key (Key has been
                                   provisioned).

**/
INT32
EFIAPI
RPMBProgProvisionKey (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *CmdPktBuffer,
   UINT32                       *RespPktBuffer
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   INT32 RpmbStatus = RPMB_ERR_GENERAL;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == CmdPktBuffer) || (NULL == RespPktBuffer) || (NULL == This))
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBProgProvisionKeyExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_RPMB (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBProgProvisionKeyExit;
   }

   rc = rpmb_write_sectors(sdcc_handle, CmdPktBuffer, 1,
                           RPMB_PROVISION_PKT, RespPktBuffer);

   if (SDCC_NO_ERROR != rc)
   {
      MMCHSDecodeErrorCode (rc);
      RpmbStatus = RPMB_ERR_GENERAL;
      goto RPMBProgProvisionKeyExit;
   }

   RpmbStatus = RPMB_NO_ERROR;

RPMBProgProvisionKeyExit:
   gBS->RestoreTPL (PrevTPL);   

   return RpmbStatus;
}

/**
  Function: RPMBReadBlocks

  This function reads the sectors from RPMB partition using the
  cmd packet buffers provided.

  @param   CmdPktBuffer  [IN] : Pointer to a formatted packet for Read Request
  @param   PktCount      [IN] : How many half sectors to read
  @param   DataBuffer    [OUT]: Pointer to data read from eMMC

  @retval  RPMB_NO_ERROR            Successfully read the Counter packet
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_READ            Failed to read the sectors

**/
INT32
EFIAPI
RPMBReadBlocks (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *CmdPktBuffer,
   UINT32                       PktCount,
   UINT32                       *DataBuffer
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device  *sdcc_handle = NULL;
   SDCC_STATUS rc = SDCC_NO_ERROR;
   INT32 RpmbStatus = RPMB_ERR_GENERAL;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == CmdPktBuffer) || (NULL == DataBuffer) ||
       (NULL == This) || (PktCount == 0))
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBReadBlocksExit;
   }

   if (PktCount > SDCC_MAX_BLOCK_TRANSFER_SIZE)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBReadBlocksExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_RPMB (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBReadBlocksExit;
   }

   rc = rpmb_read_sectors(sdcc_handle, CmdPktBuffer, PktCount, DataBuffer);
   if (SDCC_NO_ERROR != rc)
   {
      MMCHSDecodeErrorCode (rc);
      RpmbStatus = RPMB_ERR_READ;
      goto RPMBReadBlocksExit;
   }
     
   RpmbStatus = RPMB_NO_ERROR;

RPMBReadBlocksExit:   
   gBS->RestoreTPL (PrevTPL);

   return RpmbStatus;
}

/**
  Function: RPMBWriteBlocks

  This function writes the sectors to the RPMB partition using
  the provided cmd packet buffers

  @param  CmdPktBuffer  [IN] :  Pointer to a formatted packets to be sent
                                to eMMC to write to RPMB partition
  @param  PktCount      [IN] :  Number of half sectors to write
  @param  RespPktBuffer [OUT]:  Pointer to the response packet from eMMC

  @retval  RPMB_NO_ERROR            Successfully Write the RPMB sectors.
                                    Caller should authenticate the Response
                                    packet and validate the Write Counter
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_WRITE           Failed to write the sectors

**/
INT32
EFIAPI
RPMBWriteBlocks (
   EFI_SDCC_RPMB_PROTOCOL       *This,
   UINT32                       *CmdPktBuffer,
   UINT32                       PktCount,
   UINT32                       *RespPktBuffer
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   INT32 RpmbStatus = RPMB_ERR_GENERAL;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == CmdPktBuffer) || (NULL == RespPktBuffer) ||
      (NULL == This) || (PktCount == 0))
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBWriteBlocksExit;
   }

   if (PktCount > SDCC_MAX_BLOCK_TRANSFER_SIZE)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBWriteBlocksExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_RPMB (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      RpmbStatus = RPMB_ERR_INVALID_PARAM;
      goto RPMBWriteBlocksExit;
   }

   rc = rpmb_write_sectors(sdcc_handle, CmdPktBuffer, PktCount,
                           RPMB_DATA_PKT, RespPktBuffer);

   if (SDCC_NO_ERROR != rc)
   {
      MMCHSDecodeErrorCode (rc);
      RpmbStatus = RPMB_ERR_WRITE;
      goto RPMBWriteBlocksExit;
   }

   RpmbStatus = RPMB_NO_ERROR;

RPMBWriteBlocksExit:
   gBS->RestoreTPL (PrevTPL);   

   return RpmbStatus;
}


/**
  Function: EmmcWriteProtectGetDeviceCapabilities

  Returns information about the eMMC write protection capabilities

  @param This             A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param Capabilities     Capabilities of the MMC device.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
*/
EFI_STATUS EFIAPI EmmcWriteProtectGetDeviceCapabilities (
   IN EFI_EMMC_WP_PROTOCOL *This,
   OUT EFI_EMMC_WP_CAPABILITIES *Capabilities
)
{
   SDCC_STATUS rc;
   MMCHS_DEV   *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == Capabilities) || (NULL == This))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcWriteProtectGetDeviceCapabilitiesExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_WRITE_PROTECT (This);

   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcWriteProtectGetDeviceCapabilitiesExit;
   }

   /* Get eMMC Card Info */
   rc = sdcc_handle_mem_get_device_info(sdcc_handle, &gCardInfo);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n\n\n", rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcWriteProtectGetDeviceCapabilitiesExit;
   }

   Capabilities->WriteProtectGroupEnable = gCardInfo.write_prot_support;
   Capabilities->HardwareResetEnabled = gCardInfo.hw_reset_function;
   Capabilities->WriteProtectGroupSize = gCardInfo.write_protect_group_size_in_sectors;

   EfiStatus = EFI_SUCCESS;

EmmcWriteProtectGetDeviceCapabilitiesExit:
   gBS->RestoreTPL (PrevTPL);
   
   return EfiStatus;
}

/**
  Function: EmmcSetWriteProtect

  This function applies write protection to the range specified.
  Both the SectorStart and SectorCount must align to the WriteProtectGroupSize
  (as calculated for GetDeviceCapabilities). If either is not aligned,
  EFI_INVALID_PARAMETER is returned. If the parameters are valid,
  this function sends the commands to the eMMC part to apply "Power-On" write
  protection to the range. This interface only supports the "Power-On" write protect mode.

  @param This         A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param SectorStart  The start sector for the write protect operation
  @param SectorCount  The count of sectors to apply power-on write protection to.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error applying
                                 the write protect commands.
*/
EFI_STATUS EFIAPI EmmcSetWriteProtect (
   IN EFI_EMMC_WP_PROTOCOL *This,
   IN UINT32 SectorStart,
   IN UINT32 SectorCount
)
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device  *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if (NULL == This)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSetWriteProtectExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_WRITE_PROTECT (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSetWriteProtectExit;   
   }

   rc = sdcc_handle_set_power_on_write_protection(sdcc_handle, SectorStart, SectorCount);

   if (SDCC_ERR_INVALID_PARAM == rc)
   {
      DEBUG ((EFI_D_ERROR, "Write Protect:: Invalid Parameter. %x\n\n\n", rc));
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSetWriteProtectExit;
   }

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Failed to write protect. Sector = %x. "
                           "Error = %x\n\n\n", SectorStart, rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcSetWriteProtectExit;
   }

   EfiStatus = EFI_SUCCESS;

EmmcSetWriteProtectExit:
   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/**
  Function: EmmcSendWriteProtectType

  Returns the type of protection for the group containing the sector specified by "Sector".
  CMD31 (SEND_WRITE_PROT_TYPE) is used to get the protection type for the write protect groups,
  then mask off the two LSBs that indicates the protection type for the starting group specified
  by the Sector address. See Table 26 in "JEDEC Standard No. 84-A441" for details.

  @param This         A pointer to the EFI_EMMC_WP_PROTOCOL instance.
  @param Sector       A sector address to the write protect group.
                      Must be aligned to the write protect group size.
  @param ProtectType  The type of protect applied.

  @return EFI_SUCCESS             The function returned successfully.
  @return EFI_INVALID_PARAMETER   A parameter is invalid.
  @return EFI_DEVICE_ERROR        The physical device reported an error.
*/
EFI_STATUS EFIAPI EmmcSendWriteProtectType (
   IN EFI_EMMC_WP_PROTOCOL *This,
   IN UINT32 Sector,
   OUT EFI_EMMC_WP_TYPE *ProtectType
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   UINT8 write_prot_type[32];
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == This) || (NULL == ProtectType))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSendWriteProtectTypeExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_WRITE_PROTECT (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSendWriteProtectTypeExit;
   }

   SetMem(write_prot_type, 8, 0x00);
   rc = sdcc_handle_get_write_protection_type (sdcc_handle, Sector,
                                               write_prot_type);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Failed to write protect Sector = %x. "
                           "Error = %x\n\n\n", Sector, rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcSendWriteProtectTypeExit;
   }

   /* The 2 Least Significant Bits are the Write Protection type for the
   protection group specified by the starting address */
   *ProtectType = (EFI_EMMC_WP_TYPE)(write_prot_type[7] & 0x03);

   EfiStatus = EFI_SUCCESS;

EmmcSendWriteProtectTypeExit:
   gBS->RestoreTPL (PrevTPL);   
   
   return EfiStatus;
}

/**
  Function: EmmcEraseBlocks

  This function erases the eMMC sectors specified by LBA and
  Size. Both the LBA and Size must align to the 
  EraseLengthGranularity. If either is not aligned, 
  EFI_INVALID_PARAMETER is returned. If the parameters are 
  valid, this function sends the commands to the eMMC part to 
  apply ERASE erase to the specified sectors. 

  @param This         A pointer to the calling context. 
  @param MediaId      Current media ID 
  @param LBA          The start sector for the erase operation 
  @param Token        Pointer to the token associated with the 
                      transaction
  @param Size         The number of bytes to apply erase 

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcEraseBlocks (
   IN EFI_BLOCK_IO_PROTOCOL *This,
   IN UINT32 MediaId,
   IN EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN UINTN Size
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device  *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;
   UINT32 blocks = 0; 

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == This) || (0 == Size))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcEraseBlocksExit;
   }

   /* Validate block size and access range */
   if ((LBA > This->Media->LastBlock) || (This->Media->BlockSize == 0)) {
      return EFI_INVALID_PARAMETER;
   }

   if (This->Media->MediaId != MediaId) {
     return EFI_MEDIA_CHANGED;
   }

   MmchsDevice = MMCHS_DEV_FROM_BLOCKIO (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcEraseBlocksExit;   
   }

   blocks = Size / (This->Media->BlockSize);

   rc = sdcc_handle_erase(sdcc_handle, LBA, blocks);
   if (SDCC_ERR_INVALID_PARAM == rc)
   {
      DEBUG ((EFI_D_ERROR, "Emmc Erase: Invalid Parameter. SectorStart = 0x%x"
             "Count = 0x%x, handle = 0x%x\n", LBA, blocks, sdcc_handle));
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcEraseBlocksExit;
   }

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Failed to Erase. SectorStart = 0x%x. "
             "Count = 0x%x. Error = 0x%x\n\n", LBA, blocks, rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcEraseBlocksExit;
   }

   EfiStatus = EFI_SUCCESS;

EmmcEraseBlocksExit:
   if (Token != NULL) 
   {
      Token->Event = NULL;
      Token->TransactionStatus = EfiStatus;
   }

   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/*
  Function: EmmcSecureEraseGetDeviceInfo

  This function retrieves the eMMC's info needed to perform the secure erase operation

  @param This              A pointer to the EFI_EMMC_SECURE_ERASE_PROTOCOL instance.
  @param EraseDeviceInfo   eMMC's erase info such as the EraseGroupSize

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcSecureEraseGetDeviceInfo (
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This,
   OUT EFI_EMMC_SECURE_ERASE_DEVICE_INFO *EraseDeviceInfo
   )
{
   SDCC_STATUS status;
   MMCHS_DEV   *MmchsDevice;
   struct sdcc_device *sdcc_handle = NULL;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if ((NULL == EraseDeviceInfo) || (NULL == This))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSecureEraseGetDeviceInfoExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_SECURE_ERASE (This);

   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSecureEraseGetDeviceInfoExit;
   }

   /* Get eMMC Card Info */
   status = sdcc_handle_mem_get_device_info(sdcc_handle, &gCardInfo);

   if (SDCC_NO_ERROR != status)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n", status));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcSecureEraseGetDeviceInfoExit;
   }

   EraseDeviceInfo->EraseGroupSize = gCardInfo.erase_group_size_in_sectors;

   EfiStatus = EFI_SUCCESS;

EmmcSecureEraseGetDeviceInfoExit:
   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/**
  Function: EmmcSecureErase

  This function erases the eMMC sectors specified by SectorStart and SectorCount.
  Both the SectorStart and SectorCount must align to the EraseGroupSize
  If either is not aligned, EFI_INVALID_PARAMETER is returned.
  If the parameters are valid, this function sends the commands to the eMMC part to
  apply ERASE erase to the specified sectors.

  @param This         A pointer to the EFI_EMMC_ERASE_PROTOCOL instance.
  @param SectorStart  The start sector for the erase operation
  @param SectorCount  The number of sectors to apply ERASE erase.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI EmmcSecureEraseBlocks (
   IN EFI_EMMC_SECURE_ERASE_PROTOCOL *This,
   IN UINT32 SectorStart,
   IN UINT32 SectorCount
   )
{
   MMCHS_DEV  *MmchsDevice;
   struct sdcc_device  *sdcc_handle = NULL;
   SDCC_STATUS  rc = SDCC_NO_ERROR;
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if (NULL == This)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSecureEraseBlocksExit;
   }

   MmchsDevice = MMCHS_DEV_FROM_SECURE_ERASE (This);
   sdcc_handle = MmchsDevice->DeviceHandle;

   if (NULL == sdcc_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSecureEraseBlocksExit;
   }

   rc = sdcc_handle_secure_erase(sdcc_handle, SectorStart, SectorCount);

   if (SDCC_ERR_INVALID_PARAM == rc)
   {
      DEBUG ((EFI_D_ERROR, "Emmc Secure Erase:: Invalid Parameter. SectorStart = 0x%x"
             "Count = 0x%x, handle = 0x%x\n", SectorStart, SectorCount, sdcc_handle));
      EfiStatus = EFI_INVALID_PARAMETER;
      goto EmmcSecureEraseBlocksExit;
   }

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Failed to Secure Erase. SectorStart = 0x%x. "
             "Count = 0x%x. Error = 0x%x\n\n", SectorStart, SectorCount, rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto EmmcSecureEraseBlocksExit;
   }

   EfiStatus = EFI_SUCCESS;

EmmcSecureEraseBlocksExit:
   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/**
  Function: SdccEnableSdhciMode

  This function enables the Host Controller specification

  @param This      A pointer to the EFI_SDCC_CONFIG_PROTOCOL instance.

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid.
**/
EFI_STATUS EFIAPI SdccEnableSdhciMode (
   IN EFI_SDCC_CONFIG_PROTOCOL *This
   )
{
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(SDCC_TPL);

   if (NULL == This)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto SdccEnableSdhciModeExit;
   }

   (void)sdcc_sdhci_mode_enable();

   EfiStatus = EFI_SUCCESS;

SdccEnableSdhciModeExit:   
   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/**
  Function: SdCardDetectionHandler

  This function detects the external SD card and installs the BlockIO protocol
  for the external SD card if the card is inserted.
  This function is called when DetectSdCardEvent is signaled.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.
**/
VOID
EFIAPI
SdCardDetectionHandler (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
   EFI_STATUS  status;
   EFI_HANDLE  SdTempHandle = NULL;

   if (NULL != MmchsDeviceSd.DeviceHandle)
   {
      DEBUG ((EFI_D_INFO, "SD card has already been initialized \n"));
      return;
   }

   status = DetectCard(&MmchsDeviceSd, SDCC_SD_PHYSICAL_PARTITION_USER);

   if (EFI_SUCCESS != status)
   {
      DEBUG ((EFI_D_INFO, "Detect external SD card failed \n"));
      return;
   }

   /* Publish BlockIO protocol for SD card */
   status = gBS->InstallMultipleProtocolInterfaces (
                        &SdTempHandle,
                        &gEfiBlockIoProtocolGuid, &MmchsDeviceSd.BlkIo,
                        &gEfiDevicePathProtocolGuid, &gSdDevicePath,
                        &gEfiMemCardInfoProtocolGuid, &MmchsDeviceSd.CardInfo,
                        NULL
                        );

   ASSERT_EFI_ERROR(status);

   /* Save the SD's Handle */
   MmchsDeviceSd.ClientHandle = SdTempHandle;

   if (SdDetectEventCreated)
   {
      gBS->CloseEvent(DetectSdCardEvent);
      SdDetectEventCreated = FALSE;
   }

   return;
}

/**
   Function: DetectCard

   This function checks if a card is presence. Upon initial detection,
   it opens communication with the card by sdcc_handle_open. It
   saves the returned Handle in the global variable MMCHSDevice
   and sets the MediaPresent flag for future use

   @param  MMCHSDevice            Pointer to a MMCHS_DEV variable
   @param  partition_num          Physical partition number

   @retval EFI_SUCCESS            The card dection completed successfully.
   @retval EFI_NO_MEDIA           There is no memory card
   @retval EFI_DEVICE_ERROR       A memory card is detected, but there're errors
                                  when trying to access it.
**/
EFI_STATUS DetectCard (
   IN MMCHS_DEV  *MMCHSDevice,
   UINT32        partition_num
   )
{
   SDCC_STATUS rc;
   UINT32 driveno;
   struct sdcc_device *sdcc_handle;
   UINT32 partition_size = 0;

   sdcc_handle = MMCHSDevice->DeviceHandle;
   driveno = MMCHSDevice->SlotNumber;

   if (SDCC_EMMC_MAX_PARTITION_ACCESS <= partition_num)
   {
      return EFI_INVALID_PARAMETER;
   }

   if (NULL != sdcc_handle)
   {
      return EFI_SUCCESS;
   }

   sdcc_handle = sdcc_handle_open(driveno, partition_num);

   if (NULL == sdcc_handle)
   {
      if (driveno == PcdGet32(SDCCExtSlotNumber))
      {
         DEBUG ((EFI_D_INFO, "External SD card detection failed, "
                              "possibly due to external card not presence\n"));
      }
      else
      {
         DEBUG ((EFI_D_ERROR, "Card Detection FAILED! Handle %x, Slot %x\n",
                 sdcc_handle, driveno));
      }

      MMCHSDevice->DeviceHandle = NULL;

      /* RPMB partition does not implement BlockIO protocol */
      if (SDCC_EMMC_PHY_PARTITION_RPMB != partition_num)
      {
         MMCHSDevice->BlkIo.Media->MediaPresent = FALSE;
      }

      return EFI_NO_MEDIA;
   }

   if (SDCC_EMMC_PHY_PARTITION_USER_AREA == partition_num)
   {
      partition_size = sdcc_handle->hdev->mem.mem_info.card_size_in_sectors;
   }
   else if ((SDCC_EMMC_PHY_PARTITION_BOOT_PART_1 == partition_num) ||
            (SDCC_EMMC_PHY_PARTITION_BOOT_PART_2 == partition_num))
   {
      partition_size = sdcc_handle->hdev->mem.partition_boot_size;
   }
   else if (SDCC_EMMC_PHY_PARTITION_RPMB == partition_num)
   {
      partition_size = sdcc_handle->hdev->mem.mem_info.rpmb_parti_size_in_byte;
   }
   else if ((SDCC_EMMC_PHY_PARTITION_GPP_1 <= partition_num) &&
            (SDCC_EMMC_PHY_PARTITION_GPP_4 >= partition_num))
   {
      partition_size = sdcc_handle->hdev->mem.partition_gp_size[partition_num - 4];
   }

   if (0 == partition_size)
   {
      DEBUG((EFI_D_INFO, "Partition %d card size = 0\n", partition_num));

      MMCHSDevice->DeviceHandle = NULL;

      /* RPMB partition does not implement BlockIO protocol */
      if (SDCC_EMMC_PHY_PARTITION_RPMB != partition_num)
      {
         MMCHSDevice->BlkIo.Media->MediaPresent = FALSE;
      }

      return EFI_NO_MEDIA;
   }

   /* Get Card Info */
   rc = sdcc_handle_mem_get_device_info(sdcc_handle, &gCardInfo);

   if (SDCC_NO_ERROR != rc)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED! Status %x. Slot %x\n\n\n",
              rc, driveno));
      return EFI_DEVICE_ERROR;
   }

   MMCHSDevice->DeviceHandle = sdcc_handle;

   if (SDCC_EMMC_PHY_PARTITION_RPMB != partition_num)
   {
      MMCHSDevice->BlkIo.Media->MediaPresent = TRUE;
      MMCHSDevice->BlkIo.Media->LastBlock    = (gCardInfo.card_size_in_sectors - 1);
      MMCHSDevice->BlkIo.Media->BlockSize    = gCardInfo.block_len;
      MMCHSDevice->BlkIo.Media->ReadOnly     = 0;
      MMCHSDevice->BlkIo.Media->MediaPresent = TRUE;
   }
   else
   {
      MMCHSDevice->SdccRPMB.ReliableWriteCount =
         gCardInfo.reliable_write_sector_count;

      /* For RPMB, reliable_write_sector_count = 1 supports either
         256 bytes (1 512 byte frame) or 512 bytes (2 512 byte frames). Use
         the larger value of the two for better performance. */
      if (gCardInfo.reliable_write_sector_count == 1)
      {
         MMCHSDevice->SdccRPMB.ReliableWriteCount = 2; 
      }

      MMCHSDevice->SdccRPMB.RPMBPartitionSizeInBytes =
         gCardInfo.rpmb_parti_size_in_byte;
      MMCHSDevice->SdccRPMB.RPMBMaxTransferSize =
         SDCC_MAX_BLOCK_TRANSFER_SIZE;
   }

   if (FixedPcdGetBool(SDCCEmmcHwResetEnabled))
   {
      if ((SDCC_EMMC_PHY_PARTITION_USER_AREA == partition_num) &&
          (MMCHSDevice->BlkIo.Media->RemovableMedia == FALSE))
      {
        /* The MSM Reset Output signal is tied to the eMMC Reset pin */
        /* Having eMMC's HW Reset Enable, the MSM Reset signal will also reset the
        eMMC. */
        /* This setting will not take effect until a reset or a power cycle event
        occurs. */
        /* This is a one time irreversible setting. Once set, it remains set */
        /* As a result of this setting, the eMMC's internal write protection bits */
        /* are clear upon having a reset or power cycle event*/
         if ( gCardInfo.hw_reset_function == 0)
         {
            rc = sdcc_enable_hw_reset(sdcc_handle);

            if (SDCC_NO_ERROR != rc)
            {
               DEBUG ((EFI_D_ERROR, "Failed to enable Hw Reset \n"));
               return EFI_DEVICE_ERROR;
            }
         }
      }
   }

   DEBUG ((EFI_D_INFO, "\n Detect Card SUCCEEDED!! Slot %x\n\n\n", driveno));

   return EFI_SUCCESS;
}

VOID
EFIAPI
MMCHSStall (
   IN  UINT32   us
   )
{
   gBS->Stall(us);
}

VOID
EFIAPI
RegisterListener (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
   EFI_STATUS Status;

   Status = RPMBListenerInit((UINT32) Context); 
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize RPMB Listener, Status 0x%08x\n", Status));
   }

   Status = GPTListenerInit((UINT32) Context);
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize GPT Listener, Status 0x%08x\n", Status));
   }

   Status = InitPartitionConfig();
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize Partition config, Status 0x%08x\n", Status));
   }
}

VOID
EFIAPI
SdccDxeExitBSCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
   (void) sdcc_sdhci_mode_enable(); 
}

/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It convers pointer to new virtual address.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
VariableClassAddressChangeEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
   UINTN partition_num;
   UINTN driveno;
   UINTN maxslot;

   /*
    * Initilize the HWIO address table for access to hwio regs using the
    * drive number as the index:
    */
   maxslot = (UINTN) sdcc_bsp_get_slot_count();
   for (driveno = 0; driveno < maxslot; driveno++)
   {
      if (sdcc_bsp_get_slot_access_type(driveno) != SDCC_SLOT_INVALID)
      {
         EfiConvertPointer (0x0, (VOID **) &sdcc_reg_hc[driveno]);
      }
   }

   /*
    * Convert MmchesDevice*
    */
   if (NULL != hHandleSd)
   {
      if (NULL != hHandleSd->hdev)
      {
         EfiConvertPointer (0x0, (VOID **) &(hHandleSd->hdev->pslot));
      }
      EfiConvertPointer (0x0, (VOID **) &(hHandleSd->hdev));
   }
   EfiConvertPointer (0x0, (VOID **) &(hHandleSd));
   EfiConvertPointer (0x0, (VOID **) &(MmchsDeviceSd.ClientHandle));
   if (NULL != (struct sdcc_device *)MmchsDeviceSd.DeviceHandle)
   {
     if (NULL != (((struct sdcc_device *)MmchsDeviceSd.DeviceHandle)->hdev))
     {
        EfiConvertPointer (0x0, (VOID **) &(((struct sdcc_device *)MmchsDeviceSd.DeviceHandle)->hdev->pslot));
     }
     EfiConvertPointer (0x0, (VOID **) &(((struct sdcc_device *)MmchsDeviceSd.DeviceHandle)->hdev));
   }
   EfiConvertPointer (0x0, (VOID **) &(MmchsDeviceSd.DeviceHandle));


   for (partition_num = 0; partition_num<SDCC_EMMC_MAX_PARTITION_ACCESS; partition_num++)
   {
      if (NULL != (struct sdcc_device *)MmchsDeviceEmmc[partition_num].DeviceHandle)
      {
         if (NULL != ((struct sdcc_device *)MmchsDeviceEmmc[partition_num].DeviceHandle)->hdev)
         {
            EfiConvertPointer (0x0, (VOID **) &(((struct sdcc_device *)MmchsDeviceEmmc[partition_num].DeviceHandle)->hdev->pslot));
         }
         EfiConvertPointer (0x0, (VOID **) &(((struct sdcc_device *)MmchsDeviceEmmc[partition_num].DeviceHandle)->hdev));
      }
      EfiConvertPointer (0x0, (VOID **) &(MmchsDeviceEmmc[partition_num].DeviceHandle));
      EfiConvertPointer (0x0, (VOID **) &(MmchsDeviceEmmc[partition_num].ClientHandle));
   }

}

EFI_STATUS
EFIAPI
SdccDxeEntryPoint (
   IN EFI_HANDLE         ImageHandle,
   IN EFI_SYSTEM_TABLE   *SystemTable
   )
{
   EFI_HANDLE SdTempHandle;
   EFI_STATUS Status;
   UINT32 partition_num = 0;

   ZeroMem (&gCardInfo, sizeof (gCardInfo));
   
   /* Skip eMMC BlockIO handles if not booting from eMMC */
   if (!boot_from_emmc ())
   {       
       goto Install_SD_Handle;
   }

   /* Create the RegisterListener callback */
   Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL,
                             TPL_CALLBACK,
                             RegisterListener,
                             (VOID *)SIGNATURE_32('e','m','m','c'),
                             &ScmInitialized
                             );
   ASSERT_EFI_ERROR (Status);

   Status = gBS->RegisterProtocolNotify(&gQcomScmProtocolGuid,
                                        ScmInitialized,
                                        (VOID *)&ScmInitializedToken);

   ASSERT_EFI_ERROR(Status);
   /* Make sure install eMMC handle before SD card */
   /* External UEFI applications depend on this order */
   for (partition_num = 0; partition_num < SDCC_EMMC_MAX_PARTITION_ACCESS; partition_num++)
   {
      MmchsDeviceEmmc[partition_num].Signature    = MMCHS_DEV_SIGNATURE;
      MmchsDeviceEmmc[partition_num].SlotNumber   = PcdGet32(SDCCEmmcSlotNumber);
      MmchsDeviceEmmc[partition_num].DeviceHandle = hHandleEmmc;
      MmchsDeviceEmmc[partition_num].CardInfo     = gEmmcCardInfoProtocol;

      if (SDCC_EMMC_PHY_PARTITION_RPMB != partition_num)
      {
         MmchsDeviceEmmc[partition_num].BlkIo       = gBlockIoEmmc;
         MmchsDeviceEmmc[partition_num].BlkIo2      = gBlockIo2Emmc;
         MmchsDeviceEmmc[partition_num].BlkIo.Media = &gMediaEmmc[partition_num];
         MmchsDeviceEmmc[partition_num].BlkIo2.Media = &gMediaEmmc[partition_num];
         CopyMem(MmchsDeviceEmmc[partition_num].BlkIo.Media,
                 &EmmcMedia, sizeof(EFI_BLOCK_IO_MEDIA));

         MmchsDeviceEmmc[partition_num].EmmcWriteProtect = gEmmcWriteProtectProtocol;

         if (SDCC_EMMC_PHY_PARTITION_USER_AREA == partition_num)
         {
            MmchsDeviceEmmc[partition_num].EmmcErase         = gEmmcEraseProtocol;
            MmchsDeviceEmmc[partition_num].SdccConfig        = gSdccConfigProtocol;
         }
      }
      else
      {
         MmchsDeviceEmmc[partition_num].SdccRPMB     = gSdccRpmbProtocol;
      }

      gMmcHsDevicePath[partition_num].Mmc = MmcPath;
      gMmcHsDevicePath[partition_num].Mmc.Guid = EmmcDevicePathGuids[partition_num];
      gMmcHsDevicePath[partition_num].End = EndPath;

      /* eMMC is non-removable. Detect eMMC card once at entry point.
         SD card is removable and SD card presence is polled on Reads */
      Status = DetectCard(&MmchsDeviceEmmc[partition_num], partition_num);

      if (EFI_SUCCESS == Status)
      {
         if (SDCC_EMMC_PHY_PARTITION_USER_AREA == partition_num)
         {
            /* User partition supports the following protocols: */
            /* BlkIO, CardInfo, EmmcWriteProtect, EmmcErase */
            /* Note: Only User partition supports EmmcErase protocol */
            Status = gBS->InstallMultipleProtocolInterfaces (
                        &MmchsDeviceEmmc[partition_num].ClientHandle,
                        &gEfiBlockIoProtocolGuid, &MmchsDeviceEmmc[partition_num].BlkIo,
                        &gEfiBlockIo2ProtocolGuid, &MmchsDeviceEmmc[partition_num].BlkIo2,
                        &gEfiDevicePathProtocolGuid, &gMmcHsDevicePath[partition_num],
                        &gEfiMemCardInfoProtocolGuid, &MmchsDeviceEmmc[partition_num].CardInfo,
                        &gEfiEmmcWpProtocolGuid, &MmchsDeviceEmmc[partition_num].EmmcWriteProtect,
                        &gEfiEraseBlockProtocolGuid, &MmchsDeviceEmmc[partition_num].EmmcErase,
                        &gEfiSdccConfigProtocolGuid, &MmchsDeviceEmmc[partition_num].SdccConfig,
                        NULL
                        );
         }
         else if (SDCC_EMMC_PHY_PARTITION_RPMB == partition_num)
         {
            /* RPMB partition supports: SdccRPMB, CardInfo protocols */
            /* Note: only RPMB partition supports SdccRPMB protocol */
            Status = gBS->InstallMultipleProtocolInterfaces (
                        &MmchsDeviceEmmc[partition_num].ClientHandle,
                        &gEfiDevicePathProtocolGuid, &gMmcHsDevicePath[partition_num],
                        &gEfiMemCardInfoProtocolGuid, &MmchsDeviceEmmc[partition_num].CardInfo,
                        &gEfiSdccRpmbProtocolGuid, &MmchsDeviceEmmc[partition_num].SdccRPMB,
                        NULL
                        );
         }
         else
         {
            /* GPP partitions supports: BlockIo, CardInfo, EmmcWriteProtect protocols */
            Status = gBS->InstallMultipleProtocolInterfaces (
                        &MmchsDeviceEmmc[partition_num].ClientHandle,
                        &gEfiBlockIoProtocolGuid, &MmchsDeviceEmmc[partition_num].BlkIo,
                        &gEfiBlockIo2ProtocolGuid, &MmchsDeviceEmmc[partition_num].BlkIo2,
                        &gEfiDevicePathProtocolGuid, &gMmcHsDevicePath[partition_num],
                        &gEfiMemCardInfoProtocolGuid, &MmchsDeviceEmmc[partition_num].CardInfo,
                        &gEfiEmmcWpProtocolGuid, &MmchsDeviceEmmc[partition_num].EmmcWriteProtect,
                        NULL
                        );
         }

         ASSERT_EFI_ERROR(Status);

      }
   }
   
   /* Allocate memory for RPMB listener */
   Status = RPMBListenerAllocMem();
   if(Status != EFI_SUCCESS)
   {
      DEBUG ((EFI_D_ERROR, "Failed to allocate memory for RPMB listener\n"));
      return Status;
   }

   /* Allocate memory for GPT listener */
   Status = GPTListenerAllocMem();
   if(Status != EFI_SUCCESS)
   {
      DEBUG ((EFI_D_ERROR, "Failed to allocate memory for GPT listener\n"));
      return Status;
   }

   EmmcDriveNo = FixedPcdGetBool(SDCCEmmcSlotNumber);
   
   MMCHS_TransferInfo.BlkIO2Initialized = TRUE;
   
   /*Initialize request list for Block IO 2*/	
   Status = BlkIo2ReqListInit(&EmmcReqList, REQ_LIST_SIZE);

   if(Status == EFI_SUCCESS)
   {
      /*Create Internal Event for Block IO 2*/
      Status = gBS->CreateEvent (
                       EVT_TIMER | EVT_NOTIFY_SIGNAL,
                       SDCC_TPL,
                       BlkIo2TimerEventNotify,
                       NULL,
                       &BlkIo2InternalEvent
                       );
      
      if(EFI_ERROR (Status)) 
      {
         MMCHS_TransferInfo.BlkIO2Initialized = FALSE;
         DEBUG ((EFI_D_WARN, "Block IO 2 timer creation failed, cannot use Block IO 2.\n"));
      }
   }   
   else 
   {
      MMCHS_TransferInfo.BlkIO2Initialized = FALSE;
      DEBUG ((EFI_D_WARN, "Initialization failed, cannot use Block IO 2.\n"));
   }

   Status = EFI_SUCCESS;
   
Install_SD_Handle:
   
   MmchsDeviceSd.Signature      = MMCHS_DEV_SIGNATURE;
   MmchsDeviceSd.SlotNumber     = PcdGet32(SDCCExtSlotNumber);
   MmchsDeviceSd.DeviceHandle   = hHandleSd;
   MmchsDeviceSd.BlkIo          = gBlockIoSd;
   MmchsDeviceSd.CardInfo       = gSdCardInfoProtocol;
   MmchsDeviceSd.SdccConfig     = gSdccConfigProtocol;

   if (FixedPcdGetBool(SDCCEventBasedSDDetectEnabled))
   {
      /* Create event to detect external SD card */
      Status = gBS->CreateEventEx (
                     EVT_NOTIFY_SIGNAL,
                     TPL_CALLBACK,
                     SdCardDetectionHandler,
                     NULL,
                     &gEfiEventDetectSdCardGuid,
                     &DetectSdCardEvent
                     );

      ASSERT_EFI_ERROR (Status);

      SdDetectEventCreated = TRUE;

      /* Register for Exit Boot Service Event */
      Status = gBS->CreateEventEx (EVT_NOTIFY_SIGNAL,
                                   TPL_CALLBACK,
                                   SdccDxeExitBSCallback,
                                   NULL,
                                   &gEfiEventExitBootServicesGuid,
                                   &ExitBootServicesEvent);

      ASSERT_EFI_ERROR (Status);
   }
   else
   {
      /* if Handle is NULL, a new handle is to be allocated */
      SdTempHandle = NULL;

      /* Publish SD BlockIO. */
      Status = gBS->InstallMultipleProtocolInterfaces (
                           &SdTempHandle,
                           &gEfiBlockIoProtocolGuid, &MmchsDeviceSd.BlkIo,
                           &gEfiDevicePathProtocolGuid, &gSdDevicePath,
                           &gEfiMemCardInfoProtocolGuid, &MmchsDeviceSd.CardInfo,
                           NULL
                           );

      ASSERT_EFI_ERROR(Status);

      /* Save the SD's Handle */
      MmchsDeviceSd.ClientHandle = SdTempHandle;
   }

   Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  VariableClassAddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
   ASSERT_EFI_ERROR (Status);

   return EFI_SUCCESS;
}
