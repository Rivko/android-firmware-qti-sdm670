/** @file FwPowerCheck.h
   
  Header for Battery power level checks for Firmware Update.

  Microsoft requirement is that system must have at least 50% battery life to
  as a pre-installation check to permit firmware update.

  Set ALLOW_NO_BATTERY_FIRMWARE_UPDATE to 1 to permit firmware update
  on devices that have no battery connected on Qualcomm reference hardware.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2012/4/30      rsb         Initial version

=============================================================================*/
#ifndef __QCOM_FW_POWERCHECK_H_
#define __QCOM_FW_POWERCHECK_H_

#include "FwEsrt.h"
#include <Library/FwCommonLib.h>

/**
 * Determine if the battery power levels have sufficient power
 * to permit firmware update to proceed based on the pre-condition
 * checks required for UEFI firmware update:
 *    1) System must have at least 50% battery life
 *
 * For Qualcomm reference hardware (if ALLOW_NO_BATTERY_FIRMWARE_UPDATE is defined):
 *    2) or has a DC adaptor plugged in with no battery
 *
 * If the function returns EFI_SUCCESS, then *bPowerGood and *lastAttemptStatus
 * will be modified.  If bPowerGood is FALSE, then lastAttemptStatus
 * should indicate either of FW_LAST_ATMPT_STATUS_POWER_AC_NOT_CONNECTED
 * or FW_LAST_ATMPT_STATUS_POWER_INSUFFICIENT_BATTERY depending on
 * the power condition.
 * 
 * @param bPowerGood        - Return boolean indicating there is sufficient
 *                            power to proceed with firmware update.
 * @param MsftAttemptStatus - The error code to populate to the ESRT
 *                            last attempt status field if bPowerGood is
 *                            FALSE.
 * @param QcResultFlag      - The error code to return to QCFirmwareUpdate 
 *                            result
 * 
 * @return EFI_STATUS 
 */
EFI_STATUS
FwUpdateCheckPowerLevel(
  OUT BOOLEAN *bPowerGood,
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag

  );



#endif
