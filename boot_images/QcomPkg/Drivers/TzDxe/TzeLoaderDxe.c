/** @file TzeLoaderDxe.c
  This file implements load

  Copyright (c) 2011-2017, Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 08/01/16   rj       Added functionality for showing qsee logs (tz apps logs)
 07/20/16   rj       updated load image function to take partition lable and moved LoadSecureApps
 07/15/16   vk       Update log format for grep
 07/13/16   rj       Added timing profile for app region notification scm call
 07/05/16   bh       Update file system call after API change
 06/23/16   pd       Added a new generic api for loading image from partition.
 06/01/16   rj       Added functionality to load common lib from partition based label 
 04/24/15   sm       Fixed incorrect parameter in LoadImageFromBuffer
 04/23/15   vk       Add UFS support
 03/16/15   sm       Update filname and add better error messages
 03/02/15   vk       LoadFileFromPartition signature update
 02/26/15   vk       Update filename
 12/05/14   vk       Add log message
 12/03/14   sk       Added support for config flags
 10/16/14   vk       LoadFileFromPartition signature update
 08/15/14   sm       Switched to new SCM API
 07/08/14   sk       Fixed KW null-check error
 04/17/14   sm       Removed debug variable
 03/18/14   sm       Updated some types and standardized typecasting
 12/27/13   sp       Updated to new fields of TPMControlAreaMemRegionInfo
 08/01/13   wuc      Load cmnlib and dxhdcp2
 07/25/13   shl      Changed printing level.
 02/21/13   shl      ScmDxe cleanup.
 01/24/13   ag       Use SCMDxe to load apps  
 01/10/13   niting   Updated loading of TZ Apps
 12/14/12   ag       Load TzApps at UEFI specified region
 12/13/12   ag       TzVersion issue
 12/05/12   shl      Reorg initializations, moved them inot TzDxe.c
 12/04/12   ag       Fix warnings
 11/08/12   shl      Added loading SSD key store
 05/17/12   ag       Tz1.4 related app loading changes
 04/03/12   jz       Enable UncachedFreePool call, cleanup
 03/07/12   rs       Added intialization of UEFI Encryption Protocol
 03/06/12   jz       Renamed PcdTZSharedParamsBaseOffset to PcdTZScmCmdBufferBaseOffset
 11/23/11   jz       Memory map PCDs cleanup
 08/15/11   yg       Created new file
 07/14/17   dp       Moved MountFatPartition()from TzeLoaderProtocolInit() to WP specific file.
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <Include/scm_sip_interface.h>
#include <Include/scm_qsee_interface.h>
#include <TzeLoaderDxe.h>
#include <Library/SerialPortLib.h>
#include <LoaderUtils.h>

QCOM_SCM_PROTOCOL *QcomScmProtocol = NULL;

#define TZBSP_computed_VERSION(major, minor, patch) \
  (((major & 0x3FF) << 22) | ((minor & 0x3FF) << 12) | (patch & 0xFFF))



/* Ideally we should not find more than 1 handle */
HandleInfo HandleInfoList[4];
UINT32 MaxHandles;
PartiSelectFilter HandleFilter;

extern EFI_GUID gEfiEmmcGppPartition1Guid;
extern EFI_GUID gEfiEmmcUserPartitionGuid;
extern EFI_GUID gEfiUfsLU4Guid;            //TZAPPS

extern EFI_STATUS EFIAPI GetMemRegionInfoByName (IN CHAR8* RegionName, IN OUT MemRegionInfo* MemoryRegion);

UINT32*          UncachedMem;
UINTN            UncachedMemSize;
#define          UNCACHED_MEM_DEFAULT_SIZE     (520 * 1024)
UINT64 *log_buffer = NULL;

/*------------------------------------------------------------------------
                              PUBLIC METHODS
------------------------------------------------------------------------*/

extern EFI_STATUS
LoadSecureApps(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_HANDLE                      ImageHandle,
   IN  EFI_SYSTEM_TABLE               *SystemTable,
   OUT UINT32                         *AppIdPtr
);

static EFI_TZE_LOADER_PROTOCOL TzeLoaderProtocol = 
{
  EFI_TZE_LOADER_PROTOCOL_REVISION,
  LoadImageFromPartition,
  LoadImageFromFile,
  LoadImageFromBuffer,
  LoadImageFromFileSystem,
  LoadSecureApps,
  LoadImageFromPartitionLabel,
  LoadExternalImageFromFileSystem,
};

// Allocated once and never freed until UEFI exits.
static VOID *
TzeUncachedAllocatePool(IN UINTN  AllocationSize)
{
  if (AllocationSize > UncachedMemSize)
  {
    UncachedSafeFreePool (UncachedMem);
    UncachedMem = UncachedAllocatePool (AllocationSize);
    if (UncachedMem == NULL)
      return NULL;
    UncachedMemSize = AllocationSize;
  }
  return UncachedMem;
}


static VOID
TzeUncachedFreePool (
  IN VOID   *Buffer
)
{
  // This buffer need not be freed, UEFI will free it
  // before returning to HLOS.
  //UncachedSafeFreePool(Buffer);
  return;
}

/**
  Does the scm call to TZOS_LOAD_EXTERNAL_IMAGE
  @param[in]  mdt_len       MDT size of the image to load.
  @param[in]  img_len       Size of mbn for the image to load.
  @param[in]  pa            Pointer to the path where the image is located.
  @return EFI_SUCCESS if successful
**/
static UINT32
QseeLoadExternalImageSyscall (
  UINTN mdt_len,
  UINTN img_len,
  VOID *pa)
{

  UINT32                          SmcId;
  UINT32                          ParamId;
  UINT64                          Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                          Results[SCM_MAX_NUM_RESULTS] = {0};
  EFI_STATUS     Status;
  
  SmcId=TZ_OS_LOAD_EXTERNAL_IMAGE_ID;
  ParamId=TZ_OS_LOAD_EXTERNAL_IMAGE_ID_PARAM_ID;
  Parameters[0]=mdt_len;
  Parameters[1]=img_len;
  Parameters[2]=(UINT32)(UINTN)pa;

  Status = QcomScmProtocol->ScmSipSysCall(QcomScmProtocol,
                                            SmcId,
                                            ParamId,
                                            Parameters,
                                            Results);
  if (Status != EFI_SUCCESS)
   { 
     return 1;
   }
  return Status;
}
/* 
*  
* Load Services MBN using Tz2.0 qsee scm command
*/
static EFI_STATUS
QseeLoadServiceImageSyscall (
  UINT32 mdt_len,
  UINT32 img_len,
  VOID   *pa
  )
{
   qsee_load_serv_image_t *qsee_syscallp;
   UINT32                  app_id = 0;
   EFI_STATUS              Status;

   qsee_syscallp= (qsee_load_serv_image_t *)AllocatePool(sizeof(qsee_load_serv_image_t));
   if (NULL == qsee_syscallp)
   {
      return EFI_OUT_OF_RESOURCES;
   }
   SetMem(qsee_syscallp, sizeof(qsee_load_serv_image_t), 0x0);
   qsee_syscallp->mdt_len = mdt_len;
   qsee_syscallp->img_len = img_len;
   qsee_syscallp->pa = (UINT32)(UINTN)pa;

   Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                            APP_LOAD_SERV_IMAGE_CMD,
                                            &(app_id),
                                            qsee_syscallp,
                                            sizeof(qsee_load_serv_image_t),
                                            NULL,
                                            0);

   FreePool(qsee_syscallp);
   return Status;
}

/*Adding syscall to get TZ version number
 * WIll send a syscall to TZ and find out the current TZ version number.
 * Tz1.3 will return an error as this functionality is not implemented
 */


EFI_STATUS
TzGetFeatureVersion (
  UINT32  FeatureId,
  UINT32 *Version
  )
{
  EFI_STATUS                Status;
  UINT64                    Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                    Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_feature_version_req_t *SysCallReq = (tz_feature_version_req_t*)Parameters;
  tz_feature_version_rsp_t *SysCallRsp = (tz_feature_version_rsp_t*)Results;

  SysCallReq->feature_id = FeatureId;

  /* Make a SCM Sys call */
  Status = QcomScmProtocol->ScmSipSysCall (QcomScmProtocol,
                                           TZ_INFO_GET_FEATURE_VERSION_ID,
                                           TZ_INFO_GET_FEATURE_VERSION_ID_PARAM_ID,
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
    Status = EFI_DEVICE_ERROR;
    DEBUG(( EFI_D_ERROR, "TZ_INFO_GET_FEATURE_VERSION_ID failed, Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
    goto ErrorExit;
  }

  *Version = SysCallRsp->version;

ErrorExit:
  return Status;
}

/*
* Notify TZ where to load QSEE sec apps
*/
EFI_STATUS
QseeAppsRegionNotificationSyscall (
  UINT64 applications_region_addr,
  UINT64 applications_region_size
  )
{
   qsee_apps_region_notification_t *qsee_syscallp;
   UINT32 app_id = 0;
   EFI_STATUS              Status;
   UINT32 startTime = 0, endTime = 0;

   qsee_syscallp= (qsee_apps_region_notification_t*)AllocatePool(sizeof(qsee_apps_region_notification_t));
   if (NULL == qsee_syscallp)
   {
      return EFI_OUT_OF_RESOURCES;
   }
   SetMem(qsee_syscallp, sizeof(qsee_apps_region_notification_t), 0x0);
   qsee_syscallp->applications_region_addr = (UINT32)applications_region_addr;
   qsee_syscallp->applications_region_size = (UINT32)applications_region_size;

   startTime = GetTimerCountms();
   Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                            APP_REGION_NOTIFICATION_CMD,
                                            &(app_id),
                                            qsee_syscallp,
                                            sizeof(qsee_apps_region_notification_t),
                                            NULL,
                                            0);

   endTime = GetTimerCountms();
   DEBUG((EFI_D_WARN, "AppReg Init:%d mS\n", endTime-startTime));

   FreePool(qsee_syscallp);
   return Status;
}

EFI_STATUS
QseeRegisterLogBuffer()
{
  qsee_register_log_buffer_t *qsee_syscallp;
  UINT32 app_id = 0;
  EFI_STATUS              Status;
  UINT64 *log = NULL;

  qsee_syscallp= (qsee_register_log_buffer_t*)AllocatePool(sizeof(qsee_register_log_buffer_t));
  if (NULL == qsee_syscallp)
  {
     DEBUG((EFI_D_WARN, "Failed to allocate memory:(size=%d)\n", sizeof(qsee_register_log_buffer_t)));
     return EFI_OUT_OF_RESOURCES;
  }

  log = (UINT64 *)UncachedAllocatePool(QSEE_LOG_BUF_SIZE);
  if (NULL == log)
  {
     Status =  EFI_OUT_OF_RESOURCES;
     DEBUG ((EFI_D_WARN, "Failed to allocate uncached memory:(size=%d)\n", QSEE_LOG_BUF_SIZE));
     goto Exit;
  }

  SetMem(qsee_syscallp, sizeof(qsee_register_log_buffer_t), 0x0);
  qsee_syscallp->log_buffer_addr = (UINT32)log;
  qsee_syscallp->log_buffer_size = (UINT32)QSEE_LOG_BUF_SIZE;

  Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                           APP_REGISTER_LOG_BUF_COMMAND,
                                           &(app_id),
                                           qsee_syscallp,
                                           sizeof(qsee_register_log_buffer_t),
                                           NULL,
                                           0);

  if (Status != EFI_SUCCESS) 
  {
      DEBUG((EFI_D_WARN, "Failed to send register log buffer commnd: %d\n", Status));
      goto Exit;
  }

  log_buffer = log;
  Status = EFI_SUCCESS;

Exit:
  if(log)
    UncachedSafeFreePool(log);
  FreePool(qsee_syscallp);
  return Status;
}

/**
  PIL Loader's Entry point
  
  @param  ImageHandle       EFI_HANDLE.
  @param  SystemTable       EFI_SYSTEM_TABLE.

  @return EFI_SUCCESS       Success.
**/
EFI_STATUS
EFIAPI
TzeLoaderProtocolInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_HANDLE              handle = NULL;
  EFI_STATUS              Status;
  UINT32                  LoadSecAppsFlag = 0;
  UINT32                  configFlag =0;

  UncachedMem = (UINT32*)TzeUncachedAllocatePool (UNCACHED_MEM_DEFAULT_SIZE);
  if (UncachedMem == NULL)
    return EFI_OUT_OF_RESOURCES;
  UncachedMemSize = UNCACHED_MEM_DEFAULT_SIZE;

  Status = gBS->InstallMultipleProtocolInterfaces(
                              &handle, 
                              &gEfiTzeLoaderProtocolGuid, (void **)&TzeLoaderProtocol,
                              NULL);

  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  DEBUG((EFI_D_INFO, "Attempting to load SCM protocol\n"));
  Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&QcomScmProtocol);
  ASSERT_EFI_ERROR(Status);

  /* Check if qsee logs are enabled (in uefiplat.cfg file) */
  Status = GetConfigValue("SecurityFlag", &configFlag);
  if ((Status != EFI_SUCCESS) || ((configFlag & 0x100) != 0x100)) 
  {
    DEBUG((EFI_D_INFO, "Failed to get QseeLogEnableFlag, Status=%r\n", Status));
  }
  else 
  {
    Status = QseeRegisterLogBuffer();
    if (Status != EFI_SUCCESS)
    {
      DEBUG ((EFI_D_WARN, "QseeRegisterLogBuffer: Failed to register log buffer message %p\n", Status));
    }
  }

  Status = GetConfigValue ("SecurityFlag", &LoadSecAppsFlag);
  // Default to enabled if flag is not found
  if ((Status != EFI_SUCCESS) || ((LoadSecAppsFlag & 0x40) == 0x40))
  {
    Status = LoadSecureApps(&TzeLoaderProtocol, ImageHandle, SystemTable, NULL);
  }
  else 
  {
    DEBUG ((EFI_D_INFO, "LoadSecAppsFlag found, but is not set. Skipping loading of TAs.\n"));  
  }

  ASSERT_EFI_ERROR(Status);

  return Status;
}

int DisplayQseeLog(UINT32 startOffset, UINT32 endOffset)
{
  UINT32 MaxBufSize = 0;
  UINT32 LogBufSize = 0;
  UINT32 LogBufFirstHalf = 0;
  EFI_STATUS  ret = EFI_SUCCESS;
  void *pLogBuf = NULL;
  tzdbg_log_t *log = (tzdbg_log_t *)log_buffer;

  MaxBufSize = QSEE_LOG_BUF_SIZE - sizeof(struct tzdbg_log_pos_t);
  if (startOffset < endOffset)
  {
    LogBufSize = endOffset - startOffset;
    pLogBuf = AllocatePool(LogBufSize);
    if (NULL == pLogBuf)
    {
      ret = EFI_DEVICE_ERROR;
      DEBUG ((EFI_D_ERROR, "Failed to alloc buffer to print TZ Log:%u\n", LogBufSize));
      goto Exit;
    }
    SetMem(pLogBuf, LogBufSize, 0);
    memscpy(pLogBuf, LogBufSize, (void *)(log->log_buf + startOffset), LogBufSize);
  }
  else if ( endOffset < startOffset)
  {
    LogBufSize =  MaxBufSize - (startOffset - endOffset);
    LogBufFirstHalf = MaxBufSize - startOffset;
    pLogBuf = AllocatePool(LogBufSize);
    if (NULL == pLogBuf)
    {
      ret = EFI_DEVICE_ERROR;
      DEBUG ((EFI_D_ERROR, "Failed to alloc buffer to print TZ Log:%u\n", LogBufSize));
      goto Exit;
    }
    SetMem(pLogBuf, LogBufSize, 0);
    memscpy(pLogBuf, LogBufSize, (void *)(log->log_buf + startOffset), LogBufFirstHalf);
    memscpy((void *)(pLogBuf+ LogBufFirstHalf), (LogBufSize - LogBufFirstHalf), log->log_buf, endOffset);
  }
  else //endOffset == startOffset (no log is updated)
  {
    ret = EFI_SUCCESS;
    goto Exit;
  }

  SerialPortWrite((UINT8 *)pLogBuf, (UINTN)(LogBufSize));
  FreePool(pLogBuf);
Exit:
  return ret;
}

/*
* Load app using TZ qsee scm command
*/
UINT32
QseeAppStartSyscall (
  UINTN mdt_len,
  UINTN img_len,
  VOID *pa,
  CHAR8 img_name[QSEE_MAX_NAME_SIZE])
{
   qsee_app_start_req_t *qsee_syscallp;
   UINT32         app_id = 0;
   EFI_STATUS     Status;

   qsee_syscallp= (qsee_app_start_req_t *)AllocatePool(sizeof(qsee_app_start_req_t));
   if (NULL == qsee_syscallp)
   {
      return app_id;
   }

   SetMem(qsee_syscallp, sizeof(qsee_app_start_req_t), 0x0);
   qsee_syscallp->mdt_len = (UINT32)mdt_len;
   qsee_syscallp->img_len = (UINT32)img_len;
   qsee_syscallp->pa = (UINT32)(UINTN)pa;
   CopyMem((VOID *)qsee_syscallp->app_name, img_name, QSEE_MAX_NAME_SIZE);

   Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                            APP_START_APP_CMD,
                                            &(app_id),
                                            qsee_syscallp,
                                            sizeof(qsee_app_start_req_t),
                                            NULL,
                                            0);
   if (Status != EFI_SUCCESS)
   {
     app_id = 0;
   }
   FreePool(qsee_syscallp);
   return app_id;
}

EFI_STATUS
LoadImageFromBuffer(
      IN  EFI_TZE_LOADER_PROTOCOL     *This,
      IN  UINT32                      *Buffer,
      IN  UINTN                        BufferSize,
      OUT UINT32                      *AppIdPtr
    )
{

  EFI_STATUS          Status;
  UINT32             *MemLoc;
  UINT32              AppId = 0;


  if ((Buffer == NULL) || (BufferSize == 0))
    return EFI_INVALID_PARAMETER;

    //TZ1.4 App loading
      //Allocate uncahed buffer
    MemLoc = TzeUncachedAllocatePool (BufferSize);
    if (MemLoc == NULL)
      return EFI_OUT_OF_RESOURCES;

    CopyMem(MemLoc, Buffer, BufferSize);

    AppId = QseeAppStartSyscall(0, BufferSize, MemLoc, "");
    if(MemLoc)
      TzeUncachedFreePool (MemLoc);

  if (AppIdPtr)
     *AppIdPtr = AppId;

  if ((AppId == 0) || (AppId == 0xFFFFFFFF))   
      Status = EFI_LOAD_ERROR;
  else
      Status = EFI_SUCCESS;

  return Status;
  
}

EFI_STATUS
LoadImageFromFile(
      IN  EFI_TZE_LOADER_PROTOCOL      *This,
      IN  CHAR8*                       FilePath,
      OUT UINT32                       *AppIdPtr
    )
{
  EFI_STATUS        Status;
  EFI_OPEN_FILE     *File;
  VOID              *Address;
  UINTN             Size;

  if (FilePath == NULL)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (FilePath, EFI_FILE_MODE_READ, 0);
  if (File == NULL)
    return EFI_LOAD_ERROR;

  Status = EfiReadAllocatePool (File, &Address, &Size);
  if (EFI_ERROR (Status))
  {
    EfiClose (File);
    return Status;
  }

   Status= LoadImageFromBuffer(This, Address, Size, AppIdPtr);

  gBS->FreePool (Address);
  EfiClose (File);

  return Status;
}


EFI_STATUS
LoadImageFromPartition(
    IN  EFI_TZE_LOADER_PROTOCOL        *This,
    IN  EFI_GUID                       *pPartitionGuid,
    IN  UINT16                         *pPartitionLabel,
    OUT UINT32                         *AppIdPtr
    )
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT32                  *MemLoc;
  EFI_BLOCK_IO_PROTOCOL   *BlkIo;
  UINTN                   ImgSize;
  CHAR8                   img_name[QSEE_MAX_NAME_SIZE] = {0};
  UINT32                  AppId = 0;

  if ( (pPartitionLabel == NULL) && (pPartitionGuid ==NULL))
  {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "Load Image from partition label (%s)\n", pPartitionLabel));

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  if (pPartitionLabel != NULL) 
  {
    /* Ignore GUID, and proceed with partition label */
    Attribs |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
    HandleFilter.PartitionLabel = pPartitionLabel;
    HandleFilter.PartitionType = 0;
    HandleFilter.RootDeviceType = 0;
  }
  else
  {
    Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;
    HandleFilter.PartitionType = pPartitionGuid;
  }

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if (Status != EFI_SUCCESS)
    return Status;

  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1)
    return EFI_INVALID_PARAMETER;

  if (MaxHandles == 0)
  {
    DEBUG ((EFI_D_WARN, "Image partition label not found\n"));
    return EFI_NOT_FOUND;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  ImgSize = BlkIo->Media->LastBlock * BlkIo->Media->BlockSize;
  
  MemLoc = TzeUncachedAllocatePool(ImgSize);
  if (MemLoc == NULL)
    return EFI_OUT_OF_RESOURCES;

  do
  {
    Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, ImgSize, MemLoc);
    if (EFI_ERROR (Status))
      break;

      AppId = QseeAppStartSyscall(0, ImgSize, MemLoc, img_name);

    if (AppIdPtr)
      *AppIdPtr = AppId;

    if ((AppId == 0) || (AppId == 0xFFFFFFFF))   
      Status = EFI_LOAD_ERROR;
    else
      Status = EFI_SUCCESS;
  }
  while (0);

  if(MemLoc)
    TzeUncachedFreePool (MemLoc);

  return Status;
}

EFI_STATUS
LoadImageFromPartitionLabel(
    IN  EFI_TZE_LOADER_PROTOCOL        *This,
    IN  UINT16                         *pPartitionLabel,
    OUT UINT32                         *AppIdPtr
    )
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT32                  *MemLoc;
  EFI_BLOCK_IO_PROTOCOL   *BlkIo;
  UINTN                   ImgSize;

  if ( pPartitionLabel == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG ((EFI_D_INFO, "Load Image from partition label (%s)\n", pPartitionLabel));

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_LABEL;
  Attribs |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.PartitionLabel = pPartitionLabel;
  HandleFilter.VolumeName = 0;
  HandleFilter.RootDeviceType = 0;
  HandleFilter.PartitionType = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);

  if ( EFI_ERROR(Status) )
  {
    DEBUG((EFI_D_WARN, "Failed to get handle (label = %s)\n", pPartitionLabel));
    return Status;
  }

  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1)
  {
    DEBUG ((EFI_D_WARN, "Ambiguity in Image partition\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (MaxHandles == 0)
  {
    DEBUG ((EFI_D_WARN, "Image partition label not found\n"));
    return EFI_NOT_FOUND;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  ImgSize = BlkIo->Media->LastBlock * BlkIo->Media->BlockSize;
  
  MemLoc = TzeUncachedAllocatePool(ImgSize);
  if (MemLoc == NULL)
    return EFI_OUT_OF_RESOURCES;
  
  do
  {
    Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, ImgSize, MemLoc);
    if (EFI_ERROR (Status))
      break;

    Status = QseeLoadServiceImageSyscall(0, ImgSize, MemLoc);
  }
  while (0);

  if(MemLoc)
    TzeUncachedFreePool (MemLoc);

  return Status;
}

static EFI_STATUS
LoadImageFromPartitionUsingGuid(
  IN  EFI_TZE_LOADER_PROTOCOL        *This,
  IN  EFI_GUID                       *pPartitionGuid,
  OUT VOID                           **MemLocPtr,
  OUT UINTN                          *ImgSizePtr
  )
{
  UINT32                  Attribs = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT32                  *MemLoc = NULL;
  EFI_BLOCK_IO_PROTOCOL   *BlkIo = NULL;
  UINTN                   ImgSize = 0;

  /* Select the BlkIo handle that represents the partition by the
   * referenced GUID type in GPT partition on Non removable media
   * ie eMMC device */
  Attribs |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attribs |= BLK_IO_SEL_PARTITIONED_GPT;
  Attribs |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;
  Attribs |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;

  if(!pPartitionGuid)
    return EFI_INVALID_PARAMETER;

  HandleFilter.PartitionType = pPartitionGuid;

  MaxHandles = sizeof(HandleInfoList)/sizeof(*HandleInfoList);
  HandleFilter.VolumeName = 0;

  Status = GetBlkIOHandles (Attribs, &HandleFilter, HandleInfoList, &MaxHandles);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_WARN, "GetBlkIOHandles failed Status: %d\n", Status));
    goto ErrorExit;
  }

  /* Having more than one partition is considered ambiguity, so return
   * invalid param */
  if (MaxHandles > 1)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  if (MaxHandles == 0)
  {
    DEBUG ((EFI_D_WARN, "Image partition label not found\n"));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  /* Get the Block IO protocol instance from the handle */
  BlkIo = HandleInfoList[0].BlkIo;

  /* Get the partition size and round it up to EFI_PAGE_SIZE */
  ImgSize = BlkIo->Media->LastBlock * BlkIo->Media->BlockSize;

  MemLoc = TzeUncachedAllocatePool (ImgSize);
  if (MemLoc == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = BlkIo->ReadBlocks (BlkIo, BlkIo->Media->MediaId, 0, ImgSize, MemLoc);
  if (EFI_ERROR (Status))
    goto ErrorExit;

  if(MemLocPtr)
    *MemLocPtr = MemLoc;
  if(ImgSizePtr)
    *ImgSizePtr = ImgSize;

  Status = EFI_SUCCESS;

ErrorExit:
  if(Status && MemLoc)
  {
    TzeUncachedFreePool(MemLoc);
    if(MemLocPtr)
      *MemLocPtr = NULL;
    if(ImgSizePtr)
      *ImgSizePtr = 0;
  }
  return Status;
}
/**  
  @param[in]  This           Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionGuid GUID of the partition that contains the TZExec 
                             application image. The partition is searched only 
                             in the nonremovable device's GUID partition table.
  @param[in]  Path           Pointer to the path where the image is located.
  @param[out] MemLoc         Pointer to buffer where the image is loaded in ns El1.
  @param[out] ReadFileSize   Size of buffer where the image is loaded.                            
  @return EFI_SUCCESS if successful
**/
static EFI_STATUS LoadSecureImageFromFileSystemHelper(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   IN  CHAR16                         *Path,
   OUT VOID                           **MemLocation,
   OUT UINTN                          *FileSizePtr
)
{
  UINT32                  Attrib = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT8                   *CfgBuffPtr = NULL;
  VOID                    *MemLoc=NULL;
  UINTN                   ReadFileSize=0; 

  
  if( (pPartitionGuid == NULL) || (Path == NULL)) 
    return EFI_INVALID_PARAMETER;

  Attrib |= BLK_IO_SEL_PARTITIONED_GPT;
  Attrib |= BLK_IO_SEL_MEDIA_TYPE_NON_REMOVABLE;
  Attrib |= BLK_IO_SEL_MATCH_ROOT_DEVICE;
  Attrib |= BLK_IO_SEL_MATCH_PARTITION_TYPE_GUID;
  Attrib |= BLK_IO_SEL_SELECT_MOUNTED_FILESYSTEM;

  if (boot_from_emmc())
  {
  // Attempt to load from GPP Partition
  Status = LoadFileFromPartition(Attrib, Path, &gEfiEmmcGppPartition1Guid, pPartitionGuid, NULL, &CfgBuffPtr,
                                 &ReadFileSize);
  // If failed attempt to load from User Partition
  if (Status != EFI_SUCCESS)
    Status = LoadFileFromPartition(Attrib, Path, &gEfiEmmcUserPartitionGuid, pPartitionGuid, NULL, &CfgBuffPtr,
                                 &ReadFileSize);
    if (EFI_ERROR (Status))
    {
    DEBUG ((EFI_D_INFO, "LoadSecureImageFromFileSystem: LoadFileFromPartition failed for %s, status: %p\n", Path, Status));
    return Status;
  }
  }
  else if (boot_from_ufs())
  {
    // Attempt to load from UFS LUN 4
    Status = LoadFileFromPartition(Attrib, Path, &gEfiUfsLU4Guid, pPartitionGuid, NULL, &CfgBuffPtr,
                                 &ReadFileSize);
    if (EFI_ERROR (Status)) 
    {
      DEBUG ((EFI_D_INFO, "LoadSecureImageFromFileSystem: LoadFileFromPartition failed for %s, status: %p\n", Path, Status));
      return Status;
    }
  }
  else
  {
    DEBUG ((EFI_D_ERROR, "LoadSecureImageFromFileSystem: Invalid boot device !\n"));
    return EFI_LOAD_ERROR;
  }

  MemLoc = TzeUncachedAllocatePool(ReadFileSize);
  if (MemLoc == NULL)
    return EFI_OUT_OF_RESOURCES;

  CopyMem((void*)MemLoc, CfgBuffPtr, ReadFileSize);  
  *FileSizePtr = ReadFileSize;
  *MemLocation=MemLoc;
  FreePool(CfgBuffPtr);

  return Status;
}

EFI_STATUS LoadSecureImageFromFileSystem
(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   CHAR16                             *Path,
   IN BOOLEAN                         ServicesMbn,
   OUT UINT32                         *AppIdPtr
)
{
  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *MemLoc=NULL;
  UINTN                   ReadFileSize=0;
  UINT32                  AppId = 0;

  if(!Path)
  {
    Status = LoadImageFromPartitionUsingGuid(This, pPartitionGuid, &MemLoc, &ReadFileSize);
    if(Status)
    {
      DEBUG ((EFI_D_ERROR, "LoadImageFromPartitionUsingGuid Failed: %d\n", Status));
      goto ErrorExit;
    }
  }
  else
  {
    Status = LoadSecureImageFromFileSystemHelper(This,pPartitionGuid,Path, &MemLoc, &ReadFileSize);
    if(Status)
    {
      DEBUG ((EFI_D_ERROR, "LoadSecureImageFromFileSystemHelper Failed: %d\n", Status));
      goto ErrorExit;
    }
  }

  if (ServicesMbn == FALSE)
  { //normal app load
    AppId = QseeAppStartSyscall(0, ReadFileSize,MemLoc, "");
    if ((AppId == 0) || (AppId == 0xFFFFFFFF))
    {
      DEBUG ((EFI_D_ERROR, "QseeLoadServiceImageSyscall Failed\n"));
      Status = EFI_LOAD_ERROR;
      goto ErrorExit;
    }
    else
    {
      Status = EFI_SUCCESS;
      if (AppIdPtr)
        *AppIdPtr = AppId;
      goto ErrorExit;
    }
  }
  else
  {
    Status = QseeLoadServiceImageSyscall(0, ReadFileSize, MemLoc);
    if(Status)
    {
      DEBUG ((EFI_D_ERROR, "QseeLoadServiceImageSyscall Failed\n"));
      goto ErrorExit;
    }
  }

  Status = EFI_SUCCESS;

ErrorExit:
  if (MemLoc)
    TzeUncachedFreePool (MemLoc);

  return Status;
}

/**
  Loads image from a partition specified by GUID Partition Type GUID 
  and Path
  
  @param  Path            Directory containging ACPI table files
  @param  pPartitionGuid  GUID eg:  TzAppsPartitionType
  @param AppIdPtr         out  if success holds app id on return
   
  @return EFI_SUCCESS if successful
**/
 
EFI_STATUS
LoadImageFromFileSystem(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   CHAR16                             *Path,
   OUT UINT32                         *AppIdPtr

)
{
 return LoadSecureImageFromFileSystem(This, pPartitionGuid, Path,
                              FALSE, AppIdPtr);
}

 /**
  Loads external images, based on type of image in secure user 
  space.
  
  @param[in]  This           Pointer to the EFI_TZE_LOADER_PROTOCOL instance.
  @param[in]  pPartitionGuid GUID of the partition that contains the TZExec 
                             application image. The partition is searched only 
                             in the nonremovable device's GUID partition table.
  @param[in]  Path           Pointer to the path where the image is located.
  @param[in]  image_type     Enum image_type to choose type of syscall to be executed to
                             load the image.
  @param[out] Buffer         Pointer to buffer where the image is loaded in ns El1.
  @param[out] BufferSize     Size of buffer where the image is loaded.                            
  @param[out] AppIdPtr       Application ID pointer.
  @return EFI_SUCCESS if successful
**/
EFI_STATUS
LoadExternalImageFromFileSystem(
   IN  EFI_TZE_LOADER_PROTOCOL        *This,
   IN  EFI_GUID                       *pPartitionGuid,
   IN  CHAR16                         *Path,
   IN  tz_load_image_type             ImageType,
   IN VOID                            *Buffer,
   OUT UINTN                          *BufferSize,
   OUT UINT32                         *AppIdPtr
)
{

  EFI_STATUS              Status = EFI_SUCCESS;
  VOID                    *MemLoc=NULL;
  UINTN                   ReadFileSize=0;
  UINT32                  AppId = 0;
  
  Status= LoadSecureImageFromFileSystemHelper(This,pPartitionGuid,Path, &MemLoc, &ReadFileSize );
  if(Status)
  {
    DEBUG ((EFI_D_ERROR, "LoadImageFromPartitionUsingGuid Failed: %d\n", Status));
    return Status;
  }

  if (ImageType == APP_IMAGE)
  { //normal app load
    AppId = QseeAppStartSyscall(0, ReadFileSize,MemLoc, "");
    if (AppIdPtr)
      *AppIdPtr = AppId;
    if ((AppId == 0) || (AppId == 0xFFFFFFFF))   
      Status = EFI_LOAD_ERROR;
    else
      Status = EFI_SUCCESS;
  } 
  else if (ImageType == EXTERNAL_IMAGE)
  {
    Status =QseeLoadExternalImageSyscall(0, ReadFileSize,MemLoc);
  }
  else if (ImageType == SERVICE_IMAGE)
  {
    Status = QseeLoadServiceImageSyscall(0, ReadFileSize, MemLoc);
  }
  else if(ImageType == LOAD_IMAGE )
  { 
   if (Buffer)  
    {
        CopyMem(Buffer, MemLoc, ReadFileSize); 
        *BufferSize = ReadFileSize;  
    }
   else{
        *BufferSize = ReadFileSize;  
   }
  }
  else
  {
   DEBUG ((EFI_D_INFO, "LoadExternalImageFromFileSystem:Invalid tz load image type"));
   Status = EFI_LOAD_ERROR;
   return Status;
  }

  if (Status != 0) 
  {
      Status = EFI_LOAD_ERROR;
  }
  else
  {
      Status = EFI_SUCCESS; 
  }
  if(MemLoc != NULL)
  {
    TzeUncachedFreePool (MemLoc);
  }
  return Status;
}

