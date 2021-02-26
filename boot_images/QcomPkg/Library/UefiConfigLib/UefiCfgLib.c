/** @file UefiCfgLib.c
   
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/19/14   vk      GetConfigValue64 support
 01/16/13   vk      Initial Version

=============================================================================*/

#include "ShLib.h"
#include <Library/UefiCfgLib.h>

/* Lib interface pointer instance local to the app,
 * for using in the pass through layer */
STATIC UefiCfgLibType *UefiCfgLibPtr = NULL;

EFI_STATUS UefiCfgInitialize (VOID);

/* Gets Memory Region Info by Region Name string from config file */
EFI_STATUS EFIAPI
GetMemRegionInfoByName (CHAR8* RegionName, MemRegionInfo* MemoryRegion)
{
  if (UefiCfgLibPtr == NULL)
  {
    UefiCfgInitialize();
    if (UefiCfgLibPtr == NULL)
      return EFI_NOT_READY;
  }
  return UefiCfgLibPtr->GetMemInfoByName (RegionName, MemoryRegion);
}

/* Gets Config String */
EFI_STATUS
GetConfigString (CHAR8* Key, CHAR8* Value, UINTN* ValBuffSize)
{
  if (UefiCfgLibPtr == NULL)
  {
    UefiCfgInitialize();
    if (UefiCfgLibPtr == NULL)
      return EFI_NOT_READY;
  }
  return UefiCfgLibPtr->GetCfgInfoString (Key, Value, ValBuffSize);
}

/* Gets Config Value as UINT32 */
EFI_STATUS
GetConfigValue (CHAR8* Key, UINT32* Value)
{
  if (UefiCfgLibPtr == NULL)
  {
    UefiCfgInitialize();
    if (UefiCfgLibPtr == NULL)
      return EFI_NOT_READY;
  }
  return UefiCfgLibPtr->GetCfgInfoVal (Key, Value);
}


/* Gets Config Value as UINT64 */
EFI_STATUS
GetConfigValue64 (CHAR8* Key, UINT64* Value)
{
  if (UefiCfgLibPtr == NULL)
  {
    UefiCfgInitialize();
    if (UefiCfgLibPtr == NULL)
      return EFI_NOT_READY;
  }
  return UefiCfgLibPtr->GetCfgInfoVal64 (Key, Value);
}

EFI_STATUS UefiCfgInitialize (VOID)
{
  ShLibLoaderType *ShLibLoader;

  ShLibLoader = GetShLibLoader();
  if (ShLibLoader == NULL)
    return EFI_NOT_READY;

  return ShLibLoader->LoadLib (UEFI_CFG_LIB_NAME, 0, (VOID*)&UefiCfgLibPtr);
}


