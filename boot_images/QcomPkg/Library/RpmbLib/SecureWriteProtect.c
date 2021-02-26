/** @file SecureWriteProtect.c
 *
 * This file provides routines for communicating with the 
 * storage secure application
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc. 
 * All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     ---------------------------------------------------------
  2017-09-11   jt      Optimizations/updates for app loading 
  2016-08-29   jt      Initial Version

 =============================================================================*/

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiCfgLib.h>
#include <Protocol/EFIQseecom.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFITzeLoader.h>
#include "StorSecApp.h"
 
static UINT8 *ScmBuffer = NULL; 
static QCOM_QSEECOM_PROTOCOL *QseecomProtocol = NULL; 
static QCOM_SCM_PROTOCOL *ScmProtocol = NULL;
static EFI_TZE_LOADER_PROTOCOL *TzeLoaderProtocol = NULL; 
static CHAR8* StorSecAppName = "qcom.tz.storsecapp";
static UINT32 StorSecAppId = 0;

STATIC EFI_GUID TzAppsPartitionGuid =
	  { 0x14D11C40, 0x2A3D, 0x4F97, { 0x88, 0x2D, 0x10, 0x3A, 0x1E, 0xC0, 0x93, 0x33 } };

/**
  This function sends requests to the storage secure application 
**/
EFI_STATUS
SendStorSecAppRequest(
   VOID *Req, 
   UINT32 ReqLen, 
   VOID *Resp, 
   UINT32 RespLen)
{
   CHAR8 *OsTypeString = NULL; 
   EFI_STATUS Status = EFI_SUCCESS; 
   UINTN OsTypeStringLen = MAX_OS_TYPE_LEN; 

   if ((NULL == Req) || (NULL == Resp) || (0 == ReqLen) || (0 == RespLen))
   {
      return EFI_INVALID_PARAMETER; 
   }

   if (NULL == ScmProtocol) 
   {
      Status = gBS->LocateProtocol (&gQcomScmProtocolGuid, NULL, (VOID**)&ScmProtocol);
      if (EFI_SUCCESS != Status)
      { 
         DEBUG ((EFI_D_ERROR, "Locate SCM protocol failed with return value of 0x%x\n", Status));
         return Status; 
      }
   }

   if (0 == StorSecAppId) 
   {
      OsTypeString = AllocatePool (OsTypeStringLen * sizeof(CHAR8));
      if (NULL == OsTypeString)
      {
         DEBUG ((EFI_D_ERROR, "Failed to allocate OsTypeString\n"));
         return EFI_OUT_OF_RESOURCES; 
      }

      Status = GetConfigString ("OsTypeString", OsTypeString, &OsTypeStringLen); 
      if ((EFI_SUCCESS != Status) || (0 == OsTypeStringLen))
      {
         DEBUG ((EFI_D_ERROR, "Error reading OsTypeString\n")); 
         goto Finish;   
      }

      /* Choose the correct way to load the application based on the OS type */
      if (0 == AsciiStrCmp(OsTypeString, "WP"))
      {
         Status = ScmProtocol->ScmSendCommand(ScmProtocol, APP_QUERY_APPID_CMD,
                                              &StorSecAppId, StorSecAppName,
                                              AsciiStrSize(StorSecAppName), NULL, 0);
         if (EFI_SUCCESS == Status)
         {
            goto SendRequest; 
         }
         else
         {
            if (NULL == TzeLoaderProtocol)
            {
               Status = gBS->LocateProtocol (&gEfiTzeLoaderProtocolGuid, NULL, (VOID **)&TzeLoaderProtocol); 
               if (EFI_SUCCESS != Status)
               { 
                  DEBUG ((EFI_D_ERROR, "Locate TzeLoader protocol failed with return value of 0x%x\n", Status));
                  goto Finish;  
               }
            }

            Status = TzeLoaderProtocol->LoadImageFromFileSystem(TzeLoaderProtocol, &TzAppsPartitionGuid, 
                                                                STORSECAPP_MBN_FILE, &StorSecAppId);
            if (EFI_SUCCESS != Status)
            { 
               DEBUG ((EFI_D_ERROR, "Loading storsec from TZAPPS failed with return value of 0x%x\n", Status));
               goto Finish;  
            }
         }
      }
      else if (0 == AsciiStrCmp(OsTypeString, "LA"))
      {
         if (NULL == QseecomProtocol)
         {
            Status = gBS->LocateProtocol (&gQcomQseecomProtocolGuid, NULL, (VOID **)&QseecomProtocol); 
            if (EFI_SUCCESS != Status)
            { 
               DEBUG ((EFI_D_ERROR, "Locate Qseecom protocol failed with return value of 0x%x\n", Status));
               goto Finish;  
            }
         }

         Status = QseecomProtocol->QseecomStartApp(QseecomProtocol, STORSECAPP_NAME, &StorSecAppId);
         if (EFI_SUCCESS != Status)
         {
            DEBUG ((EFI_D_ERROR, "Loading storsec failed with return value of 0x%x\n", Status));
            goto Finish;  
         }
      }
      else
      {
         DEBUG ((EFI_D_ERROR, "Invalid OsTypeString\n")); 
         Status = EFI_INVALID_PARAMETER; 
         goto Finish; 
      }
   }      

SendRequest: 
   /* Send request to the storage secure application */
   Status = ScmProtocol->ScmSendCommand(ScmProtocol,
                                        APP_SEND_DATA_CMD,
                                        &StorSecAppId,
                                        Req,
                                        ReqLen,
                                        Resp,
                                        RespLen);
   if (EFI_SUCCESS != Status)
   {
      DEBUG((EFI_D_ERROR, "ScmSendCommand failed, Status = 0x%x\n", Status)); 
   }

Finish: 
   if (NULL != OsTypeString)
   {
      FreePool(OsTypeString); 
   }
   return Status; 
}

/**
  Gets the Secure Write Protect Configuration Block 
**/
EFI_STATUS UFSGetSecureWpConfigBlock( 
   SdMgrSecureWpInfo *ConfigBlk
)
{
   EFI_STATUS Status; 
   SdMgrSecureWpInfo *WpInfo; 
   TzSvcSecureWpReq *Req; 
   TzSvcSecureWpRsp *Resp;  
   UINT32 ReqLength = sizeof(TzSvcSecureWpReq) + sizeof(SdMgrSecureWpInfo); 
   UINT32 RespLength = sizeof(TzSvcSecureWpRsp) + sizeof(SdMgrSecureWpInfo);

   if (NULL == ConfigBlk)
   {
      return EFI_INVALID_PARAMETER; 
   }

   if (NULL == ScmBuffer)
   {
      ScmBuffer = AllocateAlignedPages((SCM_BUFFER_SIZE / EFI_PAGE_SIZE), 
                                       EFI_PAGE_SIZE);
      if (NULL == ScmBuffer) 
      {
         DEBUG ((EFI_D_ERROR, "Failed to allocate SCM buffer\n")); 
         return EFI_DEVICE_ERROR; 
      }
   }

   /* Create request */
   Req = (TzSvcSecureWpReq *)ScmBuffer; 
   SetMem(Req, ReqLength, 0); 
   Req->CommandId = CMD_GET_SECURE_WP_CONFIG; 
   Req->DataSize = sizeof(SdMgrSecureWpInfo);
   Req->DataOffset = sizeof(TzSvcSecureWpReq); 

   WpInfo = (SdMgrSecureWpInfo *) ((UINT8 *)Req + Req->DataOffset);
   WpInfo->LunNumber = ConfigBlk->LunNumber; 

   /* Create response */
   Resp = (TzSvcSecureWpRsp *) (ScmBuffer + ReqLength);  
   SetMem(Resp, RespLength, 0); 

   /* Send request to secure app */
   Status = SendStorSecAppRequest(Req, ReqLength, Resp, RespLength);   
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "UFSGetSecureWpConfigBlock: SendStorSecAppRequest "
                           "failed with return value of 0x%x\n", Status));
      return Status; 
   }
   if (0 != Resp->Status)
   {
      DEBUG ((EFI_D_ERROR, "UFSGetSecureWpConfigBlock: SendStorSecAppRequest "
                           "response status 0x%x\n", Resp->Status));
      return EFI_DEVICE_ERROR; 
   }

   CopyMem(ConfigBlk, (UINT8*)Resp + Resp->DataOffset, sizeof(SdMgrSecureWpInfo)); 

   return EFI_SUCCESS;  
}

/**
  Set the Secure Write Protect Configuration Block 
**/
EFI_STATUS UFSSetSecureWpConfigBlock( 
   SdMgrSecureWpInfo *ConfigBlk
)
{
   EFI_STATUS Status; 
   TzSvcSecureWpReq *Req; 
   TzSvcSecureWpRsp *Resp;  
   UINT32 ReqLength = sizeof(TzSvcSecureWpReq) + sizeof(SdMgrSecureWpInfo); 
   UINT32 RespLength = sizeof(TzSvcSecureWpRsp); 

   if (NULL == ConfigBlk) 
   {
      return EFI_DEVICE_ERROR; 
   }

   if (NULL == ScmBuffer)
   {
      ScmBuffer = AllocateAlignedPages((SCM_BUFFER_SIZE / EFI_PAGE_SIZE), 
                                       EFI_PAGE_SIZE);
      if (NULL == ScmBuffer) 
      {
         DEBUG ((EFI_D_ERROR, "Failed to Allocate SCM buffer\n")); 
         return EFI_DEVICE_ERROR; 
      }
   }

   /* Create request */
   Req = (TzSvcSecureWpReq *)ScmBuffer; 
   SetMem(Req, ReqLength, 0x00); 
   Req->CommandId = CMD_SET_SECURE_WP_CONFIG; 
   Req->DataSize = sizeof(SdMgrSecureWpInfo);
   Req->DataOffset = sizeof(TzSvcSecureWpReq);
   CopyMem((UINT8 *)Req + Req->DataOffset, ConfigBlk, sizeof(SdMgrSecureWpInfo));

   /* Create response */
   Resp = (TzSvcSecureWpRsp *) (ScmBuffer + ReqLength); 
   SetMem(Resp, RespLength, 0x00); 

   /* Send request to secure app */
   Status = SendStorSecAppRequest(Req, ReqLength, Resp, RespLength);   
   if (EFI_SUCCESS != Status) 
   {
      DEBUG ((EFI_D_ERROR, "UFSSetSecureWpConfigBlock: SendStorSecAppRequest failed "
                           "with return value of 0x%x\n", Status));
      return Status; 
   }

   if (0 != Resp->Status)
   {
      DEBUG ((EFI_D_ERROR, "UFSSetSecureWpConfigBlock: SendStorSecAppRequest response "
                           "status 0x%x\n", Resp->Status));
      return EFI_DEVICE_ERROR; 
   }

   return EFI_SUCCESS; 
}
