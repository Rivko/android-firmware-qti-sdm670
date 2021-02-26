/** @file ShLibMgr.c
   
  Contains Shared Library loader access functions.

  Copyright (c) 2012-2014,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/04/14   vk      Warning fix
 10/22/13   vk      Update return types for 64 bit
 01/17/13   vk      Warning fix, SetMem param swap
 12/10/12   yg      KW warning fix
 09/17/12   yg      Initial Version

=============================================================================*/
#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>

#include "ShLib.h"
#include "ShLibInstall.h"

/* Corresponds to Ver 1.1 */
#define SH_LIB_LOADER_VERSION   0x00010001

/* Extract Major version number */
#define GET_MAJOR_VER(x)        (((UINT32)(x)) >> 16)

EFI_STATUS InstallLibrary (CHAR8* LibName, UINT32 LibVersion, VOID *LibIntf);
EFI_STATUS LoadLibrary (CHAR8* LibName, UINT32 LibVersion, VOID **LibIntfPtr);

ShLibLoaderType ShLibMgr = 
  {
    SH_LIB_LOADER_VERSION,
    InstallLibrary,
    LoadLibrary,
  };

#define  MAX_LIB_COUNT             (32U)
#define  MAX_NAME_BUFFER_SIZE      (MAX_LIB_COUNT * 32)

typedef struct {
   CHAR8*      Name;
   UINT32      Version;
   VOID*       Intf;
}ShLibModuleType;

typedef struct {
   UINT32            LoaderVersion;
   ShLibModuleType   Lib[MAX_LIB_COUNT];
   UINT32            LoadedLibCount;
   CHAR8*            NameBufferPtr;
   CHAR8*            NameBufferEnd;
   CHAR8             NameBuffer[MAX_NAME_BUFFER_SIZE];
}ShLibMgrType;

/* TODO: Use Dynamic memory allocation */
ShLibMgrType ShLib;

extern EFI_GUID gEfiShLibHobGuid;

/***********************************************************************
 *   
 *                   Private functions
 *
 ************************************************************************/
STATIC EFI_STATUS
InstallShLibLoader (ShLibLoaderType* Intf)
{
  UINTN Data = (UINTN)Intf;

  /* Asserts on failure */
  BuildGuidDataHob (&gEfiShLibHobGuid, &Data, sizeof(Data));

  return 0;
}

/***********************************************************************
 *   
 *                   Public functions
 *
 ************************************************************************/
EFI_STATUS
InstallLibrary (CHAR8* LibName, UINT32 LibVersion, VOID *LibIntf)
{
  UINTN Cnt, Len, RealLen;
  CHAR8* LibNameLoc;

  if ((LibIntf == NULL) || (LibName == NULL))
    return LIB_ERR_INVALID_PARAM;

  Cnt = ShLib.LoadedLibCount;
  RealLen = AsciiStrLen(LibName);
  Len = RealLen;
  Len += 3UL;
  Len &= (~3UL);  // Align to 4 bytes boundary

  if (Len < 1)
    return LIB_ERR_INVALID_PARAM;

  if (ShLib.LoaderVersion > SH_LIB_LOADER_VERSION)
    return LIB_ERR_INCOMPATIBLE_LOADER;

  if (Cnt >= MAX_LIB_COUNT) 
    return LIB_ERR_OUT_OF_RESOURCES;

  if(Len >= ((UINTN)ShLib.NameBufferEnd - (UINTN)ShLib.NameBufferPtr))
    return LIB_ERR_OUT_OF_RESOURCES;

  LibNameLoc = ShLib.NameBufferPtr;
  ShLib.NameBufferPtr += Len;
  ++ShLib.LoadedLibCount;

  /* TODO: Verify if the lib is already loaded. */
  AsciiStrnCpy (LibNameLoc, LibName, RealLen);
  LibNameLoc[RealLen] = 0;

  ShLib.Lib[Cnt].Name = LibNameLoc;
  ShLib.Lib[Cnt].Version = LibVersion;
  ShLib.Lib[Cnt].Intf = LibIntf;
        
  return LIB_SUCCESS;
}

EFI_STATUS
LoadLibrary (CHAR8* LibName, UINT32 LibVersion, VOID **LibIntfPtr)
{
  UINTN i, Cnt = ShLib.LoadedLibCount;
  VOID*  BestIntf = NULL;

  if (LibIntfPtr == NULL)
    return LIB_ERR_INVALID_PARAM;

  if (ShLib.LoaderVersion > SH_LIB_LOADER_VERSION)
    return LIB_ERR_INCOMPATIBLE_LOADER;

  for (i = 0; i < Cnt; ++i)
  {
	/* Uninstalling a lib could move all other entries one level up */
    if (ShLib.Lib[i].Name == NULL)
      break;

    if (AsciiStriCmp (ShLib.Lib[i].Name, LibName) == 0)
    {
      if ((BestIntf == NULL) && (ShLib.Lib[i].Version >= LibVersion))
      {
        if (GET_MAJOR_VER(ShLib.Lib[i].Version) == GET_MAJOR_VER(LibVersion))
          BestIntf = ShLib.Lib[i].Intf;
      }

      if ((LibVersion == 0) || (ShLib.Lib[i].Version == LibVersion))
      {
        *LibIntfPtr = ShLib.Lib[i].Intf;
        return LIB_SUCCESS;
      }
    }
  }

  if (BestIntf != NULL)
  {
  	*LibIntfPtr = BestIntf;
    return LIB_CLOSE_MATCH_FOUND;
  }

  return LIB_ERR_NOT_FOUND;
}

EFI_STATUS
ShLibMgrInit (VOID)
{
  SetMem (&ShLib, sizeof(ShLib), 0);
  ShLib.LoaderVersion = SH_LIB_LOADER_VERSION;
  ShLib.NameBufferPtr = ShLib.NameBuffer;
  ShLib.NameBufferEnd = ShLib.NameBuffer + sizeof(ShLib.NameBuffer);
  return InstallShLibLoader (&ShLibMgr);
}

