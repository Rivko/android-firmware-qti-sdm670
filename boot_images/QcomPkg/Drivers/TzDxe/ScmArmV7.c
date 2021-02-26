/** @file ScmArmV7.c
   
  ScmDxe provides secure channel messaging functions to communicate with Trust Zone.

  Copyright (c) 2014 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/16/14   sm      Branched from ScmDxe

 =============================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/EventGroup.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFITrEE.h>
#include "tz_interface_armv7.h"
#include "qsee_interface_armv7.h"
#include "tz_syscall.h"

#define TZ_SYSCALL_FUNC_ID(id) (((id & 0xFF00) << 2) | (id & 0xFF))

// local table for recording Listener info
extern ListenerTableType  ListenerTable[MAX_LISTENER_NUM];
extern BOOLEAN            ScmCallInProgress;

// this is for calculate the memory size to cover all cases, 
// if a new structure is added in tz_bsp.h, then it should also
// be added here. The reason for this is because UnchachedAllocate() is super slow
typedef union {
  tz_app_start_syscall_t                AppStart;
  tz_app_shutdown_syscall_t             AppShutdown;
  tz_app_lookup_syscall_t               QueryAppId;
  tz_app_register_listener_syscall_t    RegisterListener;
  tz_app_deregister_listener_syscall_t  DeregisterListener;
  tz_app_send_data_syscall_t            SendData;
  tz_app_listener_response_syscall_t    ListenerResponse; 
  tz_apps_region_syscall_t              AppsRegionSyscall;
  tz_serv_image_syscall_t               ServImageSyscall; 
  tz_provision_rpmb_key_syscall_t       ProvisionRpmb;
} QseeCommandUnionType;

EFI_STATUS
ScmArmV7Initialize (VOID)
{
  return EFI_SUCCESS;
}

/** 
  This function packs the common fields in qsee command structure.
   
  @param[in/out]   Cmd     Command data.
  @param[in]       CmdLen  Command data length.

  @return  EFI_SUCCESS:    Always.
*/
static EFI_STATUS
ScmPackCommonCmd(
  IN OUT VOID              *Cmd,
  IN     UINTN              CmdLen
)
{
  tz_syscall_req_t *pReq = NULL;
  tz_syscall_rsp_t *pRsp = NULL;
  UINTN  QseeReqCmdLen = CmdLen - sizeof(tz_syscall_req_t) - sizeof(tz_syscall_rsp_t) - sizeof(qsee_command_resp_info_t);
  UINTN  CommonRspOffset = sizeof(tz_syscall_req_t) + QseeReqCmdLen;

  SetMem((UINT8 *)Cmd, CmdLen, 0x00);

  pReq = (tz_syscall_req_t *)Cmd;

  // for tz_common_req_t
  pReq->command_start_off    = sizeof(tz_syscall_req_t);
  pReq->response_write_off   = (UINT32)CommonRspOffset;
  pReq->command_id           = TZ_EXECUTIVE_ID;

  pRsp = (tz_syscall_rsp_t *)((UINT8*)Cmd+CommonRspOffset);

  // for tz_common_rsp_t
  pRsp->len                  = (UINT32) sizeof(tz_syscall_rsp_t) + sizeof(qsee_command_resp_info_t);
  pRsp->response_start_off   = (UINT32) sizeof(tz_syscall_rsp_t);	

  return EFI_SUCCESS;
}

/** 
  This function calls into trust zone, any non-qsee command should use this interface
   
  @param[in/out]        Cmd    Command data passed into trust zone.

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
ScmArmV7SysCall (
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN OUT const VOID             *Cmd
)
{
  INT32          TzStatus;
  EFI_STATUS     Status = EFI_SUCCESS;

  if(ScmCallInProgress)
  {
    return EFI_INVALID_PARAMETER;
  }
  ScmCallInProgress = TRUE;

  TzStatus = tz_slowcall(Cmd);
  if (TzStatus != 0)
  {
    DEBUG(( EFI_D_INFO, "tz_slowcall failed, TzStatus = (0x%x)\r\n", TzStatus));
    if (TzStatus == TZ_RETURN_OPERATION_NOT_VALID)
    {
      Status = EFI_UNSUPPORTED;
    }
    else if (TzStatus == TZ_RETURN_ALREADY_DONE)
    {
      Status = EFI_ALREADY_STARTED;
    }
    else
    {
      Status = EFI_DEVICE_ERROR;
    }
  }

  ScmCallInProgress = FALSE;

  return Status;
}

EFI_STATUS
ScmArmV7SipSysCall(
  IN  QCOM_SCM_PROTOCOL   *This, 
  IN  UINT32               SmcId,
  IN  UINT32               ParamId,
  IN  UINT64               Parameters[SCM_MAX_NUM_PARAMETERS],
  OUT UINT64               Results[SCM_MAX_NUM_RESULTS]
  )
{
  static VOID             *SipCmdPtr;
  static UINTN             SipCmdSize;

  EFI_STATUS               Status;
  tz_syscall_req_t        *CommonReq = NULL;
  tz_syscall_rsp_t        *CommonRsp = NULL;
  UINT32                  *SyscallReq = NULL;
  UINT32                  *SyscallRsp = NULL;
  UINTN                    i;

  if(Parameters == NULL || Results == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  if(SipCmdPtr == NULL)
  {
    SipCmdSize = sizeof(tz_syscall_req_t) + (SCM_MAX_NUM_PARAMETERS * sizeof(UINT32)) +
                 sizeof(tz_syscall_rsp_t) + ((SCM_MAX_NUM_RESULTS - 1) * sizeof(UINT32));

    SipCmdPtr = UncachedAllocatePool(SipCmdSize);
    if(SipCmdPtr == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto ErrorExit;
    }
  }

  ZeroMem( SipCmdPtr, SipCmdSize );

  // Fill out common request structure
  CommonReq                         = (tz_syscall_req_t*)SipCmdPtr;
  CommonReq->command_id             = TZ_SYSCALL_FUNC_ID(SmcId);
  CommonReq->command_start_off      = sizeof(tz_syscall_req_t);
  CommonReq->response_write_off     = sizeof(tz_syscall_req_t) + (SCM_MAX_NUM_PARAMETERS * sizeof(UINT32));
  CommonReq->len                    = (UINT32)SipCmdSize;

  // Copy parameters into request buffer
  SyscallReq                        = (UINT32*)((UINT8*)CommonReq + CommonReq->command_start_off);
  for (i = 0; i < SCM_MAX_NUM_PARAMETERS; i++)
  {
    SyscallReq[i]                   = (UINT32)Parameters[i];
  }

  CommonRsp                         = (tz_syscall_rsp_t*)((UINT8*)CommonReq + CommonReq->response_write_off);
  CommonRsp->len                    = sizeof(tz_syscall_rsp_t) + ((SCM_MAX_NUM_RESULTS - 1) * sizeof(UINT32));
  CommonRsp->response_start_off     = sizeof(tz_syscall_rsp_t);

  Status = ScmArmV7SysCall(This, SipCmdPtr);
  if(Status != EFI_SUCCESS)
  {
    goto ErrorExit;
  }

  // Copy response into result buffer
  SyscallRsp                        = (UINT32*)((UINT8*)CommonRsp + CommonRsp->response_start_off - sizeof(UINT32));
  for (i = 0; i < SCM_MAX_NUM_RESULTS; i++)
  {
    Results[i]                      = SyscallRsp[i];
  }


ErrorExit:
  return Status;
}

/** 
  This function is fast call with 2 parameters, will be obsolited.
   
  @param[in/out]          Id    Token Id
  @param[in]          Param0    parameter 0
  @param[in]          Param1    parameter 1

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
ScmArmV7FastCall(
  IN QCOM_SCM_PROTOCOL     *This, 
  IN UINT32                 Id,
  IN UINT32                 Param0,
  IN UINT32                 Param1
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(ScmCallInProgress)
  {
    return EFI_INVALID_PARAMETER;
  }
  ScmCallInProgress = TRUE;

  if( 0 != tz_fastcall((Id << 12) | 2, 0xdeadbeef, (Param0), (Param1)) )
  {
    Status = EFI_DEVICE_ERROR;
  }

  ScmCallInProgress = FALSE;

  return Status;
}

/** 
  This function calls into trust zone to get TZ version
   
  @param[in/out]    Version    Pointer to TZ version address.

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS 
ScmArmV7GetVersion(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN OUT UINT32                 *Version
)
{
  int TzStatus;
  EFI_STATUS     Status = EFI_SUCCESS;

  if(ScmCallInProgress)
  {
    return EFI_INVALID_PARAMETER;
  }
  ScmCallInProgress = TRUE;

  TzStatus = tz_get_version((UINT32 *)Version);
  if ( TzStatus != 0)
  {
    DEBUG(( EFI_D_INFO, "tz_get_version failed, TzStatus = (0x%x)\r\n", TzStatus));
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

ErrorExit:
  ScmCallInProgress = FALSE;
	
  return Status;
}

/** 
  This function calls into trust zone, any qsee command should use this interface
  For all Qsee command, the listener will be monitored and action will take if there
  is request coming from TZ.
   
  @param[in/out]     CmdPtr    Command data passed into trust zone.

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
ScmArmV7QseeSysCall(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN OUT VOID                   *CmdPtr
)
{
  static tz_app_listener_response_syscall_t  *ListenerCmdPtr;
  static UINTN                                ListenerCmdSize;

  EFI_STATUS                  Status;
  UINT32                      ListenerId;
  UINT32                      Index;
  VOID                       *SysCallCmdPtr;
  tz_syscall_rsp_t           *SyscallTzResponse;
  qsee_command_resp_info_t   *SyscallQseeResponse;
  tz_syscall_rsp_t           *TzResponse;
  qsee_command_resp_info_t   *QseeResponse;

  // assign pointer here, CmdPtr will be kept.
  SysCallCmdPtr = CmdPtr;

  do 
  {
    Status = ScmArmV7SysCall(NULL, SysCallCmdPtr);

    if( Status != EFI_SUCCESS )
    {
      goto ErrorExit;
    }

    SyscallTzResponse   = (tz_syscall_rsp_t*)((UINT8*)SysCallCmdPtr + ((tz_syscall_req_t *) SysCallCmdPtr )->response_write_off);
    SyscallQseeResponse = (qsee_command_resp_info_t*)((UINT8*)SyscallTzResponse + sizeof(tz_syscall_rsp_t));

    // check if there is a listener request.
    if( SyscallQseeResponse->result == TZOS_RESULT_INCOMPLETE && 
        SyscallQseeResponse->resp_type == QSEE_LISTENER_ID)
    {
      ListenerId = SyscallQseeResponse->data;

      if( (Status = ScmListenerLookUpIndex( ListenerId, &Index)) != EFI_SUCCESS )
      {
        // Assert ....
        DEBUG(( EFI_D_INFO, " Look up Index failed. \r\n"));
        ASSERT_EFI_ERROR (Status);
        break;
      }

      //Call the callback function 
      Status = ListenerTable[Index].CallbackPtr(
                                 ListenerTable[Index].CallbackHandle, 
                                 ListenerTable[Index].SharedBufferPtr,
                                 ListenerTable[Index].SharedBufferLen);
				
      if ( Status != EFI_SUCCESS )
      {
        // assert ....need to find out why
        DEBUG(( EFI_D_INFO, " Callback function return bad. \r\n"));
        ASSERT_EFI_ERROR (Status);
        break;
      }
      // in case multiple rounds of listener requests, don't claim more than once
      // once claimed, never release, so no need to claim again and performance will be better
      if( ListenerCmdPtr == NULL )
      {
        ListenerCmdSize = sizeof( tz_app_listener_response_syscall_t);
        ListenerCmdPtr =  (tz_app_listener_response_syscall_t*)UncachedAllocatePool( ListenerCmdSize );
        if( ListenerCmdPtr == NULL )
        {
          Status =  EFI_OUT_OF_RESOURCES; 
          goto ErrorExit;
        }
      }

      ScmPackCommonCmd ( ListenerCmdPtr, ListenerCmdSize);
      ListenerCmdPtr->common_req.len  = (UINT32)ListenerCmdSize;
      ListenerCmdPtr->cmd_id          = APP_LISTENER_DATA_RSP_CMD;
      ListenerCmdPtr->req.listener_id = ListenerId;
      ListenerCmdPtr->req_len         = sizeof (qsee_listener_rsp_t) + sizeof(UINT32);
      // assign syscall pointer
      SysCallCmdPtr                   = ListenerCmdPtr;
    }
    else 
    {
      // no listener request, assemble return for top qsee sturcture
      SyscallTzResponse   = (tz_syscall_rsp_t*)((UINT8*)SysCallCmdPtr + ((tz_syscall_req_t *) SysCallCmdPtr )->response_write_off);
      SyscallQseeResponse = (qsee_command_resp_info_t*)((UINT8*)SyscallTzResponse + sizeof(tz_syscall_rsp_t));
      TzResponse          = (tz_syscall_rsp_t*)((UINT8*)CmdPtr + ((tz_syscall_req_t *) CmdPtr )->response_write_off);
      QseeResponse        = (qsee_command_resp_info_t*)((UINT8*)TzResponse + sizeof(tz_syscall_rsp_t));

      TzResponse->status             = SyscallTzResponse->status;

      QseeResponse->result           = SyscallQseeResponse->result;
      QseeResponse->resp_type        = SyscallQseeResponse->resp_type;
      QseeResponse->data             = SyscallQseeResponse->data;

      break;
    }

  // if come here, let's continue scm sys call.

  } while(TRUE);

ErrorExit:

  return Status;
}

/** 
  This function is interface for sending command to ScmDxe. It is designed for
  fit the new TZ 1.4 interface structures. Here is how it is used:

  APP_START_APP_CMD            AppId = NULL, Req = &(Your request), Rsp=NULL
  APP_SHUTDOWN_CMD             AppId =&(Your AppID), Req = NULL, Rsp = NULL 
  APP_QUERY_APPID_CMD          AppId==&(Your AppID), Req = &(your ascii app name string), Rsp=NULL
  APP_REGISTER_LISTENER_CMD    separate interface
  APP_DEREGISTER_LISTENER_CMD  handled internally
  APP_SEND_DATA_CMD            AppId =&(Your AppID), Req = &(Your request), Rsp =&(your response) 
  APP_LISTENER_DATA_RSP_CMD    handled internally
  APP_PROVISION_RPMB_KEY_COMMAND   AppId=NULL  ,Req=&(your key_type), Rsp !=NULL RspLen >=4
  APP_PROVISION_RPMB_KEY_COMMAND   AppId=NULL  ,Req=NULL, ReqLen = 0, Rsp !=NULL RspLen >=4

  @param[in/out]   CmdId    Command type
  @param[in/out]  *AppId    application id address  
  @param[in/out]  *Req      Req pointer
  @param[in]       ReqLen   Req data length
  @param[in/out]  *Rsp      Rsp Pointer
  @param[in]       RspLen   Rsp data length

  @return  EFI_SUCCESS:     Function completed successfully.
          Other values:     Failed 
*/
EFI_STATUS
ScmArmV7SendCommand(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN     AppCmdType              CmdId, 
  IN     UINT32                 *AppId,
  IN OUT VOID                   *Req,
  IN     UINTN                   ReqLen,
  IN OUT VOID                   *Rsp,
  IN     UINTN                   RspLen
)
{
  static VOID        *QseeCmdPtr;
  static VOID        *ReqPtr;
  static UINTN        CurrentReqSize;

  EFI_STATUS          Status = EFI_INVALID_PARAMETER;
  UINTN               CmdSize;
  UINTN               ReqSize;
  UINTN               NumOf4K;
  // The following are used for typecasting Paremeters to specific type of SysCall
  tz_app_start_syscall_t          *AppStartSysCall;
  tz_app_shutdown_syscall_t       *AppShutdownSysCall;
  tz_app_lookup_syscall_t         *AppLookupSysCall;
  tz_app_send_data_syscall_t      *AppSendDataSyCall;
  tz_serv_image_syscall_t         *ServImageSysCall;
  tz_apps_region_syscall_t        *AppsRegionSyscall;
  tz_provision_rpmb_key_syscall_t *ProvisionRpmbKey;
  tz_syscall_rsp_t                *TzResponse;
  qsee_command_resp_info_t        *QseeResponse;

  // Coming from external of ScmDxe, check the pointer, other pointers are checked case by case.
  if( This == NULL )
  {
    goto ErrorExit;
  }

  if( QseeCmdPtr == NULL )
  {
     QseeCmdPtr = UncachedAllocatePool( sizeof(QseeCommandUnionType));
     if( QseeCmdPtr == NULL )
     {
       Status = EFI_OUT_OF_RESOURCES;
       goto ErrorExit;
     }
  }

  //Create the QSEE commands here, need to consult with owner what is memory requirement here.
  switch( CmdId )
  {
    case  APP_START_APP_CMD:
        if( Req == NULL )
        {
          goto ErrorExit;
        }
        CmdSize                                     = sizeof(tz_app_start_syscall_t);
        AppStartSysCall                             = (tz_app_start_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( AppStartSysCall, CmdSize );
        AppStartSysCall->common_req.len             = (UINT32)CmdSize;
        AppStartSysCall->cmd_id                     = CmdId;
        CopyMem( &(AppStartSysCall->req), Req, ReqLen );
        AppStartSysCall->req_len	                  = (UINT32)(ReqLen + sizeof(UINT32));

        break;

	case  APP_SHUTDOWN_CMD:
        if( AppId == NULL )
        {
          goto ErrorExit;
        }
        CmdSize                                     = sizeof( tz_app_shutdown_syscall_t);
        AppShutdownSysCall                          = (tz_app_shutdown_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( AppShutdownSysCall, CmdSize);
        AppShutdownSysCall->common_req.len          = (UINT32)CmdSize;
        AppShutdownSysCall->cmd_id                  = CmdId;
        AppShutdownSysCall->app_id                  = *AppId;
        AppShutdownSysCall->req_len                 = 2*sizeof(UINT32);

        break;

    case  APP_QUERY_APPID_CMD:
        if( AppId == NULL || Req == NULL || ReqLen == 0)
        {
          goto ErrorExit;
        }

        CmdSize                                     = sizeof(tz_app_lookup_syscall_t);
        AppLookupSysCall                            = (tz_app_lookup_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( AppLookupSysCall, CmdSize);
        AppLookupSysCall->common_req.len            = (UINT32)CmdSize;
        AppLookupSysCall->cmd_id                    = CmdId;
        CopyMem( AppLookupSysCall->app_name, Req, ReqLen );
        AppLookupSysCall->req_len	                  = (UINT32)(ReqLen + sizeof(UINT32));

        break;

    case  APP_REGISTER_LISTENER_CMD:
    case  APP_DEREGISTER_LISTENER_CMD:
        // handled internaly using different API, bail out
        goto ErrorExit;

    case  APP_SEND_DATA_CMD:
        if( AppId == NULL || Req == NULL || Rsp == NULL || (ReqLen + RspLen) == 0)
        {
          Status = EFI_INVALID_PARAMETER;
          goto ErrorExit;
        }
        CmdSize                                      = sizeof(tz_app_send_data_syscall_t);
        AppSendDataSyCall                            = (tz_app_send_data_syscall_t*)QseeCmdPtr;

        // special memory requirement here, 4K multiple aligned at 4K for both req+rsp
        ReqSize = ReqLen + RspLen;
        // Rounded up for multiple 4k
        NumOf4K = ReqSize/TZ_MEM_ALIGNMENT_SIZE;
        if((ReqSize - NumOf4K*TZ_MEM_ALIGNMENT_SIZE) != 0)
        {
          ReqSize = ( NumOf4K + 1)*TZ_MEM_ALIGNMENT_SIZE;
        }
        
Reallocate:
        if( ReqPtr == NULL )
        {
          ReqPtr = UncachedAllocateAlignedPool( ReqSize, TZ_MEM_ALIGNMENT_SIZE);
          if( ReqPtr == NULL )
          {
            Status = EFI_OUT_OF_RESOURCES;
            goto ErrorExit;
          }
          CurrentReqSize = ReqSize;
          DEBUG(( EFI_D_INFO, "UncachedAllocateAlignedPool  ---, ReqSize = %d\r\n", ReqSize));
        }
        else 
        {
          if ( ReqSize > CurrentReqSize )
          {
             UncachedFreeAlignedPool( ReqPtr );  
             ReqPtr = NULL;
             CurrentReqSize = 0;
             goto Reallocate;
          }
        }

       // assign allocated memory
       ScmPackCommonCmd ( AppSendDataSyCall, CmdSize);
       AppSendDataSyCall->req.req_ptr                = (UINT32)(UINTN)ReqPtr;
       AppSendDataSyCall->req.rsp_ptr                = (UINT32)(UINTN)((UINT8 *)ReqPtr + ReqLen);
       AppSendDataSyCall->common_req.len             = (UINT32)CmdSize;
       AppSendDataSyCall->cmd_id                     = CmdId;
       AppSendDataSyCall->req.app_id                 = *AppId;
       CopyMem( (VOID*)(UINTN)AppSendDataSyCall->req.req_ptr, Req, ReqLen );
       AppSendDataSyCall->req.req_len                = (UINT32)ReqLen;
       CopyMem( (VOID*)(UINTN)AppSendDataSyCall->req.rsp_ptr, Rsp, RspLen );
       AppSendDataSyCall->req.rsp_len                = (UINT32)RspLen;
       AppSendDataSyCall->req_len                    =  sizeof(qsee_client_send_data_req_t) + sizeof(UINT32);

       break;

    case  APP_LISTENER_DATA_RSP_CMD:
//        break;
    case  APP_LOAD_EXTERNAL_ELF_CMD:
//        break;
    case  APP_UNLOAD_EXTERNAL_ELF_CMD:
//        break;
    case  APP_GET_APP_STATE_CMD:
//        break;
          Status = RETURN_UNSUPPORTED;
          goto ErrorExit;

    case  APP_LOAD_SERV_IMAGE_CMD:
         if( Req == NULL )
         {
           goto ErrorExit;
         }
         CmdSize                                     = sizeof(tz_serv_image_syscall_t);
         ServImageSysCall                            = (tz_serv_image_syscall_t*)QseeCmdPtr;

         ScmPackCommonCmd ( ServImageSysCall, CmdSize );
         ServImageSysCall->common_req.len            = (UINT32)CmdSize;
         ServImageSysCall->cmd_id                    = CmdId;
         CopyMem( (VOID *)&(ServImageSysCall->req), Req, ReqLen );
         ServImageSysCall->req_len	                 = (UINT32)(ReqLen + sizeof(UINT32));

         break;

    case  APP_UNLOAD_SERV_IMAGE_CMD:
         Status = RETURN_UNSUPPORTED;
         goto ErrorExit;
	    
   case  APP_REGION_NOTIFICATION_CMD: 
        if( Req == NULL )
        {
          goto ErrorExit;
        }
        CmdSize                                     = sizeof(tz_apps_region_syscall_t);
        AppsRegionSyscall                           = (tz_apps_region_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( AppsRegionSyscall, CmdSize );
        AppsRegionSyscall->common_req.len           = (UINT32)CmdSize;
        AppsRegionSyscall->cmd_id                   = CmdId;
        CopyMem( (VOID *)&(AppsRegionSyscall->req), Req, ReqLen );
        AppsRegionSyscall->req_len	                = (UINT32)(ReqLen + sizeof(UINT32));
        break;

   case APP_REGISTER_LOG_BUF_COMMAND:
        Status = RETURN_UNSUPPORTED;
        goto ErrorExit;

   case APP_PROVISION_RPMB_KEY_COMMAND:
        if( Req == NULL || ReqLen == 0 || Rsp == NULL || RspLen == 0)
        {
          goto ErrorExit;
        }

        CmdSize                                     = sizeof(tz_provision_rpmb_key_syscall_t);
        ProvisionRpmbKey                            = (tz_provision_rpmb_key_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( ProvisionRpmbKey, CmdSize);
        ProvisionRpmbKey->common_req.len            = (UINT32)CmdSize;
        ProvisionRpmbKey->cmd_id                    = CmdId;
        CopyMem( (VOID *)&(ProvisionRpmbKey->req), Req, ReqLen );
        ProvisionRpmbKey->req_len	                  = (UINT32)(ReqLen + sizeof(UINT32));
        break;

   case APP_RPMB_ERASE_COMMAND:
        if( Req != NULL || ReqLen != 0 || Rsp == NULL || RspLen == 0)
        {
          goto ErrorExit;
        }

        // borrow tz_provision_rpmb_key_syscall_t because Req and ReqLen are not used, 
        CmdSize                                     = sizeof(tz_provision_rpmb_key_syscall_t);
        ProvisionRpmbKey                            = (tz_provision_rpmb_key_syscall_t*)QseeCmdPtr;

        ScmPackCommonCmd ( ProvisionRpmbKey, CmdSize);
        ProvisionRpmbKey->common_req.len            = (UINT32)CmdSize;
        ProvisionRpmbKey->cmd_id                    = CmdId;
        ProvisionRpmbKey->req_len	                  = (UINT32)(ReqLen + sizeof(UINT32));
        break;

   default:
        Status = RETURN_UNSUPPORTED;
        goto ErrorExit;
  }

  // catch any wrong implementation here for Union thing
  if( CmdSize > sizeof(QseeCommandUnionType))
  {
    DEBUG(( EFI_D_INFO, " Go QseeCommandUnionType definition and fix there !!!!. \r\n"));
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR (Status);
    goto ErrorExit;
  }

  // Find QseeResponse
  TzResponse   = (tz_syscall_rsp_t*)((UINT8 *)QseeCmdPtr + ((tz_syscall_req_t *)QseeCmdPtr )->response_write_off);
  QseeResponse = (qsee_command_resp_info_t*)((UINT8*)TzResponse + sizeof(tz_syscall_rsp_t));

  Status = ScmArmV7QseeSysCall(This, QseeCmdPtr);
  if( Status != EFI_SUCCESS )
  {
    goto ErrorExit;
  }

  // check tz result.
  if(TzResponse->status != TZ_RETURN_SUCCESS || 
     QseeResponse->result != TZOS_RESULT_SUCCESS )
  {
    // pass the result to caller through Rsp, this is for RPMB provision and erase specially
    // other cases may be imapcted, anyway return Status is bad in this case., Rsp can't be trusted.
    // only RPMB provision and erase cases care this value so far.
    if ( ( CmdId == APP_PROVISION_RPMB_KEY_COMMAND || CmdId == APP_RPMB_ERASE_COMMAND ) &&
		Rsp != NULL && RspLen != 0 )
    {
      *(( UINT32 *)Rsp) = QseeResponse->result;
    }

    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

  if(QseeResponse->resp_type == QSEE_APP_ID)
  {
    switch ( CmdId )
    {
      case  APP_START_APP_CMD:
      case  APP_QUERY_APPID_CMD:
      case  APP_SHUTDOWN_CMD:

        *AppId = QseeResponse->data; 
         break;

      case  APP_SEND_DATA_CMD:
         AppSendDataSyCall = (tz_app_send_data_syscall_t*)QseeCmdPtr;
         if( AppSendDataSyCall->req.rsp_len > RspLen )
         {
           Status = EFI_OUT_OF_RESOURCES;
           goto ErrorExit;
         }
         CopyMem ( Rsp, (VOID*)(UINTN)AppSendDataSyCall->req.rsp_ptr, AppSendDataSyCall->req.rsp_len );
         break;
      default:
        Status = RETURN_UNSUPPORTED;
        goto ErrorExit;
    }
  }

ErrorExit:

  return Status;

}

/** 
  This function calls trust zone to register a callback function
  ScmDxe also keep a record of this registration.
   
  @param[in]   ListenerId         Listener ID.
  @param[in]   CallbackPtr        Pointer to callback fucntion.
  @param[in]   CallbackHandle     handle pointer from caller
  @param[in]   SharedBufferPtr    
  @param[in]   SharedBufferLen

  @return            0:   Function completed successfully.
          Other values:   Failed 
*/
EFI_STATUS
ScmArmV7RegisterCallback(
  IN QCOM_SCM_PROTOCOL             *This,
  IN UINT32                         ListenerId,
  IN QcomScmListenerCallbackPtr     CallbackPtr,
  IN VOID                          *CallbackHandle,
  IN UINT8                         *SharedBufferPtr,
  IN UINTN                          SharedBufferLen
)
{
  EFI_STATUS                          Status;
  UINTN                               CmdSize;
  UINT32                              ii;
  tz_app_register_listener_syscall_t *AppRegisterListenerSyscall = NULL;

  //Create the QSEE Listener service register command.
  CmdSize                                      = sizeof(tz_app_register_listener_syscall_t);
  AppRegisterListenerSyscall                   = (tz_app_register_listener_syscall_t*)UncachedAllocatePool( CmdSize );
  if( AppRegisterListenerSyscall == NULL )
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  ScmPackCommonCmd ( AppRegisterListenerSyscall, CmdSize);
  AppRegisterListenerSyscall->common_req.len   = (UINT32)CmdSize;
  AppRegisterListenerSyscall->cmd_id           = QSEE_REGISTER_LISTENER;
  AppRegisterListenerSyscall->req.listener_id  = ListenerId;
  AppRegisterListenerSyscall->req.sb_ptr       = (UINT32)(UINTN)SharedBufferPtr;
  AppRegisterListenerSyscall->req.sb_len       = (UINT32)SharedBufferLen;
  AppRegisterListenerSyscall->req_len          = sizeof(tz_app_register_listener_syscall_t);

  Status = ScmArmV7QseeSysCall(This, AppRegisterListenerSyscall);

  if( Status == EFI_SUCCESS && AppRegisterListenerSyscall->rsp.result == TZOS_RESULT_SUCCESS)
  {
    for(ii = 0; ii < MAX_LISTENER_NUM; ii++)
    {
      if( ListenerTable[ii].ListenerId  == 0 )
      {
        ListenerTable[ii].ListenerId      = ListenerId;
        ListenerTable[ii].CallbackPtr     = CallbackPtr;
        ListenerTable[ii].CallbackHandle  = CallbackHandle;
        ListenerTable[ii].SharedBufferPtr = SharedBufferPtr;
        ListenerTable[ii].SharedBufferLen = SharedBufferLen;
        break;
      }
    }
    if (ii == MAX_LISTENER_NUM)
    {
      DEBUG(( EFI_D_INFO, "Maximum number of callback function reached. \r\n"));
      Status = EFI_OUT_OF_RESOURCES;
    }
  }

ErrorExit:

  if( AppRegisterListenerSyscall != NULL )
  {
    UncachedFreePool( AppRegisterListenerSyscall );
  }

  return Status;
}

/**
  Deregister listener service and delete entry from listener table

  @param  IN      Event          The event to be signaled.
  @param  IN      ListenerId     Listener Id to be removed
  @retval EFI_STATUS Status   

**/
EFI_STATUS
ScmArmV7DeRegisterCallback(
  IN QCOM_SCM_PROTOCOL  *This,
  IN UINT32             ListenerId
)
{
  EFI_STATUS                            Status;
  UINT32                                ii;
  UINTN                                 CmdSize;
  tz_app_deregister_listener_syscall_t *AppDeregisterListenerSyscall = NULL;

  for(ii = 0; ii < MAX_LISTENER_NUM; ii++)
  {
    if( ListenerTable[ii].ListenerId  == ListenerId)
    {
      break;
    }
  }
  if (ii == MAX_LISTENER_NUM)
  {
    DEBUG(( EFI_D_INFO, "Maximum number of callback function reached. \r\n"));
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  // claim memory for deregister
  CmdSize                                      = sizeof( tz_app_deregister_listener_syscall_t);
  AppDeregisterListenerSyscall                 = (tz_app_deregister_listener_syscall_t*)UncachedAllocatePool( CmdSize );
  if( AppDeregisterListenerSyscall == NULL )
  {
    Status =  EFI_OUT_OF_RESOURCES; 
    goto ErrorExit;
  }

  // packing
  ScmPackCommonCmd ( AppDeregisterListenerSyscall, CmdSize);
  AppDeregisterListenerSyscall->common_req.len = (UINT32)CmdSize;
  AppDeregisterListenerSyscall->cmd_id         = QSEE_DEREGISTER_LISTENER;
  AppDeregisterListenerSyscall->req_len        = sizeof(qsee_listener_rsp_t);
  AppDeregisterListenerSyscall->req.listener_id = ListenerId;

  Status = ScmArmV7QseeSysCall(NULL, AppDeregisterListenerSyscall);

  // check result.
  if( Status != EFI_SUCCESS || AppDeregisterListenerSyscall->rsp.result != TZOS_RESULT_SUCCESS )
  {
    // Assert ....
    DEBUG(( EFI_D_INFO, " Deregister ListenerId failed. \r\n"));
    ASSERT_EFI_ERROR (Status || AppDeregisterListenerSyscall->rsp.result);
  }

  // Remove entry from ListenerTable
  ListenerTable[ii].ListenerId      = 0;
  ListenerTable[ii].CallbackPtr     = NULL;
  ListenerTable[ii].CallbackHandle  = NULL;
  ListenerTable[ii].SharedBufferPtr = NULL;
  ListenerTable[ii].SharedBufferLen = 0;

ErrorExit:
  if( AppDeregisterListenerSyscall != NULL )
  {
    UncachedFreePool( AppDeregisterListenerSyscall );
    AppDeregisterListenerSyscall = NULL;
  }

  return Status;
}

/**
  This is called when notified that boot services has exited

  @param  IN      Event          The event to be signaled.
  @retval EFI_STATUS Status   

**/
EFI_STATUS
ScmArmV7ExitBootServicesHandler(
  IN QCOM_SCM_PROTOCOL  *This 
)
{
  EFI_STATUS                            Status;
  UINT32                                ii;
  UINTN                                 CmdSize;
  tz_app_deregister_listener_syscall_t *AppDeregisterListenerSyscall = NULL;
  EFI_TPM_PROTOCOL                     *pTPMProtocol;

  // Locate EFI_TPM_PROTOCOL. It used to send TPM commands to Winsecapp.
  Status = gBS->LocateProtocol ( &gEfiTpmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pTPMProtocol
                               );
  if ( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Locate TPM Protocol failed, Status =  (0x%p)\r\n", Status));
    goto ErrorExit;
  }
  
    // Send Flush Disable command to Winsecapp. SCM driver in the HLOS will enable the FLush after its ready.
  Status = pTPMProtocol->FlushDisable(pTPMProtocol);
  if ( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Flush Disable Failed, Status =  (0x%p)\r\n", Status));
    goto ErrorExit;
  }

  // claim memory for deregister
  CmdSize                                      = sizeof( tz_app_deregister_listener_syscall_t);
  AppDeregisterListenerSyscall                 = (tz_app_deregister_listener_syscall_t*)UncachedAllocatePool( CmdSize );
  if( AppDeregisterListenerSyscall == NULL )
  {
    Status =  EFI_OUT_OF_RESOURCES; 
    goto ErrorExit;
  }
  // packing
  ScmPackCommonCmd ( AppDeregisterListenerSyscall, CmdSize);
  AppDeregisterListenerSyscall->common_req.len = (UINT32)CmdSize;
  AppDeregisterListenerSyscall->cmd_id         = QSEE_DEREGISTER_LISTENER;
  AppDeregisterListenerSyscall->req_len        = sizeof(qsee_listener_rsp_t);

  for(ii = 0; ii < MAX_LISTENER_NUM; ii++)
  {
    if( ListenerTable[ii].ListenerId  != 0)
    {
      AppDeregisterListenerSyscall->req.listener_id = ListenerTable[ii].ListenerId;
      
      Status = ScmArmV7QseeSysCall(NULL, AppDeregisterListenerSyscall);
      
      // check result.
      if( Status != EFI_SUCCESS || AppDeregisterListenerSyscall->rsp.result != TZOS_RESULT_SUCCESS )
      {
        // Assert ....
        DEBUG(( EFI_D_INFO, " Deregister ListenerId failed. \r\n"));
        ASSERT_EFI_ERROR (Status || AppDeregisterListenerSyscall->rsp.result);
      }
    }
  }

ErrorExit:
  if( AppDeregisterListenerSyscall != NULL )
  {
    UncachedFreePool( AppDeregisterListenerSyscall );
    AppDeregisterListenerSyscall = NULL;
  }

  return Status;
}  

QCOM_SCM_PROTOCOL QcomScmArmV7ProtocolImpl = 
{
  QCOM_SCM_PROTOCOL_REVISION,
  ScmArmV7SysCall,
  ScmArmV7FastCall,
  ScmArmV7GetVersion,
  ScmArmV7RegisterCallback,
  ScmArmV7SendCommand,
  ScmArmV7ExitBootServicesHandler,
  ScmArmV7SipSysCall
};
