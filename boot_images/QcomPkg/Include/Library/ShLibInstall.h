/** @file ShLibInstall.h
   
  Contains Shared Library Mgr Installer function interface.

  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved. 
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/22/13   vk      Update return types for 64 bit
 09/17/12   yg      Initial Version

=============================================================================*/
#ifndef __SH_LIB_INSTALL__H__
#define __SH_LIB_INSTALL__H__

#include <Uefi.h>

/* This function installs the Shared Library Loader Mgr into system.
 * Only SEC should be calling this function only once during system Init.
 * For all client related access functions to Install/Load shared libraries
 * refer to ShLib.h */
EFI_STATUS ShLibMgrInit (VOID);


#endif /* __SH_LIB_INSTALL__H__ */

