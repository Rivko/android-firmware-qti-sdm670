#ifndef __HWIO_LPASS_TCSR_H__
#define __HWIO_LPASS_TCSR_H__
/*
===========================================================================
*/
/**
  @file hwio_lpass_tcsr.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    SDM670 (Warlock) [napaliq_v1.0_p3q2r100_v3_BTO]
 
  This file contains HWIO register definitions for the following modules:
    LPASS_LPASS_TCSR_REGS


  Generation parameters: 
  { u'filename': u'hwio_lpass_tcsr.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'LPASS_LPASS_TCSR_REGS'],
    u'output-offsets': True}
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

  $Header: //components/rel/core.qdsp6/2.1.c4/power/dangergen/src/target/670/hwio_lpass_tcsr.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: LPASS_LPASS_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define LPASS_LPASS_TCSR_REGS_REG_BASE                                              (LPASS_BASE      + 0x00b68000)
#define LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS                                         0x00b68000

#define HWIO_LPASS_HW_VERSION_ADDR                                                  (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00000000)
#define HWIO_LPASS_HW_VERSION_OFFS                                                  (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00000000)
#define HWIO_LPASS_HW_VERSION_RMSK                                                  0xffffffff
#define HWIO_LPASS_HW_VERSION_IN          \
        in_dword_masked(HWIO_LPASS_HW_VERSION_ADDR, HWIO_LPASS_HW_VERSION_RMSK)
#define HWIO_LPASS_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_LPASS_HW_VERSION_ADDR, m)
#define HWIO_LPASS_HW_VERSION_MAJOR_BMSK                                            0xf0000000
#define HWIO_LPASS_HW_VERSION_MAJOR_SHFT                                                  0x1c
#define HWIO_LPASS_HW_VERSION_MINOR_BMSK                                             0xfff0000
#define HWIO_LPASS_HW_VERSION_MINOR_SHFT                                                  0x10
#define HWIO_LPASS_HW_VERSION_STEP_BMSK                                                 0xffff
#define HWIO_LPASS_HW_VERSION_STEP_SHFT                                                    0x0

#define HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR                                             (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00001000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_OFFS                                             (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00001000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_RMSK                                             0x8000001f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR, HWIO_LPASS_VFR_IRQ_MUX_CTL_RMSK)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR, m)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR,v)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_VFR_IRQ_MUX_CTL_ADDR,m,v,HWIO_LPASS_VFR_IRQ_MUX_CTL_IN)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_VFR_BMSK                                         0x80000000
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_VFR_SHFT                                               0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_SEL_BMSK                                               0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_SEL_SHFT                                                0x0

#define HWIO_LPASS_VFR_STC_MSB_ADDR                                                 (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00001004)
#define HWIO_LPASS_VFR_STC_MSB_OFFS                                                 (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00001004)
#define HWIO_LPASS_VFR_STC_MSB_RMSK                                                   0xffffff
#define HWIO_LPASS_VFR_STC_MSB_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_ADDR, HWIO_LPASS_VFR_STC_MSB_RMSK)
#define HWIO_LPASS_VFR_STC_MSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_ADDR, m)
#define HWIO_LPASS_VFR_STC_MSB_MSB_BMSK                                               0xffffff
#define HWIO_LPASS_VFR_STC_MSB_MSB_SHFT                                                    0x0

#define HWIO_LPASS_VFR_STC_LSB_ADDR                                                 (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00001008)
#define HWIO_LPASS_VFR_STC_LSB_OFFS                                                 (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00001008)
#define HWIO_LPASS_VFR_STC_LSB_RMSK                                                 0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_ADDR, HWIO_LPASS_VFR_STC_LSB_RMSK)
#define HWIO_LPASS_VFR_STC_LSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_ADDR, m)
#define HWIO_LPASS_VFR_STC_LSB_LSB_BMSK                                             0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_LSB_SHFT                                                    0x0

#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR                                      (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x0000100c)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_OFFS                                      (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x0000100c)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_RMSK                                            0xff
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_IN          \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR, HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_RMSK)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_INM(m)      \
        in_dword_masked(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR, m)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_OUT(v)      \
        out_dword(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR,v)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_ADDR,m,v,HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_IN)
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_LPASS_CORE_CLAMP_ZERO_DELAY_BMSK                0xff
#define HWIO_LPASS_AUDIO_CORE_CLAMP_DELAY_LPASS_CORE_CLAMP_ZERO_DELAY_SHFT                 0x0

#define HWIO_LPASS_DBG_CLK_ADDR                                                     (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00001010)
#define HWIO_LPASS_DBG_CLK_OFFS                                                     (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00001010)
#define HWIO_LPASS_DBG_CLK_RMSK                                                            0xf
#define HWIO_LPASS_DBG_CLK_IN          \
        in_dword_masked(HWIO_LPASS_DBG_CLK_ADDR, HWIO_LPASS_DBG_CLK_RMSK)
#define HWIO_LPASS_DBG_CLK_INM(m)      \
        in_dword_masked(HWIO_LPASS_DBG_CLK_ADDR, m)
#define HWIO_LPASS_DBG_CLK_OUT(v)      \
        out_dword(HWIO_LPASS_DBG_CLK_ADDR,v)
#define HWIO_LPASS_DBG_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_DBG_CLK_ADDR,m,v,HWIO_LPASS_DBG_CLK_IN)
#define HWIO_LPASS_DBG_CLK_RESETN_MUX_SEL_BMSK                                             0xc
#define HWIO_LPASS_DBG_CLK_RESETN_MUX_SEL_SHFT                                             0x2
#define HWIO_LPASS_DBG_CLK_BYPASSNL_MUX_SEL_BMSK                                           0x3
#define HWIO_LPASS_DBG_CLK_BYPASSNL_MUX_SEL_SHFT                                           0x0

#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR                                           (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00002000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_OFFS                                           (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00002000)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_RMSK                                           0x8000001f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR, HWIO_LPASS_VFR_IRQ_MUX_CTL_1_RMSK)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR, m)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_OUT(v)      \
        out_dword(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR,v)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_VFR_IRQ_MUX_CTL_1_ADDR,m,v,HWIO_LPASS_VFR_IRQ_MUX_CTL_1_IN)
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_VFR_BMSK                                       0x80000000
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_VFR_SHFT                                             0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_SEL_BMSK                                             0x1f
#define HWIO_LPASS_VFR_IRQ_MUX_CTL_1_SEL_SHFT                                              0x0

#define HWIO_LPASS_VFR_STC_MSB_1_ADDR                                               (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00002004)
#define HWIO_LPASS_VFR_STC_MSB_1_OFFS                                               (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00002004)
#define HWIO_LPASS_VFR_STC_MSB_1_RMSK                                                 0xffffff
#define HWIO_LPASS_VFR_STC_MSB_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_1_ADDR, HWIO_LPASS_VFR_STC_MSB_1_RMSK)
#define HWIO_LPASS_VFR_STC_MSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_MSB_1_ADDR, m)
#define HWIO_LPASS_VFR_STC_MSB_1_MSB_BMSK                                             0xffffff
#define HWIO_LPASS_VFR_STC_MSB_1_MSB_SHFT                                                  0x0

#define HWIO_LPASS_VFR_STC_LSB_1_ADDR                                               (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00002008)
#define HWIO_LPASS_VFR_STC_LSB_1_OFFS                                               (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00002008)
#define HWIO_LPASS_VFR_STC_LSB_1_RMSK                                               0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_1_IN          \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_1_ADDR, HWIO_LPASS_VFR_STC_LSB_1_RMSK)
#define HWIO_LPASS_VFR_STC_LSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_VFR_STC_LSB_1_ADDR, m)
#define HWIO_LPASS_VFR_STC_LSB_1_LSB_BMSK                                           0xffffffff
#define HWIO_LPASS_VFR_STC_LSB_1_LSB_SHFT                                                  0x0

#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR                                       (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00003000)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_OFFS                                       (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00003000)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_RMSK                                       0x8000ffff
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR, HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR,v)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_ADDR,m,v,HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_IN)
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_VFR_BMSK                                   0x80000000
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_VFR_SHFT                                         0x1f
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_SEL_BMSK                                       0xffff
#define HWIO_LPASS_SENSOR_IRQ_TS_MUX_CTL_SEL_SHFT                                          0x0

#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR                                          (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00003004)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_OFFS                                          (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00003004)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_RMSK                                            0xffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR, HWIO_LPASS_SENSOR_IRQ_STC_MSB_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_MSB_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_MSB_BMSK                                        0xffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_MSB_MSB_SHFT                                             0x0

#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR                                          (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00003008)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_OFFS                                          (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00003008)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_RMSK                                          0xffffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_IN          \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR, HWIO_LPASS_SENSOR_IRQ_STC_LSB_RMSK)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_INM(m)      \
        in_dword_masked(HWIO_LPASS_SENSOR_IRQ_STC_LSB_ADDR, m)
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_LSB_BMSK                                      0xffffffff
#define HWIO_LPASS_SENSOR_IRQ_STC_LSB_LSB_SHFT                                             0x0

#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR                                         (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00004000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_OFFS                                         (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00004000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_RMSK                                         0x8000ffff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR, HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_RMSK)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_OUT(v)      \
        out_dword(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR,v)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_ADDR,m,v,HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_IN)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_VFR_BMSK                                     0x80000000
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_VFR_SHFT                                           0x1f
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_SEL_BMSK                                         0xffff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_0_SEL_SHFT                                            0x0

#define HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR                                            (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00004004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_OFFS                                            (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00004004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_RMSK                                              0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR, HWIO_LPASS_GP_IRQ_STC_MSB_0_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_MSB_BMSK                                          0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_0_MSB_SHFT                                               0x0

#define HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR                                            (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00004008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_OFFS                                            (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00004008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_RMSK                                            0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR, HWIO_LPASS_GP_IRQ_STC_LSB_0_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_0_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_LSB_BMSK                                        0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_0_LSB_SHFT                                               0x0

#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR                                         (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00005000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_OFFS                                         (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00005000)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_RMSK                                         0x8000ffff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR, HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_RMSK)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_OUT(v)      \
        out_dword(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR,v)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_ADDR,m,v,HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_IN)
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_VFR_BMSK                                     0x80000000
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_VFR_SHFT                                           0x1f
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_SEL_BMSK                                         0xffff
#define HWIO_LPASS_GP_IRQ_TS_MUX_CTL_1_SEL_SHFT                                            0x0

#define HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR                                            (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00005004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_OFFS                                            (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00005004)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_RMSK                                              0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR, HWIO_LPASS_GP_IRQ_STC_MSB_1_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_MSB_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_MSB_BMSK                                          0xffffff
#define HWIO_LPASS_GP_IRQ_STC_MSB_1_MSB_SHFT                                               0x0

#define HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR                                            (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00005008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_OFFS                                            (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00005008)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_RMSK                                            0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_IN          \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR, HWIO_LPASS_GP_IRQ_STC_LSB_1_RMSK)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_INM(m)      \
        in_dword_masked(HWIO_LPASS_GP_IRQ_STC_LSB_1_ADDR, m)
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_LSB_BMSK                                        0xffffffff
#define HWIO_LPASS_GP_IRQ_STC_LSB_1_LSB_SHFT                                               0x0

#define HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR                                              (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00006000)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_OFFS                                              (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00006000)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_RMSK                                                0xffffff
#define HWIO_LPASS_SB_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR, HWIO_LPASS_SB_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SB_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_SB_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_23_BMSK                          0x800000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_23_SHFT                              0x17
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_22_BMSK                          0x400000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_22_SHFT                              0x16
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_21_BMSK                          0x200000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_21_SHFT                              0x15
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_20_BMSK                          0x100000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_20_SHFT                              0x14
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_19_BMSK                           0x80000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_19_SHFT                              0x13
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_18_BMSK                           0x40000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_18_SHFT                              0x12
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_17_BMSK                           0x20000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_17_SHFT                              0x11
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_16_BMSK                           0x10000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_16_SHFT                              0x10
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_15_BMSK                            0x8000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_15_SHFT                               0xf
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_14_BMSK                            0x4000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_14_SHFT                               0xe
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_13_BMSK                            0x2000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_13_SHFT                               0xd
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_12_BMSK                            0x1000
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_12_SHFT                               0xc
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_11_BMSK                             0x800
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_11_SHFT                               0xb
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_10_BMSK                             0x400
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_10_SHFT                               0xa
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_9_BMSK                              0x200
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_9_SHFT                                0x9
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_8_BMSK                              0x100
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_8_SHFT                                0x8
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_7_BMSK                               0x80
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_7_SHFT                                0x7
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_6_BMSK                               0x40
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_6_SHFT                                0x6
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_5_BMSK                               0x20
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_5_SHFT                                0x5
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_4_BMSK                               0x10
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_4_SHFT                                0x4
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_3_BMSK                                0x8
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_3_SHFT                                0x3
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_2_BMSK                                0x4
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_2_SHFT                                0x2
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_1_BMSK                                0x2
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_1_SHFT                                0x1
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_0_BMSK                                0x1
#define HWIO_LPASS_SB_VFR_IRQ_CTL_SB_VFR_IRQ_CTL_SEL_0_SHFT                                0x0

#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR                                           (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00006004)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_OFFS                                           (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00006004)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_RMSK                                                0xfff
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR, HWIO_LPASS_LPAIF_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_LPAIF_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_LPAIF_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_5_BMSK                  0x800
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_5_SHFT                    0xb
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_4_BMSK                  0x400
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_4_SHFT                    0xa
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_3_BMSK                  0x200
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_3_SHFT                    0x9
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_2_BMSK                  0x100
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_2_SHFT                    0x8
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_1_BMSK                   0x80
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_1_SHFT                    0x7
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_0_BMSK                   0x40
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RWDMA_SEL_0_SHFT                    0x6
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_5_BMSK                    0x20
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_5_SHFT                     0x5
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_4_BMSK                    0x10
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_4_SHFT                     0x4
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_3_BMSK                     0x8
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_3_SHFT                     0x3
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_2_BMSK                     0x4
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_2_SHFT                     0x2
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_1_BMSK                     0x2
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_1_SHFT                     0x1
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_0_BMSK                     0x1
#define HWIO_LPASS_LPAIF_VFR_IRQ_CTL_LPAIF_VFR_IRQ_CTL_RDMA_SEL_0_SHFT                     0x0

#define HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR                                             (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00006008)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_OFFS                                             (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00006008)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_RMSK                                                    0x1
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR, HWIO_LPASS_WCN_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_WCN_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_WCN_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_SEL_BMSK                                                0x1
#define HWIO_LPASS_WCN_VFR_IRQ_CTL_SEL_SHFT                                                0x0

#define HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR                                             (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x0000600c)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_OFFS                                             (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x0000600c)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_RMSK                                                    0x1
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR, HWIO_LPASS_AVT_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_AVT_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_AVT_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_SEL_BMSK                                                0x1
#define HWIO_LPASS_AVT_VFR_IRQ_CTL_SEL_SHFT                                                0x0

#define HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR                                             (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00006010)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_OFFS                                             (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00006010)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_RMSK                                               0xffffff
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_IN          \
        in_dword_masked(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR, HWIO_LPASS_SB1_VFR_IRQ_CTL_RMSK)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR, m)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR,v)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_SB1_VFR_IRQ_CTL_ADDR,m,v,HWIO_LPASS_SB1_VFR_IRQ_CTL_IN)
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_23_BMSK                        0x800000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_23_SHFT                            0x17
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_22_BMSK                        0x400000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_22_SHFT                            0x16
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_21_BMSK                        0x200000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_21_SHFT                            0x15
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_20_BMSK                        0x100000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_20_SHFT                            0x14
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_19_BMSK                         0x80000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_19_SHFT                            0x13
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_18_BMSK                         0x40000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_18_SHFT                            0x12
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_17_BMSK                         0x20000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_17_SHFT                            0x11
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_16_BMSK                         0x10000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_16_SHFT                            0x10
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_15_BMSK                          0x8000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_15_SHFT                             0xf
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_14_BMSK                          0x4000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_14_SHFT                             0xe
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_13_BMSK                          0x2000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_13_SHFT                             0xd
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_12_BMSK                          0x1000
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_12_SHFT                             0xc
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_11_BMSK                           0x800
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_11_SHFT                             0xb
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_10_BMSK                           0x400
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_10_SHFT                             0xa
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_9_BMSK                            0x200
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_9_SHFT                              0x9
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_8_BMSK                            0x100
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_8_SHFT                              0x8
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_7_BMSK                             0x80
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_7_SHFT                              0x7
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_6_BMSK                             0x40
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_6_SHFT                              0x6
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_5_BMSK                             0x20
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_5_SHFT                              0x5
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_4_BMSK                             0x10
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_4_SHFT                              0x4
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_3_BMSK                              0x8
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_3_SHFT                              0x3
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_2_BMSK                              0x4
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_2_SHFT                              0x2
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_1_BMSK                              0x2
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_1_SHFT                              0x1
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_0_BMSK                              0x1
#define HWIO_LPASS_SB1_VFR_IRQ_CTL_SB1_VFR_IRQ_CTL_SEL_0_SHFT                              0x0

#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_ADDR                                  (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00007000)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_OFFS                                  (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00007000)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_RMSK                                      0x3333
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_IN          \
        in_dword_masked(HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_ADDR, HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_RMSK)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_ADDR, m)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_OUT(v)      \
        out_dword(HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_ADDR,v)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_ADDR,m,v,HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_IN)
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA2_BMSK                                0x3000
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA2_SHFT                                   0xc
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA1_BMSK                                 0x300
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA1_SHFT                                   0x8
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA0_BMSK                                  0x30
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_DATA0_SHFT                                   0x4
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_CLK_BMSK                                     0x3
#define HWIO_LPASS_PAD_AUD_SB_SLEWRATE_CTL_EN_CLK_SHFT                                     0x0

#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_ADDR                                  (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00008000)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_OFFS                                  (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00008000)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_RMSK                                        0x33
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_IN          \
        in_dword_masked(HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_ADDR, HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_RMSK)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_INM(m)      \
        in_dword_masked(HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_ADDR, m)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_OUT(v)      \
        out_dword(HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_ADDR,v)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_ADDR,m,v,HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_IN)
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_DATA0_BMSK                                  0x30
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_DATA0_SHFT                                   0x4
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_CLK_BMSK                                     0x3
#define HWIO_LPASS_PAD_QCA_SB_SLEWRATE_CTL_EN_CLK_SHFT                                     0x0

#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_ADDR                                          (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x00009000)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_OFFS                                          (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x00009000)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_RMSK                                                 0x1
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_IN          \
        in_dword_masked(HWIO_LPASS_TCSR_LPASS_QSM_CTL_ADDR, HWIO_LPASS_TCSR_LPASS_QSM_CTL_RMSK)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_INM(m)      \
        in_dword_masked(HWIO_LPASS_TCSR_LPASS_QSM_CTL_ADDR, m)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_OUT(v)      \
        out_dword(HWIO_LPASS_TCSR_LPASS_QSM_CTL_ADDR,v)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_TCSR_LPASS_QSM_CTL_ADDR,m,v,HWIO_LPASS_TCSR_LPASS_QSM_CTL_IN)
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_LPASS_QSM_ENABLE_BMSK                                0x1
#define HWIO_LPASS_TCSR_LPASS_QSM_CTL_LPASS_QSM_ENABLE_SHFT                                0x0

#define HWIO_LPASS_CODEC_ADDR                                                       (LPASS_LPASS_TCSR_REGS_REG_BASE      + 0x0000a000)
#define HWIO_LPASS_CODEC_OFFS                                                       (LPASS_LPASS_TCSR_REGS_REG_BASE_OFFS + 0x0000a000)
#define HWIO_LPASS_CODEC_RMSK                                                              0x1
#define HWIO_LPASS_CODEC_IN          \
        in_dword_masked(HWIO_LPASS_CODEC_ADDR, HWIO_LPASS_CODEC_RMSK)
#define HWIO_LPASS_CODEC_INM(m)      \
        in_dword_masked(HWIO_LPASS_CODEC_ADDR, m)
#define HWIO_LPASS_CODEC_OUT(v)      \
        out_dword(HWIO_LPASS_CODEC_ADDR,v)
#define HWIO_LPASS_CODEC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_LPASS_CODEC_ADDR,m,v,HWIO_LPASS_CODEC_IN)
#define HWIO_LPASS_CODEC_RST_N_BMSK                                                        0x1
#define HWIO_LPASS_CODEC_RST_N_SHFT                                                        0x0


#endif /* __HWIO_LPASS_TCSR_H__ */
