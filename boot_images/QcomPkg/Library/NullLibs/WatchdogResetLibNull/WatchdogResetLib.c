/** @file WatchdogResetLib.c
   
  Library implementation to support Watchdog Standalone call.

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY
							  
 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/24/12   rks     Created
=============================================================================*/

#include <Library/BaseLib.h>

/**
  Reset the MSM. 
   
**/
VOID
EFIAPI
 WatchdogReset  (
  VOID
)
{
  /* Wait for watchdog to expire */
  CpuDeadLoop ();
}
