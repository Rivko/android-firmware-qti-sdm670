/*
==============================================================================

FILE:         HALclkAPCS.c

DESCRIPTION:
  This file contains the clock HAL code to control the APCS clocks.


==============================================================================

                             Edit History


==============================================================================
   Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"
#include "HALclkAPCS.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/


/* ============================================================================
**    Data
** ==========================================================================*/


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  Function : HAL_clk_EnableMiscDependencies
**
** ======================================================================== */

void HAL_clk_EnableMiscDependencies()
{
  HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_RESET, 1);
  HAL_clk_BusyWait(1);
  HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_RESET, 0);
  HWIO_OUTF(GPUCC_GPU_CC_GX_DOMAIN_MISC, GPU_GX_GMEM_CLAMP_IO, 0);
} /* End HAL_clk_EnableMiscDependencies */


/* ===========================================================================
**  Function : HAL_clk_OSM_LUT_READ_FREQUENCY
**
** ======================================================================== */

boolean HAL_clk_OSM_LUT_READ_FREQUENCY
(
  HAL_clk_APCSCPUType  eCore,
  uint32               nRow,
  uint32              *nIndex,
  uint32              *nPllSource,
  uint32              *nPllDiv,
  uint32              *nDroopCode,
  uint32              *nCoreCount,
  uint32              *nLVal
)
{
  /* Sanity check */
  if (eCore      >= HAL_CLK_APCS_CPU_TOTAL ||
      nRow       >= OSM_LUT_MAX_ROWS       ||
      nIndex     == NULL                   ||
      nPllSource == NULL                   ||
      nPllDiv    == NULL                   ||
      nDroopCode == NULL                   ||
      nCoreCount == NULL                   ||
      nLVal      == NULL                   )
  {
    return FALSE;
  }

  /* Read program from the hardware. */
  switch(eCore)
  {
    case HAL_CLK_APCS_CPU_CLUSTER0_C0:
    case HAL_CLK_APCS_CPU_CLUSTER0_C1:
    case HAL_CLK_APCS_CPU_CLUSTER0_C2:
    case HAL_CLK_APCS_CPU_CLUSTER0_C3:
    case HAL_CLK_APCS_CPU_CLUSTER0_C4:
    case HAL_CLK_APCS_CPU_CLUSTER0_C5:
      *nIndex     = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, INDEX);
      *nPllSource = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_SRC);
      *nPllDiv    = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_DIV_VAL);
      *nDroopCode = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, DROOP_CODE);
      *nCoreCount = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, CORE_CNT_SUPPORTED);
      *nLVal      = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_FREQUENCY, nRow, LVAL);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C0:
    case HAL_CLK_APCS_CPU_CLUSTER1_C1:
      *nIndex     = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, INDEX);
      *nPllSource = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_SRC);
      *nPllDiv    = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_DIV_VAL);
      *nDroopCode = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, DROOP_CODE);
      *nCoreCount = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, CORE_CNT_SUPPORTED);
      *nLVal      = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, LVAL);
      break;

    case HAL_CLK_APCS_CPU_L3:
      *nIndex     = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, INDEX);
      *nPllSource = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_SRC);
      *nPllDiv    = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, PLL_DIV_VAL);
      *nDroopCode = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, DROOP_CODE);
      *nCoreCount = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, CORE_CNT_SUPPORTED);
      *nLVal      = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_FREQUENCY, nRow, LVAL);
      break;

    default:
      return FALSE;
  }

  return TRUE;

} /* END HAL_clk_OSM_LUT_READ_FREQUENCY */


/* ===========================================================================
**  Function : HAL_clk_OSM_LUT_READ_VOLTAGE
**
** ======================================================================== */

boolean HAL_clk_OSM_LUT_READ_VOLTAGE
(
  HAL_clk_APCSCPUType  eCore,
  uint32               nRow,
  uint32              *nMemAccLevel,
  uint32              *nVirtualCorner,
  uint32              *nOpenLoopVoltage
)
{
  /* Sanity check */
  if (eCore            >= HAL_CLK_APCS_CPU_TOTAL ||
      nRow             >= OSM_LUT_MAX_ROWS       ||
      nMemAccLevel     == NULL                   ||
      nVirtualCorner   == NULL                   ||
      nOpenLoopVoltage == NULL                   )
  {
    return FALSE;
  }

  /* Program the hardware. */
  switch(eCore)
  {
    case HAL_CLK_APCS_CPU_CLUSTER0_C0:
    case HAL_CLK_APCS_CPU_CLUSTER0_C1:
    case HAL_CLK_APCS_CPU_CLUSTER0_C2:
    case HAL_CLK_APCS_CPU_CLUSTER0_C3:
    case HAL_CLK_APCS_CPU_CLUSTER0_C4:
    case HAL_CLK_APCS_CPU_CLUSTER0_C5:
      *nMemAccLevel     = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_VOLTAGE, nRow, MEM_ACC_LEVEL);
      *nVirtualCorner   = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_VOLTAGE, nRow, VIRTUAL_CORNER);
      *nOpenLoopVoltage = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_VOLTAGE, nRow, OPEN_LOOP_VOLTAGE);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C0:
    case HAL_CLK_APCS_CPU_CLUSTER1_C1:
      *nMemAccLevel     = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, MEM_ACC_LEVEL);
      *nVirtualCorner   = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, VIRTUAL_CORNER);
      *nOpenLoopVoltage = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, OPEN_LOOP_VOLTAGE);
      break;

    case HAL_CLK_APCS_CPU_L3:
      *nMemAccLevel     = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, MEM_ACC_LEVEL);
      *nVirtualCorner   = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, VIRTUAL_CORNER);
      *nOpenLoopVoltage = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_VOLTAGE, nRow, OPEN_LOOP_VOLTAGE);
      break;

    default:
      return FALSE;
  }

  return TRUE;

} /* END HAL_clk_OSM_LUT_READ_VOLTAGE */


/* ===========================================================================
**  Function : HAL_clk_OSM_LUT_READ_PLL_OVERRIDES
**
** ======================================================================== */

boolean HAL_clk_OSM_LUT_READ_PLL_OVERRIDES
(
  HAL_clk_APCSCPUType  eCore,
  uint32               nRow,
  uint32              *nLVal,
  uint32              *nCcSafeFrequency,
  uint32              *nDcvsSafeFrequency
)
{
  /* Sanity check */
  if (eCore              >= HAL_CLK_APCS_CPU_TOTAL ||
      nRow               >= OSM_LUT_MAX_ROWS       ||
      nLVal              == NULL                   ||
      nCcSafeFrequency   == NULL                   ||
      nDcvsSafeFrequency == NULL                   )
  {
    return FALSE;
  }

  /* Program the hardware. */
  switch(eCore)
  {
    case HAL_CLK_APCS_CPU_CLUSTER0_C0:
    case HAL_CLK_APCS_CPU_CLUSTER0_C1:
    case HAL_CLK_APCS_CPU_CLUSTER0_C2:
    case HAL_CLK_APCS_CPU_CLUSTER0_C3:
    case HAL_CLK_APCS_CPU_CLUSTER0_C4:
    case HAL_CLK_APCS_CPU_CLUSTER0_C5:
      *nLVal              = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, ONE_BY_L_VAL);
      *nCcSafeFrequency   = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, CC_SAFE_L_VAL);
      *nDcvsSafeFrequency = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, DCVS_SAFE_L_VAL);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C0:
    case HAL_CLK_APCS_CPU_CLUSTER1_C1:
      *nLVal              = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, ONE_BY_L_VAL);
      *nCcSafeFrequency   = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, CC_SAFE_L_VAL);
      *nDcvsSafeFrequency = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, DCVS_SAFE_L_VAL);
      break;

    case HAL_CLK_APCS_CPU_L3:
      *nLVal              = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, ONE_BY_L_VAL);
      *nCcSafeFrequency   = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, CC_SAFE_L_VAL);
      *nDcvsSafeFrequency = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_PLL_OVERRIDES, nRow, DCVS_SAFE_L_VAL);
      break;

    default:
      return FALSE;
  }

  return TRUE;

} /* END HAL_clk_OSM_LUT_READ_PLL_OVERRIDES */


/* ===========================================================================
**  Function : HAL_clk_OSM_LUT_READ_SPARE_0
**
** ======================================================================== */

boolean HAL_clk_OSM_LUT_READ_SPARE_0
(
  HAL_clk_APCSCPUType  eCore,
  uint32               nRow,
  uint32              *nSpare
)
{
  /* Sanity check */
  if (eCore  >= HAL_CLK_APCS_CPU_TOTAL ||
      nRow   >= OSM_LUT_MAX_ROWS       ||
      nSpare == NULL                   )
  {
    return FALSE;
  }

  /* Program the hardware. */
  switch(eCore)
  {
    case HAL_CLK_APCS_CPU_CLUSTER0_C0:
    case HAL_CLK_APCS_CPU_CLUSTER0_C1:
    case HAL_CLK_APCS_CPU_CLUSTER0_C2:
    case HAL_CLK_APCS_CPU_CLUSTER0_C3:
    case HAL_CLK_APCS_CPU_CLUSTER0_C4:
    case HAL_CLK_APCS_CPU_CLUSTER0_C5:
      *nSpare = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM1_DCVS_LUT_ENTRYr_SPARE_0, nRow, SPARE_0);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C0:
    case HAL_CLK_APCS_CPU_CLUSTER1_C1:
      *nSpare = HWIO_INFI(APSS_OSM_RAIL1_CLKDOM0_DCVS_LUT_ENTRYr_SPARE_0, nRow, SPARE_0);
      break;

    case HAL_CLK_APCS_CPU_L3:
      *nSpare = HWIO_INFI(APSS_OSM_RAIL0_CLKDOM0_DCVS_LUT_ENTRYr_SPARE_0, nRow, SPARE_0);
      break;

    default:
      return FALSE;
  }

  return TRUE;

} /* END HAL_clk_OSM_LUT_READ_SPARE_0 */


/* ===========================================================================
**  Function : HAL_clk_IS_OSM_ENABLED
**
** ======================================================================== */

boolean HAL_clk_IS_OSM_ENABLED()
{
  if (HWIO_INF(APSS_OSM_RAIL0_CLKDOM1_OSM_ENABLE, ENABLE_OSM) ||
      HWIO_INF(APSS_OSM_RAIL1_CLKDOM0_OSM_ENABLE, ENABLE_OSM) ||
      HWIO_INF(APSS_OSM_RAIL0_CLKDOM0_OSM_ENABLE, ENABLE_OSM) )
  {
    return TRUE;
  }

  return FALSE;

} /* END HAL_clk_IS_OSM_ENABLED */


/* ===========================================================================
**  Function : HAL_clk_OSM_SET_DESIRED_PERF_LEVEL
**
** ======================================================================== */

boolean HAL_clk_OSM_SET_DESIRED_PERF_LEVEL
(
  HAL_clk_APCSCPUType eCore,
  uint32              nIndex
)
{
  /* Sanity check */
  if (eCore  >= HAL_CLK_APCS_CPU_TOTAL ||
      nIndex >= OSM_LUT_MAX_ROWS       )
  {
    return FALSE;
  }

  switch(eCore)
  {
    case HAL_CLK_APCS_CPU_CLUSTER0_C0:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 0, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER0_C1:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 1, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER0_C2:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 2, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER0_C3:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 3, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER0_C4:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 4, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER0_C5:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM1_COREd_PERF_STATE_DESIRED, 5, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C0:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL1_CLKDOM0_COREd_PERF_STATE_DESIRED, 0, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_CLUSTER1_C1:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL1_CLKDOM0_COREd_PERF_STATE_DESIRED, 1, OS_REQUEST, nIndex);
      break;

    case HAL_CLK_APCS_CPU_L3:
      /* Program the hardware. */
      HWIO_OUTFI(APSS_OSM_RAIL0_CLKDOM0_COREd_PERF_STATE_DESIRED, 0, OS_REQUEST, nIndex);
      break;

    default:
      return FALSE;
  }

  return TRUE;

} /* END HAL_clk_OSM_SET_DESIRED_PERF_LEVEL */

