/*
===========================================================================
*/
/**
  @file ClockFwk.c

  DAL framework implementation file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/src/ClockFwk.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "ClockDriver.h"
#include "DDIHWIO.h"
#include "DDIPlatformInfo.h"
#include "DALPropDef.h"
#include "qurt_rmutex.h"
#include "stringl.h"


/*=========================================================================
      Externs
==========================================================================*/

extern ClockBSPType ClockBSP;


/*=========================================================================
      Macro Definitions
==========================================================================*/


/*
 * Take a DAL driver handle and dereference into the driver context.
 */
#define HANDLE2CTXT(h) \
  &((ClockDALClientCtxt *)(h->pClientCtxt))->pClockDevCtxt->pClockDrvCtxt->DrvCtxt


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Function Prototypes
==========================================================================*/

DALResult Clock_DeviceAttachEx(const char *, DALDEVICEID, DalDeviceHandle **, int);
static DALResult DALClock_DriverInit(ClockDALDrvCtxt *);
static DALResult DALClock_DriverDeInit(ClockDALDrvCtxt *);
static void Clock_ReleaseClientResources(ClockDALClientCtxt* pClientCtxt);


/*=========================================================================
      Data Declarations
==========================================================================*/

static DALDEVICEID DALClock_DeviceId[1] =
{
  DALDEVICEID_CLOCK
};

const DALREG_DriverInfo DALClock_DriverInfo =
{
  {.pfnDALNewFuncEx = Clock_DeviceAttachEx}, 1, DALClock_DeviceId
};

static ClockDALDrvCtxt Clock_DriverCtxt =
{
  {
    DALClock_DriverInit,
    DALClock_DriverDeInit
    },
  1,
  sizeof(ClockDALDevCtxt)
};

/*
 * Mutex for lite implementation.
 */
static qurt_mutex_t hClockLiteMutex = QURT_MUTEX_INIT;


/*
 * Static array of client contexts.  The number of client contexts in this
 * array must be sufficiently large to accommodate the number of s
 * supported on the subsystem.
 */
static ClockDALClientCtxt ClockClientCtxts[CLOCK_CTXT_TOTAL];


/*=========================================================================
      Functions
==========================================================================*/


static uint32
Clock_AddRef(DalClockHandle *h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

static uint32
Clock_DeviceDetach(uint32 ClientHandle, DalDeviceHandle* h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));

   if (dwref == 0)
   {
     /*
      * Release all allocated resources for this client context.
      */
     Clock_ReleaseClientResources(((DalClockHandle *)h)->pClientCtxt);
   }
   return dwref;
}

static DALResult
Clock_DeviceInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
Clock_DeviceDeInit(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
Clock_DevicePowerEvent(uint32 ClientHandle, DalDeviceHandle* h, DalPowerCmd PowerCmd,
                                DalPowerDomain PowerDomain )
{
   return DAL_SUCCESS;

}

static DALResult
Clock_DeviceOpen(uint32 ClientHandle, DalDeviceHandle* h, uint32 mode)
{
   return DAL_SUCCESS;
}

static DALResult
Clock_DeviceClose(uint32 ClientHandle, DalDeviceHandle* h)
{
   return DAL_SUCCESS;
}

static DALResult
Clock_DeviceInfo(uint32 ClientHandle, DalDeviceHandle* h, DalDeviceInfo* pInfo, uint32 infoSize)
{
  DalDeviceInfo Info;

  if (DalSys_CopyFromUser(ClientHandle, &Info, pInfo, sizeof(Info)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  Info.Version = DALCLOCK_INTERFACE_VERSION;
  if (DalSys_CopyToUser(ClientHandle, pInfo, &Info, sizeof(Info)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult
Clock_DeviceSysRequest(uint32 ClientHandle, DalDeviceHandle* h,DalSysReq ReqIdx,
                                const void* SrcBuf, uint32 SrcBufLen,
                                void* DestBuf, uint32 DestBufLen, uint32* DestBufLenReq)
{
   return DAL_SUCCESS;
}


static void DALClock_GetStubFlags
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult                    eResult;
  ClockPropertyValueType       PropVal;
  ClockStubType               *pClockStubConfig;
  DalPlatformInfoPlatformType  PlatformInfo;

  /*-----------------------------------------------------------------------*/
  /* Get the stub config.                                                  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockStubConfig", &PropVal);
  if (eResult == DAL_SUCCESS)
  {
    pClockStubConfig = (ClockStubType *)PropVal;
    PlatformInfo = DalPlatformInfo_Platform();

    if ((PlatformInfo == DALPLATFORMINFO_TYPE_RUMI) &&
        (pClockStubConfig->bRUMI == TRUE))
    {
      CLOCK_GLOBAL_FLAG_SET(STUB_API);

      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
        "CLOCK_GLOBAL_FLAG_STUB_API set - stubbing out on RUMI.");
    }
    if ((PlatformInfo == DALPLATFORMINFO_TYPE_VIRTIO) &&
        (pClockStubConfig->bVirtio == TRUE))
    {
      CLOCK_GLOBAL_FLAG_SET(STUB_API);

      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_INFO,
        "CLOCK_GLOBAL_FLAG_STUB_API set - stubbing out on Virtio.");
    }
  }
}


static DALResult DALClock_DriverInit
(
  ClockDALDrvCtxt *drvCtxt
)
{
  DALResult     eRes;
  ClockDrvCtxt *pDrvCtxt;

  /*-----------------------------------------------------------------------*/
  /* ENTER CRITICAL SECTION: Protect vs. lite implementation.              */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt = Clock_GetDrvCtxt();

  /*-----------------------------------------------------------------------*/
  /* Hookup BSP data.                                                      */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->pBSP = &ClockBSP;

  /*-----------------------------------------------------------------------*/
  /* Get our property handle.                                              */
  /*-----------------------------------------------------------------------*/

  eRes = DALSYS_GetDALPropertyHandle(
    DALDEVICEID_CLOCK, Clock_DriverCtxt.ClockDevCtxt[0].hProp);

  if (eRes != DAL_SUCCESS)
  {
    Clock_ReleaseDrvCtxt();
    return eRes;
  }

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.  This is done here since the common code may  */
  /* live in environments without the DAL framework.                       */
  /*-----------------------------------------------------------------------*/

  DAL_DeviceAttach(DALDEVICEID_HWIO, &Clock_DriverCtxt.phHWIO);

  /*-----------------------------------------------------------------------*/
  /* Get the stub flags.                                                   */
  /*-----------------------------------------------------------------------*/

  DALClock_GetStubFlags(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Perform driver initialization.                                        */
  /*-----------------------------------------------------------------------*/

  if (CLOCK_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    eRes = ClockStub_DriverInit(pDrvCtxt);
  }
  else
  {
    eRes = Clock_DriverInit(pDrvCtxt);
  }

  /*-----------------------------------------------------------------------*/
  /* Transfer reference counts from lite implementation to DAL.            */
  /*-----------------------------------------------------------------------*/

  eRes = Clock_TransferReferenceCount();
  if (eRes != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_TransferReferenceCount failed.");
  }

  /*-----------------------------------------------------------------------*/
  /* END CRITICAL SECTION: Protect vs. lite implementation.                */
  /*-----------------------------------------------------------------------*/

  Clock_ReleaseDrvCtxt();

  return eRes;
}

static DALResult DALClock_DriverDeInit(ClockDALDrvCtxt *drvCtxt)
{
  DALResult      eRes;
  ClockDrvCtxt  *pDrvCtxt = &drvCtxt->DrvCtxt;

  if (CLOCK_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    eRes = ClockStub_DriverDeInit(&drvCtxt->DrvCtxt);
  }
  else
  {
    eRes = Clock_DriverDeInit(&drvCtxt->DrvCtxt);
  }

  return eRes;
}

static DALResult DALClock_DupStringFromUser(uint32 ClientHandle, const char *szString, char **szDupString, size_t szStringSize)
{
  if (!szString)
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  if (DALSYS_Malloc(szStringSize, (void **)szDupString) != DAL_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_ERROR,
      "Unable to allocate %lu bytes for local string copy.", szStringSize);
    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }

  if (DalSys_CopyFromUser(ClientHandle, *szDupString, szString, szStringSize))
  {
    DALSYS_Free(*szDupString);
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}


typedef DALResult (*ClockGetIdFunc) (ClockDrvCtxt *, const char *,  ClockIdType *);

static DALResult DALClock_GetId(uint32 ClientHandle, DalDeviceHandle *h,  const char * szName, size_t szNameSize, ClockIdType * pnId, ClockGetIdFunc fGetId)
{
  char        *szNameCopy;
  DALResult    eResult;
  ClockIdType  nId;

  /*-----------------------------------------------------------------------*/
  /* Local client.                                                         */
  /*-----------------------------------------------------------------------*/

  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return fGetId(HANDLE2CTXT(h), szName, pnId);
  }

  /*-----------------------------------------------------------------------*/
  /* Remote client.                                                        */
  /*-----------------------------------------------------------------------*/

  eResult = DALClock_DupStringFromUser(ClientHandle, szName, &szNameCopy, szNameSize);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  eResult = fGetId(HANDLE2CTXT(h), szNameCopy, &nId);
  DALSYS_Free(szNameCopy);

  if (!pnId || DalSys_CopyToUser(ClientHandle, pnId, &nId, sizeof(nId)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}


static DALResult DALClock_GetClockId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szClock, size_t szClockSize, ClockIdType * pnId)
{
  return DALClock_GetId(ClientHandle, h, szClock, szClockSize, pnId, Clock_GetClockId);
}

static DALResult DALClock_EnableClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_EnableClock(((DalClockHandle *)h)->pClientCtxt, pClock);
}

static DALResult DALClock_EnableDFS(uint32 ClientHandle, DalDeviceHandle * h,  ClockIdType  nClock)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_EnableDFS(pDrvCtxt, pClock);
}

static DALResult DALClock_IsDFSEnabled(uint32 ClientHandle,  DalDeviceHandle * h,  ClockIdType  nClock,  boolean * pbIsEnabled)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult      eResult;
  boolean        bIsEnabled;

  eResult = Clock_IsDFSEnabled(pDrvCtxt, pClock, &bIsEnabled);
  if (!pbIsEnabled || DalSys_CopyToUser(ClientHandle, pbIsEnabled, &bIsEnabled, sizeof(bIsEnabled)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_DisableClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_DisableClock(((DalClockHandle *)h)->pClientCtxt, pClock);
}

static DALResult DALClock_SetClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq )
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult eResult;
  uint32    nResultFreq;

  if (pClock && pClock->pDomain && CLOCK_FLAG_IS_SET(pClock->pDomain, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  eResult = Clock_SetClockFrequency(pDrvCtxt, pClock, nFreq, eMatch, &nResultFreq);
  if (pnResultFreq && DalSys_CopyToUser(ClientHandle, pnResultFreq, &nResultFreq, sizeof(nResultFreq)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_SetClockDivider(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32  nDivider)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_SetClockDivider(pDrvCtxt, pClock, nDivider);
}

static DALResult DALClock_InvertClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean  bInvert)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_InvertClock(pDrvCtxt, pClock, bInvert);
}

static DALResult DALClock_ResetClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_ResetClock(pDrvCtxt, pClock, eReset);
}

static DALResult DALClock_ResetClock_STM(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_ResetClock_STM(pDrvCtxt, pClock, eReset);
}

static DALResult DALClock_ResetClockBranch(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_ResetClockBranch(pDrvCtxt, pClock, eReset);
}

static DALResult DALClock_IsClockEnabled(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean * pbIsEnabled)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult      eResult;
  boolean        bIsEnabled;

  eResult = Clock_IsClockEnabled(pDrvCtxt, pClock, &bIsEnabled);
  if (!pbIsEnabled || DalSys_CopyToUser(ClientHandle, pbIsEnabled, &bIsEnabled, sizeof(bIsEnabled)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_IsClockOn(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean * pbIsOn)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult      eResult;
  boolean        bIsOn;

  eResult = Clock_IsClockOn(pDrvCtxt, pClock, &bIsOn);
  if (!pbIsOn || DalSys_CopyToUser(ClientHandle, pbIsOn, &bIsOn, sizeof(bIsOn)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_GetClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult      eResult;
  uint32         nFrequencyHz;

  eResult = Clock_GetClockFrequency(pDrvCtxt, pClock, &nFrequencyHz);
  if (!pnFrequencyHz || DalSys_CopyToUser(ClientHandle, pnFrequencyHz, &nFrequencyHz, sizeof(nFrequencyHz)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_SelectExternalSource(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 nFreqHz,  uint32 nSource, uint32  nDiv2x,  uint32  nM,  uint32  nN,  uint32  n2D)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  return Clock_SelectExternalSource(pDrvCtxt, pClock, nFreqHz, nSource, nDiv2x, nM, nN, n2D);
}

static DALResult DALClock_CalcClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  DALResult      eResult;
  uint32         nFrequencyHz;

  eResult = Clock_CalcClockFrequency(pDrvCtxt, pClock, &nFrequencyHz);
  if (!pnFrequencyHz || DalSys_CopyToUser(ClientHandle, pnFrequencyHz, &nFrequencyHz, sizeof(nFrequencyHz)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_GetDebugMuxId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szDebugMux, size_t szDebugMuxSize, ClockDebugMuxIdType * pnId)
{
  return DALClock_GetId(ClientHandle, h, szDebugMux, szDebugMuxSize, pnId, Clock_GetDebugMuxId);
}

static DALResult DALClock_CalcFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockDebugMuxIdType nDebugMux, uint32 nMuxSel, uint32 *pnFrequencyHz)
{
  ClockDrvCtxt      *pDrvCtxt = HANDLE2CTXT(h);
  ClockDebugMuxType *pDebugMux = Clock_GetDebugMuxNode(pDrvCtxt, nDebugMux);
  DALResult          eResult;
  uint32             nFrequencyHz;

  eResult = Clock_CalcFrequency(pDrvCtxt, pDebugMux, nMuxSel, &nFrequencyHz);
  if (!pnFrequencyHz || DalSys_CopyToUser(ClientHandle, pnFrequencyHz, &nFrequencyHz, sizeof(nFrequencyHz)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_GetPowerDomainId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szPowerDomain, size_t szPowerDomainSize, ClockPowerDomainIdType * pnId)
{
  return DALClock_GetId(ClientHandle, h, szPowerDomain, szPowerDomainSize, pnId, Clock_GetPowerDomainId);
}

static DALResult DALClock_EnablePowerDomain(uint32 ClientHandle, DalDeviceHandle* h,  ClockPowerDomainIdType  nPowerDomainId)
{
  ClockDrvCtxt              *pDrvCtxt = HANDLE2CTXT(h);
  ClockPowerDomainNodeType  *pPowerDomain = Clock_GetPowerDomainNode(pDrvCtxt, nPowerDomainId);

  if (pPowerDomain && CLOCK_FLAG_IS_SET(pPowerDomain, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_EnablePowerDomain(((DalClockHandle *)h)->pClientCtxt, pPowerDomain);
}

static DALResult DALClock_DisablePowerDomain(uint32 ClientHandle, DalDeviceHandle* h,  ClockPowerDomainIdType  nPowerDomainId)
{
  ClockDrvCtxt              *pDrvCtxt = HANDLE2CTXT(h);
  ClockPowerDomainNodeType  *pPowerDomain = Clock_GetPowerDomainNode(pDrvCtxt, nPowerDomainId);

  if (pPowerDomain && CLOCK_FLAG_IS_SET(pPowerDomain, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_DisablePowerDomain(((DalClockHandle *)h)->pClientCtxt, pPowerDomain);
}

static DALResult DALClock_EnableDCVS(uint32 ClientHandle, DalDeviceHandle* h)
  { return Clock_EnableDCVS(HANDLE2CTXT(h)); }

static DALResult DALClock_LoadNV(uint32 ClientHandle, DalDeviceHandle* h)
  { return Clock_LoadNV(HANDLE2CTXT(h)); }

static DALResult DALClock_ProcessorSleep(uint32 ClientHandle, DalDeviceHandle* h,  ClockSleepModeType  eMode,  uint32  nFlags)
  { return Clock_ProcessorSleep(HANDLE2CTXT(h), eMode, nFlags); }

static DALResult DALClock_ProcessorRestore(uint32 ClientHandle, DalDeviceHandle* h,  ClockSleepModeType  eMode,  uint32  nFlags)
  { return Clock_ProcessorRestore(HANDLE2CTXT(h), eMode, nFlags); }

static DALResult DALClock_ConfigClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockConfigType  eConfig)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType *pClock = Clock_GetClockNode(pDrvCtxt, nClock);

  if (pClock && CLOCK_FLAG_IS_SET(pClock, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_ConfigClock(pDrvCtxt, pClock, eConfig);
}

static DALResult DALClock_SetUARTBaudClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock, ClockIdType nRefClock, uint32 nDI, uint32 nFI, uint32 *pnResultFreqHz)
  { return DAL_ERROR_NOT_SUPPORTED; }

static DALResult DALClock_LogState(uint32 ClientHandle, DalDeviceHandle* h,  void *pULog,  uint32 nFlags)
{
  if (ClientHandle != DALDEVICE_LOCAL_CLIENTID)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  return Clock_LogState(HANDLE2CTXT(h), (ULogHandle)pULog, nFlags);
}

static DALResult DALClock_GetSourceId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szSource, size_t szSourceSize, SourceIdType * pnId)
{
  return DALClock_GetId(ClientHandle, h, szSource, szSourceSize, pnId, Clock_GetSourceId);
}

static DALResult DALClock_EnableSource(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource)
{
  ClockDrvCtxt        *pDrvCtxt = HANDLE2CTXT(h);
  ClockSourceNodeType *pSource = Clock_GetSourceNode(pDrvCtxt, nSource);

  if (pSource && CLOCK_FLAG_IS_SET(pSource, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_EnableSource(((DalClockHandle *)h)->pClientCtxt, pSource);
}

static DALResult DALClock_DisableSource(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource)
{
  ClockDrvCtxt        *pDrvCtxt = HANDLE2CTXT(h);
  ClockSourceNodeType *pSource = Clock_GetSourceNode(pDrvCtxt, nSource);

  if (pSource && CLOCK_FLAG_IS_SET(pSource, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_DisableSource(((DalClockHandle *)h)->pClientCtxt, pSource);
}

static DALResult DALClock_SetSourceFrequency(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq )
{
  ClockDrvCtxt        *pDrvCtxt = HANDLE2CTXT(h);
  ClockSourceNodeType *pSource = Clock_GetSourceNode(pDrvCtxt, nSource);
  DALResult            eResult;
  uint32               nResultFreq;

  if (pSource && CLOCK_FLAG_IS_SET(pSource, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  eResult = Clock_SetSourceFrequency(pDrvCtxt, pSource, nFreq, eMatch, &nResultFreq);
  if (pnResultFreq && DalSys_CopyToUser(ClientHandle, pnResultFreq, &nResultFreq, sizeof(nResultFreq)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_GetSourceFrequency(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource,  uint32 * pnFrequencyHz)
{
  ClockDrvCtxt        *pDrvCtxt = HANDLE2CTXT(h);
  ClockSourceNodeType *pSource = Clock_GetSourceNode(pDrvCtxt, nSource);
  DALResult            eResult;
  uint32               nFrequencyHz;

  eResult = Clock_GetSourceFrequency(pDrvCtxt, pSource, &nFrequencyHz);
  if (!pnFrequencyHz || DalSys_CopyToUser(ClientHandle, pnFrequencyHz, &nFrequencyHz, sizeof(nFrequencyHz)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_SetCPUMaxFrequencyAtCurrentVoltage(uint32 ClientHandle, DalDeviceHandle* h, boolean bRequired, uint32 * pnResultFreqHz)
{
  ClockDrvCtxt  *pDrvCtxt = HANDLE2CTXT(h);
  DALResult      eResult;
  uint32         nResultFreqHz;

  eResult = Clock_SetCPUMaxFrequencyAtCurrentVoltage(pDrvCtxt, bRequired, &nResultFreqHz);
  if (pnResultFreqHz && DalSys_CopyToUser(ClientHandle, pnResultFreqHz, &nResultFreqHz, sizeof(nResultFreqHz)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return eResult;
}

static DALResult DALClock_GetDividerId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szDivider, size_t szDividerSize, ClockDividerIdType * pnId)
{
  return DALClock_GetId(ClientHandle, h, szDivider, szDividerSize, pnId, Clock_GetDividerId);
}

static DALResult DALClock_SetDivider(uint32 ClientHandle, DalDeviceHandle* h,  ClockDividerIdType  nDividerId,  uint32  nDivider)
{
  ClockDrvCtxt          *pDrvCtxt = HANDLE2CTXT(h);
  ClockDividerNodeType  *pDivider = Clock_GetDividerNode(pDrvCtxt, nDividerId);

  if (pDivider && CLOCK_FLAG_IS_SET(pDivider, INTERNAL_CONTROL))
  {
    return DAL_ERROR;
  }

  return Clock_SetDivider(HANDLE2CTXT(h), pDivider, nDivider);
}

static DALResult DALClock_GetFrequencyPlan(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType nClock, ClockFreqPlanType **paFreqPlan, uint32 *pnFreqPlanLen)
{
  ClockDrvCtxt      *pDrvCtxt = HANDLE2CTXT(h);
  ClockNodeType     *pClock = Clock_GetClockNode(pDrvCtxt, nClock);
  ClockFreqPlanType *aFreqPlan;
  ClockFreqPlanType *aFreqPlanTemp;
  DALResult          eResult;
  uint32             nFreqPlanLen;
  uint32             nFreqPlanLenTemp;
  uint32             nSize;

  /*-----------------------------------------------------------------------*/
  /* Local client.                                                         */
  /*-----------------------------------------------------------------------*/

  if (ClientHandle == DALDEVICE_LOCAL_CLIENTID)
  {
    return Clock_GetFrequencyPlan(pDrvCtxt, pClock, paFreqPlan, pnFreqPlanLen);
  }

  /*-----------------------------------------------------------------------*/
  /* Remote Client.                                                        */
  /*-----------------------------------------------------------------------*/

  if (!pnFreqPlanLen)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*
   * Call first with NULL array to get required array length.
   */
  eResult = Clock_GetFrequencyPlan(pDrvCtxt, pClock, NULL, &nFreqPlanLenTemp);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*
   * Copy user-supplied length to a temporary, local variable and copy the
   * required length back to user.
   */
  if (DalSys_CopyFromUser(ClientHandle, &nFreqPlanLen, pnFreqPlanLen, sizeof(*pnFreqPlanLen)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }
  if (DalSys_CopyToUser(ClientHandle, pnFreqPlanLen, &nFreqPlanLenTemp, sizeof(nFreqPlanLenTemp)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  /*
   * If no user-supplied array, then only the required length is being
   * queried. Return before allocating or copying array.
   */
  if (!paFreqPlan)
  {
    return DAL_SUCCESS;
  }

  /*
   * If there is a user-supplied array but it's not long enough, then
   * return an error before copying to it.
   */
  if (nFreqPlanLen < nFreqPlanLenTemp)
  {
    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }

  /*
   * Copy user array location into temporary, local variable.
   */
  if (DalSys_CopyFromUser(ClientHandle, &aFreqPlan, paFreqPlan, sizeof(*paFreqPlan)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  /*
   * Allocate temporary array and memset to 0 to prevent any root memory hiding
   * in padded regions being copied to user. Use secure_memset to prevent
   * the compiler from optimizing it out.
   */
  nSize = nFreqPlanLenTemp * sizeof(ClockFreqPlanType);
  if (DALSYS_Malloc(nSize, (void **)&aFreqPlanTemp) != DAL_SUCCESS)
  {
    DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_ERROR,
      "Unable to allocate %lu bytes for frequency plan.", nSize);
    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }
  secure_memset(aFreqPlanTemp, 0, nSize);

  /*
   * Populate temporary array and copy it to user.
   */
  eResult = Clock_GetFrequencyPlan(pDrvCtxt, pClock, &aFreqPlanTemp, &nFreqPlanLenTemp);
  if (DalSys_CopyToUser(ClientHandle, aFreqPlan, aFreqPlanTemp, nSize))
  {
    DALSYS_Free(aFreqPlanTemp);
    return DAL_ERROR_INVALID_ADDRESS;
  }

  DALSYS_Free(aFreqPlanTemp);
  return eResult;
}

static DALResult DALClock_GetClockInfo(uint32 ClientHandle, DalDeviceHandle * h,  ClockIdType nClock, ClockInfoType *pClockInfo)
{
  ClockDrvCtxt *pDrvCtxt = HANDLE2CTXT(h);

  /*
   * Can't support this API for user PDs since the structure contains a pointer
   * to a string. Can't pass the pointer to our root string and can't allocate
   * a new string buffer in user space.
   */
  if (ClientHandle != DALDEVICE_LOCAL_CLIENTID)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  return Clock_GetClockInfo(pDrvCtxt, Clock_GetClockNode(pDrvCtxt, nClock), pClockInfo);
}

/*-----------------------------------------------------------------------*/
/* Stubs.                                                                */
/*-----------------------------------------------------------------------*/


static DALResult DALClockStub_GetClockId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szClock, size_t szClockSize, ClockIdType * pnId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_EnableClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_EnableDFS(uint32 ClientHandle, DalDeviceHandle * h,  ClockIdType  nClock)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_IsDFSEnabled(uint32 ClientHandle,  DalDeviceHandle * h,  ClockIdType  nClock,  boolean * pbIsEnabled)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_DisableClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SetClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq )
{
  if (pnResultFreq && DalSys_CopyToUser(ClientHandle, pnResultFreq, &nFreq, sizeof(nFreq)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult DALClockStub_SetClockDivider(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32  nDivider)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_InvertClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean  bInvert)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ResetClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ResetClock_STM(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ResetClockBranch(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockResetType  eReset)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_IsClockEnabled(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean * pbIsEnabled)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_IsClockOn(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  boolean * pbIsOn)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_GetClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SelectExternalSource(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 nFreqHz,  uint32 nSource, uint32  nDiv2x,  uint32  nM,  uint32  nN,  uint32  n2D)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_CalcClockFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  uint32 * pnFrequencyHz)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_GetDebugMuxId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szDebugMux, size_t szDebugMuxSize, ClockDebugMuxIdType * pnId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_CalcFrequency(uint32 ClientHandle, DalDeviceHandle* h,  ClockDebugMuxIdType nDebugMux, uint32 nMuxSel, uint32 * pnFrequencyHz)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_GetPowerDomainId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szPowerDomain, size_t szPowerDomainSize, ClockPowerDomainIdType * pnPowerDomainId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_EnablePowerDomain(uint32 ClientHandle, DalDeviceHandle* h,  ClockPowerDomainIdType  nPowerDomainId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_DisablePowerDomain(uint32 ClientHandle, DalDeviceHandle* h,  ClockPowerDomainIdType  nPowerDomainId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_EnableDCVS(uint32 ClientHandle, DalDeviceHandle* h)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_LoadNV(uint32 ClientHandle, DalDeviceHandle* h)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ProcessorSleep(uint32 ClientHandle, DalDeviceHandle* h,  ClockSleepModeType  eMode,  uint32  nFlags)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ProcessorRestore(uint32 ClientHandle, DalDeviceHandle* h,  ClockSleepModeType  eMode,  uint32  nFlags)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_ConfigClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock,  ClockConfigType  eConfig)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SetUARTBaudClock(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType  nClock, ClockIdType nRefClock, uint32 nDI, uint32 nFI, uint32 *pnResultFreqHz)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_LogState(uint32 ClientHandle, DalDeviceHandle* h,  void *pULog,  uint32 nFlags)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_GetSourceId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szSource, size_t szSourceSize,  SourceIdType * pnId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_EnableSource(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_DisableSource(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SetSourceFrequency(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource,  uint32  nFreq,  ClockFrequencyType  eMatch, uint32 *pnResultFreq )
{
  if (pnResultFreq && DalSys_CopyToUser(ClientHandle, pnResultFreq, &nFreq, sizeof(nFreq)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult DALClockStub_GetSourceFrequency(uint32 ClientHandle, DalDeviceHandle* h,  SourceIdType  nSource,  uint32 * pnFrequencyHz)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SetCPUMaxFrequencyAtCurrentVoltage(uint32 ClientHandle, DalDeviceHandle* h, boolean bRequired, uint32 * pnResultFreqHz)
{
  uint32 nFreq = 0;

  if (pnResultFreqHz && DalSys_CopyToUser(ClientHandle, pnResultFreqHz, &nFreq, sizeof(nFreq)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult DALClockStub_GetDividerId(uint32 ClientHandle, DalDeviceHandle* h,  const char * szDivider, size_t szDividerSize, ClockDividerIdType * pnId)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_SetDivider(uint32 ClientHandle, DalDeviceHandle* h,  ClockDividerIdType  nDividerId,  uint32  nDivider)
  { return DAL_SUCCESS; }

static DALResult DALClockStub_GetFrequencyPlan(uint32 ClientHandle, DalDeviceHandle* h,  ClockIdType nClock, ClockFreqPlanType **aFreqPlan, uint32 *pnSize)
{
  uint32 nSize = 0;

  if (!pnSize || DalSys_CopyToUser(ClientHandle, pnSize, &nSize, sizeof(nSize)))
  {
    return DAL_ERROR_INVALID_ADDRESS;
  }

  return DAL_SUCCESS;
}

static DALResult DALClockStub_GetClockInfo(uint32 ClientHandle, DalDeviceHandle * h, ClockIdType nClock, ClockInfoType *pClockInfo)
  { return DAL_SUCCESS; }


/* =========================================================================
**  Function : Clock_InitInterface
** =========================================================================*/
/**
  Initialize the client context for communicating to the DAL.

  This function initializes the given client context.  It is invoked
  internally when a client attaches to the dal.

  @param *pclientCtxt [in] -- Pointer to the DAL client context.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_InitInterface
(
  ClockDALClientCtxt* pclientCtxt
)
{
  ClockDrvCtxt  *pDrvCtxt = &pclientCtxt->pClockDevCtxt->pClockDrvCtxt->DrvCtxt;
  static DalClock vtbl =
  {
    .DalDevice = {
      { .AttachEx = Clock_DeviceAttachEx },
      Clock_DeviceDetach,
      Clock_DeviceInit,
      Clock_DeviceDeInit,
      Clock_DeviceOpen,
      Clock_DeviceClose,
      Clock_DeviceInfo,
      Clock_DevicePowerEvent,
      Clock_DeviceSysRequest
    },

    .GetClockId                             = DALClock_GetClockId,
    .EnableClock                            = DALClock_EnableClock,
    .DisableClock                           = DALClock_DisableClock,
    .SetClockFrequency                      = DALClock_SetClockFrequency,
    .SetClockDivider                        = DALClock_SetClockDivider,
    .InvertClock                            = DALClock_InvertClock,
    .ResetClock                             = DALClock_ResetClock,
    .ResetClock_STM                         = DALClock_ResetClock_STM,
    .IsClockEnabled                         = DALClock_IsClockEnabled,
    .IsClockOn                              = DALClock_IsClockOn,
    .GetClockFrequency                      = DALClock_GetClockFrequency,
    .SelectExternalSource                   = DALClock_SelectExternalSource,
    .GetPowerDomainId                       = DALClock_GetPowerDomainId,
    .EnablePowerDomain                      = DALClock_EnablePowerDomain,
    .DisablePowerDomain                     = DALClock_DisablePowerDomain,
    .EnableDCVS                             = DALClock_EnableDCVS,
    .LoadNV                                 = DALClock_LoadNV,
    .ProcessorSleep                         = DALClock_ProcessorSleep,
    .ProcessorRestore                       = DALClock_ProcessorRestore,
    .CalcClockFrequency                     = DALClock_CalcClockFrequency,
    .GetDebugMuxId                          = DALClock_GetDebugMuxId,
    .CalcFrequency                          = DALClock_CalcFrequency,
    .ConfigClock                            = DALClock_ConfigClock,
    .SetUARTBaudClock                       = DALClock_SetUARTBaudClock,
    .LogState                               = DALClock_LogState,
    .GetSourceId                            = DALClock_GetSourceId,
    .EnableSource                           = DALClock_EnableSource,
    .DisableSource                          = DALClock_DisableSource,
    .SetSourceFrequency                     = DALClock_SetSourceFrequency,
    .GetSourceFrequency                     = DALClock_GetSourceFrequency,
    .SetCPUMaxFrequencyAtCurrentVoltage     = DALClock_SetCPUMaxFrequencyAtCurrentVoltage,
    .ResetClockBranch                       = DALClock_ResetClockBranch,
    .GetDividerId                           = DALClock_GetDividerId,
    .SetDivider                             = DALClock_SetDivider,
    .GetFrequencyPlan                       = DALClock_GetFrequencyPlan,
    .GetClockInfo                           = DALClock_GetClockInfo,
    .EnableDFS                              = DALClock_EnableDFS,
    .IsDFSEnabled                           = DALClock_IsDFSEnabled,
  };

  /*-----------------------------------------------------------------------*/
  /* Stub out the DAL API if the API stub flag is set.                     */
  /*-----------------------------------------------------------------------*/

  if (CLOCK_GLOBAL_FLAG_IS_SET(STUB_API))
  {
    vtbl.GetClockId                         = DALClockStub_GetClockId;
    vtbl.EnableClock                        = DALClockStub_EnableClock;
    vtbl.DisableClock                       = DALClockStub_DisableClock;
    vtbl.SetClockFrequency                  = DALClockStub_SetClockFrequency;
    vtbl.SetClockDivider                    = DALClockStub_SetClockDivider;
    vtbl.InvertClock                        = DALClockStub_InvertClock;
    vtbl.ResetClock                         = DALClockStub_ResetClock;
    vtbl.ResetClock_STM                     = DALClockStub_ResetClock_STM;
    vtbl.IsClockEnabled                     = DALClockStub_IsClockEnabled;
    vtbl.IsClockOn                          = DALClockStub_IsClockOn;
    vtbl.GetClockFrequency                  = DALClockStub_GetClockFrequency;
    vtbl.SelectExternalSource               = DALClockStub_SelectExternalSource;
    vtbl.GetPowerDomainId                   = DALClockStub_GetPowerDomainId;
    vtbl.EnablePowerDomain                  = DALClockStub_EnablePowerDomain;
    vtbl.DisablePowerDomain                 = DALClockStub_DisablePowerDomain;
    vtbl.EnableDCVS                         = DALClockStub_EnableDCVS;
    vtbl.LoadNV                             = DALClockStub_LoadNV;
    vtbl.ProcessorSleep                     = DALClockStub_ProcessorSleep;
    vtbl.ProcessorRestore                   = DALClockStub_ProcessorRestore;
    vtbl.CalcClockFrequency                 = DALClockStub_CalcClockFrequency;
    vtbl.GetDebugMuxId                      = DALClockStub_GetDebugMuxId;
    vtbl.CalcFrequency                      = DALClockStub_CalcFrequency;
    vtbl.ConfigClock                        = DALClockStub_ConfigClock;
    vtbl.SetUARTBaudClock                   = DALClockStub_SetUARTBaudClock;
    vtbl.LogState                           = DALClockStub_LogState;
    vtbl.GetSourceId                        = DALClockStub_GetSourceId;
    vtbl.EnableSource                       = DALClockStub_EnableSource;
    vtbl.DisableSource                      = DALClockStub_DisableSource;
    vtbl.SetSourceFrequency                 = DALClockStub_SetSourceFrequency;
    vtbl.GetSourceFrequency                 = DALClockStub_GetSourceFrequency;
    vtbl.SetCPUMaxFrequencyAtCurrentVoltage = DALClockStub_SetCPUMaxFrequencyAtCurrentVoltage;
    vtbl.ResetClockBranch                   = DALClockStub_ResetClockBranch;
    vtbl.GetDividerId                       = DALClockStub_GetDividerId;
    vtbl.SetDivider                         = DALClockStub_SetDivider;
    vtbl.GetFrequencyPlan                   = DALClockStub_GetFrequencyPlan;
    vtbl.GetClockInfo                       = DALClockStub_GetClockInfo;
    vtbl.EnableDFS                          = DALClockStub_EnableDFS;
    vtbl.IsDFSEnabled                       = DALClockStub_IsDFSEnabled;
  }

  pclientCtxt->DalClockHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pclientCtxt->DalClockHandle.pVtbl  = &vtbl;
  pclientCtxt->DalClockHandle.pClientCtxt = pclientCtxt;
  pclientCtxt->DalClockHandle.dwVtblen = sizeof(vtbl)/sizeof(void (*)(void));

} /* END Clock_InitInterface */


/* =========================================================================
**  Function : Clock_DeviceAttachEx
** =========================================================================*/
/**
  Internal DAL framework function for attaching to the clock DAL in multi-
  environments.

  This function attaches to the clock DAL and initializes the given
  device handle in multi- environments for providing multi- restart
  functionality.

  @param *pszArg[in] -- String passed by user during attach.
  @param DeviceId   [in]  -- The device id, should be DALDEVICEID_CLOCK
  @param phDalDevice[out] -- DAL device handle to fill in.
  @param nPDID  [in]  -- Client ID/ that the call is coming from.

  @return
  DAL_SUCCESS - Successfully initialized and attached to driver.
  DAL_ERROR   - Failed to initialize the driver.

  @dependencies
  None.
*/

DALResult
Clock_DeviceAttachEx(const char *pszArg,
                        DALDEVICEID DeviceId,
                        DalDeviceHandle **phDalDevice,
                        int nPDID)
{
   DALResult nErr;
   uint32 nIdx;
   ClockDALClientCtxt *pClientCtxt = NULL;
   *phDalDevice = NULL;

   /*
    * Look for a valid client context containing a matching nPDID value and
    * connect this request if found.
    */
   for (nIdx=0; nIdx<CLOCK_CTXT_TOTAL; nIdx++)
   {
     if ((ClockClientCtxts[nIdx].CtxtInfo.nPDID == nPDID) &&
         (ClockClientCtxts[nIdx].CtxtInfo.bInUse == TRUE))
     {
       pClientCtxt = &ClockClientCtxts[nIdx];
       break;
     }
   }

   /*
    * If there was no valid client context associated with the client ID, then
    * look for the next available client context that can be connected to this
    * nPDID.
    */
   if (pClientCtxt == NULL)
   {
     for (nIdx=0; nIdx<CLOCK_CTXT_TOTAL; nIdx++)
     {
       if(ClockClientCtxts[nIdx].CtxtInfo.bInUse == FALSE)
       {
         pClientCtxt = &ClockClientCtxts[nIdx];
         pClientCtxt->CtxtInfo.nPDID = nPDID;
         pClientCtxt->CtxtInfo.nIdx = nIdx;
         pClientCtxt->CtxtInfo.bInUse = TRUE;
         break;
       }
     }

     if (pClientCtxt == NULL)
     {
       return DAL_ERROR;
     }
   }

   nErr = DALFW_AttachToDevice(DeviceId,(DALDrvCtxt *)&Clock_DriverCtxt,
                                        (DALClientCtxt *)pClientCtxt);
   if (DAL_SUCCESS == nErr)
   {
      Clock_InitInterface(pClientCtxt);
      Clock_AddRef(&(pClientCtxt->DalClockHandle));
      *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalClockHandle);
   }
   return nErr;

} /* END Clock_DeviceAttachEx */


/* =========================================================================
**  Function : Clock_GetPropertyValue
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_GetPropertyValue
(
  const char              *szName,
  ClockPropertyValueType  *pValue
)
{
  DALResult         eRes;
  DALSYSPropertyVar Var;

  eRes = DALSYS_GetPropertyValue(
    Clock_DriverCtxt.ClockDevCtxt[0].hProp, szName, 0, &Var);

  if (eRes != DAL_SUCCESS)
  {
    return eRes;
  }

  if (Var.dwType == DALPROP_ATTR_TYPE_STRUCT_PTR)
  {
    *pValue = (ClockPropertyValueType)Var.Val.pStruct;
  }
  else if (Var.dwType == DALPROP_ATTR_TYPE_UINT32)
  {
    *pValue = (ClockPropertyValueType)Var.Val.dwVal;
  }

  return DAL_SUCCESS;

} /* END Clock_GetPropertyValue */


/* =========================================================================
**  Function : Clock_MapHWIORegion
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_MapHWIORegion
(
  uintnt  nPhysAddr,
  uint32  nSizeBytes,
  uintnt *pnVirtAddr
)
{
  DALResult eRes = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/
  /* If we attached to the HWIO DAL, then map the regions.                 */
  /*-----------------------------------------------------------------------*/

  if (Clock_DriverCtxt.phHWIO != NULL)
  {
    eRes = DalHWIO_MapRegionByAddress(
      Clock_DriverCtxt.phHWIO, (uint8 *)nPhysAddr, (uint8 **)pnVirtAddr);
  }

  /*-----------------------------------------------------------------------*/
  /* No HWIO DAL, so assume base address is the physical address.          */
  /*-----------------------------------------------------------------------*/

  else
  {
    *pnVirtAddr = nPhysAddr;
  }

  return eRes;

} /* END Clock_MapHWIORegion */


/*=========================================================================
**  Function : Clock_GetDrvCtxt
** =========================================================================*/
/**
  See ClockDriver.h
*/

ClockDrvCtxt *Clock_GetDrvCtxt(void)
{
  /*-----------------------------------------------------------------------*/
  /* Acquire mutex shared with lite API.                                   */
  /*-----------------------------------------------------------------------*/

  /*
   * This is a recursive mutex. Nested calls will not result in deadlocks.
   */

  qurt_rmutex_lock(&hClockLiteMutex);

  /*-----------------------------------------------------------------------*/
  /* Return driver context.                                                */
  /*-----------------------------------------------------------------------*/

  return &Clock_DriverCtxt.DrvCtxt;

} /* END of Clock_GetDrvCtxt */


/*=========================================================================
**  Function : Clock_GetDrvCtxtNoLock
** =========================================================================*/
/**
  Internal DAL Clock API to retrieve the driver context without causing a
  mutex lock.

  THis function returns the static clock driver context without locking a
  mutex in the process.

  @return
  A pointer to the static clock driver context.

  @dependencies
  None.
*/
ClockDrvCtxt *Clock_GetDrvCtxtNoLock(void)
{
  return &Clock_DriverCtxt.DrvCtxt;

} /* END Clock_GetDrvCtxtNoLock */


/*=========================================================================
**  Function : Clock_ReleaseDrvCtxt
** =========================================================================*/
/**
  See ClockDriver.h
*/

void Clock_ReleaseDrvCtxt(void)
{
  /*-----------------------------------------------------------------------*/
  /* Release mutex shared with lite API.                                   */
  /*-----------------------------------------------------------------------*/

  qurt_rmutex_unlock(&hClockLiteMutex);

} /* END of Clock_ReleaseDrvCtxt */


/*=========================================================================
**  Function : Clock_GetInitialClientCtxt
** =========================================================================*/
/**
  Internal DAL framework function for retrieving the first client context

  This function retrieves the first client context which will be associated
  with the root client (or ).  This will always be initialized on the first
  attach from RCINIT.

  @return
  Pointer to the first client context (or root ).

  @dependencies
  None.
*/

ClockDALClientCtxt *Clock_GetInitialClientCtxt(void)
{
  return &ClockClientCtxts[0];

} /* END Clock_GetInitialClientCtxt */


/*=========================================================================
**  Function : Clock_ReleaseClientResources
** =========================================================================*/
/**
  Internal API to release resources

  Cleans up all enabled clock resources for a particular client that
  has done a final detach on the clock driver.  In most cases this will occur
  after a  restart after the DAL framework has decremented all stale
  device attach references.

  @return
  None.

  @dependencies
  None.
*/

static void Clock_ReleaseClientResources
(
  ClockDALClientCtxt* pClientCtxt
)
{
  uint32        nIdx;
  ClockDrvCtxt *pDrvCtxt = Clock_GetDrvCtxtNoLock();
  ClockBSPType *pBSP = pDrvCtxt->pBSP;

  /*
   * Disable all clocks associated with this client context.
   */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nClockRefs > 0)
  {
    for (nIdx=0; nIdx<pBSP->nNumClocks; nIdx++)
    {
      while (pBSP->aClocks[nIdx].nReferenceCount[pClientCtxt->CtxtInfo.nIdx] > 0)
      {
        Clock_DisableClock(pClientCtxt, &pBSP->aClocks[nIdx]);
      }
    }
  }

  /*
   * Disable all external source requests associated with this client context.
   */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nSourceRefs > 0)
  {
    for (nIdx=0; nIdx<pBSP->nNumSources; nIdx++)
    {
      while (pBSP->aSources[nIdx]->nReferenceCountExternal[pClientCtxt->CtxtInfo.nIdx] > 0)
      {
        Clock_DisableSource(pClientCtxt, pBSP->aSources[nIdx]);
      }
    }
  }

  /*
   * Disable all power domains associated with this client context.
   */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nPowerDomainRefs > 0)
  {
    for (nIdx=0; nIdx<pBSP->nNumPowerDomains; nIdx++)
    {
      while (pBSP->aPowerDomains[nIdx].nReferenceCount[pClientCtxt->CtxtInfo.nIdx] > 0)
      {
        Clock_DisablePowerDomain(pClientCtxt, &pBSP->aPowerDomains[nIdx]);
      }
    }
  }

  /*
   * Clean up and allow resuse of the client context.
   */
   ClockClientCtxts[pClientCtxt->CtxtInfo.nIdx].CtxtInfo.nPDID = 0;
   ClockClientCtxts[pClientCtxt->CtxtInfo.nIdx].CtxtInfo.bInUse = FALSE;

} /* END Clock_ReleaseClientResources */

