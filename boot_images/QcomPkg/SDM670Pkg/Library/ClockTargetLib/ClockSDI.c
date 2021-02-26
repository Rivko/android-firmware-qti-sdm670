/**
==============================================================================

  @file ClockSDI.c
  
  Clock functions for support System Debug Image driver.  Since SDI image is
  required to be every small memory footprint, clock drivers only brings in
  necessary code to support it.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockSDI.c#1 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ClockHWIO.h"
#include "ClockDriver.h"

/*=========================================================================
                       MACRO DEFINITIONS
==========================================================================*/

#define CLK_ELEM_SIZE(array)      sizeof((array)[0])
#define CLK_NUM_ELEM(array)       (sizeof(array) / CLK_ELEM_SIZE(array))

#define CLK_BITS_ARE_CLR(nVal, nMask)    (((nVal) & (nMask)) == 0)
#define CLK_BITS_ARE_SET(nVal, nMask)    (((nVal) & (nMask)) == (nMask))

#define CLK_ENABLE_MSK 0x00000001
#define CLK_OFF_MSK    0x80000000
#define CLK_GDSCR_SW_COLLAPSE_MSK 0x00000001
#define CLK_GDSCR_PWR_ON_MSK      0x80000000
#define CLK_GDSCR_TIMEOUT_US      150

/* HW_CTL bit mask */
#define CLK_HW_CTL_MSK 0x2

/*
 * ClockSDIConfigMuxType
 *
 * Parameters used for configuring a standard clock multiplexer.
 *
 *  nCMDRCGRAddr - The address of the CMD RCGR register.
 *  eSource     - The source to use.
 *  nDiv2x      - The integer (2 * divider) value to use.
 *  nM          - The M value for any M/N counter, or 0 to bypass.
 *  nN          - The N value for any M/N counter.
 *  n2D         - Twice the D value for any M/N counter.
 */
typedef struct ClockSDIConfigMux
{
  uintnt nCMDRCGRAddr;
  uint32 eSource;   // Originally 'ClockSourceType'.
  uint32 nDiv2x;
} ClockSDIConfigMuxType;

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
      Data 
==========================================================================*/

ClockConfigPLLType ClockSDI_GPLL0 = 
{
  .bConfig       = FALSE,
  .nRefCount     = 0,
  .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL0_MODE),
  .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
  .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL0),
  .nCalibCtrl    =  2, /* 2 = Automatic */
  .nPreDiv       =  1,
  .nPostDivOdd   =  0,
  .nPostDivEven  =  2,
  .nL            =  31,
  .nCalibrationL =  0,
  .nFracVal      =  0x4000,
  .nFracFormat   =  0, /* 0 = Alpha */
  .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
  .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
};

ClockConfigPLLType ClockSDI_GPLL2 = 
{
  .bConfig       = FALSE,
  .nRefCount     = 0,
  .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL2_MODE),
  .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
  .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL2),
  .nCalibCtrl    =  2, /* 2 = Automatic */
  .nPreDiv       =  1,
  .nPostDivOdd   =  0,
  .nPostDivEven  =  0,
  .nL            =  20,
  .nCalibrationL =  0,
  .nFracVal      =  0xD555,
  .nFracFormat   =  0, /* 0 = Alpha */
  .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
  .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
};

ClockConfigPLLType ClockSDI_GPLL4 = 
{
  .bConfig       = FALSE,
  .nRefCount     = 0,
  .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL4_MODE),
  .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
  .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL4),
  .nCalibCtrl    =  2, /* 2 = Automatic */
  .nPreDiv       =  1,
  .nPostDivOdd   =  0,
  .nPostDivEven  =  0,
  .nL            =  41,
  .nCalibrationL =  0,
  .nFracVal      =  0xFAAA,
  .nFracFormat   =  0, /* 0 = Alpha */
  .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
  .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
};
/*=========================================================================
      Function Prototypes
==========================================================================*/
static boolean ClockSDI_TriggerUpdate ( uintnt cmdReg, uintnt nMask );
static boolean ClockSDI_ToggleClock   ( uintnt CBCR_addr, Clock_CBCRtoggleType toggle_clk );
static boolean ClockSDI_ConfigurePLL  ( const ClockConfigPLLType *pConfig );
static boolean ClockSDI_ConfigMux     ( const ClockSDIConfigMuxType *pConfig );
static boolean ClockSDI_EnablePLL     ( const ClockConfigPLLType *pConfig );


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : ClockSDI_EnablePLL
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
static boolean ClockSDI_EnablePLL(const ClockConfigPLLType *pConfig)
{
  uint64 nModeAddr, nVoteAddr;
  uint32 nModeVal, nVoteVal = 0;
  //uint32 nTimeout = 0;

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
    if ( !ClockSDI_ConfigurePLL(pConfig) ) return FALSE;
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
      // SDI does not appear to have busywait()
      // if ( nTimeout++ >= CLOCK_PLL_LOCK_TIMEOUT_US )
      // {
      //   return FALSE;
      // }

      // busywait(1);
    }
  }
  else
  {
    // There is no support in SDI for a non-FSM PLL.
    // Lack of a calibrated busywait() is problematic.
    return FALSE;
  }

  return TRUE;
}



/* =========================================================================
**  Function : ClockSDI_ConfigurePLL
** =========================================================================*/
/**
  Configures a PLL.

  @param *pConfig [in] -- PLL configuration

*/
static boolean ClockSDI_ConfigurePLL(const ClockConfigPLLType *pConfig)
{
  uint64 nModeAddr;
  uint32 nUserCtlVal, nUserCtlValU;
  uint32 nModeVal = 0;
  
  if ( (pConfig == NULL) || (pConfig->nPLLModeAddr == 0) )
  {
    return FALSE;
  }

  //pConfig->bConfig = TRUE;
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
**  Function : ClockSDI_ConfigMux
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
static boolean ClockSDI_ConfigMux (const ClockSDIConfigMuxType *pConfig)
{
  uintnt nCmdRCGRAddr, nCfgRCGRAddr;
  uint32 nCmdRCGRVal,  nCfgRCGRVal;
  uint32 nSource  = 0;

  nSource = pConfig->eSource;

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

  /* Write the final CFG register value. */
  outp32(nCfgRCGRAddr, nCfgRCGRVal);

  /* Trigger the update. */
  nCmdRCGRVal |= CLOCK_CMD_CFG_UPDATE_FMSK;
  outp32(nCmdRCGRAddr, nCmdRCGRVal);

  /* Wait until update finishes. */
  while ( inp32(nCmdRCGRAddr) & CLOCK_CMD_CFG_UPDATE_FMSK );

  return TRUE;
}


/* =========================================================================
**  Function : ClockSDI_TriggerUpdate
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
static boolean ClockSDI_TriggerUpdate(uintnt cmdReg, uintnt nMask)
{
  //uint32 nTimeout = 0;
  uintnt nVal;

  nVal = inp32(cmdReg) | nMask;

  /* Trigger CMD_REG:UPDATE */
  outp32(cmdReg, nVal);

  /* Wait for CMD_REG:UPDATE to clear, showing the clock switch is complete */
  while (inp32(cmdReg) & nMask)
  {
    // SDI lacks busywait()
    // if (nTimeout++ >= CLOCK_UPDATE_TIMEOUT_US)
    // {
    //   return FALSE;
    // }
    // busywait(1);
  }
  return TRUE;
}


/* =========================================================================
**  Function : ClockSDI_ToggleClock
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
static boolean ClockSDI_ToggleClock(uintnt CBCR_addr, Clock_CBCRtoggleType toggle_clk)
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




/* ========================================================================
**  Function : Clock_ConfigureDDR
** ======================================================================*/
/*
    Description: Configure all clocks needed for DDR configuration.  This
    extension API is used for bootup and emergency download mode.

    @param None
    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_ConfigureDDR( void )
{
  ClockSDIConfigMuxType clkCfgDDR = {HWIO_ADDR(GCC_DDRMC_CH0_ROOT_CMD_RCGR), 2, 4}; // 200 MHz
  ClockSDIConfigMuxType clkCfgSHRM = {HWIO_ADDR(GCC_SHRM_CMD_RCGR), 1, 3};          // 400 MHz
  ClockSDIConfigMuxType clkCfgMemNOC = {HWIO_ADDR(GCC_MEMNOC_CMD_RCGR), 5, 0};      // 806 MHz
  
  (void)ClockSDI_EnablePLL( &ClockSDI_GPLL2 );
  (void)ClockSDI_ConfigMux( &clkCfgDDR );

  /* Enable all MemNOC clocks */
  ClockSDI_ToggleClock(HWIO_GCC_MSS_Q6_MEMNOC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  (void)ClockSDI_EnablePLL( &ClockSDI_GPLL0 );
  (void)ClockSDI_ConfigMux( &clkCfgSHRM );
  (void)ClockSDI_EnablePLL( &ClockSDI_GPLL4 );
  (void)ClockSDI_ConfigMux( &clkCfgMemNOC );

  ClockSDI_ToggleClock(HWIO_GCC_SHRM_CFG_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_SHRM_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_PIMEM_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_PIMEM_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_SYS_NOC_PIMEM_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_SYS_NOC_GC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  HWIO_OUTF(GCC_MMNOC_GDSCR, SW_COLLAPSE, 0);
  while( HWIO_INF(GCC_MMNOC_GDSCR, PWR_ON) == 0);

  ClockSDI_ToggleClock(HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  ClockSDI_ToggleClock( HWIO_ADDR(GCC_DNOC_CFG_CBCR), CLK_TOGGLE_ENABLE );
  ClockSDI_ToggleClock( HWIO_ADDR(GCC_SNOC_QOSGEN_EXTREF_CBCR), CLK_TOGGLE_ENABLE );

  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_XO_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_SLEEP_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  // ClockSDI_ToggleClock(HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_TCU_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_SYS_NOC_GC_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_SYS_NOC_SF_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_TURING_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_MSS_Q6_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRSS_GPU_AXI_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  ClockSDI_ToggleClock(HWIO_GCC_DDR_I_HCLK_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_GCC_DDRMC_CH0_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  ClockSDI_ToggleClock(HWIO_DISP_CC_MDSS_RSCC_AHB_CBCR_ADDR, CLK_TOGGLE_ENABLE);
  ClockSDI_ToggleClock(HWIO_DISP_CC_MDSS_RSCC_VSYNC_CBCR_ADDR, CLK_TOGGLE_ENABLE);

  return TRUE;
}
