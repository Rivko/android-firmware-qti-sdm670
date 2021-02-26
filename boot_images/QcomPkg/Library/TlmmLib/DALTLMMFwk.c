/*==============================================================================

FILE:      DalTLMMFwk.c

DESCRIPTION: This file implements a TLMM DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "DALTLMM.h"
#include "DALFramework.h"
#include "DALSys.h"
#include "DDITlmm.h"
#include "DALTLMM.h"
#include "DALSys.h"
#include "string.h" //for memset warning


DALResult 
TLMM_DalTlmm_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
TLMM_DalTlmm_AddRef(DalTlmmHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

TLMMDrvCtxt TLMM_DriverCtxt = 
{
    {
        TLMM_DriverInit,
 		TLMM_DriverDeInit
 	},1,
 	sizeof(TLMMDevCtxt)
};

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
TLMM_DalTlmm_Detach(DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
	return dwref;
}

static DALResult 
TLMM_DalTlmm_Init(DalDeviceHandle *h)
{  
   TLMMClientCtxt *pClientCtxt = (TLMMClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pDevCtxt->DevId,
	                           pClientCtxt->pDevCtxt->hProp);
   return TLMM_DeviceInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_DeInit(DalDeviceHandle *h)
{
   return TLMM_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_PowerEvent(DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return TLMM_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
TLMM_DalTlmm_Open(DalDeviceHandle* h, uint32 mode)
{
    return TLMM_Open(h->pClientCtxt,mode);
}

static DALResult 
TLMM_DalTlmm_Close(DalDeviceHandle* h)
{
    return TLMM_Close(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_Info(DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   info->Version = DALTLMM_INTERFACE_VERSION;
   return TLMM_Info(h->pClientCtxt,info,infoSize);
}

static DALResult 
TLMM_DalTlmm_SysRequest(DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTlmm Interface. 
------------------------------------------------------------------------------*/

static DALResult 
TLMM_DalTlmm_ConfigGpio( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioEnableType enable) 
{
	return TLMM_ConfigGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, enable);
}
static DALResult 
TLMM_DalTlmm_ConfigGpioGroup( DalDeviceHandle * h, DALGpioEnableType enable, DALGpioSignalType *gpio_configs, uint32 size) 
{
	return TLMM_ConfigGpioGroup(((DalTlmmHandle *)h)->pClientCtxt, enable, gpio_configs, size);
}
static DALResult 
TLMM_DalTlmm_GpioIn( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioValueType* value) 
{
	return TLMM_GpioIn(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, value);
}
static DALResult 
TLMM_DalTlmm_GpioOut( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioValueType value) 
{
	return TLMM_GpioOut(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, value);
}
static DALResult 
TLMM_DalTlmm_GpioOutGroup( DalDeviceHandle * h, DALGpioSignalType* gpio_configs, uint32 size, DALGpioValueType value) 
{
	return TLMM_GpioOutGroup(((DalTlmmHandle *)h)->pClientCtxt, gpio_configs, size, value);
}
static DALResult 
TLMM_DalTlmm_SetPort( DalDeviceHandle * h, DALGpioPortType port, uint32  value) 
{
	return TLMM_SetPort(((DalTlmmHandle *)h)->pClientCtxt, port, value);
}
static DALResult 
TLMM_DalTlmm_GetCurrentConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
	return TLMM_GetCurrentConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_SetInactiveConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType gpio_config ) 
{
	return TLMM_SetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_GetInactiveConfig( DalDeviceHandle * h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
	return TLMM_GetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_GetGpioNumber( DalDeviceHandle * h, DALGpioSignalType gpio_config, uint32 *  number) 
{
	return TLMM_GetGpioNumber(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, number);
}
static DALResult 
TLMM_DalTlmm_GetGpioStatus( DalDeviceHandle * h, DALGpioSignalType gpio_config, DALGpioStatusType *peStatus) 
{
	return TLMM_GetGpioStatus(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, peStatus);
}
static DALResult 
TLMM_DalTlmm_GetOutput( DalDeviceHandle * h, uint32 gpio_number, DALGpioValueType* value ) 
{
	return TLMM_GetOutput( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, value );
}
static DALResult 
TLMM_DalTlmm_PostInit( DalDeviceHandle * h) 
{
	return TLMM_PostInit( ((DalTlmmHandle *)h)->pClientCtxt );
}

static void 
TLMM_InitInterface(TLMMClientCtxt* pclientCtxt)
{
    static const DalTlmm vtbl = {
       {
          TLMM_DalTlmm_Attach,
          TLMM_DalTlmm_Detach,
          TLMM_DalTlmm_Init,
          TLMM_DalTlmm_DeInit,
          TLMM_DalTlmm_Open,
          TLMM_DalTlmm_Close,
          TLMM_DalTlmm_Info,
          TLMM_DalTlmm_PowerEvent,
          TLMM_DalTlmm_SysRequest
        } ,
		TLMM_DalTlmm_ConfigGpio,
		TLMM_DalTlmm_ConfigGpioGroup,
		TLMM_DalTlmm_GpioIn,
		TLMM_DalTlmm_GpioOut,
		TLMM_DalTlmm_GpioOutGroup,
		TLMM_DalTlmm_SetPort,
    TLMM_DalTlmm_GetCurrentConfig,
    TLMM_DalTlmm_SetInactiveConfig,
    TLMM_DalTlmm_GetInactiveConfig,
    TLMM_DalTlmm_GetGpioNumber,
    TLMM_DalTlmm_GetGpioStatus,
    TLMM_DalTlmm_GetOutput,
    TLMM_DalTlmm_PostInit
    };  
	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTlmmHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTlmmHandle.pVtbl  = &vtbl;
    pclientCtxt->DalTlmmHandle.pClientCtxt = pclientCtxt;

}

DALResult 
TLMM_DalTlmm_Attach(const char *pszArg, DALDEVICEID DeviceId, 
                            DalDeviceHandle **phDalDevice)
{               
    DALResult nErr;
    static TLMMClientCtxt ClientCtxt;
    TLMMClientCtxt *pClientCtxt =  &ClientCtxt;

	*phDalDevice = NULL;

    nErr = DALFW_AttachToDevice(DALDEVICEID_TLMM,(DALDrvCtxt *)&TLMM_DriverCtxt,
			                    (DALClientCtxt *)pClientCtxt);
    if (DAL_SUCCESS == nErr)
    {
        TLMM_InitInterface(pClientCtxt);
		TLMM_DalTlmm_AddRef(&(pClientCtxt->DalTlmmHandle));
		*phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalTlmmHandle);
    }
    return nErr;

}

