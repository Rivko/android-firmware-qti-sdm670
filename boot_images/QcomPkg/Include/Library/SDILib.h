/** @file SDILib.h
   
  Offline Crash Dump Definitions

  Copyright (c) 2017 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/24/16   c_mk    Initial Version 
 =============================================================================*/
 
#ifndef __SDILIB_H__
#define __SDILIB_H__
 
#include <Uefi.h>
 
/** TZ fast call to disable SDI*/
EFI_STATUS
EFIAPI
SDIDisable( VOID );

/** TZ fast call to disable SDI and  initiate warm reset*/
EFI_STATUS
EFIAPI
SDIDisableAndReset( VOID );

#endif /* __SDILIB_H__ */