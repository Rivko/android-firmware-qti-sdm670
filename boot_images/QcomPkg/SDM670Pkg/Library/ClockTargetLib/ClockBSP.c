/**
==============================================================================

  @file ClockBSP.c

  Definitions of the support clock perf level.

  Copyright (c) 2016-2017 QUALCOMM Technologies, Inc.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential.

==============================================================================


==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/ClockTargetLib/ClockBSP.c#5 $

  when      who     what, where, why
  --------  ------  ------------------------------------------------------
  06/30/17     vg   Support QUP V3 clock set frequency for 19.2Mhz
  02/08/17  vphan   Support QUP V3 clock set frequency
  11/11/16  vphan   Initial SDM845 revision, branched from MSM8998.

==============================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"
#include "ClockDriver.h"
#include "ClockHWIO.h"

/*=========================================================================
      Prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/
Clock_RailwayType ClockRailway =
{
  "vddcx",
  0,
  0,
  "vddmx",
  0,
  0
};

Clock_ConfigType Clock_ConfigData =
{
  /* GPLL0 @ 600 MHz (general purpose PLL). */
  .PLL0_Cfg =
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
  },
 
  /* GPLL1 @ 1066 MHz. */
  .PLL1_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL1_MODE),
    .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
    .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL1),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  55,
    .nCalibrationL =  0,
    .nFracVal      =  0x8555,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

  /* GPLL2 @ 400 MHz (dynamic config--dedicated to BIMC). */
  .PLL2_Cfg =
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
  },

  /* GPLL3 @ 933 MHz (dynamic config--dedicated to BIMC). */
  .PLL3_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL3_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE), 
    .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL3),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  48,
    .nCalibrationL =  0,
    .nFracVal      =  0x9800,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

  /* GPLL4 @ 806 MHz */
  .PLL4_Cfg =
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
  },

  /* GPLL5 @ 933 MHz */
  .PLL5_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL5_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_SPARE_GPLL_ENA_VOTE),
    .nVoteMask     =  HWIO_FMSK(GCC_SPARE_GPLL_ENA_VOTE, GPLL5),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  48,
    .nCalibrationL =  0,
    .nFracVal      =  0x9800,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

  /* GPLL6 @ 384 MHz */
  .PLL6_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(GCC_GPLL6_MODE), 
    .nVoteAddr     =  HWIO_ADDR(GCC_APCS_GPLL_ENA_VOTE),
    .nVoteMask     =  HWIO_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL6),
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  0,
    .nL            =  20,
    .nCalibrationL =  0,
    .nFracVal      =  0,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

  .AOSS_PLL0_Cfg =
  {
    .bConfig       = FALSE,
    .nRefCount     = 0,
    .nPLLModeAddr  =  HWIO_ADDR(AOSS_CC_PLL0_MODE), 
    .nVoteAddr     =  0,
    .nVoteMask     =  0,
    .nCalibCtrl    =  2, /* 2 = Automatic */
    .nPreDiv       =  1,
    .nPostDivOdd   =  0,
    .nPostDivEven  =  2,
    .nL            =  20,
    .nCalibrationL =  0,
    .nFracVal      =  0xD555,
    .nFracFormat   =  0, /* 0 = Alpha */
    .nConfigCtl    =  FABIA_CONFIG_CTL_VAL,
    .nTestCtl      =  FABIA_PLL_TEST_CTL_VAL,
  },

  .CPU_Cfg =
  {
    /* ClockAPCSSourceType, nLVal, nDiv2x, nReserved, nFreqKHz */
    {APCS_SOURCE_CXO,         0,     0,        0,       19200  },  /* PERF NONE - 19.2MHz    */
    {APCS_SOURCE_AG_EARLY,   79,     0,        0,      1516800 },  /* NOM       - 1516.8MHz  */
    {APCS_SOURCE_AG_EARLY,   89,     0,        0,      1708800 },  /* MAX       - 1708.8MHz  */
    {APCS_SOURCE_AG_EARLY,   79,     0,        0,      1516800 },  /* DEFAULT   - 1516.8MHz  */
  },
  .L3_Cfg =
  {
    /* ClockAPCSSourceType, nLVal, nDiv2x, nReserved, nFreqKHz */
    {APCS_SOURCE_CXO,           0,   0,        0,        19200},  /* PERF NONE - 19.2MHz    */
    {APCS_SOURCE_AG_EARLY,     46,   0,        0,       883200},  /* NOM       - 883.2MHz  */
    {APCS_SOURCE_AG_EARLY,     75,   0,        0,      1440000},  /* MAX       - 1440 MHz  */
    {APCS_SOURCE_AG_EARLY,     46,   0,        0,       883200},  /* DEFAULT   - 883.2MHz  */
  },
   .SNOC_Cfg = 
  {
    {HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  6, 0, 0, 0},  /* NOM - 200 MHz (NOM)         */
    {HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR), 6, MUX_GCC, SRC_GPLL0,  5, 0, 0, 0},  /* MAX - 240 MHz (TURBO)       */
    {HWIO_ADDR(GCC_SYS_NOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  6, 0, 0, 0}   /* DEFAULT -200 MHz (Max Nom)  */
  },
  .SNOC_GC_Cfg = 
  {
    {HWIO_ADDR(GCC_SYS_NOC_GC_AXI_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_SYS_NOC_GC_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  4, 0, 0, 0},  /* NOM - 300 MHz (NOM)        */
    {HWIO_ADDR(GCC_SYS_NOC_GC_AXI_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  6, 0, 0, 0},  /* MAX - 355.33 MHz (TURBO)   */
    {HWIO_ADDR(GCC_SYS_NOC_GC_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  4, 0, 0, 0}   /* DEFAULT -300 MHz (Max Nom) */
  },
  .SNOC_SF_Cfg = 
  {
    {HWIO_ADDR(GCC_SYS_NOC_SF_AXI_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_SYS_NOC_SF_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0},  /* NOM - 403 MHz (NOM)        */
    {HWIO_ADDR(GCC_SYS_NOC_SF_AXI_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  4, 0, 0, 0},  /* MAX - 533 MHz (TURBO)      */
    {HWIO_ADDR(GCC_SYS_NOC_SF_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0}   /* DEFAULT -403 MHz (Max Nom) */
  },
  .AGNOC_Cfg = 
  {
    {HWIO_ADDR(GCC_AGGRE_NOC_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_AGGRE_NOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  6, 0, 0, 0},  /* NOM - 200 MHz (NOM)         */
    {HWIO_ADDR(GCC_AGGRE_NOC_CMD_RCGR), 6, MUX_GCC, SRC_GPLL0,  5, 0, 0, 0},  /* MAX - 240 MHz (TURBO)       */
    {HWIO_ADDR(GCC_AGGRE_NOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  6, 0, 0, 0}   /* DEFAULT - 200 MHz (Max Nom) */
  },
  .AGNOC_EAST_Cfg = 
  {
    {HWIO_ADDR(GCC_AGGRE_NOC_EAST_SF_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_AGGRE_NOC_EAST_SF_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  3, 0, 0, 0},  /* NOM - 400 MHz (NOM)         */
    {HWIO_ADDR(GCC_AGGRE_NOC_EAST_SF_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  4, 0, 0, 0},  /* MAX - 533 MHz (TURBO)       */
    {HWIO_ADDR(GCC_AGGRE_NOC_EAST_SF_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  3, 0, 0, 0}   /* DEFAULT - 400 MHz (Max Nom) */
  },
  .AGNOC_NORTH_Cfg = 
  {
    {HWIO_ADDR(GCC_AGGRE_NOC_NORTH_SF_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE */
    {HWIO_ADDR(GCC_AGGRE_NOC_NORTH_SF_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  3, 0, 0, 0},  /* NOM - 400 MHz (NOM)         */
    {HWIO_ADDR(GCC_AGGRE_NOC_NORTH_SF_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  4, 0, 0, 0},  /* MAX - 533 MHz (TURBO)       */
    {HWIO_ADDR(GCC_AGGRE_NOC_NORTH_SF_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  3, 0, 0, 0}   /* DEFAULT - 533 MHz (Max Nom) */
  },
  .CNOC_Cfg = 
  {
    {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                    */
    {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0},  /* NOM - 75 MHz (NOM)           */
    {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0},  /* MAX - 75 MHz (TURBO)         */
    {HWIO_ADDR(GCC_CONFIG_NOC_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 16, 0, 0, 0}   /* DEFAULT - 75 MHz (Max Nom)   */
  },
  .CNOC_Periph_Cfg =
  {
    {HWIO_ADDR(GCC_CNOC_PERIPH_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0}, /* PERF NONE                     */
    {HWIO_ADDR(GCC_CNOC_PERIPH_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}, /* NOM - 100 MHz (NOM)           */
    {HWIO_ADDR(GCC_CNOC_PERIPH_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}, /* MAX - 100 MHz (TURBO)         */
    {HWIO_ADDR(GCC_CNOC_PERIPH_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}  /* DEFAULT - 100 MHz (Max Nom)   */
  },
  .CPU_AXI_Cfg = 
  {
    {HWIO_ADDR(GCC_CPUSS_AXI_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                     */
    {HWIO_ADDR(GCC_CPUSS_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  0, 0, 0, 0},  /* NOM - 806MHz (NOM)            */
    {HWIO_ADDR(GCC_CPUSS_AXI_CMD_RCGR), 6, MUX_GCC, SRC_GPLL5,  0, 0, 0, 0},  /* MAX - 933MHz (TURBO)          */
    {HWIO_ADDR(GCC_CPUSS_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  0, 0, 0, 0}   /* DEFAULT - 806MHz (Max Nom)    */
  },
  .TURING_AXI_Cfg = 
  {
    {HWIO_ADDR(GCC_TURING_AXI_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                     */
    {HWIO_ADDR(GCC_TURING_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0},  /* NOM - 403MHz (NOM)            */
    {HWIO_ADDR(GCC_TURING_AXI_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  4, 0, 0, 0},  /* MAX - 533MHz (TURBO)          */
    {HWIO_ADDR(GCC_TURING_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0}   /* DEFAULT - 403MHz (Max Nom)    */
  },
  .MEMNOC_Cfg =
  {
    {HWIO_ADDR(GCC_MEMNOC_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                         */
    {HWIO_ADDR(GCC_MEMNOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  0, 0, 0, 0},  /* NOM - 806MHz (NOM)                */
    {HWIO_ADDR(GCC_MEMNOC_CMD_RCGR), 6, MUX_GCC, SRC_GPLL5,  0, 0, 0, 0},  /* MAX - 933MHz (TURBO)              */
    {HWIO_ADDR(GCC_MEMNOC_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  0, 0, 0, 0},  /* DEFAULT - 806MHz (NOM)            */
  },
  .GPU_MEMNOC_Cfg =
  {
    {HWIO_ADDR(GCC_GPU_MEMNOC_GFX_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                 */
    {HWIO_ADDR(GCC_GPU_MEMNOC_GFX_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0},  /* NOM - 403MHz (NOM)        */
    {HWIO_ADDR(GCC_GPU_MEMNOC_GFX_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  4, 0, 0, 0},  /* MAX - 533MHz (TURBO)      */
    {HWIO_ADDR(GCC_GPU_MEMNOC_GFX_CMD_RCGR), 5, MUX_GCC, SRC_GPLL4,  4, 0, 0, 0},  /* DEFAULT - 403MHz (NOM)    */
  },
  .Q6_MEMNOC_Cfg =
  {
    {HWIO_ADDR(GCC_MSS_Q6_MEMNOC_AXI_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE               */
    {HWIO_ADDR(GCC_MSS_Q6_MEMNOC_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  0, 0, 0, 0},  /* MAX - 600MHz (NOM)      */
    {HWIO_ADDR(GCC_MSS_Q6_MEMNOC_AXI_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1,  3, 0, 0, 0},  /* NOM - 710.67MHz (TURBO) */
    {HWIO_ADDR(GCC_MSS_Q6_MEMNOC_AXI_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0,  0, 0, 0, 0},  /* DEFAULT - 600MHz (NOM)  */
  },
  .IPA_Cfg =
  {
    {HWIO_ADDR(GCC_IPA_2X_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE               */
    {HWIO_ADDR(GCC_IPA_2X_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0,  4, 0, 0, 0},  /* NOM - 300MHz (NOM)      */
    {HWIO_ADDR(GCC_IPA_2X_CMD_RCGR), 5, MUX_GCC, SRC_GPLL1,  6, 0, 0, 0},  /* MAX - 355.33MHz (TURBO) */
    {HWIO_ADDR(GCC_IPA_2X_CMD_RCGR), 4, MUX_GCC, SRC_GPLL0,  4, 0, 0, 0},  /* DEFAULT - 300MHz (NOM)  */
  },
  .CE_Cfg =
  {
    {HWIO_ADDR(GCC_CE1_CMD_RCGR), 1, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                */
    {HWIO_ADDR(GCC_CE1_CMD_RCGR), 3, MUX_GCC, SRC_GPLL0,  7, 0, 0, 0},  /* NOM - 171.43MHz (NOM)    */
    {HWIO_ADDR(GCC_CE1_CMD_RCGR), 3, MUX_GCC, SRC_GPLL0,  7, 0, 0, 0},  /* MAX - 171.43MHz (TURBO)  */
    {HWIO_ADDR(GCC_CE1_CMD_RCGR), 3, MUX_GCC, SRC_GPLL0,  7, 0, 0, 0},  /* DEFAULT - 171.43MHz (NOM)*/
  },
  .UFS_Cfg = 
  {
    {HWIO_ADDR(GCC_UFS_PHY_AXI_CMD_RCGR), 0, MUX_GCC, SRC_CXO,   0,  0, 0, 0}, /* PERF NONE         */
    {HWIO_ADDR(GCC_UFS_PHY_AXI_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 6,  0, 0, 0}, /* NOM - 200 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_AXI_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 5,  0, 0, 0}, /* MAX - 240 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_AXI_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 6,  0, 0, 0}, /* DEFAULT - 200 MHz */
  },
  .UFS_Ice_Cfg = 
  {
    {HWIO_ADDR(GCC_UFS_PHY_ICE_CORE_CMD_RCGR), 0, MUX_GCC, SRC_CXO,   0,  0, 0, 0}, /* PERF NONE         */
    {HWIO_ADDR(GCC_UFS_PHY_ICE_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 4,  0, 0, 0}, /* NOM - 300 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_ICE_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 4,  0, 0, 0}, /* MAX - 300 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_ICE_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 4,  0, 0, 0}, /* DEFAULT - 300 MHz */
  },
  .UFS_Unipro_Cfg = 
  {
    {HWIO_ADDR(GCC_UFS_PHY_UNIPRO_CORE_CMD_RCGR), 0, MUX_GCC, SRC_CXO,   0,  0, 0, 0}, /* PERF NONE         */
    {HWIO_ADDR(GCC_UFS_PHY_UNIPRO_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 8,  0, 0, 0}, /* NOM - 150 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_UNIPRO_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 8,  0, 0, 0}, /* MAX - 150 MHz     */
    {HWIO_ADDR(GCC_UFS_PHY_UNIPRO_CORE_CMD_RCGR), 0, MUX_GCC, SRC_GPLL0, 8,  0, 0, 0}, /* DEFAULT - 150 MHz */
  },
  .QUPV3Core2X_Cfg =
  {
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), 0, MUX_GCC, SRC_CXO,    0, 0, 0, 0},  /* PERF NONE                */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0, 6, 0, 0, 0},  /* NOM - 200MHz (NOM)       */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), 6, MUX_GCC, SRC_GPLL1, 9, 0, 0, 0},  /* MAX - 236.89MHz (TURBO)  */
    {HWIO_ADDR(GCC_QUPV3_WRAP0_CORE_2X_CMD_RCGR), 5, MUX_GCC, SRC_GPLL0, 6, 0, 0, 0},  /* DEFAULT - 200MHz (NOM)   */
  },
  .QUPV3_Cfg =
  {
    { 7372800,  {0, 0, MUX_GCC, SRC_GPLL0, 1, 192, 15625, 15625} }, 
    { 14745600, {0, 0, MUX_GCC, SRC_GPLL0, 1, 384, 15625, 15625} },
    { 16000000, {0, 0, MUX_GCC, SRC_GPLL0, 10,  2,    15,    15} },
    { 19200000, {0, 0, MUX_GCC, SRC_CXO,   0,   0,     0,     0} },
    { 24000000, {0, 0, MUX_GCC, SRC_GPLL0, 10,  1,     5,     5} },
    { 32000000, {0, 0, MUX_GCC, SRC_GPLL0, 10,  4,    15,    15} },
    { 96000000, {0, 0, MUX_GCC, SRC_GPLL0,  1,  4,    25,    25} },
  },

  /* SDC configuration : for backwards compatiblity to the old API */
  .SDC_Cfg =
  {
    0,       /* PERF NONE           */
    25000,   /* NOMINAL - 25MHz     */
    50000,   /* MAX - 50MHz         */
    400      /* DEFAULT - At 400KHz */
  },

  /*  SDC1 extended configurations (As of now using SDC2_Ext_Cfg, update this after sw clk plan updated in ipcatlog)*/
  .SDC1_Ext_Cfg =
  {

    {   400,  {0, 0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 1, 2, 2}},
    { 50000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 0, 0, 0}},
    {100000,  {0, 0, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}},
    {192000,  {0, 0, MUX_GCC, SRC_GPLL6,  4, 0, 0, 0}},
    {384000,  {0, 0, MUX_GCC, SRC_GPLL6,  2, 0, 0, 0}},
    {0,  {0,}},
  },
  /*  SDC2 extended configurations */
  .SDC2_Ext_Cfg =
  {
    {   400,  {0, 0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 1, 2, 2}},
    { 50000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 0, 0, 0}},
    {100000,  {0, 0, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}},
    {201500,  {0, 0, MUX_GCC, SRC_GPLL4,  8, 0, 0, 0}},
    {0,  {0,}},
  },
  /*  SDC4 extended configurations */
  .SDC4_Ext_Cfg =
  {
    {   400,  {0, 0, MUX_GCC, SRC_CXO,   24, 1, 4, 4}},
    { 25000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 1, 2, 2}},
    { 50000,  {0, 0, MUX_GCC, SRC_GPLL0, 24, 0, 0, 0}},
    {100000,  {0, 0, MUX_GCC, SRC_GPLL0, 12, 0, 0, 0}},
    {0,  {0,}},
  },

  /* Mapping the clock Driver Voltage corner ENUMs to Railway Driver Voltage Corner ENUMs */
  .aClockMapCorner[CLOCK_BOOT_PERF_NONE]    = RAILWAY_NO_REQUEST,
  .aClockMapCorner[CLOCK_BOOT_PERF_NOMINAL] = RAILWAY_NOMINAL,
  .aClockMapCorner[CLOCK_BOOT_PERF_TURBO]   = RAILWAY_TURBO,
  .aClockMapCorner[CLOCK_BOOT_PERF_DEFAULT] = RAILWAY_NOMINAL,
};