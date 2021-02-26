/**
==============================================================================

  @file ClockBIMC.c

  This file provides clock BIMC/DDR settings.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockBIMC.c#3 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockBoot.h"
#include "ClockHWIO.h"
#include "ClockBSP.h"
#include "ddr_drivers.h"
#include "ddr_common.h"
#include "railway.h"
#include "boot_util.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/
#define CLOCK_ERROR -1
/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)


/*=========================================================================
     Externs
==========================================================================*/
extern ClockSrcDynCfgType* Clock_GetClockSrcCfg( void );
extern FREQ_STATE* Clock_GetDSFConfig( void );

/*=========================================================================
      Function Prototypes
==========================================================================*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue);

/*=========================================================================
      Data
==========================================================================*/
#define MAX_DDR_LEVELS 12

ClockSrcDynCfgType SrcCfg[] =
{
  /* =============================================================
  **  nFreqKHz, nSource,                   nL,   nFracVal, nDiv2x }
  ** =============================================================*/
  {    19200,   HAL_CLK_SOURCE_XO,         0,    0,        0 }, // XO for DDRCC mode
  {   100000,   HAL_CLK_SOURCE_GPLL0_EVEN, 0,    0,        6 },
  {   200000,   HAL_CLK_SOURCE_GPLL2,      0x14, 0xD555,   4 },
  {   300000,   HAL_CLK_SOURCE_GPLL0_EVEN, 0,    0,        0 },
  {   451200,   HAL_CLK_SOURCE_GPLL2,      0x17, 0x8000,   0 }, 
  {   547200,   HAL_CLK_SOURCE_GPLL2,      0x1C, 0x8000,   0 },
  {   681600,   HAL_CLK_SOURCE_GPLL2,      0x23, 0x8000,   0 },
  { 0 }
};

static FREQ_STATE* pMCCfg = NULL;

/* The current rate of the DRR clock in kHz */
static uint32 nDDRSpeedKHz = 200000;

/*
 * Clock configure performance levels for inform AOP during initialize
 */
extern ClockCfgCPType Clock_PerfData;

/*=========================================================================
      Function Definitions
=========================================================================*/

/* ============================================================================
**  Function : Clock_MapCorner
** ============================================================================
*/
/**
  Due to memory constraint, we cannot use voltage code in MC/SHUB table.
  This driver is used for convert voltage corner to railway corner

  @params
     rail - VOLTAGE_RAIL value
  @return
    railway corner value

  @dependencies
    None.

  @sa
    None
*/
railway_corner Clock_MapCorner( VOLTAGE_RAIL rail )
{
  if ( rail >= TURBO_L1 )
    return RAILWAY_TURBO_L1;
  else if ( rail >= TURBO )
    return RAILWAY_TURBO;
  else if ( rail >= TURBO )
    return RAILWAY_TURBO;
  else if ( rail >= NOMINAL )
    return RAILWAY_NOMINAL;
  else if ( rail >= SVS_L1 )
    return RAILWAY_SVS_HIGH;
  else if ( rail >= SVS )
    return RAILWAY_SVS;
  else if ( rail >= LOW_SVS )
    return RAILWAY_SVS_LOW;
  else if ( rail >= MIN_SVS )
    return RAILWAY_SVS_MIN;
  else if ( rail >= VDD_RET )
    return RAILWAY_RETENTION;
  else
    return RAILWAY_NO_REQUEST;
}

/* =========================================================
**  Function : Clock_DDRSpeed()
** =======================================================*/
/* 
 * @param None
 * @return The clock rate of DDR in kHz
 */
uint32 Clock_DDRSpeed()
{
  return nDDRSpeedKHz;
}


/* ============================================================================
**  Function : Clock_GetSrcCfgIndex
** ============================================================================
*/
/**
  Get clock source config index that supports the frequency.  If the 

  @params
    pNewCfg           [in]  - BIMC config to prepare.
    pOrigCfg          [in]  - Previous BIMC config.
    peSourceToDisable [out] - BIMC source that might require disabling after
                              the clock switch via Clock_DisableBIMCSources().

  @return
    True upon success.

  @dependencies
    None.

  @sa
    None
*/
uint32 Clock_GetSrcCfgIndex( FREQ_STATE* pFreqCfg )
{
  uint32 nIndex;

  /* Default XO source */
  if ( pFreqCfg->mode == 1 )
    return 0;

  for ( nIndex = 1; SrcCfg[nIndex].nFreqKHz != 0; nIndex++ )
  {
    if ( SrcCfg[nIndex].nFreqKHz == pFreqCfg->freq_khz )
      return nIndex;
  }
  return CLOCK_ERROR;
}


/* ============================================================================
**  Function : Clock_EnableBIMCSource
** ============================================================================
*/
/**
  Configures and enables a BIMC PLL. Handles GPLL2/GPLL3 ping-pong.

  @params
    pNewCfg           [in]  - BIMC config to prepare.
    pOrigCfg          [in]  - Previous BIMC config.
    peSourceToDisable [out] - BIMC source that might require disabling after
                              the clock switch via Clock_DisableBIMCSources().

  @return
    True upon success.

  @dependencies
    None.

  @sa
    None
*/
static boolean Clock_EnableBIMCSource
(
  FREQ_STATE *pNewDDRCfg,
  FREQ_STATE *pOldDDRCfg,
  uint8       *pnSrcDisable
)
{
  Clock_ConfigType          *pClockCfg;
  ClockConfigPLLType        *pPLLCfg;
  ClockSourceType           ePLL;
  uint8                     eNewSource;
  const ClockSrcDynCfgType  *pDynPLLCfg;
  uint32                    nSrcIndex;

  /* Get clock source config index */
  nSrcIndex = Clock_GetSrcCfgIndex( pNewDDRCfg );
  if ( nSrcIndex == CLOCK_ERROR )
    return FALSE;

  eNewSource = SrcCfg[nSrcIndex].nSource;

  /* Nothing to do for XO/GPLL0 source */
  if ( (eNewSource == HAL_CLK_SOURCE_GPLL0_EVEN) ||
       (eNewSource == HAL_CLK_SOURCE_GPLL0) ||
       (eNewSource == HAL_CLK_SOURCE_XO) )
  {
    nSrcIndex = Clock_GetSrcCfgIndex( pOldDDRCfg );
    *pnSrcDisable = SrcCfg[nSrcIndex].nSource;
    return TRUE;
  }

  /* Else PLL must be GPLL2 or GPLL3. */
  if ( (eNewSource != HAL_CLK_SOURCE_GPLL2) &&
       (eNewSource != HAL_CLK_SOURCE_GPLL3) )
  {
    return FALSE;
  }
  
  pClockCfg = Clock_Config();

  /* Switch to whichever of GPLL2/GPLL3 is currently inactive. */
  if ( HWIO_INF(GCC_GPLL2_MODE, PLL_ACTIVE_FLAG) == 0 )
  {
    SrcCfg[nSrcIndex].nSource = HAL_CLK_SOURCE_GPLL2;
    *pnSrcDisable = HAL_CLK_SOURCE_GPLL3;

    pPLLCfg = &pClockCfg->PLL2_Cfg;
    ePLL = SRC_GPLL2;
  }
  else if ( HWIO_INF(GCC_GPLL3_MODE, PLL_ACTIVE_FLAG) == 0 )
  {
    SrcCfg[nSrcIndex].nSource = HAL_CLK_SOURCE_GPLL3;
    *pnSrcDisable = HAL_CLK_SOURCE_GPLL2;

    pPLLCfg = &pClockCfg->PLL3_Cfg;
    ePLL = SRC_GPLL3;
  }
  else
  {
    /* Both GPLL2 and GPLL3 are active => bug. */
    return FALSE;
  }

  /* Set new L and nFracVal value */
  pDynPLLCfg = &SrcCfg[nSrcIndex];
  pPLLCfg->nL = pDynPLLCfg->nL;
  pPLLCfg->nFracVal = pDynPLLCfg->nFracVal;
  
  if ( !Clock_ConfigureSource(ePLL) ) return FALSE;
  return Clock_EnableSource( SrcCfg[nSrcIndex].nSource );
}

/* ============================================================================
**  Function : Clock_DisableBIMCSource
** ============================================================================
*/
/**
  Disables a BIMC PLL if necessary.

  @params
    eSource [in]  - BIMC source to disable.

  @return
    True upon success.

  @dependencies
    None.

  @sa
    None
*/
static boolean Clock_DisableBIMCSource
(
  HAL_clk_SourceType    eSource
)
{
  /* Never disable GPLL0/XO. */
  if ( (eSource == HAL_CLK_SOURCE_XO) ||
       (eSource == HAL_CLK_SOURCE_GPLL0) ||
       (eSource == HAL_CLK_SOURCE_GPLL0_EVEN))
  {
    return TRUE;
  }

  /* Else PLL must be GPLL2 or GPLL3. */
  if ( (eSource != HAL_CLK_SOURCE_GPLL2) &&
       (eSource != HAL_CLK_SOURCE_GPLL3) )
  {
    return FALSE;
  }

  return Clock_DisableSource( (ClockSourceType)eSource );
}

/* =========================================================================
**  Function : HAL_clk_DDRSetConfigMux
** =========================================================================*/
/*!
    Configure BIMC clock domain.

    @param 
      nCmdRCGRAddr     - Clock CMD_RCGR address
      nSourceIndex    - Source Index
      nDiv2x          - Divider

    @return
      None.

    @dependencies

    @sa None
*/

static boolean HAL_clk_DDRSetConfigMux
(
  uint8 nChannel,
  uint8 nSource,
  uint8 nDiv2x
)
{
  ClockConfigMuxType clkCfg = {HWIO_ADDR(GCC_DDRMC_CH0_ROOT_CMD_RCGR), 0, MUX_GCC, SRC_CXO, 0, 0, 0, 0};

  /* Default already at channel-0.  Only switch to channel-1 if required */
  if ( nChannel == 1 )
  {
    clkCfg.nCMDRCGRAddr = HWIO_ADDR(GCC_DDRMC_CH1_ROOT_CMD_RCGR);
  }
  
  clkCfg.eSource = nSource;
  clkCfg.nDiv2x = nDiv2x;
  
  return Clock_ConfigMux(&clkCfg);
}

/* =========================================================================
**  Function : HAL_clk_DDRConfigMux
** =========================================================================*/
/*!
    Programs 

    @param
      pmConfig -  [IN] Clock mux config structure

    @return
      None.

    @dependencies
      None.

    @sa
      None.
*/

static boolean HAL_clk_DDRConfigMux
(
  FREQ_STATE *pmConfig,
  int nNextCPLL
)
{
  uint32 nSourceSelect, nSrcIndex;

  if ( pmConfig == NULL )
    return FALSE;

  /* Get clock source config index */
  nSrcIndex = Clock_GetSrcCfgIndex( pmConfig );
  if ( nSrcIndex == CLOCK_ERROR )
    return FALSE;

  /* Get source index from source enum. */
  if ( !Clock_SourceMapToGCC((ClockSourceType)SrcCfg[nSrcIndex].nSource, &nSourceSelect) )
    return FALSE;

  /*
   * Set GCC_BIMC_DDR_CPLL config.
   * Ping-pong between CPLL0 and CPLL1, starting from CPLL0.
   * On switches from DDRCC to GCC, we must start over from CPLL0.
   *
   */
  if ( nNextCPLL == 0)
  {
    if ( !HAL_clk_DDRSetConfigMux(0, SrcCfg[nSrcIndex].nSource, SrcCfg[nSrcIndex].nDiv2x) ) return FALSE;
    if ( pmConfig->mode == GCC )
    {
      Clock_ToggleClock(HWIO_GCC_DDRMC_CH0_CBCR_ADDR, CLK_TOGGLE_ENABLE); // GCC Mode needs CPLL clock
    }
  } 
  else 
  {
    if ( !HAL_clk_DDRSetConfigMux(1, SrcCfg[nSrcIndex].nSource, SrcCfg[nSrcIndex].nDiv2x) ) return FALSE;
    if ( pmConfig->mode == GCC )
    {
      Clock_ToggleClock(HWIO_GCC_DDRMC_CH1_CBCR_ADDR, CLK_TOGGLE_ENABLE); // GCC Mode needs CPLL clock
    }
  }

  return TRUE;
} /* HAL_clk_DDRConfigMux */


/* =========================================================================
**  Function : Clock_FindBIMCLevel
** =========================================================================*/
/*!
    Find the table line that meets or exceeds the request.

    @param -  nFreqHz [in] Frequency to search for in the table
    @param -  pConfigTable [in] Table to search
    @param -  nTableLimit [in] Length of table 
    @return - index to the table row to be used.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_FindBIMCLevel
( 
  uint32 nFreqKHz,
  FREQ_STATE *pDDRConfig
)
{
  uint32 index;

  /* Start with index_1 since index_0 is for DDR power collapsed */
  for(index = 1; (index < MAX_DDR_LEVELS) && (pDDRConfig[index].freq_khz != 0); index++)
  {
    if( pDDRConfig[index].freq_khz >= nFreqKHz )
    {
      return index;
    }
  }

  /* If not found, return index of highest enable clock plan in the table */
  if ( index == MAX_DDR_LEVELS )
  {
    return (MAX_DDR_LEVELS-1);
  }

  return 0;
}


/* =========================================================================
**  Function : Clock_SetBIMCSpeed
** =========================================================================*/
/*!
    Switch DDR and the related BIMC roots to the requested frequency

    @param -  nFreqHz [in] Frequency to search for in the table
    @return - FALSE on fail, TRUE on success

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetBIMCSpeed(uint32 nFreqKHz )
{
  railway_corner    vdda, vddcx;
  uint32            nCurrKHz, nCurrLvlIdx, nNewLvlIdx; 
  Clock_RailwayType *pClockRailway = Clock_RailwayConfig();
  FREQ_STATE       *pCurrDDRCfg;
  FREQ_STATE       *pNewDDRCfg;
  uint8             nSrcDisable;
  int               nNextCPLL, nPrevCPLL;

  /* Do nothing if Cx or VDDA_EBI railways is not supported */
  if ( (RAIL_NOT_SUPPORTED_BY_RAILWAY == pClockRailway->nCxRailId) ||
       (RAIL_NOT_SUPPORTED_BY_RAILWAY == pClockRailway->nEBIRailId) )
    return TRUE;

  /* Get MC configure data */
  if ( pMCCfg == NULL)
  {
    ddr_clock_plan DDRQuery = ddr_clock_query(MC_CLOCK_PLAN);

    /* Spin in while-loop for error if number of DDR levels is not matching */
    while ( DDRQuery.num_levels != MAX_DDR_LEVELS );

    pMCCfg = DDRQuery.state;
  }

  /* Find current level */
  nCurrKHz = Clock_DDRSpeed();
  nCurrLvlIdx = Clock_FindBIMCLevel ( nCurrKHz, pMCCfg );

  pCurrDDRCfg = &pMCCfg[nCurrLvlIdx];

  /* Find new level */
  nNewLvlIdx = Clock_FindBIMCLevel ( nFreqKHz, pMCCfg );
  pNewDDRCfg = &pMCCfg[nNewLvlIdx];
  nFreqKHz = pMCCfg[nNewLvlIdx].freq_khz;

  // Change VDDA_EBI rail if increase
  if ( pMCCfg[nCurrLvlIdx].vdda < pMCCfg[nNewLvlIdx].vdda )
  {
    vdda = Clock_MapCorner( pMCCfg[nNewLvlIdx].vdda );
    if( Clock_SwitchRail(CLOCK_RAIL_MX, vdda, CLOCK_RAIL_CLIENT_DDR) == FALSE) return FALSE;
  }

  /* 
   * Vote for higher voltage before the switch if the new frequency is more
   * than the current frequency.
   */
  if ( nCurrKHz < nFreqKHz )
  {
    vddcx = Clock_MapCorner( pMCCfg[nNewLvlIdx].vddcx );
    if( Clock_SwitchRail(CLOCK_RAIL_CX, vddcx, CLOCK_RAIL_CLIENT_DDR) == FALSE) return FALSE;
  }

  /* Enable the next source before switch */
  if( !Clock_EnableBIMCSource( pNewDDRCfg, pCurrDDRCfg, &nSrcDisable) )
  {
    return FALSE;
  }

  nNextCPLL = 0;   /* This default covers the GCC -> DDRCC mode change */
  nPrevCPLL = -1;  /* -1 means no previous CPLL, such as DDRCC mode */
  if( HWIO_INF(GCC_DDRMC_CH0_CBCR, CLK_ENABLE) == 1 )
  {
    nPrevCPLL = 0;
    nNextCPLL = 1;
  }
  if( HWIO_INF(GCC_DDRMC_CH1_CBCR, CLK_ENABLE) == 1 )
  {
    if(nPrevCPLL == 0) return FALSE; /* Both CPLLs are On.  That is bad */
    nPrevCPLL = 1;
    nNextCPLL = 0;
  }
  
  if( !HAL_clk_DDRConfigMux( pNewDDRCfg, nNextCPLL ) )
  {
    return FALSE;
  }

  ddr_pre_clock_switch( nCurrKHz, nFreqKHz, DDR_CH_ALL );
  ddr_clock_switch( nCurrKHz, nFreqKHz, DDR_CH_ALL );
  ddr_post_clock_switch( nCurrKHz, nFreqKHz, DDR_CH_ALL );

  /* Park previous CPLL at CXO/1 and turn it off. */
  if( nPrevCPLL == 0)
  {
    ClockConfigMuxType clkCfg0 = {HWIO_ADDR(GCC_DDRMC_CH0_ROOT_CMD_RCGR), 0, MUX_GCC, SRC_CXO, 0, 0, 0, 0};
    Clock_ConfigMux( &clkCfg0 );

    Clock_ToggleClock(HWIO_GCC_DDRMC_CH0_CBCR_ADDR, CLK_TOGGLE_DISABLE);
  }
  if( nPrevCPLL == 1)
  {
    ClockConfigMuxType clkCfg1 = {HWIO_ADDR(GCC_DDRMC_CH1_ROOT_CMD_RCGR), 0, MUX_GCC, SRC_CXO, 0, 0, 0, 0};
    Clock_ConfigMux( &clkCfg1 );

    Clock_ToggleClock(HWIO_GCC_DDRMC_CH1_CBCR_ADDR, CLK_TOGGLE_DISABLE);
  }

  /* Disable previous DDR-dedicated sources after the switch */
  Clock_DisableBIMCSource(nSrcDisable);

  /* 
   * Vote for lower voltage after the switch if the new frequency is less
   * than the current frequency.
   */
  if ( nCurrKHz > nFreqKHz )
  {
    vddcx = Clock_MapCorner( pNewDDRCfg->vddcx );
    if (Clock_SwitchRail(CLOCK_RAIL_CX, vddcx, CLOCK_RAIL_CLIENT_DDR) == FALSE) return FALSE;
  }

  // Change VDDA_EBI rail if decrease
  if ( pMCCfg[nCurrLvlIdx].vdda > pMCCfg[nNewLvlIdx].vdda )
  {
    vdda = Clock_MapCorner( pMCCfg[nNewLvlIdx].vdda );
    if (Clock_SwitchRail(CLOCK_RAIL_MX, vdda, CLOCK_RAIL_CLIENT_DDR) == FALSE) return FALSE;
  }

  Clock_PerfData.nDDR = nNewLvlIdx;
  nDDRSpeedKHz = nFreqKHz;

  return TRUE;
}

/* =========================================================================
**  Function : Clock_CopyBIMCPlanToRPM()
** =========================================================================*/
/*!
    Copy the BIMC clock plan into shared memory for the RPM to use.

    @param -  
    @return - FALSE on fail, TRUE on success

    @dependencies
    None.

    @sa None
*/
boolean Clock_CopyBIMCPlanToRPM()
{
  return TRUE;
}
