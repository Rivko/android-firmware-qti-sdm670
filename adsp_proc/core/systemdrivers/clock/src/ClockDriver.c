/*
===========================================================================
*/
/**
  @file Clock.c

  Interface implementation file for the clock device driver.
*/
/*
  ====================================================================

  Copyright (c) 2010-2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/src/ClockDriver.c#3 $
  $DateTime: 2017/10/25 23:14:56 $
  $Author: pwbldsvc $

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "ClockDriver.h"
#include "HALclk.h"
#include "DDIChipInfo.h"
#include "ClockSWEVT.h"
#include "DDIPlatformInfo.h"


/*=========================================================================
      Forward references
==========================================================================*/

static DALResult Clock_InitDFSDomains
(
  ClockDrvCtxt *pDrvCtxt
);


/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult               eResult;
  HAL_clk_ContextType     HALClkCtxt = {0};
  ClockPropertyValueType  PropVal;
  uint32                  nLogSize;

  /*-----------------------------------------------------------------------*/
  /* Create the driver synchronization object.                             */
  /*-----------------------------------------------------------------------*/

  eResult = DALSYS_SyncCreate (DALSYS_SYNC_ATTR_RESOURCE,
                               &pDrvCtxt->hLock, &pDrvCtxt->LockObj);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "DALSYS_SyncCreate failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the log.                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_GetPropertyValue("ClockLogDefaults", &PropVal);

  if (eResult == DAL_SUCCESS)
  {
    nLogSize = ((ClockLogType *)PropVal)->nLogSize;
    pDrvCtxt->pBSP->nFlags |= ((ClockLogType *)PropVal)->nGlobalLogFlags;
  }
  else
  {
    nLogSize = CLOCK_DEFAULT_LOG_SIZE;
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
      "Missing clock log defaults. Defaulting size to %lu.", nLogSize);
  }

    ULogFront_RealTimeInit (&pDrvCtxt->hClockLog, "Clock Log",
                          nLogSize,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);

  ULogCore_HeaderSet (pDrvCtxt->hClockLog,
                      "Content-Type: text/tagged-log-1.0;");

  /*-----------------------------------------------------------------------*/
  /* Initialize base addresses.                                            */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitBases(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitBases failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the clock HAL.                                             */
  /*-----------------------------------------------------------------------*/

  /*
   * Use DALSys busy wait function for HAL modules.
   */
  HALClkCtxt.BusyWait = DALSYS_BusyWait;

  /*
   * Get chip version to send to the HAL.
   */
  HALClkCtxt.nChipVersion = DalChipInfo_ChipVersion();
  HALClkCtxt.nChipId = DalChipInfo_ChipId();
  HALClkCtxt.nChipFamily = DalChipInfo_ChipFamily();

  HAL_clk_Init(&HALClkCtxt);

  /*-----------------------------------------------------------------------*/
  /* Init voltage module.                                                  */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitVoltage(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitVoltage failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Init DFS domains.                                                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitDFSDomains(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitDFSDomains failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver.                                 */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitTarget (pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitTarget failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Init NPA resources.                                                   */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitNPA(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "Clock_InitNPA failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_DriverInit */


/* =========================================================================
**  Function : ClockStub_DriverInit
** =========================================================================*/
/**
  Initialize the clock driver stub.

  This function initializes the clock driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult eResult;

  /*-----------------------------------------------------------------------*/
  /* Initialize the image specific driver stub.                            */
  /*-----------------------------------------------------------------------*/

  eResult = ClockStub_InitTarget(pDrvCtxt);

  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
      "ClockStub_InitTarget failed.");
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialization complete.                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ClockStub_DriverInit */


/* =========================================================================
**  Function : Clock_MapAddress
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_MapAddress
(
  ClockDrvCtxt *pDrvCtxt,
  uintnt       *pnAddr
)
{
  ClockBaseType *pBase;
  uint32         i;

  if (*pnAddr == 0)
  {
    return DAL_SUCCESS;
  }

  for (i = 0; i < pDrvCtxt->pBSP->nNumBases; i++)
  {
    pBase = &pDrvCtxt->pBSP->aBases[i];

    if (*pnAddr >= pBase->nPhysAddr &&
        *pnAddr <  pBase->nPhysAddr + pBase->nSize)
    {
      *pnAddr = *pnAddr - pBase->nPhysAddr + *pBase->pnVirtAddr;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END Clock_MapAddress */


/* =========================================================================
**  Function : Clock_InitBases
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_InitBases
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockBSPType                *pBSP = pDrvCtxt->pBSP;
  ClockBaseType               *pBase;
  HAL_clk_ClockDescType       *pHALClock;
  HAL_clk_SourceDescType      *pHALSource;
  HAL_clk_ClockDomainDescType *pHALDomain;
  HAL_clk_DividerDescType     *pHALDivider;
  HAL_clk_PowerDomainDescType *pHALPowerDomain;
  DALResult                    eResult = DAL_SUCCESS;
  uint32                       i;

  if (pBSP->aBases == NULL || pBSP->nNumBases == 0)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Map base regions.                                                     */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pBSP->nNumBases; i++)
  {
    pBase = &pBSP->aBases[i];
    eResult = Clock_MapHWIORegion(pBase->nPhysAddr, pBase->nSize, pBase->pnVirtAddr);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Map individual register addresses.                                    */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pBSP->nNumClocks; i++)
  {
    pHALClock = &pBSP->aClocks[i].HALClock;
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALClock->mRegisters.nBranchAddr);
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALClock->mRegisters.nResetAddr);
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALClock->mRegisters.mHWVotable.nAddr);
  }

  for (i = 0; i < pBSP->nNumDomains; i++)
  {
    pHALDomain = &pBSP->aDomains[i]->HALDomain;
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALDomain->nCGRAddr);
  }

  for (i = 0; i < pBSP->nNumSources; i++)
  {
    pHALSource = &pBSP->aSources[i]->HALSource;
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALSource->nAddr);
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALSource->VoterRegister.nAddr);
  }

  for (i = 0; i < pBSP->nNumDividers; i++)
  {
    pHALDivider = &pBSP->aDividers[i].HALDivider;
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALDivider->nAddr);
  }

  for (i = 0; i < pBSP->nNumPowerDomains; i++)
  {
    pHALPowerDomain = &pBSP->aPowerDomains[i].HALPowerDomain;
    eResult |= Clock_MapAddress(pDrvCtxt, &pHALPowerDomain->nGDSCRAddr);
  }

  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END Clock_InitBases */


/* =========================================================================
**  Function : Clock_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult Clock_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Destroy the mutex object.                                             */
  /*-----------------------------------------------------------------------*/

  DALSYS_DestroyObject(pDrvCtxt->hLock);

  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_DriverDeInit */


/* =========================================================================
**  Function : ClockStub_DriverDeInit
** =========================================================================*/
/**
  De-initialize the clock driver stub.

  This function shuts-down the clock driver.  It is not expected to ever
  be called.

  @param *pdrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Deinititialization was successful.
  DAL_ERROR -- Deinitialization failed.

  @dependencies
  None.
*/

DALResult ClockStub_DriverDeInit
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Shut-down complete.                                                   */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ClockStub_DriverDeInit */


/* =========================================================================
**  Function : Clock_GetClockId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetClockId
(
  ClockDrvCtxt *pDrvCtxt,
  const char   *szClock,
  ClockIdType  *pnId
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  uint32   nClockIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szClock || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Search the array of clock nodes, break if we find a match             */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nClockIndex = 0; nClockIndex < pBSP->nNumClocks; nClockIndex++)
  {
    if (strcmp (szClock, pBSP->aClocks[nClockIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the clock node if found, otherwise return NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = CLOCK_MAKE_ID(CLOCK_ID_CATEGORY_CLOCK, nClockIndex);
    return DAL_SUCCESS;
  }
  else
  {
    *pnId = CLOCK_ID_INVALID;
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetClockId */


/*=========================================================================
**  Function : Clock_GetClockNode
** =========================================================================*/
/**
  See ClockDriver.h
*/

ClockNodeType *Clock_GetClockNode
(
  ClockDrvCtxt *pDrvCtxt,
  ClockIdType   nId
)
{
  ClockIdCategoryType nCat = CLOCK_GET_ID_CATEGORY(nId);
  uint32              nIdx = CLOCK_GET_ID_INDEX(nId);

  if (nCat == CLOCK_ID_CATEGORY_CLOCK && nIdx < pDrvCtxt->pBSP->nNumClocks)
  {
    return &pDrvCtxt->pBSP->aClocks[nIdx];
  }

  return NULL;

} /* END Clock_GetClockNode */


/*=========================================================================
**  Function : Clock_InitDFSDomain
** =========================================================================*/
/**
  Initialize a DFS domain.

  @param  *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.
*/

static DALResult Clock_InitDFSDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pDomain
)
{
  HAL_clk_ClockMuxConfigType   DetectedCfg;
  const ClockSourceMapType    *pSourceMap;
  ClockMuxConfigType          *pFreqConfig;
  rail_voltage_level           eVRegLevel;
  DALResult                    eResult;
  uint32                       nFreqHz;
  uint32                       nDFSStates;
  uint32                       nSize;
  uint32                       i;
  uint32                       nDenom;

    /*
   * If DFS isn't enabled in HW, then there is nothing to do.
     */
    if (!HAL_clk_IsClockDomainDFSEnabled(&pDomain->HALDomain))
    {
    return DAL_SUCCESS;
    }

    CLOCK_FLAG_SET(pDomain, DFS_CONTROLLED);

    /*-----------------------------------------------------------------------*/
    /* Allocate new BSP array to store DFS configurations.                   */
    /*-----------------------------------------------------------------------*/

    nDFSStates = pDomain->HALDomain.nDFSStates;
    pDomain->nBSPLen = nDFSStates;

    nSize = nDFSStates * sizeof(ClockMuxConfigType);
    eResult = DALSYS_Malloc(nSize, (void **)&pDomain->aBSP);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to allocate %lu bytes for Clock DFS array.", nSize);

      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }

    DALSYS_memset(pDomain->aBSP, 0, nSize);

    /*-----------------------------------------------------------------------*/
    /* Initialize new BSP array with dynamically detected DFS configs.       */
    /*-----------------------------------------------------------------------*/

    for (i = 0; i < nDFSStates; i++)
    {
      HAL_clk_DetectDFSMuxConfig(&pDomain->HALDomain, &DetectedCfg, i);

      eResult =
        Clock_GetMappedSource(pDomain->aSourceMap, DetectedCfg.nMuxSel, &pSourceMap);
      if (eResult != DAL_SUCCESS || pSourceMap->nFreqHz == 0)
      {
        DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
          "Unable to map mux input to source for DFS domain.");
        return DAL_ERROR_INTERNAL;
      }

      nDenom = DetectedCfg.nDiv2x;
      nFreqHz = ((pSourceMap->nFreqHz * 2 + nDenom / 2) / nDenom);

      if (DetectedCfg.nM)
      {
        nDenom = DetectedCfg.nN;
        nFreqHz =
          (uint32)(((uint64)nFreqHz * DetectedCfg.nM + nDenom / 2) / nDenom);
      }

      eResult = Clock_GetRequiredCorner(pDomain->aFMax, nFreqHz, &eVRegLevel);
      if (eResult != DAL_SUCCESS)
      {
        /*
         * Frequency from boot exceeds our known max, so the fmax has most
         * likely changed. Default to Turbo until the local BSP data is
         * updated.
         */
        eVRegLevel = RAIL_VOLTAGE_LEVEL_TUR;
        DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
          "DFS frequency exceeds known max. Assuming Turbo.");
      }

      pFreqConfig = &pDomain->aBSP[i];
      pFreqConfig->nFreqHz = nFreqHz;
      pFreqConfig->eVRegLevel = eVRegLevel;
      pFreqConfig->pSource = pSourceMap->pSource;
      DALSYS_memscpy(&pFreqConfig->HALConfig, sizeof(pFreqConfig->HALConfig),
                     &DetectedCfg,            sizeof(DetectedCfg));
    }

  return DAL_SUCCESS;

} /* END Clock_InitDFSDomain */


/*=========================================================================
**  Function : Clock_InitDFSDomains
** =========================================================================*/
/**
  Initialize DFS domains.

  @param  *pDrvCtxt [in] -- Handle to the DAL driver context.

  @return
  DAL_SUCCESS -- Initialization was successful.
  DAL_ERROR -- Initialization failed.
*/

static DALResult Clock_InitDFSDomains
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockDomainNodeType *pDomain;
  DALResult            eResult;
  uint32               i;

  for (i = 0; i < pDrvCtxt->pBSP->nNumDomains; i++)
  {
    pDomain = pDrvCtxt->pBSP->aDomains[i];
    if (pDomain->HALDomain.nDFSStates > 0)
    {
      eResult = Clock_InitDFSDomain(pDrvCtxt, pDomain);
      if (eResult != DAL_SUCCESS)
      {
        return DAL_ERROR;
      }
    }
  }

  return DAL_SUCCESS;

} /* END Clock_InitDFSDomains */


/* =========================================================================
**  Function : Clock_EnableDFS
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableDFS
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockNodeType      *pClock
)
{
  ClockDomainNodeType *pDomain;
  ClockMuxConfigType  *pConfig;
  DALResult            eResult;
  uint32               i;

  if (!pClock)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (!pClock->pDomain || pClock->pDomain->HALDomain.nDFSStates == 0)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  pDomain = pClock->pDomain;
  if (CLOCK_FLAG_IS_SET(pDomain, DFS_CONTROLLED))
  {
    return DAL_SUCCESS;
  }

  /*
   * Write DFS-supported configurations to HW.
   */
  for (i = 0; i < pDomain->nBSPLen; i++)
  {
    pConfig = &pDomain->aBSP[i];
    if (pConfig->HALConfig.nDFSIdx != 0xFF &&
        Clock_IsBSPSupported(&pConfig->HWVersion))
    {
      HAL_clk_ConfigDFSMux(&pDomain->HALDomain, &pConfig->HALConfig);
    }
  }

  /*
   * Enable DFS control in HW.
   */
  HAL_clk_EnableDFS(&pDomain->HALDomain);

  /*
   * Now that DFS is configured and enabled in HW, initialize the driver node
   * to detect and dynamically create new BSP array from configured DFS HW
   * settings.
   */
  eResult = Clock_InitDFSDomain(pDrvCtxt, pDomain);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  return DAL_SUCCESS;

} /* END Clock_EnableDFS */


/* =========================================================================
**  Function : Clock_IsDFSEnabled
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_IsDFSEnabled
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  boolean       *pbIsEnabled
)
{
  if (!pClock || !pbIsEnabled)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (!pClock->pDomain)
  {
    *pbIsEnabled = FALSE;
  }
  else
  {
    DALCLOCK_LOCK(pDrvCtxt);
    *pbIsEnabled = HAL_clk_IsClockDomainDFSEnabled(&pClock->pDomain->HALDomain);
    DALCLOCK_FREE(pDrvCtxt);
  }

  return DAL_SUCCESS;

} /* END Clock_IsDFSEnabled */


/* =========================================================================
**  Function : Clock_InitClockDomain
** =========================================================================*/
/**
  Initializes a clock domain.

  This function initializes a clock domain, generally as a result of a call to
  Clock_EnableClockDomain().

  @param *pDrvCtxt [in] -- Pointer to the driver context.
  @param *pClockDomain [in] -- Pointer to a clock domain node.

  @return
  DAL_SUCCESS -- The domain was successfully initialized.
  DAL_ERROR -- The domain was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_InitClockDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pClockDomain
)
{
  HAL_clk_ClockMuxConfigType  DetectedConfig = {0};
  HAL_clk_ClockMuxConfigType *pSafeMuxConfig;
  const ClockMuxConfigType   *pFreqConfig;
  DALResult                   eResult;
  uint32                      i;

  if (!pClockDomain->aBSP ||
      pClockDomain->pActiveMuxConfig ||
      CLOCK_FLAG_IS_SET(pClockDomain, DFS_CONTROLLED))
  {
    return DAL_SUCCESS;
  }

  HAL_clk_DetectClockMuxConfig(&pClockDomain->HALDomain, &DetectedConfig);

  for (i = 0; i < pClockDomain->nBSPLen; i++)
  {
    pFreqConfig = &pClockDomain->aBSP[i];
    if (!Clock_IsBSPSupported(&pFreqConfig->HWVersion))
    {
      continue;
    }

    /*
     * Check if this configuration matches what was detected.
     */
    if (HAL_clk_IsClockMuxConfigEqual(&DetectedConfig, &pFreqConfig->HALConfig))
    {
      /*
       * If the frequency config has a source to configure, then
       * detect the source configuration as well.
       */
      if (pFreqConfig->pSource && pFreqConfig->pSourceFreqConfig)
      {
        eResult = Clock_DetectSourceConfig(pDrvCtxt, pFreqConfig->pSource);
        if (eResult != DAL_SUCCESS)
        {
          return DAL_ERROR;
        }
      }

      /*
       * If there is no source component or the active source configuration
       * matches, then we've found the active domain configuration.
       */
      if (!pFreqConfig->pSource ||
          !pFreqConfig->pSourceFreqConfig ||
           pFreqConfig->pSource->pActiveFreqConfig == pFreqConfig->pSourceFreqConfig)
      {
        pClockDomain->pActiveMuxConfig = pFreqConfig;
        pClockDomain->pSource = pFreqConfig->pSource;
        break;
      }
    }
  }

  /*
   * If we didn't find a match for the detected configuration in the BSP data
   * and the detected configuration isn't the default safe configuration, then
   * return an error. Don't flag an error for the safe configuration since it
   * doesn't have any dependencies and including it in every domains BSP
   * data is expensive.
   */
  if (pClockDomain->pActiveMuxConfig == NULL)
  {
    pSafeMuxConfig = &pDrvCtxt->pBSP->SafeMuxConfig;
    if (!HAL_clk_IsClockMuxConfigEqual(&DetectedConfig, pSafeMuxConfig))
    {
      return DAL_ERROR;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_InitClockDomain */


/* =========================================================================
**  Function : Clock_EnableClockDomain
** =========================================================================*/
/**
  Enables a clock domain.

  This function enables a clock domain, generally as a result of a call to
  Clock_EnableClock().

  @param *pDrvCtxt [in] -- Pointer to the driver context.
  @param *pClockDomain [in] -- Pointer to a clock domain node.
  @param  nClockFlags  [in] -- Flags for any clock attributes.

  @return
  DAL_SUCCESS -- The domain was successfully enabled.
  DAL_ERROR -- The domain was not enabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_EnableClockDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pClockDomain,
  uint32               nClockFlags
)
{
  DALResult             eResult;
  boolean               bSuppressibleRequest;
  ClockVRegRequestType  VRegRequest;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /*
   * Initialize domain if not done yet.
   */
  if (pClockDomain->aBSP != NULL &&
      pClockDomain->pActiveMuxConfig == NULL)
  {
    eResult = Clock_InitClockDomain(pDrvCtxt, pClockDomain);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /* Check if the clock or domain are suppressible */
  if ((nClockFlags & CLOCK_FLAG_SUPPRESSIBLE) != 0 ||
      CLOCK_FLAG_IS_SET(pClockDomain, SUPPRESSIBLE))
  {
    bSuppressibleRequest = TRUE;
  }
  else
  {
    bSuppressibleRequest = FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    /*
     * We enable the source for the first insuppressible clock even if
     * another suppressible clock has already enabled the clock domain
     * because we need to keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCount == 0)
    {
      /* If this domain has a controllable source, enable it first */
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pClockDomain->pSource, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * When there is no suppressible reference count, make the initial voltage
       * request. When there is already a suppressible reference count, change
       * the request to suppressible.
       */
      if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
          pClockDomain->pActiveMuxConfig != NULL)
      {
        VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
        VRegRequest.bSuppressible = FALSE;
        Clock_VoltageRequest(pClockDomain->pRail, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Increment the insuppressible reference count.
     */
    pClockDomain->nReferenceCount++;
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * We enable the source for the first suppressible clock even if
     * another insuppressible clock has already enabled the clock domain
     * because we need to keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCountSuppressible == 0)
    {
      /* If this domain has a controllable source, enable it first */
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_EnableSourceInternal(pDrvCtxt, pClockDomain->pSource, TRUE);
      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * Only make the voltage request if there is no insuppressible reference
     * count, since the insuppressible voltage requests take priority.
     */
    if (pClockDomain->nReferenceCount == 0 &&
        !CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
        pClockDomain->pActiveMuxConfig != NULL)
    {
      VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
      VRegRequest.bSuppressible = TRUE;
      Clock_VoltageRequest(pClockDomain->pRail, &pClockDomain->VRegRequest, &VRegRequest);
    }
  }

    /*
     * Increment the reference count.
     */
    pClockDomain->nReferenceCountSuppressible++;
  }

  /*
   * Restore domain to its active configuration after enabling it, since
   * it was set to a safe frequency before disabling.
   */
  if ((pClockDomain->nReferenceCount + pClockDomain->nReferenceCountSuppressible) == 1 &&
      pClockDomain->pActiveMuxConfig &&
      !CLOCK_FLAG_IS_SET(pClockDomain, DFS_CONTROLLED))
  {
    HAL_clk_ConfigClockMux(&pClockDomain->HALDomain,
      &pClockDomain->pActiveMuxConfig->HALConfig);
  }

  return DAL_SUCCESS;

} /* END Clock_EnableClockDomain */


/* =========================================================================
**  Function : Clock_DisableClockDomain
** =========================================================================*/
/**
  Disables a clock domain.

  This function disables a clock domain, generally as a result of a call to
  Clock_DisableClock().

  @param *pDrvCtxt     [in] -- Pointer to the driver context.
  @param *pClockDomain [in] -- Pointer to a clock domain node.
  @param  nClockFlags  [in] -- Flags for any clock attributes.

  @return
  DAL_SUCCESS -- The domain was successfully disabled.
  DAL_ERROR -- The domain was not disabled.

  @dependencies
  The clock mutex must be locked prior to calling this function.
*/

DALResult Clock_DisableClockDomain
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pClockDomain,
  uint32               nClockFlags
)
{
  DALResult             eResult;
  boolean               bSuppressibleRequest;
  ClockVRegRequestType  VRegRequest;
  uint32                nTimeout = 0;
  boolean               bIsOn;

  /* Make sure the domain handle is valid */
  if (pClockDomain == NULL)
  {
    return DAL_ERROR;
  }

  /* Check if the clock or domain are suppressible */
  if ((nClockFlags & CLOCK_FLAG_SUPPRESSIBLE) != 0 ||
      CLOCK_FLAG_IS_SET(pClockDomain, SUPPRESSIBLE))
  {
    bSuppressibleRequest = TRUE;
  }
  else
  {
    bSuppressibleRequest = FALSE;
  }

  if ((pClockDomain->nReferenceCount + pClockDomain->nReferenceCountSuppressible) == 1)
  {
    /*-----------------------------------------------------------------------*/
    /* In some cases when disabling the last clock in a domain that is       */
    /* configured to a low frequency, it can take extra time for the clock   */
    /* root to receive the "off" signal.  If the "off" signal has not been   */
    /* received before the source is turned off, the clock domain can be     */
    /* left in an unexpected state, causing a hang when switching the domain */
    /* frequency.  To avoid disabling the source before the clock domain     */
    /* receives this off signal, if there are no more clocks enabled in this */
    /* domain, wait for the root-off signal from HW.                         */
    /*-----------------------------------------------------------------------*/

    if (CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_WAIT_FOR_ROOT_OFF))
    {
      if (HAL_clk_IsClockDomainSupported(&pClockDomain->HALDomain, HAL_CLK_CLOCK_DOMAIN_API_ISON))
      {
        bIsOn = HAL_clk_IsClockDomainOn(&pClockDomain->HALDomain);

        while ((bIsOn == TRUE) && (nTimeout++ < 150))
        {
          DALSYS_BusyWait(1);
          bIsOn = HAL_clk_IsClockDomainOn(&pClockDomain->HALDomain);
        }

        if (nTimeout == 150)
        {
          DALSYS_LogEvent (
              DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
              "Clock domain root did not turn off after all domain clocks are off");
          return DAL_ERROR_INTERNAL;
        }
      }
    }
    
    /*
     * Set domain to a safe configuration prior to disabling it to ensure
     * source and voltage dependencies are satisfied if some other entity
     * (HW or SW) enables it without our knowledge.
     */
    if (pClockDomain->pActiveMuxConfig &&
        !CLOCK_FLAG_IS_SET(pClockDomain, DFS_CONTROLLED))
    {
      HAL_clk_ConfigClockMux(&pClockDomain->HALDomain, &pDrvCtxt->pBSP->SafeMuxConfig);
    }    
  }

  /*-----------------------------------------------------------------------*/
  /* Process insuppressible requests.                                      */
  /*-----------------------------------------------------------------------*/

  if (!bSuppressibleRequest)
  {
    /*
     * We disable the source for the last insuppressible clock even if
     * another suppressible clock is still enabled because we need to keep
     * reference counting within the source.
     */
    if (pClockDomain->nReferenceCount == 1)
    {
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pClockDomain->pSource, FALSE, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * If there is no suppressible reference count, just remove the voltage
       * request. If there is a suppressible reference count, then update the
       * voltage request to be suppressible.
       */
      if (!CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST) &&
          pClockDomain->pActiveMuxConfig != NULL)
      {
        if (pClockDomain->nReferenceCountSuppressible == 0)
        {
          VRegRequest.eVRegLevel = RAIL_VOLTAGE_LEVEL_OFF;
          VRegRequest.bSuppressible = FALSE;
        }
        else
        {
          VRegRequest.eVRegLevel = pClockDomain->pActiveMuxConfig->eVRegLevel;
          VRegRequest.bSuppressible = TRUE;
        }

        Clock_VoltageRequest(pClockDomain->pRail, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Decrement insuppressible reference count.
     */
    if (pClockDomain->nReferenceCount > 0)
    {
      pClockDomain->nReferenceCount--;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Process suppressible request.                                         */
  /*-----------------------------------------------------------------------*/

  else
  {
    /*
     * We disable the source for the last suppressible clock even if
     * another insuppressible clock is still enabled because we need to
     * keep reference counting within the source.
     */
    if (pClockDomain->nReferenceCountSuppressible == 1)
    {
      if (pClockDomain->pSource != NULL)
      {
        eResult = Clock_DisableSourceInternal(pDrvCtxt, pClockDomain->pSource, TRUE, FALSE);
        if (eResult != DAL_SUCCESS)
        {
          return eResult;
        }
      }

      /*
       * Only remove the voltage request if there are no insuppressible
       * reference counts.
       */
      if (pClockDomain->nReferenceCount == 0 &&
          !CLOCK_FLAG_IS_SET(pClockDomain, DOMAIN_HOLD_VREG_REQUEST))
      {
        VRegRequest.eVRegLevel = RAIL_VOLTAGE_LEVEL_OFF;
        VRegRequest.bSuppressible = TRUE;
        Clock_VoltageRequest(pClockDomain->pRail, &pClockDomain->VRegRequest, &VRegRequest);
      }
    }

    /*
     * Decrement suppressible reference count.
     */
    if (pClockDomain->nReferenceCountSuppressible > 0)
    {
      pClockDomain->nReferenceCountSuppressible--;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_DisableClockDomain */


/* =========================================================================
**  Function : Clock_EnableClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableClock
(
  ClockDALClientCtxt *pClientCtxt,
  ClockNodeType      *pClock
)
{
  DALResult        eResult;
  boolean          bIsOn;
  uint32           nTimeout = 0;
  ClockDrvCtxt    *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the clock handle is valid */
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the clock */
  if (pClock->nReferenceCountTotal == 0)
  {
    /* Enable the domain first */
    eResult =
      Clock_EnableClockDomain(pDrvCtxt, pClock->pDomain, pClock->nFlags);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }

    /* Enable the clock */
    HAL_clk_EnableClock (&pClock->HALClock);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 1)", pClock->szName);
    }

    /* Determine whether this clock supports "IsOn" API functionality */
    if(HAL_clk_IsClockSupported (&pClock->HALClock, HAL_CLK_CLOCK_API_ISON))
    {
      bIsOn = HAL_clk_IsClockOn(&pClock->HALClock);

      /* Wait for the clock to turn ON */
      while (!bIsOn && nTimeout++ < 150)
      {
        DALSYS_BusyWait(1);
        bIsOn = HAL_clk_IsClockOn(&pClock->HALClock);
      }

      /* Log a warning if the clock does not turn ON */
      if(!bIsOn)
      {
        DALSYS_LogEvent (
          DALDEVICEID_CLOCK, DALSYS_LOGEVENT_WARNING,
            "Unable to turn ON clock: %s.", pClock->szName);
      }
    }
  }

  /* Increment the reference count for the requesting client. */
  pClock->nReferenceCount[pClientCtxt->CtxtInfo.nIdx]++;
  pClock->nReferenceCountTotal++;
  pClientCtxt->CtxtInfo.TotalCtxtReferences.nClockRefs++;

  /* Log clock enable event */
  Clock_SWEvent(
    CLOCK_EVENT_CLOCK_STATUS,
    3,
    pClock->nUniqueId,
    1, pClock->nReferenceCountTotal);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_EnableClock */


/* =========================================================================
**  Function : Clock_DisableClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_DisableClock
(
  ClockDALClientCtxt  *pClientCtxt,
  ClockNodeType       *pClock
)
{
  DALResult        eResult;
  ClockDrvCtxt    *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the clock handle is valid */
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*
   * If the reference count from the client is 1 and there are no other referneces,
   * we need to disable the clock. Ignore if the DO_NOT_DISABLE bits are set.
   */
  if ((pClock->nReferenceCount[pClientCtxt->CtxtInfo.nIdx] == 1) &&
      (pClock->nReferenceCountTotal == 1) &&
      !CLOCK_FLAG_IS_SET(pClock, DO_NOT_DISABLE) &&
      !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_CLOCKS))
  {
    /* Disable the clock */
    HAL_clk_DisableClock (&pClock->HALClock);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 0)", pClock->szName);
    }

    /* Disable the domain */
    eResult =
      Clock_DisableClockDomain(pDrvCtxt, pClock->pDomain, pClock->nFlags);

    if (eResult != DAL_SUCCESS)
    {
      DALCLOCK_FREE(pDrvCtxt);
      return DAL_ERROR_INTERNAL;
    }

  }

  /* Decrement the reference count for this client. */
  if (pClock->nReferenceCount[pClientCtxt->CtxtInfo.nIdx] > 0)
  {
    pClock->nReferenceCount[pClientCtxt->CtxtInfo.nIdx]--;
  }

  /* Decrement the total reference counts for this clock. */
  if (pClock->nReferenceCountTotal > 0)
  {
    pClock->nReferenceCountTotal--;
  }

  /* Mirror the total client clock references. */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nClockRefs > 0)
  {
    pClientCtxt->CtxtInfo.TotalCtxtReferences.nClockRefs--;
  }

  /* Log clock disable event */
  Clock_SWEvent(
    CLOCK_EVENT_CLOCK_STATUS,
    3,
    pClock->nUniqueId,
    0, pClock->nReferenceCountTotal);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_DisableClock */


/* =========================================================================
**  Function : Clock_FindClockConfig
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_FindClockConfig
(
  ClockDomainNodeType *pDomain,
  uint32               nFreqHz,
  ClockFrequencyType   eMatch,
  ClockMuxConfigType **pMatchingConfig
)
{
  uint32              nAtLeastFrequency = 0xFFFFFFFF;
  ClockMuxConfigType *pAtLeastConfig = NULL;
  uint32              nAtMostFrequency = 0;
  ClockMuxConfigType *pAtMostConfig = NULL;
  ClockMuxConfigType *pBSPConfig;
  uint32              i;

  /* Make sure the list of configurations isn't empty */
  if (pDomain == NULL || pMatchingConfig == NULL)
  {
    return DAL_ERROR;
  }

  /* Loop over all configurations */
  for (i = 0; i < pDomain->nBSPLen; i++)
  {
    pBSPConfig = &pDomain->aBSP[i];

    /*
     * Skip configurations that aren't for this chip or version.
     */
    if (!Clock_IsBSPSupported(&pBSPConfig->HWVersion))
    {
      continue;
    }

    /* Stop if we find an exact match */
    if (pBSPConfig->nFreqHz == nFreqHz)
    {
      *pMatchingConfig = pBSPConfig;
      return DAL_SUCCESS;
    }

    /* Look for the lowest at-least frequency */
    if ((pBSPConfig->nFreqHz > nFreqHz) &&
        (pBSPConfig->nFreqHz < nAtLeastFrequency))
    {
      nAtLeastFrequency = pBSPConfig->nFreqHz;
      pAtLeastConfig = pBSPConfig;
    }
    /* Look for the highest at-most frequency */
    else if ((pBSPConfig->nFreqHz < nFreqHz) &&
             (pBSPConfig->nFreqHz > nAtMostFrequency))
    {
      nAtMostFrequency = pBSPConfig->nFreqHz;
      pAtMostConfig = pBSPConfig;
    }
  }

  /* Select a config based on the match criteria */
  switch (eMatch)
  {
    case CLOCK_FREQUENCY_HZ_AT_LEAST:
      if (pAtLeastConfig != NULL)
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_AT_MOST:
      if (pAtMostConfig != NULL)
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      break;

    case CLOCK_FREQUENCY_HZ_CLOSEST:
      if ((pAtLeastConfig != NULL) && (pAtMostConfig == NULL))
      {
        *pMatchingConfig = pAtLeastConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtMostConfig != NULL) && (pAtLeastConfig == NULL))
      {
        *pMatchingConfig = pAtMostConfig;
        return DAL_SUCCESS;
      }
      else if ((pAtLeastConfig != NULL) && (pAtMostConfig != NULL))
      {
        /* Select the closest match, select the lowest in case of a tie */
        if ((nAtLeastFrequency - nFreqHz) < (nFreqHz - nAtMostFrequency))
        {
          *pMatchingConfig = pAtLeastConfig;
          return DAL_SUCCESS;
        }
        else
        {
          *pMatchingConfig = pAtMostConfig;
          return DAL_SUCCESS;
        }
      }
      break;

    default:
      break;
  }

  /* No match was found */
  *pMatchingConfig = NULL;

  return DAL_ERROR;

} /* END Clock_FindClockConfig */


/* =========================================================================
**  Function : Clock_SetClockConfig
** =========================================================================*/
/*
  See ClockDriver.h.
*/

DALResult Clock_SetClockConfig
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDomainNodeType *pDomain,
  ClockMuxConfigType  *pNewConfig
)
{
  ClockSourceNodeType       *pNewSource;
  DALResult                  eResult;
  boolean                    bVoltageRequest;
  boolean                    bSlewing;
  boolean                    bStub;
  uint32                     nRefCountTotal;
  ClockVRegRequestType       VRegRequest;
  const ClockMuxConfigType  *pActiveConfig;

  /* Validate parameters */
  if (pDomain == NULL || pNewConfig == NULL)
  {
    return DAL_ERROR;
  }

  if (CLOCK_FLAG_IS_SET(pDomain, STUB_HW_RUMI) &&
      (DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI ||
       DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_VIRTIO))
  {
    bStub = TRUE;
  }
  else
  {
    bStub = FALSE;
  }

  nRefCountTotal = pDomain->nReferenceCount + pDomain->nReferenceCountSuppressible;
  pActiveConfig = pDomain->pActiveMuxConfig;

  /*
   * If the domain is currently disabled, then just update the active
   * configuration without updating HW since disabled domains are
   * parked in a safe configuration. The new frequency will be configured
   * when enabling the domain.
   */
  if (nRefCountTotal == 0)
  {
    pDomain->pSource = pNewConfig->pSource;
    pDomain->pActiveMuxConfig = pNewConfig;
    if (pDomain->pSource && pNewConfig->pSourceFreqConfig)
    {
      pDomain->pSource->pActiveFreqConfig = pNewConfig->pSourceFreqConfig;
    }

    return DAL_SUCCESS;
  }

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest = !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);

  /*
   * Voltage requests to make if necessary.
   */
  VRegRequest.eVRegLevel = pNewConfig->eVRegLevel;
  VRegRequest.bSuppressible = (pDomain->nReferenceCount == 0);

  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) &&
      (pNewConfig->eVRegLevel > pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDomain->pRail, &pDomain->VRegRequest, &VRegRequest);
  }

  pNewSource = pNewConfig->pSource;

  bSlewing =
    (pActiveConfig &&
     pNewConfig->pSourceFreqConfig != pActiveConfig->pSourceFreqConfig &&
     pNewSource == pDomain->pSource);
  if (bSlewing)
  {
    /*
     * Slewing to different PLL outputs isn't supported, since the driver
     * doesn't know the divider configurations for the dividers based on
     * the mux select in the domain and thus can't determine proper sequencing.
     */
    if (pNewConfig->HALConfig.nMuxSel != pActiveConfig->HALConfig.nMuxSel)
    {
      return DAL_ERROR_INTERNAL;
    }

    /*
     * If the domain divider is increasing, then it needs to be updated
     * before slewing the PLL to prevent temporarily slewing above
     * the target frequency.
     */
    if (!CLOCK_FLAG_IS_SET(pDomain, DFS_CONTROLLED) &&
        pNewConfig->HALConfig.nDiv2x > pActiveConfig->HALConfig.nDiv2x &&
        !bStub)
    {
      HAL_clk_ConfigClockMux(&pDomain->HALDomain, &pNewConfig->HALConfig);
    }
  }

  /*
   * Configure the source if required.
   */
  if (pNewSource != NULL && pNewConfig->pSourceFreqConfig != NULL)
  {
    eResult = Clock_ConfigSource(
      pDrvCtxt, pNewSource, pNewConfig->pSourceFreqConfig);
    if (eResult != DAL_SUCCESS)
    {
      // TODO error!
      return eResult;
    }
  }

  /*
   * If the domain is active, and the new source is different from the
   * current one then we enable the new source.
   */
  if (pDomain->pSource != pNewSource &&
      pNewSource != NULL)
  {
    /*
     * If there are insuppressible clocks enabled in this clock domain then we
     * need to make an insuppressible request on the new source.
     */
    if (pDomain->nReferenceCount > 0)
    {
      eResult = Clock_EnableSourceInternal(pDrvCtxt, pNewSource, FALSE);
      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * If there are suppressible clocks enabled in this clock domain then we
     * need to make a suppressible request on the new source.
     */
    if (pDomain->nReferenceCountSuppressible > 0)
    {
      eResult = Clock_EnableSourceInternal(pDrvCtxt, pNewSource, TRUE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }
  }

  /*
   * Configure the clock MUX
   */
  if (!CLOCK_FLAG_IS_SET(pDomain, DFS_CONTROLLED) &&
      (!bSlewing ||
       pNewConfig->HALConfig.nDiv2x <= pActiveConfig->HALConfig.nDiv2x) &&
      !bStub)
  {
    HAL_clk_ConfigClockMux(&pDomain->HALDomain, &pNewConfig->HALConfig);
  }

  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) &&
      (pNewConfig->eVRegLevel < pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDomain->pRail, &pDomain->VRegRequest, &VRegRequest);
  }

  /*
   * If the domain is active, and the new source is different from the
   * current then we disable the old source.
   */
  if (pDomain->pSource != pNewSource &&
      pDomain->pSource != NULL)
  {
    /*
     * If there are insuppressible clocks enabled in this clock domain then we
     * need to remove the insuppressible request on the old source.
     */
    if (pDomain->nReferenceCount > 0)
    {
      eResult = Clock_DisableSourceInternal(pDrvCtxt, pDomain->pSource, FALSE, FALSE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }

    /*
     * If there are suppressible clocks enabled in this clock domain then we
     * need to remove the suppressible request on the old source.
     */
    if (pDomain->nReferenceCountSuppressible > 0)
    {
      eResult = Clock_DisableSourceInternal(pDrvCtxt, pDomain->pSource, TRUE, FALSE);

      if (eResult != DAL_SUCCESS)
      {
        return eResult;
      }
    }
  }

  /* Link the domain node to the new source node and update the active config */
  pDomain->pSource = pNewSource;
  pDomain->pActiveMuxConfig = pNewConfig;
  return DAL_SUCCESS;

} /* END Clock_SetClockConfig */


/* =========================================================================
**  Function : Clock_SetClockFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetClockFrequency
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockNodeType      *pClock,
  uint32              nFreq,
  ClockFrequencyType  eMatch,
  uint32             *pnResultFreq
)
{
  ClockDomainNodeType *pDomain;
  ClockMuxConfigType  *pNewConfig = NULL;
  DALResult            eResult;
  uint32               nFreqHz;
  uint32               nMult;
  uint32               nResultFreq;

  /* Initialize the output parameter */
  if (pnResultFreq != NULL)
  {
    *pnResultFreq = 0;
  }

  /* Make sure the clock handle and domain are valid and supports frequency configuration. */
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDomain == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }
  
  if (pClock->pDomain->aBSP == NULL)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  pDomain = pClock->pDomain;

  /* Convert to Hz if necessary. */
  if (eMatch >= CLOCK_FREQUENCY_MHZ_AT_LEAST)
  {
    nMult = 1000000;
    nFreqHz = nFreq * 1000000;
    eMatch -= CLOCK_FREQUENCY_MHZ_AT_LEAST;
  }
  else if (eMatch >= CLOCK_FREQUENCY_KHZ_AT_LEAST)
  {
    nMult = 1000;
    nFreqHz = nFreq * 1000;
    eMatch -= CLOCK_FREQUENCY_KHZ_AT_LEAST;
  }
  else
  {
    nMult = 1;
    nFreqHz = nFreq;
  }

  /* Look for a valid configuration */
  eResult = Clock_FindClockConfig (pDomain, nFreqHz, eMatch,
                                   &pNewConfig);

  if ((eResult != DAL_SUCCESS) || (pNewConfig == NULL))
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the new config is the same as the active config, we're done */
  nResultFreq = 0;
  if (pNewConfig != pDomain->pActiveMuxConfig)
  {
    eResult = Clock_SetClockConfig(pDrvCtxt, pDomain, pNewConfig);

    if (eResult == DAL_SUCCESS)
    {
      nResultFreq = pNewConfig->nFreqHz / nMult;

      if (CLOCK_FLAG_IS_SET(pClock, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pClock->szName, pNewConfig->nFreqHz);
      }

      /*
       * Only send out SW event when successful.
       */
      Clock_SWEvent(
        CLOCK_EVENT_CLOCK_FREQ,
        2,
        pClock->nUniqueId,
        ((nResultFreq) * nMult) );
    }
    else
    {
      eResult = DAL_ERROR_INTERNAL;
    }
  }
  else
  {
    nResultFreq = pDomain->pActiveMuxConfig->nFreqHz;
  }

  if (pnResultFreq != NULL)
  {
    *pnResultFreq = nResultFreq;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_SetClockFrequency */


/* =========================================================================
**  Function : Clock_SetClockDivider
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetClockDivider
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  uint32         nDivider
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure the hardware divider.                                       */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  HAL_clk_ConfigClockDivider(&pClock->HALClock, nDivider);

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pClock, LOG_CONFIG_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_CONFIG_CHANGE))
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                      "%s (Clock Divider: %lu)",
                      pClock->szName, nDivider);
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SetClockDivider */


/* =========================================================================
**  Function : Clock_InvertClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_InvertClock
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  boolean        bInvert
)
{
  DALResult eResult = DAL_ERROR_NOT_SUPPORTED;

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure the clock inversion if supported.                           */
  /*-----------------------------------------------------------------------*/

  if (TRUE == HAL_clk_IsClockSupported(&pClock->HALClock, HAL_CLK_CLOCK_API_INVERT))
  {
    DALCLOCK_LOCK(pDrvCtxt);

    HAL_clk_InvertClock(&pClock->HALClock, bInvert);

    eResult = DAL_SUCCESS;

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_INVERT_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_INVERT_CHANGE))
    {
      ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
          "%s (Inverted: %lu)",
          pClock->szName, (uint32)bInvert);
    }

    DALCLOCK_FREE(pDrvCtxt);
  }

  return eResult;

} /* END Clock_InvertClock */


/* =========================================================================
**  Function : Clock_ResetClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ResetClock
(
  ClockDrvCtxt   *pDrvCtxt,
  ClockNodeType  *pClock,
  ClockResetType  eReset
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Reset clock.                                                          */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (eReset == CLOCK_RESET_ASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(&pClock->HALClock, TRUE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Reset: 1)", pClock->szName);
    }
  }

  if (eReset == CLOCK_RESET_DEASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(&pClock->HALClock, FALSE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Reset: 0)", pClock->szName);
    }
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_ResetClock */


/* =========================================================================
**  Function : Clock_ResetClock_STM
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ResetClock_STM
(
  ClockDrvCtxt   *pDrvCtxt,
  ClockNodeType  *pClock,
  ClockResetType  eReset
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Reset clock.                                                          */
  /*-----------------------------------------------------------------------*/

  if (eReset == CLOCK_RESET_ASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(&pClock->HALClock, TRUE);
  }
  if (eReset == CLOCK_RESET_DEASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClock(&pClock->HALClock, FALSE);
  }

  return DAL_SUCCESS;

} /* END Clock_ResetClock_STM */


/* =========================================================================
**  Function : Clock_ResetClockBranch
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ResetClockBranch
(
  ClockDrvCtxt   *pDrvCtxt,
  ClockNodeType  *pClock,
  ClockResetType  eReset
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Reset clock.                                                          */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  if (eReset == CLOCK_RESET_ASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClockBranch(&pClock->HALClock, TRUE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (ResetBranch: 1)", pClock->szName);
    }
  }

  if (eReset == CLOCK_RESET_DEASSERT || eReset == CLOCK_RESET_PULSE)
  {
    HAL_clk_ResetClockBranch(&pClock->HALClock, FALSE);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pClock, LOG_RESET) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_RESET))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (ResetBranch: 0)", pClock->szName);
    }
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_ResetClockBranch */


/* =========================================================================
**  Function : Clock_IsClockEnabled
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_IsClockEnabled
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  boolean       *pbIsEnabled
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check both pointer arguments.                                  */
  /*-----------------------------------------------------------------------*/

  if (pbIsEnabled == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }
  
  /*-----------------------------------------------------------------------*/
  /* Initialize the output argument here.                                  */
  /*-----------------------------------------------------------------------*/

  *pbIsEnabled = FALSE;
  
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  *pbIsEnabled = HAL_clk_IsClockEnabled(&pClock->HALClock);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_IsClockEnabled */


/* =========================================================================
**  Function : Clock_IsClockOn
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_IsClockOn
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  boolean       *pbIsOn
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check both pointer arguments.                                  */
  /*-----------------------------------------------------------------------*/

  if (pbIsOn == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the output argument here.                                  */
  /*-----------------------------------------------------------------------*/

  *pbIsOn = FALSE;
  
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Get state from the HAL.                                               */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  *pbIsOn = HAL_clk_IsClockOn(&pClock->HALClock);

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_IsClockOn */


/* =========================================================================
**  Function : Clock_GetClockFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetClockFrequency
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  uint32        *pnFrequencyHz
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL || pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDomain->pActiveMuxConfig != NULL)
  {
    *pnFrequencyHz = pClock->pDomain->pActiveMuxConfig->nFreqHz;
    return DAL_SUCCESS;
  }

  *pnFrequencyHz = 0;
  return DAL_ERROR_NOT_INITIALIZED;

} /* END Clock_GetClockFrequency */


/* =========================================================================
**  Function : Clock_SelectExternalSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SelectExternalSource
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  uint32         nFreqHz,
  uint32         nSource,
  uint32         nDiv2x,
  uint32         nM,
  uint32         nN,
  uint32         n2D
)
{
  HAL_clk_ClockMuxConfigType  Config;
  ClockDomainNodeType        *pDomain;
  rail_voltage_level          eVRegLevel;
  boolean                     bVoltageRequest;
  ClockVRegRequestType        VRegRequest;
  DALResult                   eResult;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the arguments.                                           */
  /*-----------------------------------------------------------------------*/
  
  /* TODO: only allow nSource that is external? */

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDomain == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }
  
  DALCLOCK_LOCK(pDrvCtxt);

  pDomain = pClock->pDomain;

  /*
   * Find the voltage corner data for the given frequency, if specified.
   * If the voltage corner data. or the frequency is not specified, then
   * assume the voltage does not matter.
   */
  if (nFreqHz != 0)
  {
    eResult = Clock_GetRequiredCorner(pDomain->aFMax, nFreqHz, &eVRegLevel);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
  }
  else
  {
    /* TODO: require clients to specify frequency? */
    eVRegLevel = RAIL_VOLTAGE_LEVEL_OFF;
  }

  /*
   * Determine if we should make a voltage request.  We do so if this
   * domain is not configured to make requests when enabled, or if it
   * is thus configured and is currently enabled.
   */
  bVoltageRequest =
    (pDomain->nReferenceCount > 0 || pDomain->nReferenceCountSuppressible > 0) &&
    !CLOCK_FLAG_IS_SET(pDomain, DOMAIN_HOLD_VREG_REQUEST);

  /*
   * Voltage requests to make if necessary.
   */
    VRegRequest.bSuppressible = (pDomain->nReferenceCount == 0);
  VRegRequest.eVRegLevel = eVRegLevel;

  /*
   * If we are increasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) &&
      (eVRegLevel > pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDomain->pRail, &pDomain->VRegRequest, &VRegRequest);
  }

  /*-----------------------------------------------------------------------*/
  /* Define the mux configuration.                                         */
  /*-----------------------------------------------------------------------*/

  Config.nMuxSel = nSource;
  Config.nDiv2x  = nDiv2x;
  Config.nM      = nM;
  Config.nN      = nN;

  /*-----------------------------------------------------------------------*/
  /* Configure the clock to external source.                               */
  /*-----------------------------------------------------------------------*/

  HAL_clk_ConfigClockMux(&pDomain->HALDomain, &Config);

  /*
   * If we are decreasing the voltage requirement, do so now.
   */
  if ((bVoltageRequest == TRUE) &&
      (eVRegLevel < pDomain->VRegRequest.eVRegLevel))
  {
    Clock_VoltageRequest(pDomain->pRail, &pDomain->VRegRequest, &VRegRequest);
  }

  /*-----------------------------------------------------------------------*/
  /* Update the source tree.                                               */
  /*-----------------------------------------------------------------------*/

  if (pDomain->pSource != NULL && pDomain->nReferenceCount > 0)
  {
    Clock_DisableSourceInternal(
      pDrvCtxt, pDomain->pSource, CLOCK_FLAG_IS_SET(pDomain, SUPPRESSIBLE), FALSE);
  }

  pDomain->pSource = NULL;
  pDomain->pActiveMuxConfig = NULL;
  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SelectExternalSource */


/* =========================================================================
**  Function : Clock_GetDebugMuxId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetDebugMuxId
(
  ClockDrvCtxt *pDrvCtxt,
  const char   *szDebugMux,
  ClockDebugMuxIdType  *pnId
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  uint32   nDebugMuxIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szDebugMux || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Search the array of clock nodes, break if we find a match             */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nDebugMuxIndex = 0; nDebugMuxIndex < pBSP->nNumDebugMuxes; nDebugMuxIndex++)
  {
    if (strcmp (szDebugMux, pBSP->aDebugMuxes[nDebugMuxIndex]->szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the clock node if found, otherwise return NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = CLOCK_MAKE_ID(CLOCK_ID_CATEGORY_DEBUGMUX, nDebugMuxIndex);
    return DAL_SUCCESS;
  }
  else
  {
    *pnId = CLOCK_ID_INVALID;
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetDebugMuxId */


/*=========================================================================
**  Function : DebugMux_GetDebugMuxNode
** =========================================================================*/
/**
  See DebugMuxDriver.h
*/

ClockDebugMuxType *Clock_GetDebugMuxNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDebugMuxIdType  nId
)
{
  ClockIdCategoryType nCat = CLOCK_GET_ID_CATEGORY(nId);
  uint32              nIdx = CLOCK_GET_ID_INDEX(nId);

  if (nCat == CLOCK_ID_CATEGORY_DEBUGMUX &&
      nIdx < pDrvCtxt->pBSP->nNumDebugMuxes)
  {
    return pDrvCtxt->pBSP->aDebugMuxes[nIdx];
  }

  return NULL;

} /* END Clock_GetDebugMuxNode */


/*=========================================================================
**  Function : Clock_GetDebugMuxInputDiv
** =========================================================================*/
/**
  Get the divider applied to the specific mux input.

  @param *pDebugMux        [in] -- Pointer to debug mux.
  @param  nMuxSel          [in] -- Mux select value for which to return the divider.

  @return
  Divide value applied to input specified by nMuxSel.

  @dependencies
  None.
*/

static uint32 Clock_GetDebugMuxInputDiv
(
  ClockDebugMuxType *pDebugMux,
  uint32             nMuxSel
)
{
  ClockDebugMuxInputDivType *pInputDiv;

  if (!pDebugMux || !pDebugMux->anInputDivs)
  {
    return 1;
  }

  for (pInputDiv = pDebugMux->anInputDivs;
       pInputDiv->nMuxSel != CLOCK_DEBUG_MUX_INPUT_DIV_END;
       pInputDiv++)
  {
    if (pInputDiv->nMuxSel == nMuxSel)
    {
      return MAX(1, pInputDiv->nDivider);
    }
  }

  return 1;

} /* END Clock_GetDebugMuxInputDiv */


/* =========================================================================
**  Function : Clock_CalcFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_CalcFrequency
(
  ClockDrvCtxt      *pDrvCtxt,
  ClockDebugMuxType *pDebugMux,
  uint32             nDebugMuxSel,
  uint32            *pnFrequencyHz
)
{
  uint32             nMultiplier;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the arguments.                                           */
  /*-----------------------------------------------------------------------*/

  if (pDebugMux == NULL || pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure requested mux.                                              */
  /*-----------------------------------------------------------------------*/

  nMultiplier = MAX(1, pDebugMux->nDivider);
  nMultiplier *= Clock_GetDebugMuxInputDiv(pDebugMux, nDebugMuxSel);

  HAL_clk_ConfigDebugMux(&pDebugMux->HALDebugMux, nDebugMuxSel);

  /*-----------------------------------------------------------------------*/
  /* Configure parent muxes.                                               */
  /*-----------------------------------------------------------------------*/

  for (; pDebugMux->pParent != NULL; pDebugMux = pDebugMux->pParent)
  {
    nMultiplier *= MAX(1, pDebugMux->pParent->nDivider);
    nMultiplier *= Clock_GetDebugMuxInputDiv(pDebugMux->pParent,
      pDebugMux->nParentSel);

    HAL_clk_ConfigDebugMux(&pDebugMux->pParent->HALDebugMux,
      pDebugMux->nParentSel);
  }

  /*-----------------------------------------------------------------------*/
  /* Calculate frequency.                                                  */
  /*-----------------------------------------------------------------------*/

  HAL_clk_CalcDebugMuxFreq(&pDebugMux->HALDebugMux, pnFrequencyHz);
  *pnFrequencyHz *= nMultiplier;

  return DAL_SUCCESS;

} /* END Clock_CalcFrequency */


/* =========================================================================
**  Function : Clock_CalcClockFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_CalcClockFrequency
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  uint32        *pnFrequencyHz
)
{
  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pClock->pDebugMux == NULL ||
      pClock->HALClock.nDebugMuxSel == HAL_CLK_NO_DEBUG_MUX_SEL)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  return Clock_CalcFrequency(
    pDrvCtxt,
    pClock->pDebugMux,
    pClock->HALClock.nDebugMuxSel,
    pnFrequencyHz);

} /* END Clock_CalcClockFrequency */


/* =========================================================================
**  Function : Clock_GetPowerDomainId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetPowerDomainId
(
  ClockDrvCtxt           *pDrvCtxt,
  const char             *szPowerDomain,
  ClockPowerDomainIdType *pnPowerDomainId
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  uint32   nPowerDomainIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szPowerDomain || !pnPowerDomainId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Search the array of power domain nodes, break if we find a match      */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nPowerDomainIndex = 0; nPowerDomainIndex < pBSP->nNumPowerDomains; nPowerDomainIndex++)
  {
    if (strcmp (szPowerDomain, pBSP->aPowerDomains[nPowerDomainIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the power domain node if found, otherwise NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnPowerDomainId =
      CLOCK_MAKE_ID(CLOCK_ID_CATEGORY_POWERDOMAIN, nPowerDomainIndex);
    return DAL_SUCCESS;
  }
  else
  {
    *pnPowerDomainId = CLOCK_ID_INVALID;
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetPowerDomainId */


/*=========================================================================
**  Function : Clock_GetPowerDomainNode
** =========================================================================*/
/**
  See ClockDriver.h
*/

ClockPowerDomainNodeType *Clock_GetPowerDomainNode
(
  ClockDrvCtxt            *pDrvCtxt,
  ClockPowerDomainIdType   nId
)
{
  ClockIdCategoryType nCat = CLOCK_GET_ID_CATEGORY(nId);
  uint32              nIdx = CLOCK_GET_ID_INDEX(nId);

  if (nCat == CLOCK_ID_CATEGORY_POWERDOMAIN &&
      nIdx < pDrvCtxt->pBSP->nNumPowerDomains)
  {
    return &pDrvCtxt->pBSP->aPowerDomains[nIdx];
  }

  return NULL;

} /* END Clock_GetPowerDomainNode */


/* =========================================================================
**  Function : Clock_EnablePowerDomain
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnablePowerDomain
(
  ClockDALClientCtxt        *pClientCtxt,
  ClockPowerDomainNodeType  *pPowerDomain
)
{
  ClockDrvCtxt             *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /* If the reference count is zero, we need to enable the power domain */
  if (pPowerDomain->nReferenceCountTotal == 0)
  {
    HAL_clk_EnablePowerDomain (&pPowerDomain->HALPowerDomain);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pPowerDomain, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_POWER_DOMAIN_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 1)", pPowerDomain->szName);
    }
  }

  /* Increment the reference counts for this client, clock, and context. */
  pPowerDomain->nReferenceCount[pClientCtxt->CtxtInfo.nIdx]++;
  pPowerDomain->nReferenceCountTotal++;
  pClientCtxt->CtxtInfo.TotalCtxtReferences.nPowerDomainRefs++;

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_EnablePowerDomain */


/* =========================================================================
**  Function : Clock_DisablePowerDomain
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_DisablePowerDomain
(
  ClockDALClientCtxt        *pClientCtxt,
  ClockPowerDomainNodeType  *pPowerDomain
)
{
  ClockDrvCtxt             *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the power domain handle is valid */
  if (pPowerDomain == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*
   * If the total reference counts is one and it is for this client context,
     we need to disable the power domain.  Ignore if the DO_NOT_DISABLE bits are set.
   */
  if ((pPowerDomain->nReferenceCount[pClientCtxt->CtxtInfo.nIdx] == 1) &&
      (pPowerDomain->nReferenceCountTotal == 1) &&
      !CLOCK_FLAG_IS_SET(pPowerDomain, DO_NOT_DISABLE) &&
      !CLOCK_GLOBAL_FLAG_IS_SET(DO_NOT_DISABLE_POWER_DOMAINS))
  {
    HAL_clk_DisablePowerDomain (&pPowerDomain->HALPowerDomain);

    /* Logging */
    if (CLOCK_FLAG_IS_SET(pPowerDomain, LOG_STATE_CHANGE) ||
        CLOCK_GLOBAL_FLAG_IS_SET(LOG_POWER_DOMAIN_STATE_CHANGE))
    {
      ULOG_RT_PRINTF_1 (pDrvCtxt->hClockLog,
                        "%s (Enabled: 0)", pPowerDomain->szName);
    }
  }

  /* Decrement the reference count */
  if (pPowerDomain->nReferenceCount[pClientCtxt->CtxtInfo.nIdx] > 0)
  {
    pPowerDomain->nReferenceCount[pClientCtxt->CtxtInfo.nIdx]--;
  }

  /* Decrement the total domain refrence counts. */
  if (pPowerDomain->nReferenceCountTotal > 0)
  {
    pPowerDomain->nReferenceCountTotal--;
  }

  /* Decrement the total number of power domain references for this client context. */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nPowerDomainRefs > 0)
  {
    pClientCtxt->CtxtInfo.TotalCtxtReferences.nPowerDomainRefs--;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_DisablePowerDomain */


/* =========================================================================
**  Function : Clock_ConfigClock
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_ConfigClock
(
  ClockDrvCtxt     *pDrvCtxt,
  ClockNodeType    *pClock,
  ClockConfigType  eConfig
)
{
  DALResult eResult = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pClock == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALCLOCK_LOCK(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Since we have mapped the lower bits of HAL_clk_ClockConfigType to be  */
  /* the same as ClockConfigType, we can just pass the value directly to   */
  /* HAL layer.  Thus, only doing range check here.                        */
  /*-----------------------------------------------------------------------*/

  if(((uint32)eConfig > 0) && (eConfig < CLOCK_CONFIG_TOTAL))
  {
    HAL_clk_ConfigClock(&pClock->HALClock, (HAL_clk_ClockConfigType) eConfig);
  }
  else
  {
    eResult = DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  DALCLOCK_FREE(pDrvCtxt);

  return eResult;

} /* END Clock_ConfigClock */


/* =========================================================================
**  Function : Clock_LogState
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_LogState
(
  ClockDrvCtxt        *pDrvCtxt,
  ULogHandle           hULog,
  uint32               nFlags
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  ClockNodeType               *pClock;
  ClockPowerDomainNodeType    *pPowerDomain;
  uint32                       i, nFreqHz, nFreqHzConfig;
  int32                        nIsOn, nIsEnabled, nReset;
  boolean                      bIsEnabled, bIsOn, bCalcSupported;
  DALResult                    eResult;
  const char                  *szSource;

  if(hULog == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  ULOG_RT_PRINTF_0(hULog, ",CLOCK,ON,ENABLED,RESET,REF COUNT,FREQ CONFIG,FREQ CALC,VOLTAGE,SOURCE");

  /*------------------------------------------------------------------------*/
  /* Log clock state.                                                       */
  /*------------------------------------------------------------------------*/

  for (i = 0; i < pBSP->nNumClocks; i++)
  {
    pClock = &pBSP->aClocks[i];
    nIsEnabled = nIsOn = nReset = -1;
    nFreqHzConfig = 0;
    bCalcSupported = FALSE;

    if (CLOCK_FLAG_IS_SET(pClock, EXPLICIT_ACCESS_ONLY))
    {
      continue;
    }

    /*
     * Get enabled state.
     */
    if (HAL_clk_IsClockSupported(&pClock->HALClock, HAL_CLK_CLOCK_API_ISENABLED))
    {
      Clock_IsClockEnabled(pDrvCtxt, pClock, &bIsEnabled);
      nIsEnabled = bIsEnabled ? 1 : 0;
    }

    /*
     * Get ON state.
     */
    if (HAL_clk_IsClockSupported(&pClock->HALClock, HAL_CLK_CLOCK_API_ISON))
    {
      Clock_IsClockOn(pDrvCtxt, pClock, &bIsOn);
      nIsOn = bIsOn ? 1 : 0;
    }

    /*
     * Get reset state.
     */
    if (HAL_clk_IsClockSupported(&pClock->HALClock, HAL_CLK_CLOCK_API_RESET))
    {
      nReset = HAL_clk_IsClockReset(&pClock->HALClock);
    }

    /*
     * Get frequency.
     */
    if (pClock->pDomain->pActiveMuxConfig != NULL)
    {
      nFreqHzConfig = pClock->pDomain->pActiveMuxConfig->nFreqHz;
    }

    /*
     * Calculate frequency.
     */
    if ((nFlags & CLOCK_LOG_STATE_CLOCK_FREQUENCY) &&
        HAL_clk_IsClockSupported(&pClock->HALClock, HAL_CLK_CLOCK_API_CALCFREQ))
    {
      bCalcSupported = TRUE;
      Clock_CalcClockFrequency(pDrvCtxt, pClock, &nFreqHz);
    }

    /*
     * Get source.
     */
    if (pClock->pDomain->pSource)
    {
      szSource = pClock->pDomain->pSource->szName;
    }
    else
    {
      szSource = "";
    }

    /*
     * Log results.
     */
    if (bCalcSupported)
    {
      eResult = ULOG_RT_PRINTF_9(hULog, ",%s,%d,%d,%d,%d,%u,%u,%d,%s",
        pClock->szName, nIsOn, nIsEnabled, nReset, pClock->nReferenceCount,
        nFreqHzConfig, nFreqHz, pClock->pDomain->VRegRequest.eVRegLevel, szSource);
    }
    else
    {
      eResult = ULOG_RT_PRINTF_8(hULog, ",%s,%d,%d,%d,%d,%u,-1,%d,%s",
        pClock->szName, nIsOn, nIsEnabled, nReset, pClock->nReferenceCount,
        nFreqHzConfig, pClock->pDomain->VRegRequest.eVRegLevel, szSource);
    }

    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  /*------------------------------------------------------------------------*/
  /* Log power domain state.                                                */
  /*------------------------------------------------------------------------*/

  ULOG_RT_PRINTF_0(hULog, "");
  ULOG_RT_PRINTF_0(hULog, "");
  ULOG_RT_PRINTF_0(hULog, ",POWER DOMAIN,ON,ENABLED,REF COUNT");

  for (i = 0; i < pBSP->nNumPowerDomains; i++)
  {
    pPowerDomain = &pBSP->aPowerDomains[i];
    if (CLOCK_FLAG_IS_SET(pPowerDomain, EXPLICIT_ACCESS_ONLY))
    {
      continue;
    }

    bIsEnabled = HAL_clk_IsPowerDomainEnabled(&pBSP->aPowerDomains[i].HALPowerDomain);
    bIsOn = HAL_clk_IsPowerDomainOn(&pBSP->aPowerDomains[i].HALPowerDomain);

    eResult = ULOG_RT_PRINTF_4(hULog, ",%s,%d,%d,%d", pPowerDomain->szName, bIsOn,
      bIsEnabled, pPowerDomain->nReferenceCount);

    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }

  return DAL_SUCCESS;

} /* END Clock_LogState */


/* =========================================================================
**  Function : Clock_GetSourceId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetSourceId
(
  ClockDrvCtxt  *pDrvCtxt,
  const char    *szSource,
  SourceIdType  *pnId
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  uint32   nSourceIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szSource || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Search the array of source nodes, break if we find a match            */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nSourceIndex = 0; nSourceIndex < pBSP->nNumSources; nSourceIndex++)
  {
    if (strcmp (szSource, pBSP->aSources[nSourceIndex]->szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the source node if found, otherwise return NULL   */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = CLOCK_MAKE_ID(CLOCK_ID_CATEGORY_SOURCE, nSourceIndex);
    return DAL_SUCCESS;
  }
  else
  {
    *pnId = CLOCK_ID_INVALID;
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetSourceId */


/*=========================================================================
**  Function : Source_GetSourceNode
** =========================================================================*/
/**
  See SourceDriver.h
*/

ClockSourceNodeType *Clock_GetSourceNode
(
  ClockDrvCtxt  *pDrvCtxt,
  SourceIdType   nId
)
{
  ClockIdCategoryType nCat = CLOCK_GET_ID_CATEGORY(nId);
  uint32              nIdx = CLOCK_GET_ID_INDEX(nId);

  if (nCat == CLOCK_ID_CATEGORY_SOURCE && nIdx < pDrvCtxt->pBSP->nNumSources)
  {
    return pDrvCtxt->pBSP->aSources[nIdx];
  }

  return NULL;

} /* END Clock_GetSourceNode */


/* =========================================================================
**  Function : Clock_EnableSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_EnableSource
(
  ClockDALClientCtxt  *pClientCtxt,
  ClockSourceNodeType *pSource
)
{
  DALResult            eResult;
  ClockDrvCtxt        *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the source handle is valid */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*
   * Check if we should bypass the resource's dependencies and directly
   * configure the resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_LOCK(pDrvCtxt);
  }

  /*
   * If the external reference count is zero then we need to call the internal
   * enable source API.
   */
  if (pSource->nReferenceCountExternalTotal == 0)
  {
    eResult = Clock_EnableSourceInternal(pDrvCtxt, pSource, FALSE);
    if (eResult != DAL_SUCCESS)
    {
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
        DALCLOCK_FREE(pDrvCtxt);
      }
      return DAL_ERROR_INTERNAL;
    }
  }

  /* Increment the reference count */
  pSource->nReferenceCountExternal[pClientCtxt->CtxtInfo.nIdx]++;
  pSource->nReferenceCountExternalTotal++;
  pClientCtxt->CtxtInfo.TotalCtxtReferences.nSourceRefs++;

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_FREE(pDrvCtxt);
  }

  return DAL_SUCCESS;

} /* END Clock_EnableSource */


/* =========================================================================
**  Function : Clock_DisableSource
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_DisableSource
(
  ClockDALClientCtxt  *pClientCtxt,
  ClockSourceNodeType *pSource
)
{
  DALResult            eResult;
  ClockDrvCtxt        *pDrvCtxt = Clock_GetDrvCtxtNoLock();

  /* Make sure the source handle is valid */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*
   * Check if we should bypass the DAL driver and directly configure the
   * resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_LOCK(pDrvCtxt);
  }

  /*
   * If the external reference count is one then we need to call the internal
   * disable source API.
   */
  if ((pSource->nReferenceCountExternal[pClientCtxt->CtxtInfo.nIdx] == 1) && 
      (pSource->nReferenceCountExternalTotal == 1))
  {
    eResult = Clock_DisableSourceInternal(pDrvCtxt, pSource, FALSE, FALSE);
    if (eResult != DAL_SUCCESS)
    {
      if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
      {
        DALCLOCK_FREE(pDrvCtxt);
      }
      return DAL_ERROR_INTERNAL;
    }
  }

  /* Decrement the reference count */
  if (pSource->nReferenceCountExternal[pClientCtxt->CtxtInfo.nIdx] > 0)
  {
    pSource->nReferenceCountExternal[pClientCtxt->CtxtInfo.nIdx]--;
  }

  /* Decrement the total reference count */
  if (pSource->nReferenceCountExternalTotal > 0)
  {
    pSource->nReferenceCountExternalTotal--;
  }

  /* Decrement the total number of refernces from this client context. */
  if (pClientCtxt->CtxtInfo.TotalCtxtReferences.nSourceRefs > 0)
  {
    pClientCtxt->CtxtInfo.TotalCtxtReferences.nSourceRefs--;
  }

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_FREE(pDrvCtxt);
  }

  return DAL_SUCCESS;

} /* END Clock_DisableSource */


/* =========================================================================
**  Function : Clock_SetSourceFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetSourceFrequency
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32               nFreq,
  ClockFrequencyType   eMatch,
  uint32              *pnResultFreq
)
{
  const ClockSourceFreqConfigType  *pNewConfig = NULL;
  DALResult                   eResult;
  uint32                      nFreqHz;
  uint32                      nMult;
  uint32                      nResultFreq;

  /* Initialize output parameter */
  if (pnResultFreq != NULL)
  {
    *pnResultFreq = 0;
  }

  /* Make sure the source handle is valid and supports frequency configuration. */
  if (pSource == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /* Convert to Hz if necessary. */
  if (eMatch >= CLOCK_FREQUENCY_MHZ_AT_LEAST)
  {
    nMult = 1000000;
    nFreqHz = nFreq * 1000000;
    eMatch -= CLOCK_FREQUENCY_MHZ_AT_LEAST;
  }
  else if (eMatch >= CLOCK_FREQUENCY_KHZ_AT_LEAST)
  {
    nMult = 1000;
    nFreqHz = nFreq * 1000;
    eMatch -= CLOCK_FREQUENCY_KHZ_AT_LEAST;
  }
  else
  {
    nMult = 1;
    nFreqHz = nFreq;
  }

  /* Look for a valid configuration */
  eResult =
    Clock_FindSourceConfig(
      pSource,
      nFreqHz,
      eMatch,
      &pNewConfig);

  if ((eResult != DAL_SUCCESS) || (pNewConfig == NULL))
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  /*
   * Check if we should bypass the resource's dependencies and directly
   * configure the resource to the requested state at the HAL level.
   *
   * The clock driver provides this type of interface for resources whose
   * dependencies are implicitly met (XO, SVS voltage, etc.). In this case,
   * we bypass the DAL driver which ensures resource synchronization, and
   * initiates voltage updates.
   *
   * This is an optimization designed to avoid blocking on other threads
   * that are in the middle of these updates. The logic is that since the
   * current resource's requirements are already met, we can configure the
   * hardware directly and skip record keeping for this resource which
   * also requires grabbing the shared driver mutex.
   *
   * Note: clients must avoid making concurrent calls into the clock
   * driver DAL APIs for this resource, since the clock driver no
   * longer protects its state with a lock.
   */
  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_LOCK(pDrvCtxt);
  }

  nResultFreq = 0;

  /* If the new config is the same as the active config, we're done */
  if (pNewConfig != pSource->pActiveFreqConfig)
  {
    eResult = Clock_ConfigSource(pDrvCtxt, pSource, pNewConfig);
    if (eResult == DAL_SUCCESS)
    {
      nResultFreq = pNewConfig->nFreqHz / nMult;

      if (CLOCK_FLAG_IS_SET(pSource, LOG_FREQ_CHANGE) ||
          CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
      {
        ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                          "%s (Freq: %lu)",
                          pSource->szName, pNewConfig->nFreqHz);
      }

      /*
       * Only send out SW event when successful.
       */
      Clock_SWEvent(
        CLOCK_EVENT_SOURCE_FREQ,
        2,
        0, /* TODO: log source identifier */
        nResultFreq * nMult);
    }
    else
    {
      eResult = DAL_ERROR_INTERNAL;
    }
  }
  else
  {
    nResultFreq = pSource->pActiveFreqConfig->nFreqHz;
  }

  if (pnResultFreq != NULL)
  {
    *pnResultFreq = nResultFreq;
  }

  if (!CLOCK_FLAG_IS_SET(pSource, BYPASS_DEPENDENCIES))
  {
    DALCLOCK_FREE(pDrvCtxt);
  }

  return eResult;

} /* END Clock_SetSourceFrequency */


/* =========================================================================
**  Function : Clock_GetSourceFrequency
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetSourceFrequency
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockSourceNodeType *pSource,
  uint32              *pnFrequencyHz
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pSource == NULL || pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (pSource->pActiveFreqConfig != NULL)
  {
    *pnFrequencyHz = pSource->pActiveFreqConfig->nFreqHz;
    return DAL_SUCCESS;
  }

  *pnFrequencyHz = 0;
  return DAL_ERROR_NOT_INITIALIZED;

} /* END Clock_GetSourceFrequency */


/*=========================================================================
**  Function : Clock_IsBSPSupported
** =========================================================================*/
/**
 *
  See ClockDriver.h
*/

boolean Clock_IsBSPSupported
(
  const ClockHWVersionType *pHWVersion
)
{
  uint32                   nHWVersion, nHWMaxVersion, nHWMinVersion;
  DalChipInfoIdType 	   eChipId;
  const DalChipInfoIdType *aeChipInfoId;

  /*-----------------------------------------------------------------------*/
  /* Validate argument pointer is usable.                                  */
  /*-----------------------------------------------------------------------*/

  if (pHWVersion == NULL)
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate chip family. If not provided then we do not check.           */
  /*-----------------------------------------------------------------------*/

  if ( (pHWVersion->eChipInfoFamily != 0) &&
       (pHWVersion->eChipInfoFamily != DalChipInfo_ChipFamily()) )
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------*/
  /* Ensure the specific chip ID is supported.                             */
  /*-----------------------------------------------------------------------*/

  aeChipInfoId = pHWVersion->aeChipInfoId;

  if (aeChipInfoId != NULL)
  {
    eChipId = DalChipInfo_ChipId();

    while (*aeChipInfoId < DALCHIPINFO_NUM_IDS)
    {
      if (eChipId == *aeChipInfoId)
      {
        break;
      }

      aeChipInfoId++;
    }

    if (*aeChipInfoId == DALCHIPINFO_NUM_IDS)
    {
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Get the actual hardware revision.                                     */
  /*-----------------------------------------------------------------------*/

  nHWVersion = HAL_clk_GetChipVersion();

  nHWMinVersion =
    DALCHIPINFO_VERSION(pHWVersion->Min.nMajor, pHWVersion->Min.nMinor);
  nHWMaxVersion =
    DALCHIPINFO_VERSION(pHWVersion->Max.nMajor, pHWVersion->Max.nMinor);

  /*-----------------------------------------------------------------------*/
  /* No chip version data implies support in all HW versions.              */
  /*-----------------------------------------------------------------------*/

  if ( (nHWMinVersion == 0) && (nHWMaxVersion == 0) )
  {
    return TRUE;
  }

  /*-----------------------------------------------------------------------*/
  /* Validate the HW version falls within the min/max.                     */
  /*-----------------------------------------------------------------------*/

  if ( (nHWVersion >= nHWMinVersion) && (nHWVersion < nHWMaxVersion) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/* =========================================================================
**  Function : Clock_GetDividerId
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_GetDividerId
(
  ClockDrvCtxt        *pDrvCtxt,
  const char          *szDivider,
  ClockDividerIdType  *pnId
)
{
  ClockBSPType *pBSP = pDrvCtxt->pBSP;
  uint32   nDividerIndex;
  boolean  bFound;

  /*-----------------------------------------------------------------------*/
  /* Validate pointers are usable.                                         */
  /*-----------------------------------------------------------------------*/

  if (!szDivider || !pnId)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Search the array of clock nodes, break if we find a match             */
  /*-----------------------------------------------------------------------*/

  bFound = FALSE;

  for (nDividerIndex = 0; nDividerIndex < pBSP->nNumDividers; nDividerIndex++)
  {
    if (strcmp (szDivider, pBSP->aDividers[nDividerIndex].szName) == 0)
    {
      bFound = TRUE;
      break;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Return a pointer to the clock node if found, otherwise return NULL    */
  /*-----------------------------------------------------------------------*/

  if (bFound)
  {
    *pnId = CLOCK_MAKE_ID(CLOCK_ID_CATEGORY_DIVIDER, nDividerIndex);
    return DAL_SUCCESS;
  }
  else
  {
    *pnId = CLOCK_ID_INVALID;
    return DAL_ERROR_NOT_FOUND;
  }

} /* END Clock_GetDividerId */


/*=========================================================================
**  Function : Clock_GetDividerNode
** =========================================================================*/
/**
  See ClockDriver.h
*/

ClockDividerNodeType *Clock_GetDividerNode
(
  ClockDrvCtxt        *pDrvCtxt,
  ClockDividerIdType   nId
)
{
  ClockIdCategoryType nCat = CLOCK_GET_ID_CATEGORY(nId);
  uint32              nIdx = CLOCK_GET_ID_INDEX(nId);

  if (nCat == CLOCK_ID_CATEGORY_DIVIDER && nIdx < pDrvCtxt->pBSP->nNumDividers)
  {
    return &pDrvCtxt->pBSP->aDividers[nIdx];
  }

  return NULL;

} /* END Clock_GetDividerNode */

/* =========================================================================
**  Function : Clock_SetClockDivider
** =========================================================================*/
/*
  See DDIClock.h
*/

DALResult Clock_SetDivider
(
  ClockDrvCtxt          *pDrvCtxt,
  ClockDividerNodeType  *pDivider,
  uint32                 nDivider
)
{
  /*-----------------------------------------------------------------------*/
  /* Sanity check the handle.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDivider == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  /*-----------------------------------------------------------------------*/
  /* Configure the hardware divider.                                       */
  /*-----------------------------------------------------------------------*/

  DALCLOCK_LOCK(pDrvCtxt);

  HAL_clk_ConfigDivider(&pDivider->HALDivider, nDivider);

  /* Logging */
  if (CLOCK_FLAG_IS_SET(pDivider, LOG_CONFIG_CHANGE) ||
      CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_CONFIG_CHANGE))
  {
    ULOG_RT_PRINTF_2 (pDrvCtxt->hClockLog,
                      "%s (Divider: %lu)",
                      pDivider->szName, nDivider);
  }

  DALCLOCK_FREE(pDrvCtxt);

  return DAL_SUCCESS;

} /* END Clock_SetClockDivider */


/* =========================================================================
**  Function : Clock_GetFrequencyPlan
** =========================================================================*/
/**
  See DDIClock.h
*/

DALResult Clock_GetFrequencyPlan
(
  ClockDrvCtxt       *pDrvCtxt,
  ClockNodeType      *pClock,
  ClockFreqPlanType **aFreqPlan,
  uint32             *nSize
)
{
  ClockMuxConfigType  *pBSP;
  ClockFreqPlanType   *pFreqPlan;
  uint32               nBytes, nBSPTotal = 0;
  uint32               i;

  if (!nSize || !pClock)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (!pClock->pDomain || pClock->pDomain->aBSP == NULL)
  {
    *nSize = 0;
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Count total number of supported frequencies.                          */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < pClock->pDomain->nBSPLen; i++)
  {
    pBSP = &pClock->pDomain->aBSP[i];
    if (Clock_IsBSPSupported(&pBSP->HWVersion))
    {
      nBSPTotal++;
    }
  }

  if (!aFreqPlan)
  {
    *nSize = nBSPTotal;
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* If the client passes a NULL pointer, allocate the array for them.     */
  /*-----------------------------------------------------------------------*/

  if (*aFreqPlan == NULL)
  {
    *nSize = nBSPTotal;
    nBytes = nBSPTotal * sizeof(ClockFreqPlanType);

    if (DALSYS_Malloc(nBytes, (void **)aFreqPlan) != DAL_SUCCESS)
    {
      DALSYS_LogEvent(DALDEVICEID_CLOCK, DALSYS_LOGEVENT_ERROR,
        "Unable to allocate %lu bytes for %s frequency plan.",
        nBytes, pClock->szName);
      *nSize = 0;
      return DAL_ERROR_INSUFFICIENT_MEMORY;
    }
  }

  if (*nSize < nBSPTotal)
  {
    /*
     * Buffer is too small. Return an error and inform the client of
     * the required buffer size.
     */
    *nSize = nBSPTotal;
    return DAL_ERROR_INSUFFICIENT_MEMORY;
  }

  /*-----------------------------------------------------------------------*/
  /* Fill in the frequency plan.                                           */
  /*-----------------------------------------------------------------------*/

  pFreqPlan = *aFreqPlan;
  for (i = 0; i < pClock->pDomain->nBSPLen; i++)
  {
    pBSP = &pClock->pDomain->aBSP[i];
    if (!Clock_IsBSPSupported(&pBSP->HWVersion))
    {
      continue;
    }

    pFreqPlan->nFreqHz = pBSP->nFreqHz;
    pFreqPlan->eVRegLevel = pBSP->eVRegLevel;
    pFreqPlan++;
  }

  *nSize = nBSPTotal;
  return DAL_SUCCESS;

} /* END Clock_GetFrequencyPlan */


/* =========================================================================
**  Function : Clock_GetClockInfo
** =========================================================================*/
/**
  See DDIClock.h
*/

DALResult Clock_GetClockInfo
(
  ClockDrvCtxt  *pDrvCtxt,
  ClockNodeType *pClock,
  ClockInfoType *pClockInfo
)
{
  ClockDomainNodeType *pDomain;

  if (!pClock || !pClockInfo)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  DALSYS_memset(pClockInfo, 0, sizeof(*pClockInfo));

  pDomain = pClock->pDomain;
  if (pDomain && pDomain->pRail)
  {
    pClockInfo->szRail = pDomain->pRail->szName;
  }

  return DAL_SUCCESS;

} /* END Clock_GetClockInfo */


/* =========================================================================
**  Function : Clock_GetHighestFreqConfig
** =========================================================================*/
/**
  Get the highest frequency configuration supported by domain.

  @param *pDomain[in] -- Pointer to a clock domain.

  @return
  Non-NULL  -- Highest configuration found.
  NULL      -- No supported configurations found.

  @dependencies
  None.
*/

static ClockMuxConfigType *Clock_GetHighestFreqConfig
(
  ClockDomainNodeType *pDomain
)
{
  ClockMuxConfigType  *pConfig;
  int32                i;

  if (!pDomain || !pDomain->aBSP)
  {
    return NULL;
  }

  for (i = pDomain->nBSPLen - 1; i >= 0; i--)
  {
    pConfig = &pDomain->aBSP[i];
    if (Clock_IsBSPSupported(&pConfig->HWVersion))
    {
      return pConfig;
    }
  }

  return NULL;

}; /* END Clock_GetHighestFreqConfig */


/* =========================================================================
**  Function : Clock_InitNPA
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_InitNPA
(
  ClockDrvCtxt *pDrvCtxt
)
{
  ClockNPANodeType    *pNodeDef;
  ClockNPACtxtType    *pNPACtxt;
  ClockMuxConfigType  *pConfig;
  ClockNodeType       *pClock;
  ClockIdType          nClockId;
  uint32               nMaxFreqKHz;
  uint32               i;

  pNodeDef = pDrvCtxt->pBSP->pGenericNPANode;
  if (!pNodeDef)
  {
    return DAL_SUCCESS;
  }

  for (i = 0; i < pNodeDef->Node.resource_count; i++)
  {
    pNPACtxt = pNodeDef->Node.resources[i].data;

    Clock_GetClockId(pDrvCtxt, pNPACtxt->szClockName, &nClockId);
    pClock = Clock_GetClockNode(pDrvCtxt, nClockId);
    if (!pClock || !pClock->pDomain || !pClock->pDomain->aBSP)
    {
      return DAL_ERROR;
    }

    pNPACtxt->pClock = pClock;

    CLOCK_FLAG_SET(pClock->pDomain, INTERNAL_CONTROL);

    pConfig = Clock_GetHighestFreqConfig(pClock->pDomain);
    if (!pConfig)
    {
      return DAL_ERROR;
    }

    nMaxFreqKHz = pConfig->nFreqHz / 1000;
    pNodeDef->Node.resources[i].max = nMaxFreqKHz;
    pNodeDef->anInitStates[i] = MIN(pNodeDef->anInitStates[i], nMaxFreqKHz);
  }

  pNodeDef->Node.data = pDrvCtxt;
  npa_define_node_cb(&pNodeDef->Node, pNodeDef->anInitStates, NULL, NULL);
  return DAL_SUCCESS;
};


/* =========================================================================
**  Function : Clock_GenericNPANodeFunc
** =========================================================================*/
/**
  See ClockDriver.h
*/

npa_resource_state Clock_GenericNPANodeFunc
(
  npa_resource       *pResource,
  npa_client_handle   hClient,
  npa_resource_state  nState
)
{
  ClockDrvCtxt        *pDrvCtxt = (ClockDrvCtxt *)pResource->node->data;
  ClockNPACtxtType    *pNPACtxt = (ClockNPACtxtType *)pResource->definition->data;
  ClockDALClientCtxt  *pClientCtxt = Clock_GetInitialClientCtxt();
  uint32               nResultFreq;
  uint32               nFreqHz = NPA_TO_HZ(nState);
  DALResult            eResult;

  /*-----------------------------------------------------------------------*/
  /* Perform necessary setup during resource creation.                     */
  /*-----------------------------------------------------------------------*/

  if (hClient->type == NPA_CLIENT_INITIALIZE)
  {
    if (nFreqHz == 0)
    {
      return 0;
    }

    /*
     * If we're setting a non-zero initial state, then assume we also want
     * to enable the clock. Most of the time this would be for suppressible
     * clocks like CPUs or busses.
     */
    eResult = Clock_EnableClock(pClientCtxt, pNPACtxt->pClock);
    if (eResult != DAL_SUCCESS)
    {
      DALSYS_LogEvent (
        DALDEVICEID_CLOCK, DALSYS_LOGEVENT_FATAL_ERROR,
        "Unable to enable root NPA node clock.");
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Update clock frequency.                                               */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_SetClockFrequency(pDrvCtxt, pNPACtxt->pClock,
    nFreqHz, CLOCK_FREQUENCY_HZ_AT_LEAST, &nResultFreq);
  if (eResult != DAL_SUCCESS)
  {
    return pResource->active_state;
  }

  return nResultFreq / 1000;

} /* END Clock_GenericNPANodeFunc */


/* =========================================================================
**  Function : Clock_GetRequiredCorner
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_GetRequiredCorner
(
  const ClockFMaxType *aFMax,
  uint32               nFreqHz,
  rail_voltage_level  *peVRegLevel
)
{
  const ClockFMaxType *pFMax;

  if (!aFMax || !peVRegLevel)
  {
    return DAL_ERROR;
  }

  for (pFMax = aFMax; pFMax->nMaxFreqHz; pFMax++)
  {
    if (!Clock_IsBSPSupported(&pFMax->HWVersion))
    {
      continue;
    }

    if (pFMax->nMaxFreqHz >= nFreqHz)
    {
      *peVRegLevel = pFMax->eVRegLevel;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END Clock_GetRequiredCorner */


/* =========================================================================
**  Function : Clock_GetMappedSource
** =========================================================================*/
/**
  See ClockDriver.h
*/

DALResult Clock_GetMappedSource
(
  const ClockSourceMapType   *aSourceMap,
  uint32                      nMuxSel,
  const ClockSourceMapType  **pMatchingSourceMap
)
{
  const ClockSourceMapType *pSourceMap;

  if (!aSourceMap || !pMatchingSourceMap)
  {
    return DAL_ERROR;
  }

  for (pSourceMap = aSourceMap;
       pSourceMap->nMuxSel != CLOCK_SOURCE_MAP_END; pSourceMap++)
  {
    if (!Clock_IsBSPSupported(&pSourceMap->HWVersion))
    {
      continue;
    }

    if (pSourceMap->nMuxSel == nMuxSel)
    {
      *pMatchingSourceMap = pSourceMap;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR;

} /* END Clock_GetMappedSource */


/* =========================================================================
**  Function : Clock_AssignNPAState
** =========================================================================*/
/**
  See ClockDriver.h
*/

npa_resource_state Clock_AssignNPAState
(
  npa_resource        *pResource,
  npa_resource_state   nRequest
)
{
  npa_client          DummyClient = {0};
  npa_client_handle   hClient = (npa_client_handle)&DummyClient;
  npa_resource_state  nActiveState;

  npa_resource_lock(pResource);
  nActiveState = pResource->node->driver_fcn(pResource, hClient, nRequest);
  npa_assign_resource_state(pResource, nActiveState);
  npa_resource_unlock(pResource);

  return nActiveState;

} /* END Clock_AssignNPAState */


/* =========================================================================
**  Function : Clock_RestoreNPAState
** =========================================================================*/
/**
  See ClockDriver.h
*/

npa_resource_state Clock_RestoreNPAState
(
  npa_resource *pResource
)
{
  return Clock_AssignNPAState(pResource, pResource->request_state);

} /* END Clock_RestoreNPAState */

