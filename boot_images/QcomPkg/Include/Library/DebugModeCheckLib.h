#ifndef __DEBUGMODECHECKLIB_H__
#define __DEBUGMODECHECKLIB_H__

/** @file  
                             DebugModeCheckLib.h

  This is for function declaration of Debug Mode Checks

  Copyright (c) 2013 Qualcomm Technologies, Inc. All rights reserved.

**/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/13   sm      Initial version.
===========================================================================*/

/**
  This function checks if a debug policy is set

  @param[in]  VOID

  @retval BOOLEAN              A Debug Policy is set.
**/

BOOLEAN DebugPolicyIsSet( VOID );

/**
  This function verifies the content stored in PKCS7 stucture meets the following criteria:
  >  Size of data signed in the PKCS#7 is 0x58 bytes
  >  USHORT at offset 0x0 is equal to 0x2
  >  ULONG at offset 0x2 is equal to 0x1
  >  GUID at offset 0x6 is equal to {0x0cdad82e, 0xd839, 0x4754, {0x89, 0xa1, 0x84, 0x4a, 0xb2, 0x82, 0x31, 0x2b}};
  >  ULONG64 at offset 0x50 is equal to the ULONG64 stored in the UEFI variable {77FA9ABD-0359-4D32-BD60-28F4E78F784B}: CopyOfDeviceID


  @param[in]  VOID

  @retval BOOLEAN              Verfies the Debug Policy.
**/
BOOLEAN DebugPolicyVerify( VOID );


#endif /* __DEBUGMODECHECKLIB_H__ */
