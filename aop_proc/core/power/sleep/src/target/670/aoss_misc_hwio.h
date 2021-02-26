#ifndef __AOSS_MISC_HWIO_H__
#define __AOSS_MISC_HWIO_H__
/*
===========================================================================
*/
/**
  @file aoss_misc_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p3q1r51.1.4_MTO_partitioned_rtl]
 
  This file contains HWIO register definitions for the following modules:
    AOSS_MISC

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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/sleep/src/target/670/aoss_misc_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: AOSS_MISC
 *--------------------------------------------------------------------------*/

#define AOSS_MISC_REG_BASE                                                 (AOSS_BASE      + 0x01260000)
#define AOSS_MISC_REG_BASE_OFFS                                            0x01260000

#define HWIO_AOSS_PERIPHERALS_HW_VERSION_ADDR                              (AOSS_MISC_REG_BASE      + 0x00000000)
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_OFFS                              (AOSS_MISC_REG_BASE_OFFS + 0x00000000)
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_RMSK                                0xffffff
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_IN          \
        in_dword_masked(HWIO_AOSS_PERIPHERALS_HW_VERSION_ADDR, HWIO_AOSS_PERIPHERALS_HW_VERSION_RMSK)
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_AOSS_PERIPHERALS_HW_VERSION_ADDR, m)
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_MAJOR_BMSK                          0xff0000
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_MAJOR_SHFT                              0x10
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_MINOR_BMSK                            0xff00
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_MINOR_SHFT                               0x8
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_STEP_BMSK                               0xff
#define HWIO_AOSS_PERIPHERALS_HW_VERSION_STEP_SHFT                                0x0

#define HWIO_MPM_DEBUG_BUS_EN_ADDR                                         (AOSS_MISC_REG_BASE      + 0x00000004)
#define HWIO_MPM_DEBUG_BUS_EN_OFFS                                         (AOSS_MISC_REG_BASE_OFFS + 0x00000004)
#define HWIO_MPM_DEBUG_BUS_EN_RMSK                                                0x1
#define HWIO_MPM_DEBUG_BUS_EN_IN          \
        in_dword_masked(HWIO_MPM_DEBUG_BUS_EN_ADDR, HWIO_MPM_DEBUG_BUS_EN_RMSK)
#define HWIO_MPM_DEBUG_BUS_EN_INM(m)      \
        in_dword_masked(HWIO_MPM_DEBUG_BUS_EN_ADDR, m)
#define HWIO_MPM_DEBUG_BUS_EN_OUT(v)      \
        out_dword(HWIO_MPM_DEBUG_BUS_EN_ADDR,v)
#define HWIO_MPM_DEBUG_BUS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_DEBUG_BUS_EN_ADDR,m,v,HWIO_MPM_DEBUG_BUS_EN_IN)
#define HWIO_MPM_DEBUG_BUS_EN_DEBUG_EN_BMSK                                       0x1
#define HWIO_MPM_DEBUG_BUS_EN_DEBUG_EN_SHFT                                       0x0

#define HWIO_MPM_CNTR_CLK_CONTROL_ADDR                                     (AOSS_MISC_REG_BASE      + 0x0000000c)
#define HWIO_MPM_CNTR_CLK_CONTROL_OFFS                                     (AOSS_MISC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_MPM_CNTR_CLK_CONTROL_RMSK                                            0x7
#define HWIO_MPM_CNTR_CLK_CONTROL_IN          \
        in_dword_masked(HWIO_MPM_CNTR_CLK_CONTROL_ADDR, HWIO_MPM_CNTR_CLK_CONTROL_RMSK)
#define HWIO_MPM_CNTR_CLK_CONTROL_INM(m)      \
        in_dword_masked(HWIO_MPM_CNTR_CLK_CONTROL_ADDR, m)
#define HWIO_MPM_CNTR_CLK_CONTROL_OUT(v)      \
        out_dword(HWIO_MPM_CNTR_CLK_CONTROL_ADDR,v)
#define HWIO_MPM_CNTR_CLK_CONTROL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CNTR_CLK_CONTROL_ADDR,m,v,HWIO_MPM_CNTR_CLK_CONTROL_IN)
#define HWIO_MPM_CNTR_CLK_CONTROL_CLK_STATUS_BMSK                                 0x4
#define HWIO_MPM_CNTR_CLK_CONTROL_CLK_STATUS_SHFT                                 0x2
#define HWIO_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_BMSK                        0x2
#define HWIO_MPM_CNTR_CLK_CONTROL_SW_OVERWRITE_SWITCH_SHFT                        0x1
#define HWIO_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_BMSK                              0x1
#define HWIO_MPM_CNTR_CLK_CONTROL_SW_CLK_SWITCH_SHFT                              0x0

#define HWIO_MPM_CNTR_FRAC_ADDR                                            (AOSS_MISC_REG_BASE      + 0x00000010)
#define HWIO_MPM_CNTR_FRAC_OFFS                                            (AOSS_MISC_REG_BASE_OFFS + 0x00000010)
#define HWIO_MPM_CNTR_FRAC_RMSK                                                0xffff
#define HWIO_MPM_CNTR_FRAC_IN          \
        in_dword_masked(HWIO_MPM_CNTR_FRAC_ADDR, HWIO_MPM_CNTR_FRAC_RMSK)
#define HWIO_MPM_CNTR_FRAC_INM(m)      \
        in_dword_masked(HWIO_MPM_CNTR_FRAC_ADDR, m)
#define HWIO_MPM_CNTR_FRAC_OUT(v)      \
        out_dword(HWIO_MPM_CNTR_FRAC_ADDR,v)
#define HWIO_MPM_CNTR_FRAC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CNTR_FRAC_ADDR,m,v,HWIO_MPM_CNTR_FRAC_IN)
#define HWIO_MPM_CNTR_FRAC_LOAD_VAL_BMSK                                       0xffff
#define HWIO_MPM_CNTR_FRAC_LOAD_VAL_SHFT                                          0x0

#define HWIO_MPM_CNTR_INCR_DATA_ADDR                                       (AOSS_MISC_REG_BASE      + 0x00000014)
#define HWIO_MPM_CNTR_INCR_DATA_OFFS                                       (AOSS_MISC_REG_BASE_OFFS + 0x00000014)
#define HWIO_MPM_CNTR_INCR_DATA_RMSK                                       0xffffffff
#define HWIO_MPM_CNTR_INCR_DATA_IN          \
        in_dword_masked(HWIO_MPM_CNTR_INCR_DATA_ADDR, HWIO_MPM_CNTR_INCR_DATA_RMSK)
#define HWIO_MPM_CNTR_INCR_DATA_INM(m)      \
        in_dword_masked(HWIO_MPM_CNTR_INCR_DATA_ADDR, m)
#define HWIO_MPM_CNTR_INCR_DATA_OUT(v)      \
        out_dword(HWIO_MPM_CNTR_INCR_DATA_ADDR,v)
#define HWIO_MPM_CNTR_INCR_DATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_CNTR_INCR_DATA_ADDR,m,v,HWIO_MPM_CNTR_INCR_DATA_IN)
#define HWIO_MPM_CNTR_INCR_DATA_MULT_BMSK                                  0xffffffff
#define HWIO_MPM_CNTR_INCR_DATA_MULT_SHFT                                         0x0

#define HWIO_MPM_VREF_PWRSAVE_EN_ADDR                                      (AOSS_MISC_REG_BASE      + 0x00000018)
#define HWIO_MPM_VREF_PWRSAVE_EN_OFFS                                      (AOSS_MISC_REG_BASE_OFFS + 0x00000018)
#define HWIO_MPM_VREF_PWRSAVE_EN_RMSK                                             0x1
#define HWIO_MPM_VREF_PWRSAVE_EN_IN          \
        in_dword_masked(HWIO_MPM_VREF_PWRSAVE_EN_ADDR, HWIO_MPM_VREF_PWRSAVE_EN_RMSK)
#define HWIO_MPM_VREF_PWRSAVE_EN_INM(m)      \
        in_dword_masked(HWIO_MPM_VREF_PWRSAVE_EN_ADDR, m)
#define HWIO_MPM_VREF_PWRSAVE_EN_OUT(v)      \
        out_dword(HWIO_MPM_VREF_PWRSAVE_EN_ADDR,v)
#define HWIO_MPM_VREF_PWRSAVE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM_VREF_PWRSAVE_EN_ADDR,m,v,HWIO_MPM_VREF_PWRSAVE_EN_IN)
#define HWIO_MPM_VREF_PWRSAVE_EN_ENABLE_BMSK                                      0x1
#define HWIO_MPM_VREF_PWRSAVE_EN_ENABLE_SHFT                                      0x0

#define HWIO_ABT_TIMER_LOADVAL_ADDR                                        (AOSS_MISC_REG_BASE      + 0x00000100)
#define HWIO_ABT_TIMER_LOADVAL_OFFS                                        (AOSS_MISC_REG_BASE_OFFS + 0x00000100)
#define HWIO_ABT_TIMER_LOADVAL_RMSK                                              0xff
#define HWIO_ABT_TIMER_LOADVAL_IN          \
        in_dword_masked(HWIO_ABT_TIMER_LOADVAL_ADDR, HWIO_ABT_TIMER_LOADVAL_RMSK)
#define HWIO_ABT_TIMER_LOADVAL_INM(m)      \
        in_dword_masked(HWIO_ABT_TIMER_LOADVAL_ADDR, m)
#define HWIO_ABT_TIMER_LOADVAL_OUT(v)      \
        out_dword(HWIO_ABT_TIMER_LOADVAL_ADDR,v)
#define HWIO_ABT_TIMER_LOADVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ABT_TIMER_LOADVAL_ADDR,m,v,HWIO_ABT_TIMER_LOADVAL_IN)
#define HWIO_ABT_TIMER_LOADVAL_LOAD_VAL_BMSK                                     0xff
#define HWIO_ABT_TIMER_LOADVAL_LOAD_VAL_SHFT                                      0x0

#define HWIO_ABT_INTR_STATUS_ADDR                                          (AOSS_MISC_REG_BASE      + 0x00000104)
#define HWIO_ABT_INTR_STATUS_OFFS                                          (AOSS_MISC_REG_BASE_OFFS + 0x00000104)
#define HWIO_ABT_INTR_STATUS_RMSK                                                 0x1
#define HWIO_ABT_INTR_STATUS_IN          \
        in_dword_masked(HWIO_ABT_INTR_STATUS_ADDR, HWIO_ABT_INTR_STATUS_RMSK)
#define HWIO_ABT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_ABT_INTR_STATUS_ADDR, m)
#define HWIO_ABT_INTR_STATUS_INTR_STATUS_BMSK                                     0x1
#define HWIO_ABT_INTR_STATUS_INTR_STATUS_SHFT                                     0x0

#define HWIO_ABT_INTR_CLEAR_ADDR                                           (AOSS_MISC_REG_BASE      + 0x00000108)
#define HWIO_ABT_INTR_CLEAR_OFFS                                           (AOSS_MISC_REG_BASE_OFFS + 0x00000108)
#define HWIO_ABT_INTR_CLEAR_RMSK                                                  0x1
#define HWIO_ABT_INTR_CLEAR_OUT(v)      \
        out_dword(HWIO_ABT_INTR_CLEAR_ADDR,v)
#define HWIO_ABT_INTR_CLEAR_INTR_CLEAR_BMSK                                       0x1
#define HWIO_ABT_INTR_CLEAR_INTR_CLEAR_SHFT                                       0x0

#define HWIO_ABT_INTR_ENABLE_ADDR                                          (AOSS_MISC_REG_BASE      + 0x0000010c)
#define HWIO_ABT_INTR_ENABLE_OFFS                                          (AOSS_MISC_REG_BASE_OFFS + 0x0000010c)
#define HWIO_ABT_INTR_ENABLE_RMSK                                                 0x1
#define HWIO_ABT_INTR_ENABLE_IN          \
        in_dword_masked(HWIO_ABT_INTR_ENABLE_ADDR, HWIO_ABT_INTR_ENABLE_RMSK)
#define HWIO_ABT_INTR_ENABLE_INM(m)      \
        in_dword_masked(HWIO_ABT_INTR_ENABLE_ADDR, m)
#define HWIO_ABT_INTR_ENABLE_OUT(v)      \
        out_dword(HWIO_ABT_INTR_ENABLE_ADDR,v)
#define HWIO_ABT_INTR_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ABT_INTR_ENABLE_ADDR,m,v,HWIO_ABT_INTR_ENABLE_IN)
#define HWIO_ABT_INTR_ENABLE_INTR_EN_BMSK                                         0x1
#define HWIO_ABT_INTR_ENABLE_INTR_EN_SHFT                                         0x0

#define HWIO_ABT_SYND_ADDR0_ADDR                                           (AOSS_MISC_REG_BASE      + 0x00000110)
#define HWIO_ABT_SYND_ADDR0_OFFS                                           (AOSS_MISC_REG_BASE_OFFS + 0x00000110)
#define HWIO_ABT_SYND_ADDR0_RMSK                                           0xffffffff
#define HWIO_ABT_SYND_ADDR0_IN          \
        in_dword_masked(HWIO_ABT_SYND_ADDR0_ADDR, HWIO_ABT_SYND_ADDR0_RMSK)
#define HWIO_ABT_SYND_ADDR0_INM(m)      \
        in_dword_masked(HWIO_ABT_SYND_ADDR0_ADDR, m)
#define HWIO_ABT_SYND_ADDR0_SYND_ADDR0_BMSK                                0xffffffff
#define HWIO_ABT_SYND_ADDR0_SYND_ADDR0_SHFT                                       0x0

#define HWIO_ABT_SLAVE_MASK_CLR_ADDR                                       (AOSS_MISC_REG_BASE      + 0x00000114)
#define HWIO_ABT_SLAVE_MASK_CLR_OFFS                                       (AOSS_MISC_REG_BASE_OFFS + 0x00000114)
#define HWIO_ABT_SLAVE_MASK_CLR_RMSK                                              0x1
#define HWIO_ABT_SLAVE_MASK_CLR_IN          \
        in_dword_masked(HWIO_ABT_SLAVE_MASK_CLR_ADDR, HWIO_ABT_SLAVE_MASK_CLR_RMSK)
#define HWIO_ABT_SLAVE_MASK_CLR_INM(m)      \
        in_dword_masked(HWIO_ABT_SLAVE_MASK_CLR_ADDR, m)
#define HWIO_ABT_SLAVE_MASK_CLR_OUT(v)      \
        out_dword(HWIO_ABT_SLAVE_MASK_CLR_ADDR,v)
#define HWIO_ABT_SLAVE_MASK_CLR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_ABT_SLAVE_MASK_CLR_ADDR,m,v,HWIO_ABT_SLAVE_MASK_CLR_IN)
#define HWIO_ABT_SLAVE_MASK_CLR_MASK_CLEAR_BMSK                                   0x1
#define HWIO_ABT_SLAVE_MASK_CLR_MASK_CLEAR_SHFT                                   0x0

#define HWIO_ABT_SLAVE_MASK_STATUS_ADDR                                    (AOSS_MISC_REG_BASE      + 0x00000118)
#define HWIO_ABT_SLAVE_MASK_STATUS_OFFS                                    (AOSS_MISC_REG_BASE_OFFS + 0x00000118)
#define HWIO_ABT_SLAVE_MASK_STATUS_RMSK                                          0x3f
#define HWIO_ABT_SLAVE_MASK_STATUS_IN          \
        in_dword_masked(HWIO_ABT_SLAVE_MASK_STATUS_ADDR, HWIO_ABT_SLAVE_MASK_STATUS_RMSK)
#define HWIO_ABT_SLAVE_MASK_STATUS_INM(m)      \
        in_dword_masked(HWIO_ABT_SLAVE_MASK_STATUS_ADDR, m)
#define HWIO_ABT_SLAVE_MASK_STATUS_MASK_BMSK                                     0x3f
#define HWIO_ABT_SLAVE_MASK_STATUS_MASK_SHFT                                      0x0

#define HWIO_AOSS_TEST_EN_ADDR                                             (AOSS_MISC_REG_BASE      + 0x00000200)
#define HWIO_AOSS_TEST_EN_OFFS                                             (AOSS_MISC_REG_BASE_OFFS + 0x00000200)
#define HWIO_AOSS_TEST_EN_RMSK                                                   0xff
#define HWIO_AOSS_TEST_EN_IN          \
        in_dword_masked(HWIO_AOSS_TEST_EN_ADDR, HWIO_AOSS_TEST_EN_RMSK)
#define HWIO_AOSS_TEST_EN_INM(m)      \
        in_dword_masked(HWIO_AOSS_TEST_EN_ADDR, m)
#define HWIO_AOSS_TEST_EN_OUT(v)      \
        out_dword(HWIO_AOSS_TEST_EN_ADDR,v)
#define HWIO_AOSS_TEST_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOSS_TEST_EN_ADDR,m,v,HWIO_AOSS_TEST_EN_IN)
#define HWIO_AOSS_TEST_EN_AOP_BRIDGE_TEST_EN_BMSK                                0xc0
#define HWIO_AOSS_TEST_EN_AOP_BRIDGE_TEST_EN_SHFT                                 0x6
#define HWIO_AOSS_TEST_EN_AOP_MST_NOC_MASTER_BRIDGE_TEST_EN_BMSK                 0x20
#define HWIO_AOSS_TEST_EN_AOP_MST_NOC_MASTER_BRIDGE_TEST_EN_SHFT                  0x5
#define HWIO_AOSS_TEST_EN_AOP_MST_NOC_SLAVE_BRIDGE_TEST_EN_BMSK                  0x10
#define HWIO_AOSS_TEST_EN_AOP_MST_NOC_SLAVE_BRIDGE_TEST_EN_SHFT                   0x4
#define HWIO_AOSS_TEST_EN_NOC_MST_AOSS_MASTER_BRIDGE_TEST_EN_BMSK                 0x8
#define HWIO_AOSS_TEST_EN_NOC_MST_AOSS_MASTER_BRIDGE_TEST_EN_SHFT                 0x3
#define HWIO_AOSS_TEST_EN_NOC_MST_AOSS_SLAVE_BRIDGE_TEST_EN_BMSK                  0x4
#define HWIO_AOSS_TEST_EN_NOC_MST_AOSS_SLAVE_BRIDGE_TEST_EN_SHFT                  0x2
#define HWIO_AOSS_TEST_EN_NOC_MST_AOP_MASTER_BRIDGE_TEST_EN_BMSK                  0x2
#define HWIO_AOSS_TEST_EN_NOC_MST_AOP_MASTER_BRIDGE_TEST_EN_SHFT                  0x1
#define HWIO_AOSS_TEST_EN_NOC_MST_AOP_SLAVE_BRIDGE_TEST_EN_BMSK                   0x1
#define HWIO_AOSS_TEST_EN_NOC_MST_AOP_SLAVE_BRIDGE_TEST_EN_SHFT                   0x0

#define HWIO_RPMH_SCANDUMP_DISABLE_ADDR                                    (AOSS_MISC_REG_BASE      + 0x00000204)
#define HWIO_RPMH_SCANDUMP_DISABLE_OFFS                                    (AOSS_MISC_REG_BASE_OFFS + 0x00000204)
#define HWIO_RPMH_SCANDUMP_DISABLE_RMSK                                           0x1
#define HWIO_RPMH_SCANDUMP_DISABLE_IN          \
        in_dword_masked(HWIO_RPMH_SCANDUMP_DISABLE_ADDR, HWIO_RPMH_SCANDUMP_DISABLE_RMSK)
#define HWIO_RPMH_SCANDUMP_DISABLE_INM(m)      \
        in_dword_masked(HWIO_RPMH_SCANDUMP_DISABLE_ADDR, m)
#define HWIO_RPMH_SCANDUMP_DISABLE_OUT(v)      \
        out_dword(HWIO_RPMH_SCANDUMP_DISABLE_ADDR,v)
#define HWIO_RPMH_SCANDUMP_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RPMH_SCANDUMP_DISABLE_ADDR,m,v,HWIO_RPMH_SCANDUMP_DISABLE_IN)
#define HWIO_RPMH_SCANDUMP_DISABLE_SCANDUMP_DISABLE_BMSK                          0x1
#define HWIO_RPMH_SCANDUMP_DISABLE_SCANDUMP_DISABLE_SHFT                          0x0

#define HWIO_APM_SW_OVERRIDE_ADDR                                          (AOSS_MISC_REG_BASE      + 0x00000300)
#define HWIO_APM_SW_OVERRIDE_OFFS                                          (AOSS_MISC_REG_BASE_OFFS + 0x00000300)
#define HWIO_APM_SW_OVERRIDE_RMSK                                                 0x7
#define HWIO_APM_SW_OVERRIDE_IN          \
        in_dword_masked(HWIO_APM_SW_OVERRIDE_ADDR, HWIO_APM_SW_OVERRIDE_RMSK)
#define HWIO_APM_SW_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_APM_SW_OVERRIDE_ADDR, m)
#define HWIO_APM_SW_OVERRIDE_OUT(v)      \
        out_dword(HWIO_APM_SW_OVERRIDE_ADDR,v)
#define HWIO_APM_SW_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APM_SW_OVERRIDE_ADDR,m,v,HWIO_APM_SW_OVERRIDE_IN)
#define HWIO_APM_SW_OVERRIDE_VDD_SSCMX_APM_CTRL_SEL_BMSK                          0x4
#define HWIO_APM_SW_OVERRIDE_VDD_SSCMX_APM_CTRL_SEL_SHFT                          0x2
#define HWIO_APM_SW_OVERRIDE_VDD_CX_APM_CTRL_SEL_BMSK                             0x2
#define HWIO_APM_SW_OVERRIDE_VDD_CX_APM_CTRL_SEL_SHFT                             0x1
#define HWIO_APM_SW_OVERRIDE_VDD_AOSS_APM_CTRL_SEL_BMSK                           0x1
#define HWIO_APM_SW_OVERRIDE_VDD_AOSS_APM_CTRL_SEL_SHFT                           0x0


#endif /* __AOSS_MISC_HWIO_H__ */
