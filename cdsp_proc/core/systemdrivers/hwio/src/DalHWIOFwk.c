/*==============================================================================

FILE:      DalHWIOFwk.c

DESCRIPTION: This file implements a HWIO DeviceDriver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

    Copyright (c) 2008-2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DALSys.h"
#include "DDIHWIO.h"
#include "DalHWIO.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "string.h" //for memset warning
#include "stringl.h"


DALResult 
HWIO_DalHWIO_Attach(const char *, DALDEVICEID,DalDeviceHandle **);
  
static uint32 
HWIO_DalHWIO_AddRef(DalHWIOHandle* h)
{
    return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

static uint32 
HWIO_DalHWIO_Detach(uint32 ClientId, DalDeviceHandle* h)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult 
HWIO_DalHWIO_Init(uint32 ClientId, DalDeviceHandle *h)
{  
  HWIOClientCtxt *pClientCtxt = (HWIOClientCtxt *)(h->pClientCtxt);
  DALSYS_GetDALPropertyHandle(
    pClientCtxt->pHWIODevCtxt->DevId, pClientCtxt->pHWIODevCtxt->hProp);
  return HWIO_DeviceInit(h->pClientCtxt);
}

static DALResult 
HWIO_DalHWIO_DeInit(uint32 ClientId, DalDeviceHandle *h)
{
  return HWIO_DeviceDeInit(h->pClientCtxt);
}

static DALResult 
HWIO_DalHWIO_PowerEvent(uint32 ClientId, DalDeviceHandle *h, DalPowerCmd PowerCmd, 
                                DalPowerDomain PowerDomain )
{
  return HWIO_PowerEvent(h->pClientCtxt,PowerCmd,PowerDomain);
}

static DALResult 
HWIO_DalHWIO_Open(uint32 ClientId, DalDeviceHandle* h, uint32 mode)
{
  return HWIO_Open(h->pClientCtxt,mode);
}

static DALResult 
HWIO_DalHWIO_Close(uint32 ClientId, DalDeviceHandle* h)
{
    return HWIO_Close(h->pClientCtxt);
}

static DALResult 
HWIO_DalHWIO_Info(uint32 ClientId, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  DALResult nResult;
  DalDeviceInfo sInfo;

  if( DalSys_CopyFromUser(ClientId, &sInfo, info, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  sInfo.Version = DALHWIO_INTERFACE_VERSION;
  nResult = HWIO_Info(h->pClientCtxt,&sInfo,sizeof(sInfo));

  if ( nResult != DAL_SUCCESS )
  {
    return nResult;
  }

  if( DalSys_CopyToUser(ClientId, info, &sInfo, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}

static DALResult 
HWIO_DalHWIO_SysRequest(uint32 ClientId, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen, 
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
  return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalHWIO Interface. 
------------------------------------------------------------------------------*/

static DALResult 
HWIO_DalHWIO_MapRegion( uint32 ClientId, DalDeviceHandle * h, const char * szBase, uint8 ** ppAddress) 
{
  uint32 nMaxStrSize;
  uint8 *pReadBuffer = NULL, *pReturnAddress = NULL;
  DALResult nResult;

  /*
   * Check if it's the local client.  If so, just call directly into driver and return quickly.
   */
  if ( ClientId == DALDEVICE_LOCAL_CLIENTID )
  {
  return HWIO_MapRegion(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt, szBase,
    ppAddress);
  }

  /*
   * Get max string size
   */
  nMaxStrSize = HWIO_GetMaxStrSize(((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt);

  /*
   * Malloc read buffer to read string into
   */
  if ( DALSYS_Malloc(nMaxStrSize, (void **)&pReadBuffer) != DAL_SUCCESS )
  {
    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }

  /*
   * Copy string
   */
  if (DalSys_CopyFromUser(ClientId, pReadBuffer, szBase, nMaxStrSize))
  {
    DALSYS_Free(pReadBuffer);
    return DAL_ERROR_INVALID_ADDRESS;
  }

  /*
   * Verify client supplied string
   */
  if( strnlen((char*)pReadBuffer, nMaxStrSize) == nMaxStrSize )
  {
    DALSYS_Free(pReadBuffer);
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  /*
   * Call internal function
   */
  nResult = HWIO_MapRegion(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt, (char *)pReadBuffer,
    &pReturnAddress);

  /*
   * Free up memory
   */
  DALSYS_Free(pReadBuffer);

  /*
   * Copy return address
   */
  if ( DalSys_CopyToUser(ClientId, ppAddress, &pReturnAddress, sizeof(uint8**)) )
  {
    if ( nResult == DAL_SUCCESS )
    {
      /*
       * If it was successful, unmap it
       */
      (void) HWIO_UnMapRegion(
         ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt, pReturnAddress);
    }
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}

static DALResult 
HWIO_DalHWIO_MapRegionByAddress(uint32 ClientId,  DalDeviceHandle * h, uint8 *pPhysAddress, uint8 ** ppAddress)
{
  uint8* pReturnAddress = NULL;
  DALResult nResult;

  /*
   * Check if it's the local client.  If so, just call directly into driver and return quickly.
   */
  if ( ClientId == DALDEVICE_LOCAL_CLIENTID )
  {
  return HWIO_MapRegionByAddress(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt,
    pPhysAddress, ppAddress);
  }

  nResult = HWIO_MapRegionByAddress(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt,
    pPhysAddress, &pReturnAddress);

  /*
   * Copy return address
   */
  if ( DalSys_CopyToUser(ClientId, ppAddress, &pReturnAddress, sizeof(uint8*)) )
  {
    if ( nResult == DAL_SUCCESS )
    {
      /*
       * If it was successful, unmap it
       */
      (void) HWIO_UnMapRegion(
         ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt, pReturnAddress);
    }
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return nResult;
}

static DALResult 
HWIO_DalHWIO_UnMapRegion(uint32 ClientId,  DalDeviceHandle * h, uint8 * pAddress) 
{
  return HWIO_UnMapRegion(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt,
    pAddress);
}

static DALResult 
HWIO_DalHWIO_MapAllRegions(uint32 ClientId,  DalDeviceHandle * h ) 
{
  return HWIO_MapAllRegions(
    ((HWIOClientCtxt *)(h->pClientCtxt))->pHWIODevCtxt->pHWIODrvCtxt);
}

static void HWIO_InitInterface
(
  HWIOClientCtxt *pclientCtxt
)
{
  static const DalHWIO vtbl =
  {
    {
      HWIO_DalHWIO_Attach,
      HWIO_DalHWIO_Detach,
      HWIO_DalHWIO_Init,
      HWIO_DalHWIO_DeInit,
      HWIO_DalHWIO_Open,
      HWIO_DalHWIO_Close,
      HWIO_DalHWIO_Info,
      HWIO_DalHWIO_PowerEvent,
      HWIO_DalHWIO_SysRequest
    },
    HWIO_DalHWIO_MapRegion,
    HWIO_DalHWIO_UnMapRegion,
    HWIO_DalHWIO_MapRegionByAddress,
    HWIO_DalHWIO_MapAllRegions
  };

  pclientCtxt->DalHWIOHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalHWIOHandle.pVtbl  = &vtbl;
  pclientCtxt->DalHWIOHandle.dwVtblen = sizeof(vtbl)/sizeof(void(*)(void));
  pclientCtxt->DalHWIOHandle.pClientCtxt = pclientCtxt;
}

static HWIODrvCtxt HWIO_drvCtxt =
{
  {
    HWIO_DriverInit,
    HWIO_DriverDeInit
  },
  1,
  sizeof(HWIODevCtxt)
};


static DALDEVICEID DalHWIO_DeviceId[1] = { DALDEVICEID_HWIO };

const DALREG_DriverInfo DALHWIO_DriverInfo = 
{
  HWIO_DalHWIO_Attach,
  1,
  DalHWIO_DeviceId
};



DALResult 
HWIO_DalHWIO_Attach
(
  const char *pszArg,
  DALDEVICEID DeviceId, 
  DalDeviceHandle **phDalDevice
)
{               
  DALResult nErr;
  static HWIOClientCtxt clientCtxt;

  HWIOClientCtxt *pclientCtxt = &clientCtxt;

  *phDalDevice = NULL;

  nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&HWIO_drvCtxt,
                              (DALClientCtxt *)pclientCtxt);
  if (DAL_SUCCESS == nErr)
  {
    HWIO_InitInterface(pclientCtxt);
    HWIO_DalHWIO_AddRef(&(pclientCtxt->DalHWIOHandle));
    *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->DalHWIOHandle);
  }
  return nErr;
}

