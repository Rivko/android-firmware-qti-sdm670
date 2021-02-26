/*
===========================================================================
*/
/**
  @file ClockUEFI.c

  This file contains the clock device driver UEFI specific code.
*/
/*
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockUEFI.c#1 $
  $DateTime: 2017/09/29 23:30:49 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include <string.h>
#include "Drivers/ClockDxe/ClockDriver.h"
#include "ClockApps.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      External Declarations
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

extern DALResult Clock_SetCPUConfig
(
  ClockDrvCtxt                  *pDrvCtxt,
  ClockAppsCPUType               eCPU,
  ClockAppsPerfLevelConfigType  *pPerfConfig
);


/*=========================================================================
      Data Declarations
==========================================================================*/

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_GetMaxPerfLevel
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult
Clock_GetMaxPerfLevel (
  uint32 nCPU,
  uint32 *pnPerfLevel
)
{
  ClockDrvCtxt              *pDrvCtxt = Clock_GetDrvCtxt();
  ClockAppsCtxtType         *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType *pBSPConfig;

  if ((ClockAppsCPUType)nCPU >= CLOCK_APPS_CPU_TOTAL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pBSPConfig = pAppsCtxt->aCPU[nCPU].pBSPConfig->pConfig;

  if (pBSPConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  *pnPerfLevel = pBSPConfig->nMaxPerfLevel;

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_GetMinPerfLevel
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_GetMinPerfLevel
(
  uint32 nCPU,
  uint32 *pnPerfLevel
)
{
  ClockDrvCtxt              *pDrvCtxt = Clock_GetDrvCtxt();
  ClockAppsCtxtType         *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType *pBSPConfig;

  if ((ClockAppsCPUType)nCPU >= CLOCK_APPS_CPU_TOTAL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pBSPConfig = pAppsCtxt->aCPU[nCPU].pBSPConfig->pConfig;

  if (pBSPConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  *pnPerfLevel = pBSPConfig->nMinPerfLevel;

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_GetCpuPerfLevelFrequency
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult
Clock_GetCpuPerfLevelFrequency (
  uint32 nCPU,
  uint32 nPerfLevel,
  uint32 *pnFrequencyHz,
  uint32 *pnRequiredVoltage
)
{
  ClockDrvCtxt              *pDrvCtxt = Clock_GetDrvCtxt();
  ClockAppsCtxtType         *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType *pBSPConfig;
  uint32                     nCornerTableIndex;

  if ((ClockAppsCPUType)nCPU >= CLOCK_APPS_CPU_TOTAL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pBSPConfig = pAppsCtxt->aCPU[nCPU].pBSPConfig->pConfig;

  if (pBSPConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  if (nPerfLevel < pBSPConfig->nMinPerfLevel || nPerfLevel > pBSPConfig->nMaxPerfLevel)
  {
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  if (nPerfLevel >= SIZEOF_ARRAY(pBSPConfig->anPerfLevel))
  {
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  *pnFrequencyHz = Clock_MapPerfLevelToFrequency(pDrvCtxt, (ClockAppsCPUType)nCPU, nPerfLevel);

  nCornerTableIndex = pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nPerfLevel]].nVRegAPCCornerIndex;

  *pnRequiredVoltage = (Clock_MapVDDAPCCornerToVoltage(pDrvCtxt, (ClockAppsCPUType)nCPU, nCornerTableIndex) + 500) / 1000;

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_SetCpuPerfLevel
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult
Clock_SetCpuPerfLevel (
  uint32 nCPU,
  uint32 nPerfLevel,
  uint32 *pnFrequencyHz
)
{
  DALResult                  eResult;
  ClockDrvCtxt              *pDrvCtxt = Clock_GetDrvCtxt();
  ClockAppsCtxtType         *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType *pBSPConfig;
  uint32                     nValidPerfLevel, nBoostPerfLevel = (uint32)-1;

  if (pnFrequencyHz == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  *pnFrequencyHz = 0;

  if ((ClockAppsCPUType)nCPU >= CLOCK_APPS_CPU_TOTAL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pBSPConfig = pAppsCtxt->aCPU[nCPU].pBSPConfig->pConfig;

  if (pBSPConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  /* Do not check DCVS perf level range, if set to bypass it. */
  if (!CLOCK_GLOBAL_FLAG_IS_SET(CPU_BYPASS_RANGE_CHECKING))
  {
    if (nPerfLevel < pBSPConfig->nMinPerfLevel || nPerfLevel > pBSPConfig->nMaxPerfLevel)
    {
      Clock_ReadAppsBSPConfig(CLOCK_CONFIG_PARAM_GET_BOOST_PERF_LEVEL_CLUSTER_0, &nBoostPerfLevel);
      if (nPerfLevel != nBoostPerfLevel)
      {
        return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
      }
    }
  }
  /* Clamp the given perf level within the valid range. */
  else
  {
    if (nPerfLevel < pBSPConfig->nMinPerfLevel)
    {
      nPerfLevel = pBSPConfig->nMinPerfLevel;
    }
    if (nPerfLevel > pBSPConfig->nMaxPerfLevel)
    {
      nPerfLevel = pBSPConfig->nMaxPerfLevel;
    }
  }

  ///
  /// Validate the desired performance level against the performance levels
  /// supported by this version of the SoC.
  ///
  for (nValidPerfLevel = nPerfLevel;
       nValidPerfLevel < pBSPConfig->nMaxPerfLevel;
       ++nValidPerfLevel)
  {
    if (Clock_IsBSPSupported(&pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nValidPerfLevel]].HWVersion))
    {
      break;
    }
  }

  /* Do not check DCVS perf level range, if set to bypass it. */
  if (!CLOCK_GLOBAL_FLAG_IS_SET(CPU_BYPASS_RANGE_CHECKING))
  {
    if (nValidPerfLevel > pBSPConfig->nMaxPerfLevel && nValidPerfLevel != nBoostPerfLevel)
    {
      return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
    }
  }

  if (nValidPerfLevel >= SIZEOF_ARRAY(pBSPConfig->anPerfLevel))
  {
    return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
  }

  /* Check the state of DCVS and return immediately if DCVS is disabled. */
  if (pDrvCtxt->bDCVSEnabled == TRUE || CLOCK_GLOBAL_FLAG_IS_SET(CPU_TEST_IN_PROGRESS))
  {
    eResult = Clock_SetCPUConfig(
              pDrvCtxt,
              (ClockAppsCPUType)nCPU,
              &pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nValidPerfLevel]]);

    if (eResult != DAL_SUCCESS)
    {
      return eResult;
    }
  }

  *pnFrequencyHz = pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nValidPerfLevel]].nFreqHz;

  return DAL_SUCCESS;
}


/* =========================================================================
**  Function : Clock_GetCpuPerfLevel
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult
Clock_GetCpuPerfLevel (
  uint32 nCPU,
  uint32 *pnPerfLevel
)
{
  ClockDrvCtxt                 *pDrvCtxt = Clock_GetDrvCtxt();
  ClockAppsCtxtType            *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;
  ClockAppsBSPCPUConfigType    *pBSPConfig;
  ClockAppsPerfLevelConfigType *pPerfConfig;
  uint32                        nPerfLevel;

  if ((ClockAppsCPUType)nCPU >= CLOCK_APPS_CPU_TOTAL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  pBSPConfig = pAppsCtxt->aCPU[nCPU].pBSPConfig->pConfig;

  if (pBSPConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  pPerfConfig = pAppsCtxt->aCPU[nCPU].pActiveConfig;

  if (pPerfConfig == NULL)
  {
    return DAL_ERROR_INTERNAL;
  }

  for (nPerfLevel = pBSPConfig->nMinPerfLevel;
       nPerfLevel < pBSPConfig->nMaxPerfLevel + 1;
       ++nPerfLevel)
  {
    if (&pBSPConfig->aPerfConfig[pBSPConfig->anPerfLevel[nPerfLevel]] == pPerfConfig)
    {
      *pnPerfLevel = nPerfLevel;
      return DAL_SUCCESS;
    }
  }

  return DAL_ERROR_OUT_OF_RANGE_PARAMETER;
}

