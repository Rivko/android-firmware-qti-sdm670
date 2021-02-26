#ifndef __CPR_HWIO_H__
#define __CPR_HWIO_H__
/*
===========================================================================
*/
/**
  @file cpr_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r93]
 
  This file contains HWIO register definitions for the following modules:
    CX_CPR3

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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/CPRTargetLib/target/670/hwio/cpr_hwio.h#1 $
  $DateTime: 2017/09/18 09:26:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"
#include "msmhwio.h"

/*----------------------------------------------------------------------------
 * MODULE: CX_CPR3
 *--------------------------------------------------------------------------*/

#define CX_CPR3_REG_BASE                                                             (CX_CPR3_BASE      + 0x00000000)
#define CX_CPR3_REG_BASE_PHYS                                                        (CX_CPR3_BASE_PHYS + 0x00000000)
#define CX_CPR3_REG_BASE_OFFS                                                        0x00000000

#define HWIO_CPR_VERSION_ADDR(x)                                                     ((x) + 0x00000000)
#define HWIO_CPR_VERSION_PHYS(x)                                                     ((x) + 0x00000000)
#define HWIO_CPR_VERSION_OFFS                                                        (0x00000000)
#define HWIO_CPR_VERSION_RMSK                                                        0xffffffff
#define HWIO_CPR_VERSION_IN(x)      \
        in_dword_masked(HWIO_CPR_VERSION_ADDR(x), HWIO_CPR_VERSION_RMSK)
#define HWIO_CPR_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_CPR_VERSION_ADDR(x), m)
#define HWIO_CPR_VERSION_CPR_VERSION_BMSK                                            0xffffffff
#define HWIO_CPR_VERSION_CPR_VERSION_SHFT                                                   0x0

#define HWIO_CPR_FSM_CTL_ADDR(x)                                                     ((x) + 0x00000004)
#define HWIO_CPR_FSM_CTL_PHYS(x)                                                     ((x) + 0x00000004)
#define HWIO_CPR_FSM_CTL_OFFS                                                        (0x00000004)
#define HWIO_CPR_FSM_CTL_RMSK                                                        0xffffffff
#define HWIO_CPR_FSM_CTL_IN(x)      \
        in_dword_masked(HWIO_CPR_FSM_CTL_ADDR(x), HWIO_CPR_FSM_CTL_RMSK)
#define HWIO_CPR_FSM_CTL_INM(x, m)      \
        in_dword_masked(HWIO_CPR_FSM_CTL_ADDR(x), m)
#define HWIO_CPR_FSM_CTL_OUT(x, v)      \
        out_dword(HWIO_CPR_FSM_CTL_ADDR(x),v)
#define HWIO_CPR_FSM_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_FSM_CTL_ADDR(x),m,v,HWIO_CPR_FSM_CTL_IN(x))
#define HWIO_CPR_FSM_CTL_COUNT_REPEAT_BMSK                                           0xfffffe00
#define HWIO_CPR_FSM_CTL_COUNT_REPEAT_SHFT                                                  0x9
#define HWIO_CPR_FSM_CTL_SET_HTOL_MODE_BMSK                                               0x100
#define HWIO_CPR_FSM_CTL_SET_HTOL_MODE_SHFT                                                 0x8
#define HWIO_CPR_FSM_CTL_COUNT_MODE_BMSK                                                   0xc0
#define HWIO_CPR_FSM_CTL_COUNT_MODE_SHFT                                                    0x6
#define HWIO_CPR_FSM_CTL_IDLE_CLOCKS_BMSK                                                  0x3e
#define HWIO_CPR_FSM_CTL_IDLE_CLOCKS_SHFT                                                   0x1
#define HWIO_CPR_FSM_CTL_LOOP_EN_BMSK                                                       0x1
#define HWIO_CPR_FSM_CTL_LOOP_EN_SHFT                                                       0x0

#define HWIO_CPR_FSM_STA_ADDR(x)                                                     ((x) + 0x00000008)
#define HWIO_CPR_FSM_STA_PHYS(x)                                                     ((x) + 0x00000008)
#define HWIO_CPR_FSM_STA_OFFS                                                        (0x00000008)
#define HWIO_CPR_FSM_STA_RMSK                                                               0x3
#define HWIO_CPR_FSM_STA_IN(x)      \
        in_dword_masked(HWIO_CPR_FSM_STA_ADDR(x), HWIO_CPR_FSM_STA_RMSK)
#define HWIO_CPR_FSM_STA_INM(x, m)      \
        in_dword_masked(HWIO_CPR_FSM_STA_ADDR(x), m)
#define HWIO_CPR_FSM_STA_PAGE_IS_AGE_BMSK                                                   0x2
#define HWIO_CPR_FSM_STA_PAGE_IS_AGE_SHFT                                                   0x1
#define HWIO_CPR_FSM_STA_BUSY_BMSK                                                          0x1
#define HWIO_CPR_FSM_STA_BUSY_SHFT                                                          0x0

#define HWIO_CPR_TIMER_AUTO_CONT_ADDR(x)                                             ((x) + 0x0000000c)
#define HWIO_CPR_TIMER_AUTO_CONT_PHYS(x)                                             ((x) + 0x0000000c)
#define HWIO_CPR_TIMER_AUTO_CONT_OFFS                                                (0x0000000c)
#define HWIO_CPR_TIMER_AUTO_CONT_RMSK                                                0xffffffff
#define HWIO_CPR_TIMER_AUTO_CONT_IN(x)      \
        in_dword_masked(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x), HWIO_CPR_TIMER_AUTO_CONT_RMSK)
#define HWIO_CPR_TIMER_AUTO_CONT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x), m)
#define HWIO_CPR_TIMER_AUTO_CONT_OUT(x, v)      \
        out_dword(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x),v)
#define HWIO_CPR_TIMER_AUTO_CONT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_TIMER_AUTO_CONT_ADDR(x),m,v,HWIO_CPR_TIMER_AUTO_CONT_IN(x))
#define HWIO_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_BMSK                             0xffffffff
#define HWIO_CPR_TIMER_AUTO_CONT_AUTO_CONT_INTERVAL_SHFT                                    0x0

#define HWIO_CPR_TIMER_CLAMP_ADDR(x)                                                 ((x) + 0x00000010)
#define HWIO_CPR_TIMER_CLAMP_PHYS(x)                                                 ((x) + 0x00000010)
#define HWIO_CPR_TIMER_CLAMP_OFFS                                                    (0x00000010)
#define HWIO_CPR_TIMER_CLAMP_RMSK                                                    0xf80000ff
#define HWIO_CPR_TIMER_CLAMP_IN(x)      \
        in_dword_masked(HWIO_CPR_TIMER_CLAMP_ADDR(x), HWIO_CPR_TIMER_CLAMP_RMSK)
#define HWIO_CPR_TIMER_CLAMP_INM(x, m)      \
        in_dword_masked(HWIO_CPR_TIMER_CLAMP_ADDR(x), m)
#define HWIO_CPR_TIMER_CLAMP_OUT(x, v)      \
        out_dword(HWIO_CPR_TIMER_CLAMP_ADDR(x),v)
#define HWIO_CPR_TIMER_CLAMP_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_TIMER_CLAMP_ADDR(x),m,v,HWIO_CPR_TIMER_CLAMP_IN(x))
#define HWIO_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_BMSK                                  0x80000000
#define HWIO_CPR_TIMER_CLAMP_CPR_DISABLE_VALID_SHFT                                        0x1f
#define HWIO_CPR_TIMER_CLAMP_GDHS_RESTART_ENABLE_BMSK                                0x40000000
#define HWIO_CPR_TIMER_CLAMP_GDHS_RESTART_ENABLE_SHFT                                      0x1e
#define HWIO_CPR_TIMER_CLAMP_BHS_ON_RESTART_ENABLE_BMSK                              0x20000000
#define HWIO_CPR_TIMER_CLAMP_BHS_ON_RESTART_ENABLE_SHFT                                    0x1d
#define HWIO_CPR_TIMER_CLAMP_CLEAR_IRQ_WITH_CONT_EN_BMSK                             0x10000000
#define HWIO_CPR_TIMER_CLAMP_CLEAR_IRQ_WITH_CONT_EN_SHFT                                   0x1c
#define HWIO_CPR_TIMER_CLAMP_THREAD_AGGREGATION_EN_BMSK                               0x8000000
#define HWIO_CPR_TIMER_CLAMP_THREAD_AGGREGATION_EN_SHFT                                    0x1b
#define HWIO_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_BMSK                                     0xff
#define HWIO_CPR_TIMER_CLAMP_CLAMP_TIMER_INTERVAL_SHFT                                      0x0

#define HWIO_CPR_STEP_QUOT_INIT_ADDR(x)                                              ((x) + 0x00000014)
#define HWIO_CPR_STEP_QUOT_INIT_PHYS(x)                                              ((x) + 0x00000014)
#define HWIO_CPR_STEP_QUOT_INIT_OFFS                                                 (0x00000014)
#define HWIO_CPR_STEP_QUOT_INIT_RMSK                                                      0xfff
#define HWIO_CPR_STEP_QUOT_INIT_IN(x)      \
        in_dword_masked(HWIO_CPR_STEP_QUOT_INIT_ADDR(x), HWIO_CPR_STEP_QUOT_INIT_RMSK)
#define HWIO_CPR_STEP_QUOT_INIT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_STEP_QUOT_INIT_ADDR(x), m)
#define HWIO_CPR_STEP_QUOT_INIT_OUT(x, v)      \
        out_dword(HWIO_CPR_STEP_QUOT_INIT_ADDR(x),v)
#define HWIO_CPR_STEP_QUOT_INIT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_STEP_QUOT_INIT_ADDR(x),m,v,HWIO_CPR_STEP_QUOT_INIT_IN(x))
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_BMSK                                   0xfc0
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MAX_SHFT                                     0x6
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_BMSK                                    0x3f
#define HWIO_CPR_STEP_QUOT_INIT_STEP_QUOT_INIT_MIN_SHFT                                     0x0

#define HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x)                                           ((x) + 0x00000020)
#define HWIO_CPR_BIST_CHAIN_CHECK0_PHYS(x)                                           ((x) + 0x00000020)
#define HWIO_CPR_BIST_CHAIN_CHECK0_OFFS                                              (0x00000020)
#define HWIO_CPR_BIST_CHAIN_CHECK0_RMSK                                              0x80000fff
#define HWIO_CPR_BIST_CHAIN_CHECK0_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x), HWIO_CPR_BIST_CHAIN_CHECK0_RMSK)
#define HWIO_CPR_BIST_CHAIN_CHECK0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x), m)
#define HWIO_CPR_BIST_CHAIN_CHECK0_OUT(x, v)      \
        out_dword(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x),v)
#define HWIO_CPR_BIST_CHAIN_CHECK0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_BIST_CHAIN_CHECK0_ADDR(x),m,v,HWIO_CPR_BIST_CHAIN_CHECK0_IN(x))
#define HWIO_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_BMSK                               0x80000000
#define HWIO_CPR_BIST_CHAIN_CHECK0_EN_CHAIN_CHECK_SHFT                                     0x1f
#define HWIO_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_BMSK                                 0xfff
#define HWIO_CPR_BIST_CHAIN_CHECK0_SCLK_CNT_EXPECTED_SHFT                                   0x0

#define HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x)                                           ((x) + 0x00000024)
#define HWIO_CPR_BIST_CHAIN_CHECK1_PHYS(x)                                           ((x) + 0x00000024)
#define HWIO_CPR_BIST_CHAIN_CHECK1_OFFS                                              (0x00000024)
#define HWIO_CPR_BIST_CHAIN_CHECK1_RMSK                                              0xe0ffffff
#define HWIO_CPR_BIST_CHAIN_CHECK1_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x), HWIO_CPR_BIST_CHAIN_CHECK1_RMSK)
#define HWIO_CPR_BIST_CHAIN_CHECK1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_CHAIN_CHECK1_ADDR(x), m)
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_BMSK                                0x80000000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_DONE_SHFT                                      0x1f
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_BMSK                                0x60000000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT_FAIL_SHFT                                      0x1d
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_BMSK                                      0xfff000
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT1_SHFT                                           0xc
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_BMSK                                         0xfff
#define HWIO_CPR_BIST_CHAIN_CHECK1_SCLK_CNT0_SHFT                                           0x0

#define HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x)                                            ((x) + 0x00000028)
#define HWIO_CPR_BIST_QUOT_CHECK0_PHYS(x)                                            ((x) + 0x00000028)
#define HWIO_CPR_BIST_QUOT_CHECK0_OFFS                                               (0x00000028)
#define HWIO_CPR_BIST_QUOT_CHECK0_RMSK                                               0x80ffffff
#define HWIO_CPR_BIST_QUOT_CHECK0_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x), HWIO_CPR_BIST_QUOT_CHECK0_RMSK)
#define HWIO_CPR_BIST_QUOT_CHECK0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x), m)
#define HWIO_CPR_BIST_QUOT_CHECK0_OUT(x, v)      \
        out_dword(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x),v)
#define HWIO_CPR_BIST_QUOT_CHECK0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_BIST_QUOT_CHECK0_ADDR(x),m,v,HWIO_CPR_BIST_QUOT_CHECK0_IN(x))
#define HWIO_CPR_BIST_QUOT_CHECK0_FAIL_BMSK                                          0x80000000
#define HWIO_CPR_BIST_QUOT_CHECK0_FAIL_SHFT                                                0x1f
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_BMSK                                     0xfff000
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMAX_SHFT                                          0xc
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_BMSK                                        0xfff
#define HWIO_CPR_BIST_QUOT_CHECK0_QUOT_ABSMIN_SHFT                                          0x0

#define HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x)                                            ((x) + 0x0000002c)
#define HWIO_CPR_BIST_QUOT_CHECK1_PHYS(x)                                            ((x) + 0x0000002c)
#define HWIO_CPR_BIST_QUOT_CHECK1_OFFS                                               (0x0000002c)
#define HWIO_CPR_BIST_QUOT_CHECK1_RMSK                                               0xffffffff
#define HWIO_CPR_BIST_QUOT_CHECK1_IN(x)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x), HWIO_CPR_BIST_QUOT_CHECK1_RMSK)
#define HWIO_CPR_BIST_QUOT_CHECK1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_BIST_QUOT_CHECK1_ADDR(x), m)
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_BMSK                                  0xf0000000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MAX_SHFT                                        0x1c
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_BMSK                                   0xf000000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_SEL_MIN_SHFT                                        0x18
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_BMSK                                   0xfff000
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MAX_SHFT                                        0xc
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_BMSK                                      0xfff
#define HWIO_CPR_BIST_QUOT_CHECK1_FAIL_QUOT_MIN_SHFT                                        0x0

#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n)                                    ((base) + 0x00000030 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_PHYS(base,n)                                    ((base) + 0x00000030 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_OFFS(base,n)                                    (0x00000030 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_RMSK                                            0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_MAXn                                                     1
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n), HWIO_CPR_BIST_FAIL_MAP_MIN_n_RMSK)
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MIN_n_ADDR(base,n), mask)
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_BMSK                          0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MIN_n_BIST_FAIL_MAP_MIN_SHFT                                 0x0

#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n)                                    ((base) + 0x00000050 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_PHYS(base,n)                                    ((base) + 0x00000050 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_OFFS(base,n)                                    (0x00000050 + 0x4 * (n))
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_RMSK                                            0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_MAXn                                                     1
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n), HWIO_CPR_BIST_FAIL_MAP_MAX_n_RMSK)
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_BIST_FAIL_MAP_MAX_n_ADDR(base,n), mask)
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_BMSK                          0xffffffff
#define HWIO_CPR_BIST_FAIL_MAP_MAX_n_BIST_FAIL_MAP_MAX_SHFT                                 0x0

#define HWIO_CPR_DEBUG_ADDR(x)                                                       ((x) + 0x00000070)
#define HWIO_CPR_DEBUG_PHYS(x)                                                       ((x) + 0x00000070)
#define HWIO_CPR_DEBUG_OFFS                                                          (0x00000070)
#define HWIO_CPR_DEBUG_RMSK                                                               0xfff
#define HWIO_CPR_DEBUG_IN(x)      \
        in_dword_masked(HWIO_CPR_DEBUG_ADDR(x), HWIO_CPR_DEBUG_RMSK)
#define HWIO_CPR_DEBUG_INM(x, m)      \
        in_dword_masked(HWIO_CPR_DEBUG_ADDR(x), m)
#define HWIO_CPR_DEBUG_OUT(x, v)      \
        out_dword(HWIO_CPR_DEBUG_ADDR(x),v)
#define HWIO_CPR_DEBUG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_DEBUG_ADDR(x),m,v,HWIO_CPR_DEBUG_IN(x))
#define HWIO_CPR_DEBUG_DEBUG_RO_SEL_BMSK                                                  0xf00
#define HWIO_CPR_DEBUG_DEBUG_RO_SEL_SHFT                                                    0x8
#define HWIO_CPR_DEBUG_CPR_LOG_THREAD_SEL_BMSK                                             0xe0
#define HWIO_CPR_DEBUG_CPR_LOG_THREAD_SEL_SHFT                                              0x5
#define HWIO_CPR_DEBUG_DEBUG_BUS_SEL_BMSK                                                  0x1f
#define HWIO_CPR_DEBUG_DEBUG_BUS_SEL_SHFT                                                   0x0

#define HWIO_CPR_DEBUG_LOG_n_ADDR(base,n)                                            ((base) + 0x00000080 + 0x4 * (n))
#define HWIO_CPR_DEBUG_LOG_n_PHYS(base,n)                                            ((base) + 0x00000080 + 0x4 * (n))
#define HWIO_CPR_DEBUG_LOG_n_OFFS(base,n)                                            (0x00000080 + 0x4 * (n))
#define HWIO_CPR_DEBUG_LOG_n_RMSK                                                    0x3fff000e
#define HWIO_CPR_DEBUG_LOG_n_MAXn                                                             7
#define HWIO_CPR_DEBUG_LOG_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_DEBUG_LOG_n_ADDR(base,n), HWIO_CPR_DEBUG_LOG_n_RMSK)
#define HWIO_CPR_DEBUG_LOG_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_DEBUG_LOG_n_ADDR(base,n), mask)
#define HWIO_CPR_DEBUG_LOG_n_MID_COUNT_BMSK                                          0x3fff0000
#define HWIO_CPR_DEBUG_LOG_n_MID_COUNT_SHFT                                                0x10
#define HWIO_CPR_DEBUG_LOG_n_UP_FLAG_BMSK                                                   0x8
#define HWIO_CPR_DEBUG_LOG_n_UP_FLAG_SHFT                                                   0x3
#define HWIO_CPR_DEBUG_LOG_n_MID_FLAG_BMSK                                                  0x4
#define HWIO_CPR_DEBUG_LOG_n_MID_FLAG_SHFT                                                  0x2
#define HWIO_CPR_DEBUG_LOG_n_DOWN_FLAG_BMSK                                                 0x2
#define HWIO_CPR_DEBUG_LOG_n_DOWN_FLAG_SHFT                                                 0x1

#define HWIO_CPR_GCNT0_ADDR(x)                                                       ((x) + 0x000000a0)
#define HWIO_CPR_GCNT0_PHYS(x)                                                       ((x) + 0x000000a0)
#define HWIO_CPR_GCNT0_OFFS                                                          (0x000000a0)
#define HWIO_CPR_GCNT0_RMSK                                                              0xffff
#define HWIO_CPR_GCNT0_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT0_ADDR(x), HWIO_CPR_GCNT0_RMSK)
#define HWIO_CPR_GCNT0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT0_ADDR(x), m)
#define HWIO_CPR_GCNT0_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT0_ADDR(x),v)
#define HWIO_CPR_GCNT0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT0_ADDR(x),m,v,HWIO_CPR_GCNT0_IN(x))
#define HWIO_CPR_GCNT0_STEP_QUOT_INIT_PER_RO_KV0_BMSK                                    0xfc00
#define HWIO_CPR_GCNT0_STEP_QUOT_INIT_PER_RO_KV0_SHFT                                       0xa
#define HWIO_CPR_GCNT0_GCNT0_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT0_GCNT0_SHFT                                                           0x0

#define HWIO_CPR_GCNT1_ADDR(x)                                                       ((x) + 0x000000a4)
#define HWIO_CPR_GCNT1_PHYS(x)                                                       ((x) + 0x000000a4)
#define HWIO_CPR_GCNT1_OFFS                                                          (0x000000a4)
#define HWIO_CPR_GCNT1_RMSK                                                              0xffff
#define HWIO_CPR_GCNT1_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT1_ADDR(x), HWIO_CPR_GCNT1_RMSK)
#define HWIO_CPR_GCNT1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT1_ADDR(x), m)
#define HWIO_CPR_GCNT1_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT1_ADDR(x),v)
#define HWIO_CPR_GCNT1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT1_ADDR(x),m,v,HWIO_CPR_GCNT1_IN(x))
#define HWIO_CPR_GCNT1_STEP_QUOT_INIT_PER_RO_KV1_BMSK                                    0xfc00
#define HWIO_CPR_GCNT1_STEP_QUOT_INIT_PER_RO_KV1_SHFT                                       0xa
#define HWIO_CPR_GCNT1_GCNT1_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT1_GCNT1_SHFT                                                           0x0

#define HWIO_CPR_GCNT2_ADDR(x)                                                       ((x) + 0x000000a8)
#define HWIO_CPR_GCNT2_PHYS(x)                                                       ((x) + 0x000000a8)
#define HWIO_CPR_GCNT2_OFFS                                                          (0x000000a8)
#define HWIO_CPR_GCNT2_RMSK                                                              0xffff
#define HWIO_CPR_GCNT2_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT2_ADDR(x), HWIO_CPR_GCNT2_RMSK)
#define HWIO_CPR_GCNT2_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT2_ADDR(x), m)
#define HWIO_CPR_GCNT2_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT2_ADDR(x),v)
#define HWIO_CPR_GCNT2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT2_ADDR(x),m,v,HWIO_CPR_GCNT2_IN(x))
#define HWIO_CPR_GCNT2_STEP_QUOT_INIT_PER_RO_KV2_BMSK                                    0xfc00
#define HWIO_CPR_GCNT2_STEP_QUOT_INIT_PER_RO_KV2_SHFT                                       0xa
#define HWIO_CPR_GCNT2_GCNT2_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT2_GCNT2_SHFT                                                           0x0

#define HWIO_CPR_GCNT3_ADDR(x)                                                       ((x) + 0x000000ac)
#define HWIO_CPR_GCNT3_PHYS(x)                                                       ((x) + 0x000000ac)
#define HWIO_CPR_GCNT3_OFFS                                                          (0x000000ac)
#define HWIO_CPR_GCNT3_RMSK                                                              0xffff
#define HWIO_CPR_GCNT3_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT3_ADDR(x), HWIO_CPR_GCNT3_RMSK)
#define HWIO_CPR_GCNT3_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT3_ADDR(x), m)
#define HWIO_CPR_GCNT3_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT3_ADDR(x),v)
#define HWIO_CPR_GCNT3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT3_ADDR(x),m,v,HWIO_CPR_GCNT3_IN(x))
#define HWIO_CPR_GCNT3_STEP_QUOT_INIT_PER_RO_KV3_BMSK                                    0xfc00
#define HWIO_CPR_GCNT3_STEP_QUOT_INIT_PER_RO_KV3_SHFT                                       0xa
#define HWIO_CPR_GCNT3_GCNT3_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT3_GCNT3_SHFT                                                           0x0

#define HWIO_CPR_GCNT4_ADDR(x)                                                       ((x) + 0x000000b0)
#define HWIO_CPR_GCNT4_PHYS(x)                                                       ((x) + 0x000000b0)
#define HWIO_CPR_GCNT4_OFFS                                                          (0x000000b0)
#define HWIO_CPR_GCNT4_RMSK                                                              0xffff
#define HWIO_CPR_GCNT4_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT4_ADDR(x), HWIO_CPR_GCNT4_RMSK)
#define HWIO_CPR_GCNT4_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT4_ADDR(x), m)
#define HWIO_CPR_GCNT4_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT4_ADDR(x),v)
#define HWIO_CPR_GCNT4_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT4_ADDR(x),m,v,HWIO_CPR_GCNT4_IN(x))
#define HWIO_CPR_GCNT4_STEP_QUOT_INIT_PER_RO_KV4_BMSK                                    0xfc00
#define HWIO_CPR_GCNT4_STEP_QUOT_INIT_PER_RO_KV4_SHFT                                       0xa
#define HWIO_CPR_GCNT4_GCNT4_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT4_GCNT4_SHFT                                                           0x0

#define HWIO_CPR_GCNT5_ADDR(x)                                                       ((x) + 0x000000b4)
#define HWIO_CPR_GCNT5_PHYS(x)                                                       ((x) + 0x000000b4)
#define HWIO_CPR_GCNT5_OFFS                                                          (0x000000b4)
#define HWIO_CPR_GCNT5_RMSK                                                              0xffff
#define HWIO_CPR_GCNT5_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT5_ADDR(x), HWIO_CPR_GCNT5_RMSK)
#define HWIO_CPR_GCNT5_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT5_ADDR(x), m)
#define HWIO_CPR_GCNT5_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT5_ADDR(x),v)
#define HWIO_CPR_GCNT5_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT5_ADDR(x),m,v,HWIO_CPR_GCNT5_IN(x))
#define HWIO_CPR_GCNT5_STEP_QUOT_INIT_PER_RO_KV5_BMSK                                    0xfc00
#define HWIO_CPR_GCNT5_STEP_QUOT_INIT_PER_RO_KV5_SHFT                                       0xa
#define HWIO_CPR_GCNT5_GCNT5_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT5_GCNT5_SHFT                                                           0x0

#define HWIO_CPR_GCNT6_ADDR(x)                                                       ((x) + 0x000000b8)
#define HWIO_CPR_GCNT6_PHYS(x)                                                       ((x) + 0x000000b8)
#define HWIO_CPR_GCNT6_OFFS                                                          (0x000000b8)
#define HWIO_CPR_GCNT6_RMSK                                                              0xffff
#define HWIO_CPR_GCNT6_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT6_ADDR(x), HWIO_CPR_GCNT6_RMSK)
#define HWIO_CPR_GCNT6_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT6_ADDR(x), m)
#define HWIO_CPR_GCNT6_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT6_ADDR(x),v)
#define HWIO_CPR_GCNT6_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT6_ADDR(x),m,v,HWIO_CPR_GCNT6_IN(x))
#define HWIO_CPR_GCNT6_STEP_QUOT_INIT_PER_RO_KV6_BMSK                                    0xfc00
#define HWIO_CPR_GCNT6_STEP_QUOT_INIT_PER_RO_KV6_SHFT                                       0xa
#define HWIO_CPR_GCNT6_GCNT6_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT6_GCNT6_SHFT                                                           0x0

#define HWIO_CPR_GCNT7_ADDR(x)                                                       ((x) + 0x000000bc)
#define HWIO_CPR_GCNT7_PHYS(x)                                                       ((x) + 0x000000bc)
#define HWIO_CPR_GCNT7_OFFS                                                          (0x000000bc)
#define HWIO_CPR_GCNT7_RMSK                                                              0xffff
#define HWIO_CPR_GCNT7_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT7_ADDR(x), HWIO_CPR_GCNT7_RMSK)
#define HWIO_CPR_GCNT7_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT7_ADDR(x), m)
#define HWIO_CPR_GCNT7_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT7_ADDR(x),v)
#define HWIO_CPR_GCNT7_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT7_ADDR(x),m,v,HWIO_CPR_GCNT7_IN(x))
#define HWIO_CPR_GCNT7_STEP_QUOT_INIT_PER_RO_KV7_BMSK                                    0xfc00
#define HWIO_CPR_GCNT7_STEP_QUOT_INIT_PER_RO_KV7_SHFT                                       0xa
#define HWIO_CPR_GCNT7_GCNT7_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT7_GCNT7_SHFT                                                           0x0

#define HWIO_CPR_GCNT8_ADDR(x)                                                       ((x) + 0x000000c0)
#define HWIO_CPR_GCNT8_PHYS(x)                                                       ((x) + 0x000000c0)
#define HWIO_CPR_GCNT8_OFFS                                                          (0x000000c0)
#define HWIO_CPR_GCNT8_RMSK                                                              0xffff
#define HWIO_CPR_GCNT8_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT8_ADDR(x), HWIO_CPR_GCNT8_RMSK)
#define HWIO_CPR_GCNT8_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT8_ADDR(x), m)
#define HWIO_CPR_GCNT8_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT8_ADDR(x),v)
#define HWIO_CPR_GCNT8_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT8_ADDR(x),m,v,HWIO_CPR_GCNT8_IN(x))
#define HWIO_CPR_GCNT8_STEP_QUOT_INIT_PER_RO_KV8_BMSK                                    0xfc00
#define HWIO_CPR_GCNT8_STEP_QUOT_INIT_PER_RO_KV8_SHFT                                       0xa
#define HWIO_CPR_GCNT8_GCNT8_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT8_GCNT8_SHFT                                                           0x0

#define HWIO_CPR_GCNT9_ADDR(x)                                                       ((x) + 0x000000c4)
#define HWIO_CPR_GCNT9_PHYS(x)                                                       ((x) + 0x000000c4)
#define HWIO_CPR_GCNT9_OFFS                                                          (0x000000c4)
#define HWIO_CPR_GCNT9_RMSK                                                              0xffff
#define HWIO_CPR_GCNT9_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT9_ADDR(x), HWIO_CPR_GCNT9_RMSK)
#define HWIO_CPR_GCNT9_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT9_ADDR(x), m)
#define HWIO_CPR_GCNT9_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT9_ADDR(x),v)
#define HWIO_CPR_GCNT9_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT9_ADDR(x),m,v,HWIO_CPR_GCNT9_IN(x))
#define HWIO_CPR_GCNT9_STEP_QUOT_INIT_PER_RO_KV9_BMSK                                    0xfc00
#define HWIO_CPR_GCNT9_STEP_QUOT_INIT_PER_RO_KV9_SHFT                                       0xa
#define HWIO_CPR_GCNT9_GCNT9_BMSK                                                         0x3ff
#define HWIO_CPR_GCNT9_GCNT9_SHFT                                                           0x0

#define HWIO_CPR_GCNT10_ADDR(x)                                                      ((x) + 0x000000c8)
#define HWIO_CPR_GCNT10_PHYS(x)                                                      ((x) + 0x000000c8)
#define HWIO_CPR_GCNT10_OFFS                                                         (0x000000c8)
#define HWIO_CPR_GCNT10_RMSK                                                             0xffff
#define HWIO_CPR_GCNT10_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT10_ADDR(x), HWIO_CPR_GCNT10_RMSK)
#define HWIO_CPR_GCNT10_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT10_ADDR(x), m)
#define HWIO_CPR_GCNT10_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT10_ADDR(x),v)
#define HWIO_CPR_GCNT10_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT10_ADDR(x),m,v,HWIO_CPR_GCNT10_IN(x))
#define HWIO_CPR_GCNT10_STEP_QUOT_INIT_PER_RO_KV10_BMSK                                  0xfc00
#define HWIO_CPR_GCNT10_STEP_QUOT_INIT_PER_RO_KV10_SHFT                                     0xa
#define HWIO_CPR_GCNT10_GCNT10_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT10_GCNT10_SHFT                                                         0x0

#define HWIO_CPR_GCNT11_ADDR(x)                                                      ((x) + 0x000000cc)
#define HWIO_CPR_GCNT11_PHYS(x)                                                      ((x) + 0x000000cc)
#define HWIO_CPR_GCNT11_OFFS                                                         (0x000000cc)
#define HWIO_CPR_GCNT11_RMSK                                                             0xffff
#define HWIO_CPR_GCNT11_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT11_ADDR(x), HWIO_CPR_GCNT11_RMSK)
#define HWIO_CPR_GCNT11_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT11_ADDR(x), m)
#define HWIO_CPR_GCNT11_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT11_ADDR(x),v)
#define HWIO_CPR_GCNT11_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT11_ADDR(x),m,v,HWIO_CPR_GCNT11_IN(x))
#define HWIO_CPR_GCNT11_STEP_QUOT_INIT_PER_RO_KV11_BMSK                                  0xfc00
#define HWIO_CPR_GCNT11_STEP_QUOT_INIT_PER_RO_KV11_SHFT                                     0xa
#define HWIO_CPR_GCNT11_GCNT11_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT11_GCNT11_SHFT                                                         0x0

#define HWIO_CPR_GCNT12_ADDR(x)                                                      ((x) + 0x000000d0)
#define HWIO_CPR_GCNT12_PHYS(x)                                                      ((x) + 0x000000d0)
#define HWIO_CPR_GCNT12_OFFS                                                         (0x000000d0)
#define HWIO_CPR_GCNT12_RMSK                                                             0xffff
#define HWIO_CPR_GCNT12_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT12_ADDR(x), HWIO_CPR_GCNT12_RMSK)
#define HWIO_CPR_GCNT12_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT12_ADDR(x), m)
#define HWIO_CPR_GCNT12_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT12_ADDR(x),v)
#define HWIO_CPR_GCNT12_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT12_ADDR(x),m,v,HWIO_CPR_GCNT12_IN(x))
#define HWIO_CPR_GCNT12_STEP_QUOT_INIT_PER_RO_KV12_BMSK                                  0xfc00
#define HWIO_CPR_GCNT12_STEP_QUOT_INIT_PER_RO_KV12_SHFT                                     0xa
#define HWIO_CPR_GCNT12_GCNT12_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT12_GCNT12_SHFT                                                         0x0

#define HWIO_CPR_GCNT13_ADDR(x)                                                      ((x) + 0x000000d4)
#define HWIO_CPR_GCNT13_PHYS(x)                                                      ((x) + 0x000000d4)
#define HWIO_CPR_GCNT13_OFFS                                                         (0x000000d4)
#define HWIO_CPR_GCNT13_RMSK                                                             0xffff
#define HWIO_CPR_GCNT13_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT13_ADDR(x), HWIO_CPR_GCNT13_RMSK)
#define HWIO_CPR_GCNT13_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT13_ADDR(x), m)
#define HWIO_CPR_GCNT13_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT13_ADDR(x),v)
#define HWIO_CPR_GCNT13_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT13_ADDR(x),m,v,HWIO_CPR_GCNT13_IN(x))
#define HWIO_CPR_GCNT13_STEP_QUOT_INIT_PER_RO_KV13_BMSK                                  0xfc00
#define HWIO_CPR_GCNT13_STEP_QUOT_INIT_PER_RO_KV13_SHFT                                     0xa
#define HWIO_CPR_GCNT13_GCNT13_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT13_GCNT13_SHFT                                                         0x0

#define HWIO_CPR_GCNT14_ADDR(x)                                                      ((x) + 0x000000d8)
#define HWIO_CPR_GCNT14_PHYS(x)                                                      ((x) + 0x000000d8)
#define HWIO_CPR_GCNT14_OFFS                                                         (0x000000d8)
#define HWIO_CPR_GCNT14_RMSK                                                             0xffff
#define HWIO_CPR_GCNT14_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT14_ADDR(x), HWIO_CPR_GCNT14_RMSK)
#define HWIO_CPR_GCNT14_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT14_ADDR(x), m)
#define HWIO_CPR_GCNT14_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT14_ADDR(x),v)
#define HWIO_CPR_GCNT14_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT14_ADDR(x),m,v,HWIO_CPR_GCNT14_IN(x))
#define HWIO_CPR_GCNT14_STEP_QUOT_INIT_PER_RO_KV14_BMSK                                  0xfc00
#define HWIO_CPR_GCNT14_STEP_QUOT_INIT_PER_RO_KV14_SHFT                                     0xa
#define HWIO_CPR_GCNT14_GCNT14_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT14_GCNT14_SHFT                                                         0x0

#define HWIO_CPR_GCNT15_ADDR(x)                                                      ((x) + 0x000000dc)
#define HWIO_CPR_GCNT15_PHYS(x)                                                      ((x) + 0x000000dc)
#define HWIO_CPR_GCNT15_OFFS                                                         (0x000000dc)
#define HWIO_CPR_GCNT15_RMSK                                                             0xffff
#define HWIO_CPR_GCNT15_IN(x)      \
        in_dword_masked(HWIO_CPR_GCNT15_ADDR(x), HWIO_CPR_GCNT15_RMSK)
#define HWIO_CPR_GCNT15_INM(x, m)      \
        in_dword_masked(HWIO_CPR_GCNT15_ADDR(x), m)
#define HWIO_CPR_GCNT15_OUT(x, v)      \
        out_dword(HWIO_CPR_GCNT15_ADDR(x),v)
#define HWIO_CPR_GCNT15_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_GCNT15_ADDR(x),m,v,HWIO_CPR_GCNT15_IN(x))
#define HWIO_CPR_GCNT15_STEP_QUOT_INIT_PER_RO_KV15_BMSK                                  0xfc00
#define HWIO_CPR_GCNT15_STEP_QUOT_INIT_PER_RO_KV15_SHFT                                     0xa
#define HWIO_CPR_GCNT15_GCNT15_BMSK                                                       0x3ff
#define HWIO_CPR_GCNT15_GCNT15_SHFT                                                         0x0

#define HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n)                                   ((base) + 0x000000e0 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_PHYS(base,n)                                   ((base) + 0x000000e0 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_OFFS(base,n)                                   (0x000000e0 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_RMSK                                           0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_MAXn                                                    1
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n), HWIO_CPR_SENSOR_BYPASS_WRITEn_RMSK)
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_CPR_SENSOR_BYPASS_WRITEn_ADDR(base,n),mask,val,HWIO_CPR_SENSOR_BYPASS_WRITEn_INI(base,n))
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_BMSK                             0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_WRITEn_SENSOR_BYPASS_SHFT                                    0x0

#define HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n)                                    ((base) + 0x00000100 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_READn_PHYS(base,n)                                    ((base) + 0x00000100 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_READn_OFFS(base,n)                                    (0x00000100 + 0x4 * (n))
#define HWIO_CPR_SENSOR_BYPASS_READn_RMSK                                            0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_READn_MAXn                                                     1
#define HWIO_CPR_SENSOR_BYPASS_READn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n), HWIO_CPR_SENSOR_BYPASS_READn_RMSK)
#define HWIO_CPR_SENSOR_BYPASS_READn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_BYPASS_READn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_BMSK                              0xffffffff
#define HWIO_CPR_SENSOR_BYPASS_READn_SENSOR_BYPASS_SHFT                                     0x0

#define HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n)                                     ((base) + 0x00000120 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_WRITEn_PHYS(base,n)                                     ((base) + 0x00000120 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_WRITEn_OFFS(base,n)                                     (0x00000120 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_WRITEn_RMSK                                             0xffffffff
#define HWIO_CPR_SENSOR_MASK_WRITEn_MAXn                                                      1
#define HWIO_CPR_SENSOR_MASK_WRITEn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n), HWIO_CPR_SENSOR_MASK_WRITEn_RMSK)
#define HWIO_CPR_SENSOR_MASK_WRITEn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_MASK_WRITEn_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_MASK_WRITEn_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_CPR_SENSOR_MASK_WRITEn_ADDR(base,n),mask,val,HWIO_CPR_SENSOR_MASK_WRITEn_INI(base,n))
#define HWIO_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_BMSK                                 0xffffffff
#define HWIO_CPR_SENSOR_MASK_WRITEn_SENSOR_MASK_SHFT                                        0x0

#define HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n)                                      ((base) + 0x00000140 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_READn_PHYS(base,n)                                      ((base) + 0x00000140 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_READn_OFFS(base,n)                                      (0x00000140 + 0x4 * (n))
#define HWIO_CPR_SENSOR_MASK_READn_RMSK                                              0xffffffff
#define HWIO_CPR_SENSOR_MASK_READn_MAXn                                                       1
#define HWIO_CPR_SENSOR_MASK_READn_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n), HWIO_CPR_SENSOR_MASK_READn_RMSK)
#define HWIO_CPR_SENSOR_MASK_READn_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_MASK_READn_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_MASK_READn_SENSOR_MASK_BMSK                                  0xffffffff
#define HWIO_CPR_SENSOR_MASK_READn_SENSOR_MASK_SHFT                                         0x0

#define HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n)                                        ((base) + 0x00000200 + 0x4 * (n))
#define HWIO_CPR_SENSOR_THREAD_n_PHYS(base,n)                                        ((base) + 0x00000200 + 0x4 * (n))
#define HWIO_CPR_SENSOR_THREAD_n_OFFS(base,n)                                        (0x00000200 + 0x4 * (n))
#define HWIO_CPR_SENSOR_THREAD_n_RMSK                                                       0x1
#define HWIO_CPR_SENSOR_THREAD_n_MAXn                                                        62
#define HWIO_CPR_SENSOR_THREAD_n_INI(base,n)        \
        in_dword_masked(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n), HWIO_CPR_SENSOR_THREAD_n_RMSK)
#define HWIO_CPR_SENSOR_THREAD_n_INMI(base,n,mask)    \
        in_dword_masked(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n), mask)
#define HWIO_CPR_SENSOR_THREAD_n_OUTI(base,n,val)    \
        out_dword(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n),val)
#define HWIO_CPR_SENSOR_THREAD_n_OUTMI(base,n,mask,val) \
        out_dword_masked_ns(HWIO_CPR_SENSOR_THREAD_n_ADDR(base,n),mask,val,HWIO_CPR_SENSOR_THREAD_n_INI(base,n))
#define HWIO_CPR_SENSOR_THREAD_n_THREAD_ID_BMSK                                             0x1
#define HWIO_CPR_SENSOR_THREAD_n_THREAD_ID_SHFT                                             0x0

#define HWIO_CPR_MISC_REGISTER_ADDR(x)                                               ((x) + 0x00000700)
#define HWIO_CPR_MISC_REGISTER_PHYS(x)                                               ((x) + 0x00000700)
#define HWIO_CPR_MISC_REGISTER_OFFS                                                  (0x00000700)
#define HWIO_CPR_MISC_REGISTER_RMSK                                                  0xffffffff
#define HWIO_CPR_MISC_REGISTER_IN(x)      \
        in_dword_masked(HWIO_CPR_MISC_REGISTER_ADDR(x), HWIO_CPR_MISC_REGISTER_RMSK)
#define HWIO_CPR_MISC_REGISTER_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MISC_REGISTER_ADDR(x), m)
#define HWIO_CPR_MISC_REGISTER_OUT(x, v)      \
        out_dword(HWIO_CPR_MISC_REGISTER_ADDR(x),v)
#define HWIO_CPR_MISC_REGISTER_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MISC_REGISTER_ADDR(x),m,v,HWIO_CPR_MISC_REGISTER_IN(x))
#define HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_END_BMSK                               0xf0000000
#define HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_END_SHFT                                     0x1c
#define HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_START_BMSK                              0xf000000
#define HWIO_CPR_MISC_REGISTER_TEMP_SENSOR_ID_START_SHFT                                   0x18
#define HWIO_CPR_MISC_REGISTER_MARGIN_TABLE_ROW_SELECT_BMSK                            0xf00000
#define HWIO_CPR_MISC_REGISTER_MARGIN_TABLE_ROW_SELECT_SHFT                                0x14
#define HWIO_CPR_MISC_REGISTER_UNUSED_BMSK                                              0xffc00
#define HWIO_CPR_MISC_REGISTER_UNUSED_SHFT                                                  0xa
#define HWIO_CPR_MISC_REGISTER_CPRF_MID_COMMAND_EN_BMSK                                   0x200
#define HWIO_CPR_MISC_REGISTER_CPRF_MID_COMMAND_EN_SHFT                                     0x9
#define HWIO_CPR_MISC_REGISTER_CPR_TSENSE_CTL_READING_EN_BMSK                             0x100
#define HWIO_CPR_MISC_REGISTER_CPR_TSENSE_CTL_READING_EN_SHFT                               0x8
#define HWIO_CPR_MISC_REGISTER_CPR_AGGRESIVE_CGC_DISABLE_BMSK                              0x80
#define HWIO_CPR_MISC_REGISTER_CPR_AGGRESIVE_CGC_DISABLE_SHFT                               0x7
#define HWIO_CPR_MISC_REGISTER_CPRF_SETTLE_TIMER_DOWN_EN_BMSK                              0x40
#define HWIO_CPR_MISC_REGISTER_CPRF_SETTLE_TIMER_DOWN_EN_SHFT                               0x6
#define HWIO_CPR_MISC_REGISTER_CPRF_INTERFACE_ENABLE_BMSK                                  0x20
#define HWIO_CPR_MISC_REGISTER_CPRF_INTERFACE_ENABLE_SHFT                                   0x5
#define HWIO_CPR_MISC_REGISTER_STEP_QUOT_INIT_PER_RO_BMSK                                  0x10
#define HWIO_CPR_MISC_REGISTER_STEP_QUOT_INIT_PER_RO_SHFT                                   0x4
#define HWIO_CPR_MISC_REGISTER_THREAD_HAS_ALWAYS_VOTE_BMSK                                  0x8
#define HWIO_CPR_MISC_REGISTER_THREAD_HAS_ALWAYS_VOTE_SHFT                                  0x3
#define HWIO_CPR_MISC_REGISTER_RESET_STEP_QUOT_LOOP_EN_BMSK                                 0x4
#define HWIO_CPR_MISC_REGISTER_RESET_STEP_QUOT_LOOP_EN_SHFT                                 0x2
#define HWIO_CPR_MISC_REGISTER_ADD_SUB_MARGIN_BMSK                                          0x2
#define HWIO_CPR_MISC_REGISTER_ADD_SUB_MARGIN_SHFT                                          0x1
#define HWIO_CPR_MISC_REGISTER_TEMP_MARGIN_POLARITY_BMSK                                    0x1
#define HWIO_CPR_MISC_REGISTER_TEMP_MARGIN_POLARITY_SHFT                                    0x0

#define HWIO_CPR_PER_SENSOR_QUOT_0_ADDR(x)                                           ((x) + 0x00000780)
#define HWIO_CPR_PER_SENSOR_QUOT_0_PHYS(x)                                           ((x) + 0x00000780)
#define HWIO_CPR_PER_SENSOR_QUOT_0_OFFS                                              (0x00000780)
#define HWIO_CPR_PER_SENSOR_QUOT_0_RMSK                                                 0xfffff
#define HWIO_CPR_PER_SENSOR_QUOT_0_IN(x)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_0_ADDR(x), HWIO_CPR_PER_SENSOR_QUOT_0_RMSK)
#define HWIO_CPR_PER_SENSOR_QUOT_0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_0_ADDR(x), m)
#define HWIO_CPR_PER_SENSOR_QUOT_0_OUT(x, v)      \
        out_dword(HWIO_CPR_PER_SENSOR_QUOT_0_ADDR(x),v)
#define HWIO_CPR_PER_SENSOR_QUOT_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_PER_SENSOR_QUOT_0_ADDR(x),m,v,HWIO_CPR_PER_SENSOR_QUOT_0_IN(x))
#define HWIO_CPR_PER_SENSOR_QUOT_0_PER_SENSOR_QUOT_QUOT_0_BMSK                          0xfff00
#define HWIO_CPR_PER_SENSOR_QUOT_0_PER_SENSOR_QUOT_QUOT_0_SHFT                              0x8
#define HWIO_CPR_PER_SENSOR_QUOT_0_PER_SENSOR_QUOT_ID_0_BMSK                               0xff
#define HWIO_CPR_PER_SENSOR_QUOT_0_PER_SENSOR_QUOT_ID_0_SHFT                                0x0

#define HWIO_CPR_PER_SENSOR_QUOT_1_ADDR(x)                                           ((x) + 0x00000784)
#define HWIO_CPR_PER_SENSOR_QUOT_1_PHYS(x)                                           ((x) + 0x00000784)
#define HWIO_CPR_PER_SENSOR_QUOT_1_OFFS                                              (0x00000784)
#define HWIO_CPR_PER_SENSOR_QUOT_1_RMSK                                                 0xfffff
#define HWIO_CPR_PER_SENSOR_QUOT_1_IN(x)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_1_ADDR(x), HWIO_CPR_PER_SENSOR_QUOT_1_RMSK)
#define HWIO_CPR_PER_SENSOR_QUOT_1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_1_ADDR(x), m)
#define HWIO_CPR_PER_SENSOR_QUOT_1_OUT(x, v)      \
        out_dword(HWIO_CPR_PER_SENSOR_QUOT_1_ADDR(x),v)
#define HWIO_CPR_PER_SENSOR_QUOT_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_PER_SENSOR_QUOT_1_ADDR(x),m,v,HWIO_CPR_PER_SENSOR_QUOT_1_IN(x))
#define HWIO_CPR_PER_SENSOR_QUOT_1_PER_SENSOR_QUOT_QUOT_1_BMSK                          0xfff00
#define HWIO_CPR_PER_SENSOR_QUOT_1_PER_SENSOR_QUOT_QUOT_1_SHFT                              0x8
#define HWIO_CPR_PER_SENSOR_QUOT_1_PER_SENSOR_QUOT_ID_1_BMSK                               0xff
#define HWIO_CPR_PER_SENSOR_QUOT_1_PER_SENSOR_QUOT_ID_1_SHFT                                0x0

#define HWIO_CPR_PER_SENSOR_QUOT_2_ADDR(x)                                           ((x) + 0x00000788)
#define HWIO_CPR_PER_SENSOR_QUOT_2_PHYS(x)                                           ((x) + 0x00000788)
#define HWIO_CPR_PER_SENSOR_QUOT_2_OFFS                                              (0x00000788)
#define HWIO_CPR_PER_SENSOR_QUOT_2_RMSK                                                 0xfffff
#define HWIO_CPR_PER_SENSOR_QUOT_2_IN(x)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_2_ADDR(x), HWIO_CPR_PER_SENSOR_QUOT_2_RMSK)
#define HWIO_CPR_PER_SENSOR_QUOT_2_INM(x, m)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_2_ADDR(x), m)
#define HWIO_CPR_PER_SENSOR_QUOT_2_OUT(x, v)      \
        out_dword(HWIO_CPR_PER_SENSOR_QUOT_2_ADDR(x),v)
#define HWIO_CPR_PER_SENSOR_QUOT_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_PER_SENSOR_QUOT_2_ADDR(x),m,v,HWIO_CPR_PER_SENSOR_QUOT_2_IN(x))
#define HWIO_CPR_PER_SENSOR_QUOT_2_PER_SENSOR_QUOT_QUOT_2_BMSK                          0xfff00
#define HWIO_CPR_PER_SENSOR_QUOT_2_PER_SENSOR_QUOT_QUOT_2_SHFT                              0x8
#define HWIO_CPR_PER_SENSOR_QUOT_2_PER_SENSOR_QUOT_ID_2_BMSK                               0xff
#define HWIO_CPR_PER_SENSOR_QUOT_2_PER_SENSOR_QUOT_ID_2_SHFT                                0x0

#define HWIO_CPR_PER_SENSOR_QUOT_3_ADDR(x)                                           ((x) + 0x0000078c)
#define HWIO_CPR_PER_SENSOR_QUOT_3_PHYS(x)                                           ((x) + 0x0000078c)
#define HWIO_CPR_PER_SENSOR_QUOT_3_OFFS                                              (0x0000078c)
#define HWIO_CPR_PER_SENSOR_QUOT_3_RMSK                                                 0xfffff
#define HWIO_CPR_PER_SENSOR_QUOT_3_IN(x)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_3_ADDR(x), HWIO_CPR_PER_SENSOR_QUOT_3_RMSK)
#define HWIO_CPR_PER_SENSOR_QUOT_3_INM(x, m)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_3_ADDR(x), m)
#define HWIO_CPR_PER_SENSOR_QUOT_3_OUT(x, v)      \
        out_dword(HWIO_CPR_PER_SENSOR_QUOT_3_ADDR(x),v)
#define HWIO_CPR_PER_SENSOR_QUOT_3_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_PER_SENSOR_QUOT_3_ADDR(x),m,v,HWIO_CPR_PER_SENSOR_QUOT_3_IN(x))
#define HWIO_CPR_PER_SENSOR_QUOT_3_PER_SENSOR_QUOT_QUOT_3_BMSK                          0xfff00
#define HWIO_CPR_PER_SENSOR_QUOT_3_PER_SENSOR_QUOT_QUOT_3_SHFT                              0x8
#define HWIO_CPR_PER_SENSOR_QUOT_3_PER_SENSOR_QUOT_ID_3_BMSK                               0xff
#define HWIO_CPR_PER_SENSOR_QUOT_3_PER_SENSOR_QUOT_ID_3_SHFT                                0x0

#define HWIO_CPR_PER_SENSOR_QUOT_4_ADDR(x)                                           ((x) + 0x00000790)
#define HWIO_CPR_PER_SENSOR_QUOT_4_PHYS(x)                                           ((x) + 0x00000790)
#define HWIO_CPR_PER_SENSOR_QUOT_4_OFFS                                              (0x00000790)
#define HWIO_CPR_PER_SENSOR_QUOT_4_RMSK                                                 0xfffff
#define HWIO_CPR_PER_SENSOR_QUOT_4_IN(x)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_4_ADDR(x), HWIO_CPR_PER_SENSOR_QUOT_4_RMSK)
#define HWIO_CPR_PER_SENSOR_QUOT_4_INM(x, m)      \
        in_dword_masked(HWIO_CPR_PER_SENSOR_QUOT_4_ADDR(x), m)
#define HWIO_CPR_PER_SENSOR_QUOT_4_OUT(x, v)      \
        out_dword(HWIO_CPR_PER_SENSOR_QUOT_4_ADDR(x),v)
#define HWIO_CPR_PER_SENSOR_QUOT_4_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_PER_SENSOR_QUOT_4_ADDR(x),m,v,HWIO_CPR_PER_SENSOR_QUOT_4_IN(x))
#define HWIO_CPR_PER_SENSOR_QUOT_4_PER_SENSOR_QUOT_QUOT_4_BMSK                          0xfff00
#define HWIO_CPR_PER_SENSOR_QUOT_4_PER_SENSOR_QUOT_QUOT_4_SHFT                              0x8
#define HWIO_CPR_PER_SENSOR_QUOT_4_PER_SENSOR_QUOT_ID_4_BMSK                               0xff
#define HWIO_CPR_PER_SENSOR_QUOT_4_PER_SENSOR_QUOT_ID_4_SHFT                                0x0

#define HWIO_CPR_DEBUG_BUS_ADDR(x)                                                   ((x) + 0x00000794)
#define HWIO_CPR_DEBUG_BUS_PHYS(x)                                                   ((x) + 0x00000794)
#define HWIO_CPR_DEBUG_BUS_OFFS                                                      (0x00000794)
#define HWIO_CPR_DEBUG_BUS_RMSK                                                      0xffffffff
#define HWIO_CPR_DEBUG_BUS_IN(x)      \
        in_dword_masked(HWIO_CPR_DEBUG_BUS_ADDR(x), HWIO_CPR_DEBUG_BUS_RMSK)
#define HWIO_CPR_DEBUG_BUS_INM(x, m)      \
        in_dword_masked(HWIO_CPR_DEBUG_BUS_ADDR(x), m)
#define HWIO_CPR_DEBUG_BUS_CPR_DEBUG_BUS_BMSK                                        0xffffffff
#define HWIO_CPR_DEBUG_BUS_CPR_DEBUG_BUS_SHFT                                               0x0

#define HWIO_CPR_CORE_ADJ_MODE_REG_ADDR(x)                                           ((x) + 0x00000798)
#define HWIO_CPR_CORE_ADJ_MODE_REG_PHYS(x)                                           ((x) + 0x00000798)
#define HWIO_CPR_CORE_ADJ_MODE_REG_OFFS                                              (0x00000798)
#define HWIO_CPR_CORE_ADJ_MODE_REG_RMSK                                              0xffffffff
#define HWIO_CPR_CORE_ADJ_MODE_REG_IN(x)      \
        in_dword_masked(HWIO_CPR_CORE_ADJ_MODE_REG_ADDR(x), HWIO_CPR_CORE_ADJ_MODE_REG_RMSK)
#define HWIO_CPR_CORE_ADJ_MODE_REG_INM(x, m)      \
        in_dword_masked(HWIO_CPR_CORE_ADJ_MODE_REG_ADDR(x), m)
#define HWIO_CPR_CORE_ADJ_MODE_REG_OUT(x, v)      \
        out_dword(HWIO_CPR_CORE_ADJ_MODE_REG_ADDR(x),v)
#define HWIO_CPR_CORE_ADJ_MODE_REG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_CORE_ADJ_MODE_REG_ADDR(x),m,v,HWIO_CPR_CORE_ADJ_MODE_REG_IN(x))
#define HWIO_CPR_CORE_ADJ_MODE_REG_MODE_CHANGE_READY_BMSK                            0x80000000
#define HWIO_CPR_CORE_ADJ_MODE_REG_MODE_CHANGE_READY_SHFT                                  0x1f
#define HWIO_CPR_CORE_ADJ_MODE_REG_CODE_ADJ_NUM_CORES_BMSK                           0x78000000
#define HWIO_CPR_CORE_ADJ_MODE_REG_CODE_ADJ_NUM_CORES_SHFT                                 0x1b
#define HWIO_CPR_CORE_ADJ_MODE_REG_UNUSED_BMSK                                        0x7fffffe
#define HWIO_CPR_CORE_ADJ_MODE_REG_UNUSED_SHFT                                              0x1
#define HWIO_CPR_CORE_ADJ_MODE_REG_MODE_CHANGE_BMSK                                         0x1
#define HWIO_CPR_CORE_ADJ_MODE_REG_MODE_CHANGE_SHFT                                         0x0

#define HWIO_CPR_CPR_SRAM_SENSORS_ADDR(x)                                            ((x) + 0x0000079c)
#define HWIO_CPR_CPR_SRAM_SENSORS_PHYS(x)                                            ((x) + 0x0000079c)
#define HWIO_CPR_CPR_SRAM_SENSORS_OFFS                                               (0x0000079c)
#define HWIO_CPR_CPR_SRAM_SENSORS_RMSK                                               0xc3ffffff
#define HWIO_CPR_CPR_SRAM_SENSORS_IN(x)      \
        in_dword_masked(HWIO_CPR_CPR_SRAM_SENSORS_ADDR(x), HWIO_CPR_CPR_SRAM_SENSORS_RMSK)
#define HWIO_CPR_CPR_SRAM_SENSORS_INM(x, m)      \
        in_dword_masked(HWIO_CPR_CPR_SRAM_SENSORS_ADDR(x), m)
#define HWIO_CPR_CPR_SRAM_SENSORS_OUT(x, v)      \
        out_dword(HWIO_CPR_CPR_SRAM_SENSORS_ADDR(x),v)
#define HWIO_CPR_CPR_SRAM_SENSORS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_CPR_SRAM_SENSORS_ADDR(x),m,v,HWIO_CPR_CPR_SRAM_SENSORS_IN(x))
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM0_RESET_BMSK                                   0x80000000
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM0_RESET_SHFT                                         0x1f
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM1_RESET_BMSK                                   0x40000000
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM1_RESET_SHFT                                         0x1e
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM1_DATA_BMSK                                     0x3ffe000
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM1_DATA_SHFT                                           0xd
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM0_DATA_BMSK                                        0x1fff
#define HWIO_CPR_CPR_SRAM_SENSORS_SRAM0_DATA_SHFT                                           0x0

#define HWIO_CPR_CPRF_DN_TIMER_ADDR(x)                                               ((x) + 0x000007a0)
#define HWIO_CPR_CPRF_DN_TIMER_PHYS(x)                                               ((x) + 0x000007a0)
#define HWIO_CPR_CPRF_DN_TIMER_OFFS                                                  (0x000007a0)
#define HWIO_CPR_CPRF_DN_TIMER_RMSK                                                  0xffffffff
#define HWIO_CPR_CPRF_DN_TIMER_IN(x)      \
        in_dword_masked(HWIO_CPR_CPRF_DN_TIMER_ADDR(x), HWIO_CPR_CPRF_DN_TIMER_RMSK)
#define HWIO_CPR_CPRF_DN_TIMER_INM(x, m)      \
        in_dword_masked(HWIO_CPR_CPRF_DN_TIMER_ADDR(x), m)
#define HWIO_CPR_CPRF_DN_TIMER_OUT(x, v)      \
        out_dword(HWIO_CPR_CPRF_DN_TIMER_ADDR(x),v)
#define HWIO_CPR_CPRF_DN_TIMER_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_CPRF_DN_TIMER_ADDR(x),m,v,HWIO_CPR_CPRF_DN_TIMER_IN(x))
#define HWIO_CPR_CPRF_DN_TIMER_TIMER_INTERVAL_BMSK                                   0xffffffff
#define HWIO_CPR_CPRF_DN_TIMER_TIMER_INTERVAL_SHFT                                          0x0

#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_ADDR(x)                                        ((x) + 0x000007a4)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_PHYS(x)                                        ((x) + 0x000007a4)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_OFFS                                           (0x000007a4)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_RMSK                                                0x3ff
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_IN(x)      \
        in_dword_masked(HWIO_CPR_SAW_ERROR_STEP_LIMIT_ADDR(x), HWIO_CPR_SAW_ERROR_STEP_LIMIT_RMSK)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_SAW_ERROR_STEP_LIMIT_ADDR(x), m)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_OUT(x, v)      \
        out_dword(HWIO_CPR_SAW_ERROR_STEP_LIMIT_ADDR(x),v)
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_SAW_ERROR_STEP_LIMIT_ADDR(x),m,v,HWIO_CPR_SAW_ERROR_STEP_LIMIT_IN(x))
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_DN_ERROR_STEP_LIMIT_BMSK                            0x3e0
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_DN_ERROR_STEP_LIMIT_SHFT                              0x5
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_UP_ERROR_STEP_LIMIT_BMSK                             0x1f
#define HWIO_CPR_SAW_ERROR_STEP_LIMIT_UP_ERROR_STEP_LIMIT_SHFT                              0x0

#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_ADDR(x)                                     ((x) + 0x000007a8)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_PHYS(x)                                     ((x) + 0x000007a8)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_OFFS                                        (0x000007a8)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_RMSK                                        0xffffffff
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_ADDR(x), HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_RMSK)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_IN(x))
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_UNUSED_BMSK                                 0xe0000000
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_UNUSED_SHFT                                       0x1d
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_TIMER_SETTLE_VOLTAGE_COUNT_BMSK             0x1ffc0000
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_TIMER_SETTLE_VOLTAGE_COUNT_SHFT                   0x12
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_TIMER_LOWER_VOLTAGE_COUNT_BMSK                 0x3ffff
#define HWIO_CPR_MARGIN_TEMP_CORE_TIMERS_TIMER_LOWER_VOLTAGE_COUNT_SHFT                     0x0

#define HWIO_CPR_MARGIN_TEMP_0_ADDR(x)                                               ((x) + 0x000007ac)
#define HWIO_CPR_MARGIN_TEMP_0_PHYS(x)                                               ((x) + 0x000007ac)
#define HWIO_CPR_MARGIN_TEMP_0_OFFS                                                  (0x000007ac)
#define HWIO_CPR_MARGIN_TEMP_0_RMSK                                                  0xffffffff
#define HWIO_CPR_MARGIN_TEMP_0_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_0_ADDR(x), HWIO_CPR_MARGIN_TEMP_0_RMSK)
#define HWIO_CPR_MARGIN_TEMP_0_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_0_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_0_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_0_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_0_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_0_IN(x))
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND3_BMSK                                 0xff000000
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND3_SHFT                                       0x18
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND2_BMSK                                   0xff0000
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND2_SHFT                                       0x10
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND1_BMSK                                     0xff00
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND1_SHFT                                        0x8
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND0_BMSK                                       0xff
#define HWIO_CPR_MARGIN_TEMP_0_MARGIN_TEMPBAND0_SHFT                                        0x0

#define HWIO_CPR_MARGIN_TEMP_1_ADDR(x)                                               ((x) + 0x000007b0)
#define HWIO_CPR_MARGIN_TEMP_1_PHYS(x)                                               ((x) + 0x000007b0)
#define HWIO_CPR_MARGIN_TEMP_1_OFFS                                                  (0x000007b0)
#define HWIO_CPR_MARGIN_TEMP_1_RMSK                                                  0xffffffff
#define HWIO_CPR_MARGIN_TEMP_1_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_1_ADDR(x), HWIO_CPR_MARGIN_TEMP_1_RMSK)
#define HWIO_CPR_MARGIN_TEMP_1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_1_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_1_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_1_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_1_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_1_IN(x))
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND3_BMSK                                 0xff000000
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND3_SHFT                                       0x18
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND2_BMSK                                   0xff0000
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND2_SHFT                                       0x10
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND1_BMSK                                     0xff00
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND1_SHFT                                        0x8
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND0_BMSK                                       0xff
#define HWIO_CPR_MARGIN_TEMP_1_MARGIN_TEMPBAND0_SHFT                                        0x0

#define HWIO_CPR_MARGIN_TEMP_2_ADDR(x)                                               ((x) + 0x000007b4)
#define HWIO_CPR_MARGIN_TEMP_2_PHYS(x)                                               ((x) + 0x000007b4)
#define HWIO_CPR_MARGIN_TEMP_2_OFFS                                                  (0x000007b4)
#define HWIO_CPR_MARGIN_TEMP_2_RMSK                                                  0xffffffff
#define HWIO_CPR_MARGIN_TEMP_2_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_2_ADDR(x), HWIO_CPR_MARGIN_TEMP_2_RMSK)
#define HWIO_CPR_MARGIN_TEMP_2_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_2_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_2_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_2_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_2_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_2_IN(x))
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND3_BMSK                                 0xff000000
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND3_SHFT                                       0x18
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND2_BMSK                                   0xff0000
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND2_SHFT                                       0x10
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND1_BMSK                                     0xff00
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND1_SHFT                                        0x8
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND0_BMSK                                       0xff
#define HWIO_CPR_MARGIN_TEMP_2_MARGIN_TEMPBAND0_SHFT                                        0x0

#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_ADDR(x)                                      ((x) + 0x000007b8)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_PHYS(x)                                      ((x) + 0x000007b8)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_OFFS                                         (0x000007b8)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_RMSK                                         0xffffffff
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_DCVS_POINT_ADDR(x), HWIO_CPR_MARGIN_TEMP_DCVS_POINT_RMSK)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_DCVS_POINT_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_DCVS_POINT_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_DCVS_POINT_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_DCVS_POINT_IN(x))
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_UNUSED_BMSK                                  0xffff8000
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_UNUSED_SHFT                                         0xf
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT2_BMSK                                 0x7c00
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT2_SHFT                                    0xa
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT1_BMSK                                  0x3e0
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT1_SHFT                                    0x5
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT0_BMSK                                   0x1f
#define HWIO_CPR_MARGIN_TEMP_DCVS_POINT_DCVS_POINT0_SHFT                                    0x0

#define HWIO_CPR_TEMP_SENSOR_DATA_ADDR(x)                                            ((x) + 0x000007ec)
#define HWIO_CPR_TEMP_SENSOR_DATA_PHYS(x)                                            ((x) + 0x000007ec)
#define HWIO_CPR_TEMP_SENSOR_DATA_OFFS                                               (0x000007ec)
#define HWIO_CPR_TEMP_SENSOR_DATA_RMSK                                               0xffffffff
#define HWIO_CPR_TEMP_SENSOR_DATA_IN(x)      \
        in_dword_masked(HWIO_CPR_TEMP_SENSOR_DATA_ADDR(x), HWIO_CPR_TEMP_SENSOR_DATA_RMSK)
#define HWIO_CPR_TEMP_SENSOR_DATA_INM(x, m)      \
        in_dword_masked(HWIO_CPR_TEMP_SENSOR_DATA_ADDR(x), m)
#define HWIO_CPR_TEMP_SENSOR_DATA_COLD_SENSOR_BMSK                                   0xf0000000
#define HWIO_CPR_TEMP_SENSOR_DATA_COLD_SENSOR_SHFT                                         0x1c
#define HWIO_CPR_TEMP_SENSOR_DATA_COLD_TEMP_BMSK                                      0xfff0000
#define HWIO_CPR_TEMP_SENSOR_DATA_COLD_TEMP_SHFT                                           0x10
#define HWIO_CPR_TEMP_SENSOR_DATA_HOT_SENSOR_BMSK                                        0xf000
#define HWIO_CPR_TEMP_SENSOR_DATA_HOT_SENSOR_SHFT                                           0xc
#define HWIO_CPR_TEMP_SENSOR_DATA_HOT_TEMP_BMSK                                           0xfff
#define HWIO_CPR_TEMP_SENSOR_DATA_HOT_TEMP_SHFT                                             0x0

#define HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(x)                                        ((x) + 0x000007f0)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_PHYS(x)                                        ((x) + 0x000007f0)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_OFFS                                           (0x000007f0)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_RMSK                                           0xc0ffffff
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(x), HWIO_CPR_MARGIN_TEMP_POINT0N1_RMSK)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_POINT0N1_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_POINT0N1_IN(x))
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_BELOWPOINT0_HOTCOLD_BMSK                       0x80000000
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_BELOWPOINT0_HOTCOLD_SHFT                             0x1f
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_BELOWPOINT1_HOTCOLD_BMSK                       0x40000000
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_BELOWPOINT1_HOTCOLD_SHFT                             0x1e
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT1_BMSK                                      0xfff000
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT1_SHFT                                           0xc
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT0_BMSK                                         0xfff
#define HWIO_CPR_MARGIN_TEMP_POINT0N1_POINT0_SHFT                                           0x0

#define HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(x)                                          ((x) + 0x000007f4)
#define HWIO_CPR_MARGIN_TEMP_POINT2_PHYS(x)                                          ((x) + 0x000007f4)
#define HWIO_CPR_MARGIN_TEMP_POINT2_OFFS                                             (0x000007f4)
#define HWIO_CPR_MARGIN_TEMP_POINT2_RMSK                                             0xc0000fff
#define HWIO_CPR_MARGIN_TEMP_POINT2_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(x), HWIO_CPR_MARGIN_TEMP_POINT2_RMSK)
#define HWIO_CPR_MARGIN_TEMP_POINT2_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(x), m)
#define HWIO_CPR_MARGIN_TEMP_POINT2_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(x),v)
#define HWIO_CPR_MARGIN_TEMP_POINT2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_TEMP_POINT2_ADDR(x),m,v,HWIO_CPR_MARGIN_TEMP_POINT2_IN(x))
#define HWIO_CPR_MARGIN_TEMP_POINT2_BELOWPOINT2_HOTCOLD_BMSK                         0x80000000
#define HWIO_CPR_MARGIN_TEMP_POINT2_BELOWPOINT2_HOTCOLD_SHFT                               0x1f
#define HWIO_CPR_MARGIN_TEMP_POINT2_ABOVEPOINT2_HOTCOLD_BMSK                         0x40000000
#define HWIO_CPR_MARGIN_TEMP_POINT2_ABOVEPOINT2_HOTCOLD_SHFT                               0x1e
#define HWIO_CPR_MARGIN_TEMP_POINT2_POINT2_BMSK                                           0xfff
#define HWIO_CPR_MARGIN_TEMP_POINT2_POINT2_SHFT                                             0x0

#define HWIO_CPR_MARGIN_ADJ_CTL_ADDR(x)                                              ((x) + 0x000007f8)
#define HWIO_CPR_MARGIN_ADJ_CTL_PHYS(x)                                              ((x) + 0x000007f8)
#define HWIO_CPR_MARGIN_ADJ_CTL_OFFS                                                 (0x000007f8)
#define HWIO_CPR_MARGIN_ADJ_CTL_RMSK                                                 0xffffffff
#define HWIO_CPR_MARGIN_ADJ_CTL_IN(x)      \
        in_dword_masked(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(x), HWIO_CPR_MARGIN_ADJ_CTL_RMSK)
#define HWIO_CPR_MARGIN_ADJ_CTL_INM(x, m)      \
        in_dword_masked(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(x), m)
#define HWIO_CPR_MARGIN_ADJ_CTL_OUT(x, v)      \
        out_dword(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(x),v)
#define HWIO_CPR_MARGIN_ADJ_CTL_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_MARGIN_ADJ_CTL_ADDR(x),m,v,HWIO_CPR_MARGIN_ADJ_CTL_IN(x))
#define HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_SINGLE_STEP_QUOT_BMSK                  0xfc000000
#define HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_SINGLE_STEP_QUOT_SHFT                        0x1a
#define HWIO_CPR_MARGIN_ADJ_CTL_MAX_NUM_CORES_BMSK                                    0x3c00000
#define HWIO_CPR_MARGIN_ADJ_CTL_MAX_NUM_CORES_SHFT                                         0x16
#define HWIO_CPR_MARGIN_ADJ_CTL_INITIAL_TEMP_BAND_BMSK                                 0x380000
#define HWIO_CPR_MARGIN_ADJ_CTL_INITIAL_TEMP_BAND_SHFT                                     0x13
#define HWIO_CPR_MARGIN_ADJ_CTL_LOWER_VOLTAGE_WAIT_SELECT_BMSK                          0x60000
#define HWIO_CPR_MARGIN_ADJ_CTL_LOWER_VOLTAGE_WAIT_SELECT_SHFT                             0x11
#define HWIO_CPR_MARGIN_ADJ_CTL_PMIC_STEP_SIZE_BMSK                                     0x1f000
#define HWIO_CPR_MARGIN_ADJ_CTL_PMIC_STEP_SIZE_SHFT                                         0xc
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_BF_BMSK                                       0x800
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_BF_SHFT                                         0xb
#define HWIO_CPR_MARGIN_ADJ_CTL_SAW_ERROR_REPEAT_EN_BMSK                                  0x400
#define HWIO_CPR_MARGIN_ADJ_CTL_SAW_ERROR_REPEAT_EN_SHFT                                    0xa
#define HWIO_CPR_MARGIN_ADJ_CTL_ADJ_SW_RESET_BMSK                                         0x200
#define HWIO_CPR_MARGIN_ADJ_CTL_ADJ_SW_RESET_SHFT                                           0x9
#define HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_EN_BMSK                                     0x100
#define HWIO_CPR_MARGIN_ADJ_CTL_KV_MARGIN_ADJ_EN_SHFT                                       0x8
#define HWIO_CPR_MARGIN_ADJ_CTL_PER_RO_KV_MARGIN_EN_BMSK                                   0x80
#define HWIO_CPR_MARGIN_ADJ_CTL_PER_RO_KV_MARGIN_EN_SHFT                                    0x7
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_TEMP_EN_BMSK                                   0x40
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_TEMP_EN_SHFT                                    0x6
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_EN_BMSK                                        0x20
#define HWIO_CPR_MARGIN_ADJ_CTL_WAIT_MID_DN_EN_SHFT                                         0x5
#define HWIO_CPR_MARGIN_ADJ_CTL_CLOSED_LOOP_EN_BMSK                                        0x10
#define HWIO_CPR_MARGIN_ADJ_CTL_CLOSED_LOOP_EN_SHFT                                         0x4
#define HWIO_CPR_MARGIN_ADJ_CTL_TIMER_SETTLE_VOLTAGE_EN_BMSK                                0x8
#define HWIO_CPR_MARGIN_ADJ_CTL_TIMER_SETTLE_VOLTAGE_EN_SHFT                                0x3
#define HWIO_CPR_MARGIN_ADJ_CTL_TEMP_ADJ_EN_BMSK                                            0x4
#define HWIO_CPR_MARGIN_ADJ_CTL_TEMP_ADJ_EN_SHFT                                            0x2
#define HWIO_CPR_MARGIN_ADJ_CTL_CORE_ADJ_EN_BMSK                                            0x2
#define HWIO_CPR_MARGIN_ADJ_CTL_CORE_ADJ_EN_SHFT                                            0x1
#define HWIO_CPR_MARGIN_ADJ_CTL_BOOST_EN_BMSK                                               0x1
#define HWIO_CPR_MARGIN_ADJ_CTL_BOOST_EN_SHFT                                               0x0

#define HWIO_CPR_HW_STARVATION_THRESHOLD_ADDR(x)                                     ((x) + 0x000007fc)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_PHYS(x)                                     ((x) + 0x000007fc)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_OFFS                                        (0x000007fc)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_RMSK                                        0xffffffff
#define HWIO_CPR_HW_STARVATION_THRESHOLD_IN(x)      \
        in_dword_masked(HWIO_CPR_HW_STARVATION_THRESHOLD_ADDR(x), HWIO_CPR_HW_STARVATION_THRESHOLD_RMSK)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_INM(x, m)      \
        in_dword_masked(HWIO_CPR_HW_STARVATION_THRESHOLD_ADDR(x), m)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_OUT(x, v)      \
        out_dword(HWIO_CPR_HW_STARVATION_THRESHOLD_ADDR(x),v)
#define HWIO_CPR_HW_STARVATION_THRESHOLD_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_CPR_HW_STARVATION_THRESHOLD_ADDR(x),m,v,HWIO_CPR_HW_STARVATION_THRESHOLD_IN(x))
#define HWIO_CPR_HW_STARVATION_THRESHOLD_THRESHOLD_BMSK                              0xffffffff
#define HWIO_CPR_HW_STARVATION_THRESHOLD_THRESHOLD_SHFT                                     0x0

#define HWIO_CPR_CONT_CMD_t_ADDR(base,t)                                             ((base) + 0x00000800 + 0x440 * (t))
#define HWIO_CPR_CONT_CMD_t_PHYS(base,t)                                             ((base) + 0x00000800 + 0x440 * (t))
#define HWIO_CPR_CONT_CMD_t_OFFS(base,t)                                             (0x00000800 + 0x440 * (t))
#define HWIO_CPR_CONT_CMD_t_RMSK                                                            0x1
#define HWIO_CPR_CONT_CMD_t_MAXt                                                              0
#define HWIO_CPR_CONT_CMD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_CONT_CMD_t_ADDR(base,t),val)
#define HWIO_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_BMSK                                       0x1
#define HWIO_CPR_CONT_CMD_t_VDD_CHANGED_ONE_STEP_SHFT                                       0x0

#define HWIO_CPR_THRESHOLD_t_ADDR(base,t)                                            ((base) + 0x00000808 + 0x440 * (t))
#define HWIO_CPR_THRESHOLD_t_PHYS(base,t)                                            ((base) + 0x00000808 + 0x440 * (t))
#define HWIO_CPR_THRESHOLD_t_OFFS(base,t)                                            (0x00000808 + 0x440 * (t))
#define HWIO_CPR_THRESHOLD_t_RMSK                                                       0x3ffff
#define HWIO_CPR_THRESHOLD_t_MAXt                                                             0
#define HWIO_CPR_THRESHOLD_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_THRESHOLD_t_ADDR(base,t), HWIO_CPR_THRESHOLD_t_RMSK)
#define HWIO_CPR_THRESHOLD_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_THRESHOLD_t_ADDR(base,t), mask)
#define HWIO_CPR_THRESHOLD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_THRESHOLD_t_ADDR(base,t),val)
#define HWIO_CPR_THRESHOLD_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_THRESHOLD_t_ADDR(base,t),mask,val,HWIO_CPR_THRESHOLD_t_INI(base,t))
#define HWIO_CPR_THRESHOLD_t_UP_THRESHOLD_BMSK                                          0x3e000
#define HWIO_CPR_THRESHOLD_t_UP_THRESHOLD_SHFT                                              0xd
#define HWIO_CPR_THRESHOLD_t_DN_THRESHOLD_BMSK                                           0x1f00
#define HWIO_CPR_THRESHOLD_t_DN_THRESHOLD_SHFT                                              0x8
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_UP_BMSK                                           0xf0
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_UP_SHFT                                            0x4
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_DN_BMSK                                            0xf
#define HWIO_CPR_THRESHOLD_t_CONSECUTIVE_DN_SHFT                                            0x0

#define HWIO_CPR_MASK_THREAD_t_ADDR(base,t)                                          ((base) + 0x0000080c + 0x440 * (t))
#define HWIO_CPR_MASK_THREAD_t_PHYS(base,t)                                          ((base) + 0x0000080c + 0x440 * (t))
#define HWIO_CPR_MASK_THREAD_t_OFFS(base,t)                                          (0x0000080c + 0x440 * (t))
#define HWIO_CPR_MASK_THREAD_t_RMSK                                                  0x8000ffff
#define HWIO_CPR_MASK_THREAD_t_MAXt                                                           0
#define HWIO_CPR_MASK_THREAD_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_MASK_THREAD_t_ADDR(base,t), HWIO_CPR_MASK_THREAD_t_RMSK)
#define HWIO_CPR_MASK_THREAD_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_MASK_THREAD_t_ADDR(base,t), mask)
#define HWIO_CPR_MASK_THREAD_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_MASK_THREAD_t_ADDR(base,t),val)
#define HWIO_CPR_MASK_THREAD_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_MASK_THREAD_t_ADDR(base,t),mask,val,HWIO_CPR_MASK_THREAD_t_INI(base,t))
#define HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD_BMSK                                   0x80000000
#define HWIO_CPR_MASK_THREAD_t_DISABLE_THREAD_SHFT                                         0x1f
#define HWIO_CPR_MASK_THREAD_t_RO_MASK4THREAD_BMSK                                       0xffff
#define HWIO_CPR_MASK_THREAD_t_RO_MASK4THREAD_SHFT                                          0x0

#define HWIO_CPR_RESULT0_t_ADDR(base,t)                                              ((base) + 0x00000810 + 0x440 * (t))
#define HWIO_CPR_RESULT0_t_PHYS(base,t)                                              ((base) + 0x00000810 + 0x440 * (t))
#define HWIO_CPR_RESULT0_t_OFFS(base,t)                                              (0x00000810 + 0x440 * (t))
#define HWIO_CPR_RESULT0_t_RMSK                                                        0x1fffff
#define HWIO_CPR_RESULT0_t_MAXt                                                               0
#define HWIO_CPR_RESULT0_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT0_t_ADDR(base,t), HWIO_CPR_RESULT0_t_RMSK)
#define HWIO_CPR_RESULT0_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT0_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT0_t_ERROR_LT_0_BMSK                                             0x100000
#define HWIO_CPR_RESULT0_t_ERROR_LT_0_SHFT                                                 0x14
#define HWIO_CPR_RESULT0_t_ERROR_BMSK                                                   0xfff00
#define HWIO_CPR_RESULT0_t_ERROR_SHFT                                                       0x8
#define HWIO_CPR_RESULT0_t_ERROR_STEPS_BMSK                                                0xf8
#define HWIO_CPR_RESULT0_t_ERROR_STEPS_SHFT                                                 0x3
#define HWIO_CPR_RESULT0_t_STEP_UP_BMSK                                                     0x4
#define HWIO_CPR_RESULT0_t_STEP_UP_SHFT                                                     0x2
#define HWIO_CPR_RESULT0_t_STEP_DN_BMSK                                                     0x2
#define HWIO_CPR_RESULT0_t_STEP_DN_SHFT                                                     0x1
#define HWIO_CPR_RESULT0_t_BUSY_BMSK                                                        0x1
#define HWIO_CPR_RESULT0_t_BUSY_SHFT                                                        0x0

#define HWIO_CPR_RESULT1_t_ADDR(base,t)                                              ((base) + 0x00000814 + 0x440 * (t))
#define HWIO_CPR_RESULT1_t_PHYS(base,t)                                              ((base) + 0x00000814 + 0x440 * (t))
#define HWIO_CPR_RESULT1_t_OFFS(base,t)                                              (0x00000814 + 0x440 * (t))
#define HWIO_CPR_RESULT1_t_RMSK                                                      0xffffffff
#define HWIO_CPR_RESULT1_t_MAXt                                                               0
#define HWIO_CPR_RESULT1_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT1_t_ADDR(base,t), HWIO_CPR_RESULT1_t_RMSK)
#define HWIO_CPR_RESULT1_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT1_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT1_t_SEL_MAX_BMSK                                              0xf0000000
#define HWIO_CPR_RESULT1_t_SEL_MAX_SHFT                                                    0x1c
#define HWIO_CPR_RESULT1_t_SEL_MIN_BMSK                                               0xf000000
#define HWIO_CPR_RESULT1_t_SEL_MIN_SHFT                                                    0x18
#define HWIO_CPR_RESULT1_t_QUOT_MAX_BMSK                                               0xfff000
#define HWIO_CPR_RESULT1_t_QUOT_MAX_SHFT                                                    0xc
#define HWIO_CPR_RESULT1_t_QUOT_MIN_BMSK                                                  0xfff
#define HWIO_CPR_RESULT1_t_QUOT_MIN_SHFT                                                    0x0

#define HWIO_CPR_RESULT2_t_ADDR(base,t)                                              ((base) + 0x00000818 + 0x440 * (t))
#define HWIO_CPR_RESULT2_t_PHYS(base,t)                                              ((base) + 0x00000818 + 0x440 * (t))
#define HWIO_CPR_RESULT2_t_OFFS(base,t)                                              (0x00000818 + 0x440 * (t))
#define HWIO_CPR_RESULT2_t_RMSK                                                      0xffff0fff
#define HWIO_CPR_RESULT2_t_MAXt                                                               0
#define HWIO_CPR_RESULT2_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_RESULT2_t_ADDR(base,t), HWIO_CPR_RESULT2_t_RMSK)
#define HWIO_CPR_RESULT2_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_RESULT2_t_ADDR(base,t), mask)
#define HWIO_CPR_RESULT2_t_SENSOR_MAX_BMSK                                           0xff000000
#define HWIO_CPR_RESULT2_t_SENSOR_MAX_SHFT                                                 0x18
#define HWIO_CPR_RESULT2_t_SENSOR_MIN_BMSK                                             0xff0000
#define HWIO_CPR_RESULT2_t_SENSOR_MIN_SHFT                                                 0x10
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MAX_BMSK                                             0xfc0
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MAX_SHFT                                               0x6
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MIN_BMSK                                              0x3f
#define HWIO_CPR_RESULT2_t_STEP_QUOT_MIN_SHFT                                               0x0

#define HWIO_CPR_IRQ_EN_t_ADDR(base,t)                                               ((base) + 0x0000081c + 0x440 * (t))
#define HWIO_CPR_IRQ_EN_t_PHYS(base,t)                                               ((base) + 0x0000081c + 0x440 * (t))
#define HWIO_CPR_IRQ_EN_t_OFFS(base,t)                                               (0x0000081c + 0x440 * (t))
#define HWIO_CPR_IRQ_EN_t_RMSK                                                       0xc000000f
#define HWIO_CPR_IRQ_EN_t_MAXt                                                                0
#define HWIO_CPR_IRQ_EN_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_IRQ_EN_t_ADDR(base,t), HWIO_CPR_IRQ_EN_t_RMSK)
#define HWIO_CPR_IRQ_EN_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_IRQ_EN_t_ADDR(base,t), mask)
#define HWIO_CPR_IRQ_EN_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_IRQ_EN_t_ADDR(base,t),val)
#define HWIO_CPR_IRQ_EN_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_IRQ_EN_t_ADDR(base,t),mask,val,HWIO_CPR_IRQ_EN_t_INI(base,t))
#define HWIO_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_BMSK                            0x80000000
#define HWIO_CPR_IRQ_EN_t_CLAMP_CHANGE_WHILE_BUSY_EN_SHFT                                  0x1f
#define HWIO_CPR_IRQ_EN_t_STARVATION_ERROR_EN_BMSK                                   0x40000000
#define HWIO_CPR_IRQ_EN_t_STARVATION_ERROR_EN_SHFT                                         0x1e
#define HWIO_CPR_IRQ_EN_t_UP_FLAG_EN_BMSK                                                   0x8
#define HWIO_CPR_IRQ_EN_t_UP_FLAG_EN_SHFT                                                   0x3
#define HWIO_CPR_IRQ_EN_t_MID_FLAG_EN_BMSK                                                  0x4
#define HWIO_CPR_IRQ_EN_t_MID_FLAG_EN_SHFT                                                  0x2
#define HWIO_CPR_IRQ_EN_t_DOWN_FLAG_EN_BMSK                                                 0x2
#define HWIO_CPR_IRQ_EN_t_DOWN_FLAG_EN_SHFT                                                 0x1
#define HWIO_CPR_IRQ_EN_t_CPR_DONE_EN_BMSK                                                  0x1
#define HWIO_CPR_IRQ_EN_t_CPR_DONE_EN_SHFT                                                  0x0

#define HWIO_CPR_IRQ_CLEAR_t_ADDR(base,t)                                            ((base) + 0x00000820 + 0x440 * (t))
#define HWIO_CPR_IRQ_CLEAR_t_PHYS(base,t)                                            ((base) + 0x00000820 + 0x440 * (t))
#define HWIO_CPR_IRQ_CLEAR_t_OFFS(base,t)                                            (0x00000820 + 0x440 * (t))
#define HWIO_CPR_IRQ_CLEAR_t_RMSK                                                    0xc000000f
#define HWIO_CPR_IRQ_CLEAR_t_MAXt                                                             0
#define HWIO_CPR_IRQ_CLEAR_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_IRQ_CLEAR_t_ADDR(base,t),val)
#define HWIO_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_BMSK                      0x80000000
#define HWIO_CPR_IRQ_CLEAR_t_CLAMP_CHANGE_WHILE_BUSY_CLEAR_SHFT                            0x1f
#define HWIO_CPR_IRQ_CLEAR_t_STARVATION_ERROR_CLEAR_BMSK                             0x40000000
#define HWIO_CPR_IRQ_CLEAR_t_STARVATION_ERROR_CLEAR_SHFT                                   0x1e
#define HWIO_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_BMSK                                             0x8
#define HWIO_CPR_IRQ_CLEAR_t_UP_FLAG_CLEAR_SHFT                                             0x3
#define HWIO_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_BMSK                                            0x4
#define HWIO_CPR_IRQ_CLEAR_t_MID_FLAG_CLEAR_SHFT                                            0x2
#define HWIO_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_BMSK                                           0x2
#define HWIO_CPR_IRQ_CLEAR_t_DOWN_FLAG_CLEAR_SHFT                                           0x1
#define HWIO_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_BMSK                                            0x1
#define HWIO_CPR_IRQ_CLEAR_t_CPR_DONE_CLEAR_SHFT                                            0x0

#define HWIO_CPR_IRQ_STATUS_t_ADDR(base,t)                                           ((base) + 0x00000824 + 0x440 * (t))
#define HWIO_CPR_IRQ_STATUS_t_PHYS(base,t)                                           ((base) + 0x00000824 + 0x440 * (t))
#define HWIO_CPR_IRQ_STATUS_t_OFFS(base,t)                                           (0x00000824 + 0x440 * (t))
#define HWIO_CPR_IRQ_STATUS_t_RMSK                                                   0xc000000f
#define HWIO_CPR_IRQ_STATUS_t_MAXt                                                            0
#define HWIO_CPR_IRQ_STATUS_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_IRQ_STATUS_t_ADDR(base,t), HWIO_CPR_IRQ_STATUS_t_RMSK)
#define HWIO_CPR_IRQ_STATUS_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_IRQ_STATUS_t_ADDR(base,t), mask)
#define HWIO_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_BMSK                           0x80000000
#define HWIO_CPR_IRQ_STATUS_t_CLAMP_CHANGE_WHILE_BUSY_SHFT                                 0x1f
#define HWIO_CPR_IRQ_STATUS_t_STARVATION_ERROR_BMSK                                  0x40000000
#define HWIO_CPR_IRQ_STATUS_t_STARVATION_ERROR_SHFT                                        0x1e
#define HWIO_CPR_IRQ_STATUS_t_UP_FLAG_BMSK                                                  0x8
#define HWIO_CPR_IRQ_STATUS_t_UP_FLAG_SHFT                                                  0x3
#define HWIO_CPR_IRQ_STATUS_t_MID_FLAG_BMSK                                                 0x4
#define HWIO_CPR_IRQ_STATUS_t_MID_FLAG_SHFT                                                 0x2
#define HWIO_CPR_IRQ_STATUS_t_DOWN_FLAG_BMSK                                                0x2
#define HWIO_CPR_IRQ_STATUS_t_DOWN_FLAG_SHFT                                                0x1
#define HWIO_CPR_IRQ_STATUS_t_CPR_DONE_BMSK                                                 0x1
#define HWIO_CPR_IRQ_STATUS_t_CPR_DONE_SHFT                                                 0x0

#define HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t)                                   ((base) + 0x00000828 + 0x440 * (t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_PHYS(base,t)                                   ((base) + 0x00000828 + 0x440 * (t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_OFFS(base,t)                                   (0x00000828 + 0x440 * (t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_RMSK                                                  0xf
#define HWIO_CPR_TARGET_MODE_SELECT_t_MAXt                                                    0
#define HWIO_CPR_TARGET_MODE_SELECT_t_INI(base,t)        \
        in_dword_masked(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t), HWIO_CPR_TARGET_MODE_SELECT_t_RMSK)
#define HWIO_CPR_TARGET_MODE_SELECT_t_INMI(base,t,mask)    \
        in_dword_masked(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t), mask)
#define HWIO_CPR_TARGET_MODE_SELECT_t_OUTI(base,t,val)    \
        out_dword(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t),val)
#define HWIO_CPR_TARGET_MODE_SELECT_t_OUTMI(base,t,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET_MODE_SELECT_t_ADDR(base,t),mask,val,HWIO_CPR_TARGET_MODE_SELECT_t_INI(base,t))
#define HWIO_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_BMSK                               0xf
#define HWIO_CPR_TARGET_MODE_SELECT_t_TARGET_MODE_SELECT_SHFT                               0x0

#define HWIO_CPR_TARGET0_t_m_ADDR(base,t,m)                                          ((base) + 0x00000840 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET0_t_m_PHYS(base,t,m)                                          ((base) + 0x00000840 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET0_t_m_OFFS(base,t,m)                                          (0x00000840 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET0_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET0_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET0_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET0_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m), HWIO_CPR_TARGET0_t_m_RMSK)
#define HWIO_CPR_TARGET0_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET0_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET0_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET0_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET0_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET0_t_m_TARGET0_BMSK                                                 0xfff
#define HWIO_CPR_TARGET0_t_m_TARGET0_SHFT                                                   0x0

#define HWIO_CPR_TARGET1_t_m_ADDR(base,t,m)                                          ((base) + 0x00000844 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET1_t_m_PHYS(base,t,m)                                          ((base) + 0x00000844 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET1_t_m_OFFS(base,t,m)                                          (0x00000844 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET1_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET1_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET1_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET1_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m), HWIO_CPR_TARGET1_t_m_RMSK)
#define HWIO_CPR_TARGET1_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET1_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET1_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET1_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET1_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET1_t_m_TARGET1_BMSK                                                 0xfff
#define HWIO_CPR_TARGET1_t_m_TARGET1_SHFT                                                   0x0

#define HWIO_CPR_TARGET2_t_m_ADDR(base,t,m)                                          ((base) + 0x00000848 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET2_t_m_PHYS(base,t,m)                                          ((base) + 0x00000848 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET2_t_m_OFFS(base,t,m)                                          (0x00000848 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET2_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET2_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET2_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET2_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m), HWIO_CPR_TARGET2_t_m_RMSK)
#define HWIO_CPR_TARGET2_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET2_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET2_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET2_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET2_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET2_t_m_TARGET2_BMSK                                                 0xfff
#define HWIO_CPR_TARGET2_t_m_TARGET2_SHFT                                                   0x0

#define HWIO_CPR_TARGET3_t_m_ADDR(base,t,m)                                          ((base) + 0x0000084c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET3_t_m_PHYS(base,t,m)                                          ((base) + 0x0000084c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET3_t_m_OFFS(base,t,m)                                          (0x0000084c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET3_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET3_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET3_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET3_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m), HWIO_CPR_TARGET3_t_m_RMSK)
#define HWIO_CPR_TARGET3_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET3_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET3_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET3_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET3_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET3_t_m_TARGET3_BMSK                                                 0xfff
#define HWIO_CPR_TARGET3_t_m_TARGET3_SHFT                                                   0x0

#define HWIO_CPR_TARGET4_t_m_ADDR(base,t,m)                                          ((base) + 0x00000850 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET4_t_m_PHYS(base,t,m)                                          ((base) + 0x00000850 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET4_t_m_OFFS(base,t,m)                                          (0x00000850 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET4_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET4_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET4_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET4_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m), HWIO_CPR_TARGET4_t_m_RMSK)
#define HWIO_CPR_TARGET4_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET4_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET4_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET4_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET4_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET4_t_m_TARGET4_BMSK                                                 0xfff
#define HWIO_CPR_TARGET4_t_m_TARGET4_SHFT                                                   0x0

#define HWIO_CPR_TARGET5_t_m_ADDR(base,t,m)                                          ((base) + 0x00000854 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET5_t_m_PHYS(base,t,m)                                          ((base) + 0x00000854 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET5_t_m_OFFS(base,t,m)                                          (0x00000854 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET5_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET5_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET5_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET5_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m), HWIO_CPR_TARGET5_t_m_RMSK)
#define HWIO_CPR_TARGET5_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET5_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET5_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET5_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET5_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET5_t_m_TARGET5_BMSK                                                 0xfff
#define HWIO_CPR_TARGET5_t_m_TARGET5_SHFT                                                   0x0

#define HWIO_CPR_TARGET6_t_m_ADDR(base,t,m)                                          ((base) + 0x00000858 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET6_t_m_PHYS(base,t,m)                                          ((base) + 0x00000858 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET6_t_m_OFFS(base,t,m)                                          (0x00000858 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET6_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET6_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET6_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET6_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m), HWIO_CPR_TARGET6_t_m_RMSK)
#define HWIO_CPR_TARGET6_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET6_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET6_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET6_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET6_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET6_t_m_TARGET6_BMSK                                                 0xfff
#define HWIO_CPR_TARGET6_t_m_TARGET6_SHFT                                                   0x0

#define HWIO_CPR_TARGET7_t_m_ADDR(base,t,m)                                          ((base) + 0x0000085c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET7_t_m_PHYS(base,t,m)                                          ((base) + 0x0000085c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET7_t_m_OFFS(base,t,m)                                          (0x0000085c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET7_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET7_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET7_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET7_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m), HWIO_CPR_TARGET7_t_m_RMSK)
#define HWIO_CPR_TARGET7_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET7_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET7_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET7_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET7_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET7_t_m_TARGET7_BMSK                                                 0xfff
#define HWIO_CPR_TARGET7_t_m_TARGET7_SHFT                                                   0x0

#define HWIO_CPR_TARGET8_t_m_ADDR(base,t,m)                                          ((base) + 0x00000860 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET8_t_m_PHYS(base,t,m)                                          ((base) + 0x00000860 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET8_t_m_OFFS(base,t,m)                                          (0x00000860 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET8_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET8_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET8_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET8_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m), HWIO_CPR_TARGET8_t_m_RMSK)
#define HWIO_CPR_TARGET8_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET8_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET8_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET8_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET8_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET8_t_m_TARGET8_BMSK                                                 0xfff
#define HWIO_CPR_TARGET8_t_m_TARGET8_SHFT                                                   0x0

#define HWIO_CPR_TARGET9_t_m_ADDR(base,t,m)                                          ((base) + 0x00000864 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET9_t_m_PHYS(base,t,m)                                          ((base) + 0x00000864 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET9_t_m_OFFS(base,t,m)                                          (0x00000864 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET9_t_m_RMSK                                                         0xfff
#define HWIO_CPR_TARGET9_t_m_MAXt                                                             0
#define HWIO_CPR_TARGET9_t_m_MAXm                                                            15
#define HWIO_CPR_TARGET9_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m), HWIO_CPR_TARGET9_t_m_RMSK)
#define HWIO_CPR_TARGET9_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET9_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET9_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET9_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET9_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET9_t_m_TARGET9_BMSK                                                 0xfff
#define HWIO_CPR_TARGET9_t_m_TARGET9_SHFT                                                   0x0

#define HWIO_CPR_TARGET10_t_m_ADDR(base,t,m)                                         ((base) + 0x00000868 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET10_t_m_PHYS(base,t,m)                                         ((base) + 0x00000868 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET10_t_m_OFFS(base,t,m)                                         (0x00000868 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET10_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET10_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET10_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET10_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m), HWIO_CPR_TARGET10_t_m_RMSK)
#define HWIO_CPR_TARGET10_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET10_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET10_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET10_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET10_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET10_t_m_TARGET10_BMSK                                               0xfff
#define HWIO_CPR_TARGET10_t_m_TARGET10_SHFT                                                 0x0

#define HWIO_CPR_TARGET11_t_m_ADDR(base,t,m)                                         ((base) + 0x0000086c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET11_t_m_PHYS(base,t,m)                                         ((base) + 0x0000086c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET11_t_m_OFFS(base,t,m)                                         (0x0000086c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET11_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET11_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET11_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET11_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m), HWIO_CPR_TARGET11_t_m_RMSK)
#define HWIO_CPR_TARGET11_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET11_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET11_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET11_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET11_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET11_t_m_TARGET11_BMSK                                               0xfff
#define HWIO_CPR_TARGET11_t_m_TARGET11_SHFT                                                 0x0

#define HWIO_CPR_TARGET12_t_m_ADDR(base,t,m)                                         ((base) + 0x00000870 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET12_t_m_PHYS(base,t,m)                                         ((base) + 0x00000870 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET12_t_m_OFFS(base,t,m)                                         (0x00000870 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET12_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET12_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET12_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET12_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m), HWIO_CPR_TARGET12_t_m_RMSK)
#define HWIO_CPR_TARGET12_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET12_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET12_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET12_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET12_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET12_t_m_TARGET12_BMSK                                               0xfff
#define HWIO_CPR_TARGET12_t_m_TARGET12_SHFT                                                 0x0

#define HWIO_CPR_TARGET13_t_m_ADDR(base,t,m)                                         ((base) + 0x00000874 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET13_t_m_PHYS(base,t,m)                                         ((base) + 0x00000874 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET13_t_m_OFFS(base,t,m)                                         (0x00000874 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET13_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET13_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET13_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET13_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m), HWIO_CPR_TARGET13_t_m_RMSK)
#define HWIO_CPR_TARGET13_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET13_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET13_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET13_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET13_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET13_t_m_TARGET13_BMSK                                               0xfff
#define HWIO_CPR_TARGET13_t_m_TARGET13_SHFT                                                 0x0

#define HWIO_CPR_TARGET14_t_m_ADDR(base,t,m)                                         ((base) + 0x00000878 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET14_t_m_PHYS(base,t,m)                                         ((base) + 0x00000878 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET14_t_m_OFFS(base,t,m)                                         (0x00000878 + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET14_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET14_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET14_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET14_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m), HWIO_CPR_TARGET14_t_m_RMSK)
#define HWIO_CPR_TARGET14_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET14_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET14_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET14_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET14_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET14_t_m_TARGET14_BMSK                                               0xfff
#define HWIO_CPR_TARGET14_t_m_TARGET14_SHFT                                                 0x0

#define HWIO_CPR_TARGET15_t_m_ADDR(base,t,m)                                         ((base) + 0x0000087c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET15_t_m_PHYS(base,t,m)                                         ((base) + 0x0000087c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET15_t_m_OFFS(base,t,m)                                         (0x0000087c + 0x440 * (t) + 0x40 * (m))
#define HWIO_CPR_TARGET15_t_m_RMSK                                                        0xfff
#define HWIO_CPR_TARGET15_t_m_MAXt                                                            0
#define HWIO_CPR_TARGET15_t_m_MAXm                                                           15
#define HWIO_CPR_TARGET15_t_m_INI2(base,t,m)        \
        in_dword_masked(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m), HWIO_CPR_TARGET15_t_m_RMSK)
#define HWIO_CPR_TARGET15_t_m_INMI2(base,t,m,mask)    \
        in_dword_masked(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m), mask)
#define HWIO_CPR_TARGET15_t_m_OUTI2(base,t,m,val)    \
        out_dword(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m),val)
#define HWIO_CPR_TARGET15_t_m_OUTMI2(base,t,m,mask,val) \
        out_dword_masked_ns(HWIO_CPR_TARGET15_t_m_ADDR(base,t,m),mask,val,HWIO_CPR_TARGET15_t_m_INI2(base,t,m))
#define HWIO_CPR_TARGET15_t_m_TARGET15_BMSK                                               0xfff
#define HWIO_CPR_TARGET15_t_m_TARGET15_SHFT                                                 0x0


#endif /* __CPR_HWIO_H__ */
