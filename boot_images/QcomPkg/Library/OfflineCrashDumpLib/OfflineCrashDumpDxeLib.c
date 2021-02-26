/** @file OfflineCrashDumpDxeLib.c
 
  Offline Crash Dump support
 
  Copyright (c) 2013-2016 by Qualcomm Technologies, Inc. All Rights Reserved. 
  
*/
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    ---------------------------------------------------
 01/13/17   ly      Remove setting OCD flag in shared IMEM, only read from it.
 09/08/16   ly      Update OfflineMemoryDumpCapable field based on variable OfflineMemoryDumpCapability
 08/24/16   ly      Update offline crash dump and reset buffer 
 08/11/16   bh      Move cookie logic to new lib
 01/07/16   na      Change  EFI_D_ERROR to EFI_D_WARN in IsMemoryDumpEnabled()
 07/29/15   ao      bring in crash Dump related code from Sec.c
 06/25/15   bh      Change guid name
 4/14/15    to      Updates for 8996 -- use of SCM interface
 09/02/14   shireesh Added Code to update APRegADDRESS in UEFI variable 
 08/15/14   sm      Switched to new SCM API
 07/11/14   shireesh Enabled printing of dumpcapability for validation purposes
 05/16/14   shireesh Fixed bug in previous IsMemoryDumpEnabled fix
 04/10/14   niting  Fixed warnings
 01/28/14   shl     Added mMemoryCaptureMode check in IsMemoryDumpEnabled()
 11/01/13   vk      Fix 64 bit compiler warnings
 07/24/13   yg      Reduce Logging
 05/10/13   niting  Add IsMemoryDumpEnabled
 05/09/13   vk      Add GetSavedMemoryCaptureMode
 03/23/13   niting  Initial revision.
========================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/HobLib.h>
#include <Library/OfflineCrashDump.h>
#include <Library/UefiLib.h>
#include <Protocol/EFIScm.h>
#include  <Library/DBIDump.h>
#include <Include/scm_sip_interface.h>
#include <Library/CookieLib.h>

STATIC UINT32 GlobalMemCaptureModeValue = 0;

/** 
   SCM Sys Call to disable WDOG DEBUG (GCC_RESET_DEBUG_ENABLE) 
**/
EFI_STATUS
EFIAPI
SDIDisableSysCall()
{ 
  QCOM_SCM_PROTOCOL                 *pQcomScmProtocol                  = NULL;
  EFI_STATUS                        Status                             = EFI_SUCCESS;
  UINT64                            Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                            Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_config_hw_for_ram_dump_req_t   *SysCallReq                        = (tz_config_hw_for_ram_dump_req_t*)Parameters;
  tz_syscall_rsp_t                  *SysCallRsp                        = (tz_syscall_rsp_t*)Results;
 
  //Initializing cmd structure for scm sys call
  SysCallReq->disable_wd_dbg        = 1;
  SysCallReq->boot_partition_sel    = 0;

  // Locate QCOM_SCM_PROTOCOL. 
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_INFO, " Locate SCM Protocol failed for Wdog disable, Status =  (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // make a SCM Sys call 
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_CONFIG_HW_FOR_RAM_DUMP_ID,
                                            TZ_CONFIG_HW_FOR_RAM_DUMP_ID_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "ScmSipSysCall() failed for WDOG DEBUG disable, Status = (0x%x)\r\n", Status));
    goto ErrorExit;
  }
  if (SysCallRsp->status != 1)
  {    
    Status = EFI_DEVICE_ERROR;
    DEBUG(( EFI_D_ERROR, "TZ_CONFIG_HW_FOR_RAM_DUMP failed, Status = (0x%x)\r\n", SysCallRsp->status));
    goto ErrorExit;
  }

ErrorExit:
  return Status;
}

/**
  InCarveoutMode - Check if the device is in the carveout mode (memory capture mode)
**/
EFIAPI
BOOLEAN
InCarveoutMode(
  VOID
  )
{
  EFI_HOB_GUID_TYPE *GuidHob;
  UINTN             *MemoryCaptureModeAddr;
  volatile UINT32   *MemoryCaptureMode;
  BOOLEAN           CarveoutMode = FALSE;
    
  /* Check if the device is in carveout mode */
  if ((GuidHob = GetFirstGuidHob(&gQcomMemoryCaptureGuid)) != NULL)
  {
    /* Get the address from the HOB */
    MemoryCaptureModeAddr = GET_GUID_HOB_DATA(GuidHob);

    /* pCrashDumpAddr points to the location that has the cookie value */
    MemoryCaptureMode = (UINT32 *)(*MemoryCaptureModeAddr);
    if (*MemoryCaptureMode == 1)
      CarveoutMode = TRUE;
  }

  return CarveoutMode;
}

STATIC
BOOLEAN
EFIAPI
IsMemDumpAllowedSysCall()
{ 
  QCOM_SCM_PROTOCOL *pQcomScmProtocol = NULL;
  EFI_STATUS Status = EFI_SUCCESS;
  BOOLEAN MemDumpAllowed = FALSE;
  UINT64 Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64 Results[SCM_MAX_NUM_RESULTS] = {0};
  tz_security_allows_mem_dump_rsp_t *SysCallRsp = (tz_security_allows_mem_dump_rsp_t*)Results;
  
  /* Locate QCOM_SCM_PROTOCOL */
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if( EFI_ERROR(Status)) 
  {
    DEBUG(( EFI_D_ERROR, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
    goto ErrorExit;
  }

  // make a SCM Sys call 
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID,
                                            TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP_ID_PARAM_ID,
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
    DEBUG(( EFI_D_ERROR, "TZ_DUMP_SECURITY_ALLOWS_MEM_DUMP failed, Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
    goto ErrorExit;
  }
  else
  {
    MemDumpAllowed = TRUE;
    DEBUG(( EFI_D_INFO, "Memory Dumping is allowed\r\n"));
  }
/* TODO: TZ.BF.4.0 (CRM 251) does not use this interface yet. Update when TZ uses this
  if (SysCallRsp->allowed != 0)
  {
    MemDumpAllowed = TRUE;
    DEBUG(( EFI_D_ERROR, "Memory Dumping is allowed\r\n"));
  } 
*/

ErrorExit:

  return MemDumpAllowed;
}

EFI_STATUS
EFIAPI
SetAPRegAddress(UINT64 Address)
{
  EFI_STATUS Status;
  UINT64 APRegAddr = Address;
  UINT64 APRegAddrSize = sizeof(APRegAddr);
  Status = gRT->SetVariable (AP_REG_ADDR_VARNAME, 
                             &gOSAVendorGuid, 
                             (EFI_VARIABLE_NON_VOLATILE | 
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_RUNTIME_ACCESS),
                             APRegAddrSize, 
                             &APRegAddr);

#ifdef ENABLE_OCD_DEBUG
  if (Status == EFI_SUCCESS)
    DEBUG((EFI_D_ERROR, "Updating %s to: 0x%llx\r\n", AP_REG_ADDR_VARNAME,  APRegAddr));
#endif

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
InitializeOfflineCrashDumpConfigTable( 
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE **OfflineCrashDumpConfigTable 
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *mOfflineCrashDumpConfigTable = NULL;
  UINT32  SharedIMEMBaseAddr;
  UINT32* ApregAddPtr;    //TODO: Clean up for 64 bit

  if (OfflineCrashDumpConfigTable == NULL)
    return EFI_INVALID_PARAMETER;

  /* Initialize Offline Crash Dump Config Table */
  mOfflineCrashDumpConfigTable = 
    (EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *)AllocateRuntimePool(sizeof(EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE));
  if (mOfflineCrashDumpConfigTable == NULL)
  {
    *OfflineCrashDumpConfigTable = NULL;
    return EFI_OUT_OF_RESOURCES;
  }

  mOfflineCrashDumpConfigTable->Version = OFFLINE_CRASHDUMP_CONFIG_TABLE_VERSION;
  mOfflineCrashDumpConfigTable->AbnormalResetOccurred = 0;
  mOfflineCrashDumpConfigTable->OfflineMemoryDumpCapable = 0;

  DEBUG((EFI_D_INFO, "Initializing Offline Crash Dump Configuration Table to default values:\r\n"));
  DEBUG((EFI_D_INFO, "  Version: 0x%08x\r\n", mOfflineCrashDumpConfigTable->Version));
  DEBUG((EFI_D_INFO, "  AbnormalResetOccurred: 0x%08x\r\n", mOfflineCrashDumpConfigTable->AbnormalResetOccurred));
  
  Status = gBS->InstallConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                          (VOID*)mOfflineCrashDumpConfigTable);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "ERROR: Failed to install Offline Crash Dump Configuration Table\r\n"));
    return Status;
  }

  *OfflineCrashDumpConfigTable = mOfflineCrashDumpConfigTable;
    /* Set the IMEM Cookie with the address to the DbiDumpTable */
  Status = GetConfigValue ("SharedIMEMBaseAddr", &SharedIMEMBaseAddr); 
  if (Status != EFI_SUCCESS ) 
  {
    DEBUG((EFI_D_ERROR, "ERROR: Failed to get valide SharedIMEMBaseAddr from cfg file\r\n"));
    return Status;
  }
  /* TODO: Fix this when Shared IMEM table is fixed to support 64-bit addressing */
#ifdef _MSC_VER
#pragma warning( disable : 4312 )
#endif
  ApregAddPtr = ( UINT32 *) (( unsigned long) ( SharedIMEMBaseAddr + DBI_SHARED_IMEM_COOKIE_OFFSET) );
#ifdef _MSC_VER
#pragma warning( default : 4312 )
#endif

  Status = SetAPRegAddress((UINT64)*ApregAddPtr);

  if (Status != EFI_SUCCESS)
  {
      DEBUG(( EFI_D_ERROR, "Failed to set APRegAddress, Status = (0x%x)\r\n", Status));
      return Status;
  }
  DEBUG((EFI_D_INFO, "  Successfully installed Offline Crash Dump Configuration Table\r\n"));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
DisableOfflineCrashDump ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  /* Clear memory capture mode to disable crash dump */
  Status = SetMemoryCaptureMode(OFFLINE_CRASH_DUMP_DISABLE);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to clear Memory Capture Mode\r\n"));
    return Status;
  }

  /* Clear DLOAD cookies if flag is set */
  Status = ClearDLOADCookie();
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to clear DLOAD cookies\r\n"));
    return Status;
  }

  return Status;
}

UINT32
EFIAPI
GetSavedMemoryCaptureMode ( VOID )
{
  
  return GlobalMemCaptureModeValue;
}

STATIC
EFI_STATUS
EFIAPI
RestoreMemoryCaptureMode ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 MemoryCaptureModeValue = 0;
  
  /* Check if legacy offline crash dump occurred */

  //Status = GetMemoryCaptureMode(&MemoryCaptureModeValue);
  MemoryCaptureModeValue = GlobalMemCaptureModeValue;

  DEBUG((EFI_D_INFO, "Successfully retrieved Memory Capture Mode value HOB\r\n"));
  DEBUG((EFI_D_INFO, "  Memory Capture Mode HOB value: 0x%08x\r\n", MemoryCaptureModeValue));

  /* Restore legacy crash dump into Memory Capture Mode if applicable */
  if (MemoryCaptureModeValue == OFFLINE_CRASH_DUMP_LEGACY_ENABLE)
  {
    Status = SetMemoryCaptureMode(OFFLINE_CRASH_DUMP_LEGACY_ENABLE);
    if (Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "Failed to restore Memory Capture Mode\r\n"));
      return Status;
    }

    DEBUG((EFI_D_INFO, "Restored Memory Capture Mode to 0x%02x\r\n", MemoryCaptureModeValue));

    Status = SetDLOADCookie();
    if (Status != EFI_SUCCESS)
    {
      DEBUG((EFI_D_ERROR, "Failed to restore DLOAD cookies\r\n"));
      return Status;
    }
  }

  return Status;
}


EFI_STATUS
EFIAPI
UpdateOfflineCrashDumpConfigTable( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *OfflineCrashDumpConfigTable = NULL;
  UINT8 Data;
  UINTN DataSize;
  UINT32 AbnormalResetOccurredVal;
  BOOLEAN MemDumpAllowed = FALSE;

  DEBUG((EFI_D_INFO, "OfflineCrashDump DXE Initialization Start\r\n"));
  PrintOfflineCrashDumpValuesDxe();

  /* Initialize config table */
  Status = InitializeOfflineCrashDumpConfigTable(&OfflineCrashDumpConfigTable);
  if ((Status != EFI_SUCCESS) || (OfflineCrashDumpConfigTable == NULL))
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize Offline Crash Dump Configuration Table\r\n"));
    return Status;
  }
  
  /* Fill in AbnormalResetOccurred value from previous boot */
  Status = GetAbnormalResetOccurred(&AbnormalResetOccurredVal);  
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Could not retreive HOB for AbnormalResetOccurredVal\r\n"));
    Status = EFI_LOAD_ERROR;
    goto ExitOCD;
  }
  
 if (!IsAbnormalResetOccurredValid(AbnormalResetOccurredVal))
 {
   /* Initialize HOB to 0 */
   AbnormalResetOccurredVal = ABNORMAL_RESET_DISABLE;
 }

  DEBUG((EFI_D_INFO, "Successfully retrieved AbnormalResetOccurredVal HOB\r\n"));
  DEBUG((EFI_D_INFO, "  AbnormalResetOccurred HOB value: 0x%08x\r\n", AbnormalResetOccurredVal));
  OfflineCrashDumpConfigTable->AbnormalResetOccurred = AbnormalResetOccurredVal;
  //Device suppose to have reset reason enabled all times
  OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable |= RESETREASON_ENABLED;
 
  /* Check if crash dump is allowed */
  MemDumpAllowed = IsMemDumpAllowedSysCall();

  /* Update config table */
  if (!MemDumpAllowed)
  {
    /* Disable crash dump if not allowed */
    OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable &= ~MEMORYDUMP_ENABLED;
    Status = DisableOfflineCrashDump();
    goto ExitOCD;
  }
  
  /* Enable offline crash dump */
  OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable |= MEMORYDUMP_ENABLED;

  /* Check if EnableLegacySBLDump is set - this has precedence over all other cookie settings */
  DataSize = sizeof(Data);
  Status = gRT->GetVariable (ENABLE_LEGACY_SBL_DUMP_VARNAME,
                             &gQcomTokenSpaceGuid,
                             NULL,
                             &DataSize,
                             &Data);
  if ((Status == EFI_SUCCESS) && (Data == 0x1))
  {
    DEBUG((EFI_D_WARN, "Enabling SD card dump\r\n"));
    SetMemoryCaptureMode(OFFLINE_CRASH_DUMP_DISABLE);
    goto ExitOCD;
  }

  /* Determine whether offline crash dump needs to be enabled or not */
  DataSize = sizeof(Data);
  Status = gRT->GetVariable (OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME,
                             &gOSAVendorGuid,
                             NULL,
                             &DataSize,
                             &Data);
  //////////////////////////////////////////////////////////////////////////////////////// 
  //To-remove later on once we figure out why HLOS disable offline ram dump
  //Alway enable ram dump for now
  Data |= MEMORYDUMP_ENABLED;
  ////////////////////////////////////////////////////////////////////////////////////////
  /* If offline crash dump is enabled or OfflineMemoryDumpUseCapability does not exist,
     do not clear any cookies */
  if (((Status == EFI_SUCCESS) && ((Data&OFFLINE_MEMORY_DUMP_USE_CAPABILITY_GPT_BMSK) == OFFLINE_MEMORY_DUMP_USE_CAPABILITY_GPT_BMSK)) ||
      (Status == EFI_NOT_FOUND))
  {
    DEBUG((EFI_D_INFO, "Enabling offline crash dump, OfflineMemoryDumpUseCapability: %a\r\n", 
           (Status == EFI_SUCCESS) ? "0x1" : "Not Found"));
    goto RestoreMemoryCapture;
  }
  else if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Error while retrieving OfflineMemoryDumpUseCapability: 0x%08x\r\n", Status));
    goto ExitOCD;
  }

  /* At this point, it means offline_memory_dump_use_capability is DISABLED, RAMDUMP NOT ALLOWED: 
     1). Disable Offline ram dump: clear memory capture mode and dload cookie.
     2). Disable SDI WDOG DEBUG
     3). Update OfflineMemoryDumpCapable field in OfflineCrashDumpConfigTable based on variable reading
   */   
  DEBUG((EFI_D_INFO, "Disabling offline crash dump, OfflineMemoryDumpUseCapability: %a\r\n"));         
  OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable &= ~MEMORYDUMP_ENABLED;
  Status = DisableOfflineCrashDump();
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to disable Offline Ram Dump Mode\r\n"));
    goto ExitOCD;
  }
  
  /* Disable SDI WDOG DEBUG */
  Status = SDIDisableSysCall();
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to disable SDI WDOG DEBUG\r\n"));
    goto ExitOCD;
  }

RestoreMemoryCapture:
  /* Only restore Memory Capture Mode if legacy crash occurred */
  Status = RestoreMemoryCaptureMode();
  if (Status != EFI_SUCCESS)
    goto ExitOCD;

ExitOCD:
  DEBUG((EFI_D_INFO, "OfflineCrashDump DXE Initialization Complete\r\n"));
  PrintOfflineCrashDumpValuesDxe();
  DEBUG((EFI_D_WARN, "DumpCapability:0x%08x\r\n", OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable));
  return Status;
}

VOID
EFIAPI
PrintOfflineCrashDumpValuesDxe ( VOID )
{
#ifdef ENABLE_OCD_DEBUG
  UINT32 MemoryCaptureMode = 0;
  UINT32 AbnormalResetOccurred = 0;
  BOOLEAN DLOADCookieSet = FALSE;
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *OfflineCrashDumpConfigTable = NULL;

  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Value;
  UINTN Size = sizeof(Value);

  DEBUG((EFI_D_ERROR, "************Printing Offline Crash Dump Values**************\r\n"));
  Status = EfiGetSystemConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                          (VOID**)&OfflineCrashDumpConfigTable);

  if (Status == EFI_NOT_FOUND)
  {
    DEBUG((EFI_D_ERROR, "  Offline Crash Dump Config Table does not exist in System Configuration Table\r\n"));
    Status = EFI_SUCCESS;
  }
  else if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "ERROR: Could not successfully get item from System Configuration Table\r\n"));
    return;
  }

  if (OfflineCrashDumpConfigTable == NULL)
  {
    DEBUG((EFI_D_ERROR, "  Offline Crash Dump Configuration Table: Uninitialized\r\n"));
  }
  else
  {
    DEBUG((EFI_D_ERROR, "  Offline Crash Dump Configuration Table:\r\n"));
    DEBUG((EFI_D_ERROR, "   Version: 0x%08x\r\n", OfflineCrashDumpConfigTable->Version));
    DEBUG((EFI_D_ERROR, "   AbnormalResetOccurred: 0x%08x\r\n", OfflineCrashDumpConfigTable->AbnormalResetOccurred));
    DEBUG((EFI_D_ERROR, "   OfflineMemoryDumpCapable: 0x%08x\r\n", OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable));
  }

  /* Print OfflineMemoryDumpUseCapability */
  Status = gRT->GetVariable(OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME,
                            &gOSAVendorGuid,
                            NULL,
                            &Size,
                            &Value);
  if (Status != EFI_SUCCESS)
    DEBUG((EFI_D_ERROR, "  Could not retrieve UEFI variable: OfflineMemoryDumpUseCapability, Status = 0x%08x\r\n", Status));
  else
    DEBUG((EFI_D_ERROR, "  OfflineMemoryDumpUseCapability: 0x%02x\r\n", Value));

  /* Print EnableLegacySBLDump */
  Size = sizeof(Value);
  Status = gRT->GetVariable(ENABLE_LEGACY_SBL_DUMP_VARNAME,
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &Size,
                            &Value);
  if (Status != EFI_SUCCESS)
    DEBUG((EFI_D_ERROR, "  Could not retrieve UEFI variable: EnableLegacySBLDump, Status = 0x%08x\r\n", Status));
  else
    DEBUG((EFI_D_ERROR, "  EnableLegacySBLDump: 0x%02x\r\n", Value));

  DEBUG((EFI_D_ERROR, "  Offline Crash Dump Values:\r\n"));
  Status = GetMemoryCaptureMode(&MemoryCaptureMode);
  if (Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "    MemoryCaptureMode: 0x%08x\r\n", MemoryCaptureMode));
  }

  Status = GetAbnormalResetOccurred(&AbnormalResetOccurred);
  if (Status == EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "    AbnormalResetOccurred: 0x%08x\r\n", AbnormalResetOccurred));
  }

  DLOADCookieSet = IsDLOADCookieSet();
  DEBUG((EFI_D_ERROR, "    Is DLOADCookieSet?: %a\r\n", DLOADCookieSet ? "Set" : "Cleared" ));
  DEBUG((EFI_D_ERROR, "************************************************************\r\n"));
#endif
}

EFIAPI
BOOLEAN
IsMemoryDumpEnabled( VOID )
{
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *OfflineCrashDumpConfigTable = NULL;
  EFI_STATUS Status = EFI_SUCCESS;

  Status = EfiGetSystemConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                          (VOID**)&OfflineCrashDumpConfigTable);

  if ((Status != EFI_SUCCESS) || (OfflineCrashDumpConfigTable == NULL))
  {
    DEBUG(( EFI_D_ERROR, "Offline Crash Dump Config Table does not exist in System Configuration Table\r\n"));
    return FALSE;
  }

  if ( OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable & MEMORYDUMP_ENABLED )
  {
    DEBUG(( EFI_D_WARN, "offline memory dump is enabled\r\n"));
    return TRUE;
  }
  else
  {
    DEBUG(( EFI_D_ERROR, "offline memory dump is disabled \r\n"));
    return FALSE;
  }
}

EFIAPI
BOOLEAN
IsOfflineCrashDumpEnabled( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mMemoryCaptureMode = 0;

  Status = GetMemoryCaptureMode(&mMemoryCaptureMode);
  if (Status != EFI_SUCCESS )
    return FALSE;

  if( mMemoryCaptureMode != OFFLINE_CRASH_DUMP_ENABLE)
    return FALSE;

  /* Cookie is enabled for offline crash dump, check if TZ enables dumps also */
  return IsMemoryDumpEnabled();
}

VOID
HandleCrashDump ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  /* HLOS Cookie Addr is offset 0xC from start of cookie base */
  UINT32 MemoryCaptureModeValue = 0;

  DEBUG((EFI_D_INFO, "SEC Offline Crash Dump Handling Start\r\n"));
  PrintOfflineCrashDumpValues();
  
  /* Always enable SBL dumps */
  SetDLOADCookie();

  /* Build HOB to pass up current Memory Capture Mode value */
  Status = GetMemoryCaptureMode(&MemoryCaptureModeValue);
  if (Status != EFI_SUCCESS)
  {
    DEBUG((EFI_D_ERROR, "Failed to retrieve Memory Capture Mode value\r\n"));
    return;
  }
  if (!IsMemoryCaptureModeValid(MemoryCaptureModeValue))
  {
    /* Initialize HOB to 0 */
    MemoryCaptureModeValue = OFFLINE_CRASH_DUMP_DISABLE;
  }

  GlobalMemCaptureModeValue = MemoryCaptureModeValue;
  
  /* Enable SBL offline crash dump */
  SetMemoryCaptureMode(OFFLINE_CRASH_DUMP_ENABLE);

  DEBUG((EFI_D_INFO, "SEC Offline Crash Dump Handling Complete\r\n"));
  PrintOfflineCrashDumpValues();
}

