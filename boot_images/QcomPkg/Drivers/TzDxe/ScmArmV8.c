/** @file ScmArmV8.c
   
  ScmDxe provides secure channel messaging functions to communicate with Trust Zone.

  Copyright (c) 2014-2017 Copyright Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/18/16   rj      Added register log buffer command
 02/18/15   sm      Changed SMC to not truncate parameters to 32bit
 12/22/14   sm      Changed allocation to happen on initialization
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
#include <Library/UefiCfgLib.h>
#include "tz_interface_armv8.h"
#include "qsee_interface_armv8.h"
#include "tz_syscall.h"
#include "QseeComDxe.h"

 // 64bit SMC call version
extern SmcErrnoType tz_armv8_smc_call(uint32 smc_id, uint32 param_id, uint64 parameters[4], uint64 *trusted_os_id);

// local table for recording Listener info
extern ListenerTableType  ListenerTable[MAX_LISTENER_NUM];
extern BOOLEAN            ScmCallInProgress;

#define NUM_DIRECT_REQUEST_PARAMETERS     4
#define NUM_INDIRECT_REQUEST_PARAMETERS   7
#define INDIRECT_REQUEST_PARAMETER        3

typedef struct _SMC_ARG_LIST_32 {  
  UINT32 Args[NUM_INDIRECT_REQUEST_PARAMETERS];
} SMC_ARG_LIST_32;  

typedef struct _SMC_ARG_LIST_64 {
  UINT64 Args[NUM_INDIRECT_REQUEST_PARAMETERS];
} SMC_ARG_LIST_64;

// GLOBALs
static SMC_ARG_LIST_64   *IndirectParameters = NULL;
static VOID              *ReqPtr = NULL;
static UINTN              CurrentReqSize = 0;

#define INITIAL_REQ_SIZE (8 * 1024)
extern int DisplayQseeLog(UINT32, UINT32);
extern UINT64 *log_buffer;

EFI_STATUS
ScmArmV8Initialize (VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;

  IndirectParameters = (SMC_ARG_LIST_64*)UncachedAllocateAlignedPool( sizeof(SMC_ARG_LIST_64), TZ_MEM_ALIGNMENT_SIZE);
  if(IndirectParameters == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  ReqPtr = UncachedAllocateAlignedPool( INITIAL_REQ_SIZE, TZ_MEM_ALIGNMENT_SIZE);
  if(ReqPtr == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  CurrentReqSize = INITIAL_REQ_SIZE;

ErrorExit:
  return Status;
}

EFI_STATUS
ScmInternalArmV8SysCall(
  IN  UINT32               SmcId,
  IN  UINT32               ParamId,
  IN  UINT64               Parameters[SCM_MAX_NUM_PARAMETERS],
  OUT UINT64               Results[SCM_MAX_NUM_RESULTS],
  OUT UINT64              *TrustedOsId
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  SmcErrnoType          TzStatus;
  UINT64                DirectParameters[NUM_DIRECT_REQUEST_PARAMETERS] = {0};
  UINT64                InternalTrustedOsId=0;
  UINTN                 i, NumArguments;

  if(Parameters == NULL || Results == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  if(ScmCallInProgress)
  {
    return EFI_INVALID_PARAMETER;
  }
  ScmCallInProgress = TRUE;

  NumArguments = ParamId & TZ_SYSCALL_PARAM_NARGS_MASK;

  if(NumArguments > SCM_MAX_NUM_PARAMETERS)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }
  else if(NumArguments > NUM_DIRECT_REQUEST_PARAMETERS)
  {
    for(i = 0; i < INDIRECT_REQUEST_PARAMETER; i++)
    {
      DirectParameters[i] =  Parameters[i];
    }

    DirectParameters[INDIRECT_REQUEST_PARAMETER] = (UINTN)IndirectParameters;

    for(i = INDIRECT_REQUEST_PARAMETER; i < NumArguments; i++)
    {
      IndirectParameters->Args[i - INDIRECT_REQUEST_PARAMETER] =  Parameters[i];
    }
  }
  else
  {
    for(i = 0; i < NumArguments; i++)
    {
      DirectParameters[i] =  Parameters[i];
    }
  }

  TzStatus = tz_armv8_smc_call(
    SET_BIT(SmcId, AARCH_BIT),
    ParamId,
    DirectParameters,
    &InternalTrustedOsId
    );
  if(TzStatus != SMC_SUCCESS)
  {
    DEBUG(( EFI_D_INFO, "tz_smc_armv8 failed, TzStatus = (0x%x)\r\n", TzStatus));

    Results[0] = DirectParameters[0];

    if(TzStatus == SMC_ERR_UNKNOWN_SMC_ID)
    {
      Status = EFI_UNSUPPORTED;
    }
    else if(TzStatus == SMC_ERR_SYSCALL_FAILED && (INT32)Results[0] == TZ_RETURN_ALREADY_DONE)
    {      
      Status = EFI_ALREADY_STARTED;
    }
    else
    {
      Status = EFI_DEVICE_ERROR;
    }
  }
  else
  {
    Results[0] = TZ_RETURN_SUCCESS;

    for(i = 0; i < SCM_MAX_NUM_RESULTS - 1; i++)
    {
      Results[i+1] = DirectParameters[i];
    }
  }

  if(TrustedOsId != NULL)
  {
    *TrustedOsId = InternalTrustedOsId;
  }

ErrorExit:
  ScmCallInProgress = FALSE;

  return Status;
}


EFI_STATUS
ScmArmV8SipSysCall(
  IN  QCOM_SCM_PROTOCOL   *This, 
  IN  UINT32               SmcId,
  IN  UINT32               ParamId,
  IN  UINT64               Parameters[SCM_MAX_NUM_PARAMETERS],
  OUT UINT64               Results[SCM_MAX_NUM_RESULTS]
  )
{
  return ScmInternalArmV8SysCall(
    SmcId,
    ParamId,
    Parameters,
    Results,
    NULL
    );
}

EFI_STATUS
ScmArmV8QseeSysCall(
  IN  QCOM_SCM_PROTOCOL   *This, 
  IN  UINT32               SmcId,
  IN  UINT32               ParamId,
  IN  UINT64               Parameters[SCM_MAX_NUM_PARAMETERS],
  OUT UINT64               Results[SCM_MAX_NUM_RESULTS]
  )
{
  EFI_STATUS                  Status; 
  UINT32                      QseeSmcId;
  UINT32                      QseeParamId;
  UINT64                      QseeParameters[SCM_MAX_NUM_PARAMETERS];
  UINT64                      QseeResults[SCM_MAX_NUM_RESULTS];
  UINT64                      QseeTrustedOsId;
  qsee_command_resp_info_t   *QseeRsp;
  UINT32                      ListenerId;
  UINT32                      Index;
  tzos_listener_response_handler_t *ListenerCmdPtr = (tzos_listener_response_handler_t*)QseeParameters;

  QseeSmcId = SmcId;
  QseeParamId = ParamId;
  QseeTrustedOsId = 0;
  CopyMem( QseeParameters, Parameters, SCM_MAX_NUM_PARAMETERS * sizeof(UINT64) );

  do 
  {
    Status = ScmInternalArmV8SysCall(
      QseeSmcId,
      QseeParamId,
      QseeParameters,
      QseeResults,
      &QseeTrustedOsId
      );
    if(Status != EFI_SUCCESS)
    {
      goto ErrorExit;
    }

    QseeRsp = (qsee_command_resp_info_t*)QseeResults;

    if((QseeRsp->result == TZOS_RESULT_INCOMPLETE) && 
       (QseeRsp->resp_type == QSEE_LISTENER_ID))
    {
      ListenerId = QseeRsp->data;

      if( (Status = ScmListenerLookUpIndex( ListenerId, &Index)) != EFI_SUCCESS )
      {
        // Assert ....
        DEBUG(( EFI_D_INFO, "Look up index failed. \r\n"));
        ASSERT_EFI_ERROR (Status);
        break;
      }

      // Call the callback function 
      Status = ListenerTable[Index].CallbackPtr(
                                 ListenerTable[Index].CallbackHandle, 
                                 ListenerTable[Index].SharedBufferPtr,
                                 ListenerTable[Index].SharedBufferLen);
        
      if (Status != EFI_SUCCESS)
      {
        DEBUG(( EFI_D_INFO, "Listener Callback function failed. \r\n"));
        ListenerCmdPtr->status = TZOS_RESULT_FAILURE;  
      }
      else
      {
        ListenerCmdPtr->status = TZOS_RESULT_SUCCESS; 
      }

      ListenerCmdPtr->listener_id = ListenerId;  

      QseeSmcId = TZ_OS_LISTENER_RESPONSE_HANDLER_ID;
      QseeParamId = TZ_OS_LISTENER_RESPONSE_HANDLER_ID_PARAM_ID;
    }
    else 
    {
      // No listener request. Save results.
      CopyMem( Results, QseeResults, SCM_MAX_NUM_RESULTS * sizeof(UINT64) );
      break;
    }

  } while(TRUE);

ErrorExit:

  return Status;
}

/** 
  This function calls into trust zone; any non-qsee command should use this interface.
  Does tranlation from old interface into new interface.
   
  @param[in/out]        Cmd    Command data passed into trust zone.

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
ScmArmV8SysCall(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN OUT const VOID             *Cmd
)
{
  return EFI_UNSUPPORTED;
}

/** 
  This function is unsupported
   
  @param[in/out]          Id    Token Id
  @param[in]          Param0    parameter 0
  @param[in]          Param1    parameter 1

  @return  EFI_DEVICE_UNSUPPORTED:  This function is unsupported
*/
EFI_STATUS
ScmArmV8FastCall(
  IN QCOM_SCM_PROTOCOL     *This, 
  IN UINT32                 Id,
  IN UINT32                 Param0,
  IN UINT32                 Param1
)
{
  return EFI_UNSUPPORTED;
}

/** 
  This function calls into trust zone to get TZ version
   
  @param[in/out]    Version    Pointer to TZ version address.

  @return       EFI_SUCCESS:   Function completed successfully.
           EFI_DEVICE_ERROR:   Failed 
*/
EFI_STATUS
ScmArmV8GetVersion(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN OUT UINT32                 *Version
)
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
ScmArmV8SendCommand(
  IN     QCOM_SCM_PROTOCOL      *This, 
  IN     AppCmdType              CmdId, 
  IN     UINT32                 *AppId,
  IN OUT VOID                   *Req,
  IN     UINTN                   ReqLen,
  IN OUT VOID                   *Rsp,
  IN     UINTN                   RspLen
)
{
  EFI_STATUS                      Status = EFI_INVALID_PARAMETER;
  UINT32                          SmcId;
  UINT32                          ParamId;
  UINT64                          Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                          Results[SCM_MAX_NUM_RESULTS] = {0};
  UINTN                           ReqSize;
  UINTN                           NumOf4K;
  UINT32                         *Request = Req;
  // The following are used for typecasting Paremeters to specific type of SysCall
  tzos_app_start_t               *AppStartSysCall;
  tzos_app_shutdown_t            *AppShutdownSysCall;
  tzos_app_lookup_t              *AppLookupSysCall;
  tzapp_qsapp_send_data_t        *AppSendDataSyCall;
  tzos_load_services_image_t     *ServImageSysCall;
  tzos_app_region_type_t         *AppsRegionSyscall;
  tzos_rpmb_provision_key_t      *ProvisionRpmbKey;
  qsee_command_resp_info_t       *QseeResponse;
  tzos_log_buffer_type_t         *LogBufferSyscall;

  struct tzdbg_log_t *log = NULL;
  UINT32 QseeLogStart = 0;
  UINT32 QseeLogNewStart = 0;

  // Coming from external of ScmDxe, check the pointer, other pointers are checked case by case.
  if( This == NULL )
  {
    goto ErrorExit;
  }

  //Create the QSEE commands here, need to consult with owner what is memory requirement here.
  switch( CmdId )
  {
    case  APP_START_APP_CMD:
        if( Req == NULL )
        {
          goto ErrorExit;
        }

        AppStartSysCall                             = (tzos_app_start_t*)Parameters;
        AppStartSysCall->mdt_len                    = Request[0];
        AppStartSysCall->img_len                    = Request[1];
        AppStartSysCall->pa                         = Request[2];
        SmcId                                       = TZ_OS_APP_START_ID;
        ParamId                                     = TZ_OS_APP_START_ID_PARAM_ID;

        break;

  case  APP_SHUTDOWN_CMD:
        if( AppId == NULL )
        {
          goto ErrorExit;
        }

        AppShutdownSysCall                          = (tzos_app_shutdown_t*)Parameters;
        AppShutdownSysCall->app_id                  = *AppId;
        SmcId                                       = TZ_OS_APP_SHUTDOWN_ID;
        ParamId                                     = TZ_OS_APP_SHUTDOWN_ID_PARAM_ID;

        break;

    case  APP_QUERY_APPID_CMD:
        if( AppId == NULL || Req == NULL || ReqLen == 0)
        {
          goto ErrorExit;
        }

        AppLookupSysCall                            = (tzos_app_lookup_t*)Parameters;
        AppLookupSysCall->app_name                  = (UINTN)ReqPtr;
        CopyMem( (VOID*)(UINTN)AppLookupSysCall->app_name, Req, ReqLen );
        AppLookupSysCall->name_len                  = ReqLen;
        SmcId                                       = TZ_OS_APP_LOOKUP_ID;
        ParamId                                     = TZ_OS_APP_LOOKUP_ID_PARAM_ID;

        break;

    case  APP_REGISTER_LISTENER_CMD:
    case  APP_DEREGISTER_LISTENER_CMD:
        // handled internaly using different API, bail out
        Status = EFI_UNSUPPORTED;
        goto ErrorExit;

    case  APP_SEND_DATA_CMD:
        if( AppId == NULL || Req == NULL || Rsp == NULL || (ReqLen + RspLen) == 0)
        {
          goto ErrorExit;
        }

        // special memory requirement here, 4K multiple aligned at 4K for both req+rsp
        ReqSize = ReqLen + RspLen;
        // Rounded up for multiple 4k
        NumOf4K = ReqSize/TZ_MEM_ALIGNMENT_SIZE;
        if((ReqSize - NumOf4K*TZ_MEM_ALIGNMENT_SIZE) != 0)
        {
          ReqSize = ( NumOf4K + 1)*TZ_MEM_ALIGNMENT_SIZE;
        }
        
ReallocateReq:
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
             goto ReallocateReq;
          }
        }

        // assign allocated memory
        AppSendDataSyCall                           = (tzapp_qsapp_send_data_t*)Parameters;
        AppSendDataSyCall->req_ptr                  = (UINTN)ReqPtr;
        AppSendDataSyCall->rsp_ptr                  = (UINTN)((UINT8*)ReqPtr + ReqLen);
        AppSendDataSyCall->app_id                   = *AppId;
        CopyMem( (VOID*)(UINTN)AppSendDataSyCall->req_ptr, Req, ReqLen );
        AppSendDataSyCall->req_len                  = ReqLen;
        CopyMem( (VOID*)(UINTN)AppSendDataSyCall->rsp_ptr, Rsp, RspLen );
        AppSendDataSyCall->rsp_len                  = RspLen;
        SmcId                                       = TZ_APP_QSAPP_SEND_DATA_ID;
        ParamId                                     = TZ_APP_QSAPP_SEND_DATA_ID_PARAM_ID;

       break;

    case  APP_LISTENER_DATA_RSP_CMD:
    case  APP_LOAD_EXTERNAL_ELF_CMD:
    case  APP_UNLOAD_EXTERNAL_ELF_CMD:
    case  APP_GET_APP_STATE_CMD:
          Status = EFI_UNSUPPORTED;
          goto ErrorExit;

    case  APP_LOAD_SERV_IMAGE_CMD:
         if( Req == NULL )
        {
          goto ErrorExit;
        }

        ServImageSysCall                            = (tzos_load_services_image_t*)Parameters;
        ServImageSysCall->mdt_len                   = Request[0];
        ServImageSysCall->img_len                   = Request[1];
        ServImageSysCall->pa                        = Request[2];
        SmcId                                       = TZ_OS_LOAD_SERVICES_IMAGE_ID;
        ParamId                                     = TZ_OS_LOAD_SERVICES_IMAGE_ID_PARAM_ID;

        break;

    case  APP_UNLOAD_SERV_IMAGE_CMD:
        Status = EFI_UNSUPPORTED;
        goto ErrorExit;
      
   case  APP_REGION_NOTIFICATION_CMD: 
        if( Req == NULL )
        {
          goto ErrorExit;
        }

        AppsRegionSyscall                           = (tzos_app_region_type_t*)Parameters;
        AppsRegionSyscall->app_rgn_addr             = Request[0];
        AppsRegionSyscall->app_rgn_size             = Request[1];
        SmcId                                       = TZ_OS_APP_REGION_NOTIFICATION_ID;
        ParamId                                     = TZ_OS_APP_REGION_NOTIFICATION_ID_PARAM_ID;

        break;

   case APP_REGISTER_LOG_BUF_COMMAND:
        if( Req == NULL )
        {
        Status = RETURN_UNSUPPORTED;
        goto ErrorExit;
        }
        LogBufferSyscall                           = (tzos_log_buffer_type_t*)Parameters;
        LogBufferSyscall->log_addr                 = Request[0];
        LogBufferSyscall->log_size                 = Request[1];
        SmcId                                       = TZ_OS_REGISTER_LOG_BUFFER_ID;
        ParamId                                     = TZ_OS_REGISTER_LOG_BUFFER_ID_PARAM_ID;
        
        break;

   case APP_PROVISION_RPMB_KEY_COMMAND:
        if( Req == NULL || ReqLen == 0 || Rsp == NULL || RspLen == 0)
        {
          goto ErrorExit;
        }

        ProvisionRpmbKey                            = (tzos_rpmb_provision_key_t*)Parameters;
        ProvisionRpmbKey->key_type                  = Request[0];
        SmcId                                       = TZ_OS_RPMB_PROVISION_KEY_ID;
        ParamId                                     = TZ_OS_RPMB_PROVISION_KEY_ID_PARAM_ID;

        break;

   case APP_RPMB_ERASE_COMMAND:
        if( Rsp == NULL || RspLen == 0)
        {
          goto ErrorExit;
        }

        SmcId                                       = TZ_OS_RPMB_ERASE_ID;
        ParamId                                     = TZ_OS_RPMB_ERASE_ID_PARAM_ID;

        break;
   case APP_RPMB_CHECK_PROV_STATUS_COMMAND:
        if(Rsp == NULL || RspLen == 0)
        {
          goto ErrorExit;
        }
		
        SmcId                                       = TZ_OS_RPMB_CHECK_PROV_STATUS_ID;
        ParamId                                     = TZ_OS_RPMB_CHECK_PROV_STATUS_ID_PARAM_ID;
        break;

   default:
        Status = EFI_UNSUPPORTED;
        goto ErrorExit;
  }
  if (log_buffer && (CmdId==APP_START_APP_CMD || CmdId==APP_SHUTDOWN_CMD || CmdId==APP_SEND_DATA_CMD))
  {
     log = (struct tzdbg_log_t *)log_buffer;
     QseeLogStart = (UINT32) (log->log_pos.offset);
  }


  Status = ScmArmV8QseeSysCall(This,
    SmcId,
    ParamId,
    Parameters,
    Results
    );

  if (log_buffer  && (CmdId==APP_START_APP_CMD || CmdId==APP_SHUTDOWN_CMD || CmdId==APP_SEND_DATA_CMD))
  {
    QseeLogNewStart = (UINT32) log->log_pos.offset;
    DisplayQseeLog (QseeLogStart, QseeLogNewStart);
  }

  if( Status != EFI_SUCCESS )
  {
    goto ErrorExit;
  }

  QseeResponse = (qsee_command_resp_info_t*)Results;

  // Check QSEE result.
  if(QseeResponse->result != TZOS_RESULT_SUCCESS )
  {
    // pass the result to caller through Rsp, this is for RPMB provision and erase specially
    // other cases may be impacted, anyway return Status is bad in this case., Rsp can't be trusted.
    // only RPMB provision and erase cases care this value so far.
    if ( ( CmdId == APP_PROVISION_RPMB_KEY_COMMAND || CmdId == APP_RPMB_ERASE_COMMAND || CmdId == APP_RPMB_CHECK_PROV_STATUS_COMMAND ) &&
           Rsp != NULL && RspLen != 0 )
    {
      *((UINT32 *)Rsp) = QseeResponse->result;
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
         CopyMem ( Rsp, (UINT8*)ReqPtr + ReqLen, RspLen );
         break;
      default:
        Status = EFI_UNSUPPORTED;
        goto ErrorExit;
    }
  }
  if (CmdId == APP_RPMB_CHECK_PROV_STATUS_COMMAND) {

  	*((UINT64 *)Rsp) =  QseeResponse->result;
	
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
ScmArmV8RegisterCallback(
  IN QCOM_SCM_PROTOCOL             *This,
  IN UINT32                         ListenerId,
  IN QcomScmListenerCallbackPtr     CallbackPtr,
  IN VOID                          *CallbackHandle,
  IN UINT8                         *SharedBufferPtr,
  IN UINTN                          SharedBufferLen
)
{
  EFI_STATUS                          Status;
  UINT64                              Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                              Results[SCM_MAX_NUM_RESULTS] = {0};
  tzos_register_listener_t           *AppRegisterListenerSyscall;
  qsee_command_resp_info_t           *QseeResponse;
  UINT32                              ii;

  AppRegisterListenerSyscall                   = (tzos_register_listener_t*)Parameters;
  AppRegisterListenerSyscall->listener_id      = ListenerId;
  AppRegisterListenerSyscall->req_ptr          = (UINTN)SharedBufferPtr;
  AppRegisterListenerSyscall->req_len          = SharedBufferLen;
  
  Status = ScmArmV8QseeSysCall(This,
    TZ_OS_REGISTER_LISTENER_ID,
    TZ_OS_REGISTER_LISTENER_ID_PARAM_ID,
    Parameters,
    Results);

  QseeResponse                                 = (qsee_command_resp_info_t*)Results;

  if( Status == EFI_SUCCESS && QseeResponse->result == TZOS_RESULT_SUCCESS )
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

  return Status;
}

/**
  Deregister listener service and delete entry from listener table

  @param  IN      Event          The event to be signaled.
  @param  IN      ListenerId     Listener Id to be removed
  @retval EFI_STATUS Status   

**/
EFI_STATUS
ScmArmV8DeRegisterCallback(
  IN QCOM_SCM_PROTOCOL  *This,
  IN UINT32             ListenerId
)
{
  EFI_STATUS                            Status;
  UINT32                                ii;
  UINT64                                Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                                Results[SCM_MAX_NUM_RESULTS] = {0};
  tzos_deregister_listener_t           *AppDeregisterListenerSyscall;
  qsee_command_resp_info_t             *QseeResponse;

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

  AppDeregisterListenerSyscall               = (tzos_deregister_listener_t*)Parameters;
  AppDeregisterListenerSyscall->listener_id = ListenerId;
  Status = ScmArmV8QseeSysCall(This,
    TZ_OS_DEREGISTER_LISTENER_ID,
    TZ_OS_DEREGISTER_LISTENER_ID_PARAM_ID,
    Parameters,
    Results);

  QseeResponse = (qsee_command_resp_info_t*)Results;

  // check result.
  if( Status != EFI_SUCCESS || QseeResponse->result != TZOS_RESULT_SUCCESS )
  {
    // Assert ....
    DEBUG(( EFI_D_INFO, "Deregister ListenerId failed. \r\n"));
    ASSERT_EFI_ERROR (Status);
  }

  // Remove entry from ListenerTable
  ListenerTable[ii].ListenerId      = 0;
  ListenerTable[ii].CallbackPtr     = NULL;
  ListenerTable[ii].CallbackHandle  = NULL;
  ListenerTable[ii].SharedBufferPtr = NULL;
  ListenerTable[ii].SharedBufferLen = 0;

ErrorExit:

  return Status;
}

/**
  This is called when notified that boot services has exited

  @param  IN      Event          The event to be signaled.
  @retval EFI_STATUS Status   

**/
EFI_STATUS
ScmArmV8ExitBootServicesHandler(
  IN QCOM_SCM_PROTOCOL  *This 
)
{
  EFI_STATUS                            Status;
  UINT32                                ii;
  UINT64                                Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                                Results[SCM_MAX_NUM_RESULTS] = {0};
  tzos_deregister_listener_t           *AppDeregisterListenerSyscall;
  qsee_command_resp_info_t             *QseeResponse;
  EFI_TPM_PROTOCOL                     *pTPMProtocol;
  UINT32                               TreeTpmEnableFlag = 0;

  Status = GetConfigValue ("SecurityFlag", &TreeTpmEnableFlag);        // Get this flag from uefiplat.cfg. Depending on the flag, process call to this function
  if ((Status == EFI_SUCCESS) && ((TreeTpmEnableFlag & 0x2) == 0x2))
  {
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
  }

  AppDeregisterListenerSyscall               = (tzos_deregister_listener_t*)Parameters;

  for(ii = 0; ii < MAX_LISTENER_NUM; ii++)
  {
    if( ListenerTable[ii].ListenerId  != 0)
    {
      AppDeregisterListenerSyscall->listener_id = ListenerTable[ii].ListenerId;
      
      Status = ScmArmV8QseeSysCall(This,
        TZ_OS_DEREGISTER_LISTENER_ID,
        TZ_OS_DEREGISTER_LISTENER_ID_PARAM_ID,
        Parameters,
        Results);
      
      QseeResponse = (qsee_command_resp_info_t*)Results;
      
      // check result.
      if( Status != EFI_SUCCESS || QseeResponse->result != TZOS_RESULT_SUCCESS )
      {
        // Assert ....
        DEBUG(( EFI_D_INFO, "Deregister ListenerId failed. \r\n"));
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

ErrorExit:

  return Status;
}  

QCOM_SCM_PROTOCOL QcomScmArmV8ProtocolImpl = 
{
  QCOM_SCM_PROTOCOL_REVISION,
  ScmArmV8SysCall,
  ScmArmV8FastCall,
  ScmArmV8GetVersion,
  ScmArmV8RegisterCallback,
  ScmArmV8SendCommand,
  ScmArmV8ExitBootServicesHandler,
  ScmArmV8SipSysCall,
  ScmArmV8DeRegisterCallback
};

BOOLEAN
IsArmV8Smc(VOID)
{
  EFI_STATUS Status;
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = { 0 };
  UINT64 Results[SCM_MAX_NUM_RESULTS] = { 0 };

  Parameters[0] = TZ_INFO_GET_DIAG_ID;

  Status = ScmInternalArmV8SysCall(
    TZ_INFO_IS_SVC_AVAILABLE_ID,
    TZ_INFO_IS_SVC_AVAILABLE_ID_PARAM_ID,
    Parameters,
    Results,
    NULL
    );

  if (EFI_ERROR(Status))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
