/** @file DALSYSLoader.c

  DALSYS library for XBL loader

  Copyright (c) 2010-2011,2014 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/02/17   yps     Added DALSYS_DeInitMod
 09/19/14   sho     Ensure the action in DALSYS_InitMod is done only on first invocation
 08/15/14   aa      Adding String Device Support
 08/11/14   sho     Creation 

=============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include "DALDeviceId.h"
#include "DALGlbCtxt.h"
#include "smem.h"
#include <Uefi.h>
#include <Library/DebugLib.h>


extern DALProps gDALModStringDeviceInfoList;
extern DALREG_DriverInfoList gDALModDriverInfoList;

/*=============================================================================
Forward Declarations
=============================================================================*/
DALResult System_DalSystem_Attach(const char *pszArg, DALDEVICEID DeviceId,
                                  DalDeviceHandle **phDalDevice);
                                  
/*=============================================================================
 Variable Definitions
=============================================================================*/
static DALDEVICEID DalSystem_DeviceId[1] = {DALDEVICEID_SYSTEM};
const DALREG_DriverInfo
DALSystem_DriverInfo = { System_DalSystem_Attach,
                         0,
                         DalSystem_DeviceId
                       };

static DALProps *
DALSYS_GetDAL_Mod(void)
{
   return (DALProps *)&gDALModStringDeviceInfoList;
}

void
DALSYS_InitMod(DALSYSConfig * pCfg)
{
   // below is used to protect against multiple invocations
   static int hasInit = 0;
   if (!hasInit)
   {
      hasInit = 1;
      DALSYSCMN_RegisterModDriverList(&gDALModDriverInfoList);
   }
}

void
DALSYS_DeInitMod(void)
{
  DALSYSCMN_DeRegisterModDriverList(&gDALModDriverInfoList);
}

DALResult
_DALSYSCMN_GetStringDeviceObject(const char *pszDevName, const char *pszArg, DalDeviceHandle **phDevice)
{ 
   const StringDevice *pStringDeviceObj = NULL;
   DALREG_DriverInfo *pDriverInfo = NULL;
   
   if(DAL_SUCCESS == _DAL_StringDeviceObjectLookup(pszDevName, &pStringDeviceObj, DALSYS_GetDAL_Mod())){
      //Get the Property Dir Info Ptr
      pDriverInfo = pStringDeviceObj->pFunctionName;
      return pDriverInfo->pfnDALNewFunc(pszDevName,0,phDevice);
   } 
   return DAL_ERROR;
}

static uint32
DJB2_hash(const char *pszDevName)
{
   uint32 dwHash = 5381;
   while (*pszDevName)
      dwHash = ((dwHash << 5) + dwHash) + (int)*pszDevName++; /* dwHash * 33 + c */
   return dwHash;
}

DALResult
_DAL_StringDeviceObjectLookup(const char *pszDevName, const StringDevice **ppStringDeviceObj, DALProps *pDALPtr)
{
   uint32 dwDriverHash;
   int dwIndex = 0;
   int dwSize;
   const uint32 *pCollisionsIndexes = NULL;

   if(pDALPtr!= NULL && pDALPtr->pDevices!=NULL && pszDevName!= NULL 
     && dwIndex< pDALPtr->dwDeviceSize && ppStringDeviceObj!=NULL)
   {
      dwDriverHash = DJB2_hash(pszDevName); 
      for(dwIndex=0;dwIndex< pDALPtr->dwDeviceSize ; dwIndex++)
      {
         if(dwDriverHash ==  pDALPtr->pDevices[dwIndex].dwHash)
         {
            break;
         } 
      }
      // Finish the search with right device index. 
      if(dwIndex != pDALPtr->dwDeviceSize)
      {
         // if there is no Collisions.
         if(pDALPtr->pDevices[dwIndex].dwNumCollision == 0)
         {
            *ppStringDeviceObj = &(pDALPtr->pDevices[dwIndex]);
            return DAL_SUCCESS;
         }
         // If there is a Collision, scan the list for right device object.
         pCollisionsIndexes = pDALPtr->pDevices[dwIndex].pdwCollisions;
         dwSize = pDALPtr->pDevices[dwIndex].dwNumCollision;
         for(dwIndex=0;dwIndex<dwSize;dwIndex++)
         {
            // Get the dwIndex, where the str might be
            if(AsciiStrLen(pszDevName) == AsciiStrLen(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName))
            {
               if(AsciiStrnCmp(pszDevName, pDALPtr->pDevices[pCollisionsIndexes[dwIndex]].pszName, AsciiStrLen(pszDevName))==0)
               {
                  *ppStringDeviceObj = &(pDALPtr->pDevices[pCollisionsIndexes[dwIndex]]);
                  return DAL_SUCCESS;
               }
            }
         }
      }   
   }
   return DAL_ERROR_NOT_FOUND;
}

/*=============================================================================
 Stubbed Function Definitions
=============================================================================*/
DALResult
System_DalSystem_Attach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   *phDalDevice = NULL;
   return DAL_ERROR;
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

// Allocate global context from shared memory driver
EFI_STATUS
_AllocGlbCtxtMem(DALSYSMemAddr *p_ctxt_addr, UINT32 size)
{
   *p_ctxt_addr = (DALSYSMemAddr)smem_alloc(SMEM_DAL_AREA, size);
   if (*p_ctxt_addr)
   {
      return EFI_SUCCESS;
   }
   else
   {
      return EFI_PROTOCOL_ERROR;
   }
}
