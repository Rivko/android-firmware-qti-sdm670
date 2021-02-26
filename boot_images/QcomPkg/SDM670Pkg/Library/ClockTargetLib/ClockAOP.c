/**
==============================================================================
  @file ClockAOP.c

  Clock functions for enable and share data to AOP.

  Copyright (c) 2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockAOP.c#3 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  02/23/17  vphan   Initial SDM845 revision.

==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockHWIO.h"
#include "ClockBSP.h"
#include "ClockDriver.h"
#include "boot_util.h"
#include "smem.h"
#include "ddr_drivers.h"
#include "busywait.h"
#include "CoreMsgRam.h"

/*=========================================================================
                       MACRO DEFINITIONS
==========================================================================*/
#define CLOCK_SMEM_VERSION 1

#define PIPPO_CONFIG_CTL_VAL 0x4007300000E3D586ULL
#define PIPPO_TEST_CTL_VAL   0x001E000000400000ULL
#define PIPPO_USER_CTL_U_VAL 0x00250022

/*
 * Max Pippo lock time. Max lock time with calibration is expected to be 275us.
 */
#define PIPPO_PLL_LOCK_TIMEOUT_US 600

/*=========================================================================
      Data
==========================================================================*/
typedef struct
{
  uint16  nVersion;         /* A version number to verify matching XBL and RPM data structures */
  uint8   nNumMCStates;     /* Number of MC states */
  uint8   nNumSHUBStates;   /* Number of SHUB states */
  uint16  nBootCPOffset;    /* Offset to boot CP table address */
  uint16  nMCOffset;        /* Offset to MC table address */
  uint16  nSHUBOffset;      /* Offset to SHUB table address */
  uint16  nReserved;        /* Reserved */
} Clock_SMEMInfoType;

/*
 * PLLs configuration data
 */
extern Clock_ConfigType Clock_ConfigData;

/*
 * Clock configure performance levels for inform AOP during initialize
 */
extern ClockCfgCPType Clock_PerfData;

static ddr_clock_plan MCPlan;
static ddr_clock_plan SHUBPlan;


/*=========================================================================
      Function Prototypes
==========================================================================*/
boolean Clock_ConfigAOP( void );

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

/* =========================================================================
**  Function : Clock_EnableAOPPLL1
** =========================================================================*/
/**
  Enable AOP_PLL1.
*/

static boolean Clock_EnableAOPPLL1
(
  void
)
{
  uint32 nVal;
  uint32 nTimeout = 0;

  HWIO_OUT( AOSS_CC_PLL1_L_VAL, 0xA );
  HWIO_OUT( AOSS_CC_PLL1_CONFIG_CTL_U, PIPPO_CONFIG_CTL_VAL >> 32 );
  HWIO_OUT( AOSS_CC_PLL1_CONFIG_CTL,   PIPPO_CONFIG_CTL_VAL & 0xffffffff );
  HWIO_OUT( AOSS_CC_PLL1_TEST_CTL_U,   PIPPO_TEST_CTL_VAL >> 32 );
  HWIO_OUT( AOSS_CC_PLL1_TEST_CTL,     PIPPO_TEST_CTL_VAL & 0xffffffff );

  nVal = 0;
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL, POST_DIV_RATIO_EVEN, 1); /* div-2 */
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL, PLLOUT_MAIN, 1);
  HWIO_OUT(AOSS_CC_PLL1_USER_CTL, nVal);

  nVal = PIPPO_USER_CTL_U_VAL;
  /* Select startup oscillator for input. */
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL_U, INITIAL_INPUT_CLOCK_MUX, 2);

  /* Select internal DCO (internally generated 38.4MHz) for output. */
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL_U, INTERNAL_CLOCK_SELECTION, 2);

  /*
   * Select XO initially by default and allow pll_clock_select to toggle
   * between XO and the internal clock. These values are important for
   * power collapse to work properly. After power collapse the PLL will
   * be in bypass, so it needs to default to XO otherwise everything
   * downstream will be unclocked. The RSC can't control these bits,
   * so they must be set once and never changed.
   */
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL_U, INITIAL_OUTPUT_CLOCK_MUX, 1);
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL_U, REF_CLK_AT_OUT, 0);

  /*
   * 2^CAL_SCALING * F_xo = F_dco. In this case, F_xo is 19.2MHz and F_dco
   * is 38.4MHz, so set CAL_SCALING to 1.
   */
  nVal |= HWIO_FVAL(AOSS_CC_PLL1_USER_CTL_U, CAL_SCALING, 1);

  HWIO_OUT(AOSS_CC_PLL1_USER_CTL_U, nVal);

  /* Enable the PLL. */
  HWIO_OUTF(AOSS_CC_PLL1_MODE, PLL_CLK_XO_PRESENT, 1);
  HWIO_OUTF(AOSS_CC_PLL1_MODE, PLL_RESET_N, 1);
  HWIO_OUTF(AOSS_CC_PLL1_OPMODE, PLL_OPMODE, 1);

  /* Wait for lock, after which the PLL will be calibrated. */
  while (HWIO_INF(AOSS_CC_PLL1_MODE, PLL_LOCK_DET) == 0)
  {
    if (nTimeout++ >= PIPPO_PLL_LOCK_TIMEOUT_US)
    {
      return FALSE;
    }

    busywait(1);
  }

  /* Switch to the internally generated clock and enable the outputs. */
  HWIO_OUTF(AOSS_CC_PLL1_MODE, PLL_CLOCK_SELECT, 1);
  HWIO_OUTF(AOSS_CC_PLL1_MODE, PLL_OUTCTRL, 1);

  return TRUE;

} /* END Clock_EnableAOPPLL1 */


/* ============================================================================
**  Function : Clock_ConfigDDRModeForAOP
** ============================================================================
*/
/*!
    This function is used to make sure that the current DDR setting is in
  DDRCC mode before hand over to AOP for RPMh management.  This API requires
  DDR is following these two HW requirements:
    1 - DDR clocks in DDRCC mode.  If the current settings is not in DDRCC
        mode, search switch to lowest support DDRCC mode
    2 - If there is no supported DDRCC mode, make sure that DDR is in MC channel 1,
        so RPMh can do it first switch to channel 0.
  
  @param
    None
    
  @return
    None
   
  @dependencies
    None.

*/
boolean Clock_ConfigDDRModeForAOP( void )
{
  uint32 nIndex;

  /* Set DDR in DDR CC mode before handle to AOP */
  if ( MCPlan.state[Clock_PerfData.nDDR].mode != DDRCC )
  {
    /* Since current DDR is not in DDRCC mode,
     * search for next support DDR and switch to it
     */
    for ( nIndex = 0; nIndex < MCPlan.num_levels; nIndex++ )
    {
      if ( (MCPlan.state[nIndex].freq_enable==TRUE) && (MCPlan.state[nIndex].mode==DDRCC) )
      {
        return Clock_SetBIMCSpeed( MCPlan.state[nIndex].freq_khz );
      }
    }

    /* Since there is not support DDRCC frequency plan, make sure current
     * DDR is in MC channel 1 */
    if( HWIO_INF(GCC_DDRMC_CH0_CBCR, CLK_ENABLE) == 1 )
    {
      return Clock_SetBIMCSpeed( MCPlan.state[Clock_PerfData.nDDR].freq_khz );
    }
  }

  return TRUE;
}


/* ============================================================================
**  Function : Clock_ShareDataToAOP
** ============================================================================
*/
/*!
    This function is used for saving clock configure performance levels into SMEM
  for sharing with AOP.  AOP needs the know current performance level settings in
  XBL, so it can put the same initial vote during AOP initialize.
  
  @param
    None
    
  @return
    None
   
  @dependencies
    None.

*/
boolean Clock_ShareDataToAOP( void )
{
  Clock_SMEMInfoType* pSMEMInfo;
  uint32 nTip, nSize;
  uint32 smem_size = 
              sizeof(Clock_SMEMInfoType) +
              sizeof(ClockCfgCPType) + 
              (sizeof(FREQ_STATE) * MCPlan.num_levels) +
              (sizeof(FREQ_STATE) * SHUBPlan.num_levels);
  void *pSMEM = smem_alloc(SMEM_CLOCK_INFO, smem_size);

  if(pSMEM == NULL) return FALSE;

  /* Store smem address in msg_ram since there is no smem driver in AOP */
  msg_ram_set_smem_address((uint32)SMEM_CLOCK_INFO, (uint32)pSMEM);

  /* Store SMEMInfo */
  pSMEMInfo = pSMEM;
  pSMEMInfo->nVersion = CLOCK_SMEM_VERSION;
  pSMEMInfo->nNumMCStates = MCPlan.num_levels;
  pSMEMInfo->nNumSHUBStates = SHUBPlan.num_levels;
  pSMEMInfo->nBootCPOffset = sizeof(Clock_SMEMInfoType);
  pSMEMInfo->nMCOffset = pSMEMInfo->nBootCPOffset + sizeof(ClockCfgCPType);
  pSMEMInfo->nSHUBOffset = pSMEMInfo->nMCOffset + (sizeof(FREQ_STATE) * MCPlan.num_levels);

  /* Store BootCfgCP info */
  nTip = sizeof(Clock_SMEMInfoType);
  nSize = sizeof(ClockCfgCPType);
  qmemscpy( (pSMEM+nTip), nSize, &Clock_PerfData, nSize);

  /* Store MC info */
  nTip += nSize;
  nSize = sizeof(FREQ_STATE) * MCPlan.num_levels;
  qmemscpy( (pSMEM+nTip), nSize, &MCPlan.state[0], nSize);

  /* Store SHUB info */
  nTip += nSize;
  nSize = sizeof(FREQ_STATE) * SHUBPlan.num_levels;
  qmemscpy( (pSMEM+nTip), nSize, &SHUBPlan.state[0], nSize);

  nTip += nSize;

  if ( nTip == smem_size )
    return TRUE;
  else
    return FALSE;
}


/* ============================================================================
**  Function : Clock_ConfigAOP
** ============================================================================
*/
/*!
    This function is used for config all necessary clocks, PLLs and saving 
  clock performance settings and DDR info into shared memory (SMEM) for sharing 
  with AOP.
  
  @param
    None
    
  @return
    None
   
  @dependencies
    None.

*/
boolean Clock_ConfigAOP( void )
{
  ClockConfigMuxType clkCfg =     {HWIO_ADDR(AOSS_CC_RO_CMD_RCGR),     0, MUX_AOSS, SRC_AOPLL1,       0, 0, 0, 0};  // 19.2MHz
  ClockConfigMuxType clkAOPCfg =  {HWIO_ADDR(AOSS_CC_AOP_CMD_RCGR),    0, MUX_AOSS, SRC_AOPLL0_EVEN,  0, 0, 0, 0};  // 200MHz
  ClockConfigMuxType clkSWAOCfg = {HWIO_ADDR(AOSS_CC_SWAO_CMD_RCGR),   0, MUX_AOSS, SRC_AOPLL0_EVEN,  4, 0, 0, 0};  // 100MHz
  ClockConfigMuxType clkEUDCfg =  {HWIO_ADDR(AOSS_CC_EUD_AT_CMD_RCGR), 0, MUX_AOSS, SRC_AOPLL0_EVEN, 10, 0, 0, 0};  // 40MHz

  /* Enable AOP PLL1 */
  if ( Clock_EnableAOPPLL1() == FALSE ) return FALSE;
  /* Select AOP_PLL1 as reference for AOP_PLL0 */
  HWIO_OUTF( AOSS_CC_PLL_CLK_SEL, FABIA_CLK_REF_SEL, 1);

  while ( !Clock_EnableSource(SRC_AOPLL0_EVEN) );
  Clock_ConfigMux(&clkAOPCfg);
  Clock_ConfigMux(&clkSWAOCfg);
  Clock_ConfigMux(&clkEUDCfg);

  /* Configure all PLLs before handle to AOP */
  if ( Clock_ConfigData.PLL0_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL0_Cfg) ) return FALSE;
  
  if ( Clock_ConfigData.PLL1_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL1_Cfg) ) return FALSE;

  if ( Clock_ConfigData.PLL2_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL2_Cfg) ) return FALSE;

  if ( Clock_ConfigData.PLL3_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL3_Cfg) ) return FALSE;

  if ( Clock_ConfigData.PLL4_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL4_Cfg) ) return FALSE;

  if ( Clock_ConfigData.PLL5_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL5_Cfg) ) return FALSE;

  if ( Clock_ConfigData.PLL6_Cfg.bConfig == FALSE )
    if ( !Clock_ConfigurePLL(&Clock_ConfigData.PLL6_Cfg) ) return FALSE;



  /* Switch all AOP XO source clocks to AOP PLL1 */ 
  Clock_ConfigMux(&clkCfg);

  clkCfg.nCMDRCGRAddr = HWIO_ADDR(AOSS_CC_SPMI_AOD_SER_CMD_RCGR);
  Clock_ConfigMux(&clkCfg);

  clkCfg.nCMDRCGRAddr = HWIO_ADDR(AOSS_CC_SWAO_TS_CMD_RCGR);
  Clock_ConfigMux(&clkCfg);

  HWIO_OUTF( AOSS_CC_AOP_RO_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_PWR_MUX_CTRL_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_AO_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_MEM_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_TS_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_TS_AO_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_SWAO_BUS_CBCR, CLK_ENABLE, 1 );
  HWIO_OUTF( AOSS_CC_GLOBAL_CNTR_CBCR, CLK_ENABLE, 1 );

  MCPlan = ddr_clock_query(MC_CLOCK_PLAN);
  SHUBPlan = ddr_clock_query(SHUB_CLOCK_PLAN);

  /* Configure DDR mode before handle to AOP */
  if ( !Clock_ConfigDDRModeForAOP() ) return FALSE;

  /* Store config data to SMEM for sharing with AOP */
  return Clock_ShareDataToAOP();

}
