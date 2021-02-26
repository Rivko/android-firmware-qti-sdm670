/** @file FwProvisionLib.h
   
  Header for externally-available functions of the 
  FwProvisionLib 

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/05/16     Mic         Moved from OSAppPkg to QComPkg
 2011/10/12     jthompso    Initial version

=============================================================================*/
#ifndef _FW_PROVISION_LIB_H_
#define _FW_PROVISION_LIB_H_

#include <Uefi.h>

/**
 * This function performs firmware provisioning.
 * 
 * @param ImageHandle - Passed from calling application
 * @param SystemTable - Passed from calling application
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
RunFwProvisionMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

/**
 * This Function gets the state of secure boot
 * 
 * 
 * @param SecureBootEnabled - A caller allocated buffer to store 
 *  						  secure boot state.
 * @return EFI_STATUS 
 */

EFI_STATUS
GetSecureBootState(
  OUT BOOLEAN *SecureBootEnabled
  );

#endif
