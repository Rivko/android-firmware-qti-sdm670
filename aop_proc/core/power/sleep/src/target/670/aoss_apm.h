#ifndef __AOSS_APM_H__
#define __AOSS_APM_H__
/*
===========================================================================
*/
/**
  @file aoss_apm.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r28.2.0]
 
  This file contains HWIO register definitions for the following modules:
    VDD_AOSS_APM

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/sleep/src/target/670/aoss_apm.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: VDD_AOSS_APM
 *--------------------------------------------------------------------------*/

#define VDD_AOSS_APM_REG_BASE                                                                   (AOSS_BASE      + 0x01266000)
#define VDD_AOSS_APM_REG_BASE_OFFS                                                              0x01266000

#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000000)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000000)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_RMSK                                              0x7f00710f
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_RST_OBSER_ARES_BMSK                           0x40000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_RST_OBSER_ARES_SHFT                                 0x1e
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_AON_OVERRIDE_AUX_BMSK                         0x20000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_AON_OVERRIDE_AUX_SHFT                               0x1d
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_AON_OVERRIDE_DOM_BMSK                         0x10000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_APM_AON_OVERRIDE_DOM_SHFT                               0x1c
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_PMUX_AUX_OUT_MX_POR_BMSK                       0x8000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_PMUX_AUX_OUT_MX_POR_SHFT                            0x1b
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_PMUX_DOM_OUT_MX_POR_BMSK                       0x4000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_PMUX_DOM_OUT_MX_POR_SHFT                            0x1a
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_SELF_TIMED_OUT_MX_POR_BMSK                     0x2000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_DIS_SELF_TIMED_OUT_MX_POR_SHFT                          0x19
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_RST_USE_DOM_POR_BMSK                               0x1000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_RST_USE_DOM_POR_SHFT                                    0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_COMP_INT_EN_BMSK                                      0x4000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_COMP_INT_EN_SHFT                                         0xe
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_COMP_SPEC_BMSK                                        0x3000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_COMP_SPEC_SHFT                                           0xc
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_EN_COMP_BMSK                                           0x100
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_EN_COMP_SHFT                                             0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_START_APM_SWITCH_BMSK                                    0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_START_APM_SWITCH_SHFT                                    0x3
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_TILE_ACT_BR_BMSK                                         0x6
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_TILE_ACT_BR_SHFT                                         0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ENABLE_NORMAL_OP_BMSK                                    0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_OUT_ENABLE_NORMAL_OP_SHFT                                    0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ADDR                                               (VDD_AOSS_APM_REG_BASE      + 0x00000004)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_OFFS                                               (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000004)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_RMSK                                                 0x11f717
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_APM_RST_OBSER_BMSK                                   0x100000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_APM_RST_OBSER_SHFT                                       0x14
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ERROR_CODE_BMSK                                       0x1e000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ERROR_CODE_SHFT                                           0xd
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ERROR_BMSK                                             0x1000
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_ERROR_SHFT                                                0xc
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_COMP_OUT_INT_BMSK                                       0x400
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_COMP_OUT_INT_SHFT                                         0xa
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_COMP_OUT_BMSK                                           0x300
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_COMP_OUT_SHFT                                             0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_EN_COMP_FDBK_BMSK                                        0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_EN_COMP_FDBK_SHFT                                         0x4
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_TILE_ACTIVE_BR_BMSK                                       0x6
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_TILE_ACTIVE_BR_SHFT                                       0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_TILE_TRANS_DONE_BMSK                                      0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_FUNC_IN_TILE_TRANS_DONE_SHFT                                      0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000008)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000008)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_RMSK                                               0x3fcffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OVERRIDE_NEXT_STATE_BMSK                           0x3e00000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OVERRIDE_NEXT_STATE_SHFT                                0x15
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OVERRIDE_SM_BMSK                                    0x100000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_OVERRIDE_SM_SHFT                                        0x14
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_RST_TO_TILE_BMSK                                     0x80000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_RST_TO_TILE_SHFT                                        0x13
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_TILE_RST_OVERRIDE_BMSK                               0x40000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_TILE_RST_OVERRIDE_SHFT                                  0x12
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_APM_CONFIG_BMSK                                       0xffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_0_APM_CONFIG_SHFT                                          0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x0000000c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x0000000c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_AUX2DOM_STEP_1_CNT_BMSK                   0x80000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_AUX2DOM_STEP_1_CNT_SHFT                         0x1f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_AUX2DOM_STEP_1_CNT_BMSK                           0x7f000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_AUX2DOM_STEP_1_CNT_SHFT                                 0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_3_CNT_BMSK                     0x800000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_3_CNT_SHFT                         0x17
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_3_CNT_BMSK                             0x7f0000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_3_CNT_SHFT                                 0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_2_CNT_BMSK                       0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_2_CNT_SHFT                          0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_2_CNT_BMSK                               0x7f00
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_2_CNT_SHFT                                  0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_1_CNT_BMSK                         0x80
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DISABLE_DOM2AUX_STEP_1_CNT_SHFT                          0x7
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_1_CNT_BMSK                                 0x7f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_1_DOM2AUX_STEP_1_CNT_SHFT                                  0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000010)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000010)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_DOM2PC_STEP_1_CNT_BMSK                    0x80000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_DOM2PC_STEP_1_CNT_SHFT                          0x1f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DOM2PC_STEP_1_CNT_BMSK                            0x7f000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DOM2PC_STEP_1_CNT_SHFT                                  0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2PC_STEP_1_CNT_BMSK                      0x800000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2PC_STEP_1_CNT_SHFT                          0x17
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2PC_STEP_1_CNT_BMSK                              0x7f0000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2PC_STEP_1_CNT_SHFT                                  0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2DOM_STEP_3_CNT_BMSK                       0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2DOM_STEP_3_CNT_SHFT                          0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2DOM_STEP_3_CNT_BMSK                               0x7f00
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2DOM_STEP_3_CNT_SHFT                                  0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2DOM_STEP_2_CNT_BMSK                         0x80
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_DISABLE_AUX2DOM_STEP_2_CNT_SHFT                          0x7
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2DOM_STEP_2_CNT_BMSK                                 0x7f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_2_AUX2DOM_STEP_2_CNT_SHFT                                  0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000014)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000014)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2DOM_STEP_1_CNT_BMSK                    0x80000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2DOM_STEP_1_CNT_SHFT                          0x1f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2DOM_STEP_1_CNT_BMSK                            0x7f000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2DOM_STEP_1_CNT_SHFT                                  0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_3_CNT_BMSK                      0x800000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_3_CNT_SHFT                          0x17
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_3_CNT_BMSK                              0x7f0000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_3_CNT_SHFT                                  0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_2_CNT_BMSK                        0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_2_CNT_SHFT                           0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_2_CNT_BMSK                                0x7f00
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_2_CNT_SHFT                                   0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_1_CNT_BMSK                          0x80
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_DISABLE_PC2AUX_STEP_1_CNT_SHFT                           0x7
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_1_CNT_BMSK                                  0x7f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_3_PC2AUX_STEP_1_CNT_SHFT                                   0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000018)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000018)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_RMSK                                              0x7fffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2COMP_EN_BMSK                       0x40000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2COMP_EN_SHFT                             0x1e
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2COMP_EN_BMSK                           0x20000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2COMP_EN_SHFT                                 0x1d
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_PMUX_AUX_POR_BMSK          0x10000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_PMUX_AUX_POR_SHFT                0x1c
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_PMUX_AUX_POR_BMSK               0x8000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_PMUX_AUX_POR_SHFT                    0x1b
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_PMUX_DOM_POR_BMSK           0x4000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_PMUX_DOM_POR_SHFT                0x1a
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_PMUX_DOM_POR_BMSK               0x2000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_PMUX_DOM_POR_SHFT                    0x19
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_ENR_BMSK                       0x1000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_ENR_SHFT                            0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_ENR_BMSK                            0x800000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_ENR_SHFT                                0x17
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_AUX_BMSK                     0x400000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_VAL_APM2TILE_EN_AUX_SHFT                         0x16
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_AUX_BMSK                         0x200000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_APM2TILE_EN_AUX_SHFT                             0x15
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_PMUX_AUX_OUT_POR_BMSK            0x100000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_PMUX_AUX_OUT_POR_SHFT                0x14
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_PMUX_DOM_OUT_POR_BMSK             0x80000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_PMUX_DOM_OUT_POR_SHFT                0x13
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_AUX_BMSK                          0x40000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_EN_AUX_SHFT                             0x12
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_FDBK_OUT_BMSK                        0x20000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_FDBK_OUT_SHFT                           0x11
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_ENR_BMSK                             0x10000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_BYPASS_TILE2APM_ENR_SHFT                                0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_DISABLE_PC2DOM_STEP_3_CNT_BMSK                        0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_DISABLE_PC2DOM_STEP_3_CNT_SHFT                           0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_PC2DOM_STEP_3_CNT_BMSK                                0x7f00
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_PC2DOM_STEP_3_CNT_SHFT                                   0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_DISABLE_PC2DOM_STEP_2_CNT_BMSK                          0x80
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_DISABLE_PC2DOM_STEP_2_CNT_SHFT                           0x7
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_PC2DOM_STEP_2_CNT_BMSK                                  0x7f
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_4_PC2DOM_STEP_2_CNT_SHFT                                   0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x0000001c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x0000001c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_BMSK             0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_SHFT                    0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000020)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000020)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_BMSK             0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_SHFT                    0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000024)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000024)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_RMSK                                               0x30183ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ISO_GEN_OBSER_RST_PX11_BMSK                        0x2000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ISO_GEN_OBSER_RST_PX11_SHFT                             0x19
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ISO_GEN_OBSER_RST_PX3_BMSK                         0x1000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_ISO_GEN_OBSER_RST_PX3_SHFT                              0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_CTRL2TILE_COMP_OUT_BYPASS_VAL_BMSK                   0x10000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_CTRL2TILE_COMP_OUT_BYPASS_VAL_SHFT                      0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_CTRL2TILE_COMP_OUT_BYPASS_BMSK                        0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_CTRL2TILE_COMP_OUT_BYPASS_SHFT                           0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_COMP_INST_HYST_TIMER_BMSK                              0x3ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_7_COMP_INST_HYST_TIMER_SHFT                                0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000028)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000028)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_BMSK             0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_SHFT                    0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x0000002c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x0000002c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_BMSK             0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_SHFT                    0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000030)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000030)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_RMSK                                                   0x3ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_RSVD_OUT_BMSK                                          0x3ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_OUT_RSVD_OUT_SHFT                                            0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000040)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000040)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_RMSK                                               0x3ffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_PMUX_AUX_OUT_POR_BMSK                 0x2000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_PMUX_AUX_OUT_POR_SHFT                      0x19
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_PMUX_DOM_OUT_POR_BMSK                 0x1000000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_PMUX_DOM_OUT_POR_SHFT                      0x18
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_SELF_TIMED_OUT_POR_BMSK                0x800000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_SELF_TIMED_OUT_POR_SHFT                    0x17
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_RST_USE_DOM_OUT_POR_BMSK                  0x400000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_RST_USE_DOM_OUT_POR_SHFT                      0x16
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_PMUX_AUX_POR_BMSK                      0x200000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_PMUX_AUX_POR_SHFT                          0x15
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_PMUX_DOM_POR_BMSK                      0x100000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_PMUX_DOM_POR_SHFT                          0x14
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_SELF_TIMED_POR_BMSK                     0x80000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_SELF_TIMED_POR_SHFT                        0x13
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_RST_USE_DOM_POR_BMSK                       0x40000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_RST_USE_DOM_POR_SHFT                          0x12
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_ENR_BMSK                                   0x20000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_ENR_SHFT                                      0x11
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_AUX_BMSK                                0x10000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_EN_AUX_SHFT                                   0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_FDBK_OUT_BMSK                               0x8000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_FDBK_OUT_SHFT                                  0xf
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_RST_FDBK_OUT_BMSK                           0x4000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_TILE2CTRL_RST_FDBK_OUT_SHFT                              0xe
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_AUX_BMSK                                 0x2000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_AUX_SHFT                                    0xd
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_AUX_SM_BMSK                              0x1000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_EN_AUX_SM_SHFT                                 0xc
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_ENR_BMSK                                     0x800
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_ENR_SHFT                                       0xb
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_ENR_SM_BMSK                                  0x400
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_CTRL2TILE_ENR_SM_SHFT                                    0xa
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_APM_NEXT_STATE_BMSK                                    0x3e0
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_APM_NEXT_STATE_SHFT                                      0x5
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_APM_CURR_STATE_BMSK                                     0x1f
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_0_APM_CURR_STATE_SHFT                                      0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000044)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000044)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_RMSK                                                  0x3fff
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_3_CNT_EXP_BMSK                            0x2000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_3_CNT_EXP_SHFT                               0xd
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_2_CNT_EXP_BMSK                            0x1000
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_2_CNT_EXP_SHFT                               0xc
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_1_CNT_EXP_BMSK                             0x800
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2DOM_STEP_1_CNT_EXP_SHFT                               0xb
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_3_CNT_EXP_BMSK                             0x400
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_3_CNT_EXP_SHFT                               0xa
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_2_CNT_EXP_BMSK                             0x200
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_2_CNT_EXP_SHFT                               0x9
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_1_CNT_EXP_BMSK                             0x100
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_PC2AUX_STEP_1_CNT_EXP_SHFT                               0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2PC_STEP_1_CNT_EXP_BMSK                              0x80
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2PC_STEP_1_CNT_EXP_SHFT                               0x7
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2PC_STEP_1_CNT_EXP_BMSK                              0x40
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2PC_STEP_1_CNT_EXP_SHFT                               0x6
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_3_CNT_EXP_BMSK                             0x20
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_3_CNT_EXP_SHFT                              0x5
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_2_CNT_EXP_BMSK                             0x10
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_2_CNT_EXP_SHFT                              0x4
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_1_CNT_EXP_BMSK                              0x8
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_AUX2DOM_STEP_1_CNT_EXP_SHFT                              0x3
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_3_CNT_EXP_BMSK                              0x4
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_3_CNT_EXP_SHFT                              0x2
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_2_CNT_EXP_BMSK                              0x2
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_2_CNT_EXP_SHFT                              0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_1_CNT_EXP_BMSK                              0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_1_DOM2AUX_STEP_1_CNT_EXP_SHFT                              0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x00000048)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000048)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_RMSK                                              0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_APM_STATUS_2_BMSK                                 0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_2_APM_STATUS_2_SHFT                                        0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ADDR                                              (VDD_AOSS_APM_REG_BASE      + 0x0000004c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_OFFS                                              (VDD_AOSS_APM_REG_BASE_OFFS + 0x0000004c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_RMSK                                                     0x3
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ISO_GEN_OBSER_OUT_PX11_BMSK                              0x2
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ISO_GEN_OBSER_OUT_PX11_SHFT                              0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ISO_GEN_OBSER_OUT_PX3_BMSK                               0x1
#define HWIO_VDD_AOSS_APM_CONTROLLER_STATUS_3_ISO_GEN_OBSER_OUT_PX3_SHFT                               0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_ADDR                                               (VDD_AOSS_APM_REG_BASE      + 0x00000050)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_OFFS                                               (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000050)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_RMSK                                                    0x3ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_RSVD_IN_BMSK                                            0x3ff
#define HWIO_VDD_AOSS_APM_CONTROLLER_RSVD_IN_RSVD_IN_SHFT                                              0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_ADDR                                            (VDD_AOSS_APM_REG_BASE      + 0x00000054)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_OFFS                                            (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000054)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_RMSK                                            0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_BMSK           0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_8_1_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_SHFT                  0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_ADDR                                            (VDD_AOSS_APM_REG_BASE      + 0x00000058)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_OFFS                                            (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000058)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_RMSK                                            0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_BMSK           0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_9_1_APM2TILE_AUX2DOM_FDBK_CONFIG_SEL_SHFT                  0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_ADDR                                            (VDD_AOSS_APM_REG_BASE      + 0x0000005c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_OFFS                                            (VDD_AOSS_APM_REG_BASE_OFFS + 0x0000005c)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_RMSK                                            0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_BMSK           0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_5_1_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_SHFT                  0x0

#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_ADDR                                            (VDD_AOSS_APM_REG_BASE      + 0x00000060)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_OFFS                                            (VDD_AOSS_APM_REG_BASE_OFFS + 0x00000060)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_RMSK                                            0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_IN          \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_ADDR, HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_RMSK)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_INM(m)      \
        in_dword_masked(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_ADDR, m)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_OUT(v)      \
        out_dword(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_ADDR,v)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_ADDR,m,v,HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_IN)
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_BMSK           0xffffffff
#define HWIO_VDD_AOSS_APM_CONTROLLER_CONFIG_6_1_APM2TILE_DOM2AUX_FDBK_CONFIG_SEL_SHFT                  0x0


#endif /* __AOSS_APM_H__ */
