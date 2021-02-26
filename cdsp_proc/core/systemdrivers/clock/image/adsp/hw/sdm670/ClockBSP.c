/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains the clock driver BSP data.

  Auto-generated from the following IPCat controller releases:

    SDM670v1 (napaliq_1.0):
      GCC       : z_napaliq_clk_ctl_V1.0_p3q2r57.5" [napali_v2_gcc.r3]
      LPASS_AON_CC: Z_napaliQ_lpass_aon_cc_P3Q2R14.1 [napaliq_lpass_aon.r3]
      LPASS_CORE_CC: z_lpass_napaliq_p3q1r7.0 [napali_lpass_core_cc.r4]


==============================================================================

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/clock/image/adsp/hw/sdm670/ClockBSP.c#2 $

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
extern HAL_clk_PowerDomainControlType     HAL_clk_AudioCorePowerDomainControl;
extern HAL_clk_SourceControlType          HAL_clk_AgeraPLLControl;
extern HAL_clk_SourceControlType          HAL_clk_FabiaPLLControl;

/*
 * Debug mux control structures.
 */
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlGCC;
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlLPASSAONCC;
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlLPASSAONCCQ6;
extern HAL_clk_DebugMuxControlType        HAL_clk_DebugMuxControlLPASSCORECC;


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

static ClockRailType ClockRail_VDDSSCINT =
{
  .szName = "/vcs/vdd_ssc_int",
};

static ClockRailType *aRails[] =
{
  &ClockRail_VDDCX,
  &ClockRail_VDDSSCINT,
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

static ClockDebugMuxType ClockDebugMux_LPASSAONCC =
{
  .szName       = "lpass_aon_cc",
  .pParent      = &ClockDebugMux_GCC,
  .nParentSel   = 200,
  .HALDebugMux  = {
    .pmControl  = &HAL_clk_DebugMuxControlLPASSAONCC,
  },
};

static ClockDebugMuxType ClockDebugMux_LPASSAONCCQ6 =
{
  .szName       = "lpass_aon_cc_q6",
  .pParent      = &ClockDebugMux_LPASSAONCC,
  .nParentSel   = 30,
  .HALDebugMux  = {
    .pmControl  = &HAL_clk_DebugMuxControlLPASSAONCCQ6,
  },
};

static ClockDebugMuxType ClockDebugMux_LPASSCORECC =
{
  .szName       = "lpass_core_cc",
  .pParent      = &ClockDebugMux_LPASSAONCC,
  .nParentSel   = 63,
  .HALDebugMux  = {
    .pmControl  = &HAL_clk_DebugMuxControlLPASSCORECC,
  },
};

static ClockDebugMuxType *aDebugMuxes[] =
{
  &ClockDebugMux_GCC,
  &ClockDebugMux_LPASSAONCC,
  &ClockDebugMux_LPASSAONCCQ6,
  &ClockDebugMux_LPASSCORECC,
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
  {  600000000, &ClockSource_XO, { 0, 0, 1, 31,  0x00004000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {.Fabia.nEven = 2}             }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};

/*
 * ClockSourceBSP_LPASSAONCC_QDSP6SSPLL
 *
 * Used by:
 *   ClockSource_LPASSAONCC_QDSP6SSPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSAONCC_QDSP6SSPLL[] =
{
  {  844800000, &ClockSource_XO, { 0, 0, 1, 44,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
  {  940800000, &ClockSource_XO, { 0, 0, 1, 49,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
  { 1036800000, &ClockSource_XO, { 0, 0, 1, 54,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
  { 1190400000, &ClockSource_XO, { 0, 0, 1, 62,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS_L1,     },
};
static const ClockSourceFreqConfigType ClockSourceBSPCal_LPASSAONCC_QDSP6SSPLL[] =
{
  { 1017600000, &ClockSource_XO, { 0, 0, 1, 53,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {{0}}                          }, RAIL_VOLTAGE_LEVEL_SVS,        },
};

/*
 * ClockSourceBSP_LPASSAONCC_SCCPLL
 *
 * Used by:
 *   ClockSource_LPASSAONCC_SCCPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSAONCC_SCCPLL[] =
{
  {  600000000, NULL           , { 0, 0, 1, 31,  0x00004000, HAL_CLK_PLL_FRAC_MODE_ALPHA,  {.Fabia.nEven = 2}            }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};


/*
 * ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIODIGPLL
 *
 * Used by:
 *   ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIODIGPLL[] =
{
  {  614400000, &ClockSource_XO, { 0, 0, 1, 32,  0x00000000, HAL_CLK_PLL_FRAC_MODE_ALPHA, {.Fabia.nOdd = 5}              }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};

/*
 * ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIOPLL
 *
 * Used by:
 *   ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL
 */
static const ClockSourceFreqConfigType ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIOPLL[] =
{
  { 1128960000, &ClockSource_XO, { 0, 0, 1, 58,  0x00040005, HAL_CLK_PLL_FRAC_MODE_MN, {.Agera.nMain = 2}             }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },
};


/*=========================================================================
      Sources
==========================================================================*/

static ClockSourceNodeType ClockSource_GCC_GPLL0 =
{
  .szName       = "gpll0",
  .nFlags       = CLOCK_FLAG_READ_ONLY,
  .aBSP         = ClockSourceBSP_GCC_GPLL0,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_GCC_GPLL0),
  .pRail        = &ClockRail_VDDCX,
  .nConfigMask  = CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE |
                  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(GCC_GPLL0_MODE),
    .VoterRegister  = {HWIO_ADDR(GCC_PROC_CLK_PLL_ENA_VOTE), 0x1},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSAONCC_QDSP6SSPLL =
{
  .szName       = "qdsp6ss_pll",
  .nFlags       = CLOCK_FLAG_SUPPORTS_SLEWING,
  .aBSP         = ClockSourceBSP_LPASSAONCC_QDSP6SSPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSAONCC_QDSP6SSPLL),
  .pRail        = &ClockRail_VDDSSCINT,
  .nConfigMask  = 0x0,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(SSC_QDSP6SS_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
  .pCalibrationFreqConfig     = ClockSourceBSPCal_LPASSAONCC_QDSP6SSPLL,
  .nCalibrationFreqConfigLen  = SIZEOF_ARRAY(ClockSourceBSPCal_LPASSAONCC_QDSP6SSPLL),
};

static ClockSourceNodeType ClockSource_LPASSAONCC_SCCPLL =
{
  .szName       = "scc_pll",
  .nFlags       = 0x0,
  .aBSP         = ClockSourceBSP_LPASSAONCC_SCCPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSAONCC_SCCPLL),
  .pRail        = &ClockRail_VDDSSCINT,
  .nConfigMask  = CLOCK_CONFIG_PLL_EVEN_OUTPUT_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(SSC_SCC_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL =
{
  .szName       = "lpass_lpaaudio_dig_pll",
  .nFlags       = 0x0,
  .aBSP         = ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIODIGPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIODIGPLL),
  .pRail        = &ClockRail_VDDSSCINT,
  .nConfigMask  = CLOCK_CONFIG_PLL_ODD_OUTPUT_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(LPASS_LPAAUDIO_DIG_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_FabiaPLLControl,
  },
};

static ClockSourceNodeType ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL =
{
  .szName       = "lpass_lpaaudio_pll",
  .nFlags       = 0x0,
  .aBSP         = ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIOPLL,
  .nBSPLen      = SIZEOF_ARRAY(ClockSourceBSP_LPASSCORECC_LPASSLPAAUDIOPLL),
  .pRail        = &ClockRail_VDDSSCINT,
  .nConfigMask  = CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
  .eDisableMode = HAL_CLK_SOURCE_DISABLE_MODE_NORMAL,
  .HALSource = {
    .nAddr          = HWIO_ADDR(LPASS_LPAAUDIO_PLL_MODE),
    .VoterRegister  = {0, 0},
    .pmControl      = &HAL_clk_AgeraPLLControl,
  },
};


static ClockSourceNodeType *aSources[] =
{
  &ClockSource_GCC_GPLL0,
  &ClockSource_LPASSAONCC_QDSP6SSPLL,
  &ClockSource_LPASSAONCC_SCCPLL,
  &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL,
  &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,
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
    .szName    = "scc_cdiv_ccd_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_CDIV_CCD_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_cdiv_qupv3_2xcore_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_cdiv_qupv3_core_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_cdiv_sdc_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_CDIV_SDC_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_main_rcg_cdiv_bus_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_main_rcg_cdiv_dbg_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_main_rcg_cdiv_dbg_tsctr_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_pll_lv_test_postdiv_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = 0,
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_pll_out_main_cdiv_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR),
      .pmControl   = &HAL_clk_GenericDividerControl,
    },
  },
  {
    .szName    = "scc_qtimer_tmr_cdiv_div_clk_src",
    .nFlags    = 0x0,
    .HALDivider = {
      .nAddr       = HWIO_ADDR(SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR),
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
 *   ClockDomain_GCC_GCCQUPV3WRAP1S6
 *   ClockDomain_GCC_GCCQUPV3WRAP1S7
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
 * ClockDomainBSP_LPASSAONCC_Q6PLLREF
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_Q6PLLREF
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_Q6PLLREF[] =
{
  {   19200000, &ClockSource_XO,                          { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = GATED_XO_CLK */
};

/*
 * ClockDomainBSP_LPASSAONCC_QDSP6SSRCG
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_QDSP6SSRCG
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_QDSP6SSRCG[] =
{
  {  300000000, &ClockSource_LPASSAONCC_SCCPLL,           { 4,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                              },  /* Mux[4] = SCC_PLL_OUT_MAIN_CDIV_DIV_CLK_SRC */
  {  400000000, &ClockSource_LPASSAONCC_SCCPLL,           { 3,     3,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,                                                 /* 600.0  */ },  /* Mux[3] = SCC_PLL_OUT_MAIN     */
  {  600000000, &ClockSource_LPASSAONCC_SCCPLL,           { 3,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,                                                      /* 600.0  */ },  /* Mux[3] = SCC_PLL_OUT_MAIN     */
  {  844800000, &ClockSource_LPASSAONCC_QDSP6SSPLL,       { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        {{ 0 }}, &ClockSourceBSP_LPASSAONCC_QDSP6SSPLL[ 0]    /* 840.8  */ },  /* Mux[2] = QDSP6SS_PLL_OUT_MAIN */ 
  {  940800000, &ClockSource_LPASSAONCC_QDSP6SSPLL,       { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        {{ 0 }}, &ClockSourceBSP_LPASSAONCC_QDSP6SSPLL[ 1]    /* 940.0  */ },  /* Mux[2] = QDSP6SS_PLL_OUT_MAIN */
  { 1036800000, &ClockSource_LPASSAONCC_QDSP6SSPLL,       { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_TUR,        {{ 0 }}, &ClockSourceBSP_LPASSAONCC_QDSP6SSPLL[ 2]    /* 1036.8  */},  /* Mux[2] = QDSP6SS_PLL_OUT_MAIN */
  { 1190400000, &ClockSource_LPASSAONCC_QDSP6SSPLL,       { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_TUR,        {{ 0 }}, &ClockSourceBSP_LPASSAONCC_QDSP6SSPLL[ 3]    /* 1190.0 */ },  /* Mux[2] = QDSP6SS_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSAONCC_SCCMAINRCG
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCMAINRCG
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_SCCMAINRCG[] =
{
  {   50000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,    12,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  200000000, &ClockSource_LPASSAONCC_SCCPLL,           { 6,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = SCC_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSAONCC_SCCPLLREF
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCPLLREF
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_SCCPLLREF[] =
{
  {   19200000, NULL,                                    { 1,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[1] = GATED_RO_CLK */
};


/*
 * ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0
 *
 * Note: These frequencies are used by domains possibly under DFS control. The
 *       DFS-enable state will be detected during init. If under DFS control,
 *       the domain will dynamically allocate its own BSP array and populate
 *       it with configurations detected from the DFS registers.
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE0
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE1
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE2
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE3
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE4
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE5
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0[] =
{
  {    7372800, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     384,   15625, 0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   14745600, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     768,   15625, 0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   19200000, NULL,                                     { 0,     2,     0,     0,     0x00 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                             },   /* Mux[0] = GATED_RO_CLK */
  {   29491200, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     1536,  15625,  0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   32000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     8,     75,    0x01 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   48000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     4,     25,    0x02 }, RAIL_VOLTAGE_LEVEL_MIN_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   64000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     16,    75,     0x03 }, RAIL_VOLTAGE_LEVEL_LOW_SVS,                                                /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {   80000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     4,     15,    0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */   
  {   96000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     8,     25,    0x04 }, RAIL_VOLTAGE_LEVEL_LOW_SVS,                                                 /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  100000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     6,    0,     0,     0x05 }, RAIL_VOLTAGE_LEVEL_LOW_SVS,                                                  /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  102400000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,    128,   375,    0xFF }, RAIL_VOLTAGE_LEVEL_SVS,                                                     /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  112000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     28,    75,    0xFF }, RAIL_VOLTAGE_LEVEL_SVS,                                                     /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  117964800, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,    6144,  15625, 0xFF }, RAIL_VOLTAGE_LEVEL_SVS,                                                      /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  120000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     5,     0,     0,    0x06 }, RAIL_VOLTAGE_LEVEL_SVS,                                                      /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
  {  128000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     2,     32,    75,    0xFF }, RAIL_VOLTAGE_LEVEL_SVS,                                                     /* 300.0  */},   /* Mux[2] = SCC_PLL_OUT_EVEN */
};

/*
 * ClockDomainBSP_LPASSAONCC_SCCVSVDDCX
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCVSVDDCX
 *   ClockDomain_LPASSAONCC_SCCVSVDDMX
 */
static ClockMuxConfigType ClockDomainBSP_LPASSAONCC_SCCVSVDDCX[] =
{
  {   19200000, NULL,                                     { 0,     2,     0,     0,      0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },   /* Mux[0] = GATED_RO_CLK */
  {  100000000, &ClockSource_LPASSAONCC_SCCPLL,           { 2,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,         },   /* Mux[2] = SCC_PLL_OUT_EVEN */
};

/*
 * ClockDomainBSP_LPASSCORECC_ATIME
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_ATIME
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_ATIME[] =
{
  {   15360000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                                  /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   19200000, &ClockSource_XO,                              { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                                  /* Mux[0] = XO */
  {   30720000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                                  /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   61440000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                                  /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {  122880000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                                  /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_AUDSLIMBUS
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_AUDSLIMBUS
 *   ClockDomain_LPASSCORECC_QCASLIMBUS
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_AUDSLIMBUS[] =
{
  {    6144000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   12288000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   24576000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_CORE
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_CORE
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_CORE[] =
{
  {   19200000, &ClockSource_XO,                              { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
  {   25600000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     24,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   51200000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     12,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  102400000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  204800000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 1,     6,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCORECC_EXTMCLK0
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_EXTMCLK0
 *   ClockDomain_LPASSCORECC_EXTMCLK1
 *   ClockDomain_LPASSCORECC_EXTMCLK2
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_EXTMCLK0[] =
{
  {     256000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     352800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                              { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   22579200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   24576000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_INTI2S01
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_INTI2S01
 *   ClockDomain_LPASSCORECC_INTI2S2
 *   ClockDomain_LPASSCORECC_INTI2S3
 *   ClockDomain_LPASSCORECC_INTI2S4
 *   ClockDomain_LPASSCORECC_INTI2S56
 *   ClockDomain_LPASSCORECC_LPAIFPRI
 *   ClockDomain_LPASSCORECC_LPAIFQUAD
 *   ClockDomain_LPASSCORECC_LPAIFSEC
 *   ClockDomain_LPASSCORECC_LPAIFSPKR
 *   ClockDomain_LPASSCORECC_LPAIFTER
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_INTI2S01[] =
{
  {     256000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     352800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                              { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   16384000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     10,    2,     3,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   22579200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   24576000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_INTMCLK0
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_INTMCLK0
 *   ClockDomain_LPASSCORECC_INTMCLK1
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_INTMCLK0[] =
{
  {     256000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     352800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     32,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     512000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {     705600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {     768000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     16,    0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1024000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    1411200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    1536000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     8,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2048000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    2822400, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    3072000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     4,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    4096000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    5644800, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {    6144000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    1,     2,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    8192000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     30,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {    9600000, &ClockSource_XO,                              { 0,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[0] = XO */
  {   11289600, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   12288000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     20,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   22579200, &ClockSource_LPASSCORECC_LPASSLPAAUDIOPLL,    { 6,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[6] = LPASS_LPAAUDIO_PLL_OUT_AUX_DIV_CLK_SRC */
  {   24576000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     10,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_LPAIFPCMOE
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_LPAIFPCMOE
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_LPAIFPCMOE[] =
{
  {   15360000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   30720000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {   61440000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
  {  122880000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 5,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[5] = LPASS_LPAAUDIO_DIG_PLL_OUT_ODD */
};

/*
 * ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX
 *   ClockDomain_LPASSCORECC_LPASSLPAAUDIOPLLREFCLKMUX
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX[] =
{
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                                /* Mux[0] = XO */
};

/*
 * ClockDomainBSP_LPASSCORECC_RESAMPLER
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_RESAMPLER
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_RESAMPLER[] =
{
  {   38400000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     16,    0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {   76800000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_LOW_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  153600000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     4,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  307200000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_NOM,        },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
};

/*
 * ClockDomainBSP_LPASSCORECC_SWNPL
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_SWNPL
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_SWNPL[] =
{
  {   76800000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 2,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                            /* Mux[2] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN_DIV_CLK_SRC */
  {  153600000, &ClockSource_LPASSCORECC_LPASSLPAAUDIODIGPLL, { 1,     8,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_SVS,        },                                                            /* Mux[1] = LPASS_LPAAUDIO_DIG_PLL_OUT_MAIN */
};

/*
 * ClockDomainBSP_LPASSCORECC_XO
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_XO
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_XO[] =
{
  {   19200000, &ClockSource_XO,                          { 0,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                                /* Mux[0] = XO */
};

/*
 * ClockDomainBSP_LPASSCORECC_SLEEP
 *
 * Used by:
 *   ClockDomain_LPASSCORECC_SLEEP
 */
static ClockMuxConfigType ClockDomainBSP_LPASSCORECC_SLEEP[] =
{
  {   32768, NULL,                          { 1,     2,     0,     0,     0xFF }, RAIL_VOLTAGE_LEVEL_MIN_SVS,    },                                                                /* Mux[0] = XO */
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
 *   ClockDomain_GCC_GCCQUPV3WRAP1S6
 *   ClockDomain_GCC_GCCQUPV3WRAP1S7
 */
static const ClockSourceMapType ClockSourceMap_GCC_GCCQUPV3WRAP0S0[] =
{

  { 0,   19200000, &ClockSource_XO },
  { 1,  600000000, &ClockSource_GCC_GPLL0 },
  { 6,  300000000, &ClockSource_GCC_GPLL0 },
  { 7,  400000000, NULL },
  { CLOCK_SOURCE_MAP_END }
};

/*
 * ClockSourceMap_LPASSAONCC_SCCQUPV3SE0
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCMAINRCGCDIVQ6SPMDIV
 *   ClockDomain_LPASSAONCC_SCCMAINRCG
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE0
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE1
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE2
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE3
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE4
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE5
 *   ClockDomain_LPASSAONCC_SCCVSVDDCX
 *   ClockDomain_LPASSAONCC_SCCVSVDDMX
 */
static const ClockSourceMapType ClockSourceMap_LPASSAONCC_SCCQUPV3SE0[] =
{
  { 0,   19200000, NULL                           },
  { 1,      32768, NULL                           },
  { 2,  300000000, &ClockSource_LPASSAONCC_SCCPLL },
  { 3,   19200000, &ClockSource_XO                },
  { 4,  600000000, &ClockSource_LPASSAONCC_SCCPLL },
  { 5,          0, &ClockSource_LPASSAONCC_QDSP6SSPLL },
  { 6,  600000000, &ClockSource_LPASSAONCC_SCCPLL },
  { 7,  400000000, NULL                           },
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
 *   ClockDomain_GCC_GCCQUPV3WRAP1S6
 *   ClockDomain_GCC_GCCQUPV3WRAP1S7
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
 * ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0
 *
 * Used by:
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE0
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE1
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE2
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE3
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE4
 *   ClockDomain_LPASSAONCC_SCCQUPV3SE5
 */
static const ClockFMaxType ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0[] =
{
  {   50000000, RAIL_VOLTAGE_LEVEL_MIN_SVS },
  {  100000000, RAIL_VOLTAGE_LEVEL_LOW_SVS },
  {  150000000, RAIL_VOLTAGE_LEVEL_SVS },
  {  200000000, RAIL_VOLTAGE_LEVEL_NOM },
  {  0  }
};

/*
 * ClockDomainFMax_LPASSCC_CLOCKDRIVERGENEBIT
 *
 * Used by:
 *   ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT
 *   ClockDomain_LPASSCC_LPAIFPRI
 *   ClockDomain_LPASSCC_LPAIFQUAD
 *   ClockDomain_LPASSCC_LPAIFSEC
 *   ClockDomain_LPASSCC_LPAIFSPKR
 *   ClockDomain_LPASSCC_LPAIFTER
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

static ClockDomainNodeType ClockDomain_GCC_CLOCKDRIVERGENXO =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = NULL,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

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

static ClockDomainNodeType ClockDomain_GCC_GCCAGGRENOCNORTHSF =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_READ_ONLY |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_AGGRE_NOC_NORTH_SF_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCCE1 =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDCX,
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

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S6 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S6_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_GCC_GCCQUPV3WRAP1S7 =
{
  .aBSP      = ClockDomainBSP_GCC_GCCQUPV3WRAP0S0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_GCC_GCCQUPV3WRAP0S0),
  .pRail     = &ClockRail_VDDCX,
  .aFMax     = ClockDomainFMax_GCC_GCCQUPV3WRAP0S0,
  .aSourceMap = ClockSourceMap_GCC_GCCQUPV3WRAP0S0,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(GCC_QUPV3_WRAP1_S7_CMD_RCGR),
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

static ClockDomainNodeType ClockDomain_LPASSAONCC_Q6PLLREF =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_Q6PLLREF,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_Q6PLLREF),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_QDSP6SSRCG =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_QDSP6SSRCG,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_QDSP6SSRCG),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_QDSP6SS_CORE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCDEBUGMUX =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCMAINRCGCDIVQ6SPMDIV =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCMAINRCG =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCMAINRCG,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCMAINRCG),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_MAIN_RCG_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCPLLLVTESTPREDIV =
{
  .aBSP      = NULL,
  .nBSPLen   = 0,
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCPLLREF =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCPLLREF,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCPLLREF),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE0 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE1 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE2 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE3 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE3_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE4 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE4_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCQUPV3SE5 =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCQUPV3SE0),
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSAONCC_SCCQUPV3SE0,
  .aSourceMap = ClockSourceMap_LPASSAONCC_SCCQUPV3SE0,
  .pSource   = NULL,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF |
               CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_QUPV3_SE5_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
    .nDFSStates = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCVSVDDCX =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCVSVDDCX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCVSVDDCX),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_VS_VDDCX_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSAONCC_SCCVSVDDMX =
{
  .aBSP      = ClockDomainBSP_LPASSAONCC_SCCVSVDDCX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSAONCC_SCCVSVDDCX),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(SSC_SCC_VS_VDDMX_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_ATIME =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_ATIME,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_ATIME),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_ATIME_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_AUDSLIMBUS =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_AUDSLIMBUS,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_AUDSLIMBUS),
  .pRail     = &ClockRail_VDDSSCINT,
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
  .pRail     = &ClockRail_VDDSSCINT,
  .aFMax     = ClockDomainFMax_LPASSCC_CLOCKDRIVERGENEBIT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_CORE =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_CORE,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_CORE),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_CORE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_EXTMCLK0 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_EXTMCLK0),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_EXTMCLK1 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_EXTMCLK0),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_EXTMCLK2 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_EXTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_EXTMCLK0),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_EXT_MCLK2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTI2S01 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_I2S0_1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTI2S2 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_I2S2_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTI2S3 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_I2S3_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTI2S4 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_I2S4_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTI2S56 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_I2S5_6_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTMCLK0 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTMCLK0),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_MCLK0_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_INTMCLK1 =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTMCLK0,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTMCLK0),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_INT_MCLK1_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFPCMOE =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_LPAIFPCMOE,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_LPAIFPCMOE),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_PCMOE_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFPRI =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_PRI_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFQUAD =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_QUAD_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFSEC =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_SEC_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFSPKR =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_SPKR_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPAIFTER =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_INTI2S01,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_INTI2S01),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_LPAIF_TER_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 16,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_LPASSLPAAUDIOPLLREFCLKMUX =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_QCASLIMBUS =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_AUDSLIMBUS,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_AUDSLIMBUS),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_QCA_SLIMBUS_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_RESAMPLER =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_RESAMPLER,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_RESAMPLER),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_RESAMPLER_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_SWNPL =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_SWNPL,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_SWNPL),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_SW_NPL_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
    .nMNDWidth  = 8,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_XO =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_XO,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_XO),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = &ClockSource_XO,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = HWIO_ADDR(LPASS_XO_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType ClockDomain_LPASSCORECC_SLEEP =
{
  .aBSP      = ClockDomainBSP_LPASSCORECC_SLEEP,
  .nBSPLen   = SIZEOF_ARRAY(ClockDomainBSP_LPASSCORECC_SLEEP),
  .pRail     = &ClockRail_VDDSSCINT,
  .pSource   = NULL,
  .nFlags    = 0x0,
  .HALDomain = {
    .nCGRAddr   = 0,//HWIO_ADDR(LPASS_SLEEP_CMD_RCGR),
    .pmControl  = &HAL_clk_GenericDomainControl,
  },
};

static ClockDomainNodeType *aDomains[] =
{
  &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
  &ClockDomain_GCC_CLOCKDRIVERGENXO,
  &ClockDomain_GCC_GCCAGGRENOC,
  &ClockDomain_GCC_GCCAGGRENOCNORTHSF,
  &ClockDomain_GCC_GCCCE1,
  &ClockDomain_GCC_GCCCONFIGNOC,
  &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
  &ClockDomain_GCC_GCCQUPV3WRAP0S0,
  &ClockDomain_GCC_GCCQUPV3WRAP0S1,
  &ClockDomain_GCC_GCCQUPV3WRAP0S2,
  &ClockDomain_GCC_GCCQUPV3WRAP0S3,
  &ClockDomain_GCC_GCCQUPV3WRAP0S4,
  &ClockDomain_GCC_GCCQUPV3WRAP0S5,
  &ClockDomain_GCC_GCCQUPV3WRAP0S6,
  &ClockDomain_GCC_GCCQUPV3WRAP0S7,
  &ClockDomain_GCC_GCCQUPV3WRAP1S0,
  &ClockDomain_GCC_GCCQUPV3WRAP1S1,
  &ClockDomain_GCC_GCCQUPV3WRAP1S2,
  &ClockDomain_GCC_GCCQUPV3WRAP1S3,
  &ClockDomain_GCC_GCCQUPV3WRAP1S4,
  &ClockDomain_GCC_GCCQUPV3WRAP1S5,
  &ClockDomain_GCC_GCCQUPV3WRAP1S6,
  &ClockDomain_GCC_GCCQUPV3WRAP1S7,
  &ClockDomain_GCC_GCCSYSNOC,
  &ClockDomain_LPASSAONCC_Q6PLLREF,
  &ClockDomain_LPASSAONCC_QDSP6SSRCG,
  &ClockDomain_LPASSAONCC_SCCDEBUGMUX,
  &ClockDomain_LPASSAONCC_SCCMAINRCG,
  &ClockDomain_LPASSAONCC_SCCMAINRCGCDIVQ6SPMDIV,
  &ClockDomain_LPASSAONCC_SCCPLLLVTESTPREDIV,
  &ClockDomain_LPASSAONCC_SCCPLLREF,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE0,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE1,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE2,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE3,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE4,
  &ClockDomain_LPASSAONCC_SCCQUPV3SE5,
  &ClockDomain_LPASSAONCC_SCCVSVDDCX,
  &ClockDomain_LPASSAONCC_SCCVSVDDMX,
  &ClockDomain_LPASSCORECC_ATIME,
  &ClockDomain_LPASSCORECC_AUDSLIMBUS,
  &ClockDomain_LPASSCORECC_CORE,
  &ClockDomain_LPASSCORECC_EXTMCLK0,
  &ClockDomain_LPASSCORECC_EXTMCLK1,
  &ClockDomain_LPASSCORECC_EXTMCLK2,
  &ClockDomain_LPASSCORECC_INTI2S01,
  &ClockDomain_LPASSCORECC_INTI2S2,
  &ClockDomain_LPASSCORECC_INTI2S3,
  &ClockDomain_LPASSCORECC_INTI2S4,
  &ClockDomain_LPASSCORECC_INTI2S56,
  &ClockDomain_LPASSCORECC_INTMCLK0,
  &ClockDomain_LPASSCORECC_INTMCLK1,
  &ClockDomain_LPASSCORECC_LPAIFPCMOE,
  &ClockDomain_LPASSCORECC_LPAIFPRI,
  &ClockDomain_LPASSCORECC_LPAIFQUAD,
  &ClockDomain_LPASSCORECC_LPAIFSEC,
  &ClockDomain_LPASSCORECC_LPAIFSPKR,
  &ClockDomain_LPASSCORECC_LPAIFTER,
  &ClockDomain_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX,
  &ClockDomain_LPASSCORECC_LPASSLPAAUDIOPLLREFCLKMUX,
  &ClockDomain_LPASSCORECC_QCASLIMBUS,
  &ClockDomain_LPASSCORECC_RESAMPLER,
  &ClockDomain_LPASSCORECC_SWNPL,
  &ClockDomain_LPASSCORECC_XO,
  &ClockDomain_LPASSCORECC_SLEEP,
};


/*=========================================================================
      Clocks
==========================================================================*/

static ClockNodeType aClocks[] =
{
  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_CLOCKDRIVERGENXO                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "qdsp6ss_xo_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENXO,
    .pDebugMux = &ClockDebugMux_LPASSAONCCQ6,
    .nUniqueId = 0x3532185D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_QDSP6SS_XO_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 6, 
    },
  },
  {
    .szName    = "scc_qsm_xo_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENXO,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x84E4D3B0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QSM_XO_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 15,
    },
  },
  {
    .szName    = "scc_ro_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENXO,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x375DE3E7,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_RO_CBCR), HWIO_ADDR(SSC_SCC_AOSS_CC_RO_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 55,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_codec_spkr_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0x498751E9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_pri_ebit_clk",
    .pDomain   = &ClockDomain_LPASSCC_CLOCKDRIVERGENEBIT,
    .pDebugMux = NULL,
    .nUniqueId = 0xA2C3C174,
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
    .nUniqueId = 0x83684299,
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
    .nUniqueId = 0x48E40B6B,
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
    .nUniqueId = 0x4F796CB0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_TER_EBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },
  
  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE                                */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "qdsp6ss_sleep_clk",
    .pDomain   = &ClockDomain_GCC_CLOCKDRIVERGENNOSOURCE,
    .pDebugMux = &ClockDebugMux_LPASSAONCCQ6,
    .nUniqueId = 0xA07462B5,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_QDSP6SS_SLEEP_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 7,
    },
  },
  {
    .szName    = "scc_sleep_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_SLEEP,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xAF817241,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SLEEP_CBCR), HWIO_ADDR(SSC_SCC_SLEEP_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 46,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCAGGRENOC                                           */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap_0_m_ahb_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x79AD882E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP_0_M_AHB_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_0_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x40} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 116,
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
      .nDebugMuxSel = 130,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCAGGRENOCNORTHSF                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_lpass_core_axim_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCNORTHSF,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x6954D91D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_LPASS_CORE_AXIM_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 196,
    },
  },
  {
    .szName    = "gcc_lpass_q6_axi_clk",
    .pDomain   = &ClockDomain_GCC_GCCAGGRENOCNORTHSF,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x93D19999,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_LPASS_Q6_AXI_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 195,
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
      .nDebugMuxSel = 167,
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
      .nDebugMuxSel = 148,
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
      .nDebugMuxSel = 169,
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
      .nDebugMuxSel = 143,
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
      .nDebugMuxSel = 117,
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
      .nDebugMuxSel = 131,
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
      .nDebugMuxSel = 119,
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
      .nDebugMuxSel = 118,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap1_core_2x_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0xD7DB82C0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_2X_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x40000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 128,
    },
  },
  {
    .szName    = "gcc_qupv3_wrap1_core_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP0CORE2X,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x40B685B0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_CORE_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x80000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 129,
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
      .nDebugMuxSel = 120,
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
      .nDebugMuxSel = 121,
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
      .nDebugMuxSel = 122,
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
      .nDebugMuxSel = 123,
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
      .nDebugMuxSel = 124,
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
      .nDebugMuxSel = 125,
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
      .nDebugMuxSel = 126,
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
      .nDebugMuxSel = 127,
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
      .nDebugMuxSel = 132,
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
      .nDebugMuxSel = 133,
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
      .nDebugMuxSel = 134,
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
      .nDebugMuxSel = 135,
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
      .nDebugMuxSel = 136,
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
      .nDebugMuxSel = 137,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S6                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s6_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S6,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x6FB2266A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S6_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x10000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 138,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCQUPV3WRAP1S7                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_qupv3_wrap1_s7_clk",
    .pDomain   = &ClockDomain_GCC_GCCQUPV3WRAP1S7,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x90473589,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_QUPV3_WRAP1_S7_CBCR), HWIO_ADDR(GCC_QUPV3_WRAPPER_1_BCR), {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE_1), 0x20000000} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 139,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_GCC_GCCSYSNOC                                             */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "gcc_ce1_axi_clk",
    .pDomain   = &ClockDomain_GCC_GCCSYSNOC,
    .pDebugMux = &ClockDebugMux_GCC,
    .nUniqueId = 0x96B77A7C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(GCC_CE1_AXI_CBCR), 0, {HWIO_ADDR(GCC_PROC_CLK_BRANCH_ENA_VOTE), 0x10} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 168,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_Q6PLLREF                                       */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "q6_pll_ref_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_Q6PLLREF,
    .pDebugMux = NULL,
    .nUniqueId = 0x0E078901,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_QDSP6SSRCG                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "q6_core_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_QDSP6SSRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCCQ6,
    .nUniqueId = 0xA7F615FF,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 1,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCDEBUGMUX                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_debug_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCDEBUGMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0xDFE51C05,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_DEBUG_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCMAINRCGCDIVQ6SPMDIV                         */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_q6_spm_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCGCDIVQ6SPMDIV,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x3E836181,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_SPM_CBCR), HWIO_ADDR(SSC_SCC_Q6_SPM_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 11,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCMAINRCG                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_ahb_timeout_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xAE077C11,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_AHB_TIMEOUT_CBCR), HWIO_ADDR(SSC_SCC_AHB_TIMEOUT_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 20,
    },
  },
  {
    .szName    = "scc_at_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xD89548D2,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_AT_CBCR), HWIO_ADDR(SSC_SCC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 23,
    },
  },
  {
    .szName    = "scc_bus_alt_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x93887291,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_BUS_ALT_CBCR), HWIO_ADDR(SSC_SCC_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 57,
    },
  },
  {
    .szName    = "scc_ccd_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x09C2B51A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_CCD_CBCR), HWIO_ADDR(SSC_SCC_CCD_CORE_AND_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 5,
    },
  },
  {
    .szName    = "scc_ccd_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x86B7F3F4,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_CCD_HCLK_CBCR), HWIO_ADDR(SSC_SCC_CCD_CORE_AND_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 50,
    },
  },
  {
    .szName    = "scc_ccd_hclk_noc_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x6553DA6D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_CCD_CORE_AND_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 48,
    },
  },
  {
    .szName    = "scc_ccd_hclk_ungated_to_odsc_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xBF2E0353,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR), HWIO_ADDR(SSC_SCC_CCD_CORE_AND_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 56,
    },
  },
  {
    .szName    = "scc_crif_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xDD3FB8F3,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_CRIF_CBCR), HWIO_ADDR(SSC_SCC_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 12,
    },
  },
  {
    .szName    = "scc_csr_h_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x1851C837,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_CSR_H_CBCR), HWIO_ADDR(SSC_SCC_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 13,
    },
  },
  {
    .szName    = "scc_data_h_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x4B54D279,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_DATA_H_CBCR), HWIO_ADDR(SSC_SCC_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 47,
    },
  },
  {
    .szName    = "scc_dbg_tsctr_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xCC62C5FD,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_DBG_TSCTR_CBCR), HWIO_ADDR(SSC_SCC_DBG_TSCTR_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 10,
    },
  },
  {
    .szName    = "scc_noc_atb_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xD62B0FD6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_NOC_ATB_CBCR), HWIO_ADDR(SSC_SCC_NOC_ATB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 41,
    },
  },
  {
    .szName    = "scc_noc_bus_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xE62E2392,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_NOC_BUS_CBCR), HWIO_ADDR(SSC_SCC_NOC_BUS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 35,
    },
  },
  {
    .szName    = "scc_pclkdbg_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x3C85487B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_PCLKDBG_CBCR), HWIO_ADDR(SSC_SCC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 25,
    },
  },
  {
    .szName    = "scc_q6_ahbm_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xB7DE0EC3,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_AHBM_CBCR), HWIO_ADDR(SSC_SCC_Q6_AHB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 16,
    },
  },
  {
    .szName    = "scc_q6_ahbs_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xA050974C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_AHBS_CBCR), HWIO_ADDR(SSC_SCC_Q6_AHB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 17,
    },
  },
  {
    .szName    = "scc_q6_atbm_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x309AE421,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_ATBM_CBCR), HWIO_ADDR(SSC_SCC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 24,
    },
  },
  {
    .szName    = "scc_q6_xpu2_client_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x2911276A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_XPU2_CLIENT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 21,
    },
  },
  {
    .szName    = "scc_q6_xpu2_config_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xCA3DE819,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_Q6_XPU2_CONFIG_CBCR), HWIO_ADDR(SSC_SCC_Q6_XPU2_CONFIG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 22,
    },
  },
  {
    .szName    = "scc_qtimer_ahb_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xC97694E5,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QTIMER_AHB_CBCR), HWIO_ADDR(SSC_SCC_QTIMER_AHB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 34,
    },
  },
  {
    .szName    = "scc_qtimer_tmr_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x8218AFE9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QTIMER_TMR_CBCR), HWIO_ADDR(SSC_SCC_QTIMER_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 14,
    },
  },
  {
    .szName    = "scc_qupv3_2xcore_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x217B8E8B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_2XCORE_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_CORE_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x400} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 42,
    },
  },
  {
    .szName    = "scc_qupv3_core_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x600799B0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_CORE_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_CORE_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x800} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 43,
    },
  },
  {
    .szName    = "scc_qupv3_m_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x4680EB9C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_M_HCLK_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_BUS_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x2} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 32,
    },
  },
  {
    .szName    = "scc_qupv3_s_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x3007045B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_S_HCLK_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_BUS_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x1} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 18,
    },
  },
  {
    .szName    = "scc_rsc_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xCA80BAE2,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_RSC_HCLK_CBCR), HWIO_ADDR(SSC_SCC_RSC_HCLK_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 33,
    },
  },
  {
    .szName    = "scc_sdc_bus_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x8D1BCEC0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_BUS_HCLK_CBCR), HWIO_ADDR(SSC_SCC_SDC_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 45,
    },
  },
  {
    .szName    = "scc_sdc_bus_hclk_noc_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x50117349,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_SDC_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 49,
    },
  },
  {
    .szName    = "scc_sdc_cti_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x1FE8C7E4,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_CTI_CBCR), HWIO_ADDR(SSC_SCC_SDC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 38,
    },
  },
  {
    .szName    = "scc_sdc_cti_pclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xFD51883E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_CTI_PCLK_CBCR), HWIO_ADDR(SSC_SCC_SDC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 37,
    },
  },
  {
    .szName    = "scc_sdc_dap_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x4FE680AC,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_DAP_CBCR), HWIO_ADDR(SSC_SCC_SDC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 36,
    },
  },
  {
    .szName    = "scc_sdc_proc_fclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xF4461EBE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_PROC_FCLK_CBCR), HWIO_ADDR(SSC_SCC_SDC_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 9,
    },
  },
  {
    .szName    = "scc_sdc_proc_hclk_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x6E513B04,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_PROC_HCLK_CBCR), HWIO_ADDR(SSC_SCC_SDC_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 44,
    },
  },
  {
    .szName    = "scc_sdc_qdss_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x09B3B53B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_QDSS_CBCR), HWIO_ADDR(SSC_SCC_SDC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 39,
    },
  },
  {
    .szName    = "scc_sdc_ts_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x43D854D3,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_TS_CBCR), HWIO_ADDR(SSC_SCC_SDC_DBG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 40,
    },
  },
  {
    .szName    = "scc_sdc_xpu2_client_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x41B02DA0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_XPU2_CLIENT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 53,
    },
  },
  {
    .szName    = "scc_sdc_xpu2_config_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x74E43E2F,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SDC_XPU2_CONFIG_CBCR), HWIO_ADDR(SSC_SCC_SDC_XPU2_CONFIG_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 54,
    },
  },
  {
    .szName    = "scc_smem_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x986A33B9,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_SMEM_CBCR), HWIO_ADDR(SSC_SCC_SMEM_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 19,
    },
  },
  {
    .szName    = "scc_vs_cx_ahb_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xEFEB7B00,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_VS_CX_AHB_CBCR), HWIO_ADDR(SSC_SCC_VS_CX_AHB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 51,
    },
  },
  {
    .szName    = "scc_vs_mx_ahb_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCMAINRCG,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x708A45F2,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_VS_MX_AHB_CBCR), HWIO_ADDR(SSC_SCC_VS_MX_AHB_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 52,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCPLLLVTESTPREDIV                             */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_pll_lv_test_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCPLLLVTESTPREDIV,
    .pDebugMux = NULL,
    .nUniqueId = 0xC0A0C695,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_PLL_LV_TEST_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCPLLREF                                      */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "scc_pll_ref_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCPLLREF,
    .pDebugMux = NULL,
    .nUniqueId = 0x3595E773,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE0                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se0_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE0,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xA468C1B6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE0_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE0_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x8} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 2,
    },
  },
  {
    .szName    = "scc_qupv3_se0_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE0,
    .pDebugMux = NULL,
    .nUniqueId = 0x13326D95,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE0_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE1                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se1_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE1,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xAA15C784,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE1_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE1_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x10} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 3,
    },
  },
  {
    .szName    = "scc_qupv3_se1_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE1,
    .pDebugMux = NULL,
    .nUniqueId = 0xC8A4B77E,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE1_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE2                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se2_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE2,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x7DADD310,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE2_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE2_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x20} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 4,
    },
  },
  {
    .szName    = "scc_qupv3_se2_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE2,
    .pDebugMux = NULL,
    .nUniqueId = 0xB17C8A24,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE2_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE3                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se3_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE3,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0xB68904E4,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE3_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE3_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x40} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 6,
    },
  },
  {
    .szName    = "scc_qupv3_se3_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE3,
    .pDebugMux = NULL,
    .nUniqueId = 0xE5069374,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE3_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE4                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se4_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE4,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x60739DC7,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE4_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE4_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x80} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 7,
    },
  },
  {
    .szName    = "scc_qupv3_se4_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE4,
    .pDebugMux = NULL,
    .nUniqueId = 0x268ECD27,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE4_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCQUPV3SE5                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_qupv3_se5_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE5,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x714E95F0,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_QUPV3_SE5_CBCR), HWIO_ADDR(SSC_SCC_QUPV3_SE5_BCR), {HWIO_ADDR(SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE), 0x100} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 8,
    },
  },
  {
    .szName    = "scc_qupv3_se5_clk_src",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCQUPV3SE5,
    .pDebugMux = NULL,
    .nUniqueId = 0x321E9F0F,
    .nFlags    = CLOCK_FLAG_SUPPRESSIBLE,
    .HALClock  = {
      .mRegisters   = { 0, HWIO_ADDR(SSC_SCC_QUPV3_SE5_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCVSVDDCX                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_vs_vddcx_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCVSVDDCX,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x972C3E38,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_VS_VDDCX_CBCR), HWIO_ADDR(SSC_SCC_VS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 27,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSAONCC_SCCVSVDDMX                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "scc_vs_vddmx_clk",
    .pDomain   = &ClockDomain_LPASSAONCC_SCCVSVDDMX,
    .pDebugMux = &ClockDebugMux_LPASSAONCC,
    .nUniqueId = 0x6D1774AA,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(SSC_SCC_VS_VDDMX_CBCR), HWIO_ADDR(SSC_SCC_VS_BCR), {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 26,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_ATIME                                         */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_avsync_atime_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_ATIME,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xAC904D02,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AVSYNC_ATIME_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 47,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_AUDSLIMBUS                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_aud_slimbus_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_AUDSLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x88ACE38A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 50,
    },
  },
  {
    .szName    = "lpass_audio_core_aud_slimbus_npl_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_AUDSLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x4F4B9BAE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_NPL_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 88,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_CORE                                          */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_aud_slimbus_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x9E5BB985,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 54,
    },
  },
  {
    .szName    = "lpass_audio_core_bus_timeout_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x3EB41E7A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 86,
    },
  },
  {
    .szName    = "lpass_audio_core_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x404465B2,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 52,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xDF08115A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 87,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem0_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x6C551815,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM0_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 56,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem1_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x6A061C50,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM1_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 57,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem2_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x5C996337,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM2_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 58,
    },
  },
  {
    .szName    = "lpass_audio_core_lpm_mem3_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xA3C41D97,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPM_MEM3_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 59,
    },
  },
  {
    .szName    = "lpass_audio_core_qca_slimbus_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x226AA016,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 55,
    },
  },
  {
    .szName    = "lpass_audio_core_sysnoc_mport_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xDCB01079,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 53,
    },
  },
  {
    .szName    = "lpass_audio_core_sysnoc_sway_core_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_CORE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x66BC6624,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 63,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_EXTMCLK0                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_ext_mclk0_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_EXTMCLK0,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x34CAAB2D,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_EXT_MCLK0_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 70,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_EXTMCLK1                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_ext_mclk1_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_EXTMCLK1,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x0C4FE622,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_EXT_MCLK1_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 71,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_EXTMCLK2                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_ext_mclk2_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_EXTMCLK2,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x73BFB4D7,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_EXT_MCLK2_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 72,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTI2S01                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "audio_core_int_i2s0_1_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTI2S01,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x261A6B4A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_I2S0_1_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 91,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTI2S2                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "audio_core_int_i2s2_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTI2S2,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xB3E11D2A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_I2S2_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 92,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTI2S3                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "audio_core_int_i2s3_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTI2S3,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xF1C5C85A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_I2S3_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 93,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTI2S4                                       */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "audio_core_int_i2s4_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTI2S4,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x5EBAF001,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_I2S4_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 97,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTI2S56                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "audio_core_int_i2s5_6_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTI2S56,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xEE1CF73A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_I2S5_6_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 94,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTMCLK0                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_int_mclk0_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTMCLK0,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x705FD80C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_MCLK0_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 95,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_INTMCLK1                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_int_mclk1_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_INTMCLK1,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x8B8A19B6,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_INT_MCLK1_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 96,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFPCMOE                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_pcm_data_oe_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFPCMOE,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x074246CE,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_PCM_DATA_OE_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 46,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFPRI                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_pri_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFPRI,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x4E01A16C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_PRI_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 39,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFQUAD                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_quad_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFQUAD,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x50A2077C,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_QUAD_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 45,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFSEC                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_sec_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFSEC,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x227EFE9E,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_SEC_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 41,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFSPKR                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_codec_spkr_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFSPKR,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x402D483B,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 37,
    },
  },
  {
    .szName    = "lpass_audio_core_lpaif_codec_spkr_osr_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFSPKR,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xBA78B3FD,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_CODEC_SPKR_OSR_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 36,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPAIFTER                                      */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_lpaif_ter_ibit_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_LPAIFTER,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xAC022703,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_LPAIF_TER_IBIT_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 43,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX                  */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "lpass_lpaaudio_dig_pll_ref_clk_mux",
    .pDomain   = &ClockDomain_LPASSCORECC_LPASSLPAAUDIODIGPLLREFCLKMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0x3F67ABA1,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_LPASSLPAAUDIOPLLREFCLKMUX                     */
  /*-----------------------------------------------------------------------*/

  /*
   * Fake clock added with domain name since there are no actual branches
   * on the domain. Used to control frequency.
   */
  {
    .szName    = "lpass_lpaaudio_pll_ref_clk_mux",
    .pDomain   = &ClockDomain_LPASSCORECC_LPASSLPAAUDIOPLLREFCLKMUX,
    .pDebugMux = NULL,
    .nUniqueId = 0x03E405A5,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { 0, 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = HAL_CLK_NO_DEBUG_MUX_SEL,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_QCASLIMBUS                                    */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_qca_slimbus_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_QCASLIMBUS,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0x7DC4180A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_QCA_SLIMBUS_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 51,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_RESAMPLER                                     */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_resampler_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_RESAMPLER,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xB4C76C2A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_RESAMPLER_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 49,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_SWNPL                                         */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_sw_npl_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_SWNPL,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xE05C0E5A,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SW_NPL_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 90,
    },
  },

  /*-----------------------------------------------------------------------*/
  /* ClockDomain_LPASSCORECC_XO                                            */
  /*-----------------------------------------------------------------------*/
  {
    .szName    = "lpass_audio_core_avsync_stc_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_XO,
    .pDebugMux = &ClockDebugMux_LPASSCORECC,
    .nUniqueId = 0xA500D593,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_AVSYNC_STC_CBCR), 0, {0, 0} },
      .pmControl    = &HAL_clk_GenericClockControl,
      .nDebugMuxSel = 48,
    },
  },
  {
    .szName    = "lpass_audio_core_sampling_clk",
    .pDomain   = &ClockDomain_LPASSCORECC_XO,
    .pDebugMux = NULL,
    .nUniqueId = 0x7200F8FA,
    .nFlags    = 0x0,
    .HALClock  = {
      .mRegisters   = { HWIO_ADDR(LPASS_AUDIO_CORE_SAMPLING_CBCR), 0, {0, 0} },
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
      .nGDSCRAddr   = HWIO_ADDR(SSC_SCC_AUDIO_CORE_GDSCR),
      .pmControl    = &HAL_clk_AudioCorePowerDomainControl,
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

