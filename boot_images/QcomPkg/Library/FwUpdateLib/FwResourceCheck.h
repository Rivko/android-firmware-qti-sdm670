/** @file FwResourceCheck.h
   
 Resource availability check functions for data-driven firmware update

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when             who         what, where, why
 ----------   ---         --------------------------------------------------
 2012/10/08    mic        Initial version

=============================================================================*/

#ifndef __QCOM_FW_RESOURCE_CHECK_H_
#define __QCOM_FW_RESOURCE_CHECK_H_
#include <Library/FwCommonLib.h>


/**
 * This function should check resource availability
 * for firmware update
 *   
 *
 * @return : EFI_STATUS
 */
EFI_STATUS
FwUpdateResourceCheckMain(
  OUT ESRT_LAST_ATTEMPT_STATUS *MsftAttemptStatus,
  OUT QCFW_UPDATE_FLAG_VALUE   *QcResultFlag
  );

#endif


