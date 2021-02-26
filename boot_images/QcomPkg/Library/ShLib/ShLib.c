/** @file ShLib.c
   
  Contains Shared Library loader access functions.

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/17/13   vk      Fix warning
 09/17/12   yg      Initial Version

=============================================================================*/
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <PiDxe.h>
#include "ShLib.h"
#include <Library/HobLib.h>

extern EFI_GUID gEfiShLibHobGuid;

ShLibLoaderType* GetShLibLoader (VOID)
{
  EFI_HOB_GUID_TYPE *GuidHob;
  UINT32** DataPtr;

  GuidHob = GetFirstGuidHob(&gEfiShLibHobGuid);

  if (GuidHob == NULL)
    return NULL;

  DataPtr = GET_GUID_HOB_DATA(GuidHob);
  return (ShLibLoaderType*) *DataPtr;
}


