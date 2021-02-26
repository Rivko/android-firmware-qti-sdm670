/*==============================================================================

FILE:      DalTLMMFwk.c

DESCRIPTION: This file implements a TLMM DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2017 QUALCOMM Technologies, Incorporated.
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

DALResult 
TLMM_DalTlmm_AttachEx(const char *pszArg,
                      DALDEVICEID DeviceId, 
                      DalDeviceHandle **phDalDevice,
                      int nClientId);
  
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


#define TLMM_NUM_CLIENT_CTXT                 3

TLMMClientCtxt TLMMClients[TLMM_NUM_CLIENT_CTXT];

/*
 * Used to check GPIO size.
 */
extern uint32 gnTotalGPIO;

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
TLMM_DalTlmm_Detach(uint32 ClientHandle, DalDeviceHandle* h)
{
    uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));

  if (dwref == 0)
  {
    (void)TLMM_DetachCleanup(((DalTlmmHandle *)h)->pClientCtxt);
  }
	return dwref;
}

static DALResult 
TLMM_DalTlmm_Init(uint32 ClientHandle, DalDeviceHandle* h)
{  
   TLMMClientCtxt *pClientCtxt = (TLMMClientCtxt *)(h->pClientCtxt);
   DALSYS_GetDALPropertyHandle(pClientCtxt->pDevCtxt->DevId,
	                           pClientCtxt->pDevCtxt->hProp);
   return TLMM_DeviceInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_DeInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   return TLMM_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_PowerEvent(uint32 ClientHandle, DalDeviceHandle* h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
    return TLMM_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
TLMM_DalTlmm_Open(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
    return TLMM_Open(h->pClientCtxt,mode);
}

static DALResult 
TLMM_DalTlmm_Close(uint32 ClientHandle, DalDeviceHandle* h)
{
    return TLMM_Close(h->pClientCtxt);
}

static DALResult 
TLMM_DalTlmm_Info(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
   DALResult nResult;
   DalDeviceInfo tInfo;
   
   tInfo.Version = DALTLMM_INTERFACE_VERSION;
   nResult = TLMM_Info(h->pClientCtxt, &tInfo, infoSize);
   
   if ( DalSys_CopyToUser(ClientHandle, info, &tInfo, sizeof(tInfo)) )
   {
     return DAL_ERROR_INVALID_ADDRESS;
   }
   
   return nResult;
}

static DALResult 
TLMM_DalTlmm_SysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
	return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalTlmm Interface. 
------------------------------------------------------------------------------*/

static DALResult 
TLMM_DalTlmm_ConfigGpio(uint32 ClientHandle, DalDeviceHandle* h, DALGpioSignalType gpio_config, DALGpioEnableType enable) 
{
	return TLMM_ConfigGpio(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, enable);
}
static DALResult 
TLMM_DalTlmm_ConfigGpioGroup(uint32 ClientHandle, DalDeviceHandle* h, DALGpioEnableType enable, DALGpioSignalType *gpio_configs, uint32 size) 
{
  DALGpioSignalType *pnGpioConfigs = NULL;
  DALResult eRes = DAL_ERROR;
  
  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_ConfigGpioGroup(((DalTlmmHandle *)h)->pClientCtxt, enable, gpio_configs, size);
  }
  
  /*
   * Make sure the size is not greater than the total number of GPIO.  In this context, size is the number of configurations,
   * NOT the size in bytes.
   */
  if ( size <= gnTotalGPIO )
  {
    if (DALSYS_Malloc(size * sizeof(DALGpioSignalType), (void **)&pnGpioConfigs) != DAL_SUCCESS)
    {
      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }
    
    if ( pnGpioConfigs != NULL )
    {
      memset(pnGpioConfigs, 0x0, size * sizeof(DALGpioSignalType));
      
      if ( DalSys_CopyFromUser(ClientHandle, pnGpioConfigs, gpio_configs, (size * sizeof(DALGpioSignalType))) )
      {
        DALSYS_Free(pnGpioConfigs);
        return DAL_ERROR_INVALID_ADDRESS;
      }
      
      eRes = TLMM_ConfigGpioGroup(((DalTlmmHandle *)h)->pClientCtxt, enable, pnGpioConfigs, size);
      DALSYS_Free(pnGpioConfigs);
      return eRes;
    }
  }
  return DAL_ERROR;  
}
static DALResult 
TLMM_DalTlmm_GetGpioNumber(uint32 ClientHandle, DalDeviceHandle* h, DALGpioSignalType gpio_config, uint32 *  number) 
{
  uint32 nPinNumber;
  DALResult nResult = TLMM_GetGpioNumber(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, &nPinNumber);
  
  if (nResult == DAL_SUCCESS)
  {
    if ( DalSys_CopyToUser(ClientHandle, number, &nPinNumber, sizeof(nPinNumber)))
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
	return nResult;
}
static DALResult 
TLMM_DalTlmm_GpioIn(uint32 ClientHandle, DalDeviceHandle* h, DALGpioSignalType gpio_config, DALGpioValueType* value) 
{
  DALGpioValueType Value;
  DALResult nResult = TLMM_GpioIn(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, &Value);
  
  if ( DAL_SUCCESS == nResult )
  {
    if ( DalSys_CopyToUser(ClientHandle, value, &Value, sizeof(Value)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  return nResult;
}
static DALResult 
TLMM_DalTlmm_GpioOut(uint32 ClientHandle, DalDeviceHandle* h, DALGpioSignalType gpio_config, DALGpioValueType value) 
{
	return TLMM_GpioOut(((DalTlmmHandle *)h)->pClientCtxt, gpio_config, value);
}
static DALResult 
TLMM_DalTlmm_GpioOutGroup(uint32 ClientHandle, DalDeviceHandle* h, DALGpioSignalType* gpio_configs, uint32 size, DALGpioValueType value) 
{
  DALGpioSignalType *pnGpioConfigs = NULL;
  DALResult eRes = DAL_ERROR;
  
  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_GpioOutGroup(((DalTlmmHandle *)h)->pClientCtxt, gpio_configs, size, value);
  }
  
  /*
   * The value of "size" in this context is actually the number of elements in the array.
   */
  if ( size <= gnTotalGPIO )
  {
    if (DALSYS_Malloc(size * sizeof(DALGpioSignalType), (void **)&pnGpioConfigs) != DAL_SUCCESS)
    {
      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }
    
    if ( pnGpioConfigs != NULL )
    {
      memset(pnGpioConfigs, 0x0, size * sizeof(DALGpioSignalType));
      
      if ( DalSys_CopyFromUser(ClientHandle, &pnGpioConfigs, gpio_configs, (size * sizeof(DALGpioSignalType))) )
      {
        DALSYS_Free(pnGpioConfigs);
        return DAL_ERROR_INVALID_ADDRESS;
      }

      eRes = TLMM_GpioOutGroup(((DalTlmmHandle *)h)->pClientCtxt, pnGpioConfigs, size, value);
      DALSYS_Free(pnGpioConfigs);
      return eRes;
    }
  }
  return DAL_ERROR;
}
static DALResult 
TLMM_DalTlmm_SetPort(uint32 ClientHandle, DalDeviceHandle* h, DALGpioPortType port, uint32  value) 
{
	return TLMM_SetPort(((DalTlmmHandle *)h)->pClientCtxt, port, value);
}
static DALResult 
TLMM_DalTlmm_GetCurrentConfig(uint32 ClientHandle, DalDeviceHandle* h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
  DALGpioSignalType nGpioConfig;
  DALResult nResult = TLMM_GetCurrentConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, &nGpioConfig );
  
  if ( DAL_SUCCESS == nResult )
  {
    if ( DalSys_CopyToUser(ClientHandle, gpio_config, &nGpioConfig, sizeof(DALGpioSignalType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  return nResult;  
}
static DALResult 
TLMM_DalTlmm_GetPinStatus(uint32 ClientHandle, DalDeviceHandle* h, uint32  gpio_number, DALGpioStatusType* status ) 
{
  DALGpioStatusType nStatus;
  DALResult nResult = TLMM_GetPinStatus( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, &nStatus );
  
  if ( DAL_SUCCESS == nResult )
  {
    if ( DalSys_CopyToUser(ClientHandle, status, &nStatus, sizeof(DALGpioStatusType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  return nResult;    
}
static DALResult 
TLMM_DalTlmm_SetInactiveConfig(uint32 ClientHandle, DalDeviceHandle* h, uint32 gpio_number, DALGpioSignalType gpio_config ) 
{
	return TLMM_SetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, gpio_config );
}
static DALResult 
TLMM_DalTlmm_GetInactiveConfig(uint32 ClientHandle, DalDeviceHandle* h, uint32 gpio_number, DALGpioSignalType* gpio_config ) 
{
  DALGpioSignalType nGpioConfig;
  DALResult nResult = TLMM_GetInactiveConfig( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, &nGpioConfig );
  
  if ( DAL_SUCCESS == nResult )
  {
    if ( DalSys_CopyToUser(ClientHandle, gpio_config, &nGpioConfig, sizeof(DALGpioSignalType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  
	return nResult;
}
static DALResult 
TLMM_DalTlmm_LockGpio(uint32 ClientHandle, DalDeviceHandle* h, DALGpioClientType client, uint32 gpio_number ) 
{
	return TLMM_LockGpio( ((DalTlmmHandle *)h)->pClientCtxt, client, gpio_number );
}
static DALResult 
TLMM_DalTlmm_UnlockGpio(uint32 ClientHandle, DalDeviceHandle* h, DALGpioClientType client, uint32 gpio_number ) 
{
	return TLMM_UnlockGpio( ((DalTlmmHandle *)h)->pClientCtxt, client, gpio_number );
}
static DALResult 
TLMM_DalTlmm_IsGpioLocked(uint32 ClientHandle, DalDeviceHandle* h, uint32 gpio_number, DALGpioLockType* lock ) 
{
	return TLMM_IsGpioLocked( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, lock );
}
static DALResult 
TLMM_DalTlmm_GetOutput(uint32 ClientHandle, DalDeviceHandle* h, uint32 gpio_number, DALGpioValueType* value ) 
{
  DALGpioValueType nValue;
  DALResult nResult = TLMM_GetOutput( ((DalTlmmHandle *)h)->pClientCtxt, gpio_number, &nValue );
  
  if ( DAL_SUCCESS == nResult )
  {
    if ( DalSys_CopyToUser(ClientHandle, value, &nValue, sizeof(DALGpioValueType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
	return nResult;
}
static DALResult 
TLMM_DalTlmm_GetGpioId(uint32 ClientHandle, DalDeviceHandle* h, const char* pszGpioStr, uint32 nStrLen, DALGpioIdType* pnGpioId ) 
{
  DALResult nResult = DAL_ERROR;
  const char* pszPinName = NULL;
  DALGpioIdType nGpioId;
  
  if ( nStrLen > TLMM_MAX_STRING_LENGTH )
  {
    return DAL_ERROR;
  }
  
  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return TLMM_GetGpioId( ((DalTlmmHandle *)h)->pClientCtxt, pszGpioStr, nStrLen, pnGpioId );
  }
  
  if ( pszGpioStr != NULL )
  {
    if (DALSYS_Malloc(nStrLen, (void **)&pszPinName) != DAL_SUCCESS)
    {
      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }

    if (DalSys_CopyFromUser(ClientHandle, (void*)pszPinName, (void*)pszGpioStr, nStrLen))
    {
      DALSYS_Free((void*)pszPinName);
      return DAL_ERROR_INVALID_ADDRESS;
    }
    
    nResult = TLMM_GetGpioId( ((DalTlmmHandle *)h)->pClientCtxt, pszPinName, nStrLen, &nGpioId );
    
    if ( nResult == DAL_SUCCESS )
    {
      if (DalSys_CopyToUser(ClientHandle, pnGpioId, &nGpioId, sizeof(DALGpioIdType)))
      {
        DALSYS_Free((void*)pszPinName);
        return DAL_ERROR_INVALID_ADDRESS;
      }     
    }
    DALSYS_Free((void*)pszPinName);
  }
	return nResult;
}
static DALResult 
TLMM_DalTlmm_ConfigGpioId(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId, DalTlmm_GpioConfigIdType* pUserSettings ) 
{
  DalTlmm_GpioConfigIdType UserSettings;

  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_ConfigGpioId( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId, pUserSettings );
  }
	
  if ( DalSys_CopyFromUser(ClientHandle, &UserSettings, pUserSettings, sizeof(DalTlmm_GpioConfigIdType)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  
  return TLMM_ConfigGpioId( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId, &UserSettings );

}
static DALResult 
TLMM_DalTlmm_ConfigGpioIdInactive(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId ) 
{
	return TLMM_ConfigGpioIdInactive( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId );
}
static DALResult 
TLMM_DalTlmm_GpioIdOut(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId, DALGpioValueType eValue) 
{
	return TLMM_GpioIdOut(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eValue);
}
static DALResult 
TLMM_DalTlmm_GpioIdIn(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId, DALGpioValueType *eValue) 
{
  DALGpioValueType nValue;
  DALResult nResult;
  
  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_GpioIdIn( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eValue );
  }
	
  nResult = TLMM_GpioIdIn( ((DalTlmmHandle *)h)->pClientCtxt, nGpioId, &nValue );
  
  if ( nResult == DAL_SUCCESS )
  {
    if ( DalSys_CopyToUser(ClientHandle, eValue, &nValue, sizeof(DALGpioValueType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  return nResult;
}
static DALResult 
TLMM_DalTlmm_ReleaseGpioId(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId) 
{
	return TLMM_ReleaseGpioId(((DalTlmmHandle *)h)->pClientCtxt, nGpioId);
}
static DALResult 
TLMM_DalTlmm_SelectGpioIdMode(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId,  DalGpioModeType eMode, DalTlmm_GpioConfigIdType* pUserSettings) 
{
  DalTlmm_GpioConfigIdType UserSettings;
  
  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_SelectGpioIdMode(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eMode, pUserSettings);
  }
  
  if ( DalSys_CopyFromUser(ClientHandle, &UserSettings, pUserSettings, sizeof(DalTlmm_GpioConfigIdType)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  return TLMM_SelectGpioIdMode(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, eMode, &UserSettings);
}
static DALResult 
TLMM_DalTlmm_GetGpioIdSettings(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId, DalTlmm_GpioIdSettingsType* pGpioSettings) 
{
  DALResult nResult;
  DalTlmm_GpioIdSettingsType GpioSettings;
  
  if ( ClientHandle == DALDEVICE_LOCAL_CLIENTID )
  {
    return TLMM_GetGpioIdSettings(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, pGpioSettings);
  }

  nResult = TLMM_GetGpioIdSettings(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, &GpioSettings);
  
  if ( nResult == DAL_SUCCESS )
  {
    if ( DalSys_CopyToUser(ClientHandle, pGpioSettings, &GpioSettings, sizeof(DalTlmm_GpioIdSettingsType)) )
    {
      return DAL_ERROR_INVALID_ADDRESS;
    }
  }
  return nResult;
}
static DALResult 
TLMM_DalTlmm_ConfigGpioIdModeIndexed(uint32 ClientHandle, DalDeviceHandle* h, DALGpioIdType nGpioId, uint32 nMode) 
{
	return TLMM_ConfigGpioIdModeIndexed(((DalTlmmHandle *)h)->pClientCtxt, nGpioId, nMode);
}

static void 
TLMM_InitInterface(TLMMClientCtxt* pclientCtxt)
{
    static const DalTlmm vtbl =  {
       {
          {.AttachEx = TLMM_DalTlmm_AttachEx},
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
        TLMM_DalTlmm_GetGpioNumber,
        TLMM_DalTlmm_GpioIn,
        TLMM_DalTlmm_GpioOut,
        TLMM_DalTlmm_GpioOutGroup,
        TLMM_DalTlmm_SetPort,
        TLMM_DalTlmm_GetCurrentConfig,
        TLMM_DalTlmm_GetPinStatus,
        TLMM_DalTlmm_SetInactiveConfig,
        TLMM_DalTlmm_GetInactiveConfig,
        TLMM_DalTlmm_LockGpio,
        TLMM_DalTlmm_UnlockGpio,
        TLMM_DalTlmm_IsGpioLocked,
        TLMM_DalTlmm_GetOutput,
        TLMM_DalTlmm_GetGpioId,
        TLMM_DalTlmm_ConfigGpioId,
        TLMM_DalTlmm_ConfigGpioIdInactive,
        TLMM_DalTlmm_GpioIdOut,
        TLMM_DalTlmm_GpioIdIn,
        TLMM_DalTlmm_ReleaseGpioId,
        TLMM_DalTlmm_SelectGpioIdMode,
        TLMM_DalTlmm_GetGpioIdSettings,
        TLMM_DalTlmm_ConfigGpioIdModeIndexed
    };  

	/*--------------------------------------------------------------------------
	Depending upon client type setup the vtables (entry points)
	--------------------------------------------------------------------------*/
    pclientCtxt->DalTlmmHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
    pclientCtxt->DalTlmmHandle.pVtbl  = &vtbl;

    /*------------------------------------------------------------------------
      The dwVtblen is the total number of API pointers in the VTbl.
    ------------------------------------------------------------------------*/
    pclientCtxt->DalTlmmHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));
    pclientCtxt->DalTlmmHandle.pClientCtxt = pclientCtxt;

}

DALResult 
TLMM_DalTlmm_AttachEx(const char *pszArg,
                      DALDEVICEID DeviceId, 
                      DalDeviceHandle **phDalDevice,
                      int nClientId)
{               
    DALResult nErr;
    uint32 nIdx;

    TLMMClientCtxt *pClientCtxt = NULL; // &ClientCtxt;

    for (nIdx = 0; nIdx < TLMM_NUM_CLIENT_CTXT; nIdx++)
    {
      if ((TLMMClients[nIdx].ClientInfo.nClientId == nClientId) &&
          (TLMMClients[nIdx].ClientInfo.bInUse == TRUE))
      {
        pClientCtxt = &TLMMClients[nIdx];
        break;
      }
    }

    if (pClientCtxt == NULL)
    {
      for (nIdx = 0; nIdx < TLMM_NUM_CLIENT_CTXT; nIdx++)
      {
        if (TLMMClients[nIdx].ClientInfo.bInUse == FALSE)
        {
          pClientCtxt = &TLMMClients[nIdx];
          pClientCtxt->ClientInfo.nIdx = nIdx;
          pClientCtxt->ClientInfo.nClientId = nClientId;
          pClientCtxt->ClientInfo.bInUse = TRUE;
          break;
        }
      }
    }

    if (pClientCtxt == NULL)
    {
      return DAL_ERROR;
    }

    *phDalDevice = NULL;

    nErr = DALFW_AttachToDevice(DALDEVICEID_TLMM,(DALDrvCtxt *)&TLMM_DriverCtxt,
		                        (DALClientCtxt *)pClientCtxt);
    if (DAL_SUCCESS == nErr)
    {
      TLMM_InitInterface(pClientCtxt);
      TLMM_DalTlmm_AddRef(&(pClientCtxt->DalTlmmHandle));
      *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalTlmmHandle);
      pClientCtxt->pDrvCtxt = &TLMM_DriverCtxt;
    }

    return nErr;
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

void TLMM_ClearContextData(uint32 nContextIdx)
{
  if (nContextIdx < TLMM_NUM_CLIENT_CTXT)
  {
    TLMMClients[nContextIdx].ClientInfo.nClientId = 0;
    TLMMClients[nContextIdx].ClientInfo.nIdx = 0;
    TLMMClients[nContextIdx].ClientInfo.bInUse = FALSE;
  }
}

