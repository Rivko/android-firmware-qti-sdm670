#ifndef __BCM_WA_HWIO_H__
#define __BCM_WA_HWIO_H__
/*
===========================================================================
*/
/**
  @file bcm_wa_hwio.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG

  'Include' filters applied: MMNOC[GCC_CLK_CTL_REG] GDS_HW_CTRL_SW[GCC_CLK_CTL_REG] 
  'Exclude' filters applied: RESERVED DUMMY 
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

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/systemdrivers/icb/src/670/bcm_wa_hwio.h#1 $
  $DateTime: 2018/02/16 04:48:18 $
  $Author: pwbldsvc $

  ===========================================================================
*/


#include <HALhwio.h>
#include <msmhwiobase.h>


/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                                   (CLK_CTL_BASE      + 0x00000000)

#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00005028)
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_RMSK                                                       0x80000005
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_ADDR, HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_RMSK)
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_ADDR, m)
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_ADDR,v)
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_ADDR,m,v,HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_IN)
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_ARES_BMSK                                                     0x4
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_ARES_SHFT                                                     0x2
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_CFG_NOC_MMNOC_AHB_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x000052c0)
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_RMSK                                                      0x1
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_IN          \
        in_dword_masked(HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_ADDR, HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_RMSK)
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_INM(m)      \
        in_dword_masked(HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_ADDR, m)
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_OUT(v)      \
        out_dword(HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_ADDR,v)
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_ADDR,m,v,HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_IN)
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_DCD_ENABLE_BMSK                                           0x1
#define HWIO_GCC_CONFIG_NOC_MMNOC_DCD_CDIV_DCDR_DCD_ENABLE_SHFT                                           0x0

#define HWIO_GCC_MMNOC_BCR_ADDR                                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00009000)
#define HWIO_GCC_MMNOC_BCR_RMSK                                                                           0x1
#define HWIO_GCC_MMNOC_BCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_BCR_ADDR, HWIO_GCC_MMNOC_BCR_RMSK)
#define HWIO_GCC_MMNOC_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_BCR_ADDR, m)
#define HWIO_GCC_MMNOC_BCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_BCR_ADDR,v)
#define HWIO_GCC_MMNOC_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_BCR_ADDR,m,v,HWIO_GCC_MMNOC_BCR_IN)
#define HWIO_GCC_MMNOC_BCR_BLK_ARES_BMSK                                                                  0x1
#define HWIO_GCC_MMNOC_BCR_BLK_ARES_SHFT                                                                  0x0

#define HWIO_GCC_MMNOC_GDSCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00009004)
#define HWIO_GCC_MMNOC_GDSCR_RMSK                                                                  0xf8ffffff
#define HWIO_GCC_MMNOC_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDSCR_ADDR, HWIO_GCC_MMNOC_GDSCR_RMSK)
#define HWIO_GCC_MMNOC_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDSCR_ADDR, m)
#define HWIO_GCC_MMNOC_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDSCR_ADDR,v)
#define HWIO_GCC_MMNOC_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDSCR_ADDR,m,v,HWIO_GCC_MMNOC_GDSCR_IN)
#define HWIO_GCC_MMNOC_GDSCR_PWR_ON_BMSK                                                           0x80000000
#define HWIO_GCC_MMNOC_GDSCR_PWR_ON_SHFT                                                                 0x1f
#define HWIO_GCC_MMNOC_GDSCR_GDSC_STATE_BMSK                                                       0x78000000
#define HWIO_GCC_MMNOC_GDSCR_GDSC_STATE_SHFT                                                             0x1b
#define HWIO_GCC_MMNOC_GDSCR_EN_REST_WAIT_BMSK                                                       0xf00000
#define HWIO_GCC_MMNOC_GDSCR_EN_REST_WAIT_SHFT                                                           0x14
#define HWIO_GCC_MMNOC_GDSCR_EN_FEW_WAIT_BMSK                                                         0xf0000
#define HWIO_GCC_MMNOC_GDSCR_EN_FEW_WAIT_SHFT                                                            0x10
#define HWIO_GCC_MMNOC_GDSCR_CLK_DIS_WAIT_BMSK                                                         0xf000
#define HWIO_GCC_MMNOC_GDSCR_CLK_DIS_WAIT_SHFT                                                            0xc
#define HWIO_GCC_MMNOC_GDSCR_RETAIN_FF_ENABLE_BMSK                                                      0x800
#define HWIO_GCC_MMNOC_GDSCR_RETAIN_FF_ENABLE_SHFT                                                        0xb
#define HWIO_GCC_MMNOC_GDSCR_RESTORE_BMSK                                                               0x400
#define HWIO_GCC_MMNOC_GDSCR_RESTORE_SHFT                                                                 0xa
#define HWIO_GCC_MMNOC_GDSCR_SAVE_BMSK                                                                  0x200
#define HWIO_GCC_MMNOC_GDSCR_SAVE_SHFT                                                                    0x9
#define HWIO_GCC_MMNOC_GDSCR_RETAIN_BMSK                                                                0x100
#define HWIO_GCC_MMNOC_GDSCR_RETAIN_SHFT                                                                  0x8
#define HWIO_GCC_MMNOC_GDSCR_EN_REST_BMSK                                                                0x80
#define HWIO_GCC_MMNOC_GDSCR_EN_REST_SHFT                                                                 0x7
#define HWIO_GCC_MMNOC_GDSCR_EN_FEW_BMSK                                                                 0x40
#define HWIO_GCC_MMNOC_GDSCR_EN_FEW_SHFT                                                                  0x6
#define HWIO_GCC_MMNOC_GDSCR_CLAMP_IO_BMSK                                                               0x20
#define HWIO_GCC_MMNOC_GDSCR_CLAMP_IO_SHFT                                                                0x5
#define HWIO_GCC_MMNOC_GDSCR_CLK_DISABLE_BMSK                                                            0x10
#define HWIO_GCC_MMNOC_GDSCR_CLK_DISABLE_SHFT                                                             0x4
#define HWIO_GCC_MMNOC_GDSCR_PD_ARES_BMSK                                                                 0x8
#define HWIO_GCC_MMNOC_GDSCR_PD_ARES_SHFT                                                                 0x3
#define HWIO_GCC_MMNOC_GDSCR_SW_OVERRIDE_BMSK                                                             0x4
#define HWIO_GCC_MMNOC_GDSCR_SW_OVERRIDE_SHFT                                                             0x2
#define HWIO_GCC_MMNOC_GDSCR_HW_CONTROL_BMSK                                                              0x2
#define HWIO_GCC_MMNOC_GDSCR_HW_CONTROL_SHFT                                                              0x1
#define HWIO_GCC_MMNOC_GDSCR_SW_COLLAPSE_BMSK                                                             0x1
#define HWIO_GCC_MMNOC_GDSCR_SW_COLLAPSE_SHFT                                                             0x0

#define HWIO_GCC_MMNOC_CFG_GDSCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00009008)
#define HWIO_GCC_MMNOC_CFG_GDSCR_RMSK                                                              0xffffffff
#define HWIO_GCC_MMNOC_CFG_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_CFG_GDSCR_ADDR, HWIO_GCC_MMNOC_CFG_GDSCR_RMSK)
#define HWIO_GCC_MMNOC_CFG_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_CFG_GDSCR_ADDR, m)
#define HWIO_GCC_MMNOC_CFG_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_CFG_GDSCR_ADDR,v)
#define HWIO_GCC_MMNOC_CFG_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_CFG_GDSCR_ADDR,m,v,HWIO_GCC_MMNOC_CFG_GDSCR_IN)
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_SPARE_CTRL_IN_BMSK                                           0xf0000000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_SPARE_CTRL_IN_SHFT                                                 0x1c
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_BMSK                                           0xc000000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_SPARE_CTRL_OUT_SHFT                                                0x1a
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PWR_DWN_START_BMSK                                            0x2000000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PWR_DWN_START_SHFT                                                 0x19
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PWR_UP_START_BMSK                                             0x1000000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PWR_UP_START_SHFT                                                  0x18
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_BMSK                                      0xf00000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_CFG_FSM_STATE_STATUS_SHFT                                          0x14
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_BMSK                                         0x80000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_PWR_ACK_STATUS_SHFT                                            0x13
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_ENR_ACK_STATUS_BMSK                                             0x40000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_ENR_ACK_STATUS_SHFT                                                0x12
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_ENF_ACK_STATUS_BMSK                                             0x20000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_ENF_ACK_STATUS_SHFT                                                0x11
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_BMSK                                          0x10000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_POWER_UP_COMPLETE_SHFT                                             0x10
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_BMSK                                         0x8000
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_POWER_DOWN_COMPLETE_SHFT                                            0xf
#define HWIO_GCC_MMNOC_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_BMSK                                        0x7800
#define HWIO_GCC_MMNOC_CFG_GDSCR_SOFTWARE_CONTROL_OVERRIDE_SHFT                                           0xb
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_HANDSHAKE_DIS_BMSK                                                0x400
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_HANDSHAKE_DIS_SHFT                                                  0xa
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_BMSK                                         0x200
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_PERI_FORCE_IN_SW_SHFT                                           0x9
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_BMSK                                         0x100
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_MEM_CORE_FORCE_IN_SW_SHFT                                           0x8
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_BMSK                                             0x80
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PHASE_RESET_EN_SW_SHFT                                              0x7
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_BMSK                                    0x60
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PHASE_RESET_DELAY_COUNT_SW_SHFT                                     0x5
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_BMSK                                              0x10
#define HWIO_GCC_MMNOC_CFG_GDSCR_GDSC_PSCBC_PWR_DWN_SW_SHFT                                               0x4
#define HWIO_GCC_MMNOC_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_BMSK                                        0x8
#define HWIO_GCC_MMNOC_CFG_GDSCR_UNCLAMP_IO_SOFTWARE_OVERRIDE_SHFT                                        0x3
#define HWIO_GCC_MMNOC_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_BMSK                                      0x4
#define HWIO_GCC_MMNOC_CFG_GDSCR_SAVE_RESTORE_SOFTWARE_OVERRIDE_SHFT                                      0x2
#define HWIO_GCC_MMNOC_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_BMSK                                          0x2
#define HWIO_GCC_MMNOC_CFG_GDSCR_CLAMP_IO_SOFTWARE_OVERRIDE_SHFT                                          0x1
#define HWIO_GCC_MMNOC_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_BMSK                                       0x1
#define HWIO_GCC_MMNOC_CFG_GDSCR_DISABLE_CLK_SOFTWARE_OVERRIDE_SHFT                                       0x0

#define HWIO_GCC_MMNOC_TBU_SF_CBCR_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000900c)
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_RMSK                                                            0x80007ff5
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_SF_CBCR_ADDR, HWIO_GCC_MMNOC_TBU_SF_CBCR_RMSK)
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_SF_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_SF_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_SF_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_SF_CBCR_IN)
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_OFF_BMSK                                                    0x80000000
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_OFF_SHFT                                                          0x1f
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_CORE_ON_BMSK                                              0x4000
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_CORE_ON_SHFT                                                 0xe
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                            0x2000
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                               0xd
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                           0x1000
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                              0xc
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_WAKEUP_BMSK                                                          0xf00
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_WAKEUP_SHFT                                                            0x8
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_SLEEP_BMSK                                                            0xf0
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_SLEEP_SHFT                                                             0x4
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_ARES_BMSK                                                          0x4
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_ARES_SHFT                                                          0x2
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_MMNOC_TBU_SF_CBCR_CLK_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_MMNOC_TBU_SF_SREGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009010)
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_RMSK                                                           0xfffff37e
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_SF_SREGR_ADDR, HWIO_GCC_MMNOC_TBU_SF_SREGR_RMSK)
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_SF_SREGR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_SF_SREGR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_SF_SREGR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_SF_SREGR_IN)
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_BMSK                                 0xff000000
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_SHFT                                       0x18
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SREG_PSCBC_SPARE_CTRL_IN_BMSK                                    0xff0000
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SREG_PSCBC_SPARE_CTRL_IN_SHFT                                        0x10
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_RESERVE_BITS15_13_BMSK                                             0xe000
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_RESERVE_BITS15_13_SHFT                                                0xd
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_BMSK                                   0x1000
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_SHFT                                      0xc
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_DIV_RATIO_SLP_STG_CLK_BMSK                                       0x300
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_DIV_RATIO_SLP_STG_CLK_SHFT                                         0x8
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_FORCE_CLK_ON_BMSK                                                    0x40
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_FORCE_CLK_ON_SHFT                                                     0x6
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_RST_SEL_SLP_STG_BMSK                                              0x20
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_RST_SEL_SLP_STG_SHFT                                               0x5
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_RST_SLP_STG_BMSK                                                  0x10
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_RST_SLP_STG_SHFT                                                   0x4
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CTRL_PWR_DOWN_BMSK                                                 0x8
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CTRL_PWR_DOWN_SHFT                                                 0x3
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CLK_EN_SEL_SLP_STG_BMSK                                            0x4
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CLK_EN_SEL_SLP_STG_SHFT                                            0x2
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CLK_EN_SLP_STG_BMSK                                                0x2
#define HWIO_GCC_MMNOC_TBU_SF_SREGR_SW_CLK_EN_SLP_STG_SHFT                                                0x1

#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009014)
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_RMSK                                                           0x80007ff5
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR, HWIO_GCC_MMNOC_TBU_HF0_CBCR_RMSK)
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_HF0_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_HF0_CBCR_IN)
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_ARES_BMSK                                                         0x4
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_ARES_SHFT                                                         0x2
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MMNOC_TBU_HF0_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00009018)
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_RMSK                                                          0xfffff37e
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF0_SREGR_ADDR, HWIO_GCC_MMNOC_TBU_HF0_SREGR_RMSK)
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF0_SREGR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_HF0_SREGR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_HF0_SREGR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_HF0_SREGR_IN)
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_BMSK                                0xff000000
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_SHFT                                      0x18
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SREG_PSCBC_SPARE_CTRL_IN_BMSK                                   0xff0000
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SREG_PSCBC_SPARE_CTRL_IN_SHFT                                       0x10
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_RESERVE_BITS15_13_BMSK                                            0xe000
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_RESERVE_BITS15_13_SHFT                                               0xd
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_BMSK                                  0x1000
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_SHFT                                     0xc
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_DIV_RATIO_SLP_STG_CLK_BMSK                                      0x300
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_DIV_RATIO_SLP_STG_CLK_SHFT                                        0x8
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_FORCE_CLK_ON_BMSK                                                   0x40
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_FORCE_CLK_ON_SHFT                                                    0x6
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_RST_SEL_SLP_STG_BMSK                                             0x20
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_RST_SEL_SLP_STG_SHFT                                              0x5
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_RST_SLP_STG_BMSK                                                 0x10
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_RST_SLP_STG_SHFT                                                  0x4
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CTRL_PWR_DOWN_BMSK                                                0x8
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CTRL_PWR_DOWN_SHFT                                                0x3
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CLK_EN_SEL_SLP_STG_BMSK                                           0x4
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CLK_EN_SEL_SLP_STG_SHFT                                           0x2
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CLK_EN_SLP_STG_BMSK                                               0x2
#define HWIO_GCC_MMNOC_TBU_HF0_SREGR_SW_CLK_EN_SLP_STG_SHFT                                               0x1

#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0000901c)
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_RMSK                                                           0x80007ff5
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR, HWIO_GCC_MMNOC_TBU_HF1_CBCR_RMSK)
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_HF1_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_HF1_CBCR_IN)
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_ARES_BMSK                                                         0x4
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_ARES_SHFT                                                         0x2
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MMNOC_TBU_HF1_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00009020)
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_RMSK                                                          0xfffff37e
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF1_SREGR_ADDR, HWIO_GCC_MMNOC_TBU_HF1_SREGR_RMSK)
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_TBU_HF1_SREGR_ADDR, m)
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_TBU_HF1_SREGR_ADDR,v)
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_TBU_HF1_SREGR_ADDR,m,v,HWIO_GCC_MMNOC_TBU_HF1_SREGR_IN)
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_BMSK                                0xff000000
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SREG_PSCBC_SPARE_CTRL_OUT_SHFT                                      0x18
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SREG_PSCBC_SPARE_CTRL_IN_BMSK                                   0xff0000
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SREG_PSCBC_SPARE_CTRL_IN_SHFT                                       0x10
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_RESERVE_BITS15_13_BMSK                                            0xe000
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_RESERVE_BITS15_13_SHFT                                               0xd
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_BMSK                                  0x1000
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_SM_PSCBC_SEQ_IN_OVERRIDE_SHFT                                     0xc
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_DIV_RATIO_SLP_STG_CLK_BMSK                                      0x300
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_DIV_RATIO_SLP_STG_CLK_SHFT                                        0x8
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_FORCE_CLK_ON_BMSK                                                   0x40
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_FORCE_CLK_ON_SHFT                                                    0x6
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_RST_SEL_SLP_STG_BMSK                                             0x20
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_RST_SEL_SLP_STG_SHFT                                              0x5
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_RST_SLP_STG_BMSK                                                 0x10
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_RST_SLP_STG_SHFT                                                  0x4
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CTRL_PWR_DOWN_BMSK                                                0x8
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CTRL_PWR_DOWN_SHFT                                                0x3
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CLK_EN_SEL_SLP_STG_BMSK                                           0x4
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CLK_EN_SEL_SLP_STG_SHFT                                           0x2
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CLK_EN_SLP_STG_BMSK                                               0x2
#define HWIO_GCC_MMNOC_TBU_HF1_SREGR_SW_CLK_EN_SLP_STG_SHFT                                               0x1

#define HWIO_GCC_MMNOC_AT_CBCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00009024)
#define HWIO_GCC_MMNOC_AT_CBCR_RMSK                                                                0x80000005
#define HWIO_GCC_MMNOC_AT_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_AT_CBCR_ADDR, HWIO_GCC_MMNOC_AT_CBCR_RMSK)
#define HWIO_GCC_MMNOC_AT_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_AT_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_AT_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_AT_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_AT_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_AT_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_AT_CBCR_IN)
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_OFF_BMSK                                                        0x80000000
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_OFF_SHFT                                                              0x1f
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_ARES_BMSK                                                              0x4
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_ARES_SHFT                                                              0x2
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_ENABLE_BMSK                                                            0x1
#define HWIO_GCC_MMNOC_AT_CBCR_CLK_ENABLE_SHFT                                                            0x0

#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009028)
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_RMSK                                                           0x80000007
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR, HWIO_GCC_MMNOC_AHB_CFG_CBCR_RMSK)
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_AHB_CFG_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_AHB_CFG_CBCR_IN)
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_ARES_BMSK                                                         0x4
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_ARES_SHFT                                                         0x2
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_HW_CTL_BMSK                                                           0x2
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_HW_CTL_SHFT                                                           0x1
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_MMNOC_AHB_CFG_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0000902c)
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_RMSK                                                        0x80000005
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_IN          \
        in_dword_masked(HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR, HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_RMSK)
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR, m)
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR,v)
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_ADDR,m,v,HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_IN)
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_ARES_BMSK                                                      0x4
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_ARES_SHFT                                                      0x2
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_NOC_MMNOC_DCD_XO_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00009030)
#define HWIO_GCC_MMNOC_HF_QX_CBCR_RMSK                                                             0x80000005
#define HWIO_GCC_MMNOC_HF_QX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR, HWIO_GCC_MMNOC_HF_QX_CBCR_RMSK)
#define HWIO_GCC_MMNOC_HF_QX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_HF_QX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_HF_QX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_HF_QX_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_HF_QX_CBCR_IN)
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_ARES_BMSK                                                           0x4
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_ARES_SHFT                                                           0x2
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_MMNOC_HF_QX_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00009048)
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_RMSK                                                             0x1ffff
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_CMD_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_CMD_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_CMD_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_CMD_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_CMD_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_CMD_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_RCG_SW_CTRL_BMSK                                                 0x18000
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_RCG_SW_CTRL_SHFT                                                     0xf
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_SW_PERF_STATE_BMSK                                                0x7800
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_SW_PERF_STATE_SHFT                                                   0xb
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_SW_OVERRIDE_BMSK                                                   0x400
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_SW_OVERRIDE_SHFT                                                     0xa
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_PERF_STATE_UPDATE_STATUS_BMSK                                      0x200
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_PERF_STATE_UPDATE_STATUS_SHFT                                        0x9
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_DFS_FSM_STATE_BMSK                                                 0x1c0
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_DFS_FSM_STATE_SHFT                                                   0x6
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_HW_CLK_CONTROL_BMSK                                                 0x20
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_HW_CLK_CONTROL_SHFT                                                  0x5
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_CURR_PERF_STATE_BMSK                                                0x1e
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_CURR_PERF_STATE_SHFT                                                 0x1
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_DFS_EN_BMSK                                                          0x1
#define HWIO_GCC_RPMH_MMNOC_CMD_DFSR_DFS_EN_SHFT                                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009050)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF0_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009054)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF1_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009058)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF2_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000905c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF3_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009060)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF4_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009064)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF5_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009068)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF6_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000906c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF7_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009070)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF8_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009074)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF9_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009078)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF10_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0000907c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF11_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009080)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF12_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009084)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF13_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00009088)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF14_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0000908c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_HF_QX_PERF15_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00009034)
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ADDR, HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_RMSK)
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ADDR,m,v,HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_IN)
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_MMNOC_HF_QX_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00009038)
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_RMSK                                                           0x11071f
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_ADDR, HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_RMSK)
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_ADDR,m,v,HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_IN)
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_HW_CLK_CONTROL_BMSK                                            0x100000
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_HW_CLK_CONTROL_SHFT                                                0x14
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_RCGLITE_DISABLE_BMSK                                            0x10000
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_RCGLITE_DISABLE_SHFT                                               0x10
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_MMNOC_HF_QX_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00009160)
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_RMSK                                                           0x1
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_ADDR, HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_RMSK)
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_ADDR, m)
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_ADDR,v)
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_ADDR,m,v,HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_IN)
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_DCD_ENABLE_BMSK                                                0x1
#define HWIO_GCC_MMNOC_HF_QX_DCD_CDIV_DCDR_DCD_ENABLE_SHFT                                                0x0

#define HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00009164)
#define HWIO_GCC_MMNOC_SF_QX_CBCR_RMSK                                                             0x80000005
#define HWIO_GCC_MMNOC_SF_QX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR, HWIO_GCC_MMNOC_SF_QX_CBCR_RMSK)
#define HWIO_GCC_MMNOC_SF_QX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_SF_QX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_SF_QX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_SF_QX_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_SF_QX_CBCR_IN)
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_ARES_BMSK                                                           0x4
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_ARES_SHFT                                                           0x2
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_MMNOC_SF_QX_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009184)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF0_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009188)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF1_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000918c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF2_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009190)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF3_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009194)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF4_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00009198)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF5_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0000919c)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF6_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x000091a0)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF7_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x000091a4)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF8_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x000091a8)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_RMSK                                                 0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_SRC_SEL_BMSK                                         0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_SRC_SEL_SHFT                                           0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_SRC_DIV_BMSK                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF9_DFSR_SRC_DIV_SHFT                                           0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091ac)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF10_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091b0)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF11_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091b4)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF12_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091b8)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF13_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091bc)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF14_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_ADDR                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x000091c0)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_RMSK                                                0x71f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_ADDR, HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_RMSK)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_IN)
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_SRC_SEL_BMSK                                        0x700
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_SRC_SEL_SHFT                                          0x8
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_SRC_DIV_BMSK                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_MMNOC_SF_QX_PERF15_DFSR_SRC_DIV_SHFT                                          0x0

#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00009168)
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_RMSK                                                         0x80000013
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ADDR, HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_RMSK)
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ADDR, m)
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ADDR,v)
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ADDR,m,v,HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_IN)
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ROOT_OFF_BMSK                                                0x80000000
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ROOT_OFF_SHFT                                                      0x1f
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                0x10
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                 0x4
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ROOT_EN_BMSK                                                        0x2
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_ROOT_EN_SHFT                                                        0x1
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_UPDATE_BMSK                                                         0x1
#define HWIO_GCC_MMNOC_SF_QX_CMD_RCGR_UPDATE_SHFT                                                         0x0

#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0000916c)
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_RMSK                                                           0x11071f
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_ADDR, HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_RMSK)
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_ADDR, m)
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_ADDR,v)
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_ADDR,m,v,HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_IN)
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_HW_CLK_CONTROL_BMSK                                            0x100000
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_HW_CLK_CONTROL_SHFT                                                0x14
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_RCGLITE_DISABLE_BMSK                                            0x10000
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_RCGLITE_DISABLE_SHFT                                               0x10
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_SRC_SEL_BMSK                                                      0x700
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_SRC_SEL_SHFT                                                        0x8
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_SRC_DIV_BMSK                                                       0x1f
#define HWIO_GCC_MMNOC_SF_QX_CFG_RCGR_SRC_DIV_SHFT                                                        0x0

#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00009294)
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_RMSK                                                           0x1
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_ADDR, HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_RMSK)
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_ADDR, m)
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_ADDR,v)
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_ADDR,m,v,HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_IN)
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_DCD_ENABLE_BMSK                                                0x1
#define HWIO_GCC_MMNOC_SF_QX_DCD_CDIV_DCDR_DCD_ENABLE_SHFT                                                0x0

#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00009298)
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_RMSK                                                     0x80000005
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR, HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_RMSK)
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR, m)
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR,v)
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_ADDR,m,v,HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_IN)
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_OFF_BMSK                                             0x80000000
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_OFF_SHFT                                                   0x1f
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_ARES_BMSK                                                   0x4
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_ARES_SHFT                                                   0x2
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_ENABLE_BMSK                                                 0x1
#define HWIO_GCC_MMNOC_QOSGEN_EXTREF_CBCR_CLK_ENABLE_SHFT                                                 0x0

#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00044004)
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_RMSK                                                       0x80000005
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR, HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_RMSK)
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR, m)
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR,v)
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_ADDR,m,v,HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_IN)
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_ARES_BMSK                                                     0x4
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_ARES_SHFT                                                     0x2
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_DDRSS_MMNOC_SF_QX_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x00044008)
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_RMSK                                                       0x80000005
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_IN          \
        in_dword_masked(HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR, HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_RMSK)
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR, m)
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR,v)
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_ADDR,m,v,HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_IN)
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_ARES_BMSK                                                     0x4
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_ARES_SHFT                                                     0x2
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_DDRSS_MMNOC_HF_QX_CBCR_CLK_ENABLE_SHFT                                                   0x0

#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00062030)
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_RMSK                                                                 0x1
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_IN          \
        in_dword_masked(HWIO_GCC_GDS_HW_CTRL_SW_OVRD_ADDR, HWIO_GCC_GDS_HW_CTRL_SW_OVRD_RMSK)
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_INM(m)      \
        in_dword_masked(HWIO_GCC_GDS_HW_CTRL_SW_OVRD_ADDR, m)
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_OUT(v)      \
        out_dword(HWIO_GCC_GDS_HW_CTRL_SW_OVRD_ADDR,v)
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GDS_HW_CTRL_SW_OVRD_ADDR,m,v,HWIO_GCC_GDS_HW_CTRL_SW_OVRD_IN)
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_DISABLE_BMSK                                                         0x1
#define HWIO_GCC_GDS_HW_CTRL_SW_OVRD_DISABLE_SHFT                                                         0x0

#define HWIO_GCC_MMNOC_GDSC_OVRD_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00078004)
#define HWIO_GCC_MMNOC_GDSC_OVRD_RMSK                                                                     0x3
#define HWIO_GCC_MMNOC_GDSC_OVRD_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDSC_OVRD_ADDR, HWIO_GCC_MMNOC_GDSC_OVRD_RMSK)
#define HWIO_GCC_MMNOC_GDSC_OVRD_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDSC_OVRD_ADDR, m)
#define HWIO_GCC_MMNOC_GDSC_OVRD_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDSC_OVRD_ADDR,v)
#define HWIO_GCC_MMNOC_GDSC_OVRD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDSC_OVRD_ADDR,m,v,HWIO_GCC_MMNOC_GDSC_OVRD_IN)
#define HWIO_GCC_MMNOC_GDSC_OVRD_SW_OVERRIDE_BMSK                                                         0x2
#define HWIO_GCC_MMNOC_GDSC_OVRD_SW_OVERRIDE_SHFT                                                         0x1
#define HWIO_GCC_MMNOC_GDSC_OVRD_RETAIN_FF_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_MMNOC_GDSC_OVRD_RETAIN_FF_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b014)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                                 0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                         0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                               0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                             0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                             0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b018)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                                0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b01c)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                                0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b040)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                                0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b044)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                                 0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                          0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                                0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                            0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                            0x0

#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007b048)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                                0x80000001
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_TZ_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c014)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                                0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c018)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                               0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c01c)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                               0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c040)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                               0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                        0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                              0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                          0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                          0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c044)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                                0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0007c048)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                               0x80000001
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                        0x80000000
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                              0x1f
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                          0x1
#define HWIO_GCC_HYP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                          0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d014)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                              0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                      0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                            0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                          0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                          0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d018)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                             0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d01c)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                             0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d040)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                             0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                      0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                            0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                        0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                        0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d044)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                              0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                       0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                             0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                         0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                         0x0

#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007d048)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                             0x80000001
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                      0x80000000
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                            0x1f
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                        0x1
#define HWIO_GCC_HLOS1_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                        0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e014)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                              0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                      0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                            0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                          0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                          0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e018)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                             0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e01c)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                             0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                     0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                           0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                         0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                         0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e040)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                             0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                      0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                            0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                        0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                        0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e044)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                              0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                       0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                             0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                         0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                         0x0

#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0007e048)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                             0x80000001
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                      0x80000000
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                            0x1f
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                        0x1
#define HWIO_GCC_HLOS2_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                        0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x00082060)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_RMSK                                                            0xffffdfff
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDS_HW_CTRL_ADDR,v)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDS_HW_CTRL_ADDR,m,v,HWIO_GCC_MMNOC_GDS_HW_CTRL_IN)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_PWR_ON_STATUS_BMSK                                              0x80000000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_PWR_ON_STATUS_SHFT                                                    0x1f
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_RESERVE_30_15_BMSK                                              0x7fff8000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_RESERVE_30_15_SHFT                                                     0xf
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_COLLAPSE_OUT_BMSK                                                   0x4000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_COLLAPSE_OUT_SHFT                                                      0xe
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_HALT_ACK_TIMEOUT_BMSK                                               0x1fe0
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_HALT_ACK_TIMEOUT_SHFT                                                  0x5
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_GDS_HW_STATE_BMSK                                                     0x1e
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_GDS_HW_STATE_SHFT                                                      0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SW_OVERRIDE_BMSK                                                       0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SW_OVERRIDE_SHFT                                                       0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_ADDR                                                     (GCC_CLK_CTL_REG_REG_BASE      + 0x00082064)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RMSK                                                     0xffffffff
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_ADDR,v)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_ADDR,m,v,HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_IN)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_23_31_BMSK                                       0xff800000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_23_31_SHFT                                             0x17
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_DVM_HALT1_PWR_DOWN_ACK_STATUS_BMSK                         0x700000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_DVM_HALT1_PWR_DOWN_ACK_STATUS_SHFT                             0x14
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_19_BMSK                                             0x80000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_19_SHFT                                                0x13
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_DVM_HALT1_PWR_UP_ACK_STATUS_BMSK                            0x70000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_DVM_HALT1_PWR_UP_ACK_STATUS_SHFT                               0x10
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_15_BMSK                                              0x8000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_15_SHFT                                                 0xf
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT1_PWR_DOWN_ACK_STATUS_BMSK                               0x7000
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT1_PWR_DOWN_ACK_STATUS_SHFT                                  0xc
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_11_BMSK                                               0x800
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_11_SHFT                                                 0xb
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT1_PWR_UP_ACK_STATUS_BMSK                                  0x700
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT1_PWR_UP_ACK_STATUS_SHFT                                    0x8
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_7_5_BMSK                                               0xe0
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_7_5_SHFT                                                0x5
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT2_PWR_DOWN_ACK_STATUS_BMSK                                 0x10
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT2_PWR_DOWN_ACK_STATUS_SHFT                                  0x4
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_3_1_BMSK                                                0xe
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_RESERVE_3_1_SHFT                                                0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT2_PWR_UP_ACK_STATUS_BMSK                                    0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_STATUS_HALT2_PWR_UP_ACK_STATUS_SHFT                                    0x0

#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00182064)
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_RMSK                                                         0x7f
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_ADDR, HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_RMSK)
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_ADDR, m)
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_ADDR,v)
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_ADDR,m,v,HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_IN)
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_DVM_HALT_REQ_BMSK                                            0x70
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_DVM_HALT_REQ_SHFT                                             0x4
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_MMU_TBU_HALT_REQ_BMSK                                         0xe
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_MMU_TBU_HALT_REQ_SHFT                                         0x1
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_NOC_HALT_REQ_BMSK                                             0x1
#define HWIO_GCC_MMNOC_HALT_REQ_GDS_HW_CTRL_NOC_HALT_REQ_SHFT                                             0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00182068)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_RMSK                                                        0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_STATUS_BMSK                                                 0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_STATUS_STATUS_SHFT                                                 0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0018206c)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_RMSK                                                          0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_ADDR,v)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_ADDR,m,v,HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_IN)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_MASK_BMSK                                                     0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_MASK_MASK_SHFT                                                     0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x00182070)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_RMSK                                                         0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_ADDR,v)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_ADDR,m,v,HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_IN)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_CLEAR_BMSK                                                   0x1
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_IRQ_CLEAR_CLEAR_SHFT                                                   0x0

#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00182074)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_RMSK                                                            0xff
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_IN          \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_ADDR, HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_RMSK)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_INM(m)      \
        in_dword_masked(HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_ADDR, m)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_OUT(v)      \
        out_dword(HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_ADDR,v)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_ADDR,m,v,HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_IN)
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_SPARE_BMSK                                                      0xff
#define HWIO_GCC_MMNOC_GDS_HW_CTRL_SPARE_SPARE_SHFT                                                       0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00086014)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                                 0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                         0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                               0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                             0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                             0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00086018)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                                0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0008601c)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                                0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00086040)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                                0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                                 (GCC_CLK_CTL_REG_REG_BASE      + 0x00086044)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                                 0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                          0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                                0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                            0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                            0x0

#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00086048)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                                0x80000001
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_SP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00087014)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                                0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                        0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                              0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                            0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                            0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00087018)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                               0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0008701c)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                               0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                       0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                             0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                           0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                           0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00087040)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                               0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                        0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                              0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                          0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                          0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00087044)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                                0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                         0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                               0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                           0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                           0x0

#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00087048)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                               0x80000001
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                        0x80000000
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                              0x1f
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                          0x1
#define HWIO_GCC_MSS_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                          0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00019014)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK                                         0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_BMSK                                 0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_OFF_SHFT                                       0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_BMSK                                     0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_CLK_CLK_ENABLE_SHFT                                     0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00019018)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK                                        0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_BMSK                                0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_OFF_SHFT                                      0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_BMSK                                    0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_CLK_CLK_ENABLE_SHFT                                    0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0001901c)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK                                        0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_BMSK                                0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_OFF_SHFT                                      0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_BMSK                                    0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_CLK_CLK_ENABLE_SHFT                                    0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00019040)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK                                        0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_BMSK                                 0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_PWR_ON_SHFT                                       0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_BMSK                                   0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF0_GDS_SW_COLLAPSE_SHFT                                   0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00019044)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK                                         0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_BMSK                                  0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_PWR_ON_SHFT                                        0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_BMSK                                    0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_SF_GDS_SW_COLLAPSE_SHFT                                    0x0

#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x00019048)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK                                        0x80000001
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN          \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_RMSK)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_INM(m)      \
        in_dword_masked(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR, m)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUT(v)      \
        out_dword(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,v)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_ADDR,m,v,HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_IN)
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_BMSK                                 0x80000000
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_PWR_ON_SHFT                                       0x1f
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_BMSK                                   0x1
#define HWIO_GCC_TURING_DSP_VOTE_MMNOC_MMU_TBU_HF1_GDS_SW_COLLAPSE_SHFT                                   0x0

#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095004)
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF0_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095008)
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF1_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0009500c)
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF2_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095010)
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF3_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095014)
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF4_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095018)
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF5_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0009501c)
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF6_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095020)
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF7_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095024)
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF8_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00095028)
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_RMSK                                                          0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL5_BMSK                                                    0x10
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL5_SHFT                                                     0x4
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL4_BMSK                                                     0x8
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL4_SHFT                                                     0x3
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL2_3_BMSK                                                   0x4
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL2_3_SHFT                                                   0x2
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_RPMH_MMNOC_PERF9_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0009502c)
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF10_ENA_VOTE_GPLL0_SHFT                                                    0x0

#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00095030)
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF11_ENA_VOTE_GPLL0_SHFT                                                    0x0

#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00095034)
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF12_ENA_VOTE_GPLL0_SHFT                                                    0x0

#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00095038)
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF13_ENA_VOTE_GPLL0_SHFT                                                    0x0

#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0009503c)
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF14_ENA_VOTE_GPLL0_SHFT                                                    0x0

#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x00095040)
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_RMSK                                                         0x1f
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_ADDR, HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_RMSK)
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_ADDR, m)
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_ADDR,v)
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_ADDR,m,v,HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_IN)
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL5_BMSK                                                   0x10
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL5_SHFT                                                    0x4
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL4_BMSK                                                    0x8
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL4_SHFT                                                    0x3
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL2_3_BMSK                                                  0x4
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL2_3_SHFT                                                  0x2
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL1_BMSK                                                    0x2
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL1_SHFT                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL0_BMSK                                                    0x1
#define HWIO_GCC_RPMH_MMNOC_PERF15_ENA_VOTE_GPLL0_SHFT                                                    0x0


#endif /* __BCM_WA_HWIO_H__ */
