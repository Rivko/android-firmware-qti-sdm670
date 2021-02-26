/** @file QseeComDxe.c
   
  QseeComDxe.c provides QSEECOM interface in uefi environment. 

  Copyright (c) 2012-2017 Copyright Qualcomm Technologies, Inc. 
  All Rights Reserved. Qualcomm Technologies Proprietary and 
  Confidential. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/24/17   ys     Added new API to QSEECOM protocol
 08/01/16   rj     Added functionality for showing qsee logs (tz apps logs)
 07/12/16   rj     Initial revision
 =============================================================================*/

#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIQseecom.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFITzeLoader.h>
#include <Include/scm_qsee_interface.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SerialPortLib.h>
#include <Include/Library/MDPPlatformLib.h>
#include "QcomLib.h"
#include "LoaderUtils.h"
#include "QseeComDxe.h"

extern QCOM_SCM_PROTOCOL      *QcomScmProtocol;
extern UINT64 *log_buffer;

#define SUFFIX_64BIT_TA ("64")
#define SUFFIX_SIZE_64BIT_TA (2)
// Including NULL
#define SUFFIX_SIZE (3)
#define TZ_FVER_QSEE                         10 /**< QSEE application layer. */

#define TZBSP_computed_VERSION(major, minor, patch) \
  (((major & 0x3FF) << 22) | ((minor & 0x3FF) << 12) | (patch & 0xFFF))

#define COMMON_LIB_LABEL                     L"cmnlib"
#define COMMON_LIB_64_LABEL                  L"cmnlib64"
#define KEYMASTER_MBN_LABEL                  L"keymaster"

//Fat16 partition; @TODO: move all this once tested
STATIC EFI_GUID KeyMasterPartitionGuid =
  { 0xA11D2A7C, 0xD82A, 0x4C2F, { 0x8A, 0x01, 0x18, 0x05, 0x24, 0x0E, 0x66, 0x26 } };
STATIC EFI_GUID CommonLibPartitionGuid =
  { 0x73471795, 0xAB54, 0x43F9, { 0xA8, 0x47, 0x4F, 0x72, 0xEA, 0x5C, 0xBE, 0xF5 } };
//8EA64893-1267-4A1B-947C-7C362ACAAD2C
STATIC EFI_GUID CommonLib64PartitionGuid =
  { 0x8EA64893, 0x1267, 0x4A1B, { 0x94, 0x7C, 0x7C, 0x36, 0x2A, 0xCA, 0xAD, 0x2C } };

BOOLEAN TZSecureAppLoadCalled = FALSE;
BOOLEAN Flag = FALSE;

extern
EFI_STATUS
LoadImageFromPartition(
    IN  EFI_TZE_LOADER_PROTOCOL        *This,
    IN  EFI_GUID                       *pPartitionGuid,
    IN  UINT16                         *pPartitionLabel,
    OUT UINT32                         *AppIdPtr
 );

UINT32 QueryAppId(CHAR8 *AppName)
{
  EFI_STATUS    Status;
  UINT32        AppId = 0;

  Status = QcomScmProtocol->ScmSendCommand(
                                             QcomScmProtocol, 
                                             APP_QUERY_APPID_CMD, 
                                             &AppId,
                                             AppName,
                                             AsciiStrnLenS(AppName, QSEE_MAX_NAME_SIZE),
                                             NULL,
                                             0
                                            );
  if (EFI_ERROR(Status)) 
  {
    DEBUG ((EFI_D_INFO, "QueryAppId for (%a) Failed: Status =  %d\n", AppName, Status));
    AppId = 0;
    goto Exit;
  }

Exit:
  return AppId;
}

static EFI_STATUS
LoadSecureAppsLAHelper(
  IN  EFI_TZE_LOADER_PROTOCOL        	*This
)
{
  UINT64                  MemBase = 0;
  UINT64                  MemSize = 0;
  EFI_STATUS              Status = EFI_SUCCESS;
  UINT32                  AppId = 0;
  UINT32                  version = 0;
  UINT32                  computed_version = 0;
  UINT32                  CommonMbnLoadFlag = 0;
  UINT32                  LoadKeymasterFlag = 0;
  EFI_CHIPINFO_PROTOCOL   *hChipInfoProtocol;
  MDPPlatformInfo         sPlatformInfo;

  if (TZSecureAppLoadCalled == TRUE)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  Status = TzGetFeatureVersion(TZ_FVER_QSEE, &version);
  if(EFI_ERROR(Status))
  {
    goto ErrorExit;
  }

  computed_version = TZBSP_computed_VERSION (1,2,0);
  if (version >= computed_version)
  {
    Status = GetConfigValue64 ("TzAppsRegnAddr", &MemBase);
    if (Status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_INFO, "LoadSecureApps: Unable to find TZApps region addr in config file\n"));
        ASSERT (Status == EFI_SUCCESS);
    }

    Status = GetConfigValue64 ("TzAppsRegnSize", &MemSize);
    if (Status != EFI_SUCCESS)
    {
        DEBUG ((EFI_D_INFO, "LoadSecureApps: Unable to find TZApps region size in config file\n"));
        ASSERT (Status == EFI_SUCCESS);
    }

	if (EFI_SUCCESS == gBS->LocateProtocol(&gEfiChipInfoProtocolGuid,NULL,(VOID **)&hChipInfoProtocol))
	{
		if (EFI_SUCCESS != hChipInfoProtocol->GetChipId(hChipInfoProtocol,&sPlatformInfo.sEFIChipSetId))
		{
			DEBUG((EFI_D_WARN, "QseecomDxe: GetChipId() failed. \n"));
		}

		if (EFICHIPINFO_ID_QCS605 == sPlatformInfo.sEFIChipSetId)
		{
			MemSize = 0x800000;
		}
    }

    Status = QseeAppsRegionNotificationSyscall(MemBase, MemSize);
    ASSERT_EFI_ERROR(Status);
  }

  TZSecureAppLoadCalled = TRUE;

  computed_version = TZBSP_computed_VERSION (1,1,0);
  if (version >= computed_version)
  {
    Status = GetConfigValue ("SecurityFlag", &CommonMbnLoadFlag);        // Get this flag from uefiplat.cfg. Depending on the flag, process call to this function
    if ((Status != EFI_SUCCESS) || ((CommonMbnLoadFlag & 0x4) != 0x4))
    {
      DEBUG ((EFI_D_INFO, "CommonMbnLoadFlag not found in uefiplat.cfg. Defaulting to 0.\r\n"));
      // Default to 0. Does not matter what the flag is
      CommonMbnLoadFlag = 0;
      Status = EFI_SUCCESS;   // Continue for Hawker
    }
    else
    {
      Status = LoadSecureImageFromFileSystem (This,
                                       &CommonLibPartitionGuid,
                                       NULL,
                                       TRUE,
                                       &AppId);
      if (Status != EFI_SUCCESS)
      {
          DEBUG ((EFI_D_WARN, "Unable to load cmnlib Status:%d\r\n", Status));
          Status = LoadImageFromPartitionLabel (This,
                                                COMMON_LIB_LABEL,
                                                &AppId);
          if(Status != EFI_SUCCESS)
          {
            DEBUG ((EFI_D_WARN, "Unable to load cmnlib using label Status:%d\r\n", Status));
            ASSERT_EFI_ERROR(Status);
          }
          //gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      }


      Status = LoadSecureImageFromFileSystem (This,
                                       &CommonLib64PartitionGuid,
                                       NULL,
                                       TRUE,
                                       &AppId);
      if (Status != EFI_SUCCESS)
      {
        DEBUG ((EFI_D_WARN, "Unable to load cmnlib64 Status:%d\r\n", Status));
        Status = LoadImageFromPartitionLabel (This,
                                              COMMON_LIB_64_LABEL,
                                              &AppId);
        if(Status != EFI_SUCCESS)
        {
          DEBUG ((EFI_D_WARN, "Unable to load cmnlib using label Status:%d\r\n", Status));
          ASSERT_EFI_ERROR(Status);
        }
        //gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      }
    }
  }

  Status = GetConfigValue ("SecurityFlag", &LoadKeymasterFlag);        // Get this flag from uefiplat.cfg. Depending on the flag, process call to this function
  if ((Status != EFI_SUCCESS) || ((LoadKeymasterFlag & 0x80) != 0x80))
  {
    DEBUG ((EFI_D_INFO, "KeymasterFlag not found in uefiplat.cfg. Defaulting to 0.\r\n"));
    LoadKeymasterFlag = 0;
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = LoadSecureImageFromFileSystem (This,
                                     &KeyMasterPartitionGuid,
                                     NULL,
                                     FALSE,
                                     &AppId);
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "Keymaster failed to load! Status = 0x%x\r\n", Status));
      Status = LoadImageFromPartition (This,
                                       &KeyMasterPartitionGuid,
                                       KEYMASTER_MBN_LABEL,
                                       &AppId);
      if(Status != EFI_SUCCESS)
      {
        DEBUG ((EFI_D_WARN, "Unable to load Keymaster using label Status:%d\r\n", Status));
        ASSERT_EFI_ERROR(Status);
      }
      //gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    }
    DEBUG ((EFI_D_ERROR, "LoadSecureApps: Load app from partition(%s): Status = 0x%p, AppId = %d\r\n",
                                               KEYMASTER_MBN_LABEL, Status, AppId));
  }

  Status = EFI_SUCCESS;

ErrorExit:
  return Status;
}


/** 
  This function ?????
   
  @param[in/out]    .

  @return  EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
EFI_QseecomStartApp(
  IN     QCOM_QSEECOM_PROTOCOL      *This, 
  IN     CHAR8                       *AppName,
  OUT    UINT32                   *handleId
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 AppId = 0;
  UINT16 *AppNameL = NULL;

  DEBUG ((EFI_D_INFO, "EFI_QseecomStartApp: AppName %a\n", AppName));

  if(!Flag) {
    Status = LoadSecureAppsLAHelper((EFI_TZE_LOADER_PROTOCOL*)This);
    if(Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_WARN, "LoadSecureAppsLAHelper: Failed to get LoadSecureAppsLAHelper, Status = %d\r\n", Status));
      return Status;
    }
    Flag = TRUE;	
  }

  /* Check all the input parameters */
  if(AppName == NULL || This ==NULL || (AsciiStrnLenS(AppName, QSEE_MAX_NAME_SIZE) >= QSEE_MAX_NAME_SIZE))
  {
    DEBUG ((EFI_D_WARN, "EFI_QseecomStartApp: Invalid Parameters AppName: %a\n", AppName));
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  /* Validate if SCM protocol is initialized */
  if(QcomScmProtocol == NULL )
  {
    Status = EFI_LOAD_ERROR;
    return Status;
  }

  /* Querry if app is already loaded */
  AppId = QueryAppId(AppName);
  if (AppId != 0)
  {
      *handleId = AppId;
      return Status;
  }

  if(strlen(AppName) < (QSEE_MAX_NAME_SIZE - SUFFIX_SIZE)) {
      CHAR8 AppName64[QSEE_MAX_NAME_SIZE];

      memset(AppName64, 0, QSEE_MAX_NAME_SIZE);
      memscpy(AppName64, QSEE_MAX_NAME_SIZE, AppName, strlen(AppName));
      memscpy(AppName64 + strlen(AppName), (QSEE_MAX_NAME_SIZE - strlen(AppName) - 1),
            SUFFIX_64BIT_TA, SUFFIX_SIZE_64BIT_TA);
      AppId = QueryAppId(AppName64);
      if (AppId != 0)
      {
          *handleId = AppId;
          return Status;
      }
  }

  AppNameL = AllocatePool(QSEE_MAX_NAME_SIZE * sizeof(UINT16));
  if(!AppNameL)
  {
    DEBUG ((EFI_D_ERROR, "AppNameL allocation failed"));
    return EFI_DEVICE_ERROR;
  }
  AsciiStrToUnicodeStr(AppName, AppNameL);

  /* Load the app from partion */
  Status = LoadImageFromPartition (( EFI_TZE_LOADER_PROTOCOL*)This,
                                     NULL,
                                     AppNameL,
                                     &AppId);
  if(EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_WARN, "EFI_QseecomStartApp: Load from partition (%a)Failed:  Status(0x%p), appId(%d)\n", AppName, Status, AppId));
    AppId = 0;
  }
  *handleId = AppId;
  FreePool(AppNameL);
  return Status;
}

EFI_STATUS
EFI_QseecomStartAppByGuid(
  IN     QCOM_QSEECOM_PROTOCOL      *This, 
  IN     EFI_GUID                  * EfiGuid,
  OUT    UINT32                   *handleId
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 AppId = 0;


  /* Check all the input parameters */
  if( This ==NULL)
  {
    DEBUG ((EFI_D_WARN, "EFI_QseecomStartApp: Invalid Parameters\n"));
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }

  /* Validate if SCM protocol is initialized */
  if(QcomScmProtocol == NULL )
  {
    Status = EFI_LOAD_ERROR;
    return Status;
  }

  /* Load the app from partion */
  Status = LoadImageFromFileSystem (( EFI_TZE_LOADER_PROTOCOL*)This,
                                     EfiGuid,
                                     NULL,
                                     &AppId);
  if(EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_WARN, "EFI_QseecomStartApp: Load Failed:  Status(0x%p)\n", Status));
    AppId = 0;
  }
  *handleId = AppId;
  return Status;
}

/** 
  This function ?????
   
  @param[in/out]    .

  @return  EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
EFI_QseecomShutdownApp(
  IN     QCOM_QSEECOM_PROTOCOL      *This, 
  IN    UINT32                   HandleId
)
{
  EFI_STATUS Status;

  /* Validate if SCM protocol is initialized */
  if(QcomScmProtocol == NULL )
  {
    Status = EFI_LOAD_ERROR;
    return Status;
  }

  Status = QcomScmProtocol->ScmSendCommand(QcomScmProtocol,
                                       APP_SHUTDOWN_CMD,
                                       &HandleId,
                                       NULL,
                                       0,
                                       NULL,
                                       0);
  return Status;
}


/** 
  This function ?????
   
  @param[in/out]    .

  @return  EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
EFI_QseecomSendCmd(
  IN     QCOM_QSEECOM_PROTOCOL      *This, 
  IN     UINT32                  HandleId,
  IN     UINT8                   *send_buf,
  IN     UINT32                  sbuf_len,
  IN OUT UINT8                   *rsp_buf,
  IN     UINT32                  rbuf_len
)
{
  EFI_STATUS Status;

  /* Validate if SCM protocol is initialized */
  if(QcomScmProtocol == NULL )
  {
    Status = EFI_LOAD_ERROR;
    return Status;
  }

  Status = QcomScmProtocol->ScmSendCommand(  QcomScmProtocol, 
                                             APP_SEND_DATA_CMD, 
                                             &HandleId,
                                             send_buf,
                                             sbuf_len,
                                             rsp_buf,
                                             rbuf_len
                                            );
  return Status;
}

static QCOM_QSEECOM_PROTOCOL QcomQseecomProtocolImpl = 
{
  QCOM_QSEECOM_PROTOCOL_REVISION,
  EFI_QseecomStartApp,
  EFI_QseecomShutdownApp,
  EFI_QseecomSendCmd,
  EFI_QseecomStartAppByGuid
};


// ScmQseeCom protocol implementation
// Protocol initialization
EFI_STATUS
QseeComProtocolInit (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
  EFI_STATUS              Status;

  /* Validate if SCM protocol is initialized (Dependency)*/
  if(QcomScmProtocol == NULL )
  {
    Status = EFI_LOAD_ERROR;
    return Status;
  }

  // publish protocol
  Status = gBS->InstallMultipleProtocolInterfaces(
           &ImageHandle,
           &gQcomQseecomProtocolGuid,
           (void **)&QcomQseecomProtocolImpl,
           NULL);
    
  return Status;
}

