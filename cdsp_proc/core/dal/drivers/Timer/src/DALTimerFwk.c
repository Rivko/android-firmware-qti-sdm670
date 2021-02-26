/*==============================================================================

FILE:      DalTimerFwk.c

DESCRIPTION: This file implements a Timer DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDITimer.h"
#include "DALTimer.h"
#include "string.h" //for memset warning


DALResult 
Timer_DalTimer_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
Timer_DalTimer_AddRef(DalTimerHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
Timer_DalTimer_Detach(uint32 ClientHandle, DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
Timer_DalTimer_Init(uint32 ClientHandle, DalDeviceHandle *h)
{
   TimerClientCtxt *pTimerClientCtxt = (TimerClientCtxt *)(h->pClientCtxt);
   TimerDevCtxt *pTimerDevCtxt = pTimerClientCtxt->pTimerDevCtxt;

   /*----------------------------------------------------------------------
    Establish the DAL property handle for this device
    ----------------------------------------------------------------------*/
   DALSYS_GetDALPropertyHandle(pTimerDevCtxt->DevId,  pTimerDevCtxt->hProp);  

   return Timer_DeviceInit(pTimerClientCtxt);
}

static DALResult 
Timer_DalTimer_DeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
   return Timer_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
Timer_DalTimer_PowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return Timer_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
Timer_DalTimer_Open(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
    return Timer_Open(h->pClientCtxt,mode);
}

static DALResult 
Timer_DalTimer_Close(uint32 ClientHandle, DalDeviceHandle* h)
{
    return Timer_Close(h->pClientCtxt);
}

static DALResult 
Timer_DalTimer_Info(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   DalDeviceInfo sInfo;
   sInfo.Version = DALTIMER_INTERFACE_VERSION;

   if( 0 != DalSys_CopyToUser(ClientHandle, &info->Version, &sInfo.Version, sizeof(sInfo.Version)))
   {
     return DAL_ERROR;
   }
   return Timer_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
Timer_DalTimer_SysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_ERROR_NOT_SUPPORTED;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTimer Interface. 
------------------------------------------------------------------------------*/

static DALResult 
Timer_DalTimer_Register( uint32 ClientHandle, DalDeviceHandle * h, const DALSYSEventHandle  hEvent, uint32  time) 
{
	return Timer_Register(((DalTimerHandle *)h)->pClientCtxt, hEvent, time);
}
static DALResult 
Timer_DalTimer_UnRegister(uint32 ClientHandle, DalDeviceHandle * h, const DALSYSEventHandle  hEvent) 
{
	return Timer_UnRegister(((DalTimerHandle *)h)->pClientCtxt, hEvent);
}
static DALResult 
Timer_DalTimer_GetTimerCount(uint32 ClientHandle, DalDeviceHandle * h, void * timer_count,  uint32 size) 
{
  DALResult retVal;
  unsigned long long count; 
  retVal = Timer_GetTimerCount(((DalTimerHandle *)h)->pClientCtxt, &count, size);

  if(DAL_SUCCESS == retVal)
  {
      if(0 != DalSys_CopyToUser(ClientHandle, timer_count, &count, sizeof(unsigned long long)))
      {
        retVal = DAL_ERROR;
      }
  }

	return retVal;
}

static void 
Timer_InitInterface(TimerClientCtxt* pclientCtxt)
{
    static const DalTimer vtbl = {
       {
          Timer_DalTimer_Attach,
          Timer_DalTimer_Detach,
          Timer_DalTimer_Init,
          Timer_DalTimer_DeInit,
          Timer_DalTimer_Open,
          Timer_DalTimer_Close,
          Timer_DalTimer_Info,
          Timer_DalTimer_PowerEvent,
          Timer_DalTimer_SysRequest
        } ,
      Timer_DalTimer_Register,
      Timer_DalTimer_UnRegister,
      Timer_DalTimer_GetTimerCount
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTimerHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTimerHandle.pVtbl  = &vtbl;
    pclientCtxt->DalTimerHandle.pClientCtxt = pclientCtxt;
    pclientCtxt->DalTimerHandle.dwVtblen = sizeof(vtbl)/sizeof(void(*)(void));

}

DALResult 
Timer_DalTimer_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
	static TimerDrvCtxt drvCtxt = {{Timer_DriverInit,
									   Timer_DriverDeInit
									  },1,
									  sizeof(TimerDevCtxt)};
    static TimerClientCtxt clientCtxt;

    TimerClientCtxt *pclientCtxt = &clientCtxt;


	*phDalDevice = NULL;

        nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
			                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
        {
            Timer_InitInterface(pclientCtxt);
			Timer_DalTimer_AddRef(&(pclientCtxt->DalTimerHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalTimerHandle);
        }
    return nErr;
}

