/** @file FwUpdateLib.h
   
  Header for externally-available functions of the FwUpdateLib 

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/08/03     mic         Added maximum update attempts support
 2012/05/17     Mic         Moved from OSAppPkg to QcomPkg
 2011/10/12     jthompso    Initial version

=============================================================================*/
#ifndef _FW_UPDATE_LIB_H_
#define _FW_UPDATE_LIB_H_

#include <Uefi.h>
#include <Library/FwUpdateLib/FwCapsule.h>
#include <Library/FwUpdateLib/FwEsrt.h>

#define UPDATE_ATTEMPT_COUNT_MAX 3

typedef enum
{
  QCOM_FW_UPDATE_MODE_QCFW,
  QCOM_FW_UPDATE_MODE_CAPSULE,
  QCOM_FW_UPDATE_MODE_NOUPDATE,
  QCOM_FW_UPDATE_MODE_MAX = QCOM_FW_UPDATE_MODE_NOUPDATE
}QCOM_FW_UPDATE_MODE;

/**
  The user Entry Point for FirmwareUpdate. 

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
RunFwUpdateMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

/**
  TODO

**/

EFI_STATUS
EFIAPI
FwUpdatePhaseMain (
  IN EFI_SYSTEM_TABLE  *SystemTable,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  );
/**
  TODO

**/

EFI_STATUS
FwUpdateDone(
  IN EFI_SYSTEM_TABLE         *SystemTable,
  IN ESRT_LAST_ATTEMPT_STATUS AttemptStatus,
  IN QCFW_UPDATE_FLAG_VALUE   QcResultFlag
  );

/**
  TODO

**/

EFI_STATUS
FwUpdateClearFlagOrCapsule(
  IN QCFW_UPDATE_FLAG_VALUE QcResultFlag);



#endif
