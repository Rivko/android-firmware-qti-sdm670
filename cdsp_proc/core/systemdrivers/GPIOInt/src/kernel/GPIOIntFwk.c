/*==============================================================================

  D A L   G P I O   I N T E R R U P T   C O N T R O L L E R  F R A M E W O R K  

DESCRIPTION
  This file implements a GPIOInt DeviceDriver.
  The definitions and api in this module are used internally 
  in the GPIO interrupt controller and are not to be exposed to the clients.

REFERENCES

       Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/GPIOInt/src/kernel/GPIOIntFwk.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIGPIOInt.h"
#include "GPIOInt.h"
#include "string.h" //for memset warning


DALResult 
GPIOInt_GPIOInt_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
GPIOInt_GPIOInt_AddRef(GPIOIntHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
GPIOInt_GPIOInt_Detach(uint32 ClientHandle, DalDeviceHandle* h )
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
     if(0 == dwref)	{
        //Release the client ctxt;
        DALSYS_Free(h->pClientCtxt);
    }
    return dwref;
  }
  else
  {
    return 0;
  }
}

static DALResult 
GPIOInt_GPIOInt_Init(uint32 ClientHandle, DalDeviceHandle *h)
{  
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    GPIOIntClientCtxt *pClientCtxt = (GPIOIntClientCtxt *)(h->pClientCtxt);
    DALSYS_GetDALPropertyHandle(pClientCtxt->pGPIOIntDevCtxt->DevId,
                                pClientCtxt->pGPIOIntDevCtxt->hProp);
    return GPIOIntr_DeviceInit(h->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_DeInit(uint32 ClientHandle, DalDeviceHandle *h)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_DeviceDeInit(h->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_PowerEvent(uint32 ClientHandle, DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_Open(uint32 ClientHandle, DalDeviceHandle* h , uint32 mode)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_Open(h->pClientCtxt,mode);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_Close(uint32 ClientHandle, DalDeviceHandle* h )
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_Close(h->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_Info(uint32 ClientHandle, DalDeviceHandle* h , DalDeviceInfo* info, uint32 infoSize)
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    info->Version = GPIOINT_INTERFACE_VERSION;
    return GPIOIntr_Info(h->pClientCtxt,info,infoSize);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_SysRequest(uint32 ClientHandle, DalDeviceHandle* h ,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalGPIOInt Interface. 
------------------------------------------------------------------------------*/

static DALResult 
GPIOInt_GPIOInt_SetTrigger(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, GPIOIntTriggerType  trigger) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_SetTrigger(((GPIOIntHandle *)h)->pClientCtxt, gpio, trigger);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_RegisterIsr(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, GPIOIntTriggerType trigger, 
                             GPIOINTISR isr,GPIOINTISRCtx param) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_RegisterIsr(((GPIOIntHandle *)h)->pClientCtxt, gpio, trigger,isr,param);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_RegisterInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, GPIOIntTriggerType trigger, 
                             GPIOINTISR isr,GPIOINTISRCtx param,uint32 nFlags) 
{
	return DAL_ERROR;
}
static DALResult 
GPIOInt_GPIOInt_RegisterEvent(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio,GPIOIntTriggerType trigger, 
                                                                  DALSYSEventHandle event) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_RegisterEvent(((GPIOIntHandle *)h)->pClientCtxt,gpio,trigger,event);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_DeRegisterEvent(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio,DALSYSEventHandle event) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_DeRegisterEvent(((GPIOIntHandle *)h)->pClientCtxt, gpio,event);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_DeregisterIsr(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, GPIOINTISR isr) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_DeregisterIsr(((GPIOIntHandle *)h)->pClientCtxt, gpio, isr);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptEnabled(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_IsInterruptEnabled(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptPending(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_IsInterruptPending(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptSet(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_IsInterruptSet(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_IsInterruptRegistered(uint32 ClientHandle,  DalDeviceHandle * h, uint32  gpio, void *  state, uint32  size) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_IsInterruptRegistered(((GPIOIntHandle *)h)->pClientCtxt, gpio, (uint32 *)state);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_Save(uint32 ClientHandle,  DalDeviceHandle * h) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_Save(((GPIOIntHandle *)h)->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}
static DALResult 
GPIOInt_GPIOInt_Restore(uint32 ClientHandle,  DalDeviceHandle * h) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_Restore(((GPIOIntHandle *)h)->pClientCtxt);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_DisableInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_DisableInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_EnableInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_EnableInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_DisableGPIOInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_DisableGPIOInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_EnableGPIOInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_EnableGPIOInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_InterruptNotify(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
	return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_MonitorInterrupts(uint32 ClientHandle,  DalDeviceHandle * h, GPIOINTISR isr, uint32 enable) 
{
  /*
   * This API is not supported on 8660/8960.
   */
	return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_MapMPMInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio, uint32 mpm_interrupt_id) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_MapMPMInterrupt(((GPIOIntHandle *)h)->pClientCtxt, gpio, mpm_interrupt_id);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_AttachRemote(uint32 ClientHandle,  DalDeviceHandle * h, uint32 processor) 
{
  /*
   * This API is not supported on 8660/8960.
   */
  return DAL_ERROR;
}

static DALResult 
GPIOInt_GPIOInt_TriggerInterrupt(uint32 ClientHandle, DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_TriggerInterrupt(((GPIOIntHandle *)h)->pClientCtxt,gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_ClearInterrupt(uint32 ClientHandle,  DalDeviceHandle * h, uint32 gpio) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_ClearInterrupt(((GPIOIntHandle *)h)->pClientCtxt,gpio);
  }
  else
  {
    return DAL_ERROR;
  }
}

static DALResult 
GPIOInt_GPIOInt_SetDirectConnectGPIOMapping( uint32 ClientHandle, DalDeviceHandle * h, uint32 gpio, uint32 direct_connect_line) 
{
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return GPIOIntr_SetDirectConnectGPIOMapping(((GPIOIntHandle *)h)->pClientCtxt,gpio,direct_connect_line);
  }
  else
  {
    return DAL_ERROR;
  }
}

static void 
GPIOInt_InitInterface(GPIOIntClientCtxt* pclientCtxt)
{
    static const GPIOInt vtbl = {
       {
          GPIOInt_GPIOInt_Attach,
          GPIOInt_GPIOInt_Detach,
          GPIOInt_GPIOInt_Init,
          GPIOInt_GPIOInt_DeInit,
          GPIOInt_GPIOInt_Open,
          GPIOInt_GPIOInt_Close,
          GPIOInt_GPIOInt_Info,
          GPIOInt_GPIOInt_PowerEvent,
          GPIOInt_GPIOInt_SysRequest
        } ,
	GPIOInt_GPIOInt_SetTrigger,
	GPIOInt_GPIOInt_RegisterIsr,
        GPIOInt_GPIOInt_RegisterEvent,
        GPIOInt_GPIOInt_DeRegisterEvent,
	GPIOInt_GPIOInt_DeregisterIsr,
	GPIOInt_GPIOInt_IsInterruptEnabled,
	GPIOInt_GPIOInt_IsInterruptPending,
	GPIOInt_GPIOInt_Save,
	GPIOInt_GPIOInt_Restore,
        GPIOInt_GPIOInt_DisableInterrupt,
        GPIOInt_GPIOInt_EnableInterrupt,
        GPIOInt_GPIOInt_InterruptNotify,
        GPIOInt_GPIOInt_MonitorInterrupts,
        GPIOInt_GPIOInt_MapMPMInterrupt,
        GPIOInt_GPIOInt_AttachRemote,
        GPIOInt_GPIOInt_TriggerInterrupt,
        GPIOInt_GPIOInt_ClearInterrupt,
        GPIOInt_GPIOInt_IsInterruptSet,
        GPIOInt_GPIOInt_SetDirectConnectGPIOMapping,
        GPIOInt_GPIOInt_IsInterruptRegistered,
        GPIOInt_GPIOInt_DisableGPIOInterrupt,
        GPIOInt_GPIOInt_EnableGPIOInterrupt,
       GPIOInt_GPIOInt_RegisterInterrupt,
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->GPIOIntHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->GPIOIntHandle.pVtbl  = &vtbl;
    pclientCtxt->GPIOIntHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));
    pclientCtxt->GPIOIntHandle.pClientCtxt = pclientCtxt;

}

DALResult 
GPIOInt_GPIOInt_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
    static GPIOIntDrvCtxt drvCtxt = {{GPIOIntr_DriverInit,
                                      GPIOIntr_DriverDeInit
                                     },2,
                                     sizeof(GPIOIntDevCtxt)};

    GPIOIntClientCtxt *pclientCtxt =  NULL;

    nErr = DALSYS_Malloc(sizeof(GPIOIntClientCtxt),(void **)&pclientCtxt);
    if(nErr != DAL_SUCCESS)
    {
      return nErr;
    }
    DALSYS_memset(pclientCtxt,0,sizeof(GPIOIntClientCtxt));

    *phDalDevice = NULL;

     if (DAL_SUCCESS == nErr) {
    nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&drvCtxt,
                        (DALClientCtxt *)pclientCtxt);
        if (DAL_SUCCESS == nErr)
    {
      GPIOInt_InitInterface(pclientCtxt);
      GPIOInt_GPIOInt_AddRef(&(pclientCtxt->GPIOIntHandle));
			*phDalDevice = (DalDeviceHandle *)&(pclientCtxt->GPIOIntHandle);
        }
    } 
    return nErr;
}

