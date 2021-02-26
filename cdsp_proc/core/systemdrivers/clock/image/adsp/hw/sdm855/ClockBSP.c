/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains the clock driver BSP data.

  Auto-generated from the following IPCat controller releases:

    SDM855v1 (hana_1.0):
      GCC       : z_hana_clk_ctl_p3q1r12 [hana_gcc.r1]
      LPASS_CC  : hana_lpass_p2q1r4.5 [hana_lpass_cc.r1]


==============================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/adsp/hw/sdm855/ClockBSP.c#1 $

==============================================================================
            Copyright (c) 2017 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"
#include "HALclkHWIO.h"
#include "HALhwio.h"


/*=========================================================================
      Externs
==========================================================================*/

extern HAL_clk_ClockControlType           HAL_clk_GenericClockControl;
extern HAL_clk_ClockDomainControlType     HAL_clk_GenericDomainControl;
extern HAL_clk_DividerControlType         HAL_clk_GenericDividerControl;
extern HAL_clk_PowerDomainControlType     HAL_clk_GenericPowerDomainControl;
extern HAL_clk_SourceControlType          HAL_clk_AgeraPLLControl;
extern HAL_clk_SourceControlType          HAL_clk_FabiaPLLControl;
extern HAL_clk_SourceControlType          HAL_clk_TrionPLLControl;

/*
 * Debug mux control structures.
 */
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlGCC;
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlLPASSCC;


/*=========================================================================
      Data Declarations
==========================================================================*/


/*=========================================================================
      Voltage Rails
==========================================================================*/

static ClockRailType ClockRail_VDDCX =
{
  .szName = "/vcs/vdd_cx",
};

static ClockRailType *aRails[] =
{
  &ClockRail_VDDCX,
};

/*=========================================================================
      Debug Muxes
==========================================================================*/

static ClockDebugMuxType ClockDebugMux_GCC =
{
  .szName       = "gcc",
  .nDivider     = 4,
  .HALDebugMux  = {
    .pmControl  = &HAL_clk_DebugMuxControlGCC,
  },
};

static ClockDebugMuxType ClockDebugMux_LPASSCC =
{
  .szName       = "lpass_cc",
  .pParent      = &ClockDebugMux_GCC,
  .nParentSel   = 215,
  .HALDebugMux  = {
    .pmControl  = &HAL_clk_DebugMuxControlLPASSCC,
  },
};

static ClockDebugMuxType *aDebugMuxes[] =
{
  &ClockDebugMux_GCC,
  &ClockDebugMux_LPASSCC,
};


/*=========================================================================
      Primary Sources
==========================================================================*/

static ClockSourceNodeType ClockSource_XO =
{
  .szName    = "/pmic/client/xo",
  .nFlags    = 0x0,
};


/*=========================================================================
      Source BSP
==========================================================================*/

/*
 * ClockSourceBSP_GCC_GPLL0
 *
 * Used by:
 */
static const ClockSourceFreqConfigType ClockSourceBSP_GCC_GPLL0[] =
{
  {  600000000, &ClockSource_XO, { 0, 0, 1, 31,  0x00004000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {.Trion.nEven = 2}             }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};

/*
 * ClockSourceBSP_GCC_GPLL1
 *
 * Used by:
 */
static const ClockSourceFreqConfigType ClockSourceBSP_GCC_GPLL1[] =
{
  { 1066000000, &ClockSource_XO, { 0, 0, 1, 55,  0x00008555, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },
};

/*
 * ClockSourceBSP_LPASSCC_LPASSLPAAUDIODIGPLL
 *
 * Used by:
 *   ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSCC_LPASSLPAAUDIODIGPLL[] =
{
  {  614400000, &ClockSource_XO, { 0, 0, 1, 32,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {.Fabia.nOdd = 5}              }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};

/*
 * ClockSourceBSP_LPASSCC_LPASSLPAAUDIOPLL
 *
 * Used by:
 *   ClockSource_LPASSCC_LPASSLPAAUDIOPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSCC_LPASSLPAAUDIOPLL[] =
{
  { 1128960000, &ClockSource_XO, { 0, 0, 1, 58,  0x00000000, HAL_CLK_PLL_FRAC_MODE_MN, {.Agera.nMain = 2}             }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};

/*
 * ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL
 *
 * Used by:
 *   ClockSource_LPASSCC_LPASSQDSP6SSPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[] =
{
  {  364800000, &ClockSource_XO, { 0, 0, 1, 19,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
  {  576000000, &ClockSource_XO, { 0, 0, 1, 30,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
  {  768000000, &ClockSource_XO, { 0, 0, 1, 40,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
  {  960000000, &ClockSource_XO, { 0, 0, 1, 50,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
  { 1171200000, &ClockSource_XO, { 0, 0, 1, 61,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },
  { 1324800000, &ClockSource_XO, { 0, 0, 1, 69,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },
  { 1497600000, &ClockSource_XO, { 0, 0, 1, 78,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },
};
static const ClockSourceFreqConfigType ClockSourceBSPCal_LPASSCC_LPASSQDSP6SSPLL[] =
{
  {  921600000, &ClockSource_XO, { 0, 0, 1, 48,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
};


/*=========================================================================
      Sources
==========================================================================*/

static ClockSourceNodeType ClockSource_GCC_GPLL0 =
{
  .szName       = "gpll0",
  .nFlags       = CLOCK_FLAG_READ_ONLY |
                  CLOCK_FLAG_STUB_HW_RUMI,
  .aBSP         = ClockSourceBSP_GCC_GPLL0,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_GCC_GPLL0),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE |
                  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(GCC_GPLL0_MODE),
    .VoterRegister  = {HWIO_ADDR(GCC_PROC_CLK_PLL_ENA_VOTE), 0x1},
    .pmControl      = &HAL_clk_TrionPLLControl,
  },
};

static ClockSourceNodeType ClockSource_GCC_GPLL1 =
{
  .szName       = "gpll1",
  .nFlags       = CLOCK_FLAG_READ_ONLY,
  .aBSP         = ClockSourceBSP_GCC_GPLL1,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_GCC_GPLL1),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(GCC_GPLL1_MODE),
    .VoterRegister  = {HWIO_ADDR(GCC_PROC_CLK_PLL_ENA_VOTE), 0x2},
    .pmControl      = &HAL_clk_TrionPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL =
{
  .szName       = "lpass_lpaaudio_dig_pll",
  .nFlags       = 0x0,
  .aBSP         = ClockSourceBSP_LPASSCC_LPASSLPAAUDIODIGPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSCC_LPASSLPAAUDIODIGPLL),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(LPASS_LPAAUDIO_DIG_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSCC_LPASSLPAAUDIOPLL =
{
  .szName       = "lpass_lpaaudio_pll",
  .nFlags       = 0x0,
  .aBSP         = ClockSourceBSP_LPASSCC_LPASSLPAAUDIOPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSCC_LPASSLPAAUDIOPLL),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(LPASS_LPAAUDIO_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_AgeraPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSCC_LPASSQDSP6SSPLL =
{
  .szName       = "lpass_qdsp6ss_pll",
  .nFlags       = CLOCK_FLAG_SUPPORTS_SLEWING |
                  CLOCK_FLAG_SUPPRESSIBLE,
  .aBSP         = ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = 0x0,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(LPASS_QDSP6SS_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
  .pCalibrationFreqConfig     = ClockSourceBSPCal_LPASSCC_LPASSQDSP6SSPLL,
  .nCalibrationFreqConfigLen  = SIZEOF_ARRAY(ClockSourceBSPCal_LPASSCC_LPASSQDSP6SSPLL),
};


static ClockSourceNodeType *aSources[] =
{
  &ClockSource_GCC_GPLL0,
  &ClockSource_GCC_GPLL1,
  &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL,
  &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,
  &ClockSource_LPASSCC_LPASSQDSP6SSPLL,
  &ClockSource_XO,
};


/*=========================================================================
      Dividers
==========================================================================*/

static ClockDividerNodeType aDividers[] =
{
  {
    .szName    = "gcc_qupv3_wrap0_core_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap1_core_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap2_core_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(GCC_QUPV3_WRAP2_CORE_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "lpass_lpaaudio_dig_pll_out_main_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = 0,
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "lpass_lpaaudio_pll_out_aux2_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = 0,
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "lpass_lpaaudio_pll_out_aux_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = 0,
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "lpass_lpaaudio_pll_out_main_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = 0,
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
};

/*=========================================================================
      Domain BSP
==========================================================================*/

/*
 * ClockDomainBSP_GCC_GCCQUPV3WRAP0S0
 *
 * Note: These frequencies are used by domains possibly under DFS control. The
 *       DFS-enable state will be detected during init. If under DFS control,
 *       the domain will dynamically allocate its own BSP array and populate
 *       it with configurations detected from the DFS registers.
 *
 * Used by:
 *   ClockDomain_GCC_GCCQUPV3WRAP0S0
 *   ClockDomain_GCC_GCCQUPV3WRAP0S1
 *   ClockDomain_GCC_GCCQUPV3WRAP0S2
 *   ClockDomain_GCC_GCCQUPV3WRAP0S3
 *   ClockDomain_GCC_GCCQUPV3WRAP0S4
 *   ClockDomain_GCC_GCCQUPV3WRAP0S5
 *   ClockDomain_GCC_GCCQUPV3WRAP0S6
 *   ClockDomain_GCC_GCCQUPV3WRAP0S7
 *   ClockDomain_GCC_GCCQUPV3WRAP1S0
 *   ClockDomain_GCC_GCCQUPV3WRAP1S1
 *   ClockDomain_GCC_GCCQUPV3WRAP1S2
 *   ClockDomain_GCC_GCCQUPV3WRAP1S3
 *   ClockDomain_GCC_GCCQUPV3WRAP1S4
 *   ClockDomain_GCC_GCCQUPV3WRAP1S5
 *   ClockDomain_GCC_GCCQUPV3WRAP2S0
 *   ClockDomain_GCC_GCCQUPV3WRAP2S1
 *   ClockDomain_GCC_GCCQUPV3WRAP2S2
 *   ClockDomain_GCC_GCCQUPV3WRAP2S3
 *   ClockDomain_GCC_GCCQUPV3WRAP2S4
 *   ClockDomain_GCC_GCCQUPV3WRAP2S5
 */
static ClockMuxConfigType ClockDomainBSP_GCC_GCCQUPV3WRAP0S0[] =
{
  {    7372800, &ClockSource_GCC_GPLL0,                   { 6,     2,     384,   15625, 0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   14745600, &ClockSource_GCC_GPLL0,                   { 6,     2,     768,   15625, 0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0x00 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = BI_TCXO */
  {   29491200, &ClockSource_GCC_GPLL0,                   { 6,     2,     1536,  15625, 0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   32000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     8,     75,    0x01 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   48000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     4,     25,    0x02 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   64000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     16,    75,    0x03 }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   80000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     4,     15,    0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {   96000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     8,     25,    0x04 }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  100000000, &ClockSource_GCC_GPLL0,                   { 6,     6,     0,     0,     0x05 }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  102400000, &ClockSource_GCC_GPLL0,                   { 6,     2,     128,   375,   0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  112000000, &ClockSource_GCC_GPLL0,                   { 6,     2,     28,    75,    0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  117964800, &ClockSource_GCC_GPLL0,                   { 6,     2,     6144,  15625, 0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  120000000, &ClockSource_GCC_GPLL0,                   { 6,     5,     0,     0,     0x06 }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  128000000, &ClockSource_GCC_GPLL0,                   { 1,     2,     16,    75,    0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = GPLL0_OUT_MAIN */
};

/*
 * ClockDomainBSP_GCC_GCCQUPV3WRAP1CORE2X
 *
 * Note: These frequencies are used by domains possibly under DFS control. The
 *       DFS-enable state will be detected during init. If under DFS control,
 *       the domain will dynamically allocate its own BSP array and populate
 *       it with configurations detected from the DFS registers.
 *
 * Used by:
 *   ClockDomain_GCC_GCCQUPV3WRAP1CORE2X
 */
static ClockMuxConfigType ClockDomainBSP_GCC_GCCQUPV3WRAP1CORE2X[] =
{
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0x00 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = BI_TCXO */
  {   50000000, &ClockSource_GCC_GPLL0,                   { 6,     12,    0,     0,     0x02 }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[6] = GPLL0_OUT_EVEN */
  {  100000000, &ClockSource_GCC_GPLL0,                   { 1,     12,    0,     0,     0x03 }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[1] = GPLL0_OUT_MAIN */
  {  150000000, &ClockSource_GCC_GPLL0,                   { 1,     8,     0,     0,     0x04 }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },                                                            /* Mux[1] = GPLL0_OUT_MAIN */
  {  200000000, &ClockSource_GCC_GPLL0,                   { 1,     6,     0,     0,     0x05 }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = GPLL0_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_AON
 *
 * Used by:
 *   ClockDomain_LPASSCC_AON
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_AON[] =
{
  {   25600000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     24,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   51200000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     12,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  102400000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  204800000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 1,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_ATIME
 *
 * Used by:
 *   ClockDomain_LPASSCC_ATIME
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_ATIME[] =
{
  {   15360000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[0] = XO */
  {   30720000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   61440000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {  122880000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_AUDSLIMBUS
 *
 * Used by:
 *   ClockDomain_LPASSCC_AUDSLIMBUS
 *   ClockDomain_LPASSCC_QCASLIMBUS
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_AUDSLIMBUS[] =
{
  {    6144000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   12288000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_CORE
 *
 * Used by:
 *   ClockDomain_LPASSCC_CORE
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_CORE[] =
{
  {   19200000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     32,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   38400000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   76800000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  153600000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 1,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_EXTMCLK0
 *
 * Used by:
 *   ClockDomain_LPASSCC_EXTMCLK0
 *   ClockDomain_LPASSCC_EXTMCLK1
 *   ClockDomain_LPASSCC_EXTMCLK2
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_EXTMCLK0[] =
{
  {     352800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                          { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_HWAF
 *
 * Used by:
 *   ClockDomain_LPASSCC_HWAF
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_HWAF[] =
{
  {   76800000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  153600000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  307200000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  614400000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 1,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_LPAIFHSIF0
 *
 * Used by:
 *   ClockDomain_LPASSCC_LPAIFHSIF0
 *   ClockDomain_LPASSCC_LPAIFHSIF1
 *   ClockDomain_LPASSCC_LPAIFHSIF2
 *   ClockDomain_LPASSCC_LPAIFQUI
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_LPAIFHSIF0[] =
{
  {    3072000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    6144000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   12288000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_LPAIFPCMOE
 *
 * Used by:
 *   ClockDomain_LPASSCC_LPAIFPCMOE
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_LPAIFPCMOE[] =
{
  {     352800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                          { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   15360000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   30720000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   61440000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {  122880000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_LPAIFPRI
 *
 * Used by:
 *   ClockDomain_LPASSCC_LPAIFPRI
 *   ClockDomain_LPASSCC_LPAIFQUAD
 *   ClockDomain_LPASSCC_LPAIFSEC
 *   ClockDomain_LPASSCC_LPAIFTER
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_LPAIFPRI[] =
{
  {     352800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                          { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX
 *
 * Used by:
 *   ClockDomain_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX
 *   ClockDomain_LPASSCC_LPASSLPAAUDIOPLLREFCLKMUX
 *   ClockDomain_LPASSCC_LPASSQDSP6SSPLLREFCLKMUX
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX[] =
{
  {          0, &ClockSource_XO,                          { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
};

/*
 * ClockDomainBSP_LPASSCC_LPASSQDSP6SSCORE
 *
 * Used by:
 *   ClockDomain_LPASSCC_LPASSQDSP6SSCORE
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_LPASSQDSP6SSCORE[] =
{
  {  364800000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 0] /* 364.0  */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  {  576000000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 1] /* 576.0  */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  {  768000000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 2] /* 768.0  */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  {  960000000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS_L1,     {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 3] /* 960.0  */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  { 1171200000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 4] /* 1171.0 */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  { 1324800000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM_L1,     {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 5] /* 1324.0 */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
  { 1497600000, &ClockSource_LPASSCC_LPASSQDSP6SSPLL,     { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_TUR,        {{ 0 }}, &ClockSourceBSP_LPASSCC_LPASSQDSP6SSPLL[ 6] /* 1497.0 */ },  /* Mux[2] = LPASS_QDSP6SS_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_QOSFIXEDLATCOUNTER
 *
 * Used by:
 *   ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_QOSFIXEDLATCOUNTER[] =
{
  {   61440000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {  122880000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 5,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCC_RESAMPLER
 *
 * Used by:
 *   ClockDomain_LPASSCC_RESAMPLER
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_RESAMPLER[] =
{
  {   38400000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   76800000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  153600000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 2,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  307200000, &ClockSource_LPASSCC_LPASSLPAAUDIODIGPLL, { 1,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCC_SLEEP
 *
 * Used by:
 *   ClockDomain_LPASSCC_SLEEP
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_SLEEP[] =
{
  {      32000, NULL,                                     { 1,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[1] = SLEEP_CLK */
};

/*
 * ClockDomainBSP_LPASSCC_XO
 *
 * Used by:
 *   ClockDomain_LPASSCC_XO
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCC_XO[] =
{
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
};


/*=========================================================================
      Domain Source Maps
==========================================================================*/

/*
 * ClockSourceMap_GCC_GCCQUPV3WRAP0S0
 *
 * Used by:
 *   ClockDomain_GCC_GCCQUPV3WRAP0S0
 *   ClockDomain_GCC_GCCQUPV3WRAP0S1
 *   ClockDomain_GCC_GCCQUPV3WRAP0S2
 *   ClockDomain_GCC_GCCQUPV3WRAP0S3
 *   ClockDomain_GCC_GCCQUPV3WRAP0S4
 *   ClockDomain_GCC_GCCQUPV3WRAP0S5
 *   ClockDomain_GCC_GCCQUPV3WRAP0S6
 *   ClockDomain_GCC_GCCQUPV3WRAP0S7
 *   ClockDomain_GCC_GCCQUPV3WRAP1S0
 *   ClockDomain_GCC_GCCQUPV3WRAP1S1
 *   ClockDomain_GCC_GCCQUPV3WRAP1S2
 *   ClockDomain_GCC_GCCQUPV3WRAP1S3
 *   ClockDomain_GCC_GCCQUPV3WRAP1S4
 *   ClockDomain_GCC_GCCQUPV3WRAP1S5
 *   ClockDomain_GCC_GCCQUPV3WRAP2S0
 *   ClockDomain_GCC_GCCQUPV3WRAP2S1
 *   ClockDomain_GCC_GCCQUPV3WRAP2S2
 *   ClockDomain_GCC_GCCQUPV3WRAP2S3
 *   ClockDomain_GCC_GCCQUPV3WRAP2S4
 *   ClockDomain_GCC_GCCQUPV3WRAP2S5
 */
static const ClockSourceMapType ClockSourceMap_GCC_GCCQUPV3WRAP0S0[] =
{

  { 0,   19200000, &ClockSource_XO },
  { 1,  600000000, &ClockSource_GCC_GPLL0 },
  { 6,  300000000, &ClockSource_GCC_GPLL0 },
  { 7,  400000000, NULL },
  { CLOCK_SOURCE_MAP_END }
};


/*=========================================================================
      Domain FMAX
==========================================================================*/

/*
 * ClockDomainFMax_GCC_GCCQUPV3WRAP0S0
 *
 * Used by:
 *   ClockDomain_GCC_GCCQUPV3WRAP0S0
 *   ClockDomain_GCC_GCCQUPV3WRAP0S1
 *   ClockDomain_GCC_GCCQUPV3WRAP0S2
 *   ClockDomain_GCC_GCCQUPV3WRAP0S3
 *   ClockDomain_GCC_GCCQUPV3WRAP0S4
 *   ClockDomain_GCC_GCCQUPV3WRAP0S5
 *   ClockDomain_GCC_GCCQUPV3WRAP0S6
 *   ClockDomain_GCC_GCCQUPV3WRAP0S7
 *   ClockDomain_GCC_GCCQUPV3WRAP1S0
 *   ClockDomain_GCC_GCCQUPV3WRAP1S1
 *   ClockDomain_GCC_GCCQUPV3WRAP1S2
 *   ClockDomain_GCC_GCCQUPV3WRAP1S3
 *   ClockDomain_GCC_GCCQUPV3WRAP1S4
 *   ClockDomain_GCC_GCCQUPV3WRAP1S5
 *   ClockDomain_GCC_GCCQUPV3WRAP2S0
 *   ClockDomain_GCC_GCCQUPV3WRAP2S1
 *   ClockDomain_GCC_GCCQUPV3WRAP2S2
 *   ClockDomain_GCC_GCCQUPV3WRAP2S3
 *   ClockDomain_GCC_GCCQUPV3WRAP2S4
 *   ClockDomain_GCC_GCCQUPV3WRAP2S5
 */
static const ClockFMaxType ClockDomainFMax_GCC_GCCQUPV3WRAP0S0[] =
{
  {   50000000, RAIL_VOLTAGE_LEVEL_MIN_SVS },
  {   75000000, RAIL_VOLTAGE_LEVEL_LOW_SVS },
  {  100000000, RAIL_VOLTAGE_LEVEL_SVS },
  {  128000000, RAIL_VOLTAGE_LEVEL_NOM },
  {  0  }
};

/*
 * ClockDomainFMax_LPASSCC_CLOCKDRIVERGENEBIT
 *
 * Used by:
 *   ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT
 */
static const ClockFMaxType ClockDomainFMax_LPASSCC_CLOCKDRIVERGENEBIT[] =
{
  {    3072000, RAIL_VOLTAGE_LEVEL_MIN_SVS },
  {    6144000, RAIL_VOLTAGE_LEVEL_LOW_SVS },
  {   12288000, RAIL_VOLTAGE_LEVEL_SVS },
  {   24576000, RAIL_VOLTAGE_LEVEL_NOM },
  {  0  }
};


/*=========================================================================
      Domains
==========================================================================*/

static ClockDomainNodeType ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE =
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

static ClockDomainNodeType ClockDomain_GCC_GCCAGGRENOC =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_AGGRE_NOC_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCAGGRENOCWESTSF =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_AGGRE_NOC_WEST_SF_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCCE1 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_CE1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCCONFIGNOC =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQDSSATBA =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QDSS_ATB_A_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0CORE2X =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S0 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S1 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S2 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S3 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S3_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S4 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S4_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S5 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S5_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S6 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S6_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP0S7 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP0_S7_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1CORE2X =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP1CORE2X,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP1CORE2X),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_2X_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S0 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S1 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S2 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S3 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S3_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S4 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S4_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S5 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S5_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2CORE2X =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_CORE_2X_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S0 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S1 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S2 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S3 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S3_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S4 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S4_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP2S5 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP2_S5_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCSYSNOC =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_AON =
{
  .aBSP      = ClockDomainBSP_LPASSCC_AON,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_AON),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_AON_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_ATIME =
{
  .aBSP      = ClockDomainBSP_LPASSCC_ATIME,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_ATIME),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_ATIME_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_AUDSLIMBUS =
{
  .aBSP      = ClockDomainBSP_LPASSCC_AUDSLIMBUS,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_AUDSLIMBUS),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_AUD_SLIMBUS_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_LPASSCC_CLOCKDRIVERGENEBIT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_CORE =
{
  .aBSP      = ClockDomainBSP_LPASSCC_CORE,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_CORE),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_CORE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_EXTMCLK0 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_EXTMCLK0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_EXTMCLK1 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_EXTMCLK0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_EXTMCLK2 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_EXTMCLK0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_HWAF =
{
  .aBSP      = ClockDomainBSP_LPASSCC_HWAF,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_HWAF),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_HW_AF_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFHSIF0 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFHSIF0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFHSIF0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_HS_IF_0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFHSIF1 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFHSIF0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFHSIF0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_HS_IF_1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFHSIF2 =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFHSIF0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFHSIF0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_HS_IF_2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFPCMOE =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFPCMOE,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFPCMOE),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_PCMOE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFPRI =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFPRI,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFPRI),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_PRI_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFQUAD =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFPRI,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFPRI),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_QUAD_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFQUI =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFHSIF0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFHSIF0),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_QUI_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFSEC =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFPRI,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFPRI),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_SEC_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPAIFTER =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPAIFPRI,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPAIFPRI),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_TER_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPASSLPAAUDIOPLLREFCLKMUX =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPASSQDSP6SSCORE =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPASSQDSP6SSCORE,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPASSQDSP6SSCORE),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_QDSP6SS_CORE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_LPASSQDSP6SSPLLREFCLKMUX =
{
  .aBSP      = ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_QCASLIMBUS =
{
  .aBSP      = ClockDomainBSP_LPASSCC_AUDSLIMBUS,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_AUDSLIMBUS),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_QCA_SLIMBUS_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER =
{
  .aBSP      = ClockDomainBSP_LPASSCC_QOSFIXEDLATCOUNTER,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_QOSFIXEDLATCOUNTER),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_QOS_FIXED_LAT_COUNTER_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_RESAMPLER =
{
  .aBSP      = ClockDomainBSP_LPASSCC_RESAMPLER,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_RESAMPLER),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_RESAMPLER_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_SLEEP =
{
  .aBSP      = ClockDomainBSP_LPASSCC_SLEEP,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_SLEEP),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_SLEEP_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCC_XO =
{
  .aBSP      = ClockDomainBSP_LPASSCC_XO,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCC_XO),
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_XO_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};


static ClockDomainNodeType *aDomains[] =
{
  &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
  &ClockDomain_GCC_GCCAGGRENOC,
  &ClockDomain_GCC_GCCAGGRENOCWESTSF,
  &ClockDomain_GCC_GCCCE1,
  &ClockDomain_GCC_GCCCONFIGNOC,
  &ClockDomain_GCC_GCCQDSSATBA,
  &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
  &ClockDomain_GCC_GCCQUPV3WRAP0S0,
  &ClockDomain_GCC_GCCQUPV3WRAP0S1,
  &ClockDomain_GCC_GCCQUPV3WRAP0S2,
  &ClockDomain_GCC_GCCQUPV3WRAP0S3,
  &ClockDomain_GCC_GCCQUPV3WRAP0S4,
  &ClockDomain_GCC_GCCQUPV3WRAP0S5,
  &ClockDomain_GCC_GCCQUPV3WRAP0S6,
  &ClockDomain_GCC_GCCQUPV3WRAP0S7,
  &ClockDomain_GCC_GCCQUPV3WRAP1CORE2X,
  &ClockDomain_GCC_GCCQUPV3WRAP1S0,
  &ClockDomain_GCC_GCCQUPV3WRAP1S1,
  &ClockDomain_GCC_GCCQUPV3WRAP1S2,
  &ClockDomain_GCC_GCCQUPV3WRAP1S3,
  &ClockDomain_GCC_GCCQUPV3WRAP1S4,
  &ClockDomain_GCC_GCCQUPV3WRAP1S5,
  &ClockDomain_GCC_GCCQUPV3WRAP2CORE2X,
  &ClockDomain_GCC_GCCQUPV3WRAP2S0,
  &ClockDomain_GCC_GCCQUPV3WRAP2S1,
  &ClockDomain_GCC_GCCQUPV3WRAP2S2,
  &ClockDomain_GCC_GCCQUPV3WRAP2S3,
  &ClockDomain_GCC_GCCQUPV3WRAP2S4,
  &ClockDomain_GCC_GCCQUPV3WRAP2S5,
  &ClockDomain_GCC_GCCSYSNOC,
  &ClockDomain_LPASSCC_AON,
  &ClockDomain_LPASSCC_ATIME,
  &ClockDomain_LPASSCC_AUDSLIMBUS,
  &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
  &ClockDomain_LPASSCC_CORE,
  &ClockDomain_LPASSCC_EXTMCLK0,
  &ClockDomain_LPASSCC_EXTMCLK1,
  &ClockDomain_LPASSCC_EXTMCLK2,
  &ClockDomain_LPASSCC_HWAF,
  &ClockDomain_LPASSCC_LPAIFHSIF0,
  &ClockDomain_LPASSCC_LPAIFHSIF1,
  &ClockDomain_LPASSCC_LPAIFHSIF2,
  &ClockDomain_LPASSCC_LPAIFPCMOE,
  &ClockDomain_LPASSCC_LPAIFPRI,
  &ClockDomain_LPASSCC_LPAIFQUAD,
  &ClockDomain_LPASSCC_LPAIFQUI,
  &ClockDomain_LPASSCC_LPAIFSEC,
  &ClockDomain_LPASSCC_LPAIFTER,
  &ClockDomain_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  &ClockDomain_LPASSCC_LPASSLPAAUDIOPLLREFCLKMUX,
  &ClockDomain_LPASSCC_LPASSQDSP6SSCORE,
  &ClockDomain_LPASSCC_LPASSQDSP6SSPLLREFCLKMUX,
  &ClockDomain_LPASSCC_QCASLIMBUS,
  &ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER,
  &ClockDomain_LPASSCC_RESAMPLER,
  &ClockDomain_LPASSCC_SLEEP,
  &ClockDomain_LPASSCC_XO,
};


/*=========================================================================
      Clocks
==========================================================================*/

static ClockNodeType aClocks[] =
{
  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_0_ebit_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
    .pDebugMux = NULL,
    .nUniqueId = 0x29276EE1,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_0_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_1_ebit_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
    .pDebugMux = NULL,
    .nUniqueId = 0x03F0A694,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_1_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_2_ebit_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
    .pDebugMux = NULL,
    .nUniqueId = 0xE632DA6E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_2_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_qui_ebit_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
    .pDebugMux = NULL,
    .nUniqueId = 0x8ADED936,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUI_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCAGGRENOC                                           */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_ce1_axi_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x96B77A7C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_CE1_AXI_CBCR), 0, {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x10} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 181,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_0_m_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x79AD882E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_0_M_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x40} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 130,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_1_m_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x728861DF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_1_M_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x100000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 142,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_2_m_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xFBEA7E74,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_2_M_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x4} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 385,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCAGGRENOCWESTSF                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_lpass_core_axim_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCWESTSF,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x6954D91D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_LPASS_CORE_AXIM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 210,
    },
  },
  {
    .szName    = "gcc_lpass_q6_axi_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCWESTSF,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x93D19999,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_LPASS_Q6_AXI_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 209,
    },
  },
  {
    .szName    = "lpass_audio_core_axim_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCWESTSF,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xE8A0780F,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AXIM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 60,
    },
  },
  {
    .szName    = "lpass_audio_core_hw_af_noc_anoc_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCWESTSF,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xF114D71C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_HW_AF_NOC_ANOC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 99,
    },
  },
  {
    .szName    = "lpass_q6ss_q6_axim_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCWESTSF,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x41E17DB7,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_Q6SS_Q6_AXIM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 79,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCCE1                                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_ce1_clk",
    .pDomain   = &ClockDomain_GCC_GCCCE1,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x3DF99368,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_CE1_CBCR), 0, {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x20} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 180,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCCONFIGNOC                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_boot_rom_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x20C2EB7C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_BOOT_ROM_AHB_CBCR), 0, {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x400} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 160,
    },
  },
  {
    .szName    = "gcc_ce1_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x5BEC4031,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_CE1_AHB_CBCR), 0, {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x8} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 182,
    },
  },
  {
    .szName    = "gcc_prng_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xBFB51CCC,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_PRNG_AHB_CBCR), HWIO_ADDR(GCC_PRNG_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x2000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 155,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_0_s_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xB8C59FEE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_0_S_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x80} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 131,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_1_s_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x3428967E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_1_S_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x200000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 143,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap_2_s_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCCONFIGNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x306EA255,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_2_S_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x2} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 386,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQDSSATBA                                           */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_hw_af_atbm_at_clk",
    .pDomain   = &ClockDomain_GCC_GCCQDSSATBA,
    .pDebugMux = NULL,
    .nUniqueId = 0x4346ED39,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0CORE2X                                   */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_core_2x_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x77020808,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x200} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 133,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap0_core_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x1B88DC58,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x100} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 132,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S0                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s0_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S0,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x56C239B6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S0_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x400} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 134,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S1                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s1_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S1,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x299AA1D4,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S1_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x800} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 135,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S2                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s2_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S2,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x06533C2C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S2_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x1000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 136,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S3                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s3_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S3,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x939588DB,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S3_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x2000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 137,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S4                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s4_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S4,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x62F41847,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S4_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x4000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 138,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S5                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s5_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S5,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x5751B5F3,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S5_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x8000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 139,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S6                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s6_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S6,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x917A11AD,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S6_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x10000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 140,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP0S7                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap0_s7_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0S7,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x69D7DBAF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP0_S7_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x20000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 141,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1CORE2X                                   */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_core_2x_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xD7DB82C0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_2X_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x40000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 145,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap1_core_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x40B685B0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x80000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 144,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S0                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s0_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S0,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xA40B3E1C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S0_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x400000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 146,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S1                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s1_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S1,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xDE8BDAAC,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S1_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x800000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 147,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S2                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s2_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S2,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x45AB34F6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S2_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x1000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 148,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S3                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s3_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S3,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x6CBC20FD,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S3_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x2000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 149,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S4                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s4_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S4,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x2A814B46,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S4_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x4000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 150,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S5                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s5_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S5,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x8279058C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S5_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x8000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 151,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2CORE2X                                   */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_core_2x_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x70CC1B69,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_CORE_2X_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x8} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 388,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap2_core_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xAC5F6885,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_CORE_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x1} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 387,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S0                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s0_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S0,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x62380FFF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S0_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x10} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 389,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S1                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s1_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S1,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xF02791D2,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S1_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x20} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 390,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S2                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s2_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S2,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xA5E32B19,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S2_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x40} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 391,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S3                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s3_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S3,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xEEF92C79,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S3_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x80} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 392,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S4                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s4_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S4,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x7980B17E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S4_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x100} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 393,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP2S5                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap2_s5_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP2S5,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x9777FCA3,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP2_S5_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_2_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_2), 0x200} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 394,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCSYSNOC                                             */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_sysnoc_sway_snoc_clk",
    .pDomain   = &ClockDomain_GCC_GCCSYSNOC,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x971AF290,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SYSNOC_SWAY_SNOC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 63,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_sysnoc_sway_snoc_clk",
    .pDomain   = &ClockDomain_GCC_GCCSYSNOC,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x95F05852,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_SYSNOC_SWAY_SNOC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 67,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_AON                                               */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_qdsp_sway_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x9AB64E1E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QDSP_SWAY_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 61,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xB0DA397B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 68,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_bus_timeout_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xE21DEE2C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_BUS_TIMEOUT_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 77,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_qos_ahbs_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xA0EA237D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_QOS_AHBS_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 69,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_rscc_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x57FBBECD,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_RSCC_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 107,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_sysnoc_sway_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xCD08974D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_SYSNOC_SWAY_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 73,
    },
  },
  {
    .szName    = "lpass_q6ss_ahbm_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x87B1B338,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_Q6SS_AHBM_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 74,
    },
  },
  {
    .szName    = "lpass_q6ss_ahbs_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x22317559,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_Q6SS_AHBS_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 80,
    },
  },
  {
    .szName    = "lpass_q6ss_alt_reset_aon_clk",
    .pDomain   = &ClockDomain_LPASSCC_AON,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x2686EBC1,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_Q6SS_ALT_RESET_AON_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 89,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_ATIME                                             */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_avsync_atime_clk",
    .pDomain   = &ClockDomain_LPASSCC_ATIME,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x0C3E01A9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 47,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_AUDSLIMBUS                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_aud_slimbus_clk",
    .pDomain   = &ClockDomain_LPASSCC_AUDSLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x91822394,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 50,
    },
  },
  {
    .szName    = "lpass_audio_core_aud_slimbus_npl_clk",
    .pDomain   = &ClockDomain_LPASSCC_AUDSLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x46D4F446,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 88,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_codec_spkr_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0x0BFE4F5F,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_pri_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0xE8074F4C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_PRI_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_quad_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0x7C7DA714,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUAD_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_sec_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0xD17EDB16,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_SEC_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_ter_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0x828B6969,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_CORE                                              */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_aud_slimbus_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x731F65B0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 54,
    },
  },
  {
    .szName    = "lpass_audio_core_bus_timeout_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xFEDADA25,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 86,
    },
  },
  {
    .szName    = "lpass_audio_core_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xA8CB75EB,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 52,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_hs_rd_mem_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x945FE5F0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_RD_MEM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 106,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_hs_wr_mem_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xA257919E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_WR_MEM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 105,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x08E66FB9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 87,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem0_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x989A14D9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 56,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem1_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x6A34BE64,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 57,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem2_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x957BF413,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 58,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem3_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x1E34D235,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 59,
    },
  },
  {
    .szName    = "lpass_audio_core_qca_slimbus_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x08F4A4EA,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 55,
    },
  },
  {
    .szName    = "lpass_audio_core_sysnoc_mport_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x36E37E7F,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 53,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_EXTMCLK0                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_wrapper_ext_mclk0_clk",
    .pDomain   = &ClockDomain_LPASSCC_EXTMCLK0,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x71E5841B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_EXT_MCLK0_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 70,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_EXTMCLK1                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_wrapper_ext_mclk1_clk",
    .pDomain   = &ClockDomain_LPASSCC_EXTMCLK1,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x31F0FD44,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_EXT_MCLK1_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 71,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_EXTMCLK2                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_wrapper_ext_mclk2_clk",
    .pDomain   = &ClockDomain_LPASSCC_EXTMCLK2,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x6817EA16,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_EXT_MCLK2_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 72,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_HWAF                                              */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_hw_af_clk",
    .pDomain   = &ClockDomain_LPASSCC_HWAF,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xBCB54026,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_HW_AF_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 98,
    },
  },
  {
    .szName    = "lpass_audio_core_hw_af_noc_clk",
    .pDomain   = &ClockDomain_LPASSCC_HWAF,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x5009E150,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_HW_AF_NOC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 100,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFHSIF0                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_0_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFHSIF0,
    .pDebugMux = NULL,
    .nUniqueId = 0x7101C559,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_0_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFHSIF1                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_1_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFHSIF1,
    .pDebugMux = NULL,
    .nUniqueId = 0xF1866923,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_1_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFHSIF2                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_hs_if_2_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFHSIF2,
    .pDebugMux = NULL,
    .nUniqueId = 0x04E030E6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_HS_IF_2_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFPCMOE                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_pcm_data_oe_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFPCMOE,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x10CA9262,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 46,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFPRI                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_pri_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFPRI,
    .pDebugMux = NULL,
    .nUniqueId = 0x81D6B716,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFQUAD                                         */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_quad_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFQUAD,
    .pDebugMux = NULL,
    .nUniqueId = 0xCBEACB67,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFQUI                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_qui_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFQUI,
    .pDebugMux = NULL,
    .nUniqueId = 0x071745E6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUI_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_qui_osr_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFQUI,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xF16873E8,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUI_OSR_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 36,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFSEC                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_sec_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFSEC,
    .pDebugMux = NULL,
    .nUniqueId = 0xFA1F9AD6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPAIFTER                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_ter_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPAIFTER,
    .pDebugMux = NULL,
    .nUniqueId = 0x5F9E0A9B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX                      */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "lpass_lpaaudio_dig_pll_ref_clk_mux",
    .pDomain   = &ClockDomain_LPASSCC_LPASSLPAAUDIODIGPLLREFCLKMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0xD31E3460,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPASSLPAAUDIOPLLREFCLKMUX                         */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "lpass_lpaaudio_pll_ref_clk_mux",
    .pDomain   = &ClockDomain_LPASSCC_LPASSLPAAUDIOPLLREFCLKMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0xE25797CF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPASSQDSP6SSCORE                                  */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_qdsp6ss_core_clk",
    .pDomain   = &ClockDomain_LPASSCC_LPASSQDSP6SSCORE,
    .pDebugMux = NULL,
    .nUniqueId = 0x32301DE0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_LPASSQDSP6SSPLLREFCLKMUX                          */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "lpass_qdsp6ss_pll_ref_clk_mux",
    .pDomain   = &ClockDomain_LPASSCC_LPASSQDSP6SSPLLREFCLKMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0x49A1E79C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_QCASLIMBUS                                        */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_qca_slimbus_clk",
    .pDomain   = &ClockDomain_LPASSCC_QCASLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xCF3B41A8,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 51,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_qos_dmonitor_fixed_lat_counter_clk",
    .pDomain   = &ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xD3A280EE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 85,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_qos_danger_fixed_lat_counter_clk",
    .pDomain   = &ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x8910E2D8,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_QOS_DANGER_FIXED_LAT_COUNTER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 83,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_qos_dmonitor_fixed_lat_counter_clk",
    .pDomain   = &ClockDomain_LPASSCC_QOSFIXEDLATCOUNTER,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xC42CE894,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_QOS_DMONITOR_FIXED_LAT_COUNTER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 82,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_RESAMPLER                                         */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_resampler_clk",
    .pDomain   = &ClockDomain_LPASSCC_RESAMPLER,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0x48DCDF9C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_RESAMPLER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 49,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_SLEEP                                             */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_bcr_slp_clk",
    .pDomain   = &ClockDomain_LPASSCC_SLEEP,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xC829AB1D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_BCR_SLP_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 64,
    },
  },
  {
    .szName    = "lpass_q6ss_bcr_slp_clk",
    .pDomain   = &ClockDomain_LPASSCC_SLEEP,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xF3DEE96C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_Q6SS_BCR_SLP_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 75,
    },
  },
  {
    .szName    = "lpass_qdsp6ss_sleep_clk",
    .pDomain   = &ClockDomain_LPASSCC_SLEEP,
    .pDebugMux = NULL,
    .nUniqueId = 0x0EFB72AE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_XO                                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_avsync_stc_clk",
    .pDomain   = &ClockDomain_LPASSCC_XO,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xDD63F8F5,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AVSYNC_STC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 48,
    },
  },
  {
    .szName    = "lpass_audio_core_gdsc_xo_clk",
    .pDomain   = &ClockDomain_LPASSCC_XO,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xD77E2753,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_GDSC_XO_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 35,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_qos_xo_lat_counter_clk",
    .pDomain   = &ClockDomain_LPASSCC_XO,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xD222EFDA,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_QOS_XO_LAT_COUNTER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 81,
    },
  },
  {
    .szName    = "lpass_audio_wrapper_rscc_xo_clk",
    .pDomain   = &ClockDomain_LPASSCC_XO,
    .pDebugMux = &ClockDebugMux_LPASSCC,
    .nUniqueId = 0xFD8AE307,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_WRAPPER_RSCC_XO_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 104,
    },
  },
  {
    .szName    = "lpass_qdsp6ss_xo_clk",
    .pDomain   = &ClockDomain_LPASSCC_XO,
    .pDebugMux = NULL,
    .nUniqueId = 0x5295C2DF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
};


/*=========================================================================
      Power Domains
==========================================================================*/

static ClockPowerDomainNodeType aPowerDomains[] =
{
  {
    .szName         = "audio_core_gdsc",
    .nFlags         = 0x0,
    .HALPowerDomain = {
      .nGDSCRAddr   = HWIO_ADDR(LPASS_AUDIO_CORE_GDSCR),
      .pmControl    = &HAL_clk_GenericPowerDomainControl,
    },
  },
};

/*=========================================================================
      Main BSP
==========================================================================*/

ClockBSPType ClockBSP =
{
  .aBases            = NULL,
  .nNumBases         = 0,
  .aRails            = aRails,
  .nNumRails         = SIZEOF_ARRAY(aRails),
  .aClocks           = aClocks,
  .nNumClocks        = SIZEOF_ARRAY(aClocks),
  .aDomains          = aDomains,
  .nNumDomains       = SIZEOF_ARRAY(aDomains),
  .aSources          = aSources,
  .nNumSources       = SIZEOF_ARRAY(aSources),
  .aDividers         = aDividers,
  .nNumDividers      = SIZEOF_ARRAY(aDividers),
  .aPowerDomains     = aPowerDomains,
  .nNumPowerDomains  = SIZEOF_ARRAY(aPowerDomains),
  .aDebugMuxes       = aDebugMuxes,
  .nNumDebugMuxes    = SIZEOF_ARRAY(aDebugMuxes),
  .nFlags            = CLOCK_GLOBAL_FLAG_LOG_CLOCK_FREQ_CHANGE |
                       CLOCK_GLOBAL_FLAG_LOG_SOURCE_STATE_CHANGE,
  .SafeMuxConfig     = { 0, 2, 0, 0 },
};

