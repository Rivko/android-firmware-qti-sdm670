/** @file DALModEnv.c

  DAL module environment

  Copyright (c) 2010-2014, Qualcomm Technologies Incorporated. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/24/14   sjohn   Merged standard C library functions needed in DAL
 08/08/14   sjohn   Added Safe memcpy - memcpy_s
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 03/13/14   rli     Define memcpy only for armcc compiler
 06/17/11   sho     Add memory barrier support
 08/31/10   sho     Creation 
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 03/13/14   rli     Define memcpy only for armcc compiler
 06/17/11   sho     Add memory barrier support
 08/31/10   sho     Creation 
 09/19/14   sho     Update DALSYS_InitMod to use new API to register devices 
 08/15/14   aa      Adding new parameter to DALSYS_DeInitMod for string devices
 08/13/14   aa      Adding Function to compute hash
 08/13/14   aa      Added Support for string based devices
 08/13/14   aa      Added Initialization data structures for string based device
                    driver list.
 08/12/14   sho     Remove a redundant parameter in DeviceAttach
 05/01/14   asmitp  Renamed header file DALSYSProtocol.h to EFIDALSYSProtocol.h
 03/13/14   rli     Define memcpy only for armcc compiler
 06/17/11   sho     Add memory barrier support
 08/31/10   sho     Creation 

=============================================================================*/

#include "DALSys.h"
#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALPropDef.h"
#include "DALReg.h"
#include "DALFramework.h"
#include <Uefi.h>
#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIDALSYSProtocol.h>
#include <Library/QcomBaseLib.h>


#define MAX_LOG_LENGTH  0x80
#define DALMOD_CHECK_UNINIT(FncTblPtr, RetVal) \
   if (FncTblPtr == NULL) return RetVal
#define DALMOD_CHECK_UNINIT_NO_RETVAL(FncTblPtr) \
   if (FncTblPtr == NULL) return

/*=============================================================================
 Variable Declarations
=============================================================================*/
// List of DALs in the UEFI driver statically linked to this library
static DALREG_DriverInfoList * gpDALModDriverInfoList;

// List of DALs(String Devices) in the UEFI driver statically linked to this library
static DALProps gDALModStringDeviceInfoList;
static DALProps *gpDALModStringDeviceInfoList = &gDALModStringDeviceInfoList;

/*=============================================================================
Forward Declarations
=============================================================================*/
DALResult System_DalSystem_Attach(const char *pszArg, DALDEVICEID DeviceId,
                                  DalDeviceHandle **phDalDevice);
                                  
/*=============================================================================
 Variable Definitions
=============================================================================*/
// DALSYS protocol for init and deinit
static EFI_DALSYS_PROTOCOL *gpDALSYSInitFncPtr = NULL;
// pointer to other DALSYS functions
static DALSYSFncPtrTbl *gpDALSYSFncPtr;                 
// Dummy place holder for system device
static DALDEVICEID DalSystem_DeviceId[1] = {DALDEVICEID_SYSTEM};

const DALREG_DriverInfo
DALSystem_DriverInfo = { System_DalSystem_Attach,
                         0,
                         DalSystem_DeviceId
                       };

/*=============================================================================
 Empty Function Definitions
=============================================================================*/
static void getDALSYSProtocol(void)
{
   EFI_STATUS status;
   status = gBS->LocateProtocol(&gEfiDalSysProtocolGuid, NULL, 
      (VOID **)&gpDALSYSInitFncPtr);
   ASSERT_EFI_ERROR(status);
   ASSERT(gpDALSYSInitFncPtr);
}

DALResult
System_DalSystem_Attach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;
   return DAL_ERROR;
}

//DJB Hash function
uint32
DALYS_ComputeHash(const char *pszDevName)
{
   uint32 dwHash = 5381; //magic constant for fewer collisions
   while (*pszDevName)
      dwHash = ((dwHash << 5) + dwHash) + (int)*pszDevName++; /* dwHash * 33 + c */
   return dwHash;
}

DALResult
DALSYS_DestroyObject(DALSYSObjHandle hObj)
{
   return DAL_SUCCESS;
}

DALResult
DALSYS_CopyObject(DALSYSObjHandle hObjOrig, DALSYSObjHandle *phObjCopy )
{
   return DAL_ERROR;
}

DALResult
DALSYS_RegisterWorkLoop(uint32 dwPriority,
                        uint32 dwMaxNumEvents,
                        DALSYSWorkLoopHandle *phWorkLoop,
                        DALSYSWorkLoopObj *pWorkLoopObj)
{
   return DAL_ERROR;
}

DALResult
DALSYS_AddEventToWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                          DALSYSWorkLoopExecute pfnWorkLoopExecute,
                          void * pArg,
                          DALSYSEventHandle hEvent,
                          DALSYSSyncHandle hSync)
{
   return DAL_ERROR;
}

DALResult
DALSYS_DeleteEventFromWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                               DALSYSEventHandle hEvent)
{
   return DAL_ERROR;
}

DALResult
DALSYS_EventWait(DALSYSEventHandle hEvent)
{
   return DAL_ERROR;
}

DALResult
DALSYS_EventMultipleWait(DALSYSEventHandle* phEvent, int nEvents,
                         uint32 dwTimeoutUs, uint32 *pdwEventIdx)
{
   return DAL_ERROR;
}

void
DALSYS_SyncEnter(DALSYSSyncHandle hSync)
{
}

void
DALSYS_SyncLeave(DALSYSSyncHandle hSync)
{
}

DALResult
DALSYS_MemDescAddBuf(DALSysMemDescList * pMemDesc, uint32 dwMemDescBufIdx,
                          uint32 dwOffset, uint32 dwSize)
{
   return DAL_ERROR;
}

DALResult
DALSYS_MemDescPrepare(DALSysMemDescList * pMemDesc, uint32 dwBufIdx)
{
   return DAL_ERROR;
}

DALResult
DALSYS_MemDescValidate(DALSysMemDescList * pMemDesc,uint32 dwBufIdx)
{
   return DAL_ERROR;
}

void
DALSYS_InitSystemHandle(DalDeviceHandle *hDalDevice)
{
}

DALResult
DAL_DeviceAttachRemote(const char *pszArg,
                   DALDEVICEID DevId,
                   DALInterfaceVersion ClientVersion,
                   DalDeviceHandle **phDALDevice)
{
   return DAL_ERROR;
}

DALResult
DALSYS_MemRegionMapPhys(DALSYSMemHandle hMem, uint32 dwVirtualBaseOffset,
                        DALSYSMemAddr PhysicalAddr, uint32 dwLen)
{
   return DAL_SUCCESS;
}

/*=============================================================================
 Function Definitions
=============================================================================*/
void
DALSYS_InitMod(DALSYSConfig * pCfg)
{
   if (pCfg)
   {
      static int configDefined = 0;
      if (configDefined)
      {
         DEBUG ((EFI_D_ERROR, 
           "Module cannot re-initialize DAL module environment\n"));
         ASSERT_EFI_ERROR (0);
      }
      configDefined = 1;

      // populate the DALProps structure which contains 
      // the string device info list
      gpDALModStringDeviceInfoList->dwDeviceSize = 
         pCfg->stringDeviceInfoList.dwLen;
      gpDALModStringDeviceInfoList->pDevices = 
         pCfg->stringDeviceInfoList.pDevices;
      gpDALModDriverInfoList = &pCfg->numericDriverInfoList;

      getDALSYSProtocol();
      gpDALSYSInitFncPtr->init(gpDALModDriverInfoList, 
         gpDALModStringDeviceInfoList, &gpDALSYSFncPtr);
   }
   else
   {
      if (!gpDALSYSFncPtr)
      {
         getDALSYSProtocol();
         gpDALSYSInitFncPtr->init(NULL, NULL, &gpDALSYSFncPtr);
      }
   }

}

void
DALSYS_DeInitMod(void)
{
   gpDALSYSInitFncPtr->deinit(gpDALModDriverInfoList, 
         gpDALModStringDeviceInfoList);
}

DALResult
DALSYS_EventCreate(uint32 dwEvtAttrib, DALSYSEventHandle *phEvent,
                   DALSYSEventObj *pEventObj)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_EventCreateFnc( dwEvtAttrib, phEvent, pEventObj );
}

DALResult
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam,
                   void *pPayload, uint32 dwPayloadSize)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_EventCtrlFnc( hEvent, dwCtrl, dwParam, pPayload, 
         dwPayloadSize );
}

DALResult
DALSYS_SetupCallbackEvent(DALSYSEventHandle hEvent, DALSYSCallbackFunc cbFunc,
                          DALSYSCallbackFuncCtx cbFuncCtx)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_SetupCallbackEventFnc( hEvent, cbFunc, cbFuncCtx );
}

DALResult
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_SyncCreateFnc( dwAttribs, phSync, pSyncObj );
}

DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr VirtualAddr,
                      DALSYSMemAddr PhysicalAddr, uint32 dwLen, DALSYSMemHandle *phMem,
                      DALSYSMemObj *pObj)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_MemRegionAllocFnc( dwAttribs, VirtualAddr,
         PhysicalAddr, dwLen, phMem, pObj );
}

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_MemInfoFnc( hMem, pMemInfo );
}

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_CacheCommandFnc( CacheCmd, VirtualAddr, dwLen );
}

DALResult
DALSYS_Malloc(uint32 dwSize, void **ppMem)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_MallocFnc( dwSize, ppMem );
}

DALResult
DALSYS_Free(void *pmem)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_FreeFnc( pmem );
}

void
DALSYS_BusyWait(uint32 pause_time_us)
{
   DALMOD_CHECK_UNINIT_NO_RETVAL(gpDALSYSFncPtr);
   gpDALSYSFncPtr->DALSYS_BusyWaitFnc( pause_time_us );
}

UINTN 
DALSYS_memscpy(VOID * pDest, UINTN iDestSz, VOID * pSrc, UINTN iSrcSize)
{
   return CopyMemS(pDest, iDestSz, pSrc, iSrcSize);
}

VOID * 
DALSYS_memset(VOID * buffer, UINT8 value, UINTN length)
{
   // the length and value parameters are ordered differently in SetMem API
   return SetMem( buffer, length, value );
}


DALResult
DALSYS_GetDALPropertyHandle(DALDEVICEID DeviceId,DALSYSPropertyHandle hDALProps)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_GetDALPropertyHandleFnc( DeviceId, hDALProps );
}

DALResult
DALSYS_GetDALPropertyHandleStr(const char *pszDevName, DALSYSPropertyHandle hDALProps)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_GetDALPropertyHandleStrFnc( pszDevName, hDALProps );
}

DALResult
DALSYS_GetPropertyValue(DALSYSPropertyHandle hDALProps, const char *pszName,
                        uint32 dwId,
                        DALSYSPropertyVar *pDALPropVar)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_GetPropertyValueFnc( hDALProps, pszName,
         dwId, pDALPropVar );
}

void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, const char * pszFmt, ...)
{
   CHAR8 buffer[MAX_LOG_LENGTH];
   VA_LIST argument;

   DALMOD_CHECK_UNINIT_NO_RETVAL(gpDALSYSFncPtr);

   ASSERT(pszFmt);

   VA_START(argument, pszFmt);
   AsciiVSPrint(buffer, sizeof(buffer), pszFmt, argument);
   VA_END(argument);

   gpDALSYSFncPtr->DALSYS_LogEventFnc( DeviceId, dwLogEventType, buffer);
}

DALResult
DAL_DeviceAttachEx(const char *pszArg,
      DALDEVICEID DevId,
      DALInterfaceVersion ClientVersion,
      DalDeviceHandle **phDALDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_DeviceAttachExFnc(pszArg,DevId,ClientVersion,phDALDevice);
}

DALResult
DAL_DeviceAttach( DALDEVICEID DevId,
                  DalDeviceHandle **phDalDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_DeviceAttachFnc(DevId,phDalDevice);
}

DALResult
DAL_StringDeviceAttach( const char *pszDevName,
                       DalDeviceHandle **phDalDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_StringDeviceAttachFnc(pszDevName,phDalDevice);
}

DALResult
DAL_StringDeviceAttachEx(const char *pszArg,
      const char *pszDevName,
      DALInterfaceVersion ClientVersion,
      DalDeviceHandle **phDALDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_StringDeviceAttachExFnc(pszArg,pszDevName,ClientVersion,phDALDevice);
}

DALResult
DAL_DeviceDetach(DalDeviceHandle * hDalDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_DeviceDetachFnc(hDalDevice);
}

DALResult
DAL_DeviceAttachLocal(const char *pszArg,DALDEVICEID DevId,
                      DalDeviceHandle **phDalDevice)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALSYS_DeviceAttachFnc(DevId,phDalDevice);

}

// relay memory barrier function call to ARM library
void memory_barrier(void)
{
   ArmDataMemoryBarrier();
}

// Global Context wrapper functions

DALResult
DALGLBCTXT_Init()
{
   // this does nothing as initialization is done in
   // DALSYS DXE driver entry point
   return DAL_SUCCESS;
}

DALResult
DALGLBCTXT_AllocCtxt(const char * name, UINT32 size_req, UINT32 lock_type, VOID ** ctxt)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALGLBCTXT_AllocCtxtFnc(
         name, size_req, lock_type, ctxt);
}

DALResult
DALGLBCTXT_FindCtxt(const char * name, VOID ** ctxt)
{
   DALMOD_CHECK_UNINIT(gpDALSYSFncPtr, DAL_ERROR);
   return gpDALSYSFncPtr->DALGLBCTXT_FindCtxtFnc(
         name, ctxt);
}

/*=============================================================================
 Standard C library functions needed in DAL
 Equivalent functions are found in UEFI
=============================================================================*/

/**
 * <!-- memcpy_s -->
 *
 * @brief Safe Memcpy - 
 *  
 * Safe memcpy - Checks the size of destination buffer as well.
 * Calling it core_memcpy() to keep inline with mainline.
 *
 * @return : void
 */
size_t memcpy_s(void * pDest, size_t DestLength, const void * pSrc, size_t Count)
{
  return CopyMemS (pDest, DestLength, pSrc, Count);
}
