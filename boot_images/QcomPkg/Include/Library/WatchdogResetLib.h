/** @file WatchdogResetLib.h
   
  Library implementation to support Watchdog Standalone call.

  Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved. 

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/24/12   rks      Created
=============================================================================*/
#ifndef __WATCHDOGRESETLIB_H__
#define __WATCHDOGRESETLIB_H__


/**
  Reset the MSM. 
   
**/

VOID
EFIAPI
 WatchdogReset (
  VOID
);

#endif /* __WATCHDOGRESETLIB_H__ */
