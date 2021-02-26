/** @file UefiConfigLibImpl.c
   
  Contains UEFI Config Shared Library loader and functions.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/18/14   jb      Implement 64 bit support
 05/22/14   vk      Revert type change
 05/19/14   vk      GetConfigValue64 support
 06/11/13   niting  Added func to get config value from an input table
 03/07/13   vk      Fix copy size
 01/15/13   vk      Initial Version

=============================================================================*/

#include "ShLib.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiCfgLib.h>

/* Version 1.2 */
#define CURRENT_UEFI_CFG_LIB_VERSION  0x00010002

STATIC MemRegionInfo* MemRegionTable;
STATIC UINTN MemRegionCount;
STATIC ConfigPair *ConfigTable;
STATIC UINTN ConfigCount;

STATIC BOOLEAN
IsDigit (CHAR8* Ch)
{
  if (Ch == NULL)  /* For KW */
   return FALSE;

  return (BOOLEAN) ((*Ch >= '0') && (*Ch <= '9'));
}

STATIC BOOLEAN
IsHexDigit (CHAR8* Ch)
{
  if (Ch == NULL)  /* For KW */
   return FALSE;

  return (BOOLEAN) ((IsDigit(Ch)) ||
                    (*Ch >= 'A' && *Ch <= 'F') ||
                    (*Ch >= 'a' && *Ch <= 'f'));
}

STATIC BOOLEAN
IsDigitString (CHAR8* Ch)
{
  if (Ch == NULL)
   return FALSE;

  while (*Ch != '\0')
  {
    if (!IsDigit(Ch))
      return FALSE;
    Ch++;  
  }

  return TRUE;
}

STATIC BOOLEAN
IsHexDigitString (CHAR8* Ch)
{
  if (Ch == NULL)
   return FALSE;

  while (*Ch != '\0')
  {
    if (!IsHexDigit(Ch))
      return FALSE;
    Ch++;  
  }

  return TRUE;
}


/* Gets Memory Region Info by Region Name string from config file */
EFI_STATUS
EFIAPI
GetMemoryRegionInfoByName ( CHAR8* RegionName, MemRegionInfo* MemoryRegion )
{
  UINTN i = 0;
  
  /* Check if sec initialized properly */
  if ((MemRegionTable == NULL) || (MemRegionCount == 0))
    return EFI_NOT_READY;

  if ((MemoryRegion == NULL) || (RegionName == NULL))
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < MemRegionCount; i++)
  {
    if (AsciiStriCmp (MemRegionTable[i].Name, RegionName) == 0)
    {
      CopyMem (MemoryRegion, &MemRegionTable[i], sizeof(MemRegionInfo));
      return EFI_SUCCESS;
    }
  }
  
  return EFI_NOT_FOUND;
}

EFI_STATUS
GetConfigStr (CHAR8* Key, CHAR8* Value, UINTN* ValBuffSize)
{
  UINTN i = 0;

  if ((ConfigTable == NULL) || (ConfigCount == 0))
    return EFI_NOT_READY;
  
  if ((Value == NULL) || (ValBuffSize == NULL))
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < ConfigCount; i++)
  {
    if (AsciiStriCmp (ConfigTable[i].Key, Key) == 0)
    {
      /* Should not be NULL */
      if (ConfigTable[i].Value == NULL)
        return EFI_UNSUPPORTED;

      if ((AsciiStrLen (ConfigTable[i].Value) + 1) <= *ValBuffSize)
      {
        AsciiStrCpy (Value, ConfigTable[i].Value);
        *ValBuffSize = AsciiStrLen (ConfigTable[i].Value) + 1;
        return EFI_SUCCESS;
      }
      else
      {
        *ValBuffSize = AsciiStrLen (ConfigTable[i].Value) + 1;
        return EFI_BUFFER_TOO_SMALL;
      }
    }
  }

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
GetConfigVal64FromTable (CHAR8* Key, UINT64* Value,
                         ConfigPair *mConfigTable, UINTN mConfigCount)
{
  UINTN i = 0;

  if ((mConfigTable == NULL) || (mConfigCount == 0))
    return EFI_NOT_READY;

  if (Value == NULL)
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < mConfigCount; i++)
  {
    if (AsciiStriCmp (mConfigTable[i].Key, Key) == 0)
    {
      /* Should not be NULL */
      if (mConfigTable[i].Value == NULL)
        return EFI_UNSUPPORTED;

      /* Check if hex */
      if ((AsciiStrLen (mConfigTable[i].Value) <= 18) &&
          ((AsciiStrnCmp (mConfigTable[i].Value, "0x", 2) == 0) ||
          (AsciiStrnCmp (mConfigTable[i].Value, "0X", 2) == 0)))
      {
        if (IsHexDigitString (mConfigTable[i].Value + 2))
        {
          *Value = AsciiStrHexToUintn (mConfigTable[i].Value);
          return EFI_SUCCESS;
        }
        else
          return EFI_UNSUPPORTED;
      }
      else
      {
        /* if not hex */
        if (IsDigitString (mConfigTable[i].Value))
        {
          *Value = AsciiStrDecimalToUintn (mConfigTable[i].Value);
          return EFI_SUCCESS;
        }
        else
          return EFI_UNSUPPORTED;
      }
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetConfigValFromTable (CHAR8* Key, UINT32* Value,
                       ConfigPair *mConfigTable, UINTN mConfigCount)
{
  UINT64 Value64;
  EFI_STATUS Status = GetConfigVal64FromTable (Key, &Value64, mConfigTable, mConfigCount);
  if (Value64 >= 0x100000000)
  {
    return EFI_INVALID_PARAMETER;
  }
  else
  {
    *Value = (UINT32)Value64;
    return Status;
  }
}

EFI_STATUS
GetConfigVal (CHAR8* Key, UINT32* Value)
{
  return GetConfigValFromTable (Key, Value, ConfigTable, ConfigCount);
}

EFI_STATUS
GetConfigVal64 (CHAR8* Key, UINT64* Value)
{
  return GetConfigVal64FromTable (Key, Value, ConfigTable, ConfigCount);
}

UefiCfgLibType UefiCfgLib =
{
  .LibVersion       = CURRENT_UEFI_CFG_LIB_VERSION,
  .GetMemInfoByName = GetMemoryRegionInfoByName,
  .GetCfgInfoString = GetConfigStr,
  .GetCfgInfoVal    = GetConfigVal,
  .GetCfgInfoVal64  = GetConfigVal64
};

EFI_STATUS
UefiCfgShLibInit (MemRegionInfo *MemRegions, UINTN MemRegCount, 
                  ConfigPair* CfgTblPtr, UINTN CfgCount)
{
  ShLibLoaderType *ShLibLoader;
  INT32  LoadStatus;

  ShLibLoader = GetShLibLoader();
  if (ShLibLoader == NULL)
    return EFI_NOT_FOUND;

  if ((MemRegions == NULL) || (MemRegCount == 0 ) ||
      (CfgTblPtr == NULL) || (CfgCount == 0))
    return EFI_INVALID_PARAMETER;

  MemRegionTable = MemRegions;
  MemRegionCount = MemRegCount;
  
  ConfigTable = CfgTblPtr;
  ConfigCount = CfgCount;

  LoadStatus = ShLibLoader->InstallLib (UEFI_CFG_LIB_NAME, UefiCfgLib.LibVersion, (VOID*)&UefiCfgLib);

  if (LoadStatus == LIB_SUCCESS)
    return EFI_SUCCESS;
  else
    return EFI_NOT_FOUND;
}
