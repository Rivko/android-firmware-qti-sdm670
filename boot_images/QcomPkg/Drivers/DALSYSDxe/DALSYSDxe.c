/*=============================================================================
  @file DALSYSDxe.c

  DALSYS DXE driver

  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

=-=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/19/14   sho     Ensure we are not adding empty device list to module list
 08/15/14   aa      Added string driver info list param to _DALSYS_DeInit
 08/14/14   aa      Added string driver info list param to _DALSYS_Init
 08/13/14   aa      Removed RegisterDriverlist API and moved to DALCommonLib
 08/07/14   sho     Move implementation to DALSYSDxeLib

=============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALFramework.h"
#include "DALGlbCtxt.h"
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Protocol/EFIDALSYSProtocol.h>

/*=============================================================================
 Variable Declarations
=============================================================================*/
// DALSYS Public Protocol 
EFI_DALSYS_PROTOCOL DalSysProtocol =
{
   DALSYS_REVISION,
   _DALSYS_Init,
   _DALSYS_DeInit
};

/*-------------------------------------------------------------------
 Function Definitions 
 ------------------------------------------------------------------*/
// former entry point of DAL Global Context when it was a standalone
// DXE driver; this is now merged with DALSYS DXE driver
extern EFI_STATUS DALGlbCtxtEntryPoint(void);

VOID
_DALSYS_Init(DALREG_DriverInfoList *pModDriverList,
             DALProps *pDALModStringDevDrvInfoList,
             DALSYSFncPtrTbl **pFncTblPtr)
{
   // this only implements a subset of the DALSYS functionalities
   static DALSYSFncPtrTbl DALSYSFncPtrTbl = {
      NULL, //_DALSYS_InitSystemHandle
      NULL, //_DALSYS_DestroyObject
      NULL, //_DALSYS_CopyObject
      NULL, //_DALSYS_RegisterWorkLoop
      NULL, //_DALSYS_AddEventToWorkLoop,
      NULL, //_DALSYS_DeleteEventFromWorkLoop,
      DALSYS_EventCreate,
      DALSYS_EventCtrlEx,
      NULL, //_DALSYS_EventWait,
      NULL, //_DALSYS_EventMultipleWait,
      DALSYS_SetupCallbackEvent,
      DALSYS_SyncCreate,
      NULL, //_DALSYS_SyncEnter,
      NULL, //_DALSYS_SyncLeave,
      DALSYS_MemRegionAlloc,
      NULL, //_DALSYS_MemRegionMapPhys,
      DALSYS_MemInfo,
      DALSYS_CacheCommand,
      DALSYS_Malloc,
      DALSYS_Free,
      DALSYS_BusyWait,
      NULL,/*_DALSYS_MemDescAddBuf*/
      NULL,/*_DALSYS_MemDescPrepare*/
      NULL,/*_DALSYS_MemDescValidate*/
      DALSYS_GetDALPropertyHandle,
      DALSYS_GetDALPropertyHandleStr,
      DAL_DeviceAttach,
      DAL_DeviceAttachEx,
      DAL_StringDeviceAttach,
      DAL_StringDeviceAttachEx,
      NULL, //_DAL_DeviceAttachRemote,
      DAL_DeviceDetach,
      DALSYS_GetPropertyValue,
      DALSYS_LogEvent,
      DALGLBCTXT_AllocCtxt,
      DALGLBCTXT_FindCtxt
      };
   *pFncTblPtr = &DALSYSFncPtrTbl;

   // Add the numeric device driver list only if device count is non-zero
   if(pModDriverList && pModDriverList->dwLen)
      DALSYSCMN_RegisterModDriverList(pModDriverList);
    
   // Add the string device driver list only if device count is non-zero
   if(pDALModStringDevDrvInfoList && pDALModStringDevDrvInfoList->dwDeviceSize)
      DALSYSCMN_RegisterStrModDriverList(pDALModStringDevDrvInfoList);
}

VOID
_DALSYS_DeInit(DALREG_DriverInfoList *pModDriverList, 
               DALProps *pDALModStringDevDrvInfoList)
{
    DALSYSCMN_DeRegisterModDriverList(pModDriverList);
    DALSYSCMN_DeRegisterStrModDriverList(pDALModStringDevDrvInfoList);
}

EFI_STATUS
EFIAPI
DALSYSEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
   EFI_HANDLE  handle = NULL;
   EFI_STATUS  status;

   // Publish the DALSYS Protocol
   status = gBS->InstallMultipleProtocolInterfaces( &handle, 
         &gEfiDalSysProtocolGuid, 
         (void **)&DalSysProtocol, NULL );
   if (EFI_SUCCESS != status)
   {
      return status;
   }

   if (DAL_SUCCESS != DALGLBCTXT_Init())
   {
      return EFI_D_ERROR;
   }
   return EFI_SUCCESS;
}
