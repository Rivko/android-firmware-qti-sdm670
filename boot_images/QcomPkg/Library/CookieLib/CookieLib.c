/*********************************************************************
 * @file CookieLib.c
 *
 * @brief Cookie support
 *
 * Copyright (c) 2016-2018 by Qualcomm Technologies, Inc. All Rights Reserved.
 *
 *********************************************************************/
/*=======================================================================
                        Edit History

 when       who     what, where, why
 --------   ----    ---------------------------------------------------
 09/26/16   vk      Cleanup to make it baselib
 08/11/16   bh      Initial revision.

========================================================================*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>

#include <Library/UefiCfgLib.h>
#include <Library/SerialPortShLib.h>
#include <Library/TzRuntimeLib.h>
#include <Library/CookieLib.h>

EFI_STATUS
EFIAPI
SetDLOADCookie(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mDloadCookieAddr = 0;
  UINT32 mDloadCookieValue = 0;
  UINT64 DloadCookieInfo[2];

  Status = GetConfigValue ("DloadCookieAddr", &mDloadCookieAddr);
  if ((Status != EFI_SUCCESS) || (mDloadCookieAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "DloadCookieAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }

  Status = GetConfigValue ("DloadCookieValue", &mDloadCookieValue);
  if ((Status != EFI_SUCCESS) || (mDloadCookieValue == 0))
  {
    DEBUG ((EFI_D_ERROR, "DloadCookieValue not found in uefiplat.cfg\r\n"));
    return Status;
  }

/* For SW that supports new SCM call, we have the cookie size set to 0 */
  DloadCookieInfo[0] = mDloadCookieAddr;
  DloadCookieInfo[1] = mDloadCookieValue;
  Status = TzFastcall (TZ_IO_ACCESS_WRITE_ID, TZ_IO_ACCESS_WRITE_ID_PARAM_ID, DloadCookieInfo, 2);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
  } 
  return Status;
}

EFI_STATUS
EFIAPI
ClearDLOADCookie(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mDloadCookieAddr = 0;
  UINT64 DloadCookieInfo[2];
    
  Status = GetConfigValue ("DloadCookieAddr", &mDloadCookieAddr);
  if ((Status != EFI_SUCCESS) || (mDloadCookieAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "DloadCookieAddr not found in uefiplat.cfg\r\n"));
    return Status;
  }
   
 /* For SW that supports new SCM call, we have the cookie size set to 0 */
  DloadCookieInfo[0] = mDloadCookieAddr;
  DloadCookieInfo[1] = 0x0;
  Status = TzFastcall (TZ_IO_ACCESS_WRITE_ID, TZ_IO_ACCESS_WRITE_ID_PARAM_ID, DloadCookieInfo, 2);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
  }

  return Status;
}

EFI_STATUS
EFIAPI
ClearDLOADCookieRuntime(
  UINTN mDloadCookieAddr
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT64 DloadCookieInfo[2];  

  if (mDloadCookieAddr == 0)
    return EFI_INVALID_PARAMETER;
 
  /* For SW that supports new SCM call, we have the cookie size set to 0 */
  DloadCookieInfo[0] = mDloadCookieAddr;
  DloadCookieInfo[1] = 0x0;
  Status = TzFastcall (TZ_IO_ACCESS_WRITE_ID, TZ_IO_ACCESS_WRITE_ID_PARAM_ID, DloadCookieInfo, 2);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
  }

  return Status;
}

BOOLEAN
EFIAPI
IsDLOADCookieSet ( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mDloadCookieAddr = 0;
  UINT64 DloadCookieInfo[2] = {0};
  
  
  Status = GetConfigValue ("DloadCookieAddr", &mDloadCookieAddr);
  if ((Status != EFI_SUCCESS) || (mDloadCookieAddr == 0))
  {
    DEBUG ((EFI_D_ERROR, "DloadCookieAddr not found in uefiplat.cfg\r\n"));
    return FALSE;
  }
  
  /* Only check the bits indicated by the cookie */
  /*Bits 20 to 23 are reserved for XBL dload cookies */
  DloadCookieInfo[0] = mDloadCookieAddr;
  Status = TzFastcall (TZ_IO_ACCESS_READ_ID, TZ_IO_ACCESS_READ_ID_PARAM_ID, DloadCookieInfo, 1);
  if (Status != EFI_SUCCESS)
  {
   DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
  }

  if ( DloadCookieInfo[0] & DLOAD_MODE_BIT_MASK)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
   
}


EFI_STATUS
EFIAPI
SetEDLCookie(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mEDLCookieAddr = 0;
  UINT64 EDLCookieValue[2];

  Status = GetConfigValue ("DloadCookieAddr", &mEDLCookieAddr);
  if ((Status != EFI_SUCCESS) || (mEDLCookieAddr == 0))
  {
    DEBUG ((EFI_D_WARN, "EDLCookieAddr not found in uefiplat.cfg. Using PcdEDLCookieAddr.\r\n"));
    mEDLCookieAddr = PcdGet32(PcdEDLCookieAddr);
  }
  
  EDLCookieValue[0] = mEDLCookieAddr;
  EDLCookieValue[1] = 0x1;
  Status = TzFastcall (TZ_IO_ACCESS_WRITE_ID, TZ_IO_ACCESS_WRITE_ID_PARAM_ID, EDLCookieValue, 2);
  if (Status != EFI_SUCCESS)
  {
    DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
  }
  return Status;
}

EFI_STATUS
EFIAPI
ClearEDLCookie(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 mEDLCookieAddr = 0;
  UINT64 PcdEDLCookie[2];
  
  Status = GetConfigValue ("EDLCookieAddr", &mEDLCookieAddr);
  if ((Status != EFI_SUCCESS) || (mEDLCookieAddr == 0))
  {
    DEBUG ((EFI_D_WARN, "EDLCookieAddr not found in uefiplat.cfg. Using PcdEDLCookieAddr.\r\n"));
	mEDLCookieAddr = PcdGet32(PcdEDLCookieAddr);
  }

  if(PcdGet32(PcdEDLCookieSize) == 4) {
    /* Only clear the bit indicated by the cookie */
    PcdEDLCookie[0] = MmioRead32(mEDLCookieAddr) & ~PcdGet32(PcdEDLCookie0);
    PcdEDLCookie[1] = 0;
    Status = TzFastcall (TZ_FORCE_DLOAD_ID, TZ_FORCE_DLOAD_ID_PARAM_ID, PcdEDLCookie, 2);
    if (Status != EFI_SUCCESS)
    {
      DEBUG(( EFI_D_ERROR, "TzFastcall() failed for TZ_IO_ACCESS_WRITE_ID, Status = (0x%x)\r\n", Status));
    }
  }
  else {
    /* Clear the cookies */
    MmioWrite32(mEDLCookieAddr, 0);
    MmioWrite32(mEDLCookieAddr + 4, 0);
    MmioWrite32(mEDLCookieAddr + 8, 0);
  }

  return Status;
}

