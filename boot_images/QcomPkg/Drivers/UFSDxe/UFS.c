/**
* @file     UFS.c
* @brief   Universal Flash Storage (UFS) Dxe Driver 
*
*  Copyright (c) 2013 - 2017, Qualcomm Technologies, Inc. All rights reserved.
*/


/*=============================================================================
                              EDIT HISTORY


when            who   what, where, why
----------      ---   -----------------------------------------------------------
2017-02-01      jt    Use wrapper functions for BlkIo 
2016-11-30      jt    Return all entries in UFSGetLUWriteProtectInfo
2016-09-21      jt    Secure write protect changes   
2016-09-20      jt    Correct serial number length  
2016-06-28      jt    Add get/set boot LUN APIs 
2016-01-26      jt    Card info changes 
2016-01-21      rh    Adding BlockIO2 
2015-10-19      jt    Pass MediaId as argument for GPTListenerInit
2015-05-13      jb    Change device path type name to not conflict with standard one
2015-04-03      rm    Add check for boot device
2015-03-26      rm    Add RPMB protocols  
10-09-2014      rh    Adding RPMB APIs
09-23-2014      rh    Use unique GUID for LUs, different from eMMC
05-05-2013      ai    Initial version

=============================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/ArmLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/StorSecApp.h>
#include <Protocol/BlockIo.h>
#include <Protocol/BlockIo2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/EFIClock.h>
#include <Protocol/EFIHWIO.h>

#include <Library/GPTListener.h>
#include <Library/RpmbListener.h>
#include <Library/RpmbLib.h>
#include <Protocol/EFIRpmb.h>
#include <Protocol/EFIEraseBlock.h>
#include <Protocol/EFIStorageWriteProtect.h>

#include "UfsBlockIO2.h"
#include "UFS.h"
#include <api/storage/ufs_api.h>

#include <BootConfig.h>

#define UFS_TPL TPL_CALLBACK

/* RPMB transfer limit */
#define RPMB_MAX_BLOCK_TRANSFER_SIZE 0xffff

/* UFS Device Path */
typedef struct {
   VENDOR_DEVICE_PATH  Ufs;
   EFI_DEVICE_PATH     End;
} QCUFS_DEVICE_PATH;

static QCUFS_DEVICE_PATH gUfsDevicePath [UFS_UEFI_MAX_LUN_ACCESS]; 

static VENDOR_DEVICE_PATH UfsPath = 
{ 
   HARDWARE_DEVICE_PATH,
   HW_VENDOR_DP,
   (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
   (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
   0 
};

static EFI_DEVICE_PATH UfsEndPath = 
{ 
   END_DEVICE_PATH_TYPE,
   END_ENTIRE_DEVICE_PATH_SUBTYPE,
   sizeof (EFI_DEVICE_PATH_PROTOCOL),
   0
}; 

#define _UFS_DEV_PROTOTYPE    { UFS_DEV_SIGNATURE, NULL, NULL, 0, 0, 0, 0, 0 }

static UFS_DEV gUfsDevice[UFS_UEFI_MAX_LUN_ACCESS] = {
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE,
   _UFS_DEV_PROTOTYPE
}; 

/* UFS Block IO's Media */
static EFI_BLOCK_IO_MEDIA UFSMedia = {
   SIGNATURE_32('u','f','s',' '),            /* MediaId */
   FALSE,                                    /* RemovableMedia */
   FALSE,                                    /* MediaPresent */
   FALSE,                                    /* LogicalPartition */
   FALSE,                                    /* ReadOnly */
   FALSE,                                    /* WriteCaching */
   4096,                                     /* BlockSize */
   8,                                        /* IoAlign */
   0,                                        /* LastBlock */
   0,                                        /* LowestAlignedLba */
   0,                                        /* LogicalBlocksPerPhysicalBlock */
   0                                         /* OptimalTransferLengthGranularity */  
}; 

static EFI_BLOCK_IO_MEDIA gMediaUfs[UFS_UEFI_MAX_LUN_ACCESS]; 

/* UFS Block IO Protocol */
EFI_BLOCK_IO_PROTOCOL gBlockIoUfs = {
   EFI_BLOCK_IO_PROTOCOL_REVISION3,          /* Revision */
   0,                                        /* Media */
   UFSReset,                                 /* Reset */
   UFSReadBlocksWrapper,                     /* ReadBlocks */
   UFSWriteBlocksWrapper,                    /* WriteBlocks */
   UFSFlushBlocks                            /* FlushBlocks */
};

/* UFS Block IO2 Protocol */
EFI_BLOCK_IO2_PROTOCOL gBlockIo2Ufs = {
   0,                                        /* Media */
   UFSResetEx,                               /* Reset */
   UFSReadBlocksEx,                          /* ReadBlocksEx */
   UFSWriteBlocksEx,                         /* WriteBlocksEx */
   UFSFlushBlocksEx                          /* FlushBlocksEx */
};

/* Device Paths for all the UFS LUNs */
static EFI_GUID UfsDevicePathGuids[UFS_UEFI_MAX_LUN_ACCESS] = {
   EFI_UFS_LUN_0_GUID,
   EFI_UFS_LUN_1_GUID,
   EFI_UFS_LUN_2_GUID,
   EFI_UFS_LUN_3_GUID,
   EFI_UFS_LUN_4_GUID,
   EFI_UFS_LUN_5_GUID,
   EFI_UFS_LUN_6_GUID,
   EFI_UFS_LUN_7_GUID
}; 

/* RPMB Protocol */
static const EFI_SDCC_RPMB_PROTOCOL gUfsRpmbProtocol = {
   EFI_RPMB_PROTOCOL_REVISION,               /* Revision */
   0,                                        /* Reliable Write Count */
   0,                                        /* RPMB Partition Size */
   RPMBReadCounterPkt,                       /* Read Counter*/ 
   RPMBProgProvisionKey,                     /* Program Provision Key */
   RPMBReadBlocks,                           /* Read Sectors */
   RPMBWriteBlocks,                          /* Write Sectors */
   0                                         /* Max RMPB Read/Write Transfer Size */
};

/* UFS Card Info Protocol */
static EFI_MEM_CARDINFO_PROTOCOL gUfsCardInfoProtocol = {
   EFI_MEM_CARD_INFO_PROTOCOL_REVISION,      /* Revision */
   UFSGetCardInfo,                           /* GetCardInfo */
   UFSGetActiveBootLU,                       /* GetBootLU */
   UFSSetActiveBootLU                        /* SetBootLU */
};

/* Storage Write Protect Protocol */
static EFI_STORAGE_WP_PROTOCOL gUfsWriteProtectProtocol = {
   EFI_STORAGE_WP_PROTOCOL_REVISION,      /* Revision */
   UFSGetSupportedLbaWriteProtectTypes,   /* GetSupportedLbaWriteProtectTypes */                      
   UFSSetLUWriteProtect,                  /* SetLUWriteProtect */    
   UFSSetLbaWriteProtect,                 /* SetLbaWriteProtect */
   UFSClearWriteProtect,                  /* ClearWriteProtect */
   UFSGetLUWriteProtectInfo               /* GetLUWriteProtectInfo */              
};

/* Erase Blocks Protocol */
static const EFI_ERASE_BLOCK_PROTOCOL gUfsEraseBlkProtocol = {
   EFI_ERASE_BLOCK_PROTOCOL_REVISION,        /* Revision */
   4096,                                     /* Erase Granularity */
   UFSEraseBlocks                            /* Erase Blocks */
};


static UFS_DEV gUfsRpmbDevice = _UFS_DEV_PROTOTYPE;
static UFS_DEV gUfsBootDevice = _UFS_DEV_PROTOTYPE; 

static QCUFS_DEVICE_PATH gUfsRpmbDevicePath;

/* SCM registration event */
static EFI_EVENT ScmInitialized = NULL; 
static VOID *ScmInitializedToken = NULL;
extern EFI_GUID gQcomScmProtocolGuid;

/** Connection to blockIO2 **/
#define  REQ_LIST_SIZE  128
EFI_EVENT         gUfsBlkIo2TimerEvent;
BLKIO2_REQ_LIST   gUfsReqList;
DRIVER_INFO       gUfsTransferInfo;
EFI_EVENT         gUfsPurgeTimerEvent;
EFI_BLOCK_IO_PROTOCOL *gUfsCurrentErase;

VOID PurgeTimerEventNotify (
            IN  EFI_EVENT  Event,
            IN  VOID      *Context);

EFI_STATUS UFSIoWrapper (
            IN VOID                     *pThis,
            IN UINT32                    MediaId,
            IN EFI_LBA                   Lba,
            IN OUT EFI_BLOCK_IO2_TOKEN  *Token,
            IN UINTN                     BufferSize,
            IN OUT VOID                 *Buffer);

/**
   Reset the Block Device.
**/
EFI_STATUS EFIAPI UFSReset (
            IN EFI_BLOCK_IO_PROTOCOL   *This,
            IN BOOLEAN                 ExtendedVerification)
{
   return EFI_SUCCESS; 
}

/**
   Return supported write protect types
**/
EFI_STATUS EFIAPI UFSGetSupportedLbaWriteProtectTypes(
   IN EFI_STORAGE_WP_PROTOCOL *This, 
   OUT UINT32 *LbaWpTypes
)
{
   INT32 rc; 
   UFS_DEV *UfsDevice;
   struct ufs_handle *hUFS;
   struct ufs_info_type info;
   if ((NULL == This) || (NULL == LbaWpTypes)) 
   {
      return EFI_INVALID_PARAMETER; 
   }

   UfsDevice = UFS_DEV_FROM_WRITE_PROTECT (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;   

   *LbaWpTypes = WP_NOT_PROTECTED; 

   /* Get device info to determine bLUWriteProtect value */
   rc = ufs_get_device_info (hUFS, &info);
   if (UFS_EOK != rc) 
   {
      DEBUG ((EFI_D_ERROR, "GetSupportedLbaWriteProtectTypes: Get device "
                           "info failed, status 0x%x\n", rc));
      return EFI_DEVICE_ERROR;
   }

   /* Set options based on bLUWriteProtect */
   if (2 == info.bLUWriteProtect)
   {
      *LbaWpTypes |= WP_PERMANENT; 
   }
   else if (1 == info.bLUWriteProtect) 
   {
      *LbaWpTypes |= WP_POWERON; 
   }
   else if (0 == info.bLUWriteProtect) 
   {
      *LbaWpTypes |= (WP_SECURE_NV | WP_SECURE_P | WP_SECURE_P_EN);
   }
   else
   {
      DEBUG ((EFI_D_ERROR, "GetSupportedLbaWriteProtectTypes: Invalid value "
                           "for bLUWriteProtect\n")); 
      return EFI_INVALID_PARAMETER; 
   }

   return EFI_SUCCESS; 
}

/**
   Set write protection for the entire LU
**/
EFI_STATUS EFIAPI UFSSetLUWriteProtect(
   IN EFI_STORAGE_WP_PROTOCOL *This
)
{
   INT32 rc; 
   UFS_DEV *UfsDevice;
   struct ufs_handle *hUFS;

   /* Validate passed-in pointers are not NULL */
   if (NULL == This)
   {
      return EFI_INVALID_PARAMETER;
   }

   UfsDevice = UFS_DEV_FROM_WRITE_PROTECT (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       

   /* Set fPowerOnWPEn field in the flags */
   rc = ufs_set_pon_write_protect(hUFS); 
   if (UFS_EOK != rc) 
   {
      DEBUG ((EFI_D_ERROR, "ufs_set_pon_write_protect with return value of %d\n", rc));
      return EFI_DEVICE_ERROR;
   }

   return EFI_SUCCESS; 
}

/**
   Get write protection information for the LU specified
**/
EFI_STATUS EFIAPI UFSGetLUWriteProtectInfo(
   IN EFI_STORAGE_WP_PROTOCOL *This, 
   OUT EFI_WP_CONFIG *ProtectInfo
)
{
   EFI_STATUS Status; 
   INT32 rc; 
   SdMgrSecureWpInfo WpInfo; 
   UFS_DEV *UfsDevice;
   struct ufs_handle *hUFS;
   struct ufs_info_type info;
   UINT8 i = 0, index = 0; 

   if ((NULL == This) || (NULL == ProtectInfo))
   {
      return EFI_INVALID_PARAMETER; 
   }

   UfsDevice = UFS_DEV_FROM_WRITE_PROTECT (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;  

   /* Get device info to determine bLUWriteProtect value */
   rc = ufs_get_device_info (hUFS, &info);
   if (UFS_EOK != rc) 
   {
      DEBUG ((EFI_D_ERROR, "GetLbaWriteProtectType: Get device info failed, status 0x%x\n", rc));
      return EFI_DEVICE_ERROR;
   }

   /* If fPermanentWPEn = 1, then permanent write protection will be enabled for
      those LUNs where bLUWriteProtect = 2, regardless of LBA */
   if (2 == info.bLUWriteProtect)
   {
      if (info.fPermanentWPEn) 
      {
         ProtectInfo->NumEntries = 1;
         ProtectInfo->WpEntries[0].WpType = WP_PERMANENT; 
         ProtectInfo->WpEntries[0].Lba = 0; 
         ProtectInfo->WpEntries[0].NumBlocks = info.dLuTotalBlocks;  
         return EFI_SUCCESS; 
      }
   }
   /* If fPowerOnWPEn = 1, then power on write protection will be enabled for
      those LUNs where bLUWriteProtect = 1, regardless of LBA */
   else if (1 == info.bLUWriteProtect)
   {
      if (info.fPowerOnWPEn) 
      {
         ProtectInfo->NumEntries = 1;
         ProtectInfo->WpEntries[0].WpType = WP_POWERON;  
         ProtectInfo->WpEntries[0].Lba = 0; 
         ProtectInfo->WpEntries[0].NumBlocks = info.dLuTotalBlocks; 
         return EFI_SUCCESS; 
      }
   }
   /* If bLUWriteProtect = 0, then check to see if the LBA is part of a secure
      write protect configuration block */
   else if (0 == info.bLUWriteProtect) 
   {
      for (i = 0; i < UFS_UEFI_MAX_LUN_ACCESS; i++) 
      {
         if (UfsDevice == &gUfsDevice[i]) 
         {
            WpInfo.LunNumber = i; 
         }
      }

      ProtectInfo->NumEntries = 0; 

      /* Get Secure Write Protect Configuration Block */
      Status = UFSGetSecureWpConfigBlock(&WpInfo); 
      if (EFI_SUCCESS != Status)
      {
         DEBUG ((EFI_D_ERROR, "GetLbaWriteProtect: UFSGetSecureWpConfigBlockt failed "
                              "with return value of 0x%x\n", Status));
         return Status; 
      }

      for (i = 0; i < WpInfo.NumEntries; i++) 
      {
         ProtectInfo->NumEntries++; 
         ProtectInfo->WpEntries[index].Lba = WpInfo.WpEntries[i].Addr; 
         ProtectInfo->WpEntries[index].NumBlocks = WpInfo.WpEntries[i].NumBlocks; 

         /* NV-type when WPT = 0 */
         if (0 == WpInfo.WpEntries[i].WpTypeMask) 
         {
            ProtectInfo->WpEntries[index].WpType = WP_SECURE_NV; 
         }
         /* P-type when WPT = 1 */
         else if (1 == WpInfo.WpEntries[i].WpTypeMask) 
         {
            ProtectInfo->WpEntries[index].WpType = WP_SECURE_P; 
         }
         /* NV-AWP-type when WPT = 2 */
         else if (2 == WpInfo.WpEntries[i].WpTypeMask) 
         {
            ProtectInfo->WpEntries[index].WpType = WP_SECURE_P_EN;
         }

         index++; 
      }
   }
   else
   {
      DEBUG ((EFI_D_ERROR, "GetLbaWriteProtect: Invalid bLUWriteProtect "
                           "value 0x%x\n", info.bLUWriteProtect));  
      return EFI_INVALID_PARAMETER; 
   }
 
   return EFI_SUCCESS;  
}

/**
   Set write protection for the LBA ranges
**/
EFI_STATUS EFIAPI UFSSetLbaWriteProtect(
   IN EFI_STORAGE_WP_PROTOCOL *This,      
   IN EFI_WP_CONFIG *WpConfig
)
{
   EFI_STATUS Status; 
   INT32 rc; 
   SdMgrSecureWpInfo WpInfo;
   UFS_DEV *UfsDevice; 
   struct ufs_handle *hUFS;
   struct ufs_info_type info;
   UINT8 i;

   if ((NULL == This) || (NULL == WpConfig)) 
   {
      return EFI_DEVICE_ERROR; 
   }

   UfsDevice = UFS_DEV_FROM_WRITE_PROTECT (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;

   /* Get device info to determine bLUWriteProtect value */
   rc = ufs_get_device_info (hUFS, &info);
   if (UFS_EOK != rc) 
   {
      DEBUG ((EFI_D_ERROR, "SetLbaWriteProtect: Get device info failed, status 0x%x\n", rc));
      return EFI_DEVICE_ERROR;
   }
 
   if (0 != info.bLUWriteProtect)
   {
      DEBUG ((EFI_D_ERROR, "LBA write protect only allowed for bLUWriteProtect = 0\n", rc));
      return EFI_INVALID_PARAMETER; 
   }

   for (i = 0; i < UFS_UEFI_MAX_LUN_ACCESS; i++) 
   {
      if (UfsDevice == &gUfsDevice[i]) 
      {
         WpInfo.LunNumber = i; 
      }
   }
 
   Status = UFSGetSecureWpConfigBlock(&WpInfo);
   if (EFI_SUCCESS != Status) 
   {
      DEBUG ((EFI_D_ERROR, "UFSSetLbaWriteProtect: UFSGetSecureWpConfigBlock failed "
                           "with return value of 0x%x\n", Status));
      return Status; 
   }

   /* Once the configuration block is updated, we won't have NumEntries = 0.
      If we see that NumEntries = 1 and the first entry disabled, then
      we know that the ClearWriteProtect has happened and so we essentially
      have NumEntries = 0 */
   if ((WpInfo.NumEntries == 1) && (WpInfo.WpEntries[0].WpEnable == 0))
   {
      WpInfo.NumEntries = 0; 
   }

   if ((MAX_SECURE_WP_ENTRIES - WpInfo.NumEntries) < WpConfig->NumEntries) 
   {
      DEBUG ((EFI_D_ERROR, "Number of configuration entries too large\n"));
      return EFI_INVALID_PARAMETER; 
   }

   /* Set up the configuration structure */
   for (i = WpInfo.NumEntries; i < (WpInfo.NumEntries + WpConfig->NumEntries); i++) 
   {
      WpInfo.WpEntries[i].WpEnable = 1; 

      if (WP_SECURE_NV == WpConfig->WpEntries[i-WpInfo.NumEntries].WpType) 
      {
         WpInfo.WpEntries[i].WpTypeMask = 0; 
      }
      else if (WP_SECURE_P == WpConfig->WpEntries[i-WpInfo.NumEntries].WpType)  
      {
         WpInfo.WpEntries[i].WpTypeMask = 1; 
      }
      else if (WP_SECURE_P_EN == WpConfig->WpEntries[i-WpInfo.NumEntries].WpType)  
      {
         WpInfo.WpEntries[i].WpTypeMask = 2; 
      }
      else
      {
         DEBUG ((EFI_D_ERROR, "UFSSetLbaWriteProtect: Invalid WpType"));
         return EFI_INVALID_PARAMETER;
      }

      WpInfo.WpEntries[i].Addr = WpConfig->WpEntries[i-WpInfo.NumEntries].Lba;
      WpInfo.WpEntries[i].NumBlocks = WpConfig->WpEntries[i-WpInfo.NumEntries].NumBlocks;

   }

   WpInfo.NumEntries += WpConfig->NumEntries;

   /* Send request to secure app */
   Status = UFSSetSecureWpConfigBlock(&WpInfo);   
   if (EFI_SUCCESS != Status) 
   {
      DEBUG ((EFI_D_ERROR, "UFSSetLbaWriteProtect: UFSSetSecureWpConfigBlock failed "
                           "with return value of 0x%x\n", Status));
      return Status; 
   }

   return EFI_SUCCESS; 
}

/**
   Clear secure write protection entries for the LU specified
**/
EFI_STATUS EFIAPI UFSClearWriteProtect(
   IN EFI_STORAGE_WP_PROTOCOL *This
)
{
   EFI_STATUS Status;
   INT32 rc; 
   SdMgrSecureWpInfo WpInfo; 
   UFS_DEV *UfsDevice; 
   struct ufs_handle *hUFS;
   struct ufs_info_type info;
   UINT8 i = 0; 

   if (NULL == This) 
   {
      return EFI_INVALID_PARAMETER; 
   }

   UfsDevice = UFS_DEV_FROM_WRITE_PROTECT (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;

   /* Get device info to determine bLUWriteProtect value */
   rc = ufs_get_device_info (hUFS, &info);
   if (UFS_EOK != rc) 
   {
      DEBUG ((EFI_D_ERROR, "ClearWriteProtect: Get device info failed, status 0x%x\n", rc));
      return EFI_DEVICE_ERROR;
   }
 
   if (0 != info.bLUWriteProtect)
   {
      DEBUG ((EFI_D_ERROR, "Clear write protect only allowed for bLUWriteProtect = 0\n"));
      return EFI_INVALID_PARAMETER; 
   }

   for (i = 0; i < UFS_UEFI_MAX_LUN_ACCESS; i++) 
   {
      if (UfsDevice == &gUfsDevice[i]) 
      {
         WpInfo.LunNumber = i; 
      }
   }

   /* Get Secure Write Protect Configuration Block */
   Status = UFSGetSecureWpConfigBlock(&WpInfo); 
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "ClearWriteProtect: UFSGetSecureWpConfigBlock failed "
                           "with return value of 0x%x\n", Status));
      return Status; 
   }

   /* NumEntries > 0, so we clear the entries, but set NumEntries = 1 */
   SetMem(WpInfo.WpEntries, MAX_SECURE_WP_ENTRIES * sizeof(SdMgrSecureWpInfoEntry), 0x00);
   WpInfo.NumEntries = 0x1; 

   /* Update the Secure Write Protect Configuration Block */
   Status = UFSSetSecureWpConfigBlock(&WpInfo);
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "ClearWriteProtect: UFSSetSecureWpConfigBlock failed "
                           "with return value of 0x%x\n", Status));
      return Status; 
   }

   return EFI_SUCCESS; 
}

/**
   Read BufferSize bytes from Lba into Buffer. 
**/
EFI_STATUS EFIAPI UFSReadBlocks (
            IN EFI_BLOCK_IO_PROTOCOL   *This,
            IN UINT32                  MediaId,
            IN EFI_LBA                 Lba,
            IN UINTN                   BufferSize,
            OUT VOID                   *Buffer)
{
   INT32  rc; 
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   UINT32 transfer_size = 0, block_count = 0, block_size = 0;
   UINT8 *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media)) {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */
   if ((BufferSize%(This->Media->BlockSize)) != 0 ) {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0)) {
      return EFI_INVALID_PARAMETER;
   }
   
   if (This->Media->MediaId != MediaId) {
      return EFI_MEDIA_CHANGED;
   }

   UfsDevice = UFS_DEV_FROM_BLOCKIO (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   
   /* If BufferSize is zero, the host is pinging the card for its presence */
   if (0 == BufferSize)
   {
      if (!hUFS)
      {
         (void) ufs_close(hUFS);       
         UfsDevice->DeviceHandle = NULL;   
         UfsDevice->BlkIo.Media->MediaPresent = FALSE;        
         return EFI_NO_MEDIA;
      }
      else
      {
         return EFI_SUCCESS;
      }
   }

   block_size = UfsDevice->BlkIo.Media->BlockSize;
   block_count = (BufferSize/block_size);

   /* Check if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/block_size)) > (UfsDevice->BlkIo.Media->LastBlock + 1)) {
      return EFI_INVALID_PARAMETER;
   }

   /* Break transfer into smaller transfers to UFS_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;

   while (block_count > 0)
   {
      transfer_size = (block_count > UFS_MAX_BLOCK_TRANSFERS) ? 
         UFS_MAX_BLOCK_TRANSFERS : block_count;

      rc = ufs_read( hUFS, temp_buffer, Lba, transfer_size);

      Lba = Lba + transfer_size;
      block_count = block_count - transfer_size;
      temp_buffer = temp_buffer + (transfer_size * block_size);
   
      if (UFS_EOK != rc) {
         DEBUG ((EFI_D_ERROR, "UFS READ with return value of %d\n", rc));
         return EFI_DEVICE_ERROR;
      }
   }
   
   return EFI_SUCCESS;
}

/**
   Write BufferSize bytes from Lba into Buffer. 
**/
EFI_STATUS EFIAPI UFSWriteBlocks (
            IN EFI_BLOCK_IO_PROTOCOL   *This,
            IN UINT32                  MediaId,
            IN EFI_LBA                 Lba,
            IN UINTN                   BufferSize,
            IN VOID                    *Buffer)
{
   INT32  rc;   
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   UINT32 transfer_size = 0, block_count = 0, block_size = 0;
   UINT8 *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media)) {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */   
   if (((BufferSize%(This->Media->BlockSize)) != 0)) {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0)) {
      return EFI_INVALID_PARAMETER;
   }
   
   /* Validate if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/This->Media->BlockSize)) > (This->Media->LastBlock + 1)) {
      return EFI_INVALID_PARAMETER;
   }
     
   if (This->Media->MediaId != MediaId) {
      return EFI_MEDIA_CHANGED;
   }

   UfsDevice = UFS_DEV_FROM_BLOCKIO (This);
   hUFS = UfsDevice->DeviceHandle;
   
   if (NULL == hUFS) {
      return EFI_INVALID_PARAMETER;
   }
  
   /* Break transfer into smaller transfers to UFS_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;
   block_size = UfsDevice->BlkIo.Media->BlockSize;
   block_count = (BufferSize/block_size);

   while (block_count > 0)
   {
     transfer_size = (block_count > UFS_MAX_BLOCK_TRANSFERS) ? 
                         UFS_MAX_BLOCK_TRANSFERS : block_count;

     rc = ufs_write( hUFS, temp_buffer, Lba, transfer_size);

     Lba = Lba + transfer_size;
     block_count = block_count - transfer_size;
     temp_buffer = temp_buffer + (transfer_size * block_size);
   
     if (UFS_EOK != rc) {
        DEBUG ((EFI_D_ERROR, "UFS WRITE with return value of %d\n", rc));
        return EFI_DEVICE_ERROR;
     }
   }
   return EFI_SUCCESS;
}


/**
   Flush the Block Device.   
**/
EFI_STATUS EFIAPI UFSFlushBlocks (
            IN EFI_BLOCK_IO_PROTOCOL  *This)
{
   return EFI_SUCCESS;
}

VOID EFIAPI RegisterListener (
            IN EFI_EVENT        Event,
            IN VOID             *Context)
{
   EFI_STATUS EfiStatus = EFI_UNSUPPORTED; 

   EfiStatus = RPMBListenerInit((UINT32) Context); 
   if (EFI_SUCCESS != EfiStatus)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize RPMB Listener, Status 0x%08x\n", EfiStatus));
   }

   EfiStatus = GPTListenerInit((UINT32) Context); 
   if (EFI_SUCCESS != EfiStatus)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize GPT Listener, Status 0x%08x\n", EfiStatus));
   }

   EfiStatus = InitPartitionConfig();
   if (EFI_SUCCESS != EfiStatus)
   {
      DEBUG ((EFI_D_ERROR, "Failed to initialize Partition config, Status 0x%08x\n", EfiStatus));
   }  
}

/**
   Driver initializatoin entry point.
**/
EFI_STATUS EFIAPI UFSDxeInitialize (
            IN EFI_HANDLE         ImageHandle,
            IN EFI_SYSTEM_TABLE   *SystemTable)
{
   EFI_STATUS Status;
   UINT32 lun = 0;
   UINT32 lun_enabled_mask = 0;
   UINT8  total_luns_enabled = 0;
   INT32 rc = 0;
   struct ufs_info_type ufs_info;
   struct ufs_handle *ufs_device;

   /* Don't do anything if it doesn't boot from UFS */   
   if (!boot_from_ufs ()) {
      return EFI_SUCCESS;
   }
   
// TODO: Remove Timer Profiling Code
#if 0
   {
      Timetick_timer_Type Timer;
      UINT32 call_count = 0;
      UINT64 starting_tick = 0, ending_tick = 0, total_ticks = 0;       
      UINT32 TimetickFreq = 0;
      UINT64 uSeconds = 0;
 
      if(PcdGet32(PcdQTimerEnabled))
      {
        Timer = TIMETICK_QTIMER;
      }
      else
      {
        Timer = TIMETICK_TIMER_ADGT;
      }
 
      /* Enable TimeTick for collecting performance data */
      Timetick_Init();
      Timetick_Enable(Timer, TRUE);
      
      Timetick_GetCount(Timer, &starting_tick);
      //Delay of 5s
      for (call_count=0; call_count<5000; call_count++)
      {
        ufs_osal_stall_us(1);
      }       
      Timetick_GetCount(Timer, &ending_tick);
      total_ticks = ending_tick - starting_tick;

      /* Convert ticks to micro seconds */
      Timetick_GetFreq(Timer, &TimetickFreq);
      uSeconds = MultU64x32(total_ticks, 1000000);
      uSeconds = DivU64x32(uSeconds, TimetickFreq);  
      DEBUG ((EFI_D_ERROR, "uSeconds = %ld\n", uSeconds));
   }
#endif
   
   /* Create the RegisterListener callback */
   Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL,
                             TPL_CALLBACK,
                             RegisterListener,
                             (VOID *)SIGNATURE_32('u','f','s',' '),
                             &ScmInitialized
                             );
   ASSERT_EFI_ERROR (Status); 
   
   Status = gBS->RegisterProtocolNotify(&gQcomScmProtocolGuid,
                                        ScmInitialized, 
                                        (VOID *)&ScmInitializedToken); 

   ASSERT_EFI_ERROR(Status);
   
   /* Initialize UFS */
   ufs_device = ufs_open (0, UFS_WLUN_DEVICE);
   if (!ufs_device)
      ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);

   /* Get valid LUNs */
   rc = ufs_get_device_info (ufs_device, &ufs_info); 
   if (UFS_EOK != rc)
      ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
   lun_enabled_mask = ufs_info.dLunEnabled;
   total_luns_enabled = ufs_info.bNumberLu;
  
   for (lun = 0; lun < total_luns_enabled; lun++)
   { 
      // Check if LUN is enabled
      if (!(lun_enabled_mask & ((UINT32) 1) << lun))
         continue;

      gUfsDevicePath[lun].Ufs = UfsPath; 
      gUfsDevicePath[lun].Ufs.Guid = UfsDevicePathGuids[lun]; 
      gUfsDevicePath[lun].End = UfsEndPath;
      
      gUfsDevice[lun].Signature    = UFS_DEV_SIGNATURE;
      gUfsDevice[lun].BlkIo        = gBlockIoUfs;
      gUfsDevice[lun].BlkIo.Media  = &gMediaUfs[lun];
      gUfsDevice[lun].BlkIo2        = gBlockIo2Ufs;
      gUfsDevice[lun].BlkIo2.Media  = &gMediaUfs[lun];
      gUfsDevice[lun].CardInfo     = gUfsCardInfoProtocol;
      gUfsDevice[lun].EraseBlk     = gUfsEraseBlkProtocol;
      CopyMem(gUfsDevice[lun].BlkIo.Media, 
                 &UFSMedia, sizeof(EFI_BLOCK_IO_MEDIA));  
      gUfsDevice[lun].WriteProtect = gUfsWriteProtectProtocol;

      /* Initialize LUN */
      gUfsDevice[lun].DeviceHandle = ufs_open (0, lun);
      if (!gUfsDevice[lun].DeviceHandle) {
         gUfsDevice[lun].BlkIo.Media->MediaPresent = FALSE;
         continue;
      }
      gUfsDevice[lun].BlkIo.Media->MediaPresent = TRUE;
      rc = ufs_get_device_info ((struct ufs_handle *)gUfsDevice[lun].DeviceHandle, &ufs_info); 
      if (UFS_EOK != rc)
         ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
      gUfsDevice[lun].BlkIo.Media->LastBlock = ufs_info.dLuTotalBlocks - 1;

      /* Assume LUN0 and install the following protocols: */ 
      /* BlkIO */
      Status = gBS->InstallMultipleProtocolInterfaces (
         &gUfsDevice[lun].ClientHandle,  
         &gEfiBlockIoProtocolGuid, &gUfsDevice[lun].BlkIo, 
         &gEfiDevicePathProtocolGuid, &gUfsDevicePath[lun],
         &gEfiMemCardInfoProtocolGuid, &gUfsDevice[lun].CardInfo,
         &gEfiBlockIo2ProtocolGuid, &gUfsDevice[lun].BlkIo2,
         &gEfiEraseBlockProtocolGuid, &gUfsDevice[lun].EraseBlk,
         &gEfiStorageWpProtocolGuid, &gUfsDevice[lun].WriteProtect, 
         NULL
      );
      ASSERT_EFI_ERROR(Status);
   }
   
   gUfsRpmbDevicePath.Ufs = UfsPath; 
   gUfsRpmbDevicePath.Ufs.Guid = (EFI_GUID)EFI_UFS_RPMB_LUN_GUID;
   gUfsRpmbDevicePath.End = UfsEndPath;
   
   gUfsRpmbDevice.RPMB = gUfsRpmbProtocol;
   gUfsRpmbDevice.CardInfo = gUfsCardInfoProtocol;
  
   gUfsRpmbDevice.DeviceHandle = ufs_open (0, UFS_WLUN_RPMB);
   if (!gUfsRpmbDevice.DeviceHandle) {
      DEBUG ((EFI_D_ERROR, "Cannot open UFS RPMB!\n"));
   }   
   rc = ufs_get_device_info ((struct ufs_handle *)gUfsRpmbDevice.DeviceHandle, &ufs_info); 
   if (UFS_EOK != rc)  ASSERT_EFI_ERROR(EFI_DEVICE_ERROR);
   
   gUfsRpmbDevice.RPMB.RPMBMaxTransferSize = ufs_info.qLogicalBlockCount * 256;
   gUfsRpmbDevice.RPMB.RPMBPartitionSizeInBytes = ufs_info.qLogicalBlockCount * 256;
   gUfsRpmbDevice.RPMB.ReliableWriteCount = ufs_info.bRPMB_ReadWriteSize;
   
   Status = gBS->InstallMultipleProtocolInterfaces (
      &gUfsRpmbDevice.ClientHandle,  
      &gEfiSdccRpmbProtocolGuid, &gUfsRpmbDevice.RPMB,
      &gEfiDevicePathProtocolGuid, &gUfsRpmbDevicePath,
      &gEfiMemCardInfoProtocolGuid, &gUfsRpmbDevice.CardInfo,
      NULL
   );  

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

   /* Initialize request list for Block IO 2 */ 
   gUfsTransferInfo.BlkIO2Initialized = TRUE;
   Status = BlkIo2ReqListInit(&gUfsReqList, REQ_LIST_SIZE);

   if(Status == EFI_SUCCESS)
   {
      /* Create Internal Event for Block IO 2 */
      Status = gBS->CreateEvent (
                       EVT_TIMER | EVT_NOTIFY_SIGNAL,
                       UFS_TPL,
                       BlkIo2TimerEventNotify,
                       NULL,
                       &gUfsBlkIo2TimerEvent
                       );
      
      if(EFI_ERROR (Status)) 
      {
         gUfsTransferInfo.BlkIO2Initialized = FALSE;
         DEBUG ((EFI_D_WARN, "Block IO 2 timer creation failed, cannot use Block IO 2.\n"));
      }
   }   
   else 
   {
      gUfsTransferInfo.BlkIO2Initialized = FALSE;
      DEBUG ((EFI_D_WARN, "Initialization failed, cannot use Block IO 2.\n"));
   }

   /* Crate an event for Erase, Purge */
   gUfsTransferInfo.PurgeEventInitialized = TRUE;
   Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    UFS_TPL,
                    PurgeTimerEventNotify,
                    NULL,
                    &gUfsPurgeTimerEvent
                    );
   
   if(EFI_ERROR (Status)) 
   {
      gUfsTransferInfo.PurgeEventInitialized = FALSE;
      DEBUG ((EFI_D_WARN, "Purge event timer creation failed, cannot use non-blocking block erase.\n"));
   }
   
   return EFI_SUCCESS;
}


/**
   Get the active UFS Boot LU  
**/
EFI_STATUS EFIAPI UFSGetActiveBootLU (
   EFI_MEM_CARDINFO_PROTOCOL  *This,
   UINT32                     *ActiveBootLU)
{
   INT32 rc;
   struct ufs_info_type info;

   if ((NULL == This) || (NULL == ActiveBootLU))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Open WLUN BOOT and get Card Info */
   if (!gUfsBootDevice.DeviceHandle) 
   {
      gUfsBootDevice.DeviceHandle = ufs_open(0, UFS_WLUN_BOOT);
      if (!gUfsBootDevice.DeviceHandle)
      {
         DEBUG ((EFI_D_ERROR, "Cannot open UFS WLUN BOOT!\n"));
         return EFI_DEVICE_ERROR; 
      }
   }

   rc = ufs_get_device_info ((struct ufs_handle *)gUfsBootDevice.DeviceHandle, &info); 
   if (UFS_EOK != rc)  
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED %x\n\n\n", rc)); 
      return EFI_DEVICE_ERROR;
   }

   /* bLUN tells us which BOOT LUN is current */
   *ActiveBootLU = info.bLUN;     

   return EFI_SUCCESS; 
}

/**
   Set the active UFS Boot LU 
**/
EFI_STATUS EFIAPI UFSSetActiveBootLU (
   EFI_MEM_CARDINFO_PROTOCOL *This,
   UINT32                    BootLU)
{
   INT32 rc;
   UFS_DEV *UfsDevice;
   struct ufs_handle *hUFS;

   if ((NULL == This) || (BootLU > UFS_LUN_BOOT_B)) 
   {
      return EFI_INVALID_PARAMETER;
   }

   UfsDevice = UFS_DEV_FROM_CARD_INFO (This);
   hUFS = UfsDevice->DeviceHandle;

   if (NULL == hUFS) 
   {
      return EFI_INVALID_PARAMETER;
   }

   rc =  ufs_set_bootable (hUFS, BootLU); 
   if (UFS_EOK != rc)
   {
      DEBUG ((EFI_D_ERROR, "Set Active Boot LU FAILED. %x\n\n\n", rc));
      return EFI_DEVICE_ERROR;
   }

   return EFI_SUCCESS; 
}

/**
   Get the card information 
**/
EFI_STATUS EFIAPI UFSGetCardInfo (
            EFI_MEM_CARDINFO_PROTOCOL   *This,
            MEM_CARD_INFO               *card_info)
{
   char serial_num_str[254] = {0}; // 126 unicode chars + length + IDN 
   INT32 rc;
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   struct ufs_info_type info;
   int i;

   if ((NULL == card_info) || (NULL == This)) {
      return EFI_INVALID_PARAMETER;
   }

   UfsDevice = UFS_DEV_FROM_CARD_INFO (This);
   hUFS = UfsDevice->DeviceHandle;
   if (NULL == hUFS) {
      return EFI_INVALID_PARAMETER;
   }

   /* Get Card Info */
   rc = ufs_get_device_info (hUFS, &info);
   if (UFS_EOK != rc) {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n\n\n", rc));
      return EFI_DEVICE_ERROR;
   }

   /* Copy the information out */
   card_info->card_type[0] = 'U';
   card_info->card_type[1] = 'F';
   card_info->card_type[2] = 'S';
   card_info->card_type[3] = ' ';

   card_info->mfr_id = info.wManufacturerID;
   rc = ufs_get_device_info_str (hUFS, info.iSerialNumber, 
                                 serial_num_str, sizeof(serial_num_str)); 
   if (UFS_EOK != rc || serial_num_str[1] != 0x05)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Serial Number FAILED. %x\n\n\n", rc));
      return EFI_DEVICE_ERROR;
   }

   card_info->serial_num_len = serial_num_str[0] - 2;  // Get length and reduce by 2 bytes (length, IDN)

   CopyMem(card_info->product_serial_num, &serial_num_str[2], card_info->serial_num_len);

   for (i = 0; i < 29; i++) 
   {
      card_info->inquiry_str[i] = info.inquiry_str[i];
   }

   rc = ufs_get_device_info ((struct ufs_handle *)gUfsRpmbDevice.DeviceHandle, &info); 
   if (UFS_EOK != rc)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n\n\n", rc));
      return EFI_DEVICE_ERROR;
   }
   card_info->rpmb_size_in_byte = info.qLogicalBlockCount * 256;
   card_info->reliable_write_count = info.bRPMB_ReadWriteSize;

   return EFI_SUCCESS;
}


/**
   This function retrieves Read Counter Response packet from flash device.
   The Response packet includes the Write Counter as well as the MAC which
   is used later to validate the Response packet
  
   @param  RespPktBuffer [OUT]    Pointer to the response from the Read
                                  Counter command
    
   @retval  RPMB_NO_ERROR           Successfully read the Counter packet.
   @retval  RPMB_ERR_INVALID_PARAM  NULL pointer is passed in to the function.
   @retval  RPMB_ERR_READ           Failed to read the Counter packet.
**/
INT32 EFIAPI RPMBReadCounterPkt (
            EFI_SDCC_RPMB_PROTOCOL       *This,
            UINT32                       *RespPktBuffer)
{
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   INT32  rc = 0;

   if ((NULL == This) || (NULL == RespPktBuffer)) {
      return RPMB_ERR_INVALID_PARAM;
   }
   
   UfsDevice = UFS_DEV_FROM_RPMB (This);
   hUFS = UfsDevice->DeviceHandle;
  
   if (NULL == hUFS) {
      return RPMB_ERR_INVALID_PARAM;
   }
 
   rc = ufs_rpmb_read_cnt_pkt(hUFS, (UINT8 *)RespPktBuffer);

   if (UFS_EOK != rc) {
      return RPMB_ERR_READ;
   }
   return RPMB_NO_ERROR;
}

/**
 This function programs the RPMB's key using the provided packet
 formatted as ProvisionKey command. 

 @param CmdPktBuffer  [IN] : Pointer to a ProvisionKey command packet
                             to be sent to RPMB to program its key
 @param RespPktBuffer [OUT]: Pointer to the response packet for the
                             ProvisionKey command
 
 @retval  RPMB_NO_ERROR            Successfully provision the RPMB's Key.
 @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function.
 @retval  RPMB_ERR_GENERAL         Failed to provision the key (Key has been
                                   provisioned).
**/
INT32 EFIAPI RPMBProgProvisionKey (
            EFI_SDCC_RPMB_PROTOCOL       *This,
            UINT32                       *CmdPktBuffer,
            UINT32                       *RespPktBuffer )
{
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS = NULL;
   INT32  rc = UFS_EOK; 

   if ((NULL == CmdPktBuffer) || (NULL == RespPktBuffer) || (NULL == This)) {
      return RPMB_ERR_INVALID_PARAM;
   }

   UfsDevice = UFS_DEV_FROM_RPMB (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   
   if (NULL == hUFS) {
      return RPMB_ERR_INVALID_PARAM;
   }

   rc = ufs_rpmb_write_blocks(hUFS, (UINT8 *)CmdPktBuffer, 
                              1, (UINT8 *)RespPktBuffer);

   if (UFS_EOK != rc) {
      return RPMB_ERR_GENERAL;
   }

   return RPMB_NO_ERROR;
}

/**
  This function reads the sectors from RPMB partition using the
  cmd packet buffers provided.
 
  @param   CmdPktBuffer  [IN] : Pointer to a formatted packet for Read Request 
  @param   PktCount      [IN] : How many half sectors to read   
  @param   DataBuffer    [OUT]: Pointer to data read from PRMB
 
  @retval  RPMB_NO_ERROR            Successfully read the Counter packet
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_READ            Failed to read the sectors
     
**/ 
INT32 EFIAPI RPMBReadBlocks (
            EFI_SDCC_RPMB_PROTOCOL       *This,
            UINT32                       *CmdPktBuffer,
            UINT32                       PktCount,
            UINT32                       *DataBuffer)
{
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   INT32 rc = UFS_EOK;

   if ((NULL == CmdPktBuffer) || (NULL == DataBuffer) || 
       (NULL == This) || (PktCount == 0))
   {
      return RPMB_ERR_INVALID_PARAM;
   }
   
   if (PktCount > RPMB_MAX_BLOCK_TRANSFER_SIZE) {
      return RPMB_ERR_INVALID_PARAM;
   }
   
   UfsDevice = UFS_DEV_FROM_RPMB (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   
   if (NULL == hUFS) {
      return RPMB_ERR_INVALID_PARAM;
   }

   rc = ufs_rpmb_read_blocks(hUFS, (UINT8 *)CmdPktBuffer, 
                             PktCount, (UINT8 *)DataBuffer); 
   if (UFS_EOK != rc) {
      return RPMB_ERR_READ;
   }
     
   return RPMB_NO_ERROR;

}

/**
  This function writes the sectors to the RPMB partition using
  the provided cmd packet buffers
                       
  @param  CmdPktBuffer  [IN] :  Pointer to a formatted packets to be sent
                                to PRMB to write to RPMB partition
  @param  PktCount      [IN] :  Number of half sectors to write
  @param  RespPktBuffer [OUT]:  Pointer to the response packet from PRMB
 
  @retval  RPMB_NO_ERROR            Successfully Write the RPMB sectors.
                                    Caller should authenticate the Response
                                    packet and validate the Write Counter
  @retval  RPMB_ERR_INVALID_PARAM   NULL pointer is passed in to the function
  @retval  RPMB_ERR_WRITE           Failed to write the sectors
**/
INT32 EFIAPI RPMBWriteBlocks (
            EFI_SDCC_RPMB_PROTOCOL       *This,
            UINT32                       *CmdPktBuffer,
            UINT32                       PktCount,
            UINT32                       *RespPktBuffer )
{
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS = NULL;
   INT32  rc = UFS_EOK;

   if ((NULL == CmdPktBuffer) || (NULL == RespPktBuffer) || 
      (NULL == This) || (PktCount == 0)) 
   {
      return RPMB_ERR_INVALID_PARAM;
   }

   if (PktCount > RPMB_MAX_BLOCK_TRANSFER_SIZE) {
      return RPMB_ERR_INVALID_PARAM;
   }

   UfsDevice = UFS_DEV_FROM_RPMB (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   
   if (NULL == hUFS) {
      return RPMB_ERR_INVALID_PARAM;
   }

   rc = ufs_rpmb_write_blocks(hUFS, (UINT8 *)CmdPktBuffer, 
                              PktCount, (UINT8 *)RespPktBuffer);

   if (UFS_EOK != rc) {
      return RPMB_ERR_WRITE;
   }

   return RPMB_NO_ERROR;
}

/* Block IO2 supporting functions */

/**
   A wrapper for Block IO and Block IO 2. whether Block IO or Block IO 2 protocol
   will be used depending on caller and device status.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  Token    A pointer to the token associated with the transaction.
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
EFI_STATUS UFSIoWrapper (
            IN VOID                     *pThis,
            IN UINT32                    MediaId,
            IN EFI_LBA                   Lba,
            IN OUT EFI_BLOCK_IO2_TOKEN  *Token,
            IN UINTN                     BufferSize,
            IN OUT VOID                 *Buffer)
{
   EFI_STATUS        Status = EFI_SUCCESS;
   UFS_DEV           *UfsDevice;
   UINT32            block_size = 0;
   BLKIO2_REQ_LIST  *pReqList = &gUfsReqList;
   
   if (gUfsTransferInfo.TransferType == BLOCKIO_READ || 
       gUfsTransferInfo.TransferType == BLOCKIO_WRITE)
   {
      EFI_BLOCK_IO_PROTOCOL  *This = pThis;
      
      // Cancel timer for Block IO 
      if (gUfsTransferInfo.TimerSet == TRUE) {
         gBS->SetTimer (gUfsBlkIo2TimerEvent, TimerCancel, 0); 
      }

      // If no Block IO 2 tranfer is in progress 
      if (!pReqList->CurReq->IsStart)
      {
         if (gUfsTransferInfo.TransferType == BLOCKIO_READ) {
            Status = UFSReadBlocks(This, MediaId, Lba, BufferSize, Buffer);
         } else {           
            Status = UFSWriteBlocks(This, MediaId, Lba, BufferSize, Buffer);
         }

         // Set timer if necessary
         if (TRUE == gUfsTransferInfo.TimerSet) {
            gBS->SetTimer (gUfsBlkIo2TimerEvent, TimerPeriodic, BLKIO2_POLL_PERIOD);
         }
         goto WrapperExit;
      }
      
      // If Block IO 2 is in progress, Block IO will take over the transfer 
      // and has priority to run. Block IO 2 resumes after Block IO 
      if (pReqList->CurReq->IsStart) {         
         Status = BlkIoTakeOverTransfer(This, MediaId, Lba, BufferSize, Buffer);
      }

      // Set timer to call Block IO 2 
      if (TRUE  == gUfsTransferInfo.TimerSet) {
         gBS->SetTimer (gUfsBlkIo2TimerEvent, TimerPeriodic, BLKIO2_POLL_PERIOD);
      }
   }
   // If caller calls Block IO 2 
   else 
   {  
      EFI_BLOCK_IO2_PROTOCOL *This = pThis;

      // Validate passed-in pointers are not NULL 
      if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media) || 
          (NULL == Token) || (NULL == Token->Event)) {
         Status = EFI_INVALID_PARAMETER;
         goto WrapperExit;
      }
      
      // Validate if buffer is multiple of block size 
      if ((BufferSize%(This->Media->BlockSize) != 0) || (BufferSize == 0)) {
         Status = EFI_BAD_BUFFER_SIZE;
         goto WrapperExit;
      }

      // Validate block size and access range 
      if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0)) {
         Status = EFI_INVALID_PARAMETER;
         goto WrapperExit;
      }
   
      if (This->Media->MediaId != MediaId) {
         Status = EFI_MEDIA_CHANGED;
         goto WrapperExit;
      }
     
      UfsDevice = UFS_DEV_FROM_BLOCKIO2 (This);       
      block_size = UfsDevice->BlkIo.Media->BlockSize;

      if ((Lba + (BufferSize/block_size)) > (UfsDevice->BlkIo.Media->LastBlock + 1)) {
         Status = EFI_INVALID_PARAMETER;
         goto WrapperExit;
      }   
      
      // Append request to queue 
      EfiAcquireLock (&pReqList->ReqListLock);

      if (pReqList->Full == TRUE) { 
         EfiReleaseLock (&pReqList->ReqListLock);      
         Status = EFI_OUT_OF_RESOURCES;
         goto WrapperExit;
      }
  
      pReqList->LastReq->Token = Token;
      pReqList->LastReq->This = This;
      pReqList->LastReq->MediaId = MediaId;
      pReqList->LastReq->Lba = Lba;
      pReqList->LastReq->BufferSize = BufferSize;
      pReqList->LastReq->Buffer = Buffer;
      
      // Append opeartion type
      if (gUfsTransferInfo.TransferType == BLOCKIO2_READ) {        
         pReqList->LastReq->Type = BLOCKIO2_READ;  
      } else {        
         pReqList->LastReq->Type = BLOCKIO2_WRITE;
      }

      pReqList->LastReq ++;
  
      if(pReqList->LastReq == pReqList->Tail) {
         pReqList->LastReq = pReqList->Head;  
      }
      
      if(pReqList->LastReq == pReqList->CurReq) { 
         pReqList->Full = TRUE;                    
      }

      EfiReleaseLock (&pReqList->ReqListLock);           
      
      // Call Notity function to do real Block IO 2 transfer 
      BlkIo2TimerEventNotify (NULL, NULL);   
   }

WrapperExit:
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
EFI_STATUS UFSReadBlocksWrapper (
            IN EFI_BLOCK_IO_PROTOCOL  *This,
            IN UINT32                  MediaId,
            IN EFI_LBA                 Lba,
            IN UINTN                   BufferSize,
            OUT VOID                  *Buffer)
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   // UFS accesses has NOTIFY priority 
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL (UFS_TPL);

   gUfsTransferInfo.TransferType = BLOCKIO_READ;
   Status = UFSIoWrapper (This, MediaId, Lba, NULL, BufferSize, Buffer);

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
EFI_STATUS UFSWriteBlocksWrapper (
            IN EFI_BLOCK_IO_PROTOCOL   *This,
            IN UINT32                  MediaId,
            IN EFI_LBA                 Lba,
            IN UINTN                   BufferSize,
            IN VOID                    *Buffer)
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   // UFS accesses has NOTIFY priority 
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL (UFS_TPL);

   gUfsTransferInfo.TransferType = BLOCKIO_WRITE;
   Status = UFSIoWrapper(This, MediaId, Lba, NULL, BufferSize, Buffer);

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
  @param[in, out]  Token       A pointer to the token associated with the transaction.
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
EFI_STATUS UFSReadBlocksEx (
            IN     EFI_BLOCK_IO2_PROTOCOL *This,
            IN     UINT32                 MediaId,
            IN     EFI_LBA                Lba,
            IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
            IN     UINTN                  BufferSize,
            OUT VOID                      *Buffer)
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   /* UFS accesses has NOTIFY priority */
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(UFS_TPL);

   /* Do not use Block IO 2 if not initialized succesfully */
   if (gUfsTransferInfo.BlkIO2Initialized == FALSE)
   {
      Status = EFI_OUT_OF_RESOURCES;
      goto ReadBlocksExExit;
   }

   gUfsTransferInfo.TransferType = BLOCKIO2_READ;
   Status = UFSIoWrapper(This, MediaId, Lba, Token, BufferSize, Buffer);

ReadBlocksExExit:
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
EFI_STATUS UFSWriteBlocksEx (
            IN     EFI_BLOCK_IO2_PROTOCOL *This,
            IN     UINT32                 MediaId,
            IN     EFI_LBA                Lba,
            IN OUT EFI_BLOCK_IO2_TOKEN    *Token,
            IN     UINTN                  BufferSize,
            OUT VOID                      *Buffer)
{
   EFI_STATUS Status = EFI_UNSUPPORTED;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(UFS_TPL);

   /* Do not use Block IO 2 if not initialized succesfully */
   if (gUfsTransferInfo.BlkIO2Initialized == FALSE)
   {
      Status = EFI_OUT_OF_RESOURCES;
      goto WriteBlocksExExit;
   }  

   gUfsTransferInfo.TransferType = BLOCKIO2_WRITE;
   Status = UFSIoWrapper(This, MediaId, Lba, Token, BufferSize, Buffer);

WriteBlocksExExit:
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
EFI_STATUS EFIAPI UFSResetEx (
            IN EFI_BLOCK_IO2_PROTOCOL  *This,
            IN BOOLEAN                 ExtendedVerification)
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
EFI_STATUS EFIAPI UFSFlushBlocksEx (
            IN     EFI_BLOCK_IO2_PROTOCOL   *This,
            IN OUT EFI_BLOCK_IO2_TOKEN      *Token)
{
   return EFI_SUCCESS;
}


/**
  Erase blocks in the Block Device
 
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
EFI_STATUS EFIAPI UFSEraseBlocks (
   IN     EFI_BLOCK_IO_PROTOCOL *This,
   IN     UINT32 MediaId,
   IN     EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN     UINTN Size
   )
{
   INT32  rc; 
   UFS_DEV    *UfsDevice;
   struct ufs_handle *hUFS;
   UINT32 blocks;
   EFI_STATUS Status;

   /* Validate passed-in size are not zero */
   if (0 == Size) {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if size is multiple of block size */
   if ((Size%(This->Media->BlockSize)) != 0 ) {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate block size and access range */
   if ((LBA > This->Media->LastBlock) || (This->Media->BlockSize == 0)) {
      return EFI_INVALID_PARAMETER;
   }
   
   if (This->Media->MediaId != MediaId) {
      return EFI_MEDIA_CHANGED;
   }

   gUfsCurrentErase = This;
   UfsDevice = UFS_DEV_FROM_BLOCKIO (This);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   
   /* Check if LBA plus the total sectors trying to erase would exceed the */
   /* total size of the partition */
   if ((LBA + (Size/This->Media->BlockSize)) > (UfsDevice->BlkIo.Media->LastBlock + 1)) {
      return EFI_INVALID_PARAMETER;
   }

   blocks = Size/(This->Media->BlockSize);

   rc = ufs_unmap (hUFS, LBA, blocks);
   if (UFS_EOK != rc) {
      DEBUG ((EFI_D_ERROR, "UFS UNMAP with return value of %d\n", rc));
      return EFI_DEVICE_ERROR;
   }

   /* Setup non-block purge sequence if purge event is initialized */
   if (gUfsTransferInfo.PurgeEventInitialized && Token != NULL) {
      rc = ufs_purge_nonblock_setup (hUFS);
      if (rc == -UFS_EBUSY) {
         // Device in busy, continue the non-blocking wait
         gUfsTransferInfo.TransferType = BLOCKERASE_PURGE;
      }
      else if (rc == UFS_EOK) {
         // Device is out of busy already, no need to wait
         gUfsTransferInfo.TransferType = 0;
         if (Token != NULL) {
            gUfsTransferInfo.EraseToken = NULL;
            Token->TransactionStatus = EFI_SUCCESS;
         }
         return EFI_SUCCESS;
      }
      else {
         DEBUG ((EFI_D_WARN, "UFS non-blocking purge setup failed with value of %d\n", rc));
      }
   }

   rc = ufs_purge_and_wait (hUFS);
   if (rc == -UFS_EBUSY) {
      /* Pending transfer is taking place, setting up a timer */
      gBS->SetTimer (gUfsPurgeTimerEvent, TimerPeriodic, BLKIO2_POLL_PERIOD);
      if (Token != NULL) {
         gUfsTransferInfo.EraseToken = Token;
         Status = gBS->CreateEvent (EVT_TIMER,
                                    TPL_APPLICATION,
                                    NULL,
                                    NULL,
                                    &Token->Event );
         ASSERT_EFI_ERROR (Status); 
         Token->TransactionStatus = EFI_SUCCESS;
      }
      return EFI_SUCCESS;
   }
   else if (rc != UFS_EOK) {
      DEBUG ((EFI_D_ERROR, "UFS purge with return value of %d\n", rc));
      if (Token != NULL) {
         Token->Event = NULL;
         Token->TransactionStatus = EFI_DEVICE_ERROR;
      }
      return EFI_DEVICE_ERROR;
   }

   /* Purge completed, set the token to NULL */
   if (Token != NULL) {
      Token->Event = NULL;
      Token->TransactionStatus = EFI_SUCCESS;
   }
   
   return EFI_SUCCESS;
}

/******************************************************************************
*  Handle the timer event for purge
*
* Arguments:
*    Event              [IN] : UEFI event method, not used
*    Context            [IN] : Can be any forms of input parameter, not used
******************************************************************************/
VOID PurgeTimerEventNotify (
                     IN  EFI_EVENT  Event,
                     IN  VOID      *Context)
{  
   INT32 rc = -UFS_ENOENT;  
   EFI_ERASE_BLOCK_TOKEN  *temp_token;
   UFS_DEV                *UfsDevice;
   struct ufs_handle      *hUFS;

   UfsDevice = UFS_DEV_FROM_BLOCKIO (gUfsCurrentErase);
   hUFS = (struct ufs_handle *) UfsDevice->DeviceHandle;       
   rc = ufs_purge_and_wait (hUFS);
   
   if (rc == UFS_EOK) {
      gBS->SetTimer (gUfsPurgeTimerEvent, TimerCancel, 0);
      temp_token = gUfsTransferInfo.EraseToken;
      gBS->SignalEvent(temp_token->Event);   
      return;
   }
   if (rc == -UFS_EBUSY) {
      /* Still busy, wait more */
      return;
   }
   else {
      DEBUG ((EFI_D_ERROR, "UFS purge with return value of %d\n", rc));
      return;
   }

   return;
}

