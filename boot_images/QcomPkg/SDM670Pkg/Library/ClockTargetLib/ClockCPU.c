/**
==============================================================================

  @file ClockCPU.c

  This file provides clock initialization for the Apps CPU.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockCPU.c#5 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockHWIO.h"
#include "ClockBSP.h"
#include <HALcomdef.h>
#include <HALhwio.h>
#include <busywait.h>

/*=========================================================================
      Macro Definitions
==========================================================================*/
#define STATUS_OFFS         (HWIO_SILVER_PLL_STATUS_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define GFMUX_OFFS          (HWIO_SILVER_CFG_GFMUX_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define USER_CTL_OFFS       (HWIO_SILVER_PLL_USER_CTL_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define CONFIG_CTL_LO_OFFS  (HWIO_SILVER_PLL_CONFIG_CTL_LO_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define CONFIG_CTL_HI_OFFS  (HWIO_SILVER_PLL_CONFIG_CTL_HI_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define TEST_CTL_LO_OFFS    (HWIO_SILVER_PLL_TEST_CTL_LO_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define TEST_CTL_HI_OFFS    (HWIO_SILVER_PLL_TEST_CTL_HI_ADDR - HWIO_SILVER_PLL_MODE_ADDR)
#define L_VAL_OFFS          (HWIO_SILVER_PLL_L_VAL_ADDR - HWIO_SILVER_PLL_MODE_ADDR)

#define LOCK_DET_BMSK       HWIO_SILVER_PLL_STATUS_LOCK_DET_BMSK
#define MODE_CTL_MSK        0x7
#define GFMUX_SRC_SEL_BMSK  0x3


#define USER_CTL_SETTINGS      0x00000009
#define CONFIG_CTL_HI_SETTINGS 0x000003D2
#define CONFIG_CTL_LO_SETTINGS 0x20000AA8
#define TEST_CTL_HI_SETTINGS   0x00004000
#define TEST_CTL_LO_SETTINGS   0x04000400
#define GFMUX_STS_SEL_B_BMSK  (0x40000000)

/*=========================================================================
     Externs
==========================================================================*/
extern boolean bClockRailwayInit;
/*=========================================================================
      Function Prototypes
==========================================================================*/
static boolean Clock_EnableAppsPLL(ClockAPCSCfgType *pConfig, uintnt nPLLModeAddr, 
                                   boolean bConfigOnly);
static boolean Clock_DisableAppsPLL(uintnt nPLLModeAddr);

/*=========================================================================
      Data
==========================================================================*/

/* The current rate of the APPS cluster 0 clock in KHz */
static uint32 Clock_APPSCL0SpeedKHz = 1516800;


/*=========================================================================
      Function Definitions
==========================================================================*/

/* ========================================================================
**  Function : Clock_GetAPSSCL0SpeedKHz
** ======================================================================*/
/*
    Description: Configure CPU clock source

    @param
      None.

    @return
      Frequency of APPS cluster zero in KHZ.

    @dependencies
      None.

    @sa
      None.
*/

uint32 Clock_GetAPSSCL0SpeedKHz(void)
{
  return Clock_APPSCL0SpeedKHz;
}


/* ========================================================================
**  Function : Clock_ConfigAPSSClock
** ======================================================================*/
/*
    Description: Configure CPU clock or L3 clock

    @param
      ClkCfg - Clock configuration
      nModeAddr - Clock mode address

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

    @dependencies
      None.

    @sa None
*/
boolean Clock_ConfigAPSSClock( ClockAPCSCfgType *ClkCfg, uintnt nModeAddr )
{
  uint32 nRegVal;

  if ( ClkCfg == NULL ) return FALSE;

  /* Used Agera PLL */
  if ( (ClkCfg->nCPUSource == APCS_SOURCE_AG_EARLY) || 
       (ClkCfg->nCPUSource == APCS_SOURCE_AG_MAIN) )
  {
    /* Temporary switch to GPLL0 if APCS PLL is running */
    if ( (inp32(nModeAddr+STATUS_OFFS) & LOCK_DET_BMSK) != 0 )
    {
      nRegVal = inp32(nModeAddr+GFMUX_OFFS) & ~GFMUX_SRC_SEL_BMSK | APCS_SOURCE_GPLL0_MAIN;
      outp32( (nModeAddr+GFMUX_OFFS), nRegVal );
       /* Poll for GFMUX_STS_SEL_B_BMSK to become 1, which indicates GPLL0_MAIN_CLK is the src of cluster, then disable the agera PLL */
      while( !(inp32(nModeAddr+GFMUX_OFFS) & GFMUX_STS_SEL_B_BMSK));
      Clock_DisableAppsPLL( nModeAddr );
    }

    if ( !Clock_EnableAppsPLL(ClkCfg, nModeAddr, FALSE) ) return FALSE;
    nRegVal = inp32(nModeAddr+GFMUX_OFFS) & ~GFMUX_SRC_SEL_BMSK | ClkCfg->nCPUSource;
    outp32( (nModeAddr+GFMUX_OFFS), nRegVal );
  }
  else if ( ClkCfg->nCPUSource == APCS_SOURCE_GPLL0_MAIN )
  {
    nRegVal = inp32(nModeAddr+GFMUX_OFFS) & ~GFMUX_SRC_SEL_BMSK | APCS_SOURCE_GPLL0_MAIN;
    outp32( (nModeAddr+GFMUX_OFFS), nRegVal );
    Clock_DisableAppsPLL( nModeAddr );
  }
  else /* XO - Ref clk */
  {
    nRegVal = inp32(nModeAddr+GFMUX_OFFS) & ~GFMUX_SRC_SEL_BMSK;
    outp32( (nModeAddr+GFMUX_OFFS), nRegVal );
    Clock_DisableAppsPLL( nModeAddr );
  }

  return TRUE;
}


/* ============================================================================
**  Function : Clock_SetCPUPerfLevel
** ============================================================================
*/
/**
  Configure A53 CPU to a specific perf level.

  @param eCPUPerfLevel [in] - CPU performance level.

  @return
  TRUE -- CPU configuration to perf level was successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/
boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel)
{
  static ClockBootPerfLevelType eCurrLevel = CLOCK_BOOT_PERF_NONE;
  ClockAPCSCfgType AppsPLLDefault = { APCS_SOURCE_AG_EARLY, 34, 0, 0, 652800 };
  Clock_ConfigType *cfg = Clock_Config();
  ClockAPCSCfgType *CPU_Cfg;
  ClockConfigMuxType MuxCfg = { .nCMDRCGRAddr = HWIO_ADDR(GCC_CPUSS_GPLL0_CMD_RCGR),
                                .eMux = MUX_GCC,
                                .eSource = SRC_GPLL0_EVEN,
                                .nDiv2x = 2
                              };

  /* Requirement:  Apps needs 300 MHz GPLL0 for low voltage corners */
  Clock_SetClockConfig(&MuxCfg);

  /* Set CPU speed */
  if ( eCurrLevel == eCPUPerfLevel ) return TRUE;
  if( eCPUPerfLevel >= CLOCK_BOOT_PERF_NUM ) return FALSE;
    
  /* This function is called from Clock_DebugInit, but by that time railway driver will not be ready..
   * bClockRailwayInit will be set to TRUE in the Clock_PreDDRInit function till that time No  rail-switching supported?
   * So incase this API called with TURBO before railway driver init force it NOM to avoid boot up issues
   */
  if(eCPUPerfLevel == CLOCK_BOOT_PERF_TURBO && bClockRailwayInit == FALSE) eCPUPerfLevel = CLOCK_BOOT_PERF_NOMINAL;

  eCurrLevel = eCPUPerfLevel;

  /* Switch the CPU rails to requested PerfLevel*/
  if ( Clock_SwitchRail(CLOCK_RAIL_APC, cfg->aClockMapCorner[eCPUPerfLevel], CLOCK_RAIL_CLIENT_BOOT) == FALSE ) return FALSE;

  CPU_Cfg = &cfg->CPU_Cfg[eCPUPerfLevel];
  if ( Clock_ConfigAPSSClock(CPU_Cfg, HWIO_SILVER_PLL_MODE_ADDR) == FALSE ) return FALSE;
  Clock_APPSCL0SpeedKHz = CPU_Cfg->nFreqKHz;

  /* Set L3 speed */
  if ( Clock_ConfigAPSSClock(&cfg->L3_Cfg[eCPUPerfLevel], HWIO_L3_PLL_MODE_ADDR) == FALSE ) return FALSE;

  
  /* If the silver cluster PLL is not on, set it up as this is a pre-requisite for the OSM FSM. */
  if( (HWIO_IN( SILVER_PLL_MODE ) & (~MODE_CTL_MSK)) != 0x7 )
  {
    Clock_EnableAppsPLL(&AppsPLLDefault, HWIO_SILVER_PLL_MODE_ADDR, FALSE);
  }

  /* Set Gold PLL  up as this is a pre-requisite for the OSM in HLOS. */
  /* At this moment, the gold rail is not powered.  We will defer the PLL lock into */
  /* TZ when the gold rail is powered, and the first gold core is unclamped. */
  Clock_EnableAppsPLL(&AppsPLLDefault, HWIO_GOLD_PLL_MODE_ADDR, TRUE);
  /* Initialize GOLD MUX to be 300 MHz */
  HWIO_OUT(GOLD_CFG_GFMUX, APCS_SOURCE_GPLL0_MAIN);
  return TRUE;
}


/* ============================================================================
**  Function : Clock_EnableAppsPLL
** ============================================================================
    Configure and enable a high frequency Apps PLL.

    @param
      pConfig -  [IN] PLL configuration structure
      nPLLModeAddr - [IN] PLL base address (MODE address)
      bConfigOnly [IN] - Do not lock the PLL, just configure it.

    @return
      TRUE -- Initialization was successful.
      FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
static boolean Clock_EnableAppsPLL
(
  ClockAPCSCfgType *pConfig,
  uintnt           nPLLModeAddr,
  boolean          bConfigOnly
)
{
  if ( (pConfig == NULL) || (pConfig->nLVal==0) || (nPLLModeAddr==0) ) return FALSE;

  /* Return failed if PLL is already enabled */
  if ( (inp32(nPLLModeAddr+STATUS_OFFS) & LOCK_DET_BMSK) != 0 ) return FALSE;

  outp32( (nPLLModeAddr+USER_CTL_OFFS), USER_CTL_SETTINGS );
  outp32( (nPLLModeAddr+CONFIG_CTL_LO_OFFS), CONFIG_CTL_LO_SETTINGS );
  outp32( (nPLLModeAddr+CONFIG_CTL_HI_OFFS), CONFIG_CTL_HI_SETTINGS );
  outp32( (nPLLModeAddr+TEST_CTL_LO_OFFS), TEST_CTL_LO_SETTINGS );
  outp32( (nPLLModeAddr+TEST_CTL_HI_OFFS), TEST_CTL_HI_SETTINGS );
  outp32( (nPLLModeAddr+L_VAL_OFFS), pConfig->nLVal );

  if( bConfigOnly == FALSE) 
  {
  /* Enable PLL */
  outp32( nPLLModeAddr, 0x2 ); // Set BYPASSNL
  busywait(5);
  outp32( nPLLModeAddr, 0x6 ); // Set RESET_N

  /* Poll PLL lock detectin */
  while( (inp32(nPLLModeAddr+STATUS_OFFS) & LOCK_DET_BMSK) == 0 )
  {
    busywait(1);
  }
  outp32( nPLLModeAddr, 0x7 ); // Set OUTCTRL
  }

  return TRUE;

} /* END Clock_EnableAppsPLL */

/* ============================================================================
**  Function : Clock_DisableAppsPLL
** ============================================================================
    Turn off the Apps PLL.

    @param
      pConfig -  [IN] PLL configuration structure

    @return
      void

    @dependencies
    None.

    @sa None
*/
static boolean Clock_DisableAppsPLL
(
  uintnt nPLLModeAddr
)
{
  outp32( nPLLModeAddr, 0 );
  return TRUE;
}

/* ============================================================================
**  Function : Clock_LMhInit
** ============================================================================
*/
/*  Initialize for the current limiter calibration.
 *  Power on A57 at Turbo Voltage(A57 operation and stability is not a requirement)
    L2 headswitch needs to be closed as well.

    @param nA57uV  -  [IN] requested voltage in microvolts
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_LMhInit( uint32 nA57uV )
{

  return TRUE;
}

/* ============================================================================
**  Function : Clock_LMhPreCalibration
** ============================================================================
*/
/*  Set the A57 cores at the maximum clock rate.

    @param  nFreqKHz : The frequency to set in KHz
    @return : rounded freqeuncy that was set, or 0 on failure in kHz.

    @dependencies
    None.

    @sa None
*/
uint32 Clock_LMhPreCalibration( uint32 nFreqKHz )
{
  return 0;
}

/* ============================================================================
**  Function : Clock_LMhDeInit
** ============================================================================
*/
/*  Disable the PLL, turn of the L2 head switch, power off the rail.

    @param  None.
    @return
    TRUE -- Success.
    FALSE -- Reserved, not used.

    @dependencies
    None.

    @sa None
*/
boolean Clock_LMhDeInit( void )
{
  return TRUE;
} /* Clock_LMhDeInit */
