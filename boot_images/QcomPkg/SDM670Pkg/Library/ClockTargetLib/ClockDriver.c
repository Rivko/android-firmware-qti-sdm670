/**
==============================================================================

  @file ClockDriver.c

  Clock driver functions for PLL and clock management

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockDriver.c#1 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockBoot.h"
#include "ClockHWIO.h"
#include "busywait.h"
#include "ClockDriver.h"
#include "ClockBSP.h"

/*=========================================================================
      Macro Definitions
==========================================================================*/

/*
 * Definitions for configuring the PLL in FSM Mode.
 */
#define CLOCK_PLL_BIAS_COUNT_VAL  (0x6 << HWIO_SHFT(GCC_GPLL0_MODE, PLL_BIAS_COUNT))
#define CLOCK_PLL_LOCK_COUNT_VAL  (0x0 << HWIO_SHFT(GCC_GPLL0_MODE, PLL_LOCK_COUNT))

/*
 * CLOCK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define CLOCK_PLL_INIT_TIME_US 5

/*
 * CLOCK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define CLOCK_PLL_WARMUP_TIME_US 25

/*
 * NOT_2D / NOT_N_MINUS_M
 *
 * Macros to return the inverted value of the 2D field or (N - M)
 * in a type 1 mux structure.  Used to prepare the value for writing
 * to the hardware register field.
 */
#define NOT_2D(mux)         (~(mux)->n2D)
#define NOT_N_MINUS_M(mux)  (~((mux)->nN - (mux)->nM))

/*
 * HALF_DIVIDER
 *
 * Macro to return the normalized half divider for a given mux structure.
 * NOTE: Expecting (2 * divider) value as input.
 */
#define HALF_DIVIDER(mux)  ((mux)->nDiv2x ? (((mux)->nDiv2x) - 1) : 0)

/*
 * PLL lock timeout (2x max locking time).
 */
#define CLOCK_PLL_LOCK_TIMEOUT_US 200

/* HW_CTL bit mask */
#define CLK_HW_CTL_MSK 0x2

/* Definitions for generalizing clock configuration */
#define CLOCK_CMD_CFG_UPDATE_FMSK   HWIO_FMSK(GCC_SDCC2_APPS_CMD_RCGR, UPDATE)

#define CLOCK_CFG_REG_OFFSET        (HWIO_ADDR(GCC_SDCC2_APPS_CFG_RCGR)-HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR))
#define CLOCK_CFG_RCGR_SRC_SEL_FMSK  HWIO_FMSK(GCC_SDCC2_APPS_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_RCGR_SRC_SEL_SHFT  HWIO_SHFT(GCC_SDCC2_APPS_CFG_RCGR, SRC_SEL)
#define CLOCK_CFG_RCGR_SRC_DIV_FMSK  HWIO_FMSK(GCC_SDCC2_APPS_CFG_RCGR, SRC_DIV)
#define CLOCK_CFG_RCGR_SRC_DIV_SHFT  HWIO_SHFT(GCC_SDCC2_APPS_CFG_RCGR, SRC_DIV)

#define CLOCK_CFG_RCGR_MODE_FMSK     HWIO_FMSK(GCC_SDCC2_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_RCGR_MODE_SHFT     HWIO_SHFT(GCC_SDCC2_APPS_CFG_RCGR, MODE)
#define CLOCK_CFG_RCGR_MODE_DUAL_EDGE_VAL  0x2
#define CLOCK_CFG_RCGR_MODE_BYPASS_VAL     0x0

#define CLOCK_M_REG_OFFSET         (HWIO_ADDR(GCC_SDCC2_APPS_M)-HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR))
#define CLOCK_N_REG_OFFSET         (HWIO_ADDR(GCC_SDCC2_APPS_N)-HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR))
#define CLOCK_D_REG_OFFSET         (HWIO_ADDR(GCC_SDCC2_APPS_D)-HWIO_ADDR(GCC_SDCC2_APPS_CMD_RCGR))

/*
 * Fabia PLL definitions (GPLL0 is Fabia type in SDM845).
 */
#define FABIA_PLL_MODE                  GCC_GPLL0_MODE
#define FABIA_PLL_L_VAL                 GCC_GPLL0_L_VAL
#define FABIA_PLL_CAL_L_VAL             GCC_GPLL0_CAL_L_VAL
#define FABIA_PLL_USER_CTL              GCC_GPLL0_USER_CTL
#define FABIA_PLL_USER_CTL_U            GCC_GPLL0_USER_CTL_U
#define FABIA_PLL_CONFIG_CTL            GCC_GPLL0_CONFIG_CTL
#define FABIA_PLL_CONFIG_CTL_U          GCC_GPLL0_CONFIG_CTL_U
#define FABIA_PLL_TEST_CTL              GCC_GPLL0_TEST_CTL
#define FABIA_PLL_TEST_CTL_U            GCC_GPLL0_TEST_CTL_U
#define FABIA_PLL_STATUS                GCC_GPLL0_STATUS
#define FABIA_PLL_FREQ_CTL              GCC_GPLL0_FREQ_CTL
#define FABIA_PLL_OPMODE                GCC_GPLL0_OPMODE
#define FABIA_PLL_STATE                 GCC_GPLL0_STATE
#define FABIA_PLL_DROOP                 GCC_GPLL0_DROOP
#define FABIA_PLL_FRAC_VAL              GCC_GPLL0_FRAC_VAL
#define FABIA_PLL_SPARE                 GCC_GPLL0_SPARE

#define FABIA_PLL_REG_ADDR_BASE         FABIA_PLL_MODE
#define FABIA_PLL_REG_ADDR_OFFS(reg) \
    (HWIO_ADDR(FABIA_##reg) - HWIO_ADDR(FABIA_PLL_REG_ADDR_BASE))

#define FABIA_PLL_OPMODE_STANDBY        0x0
#define FABIA_PLL_OPMODE_RUN            0x1
#define FABIA_PLL_OPMODE_FREEZE         0x3

#define FABIA_PLL_CALIB_MODE_ALWAYS     0x0
#define FABIA_PLL_CALIB_MODE_NEVER      0x1
#define FABIA_PLL_CALIB_MODE_AUTOMATIC  0x2

#define FABIA_PLL_FRAC_MODE_ALPHA       0x0
#define FABIA_PLL_FRAC_MODE_M_OVER_N    0x1

#define FABIA_PLL_M_N_FRAC(nM, nN) \
    ((((uint8)(nM)) << 8) | ((uint8)(nN)))

#define FABIA_PLL_MODE_ACTIVE  \
    (HWIO_FMSK(FABIA_PLL_MODE, PLL_OUTCTRL) | \
     HWIO_FMSK(FABIA_PLL_MODE, PLL_RESET_N))

#define FABIA_PLL_IS_ACTIVE(nModeVal) \
    CLK_BITS_ARE_SET(nModeVal, FABIA_PLL_MODE_ACTIVE)


/*=========================================================================
      Function Prototypes
==========================================================================*/
extern Clock_RailwayType ClockRailway;
extern Clock_ConfigType Clock_ConfigData;

extern boolean Clock_SourceMapToMux(const ClockConfigMuxType *pConfig, uint32 *nMuxValue );
boolean Clock_ConfigurePLL(ClockConfigPLLType *pConfig);
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_SourceMapToAOSS(ClockSourceType eSource, uint32 *nMuxValue);
boolean Clock_SourceMapToDisp(ClockSourceType eSource, uint32 *nMuxValue);

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : Clock_EnablePLL
** ============================================================================
*/
/*!
    Enable a PLL.  If it is voteable, it will be set for FSM mode
    and voted for using the vote register. Doesn't configure the PLL.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_EnablePLL( ClockConfigPLLType *pConfig )
{
  uint64 nModeAddr, nVoteAddr;
  uint32 nModeVal, nUserCtlVal, nVoteVal = 0;
  uint32 nTimeout = 0;

  if ( (pConfig == NULL) || (pConfig->nPLLModeAddr == 0) )
  {
    return FALSE;
  }

  nModeAddr = pConfig->nPLLModeAddr;
  nVoteAddr = pConfig->nVoteAddr;
  nModeVal  = inp32(nModeAddr);

  /* Return if the PLL is manually enabled. */
  if ( FABIA_PLL_IS_ACTIVE(nModeVal) )
  {
    return TRUE;
  }

  /* Return if the PLL is FSM voteable and voted for. */
  if(nVoteAddr != 0) {
    nVoteVal = inp32(nVoteAddr);
  }
  if (nModeVal & HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_ENA))
  {
    if ( CLK_BITS_ARE_SET(nVoteVal, pConfig->nVoteMask) )
    {
      if( CLK_BITS_ARE_SET(nModeVal, HWIO_FMSK(FABIA_PLL_MODE, PLL_ACTIVE_FLAG)) )
      {
        return TRUE;
      }
    }
  }

  /*
   * Configure the PLL (if not in FSM mode).
   */
  if ( (nModeVal & HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_ENA)) == 0)
  {
    if ( !Clock_ConfigurePLL(pConfig) ) return FALSE;
  }

  /*
   * Enable the PLL. If there is an address for a voting register, then the
   * PLL is in FSM voteable mode. In this case, the enable sequence is
   * handled in hardware, guided by the BIAS and LOCK counts.
   */
  if ( nVoteAddr != 0 )
  {
    outp32(nVoteAddr, nVoteVal | pConfig->nVoteMask);

    /* Wait for the PLL to go active. */
    while ( CLK_BITS_ARE_CLR(inp32(nModeAddr), HWIO_FMSK(FABIA_PLL_MODE, PLL_ACTIVE_FLAG)) )
    {
      if ( nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US )
      {
        return FALSE;
      }

      busywait(1);
    }
  }
  else
  {
    /* No voting register--follow the manual PLL enable sequence. */
    outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_OPMODE), 0);

    /* Assert the reset_n field to put the PLL in standby state. */
    nModeVal = (inp32(nModeAddr) | HWIO_FMSK(FABIA_PLL_MODE, PLL_RESET_N));
    outp32(nModeAddr, nModeVal);

    /* Set OPMODE to run state. */
    outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_OPMODE), FABIA_PLL_OPMODE_RUN);
    busywait(CLOCK_PLL_WARMUP_TIME_US);

    /* Wait for the PLL to lock within the given timeout period. */
    while ( CLK_BITS_ARE_CLR(inp32(nModeAddr), HWIO_FMSK(FABIA_PLL_MODE, PLL_LOCK_DET)) )
    {
      if ( nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US )
      {
        return FALSE;
      }

      busywait(1);
    }

    /* Enable PLLOUT_MAIN. */
    nUserCtlVal = inp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL));
    nUserCtlVal |= HWIO_FMSK(FABIA_PLL_USER_CTL, PLLOUT_MAIN);
    outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL), nUserCtlVal);

    /* Assert the PLL Output control bit. */
    nModeVal |= HWIO_FMSK(FABIA_PLL_MODE, PLL_OUTCTRL);
    outp32(nModeAddr, nModeVal);
  }

  return TRUE;
}


/* ============================================================================
**  Function : Clock_DisablePLL
** ============================================================================
*/
/*!
    PLL shutdown. When in the off state, the bias is off and draws no power.
    This procedure results in calibration being required upon the next enable.

    @param pConfig -  [IN] PLL configuration structure
    @return
    TRUE -- The PLL was succesfully disabled.
    FALSE -- Disable failed.

    @dependencies None.

    @sa None.
*/
boolean Clock_DisablePLL( ClockConfigPLLType *pConfig )
{
  uint64 nModeAddr;
  uint32 nModeVal;
  uint32 nVoteVal = 0x0;

  if ( (pConfig == NULL) || (pConfig->nPLLModeAddr == 0) )
  {
    return FALSE;
  }

  nModeAddr = pConfig->nPLLModeAddr;

  /* Read current mode value. */
  nModeVal  = inp32(nModeAddr);

  if (nModeVal & HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_ENA))
  {
    if(pConfig->nVoteAddr != 0) 
    {
      nVoteVal = inp32(pConfig->nVoteAddr);
      nVoteVal = (nVoteVal & ~pConfig->nVoteMask);
      outp32(pConfig->nVoteAddr, nVoteVal);
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    /* De-assert the OUTCTL field. */
    nModeVal &= ~HWIO_FMSK(FABIA_PLL_MODE, PLL_OUTCTRL);
    outp32(nModeAddr, nModeVal);

    /* Wait 2 ticks of the output main clock. */
    busywait(1);

    /* Put the PLL in reset mode. */
    nModeVal &= ~HWIO_FMSK(FABIA_PLL_MODE, PLL_RESET_N);
    outp32(nModeAddr, nModeVal);

    /* Set OPMODE to standby state. */
    outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_OPMODE), FABIA_PLL_OPMODE_STANDBY);
  }

  return TRUE;
}


/* =========================================================================
**  Function : Clock_ConfigurePLL
** =========================================================================*/
/**
  Configures a PLL.

  @param *pConfig [in] -- PLL configuration

*/
boolean Clock_ConfigurePLL(ClockConfigPLLType *pConfig)
{
  uint64 nModeAddr;
  uint32 nUserCtlVal, nUserCtlValU;
  uint32 nModeVal = 0;
  
  if ( (pConfig == NULL) || (pConfig->nPLLModeAddr == 0) )
  {
    return FALSE;
  }

  pConfig->bConfig = TRUE;
  nModeAddr = pConfig->nPLLModeAddr;
  nModeVal = inp32(nModeAddr);
  
   /* Return if the PLL is manually enabled. */
  if ( FABIA_PLL_IS_ACTIVE(nModeVal)  ||
       CLK_BITS_ARE_SET(nModeVal, HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_ENA) |
                                  HWIO_FMSK(FABIA_PLL_MODE, PLL_ACTIVE_FLAG)) )
  {
    return TRUE;
  }

  
  /* Clear all bits to put PLL in reset, bypass and output disabled. */
  outp32(nModeAddr, 0x0);

  /* Set CONFIG_CTL and TEST_CTL */
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_CONFIG_CTL), pConfig->nConfigCtl & 0xffffffff);
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_CONFIG_CTL_U), pConfig->nConfigCtl >> 32);
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_TEST_CTL), pConfig->nTestCtl & 0xffffffff);
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_TEST_CTL_U), pConfig->nTestCtl >> 32);
  
  /* Program L, FRAC_VAL, and CAL_L. */
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_L_VAL), pConfig->nL);
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_FRAC_VAL), pConfig->nFracVal);
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_CAL_L_VAL), pConfig->nCalibrationL);

  /* Get the UserCtlVal value and clear out fields we will configure. */
  nUserCtlVal = inp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL));
  nUserCtlVal &= ~(HWIO_FMSK(FABIA_PLL_USER_CTL, PRE_DIV_RATIO)       |
                   HWIO_FMSK(FABIA_PLL_USER_CTL, POST_DIV_RATIO_EVEN) |
                   HWIO_FMSK(FABIA_PLL_USER_CTL, POST_DIV_RATIO_ODD));

  nUserCtlValU = inp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL_U));
  nUserCtlValU |= FABIA_USER_CTL_U_VAL;
  nUserCtlValU &= ~(HWIO_FMSK(FABIA_PLL_USER_CTL_U, FRAC_FORMAT_SEL) |
                    HWIO_FMSK(FABIA_PLL_USER_CTL_U, CALIB_CTRL));

  /* Program the pre-div value (div-1 to div-2). */
  if ( (pConfig->nPreDiv <= 2) && (pConfig->nPreDiv > 0) )
  {
    nUserCtlVal |= ((pConfig->nPreDiv - 1) << HWIO_SHFT(FABIA_PLL_USER_CTL, PRE_DIV_RATIO));
  }

  /*
   * Program the post-div ODD  value (div-1, 3, 5 and 7 are supported)
   */
  if ((pConfig->nPostDivOdd == 3) || (pConfig->nPostDivOdd == 5) ||
      (pConfig->nPostDivOdd == 7))
  {
    nUserCtlVal |= ((pConfig->nPostDivOdd) << HWIO_SHFT(FABIA_PLL_USER_CTL, POST_DIV_RATIO_ODD));
    nUserCtlVal |= HWIO_FMSK(FABIA_PLL_USER_CTL, PLLOUT_ODD);
  }

  /*
   * Program the post-div EVEN  value (div-1, 2, 4 and 8 are supported)
   */
  if ((pConfig->nPostDivEven == 2) || (pConfig->nPostDivEven == 4) ||
      (pConfig->nPostDivEven == 8))
  {
    nUserCtlVal |= ((pConfig->nPostDivEven - 1) << HWIO_SHFT(FABIA_PLL_USER_CTL, POST_DIV_RATIO_EVEN));
    nUserCtlVal |= HWIO_FMSK(FABIA_PLL_USER_CTL, PLLOUT_EVEN);
  }

  /* Set default lock detector to Fine instead of Coarse */
  nUserCtlValU |= HWIO_FMSK(FABIA_PLL_USER_CTL_U, USE_ONLY_FINE_LOCK_DET);
  
  /* Enable auto-calibration option. */
  nUserCtlValU |= ((pConfig->nCalibCtrl) << HWIO_SHFT(FABIA_PLL_USER_CTL_U, CALIB_CTRL));

  /* Program the fractional mode selection. */
  if ( pConfig->nFracFormat > 0 )
  {
    nUserCtlValU |= (pConfig->nFracFormat << HWIO_SHFT(FABIA_PLL_USER_CTL_U, FRAC_FORMAT_SEL));
  }

  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_FRAC_VAL), pConfig->nFracVal);

  /* Program the PLL_USER_CTL_U register. */
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL_U), nUserCtlValU);

  /* Program the PLL_USER_CTL register. */
  outp32(nModeAddr + FABIA_PLL_REG_ADDR_OFFS(PLL_USER_CTL), nUserCtlVal);

  /* 
   * If there is an address for a voting register, then the PLL is in
   * FSM voteable mode. In this case, the enable sequence is handled in
   * hardware, guided by the BIAS and LOCK counts.
   */
  if ( pConfig->nVoteAddr != 0 )
  {
    nModeVal = HWIO_FMSK(FABIA_PLL_MODE, PLL_RESET_N);

    /* Put the FSM in reset. */
    nModeVal |= HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_RESET);
    outp32(nModeAddr, nModeVal);

    /* Program the FSM portion of the mode register. */
    nModeVal &= ~HWIO_FMSK(FABIA_PLL_MODE, PLL_BIAS_COUNT);
    nModeVal &= ~HWIO_FMSK(FABIA_PLL_MODE, PLL_LOCK_COUNT);
    nModeVal |= CLOCK_PLL_BIAS_COUNT_VAL;
    nModeVal |= CLOCK_PLL_LOCK_COUNT_VAL;
    nModeVal |= HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_ENA);

    /* Finally program the Mode register. */
    outp32(nModeAddr, nModeVal);

    /* Take the FSM out of reset. */
    nModeVal &= ~HWIO_FMSK(FABIA_PLL_MODE, PLL_VOTE_FSM_RESET);
    outp32(nModeAddr, nModeVal);
  }

  return TRUE;
}


/* =========================================================================
**  Function : Clock_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux which turn on the source and switch mux.

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
void Clock_SetClockConfig( ClockConfigMuxType *pClockCfg )
{
  /* Used while-loop to trap error for easy triage */
  while( !Clock_EnableSource(pClockCfg->eSource) );
  while( !Clock_ConfigMux(pClockCfg) );
}


/* =========================================================================
**  Function : Clock_ConfigMux
** =========================================================================*/
/*!
    Configure a clock mux. 

    @param pConfig -  [IN] Clock mux config structure
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ConfigMux (const ClockConfigMuxType *pConfig)
{
  uintnt nCmdRCGRAddr, nCfgRCGRAddr;
  uint32 nCmdRCGRVal,  nCfgRCGRVal;
  uintnt nMAddr, nNAddr, nDAddr;
  uint32 nSource  = 0;
  uint32 nTimeout = 0;

  if( ! Clock_SourceMapToMux( pConfig, &nSource) )
  {
    return FALSE;
  }

  nCmdRCGRAddr = pConfig->nCMDRCGRAddr;
  nCmdRCGRVal  = inp32(nCmdRCGRAddr);
  nCfgRCGRAddr = pConfig->nCMDRCGRAddr + CLOCK_CFG_REG_OFFSET; 
  nCfgRCGRVal  = inp32(nCfgRCGRAddr);

  /* Clear the fields. */
  nCfgRCGRVal &= ~(CLOCK_CFG_RCGR_SRC_SEL_FMSK |
                  CLOCK_CFG_RCGR_SRC_DIV_FMSK |
                  CLOCK_CFG_RCGR_MODE_FMSK);

  /* Program the source and divider values. */
  nCfgRCGRVal |= (nSource << CLOCK_CFG_RCGR_SRC_SEL_SHFT)
                  & CLOCK_CFG_RCGR_SRC_SEL_FMSK;
  nCfgRCGRVal |= ((HALF_DIVIDER(pConfig) << CLOCK_CFG_RCGR_SRC_DIV_SHFT)
                  & CLOCK_CFG_RCGR_SRC_DIV_FMSK);

  /* Set MND counter mode depending on if it is in use. */
  if (pConfig->nM != 0 && (pConfig->nM < pConfig->nN))
  {
    nMAddr = pConfig->nCMDRCGRAddr + CLOCK_M_REG_OFFSET;
    nNAddr = pConfig->nCMDRCGRAddr + CLOCK_N_REG_OFFSET;
    nDAddr = pConfig->nCMDRCGRAddr + CLOCK_D_REG_OFFSET;

    outp32(nMAddr, pConfig->nM);
    outp32(nNAddr, NOT_N_MINUS_M(pConfig));
    outp32(nDAddr, NOT_2D(pConfig));

    nCfgRCGRVal |= ((CLOCK_CFG_RCGR_MODE_DUAL_EDGE_VAL << CLOCK_CFG_RCGR_MODE_SHFT)
                    & CLOCK_CFG_RCGR_MODE_FMSK);
  }

  /* Write the final CFG register value. */
  outp32(nCfgRCGRAddr, nCfgRCGRVal);

  /* Trigger the update. */
  nCmdRCGRVal |= CLOCK_CMD_CFG_UPDATE_FMSK;
  outp32(nCmdRCGRAddr, nCmdRCGRVal);

  /* Wait until update finishes. */
  while ( inp32(nCmdRCGRAddr) & CLOCK_CMD_CFG_UPDATE_FMSK )
  {
    if ( nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US )
    {
      return FALSE;
    }

    busywait(1);
  }

  return TRUE;
}

  
/* =========================================================================
**  Function : Clock_TriggerUpdate
** =========================================================================*/
/*!
    Clock switch and wait for UPDATE bit to complete

    @param cmdReg - Address of the CBCR register

    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_TriggerUpdate(uintnt cmdReg, uintnt nMask)
{
  uint32 nTimeout = 0;
  uintnt nVal;

  nVal = inp32(cmdReg) | nMask;

  /* Trigger CMD_REG:UPDATE */
  outp32(cmdReg, nVal);

  /* Wait for CMD_REG:UPDATE to clear, showing the clock switch is complete */
  while (inp32(cmdReg) & nMask)
  {
    if (nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US)
    {
      return FALSE;
    }
    busywait(1);
  }
  return TRUE;
}  
  
/* =========================================================================
**  Function : Clock_ToggleClock
** =========================================================================*/
/*!
    Enable/Disable a Clock and poll for CLK_OFF BIT. 

    @param CBCR_addr - Address of the CBCR register
           enable :-  enable/disable the CBCR 
    TRUE -- CBCR programming successful.
    FALSE -- CBCR programming failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ToggleClock(uintnt CBCR_addr, Clock_CBCRtoggleType toggle_clk)
{
  uintnt CBCR_value;
  
  if(toggle_clk >= NUM_CLK_TOGGLE_TYPES) return FALSE;

  CBCR_value = inp32(CBCR_addr);
  
  if(toggle_clk == CLK_TOGGLE_ENABLE)  
  {
    CBCR_value = CBCR_value | CLK_ENABLE_MSK;
    outp32(CBCR_addr, CBCR_value);  

    /* Poll clock status if the clock is not under HW control */
    if ( (CBCR_value & CLK_HW_CTL_MSK) != CLK_HW_CTL_MSK )
    {
    do
    {
      CBCR_value = inp32(CBCR_addr);
    }while((CBCR_value & CLK_OFF_MSK) != 0);
    }    
  }
  else
  {
    CBCR_value = CBCR_value & ~CLK_ENABLE_MSK;
    outp32(CBCR_addr, CBCR_value);  
  }
  return TRUE;
}


/* ===========================================================================
**  Clock_PowerDomainEnable
** ======================================================================== */

boolean Clock_PowerDomainEnable ( uintnt nGDSCRAddr )
{
  uintnt nVal;
  uintnt nTimeout = 0;

  if( nGDSCRAddr == 0 )
  {
    return FALSE;
  }

  nVal  = inp32(nGDSCRAddr);

  /*
   * Clear the SW PD collapse bit
   */
  nVal &= ~CLK_GDSCR_SW_COLLAPSE_MSK;
  outp32(nGDSCRAddr, nVal);

  /*
   * Wait for PD ON
   */
  while( !(inp32(nGDSCRAddr) & CLK_GDSCR_PWR_ON_MSK) )
  {
    if (nTimeout++ >= CLK_GDSCR_TIMEOUT_US)
    {
      return FALSE;
    }
    busywait(1);
  }
  return TRUE;

} /* END Clock_PowerDomainEnable */


/* ===========================================================================
**  Clock_PowerDomainDisable
** ======================================================================== */

boolean Clock_PowerDomainDisable ( uintnt nGDSCRAddr )
{
  uintnt nVal;

  /* Sanity check */
  if(nGDSCRAddr == 0)
  {
    return FALSE;
  }

  nVal  = inp32(nGDSCRAddr);

  /* Set the SW PD collapse bit */
  nVal |= CLK_GDSCR_SW_COLLAPSE_MSK;
  outp32(nGDSCRAddr, nVal);

  return TRUE;
} /* END Clock_PowerDomainDisable */


boolean (*Clock_MuxMap[NUM_MUX_TYPES])(ClockSourceType, uint32 *) =
{
  Clock_SourceMapToGCC, 
  Clock_SourceMapToAOSS,
  Clock_SourceMapToDisp
};

/*=========================================================================
      Functions
==========================================================================*/

/* ============================================================================
**  Function : Clock_RailwayCfg
** ============================================================================
*/
/*!
    Return a pointer to the Railway configuration data.

   @param  None

   @retval a pointer to the Railway configuration data

*/
Clock_RailwayType *Clock_RailwayConfig( void )
{
  return &ClockRailway;
}

/* ============================================================================
**  Function : Clock_Config
** ============================================================================
*/
/*!
    Return a pointer to the configuration data.

   @param  None

   @retval a pointer to the configuration data

*/
Clock_ConfigType *Clock_Config( void )
{
  return &Clock_ConfigData;
}

/* ============================================================================
**  Function : Clock_SourceMapToMux
** ============================================================================
*/
boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
)
{
  if( (pConfig == NULL) ||
      (nMuxValue == NULL) ||
      pConfig->eMux >= NUM_MUX_TYPES )
  {
    return FALSE;
  }

  return Clock_MuxMap[pConfig->eMux](pConfig->eSource, nMuxValue);
}


/* ============================================================================
**  Function : Clock_SourceMapToGCC
** ============================================================================
*/
/*!
   Map the source enumeration to a physical mux setting for GCC.

   @param  eSource : The source enumeration to map.
   @param  nMuxValue : output parameter.

   @retval a pointer to the configuration data

*/
boolean Clock_SourceMapToGCC(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_GPLL0:
      *nMuxValue = 1;
      break;
    case SRC_GPLL1:
      *nMuxValue = 4;
      break;
    case SRC_GPLL2:
      *nMuxValue = 2;
      break;
    case SRC_GPLL3:
      *nMuxValue = 3;
      break;
    case SRC_GPLL4:
      *nMuxValue = 5;
      break;      
    case SRC_GPLL5:
      *nMuxValue = 3;
      break;      
    case SRC_GPLL6:
      *nMuxValue = 2;
      break;
    case SRC_GPLL0_EVEN:
      *nMuxValue = 6;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SourceMapToAOSS
** ============================================================================
*/
/*!
   Map a ClockSourceType into a physical mux setting for the AOSS muxes.

   @param  None

   @retval a pointer to the configuration data

*/
boolean Clock_SourceMapToAOSS(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_AOPLL0_EVEN:
      *nMuxValue = 1;
      break;
    case SRC_AOPLL1:
      *nMuxValue = 3;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

/* ============================================================================
**  Function : Clock_SourceMapToDisp
** ============================================================================
*/
/*!
   Map a ClockSourceType into a physical mux setting for the AOSS muxes.

   @param  None

   @retval a pointer to the configuration data

*/
boolean Clock_SourceMapToDisp(ClockSourceType eSource, uint32 *nMuxValue)
{
  switch( eSource )
  {
    case SRC_CXO:
      *nMuxValue = 0;
      break;
    case SRC_EXT1:
    case SRC_DISP_PLL0:
      *nMuxValue = 1;
      break;
    case SRC_EXT2:
      *nMuxValue = 2;
      break;
    case SRC_GPLL0:
      *nMuxValue = 4;
      break;
    case SRC_GPLL0_EVEN:
      *nMuxValue = 5;
      break;
    default:
      return FALSE;
  }
  return TRUE;
}
/* ============================================================================
**  Function : Clock_EnableSource
** ============================================================================
*/

boolean Clock_EnableSource( ClockSourceType eSource )
{
  boolean rtrn = TRUE;
  ClockConfigPLLType *pPLLCfg = NULL;

  switch( eSource )
  {
    case SRC_CXO:
      break;

    case SRC_GPLL0:
    case SRC_GPLL0_EVEN:
      pPLLCfg = &Clock_ConfigData.PLL0_Cfg;
      break;

    case SRC_GPLL1:
      pPLLCfg = &Clock_ConfigData.PLL1_Cfg;
      break;

    case SRC_GPLL2:
      pPLLCfg = &Clock_ConfigData.PLL2_Cfg;
      break;

    case SRC_GPLL3:
      pPLLCfg = &Clock_ConfigData.PLL3_Cfg;
      break;

    case SRC_GPLL4:
      pPLLCfg = &Clock_ConfigData.PLL4_Cfg;
      break;

    case SRC_GPLL5:
      pPLLCfg = &Clock_ConfigData.PLL5_Cfg;
      break;

    case SRC_GPLL6:
      pPLLCfg = &Clock_ConfigData.PLL6_Cfg;
      break;

    case SRC_AOPLL0_EVEN:
      pPLLCfg = &Clock_ConfigData.AOSS_PLL0_Cfg;
      break;

    default:
      return FALSE; // Not support PLL
      break;
  }

  if ( pPLLCfg != NULL )
  {
    if ( pPLLCfg->nRefCount == 0 )
    {
      rtrn = Clock_EnablePLL(pPLLCfg);
    }
    pPLLCfg->nRefCount++;
  }
  
  return rtrn;
}

/* ============================================================================
**  Function : Clock_DisableSource
** ============================================================================
*/
boolean Clock_DisableSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;
  ClockConfigPLLType *pPLLCfg = NULL;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;

    case SRC_GPLL0:
    case SRC_GPLL0_EVEN:
      /* There is currently no use case to disable GPLL0 in XBL Loader. */
      break;

    case SRC_GPLL1:
      pPLLCfg = &Clock_ConfigData.PLL1_Cfg;
      break;

    case SRC_GPLL2:
      pPLLCfg = &Clock_ConfigData.PLL2_Cfg;
      break;

    case SRC_GPLL3:
      pPLLCfg = &Clock_ConfigData.PLL3_Cfg;
      break;

    case SRC_GPLL4:
      pPLLCfg = &Clock_ConfigData.PLL4_Cfg;
      break;

    case SRC_GPLL5:
      pPLLCfg = &Clock_ConfigData.PLL5_Cfg;
      break;

    case SRC_GPLL6:
      pPLLCfg = &Clock_ConfigData.PLL6_Cfg;
      break;

    case SRC_AOPLL0_EVEN:
      pPLLCfg = &Clock_ConfigData.AOSS_PLL0_Cfg;
      break;

    default:
      break;
  }
  
  if ( pPLLCfg != NULL )
  {
    if ( pPLLCfg->nRefCount == 1 )
    {
      rtrn = Clock_DisablePLL(pPLLCfg);
    }
    if ( pPLLCfg->nRefCount > 0 )
      pPLLCfg->nRefCount--;
  }
  
  return rtrn;
}

/* ============================================================================
**  Function : Clock_ConfigureSource
** ============================================================================
*/
boolean Clock_ConfigureSource( ClockSourceType eSource )
{
  boolean rtrn = FALSE;

  switch( eSource )
  {
    case SRC_CXO:
      rtrn = TRUE;
      break;

    case SRC_GPLL0:
    case SRC_GPLL0_EVEN:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL0_Cfg);
      break;

    case SRC_GPLL1:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL1_Cfg);
      break;

    case SRC_GPLL2:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL2_Cfg);
      break;

    case SRC_GPLL3:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL3_Cfg);
      break;

    case SRC_GPLL4:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL4_Cfg);
      break;

    case SRC_GPLL5:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL5_Cfg);
      break;

    case SRC_GPLL6:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.PLL6_Cfg);
      break;

    case SRC_AOPLL0_EVEN:
      rtrn = Clock_ConfigurePLL(&Clock_ConfigData.AOSS_PLL0_Cfg);
      break;

    default:
      break;
  }
  return rtrn;
}
