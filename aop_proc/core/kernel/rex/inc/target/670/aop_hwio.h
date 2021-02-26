#ifndef __AOP_HWIO_H__
#define __AOP_HWIO_H__
/*
===========================================================================
*/
/**
  @file aop_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p2q1r81]
 
  This file contains HWIO register definitions for the following modules:
    AOP_DEC
    AOP_F0_QTMR_V1_F0
    AOP_F1_QTMR_V1_F1
    AOP_QTMR_AC

  'Include' filters applied: 
  'Exclude' filters applied: DUMMY RESERVED 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/inc/target/670/aop_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: AOP_DEC
 *--------------------------------------------------------------------------*/

#define AOP_DEC_REG_BASE                                                    (AOSS_BASE      + 0x00080000)
#define AOP_DEC_REG_BASE_PHYS                                               (AOSS_BASE_PHYS + 0x00080000)
#define AOP_DEC_REG_BASE_OFFS                                               0x00080000

#define HWIO_AOP_HW_VERSION_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000000)
#define HWIO_AOP_HW_VERSION_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000000)
#define HWIO_AOP_HW_VERSION_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000000)
#define HWIO_AOP_HW_VERSION_RMSK                                            0xffffffff
#define HWIO_AOP_HW_VERSION_IN          \
        in_dword_masked(HWIO_AOP_HW_VERSION_ADDR, HWIO_AOP_HW_VERSION_RMSK)
#define HWIO_AOP_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_AOP_HW_VERSION_ADDR, m)
#define HWIO_AOP_HW_VERSION_MAJOR_BMSK                                      0xf0000000
#define HWIO_AOP_HW_VERSION_MAJOR_SHFT                                            0x1c
#define HWIO_AOP_HW_VERSION_MINOR_BMSK                                       0xfff0000
#define HWIO_AOP_HW_VERSION_MINOR_SHFT                                            0x10
#define HWIO_AOP_HW_VERSION_STEP_BMSK                                           0xffff
#define HWIO_AOP_HW_VERSION_STEP_SHFT                                              0x0

#define HWIO_AOP_WFI_CONFIG_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000004)
#define HWIO_AOP_WFI_CONFIG_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000004)
#define HWIO_AOP_WFI_CONFIG_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000004)
#define HWIO_AOP_WFI_CONFIG_RMSK                                                   0x7
#define HWIO_AOP_WFI_CONFIG_IN          \
        in_dword_masked(HWIO_AOP_WFI_CONFIG_ADDR, HWIO_AOP_WFI_CONFIG_RMSK)
#define HWIO_AOP_WFI_CONFIG_INM(m)      \
        in_dword_masked(HWIO_AOP_WFI_CONFIG_ADDR, m)
#define HWIO_AOP_WFI_CONFIG_OUT(v)      \
        out_dword(HWIO_AOP_WFI_CONFIG_ADDR,v)
#define HWIO_AOP_WFI_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WFI_CONFIG_ADDR,m,v,HWIO_AOP_WFI_CONFIG_IN)
#define HWIO_AOP_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_BMSK                               0x4
#define HWIO_AOP_WFI_CONFIG_CHIP_SLEEP_UPON_WFI_SHFT                               0x2
#define HWIO_AOP_WFI_CONFIG_BUS_CLK_HALT_BMSK                                      0x2
#define HWIO_AOP_WFI_CONFIG_BUS_CLK_HALT_SHFT                                      0x1
#define HWIO_AOP_WFI_CONFIG_PROC_CLK_HALT_BMSK                                     0x1
#define HWIO_AOP_WFI_CONFIG_PROC_CLK_HALT_SHFT                                     0x0

#define HWIO_AOP_TIMERS_CLK_OFF_CTL_ADDR                                    (AOP_DEC_REG_BASE      + 0x00000008)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_PHYS                                    (AOP_DEC_REG_BASE_PHYS + 0x00000008)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_OFFS                                    (AOP_DEC_REG_BASE_OFFS + 0x00000008)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_RMSK                                           0x1
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_IN          \
        in_dword_masked(HWIO_AOP_TIMERS_CLK_OFF_CTL_ADDR, HWIO_AOP_TIMERS_CLK_OFF_CTL_RMSK)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_INM(m)      \
        in_dword_masked(HWIO_AOP_TIMERS_CLK_OFF_CTL_ADDR, m)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_OUT(v)      \
        out_dword(HWIO_AOP_TIMERS_CLK_OFF_CTL_ADDR,v)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_TIMERS_CLK_OFF_CTL_ADDR,m,v,HWIO_AOP_TIMERS_CLK_OFF_CTL_IN)
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_BMSK                        0x1
#define HWIO_AOP_TIMERS_CLK_OFF_CTL_WDOG_TIMER_CLK_OFF_SHFT                        0x0

#define HWIO_AOP_IPC_ADDR                                                   (AOP_DEC_REG_BASE      + 0x0000000c)
#define HWIO_AOP_IPC_PHYS                                                   (AOP_DEC_REG_BASE_PHYS + 0x0000000c)
#define HWIO_AOP_IPC_OFFS                                                   (AOP_DEC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_AOP_IPC_RMSK                                                      0xfffff
#define HWIO_AOP_IPC_OUT(v)      \
        out_dword(HWIO_AOP_IPC_ADDR,v)
#define HWIO_AOP_IPC_CDSP_IPC_BMSK                                             0xc0000
#define HWIO_AOP_IPC_CDSP_IPC_SHFT                                                0x12
#define HWIO_AOP_IPC_CAMSS_IPC_BMSK                                            0x30000
#define HWIO_AOP_IPC_CAMSS_IPC_SHFT                                               0x10
#define HWIO_AOP_IPC_GFX_IPC_BMSK                                               0xc000
#define HWIO_AOP_IPC_GFX_IPC_SHFT                                                  0xe
#define HWIO_AOP_IPC_SPSS_IPC_BMSK                                              0x3000
#define HWIO_AOP_IPC_SPSS_IPC_SHFT                                                 0xc
#define HWIO_AOP_IPC_MSS_IPC_BMSK                                                0xc00
#define HWIO_AOP_IPC_MSS_IPC_SHFT                                                  0xa
#define HWIO_AOP_IPC_SSC_IPC_BMSK                                                0x300
#define HWIO_AOP_IPC_SSC_IPC_SHFT                                                  0x8
#define HWIO_AOP_IPC_LPASS_IPC_BMSK                                               0xc0
#define HWIO_AOP_IPC_LPASS_IPC_SHFT                                                0x6
#define HWIO_AOP_IPC_APCS_HV_IPC_BMSK                                             0x30
#define HWIO_AOP_IPC_APCS_HV_IPC_SHFT                                              0x4
#define HWIO_AOP_IPC_APCS_TZ_IPC_BMSK                                              0xc
#define HWIO_AOP_IPC_APCS_TZ_IPC_SHFT                                              0x2
#define HWIO_AOP_IPC_APCS_HLOS_IPC_BMSK                                            0x3
#define HWIO_AOP_IPC_APCS_HLOS_IPC_SHFT                                            0x0

#define HWIO_AOP_GPO_WDATA_ADDR                                             (AOP_DEC_REG_BASE      + 0x00000010)
#define HWIO_AOP_GPO_WDATA_PHYS                                             (AOP_DEC_REG_BASE_PHYS + 0x00000010)
#define HWIO_AOP_GPO_WDATA_OFFS                                             (AOP_DEC_REG_BASE_OFFS + 0x00000010)
#define HWIO_AOP_GPO_WDATA_RMSK                                             0xffffffff
#define HWIO_AOP_GPO_WDATA_IN          \
        in_dword_masked(HWIO_AOP_GPO_WDATA_ADDR, HWIO_AOP_GPO_WDATA_RMSK)
#define HWIO_AOP_GPO_WDATA_INM(m)      \
        in_dword_masked(HWIO_AOP_GPO_WDATA_ADDR, m)
#define HWIO_AOP_GPO_WDATA_OUT(v)      \
        out_dword(HWIO_AOP_GPO_WDATA_ADDR,v)
#define HWIO_AOP_GPO_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_GPO_WDATA_ADDR,m,v,HWIO_AOP_GPO_WDATA_IN)
#define HWIO_AOP_GPO_WDATA_WDATA_BMSK                                       0xffffffff
#define HWIO_AOP_GPO_WDATA_WDATA_SHFT                                              0x0

#define HWIO_AOP_GPO_WDSET_ADDR                                             (AOP_DEC_REG_BASE      + 0x00000014)
#define HWIO_AOP_GPO_WDSET_PHYS                                             (AOP_DEC_REG_BASE_PHYS + 0x00000014)
#define HWIO_AOP_GPO_WDSET_OFFS                                             (AOP_DEC_REG_BASE_OFFS + 0x00000014)
#define HWIO_AOP_GPO_WDSET_RMSK                                             0xffffffff
#define HWIO_AOP_GPO_WDSET_OUT(v)      \
        out_dword(HWIO_AOP_GPO_WDSET_ADDR,v)
#define HWIO_AOP_GPO_WDSET_WDSET_BMSK                                       0xffffffff
#define HWIO_AOP_GPO_WDSET_WDSET_SHFT                                              0x0

#define HWIO_AOP_GPO_WDCLR_ADDR                                             (AOP_DEC_REG_BASE      + 0x00000018)
#define HWIO_AOP_GPO_WDCLR_PHYS                                             (AOP_DEC_REG_BASE_PHYS + 0x00000018)
#define HWIO_AOP_GPO_WDCLR_OFFS                                             (AOP_DEC_REG_BASE_OFFS + 0x00000018)
#define HWIO_AOP_GPO_WDCLR_RMSK                                             0xffffffff
#define HWIO_AOP_GPO_WDCLR_OUT(v)      \
        out_dword(HWIO_AOP_GPO_WDCLR_ADDR,v)
#define HWIO_AOP_GPO_WDCLR_WDCLR_BMSK                                       0xffffffff
#define HWIO_AOP_GPO_WDCLR_WDCLR_SHFT                                              0x0

#define HWIO_AOP_SLAVES_CLK_GATING_ADDR                                     (AOP_DEC_REG_BASE      + 0x0000001c)
#define HWIO_AOP_SLAVES_CLK_GATING_PHYS                                     (AOP_DEC_REG_BASE_PHYS + 0x0000001c)
#define HWIO_AOP_SLAVES_CLK_GATING_OFFS                                     (AOP_DEC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_AOP_SLAVES_CLK_GATING_RMSK                                            0xf
#define HWIO_AOP_SLAVES_CLK_GATING_IN          \
        in_dword_masked(HWIO_AOP_SLAVES_CLK_GATING_ADDR, HWIO_AOP_SLAVES_CLK_GATING_RMSK)
#define HWIO_AOP_SLAVES_CLK_GATING_INM(m)      \
        in_dword_masked(HWIO_AOP_SLAVES_CLK_GATING_ADDR, m)
#define HWIO_AOP_SLAVES_CLK_GATING_OUT(v)      \
        out_dword(HWIO_AOP_SLAVES_CLK_GATING_ADDR,v)
#define HWIO_AOP_SLAVES_CLK_GATING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_SLAVES_CLK_GATING_ADDR,m,v,HWIO_AOP_SLAVES_CLK_GATING_IN)
#define HWIO_AOP_SLAVES_CLK_GATING_INTR_CLK_GATING_BMSK                            0x8
#define HWIO_AOP_SLAVES_CLK_GATING_INTR_CLK_GATING_SHFT                            0x3
#define HWIO_AOP_SLAVES_CLK_GATING_RAM_CLK_GATING_BMSK                             0x4
#define HWIO_AOP_SLAVES_CLK_GATING_RAM_CLK_GATING_SHFT                             0x2
#define HWIO_AOP_SLAVES_CLK_GATING_PERIPH_CLK_GATING_BMSK                          0x2
#define HWIO_AOP_SLAVES_CLK_GATING_PERIPH_CLK_GATING_SHFT                          0x1
#define HWIO_AOP_SLAVES_CLK_GATING_SLP_WKUP_FSM_CLK_GATING_BMSK                    0x1
#define HWIO_AOP_SLAVES_CLK_GATING_SLP_WKUP_FSM_CLK_GATING_SHFT                    0x0

#define HWIO_AOP_INTR_POLARITY_0_ADDR                                       (AOP_DEC_REG_BASE      + 0x00000030)
#define HWIO_AOP_INTR_POLARITY_0_PHYS                                       (AOP_DEC_REG_BASE_PHYS + 0x00000030)
#define HWIO_AOP_INTR_POLARITY_0_OFFS                                       (AOP_DEC_REG_BASE_OFFS + 0x00000030)
#define HWIO_AOP_INTR_POLARITY_0_RMSK                                       0xffffffff
#define HWIO_AOP_INTR_POLARITY_0_IN          \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_0_ADDR, HWIO_AOP_INTR_POLARITY_0_RMSK)
#define HWIO_AOP_INTR_POLARITY_0_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_0_ADDR, m)
#define HWIO_AOP_INTR_POLARITY_0_OUT(v)      \
        out_dword(HWIO_AOP_INTR_POLARITY_0_ADDR,v)
#define HWIO_AOP_INTR_POLARITY_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_POLARITY_0_ADDR,m,v,HWIO_AOP_INTR_POLARITY_0_IN)
#define HWIO_AOP_INTR_POLARITY_0_POLARITY_BMSK                              0xffffffff
#define HWIO_AOP_INTR_POLARITY_0_POLARITY_SHFT                                     0x0

#define HWIO_AOP_INTR_POLARITY_1_ADDR                                       (AOP_DEC_REG_BASE      + 0x00000034)
#define HWIO_AOP_INTR_POLARITY_1_PHYS                                       (AOP_DEC_REG_BASE_PHYS + 0x00000034)
#define HWIO_AOP_INTR_POLARITY_1_OFFS                                       (AOP_DEC_REG_BASE_OFFS + 0x00000034)
#define HWIO_AOP_INTR_POLARITY_1_RMSK                                       0xffffffff
#define HWIO_AOP_INTR_POLARITY_1_IN          \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_1_ADDR, HWIO_AOP_INTR_POLARITY_1_RMSK)
#define HWIO_AOP_INTR_POLARITY_1_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_1_ADDR, m)
#define HWIO_AOP_INTR_POLARITY_1_OUT(v)      \
        out_dword(HWIO_AOP_INTR_POLARITY_1_ADDR,v)
#define HWIO_AOP_INTR_POLARITY_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_POLARITY_1_ADDR,m,v,HWIO_AOP_INTR_POLARITY_1_IN)
#define HWIO_AOP_INTR_POLARITY_1_POLARITY_BMSK                              0xffffffff
#define HWIO_AOP_INTR_POLARITY_1_POLARITY_SHFT                                     0x0

#define HWIO_AOP_INTR_EDG_LVL_0_ADDR                                        (AOP_DEC_REG_BASE      + 0x00000038)
#define HWIO_AOP_INTR_EDG_LVL_0_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x00000038)
#define HWIO_AOP_INTR_EDG_LVL_0_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x00000038)
#define HWIO_AOP_INTR_EDG_LVL_0_RMSK                                        0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_0_IN          \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_0_ADDR, HWIO_AOP_INTR_EDG_LVL_0_RMSK)
#define HWIO_AOP_INTR_EDG_LVL_0_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_0_ADDR, m)
#define HWIO_AOP_INTR_EDG_LVL_0_OUT(v)      \
        out_dword(HWIO_AOP_INTR_EDG_LVL_0_ADDR,v)
#define HWIO_AOP_INTR_EDG_LVL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_EDG_LVL_0_ADDR,m,v,HWIO_AOP_INTR_EDG_LVL_0_IN)
#define HWIO_AOP_INTR_EDG_LVL_0_EDG_LVL_BMSK                                0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_0_EDG_LVL_SHFT                                       0x0

#define HWIO_AOP_INTR_EDG_LVL_1_ADDR                                        (AOP_DEC_REG_BASE      + 0x0000003c)
#define HWIO_AOP_INTR_EDG_LVL_1_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x0000003c)
#define HWIO_AOP_INTR_EDG_LVL_1_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x0000003c)
#define HWIO_AOP_INTR_EDG_LVL_1_RMSK                                        0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_1_IN          \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_1_ADDR, HWIO_AOP_INTR_EDG_LVL_1_RMSK)
#define HWIO_AOP_INTR_EDG_LVL_1_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_1_ADDR, m)
#define HWIO_AOP_INTR_EDG_LVL_1_OUT(v)      \
        out_dword(HWIO_AOP_INTR_EDG_LVL_1_ADDR,v)
#define HWIO_AOP_INTR_EDG_LVL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_EDG_LVL_1_ADDR,m,v,HWIO_AOP_INTR_EDG_LVL_1_IN)
#define HWIO_AOP_INTR_EDG_LVL_1_EDG_LVL_BMSK                                0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_1_EDG_LVL_SHFT                                       0x0

#define HWIO_AOP_WDOG_RESET_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000040)
#define HWIO_AOP_WDOG_RESET_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000040)
#define HWIO_AOP_WDOG_RESET_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000040)
#define HWIO_AOP_WDOG_RESET_RMSK                                                   0x3
#define HWIO_AOP_WDOG_RESET_IN          \
        in_dword_masked(HWIO_AOP_WDOG_RESET_ADDR, HWIO_AOP_WDOG_RESET_RMSK)
#define HWIO_AOP_WDOG_RESET_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_RESET_ADDR, m)
#define HWIO_AOP_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_RESET_ADDR,v)
#define HWIO_AOP_WDOG_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_RESET_ADDR,m,v,HWIO_AOP_WDOG_RESET_IN)
#define HWIO_AOP_WDOG_RESET_SYNC_STATUS_BMSK                                       0x2
#define HWIO_AOP_WDOG_RESET_SYNC_STATUS_SHFT                                       0x1
#define HWIO_AOP_WDOG_RESET_WDOG_RESET_BMSK                                        0x1
#define HWIO_AOP_WDOG_RESET_WDOG_RESET_SHFT                                        0x0

#define HWIO_AOP_WDOG_CTRL_ADDR                                             (AOP_DEC_REG_BASE      + 0x00000044)
#define HWIO_AOP_WDOG_CTRL_PHYS                                             (AOP_DEC_REG_BASE_PHYS + 0x00000044)
#define HWIO_AOP_WDOG_CTRL_OFFS                                             (AOP_DEC_REG_BASE_OFFS + 0x00000044)
#define HWIO_AOP_WDOG_CTRL_RMSK                                                    0x3
#define HWIO_AOP_WDOG_CTRL_IN          \
        in_dword_masked(HWIO_AOP_WDOG_CTRL_ADDR, HWIO_AOP_WDOG_CTRL_RMSK)
#define HWIO_AOP_WDOG_CTRL_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_CTRL_ADDR, m)
#define HWIO_AOP_WDOG_CTRL_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_CTRL_ADDR,v)
#define HWIO_AOP_WDOG_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_CTRL_ADDR,m,v,HWIO_AOP_WDOG_CTRL_IN)
#define HWIO_AOP_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_BMSK                                 0x2
#define HWIO_AOP_WDOG_CTRL_HW_WAKEUP_SLEEP_EN_SHFT                                 0x1
#define HWIO_AOP_WDOG_CTRL_ENABLE_BMSK                                             0x1
#define HWIO_AOP_WDOG_CTRL_ENABLE_SHFT                                             0x0

#define HWIO_AOP_WDOG_STATUS_ADDR                                           (AOP_DEC_REG_BASE      + 0x00000048)
#define HWIO_AOP_WDOG_STATUS_PHYS                                           (AOP_DEC_REG_BASE_PHYS + 0x00000048)
#define HWIO_AOP_WDOG_STATUS_OFFS                                           (AOP_DEC_REG_BASE_OFFS + 0x00000048)
#define HWIO_AOP_WDOG_STATUS_RMSK                                             0x7fffff
#define HWIO_AOP_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_AOP_WDOG_STATUS_ADDR, HWIO_AOP_WDOG_STATUS_RMSK)
#define HWIO_AOP_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_STATUS_ADDR, m)
#define HWIO_AOP_WDOG_STATUS_WDOG_COUNT_BMSK                                  0x7ffff8
#define HWIO_AOP_WDOG_STATUS_WDOG_COUNT_SHFT                                       0x3
#define HWIO_AOP_WDOG_STATUS_WDOG_CNT_RESET_STATUS_BMSK                            0x4
#define HWIO_AOP_WDOG_STATUS_WDOG_CNT_RESET_STATUS_SHFT                            0x2
#define HWIO_AOP_WDOG_STATUS_WDOG_FROZEN_BMSK                                      0x2
#define HWIO_AOP_WDOG_STATUS_WDOG_FROZEN_SHFT                                      0x1
#define HWIO_AOP_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                              0x1
#define HWIO_AOP_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                              0x0

#define HWIO_AOP_WDOG_BARK_TIME_ADDR                                        (AOP_DEC_REG_BASE      + 0x0000004c)
#define HWIO_AOP_WDOG_BARK_TIME_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x0000004c)
#define HWIO_AOP_WDOG_BARK_TIME_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x0000004c)
#define HWIO_AOP_WDOG_BARK_TIME_RMSK                                          0x1fffff
#define HWIO_AOP_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_AOP_WDOG_BARK_TIME_ADDR, HWIO_AOP_WDOG_BARK_TIME_RMSK)
#define HWIO_AOP_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_BARK_TIME_ADDR, m)
#define HWIO_AOP_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_BARK_TIME_ADDR,v)
#define HWIO_AOP_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_BARK_TIME_ADDR,m,v,HWIO_AOP_WDOG_BARK_TIME_IN)
#define HWIO_AOP_WDOG_BARK_TIME_SYNC_STATUS_BMSK                              0x100000
#define HWIO_AOP_WDOG_BARK_TIME_SYNC_STATUS_SHFT                                  0x14
#define HWIO_AOP_WDOG_BARK_TIME_WDOG_BARK_VAL_BMSK                             0xfffff
#define HWIO_AOP_WDOG_BARK_TIME_WDOG_BARK_VAL_SHFT                                 0x0

#define HWIO_AOP_WDOG_BITE_TIME_ADDR                                        (AOP_DEC_REG_BASE      + 0x00000050)
#define HWIO_AOP_WDOG_BITE_TIME_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x00000050)
#define HWIO_AOP_WDOG_BITE_TIME_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x00000050)
#define HWIO_AOP_WDOG_BITE_TIME_RMSK                                          0x1fffff
#define HWIO_AOP_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_AOP_WDOG_BITE_TIME_ADDR, HWIO_AOP_WDOG_BITE_TIME_RMSK)
#define HWIO_AOP_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_BITE_TIME_ADDR, m)
#define HWIO_AOP_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_BITE_TIME_ADDR,v)
#define HWIO_AOP_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_BITE_TIME_ADDR,m,v,HWIO_AOP_WDOG_BITE_TIME_IN)
#define HWIO_AOP_WDOG_BITE_TIME_SYNC_STATUS_BMSK                              0x100000
#define HWIO_AOP_WDOG_BITE_TIME_SYNC_STATUS_SHFT                                  0x14
#define HWIO_AOP_WDOG_BITE_TIME_WDOG_BITE_VAL_BMSK                             0xfffff
#define HWIO_AOP_WDOG_BITE_TIME_WDOG_BITE_VAL_SHFT                                 0x0

#define HWIO_AOP_WDOG_TEST_LOAD_ADDR                                        (AOP_DEC_REG_BASE      + 0x00000054)
#define HWIO_AOP_WDOG_TEST_LOAD_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x00000054)
#define HWIO_AOP_WDOG_TEST_LOAD_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x00000054)
#define HWIO_AOP_WDOG_TEST_LOAD_RMSK                                               0x3
#define HWIO_AOP_WDOG_TEST_LOAD_IN          \
        in_dword_masked(HWIO_AOP_WDOG_TEST_LOAD_ADDR, HWIO_AOP_WDOG_TEST_LOAD_RMSK)
#define HWIO_AOP_WDOG_TEST_LOAD_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_TEST_LOAD_ADDR, m)
#define HWIO_AOP_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_AOP_WDOG_TEST_LOAD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_TEST_LOAD_ADDR,m,v,HWIO_AOP_WDOG_TEST_LOAD_IN)
#define HWIO_AOP_WDOG_TEST_LOAD_SYNC_STATUS_BMSK                                   0x2
#define HWIO_AOP_WDOG_TEST_LOAD_SYNC_STATUS_SHFT                                   0x1
#define HWIO_AOP_WDOG_TEST_LOAD_LOAD_BMSK                                          0x1
#define HWIO_AOP_WDOG_TEST_LOAD_LOAD_SHFT                                          0x0

#define HWIO_AOP_WDOG_TEST_ADDR                                             (AOP_DEC_REG_BASE      + 0x00000058)
#define HWIO_AOP_WDOG_TEST_PHYS                                             (AOP_DEC_REG_BASE_PHYS + 0x00000058)
#define HWIO_AOP_WDOG_TEST_OFFS                                             (AOP_DEC_REG_BASE_OFFS + 0x00000058)
#define HWIO_AOP_WDOG_TEST_RMSK                                               0x1fffff
#define HWIO_AOP_WDOG_TEST_IN          \
        in_dword_masked(HWIO_AOP_WDOG_TEST_ADDR, HWIO_AOP_WDOG_TEST_RMSK)
#define HWIO_AOP_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_AOP_WDOG_TEST_ADDR, m)
#define HWIO_AOP_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_AOP_WDOG_TEST_ADDR,v)
#define HWIO_AOP_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_WDOG_TEST_ADDR,m,v,HWIO_AOP_WDOG_TEST_IN)
#define HWIO_AOP_WDOG_TEST_SYNC_STATUS_BMSK                                   0x100000
#define HWIO_AOP_WDOG_TEST_SYNC_STATUS_SHFT                                       0x14
#define HWIO_AOP_WDOG_TEST_LOAD_VALUE_BMSK                                     0xfffff
#define HWIO_AOP_WDOG_TEST_LOAD_VALUE_SHFT                                         0x0

#define HWIO_AOP_TEST_BUS_SEL_ADDR                                          (AOP_DEC_REG_BASE      + 0x0000005c)
#define HWIO_AOP_TEST_BUS_SEL_PHYS                                          (AOP_DEC_REG_BASE_PHYS + 0x0000005c)
#define HWIO_AOP_TEST_BUS_SEL_OFFS                                          (AOP_DEC_REG_BASE_OFFS + 0x0000005c)
#define HWIO_AOP_TEST_BUS_SEL_RMSK                                                 0xf
#define HWIO_AOP_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_AOP_TEST_BUS_SEL_ADDR, HWIO_AOP_TEST_BUS_SEL_RMSK)
#define HWIO_AOP_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_AOP_TEST_BUS_SEL_ADDR, m)
#define HWIO_AOP_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_AOP_TEST_BUS_SEL_ADDR,v)
#define HWIO_AOP_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_TEST_BUS_SEL_ADDR,m,v,HWIO_AOP_TEST_BUS_SEL_IN)
#define HWIO_AOP_TEST_BUS_SEL_VAL_BMSK                                             0xf
#define HWIO_AOP_TEST_BUS_SEL_VAL_SHFT                                             0x0

#define HWIO_AOP_SPARE_REG0_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000060)
#define HWIO_AOP_SPARE_REG0_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000060)
#define HWIO_AOP_SPARE_REG0_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000060)
#define HWIO_AOP_SPARE_REG0_RMSK                                            0xffffffff
#define HWIO_AOP_SPARE_REG0_IN          \
        in_dword_masked(HWIO_AOP_SPARE_REG0_ADDR, HWIO_AOP_SPARE_REG0_RMSK)
#define HWIO_AOP_SPARE_REG0_INM(m)      \
        in_dword_masked(HWIO_AOP_SPARE_REG0_ADDR, m)
#define HWIO_AOP_SPARE_REG0_OUT(v)      \
        out_dword(HWIO_AOP_SPARE_REG0_ADDR,v)
#define HWIO_AOP_SPARE_REG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_SPARE_REG0_ADDR,m,v,HWIO_AOP_SPARE_REG0_IN)
#define HWIO_AOP_SPARE_REG0_WDATA_BMSK                                      0xffffffff
#define HWIO_AOP_SPARE_REG0_WDATA_SHFT                                             0x0

#define HWIO_AOP_SPARE_REG1_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000064)
#define HWIO_AOP_SPARE_REG1_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000064)
#define HWIO_AOP_SPARE_REG1_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000064)
#define HWIO_AOP_SPARE_REG1_RMSK                                            0xffffffff
#define HWIO_AOP_SPARE_REG1_IN          \
        in_dword_masked(HWIO_AOP_SPARE_REG1_ADDR, HWIO_AOP_SPARE_REG1_RMSK)
#define HWIO_AOP_SPARE_REG1_INM(m)      \
        in_dword_masked(HWIO_AOP_SPARE_REG1_ADDR, m)
#define HWIO_AOP_SPARE_REG1_OUT(v)      \
        out_dword(HWIO_AOP_SPARE_REG1_ADDR,v)
#define HWIO_AOP_SPARE_REG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_SPARE_REG1_ADDR,m,v,HWIO_AOP_SPARE_REG1_IN)
#define HWIO_AOP_SPARE_REG1_WDATA_BMSK                                      0xffffffff
#define HWIO_AOP_SPARE_REG1_WDATA_SHFT                                             0x0

#define HWIO_AOP_SPARE_REG2_ADDR                                            (AOP_DEC_REG_BASE      + 0x00000068)
#define HWIO_AOP_SPARE_REG2_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x00000068)
#define HWIO_AOP_SPARE_REG2_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x00000068)
#define HWIO_AOP_SPARE_REG2_RMSK                                            0xffffffff
#define HWIO_AOP_SPARE_REG2_IN          \
        in_dword_masked(HWIO_AOP_SPARE_REG2_ADDR, HWIO_AOP_SPARE_REG2_RMSK)
#define HWIO_AOP_SPARE_REG2_INM(m)      \
        in_dword_masked(HWIO_AOP_SPARE_REG2_ADDR, m)
#define HWIO_AOP_SPARE_REG2_OUT(v)      \
        out_dword(HWIO_AOP_SPARE_REG2_ADDR,v)
#define HWIO_AOP_SPARE_REG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_SPARE_REG2_ADDR,m,v,HWIO_AOP_SPARE_REG2_IN)
#define HWIO_AOP_SPARE_REG2_WDATA_BMSK                                      0xffffffff
#define HWIO_AOP_SPARE_REG2_WDATA_SHFT                                             0x0

#define HWIO_AOP_SPARE_REG3_ADDR                                            (AOP_DEC_REG_BASE      + 0x0000006c)
#define HWIO_AOP_SPARE_REG3_PHYS                                            (AOP_DEC_REG_BASE_PHYS + 0x0000006c)
#define HWIO_AOP_SPARE_REG3_OFFS                                            (AOP_DEC_REG_BASE_OFFS + 0x0000006c)
#define HWIO_AOP_SPARE_REG3_RMSK                                            0xffffffff
#define HWIO_AOP_SPARE_REG3_IN          \
        in_dword_masked(HWIO_AOP_SPARE_REG3_ADDR, HWIO_AOP_SPARE_REG3_RMSK)
#define HWIO_AOP_SPARE_REG3_INM(m)      \
        in_dword_masked(HWIO_AOP_SPARE_REG3_ADDR, m)
#define HWIO_AOP_SPARE_REG3_OUT(v)      \
        out_dword(HWIO_AOP_SPARE_REG3_ADDR,v)
#define HWIO_AOP_SPARE_REG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_SPARE_REG3_ADDR,m,v,HWIO_AOP_SPARE_REG3_IN)
#define HWIO_AOP_SPARE_REG3_WDATA_BMSK                                      0xffffffff
#define HWIO_AOP_SPARE_REG3_WDATA_SHFT                                             0x0

#define HWIO_AOP_PAGE_SELECT_ADDR                                           (AOP_DEC_REG_BASE      + 0x00000070)
#define HWIO_AOP_PAGE_SELECT_PHYS                                           (AOP_DEC_REG_BASE_PHYS + 0x00000070)
#define HWIO_AOP_PAGE_SELECT_OFFS                                           (AOP_DEC_REG_BASE_OFFS + 0x00000070)
#define HWIO_AOP_PAGE_SELECT_RMSK                                                 0x3f
#define HWIO_AOP_PAGE_SELECT_IN          \
        in_dword_masked(HWIO_AOP_PAGE_SELECT_ADDR, HWIO_AOP_PAGE_SELECT_RMSK)
#define HWIO_AOP_PAGE_SELECT_INM(m)      \
        in_dword_masked(HWIO_AOP_PAGE_SELECT_ADDR, m)
#define HWIO_AOP_PAGE_SELECT_OUT(v)      \
        out_dword(HWIO_AOP_PAGE_SELECT_ADDR,v)
#define HWIO_AOP_PAGE_SELECT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_PAGE_SELECT_ADDR,m,v,HWIO_AOP_PAGE_SELECT_IN)
#define HWIO_AOP_PAGE_SELECT_PAGE_SELECT_BMSK                                     0x3f
#define HWIO_AOP_PAGE_SELECT_PAGE_SELECT_SHFT                                      0x0

#define HWIO_AOP_BRIDGES_CLK_GATING_ADDR                                    (AOP_DEC_REG_BASE      + 0x00000074)
#define HWIO_AOP_BRIDGES_CLK_GATING_PHYS                                    (AOP_DEC_REG_BASE_PHYS + 0x00000074)
#define HWIO_AOP_BRIDGES_CLK_GATING_OFFS                                    (AOP_DEC_REG_BASE_OFFS + 0x00000074)
#define HWIO_AOP_BRIDGES_CLK_GATING_RMSK                                           0x3
#define HWIO_AOP_BRIDGES_CLK_GATING_IN          \
        in_dword_masked(HWIO_AOP_BRIDGES_CLK_GATING_ADDR, HWIO_AOP_BRIDGES_CLK_GATING_RMSK)
#define HWIO_AOP_BRIDGES_CLK_GATING_INM(m)      \
        in_dword_masked(HWIO_AOP_BRIDGES_CLK_GATING_ADDR, m)
#define HWIO_AOP_BRIDGES_CLK_GATING_OUT(v)      \
        out_dword(HWIO_AOP_BRIDGES_CLK_GATING_ADDR,v)
#define HWIO_AOP_BRIDGES_CLK_GATING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_BRIDGES_CLK_GATING_ADDR,m,v,HWIO_AOP_BRIDGES_CLK_GATING_IN)
#define HWIO_AOP_BRIDGES_CLK_GATING_AOP_BRIDGE_CLK_GATING_BMSK                     0x2
#define HWIO_AOP_BRIDGES_CLK_GATING_AOP_BRIDGE_CLK_GATING_SHFT                     0x1
#define HWIO_AOP_BRIDGES_CLK_GATING_NOC_BRIDGE_CLK_GATING_BMSK                     0x1
#define HWIO_AOP_BRIDGES_CLK_GATING_NOC_BRIDGE_CLK_GATING_SHFT                     0x0

#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_ADDR                               (AOP_DEC_REG_BASE      + 0x00000078)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_PHYS                               (AOP_DEC_REG_BASE_PHYS + 0x00000078)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_OFFS                               (AOP_DEC_REG_BASE_OFFS + 0x00000078)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_RMSK                               0x7fffffff
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_IN          \
        in_dword_masked(HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_ADDR, HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_RMSK)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_INM(m)      \
        in_dword_masked(HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_ADDR, m)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_OUT(v)      \
        out_dword(HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_ADDR,v)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_ADDR,m,v,HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_IN)
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_AOP_BRIDGE_HYST_CNTR_BMSK          0x7fff0000
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_AOP_BRIDGE_HYST_CNTR_SHFT                0x10
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_NOC_BRDG_SYNC_STATUS_BMSK              0x8000
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_NOC_BRDG_SYNC_STATUS_SHFT                 0xf
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_NOC_BRIDGE_HYST_CNTR_BMSK              0x7fff
#define HWIO_AOP_BRIDGES_HYSTERESIS_CNTR_NOC_BRIDGE_HYST_CNTR_SHFT                 0x0

#define HWIO_AOP_MISC_ADDR                                                  (AOP_DEC_REG_BASE      + 0x0000007c)
#define HWIO_AOP_MISC_PHYS                                                  (AOP_DEC_REG_BASE_PHYS + 0x0000007c)
#define HWIO_AOP_MISC_OFFS                                                  (AOP_DEC_REG_BASE_OFFS + 0x0000007c)
#define HWIO_AOP_MISC_RMSK                                                         0x3
#define HWIO_AOP_MISC_IN          \
        in_dword_masked(HWIO_AOP_MISC_ADDR, HWIO_AOP_MISC_RMSK)
#define HWIO_AOP_MISC_INM(m)      \
        in_dword_masked(HWIO_AOP_MISC_ADDR, m)
#define HWIO_AOP_MISC_OUT(v)      \
        out_dword(HWIO_AOP_MISC_ADDR,v)
#define HWIO_AOP_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_MISC_ADDR,m,v,HWIO_AOP_MISC_IN)
#define HWIO_AOP_MISC_AOP_BRIDGE_POST_EN_BMSK                                      0x2
#define HWIO_AOP_MISC_AOP_BRIDGE_POST_EN_SHFT                                      0x1
#define HWIO_AOP_MISC_NOC_BRIDGE_POST_EN_BMSK                                      0x1
#define HWIO_AOP_MISC_NOC_BRIDGE_POST_EN_SHFT                                      0x0

#define HWIO_AOP_CODE_RAM_DENSITY_ADDR                                      (AOP_DEC_REG_BASE      + 0x00000080)
#define HWIO_AOP_CODE_RAM_DENSITY_PHYS                                      (AOP_DEC_REG_BASE_PHYS + 0x00000080)
#define HWIO_AOP_CODE_RAM_DENSITY_OFFS                                      (AOP_DEC_REG_BASE_OFFS + 0x00000080)
#define HWIO_AOP_CODE_RAM_DENSITY_RMSK                                        0xffffff
#define HWIO_AOP_CODE_RAM_DENSITY_IN          \
        in_dword_masked(HWIO_AOP_CODE_RAM_DENSITY_ADDR, HWIO_AOP_CODE_RAM_DENSITY_RMSK)
#define HWIO_AOP_CODE_RAM_DENSITY_INM(m)      \
        in_dword_masked(HWIO_AOP_CODE_RAM_DENSITY_ADDR, m)
#define HWIO_AOP_CODE_RAM_DENSITY_DENSITY_BMSK                                0xffffff
#define HWIO_AOP_CODE_RAM_DENSITY_DENSITY_SHFT                                     0x0

#define HWIO_AOP_DATA_RAM_DENSITY_ADDR                                      (AOP_DEC_REG_BASE      + 0x00000084)
#define HWIO_AOP_DATA_RAM_DENSITY_PHYS                                      (AOP_DEC_REG_BASE_PHYS + 0x00000084)
#define HWIO_AOP_DATA_RAM_DENSITY_OFFS                                      (AOP_DEC_REG_BASE_OFFS + 0x00000084)
#define HWIO_AOP_DATA_RAM_DENSITY_RMSK                                        0xffffff
#define HWIO_AOP_DATA_RAM_DENSITY_IN          \
        in_dword_masked(HWIO_AOP_DATA_RAM_DENSITY_ADDR, HWIO_AOP_DATA_RAM_DENSITY_RMSK)
#define HWIO_AOP_DATA_RAM_DENSITY_INM(m)      \
        in_dword_masked(HWIO_AOP_DATA_RAM_DENSITY_ADDR, m)
#define HWIO_AOP_DATA_RAM_DENSITY_DENSITY_BMSK                                0xffffff
#define HWIO_AOP_DATA_RAM_DENSITY_DENSITY_SHFT                                     0x0

#define HWIO_AOP_MSG_RAM_DENSITY_ADDR                                       (AOP_DEC_REG_BASE      + 0x00000088)
#define HWIO_AOP_MSG_RAM_DENSITY_PHYS                                       (AOP_DEC_REG_BASE_PHYS + 0x00000088)
#define HWIO_AOP_MSG_RAM_DENSITY_OFFS                                       (AOP_DEC_REG_BASE_OFFS + 0x00000088)
#define HWIO_AOP_MSG_RAM_DENSITY_RMSK                                         0xffffff
#define HWIO_AOP_MSG_RAM_DENSITY_IN          \
        in_dword_masked(HWIO_AOP_MSG_RAM_DENSITY_ADDR, HWIO_AOP_MSG_RAM_DENSITY_RMSK)
#define HWIO_AOP_MSG_RAM_DENSITY_INM(m)      \
        in_dword_masked(HWIO_AOP_MSG_RAM_DENSITY_ADDR, m)
#define HWIO_AOP_MSG_RAM_DENSITY_DENSITY_BMSK                                 0xffffff
#define HWIO_AOP_MSG_RAM_DENSITY_DENSITY_SHFT                                      0x0

#define HWIO_AOP_INTR_POLARITY_2_ADDR                                       (AOP_DEC_REG_BASE      + 0x0000008c)
#define HWIO_AOP_INTR_POLARITY_2_PHYS                                       (AOP_DEC_REG_BASE_PHYS + 0x0000008c)
#define HWIO_AOP_INTR_POLARITY_2_OFFS                                       (AOP_DEC_REG_BASE_OFFS + 0x0000008c)
#define HWIO_AOP_INTR_POLARITY_2_RMSK                                       0xffffffff
#define HWIO_AOP_INTR_POLARITY_2_IN          \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_2_ADDR, HWIO_AOP_INTR_POLARITY_2_RMSK)
#define HWIO_AOP_INTR_POLARITY_2_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_2_ADDR, m)
#define HWIO_AOP_INTR_POLARITY_2_OUT(v)      \
        out_dword(HWIO_AOP_INTR_POLARITY_2_ADDR,v)
#define HWIO_AOP_INTR_POLARITY_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_POLARITY_2_ADDR,m,v,HWIO_AOP_INTR_POLARITY_2_IN)
#define HWIO_AOP_INTR_POLARITY_2_POLARITY_BMSK                              0xffffffff
#define HWIO_AOP_INTR_POLARITY_2_POLARITY_SHFT                                     0x0

#define HWIO_AOP_INTR_POLARITY_3_ADDR                                       (AOP_DEC_REG_BASE      + 0x00000090)
#define HWIO_AOP_INTR_POLARITY_3_PHYS                                       (AOP_DEC_REG_BASE_PHYS + 0x00000090)
#define HWIO_AOP_INTR_POLARITY_3_OFFS                                       (AOP_DEC_REG_BASE_OFFS + 0x00000090)
#define HWIO_AOP_INTR_POLARITY_3_RMSK                                       0xffffffff
#define HWIO_AOP_INTR_POLARITY_3_IN          \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_3_ADDR, HWIO_AOP_INTR_POLARITY_3_RMSK)
#define HWIO_AOP_INTR_POLARITY_3_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_POLARITY_3_ADDR, m)
#define HWIO_AOP_INTR_POLARITY_3_OUT(v)      \
        out_dword(HWIO_AOP_INTR_POLARITY_3_ADDR,v)
#define HWIO_AOP_INTR_POLARITY_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_POLARITY_3_ADDR,m,v,HWIO_AOP_INTR_POLARITY_3_IN)
#define HWIO_AOP_INTR_POLARITY_3_POLARITY_BMSK                              0xffffffff
#define HWIO_AOP_INTR_POLARITY_3_POLARITY_SHFT                                     0x0

#define HWIO_AOP_INTR_EDG_LVL_2_ADDR                                        (AOP_DEC_REG_BASE      + 0x00000094)
#define HWIO_AOP_INTR_EDG_LVL_2_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x00000094)
#define HWIO_AOP_INTR_EDG_LVL_2_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x00000094)
#define HWIO_AOP_INTR_EDG_LVL_2_RMSK                                        0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_2_IN          \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_2_ADDR, HWIO_AOP_INTR_EDG_LVL_2_RMSK)
#define HWIO_AOP_INTR_EDG_LVL_2_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_2_ADDR, m)
#define HWIO_AOP_INTR_EDG_LVL_2_OUT(v)      \
        out_dword(HWIO_AOP_INTR_EDG_LVL_2_ADDR,v)
#define HWIO_AOP_INTR_EDG_LVL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_EDG_LVL_2_ADDR,m,v,HWIO_AOP_INTR_EDG_LVL_2_IN)
#define HWIO_AOP_INTR_EDG_LVL_2_EDG_LVL_BMSK                                0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_2_EDG_LVL_SHFT                                       0x0

#define HWIO_AOP_INTR_EDG_LVL_3_ADDR                                        (AOP_DEC_REG_BASE      + 0x00000098)
#define HWIO_AOP_INTR_EDG_LVL_3_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x00000098)
#define HWIO_AOP_INTR_EDG_LVL_3_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x00000098)
#define HWIO_AOP_INTR_EDG_LVL_3_RMSK                                        0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_3_IN          \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_3_ADDR, HWIO_AOP_INTR_EDG_LVL_3_RMSK)
#define HWIO_AOP_INTR_EDG_LVL_3_INM(m)      \
        in_dword_masked(HWIO_AOP_INTR_EDG_LVL_3_ADDR, m)
#define HWIO_AOP_INTR_EDG_LVL_3_OUT(v)      \
        out_dword(HWIO_AOP_INTR_EDG_LVL_3_ADDR,v)
#define HWIO_AOP_INTR_EDG_LVL_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_INTR_EDG_LVL_3_ADDR,m,v,HWIO_AOP_INTR_EDG_LVL_3_IN)
#define HWIO_AOP_INTR_EDG_LVL_3_EDG_LVL_BMSK                                0xffffffff
#define HWIO_AOP_INTR_EDG_LVL_3_EDG_LVL_SHFT                                       0x0

#define HWIO_AOP_IPC_PULSEWIDTH_ADDR                                        (AOP_DEC_REG_BASE      + 0x0000009c)
#define HWIO_AOP_IPC_PULSEWIDTH_PHYS                                        (AOP_DEC_REG_BASE_PHYS + 0x0000009c)
#define HWIO_AOP_IPC_PULSEWIDTH_OFFS                                        (AOP_DEC_REG_BASE_OFFS + 0x0000009c)
#define HWIO_AOP_IPC_PULSEWIDTH_RMSK                                               0xf
#define HWIO_AOP_IPC_PULSEWIDTH_IN          \
        in_dword_masked(HWIO_AOP_IPC_PULSEWIDTH_ADDR, HWIO_AOP_IPC_PULSEWIDTH_RMSK)
#define HWIO_AOP_IPC_PULSEWIDTH_INM(m)      \
        in_dword_masked(HWIO_AOP_IPC_PULSEWIDTH_ADDR, m)
#define HWIO_AOP_IPC_PULSEWIDTH_OUT(v)      \
        out_dword(HWIO_AOP_IPC_PULSEWIDTH_ADDR,v)
#define HWIO_AOP_IPC_PULSEWIDTH_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_IPC_PULSEWIDTH_ADDR,m,v,HWIO_AOP_IPC_PULSEWIDTH_IN)
#define HWIO_AOP_IPC_PULSEWIDTH_WIDTH_BMSK                                         0xf
#define HWIO_AOP_IPC_PULSEWIDTH_WIDTH_SHFT                                         0x0

#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_ADDR                                    (AOP_DEC_REG_BASE      + 0x000000a0)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_PHYS                                    (AOP_DEC_REG_BASE_PHYS + 0x000000a0)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_OFFS                                    (AOP_DEC_REG_BASE_OFFS + 0x000000a0)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_RMSK                                           0x1
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_IN          \
        in_dword_masked(HWIO_AOP_ALWAYS_ON_PWR_HOLD_ADDR, HWIO_AOP_ALWAYS_ON_PWR_HOLD_RMSK)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_INM(m)      \
        in_dword_masked(HWIO_AOP_ALWAYS_ON_PWR_HOLD_ADDR, m)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_OUT(v)      \
        out_dword(HWIO_AOP_ALWAYS_ON_PWR_HOLD_ADDR,v)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_ALWAYS_ON_PWR_HOLD_ADDR,m,v,HWIO_AOP_ALWAYS_ON_PWR_HOLD_IN)
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_ALWAYS_ON_PWR_HOLD_BMSK                        0x1
#define HWIO_AOP_ALWAYS_ON_PWR_HOLD_ALWAYS_ON_PWR_HOLD_SHFT                        0x0

/*----------------------------------------------------------------------------
 * MODULE: AOP_F0_QTMR_V1_F0
 *--------------------------------------------------------------------------*/

#define AOP_F0_QTMR_V1_F0_REG_BASE                                  (AOSS_BASE      + 0x000a0000)
#define AOP_F0_QTMR_V1_F0_REG_BASE_PHYS                             (AOSS_BASE_PHYS + 0x000a0000)
#define AOP_F0_QTMR_V1_F0_REG_BASE_OFFS                             0x000a0000

#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000000)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000000)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000000)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_ADDR, HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000004)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000004)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000004)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_ADDR, HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000008)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000008)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000008)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_ADDR, HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x0000000c)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x0000000c)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x0000000c)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_ADDR, HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_ADDR                             (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000010)
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_PHYS                             (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000010)
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_OFFS                             (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000010)
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTFRQ_ADDR, HWIO_AOP_F0_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000014)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000014)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000014)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_ADDR, HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_AOP_F0_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_ADDR                         (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000018)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_PHYS                         (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000018)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_OFFS                         (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000018)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_ADDR                         (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x0000001c)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_PHYS                         (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x0000001c)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_OFFS                         (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x0000001c)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_ADDR                       (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000020)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_PHYS                       (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000020)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_OFFS                       (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000020)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_ADDR                       (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000024)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_PHYS                       (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000024)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_OFFS                       (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000024)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000028)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000028)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000028)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_ADDR, HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ADDR                           (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x0000002c)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_PHYS                           (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x0000002c)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_OFFS                           (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x0000002c)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ADDR, HWIO_AOP_F0_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTP_CTL_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_AOP_F0_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_ADDR                       (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000030)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_PHYS                       (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000030)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_OFFS                       (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000030)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_ADDR                       (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000034)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_PHYS                       (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000034)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_OFFS                       (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000034)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_ADDR                          (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000038)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_PHYS                          (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000038)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_OFFS                          (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000038)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_ADDR, HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_AOP_F0_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ADDR                           (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x0000003c)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_PHYS                           (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x0000003c)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_OFFS                           (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x0000003c)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ADDR, HWIO_AOP_F0_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_AOP_F0_QTMR_V1_CNTV_CTL_IN)
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_AOP_F0_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_AOP_F0_QTMR_V1_VERSION_ADDR                            (AOP_F0_QTMR_V1_F0_REG_BASE      + 0x00000fd0)
#define HWIO_AOP_F0_QTMR_V1_VERSION_PHYS                            (AOP_F0_QTMR_V1_F0_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_AOP_F0_QTMR_V1_VERSION_OFFS                            (AOP_F0_QTMR_V1_F0_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_AOP_F0_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_AOP_F0_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_VERSION_ADDR, HWIO_AOP_F0_QTMR_V1_VERSION_RMSK)
#define HWIO_AOP_F0_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_AOP_F0_QTMR_V1_VERSION_ADDR, m)
#define HWIO_AOP_F0_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_AOP_F0_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_AOP_F0_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_AOP_F0_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_AOP_F0_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_AOP_F0_QTMR_V1_VERSION_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: AOP_F1_QTMR_V1_F1
 *--------------------------------------------------------------------------*/

#define AOP_F1_QTMR_V1_F1_REG_BASE                                  (AOSS_BASE      + 0x000b0000)
#define AOP_F1_QTMR_V1_F1_REG_BASE_PHYS                             (AOSS_BASE_PHYS + 0x000b0000)
#define AOP_F1_QTMR_V1_F1_REG_BASE_OFFS                             0x000b0000

#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000000)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000000)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000000)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_ADDR, HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000004)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000004)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000004)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_ADDR, HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000008)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000008)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000008)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_ADDR, HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x0000000c)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x0000000c)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x0000000c)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_ADDR, HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_ADDR                             (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000010)
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_PHYS                             (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000010)
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_OFFS                             (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000010)
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTFRQ_ADDR, HWIO_AOP_F1_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000014)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000014)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000014)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_ADDR, HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_AOP_F1_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_ADDR                         (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000018)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_PHYS                         (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000018)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_OFFS                         (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000018)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_ADDR                         (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x0000001c)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_PHYS                         (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x0000001c)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_OFFS                         (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x0000001c)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_ADDR                       (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000020)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_PHYS                       (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000020)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_OFFS                       (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000020)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_ADDR                       (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000024)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_PHYS                       (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000024)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_OFFS                       (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000024)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000028)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000028)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000028)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_ADDR, HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ADDR                           (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x0000002c)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_PHYS                           (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x0000002c)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_OFFS                           (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x0000002c)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ADDR, HWIO_AOP_F1_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTP_CTL_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_AOP_F1_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_ADDR                       (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000030)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_PHYS                       (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000030)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_OFFS                       (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000030)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_ADDR                       (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000034)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_PHYS                       (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000034)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_OFFS                       (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000034)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_ADDR                          (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000038)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_PHYS                          (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000038)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_OFFS                          (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000038)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_ADDR, HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_AOP_F1_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ADDR                           (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x0000003c)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_PHYS                           (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x0000003c)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_OFFS                           (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x0000003c)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ADDR, HWIO_AOP_F1_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_AOP_F1_QTMR_V1_CNTV_CTL_IN)
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_AOP_F1_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_AOP_F1_QTMR_V1_VERSION_ADDR                            (AOP_F1_QTMR_V1_F1_REG_BASE      + 0x00000fd0)
#define HWIO_AOP_F1_QTMR_V1_VERSION_PHYS                            (AOP_F1_QTMR_V1_F1_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_AOP_F1_QTMR_V1_VERSION_OFFS                            (AOP_F1_QTMR_V1_F1_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_AOP_F1_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_AOP_F1_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_VERSION_ADDR, HWIO_AOP_F1_QTMR_V1_VERSION_RMSK)
#define HWIO_AOP_F1_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_AOP_F1_QTMR_V1_VERSION_ADDR, m)
#define HWIO_AOP_F1_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_AOP_F1_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_AOP_F1_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_AOP_F1_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_AOP_F1_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_AOP_F1_QTMR_V1_VERSION_STEP_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: AOP_QTMR_AC
 *--------------------------------------------------------------------------*/

#define AOP_QTMR_AC_REG_BASE                                        (AOSS_BASE      + 0x00090000)
#define AOP_QTMR_AC_REG_BASE_PHYS                                   (AOSS_BASE_PHYS + 0x00090000)
#define AOP_QTMR_AC_REG_BASE_OFFS                                   0x00090000

#define HWIO_AOP_QTMR_AC_CNTFRQ_ADDR                                (AOP_QTMR_AC_REG_BASE      + 0x00000000)
#define HWIO_AOP_QTMR_AC_CNTFRQ_PHYS                                (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000000)
#define HWIO_AOP_QTMR_AC_CNTFRQ_OFFS                                (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000000)
#define HWIO_AOP_QTMR_AC_CNTFRQ_RMSK                                0xffffffff
#define HWIO_AOP_QTMR_AC_CNTFRQ_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTFRQ_ADDR, HWIO_AOP_QTMR_AC_CNTFRQ_RMSK)
#define HWIO_AOP_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_AOP_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_AOP_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_AOP_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_AOP_QTMR_AC_CNTFRQ_IN)
#define HWIO_AOP_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                         0xffffffff
#define HWIO_AOP_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                                0x0

#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_ADDR                           (AOP_QTMR_AC_REG_BASE      + 0x00000004)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_PHYS                           (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000004)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_OFFS                           (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000004)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_RMSK                                  0x3
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTNSAR_FG0_ADDR, HWIO_AOP_QTMR_AC_CNTNSAR_FG0_RMSK)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTNSAR_FG0_ADDR, m)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_OUT(v)      \
        out_dword(HWIO_AOP_QTMR_AC_CNTNSAR_FG0_ADDR,v)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CNTNSAR_FG0_ADDR,m,v,HWIO_AOP_QTMR_AC_CNTNSAR_FG0_IN)
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_NSN_BMSK                              0x3
#define HWIO_AOP_QTMR_AC_CNTNSAR_FG0_NSN_SHFT                              0x0

#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_ADDR                           (AOP_QTMR_AC_REG_BASE      + 0x00000008)
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_PHYS                           (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000008)
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_OFFS                           (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000008)
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_RMSK                           0xffffffff
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTTIDR_FG0_ADDR, HWIO_AOP_QTMR_AC_CNTTIDR_FG0_RMSK)
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTTIDR_FG0_ADDR, m)
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F7_CFG_BMSK                    0xf0000000
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F7_CFG_SHFT                          0x1c
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F6_CFG_BMSK                     0xf000000
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F6_CFG_SHFT                          0x18
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F5_CFG_BMSK                      0xf00000
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F5_CFG_SHFT                          0x14
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F4_CFG_BMSK                       0xf0000
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F4_CFG_SHFT                          0x10
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F3_CFG_BMSK                        0xf000
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F3_CFG_SHFT                           0xc
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F2_CFG_BMSK                         0xf00
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F2_CFG_SHFT                           0x8
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F1_CFG_BMSK                          0xf0
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F1_CFG_SHFT                           0x4
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F0_CFG_BMSK                           0xf
#define HWIO_AOP_QTMR_AC_CNTTIDR_FG0_F0_CFG_SHFT                           0x0

#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_ADDR(n)                        (AOP_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_PHYS(n)                        (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000040 + 0x4 * (n))
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_OFFS(n)                        (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000040 + 0x4 * (n))
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RMSK                                 0x3f
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_MAXn                                    1
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_INI(n)        \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTACRn_FG0_ADDR(n), HWIO_AOP_QTMR_AC_CNTACRn_FG0_RMSK)
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_INMI(n,mask)    \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTACRn_FG0_ADDR(n), mask)
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_OUTI(n,val)    \
        out_dword(HWIO_AOP_QTMR_AC_CNTACRn_FG0_ADDR(n),val)
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CNTACRn_FG0_ADDR(n),mask,val,HWIO_AOP_QTMR_AC_CNTACRn_FG0_INI(n))
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RWPT_BMSK                            0x20
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RWPT_SHFT                             0x5
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RWVT_BMSK                            0x10
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RWVT_SHFT                             0x4
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RVOFF_BMSK                            0x8
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RVOFF_SHFT                            0x3
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RFRQ_BMSK                             0x4
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RFRQ_SHFT                             0x2
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RPVCT_BMSK                            0x2
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RPVCT_SHFT                            0x1
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RPCT_BMSK                             0x1
#define HWIO_AOP_QTMR_AC_CNTACRn_FG0_RPCT_SHFT                             0x0

#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n)                   (AOP_QTMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_PHYS(n)                   (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000080 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_OFFS(n)                   (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000080 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK                      0xffffffff
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_MAXn                               1
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n)        \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_RMSK)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n), mask)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_OUTI(n,val)    \
        out_dword(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),val)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_ADDR(n),mask,val,HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_INI(n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_BMSK           0xffffffff
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_LO_n_CNTVOFF_LO_SHFT                  0x0

#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n)                   (AOP_QTMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_PHYS(n)                   (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000084 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_OFFS(n)                   (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000084 + 0x8 * (n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK                        0xffffff
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_MAXn                               1
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n)        \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_RMSK)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n), mask)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_OUTI(n,val)    \
        out_dword(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),val)
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_ADDR(n),mask,val,HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_INI(n))
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_BMSK             0xffffff
#define HWIO_AOP_QTMR_AC_CNTVOFF_FG0_HI_n_CNTVOFF_HI_SHFT                  0x0

#define HWIO_AOP_QTMR_AC_CFG_ADDR                                   (AOP_QTMR_AC_REG_BASE      + 0x00000fc0)
#define HWIO_AOP_QTMR_AC_CFG_PHYS                                   (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000fc0)
#define HWIO_AOP_QTMR_AC_CFG_OFFS                                   (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000fc0)
#define HWIO_AOP_QTMR_AC_CFG_RMSK                                          0x3
#define HWIO_AOP_QTMR_AC_CFG_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_CFG_ADDR, HWIO_AOP_QTMR_AC_CFG_RMSK)
#define HWIO_AOP_QTMR_AC_CFG_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_CFG_ADDR, m)
#define HWIO_AOP_QTMR_AC_CFG_OUT(v)      \
        out_dword(HWIO_AOP_QTMR_AC_CFG_ADDR,v)
#define HWIO_AOP_QTMR_AC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_CFG_ADDR,m,v,HWIO_AOP_QTMR_AC_CFG_IN)
#define HWIO_AOP_QTMR_AC_CFG_DSBL_ATOMIC_BMSK                              0x2
#define HWIO_AOP_QTMR_AC_CFG_DSBL_ATOMIC_SHFT                              0x1
#define HWIO_AOP_QTMR_AC_CFG_TEST_BUS_EN_BMSK                              0x1
#define HWIO_AOP_QTMR_AC_CFG_TEST_BUS_EN_SHFT                              0x0

#define HWIO_AOP_QTMR_AC_VERSION_ADDR                               (AOP_QTMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_AOP_QTMR_AC_VERSION_PHYS                               (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_AOP_QTMR_AC_VERSION_OFFS                               (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_AOP_QTMR_AC_VERSION_RMSK                               0xffffffff
#define HWIO_AOP_QTMR_AC_VERSION_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_VERSION_ADDR, HWIO_AOP_QTMR_AC_VERSION_RMSK)
#define HWIO_AOP_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_VERSION_ADDR, m)
#define HWIO_AOP_QTMR_AC_VERSION_MAJOR_BMSK                         0xf0000000
#define HWIO_AOP_QTMR_AC_VERSION_MAJOR_SHFT                               0x1c
#define HWIO_AOP_QTMR_AC_VERSION_MINOR_BMSK                          0xfff0000
#define HWIO_AOP_QTMR_AC_VERSION_MINOR_SHFT                               0x10
#define HWIO_AOP_QTMR_AC_VERSION_STEP_BMSK                              0xffff
#define HWIO_AOP_QTMR_AC_VERSION_STEP_SHFT                                 0x0

#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_ADDR                        (AOP_QTMR_AC_REG_BASE      + 0x00000fe0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_PHYS                        (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000fe0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_OFFS                        (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000fe0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_RMSK                        0xffffffff
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_ADDR, HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_RMSK)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_ADDR, m)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_OUT(v)      \
        out_dword(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_ADDR,v)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_ADDR,m,v,HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_IN)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_BMSK          0xffffffff
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_1_HW_FRAME_SEL1_SHFT                 0x0

#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_ADDR                        (AOP_QTMR_AC_REG_BASE      + 0x00000ff0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_PHYS                        (AOP_QTMR_AC_REG_BASE_PHYS + 0x00000ff0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_OFFS                        (AOP_QTMR_AC_REG_BASE_OFFS + 0x00000ff0)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_RMSK                        0xffffffff
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_IN          \
        in_dword_masked(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_ADDR, HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_RMSK)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_INM(m)      \
        in_dword_masked(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_ADDR, m)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_OUT(v)      \
        out_dword(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_ADDR,v)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_ADDR,m,v,HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_IN)
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_BMSK          0xffffffff
#define HWIO_AOP_QTMR_AC_HW_FRAME_SEL_2_HW_FRAME_SEL2_SHFT                 0x0


#endif /* __AOP_HWIO_H__ */
