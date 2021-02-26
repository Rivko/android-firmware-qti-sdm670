/*********************************************************************
 * @file CookieLibNull.c
 *
 * @brief Cookie support
 *
 * Copyright (c) 2016 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    ---------------------------------------------------
 08/11/16   bh      Initial revision.

========================================================================*/

#include <Uefi.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
SetDLOADCookie(VOID)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ClearDLOADCookie(VOID)
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ClearDLOADCookieRuntime(
  UINTN mDloadCookieAddr
  )
{
  return EFI_SUCCESS;
}

BOOLEAN
EFIAPI
IsDLOADCookieSet ( VOID )
{
  return FALSE;
}


EFI_STATUS
EFIAPI
SetEDLCookie(VOID)
{
  return EFI_SUCCESS;
  
}

EFI_STATUS
EFIAPI
ClearEDLCookie(VOID)
{
  return EFI_SUCCESS;
}

