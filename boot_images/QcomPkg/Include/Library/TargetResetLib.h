/** @file TargetResetLib.h
   
  Contains TargetReset specific library functions.

  Copyright (c) 2013 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/17/13   vk      Initial Version
=============================================================================*/
#ifndef __TARGET_RESET_LIB_H__
#define __TARGET_RESET_LIB_H__

#include <Uefi.h>

VOID
TargetReset (VOID);

VOID
TargetPowerOff (VOID);

#endif /* __TARGET_RESET_LIB_H_ */
