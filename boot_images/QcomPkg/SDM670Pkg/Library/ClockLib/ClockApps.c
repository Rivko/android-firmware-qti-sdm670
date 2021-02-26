/*
===========================================================================
*/
/**
  @file ClockApps.c

  Main entry point for the SDM670 main application processor clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2017 QUALCOMM Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockLib/ClockApps.c#3 $
  $DateTime: 2017/10/19 21:08:15 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockDriver.h"
#include "ClockApps.h"
#include "HALhwio.h"
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIPlatformInfoTypes.h>
#include <Protocol/EFIPlatformInfo.h>

#include <Pi/PiStatusCode.h>
#include <Protocol/StatusCode.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Library/BaseMemoryLib.h>
#include "Lpm.h"
#include "Drivers/ClockDxe/ClockDriver.h"

/*=========================================================================
      Type Definitions
==========================================================================*/

/*=========================================================================
      Constants and Macros
==========================================================================*/

/* Initial CPU cluster frequencies -- highest available at Nominal Corner */
#define SILVER_CLUSTER_INITIAL_PERF_LEVEL  6      // OSM perf level (1516.8 MHz)
#define GOLD_CLUSTER_INITIAL_PERF_LEVEL    0       // OSM perf level (300 MHz)


/*=========================================================================
      Externs
==========================================================================*/

extern HAL_clk_ClockControlType           HAL_clk_GenericClockControl;
extern HAL_clk_ClockDomainControlType     HAL_clk_GenericDomainControl;
extern EFI_STATUS Clock_RegisterLPMCallBacks (BOOLEAN bIsRegisterLpm);
extern void Clock_MdssMdpRequest (ClockDrvCtxt *, BOOLEAN);

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
      Data
==========================================================================*/

/*
 * Clock Apps context.
 */
ClockAppsCtxtType Clock_AppsCtxt;

/*=========================================================================
      CPU Clock Domains
==========================================================================*/

static ClockDomainNodeType ClockDomain_CPU_C0 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C1 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C2 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C3 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C4 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C5 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C6 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_C7 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_CPU_L3 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType *aCpuDomains[] =
{
  &ClockDomain_CPU_C0,
  &ClockDomain_CPU_C1,
  &ClockDomain_CPU_C2,
  &ClockDomain_CPU_C3,
  &ClockDomain_CPU_C4,
  &ClockDomain_CPU_C5,
  &ClockDomain_CPU_C6,
  &ClockDomain_CPU_C7,
  &ClockDomain_CPU_L3,
};

/*=========================================================================
      CPU Clocks
==========================================================================*/

static ClockNodeType aCpuClocks[] =
{
  {
    .szName    = "apcs_silver_0_clk",
    .pDomain   = &ClockDomain_CPU_C0,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_silver_1_clk",
    .pDomain   = &ClockDomain_CPU_C1,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_silver_2_clk",
    .pDomain   = &ClockDomain_CPU_C2,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_silver_3_clk",
    .pDomain   = &ClockDomain_CPU_C3,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_silver_4_clk",
    .pDomain   = &ClockDomain_CPU_C4,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_silver_5_clk",
    .pDomain   = &ClockDomain_CPU_C5,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_gold_0_clk",
    .pDomain   = &ClockDomain_CPU_C6,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_gold_1_clk",
    .pDomain   = &ClockDomain_CPU_C7,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "apcs_l3_clk",
    .pDomain   = &ClockDomain_CPU_L3,
    .pDebugMux = NULL,
    .nUniqueId = 0x0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
};


/*=========================================================================
      Externals
==========================================================================*/

/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_InitTarget
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_InitTarget
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult          eResult = DAL_SUCCESS;
  ClockAppsBSPType  *pAppsBSPConfig;
  uint32             i, nFrequencyHz;
  ClockIdType        nClockId;
  ClockNodeType     *pClock;
  EFI_STATUS         Status;

  /*
   * Hookup the apps context to the driver context.
   */
  pDrvCtxt->pImageCtxt = &Clock_AppsCtxt;

  /*-----------------------------------------------------------------------*/
  /* Initialize the Apps BSP data.                                         */
  /*-----------------------------------------------------------------------*/

  pAppsBSPConfig = Clock_GetAppsBSP();

  for (i = 0; i < CLOCK_APPS_CPU_TOTAL; i++)
  {
    /*
     * Hookup the BSP data.
     */
    switch (i)
    {
      case CLOCK_APPS_CPU_C0:
      case CLOCK_APPS_CPU_C1:
      case CLOCK_APPS_CPU_C2:
      case CLOCK_APPS_CPU_C3:
      case CLOCK_APPS_CPU_C4:
      case CLOCK_APPS_CPU_C5:
        Clock_AppsCtxt.aCPU[i].pBSPConfig = &pAppsBSPConfig->aCPUConfig[CLOCK_APPS_CPU_CLUSTER_C0];
        break;

      case CLOCK_APPS_CPU_C6:
      case CLOCK_APPS_CPU_C7:
        Clock_AppsCtxt.aCPU[i].pBSPConfig = &pAppsBSPConfig->aCPUConfig[CLOCK_APPS_CPU_CLUSTER_C1];
        break;

      case CLOCK_APPS_CPU_L3:
        Clock_AppsCtxt.aCPU[i].pBSPConfig = &pAppsBSPConfig->aCPUConfig[CLOCK_APPS_CPU_CLUSTER_L3];
        break;

      default:
        return DAL_ERROR_INTERNAL;
    }
  }

  eResult = Clock_InitAppsBSP(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Name and configure the CPU clocks.                                    */
  /*-----------------------------------------------------------------------*/

  for (i = 0; i < CLOCK_APPS_CPU_TOTAL; i++)
  {
    /*
     * Get the clock ID.
     */
    eResult = Clock_GetClockId(pDrvCtxt, Clock_AppsCtxt.aCPU[i].pBSPConfig->szName, &nClockId);
    if (eResult != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
    pClock = (ClockNodeType *)nClockId;

    /* Populate the aCpuClocks array data with the corresponding BSP values. */
    Clock_AppsCtxt.aCPU[i].pClock = &aCpuClocks[i];
    aCpuClocks[i].pDebugMux = pClock->pDebugMux;
    aCpuClocks[i].HALClock.nDebugMuxSel = pClock->HALClock.nDebugMuxSel;

    /* Default all CPU active configuration to the first BSP entry. */
    Clock_AppsCtxt.aCPU[i].pActiveConfig = &Clock_AppsCtxt.aCPU[i].pBSPConfig->pConfig->aPerfConfig[0];

    /*
     * Store the original min and max perf levels from BSP data. The BSP values can be updated
     * by subsequent calls to PepClockSetMinPerfLevel and PepClockSetMaxPerfLevel APIs.
     */
    Clock_AppsCtxt.aCPU[i].nMinPerfLevel = Clock_AppsCtxt.aCPU[i].pBSPConfig->pConfig->nMinPerfLevel;
    Clock_AppsCtxt.aCPU[i].nMaxPerfLevel = Clock_AppsCtxt.aCPU[i].pBSPConfig->pConfig->nMaxPerfLevel;

    /* Initialize the CPU ActiveMuxConfig structure. */
    memset(&Clock_AppsCtxt.aCPU[i].ActiveMuxConfig, 0, sizeof(ClockMuxConfigType));
    Clock_AppsCtxt.aCPU[i].pClock->pDomain->pActiveMuxConfig = &Clock_AppsCtxt.aCPU[i].ActiveMuxConfig;
  }

  /*-----------------------------------------------------------------------*/
  /* Initialize the DCVS module.                                           */
  /*-----------------------------------------------------------------------*/

  Clock_InitDCVS(pDrvCtxt);

  /*-----------------------------------------------------------------------*/
  /* Request highest available NOMINAL frequency.                          */
  /*-----------------------------------------------------------------------*/

  CLOCK_GLOBAL_FLAG_SET(CPU_BYPASS_RANGE_CHECKING);
  for (i = CLOCK_APPS_CPU_C0; i < CLOCK_APPS_CPU_CLUSTER_0_TOTAL; i++)
  {
    if (Clock_SetCpuPerfLevel(i, SILVER_CLUSTER_INITIAL_PERF_LEVEL, &nFrequencyHz) != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }
  for (i = CLOCK_APPS_CPU_C6; i < CLOCK_APPS_CPU_CLUSTER_1_TOTAL; i++)
  {
    if (Clock_SetCpuPerfLevel(i, GOLD_CLUSTER_INITIAL_PERF_LEVEL, &nFrequencyHz) != DAL_SUCCESS)
    {
      return DAL_ERROR_INTERNAL;
    }
  }
  CLOCK_GLOBAL_FLAG_CLEAR(CPU_BYPASS_RANGE_CHECKING);

  Clock_MdssMdpRequest(pDrvCtxt, FALSE);
  /*-----------------------------------------------------------------------*/
  /* Register LPM callbacks.                                                            */
  /*-----------------------------------------------------------------------*/
  Status = Clock_RegisterLPMCallBacks(TRUE);

  /*-----------------------------------------------------------------------*/
  /* Run the BIST if enabled.                                              */
  /*-----------------------------------------------------------------------*/

  if (pDrvCtxt->nBISTLogSize == 0)
  {
    pDrvCtxt->nBISTLogSize = CLOCK_BIST_DEFAULT_LOG_SIZE;
  }

  if (pDrvCtxt->bBISTEnabled)
  {
    Clock_BIST (pDrvCtxt);
  }

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitTarget */


/* =========================================================================
**  Function : Clock_DeInitTarget
** =========================================================================*/
/*
  See ClockDriver.h
*/

DALResult Clock_DeInitTarget
(
  ClockDrvCtxt *pDrvCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Clear out driver context data.                                        */
  /*-----------------------------------------------------------------------*/

  memset((void *)pDrvCtxt->pImageCtxt, 0x0, sizeof(ClockAppsCtxtType));

  return DAL_SUCCESS;

} /* END Clock_DeInitTarget */


/* =========================================================================
**  Function : Clock_NormalizeChipInfo
** =========================================================================*/
/*
  See ClockDriver.h
*/

void Clock_NormalizeChipInfo
(
  HAL_clk_ContextType *HALClkCtxt
)
{
  /* Normalize the Chip ID */
  switch (HALClkCtxt->nChipId)
  {
    case CHIPINFO_ID_SDM670:
    default:
      HALClkCtxt->nChipId = CHIPINFO_ID_SDM670;
      break;
  }
}


/* =========================================================================
**  Function : Clock_GetCpuDomains
** =========================================================================*/

ClockDomainNodeType **Clock_GetCpuDomains(void)
{
  return &aCpuDomains[0];
}

