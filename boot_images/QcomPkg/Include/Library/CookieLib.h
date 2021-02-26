/** @file CookieLib.h
   
  Cookie Definitions

  Copyright (c) 2016 by Qualcomm Technologies, Inc. All Rights Reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/11/16   bh      Initial revision.

=============================================================================*/
#ifndef __COOKIELIB_H__
#define __COOKIELIB_H__

#include <Uefi.h>

#define DLOAD_MODE_BIT_MASK  0x10

/** Get/Set/Validity APIs for DLOAD */
EFI_STATUS
EFIAPI
SetDLOADCookie(VOID);

EFI_STATUS
EFIAPI
ClearDLOADCookie(VOID);

EFI_STATUS
EFIAPI
ClearDLOADCookieRuntime(
  UINTN mDloadCookieAddr
  );

BOOLEAN
EFIAPI
IsDLOADCookieSet(VOID);

/** Get/Set APIs for EDL */
EFI_STATUS
EFIAPI
SetEDLCookie(VOID);

EFI_STATUS
EFIAPI
ClearEDLCookie(VOID);

#endif /* __COOKIELIB_H__ */
