/*==============================================================================
@file CoreUtils_dll.c

File to track performance on dll loading.

Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreUtils_dll.c#1 $
==============================================================================*/
#include "DALSys.h"
#include <windows.h>

/*===========================================================================

FUNCTION  DllMain

DESCRIPTION
  Calls perfcnt_init and initializes ARM11 Performance Counters on
  Dll Load (DLL_PROCESS_ATTACH)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
BOOL DllMain
(
  HANDLE  hinstDLL,
  DWORD   Op,
  LPVOID  lpvReserved
)
{
  switch (Op)
  {
    case DLL_PROCESS_ATTACH :
      //DisableThreadLibraryCalls((HMODULE)hinstDLL);
      DALSYS_InitMod(NULL);
      break;
    case DLL_PROCESS_DETACH :
      break;
    default :
      break;
  }

  return TRUE;
}
