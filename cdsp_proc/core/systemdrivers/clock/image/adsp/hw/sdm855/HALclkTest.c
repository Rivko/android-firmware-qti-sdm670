/*
==============================================================================

FILE:         HALclkTest.c

DESCRIPTION:
  Auto-generated file containing custom debug mux control functions.

==============================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/adsp/hw/sdm855/HALclkTest.c#1 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "HALclkInternal.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"

/*=========================================================================
      Definitions
==========================================================================*/

#define HAL_CLK_FRQ_MEASURE_CNT_SHORT  0x0100
#define HAL_CLK_FRQ_MEASURE_CNT_LONG   0x8000


/*=========================================================================
      GCC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxGCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32                     nMuxSel
)
{
  HWIO_OUTF(GCC_DEBUG_CLK_CTL, MUX_SEL, nMuxSel);
  HWIO_OUTF(GCC_DEBUG_DIV_CDIVR, CLK_DIV, 3);
  HWIO_OUTF(GCC_DEBUG_CBCR, CLK_ENABLE, 1);
}

static uint32 HAL_clk_RunFreqCounterGCC
(
  uint32 nTermCount
)
{
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 0);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, XO_DIV4_TERM_CNT, nTermCount);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 1);
  HAL_clk_BusyWait(5);
  while (HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, XO_DIV4_CNT_DONE) == 0);
  HWIO_OUTF(GCC_CLOCK_FRQ_MEASURE_CTL, CNT_EN, 0);
  return HWIO_INF(GCC_CLOCK_FRQ_MEASURE_STATUS, MEASURE_CNT);
}

static void HAL_clk_CalcDebugMuxFreqGCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32                    *pnFreqHz
)
{
  uint32 nShortCount, nLongCount;
  uint32 nDenominator;

  /*
   * Enable counter reference clock.
   */
  HWIO_OUTF(GCC_XO_DIV4_CBCR, CLK_ENABLE, 1);

  /*
   * The counter only resets after a new clock edge comes in, so if the
   * clock under test is off there will be no edges and the counter
   * will retain the result from the previous measurement. The only way
   * to detect a disabled clock is to perform two measurements of
   * different length, since if the clock is on the counts will always be
   * different.
   */
  nShortCount = HAL_clk_RunFreqCounterGCC(HAL_CLK_FRQ_MEASURE_CNT_SHORT);
  nLongCount = HAL_clk_RunFreqCounterGCC(HAL_CLK_FRQ_MEASURE_CNT_LONG);
  if (nShortCount == nLongCount)
  {
    *pnFreqHz = 0;
  }
  else
  {
    /*
     * Calculate the frequency.  Function is provided by
     * Power Control 42.8.1.2 Measurement technique
     * f ring = f tcxo/4 * (Nring + 1.5) / (TCtcxo + 3.5)
     *
     * The formula below is the equivalent one converted
     * to integer arithmetic.
     *
     * We add half of the denominator value to the dividend
     * to guard against rounding errors inherent to
     * integer arithmetic.
     */
    nDenominator = HAL_CLK_FRQ_MEASURE_CNT_LONG * 2 + 7;
    *pnFreqHz = (uint32)(((9600 * (uint64)nLongCount + 14400) + nDenominator / 2)
                         / nDenominator * 1000);
  }
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlGCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxGCC,
  .CalcFreq   = HAL_clk_CalcDebugMuxFreqGCC,
};


/*=========================================================================
      LPASS_CC Debug Mux Control Functions
==========================================================================*/

static void HAL_clk_ConfigDebugMuxLPASSCC
(
  HAL_clk_DebugMuxDescType  *pmDebugMux,
  uint32                     nMuxSel
)
{
  HWIO_OUTF(LPASS_LPASS_CC_DEBUG_MUX_MUXR, MUX_SEL, nMuxSel);
  HWIO_OUTF(LPASS_LPASS_CC_DEBUG_DIV_CDIVR, CLK_DIV, 0);
  HWIO_OUTF(LPASS_LPASS_CC_DEBUG_CBCR, CLK_ENABLE, 1);
}

HAL_clk_DebugMuxControlType HAL_clk_DebugMuxControlLPASSCC =
{
  .ConfigMux  = HAL_clk_ConfigDebugMuxLPASSCC,
};

