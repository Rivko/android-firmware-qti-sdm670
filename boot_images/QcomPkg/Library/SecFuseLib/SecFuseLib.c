/*===========================================================================

                              SEC Fuse Lib

GENERAL DESCRIPTION
  This module provides support to sec fuse lib check. 

Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
06/22/17   na         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <scm_sip_interface.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

extern UINT64 GetSecurityState (OUT tz_get_secure_state_rsp_t*);

/*===========================================================================
                    VARIABLE DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
                        LOCAL FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================
**  Function :  JTAGDebugDisableFusesBlown
** ==========================================================================
*/
/**
 Check if the debug disable fuses are blown

  @retval TRUE Any of the related DEBUG_DISABLE fuses are blown
  @retval FALSE None of the related DEBUG_DISABLE fuses are blown

**/
BOOLEAN 
EFIAPI
JTAGDebugDisableFusesBlown(void)
{
  UINTN                      Status;
  tz_get_secure_state_rsp_t  TzResponse;
  UINTN                      SecurityState = 0; 
  
  Status = GetSecurityState(&TzResponse);
  if(Status != 0) 
  {  
    DEBUG((EFI_D_ERROR, "GetSecurityState Failed!\r\n"));
    ASSERT(Status == 0);
    CpuDeadLoop();
    return FALSE;
  }

  SecurityState = TzResponse.status_0;
  
  //Check the Bit#2 for debug disable check
  if(!((SecurityState >> 2) & 0x1)) 
  {
    return TRUE; //debug disable check pass, debug disabled
  }

  return FALSE; //debug disable check failed, debug enabled
}
