#ifndef __HALCLKAGERAPLL_H__
#define __HALCLKAGERAPLL_H__
/*
==============================================================================

FILE:         HALclkAgeraPLL.h

DESCRIPTION:
  Internal, chipset specific PLL definitions for the clock HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/HALclkSharedLib/HALclkAgeraPLL.h#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/22/16   sr      Created.

==============================================================================
    Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALclkInternal.h"



/* ============================================================================
**    Definitions
** ==========================================================================*/


/*
 * PLL register offsets.
 */
#define HWIO_PLL_MODE_OFFS                                  0x00000000
#define HWIO_PLL_L_VAL_OFFS                                 0x00000004
#define HWIO_PLL_ALPHA_VAL_OFFS                             0x00000008
#define HWIO_PLL_USER_CTL_OFFS                              0x0000000c
#define HWIO_PLL_CONFIG_CTL_LO_OFFS                         0x00000010
#define HWIO_PLL_CONFIG_CTL_HI_OFFS                         0x00000014
#define HWIO_PLL_TEST_CTL_LO_OFFS                           0x00000018
#define HWIO_PLL_TEST_CTL_HI_OFFS                           0x0000001c
#define HWIO_PLL_SSC_DELTA_ALPHA_OFFS                       0x00000020
#define HWIO_PLL_SSC_UPDATE_RATE_OFFS                       0x00000024
#define HWIO_PLL_STATUS_OFFS                                0x0000002c
#define HWIO_PLL_SECURE_OFFS                                0x00000040


/*
 * PLL_MODE register definitions.
 */
#define HWIO_PLL_MODE_LOCK_DET_BMSK                         0x80000000
#define HWIO_PLL_MODE_LOCK_DET_SHFT                               0x1f
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                  0x40000000
#define HWIO_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                        0x1e
#define HWIO_PLL_MODE_FSM_RES_BMSK                            0x200000
#define HWIO_PLL_MODE_FSM_RES_SHFT                                0x15
#define HWIO_PLL_MODE_FSM_EN_BMSK                             0x100000
#define HWIO_PLL_MODE_FSM_EN_SHFT                                 0x14
#define HWIO_PLL_MODE_BIAS_COUNT_BMSK                          0xfc000
#define HWIO_PLL_MODE_BIAS_COUNT_SHFT                              0xe
#define HWIO_PLL_MODE_LOCK_COUNT_BMSK                           0x3f00
#define HWIO_PLL_MODE_LOCK_COUNT_SHFT                              0x8
#define HWIO_PLL_MODE_PLLTEST_BMSK                                 0x8
#define HWIO_PLL_MODE_PLLTEST_SHFT                                 0x3
#define HWIO_PLL_MODE_RESET_N_BMSK                                 0x4
#define HWIO_PLL_MODE_RESET_N_SHFT                                 0x2
#define HWIO_PLL_MODE_BYPASSNL_BMSK                                0x2
#define HWIO_PLL_MODE_BYPASSNL_SHFT                                0x1
#define HWIO_PLL_MODE_OUTCTRL_BMSK                                 0x1
#define HWIO_PLL_MODE_OUTCTRL_SHFT                                 0x0


/*
 * PLL_USER_CTL register definitions.
 */
#define HWIO_PLL_USER_CTL_ALPHA_MODE_BMSK                    0x2000000
#define HWIO_PLL_USER_CTL_ALPHA_MODE_SHFT                         0x19
#define HWIO_PLL_USER_CTL_MN_ACC_BMSK                        0x1000000
#define HWIO_PLL_USER_CTL_MN_ACC_SHFT                             0x18
#define HWIO_PLL_USER_CTL_CLK_REF_PRE_DIV_BMSK                  0x1000
#define HWIO_PLL_USER_CTL_CLK_REF_PRE_DIV_SHFT                     0xc
#define HWIO_PLL_USER_CTL_CLK_POST_DIV_BMSK                      0x300
#define HWIO_PLL_USER_CTL_CLK_POST_DIV_SHFT                        0x8
#define HWIO_PLL_USER_CTL_OUT_INV_EN_BMSK                         0x80
#define HWIO_PLL_USER_CTL_OUT_INV_EN_SHFT                          0x7
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_EN_BMSK                  0x10
#define HWIO_PLL_USER_CTL_PLLOUT_LV_TEST_EN_SHFT                   0x4
#define HWIO_PLL_USER_CTL_LVEARLY_EN_BMSK                          0x8
#define HWIO_PLL_USER_CTL_LVEARLY_EN_SHFT                          0x3
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_EN_BMSK                   0x4
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX2_EN_SHFT                   0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_EN_BMSK                    0x2
#define HWIO_PLL_USER_CTL_PLLOUT_LV_AUX_EN_SHFT                    0x1
#define HWIO_PLL_USER_CTL_LVMAIN_EN_BMSK                           0x1
#define HWIO_PLL_USER_CTL_LVMAIN_EN_SHFT                           0x0


/*
 * PLL_CONFIG_CTL_LO register definitions.
 */
#define HWIO_PLL_CONFIG_CTL_LO_INJMODECTL_BMSK              0xc0000000
#define HWIO_PLL_CONFIG_CTL_LO_INJMODECTL_SHFT                    0x1e
#define HWIO_PLL_CONFIG_CTL_LO_WINCTL_BMSK                  0x30000000
#define HWIO_PLL_CONFIG_CTL_LO_WINCTL_SHFT                        0x1c
#define HWIO_PLL_CONFIG_CTL_LO_LOCKDET_BMSK                  0xc000000
#define HWIO_PLL_CONFIG_CTL_LO_LOCKDET_SHFT                       0x1a
#define HWIO_PLL_CONFIG_CTL_LO_VCO_TRTL_BMSK                   0x60000
#define HWIO_PLL_CONFIG_CTL_LO_VCO_TRTL_SHFT                      0x11
#define HWIO_PLL_CONFIG_CTL_LO_INJSEL_BMSK                     0x18000
#define HWIO_PLL_CONFIG_CTL_LO_INJSEL_SHFT                         0xf
#define HWIO_PLL_CONFIG_CTL_LO_ISEED_GATE_BMSK                  0x4000
#define HWIO_PLL_CONFIG_CTL_LO_ISEED_GATE_SHFT                     0xe
#define HWIO_PLL_CONFIG_CTL_LO_VCO_DCAPS_BMSK                   0x3000
#define HWIO_PLL_CONFIG_CTL_LO_VCO_DCAPS_SHFT                      0xc
#define HWIO_PLL_CONFIG_CTL_LO_SEL_IREG_BMSK                     0xc00
#define HWIO_PLL_CONFIG_CTL_LO_SEL_IREG_SHFT                       0xa
#define HWIO_PLL_CONFIG_CTL_LO_DROOP_OFFSET_BMSK                 0x300
#define HWIO_PLL_CONFIG_CTL_LO_DROOP_OFFSET_SHFT                   0x8
#define HWIO_PLL_CONFIG_CTL_LO_ANTI_DEADZONE_PRG_IN_BMSK          0xc0
#define HWIO_PLL_CONFIG_CTL_LO_ANTI_DEADZONE_PRG_IN_SHFT           0x6
#define HWIO_PLL_CONFIG_CTL_LO_ISEED_CTL_BMSK                     0x30
#define HWIO_PLL_CONFIG_CTL_LO_ISEED_CTL_SHFT                      0x4
#define HWIO_PLL_CONFIG_CTL_LO_CURR_DIV_BMSK                       0xc
#define HWIO_PLL_CONFIG_CTL_LO_CURR_DIV_SHFT                       0x2
#define HWIO_PLL_CONFIG_CTL_LO_DLY_BMSK                            0x3
#define HWIO_PLL_CONFIG_CTL_LO_DLY_SHFT                            0x0


/*
 * PLL_CONFIG_CTL_HI register definitions.
 */
#define HWIO_PLL_CONFIG_CTL_HI_INJMODECTL_BMSK              0xc0000000
#define HWIO_PLL_CONFIG_CTL_HI_INJMODECTL_SHFT                    0x1e
#define HWIO_PLL_CONFIG_CTL_HI_WINCTL_BMSK                  0x30000000
#define HWIO_PLL_CONFIG_CTL_HI_WINCTL_SHFT                        0x1c
#define HWIO_PLL_CONFIG_CTL_HI_LOCKDET_BMSK                  0xc000000
#define HWIO_PLL_CONFIG_CTL_HI_LOCKDET_SHFT                       0x1a
#define HWIO_PLL_CONFIG_CTL_HI_VCO_TRTL_BMSK                   0x60000
#define HWIO_PLL_CONFIG_CTL_HI_VCO_TRTL_SHFT                      0x11
#define HWIO_PLL_CONFIG_CTL_HI_INJSEL_BMSK                     0x18000
#define HWIO_PLL_CONFIG_CTL_HI_INJSEL_SHFT                         0xf
#define HWIO_PLL_CONFIG_CTL_HI_ISEED_GATE_BMSK                  0x4000
#define HWIO_PLL_CONFIG_CTL_HI_ISEED_GATE_SHFT                     0xe
#define HWIO_PLL_CONFIG_CTL_HI_VCO_DCAPS_BMSK                   0x3000
#define HWIO_PLL_CONFIG_CTL_HI_VCO_DCAPS_SHFT                      0xc
#define HWIO_PLL_CONFIG_CTL_HI_SEL_IREG_BMSK                     0xc00
#define HWIO_PLL_CONFIG_CTL_HI_SEL_IREG_SHFT                       0xa
#define HWIO_PLL_CONFIG_CTL_HI_DROOP_OFFSET_BMSK                 0x300
#define HWIO_PLL_CONFIG_CTL_HI_DROOP_OFFSET_SHFT                   0x8
#define HWIO_PLL_CONFIG_CTL_HI_ANTI_DEADZONE_PRG_IN_BMSK          0xc0
#define HWIO_PLL_CONFIG_CTL_HI_ANTI_DEADZONE_PRG_IN_SHFT           0x6
#define HWIO_PLL_CONFIG_CTL_HI_ISEED_CTL_BMSK                     0x30
#define HWIO_PLL_CONFIG_CTL_HI_ISEED_CTL_SHFT                      0x4
#define HWIO_PLL_CONFIG_CTL_HI_CURR_DIV_BMSK                       0xc
#define HWIO_PLL_CONFIG_CTL_HI_CURR_DIV_SHFT                       0x2
#define HWIO_PLL_CONFIG_CTL_HI_DLY_BMSK                            0x3
#define HWIO_PLL_CONFIG_CTL_HI_DLY_SHFT                            0x0


/*
 * PLL_TEST_CTL_LO register definitions.
 */
#define HWIO_PLL_TEST_CTL_LO_OV_VLT_EN_BMSK                 0x80000000
#define HWIO_PLL_TEST_CTL_LO_OV_VLT_EN_SHFT                       0x1f
#define HWIO_PLL_TEST_CTL_LO_PUL_WIDTH_CTL_BMSK             0x40000000
#define HWIO_PLL_TEST_CTL_LO_PUL_WIDTH_CTL_SHFT                   0x1e
#define HWIO_PLL_TEST_CTL_LO_EN_GATE_VCO_BMSK               0x20000000
#define HWIO_PLL_TEST_CTL_LO_EN_GATE_VCO_SHFT                     0x1d
#define HWIO_PLL_TEST_CTL_LO_EN_SYNC_BMSK                   0x10000000
#define HWIO_PLL_TEST_CTL_LO_EN_SYNC_SHFT                         0x1c
#define HWIO_PLL_TEST_CTL_LO_OV_VLT_CNT_BMSK                 0xc000000
#define HWIO_PLL_TEST_CTL_LO_OV_VLT_CNT_SHFT                      0x1a
#define HWIO_PLL_TEST_CTL_LO_BYP_LSB_BMSK                    0x2000000
#define HWIO_PLL_TEST_CTL_LO_BYP_LSB_SHFT                         0x19
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_CTL_BMSK             0x1800000
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_CTL_SHFT                  0x17
#define HWIO_PLL_TEST_CTL_LO_NOISE_GEN_CFG_BMSK               0x700000
#define HWIO_PLL_TEST_CTL_LO_NOISE_GEN_CFG_SHFT                   0x14
#define HWIO_PLL_TEST_CTL_LO_NOISE_GEN_EN_BMSK                 0x80000
#define HWIO_PLL_TEST_CTL_LO_NOISE_GEN_EN_SHFT                    0x13
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_OSC_CTL_BMSK           0x60000
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_OSC_CTL_SHFT              0x11
#define HWIO_PLL_TEST_CTL_LO_PLLOUT_LV_TEST_BMSK                0x1800
#define HWIO_PLL_TEST_CTL_LO_PLLOUT_LV_TEST_SHFT                   0xf
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_OSC_EN_BMSK              0x400
#define HWIO_PLL_TEST_CTL_LO_NOISE_MEAS_OSC_EN_SHFT                0xe
#define HWIO_PLL_TEST_CTL_LO_FILT_INT_BMSK                       0x200
#define HWIO_PLL_TEST_CTL_LO_FILT_INT_SHFT                         0xd
#define HWIO_PLL_TEST_CTL_LO_DTEST_BMSK                          0x100
#define HWIO_PLL_TEST_CTL_LO_DTEST_SHFT                            0xc
#define HWIO_PLL_TEST_CTL_LO_PLLOUT_HV_EN_BMSK                    0x80
#define HWIO_PLL_TEST_CTL_LO_PLLOUT_HV_EN_SHFT                     0x7
#define HWIO_PLL_TEST_CTL_LO_VDDA_ATEST0_BMSK                     0x40
#define HWIO_PLL_TEST_CTL_LO_VDDA_ATEST0_SHFT                      0x6
#define HWIO_PLL_TEST_CTL_LO_AVDD_ATEST1_BMSK                     0x20
#define HWIO_PLL_TEST_CTL_LO_AVDD_ATEST1_SHFT                      0x5
#define HWIO_PLL_TEST_CTL_LO_VDDA_ATEST1_BMSK                     0x10
#define HWIO_PLL_TEST_CTL_LO_VDDA_ATEST1_SHFT                      0x4
#define HWIO_PLL_TEST_CTL_LO_FILT_OPAMP_EN_BMSK                    0x8
#define HWIO_PLL_TEST_CTL_LO_FILT_OPAMP_EN_SHFT                    0x3
#define HWIO_PLL_TEST_CTL_LO_EN_ATEST0_BMSK                        0x4
#define HWIO_PLL_TEST_CTL_LO_EN_ATEST0_SHFT                        0x2
#define HWIO_PLL_TEST_CTL_LO_VREG_SLAVE_BMSK                       0x2
#define HWIO_PLL_TEST_CTL_LO_VREG_SLAVE_SHFT                       0x1
#define HWIO_PLL_TEST_CTL_LO_VREG_MST_BMSK                         0x1
#define HWIO_PLL_TEST_CTL_LO_VREG_MST_SHFT                         0x0

/*
 * PLL_TEST_CTL_HI register definitions.
 */
#define HWIO_PLL_TEST_CTL_HI_OV_VLT_EN_BMSK                 0x80000000
#define HWIO_PLL_TEST_CTL_HI_OV_VLT_EN_SHFT                       0x1f
#define HWIO_PLL_TEST_CTL_HI_PUL_WIDTH_CTL_BMSK             0x40000000
#define HWIO_PLL_TEST_CTL_HI_PUL_WIDTH_CTL_SHFT                   0x1e
#define HWIO_PLL_TEST_CTL_HI_EN_GATE_VCO_BMSK               0x20000000
#define HWIO_PLL_TEST_CTL_HI_EN_GATE_VCO_SHFT                     0x1d
#define HWIO_PLL_TEST_CTL_HI_EN_SYNC_BMSK                   0x10000000
#define HWIO_PLL_TEST_CTL_HI_EN_SYNC_SHFT                         0x1c
#define HWIO_PLL_TEST_CTL_HI_OV_VLT_CNT_BMSK                 0xc000000
#define HWIO_PLL_TEST_CTL_HI_OV_VLT_CNT_SHFT                      0x1a
#define HWIO_PLL_TEST_CTL_HI_BYP_LSB_BMSK                    0x2000000
#define HWIO_PLL_TEST_CTL_HI_BYP_LSB_SHFT                         0x19
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_CTL_BMSK             0x1800000
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_CTL_SHFT                  0x17
#define HWIO_PLL_TEST_CTL_HI_NOISE_GEN_CFG_BMSK               0x700000
#define HWIO_PLL_TEST_CTL_HI_NOISE_GEN_CFG_SHFT                   0x14
#define HWIO_PLL_TEST_CTL_HI_NOISE_GEN_EN_BMSK                 0x80000
#define HWIO_PLL_TEST_CTL_HI_NOISE_GEN_EN_SHFT                    0x13
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_OSC_CTL_BMSK           0x60000
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_OSC_CTL_SHFT              0x11
#define HWIO_PLL_TEST_CTL_HI_PLLOUT_LV_TEST_BMSK                0x1800
#define HWIO_PLL_TEST_CTL_HI_PLLOUT_LV_TEST_SHFT                   0xf
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_OSC_EN_BMSK              0x400
#define HWIO_PLL_TEST_CTL_HI_NOISE_MEAS_OSC_EN_SHFT                0xe
#define HWIO_PLL_TEST_CTL_HI_FILT_INT_BMSK                       0x200
#define HWIO_PLL_TEST_CTL_HI_FILT_INT_SHFT                         0xd
#define HWIO_PLL_TEST_CTL_HI_DTEST_BMSK                          0x100
#define HWIO_PLL_TEST_CTL_HI_DTEST_SHFT                            0xc
#define HWIO_PLL_TEST_CTL_HI_PLLOUT_HV_EN_BMSK                    0x80
#define HWIO_PLL_TEST_CTL_HI_PLLOUT_HV_EN_SHFT                     0x7
#define HWIO_PLL_TEST_CTL_HI_VDDA_ATEST0_BMSK                     0x40
#define HWIO_PLL_TEST_CTL_HI_VDDA_ATEST0_SHFT                      0x6
#define HWIO_PLL_TEST_CTL_HI_AVDD_ATEST1_BMSK                     0x20
#define HWIO_PLL_TEST_CTL_HI_AVDD_ATEST1_SHFT                      0x5
#define HWIO_PLL_TEST_CTL_HI_VDDA_ATEST1_BMSK                     0x10
#define HWIO_PLL_TEST_CTL_HI_VDDA_ATEST1_SHFT                      0x4
#define HWIO_PLL_TEST_CTL_HI_FILT_OPAMP_EN_BMSK                    0x8
#define HWIO_PLL_TEST_CTL_HI_FILT_OPAMP_EN_SHFT                    0x3
#define HWIO_PLL_TEST_CTL_HI_EN_ATEST0_BMSK                        0x4
#define HWIO_PLL_TEST_CTL_HI_EN_ATEST0_SHFT                        0x2
#define HWIO_PLL_TEST_CTL_HI_VREG_SLAVE_BMSK                       0x2
#define HWIO_PLL_TEST_CTL_HI_VREG_SLAVE_SHFT                       0x1
#define HWIO_PLL_TEST_CTL_HI_VREG_MST_BMSK                         0x1
#define HWIO_PLL_TEST_CTL_HI_VREG_MST_SHFT                         0x0


/*
 * PLL_STATUS register definitions.
 */
#define HWIO_PLL_STATUS_LOCK_DET_BMSK                          0x10000
#define HWIO_PLL_STATUS_LOCK_DET_SHFT                             0x10
#define HWIO_PLL_STATUS_OV_VLT_CNT_STS_BMSK                       0x3c
#define HWIO_PLL_STATUS_OV_VLT_CNT_STS_SHFT                        0x2
#define HWIO_PLL_STATUS_OV_VLT_DET_BMSK                            0x2
#define HWIO_PLL_STATUS_OV_VLT_DET_SHFT                            0x1
#define HWIO_PLL_STATUS_CLK_N_BMSK                                 0x1
#define HWIO_PLL_STATUS_CLK_N_SHFT                                 0x0


/*
 * PLL_SECURE register definitions.
 */
#define HWIO_PLL_SECURE_DROOP_CTL_BMSK                             0x2
#define HWIO_PLL_SECURE_DROOP_CTL_SHFT                             0x1
#define HWIO_PLL_SECURE_PLL_CTL_BMSK                               0x1
#define HWIO_PLL_SECURE_PLL_CTL_SHFT                               0x0


/*
 * PLL_ALPHA_VAL register definitions.
 */
#define HWIO_PLL_ALPHA_VAL_M_BMSK                               0xff00
#define HWIO_PLL_ALPHA_VAL_M_SHFT                                  0x8
#define HWIO_PLL_ALPHA_VAL_N_BMSK                                 0xff
#define HWIO_PLL_ALPHA_VAL_N_SHFT                                  0x0


/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    External Definitions
** ==========================================================================*/


/* ============================================================================
**    Functions
** ==========================================================================*/

/*
 * Source control functions.  These can be used if some of the control
 * needs to be overridden by a specific PLL.
 */
static boolean HAL_clk_AgeraPLLEnable           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static void    HAL_clk_AgeraPLLDisable          ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceDisableModeType eMode, void *pData );
static boolean HAL_clk_AgeraPLLIsEnabled        ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_AgeraPLLConfig           ( HAL_clk_SourceDescType *pSource, HAL_clk_SourceConfigType eConfig );
static boolean HAL_clk_AgeraPLLConfigPLL        ( HAL_clk_SourceDescType *pSource, const HAL_clk_PLLConfigType *pmConfig, HAL_clk_SourceConfigMode eMode );
static void    HAL_clk_AgeraPLLDetectPLLConfig  ( HAL_clk_SourceDescType *pSource, HAL_clk_PLLConfigType *pmConfig );
static boolean HAL_clk_AgeraPLLEnableVote       ( HAL_clk_SourceDescType *pSource );
static void    HAL_clk_AgeraPLLDisableVote      ( HAL_clk_SourceDescType *pSource );
static boolean HAL_clk_AgeraPLLIsVoteEnabled    ( HAL_clk_SourceDescType *pSource );
static boolean HAL_clk_AgeraPLLIsPLLConfigEqual ( const HAL_clk_PLLConfigType *pConfigA, const HAL_clk_PLLConfigType *pConfigB);

#endif  /* __HALCLKAGERAPLL_H__ */

