#ifndef __HALCLKHWIOASM_H__
#define __HALCLKHWIOASM_H__
/*
===========================================================================
*/
/**
  @file HALclkHWIOASM.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM855 (Hana) [hana_v1.0_p1q0r10]
 
  This file contains HWIO register definitions for the following modules:
    SSC_SCC_SCC_SCC_REG
    SSC_QDSP6V66SS_PUB
    SSC_QDSP6V66SS_PRIVATE
    SSC_QDSP6SS_QDSP6SS_QTMR_AC
    SSC_QDSP6SS_QTMR_F0_0
    SSC_QDSP6SS_QTMR_F1_1
    SSC_QDSP6SS_QTMR_F2_2
    SSC_QDSP6SS_RSCC_RSCC_RSC
    SSC_MCC_REGS

  'Include' filters applied: 
  'Exclude' filters applied: 
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

  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/slpi/hw/sdm855/HALclkHWIOASM.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#define SSC_BASE_PHYS     0x05400000
#define SSC_BASE          SSC_BASE_PHYS

/*----------------------------------------------------------------------------
 * MODULE: SSC_SCC_SCC_SCC_REG
 *--------------------------------------------------------------------------*/

#define SSC_SCC_SCC_SCC_REG_REG_BASE                                               (SSC_BASE      + 0x00b10000)
#define SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS                                          0x00b10000

#define HWIO_SSC_SCC_PLL_MODE_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000000)
#define HWIO_SSC_SCC_PLL_MODE_OFFS                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000000)
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
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT7_BMSK                                          0x80
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT7_SHFT                                           0x7
#define HWIO_SSC_SCC_PLL_MODE_OUT_TEST_HW_CTL_BMSK                                       0x40
#define HWIO_SSC_SCC_PLL_MODE_OUT_TEST_HW_CTL_SHFT                                        0x6
#define HWIO_SSC_SCC_PLL_MODE_OUT_ODD_HW_CTL_BMSK                                        0x20
#define HWIO_SSC_SCC_PLL_MODE_OUT_ODD_HW_CTL_SHFT                                         0x5
#define HWIO_SSC_SCC_PLL_MODE_OUT_EVEN_HW_CTL_BMSK                                       0x10
#define HWIO_SSC_SCC_PLL_MODE_OUT_EVEN_HW_CTL_SHFT                                        0x4
#define HWIO_SSC_SCC_PLL_MODE_OUT_MAIN_HW_CTL_BMSK                                        0x8
#define HWIO_SSC_SCC_PLL_MODE_OUT_MAIN_HW_CTL_SHFT                                        0x3
#define HWIO_SSC_SCC_PLL_MODE_PLL_RESET_N_BMSK                                            0x4
#define HWIO_SSC_SCC_PLL_MODE_PLL_RESET_N_SHFT                                            0x2
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT1_BMSK                                           0x2
#define HWIO_SSC_SCC_PLL_MODE_RESERVE_BIT1_SHFT                                           0x1
#define HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_BMSK                                            0x1
#define HWIO_SSC_SCC_PLL_MODE_PLL_OUTCTRL_SHFT                                            0x0

#define HWIO_SSC_SCC_PLL_L_VAL_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000004)
#define HWIO_SSC_SCC_PLL_L_VAL_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000004)
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
#define HWIO_SSC_SCC_PLL_CAL_L_VAL_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000008)
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
#define HWIO_SSC_SCC_PLL_USER_CTL_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000000c)
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
#define HWIO_SSC_SCC_PLL_USER_CTL_U_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RMSK                                           0xffffffff
#define HWIO_SSC_SCC_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR, HWIO_SSC_SCC_PLL_USER_CTL_U_RMSK)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR, m)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR,v)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_USER_CTL_U_ADDR,m,v,HWIO_SSC_SCC_PLL_USER_CTL_U_IN)
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BITS63_51_BMSK                         0xfff80000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BITS63_51_SHFT                               0x13
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                          0x40000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                             0x12
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                          0x20000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                             0x11
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BIT48_BMSK                                0x10000
#define HWIO_SSC_SCC_PLL_USER_CTL_U_RESERVE_BIT48_SHFT                                   0x10
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

#define HWIO_SSC_SCC_PLL_USER_CTL_U1_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000014)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000014)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_RMSK                                          0xffffffff
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U1_ADDR, HWIO_SSC_SCC_PLL_USER_CTL_U1_RMSK)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_USER_CTL_U1_ADDR, m)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_USER_CTL_U1_ADDR,v)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_USER_CTL_U1_ADDR,m,v,HWIO_SSC_SCC_PLL_USER_CTL_U1_IN)
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_RESERVE_BITS95_64_BMSK                        0xffffffff
#define HWIO_SSC_SCC_PLL_USER_CTL_U1_RESERVE_BITS95_64_SHFT                               0x0

#define HWIO_SSC_SCC_PLL_CONFIG_CTL_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000018)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000018)
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

#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000001c)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000001c)
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
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT51_BMSK                              0x80000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT51_SHFT                                 0x13
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT50_BMSK                              0x40000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT50_SHFT                                 0x12
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                               0x30000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                                  0x10
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT47_BMSK                               0x8000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT47_SHFT                                  0xf
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT46_BMSK                               0x4000
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U_RESERVE_BIT46_SHFT                                  0xe
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

#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000020)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_RMSK                                        0xffffffff
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_ADDR, HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_RMSK)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_ADDR, m)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_ADDR,v)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_ADDR,m,v,HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_IN)
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_RESERVE_BITS95_64_BMSK                      0xffffffff
#define HWIO_SSC_SCC_PLL_CONFIG_CTL_U1_RESERVE_BITS95_64_SHFT                             0x0

#define HWIO_SSC_SCC_PLL_TEST_CTL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000024)
#define HWIO_SSC_SCC_PLL_TEST_CTL_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000024)
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

#define HWIO_SSC_SCC_PLL_TEST_CTL_U_ADDR                                           (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000028)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000028)
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

#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_ADDR                                          (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000002c)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_RMSK                                          0xffffffff
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_U1_ADDR, HWIO_SSC_SCC_PLL_TEST_CTL_U1_RMSK)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_TEST_CTL_U1_ADDR, m)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_TEST_CTL_U1_ADDR,v)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_TEST_CTL_U1_ADDR,m,v,HWIO_SSC_SCC_PLL_TEST_CTL_U1_IN)
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_RESERVE_BITS95_64_BMSK                        0xffffffff
#define HWIO_SSC_SCC_PLL_TEST_CTL_U1_RESERVE_BITS95_64_SHFT                               0x0

#define HWIO_SSC_SCC_PLL_STATUS_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000030)
#define HWIO_SSC_SCC_PLL_STATUS_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000030)
#define HWIO_SSC_SCC_PLL_STATUS_RMSK                                               0xffffffff
#define HWIO_SSC_SCC_PLL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATUS_ADDR, HWIO_SSC_SCC_PLL_STATUS_RMSK)
#define HWIO_SSC_SCC_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATUS_ADDR, m)
#define HWIO_SSC_SCC_PLL_STATUS_STATUS_31_0_BMSK                                   0xffffffff
#define HWIO_SSC_SCC_PLL_STATUS_STATUS_31_0_SHFT                                          0x0

#define HWIO_SSC_SCC_PLL_FREQ_CTL_ADDR                                             (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000034)
#define HWIO_SSC_SCC_PLL_FREQ_CTL_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000034)
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

#define HWIO_SSC_SCC_PLL_OPMODE_ADDR                                               (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000038)
#define HWIO_SSC_SCC_PLL_OPMODE_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000038)
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

#define HWIO_SSC_SCC_PLL_STATE_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000003c)
#define HWIO_SSC_SCC_PLL_STATE_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SSC_SCC_PLL_STATE_RMSK                                                       0x7
#define HWIO_SSC_SCC_PLL_STATE_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATE_ADDR, HWIO_SSC_SCC_PLL_STATE_RMSK)
#define HWIO_SSC_SCC_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_STATE_ADDR, m)
#define HWIO_SSC_SCC_PLL_STATE_PLL_STATE_BMSK                                             0x7
#define HWIO_SSC_SCC_PLL_STATE_PLL_STATE_SHFT                                             0x0

#define HWIO_SSC_SCC_PLL_ALPHA_VAL_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000040)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000040)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_RMSK                                                0xffff
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_ALPHA_VAL_ADDR, HWIO_SSC_SCC_PLL_ALPHA_VAL_RMSK)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_ALPHA_VAL_ADDR,m,v,HWIO_SSC_SCC_PLL_ALPHA_VAL_IN)
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_PLL_ALPHA_VAL_BMSK                                  0xffff
#define HWIO_SSC_SCC_PLL_ALPHA_VAL_PLL_ALPHA_VAL_SHFT                                     0x0

#define HWIO_SSC_SCC_PLL_SPARE_ADDR                                                (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000044)
#define HWIO_SSC_SCC_PLL_SPARE_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000044)
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

#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000048)
#define HWIO_SSC_SCC_PLL_SSC_DELTA_ALPHA_OFFS                                      (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000048)
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

#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_ADDR                                      (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000004c)
#define HWIO_SSC_SCC_PLL_SSC_UPDATE_RATE_OFFS                                      (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000004c)
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

#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000050)
#define HWIO_SSC_SCC_PLL_SSC_NUM_STEPS_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000050)
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
#define HWIO_SSC_SCC_MAIN_RCG_CMD_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00001000)
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
#define HWIO_SSC_SCC_MAIN_RCG_CFG_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00001004)
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
#define HWIO_SSC_SCC_QUPV3_SE0_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002000)
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
#define HWIO_SSC_SCC_DFS_SE0_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002018)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002020)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002024)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002028)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000202c)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002030)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002034)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002038)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000203c)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002060)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002064)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002068)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000206c)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002070)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002074)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002078)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000207c)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020a0)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020a4)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020a8)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020ac)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020b0)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020b4)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020b8)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020bc)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020e0)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020e4)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020e8)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020ec)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020f0)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020f4)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020f8)
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
#define HWIO_SSC_SCC_DFS_SE0_QUPV3_SE0_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000020fc)
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
#define HWIO_SSC_SCC_QUPV3_SE0_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002004)
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
#define HWIO_SSC_SCC_QUPV3_SE0_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002008)
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
#define HWIO_SSC_SCC_QUPV3_SE0_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000200c)
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
#define HWIO_SSC_SCC_QUPV3_SE0_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002010)
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
#define HWIO_SSC_SCC_QUPV3_SE0_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002014)
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
#define HWIO_SSC_SCC_QUPV3_SE0_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00002130)
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
#define HWIO_SSC_SCC_QUPV3_SE1_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003000)
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
#define HWIO_SSC_SCC_DFS_SE1_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003018)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003020)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003024)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003028)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000302c)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003030)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003034)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003038)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000303c)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003060)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003064)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003068)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000306c)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003070)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003074)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003078)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000307c)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030a0)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030a4)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030a8)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030ac)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030b0)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030b4)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030b8)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030bc)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030e0)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030e4)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030e8)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030ec)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030f0)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030f4)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030f8)
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
#define HWIO_SSC_SCC_DFS_SE1_QUPV3_SE1_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000030fc)
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
#define HWIO_SSC_SCC_QUPV3_SE1_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003004)
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
#define HWIO_SSC_SCC_QUPV3_SE1_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003008)
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
#define HWIO_SSC_SCC_QUPV3_SE1_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000300c)
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
#define HWIO_SSC_SCC_QUPV3_SE1_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003010)
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
#define HWIO_SSC_SCC_QUPV3_SE1_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003014)
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
#define HWIO_SSC_SCC_QUPV3_SE1_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00003130)
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
#define HWIO_SSC_SCC_QUPV3_SE2_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004000)
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
#define HWIO_SSC_SCC_DFS_SE2_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004018)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004020)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004024)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004028)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000402c)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004030)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004034)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004038)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000403c)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004060)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004064)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004068)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000406c)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004070)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004074)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004078)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000407c)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040a0)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040a4)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040a8)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040ac)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040b0)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040b4)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040b8)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040bc)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040e0)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040e4)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040e8)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040ec)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040f0)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040f4)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040f8)
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
#define HWIO_SSC_SCC_DFS_SE2_QUPV3_SE2_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x000040fc)
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
#define HWIO_SSC_SCC_QUPV3_SE2_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004004)
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
#define HWIO_SSC_SCC_QUPV3_SE2_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004008)
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
#define HWIO_SSC_SCC_QUPV3_SE2_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000400c)
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
#define HWIO_SSC_SCC_QUPV3_SE2_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004010)
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
#define HWIO_SSC_SCC_QUPV3_SE2_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004014)
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
#define HWIO_SSC_SCC_QUPV3_SE2_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00004130)
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
#define HWIO_SSC_SCC_QUPV3_SE3_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b000)
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
#define HWIO_SSC_SCC_DFS_SE3_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b018)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b020)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b024)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b028)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b02c)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b030)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b034)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b038)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b03c)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b060)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b064)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b068)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b06c)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b070)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b074)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b078)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b07c)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0a0)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0a4)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0a8)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0ac)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0b0)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0b4)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0b8)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0bc)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0e0)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0e4)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0e8)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0ec)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0f0)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0f4)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0f8)
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
#define HWIO_SSC_SCC_DFS_SE3_QUPV3_SE3_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b0fc)
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
#define HWIO_SSC_SCC_QUPV3_SE3_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b004)
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
#define HWIO_SSC_SCC_QUPV3_SE3_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b008)
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
#define HWIO_SSC_SCC_QUPV3_SE3_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b00c)
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
#define HWIO_SSC_SCC_QUPV3_SE3_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b010)
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
#define HWIO_SSC_SCC_QUPV3_SE3_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b014)
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
#define HWIO_SSC_SCC_QUPV3_SE3_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000b130)
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
#define HWIO_SSC_SCC_QUPV3_SE4_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c000)
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
#define HWIO_SSC_SCC_DFS_SE4_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c018)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c020)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c024)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c028)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c02c)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c030)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c034)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c038)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c03c)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c060)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c064)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c068)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c06c)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c070)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c074)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c078)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c07c)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0a0)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0a4)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0a8)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0ac)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0b0)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0b4)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0b8)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0bc)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0e0)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0e4)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0e8)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0ec)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0f0)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0f4)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0f8)
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
#define HWIO_SSC_SCC_DFS_SE4_QUPV3_SE4_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c0fc)
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
#define HWIO_SSC_SCC_QUPV3_SE4_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c004)
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
#define HWIO_SSC_SCC_QUPV3_SE4_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c008)
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
#define HWIO_SSC_SCC_QUPV3_SE4_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c00c)
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
#define HWIO_SSC_SCC_QUPV3_SE4_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c010)
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
#define HWIO_SSC_SCC_QUPV3_SE4_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c014)
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
#define HWIO_SSC_SCC_QUPV3_SE4_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000c130)
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
#define HWIO_SSC_SCC_QUPV3_SE5_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d000)
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
#define HWIO_SSC_SCC_DFS_SE5_CMD_DFSR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d018)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d020)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d024)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d028)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d02c)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d030)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d034)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d038)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_DFSR_OFFS                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d03c)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d060)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d064)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d068)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d06c)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d070)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d074)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d078)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_M_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d07c)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0a0)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0a4)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0a8)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0ac)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0b0)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0b4)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0b8)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_N_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0bc)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF0_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0e0)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF1_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0e4)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF2_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0e8)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF3_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0ec)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF4_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0f0)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF5_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0f4)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF6_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0f8)
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
#define HWIO_SSC_SCC_DFS_SE5_QUPV3_SE5_PERF7_D_DFSR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d0fc)
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
#define HWIO_SSC_SCC_QUPV3_SE5_CMD_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d004)
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
#define HWIO_SSC_SCC_QUPV3_SE5_CFG_RCGR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d008)
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
#define HWIO_SSC_SCC_QUPV3_SE5_M_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d00c)
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
#define HWIO_SSC_SCC_QUPV3_SE5_N_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d010)
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
#define HWIO_SSC_SCC_QUPV3_SE5_D_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d014)
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
#define HWIO_SSC_SCC_QUPV3_SE5_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000d130)
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
#define HWIO_SSC_SCC_Q6_SPM_BCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00008000)
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
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_Q6_SPM_DIV_CDIVR_OFFS                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00008004)
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
#define HWIO_SSC_SCC_Q6_SPM_CBCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00008008)
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
#define HWIO_SSC_SCC_CCD_CORE_AND_BUS_BCR_OFFS                                     (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000e000)
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
#define HWIO_SSC_SCC_CDIV_CCD_DIV_CDIVR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000e004)
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
#define HWIO_SSC_SCC_CCD_CBCR_OFFS                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000e008)
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
#define HWIO_SSC_SCC_CCD_HCLK_CBCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000e00c)
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
#define HWIO_SSC_SCC_CCD_HCLK_UNGATED_TO_ODSC_CBCR_OFFS                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000e010)
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
#define HWIO_SSC_SCC_QUPV3_CORE_BCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00005000)
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
#define HWIO_SSC_SCC_CDIV_QUPV3_2XCORE_DIV_CDIVR_OFFS                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00005004)
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
#define HWIO_SSC_SCC_QUPV3_2XCORE_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00005008)
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
#define HWIO_SSC_SCC_CDIV_QUPV3_CORE_DIV_CDIVR_OFFS                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000500c)
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
#define HWIO_SSC_SCC_QUPV3_CORE_CBCR_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00005010)
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
#define HWIO_SSC_SCC_SLEEP_BCR_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00010000)
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
#define HWIO_SSC_SCC_SLEEP_CBCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00010004)
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
#define HWIO_SSC_SCC_AOSS_CC_RO_BCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00010008)
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
#define HWIO_SSC_SCC_RO_CBCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0001000c)
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
#define HWIO_SSC_SCC_SDC_BCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f000)
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
#define HWIO_SSC_SCC_CDIV_SDC_DIV_CDIVR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f004)
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
#define HWIO_SSC_SCC_SDC_PROC_FCLK_CBCR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f008)
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
#define HWIO_SSC_SCC_SDC_PROC_HCLK_CBCR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f00c)
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
#define HWIO_SSC_SCC_SDC_BUS_HCLK_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000f010)
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
#define HWIO_SSC_SCC_BUS_BCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009000)
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
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_BUS_DIV_CDIVR_OFFS                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009004)
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
#define HWIO_SSC_SCC_CRIF_CBCR_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009008)
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
#define HWIO_SSC_SCC_DATA_H_CBCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000900c)
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
#define HWIO_SSC_SCC_CSR_H_CBCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009010)
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
#define HWIO_SSC_SCC_BUS_ALT_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009014)
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
#define HWIO_SSC_SCC_Q6_AHB_BCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009018)
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
#define HWIO_SSC_SCC_Q6_AHBM_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000901c)
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
#define HWIO_SSC_SCC_Q6_AHBS_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009020)
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
#define HWIO_SSC_SCC_SMEM_BCR_OFFS                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009024)
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
#define HWIO_SSC_SCC_SMEM_CBCR_OFFS                                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009028)
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
#define HWIO_SSC_SCC_AHB_TIMEOUT_BCR_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000902c)
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
#define HWIO_SSC_SCC_AHB_TIMEOUT_CBCR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009030)
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
#define HWIO_SSC_SCC_Q6_XPU2_CLIENT_CBCR_OFFS                                      (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009034)
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
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_BCR_OFFS                                       (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009038)
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
#define HWIO_SSC_SCC_Q6_XPU2_CONFIG_CBCR_OFFS                                      (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000903c)
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
#define HWIO_SSC_SCC_SDC_XPU2_CLIENT_CBCR_OFFS                                     (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009040)
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
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_BCR_OFFS                                      (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009044)
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
#define HWIO_SSC_SCC_SDC_XPU2_CONFIG_CBCR_OFFS                                     (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009048)
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
#define HWIO_SSC_SCC_VS_CX_AHB_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000904c)
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
#define HWIO_SSC_SCC_VS_CX_AHB_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009050)
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
#define HWIO_SSC_SCC_VS_MX_AHB_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009054)
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
#define HWIO_SSC_SCC_VS_MX_AHB_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009058)
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
#define HWIO_SSC_SCC_QTIMER_BCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000905c)
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
#define HWIO_SSC_SCC_QTIMER_TMR_CDIV_DIV_CDIVR_OFFS                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009060)
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
#define HWIO_SSC_SCC_QTIMER_TMR_CBCR_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009064)
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
#define HWIO_SSC_SCC_QUPV3_BUS_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009068)
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
#define HWIO_SSC_SCC_QUPV3_S_HCLK_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000906c)
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
#define HWIO_SSC_SCC_QUPV3_M_HCLK_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009070)
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
#define HWIO_SSC_SCC_RSC_HCLK_BCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009074)
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
#define HWIO_SSC_SCC_RSC_HCLK_CBCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009078)
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
#define HWIO_SSC_SCC_QTIMER_AHB_BCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000907c)
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
#define HWIO_SSC_SCC_QTIMER_AHB_CBCR_OFFS                                          (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009080)
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
#define HWIO_SSC_SCC_NOC_BUS_BCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009084)
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
#define HWIO_SSC_SCC_NOC_BUS_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00009088)
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
#define HWIO_SSC_SCC_DBG_BCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a000)
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
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_DIV_CDIVR_OFFS                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a004)
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
#define HWIO_SSC_SCC_AT_CBCR_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a008)
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
#define HWIO_SSC_SCC_Q6_ATBM_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a00c)
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
#define HWIO_SSC_SCC_PCLKDBG_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a010)
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
#define HWIO_SSC_SCC_VS_BCR_OFFS                                                   (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a014)
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
#define HWIO_SSC_SCC_VS_VDDMX_CMD_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a018)
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
#define HWIO_SSC_SCC_VS_VDDMX_CFG_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a01c)
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
#define HWIO_SSC_SCC_VS_VDDCX_CMD_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a030)
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
#define HWIO_SSC_SCC_VS_VDDCX_CFG_RCGR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a034)
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
#define HWIO_SSC_SCC_VS_VDDMX_CBCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a048)
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
#define HWIO_SSC_SCC_VS_VDDCX_CBCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a04c)
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
#define HWIO_SSC_SCC_DBG_TSCTR_BCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a050)
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
#define HWIO_SSC_SCC_MAIN_RCG_CDIV_DBG_TSCTR_DIV_CDIVR_OFFS                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a054)
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
#define HWIO_SSC_SCC_DBG_TSCTR_CBCR_OFFS                                           (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a058)
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
#define HWIO_SSC_SCC_SDC_DBG_BCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a05c)
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
#define HWIO_SSC_SCC_SDC_DAP_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a060)
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
#define HWIO_SSC_SCC_SDC_CTI_PCLK_CBCR_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a064)
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
#define HWIO_SSC_SCC_SDC_CTI_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a068)
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
#define HWIO_SSC_SCC_SDC_QDSS_CBCR_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a06c)
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
#define HWIO_SSC_SCC_SDC_TS_CBCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a070)
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
#define HWIO_SSC_SCC_NOC_ATB_BCR_OFFS                                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a074)
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
#define HWIO_SSC_SCC_NOC_ATB_CBCR_OFFS                                             (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a078)
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
#define HWIO_SSC_SCC_PLL_OUT_MAIN_CDIV_DIV_CDIVR_OFFS                              (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000080)
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

#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_ADDR                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a090)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_OFFS                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a090)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_RMSK                                   0xf
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_IN          \
        in_dword_masked(HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_ADDR, HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_RMSK)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_INM(m)      \
        in_dword_masked(HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_ADDR, m)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_OUT(v)      \
        out_dword(HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_ADDR,v)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_ADDR,m,v,HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_IN)
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_CLK_DIV_BMSK                           0xf
#define HWIO_SSC_SCC_PLL_LV_TEST_POSTDIV_DIV_CDIVR_CLK_DIV_SHFT                           0x0

#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_ADDR                                         (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000a080)
#define HWIO_SSC_SCC_PLL_LV_TEST_CBCR_OFFS                                         (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a080)
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
#define HWIO_SSC_SCC_DEBUG_CBCR_OFFS                                               (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000a084)
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

#define HWIO_SSC_SCC_Q6_MISC_ADDR                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000064)
#define HWIO_SSC_SCC_Q6_MISC_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000064)
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

#define HWIO_SSC_SCC_CBC_MISC_ADDR                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000068)
#define HWIO_SSC_SCC_CBC_MISC_OFFS                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000068)
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

#define HWIO_SSC_SCC_DEBUG_CLK_CTL_ADDR                                            (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x0000006c)
#define HWIO_SSC_SCC_DEBUG_CLK_CTL_OFFS                                            (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0000006c)
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

#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_ADDR                                        (SSC_SCC_SCC_SCC_REG_REG_BASE      + 0x00000070)
#define HWIO_SSC_SCC_AON_CTRL_OVRD_REG_OFFS                                        (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000070)
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
#define HWIO_SSC_SCC_SCC_MISC_OFFS                                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00000060)
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
#define HWIO_SSC_SCC_SPARE_1_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0001f000)
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
#define HWIO_SSC_SCC_SPARE_2_OFFS                                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x0001e000)
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
#define HWIO_SSC_SCC_TZ_CLOCK_BRANCH_EN_VOTE_OFFS                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00020000)
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
#define HWIO_SSC_SCC_HLOS_CLOCK_BRANCH_EN_VOTE_OFFS                                (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00021000)
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
#define HWIO_SSC_SCC_SDC_CLOCK_BRANCH_EN_VOTE_OFFS                                 (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00022000)
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
#define HWIO_SSC_SCC_Q6_CLOCK_BRANCH_EN_VOTE_OFFS                                  (SSC_SCC_SCC_SCC_REG_REG_BASE_OFFS + 0x00023000)
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

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6V66SS_PUB
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6V66SS_PUB_REG_BASE                                                 (SSC_BASE      + 0x00400000)
#define SSC_QDSP6V66SS_PUB_REG_BASE_OFFS                                            0x00400000

#define HWIO_SSC_QDSP6SS_VERSION_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_VERSION_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_QDSP6SS_VERSION_RMSK                                               0xffffffff
#define HWIO_SSC_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_VERSION_ADDR, HWIO_SSC_QDSP6SS_VERSION_RMSK)
#define HWIO_SSC_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_VERSION_ADDR, m)
#define HWIO_SSC_QDSP6SS_VERSION_MAJOR_BMSK                                         0xf0000000
#define HWIO_SSC_QDSP6SS_VERSION_MAJOR_SHFT                                               0x1c
#define HWIO_SSC_QDSP6SS_VERSION_MINOR_BMSK                                          0xfff0000
#define HWIO_SSC_QDSP6SS_VERSION_MINOR_SHFT                                               0x10
#define HWIO_SSC_QDSP6SS_VERSION_STEP_BMSK                                              0xffff
#define HWIO_SSC_QDSP6SS_VERSION_STEP_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_RST_EVB_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000010)
#define HWIO_SSC_QDSP6SS_RST_EVB_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_QDSP6SS_RST_EVB_RMSK                                                0xffffff0
#define HWIO_SSC_QDSP6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RST_EVB_ADDR, HWIO_SSC_QDSP6SS_RST_EVB_RMSK)
#define HWIO_SSC_QDSP6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RST_EVB_ADDR, m)
#define HWIO_SSC_QDSP6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RST_EVB_ADDR,v)
#define HWIO_SSC_QDSP6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RST_EVB_ADDR,m,v,HWIO_SSC_QDSP6SS_RST_EVB_IN)
#define HWIO_SSC_QDSP6SS_RST_EVB_EVB_BMSK                                            0xffffff0
#define HWIO_SSC_QDSP6SS_RST_EVB_EVB_SHFT                                                  0x4

#define HWIO_SSC_QDSP6SS_DBG_CFG_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000018)
#define HWIO_SSC_QDSP6SS_DBG_CFG_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000018)
#define HWIO_SSC_QDSP6SS_DBG_CFG_RMSK                                               0xffff0002
#define HWIO_SSC_QDSP6SS_DBG_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR, HWIO_SSC_QDSP6SS_DBG_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DBG_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_DBG_CFG_IN)
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SW_REG_BMSK                                    0xff000000
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SW_REG_SHFT                                          0x18
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SPARE_BMSK                                       0xff0000
#define HWIO_SSC_QDSP6SS_DBG_CFG_DBG_SPARE_SHFT                                           0x10
#define HWIO_SSC_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_SHFT                                         0x1

#define HWIO_SSC_QDSP6SS_RET_CFG_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QDSP6SS_RET_CFG_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SSC_QDSP6SS_RET_CFG_RMSK                                                      0x1
#define HWIO_SSC_QDSP6SS_RET_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RET_CFG_ADDR, HWIO_SSC_QDSP6SS_RET_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_RET_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RET_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_RET_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RET_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_RET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RET_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_RET_CFG_IN)
#define HWIO_SSC_QDSP6SS_RET_CFG_RET_ARES_ENA_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_RET_CFG_RET_ARES_ENA_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000020)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_RMSK                                             0x80000001
#define HWIO_SSC_QDSP6SS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR, HWIO_SSC_QDSP6SS_CORE_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CBCR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CORE_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000028)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000028)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_RMSK                                         0x80000013
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR, HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_BMSK                                0x80000000
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                      0x1f
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                0x10
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                 0x4
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_CORE_CMD_RCGR_UPDATE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RMSK                                           0x11071f
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR, HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_IN)
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_BMSK                            0x100000
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_SHFT                                0x14
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_BMSK                            0x10000
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_SHFT                               0x10
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK                                      0x700
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT                                        0x8
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK                                       0x1f
#define HWIO_SSC_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000048)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000048)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_RMSK                                               0x1
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR, HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_RMSK)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_ADDR,m,v,HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_IN)
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_BMSK                                  0x1
#define HWIO_SSC_QDSP6SS_CLOCK_SPDM_MON_Q6_MON_CLKEN_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_XO_CBCR_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000038)
#define HWIO_SSC_QDSP6SS_XO_CBCR_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000038)
#define HWIO_SSC_QDSP6SS_XO_CBCR_RMSK                                               0x80000001
#define HWIO_SSC_QDSP6SS_XO_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR, HWIO_SSC_QDSP6SS_XO_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_XO_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_XO_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_XO_CBCR_IN)
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKOFF_BMSK                                        0x80000000
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKOFF_SHFT                                              0x1f
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKEN_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_XO_CBCR_CLKEN_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_RMSK                                            0x80000001
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR, HWIO_SSC_QDSP6SS_SLEEP_CBCR_RMSK)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR, m)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR,v)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_SLEEP_CBCR_ADDR,m,v,HWIO_SSC_QDSP6SS_SLEEP_CBCR_IN)
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKOFF_BMSK                                     0x80000000
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKOFF_SHFT                                           0x1f
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_SLEEP_CBCR_CLKEN_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_NMI_ADDR                                                   (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_SSC_QDSP6SS_NMI_OFFS                                                   (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000040)
#define HWIO_SSC_QDSP6SS_NMI_RMSK                                                          0x3
#define HWIO_SSC_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_NMI_ADDR,v)
#define HWIO_SSC_QDSP6SS_NMI_CLEAR_STATUS_BMSK                                             0x2
#define HWIO_SSC_QDSP6SS_NMI_CLEAR_STATUS_SHFT                                             0x1
#define HWIO_SSC_QDSP6SS_NMI_SET_NMI_BMSK                                                  0x1
#define HWIO_SSC_QDSP6SS_NMI_SET_NMI_SHFT                                                  0x0

#define HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000044)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_RMSK                                                   0x7
#define HWIO_SSC_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR, HWIO_SSC_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000088)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000088)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_RMSK                                                 0x7
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR, HWIO_SSC_QDSP6SS_INTF_HALTREQ_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_INTF_HALTREQ_ADDR,m,v,HWIO_SSC_QDSP6SS_INTF_HALTREQ_IN)
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM2_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM2_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM_BMSK                                            0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_AXIM_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ALL_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTREQ_ALL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000008c)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000008c)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_RMSK                                                 0x7
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR, HWIO_SSC_QDSP6SS_INTF_HALTACK_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_HALTACK_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM2_BMSK                                           0x4
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM2_SHFT                                           0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM_BMSK                                            0x2
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_AXIM_SHFT                                            0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ALL_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_INTF_HALTACK_ALL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000090)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000090)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_RMSK                                                0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTFCLAMP_SET_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_BMSK                                  0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_SHFT                                  0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_BMSK                                  0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_SHFT                                  0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_BMSK                                   0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_SHFT                                   0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000094)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000094)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_RMSK                                              0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_BMSK                                0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_SHFT                                0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_BMSK                                0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_SHFT                                0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_BMSK                                 0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_SHFT                                 0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR                                      (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000098)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_OFFS                                      (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000098)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_RMSK                                             0xf
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR, HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_BMSK                               0x8
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_SHFT                               0x3
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_BMSK                               0x4
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_BMSK                                0x1
#define HWIO_SSC_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000009c)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000009c)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_RMSK                                              0x3
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR, HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_RMSK)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR, m)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR,v)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_ADDR,m,v,HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_IN)
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM2_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM2_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_INTF_FIFO_RESET_AXIM_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x000000a0)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x000000a0)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RMSK                                                 0xf
#define HWIO_SSC_QDSP6SS_STATERET_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR, HWIO_SSC_QDSP6SS_STATERET_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STATERET_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_STATERET_CTL_IN)
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKEUP_IN_BMSK                                       0x8
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKEUP_IN_SHFT                                       0x3
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKE_IRQ_BMSK                                        0x4
#define HWIO_SSC_QDSP6SS_STATERET_CTL_WAKE_IRQ_SHFT                                        0x2
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RESTORE_BMSK                                         0x2
#define HWIO_SSC_QDSP6SS_STATERET_CTL_RESTORE_SHFT                                         0x1
#define HWIO_SSC_QDSP6SS_STATERET_CTL_SAVE_BMSK                                            0x1
#define HWIO_SSC_QDSP6SS_STATERET_CTL_SAVE_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x000000b0)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x000000b0)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_RMSK                                              0xffff
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR, HWIO_SSC_QDSP6SS_CGC_OVERRIDE_RMSK)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR, m)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR,v)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CGC_OVERRIDE_ADDR,m,v,HWIO_SSC_QDSP6SS_CGC_OVERRIDE_IN)
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_BMSK                                   0x8000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_SHFT                                      0xf
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_BMSK                                  0x4000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_SHFT                                     0xe
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_BMSK                                  0x2000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_SHFT                                     0xd
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_BMSK                                  0x1000
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_SHFT                                     0xc
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_BMSK                                   0x800
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_SHFT                                     0xb
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_BMSK                                   0x400
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_SHFT                                     0xa
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_BMSK                              0x300
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_SHFT                                0x8
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_BMSK                                     0x80
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_SHFT                                      0x7
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_BMSK                              0x60
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_SHFT                               0x5
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_BMSK                               0x10
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_SHFT                                0x4
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_BMSK                                 0x8
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_SHFT                                 0x3
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_BMSK                                     0x4
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_SHFT                                     0x2
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x000000c0)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x000000c0)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_RMSK                                                 0xf
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_BHS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_BHS_CTL_IN)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_BMSK                                       0xf
#define HWIO_SSC_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x000000d0)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x000000d0)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_RMSK                                                 0x3
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR, HWIO_SSC_QDSP6SS_CPR_OVERRIDE_RMSK)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPR_OVERRIDE_ADDR,m,v,HWIO_SSC_QDSP6SS_CPR_OVERRIDE_IN)
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000100)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000100)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_RMSK                                             0xfffc0000
#define HWIO_SSC_QDSP6SS_STRAP_TCM_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR, HWIO_SSC_QDSP6SS_STRAP_TCM_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_TCM_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_TCM_IN)
#define HWIO_SSC_QDSP6SS_STRAP_TCM_BASE_ADDR_BMSK                                   0xfffc0000
#define HWIO_SSC_QDSP6SS_STRAP_TCM_BASE_ADDR_SHFT                                         0x12

#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000104)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000104)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_RMSK                                        0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_BMSK                             0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_SHFT                                    0xe

#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000108)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000108)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_RMSK                                        0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_BMSK                             0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_SHFT                                    0xe

#define HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR                                              (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000118)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_OFFS                                              (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000118)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_RMSK                                              0xffffffff
#define HWIO_SSC_QDSP6SS_DCC_CTRL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR, HWIO_SSC_QDSP6SS_DCC_CTRL_RMSK)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR, m)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR,v)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DCC_CTRL_ADDR,m,v,HWIO_SSC_QDSP6SS_DCC_CTRL_IN)
#define HWIO_SSC_QDSP6SS_DCC_CTRL_CTRL_BMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_DCC_CTRL_CTRL_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR                                      (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000011c)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_OFFS                                      (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000011c)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_RMSK                                      0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_BMSK                           0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_SHFT                                  0xe

#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR                                      (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000120)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_OFFS                                      (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000120)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_RMSK                                      0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR, HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_IN)
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_BMSK                           0xffffc000
#define HWIO_SSC_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_SHFT                                  0xe

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR                                     (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000124)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_OFFS                                     (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000124)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_RMSK                                            0x1
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_STRAP_QMC_ACC_SEL_BMSK                          0x1
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_SEL_STRAP_QMC_ACC_SEL_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000128)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000128)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_RMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_BMSK                           0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_STRAP_QMC_ACC_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000012c)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000012c)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_RMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR, HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_RMSK)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_ADDR,m,v,HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_IN)
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_STRAP_QMC_ACC_U_BMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_STRAP_QMC_ACC_U_STRAP_QMC_ACC_U_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000130)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000130)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_RMSK                                                  0x3
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR, HWIO_SSC_QDSP6SS_QMC_SVS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QMC_SVS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_QMC_SVS_CTL_IN)
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_BMSK                                      0x1
#define HWIO_SSC_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR                                  (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000140)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_OFFS                                  (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000140)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_RMSK                                         0xf
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR, HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_IN)
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_DIV_BMSK                                     0xf
#define HWIO_SSC_QDSP6SS_CORE_MEM_STAGGER_CTL_DIV_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_PLL_MODE_ADDR                                              (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000200)
#define HWIO_SSC_QDSP6SS_PLL_MODE_OFFS                                              (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000200)
#define HWIO_SSC_QDSP6SS_PLL_MODE_RMSK                                              0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_MODE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR, HWIO_SSC_QDSP6SS_PLL_MODE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_MODE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_MODE_IN)
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK                                 0x80000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_SHFT                                       0x1f
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                              0x40000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                    0x1e
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_BMSK                                0x20000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_SHFT                                      0x1d
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_FINE_BMSK                            0x10000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_DET_FINE_SHFT                                  0x1c
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS27_25_BMSK                             0xe000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BITS27_25_SHFT                                  0x19
#define HWIO_SSC_QDSP6SS_PLL_MODE_FSM_LEGACY_MODE_BMSK                               0x1000000
#define HWIO_SSC_QDSP6SS_PLL_MODE_FSM_LEGACY_MODE_SHFT                                    0x18
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                     0x800000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                         0x17
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_UPDATE_BMSK                                     0x400000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_UPDATE_SHFT                                         0x16
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                             0x200000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                                 0x15
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                               0x100000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                   0x14
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_BMSK                                  0xfc000
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_SHFT                                      0xe
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_BMSK                                   0x3f00
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_SHFT                                      0x8
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT7_BMSK                                       0x80
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT7_SHFT                                        0x7
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_TEST_HW_CTL_BMSK                                    0x40
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_TEST_HW_CTL_SHFT                                     0x6
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_ODD_HW_CTL_BMSK                                     0x20
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_ODD_HW_CTL_SHFT                                      0x5
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_EVEN_HW_CTL_BMSK                                    0x10
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_EVEN_HW_CTL_SHFT                                     0x4
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_MAIN_HW_CTL_BMSK                                     0x8
#define HWIO_SSC_QDSP6SS_PLL_MODE_OUT_MAIN_HW_CTL_SHFT                                     0x3
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK                                         0x4
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_RESET_N_SHFT                                         0x2
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT1_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_PLL_MODE_RESERVE_BIT1_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK                                         0x1
#define HWIO_SSC_QDSP6SS_PLL_MODE_PLL_OUTCTRL_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000204)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000204)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_RMSK                                                 0xffff
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_L_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_L_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_L_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_PLL_L_BMSK                                           0xffff
#define HWIO_SSC_QDSP6SS_PLL_L_VAL_PLL_L_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000208)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000208)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_RMSK                                             0xffff
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_PLL_CAL_L_BMSK                                   0xffff
#define HWIO_SSC_QDSP6SS_PLL_CAL_L_VAL_PLL_CAL_L_SHFT                                      0x0

#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000020c)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000020c)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_USER_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_USER_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_USER_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_19_BMSK                        0xfff80000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_19_SHFT                              0x13
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                               0x70000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                  0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_ODD_BMSK                           0xf000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_ODD_SHFT                              0xc
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_EVEN_BMSK                           0xf00
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_EVEN_SHFT                             0x8
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT_CLK_POLARITY_BMSK                               0x80
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_OUT_CLK_POLARITY_SHFT                                0x7
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                                0x60
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                                 0x5
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_TEST_BMSK                                    0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_TEST_SHFT                                     0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BIT3_BMSK                                    0x8
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_RESERVE_BIT3_SHFT                                    0x3
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_ODD_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_ODD_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_EVEN_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_EVEN_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_BMSK                                     0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_PLLOUT_MAIN_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000210)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000210)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS63_51_BMSK                      0xfff80000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS63_51_SHFT                            0x13
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_BMSK                       0x40000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_PREDIV_WHEN_CAL_SHFT                          0x12
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_BMSK                       0x20000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_EXT_SAVERESTORE_SHFT                          0x11
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BIT48_BMSK                             0x10000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_RESERVE_BIT48_SHFT                                0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_FRAC_FORMAT_SEL_BMSK                            0x8000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_FRAC_FORMAT_SEL_SHFT                               0xf
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_BMSK                         0x4000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_BIAS_ON_IN_STANDBY_SHFT                            0xe
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_REF_CLK_AT_OUT_BMSK                             0x2000
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_REF_CLK_AT_OUT_SHFT                                0xd
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_BMSK                      0x1800
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_SCALE_FREQ_ON_RESTART_SHFT                         0xb
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                      0x400
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                        0xa
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                             0x3e0
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                               0x5
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_EN_BMSK                               0x10
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_EN_SHFT                                0x4
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_CALIB_CTRL_BMSK                                    0xe
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_CALIB_CTRL_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_BMSK                        0x1
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U_USE_ONLY_FINE_LOCK_DET_SHFT                        0x0

#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000214)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000214)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_RMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_ADDR, HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_IN)
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_RESERVE_BITS95_64_BMSK                     0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_USER_CTL_U1_RESERVE_BITS95_64_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000218)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000218)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_BMSK                     0xfc000000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LDC_THRESHOLD_SHFT                           0x1a
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_BMSK                    0x3c00000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LDC_THRESHOLD_SHFT                         0x16
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_BMSK            0x3c0000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_NEG_THRESHOLD_SHFT                0x12
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_BMSK             0x3c000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_COARSE_LOCK_DET_POS_THRESHOLD_SHFT                 0xe
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_BMSK                    0x3800
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_THRESHOLD_SHFT                       0xb
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_BMSK                   0x700
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FINE_LOCK_DET_SAMPLE_SIZE_SHFT                     0x8
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_BMSK                         0xf0
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_SLEWING_KFN_SHFT                          0x4
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_KFN_BMSK                                  0xf
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_FWD_GAIN_KFN_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR                                      (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000021c)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OFFS                                      (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000021c)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIST_CFG_BMSK                             0xfff00000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIST_CFG_SHFT                                   0x14
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT51_BMSK                           0x80000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT51_SHFT                              0x13
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT50_BMSK                           0x40000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT50_SHFT                              0x12
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_FORCE_DAC_ON_BMSK                            0x30000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_FORCE_DAC_ON_SHFT                               0x10
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT47_BMSK                            0x8000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT47_SHFT                               0xf
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT46_BMSK                            0x4000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_RESERVE_BIT46_SHFT                               0xe
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_BMSK                        0x3000
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_OSC_SETTLING_TIME_SHFT                           0xc
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_BMSK                       0xc00
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_NUM_STEPS_FAST_LOCK_SHFT                         0xa
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_BMSK                           0x380
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_PREDIV_WHEN_CAL_SHFT                             0x7
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_BMSK                0x40
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_INC_MIN_GLITCH_THRESHOLD_4X_SHFT                 0x6
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_BMSK                       0x30
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_MIN_GLITCH_THRESHOLD_SHFT                        0x4
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_BMSK                       0x8
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_USE_BOTH_REF_CLK_EDGE_SHFT                       0x3
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_BMSK                            0x6
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_BIAS_WARMUP_TIME_SHFT                            0x1
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_BMSK                     0x1
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U_DIV_LOCK_DET_THRESHOLDS_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_ADDR                                     (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000220)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_OFFS                                     (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000220)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_RMSK                                     0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_ADDR, HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_IN)
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_RESERVE_BITS95_64_BMSK                   0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_CONFIG_CTL_U1_RESERVE_BITS95_64_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000224)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000224)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_TEST_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_TEST_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_BMSK                  0xc0000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ADD_REF_CYC_CALIB_STEPS_SHFT                        0x1e
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_BMSK              0x30000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_DETECTOR_COUNT_LIMIT_SHFT                    0x1c
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_BMSK                     0x8000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_GLITCH_PREVENTION_DIS_SHFT                          0x1b
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_FINE_FCW_BMSK                                  0x7e00000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_FINE_FCW_SHFT                                       0x15
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                          0x100000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                              0x14
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_BMSK                                  0xff000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_SHFT                                      0xc
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_BMSK                           0x800
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_FCW_SHFT                             0xb
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_PROCESS_CALIB_WORD_BMSK                            0x700
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_PROCESS_CALIB_WORD_SHFT                              0x8
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_BMSK                            0x80
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_OVERRIDE_CALIB_WORD_SHFT                             0x7
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                   0x40
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                    0x6
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_VREF_TEST_BMSK                                   0x20
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_VREF_TEST_SHFT                                    0x5
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_VDCO_TEST_BMSK                                   0x10
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_VDCO_TEST_SHFT                                    0x4
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_BMSK                             0x8
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_BYPASS_ATEST_BUFFER_SHFT                             0x3
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_IVCO_TEST_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_EN_IVCO_TEST_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_BMSK                                       0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_SHFT                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000228)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000228)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR, HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_IN)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_BMSK                    0x80000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_ENABLE_TRAINING_SEQ_SHFT                          0x1f
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_BMSK                       0x40000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_LOGIC_DEP_SHFT                             0x1e
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_BIAS_DET_BMSK                        0x20000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_BIAS_DET_SHFT                              0x1d
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_BMSK                       0x10000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MSB_CLOCK_SELECT_SHFT                             0x1c
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_BMSK                  0x8000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_FIRST_ORDER_DSM_SHFT                       0x1b
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_BMSK               0x6000000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DCO_OFFSET_CURRENT_ADJUST_SHFT                    0x19
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_BMSK                    0x1c00000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_MIRROR_DEVICE_ADJUST_SHFT                         0x16
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_BMSK                     0x200000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_STARTUP_CIR_DIS_SHFT                         0x15
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DISABLE_CLAMP_BMSK                            0x100000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DISABLE_CLAMP_SHFT                                0x14
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_BMSK                       0x80000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_OF_BIAS_SHFT                          0x13
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_BMSK                   0x40000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BYPASS_MODE_FOR_BIAS_EN_SHFT                      0x12
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_ADJUST_BMSK                               0x30000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_BIAS_ADJUST_SHFT                                  0x10
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIV2_NMO_EN_BMSK                                0x8000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIV2_NMO_EN_SHFT                                   0xf
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIS_AMP_STARTUP_BMSK                            0x4000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DIS_AMP_STARTUP_SHFT                               0xe
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SINGLE_DMET_EN_BMSK                             0x2000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SINGLE_DMET_EN_SHFT                                0xd
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_BMSK                           0x1000
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_DEMET_WINDOW_DIS_SHFT                              0xc
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                                 0xc00
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                   0xa
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                   0x380
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                     0x7
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_EN_BMSK                                 0x40
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_EN_SHFT                                  0x6
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                 0x20
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                  0x5
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                             0x18
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                              0x3
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_STATUS_REG_EN_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_STATUS_REG_EN_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FINE_TUNE_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FINE_TUNE_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FAST_LOCK_BMSK                                0x1
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U_SKIP_FAST_LOCK_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000022c)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000022c)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_RMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_ADDR, HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_IN)
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_RESERVE_BITS95_64_BMSK                     0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_TEST_CTL_U1_RESERVE_BITS95_64_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000230)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000230)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_RMSK                                            0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR, HWIO_SSC_QDSP6SS_PLL_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_STATUS_STATUS_31_0_BMSK                                0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_STATUS_STATUS_31_0_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000234)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000234)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR, HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000238)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000238)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_RMSK                                                   0x7
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR, HWIO_SSC_QDSP6SS_PLL_OPMODE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_OPMODE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_OPMODE_IN)
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_BMSK                                        0x7
#define HWIO_SSC_QDSP6SS_PLL_OPMODE_PLL_OPMODE_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_PLL_STATE_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000023c)
#define HWIO_SSC_QDSP6SS_PLL_STATE_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000023c)
#define HWIO_SSC_QDSP6SS_PLL_STATE_RMSK                                                    0x7
#define HWIO_SSC_QDSP6SS_PLL_STATE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATE_ADDR, HWIO_SSC_QDSP6SS_PLL_STATE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_STATE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_STATE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_STATE_PLL_STATE_BMSK                                          0x7
#define HWIO_SSC_QDSP6SS_PLL_STATE_PLL_STATE_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000240)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000240)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_RMSK                                             0xffff
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_ADDR, HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_IN)
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_VAL_BMSK                               0xffff
#define HWIO_SSC_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_VAL_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000244)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000244)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_RMSK                                                   0xff
#define HWIO_SSC_QDSP6SS_PLL_SPARE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR, HWIO_SSC_QDSP6SS_PLL_SPARE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SPARE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SPARE_IN)
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_OUTPUTS_BMSK                                     0xf0
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_OUTPUTS_SHFT                                      0x4
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_INPUTS_BMSK                                       0xf
#define HWIO_SSC_QDSP6SS_PLL_SPARE_SPARE_INPUTS_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR                                   (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000248)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_OFFS                                   (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000248)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RMSK                                         0xff
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_BMSK                         0xff
#define HWIO_SSC_QDSP6SS_PLL_SSC_DELTA_ALPHA_SSC_DELTA_ALPHA_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR                                     (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000024c)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_OFFS                                     (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000024c)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RMSK                                            0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_BMSK                              0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_NUM_STEPS_SSC_NUM_STEPS_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR                                   (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000250)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_OFFS                                   (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000250)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RMSK                                          0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR, HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_IN)
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_BMSK                          0xf
#define HWIO_SSC_QDSP6SS_PLL_SSC_UPDATE_RATE_SSC_UPDATE_RATE_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR                                 (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000260)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_OFFS                                 (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000260)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK                                        0x3
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_BMSK                     0x2
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_SHFT                     0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_BMSK                     0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR                                    (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000264)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_OFFS                                    (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000264)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK                                           0x3
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_IN)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_BMSK                            0x2
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_SHFT                            0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_BMSK                            0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR                                 (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000268)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OFFS                                 (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000268)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK                                       0x1f
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_BMSK                           0x1f
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR                                    (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000026c)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_OFFS                                    (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000026c)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_RMSK                                           0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000300)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000300)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_RMSK                                                  0xf
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR, HWIO_SSC_QDSP6SS_DBG_NMI_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_DBG_NMI_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_DBG_NMI_CFG_IN)
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_BMSK                               0x8
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_SHFT                               0x3
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_BMSK                       0x4
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_SHFT                       0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_BMSK                           0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_SHFT                           0x0

#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR                                    (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000304)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_OFFS                                    (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000304)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK                                           0x7
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_NMI_CAUSE_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_NMI_CAUSE_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_BMSK                            0x1
#define HWIO_SSC_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000400)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000400)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_RMSK                                              0x1
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR, HWIO_SSC_QDSP6SS_BOOT_CORE_START_RMSK)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR, m)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR,v)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_BOOT_CORE_START_ADDR,m,v,HWIO_SSC_QDSP6SS_BOOT_CORE_START_IN)
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_START_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_BOOT_CORE_START_START_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_BOOT_CMD_ADDR                                              (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000404)
#define HWIO_SSC_QDSP6SS_BOOT_CMD_OFFS                                              (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000404)
#define HWIO_SSC_QDSP6SS_BOOT_CMD_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_BOOT_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BOOT_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_BOOT_CMD_CMD_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_BOOT_CMD_CMD_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000408)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000408)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_RMSK                                                  0x1
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR, HWIO_SSC_QDSP6SS_BOOT_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BOOT_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_STATUS_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_BOOT_STATUS_STATUS_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000410)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000410)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CORE_BHS_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CORE_BHS_CFG_IN)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_BHS_ON_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CFG_BHS_ON_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000414)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000414)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_RMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CORE_BHS_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_UPDATE_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_CMD_UPDATE_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR                                       (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000418)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_OFFS                                       (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000418)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_RMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR, HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_BHS_ON_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_CORE_BHS_STATUS_BHS_ON_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000420)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000420)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_RMSK                                                  0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR, HWIO_SSC_QDSP6SS_LDO_CTL_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_LDO_CTL_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_LDO_CTL_CFG_IN)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BG_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_BMSK                                       0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_PWR_UP_SHFT                                       0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_BYP_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CFG_LDO_BYP_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000424)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000424)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_RMSK                                                  0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_LDO_CTL_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_BMSK                             0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_BG_UPDATE_SHFT                             0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_BMSK                                0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_PWR_UP_UPDATE_SHFT                                0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_BMSK                                   0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_CMD_LDO_BYP_UPDATE_SHFT                                   0x0

#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000428)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000428)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_RMSK                                               0x7
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR, HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_BMSK                                 0x4
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BG_SHFT                                 0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_PWR_UP_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_LDO_CTL_STATUS_LDO_BYP_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_CFG_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000430)
#define HWIO_SSC_QDSP6SS_MEM_CFG_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000430)
#define HWIO_SSC_QDSP6SS_MEM_CFG_RMSK                                                 0x1f001f
#define HWIO_SSC_QDSP6SS_MEM_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR, HWIO_SSC_QDSP6SS_MEM_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_MEM_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_MEM_CFG_IN)
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_BMSK                                 0x1e0000
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_SHFT                                     0x11
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_BMSK                                    0x10000
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_SHFT                                       0x10
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_BMSK                                    0x1e
#define HWIO_SSC_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_CMD_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000434)
#define HWIO_SSC_QDSP6SS_MEM_CMD_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000434)
#define HWIO_SSC_QDSP6SS_MEM_CMD_RMSK                                                      0x3
#define HWIO_SSC_QDSP6SS_MEM_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000438)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000438)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_RMSK                                              0x1f001f
#define HWIO_SSC_QDSP6SS_MEM_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR, HWIO_SSC_QDSP6SS_MEM_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_BMSK                              0x1e0000
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_SHFT                                  0x11
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_BMSK                                 0x10000
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_SHFT                                    0x10
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_BMSK                                 0x1e
#define HWIO_SSC_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_RESET_CFG_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000440)
#define HWIO_SSC_QDSP6SS_RESET_CFG_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000440)
#define HWIO_SSC_QDSP6SS_RESET_CFG_RMSK                                                 0x3fff
#define HWIO_SSC_QDSP6SS_RESET_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR, HWIO_SSC_QDSP6SS_RESET_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_RESET_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_RESET_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_RESET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RESET_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_RESET_CFG_IN)
#define HWIO_SSC_QDSP6SS_RESET_CFG_MNOC_BMSK                                            0x2000
#define HWIO_SSC_QDSP6SS_RESET_CFG_MNOC_SHFT                                               0xd
#define HWIO_SSC_QDSP6SS_RESET_CFG_APB_BMSK                                             0x1000
#define HWIO_SSC_QDSP6SS_RESET_CFG_APB_SHFT                                                0xc
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS2_BMSK                                            0x800
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS2_SHFT                                              0xb
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI1_BMSK                                             0x400
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI1_SHFT                                               0xa
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI0_BMSK                                             0x200
#define HWIO_SSC_QDSP6SS_RESET_CFG_CSI0_SHFT                                               0x9
#define HWIO_SSC_QDSP6SS_RESET_CFG_ARCH_BMSK                                             0x100
#define HWIO_SSC_QDSP6SS_RESET_CFG_ARCH_SHFT                                               0x8
#define HWIO_SSC_QDSP6SS_RESET_CFG_CORE_BMSK                                              0x80
#define HWIO_SSC_QDSP6SS_RESET_CFG_CORE_SHFT                                               0x7
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM2_BMSK                                             0x40
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM2_SHFT                                              0x6
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS_BMSK                                              0x20
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIS_SHFT                                               0x5
#define HWIO_SSC_QDSP6SS_RESET_CFG_ATBM_BMSK                                              0x10
#define HWIO_SSC_QDSP6SS_RESET_CFG_ATBM_SHFT                                               0x4
#define HWIO_SSC_QDSP6SS_RESET_CFG_AHBM_BMSK                                               0x8
#define HWIO_SSC_QDSP6SS_RESET_CFG_AHBM_SHFT                                               0x3
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM_BMSK                                               0x4
#define HWIO_SSC_QDSP6SS_RESET_CFG_AXIM_SHFT                                               0x2
#define HWIO_SSC_QDSP6SS_RESET_CFG_ISDB_ETM_BMSK                                           0x2
#define HWIO_SSC_QDSP6SS_RESET_CFG_ISDB_ETM_SHFT                                           0x1
#define HWIO_SSC_QDSP6SS_RESET_CFG_RET_BMSK                                                0x1
#define HWIO_SSC_QDSP6SS_RESET_CFG_RET_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_RESET_CMD_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000444)
#define HWIO_SSC_QDSP6SS_RESET_CMD_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000444)
#define HWIO_SSC_QDSP6SS_RESET_CMD_RMSK                                                    0x1
#define HWIO_SSC_QDSP6SS_RESET_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RESET_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_RESET_CMD_UPDATE_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_RESET_CMD_UPDATE_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000448)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000448)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RMSK                                              0x3fff
#define HWIO_SSC_QDSP6SS_RESET_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR, HWIO_SSC_QDSP6SS_RESET_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RESET_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_RESET_STATUS_MNOC_BMSK                                         0x2000
#define HWIO_SSC_QDSP6SS_RESET_STATUS_MNOC_SHFT                                            0xd
#define HWIO_SSC_QDSP6SS_RESET_STATUS_APB_BMSK                                          0x1000
#define HWIO_SSC_QDSP6SS_RESET_STATUS_APB_SHFT                                             0xc
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS2_BMSK                                         0x800
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS2_SHFT                                           0xb
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI1_BMSK                                          0x400
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI1_SHFT                                            0xa
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI0_BMSK                                          0x200
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CSI0_SHFT                                            0x9
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ARCH_BMSK                                          0x100
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ARCH_SHFT                                            0x8
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CORE_BMSK                                           0x80
#define HWIO_SSC_QDSP6SS_RESET_STATUS_CORE_SHFT                                            0x7
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM2_BMSK                                          0x40
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM2_SHFT                                           0x6
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS_BMSK                                           0x20
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIS_SHFT                                            0x5
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ATBM_BMSK                                           0x10
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ATBM_SHFT                                            0x4
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AHBM_BMSK                                            0x8
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AHBM_SHFT                                            0x3
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM_BMSK                                            0x4
#define HWIO_SSC_QDSP6SS_RESET_STATUS_AXIM_SHFT                                            0x2
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ISDB_ETM_BMSK                                        0x2
#define HWIO_SSC_QDSP6SS_RESET_STATUS_ISDB_ETM_SHFT                                        0x1
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RET_BMSK                                             0x1
#define HWIO_SSC_QDSP6SS_RESET_STATUS_RET_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000450)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000450)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_RMSK                                                    0xd
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR, HWIO_SSC_QDSP6SS_CLAMP_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLAMP_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CLAMP_CFG_IN)
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_BMSK                                       0x8
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_SHFT                                       0x3
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_BMSK                                      0x4
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_SHFT                                      0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_IO_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CFG_CLAMP_IO_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CLAMP_CMD_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000454)
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000454)
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_RMSK                                                    0xd
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLAMP_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_BMSK                                0x8
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_SHFT                                0x3
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_BMSK                               0x4
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_SHFT                               0x2
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000458)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000458)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_RMSK                                                 0xd
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR, HWIO_SSC_QDSP6SS_CLAMP_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLAMP_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_BMSK                                    0x8
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_SHFT                                    0x3
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_BMSK                                   0x4
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_SHFT                                   0x2
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_IO_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_CLAMP_STATUS_CLAMP_IO_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_CLK_CFG_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000460)
#define HWIO_SSC_QDSP6SS_CLK_CFG_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000460)
#define HWIO_SSC_QDSP6SS_CLK_CFG_RMSK                                                    0x3ff
#define HWIO_SSC_QDSP6SS_CLK_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR, HWIO_SSC_QDSP6SS_CLK_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CLK_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLK_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLK_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CLK_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CLK_CFG_IN)
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS2_BMSK                                              0x200
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS2_SHFT                                                0x9
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI1_BMSK                                               0x100
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI1_SHFT                                                 0x8
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI0_BMSK                                                0x80
#define HWIO_SSC_QDSP6SS_CLK_CFG_CSI0_SHFT                                                 0x7
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM2_BMSK                                               0x40
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM2_SHFT                                                0x6
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS_BMSK                                                0x20
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIS_SHFT                                                 0x5
#define HWIO_SSC_QDSP6SS_CLK_CFG_ATBM_BMSK                                                0x10
#define HWIO_SSC_QDSP6SS_CLK_CFG_ATBM_SHFT                                                 0x4
#define HWIO_SSC_QDSP6SS_CLK_CFG_AHBM_BMSK                                                 0x8
#define HWIO_SSC_QDSP6SS_CLK_CFG_AHBM_SHFT                                                 0x3
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM_BMSK                                                 0x4
#define HWIO_SSC_QDSP6SS_CLK_CFG_AXIM_SHFT                                                 0x2
#define HWIO_SSC_QDSP6SS_CLK_CFG_APB_BMSK                                                  0x2
#define HWIO_SSC_QDSP6SS_CLK_CFG_APB_SHFT                                                  0x1
#define HWIO_SSC_QDSP6SS_CLK_CFG_CORE_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_CLK_CFG_CORE_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_CLK_CMD_ADDR                                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000464)
#define HWIO_SSC_QDSP6SS_CLK_CMD_OFFS                                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000464)
#define HWIO_SSC_QDSP6SS_CLK_CMD_RMSK                                                      0x1
#define HWIO_SSC_QDSP6SS_CLK_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CLK_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CLK_CMD_UPDATE_CLK_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CLK_CMD_UPDATE_CLK_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000468)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000468)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_RMSK                                                 0x3ff
#define HWIO_SSC_QDSP6SS_CLK_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR, HWIO_SSC_QDSP6SS_CLK_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CLK_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS2_BMSK                                           0x200
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS2_SHFT                                             0x9
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI1_BMSK                                            0x100
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI1_SHFT                                              0x8
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI0_BMSK                                             0x80
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CSI0_SHFT                                              0x7
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM2_BMSK                                            0x40
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM2_SHFT                                             0x6
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS_BMSK                                             0x20
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIS_SHFT                                              0x5
#define HWIO_SSC_QDSP6SS_CLK_STATUS_ATBM_BMSK                                             0x10
#define HWIO_SSC_QDSP6SS_CLK_STATUS_ATBM_SHFT                                              0x4
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AHBM_BMSK                                              0x8
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AHBM_SHFT                                              0x3
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM_BMSK                                              0x4
#define HWIO_SSC_QDSP6SS_CLK_STATUS_AXIM_SHFT                                              0x2
#define HWIO_SSC_QDSP6SS_CLK_STATUS_APB_BMSK                                               0x2
#define HWIO_SSC_QDSP6SS_CLK_STATUS_APB_SHFT                                               0x1
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CORE_BMSK                                              0x1
#define HWIO_SSC_QDSP6SS_CLK_STATUS_CORE_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR                                 (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000470)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_OFFS                                 (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000470)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_RMSK                                        0x3
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR, HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_IN)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_ALT_ARES_BMSK                       0x2
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_ALT_ARES_SHFT                       0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_FUNC_ARES_BMSK                      0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_FUNC_ARES_SHFT                      0x0

#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_ADDR                                 (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000474)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_OFFS                                 (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000474)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_RMSK                                        0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_UPDATE_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_CMD_UPDATE_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR                              (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000478)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_OFFS                              (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000478)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_RMSK                                     0x3
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR, HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_ALT_ARES_BMSK                    0x2
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_ALT_ARES_SHFT                    0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_FUNC_ARES_BMSK                   0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_FUNC_ARES_SHFT                   0x0

#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_ADDR                               (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000484)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_OFFS                               (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000484)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RMSK                                      0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RESTORE_CMD_BMSK                          0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RESTORE_CMD_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_ADDR                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000488)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_OFFS                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000488)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RMSK                                   0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RESTORE_BUSY_BMSK                      0x1
#define HWIO_SSC_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RESTORE_BUSY_SHFT                      0x0

#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000508)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000508)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR, HWIO_SSC_QDSP6SS_CP_CLK_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CP_CLK_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_CP_CLK_CTL_IN)
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_CLK_ENA_BMSK                                           0x1
#define HWIO_SSC_QDSP6SS_CP_CLK_CTL_CLK_ENA_SHFT                                           0x0

#define HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000520)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000520)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_RMSK                                                    0x3
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR, HWIO_SSC_QDSP6SS_CPMEM_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPMEM_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_CPMEM_CFG_IN)
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_BMSK                                     0x2
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_SHFT                                     0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_BMSK                                    0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_CPMEM_CMD_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000524)
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000524)
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_RMSK                                                    0x3
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_BMSK                              0x2
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_SHFT                              0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_BMSK                             0x1
#define HWIO_SSC_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000528)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000528)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_RMSK                                                 0x3
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR, HWIO_SSC_QDSP6SS_CPMEM_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_BMSK                                  0x2
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_SHFT                                  0x1
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_BMSK                                 0x1
#define HWIO_SSC_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000530)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000530)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_RMSK                                              0x3f003f
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_CFG_ADDR, HWIO_SSC_QDSP6SS_L1SMEM_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L1SMEM_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L1SMEM_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_L1SMEM_CFG_IN)
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_L1S_SLP_RET_N_BMSK                                0x3f0000
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_L1S_SLP_RET_N_SHFT                                    0x10
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_L1S_SLP_NRET_N_BMSK                                   0x3f
#define HWIO_SSC_QDSP6SS_L1SMEM_CFG_L1S_SLP_NRET_N_SHFT                                    0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000534)
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000534)
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_RMSK                                                   0x3
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L1SMEM_CMD_ADDR,v)
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_RET_N_BMSK                              0x2
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_RET_N_SHFT                              0x1
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_NRET_N_BMSK                             0x1
#define HWIO_SSC_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_NRET_N_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00000538)
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00000538)
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_RMSK                                           0x3f003f
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_STATUS_ADDR, HWIO_SSC_QDSP6SS_L1SMEM_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_L1S_SLP_RET_N_BMSK                             0x3f0000
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_L1S_SLP_RET_N_SHFT                                 0x10
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_L1S_SLP_NRET_N_BMSK                                0x3f
#define HWIO_SSC_QDSP6SS_L1SMEM_STATUS_L1S_SLP_NRET_N_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002000)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002000)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_BUS_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_BUS_CTL_IN)
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_BMSK                                0xff000000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_SHFT                                      0x18
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_INV_BMSK                                    0x800000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_INV_SHFT                                        0x17
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_SEL_BMSK                                    0x7c0000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_HWE_SEL_SHFT                                        0x12
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_BMSK                                  0x20000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_SHFT                                     0x11
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_Q6_SEL_BMSK                                      0x10000
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_Q6_SEL_SHFT                                         0x10
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SEL_BMSK                                          0xffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_CTL_SEL_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002004)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002004)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_VALUE_BMSK                                  0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_VALUE_VALUE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR                                        (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002008)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_OFFS                                        (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002008)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_RMSK                                        0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR, HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_IN)
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_VALUE_BMSK                                  0xffffffff
#define HWIO_SSC_QDSP6SS_TEST_BUS_WDATA_VALUE_SHFT                                         0x0

#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR                                            (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000200c)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_OFFS                                            (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000200c)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_RMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR, HWIO_SSC_QDSP6SS_PWRDBG_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_PWRDBG_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_PWRDBG_CTL_IN)
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ENA_BMSK                                               0x1
#define HWIO_SSC_QDSP6SS_PWRDBG_CTL_ENA_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002010)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002010)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_RMSK                                                0x7f
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR, HWIO_SSC_QDSP6SS_TEST_CLK_CTL_RMSK)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR, m)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR,v)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_TEST_CLK_CTL_ADDR,m,v,HWIO_SSC_QDSP6SS_TEST_CLK_CTL_IN)
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_BMSK                                  0x40
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_SHFT                                   0x6
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_BMSK                                 0x20
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_SHFT                                  0x5
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_BMSK                                    0x1f
#define HWIO_SSC_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_SHFT                                     0x0

#define HWIO_SSC_QDSP6SS_BHS_TEST_ADDR                                              (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002014)
#define HWIO_SSC_QDSP6SS_BHS_TEST_OFFS                                              (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002014)
#define HWIO_SSC_QDSP6SS_BHS_TEST_RMSK                                                     0x1
#define HWIO_SSC_QDSP6SS_BHS_TEST_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR, HWIO_SSC_QDSP6SS_BHS_TEST_RMSK)
#define HWIO_SSC_QDSP6SS_BHS_TEST_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR, m)
#define HWIO_SSC_QDSP6SS_BHS_TEST_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR,v)
#define HWIO_SSC_QDSP6SS_BHS_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_BHS_TEST_ADDR,m,v,HWIO_SSC_QDSP6SS_BHS_TEST_IN)
#define HWIO_SSC_QDSP6SS_BHS_TEST_INSTANT_ON_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_BHS_TEST_INSTANT_ON_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002018)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002018)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_RMSK                                            0xffffff
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR, HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_RMSK)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR, m)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR,v)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ADDR,m,v,HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_IN)
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_BMSK                                0xffffe0
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_SHFT                                     0x5
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_BMSK                                        0x10
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_SHFT                                         0x4
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_BMSK                                 0x8
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_SHFT                                 0x3
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_BMSK                                    0x4
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_SHFT                                    0x2
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_BMSK                                  0x1
#define HWIO_SSC_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_SHFT                                  0x0

#define HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002028)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002028)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_RMSK                                                 0x3f
#define HWIO_SSC_QDSP6SS_CORE_STATUS_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR, HWIO_SSC_QDSP6SS_CORE_STATUS_RMSK)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CORE_STATUS_ADDR, m)
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU1_VX_IDLE_BMSK                                     0x20
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU1_VX_IDLE_SHFT                                      0x5
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU0_VX_IDLE_BMSK                                     0x10
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU0_VX_IDLE_SHFT                                      0x4
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU_OVERALL_IDLE_BMSK                                  0x8
#define HWIO_SSC_QDSP6SS_CORE_STATUS_VU_OVERALL_IDLE_SHFT                                  0x3
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CP0_IDLE_BMSK                                         0x4
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CP0_IDLE_SHFT                                         0x2
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_BMSK                                    0x2
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_SHFT                                    0x1
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_STATE_BMSK                                       0x1
#define HWIO_SSC_QDSP6SS_CORE_STATUS_CORE_STATE_SHFT                                       0x0

#define HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR                                             (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002030)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OFFS                                             (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002030)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_RMSK                                                 0xffff
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR, HWIO_SSC_QDSP6SS_MEM_DEBUG_RMSK)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR, m)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR,v)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_MEM_DEBUG_ADDR,m,v,HWIO_SSC_QDSP6SS_MEM_DEBUG_IN)
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OFF_BMSK                                             0xffff
#define HWIO_SSC_QDSP6SS_MEM_DEBUG_OFF_SHFT                                                0x0

#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002034)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002034)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR, HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_RMSK)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR, m)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR,v)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_ADDR,m,v,HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_IN)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OFF_BMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG0_OFF_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR                                          (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002038)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OFFS                                          (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002038)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_RMSK                                          0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR, HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_RMSK)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR, m)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR,v)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_ADDR,m,v,HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_IN)
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OFF_BMSK                                      0xffffffff
#define HWIO_SSC_QDSP6SS_L2MEM_DEBUG1_OFF_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR                                           (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x0000203c)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OFFS                                           (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x0000203c)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_RMSK                                           0xffffffff
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR, HWIO_SSC_QDSP6SS_CPMEM_DEBUG_RMSK)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR, m)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR,v)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_CPMEM_DEBUG_ADDR,m,v,HWIO_SSC_QDSP6SS_CPMEM_DEBUG_IN)
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OFF_BMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_CPMEM_DEBUG_OFF_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002040)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002040)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_RMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_ADDR, HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_RMSK)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_ADDR, m)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_ADDR,v)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_ADDR,m,v,HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_IN)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_OFF_BMSK                                     0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG0_OFF_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002044)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002044)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_RMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_ADDR, HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_RMSK)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_ADDR, m)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_ADDR,v)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_ADDR,m,v,HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_IN)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_OFF_BMSK                                     0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG1_OFF_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_ADDR                                         (SSC_QDSP6V66SS_PUB_REG_BASE      + 0x00002048)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_OFFS                                         (SSC_QDSP6V66SS_PUB_REG_BASE_OFFS + 0x00002048)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_RMSK                                         0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_ADDR, HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_RMSK)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_ADDR, m)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_ADDR,v)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_ADDR,m,v,HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_IN)
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_OFF_BMSK                                     0xffffffff
#define HWIO_SSC_QDSP6SS_L1SMEM_DEBUG2_OFF_SHFT                                            0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6V66SS_PRIVATE
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6V66SS_PRIVATE_REG_BASE (SSC_BASE      + 0x00480000)
#define SSC_QDSP6V66SS_PRIVATE_REG_BASE_OFFS 0x00480000

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6SS_QDSP6SS_QTMR_AC
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE                                (SSC_BASE      + 0x004a0000)
#define SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS                           0x004a0000

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_ADDR                                (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_OFFS                                (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_RMSK                                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_IN)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                         0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                                0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_ADDR                           (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_OFFS                           (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_NSN_BMSK                              0x7
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTNSAR_FG0_NSN_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_ADDR                           (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_OFFS                           (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_RMSK                           0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F7_CFG_BMSK                    0xf0000000
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F7_CFG_SHFT                          0x1c
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F6_CFG_BMSK                     0xf000000
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F6_CFG_SHFT                          0x18
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F5_CFG_BMSK                      0xf00000
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F5_CFG_SHFT                          0x14
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F4_CFG_BMSK                       0xf0000
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F4_CFG_SHFT                          0x10
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F3_CFG_BMSK                        0xf000
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F3_CFG_SHFT                           0xc
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F2_CFG_BMSK                         0xf00
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F2_CFG_SHFT                           0x8
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F1_CFG_BMSK                          0xf0
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F1_CFG_SHFT                           0x4
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F0_CFG_BMSK                           0xf
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTTIDR_FG0_F0_CFG_SHFT                           0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(n)                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_OFFS(n)                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RMSK                                 0x3f
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_MAXn                                    2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_INI(n)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(n), HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(n), mask)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_OUTI(n,val)    \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(n),val)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_ADDR(n),mask,val,HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_INI(n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RWPT_BMSK                            0x20
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RWPT_SHFT                             0x5
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RWVT_BMSK                            0x10
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RWVT_SHFT                             0x4
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RVOFF_BMSK                            0x8
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RVOFF_SHFT                            0x3
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RFRQ_BMSK                             0x4
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RFRQ_SHFT                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RPVCT_BMSK                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RPVCT_SHFT                            0x1
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RPCT_BMSK                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTACRn_FG0_RPCT_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n)                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_OFFS(n)                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000080 + 0x8 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK                      0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_MAXn                               2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), mask)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_OUTI(n,val)    \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),val)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),mask,val,HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_BMSK           0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_SHFT                  0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n)                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_OFFS(n)                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000084 + 0x8 * (n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK                        0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_MAXn                               2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), mask)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_OUTI(n,val)    \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),val)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),mask,val,HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n))
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_BMSK             0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_SHFT                  0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_ADDR                                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000fc0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_OFFS                                   (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000fc0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_RMSK                                          0x3
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CFG_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_CFG_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_CFG_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_CFG_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_CFG_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_AC_CFG_IN)
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_DSBL_ATOMIC_BMSK                              0x2
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_DSBL_ATOMIC_SHFT                              0x1
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_TEST_BUS_EN_BMSK                              0x1
#define HWIO_SSC_QDSP6SS_QTMR_AC_CFG_TEST_BUS_EN_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_ADDR                               (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_OFFS                               (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_RMSK                               0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_MAJOR_BMSK                         0xf0000000
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_MAJOR_SHFT                               0x1c
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_MINOR_BMSK                          0xfff0000
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_MINOR_SHFT                               0x10
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_STEP_BMSK                              0xffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_VERSION_STEP_SHFT                                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_ADDR                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000fe0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_OFFS                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000fe0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_RMSK                        0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_ADDR                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000ff0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_OFFS                        (SSC_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_OFFS + 0x00000ff0)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_RMSK                        0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6SS_QTMR_F0_0
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6SS_QTMR_F0_0_REG_BASE                                     (SSC_BASE      + 0x004a1000)
#define SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS                                0x004a1000

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_0_CNTPCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_0_CNTPCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_0_CNTVCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_0_CNTVCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR                             (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_OFFS                             (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_RMSK                             0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_CNTFRQ_BMSK                      0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_0_CNTFRQ_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_RMSK                               0x303
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0CTEN_BMSK                       0x200
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0CTEN_SHFT                         0x9
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VTEN_BMSK                       0x100
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VTEN_SHFT                         0x8
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0VCTEN_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_BMSK                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_0_PL0PCTEN_SHFT                        0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR                         (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_OFFS                         (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_RMSK                         0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_0_CNTVOFF_L0_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR                         (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_OFFS                         (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_RMSK                           0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_0_CNTVOFF_HI_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_OFFS                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_0_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_OFFS                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_0_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_0_CNTP_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR                           (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_OFFS                           (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_0_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_OFFS                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_0_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_OFFS                       (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_0_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_OFFS                          (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_0_CNTV_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR                           (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_OFFS                           (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_0_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_ADDR                            (SSC_QDSP6SS_QTMR_F0_0_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_OFFS                            (SSC_QDSP6SS_QTMR_F0_0_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_RMSK                            0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_MAJOR_BMSK                      0xf0000000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_MAJOR_SHFT                            0x1c
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_MINOR_BMSK                       0xfff0000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_MINOR_SHFT                            0x10
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_STEP_BMSK                           0xffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_0_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6SS_QTMR_F1_1
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6SS_QTMR_F1_1_REG_BASE                                     (SSC_BASE      + 0x004a2000)
#define SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS                                0x004a2000

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_1_CNTPCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_1_CNTVCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_1_CNTVCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR                             (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_OFFS                             (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_RMSK                             0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_CNTFRQ_BMSK                      0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_1_CNTFRQ_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_RMSK                               0x303
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0CTEN_BMSK                       0x200
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0CTEN_SHFT                         0x9
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VTEN_BMSK                       0x100
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VTEN_SHFT                         0x8
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VCTEN_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0VCTEN_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0PCTEN_BMSK                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_1_PL0PCTEN_SHFT                        0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR                         (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_OFFS                         (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_RMSK                         0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_1_CNTVOFF_L0_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR                         (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_OFFS                         (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_RMSK                           0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_1_CNTVOFF_HI_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_OFFS                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_1_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_OFFS                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_1_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_1_CNTP_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR                           (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_OFFS                           (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_1_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_OFFS                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_1_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_OFFS                       (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_1_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_OFFS                          (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_1_CNTV_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR                           (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_OFFS                           (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_1_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_ADDR                            (SSC_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_OFFS                            (SSC_QDSP6SS_QTMR_F1_1_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_RMSK                            0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_MAJOR_BMSK                      0xf0000000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_MAJOR_SHFT                            0x1c
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_MINOR_BMSK                       0xfff0000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_MINOR_SHFT                            0x10
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_STEP_BMSK                           0xffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_1_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6SS_QTMR_F2_2
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6SS_QTMR_F2_2_REG_BASE                                     (SSC_BASE      + 0x004a3000)
#define SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS                                0x004a3000

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_LO_2_CNTPCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPCT_HI_2_CNTPCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000008)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_LO_2_CNTVCT_LO_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_RMSK                            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVCT_HI_2_CNTVCT_HI_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_ADDR                             (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_OFFS                             (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_RMSK                             0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_CNTFRQ_BMSK                      0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTFRQ_2_CNTFRQ_SHFT                             0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000014)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_RMSK                               0x303
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0CTEN_BMSK                       0x200
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0CTEN_SHFT                         0x9
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0VTEN_BMSK                       0x100
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0VTEN_SHFT                         0x8
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0VCTEN_BMSK                        0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0VCTEN_SHFT                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0PCTEN_BMSK                        0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTPL0ACR_2_PL0PCTEN_SHFT                        0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_ADDR                         (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_OFFS                         (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000018)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_RMSK                         0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_LO_2_CNTVOFF_L0_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_ADDR                         (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_OFFS                         (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_RMSK                           0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTVOFF_HI_2_CNTVOFF_HI_SHFT                     0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_ADDR                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_OFFS                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_LO_2_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_ADDR                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_OFFS                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000024)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CVAL_HI_2_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000028)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_TVAL_2_CNTP_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ADDR                           (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_OFFS                           (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTP_CTL_2_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_ADDR                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_OFFS                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000030)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_RMSK                       0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_LO_2_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_ADDR                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_OFFS                       (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000034)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_RMSK                         0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CVAL_HI_2_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_ADDR                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_OFFS                          (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000038)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_RMSK                          0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_TVAL_2_CNTV_TVAL_SHFT                       0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ADDR                           (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_OFFS                           (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_RMSK                                  0x7
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ADDR,v)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ADDR,m,v,HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_IN)
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ISTAT_BMSK                            0x4
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_ISTAT_SHFT                            0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_IMSK_BMSK                             0x2
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_IMSK_SHFT                             0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_EN_BMSK                               0x1
#define HWIO_SSC_QDSP6SS_QTMR_V1_CNTV_CTL_2_EN_SHFT                               0x0

#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_ADDR                            (SSC_QDSP6SS_QTMR_F2_2_REG_BASE      + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_OFFS                            (SSC_QDSP6SS_QTMR_F2_2_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_RMSK                            0xffffffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_ADDR, HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_RMSK)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_ADDR, m)
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_MAJOR_BMSK                      0xf0000000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_MAJOR_SHFT                            0x1c
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_MINOR_BMSK                       0xfff0000
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_MINOR_SHFT                            0x10
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_STEP_BMSK                           0xffff
#define HWIO_SSC_QDSP6SS_QTMR_V1_VERSION_2_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_QDSP6SS_RSCC_RSCC_RSC
 *--------------------------------------------------------------------------*/

#define SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE                                                                              (SSC_BASE      + 0x004b0000)
#define SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS                                                                         0x004b0000

#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_ADDR(d)                                                                       (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000000 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_OFFS(d)                                                                       (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000000 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_RMSK                                                                            0xffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_MAXd                                                                                   0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                  0xff0000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                      0x10
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_MINOR_VER_BMSK                                                                    0xff00
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_MINOR_VER_SHFT                                                                       0x8
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_STEP_VER_BMSK                                                                       0xff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ID_DRVd_STEP_VER_SHFT                                                                        0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d)                                                      (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000004 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_OFFS(d)                                                      (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000004 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                          0x71f1f1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                  0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_BMSK                                             0x4000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_SHFT                                                  0x1a
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                             0x2000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                  0x19
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                0x1000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                     0x18
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                              0x1f0000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                  0x10
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                0x1f00
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                   0x8
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                    0x1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                     0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d)                                                         (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000008 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_OFFS(d)                                                         (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000008 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                             0x7ff0f1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                     0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                     0x7000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                          0x18
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                            0xff0000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                0x10
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                   0xf00
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                     0x8
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                              0x1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d)                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x0000000c + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_OFFS(d)                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x0000000c + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                0xf8ffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                         0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                               0xf8000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                     0x1b
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                     0xfc0000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                         0x12
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                      0x3f000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                          0xc
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                        0xfc0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                          0x6
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                         0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_ADDR(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000010 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_OFFS(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000010 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_RMSK                                                                      0x101ffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_MAXd                                                                              0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_OUTI(d,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_ADDR(d),val)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_OUTMI(d,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_ADDR(d),mask,val,HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_INI(d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_BMSK                                                     0x1000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_SHFT                                                          0x18
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_BMSK                                                0x10000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_SHFT                                                   0x10
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                    0xffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                       0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_ADDR(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000014 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_OFFS(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000014 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_RMSK                                                                     0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_MAXd                                                                              0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                  0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                         0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_ADDR(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000018 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_OFFS(d)                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000018 + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_RMSK                                                                       0xffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_MAXd                                                                              0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_INI(d)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_ADDR(d), HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_INMI(d,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_ADDR(d), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                    0xffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                         0x0

#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_ADDR                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x0000004c)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_OFFS                                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x0000004c)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_RMSK                                                                         0x3
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                   0x3
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                   0x0

#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m)                                                       (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OFFS(d,m)                                                       (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                            0x80000007
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                     0
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                    31
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),val)
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ADDR(d,m),mask,val,HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_INI2(d,m))
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                     0x80000000
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                           0x1f
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                      0x7
#define HWIO_SSC_QDSP6SS_RSCC_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                      0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000000d0)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_OFFS                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000000d0)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                   0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                  0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                  0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR                                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000000d4)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OFFS                                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000000d4)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                    0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                               0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                               0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000000d8)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OFFS                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000000d8)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                              0x10ffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                          0x100000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                              0x14
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                         0xffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                            0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ADDR                                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000100)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_OFFS                                                             (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000100)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_RMSK                                                                    0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ERROR_RESP_FOR_INVALID_ADDR_BMSK                                        0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_ERROR_RESP_CTRL_DRV0_ERROR_RESP_FOR_INVALID_ADDR_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000200)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OFFS                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000200)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                               0x3
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                         0x3
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                         0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m)                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OFFS(d,m)                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                   0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                   5
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(d,m,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),val)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(d,m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(d,m),mask,val,HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(d,m))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                          0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                          0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m)                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_OFFS(d,m)                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                 0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                          0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                          5
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                       0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m)                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_OFFS(d,m)                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                   0xffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                          0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                          5
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                         0xffffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                              0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m)                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_OFFS(d,m)                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                           0x101
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                               0
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                               5
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(d,m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(d,m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(d,m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                               0x100
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                 0x8
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                    0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                    0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000400)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OFFS                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000400)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                               0x80000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                     0x1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                     0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                       0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_ADDR                                                                    (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000404)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_OFFS                                                                    (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000404)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_RMSK                                                                           0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                 0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR                                                         (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000408)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_OFFS                                                         (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000408)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                              0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                           0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                             0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i)                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000410 + 0x4 * (i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OFFS(i)                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000410 + 0x4 * (i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                    3
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),val)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(i),mask,val,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                           0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                             0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i)                                                      (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000450 + 0x4 * (i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OFFS(i)                                                      (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000450 + 0x4 * (i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                            0xfffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                  3
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(i,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i), mask)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(i,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),val)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(i,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(i),mask,val,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(i))
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                  0xfffff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                      0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000460)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OFFS                                                        (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000460)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                               0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                          0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                          0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR                                          (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000464)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OFFS                                          (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000464)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                               0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK               0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                 0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000490)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OFFS                                                     (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000490)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                     0x800003ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                0x80000000
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                      0x1f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                      0x3ff
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                        0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR                                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000494)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_OFFS                                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000494)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                       0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                      0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                      0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000498)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OFFS                                                            (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000498)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                   0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                 0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                 0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR                                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x0000049c)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_OFFS                                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x0000049c)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                       0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                   0x1
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                   0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000004a0)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OFFS                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000004a0)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                       0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                           0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000004a4)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OFFS                                                  (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000004a4)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                        0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                             0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                              0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000004a8)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OFFS                                                (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000004a8)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                      0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                         0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                          0x0

#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x000004ac)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OFFS                                                 (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x000004ac)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                       0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN          \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(m)      \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR, m)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(v)      \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,v)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR,m,v,HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN)
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                           0x3f
#define HWIO_SSC_QDSP6SS_RSCC_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                            0x0

#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_ADDR(m)                                                                    (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE      + 0x00000600 + 0x4 * (m))
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_OFFS(m)                                                                    (SSC_QDSP6SS_RSCC_RSCC_RSC_REG_BASE_OFFS + 0x00000600 + 0x4 * (m))
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_RMSK                                                                       0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_MAXm                                                                               95
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_INI(m)        \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_ADDR(m), HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_INMI(m,mask)    \
        in_dword_masked(HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_ADDR(m), mask)
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_OUTI(m,val)    \
        out_dword(HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_ADDR(m),val)
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_ADDR(m),mask,val,HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_INI(m))
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                              0xffffffff
#define HWIO_SSC_QDSP6SS_RSCC_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: SSC_MCC_REGS
 *--------------------------------------------------------------------------*/

#define SSC_MCC_REGS_REG_BASE                                                    (SSC_BASE      + 0x00b00000)
#define SSC_MCC_REGS_REG_BASE_OFFS                                               0x00b00000

#define HWIO_SSC_SSCAON_SPARE_ADDR                                               (SSC_MCC_REGS_REG_BASE      + 0x00000000)
#define HWIO_SSC_SSCAON_SPARE_OFFS                                               (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000000)
#define HWIO_SSC_SSCAON_SPARE_RMSK                                               0xffffffff
#define HWIO_SSC_SSCAON_SPARE_IN          \
        in_dword_masked(HWIO_SSC_SSCAON_SPARE_ADDR, HWIO_SSC_SSCAON_SPARE_RMSK)
#define HWIO_SSC_SSCAON_SPARE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSCAON_SPARE_ADDR, m)
#define HWIO_SSC_SSCAON_SPARE_OUT(v)      \
        out_dword(HWIO_SSC_SSCAON_SPARE_ADDR,v)
#define HWIO_SSC_SSCAON_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSCAON_SPARE_ADDR,m,v,HWIO_SSC_SSCAON_SPARE_IN)
#define HWIO_SSC_SSCAON_SPARE_SSCAON_SSC_SPARE_IN_BMSK                           0xffff0000
#define HWIO_SSC_SSCAON_SPARE_SSCAON_SSC_SPARE_IN_SHFT                                 0x10
#define HWIO_SSC_SSCAON_SPARE_SSCAON_SSC_SPARE_OUT_BMSK                              0xffff
#define HWIO_SSC_SSCAON_SPARE_SSCAON_SSC_SPARE_OUT_SHFT                                 0x0

#define HWIO_SSC_SSCAON_SM_CSR1_ADDR                                             (SSC_MCC_REGS_REG_BASE      + 0x00000004)
#define HWIO_SSC_SSCAON_SM_CSR1_OFFS                                             (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000004)
#define HWIO_SSC_SSCAON_SM_CSR1_RMSK                                                  0x1ff
#define HWIO_SSC_SSCAON_SM_CSR1_IN          \
        in_dword_masked(HWIO_SSC_SSCAON_SM_CSR1_ADDR, HWIO_SSC_SSCAON_SM_CSR1_RMSK)
#define HWIO_SSC_SSCAON_SM_CSR1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSCAON_SM_CSR1_ADDR, m)
#define HWIO_SSC_SSCAON_SM_CSR1_OUT(v)      \
        out_dword(HWIO_SSC_SSCAON_SM_CSR1_ADDR,v)
#define HWIO_SSC_SSCAON_SM_CSR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSCAON_SM_CSR1_ADDR,m,v,HWIO_SSC_SSCAON_SM_CSR1_IN)
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_TMR_EN_BMSK                                       0x100
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_TMR_EN_SHFT                                         0x8
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_WR_BMSK                                    0x80
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_WR_SHFT                                     0x7
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_BMSK                                       0x7f
#define HWIO_SSC_SSCAON_SM_CSR1_PLL_LOCKDLY_SHFT                                        0x0

#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_ADDR                                 (SSC_MCC_REGS_REG_BASE      + 0x00000008)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_OFFS                                 (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000008)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_RMSK                                        0x3
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_ADDR, HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_RMSK)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_ADDR, m)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_ADDR,v)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_ADDR,m,v,HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_IN)
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_VAL_BMSK                                    0x3
#define HWIO_SSC_SSC_RSC_TS_PROFILING_EVENT_VAL_SHFT                                    0x0

#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR                                   (SSC_MCC_REGS_REG_BASE      + 0x0000000c)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_OFFS                                   (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_RMSK                                     0x1fffff
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR, HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_RMSK)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR, m)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR,v)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_ADDR,m,v,HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_IN)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_VAL_BMSK                                 0x1fffff
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_VAL_VAL_SHFT                                      0x0

#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR                                  (SSC_MCC_REGS_REG_BASE      + 0x00000010)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_OFFS                                  (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000010)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_RMSK                                    0x1fffff
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR, HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_RMSK)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR, m)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR,v)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_ADDR,m,v,HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_IN)
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_MASK_VAL_BMSK                           0x1fffff
#define HWIO_SSC_SSC_RSC_PWR_CTL_OVRD_MASK_MASK_VAL_SHFT                                0x0

#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00000014)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000014)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_RMSK                                  0x1fffff
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR, HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_RMSK)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR, m)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR,v)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_ADDR,m,v,HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_IN)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_VAL_BMSK                              0x1fffff
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_VAL_VAL_SHFT                                   0x0

#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR                               (SSC_MCC_REGS_REG_BASE      + 0x00000018)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_OFFS                               (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000018)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_RMSK                                 0x1fffff
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR, HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_RMSK)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR, m)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR,v)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_ADDR,m,v,HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_IN)
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_MASK_VAL_BMSK                        0x1fffff
#define HWIO_SSC_SSC_RSC_WAIT_EVENT_OVRD_MASK_MASK_VAL_SHFT                             0x0

#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR                                  (SSC_MCC_REGS_REG_BASE      + 0x0000001c)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_OFFS                                  (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_RMSK                                         0xf
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR, HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_RMSK)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR, m)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR,v)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_ADDR,m,v,HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_IN)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_VAL_BMSK                                     0xf
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_VAL_VAL_SHFT                                     0x0

#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR                                 (SSC_MCC_REGS_REG_BASE      + 0x00000020)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_OFFS                                 (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000020)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_RMSK                                        0xf
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR, HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_RMSK)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR, m)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR,v)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_ADDR,m,v,HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_IN)
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_MASK_VAL_BMSK                               0xf
#define HWIO_SSC_SSC_RSC_BR_EVENT_OVRD_MASK_MASK_VAL_SHFT                               0x0

#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x00000024)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000024)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_RMSK                                               0x3
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_IN          \
        in_dword_masked(HWIO_SSC_SSC_CCD_TESTBUS_SEL_ADDR, HWIO_SSC_SSC_CCD_TESTBUS_SEL_RMSK)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_CCD_TESTBUS_SEL_ADDR, m)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_CCD_TESTBUS_SEL_ADDR,v)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_CCD_TESTBUS_SEL_ADDR,m,v,HWIO_SSC_SSC_CCD_TESTBUS_SEL_IN)
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_SEL_BMSK                                           0x3
#define HWIO_SSC_SSC_CCD_TESTBUS_SEL_SEL_SHFT                                           0x0

#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR                                    (SSC_MCC_REGS_REG_BASE      + 0x00000028)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_OFFS                                    (SSC_MCC_REGS_REG_BASE_OFFS + 0x00000028)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_RMSK                                           0x3
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_IN          \
        in_dword_masked(HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR, HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_RMSK)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR, m)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_OUT(v)      \
        out_dword(HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR,v)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ADDR,m,v,HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_IN)
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ISLAND_MODE_BMSK                               0x2
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_ISLAND_MODE_SHFT                               0x1
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_DISABLE_GCC_TO_SSC_CLKS_BMSK                   0x1
#define HWIO_SSC_SSC_GCC_TO_SSC_CLKS_OFF_DISABLE_GCC_TO_SSC_CLKS_SHFT                   0x0

#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_ADDR                                       (SSC_MCC_REGS_REG_BASE      + 0x00001000)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_OFFS                                       (SSC_MCC_REGS_REG_BASE_OFFS + 0x00001000)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_RMSK                                              0x1
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_IN          \
        in_dword_masked(HWIO_SSC_SSC_TEST_BUS_SEL_CFG_ADDR, HWIO_SSC_SSC_TEST_BUS_SEL_CFG_RMSK)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TEST_BUS_SEL_CFG_ADDR, m)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_OUT(v)      \
        out_dword(HWIO_SSC_SSC_TEST_BUS_SEL_CFG_ADDR,v)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TEST_BUS_SEL_CFG_ADDR,m,v,HWIO_SSC_SSC_TEST_BUS_SEL_CFG_IN)
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_SSC_TEST_BUS_SEL_BMSK                             0x1
#define HWIO_SSC_SSC_TEST_BUS_SEL_CFG_SSC_TEST_BUS_SEL_SHFT                             0x0

#define HWIO_SSC_SSC_HW_VERSION_ADDR                                             (SSC_MCC_REGS_REG_BASE      + 0x00001004)
#define HWIO_SSC_SSC_HW_VERSION_OFFS                                             (SSC_MCC_REGS_REG_BASE_OFFS + 0x00001004)
#define HWIO_SSC_SSC_HW_VERSION_RMSK                                             0xffffffff
#define HWIO_SSC_SSC_HW_VERSION_IN          \
        in_dword_masked(HWIO_SSC_SSC_HW_VERSION_ADDR, HWIO_SSC_SSC_HW_VERSION_RMSK)
#define HWIO_SSC_SSC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_HW_VERSION_ADDR, m)
#define HWIO_SSC_SSC_HW_VERSION_MAJOR_BMSK                                       0xf0000000
#define HWIO_SSC_SSC_HW_VERSION_MAJOR_SHFT                                             0x1c
#define HWIO_SSC_SSC_HW_VERSION_MINOR_BMSK                                        0xfff0000
#define HWIO_SSC_SSC_HW_VERSION_MINOR_SHFT                                             0x10
#define HWIO_SSC_SSC_HW_VERSION_STEP_BMSK                                            0xffff
#define HWIO_SSC_SSC_HW_VERSION_STEP_SHFT                                               0x0

#define HWIO_SSC_SSC_XO_CLK_GATE_ADDR                                            (SSC_MCC_REGS_REG_BASE      + 0x00001008)
#define HWIO_SSC_SSC_XO_CLK_GATE_OFFS                                            (SSC_MCC_REGS_REG_BASE_OFFS + 0x00001008)
#define HWIO_SSC_SSC_XO_CLK_GATE_RMSK                                                   0x1
#define HWIO_SSC_SSC_XO_CLK_GATE_IN          \
        in_dword_masked(HWIO_SSC_SSC_XO_CLK_GATE_ADDR, HWIO_SSC_SSC_XO_CLK_GATE_RMSK)
#define HWIO_SSC_SSC_XO_CLK_GATE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_XO_CLK_GATE_ADDR, m)
#define HWIO_SSC_SSC_XO_CLK_GATE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_XO_CLK_GATE_ADDR,v)
#define HWIO_SSC_SSC_XO_CLK_GATE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_XO_CLK_GATE_ADDR,m,v,HWIO_SSC_SSC_XO_CLK_GATE_IN)
#define HWIO_SSC_SSC_XO_CLK_GATE_SSC_XO_CLK_GATE_EN_BMSK                                0x1
#define HWIO_SSC_SSC_XO_CLK_GATE_SSC_XO_CLK_GATE_EN_SHFT                                0x0

#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_ADDR                                 (SSC_MCC_REGS_REG_BASE      + 0x0000100c)
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_OFFS                                 (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000100c)
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_RMSK                                        0x1
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_IN          \
        in_dword_masked(HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_ADDR, HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_RMSK)
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_ADDR, m)
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_DISABLE_BMSK                                0x1
#define HWIO_SSC_SSC_SW_ISLAND_MODE_DISABLE_DISABLE_SHFT                                0x0

#define HWIO_SSC_SSC_I3C0_CFG_ADDR                                               (SSC_MCC_REGS_REG_BASE      + 0x00001010)
#define HWIO_SSC_SSC_I3C0_CFG_OFFS                                               (SSC_MCC_REGS_REG_BASE_OFFS + 0x00001010)
#define HWIO_SSC_SSC_I3C0_CFG_RMSK                                                     0x3f
#define HWIO_SSC_SSC_I3C0_CFG_IN          \
        in_dword_masked(HWIO_SSC_SSC_I3C0_CFG_ADDR, HWIO_SSC_SSC_I3C0_CFG_RMSK)
#define HWIO_SSC_SSC_I3C0_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_I3C0_CFG_ADDR, m)
#define HWIO_SSC_SSC_I3C0_CFG_OUT(v)      \
        out_dword(HWIO_SSC_SSC_I3C0_CFG_ADDR,v)
#define HWIO_SSC_SSC_I3C0_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_I3C0_CFG_ADDR,m,v,HWIO_SSC_SSC_I3C0_CFG_IN)
#define HWIO_SSC_SSC_I3C0_CFG_SDA_I3C0_CFG_MODE_BMSK                                   0x38
#define HWIO_SSC_SSC_I3C0_CFG_SDA_I3C0_CFG_MODE_SHFT                                    0x3
#define HWIO_SSC_SSC_I3C0_CFG_SCL_I3C0_CFG_MODE_BMSK                                    0x7
#define HWIO_SSC_SSC_I3C0_CFG_SCL_I3C0_CFG_MODE_SHFT                                    0x0

#define HWIO_SSC_SSC_I3C1_CFG_ADDR                                               (SSC_MCC_REGS_REG_BASE      + 0x00001014)
#define HWIO_SSC_SSC_I3C1_CFG_OFFS                                               (SSC_MCC_REGS_REG_BASE_OFFS + 0x00001014)
#define HWIO_SSC_SSC_I3C1_CFG_RMSK                                                     0x3f
#define HWIO_SSC_SSC_I3C1_CFG_IN          \
        in_dword_masked(HWIO_SSC_SSC_I3C1_CFG_ADDR, HWIO_SSC_SSC_I3C1_CFG_RMSK)
#define HWIO_SSC_SSC_I3C1_CFG_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_I3C1_CFG_ADDR, m)
#define HWIO_SSC_SSC_I3C1_CFG_OUT(v)      \
        out_dword(HWIO_SSC_SSC_I3C1_CFG_ADDR,v)
#define HWIO_SSC_SSC_I3C1_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_I3C1_CFG_ADDR,m,v,HWIO_SSC_SSC_I3C1_CFG_IN)
#define HWIO_SSC_SSC_I3C1_CFG_SDA_I3C1_CFG_MODE_BMSK                                   0x38
#define HWIO_SSC_SSC_I3C1_CFG_SDA_I3C1_CFG_MODE_SHFT                                    0x3
#define HWIO_SSC_SSC_I3C1_CFG_SCL_I3C1_CFG_MODE_BMSK                                    0x7
#define HWIO_SSC_SSC_I3C1_CFG_SCL_I3C1_CFG_MODE_SHFT                                    0x0

#define HWIO_SSC_SSC_IEC_CTL_ADDR                                                (SSC_MCC_REGS_REG_BASE      + 0x00002000)
#define HWIO_SSC_SSC_IEC_CTL_OFFS                                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002000)
#define HWIO_SSC_SSC_IEC_CTL_RMSK                                                      0x7f
#define HWIO_SSC_SSC_IEC_CTL_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_CTL_ADDR, HWIO_SSC_SSC_IEC_CTL_RMSK)
#define HWIO_SSC_SSC_IEC_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_CTL_ADDR, m)
#define HWIO_SSC_SSC_IEC_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_CTL_ADDR,v)
#define HWIO_SSC_SSC_IEC_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_CTL_ADDR,m,v,HWIO_SSC_SSC_IEC_CTL_IN)
#define HWIO_SSC_SSC_IEC_CTL_BUS_ISO_ENTER_ISLAND_DIS_BMSK                             0x40
#define HWIO_SSC_SSC_IEC_CTL_BUS_ISO_ENTER_ISLAND_DIS_SHFT                              0x6
#define HWIO_SSC_SSC_IEC_CTL_L2_MISS_ISLAND_EXIT_EN_BMSK                               0x20
#define HWIO_SSC_SSC_IEC_CTL_L2_MISS_ISLAND_EXIT_EN_SHFT                                0x5
#define HWIO_SSC_SSC_IEC_CTL_ISO_AHB_MASTER_BYPASS_BMSK                                0x10
#define HWIO_SSC_SSC_IEC_CTL_ISO_AHB_MASTER_BYPASS_SHFT                                 0x4
#define HWIO_SSC_SSC_IEC_CTL_ISO_AHB_SLAVE_BYPASS_BMSK                                  0x8
#define HWIO_SSC_SSC_IEC_CTL_ISO_AHB_SLAVE_BYPASS_SHFT                                  0x3
#define HWIO_SSC_SSC_IEC_CTL_AHB_REQPEND_CLR_BMSK                                       0x4
#define HWIO_SSC_SSC_IEC_CTL_AHB_REQPEND_CLR_SHFT                                       0x2
#define HWIO_SSC_SSC_IEC_CTL_ENTER_ISLAND_MODE_IRQ_CLR_BMSK                             0x2
#define HWIO_SSC_SSC_IEC_CTL_ENTER_ISLAND_MODE_IRQ_CLR_SHFT                             0x1
#define HWIO_SSC_SSC_IEC_CTL_EXIT_ISLAND_MODE_TRIG_BMSK                                 0x1
#define HWIO_SSC_SSC_IEC_CTL_EXIT_ISLAND_MODE_TRIG_SHFT                                 0x0

#define HWIO_SSC_SSC_IEC_STAT_ADDR                                               (SSC_MCC_REGS_REG_BASE      + 0x00002004)
#define HWIO_SSC_SSC_IEC_STAT_OFFS                                               (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002004)
#define HWIO_SSC_SSC_IEC_STAT_RMSK                                                      0x1
#define HWIO_SSC_SSC_IEC_STAT_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_STAT_ADDR, HWIO_SSC_SSC_IEC_STAT_RMSK)
#define HWIO_SSC_SSC_IEC_STAT_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_STAT_ADDR, m)
#define HWIO_SSC_SSC_IEC_STAT_ISLAND_MODE_BMSK                                          0x1
#define HWIO_SSC_SSC_IEC_STAT_ISLAND_MODE_SHFT                                          0x0

#define HWIO_SSC_SSC_IEC_BSY_WAIT_ADDR                                           (SSC_MCC_REGS_REG_BASE      + 0x00002008)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_OFFS                                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002008)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_RMSK                                                  0x3
#define HWIO_SSC_SSC_IEC_BSY_WAIT_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_BSY_WAIT_ADDR, HWIO_SSC_SSC_IEC_BSY_WAIT_RMSK)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_BSY_WAIT_ADDR, m)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_BSY_WAIT_ADDR,v)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_BSY_WAIT_ADDR,m,v,HWIO_SSC_SSC_IEC_BSY_WAIT_IN)
#define HWIO_SSC_SSC_IEC_BSY_WAIT_BUSY_WAIT_EN_BMSK                                     0x2
#define HWIO_SSC_SSC_IEC_BSY_WAIT_BUSY_WAIT_EN_SHFT                                     0x1
#define HWIO_SSC_SSC_IEC_BSY_WAIT_BUSY_WAIT_RST_BMSK                                    0x1
#define HWIO_SSC_SSC_IEC_BSY_WAIT_BUSY_WAIT_RST_SHFT                                    0x0

#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x0000200c)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000200c)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_RMSK                                        0xffffffff
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_SQCNTR_VAL0_ADDR, HWIO_SSC_SSC_IEC_SQCNTR_VAL0_RMSK)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_SQCNTR_VAL0_ADDR, m)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_SQCNTR_VAL0_BMSK                            0xffffffff
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL0_SQCNTR_VAL0_SHFT                                   0x0

#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x00002010)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002010)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_RMSK                                          0xffffff
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_SQCNTR_VAL1_ADDR, HWIO_SSC_SSC_IEC_SQCNTR_VAL1_RMSK)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_SQCNTR_VAL1_ADDR, m)
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_SQCNTR_VAL1_BMSK                              0xffffff
#define HWIO_SSC_SSC_IEC_SQCNTR_VAL1_SQCNTR_VAL1_SHFT                                   0x0

#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00002014)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002014)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_RMSK                                0xffffffff
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR, HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_RMSK)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_ADDR, m)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_BUSY_WAIT_CNTR_VAL0_BMSK            0xffffffff
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL0_BUSY_WAIT_CNTR_VAL0_SHFT                   0x0

#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00002018)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002018)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_RMSK                                  0xffffff
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR, HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_RMSK)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_ADDR, m)
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_BUSY_WAIT_CNTR_VAL1_BMSK              0xffffff
#define HWIO_SSC_SSC_IEC_BUSY_WAIT_CNTR_VAL1_BUSY_WAIT_CNTR_VAL1_SHFT                   0x0

#define HWIO_SSC_SSC_IEC_HADDR_VAL0_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x0000201c)
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000201c)
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_RMSK                                         0xffffffff
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HADDR_VAL0_ADDR, HWIO_SSC_SSC_IEC_HADDR_VAL0_RMSK)
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HADDR_VAL0_ADDR, m)
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_HADDR_VAL0_BMSK                              0xffffffff
#define HWIO_SSC_SSC_IEC_HADDR_VAL0_HADDR_VAL0_SHFT                                     0x0

#define HWIO_SSC_SSC_IEC_HADDR_VAL1_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x00002020)
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002020)
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_RMSK                                                0xf
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HADDR_VAL1_ADDR, HWIO_SSC_SSC_IEC_HADDR_VAL1_RMSK)
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HADDR_VAL1_ADDR, m)
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_HADDR_VAL1_BMSK                                     0xf
#define HWIO_SSC_SSC_IEC_HADDR_VAL1_HADDR_VAL1_SHFT                                     0x0

#define HWIO_SSC_SSC_IEC_HMID_VAL_ADDR                                           (SSC_MCC_REGS_REG_BASE      + 0x00002024)
#define HWIO_SSC_SSC_IEC_HMID_VAL_OFFS                                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002024)
#define HWIO_SSC_SSC_IEC_HMID_VAL_RMSK                                                 0xff
#define HWIO_SSC_SSC_IEC_HMID_VAL_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HMID_VAL_ADDR, HWIO_SSC_SSC_IEC_HMID_VAL_RMSK)
#define HWIO_SSC_SSC_IEC_HMID_VAL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HMID_VAL_ADDR, m)
#define HWIO_SSC_SSC_IEC_HMID_VAL_SSC_IEC_HMID_VAL_BMSK                                0xff
#define HWIO_SSC_SSC_IEC_HMID_VAL_SSC_IEC_HMID_VAL_SHFT                                 0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002028)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002028)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL0_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x0000202c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000202c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL1_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002030)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002030)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL2_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002034)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002034)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL3_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002038)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002038)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL4_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x0000203c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000203c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL5_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002040)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002040)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL6_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002044)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002044)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL7_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00002048)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002048)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL8_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x0000204c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000204c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_RMSK                                      0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_SSC_HWEVENTS_SEL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL9_SSC_HWEVENTS_SEL_SHFT                            0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002050)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002050)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL10_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002054)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002054)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL11_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002058)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002058)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL12_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000205c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000205c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL13_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002060)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002060)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL14_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002064)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002064)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL15_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002068)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002068)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL16_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000206c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000206c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL17_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002070)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002070)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL18_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002074)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002074)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL19_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002078)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002078)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL20_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000207c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000207c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL21_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002080)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002080)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL22_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002084)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002084)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL23_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002088)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002088)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL24_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000208c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000208c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL25_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002090)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002090)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL26_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002094)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002094)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL27_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002098)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002098)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL28_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000209c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000209c)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL29_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002100)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002100)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL30_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x00002104)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002104)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_RMSK                                     0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_SSC_HWEVENTS_SEL_BMSK                    0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_SEL31_SSC_HWEVENTS_SEL_SHFT                           0x0

#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x00002108)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x00002108)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_RMSK                                        0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_IN          \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_EN_ADDR, HWIO_SSC_SSC_IEC_HWEVENTS_EN_RMSK)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_IEC_HWEVENTS_EN_ADDR, m)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_OUT(v)      \
        out_dword(HWIO_SSC_SSC_IEC_HWEVENTS_EN_ADDR,v)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_IEC_HWEVENTS_EN_ADDR,m,v,HWIO_SSC_SSC_IEC_HWEVENTS_EN_IN)
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_HWEVENTS_EN_BMSK                            0xffffffff
#define HWIO_SSC_SSC_IEC_HWEVENTS_EN_HWEVENTS_EN_SHFT                                   0x0

#define HWIO_SSC_SSC_DATA_BUS_CONFIG_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x00003000)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x00003000)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_RMSK                                             0xfff
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_IN          \
        in_dword_masked(HWIO_SSC_SSC_DATA_BUS_CONFIG_ADDR, HWIO_SSC_SSC_DATA_BUS_CONFIG_RMSK)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_DATA_BUS_CONFIG_ADDR, m)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SSC_SSC_DATA_BUS_CONFIG_ADDR,v)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_DATA_BUS_CONFIG_ADDR,m,v,HWIO_SSC_SSC_DATA_BUS_CONFIG_IN)
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_AHBE_GLOBAL_EN_BMSK                              0x800
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_AHBE_GLOBAL_EN_SHFT                                0xb
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_AHBE_TIMEBASE_BMSK                               0x400
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_AHBE_TIMEBASE_SHFT                                 0xa
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_LOCK_EN_ARRAY_BMSK                               0x380
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_LOCK_EN_ARRAY_SHFT                                 0x7
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_PRIORITY_ARRAY_BMSK                               0x7e
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_PRIORITY_ARRAY_SHFT                                0x1
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_ARB_SCHEME_SEL_BMSK                                0x1
#define HWIO_SSC_SSC_DATA_BUS_CONFIG_ARB_SCHEME_SEL_SHFT                                0x0

#define HWIO_SSC_SSC_DATA_BUS_STATUS_ADDR                                        (SSC_MCC_REGS_REG_BASE      + 0x00003004)
#define HWIO_SSC_SSC_DATA_BUS_STATUS_OFFS                                        (SSC_MCC_REGS_REG_BASE_OFFS + 0x00003004)
#define HWIO_SSC_SSC_DATA_BUS_STATUS_RMSK                                               0x7
#define HWIO_SSC_SSC_DATA_BUS_STATUS_IN          \
        in_dword_masked(HWIO_SSC_SSC_DATA_BUS_STATUS_ADDR, HWIO_SSC_SSC_DATA_BUS_STATUS_RMSK)
#define HWIO_SSC_SSC_DATA_BUS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_DATA_BUS_STATUS_ADDR, m)
#define HWIO_SSC_SSC_DATA_BUS_STATUS_AHBE_STATUS_BMSK                                   0x7
#define HWIO_SSC_SSC_DATA_BUS_STATUS_AHBE_STATUS_SHFT                                   0x0

#define HWIO_SSC_SSC_CFG_BUS_CONFIG_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x00004000)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x00004000)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_RMSK                                              0xfff
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_IN          \
        in_dword_masked(HWIO_SSC_SSC_CFG_BUS_CONFIG_ADDR, HWIO_SSC_SSC_CFG_BUS_CONFIG_RMSK)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_CFG_BUS_CONFIG_ADDR, m)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SSC_SSC_CFG_BUS_CONFIG_ADDR,v)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_CFG_BUS_CONFIG_ADDR,m,v,HWIO_SSC_SSC_CFG_BUS_CONFIG_IN)
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_AHBE_GLOBAL_EN_BMSK                               0x800
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_AHBE_GLOBAL_EN_SHFT                                 0xb
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_AHBE_TIMEBASE_BMSK                                0x400
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_AHBE_TIMEBASE_SHFT                                  0xa
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_LOCK_EN_ARRAY_BMSK                                0x380
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_LOCK_EN_ARRAY_SHFT                                  0x7
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_PRIORITY_ARRAY_BMSK                                0x7e
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_PRIORITY_ARRAY_SHFT                                 0x1
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_ARB_SCHEME_SEL_BMSK                                 0x1
#define HWIO_SSC_SSC_CFG_BUS_CONFIG_ARB_SCHEME_SEL_SHFT                                 0x0

#define HWIO_SSC_SSC_CFG_BUS_STATUS_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x00004004)
#define HWIO_SSC_SSC_CFG_BUS_STATUS_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x00004004)
#define HWIO_SSC_SSC_CFG_BUS_STATUS_RMSK                                                0x7
#define HWIO_SSC_SSC_CFG_BUS_STATUS_IN          \
        in_dword_masked(HWIO_SSC_SSC_CFG_BUS_STATUS_ADDR, HWIO_SSC_SSC_CFG_BUS_STATUS_RMSK)
#define HWIO_SSC_SSC_CFG_BUS_STATUS_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_CFG_BUS_STATUS_ADDR, m)
#define HWIO_SSC_SSC_CFG_BUS_STATUS_AHBE_STATUS_BMSK                                    0x7
#define HWIO_SSC_SSC_CFG_BUS_STATUS_AHBE_STATUS_SHFT                                    0x0

#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR                                       (SSC_MCC_REGS_REG_BASE      + 0x00005000)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_OFFS                                       (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005000)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_RMSK                                       0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_IN          \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR, HWIO_SSC_SSC_TSGEN_PRELOAD_LO_RMSK)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR, m)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_OUT(v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR,v)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_PRELOAD_LO_ADDR,m,v,HWIO_SSC_SSC_TSGEN_PRELOAD_LO_IN)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_BMSK                      0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_LO_TSGEN_PRELOAD_LO_SHFT                             0x0

#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR                                       (SSC_MCC_REGS_REG_BASE      + 0x00005004)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_OFFS                                       (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005004)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_RMSK                                       0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_IN          \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR, HWIO_SSC_SSC_TSGEN_PRELOAD_HI_RMSK)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR, m)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_OUT(v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR,v)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_PRELOAD_HI_ADDR,m,v,HWIO_SSC_SSC_TSGEN_PRELOAD_HI_IN)
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_BMSK                      0xffffffff
#define HWIO_SSC_SSC_TSGEN_PRELOAD_HI_TSGEN_PRELOAD_HI_SHFT                             0x0

#define HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR                                           (SSC_MCC_REGS_REG_BASE      + 0x00005008)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_OFFS                                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005008)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_RMSK                                           0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_LO_IN          \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR, HWIO_SSC_SSC_TSGEN_CTR_LO_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_LO_ADDR, m)
#define HWIO_SSC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_BMSK                              0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_LO_TSGEN_CTR_LO_SHFT                                     0x0

#define HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR                                           (SSC_MCC_REGS_REG_BASE      + 0x0000500c)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_OFFS                                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000500c)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_RMSK                                           0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_HI_IN          \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR, HWIO_SSC_SSC_TSGEN_CTR_HI_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTR_HI_ADDR, m)
#define HWIO_SSC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_BMSK                              0xffffffff
#define HWIO_SSC_SSC_TSGEN_CTR_HI_TSGEN_CTR_HI_SHFT                                     0x0

#define HWIO_SSC_SSC_TSGEN_CTL_ADDR                                              (SSC_MCC_REGS_REG_BASE      + 0x00005010)
#define HWIO_SSC_SSC_TSGEN_CTL_OFFS                                              (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005010)
#define HWIO_SSC_SSC_TSGEN_CTL_RMSK                                                     0x3
#define HWIO_SSC_SSC_TSGEN_CTL_IN          \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTL_ADDR, HWIO_SSC_SSC_TSGEN_CTL_RMSK)
#define HWIO_SSC_SSC_TSGEN_CTL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_TSGEN_CTL_ADDR, m)
#define HWIO_SSC_SSC_TSGEN_CTL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_TSGEN_CTL_ADDR,v)
#define HWIO_SSC_SSC_TSGEN_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_TSGEN_CTL_ADDR,m,v,HWIO_SSC_SSC_TSGEN_CTL_IN)
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_BMSK                                    0x2
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_PRELOAD_EN_SHFT                                    0x1
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_BMSK                                   0x1
#define HWIO_SSC_SSC_TSGEN_CTL_TSGEN_DISABLE_CNT_SHFT                                   0x0

#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00005014)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005014)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR, HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_ATB_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00005018)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005018)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR, HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_APB_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x0000501c)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000501c)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR, HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_DAP_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR                                (SSC_MCC_REGS_REG_BASE      + 0x00005020)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_OFFS                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x00005020)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK                                       0x7
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_IN          \
        in_dword_masked(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR, HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_RMSK)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR, m)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,v)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ADDR,m,v,HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_IN)
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_BMSK                               0x4
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_REQUEST_SHFT                               0x2
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_BMSK                                0x2
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACTIVE_SHFT                                0x1
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_BMSK                                   0x1
#define HWIO_SSC_SSC_NTS_LOW_POWER_HANDSHAKE_ACK_SHFT                                   0x0

#define HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x00006000)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x00006000)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_RMSK                                             0xffff
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_IN          \
        in_dword_masked(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR, HWIO_SSC_SSC_HM_TESTBUS_SEL_RMSK)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR, m)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR,v)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_HM_TESTBUS_SEL_ADDR,m,v,HWIO_SSC_SSC_HM_TESTBUS_SEL_IN)
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_BMSK                               0xffff
#define HWIO_SSC_SSC_HM_TESTBUS_SEL_I_TESTBUS_SEL_SHFT                                  0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR                          (SSC_MCC_REGS_REG_BASE      + 0x00007000)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OFFS                          (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007000)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK                          0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_ADDR,m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_IN)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_BMSK                      0xfffffffc
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_RFU_SHFT                             0x2
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_BMSK                 0x2
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_TIMESTAMPENABLE_SHFT                 0x1
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_BMSK                         0x1
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_TIMESTAMPCTRL_CAPTURE_SHFT                         0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR                             (SSC_MCC_REGS_REG_BASE      + 0x00007004)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_OFFS                             (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007004)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK                             0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL0_TIMEVAL_SHFT                            0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR                             (SSC_MCC_REGS_REG_BASE      + 0x00007008)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_OFFS                             (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007008)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK                             0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_BMSK                     0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_AOTIMEVAL1_TIMEVAL_SHFT                            0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR                           (SSC_MCC_REGS_REG_BASE      + 0x0000700c)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_OFFS                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000700c)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK                           0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_BMSK                   0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL0_TIMEVAL_SHFT                          0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR                           (SSC_MCC_REGS_REG_BASE      + 0x00007010)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_OFFS                           (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007010)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK                           0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_BMSK                   0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMEVAL1_TIMEVAL_SHFT                          0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR                          (SSC_MCC_REGS_REG_BASE      + 0x00007014)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OFFS                          (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007014)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK                          0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUT(v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_ADDR,m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_IN)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_BMSK                 0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD0_TIMELOAD_SHFT                        0x0

#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR                          (SSC_MCC_REGS_REG_BASE      + 0x00007018)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OFFS                          (SSC_MCC_REGS_REG_BASE_OFFS + 0x00007018)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK                          0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN          \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_RMSK)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR, m)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUT(v)      \
        out_dword(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,v)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_ADDR,m,v,HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_IN)
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_BMSK                 0xffffffff
#define HWIO_SSC_SSC_QDSS_CS_QDSSCSR_QDSSTIMELOAD1_TIMELOAD_SHFT                        0x0

#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x00008000)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x00008000)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_RMSK                                           0x1ff
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_IN          \
        in_dword_masked(HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_ADDR, HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_RMSK)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_ADDR, m)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_ADDR,v)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_ADDR,m,v,HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_IN)
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_WAKEUP_IE_BMSK                                 0x1ff
#define HWIO_SSC_SSC_WAKEUP_PRIMARY_IE_WAKEUP_IE_SHFT                                   0x0

#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_ADDR                                    (SSC_MCC_REGS_REG_BASE      + 0x00009000)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_OFFS                                    (SSC_MCC_REGS_REG_BASE_OFFS + 0x00009000)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_RMSK                                         0x1ff
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_IN          \
        in_dword_masked(HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_ADDR, HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_RMSK)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_ADDR, m)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_OUT(v)      \
        out_dword(HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_ADDR,v)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_ADDR,m,v,HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_IN)
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_WAKEUP_IE_BMSK                               0x1ff
#define HWIO_SSC_SSC_WAKEUP_SECONDARY_IE_WAKEUP_IE_SHFT                                 0x0

#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR                                     (SSC_MCC_REGS_REG_BASE      + 0x0000b000)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_OFFS                                     (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000b000)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_RMSK                                            0x1
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_IN          \
        in_dword_masked(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR, HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_RMSK)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR, m)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_OUT(v)      \
        out_dword(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR,v)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_ADDR,m,v,HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_IN)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_SEL_BMSK                                        0x1
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_SEL_SEL_SHFT                                        0x0

#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR                                    (SSC_MCC_REGS_REG_BASE      + 0x0000b004)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_OFFS                                    (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000b004)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_RMSK                                     0xfffffff
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_IN          \
        in_dword_masked(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR, HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_RMSK)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR, m)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_OUT(v)      \
        out_dword(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR,v)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR,m,v,HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_IN)
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR_BMSK                                0xffffff0
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_ADDR_SHFT                                      0x4
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_RSVD_BIT3_0_BMSK                               0xf
#define HWIO_SSC_SSC_EFUSE_Q6SS_EVB_ADDR_RSVD_BIT3_0_SHFT                               0x0

#define HWIO_SSC_SSC_ALT_RESET_Q6SS_ADDR                                         (SSC_MCC_REGS_REG_BASE      + 0x0000b008)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_OFFS                                         (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000b008)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_RMSK                                                0x1
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_IN          \
        in_dword_masked(HWIO_SSC_SSC_ALT_RESET_Q6SS_ADDR, HWIO_SSC_SSC_ALT_RESET_Q6SS_RMSK)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_ALT_RESET_Q6SS_ADDR, m)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_OUT(v)      \
        out_dword(HWIO_SSC_SSC_ALT_RESET_Q6SS_ADDR,v)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_ALT_RESET_Q6SS_ADDR,m,v,HWIO_SSC_SSC_ALT_RESET_Q6SS_IN)
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_SSC_ALT_ARES_ENABLE_BMSK                            0x1
#define HWIO_SSC_SSC_ALT_RESET_Q6SS_SSC_ALT_ARES_ENABLE_SHFT                            0x0

#define HWIO_SSC_SSC_SPARE_1_ADDR                                                (SSC_MCC_REGS_REG_BASE      + 0x0000c000)
#define HWIO_SSC_SSC_SPARE_1_OFFS                                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000c000)
#define HWIO_SSC_SSC_SPARE_1_RMSK                                                0xffffffff
#define HWIO_SSC_SSC_SPARE_1_IN          \
        in_dword_masked(HWIO_SSC_SSC_SPARE_1_ADDR, HWIO_SSC_SSC_SPARE_1_RMSK)
#define HWIO_SSC_SSC_SPARE_1_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_SPARE_1_ADDR, m)
#define HWIO_SSC_SSC_SPARE_1_OUT(v)      \
        out_dword(HWIO_SSC_SSC_SPARE_1_ADDR,v)
#define HWIO_SSC_SSC_SPARE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_SPARE_1_ADDR,m,v,HWIO_SSC_SSC_SPARE_1_IN)
#define HWIO_SSC_SSC_SPARE_1_SPARE_IN_BMSK                                       0xffff0000
#define HWIO_SSC_SSC_SPARE_1_SPARE_IN_SHFT                                             0x10
#define HWIO_SSC_SSC_SPARE_1_SPARE_OUT_BMSK                                          0xffff
#define HWIO_SSC_SSC_SPARE_1_SPARE_OUT_SHFT                                             0x0

#define HWIO_SSC_SSC_SPARE_2_ADDR                                                (SSC_MCC_REGS_REG_BASE      + 0x0000d000)
#define HWIO_SSC_SSC_SPARE_2_OFFS                                                (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000d000)
#define HWIO_SSC_SSC_SPARE_2_RMSK                                                0xffffffff
#define HWIO_SSC_SSC_SPARE_2_IN          \
        in_dword_masked(HWIO_SSC_SSC_SPARE_2_ADDR, HWIO_SSC_SSC_SPARE_2_RMSK)
#define HWIO_SSC_SSC_SPARE_2_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_SPARE_2_ADDR, m)
#define HWIO_SSC_SSC_SPARE_2_OUT(v)      \
        out_dword(HWIO_SSC_SSC_SPARE_2_ADDR,v)
#define HWIO_SSC_SSC_SPARE_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_SPARE_2_ADDR,m,v,HWIO_SSC_SSC_SPARE_2_IN)
#define HWIO_SSC_SSC_SPARE_2_SPARE_IN_BMSK                                       0xffff0000
#define HWIO_SSC_SSC_SPARE_2_SPARE_IN_SHFT                                             0x10
#define HWIO_SSC_SSC_SPARE_2_SPARE_OUT_BMSK                                          0xffff
#define HWIO_SSC_SSC_SPARE_2_SPARE_OUT_SHFT                                             0x0

#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_ADDR                                      (SSC_MCC_REGS_REG_BASE      + 0x0000e000)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OFFS                                      (SSC_MCC_REGS_REG_BASE_OFFS + 0x0000e000)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_RMSK                                             0xf
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_IN          \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_ADDR, HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_RMSK)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_INM(m)      \
        in_dword_masked(HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_ADDR, m)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OUT(v)      \
        out_dword(HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_ADDR,v)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_ADDR,m,v,HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_IN)
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OP_MODE_OVRD_BMSK                                0x8
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OP_MODE_OVRD_SHFT                                0x3
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OPMODE_OVRD_VAL_BMSK                             0x4
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_OPMODE_OVRD_VAL_SHFT                             0x2
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_BMSK                                 0x2
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_SHFT                                 0x1
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_VAL_BMSK                             0x1
#define HWIO_SSC_SSC_RSC_PWR_CTRL_OVRD_CBC_EN_OVRD_VAL_SHFT                             0x0


#endif /* __HALCLKHWIOASM_H__ */
