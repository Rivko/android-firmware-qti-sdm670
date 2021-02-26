/** @file TargetInitLib.h
   
  Contains TargetInit specific library functions.

  Copyright (c) 2013,2017 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/27/17   vk      Remove unused API
 01/22/13   vk      Initial Version
=============================================================================*/
#ifndef __TARGET_INIT_LIB_H__
#define __TARGET_INIT_LIB_H__

#include <Uefi.h>

VOID
TargetEarlyInit (VOID);

VOID
TargetLateInit (VOID);

#endif /* __TARGET_INIT_LIB_H_ */
