/*==============================================================================

FILE:      GPIOMgr.c

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

  Copyright © 2015 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DDIGPIOMgr.h"
#include "GPIOMgr.h"
#include "HALgpioint.h"
#include "string.h" //for memset warning

/*
 * Maintain pointer for debugability
 */
static volatile GPIOMgrDevCtxt *GPIOMgrData;

/*------------------------------------------------------------------------------
Declaring a "GPIOMgr" Driver,Device and Client Context
------------------------------------------------------------------------------*/
#define GPIOMGR_VERIFY(h) if( h == NULL ) return DAL_ERROR

#define GPIOMGR_GET_CLIENT_CTXT(h) ((GPIOMgrIFHandle *)h)->pClientCtxt

#define GPIOMGR_GET_DEVICE_CTXT(h) h->pGPIOMgrDevCtxt

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/
DALResult
GPIOMgr_DeviceAttach(const char *, DALDEVICEID,DalDeviceHandle **);

/*------------------------------------------------------------------------------
Info file
------------------------------------------------------------------------------*/
const DALREG_DriverInfo
DALGPIOMgr_DriverInfo = { GPIOMgr_DeviceAttach, 0, NULL};

/*------------------------------------------------------------------------------
Driver functions
------------------------------------------------------------------------------*/

/* =========================================================================
**  Function : GPIOMgr_AddRef
** =========================================================================*/
/**
  Adds a device reference to the Dal framework.

  @param[in] h Pointer to the Dal Interface Handle. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static uint32
GPIOMgr_AddRef(GPIOMgrIFHandle* h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/

/* =========================================================================
**  Function : GPIOMgr_DriverInit
** =========================================================================*/
/**
  Initializes the driver context.

  @param[in] pdrvCtxt Pointer to the Dal Driver context. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

DALResult
GPIOMgr_DriverInit(GPIOMgrDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}

/* =========================================================================
**  Function : GPIOMgr_DriverDeInit
** =========================================================================*/
/**
  De-initializes the driver context.

  @param[in] pdrvCtxt Pointer to the Dal Driver context. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

DALResult
GPIOMgr_DriverDeInit(GPIOMgrDrvCtxt *pdrvCtxt)
{
   return DAL_SUCCESS;
}


/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/

/* =========================================================================
**  Function : GPIOMgr_DeviceDetach
** =========================================================================*/
/**
  Detaches from the GPIOMgr device.

  @param[in] h Handle pointer to the dal device. 
  @return
  uint32 - Number of devices remaining. 
  @dependencies
  None.
*/

static uint32
GPIOMgr_DeviceDetach(uint32 ClientHandle, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   if(0 == dwref)  {
      //Release the client ctxt;
      DALSYS_Free(h->pClientCtxt);
   }
   return dwref;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceInit
** =========================================================================*/
/**
  Initializes the Device context for the Dal Handle.

  @param[in]  h  The DAL Device handle for GPIOMgr.
  @return
  DAL_SUCCESS -- is returned if the device is correctly initialized. 
  DAL_ERROR -- is returned if there was an error initializing the device. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   GPIOMgrDevCtxt *dCtxt;
   GPIOMgrClientCtxt *cCtxt;
   DALResult eResult = DAL_ERROR;
   DALSYS_PROPERTY_HANDLE_DECLARE(hGPIOMgrProps); 
   DALSYSPropertyVar sProp;  
   uint32 nIter;

   if ( h == NULL )
   {
     return DAL_ERROR;
   }

   cCtxt = ((GPIOMgrIFHandle *)h)->pClientCtxt;

   if ( cCtxt == NULL ) 
   {
     return DAL_ERROR;
   }

   dCtxt = cCtxt->pGPIOMgrDevCtxt; 

   if ( dCtxt == NULL ) 
   {
     return DAL_ERROR;
   }

   if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
                                        &(dCtxt->hMutex), &(dCtxt->hSyncObj)) )
   {
     return DAL_ERROR;
   }

   if ( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr( "GPIOManager",
                                                        hGPIOMgrProps ) )
   {
     return DAL_ERROR;
   }

   (void)DALSYS_SyncEnter(dCtxt->hMutex);

   if ( dCtxt->pState == NULL )
   {
     eResult = DALSYS_GetPropertyValue(hGPIOMgrProps,
                                       "GPIOMGR_NUM_GPIOS", 0,
                                       &sProp); 
     if ( eResult == DAL_SUCCESS )
     {
       dCtxt->nMaxGpios = sProp.Val.dwVal;

        eResult = DALSYS_Malloc( ( dCtxt->nMaxGpios * sizeof( GPIOMgr_StateType ) ), 
                                 ( void** )&dCtxt->pState );

        if ( eResult == DAL_SUCCESS && dCtxt->pState != NULL )
        {
          for ( nIter = 0; nIter < dCtxt->nMaxGpios; nIter++ ) 
          {
            dCtxt->pState[nIter].nPD    = GPIOMGR_PD_NONE;
            dCtxt->pState[nIter].nFlags = 0;
            dCtxt->pState[nIter].nDirConnID = (uint8)HAL_GPIOINT_SUMMARY;
            dCtxt->pState[nIter].nTargetProc = GPIOMGR_DEVICE_NONE;
          }
        }
        else
        {
          (void)DALSYS_SyncLeave(dCtxt->hMutex);
          return DAL_ERROR;
        }

     }
     else
     {
       (void)DALSYS_SyncLeave(dCtxt->hMutex);
       return DAL_ERROR;
     }

   }

   eResult = DALSYS_GetPropertyValue(hGPIOMgrProps,
                                     "GPIOMGR_PD_CONFIG_MAP", 0,
                                     &sProp); 

   if ( eResult == DAL_SUCCESS )
   {
       dCtxt->pPDMap = (GPIOMgrConfigPDType *)sProp.Val.pStruct;
       dCtxt->nPDMapSize = 0;
       while ( !( dCtxt->pPDMap[dCtxt->nPDMapSize].nTargetProc == GPIOMGR_DEVICE_NONE ) )
       {
           dCtxt->nPDMapSize++;
       }

   }

   eResult = DALSYS_GetPropertyValue(hGPIOMgrProps,
                                     "GPIOMGR_DIRCONN_CONFIG_MAP", 0,
                                     &sProp); 

   if ( eResult == DAL_SUCCESS )
   {
       dCtxt->pDirConnMap = (GPIOMgrConfigMapType *)sProp.Val.pStruct;
       dCtxt->nDirConnMapSize = 0;
       while ( !( dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nDirConnID     == 0 &&
                  dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nInterruptID   == 0 &&
                  dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO          == 0 &&
                  dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nReservedPD    == 0 ) )
       {
           if ( dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO != GPIOMGR_UNUSED_GPIO &&
                dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO < dCtxt->nMaxGpios )
           {
               dCtxt->pState[dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO].nFlags     |= GPIOMGR_FLAG_RESERVED;
               dCtxt->pState[dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO].nPD         = (uint8)dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nReservedPD;
               dCtxt->pState[dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO].nDirConnID  = dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nDirConnID;
               dCtxt->pState[dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nGPIO].nTargetProc = dCtxt->pPDMap[dCtxt->pDirConnMap[dCtxt->nDirConnMapSize].nReservedPD].nTargetProc;
           }

           dCtxt->nDirConnMapSize++;
       }
   }

   GPIOMgrData = dCtxt;

   (void)DALSYS_SyncLeave(dCtxt->hMutex);

   return eResult;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceDeInit
** =========================================================================*/
/**
  De-initializes the Device context from the Dal Handle.

  @param[in]  h  The DAL Device handle for GPIOMgr.
  @return
  DAL_SUCCESS -- Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

/* =========================================================================
**  Function : GPIOMgr_DevicePowerEvent
** =========================================================================*/
/**
  Callback for power events.

  @param[in] h            Pointer to the Dal device handle. 
  @param[in] PowerCmd     Event enumeration of the power event that occurs. 
  @param[in] PowerDomain  Enumeration of the power domain in which the event occurs. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DevicePowerEvent(uint32 ClientHandle, DalDeviceHandle* h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

/* =========================================================================
**  Function : GPIOMgr_DeviceOpen
** =========================================================================*/
/**
  Opens the device in a specific mode.

  @param[in] h      Pointer to the Dal device handle. 
  @param[in] mode   Mode to open device. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceOpen(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceClose
** =========================================================================*/
/**
  Closes the device.

  @param[in] h      Pointer to the Dal device handle. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceClose(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceInfo
** =========================================================================*/
/**
  Returns version info of the Dal Device.

  @param[in]  h         Pointer to the Dal device handle. 
  @param[out] info      Pointer to the info structure. 
  @param[in]  infoSize  Size of the info structure passed in. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceInfo(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* info, uint32 infoSize)
{
  DalDeviceInfo sInfo;

  if( DalSys_CopyFromUser(ClientHandle, &sInfo, info, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  sInfo.Version = GPIOMGR_INTERFACE_VERSION;

  if( DalSys_CopyToUser(ClientHandle, info, &sInfo, sizeof(sInfo)) )
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceSysRequest
** =========================================================================*/
/**
  Unused function.

  @param[in]  h         Pointer to the Dal device handle. 
  @return
  DAL_SUCCESS - Always. 
  @dependencies
  None.
*/

static DALResult
GPIOMgr_DeviceSysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in GPIOMgrIF Interface.
------------------------------------------------------------------------------*/

/* =========================================================================
**  Function : GPIOMgr_InitInterface
** =========================================================================*/
/**
  Initializes the Vector Table of the Dal device.

  @param[in]  pclientCtxt The client context pointer of the GPIOMgr DAL.
  @return
  None. 
  @dependencies
  None.
*/

static void
GPIOMgr_InitInterface(GPIOMgrClientCtxt* pclientCtxt)
{
   static const GPIOMgrIF vtbl =
   {
      {
         GPIOMgr_DeviceAttach,
         GPIOMgr_DeviceDetach,
         GPIOMgr_DeviceInit,
         GPIOMgr_DeviceDeInit,
         GPIOMgr_DeviceOpen,
         GPIOMgr_DeviceClose,
         GPIOMgr_DeviceInfo,
         GPIOMgr_DevicePowerEvent,
         GPIOMgr_DeviceSysRequest
      },
      GPIOMgr_LockGPIO,
      GPIOMgr_ReleaseGPIO,
      GPIOMgr_GetGPIOCurrentPD,
      GPIOMgr_GetDirectConnectGPIO,
      GPIOMgr_ReleaseDirectConnectGPIO,
      GPIOMgr_GetDirectConnectInterruptID
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->GPIOMgrIFHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->GPIOMgrIFHandle.pVtbl  = &vtbl;
   pclientCtxt->GPIOMgrIFHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));
   pclientCtxt->GPIOMgrIFHandle.pClientCtxt = pclientCtxt;
}

/* =========================================================================
**  Function : GPIOMgr_DeviceAttach
** =========================================================================*/
/**
  Attaches to the GPIOMgr device.

  @param[in]  pszArg      Null terminated string of the GPIOMgr device string.
  @param[in]  DeviceId    Unused. 
  @param[out] phDalDevice Handle pointer to the dal device. 
  @return
  DAL_SUCCESS -- is returned if the device is correctly attached. 
  DAL_ERROR -- is returned if the device is not found. 
  @dependencies
  None.
*/

DALResult
GPIOMgr_DeviceAttach(const char *pszArg, DALDEVICEID DeviceId,
                            DalDeviceHandle **phDalDevice)
{
   DALResult nErr;
   static GPIOMgrDrvCtxt drvCtxt = {{GPIOMgr_DriverInit,
                                      GPIOMgr_DriverDeInit
                                     },1,
                                     sizeof(GPIOMgrDevCtxt)};
   GPIOMgrClientCtxt *pclientCtxt = NULL;

   nErr = DALSYS_Malloc(sizeof(GPIOMgrClientCtxt),(void **)&pclientCtxt);

   if ( nErr != DAL_SUCCESS ) {
      return DAL_ERROR;
   }

   DALSYS_memset(pclientCtxt,0,sizeof(GPIOMgrClientCtxt));

   *phDalDevice = NULL;

   if (DAL_SUCCESS == nErr) {
      nErr = DALFW_AttachToStringDevice(pszArg,(DALDrvCtxt *)&drvCtxt,
                                           (DALClientCtxt *)pclientCtxt);
      if (DAL_SUCCESS == nErr)
      {
         GPIOMgr_InitInterface(pclientCtxt);
         GPIOMgr_AddRef(&(pclientCtxt->GPIOMgrIFHandle));
         *phDalDevice = (DalDeviceHandle *)&(pclientCtxt->GPIOMgrIFHandle);
      }
   }
   return nErr;
}


/* =========================================================================
**  Function : GPIOMgr_GetDevCtxt
** =========================================================================*/
/**
  Gets the Device context from the Dal Handle.

  @param[in]  h         The client context pointer of the GPIOMgr DAL.
  @param[out] dCtxtOut  Device context pointer. 
  @return
  DAL_SUCCESS -- is returned if the device context pointer is correctly returned. 
  DAL_ERROR -- is returned if the device context pointer is not correctly returned. 
  @dependencies
  None.
*/

static DALResult GPIOMgr_GetDevCtxt(DalDeviceHandle * h, GPIOMgrDevCtxt **dCtxtOut) 
{
  GPIOMgrClientCtxt *cCtxt;
  GPIOMgrDevCtxt *dCtxt = NULL;

  GPIOMGR_VERIFY(h);
  GPIOMGR_VERIFY(dCtxtOut);

  cCtxt = GPIOMGR_GET_CLIENT_CTXT(h);
  GPIOMGR_VERIFY(cCtxt);

  dCtxt = GPIOMGR_GET_DEVICE_CTXT(cCtxt);
  GPIOMGR_VERIFY(dCtxt);

  *dCtxtOut = dCtxt;

  return DAL_SUCCESS;
}

static uint32 GPIOMgr_GetPDDevice(GPIOMgrDevCtxt *dCtxt, uint32 nPD)
{
    if ( nPD < dCtxt->nPDMapSize )
    {
        return dCtxt->pPDMap[nPD].nTargetProc;
    }
    return GPIOMGR_DEVICE_NONE;
}

/* =========================================================================
**  Function : GPIOMgr_LockGPIO
** =========================================================================*/
/**
  Locks a given GPIO.

  This function will attempt to lock a given GPIO to a given PD.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO pin number to be locked. 
  @param[in] nPD        The PD which the GPIO is to be locked. 
  @return
  DAL_SUCCESS -- is returned if the GPIO is successfully locked. 
  DAL_ERROR -- is returned if the GPIO is in use by another 
  client. 
  @dependencies
  None.
*/

DALResult GPIOMgr_LockGPIO(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO, uint32 nPD)
{
   GPIOMgrDevCtxt *dCtxt = NULL;
   DALResult eResult = DAL_ERROR;

   if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
   {
     return DAL_ERROR;
   }

   (void)DALSYS_SyncEnter(dCtxt->hMutex);

   if ( ( nGPIO < dCtxt->nMaxGpios ) &&
        ( dCtxt->pState != NULL ) &&
        ( !( dCtxt->pState[nGPIO].nFlags & GPIOMGR_FLAG_INUSE ) ) )
   {
     dCtxt->pState[nGPIO].nFlags |= GPIOMGR_FLAG_INUSE;
     dCtxt->pState[nGPIO].nPD = (uint8)nPD;
     eResult = DAL_SUCCESS;
   }

   (void)DALSYS_SyncLeave(dCtxt->hMutex);

   return eResult;
}


/* =========================================================================
**  Function : GPIOMgr_ReleaseGPIO
** =========================================================================*/
/**
  Unlocks a given GPIO.

  This function will attempt to unlock a given GPIO from a given PD.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO pin number to be unlocked. 
  @return
  DAL_SUCCESS -- is returned if the GPIO is successfully unlocked. 
  DAL_ERROR -- is returned if the GPIO is not locked prior to call. 
  @dependencies
  None.
*/

DALResult GPIOMgr_ReleaseGPIO(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO)
{
   GPIOMgrDevCtxt *dCtxt = NULL;
   DALResult eResult = DAL_ERROR;

   if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
   {
     return DAL_ERROR;
   }

   (void)DALSYS_SyncEnter(dCtxt->hMutex);

   if ( ( nGPIO < dCtxt->nMaxGpios ) &&
        ( dCtxt->pState != NULL ) &&
        ( dCtxt->pState[nGPIO].nFlags & GPIOMGR_FLAG_INUSE ) )
   {
     dCtxt->pState[nGPIO].nFlags &= ~GPIOMGR_FLAG_INUSE;
     dCtxt->pState[nGPIO].nPD = GPIOMGR_PD_NONE;
     eResult = DAL_SUCCESS;
   }

   (void)DALSYS_SyncLeave(dCtxt->hMutex);

   return eResult;
}


/* =========================================================================
**  Function : GPIOMgr_GetGPIOCurrentPD
** =========================================================================*/
/**
  Gets the current PD that the GPIO is locked to.

  @param[in]  _h         The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO      The GPIO pin number to fetch the current PD. 
  @param[out] pnPD       The PD that the GPIO is registered to. 
  @return
  DAL_SUCCESS -- is returned if PD successfully returned. 
  DAL_ERROR -- is returned if the GPIO is not currently in use. 
  @dependencies
  None.
*/

DALResult GPIOMgr_GetGPIOCurrentPD(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO, uint32* pnPD)
{
   GPIOMgrDevCtxt *dCtxt = NULL;
   DALResult eResult = DAL_ERROR;
   uint32 nPD;

   if ( pnPD == NULL )
   {
     return DAL_ERROR;
   }

   if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
   {
     return DAL_ERROR;
   }

   (void)DALSYS_SyncEnter(dCtxt->hMutex);

   if ( ( nGPIO < dCtxt->nMaxGpios ) &&
        ( dCtxt->pState != NULL ) &&
        ( dCtxt->pState[nGPIO].nFlags & GPIOMGR_FLAG_INUSE ) )
   {
     nPD = (uint32)dCtxt->pState[nGPIO].nPD;
     if ( DalSys_CopyToUser(ClientHandle, pnPD, &nPD, sizeof(nPD)) )
     {
       eResult = DAL_ERROR_INVALID_ADDRESS;
     }
     else
     {
       eResult = DAL_SUCCESS;
     }
   }

   (void)DALSYS_SyncLeave(dCtxt->hMutex);

   return eResult;
}

/* =========================================================================
**  Function : GPIOMgr_GetDirectConnectGPIO
** =========================================================================*/
/**
  Gets a free direct connect GPIO for the GPIO given.

  @param[in]  _h          The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO       The GPIO pin number to attach to the direct connect. 
  @param[out] pDirConnID  The direct connect index assigned to the GPIO. 
  @return
  DAL_SUCCESS -- is returned if a direct connect was assigned. 
  DAL_ERROR -- is returned if there are no more direct connects. 
  @dependencies
  GPIO must be locked.
*/

DALResult GPIOMgr_GetDirectConnectGPIO(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO, uint32* pDirConnID)
{
    GPIOMgrDevCtxt *dCtxt = NULL;
    DALResult eResult = DAL_ERROR;
    uint32 nDirConn = 0;
    uint32 nCurrPD;
    uint32 nPDDevice;
    uint32 nDirConnID;

    if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
    {
      return DAL_ERROR;
    }

    if ( dCtxt->pState == NULL ||
         nGPIO > dCtxt->nMaxGpios )
    {
      return DAL_ERROR;
    }

    nCurrPD = dCtxt->pState[nGPIO].nPD; 
    nPDDevice = GPIOMgr_GetPDDevice(dCtxt, nCurrPD);

    if ( pDirConnID == NULL ||
         nPDDevice >= GPIOMGR_DEVICE_NONE )
    {
      return DAL_ERROR;
    }

    (void)DALSYS_SyncEnter(dCtxt->hMutex);

    if ( ( nGPIO < dCtxt->nMaxGpios ) )
    {
        for ( nDirConn = 0; nDirConn < dCtxt->nDirConnMapSize; nDirConn++ )
        {
            /*
             * Make sure that the Direct connect is unused and that the PD is correct
             */
            if ( ( dCtxt->pDirConnMap[nDirConn].nGPIO == GPIOMGR_UNUSED_GPIO ) &&
                 ( dCtxt->pDirConnMap[nDirConn].nReservedPD == nCurrPD ) )
            {
                dCtxt->pDirConnMap[nDirConn].nGPIO = nGPIO;
                dCtxt->pState[nGPIO].nTargetProc = (uint8)nPDDevice;
                dCtxt->pState[nGPIO].nDirConnID = (uint8)dCtxt->pDirConnMap[nDirConn].nDirConnID;
                nDirConnID = (uint32)dCtxt->pDirConnMap[nDirConn].nDirConnID;
                if ( DalSys_CopyToUser(ClientHandle, pDirConnID, &nDirConnID, sizeof(nDirConnID)) )
                {
                  eResult = DAL_ERROR_INVALID_ADDRESS;
                }
                else
                {
                  eResult = DAL_SUCCESS;
                }
                break;
            }
        }
    }

    (void)DALSYS_SyncLeave(dCtxt->hMutex);

    return eResult;
}

/* =========================================================================
**  Function : GPIOMgr_ReleaseDirectConnectGPIO
** =========================================================================*/
/**
  Releases a direct connect GPIO for the GPIO given.  This GPIO will now
  be added back to the free direct connect pool.

  @param[in] _h         The client context pointer of the GPIOMgr DAL.
  @param[in] nGPIO      The GPIO currently assigned to the direct connect. 
  @param[in] nDirConnID The direct connect ID to be released. 
  @return
  DAL_SUCCESS -- is returned if GPIO successfully released from direct connect. 
  DAL_ERROR -- is returned if the GPIO was never in use by direct connect. 
  @dependencies
  Direct connect must be already assigned to this GPIO.
*/

DALResult GPIOMgr_ReleaseDirectConnectGPIO(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO, uint32 nDirConnID)
{
    GPIOMgrDevCtxt *dCtxt = NULL;
    DALResult eResult = DAL_ERROR;
    uint32 nDirConn = 0;

    if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
    {
      return DAL_ERROR;
    }

    (void)DALSYS_SyncEnter(dCtxt->hMutex);

    if ( ( nGPIO < dCtxt->nMaxGpios ) &&
         ( dCtxt->pState != NULL ) )
    {
        for ( nDirConn = 0; nDirConn < dCtxt->nDirConnMapSize; nDirConn++ )
        {
            if ( ( dCtxt->pDirConnMap[nDirConn].nGPIO == nGPIO ) &&
                 ( dCtxt->pDirConnMap[nDirConn].nDirConnID == nDirConnID ) )
            {
                dCtxt->pDirConnMap[nDirConn].nGPIO = GPIOMGR_UNUSED_GPIO;
                dCtxt->pState[nGPIO].nDirConnID = (uint8)HAL_GPIOINT_SUMMARY;
                dCtxt->pState[nGPIO].nTargetProc = GPIOMGR_DEVICE_NONE;
                eResult = DAL_SUCCESS;
                break;
            }
        }
    }

    (void)DALSYS_SyncLeave(dCtxt->hMutex);

    return eResult;
}

/* =========================================================================
**  Function : GPIOMgr_GetDirectConnectInterruptID
** =========================================================================*/
/**
  Gets the interrupt ID of a direct connect interrupt that is locked to a GPIO.

  @param[in]  _h         The client context pointer of the GPIOMgr DAL.
  @param[in]  nGPIO      The GPIO currently assigned to the direct connect. 
  @param[out] pDirConnID The direct connect interrupt ID. 
  @return
  DAL_SUCCESS -- is returned if direct connect interrupt ID successfully returned. 
  DAL_ERROR -- is returned if the GPIO is not assigned a direct connect ID. 
  @dependencies
  None.
*/

DALResult GPIOMgr_GetDirectConnectInterruptID(uint32 ClientHandle, DalDeviceHandle * h, uint32 nGPIO, uint32* pInterruptID)
{
    GPIOMgrDevCtxt *dCtxt = NULL;
    DALResult eResult = DAL_ERROR;
    uint32 nDirConn = 0;
    uint32 nInterruptID;

    if( DAL_SUCCESS != GPIOMgr_GetDevCtxt(h, &dCtxt) )
    {
      return DAL_ERROR;
    }

    (void)DALSYS_SyncEnter(dCtxt->hMutex);

    if ( ( nGPIO < dCtxt->nMaxGpios ) &&
         ( dCtxt->pState != NULL ) )
    {
        for ( nDirConn = 0; nDirConn < dCtxt->nDirConnMapSize; nDirConn++ )
        {
            if ( dCtxt->pDirConnMap[nDirConn].nGPIO == nGPIO )
            {
                nInterruptID = (uint32)dCtxt->pDirConnMap[nDirConn].nInterruptID;
                if( DalSys_CopyToUser(ClientHandle, pInterruptID, &nInterruptID, sizeof(nInterruptID)) )
                {
                  eResult = DAL_ERROR_INVALID_ADDRESS;
                }
                else
                {
                  eResult = DAL_SUCCESS;
                }
                break;
            }
        }
    }

    (void)DALSYS_SyncLeave(dCtxt->hMutex);

    return eResult;
}
