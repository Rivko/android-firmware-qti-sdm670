/** @file FuseControlLib.c
   
  Contains target specific library functions related to fuses.
 
  Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.  All Rights
  Reserved. Qualcomm Technologies Proprietary and Confidential.
   
**/

/*
  ? Status.value bits returned from tz:
- Bit 0: secboot enabling check failed (auth_en is not blown or oem pk hash is not programmed for qpsa signing)
- Bit 1: Sec HW key is not programmed
- Bit 2: debug disable check failed (include checking ?debug override?, which should not be blown if debug is disabled)
- Bit 3: Anti-rollback check failed
- Bit 4: fuse config check failed (for fec_en and read-write permission)
- Bit 5: rpmb check failed (rpmb is not provisioned)
? IsProductionDevice check:
- Check bits 0 AND 1 AND 2 in status.value
? JTAG disable check:
- Check bit 2 only in status.value
? RPMB provision check:
- Check bit 5 only in status.value
? SHK check:
- Check bit 1 only in status.value
? Rollback check:
- Check bit 3 only in status.value
*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/13/15   sm      Improved SCM call and removed Init call
 01/08/15   sk      Added for HSTI support - rpmb, shk, rollbackcheck APIs
 10/09/14   sk      Added support for constructor and exitboot services callback
 09/12/14   sk      Removed msmhwio header dependency
 08/08/14   rp      Enabled fuselib check through TZ API
 04/10/13   shl     Added production device detection
 03/22/13   ag      enable fusecontrol check 
 02/01/13   shl     Warning fix
 09/11/12   shl     Added Not block checking
 05/25/12   niting  Created
=============================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <HALhwio.h>
#include <Library/FuseControlLib.h>
#include <Library/UefiCfgLib.h>

#include <Protocol/EFIScm.h>                        // for QCOM_SCM_PROTOCOL
#include <Include/scm_sip_interface.h>              // for TZ Structs
#include <Library/DebugLib.h>                       // for Debug(())
#include <Library/UncachedMemoryAllocationLib.h>    // for UncachedAllocatePool
#include <Library/UefiBootServicesTableLib.h>       // for gBS
#include <Guid/EventGroup.h>
 
#define ERROR_SECURITY_STATE 0xFFFFFFFF

EFI_EVENT  FuseExitBootServicesEvent;

// FUNCTIONS
UINT32
EFIAPI
GetSecurityState(void)
{
  QCOM_SCM_PROTOCOL          *pQcomScmProtocol = NULL;
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT64                      Parameters[SCM_MAX_NUM_PARAMETERS] = {0};
  UINT64                      Results[SCM_MAX_NUM_RESULTS]       = {0};
  tz_get_secure_state_rsp_t  *SysCallRsp = (tz_get_secure_state_rsp_t*)Results;
  UINT32                      SecurityStateReturn = 0x0;
  
  // Locate QCOM_SCM_PROTOCOL. 
  Status = gBS->LocateProtocol ( &gQcomScmProtocolGuid, 
                                 NULL, 
                                 (VOID **)&pQcomScmProtocol
                               );
  if (EFI_ERROR(Status))// || (pQcomScmProtocol == NULL)) 
  {
    DEBUG((EFI_D_WARN, " Locate SCM Protocol failed, Status =  (0x%x)\r\n", Status));
	  Status = EFI_NOT_FOUND;
	  goto ErrorExit;
  }
  // Make ScmSipSysCall 
  Status = pQcomScmProtocol->ScmSipSysCall (pQcomScmProtocol,
                                            TZ_INFO_GET_SECURE_STATE,
                                            TZ_INFO_GET_SECURE_STATE_PARAM_ID,
                                            Parameters,
                                            Results
                                            );
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, " ScmSipSysCall() failed, Status = (0x%x)\r\n", Status));
	  Status = ERROR_SECURITY_STATE;
    goto ErrorExit;
  }

  if (SysCallRsp->common_rsp.status != 1)
  {
    DEBUG((EFI_D_WARN," ScmSysCall() failed, common_rsp Status = (0x%x)\r\n", SysCallRsp->common_rsp.status));
	  Status = ERROR_SECURITY_STATE;
    goto ErrorExit;
  }
  // Parse the return value and assign
  SecurityStateReturn = SysCallRsp->status_0;
  
ErrorExit:

  if(EFI_SUCCESS != Status)
    return ERROR_SECURITY_STATE; //Error during getting security state

  return SecurityStateReturn; //Return the security status value
}


/**
  Check if the necessary conditions are satisfied to auto provision the RPMB

  @retval  TRUE    Auto Provisioning enabled
  @retval  FALSE   Auto Provisioning disabled

**/
BOOLEAN 
EFIAPI
AutoProvisionRPMBEnabled(void)
{
    return FALSE;
}


/**
 Check if the debug disable fuses are blown

  @retval TRUE Any of the related DEBUG_DISABLE fuses are blown
  @retval FALSE None of the related DEBUG_DISABLE fuses are blown

**/
BOOLEAN 
EFIAPI
JTAGDebugDisableFusesBlown(void)
{
  
  UINT32 secure_state = 0;
  
  secure_state = GetSecurityState();
  if(secure_state == ERROR_SECURITY_STATE)
  {  
    return FALSE;
  }
  
  //Check the Bit#2 for debug disable check
  if(!((secure_state >> 2) & 0x1))
    return TRUE; //debug disable check pass, debug disabled

  return FALSE; //debug disable check failed, debug enabled
}


/**
 Check if this is a production device

  @retval TRUE if it is
  @retval FALSE if it is not

**/
BOOLEAN 
EFIAPI
IsProductionDevice(void)
{
  UINT32 secure_state = 0;

  secure_state = GetSecurityState();
  if( ERROR_SECURITY_STATE == secure_state )
  {
    return FALSE;
  }


  // Bit#0 AND Bit#1 AND Bit#2 for production Device
  if( (!(secure_state & 0x1) && !((secure_state >> 1) & 0x1) && !((secure_state >> 2) & 0x1) ))
    return TRUE; // production Device Check PASS

  return FALSE; // Production Device Check FAIL, not a production device
}

/**
  Check if RPMB is provisioned

  @retval TRUE if it is
  @retval FALSE if it is not

**/
BOOLEAN 
EFIAPI
IsRpmbProvisioned(void)
{
  UINT32 secure_state = 0;

  secure_state = GetSecurityState();
  if( ERROR_SECURITY_STATE == secure_state )
  {
    return FALSE;
  }

  // Bit#5 rpmb provision check
  if(! (((secure_state >> 5) & 0x1)))
    return TRUE; // rpmb provisioned

  return FALSE; // rpmb provision Check FAIL
}

/**
  Check if SHK is provisioned

  @retval TRUE if it is
  @retval FALSE if it is not

**/
BOOLEAN 
EFIAPI
IsShkProvisioned(void)
{
  UINT32 secure_state = 0;

  secure_state = GetSecurityState();
  if( ERROR_SECURITY_STATE == secure_state )
  {
    return FALSE;
  }

  // Bit#1 shk provision check
  if(! (((secure_state >> 1) & 0x1)))
    return TRUE; // shk provisioned

  return FALSE; // shk provision Check FAIL
}

/**
  Check if rollback is enabled

  @retval TRUE if it is
  @retval FALSE if it is not

**/
BOOLEAN 
EFIAPI
IsRollbackEnabled(void)
{
  UINT32 secure_state = 0;

  secure_state = GetSecurityState();
  if( ERROR_SECURITY_STATE == secure_state )
  {
    return FALSE;
  }

  // Bit#3 rollback enable check
  if(! (((secure_state >> 3) & 0x1)))
    return TRUE; // rollback enabled

  return FALSE; // rollback check FAIL
}

