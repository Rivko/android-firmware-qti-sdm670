/** @file MemoryMapDxe.c
   
  Implementation for Qualcomm UEFI subsystem memory protocol interface.

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/22/16   bh      Updated retail check
 06/08/16   sj      removed the mass storage cookie references
 08/15/14   sm      Switched to new SCM API
 08/12/13   niting  Deprecate legacy offline dumps
 07/24/13   yg      Trimdown logs
 06/25/13   niting  Always setup variables for legacy UEFI dump
 06/11/13   niting  Restore 2MB reserved region when crash dump is not enabled
 04/03/13   niting  Added offline crash dump support
 03/20/13   nk      Support for error fatal on XPU Violations 
 02/19/13   nk      Changed not to do SCM sys call in carveout mode
 02/11/13   niting  Updated cookie PCDs
 01/08/13   nk      Defined dump address in QcomLib.h
 01/08/13   nk      Support for TZ dumps
 08/27/12   vishalo Swap DLOAD cookie values
 06/08/12   vishalo Add variables for crash dump support
 05/07/12   jz      Updated for subsystem memory report
 10/04/11   jz      Initial revision.
=============================================================================*/

#include <Uefi.h>
#include <Protocol/EFIMemoryMap.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemmapLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Protocol/EFIScm.h>
#include <Library/DBIDump.h>
#include <Library/UefiCfgLib.h>
#include <Library/OfflineCrashDump.h>
#include <Library/QcomLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/FuseControlLib.h>
#include <Library/QcomTargetLib.h>

EFI_STATUS EFIAPI GetSubsystemMemoryMap ( 
  IN EFI_SUBSYSTEM_MEMORY_PROTOCOL       *This,
  IN OUT UINTN                           *Size,
  IN OUT EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *MemoryMap,
  OUT UINTN                              *DescriptorCount
  );

/* Memory map Protocol */
static EFI_SUBSYSTEM_MEMORY_PROTOCOL gMemoryMapProtocol = 
{
  EFI_MEMORYMAP_PROTOCOL_REVISION,
  GetSubsystemMemoryMap
};

extern EFI_GUID gQcomMemoryCaptureGuid;
extern EFI_GUID gEfiMemoryMapProtocolGuid;

EFI_HOB_GUID_TYPE *MemoryCaptureGuidHob = NULL;

/** 
 
  SCM Sys Call for TZ dumps 
   
**/
EFI_STATUS
EFIAPI
DumpSysCall()
{ 
  QCOM_SCM_PROTOCOL                 *pQcomScmProtocol                  = NULL;
  EFI_STATUS                        Status                             = EFI_SUCCESS;
  UINT64                            Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                            Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_dump_set_cpu_ctx_buf_req_t     *SysCallReq                        = (tz_dump_set_cpu_ctx_buf_req_t*)Parameters;
  tz_syscall_rsp_t                  *SysCallRsp                        = (tz_syscall_rsp_t*)Results;  
  
  /* Initialize cmd structure for scm sys call */
  SysCallReq->addr  = CPU_REG_DUMP_START_ADDRESS;
  SysCallReq->size  = CPU_REG_DUMP_END_ADDRESS - CPU_REG_DUMP_START_ADDRESS;

  /* Locate QCOM_SCM_PROTOCOL */
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  /* Make SCM Sys call */
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_DUMP_SET_CPU_CTX_BUF_ID,
                                            TZ_DUMP_SET_CPU_CTX_BUF_ID_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->status != 1)
  {    
    Status = EFI_DEVICE_ERROR;
    DEBUG(( EFI_D_ERROR, "TZ_DUMP_SET_CPU_CTX_BUF failed, Status = (0x%x)\r\n", SysCallRsp->status));
    goto ErrorExit;
  }

  /* Setup APRegAddress */
  UINT64 APRegAddr = CPU_REG_DUMP_START_ADDRESS;
  Status = SetAPRegAddress(APRegAddr);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "Failed to set APRegAddress, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }

ErrorExit:
  return Status;
}

EFI_STATUS
EFIAPI
XpuSysCall(
  IN tz_xpu_violation_err_fatal_t XpuEnableFlag
)
{
  QCOM_SCM_PROTOCOL                *pQcomScmProtocol                  = NULL;
  EFI_STATUS                       Status                             = EFI_SUCCESS;
  UINT64                           Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                           Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_xpu_violation_err_fatal_req_t *SysCallReq                        = (tz_xpu_violation_err_fatal_req_t*)Parameters;
  tz_xpu_violation_err_fatal_rsp_t *SysCallRsp                        = (tz_xpu_violation_err_fatal_rsp_t*)Results;
  
  //Initialize cmd structure for scm sys call
  SysCallReq->config        = XpuEnableFlag;
  SysCallReq->spare         = 0;

  // Locate QCOM_SCM_PROTOCOL. 
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // make a SCM Sys call 
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_CONFIG_XPU_VIOLATION_ERR_FATAL,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->common_rsp.status != 1 || SysCallRsp->status != XpuEnableFlag)
  {    
    Status = EFI_DEVICE_ERROR;
    DEBUG(( EFI_D_ERROR, "TZ_CONFIG_XPU_VIOLATION_ERR_FATAL failed, Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
    goto ErrorExit;
  }

ErrorExit:
  return Status;
 
}

/**
  Returns information about platform memory map, categorized by subsystems.
  
  @param This     Instance pointer for this protocol
  @param Size     Pointer to the size in bytes of the MemoryMap buffer
  @param MemoryMap Ponter to the buffer for subsystem memory map, which is
                   an array of EFI_SUBSYSTEM_MEMORY_DESCRIPTOR
  @param DescriptorCount Pointer to the location containing the descriptor count
  
  @retval EFI_SUCCESS The function returned successfully.
  @retval EFI_INVALID_PARAMETER The parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL The supplied MemoryMap buffer was too small.
                               The current buffer size needed to hold the
                               memory map is returned in Size.

**/
EFI_STATUS
EFIAPI
GetSubsystemMemoryMap(
  IN EFI_SUBSYSTEM_MEMORY_PROTOCOL       *This,
  IN OUT UINTN                           *Size,
  IN OUT EFI_SUBSYSTEM_MEMORY_DESCRIPTOR *MemoryMap,
  OUT UINTN                              *DescriptorCount
  )
{
  EFI_STATUS  Status;

  Status = GetSubsysMemMap(Size, MemoryMap, DescriptorCount);
  return Status;
}

EFI_STATUS
EFIAPI
SetupLegacyUEFIDumpVariables ( VOID )
{
  EFI_STATUS  Status;
  UINT32 *CookieAddr;
  

  DEBUG ((EFI_D_INFO, "Setting Legacy UEFI Crash Dump variables\r\n"));

   CookieAddr = GET_GUID_HOB_DATA(MemoryCaptureGuidHob);
  DEBUG((EFI_D_INFO, "Set cookie address: 0x%x\n", *CookieAddr));

  /* Set variable for memory_capture_mode_address (cookie address) */
  Status = gRT->SetVariable(L"memory_capture_mode_address", &gQcomMemoryCaptureGuid,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
                            sizeof(UINT32), CookieAddr);
  if (EFI_ERROR(Status))
  {
    DEBUG ((EFI_D_ERROR, "SetVariable failed.(Status = %r)\n", Status));
    return Status;
  }

  return Status;
}

/**
  Entry function for the memory driver api.
  
  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.
  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
MemoryMapEntryPoint
(
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{
  /* Handle to install the memory protocol */
  EFI_HANDLE  Handle = NULL;
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 MemoryCaptureMode;
  BOOLEAN CarveOutModeEnabled = FALSE;
  UINT32 XpuEnableSize;
  UINT8 XpuEnableData = 0;
  tz_xpu_violation_err_fatal_t XpuEnableFlag = 0;

  /* See if the debug mode HOB is available */
  MemoryCaptureGuidHob = GetFirstGuidHob(&gQcomMemoryCaptureGuid);
  if ((MemoryCaptureGuidHob == NULL) || RETAIL)
  {
    DEBUG((EFI_D_WARN, "UEFI Crashdump Debug mode not enabled\n"));
    CarveOutModeEnabled = FALSE;
  }
  else
  {
    DEBUG((EFI_D_INFO, "UEFI Crashdump Debug mode enabled\n"));
    CarveOutModeEnabled = TRUE;
  }

#if 0
  Status = SetupLegacyUEFIDumpVariables();
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to setup legacy UEFI variables for crash dump (Status = 0x%08x)\r\n", Status));
    return Status;
  }
#endif

  /* Get Memory Capture Mode to check for legacy crash */
  Status = GetMemoryCaptureMode(&MemoryCaptureMode);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  DEBUG((EFI_D_INFO, "Memory Capture Mode value: 0x%08x\r\n", MemoryCaptureMode));
  
  XpuEnableSize = sizeof(UINT8);
  Status = gRT->GetVariable (L"XpuErrFatalEnable", 
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &XpuEnableSize,
                             &XpuEnableData);

  if (Status == EFI_SUCCESS && XpuEnableData == 1)
  {
    XpuEnableFlag = TZ_XPU_VIOLATION_ERR_FATAL_ENABLE;
    DEBUG(( EFI_D_WARN, "Xpu Violation configured as Error fatal\n"));
  }
  else
  {
    XpuEnableFlag = TZ_XPU_VIOLATION_ERR_FATAL_DISABLE;
    DEBUG(( EFI_D_INFO, "Xpu Violation not configured as Error fatal\n"));
  }
  Status = XpuSysCall(XpuEnableFlag);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "Failed to setup XPU violations\r\n"));
  }

  /* If legacy mode enabled */
  if (CarveOutModeEnabled)
  {
    /* Check if legacy crash happened - dump registers */
    if (MemoryCaptureMode == 1)
      Status = MemDumpInit();
    else
      Status = DumpSysCall();

    if (EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "Failed to initialize memory dump.(Status = %r)\r\n", Status));
      return Status;
    }

    /* Publish the memory map protocol */
    Status = gBS->InstallMultipleProtocolInterfaces( 
                  &Handle, 
                  &gEfiMemoryMapProtocolGuid, 
                  (void **)&gMemoryMapProtocol, NULL );
    if (EFI_ERROR(Status))
      DEBUG((EFI_D_ERROR, "Failed to install memory protocol.(Status = %r)\r\n", Status));
  }
  else
  {
    /* Setup dump region */
    Status = DumpSysCall();
  }

  return Status;
}
