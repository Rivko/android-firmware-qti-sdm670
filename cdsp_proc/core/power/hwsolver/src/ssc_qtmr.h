#ifndef __SSC_QTMR__
#define __SSC_QTMR__
/*
===========================================================================
*/
/**
  @file ssc_qtmr
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r41.1.0]
 
  This file contains HWIO register definitions for the following modules:
    SSC_QTMR_F0_0

  'Include' filters applied: 
  'Exclude' filters applied: 
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.qdsp6/2.1.c4/power/hwsolver/src/ssc_qtmr.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "HALhwio.h"
#include "msmhwiobase.h"

#define SSC_SDC_DEC_REG_BASE                                                    (SSC_BASE      + 0x00880000)

#define HWIO_SSC_SDC_HW_VERSION_ADDR                                            (SSC_SDC_DEC_REG_BASE      + 0x00000000)
#define HWIO_SSC_SDC_HW_VERSION_RMSK                                            0xffffffff
#define HWIO_SSC_SDC_HW_VERSION_IN          \
        in_dword_masked(HWIO_SSC_SDC_HW_VERSION_ADDR, HWIO_SSC_SDC_HW_VERSION_RMSK)
#define HWIO_SSC_SDC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_SDC_HW_VERSION_ADDR, m)
#define HWIO_SSC_SDC_HW_VERSION_MAJOR_BMSK                                      0xf0000000
#define HWIO_SSC_SDC_HW_VERSION_MAJOR_SHFT                                            0x1c
#define HWIO_SSC_SDC_HW_VERSION_MINOR_BMSK                                       0xfff0000
#define HWIO_SSC_SDC_HW_VERSION_MINOR_SHFT                                            0x10
#define HWIO_SSC_SDC_HW_VERSION_STEP_BMSK                                           0xffff
#define HWIO_SSC_SDC_HW_VERSION_STEP_SHFT                                              0x0

#define HWIO_SSC_SDC_WFI_CONFIG_ADDR                                            (SSC_SDC_DEC_REG_BASE      + 0x00000004)
#define HWIO_SSC_SDC_WFI_CONFIG_RMSK                                                   0x7
#define HWIO_SSC_SDC_WFI_CONFIG_IN          \
        in_dword_masked(HWIO_SSC_SDC_WFI_CONFIG_ADDR, HWIO_SSC_SDC_WFI_CONFIG_RMSK)
#define HWIO_SSC_SDC_WFI_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SSC_SDC_WFI_CONFIG_ADDR, m)
#define HWIO_SSC_SDC_WFI_CONFIG_OUT(v)      \
        out_dword(HWIO_SSC_SDC_WFI_CONFIG_ADDR,v)
#define HWIO_SSC_SDC_WFI_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SDC_WFI_CONFIG_ADDR,m,v,HWIO_SSC_SDC_WFI_CONFIG_IN)
#define HWIO_SSC_SDC_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_BMSK                               0x4
#define HWIO_SSC_SDC_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_SHFT                               0x2
#define HWIO_SSC_SDC_WFI_CONFIG_BUS_CLK_HALT_BMSK                                      0x2
#define HWIO_SSC_SDC_WFI_CONFIG_BUS_CLK_HALT_SHFT                                      0x1
#define HWIO_SSC_SDC_WFI_CONFIG_PROC_CLK_HALT_BMSK                                     0x1
#define HWIO_SSC_SDC_WFI_CONFIG_PROC_CLK_HALT_SHFT                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_SCC_SCC_SCC_REG
 *--------------------------------------------------------------------------*/

#define SSC_SCC_SCC_SCC_REG_REG_BASE                                               (SSC_BASE      + 0x00b10000)

#define HWIO_SSC_SCC_PLL_MODE_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000000)
#define HWIO_SSC_SCC_PLL_MODE_RMSK                                                 0xffffffff
#define HWIO_SSC_SCC_PLL_MODE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_MODE_ADDR, HWIO_SSC_SCC_PLL_MODE_RMSK)
#define HWIO_SSC_SCC_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_MODE_ADDR, m)
#define HWIO_SSC_SCC_PLL_MODE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_MODE_ADDR,v)
#define HWIO_SSC_SCC_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_MODE_ADDR,m,v,HWIO_SSC_SCC_PLL_MODE_IN)
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_BMSK                                    0x80000000
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_SHFT                                          0x1f
#define HWIO_SSC_SCC_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                                 0x40000000
#define HWIO_SSC_SCC_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                       0x1e
#define HWIO_SSC_SCC_PLL_MODE_PLL_ACK_LATCH_BMSK                                   0x20000000
#define HWIO_SSC_SCC_PLL_MODE_PLL_ACK_LATCH_SHFT                                         0x1d
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                               0x10000000
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                     0x1c
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BITS27_25_BMSK                                0xe000000
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BITS27_25_SHFT                                     0x19
#define HWIO_SSC_SCC_PLL_MODE_FSM_LEGACY_MODE_BMSK                                  0x1000000
#define HWIO_SSC_SCC_PLL_MODE_FSM_LEGACY_MODE_SHFT                                       0x18
#define HWIO_SSC_SCC_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                        0x800000
#define HWIO_SSC_SCC_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                            0x17
#define HWIO_SSC_SCC_PLL_MODE_PLL_UPDATE_BMSK                                        0x400000
#define HWIO_SSC_SCC_PLL_MODE_PLL_UPDATE_SHFT                                            0x16
#define HWIO_SSC_SCC_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                                0x200000
#define HWIO_SSC_SCC_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                    0x15
#define HWIO_SSC_SCC_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                                  0x100000
#define HWIO_SSC_SCC_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                      0x14
#define HWIO_SSC_SCC_PLL_MODE_PLL_BIAS_COUNT_BMSK                                     0xfc000
#define HWIO_SSC_SCC_PLL_MODE_PLL_BIAS_COUNT_SHFT                                         0xe
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_COUNT_BMSK                                      0x3f00
#define HWIO_SSC_SCC_PLL_MODE_PLL_LOCK_COUNT_SHFT                                         0x8
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BITS7_3_BMSK                                       0xf8
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BITS7_3_SHFT                                        0x3
#define HWIO_SSC_SCC_PLL_MODE_PLL_RESET_N_BMSK                                            0x4
#define HWIO_SSC_SCC_PLL_MODE_PLL_RESET_N_SHFT                                            0x2
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT1_BMSK                                           0x2
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT1_SHFT                                           0x1
#define HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_BMSK                                            0x1
#define HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_SHFT                                            0x0

#define HWIO_SSC_SCC_PLL_L_VAL_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000004)
#define HWIO_SSC_SCC_PLL_L_VAL_RMSK                                                    0xffff
#define HWIO_SSC_SCC_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_L_VAL_ADDR, HWIO_SSC_SCC_PLL_L_VAL_RMSK)
#define HWIO_SSC_SCC_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_L_VAL_ADDR, m)
#define HWIO_SSC_SCC_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_L_VAL_ADDR,v)
#define HWIO_SSC_SCC_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_L_VAL_ADDR,m,v,HWIO_SSC_SCC_PLL_L_VAL_IN)
#define HWIO_SSC_SCC_PLL_L_VAL_PLL_L_BMSK                                              0xffff
#define HWIO_SSC_SCC_PLL_L_VAL_PLL_L_SHFT                                                 0x0

#define HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000008)
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_RMSK                                                0xffff
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR, HWIO_SSC_SCC_PLL_CAL_L_VAL_RMSK)
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_CAL_L_VAL_ADDR,m,v,HWIO_SSC_SCC_PLL_CAL_L_VAL_IN)
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                      0xffff
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                         0x0

#define HWIO_SSC_SCC_PLL_USER_CTL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000000c)
#define HWIO_SSC_SCC_PLL_USER_CTL_RMSK                                             0xffffffff
#define HWIO_SSC_SCC_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_ADDR, HWIO_SSC_SCC_PLL_USER_CTL_RMSK)
#define HWIO_SSC_SCC_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_ADDR, m)
#define HWIO_SSC_SCC_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_USER_CTL_ADDR,v)
#define HWIO_SSC_SCC_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_USER_CTL_ADDR,m,v,HWIO_SSC_SCC_PLL_USER_CTL_IN)
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                           0xfff80000
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                                 0x13
#define HWIO_SSC_SCC_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                                  0x70000
#define HWIO_SSC_SCC_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                     0x10
#define HWIO_SSC_SCC_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                              0xf000
#define HWIO_SSC_SCC_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                                 0xc
#define HWIO_SSC_SCC_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                              0xf00
#define HWIO_SSC_SCC_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                                0x8
#define HWIO_SSC_SCC_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                                  0x80
#define HWIO_SSC_SCC_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                   0x7
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                   0x60
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                    0x5
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_TEST_BMSK                                       0x10
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_TEST_SHFT                                        0x4
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BIT3_BMSK                                       0x8
#define HWIO_SSC_SCC_PLL_USER_CTL_RESERVE_BIT3_SHFT                                       0x3
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_ODD_BMSK                                         0x4
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_ODD_SHFT                                         0x2
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                        0x2
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                        0x1
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                        0x1
#define HWIO_SSC_SCC_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                        0x0

#define HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000010)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RMSK                                           0xffffffff
#define HWIO_SSC_SCC_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR, HWIO_SSC_SCC_PLL_USER_CTL_U_RMSK)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR, m)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR,v)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR,m,v,HWIO_SSC_SCC_PLL_USER_CTL_U_IN)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DISABLE_DECAPS_BMSK                            0x80000000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DISABLE_DECAPS_SHFT                                  0x1f
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DISABLE_SHUNT_BMSK                             0x40000000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DISABLE_SHUNT_SHFT                                   0x1e
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BITS61_54_BMSK                         0x3fc00000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BITS61_54_SHFT                               0x16
#define HWIO_SSC_SCC_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_BMSK               0x200000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_SHFT                   0x15
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_BMSK                           0x100000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_DROOP_DIVIDER_SEL_SHFT                               0x14
#define HWIO_SSC_SCC_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_BMSK                          0x80000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_SCALE_FREQ_ON_DROOP_SHFT                             0x13
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                          0x40000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                             0x12
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                          0x20000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                             0x11
#define HWIO_SSC_SCC_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                           0x10000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                              0x10
#define HWIO_SSC_SCC_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                               0x8000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                                  0xf
#define HWIO_SSC_SCC_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                            0x4000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                               0xe
#define HWIO_SSC_SCC_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                                0x2000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                   0xd
#define HWIO_SSC_SCC_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                         0x1800
#define HWIO_SSC_SCC_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                            0xb
#define HWIO_SSC_SCC_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                         0x400
#define HWIO_SSC_SCC_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                           0xa
#define HWIO_SSC_SCC_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                                0x3e0
#define HWIO_SSC_SCC_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                                  0x5
#define HWIO_SSC_SCC_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                                  0x10
#define HWIO_SSC_SCC_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                   0x4
#define HWIO_SSC_SCC_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                       0xe
#define HWIO_SSC_SCC_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                       0x1
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                           0x1
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                           0x0

#define HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000014)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_RMSK                                           0xffffffff
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR, HWIO_SSC_SCC_PLL_CONFIG_CTL_RMSK)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR,m,v,HWIO_SSC_SCC_PLL_CONFIG_CTL_IN)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                        0xfc000000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                              0x1a
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                       0x3c00000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                            0x16
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK               0x3c0000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                   0x12
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK                0x3c000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                    0xe
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                       0x3800
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                          0xb
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                      0x700
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                        0x8
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                            0xf0
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                             0x4
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                     0xf
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                     0x0

#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000018)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RMSK                                         0xffffffff
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR, HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_SSC_SCC_PLL_CONFIG_CTL_U_IN)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                                0xfff00000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                      0x14
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ENABLEOVERPST_BMSK                              0x80000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ENABLEOVERPST_SHFT                                 0x13
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIASSELPST_BMSK                                 0x40000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIASSELPST_SHFT                                    0x12
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                               0x30000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                  0x10
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_BMSK                          0x8000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_SHFT                             0xf
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BYAPSS_DROOP_DIV_BMSK                            0x4000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BYAPSS_DROOP_DIV_SHFT                               0xe
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                           0x3000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                              0xc
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                          0xc00
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                            0xa
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                              0x380
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                                0x7
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                   0x40
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                    0x6
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                          0x30
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                           0x4
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                          0x8
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                          0x3
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                               0x6
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                               0x1
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                        0x1
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                        0x0

#define HWIO_SSC_SCC_PLL_TEST_CTL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000001c)
#define HWIO_SSC_SCC_PLL_TEST_CTL_RMSK                                             0xffffffff
#define HWIO_SSC_SCC_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_ADDR, HWIO_SSC_SCC_PLL_TEST_CTL_RMSK)
#define HWIO_SSC_SCC_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_ADDR, m)
#define HWIO_SSC_SCC_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_TEST_CTL_ADDR,v)
#define HWIO_SSC_SCC_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_TEST_CTL_ADDR,m,v,HWIO_SSC_SCC_PLL_TEST_CTL_IN)
#define HWIO_SSC_SCC_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                     0xc0000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                           0x1e
#define HWIO_SSC_SCC_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK                 0x30000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                       0x1c
#define HWIO_SSC_SCC_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                        0x8000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                             0x1b
#define HWIO_SSC_SCC_PLL_TEST_CTL_FINE_FCW_BMSK                                     0x7e00000
#define HWIO_SSC_SCC_PLL_TEST_CTL_FINE_FCW_SHFT                                          0x15
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                             0x100000
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                                 0x14
#define HWIO_SSC_SCC_PLL_TEST_CTL_COARSE_FCW_BMSK                                     0xff000
#define HWIO_SSC_SCC_PLL_TEST_CTL_COARSE_FCW_SHFT                                         0xc
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                              0x800
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                                0xb
#define HWIO_SSC_SCC_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                               0x700
#define HWIO_SSC_SCC_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                                 0x8
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                               0x80
#define HWIO_SSC_SCC_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                                0x7
#define HWIO_SSC_SCC_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                      0x40
#define HWIO_SSC_SCC_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                       0x6
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_VREF_TEST_BMSK                                      0x20
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_VREF_TEST_SHFT                                       0x5
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_VDCO_TEST_BMSK                                      0x10
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_VDCO_TEST_SHFT                                       0x4
#define HWIO_SSC_SCC_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                                0x8
#define HWIO_SSC_SCC_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                                0x3
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_IVCO_TEST_BMSK                                       0x4
#define HWIO_SSC_SCC_PLL_TEST_CTL_EN_IVCO_TEST_SHFT                                       0x2
#define HWIO_SSC_SCC_PLL_TEST_CTL_ATEST1_EN_BMSK                                          0x2
#define HWIO_SSC_SCC_PLL_TEST_CTL_ATEST1_EN_SHFT                                          0x1
#define HWIO_SSC_SCC_PLL_TEST_CTL_ATEST0_EN_BMSK                                          0x1
#define HWIO_SSC_SCC_PLL_TEST_CTL_ATEST0_EN_SHFT                                          0x0

#define HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000020)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_RMSK                                           0xffffffff
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR, HWIO_SSC_SCC_PLL_TEST_CTL_U_RMSK)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR,m,v,HWIO_SSC_SCC_PLL_TEST_CTL_U_IN)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                       0x80000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                             0x1f
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                          0x40000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                                0x1e
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                           0x20000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                                 0x1d
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                          0x10000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                                0x1c
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                     0x8000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                          0x1b
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK                  0x6000000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                       0x19
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                       0x1c00000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                            0x16
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                        0x200000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                            0x15
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                               0x100000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                   0x14
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                          0x80000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                             0x13
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                      0x40000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                         0x12
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                                  0x30000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                     0x10
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                   0x8000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                      0xf
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DIS_AMP_STARTUP_BMSK                               0x4000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DIS_AMP_STARTUP_SHFT                                  0xe
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                                0x2000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                   0xd
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                              0x1000
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                                 0xc
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                    0xc00
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                      0xa
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                      0x380
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                        0x7
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                    0x40
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                     0x6
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                    0x20
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                     0x5
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                                0x18
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                                 0x3
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                    0x4
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                    0x2
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                   0x2
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                   0x1
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                   0x1
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                   0x0

#define HWIO_SSC_SCC_PLL_STATUS_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000024)
#define HWIO_SSC_SCC_PLL_STATUS_RMSK                                               0xffffffff
#define HWIO_SSC_SCC_PLL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATUS_ADDR, HWIO_SSC_SCC_PLL_STATUS_RMSK)
#define HWIO_SSC_SCC_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATUS_ADDR, m)
#define HWIO_SSC_SCC_PLL_STATUS_STATUS_31_0_BMSK                                   0xffffffff
#define HWIO_SSC_SCC_PLL_STATUS_STATUS_31_0_SHFT                                          0x0

#define HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000028)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_RMSK                                             0xffffffff
#define HWIO_SSC_SCC_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR, HWIO_SSC_SCC_PLL_FREQ_CTL_RMSK)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR, m)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR,v)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR,m,v,HWIO_SSC_SCC_PLL_FREQ_CTL_IN)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                          0xffffffff
#define HWIO_SSC_SCC_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                                 0x0

#define HWIO_SSC_SCC_PLL_OPMODE_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000002c)
#define HWIO_SSC_SCC_PLL_OPMODE_RMSK                                                      0x7
#define HWIO_SSC_SCC_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_OPMODE_ADDR, HWIO_SSC_SCC_PLL_OPMODE_RMSK)
#define HWIO_SSC_SCC_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_OPMODE_ADDR, m)
#define HWIO_SSC_SCC_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_OPMODE_ADDR,v)
#define HWIO_SSC_SCC_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_OPMODE_ADDR,m,v,HWIO_SSC_SCC_PLL_OPMODE_IN)
#define HWIO_SSC_SCC_PLL_OPMODE_PLL_OPMODE_BMSK                                           0x7
#define HWIO_SSC_SCC_PLL_OPMODE_PLL_OPMODE_SHFT                                           0x0

#define HWIO_SSC_SCC_PLL_STATE_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000030)
#define HWIO_SSC_SCC_PLL_STATE_RMSK                                                       0x7
#define HWIO_SSC_SCC_PLL_STATE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATE_ADDR, HWIO_SSC_SCC_PLL_STATE_RMSK)
#define HWIO_SSC_SCC_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATE_ADDR, m)
#define HWIO_SSC_SCC_PLL_STATE_PLL_STATE_BMSK                                             0x7
#define HWIO_SSC_SCC_PLL_STATE_PLL_STATE_SHFT                                             0x0

#define HWIO_SSC_SCC_PLL_DROOP_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000034)
#define HWIO_SSC_SCC_PLL_DROOP_RMSK                                                      0xff
#define HWIO_SSC_SCC_PLL_DROOP_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_DROOP_ADDR, HWIO_SSC_SCC_PLL_DROOP_RMSK)
#define HWIO_SSC_SCC_PLL_DROOP_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_DROOP_ADDR, m)
#define HWIO_SSC_SCC_PLL_DROOP_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_DROOP_ADDR,v)
#define HWIO_SSC_SCC_PLL_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_DROOP_ADDR,m,v,HWIO_SSC_SCC_PLL_DROOP_IN)
#define HWIO_SSC_SCC_PLL_DROOP_DROOP_CODE_BMSK                                           0xfe
#define HWIO_SSC_SCC_PLL_DROOP_DROOP_CODE_SHFT                                            0x1
#define HWIO_SSC_SCC_PLL_DROOP_DROOP_EN_BMSK                                              0x1
#define HWIO_SSC_SCC_PLL_DROOP_DROOP_EN_SHFT                                              0x0

#define HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000038)
#define HWIO_SSC_SCC_PLL_FRAC_VAL_RMSK                                                 0xffff
#define HWIO_SSC_SCC_PLL_FRAC_VAL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR, HWIO_SSC_SCC_PLL_FRAC_VAL_RMSK)
#define HWIO_SSC_SCC_PLL_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR, m)
#define HWIO_SSC_SCC_PLL_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR,v)
#define HWIO_SSC_SCC_PLL_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_FRAC_VAL_ADDR,m,v,HWIO_SSC_SCC_PLL_FRAC_VAL_IN)
#define HWIO_SSC_SCC_PLL_FRAC_VAL_PLL_FRAC_VAL_BMSK                                    0xffff
#define HWIO_SSC_SCC_PLL_FRAC_VAL_PLL_FRAC_VAL_SHFT                                       0x0

#define HWIO_SSC_SCC_PLL_SPARE_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000003c)
#define HWIO_SSC_SCC_PLL_SPARE_RMSK                                                      0xff
#define HWIO_SSC_SCC_PLL_SPARE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_SPARE_ADDR, HWIO_SSC_SCC_PLL_SPARE_RMSK)
#define HWIO_SSC_SCC_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_SPARE_ADDR, m)
#define HWIO_SSC_SCC_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_SPARE_ADDR,v)
#define HWIO_SSC_SCC_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_SPARE_ADDR,m,v,HWIO_SSC_SCC_PLL_SPARE_IN)
#define HWIO_SSC_SCC_PLL_SPARE_SPARE_OUTPUTS_BMSK                                        0xf0
#define HWIO_SSC_SCC_PLL_SPARE_SPARE_OUTPUTS_SHFT                                         0x4
#define HWIO_SSC_SCC_PLL_SPARE_SPARE_INPUTS_BMSK                                          0xf
#define HWIO_SSC_SCC_PLL_SPARE_SPARE_INPUTS_SHFT                                          0x0

#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000040)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_RMSK                                            0xff
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                            0xff
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                             0x0

#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000044)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_RMSK                                             0xf
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR, HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                             0xf
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                             0x0

#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000048)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_RMSK                                               0xf
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR, HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_IN)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                                 0xf
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                                 0x0

#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00001000)
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_RMSK                                        0x80000013
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                               0x10
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                0x4
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00001004)
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_RMSK                                          0x11071f
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_HW_CLK_CONTROL_BMSK                           0x100000
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_HW_CLK_CONTROL_SHFT                               0x14
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_RCGLITE_DISABLE_BMSK                           0x10000
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_RCGLITE_DISABLE_SHFT                              0x10
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE0_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002000)
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE0_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002018)
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002020)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002024)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002028)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000202c)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002030)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002034)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002038)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000203c)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002060)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002064)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002068)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000206c)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002070)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002074)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002078)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000207c)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020a0)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020a4)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020a8)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020ac)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020b0)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020b4)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020b8)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020bc)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020e0)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020e4)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020e8)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020ec)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020f0)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020f4)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020f8)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000020fc)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002004)
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002008)
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE0_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000200c)
#define HWIO_SSC_SCC_QUPV3_SE0_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_M_ADDR, HWIO_SSC_SCC_QUPV3_SE0_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE0_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002010)
#define HWIO_SSC_SCC_QUPV3_SE0_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_N_ADDR, HWIO_SSC_SCC_QUPV3_SE0_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE0_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002014)
#define HWIO_SSC_SCC_QUPV3_SE0_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_D_ADDR, HWIO_SSC_SCC_QUPV3_SE0_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE0_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00002130)
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE0_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE0_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE0_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE0_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE0_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE1_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003000)
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE1_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003018)
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003020)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003024)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003028)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000302c)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003030)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003034)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003038)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000303c)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003060)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003064)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003068)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000306c)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003070)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003074)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003078)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000307c)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030a0)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030a4)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030a8)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030ac)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030b0)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030b4)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030b8)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030bc)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030e0)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030e4)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030e8)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030ec)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030f0)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030f4)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030f8)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000030fc)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003004)
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003008)
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE1_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000300c)
#define HWIO_SSC_SCC_QUPV3_SE1_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_M_ADDR, HWIO_SSC_SCC_QUPV3_SE1_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE1_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003010)
#define HWIO_SSC_SCC_QUPV3_SE1_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_N_ADDR, HWIO_SSC_SCC_QUPV3_SE1_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE1_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003014)
#define HWIO_SSC_SCC_QUPV3_SE1_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_D_ADDR, HWIO_SSC_SCC_QUPV3_SE1_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE1_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00003130)
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE1_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE1_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE1_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE1_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE1_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE2_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004000)
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE2_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004018)
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004020)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004024)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004028)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000402c)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004030)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004034)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004038)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000403c)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004060)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004064)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004068)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000406c)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004070)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004074)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004078)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000407c)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040a0)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040a4)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040a8)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040ac)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040b0)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040b4)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040b8)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040bc)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040e0)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040e4)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040e8)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040ec)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040f0)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040f4)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040f8)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x000040fc)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004004)
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004008)
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE2_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000400c)
#define HWIO_SSC_SCC_QUPV3_SE2_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_M_ADDR, HWIO_SSC_SCC_QUPV3_SE2_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE2_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004010)
#define HWIO_SSC_SCC_QUPV3_SE2_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_N_ADDR, HWIO_SSC_SCC_QUPV3_SE2_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE2_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004014)
#define HWIO_SSC_SCC_QUPV3_SE2_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_D_ADDR, HWIO_SSC_SCC_QUPV3_SE2_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE2_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00004130)
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE2_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE2_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE2_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE2_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE2_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE3_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b000)
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE3_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b018)
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b020)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b024)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b028)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b02c)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b030)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b034)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b038)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b03c)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b060)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b064)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b068)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b06c)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b070)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b074)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b078)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b07c)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0a0)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0a4)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0a8)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0ac)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0b0)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0b4)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0b8)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0bc)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0e0)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0e4)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0e8)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0ec)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0f0)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0f4)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0f8)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b0fc)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b004)
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b008)
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE3_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b00c)
#define HWIO_SSC_SCC_QUPV3_SE3_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_M_ADDR, HWIO_SSC_SCC_QUPV3_SE3_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE3_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b010)
#define HWIO_SSC_SCC_QUPV3_SE3_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_N_ADDR, HWIO_SSC_SCC_QUPV3_SE3_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE3_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b014)
#define HWIO_SSC_SCC_QUPV3_SE3_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_D_ADDR, HWIO_SSC_SCC_QUPV3_SE3_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE3_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000b130)
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE3_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE3_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE3_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE3_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE3_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE4_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c000)
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE4_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c018)
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c020)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c024)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c028)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c02c)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c030)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c034)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c038)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c03c)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c060)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c064)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c068)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c06c)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c070)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c074)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c078)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c07c)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0a0)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0a4)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0a8)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0ac)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0b0)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0b4)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0b8)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0bc)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0e0)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0e4)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0e8)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0ec)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0f0)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0f4)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0f8)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c0fc)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c004)
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c008)
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE4_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c00c)
#define HWIO_SSC_SCC_QUPV3_SE4_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_M_ADDR, HWIO_SSC_SCC_QUPV3_SE4_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE4_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c010)
#define HWIO_SSC_SCC_QUPV3_SE4_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_N_ADDR, HWIO_SSC_SCC_QUPV3_SE4_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE4_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c014)
#define HWIO_SSC_SCC_QUPV3_SE4_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_D_ADDR, HWIO_SSC_SCC_QUPV3_SE4_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE4_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000c130)
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE4_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE4_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE4_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE4_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE4_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE5_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d000)
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_BCR_ADDR, HWIO_SSC_SCC_QUPV3_SE5_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d018)
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_RMSK                                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_RCG_SW_CTRL_BMSK                                 0x8000
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_RCG_SW_CTRL_SHFT                                    0xf
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_SW_PERF_STATE_BMSK                               0x7800
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_SW_PERF_STATE_SHFT                                  0xb
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_SW_OVERRIDE_BMSK                                  0x400
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_SW_OVERRIDE_SHFT                                    0xa
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                     0x200
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                       0x9
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_DFS_FSM_STATE_BMSK                                0x1c0
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_DFS_FSM_STATE_SHFT                                  0x6
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_HW_CLK_CONTROL_BMSK                                0x20
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_HW_CLK_CONTROL_SHFT                                 0x5
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_CURR_PERF_STATE_BMSK                               0x1e
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_CURR_PERF_STATE_SHFT                                0x1
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_DFS_EN_BMSK                                         0x1
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_DFS_EN_SHFT                                         0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d020)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d024)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d028)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d02c)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d030)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d034)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d038)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_ADDR                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d03c)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_RMSK                                 0x371f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_MODE_BMSK                            0x3000
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_MODE_SHFT                               0xc
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_SRC_SEL_BMSK                          0x700
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_SRC_SEL_SHFT                            0x8
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_SRC_DIV_BMSK                           0x1f
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_SRC_DIV_SHFT                            0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d060)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d064)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d068)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d06c)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d070)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d074)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d078)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d07c)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_M_BMSK                             0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_M_SHFT                                0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0a0)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0a4)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0a8)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0ac)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0b0)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0b4)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0b8)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0bc)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_NOT_N_MINUS_M_BMSK                 0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_NOT_N_MINUS_M_SHFT                    0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0e0)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0e4)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0e8)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0ec)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0f0)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0f4)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0f8)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d0fc)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_RMSK                               0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_ADDR, HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_RMSK)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_ADDR, m)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_ADDR,v)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_ADDR,m,v,HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_IN)
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_NOT_2D_BMSK                        0xffff
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_NOT_2D_SHFT                           0x0

#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d004)
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_RMSK                                       0x800000f3
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_D_BMSK                                     0x80
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_D_SHFT                                      0x7
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_N_BMSK                                     0x40
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_N_SHFT                                      0x6
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_M_BMSK                                     0x20
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_M_SHFT                                      0x5
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d008)
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_RMSK                                         0x10371f
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_ADDR, HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_MODE_BMSK                                      0x3000
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_MODE_SHFT                                         0xc
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_SE5_M_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d00c)
#define HWIO_SSC_SCC_QUPV3_SE5_M_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_M_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_M_ADDR, HWIO_SSC_SCC_QUPV3_SE5_M_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_M_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_M_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_M_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_M_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_M_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_M_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_M_M_BMSK                                                0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_M_M_SHFT                                                   0x0

#define HWIO_SSC_SCC_QUPV3_SE5_N_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d010)
#define HWIO_SSC_SCC_QUPV3_SE5_N_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_N_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_N_ADDR, HWIO_SSC_SCC_QUPV3_SE5_N_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_N_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_N_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_N_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_N_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_N_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_N_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_N_NOT_N_MINUS_M_BMSK                                    0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_N_NOT_N_MINUS_M_SHFT                                       0x0

#define HWIO_SSC_SCC_QUPV3_SE5_D_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d014)
#define HWIO_SSC_SCC_QUPV3_SE5_D_RMSK                                                  0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_D_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_D_ADDR, HWIO_SSC_SCC_QUPV3_SE5_D_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_D_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_D_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_D_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_D_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_D_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_D_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_D_NOT_2D_BMSK                                           0xffff
#define HWIO_SSC_SCC_QUPV3_SE5_D_NOT_2D_SHFT                                              0x0

#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000d130)
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_RMSK                                           0x80007ff5
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_SE5_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_SE5_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_SE5_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_SE5_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_SE5_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_Q6_SPM_BCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00008000)
#define HWIO_SSC_SCC_Q6_SPM_BCR_RMSK                                                      0x1
#define HWIO_SSC_SCC_Q6_SPM_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_SPM_BCR_ADDR, HWIO_SSC_SCC_Q6_SPM_BCR_RMSK)
#define HWIO_SSC_SCC_Q6_SPM_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_SPM_BCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_SPM_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_SPM_BCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_SPM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_SPM_BCR_ADDR,m,v,HWIO_SSC_SCC_Q6_SPM_BCR_IN)
#define HWIO_SSC_SCC_Q6_SPM_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_SSC_SCC_Q6_SPM_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_ADDR                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00008004)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_RMSK                                  0xf
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_CLK_DIV_BMSK                          0xf
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_CLK_DIV_SHFT                          0x0

#define HWIO_SSC_SCC_Q6_SPM_CBCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00008008)
#define HWIO_SSC_SCC_Q6_SPM_CBCR_RMSK                                              0x80000005
#define HWIO_SSC_SCC_Q6_SPM_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_SPM_CBCR_ADDR, HWIO_SSC_SCC_Q6_SPM_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_SPM_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_SPM_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_SPM_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_SPM_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_SPM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_SPM_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_SPM_CBCR_IN)
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_ARES_BMSK                                            0x4
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_ARES_SHFT                                            0x2
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_SSC_SCC_Q6_SPM_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR                                     (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e000)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_RMSK                                            0x1
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR, HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_IN)
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_BLK_ARES_BMSK                                   0x1
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_BLK_ARES_SHFT                                   0x0

#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e004)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_RMSK                                            0x1ff
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR, HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_CLK_DIV_BMSK                                    0x1ff
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_CLK_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_CCD_CBCR_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e008)
#define HWIO_SSC_SCC_CCD_CBCR_RMSK                                                 0x80000007
#define HWIO_SSC_SCC_CCD_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_CBCR_ADDR, HWIO_SSC_SCC_CCD_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_CBCR_IN)
#define HWIO_SSC_SCC_CCD_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_SSC_SCC_CCD_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ARES_BMSK                                               0x4
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ARES_SHFT                                               0x2
#define HWIO_SSC_SCC_CCD_CBCR_HW_CTL_BMSK                                                 0x2
#define HWIO_SSC_SCC_CCD_CBCR_HW_CTL_SHFT                                                 0x1
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_SSC_SCC_CCD_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e00c)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_RMSK                                            0x80000007
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR, HWIO_SSC_SCC_CCD_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_HW_CTL_BMSK                                            0x2
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_HW_CTL_SHFT                                            0x1
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000e010)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_RMSK                            0x80000005
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR, HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_RMSK)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_ADDR,m,v,HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_IN)
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_OFF_BMSK                    0x80000000
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_OFF_SHFT                          0x1f
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ARES_BMSK                          0x4
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ARES_SHFT                          0x2
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ENABLE_BMSK                        0x1
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_CLK_ENABLE_SHFT                        0x0

#define HWIO_SSC_SCC_QUPV3_CORE_BCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00005000)
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_RMSK                                                  0x1
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_CORE_BCR_ADDR, HWIO_SSC_SCC_QUPV3_CORE_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_CORE_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_CORE_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_CORE_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_CORE_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00005004)
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_RMSK                                     0xf
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR, HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_CLK_DIV_BMSK                             0xf
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_CLK_DIV_SHFT                             0x0

#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00005008)
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_RMSK                                        0x80007ff5
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_CORE_ON_BMSK                          0x4000
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_CORE_ON_SHFT                             0xe
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                        0x2000
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                           0xd
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                       0x1000
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                          0xc
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_WAKEUP_BMSK                                      0xf00
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_WAKEUP_SHFT                                        0x8
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_SLEEP_BMSK                                        0xf0
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_SLEEP_SHFT                                         0x4
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000500c)
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_RMSK                                       0x3
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR, HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_CLK_DIV_BMSK                               0x3
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_CLK_DIV_SHFT                               0x0

#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00005010)
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_RMSK                                          0x80007ff5
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_CORE_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_CORE_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_CORE_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_CORE_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_CORE_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_CORE_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_OFF_BMSK                                  0x80000000
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_OFF_SHFT                                        0x1f
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                            0x4000
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                               0xe
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                          0x2000
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                             0xd
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                         0x1000
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                            0xc
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_WAKEUP_BMSK                                        0xf00
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_WAKEUP_SHFT                                          0x8
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_SLEEP_BMSK                                          0xf0
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_SLEEP_SHFT                                           0x4
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_ARES_BMSK                                        0x4
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_ARES_SHFT                                        0x2
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_ENABLE_BMSK                                      0x1
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_CLK_ENABLE_SHFT                                      0x0

#define HWIO_SSC_SCC_SLEEP_BCR_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00010000)
#define HWIO_SSC_SCC_SLEEP_BCR_RMSK                                                       0x1
#define HWIO_SSC_SCC_SLEEP_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SLEEP_BCR_ADDR, HWIO_SSC_SCC_SLEEP_BCR_RMSK)
#define HWIO_SSC_SCC_SLEEP_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SLEEP_BCR_ADDR, m)
#define HWIO_SSC_SCC_SLEEP_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SLEEP_BCR_ADDR,v)
#define HWIO_SSC_SCC_SLEEP_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SLEEP_BCR_ADDR,m,v,HWIO_SSC_SCC_SLEEP_BCR_IN)
#define HWIO_SSC_SCC_SLEEP_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_SSC_SCC_SLEEP_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_SSC_SCC_SLEEP_CBCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00010004)
#define HWIO_SSC_SCC_SLEEP_CBCR_RMSK                                               0x80000005
#define HWIO_SSC_SCC_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SLEEP_CBCR_ADDR, HWIO_SSC_SCC_SLEEP_CBCR_RMSK)
#define HWIO_SSC_SCC_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SLEEP_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SLEEP_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SLEEP_CBCR_ADDR,m,v,HWIO_SSC_SCC_SLEEP_CBCR_IN)
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_ARES_SHFT                                             0x2
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_SSC_SCC_SLEEP_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00010008)
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_RMSK                                                  0x1
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_AOSS_CC_RO_BCR_ADDR, HWIO_SSC_SCC_AOSS_CC_RO_BCR_RMSK)
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_AOSS_CC_RO_BCR_ADDR, m)
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_AOSS_CC_RO_BCR_ADDR,v)
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_AOSS_CC_RO_BCR_ADDR,m,v,HWIO_SSC_SCC_AOSS_CC_RO_BCR_IN)
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_SSC_SCC_RO_CBCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0001000c)
#define HWIO_SSC_SCC_RO_CBCR_RMSK                                                  0x80000005
#define HWIO_SSC_SCC_RO_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_RO_CBCR_ADDR, HWIO_SSC_SCC_RO_CBCR_RMSK)
#define HWIO_SSC_SCC_RO_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_RO_CBCR_ADDR, m)
#define HWIO_SSC_SCC_RO_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_RO_CBCR_ADDR,v)
#define HWIO_SSC_SCC_RO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_RO_CBCR_ADDR,m,v,HWIO_SSC_SCC_RO_CBCR_IN)
#define HWIO_SSC_SCC_RO_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_SSC_SCC_RO_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_SSC_SCC_RO_CBCR_CLK_ARES_BMSK                                                0x4
#define HWIO_SSC_SCC_RO_CBCR_CLK_ARES_SHFT                                                0x2
#define HWIO_SSC_SCC_RO_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_SSC_SCC_RO_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_SSC_SCC_SDC_BCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f000)
#define HWIO_SSC_SCC_SDC_BCR_RMSK                                                         0x1
#define HWIO_SSC_SCC_SDC_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_BCR_ADDR, HWIO_SSC_SCC_SDC_BCR_RMSK)
#define HWIO_SSC_SCC_SDC_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_BCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_BCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_BCR_ADDR,m,v,HWIO_SSC_SCC_SDC_BCR_IN)
#define HWIO_SSC_SCC_SDC_BCR_BLK_ARES_BMSK                                                0x1
#define HWIO_SSC_SCC_SDC_BCR_BLK_ARES_SHFT                                                0x0

#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f004)
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_RMSK                                            0x1ff
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR, HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_CLK_DIV_BMSK                                    0x1ff
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_CLK_DIV_SHFT                                      0x0

#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f008)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_RMSK                                       0x80000005
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f00c)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_RMSK                                       0x80000007
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_HW_CTL_BMSK                                       0x2
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_HW_CTL_SHFT                                       0x1
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000f010)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_RMSK                                        0x80007ff7
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_CORE_ON_BMSK                          0x4000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_CORE_ON_SHFT                             0xe
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_ON_BMSK                        0x2000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_ON_SHFT                           0xd
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                       0x1000
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                          0xc
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_WAKEUP_BMSK                                      0xf00
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_WAKEUP_SHFT                                        0x8
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_SLEEP_BMSK                                        0xf0
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_SLEEP_SHFT                                         0x4
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_HW_CTL_BMSK                                        0x2
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_HW_CTL_SHFT                                        0x1
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_BUS_BCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009000)
#define HWIO_SSC_SCC_BUS_BCR_RMSK                                                         0x1
#define HWIO_SSC_SCC_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_BUS_BCR_ADDR, HWIO_SSC_SCC_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_BUS_BCR_IN)
#define HWIO_SSC_SCC_BUS_BCR_BLK_ARES_BMSK                                                0x1
#define HWIO_SSC_SCC_BUS_BCR_BLK_ARES_SHFT                                                0x0

#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009004)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_RMSK                                     0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_CLK_DIV_BMSK                             0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_CLK_DIV_SHFT                             0x0

#define HWIO_SSC_SCC_CRIF_CBCR_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009008)
#define HWIO_SSC_SCC_CRIF_CBCR_RMSK                                                0x80000004
#define HWIO_SSC_SCC_CRIF_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CRIF_CBCR_ADDR, HWIO_SSC_SCC_CRIF_CBCR_RMSK)
#define HWIO_SSC_SCC_CRIF_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CRIF_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CRIF_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CRIF_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CRIF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CRIF_CBCR_ADDR,m,v,HWIO_SSC_SCC_CRIF_CBCR_IN)
#define HWIO_SSC_SCC_CRIF_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_SSC_SCC_CRIF_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_SSC_SCC_CRIF_CBCR_CLK_ARES_BMSK                                              0x4
#define HWIO_SSC_SCC_CRIF_CBCR_CLK_ARES_SHFT                                              0x2

#define HWIO_SSC_SCC_DATA_H_CBCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000900c)
#define HWIO_SSC_SCC_DATA_H_CBCR_RMSK                                              0x80000005
#define HWIO_SSC_SCC_DATA_H_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DATA_H_CBCR_ADDR, HWIO_SSC_SCC_DATA_H_CBCR_RMSK)
#define HWIO_SSC_SCC_DATA_H_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DATA_H_CBCR_ADDR, m)
#define HWIO_SSC_SCC_DATA_H_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DATA_H_CBCR_ADDR,v)
#define HWIO_SSC_SCC_DATA_H_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DATA_H_CBCR_ADDR,m,v,HWIO_SSC_SCC_DATA_H_CBCR_IN)
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_ARES_BMSK                                            0x4
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_ARES_SHFT                                            0x2
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_SSC_SCC_DATA_H_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_SSC_SCC_CSR_H_CBCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009010)
#define HWIO_SSC_SCC_CSR_H_CBCR_RMSK                                               0x80000004
#define HWIO_SSC_SCC_CSR_H_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_CSR_H_CBCR_ADDR, HWIO_SSC_SCC_CSR_H_CBCR_RMSK)
#define HWIO_SSC_SCC_CSR_H_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CSR_H_CBCR_ADDR, m)
#define HWIO_SSC_SCC_CSR_H_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CSR_H_CBCR_ADDR,v)
#define HWIO_SSC_SCC_CSR_H_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CSR_H_CBCR_ADDR,m,v,HWIO_SSC_SCC_CSR_H_CBCR_IN)
#define HWIO_SSC_SCC_CSR_H_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_SSC_SCC_CSR_H_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_SSC_SCC_CSR_H_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_SSC_SCC_CSR_H_CBCR_CLK_ARES_SHFT                                             0x2

#define HWIO_SSC_SCC_BUS_ALT_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009014)
#define HWIO_SSC_SCC_BUS_ALT_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_BUS_ALT_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_BUS_ALT_CBCR_ADDR, HWIO_SSC_SCC_BUS_ALT_CBCR_RMSK)
#define HWIO_SSC_SCC_BUS_ALT_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_BUS_ALT_CBCR_ADDR, m)
#define HWIO_SSC_SCC_BUS_ALT_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_BUS_ALT_CBCR_ADDR,v)
#define HWIO_SSC_SCC_BUS_ALT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_BUS_ALT_CBCR_ADDR,m,v,HWIO_SSC_SCC_BUS_ALT_CBCR_IN)
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_BUS_ALT_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_Q6_AHB_BCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009018)
#define HWIO_SSC_SCC_Q6_AHB_BCR_RMSK                                                      0x1
#define HWIO_SSC_SCC_Q6_AHB_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHB_BCR_ADDR, HWIO_SSC_SCC_Q6_AHB_BCR_RMSK)
#define HWIO_SSC_SCC_Q6_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHB_BCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_AHB_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_AHB_BCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_AHB_BCR_ADDR,m,v,HWIO_SSC_SCC_Q6_AHB_BCR_IN)
#define HWIO_SSC_SCC_Q6_AHB_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_SSC_SCC_Q6_AHB_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_SSC_SCC_Q6_AHBM_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000901c)
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_RMSK                                             0x80000007
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHBM_CBCR_ADDR, HWIO_SSC_SCC_Q6_AHBM_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHBM_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_AHBM_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_AHBM_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_AHBM_CBCR_IN)
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_HW_CTL_BMSK                                             0x2
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_HW_CTL_SHFT                                             0x1
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_Q6_AHBS_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009020)
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_RMSK                                             0x80000007
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHBS_CBCR_ADDR, HWIO_SSC_SCC_Q6_AHBS_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_AHBS_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_AHBS_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_AHBS_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_AHBS_CBCR_IN)
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_HW_CTL_BMSK                                             0x2
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_HW_CTL_SHFT                                             0x1
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_SMEM_BCR_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009024)
#define HWIO_SSC_SCC_SMEM_BCR_RMSK                                                        0x1
#define HWIO_SSC_SCC_SMEM_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SMEM_BCR_ADDR, HWIO_SSC_SCC_SMEM_BCR_RMSK)
#define HWIO_SSC_SCC_SMEM_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SMEM_BCR_ADDR, m)
#define HWIO_SSC_SCC_SMEM_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SMEM_BCR_ADDR,v)
#define HWIO_SSC_SCC_SMEM_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SMEM_BCR_ADDR,m,v,HWIO_SSC_SCC_SMEM_BCR_IN)
#define HWIO_SSC_SCC_SMEM_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_SSC_SCC_SMEM_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_SSC_SCC_SMEM_CBCR_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009028)
#define HWIO_SSC_SCC_SMEM_CBCR_RMSK                                                0x80007ff5
#define HWIO_SSC_SCC_SMEM_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SMEM_CBCR_ADDR, HWIO_SSC_SCC_SMEM_CBCR_RMSK)
#define HWIO_SSC_SCC_SMEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SMEM_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SMEM_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SMEM_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SMEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SMEM_CBCR_ADDR,m,v,HWIO_SSC_SCC_SMEM_CBCR_IN)
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_CORE_ON_BMSK                                  0x4000
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                     0xe
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                0x2000
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                   0xd
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                               0x1000
#define HWIO_SSC_SCC_SMEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                  0xc
#define HWIO_SSC_SCC_SMEM_CBCR_WAKEUP_BMSK                                              0xf00
#define HWIO_SSC_SCC_SMEM_CBCR_WAKEUP_SHFT                                                0x8
#define HWIO_SSC_SCC_SMEM_CBCR_SLEEP_BMSK                                                0xf0
#define HWIO_SSC_SCC_SMEM_CBCR_SLEEP_SHFT                                                 0x4
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_ARES_BMSK                                              0x4
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_ARES_SHFT                                              0x2
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_ENABLE_BMSK                                            0x1
#define HWIO_SSC_SCC_SMEM_CBCR_CLK_ENABLE_SHFT                                            0x0

#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000902c)
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_RMSK                                                 0x1
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_AHB_TIMEOUT_BCR_ADDR, HWIO_SSC_SCC_AHB_TIMEOUT_BCR_RMSK)
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_AHB_TIMEOUT_BCR_ADDR, m)
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_AHB_TIMEOUT_BCR_ADDR,v)
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_AHB_TIMEOUT_BCR_ADDR,m,v,HWIO_SSC_SCC_AHB_TIMEOUT_BCR_IN)
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_BLK_ARES_BMSK                                        0x1
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_BLK_ARES_SHFT                                        0x0

#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009030)
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_RMSK                                         0x80000005
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_ADDR, HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_RMSK)
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_ADDR, m)
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_ADDR,v)
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_ADDR,m,v,HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_IN)
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_OFF_BMSK                                 0x80000000
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_OFF_SHFT                                       0x1f
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_ARES_BMSK                                       0x4
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_ARES_SHFT                                       0x2
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_ENABLE_BMSK                                     0x1
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_CLK_ENABLE_SHFT                                     0x0

#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009034)
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_RMSK                                      0x80000005
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_ADDR, HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_IN)
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_ARES_BMSK                                    0x4
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_ARES_SHFT                                    0x2
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_ADDR                                       (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009038)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_RMSK                                              0x1
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_ADDR, HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_RMSK)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_ADDR,m,v,HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_IN)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_BLK_ARES_BMSK                                     0x1
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_BLK_ARES_SHFT                                     0x0

#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000903c)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_RMSK                                      0x80000005
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_ADDR, HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_IN)
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_ARES_BMSK                                    0x4
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_ARES_SHFT                                    0x2
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_ADDR                                     (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009040)
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_RMSK                                     0x80000005
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_ADDR, HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_IN)
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_OFF_BMSK                             0x80000000
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_OFF_SHFT                                   0x1f
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_ARES_BMSK                                   0x4
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_ARES_SHFT                                   0x2
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_ENABLE_BMSK                                 0x1
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_CLK_ENABLE_SHFT                                 0x0

#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009044)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_RMSK                                             0x1
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_ADDR, HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_RMSK)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_ADDR,m,v,HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_IN)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_BLK_ARES_BMSK                                    0x1
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_BLK_ARES_SHFT                                    0x0

#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_ADDR                                     (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009048)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_RMSK                                     0x80000005
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_ADDR, HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_IN)
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_OFF_BMSK                             0x80000000
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_OFF_SHFT                                   0x1f
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_ARES_BMSK                                   0x4
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_ARES_SHFT                                   0x2
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_ENABLE_BMSK                                 0x1
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_CLK_ENABLE_SHFT                                 0x0

#define HWIO_SSC_SCC_VS_CX_AHB_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000904c)
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_CX_AHB_BCR_ADDR, HWIO_SSC_SCC_VS_CX_AHB_BCR_RMSK)
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_CX_AHB_BCR_ADDR, m)
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_CX_AHB_BCR_ADDR,v)
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_CX_AHB_BCR_ADDR,m,v,HWIO_SSC_SCC_VS_CX_AHB_BCR_IN)
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009050)
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_RMSK                                           0x80000005
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_CX_AHB_CBCR_ADDR, HWIO_SSC_SCC_VS_CX_AHB_CBCR_RMSK)
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_CX_AHB_CBCR_ADDR, m)
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_CX_AHB_CBCR_ADDR,v)
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_CX_AHB_CBCR_ADDR,m,v,HWIO_SSC_SCC_VS_CX_AHB_CBCR_IN)
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_VS_MX_AHB_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009054)
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_MX_AHB_BCR_ADDR, HWIO_SSC_SCC_VS_MX_AHB_BCR_RMSK)
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_MX_AHB_BCR_ADDR, m)
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_MX_AHB_BCR_ADDR,v)
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_MX_AHB_BCR_ADDR,m,v,HWIO_SSC_SCC_VS_MX_AHB_BCR_IN)
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009058)
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_RMSK                                           0x80000005
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_MX_AHB_CBCR_ADDR, HWIO_SSC_SCC_VS_MX_AHB_CBCR_RMSK)
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_MX_AHB_CBCR_ADDR, m)
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_MX_AHB_CBCR_ADDR,v)
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_MX_AHB_CBCR_ADDR,m,v,HWIO_SSC_SCC_VS_MX_AHB_CBCR_IN)
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_QTIMER_BCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000905c)
#define HWIO_SSC_SCC_QTIMER_BCR_RMSK                                                      0x1
#define HWIO_SSC_SCC_QTIMER_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_BCR_ADDR, HWIO_SSC_SCC_QTIMER_BCR_RMSK)
#define HWIO_SSC_SCC_QTIMER_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_BCR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_BCR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_BCR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_BCR_IN)
#define HWIO_SSC_SCC_QTIMER_BCR_BLK_ARES_BMSK                                             0x1
#define HWIO_SSC_SCC_QTIMER_BCR_BLK_ARES_SHFT                                             0x0

#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009060)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_RMSK                                     0x1ff
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR, HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_CLK_DIV_BMSK                             0x1ff
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_CLK_DIV_SHFT                               0x0

#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009064)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_RMSK                                          0x80000005
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR, HWIO_SSC_SCC_QTIMER_TMR_CBCR_RMSK)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_TMR_CBCR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_TMR_CBCR_IN)
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_OFF_BMSK                                  0x80000000
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_OFF_SHFT                                        0x1f
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ARES_BMSK                                        0x4
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ARES_SHFT                                        0x2
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_BMSK                                      0x1
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_CLK_ENABLE_SHFT                                      0x0

#define HWIO_SSC_SCC_QUPV3_BUS_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009068)
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_BUS_BCR_ADDR, HWIO_SSC_SCC_QUPV3_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_BUS_BCR_IN)
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000906c)
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_RMSK                                        0x80000007
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_HW_CTL_BMSK                                        0x2
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_HW_CTL_SHFT                                        0x1
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009070)
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_RMSK                                        0x80000007
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_ADDR, HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_HW_CTL_BMSK                                        0x2
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_HW_CTL_SHFT                                        0x1
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_RSC_HCLK_BCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009074)
#define HWIO_SSC_SCC_RSC_HCLK_BCR_RMSK                                                    0x1
#define HWIO_SSC_SCC_RSC_HCLK_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_RSC_HCLK_BCR_ADDR, HWIO_SSC_SCC_RSC_HCLK_BCR_RMSK)
#define HWIO_SSC_SCC_RSC_HCLK_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_RSC_HCLK_BCR_ADDR, m)
#define HWIO_SSC_SCC_RSC_HCLK_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_RSC_HCLK_BCR_ADDR,v)
#define HWIO_SSC_SCC_RSC_HCLK_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_RSC_HCLK_BCR_ADDR,m,v,HWIO_SSC_SCC_RSC_HCLK_BCR_IN)
#define HWIO_SSC_SCC_RSC_HCLK_BCR_BLK_ARES_BMSK                                           0x1
#define HWIO_SSC_SCC_RSC_HCLK_BCR_BLK_ARES_SHFT                                           0x0

#define HWIO_SSC_SCC_RSC_HCLK_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009078)
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_RMSK                                            0x80000007
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_RSC_HCLK_CBCR_ADDR, HWIO_SSC_SCC_RSC_HCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_RSC_HCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_RSC_HCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_RSC_HCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_RSC_HCLK_CBCR_IN)
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_HW_CTL_BMSK                                            0x2
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_HW_CTL_SHFT                                            0x1
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_QTIMER_AHB_BCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000907c)
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_RMSK                                                  0x1
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_AHB_BCR_ADDR, HWIO_SSC_SCC_QTIMER_AHB_BCR_RMSK)
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_AHB_BCR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_AHB_BCR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_AHB_BCR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_AHB_BCR_IN)
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009080)
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_RMSK                                          0x80000007
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR, HWIO_SSC_SCC_QTIMER_AHB_CBCR_RMSK)
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR, m)
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR,v)
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_QTIMER_AHB_CBCR_ADDR,m,v,HWIO_SSC_SCC_QTIMER_AHB_CBCR_IN)
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_OFF_BMSK                                  0x80000000
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_OFF_SHFT                                        0x1f
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ARES_BMSK                                        0x4
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ARES_SHFT                                        0x2
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_HW_CTL_BMSK                                          0x2
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_HW_CTL_SHFT                                          0x1
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ENABLE_BMSK                                      0x1
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_CLK_ENABLE_SHFT                                      0x0

#define HWIO_SSC_SCC_NOC_BUS_BCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009084)
#define HWIO_SSC_SCC_NOC_BUS_BCR_RMSK                                                     0x1
#define HWIO_SSC_SCC_NOC_BUS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_NOC_BUS_BCR_ADDR, HWIO_SSC_SCC_NOC_BUS_BCR_RMSK)
#define HWIO_SSC_SCC_NOC_BUS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_NOC_BUS_BCR_ADDR, m)
#define HWIO_SSC_SCC_NOC_BUS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_NOC_BUS_BCR_ADDR,v)
#define HWIO_SSC_SCC_NOC_BUS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_NOC_BUS_BCR_ADDR,m,v,HWIO_SSC_SCC_NOC_BUS_BCR_IN)
#define HWIO_SSC_SCC_NOC_BUS_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_SSC_SCC_NOC_BUS_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_SSC_SCC_NOC_BUS_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00009088)
#define HWIO_SSC_SCC_NOC_BUS_CBCR_RMSK                                             0x80000006
#define HWIO_SSC_SCC_NOC_BUS_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_NOC_BUS_CBCR_ADDR, HWIO_SSC_SCC_NOC_BUS_CBCR_RMSK)
#define HWIO_SSC_SCC_NOC_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_NOC_BUS_CBCR_ADDR, m)
#define HWIO_SSC_SCC_NOC_BUS_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_NOC_BUS_CBCR_ADDR,v)
#define HWIO_SSC_SCC_NOC_BUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_NOC_BUS_CBCR_ADDR,m,v,HWIO_SSC_SCC_NOC_BUS_CBCR_IN)
#define HWIO_SSC_SCC_NOC_BUS_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_NOC_BUS_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_NOC_BUS_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_NOC_BUS_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_NOC_BUS_CBCR_HW_CTL_BMSK                                             0x2
#define HWIO_SSC_SCC_NOC_BUS_CBCR_HW_CTL_SHFT                                             0x1

#define HWIO_SSC_SCC_DBG_BCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a000)
#define HWIO_SSC_SCC_DBG_BCR_RMSK                                                         0x1
#define HWIO_SSC_SCC_DBG_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DBG_BCR_ADDR, HWIO_SSC_SCC_DBG_BCR_RMSK)
#define HWIO_SSC_SCC_DBG_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DBG_BCR_ADDR, m)
#define HWIO_SSC_SCC_DBG_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DBG_BCR_ADDR,v)
#define HWIO_SSC_SCC_DBG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DBG_BCR_ADDR,m,v,HWIO_SSC_SCC_DBG_BCR_IN)
#define HWIO_SSC_SCC_DBG_BCR_BLK_ARES_BMSK                                                0x1
#define HWIO_SSC_SCC_DBG_BCR_BLK_ARES_SHFT                                                0x0

#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_ADDR                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a004)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_RMSK                                     0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_CLK_DIV_BMSK                             0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_CLK_DIV_SHFT                             0x0

#define HWIO_SSC_SCC_AT_CBCR_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a008)
#define HWIO_SSC_SCC_AT_CBCR_RMSK                                                  0x80007ff5
#define HWIO_SSC_SCC_AT_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_AT_CBCR_ADDR, HWIO_SSC_SCC_AT_CBCR_RMSK)
#define HWIO_SSC_SCC_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_AT_CBCR_ADDR, m)
#define HWIO_SSC_SCC_AT_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_AT_CBCR_ADDR,v)
#define HWIO_SSC_SCC_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_AT_CBCR_ADDR,m,v,HWIO_SSC_SCC_AT_CBCR_IN)
#define HWIO_SSC_SCC_AT_CBCR_CLK_OFF_BMSK                                          0x80000000
#define HWIO_SSC_SCC_AT_CBCR_CLK_OFF_SHFT                                                0x1f
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_CORE_ON_BMSK                                    0x4000
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_CORE_ON_SHFT                                       0xe
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                  0x2000
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                     0xd
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                 0x1000
#define HWIO_SSC_SCC_AT_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                    0xc
#define HWIO_SSC_SCC_AT_CBCR_WAKEUP_BMSK                                                0xf00
#define HWIO_SSC_SCC_AT_CBCR_WAKEUP_SHFT                                                  0x8
#define HWIO_SSC_SCC_AT_CBCR_SLEEP_BMSK                                                  0xf0
#define HWIO_SSC_SCC_AT_CBCR_SLEEP_SHFT                                                   0x4
#define HWIO_SSC_SCC_AT_CBCR_CLK_ARES_BMSK                                                0x4
#define HWIO_SSC_SCC_AT_CBCR_CLK_ARES_SHFT                                                0x2
#define HWIO_SSC_SCC_AT_CBCR_CLK_ENABLE_BMSK                                              0x1
#define HWIO_SSC_SCC_AT_CBCR_CLK_ENABLE_SHFT                                              0x0

#define HWIO_SSC_SCC_Q6_ATBM_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a00c)
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_ATBM_CBCR_ADDR, HWIO_SSC_SCC_Q6_ATBM_CBCR_RMSK)
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_ATBM_CBCR_ADDR, m)
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_ATBM_CBCR_ADDR,v)
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_ATBM_CBCR_ADDR,m,v,HWIO_SSC_SCC_Q6_ATBM_CBCR_IN)
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_PCLKDBG_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a010)
#define HWIO_SSC_SCC_PCLKDBG_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_PCLKDBG_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_PCLKDBG_CBCR_ADDR, HWIO_SSC_SCC_PCLKDBG_CBCR_RMSK)
#define HWIO_SSC_SCC_PCLKDBG_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PCLKDBG_CBCR_ADDR, m)
#define HWIO_SSC_SCC_PCLKDBG_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PCLKDBG_CBCR_ADDR,v)
#define HWIO_SSC_SCC_PCLKDBG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PCLKDBG_CBCR_ADDR,m,v,HWIO_SSC_SCC_PCLKDBG_CBCR_IN)
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_PCLKDBG_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_VS_BCR_ADDR                                                   (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a014)
#define HWIO_SSC_SCC_VS_BCR_RMSK                                                          0x1
#define HWIO_SSC_SCC_VS_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_BCR_ADDR, HWIO_SSC_SCC_VS_BCR_RMSK)
#define HWIO_SSC_SCC_VS_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_BCR_ADDR, m)
#define HWIO_SSC_SCC_VS_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_BCR_ADDR,v)
#define HWIO_SSC_SCC_VS_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_BCR_ADDR,m,v,HWIO_SSC_SCC_VS_BCR_IN)
#define HWIO_SSC_SCC_VS_BCR_BLK_ARES_BMSK                                                 0x1
#define HWIO_SSC_SCC_VS_BCR_BLK_ARES_SHFT                                                 0x0

#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a018)
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_RMSK                                        0x80000013
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ADDR, HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_IN)
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                               0x10
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                0x4
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a01c)
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_RMSK                                          0x11071f
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_ADDR, HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_IN)
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_HW_CLK_CONTROL_BMSK                           0x100000
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_HW_CLK_CONTROL_SHFT                               0x14
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_RCGLITE_DISABLE_BMSK                           0x10000
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_RCGLITE_DISABLE_SHFT                              0x10
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a030)
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_RMSK                                        0x80000013
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ADDR, HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_RMSK)
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_IN)
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                               0x10
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                0x4
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a034)
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_RMSK                                          0x11071f
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_ADDR, HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_RMSK)
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_IN)
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_HW_CLK_CONTROL_BMSK                           0x100000
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_HW_CLK_CONTROL_SHFT                               0x14
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_RCGLITE_DISABLE_BMSK                           0x10000
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_RCGLITE_DISABLE_SHFT                              0x10
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_SSC_SCC_VS_VDDMX_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a048)
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_RMSK                                            0x80000005
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CBCR_ADDR, HWIO_SSC_SCC_VS_VDDMX_CBCR_RMSK)
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDMX_CBCR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDMX_CBCR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDMX_CBCR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDMX_CBCR_IN)
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_VS_VDDCX_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a04c)
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_RMSK                                            0x80000005
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CBCR_ADDR, HWIO_SSC_SCC_VS_VDDCX_CBCR_RMSK)
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_VS_VDDCX_CBCR_ADDR, m)
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_VS_VDDCX_CBCR_ADDR,v)
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_VS_VDDCX_CBCR_ADDR,m,v,HWIO_SSC_SCC_VS_VDDCX_CBCR_IN)
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_DBG_TSCTR_BCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a050)
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_RMSK                                                   0x1
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DBG_TSCTR_BCR_ADDR, HWIO_SSC_SCC_DBG_TSCTR_BCR_RMSK)
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DBG_TSCTR_BCR_ADDR, m)
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DBG_TSCTR_BCR_ADDR,v)
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DBG_TSCTR_BCR_ADDR,m,v,HWIO_SSC_SCC_DBG_TSCTR_BCR_IN)
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_BLK_ARES_BMSK                                          0x1
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_BLK_ARES_SHFT                                          0x0

#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_ADDR                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a054)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_RMSK                               0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_ADDR, HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_CLK_DIV_BMSK                       0x7
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_CLK_DIV_SHFT                       0x0

#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a058)
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_RMSK                                           0x80000005
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DBG_TSCTR_CBCR_ADDR, HWIO_SSC_SCC_DBG_TSCTR_CBCR_RMSK)
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DBG_TSCTR_CBCR_ADDR, m)
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DBG_TSCTR_CBCR_ADDR,v)
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DBG_TSCTR_CBCR_ADDR,m,v,HWIO_SSC_SCC_DBG_TSCTR_CBCR_IN)
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_SSC_SCC_SDC_DBG_BCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a05c)
#define HWIO_SSC_SCC_SDC_DBG_BCR_RMSK                                                     0x1
#define HWIO_SSC_SCC_SDC_DBG_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_DBG_BCR_ADDR, HWIO_SSC_SCC_SDC_DBG_BCR_RMSK)
#define HWIO_SSC_SCC_SDC_DBG_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_DBG_BCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_DBG_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_DBG_BCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_DBG_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_DBG_BCR_ADDR,m,v,HWIO_SSC_SCC_SDC_DBG_BCR_IN)
#define HWIO_SSC_SCC_SDC_DBG_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_SSC_SCC_SDC_DBG_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a060)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_SDC_DAP_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR, HWIO_SSC_SCC_SDC_DAP_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_DAP_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_DAP_CBCR_IN)
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_SDC_DAP_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a064)
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_RMSK                                        0x80000005
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_ADDR, HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_IN)
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_SSC_SCC_SDC_CTI_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a068)
#define HWIO_SSC_SCC_SDC_CTI_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_SDC_CTI_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_CTI_CBCR_ADDR, HWIO_SSC_SCC_SDC_CTI_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_CTI_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_CTI_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_CTI_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_CTI_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_CTI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_CTI_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_CTI_CBCR_IN)
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_SDC_CTI_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_SDC_QDSS_CBCR_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a06c)
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_RMSK                                            0x80000005
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_QDSS_CBCR_ADDR, HWIO_SSC_SCC_SDC_QDSS_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_QDSS_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_QDSS_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_QDSS_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_QDSS_CBCR_IN)
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_SSC_SCC_SDC_TS_CBCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a070)
#define HWIO_SSC_SCC_SDC_TS_CBCR_RMSK                                              0x80000005
#define HWIO_SSC_SCC_SDC_TS_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_TS_CBCR_ADDR, HWIO_SSC_SCC_SDC_TS_CBCR_RMSK)
#define HWIO_SSC_SCC_SDC_TS_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_TS_CBCR_ADDR, m)
#define HWIO_SSC_SCC_SDC_TS_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_TS_CBCR_ADDR,v)
#define HWIO_SSC_SCC_SDC_TS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_TS_CBCR_ADDR,m,v,HWIO_SSC_SCC_SDC_TS_CBCR_IN)
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_ARES_BMSK                                            0x4
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_ARES_SHFT                                            0x2
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_SSC_SCC_SDC_TS_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_SSC_SCC_NOC_ATB_BCR_ADDR                                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a074)
#define HWIO_SSC_SCC_NOC_ATB_BCR_RMSK                                                     0x1
#define HWIO_SSC_SCC_NOC_ATB_BCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_NOC_ATB_BCR_ADDR, HWIO_SSC_SCC_NOC_ATB_BCR_RMSK)
#define HWIO_SSC_SCC_NOC_ATB_BCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_NOC_ATB_BCR_ADDR, m)
#define HWIO_SSC_SCC_NOC_ATB_BCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_NOC_ATB_BCR_ADDR,v)
#define HWIO_SSC_SCC_NOC_ATB_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_NOC_ATB_BCR_ADDR,m,v,HWIO_SSC_SCC_NOC_ATB_BCR_IN)
#define HWIO_SSC_SCC_NOC_ATB_BCR_BLK_ARES_BMSK                                            0x1
#define HWIO_SSC_SCC_NOC_ATB_BCR_BLK_ARES_SHFT                                            0x0

#define HWIO_SSC_SCC_NOC_ATB_CBCR_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a078)
#define HWIO_SSC_SCC_NOC_ATB_CBCR_RMSK                                             0x80000005
#define HWIO_SSC_SCC_NOC_ATB_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_NOC_ATB_CBCR_ADDR, HWIO_SSC_SCC_NOC_ATB_CBCR_RMSK)
#define HWIO_SSC_SCC_NOC_ATB_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_NOC_ATB_CBCR_ADDR, m)
#define HWIO_SSC_SCC_NOC_ATB_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_NOC_ATB_CBCR_ADDR,v)
#define HWIO_SSC_SCC_NOC_ATB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_NOC_ATB_CBCR_ADDR,m,v,HWIO_SSC_SCC_NOC_ATB_CBCR_IN)
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_SSC_SCC_NOC_ATB_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR                              (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000080)
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_RMSK                                     0xf
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR, HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_CLK_DIV_BMSK                             0xf
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_CLK_DIV_SHFT                             0x0

#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a080)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_RMSK                                         0x80000005
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR, HWIO_SSC_SCC_PLL_LV_TEST_CBCR_RMSK)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR, m)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR,v)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR,m,v,HWIO_SSC_SCC_PLL_LV_TEST_CBCR_IN)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_OFF_BMSK                                 0x80000000
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_OFF_SHFT                                       0x1f
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_ARES_BMSK                                       0x4
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_ARES_SHFT                                       0x2
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_ENABLE_BMSK                                     0x1
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_CLK_ENABLE_SHFT                                     0x0

#define HWIO_SSC_SCC_DEBUG_CBCR_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a084)
#define HWIO_SSC_SCC_DEBUG_CBCR_RMSK                                               0x80000005
#define HWIO_SSC_SCC_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_SSC_SCC_DEBUG_CBCR_ADDR, HWIO_SSC_SCC_DEBUG_CBCR_RMSK)
#define HWIO_SSC_SCC_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DEBUG_CBCR_ADDR, m)
#define HWIO_SSC_SCC_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DEBUG_CBCR_ADDR,v)
#define HWIO_SSC_SCC_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DEBUG_CBCR_ADDR,m,v,HWIO_SSC_SCC_DEBUG_CBCR_IN)
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_ARES_SHFT                                             0x2
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_SSC_SCC_DEBUG_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_SSC_SCC_Q6_MISC_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000054)
#define HWIO_SSC_SCC_Q6_MISC_RMSK                                                         0x1
#define HWIO_SSC_SCC_Q6_MISC_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_MISC_ADDR, HWIO_SSC_SCC_Q6_MISC_RMSK)
#define HWIO_SSC_SCC_Q6_MISC_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_MISC_ADDR, m)
#define HWIO_SSC_SCC_Q6_MISC_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_MISC_ADDR,v)
#define HWIO_SSC_SCC_Q6_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_MISC_ADDR,m,v,HWIO_SSC_SCC_Q6_MISC_IN)
#define HWIO_SSC_SCC_Q6_MISC_Q6_REF_CLK_SEL_BMSK                                          0x1
#define HWIO_SSC_SCC_Q6_MISC_Q6_REF_CLK_SEL_SHFT                                          0x0

#define HWIO_SSC_SCC_CBC_MISC_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000058)
#define HWIO_SSC_SCC_CBC_MISC_RMSK                                                        0x1
#define HWIO_SSC_SCC_CBC_MISC_IN          \
        in_dword_masked(HWIO_SSC_SCC_CBC_MISC_ADDR, HWIO_SSC_SCC_CBC_MISC_RMSK)
#define HWIO_SSC_SCC_CBC_MISC_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_CBC_MISC_ADDR, m)
#define HWIO_SSC_SCC_CBC_MISC_OUT(v)      \
        out_dword(HWIO_SSC_SCC_CBC_MISC_ADDR,v)
#define HWIO_SSC_SCC_CBC_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_CBC_MISC_ADDR,m,v,HWIO_SSC_SCC_CBC_MISC_IN)
#define HWIO_SSC_SCC_CBC_MISC_SCC_IDDQ_ALL_CBC_DIS_BMSK                                   0x1
#define HWIO_SSC_SCC_CBC_MISC_SCC_IDDQ_ALL_CBC_DIS_SHFT                                   0x0

#define HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000005c)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_RMSK                                                0xfffe
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_IN          \
        in_dword_masked(HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR, HWIO_SSC_SCC_DEBUG_CLK_CTL_RMSK)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR, m)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR,v)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR,m,v,HWIO_SSC_SCC_DEBUG_CLK_CTL_IN)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_TEST_POST_DIV_BMSK                              0xf000
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_TEST_POST_DIV_SHFT                                 0xc
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_TEST_SEL_BMSK                                    0xc00
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_TEST_SEL_SHFT                                      0xa
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_BYPASSNL_MUX_SEL_BMSK                            0x200
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_BYPASSNL_MUX_SEL_SHFT                              0x9
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_RESETN_MUX_SEL_BMSK                              0x180
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_PLL_RESETN_MUX_SEL_SHFT                                0x7
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_MUX_SEL_BMSK                                          0x7e
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_MUX_SEL_SHFT                                           0x1

#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000050)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_RMSK                                              0x3f
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_IN          \
        in_dword_masked(HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR, HWIO_SSC_SCC_AON_CTRL_OVRD_REG_RMSK)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR, m)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_OUT(v)      \
        out_dword(HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR,v)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR,m,v,HWIO_SSC_SCC_AON_CTRL_OVRD_REG_IN)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_SLEEP_CLK_SEL_OVRD_VAL_BMSK                       0x20
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_SLEEP_CLK_SEL_OVRD_VAL_SHFT                        0x5
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_SLEEP_CLK_SEL_OVRD_BMSK                           0x10
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_SLEEP_CLK_SEL_OVRD_SHFT                            0x4
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_CBC_EN_OVRD_VAL_BMSK                               0x8
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_CBC_EN_OVRD_VAL_SHFT                               0x3
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_CBC_EN_OVRD_BMSK                                   0x4
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_CBC_EN_OVRD_SHFT                                   0x2
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_PLL_OUT_CTRL_OVRD_BMSK                             0x2
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_PLL_OUT_CTRL_OVRD_SHFT                             0x1
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_PLL_OPMODE_CTRL_OVRD_BMSK                          0x1
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_PLL_OPMODE_CTRL_OVRD_SHFT                          0x0

#define HWIO_SSC_SCC_SCC_MISC_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000060)
#define HWIO_SSC_SCC_SCC_MISC_RMSK                                                        0x1
#define HWIO_SSC_SCC_SCC_MISC_IN          \
        in_dword_masked(HWIO_SSC_SCC_SCC_MISC_ADDR, HWIO_SSC_SCC_SCC_MISC_RMSK)
#define HWIO_SSC_SCC_SCC_MISC_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SCC_MISC_ADDR, m)
#define HWIO_SSC_SCC_SCC_MISC_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SCC_MISC_ADDR,v)
#define HWIO_SSC_SCC_SCC_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SCC_MISC_ADDR,m,v,HWIO_SSC_SCC_SCC_MISC_IN)
#define HWIO_SSC_SCC_SCC_MISC_SCC_REF_CLK_SEL_BMSK                                        0x1
#define HWIO_SSC_SCC_SCC_MISC_SCC_REF_CLK_SEL_SHFT                                        0x0

#define HWIO_SSC_SCC_SPARE_1_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0001f000)
#define HWIO_SSC_SCC_SPARE_1_RMSK                                                  0xffffffff
#define HWIO_SSC_SCC_SPARE_1_IN          \
        in_dword_masked(HWIO_SSC_SCC_SPARE_1_ADDR, HWIO_SSC_SCC_SPARE_1_RMSK)
#define HWIO_SSC_SCC_SPARE_1_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SPARE_1_ADDR, m)
#define HWIO_SSC_SCC_SPARE_1_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SPARE_1_ADDR,v)
#define HWIO_SSC_SCC_SPARE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SPARE_1_ADDR,m,v,HWIO_SSC_SCC_SPARE_1_IN)
#define HWIO_SSC_SCC_SPARE_1_SCC_SPARE_IN_BMSK                                     0xffff0000
#define HWIO_SSC_SCC_SPARE_1_SCC_SPARE_IN_SHFT                                           0x10
#define HWIO_SSC_SCC_SPARE_1_SCC_SPARE_OUT_BMSK                                        0xffff
#define HWIO_SSC_SCC_SPARE_1_SCC_SPARE_OUT_SHFT                                           0x0

#define HWIO_SSC_SCC_SPARE_2_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0001e000)
#define HWIO_SSC_SCC_SPARE_2_RMSK                                                  0xffffffff
#define HWIO_SSC_SCC_SPARE_2_IN          \
        in_dword_masked(HWIO_SSC_SCC_SPARE_2_ADDR, HWIO_SSC_SCC_SPARE_2_RMSK)
#define HWIO_SSC_SCC_SPARE_2_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SPARE_2_ADDR, m)
#define HWIO_SSC_SCC_SPARE_2_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SPARE_2_ADDR,v)
#define HWIO_SSC_SCC_SPARE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SPARE_2_ADDR,m,v,HWIO_SSC_SCC_SPARE_2_IN)
#define HWIO_SSC_SCC_SPARE_2_SCC_SPARE_IN_BMSK                                     0xffff0000
#define HWIO_SSC_SCC_SPARE_2_SCC_SPARE_IN_SHFT                                           0x10
#define HWIO_SSC_SCC_SPARE_2_SCC_SPARE_OUT_BMSK                                        0xffff
#define HWIO_SSC_SCC_SPARE_2_SCC_SPARE_OUT_SHFT                                           0x0

#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_ADDR                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00020000)
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_RMSK                                       0xfff
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_IN          \
        in_dword_masked(HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_ADDR, HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_RMSK)
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_ADDR, m)
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_ADDR,v)
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_ADDR,m,v,HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_IN)
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_BMSK                    0x800
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_SHFT                      0xb
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_BMSK                 0x400
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_SHFT                   0xa
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_BMSK                     0x200
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_SHFT                       0x9
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_BMSK                     0x100
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_SHFT                       0x8
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_BMSK                      0x80
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_SHFT                       0x7
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_BMSK                      0x40
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_SHFT                       0x6
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_BMSK                      0x20
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_SHFT                       0x5
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_BMSK                      0x10
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_SHFT                       0x4
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_BMSK                       0x8
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_SHFT                       0x3
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_BMSK                     0x4
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_SHFT                     0x2
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_BMSK                        0x2
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_SHFT                        0x1
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_BMSK                        0x1
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_SHFT                        0x0

#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_ADDR                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00021000)
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_RMSK                                     0xfff
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_IN          \
        in_dword_masked(HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_ADDR, HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_RMSK)
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_ADDR, m)
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_ADDR,v)
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_ADDR,m,v,HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_IN)
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_BMSK                  0x800
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_SHFT                    0xb
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_BMSK               0x400
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_SHFT                 0xa
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_BMSK                   0x200
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_SHFT                     0x9
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_BMSK                   0x100
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_SHFT                     0x8
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_BMSK                    0x80
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_SHFT                     0x7
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_BMSK                    0x40
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_SHFT                     0x6
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_BMSK                    0x20
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_SHFT                     0x5
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_BMSK                    0x10
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_SHFT                     0x4
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_BMSK                     0x8
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_SHFT                     0x3
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_BMSK                   0x4
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_SHFT                   0x2
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_BMSK                      0x2
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_SHFT                      0x1
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_BMSK                      0x1
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_SHFT                      0x0

#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_ADDR                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00022000)
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_RMSK                                      0xfff
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_IN          \
        in_dword_masked(HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_ADDR, HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_RMSK)
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_ADDR, m)
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_ADDR,v)
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_ADDR,m,v,HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_IN)
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_BMSK                   0x800
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_SHFT                     0xb
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_BMSK                0x400
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_SHFT                  0xa
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_BMSK                    0x200
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_SHFT                      0x9
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_BMSK                    0x100
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_SHFT                      0x8
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_BMSK                     0x80
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_SHFT                      0x7
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_BMSK                     0x40
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_SHFT                      0x6
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_BMSK                     0x20
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_SHFT                      0x5
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_BMSK                     0x10
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_SHFT                      0x4
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_BMSK                      0x8
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_SHFT                      0x3
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_BMSK                    0x4
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_SHFT                    0x2
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_BMSK                       0x2
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_SHFT                       0x1
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_BMSK                       0x1
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_SHFT                       0x0

#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_ADDR                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00023000)
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_RMSK                                       0xfff
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_IN          \
        in_dword_masked(HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_ADDR, HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_RMSK)
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_ADDR, m)
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_OUT(v)      \
        out_dword(HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_ADDR,v)
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_ADDR,m,v,HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_IN)
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_BMSK                    0x800
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_CORE_CLK_ENA_SHFT                      0xb
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_BMSK                 0x400
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_2X_CORE_CLK_ENA_SHFT                   0xa
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_BMSK                     0x200
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE6_CLK_ENA_SHFT                       0x9
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_BMSK                     0x100
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE5_CLK_ENA_SHFT                       0x8
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_BMSK                      0x80
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE4_CLK_ENA_SHFT                       0x7
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_BMSK                      0x40
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE3_CLK_ENA_SHFT                       0x6
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_BMSK                      0x20
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE2_CLK_ENA_SHFT                       0x5
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_BMSK                      0x10
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE1_CLK_ENA_SHFT                       0x4
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_BMSK                       0x8
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SE0_CLK_ENA_SHFT                       0x3
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_BMSK                     0x4
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_SLEEP_CLK_ENA_SHFT                     0x2
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_BMSK                        0x2
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_M_HCLK_ENA_SHFT                        0x1
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_BMSK                        0x1
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_QUPV3_S_HCLK_ENA_SHFT                        0x0

#endif /* __SSC_QTMR__ */
