/*
===========================================================================
*/
/**
  @file ClockAppsDCVS.c

  DCVS and NPA node implementation.
*/
/*
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockAppsDCVS.c#2 $
  $DateTime: 2017/11/10 00:12:02 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockDriver.h"
#include "ClockApps.h"
#include "QdssSWEvent.h"
#include "tracer_event_ids.h"

/*=========================================================================
      Constants and Macros
==========================================================================*/

/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Externals
==========================================================================*/

/*=========================================================================
      Prototypes
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/

/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_InitDCVS
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitDCVS
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Make sure DCVS is enabled.                                            */
  /*-----------------------------------------------------------------------*/

  pDrvCtxt->bDCVSEnabled = TRUE;

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitDCVS */


/* =========================================================================
**  Function : Clock_SetCPUConfig
** =========================================================================*/
/**
  NPA node driver function for the CPU node.

  This function handles minimum frequency requests for the CPU NPA node.

  @param *pDrvCtxt [in] -- Pointer to driver context.
  @param nCPU [in] -- Which CPU domain to change.
  @param pPerfConfig [in] -- New configuration.

  @return
  DAL_SUCCESS - CPU clock configuration succeeded.
  DAL_ERROR - CPU clock configuration failed.

  @dependencies
  None.
*/

DALResult Clock_SetCPUConfig
(
  ClockDrvCtxt                  *pDrvCtxt,
  ClockAppsCPUType               eCPU,
  ClockAppsPerfLevelConfigType  *pPerfConfig
)
{
  DALResult          eResult = DAL_SUCCESS;
  ClockAppsCtxtType *pAppsCtxt = (ClockAppsCtxtType *)pDrvCtxt->pImageCtxt;

 /*-----------------------------------------------------------------------*/
  /* Sanity checks.                                                        */
  /*-----------------------------------------------------------------------*/

  if (pAppsCtxt == NULL )
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                      "DALSYS_LOGEVENT_FATAL_ERROR: Invalid configuration detected by Clock_SetCPUConfig.");
    ERR_FATAL("Clock_SetCPUConfig", 0, 0, 0);
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* If the desired frequency has already been set, just return.           */
  /*-----------------------------------------------------------------------*/

  if (pPerfConfig->nFreqHz == pAppsCtxt->aCPU[eCPU].pActiveConfig->nFreqHz)
  {
    return DAL_SUCCESS;
  }

  /*-----------------------------------------------------------------------*/
  /* Use OSM to set the desired perf level.                                */
  /*-----------------------------------------------------------------------*/

  if (!HAL_clk_OSM_SET_DESIRED_PERF_LEVEL((HAL_clk_APCSCPUType)eCPU, pPerfConfig->nVRegAPCCornerIndex))
  {
    ULOG_RT_PRINTF_0 (pDrvCtxt->hClockLog,
                      "DALSYS_LOGEVENT_FATAL_ERROR: Invalid OSM configuration detected by Clock_SetCPUConfig.");
    ERR_FATAL("Clock_SetCPUConfig", 0, 0, 0);
    return DAL_ERROR;
  }

  if (CLOCK_GLOBAL_FLAG_IS_SET(LOG_CLOCK_FREQ_CHANGE))
  {
    ULOG_RT_PRINTF_2 (
      pDrvCtxt->hClockLog,
      "Freq[/clk/cpu%d] = %lu (Hz)",
      eCPU,
      pPerfConfig->nFreqHz);
  }

  /*
   * Log CPU frequency change event.
   */
  QDSS_SWEVENT(CLOCK_EVENT_CPU_FREQ, eCPU, pPerfConfig->nFreqHz);

  /*-----------------------------------------------------------------------*/
  /* Update state.                                                         */
  /*-----------------------------------------------------------------------*/

  pAppsCtxt->aCPU[eCPU].pActiveConfig = pPerfConfig;
  pAppsCtxt->aCPU[eCPU].ActiveMuxConfig.nFreqHz = pPerfConfig->nFreqHz;

  return eResult;

} /* END Clock_SetCPUConfig */

