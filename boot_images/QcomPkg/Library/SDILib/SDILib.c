/*********************************************************************
 * @file SDILib.c
 *
 * @brief SDI reset support
 *
 * Copyright (c) 2017,2018 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    ---------------------------------------------------
 08/11/16   C_mk    Initial revision.
 ========================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/TzRuntimeLib.h>
#include <Protocol/EFIScm.h>
#include <Include/scm_sip_interface.h>



/** 
   TZ fast call to disable SDI 
**/
EFI_STATUS
EFIAPI
SDIDisable()
{ 
  EFI_STATUS                        Status                             = EFI_SUCCESS;
  UINT64                            Parameters[2]                      = {0};
  tz_config_hw_for_ram_dump_req_t   *SysCallReq                        = (tz_config_hw_for_ram_dump_req_t*)Parameters;
 
  //Initializing cmd structure for scm sys call
  SysCallReq->disable_wd_dbg        = 1;
  SysCallReq->boot_partition_sel    = 0;

  // make a TZ fast call
  Status = TzFastcall (TZ_CONFIG_HW_FOR_RAM_DUMP_ID, TZ_CONFIG_HW_FOR_RAM_DUMP_ID_PARAM_ID, Parameters, 1);

  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_CONFIG_HW_FOR_RAM_DUMP_ID, Status = (0x%x)\r\n", Status));
  }
  return Status;
}

/** 
   TZ fast call to disable SDI and  initiate warm reset
**/
EFI_STATUS
EFIAPI
SDIDisableAndReset()
{ 
  EFI_STATUS                        Status                             = EFI_SUCCESS;
  UINT64                            Parameters[2]                      = {0};
  tz_secure_wdog_trigger_req_t      *SysCallReq                        = (tz_secure_wdog_trigger_req_t*)Parameters;
 
  //Initializing cmd structure for scm sys call
  SysCallReq->disable_debug        = 1;

  // make a TZ fast call
  Status = TzFastcall (TZ_SECURE_WDOG_TRIGGER_ID, TZ_SECURE_WDOG_TRIGGER_ID_PARAM_ID, Parameters, 1);

  if (EFI_ERROR (Status)) 
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_SECURE_WDOG_TRIGGER_ID, Status = (0x%x)\r\n", Status));
  }
  return Status;
}