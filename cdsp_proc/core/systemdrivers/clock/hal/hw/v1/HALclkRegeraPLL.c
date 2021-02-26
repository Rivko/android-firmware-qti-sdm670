/*
==============================================================================

FILE:         HALclkRegeraPLL.c

DESCRIPTION:
  This file contains the clock HAL code for the Regera PLL control.
  These PLLs are of the Regera PLL variety.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/hal/hw/v1/HALclkRegeraPLL.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/07/14   sr      Created

==============================================================================
    Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkGenericPLL.h"
#include "HALclkRegeraPLL.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * HAL_CLK_PLL_MODE_*
 *
 * Definitions for configuring the PLLn_MODE register.
 */
#define HAL_CLK_PLL_MODE_ACTIVE  \
  (HWIO_FMSK(PLL_MODE, OUTCTRL)  | \
   HWIO_FMSK(PLL_MODE, BYPASSNL) | \
   HWIO_FMSK(PLL_MODE, RESET_N))

/*
 * Definitions for configuring the PLL in FSM Mode
 */
#define HAL_CLK_PLL_BIAS_COUNT_VAL   (0x6 << HWIO_SHFT(PLL_MODE, BIAS_COUNT))
#define HAL_CLK_PLL_LOCK_COUNT_VAL   (0x0 << HWIO_SHFT(PLL_MODE, LOCK_COUNT))

/*
 * HAL_CLK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define HAL_CLK_PLL_INIT_TIME_US 5

/*
 * HAL_CLK_PLL_WARMUP_TIME_US
 *
 * Warm-up time in microseconds after turning on the PLL.
 */
#define HAL_CLK_PLL_WARMUP_TIME_US 50

/*
 * HAL_CLK_LOCK_TIMEOUT_US
 *
 * Timeout in usec for PLL_LOCK_DET to get set.
 * For Regera, this is 50us worst-case lock time.
 */
#define HAL_CLK_LOCK_TIMEOUT_US 50

/*
 * HAL_CLK_UPDATED_CONFIG_CTL_VAL
 */
#define HAL_CLK_UPDATED_CONFIG_CTL_LO_VAL    0x204456BB

/*
 * HAL_CLK_UPDATED_CONFIG_CTL_U_VAL
 */
#define HAL_CLK_UPDATED_CONFIG_CTL_HI_VAL    0x00002067

/*
 * HAL_CLK_UPDATED_TEST_CTL_VAL
 */
#define HAL_CLK_UPDATED_TEST_CTL_LO_VAL      0x0

/*
 * HAL_CLK_UPDATED_TEST_CTL_U_VAL
 */
#define HAL_CLK_UPDATED_TEST_CTL_HI_VAL      0x0


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
 * HAL_clk_RegeraPLLControl
 *
 * Control function array for the Regera PLLs.
 */
HAL_clk_SourceControlType HAL_clk_RegeraPLLControl =
{
  .Enable           = HAL_clk_RegeraPLLEnable,
  .Disable          = HAL_clk_RegeraPLLDisable,
  .IsEnabled        = HAL_clk_RegeraPLLIsEnabled,
  .Config           = HAL_clk_RegeraPLLConfig,
  .ConfigPLL        = HAL_clk_RegeraPLLConfigPLL,
  .DetectPLLConfig  = HAL_clk_RegeraPLLDetectPLLConfig,
  .EnableVote       = HAL_clk_RegeraPLLEnableVote,
  .DisableVote      = HAL_clk_RegeraPLLDisableVote,
  .IsVoteEnabled    = HAL_clk_RegeraPLLIsVoteEnabled,
  .IsPLLConfigEqual = HAL_clk_RegeraPLLIsPLLConfigEqual,
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/

/* ===========================================================================
**  HAL_clk_RegeraPLLEnableVote
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLEnableVote
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32                  nVal;
  boolean                 bResult;

  /*
   * Set the appropriate bit.
   */
  nVal = inpdw(pSource->VoterRegister.nAddr) | pSource->VoterRegister.nMask;
  outpdw(pSource->VoterRegister.nAddr, nVal);

  bResult =
    HAL_clk_ValidateRegSetTimeout(
      HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
      HWIO_FMSK(PLL_MODE, PLL_ACTIVE_FLAG),
      HAL_CLK_LOCK_TIMEOUT_US);

  return bResult;

} /* END HAL_clk_RegeraPLLEnableVote */


/* ===========================================================================
**  HAL_clk_RegeraPLLDisableVote
**
** ======================================================================== */

static void HAL_clk_RegeraPLLDisableVote
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32 nVal;

  /*
   * Clear the appropriate bit.
   */
  nVal = inpdw(pSource->VoterRegister.nAddr) & ~pSource->VoterRegister.nMask;
  outpdw(pSource->VoterRegister.nAddr, nVal);

} /* END HAL_clk_RegeraPLLDisableVote */


/* ===========================================================================
**  HAL_clk_RegeraPLLIsVoteEnabled
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLIsVoteEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  /*
   * Check the appropriate bit.
   */
  if (inpdw(pSource->VoterRegister.nAddr) & pSource->VoterRegister.nMask)
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_RegeraPLLIsVoteEnabled */


/* ===========================================================================
**  HAL_clk_RegeraPLLPostDivToHW
**
** ======================================================================== */

static uint32 HAL_clk_RegeraPLLPostDivToHW (uint32 nDivider)
{
  switch (nDivider)
  {
    case 0:
    case 1:   return 0; /* Divide-by-1 */
    case 2:   return 1; /* Divide-by-2 */
    case 4:   return 3; /* Divide-by-4 */
    case 8:   return 2; /* Divide-by-8 */
    default:
      return 0;
  }

} /* END HAL_clk_RegeraPLLPostDivToHW */


/* ===========================================================================
**  HAL_clk_RegeraPLLPostDivFromHW
**
** ======================================================================== */

static uint32 HAL_clk_RegeraPLLPostDivFromHW (uint32 nDivider)
{
  switch (nDivider)
  {
    case 0:   return 1; /* Divide-by-1 */
    case 1:   return 2; /* Divide-by-2 */
    case 2:   return 8; /* Divide-by-8 */
    case 3:   return 4; /* Divide-by-4 */
    default:
      return 1;
  }

} /* END HAL_clk_RegeraPLLPostDivFromHW */



/* ===========================================================================
**  HAL_clk_RegeraPLLEnable
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLEnable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32                  nModeVal;
  boolean                 bResult;

  /*
   * Check if PLL is already enabled and return
   */
  if (HAL_clk_RegeraPLLIsEnabled(pSource))
  {
    return TRUE;
  }

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * De-assert PLL bypass.
   * This latches L and Alpha values.
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, BYPASSNL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Wait for PLL init time
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_INIT_TIME_US);

  /*
   * De-assert PLL reset.
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, RESET_N);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Wait for the PLL warm-up time.
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_WARMUP_TIME_US);

  /*
   * Wait for the PLL to lock within the timeout period.
   */
  bResult =
    HAL_clk_ValidateRegSetTimeout(
      HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
      HWIO_FMSK(PLL_MODE, LOCK_DET),
      HAL_CLK_LOCK_TIMEOUT_US);

  /*
   * Return immediately if the lock detect failed.
   */
  if (!bResult)
  {
    return FALSE;
  }

  /*
   * Put the PLL in active mode
   */
  nModeVal |= HWIO_FMSK(PLL_MODE, OUTCTRL);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  return TRUE;

} /* END HAL_clk_RegeraPLLEnable */


/* ===========================================================================
**  HAL_clk_RegeraPLLDisable
**
** ======================================================================== */

static void HAL_clk_RegeraPLLDisable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32                  nModeVal;

  /*
   * The Regera PLL's only support full enable/disable mode.
   */
  if (eMode != HAL_CLK_SOURCE_DISABLE_MODE_NORMAL)
  {
    return;
  }

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * First clear the output enable bit
   */
  nModeVal &= ~(HWIO_FMSK(PLL_MODE, OUTCTRL));
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Put the PLL in off state
   */
  nModeVal &= ~(HWIO_FMSK(PLL_MODE, RESET_N) |
                HWIO_FMSK(PLL_MODE, BYPASSNL));
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

} /* END HAL_clk_RegeraPLLDisable */


/* ===========================================================================
**  HAL_clk_RegeraPLLIsEnabled
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLIsEnabled
(
  HAL_clk_SourceDescType *pSource
)
{
  uint32                  nModeVal;

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * Check whether the PLL is enabled.
   */
  return ((nModeVal & HAL_CLK_PLL_MODE_ACTIVE) == HAL_CLK_PLL_MODE_ACTIVE);

} /* END HAL_clk_RegeraPLLIsEnabled */


/* ===========================================================================
**  HAL_clk_RegeraPLLConfig
**
** ======================================================================== */

static void HAL_clk_RegeraPLLConfig
(
  HAL_clk_SourceDescType   *pSource,
  HAL_clk_SourceConfigType  eConfig
)
{
  /*
   * Take action.
   */
  switch (eConfig)
  {
    case HAL_CLK_CONFIG_PLL_FSM_MODE_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, FSM_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, FSM_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX2_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX2_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_TEST_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_TEST_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_TEST_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_TEST_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, LVEARLY_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, LVEARLY_EN, 0);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, LVMAIN_EN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, LVMAIN_EN, 0);
      break;

    default:
      break;
  }

} /* END HAL_clk_RegeraPLLConfig */


/* ===========================================================================
**  HAL_clk_RegeraPLLConfigPLLNormal
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLConfigPLLNormal
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig
)
{
  uint32  nModeVal;
  uint32  nUserVal;
  boolean bPLLEnabled = FALSE;
  uint32  nPLLInFSMMode = 0;

  /*
   * Check PLL FSM mode.
   */
  nPLLInFSMMode = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource) &
    HWIO_FMSK(PLL_MODE, FSM_EN);

  /*
   * Check PLL mode.
   */
  bPLLEnabled = HAL_clk_RegeraPLLIsEnabled(pSource);

  /*
   * Disable the PLL.
   */
  if (bPLLEnabled)
  {
    HAL_clk_RegeraPLLDisable(pSource, HAL_CLK_SOURCE_DISABLE_MODE_NORMAL, NULL);
  }

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * Disable FSM Mode
   */
  if (nPLLInFSMMode != 0)
  {
    nModeVal &= ~HWIO_FMSK(PLL_MODE, FSM_EN);
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);
  }

  /*
   * Program updated config/test/user register values as instructed by HW.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL_LO, pSource, HAL_CLK_UPDATED_CONFIG_CTL_LO_VAL);
  HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL_HI, pSource, HAL_CLK_UPDATED_CONFIG_CTL_HI_VAL);
  HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL_LO, pSource, HAL_CLK_UPDATED_TEST_CTL_LO_VAL);
  HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL_HI, pSource, HAL_CLK_UPDATED_TEST_CTL_HI_VAL);

  /*
   * Program L/Alpha values.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pSource, pConfig->nL);
  HAL_CLK_PLL_HWIO_OUT(PLL_ALPHA_VAL, pSource, pConfig->nFracVal);

  /*
   * Get the CONFIG value and clear out fields we will configure.
   */
  nUserVal  =  HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);
  nUserVal &= ~(HWIO_FMSK(PLL_USER_CTL, ALPHA_MODE) |
                HWIO_FMSK(PLL_USER_CTL, MN_ACC) |
                HWIO_FMSK(PLL_USER_CTL, CLK_REF_PRE_DIV) |
                HWIO_FMSK(PLL_USER_CTL, CLK_POST_DIV));

  /*
   * Program the pre-div value (div-1 to div-2).
   */
  if (pConfig->nPreDiv <= 2 && pConfig->nPreDiv > 0)
  {
    nUserVal |= HWIO_FVAL(PLL_USER_CTL, CLK_REF_PRE_DIV, (pConfig->nPreDiv - 1));
  }

  /*
   * Program the post-div value.
   */
  nUserVal |= HWIO_FVAL(PLL_USER_CTL, CLK_POST_DIV,
    HAL_clk_RegeraPLLPostDivToHW(pConfig->PostDivs.Regera.nMain));

  /*
   *  Check fractional output or integer output.
   */
  if (pConfig->eFracMode == HAL_CLK_PLL_FRAC_MODE_ALPHA)
  {
    /*
     * Alpha Mode
     */
    nUserVal |= HWIO_FMSK(PLL_USER_CTL, MN_ACC);
  }
  else if (pConfig->eFracMode == HAL_CLK_PLL_FRAC_MODE_MN)
  {
    /*
     * M/N Mode
     */
    nUserVal |= HWIO_FMSK(PLL_USER_CTL, MN_ACC);
    nUserVal |= HWIO_FMSK(PLL_USER_CTL, ALPHA_MODE);
  }

  /*
   * Enable MAIN_OUT_ENA bit.
   */
  nUserVal |= HWIO_FMSK(PLL_USER_CTL, LVMAIN_EN);

  /*
   * Finally program the USER_CTL register.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pSource, nUserVal);

  /*
   * If there is an address for a voting register, then the PLL is in
   * FSM voteable mode. In this case, the enable sequence is handled in
   * hardware, guided by the BIAS and LOCK counts.
   */
  if (nPLLInFSMMode)
  {
    /*
     * Put the FSM in reset.
     */
    nModeVal |= HWIO_FMSK(PLL_MODE, FSM_RES);
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

    /*
     * Program the FSM portion of the mode register.
     */
    nModeVal &= ~HWIO_FMSK(PLL_MODE, BIAS_COUNT);
    nModeVal &= ~HWIO_FMSK(PLL_MODE, LOCK_COUNT);

    nModeVal |= HAL_CLK_PLL_BIAS_COUNT_VAL;
    nModeVal |= HAL_CLK_PLL_LOCK_COUNT_VAL;

    nModeVal |= HWIO_FMSK(PLL_MODE, FSM_EN);
    /*
     * Finally program the Mode register.
     */
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

    /*
     * Take the FSM out of reset.
     */
    nModeVal &= ~HWIO_FMSK(PLL_MODE, FSM_RES);
    HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);
  }

  /*
   * Enable the PLL if it was on before.
   */
  if (bPLLEnabled && nPLLInFSMMode == 0)
  {
    HAL_clk_RegeraPLLEnable(pSource, HAL_CLK_SOURCE_DISABLE_MODE_NORMAL, NULL);
  }

  return TRUE;

} /* END HAL_clk_RegeraPLLConfigPLLNormal */


/* ===========================================================================
**  HAL_clk_RegeraPLLConfigPLLSlew
**
**  Returns:
**    boolean - TRUE if successfully configured.
**              FALSE if unable to detect that the PLL successfully locked.
** ======================================================================== */

static boolean HAL_clk_RegeraPLLConfigPLLSlew
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig
)
{
  boolean                 bResult;

  /*
   * Program L value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pSource, pConfig->nL);

  /*
   * Wait for 11 or more PLL clk_ref ticks for PLL_LOCK_DET to transition to 0.
   * When using a reference clock of XO, the wait requirement is 570 ns.
   */
  HAL_clk_BusyWait(1);

  /*
   * Wait for the PLL to lock during the 15 us timeout period.
   */
  bResult =
    HAL_clk_ValidateRegSetTimeout(
      HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
      HWIO_FMSK(PLL_MODE, LOCK_DET),
      HAL_CLK_LOCK_TIMEOUT_US);

  return bResult;

} /* END HAL_clk_RegeraPLLConfigPLLSlew */


/* ===========================================================================
**  HAL_clk_RegeraPLLConfigPLL
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLConfigPLL
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig,
  HAL_clk_SourceConfigMode     eMode
)
{
  boolean bResult = FALSE;

  switch (eMode)
  {
    case HAL_CLK_SOURCE_CONFIG_MODE_NORMAL:
      bResult = HAL_clk_RegeraPLLConfigPLLNormal(pSource, pConfig);
      break;
    case HAL_CLK_SOURCE_CONFIG_MODE_SLEW:
      bResult = HAL_clk_RegeraPLLConfigPLLSlew(pSource, pConfig);
      break;
    default:
      bResult = FALSE;
      break;
  }

  return bResult;

} /* END HAL_clk_RegeraPLLConfigPLL */


/* ===========================================================================
**  HAL_clk_RegeraPLLDetectPLLConfig
**
** ======================================================================== */

static void HAL_clk_RegeraPLLDetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  uint32                  nConfigVal;
  uint32                  nMode;
  uint32                  nPostDiv;

  /*
   * Clear all config values not modified in this function.
   */
  pConfig->nMode = 0;

  /*
   * Read current L and Alpha values.
   */
  nMode = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);

  /*
   * Check to see if fractional value is enabled
   */
  pConfig->nFracVal = HAL_CLK_PLL_HWIO_IN(PLL_ALPHA_VAL, pSource);
  if ( nMode & HWIO_FMSK(PLL_USER_CTL, MN_ACC) )
  {
    if ( nMode & HWIO_FMSK(PLL_USER_CTL, ALPHA_MODE) )
    {
      pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_MN;
    }
    else
    {
      pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_ALPHA;
    }
  }
  else
  {
    pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_UNKNOWN;
  }

  /*
   * Read current L-Val
   */
  pConfig->nL = HAL_CLK_PLL_HWIO_IN(PLL_L_VAL, pSource);

  /*
   * Determine the oscillator source.
   */
  pConfig->nSource = 0;

  /*
   * Get the pre and post dividers.
   */
  nConfigVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);
  pConfig->nPreDiv = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, CLK_REF_PRE_DIV))
                      >> (HWIO_SHFT(PLL_USER_CTL, CLK_REF_PRE_DIV))) + 1;
  nPostDiv  = ((nConfigVal & HWIO_FMSK(PLL_USER_CTL, CLK_POST_DIV))
                          >> HWIO_SHFT(PLL_USER_CTL, CLK_POST_DIV));
  nPostDiv = HAL_clk_RegeraPLLPostDivFromHW(nPostDiv);
  pConfig->PostDivs.Regera.nMain = nPostDiv;

} /* END HAL_clk_RegeraPLLDetectPLLConfig */


/* ===========================================================================
**  HAL_clk_RegeraPLLIsPLLConfigEqual
**
** ======================================================================== */

static boolean HAL_clk_RegeraPLLIsPLLConfigEqual
(
  const HAL_clk_PLLConfigType  *pConfigA,
  const HAL_clk_PLLConfigType  *pConfigB
)
{
  uint32 nMainDivA, nMainDivB;

  if (HAL_clk_GenericIsPLLConfigEqual(pConfigA, pConfigB))
  {
    /* Treat div-0 and div-1 as the same thing. */
    nMainDivA = MAX(1, pConfigA->PostDivs.Regera.nMain);
    nMainDivB = MAX(1, pConfigB->PostDivs.Regera.nMain);

    if (nMainDivA == nMainDivB)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* END HAL_clk_RegeraPLLIsPLLConfigEqual */

