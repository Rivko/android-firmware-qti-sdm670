/*==============================================================================

FILE:      DalInterruptControllerFwk.c

DESCRIPTION: This file implements a InterruptController DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIInterruptController.h"
#include "DALInterruptController.h"
#include "string.h" //for memset warning


DALResult 
InterruptController_DalInterruptController_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
InterruptController_DalInterruptController_AddRef(DalInterruptControllerHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
InterruptController_DalInterruptController_Detach(uint32 ClientHandle, DalDeviceHandle* h)
{
  uint32 dwref;
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
  }
  else
  {
    return 0;
  }
}

static DALResult 
InterruptController_DalInterruptController_Init(uint32 ClientHandle, DalDeviceHandle* h)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_DeviceInit(h->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
InterruptController_DalInterruptController_DeInit(uint32 ClientHandle, DalDeviceHandle* h)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_DeviceDeInit(h->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
InterruptController_DalInterruptController_PowerEvent(uint32 ClientHandle, DalDeviceHandle* h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Open(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Close(uint32 ClientHandle, DalDeviceHandle* h)
{
    return DAL_SUCCESS;
}

static DALResult 
InterruptController_DalInterruptController_Info(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    info->Version = DALINTERRUPTCONTROLLER_INTERFACE_VERSION;
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
InterruptController_DalInterruptController_SysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalInterruptController Interface. 
------------------------------------------------------------------------------*/

static DALResult 
InterruptController_DalInterruptController_RegisterISR(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID, const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_RegisterISR(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, isr, ctx, IntrFlags);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
InterruptController_DalInterruptController_RegisterIST(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID, const DALISR  isr, const DALISRCtx  ctx, uint32  IntrFlags, char* pISTName) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_RegisterIST(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, isr, ctx, IntrFlags, pISTName);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
InterruptController_DalInterruptController_RegisterEvent(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID, const DALSYSEventHandle  hEvent, uint32  IntrFlags) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_RegisterEvent(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID, hEvent, IntrFlags);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_Unregister(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_Unregister(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptDone(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptDone(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptEnable(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptEnable(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptDisable(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptDisable(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptTrigger(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptClear(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptClear(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_InterruptStatus(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_InterruptStatus(((DalInterruptControllerHandle *)h)->pClientCtxt, intrID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
InterruptController_DalInterruptController_RegisterIRQHandler(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID, const DALIRQ  irq, const DALIRQCtx  ctx, uint32  bEnable) 
{
	return DAL_ERROR;
}
static DALResult 
InterruptController_DalInterruptController_SetTrigger(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID intrID, uint32 nTrigger) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_SetTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID, nTrigger);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult
InterruptController_DalInterruptController_IsInterruptPending(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID,void* bState,uint32 size)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_IsInterruptPending(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,(uint32*)bState);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult
InterruptController_DalInterruptController_IsInterruptEnabled(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID,void* bState,uint32 size)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_IsInterruptEnabled(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,(uint32*)bState);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult
InterruptController_DalInterruptController_MapWakeupInterrupt(uint32 ClientHandle, DalDeviceHandle* h, DALInterruptID  intrID,uint32 nWakeupIntID)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_MapWakeupInterrupt(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,nWakeupIntID);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult
InterruptController_DalInterruptController_IsAnyInterruptPending(uint32 ClientHandle, DalDeviceHandle* h, uint32* bState,uint32 size)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_IsAnyInterruptPending(((DalInterruptControllerHandle *)h)->pClientCtxt,bState);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult
InterruptController_DalInterruptController_Sleep(uint32 ClientHandle, DalDeviceHandle* h, InterruptControllerSleepType sleep)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_Sleep(((DalInterruptControllerHandle *)h)->pClientCtxt, sleep);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult
InterruptController_DalInterruptController_Wakeup(uint32 ClientHandle, DalDeviceHandle* h,InterruptControllerSleepType sleep)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_Wakeup(((DalInterruptControllerHandle *)h)->pClientCtxt,sleep);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult
InterruptController_DalInterruptController_GetInterruptTrigger(uint32 ClientHandle, DalDeviceHandle* h,DALInterruptID intrID ,uint32* eTrigger, uint32 size)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_GetInterruptTrigger(((DalInterruptControllerHandle *)h)->pClientCtxt,intrID,eTrigger);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult
InterruptController_DalInterruptController_GetInterruptID(uint32 ClientHandle, DalDeviceHandle* h,const char* szIntrName,uint32* pnIntrID, uint32 size)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_GetInterruptID(((DalInterruptControllerHandle *)h)->pClientCtxt,szIntrName,pnIntrID);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult
InterruptController_DalInterruptController_LogState(uint32 ClientHandle, DalDeviceHandle* h,void *pULog)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return InterruptController_LogState(((DalInterruptControllerHandle *)h)->pClientCtxt,pULog);
  }
  else
  {
    return DAL_ERROR;
  }
}

static void 
InterruptController_InitInterface(InterruptControllerClientCtxt* pclientCtxt)
{
    static const DalInterruptController vtbl = {
       {
          InterruptController_DalInterruptController_Attach,
          InterruptController_DalInterruptController_Detach,
          InterruptController_DalInterruptController_Init,
          InterruptController_DalInterruptController_DeInit,
          InterruptController_DalInterruptController_Open,
          InterruptController_DalInterruptController_Close,
          InterruptController_DalInterruptController_Info,
          InterruptController_DalInterruptController_PowerEvent,
          InterruptController_DalInterruptController_SysRequest
        } ,
		InterruptController_DalInterruptController_RegisterISR,
		InterruptController_DalInterruptController_RegisterIST,
		InterruptController_DalInterruptController_RegisterEvent,
		InterruptController_DalInterruptController_Unregister,
		InterruptController_DalInterruptController_InterruptDone,
		InterruptController_DalInterruptController_InterruptEnable,
		InterruptController_DalInterruptController_InterruptDisable,
		InterruptController_DalInterruptController_InterruptTrigger,
		InterruptController_DalInterruptController_InterruptClear,
		InterruptController_DalInterruptController_InterruptStatus,
        InterruptController_DalInterruptController_RegisterIRQHandler,
        InterruptController_DalInterruptController_SetTrigger,
        InterruptController_DalInterruptController_IsInterruptPending,
        InterruptController_DalInterruptController_IsInterruptEnabled,
        InterruptController_DalInterruptController_MapWakeupInterrupt,
        InterruptController_DalInterruptController_IsAnyInterruptPending,
        InterruptController_DalInterruptController_Sleep,
        InterruptController_DalInterruptController_Wakeup,
        InterruptController_DalInterruptController_GetInterruptTrigger,
        InterruptController_DalInterruptController_GetInterruptID,
        InterruptController_DalInterruptController_LogState

    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalInterruptControllerHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalInterruptControllerHandle.pVtbl  = &vtbl;
    pclientCtxt->DalInterruptControllerHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));
    pclientCtxt->DalInterruptControllerHandle.pClientCtxt = pclientCtxt;

}

DALResult 
InterruptController_DalInterruptController_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static InterruptControllerDrvCtxt drvCtxt = {{InterruptController_DriverInit,
									   InterruptController_DriverDeInit
									  },1,
									  sizeof(InterruptControllerDevCtxt)};
    static InterruptControllerClientCtxt clientCtxt;

    InterruptControllerClientCtxt *pclientCtxt = &clientCtxt;


	*phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            InterruptController_InitInterface(pclientCtxt);
			InterruptController_DalInterruptController_AddRef(&(pclientCtxt->DalInterruptControllerHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalInterruptControllerHandle);
        }
    return nErr;
}

