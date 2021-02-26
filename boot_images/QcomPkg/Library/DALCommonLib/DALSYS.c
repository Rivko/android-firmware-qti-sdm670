/** @file DALSYS.c

  DALSYS

  Copyright (c) 2010-2011,2014,2019 by Qualcomm Technologies, Inc. All Rights Reserved

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/30/16   vk      Move memcpy_s here
 08/11/14   sho     Creation

=============================================================================*/

#include "DALStdDef.h"
#include "DALStdErr.h"
#include "DALSysCmn.h"
#include "DALSys.h"
#include "DALReg.h"
#include <Uefi.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "string.h"

/*-------------------------------------------------------------------
 Constant Definitions 
 ------------------------------------------------------------------*/
#define ALIGN_MASK      0x00000001f
#define ALIGN_UP( s )   (((DALSYSMemAddr)(s) + ALIGN_MASK) & ~(ALIGN_MASK))
#define ADD_SIZE_FOR_PADDING(s)  ((s) + ALIGN_MASK) 

#define PID             0xB000000D

/*-------------------------------------------------------------------
 Structure Definitions 
 ------------------------------------------------------------------*/
typedef struct 
{
   INHERIT_DALSYS_BASE_OBJ();
   DALSYSCallbackFunc    cbFunc;
   DALSYSCallbackFuncCtx cbFuncCtx;
}
_DALSYSEventObj;

typedef struct 
{
   DALSYSMemObjBase BaseObj;
   void *pMemRegion;
}
_DALSYSMemObj;

/*-------------------------------------------------------------------
 Variable Definitions 
 ------------------------------------------------------------------*/

/*-------------------------------------------------------------------
 Function Definitions 
 ------------------------------------------------------------------*/

static uint32 updateUncachedMemProps( uint32 memProps )
{
   if((memProps & DALSYS_MEM_PROPS_CACHED_MASK) == DALSYS_MEM_PROPS_UNCACHED)
      memProps = (memProps & ~DALSYS_MEM_PROPS_CACHED_MASK) | DALSYS_MEM_PROPS_CACHED_WB;
   return memProps;
}

DALResult
DALSYS_EventCreate(uint32 dwEvtAttrib, DALSYSEventHandle *phEvent,
                   DALSYSEventObj *pEventObj)
{
   _DALSYSEventObj *_pEventObj = (_DALSYSEventObj *)pEventObj;
   // only support statically pre-allocated object
   if((_pEventObj == NULL) || (phEvent == NULL ))
   {   
      DEBUG(( EFI_D_ERROR, "Event object 0x%x or Event handle 0x%x is NULL\n", 
             _pEventObj, phEvent ));
      return DAL_ERROR;
   }

   // only support callback event
   if(dwEvtAttrib & DALSYS_EVENT_ATTR_CALLBACK_EVENT)
   {
      _pEventObj->dwObjInfo = 0;
      DALSYSCMN_SET_OBJECT_ID(_pEventObj->dwObjInfo,DALSYSCMN_EVENT_OBJECT_ID);
      DALSYSCMN_SET_CALLBACK_EVENT_BIT(_pEventObj->dwObjInfo);
   }
   else
   {
      DEBUG(( EFI_D_ERROR, "Attempt to create a non-callback event; dwEvtAttrib 0x%x\n", 
             dwEvtAttrib ));
      return DAL_ERROR;
   }

   *phEvent = (DALSYSEventHandle)_pEventObj;
   return DAL_SUCCESS;
}

DALResult
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl, uint32 dwParam,
                   void *pPayload, uint32 dwPayloadSize)
{
   DALResult nErr = DAL_ERROR;
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);
   if(pEventObj)
   {
      switch(dwCtrl)
      {
         case DALSYS_EVENT_CTRL_TRIGGER:
            if(DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
            {
                if(pEventObj->cbFunc)
                {
                    pEventObj->cbFunc(pEventObj->cbFuncCtx,
                                dwParam,pPayload,dwPayloadSize);
                    nErr = DAL_SUCCESS;
                }
            }
            break;

         default:
            nErr = DAL_SUCCESS;
            break;
      }
   }
   return nErr;
}

DALResult
DALSYS_SetupCallbackEvent(DALSYSEventHandle hEvent, DALSYSCallbackFunc cbFunc,
                          DALSYSCallbackFuncCtx cbFuncCtx)
{
  _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);

   if(pEventObj && DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
   {
      pEventObj->cbFunc = cbFunc;
      pEventObj->cbFuncCtx = cbFuncCtx;
      return DAL_SUCCESS;
   }
   DEBUG(( EFI_D_ERROR, "Attempt to setup a callback func in a non-callback event\n" ));
   return DAL_ERROR;
}

DALResult
DALSYS_SyncCreate(uint32 dwAttribs,
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pSyncObj)
{
   static DALEventObject dummyEventObject;
   
   if(pSyncObj == NULL)
      *phSync = (DALSYSSyncHandle)&dummyEventObject;
   else
      *phSync = (DALSYSSyncHandle)pSyncObj;
   return DAL_SUCCESS;
}

void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, 
      const char * pszLog, ...)
{
   uint32 dwEFILogEventType;

   switch (dwLogEventType)
   {
      case DALSYS_LOGEVENT_WARNING:
         dwEFILogEventType = EFI_D_WARN;
         break;

      case DALSYS_LOGEVENT_INFO:
         dwEFILogEventType = EFI_D_INFO;
         break;

      case DALSYS_LOGEVENT_FATAL_ERROR:
         ASSERT_EFI_ERROR (0);
         // fall-thru if ASSERT is disabled

      default:
         dwEFILogEventType = EFI_D_ERROR;
         break;
   }
   DEBUG ((dwEFILogEventType, "DALLOG Device [0x%x]: %a\n",
           DeviceId, pszLog ));
}
                             
static void *
_CreateMemSection(uint32 dwLen, uint32 dwAttribs,
                  DALSYSMemAddr *VirtualAddr,DALSYSMemAddr *PhysicalAddr)
{
    void *pMemRegion = NULL;

    //Only support no addr specified in this environemt
    if ( DALSYS_MEM_ADDR_NOT_SPECIFIED  == *PhysicalAddr && 
         DALSYS_MEM_ADDR_NOT_SPECIFIED == *VirtualAddr )
    {
       if(DAL_SUCCESS != DALSYS_Malloc(ADD_SIZE_FOR_PADDING(dwLen),&pMemRegion))
       {             
          pMemRegion = NULL;
       }
       pMemRegion = (void *)ALIGN_UP(pMemRegion);
       *VirtualAddr = (DALSYSMemAddr)pMemRegion;
       *PhysicalAddr = (DALSYSMemAddr)pMemRegion;
    }
    else
    {
       pMemRegion = NULL;
       DEBUG(( EFI_D_ERROR, "Cannot use specified virtual or physical addresses to create mem sections\n" ));
    }
    return pMemRegion;
}


static
_DALSYSMemObj *_AllocAndInitMemObj(_DALSYSMemObj *pMemObj,
                    uint32 dwLen,
                    uint32 dwAttribs,
                    void * pMemRegion,
                    DALSYSMemAddr VirtualAddr,
                    DALSYSMemAddr PhysicalAddr)
{
    // VirtualAddr and PhysicalAddr should not be NULL
    if(0 == VirtualAddr || 0 == PhysicalAddr)
    {
       DEBUG(( EFI_D_ERROR, "MMU SW Config Error; VirtualAddr:0x%x PhysicalAddr:0x%x\n", 
              VirtualAddr, PhysicalAddr ));
       return NULL;
    }

    if(NULL == pMemObj)
    {
        if(DAL_SUCCESS == DALSYS_Malloc(DALSYS_MEM_OBJ_SIZE_BYTES,
                                         (void **)&pMemObj))
        {
            DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
            DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pMemObj->BaseObj.dwObjInfo);
        }
        else
        {
           DEBUG(( EFI_D_ERROR, "Insufficient memory for memory object\n" ));
           return NULL;
        }
    }
    else
    {
        DALSYSCMN_RESET_ALL_BITS(pMemObj->BaseObj.dwObjInfo);
    }

    DALSYSCMN_SET_OBJECT_ID(pMemObj->BaseObj.dwObjInfo,DALSYSCMN_MEM_OBJECT_ID);

    // Update mem properties if we are asking for uncached
    // since we only support cached-WB
    dwAttribs = updateUncachedMemProps(dwAttribs);

    DALSYSCMN_SET_ATTRIB(pMemObj->BaseObj.dwObjInfo,dwAttribs);
    DALSYSCMN_RESET_MEM_COPY_BIT(pMemObj->BaseObj.dwObjInfo);

    pMemObj->BaseObj.dwLen             = dwLen;
    pMemObj->BaseObj.hOwnerProc        = PID;
    pMemObj->BaseObj.thisVirtualAddr   = (DALSYSMemAddr)pMemObj;
    pMemObj->BaseObj.OwnerVirtualAddr  = VirtualAddr;
    pMemObj->BaseObj.VirtualAddr       = VirtualAddr;
    pMemObj->BaseObj.PhysicalAddr      = PhysicalAddr;
    pMemObj->pMemRegion                = pMemRegion;
    return pMemObj;
}


DALResult
DALSYS_MemRegionAlloc(uint32 dwAttribs, DALSYSMemAddr VirtualAddr,
                      DALSYSMemAddr PhysicalAddr, uint32 dwLen, DALSYSMemHandle *phMem,
                      DALSYSMemObj *pObj)
{
   void * pMemRegion = NULL;
   _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)pObj;
   *phMem = NULL;

   if (dwAttribs & DALSYS_MEM_PROPS_PHYS_PREALLOC)
   {
      // assign memory region as the virtual address value
      pMemRegion = (void *)VirtualAddr;
   }
   else
   {
      /*----------------------------------------------------------------------
      Create the underlying memsection
      ----------------------------------------------------------------------*/
      pMemRegion = _CreateMemSection(dwLen,dwAttribs,&VirtualAddr,&PhysicalAddr);
   }

   if(NULL == pMemRegion)
      return DAL_ERROR;

   /*----------------------------------------------------------------------
   Alloc Memory for the DALSYS Mem Obj
   ----------------------------------------------------------------------*/
   pMemObj = _AllocAndInitMemObj(pMemObj,dwLen,dwAttribs,pMemRegion,
                                        VirtualAddr,PhysicalAddr);
   if(NULL == pMemObj)
   {
      // do some clean up here in real-world case
      return DAL_ERROR;
   }

   // Return handle
   *phMem = (DALSYSMemHandle)pMemObj;
   return DAL_SUCCESS;
}

DALResult
DALSYS_MemInfo(DALSYSMemHandle hMem, DALSYSMemInfo *pMemInfo)
{
    _DALSYSMemObj *pMemObj = (_DALSYSMemObj *)DALSYSCMN_HANDLE_TO_OBJECT(hMem);
    if(pMemObj)
    {
        pMemInfo->VirtualAddr  = pMemObj->BaseObj.VirtualAddr;
        pMemInfo->PhysicalAddr = pMemObj->BaseObj.PhysicalAddr;
        pMemInfo->dwLen        = pMemObj->BaseObj.dwLen;
        pMemInfo->dwMappedLen  = pMemObj->BaseObj.dwLen;
        DALSYSCMN_GET_ATTRIB(pMemObj->BaseObj.dwObjInfo,pMemInfo->dwProps);
        return DAL_SUCCESS;
    }
    return DAL_ERROR;
}

DALResult
DALSYS_CacheCommand(uint32 CacheCmd, DALSYSMemAddr VirtualAddr, uint32 dwLen)
{
   switch(CacheCmd)
   {
      case DALSYS_CACHE_CMD_INVALIDATE:
         InvalidateDataCacheRange( (void *)VirtualAddr, dwLen );
         break;
      case DALSYS_CACHE_CMD_CLEAN:
         WriteBackDataCacheRange( (void *)VirtualAddr, dwLen );
         break;
      case DALSYS_CACHE_CMD_FLUSH:
         WriteBackInvalidateDataCacheRange( (void *)VirtualAddr, dwLen);
         break;
      default:
         DEBUG(( EFI_D_ERROR, "Invalid cache command 0x%x\n", CacheCmd ));
         return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

void
DALSYS_BusyWait(uint32 pause_time_us)
{
  MicroSecondDelay(pause_time_us);
}

UINTN DALSYS_memscpy(VOID * Destination, UINTN DestLength, VOID * Source, UINTN SrcLength)
{
  if(DestLength >= SrcLength) {
    CopyMem(Destination, Source, SrcLength);
    return SrcLength;
  }

  CopyMem(Destination, Source, DestLength);
    return DestLength;
}

VOID * DALSYS_memset(VOID * buffer, UINT8 value, UINTN length)
{
   // length and value parameters are ordered different in SetMem API
   return SetMem( buffer, length, value );
}