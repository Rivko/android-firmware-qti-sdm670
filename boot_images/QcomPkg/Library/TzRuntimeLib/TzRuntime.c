/** @file  
                    TzRuntime.c

  ScmDxe driver is responsible for providing TZBSP API for TZBSP clients.

  Copyright (c) 2011,2013-2014,2017 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 07/19/17   aus      32bit mode switch protocol
 08/15/14   sm       Switched to new SCM API
 09/26/13   shl      Initial version

=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIScm.h>
#include <Protocol/EFIScmModeSwitch.h>
#include <Library/TzRuntimeLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <string.h>

#define GET_SYSCALL_FUNC_ID(id) (((id & 0xFF00) << 2) | (id & 0xFF))

#define NUM_DIRECT_REQUEST_PARAMETERS     4
#define NUM_INDIRECT_REQUEST_PARAMETERS   7
#define INDIRECT_REQUEST_PARAMETER        3

extern INT32 fastcall( UINT32 CmdId, UINT32 token, UINT32 r0, UINT32 r1, ...);
extern INT32 newsmccall(UINT32 smc_id, UINT32 param_id, UINT32 parameters[4], UINT32 *trusted_os_id);

static EFI_STATUS
TzArmV7Fastcall (
  IN UINT32                  CmdId,
  IN UINT64                 *ParamArray,
  IN UINTN                   NumParams
);

static EFI_STATUS
TzArmV8Fastcall (
  IN UINT32                  SmcId,
  IN UINT32                  ParamId,
  IN UINT64                  Parameters[10],
  IN UINTN                   NumParams
);

// return values from TZ Syscall.
enum
{
  SCM_SUCCESS                 = 0,
  SCM_INTERRUPT_OCCURED       = 1,  // The SMC instruction should be reissued
  SCM_FAIL                    = -1, // failed due to unknown error
  SCM_OPERATION_NOT_ALLOWED   = -2, // Operation currently not allowed
  SCM_OPERATION_NOT_AVAILABLE = -3, // Operation currently not available
  SCM_OPERATION_NOT_VALID     = -4, // Operation not yet implemented
  SCM_OPERATION_CANCELLED     = -5, // Operation was scheduled but canceled

  SCM_WOULD_CAUSE_DEADLOCK    = -8, // Operation would cause a deadlock
  SCM_CALLER_SHOULD_RETRY     = -9, // Caller should retry operation
  SCM_EXEC_MODULE_RESET       = -10,// Executing module was reset

  SCM_OPERATION_IN_PROGRESS   = -12,// Operation in progress
  SCM_OPERATION_ALREADY_DONE  = -13,// Operation already completed
  SCM_OUT_OF_DSM_ITEMS        = -14,// Out of DSM items
  SCM_ALLOC_MEM_FAILED        = -15,// Allocation from a memory pool failed
  SCM_ARG_NOT_RECOGNIZED      = -16,// Arg is not recognized
  SCM_VALUE_OUT_RANGE         = -17,// Arg value is out of range
  SCM_PTR_ARG_BAD             = -18,// Ptr arg is bad address
  SCM_EXPECT_DATA_BUT_NONE    = -19,// Expected data, received none
  SCM_DATA_SANITY_CHECK_FAIL  = -20,// Data failed sanity check, e.g., CRC
  SCM_CONTENTS_INVALID        = -21,// Data is correct, but contents invalid
  SCM_DATA_NO_MORE_VALID      = -22,// Data is not yet or not any more valid
  SCM_DATA_TOO_LARGE          = -23 // Data is too large to process
};

static BOOLEAN
IsArmV8Smc(VOID)
{
  EFI_STATUS Status;
  UINT64 Parameters[10] = { 0 };

  Parameters[0] = TZ_INFO_GET_DIAG_ID;

  Status = TzArmV8Fastcall(
    TZ_INFO_IS_SVC_AVAILABLE_ID,
    TZ_INFO_IS_SVC_AVAILABLE_ID_PARAM_ID,
    Parameters,
    1
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


/**
  This function makes TZBSP fast syscall call( meaning using registers
  to pass parameters)

  @param  CmdId                 TZBSP command ID
  @param  *ParamArray           A pointer to a physical address where 
                                ParamArray starts ( UINT64 type )
  @param  NumParams             Parameter array size

  @retval EFI_SUCCESS           Call is successfully.
  @retval EFI_INVALID_PARAMETER NULL Parameter is passed
  @retval EFI_DEVICE_ERROR      TZ call is failed.

**/ 
EFI_STATUS TzFastcall (
  IN UINT32                  SmcId,
  IN UINT32                  ParamId,
  IN UINT64                 *ParamArray,
  IN UINTN                   NumParams
)
{
  if(IsArmV8Smc())
  {
    return TzArmV8Fastcall(SmcId, ParamId, ParamArray, NumParams);
  }
  else
  {
    return TzArmV7Fastcall(GET_SYSCALL_FUNC_ID(SmcId), ParamArray, NumParams);
  }
}

/**
  This function makes TZBSP fast syscall call( meaning using registers
  to pass parameters)

  @param  CmdId                 TZBSP command ID
  @param  *ParamArray           A pointer to a physical address where 
                                ParamArray starts ( UINT32 type )
  @param  NumParams             Parameter array size

  @retval EFI_SUCCESS           Call is successfully.
  @retval EFI_INVALID_PARAMETER NULL Parameter is passed
  @retval EFI_DEVICE_ERROR      TZ call is failed.

**/ 
static EFI_STATUS
TzArmV7Fastcall (
  IN UINT32                  CmdId,
  IN UINT64                 *ParamArray,
  IN UINTN                   NumParams
)
{
  INT32 retVal = SCM_FAIL;

  if( ParamArray == NULL || NumParams > 10 )
  {
    return EFI_INVALID_PARAMETER;
  }

  switch ( NumParams )
  {
    case 2:
      retVal = fastcall( (CmdId<<12)|2, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1] );
      break;

    case 3:
      retVal = fastcall( (CmdId<<12)|3, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1], 
                                    (UINT32)ParamArray[2]);
      break;

    case 4:
      retVal = fastcall( (CmdId<<12)|4, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3]);
      break;

    case 5:
      retVal = fastcall( (CmdId<<12)|5, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
                                    (UINT32)ParamArray[4]);
      break;

    case 6:
      retVal = fastcall( (CmdId<<12)|6, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
                                    (UINT32)ParamArray[4], (UINT32)ParamArray[5]);
      break;

    case 7:
      retVal = fastcall( (CmdId<<12)|7, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
		                                (UINT32)ParamArray[4], (UINT32)ParamArray[5],
                                    (UINT32)ParamArray[6]);
      break;

    case 8:
      retVal = fastcall( (CmdId<<12)|8, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
                                    (UINT32)ParamArray[4], (UINT32)ParamArray[5],
                                    (UINT32)ParamArray[6], (UINT32)ParamArray[7]);
      break;

    case 9:
      retVal = fastcall( (CmdId<<12)|9, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
                                    (UINT32)ParamArray[4], (UINT32)ParamArray[5],
                                    (UINT32)ParamArray[6], (UINT32)ParamArray[7],
                                    (UINT32)ParamArray[8]);
      break;

    case 10:
      retVal = fastcall((CmdId<<12)|10, 0xdeadbeef, (UINT32)ParamArray[0], (UINT32)ParamArray[1],
                                    (UINT32)ParamArray[2], (UINT32)ParamArray[3],
                                    (UINT32)ParamArray[4], (UINT32)ParamArray[5],
                                    (UINT32)ParamArray[6], (UINT32)ParamArray[7], 
                                    (UINT32)ParamArray[8], (UINT32)ParamArray[9]);
      break;

	default:
	  break;
  }

  DEBUG(( EFI_D_INFO, "TzFastcall:  Status = (0x%p)\r\n", retVal ));

  if( retVal == SCM_SUCCESS )
  {
    return  EFI_SUCCESS;
  }
  else
  {
    return EFI_DEVICE_ERROR;
  }
}


/**
  This function makes TZBSP fast syscall call( meaning using registers
  to pass parameters)

  @param  CmdId                 TZBSP command ID
  @param  *ParamArray           A pointer to a physical address where 
                                ParamArray starts ( UINT32 type )
  @param  NumParams             Parameter array size

  @retval EFI_SUCCESS           Call is successfully.
  @retval EFI_INVALID_PARAMETER NULL Parameter is passed
  @retval EFI_DEVICE_ERROR      TZ call is failed.

**/ 
static EFI_STATUS
TzArmV8Fastcall (
  IN UINT32                  SmcId,
  IN UINT32                  ParamId,
  IN UINT64                  Parameters[10],
  IN UINTN                   NumParams
)
{

  EFI_STATUS            Status = EFI_SUCCESS;
  INT32                 TzStatus;
  UINT32                DirectParameters[NUM_DIRECT_REQUEST_PARAMETERS] = {0};
  UINT32                InternalTrustedOsId;
  UINTN                 i;

  if(Parameters == NULL)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }

  if(NumParams > NUM_DIRECT_REQUEST_PARAMETERS)
  {
    Status = EFI_INVALID_PARAMETER;
    goto ErrorExit;
  }
  else
  {
    for(i = 0; i < NumParams; i++)
    {
      DirectParameters[i] =  (UINT32)Parameters[i];
    }
  }

  TzStatus = newsmccall(
    SmcId,
    ParamId,
    DirectParameters,
    &InternalTrustedOsId
    );

  DEBUG(( EFI_D_INFO, "TzFastcall:  Status = (0x%p)\r\n", TzStatus ));

  if(TzStatus == SCM_SUCCESS)
  {
    Status = EFI_SUCCESS;
  }
  else
  {
    Status = EFI_DEVICE_ERROR;
  }

ErrorExit:

  return Status;

}

/**
  This function will trigger the SCM call to switch to 32bit mode

  @param  hlos_boot_info        Structure having user params

  @retval EFI_SUCCESS           Call is successfull.

**/ 
EFI_STATUS 
SwitchTo32bitMode (
  hlos_boot_args_t hlos_boot_info
)
{
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = { 0 };
  EFI_STATUS  Status;
  
  Parameters[0] =  (UINT64) &hlos_boot_info;
  Parameters[1] =  sizeof(hlos_boot_args_t);
  
  Status = TzFastcall(
    TZ_DO_MODE_SWITCH,
    TZ_DO_MODE_SWITCH_PARAM_ID,
    Parameters,
    2
    );

  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, " SCM call to switch to 32-bit mode failed, Status =  (0x%x)\r\n", Status));
  }
  
  /* The above call will not return */
  return Status;
}


STATIC EFI_HANDLE  mScm_handle = NULL;
QCOM_SCM_MODE_SWITCH_PROTOCOL  gQcomScmModeSwitchProtocol = 
{
  SwitchTo32bitMode
};

/**
  This function will install the runtime protocol to switch to 32bit mode

  @param  None

  @retval EFI_SUCCESS           Call is successfull.

**/ 
EFI_STATUS
TzRegisterModeSwitchProtocol (VOID)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  
  Status = gBS->InstallProtocolInterface (
                &mScm_handle,
                &gQcomScmModeSwithProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &gQcomScmModeSwitchProtocol
                );
                
  if(Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, " 32-bit mode switch protocol installation failed, Status =  (0x%x)\r\n", Status));
  }                

  return Status;
}

