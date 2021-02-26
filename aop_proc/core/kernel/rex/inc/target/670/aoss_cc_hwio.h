#ifndef __HWIO_H__
#define __HWIO_H__
/*
===========================================================================
*/
/**
  @file hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r103_MTO_INCR]
 
  This file contains HWIO register definitions for the following modules:
    AOSS_CC_AOSS_CC_REG


  Generation parameters: 
  { u'filename': 'hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'AOSS_CC_AOSS_CC_REG']}
*/
/*
  ===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/target/670/aoss_cc_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/
#include "msmhwiobase.h"
/*----------------------------------------------------------------------------
 * MODULE: AOSS_CC_AOSS_CC_REG
 *--------------------------------------------------------------------------*/

#define AOSS_CC_AOSS_CC_REG_REG_BASE                                              (AOSS_BASE      + 0x012a0000)

#define HWIO_AOSS_CC_PLL0_MODE_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000000)
#define HWIO_AOSS_CC_PLL0_MODE_RMSK                                               0xffffffff
#define HWIO_AOSS_CC_PLL0_MODE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_MODE_ADDR, HWIO_AOSS_CC_PLL0_MODE_RMSK)
#define HWIO_AOSS_CC_PLL0_MODE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_MODE_ADDR, m)
#define HWIO_AOSS_CC_PLL0_MODE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_MODE_ADDR,v)
#define HWIO_AOSS_CC_PLL0_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_MODE_ADDR,m,v,HWIO_AOSS_CC_PLL0_MODE_IN)
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_DET_BMSK                                  0x80000000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_DET_SHFT                                        0x1f
#define HWIO_AOSS_CC_PLL0_MODE_PLL_ACTIVE_FLAG_BMSK                               0x40000000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_ACTIVE_FLAG_SHFT                                     0x1e
#define HWIO_AOSS_CC_PLL0_MODE_PLL_ACK_LATCH_BMSK                                 0x20000000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_ACK_LATCH_SHFT                                       0x1d
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_DET_FINE_BMSK                             0x10000000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_DET_FINE_SHFT                                   0x1c
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BITS27_25_BMSK                              0xe000000
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BITS27_25_SHFT                                   0x19
#define HWIO_AOSS_CC_PLL0_MODE_FSM_LEGACY_MODE_BMSK                                0x1000000
#define HWIO_AOSS_CC_PLL0_MODE_FSM_LEGACY_MODE_SHFT                                     0x18
#define HWIO_AOSS_CC_PLL0_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                      0x800000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                          0x17
#define HWIO_AOSS_CC_PLL0_MODE_PLL_UPDATE_BMSK                                      0x400000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_UPDATE_SHFT                                          0x16
#define HWIO_AOSS_CC_PLL0_MODE_PLL_VOTE_FSM_RESET_BMSK                              0x200000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_VOTE_FSM_RESET_SHFT                                  0x15
#define HWIO_AOSS_CC_PLL0_MODE_PLL_VOTE_FSM_ENA_BMSK                                0x100000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_VOTE_FSM_ENA_SHFT                                    0x14
#define HWIO_AOSS_CC_PLL0_MODE_PLL_BIAS_COUNT_BMSK                                   0xfc000
#define HWIO_AOSS_CC_PLL0_MODE_PLL_BIAS_COUNT_SHFT                                       0xe
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_COUNT_BMSK                                    0x3f00
#define HWIO_AOSS_CC_PLL0_MODE_PLL_LOCK_COUNT_SHFT                                       0x8
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BITS7_3_BMSK                                     0xf8
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BITS7_3_SHFT                                      0x3
#define HWIO_AOSS_CC_PLL0_MODE_PLL_RESET_N_BMSK                                          0x4
#define HWIO_AOSS_CC_PLL0_MODE_PLL_RESET_N_SHFT                                          0x2
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BIT1_BMSK                                         0x2
#define HWIO_AOSS_CC_PLL0_MODE_RESERVE_BIT1_SHFT                                         0x1
#define HWIO_AOSS_CC_PLL0_MODE_PLL_OUTCTRL_BMSK                                          0x1
#define HWIO_AOSS_CC_PLL0_MODE_PLL_OUTCTRL_SHFT                                          0x0

#define HWIO_AOSS_CC_PLL0_L_VAL_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000004)
#define HWIO_AOSS_CC_PLL0_L_VAL_RMSK                                                  0xffff
#define HWIO_AOSS_CC_PLL0_L_VAL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_L_VAL_ADDR, HWIO_AOSS_CC_PLL0_L_VAL_RMSK)
#define HWIO_AOSS_CC_PLL0_L_VAL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_L_VAL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_L_VAL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_L_VAL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_L_VAL_ADDR,m,v,HWIO_AOSS_CC_PLL0_L_VAL_IN)
#define HWIO_AOSS_CC_PLL0_L_VAL_PLL_L_BMSK                                            0xffff
#define HWIO_AOSS_CC_PLL0_L_VAL_PLL_L_SHFT                                               0x0

#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000008)
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_RMSK                                              0xffff
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CAL_L_VAL_ADDR, HWIO_AOSS_CC_PLL0_CAL_L_VAL_RMSK)
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CAL_L_VAL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_CAL_L_VAL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_CAL_L_VAL_ADDR,m,v,HWIO_AOSS_CC_PLL0_CAL_L_VAL_IN)
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_PLL_CAL_L_BMSK                                    0xffff
#define HWIO_AOSS_CC_PLL0_CAL_L_VAL_PLL_CAL_L_SHFT                                       0x0

#define HWIO_AOSS_CC_PLL0_USER_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000000c)
#define HWIO_AOSS_CC_PLL0_USER_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL0_USER_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_USER_CTL_ADDR, HWIO_AOSS_CC_PLL0_USER_CTL_RMSK)
#define HWIO_AOSS_CC_PLL0_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_USER_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_USER_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_USER_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_USER_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL0_USER_CTL_IN)
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BITS31_19_BMSK                         0xfff80000
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BITS31_19_SHFT                               0x13
#define HWIO_AOSS_CC_PLL0_USER_CTL_PRE_DIV_RATIO_BMSK                                0x70000
#define HWIO_AOSS_CC_PLL0_USER_CTL_PRE_DIV_RATIO_SHFT                                   0x10
#define HWIO_AOSS_CC_PLL0_USER_CTL_POST_DIV_RATIO_ODD_BMSK                            0xf000
#define HWIO_AOSS_CC_PLL0_USER_CTL_POST_DIV_RATIO_ODD_SHFT                               0xc
#define HWIO_AOSS_CC_PLL0_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                            0xf00
#define HWIO_AOSS_CC_PLL0_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                              0x8
#define HWIO_AOSS_CC_PLL0_USER_CTL_OUT_CLK_POLARITY_BMSK                                0x80
#define HWIO_AOSS_CC_PLL0_USER_CTL_OUT_CLK_POLARITY_SHFT                                 0x7
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BITS6_5_BMSK                                 0x60
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BITS6_5_SHFT                                  0x5
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_TEST_BMSK                                     0x10
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_TEST_SHFT                                      0x4
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BIT3_BMSK                                     0x8
#define HWIO_AOSS_CC_PLL0_USER_CTL_RESERVE_BIT3_SHFT                                     0x3
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_ODD_BMSK                                       0x4
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_ODD_SHFT                                       0x2
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_EVEN_BMSK                                      0x2
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_EVEN_SHFT                                      0x1
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_MAIN_BMSK                                      0x1
#define HWIO_AOSS_CC_PLL0_USER_CTL_PLLOUT_MAIN_SHFT                                      0x0

#define HWIO_AOSS_CC_PLL0_USER_CTL_U_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000010)
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_USER_CTL_U_ADDR, HWIO_AOSS_CC_PLL0_USER_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_USER_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_USER_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_USER_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL0_USER_CTL_U_IN)
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DISABLE_DECAPS_BMSK                          0x80000000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DISABLE_DECAPS_SHFT                                0x1f
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DISABLE_SHUNT_BMSK                           0x40000000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DISABLE_SHUNT_SHFT                                 0x1e
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_RESERVE_BITS61_54_BMSK                       0x3fc00000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_RESERVE_BITS61_54_SHFT                             0x16
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_BMSK             0x200000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_FREEZE_LOCK_DETECTOR_IN_DROOP_SHFT                 0x15
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DROOP_DIVIDER_SEL_BMSK                         0x100000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_DROOP_DIVIDER_SEL_SHFT                             0x14
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_SCALE_FREQ_ON_DROOP_BMSK                        0x80000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_SCALE_FREQ_ON_DROOP_SHFT                           0x13
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                        0x40000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                           0x12
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                        0x20000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                           0x11
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_LOW_JITTER_MODE_EN_BMSK                         0x10000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_LOW_JITTER_MODE_EN_SHFT                            0x10
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                             0x8000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                                0xf
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                          0x4000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                             0xe
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_REF_CLK_AT_OUT_BMSK                              0x2000
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                 0xd
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                       0x1800
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                          0xb
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                       0x400
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                         0xa
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_STATUS_REGISTER_BMSK                              0x3e0
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_STATUS_REGISTER_SHFT                                0x5
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_WRITE_STATE_EN_BMSK                                0x10
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_WRITE_STATE_EN_SHFT                                 0x4
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_CALIB_CTRL_BMSK                                     0xe
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_CALIB_CTRL_SHFT                                     0x1
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                         0x1
#define HWIO_AOSS_CC_PLL0_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                         0x0

#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000014)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CONFIG_CTL_ADDR, HWIO_AOSS_CC_PLL0_CONFIG_CTL_RMSK)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CONFIG_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_CONFIG_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_CONFIG_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL0_CONFIG_CTL_IN)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                      0xfc000000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                            0x1a
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                     0x3c00000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                          0x16
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK             0x3c0000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                 0x12
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK              0x3c000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                  0xe
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                     0x3800
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                        0xb
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                    0x700
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                      0x8
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                          0xf0
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                           0x4
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                   0xf
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                   0x0

#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000018)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_RMSK                                       0xffffffff
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ADDR, HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_IN)
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIST_CFG_BMSK                              0xfff00000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIST_CFG_SHFT                                    0x14
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ENABLEOVERPST_BMSK                            0x80000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_ENABLEOVERPST_SHFT                               0x13
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIASSELPST_BMSK                               0x40000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIASSELPST_SHFT                                  0x12
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                             0x30000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                0x10
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_BMSK                        0x8000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_DIS_DMET_DROOP_DIV_SHFT                           0xf
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BYAPSS_DROOP_DIV_BMSK                          0x4000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BYAPSS_DROOP_DIV_SHFT                             0xe
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                         0x3000
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                            0xc
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                        0xc00
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                          0xa
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                            0x380
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                              0x7
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                 0x40
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                  0x6
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                        0x30
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                         0x4
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                        0x8
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                        0x3
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                             0x6
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                             0x1
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                      0x1
#define HWIO_AOSS_CC_PLL0_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                      0x0

#define HWIO_AOSS_CC_PLL0_TEST_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000001c)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL0_TEST_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_TEST_CTL_ADDR, HWIO_AOSS_CC_PLL0_TEST_CTL_RMSK)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_TEST_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_TEST_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_TEST_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL0_TEST_CTL_IN)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                   0xc0000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                         0x1e
#define HWIO_AOSS_CC_PLL0_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK               0x30000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                     0x1c
#define HWIO_AOSS_CC_PLL0_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                      0x8000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                           0x1b
#define HWIO_AOSS_CC_PLL0_TEST_CTL_FINE_FCW_BMSK                                   0x7e00000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_FINE_FCW_SHFT                                        0x15
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                           0x100000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                               0x14
#define HWIO_AOSS_CC_PLL0_TEST_CTL_COARSE_FCW_BMSK                                   0xff000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_COARSE_FCW_SHFT                                       0xc
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                            0x800
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                              0xb
#define HWIO_AOSS_CC_PLL0_TEST_CTL_PROCESS_CALIB_WORD_BMSK                             0x700
#define HWIO_AOSS_CC_PLL0_TEST_CTL_PROCESS_CALIB_WORD_SHFT                               0x8
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                             0x80
#define HWIO_AOSS_CC_PLL0_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                              0x7
#define HWIO_AOSS_CC_PLL0_TEST_CTL_DISABLE_LFSR_BMSK                                    0x40
#define HWIO_AOSS_CC_PLL0_TEST_CTL_DISABLE_LFSR_SHFT                                     0x6
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_VREF_TEST_BMSK                                    0x20
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_VREF_TEST_SHFT                                     0x5
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_VDCO_TEST_BMSK                                    0x10
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_VDCO_TEST_SHFT                                     0x4
#define HWIO_AOSS_CC_PLL0_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                              0x8
#define HWIO_AOSS_CC_PLL0_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                              0x3
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_IVCO_TEST_BMSK                                     0x4
#define HWIO_AOSS_CC_PLL0_TEST_CTL_EN_IVCO_TEST_SHFT                                     0x2
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ATEST1_EN_BMSK                                        0x2
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ATEST1_EN_SHFT                                        0x1
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ATEST0_EN_BMSK                                        0x1
#define HWIO_AOSS_CC_PLL0_TEST_CTL_ATEST0_EN_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000020)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_TEST_CTL_U_ADDR, HWIO_AOSS_CC_PLL0_TEST_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_TEST_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_TEST_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_TEST_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL0_TEST_CTL_U_IN)
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                     0x80000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                           0x1f
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                        0x40000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                              0x1e
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                         0x20000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                               0x1d
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                        0x10000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                              0x1c
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                   0x8000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                        0x1b
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK                0x6000000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                     0x19
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                     0x1c00000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                          0x16
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                      0x200000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                          0x15
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DISABLE_CLAMP_BMSK                             0x100000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DISABLE_CLAMP_SHFT                                 0x14
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                        0x80000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                           0x13
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                    0x40000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                       0x12
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BIAS_ADJUST_BMSK                                0x30000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_BIAS_ADJUST_SHFT                                   0x10
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DIV2_NMO_EN_BMSK                                 0x8000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DIV2_NMO_EN_SHFT                                    0xf
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DIS_AMP_STARTUP_BMSK                             0x4000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DIS_AMP_STARTUP_SHFT                                0xe
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SINGLE_DMET_EN_BMSK                              0x2000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                 0xd
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                            0x1000
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                               0xc
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NMO_OSC_SEL_BMSK                                  0xc00
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NMO_OSC_SEL_SHFT                                    0xa
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NOISE_MAG_BMSK                                    0x380
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NOISE_MAG_SHFT                                      0x7
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NOISE_GEN_EN_BMSK                                  0x40
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_NOISE_GEN_EN_SHFT                                   0x6
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_OSC_BIAS_GND_BMSK                                  0x20
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_OSC_BIAS_GND_SHFT                                   0x5
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                              0x18
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                               0x3
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_STATUS_REG_EN_BMSK                                  0x4
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_STATUS_REG_EN_SHFT                                  0x2
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                 0x2
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                 0x1
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                 0x1
#define HWIO_AOSS_CC_PLL0_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                 0x0

#define HWIO_AOSS_CC_PLL0_STATUS_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000024)
#define HWIO_AOSS_CC_PLL0_STATUS_RMSK                                             0xffffffff
#define HWIO_AOSS_CC_PLL0_STATUS_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_STATUS_ADDR, HWIO_AOSS_CC_PLL0_STATUS_RMSK)
#define HWIO_AOSS_CC_PLL0_STATUS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_STATUS_ADDR, m)
#define HWIO_AOSS_CC_PLL0_STATUS_STATUS_31_0_BMSK                                 0xffffffff
#define HWIO_AOSS_CC_PLL0_STATUS_STATUS_31_0_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL0_FREQ_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000028)
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_FREQ_CTL_ADDR, HWIO_AOSS_CC_PLL0_FREQ_CTL_RMSK)
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_FREQ_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_FREQ_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_FREQ_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL0_FREQ_CTL_IN)
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                        0xffffffff
#define HWIO_AOSS_CC_PLL0_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                               0x0

#define HWIO_AOSS_CC_PLL0_OPMODE_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000002c)
#define HWIO_AOSS_CC_PLL0_OPMODE_RMSK                                                    0x7
#define HWIO_AOSS_CC_PLL0_OPMODE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_OPMODE_ADDR, HWIO_AOSS_CC_PLL0_OPMODE_RMSK)
#define HWIO_AOSS_CC_PLL0_OPMODE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_OPMODE_ADDR, m)
#define HWIO_AOSS_CC_PLL0_OPMODE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_OPMODE_ADDR,v)
#define HWIO_AOSS_CC_PLL0_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_OPMODE_ADDR,m,v,HWIO_AOSS_CC_PLL0_OPMODE_IN)
#define HWIO_AOSS_CC_PLL0_OPMODE_PLL_OPMODE_BMSK                                         0x7
#define HWIO_AOSS_CC_PLL0_OPMODE_PLL_OPMODE_SHFT                                         0x0

#define HWIO_AOSS_CC_PLL0_STATE_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000030)
#define HWIO_AOSS_CC_PLL0_STATE_RMSK                                                     0x7
#define HWIO_AOSS_CC_PLL0_STATE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_STATE_ADDR, HWIO_AOSS_CC_PLL0_STATE_RMSK)
#define HWIO_AOSS_CC_PLL0_STATE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_STATE_ADDR, m)
#define HWIO_AOSS_CC_PLL0_STATE_PLL_STATE_BMSK                                           0x7
#define HWIO_AOSS_CC_PLL0_STATE_PLL_STATE_SHFT                                           0x0

#define HWIO_AOSS_CC_PLL0_DROOP_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000034)
#define HWIO_AOSS_CC_PLL0_DROOP_RMSK                                                    0xff
#define HWIO_AOSS_CC_PLL0_DROOP_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_DROOP_ADDR, HWIO_AOSS_CC_PLL0_DROOP_RMSK)
#define HWIO_AOSS_CC_PLL0_DROOP_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_DROOP_ADDR, m)
#define HWIO_AOSS_CC_PLL0_DROOP_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_DROOP_ADDR,v)
#define HWIO_AOSS_CC_PLL0_DROOP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_DROOP_ADDR,m,v,HWIO_AOSS_CC_PLL0_DROOP_IN)
#define HWIO_AOSS_CC_PLL0_DROOP_DROOP_CODE_BMSK                                         0xfe
#define HWIO_AOSS_CC_PLL0_DROOP_DROOP_CODE_SHFT                                          0x1
#define HWIO_AOSS_CC_PLL0_DROOP_DROOP_EN_BMSK                                            0x1
#define HWIO_AOSS_CC_PLL0_DROOP_DROOP_EN_SHFT                                            0x0

#define HWIO_AOSS_CC_PLL0_FRAC_VAL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000038)
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_RMSK                                               0xffff
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_FRAC_VAL_ADDR, HWIO_AOSS_CC_PLL0_FRAC_VAL_RMSK)
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_FRAC_VAL_ADDR, m)
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_FRAC_VAL_ADDR,v)
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_FRAC_VAL_ADDR,m,v,HWIO_AOSS_CC_PLL0_FRAC_VAL_IN)
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_PLL_FRAC_VAL_BMSK                                  0xffff
#define HWIO_AOSS_CC_PLL0_FRAC_VAL_PLL_FRAC_VAL_SHFT                                     0x0

#define HWIO_AOSS_CC_PLL0_SPARE_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000003c)
#define HWIO_AOSS_CC_PLL0_SPARE_RMSK                                                    0xff
#define HWIO_AOSS_CC_PLL0_SPARE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SPARE_ADDR, HWIO_AOSS_CC_PLL0_SPARE_RMSK)
#define HWIO_AOSS_CC_PLL0_SPARE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SPARE_ADDR, m)
#define HWIO_AOSS_CC_PLL0_SPARE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_SPARE_ADDR,v)
#define HWIO_AOSS_CC_PLL0_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_SPARE_ADDR,m,v,HWIO_AOSS_CC_PLL0_SPARE_IN)
#define HWIO_AOSS_CC_PLL0_SPARE_SPARE_OUTPUTS_BMSK                                      0xf0
#define HWIO_AOSS_CC_PLL0_SPARE_SPARE_OUTPUTS_SHFT                                       0x4
#define HWIO_AOSS_CC_PLL0_SPARE_SPARE_INPUTS_BMSK                                        0xf
#define HWIO_AOSS_CC_PLL0_SPARE_SPARE_INPUTS_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_ADDR                                    (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000040)
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_RMSK                                          0xff
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_ADDR, HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_RMSK)
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_IN)
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                          0xff
#define HWIO_AOSS_CC_PLL0_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                           0x0

#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_ADDR                                    (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000044)
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_RMSK                                           0xf
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_ADDR, HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_RMSK)
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_ADDR,m,v,HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_IN)
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                           0xf
#define HWIO_AOSS_CC_PLL0_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                           0x0

#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00000048)
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_RMSK                                             0xf
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_ADDR, HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_RMSK)
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_ADDR, m)
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_ADDR,v)
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_ADDR,m,v,HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_IN)
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                               0xf
#define HWIO_AOSS_CC_PLL0_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                               0x0

#define HWIO_AOSS_CC_PLL1_MODE_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001000)
#define HWIO_AOSS_CC_PLL1_MODE_RMSK                                               0xffffffff
#define HWIO_AOSS_CC_PLL1_MODE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_MODE_ADDR, HWIO_AOSS_CC_PLL1_MODE_RMSK)
#define HWIO_AOSS_CC_PLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_MODE_ADDR, m)
#define HWIO_AOSS_CC_PLL1_MODE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_MODE_ADDR,v)
#define HWIO_AOSS_CC_PLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_MODE_ADDR,m,v,HWIO_AOSS_CC_PLL1_MODE_IN)
#define HWIO_AOSS_CC_PLL1_MODE_PLL_LOCK_DET_BMSK                                  0x80000000
#define HWIO_AOSS_CC_PLL1_MODE_PLL_LOCK_DET_SHFT                                        0x1f
#define HWIO_AOSS_CC_PLL1_MODE_PLL_ACTIVE_FLAG_BMSK                               0x40000000
#define HWIO_AOSS_CC_PLL1_MODE_PLL_ACTIVE_FLAG_SHFT                                     0x1e
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BITS29_22_BMSK                             0x3fc00000
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BITS29_22_SHFT                                   0x16
#define HWIO_AOSS_CC_PLL1_MODE_PLL_VOTE_FSM_RESET_BMSK                              0x200000
#define HWIO_AOSS_CC_PLL1_MODE_PLL_VOTE_FSM_RESET_SHFT                                  0x15
#define HWIO_AOSS_CC_PLL1_MODE_PLL_VOTE_FSM_ENA_BMSK                                0x100000
#define HWIO_AOSS_CC_PLL1_MODE_PLL_VOTE_FSM_ENA_SHFT                                    0x14
#define HWIO_AOSS_CC_PLL1_MODE_PLL_BIAS_COUNT_BMSK                                   0xfc000
#define HWIO_AOSS_CC_PLL1_MODE_PLL_BIAS_COUNT_SHFT                                       0xe
#define HWIO_AOSS_CC_PLL1_MODE_PLL_LOCK_COUNT_BMSK                                    0x3f00
#define HWIO_AOSS_CC_PLL1_MODE_PLL_LOCK_COUNT_SHFT                                       0x8
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT7_BMSK                                        0x80
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT7_SHFT                                         0x7
#define HWIO_AOSS_CC_PLL1_MODE_PLL_CLOCK_SELECT_BMSK                                    0x40
#define HWIO_AOSS_CC_PLL1_MODE_PLL_CLOCK_SELECT_SHFT                                     0x6
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT5_BMSK                                        0x20
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT5_SHFT                                         0x5
#define HWIO_AOSS_CC_PLL1_MODE_PLL_CLK_XO_PRESENT_BMSK                                  0x10
#define HWIO_AOSS_CC_PLL1_MODE_PLL_CLK_XO_PRESENT_SHFT                                   0x4
#define HWIO_AOSS_CC_PLL1_MODE_PLL_IN_CALIBRATION_BMSK                                   0x8
#define HWIO_AOSS_CC_PLL1_MODE_PLL_IN_CALIBRATION_SHFT                                   0x3
#define HWIO_AOSS_CC_PLL1_MODE_PLL_RESET_N_BMSK                                          0x4
#define HWIO_AOSS_CC_PLL1_MODE_PLL_RESET_N_SHFT                                          0x2
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT1_BMSK                                         0x2
#define HWIO_AOSS_CC_PLL1_MODE_RESERVE_BIT1_SHFT                                         0x1
#define HWIO_AOSS_CC_PLL1_MODE_PLL_OUTCTRL_BMSK                                          0x1
#define HWIO_AOSS_CC_PLL1_MODE_PLL_OUTCTRL_SHFT                                          0x0

#define HWIO_AOSS_CC_PLL1_L_VAL_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001004)
#define HWIO_AOSS_CC_PLL1_L_VAL_RMSK                                                   0xfff
#define HWIO_AOSS_CC_PLL1_L_VAL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_L_VAL_ADDR, HWIO_AOSS_CC_PLL1_L_VAL_RMSK)
#define HWIO_AOSS_CC_PLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_L_VAL_ADDR, m)
#define HWIO_AOSS_CC_PLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_L_VAL_ADDR,v)
#define HWIO_AOSS_CC_PLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_L_VAL_ADDR,m,v,HWIO_AOSS_CC_PLL1_L_VAL_IN)
#define HWIO_AOSS_CC_PLL1_L_VAL_PLL_L_BMSK                                             0xfff
#define HWIO_AOSS_CC_PLL1_L_VAL_PLL_L_SHFT                                               0x0

#define HWIO_AOSS_CC_PLL1_USER_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001008)
#define HWIO_AOSS_CC_PLL1_USER_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_USER_CTL_ADDR, HWIO_AOSS_CC_PLL1_USER_CTL_RMSK)
#define HWIO_AOSS_CC_PLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_USER_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_USER_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_USER_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL1_USER_CTL_IN)
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS31_12_BMSK                         0xfffff000
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS31_12_SHFT                                0xc
#define HWIO_AOSS_CC_PLL1_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                            0xf00
#define HWIO_AOSS_CC_PLL1_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                              0x8
#define HWIO_AOSS_CC_PLL1_USER_CTL_OUT_CLK_POLARITY_BMSK                                0x80
#define HWIO_AOSS_CC_PLL1_USER_CTL_OUT_CLK_POLARITY_SHFT                                 0x7
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS6_5_BMSK                                 0x60
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS6_5_SHFT                                  0x5
#define HWIO_AOSS_CC_PLL1_USER_CTL_PLLOUT_TEST_BMSK                                     0x10
#define HWIO_AOSS_CC_PLL1_USER_CTL_PLLOUT_TEST_SHFT                                      0x4
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS3_1_BMSK                                  0xe
#define HWIO_AOSS_CC_PLL1_USER_CTL_RESERVE_BITS3_1_SHFT                                  0x1
#define HWIO_AOSS_CC_PLL1_USER_CTL_PLLOUT_MAIN_BMSK                                      0x1
#define HWIO_AOSS_CC_PLL1_USER_CTL_PLLOUT_MAIN_SHFT                                      0x0

#define HWIO_AOSS_CC_PLL1_USER_CTL_U_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000100c)
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_USER_CTL_U_ADDR, HWIO_AOSS_CC_PLL1_USER_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_USER_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_USER_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_USER_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL1_USER_CTL_U_IN)
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_RESERVE_BITS63_56_BMSK                       0xff000000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_RESERVE_BITS63_56_SHFT                             0x18
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_ENABLE_CONT_CAL_BMSK                           0x800000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_ENABLE_CONT_CAL_SHFT                               0x17
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_XO_FREQUENCY_SELECT_BMSK                       0x400000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_XO_FREQUENCY_SELECT_SHFT                           0x16
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INTERNAL_CLOCK_SELECTION_BMSK                  0x300000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INTERNAL_CLOCK_SELECTION_SHFT                      0x14
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INITIAL_INPUT_CLOCK_MUX_BMSK                    0xe0000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INITIAL_INPUT_CLOCK_MUX_SHFT                       0x11
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INITIAL_OUTPUT_CLOCK_MUX_BMSK                   0x18000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_INITIAL_OUTPUT_CLOCK_MUX_SHFT                       0xf
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                          0x4000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                             0xe
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REF_CLK_AT_OUT_BMSK                              0x2000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                 0xd
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_CALIBRATION_SETTING_BMSK                         0x1000
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_CALIBRATION_SETTING_SHFT                            0xc
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_USE_RESTORE_BMSK                                  0x800
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_USE_RESTORE_SHFT                                    0xb
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REQUEST_CAL_BMSK                                  0x400
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REQUEST_CAL_SHFT                                    0xa
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_STATUS_REGISTER_BMSK                              0x3e0
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_STATUS_REGISTER_SHFT                                0x5
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REF_DIVIDER_BMSK                                   0x18
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_REF_DIVIDER_SHFT                                    0x3
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_CAL_SCALING_BMSK                                    0x6
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_CAL_SCALING_SHFT                                    0x1
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_USE_EXTERNAL_CLK_REF_BMSK                           0x1
#define HWIO_AOSS_CC_PLL1_USER_CTL_U_USE_EXTERNAL_CLK_REF_SHFT                           0x0

#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001010)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_CONFIG_CTL_ADDR, HWIO_AOSS_CC_PLL1_CONFIG_CTL_RMSK)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_CONFIG_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_CONFIG_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_CONFIG_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL1_CONFIG_CTL_IN)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_ENABLE_BYPASS_BMSK                   0x80000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_ENABLE_BYPASS_SHFT                         0x1f
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_DISABLE_STARTUP_BMSK                 0x40000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_DISABLE_STARTUP_SHFT                       0x1e
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_DISABLE_KICKSTART_BMSK               0x20000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_DISABLE_KICKSTART_SHFT                     0x1d
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_FTUNE_BMSK                           0x1c000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_FTUNE_SHFT                                 0x1a
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_CTUNE_BMSK                            0x3000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_REF_OSC_CTUNE_SHFT                                 0x18
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_NUM_REF_CYCLES_FOR_CAL_BMSK                    0xe00000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_NUM_REF_CYCLES_FOR_CAL_SHFT                        0x15
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_INC_MIN_GLITCH_THRESHOLD_4X_BMSK               0x100000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                   0x14
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_MIN_GLITCH_THRESHOLD_BMSK                       0xc0000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_MIN_GLITCH_THRESHOLD_SHFT                          0x12
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_OSC_WARMUP_TIME_BMSK                            0x30000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_OSC_WARMUP_TIME_SHFT                               0x10
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_BIAS_WARMUP_TIME_BMSK                            0xc000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_BIAS_WARMUP_TIME_SHFT                               0xe
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                     0x3800
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                        0xb
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                    0x700
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                      0x8
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FWD_GAIN_KFN_SLEW_BMSK                             0xf0
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FWD_GAIN_KFN_SLEW_SHFT                              0x4
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                   0xf
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                   0x0

#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001014)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_RMSK                                       0xffffffff
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ADDR, HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_IN)
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_DCO_WARMUP_TIME_BMSK                       0xf0000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_DCO_WARMUP_TIME_SHFT                             0x1c
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_POST_DIV_BMSK                           0xe000000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_POST_DIV_SHFT                                0x19
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_PLL_OSC_CONTROL_BMSK                        0x1f80000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_PLL_OSC_CONTROL_SHFT                             0x13
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ENABLE_RESYNC_BMSK                            0x40000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ENABLE_RESYNC_SHFT                               0x12
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ENABLE_REF1_KICKSTART_BMSK                    0x20000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ENABLE_REF1_KICKSTART_SHFT                       0x11
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_USE_REF1_BMSK                                 0x10000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_USE_REF1_SHFT                                    0x10
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ATEST1_VOLT_SELECT_BMSK                        0xc000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_ATEST1_VOLT_SELECT_SHFT                           0xe
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_CAL_REF1_PULSE_BMSK                            0x3000
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_CAL_REF1_PULSE_SHFT                               0xc
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_OSC_CONTROL_2_BMSK                          0xff0
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_OSC_CONTROL_2_SHFT                            0x4
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_DTEST_SELECT_BMSK                             0xe
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_DTEST_SELECT_SHFT                             0x1
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_OSC_BYPASSMODE_BMSK                           0x1
#define HWIO_AOSS_CC_PLL1_CONFIG_CTL_U_REF_OSC_BYPASSMODE_SHFT                           0x0

#define HWIO_AOSS_CC_PLL1_TEST_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001018)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_TEST_CTL_ADDR, HWIO_AOSS_CC_PLL1_TEST_CTL_RMSK)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_TEST_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_TEST_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_TEST_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL1_TEST_CTL_IN)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DIV2_NMO_EN_BMSK                               0x80000000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DIV2_NMO_EN_SHFT                                     0x1f
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OVERRIDE_XO_BMSK                               0x40000000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OVERRIDE_XO_SHFT                                     0x1e
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NMO_OSC_SEL_BMSK                               0x30000000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NMO_OSC_SEL_SHFT                                     0x1c
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NOISE_MAG_BMSK                                  0xe000000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NOISE_MAG_SHFT                                       0x19
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NOISE_GEN_EN_BMSK                               0x1000000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_NOISE_GEN_EN_SHFT                                    0x18
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DIVIDE_SOSC_BMSK                                 0xc00000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DIVIDE_SOSC_SHFT                                     0x16
#define HWIO_AOSS_CC_PLL1_TEST_CTL_STATUS_REG_EN_BMSK                               0x200000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_STATUS_REG_EN_SHFT                                   0x15
#define HWIO_AOSS_CC_PLL1_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK                 0x180000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                     0x13
#define HWIO_AOSS_CC_PLL1_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                        0x40000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                           0x12
#define HWIO_AOSS_CC_PLL1_TEST_CTL_FCW_BMSK                                          0x3ff00
#define HWIO_AOSS_CC_PLL1_TEST_CTL_FCW_SHFT                                              0x8
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OVERRIDE_FCW_BMSK                                    0x80
#define HWIO_AOSS_CC_PLL1_TEST_CTL_OVERRIDE_FCW_SHFT                                     0x7
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DISABLE_LFSR_BMSK                                    0x40
#define HWIO_AOSS_CC_PLL1_TEST_CTL_DISABLE_LFSR_SHFT                                     0x6
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST1_SEL_BMSK                                      0x30
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST1_SEL_SHFT                                       0x4
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST0_SEL_BMSK                                       0xc
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST0_SEL_SHFT                                       0x2
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST1_EN_BMSK                                        0x2
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST1_EN_SHFT                                        0x1
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST0_EN_BMSK                                        0x1
#define HWIO_AOSS_CC_PLL1_TEST_CTL_ATEST0_EN_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000101c)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_TEST_CTL_U_ADDR, HWIO_AOSS_CC_PLL1_TEST_CTL_U_RMSK)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_TEST_CTL_U_ADDR, m)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_TEST_CTL_U_ADDR,v)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_TEST_CTL_U_ADDR,m,v,HWIO_AOSS_CC_PLL1_TEST_CTL_U_IN)
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_BIST_CFG_BMSK                                0xfff00000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_BIST_CFG_SHFT                                      0x14
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                           0xf0000
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                              0x10
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_RESERVE_BITS47_39_BMSK                           0xff80
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_RESERVE_BITS47_39_SHFT                              0x7
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_DCO_ON_BMSK                                  0x40
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_DCO_ON_SHFT                                   0x6
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_RESERVE_BIT37_BMSK                                 0x20
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_RESERVE_BIT37_SHFT                                  0x5
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_SEL_CURRENT_DCO_BMSK                               0x10
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_SEL_CURRENT_DCO_SHFT                                0x4
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_REFOSC_ON_BMSK                                0x8
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_REFOSC_ON_SHFT                                0x3
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_REFBIAS_ON_BMSK                               0x4
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_REFBIAS_ON_SHFT                               0x2
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_SOSC_ON_BMSK                                  0x2
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_FORCE_SOSC_ON_SHFT                                  0x1
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_ENABLE_TEST_SEQ_BMSK                                0x1
#define HWIO_AOSS_CC_PLL1_TEST_CTL_U_ENABLE_TEST_SEQ_SHFT                                0x0

#define HWIO_AOSS_CC_PLL1_STATUS_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001020)
#define HWIO_AOSS_CC_PLL1_STATUS_RMSK                                             0xffffffff
#define HWIO_AOSS_CC_PLL1_STATUS_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_STATUS_ADDR, HWIO_AOSS_CC_PLL1_STATUS_RMSK)
#define HWIO_AOSS_CC_PLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_STATUS_ADDR, m)
#define HWIO_AOSS_CC_PLL1_STATUS_STATUS_31_0_BMSK                                 0xffffffff
#define HWIO_AOSS_CC_PLL1_STATUS_STATUS_31_0_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL1_FREQ_CTL_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001024)
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_RMSK                                           0xffffffff
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_FREQ_CTL_ADDR, HWIO_AOSS_CC_PLL1_FREQ_CTL_RMSK)
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_FREQ_CTL_ADDR, m)
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_FREQ_CTL_ADDR,v)
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_FREQ_CTL_ADDR,m,v,HWIO_AOSS_CC_PLL1_FREQ_CTL_IN)
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                        0xffffffff
#define HWIO_AOSS_CC_PLL1_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                               0x0

#define HWIO_AOSS_CC_PLL1_OPMODE_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001028)
#define HWIO_AOSS_CC_PLL1_OPMODE_RMSK                                                    0x7
#define HWIO_AOSS_CC_PLL1_OPMODE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_OPMODE_ADDR, HWIO_AOSS_CC_PLL1_OPMODE_RMSK)
#define HWIO_AOSS_CC_PLL1_OPMODE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_OPMODE_ADDR, m)
#define HWIO_AOSS_CC_PLL1_OPMODE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_OPMODE_ADDR,v)
#define HWIO_AOSS_CC_PLL1_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_OPMODE_ADDR,m,v,HWIO_AOSS_CC_PLL1_OPMODE_IN)
#define HWIO_AOSS_CC_PLL1_OPMODE_PLL_OPMODE_BMSK                                         0x7
#define HWIO_AOSS_CC_PLL1_OPMODE_PLL_OPMODE_SHFT                                         0x0

#define HWIO_AOSS_CC_PLL1_STATE_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000102c)
#define HWIO_AOSS_CC_PLL1_STATE_RMSK                                                     0x7
#define HWIO_AOSS_CC_PLL1_STATE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_STATE_ADDR, HWIO_AOSS_CC_PLL1_STATE_RMSK)
#define HWIO_AOSS_CC_PLL1_STATE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_STATE_ADDR, m)
#define HWIO_AOSS_CC_PLL1_STATE_PLL_STATE_BMSK                                           0x7
#define HWIO_AOSS_CC_PLL1_STATE_PLL_STATE_SHFT                                           0x0

#define HWIO_AOSS_CC_PLL1_SPARE_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00001030)
#define HWIO_AOSS_CC_PLL1_SPARE_RMSK                                                    0xff
#define HWIO_AOSS_CC_PLL1_SPARE_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL1_SPARE_ADDR, HWIO_AOSS_CC_PLL1_SPARE_RMSK)
#define HWIO_AOSS_CC_PLL1_SPARE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL1_SPARE_ADDR, m)
#define HWIO_AOSS_CC_PLL1_SPARE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL1_SPARE_ADDR,v)
#define HWIO_AOSS_CC_PLL1_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL1_SPARE_ADDR,m,v,HWIO_AOSS_CC_PLL1_SPARE_IN)
#define HWIO_AOSS_CC_PLL1_SPARE_SPARE_OUTPUTS_BMSK                                      0xf0
#define HWIO_AOSS_CC_PLL1_SPARE_SPARE_OUTPUTS_SHFT                                       0x4
#define HWIO_AOSS_CC_PLL1_SPARE_SPARE_INPUTS_BMSK                                        0xf
#define HWIO_AOSS_CC_PLL1_SPARE_SPARE_INPUTS_SHFT                                        0x0

#define HWIO_AOSS_CC_AOP_BCR_ADDR                                                 (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002000)
#define HWIO_AOSS_CC_AOP_BCR_RMSK                                                        0x1
#define HWIO_AOSS_CC_AOP_BCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_BCR_ADDR, HWIO_AOSS_CC_AOP_BCR_RMSK)
#define HWIO_AOSS_CC_AOP_BCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_BCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_BCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_BCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_BCR_ADDR,m,v,HWIO_AOSS_CC_AOP_BCR_IN)
#define HWIO_AOSS_CC_AOP_BCR_BLK_ARES_BMSK                                               0x1
#define HWIO_AOSS_CC_AOP_BCR_BLK_ARES_SHFT                                               0x0

#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002004)
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_RMSK                                      0x80000005
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_ADDR, HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_RMSK)
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_ADDR,m,v,HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_IN)
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_ARES_BMSK                                    0x4
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_ARES_SHFT                                    0x2
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_ENABLE_BMSK                                  0x1
#define HWIO_AOSS_CC_AOP_PROC_FCLK_CBCR_CLK_ENABLE_SHFT                                  0x0

#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002008)
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_RMSK                                      0x80000004
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_ADDR, HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_RMSK)
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_ADDR,m,v,HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_IN)
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_CLK_OFF_BMSK                              0x80000000
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_CLK_OFF_SHFT                                    0x1f
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_CLK_ARES_BMSK                                    0x4
#define HWIO_AOSS_CC_AOP_PROC_HCLK_CBCR_CLK_ARES_SHFT                                    0x2

#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000200c)
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_RMSK                                        0x80007ff4
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_ADDR, HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_RMSK)
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_ADDR,m,v,HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_IN)
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                          0x4000
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                             0xe
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                        0x2000
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                           0xd
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                       0x1000
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                          0xc
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_WAKEUP_BMSK                                      0xf00
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_WAKEUP_SHFT                                        0x8
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_SLEEP_BMSK                                        0xf0
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_SLEEP_SHFT                                         0x4
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_AOSS_CC_AOP_BUS_AHB_CBCR_CLK_ARES_SHFT                                      0x2

#define HWIO_AOSS_CC_BUS_CBCR_ADDR                                                (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002010)
#define HWIO_AOSS_CC_BUS_CBCR_RMSK                                                0x80000004
#define HWIO_AOSS_CC_BUS_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_BUS_CBCR_ADDR, HWIO_AOSS_CC_BUS_CBCR_RMSK)
#define HWIO_AOSS_CC_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_BUS_CBCR_ADDR, m)
#define HWIO_AOSS_CC_BUS_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_BUS_CBCR_ADDR,v)
#define HWIO_AOSS_CC_BUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_BUS_CBCR_ADDR,m,v,HWIO_AOSS_CC_BUS_CBCR_IN)
#define HWIO_AOSS_CC_BUS_CBCR_CLK_OFF_BMSK                                        0x80000000
#define HWIO_AOSS_CC_BUS_CBCR_CLK_OFF_SHFT                                              0x1f
#define HWIO_AOSS_CC_BUS_CBCR_CLK_ARES_BMSK                                              0x4
#define HWIO_AOSS_CC_BUS_CBCR_CLK_ARES_SHFT                                              0x2

#define HWIO_AOSS_CC_RPMH_CBCR_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002014)
#define HWIO_AOSS_CC_RPMH_CBCR_RMSK                                               0x80000005
#define HWIO_AOSS_CC_RPMH_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_RPMH_CBCR_ADDR, HWIO_AOSS_CC_RPMH_CBCR_RMSK)
#define HWIO_AOSS_CC_RPMH_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RPMH_CBCR_ADDR, m)
#define HWIO_AOSS_CC_RPMH_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RPMH_CBCR_ADDR,v)
#define HWIO_AOSS_CC_RPMH_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RPMH_CBCR_ADDR,m,v,HWIO_AOSS_CC_RPMH_CBCR_IN)
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_ARES_SHFT                                             0x2
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_AOSS_CC_RPMH_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002018)
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_RMSK                                        0x80007ff5
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_MESSAGE_RAM_CBCR_ADDR, HWIO_AOSS_CC_MESSAGE_RAM_CBCR_RMSK)
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_MESSAGE_RAM_CBCR_ADDR, m)
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_MESSAGE_RAM_CBCR_ADDR,v)
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_MESSAGE_RAM_CBCR_ADDR,m,v,HWIO_AOSS_CC_MESSAGE_RAM_CBCR_IN)
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_CORE_ON_BMSK                          0x4000
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_CORE_ON_SHFT                             0xe
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                        0x2000
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                           0xd
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                       0x1000
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                          0xc
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_WAKEUP_BMSK                                      0xf00
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_WAKEUP_SHFT                                        0x8
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_SLEEP_BMSK                                        0xf0
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_SLEEP_SHFT                                         0x4
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_AOSS_CC_MESSAGE_RAM_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_AOSS_CC_AOP_RO_CBCR_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000201c)
#define HWIO_AOSS_CC_AOP_RO_CBCR_RMSK                                             0x80000005
#define HWIO_AOSS_CC_AOP_RO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_RO_CBCR_ADDR, HWIO_AOSS_CC_AOP_RO_CBCR_RMSK)
#define HWIO_AOSS_CC_AOP_RO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_RO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_RO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_RO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_RO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_RO_CBCR_ADDR,m,v,HWIO_AOSS_CC_AOP_RO_CBCR_IN)
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_AOSS_CC_AOP_RO_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002020)
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_RMSK                                       0x80000005
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_ADDR, HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_RMSK)
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_ADDR, m)
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_ADDR,v)
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_ADDR,m,v,HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_IN)
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_AOSS_CC_PWR_MUX_CTRL_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_AOSS_CC_AO_DAP_CBCR_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002024)
#define HWIO_AOSS_CC_AO_DAP_CBCR_RMSK                                             0x80000005
#define HWIO_AOSS_CC_AO_DAP_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AO_DAP_CBCR_ADDR, HWIO_AOSS_CC_AO_DAP_CBCR_RMSK)
#define HWIO_AOSS_CC_AO_DAP_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AO_DAP_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AO_DAP_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AO_DAP_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AO_DAP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AO_DAP_CBCR_ADDR,m,v,HWIO_AOSS_CC_AO_DAP_CBCR_IN)
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_AOSS_CC_AO_DAP_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_AOSS_CC_AOP_DAP_CBCR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002028)
#define HWIO_AOSS_CC_AOP_DAP_CBCR_RMSK                                            0x80000005
#define HWIO_AOSS_CC_AOP_DAP_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_DAP_CBCR_ADDR, HWIO_AOSS_CC_AOP_DAP_CBCR_RMSK)
#define HWIO_AOSS_CC_AOP_DAP_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_DAP_CBCR_ADDR, m)
#define HWIO_AOSS_CC_AOP_DAP_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_DAP_CBCR_ADDR,v)
#define HWIO_AOSS_CC_AOP_DAP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_DAP_CBCR_ADDR,m,v,HWIO_AOSS_CC_AOP_DAP_CBCR_IN)
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_AOSS_CC_AOP_DAP_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_AOSS_CC_WCSS_TS_CBCR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000202c)
#define HWIO_AOSS_CC_WCSS_TS_CBCR_RMSK                                            0x80000005
#define HWIO_AOSS_CC_WCSS_TS_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_WCSS_TS_CBCR_ADDR, HWIO_AOSS_CC_WCSS_TS_CBCR_RMSK)
#define HWIO_AOSS_CC_WCSS_TS_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_WCSS_TS_CBCR_ADDR, m)
#define HWIO_AOSS_CC_WCSS_TS_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_WCSS_TS_CBCR_ADDR,v)
#define HWIO_AOSS_CC_WCSS_TS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_WCSS_TS_CBCR_ADDR,m,v,HWIO_AOSS_CC_WCSS_TS_CBCR_IN)
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_AOSS_CC_WCSS_TS_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_AOSS_CC_RO_CBCR_ADDR                                                 (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002030)
#define HWIO_AOSS_CC_RO_CBCR_RMSK                                                 0x80000005
#define HWIO_AOSS_CC_RO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_RO_CBCR_ADDR, HWIO_AOSS_CC_RO_CBCR_RMSK)
#define HWIO_AOSS_CC_RO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_RO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_RO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RO_CBCR_ADDR,m,v,HWIO_AOSS_CC_RO_CBCR_IN)
#define HWIO_AOSS_CC_RO_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_AOSS_CC_RO_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_AOSS_CC_RO_CBCR_CLK_ARES_BMSK                                               0x4
#define HWIO_AOSS_CC_RO_CBCR_CLK_ARES_SHFT                                               0x2
#define HWIO_AOSS_CC_RO_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_AOSS_CC_RO_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_AOSS_CC_EUD_AT_CBCR_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002034)
#define HWIO_AOSS_CC_EUD_AT_CBCR_RMSK                                             0x80000005
#define HWIO_AOSS_CC_EUD_AT_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CBCR_ADDR, HWIO_AOSS_CC_EUD_AT_CBCR_RMSK)
#define HWIO_AOSS_CC_EUD_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CBCR_ADDR, m)
#define HWIO_AOSS_CC_EUD_AT_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_EUD_AT_CBCR_ADDR,v)
#define HWIO_AOSS_CC_EUD_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_EUD_AT_CBCR_ADDR,m,v,HWIO_AOSS_CC_EUD_AT_CBCR_IN)
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_OFF_BMSK                                     0x80000000
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_OFF_SHFT                                           0x1f
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_ARES_BMSK                                           0x4
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_ARES_SHFT                                           0x2
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_ENABLE_BMSK                                         0x1
#define HWIO_AOSS_CC_EUD_AT_CBCR_CLK_ENABLE_SHFT                                         0x0

#define HWIO_AOSS_CC_XO_CBCR_ADDR                                                 (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002038)
#define HWIO_AOSS_CC_XO_CBCR_RMSK                                                 0x80000005
#define HWIO_AOSS_CC_XO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_XO_CBCR_ADDR, HWIO_AOSS_CC_XO_CBCR_RMSK)
#define HWIO_AOSS_CC_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_XO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_XO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_XO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_XO_CBCR_ADDR,m,v,HWIO_AOSS_CC_XO_CBCR_IN)
#define HWIO_AOSS_CC_XO_CBCR_CLK_OFF_BMSK                                         0x80000000
#define HWIO_AOSS_CC_XO_CBCR_CLK_OFF_SHFT                                               0x1f
#define HWIO_AOSS_CC_XO_CBCR_CLK_ARES_BMSK                                               0x4
#define HWIO_AOSS_CC_XO_CBCR_CLK_ARES_SHFT                                               0x2
#define HWIO_AOSS_CC_XO_CBCR_CLK_ENABLE_BMSK                                             0x1
#define HWIO_AOSS_CC_XO_CBCR_CLK_ENABLE_SHFT                                             0x0

#define HWIO_AOSS_CC_GDSC_CBCR_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000203c)
#define HWIO_AOSS_CC_GDSC_CBCR_RMSK                                               0x80000005
#define HWIO_AOSS_CC_GDSC_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_GDSC_CBCR_ADDR, HWIO_AOSS_CC_GDSC_CBCR_RMSK)
#define HWIO_AOSS_CC_GDSC_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_GDSC_CBCR_ADDR, m)
#define HWIO_AOSS_CC_GDSC_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_GDSC_CBCR_ADDR,v)
#define HWIO_AOSS_CC_GDSC_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_GDSC_CBCR_ADDR,m,v,HWIO_AOSS_CC_GDSC_CBCR_IN)
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_ARES_SHFT                                             0x2
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_AOSS_CC_GDSC_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_AOSS_CC_AOP_CMD_RCGR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002040)
#define HWIO_AOSS_CC_AOP_CMD_RCGR_RMSK                                            0x80000013
#define HWIO_AOSS_CC_AOP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_CMD_RCGR_ADDR, HWIO_AOSS_CC_AOP_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_AOP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_AOP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_AOP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_AOP_CMD_RCGR_IN)
#define HWIO_AOSS_CC_AOP_CMD_RCGR_ROOT_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_AOP_CMD_RCGR_ROOT_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_AOP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                   0x10
#define HWIO_AOSS_CC_AOP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                    0x4
#define HWIO_AOSS_CC_AOP_CMD_RCGR_ROOT_EN_BMSK                                           0x2
#define HWIO_AOSS_CC_AOP_CMD_RCGR_ROOT_EN_SHFT                                           0x1
#define HWIO_AOSS_CC_AOP_CMD_RCGR_UPDATE_BMSK                                            0x1
#define HWIO_AOSS_CC_AOP_CMD_RCGR_UPDATE_SHFT                                            0x0

#define HWIO_AOSS_CC_AOP_CFG_RCGR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002044)
#define HWIO_AOSS_CC_AOP_CFG_RCGR_RMSK                                              0x11071f
#define HWIO_AOSS_CC_AOP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_CFG_RCGR_ADDR, HWIO_AOSS_CC_AOP_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_AOP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_AOP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_AOP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_AOP_CFG_RCGR_IN)
#define HWIO_AOSS_CC_AOP_CFG_RCGR_HW_CLK_CONTROL_BMSK                               0x100000
#define HWIO_AOSS_CC_AOP_CFG_RCGR_HW_CLK_CONTROL_SHFT                                   0x14
#define HWIO_AOSS_CC_AOP_CFG_RCGR_RCGLITE_DISABLE_BMSK                               0x10000
#define HWIO_AOSS_CC_AOP_CFG_RCGR_RCGLITE_DISABLE_SHFT                                  0x10
#define HWIO_AOSS_CC_AOP_CFG_RCGR_SRC_SEL_BMSK                                         0x700
#define HWIO_AOSS_CC_AOP_CFG_RCGR_SRC_SEL_SHFT                                           0x8
#define HWIO_AOSS_CC_AOP_CFG_RCGR_SRC_DIV_BMSK                                          0x1f
#define HWIO_AOSS_CC_AOP_CFG_RCGR_SRC_DIV_SHFT                                           0x0

#define HWIO_AOSS_CC_AOP_DIV_CDIVR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002080)
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_RMSK                                                  0xf
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_DIV_CDIVR_ADDR, HWIO_AOSS_CC_AOP_DIV_CDIVR_RMSK)
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_DIV_CDIVR_ADDR, m)
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_DIV_CDIVR_ADDR,v)
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_DIV_CDIVR_ADDR,m,v,HWIO_AOSS_CC_AOP_DIV_CDIVR_IN)
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_CLK_DIV_BMSK                                          0xf
#define HWIO_AOSS_CC_AOP_DIV_CDIVR_CLK_DIV_SHFT                                          0x0

#define HWIO_AOSS_CC_RO_CMD_RCGR_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002084)
#define HWIO_AOSS_CC_RO_CMD_RCGR_RMSK                                             0x80000013
#define HWIO_AOSS_CC_RO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_RO_CMD_RCGR_ADDR, HWIO_AOSS_CC_RO_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_RO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RO_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_RO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RO_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_RO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RO_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_RO_CMD_RCGR_IN)
#define HWIO_AOSS_CC_RO_CMD_RCGR_ROOT_OFF_BMSK                                    0x80000000
#define HWIO_AOSS_CC_RO_CMD_RCGR_ROOT_OFF_SHFT                                          0x1f
#define HWIO_AOSS_CC_RO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                    0x10
#define HWIO_AOSS_CC_RO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                     0x4
#define HWIO_AOSS_CC_RO_CMD_RCGR_ROOT_EN_BMSK                                            0x2
#define HWIO_AOSS_CC_RO_CMD_RCGR_ROOT_EN_SHFT                                            0x1
#define HWIO_AOSS_CC_RO_CMD_RCGR_UPDATE_BMSK                                             0x1
#define HWIO_AOSS_CC_RO_CMD_RCGR_UPDATE_SHFT                                             0x0

#define HWIO_AOSS_CC_RO_CFG_RCGR_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002088)
#define HWIO_AOSS_CC_RO_CFG_RCGR_RMSK                                               0x11071f
#define HWIO_AOSS_CC_RO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_RO_CFG_RCGR_ADDR, HWIO_AOSS_CC_RO_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_RO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RO_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_RO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RO_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_RO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RO_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_RO_CFG_RCGR_IN)
#define HWIO_AOSS_CC_RO_CFG_RCGR_HW_CLK_CONTROL_BMSK                                0x100000
#define HWIO_AOSS_CC_RO_CFG_RCGR_HW_CLK_CONTROL_SHFT                                    0x14
#define HWIO_AOSS_CC_RO_CFG_RCGR_RCGLITE_DISABLE_BMSK                                0x10000
#define HWIO_AOSS_CC_RO_CFG_RCGR_RCGLITE_DISABLE_SHFT                                   0x10
#define HWIO_AOSS_CC_RO_CFG_RCGR_SRC_SEL_BMSK                                          0x700
#define HWIO_AOSS_CC_RO_CFG_RCGR_SRC_SEL_SHFT                                            0x8
#define HWIO_AOSS_CC_RO_CFG_RCGR_SRC_DIV_BMSK                                           0x1f
#define HWIO_AOSS_CC_RO_CFG_RCGR_SRC_DIV_SHFT                                            0x0

#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000020c4)
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_RMSK                                         0x80000013
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ADDR, HWIO_AOSS_CC_EUD_AT_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_EUD_AT_CMD_RCGR_IN)
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                0x10
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                 0x4
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_AOSS_CC_EUD_AT_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000020c8)
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_RMSK                                           0x11071f
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CFG_RCGR_ADDR, HWIO_AOSS_CC_EUD_AT_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_EUD_AT_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_EUD_AT_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_EUD_AT_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_EUD_AT_CFG_RCGR_IN)
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_HW_CLK_CONTROL_BMSK                            0x100000
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_HW_CLK_CONTROL_SHFT                                0x14
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_RCGLITE_DISABLE_BMSK                            0x10000
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_RCGLITE_DISABLE_SHFT                               0x10
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_AOSS_CC_EUD_AT_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_AOSS_CC_TSENS_HW_CBCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000226c)
#define HWIO_AOSS_CC_TSENS_HW_CBCR_RMSK                                           0x80000004
#define HWIO_AOSS_CC_TSENS_HW_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_TSENS_HW_CBCR_ADDR, HWIO_AOSS_CC_TSENS_HW_CBCR_RMSK)
#define HWIO_AOSS_CC_TSENS_HW_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_TSENS_HW_CBCR_ADDR, m)
#define HWIO_AOSS_CC_TSENS_HW_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_TSENS_HW_CBCR_ADDR,v)
#define HWIO_AOSS_CC_TSENS_HW_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_TSENS_HW_CBCR_ADDR,m,v,HWIO_AOSS_CC_TSENS_HW_CBCR_IN)
#define HWIO_AOSS_CC_TSENS_HW_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_TSENS_HW_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_TSENS_HW_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_AOSS_CC_TSENS_HW_CBCR_CLK_ARES_SHFT                                         0x2

#define HWIO_AOSS_CC_SLEEP_CLK_BCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002104)
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_RMSK                                                  0x1
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CLK_BCR_ADDR, HWIO_AOSS_CC_SLEEP_CLK_BCR_RMSK)
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CLK_BCR_ADDR, m)
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SLEEP_CLK_BCR_ADDR,v)
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SLEEP_CLK_BCR_ADDR,m,v,HWIO_AOSS_CC_SLEEP_CLK_BCR_IN)
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_BLK_ARES_BMSK                                         0x1
#define HWIO_AOSS_CC_SLEEP_CLK_BCR_BLK_ARES_SHFT                                         0x0

#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002108)
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_RMSK                                          0x80000013
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CMD_RCGR_ADDR, HWIO_AOSS_CC_SLEEP_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SLEEP_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SLEEP_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_SLEEP_CMD_RCGR_IN)
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_ROOT_OFF_BMSK                                 0x80000000
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_ROOT_OFF_SHFT                                       0x1f
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                 0x10
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                  0x4
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_ROOT_EN_BMSK                                         0x2
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_ROOT_EN_SHFT                                         0x1
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_UPDATE_BMSK                                          0x1
#define HWIO_AOSS_CC_SLEEP_CMD_RCGR_UPDATE_SHFT                                          0x0

#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000210c)
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_RMSK                                            0x11071f
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CFG_RCGR_ADDR, HWIO_AOSS_CC_SLEEP_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SLEEP_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SLEEP_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_SLEEP_CFG_RCGR_IN)
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_HW_CLK_CONTROL_BMSK                             0x100000
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_HW_CLK_CONTROL_SHFT                                 0x14
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_RCGLITE_DISABLE_BMSK                             0x10000
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_RCGLITE_DISABLE_SHFT                                0x10
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_SRC_SEL_BMSK                                       0x700
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_SRC_SEL_SHFT                                         0x8
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_SRC_DIV_BMSK                                        0x1f
#define HWIO_AOSS_CC_SLEEP_CFG_RCGR_SRC_DIV_SHFT                                         0x0

#define HWIO_AOSS_CC_SLEEP_CBCR_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002148)
#define HWIO_AOSS_CC_SLEEP_CBCR_RMSK                                              0x80000005
#define HWIO_AOSS_CC_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CBCR_ADDR, HWIO_AOSS_CC_SLEEP_CBCR_RMSK)
#define HWIO_AOSS_CC_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SLEEP_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SLEEP_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SLEEP_CBCR_ADDR,m,v,HWIO_AOSS_CC_SLEEP_CBCR_IN)
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_ARES_BMSK                                            0x4
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_ARES_SHFT                                            0x2
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_AOSS_CC_SLEEP_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002270)
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_RMSK                                         0x80000005
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PERIPH_SLP_CBCR_ADDR, HWIO_AOSS_CC_PERIPH_SLP_CBCR_RMSK)
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PERIPH_SLP_CBCR_ADDR, m)
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PERIPH_SLP_CBCR_ADDR,v)
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PERIPH_SLP_CBCR_ADDR,m,v,HWIO_AOSS_CC_PERIPH_SLP_CBCR_IN)
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_OFF_BMSK                                 0x80000000
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_OFF_SHFT                                       0x1f
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_ARES_BMSK                                       0x4
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_ARES_SHFT                                       0x2
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_ENABLE_BMSK                                     0x1
#define HWIO_AOSS_CC_PERIPH_SLP_CBCR_CLK_ENABLE_SHFT                                     0x0

#define HWIO_AOSS_CC_SWAO_BCR_ADDR                                                (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000214c)
#define HWIO_AOSS_CC_SWAO_BCR_RMSK                                                       0x1
#define HWIO_AOSS_CC_SWAO_BCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_BCR_ADDR, HWIO_AOSS_CC_SWAO_BCR_RMSK)
#define HWIO_AOSS_CC_SWAO_BCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_BCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_BCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_BCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_BCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_BCR_IN)
#define HWIO_AOSS_CC_SWAO_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_AOSS_CC_SWAO_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_AOSS_CC_SWAO_GDSCR_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002150)
#define HWIO_AOSS_CC_SWAO_GDSCR_RMSK                                              0xf8ffffff
#define HWIO_AOSS_CC_SWAO_GDSCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_GDSCR_ADDR, HWIO_AOSS_CC_SWAO_GDSCR_RMSK)
#define HWIO_AOSS_CC_SWAO_GDSCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_GDSCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_GDSCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_GDSCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_GDSCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_GDSCR_IN)
#define HWIO_AOSS_CC_SWAO_GDSCR_PWR_ON_BMSK                                       0x80000000
#define HWIO_AOSS_CC_SWAO_GDSCR_PWR_ON_SHFT                                             0x1f
#define HWIO_AOSS_CC_SWAO_GDSCR_GDSC_STATE_BMSK                                   0x78000000
#define HWIO_AOSS_CC_SWAO_GDSCR_GDSC_STATE_SHFT                                         0x1b
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_REST_WAIT_BMSK                                   0xf00000
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_REST_WAIT_SHFT                                       0x14
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_FEW_WAIT_BMSK                                     0xf0000
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_FEW_WAIT_SHFT                                        0x10
#define HWIO_AOSS_CC_SWAO_GDSCR_CLK_DIS_WAIT_BMSK                                     0xf000
#define HWIO_AOSS_CC_SWAO_GDSCR_CLK_DIS_WAIT_SHFT                                        0xc
#define HWIO_AOSS_CC_SWAO_GDSCR_RETAIN_FF_ENABLE_BMSK                                  0x800
#define HWIO_AOSS_CC_SWAO_GDSCR_RETAIN_FF_ENABLE_SHFT                                    0xb
#define HWIO_AOSS_CC_SWAO_GDSCR_RESTORE_BMSK                                           0x400
#define HWIO_AOSS_CC_SWAO_GDSCR_RESTORE_SHFT                                             0xa
#define HWIO_AOSS_CC_SWAO_GDSCR_SAVE_BMSK                                              0x200
#define HWIO_AOSS_CC_SWAO_GDSCR_SAVE_SHFT                                                0x9
#define HWIO_AOSS_CC_SWAO_GDSCR_RETAIN_BMSK                                            0x100
#define HWIO_AOSS_CC_SWAO_GDSCR_RETAIN_SHFT                                              0x8
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_REST_BMSK                                            0x80
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_REST_SHFT                                             0x7
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_FEW_BMSK                                             0x40
#define HWIO_AOSS_CC_SWAO_GDSCR_EN_FEW_SHFT                                              0x6
#define HWIO_AOSS_CC_SWAO_GDSCR_CLAMP_IO_BMSK                                           0x20
#define HWIO_AOSS_CC_SWAO_GDSCR_CLAMP_IO_SHFT                                            0x5
#define HWIO_AOSS_CC_SWAO_GDSCR_CLK_DISABLE_BMSK                                        0x10
#define HWIO_AOSS_CC_SWAO_GDSCR_CLK_DISABLE_SHFT                                         0x4
#define HWIO_AOSS_CC_SWAO_GDSCR_PD_ARES_BMSK                                             0x8
#define HWIO_AOSS_CC_SWAO_GDSCR_PD_ARES_SHFT                                             0x3
#define HWIO_AOSS_CC_SWAO_GDSCR_SW_OVERRIDE_BMSK                                         0x4
#define HWIO_AOSS_CC_SWAO_GDSCR_SW_OVERRIDE_SHFT                                         0x2
#define HWIO_AOSS_CC_SWAO_GDSCR_HW_CONTROL_BMSK                                          0x2
#define HWIO_AOSS_CC_SWAO_GDSCR_HW_CONTROL_SHFT                                          0x1
#define HWIO_AOSS_CC_SWAO_GDSCR_SW_COLLAPSE_BMSK                                         0x1
#define HWIO_AOSS_CC_SWAO_GDSCR_SW_COLLAPSE_SHFT                                         0x0

#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002154)
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_RMSK                                          0xffffffff
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CFG_GDSCR_ADDR, HWIO_AOSS_CC_SWAO_CFG_GDSCR_RMSK)
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CFG_GDSCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_CFG_GDSCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_CFG_GDSCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_CFG_GDSCR_IN)
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_SPARE_CTRL_IN_BMSK                       0xf0000000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_SPARE_CTRL_IN_SHFT                             0x1c
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_BMSK                       0xc000000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_SHFT                            0x1a
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PWR_DWN_START_BMSK                        0x2000000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PWR_DWN_START_SHFT                             0x19
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PWR_UP_START_BMSK                         0x1000000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PWR_UP_START_SHFT                              0x18
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_BMSK                  0xf00000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_SHFT                      0x14
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_BMSK                     0x80000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_SHFT                        0x13
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_ENR_ACK_STATUS_BMSK                         0x40000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_ENR_ACK_STATUS_SHFT                            0x12
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_ENF_ACK_STATUS_BMSK                         0x20000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_ENF_ACK_STATUS_SHFT                            0x11
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_BMSK                      0x10000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_SHFT                         0x10
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_BMSK                     0x8000
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_SHFT                        0xf
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_BMSK                    0x7800
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_SHFT                       0xb
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_HANDSHAKE_DIS_BMSK                            0x400
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_HANDSHAKE_DIS_SHFT                              0xa
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_BMSK                     0x200
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_SHFT                       0x9
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_BMSK                     0x100
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_SHFT                       0x8
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_BMSK                         0x80
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_SHFT                          0x7
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_BMSK                0x60
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_SHFT                 0x5
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_BMSK                          0x10
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_SHFT                           0x4
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_BMSK                    0x8
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_SHFT                    0x3
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_BMSK                  0x4
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_SHFT                  0x2
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_BMSK                      0x2
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_SHFT                      0x1
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_BMSK                   0x1
#define HWIO_AOSS_CC_SWAO_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_SHFT                   0x0

#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_ADDR                                    (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002158)
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_RMSK                                    0x80000005
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_ADDR, HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_OFF_BMSK                            0x80000000
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_OFF_SHFT                                  0x1f
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_ARES_BMSK                                  0x4
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_ARES_SHFT                                  0x2
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_ENABLE_BMSK                                0x1
#define HWIO_AOSS_CC_SWAO_RPMH_DEBUG_CBCR_CLK_ENABLE_SHFT                                0x0

#define HWIO_AOSS_CC_SWAO_CBCR_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000215c)
#define HWIO_AOSS_CC_SWAO_CBCR_RMSK                                               0x80000005
#define HWIO_AOSS_CC_SWAO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CBCR_ADDR, HWIO_AOSS_CC_SWAO_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_OFF_BMSK                                       0x80000000
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_OFF_SHFT                                             0x1f
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_ARES_BMSK                                             0x4
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_ARES_SHFT                                             0x2
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_ENABLE_BMSK                                           0x1
#define HWIO_AOSS_CC_SWAO_CBCR_CLK_ENABLE_SHFT                                           0x0

#define HWIO_AOSS_CC_SWAO_AO_CBCR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002160)
#define HWIO_AOSS_CC_SWAO_AO_CBCR_RMSK                                            0x80000005
#define HWIO_AOSS_CC_SWAO_AO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_AO_CBCR_ADDR, HWIO_AOSS_CC_SWAO_AO_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_AO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_AO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_AO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_AO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_AO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_AO_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_AO_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_AOSS_CC_SWAO_AO_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_AOSS_CC_SWAO_MEM_CBCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002164)
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_RMSK                                           0x80007ff5
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_MEM_CBCR_ADDR, HWIO_AOSS_CC_SWAO_MEM_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_MEM_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_MEM_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_MEM_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_MEM_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_CORE_ON_BMSK                             0x4000
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_CORE_ON_SHFT                                0xe
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_PERIPH_ON_BMSK                           0x2000
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_PERIPH_ON_SHFT                              0xd
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                          0x1000
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                             0xc
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_WAKEUP_BMSK                                         0xf00
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_WAKEUP_SHFT                                           0x8
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_SLEEP_BMSK                                           0xf0
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_SLEEP_SHFT                                            0x4
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_AOSS_CC_SWAO_MEM_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_AOSS_CC_SWAO_TS_CBCR_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002168)
#define HWIO_AOSS_CC_SWAO_TS_CBCR_RMSK                                            0x80000005
#define HWIO_AOSS_CC_SWAO_TS_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CBCR_ADDR, HWIO_AOSS_CC_SWAO_TS_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_TS_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_TS_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_TS_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_TS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_TS_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_TS_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_OFF_BMSK                                    0x80000000
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_OFF_SHFT                                          0x1f
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_ARES_BMSK                                          0x4
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_ARES_SHFT                                          0x2
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_ENABLE_BMSK                                        0x1
#define HWIO_AOSS_CC_SWAO_TS_CBCR_CLK_ENABLE_SHFT                                        0x0

#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000216c)
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_RMSK                                         0x80000005
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_AO_CBCR_ADDR, HWIO_AOSS_CC_SWAO_TS_AO_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_AO_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_TS_AO_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_TS_AO_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_TS_AO_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_OFF_BMSK                                 0x80000000
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_OFF_SHFT                                       0x1f
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_ARES_BMSK                                       0x4
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_ARES_SHFT                                       0x2
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_ENABLE_BMSK                                     0x1
#define HWIO_AOSS_CC_SWAO_TS_AO_CBCR_CLK_ENABLE_SHFT                                     0x0

#define HWIO_AOSS_CC_SWAO_BUS_CBCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002170)
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_RMSK                                           0x80000005
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_BUS_CBCR_ADDR, HWIO_AOSS_CC_SWAO_BUS_CBCR_RMSK)
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_BUS_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_BUS_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_BUS_CBCR_ADDR,m,v,HWIO_AOSS_CC_SWAO_BUS_CBCR_IN)
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_AOSS_CC_SWAO_BUS_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_AOSS_CC_SWAO_CMD_RCGR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002174)
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_RMSK                                           0x80000013
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CMD_RCGR_ADDR, HWIO_AOSS_CC_SWAO_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_SWAO_CMD_RCGR_IN)
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_ROOT_OFF_BMSK                                  0x80000000
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_ROOT_OFF_SHFT                                        0x1f
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                  0x10
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                   0x4
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_ROOT_EN_BMSK                                          0x2
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_ROOT_EN_SHFT                                          0x1
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_UPDATE_BMSK                                           0x1
#define HWIO_AOSS_CC_SWAO_CMD_RCGR_UPDATE_SHFT                                           0x0

#define HWIO_AOSS_CC_SWAO_CFG_RCGR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002178)
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_RMSK                                             0x11071f
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CFG_RCGR_ADDR, HWIO_AOSS_CC_SWAO_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_SWAO_CFG_RCGR_IN)
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_HW_CLK_CONTROL_BMSK                              0x100000
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_HW_CLK_CONTROL_SHFT                                  0x14
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_RCGLITE_DISABLE_BMSK                              0x10000
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_RCGLITE_DISABLE_SHFT                                 0x10
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_SRC_SEL_BMSK                                        0x700
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_SRC_SEL_SHFT                                          0x8
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_SRC_DIV_BMSK                                         0x1f
#define HWIO_AOSS_CC_SWAO_CFG_RCGR_SRC_DIV_SHFT                                          0x0

#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000021b4)
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_RMSK                                        0x80000013
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ADDR, HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_IN)
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ROOT_OFF_BMSK                               0x80000000
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ROOT_OFF_SHFT                                     0x1f
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                               0x10
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                0x4
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ROOT_EN_BMSK                                       0x2
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_ROOT_EN_SHFT                                       0x1
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_UPDATE_BMSK                                        0x1
#define HWIO_AOSS_CC_SWAO_TS_CMD_RCGR_UPDATE_SHFT                                        0x0

#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000021b8)
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_RMSK                                          0x11071f
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_ADDR, HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_IN)
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_HW_CLK_CONTROL_BMSK                           0x100000
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_HW_CLK_CONTROL_SHFT                               0x14
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_RCGLITE_DISABLE_BMSK                           0x10000
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_RCGLITE_DISABLE_SHFT                              0x10
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_SRC_SEL_BMSK                                     0x700
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_SRC_SEL_SHFT                                       0x8
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_SRC_DIV_BMSK                                      0x1f
#define HWIO_AOSS_CC_SWAO_TS_CFG_RCGR_SRC_DIV_SHFT                                       0x0

#define HWIO_AOSS_CC_SPMI_BCR_ADDR                                                (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000021f4)
#define HWIO_AOSS_CC_SPMI_BCR_RMSK                                                       0x1
#define HWIO_AOSS_CC_SPMI_BCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_BCR_ADDR, HWIO_AOSS_CC_SPMI_BCR_RMSK)
#define HWIO_AOSS_CC_SPMI_BCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_BCR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_BCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_BCR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_BCR_ADDR,m,v,HWIO_AOSS_CC_SPMI_BCR_IN)
#define HWIO_AOSS_CC_SPMI_BCR_BLK_ARES_BMSK                                              0x1
#define HWIO_AOSS_CC_SPMI_BCR_BLK_ARES_SHFT                                              0x0

#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000021f8)
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_RMSK                                       0x80000005
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_ADDR, HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_RMSK)
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_ADDR,m,v,HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_IN)
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_AOSS_CC_SPMI_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_AOSS_CC_SPMI_SER_CBCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x000021fc)
#define HWIO_AOSS_CC_SPMI_SER_CBCR_RMSK                                           0x80000005
#define HWIO_AOSS_CC_SPMI_SER_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_SER_CBCR_ADDR, HWIO_AOSS_CC_SPMI_SER_CBCR_RMSK)
#define HWIO_AOSS_CC_SPMI_SER_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_SER_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_SER_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_SER_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_SER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_SER_CBCR_ADDR,m,v,HWIO_AOSS_CC_SPMI_SER_CBCR_IN)
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_AOSS_CC_SPMI_SER_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002200)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_RMSK                                       0x80000005
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_ADDR, HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_RMSK)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_ADDR,m,v,HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_IN)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_OFF_BMSK                               0x80000000
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_OFF_SHFT                                     0x1f
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_ARES_BMSK                                     0x4
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_ARES_SHFT                                     0x2
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_ENABLE_BMSK                                   0x1
#define HWIO_AOSS_CC_SPMI_AOD_SER_CBCR_CLK_ENABLE_SHFT                                   0x0

#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ADDR                                   (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002204)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_RMSK                                   0x80000013
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ADDR, HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_RMSK)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ADDR,m,v,HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_IN)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ROOT_OFF_BMSK                          0x80000000
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ROOT_OFF_SHFT                                0x1f
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                          0x10
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                           0x4
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ROOT_EN_BMSK                                  0x2
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_ROOT_EN_SHFT                                  0x1
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_UPDATE_BMSK                                   0x1
#define HWIO_AOSS_CC_SPMI_AOD_SER_CMD_RCGR_UPDATE_SHFT                                   0x0

#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_ADDR                                   (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002208)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_RMSK                                     0x11071f
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_ADDR, HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_RMSK)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_ADDR, m)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_ADDR,v)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_ADDR,m,v,HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_IN)
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_HW_CLK_CONTROL_BMSK                      0x100000
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_HW_CLK_CONTROL_SHFT                          0x14
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_RCGLITE_DISABLE_BMSK                      0x10000
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_RCGLITE_DISABLE_SHFT                         0x10
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_SRC_SEL_BMSK                                0x700
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_SRC_SEL_SHFT                                  0x8
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_SRC_DIV_BMSK                                 0x1f
#define HWIO_AOSS_CC_SPMI_AOD_SER_CFG_RCGR_SRC_DIV_SHFT                                  0x0

#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000223c)
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_RMSK                                                0x1
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_GLOBAL_CNTR_BCR_ADDR, HWIO_AOSS_CC_GLOBAL_CNTR_BCR_RMSK)
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_GLOBAL_CNTR_BCR_ADDR, m)
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_GLOBAL_CNTR_BCR_ADDR,v)
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_GLOBAL_CNTR_BCR_ADDR,m,v,HWIO_AOSS_CC_GLOBAL_CNTR_BCR_IN)
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_BLK_ARES_BMSK                                       0x1
#define HWIO_AOSS_CC_GLOBAL_CNTR_BCR_BLK_ARES_SHFT                                       0x0

#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002240)
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_RMSK                                        0x80000005
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_ADDR, HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_RMSK)
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_ADDR, m)
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_ADDR,v)
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_ADDR,m,v,HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_IN)
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_OFF_BMSK                                0x80000000
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_OFF_SHFT                                      0x1f
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_ARES_BMSK                                      0x4
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_ARES_SHFT                                      0x2
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_ENABLE_BMSK                                    0x1
#define HWIO_AOSS_CC_GLOBAL_CNTR_CBCR_CLK_ENABLE_SHFT                                    0x0

#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002258)
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_RMSK                                                0x3
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_DIV_CDIVR_ADDR, HWIO_AOSS_CC_DEBUG_DIV_CDIVR_RMSK)
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_DIV_CDIVR_ADDR, m)
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_DEBUG_DIV_CDIVR_ADDR,v)
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_DEBUG_DIV_CDIVR_ADDR,m,v,HWIO_AOSS_CC_DEBUG_DIV_CDIVR_IN)
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_CLK_DIV_BMSK                                        0x3
#define HWIO_AOSS_CC_DEBUG_DIV_CDIVR_CLK_DIV_SHFT                                        0x0

#define HWIO_AOSS_CC_DEBUG_CBCR_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000225c)
#define HWIO_AOSS_CC_DEBUG_CBCR_RMSK                                              0x80000005
#define HWIO_AOSS_CC_DEBUG_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_CBCR_ADDR, HWIO_AOSS_CC_DEBUG_CBCR_RMSK)
#define HWIO_AOSS_CC_DEBUG_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_CBCR_ADDR, m)
#define HWIO_AOSS_CC_DEBUG_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_DEBUG_CBCR_ADDR,v)
#define HWIO_AOSS_CC_DEBUG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_DEBUG_CBCR_ADDR,m,v,HWIO_AOSS_CC_DEBUG_CBCR_IN)
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_OFF_BMSK                                      0x80000000
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_OFF_SHFT                                            0x1f
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_ARES_BMSK                                            0x4
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_ARES_SHFT                                            0x2
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_ENABLE_BMSK                                          0x1
#define HWIO_AOSS_CC_DEBUG_CBCR_CLK_ENABLE_SHFT                                          0x0

#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002264)
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_RMSK                                             0x3
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_ADDR, HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_RMSK)
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_ADDR, m)
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_ADDR,v)
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_ADDR,m,v,HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_IN)
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_BMSK                                     0x3
#define HWIO_AOSS_CC_PLL_TEST_DIV_CDIVR_CLK_DIV_SHFT                                     0x0

#define HWIO_AOSS_CC_PLL_TEST_CBCR_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00002268)
#define HWIO_AOSS_CC_PLL_TEST_CBCR_RMSK                                           0x80000005
#define HWIO_AOSS_CC_PLL_TEST_CBCR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_CBCR_ADDR, HWIO_AOSS_CC_PLL_TEST_CBCR_RMSK)
#define HWIO_AOSS_CC_PLL_TEST_CBCR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_CBCR_ADDR, m)
#define HWIO_AOSS_CC_PLL_TEST_CBCR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_TEST_CBCR_ADDR,v)
#define HWIO_AOSS_CC_PLL_TEST_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_TEST_CBCR_ADDR,m,v,HWIO_AOSS_CC_PLL_TEST_CBCR_IN)
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_OFF_BMSK                                   0x80000000
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_OFF_SHFT                                         0x1f
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_ARES_BMSK                                         0x4
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_ARES_SHFT                                         0x2
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_ENABLE_BMSK                                       0x1
#define HWIO_AOSS_CC_PLL_TEST_CBCR_CLK_ENABLE_SHFT                                       0x0

#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003004)
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_RMSK                                                0xff
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_IN          \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_MUX_MUXR_ADDR, HWIO_AOSS_CC_DEBUG_MUX_MUXR_RMSK)
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_MUX_MUXR_ADDR, m)
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_DEBUG_MUX_MUXR_ADDR,v)
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_DEBUG_MUX_MUXR_ADDR,m,v,HWIO_AOSS_CC_DEBUG_MUX_MUXR_IN)
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_MUX_SEL_BMSK                                        0xff
#define HWIO_AOSS_CC_DEBUG_MUX_MUXR_MUX_SEL_SHFT                                         0x0

#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003008)
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_RMSK                                              0x7
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_ADDR, HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_RMSK)
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_ADDR, m)
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_ADDR,v)
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_ADDR,m,v,HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_IN)
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_BMSK                                      0x7
#define HWIO_AOSS_CC_PLL_TEST_MUX_MUXR_MUX_SEL_SHFT                                      0x0

#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000300c)
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_RMSK                                               0x7
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_RESET_N_MUXR_ADDR, HWIO_AOSS_CC_PLL_RESET_N_MUXR_RMSK)
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_RESET_N_MUXR_ADDR, m)
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_RESET_N_MUXR_ADDR,v)
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_RESET_N_MUXR_ADDR,m,v,HWIO_AOSS_CC_PLL_RESET_N_MUXR_IN)
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_MUX_SEL_BMSK                                       0x7
#define HWIO_AOSS_CC_PLL_RESET_N_MUXR_MUX_SEL_SHFT                                       0x0

#define HWIO_AOSS_CC_PLL_STATUS_MUXR_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003010)
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_RMSK                                                0x7
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_STATUS_MUXR_ADDR, HWIO_AOSS_CC_PLL_STATUS_MUXR_RMSK)
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_STATUS_MUXR_ADDR, m)
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_STATUS_MUXR_ADDR,v)
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_STATUS_MUXR_ADDR,m,v,HWIO_AOSS_CC_PLL_STATUS_MUXR_IN)
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_MUX_SEL_BMSK                                        0x7
#define HWIO_AOSS_CC_PLL_STATUS_MUXR_MUX_SEL_SHFT                                        0x0

#define HWIO_AOSS_CC_PLL_CLK_SEL_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003014)
#define HWIO_AOSS_CC_PLL_CLK_SEL_RMSK                                                    0x3
#define HWIO_AOSS_CC_PLL_CLK_SEL_IN          \
        in_dword_masked(HWIO_AOSS_CC_PLL_CLK_SEL_ADDR, HWIO_AOSS_CC_PLL_CLK_SEL_RMSK)
#define HWIO_AOSS_CC_PLL_CLK_SEL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PLL_CLK_SEL_ADDR, m)
#define HWIO_AOSS_CC_PLL_CLK_SEL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PLL_CLK_SEL_ADDR,v)
#define HWIO_AOSS_CC_PLL_CLK_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PLL_CLK_SEL_ADDR,m,v,HWIO_AOSS_CC_PLL_CLK_SEL_IN)
#define HWIO_AOSS_CC_PLL_CLK_SEL_PIPPO_CLK_XO_SEL_BMSK                                   0x2
#define HWIO_AOSS_CC_PLL_CLK_SEL_PIPPO_CLK_XO_SEL_SHFT                                   0x1
#define HWIO_AOSS_CC_PLL_CLK_SEL_FABIA_CLK_REF_SEL_BMSK                                  0x1
#define HWIO_AOSS_CC_PLL_CLK_SEL_FABIA_CLK_REF_SEL_SHFT                                  0x0

#define HWIO_AOSS_CC_CDBGPWRUPREQ_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003018)
#define HWIO_AOSS_CC_CDBGPWRUPREQ_RMSK                                            0x80000001
#define HWIO_AOSS_CC_CDBGPWRUPREQ_IN          \
        in_dword_masked(HWIO_AOSS_CC_CDBGPWRUPREQ_ADDR, HWIO_AOSS_CC_CDBGPWRUPREQ_RMSK)
#define HWIO_AOSS_CC_CDBGPWRUPREQ_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_CDBGPWRUPREQ_ADDR, m)
#define HWIO_AOSS_CC_CDBGPWRUPREQ_OUT(v)      \
        out_dword(HWIO_AOSS_CC_CDBGPWRUPREQ_ADDR,v)
#define HWIO_AOSS_CC_CDBGPWRUPREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_CDBGPWRUPREQ_ADDR,m,v,HWIO_AOSS_CC_CDBGPWRUPREQ_IN)
#define HWIO_AOSS_CC_CDBGPWRUPREQ_STATUS_BMSK                                     0x80000000
#define HWIO_AOSS_CC_CDBGPWRUPREQ_STATUS_SHFT                                           0x1f
#define HWIO_AOSS_CC_CDBGPWRUPREQ_REQ_BMSK                                               0x1
#define HWIO_AOSS_CC_CDBGPWRUPREQ_REQ_SHFT                                               0x0

#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050000)
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_RMSK                                          0x1fff
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_IN          \
        in_dword_masked(HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ADDR, HWIO_AOSS_CC_RESET_DEBUG_ENABLE_RMSK)
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ADDR, m)
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ADDR,v)
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ADDR,m,v,HWIO_AOSS_CC_RESET_DEBUG_ENABLE_IN)
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_SW_BASED_PRE_ARES_BMSK                        0x1000
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_SW_BASED_PRE_ARES_SHFT                           0xc
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ALLOW_SW_BASED_RST_DBG_EN_BMSK                 0x800
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ALLOW_SW_BASED_RST_DBG_EN_SHFT                   0xb
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PRE_SW_SRST_TMR_EN_BMSK                        0x400
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PRE_SW_SRST_TMR_EN_SHFT                          0xa
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_EUD_T32_SRST_EN_BMSK                           0x200
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_EUD_T32_SRST_EN_SHFT                             0x9
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_EUD_RESET_SECOND_PASS_EN_BMSK                  0x100
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_EUD_RESET_SECOND_PASS_EN_SHFT                    0x8
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_DISABLE_APCS_ALT_ARES_BMSK                      0x80
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_DISABLE_APCS_ALT_ARES_SHFT                       0x7
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_SECURE_WDOG_RESET_DEBUG_EN_BMSK                 0x40
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_SECURE_WDOG_RESET_DEBUG_EN_SHFT                  0x6
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_MSM_TSENSE_RESET_DEBUG_EN_BMSK                  0x20
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_MSM_TSENSE_RESET_DEBUG_EN_SHFT                   0x5
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PMIC_RESIN_RESET_DEBUG_EN_BMSK                  0x10
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PMIC_RESIN_RESET_DEBUG_EN_SHFT                   0x4
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PROC_HALT_EN_BMSK                                0x8
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_PROC_HALT_EN_SHFT                                0x3
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_AUTO_PS_HOLD_SET_EN_BMSK                         0x4
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_AUTO_PS_HOLD_SET_EN_SHFT                         0x2
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ACCESS_RESET_FIRST_PASS_EN_BMSK                  0x2
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_ACCESS_RESET_FIRST_PASS_EN_SHFT                  0x1
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_DEBUG_RESET_FIRST_PASS_EN_BMSK                   0x1
#define HWIO_AOSS_CC_RESET_DEBUG_ENABLE_DEBUG_RESET_FIRST_PASS_EN_SHFT                   0x0

#define HWIO_AOSS_CC_RESET_FSM_CTRL_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050004)
#define HWIO_AOSS_CC_RESET_FSM_CTRL_RMSK                                                0x1f
#define HWIO_AOSS_CC_RESET_FSM_CTRL_IN          \
        in_dword_masked(HWIO_AOSS_CC_RESET_FSM_CTRL_ADDR, HWIO_AOSS_CC_RESET_FSM_CTRL_RMSK)
#define HWIO_AOSS_CC_RESET_FSM_CTRL_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RESET_FSM_CTRL_ADDR, m)
#define HWIO_AOSS_CC_RESET_FSM_CTRL_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RESET_FSM_CTRL_ADDR,v)
#define HWIO_AOSS_CC_RESET_FSM_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RESET_FSM_CTRL_ADDR,m,v,HWIO_AOSS_CC_RESET_FSM_CTRL_IN)
#define HWIO_AOSS_CC_RESET_FSM_CTRL_WKUP_TMR_EN_BMSK                                    0x10
#define HWIO_AOSS_CC_RESET_FSM_CTRL_WKUP_TMR_EN_SHFT                                     0x4
#define HWIO_AOSS_CC_RESET_FSM_CTRL_NORMAL_WKUP_EN_BMSK                                  0x8
#define HWIO_AOSS_CC_RESET_FSM_CTRL_NORMAL_WKUP_EN_SHFT                                  0x3
#define HWIO_AOSS_CC_RESET_FSM_CTRL_FIRST_PASS_TMR_EN_BMSK                               0x4
#define HWIO_AOSS_CC_RESET_FSM_CTRL_FIRST_PASS_TMR_EN_SHFT                               0x2
#define HWIO_AOSS_CC_RESET_FSM_CTRL_FIRST_PASS_COMPLETE_BMSK                             0x2
#define HWIO_AOSS_CC_RESET_FSM_CTRL_FIRST_PASS_COMPLETE_SHFT                             0x1
#define HWIO_AOSS_CC_RESET_FSM_CTRL_BLOCK_RESIN_BMSK                                     0x1
#define HWIO_AOSS_CC_RESET_FSM_CTRL_BLOCK_RESIN_SHFT                                     0x0

#define HWIO_AOSS_CC_WIND_DOWN_TIMER_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050008)
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_RMSK                                         0xffffffff
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_IN          \
        in_dword_masked(HWIO_AOSS_CC_WIND_DOWN_TIMER_ADDR, HWIO_AOSS_CC_WIND_DOWN_TIMER_RMSK)
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_WIND_DOWN_TIMER_ADDR, m)
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_OUT(v)      \
        out_dword(HWIO_AOSS_CC_WIND_DOWN_TIMER_ADDR,v)
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_WIND_DOWN_TIMER_ADDR,m,v,HWIO_AOSS_CC_WIND_DOWN_TIMER_IN)
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_RESERVE_BITS31_16_BMSK                       0xffff0000
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_RESERVE_BITS31_16_SHFT                             0x10
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_TIMER_VAL_BMSK                                   0xffff
#define HWIO_AOSS_CC_WIND_DOWN_TIMER_TIMER_VAL_SHFT                                      0x0

#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_ADDR                                     (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0005000c)
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_RMSK                                     0xffffffff
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_IN          \
        in_dword_masked(HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_ADDR, HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_RMSK)
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_ADDR, m)
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_ADDR,v)
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_ADDR,m,v,HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_IN)
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_RESERVE_BITS31_16_BMSK                   0xffff0000
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_RESERVE_BITS31_16_SHFT                         0x10
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_TIMER_VAL_BMSK                               0xffff
#define HWIO_AOSS_CC_SAVE_CONTENTS_TIMER_TIMER_VAL_SHFT                                  0x0

#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050010)
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_RMSK                                      0xffffffff
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_IN          \
        in_dword_masked(HWIO_AOSS_CC_STOP_CAPTURE_TIMER_ADDR, HWIO_AOSS_CC_STOP_CAPTURE_TIMER_RMSK)
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_STOP_CAPTURE_TIMER_ADDR, m)
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_OUT(v)      \
        out_dword(HWIO_AOSS_CC_STOP_CAPTURE_TIMER_ADDR,v)
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_STOP_CAPTURE_TIMER_ADDR,m,v,HWIO_AOSS_CC_STOP_CAPTURE_TIMER_IN)
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_RESERVE_BITS31_16_BMSK                    0xffff0000
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_RESERVE_BITS31_16_SHFT                          0x10
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_TIMER_VAL_BMSK                                0xffff
#define HWIO_AOSS_CC_STOP_CAPTURE_TIMER_TIMER_VAL_SHFT                                   0x0

#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_ADDR                                 (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050014)
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_RMSK                                 0xffffffff
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_IN          \
        in_dword_masked(HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_ADDR, HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_RMSK)
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_ADDR, m)
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_ADDR,v)
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_ADDR,m,v,HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_IN)
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_RESERVE_BITS31_16_BMSK               0xffff0000
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_RESERVE_BITS31_16_SHFT                     0x10
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_TIMER_VAL_BMSK                           0xffff
#define HWIO_AOSS_CC_PREPARE_FOR_RESET_TIMER_TIMER_VAL_SHFT                              0x0

#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_ADDR                                      (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050018)
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_RMSK                                      0xffffffff
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_IN          \
        in_dword_masked(HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_ADDR, HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_RMSK)
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_ADDR, m)
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_OUT(v)      \
        out_dword(HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_ADDR,v)
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_ADDR,m,v,HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_IN)
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_TIMER_VAL_BMSK                            0xffffffff
#define HWIO_AOSS_CC_FIRST_PASS_TIMEOUT_TIMER_VAL_SHFT                                   0x0

#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_ADDR                                     (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0005001c)
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_RMSK                                     0xffffffff
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_IN          \
        in_dword_masked(HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_ADDR, HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_RMSK)
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_ADDR, m)
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_OUT(v)      \
        out_dword(HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_ADDR,v)
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_ADDR,m,v,HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_IN)
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_RESERVE_BITS31_8_BMSK                    0xffffff00
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_RESERVE_BITS31_8_SHFT                           0x8
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_TIMER_VAL_BMSK                                 0xff
#define HWIO_AOSS_CC_PRE_SW_SRST_TIMEOUT_TIMER_VAL_SHFT                                  0x0

#define HWIO_AOSS_CC_RESET_STATUS_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050020)
#define HWIO_AOSS_CC_RESET_STATUS_RMSK                                                 0x3ff
#define HWIO_AOSS_CC_RESET_STATUS_IN          \
        in_dword_masked(HWIO_AOSS_CC_RESET_STATUS_ADDR, HWIO_AOSS_CC_RESET_STATUS_RMSK)
#define HWIO_AOSS_CC_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RESET_STATUS_ADDR, m)
#define HWIO_AOSS_CC_RESET_STATUS_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RESET_STATUS_ADDR,v)
#define HWIO_AOSS_CC_RESET_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RESET_STATUS_ADDR,m,v,HWIO_AOSS_CC_RESET_STATUS_IN)
#define HWIO_AOSS_CC_RESET_STATUS_PS_HOLD_STATUS_BMSK                                  0x200
#define HWIO_AOSS_CC_RESET_STATUS_PS_HOLD_STATUS_SHFT                                    0x9
#define HWIO_AOSS_CC_RESET_STATUS_QDSS_SW_SRST_RESET_STATUS_BMSK                       0x100
#define HWIO_AOSS_CC_RESET_STATUS_QDSS_SW_SRST_RESET_STATUS_SHFT                         0x8
#define HWIO_AOSS_CC_RESET_STATUS_EUD_SRST_RESET_STATUS_BMSK                            0x80
#define HWIO_AOSS_CC_RESET_STATUS_EUD_SRST_RESET_STATUS_SHFT                             0x7
#define HWIO_AOSS_CC_RESET_STATUS_PMIC_ABNORMAL_RESIN_RESET_STATUS_BMSK                 0x40
#define HWIO_AOSS_CC_RESET_STATUS_PMIC_ABNORMAL_RESIN_RESET_STATUS_SHFT                  0x6
#define HWIO_AOSS_CC_RESET_STATUS_MSM_TSENSE1_RESET_STATUS_BMSK                         0x20
#define HWIO_AOSS_CC_RESET_STATUS_MSM_TSENSE1_RESET_STATUS_SHFT                          0x5
#define HWIO_AOSS_CC_RESET_STATUS_PROC_HALT_CTI_STATUS_BMSK                             0x10
#define HWIO_AOSS_CC_RESET_STATUS_PROC_HALT_CTI_STATUS_SHFT                              0x4
#define HWIO_AOSS_CC_RESET_STATUS_SRST_RESET_STATUS_BMSK                                 0x8
#define HWIO_AOSS_CC_RESET_STATUS_SRST_RESET_STATUS_SHFT                                 0x3
#define HWIO_AOSS_CC_RESET_STATUS_MSM_TSENSE0_RESET_STATUS_BMSK                          0x4
#define HWIO_AOSS_CC_RESET_STATUS_MSM_TSENSE0_RESET_STATUS_SHFT                          0x2
#define HWIO_AOSS_CC_RESET_STATUS_PMIC_RESIN_RESET_STATUS_BMSK                           0x2
#define HWIO_AOSS_CC_RESET_STATUS_PMIC_RESIN_RESET_STATUS_SHFT                           0x1
#define HWIO_AOSS_CC_RESET_STATUS_SECURE_WDOG_EXPIRE_RESET_STATUS_BMSK                   0x1
#define HWIO_AOSS_CC_RESET_STATUS_SECURE_WDOG_EXPIRE_RESET_STATUS_SHFT                   0x0

#define HWIO_AOSS_CC_RESET_FSM_STATUS_ADDR                                        (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050024)
#define HWIO_AOSS_CC_RESET_FSM_STATUS_RMSK                                            0x3fff
#define HWIO_AOSS_CC_RESET_FSM_STATUS_IN          \
        in_dword_masked(HWIO_AOSS_CC_RESET_FSM_STATUS_ADDR, HWIO_AOSS_CC_RESET_FSM_STATUS_RMSK)
#define HWIO_AOSS_CC_RESET_FSM_STATUS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_RESET_FSM_STATUS_ADDR, m)
#define HWIO_AOSS_CC_RESET_FSM_STATUS_OUT(v)      \
        out_dword(HWIO_AOSS_CC_RESET_FSM_STATUS_ADDR,v)
#define HWIO_AOSS_CC_RESET_FSM_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_RESET_FSM_STATUS_ADDR,m,v,HWIO_AOSS_CC_RESET_FSM_STATUS_IN)
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_PDC_HANG_STATUS_BMSK                        0x2000
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_PDC_HANG_STATUS_SHFT                           0xd
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_PDC_TRIGGER_BMSK                            0x1000
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_PDC_TRIGGER_SHFT                               0xc
#define HWIO_AOSS_CC_RESET_FSM_STATUS_NORM_WKUP_HANG_STATUS_BMSK                       0x800
#define HWIO_AOSS_CC_RESET_FSM_STATUS_NORM_WKUP_HANG_STATUS_SHFT                         0xb
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_EXPIRE_BMSK                           0x400
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_EXPIRE_SHFT                             0xa
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_TIMER_LD_BMSK                         0x200
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_TIMER_LD_SHFT                           0x9
#define HWIO_AOSS_CC_RESET_FSM_STATUS_BLOCK_RESIN_ASET_BMSK                            0x100
#define HWIO_AOSS_CC_RESET_FSM_STATUS_BLOCK_RESIN_ASET_SHFT                              0x8
#define HWIO_AOSS_CC_RESET_FSM_STATUS_BLOCK_RESIN_ARES_BMSK                             0x80
#define HWIO_AOSS_CC_RESET_FSM_STATUS_BLOCK_RESIN_ARES_SHFT                              0x7
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_TIMER_TRIGGER_BMSK                            0x40
#define HWIO_AOSS_CC_RESET_FSM_STATUS_DBG_TIMER_TRIGGER_SHFT                             0x6
#define HWIO_AOSS_CC_RESET_FSM_STATUS_RESET_DEBUG_READY_BMSK                            0x20
#define HWIO_AOSS_CC_RESET_FSM_STATUS_RESET_DEBUG_READY_SHFT                             0x5
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_BMSK                                   0x10
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FIRST_PASS_SHFT                                    0x4
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FSM_STATE_BMSK                                     0xf
#define HWIO_AOSS_CC_RESET_FSM_STATUS_FSM_STATE_SHFT                                     0x0

#define HWIO_AOSS_CC_TIC_MISC_ADDR                                                (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050028)
#define HWIO_AOSS_CC_TIC_MISC_RMSK                                                       0x3
#define HWIO_AOSS_CC_TIC_MISC_IN          \
        in_dword_masked(HWIO_AOSS_CC_TIC_MISC_ADDR, HWIO_AOSS_CC_TIC_MISC_RMSK)
#define HWIO_AOSS_CC_TIC_MISC_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_TIC_MISC_ADDR, m)
#define HWIO_AOSS_CC_TIC_MISC_OUT(v)      \
        out_dword(HWIO_AOSS_CC_TIC_MISC_ADDR,v)
#define HWIO_AOSS_CC_TIC_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_TIC_MISC_ADDR,m,v,HWIO_AOSS_CC_TIC_MISC_IN)
#define HWIO_AOSS_CC_TIC_MISC_APCS_SANS_BOOT_PROC_RESET_IN_TIC_BMSK                      0x2
#define HWIO_AOSS_CC_TIC_MISC_APCS_SANS_BOOT_PROC_RESET_IN_TIC_SHFT                      0x1
#define HWIO_AOSS_CC_TIC_MISC_BOOT_PROC_RESET_IN_TIC_BMSK                                0x1
#define HWIO_AOSS_CC_TIC_MISC_BOOT_PROC_RESET_IN_TIC_SHFT                                0x0

#define HWIO_AOSS_CC_APCS_MISC_ADDR                                               (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0005002c)
#define HWIO_AOSS_CC_APCS_MISC_RMSK                                                      0x1
#define HWIO_AOSS_CC_APCS_MISC_IN          \
        in_dword_masked(HWIO_AOSS_CC_APCS_MISC_ADDR, HWIO_AOSS_CC_APCS_MISC_RMSK)
#define HWIO_AOSS_CC_APCS_MISC_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_APCS_MISC_ADDR, m)
#define HWIO_AOSS_CC_APCS_MISC_OUT(v)      \
        out_dword(HWIO_AOSS_CC_APCS_MISC_ADDR,v)
#define HWIO_AOSS_CC_APCS_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_APCS_MISC_ADDR,m,v,HWIO_AOSS_CC_APCS_MISC_IN)
#define HWIO_AOSS_CC_APCS_MISC_AOP_RESET_BMSK                                            0x1
#define HWIO_AOSS_CC_APCS_MISC_AOP_RESET_SHFT                                            0x0

#define HWIO_AOSS_CC_APCS_RESET_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050030)
#define HWIO_AOSS_CC_APCS_RESET_RMSK                                                     0x3
#define HWIO_AOSS_CC_APCS_RESET_IN          \
        in_dword_masked(HWIO_AOSS_CC_APCS_RESET_ADDR, HWIO_AOSS_CC_APCS_RESET_RMSK)
#define HWIO_AOSS_CC_APCS_RESET_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_APCS_RESET_ADDR, m)
#define HWIO_AOSS_CC_APCS_RESET_OUT(v)      \
        out_dword(HWIO_AOSS_CC_APCS_RESET_ADDR,v)
#define HWIO_AOSS_CC_APCS_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_APCS_RESET_ADDR,m,v,HWIO_AOSS_CC_APCS_RESET_IN)
#define HWIO_AOSS_CC_APCS_RESET_APCS_SANS_BOOT_PROC_RESET_BMSK                           0x2
#define HWIO_AOSS_CC_APCS_RESET_APCS_SANS_BOOT_PROC_RESET_SHFT                           0x1
#define HWIO_AOSS_CC_APCS_RESET_BOOT_PROC_RESET_BMSK                                     0x1
#define HWIO_AOSS_CC_APCS_RESET_BOOT_PROC_RESET_SHFT                                     0x0

#define HWIO_AOSS_CC_MSS_RESTART_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00010000)
#define HWIO_AOSS_CC_MSS_RESTART_RMSK                                                    0x1
#define HWIO_AOSS_CC_MSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_MSS_RESTART_ADDR, HWIO_AOSS_CC_MSS_RESTART_RMSK)
#define HWIO_AOSS_CC_MSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_MSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_MSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_MSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_MSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_MSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_MSS_RESTART_IN)
#define HWIO_AOSS_CC_MSS_RESTART_SS_RESTART_BMSK                                         0x1
#define HWIO_AOSS_CC_MSS_RESTART_SS_RESTART_SHFT                                         0x0

#define HWIO_AOSS_CC_CAMSS_RESTART_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00011000)
#define HWIO_AOSS_CC_CAMSS_RESTART_RMSK                                                  0x1
#define HWIO_AOSS_CC_CAMSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_CAMSS_RESTART_ADDR, HWIO_AOSS_CC_CAMSS_RESTART_RMSK)
#define HWIO_AOSS_CC_CAMSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_CAMSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_CAMSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_CAMSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_CAMSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_CAMSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_CAMSS_RESTART_IN)
#define HWIO_AOSS_CC_CAMSS_RESTART_SS_RESTART_BMSK                                       0x1
#define HWIO_AOSS_CC_CAMSS_RESTART_SS_RESTART_SHFT                                       0x0

#define HWIO_AOSS_CC_VENUS_RESTART_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00012000)
#define HWIO_AOSS_CC_VENUS_RESTART_RMSK                                                  0x1
#define HWIO_AOSS_CC_VENUS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_VENUS_RESTART_ADDR, HWIO_AOSS_CC_VENUS_RESTART_RMSK)
#define HWIO_AOSS_CC_VENUS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_VENUS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_VENUS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_VENUS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_VENUS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_VENUS_RESTART_ADDR,m,v,HWIO_AOSS_CC_VENUS_RESTART_IN)
#define HWIO_AOSS_CC_VENUS_RESTART_SS_RESTART_BMSK                                       0x1
#define HWIO_AOSS_CC_VENUS_RESTART_SS_RESTART_SHFT                                       0x0

#define HWIO_AOSS_CC_GPU_RESTART_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00013000)
#define HWIO_AOSS_CC_GPU_RESTART_RMSK                                                    0x1
#define HWIO_AOSS_CC_GPU_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_GPU_RESTART_ADDR, HWIO_AOSS_CC_GPU_RESTART_RMSK)
#define HWIO_AOSS_CC_GPU_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_GPU_RESTART_ADDR, m)
#define HWIO_AOSS_CC_GPU_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_GPU_RESTART_ADDR,v)
#define HWIO_AOSS_CC_GPU_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_GPU_RESTART_ADDR,m,v,HWIO_AOSS_CC_GPU_RESTART_IN)
#define HWIO_AOSS_CC_GPU_RESTART_SS_RESTART_BMSK                                         0x1
#define HWIO_AOSS_CC_GPU_RESTART_SS_RESTART_SHFT                                         0x0

#define HWIO_AOSS_CC_DISPSS_RESTART_ADDR                                          (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00014000)
#define HWIO_AOSS_CC_DISPSS_RESTART_RMSK                                                 0x1
#define HWIO_AOSS_CC_DISPSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_DISPSS_RESTART_ADDR, HWIO_AOSS_CC_DISPSS_RESTART_RMSK)
#define HWIO_AOSS_CC_DISPSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_DISPSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_DISPSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_DISPSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_DISPSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_DISPSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_DISPSS_RESTART_IN)
#define HWIO_AOSS_CC_DISPSS_RESTART_SS_RESTART_BMSK                                      0x1
#define HWIO_AOSS_CC_DISPSS_RESTART_SS_RESTART_SHFT                                      0x0

#define HWIO_AOSS_CC_WCSS_RESTART_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00020000)
#define HWIO_AOSS_CC_WCSS_RESTART_RMSK                                                   0x1
#define HWIO_AOSS_CC_WCSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_WCSS_RESTART_ADDR, HWIO_AOSS_CC_WCSS_RESTART_RMSK)
#define HWIO_AOSS_CC_WCSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_WCSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_WCSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_WCSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_WCSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_WCSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_WCSS_RESTART_IN)
#define HWIO_AOSS_CC_WCSS_RESTART_SS_RESTART_BMSK                                        0x1
#define HWIO_AOSS_CC_WCSS_RESTART_SS_RESTART_SHFT                                        0x0

#define HWIO_AOSS_CC_GSS_RESTART_ADDR                                             (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00021000)
#define HWIO_AOSS_CC_GSS_RESTART_RMSK                                                    0x1
#define HWIO_AOSS_CC_GSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_GSS_RESTART_ADDR, HWIO_AOSS_CC_GSS_RESTART_RMSK)
#define HWIO_AOSS_CC_GSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_GSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_GSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_GSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_GSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_GSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_GSS_RESTART_IN)
#define HWIO_AOSS_CC_GSS_RESTART_SS_RESTART_BMSK                                         0x1
#define HWIO_AOSS_CC_GSS_RESTART_SS_RESTART_SHFT                                         0x0

#define HWIO_AOSS_CC_LPASS_RESTART_ADDR                                           (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00030000)
#define HWIO_AOSS_CC_LPASS_RESTART_RMSK                                                  0x1
#define HWIO_AOSS_CC_LPASS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_LPASS_RESTART_ADDR, HWIO_AOSS_CC_LPASS_RESTART_RMSK)
#define HWIO_AOSS_CC_LPASS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_LPASS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_LPASS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_LPASS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_LPASS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_LPASS_RESTART_ADDR,m,v,HWIO_AOSS_CC_LPASS_RESTART_IN)
#define HWIO_AOSS_CC_LPASS_RESTART_SS_RESTART_BMSK                                       0x1
#define HWIO_AOSS_CC_LPASS_RESTART_SS_RESTART_SHFT                                       0x0

#define HWIO_AOSS_CC_SENSORS_RESTART_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00031000)
#define HWIO_AOSS_CC_SENSORS_RESTART_RMSK                                                0x1
#define HWIO_AOSS_CC_SENSORS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_SENSORS_RESTART_ADDR, HWIO_AOSS_CC_SENSORS_RESTART_RMSK)
#define HWIO_AOSS_CC_SENSORS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SENSORS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_SENSORS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SENSORS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_SENSORS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SENSORS_RESTART_ADDR,m,v,HWIO_AOSS_CC_SENSORS_RESTART_IN)
#define HWIO_AOSS_CC_SENSORS_RESTART_SS_RESTART_BMSK                                     0x1
#define HWIO_AOSS_CC_SENSORS_RESTART_SS_RESTART_SHFT                                     0x0

#define HWIO_AOSS_CC_SPSS_RESTART_ADDR                                            (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00032000)
#define HWIO_AOSS_CC_SPSS_RESTART_RMSK                                                   0x1
#define HWIO_AOSS_CC_SPSS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPSS_RESTART_ADDR, HWIO_AOSS_CC_SPSS_RESTART_RMSK)
#define HWIO_AOSS_CC_SPSS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPSS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_SPSS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPSS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_SPSS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPSS_RESTART_ADDR,m,v,HWIO_AOSS_CC_SPSS_RESTART_IN)
#define HWIO_AOSS_CC_SPSS_RESTART_SS_RESTART_BMSK                                        0x1
#define HWIO_AOSS_CC_SPSS_RESTART_SS_RESTART_SHFT                                        0x0

#define HWIO_AOSS_CC_COMPUTESS_RESTART_ADDR                                       (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00040000)
#define HWIO_AOSS_CC_COMPUTESS_RESTART_RMSK                                              0x1
#define HWIO_AOSS_CC_COMPUTESS_RESTART_IN          \
        in_dword_masked(HWIO_AOSS_CC_COMPUTESS_RESTART_ADDR, HWIO_AOSS_CC_COMPUTESS_RESTART_RMSK)
#define HWIO_AOSS_CC_COMPUTESS_RESTART_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_COMPUTESS_RESTART_ADDR, m)
#define HWIO_AOSS_CC_COMPUTESS_RESTART_OUT(v)      \
        out_dword(HWIO_AOSS_CC_COMPUTESS_RESTART_ADDR,v)
#define HWIO_AOSS_CC_COMPUTESS_RESTART_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_COMPUTESS_RESTART_ADDR,m,v,HWIO_AOSS_CC_COMPUTESS_RESTART_IN)
#define HWIO_AOSS_CC_COMPUTESS_RESTART_SS_RESTART_BMSK                                   0x1
#define HWIO_AOSS_CC_COMPUTESS_RESTART_SS_RESTART_SHFT                                   0x0

#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_ADDR                                     (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050034)
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_RMSK                                     0xffffffff
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_IN          \
        in_dword_masked(HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_ADDR, HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_RMSK)
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_ADDR, m)
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_OUT(v)      \
        out_dword(HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_ADDR,v)
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_ADDR,m,v,HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_IN)
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_TIMER_VAL_BMSK                           0xffffffff
#define HWIO_AOSS_CC_ABNORM_WKUP_TIMEOUT_TIMER_VAL_SHFT                                  0x0

#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_ADDR                                         (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x0000301c)
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_RMSK                                                0x1
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_IN          \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_HW_EVENTS_ADDR, HWIO_AOSS_CC_DEBUG_HW_EVENTS_RMSK)
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_DEBUG_HW_EVENTS_ADDR, m)
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_OUT(v)      \
        out_dword(HWIO_AOSS_CC_DEBUG_HW_EVENTS_ADDR,v)
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_DEBUG_HW_EVENTS_ADDR,m,v,HWIO_AOSS_CC_DEBUG_HW_EVENTS_IN)
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_MUX_SEL_BMSK                                        0x1
#define HWIO_AOSS_CC_DEBUG_HW_EVENTS_MUX_SEL_SHFT                                        0x0

#define HWIO_AOSS_CC_AOP_MISC_ADDR                                                (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003020)
#define HWIO_AOSS_CC_AOP_MISC_RMSK                                                       0x1
#define HWIO_AOSS_CC_AOP_MISC_IN          \
        in_dword_masked(HWIO_AOSS_CC_AOP_MISC_ADDR, HWIO_AOSS_CC_AOP_MISC_RMSK)
#define HWIO_AOSS_CC_AOP_MISC_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_AOP_MISC_ADDR, m)
#define HWIO_AOSS_CC_AOP_MISC_OUT(v)      \
        out_dword(HWIO_AOSS_CC_AOP_MISC_ADDR,v)
#define HWIO_AOSS_CC_AOP_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_AOP_MISC_ADDR,m,v,HWIO_AOSS_CC_AOP_MISC_IN)
#define HWIO_AOSS_CC_AOP_MISC_AUTO_SCALE_DIS_BMSK                                        0x1
#define HWIO_AOSS_CC_AOP_MISC_AUTO_SCALE_DIS_SHFT                                        0x0

#define HWIO_AOSS_CC_SPARE0_REG_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003000)
#define HWIO_AOSS_CC_SPARE0_REG_RMSK                                              0xffffffff
#define HWIO_AOSS_CC_SPARE0_REG_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPARE0_REG_ADDR, HWIO_AOSS_CC_SPARE0_REG_RMSK)
#define HWIO_AOSS_CC_SPARE0_REG_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPARE0_REG_ADDR, m)
#define HWIO_AOSS_CC_SPARE0_REG_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPARE0_REG_ADDR,v)
#define HWIO_AOSS_CC_SPARE0_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPARE0_REG_ADDR,m,v,HWIO_AOSS_CC_SPARE0_REG_IN)
#define HWIO_AOSS_CC_SPARE0_REG_SPARE0_BITS_BMSK                                  0xffffffff
#define HWIO_AOSS_CC_SPARE0_REG_SPARE0_BITS_SHFT                                         0x0

#define HWIO_AOSS_CC_SPARE1_REG_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003024)
#define HWIO_AOSS_CC_SPARE1_REG_RMSK                                              0xffffffff
#define HWIO_AOSS_CC_SPARE1_REG_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPARE1_REG_ADDR, HWIO_AOSS_CC_SPARE1_REG_RMSK)
#define HWIO_AOSS_CC_SPARE1_REG_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPARE1_REG_ADDR, m)
#define HWIO_AOSS_CC_SPARE1_REG_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPARE1_REG_ADDR,v)
#define HWIO_AOSS_CC_SPARE1_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPARE1_REG_ADDR,m,v,HWIO_AOSS_CC_SPARE1_REG_IN)
#define HWIO_AOSS_CC_SPARE1_REG_SPARE1_BITS_BMSK                                  0xffffffff
#define HWIO_AOSS_CC_SPARE1_REG_SPARE1_BITS_SHFT                                         0x0

#define HWIO_AOSS_CC_SPARE2_REG_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00003028)
#define HWIO_AOSS_CC_SPARE2_REG_RMSK                                              0xffffffff
#define HWIO_AOSS_CC_SPARE2_REG_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPARE2_REG_ADDR, HWIO_AOSS_CC_SPARE2_REG_RMSK)
#define HWIO_AOSS_CC_SPARE2_REG_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPARE2_REG_ADDR, m)
#define HWIO_AOSS_CC_SPARE2_REG_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPARE2_REG_ADDR,v)
#define HWIO_AOSS_CC_SPARE2_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPARE2_REG_ADDR,m,v,HWIO_AOSS_CC_SPARE2_REG_IN)
#define HWIO_AOSS_CC_SPARE2_REG_SPARE2_BITS_BMSK                                  0xffffffff
#define HWIO_AOSS_CC_SPARE2_REG_SPARE2_BITS_SHFT                                         0x0

#define HWIO_AOSS_CC_SPARE3_REG_ADDR                                              (AOSS_CC_AOSS_CC_REG_REG_BASE      + 0x00050038)
#define HWIO_AOSS_CC_SPARE3_REG_RMSK                                              0xffffffff
#define HWIO_AOSS_CC_SPARE3_REG_IN          \
        in_dword_masked(HWIO_AOSS_CC_SPARE3_REG_ADDR, HWIO_AOSS_CC_SPARE3_REG_RMSK)
#define HWIO_AOSS_CC_SPARE3_REG_INM(m)      \
        in_dword_masked(HWIO_AOSS_CC_SPARE3_REG_ADDR, m)
#define HWIO_AOSS_CC_SPARE3_REG_OUT(v)      \
        out_dword(HWIO_AOSS_CC_SPARE3_REG_ADDR,v)
#define HWIO_AOSS_CC_SPARE3_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_CC_SPARE3_REG_ADDR,m,v,HWIO_AOSS_CC_SPARE3_REG_IN)
#define HWIO_AOSS_CC_SPARE3_REG_SPARE2_BITS_BMSK                                  0xffffffff
#define HWIO_AOSS_CC_SPARE3_REG_SPARE2_BITS_SHFT                                         0x0


#endif /* __HWIO_H__ */
