/** @file TzRuntimeLib.h
   
  Collections of TZ register calls.

  Copyright (c) 2013,2014,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/19/17   aus     32bit mode switch protocol
 08/15/14   sm      Switched to new SCM API
 09/26/13   shl     Added fast call and removed SPMI call. 
 08/26/13   shl     Initial version
=============================================================================*/
#ifndef __TZRUNTIMELIB_H__
#define __TZRUNTIMELIB_H__

#include <Include/scm_sip_interface.h>

/**
  This function makes TZBSP fast syscall call( meaning using registers
  to pass parameters)

  @param  CmdId                 TZBSP command ID
  @param  *paramArray           A pointer to a physical address where 
                                paramArray starts ( UINT32 type )
  @param  paramSize             parameter array size

  @retval EFI_SUCCESS           Call is successfully.
  @retval EFI_INVALID_PARAMETER NULL parameter is passed
  @retval EFI_DEVICE_ERROR      TZ call is failed.

**/ 
EFI_STATUS TzFastcall
(
  IN UINT32                  SmcId,
  IN UINT32                  ParamId,
  IN UINT64                 *ParamArray,
  IN UINTN                   NumParams
);

/**
  This function installs a protocol to perform mode switch

  @retval EFI_SUCCESS           Call is successfull.
  @retval Error value           Call is unsuccessfull.  
**/ 
EFI_STATUS TzRegisterModeSwitchProtocol (VOID);

#endif /* __TZRUNTIMELIB_H__ */
