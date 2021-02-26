#ifndef __HWIO_DDRAUX_DDRAUX_H__
#define __HWIO_DDRAUX_DDRAUX_H__
/*
===========================================================================
*/
/**
  @file hwio_ddraux_ddraux.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM845 (Napali) [napali_v1.0_p1q0r28.2.0]
 
  This file contains HWIO register definitions for the following modules:
    RPMH_DDRAUX_DDR_AUX

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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/ddr_mgr/src/target/670/hwio_ddraux_ddraux.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: DDR_AUX
 *--------------------------------------------------------------------------*/

#define DDR_AUX_REG_BASE                                           (AOSS_BASE      + 0x00c00700)

#define HWIO_VERSION_ADDR                                          (DDR_AUX_REG_BASE      + 0x00000000)
#define HWIO_VERSION_RMSK                                            0xffffff
#define HWIO_VERSION_IN          \
        in_dword_masked(HWIO_VERSION_ADDR, HWIO_VERSION_RMSK)
#define HWIO_VERSION_INM(m)      \
        in_dword_masked(HWIO_VERSION_ADDR, m)
#define HWIO_VERSION_MAJOR_VER_BMSK                                  0xff0000
#define HWIO_VERSION_MAJOR_VER_SHFT                                      0x10
#define HWIO_VERSION_MINOR_VER_BMSK                                    0xff00
#define HWIO_VERSION_MINOR_VER_SHFT                                       0x8
#define HWIO_VERSION_STEP_VER_BMSK                                       0xff
#define HWIO_VERSION_STEP_VER_SHFT                                        0x0

#define HWIO_DDR_AUX_HW_ENABLE_ADDR                                (DDR_AUX_REG_BASE      + 0x00000004)
#define HWIO_DDR_AUX_HW_ENABLE_RMSK                                       0x1
#define HWIO_DDR_AUX_HW_ENABLE_IN          \
        in_dword_masked(HWIO_DDR_AUX_HW_ENABLE_ADDR, HWIO_DDR_AUX_HW_ENABLE_RMSK)
#define HWIO_DDR_AUX_HW_ENABLE_INM(m)      \
        in_dword_masked(HWIO_DDR_AUX_HW_ENABLE_ADDR, m)
#define HWIO_DDR_AUX_HW_ENABLE_OUT(v)      \
        out_dword(HWIO_DDR_AUX_HW_ENABLE_ADDR,v)
#define HWIO_DDR_AUX_HW_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DDR_AUX_HW_ENABLE_ADDR,m,v,HWIO_DDR_AUX_HW_ENABLE_IN)
#define HWIO_DDR_AUX_HW_ENABLE_ENABLE_BMSK                                0x1
#define HWIO_DDR_AUX_HW_ENABLE_ENABLE_SHFT                                0x0

#define HWIO_AOP_INTR_STAT_ADDR                                    (DDR_AUX_REG_BASE      + 0x00000008)
#define HWIO_AOP_INTR_STAT_RMSK                                          0xff
#define HWIO_AOP_INTR_STAT_IN          \
        in_dword_masked(HWIO_AOP_INTR_STAT_ADDR, HWIO_AOP_INTR_STAT_RMSK)
#define HWIO_AOP_INTR_STAT_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_STAT_ADDR, m)
#define HWIO_AOP_INTR_STAT_INTR_BMSK                                     0xff
#define HWIO_AOP_INTR_STAT_INTR_SHFT                                      0x0

#define HWIO_AOP_INTR_DETAIL_ADDR                                  (DDR_AUX_REG_BASE      + 0x0000000c)
#define HWIO_AOP_INTR_DETAIL_RMSK                                  0xffffffff
#define HWIO_AOP_INTR_DETAIL_IN          \
        in_dword_masked(HWIO_AOP_INTR_DETAIL_ADDR, HWIO_AOP_INTR_DETAIL_RMSK)
#define HWIO_AOP_INTR_DETAIL_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_DETAIL_ADDR, m)
#define HWIO_AOP_INTR_DETAIL_INTR_DETAIL_BMSK                      0xffffffff
#define HWIO_AOP_INTR_DETAIL_INTR_DETAIL_SHFT                             0x0

#define HWIO_AOP_INTR_MASK_ADDR                                    (DDR_AUX_REG_BASE      + 0x00000010)
#define HWIO_AOP_INTR_MASK_RMSK                                          0xff
#define HWIO_AOP_INTR_MASK_IN          \
        in_dword_masked(HWIO_AOP_INTR_MASK_ADDR, HWIO_AOP_INTR_MASK_RMSK)
#define HWIO_AOP_INTR_MASK_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_MASK_ADDR, m)
#define HWIO_AOP_INTR_MASK_OUT(v)      \
        out_dword(HWIO_AOP_INTR_MASK_ADDR,v)
#define HWIO_AOP_INTR_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_MASK_ADDR,m,v,HWIO_AOP_INTR_MASK_IN)
#define HWIO_AOP_INTR_MASK_INTR_MASK_BMSK                                0xff
#define HWIO_AOP_INTR_MASK_INTR_MASK_SHFT                                 0x0
#define HWIO_AOP_INTR_MASK_INTR_SEQ0_BMSK                                0x01
#define HWIO_AOP_INTR_MASK_INTR_SEQ0_SHFT                                 0x0
#define HWIO_AOP_INTR_MASK_INTR_SEQ1_BMSK                                0x02
#define HWIO_AOP_INTR_MASK_INTR_SEQ1_SHFT                                 0x1
#define HWIO_AOP_INTR_MASK_INTR_TCS_DONE_BMSK                            0x04
#define HWIO_AOP_INTR_MASK_INTR_TCS_DONE_SHFT                             0x2
#define HWIO_AOP_INTR_MASK_INTR_SEQ_PC_BMSK                              0x08
#define HWIO_AOP_INTR_MASK_INTR_SEQ_PC_SHFT                               0x3
#define HWIO_AOP_INTR_MASK_INTR_SHRM_GPW_BMSK                            0x10
#define HWIO_AOP_INTR_MASK_INTR_SHRM_GPW_SHFT                             0x4
#define HWIO_AOP_INTR_MASK_INTR_SUMMARY_BMSK                             0x80
#define HWIO_AOP_INTR_MASK_INTR_SUMMARY_SHFT                              0x7

#define HWIO_AOP_INTR_CLR_ADDR                                     (DDR_AUX_REG_BASE      + 0x00000014)
#define HWIO_AOP_INTR_CLR_RMSK                                           0xff
#define HWIO_AOP_INTR_CLR_OUT(v)      \
        out_dword(HWIO_AOP_INTR_CLR_ADDR,v)
#define HWIO_AOP_INTR_CLR_INTR_CLR_BMSK                                  0xff
#define HWIO_AOP_INTR_CLR_INTR_CLR_SHFT                                   0x0

#define HWIO_ARC_INTF_PERF_LVL_ADDR                                (DDR_AUX_REG_BASE      + 0x00000018)
#define HWIO_ARC_INTF_PERF_LVL_RMSK                                     0xfff
#define HWIO_ARC_INTF_PERF_LVL_IN          \
        in_dword_masked(HWIO_ARC_INTF_PERF_LVL_ADDR, HWIO_ARC_INTF_PERF_LVL_RMSK)
#define HWIO_ARC_INTF_PERF_LVL_INM(m)      \
        in_dword_masked(HWIO_ARC_INTF_PERF_LVL_ADDR, m)
#define HWIO_ARC_INTF_PERF_LVL_PERF_LVL_BMSK                            0xfff
#define HWIO_ARC_INTF_PERF_LVL_PERF_LVL_SHFT                              0x0

#define HWIO_SHRM_RSI_GPW_DATA_ADDR                                (DDR_AUX_REG_BASE      + 0x0000001c)
#define HWIO_SHRM_RSI_GPW_DATA_RMSK                                    0xffff
#define HWIO_SHRM_RSI_GPW_DATA_IN          \
        in_dword_masked(HWIO_SHRM_RSI_GPW_DATA_ADDR, HWIO_SHRM_RSI_GPW_DATA_RMSK)
#define HWIO_SHRM_RSI_GPW_DATA_INM(m)      \
        in_dword_masked(HWIO_SHRM_RSI_GPW_DATA_ADDR, m)
#define HWIO_SHRM_RSI_GPW_DATA_DATA_BMSK                               0xffff
#define HWIO_SHRM_RSI_GPW_DATA_DATA_SHFT                                  0x0

#define HWIO_GCC_INTF_STAT_ADDR                                    (DDR_AUX_REG_BASE      + 0x00000020)
#define HWIO_GCC_INTF_STAT_RMSK                                    0x3fffffff
#define HWIO_GCC_INTF_STAT_IN          \
        in_dword_masked(HWIO_GCC_INTF_STAT_ADDR, HWIO_GCC_INTF_STAT_RMSK)
#define HWIO_GCC_INTF_STAT_INM(m)      \
        in_dword_masked(HWIO_GCC_INTF_STAT_ADDR, m)
#define HWIO_GCC_INTF_STAT_PWR_CLPS_ACK_BMSK                       0x38000000
#define HWIO_GCC_INTF_STAT_PWR_CLPS_ACK_SHFT                             0x1b
#define HWIO_GCC_INTF_STAT_DISABLE_ACK_BMSK                         0x7000000
#define HWIO_GCC_INTF_STAT_DISABLE_ACK_SHFT                              0x18
#define HWIO_GCC_INTF_STAT_POSTWORK_ACK_BMSK                         0xe00000
#define HWIO_GCC_INTF_STAT_POSTWORK_ACK_SHFT                             0x15
#define HWIO_GCC_INTF_STAT_CHANGE_ACK_BMSK                           0x1c0000
#define HWIO_GCC_INTF_STAT_CHANGE_ACK_SHFT                               0x12
#define HWIO_GCC_INTF_STAT_PREWORK_ACK_BMSK                           0x38000
#define HWIO_GCC_INTF_STAT_PREWORK_ACK_SHFT                               0xf
#define HWIO_GCC_INTF_STAT_PWR_CLPS_REQ_BMSK                           0x7000
#define HWIO_GCC_INTF_STAT_PWR_CLPS_REQ_SHFT                              0xc
#define HWIO_GCC_INTF_STAT_DISABLE_REQ_BMSK                             0xe00
#define HWIO_GCC_INTF_STAT_DISABLE_REQ_SHFT                               0x9
#define HWIO_GCC_INTF_STAT_POSTWORK_REQ_BMSK                            0x1c0
#define HWIO_GCC_INTF_STAT_POSTWORK_REQ_SHFT                              0x6
#define HWIO_GCC_INTF_STAT_CHANGE_REQ_BMSK                               0x38
#define HWIO_GCC_INTF_STAT_CHANGE_REQ_SHFT                                0x3
#define HWIO_GCC_INTF_STAT_PREWORK_REQ_BMSK                               0x7
#define HWIO_GCC_INTF_STAT_PREWORK_REQ_SHFT                               0x0

#define HWIO_ARC_INTF_STAT_ADDR                                    (DDR_AUX_REG_BASE      + 0x00000024)
#define HWIO_ARC_INTF_STAT_RMSK                                       0x3ffff
#define HWIO_ARC_INTF_STAT_IN          \
        in_dword_masked(HWIO_ARC_INTF_STAT_ADDR, HWIO_ARC_INTF_STAT_RMSK)
#define HWIO_ARC_INTF_STAT_INM(m)      \
        in_dword_masked(HWIO_ARC_INTF_STAT_ADDR, m)
#define HWIO_ARC_INTF_STAT_POSTWORK_ACK_BMSK                          0x38000
#define HWIO_ARC_INTF_STAT_POSTWORK_ACK_SHFT                              0xf
#define HWIO_ARC_INTF_STAT_CHANGE_ACK_BMSK                             0x7000
#define HWIO_ARC_INTF_STAT_CHANGE_ACK_SHFT                                0xc
#define HWIO_ARC_INTF_STAT_PREWORK_ACK_BMSK                             0xe00
#define HWIO_ARC_INTF_STAT_PREWORK_ACK_SHFT                               0x9
#define HWIO_ARC_INTF_STAT_POSTWORK_REQ_BMSK                            0x1c0
#define HWIO_ARC_INTF_STAT_POSTWORK_REQ_SHFT                              0x6
#define HWIO_ARC_INTF_STAT_CHANGE_REQ_BMSK                               0x38
#define HWIO_ARC_INTF_STAT_CHANGE_REQ_SHFT                                0x3
#define HWIO_ARC_INTF_STAT_PREWORK_REQ_BMSK                               0x7
#define HWIO_ARC_INTF_STAT_PREWORK_REQ_SHFT                               0x0

#define HWIO_BCM_INTF_STAT_ADDR                                    (DDR_AUX_REG_BASE      + 0x00000028)
#define HWIO_BCM_INTF_STAT_RMSK                                    0x3fffffff
#define HWIO_BCM_INTF_STAT_IN          \
        in_dword_masked(HWIO_BCM_INTF_STAT_ADDR, HWIO_BCM_INTF_STAT_RMSK)
#define HWIO_BCM_INTF_STAT_INM(m)      \
        in_dword_masked(HWIO_BCM_INTF_STAT_ADDR, m)
#define HWIO_BCM_INTF_STAT_PWR_CLPS_ACK_BMSK                       0x38000000
#define HWIO_BCM_INTF_STAT_PWR_CLPS_ACK_SHFT                             0x1b
#define HWIO_BCM_INTF_STAT_DISABLE_ACK_BMSK                         0x7000000
#define HWIO_BCM_INTF_STAT_DISABLE_ACK_SHFT                              0x18
#define HWIO_BCM_INTF_STAT_POSTWORK_ACK_BMSK                         0xe00000
#define HWIO_BCM_INTF_STAT_POSTWORK_ACK_SHFT                             0x15
#define HWIO_BCM_INTF_STAT_CHANGE_ACK_BMSK                           0x1c0000
#define HWIO_BCM_INTF_STAT_CHANGE_ACK_SHFT                               0x12
#define HWIO_BCM_INTF_STAT_PREWORK_ACK_BMSK                           0x38000
#define HWIO_BCM_INTF_STAT_PREWORK_ACK_SHFT                               0xf
#define HWIO_BCM_INTF_STAT_PWR_CLPS_REQ_BMSK                           0x7000
#define HWIO_BCM_INTF_STAT_PWR_CLPS_REQ_SHFT                              0xc
#define HWIO_BCM_INTF_STAT_DISABLE_REQ_BMSK                             0xe00
#define HWIO_BCM_INTF_STAT_DISABLE_REQ_SHFT                               0x9
#define HWIO_BCM_INTF_STAT_POSTWORK_REQ_BMSK                            0x1c0
#define HWIO_BCM_INTF_STAT_POSTWORK_REQ_SHFT                              0x6
#define HWIO_BCM_INTF_STAT_CHANGE_REQ_BMSK                               0x38
#define HWIO_BCM_INTF_STAT_CHANGE_REQ_SHFT                                0x3
#define HWIO_BCM_INTF_STAT_PREWORK_REQ_BMSK                               0x7
#define HWIO_BCM_INTF_STAT_PREWORK_REQ_SHFT                               0x0

#define HWIO_BCM_INTF_CLKPLAN_ADDR                                 (DDR_AUX_REG_BASE      + 0x0000002c)
#define HWIO_BCM_INTF_CLKPLAN_RMSK                                      0xfff
#define HWIO_BCM_INTF_CLKPLAN_IN          \
        in_dword_masked(HWIO_BCM_INTF_CLKPLAN_ADDR, HWIO_BCM_INTF_CLKPLAN_RMSK)
#define HWIO_BCM_INTF_CLKPLAN_INM(m)      \
        in_dword_masked(HWIO_BCM_INTF_CLKPLAN_ADDR, m)
#define HWIO_BCM_INTF_CLKPLAN_CLKPLAN_BMSK                              0xfff
#define HWIO_BCM_INTF_CLKPLAN_CLKPLAN_SHFT                                0x0

#define HWIO_RSI_TIMEOUT_ADDR                                      (DDR_AUX_REG_BASE      + 0x00000030)
#define HWIO_RSI_TIMEOUT_RMSK                                      0x80fffff3
#define HWIO_RSI_TIMEOUT_IN          \
        in_dword_masked(HWIO_RSI_TIMEOUT_ADDR, HWIO_RSI_TIMEOUT_RMSK)
#define HWIO_RSI_TIMEOUT_INM(m)      \
        in_dword_masked(HWIO_RSI_TIMEOUT_ADDR, m)
#define HWIO_RSI_TIMEOUT_OUT(v)      \
        out_dword(HWIO_RSI_TIMEOUT_ADDR,v)
#define HWIO_RSI_TIMEOUT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_RSI_TIMEOUT_ADDR,m,v,HWIO_RSI_TIMEOUT_IN)
#define HWIO_RSI_TIMEOUT_TIMEOUT_STAT_BMSK                         0x80000000
#define HWIO_RSI_TIMEOUT_TIMEOUT_STAT_SHFT                               0x1f
#define HWIO_RSI_TIMEOUT_THRESHOLD_BMSK                              0xfffff0
#define HWIO_RSI_TIMEOUT_THRESHOLD_SHFT                                   0x4
#define HWIO_RSI_TIMEOUT_CLR_TIMEOUT_BMSK                                 0x2
#define HWIO_RSI_TIMEOUT_CLR_TIMEOUT_SHFT                                 0x1
#define HWIO_RSI_TIMEOUT_ENABLE_BMSK                                      0x1
#define HWIO_RSI_TIMEOUT_ENABLE_SHFT                                      0x0

#define HWIO_DEBUG_GPIO_SRC_SEL_ADDR                               (DDR_AUX_REG_BASE      + 0x00000034)
#define HWIO_DEBUG_GPIO_SRC_SEL_RMSK                                      0x7
#define HWIO_DEBUG_GPIO_SRC_SEL_IN          \
        in_dword_masked(HWIO_DEBUG_GPIO_SRC_SEL_ADDR, HWIO_DEBUG_GPIO_SRC_SEL_RMSK)
#define HWIO_DEBUG_GPIO_SRC_SEL_INM(m)      \
        in_dword_masked(HWIO_DEBUG_GPIO_SRC_SEL_ADDR, m)
#define HWIO_DEBUG_GPIO_SRC_SEL_OUT(v)      \
        out_dword(HWIO_DEBUG_GPIO_SRC_SEL_ADDR,v)
#define HWIO_DEBUG_GPIO_SRC_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEBUG_GPIO_SRC_SEL_ADDR,m,v,HWIO_DEBUG_GPIO_SRC_SEL_IN)
#define HWIO_DEBUG_GPIO_SRC_SEL_MODE_BMSK                                 0x7
#define HWIO_DEBUG_GPIO_SRC_SEL_MODE_SHFT                                 0x0

#define HWIO_DEBUG_CLKON_ADDR                                      (DDR_AUX_REG_BASE      + 0x00000038)
#define HWIO_DEBUG_CLKON_RMSK                                             0x1
#define HWIO_DEBUG_CLKON_IN          \
        in_dword_masked(HWIO_DEBUG_CLKON_ADDR, HWIO_DEBUG_CLKON_RMSK)
#define HWIO_DEBUG_CLKON_INM(m)      \
        in_dword_masked(HWIO_DEBUG_CLKON_ADDR, m)
#define HWIO_DEBUG_CLKON_OUT(v)      \
        out_dword(HWIO_DEBUG_CLKON_ADDR,v)
#define HWIO_DEBUG_CLKON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEBUG_CLKON_ADDR,m,v,HWIO_DEBUG_CLKON_IN)
#define HWIO_DEBUG_CLKON_CLKON_BMSK                                       0x1
#define HWIO_DEBUG_CLKON_CLKON_SHFT                                       0x0


#endif /* __HWIO_DDRAUX_DDRAUX_H__ */
