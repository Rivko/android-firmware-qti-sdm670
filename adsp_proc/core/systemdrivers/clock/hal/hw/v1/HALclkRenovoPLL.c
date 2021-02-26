/*
==============================================================================

FILE:         HALclkRenovoPLL.c

DESCRIPTION:
  This file contains the clock HAL code for the Renovo PLL control.

==============================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/hal/hw/v1/HALclkRenovoPLL.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
10/27/14   vt     Created

==============================================================================
            Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkRenovoPLL.h"
#include "HALclkGenericPLL.h"
#include "DDIChipInfo.h"


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
  (HWIO_FMSK(PLL_MODE, PLL_OUTCTRL)  | \
   HWIO_FMSK(PLL_MODE, PLL_BYPASSNL) | \
   HWIO_FMSK(PLL_MODE, PLL_RESET_N))

/*
 * HAL_CLK_PLL_INIT_TIME_US
 *
 * Init time in microseconds after de-asserting PLL reset (normal mode).
 */
#define HAL_CLK_PLL_INIT_TIME_US                  5

/*
 * HAL_CLK_LOCK_TIMEOUT_US
 *
 * Timeout in US for PLL_LOCK_DET to get set.
 */
#define HAL_CLK_LOCK_TIMEOUT_US                 200

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_VALID_BMSK
 *
 * Bitmask used to read the "Is XO Valid" field.
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_VALID_BMSK     0x2

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_ATTEMPTS
 *
 * Max attempts to read XO type if it isn't valid.
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_ATTEMPTS         5

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_MASK
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_BMSK           0x3

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_19_P2
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_19_P2          0x0

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_38_P4
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_38_P4          0x2

/*
 * HAL_CLK_WCSS_PLL_XO_TYPE_48
 */
#define HAL_CLK_WCSS_PLL_XO_TYPE_48             0x3

/*
 * HAL_CLK_WSI_CMD_TIMEOUT
 */
#define HAL_CLK_WSI_CMD_TIMEOUT                 300

/*
 * HAL_CLK_UPDATED_CONFIG_CTL_VAL
 */
#define HAL_CLK_UPDATED_CONFIG_CTL_VAL    0x140209B0

/*
 * HAL_CLK_UPDATED_TEST_CTL_VAL
 */
#define HAL_CLK_UPDATED_TEST_CTL_VAL      0x08000000


/* ============================================================================
**    Data Declarations
** ==========================================================================*/

/*
 * HAL_clk_RenovoPLLControl
 *
 * Control function array for the Renovo PLLs.
 */
HAL_clk_SourceControlType HAL_clk_RenovoPLLControl =
{
  .Enable           = HAL_clk_RenovoPLLEnable,
  .Disable          = HAL_clk_RenovoPLLDisable,
  .IsEnabled        = HAL_clk_RenovoPLLIsEnabled,
  .Config           = HAL_clk_RenovoPLLConfig,
  .ConfigPLL        = HAL_clk_RenovoPLLConfigPLL,
  .DetectPLLConfig  = HAL_clk_RenovoPLLDetectPLLConfig,
  .EnableVote       = NULL,
  .DisableVote      = NULL,
  .IsVoteEnabled    = NULL,
  .IsPLLConfigEqual = HAL_clk_RenovoPLLIsPLLConfigEqual,
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


#if 0
/* ===========================================================================
**  HAL_clk_RenovoPLLWCSSDetectXO
**
**  The WPLL0 source requires a series of commands to be issued over the WSI
**  bus in order to determine what oscillator is sourcing that PLL.
**
**  Pre-condition:
**    The WSI channel needs to be selected and control be configured to SW
**    before any WSI commands can be sent in this sequence.
**
**  Return
**    HAL source enum.
**
** ======================================================================== */

static HAL_clk_RenovoPLLWCSSDetectXO
(
  void
)
{
  HAL_clk_SourceType eSource;
  uint32             nRegVal, nReadAttempts;
  boolean            bResult;

  /*
   * Set SW_BUS_SYNC command.
   */
  HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_BUS_SYNC, 1);

  /*
   * Wait for WSI2M_SYNCH_STATE to set during the timeout period.
   */
  bResult =
    HAL_clk_ValidateRegSetTimeout(
        HWIO_ADDR(WCSS_HM_A_PMM_WSIM_CTL),
        HWIO_FMSK(WCSS_HM_A_PMM_WSIM_CTL, WSI2M_SYNCH_STATE),
        HAL_CLK_WSI_CMD_TIMEOUT);

  /*
   * Return immediately if the timeout expired.
   */
  if (!bResult)
  {
    return HAL_CLK_SOURCE_NULL;
  }

  /*
   * Clear SW_BUS_SYNC command.
   */
  HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_BUS_SYNC, 0);

  /*
   * Write SYS_STAT address (0x402) to SW_REG_RD_DATA.
   */
  HWIO_OUTF(WCSS_HM_A_PMM_REG_RW_ADDR, SW_REG_RW_ADDR, 0x402);

  /*
   * Execute sequence to read the XO type but make sure to verify that the
   * field is valid.  Repeat the sequence until it is verified to be valid or
   * we've reached the maximum retry attempts.
   */
  for (nReadAttempts = 0;
       nReadAttempts < HAL_CLK_WCSS_PLL_XO_TYPE_ATTEMPTS;
       nReadAttempts++)
  {
    /*
     * Set SW_REG_READ command.
     */
    HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_REG_READ, 1);

    /*
     * Wait for RD_CMD_IP to clear during the timeout period.
     */
    bResult =
      HAL_clk_ValidateRegClearTimeout(
        HWIO_ADDR(WCSS_HM_A_PMM_WSI_CMD),
        HWIO_FMSK(WCSS_HM_A_PMM_WSI_CMD, RF_CMD_IP),
        HAL_CLK_WSI_CMD_TIMEOUT);

    /*
     * Return immediately if the timeout expired.
     */
    if (!bResult)
    {
      return HAL_CLK_SOURCE_NULL;
    }

    /*
     * Clear SW_REG_READ command.
     */
    HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_REG_READ, 0);

    /*
     * Read SW_REG_RD_DATA[1:0].
     */
    nRegVal = HWIO_INF(WCSS_HM_A_PMM_REG_READ_DATA, SW_REG_RD_DATA);

    /*
     * The XO type is only valid if nRegVal[2] is set.
     * Repeat the above reads until either this bit gets set or the timeout
     * expires and an error must be returned up the stack.
     */
    if (nRegVal & HAL_CLK_WCSS_PLL_XO_TYPE_VALID_BMSK)
    {
      break;
    }
  }

  /*
   * Verify read attempts didn't exceeded threshold.
   */
  if (nReadAttempts == HAL_CLK_WCSS_PLL_XO_TYPE_ATTEMPTS)
  {
    return HAL_CLK_SOURCE_NULL;
  }

  /*
   * Extract the XO type
   */
  eSource = HAL_CLK_SOURCE_NULL;
  nRegVal &= HAL_CLK_WCSS_PLL_XO_TYPE_BMSK;
  if (nRegVal == HAL_CLK_WCSS_PLL_XO_TYPE_19_P2)
  {
    eSource = HAL_CLK_SOURCE_RFXO;
  }
  else if (nRegVal == HAL_CLK_WCSS_PLL_XO_TYPE_38_P4)
  {
    eSource = HAL_CLK_SOURCE_RFXO_38_P4;
  }
  else if (nRegVal == HAL_CLK_WCSS_PLL_XO_TYPE_48)
  {
    eSource = HAL_CLK_SOURCE_RFXO_48;
  }

  return eSource;

} /* END HAL_clk_RenovoPLLWCSSDetectXO */


/* ===========================================================================
**  HAL_clk_RenovoPLLDetectXO
**
**  The WPLL0 source requires a series of commands to be issued over the WSI
**  bus in order to determine what oscillator is sourcing that PLL.
**
**  Pre-condition:
**    The WSI channel needs to be selected and control be configured to SW
**    before any WSI commands can be sent in this sequence.
**
**  Return
**    HAL source enum.
**
** ======================================================================== */

static HAL_clk_SourceType HAL_clk_RenovoPLLDetectXO
(
  HAL_clk_SourceDescType *pSource
)
{
  static HAL_clk_SourceType eSource = HAL_CLK_SOURCE_NULL;

  /*
   * WPLL0 is only existing Renovo source that requires special XO detection.
   */
  if (pSource->eSource != HAL_CLK_SOURCE_WPLL0)
  {
    return HAL_CLK_SOURCE_XO;
  }

  /*
   * Short-circuit if a valid XO type already detected from prior request.
   */
  if (eSource != HAL_CLK_SOURCE_NULL)
  {
    return eSource;
  }

  /*
   * Give WSI control to SW.
   */
  HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_USE_PMM_WSI, 1);

  /*
   * Select channel 0
   */
  HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, USE_WLAN1_WSI, 1);

  /*
   * Detect the XO type source the WCSS PLL.
   */
  eSource = HAL_clk_RenovoPLLWCSSDetectXO();

  /*
   * Change WSI control back to PHY.
   */
  HWIO_OUTF(WCSS_HM_A_PMM_WSI_CMD, SW_USE_PMM_WSI, 0);

  return eSource;

} /* END HAL_clk_RenovoPLLDetectXO */
#endif

/* ===========================================================================
**  HAL_clk_RenovoPLLEnable
**
**  Returns:
**    boolean - TRUE if successfully enabled. FALSE otherwise.
** ======================================================================== */

static boolean HAL_clk_RenovoPLLEnable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32                  nModeVal;
  boolean                 bResult;

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * De-assert PLL bypass.
   * This latches L, Alpha, and AlphaU values.
   */
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_BYPASSNL, 1, nModeVal);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Wait for PLL init time
   */
  HAL_clk_BusyWait(HAL_CLK_PLL_INIT_TIME_US);

  /*
   * De-assert PLL reset.
   */
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_RESET_N, 1, nModeVal);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Wait for the PLL to lock during the 50 us timeout period.
   */
  bResult =
    HAL_clk_ValidateRegSetTimeout(
      HAL_CLK_PLL_HWIO_ADDR(PLL_MODE, pSource),
      HWIO_FMSK(PLL_MODE, PLL_LOCK_DET),
      HAL_CLK_LOCK_TIMEOUT_US);

  /*
   * Return immediately if the lock failed.
   */
  if (!bResult)
  {
    return FALSE;
  }

  /*
   * Put the PLL in active mode
   */
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_OUTCTRL, 1, nModeVal);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  return TRUE;

} /* END HAL_clk_RenovoPLLEnable */


/* ===========================================================================
**  HAL_clk_RenovoPLLDisable
**
**  PLL shutdown.
**
** ======================================================================== */

static void HAL_clk_RenovoPLLDisable
(
  HAL_clk_SourceDescType        *pSource,
  HAL_clk_SourceDisableModeType  eMode,
  void                          *pData
)
{
  uint32                  nModeVal;

  /*
   * Read current mode value.
   */
  nModeVal = HAL_CLK_PLL_HWIO_IN(PLL_MODE, pSource);

  /*
   * De-assert the OUTCTL field.
   */
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_OUTCTRL, 0, nModeVal);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

  /*
   * Wait 2 ticks of the output main clock.
   */
  HAL_clk_BusyWait(1);

  /*
   * Put the PLL in reset mode
   */
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_BYPASSNL, 0, nModeVal);
  nModeVal = HAL_CLK_PLL_HWIO_OUTFV(PLL_MODE, pSource, PLL_RESET_N, 0, nModeVal);
  HAL_CLK_PLL_HWIO_OUT(PLL_MODE, pSource, nModeVal);

} /* END HAL_clk_RenovoPLLDisable */


/* ===========================================================================
**  HAL_clk_RenovoPLLIsEnabled
**
** ======================================================================== */

static boolean HAL_clk_RenovoPLLIsEnabled
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

} /* END HAL_clk_RenovoPLLIsEnabled */


/* ===========================================================================
**  HAL_clk_RenovoPLLConfig
**
** ======================================================================== */

static void HAL_clk_RenovoPLLConfig
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
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_VOTE_FSM_ENA, 1);
      break;

    case HAL_CLK_CONFIG_PLL_FSM_MODE_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_MODE, pSource, PLL_VOTE_FSM_ENA, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX, 1);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX2, 0);
      break;

    case HAL_CLK_CONFIG_PLL_AUX2_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_AUX2, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_EARLY, 1);
      break;

    case HAL_CLK_CONFIG_PLL_EARLY_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_EARLY, 0);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_ENABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_MAIN, 1);
      break;

    case HAL_CLK_CONFIG_PLL_MAIN_OUTPUT_DISABLE:
      HAL_CLK_PLL_HWIO_OUTF(PLL_USER_CTL, pSource, PLLOUT_LV_MAIN, 0);
      break;
    default:
      break;
  }

} /* END HAL_clk_RenovoPLLConfig */


/* ===========================================================================
**  HAL_clk_RenovoPLLConfigPLL
**
**  Returns:
**    boolean - TRUE if successfully configured. FALSE otherwise.
** ======================================================================== */

static boolean HAL_clk_RenovoPLLConfigPLL
(
  HAL_clk_SourceDescType      *pSource,
  const HAL_clk_PLLConfigType *pConfig,
  HAL_clk_SourceConfigMode     eMode
)
{
  uint32                  nRegVal, nFieldVal;
  boolean                 bEnabled;

  /*
   * Check PLL mode.
   */
  bEnabled = HAL_clk_RenovoPLLIsEnabled(pSource);

  /*
   * Disable the PLL.
   */
  if (bEnabled)
  {
    HAL_clk_RenovoPLLDisable(
      pSource,
      HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
      NULL);
  }

  /*
   * Program updated config/test/user register values as instructed by HW.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_CONFIG_CTL, pSource, HAL_CLK_UPDATED_CONFIG_CTL_VAL);
  HAL_CLK_PLL_HWIO_OUT(PLL_TEST_CTL, pSource, HAL_CLK_UPDATED_TEST_CTL_VAL);

  /*
   * Program L value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_L_VAL, pSource, pConfig->nL);

  /*
   * Program ALPHA value.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_ALPHA_VAL, pSource, pConfig->nFracVal);

  /*
   * Get PLL_USER_CTL register value.
   */
  nRegVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);

  /*
   * Compute the pre-div value (div-1,2 are supported).
   */
  if (pConfig->nPreDiv == 2)
  {
    nFieldVal = 1;
  }

  /*
   * Default to div-1.
   */
  else
  {
    nFieldVal = 0;
  }

  /*
   * Populate PDIV_CTL field in local variable.
   */
  nRegVal =
    HAL_CLK_PLL_HWIO_OUTFV(PLL_USER_CTL, pSource, PDIV_CTL, nFieldVal, nRegVal);

  /*
   * Compute the post-div value (div-1,2,4, and 8 are supported)
   */
  if (pConfig->PostDivs.Renovo.nMain == 2)
  {
    nFieldVal = 1;
  }
  else if (pConfig->PostDivs.Renovo.nMain == 4)
  {
    nFieldVal = 2;
  }
  else if (pConfig->PostDivs.Renovo.nMain == 8)
  {
    nFieldVal = 3;
  }

  /*
   * Default to div-1.
   */
  else
  {
    nFieldVal = 0;
  }

  /*
   * Populate POSTDIV_CTL field in local variable.
   */
  nRegVal =
    HAL_CLK_PLL_HWIO_OUTFV(PLL_USER_CTL, pSource, POSTDIV_CTL, nFieldVal, nRegVal);

  /*
   *  Check fractional output or integer output.
   */
  if (pConfig->nFracVal != 0)
  {
    nFieldVal = 1;
  }
  else
  {
    nFieldVal = 0;
  }

  /*
   * Populate ALPHA_EN field in local variable.
   */
  nRegVal =
    HAL_CLK_PLL_HWIO_OUTFV(PLL_USER_CTL, pSource, ALPHA_EN, nFieldVal, nRegVal);

  /*
   * Program the USER_CTL register.
   */
  HAL_CLK_PLL_HWIO_OUT(PLL_USER_CTL, pSource, nRegVal);

  /*
   * Enable the PLL if it was on before.
   */
  if (bEnabled)
  {
    HAL_clk_RenovoPLLEnable(
      pSource,
      HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
      NULL);
  }

  return TRUE;

} /* END HAL_clk_RenovoPLLConfigPLL */


/* ===========================================================================
**  HAL_clk_RenovoPLLDetectPLLConfig
**
** ======================================================================== */

static void HAL_clk_RenovoPLLDetectPLLConfig
(
  HAL_clk_SourceDescType *pSource,
  HAL_clk_PLLConfigType  *pConfig
)
{
  uint32                  nRegVal, nFieldVal;

  /*
   * Clear all config values not modified in this function.
   */
  pConfig->nFracVal = 0;
  pConfig->eFracMode = HAL_CLK_PLL_FRAC_MODE_ALPHA;

  /*
   * Read L value.
   */
  pConfig->nL = HAL_CLK_PLL_HWIO_IN(PLL_L_VAL, pSource);

  /*
   * Read ALPHA value.
   */
  pConfig->nFracVal = HAL_CLK_PLL_HWIO_IN(PLL_ALPHA_VAL, pSource);

  /*
   * No reference clock mux, so default to 0.
   */
  pConfig->nSource = 0;

  /*
   * Read USER_CTL register.
   */
  nRegVal = HAL_CLK_PLL_HWIO_IN(PLL_USER_CTL, pSource);

  /*
   * Extract the pre divider.
   */
  nFieldVal = HAL_CLK_PLL_HWIO_INFV(PLL_USER_CTL, pSource, PDIV_CTL, nRegVal);
  if (nFieldVal == 0)
  {
    pConfig->nPreDiv = 1;
  }
  else if (nFieldVal == 1)
  {
    pConfig->nPreDiv = 2;
  }

  /*
   * Extract the post divider.
   */
  nFieldVal = HAL_CLK_PLL_HWIO_INFV(PLL_USER_CTL, pSource, POSTDIV_CTL, nRegVal);
  if (nFieldVal == 0)
  {
    pConfig->PostDivs.Renovo.nMain = 1;
  }
  else if (nFieldVal == 1)
  {
    pConfig->PostDivs.Renovo.nMain = 2;
  }
  else if (nFieldVal == 2)
  {
    pConfig->PostDivs.Renovo.nMain = 4;
  }
  else if (nFieldVal == 3)
  {
    pConfig->PostDivs.Renovo.nMain = 8;
  }

} /* END HAL_clk_RenovoPLLDetectPLLConfig */


/* ===========================================================================
**  HAL_clk_RenovoPLLIsPLLConfigEqual
**
** ======================================================================== */

static boolean HAL_clk_RenovoPLLIsPLLConfigEqual
(
  const HAL_clk_PLLConfigType  *pConfigA,
  const HAL_clk_PLLConfigType  *pConfigB
)
{
  uint32 nMainDivA, nMainDivB;

  if (HAL_clk_GenericIsPLLConfigEqual(pConfigA, pConfigB))
  {
    /* Treat div-0 and div-1 as the same thing. */
    nMainDivA = MAX(1, pConfigA->PostDivs.Renovo.nMain);
    nMainDivB = MAX(1, pConfigB->PostDivs.Renovo.nMain);

    if (nMainDivA == nMainDivB)
    {
      return TRUE;
    }
  }

  return FALSE;

} /* END HAL_clk_RenovoPLLIsPLLConfigEqual */

